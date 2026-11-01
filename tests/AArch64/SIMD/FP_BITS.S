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

/* Pure bit operations: NaN payloads and signaling state must be preserved. */

TEST_BEGIN(FABS_ASIMDMISCFP16_R_4H, fabs_bits_4h, 1)
TEST_INPUTS(0, 0x8000, 0x7c00, 0xfc00, 0x7e55, 0xfe55, 0x7c01, 0xfc01, 1, 0x8001)
    dup v1.8h, ARG1_32
    msr fpsr, xzr
    .inst 0xef8f820
    .inst 0xef8f821
TEST_END

TEST_BEGIN(FNEG_ASIMDMISCFP16_R_4H, fneg_bits_4h, 1)
TEST_INPUTS(0, 0x8000, 0x7c00, 0xfc00, 0x7e55, 0xfe55, 0x7c01, 0xfc01, 1, 0x8001)
    dup v1.8h, ARG1_32
    msr fpsr, xzr
    .inst 0x2ef8f820
    .inst 0x2ef8f821
TEST_END

TEST_BEGIN(FMOV_ASIMDIMM_H_H_4H, fmov_bits_4h, 1)
TEST_INPUTS(0)
    .inst 0xf00fc01
    .inst 0xf03fe02
    .inst 0xf07ffe3
TEST_END

TEST_BEGIN(FABS_ASIMDMISCFP16_R_8H, fabs_bits_8h, 1)
TEST_INPUTS(0, 0x8000, 0x7c00, 0xfc00, 0x7e55, 0xfe55, 0x7c01, 0xfc01, 1, 0x8001)
    dup v1.8h, ARG1_32
    msr fpsr, xzr
    .inst 0x4ef8f820
    .inst 0x4ef8f821
TEST_END

TEST_BEGIN(FNEG_ASIMDMISCFP16_R_8H, fneg_bits_8h, 1)
TEST_INPUTS(0, 0x8000, 0x7c00, 0xfc00, 0x7e55, 0xfe55, 0x7c01, 0xfc01, 1, 0x8001)
    dup v1.8h, ARG1_32
    msr fpsr, xzr
    .inst 0x6ef8f820
    .inst 0x6ef8f821
TEST_END

TEST_BEGIN(FMOV_ASIMDIMM_H_H_8H, fmov_bits_8h, 1)
TEST_INPUTS(0)
    .inst 0x4f00fc01
    .inst 0x4f03fe02
    .inst 0x4f07ffe3
TEST_END

TEST_BEGIN(FABS_ASIMDMISC_R_2S, fabs_bits_2s, 1)
TEST_INPUTS(0, 0x80000000, 0x7f800000, 0xff800000, 0x7fc12345, 0xffc12345, 0x7f800001, 0xff800001, 1, 0x80000001)
    dup v1.4s, ARG1_32
    msr fpsr, xzr
    .inst 0xea0f820
    .inst 0xea0f821
TEST_END

TEST_BEGIN(FNEG_ASIMDMISC_R_2S, fneg_bits_2s, 1)
TEST_INPUTS(0, 0x80000000, 0x7f800000, 0xff800000, 0x7fc12345, 0xffc12345, 0x7f800001, 0xff800001, 1, 0x80000001)
    dup v1.4s, ARG1_32
    msr fpsr, xzr
    .inst 0x2ea0f820
    .inst 0x2ea0f821
TEST_END

TEST_BEGIN(FMOV_ASIMDIMM_S_S_2S, fmov_bits_2s, 1)
TEST_INPUTS(0)
    .inst 0xf00f401
    .inst 0xf03f602
    .inst 0xf07f7e3
TEST_END

TEST_BEGIN(FABS_ASIMDMISC_R_4S, fabs_bits_4s, 1)
TEST_INPUTS(0, 0x80000000, 0x7f800000, 0xff800000, 0x7fc12345, 0xffc12345, 0x7f800001, 0xff800001, 1, 0x80000001)
    dup v1.4s, ARG1_32
    msr fpsr, xzr
    .inst 0x4ea0f820
    .inst 0x4ea0f821
TEST_END

TEST_BEGIN(FNEG_ASIMDMISC_R_4S, fneg_bits_4s, 1)
TEST_INPUTS(0, 0x80000000, 0x7f800000, 0xff800000, 0x7fc12345, 0xffc12345, 0x7f800001, 0xff800001, 1, 0x80000001)
    dup v1.4s, ARG1_32
    msr fpsr, xzr
    .inst 0x6ea0f820
    .inst 0x6ea0f821
TEST_END

TEST_BEGIN(FMOV_ASIMDIMM_S_S_4S, fmov_bits_4s, 1)
TEST_INPUTS(0)
    .inst 0x4f00f401
    .inst 0x4f03f602
    .inst 0x4f07f7e3
TEST_END

TEST_BEGIN(FABS_ASIMDMISC_R_2D, fabs_bits_2d, 1)
TEST_INPUTS(0, 0x8000000000000000, 0x7ff0000000000000, 0xfff0000000000000, 0x7ff8123456789abc, 0xfff8123456789abc, 0x7ff0000000000001, 0xfff0000000000001, 1, 0x8000000000000001)
    dup v1.2d, ARG1_64
    msr fpsr, xzr
    .inst 0x4ee0f820
    .inst 0x4ee0f821
TEST_END

TEST_BEGIN(FNEG_ASIMDMISC_R_2D, fneg_bits_2d, 1)
TEST_INPUTS(0, 0x8000000000000000, 0x7ff0000000000000, 0xfff0000000000000, 0x7ff8123456789abc, 0xfff8123456789abc, 0x7ff0000000000001, 0xfff0000000000001, 1, 0x8000000000000001)
    dup v1.2d, ARG1_64
    msr fpsr, xzr
    .inst 0x6ee0f820
    .inst 0x6ee0f821
TEST_END

TEST_BEGIN(FMOV_ASIMDIMM_D2_D_2D, fmov_bits_2d, 1)
TEST_INPUTS(0)
    .inst 0x6f00f401
    .inst 0x6f03f602
    .inst 0x6f07f7e3
TEST_END
