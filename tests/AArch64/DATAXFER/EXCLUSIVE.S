/* Exclusive access widths, ordering forms, SP addressing, and CLREX. */

TEST_BEGIN(LDXRB_LR32_LDSTEXCL, ldxrb_8_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxrb w3, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(LDAXRB_LR32_LDSTEXCL, ldaxrb_8_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldaxrb w3, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(STXRB_SR32_LDSTEXCL, stxrb_8_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxrb w3, [sp]
    stxrb w4, w5, [sp]
    stxrb w6, w5, [sp]
    add sp, sp, #16
TEST_END

TEST_BEGIN(STLXRB_SR32_LDSTEXCL, stlxrb_8_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxrb w3, [sp]
    stlxrb w4, w5, [sp]
    stlxrb w6, w5, [sp]
    add sp, sp, #16
TEST_END

TEST_BEGIN(LDXRH_LR32_LDSTEXCL, ldxrh_16_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxrh w3, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(LDAXRH_LR32_LDSTEXCL, ldaxrh_16_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldaxrh w3, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(STXRH_SR32_LDSTEXCL, stxrh_16_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxrh w3, [sp]
    stxrh w4, w5, [sp]
    stxrh w6, w5, [sp]
    add sp, sp, #16
TEST_END

TEST_BEGIN(STLXRH_SR32_LDSTEXCL, stlxrh_16_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxrh w3, [sp]
    stlxrh w4, w5, [sp]
    stlxrh w6, w5, [sp]
    add sp, sp, #16
TEST_END

TEST_BEGIN(LDXR_LR32_LDSTEXCL, ldxr_32_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxr w3, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(LDAXR_LR32_LDSTEXCL, ldaxr_32_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldaxr w3, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(STXR_SR32_LDSTEXCL, stxr_32_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxr w3, [sp]
    stxr w4, w5, [sp]
    stxr w6, w5, [sp]
    add sp, sp, #16
TEST_END

TEST_BEGIN(STLXR_SR32_LDSTEXCL, stlxr_32_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxr w3, [sp]
    stlxr w4, w5, [sp]
    stlxr w6, w5, [sp]
    add sp, sp, #16
TEST_END

TEST_BEGIN(LDXR_LR64_LDSTEXCL, ldxr_64_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxr x3, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(LDAXR_LR64_LDSTEXCL, ldaxr_64_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldaxr x3, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(STXR_SR64_LDSTEXCL, stxr_64_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxr x3, [sp]
    stxr w4, x5, [sp]
    stxr w6, x5, [sp]
    add sp, sp, #16
TEST_END

TEST_BEGIN(STLXR_SR64_LDSTEXCL, stlxr_64_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    str xzr, [sp]
    ldxr x3, [sp]
    stlxr w4, x5, [sp]
    stlxr w6, x5, [sp]
    add sp, sp, #16
TEST_END


/* Pair exclusives use one reservation for the complete access shape. */

TEST_BEGIN(LDXP_LP32_LDSTEXCL, ldxp_32_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    stp xzr, xzr, [sp]
    ldxp w3, w4, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(LDAXP_LP32_LDSTEXCL, ldaxp_32_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    stp xzr, xzr, [sp]
    ldaxp w3, w4, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(STXP_SP32_LDSTEXCL, stxp_32_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    stp xzr, xzr, [sp]
    ldxp w3, w4, [sp]
    stxp w5, w6, w6, [sp]
    stxp w7, wzr, wzr, [sp]
    add sp, sp, #16
TEST_END

TEST_BEGIN(STLXP_SP32_LDSTEXCL, stlxp_32_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    stp xzr, xzr, [sp]
    ldxp w3, w4, [sp]
    stlxp w5, w6, w6, [sp]
    stlxp w7, wzr, wzr, [sp]
    add sp, sp, #16
TEST_END

TEST_BEGIN(LDXP_LP64_LDSTEXCL, ldxp_64_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    stp xzr, xzr, [sp]
    ldxp x3, x4, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(LDAXP_LP64_LDSTEXCL, ldaxp_64_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    stp xzr, xzr, [sp]
    ldaxp x3, x4, [sp]
    clrex
    add sp, sp, #16
TEST_END

TEST_BEGIN(STXP_SP64_LDSTEXCL, stxp_64_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    stp xzr, xzr, [sp]
    ldxp x3, x4, [sp]
    stxp w5, x6, x6, [sp]
    stxp w7, xzr, xzr, [sp]
    add sp, sp, #16
TEST_END

TEST_BEGIN(STLXP_SP64_LDSTEXCL, stlxp_64_sp, 1)
TEST_INPUTS(0)
    sub sp, sp, #16
    stp xzr, xzr, [sp]
    ldxp x3, x4, [sp]
    stlxp w5, x6, x6, [sp]
    stlxp w7, xzr, xzr, [sp]
    add sp, sp, #16
TEST_END

TEST_BEGIN(CLREX_BN_SYSTEM, clrex_immediates, 1)
TEST_INPUTS(0)
    clrex #0
    clrex #1
    clrex #2
    clrex #3
    clrex #4
    clrex #5
    clrex #6
    clrex #7
    clrex #8
    clrex #9
    clrex #10
    clrex #11
    clrex #12
    clrex #13
    clrex #14
    clrex #15
TEST_END
