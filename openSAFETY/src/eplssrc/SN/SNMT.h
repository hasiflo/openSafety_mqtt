/**
 * @addtogroup SN
 * @{
 * @addtogroup SNMT [SNMT] Safety Network Management Master and Slave
 * @{
 *
 * The SNMT provides the network managment functionality for the openSAFETY stack
 *
 * @file SNMT.h
 *
 * This header file defines the basic features of the SNMT system.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 * @author K. Fahrion, IXXAT Automation GmbH
 *
 *
 * <h2>History for SNMT.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>new SNMT command for SPDOs with 40 bit CT</td></tr>
 * </table>
 */

#ifndef SNMT_H
#define SNMT_H

/**
 * @def SNMT_k_FRM_TYPE
 * @brief This symbol is used to build openSAFETY frame ID for frame header info.
 *
 * Thus the value is shifted two bits left in comparison with the serialized ID.
 * It encodes the frame type of frames to be transmitted by the SNMT Slave.
 */
#define SNMT_k_FRM_TYPE                0x28u

/**
 * @name SNMT frame type
 * @{
 * @brief These symbols represent SNMT frame types to distinguish the SNMT Service Requests and Responses.
 */
/** UDID request */
#define SNMT_k_UDID_REQ     (UINT8)(0x00)
/** Assign SADR request */
#define SNMT_k_ASS_SADR     (UINT8)(0x02)
/** Extended service request */
#define SNMT_k_EXT_SER_REQ  (UINT8)(0x04)
/** reset guarding */
#define SNMT_k_RST_GUARD    (UINT8)(0x07)
/** @} */

/**
 * @name Data lengths of the SNMT service requests/reponses
 * @{
 * @brief These symbols represent data lengths in bytes inside the data area of a
 * SNMTS service request / SNMTS service response.
 */
/** extended SNMT service command length */
#define SNMT_k_LEN_SERV_CMD    0x01u
/** time stamp lenght in the SNMT "Set to op" extended request service */
#define SNMT_k_LEN_TSTMP       0x04u
/** source address length in the SNMT "Additional SADR assignment" extended request/response service */
#define SNMT_k_LEN_SADR        0x02u
/** Tx SPDO number length  in the SNMT "Additional SADR assignment" extended request/response service */
#define SNMT_k_LEN_TXSPDO      0x02u
/** Error group length in the SNMT "SN Fail" extended response service */
#define SNMT_k_LEN_ERR_GROUP   0x01u
/** Error code length in the SNMT "SN Fail" extended response service */
#define SNMT_k_LEN_ERR_CODE    0x01u
/** @} */

/**
 * @name Data offset addresses of service requests/response
 * @{
 * @brief These symbols represent offset addresses inside the data area of a SNMTS service request / SNMTS service response.
 */
#define SNMT_k_OFS_SERV_CMD     0x00u
#define SNMT_k_OFS_SADR         (SNMT_k_OFS_SERV_CMD + SNMT_k_LEN_SERV_CMD)
#define SNMT_k_OFS_TXSPDO       (SNMT_k_OFS_SADR + SNMT_k_LEN_SADR)
#define SNMT_k_OFS_ERR_GROUP    (SNMT_k_OFS_SERV_CMD + SNMT_k_LEN_SERV_CMD)
#define SNMT_k_OFS_ERR_CODE     (SNMT_k_OFS_ERR_GROUP + SNMT_k_LEN_ERR_GROUP)
#define SNMT_k_OFS_TSTMP        (SNMT_k_OFS_SERV_CMD + SNMT_k_LEN_SERV_CMD)
#define SNMT_k_OFS_UDID_SCM     (SNMT_k_OFS_SERV_CMD + SNMT_k_LEN_SERV_CMD)
#define SNMT_k_OFS_EXT_CT       (SNMT_k_OFS_SERV_CMD + SNMT_k_LEN_SERV_CMD)
/** @} */

/**
 * @enum SNMT_t_EXT_SERV_REQ
 * @brief Enumeration of all request commands available for a SNMTS extended service request.
 *
 * The request command is encoded in data byte 0 of payload data of an extended service request telegram.
 */
typedef enum
{
    SNMT_k_SN_SET_TO_PREOP = 0,
    /** The SCM requests the state transition on this node into state OPERATIONAL */
    SNMT_k_SN_SET_TO_OP = 2,
    /** A configuration tool requests the state transition of the SCM into state STOP */
    SNMT_k_SCM_SET_TO_STOP = 4,
    /** A configuration tool requests the state transition of the SCM into state OPERATIONAL */
    SNMT_k_SCM_SET_TO_OP = 6,
    /** The SCM requests this node to respond its current state (SN state) */
    SNMT_k_SCM_GUARD_SN = 8,
    /** The SCM requests the assignment of an additional SADR on this node */
    SNMT_k_ASSGN_ADD_SADR = 10,
    /** The SCM acknowledges a reported error */
    SNMT_k_SN_ACK = 12,
    /** The SCM requests the assignment of the UDID of SCM */
    SNMT_k_ASSGN_UDID_SCM = 14,
    /** The SCM requests the initialization of the extended CT on the SN */
    SNMT_k_INITIALIZE_EXT_CT = 16
} SNMT_t_EXT_SERV_REQ;


#endif

/** @} */
/** @} */
