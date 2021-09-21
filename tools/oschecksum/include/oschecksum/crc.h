/**
 * \addtogroup Tools
 * \{
 * \defgroup oschecksum openSAFETY CRC routines
 * \{
 *
 * \brief CRC8, CRC16 and CRC32 example implementations for use in openSAFETY devices
 *
 * openSAFETY uses various routines for calculating CRC checksums during configuration,
 * parameterization as well as SPDO data transfer. Those routines are documented
 * in the specification for openSAFETY. To ease development, they are also provided as
 * example implementations.
 *
 * \attention It is strongly recommended, that those routines are adapted to the targets
 * environment. Specifically all routines used during cyclic data exchange, as they
 * should be handled in a time-sensitive manner.
 *
 * \see SNMT
 * \see SSDO
 * \see SPDO
 *
 * \see SHNF_Crc8Calc
 * \see SHNF_Crc16Calc
 *
 * \file crc.h openSAFETY CRC routines for implementing all necessary CRC checks
 *
 * \copyright Copyright (c) 20011,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * \author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \author Thomas Mair, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \author Patrick Harner, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 */

#ifndef _LIBOSCHECKSUM_CRC_H_
#define _LIBOSCHECKSUM_CRC_H_

/*polynomial defines*/
/** \brief CRC-8 Generator polynomial */
#define CRC8_POLY 0x2F
/**
 * \brief CRC-16 Generator polynomial 0xBAAD
 * \version Implemented sind version V1.4
 */
#define CRC16_POLY_BAAD 0xBAAD
/** \brief CRC-16 Generator polynomial (0x755B) rotate left 0xBAAD */
#define CRC16_POLY_755B 0x755B
/** \brief Old CRC-16 generator polynomial */
#define CRC16_POLY_AC9A 0xAC9A
/** \brief Old CRC-16 generator polynomial (0x5935) rotate left 0xAC9A */
#define CRC16_POLY_5935 0x5935
/** \brief CRC-32 Generator polynomial */
#define CRC32_POLY 0x1EDC6F41

/** \brief CRC32 PNG generator polynomial
 *
 * See links for more information in regard to implementation:
 * * \url http://www.dattalo.com/technical/software/pic/crc_32.asm
 * * \url http://wiki.osdev.org/CRC32
 *
 * The algorithms is derived from
 * \url http://people.sc.fsu.edu/~jburkardt/cpp_src/crc/crc.cpp
 * as well as the zlib library implementation of the crc_png algorithm
 */
#define CRC32PNG_POLY 0xEDB88320

#include "crc8.h"
#include "crc16_BAAD.h"
#include "crc16_AC9A.h"
#include "crc16_755B.h"
#include "crc16_5935.h"
#include "crc32.h"

/**
 * \brief  Defines the export definition for the shared library
 *
 * This definition depends on the target the system builds on. On windows
 * this header file can be used as well for the creation of the DLL as for
 * the import, as it defines __declspec accordingly.
 */
#if defined (_WIN32)
  #if defined(oschecksum_EXPORTS)
    #define  OSCHECKSUM_EXPORT extern __declspec(dllexport)
  #else
    #define  OSCHECKSUM_EXPORT extern __declspec(dllimport)
  #endif /* oschecksum_EXPORTS */
#else /* defined (_WIN32) */
 #define OSCHECKSUM_EXPORT extern
#endif

#pragma pack(push, 4)

