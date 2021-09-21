/**
 * \file tablecreate.c
 * \addtogroup Tools
 * \{
 * \addtogroup oschecksum
 * \{
 * \addtogroup tablecreate TableCreate
 * This small utility will use the CRC checksum library to print out a table for each given CRC type. This utility has
 * to be built explicitly via the Makefile option "tablecreate" and will not be part of the default build.
 * \{
 * \details
 * \{
 * Copyright (c) 2011,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the BSD license (according to License.txt).
 * \}
 * \author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \version 1.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <oschecksum/crc.h>

/** \brief An enumeration for the possible CRC types */
enum eCRC_TYPE
{
        CRC8,           /**< CRC8 table type */
        CRC16,          /**< CRC16 table type */
        CRC16_AC9A,     /**< CRC16_AC9A table type */
        CRC32,          /**< CRC32 table type */
        CRC32_PNG,      /**< CRC32_PNG table type */
};

/**
 * \brief TableCreate will create a CRC table for the given table type. For possible CRC types see \ref eCRC_TYPE
 * \param argc number of arguments given
 * \param argv arguments given
 * \return 1 if the wrong number of arguments have been given, 0 otherwise
 */
int main (int argc, char ** argv)
{
        int i;
        enum eCRC_TYPE myCRCTYPE;
        uint8_t data[1];

        myCRCTYPE = CRC8;

        if ( argc != 2 )
        {
                printf ( "Wrong number of arguments!\n\tUsage: %s {CRC8|CRC16|CRC16_AC9A|CRC32|CRC32_PNG}\n\n", argv[0]);
                return 1;
        }

        if ( strcmp ( argv[1], "CRC8" ) == 0 )
                myCRCTYPE = CRC8;
        else if ( strcmp ( argv[1], "CRC16" ) == 0 )
                myCRCTYPE = CRC16;
        else if ( strcmp ( argv[1], "CRC16_AC9A" ) == 0 )
                myCRCTYPE = CRC16_AC9A;
        else if ( strcmp ( argv[1], "CRC32" ) == 0 )
                myCRCTYPE = CRC32;
        else if ( strcmp ( argv[1], "CRC32_PNG" ) == 0 )
                myCRCTYPE = CRC32_PNG;

        for ( i = 0; i < 256; i ++ )
        {
                data[0] = i;
                switch ( myCRCTYPE )
                {
                case CRC8:
                        printf ( "        0x%02X, /** %02X */\n", crc8ChecksumCalculator(1, data, 0), i); break;
                case CRC16:
                        printf ( "        0x%04X, /** %02X */\n", crc16ChecksumCalculator(1, data, 0), i); break;
                case CRC16_AC9A:
                        printf ( "          0x%04X, /** %02X */\n", crc16ChecksumCalculator_AC9A(1, data, 0), i); break;
                case CRC32:
                        printf ( "        0x%08X, /** %02X */\n", crc32ChecksumCalculator(1, data, 0), i); break;
                case CRC32_PNG:
                        printf ( "          0x%08X, /** %02X */\n", crc32ChecksumPNG_Calculator(1, data, 0), i); break;
                default:
                        break;
                }
        }

        return 0;
}

/**
 * \}
 * \}
 * \}
 */
