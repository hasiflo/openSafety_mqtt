/**
 * @addtogroup SCM
 * @{
 * @file SCMapi.h
 *
 * This file is the application interface header file of the unit.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author Joachim Stolberg, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * The aim of the SCM is to configure, verify and start all SNs of the assigned domain.
 * After the configuration of the SNs the SCM performs the node guarding for all SNs,
 * which have reached the state OPERATIONAL.
 *
 * <h2>History for SCMapi.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>06.09.2011</td><td>Hans Pill</td><td>A&P263905 added interface for openSafety Stack</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 *     <tr><td>32.01.2013</td><td>Hans Pill</td><td>changes for additional parameters according to openSafety specification</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>new module states for SPDOs with 40 bit CT</td></tr>
 * </table>
 *
 * @defgroup scmapi_cb Callback Functions
 */

#ifndef SCM_API_H
#define SCM_API_H

/**
 * @name Safety Node Status of the SOD
 * @{
 * Entry C400h - C7FEh, sub index 5
 */
/** SN is missing */
#define SCM_k_NS_MISSING          (UINT8)0
/** SN is invalid at this SADDR */
#define SCM_k_NS_INVALID          (UINT8)1
/** SN is valid, but responds to the wrong SADR */
#define SCM_k_NS_WRONG_SADR       (UINT8)2
/** SN reports the wrong UDID */
#define SCM_k_NS_UDID_MISMATCH    (UINT8)3
/** SN has a wrong parameter set */
#define SCM_k_NS_WRONG_PARAM      (UINT8)4
/** SN is missing additional parameters */
#define SCM_k_NS_WRONG_ADD_PARAM  (UINT8)5
/** SN has an incompatible version */
#define SCM_k_NS_INCOMP_VERSION   (UINT8)6
/** SN error while initializing the extended CT */
#define SCM_k_NS_ERR_INIT_CT      (UINT8)7
/** SN configuration is valid */
#define SCM_k_NS_VALID            (UINT8)128
/** SN is correctly configured and operational */
#define SCM_k_NS_OK               (UINT8)129
/** @} */

/**
* @brief This function initializes and activates the safety configuration manager
* inclusive the node guarding for all nodes.
*
* @see SCM_Deactivate()
*
* @return
* - TRUE           - function successfully finished
* - FALSE          - abort forcing error
*/
BOOLEAN SCM_Activate(void);

/**
* @brief This function deactivates the safety configuration manager inclusive the
* node guarding for all nodes.
*
* @see SCM_Activate()
*/
void SCM_Deactivate(void);

/**
* @brief This function is called from the SCM to receive the initial CT value.
*
* @ingroup scmapi_cb
*
* @return
* - < 0x000000FFFFFFFFFFULL   - valid initial CT value
* - >= 0x000000FFFFFFFFFFULL  - invalid initial CT value
*/
UINT64 SAPL_SCM_GetInitialCtValueClbk(void);

/**
 * @brief This function resets the node guarding.
 *
 * It can be called, if
 * - the SCM is in the "IDLE2" state then the safety address verification is started again.
 * - the SCM is waiting for the operator acknowledge then the safety address verification is started again.
 * - SNs are guarded by the SCM then SNs are guarded immediately again without guarding request timeout.
 *
 * Otherwise the reset node guarding request is ignored.
*/
void SCM_ResetNodeGuarding(void);

/**
* @brief This function is called for every UDID mismatch.
*
* The operator has to acknowledge this mismatch be means of SCM_OperatorAck()
*
* @ingroup scmapi_cb
*
* @see		SCM_OperatorAck()
*
* @param 	w_sadr 		SADR of the SN with the UDID mismatch.
*
* @param 	pa_newUdid 	Pointer to the UDID, which is received from the SN.
*
* @param 	w_hdl 		Handle to the FSM with the UDID mismatch, necessary for SCM_OperatorAck().
*/
void SAPL_ScmUdidMismatchClbk(UINT16 w_sadr, const UINT8 *pa_newUdid,
                              UINT16 w_hdl);

