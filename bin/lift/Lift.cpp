/*
 * Copyright (c) 2019 Trail of Bits, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "GuestCallLowering.h"
#include <llvm/Support/Error.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <remill/Arch/Arch.h>
#include <remill/Arch/Instruction.h>
#include <remill/Arch/Name.h>
#include <remill/BC/ABI.h>
#include <remill/BC/IntrinsicTable.h>
#include <remill/BC/Lifter.h>
#include <remill/BC/Optimizer.h>
#include <remill/BC/Util.h>
#include <remill/BC/Version.h>
#include <remill/OS/OS.h>
#include <remill/Version/Version.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>
#include <unordered_set>

DEFINE_string(os, REMILL_OS,
              "Operating system name of the code being "
              "translated. Valid OSes: linux, macos, windows, solaris.");
DEFINE_string(arch, "",
              "Architecture of the code being translated. "
              "Valid architectures: x86, amd64 (with or without "
              "`_avx` or `_avx512` appended), aarch64, aarch32");

DEFINE_uint64(address, -1,
              "Address at which we should assume the bytes are "
              "located in virtual memory.");

DEFINE_uint64(entry_address, -1,
              "Address of instruction that should be "
              "considered the entrypoint of this code. "
              "Defaults to the value of -address.");

DEFINE_string(bytes, "", "Hex-encoded byte string to lift.");
DEFINE_string(bytes_file, "", "Raw binary bytes to lift, mapped at -address. Mutually exclusive with -bytes.");
DEFINE_string(entry_addresses_file, "", "Whitespace-separated trace entry addresses (decimal or 0x-prefixed).");
DEFINE_bool(lift_calls, false, "Lift direct callees contained in the input bytes instead of leaving them external.");
DEFINE_string(trace_list_out, "", "Write the addresses of all lifted traces, one per line.");
DEFINE_string(semantics_dir, "", "Use semantics bitcode from this directory; an explicit path must contain the requested architecture.");
DEFINE_string(trace_heads_file, "", "Known trace boundaries; direct jumps to these addresses stay external instead of inlining across function boundaries.");

DEFINE_bool(guest_call_transfer, false, "Lower guest calls before optimization for the explicit-transfer Runtime ABI.");
DEFINE_string(guest_call_transfer_stats_out, "", "Write optional pre-inliner guest-call lowering counters; requires guest_call_transfer.");

DEFINE_string(
    ir_pre_out, "",
    "Path to the file where the LLVM IR (before optimization) should be saved");

DEFINE_string(ir_out, "", "Path to file where the LLVM IR should be saved.");
DEFINE_string(bc_out, "",
              "Path to file where the LLVM bitcode should be "
              "saved.");

DEFINE_string(signature, "", "Function signature \"reg_out(reg_in,...)\"");
DEFINE_bool(mute_state_escape, false, "Mute state escape");
DEFINE_bool(symbolic_regs, false, "Set registers to a symbolic value");

struct Memory {
  uint64_t base;
  std::vector<uint8_t> bytes;
};

static std::vector<uint64_t> ReadAddresses(const std::string &path) {
  std::ifstream input(path);
  if (!input) throw std::invalid_argument("Could not open address file " + path);
  std::vector<uint64_t> addresses;
  std::string token;
  while (input >> token) {
    size_t used = 0;
    const int base = token.size() > 2 && token[0] == '0' &&
        (token[1] == 'x' || token[1] == 'X') ? 16 : 10;
    auto address = std::stoull(token, &used, base);
    if (used != token.size() || token[0] == '-')
      throw std::invalid_argument("Invalid address token " + token);
    addresses.push_back(address);
  }
  if (input.bad() || addresses.empty())
    throw std::invalid_argument("Address file is empty or unreadable: " + path);
  return addresses;
}

// Read a binary image or decode -bytes, mapping the first byte at -address.
static Memory ReadInputBytes(uint64_t addr_mask) {
  Memory memory{FLAGS_address, {}};

  if (!FLAGS_bytes_file.empty()) {
    std::ifstream input(FLAGS_bytes_file, std::ios::binary);
    if (!input) {
      std::cerr << "Could not open input file " << FLAGS_bytes_file << std::endl;
      exit(EXIT_FAILURE);
    }
    memory.bytes.assign(std::istreambuf_iterator<char>(input), {});
    if (input.bad() || memory.bytes.empty()) {
      std::cerr << "Could not read nonempty input file " << FLAGS_bytes_file << std::endl;
      exit(EXIT_FAILURE);
    }
  } else {
    memory.bytes.reserve(FLAGS_bytes.size() / 2);

    for (size_t i = 0; i < FLAGS_bytes.size(); i += 2) {
      char nibbles[] = {FLAGS_bytes[i], FLAGS_bytes[i + 1], '\0'};
      if (!std::isxdigit(static_cast<unsigned char>(nibbles[0])) ||
          !std::isxdigit(static_cast<unsigned char>(nibbles[1]))) {
        std::cerr << "Invalid hex byte value '" << nibbles
                  << "' specified in -bytes." << std::endl;
        exit(EXIT_FAILURE);
      }
      memory.bytes.push_back(static_cast<uint8_t>(strtol(nibbles, nullptr, 16)));
    }
  }

  if (memory.bytes.size() - 1 > addr_mask - memory.base) {
    std::cerr << "Input bytes overflow the architecture's address space." << std::endl;
    exit(EXIT_FAILURE);
  }

  return memory;
}

struct SimpleTraceManager : remill::TraceManager {
  const remill::Arch *arch = nullptr;
  llvm::Module *module = nullptr;
  Memory &memory;
  uint64_t entry = 0;
  std::unordered_map<uint64_t, llvm::Function *> traces;
  std::unordered_set<uint64_t> trace_heads;

  SimpleTraceManager(const remill::Arch *arch, llvm::Module *module,
                     Memory &memory, uint64_t entry,
                     const std::vector<uint64_t> &known_heads)
      : arch(arch),
        module(module),
        memory(memory),
        entry(entry), trace_heads(known_heads.begin(), known_heads.end()) {}

  // Called when we have lifted, i.e. defined the contents, of a new trace.
  // The derived class is expected to do something useful with this.
  void SetLiftedTraceDefinition(uint64_t addr,
                                llvm::Function *lifted_func) override {
    traces[addr] = lifted_func;
  }

  // Get a definition for a lifted trace.
  //
  // NOTE: This is permitted to return a function from an arbitrary module.
  llvm::Function *GetLiftedTraceDefinition(uint64_t addr) override {

    if (auto found = traces.find(addr); found != traces.end()) {
      return found->second;
    }

    // The entry function needs to be lifted by the TraceLifter
    if (addr == entry || (FLAGS_lift_calls && TryReadExecutableByte(addr, nullptr))) {
      return nullptr;
    }

    // The get_trace_decl in TraceLifter creates a declaration for us.
    // Instead of providing an implementation, we keep it extern.
    auto name = TraceName(addr);
    auto fn = module->getFunction(name);
    if (fn == nullptr) {
      fn = arch->DeclareLiftedFunction(name, module);
    }
    return fn;
  }

  // Get a declaration for a lifted trace. The idea here is that a derived
  // class might have additional global info available to them that lets
  // them declare traces ahead of time. In order to distinguish between
  // stuff we've lifted, and stuff we haven't lifted, we allow the lifter
  // to access "defined" vs. "declared" traces.
  //
  // NOTE: This is permitted to return a function from an arbitrary module.
  llvm::Function *GetLiftedTraceDeclaration(uint64_t addr) override {
    if (trace_heads.count(addr)) {
      auto name = TraceName(addr);
      auto function = module->getFunction(name);
      return function ? function : arch->DeclareLiftedFunction(name, module);
    }
    return remill::TraceManager::GetLiftedTraceDeclaration(addr);
  }

  // Try to read an executable byte of memory. Returns `true` of the byte
  // at address `addr` is executable and readable, and updates the byte
  // pointed to by `byte` with the read value.
  bool TryReadExecutableByte(uint64_t addr, uint8_t *byte) override {
    if (addr >= memory.base && addr - memory.base < memory.bytes.size()) {
      if (byte != nullptr) {
        *byte = memory.bytes[addr - memory.base];
      }
      return true;
    } else {
      return false;
    }
  }
};

// Looks for calls to a function like `__remill_function_return`, and
// replace its state pointer with a null pointer so that the state
// pointer never escapes.
static void MuteStateEscape(llvm::Module *module, const char *func_name) {
  auto func = module->getFunction(func_name);
  if (!func) {
    return;
  }

  for (auto user : func->users()) {
    if (auto call_inst = llvm::dyn_cast<llvm::CallInst>(user)) {
      auto arg_op = call_inst->getArgOperand(remill::kStatePointerArgNum);
      call_inst->setArgOperand(remill::kStatePointerArgNum,
                               llvm::UndefValue::get(arg_op->getType()));
    }
  }
}

static void SetVersion(void) {
  std::stringstream ss;
  auto vs = remill::version::GetVersionString();
  if (0 == vs.size()) {
    vs = "unknown";
  }
  ss << vs << "\n";
  if (!remill::version::HasVersionData()) {
    ss << "No extended version information found!\n";
  } else {
    ss << "Commit Hash: " << remill::version::GetCommitHash() << "\n";
    ss << "Commit Date: " << remill::version::GetCommitDate() << "\n";
    ss << "Last commit by: " << remill::version::GetAuthorName() << " ["
       << remill::version::GetAuthorEmail() << "]\n";
    ss << "Commit Subject: [" << remill::version::GetCommitSubject() << "]\n";
    ss << "\n";
    if (remill::version::HasUncommittedChanges()) {
      ss << "Uncommitted changes were present during build.\n";
    } else {
      ss << "All changes were committed prior to building.\n";
    }
  }
  google::SetVersionString(ss.str());
}

struct Argument {
  bool is_memory = false;
  size_t size = 0;
  std::string reg;
  int64_t offset = 0;

  static int64_t parse_hex(const std::string &argument) {
    int64_t hex_value = 0;
    std::istringstream iss(argument);
    iss >> std::hex >> hex_value;
    return hex_value;
  }

  static Argument parse(const std::string &argument) {
    Argument out;
    auto mem_idx = argument.find('[');
    if (mem_idx != std::string::npos) {
      out.is_memory = true;
      if (mem_idx > 0) {
        out.size = parse_hex(argument.substr(0, mem_idx));
      } else {
        out.size = 0;
      }
      auto sign_idx = argument.find_first_of("+-");
      if (sign_idx == std::string::npos) {
        out.reg = argument.substr(mem_idx + 1, argument.size() - mem_idx - 2);
        out.offset = 0;
      } else {
        out.reg = argument.substr(mem_idx + 1, sign_idx - mem_idx - 1);
        out.offset = parse_hex(
            argument.substr(sign_idx, argument.size() - sign_idx - 1));
      }
    } else {
      out.reg = argument;
    }
    for (auto &ch : out.reg) {
      if (ch >= 'a' && ch <= 'z') {
        ch -= 'a' - 'A';
      }
    }
    return out;
  }

  void dump() {
    if (is_memory) {
      if (offset < 0) {
        printf("%zu:['%s'%ld]\n", size, reg.c_str(), offset);
      } else {
        printf("%zu:['%s'+%ld]\n", size, reg.c_str(), offset);
      }
    } else {
      printf("%s\n", reg.c_str());
    }
  }
};

int main(int argc, char *argv[]) {
  SetVersion();
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);


  if (FLAGS_bytes.empty() == FLAGS_bytes_file.empty()) {
    std::cerr << "Specify exactly one of -bytes and -bytes_file."
              << std::endl;
    return EXIT_FAILURE;
  } else if (FLAGS_bytes.size() % 2) {
    std::cerr << "Please specify an even number of nibbles to -bytes."
              << std::endl;
    return EXIT_FAILURE;
  }

  if (FLAGS_arch.empty()) {
    std::cerr
        << "No architecture specified. Valid architectures: x86, amd64 (with or without "
           "`_avx` or `_avx512` appended), aarch64, aarch32"
        << std::endl;
    return EXIT_FAILURE;
  }

  if (FLAGS_address == (uint64_t) -1) {
    FLAGS_address = 0;
  }

  if (FLAGS_entry_address == (uint64_t) -1) {
    FLAGS_entry_address = FLAGS_address;
  }

  std::vector<uint64_t> entries;
  std::vector<uint64_t> trace_heads;
  if (!FLAGS_entry_addresses_file.empty()) {
    try { entries = ReadAddresses(FLAGS_entry_addresses_file); }
    catch (const std::exception &error) {
      std::cerr << error.what() << std::endl;
      return EXIT_FAILURE;
    }
    FLAGS_entry_address = entries.front();
  } else {
    entries.push_back(FLAGS_entry_address);
  }
  if (!FLAGS_trace_heads_file.empty()) {
    try { trace_heads = ReadAddresses(FLAGS_trace_heads_file); }
    catch (const std::exception &error) {
      std::cerr << error.what() << std::endl;
      return EXIT_FAILURE;
    }
  }

  // Make sure `-address` and `-entry_address` are in-bounds for the target
  // architecture's address size.
  llvm::LLVMContext context;
  auto arch = remill::Arch::Get(
      context, FLAGS_os,
      FLAGS_arch);  // TODO: what happens with invalid arguments?
  const uint64_t addr_mask = ~0ULL >> (64UL - arch->address_size);
  if (FLAGS_address != (FLAGS_address & addr_mask)) {
    std::cerr << "Value " << std::hex << FLAGS_address
              << " passed to -address does not fit into 32-bits. Did mean"
              << " to specify a 64-bit architecture to -arch?" << std::endl;
    return EXIT_FAILURE;
  }

  for (auto entry : entries) {
    if (entry != (entry & addr_mask)) {
      std::cerr << "Value " << std::hex << entry
                << " passed to -entry_address does not fit into the architecture's address size."
                << std::endl;
      return EXIT_FAILURE;
    }
  }
  for (auto head : trace_heads) {
    if (head != (head & addr_mask)) {
      std::cerr << "Trace head does not fit into the architecture's address size." << std::endl;
      return EXIT_FAILURE;
    }
  }

  std::vector<std::filesystem::path> semantics_dirs;
  if (!FLAGS_semantics_dir.empty()) {
    semantics_dirs.emplace_back(FLAGS_semantics_dir);
    if (!remill::FindSemanticsBitcodeFile(remill::GetArchName(arch->arch_name),
                                        semantics_dirs, false)) {
      std::cerr << "No requested architecture semantics in " << FLAGS_semantics_dir << std::endl;
      return EXIT_FAILURE;
    }
  }
  std::unique_ptr<llvm::Module> module(remill::LoadArchSemantics(arch.get(), semantics_dirs));

  const auto mem_ptr_type = arch->MemoryPointerType();

  Memory memory = ReadInputBytes(addr_mask);
  SimpleTraceManager manager(arch.get(), module.get(), memory,
                             FLAGS_entry_address, trace_heads);
  for (auto entry : entries) {
    if (!manager.TryReadExecutableByte(entry, nullptr)) {
      std::cerr << "No executable code at address 0x" << std::hex
                << entry << std::endl;
      return EXIT_FAILURE;
    }
  }
  remill::IntrinsicTable intrinsics(module.get());


  auto inst_lifter = arch->DefaultLifter(intrinsics);

  remill::TraceLifter trace_lifter(arch.get(), manager);

  // Lift all discoverable traces starting from `-entry_address` into
  // `module`.
  for (auto entry : entries) {
    manager.entry = entry;
    if (!trace_lifter.Lift(entry)) {
      std::cerr << "Could not lift entry address 0x" << std::hex << entry << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (!FLAGS_trace_list_out.empty()) {
    std::ofstream output(FLAGS_trace_list_out);
    std::vector<uint64_t> addresses;
    for (const auto &trace : manager.traces) addresses.push_back(trace.first);
    std::sort(addresses.begin(), addresses.end());
    for (auto addr : addresses) output << "0x" << std::hex << addr << '\n';
    if (!output) {
      std::cerr << "Could not write trace list " << FLAGS_trace_list_out << std::endl;
      return EXIT_FAILURE;
    }
  }

  // Remove llvm.compiler.used to not preserve unused semantics
  auto compilerUsed = module->getGlobalVariable("llvm.compiler.used", true);
  if (compilerUsed != nullptr) {
    compilerUsed->eraseFromParent();
  }

  // Remove ISEL_ globals that contain pointers to the semantic functions
  std::vector<llvm::GlobalVariable *> erase;
  for (auto &G : module->globals()) {
    if (G.getName().find("ISEL_") == 0) {
      erase.push_back(&G);
    }
  }
  for (auto G : erase) {
    G->eraseFromParent();
  }

  // Remove function that keeps the references to unused intrinsics
  auto remillIntrinsics = module->getFunction("__remill_intrinsics");
  if (remillIntrinsics != nullptr) {
    remillIntrinsics->eraseFromParent();
  }

  // Remove the implementation of the __remill_sync_hyper_call from the bitcode, because
  // after inlining things get very confusing if this is actually called.
  // TODO: this should probably be removed
  auto hyperCall = module->getFunction("__remill_sync_hyper_call");
  if (hyperCall != nullptr) {
    auto name = hyperCall->getName();
    auto ty = hyperCall->getFunctionType();
    auto newFn = module->getOrInsertFunction(name.str() + "_", ty);
    hyperCall->replaceAllUsesWith(newFn.getCallee());
    hyperCall->eraseFromParent();
    newFn.getCallee()->setName(name);
  }

  // A lot of intrinsic functions are (incorrectly) marked as [[gnu::const]].
  // This causes problems where optimizer's assumptions are violated when an
  // implementation is provided. To work around this we remove these attributes
  // from the functions and from the call sites.
  // Another workaround is to first do a separate inline pass and then O3.
  // NOTE: This was fixed in https://github.com/lifting-bits/remill/commit/7f091d42
  for (auto &function : module->functions()) {
    if (function.getName().find("__remill_") != 0) {
      continue;
    }

    function.removeFnAttr(llvm::Attribute::ReadNone);
    for (auto &argument : function.args()) {
      argument.removeAttr(llvm::Attribute::ReadNone);
    }
    for (auto user : function.users()) {
      if (auto call = llvm::dyn_cast<llvm::CallInst>(user)) {
        call->removeFnAttr(llvm::Attribute::ReadNone);
      }
    }
  }

  // Dump the pre-optimization IR
  if (!FLAGS_ir_pre_out.empty()) {
    if (!remill::StoreModuleIRToFile(module.get(), FLAGS_ir_pre_out, true)) {
      LOG(ERROR) << "Could not save LLVM IR to " << FLAGS_ir_pre_out;
    }
  }

  // Optimize the module, but with a particular focus on only the functions
  // that we actually lifted.
  remill::OptimizationGuide guide = {};
  if (!FLAGS_guest_call_transfer && !FLAGS_guest_call_transfer_stats_out.empty()) {
    LOG(ERROR) << "guest_call_transfer_stats_out requires guest_call_transfer";
    return EXIT_FAILURE;
  }
  if (FLAGS_guest_call_transfer) {
    bunker::guest_transfer::LoweringStats stats;
    if (auto error = bunker::guest_transfer::LowerGuestCalls(*module, stats)) {
      LOG(ERROR) << "Guest call lowering rejected input: " << llvm::toString(std::move(error));
      return EXIT_FAILURE;
    }
    if (!FLAGS_guest_call_transfer_stats_out.empty()) {
      std::ofstream output(FLAGS_guest_call_transfer_stats_out);
      output << "{\n  \"passed\":true,\n  \"State_bytes\":1200,\n"
             << "  \"direct_non_tail\":" << stats.direct_non_tail << ",\n"
             << "  \"dynamic_non_tail\":" << stats.dynamic_non_tail << ",\n"
             << "  \"direct_tail_transfers\":" << stats.direct_tail_transfers << ",\n"
             << "  \"jump_terminals\":" << stats.jump_terminals << ",\n"
             << "  \"return_terminals\":" << stats.return_terminals << ",\n"
             << "  \"transformed_calls\":" << stats.transformed_calls << ",\n"
             << "  \"SP_offset\":" << stats.SP_offset << ",\n"
             << "  \"PC_offset\":" << stats.PC_offset << ",\n"
             << "  \"metadata_records\":" << stats.metadata_records << ",\n"
             << "  \"inverse_verified\":" << (stats.inverse_verified ? "true" : "false") << "\n}\n";
      if (!output) { LOG(ERROR) << "Could not save guest-call lowering stats"; return EXIT_FAILURE; }
    }
  }
  remill::OptimizeModule(arch, module, manager.traces, guide);

  // Create a new module in which we will move all the lifted functions. Prepare
  // the module for code of this architecture, i.e. set the data layout, triple,
  // etc.
  llvm::Module dest_module("lifted_code", context);
  arch->PrepareModuleDataLayout(&dest_module);

  llvm::Function *entry_trace = nullptr;

  // Move the lifted code into a new module. This module will be much smaller
  // because it won't be bogged down with all of the semantics definitions.
  // This is a good JITing strategy: optimize the lifted code in the semantics
  // module, move it to a new module, instrument it there, then JIT compile it.
  for (auto &lifted_entry : manager.traces) {
    if (lifted_entry.first == FLAGS_entry_address) {
      entry_trace = lifted_entry.second;
    }
    remill::MoveFunctionIntoModule(lifted_entry.second, &dest_module);

    // If we are providing a prototype, then we'll be re-optimizing the new
    // module, and we want everything to get inlined.
    if (!FLAGS_signature.empty()) {
      lifted_entry.second->setLinkage(llvm::GlobalValue::InternalLinkage);
      lifted_entry.second->removeFnAttr(llvm::Attribute::NoInline);
      lifted_entry.second->addFnAttr(llvm::Attribute::InlineHint);
      lifted_entry.second->addFnAttr(llvm::Attribute::AlwaysInline);
    }
  }

  if (FLAGS_guest_call_transfer) {
    if (auto *origins = module->getNamedMetadata("bunker.guest.call.lowering")) {
      auto *copy = dest_module.getOrInsertNamedMetadata("bunker.guest.call.lowering");
      for (auto *row : origins->operands()) copy->addOperand(row);
    }
  }

  // We have a prototype, so go create a function that will call our entrypoint.
  if (!FLAGS_signature.empty()) {
    CHECK_NOTNULL(entry_trace);

    // Set the entry trace as internal so it can be removed during optimizations
    entry_trace->setLinkage(llvm::Function::InternalLinkage);

    std::string signature;
    for (auto ch : FLAGS_signature) {
      if (ch >= 'a' && ch <= 'z') {
        ch -= 'a' - 'A';
      }
      if (ch != ' ') {
        signature.push_back(ch);
      }
    }
    auto paren_idx = signature.find('(');
    CHECK(paren_idx != std::string::npos && signature.back() == ')')
        << "Invalid function signature";

    auto output_reg_name = signature.substr(0, paren_idx);
    if (output_reg_name == "void") {
      output_reg_name.clear();
    }
    std::vector<Argument> input_args;
    std::string temp;
    for (size_t i = paren_idx + 1; i < signature.size() - 1; i++) {
      auto ch = signature[i];
      if (ch == ',') {
        input_args.push_back(Argument::parse(temp));
        temp.clear();
      } else {
        temp.push_back(ch);
      }
    }
    if (!temp.empty()) {
      input_args.push_back(Argument::parse(temp));
    }

    // Use the registers to build a function prototype.
    llvm::SmallVector<llvm::Type *, 8> arg_types;
    for (auto &arg : input_args) {
      const auto input_reg = arch->RegisterByName(arg.reg);
      CHECK(input_reg != nullptr)
          << "Invalid register name '" << arg.reg << "' used in signature '"
          << FLAGS_signature << "'";

      if (arg.size == 0) {
        arg.size = input_reg->size;
      }
      auto arg_type = llvm::Type::getIntNTy(context, arg.size * 8);
      arg_types.push_back(arg_type);
    }

    auto return_type = llvm::Type::getVoidTy(context);
    if (!output_reg_name.empty()) {
      const auto output_reg = arch->RegisterByName(output_reg_name);
      CHECK(output_reg != nullptr)
          << "Invalid register name '" << output_reg_name << "'";
      return_type = output_reg->type;
    }
    const auto func_type =
        llvm::FunctionType::get(return_type, arg_types, false);
    const auto func =
        llvm::Function::Create(func_type, llvm::GlobalValue::ExternalLinkage,
                               "call_" + entry_trace->getName(), &dest_module);

    // HACK: This is a workaround for the issue with the DSEPass making false assumptions
    func->addFnAttr("disable-tail-calls", "true");

    // Get the program counter and stack pointer registers.
    const remill::Register *pc_reg =
        arch->RegisterByName(arch->ProgramCounterRegisterName());
    const remill::Register *sp_reg =
        arch->RegisterByName(arch->StackPointerRegisterName());

    CHECK(pc_reg != nullptr)
        << "Could not find the register in the state structure "
        << "associated with the program counter.";

    CHECK(sp_reg != nullptr)
        << "Could not find the register in the state structure "
        << "associated with the stack pointer.";

    // Store all of the function arguments (corresponding with specific registers)
    // into the stack-allocated `State` structure.
    auto entry = llvm::BasicBlock::Create(context, "", func);
    llvm::IRBuilder<> ir(entry);

    const auto state_type = arch->StateStructType();
    const auto state_ptr = ir.CreateAlloca(state_type);

    auto CreateSymbolicReg = [&](const remill::Register *reg,
                                 const std::string &name) {
      std::string symbol_name = "symbolic_" + name;
      auto symbolic_fn = dest_module.getOrInsertFunction(
          "__remill_" + symbol_name, llvm::FunctionType::get(reg->type, false));
      auto fn = llvm::dyn_cast<llvm::Function>(symbolic_fn.getCallee());

      // Allow the optimizer to delete calls if the result is not used
      fn->setDoesNotAccessMemory();
      fn->setDoesNotThrow();
      fn->addFnAttr(llvm::Attribute::WillReturn);

      auto call = ir.CreateCall(symbolic_fn, {}, symbol_name);
      const auto reg_ptr = reg->AddressOf(state_ptr, entry);
      ir.CreateStore(call, reg_ptr);
    };

    // Store symbolic values into general purpose registers
    if (FLAGS_symbolic_regs) {
      arch->ForEachRegister([&](const remill::Register *reg) {
        if (reg->parent == nullptr) {
          CreateSymbolicReg(reg, reg->name);
        }
      });
    }

    // Store the program counter into the state.
    const auto trace_pc =
        llvm::ConstantInt::get(pc_reg->type, FLAGS_entry_address, false);
    ir.SetInsertPoint(entry);
    ir.CreateStore(trace_pc, pc_reg->AddressOf(state_ptr, entry));

    // Set up symbolic globals
    CreateSymbolicReg(sp_reg, "STACK");
    auto gsbase_reg = arch->RegisterByName("GSBASE");
    if (gsbase_reg != nullptr) {
      CreateSymbolicReg(gsbase_reg, "GSBASE");
    }
    auto fsbase_reg = arch->RegisterByName("FSBASE");
    if (fsbase_reg != nullptr) {
      CreateSymbolicReg(fsbase_reg, "FSBASE");
    }

    llvm::Value *mem_ptr = llvm::UndefValue::get(mem_ptr_type);

    // Store the argument registers into the state
    auto args_it = func->arg_begin();
    for (auto &input_arg : input_args) {
      const auto reg = arch->RegisterByName(input_arg.reg);
      auto reg_ptr = reg->AddressOf(state_ptr, entry);
      auto &arg = *args_it++;

      ir.SetInsertPoint(entry);
      if (input_arg.is_memory) {
        arg.setName("arg_mem_" + input_arg.reg + "_" +
                    llvm::utohexstr(input_arg.offset));
        auto helper_name =
            "__remill_write_memory_" + std::to_string(input_arg.size * 8);
        auto orig_memory_helper = module->getFunction(helper_name);
        CHECK(orig_memory_helper != nullptr)
            << "Could not find memory helper for " << helper_name;
        auto memory_helper = dest_module.getOrInsertFunction(
            helper_name, orig_memory_helper->getFunctionType());
        auto reg_value = ir.CreateLoad(reg->type, reg_ptr);
        auto arg_ptr = ir.CreateAdd(
            reg_value, llvm::ConstantInt::get(reg->type, input_arg.offset));
        ir.CreateCall(memory_helper, {mem_ptr, arg_ptr, &arg});
      } else {
        arg.setName("arg_" + input_arg.reg);
        ir.CreateStore(&arg, reg_ptr);
      }
    }

    // Call the lifted function
    llvm::Value *trace_args[remill::kNumBlockArgs] = {};
    trace_args[remill::kStatePointerArgNum] = state_ptr;
    trace_args[remill::kMemoryPointerArgNum] = mem_ptr;
    trace_args[remill::kPCArgNum] = llvm::ConstantInt::get(
        llvm::IntegerType::get(context, arch->address_size),
        FLAGS_entry_address, false);

    mem_ptr = ir.CreateCall(entry_trace, trace_args);

    // Read and return the output register
    if (!output_reg_name.empty()) {
      const auto out_reg = arch->RegisterByName(output_reg_name);
      auto out_reg_ptr = out_reg->AddressOf(state_ptr, entry);
      ir.CreateRet(ir.CreateLoad(out_reg->type, out_reg_ptr));
    } else {
      ir.CreateRetVoid();
    }

    // NOTE: Doing this prevents the helpers implementation from working properly,
    // which is why this is disabled per default.
    if (FLAGS_mute_state_escape) {
      // We want the stack-allocated `State` to be subject to scalarization
      // and mem2reg, but to "encourage" that, we need to prevent the
      // `alloca`d `State` from escaping.
      MuteStateEscape(&dest_module, "__remill_error");
      MuteStateEscape(&dest_module, "__remill_function_call");
      MuteStateEscape(&dest_module, "__remill_function_return");
      MuteStateEscape(&dest_module, "__remill_jump");
      MuteStateEscape(&dest_module, "__remill_missing_block");
    }

    // Optimize the module to inline everything
    guide.slp_vectorize = true;
    guide.loop_vectorize = true;

    auto check = remill::VerifyModuleMsg(&dest_module);
    if (check) {
      llvm::errs() << "Verification error: " << *check;
      CHECK(false);
    }
    remill::OptimizeBareModule(&dest_module, guide);
  }

  int ret = EXIT_SUCCESS;

  if (!FLAGS_ir_out.empty()) {
    if (!remill::StoreModuleIRToFile(&dest_module, FLAGS_ir_out, true)) {
      LOG(ERROR) << "Could not save LLVM IR to " << FLAGS_ir_out;
      ret = EXIT_FAILURE;
    }
  }
  if (!FLAGS_bc_out.empty()) {
    if (!remill::StoreModuleToFile(&dest_module, FLAGS_bc_out, true)) {
      LOG(ERROR) << "Could not save LLVM bitcode to " << FLAGS_bc_out;
      ret = EXIT_FAILURE;
    }
  }

  return ret;
}
