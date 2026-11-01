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
// FPToFixed(..., rounding=ZERO, fbits=0) without undefined out-of-range C++
// casts. The range check follows truncation: for unsigned conversion -0.5 is
// valid zero with IXC, whereas -1.5 saturates to zero with IOC but no new IXC.
template <typename In, typename Out, bool signed_output>
ALWAYS_INLINE Out AArch64FloatToIntegerBits(State &state, In raw) {
  constexpr unsigned fraction_bits = sizeof(In) == 4 ? 23 : 52;
  constexpr unsigned bias = sizeof(In) == 4 ? 127 : 1023;
  constexpr In fraction_mask = (In(1) << fraction_bits) - 1;
  constexpr In input_sign = In(1) << (sizeof(In) * 8 - 1);
  constexpr In exponent_mask = ~(input_sign | fraction_mask);
  constexpr Out output_sign = Out(1) << (sizeof(Out) * 8 - 1);
  const bool negative = (raw & input_sign) != 0;
  const In fraction = raw & fraction_mask;
  const unsigned encoded_exponent = static_cast<unsigned>((raw & exponent_mask) >> fraction_bits);
  const Out saturated = signed_output ? (negative ? output_sign : output_sign - Out(1))
                                      : (negative ? Out(0) : ~Out(0));
  if ((raw & exponent_mask) == exponent_mask) {
    state.sr.ioc = 1;
    return fraction ? Out(0) : saturated;  // NaNs produce zero; infinities saturate.
  }
  if (!encoded_exponent) {
    if (fraction) {
      if (state.fpcr.fz) state.sr.idc = 1;
      else state.sr.ixc = 1;
    }
    return Out(0);  // Signed zeros and all input subnormals truncate to zero.
  }
  const int exponent = static_cast<int>(encoded_exponent) - static_cast<int>(bias);
  if (exponent < 0) {
    state.sr.ixc = 1;
    return Out(0);
  }
  if (exponent >= static_cast<int>(sizeof(Out) * 8)) {
    state.sr.ioc = 1;
    return saturated;
  }
  const uint64_t significand = (UINT64_C(1) << fraction_bits) | uint64_t(fraction);
  uint64_t magnitude;
  bool inexact = false;
  if (exponent >= static_cast<int>(fraction_bits)) {
    magnitude = significand << (static_cast<unsigned>(exponent) - fraction_bits);
  } else {
    const unsigned shift = fraction_bits - static_cast<unsigned>(exponent);
    magnitude = significand >> shift;
    inexact = (significand & ((UINT64_C(1) << shift) - 1)) != 0;
  }
  if (signed_output) {
    const uint64_t limit = negative ? uint64_t(output_sign) : uint64_t(output_sign - Out(1));
    if (magnitude > limit) {
      state.sr.ioc = 1;
      return saturated;
    }
  } else if (negative && magnitude) {
    state.sr.ioc = 1;
    return Out(0);
  }
  if (inexact) state.sr.ixc = 1;
  return negative ? Out(0) - Out(magnitude) : Out(magnitude);
}

#define SIMD_FP_TO_INT(size) \
  template <size_t lanes, bool signed_output, typename S> \
  DEF_SEM(SIMDFloatToInteger##size, V128W dst, S src) { \
    auto input = UReadV##size(src); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) { \
      result.elems[i] = AArch64FloatToIntegerBits<uint##size##_t, uint##size##_t, signed_output>(state, input.elems[i]); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  }
SIMD_FP_TO_INT(32)
SIMD_FP_TO_INT(64)
#undef SIMD_FP_TO_INT
}  // namespace

#define SIMD_FP_TO_INT_ISEL(op, signed_output) \
  DEF_ISEL(op##_ASIMDMISC_R_2S) = SIMDFloatToInteger32<2, signed_output, V64>; \
  DEF_ISEL(op##_ASIMDMISC_R_4S) = SIMDFloatToInteger32<4, signed_output, V128>; \
  DEF_ISEL(op##_ASIMDMISC_R_2D) = SIMDFloatToInteger64<2, signed_output, V128>; \
  DEF_ISEL(op##_ASISDMISC_R_S) = SIMDFloatToInteger32<1, signed_output, V32>; \
  DEF_ISEL(op##_ASISDMISC_R_D) = SIMDFloatToInteger64<1, signed_output, V64>;
SIMD_FP_TO_INT_ISEL(FCVTZS, true)
SIMD_FP_TO_INT_ISEL(FCVTZU, false)
#undef SIMD_FP_TO_INT_ISEL
