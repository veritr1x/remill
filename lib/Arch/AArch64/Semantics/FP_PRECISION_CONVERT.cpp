/* Copyright (c) 2026 Trail of Bits, Inc.
 * Licensed under the Apache License, Version 2.0. */
namespace {
// FP32/FP64 conversion in integer space: ordinary LLVM fptrunc cannot carry
// dynamic FPCR rounding, ARM flushing, or deterministic NaN payload rules.
template <typename In, typename Out>
ALWAYS_INLINE Out AArch64FloatPrecisionBits(State &state, In raw) {
  constexpr unsigned in_frac = sizeof(In) == 4 ? 23 : 52;
  constexpr unsigned out_frac = sizeof(Out) == 4 ? 23 : 52;
  constexpr int in_bias = sizeof(In) == 4 ? 127 : 1023;
  constexpr int out_bias = sizeof(Out) == 4 ? 127 : 1023;
  constexpr In in_exp_mask = sizeof(In) == 4 ? 255 : 2047;
  constexpr Out out_exp_mask = sizeof(Out) == 4 ? 255 : 2047;
  constexpr In in_fraction_mask = (In(1) << in_frac) - 1;
  const bool negative = (raw >> (sizeof(In) * 8 - 1)) != 0;
  const Out sign = negative ? Out(1) << (sizeof(Out) * 8 - 1) : Out(0);
  const unsigned exponent = unsigned((raw >> in_frac) & in_exp_mask);
  uint64_t fraction = raw & in_fraction_mask;
  if (exponent == in_exp_mask) {
    if (!fraction) return sign | (out_exp_mask << out_frac);
    if (!(fraction & (UINT64_C(1) << (in_frac - 1)))) state.sr.ioc = 1;
    if (state.fpcr.dn) return (out_exp_mask << out_frac) | (Out(1) << (out_frac - 1));
    if constexpr (out_frac > in_frac) fraction <<= out_frac - in_frac;
    else fraction >>= in_frac - out_frac;
    return sign | (out_exp_mask << out_frac) | Out(fraction) | (Out(1) << (out_frac - 1));
  }
  if (!exponent) {
    if (!fraction) return sign;
    if (state.fpcr.fz) { state.sr.idc = 1; return sign; }
  } else fraction |= UINT64_C(1) << in_frac;
  const int scale = (exponent ? int(exponent) - in_bias : 1 - in_bias) - int(in_frac);
  const int leading = 63 - int(__builtin_clzll(fraction));
  int result_exp = scale + leading;
  const bool tiny = result_exp < 1 - out_bias;
  if (tiny && state.fpcr.fz) { state.sr.ufc = 1; return sign; }
  const int shift = (tiny ? 1 - out_bias - int(out_frac) : result_exp - int(out_frac)) - scale;
  uint64_t significand;
  bool inexact = false, increment = false;
  if (shift <= 0) significand = fraction << unsigned(-shift);
  else {
    significand = shift >= 64 ? 0 : fraction >> unsigned(shift);
    const uint64_t remainder = shift >= 64 ? fraction : fraction & ((UINT64_C(1) << unsigned(shift)) - 1);
    inexact = remainder != 0;
    if (inexact) {
      switch (state.fpcr.rmode) {
        case kFPURoundToNearestEven:
          if (shift <= 64) {
            const uint64_t half = UINT64_C(1) << unsigned(shift - 1);
            increment = remainder > half || (remainder == half && (significand & 1));
          }
          break;
        case kFPURoundUpInf: increment = !negative; break;
        case kFPURoundDownNegInf: increment = negative; break;
        case kFPURoundToZero: break;
      }
    }
    significand += increment;
  }
  if (!tiny && significand == (UINT64_C(1) << (out_frac + 1))) {
    significand >>= 1; ++result_exp;
  }
  if (result_exp > out_bias) {
    state.sr.ofc = 1; state.sr.ixc = 1;
    const bool infinity = state.fpcr.rmode == kFPURoundToNearestEven ||
        (state.fpcr.rmode == kFPURoundUpInf && !negative) ||
        (state.fpcr.rmode == kFPURoundDownNegInf && negative);
    return sign | ((out_exp_mask << out_frac) - Out(!infinity));
  }
  if (inexact) { state.sr.ixc = 1; if (tiny) state.sr.ufc = 1; }
  if (tiny) return sign | Out(significand);
  return sign | (Out(result_exp + out_bias) << out_frac) |
      Out(significand & ((UINT64_C(1) << out_frac) - 1));
}

template <bool upper>
DEF_SEM(FPConvertNarrow, V128W dst, V128 src) {
  const auto input = UReadV64(src);
  auto result = upper ? UReadV32(dst) : UClearV32(UReadV32(dst));
  _Pragma("unroll") for (size_t i = 0; i < 2; ++i)
    result.elems[i + (upper ? 2 : 0)] = AArch64FloatPrecisionBits<uint64_t, uint32_t>(state, input.elems[i]);
  UWriteV32(dst, result);
  return memory;
}
template <bool upper>
DEF_SEM(FPConvertWiden, V128W dst, V128 src) {
  const auto input = UReadV32(src);
  auto result = UClearV64(UReadV64(dst));
  _Pragma("unroll") for (size_t i = 0; i < 2; ++i)
    result.elems[i] = AArch64FloatPrecisionBits<uint32_t, uint64_t>(state, input.elems[i + (upper ? 2 : 0)]);
  UWriteV64(dst, result);
  return memory;
}
}  // namespace
DEF_ISEL(FCVTN_ASIMDMISC_N_2S) = FPConvertNarrow<false>;
DEF_ISEL(FCVTN_ASIMDMISC_N_4S) = FPConvertNarrow<true>;
DEF_ISEL(FCVTL_ASIMDMISC_L_2S) = FPConvertWiden<false>;
DEF_ISEL(FCVTL_ASIMDMISC_L_4S) = FPConvertWiden<true>;
