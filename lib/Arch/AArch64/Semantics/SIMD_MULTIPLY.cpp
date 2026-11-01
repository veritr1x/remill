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

// Low-half multiplication is identical for signed and unsigned inputs. Widen
// before multiplying so C++ integer promotions cannot overflow signed int for
// H elements. All accumulation and subtraction is unsigned, then truncated.
#define INTEGER_MULTIPLY(size) \
  template <size_t lanes, unsigned operation, bool by_element> \
  DEF_SEM(IntegerMultiply##size, V128W dst, V128 src1, V128 src2, I64 index) { \
    auto lhs = UReadV##size(src1); \
    auto rhs = UReadV##size(src2); \
    auto previous = UReadV##size(dst); \
    auto result = UClearV##size(previous); \
    const size_t element = by_element ? Read(index) : 0; \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) { \
      const uint64_t product = uint64_t(lhs.elems[i]) * uint64_t(rhs.elems[by_element ? element : i]); \
      uint64_t value = product; \
      if constexpr (operation == 1) value = uint64_t(previous.elems[i]) + product; \
      else if constexpr (operation == 2) value = uint64_t(previous.elems[i]) - product; \
      result.elems[i] = static_cast<uint##size##_t>(value); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  } \
  template <size_t lanes, unsigned operation> \
  DEF_SEM(IntegerMultiplyVector##size, V128W dst, V128 src1, V128 src2) { \
    return IntegerMultiply##size<lanes, operation, false>(memory, state, dst, src1, src2, I64{0}); \
  }
INTEGER_MULTIPLY(8)
INTEGER_MULTIPLY(16)
INTEGER_MULTIPLY(32)
#undef INTEGER_MULTIPLY
}  // namespace

#define INTEGER_MULTIPLY_ISEL(op, operation, arr, size, lanes) \
  DEF_ISEL(op##_ASIMDSAME_ONLY_##arr) = IntegerMultiplyVector##size<lanes, operation>;
#define INTEGER_MULTIPLY_ARRANGEMENTS(op, operation) \
  INTEGER_MULTIPLY_ISEL(op, operation, 8B, 8, 8) \
  INTEGER_MULTIPLY_ISEL(op, operation, 16B, 8, 16) \
  INTEGER_MULTIPLY_ISEL(op, operation, 4H, 16, 4) \
  INTEGER_MULTIPLY_ISEL(op, operation, 8H, 16, 8) \
  INTEGER_MULTIPLY_ISEL(op, operation, 2S, 32, 2) \
  INTEGER_MULTIPLY_ISEL(op, operation, 4S, 32, 4) \
  DEF_ISEL(op##_ASIMDELEM_R_4H) = IntegerMultiply16<4, operation, true>; \
  DEF_ISEL(op##_ASIMDELEM_R_8H) = IntegerMultiply16<8, operation, true>; \
  DEF_ISEL(op##_ASIMDELEM_R_2S) = IntegerMultiply32<2, operation, true>; \
  DEF_ISEL(op##_ASIMDELEM_R_4S) = IntegerMultiply32<4, operation, true>;
INTEGER_MULTIPLY_ARRANGEMENTS(MUL, 0)
INTEGER_MULTIPLY_ARRANGEMENTS(MLA, 1)
INTEGER_MULTIPLY_ARRANGEMENTS(MLS, 2)
#undef INTEGER_MULTIPLY_ARRANGEMENTS
#undef INTEGER_MULTIPLY_ISEL
