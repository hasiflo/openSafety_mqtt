/**
 * \file crc.h
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

#ifndef _LIBOSCHECKSUM_CRCINT_H_
#define _LIBOSCHECKSUM_CRCINT_H_


/**
 * \brief Calculates a CRC8 checksum for the given buffer
 *
 * \param ui_sub_length is an integer frame length
 * \param ui_poly the integer polynomial which will be in use
 * \param uc_subframe pointer to start address of frame array
 *
 * \return  unsinged char checksum
 */
OSCHECKSUM_EXPORT uint8_t uc_openSAFETY_CRC8(uint8_t uc_sub_length, uint8_t uc_poly, uint8_t * uc_subframe);

/**
 * \brief Calculates a CRC16 checksum for the given buffer
 *
 * \param ui_sub_length is an integer frame length
 * \param ui_poly the integer polynomial which will be in use
 * \param uc_subframe pointer to start address of frame array
 *
 * \return  unsigned int checksum
 */
OSCHECKSUM_EXPORT uint16_t ui_openSAFETY_CRC16(uint16_t ui_sub_length, uint16_t ui_poly, uint8_t *uc_subframe);

OSCHECKSUM_EXPORT uint16_t crc16ChecksumPoly(uint32_t len, const uint8_t * pBuffer,
                uint16_t initCRC, uint16_t Polynom);

#endif /* _LIBOSCHECKSUM_CRCINT_H_ */

/**
 * \}
 * \}
 */
