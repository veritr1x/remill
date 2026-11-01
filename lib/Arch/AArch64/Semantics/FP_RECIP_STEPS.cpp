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
template <typename D, typename S>
ALWAYS_INLINE D RecipFPBitCast(S value) {
  static_assert(sizeof(D) == sizeof(S), "bit cast width mismatch");
  D result;
  __builtin_memcpy(&result, &value, sizeof(result));
  return result;
}

template <typename T, bool square_root_step>
ALWAYS_INLINE bool RecipProductBelowHalfULP(T lhs, T rhs) {
  constexpr unsigned fraction_bits = sizeof(T) == 4 ? 23 : 52;
  constexpr int bias = sizeof(T) == 4 ? 127 : 1023;
  constexpr T fraction = (T(1) << fraction_bits) - 1;
  constexpr T exponent = sizeof(T) == 4 ? T(0x7f800000) : T(0x7ff0000000000000ULL);
  const unsigned lhs_exp = static_cast<unsigned>((lhs & exponent) >> fraction_bits);
  const unsigned rhs_exp = static_cast<unsigned>((rhs & exponent) >> fraction_bits);
  const T lhs_sig = (lhs & fraction) | (lhs_exp ? T(1) << fraction_bits : T(0));
  const T rhs_sig = (rhs & fraction) | (rhs_exp ? T(1) << fraction_bits : T(0));
  const uint128_t product = uint128_t(lhs_sig) * uint128_t(rhs_sig);
  // Below 2^(-p) for RECPS; below 2^(-(p-1)) before RSQRTS's scaling by 1/2.
  // These strict bounds are below half the smaller adjacent ULP at 2 and 1.5.
  const int shift = 2 * bias + static_cast<int>(fraction_bits) - (square_root_step ? 0 : 1)
                    - static_cast<int>(lhs_exp ? lhs_exp : 1) - static_cast<int>(rhs_exp ? rhs_exp : 1);
  if (shift <= 0) return false;
  if (shift >= 128) return true;
  return product < (uint128_t(1) << shift);
}

#define RECIP_FUSED_FP(size, float_type) \
  ALWAYS_INLINE uint##size##_t CheckedRecipFMA##size(State &state, uint##size##_t lhs, \
                                                   uint##size##_t rhs, uint##size##_t addend) { \
    __remill_fpu_exception_clear(kFPUExceptionAll); \
    BarrierReorder(); \
    volatile float_type a = RecipFPBitCast<float_type>(lhs); \
    volatile float_type b = RecipFPBitCast<float_type>(rhs); \
    volatile float_type c = RecipFPBitCast<float_type>(addend); \
    volatile float_type result = std::fma(a, b, c); \
    BarrierReorder(); \
    const int exceptions = __remill_fpu_exception_test(kFPUExceptionAll); \
    SetFPSRStatusFlags(state, exceptions); \
    return RecipFPBitCast<uint##size##_t>(static_cast<float_type>(result)); \
  }
RECIP_FUSED_FP(32, float32_t)
RECIP_FUSED_FP(64, float64_t)
#undef RECIP_FUSED_FP

