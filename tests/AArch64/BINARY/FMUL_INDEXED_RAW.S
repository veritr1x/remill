/* Indexed multiply: signed zero, ARM default NaN, FZ/DN and high-register lanes. */
TEST_BEGIN(FMUL_ASIMDELEM_R_SD_2S, fmul_indexed_raw_2s, 3)
TEST_INPUTS(0x80000000, 0x3f800000, 0, 0, 0x7f800000, 0, 1, 0x3f800000, 0x01000000, 0x00800000, 0x3f000000, 0x01000000, 0x7fc12345, 0x3f800000, 0x02000000)
    dup v1.4s, ARG1_32
    dup v31.4s, ARG2_32
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmul v0.2s, v1.2s, v31.s[3]
    fmul v31.2s, v31.2s, v31.s[1]
TEST_END

TEST_BEGIN(FMUL_ASIMDELEM_R_SD_4S, fmul_indexed_raw_4s, 3)
TEST_INPUTS(0x80000000, 0x3f800000, 0, 0, 0x7f800000, 0, 1, 0x3f800000, 0x01000000, 0x00800000, 0x3f000000, 0x01000000, 0x7fc12345, 0x3f800000, 0x02000000)
    dup v16.4s, ARG1_32
    dup v0.4s, ARG2_32
    msr fpcr, ARG3_64
    msr fpsr, xzr
    .inst 0x4f809214
    fmul v0.4s, v16.4s, v0.s[2]
TEST_END

TEST_BEGIN(FMUL_ASIMDELEM_R_SD_2D, fmul_indexed_raw_2d, 3)
TEST_INPUTS(0x8000000000000000, 0x3ff0000000000000, 0, 0, 0x7ff0000000000000, 0, 1, 0x3ff0000000000000, 0x01000000, 0x0010000000000000, 0x3fe0000000000000, 0x01000000, 0x7ff8123456789abc, 0x3ff0000000000000, 0x02000000)
    dup v1.2d, ARG1_64
    dup v31.2d, ARG2_64
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmul v0.2d, v1.2d, v31.d[1]
    fmul v31.2d, v31.2d, v31.d[0]
TEST_END

TEST_BEGIN(FMUL_ASISDELEM_R_SD_S, fmul_indexed_raw_s, 3)
TEST_INPUTS(0x80000000, 0x3f800000, 0, 0, 0x7f800000, 0, 1, 0x3f800000, 0x01000000, 0x7fc12345, 0x3f800000, 0x02000000)
    dup v1.4s, ARG1_32
    dup v31.4s, ARG2_32
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmul s0, s1, v31.s[3]
    fmul s31, s31, v31.s[1]
TEST_END

TEST_BEGIN(FMUL_ASISDELEM_R_SD_D, fmul_indexed_raw_d, 3)
TEST_INPUTS(0x8000000000000000, 0x3ff0000000000000, 0, 0, 0x7ff0000000000000, 0, 1, 0x3ff0000000000000, 0x01000000, 0x7ff8123456789abc, 0x3ff0000000000000, 0x02000000)
    dup v1.2d, ARG1_64
    dup v31.2d, ARG2_64
    msr fpcr, ARG3_64
    msr fpsr, xzr
    fmul d0, d1, v31.d[1]
    fmul d31, d31, v31.d[0]
TEST_END
