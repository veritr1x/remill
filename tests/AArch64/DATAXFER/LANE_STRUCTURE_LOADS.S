/* Copyright (c) 2026 Trail of Bits, Inc.
 * Licensed under the Apache License, Version 2.0.
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#define LANE_CASE(kind, reg, last, imm, isel) \
    TEST_BEGIN(isel, CAT(ld1_lane_, kind), 1) \
    TEST_INPUTS(0) \
    add x3, sp, #-256 ; \
    ld1 {reg}[0], [x3] ; \
    ld1 {reg}[last], [x3], imm ; \
    mov x4, imm ; \
    ld1 {reg}[last], [x3], x4 ; \
    TEST_END
LANE_CASE(b, v31.b, 15, #1, LD1_ASISDLSO_B1_1B)
LANE_CASE(h, v31.h, 7, #2, LD1_ASISDLSO_H1_1H)
LANE_CASE(s, v31.s, 3, #4, LD1_ASISDLSO_S1_1S)
LANE_CASE(d, v31.d, 1, #8, LD1_ASISDLSO_D1_1D)
#undef LANE_CASE

#define REPLICATE_CASE(arr, upper, reg, imm) \
    TEST_BEGIN(CAT(LD1R_ASISDLSO_R1_, upper), CAT(ld1r_all_, arr), 1) \
    TEST_INPUTS(0) \
    add x3, sp, #-256 ; \
    ld1r {reg}, [x3] ; \
    ld1r {reg}, [x3], imm ; \
    mov x4, imm ; \
    ld1r {reg}, [x3], x4 ; \
    TEST_END
REPLICATE_CASE(8b, 8B, v31.8b, #1)
REPLICATE_CASE(16b, 16B, v31.16b, #1)
REPLICATE_CASE(4h, 4H, v31.4h, #2)
REPLICATE_CASE(8h, 8H, v31.8h, #2)
REPLICATE_CASE(2s, 2S, v31.2s, #4)
REPLICATE_CASE(4s, 4S, v31.4s, #4)
REPLICATE_CASE(1d, 1D, v31.1d, #8)
REPLICATE_CASE(2d, 2D, v31.2d, #8)
#undef REPLICATE_CASE

#define STRUCTURE_CASE(arr, upper, r0, r1, r2, r3, imm) \
    TEST_BEGIN(CAT(LD4_ASISDLSEP_I4_I_, upper), CAT(ldst4_all_, arr), 1) \
    TEST_INPUTS(0) \
    add x3, sp, #-256 ; \
    add x4, sp, #-512 ; \
    mov x5, imm ; \
    ld4 {r0, r1, r2, r3}, [x3], imm ; \
    st4 {r0, r1, r2, r3}, [x4] ; \
    st4 {r0, r1, r2, r3}, [x4], imm ; \
    ld4 {r0, r1, r2, r3}, [x3], x5 ; \
    st4 {r0, r1, r2, r3}, [x4], x5 ; \
    TEST_END
STRUCTURE_CASE(8b, 8B, v31.8b, v0.8b, v1.8b, v2.8b, #32)
STRUCTURE_CASE(16b, 16B, v31.16b, v0.16b, v1.16b, v2.16b, #64)
STRUCTURE_CASE(4h, 4H, v31.4h, v0.4h, v1.4h, v2.4h, #32)
STRUCTURE_CASE(8h, 8H, v31.8h, v0.8h, v1.8h, v2.8h, #64)
STRUCTURE_CASE(2s, 2S, v31.2s, v0.2s, v1.2s, v2.2s, #32)
STRUCTURE_CASE(4s, 4S, v31.4s, v0.4s, v1.4s, v2.4s, #64)
STRUCTURE_CASE(2d, 2D, v31.2d, v0.2d, v1.2d, v2.2d, #64)
#undef STRUCTURE_CASE

TEST_BEGIN(STLR_SL64_LDSTEXCL, stlr_sizes_zero_sp, 1)
TEST_INPUTS(0, 0xffffffffffffffff)
    add x3, sp, #-256
    stlr ARG1_64, [x3]
    stlrb ARG1_32, [x3]
    stlrh ARG1_32, [x3]
    stlr xzr, [x3]
TEST_END
