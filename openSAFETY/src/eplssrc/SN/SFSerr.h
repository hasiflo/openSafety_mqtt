/**
 * @addtogroup SFS
 * @{
 *
 * @file SFSerr.h
 *
 * This file contains all errors that can occur inside the unit SFS.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 * @author K. Fahrion, IXXAT Automation GmbH
 *
 * Inside SFSerr.h the function to get an error string matching to a specific
 * error number is declared, because only the error module need to see this function declaration!
 *
 */

#ifndef SFSERR_H
#define SFSERR_H

/**
 * @def SFS_k_ERR_RXFRM_CRC8_DATALEN_INCONS
 * @brief This error means that the value in the LE field of the received EPLS sub
 * frame ONE does NOT match with the number of received bytes of the whole
 * frame. Payload data length is maximum 8 bytes.
*/
#define SFS_k_ERR_RXFRM_CRC8_DATALEN_INCONS \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 1)


/**
 * @def SFS_k_ERR_RXFRM_CRC16_DATALEN_INCONS
 * @brief This error means that the value in the LE field of the received EPLS sub
 * frame ONE does NOT match with the number of received bytes of the whole
 *  frame. Payload data length is minimum 9 bytes, maximum 254 bytes.
*/
#define SFS_k_ERR_RXFRM_CRC16_DATALEN_INCONS \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 2)


/**
 * @def SFS_k_ERR_RXFRM_DATALEN_INV
 * @brief This error means that the received value in the LE field is off valid range of 0 - 254.
*/
#define SFS_k_ERR_RXFRM_DATALEN_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 3)


/**
 * @def SFS_k_ERR_RXFRM_ID_INV
 * @brief This error means that the received openSAFETY frame contains different frame Id's in sub frame ONE and in sub frame TWO.
*/
#define SFS_k_ERR_RXFRM_ID_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 4)

/**
 * @def SFS_k_ERR_DATALEN_INV
 * @brief This error means that the received value in the LE field is off valid range of 0 - 254.
*/
#define SFS_k_ERR_DATALEN_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SFS_k_UNIT_ID, 5)


/**
 * @def SFS_k_ERR_RXDATA_INV
 * @brief This error means that the received payload data in sub frame one and sub frame two is NOT equal.
*/
#define SFS_k_ERR_RXDATA_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 6)


/**
 * @def SFS_k_ERR_SF1_RX_CRC1_INV
 * @brief This error means that the received CRC8 in sub frame ONE is NOT correct.
*/
#define SFS_k_ERR_SF1_RX_CRC1_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 7)


/**
 * @def SFS_k_ERR_SF1_RX_CRC2_INV
 * @brief This error means that the received CRC16 in sub frame ONE is NOT correct.
*/
#define SFS_k_ERR_SF1_RX_CRC2_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 8)


/**
 * @def SFS_k_ERR_SF2_RX_CRC1_INV
 * @brief This error means that the received CRC8 in sub frame TWO is NOT correct.
*/
#define SFS_k_ERR_SF2_RX_CRC1_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 9)


/**
 * @def SFS_k_ERR_SF2_RX_CRC2_INV
 * @brief This error means that the received CRC16 in sub frame TWO is NOT correct.
*/
#define SFS_k_ERR_SF2_RX_CRC2_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 10)


/**
 * @def SFS_k_ERR_DATATYPE_NOT_DEF
 * @brief This error means that the data type is unknown. Known data types are:
 * BOOLEAN, UINT8, INT8, UINT16, INT16, UINT32, INT32, UINT64, INT64.
*/
#define SFS_k_ERR_DATATYPE_NOT_DEF \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SFS_k_UNIT_ID, 11)


/**
 * @def SFS_k_ERR_RX_TELLEN_INV
 * @brief This error means that the received telegram length is invalid.
*/
#define SFS_k_ERR_RX_TELLEN_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 12)


/**
 * @def SFS_k_ERR_RX_SADR_INV
 * @brief This error means that the received SADR is NOT within valid range.
*/
#define SFS_k_ERR_RX_SADR_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 13)


/**
 * @def SFS_k_ERR_RX_SDN_INV
 * @brief This error means that the received SDN is NOT within valid range.
*/
#define SFS_k_ERR_RX_SDN_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 14)


/**
 * @def SFS_k_ERR_TX_SADR_INV
 * @brief This error means that the logical address (SADR) to be transmitted is NOT in valid range.
*/
#define SFS_k_ERR_TX_SADR_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SFS_k_UNIT_ID, 15)


/**
 * @def  SFS_k_ERR_TX_FRMID_INV
 * @brief This error means that the frame ID to be transmitted is NOT in valid range.
*/
#define SFS_k_ERR_TX_FRMID_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SFS_k_UNIT_ID, 16)


/**
 * @def SFS_k_ERR_TX_SDN_INV
 * @brief This error means that the Safety Domain Number (SDN) to be transmitted is NOT in valid range.
*/
#define SFS_k_ERR_TX_SDN_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SFS_k_UNIT_ID, 17)


/**
 * @def SFS_k_ERR_TX_LE_INV
 * @brief This error means that the payload data length (LE) to be transmitted is NOT in valid range
*/
#define SFS_k_ERR_TX_LE_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SFS_k_UNIT_ID, 18)


/**
 * @def SFS_k_ERR_TX_TADR_INV
 * @brief This error means that the  Time Request Address (TADR) to be transmitted is NOT in valid range
*/
#define SFS_k_ERR_TX_TADR_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SFS_k_UNIT_ID, 19)


/**
 * @def SFS_k_ERR_TX_TR_INV
 * @brief This error means that the  Time Request Distinctive Number (TR) is NOT in valid range
*/
#define SFS_k_ERR_TX_TR_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SFS_k_UNIT_ID, 20)


/**
 * @def SFS_k_ERR_RX_FRMLEN_EVEN
 * @brief This error means that the frame length provided by the application is an even number.
 *
 * This is not possible because of 9 bytes header info in
 * sub frame ONE and TWO an even numbers for CRC bytes and payload bytes.
*/
#define SFS_k_ERR_RX_FRMLEN_EVEN \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 21)


/**
 * @def SFS_k_ERR_NO_MEM_FROM_SHNF
 * @brief This error means that the SHNF could NOT deliver a valid reference to
 * the requested memory block
*/
#define SFS_k_ERR_NO_MEM_FROM_SHNF \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SFS_k_UNIT_ID, 22)


/**
 * @def SFS_k_ERR_SHNF_CANNOT_MARK_MEM_BLK
 * @brief This error means that the SHNF could NOT deliver a valid reference to
 * the requested memory block
*/
#define SFS_k_ERR_SHNF_CANNOT_MARK_MEM_BLK \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SFS_k_UNIT_ID, 23)


/**
 * @def SFS_k_ERR_RX_TADR_INV
 * @brief This error means that the received TADR is NOT within valid range.
 */
#define SFS_k_ERR_RX_TADR_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SFS_k_UNIT_ID, 24)


/**
 * @def SFS_k_ERR_BR_CLBK
 * @brief This error means that a before read callback is registered for the UDID
 * of the SCM SOD object with index 0x1200 and sub-index 0x04.
*/
#define SFS_k_ERR_BR_CLBK \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SFS_k_UNIT_ID, 25)

#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param        w_errorCode        error number
*
* @param        dw_addInfo         additional error information
*
* @param        pac_str            empty buffer to build the error string
*
*/
void SFS_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str);
#endif

#endif

/** @} */
