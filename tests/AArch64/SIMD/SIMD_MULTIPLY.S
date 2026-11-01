/* Integer low-half products, modular accumulation, lane selection, and overlap. */

TEST_BEGIN(MUL_ASIMDSAME_ONLY_8B, mul_8b, 1)
TEST_INPUTS(0)
    mul v3.8b, v4.8b, v5.8b
    mul v4.8b, v4.8b, v5.8b
    mul v5.8b, v4.8b, v5.8b
    mul v31.8b, v31.8b, v31.8b
TEST_END

TEST_BEGIN(MUL_ASIMDSAME_ONLY_16B, mul_16b, 1)
TEST_INPUTS(0)
    mul v3.16b, v4.16b, v5.16b
    mul v4.16b, v4.16b, v5.16b
    mul v5.16b, v4.16b, v5.16b
    mul v31.16b, v31.16b, v31.16b
TEST_END

TEST_BEGIN(MUL_ASIMDSAME_ONLY_4H, mul_4h, 1)
TEST_INPUTS(0)
    mul v3.4h, v4.4h, v5.4h
    mul v4.4h, v4.4h, v5.4h
    mul v5.4h, v4.4h, v5.4h
    mul v31.4h, v31.4h, v31.4h
TEST_END

TEST_BEGIN(MUL_ASIMDSAME_ONLY_8H, mul_8h, 1)
TEST_INPUTS(0)
    mul v3.8h, v4.8h, v5.8h
    mul v4.8h, v4.8h, v5.8h
    mul v5.8h, v4.8h, v5.8h
    mul v31.8h, v31.8h, v31.8h
TEST_END

TEST_BEGIN(MUL_ASIMDSAME_ONLY_2S, mul_2s, 1)
TEST_INPUTS(0)
    mul v3.2s, v4.2s, v5.2s
    mul v4.2s, v4.2s, v5.2s
    mul v5.2s, v4.2s, v5.2s
    mul v31.2s, v31.2s, v31.2s
TEST_END

TEST_BEGIN(MUL_ASIMDSAME_ONLY_4S, mul_4s, 1)
TEST_INPUTS(0)
    mul v3.4s, v4.4s, v5.4s
    mul v4.4s, v4.4s, v5.4s
    mul v5.4s, v4.4s, v5.4s
    mul v31.4s, v31.4s, v31.4s
TEST_END

TEST_BEGIN(MLA_ASIMDSAME_ONLY_8B, mla_8b, 1)
TEST_INPUTS(0)
    mla v3.8b, v4.8b, v5.8b
    mla v4.8b, v4.8b, v5.8b
    mla v5.8b, v4.8b, v5.8b
    mla v31.8b, v31.8b, v31.8b
TEST_END

TEST_BEGIN(MLA_ASIMDSAME_ONLY_16B, mla_16b, 1)
TEST_INPUTS(0)
    mla v3.16b, v4.16b, v5.16b
    mla v4.16b, v4.16b, v5.16b
    mla v5.16b, v4.16b, v5.16b
    mla v31.16b, v31.16b, v31.16b
TEST_END

TEST_BEGIN(MLA_ASIMDSAME_ONLY_4H, mla_4h, 1)
TEST_INPUTS(0)
    mla v3.4h, v4.4h, v5.4h
    mla v4.4h, v4.4h, v5.4h
    mla v5.4h, v4.4h, v5.4h
    mla v31.4h, v31.4h, v31.4h
TEST_END

TEST_BEGIN(MLA_ASIMDSAME_ONLY_8H, mla_8h, 1)
TEST_INPUTS(0)
    mla v3.8h, v4.8h, v5.8h
    mla v4.8h, v4.8h, v5.8h
    mla v5.8h, v4.8h, v5.8h
    mla v31.8h, v31.8h, v31.8h
TEST_END

TEST_BEGIN(MLA_ASIMDSAME_ONLY_2S, mla_2s, 1)
TEST_INPUTS(0)
    mla v3.2s, v4.2s, v5.2s
    mla v4.2s, v4.2s, v5.2s
    mla v5.2s, v4.2s, v5.2s
    mla v31.2s, v31.2s, v31.2s
TEST_END

TEST_BEGIN(MLA_ASIMDSAME_ONLY_4S, mla_4s, 1)
TEST_INPUTS(0)
    mla v3.4s, v4.4s, v5.4s
    mla v4.4s, v4.4s, v5.4s
    mla v5.4s, v4.4s, v5.4s
    mla v31.4s, v31.4s, v31.4s