template <typename T, bool square_root_step>
ALWAYS_INLINE T ReciprocalStepFPBits(State &state, T lhs, T rhs) {
  constexpr unsigned fraction_bits = sizeof(T) == 4 ? 23 : 52;
  constexpr T sign = T(1) << (sizeof(T) * 8 - 1);
  constexpr T fraction = (T(1) << fraction_bits) - 1;
  constexpr T exponent = ~(sign | fraction);
  constexpr T quiet = (fraction + T(1)) >> 1;
  constexpr T two = sizeof(T) == 4 ? T(0x40000000) : T(0x4000000000000000ULL);
  constexpr T one_and_half = sizeof(T) == 4 ? T(0x3fc00000) : T(0x3ff8000000000000ULL);
  constexpr T constant = square_root_step ? one_and_half : two;

  // ARM negates operand one before unpacking and NaN processing. Consequently
  // a propagated lhs NaN changes sign, while a rhs NaN does not. DN overrides
  // both with the positive canonical default NaN.
  lhs ^= sign;
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
    if (state.fpcr.dn) return exponent | quiet;
    return (lhs_signaling ? lhs : rhs_signaling ? rhs : lhs_nan ? lhs : rhs) | quiet;
  }
  // In particular, 0*infinity produces the step's constant without IOC.
  if (!(lhs & ~sign) || !(rhs & ~sign)) return constant;
  if ((lhs & ~sign) == exponent || (rhs & ~sign) == exponent)
    return exponent | ((lhs ^ rhs) & sign);

  if (RecipProductBelowHalfULP<T, square_root_step>(lhs, rhs)) {
    // The exact nonzero product contributes only guard/sticky information
    // around the normal positive constant. No intermediate underflow applies.
    T result = constant;
    const bool negative_product = (lhs ^ rhs) & sign;
    if (state.fpcr.rmode == kFPURoundUpInf && !negative_product) ++result;
    else if ((state.fpcr.rmode == kFPURoundDownNegInf || state.fpcr.rmode == kFPURoundToZero)
             && negative_product) --result;
    state.sr.ixc = 1;
    return result;
  }

  if constexpr (square_root_step) {
    // Halve the operand with the larger exponent. At this point a product
    // near the step's constant cannot have both operands near min-normal;
    // the selected exponent is >1 and this bit scaling is exact and normal.
    // One fused operation now computes 1.5 + (lhs/2)*rhs, avoiding both the
    // premature overflow and double rounding of fma(lhs,rhs,3)/2.
    constexpr T exponent_unit = T(1) << fraction_bits;
    if ((lhs & exponent) >= (rhs & exponent)) lhs -= exponent_unit;
    else rhs -= exponent_unit;
    if constexpr (sizeof(T) == 4) return CheckedRecipFMA32(state, lhs, rhs, one_and_half);
    else return CheckedRecipFMA64(state, lhs, rhs, one_and_half);
  } else {
    if constexpr (sizeof(T) == 4) return CheckedRecipFMA32(state, lhs, rhs, two);
    else return CheckedRecipFMA64(state, lhs, rhs, two);
  }
}

#define RECIP_STEP_SEMANTICS(size) \
  template <size_t lanes, bool square_root_step, typename S> \
  DEF_SEM(ReciprocalStepFP##size, V128W dst, S src1, S src2) { \
    auto lhs = UReadV##size(src1); \
    auto rhs = UReadV##size(src2); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) \
      result.elems[i] = ReciprocalStepFPBits<uint##size##_t, square_root_step>(state, lhs.elems[i], rhs.elems[i]); \
    UWriteV##size(dst, result); \
    return memory; \
  }
RECIP_STEP_SEMANTICS(32)
RECIP_STEP_SEMANTICS(64)
#undef RECIP_STEP_SEMANTICS
}  // namespace

#define RECIP_STEP_ISEL(op, square_root_step) \
  DEF_ISEL(op##_ASIMDSAME_ONLY_2S) = ReciprocalStepFP32<2, square_root_step, V64>; \
  DEF_ISEL(op##_ASIMDSAME_ONLY_4S) = ReciprocalStepFP32<4, square_root_step, V128>; \
  DEF_ISEL(op##_ASIMDSAME_ONLY_2D) = ReciprocalStepFP64<2, square_root_step, V128>; \
  DEF_ISEL(op##_ASISDSAME_ONLY_S) = ReciprocalStepFP32<1, square_root_step, V32>; \
  DEF_ISEL(op##_ASISDSAME_ONLY_D) = ReciprocalStepFP64<1, square_root_step, V64>;
RECIP_STEP_ISEL(FRECPS, false)
RECIP_STEP_ISEL(FRSQRTS, true)
#undef RECIP_STEP_ISEL
