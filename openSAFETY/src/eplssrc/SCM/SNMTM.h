/**
 * @addtogroup SCM_part
 * @{
 * @addtogroup SNMTM [SNTMT] Safety Network Managment Master
 * @{
 *
 * SNMTM, the network management master unit of the openSAFETY Stack.
 *
 * The SNMT Master works closely with the Safety Configuration Manager.
 * The Master provides several services to the SCM to process all SNs of
 * one Safety Domain. The Configuration Manager provides a special callback
 * function that is called by the SNMT Master in case of receiving a response
 * that was not initiated by a precedent service request. Every SN disposes
 * of a SNMT Slave to communicate with the SNMT Master. The SNMT Master Services
 * include requesting the Unique Devive ID (UDID), assigning a logical address
 * (SADR) to a specific SN, SN state control, node guarding and assigning
 * additional logical addresses to a specific SN. The SNMT Master disposes of
 * a finite state machine to process communication with several SNMT Slaves
 * quasi concurrent.
 *
 * Same as Safety Configuration Manager (SCM), only a single SNMT Master is
 * available in the network.
 *
 * @file SNMTM.h
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SNMTM.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SLV10</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>changes for SPDOs with 40 bit CT</td></tr>
 * </table>
 */

#ifndef SNMTM_H
#define SNMTM_H

/**
 * Enumeration of state transitions of the Safety Node (SN) which has the SNMT
 * Master running.
*/
typedef enum
{
  /** state transition from PRE-OPERATIONAL to   OPERATIONAL */
  SNMTM_k_PREOP_TO_OP,
  /** state transition from OPERATIONAL to   PRE-OPERATIONAL */
  SNMTM_k_OP_TO_PREOP
} SNMTM_t_SN_TRANS;



/**
* @brief This function initializes all module global and global variables of the
* SNMT Master.
*
* Every SNMT Master FSM is reset into a initial state to be available to process
* communication with a single SNMT Slave.
*
* @return
* - TRUE  - initialization succeed
* - FALSE - initialization failed
*/
BOOLEAN SNMTM_Init(void);

/**
* @brief This function processes a single Service Response that was identified by the
* control unit (SSC) as a Network Management Response from a SNMT Slave.
*
* The SNMT Master can only process SNMT Slave Service Responses. To pass the requested
* info from the SNMT Master to the SCM, callback functions are called by the SNMT Master.
* The callback functions has to be provided by the SCM. This function searches for the
* matching SNMT Master FSM which is waiting for the received service response and passes
* the service response to the matching FSM. In case of receiving a service response of
* type SNMT_SN_status_PRE_OP, a special callback function is used to inform the SCM or
* the application about the SN status, because no precedent service request initiated
* this type of SNMT Service Response.
*
* @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        ps_respHdr         reference to response header information (not checked, only called with refernece to struct in processStateReqProc()), valid range: <> NULL
*
* @param        pb_respData        reference to response data (not checked, only called with refernece to struct in processStateReqProc()), valid range: <> NULL
*
* @return
* - TRUE             - response successfully processed
* - FALSE            - response processing failed
*/
BOOLEAN SNMTM_ProcessResponse(UINT32 dw_ct, const EPLS_t_FRM_HDR *ps_respHdr,
                              const UINT8 *pb_respData);


/**
* @brief This function checks whether a single FSM is available to process transmission of one service request and reception of the  matching service response. *Attention:* This function must be called to ensure availability of a FSM to process a SNMT Service Request.
*
* @return
* - TRUE  - at least one SNMT Master FSM available
* - FALSE - no SNMT Master FSM available
*/
BOOLEAN SNMTM_CheckFsmAvailable(void);


/**
* @brief This function initiates the SNMT Service Request UDID Request to request the
* physical address (UDID) of a specific SN inside the Safety Domain.
*
* This SN is specified by its logical address (SADR). The function is used by the SCM.
* Requested data is transferred from the SNMT Master to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @see SNMTM_CheckFsmAvailable()
*
* @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum           registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr             logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @return
* - TRUE             - request transmission successfully
* - FALSE            - request transmission failed
*/
BOOLEAN SNMTM_ReqUdid(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr);


/**
* @brief This function initiates the SNMT Service Request SNMT SADR assignment to
* assigning a logical address (SADR) to a specific SN inside the Safety Domain.
*
* The function is used by the SCM. Requested data is transferred from the SNMT Master
* to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @param      dw_ct 			   consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param      w_regNum             registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param      w_sadr               logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param      pb_udid              reference to the physical address (UDID)(checked), valid range: <> NULL
*
* @return
* - TRUE             - request transmission successfully
* - FALSE            - request transmission failed
*/
BOOLEAN SNMTM_ReqAssgnSadr(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr,
                           const UINT8 *pb_udid);

