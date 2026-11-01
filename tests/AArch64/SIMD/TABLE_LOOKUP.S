/* Copyright (c) 2026 Trail of Bits, Inc.
 * Licensed under the Apache License, Version 2.0.
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#define TABLE_CASE(arr, upper, r5, r31, r4) \
    TEST_BEGIN(CAT(TBL_ASIMDTBL_L1_1_, upper), CAT(table_lookup_, arr), 1) \
    TEST_INPUTS(0) \
    tbl r5, {v31.16b}, r4 ; \
    tbl r31, {v31.16b, v0.16b}, r4 ; \
    tbl r4, {v31.16b, v0.16b, v1.16b}, r4 ; \
    tbl r5, {v31.16b, v0.16b, v1.16b, v2.16b}, r4 ; \
    tbx r5, {v31.16b}, r4 ; \
    tbx r31, {v31.16b, v0.16b}, r4 ; \
    tbx r4, {v31.16b, v0.16b, v1.16b}, r4 ; \
    tbx r5, {v31.16b, v0.16b, v1.16b, v2.16b}, r4 ; \
    TEST_END
TABLE_CASE(8b, 8B, v5.8b, v31.8b, v4.8b)
TABLE_CASE(16b, 16B, v5.16b, v31.16b, v4.16b)
#undef TABLE_CASE
