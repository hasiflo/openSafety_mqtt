/**
 * @addtogroup SNMTM
 * @{
 *
 * @file SNMTMerr.h
 * This file contains all defines and datatypes used for error handling.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * Every error recognized by this unit is defined in here.
 * This file is exported to the application to provide all error codes.
 * <h2>History for SNMTM.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>new error for SPDOs with 40 bit CT</td></tr>
 * </table>
 */


#ifndef SNMTMERR_H
#define SNMTMERR_H


/**
 * This error means that the passed event is invalid in state WF_RESPONSE or
 * is invalid because unknown at all.
*/
#define SNMTM_k_ERR_EVENT_WFRES_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SNMTM_k_UNIT_ID,     1)


/**
 * This error means that an invalid and not defined state transition of the SN was requested
*/
#define SNMTM_k_ERR_SN_TRANS_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL,SNMTM_k_UNIT_ID,      2)


/** This error means that the pointer to data refers to NULL. */
#define SNMTM_k_ERR_REF_REQ_DATA_ASS_SADR_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL,SNMTM_k_UNIT_ID,      3)


/**
 * This error means that the passed parameter "additional SADR" (add. logical
 * address of a SN) is not in valid range.
*/
#define SNMTM_k_ERR_ADD_SADR_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL,SNMTM_k_UNIT_ID,     4)


/**
 * This error means that the passed parameter "SPDO number" (number of a
 * process data object) is not in valid range.
*/
#define SNMTM_k_ERR_SPDO_NUM_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SNMTM_k_UNIT_ID,    5)


/**
 * This error means that no SNMT Master finite state machine is available
 * to process a servive request
 */
#define SNMTM_k_ERR_NO_FSM_AVAIL_REQ_UDID \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SNMTM_k_UNIT_ID,    6)


/**
 * This error means that no SNMT Master finite state machine is available
 *  to process a servive request
 */
#define SNMTM_k_ERR_NO_FSM_AVAIL_ASS_SADR \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SNMTM_k_UNIT_ID,    7)


/**
 * This error means that no SNMT Master finite state machine is available
 * to process a servive request
 */
#define SNMTM_k_ERR_NO_FSM_AVAIL_ASS_ADD_SADR \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SNMTM_k_UNIT_ID,    8)


/**
 * This error means that no SNMT Master finite state machine is available
 * to process a servive request
 */
#define SNMTM_k_ERR_NO_FSM_AVAIL_REQ_GUARD \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SNMTM_k_UNIT_ID,    9)


/**
 * This error means that no SNMT Master finite state machine is available
 * to process a servive request
 */
#define SNMTM_k_ERR_NO_FSM_AVAIL_SN_TRANS \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SNMTM_k_UNIT_ID,    10)


/**
 * This error means that the repetition counter of the specified FSM to
 * process a SNMT Master service exceeded its maximum value.
*/
#define SNMTM_k_ERR_NO_RESP_RECEIVED \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_INFO,SNMTM_k_UNIT_ID,  11)


/**
 * This error means that no SNMT Master FSM is actually processing a request,
 * that is waiting for the received response.
*/
#define SNMTM_k_ERR_RESP_NOT_ASSIGNED \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR,SNMTM_k_UNIT_ID, 12)


/**
 * This error means that the received openSAFETY frame ID is NOT valid.
 * Frame type and telegram type are encoded inside the frame ID
 * and exactly defined.
*/
#define SNMTM_k_ERR_RX_FRM_ID_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL,SNMTM_k_UNIT_ID,     13)


/**
 * This error means that a received SNMTM Service Response was rejected by
 * filtering of unit SNMTM. The reason for the rejection was the wrong
 * destination address of the received Service Response.
*/
#define SNMTM_k_ERR_RESP_REJ_WRONG_DSADR \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_INFO, SNMTM_k_UNIT_ID, 14)


/**
 * This error means that the TR-field in the received response header is NOT
 * filled with zero. The "Time Request Distinctive Number" is not used.
*/
#define SNMTM_k_ERR_RESP_TR_FIELD_NOT_UNUSED \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_INFO, SNMTM_k_UNIT_ID, 15)


/**
 * This error means that the CT-field in the received response header is NOT
 * filled with zero. The "Consecutive Time" is not used.
*/
#define SNMTM_k_ERR_RESP_CT_FIELD_NOT_UNUSED \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_INFO, SNMTM_k_UNIT_ID, 16)


/**
 * This error means that the SNMTM FSM was called in state "wait for request"
 * with any other event than k_EVT_REQ_SEND.
*/
#define SNMTM_k_ERR_EVT_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SNMTM_k_UNIT_ID,    17)

/**
 * This error means that no SNMT Master finite state machine is available
 * to process a service request */
#define SNMTM_k_ERR_NO_FSM_AVAIL_REQ_SCM_UDID \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SNMTM_k_UNIT_ID,    18)

/**
 * This error means that the reference to the number of free frames is invalid
 */
#define SNMTM_k_ERR_REF_FREE_FRMS \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SNMTM_k_UNIT_ID,    19)

/**
 * This error means that no SNMT Master finite state machine is available
 * to process a service request */
#define SNMTM_k_ERR_NO_FSM_AVAIL_INIT_CT \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SNMTM_k_UNIT_ID,    20)


#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param        w_errorCode        error number
*
* @param        dw_addInfo         additional error information
*
* @retval        pac_str            empty buffer to build the error string.
*/
void SNMTM_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str);
#endif


#endif

/** @} */
