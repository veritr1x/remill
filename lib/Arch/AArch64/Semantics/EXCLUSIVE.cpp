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

// These side-effecting runtime calls keep reservations outside lifted trace
// allocas. The runtime must synchronize ordinary memory writes and provider
// writes with the same reservation protocol, including the access shape.
extern "C" uint64_t __remill_aarch64_load_exclusive(State &, addr_t, uint64_t, uint64_t);
extern "C" uint32_t __remill_aarch64_store_exclusive(State &, addr_t, uint64_t, uint64_t, uint64_t);
extern "C" void __remill_aarch64_clear_exclusive(State &);
extern "C" void __remill_aarch64_load_exclusive_pair(State &, addr_t, uint64_t, uint64_t, uint64_t *, uint64_t *);
extern "C" uint32_t __remill_aarch64_store_exclusive_pair(State &, addr_t, uint64_t, uint64_t, uint64_t, uint64_t);

namespace {
template <typename D, typename S, unsigned size, bool acquire>
DEF_SEM(ExclusiveLoad, D dst, S src) {
  const auto value = __remill_aarch64_load_exclusive(state, AddressOf(src), size, acquire);
  WriteZExt(dst, value);
  return memory;
}
template <typename S, typename D, unsigned size, bool release>
DEF_SEM(ExclusiveStore, R32W status, S src, D dst) {
  const auto value = static_cast<uint64_t>(Read(src));
  const auto result = __remill_aarch64_store_exclusive(state, AddressOf(dst), value, size, release);
  WriteZExt(status, result);
  return memory;
}
DEF_SEM(ClearExclusive) {
  __remill_aarch64_clear_exclusive(state);
  return memory;
}
template <typename D, typename S, unsigned element_size, bool acquire>
DEF_SEM(ExclusiveLoadPair, D dst1, D dst2, S src) {
  uint64_t first = 0, second = 0;
  __remill_aarch64_load_exclusive_pair(state, AddressOf(src), element_size, acquire, &first, &second);
  WriteZExt(dst1, first);
  WriteZExt(dst2, second);
  return memory;
}
template <typename S, typename D, unsigned element_size, bool release>
DEF_SEM(ExclusiveStorePair, R32W status, S src1, S src2, D dst) {
  const auto first = static_cast<uint64_t>(Read(src1));
  const auto second = static_cast<uint64_t>(Read(src2));
  const auto result = __remill_aarch64_store_exclusive_pair(state, AddressOf(dst), first, second, element_size, release);
  WriteZExt(status, result);
  return memory;
}
}  // namespace

DEF_ISEL(LDXRB_LR32_LDSTEXCL) = ExclusiveLoad<R32W, M8, 1, false>;
DEF_ISEL(LDXRH_LR32_LDSTEXCL) = ExclusiveLoad<R32W, M16, 2, false>;
DEF_ISEL(LDXR_LR32_LDSTEXCL) = ExclusiveLoad<R32W, M32, 4, false>;
DEF_ISEL(LDXR_LR64_LDSTEXCL) = ExclusiveLoad<R64W, M64, 8, false>;
DEF_ISEL(LDAXRB_LR32_LDSTEXCL) = ExclusiveLoad<R32W, M8, 1, true>;
DEF_ISEL(LDAXRH_LR32_LDSTEXCL) = ExclusiveLoad<R32W, M16, 2, true>;
DEF_ISEL(LDAXR_LR32_LDSTEXCL) = ExclusiveLoad<R32W, M32, 4, true>;
DEF_ISEL(LDAXR_LR64_LDSTEXCL) = ExclusiveLoad<R64W, M64, 8, true>;
DEF_ISEL(STXRB_SR32_LDSTEXCL) = ExclusiveStore<R32, M8W, 1, false>;
DEF_ISEL(STXRH_SR32_LDSTEXCL) = ExclusiveStore<R32, M16W, 2, false>;
DEF_ISEL(STXR_SR32_LDSTEXCL) = ExclusiveStore<R32, M32W, 4, false>;
DEF_ISEL(STXR_SR64_LDSTEXCL) = ExclusiveStore<R64, M64W, 8, false>;
DEF_ISEL(STLXRB_SR32_LDSTEXCL) = ExclusiveStore<R32, M8W, 1, true>;
DEF_ISEL(STLXRH_SR32_LDSTEXCL) = ExclusiveStore<R32, M16W, 2, true>;
DEF_ISEL(STLXR_SR32_LDSTEXCL) = ExclusiveStore<R32, M32W, 4, true>;
DEF_ISEL(STLXR_SR64_LDSTEXCL) = ExclusiveStore<R64, M64W, 8, true>;
DEF_ISEL(CLREX_BN_SYSTEM) = ClearExclusive;
DEF_ISEL(LDXP_LP32_LDSTEXCL) = ExclusiveLoadPair<R32W, MV64, 4, false>;
DEF_ISEL(LDXP_LP64_LDSTEXCL) = ExclusiveLoadPair<R64W, MV128, 8, false>;
DEF_ISEL(LDAXP_LP32_LDSTEXCL) = ExclusiveLoadPair<R32W, MV64, 4, true>;
DEF_ISEL(LDAXP_LP64_LDSTEXCL) = ExclusiveLoadPair<R64W, MV128, 8, true>;
DEF_ISEL(STXP_SP32_LDSTEXCL) = ExclusiveStorePair<R32, MV64W, 4, false>;
DEF_ISEL(STXP_SP64_LDSTEXCL) = ExclusiveStorePair<R64, MV128W, 8, false>;
DEF_ISEL(STLXP_SP32_LDSTEXCL) = ExclusiveStorePair<R32, MV64W, 4, true>;
DEF_ISEL(STLXP_SP64_LDSTEXCL) = ExclusiveStorePair<R64, MV128W, 8, true>;
