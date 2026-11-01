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
#define STORE_LANE(size) \
  DEF_SEM(StoreLane##size, V128 src, I32 index, M##size##W dst) { \
    const auto vec = UReadV##size(src); \
    const auto value = static_cast<uint##size##_t>(UExtractV##size(vec, Read(index))); \
    Write(dst, value); \
    return memory; \
  } \
  DEF_SEM(StoreLane##size##Post, V128 src, I32 index, M##size##W dst, R64W base, ADDR next) { \
    const auto next_address = Read(next); \
    memory = StoreLane##size(memory, state, src, index, dst); \
    Write(base, next_address); \
    return memory; \
  }
STORE_LANE(8)
STORE_LANE(16)
STORE_LANE(32)
STORE_LANE(64)
#undef STORE_LANE
}  // namespace

#define STORE_LANE_ISEL(size, plain, imm, reg) \
  DEF_ISEL(ST1_ASISDLSO_##plain) = StoreLane##size; \
  DEF_ISEL(ST1_ASISDLSOP_##imm) = StoreLane##size##Post; \
  DEF_ISEL(ST1_ASISDLSOP_##reg) = StoreLane##size##Post;
STORE_LANE_ISEL(8, B1_1B, B1_I1B, BX1_R1B)
STORE_LANE_ISEL(16, H1_1H, H1_I1H, HX1_R1H)
STORE_LANE_ISEL(32, S1_1S, S1_I1S, SX1_R1S)
STORE_LANE_ISEL(64, D1_1D, D1_I1D, DX1_R1D)
#undef STORE_LANE_ISEL
