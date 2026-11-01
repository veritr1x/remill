/*
 * Copyright (c) 2026 Trail of Bits, Inc.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
namespace {
#define LOAD_LANE(size) \
  DEF_SEM(LoadLane##size, V128W dst, I32 index, M##size src) { \
    auto vec = UReadV##size(dst); \
    vec = UInsertV##size(vec, Read(index), Read(src)); \
    UWriteV##size(dst, vec); \
    return memory; \
  } \
  DEF_SEM(LoadLane##size##Post, V128W dst, I32 index, M##size src, R64W base, ADDR next) { \
    memory = LoadLane##size(memory, state, dst, index, src); \
    Write(base, Read(next)); \
    return memory; \
  } \
  template <size_t count> \
  DEF_SEM(Replicate##size, V128W dst, M##size src) { \
    auto value = Read(src); \
    auto vec = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < count; ++i) vec = UInsertV##size(vec, i, value); \
    UWriteV##size(dst, vec); \
    return memory; \
  } \
  template <size_t count> \
  DEF_SEM(Replicate##size##Post, V128W dst, M##size src, R64W base, ADDR next) { \
    memory = Replicate##size<count>(memory, state, dst, src); \
    Write(base, Read(next)); \
    return memory; \
  }
LOAD_LANE(8)
LOAD_LANE(16)
LOAD_LANE(32)
LOAD_LANE(64)
#undef LOAD_LANE
}  // namespace

#define LANE_ISEL(size, plain, imm, reg) \
  DEF_ISEL(LD1_ASISDLSO_##plain) = LoadLane##size; \
  DEF_ISEL(LD1_ASISDLSOP_##imm) = LoadLane##size##Post; \
  DEF_ISEL(LD1_ASISDLSOP_##reg) = LoadLane##size##Post;
LANE_ISEL(8, B1_1B, B1_I1B, BX1_R1B)
LANE_ISEL(16, H1_1H, H1_I1H, HX1_R1H)
LANE_ISEL(32, S1_1S, S1_I1S, SX1_R1S)
LANE_ISEL(64, D1_1D, D1_I1D, DX1_R1D)
#undef LANE_ISEL

#define REPLICATE_ISEL(arr, size, count) \
  DEF_ISEL(LD1R_ASISDLSO_R1_##arr) = Replicate##size<count>; \
  DEF_ISEL(LD1R_ASISDLSOP_R1_I_##arr) = Replicate##size##Post<count>; \
  DEF_ISEL(LD1R_ASISDLSOP_RX1_R_##arr) = Replicate##size##Post<count>;
REPLICATE_ISEL(8B, 8, 8)
REPLICATE_ISEL(16B, 8, 16)
REPLICATE_ISEL(4H, 16, 4)
REPLICATE_ISEL(8H, 16, 8)
REPLICATE_ISEL(2S, 32, 2)
REPLICATE_ISEL(4S, 32, 4)
REPLICATE_ISEL(1D, 64, 1)
REPLICATE_ISEL(2D, 64, 2)
#undef REPLICATE_ISEL
