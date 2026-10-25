/*
 * Copyright (c) 2026 Trail of Bits, Inc.
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

/* Vector FADD, FSUB, FMUL, FDIV and FADDP, with every FPSR exception. */

/* 1.0, 3.0, inf, -inf, qNaN, sNaN, max, min normal, smallest denormal, 0.0, -0.0, 0.5 */
#define TEST_INPUTS_FP32_EXCEPT \
    0x3f800000, 0x40400000, \
    0x7f800000, 0x00000000, \
    0x7f800000, 0xff800000, \
    0x7fc00000, 0x3f800000, \
    0x7f800001, 0x3f800000, \
    0x7f7fffff, 0x7f7fffff, \
    0x00800000, 0x3f000000, \
    0x00000001, 0x3f000000, \
    0x3f800000, 0x00000000, \
    0x80000000, 0x00000000, \
    0x7fc00001, 0x7f800002

#define TEST_INPUTS_FP64_EXCEPT \
    0x3ff0000000000000, 0x4008000000000000, \
    0x7ff0000000000000, 0x0000000000000000, \
    0x7ff0000000000000, 0xfff0000000000000, \
    0x7ff8000000000000, 0x3ff0000000000000, \
    0x7ff0000000000001, 0x3ff0000000000000, \
    0x7fefffffffffffff, 0x7fefffffffffffff, \
    0x0010000000000000, 0x3fe0000000000000, \
    0x0000000000000001, 0x3fe0000000000000, \
    0x3ff0000000000000, 0x0000000000000000, \
    0x8000000000000000, 0x0000000000000000

/* Lanes of v1 and v2 get both inputs in different orders, so lanes differ. */
#define LOAD_LANES_32 \
    dup v1.4s, ARG1_32; \
    dup v2.4s, ARG2_32; \
    ins v1.s[1], ARG2_32; \
    ins v2.s[2], ARG1_32; \
    msr fpsr, xzr

#define LOAD_LANES_64 \
    dup v1.2d, ARG1_64; \
    dup v2.2d, ARG2_64; \
    ins v1.d[1], ARG2_64; \
    msr fpsr, xzr

/* Expands to one line, so every statement ends in `;`, like Tests.S's macros. */
#define FP_SAME_TESTS(op, name) \
TEST_BEGIN(name##_ASIMDSAME_ONLY_2S, op##_2s, 2) ; \
TEST_INPUTS(TEST_INPUTS_FP32_EXCEPT) ; \
    LOAD_LANES_32 ; \
    op v0.2s, v1.2s, v2.2s ; \
TEST_END ; \
\
TEST_BEGIN(name##_ASIMDSAME_ONLY_4S, op##_4s, 2) ; \
TEST_INPUTS(TEST_INPUTS_FP32_EXCEPT) ; \
    LOAD_LANES_32 ; \
    op v0.4s, v1.4s, v2.4s ; \
TEST_END ; \
\
TEST_BEGIN(name##_ASIMDSAME_ONLY_2D, op##_2d, 2) ; \
TEST_INPUTS(TEST_INPUTS_FP64_EXCEPT) ; \
    LOAD_LANES_64 ; \
    op v0.2d, v1.2d, v2.2d ; \
TEST_END

FP_SAME_TESTS(fadd, FADD)
FP_SAME_TESTS(fsub, FSUB)
FP_SAME_TESTS(fmul, FMUL)
FP_SAME_TESTS(fdiv, FDIV)
FP_SAME_TESTS(faddp, FADDP)

/* The destination is also a source. */
TEST_BEGIN(FMUL_ASIMDSAME_ONLY_4S, fmul_4s_inplace, 2)
TEST_INPUTS(TEST_INPUTS_FP32_EXCEPT)
    LOAD_LANES_32
    fmul v1.4s, v1.4s, v2.4s
TEST_END
