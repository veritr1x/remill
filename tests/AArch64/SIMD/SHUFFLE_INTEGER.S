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

TEST_BEGIN(ZIP1_ASIMDPERM_ONLY_8B, zip1_8b, 1)
TEST_INPUTS(0)
    zip1 v3.8b, v4.8b, v5.8b
    zip1 v4.8b, v4.8b, v5.8b
    zip1 v5.8b, v4.8b, v5.8b
    zip1 v3.8b, v4.8b, v4.8b
    zip1 v4.8b, v4.8b, v4.8b
TEST_END

TEST_BEGIN(ZIP2_ASIMDPERM_ONLY_8B, zip2_8b, 1)
TEST_INPUTS(0)
    zip2 v3.8b, v4.8b, v5.8b
    zip2 v4.8b, v4.8b, v5.8b
    zip2 v5.8b, v4.8b, v5.8b
    zip2 v3.8b, v4.8b, v4.8b
    zip2 v4.8b, v4.8b, v4.8b
TEST_END

TEST_BEGIN(UZP1_ASIMDPERM_ONLY_8B, uzp1_8b, 1)
TEST_INPUTS(0)
    uzp1 v3.8b, v4.8b, v5.8b
    uzp1 v4.8b, v4.8b, v5.8b
    uzp1 v5.8b, v4.8b, v5.8b
    uzp1 v3.8b, v4.8b, v4.8b
    uzp1 v4.8b, v4.8b, v4.8b
TEST_END

TEST_BEGIN(UZP2_ASIMDPERM_ONLY_8B, uzp2_8b, 1)
TEST_INPUTS(0)
    uzp2 v3.8b, v4.8b, v5.8b
    uzp2 v4.8b, v4.8b, v5.8b
    uzp2 v5.8b, v4.8b, v5.8b
    uzp2 v3.8b, v4.8b, v4.8b
    uzp2 v4.8b, v4.8b, v4.8b
TEST_END

TEST_BEGIN(REV16_ASIMDMISC_R_8B, rev16_8b, 1)
TEST_INPUTS(0)
    rev16 v3.8b, v4.8b
    rev16 v4.8b, v4.8b
TEST_END

TEST_BEGIN(REV32_ASIMDMISC_R_8B, rev32_8b, 1)
TEST_INPUTS(0)
    rev32 v3.8b, v4.8b
    rev32 v4.8b, v4.8b
TEST_END

TEST_BEGIN(REV64_ASIMDMISC_R_8B, rev64_8b, 1)
TEST_INPUTS(0)
    rev64 v3.8b, v4.8b
    rev64 v4.8b, v4.8b
TEST_END

TEST_BEGIN(ZIP1_ASIMDPERM_ONLY_16B, zip1_16b, 1)
TEST_INPUTS(0)
    zip1 v3.16b, v4.16b, v5.16b
    zip1 v4.16b, v4.16b, v5.16b
    zip1 v5.16b, v4.16b, v5.16b
    zip1 v3.16b, v4.16b, v4.16b
    zip1 v4.16b, v4.16b, v4.16b
TEST_END

TEST_BEGIN(ZIP2_ASIMDPERM_ONLY_16B, zip2_16b, 1)
TEST_INPUTS(0)
    zip2 v3.16b, v4.16b, v5.16b
    zip2 v4.16b, v4.16b, v5.16b
    zip2 v5.16b, v4.16b, v5.16b
    zip2 v3.16b, v4.16b, v4.16b
    zip2 v4.16b, v4.16b, v4.16b
TEST_END

TEST_BEGIN(UZP1_ASIMDPERM_ONLY_16B, uzp1_16b, 1)
TEST_INPUTS(0)
    uzp1 v3.16b, v4.16b, v5.16b
    uzp1 v4.16b, v4.16b, v5.16b
    uzp1 v5.16b, v4.16b, v5.16b
    uzp1 v3.16b, v4.16b, v4.16b
    uzp1 v4.16b, v4.16b, v4.16b
