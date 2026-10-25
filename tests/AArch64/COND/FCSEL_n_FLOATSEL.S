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

/* FCSEL: the flags come from comparing the two inputs, then several conditions
 * pick between the vector registers' random contents. */

TEST_BEGIN(FCSEL_S_FLOATSEL, fcsel_s, 2)
TEST_INPUTS(
    0, 0,
    0, 1,
    1, 0,
    0x80000000, 1,
    0x7fffffff, 0x80000000)
    cmp ARG1_32, ARG2_32
    fcsel s0, s1, s2, eq
    fcsel s3, s4, s5, ne
    fcsel s6, s7, s8, mi
    fcsel s9, s10, s11, gt
    fcsel s12, s13, s14, lo
    fcsel s15, s16, s17, vs
TEST_END

TEST_BEGIN(FCSEL_D_FLOATSEL, fcsel_d, 2)
TEST_INPUTS(
    0, 0,
    0, 1,
    1, 0,
    0x8000000000000000, 1,
    0x7fffffffffffffff, 0x8000000000000000)
    cmp ARG1_64, ARG2_64
    fcsel d0, d1, d2, eq
    fcsel d3, d4, d5, ne
    fcsel d6, d7, d8, mi
    fcsel d9, d10, d11, gt
    fcsel d12, d13, d14, ls
    fcsel d15, d16, d17, vc
TEST_END
