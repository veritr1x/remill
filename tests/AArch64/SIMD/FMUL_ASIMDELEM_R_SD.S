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

/* FMUL by element, vector and scalar, every index; v16-v31 exercise the M bit. */

TEST_BEGIN(FMUL_ASIMDELEM_R_SD_4S, fmul_elem_4s, 2)
TEST_INPUTS(TEST_INPUTS_FP32_EXCEPT)
    LOAD_LANES_32
    mov v20.16b, v2.16b
    fmul v0.4s, v1.4s, v2.s[0]
    fmul v3.4s, v1.4s, v2.s[1]
    fmul v4.4s, v1.4s, v2.s[2]
    fmul v5.4s, v1.4s, v20.s[3]
TEST_END

TEST_BEGIN(FMUL_ASIMDELEM_R_SD_2S, fmul_elem_2s, 2)
TEST_INPUTS(TEST_INPUTS_FP32_EXCEPT)
    LOAD_LANES_32
    fmul v0.2s, v1.2s, v2.s[1]
    fmul v3.2s, v1.2s, v2.s[2]
TEST_END

TEST_BEGIN(FMUL_ASIMDELEM_R_SD_2D, fmul_elem_2d, 2)
TEST_INPUTS(TEST_INPUTS_FP64_EXCEPT)
    LOAD_LANES_64
    mov v17.16b, v2.16b
    fmul v0.2d, v1.2d, v2.d[0]
    fmul v3.2d, v1.2d, v17.d[1]
TEST_END

TEST_BEGIN(FMUL_ASISDELEM_R_SD_S, fmul_elem_s, 2)
TEST_INPUTS(TEST_INPUTS_FP32_EXCEPT)
    LOAD_LANES_32
    fmul s0, s1, v2.s[0]
    fmul s3, s1, v2.s[2]
    fmul s4, s2, v1.s[1]
TEST_END

TEST_BEGIN(FMUL_ASISDELEM_R_SD_D, fmul_elem_d, 2)
TEST_INPUTS(TEST_INPUTS_FP64_EXCEPT)
    LOAD_LANES_64
    fmul d0, d1, v2.d[0]
    fmul d3, d2, v1.d[1]
TEST_END
