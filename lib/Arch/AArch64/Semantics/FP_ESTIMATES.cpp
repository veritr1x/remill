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
// The architecture's eight-fraction-bit estimates are midpoint table lookups,
// not correctly rounded division/sqrt. Evaluate the table using integer math
// so host FP rounding, denormal modes, and exception flags cannot affect it.
template <typename T, bool square_root>
ALWAYS_INLINE T EstimateFPBits(State &state, T input) {
  constexpr unsigned p = sizeof(T) == 4 ? 23 : 52;
  constexpr int bias = sizeof(T) == 4 ? 127 : 1023;
  constexpr T sign_bit = T(1) << (sizeof(T) * 8 - 1);
  constexpr T fraction_mask = (T(1) << p) - 1;
  constexpr T exponent_mask = ~(sign_bit | fraction_mask);
  constexpr T quiet_bit = T(1) << (p - 1);
  const T sign = input & sign_bit;
  T fraction = input & fraction_mask;
  int exponent = static_cast<int>((input & exponent_mask) >> p);
  if ((input & exponent_mask) == exponent_mask && fraction) {
    if (!(fraction & quiet_bit)) state.sr.ioc = 1;
    return state.fpcr.dn ? exponent_mask | quiet_bit : input | quiet_bit;
  }
  if (!exponent && fraction && state.fpcr.fz) {
    state.sr.idc = 1;
    fraction = 0;
  }
  if (!exponent && !fraction) {
    state.sr.dzc = 1;
    return sign | exponent_mask;
  }
  if constexpr (square_root) {
    if (sign) {
      state.sr.ioc = 1;
      return exponent_mask | quiet_bit;
    }
  }
  if ((input & exponent_mask) == exponent_mask) return sign;

  if constexpr (!square_root) {
    // Below one quarter of min-normal, the reciprocal estimate overflows.
    if (!exponent && fraction < (T(1) << (p - 2))) {
      state.sr.ofc = 1;
      state.sr.ixc = 1;
      const bool infinity = state.fpcr.rmode == kFPURoundToNearestEven ||
          (state.fpcr.rmode == kFPURoundUpInf && !sign) ||
          (state.fpcr.rmode == kFPURoundDownNegInf && sign);
      return sign | (infinity ? exponent_mask : exponent_mask - 1);
    }
    if (state.fpcr.fz && exponent >= 2 * bias - 1) {
      state.sr.ufc = 1;
      return sign;
    }
  }
  T significand = fraction;
  if (exponent) {
    significand |= T(1) << p;
  } else {
    exponent = 1;
    while (!(significand & (T(1) << p))) {
      significand <<= 1;
      --exponent;
    }
  }
  unsigned estimate;
  int result_exponent;
  if constexpr (square_root) {
    // Round sqrt(N/d) to nearest using squared half-integer boundaries.
    // d is odd and N a power of two, so no half-way tie is possible.
    const uint64_t d = 2 * (128 + ((significand & fraction_mask) >> (p - 7))) + 1;
    const uint64_t n = UINT64_C(1) << ((exponent & 1) ? 26 : 25);
    unsigned low = 256, high = 512;
    while (low + 1 < high) {
      const unsigned mid = (low + high) / 2;
      const uint64_t boundary = 2 * mid - 1;
      if (d * boundary * boundary <= 4 * n) low = mid;
      else high = mid;
    }
    estimate = low;
    result_exponent = (3 * bias - 1 - exponent) / 2;
  } else {
    const unsigned q = 256 + static_cast<unsigned>((significand & fraction_mask) >> (p - 8));
    estimate = (262144 + q) / (2 * q + 1);
    result_exponent = 2 * bias - 1 - exponent;
  }
  const T result_significand = T(estimate) << (p - 8);
  if (result_exponent <= 0)
    return sign | (result_significand >> (1 - result_exponent));
  return sign | (T(result_exponent) << p) | (result_significand & fraction_mask);
}

#define FP_ESTIMATE_SEMANTICS(size) \
  template <size_t lanes, bool square_root, typename S> \
  DEF_SEM(EstimateFP##size, V128W dst, S src) { \
    auto input = UReadV##size(src); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) \
      result.elems[i] = EstimateFPBits<uint##size##_t, square_root>(state, input.elems[i]); \
    UWriteV##size(dst, result); \
    return memory; \
  }
FP_ESTIMATE_SEMANTICS(32)
FP_ESTIMATE_SEMANTICS(64)
#undef FP_ESTIMATE_SEMANTICS
}  // namespace

#define FP_ESTIMATE_ISEL(op, square_root) \
  DEF_ISEL(op##_ASIMDMISC_R_2S) = EstimateFP32<2, square_root, V64>; \
  DEF_ISEL(op##_ASIMDMISC_R_4S) = EstimateFP32<4, square_root, V128>; \
  DEF_ISEL(op##_ASIMDMISC_R_2D) = EstimateFP64<2, square_root, V128>; \
  DEF_ISEL(op##_ASISDMISC_R_S) = EstimateFP32<1, square_root, V32>; \
  DEF_ISEL(op##_ASISDMISC_R_D) = EstimateFP64<1, square_root, V64>;
FP_ESTIMATE_ISEL(FRECPE, false)
FP_ESTIMATE_ISEL(FRSQRTE, true)
#undef FP_ESTIMATE_ISEL
