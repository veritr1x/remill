/*
 * Copyright (c) 2026 Trail of Bits, Inc.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
namespace {
ALWAYS_INLINE unsigned FusedHighestBit(uint128_t value) {
  auto high = static_cast<uint64_t>(value >> 64);
  return high ? 127U - __builtin_clzll(high)
              : 63U - __builtin_clzll(static_cast<uint64_t>(value));
}

ALWAYS_INLINE uint128_t FusedShiftRightJam(uint128_t value, unsigned distance) {
  if (!distance) return value;
  if (distance >= 128) return value != 0;
  const auto lost = value & ((uint128_t(1) << distance) - 1);
  return (value >> distance) | (lost != 0);
}

// Compute a*b+c with one rounding, without host FP or intermediate overflow.
// Binary64's exact product has at most106 bits. Normalize each term at bit126,
// retaining at least21 zero low bits before alignment. If alignment discards
// bits, the exponent gap precludes deep cancellation; a jammed bit preserves
// all information needed for any53-bit rounding (also after a one-bit borrow).
// Close opposing terms retain their complete significands and cancel exactly.
template <typename T>
ALWAYS_INLINE T IndexedFusedFPBits(State &state, T accumulator, T lhs, T rhs) {
  constexpr unsigned fraction_bits = sizeof(T) == 4 ? 23 : 52;
  constexpr int bias = sizeof(T) == 4 ? 127 : 1023;
  constexpr int minimum = 1 - bias, maximum = bias;
  constexpr T sign = T(1) << (sizeof(T) * 8 - 1);
  constexpr T fraction = (T(1) << fraction_bits) - 1;
  constexpr T exponent = ~(sign | fraction);
  constexpr T quiet = (fraction + T(1)) >> 1;
  constexpr T default_nan = exponent | quiet;
  T values[3] = {accumulator, lhs, rhs};
  bool nan[3], signaling[3];
  _Pragma("unroll") for (unsigned i = 0; i < 3; ++i) {
    if (state.fpcr.fz && !(values[i] & exponent) && (values[i] & fraction)) {
      state.sr.idc = 1;
      values[i] &= sign;
    }
    nan[i] = (values[i] & exponent) == exponent && (values[i] & fraction);
    signaling[i] = nan[i] && !(values[i] & quiet);
  }
  accumulator = values[0]; lhs = values[1]; rhs = values[2];
  const bool lhs_zero = !(lhs & ~sign), rhs_zero = !(rhs & ~sign);
  const bool lhs_inf = (lhs & ~sign) == exponent, rhs_inf = (rhs & ~sign) == exponent;
  const bool invalid_product = (lhs_zero && rhs_inf) || (rhs_zero && lhs_inf);
  if (signaling[0] || signaling[1] || signaling[2]) state.sr.ioc = 1;
  // ARM's special qNaN-accumulator + (0*infinity) rule precedes ordinary
  // NaN propagation: IOC and positive default NaN even when DN is disabled.
  if (invalid_product && nan[0] && !signaling[0]) {
    state.sr.ioc = 1;
    return default_nan;
  }
  if (nan[0] || nan[1] || nan[2]) {
    if (state.fpcr.dn) return default_nan;
    _Pragma("unroll") for (unsigned i = 0; i < 3; ++i)
      if (signaling[i]) return values[i] | quiet;
    _Pragma("unroll") for (unsigned i = 0; i < 3; ++i)
      if (nan[i]) return values[i] | quiet;
  }
  if (invalid_product) { state.sr.ioc = 1; return default_nan; }
  const T product_sign = (lhs ^ rhs) & sign;
  const T addend_sign = accumulator & sign;
  const bool addend_inf = (accumulator & ~sign) == exponent;
  if (lhs_inf || rhs_inf) {
    if (addend_inf && product_sign != addend_sign) {
      state.sr.ioc = 1;
      return default_nan;
    }
    return exponent | product_sign;
  }
  if (addend_inf) return accumulator;

  const unsigned lhs_exp = static_cast<unsigned>((lhs & exponent) >> fraction_bits);
  const unsigned rhs_exp = static_cast<unsigned>((rhs & exponent) >> fraction_bits);
  const unsigned acc_exp = static_cast<unsigned>((accumulator & exponent) >> fraction_bits);
  const uint128_t lhs_sig = (lhs & fraction) | (lhs_exp ? T(1) << fraction_bits : T(0));
  const uint128_t rhs_sig = (rhs & fraction) | (rhs_exp ? T(1) << fraction_bits : T(0));
  uint128_t product = lhs_sig * rhs_sig;
  uint128_t addend = (accumulator & fraction) | (acc_exp ? T(1) << fraction_bits : T(0));
  if (!product && !addend) {
    return product_sign == addend_sign ? product_sign
        : state.fpcr.rmode == kFPURoundDownNegInf ? sign : T(0);
  }
  int product_top = 0, addend_top = 0;
  if (product) {
    const unsigned highest = FusedHighestBit(product);
    product_top = static_cast<int>(lhs_exp ? lhs_exp : 1) +
                  static_cast<int>(rhs_exp ? rhs_exp : 1) - 2 * bias -
                  2 * static_cast<int>(fraction_bits) + static_cast<int>(highest);
    product <<= 126 - highest;
  }
  if (addend) {
    const unsigned highest = FusedHighestBit(addend);
    addend_top = static_cast<int>(acc_exp ? acc_exp : 1) - bias -
                 static_cast<int>(fraction_bits) + static_cast<int>(highest);
    addend <<= 126 - highest;
  }
  const int top = !product ? addend_top : !addend ? product_top
                  : product_top > addend_top ? product_top : addend_top;
  if (product) product = FusedShiftRightJam(product, static_cast<unsigned>(top - product_top));
  if (addend) addend = FusedShiftRightJam(addend, static_cast<unsigned>(top - addend_top));
  uint128_t sum;
  T result_sign;
  if (product_sign == addend_sign) { sum = product + addend; result_sign = product_sign; }
  else if (product >= addend) { sum = product - addend; result_sign = product_sign; }
  else { sum = addend - product; result_sign = addend_sign; }
  if (!sum) return state.fpcr.rmode == kFPURoundDownNegInf ? sign : T(0);

  const int highest = static_cast<int>(FusedHighestBit(sum));
  int result_top = top - 126 + highest;
  const bool tiny = result_top < minimum;
  if (tiny && state.fpcr.fz) {
    state.sr.ufc = 1;  // ARM flushes before rounding, without adding IXC.
    return result_sign;
  }
  const int shift = tiny ? minimum - static_cast<int>(fraction_bits) - (top - 126)
                         : highest - static_cast<int>(fraction_bits);
  uint128_t rounded;
  bool inexact = false, above_half = false, at_half = false;
  if (shift <= 0) rounded = sum << -shift;
  else if (shift > 128) { rounded = 0; inexact = true; }
  else {
    const uint128_t half = uint128_t(1) << (shift - 1);
    const uint128_t discarded = shift == 128 ? sum : sum & ((uint128_t(1) << shift) - 1);
    rounded = shift == 128 ? 0 : sum >> shift;
    inexact = discarded != 0;
    above_half = discarded > half;
    at_half = discarded == half;
  }
  const unsigned mode = state.fpcr.rmode;
  const bool increment = mode == kFPURoundToNearestEven ? above_half || (at_half && (rounded & 1))
      : mode == kFPURoundUpInf ? inexact && !result_sign
      : mode == kFPURoundDownNegInf ? inexact && result_sign : false;
  rounded += increment;
  if (!tiny && rounded >= (uint128_t(1) << (fraction_bits + 1))) {
    rounded >>= 1;
    ++result_top;
  }
  if (result_top > maximum) {
    state.sr.ofc = 1;
    state.sr.ixc = 1;
    const bool infinity = mode == kFPURoundToNearestEven ||
        (mode == kFPURoundUpInf && !result_sign) ||
        (mode == kFPURoundDownNegInf && result_sign);
    return result_sign | (infinity ? exponent : exponent - T(1));
  }
  if (inexact) { state.sr.ixc = 1; if (tiny) state.sr.ufc = 1; }
  // A tiny exact value can round up into min-normal, encoded by bit f.
  if (tiny) return result_sign | static_cast<T>(rounded);
  return result_sign | (T(result_top + bias) << fraction_bits) |
         (static_cast<T>(rounded) & fraction);
}

#define INDEXED_FUSED_SEMANTICS(size) \
  template <size_t lanes> \
  DEF_SEM(IndexedFused##size, V128W dst, V128 acc, V128 src1, V128 src2, I64 lane) { \
    const auto previous = UReadV##size(acc); \
    const auto lhs = UReadV##size(src1); \
    const auto rhs = UReadV##size(src2); \
    const auto element = rhs.elems[Read(lane)]; \
    auto result = UClearV##size(previous); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) \
      result.elems[i] = IndexedFusedFPBits<uint##size##_t>(state, previous.elems[i], lhs.elems[i], element); \
    UWriteV##size(dst, result); \
    return memory; \
  }
INDEXED_FUSED_SEMANTICS(32)
INDEXED_FUSED_SEMANTICS(64)
#undef INDEXED_FUSED_SEMANTICS
}  // namespace
DEF_ISEL(FMLA_ASIMDELEM_R_SD_2S) = IndexedFused32<2>;
DEF_ISEL(FMLA_ASIMDELEM_R_SD_4S) = IndexedFused32<4>;
DEF_ISEL(FMLA_ASIMDELEM_R_SD_2D) = IndexedFused64<2>;
DEF_ISEL(FMLA_ASISDELEM_R_SD_S) = IndexedFused32<1>;
DEF_ISEL(FMLA_ASISDELEM_R_SD_D) = IndexedFused64<1>;
