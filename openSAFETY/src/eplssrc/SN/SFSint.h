/**
 * @addtogroup SFS
 * @{
 *
 * @file SFSint.h
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 * @author K. Fahrion, IXXAT Automation GmbH
 *
 */


#ifndef SFSINT_H
#define SFSINT_H


/**
 * @name Sub Frame ONE header info access
 * @{
 * @brief These symbols are used to access the header information of sub frame one of a received openSAFETY frame.
 *
 * The symbols represent ABSOLUTE byte indices/offsets inside the byte array of a received openSAFETY frame.
 *
 * @note All further possible indices inside the openSAFETY frame vary, because of
 * variable data length and 1 byte OR 2 byte CRC. As a result these further indices have to be calculated.
*/

/** index to 10 bit ADR field of sub frame one (bit 8,9 in ID encoded) */
#define k_SFRM1_HDR_ADR    0u
/** index to 6 bit ID field of sub frame one (bit 0,1 are encoded as bit 8,9 of ADR */
#define k_SFRM1_HDR_ID     1u
/** index to 8 bit frame length field of sub frame ONE */
#define k_SFRM1_HDR_LE     2u
/** index to 8 bit consecutive time field of sub frame one */
#define k_SFRM1_HDR_CT     3u
/** index to first byte of payload data of sub frame ONE */
#define k_SFRM1_DATA       4u
/** @} */

/**
 * @name Sub Frame TWO header info access
 * @{
 * @brief These symbols are used in calculation of indices for sub frame TWO of a received openSAFETY frame.
 *
 * The symbols represents RELATIVE byte index/ offsets which are used in calculation of an absolute byte indice for sub frame TWO.
*/
/** offset to 10 bit ADR field of sub frame TWO (bit 8,9 in ID encoded) */
#define k_SFRM2_HDR_ADR    0u
/** offset to 6 bit ID field of sub frame TWO (bit 0,1 are encoded as bit 8,9 of ADR */
#define k_SFRM2_HDR_ID     1u
/** offset to 8 bit consecutive time field inside sub frame TWO */
#define k_SFRM2_HDR_CT     2u
/** offset to 10 bit TADR field of sub frame TWO (bit 8,9 in TR encoded) */
#define k_SFRM2_HDR_TADR   3u
/** offset to 6 bit TR field of sub frame TWO (bit 0,1 are encoded as bit 8,9 of TADR */
#define k_SFRM2_HDR_TR     4u
/** @} */

/**
 * @def k_SFRM2_DATA
 * @brief Index to first byte of payload data field in sub frame two. This symbol is
 * also globally known as SFS_k_FRM_DATA_OFFSET and thus in SFS.h defined.
 */
#define k_SFRM2_DATA  SFS_k_FRM_DATA_OFFSET


/**
 * @name Frame header length
 * @{
 * These symbols represent the frame header length of sub frame one and sub
 * frame two of an openSAFETY frame.
*/
/** header length of sub frame ONE */
#define k_SFRM1_HDR_LEN       (UINT16)0x04
/** header length of sub frame TWO */
#define k_SFRM2_HDR_LEN       (UINT16)0x05
/** @} */

/**
 * @name CRC length of openSAFETY frames
 * @{
 * These symbols represent the number of bytes used for CRC. Short openSAFETY frames
 * (0..8 bytes payload data) use 1 byte CRC and long openSAFETY frames (9..254 bytes)
 * use 2 bytes CRC.
*/
/** 1 byte CRC for short openSAFETY frames */
#define k_CRC_LEN_SHORT   (UINT16)0x01
/** 2 byte CRC for long openSAFETY frames */
#define k_CRC_LEN_LONG    (UINT16)0x02
/** @} */

/**
 * @name Frame payload data length limits
 * @{
 * These symbols represent limits of the payload data length of a short openSAFETY frame
 * and a long openSAFETY frame. The distinction is made because of the use of
 * 1 byte CRC for short frames and 2 byte CRC for long frames.
*/
/** maximum number of payload data bytes */
#define k_MAX_DATA_LEN         0xFEu
/** minimum number of payload data bytes */
#define k_MIN_DATA_LEN         0x00u
/** max. payload data length of 8 bytes for short frames */
#define k_MAX_DATA_LEN_SHORT   0x08u
/** min. payload data length of 9 bytes for long frames */
#define k_MIN_DATA_LEN_LONG    0x09u
/** max. payload data length of 254 bytes for long frames */
#define k_MAX_DATA_LEN_LONG    0xFEu
/** @} */


/**
 * @name Frame length limits
 * @{
 * These limits represents limits of the openSAFETY frame. The limits are result of
 * some calculations. The gap between 27 and 31 is in consequence of the
 *  calculations.
*/
#define k_MIN_TEL_LEN_SHORT     (UINT16)(k_SFRM1_HDR_LEN + k_SFRM2_HDR_LEN +  \
                                k_CRC_LEN_SHORT + k_CRC_LEN_SHORT)            \

