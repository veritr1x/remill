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

/* Operand pairs plus FPCR: signed zeros, NaN priority, FZ/IDC and DN. */

TEST_BEGIN(FMIN_ASIMDSAME_ONLY_2S, fmin_raw_2s, 3)
TEST_INPUTS(0x80000000, 0, 0, 0, 0x80000000, 0, 0x7fc12345, 0x7f800001, 0, 0x7f800001, 0x7fc12345, 0, 1, 0, 0x01000000, 0x80000001, 0, 0x01000000, 1, 0x7f800001, 0x03000000, 0x7fc12345, 0x3f800000, 0x02000000)
    dup v1.4s, ARG1_32
    dup v2.4s, ARG2_32
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmin v0.2s, v1.2s, v2.2s
    fmin v1.2s, v1.2s, v2.2s
    fmin v2.2s, v1.2s, v2.2s
TEST_END

TEST_BEGIN(FMIN_ASIMDSAME_ONLY_4S, fmin_raw_4s, 3)
TEST_INPUTS(0x80000000, 0, 0, 0, 0x80000000, 0, 0x7fc12345, 0x7f800001, 0, 0x7f800001, 0x7fc12345, 0, 1, 0, 0x01000000, 0x80000001, 0, 0x01000000, 1, 0x7f800001, 0x03000000, 0x7fc12345, 0x3f800000, 0x02000000)
    dup v1.4s, ARG1_32
    dup v2.4s, ARG2_32
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmin v0.4s, v1.4s, v2.4s
    fmin v1.4s, v1.4s, v2.4s
    fmin v2.4s, v1.4s, v2.4s
TEST_END

TEST_BEGIN(FMIN_ASIMDSAME_ONLY_2D, fmin_raw_2d, 3)
TEST_INPUTS(0x8000000000000000, 0, 0, 0, 0x8000000000000000, 0, 0x7ff8123456789abc, 0x7ff0000000000001, 0, 0x7ff0000000000001, 0x7ff8123456789abc, 0, 1, 0, 0x01000000, 0x8000000000000001, 0, 0x01000000, 1, 0x7ff0000000000001, 0x03000000, 0x7ff8123456789abc, 0x3ff0000000000000, 0x02000000)
    dup v1.2d, ARG1_64
    dup v2.2d, ARG2_64
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmin v0.2d, v1.2d, v2.2d
    fmin v1.2d, v1.2d, v2.2d
    fmin v2.2d, v1.2d, v2.2d
TEST_END

TEST_BEGIN(FMIN_S_FLOATDP2, fmin_raw_s, 3)
TEST_INPUTS(0x80000000, 0, 0, 0, 0x80000000, 0, 0x7fc12345, 0x7f800001, 0, 0x7f800001, 0x7fc12345, 0, 1, 0, 0x01000000, 0x80000001, 0, 0x01000000, 1, 0x7f800001, 0x03000000, 0x7fc12345, 0x3f800000, 0x02000000)
    dup v1.4s, ARG1_32
    dup v2.4s, ARG2_32
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmin s0, s1, s2
    fmin s1, s1, s2
    fmin s2, s1, s2
TEST_END

TEST_BEGIN(FMIN_D_FLOATDP2, fmin_raw_d, 3)
TEST_INPUTS(0x8000000000000000, 0, 0, 0, 0x8000000000000000, 0, 0x7ff8123456789abc, 0x7ff0000000000001, 0, 0x7ff0000000000001, 0x7ff8123456789abc, 0, 1, 0, 0x01000000, 0x8000000000000001, 0, 0x01000000, 1, 0x7ff0000000000001, 0x03000000, 0x7ff8123456789abc, 0x3ff0000000000000, 0x02000000)
    dup v1.2d, ARG1_64
    dup v2.2d, ARG2_64
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmin d0, d1, d2
    fmin d1, d1, d2
    fmin d2, d1, d2
TEST_END

TEST_BEGIN(FMAX_ASIMDSAME_ONLY_2S, fmax_raw_2s, 3)
TEST_INPUTS(0x80000000, 0, 0, 0, 0x80000000, 0, 0x7fc12345, 0x7f800001, 0, 0x7f800001, 0x7fc12345, 0, 1, 0, 0x01000000, 0x80000001, 0, 0x01000000, 1, 0x7f800001, 0x03000000, 0x7fc12345, 0x3f800000, 0x02000000)
    dup v1.4s, ARG1_32
    dup v2.4s, ARG2_32
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmax v0.2s, v1.2s, v2.2s
    fmax v1.2s, v1.2s, v2.2s
    fmax v2.2s, v1.2s, v2.2s
TEST_END

TEST_BEGIN(FMAX_ASIMDSAME_ONLY_4S, fmax_raw_4s, 3)
TEST_INPUTS(0x80000000, 0, 0, 0, 0x80000000, 0, 0x7fc12345, 0x7f800001, 0, 0x7f800001, 0x7fc12345, 0, 1, 0, 0x01000000, 0x80000001, 0, 0x01000000, 1, 0x7f800001, 0x03000000, 0x7fc12345, 0x3f800000, 0x02000000)
    dup v1.4s, ARG1_32
    dup v2.4s, ARG2_32
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmax v0.4s, v1.4s, v2.4s
    fmax v1.4s, v1.4s, v2.4s
    fmax v2.4s, v1.4s, v2.4s
TEST_END

TEST_BEGIN(FMAX_ASIMDSAME_ONLY_2D, fmax_raw_2d, 3)
TEST_INPUTS(0x8000000000000000, 0, 0, 0, 0x8000000000000000, 0, 0x7ff8123456789abc, 0x7ff0000000000001, 0, 0x7ff0000000000001, 0x7ff8123456789abc, 0, 1, 0, 0x01000000, 0x8000000000000001, 0, 0x01000000, 1, 0x7ff0000000000001, 0x03000000, 0x7ff8123456789abc, 0x3ff0000000000000, 0x02000000)
    dup v1.2d, ARG1_64
    dup v2.2d, ARG2_64
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmax v0.2d, v1.2d, v2.2d
    fmax v1.2d, v1.2d, v2.2d
    fmax v2.2d, v1.2d, v2.2d
TEST_END

TEST_BEGIN(FMAX_S_FLOATDP2, fmax_raw_s, 3)
TEST_INPUTS(0x80000000, 0, 0, 0, 0x80000000, 0, 0x7fc12345, 0x7f800001, 0, 0x7f800001, 0x7fc12345, 0, 1, 0, 0x01000000, 0x80000001, 0, 0x01000000, 1, 0x7f800001, 0x03000000, 0x7fc12345, 0x3f800000, 0x02000000)
    dup v1.4s, ARG1_32
    dup v2.4s, ARG2_32
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmax s0, s1, s2
    fmax s1, s1, s2
    fmax s2, s1, s2
TEST_END

TEST_BEGIN(FMAX_D_FLOATDP2, fmax_raw_d, 3)
TEST_INPUTS(0x8000000000000000, 0, 0, 0, 0x8000000000000000, 0, 0x7ff8123456789abc, 0x7ff0000000000001, 0, 0x7ff0000000000001, 0x7ff8123456789abc, 0, 1, 0, 0x01000000, 0x8000000000000001, 0, 0x01000000, 1, 0x7ff0000000000001, 0x03000000, 0x7ff8123456789abc, 0x3ff0000000000000, 0x02000000)
    dup v1.2d, ARG1_64
    dup v2.2d, ARG2_64
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmax d0, d1, d2
    fmax d1, d1, d2
    fmax d2, d1, d2
TEST_END
