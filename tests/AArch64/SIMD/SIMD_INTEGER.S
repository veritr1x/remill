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

/* Cover Q=0 upper-half clearing, overlap, and initial NZCV preservation. */

TEST_BEGIN(USHL_ASIMDSAME_ONLY_8B, ushl_8b, 1)
TEST_INPUTS(0)
    ushl v3.8b, v4.8b, v5.8b
    ushl v4.8b, v4.8b, v5.8b
    ushl v5.8b, v4.8b, v5.8b
TEST_END

TEST_BEGIN(SHL_ASIMDSHF_R_8B, shl_8b, 1)
TEST_INPUTS(0)
    shl v3.8b, v4.8b, #0
    shl v4.8b, v4.8b, #7
TEST_END

TEST_BEGIN(USHL_ASIMDSAME_ONLY_16B, ushl_16b, 1)
TEST_INPUTS(0)
    ushl v3.16b, v4.16b, v5.16b
    ushl v4.16b, v4.16b, v5.16b
    ushl v5.16b, v4.16b, v5.16b
TEST_END

TEST_BEGIN(SHL_ASIMDSHF_R_16B, shl_16b, 1)
TEST_INPUTS(0)
    shl v3.16b, v4.16b, #0
    shl v4.16b, v4.16b, #7
TEST_END

TEST_BEGIN(USHL_ASIMDSAME_ONLY_4H, ushl_4h, 1)
TEST_INPUTS(0)
    ushl v3.4h, v4.4h, v5.4h
    ushl v4.4h, v4.4h, v5.4h
    ushl v5.4h, v4.4h, v5.4h
TEST_END

TEST_BEGIN(SHL_ASIMDSHF_R_4H, shl_4h, 1)
TEST_INPUTS(0)
    shl v3.4h, v4.4h, #0
    shl v4.4h, v4.4h, #15
TEST_END

TEST_BEGIN(USHL_ASIMDSAME_ONLY_8H, ushl_8h, 1)
TEST_INPUTS(0)
    ushl v3.8h, v4.8h, v5.8h
    ushl v4.8h, v4.8h, v5.8h
    ushl v5.8h, v4.8h, v5.8h
TEST_END

TEST_BEGIN(SHL_ASIMDSHF_R_8H, shl_8h, 1)
TEST_INPUTS(0)
    shl v3.8h, v4.8h, #0
    shl v4.8h, v4.8h, #15
TEST_END

TEST_BEGIN(USHL_ASIMDSAME_ONLY_2S, ushl_2s, 1)
TEST_INPUTS(0)
    ushl v3.2s, v4.2s, v5.2s
    ushl v4.2s, v4.2s, v5.2s
    ushl v5.2s, v4.2s, v5.2s
TEST_END

TEST_BEGIN(SHL_ASIMDSHF_R_2S, shl_2s, 1)
TEST_INPUTS(0)
    shl v3.2s, v4.2s, #0
    shl v4.2s, v4.2s, #31
TEST_END

TEST_BEGIN(USHL_ASIMDSAME_ONLY_4S, ushl_4s, 1)
TEST_INPUTS(0)
    ushl v3.4s, v4.4s, v5.4s
    ushl v4.4s, v4.4s, v5.4s
    ushl v5.4s, v4.4s, v5.4s
TEST_END

TEST_BEGIN(SHL_ASIMDSHF_R_4S, shl_4s, 1)
TEST_INPUTS(0)
    shl v3.4s, v4.4s, #0
    shl v4.4s, v4.4s, #31
TEST_END

TEST_BEGIN(USHL_ASIMDSAME_ONLY_2D, ushl_2d, 1)
TEST_INPUTS(0)
    ushl v3.2d, v4.2d, v5.2d
    ushl v4.2d, v4.2d, v5.2d
    ushl v5.2d, v4.2d, v5.2d
TEST_END

TEST_BEGIN(SHL_ASIMDSHF_R_2D, shl_2d, 1)
TEST_INPUTS(0)
    shl v3.2d, v4.2d, #0
    shl v4.2d, v4.2d, #63
TEST_END

TEST_BEGIN(USHL_ASISDSAME_ONLY, ushl_scalar_d, 1)
TEST_INPUTS(0)
    ushl d3, d4, d5
    ushl d4, d4, d5
    ushl d5, d4, d5
TEST_END

TEST_BEGIN(SHL_ASISDSHF_R, shl_scalar_d, 1)
TEST_INPUTS(0)
    shl d3, d4, #0
    shl d4, d4, #63
TEST_END

TEST_BEGIN(XTN_ASIMDMISC_N_8B, xtn_8b, 1)
TEST_INPUTS(0)
    xtn v3.8b, v4.8h
    xtn v4.8b, v4.8h
TEST_END

TEST_BEGIN(XTN_ASIMDMISC_N_16B, xtn2_8b, 1)
TEST_INPUTS(0)
    xtn2 v3.16b, v4.8h
    xtn2 v4.16b, v4.8h
TEST_END

TEST_BEGIN(XTN_ASIMDMISC_N_4H, xtn_4h, 1)
TEST_INPUTS(0)
    xtn v3.4h, v4.4s
    xtn v4.4h, v4.4s
TEST_END

TEST_BEGIN(XTN_ASIMDMISC_N_8H, xtn2_4h, 1)
TEST_INPUTS(0)
    xtn2 v3.8h, v4.4s
    xtn2 v4.8h, v4.4s
TEST_END

TEST_BEGIN(XTN_ASIMDMISC_N_2S, xtn_2s, 1)
TEST_INPUTS(0)
    xtn v3.2s, v4.2d
    xtn v4.2s, v4.2d
TEST_END

TEST_BEGIN(XTN_ASIMDMISC_N_4S, xtn2_2s, 1)
TEST_INPUTS(0)
    xtn2 v3.4s, v4.2d
    xtn2 v4.4s, v4.2d
TEST_END
