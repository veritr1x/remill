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
enum class ExtraFPOperation { kNegativeMultiply, kAbsoluteDifference, kSquareRoot };

template <typename D, typename S>
ALWAYS_INLINE D ExtraFPBitCast(S value) {
  static_assert(sizeof(D) == sizeof(S), "bit cast width mismatch");
  D result;
  __builtin_memcpy(&result, &value, sizeof(result));
  return result;
}

// ARM tests multiplication tininess before rounding. In particular, a tiny
// result can round up to the smallest normal yet still raise UFC with IXC.
// The exact product fits in 106 bits for binary64, so integer arithmetic can
// determine tininess without depending on the host's underflow convention.
template <typename T>
ALWAYS_INLINE bool ExtraFPProductIsTiny(T lhs, T rhs) {
  constexpr unsigned fraction_bits = sizeof(T) == 4 ? 23 : 52;
  constexpr unsigned bias = sizeof(T) == 4 ? 127 : 1023;
  constexpr T fraction = (T(1) << fraction_bits) - 1;
  constexpr T exponent = sizeof(T) == 4 ? T(0x7f800000) : T(0x7ff0000000000000ULL);
  const unsigned lhs_exp = static_cast<unsigned>((lhs & exponent) >> fraction_bits);
  const unsigned rhs_exp = static_cast<unsigned>((rhs & exponent) >> fraction_bits);
  const T lhs_sig = (lhs & fraction) | (lhs_exp ? T(1) << fraction_bits : T(0));
  const T rhs_sig = (rhs & fraction) | (rhs_exp ? T(1) << fraction_bits : T(0));
  if (!lhs_sig || !rhs_sig) return false;
  const uint128_t product = uint128_t(lhs_sig) * uint128_t(rhs_sig);
  const int shift = 1 + static_cast<int>(bias) + 2 * static_cast<int>(fraction_bits)
                    - static_cast<int>(lhs_exp ? lhs_exp : 1) - static_cast<int>(rhs_exp ? rhs_exp : 1);
  if (shift <= 0) return false;
  if (shift >= 128) return true;
  return product < (uint128_t(1) << shift);
}

