/*
 * \file crc.c
 * \version 1.0
 */

#include <stdio.h>

#include <oschecksum/crc.h>

//#include <oschecksum/config.h>

uint32_t crc32ChecksumCalculator(uint32_t len, const uint8_t * pBuffer,
                uint32_t initCRC)
{
    uint32_t crc = initCRC;
    uint8_t k = 0;
    while (len-- > 0)
    {
        /* shift left by ( ( sizeof (crc ) * 2 ) - 8 ) */
        crc ^= (*pBuffer) << 24;
        for (k = 0; k < 8; k++)
        {
            //printf ( "Bit: %02d ; CRC: 0x%04X \n", k, crc, (crc & 0x8000), ((data >> k) & 0x01));
            if (crc & 0x80000000)
                crc = (crc << 1) ^ CRC32_POLY;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

uint32_t crc32Checksum(uint32_t len, const uint8_t * pBuffer, uint32_t initCRC)
{
    uint32_t crc = initCRC;

    while (len-- > 0)
    {
        uint32_t ulTab = PrecompiledCRC32[(*pBuffer) ^ (crc >> 24)];
        crc = (crc << 8) ^ ulTab;
        pBuffer++;
    }

    return crc;
}

uint32_t crc32ChecksumPNG_Calculator(uint32_t len, const uint8_t * pBuffer, uint32_t initCRC)
{
    uint8_t byte_length = 0;
    uint32_t crc = initCRC;

    len = len;

    crc ^= (uint32_t) *pBuffer ^ 0xffffffff;

    for (byte_length = 0; byte_length < 8; byte_length++)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ CRC32PNG_POLY;
        else
            crc = crc >> 1;
    }

    return crc ^ 0xffffffff;
}

uint32_t crc32ChecksumPNG(uint32_t len, const uint8_t * pBuffer, uint32_t initCRC)
{
    //initial value has to be 0, as the algorithm first of all inverts the crc value
    uint32_t crc = initCRC ^ 0xffffffff;

    while (len-- > 0)
    {
        crc = PrecompiledCRC32PNG[ ( crc ^ *pBuffer ) & 0xff ] ^ ( crc >> 8 );
        //next byte
        pBuffer++;
    }

    //another inversion will retrieve the crc
    return crc ^ 0xffffffff;
}

uint16_t crc16ChecksumCalculatorPoly(uint32_t len, const uint8_t * pBuffer,
                uint16_t initCRC, uint16_t Polynom)
{
    uint16_t crc = initCRC;
    uint8_t k = 0;
    while (len-- > 0)
    {
        /* shift left by ( ( sizeof (crc ) * 2 ) - 8 ) */
        crc ^= (*pBuffer) << 8;
        for (k = 0; k < 8; k++)
        {
            //printf ( "Bit: %02d ; CRC: 0x%04X \n", k, crc, (crc & 0x8000), ((data >> k) & 0x01));
            if (crc & 0x8000)
                crc = (crc << 1) ^ Polynom;
            else
                crc = (crc << 1);
        }
        pBuffer++;
    }
    return crc;
}

uint16_t crc16ChecksumCalculator_AC9A(uint32_t len, const uint8_t * pBuffer,
                uint16_t initCRC)
{
    return crc16ChecksumCalculatorPoly(len, pBuffer, initCRC,
                    (uint16_t) CRC16_POLY_AC9A);
}

uint16_t crc16ChecksumCalculator(uint32_t len, const uint8_t * pBuffer,
                uint16_t initCRC)
{
    return crc16ChecksumCalculatorPoly(len, pBuffer, initCRC,
                    (uint16_t) CRC16_POLY_BAAD);
}

uint16_t crc16ChecksumPoly(uint32_t len, const uint8_t * pBuffer,
                uint16_t initCRC, uint16_t Polynom)
{
    uint16_t crc = initCRC;
    uint16_t ulTab = 0;

    while (len-- > 0)
    {
        switch (Polynom)
        {
        case (CRC16_POLY_AC9A):
#ifndef CRC_ROTATED_ENABLE
            ulTab = PrecompiledCRC16_AC9A[(*pBuffer++) ^ (crc >> 8)];
#else
            ulTab = PrecompiledCRC16_5935[(*pBuffer++) ^ (crc >> 8)];
#endif
            crc = (crc << 8) ^ ulTab;
            break;

        case (CRC16_POLY_BAAD):
#ifndef CRC_ROTATED_ENABLE
            ulTab = PrecompiledCRC16_BAAD[(*pBuffer++) ^ (crc >> 8)];
#else
            ulTab = PrecompiledCRC16_755B[(*pBuffer++) ^ (crc >> 8)];
#endif
            crc = (crc << 8) ^ ulTab;
            break;
        default:
            return (uint16_t) 0;
        }
    }

    return crc;
}

uint16_t crc16Checksum_AC9A(uint32_t len, const uint8_t * pBuffer,
                uint16_t initCRC)
{
    return crc16ChecksumPoly(len, pBuffer, initCRC, CRC16_POLY_AC9A);
}

uint16_t crc16Checksum(uint32_t len, const uint8_t * pBuffer, uint16_t initCRC)
{
    return crc16ChecksumPoly(len, pBuffer, initCRC, CRC16_POLY_BAAD);
}

uint8_t crc8ChecksumCalculator(uint32_t len, const uint8_t * pBuffer,
                uint8_t initCRC)
{
    uint8_t crc = initCRC;
    uint8_t k = 0;
    while (len-- > 0)
    {
        crc ^= (*pBuffer);
        for (k = 0; k < 8; k++)
        {
            //printf ( "Bit: %02d ; CRC: 0x%04X \n", k, crc, (crc & 0x8000), ((data >> k) & 0x01));
            if (crc & 0x80)
                crc = (crc << 1) ^ CRC8_POLY;
            else
                crc = (crc << 1);
        }
        pBuffer++;
    }
    return crc;
}

uint8_t crc8Checksum(uint32_t len, const uint8_t * pBuffer, uint8_t initCRC)
{
    uint8_t crc = initCRC;

    while (len-- > 0)
    {
        uint8_t ulTab = PrecompiledCRC8[(*pBuffer++) ^ crc];
        crc = (crc << 8) ^ ulTab;
    }

    return crc;
}
