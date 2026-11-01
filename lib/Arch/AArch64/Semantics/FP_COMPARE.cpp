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
enum class FPComparison { kEqual, kGreater, kGreaterEqual, kLess, kLessEqual };

template <typename T, FPComparison compare>
ALWAYS_INLINE bool CompareFPBits(State &state, T lhs, T rhs) {
  constexpr T sign = T(1) << (sizeof(T) * 8 - 1);
  constexpr T fraction = sizeof(T) == 4 ? T(0x007fffff) : T(0x000fffffffffffffULL);
  constexpr T exponent = ~(sign | fraction);
  constexpr T quiet = (fraction + T(1)) >> 1;

  // FPUnpack flushes input subnormals to signed zero and accumulates IDC when
  // FPCR.FZ is set. Do this before NaN processing for both operands.
  if (state.fpcr.fz) {
    if (!(lhs & exponent) && (lhs & fraction)) { state.sr.idc = 1; lhs &= sign; }
    if (!(rhs & exponent) && (rhs & fraction)) { state.sr.idc = 1; rhs &= sign; }
  }
  const bool lhs_nan = (lhs & exponent) == exponent && (lhs & fraction);
  const bool rhs_nan = (rhs & exponent) == exponent && (rhs & fraction);
  if (lhs_nan || rhs_nan) {
    // Equality is a quiet comparison: only signaling NaNs raise IOC. Ordered
    // GT/GE/LT/LE comparisons signal for every NaN, including quiet NaNs.
    if (compare != FPComparison::kEqual || (lhs_nan && !(lhs & quiet)) ||
        (rhs_nan && !(rhs & quiet))) state.sr.ioc = 1;
    return false;
  }

  const bool equal = lhs == rhs || (!(lhs & ~sign) && !(rhs & ~sign));
  bool greater = false;
  if (!equal) {
    if ((lhs ^ rhs) & sign) greater = !(lhs & sign);
    else greater = (lhs & sign) ? lhs < rhs : lhs > rhs;
  }
  if (compare == FPComparison::kEqual) return equal;
  if (compare == FPComparison::kGreater) return greater;
  if (compare == FPComparison::kGreaterEqual) return equal || greater;
  if (compare == FPComparison::kLess) return !equal && !greater;
  return !greater;
}

#define FP_COMPARE_SEMANTICS(size) \
  template <size_t lanes, FPComparison compare, typename S> \
  DEF_SEM(CompareFP##size, V128W dst, S src1, S src2) { \
    auto lhs = UReadV##size(src1); \
    auto rhs = UReadV##size(src2); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) { \
      bool value = CompareFPBits<uint##size##_t, compare>(state, lhs.elems[i], rhs.elems[i]); \
      result.elems[i] = value ? ~uint##size##_t(0) : uint##size##_t(0); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  } \
  template <size_t lanes, FPComparison compare, typename S> \
  DEF_SEM(CompareFPZero##size, V128W dst, S src) { \
    auto lhs = UReadV##size(src); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) { \
      bool value = CompareFPBits<uint##size##_t, compare>(state, lhs.elems[i], uint##size##_t(0)); \
      result.elems[i] = value ? ~uint##size##_t(0) : uint##size##_t(0); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  }
FP_COMPARE_SEMANTICS(32)
FP_COMPARE_SEMANTICS(64)
#undef FP_COMPARE_SEMANTICS
}  // namespace

#define FP_COMPARE_REGISTER_ISEL(op, compare) \
  DEF_ISEL(op##_ASIMDSAME_ONLY_2S) = CompareFP32<2, FPComparison::compare, V64>; \
  DEF_ISEL(op##_ASIMDSAME_ONLY_4S) = CompareFP32<4, FPComparison::compare, V128>; \
  DEF_ISEL(op##_ASIMDSAME_ONLY_2D) = CompareFP64<2, FPComparison::compare, V128>; \
  DEF_ISEL(op##_ASISDSAME_ONLY_S) = CompareFP32<1, FPComparison::compare, V32>; \
  DEF_ISEL(op##_ASISDSAME_ONLY_D) = CompareFP64<1, FPComparison::compare, V64>;
FP_COMPARE_REGISTER_ISEL(FCMEQ, kEqual)
FP_COMPARE_REGISTER_ISEL(FCMGT, kGreater)
FP_COMPARE_REGISTER_ISEL(FCMGE, kGreaterEqual)
#undef FP_COMPARE_REGISTER_ISEL

#define FP_COMPARE_ZERO_ISEL(op, compare) \
  DEF_ISEL(op##_ASIMDMISC_FZ_2S) = CompareFPZero32<2, FPComparison::compare, V64>; \
  DEF_ISEL(op##_ASIMDMISC_FZ_4S) = CompareFPZero32<4, FPComparison::compare, V128>; \
  DEF_ISEL(op##_ASIMDMISC_FZ_2D) = CompareFPZero64<2, FPComparison::compare, V128>; \
  DEF_ISEL(op##_ASISDMISC_FZ_S) = CompareFPZero32<1, FPComparison::compare, V32>; \
  DEF_ISEL(op##_ASISDMISC_FZ_D) = CompareFPZero64<1, FPComparison::compare, V64>;
FP_COMPARE_ZERO_ISEL(FCMEQ, kEqual)
FP_COMPARE_ZERO_ISEL(FCMGT, kGreater)
FP_COMPARE_ZERO_ISEL(FCMGE, kGreaterEqual)
FP_COMPARE_ZERO_ISEL(FCMLT, kLess)
FP_COMPARE_ZERO_ISEL(FCMLE, kLessEqual)
#undef FP_COMPARE_ZERO_ISEL
