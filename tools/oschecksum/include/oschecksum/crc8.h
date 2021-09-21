/**
 * \file crc8.h
 * Precompiled CRC8 checksum table for polynom 0x2F
 * \addtogroup Tools
 * \{
 * \addtogroup oschecksum
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

#ifndef CRC8_H_
#define CRC8_H_



#include <stdint.h>

/**
 * \brief Precompiled table for CRC8 values
 * \see CRC8_POLY
 * \see crc8Checksum
 */
static const uint8_t PrecompiledCRC8[] =
{
        0x00, /* 00 */
        0x2F, /* 01 */
        0x5E, /* 02 */
        0x71, /* 03 */
        0xBC, /* 04 */
        0x93, /* 05 */
        0xE2, /* 06 */
        0xCD, /* 07 */
        0x57, /* 08 */
        0x78, /* 09 */
        0x09, /* 0A */
        0x26, /* 0B */
        0xEB, /* 0C */
        0xC4, /* 0D */
        0xB5, /* 0E */
        0x9A, /* 0F */
        0xAE, /* 10 */
        0x81, /* 11 */
        0xF0, /* 12 */
        0xDF, /* 13 */
        0x12, /* 14 */
        0x3D, /* 15 */
        0x4C, /* 16 */
        0x63, /* 17 */
        0xF9, /* 18 */
        0xD6, /* 19 */
        0xA7, /* 1A */
        0x88, /* 1B */
        0x45, /* 1C */
        0x6A, /* 1D */
        0x1B, /* 1E */
        0x34, /* 1F */
        0x73, /* 20 */
        0x5C, /* 21 */
        0x2D, /* 22 */
        0x02, /* 23 */
        0xCF, /* 24 */
        0xE0, /* 25 */
        0x91, /* 26 */
        0xBE, /* 27 */
        0x24, /* 28 */
        0x0B, /* 29 */
        0x7A, /* 2A */
        0x55, /* 2B */
        0x98, /* 2C */
        0xB7, /* 2D */
        0xC6, /* 2E */
        0xE9, /* 2F */
        0xDD, /* 30 */
        0xF2, /* 31 */
        0x83, /* 32 */
        0xAC, /* 33 */
        0x61, /* 34 */
        0x4E, /* 35 */
        0x3F, /* 36 */
        0x10, /* 37 */
        0x8A, /* 38 */
        0xA5, /* 39 */
        0xD4, /* 3A */
        0xFB, /* 3B */
        0x36, /* 3C */
        0x19, /* 3D */
        0x68, /* 3E */
        0x47, /* 3F */
        0xE6, /* 40 */
        0xC9, /* 41 */
        0xB8, /* 42 */
        0x97, /* 43 */
        0x5A, /* 44 */
        0x75, /* 45 */
        0x04, /* 46 */
        0x2B, /* 47 */
        0xB1, /* 48 */
        0x9E, /* 49 */
        0xEF, /* 4A */
        0xC0, /* 4B */
        0x0D, /* 4C */
        0x22, /* 4D */
        0x53, /* 4E */
        0x7C, /* 4F */
        0x48, /* 50 */
        0x67, /* 51 */
        0x16, /* 52 */
        0x39, /* 53 */
        0xF4, /* 54 */
        0xDB, /* 55 */
        0xAA, /* 56 */
        0x85, /* 57 */
        0x1F, /* 58 */
        0x30, /* 59 */
        0x41, /* 5A */
        0x6E, /* 5B */
        0xA3, /* 5C */
        0x8C, /* 5D */
        0xFD, /* 5E */
        0xD2, /* 5F */
        0x95, /* 60 */
        0xBA, /* 61 */
        0xCB, /* 62 */
        0xE4, /* 63 */
        0x29, /* 64 */
        0x06, /* 65 */
        0x77, /* 66 */
        0x58, /* 67 */
        0xC2, /* 68 */
        0xED, /* 69 */
        0x9C, /* 6A */
        0xB3, /* 6B */
        0x7E, /* 6C */
        0x51, /* 6D */
        0x20, /* 6E */
        0x0F, /* 6F */
        0x3B, /* 70 */
        0x14, /* 71 */
        0x65, /* 72 */
        0x4A, /* 73 */
        0x87, /* 74 */
        0xA8, /* 75 */
        0xD9, /* 76 */
        0xF6, /* 77 */
        0x6C, /* 78 */
        0x43, /* 79 */
        0x32, /* 7A */
        0x1D, /* 7B */
        0xD0, /* 7C */
        0xFF, /* 7D */
        0x8E, /* 7E */
        0xA1, /* 7F */
        0xE3, /* 80 */
        0xCC, /* 81 */
        0xBD, /* 82 */
        0x92, /* 83 */
        0x5F, /* 84 */
        0x70, /* 85 */
        0x01, /* 86 */
        0x2E, /* 87 */
        0xB4, /* 88 */
        0x9B, /* 89 */
        0xEA, /* 8A */
        0xC5, /* 8B */
        0x08, /* 8C */
        0x27, /* 8D */
        0x56, /* 8E */
        0x79, /* 8F */
        0x4D, /* 90 */
        0x62, /* 91 */
        0x13, /* 92 */
        0x3C, /* 93 */
        0xF1, /* 94 */
        0xDE, /* 95 */
        0xAF, /* 96 */
        0x80, /* 97 */
        0x1A, /* 98 */
        0x35, /* 99 */
        0x44, /* 9A */
        0x6B, /* 9B */
        0xA6, /* 9C */
        0x89, /* 9D */
        0xF8, /* 9E */
        0xD7, /* 9F */
        0x90, /* A0 */
        0xBF, /* A1 */
        0xCE, /* A2 */
        0xE1, /* A3 */
        0x2C, /* A4 */
        0x03, /* A5 */
        0x72, /* A6 */
        0x5D, /* A7 */
        0xC7, /* A8 */
        0xE8, /* A9 */
        0x99, /* AA */
        0xB6, /* AB */
        0x7B, /* AC */
        0x54, /* AD */
        0x25, /* AE */
        0x0A, /* AF */
        0x3E, /* B0 */
        0x11, /* B1 */
        0x60, /* B2 */
        0x4F, /* B3 */
        0x82, /* B4 */
        0xAD, /* B5 */
        0xDC, /* B6 */
        0xF3, /* B7 */
        0x69, /* B8 */
        0x46, /* B9 */
        0x37, /* BA */
        0x18, /* BB */
        0xD5, /* BC */
        0xFA, /* BD */
        0x8B, /* BE */
        0xA4, /* BF */
        0x05, /* C0 */
        0x2A, /* C1 */
        0x5B, /* C2 */
        0x74, /* C3 */
        0xB9, /* C4 */
        0x96, /* C5 */
        0xE7, /* C6 */
        0xC8, /* C7 */
        0x52, /* C8 */
        0x7D, /* C9 */
        0x0C, /* CA */
        0x23, /* CB */
        0xEE, /* CC */
        0xC1, /* CD */
        0xB0, /* CE */
        0x9F, /* CF */
        0xAB, /* D0 */
        0x84, /* D1 */
        0xF5, /* D2 */
        0xDA, /* D3 */
        0x17, /* D4 */
        0x38, /* D5 */
        0x49, /* D6 */
        0x66, /* D7 */
        0xFC, /* D8 */
        0xD3, /* D9 */
        0xA2, /* DA */
        0x8D, /* DB */
        0x40, /* DC */
        0x6F, /* DD */
        0x1E, /* DE */
        0x31, /* DF */
        0x76, /* E0 */
        0x59, /* E1 */
        0x28, /* E2 */
        0x07, /* E3 */
        0xCA, /* E4 */
        0xE5, /* E5 */
        0x94, /* E6 */
        0xBB, /* E7 */
        0x21, /* E8 */
        0x0E, /* E9 */
        0x7F, /* EA */
        0x50, /* EB */
        0x9D, /* EC */
        0xB2, /* ED */
        0xC3, /* EE */
        0xEC, /* EF */
        0xD8, /* F0 */
        0xF7, /* F1 */
        0x86, /* F2 */
        0xA9, /* F3 */
        0x64, /* F4 */
        0x4B, /* F5 */
        0x3A, /* F6 */
        0x15, /* F7 */
        0x8F, /* F8 */
        0xA0, /* F9 */
        0xD1, /* FA */
        0xFE, /* FB */
        0x33, /* FC */
        0x1C, /* FD */
        0x6D, /* FE */
        0x42  /* FF */
};

#endif /* CRC8_H_ */

/**
 * \}
 * \}
 */
