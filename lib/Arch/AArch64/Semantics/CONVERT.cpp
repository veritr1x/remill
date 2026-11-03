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

DEF_SEM(FCVTZU_Float32ToUInt32, R32W dst, R32 src) {
  auto raw = Read(src);
  auto res = AArch64FloatToIntegerBits<uint32_t, uint32_t, false>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

DEF_SEM(FCVTZU_Float32ToUInt64, R64W dst, R32 src) {
  auto raw = Read(src);
  auto res = AArch64FloatToIntegerBits<uint32_t, uint64_t, false>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

DEF_SEM(FCVTZU_Float64ToUInt32, R32W dst, R64 src) {
  auto raw = Read(src);
  auto res = AArch64FloatToIntegerBits<uint64_t, uint32_t, false>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

DEF_SEM(FCVTZU_Float64ToUInt64, R64W dst, R64 src) {
  auto raw = Read(src);
  auto res = AArch64FloatToIntegerBits<uint64_t, uint64_t, false>(state, raw);
  WriteZExt(dst, res);
  return memory;
}

// FPToFixed(..., unsigned=TRUE, rounding=ZERO), baseline FP profile.
// Scale the exponent as integer data. Avoid host FP multiplication/casts and
// preserve the established fbits=0 helper and all of its exception behavior.
template <typename In, typename Out>
ALWAYS_INLINE Out AArch64UnsignedFloatToFixedBits(State &state, In raw,
                                                uint64_t fbits) {
  constexpr unsigned fraction_bits = sizeof(In) == 4 ? 23 : 52;
  constexpr unsigned bias = sizeof(In) == 4 ? 127 : 1023;
  constexpr In sign_mask = In(1) << (sizeof(In) * 8 - 1);
  constexpr In fraction_mask = (In(1) << fraction_bits) - In(1);
  constexpr In exponent_mask = ~(sign_mask | fraction_mask);
  const unsigned exponent = unsigned((raw & exponent_mask) >> fraction_bits);
  if (!exponent || (raw & exponent_mask) == exponent_mask) {
    // With fbits<=64 all S/D subnormals still truncate to zero. Delegation
    // preserves input FZ/IDC, IXC, signed zero, and NaN/infinity IOC rules.
    return AArch64FloatToIntegerBits<In, Out, false>(state, raw);
  }
  const uint64_t adjusted = uint64_t(exponent) + fbits;
  if (adjusted >= bias + sizeof(Out) * 8) {
    state.sr.ioc = 1;
    return (raw & sign_mask) ? Out(0) : ~Out(0);
  }
  // The unsigned output bound also prevents overflow of the FP exponent
  // field. Significand bits are unchanged; scaling here is exact.
  const In scaled = (raw & ~exponent_mask) | (In(adjusted) << fraction_bits);
  return AArch64FloatToIntegerBits<In, Out, false>(state, scaled);
}

#define FCVTZU_FIXED(input_bits, output_bits) \
  DEF_SEM(FCVTZU_Fixed##input_bits##ToUInt##output_bits, \
          R##output_bits##W dst, R##input_bits src, I64 fbits) { \
    auto result = AArch64UnsignedFloatToFixedBits< \
        uint##input_bits##_t, uint##output_bits##_t>(state, Read(src), Read(fbits)); \
    WriteZExt(dst, result); \
    return memory; \
  }
FCVTZU_FIXED(32, 32)
FCVTZU_FIXED(32, 64)
FCVTZU_FIXED(64, 32)
FCVTZU_FIXED(64, 64)
#undef FCVTZU_FIXED

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

DEF_ISEL(FCVTZU_32S_FLOAT2FIX) = FCVTZU_Fixed32ToUInt32;
DEF_ISEL(FCVTZU_64S_FLOAT2FIX) = FCVTZU_Fixed32ToUInt64;
DEF_ISEL(FCVTZU_32D_FLOAT2FIX) = FCVTZU_Fixed64ToUInt32;
DEF_ISEL(FCVTZU_64D_FLOAT2FIX) = FCVTZU_Fixed64ToUInt64;

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
