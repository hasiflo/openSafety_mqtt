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
#include <time.h>

#include <cunit/CUnit.h>
#include <oschecksum/crc.h>
#include "TST_oschecksum_Unit.h"


/** \brief test the consistency from the CRC8 precompiled value and the calculated value */
void TST_CRC8_cksum_calc_prec(void)
{
    uint16_t ctr = 0;
    uint8_t count[1] = {0};

    for ( ctr = 0; ctr < 256; ctr++ )
    {
        count[0] = (uint8_t) ctr;
        CU_ASSERT_EQUAL_FATAL(crc8ChecksumCalculator(1, count, 0),
                              crc8Checksum(1, count, 0));
    }
}

/**
 * \brief test the consistency from the CRC8 precompiled value
 *  and the calculated value from the crc protocol
 *
 * \param none
 *
 * \return none
 */
void TST_CRC8_cksum_protocol_prec(void)
{
    uint16_t ctr = 0;
    uint8_t count[1] = {0};

    for ( ctr = 0; ctr < 256; ctr++ )
    {
        count[0] = (uint8_t) ctr;
        CU_ASSERT_EQUAL_FATAL(uc_openSAFETY_CRC8(1, CRC8_POLY, count),
                crc8Checksum(1, count, 0));
    }
}

/**
 * \brief test the consistency from the CRC8 calculated value
 *  and the calculated value from the crc protocol
 *
 * \param none
 *
 * \return none
 */
void TST_CRC8_cksum_protocol_calc(void)
{
    uint16_t ctr = 0;
    uint8_t count[1] = {0};

    for ( ctr = 0; ctr < 256; ctr++ )
    {
        count[0] = (uint8_t) ctr;
        CU_ASSERT_EQUAL_FATAL(uc_openSAFETY_CRC8(1, CRC8_POLY, count),
                crc8Checksum(1, count, 0));
    }

}

/**\brief
 * test the consistency from the CRC8 precompiled value and the calculated value
 */
void TST_CRC8_rnd_subframe(void)
{
    /** length of payload in reference sub frame */
    uint16_t payload = rand() % 8; /*modulo 8 to get values from 0 to 8*/

    /* length of reference sub frame 1 without CRC   */
    uint8_t c_sub_length1 = (payload + 4);

    /* length of reference sub frame 2 without CRC   */
    uint8_t c_sub_length2 = (payload + 5);

    /*first part of openSAFETY short telegram    */
    uint8_t *uc_subframe_1 = (uint8_t *)NULL;

    /*second part of openSAFETY short telegram   */
    uint8_t *uc_subframe_2 = (uint8_t *)NULL;

    uint8_t counter_var1 = 0;

    /* initializing telegram arrays */
    uc_subframe_1 = (uint8_t *) malloc ( sizeof(uint8_t) * c_sub_length1 + 2);
    uc_subframe_2 = (uint8_t *) malloc ( sizeof(uint8_t) * c_sub_length2 + 2);

    /*initialize subframe memory with zero*/
    memset((void *)uc_subframe_1, 0, c_sub_length1 + 2);
    memset((void *)uc_subframe_2, 0, c_sub_length2 + 2);

    /*feed the char arrays with random values*/
    for(counter_var1 = 0; counter_var1 < c_sub_length1; counter_var1++)
    {
        uc_subframe_1[counter_var1] = rand() % 256;
    }
    for(counter_var1 = 0; counter_var1 < c_sub_length2; counter_var1++)
    {
        uc_subframe_2[counter_var1] = rand() % 256;
    }

    CU_ASSERT_EQUAL_FATAL(uc_openSAFETY_CRC8(c_sub_length1, CRC8_POLY, uc_subframe_1),
                    crc8ChecksumCalculator(c_sub_length1, uc_subframe_1, 0));

    CU_ASSERT_EQUAL_FATAL(uc_openSAFETY_CRC8(c_sub_length2, CRC8_POLY, uc_subframe_2),
                    crc8ChecksumCalculator(c_sub_length2, uc_subframe_2, 0));

    free ( uc_subframe_1 );
    free ( uc_subframe_2 );
}


/**
* \}
* \}
* \}
* \}
*/
