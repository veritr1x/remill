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

/* STP of Q registers (offset, pre- and post-index), pre- and post-index STP of
 * S and D, and pre- and post-index STR of B, H, S, D and Q. */

TEST_BEGIN(STP_Q_LDSTPAIR_OFF, stp_q_off, 1)
TEST_INPUTS(0)
    add x3, sp, #-256
    stp q0, q1, [x3]
    stp q2, q3, [x3, #64]
    stp q4, q5, [x3, #-32]
TEST_END

TEST_BEGIN(STP_Q_LDSTPAIR_PRE, stp_q_pre, 1)
TEST_INPUTS(0)
    add x3, sp, #-256
    stp q0, q1, [x3, #-64]!
    stp q2, q3, [x3, #32]!
TEST_END

TEST_BEGIN(STP_Q_LDSTPAIR_POST, stp_q_post, 1)
TEST_INPUTS(0)
    add x3, sp, #-256
    stp q0, q1, [x3], #-64
    stp q2, q3, [x3], #32
TEST_END

TEST_BEGIN(STP_D_LDSTPAIR_PRE, stp_d_pre, 1)
TEST_INPUTS(0)
    add x3, sp, #-256
    stp d0, d1, [x3, #-16]!
    stp d2, d3, [x3, #48]!
TEST_END

TEST_BEGIN(STP_D_LDSTPAIR_POST, stp_d_post, 1)
TEST_INPUTS(0)
    add x3, sp, #-256
    stp d0, d1, [x3], #-16
    stp d2, d3, [x3], #48
TEST_END

TEST_BEGIN(STP_S_LDSTPAIR_PRE, stp_s_pre, 1)
TEST_INPUTS(0)
    add x3, sp, #-256
    stp s0, s1, [x3, #-8]!
    stp s2, s3, [x3, #24]!
TEST_END

TEST_BEGIN(STP_S_LDSTPAIR_POST, stp_s_post, 1)
TEST_INPUTS(0)
    add x3, sp, #-256
    stp s0, s1, [x3], #-8
    stp s2, s3, [x3], #24
TEST_END

TEST_BEGIN(STR_Vn_LDST_IMMPRE, str_vn_pre, 1)
TEST_INPUTS(0)
    add x3, sp, #-256
    str b0, [x3, #-1]!
    str h1, [x3, #-6]!
    str s2, [x3, #-12]!
    str d3, [x3, #-24]!
    str d4, [x3, #40]!
TEST_END

TEST_BEGIN(STR_Vn_LDST_IMMPOST, str_vn_post, 1)
TEST_INPUTS(0)
    add x3, sp, #-256
    str b0, [x3], #-1
    str h1, [x3], #-6
    str s2, [x3], #-12
    str d3, [x3], #-24
    str q4, [x3], #-48
    str q5, [x3], #16
TEST_END
