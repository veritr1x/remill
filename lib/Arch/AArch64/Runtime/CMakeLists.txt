# Copyright (c) 2017 Trail of Bits, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.21)
project(AARCH64_runtime)

set(AARCH64RUNTIME_SOURCEFILES
  Instructions.cpp
  BasicBlock.cpp

  "${REMILL_LIB_DIR}/Arch/Runtime/Intrinsics.cpp"
)

# TODO(llvm/llvm-project#178603): -ffp-exception-behavior=strict fixes LLVM 22
# AArch64 SIMD fcvtzu/fcvtzs misselection but leaves strictfp-attributed
# internal functions un-inlined, crashing the lifter on LLVM 17-18.
# Re-enable once the lifter handles strictfp internal functions or upstream
# fixes the instruction selection regression.
#set_source_files_properties(Instructions.cpp PROPERTIES COMPILE_FLAGS "-O3 -g0 -ffp-exception-behavior=strict")
set_source_files_properties(Instructions.cpp PROPERTIES COMPILE_FLAGS "-O3 -g0")
set_source_files_properties(BasicBlock.cpp PROPERTIES COMPILE_FLAGS "-O0 -g3")

# Instruction semantics are included by Instructions.cpp. Track all of them,
# including newly added instruction groups, when rebuilding its bitcode.
file(GLOB AARCH64_SEMANTICS_DEPENDENCIES CONFIGURE_DEPENDS
  "${REMILL_LIB_DIR}/Arch/AArch64/Semantics/*.cpp"
)

function(add_runtime_helper target_name address_bit_size little_endian)
  message(" > Generating runtime target: ${target_name}")

  add_runtime(${target_name}
    SOURCES ${AARCH64RUNTIME_SOURCEFILES}
    ADDRESS_SIZE ${address_bit_size}
    DEFINITIONS "LITTLE_ENDIAN=${little_endian}"
    INCLUDEDIRECTORIES "${REMILL_INCLUDE_DIR}" "${REMILL_SOURCE_DIR}"
    INSTALLDESTINATION "${REMILL_INSTALL_SEMANTICS_DIR}"
    ARCH aarch64

    DEPENDENCIES
    "${REMILL_INCLUDE_DIR}/remill/Arch/Runtime/Float.h"
    "${REMILL_INCLUDE_DIR}/remill/Arch/Runtime/State.h"
    "${REMILL_INCLUDE_DIR}/remill/Arch/Runtime/Types.h"
    "${REMILL_INCLUDE_DIR}/remill/Arch/Runtime/Operators.h"
    "${REMILL_INCLUDE_DIR}/remill/Arch/Runtime/Intrinsics.h"
    "${REMILL_INCLUDE_DIR}/remill/Arch/Runtime/HyperCall.h"
    "${REMILL_INCLUDE_DIR}/remill/Arch/Runtime/Definitions.h"

    "${REMILL_INCLUDE_DIR}/remill/Arch/AArch64/Runtime/Operators.h"
    "${REMILL_INCLUDE_DIR}/remill/Arch/AArch64/Runtime/State.h"
    "${REMILL_INCLUDE_DIR}/remill/Arch/AArch64/Runtime/Types.h"

    ${AARCH64_SEMANTICS_DEPENDENCIES}
  )
endfunction()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  # add_runtime_helper(aarch64be 64 0)
  add_runtime_helper(aarch64 64 1)
  add_runtime_helper(aarch64_sleigh 64 1)
endif()
