/**
 * \file crc16_5935.h
 * Precompiled CRC16 checksum table for polynom 0x5935 the old polynom
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
 * \author Patrik Harner, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \version 1.0
 */

#ifndef CRC16_5935_H_
#define CRC16_5935_H_

#include <stdint.h>

/**
 * \brief Precompiled table for CRC16 values
 * \see CRC16_POLY_5935
 * \see crc16Checksum_5935
 */
static const uint16_t PrecompiledCRC16_5935[] =
{
          0x0000, /** 00 */
          0x5935, /** 01 */
          0xB26A, /** 02 */
          0xEB5F, /** 03 */
          0x3DE1, /** 04 */
          0x64D4, /** 05 */
          0x8F8B, /** 06 */
          0xD6BE, /** 07 */
          0x7BC2, /** 08 */
          0x22F7, /** 09 */
          0xC9A8, /** 0A */
          0x909D, /** 0B */
          0x4623, /** 0C */
          0x1F16, /** 0D */
          0xF449, /** 0E */
          0xAD7C, /** 0F */
          0xF784, /** 10 */
          0xAEB1, /** 11 */
          0x45EE, /** 12 */
          0x1CDB, /** 13 */
          0xCA65, /** 14 */
          0x9350, /** 15 */
          0x780F, /** 16 */
          0x213A, /** 17 */
          0x8C46, /** 18 */
          0xD573, /** 19 */
          0x3E2C, /** 1A */
          0x6719, /** 1B */
          0xB1A7, /** 1C */
          0xE892, /** 1D */
          0x03CD, /** 1E */
          0x5AF8, /** 1F */
          0xB63D, /** 20 */
          0xEF08, /** 21 */
          0x0457, /** 22 */
          0x5D62, /** 23 */
          0x8BDC, /** 24 */
          0xD2E9, /** 25 */
          0x39B6, /** 26 */
          0x6083, /** 27 */
          0xCDFF, /** 28 */
          0x94CA, /** 29 */
          0x7F95, /** 2A */
          0x26A0, /** 2B */
          0xF01E, /** 2C */
          0xA92B, /** 2D */
          0x4274, /** 2E */
          0x1B41, /** 2F */
          0x41B9, /** 30 */
          0x188C, /** 31 */
          0xF3D3, /** 32 */
          0xAAE6, /** 33 */
          0x7C58, /** 34 */
          0x256D, /** 35 */
          0xCE32, /** 36 */
          0x9707, /** 37 */
          0x3A7B, /** 38 */
          0x634E, /** 39 */
          0x8811, /** 3A */
          0xD124, /** 3B */
          0x079A, /** 3C */
          0x5EAF, /** 3D */
          0xB5F0, /** 3E */
          0xECC5, /** 3F */
          0x354F, /** 40 */
          0x6C7A, /** 41 */
          0x8725, /** 42 */
          0xDE10, /** 43 */
          0x08AE, /** 44 */
          0x519B, /** 45 */
          0xBAC4, /** 46 */
          0xE3F1, /** 47 */
          0x4E8D, /** 48 */
          0x17B8, /** 49 */
          0xFCE7, /** 4A */
          0xA5D2, /** 4B */
          0x736C, /** 4C */
          0x2A59, /** 4D */
          0xC106, /** 4E */
          0x9833, /** 4F */
          0xC2CB, /** 50 */
          0x9BFE, /** 51 */
          0x70A1, /** 52 */
          0x2994, /** 53 */
          0xFF2A, /** 54 */
          0xA61F, /** 55 */
          0x4D40, /** 56 */
          0x1475, /** 57 */
          0xB909, /** 58 */
          0xE03C, /** 59 */
          0x0B63, /** 5A */
          0x5256, /** 5B */
          0x84E8, /** 5C */
          0xDDDD, /** 5D */
          0x3682, /** 5E */
          0x6FB7, /** 5F */
          0x8372, /** 60 */
          0xDA47, /** 61 */
          0x3118, /** 62 */
          0x682D, /** 63 */
          0xBE93, /** 64 */
          0xE7A6, /** 65 */
          0x0CF9, /** 66 */
          0x55CC, /** 67 */
          0xF8B0, /** 68 */
          0xA185, /** 69 */
          0x4ADA, /** 6A */
          0x13EF, /** 6B */
          0xC551, /** 6C */
          0x9C64, /** 6D */
          0x773B, /** 6E */
          0x2E0E, /** 6F */
          0x74F6, /** 70 */
          0x2DC3, /** 71 */
          0xC69C, /** 72 */
          0x9FA9, /** 73 */
          0x4917, /** 74 */
          0x1022, /** 75 */
          0xFB7D, /** 76 */
          0xA248, /** 77 */
          0x0F34, /** 78 */
          0x5601, /** 79 */
          0xBD5E, /** 7A */
          0xE46B, /** 7B */
          0x32D5, /** 7C */
          0x6BE0, /** 7D */
          0x80BF, /** 7E */
          0xD98A, /** 7F */
          0x6A9E, /** 80 */
          0x33AB, /** 81 */
          0xD8F4, /** 82 */
          0x81C1, /** 83 */
          0x577F, /** 84 */
          0x0E4A, /** 85 */
          0xE515, /** 86 */
          0xBC20, /** 87 */
          0x115C, /** 88 */
          0x4869, /** 89 */
          0xA336, /** 8A */
          0xFA03, /** 8B */
          0x2CBD, /** 8C */
          0x7588, /** 8D */
          0x9ED7, /** 8E */
          0xC7E2, /** 8F */
          0x9D1A, /** 90 */
          0xC42F, /** 91 */
          0x2F70, /** 92 */
          0x7645, /** 93 */
          0xA0FB, /** 94 */
          0xF9CE, /** 95 */
          0x1291, /** 96 */
          0x4BA4, /** 97 */
          0xE6D8, /** 98 */
          0xBFED, /** 99 */
          0x54B2, /** 9A */
          0x0D87, /** 9B */
          0xDB39, /** 9C */
          0x820C, /** 9D */
          0x6953, /** 9E */
          0x3066, /** 9F */
          0xDCA3, /** A0 */
          0x8596, /** A1 */
          0x6EC9, /** A2 */
          0x37FC, /** A3 */
          0xE142, /** A4 */
          0xB877, /** A5 */
          0x5328, /** A6 */
          0x0A1D, /** A7 */
          0xA761, /** A8 */
          0xFE54, /** A9 */
          0x150B, /** AA */
          0x4C3E, /** AB */
          0x9A80, /** AC */
          0xC3B5, /** AD */
          0x28EA, /** AE */
          0x71DF, /** AF */
          0x2B27, /** B0 */
          0x7212, /** B1 */
          0x994D, /** B2 */
          0xC078, /** B3 */
          0x16C6, /** B4 */
          0x4FF3, /** B5 */
          0xA4AC, /** B6 */
          0xFD99, /** B7 */
          0x50E5, /** B8 */
          0x09D0, /** B9 */
          0xE28F, /** BA */
          0xBBBA, /** BB */
          0x6D04, /** BC */
          0x3431, /** BD */
          0xDF6E, /** BE */
          0x865B, /** BF */
          0x5FD1, /** C0 */
          0x06E4, /** C1 */
          0xEDBB, /** C2 */
          0xB48E, /** C3 */
          0x6230, /** C4 */
          0x3B05, /** C5 */
          0xD05A, /** C6 */
          0x896F, /** C7 */
          0x2413, /** C8 */
          0x7D26, /** C9 */
          0x9679, /** CA */
          0xCF4C, /** CB */
          0x19F2, /** CC */
          0x40C7, /** CD */
          0xAB98, /** CE */
          0xF2AD, /** CF */
          0xA855, /** D0 */
          0xF160, /** D1 */
          0x1A3F, /** D2 */
          0x430A, /** D3 */
          0x95B4, /** D4 */
          0xCC81, /** D5 */
          0x27DE, /** D6 */
          0x7EEB, /** D7 */
          0xD397, /** D8 */
          0x8AA2, /** D9 */
          0x61FD, /** DA */
          0x38C8, /** DB */
          0xEE76, /** DC */
          0xB743, /** DD */
          0x5C1C, /** DE */
          0x0529, /** DF */
          0xE9EC, /** E0 */
          0xB0D9, /** E1 */
          0x5B86, /** E2 */
          0x02B3, /** E3 */
          0xD40D, /** E4 */
          0x8D38, /** E5 */
          0x6667, /** E6 */
          0x3F52, /** E7 */
          0x922E, /** E8 */
          0xCB1B, /** E9 */
          0x2044, /** EA */
          0x7971, /** EB */
          0xAFCF, /** EC */
          0xF6FA, /** ED */
          0x1DA5, /** EE */
          0x4490, /** EF */
          0x1E68, /** F0 */
          0x475D, /** F1 */
          0xAC02, /** F2 */
          0xF537, /** F3 */
          0x2389, /** F4 */
          0x7ABC, /** F5 */
          0x91E3, /** F6 */
          0xC8D6, /** F7 */
          0x65AA, /** F8 */
          0x3C9F, /** F9 */
          0xD7C0, /** FA */
          0x8EF5, /** FB */
          0x584B, /** FC */
          0x017E, /** FD */
          0xEA21, /** FE */
          0xB314, /** FF */
};

#endif /* CRC16_5935_H_ */

/**
 * \}
 * \}
 */