#define k_MAX_TEL_LEN_SHORT     (UINT16)(k_SFRM1_HDR_LEN + k_SFRM2_HDR_LEN +  \
                                k_MAX_DATA_LEN_SHORT + k_MAX_DATA_LEN_SHORT + \
                                k_CRC_LEN_SHORT + k_CRC_LEN_SHORT)            \

#define k_MIN_TEL_LEN_LONG      (UINT16)(k_SFRM1_HDR_LEN + k_SFRM2_HDR_LEN +  \
                                k_MIN_DATA_LEN_LONG + k_MIN_DATA_LEN_LONG +   \
                                k_CRC_LEN_LONG + k_CRC_LEN_LONG)              \

#define k_MAX_TEL_LEN_LONG      (UINT16)(k_SFRM1_HDR_LEN + k_SFRM2_HDR_LEN +  \
                                k_MAX_DATA_LEN_LONG + k_MAX_DATA_LEN_LONG +   \
                                k_CRC_LEN_LONG + k_CRC_LEN_LONG)              \
/** @} */


/**
 * @name Fixed number of bytes of an openSAFETY frame
 * @{
 * These symbols include the fixed number of bytes within an openSAFETY frame using
 * 1 byte CRC and 2 bytes CRC.
*/
#define k_FRM_LEN_FIX_SHORT  (UINT16)(k_MIN_TEL_LEN_SHORT)
#define k_FRM_LEN_FIX_LONG   (UINT16)(0x0D)
/** @} */


/**
 * @name Addressing parameter limits
 * @{
 * These symbols represents limits of parameter inside the frame header that
 *  specifies any address (source or destination)
*/
/** minimum value is 1 */
#define k_MIN_SDN            0x0001u
/** maximum value is 1023 */
#define k_MAX_SDN            0x03FFu
/** maximum value is 1023 */
#define k_MAX_TADR           0x03FFu
/** @} */

/**
 * @def k_ADR_MASK_AND
 * @brief This bit pattern is used to clear bit 2, 3, 4, 5, 6, 7 in the ID field of a received openSAFETY sub frame one.
 *
 * In bit 0, 1 is a part of the logical address (SADR) of the openSAFETY frame encoded. This mask is used in
 * conjunction with a logical AND operation.
*/
#define k_ADR_MASK_AND        (UINT16)0x03FFu  /* 0000|0011|1111|1111 */


/**
 * @def k_SDN_HB_MASK_AND
 * @brief This bit pattern is used to clear bit 2, 3, 4, 5, 6, 7 of the high byte of the Safety Domain Number.
 *
 * Only bit 0, 1 is a part of the Safety Domain Number. This mask is used in conjunction with a logical AND operation.
*/
#define k_SDN_HB_MASK_AND 0x03u /* 0000|0011 */


/**
 * @def k_TADR_MASK_AND
 * @brief This bit pattern is used to clear bit 2, 3, 4, 5, 6, 7 in the TR field of a received openSAFETY sub frame TWO.
 *
 * In bit 0, 1 is a part of the Time Request Address encoded (TADR). This mask is used in conjunction with a
 * logical AND operation.
*/
#define k_TADR_MASK_AND  0x03u  /* 0000|0011 */


/**
 * @def k_TR_MASK_AND
 * @brief This bit pattern is used to clear bit 0, 1 in the TR field of a received openSAFETY sub frame TWO.
 *
 * In bit 2, 3, 4, 5, 6, 7 is the Time Request Address of the received openSAFETY frame encoded.
 * This mask is used in conjunction with a logical AND operation.
*/
#define k_TR_MASK_AND   0xFCu  /* 1111|1100 */


/**
 * @def k_NUM_OF_POSSIBLE_IDS
 *
 * @brief This symbol represents the maximum number of combinations of openSAFETY frame IDs in case of 6 bit length.
*/
#define k_NUM_OF_POSSIBLE_IDS  0x40u  /* 64 different combination at all */

/**
 * @def k_SNMT_FRAME_ID
 * @brief This symbol represents the SNMT Frame identifier.
*/
#define k_SNMT_FRAME_ID  0x28U

/**
 * @def k_FRAME_ID_MASK
 * @brief This symbol is used to mask the Frame identifier.
*/
#define k_FRAME_ID_MASK  0x38U

/**
* @brief This function encodes or decodes the data of the sub frame TWO of SPDO and SSDO (not SNMT) frames with the UDID of the SCM using a logical XOR operation.
*
* @param        b_instNum            instance number (not checked, called with EPLS_k_SCM_INST_NUM or checked in SSC_ProcessSNMTSSDOFrame() or SPDO_BuildTxSpdo())) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        b_frmId              frame identifier of the openSAFETY frame (not checked, checked in checkTxFrameHeader()), valid range : UINT8
*
* @param          pb_subFrmTwo         pointer to the openSAFETY sub frame TWO (not checked, only called with reference to array in SFS_FrmSerialize() and SFS_FrmDeSerialize()), valid range: <> NULL
*
*/
void SFS_ScmUdidCode(BYTE_B_INSTNUM_ UINT8 b_frmId, UINT8 *pb_subFrmTwo);


#endif

/** @} */
