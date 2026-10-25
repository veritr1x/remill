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

/* Element moves: INS (element), DUP (element) and DUP (scalar). The vector
 * registers start from the runner's random state, so every lane differs. */

TEST_BEGIN(INS_ASIMDINS_IV_V, ins_elem, 1)
TEST_INPUTS(0)
    mov v0.b[3], v1.b[15]
    mov v2.h[7], v3.h[0]
    mov v4.s[1], v5.s[2]
    mov v6.d[0], v7.d[1]
    mov v8.s[3], v8.s[0]
TEST_END

TEST_BEGIN(DUP_ASIMDINS_DV_V, dup_elem, 1)
TEST_INPUTS(0)
    dup v0.8b, v1.b[9]
    dup v2.16b, v3.b[15]
    dup v4.4h, v5.h[6]
    dup v6.8h, v7.h[1]
    dup v8.2s, v9.s[3]
    dup v10.4s, v11.s[2]
    dup v12.2d, v13.d[1]
    dup v14.4s, v14.s[1]
TEST_END

TEST_BEGIN(DUP_ASISDONE_ONLY, dup_scalar, 1)
TEST_INPUTS(0)
    mov b0, v1.b[13]
    mov h2, v3.h[5]
    mov s4, v5.s[3]
    mov d6, v7.d[1]
TEST_END
