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
template <size_t lanes, bool extend, typename... Tables>
DEF_SEM(TableLookup, V128W dst, V128 indexes, Tables... sources) {
  auto indexes_vec = UReadV8(indexes);
  auto previous = UReadV8(dst);
  auto result = UClearV8(previous);
  // Snapshot every source before the destination is written: the destination
  // may overlap either the indexes or any register in the table list.
  decltype(indexes_vec) table[] = {UReadV8(sources)...};
  _Pragma("unroll") for (size_t i = 0; i < lanes; ++i) {
    auto index = UExtractV8(indexes_vec, i);
    uint8_t value = 0;
    if (index < sizeof...(Tables) * 16) {
      value = UExtractV8(table[index / 16], index % 16);
    } else if (extend) {
      value = UExtractV8(previous, i);
    }
    result = UInsertV8(result, i, value);
  }
  UWriteV8(dst, result);
  return memory;
}
}  // namespace

#define TABLE_ISEL(op, suffix, extend, ...) \
  DEF_ISEL(op##_ASIMDTBL_##suffix##_8B) = TableLookup<8, extend, __VA_ARGS__>; \
  DEF_ISEL(op##_ASIMDTBL_##suffix##_16B) = TableLookup<16, extend, __VA_ARGS__>;
TABLE_ISEL(TBL, L1_1, false, V128)
TABLE_ISEL(TBL, L2_2, false, V128, V128)
TABLE_ISEL(TBL, L3_3, false, V128, V128, V128)
TABLE_ISEL(TBL, L4_4, false, V128, V128, V128, V128)
TABLE_ISEL(TBX, L1_1, true, V128)
TABLE_ISEL(TBX, L2_2, true, V128, V128)
TABLE_ISEL(TBX, L3_3, true, V128, V128, V128)
TABLE_ISEL(TBX, L4_4, true, V128, V128, V128, V128)
#undef TABLE_ISEL
