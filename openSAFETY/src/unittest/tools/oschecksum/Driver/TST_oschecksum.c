/**
 * \file TST_oschecksum.c
 * Specifies the unittests for the functions \ref crc8ChecksumCalculator and \ref crc8Checksum
 * \addtogroup unittest
 * \{
 * \addtogroup tools
 * \{
 * \addtogroup oschecksum
 * \{
 * \addtogroup liboschecksum_unittest
 * \{
 * \details
 * \{
 * Copyright (c) 2011,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the BSD license (according to License.txt).
 * \}
 * \author Patrik Harner, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \version 1.0
 */

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <cunit/CUnit.h>
#include "TST_oschecksum_Unit.h"
#include <oschecksum/crc.h>

/*Initialization for the test, no effects at all*/
int TST_CRC_INIT(void)
{
    return 0;
}

static CU_TestInfo crc8Tests[] = {
    { "[CRC8] Check pre-calculated against calculated", TST_CRC8_cksum_calc_prec },
    { "[CRC8] Protocol implementation against pre-calculated", TST_CRC8_cksum_protocol_prec },
    { "[CRC8] Protocol implementation against implemented calculation", TST_CRC8_cksum_protocol_calc },
    { "[CRC8] Random subframe tests", TST_CRC8_rnd_subframe },
    CU_TEST_INFO_NULL,
};

static CU_TestInfo crc16Tests[] = {
    { "[CRC16] Check pre-calculated against calculated", TST_CRC16_cksum_calc_prec },
    { "[CRC16] Check pre-calculated against calculated {Polynom AC9A}", TST_CRC16_AC9A_cksum_calc_prec },
    { "[CRC16] Protocol implementation against pre-calculated", TST_CRC16_cksum_protocol_prec},
    { "[CRC16] Protocol implementation against implemented calculation", TST_CRC16_cksum_protocol_calc },
    { "[CRC16] Random subframe tests", TST_CRC16_rnd_subframe },
    { "[CRC16] Test checksum polynom for basic functionality", TST_CRC16_checksum_poly },
    CU_TEST_INFO_NULL,
};

static CU_TestInfo crc32Tests[] = {
    { "[CRC32] Check pre-calculated against calculated", TST_CRC32_cksum_calc_prec },
    { "[CRC32PNG] Check pre-calculated against calculated", TST_CRC32_PNG_cksum_calc_prec },
    { "[CRC32] Parameter CRC check", TST_paramcrcCheck },
    CU_TEST_INFO_NULL,
};

static CU_SuiteInfo suites[] = {
    { "CRC8 Unittests", TST_CRC_INIT, NULL, crc8Tests },
    { "CRC16 Unittests", TST_CRC_INIT, NULL, crc16Tests },
    { "CRC32 Unittests", TST_CRC_INIT, NULL, crc32Tests },
    CU_SUITE_INFO_NULL,
};


/*Add tests to the suites*/
void TST_AddTests(void)
{
    assert(NULL != CU_get_registry());
    assert(!CU_is_test_running());

    /* Register suites. */
    if (CU_register_suites(suites) != CUE_SUCCESS) {
        fprintf(stderr, "suite registration failed - %s\n", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }

}/*TST_AddTests()*/

/**
* \}
* \}
* \}
* \}
*/