#ifdef __cplusplus
extern "C" {
#endif


#include "crcint.h"

/**
 * \brief Calculates a CRC32 checksum for the given buffer using the precompiled table
 *
 * \param len the length of the given buffer
 * \param pBuffer a pointer to a buffer of the given length
 * \param initCRC Initial value for the CRC calculation. In case of openSAFETY, this is always 0
 *
 * \return the CRC32 checksum for the buffer
 */
OSCHECKSUM_EXPORT uint32_t crc32Checksum(uint32_t len, const uint8_t * pBuffer, uint32_t initCRC);

/**
 * \brief Calculates a CRC32 checksum with the PNG crc checksum for the given buffer using the precompiled table
 *
 * \param len the length of the given buffer
 * \param pBuffer a pointer to a buffer of the given length
 * \param initCRC Initial value for the CRC calculation. In case of openSAFETY, this is always 0
 *
 * \return the CRC32 checksum for the buffer
 */
OSCHECKSUM_EXPORT uint32_t crc32ChecksumPNG(uint32_t len, const uint8_t * pBuffer, uint32_t initCRC);

/**
 * \brief Calculates a CRC32 checksum with the PNG crc checksum for the given buffer using the precompiled table
 *
 * \param len the length of the given buffer
 * \param pBuffer a pointer to a buffer of the given length
 * \param initCRC Initial value for the CRC calculation. In case of openSAFETY, this is always 0
 *
 * \return the CRC32 checksum for the buffer
 */
OSCHECKSUM_EXPORT uint32_t crc32ChecksumPNG_Calculator(uint32_t len, const uint8_t * pBuffer, uint32_t initCRC);

/**
 * \brief Calculates a CRC32 checksum for the given buffer
 *
 * \param len the length of the given buffer
 * \param pBuffer a pointer to a buffer of the given length
 * \param initCRC Initial value for the CRC calculation. In case of openSAFETY, this is always 0
 *
 * \return the CRC32 checksum for the buffer
 */
OSCHECKSUM_EXPORT uint32_t crc32ChecksumCalculator(uint32_t len, const uint8_t * pBuffer, uint32_t initCRC);

/**
 * \brief Calculates a CRC16 checksum for the given buffer using the precompiled table
 *
 * \param len the length of the given buffer
 * \param pBuffer a pointer to a buffer of the given length
 * \param initCRC Initial value for the CRC calculation. In case of openSAFETY, this is always 0
 *
 * \return the CRC16 checksum for the buffer
 */
OSCHECKSUM_EXPORT uint16_t crc16Checksum(uint32_t len, const uint8_t * pBuffer, uint16_t initCRC);

/**
 * \brief Calculates a CRC16 checksum for the given buffer using the precompiled table
 *
 * \param len the length of the given buffer
 * \param pBuffer a pointer to a buffer of the given length
 * \param initCRC Initial value for the CRC calculation. In case of openSAFETY, this is always 0
 *
 * \return the CRC16 checksum for the buffer
 */
OSCHECKSUM_EXPORT uint16_t crc16Checksum_AC9A(uint32_t len, const uint8_t * pBuffer, uint16_t initCRC);

/**
 * \brief Calculates a CRC16 checksum for the given buffer
 *
 * \param len the length of the given buffer
 * \param pBuffer a pointer to a buffer of the given length
 * \param initCRC Initial value for the CRC calculation. In case of openSAFETY, this is always 0
 *
 * \return the CRC16 checksum for the buffer
 */
OSCHECKSUM_EXPORT uint16_t crc16ChecksumCalculator(uint32_t len, const uint8_t * pBuffer, uint16_t initCRC);

/**
 * \brief Calculates a CRC16 checksum for the given buffer
 *
 * \param len the length of the given buffer
 * \param pBuffer a pointer to a buffer of the given length
 * \param initCRC Initial value for the CRC calculation. In case of openSAFETY, this is always 0
 *
 * \return the CRC16 checksum for the buffer
 */
OSCHECKSUM_EXPORT uint16_t crc16ChecksumCalculator_AC9A(uint32_t len, const uint8_t * pBuffer, uint16_t initCRC);

/**
 * \brief CRC32 Algorithm sanity check
 *
 * \return although being an uint8, the return value is 0 for incorrect functionality, 1 for correct one
 */
OSCHECKSUM_EXPORT uint16_t crc16SanityCheck(void);

/**
 * \brief Calculates a CRC8 checksum for the given buffer using the precompiled table
 *
 * \param len the length of the given buffer
 * \param pBuffer a pointer to a buffer of the given length
 * \param initCRC Initial value for the CRC calculation. In case of openSAFETY, this is always 0
 *
 * \return the CRC8 checksum for the buffer
 */
OSCHECKSUM_EXPORT uint8_t crc8Checksum(uint32_t len, const uint8_t * pBuffer, uint8_t initCRC);

/**
 * \brief Calculates a CRC8 checksum for the given buffer
 *
 * \param len the length of the given buffer
 * \param pBuffer a pointer to a buffer of the given length
 * \param initCRC Initial value for the CRC calculation. In case of openSAFETY, this is always 0
 *
 * \return the CRC8 checksum for the buffer
 */
OSCHECKSUM_EXPORT uint8_t crc8ChecksumCalculator(uint32_t len, const uint8_t * pBuffer, uint8_t initCRC);

/**
 * \brief CRC8 Algorithm sanity check
 *
 * \return although being an uint8, the return value is 0 for incorrect functionality, 1 for correct one
 */
OSCHECKSUM_EXPORT uint8_t crc8SanityCheck(void);
#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif /* _LIBOSCHECKSUM_CRC_H_ */

/**
 * \}
 * \}
 */
