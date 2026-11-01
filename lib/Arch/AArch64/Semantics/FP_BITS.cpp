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
// These operations are bit transfers, not floating-point arithmetic. Keeping
// them integer-only preserves signaling NaNs and payloads without exceptions.
#define VECTOR_FLOAT_BITS(size) \
  template <size_t lanes, bool negate> \
  DEF_SEM(VectorFloatSign##size, V128W dst, V128 src) { \
    auto values = UReadV##size(src); \
    auto result = UClearV##size(UReadV##size(dst)); \
    constexpr uint##size##_t sign = uint##size##_t(1) << (size - 1); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) { \
      auto value = UExtractV##size(values, i); \
      value = negate ? static_cast<uint##size##_t>(value ^ sign) \
                     : static_cast<uint##size##_t>(value & static_cast<uint##size##_t>(~sign)); \
      result = UInsertV##size(result, i, value); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  } \
  template <size_t lanes> \
  DEF_SEM(VectorFloatImmediate##size, V128W dst, I##size imm) { \
    auto result = UClearV##size(UReadV##size(dst)); \
    const auto value = Read(imm); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) result = UInsertV##size(result, i, value); \
    UWriteV##size(dst, result); \
    return memory; \
  }
VECTOR_FLOAT_BITS(16)
VECTOR_FLOAT_BITS(32)
VECTOR_FLOAT_BITS(64)
#undef VECTOR_FLOAT_BITS
}  // namespace

#define VECTOR_FLOAT_SIGN_ISEL(family, arr, size, lanes) \
  DEF_ISEL(FABS_##family##_##arr) = VectorFloatSign##size<lanes, false>; \
  DEF_ISEL(FNEG_##family##_##arr) = VectorFloatSign##size<lanes, true>;
VECTOR_FLOAT_SIGN_ISEL(ASIMDMISCFP16_R, 4H, 16, 4)
VECTOR_FLOAT_SIGN_ISEL(ASIMDMISCFP16_R, 8H, 16, 8)
VECTOR_FLOAT_SIGN_ISEL(ASIMDMISC_R, 2S, 32, 2)
VECTOR_FLOAT_SIGN_ISEL(ASIMDMISC_R, 4S, 32, 4)
VECTOR_FLOAT_SIGN_ISEL(ASIMDMISC_R, 2D, 64, 2)
#undef VECTOR_FLOAT_SIGN_ISEL

DEF_ISEL(FMOV_ASIMDIMM_H_H_4H) = VectorFloatImmediate16<4>;
DEF_ISEL(FMOV_ASIMDIMM_H_H_8H) = VectorFloatImmediate16<8>;
DEF_ISEL(FMOV_ASIMDIMM_S_S_2S) = VectorFloatImmediate32<2>;
DEF_ISEL(FMOV_ASIMDIMM_S_S_4S) = VectorFloatImmediate32<4>;
DEF_ISEL(FMOV_ASIMDIMM_D2_D_2D) = VectorFloatImmediate64<2>;
