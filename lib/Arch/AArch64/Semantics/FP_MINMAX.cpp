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

template <typename T, bool maximum>
ALWAYS_INLINE T MinMaxFPBits(State &state, T lhs, T rhs) {
  constexpr T sign = T(1) << (sizeof(T) * 8 - 1);
  constexpr T fraction = sizeof(T) == 4 ? T(0x007fffff) : T(0x000fffffffffffffULL);
  constexpr T exponent = ~(sign | fraction);
  constexpr T quiet = (fraction + T(1)) >> 1;

  // FPUnpack runs on both inputs before FPProcessNaNs. Flushing preserves the
  // sign of zero and raises IDC even when the other operand is a NaN.
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
    // Signaling NaNs take priority, then quiet NaNs; ties choose operand one.
    const T selected = lhs_signaling ? lhs : rhs_signaling ? rhs : lhs_nan ? lhs : rhs;
    return selected | quiet;
  }

  if (!(lhs & ~sign) && !(rhs & ~sign)) {
    // Equal signed zeros: MIN chooses -0, MAX chooses +0 independently of
    // operand order. This also handles two identically signed zeros.
    return maximum ? (lhs & rhs & sign) : ((lhs | rhs) & sign);
  }
  bool lhs_greater;
  if ((lhs ^ rhs) & sign) lhs_greater = !(lhs & sign);
  else lhs_greater = (lhs & sign) ? lhs < rhs : lhs > rhs;
  return maximum ? (lhs_greater ? lhs : rhs) : (lhs_greater ? rhs : lhs);
}

#define FP_MINMAX_SEMANTICS(size) \
  template <size_t lanes, bool maximum, typename S> \
  DEF_SEM(MinMaxFP##size, V128W dst, S src1, S src2) { \
    auto lhs = UReadV##size(src1); \
    auto rhs = UReadV##size(src2); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) { \
      result.elems[i] = MinMaxFPBits<uint##size##_t, maximum>(state, lhs.elems[i], rhs.elems[i]); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  }
FP_MINMAX_SEMANTICS(32)
FP_MINMAX_SEMANTICS(64)
#undef FP_MINMAX_SEMANTICS
}  // namespace

#define FP_MINMAX_ISEL(op, maximum) \
  DEF_ISEL(op##_ASIMDSAME_ONLY_2S) = MinMaxFP32<2, maximum, V64>; \
  DEF_ISEL(op##_ASIMDSAME_ONLY_4S) = MinMaxFP32<4, maximum, V128>; \
  DEF_ISEL(op##_ASIMDSAME_ONLY_2D) = MinMaxFP64<2, maximum, V128>; \
  DEF_ISEL(op##_S_FLOATDP2) = MinMaxFP32<1, maximum, V32>; \
  DEF_ISEL(op##_D_FLOATDP2) = MinMaxFP64<1, maximum, V64>;
FP_MINMAX_ISEL(FMIN, false)
FP_MINMAX_ISEL(FMAX, true)
#undef FP_MINMAX_ISEL

namespace {
// FEAT_AFP is disabled in the qualified baseline. FPMinNum/FPMaxNum first
// unpack BOTH operands, then replace a lone quiet NaN with +/-Infinity.
// Keep the existing ordinary MinMaxFPBits implementation and selectors exact.
template <typename T, bool maximum>
ALWAYS_INLINE T MinMaxNumFPBits(State &state, T lhs, T rhs) {
  constexpr T sign = T(1) << (sizeof(T) * 8 - 1);
  constexpr T fraction = sizeof(T) == 4 ? T(0x007fffff) : T(0x000fffffffffffffULL);
  constexpr T exponent = ~(sign | fraction);
  constexpr T quiet = (fraction + T(1)) >> 1;
  if (state.fpcr.fz) {
    if (!(lhs & exponent) && (lhs & fraction)) { state.sr.idc = 1; lhs &= sign; }
    if (!(rhs & exponent) && (rhs & fraction)) { state.sr.idc = 1; rhs &= sign; }
  }
  const bool lhs_qnan = (lhs & exponent) == exponent && (lhs & quiet);
  const bool rhs_qnan = (rhs & exponent) == exponent && (rhs & quiet);
  const T infinity = exponent | (maximum ? sign : T(0));
  if (lhs_qnan && !rhs_qnan) lhs = infinity;
  else if (!lhs_qnan && rhs_qnan) rhs = infinity;
  return MinMaxFPBits<T, maximum>(state, lhs, rhs);
}
#define FP_NUMERIC_MINMAX_SEMANTICS(size) \
  template <bool maximum, typename S> \
  DEF_SEM(MinMaxNumFP##size, V128W dst, S src1, S src2) { \
    const auto lhs = UReadV##size(src1); \
    const auto rhs = UReadV##size(src2); \
    auto result = UClearV##size(UReadV##size(dst)); \
    result.elems[0] = MinMaxNumFPBits<uint##size##_t, maximum>(state, lhs.elems[0], rhs.elems[0]); \
    UWriteV##size(dst, result); \
    return memory; \
  }
FP_NUMERIC_MINMAX_SEMANTICS(32)
FP_NUMERIC_MINMAX_SEMANTICS(64)
#undef FP_NUMERIC_MINMAX_SEMANTICS
}  // namespace
DEF_ISEL(FMINNM_S_FLOATDP2) = MinMaxNumFP32<false, V32>;
DEF_ISEL(FMINNM_D_FLOATDP2) = MinMaxNumFP64<false, V64>;
DEF_ISEL(FMAXNM_S_FLOATDP2) = MinMaxNumFP32<true, V32>;
DEF_ISEL(FMAXNM_D_FLOATDP2) = MinMaxNumFP64<true, V64>;
