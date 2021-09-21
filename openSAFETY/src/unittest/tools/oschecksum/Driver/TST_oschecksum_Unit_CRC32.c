/**
 * \file TST_oschecksum_Unit.c
 * Specifies the unittests for the functions CRC8checksumcalculator and CRC8checksum
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

#include <stdio.h>
#include <stdlib.h>

#include <cunit/CUnit.h>
#include <oschecksum/crc.h>
#include "TST_oschecksum_Unit.h"

#define DATA_LEN 1

/** \brief test the consistency from the CRC32 precompiled value and the calculated value */
void TST_CRC32_cksum_calc_prec(void)
{
    uint16_t ctr = 0;
    uint8_t count[1] = {0};

    for ( ctr = 0; ctr < 256; ctr++ )
    {
        count[0] = (uint8_t) ctr;
        //init 0
        CU_ASSERT_EQUAL_FATAL(crc32ChecksumCalculator(1, count, 0), crc32Checksum(1, count, 0));
        //init equal
        CU_ASSERT_EQUAL_FATAL(crc32ChecksumCalculator(1, count, 0x02), crc32Checksum(1, count, 0x02));
        //init unequal
        CU_ASSERT_NOT_EQUAL_FATAL(crc32ChecksumCalculator(1, count, 0x01), crc32Checksum(1, count, 0x02));
    }
}

/** \brief test the consistency from the CRC32_PNG precompiled value and the calculated value */
void TST_CRC32_PNG_cksum_calc_prec(void)
{
    uint16_t ctr = 0;
    uint8_t count[1] = {0};

    for ( ctr = 0; ctr < 256; ctr++ )
    {
        count[0] = (uint8_t) ctr;
        //init 0
        CU_ASSERT_EQUAL_FATAL(crc32ChecksumPNG_Calculator(1, count, 0), crc32ChecksumPNG(1, count, 0));
        //init equal
        CU_ASSERT_EQUAL_FATAL(crc32ChecksumPNG_Calculator(1, count, 2), crc32ChecksumPNG(1, count, 2));
        //init unequal
        CU_ASSERT_NOT_EQUAL_FATAL(crc32ChecksumPNG_Calculator(1, count, 1), crc32ChecksumPNG(1, count, 2));
    }
}

/**
* \}
* \}
* \}
* \}
*/