TEST_END

TEST_BEGIN(UZP2_ASIMDPERM_ONLY_16B, uzp2_16b, 1)
TEST_INPUTS(0)
    uzp2 v3.16b, v4.16b, v5.16b
    uzp2 v4.16b, v4.16b, v5.16b
    uzp2 v5.16b, v4.16b, v5.16b
    uzp2 v3.16b, v4.16b, v4.16b
    uzp2 v4.16b, v4.16b, v4.16b
TEST_END

TEST_BEGIN(REV16_ASIMDMISC_R_16B, rev16_16b, 1)
TEST_INPUTS(0)
    rev16 v3.16b, v4.16b
    rev16 v4.16b, v4.16b
TEST_END

TEST_BEGIN(REV32_ASIMDMISC_R_16B, rev32_16b, 1)
TEST_INPUTS(0)
    rev32 v3.16b, v4.16b
    rev32 v4.16b, v4.16b
TEST_END

TEST_BEGIN(REV64_ASIMDMISC_R_16B, rev64_16b, 1)
TEST_INPUTS(0)
    rev64 v3.16b, v4.16b
    rev64 v4.16b, v4.16b
TEST_END

TEST_BEGIN(ZIP1_ASIMDPERM_ONLY_4H, zip1_4h, 1)
TEST_INPUTS(0)
    zip1 v3.4h, v4.4h, v5.4h
    zip1 v4.4h, v4.4h, v5.4h
    zip1 v5.4h, v4.4h, v5.4h
    zip1 v3.4h, v4.4h, v4.4h
    zip1 v4.4h, v4.4h, v4.4h
TEST_END

TEST_BEGIN(ZIP2_ASIMDPERM_ONLY_4H, zip2_4h, 1)
TEST_INPUTS(0)
    zip2 v3.4h, v4.4h, v5.4h
    zip2 v4.4h, v4.4h, v5.4h
    zip2 v5.4h, v4.4h, v5.4h
    zip2 v3.4h, v4.4h, v4.4h
    zip2 v4.4h, v4.4h, v4.4h
TEST_END

TEST_BEGIN(UZP1_ASIMDPERM_ONLY_4H, uzp1_4h, 1)
TEST_INPUTS(0)
    uzp1 v3.4h, v4.4h, v5.4h
    uzp1 v4.4h, v4.4h, v5.4h
    uzp1 v5.4h, v4.4h, v5.4h
    uzp1 v3.4h, v4.4h, v4.4h
    uzp1 v4.4h, v4.4h, v4.4h
TEST_END

TEST_BEGIN(UZP2_ASIMDPERM_ONLY_4H, uzp2_4h, 1)
TEST_INPUTS(0)
    uzp2 v3.4h, v4.4h, v5.4h
    uzp2 v4.4h, v4.4h, v5.4h
    uzp2 v5.4h, v4.4h, v5.4h
    uzp2 v3.4h, v4.4h, v4.4h
    uzp2 v4.4h, v4.4h, v4.4h
TEST_END

TEST_BEGIN(REV32_ASIMDMISC_R_4H, rev32_4h, 1)
TEST_INPUTS(0)
    rev32 v3.4h, v4.4h
    rev32 v4.4h, v4.4h
TEST_END

TEST_BEGIN(REV64_ASIMDMISC_R_4H, rev64_4h, 1)
TEST_INPUTS(0)
    rev64 v3.4h, v4.4h
    rev64 v4.4h, v4.4h
TEST_END

TEST_BEGIN(ZIP1_ASIMDPERM_ONLY_8H, zip1_8h, 1)
TEST_INPUTS(0)
    zip1 v3.8h, v4.8h, v5.8h
    zip1 v4.8h, v4.8h, v5.8h
    zip1 v5.8h, v4.8h, v5.8h
    zip1 v3.8h, v4.8h, v4.8h
    zip1 v4.8h, v4.8h, v4.8h
