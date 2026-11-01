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

#define INTEGER_PERMUTE(size) \
  template <size_t lanes, size_t part> \
  DEF_SEM(IntegerZIP##size, V128W dst, V128 src1, V128 src2) { \
    auto a = UReadV##size(src1); \
    auto b = UReadV##size(src2); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes / 2; ++i) { \
      const size_t source = i + part * (lanes / 2); \
      result = UInsertV##size(result, 2 * i, UExtractV##size(a, source)); \
      result = UInsertV##size(result, 2 * i + 1, UExtractV##size(b, source)); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  } \
  template <size_t lanes, size_t part> \
  DEF_SEM(IntegerUZP##size, V128W dst, V128 src1, V128 src2) { \
    auto a = UReadV##size(src1); \
    auto b = UReadV##size(src2); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes / 2; ++i) { \
      const size_t source = 2 * i + part; \
      result = UInsertV##size(result, i, UExtractV##size(a, source)); \
      result = UInsertV##size(result, i + lanes / 2, UExtractV##size(b, source)); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  } \
  template <size_t lanes, size_t part> \
  DEF_SEM(IntegerTRN##size, V128W dst, V128 src1, V128 src2) { \
    auto a = UReadV##size(src1); \
    auto b = UReadV##size(src2); \
    auto result = UClearV##size(UReadV##size(dst)); \
    _Pragma("unroll") for (size_t i = 0; i < lanes / 2; ++i) { \
      const size_t source = 2 * i + part; \
      result = UInsertV##size(result, 2 * i, UExtractV##size(a, source)); \
      result = UInsertV##size(result, 2 * i + 1, UExtractV##size(b, source)); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  } \
  template <size_t lanes, size_t container_bits> \
  DEF_SEM(IntegerREV##size, V128W dst, V128 src) { \
    auto values = UReadV##size(src); \
    auto result = UClearV##size(UReadV##size(dst)); \
    constexpr size_t elements = container_bits / size; \
    _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) { \
      const size_t source = (i / elements) * elements + elements - 1 - i % elements; \
      result = UInsertV##size(result, i, UExtractV##size(values, source)); \
    } \
    UWriteV##size(dst, result); \
    return memory; \
  }

INTEGER_PERMUTE(8)
INTEGER_PERMUTE(16)
INTEGER_PERMUTE(32)
INTEGER_PERMUTE(64)
#undef INTEGER_PERMUTE
}  // namespace

#define INTEGER_PERMUTE_ISEL(arr, size, lanes) \
  DEF_ISEL(ZIP1_ASIMDPERM_ONLY_##arr) = IntegerZIP##size<lanes, 0>; \
  DEF_ISEL(ZIP2_ASIMDPERM_ONLY_##arr) = IntegerZIP##size<lanes, 1>; \
  DEF_ISEL(UZP1_ASIMDPERM_ONLY_##arr) = IntegerUZP##size<lanes, 0>; \
  DEF_ISEL(UZP2_ASIMDPERM_ONLY_##arr) = IntegerUZP##size<lanes, 1>; \
  DEF_ISEL(TRN1_ASIMDPERM_ONLY_##arr) = IntegerTRN##size<lanes, 0>; \
  DEF_ISEL(TRN2_ASIMDPERM_ONLY_##arr) = IntegerTRN##size<lanes, 1>;
INTEGER_PERMUTE_ISEL(8B, 8, 8)
INTEGER_PERMUTE_ISEL(16B, 8, 16)
INTEGER_PERMUTE_ISEL(4H, 16, 4)
INTEGER_PERMUTE_ISEL(8H, 16, 8)
INTEGER_PERMUTE_ISEL(2S, 32, 2)
INTEGER_PERMUTE_ISEL(4S, 32, 4)
INTEGER_PERMUTE_ISEL(2D, 64, 2)
#undef INTEGER_PERMUTE_ISEL

#define INTEGER_REV_ISEL(container, arr, size, lanes) \
  DEF_ISEL(REV##container##_ASIMDMISC_R_##arr) = IntegerREV##size<lanes, container>;
INTEGER_REV_ISEL(16, 8B, 8, 8)
INTEGER_REV_ISEL(16, 16B, 8, 16)
INTEGER_REV_ISEL(32, 8B, 8, 8)
INTEGER_REV_ISEL(32, 16B, 8, 16)
INTEGER_REV_ISEL(32, 4H, 16, 4)
INTEGER_REV_ISEL(32, 8H, 16, 8)
INTEGER_REV_ISEL(64, 8B, 8, 8)
INTEGER_REV_ISEL(64, 16B, 8, 16)
INTEGER_REV_ISEL(64, 4H, 16, 4)
INTEGER_REV_ISEL(64, 8H, 16, 8)
INTEGER_REV_ISEL(64, 2S, 32, 2)
INTEGER_REV_ISEL(64, 4S, 32, 4)
#undef INTEGER_REV_ISEL
