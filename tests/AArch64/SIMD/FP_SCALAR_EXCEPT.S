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

/* The existing scalar forms with the same exception-raising inputs as the
 * vector tests, to tell the vector semantics apart from shared flag handling. */

TEST_BEGIN(FDIV_S_FLOATDP2, fdiv_s_except, 2)
TEST_INPUTS(TEST_INPUTS_FP32_EXCEPT)
    fmov s1, ARG1_32
    fmov s2, ARG2_32
    msr fpsr, xzr
    fdiv s0, s1, s2
TEST_END

TEST_BEGIN(FADD_S_FLOATDP2, fadd_s_except, 2)
TEST_INPUTS(TEST_INPUTS_FP32_EXCEPT)
    fmov s1, ARG1_32
    fmov s2, ARG2_32
    msr fpsr, xzr
    fadd s0, s1, s2
TEST_END

/* Two scalar ops in a row: are the first one's flags kept? */
TEST_BEGIN(FADD_S_FLOATDP2, fadd_s_twice, 2)
TEST_INPUTS(TEST_INPUTS_FP32_EXCEPT)
    fmov s1, ARG1_32
    fmov s2, ARG2_32
    msr fpsr, xzr
    fadd s0, s1, s2
    fadd s3, s2, s2
TEST_END
