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

typedef struct sSODEntries {
    uint16_t index;
    uint8_t subindex;
    uint8_t length;
    uint32_t data;
} tSODEntries;

tSODEntries dataValues [] = {
    { (uint16_t) 0x100C, (uint8_t) 0x01, (uint8_t) 4, (uint32_t) 0x0001D4C0 },
    { 0x100C, 0x02, 1, 0x05 },
    { 0x1200, 0x03, 1, 0x02 },
    { 0x1400, 0x01, 2, 0x008 },
    { 0x1400, 0x02, 4, 0x00000032 },
    { 0x1400, 0x03, 1, 0x0D },
    { 0x1400, 0x04, 4, 0x0000028A },
    { 0x1400, 0x05, 4, 0x00000CB2 },
    { 0x1400, 0x06, 2, 0x0007 },
    { 0x1400, 0x07, 2, 0x01C3 },
    { 0x1400, 0x08, 2, 0x0003 },
    { 0x1400, 0x09, 2, 0x01BF },
    { 0x1400, 0x0A, 2, 0x0004 },
    { 0x1400, 0x0B, 4, 0x00003552 },
    { 0x1400, 0x0C, 2, 0x0001 },
    { 0x1800, 0x00, 1, 0x02 },
    { 0x1800, 0x01, 4, 0x62000108 },
    { 0x1800, 0x02, 4, 0x62000210 },
    { 0x1800, 0x03, 4, 0x00000000 },
    { 0x1800, 0x04, 4, 0x00000000 },
    { 0x1C00, 0x01, 2, 0x0007 },
    { 0x1C00, 0x02, 2, 0x0001 },
    { 0x1C00, 0x03, 1, 0x0D },
    { 0x2000, 0x00, 5, 0x00 },
    { 0x2001, 0x00, 18, 0x00 },
    { 0xC000, 0x00, 1, 0x02 },
    { 0xC000, 0x01, 4, 0x60000108 },
    { 0xC000, 0x02, 4, 0x60000210 },
    { 0xC000, 0x03, 4, 0x00000000 },
    { 0xC000, 0x04, 4, 0x00000000 }
};

uint8_t tSOD0x2000 [] = {
    0x02, 0x00, 0x08, 0x00, 0x00
};

uint8_t tSOD0x2001 [] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00
};

uint32_t EXPECTED_CRC = 0x7F6FAE78;

static uint8_t * copyData ( uint8_t * data, uint8_t len )
{
    uint8_t *buf = (void *)0;
    uint8_t ctr = 0;

    buf = (uint8_t *) malloc ( len );
    memset ( buf, 0, len );
    memcpy ( buf, data, len );

    return buf;
}

/** \brief test the consistency from the CRC32 precompiled parameter set and the calculated set */
void TST_paramcrcCheck(void)
{
    uint8_t ctr = 0, datactr = 0, nr = 0;
    uint32_t crc32 = 0;
    uint8_t * data = (void *)0;

    nr = sizeof(dataValues) / sizeof(tSODEntries);

    for ( ctr = 0; ctr < nr ; ctr++ )
    {
        data = (void *)0;

        if ( dataValues[ctr].index == 0x2000 )
            data = copyData ( (uint8_t*)&tSOD0x2000, dataValues[ctr].length );
        else if ( dataValues[ctr].index == 0x2001 )
            data = copyData ( (uint8_t*)&tSOD0x2001, dataValues[ctr].length );
        else
            data = copyData ( (uint8_t*)&dataValues[ctr].data, dataValues[ctr].length );

        crc32 = crc32Checksum(dataValues[ctr].length, (const uint8_t*) data, crc32 );

        free(data);
    }

    CU_ASSERT_EQUAL ( crc32, EXPECTED_CRC );
}

/**
* \}
* \}
* \}
* \}
*/
