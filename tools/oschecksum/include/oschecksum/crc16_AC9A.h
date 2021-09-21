/**
 * \file crc16_AC9A.h
 * Precompiled CRC16 checksum table for polynom 0xAC9A the old polynom
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

#ifndef CRC16_AC9A_H_
#define CRC16_AC9A_H_

#include <stdint.h>

/**
 * \brief Precompiled table for CRC16 values
 * \see CRC16_POLY_AC9A
 * \see crc16Checksum_AC9A
 */
static const uint16_t PrecompiledCRC16_AC9A[] =
{
        0x0000, /** 00 */
        0xAC9A, /** 01 */
        0xF5AE, /** 02 */
        0x5934, /** 03 */
        0x47C6, /** 04 */
        0xEB5C, /** 05 */
        0xB268, /** 06 */
        0x1EF2, /** 07 */
        0x8F8C, /** 08 */
        0x2316, /** 09 */
        0x7A22, /** 0A */
        0xD6B8, /** 0B */
        0xC84A, /** 0C */
        0x64D0, /** 0D */
        0x3DE4, /** 0E */
        0x917E, /** 0F */
        0xB382, /** 10 */
        0x1F18, /** 11 */
        0x462C, /** 12 */
        0xEAB6, /** 13 */
        0xF444, /** 14 */
        0x58DE, /** 15 */
        0x01EA, /** 16 */
        0xAD70, /** 17 */
        0x3C0E, /** 18 */
        0x9094, /** 19 */
        0xC9A0, /** 1A */
        0x653A, /** 1B */
        0x7BC8, /** 1C */
        0xD752, /** 1D */
        0x8E66, /** 1E */
        0x22FC, /** 1F */
        0xCB9E, /** 20 */
        0x6704, /** 21 */
        0x3E30, /** 22 */
        0x92AA, /** 23 */
        0x8C58, /** 24 */
        0x20C2, /** 25 */
        0x79F6, /** 26 */
        0xD56C, /** 27 */
        0x4412, /** 28 */
        0xE888, /** 29 */
        0xB1BC, /** 2A */
        0x1D26, /** 2B */
        0x03D4, /** 2C */
        0xAF4E, /** 2D */
        0xF67A, /** 2E */
        0x5AE0, /** 2F */
        0x781C, /** 30 */
        0xD486, /** 31 */
        0x8DB2, /** 32 */
        0x2128, /** 33 */
        0x3FDA, /** 34 */
        0x9340, /** 35 */
        0xCA74, /** 36 */
        0x66EE, /** 37 */
        0xF790, /** 38 */
        0x5B0A, /** 39 */
        0x023E, /** 3A */
        0xAEA4, /** 3B */
        0xB056, /** 3C */
        0x1CCC, /** 3D */
        0x45F8, /** 3E */
        0xE962, /** 3F */
        0x3BA6, /** 40 */
        0x973C, /** 41 */
        0xCE08, /** 42 */
        0x6292, /** 43 */
        0x7C60, /** 44 */
        0xD0FA, /** 45 */
        0x89CE, /** 46 */
        0x2554, /** 47 */
        0xB42A, /** 48 */
        0x18B0, /** 49 */
        0x4184, /** 4A */
        0xED1E, /** 4B */
        0xF3EC, /** 4C */
        0x5F76, /** 4D */
        0x0642, /** 4E */
        0xAAD8, /** 4F */
        0x8824, /** 50 */
        0x24BE, /** 51 */
        0x7D8A, /** 52 */
        0xD110, /** 53 */
        0xCFE2, /** 54 */
        0x6378, /** 55 */
        0x3A4C, /** 56 */
        0x96D6, /** 57 */
        0x07A8, /** 58 */
        0xAB32, /** 59 */
        0xF206, /** 5A */
        0x5E9C, /** 5B */
        0x406E, /** 5C */
        0xECF4, /** 5D */
        0xB5C0, /** 5E */
        0x195A, /** 5F */
        0xF038, /** 60 */
        0x5CA2, /** 61 */
        0x0596, /** 62 */
        0xA90C, /** 63 */
        0xB7FE, /** 64 */
        0x1B64, /** 65 */
        0x4250, /** 66 */
        0xEECA, /** 67 */
        0x7FB4, /** 68 */
        0xD32E, /** 69 */
        0x8A1A, /** 6A */
        0x2680, /** 6B */
        0x3872, /** 6C */
        0x94E8, /** 6D */
        0xCDDC, /** 6E */
        0x6146, /** 6F */
        0x43BA, /** 70 */
        0xEF20, /** 71 */
        0xB614, /** 72 */
        0x1A8E, /** 73 */
        0x047C, /** 74 */
        0xA8E6, /** 75 */
        0xF1D2, /** 76 */
        0x5D48, /** 77 */
        0xCC36, /** 78 */
        0x60AC, /** 79 */
        0x3998, /** 7A */
        0x9502, /** 7B */
        0x8BF0, /** 7C */
        0x276A, /** 7D */
        0x7E5E, /** 7E */
        0xD2C4, /** 7F */
        0x774C, /** 80 */
        0xDBD6, /** 81 */
        0x82E2, /** 82 */
        0x2E78, /** 83 */
        0x308A, /** 84 */
        0x9C10, /** 85 */
        0xC524, /** 86 */
        0x69BE, /** 87 */
        0xF8C0, /** 88 */
        0x545A, /** 89 */
        0x0D6E, /** 8A */
        0xA1F4, /** 8B */
        0xBF06, /** 8C */
        0x139C, /** 8D */
        0x4AA8, /** 8E */
        0xE632, /** 8F */
        0xC4CE, /** 90 */
        0x6854, /** 91 */
        0x3160, /** 92 */
        0x9DFA, /** 93 */
        0x8308, /** 94 */
        0x2F92, /** 95 */
        0x76A6, /** 96 */
        0xDA3C, /** 97 */
        0x4B42, /** 98 */
        0xE7D8, /** 99 */
        0xBEEC, /** 9A */
        0x1276, /** 9B */
        0x0C84, /** 9C */
        0xA01E, /** 9D */
        0xF92A, /** 9E */
        0x55B0, /** 9F */
        0xBCD2, /** A0 */
        0x1048, /** A1 */
        0x497C, /** A2 */
        0xE5E6, /** A3 */
        0xFB14, /** A4 */
        0x578E, /** A5 */
        0x0EBA, /** A6 */
        0xA220, /** A7 */
        0x335E, /** A8 */
        0x9FC4, /** A9 */
        0xC6F0, /** AA */
        0x6A6A, /** AB */
        0x7498, /** AC */
        0xD802, /** AD */
        0x8136, /** AE */
        0x2DAC, /** AF */
        0x0F50, /** B0 */
        0xA3CA, /** B1 */
        0xFAFE, /** B2 */
        0x5664, /** B3 */
        0x4896, /** B4 */
        0xE40C, /** B5 */
        0xBD38, /** B6 */
        0x11A2, /** B7 */
        0x80DC, /** B8 */
        0x2C46, /** B9 */
        0x7572, /** BA */
        0xD9E8, /** BB */
        0xC71A, /** BC */
        0x6B80, /** BD */
        0x32B4, /** BE */
        0x9E2E, /** BF */
        0x4CEA, /** C0 */
        0xE070, /** C1 */
        0xB944, /** C2 */
        0x15DE, /** C3 */
        0x0B2C, /** C4 */
        0xA7B6, /** C5 */
        0xFE82, /** C6 */
        0x5218, /** C7 */
        0xC366, /** C8 */
        0x6FFC, /** C9 */
        0x36C8, /** CA */
        0x9A52, /** CB */
        0x84A0, /** CC */
        0x283A, /** CD */
        0x710E, /** CE */
        0xDD94, /** CF */
        0xFF68, /** D0 */
        0x53F2, /** D1 */
        0x0AC6, /** D2 */
        0xA65C, /** D3 */
        0xB8AE, /** D4 */
        0x1434, /** D5 */
        0x4D00, /** D6 */
        0xE19A, /** D7 */
        0x70E4, /** D8 */
        0xDC7E, /** D9 */
        0x854A, /** DA */
        0x29D0, /** DB */
        0x3722, /** DC */
        0x9BB8, /** DD */
        0xC28C, /** DE */
        0x6E16, /** DF */
        0x8774, /** E0 */
        0x2BEE, /** E1 */
        0x72DA, /** E2 */
        0xDE40, /** E3 */
        0xC0B2, /** E4 */
        0x6C28, /** E5 */
        0x351C, /** E6 */
        0x9986, /** E7 */
        0x08F8, /** E8 */
        0xA462, /** E9 */
        0xFD56, /** EA */
        0x51CC, /** EB */
        0x4F3E, /** EC */
        0xE3A4, /** ED */
        0xBA90, /** EE */
        0x160A, /** EF */
        0x34F6, /** F0 */
        0x986C, /** F1 */
        0xC158, /** F2 */
        0x6DC2, /** F3 */
        0x7330, /** F4 */
        0xDFAA, /** F5 */
        0x869E, /** F6 */
        0x2A04, /** F7 */
        0xBB7A, /** F8 */
        0x17E0, /** F9 */
        0x4ED4, /** FA */
        0xE24E, /** FB */
        0xFCBC, /** FC */
        0x5026, /** FD */
        0x0912, /** FE */
        0xA588, /** FF */
};

#endif /* CRC16_H_ */

/**
 * \}
 * \}
 */