/**
* @brief This function initiates the SNMT Service Request UDID of the SCM assignment
* to assign the UDID of the SCM.
*
* The function is called by the SCM. Requested data is transferred from the SNMT Master
* to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @see							   SNMTM_CheckFsmAvailable()
*
* @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum           registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr             logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        pb_udidScm         reference to the physical address (UDID) of the SCM (not checked, only called with reference to array in SCM_WfAssignSadrResp2() and SCM_WfAssignSadrResp()), valid range: <> NULL
*
* @return
* - TRUE             - request transmission successfully
* - FALSE            - request transmission failed
*/
BOOLEAN SNMTM_ReqAssgnScmUdid(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr,
                              const UINT8 *pb_udidScm);

/**
* @brief This function initiates the initialization of the extended CT value.
*
* The function is called by the SCM. Requested data is transferred from the SNMT Master
* to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @see SNMTM_CheckFsmAvailable()
*
* @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum           registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr             logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        pb_extCt           reference to the extended CT value of the SCM (not checked, only called with reference to array in SCM_WfAssignSadrResp2() and SCM_WfAssignSadrResp()), valid range: <> NULL
*
* @return
* - TRUE             - request transmission successfully
* - FALSE            - request transmission failed
*/
BOOLEAN SNMTM_ReqInitializeCtSn(UINT32 const dw_ct, UINT16 const w_regNum, UINT16 const w_sadr,
                               UINT8 const * const pb_extCt);

/**
* @brief This function initiates the extended SNMT Service Request "Additional SADR
* assignment" to assigning additional logical addresses (SADR) to a specific SN inside
* the Safety Domain.
*
* The SN is addressed by its physical address (UDID). The function is used by the SCM.
* Requested data is transferred from the SNMT Master to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @param        dw_ct 		 		  consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum              registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr                logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        w_sadrAssgn 		  SADR to be assigned to the specified SN (checked), valid range: 2 .. 1023
*
* @param        w_spdoNumAssgn        TxSPDO number to be assigned to the additional SADR (checked), valid range: 2 .. 1023
*
* @return
* - TRUE                - request transmission successfully
* - FALSE               - request transmission failed
*/
BOOLEAN SNMTM_ReqAssgnAddSadr(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr,
                              UINT16 w_sadrAssgn, UINT16 w_spdoNumAssgn);


/**
* @brief This function initiates the extended SNMT Service Request "Node Guarding"
* to keep a specific SN inside the Safety Domain in state OPERATIONAL.
*
* The addressed SN will respond either by SNMT Service Response "SNMT_SN_status_OP"
* or "SNMT_SN_status_PRE_OP":
* - SNMT_SN_status_OP,     requested SN in operational state
* - SNMT_SN_status_PRE_OP, requested SN in preoperational state Requested data is
*   transferred from the SNMT Master to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @param        dw_ct                 consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum              registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr                logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @return
* - TRUE  - request transmission successfully
* - FALSE - request transmission failed
*/
BOOLEAN SNMTM_ReqGuarding(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr);


/**
* @brief This function initiates the extended SNMT Service Request "SN set to
* preoperational" and "SN set to operational" to handle state transitions of a
* specific SN inside the Safety Domain.
*
* A SN can be switched from preoperational to operational or from operational to
* preoperational. Both services need to be acknowledged by the SNMT Slave. Parameter
* timestamp is only used for transition to state operational. The Requested acknowledge
* is transferred from the SNMT Master to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @param        dw_ct                 consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum              registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr                logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        e_snTrans             transition to be processed (checked), valid range: &lt;SNMTM_k_PREOP_TO_OP&gt;, &lt;SNMTM_k_OP_TO_PREOP&gt;
*
* @param        dw_paramTStmp         parameter timestamp, created by the configuration tool, (not checked, any value allowed), valid range: any 32 bit value
*
* @return
* - TRUE                - request transmission successfully FALSE               - request transmission failed
*/
BOOLEAN SNMTM_ReqSnTrans(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr,
                         SNMTM_t_SN_TRANS e_snTrans, UINT32 dw_paramTStmp);


/**
* @brief This function initiates the SNMT Service Request "SNMT SN ack".
*
* The service is used by the SCM to acknowledge an error reported by the SNMT Slave that
* receives the acknowledge telegram. For acknowledge the SCM returns the reported error
* back.
*
* @param        w_sadr              logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        b_errorGroup        classification of error &lt;SNMT_SN_FAIL&gt;, (not checked, any value is allowed), valid range: any 8 bit value
*
* @param        b_errorCode         error code of &lt;SNMT_SN_FAIL&gt;, (not checked, any value is allowed), valid range: any 8 bit value
*
* @return
* - TRUE              - request transmission successfully
* - FALSE             - request transmission failed
*/
BOOLEAN SNMTM_SnErrorAck(UINT16 w_sadr, UINT8 b_errorGroup, UINT8 b_errorCode);


/**
* @brief This function is a callback function which is provided by unit SCM.
*
* The function is called by unit SNMTM to signal a reset guarding was received from a
* specific SN.
*/
void SCM_SNMTM_ResetGuardClbk(void);


#endif

/** @} */
/** @} */