TEST_END

TEST_BEGIN(MLS_ASIMDSAME_ONLY_8B, mls_8b, 1)
TEST_INPUTS(0)
    mls v3.8b, v4.8b, v5.8b
    mls v4.8b, v4.8b, v5.8b
    mls v5.8b, v4.8b, v5.8b
    mls v31.8b, v31.8b, v31.8b
TEST_END

TEST_BEGIN(MLS_ASIMDSAME_ONLY_16B, mls_16b, 1)
TEST_INPUTS(0)
    mls v3.16b, v4.16b, v5.16b
    mls v4.16b, v4.16b, v5.16b
    mls v5.16b, v4.16b, v5.16b
    mls v31.16b, v31.16b, v31.16b
TEST_END

TEST_BEGIN(MLS_ASIMDSAME_ONLY_4H, mls_4h, 1)
TEST_INPUTS(0)
    mls v3.4h, v4.4h, v5.4h
    mls v4.4h, v4.4h, v5.4h
    mls v5.4h, v4.4h, v5.4h
    mls v31.4h, v31.4h, v31.4h
TEST_END

TEST_BEGIN(MLS_ASIMDSAME_ONLY_8H, mls_8h, 1)
TEST_INPUTS(0)
    mls v3.8h, v4.8h, v5.8h
    mls v4.8h, v4.8h, v5.8h
    mls v5.8h, v4.8h, v5.8h
    mls v31.8h, v31.8h, v31.8h
TEST_END

TEST_BEGIN(MLS_ASIMDSAME_ONLY_2S, mls_2s, 1)
TEST_INPUTS(0)
    mls v3.2s, v4.2s, v5.2s
    mls v4.2s, v4.2s, v5.2s
    mls v5.2s, v4.2s, v5.2s
    mls v31.2s, v31.2s, v31.2s
TEST_END

TEST_BEGIN(MLS_ASIMDSAME_ONLY_4S, mls_4s, 1)
TEST_INPUTS(0)
    mls v3.4s, v4.4s, v5.4s
    mls v4.4s, v4.4s, v5.4s
    mls v5.4s, v4.4s, v5.4s
    mls v31.4s, v31.4s, v31.4s
TEST_END

TEST_BEGIN(MUL_ASIMDELEM_R_4H, mul_4h_element, 1)
TEST_INPUTS(0)
    mul v3.4h, v4.4h, v15.h[0]
    mul v3.4h, v4.4h, v15.h[1]
    mul v3.4h, v4.4h, v15.h[2]
    mul v3.4h, v4.4h, v15.h[3]
    mul v3.4h, v4.4h, v15.h[4]
    mul v3.4h, v4.4h, v15.h[5]
    mul v3.4h, v4.4h, v15.h[6]
    mul v3.4h, v4.4h, v15.h[7]
    mul v15.4h, v15.4h, v15.h[7]
TEST_END

TEST_BEGIN(MUL_ASIMDELEM_R_8H, mul_8h_element, 1)
TEST_INPUTS(0)
    mul v3.8h, v4.8h, v15.h[0]
    mul v3.8h, v4.8h, v15.h[1]
    mul v3.8h, v4.8h, v15.h[2]
    mul v3.8h, v4.8h, v15.h[3]
    mul v3.8h, v4.8h, v15.h[4]
    mul v3.8h, v4.8h, v15.h[5]
    mul v3.8h, v4.8h, v15.h[6]
    mul v3.8h, v4.8h, v15.h[7]
    mul v15.8h, v15.8h, v15.h[7]
TEST_END

TEST_BEGIN(MUL_ASIMDELEM_R_2S, mul_2s_element, 1)
TEST_INPUTS(0)
    mul v3.2s, v4.2s, v31.s[0]
    mul v3.2s, v4.2s, v31.s[1]
    mul v3.2s, v4.2s, v31.s[2]
    mul v3.2s, v4.2s, v31.s[3]
    mul v31.2s, v31.2s, v31.s[3]
TEST_END

TEST_BEGIN(MUL_ASIMDELEM_R_4S, mul_4s_element, 1)
TEST_INPUTS(0)
    mul v3.4s, v4.4s, v31.s[0]
    mul v3.4s, v4.4s, v31.s[1]
    mul v3.4s, v4.4s, v31.s[2]
    mul v3.4s, v4.4s, v31.s[3]
    mul v31.4s, v31.4s, v31.s[3]
