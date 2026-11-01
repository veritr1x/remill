/* Indexed fused accumulation: all lanes, aliases and scalar/Q0 upper clears. */
TEST_BEGIN(FMLA_ASIMDELEM_R_SD_2S, fmla_indexed_2s, 3)
TEST_INPUTS(0, 0x3f800001, 0x3f7fffff, 0x7fc12345, 0, 0x7f800000, 1, 0x80000001, 0x3f800000)
    dup v0.4s, ARG1_32
    dup v1.4s, ARG2_32
    dup v31.4s, ARG3_32
    fmla v0.2s, v1.2s, v31.s[0]
    fmla v1.2s, v1.2s, v31.s[1]
    fmla v31.2s, v1.2s, v31.s[2]
    fmla v31.2s, v31.2s, v31.s[3]
TEST_END

TEST_BEGIN(FMLA_ASIMDELEM_R_SD_4S, fmla_indexed_4s, 3)
TEST_INPUTS(0, 0x3f800001, 0x3f7fffff, 0x7fc12345, 0, 0x7f800000, 1, 0x80000001, 0x3f800000)
    dup v20.4s, ARG1_32
    dup v17.4s, ARG2_32
    dup v0.4s, ARG3_32
    .inst 0x4fa01234
    fmla v17.4s, v17.4s, v0.s[0]
    fmla v0.4s, v17.4s, v0.s[2]
    fmla v0.4s, v0.4s, v0.s[3]
TEST_END

TEST_BEGIN(FMLA_ASIMDELEM_R_SD_2D, fmla_indexed_2d, 3)
TEST_INPUTS(0, 0x3ff0000000000001, 0x3fefffffffffffff, 0x7ff8123456789abc, 0, 0x7ff0000000000000, 1, 0x8000000000000001, 0x3ff0000000000000)
    dup v0.2d, ARG1_64
    dup v1.2d, ARG2_64
    dup v31.2d, ARG3_64
    fmla v0.2d, v1.2d, v31.d[0]
    fmla v1.2d, v1.2d, v31.d[1]
    fmla v31.2d, v1.2d, v31.d[0]
    fmla v31.2d, v31.2d, v31.d[1]
TEST_END

TEST_BEGIN(FMLA_ASISDELEM_R_SD_S, fmla_indexed_s, 3)
TEST_INPUTS(0, 0x3f800001, 0x3f7fffff, 0x7fc12345, 0, 0x7f800000, 1, 0x80000001, 0x3f800000)
    dup v0.4s, ARG1_32
    dup v1.4s, ARG2_32
    dup v31.4s, ARG3_32
    fmla s0, s1, v31.s[0]
    fmla s1, s1, v31.s[1]
    fmla s31, s1, v31.s[2]
    fmla s31, s31, v31.s[3]
TEST_END

TEST_BEGIN(FMLA_ASISDELEM_R_SD_D, fmla_indexed_d, 3)
TEST_INPUTS(0, 0x3ff0000000000001, 0x3fefffffffffffff, 0x7ff8123456789abc, 0, 0x7ff0000000000000, 1, 0x8000000000000001, 0x3ff0000000000000)
    dup v0.2d, ARG1_64
    dup v1.2d, ARG2_64
    dup v31.2d, ARG3_64
    fmla d0, d1, v31.d[0]
    fmla d1, d1, v31.d[1]
    fmla d31, d1, v31.d[0]
    fmla d31, d31, v31.d[1]
TEST_END
