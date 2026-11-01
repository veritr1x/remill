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

#define INTEGER_SHIFT(size) \
  template <size_t lanes> \
  DEF_SEM(IntegerUSHL##size, V128W dst, V128 src, V128 shifts) { \
    auto values = UReadV##size(src); \
    auto amounts = UReadV##size(shifts); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) { \
      const uint64_t value = UExtractV##size(values, i); \
      /* The signed low byte, not the full element, controls the shift. */ \
      const int shift = static_cast<int8_t>(UExtractV##size(amounts, i)); \
      uint##size##_t shifted = 0; \
      /* Guard each shift: shifts by the element size or more produce zero. */ \
      if (shift >= 0 && shift < size) shifted = static_cast<uint##size##_t>(value << shift); \
      else if (shift < 0 && -shift < size) shifted = static_cast<uint##size##_t>(value >> -shift); \
      result = UInsertV##size(result, i, shifted); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  } \
  template <size_t lanes> \
  DEF_SEM(IntegerSHL##size, V128W dst, V128 src, I64 amount) { \
    auto values = UReadV##size(src); \
    auto result = UClearV##size(UReadV##size(dst)); \
    const auto shift = Read(amount); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) { \
      const uint64_t value = UExtractV##size(values, i); \
      const auto shifted = static_cast<uint##size##_t>(value << shift); \
      result = UInsertV##size(result, i, shifted); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  }

INTEGER_SHIFT(8)
INTEGER_SHIFT(16)
INTEGER_SHIFT(32)
INTEGER_SHIFT(64)
#undef INTEGER_SHIFT

#define INTEGER_RIGHT_SHIFT(size) \
  template <size_t lanes, bool sign_extend> \
  DEF_SEM(IntegerSHR##size, V128W dst, V128 src, I64 amount) { \
    auto values = UReadV##size(src); \
    auto result = UClearV##size(UReadV##size(dst)); \
    const auto shift = Read(amount); \
    constexpr uint64_t mask = UINT64_MAX >> (64 - size); \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) { \
      const uint64_t value = UExtractV##size(values, i); \
      /* Legal shifts include the entire element width. Never shift by 64. */ \
      uint64_t shifted = shift == size ? 0 : value >> shift; \
      if (sign_extend && (value & (UINT64_C(1) << (size - 1)))) \
        shifted |= shift == size ? mask : (mask << (size - shift)) & mask; \
      result = UInsertV##size(result, i, static_cast<uint##size##_t>(shifted)); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  }
INTEGER_RIGHT_SHIFT(8)
INTEGER_RIGHT_SHIFT(16)
INTEGER_RIGHT_SHIFT(32)
INTEGER_RIGHT_SHIFT(64)
#undef INTEGER_RIGHT_SHIFT

#define INTEGER_NARROW(narrow, wide) \
  template <bool high> \
  DEF_SEM(IntegerXTN##narrow, V128W dst, V128 src) { \
    auto values = UReadV##wide(src); \
    auto previous = UReadV##narrow(dst); \
    auto result = high ? previous : UClearV##narrow(previous); \
    constexpr size_t count = 64 / narrow; \
    _Pragma("unroll") for (size_t i = 0; i < count; ++i) { \
      const auto value = static_cast<uint##narrow##_t>(UExtractV##wide(values, i)); \
      result = UInsertV##narrow(result, i + (high ? count : 0), value); \
    } \
    UWriteV##narrow(dst, result); \
    return memory; \
  }
INTEGER_NARROW(8, 16)
INTEGER_NARROW(16, 32)
INTEGER_NARROW(32, 64)
#undef INTEGER_NARROW

}  // namespace

#define INTEGER_SHIFT_ISEL(arr, size, lanes) \
  DEF_ISEL(USHL_ASIMDSAME_ONLY_##arr) = IntegerUSHL##size<lanes>; \
  DEF_ISEL(SHL_ASIMDSHF_R_##arr) = IntegerSHL##size<lanes>; \
  DEF_ISEL(USHR_ASIMDSHF_R_##arr) = IntegerSHR##size<lanes, false>; \
  DEF_ISEL(SSHR_ASIMDSHF_R_##arr) = IntegerSHR##size<lanes, true>;
INTEGER_SHIFT_ISEL(8B, 8, 8)
INTEGER_SHIFT_ISEL(16B, 8, 16)
INTEGER_SHIFT_ISEL(4H, 16, 4)
INTEGER_SHIFT_ISEL(8H, 16, 8)
INTEGER_SHIFT_ISEL(2S, 32, 2)
INTEGER_SHIFT_ISEL(4S, 32, 4)
INTEGER_SHIFT_ISEL(2D, 64, 2)
#undef INTEGER_SHIFT_ISEL

DEF_ISEL(USHL_ASISDSAME_ONLY) = IntegerUSHL64<1>;
DEF_ISEL(SHL_ASISDSHF_R) = IntegerSHL64<1>;
DEF_ISEL(SSHR_ASISDSHF_R) = IntegerSHR64<1, true>;

DEF_ISEL(XTN_ASIMDMISC_N_8B) = IntegerXTN8<false>;
DEF_ISEL(XTN_ASIMDMISC_N_16B) = IntegerXTN8<true>;
DEF_ISEL(XTN_ASIMDMISC_N_4H) = IntegerXTN16<false>;
DEF_ISEL(XTN_ASIMDMISC_N_8H) = IntegerXTN16<true>;
DEF_ISEL(XTN_ASIMDMISC_N_2S) = IntegerXTN32<false>;
DEF_ISEL(XTN_ASIMDMISC_N_4S) = IntegerXTN32<true>;
