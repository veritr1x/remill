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

/* Run.cpp repeats each input with every NZCV combination, including both
 * carry inputs. Exercise unsigned carry, signed overflow, zero, register
 * overlap, zero-register reads and discarded writes. W-register writes must
 * clear the upper half of the destination. ADC must preserve all flags. */
#define ADDCARRY_INPUTS \
    TEST_INPUTS( \
        0, 0, 0, 1, 1, 0, 1, 1, \
        0xffffffff, 0, 0xffffffff, 1, 0xffffffff, 0xffffffff, \
        0x7fffffff, 0, 0x7fffffff, 1, 0x80000000, 0xffffffff, \
        0x80000000, 0x80000000, \
        0xffffffffffffffff, 0, 0xffffffffffffffff, 1, \
        0xffffffffffffffff, 0xffffffffffffffff, \
        0x7fffffffffffffff, 0, 0x7fffffffffffffff, 1, \
        0x8000000000000000, 0xffffffffffffffff, \
        0x8000000000000000, 0x8000000000000000)

#define ADDCARRY_TEST(isel, name, op, dst, lhs, rhs) \
    TEST_BEGIN(isel, name, 2) \
    ADDCARRY_INPUTS \
    op dst, lhs, rhs ; \
    TEST_END

#define ADDCARRY_WIDTH(isel, op, suffix, dst, lhs, rhs, zero) \
    ADDCARRY_TEST(isel, CAT3(op, _normal_, suffix), op, dst, lhs, rhs) \
    ADDCARRY_TEST(isel, CAT3(op, _lhs_overlap_, suffix), op, lhs, lhs, rhs) \
    ADDCARRY_TEST(isel, CAT3(op, _rhs_overlap_, suffix), op, rhs, lhs, rhs) \
    ADDCARRY_TEST(isel, CAT3(op, _zero_lhs_, suffix), op, dst, zero, rhs) \
    ADDCARRY_TEST(isel, CAT3(op, _zero_rhs_, suffix), op, dst, lhs, zero) \
    ADDCARRY_TEST(isel, CAT3(op, _discard_, suffix), op, zero, lhs, rhs)

ADDCARRY_WIDTH(ADC_32_ADDSUB_CARRY, adc, w, w9, ARG1_32, ARG2_32, wzr)
ADDCARRY_WIDTH(ADC_64_ADDSUB_CARRY, adc, x, x9, ARG1_64, ARG2_64, xzr)
ADDCARRY_WIDTH(ADCS_32_ADDSUB_CARRY, adcs, w, w9, ARG1_32, ARG2_32, wzr)
ADDCARRY_WIDTH(ADCS_64_ADDSUB_CARRY, adcs, x, x9, ARG1_64, ARG2_64, xzr)

#undef ADDCARRY_WIDTH
#undef ADDCARRY_TEST
#undef ADDCARRY_INPUTS