TEST_END

TEST_BEGIN(ZIP2_ASIMDPERM_ONLY_8H, zip2_8h, 1)
TEST_INPUTS(0)
    zip2 v3.8h, v4.8h, v5.8h
    zip2 v4.8h, v4.8h, v5.8h
    zip2 v5.8h, v4.8h, v5.8h
    zip2 v3.8h, v4.8h, v4.8h
    zip2 v4.8h, v4.8h, v4.8h
TEST_END

TEST_BEGIN(UZP1_ASIMDPERM_ONLY_8H, uzp1_8h, 1)
TEST_INPUTS(0)
    uzp1 v3.8h, v4.8h, v5.8h
    uzp1 v4.8h, v4.8h, v5.8h
    uzp1 v5.8h, v4.8h, v5.8h
    uzp1 v3.8h, v4.8h, v4.8h
    uzp1 v4.8h, v4.8h, v4.8h
TEST_END

TEST_BEGIN(UZP2_ASIMDPERM_ONLY_8H, uzp2_8h, 1)
TEST_INPUTS(0)
    uzp2 v3.8h, v4.8h, v5.8h
    uzp2 v4.8h, v4.8h, v5.8h
    uzp2 v5.8h, v4.8h, v5.8h
    uzp2 v3.8h, v4.8h, v4.8h
    uzp2 v4.8h, v4.8h, v4.8h
TEST_END

TEST_BEGIN(REV32_ASIMDMISC_R_8H, rev32_8h, 1)
TEST_INPUTS(0)
    rev32 v3.8h, v4.8h
    rev32 v4.8h, v4.8h
TEST_END

TEST_BEGIN(REV64_ASIMDMISC_R_8H, rev64_8h, 1)
TEST_INPUTS(0)
    rev64 v3.8h, v4.8h
    rev64 v4.8h, v4.8h
TEST_END

TEST_BEGIN(ZIP1_ASIMDPERM_ONLY_2S, zip1_2s, 1)
TEST_INPUTS(0)
    zip1 v3.2s, v4.2s, v5.2s
    zip1 v4.2s, v4.2s, v5.2s
    zip1 v5.2s, v4.2s, v5.2s
    zip1 v3.2s, v4.2s, v4.2s
    zip1 v4.2s, v4.2s, v4.2s
TEST_END

TEST_BEGIN(ZIP2_ASIMDPERM_ONLY_2S, zip2_2s, 1)
TEST_INPUTS(0)
    zip2 v3.2s, v4.2s, v5.2s
    zip2 v4.2s, v4.2s, v5.2s
    zip2 v5.2s, v4.2s, v5.2s
    zip2 v3.2s, v4.2s, v4.2s
    zip2 v4.2s, v4.2s, v4.2s
TEST_END

TEST_BEGIN(UZP1_ASIMDPERM_ONLY_2S, uzp1_2s, 1)
TEST_INPUTS(0)
    uzp1 v3.2s, v4.2s, v5.2s
    uzp1 v4.2s, v4.2s, v5.2s
    uzp1 v5.2s, v4.2s, v5.2s
    uzp1 v3.2s, v4.2s, v4.2s
    uzp1 v4.2s, v4.2s, v4.2s
TEST_END

TEST_BEGIN(UZP2_ASIMDPERM_ONLY_2S, uzp2_2s, 1)
TEST_INPUTS(0)
    uzp2 v3.2s, v4.2s, v5.2s
    uzp2 v4.2s, v4.2s, v5.2s
    uzp2 v5.2s, v4.2s, v5.2s
    uzp2 v3.2s, v4.2s, v4.2s
    uzp2 v4.2s, v4.2s, v4.2s
TEST_END

TEST_BEGIN(REV64_ASIMDMISC_R_2S, rev64_2s, 1)
TEST_INPUTS(0)
    rev64 v3.2s, v4.2s
    rev64 v4.2s, v4.2s
