/*
 * Copyright (c) 2017 Trail of Bits, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

namespace {

DEF_SEM(UCVTF_UInt32ToFloat32, V128W dst, R32 src) {
  auto value = AArch64IntegerToFloatBits<uint32_t, uint32_t, false>(state, Read(src));
  UWriteV32(dst, value);
  return memory;
}

DEF_SEM(UCVTF_UInt32ToFloat64, V128W dst, R32 src) {
  auto value = AArch64IntegerToFloatBits<uint32_t, uint64_t, false>(state, Read(src));
  UWriteV64(dst, value);
  return memory;
}

DEF_SEM(UCVTF_UInt64ToFloat32, V128W dst, R64 src) {
  auto value = AArch64IntegerToFloatBits<uint64_t, uint32_t, false>(state, Read(src));
  UWriteV32(dst, value);
  return memory;
}

DEF_SEM(UCVTF_UInt64ToFloat64, V128W dst, R64 src) {
  auto value = AArch64IntegerToFloatBits<uint64_t, uint64_t, false>(state, Read(src));
  UWriteV64(dst, value);
  return memory;
}

DEF_SEM(FCVTZU_Float32ToUInt32, R32W dst, V32 src) {
  auto raw = UExtractV32(UReadV32(src), 0);
  auto res = AArch64FloatToIntegerBits<uint32_t, uint32_t, false>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

DEF_SEM(FCVTZU_Float32ToUInt64, R64W dst, V32 src) {
  auto raw = UExtractV32(UReadV32(src), 0);
  auto res = AArch64FloatToIntegerBits<uint32_t, uint64_t, false>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

DEF_SEM(FCVTZU_Float64ToUInt32, R32W dst, V64 src) {
  auto raw = UExtractV64(UReadV64(src), 0);
  auto res = AArch64FloatToIntegerBits<uint64_t, uint32_t, false>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

DEF_SEM(FCVTZU_Float64ToUInt64, R64W dst, V64 src) {
  auto raw = UExtractV64(UReadV64(src), 0);
  auto res = AArch64FloatToIntegerBits<uint64_t, uint64_t, false>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

DEF_SEM(FCVTZS_Float32ToSInt32, R32W dst, V32 src) {
  auto raw = UExtractV32(UReadV32(src), 0);
  auto res = AArch64FloatToIntegerBits<uint32_t, uint32_t, true>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

DEF_SEM(FCVTZS_Float32ToSInt64, R64W dst, V32 src) {
  auto raw = UExtractV32(UReadV32(src), 0);
  auto res = AArch64FloatToIntegerBits<uint32_t, uint64_t, true>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

DEF_SEM(FCVTZS_Float64ToSInt32, R32W dst, V64 src) {
  auto raw = UExtractV64(UReadV64(src), 0);
  auto res = AArch64FloatToIntegerBits<uint64_t, uint32_t, true>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

DEF_SEM(FCVTZS_Float64ToSInt64, R64W dst, V64 src) {
  auto raw = UExtractV64(UReadV64(src), 0);
  auto res = AArch64FloatToIntegerBits<uint64_t, uint64_t, true>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

DEF_SEM(FCVT_Float32ToFloat64, V128W dst, V32 src) {
  auto raw = UExtractV32(UReadV32(src), 0);
  auto res = AArch64FloatPrecisionBits<uint32_t, uint64_t>(state, raw);
  UWriteV64(dst, res);
  return memory;
}

DEF_SEM(FCVT_Float64ToFloat32, V128W dst, V64 src) {
  auto raw = UExtractV64(UReadV64(src), 0);
  auto res = AArch64FloatPrecisionBits<uint64_t, uint32_t>(state, raw);
  UWriteV32(dst, res);
  return memory;
}
}  // namespace

// TODO(pag): UCVTF_H32_FLOAT2INT.
// TODO(pag): UCVTF_H64_FLOAT2INT.

DEF_ISEL(UCVTF_S32_FLOAT2INT) = UCVTF_UInt32ToFloat32;
DEF_ISEL(UCVTF_D32_FLOAT2INT) = UCVTF_UInt32ToFloat64;

DEF_ISEL(UCVTF_S64_FLOAT2INT) = UCVTF_UInt64ToFloat32;
DEF_ISEL(UCVTF_D64_FLOAT2INT) = UCVTF_UInt64ToFloat64;

DEF_ISEL(FCVTZU_64S_FLOAT2INT) = FCVTZU_Float32ToUInt64;
DEF_ISEL(FCVTZU_32S_FLOAT2INT) = FCVTZU_Float32ToUInt32;

DEF_ISEL(FCVTZU_32D_FLOAT2INT) = FCVTZU_Float64ToUInt32;
DEF_ISEL(FCVTZU_64D_FLOAT2INT) = FCVTZU_Float64ToUInt64;

DEF_ISEL(FCVTZS_32S_FLOAT2INT) = FCVTZS_Float32ToSInt32;
DEF_ISEL(FCVTZS_64S_FLOAT2INT) = FCVTZS_Float32ToSInt64;

DEF_ISEL(FCVTZS_32D_FLOAT2INT) = FCVTZS_Float64ToSInt32;
DEF_ISEL(FCVTZS_64D_FLOAT2INT) = FCVTZS_Float64ToSInt64;

DEF_ISEL(FCVT_DS_FLOATDP1) = FCVT_Float32ToFloat64;
DEF_ISEL(FCVT_SD_FLOATDP1) = FCVT_Float64ToFloat32;

namespace {

DEF_SEM(SCVTF_Int32ToFloat32, V128W dst, R32 src) {
  auto value = AArch64IntegerToFloatBits<uint32_t, uint32_t, true>(state, Read(src));
  UWriteV32(dst, value);
  return memory;
}

DEF_SEM(SCVTF_Int32ToFloat64, V128W dst, R32 src) {
  auto value = AArch64IntegerToFloatBits<uint32_t, uint64_t, true>(state, Read(src));
  UWriteV64(dst, value);
  return memory;
}

DEF_SEM(SCVTF_Int64ToFloat32, V128W dst, R64 src) {
  auto value = AArch64IntegerToFloatBits<uint64_t, uint32_t, true>(state, Read(src));
  UWriteV32(dst, value);
  return memory;
}

DEF_SEM(SCVTF_Int64ToFloat64, V128W dst, R64 src) {
  auto value = AArch64IntegerToFloatBits<uint64_t, uint64_t, true>(state, Read(src));
  UWriteV64(dst, value);
  return memory;
}

}  // namespace

// TODO(pag): SCVTF_H32_FLOAT2INT.
// TODO(pag): SCVTF_H64_FLOAT2INT.

DEF_ISEL(SCVTF_S32_FLOAT2INT) = SCVTF_Int32ToFloat32;
DEF_ISEL(SCVTF_D32_FLOAT2INT) = SCVTF_Int32ToFloat64;
DEF_ISEL(SCVTF_S64_FLOAT2INT) = SCVTF_Int64ToFloat32;
DEF_ISEL(SCVTF_D64_FLOAT2INT) = SCVTF_Int64ToFloat64;
