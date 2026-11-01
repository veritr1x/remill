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
// Build IEEE bits directly. LLVM's ordinary sitofp/uitofp operations assume
// round-to-nearest rather than consulting FPCR, even with a volatile result.
template <typename In, typename Out, bool signed_input>
ALWAYS_INLINE Out AArch64IntegerToFloatBits(State &state, In raw) {
  const bool negative = signed_input && (raw >> (sizeof(In) * 8 - 1));
  const uint64_t magnitude = negative ? uint64_t(In(0) - raw) : uint64_t(raw);
  if (!magnitude) return Out(0);
  constexpr unsigned fraction_bits = sizeof(Out) == 4 ? 23 : 52;
  constexpr unsigned bias = sizeof(Out) == 4 ? 127 : 1023;
  constexpr uint64_t fraction_mask = (UINT64_C(1) << fraction_bits) - 1;
  unsigned exponent = 63U - static_cast<unsigned>(__builtin_clzll(magnitude));
  uint64_t significand;
  if (exponent <= fraction_bits) {
    significand = magnitude << (fraction_bits - exponent);
  } else {
    const unsigned shift = exponent - fraction_bits;
    significand = magnitude >> shift;
    const uint64_t remainder = magnitude & ((UINT64_C(1) << shift) - 1);
    if (remainder) {
      state.sr.ixc = 1;
      bool increment = false;
      switch (state.fpcr.rmode) {
        case kFPURoundToNearestEven: {
          const uint64_t half = UINT64_C(1) << (shift - 1);
          increment = remainder > half || (remainder == half && (significand & 1));
          break;
        }
        case kFPURoundUpInf: increment = !negative; break;
        case kFPURoundDownNegInf: increment = negative; break;
        case kFPURoundToZero: break;
      }
      if (increment && ++significand == (UINT64_C(1) << (fraction_bits + 1))) {
        significand >>= 1;
        ++exponent;
      }
    }
  }
  const Out sign = negative ? Out(1) << (sizeof(Out) * 8 - 1) : Out(0);
  return sign | (Out(exponent + bias) << fraction_bits) | Out(significand & fraction_mask);
}

#define SIMD_INT_TO_FP(size) \
  template <size_t lanes, bool signed_input, typename S> \
  DEF_SEM(SIMDIntegerToFloat##size, V128W dst, S src) { \
    auto input = UReadV##size(src); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) { \
      result.elems[i] = AArch64IntegerToFloatBits<uint##size##_t, uint##size##_t, signed_input>(state, input.elems[i]); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  }
SIMD_INT_TO_FP(32)
SIMD_INT_TO_FP(64)
#undef SIMD_INT_TO_FP
}  // namespace

#define SIMD_INT_TO_FP_ISEL(op, signed_input) \
  DEF_ISEL(op##_ASIMDMISC_R_2S) = SIMDIntegerToFloat32<2, signed_input, V64>; \
  DEF_ISEL(op##_ASIMDMISC_R_4S) = SIMDIntegerToFloat32<4, signed_input, V128>; \
  DEF_ISEL(op##_ASIMDMISC_R_2D) = SIMDIntegerToFloat64<2, signed_input, V128>; \
  DEF_ISEL(op##_ASISDMISC_R_S) = SIMDIntegerToFloat32<1, signed_input, V32>; \
  DEF_ISEL(op##_ASISDMISC_R_D) = SIMDIntegerToFloat64<1, signed_input, V64>;
SIMD_INT_TO_FP_ISEL(SCVTF, true)
SIMD_INT_TO_FP_ISEL(UCVTF, false)
#undef SIMD_INT_TO_FP_ISEL