/**
* @brief This function must be called after every UDID mismatch, notified via
* SAPL_ScmUdidMismatchClbk().
*
* The via w_hdl specified SN will then go through the "Handle Single UDID Mismatch"
* process.
*
* @see  	SAPL_ScmUdidMismatchClbk()
*
* @param w_hdl		Handle as reference to the SN with the UDID mismatch.
* 	(checked) valid range: < SCM_cfg_MAX_NUM_OF_NODES and corresponding FSM must
* 	be in the state SCM_k_WF_OPERATOR_ACK.
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_OperatorAck(UINT16 w_hdl);

/**
* @brief  This function is called after every 'SN FAIL' during the process 'Activate SN'.
*
* The application may acknowledge this report be means of SCM_SnFailAck().
*
* @ingroup scmapi_cb
*
* @see       SCM_SnFailAck()
*
* @param w_sadr		SADR of the SN with the 'SN FAIL'.
*
* @param b_errGroup error group from the SNMT_SN_FAIL response frame.
*
* @param b_errCode 	error code from the SNMT_SN_FAIL response frame.
*
* @param w_hdl 		Handle to the SN with the UDID mismatch, necessary for SCM_SnFailAck().
*/
void SAPL_ScmSnFailClbk(UINT16 w_sadr, UINT8 b_errGroup, UINT8 b_errCode,
                        UINT16 w_hdl);

/**
* @brief This function must be called by the application after every 'SN FAIL',
* notified via SAPL_ScmSnFailClbk(). It stores the given parameters.
*
* @see          SAPL_ScmSnFailClbk()
*
* @param w_hdl			Handle from SAPL_ScmSnFailClbk() as node reference (checked).
* 	valid range: < SCM_cfg_MAX_NUM_OF_NODES and corresponding FSM must be in the state
* 	SCM_k_WF_SAPL_ACK.
*
* @param b_errGroup 	error group for the 'SNMT_SN_ACK' acknowledge frame (not checked,
* 	checked in SNMTS_SnAckRespCheck()). valid range: UINT8
*
* @param b_errCode 		error code from the 'SNMT_SN_ACK' acknowledge frame (not checked,
* 	checked in SNMTS_SnAckRespCheck()). valid range: UINT8
*
* @return
* - FALSE       - Abort forcing error
*/
BOOLEAN SCM_SnFailAck(UINT16 w_hdl, UINT8 b_errGroup, UINT8 b_errCode);

/**
* @brief This function is called for every Safety Node status change.
*
* @ingroup scmapi_cb
*
* @param w_sadr		SADR of the SN with the status change.
*
* @param         	b_oldStatus         The old SN status
*
* @param        	b_newStatus         The new SN status
*/
void SAPL_ScmNodeStatusChangedClbk(UINT16 w_sadr, UINT8 b_oldStatus,
                                   UINT8 b_newStatus);

/**
* @brief This function is called, after the revision number from the SN is received.
*
* @ingroup scmapi_cb
*
* The application has to check if the revision number should be accepted or not.
*
* @param w_sadr 			SADR of the SN with the revision number.
*
* @param dw_expRevNum		The expected revision number of the SN.
*
* @param dw_recRevNum 		The received revision number of the SN.
*
* @return
* - TRUE              - The revision number will be accepted.
* - FALSE             - The revision number will NOT be accepted.
*/
BOOLEAN SAPL_ScmRevisionNumberClbk(UINT16 w_sadr, UINT32 dw_expRevNum,
                                   UINT32 dw_recRevNum);

/**
* @brief This function triggers the node configuration respectively the node guarding FSM.
*
* @attention This function must not be called in SNMTS_k_ST_INITIALIZATION state.
* This function must be called with OUT value of number of free frame > 0 at least
* once within the smallest of the following timeouts (SOD index, sub-index)
* - life guarding:
* 	- (EPLS_k_IDX_LIFE_GUARDING, EPLS_k_SUBIDX_GUARD_TIME)
* - reset guarding:
*   - (EPLS_k_IDX_PRE_OP_SIGNAL, EPLS_k_SUBIDX_REFRESH_TIME)
*
* @param       dw_ct		consecutive time (not checked, any value allowed)
* 	valid range: UINT32
*
* @retval      pb_numFreeFrms
* - IN: number of EPLS frames are free to transmit. (checked),
*   valid range: <> NULL, value > 0
* - OUT: remaining number of free frames. If the value is zero, then not all SN may
*   triggered.
*/
void SCM_Trigger(UINT32 dw_ct, UINT8 *pb_numFreeFrms);

/**
* @brief This function is called from the SCM to ensure that the node my be processed.
*
* @ingroup scmapi_cb
*
* @param         w_sadr    SADR of the SN which is to be processed
*
* @return
* - TRUE - The SN may be processed
* - FALSE - The SN must not be processed.
*/
BOOLEAN SAPL_ScmProcessSn(UINT16 const w_sadr);

#endif /* #ifndef SCM_API_H */


/** @} */
