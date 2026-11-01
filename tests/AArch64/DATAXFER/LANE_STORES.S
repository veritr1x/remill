/* ST1 lane stores preserve vector sources and write exactly one element. */

TEST_BEGIN(ST1_ASISDLSO_B1_1B, st1_lane_b_plain, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.b }[0], [x3]
    st1 { v31.b }[1], [x3]
    st1 { v31.b }[2], [x3]
    st1 { v31.b }[3], [x3]
    st1 { v31.b }[4], [x3]
    st1 { v31.b }[5], [x3]
    st1 { v31.b }[6], [x3]
    st1 { v31.b }[7], [x3]
    st1 { v31.b }[8], [x3]
    st1 { v31.b }[9], [x3]
    st1 { v31.b }[10], [x3]
    st1 { v31.b }[11], [x3]
    st1 { v31.b }[12], [x3]
    st1 { v31.b }[13], [x3]
    st1 { v31.b }[14], [x3]
    st1 { v31.b }[15], [x3]
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.b }[15], [sp]
    mov sp, x5
TEST_END

TEST_BEGIN(ST1_ASISDLSOP_B1_I1B, st1_lane_b_imm, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.b }[0], [x3], #1
    st1 { v31.b }[1], [x3], #1
    st1 { v31.b }[2], [x3], #1
    st1 { v31.b }[3], [x3], #1
    st1 { v31.b }[4], [x3], #1
    st1 { v31.b }[5], [x3], #1
    st1 { v31.b }[6], [x3], #1
    st1 { v31.b }[7], [x3], #1
    st1 { v31.b }[8], [x3], #1
    st1 { v31.b }[9], [x3], #1
    st1 { v31.b }[10], [x3], #1
    st1 { v31.b }[11], [x3], #1
    st1 { v31.b }[12], [x3], #1
    st1 { v31.b }[13], [x3], #1
    st1 { v31.b }[14], [x3], #1
    st1 { v31.b }[15], [x3], #1
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.b }[15], [sp], #1
    mov sp, x5
TEST_END

TEST_BEGIN(ST1_ASISDLSOP_BX1_R1B, st1_lane_b_reg, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.b }[0], [x3], x4
    st1 { v31.b }[1], [x3], x4
    st1 { v31.b }[2], [x3], x4
    st1 { v31.b }[3], [x3], x4
    st1 { v31.b }[4], [x3], x4
    st1 { v31.b }[5], [x3], x4
    st1 { v31.b }[6], [x3], x4
    st1 { v31.b }[7], [x3], x4
    st1 { v31.b }[8], [x3], x4
    st1 { v31.b }[9], [x3], x4
    st1 { v31.b }[10], [x3], x4
    st1 { v31.b }[11], [x3], x4
    st1 { v31.b }[12], [x3], x4
    st1 { v31.b }[13], [x3], x4
    st1 { v31.b }[14], [x3], x4
    st1 { v31.b }[15], [x3], x4
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.b }[15], [sp], x4
    mov sp, x5
    add x3, sp, #-64
    st1 { v0.b }[0], [x3], x3
TEST_END

TEST_BEGIN(ST1_ASISDLSO_H1_1H, st1_lane_h_plain, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.h }[0], [x3]
    st1 { v31.h }[1], [x3]
    st1 { v31.h }[2], [x3]
    st1 { v31.h }[3], [x3]
    st1 { v31.h }[4], [x3]
    st1 { v31.h }[5], [x3]
    st1 { v31.h }[6], [x3]
    st1 { v31.h }[7], [x3]
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.h }[7], [sp]
    mov sp, x5
TEST_END

TEST_BEGIN(ST1_ASISDLSOP_H1_I1H, st1_lane_h_imm, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.h }[0], [x3], #2
    st1 { v31.h }[1], [x3], #2
    st1 { v31.h }[2], [x3], #2
    st1 { v31.h }[3], [x3], #2
    st1 { v31.h }[4], [x3], #2
    st1 { v31.h }[5], [x3], #2
    st1 { v31.h }[6], [x3], #2
    st1 { v31.h }[7], [x3], #2
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.h }[7], [sp], #2
    mov sp, x5
TEST_END

TEST_BEGIN(ST1_ASISDLSOP_HX1_R1H, st1_lane_h_reg, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.h }[0], [x3], x4
    st1 { v31.h }[1], [x3], x4
    st1 { v31.h }[2], [x3], x4
    st1 { v31.h }[3], [x3], x4
    st1 { v31.h }[4], [x3], x4
    st1 { v31.h }[5], [x3], x4
    st1 { v31.h }[6], [x3], x4
    st1 { v31.h }[7], [x3], x4
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.h }[7], [sp], x4
    mov sp, x5
    add x3, sp, #-64
    st1 { v0.h }[0], [x3], x3
TEST_END

TEST_BEGIN(ST1_ASISDLSO_S1_1S, st1_lane_s_plain, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.s }[0], [x3]
    st1 { v31.s }[1], [x3]
    st1 { v31.s }[2], [x3]
    st1 { v31.s }[3], [x3]
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.s }[3], [sp]
    mov sp, x5
TEST_END

TEST_BEGIN(ST1_ASISDLSOP_S1_I1S, st1_lane_s_imm, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.s }[0], [x3], #4
    st1 { v31.s }[1], [x3], #4
    st1 { v31.s }[2], [x3], #4
    st1 { v31.s }[3], [x3], #4
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.s }[3], [sp], #4
    mov sp, x5
TEST_END

TEST_BEGIN(ST1_ASISDLSOP_SX1_R1S, st1_lane_s_reg, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.s }[0], [x3], x4
    st1 { v31.s }[1], [x3], x4
    st1 { v31.s }[2], [x3], x4
    st1 { v31.s }[3], [x3], x4
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.s }[3], [sp], x4
    mov sp, x5
    add x3, sp, #-64
    st1 { v0.s }[0], [x3], x3
TEST_END

TEST_BEGIN(ST1_ASISDLSO_D1_1D, st1_lane_d_plain, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.d }[0], [x3]
    st1 { v31.d }[1], [x3]
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.d }[1], [sp]
    mov sp, x5
TEST_END

TEST_BEGIN(ST1_ASISDLSOP_D1_I1D, st1_lane_d_imm, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.d }[0], [x3], #8
    st1 { v31.d }[1], [x3], #8
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.d }[1], [sp], #8
    mov sp, x5
TEST_END

TEST_BEGIN(ST1_ASISDLSOP_DX1_R1D, st1_lane_d_reg, 1)
TEST_INPUTS(0)
    add x3, sp, #-512
    mov x4, #16
    st1 { v31.d }[0], [x3], x4
    st1 { v31.d }[1], [x3], x4
    mov x5, sp
    sub sp, sp, #16
    st1 { v31.d }[1], [sp], x4
    mov sp, x5
    add x3, sp, #-64
    st1 { v0.d }[0], [x3], x3
TEST_END
