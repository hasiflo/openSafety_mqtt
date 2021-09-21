/**
 * @addtogroup SN
 * @{
 * @addtogroup SSDO [SSDO] Safety Service Data Object Server and Client
 * @{
 *
 * SSDOS, SSDOC network managment unit of the openSAFETY stack.
 *
 * @file SSDOint.h
 *
 * This file is the internal header of SSDOC and SSDOS in common.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SSDOint.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>11.09.2014</td><td>Hans Pill</td><td>added constants for preload</td></tr>
 * </table>
 *
 * The Byte offsets values inside the data area of a service data object are used to address different
 * byte positions inside the data area of a service data object:
 *
 * <table>
 *     <tr>
 *        <th colspan="3">Data Header</th>
 *        <th>Payload Data</th>
 *     </tr>
 *     <tr>
 *         <td>DB0: SACmd</td>
 *         <td>DB1-2: Index</td>
 *         <td>DB3: Sub-Index</td>
 *         <td>DB4-DB253: Payload</td>
 *     </tr>
 * </table>
 *
 */

#ifndef SSDOINT_H
#define SSDOINT_H

/**
 * @name Byte offsets inside data area of a service data object
 * @{
 */
/** SOD Access Command */
#define k_OFS_SACMD            0x00u
/** SOD index */
#define k_OFS_IDX              0x01u
/** SOD sub-index */
#define k_OFS_SIDX             0x03u

/**
 * This symbol represents the payload data offset value of the whole SOD object length. (DB4-DB7)
 */
#define k_OFS_OBJ_LEN          0x04u
/** @} */

/**
 * @name Offset for data payload
 * @{
 *
 * These offset values are used to address the payload inside the data area of
 * the SSDO service (req/resp). Depending on the SSDO service type (segmented
 * or expedited) the payload starts at different offsets.
 */
/** SSDO Abort Code */
#define k_OFS_ABORT_CODE       0x04u
/** Initiate Up/Download, expedited */
#define k_OFS_PYLD_INIT_EXP    0x04u
/** Initiate Up/Download, segmented */
#define k_OFS_PYLD_INIT_SEG    0x08u

/** Middle End Segmented Up/Download */
#define k_OFS_PYLD_SEG         0x01u

/**
 * This symbol represents the number of bytes used for SOD access req command, index and sub index.
 *
 * Payload data begins after data header. The define represents the number of bytes used for SACmd,
 * index and sub-index
 */
#define k_DATA_HDR_LEN                 0x04u
/**
 * This symbol represents the number of bytes used for SOD access req command, index and sub index.
 *
 * Payload data begins after data header. The define represents the number of bytes used for SACmd
 */
#define k_DATA_HDR_LEN_SEG             0x01u

/**
 * This symbol represents the number of bytes used to transfer the 32 bit value of the object length.
 *
 * The object length is transferred after data header inside the service req SSDO Initiate Download,
 * segmented and inside the service resp SSDO Initiate Upload, segmented.
 *
 * The define represents the number of bytes used for object length 32 bit type
 */
#define k_DATA_OBJ_LEN                 0x04u
/** @} */

/**
 * This symbol represents the length of the abort code.
 */
#define k_ABORT_LEN               0x04u

/**
 * This symbol is used to access the toggle bit inside the SOD access command (SaCmd).
 */
#define k_TOGGLE_SACMD           (UINT8)0x10

/**
 * @name SOD Access Commands
 * @{
 *
 * SOD Access Commands used in SSDO Services (request/response).
 *
 * @attention The toggle bit of the response SOD Access Command is reset to be able to
 * compare it with the SOD Access Command of the request.
 */
/** init expedited download transfer */
#define k_DWNLD_INIT_EXP     0x21U
/** init segmented download transfer */
#define k_DWNLD_INIT_SEG     0x29U
/** download transfer of middle segment */
#define k_DWNLD_SEG_MID      0x09U
/** download transfer of last segment */
#define k_DWNLD_SEG_END      0x49U
/** init segmented preload download transfer */
#define k_DWNLD_SEG_PRE_INIT 0x2BU
/** download transfer of preload middle segment */
#define k_DWNLD_SEG_PRE_MID  0x0BU
/** init expedited upload transfer */
#define k_UPLD_INIT_EXP      0x20U
/** init segmented upload transfer */
#define k_UPLD_INIT_SEG      0x28U
/** upload transfer of middle segment */
#define k_UPLD_SEG_MID       0x08U
/** upload transfer of end segment */
#define k_UPLD_SEG_END       0x48U

/**
 * This symbol is used to access the preload bit inside the SOD access command (SaCmd).
*/
#define k_PRE_SACMD    (UINT8)0x02

/** transfer abort service */
#define k_ABORT          0x04U
/** @} */
/** This symbol is used to signalize a preload error while SSDO transfer (TR field) */
#define k_ERR_PREL_TR     0x30U
/** This symbol is used to mask out the queue length (TR field) */
#define k_QLEN_PREL_TR    0x0FU
#define k_QLEN_PREL_MIN   0x01U
#define k_QLEN_PREL_MAX   0x0FU
/**
 * This symbol represents the object size is unknown during the upload.
 */
#define k_OBJ_SIZE_UNKNOWN        0x00UL

#endif

/** @} */
/** @} */
/** @} */
