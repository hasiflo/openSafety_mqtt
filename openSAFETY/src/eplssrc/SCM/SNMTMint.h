/**
 * @addtogroup SNMTM
 * @{
 *
 * @file SNMTMint.h
 * This file is the internal header-file of the unit.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SNMTMint.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
 */


#ifndef SNMTMINT_H
#define SNMTMINT_H

/**
 * Maximum number of SNMTM finite state machines
 *
 * - if EPLS_cfg_SCM == EPLS_k_ENABLE then 1..1023
 * - if EPLS_cfg_SCM == EPLS_k_DISABLE then EPLS_k_NOT_APPLICABLE,
 *
 * @see SCM_cfg_NUM_OF_PROCESSED_NODES_PER_CALL
*/
#define SNMTM_cfg_MAX_NUM_FSM  (UINT16)SCM_cfg_MAX_NUM_OF_NODES


/**
 * This symbol represents an invalid value of a SNMTM FSM number.
 *
 * @attention The maximum number of SN to be configured concurrent must
 * be less than k_INVALID_FSM_NUM.
*/
#define k_INVALID_FSM_NUM              0xFFFFu


/** This symbol represents the payload length of a request without payload data. */
#define k_REQ_NO_DATA_LEN         0x00u

/** Enumeration of events the SNMT Master is able to process in its FSM */
typedef enum
{
	/** request for transition of SNMT Service Request */
  k_EVT_REQ_SEND,
  /** response of any request received */
  k_EVT_RESP_RECEIVED,
  /** demand of response timeout check */
  k_EVT_CHK_TIMEOUT
} t_FSM_EVENT;

extern SOD_t_ACS_OBJECT_VIRT SNMTM_s_AccessSadrScm SAFE_NO_INIT_SEKTOR;

/**
* @brief This function initializes all SNMT Master FSM and the access variables to the
* SNMTM timeout and retries objects.
*
* @return
* - TRUE  - success
* - FALSE - failure
*/
BOOLEAN SNMTM_InitFsm(void);


/**
* @brief This function processes the SNMT Master FSM.
*
* It is called by several superior function with focus on initiating a request
* transmission or focus on repeating a request transmission or processing a
* received response. Thus the interface of this function is multi-functional.

*
* @param dw_ct consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @retval pb_numFreeMngtFrms Is only used in state "wait on response", valid range: <> NULL
* - IN: max. number of openSAFETY frames of type SNMT and SSDO. This number of frames are
*   free to transmit during one appl. cycle
* - OUT: remaining number of free frames of type SNMT and SSDO. A value of zero means that
*   all required services were transm. (not checked, checked in SNMTM_BuildRequest())
*
*
* @param w_fsmNum finite state machine number, assigned internally (not checked, checked in SNMTM_BuildRequest() or SNMTM_AssignResponse() or SNMTM_GetFsmFree())), valid range: 0 .. <SNMTM_cfg_MAX_NUM_FSM-1>
*
* @param e_fsmEvent event which shall be processed by the SNMT Master FSM. (not checked, only called with enum value), valid range: see {t_FSM_EVENT}
*
* @param w_regNum Is only used in state "wait on request". registration number of the request (not checked checked in SCM_Trigger()), valid range: UINT16
*
* @param          ps_hdrInfo Reference to either request/response header info (not checked, called only with reference to struct), valid range: <> NULL
*
* @param          pb_data Reference to either to request/response data (not checked, only called with reference to byte array or checked in processStateReqProc()), valid range: <> NULL
*
* @return
* - TRUE                        - FSM processing successful
* - FALSE                       - FSM processing failed
*/
BOOLEAN SNMTM_ProcessFsm(UINT32 dw_ct, UINT8 *pb_numFreeMngtFrms,
          UINT16 w_fsmNum, t_FSM_EVENT e_fsmEvent, UINT16 w_regNum,
          const EPLS_t_FRM_HDR *ps_hdrInfo, const UINT8 *pb_data);


/**
* @brief This function returns the number of the next free FSM.
*
* If no FSM is free to process a new request, k_INVALID_FSM_NUM is returned.
*
* @return       0 .. SNMTM_cfg_MAX_NUM_FSM-1 - free FSM number k_INVALID_FSM_NUM              - no free FSM available
*/
UINT16 SNMTM_GetFsmFree(void);


/**
* @brief This function assigns a single received response to the matching request that
* is processed in a single FSM waiting for response.
*
* As result the number of the FSM, which processes the request/response is returned.
*
* @param        b_respId          frame ID of received response (checked), valid range: see {t_SNMTM_SERV_REQ} and {t_SNMTM_EXT_SERV_REQ}
*
* @param        pb_respCmd        ref to command byte of received service response (not checked, checked in processStateReqProc()), valid range: <> NULL
*
* @param        w_respTadr        address of node who transmitted the response (not checked, checked in checkRxAddrInfo()), valid range: EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @param        w_rxSdn           received SDN, (not checked, checked in checkRxAddrInfo()), valid range : k_MIN_SDN .. k_MAX_SDN
*
* @return       0 .. (SNMTM_cfg_MAX_NUM_FSM - 1)
*/
UINT16 SNMTM_AssignResponse(UINT8 b_respId, const UINT8 *pb_respCmd,
                            UINT16 w_respTadr, UINT16 w_rxSdn);


#endif

/** @} */