TEST_END

TEST_BEGIN(MLA_ASIMDELEM_R_4H, mla_4h_element, 1)
TEST_INPUTS(0)
    mla v3.4h, v4.4h, v15.h[0]
    mla v3.4h, v4.4h, v15.h[1]
    mla v3.4h, v4.4h, v15.h[2]
    mla v3.4h, v4.4h, v15.h[3]
    mla v3.4h, v4.4h, v15.h[4]
    mla v3.4h, v4.4h, v15.h[5]
    mla v3.4h, v4.4h, v15.h[6]
    mla v3.4h, v4.4h, v15.h[7]
    mla v15.4h, v15.4h, v15.h[7]
TEST_END

TEST_BEGIN(MLA_ASIMDELEM_R_8H, mla_8h_element, 1)
TEST_INPUTS(0)
    mla v3.8h, v4.8h, v15.h[0]
    mla v3.8h, v4.8h, v15.h[1]
    mla v3.8h, v4.8h, v15.h[2]
    mla v3.8h, v4.8h, v15.h[3]
    mla v3.8h, v4.8h, v15.h[4]
    mla v3.8h, v4.8h, v15.h[5]
    mla v3.8h, v4.8h, v15.h[6]
    mla v3.8h, v4.8h, v15.h[7]
    mla v15.8h, v15.8h, v15.h[7]
TEST_END

TEST_BEGIN(MLA_ASIMDELEM_R_2S, mla_2s_element, 1)
TEST_INPUTS(0)
    mla v3.2s, v4.2s, v31.s[0]
    mla v3.2s, v4.2s, v31.s[1]
    mla v3.2s, v4.2s, v31.s[2]
    mla v3.2s, v4.2s, v31.s[3]
    mla v31.2s, v31.2s, v31.s[3]
TEST_END

TEST_BEGIN(MLA_ASIMDELEM_R_4S, mla_4s_element, 1)
TEST_INPUTS(0)
    mla v3.4s, v4.4s, v31.s[0]
    mla v3.4s, v4.4s, v31.s[1]
    mla v3.4s, v4.4s, v31.s[2]
    mla v3.4s, v4.4s, v31.s[3]
    mla v31.4s, v31.4s, v31.s[3]
TEST_END

TEST_BEGIN(MLS_ASIMDELEM_R_4H, mls_4h_element, 1)
TEST_INPUTS(0)
    mls v3.4h, v4.4h, v15.h[0]
    mls v3.4h, v4.4h, v15.h[1]
    mls v3.4h, v4.4h, v15.h[2]
    mls v3.4h, v4.4h, v15.h[3]
    mls v3.4h, v4.4h, v15.h[4]
    mls v3.4h, v4.4h, v15.h[5]
    mls v3.4h, v4.4h, v15.h[6]
    mls v3.4h, v4.4h, v15.h[7]
    mls v15.4h, v15.4h, v15.h[7]
TEST_END

TEST_BEGIN(MLS_ASIMDELEM_R_8H, mls_8h_element, 1)
TEST_INPUTS(0)
    mls v3.8h, v4.8h, v15.h[0]
    mls v3.8h, v4.8h, v15.h[1]
    mls v3.8h, v4.8h, v15.h[2]
    mls v3.8h, v4.8h, v15.h[3]
    mls v3.8h, v4.8h, v15.h[4]
    mls v3.8h, v4.8h, v15.h[5]
    mls v3.8h, v4.8h, v15.h[6]
    mls v3.8h, v4.8h, v15.h[7]
    mls v15.8h, v15.8h, v15.h[7]
TEST_END

TEST_BEGIN(MLS_ASIMDELEM_R_2S, mls_2s_element, 1)
TEST_INPUTS(0)
    mls v3.2s, v4.2s, v31.s[0]
    mls v3.2s, v4.2s, v31.s[1]
    mls v3.2s, v4.2s, v31.s[2]
    mls v3.2s, v4.2s, v31.s[3]
    mls v31.2s, v31.2s, v31.s[3]
TEST_END

TEST_BEGIN(MLS_ASIMDELEM_R_4S, mls_4s_element, 1)
TEST_INPUTS(0)
    mls v3.4s, v4.4s, v31.s[0]
    mls v3.4s, v4.4s, v31.s[1]
    mls v3.4s, v4.4s, v31.s[2]
    mls v3.4s, v4.4s, v31.s[3]
    mls v31.4s, v31.4s, v31.s[3]
TEST_END