#define EXTRA_FINITE_FP(size, float_type) \
  template <ExtraFPOperation operation> \
  ALWAYS_INLINE uint##size##_t ExtraFiniteFP(State &state, uint##size##_t lhs, uint##size##_t rhs) { \
    auto a = ExtraFPBitCast<float_type>(lhs); \
    float_type result; \
    if constexpr (operation == ExtraFPOperation::kSquareRoot) { \
      result = CheckedFloatUnaryOp(state, FSqrt##size, a); \
    } else { \
      auto b = ExtraFPBitCast<float_type>(rhs); \
      if constexpr (operation == ExtraFPOperation::kNegativeMultiply) \
        result = CheckedFloatBinOp(state, FMul##size, a, b); \
      else result = CheckedFloatBinOp(state, FSub##size, a, b); \
    } \
    return ExtraFPBitCast<uint##size##_t>(result); \
  }
EXTRA_FINITE_FP(32, float32_t)
EXTRA_FINITE_FP(64, float64_t)
#undef EXTRA_FINITE_FP

template <typename T, ExtraFPOperation operation>
ALWAYS_INLINE T FinishExtraFP(T value) {
  constexpr T sign = T(1) << (sizeof(T) * 8 - 1);
  // These bit operations occur after the arithmetic has rounded. This order
  // matters for directed rounding and for the sign of propagated/default NaNs.
  if constexpr (operation == ExtraFPOperation::kNegativeMultiply) return value ^ sign;
  if constexpr (operation == ExtraFPOperation::kAbsoluteDifference) return value & ~sign;
  return value;
}

template <typename T, ExtraFPOperation operation>
ALWAYS_INLINE T ArithmeticExtraFPBits(State &state, T lhs, T rhs) {
  constexpr T sign = T(1) << (sizeof(T) * 8 - 1);
  constexpr T fraction = sizeof(T) == 4 ? T(0x007fffff) : T(0x000fffffffffffffULL);
  constexpr T exponent = ~(sign | fraction);
  constexpr T quiet = (fraction + T(1)) >> 1;
  constexpr T default_nan = exponent | quiet;
  if (state.fpcr.fz) {
    if (!(lhs & exponent) && (lhs & fraction)) { state.sr.idc = 1; lhs &= sign; }
    if (!(rhs & exponent) && (rhs & fraction)) { state.sr.idc = 1; rhs &= sign; }
  }
  const bool lhs_nan = (lhs & exponent) == exponent && (lhs & fraction);
  const bool rhs_nan = (rhs & exponent) == exponent && (rhs & fraction);
  const bool lhs_signaling = lhs_nan && !(lhs & quiet);
  const bool rhs_signaling = rhs_nan && !(rhs & quiet);
  if (lhs_nan || rhs_nan) {
    if (lhs_signaling || rhs_signaling) state.sr.ioc = 1;
    const T selected = lhs_signaling ? lhs : rhs_signaling ? rhs : lhs_nan ? lhs : rhs;
    const T result = state.fpcr.dn ? default_nan : selected | quiet;
    return FinishExtraFP<T, operation>(result);
  }
  const bool lhs_zero = !(lhs & ~sign), rhs_zero = !(rhs & ~sign);
  const bool lhs_inf = (lhs & ~sign) == exponent, rhs_inf = (rhs & ~sign) == exponent;
  if constexpr (operation == ExtraFPOperation::kSquareRoot) {
    if (lhs_zero) return lhs;  // Preserve -0, including a flushed negative input.
    if (lhs & sign) { state.sr.ioc = 1; return default_nan; }
    if (lhs_inf) return lhs;
    return ExtraFiniteFP<operation>(state, lhs, T(0));
  } else if constexpr (operation == ExtraFPOperation::kNegativeMultiply) {
    if ((lhs_inf && rhs_zero) || (rhs_inf && lhs_zero)) {
      state.sr.ioc = 1;
      return FinishExtraFP<T, operation>(default_nan);
    }
    const T result_sign = (lhs ^ rhs) & sign;
    if (lhs_inf || rhs_inf) return FinishExtraFP<T, operation>(exponent | result_sign);
    if (lhs_zero || rhs_zero) return FinishExtraFP<T, operation>(result_sign);
    const bool tiny = ExtraFPProductIsTiny(lhs, rhs);
    if (tiny && state.fpcr.fz) {
      state.sr.ufc = 1;  // Flush before rounding: UFC, without adding IXC.
      return FinishExtraFP<T, operation>(result_sign);
    }
    const T result = ExtraFiniteFP<operation>(state, lhs, rhs);
    if (tiny && __remill_fpu_exception_test(kFPUExceptionPrecision)) state.sr.ufc = 1;
    return FinishExtraFP<T, operation>(result);
  } else {
    if (lhs_inf && rhs_inf && !((lhs ^ rhs) & sign)) {
      state.sr.ioc = 1;
      return default_nan;
    }
    if (lhs_inf || rhs_inf) return exponent;
    if (lhs_zero && rhs_zero) return T(0);
    const uint8_t old_precision = state.sr.ixc;
    T result = ExtraFiniteFP<operation>(state, lhs, rhs);
    if (state.fpcr.fz && !(result & exponent) && (result & fraction)) {
      state.sr.ufc = 1;
      state.sr.ixc = old_precision;
      __remill_fpu_exception_clear(kFPUExceptionPrecision);
      result &= sign;
    }
    return FinishExtraFP<T, operation>(result);
  }
}

#define EXTRA_FP_SEMANTICS(size) \
  template <size_t lanes, ExtraFPOperation operation, typename S> \
  DEF_SEM(ArithmeticExtraFP##size, V128W dst, S src1, S src2) { \
    auto lhs = UReadV##size(src1); \
    auto rhs = UReadV##size(src2); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) \
      result.elems[i] = ArithmeticExtraFPBits<uint##size##_t, operation>(state, lhs.elems[i], rhs.elems[i]); \
    UWriteV##size(dst, result); \
    return memory; \
  } \
  template <size_t lanes, typename S> \
  DEF_SEM(SquareRootExtraFP##size, V128W dst, S src) { \
    auto values = UReadV##size(src); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) \
      result.elems[i] = ArithmeticExtraFPBits<uint##size##_t, ExtraFPOperation::kSquareRoot>(state, values.elems[i], uint##size##_t(0)); \
    UWriteV##size(dst, result); \
    return memory; \
  }
EXTRA_FP_SEMANTICS(32)
EXTRA_FP_SEMANTICS(64)
#undef EXTRA_FP_SEMANTICS
}  // namespace

DEF_ISEL(FNMUL_S_FLOATDP2) = ArithmeticExtraFP32<1, ExtraFPOperation::kNegativeMultiply, V32>;
DEF_ISEL(FNMUL_D_FLOATDP2) = ArithmeticExtraFP64<1, ExtraFPOperation::kNegativeMultiply, V64>;
DEF_ISEL(FABD_ASISDSAME_ONLY_S) = ArithmeticExtraFP32<1, ExtraFPOperation::kAbsoluteDifference, V32>;
DEF_ISEL(FABD_ASISDSAME_ONLY_D) = ArithmeticExtraFP64<1, ExtraFPOperation::kAbsoluteDifference, V64>;
DEF_ISEL(FABD_ASIMDSAME_ONLY_2S) = ArithmeticExtraFP32<2, ExtraFPOperation::kAbsoluteDifference, V64>;
DEF_ISEL(FABD_ASIMDSAME_ONLY_4S) = ArithmeticExtraFP32<4, ExtraFPOperation::kAbsoluteDifference, V128>;
DEF_ISEL(FABD_ASIMDSAME_ONLY_2D) = ArithmeticExtraFP64<2, ExtraFPOperation::kAbsoluteDifference, V128>;
DEF_ISEL(FSQRT_S_FLOATDP1) = SquareRootExtraFP32<1, V32>;
DEF_ISEL(FSQRT_D_FLOATDP1) = SquareRootExtraFP64<1, V64>;
DEF_ISEL(FSQRT_ASIMDMISC_R_2S) = SquareRootExtraFP32<2, V64>;
DEF_ISEL(FSQRT_ASIMDMISC_R_4S) = SquareRootExtraFP32<4, V128>;
DEF_ISEL(FSQRT_ASIMDMISC_R_2D) = SquareRootExtraFP64<2, V128>;