TEST_END

TEST_BEGIN(ZIP1_ASIMDPERM_ONLY_4S, zip1_4s, 1)
TEST_INPUTS(0)
    zip1 v3.4s, v4.4s, v5.4s
    zip1 v4.4s, v4.4s, v5.4s
    zip1 v5.4s, v4.4s, v5.4s
    zip1 v3.4s, v4.4s, v4.4s
    zip1 v4.4s, v4.4s, v4.4s
TEST_END

TEST_BEGIN(ZIP2_ASIMDPERM_ONLY_4S, zip2_4s, 1)
TEST_INPUTS(0)
    zip2 v3.4s, v4.4s, v5.4s
    zip2 v4.4s, v4.4s, v5.4s
    zip2 v5.4s, v4.4s, v5.4s
    zip2 v3.4s, v4.4s, v4.4s
    zip2 v4.4s, v4.4s, v4.4s
TEST_END

TEST_BEGIN(UZP1_ASIMDPERM_ONLY_4S, uzp1_4s, 1)
TEST_INPUTS(0)
    uzp1 v3.4s, v4.4s, v5.4s
    uzp1 v4.4s, v4.4s, v5.4s
    uzp1 v5.4s, v4.4s, v5.4s
    uzp1 v3.4s, v4.4s, v4.4s
    uzp1 v4.4s, v4.4s, v4.4s
TEST_END

TEST_BEGIN(UZP2_ASIMDPERM_ONLY_4S, uzp2_4s, 1)
TEST_INPUTS(0)
    uzp2 v3.4s, v4.4s, v5.4s
    uzp2 v4.4s, v4.4s, v5.4s
    uzp2 v5.4s, v4.4s, v5.4s
    uzp2 v3.4s, v4.4s, v4.4s
    uzp2 v4.4s, v4.4s, v4.4s
TEST_END

TEST_BEGIN(REV64_ASIMDMISC_R_4S, rev64_4s, 1)
TEST_INPUTS(0)
    rev64 v3.4s, v4.4s
    rev64 v4.4s, v4.4s
TEST_END

TEST_BEGIN(ZIP1_ASIMDPERM_ONLY_2D, zip1_2d, 1)
TEST_INPUTS(0)
    zip1 v3.2d, v4.2d, v5.2d
    zip1 v4.2d, v4.2d, v5.2d
    zip1 v5.2d, v4.2d, v5.2d
    zip1 v3.2d, v4.2d, v4.2d
    zip1 v4.2d, v4.2d, v4.2d
TEST_END

TEST_BEGIN(ZIP2_ASIMDPERM_ONLY_2D, zip2_2d, 1)
TEST_INPUTS(0)
    zip2 v3.2d, v4.2d, v5.2d
    zip2 v4.2d, v4.2d, v5.2d
    zip2 v5.2d, v4.2d, v5.2d
    zip2 v3.2d, v4.2d, v4.2d
    zip2 v4.2d, v4.2d, v4.2d
TEST_END

TEST_BEGIN(UZP1_ASIMDPERM_ONLY_2D, uzp1_2d, 1)
TEST_INPUTS(0)
    uzp1 v3.2d, v4.2d, v5.2d
    uzp1 v4.2d, v4.2d, v5.2d
    uzp1 v5.2d, v4.2d, v5.2d
    uzp1 v3.2d, v4.2d, v4.2d
    uzp1 v4.2d, v4.2d, v4.2d
TEST_END

TEST_BEGIN(UZP2_ASIMDPERM_ONLY_2D, uzp2_2d, 1)
TEST_INPUTS(0)
    uzp2 v3.2d, v4.2d, v5.2d
    uzp2 v4.2d, v4.2d, v5.2d
    uzp2 v5.2d, v4.2d, v5.2d
    uzp2 v3.2d, v4.2d, v4.2d
    uzp2 v4.2d, v4.2d, v4.2d
TEST_END
