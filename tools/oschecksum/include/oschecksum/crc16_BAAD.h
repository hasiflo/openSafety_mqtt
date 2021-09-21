/**
 * \file crc16_BAAD.h
 * Precompiled CRC16 checksum table for polynom 0xBAAD
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

#ifndef CRC16_BAAD_H_
#define CRC16_BAAD_H_

#include <stdint.h>

/**
 * \brief Precompiled table for CRC16 values
 * \see CRC16_POLY
 * \see crc16Checksum
 */
static const uint16_t PrecompiledCRC16_BAAD[] =
{
        0x0000, /** 00 */
        0xBAAD, /** 01 */
        0xCFF7, /** 02 */
        0x755A, /** 03 */
        0x2543, /** 04 */
        0x9FEE, /** 05 */
        0xEAB4, /** 06 */
        0x5019, /** 07 */
        0x4A86, /** 08 */
        0xF02B, /** 09 */
        0x8571, /** 0A */
        0x3FDC, /** 0B */
        0x6FC5, /** 0C */
        0xD568, /** 0D */
        0xA032, /** 0E */
        0x1A9F, /** 0F */
        0x950C, /** 10 */
        0x2FA1, /** 11 */
        0x5AFB, /** 12 */
        0xE056, /** 13 */
        0xB04F, /** 14 */
        0x0AE2, /** 15 */
        0x7FB8, /** 16 */
        0xC515, /** 17 */
        0xDF8A, /** 18 */
        0x6527, /** 19 */
        0x107D, /** 1A */
        0xAAD0, /** 1B */
        0xFAC9, /** 1C */
        0x4064, /** 1D */
        0x353E, /** 1E */
        0x8F93, /** 1F */
        0x90B5, /** 20 */
        0x2A18, /** 21 */
        0x5F42, /** 22 */
        0xE5EF, /** 23 */
        0xB5F6, /** 24 */
        0x0F5B, /** 25 */
        0x7A01, /** 26 */
        0xC0AC, /** 27 */
        0xDA33, /** 28 */
        0x609E, /** 29 */
        0x15C4, /** 2A */
        0xAF69, /** 2B */
        0xFF70, /** 2C */
        0x45DD, /** 2D */
        0x3087, /** 2E */
        0x8A2A, /** 2F */
        0x05B9, /** 30 */
        0xBF14, /** 31 */
        0xCA4E, /** 32 */
        0x70E3, /** 33 */
        0x20FA, /** 34 */
        0x9A57, /** 35 */
        0xEF0D, /** 36 */
        0x55A0, /** 37 */
        0x4F3F, /** 38 */
        0xF592, /** 39 */
        0x80C8, /** 3A */
        0x3A65, /** 3B */
        0x6A7C, /** 3C */
        0xD0D1, /** 3D */
        0xA58B, /** 3E */
        0x1F26, /** 3F */
        0x9BC7, /** 40 */
        0x216A, /** 41 */
        0x5430, /** 42 */
        0xEE9D, /** 43 */
        0xBE84, /** 44 */
        0x0429, /** 45 */
        0x7173, /** 46 */
        0xCBDE, /** 47 */
        0xD141, /** 48 */
        0x6BEC, /** 49 */
        0x1EB6, /** 4A */
        0xA41B, /** 4B */
        0xF402, /** 4C */
        0x4EAF, /** 4D */
        0x3BF5, /** 4E */
        0x8158, /** 4F */
        0x0ECB, /** 50 */
        0xB466, /** 51 */
        0xC13C, /** 52 */
        0x7B91, /** 53 */
        0x2B88, /** 54 */
        0x9125, /** 55 */
        0xE47F, /** 56 */
        0x5ED2, /** 57 */
        0x444D, /** 58 */
        0xFEE0, /** 59 */
        0x8BBA, /** 5A */
        0x3117, /** 5B */
        0x610E, /** 5C */
        0xDBA3, /** 5D */
        0xAEF9, /** 5E */
        0x1454, /** 5F */
        0x0B72, /** 60 */
        0xB1DF, /** 61 */
        0xC485, /** 62 */
        0x7E28, /** 63 */
        0x2E31, /** 64 */
        0x949C, /** 65 */
        0xE1C6, /** 66 */
        0x5B6B, /** 67 */
        0x41F4, /** 68 */
        0xFB59, /** 69 */
        0x8E03, /** 6A */
        0x34AE, /** 6B */
        0x64B7, /** 6C */
        0xDE1A, /** 6D */
        0xAB40, /** 6E */
        0x11ED, /** 6F */
        0x9E7E, /** 70 */
        0x24D3, /** 71 */
        0x5189, /** 72 */
        0xEB24, /** 73 */
        0xBB3D, /** 74 */
        0x0190, /** 75 */
        0x74CA, /** 76 */
        0xCE67, /** 77 */
        0xD4F8, /** 78 */
        0x6E55, /** 79 */
        0x1B0F, /** 7A */
        0xA1A2, /** 7B */
        0xF1BB, /** 7C */
        0x4B16, /** 7D */
        0x3E4C, /** 7E */
        0x84E1, /** 7F */
        0x8D23, /** 80 */
        0x378E, /** 81 */
        0x42D4, /** 82 */
        0xF879, /** 83 */
        0xA860, /** 84 */
        0x12CD, /** 85 */
        0x6797, /** 86 */
        0xDD3A, /** 87 */
        0xC7A5, /** 88 */
        0x7D08, /** 89 */
        0x0852, /** 8A */
        0xB2FF, /** 8B */
        0xE2E6, /** 8C */
        0x584B, /** 8D */
        0x2D11, /** 8E */
        0x97BC, /** 8F */
        0x182F, /** 90 */
        0xA282, /** 91 */
        0xD7D8, /** 92 */
        0x6D75, /** 93 */
        0x3D6C, /** 94 */
        0x87C1, /** 95 */
        0xF29B, /** 96 */
        0x4836, /** 97 */
        0x52A9, /** 98 */
        0xE804, /** 99 */
        0x9D5E, /** 9A */
        0x27F3, /** 9B */
        0x77EA, /** 9C */
        0xCD47, /** 9D */
        0xB81D, /** 9E */
        0x02B0, /** 9F */
        0x1D96, /** A0 */
        0xA73B, /** A1 */
        0xD261, /** A2 */
        0x68CC, /** A3 */
        0x38D5, /** A4 */
        0x8278, /** A5 */
        0xF722, /** A6 */
        0x4D8F, /** A7 */
        0x5710, /** A8 */
        0xEDBD, /** A9 */
        0x98E7, /** AA */
        0x224A, /** AB */
        0x7253, /** AC */
        0xC8FE, /** AD */
        0xBDA4, /** AE */
        0x0709, /** AF */
        0x889A, /** B0 */
        0x3237, /** B1 */
        0x476D, /** B2 */
        0xFDC0, /** B3 */
        0xADD9, /** B4 */
        0x1774, /** B5 */
        0x622E, /** B6 */
        0xD883, /** B7 */
        0xC21C, /** B8 */
        0x78B1, /** B9 */
        0x0DEB, /** BA */
        0xB746, /** BB */
        0xE75F, /** BC */
        0x5DF2, /** BD */
        0x28A8, /** BE */
        0x9205, /** BF */
        0x16E4, /** C0 */
        0xAC49, /** C1 */
        0xD913, /** C2 */
        0x63BE, /** C3 */
        0x33A7, /** C4 */
        0x890A, /** C5 */
        0xFC50, /** C6 */
        0x46FD, /** C7 */
        0x5C62, /** C8 */
        0xE6CF, /** C9 */
        0x9395, /** CA */
        0x2938, /** CB */
        0x7921, /** CC */
        0xC38C, /** CD */
        0xB6D6, /** CE */
        0x0C7B, /** CF */
        0x83E8, /** D0 */
        0x3945, /** D1 */
        0x4C1F, /** D2 */
        0xF6B2, /** D3 */
        0xA6AB, /** D4 */
        0x1C06, /** D5 */
        0x695C, /** D6 */
        0xD3F1, /** D7 */
        0xC96E, /** D8 */
        0x73C3, /** D9 */
        0x0699, /** DA */
        0xBC34, /** DB */
        0xEC2D, /** DC */
        0x5680, /** DD */
        0x23DA, /** DE */
        0x9977, /** DF */
        0x8651, /** E0 */
        0x3CFC, /** E1 */
        0x49A6, /** E2 */
        0xF30B, /** E3 */
        0xA312, /** E4 */
        0x19BF, /** E5 */
        0x6CE5, /** E6 */
        0xD648, /** E7 */
        0xCCD7, /** E8 */
        0x767A, /** E9 */
        0x0320, /** EA */
        0xB98D, /** EB */
        0xE994, /** EC */
        0x5339, /** ED */
        0x2663, /** EE */
        0x9CCE, /** EF */
        0x135D, /** F0 */
        0xA9F0, /** F1 */
        0xDCAA, /** F2 */
        0x6607, /** F3 */
        0x361E, /** F4 */
        0x8CB3, /** F5 */
        0xF9E9, /** F6 */
        0x4344, /** F7 */
        0x59DB, /** F8 */
        0xE376, /** F9 */
        0x962C, /** FA */
        0x2C81, /** FB */
        0x7C98, /** FC */
        0xC635, /** FD */
        0xB36F, /** FE */
        0x09C2, /** FF */
};

#endif /* CRC16_BAAD_H_ */

/**
 * \}
 * \}
 */
