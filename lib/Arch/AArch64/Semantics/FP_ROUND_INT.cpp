/* Copyright (c) 2026 Trail of Bits, Inc.
 * Licensed under the Apache License, Version 2.0. */
namespace {

// FPRoundInt(..., rounding=POSINF/NEGINF, exact=FALSE), baseline FP profile:
// FP enabled, exception traps and FEAT_AFP alternate/merging controls disabled.
// Integer-only source transport preserves NaN payloads and does not consult or
// modify native rounding/exception state. No input is an address.
template <typename T, bool toward_plus>
ALWAYS_INLINE T AArch64RoundIntegralBits(State &state, T raw) {
  constexpr unsigned fraction_bits = sizeof(T) == 4 ? 23 : 52;
  constexpr unsigned bias = sizeof(T) == 4 ? 127 : 1023;
  constexpr T fraction_mask = (T(1) << fraction_bits) - T(1);
  constexpr T sign_mask = T(1) << (sizeof(T) * 8 - 1);
  constexpr T exponent_mask = ~(sign_mask | fraction_mask);
  constexpr T quiet_bit = T(1) << (fraction_bits - 1);
  const bool negative = (raw & sign_mask) != 0;
  const T fraction = raw & fraction_mask;
  const unsigned encoded_exponent =
      unsigned((raw & exponent_mask) >> fraction_bits);

  if ((raw & exponent_mask) == exponent_mask) {
    if (!fraction) return raw;  // Infinity is unchanged.
    if (!(fraction & quiet_bit)) state.sr.ioc = 1;
    if (state.fpcr.dn) return exponent_mask | quiet_bit;
    return raw | quiet_bit;
  }
  if (!encoded_exponent) {
    if (!fraction) return raw;  // Preserve both signs of exact zero.
    if (state.fpcr.fz) {
      state.sr.idc = 1;
      return raw & sign_mask;
    }
  }
  const int exponent = int(encoded_exponent) - int(bias);
  if (exponent >= int(fraction_bits)) return raw;  // Already integral.
  const bool increment_magnitude = toward_plus ? !negative : negative;
  if (exponent < 0) {
    // Every nonzero magnitude below1 becomes signed zero or signed one.
    return (raw & sign_mask) |
        (increment_magnitude ? (T(bias) << fraction_bits) : T(0));
  }
  const unsigned shift = fraction_bits - unsigned(exponent);
  const T discarded_mask = (T(1) << shift) - T(1);
  if (!(raw & discarded_mask)) return raw;
  T result = raw & ~discarded_mask;
  if (increment_magnitude) result += T(1) << shift;
  // exact=FALSE: fractional input never raises IXC; old sticky IXC remains.
  return result;
}

template <bool toward_plus>
DEF_SEM(ScalarRoundIntegral32, V128W dst, R32 src) {
  UWriteV32(dst, (AArch64RoundIntegralBits<uint32_t, toward_plus>(
                    state, Read(src))));
  return memory;
}
template <bool toward_plus>
DEF_SEM(ScalarRoundIntegral64, V128W dst, R64 src) {
  UWriteV64(dst, (AArch64RoundIntegralBits<uint64_t, toward_plus>(
                    state, Read(src))));
  return memory;
}
}  // namespace

DEF_ISEL(FRINTP_S_FLOATDP1) = ScalarRoundIntegral32<true>;
DEF_ISEL(FRINTP_D_FLOATDP1) = ScalarRoundIntegral64<true>;
DEF_ISEL(FRINTM_S_FLOATDP1) = ScalarRoundIntegral32<false>;
DEF_ISEL(FRINTM_D_FLOATDP1) = ScalarRoundIntegral64<false>;
