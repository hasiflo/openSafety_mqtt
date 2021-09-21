/**
 * \file crc16_755B.h
 * Precompiled CRC16 checksum table for polynom 0x755B
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

#ifndef CRC16_755B_H_
#define CRC16_755B_H_

#include <stdint.h>

/**
 * \brief Precompiled table for CRC16 values
 * \see CRC16_POLY_755B
 * \see crc16Checksum_755B
 */
static const uint16_t PrecompiledCRC16_755B[] =
{
        0x0000, /** 00 */
        0x755B, /** 01 */
        0xEAB6, /** 02 */
        0x9FED, /** 03 */
        0xA037, /** 04 */
        0xD56C, /** 05 */
        0x4A81, /** 06 */
        0x3FDA, /** 07 */
        0x3535, /** 08 */
        0x406E, /** 09 */
        0xDF83, /** 0A */
        0xAAD8, /** 0B */
        0x9502, /** 0C */
        0xE059, /** 0D */
        0x7FB4, /** 0E */
        0x0AEF, /** 0F */
        0x6A6A, /** 10 */
        0x1F31, /** 11 */
        0x80DC, /** 12 */
        0xF587, /** 13 */
        0xCA5D, /** 14 */
        0xBF06, /** 15 */
        0x20EB, /** 16 */
        0x55B0, /** 17 */
        0x5F5F, /** 18 */
        0x2A04, /** 19 */
        0xB5E9, /** 1A */
        0xC0B2, /** 1B */
        0xFF68, /** 1C */
        0x8A33, /** 1D */
        0x15DE, /** 1E */
        0x6085, /** 1F */
        0xD4D4, /** 20 */
        0xA18F, /** 21 */
        0x3E62, /** 22 */
        0x4B39, /** 23 */
        0x74E3, /** 24 */
        0x01B8, /** 25 */
        0x9E55, /** 26 */
        0xEB0E, /** 27 */
        0xE1E1, /** 28 */
        0x94BA, /** 29 */
        0x0B57, /** 2A */
        0x7E0C, /** 2B */
        0x41D6, /** 2C */
        0x348D, /** 2D */
        0xAB60, /** 2E */
        0xDE3B, /** 2F */
        0xBEBE, /** 30 */
        0xCBE5, /** 31 */
        0x5408, /** 32 */
        0x2153, /** 33 */
        0x1E89, /** 34 */
        0x6BD2, /** 35 */
        0xF43F, /** 36 */
        0x8164, /** 37 */
        0x8B8B, /** 38 */
        0xFED0, /** 39 */
        0x613D, /** 3A */
        0x1466, /** 3B */
        0x2BBC, /** 3C */
        0x5EE7, /** 3D */
        0xC10A, /** 3E */
        0xB451, /** 3F */
        0xDCF3, /** 40 */
        0xA9A8, /** 41 */
        0x3645, /** 42 */
        0x431E, /** 43 */
        0x7CC4, /** 44 */
        0x099F, /** 45 */
        0x9672, /** 46 */
        0xE329, /** 47 */
        0xE9C6, /** 48 */
        0x9C9D, /** 49 */
        0x0370, /** 4A */
        0x762B, /** 4B */
        0x49F1, /** 4C */
        0x3CAA, /** 4D */
        0xA347, /** 4E */
        0xD61C, /** 4F */
        0xB699, /** 50 */
        0xC3C2, /** 51 */
        0x5C2F, /** 52 */
        0x2974, /** 53 */
        0x16AE, /** 54 */
        0x63F5, /** 55 */
        0xFC18, /** 56 */
        0x8943, /** 57 */
        0x83AC, /** 58 */
        0xF6F7, /** 59 */
        0x691A, /** 5A */
        0x1C41, /** 5B */
        0x239B, /** 5C */
        0x56C0, /** 5D */
        0xC92D, /** 5E */
        0xBC76, /** 5F */
        0x0827, /** 60 */
        0x7D7C, /** 61 */
        0xE291, /** 62 */
        0x97CA, /** 63 */
        0xA810, /** 64 */
        0xDD4B, /** 65 */
        0x42A6, /** 66 */
        0x37FD, /** 67 */
        0x3D12, /** 68 */
        0x4849, /** 69 */
        0xD7A4, /** 6A */
        0xA2FF, /** 6B */
        0x9D25, /** 6C */
        0xE87E, /** 6D */
        0x7793, /** 6E */
        0x02C8, /** 6F */
        0x624D, /** 70 */
        0x1716, /** 71 */
        0x88FB, /** 72 */
        0xFDA0, /** 73 */
        0xC27A, /** 74 */
        0xB721, /** 75 */
        0x28CC, /** 76 */
        0x5D97, /** 77 */
        0x5778, /** 78 */
        0x2223, /** 79 */
        0xBDCE, /** 7A */
        0xC895, /** 7B */
        0xF74F, /** 7C */
        0x8214, /** 7D */
        0x1DF9, /** 7E */
        0x68A2, /** 7F */
        0xCCBD, /** 80 */
        0xB9E6, /** 81 */
        0x260B, /** 82 */
        0x5350, /** 83 */
        0x6C8A, /** 84 */
        0x19D1, /** 85 */
        0x863C, /** 86 */
        0xF367, /** 87 */
        0xF988, /** 88 */
        0x8CD3, /** 89 */
        0x133E, /** 8A */
        0x6665, /** 8B */
        0x59BF, /** 8C */
        0x2CE4, /** 8D */
        0xB309, /** 8E */
        0xC652, /** 8F */
        0xA6D7, /** 90 */
        0xD38C, /** 91 */
        0x4C61, /** 92 */
        0x393A, /** 93 */
        0x06E0, /** 94 */
        0x73BB, /** 95 */
        0xEC56, /** 96 */
        0x990D, /** 97 */
        0x93E2, /** 98 */
        0xE6B9, /** 99 */
        0x7954, /** 9A */
        0x0C0F, /** 9B */
        0x33D5, /** 9C */
        0x468E, /** 9D */
        0xD963, /** 9E */
        0xAC38, /** 9F */
        0x1869, /** A0 */
        0x6D32, /** A1 */
        0xF2DF, /** A2 */
        0x8784, /** A3 */
        0xB85E, /** A4 */
        0xCD05, /** A5 */
        0x52E8, /** A6 */
        0x27B3, /** A7 */
        0x2D5C, /** A8 */
        0x5807, /** A9 */
        0xC7EA, /** AA */
        0xB2B1, /** AB */
        0x8D6B, /** AC */
        0xF830, /** AD */
        0x67DD, /** AE */
        0x1286, /** AF */
        0x7203, /** B0 */
        0x0758, /** B1 */
        0x98B5, /** B2 */
        0xEDEE, /** B3 */
        0xD234, /** B4 */
        0xA76F, /** B5 */
        0x3882, /** B6 */
        0x4DD9, /** B7 */
        0x4736, /** B8 */
        0x326D, /** B9 */
        0xAD80, /** BA */
        0xD8DB, /** BB */
        0xE701, /** BC */
        0x925A, /** BD */
        0x0DB7, /** BE */
        0x78EC, /** BF */
        0x104E, /** C0 */
        0x6515, /** C1 */
        0xFAF8, /** C2 */
        0x8FA3, /** C3 */
        0xB079, /** C4 */
        0xC522, /** C5 */
        0x5ACF, /** C6 */
        0x2F94, /** C7 */
        0x257B, /** C8 */
        0x5020, /** C9 */
        0xCFCD, /** CA */
        0xBA96, /** CB */
        0x854C, /** CC */
        0xF017, /** CD */
        0x6FFA, /** CE */
        0x1AA1, /** CF */
        0x7A24, /** D0 */
        0x0F7F, /** D1 */
        0x9092, /** D2 */
        0xE5C9, /** D3 */
        0xDA13, /** D4 */
        0xAF48, /** D5 */
        0x30A5, /** D6 */
        0x45FE, /** D7 */
        0x4F11, /** D8 */
        0x3A4A, /** D9 */
        0xA5A7, /** DA */
        0xD0FC, /** DB */
        0xEF26, /** DC */
        0x9A7D, /** DD */
        0x0590, /** DE */
        0x70CB, /** DF */
        0xC49A, /** E0 */
        0xB1C1, /** E1 */
        0x2E2C, /** E2 */
        0x5B77, /** E3 */
        0x64AD, /** E4 */
        0x11F6, /** E5 */
        0x8E1B, /** E6 */
        0xFB40, /** E7 */
        0xF1AF, /** E8 */
        0x84F4, /** E9 */
        0x1B19, /** EA */
        0x6E42, /** EB */
        0x5198, /** EC */
        0x24C3, /** ED */
        0xBB2E, /** EE */
        0xCE75, /** EF */
        0xAEF0, /** F0 */
        0xDBAB, /** F1 */
        0x4446, /** F2 */
        0x311D, /** F3 */
        0x0EC7, /** F4 */
        0x7B9C, /** F5 */
        0xE471, /** F6 */
        0x912A, /** F7 */
        0x9BC5, /** F8 */
        0xEE9E, /** F9 */
        0x7173, /** FA */
        0x0428, /** FB */
        0x3BF2, /** FC */
        0x4EA9, /** FD */
        0xD144, /** FE */
        0xA41F, /** FF */
};

#endif /* CRC16_755B_H_ */

/**
 * \}
 * \}
 */
