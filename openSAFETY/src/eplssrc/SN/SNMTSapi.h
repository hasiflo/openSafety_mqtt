/**
 * @addtogroup SNMTS
 * @{
 *
 * @file SNMTSapi.h
 *
 * This unit provides functionality to handle stack internal errors.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SNMTSapi.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>17.01.2013</td><td>Hans Pill</td><td>changes for implementation of additional parameters according to the openSafety specification</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>24.06.2014</td><td>Hans Pill</td><td>added callback functions to report state changes</td></tr>
 *     <tr><td>26.07.2018</td><td>Stefan Innerhofer</td><td>add SN fail error code for unexpected FSM event in the SNMT state machine</td></tr>
 * </table>
 */

#ifndef SNMTSAPI_H
#define SNMTSAPI_H

/**
 * @enum SNMTS_t_SN_STATE_MAIN
 * @brief This enumeration defines all SN states, which are seen by the SNMT Master and the application.
 */
typedef enum
{
    /** initialization state */
    SNMTS_k_ST_INITIALIZATION,
    /** pre-operational state */
    SNMTS_k_ST_PRE_OPERATIONAL,
    /** operational state */
    SNMTS_k_ST_OPERATIONAL
} SNMTS_t_SN_STATE_MAIN;

/**
 * @name Fail error group
 * @{
 * These symbols represent the specified error groups used to describe the reason of failure of setting the SN
 * to operational state (see SNMTS_EnterOpState()). The symbols are used in the SNMT Service Response
 * &lt;SNMT_SN_FAIL&gt; and SNMT Service Request &lt;SNMT SN ACK&gt;.
 */
/** group device */
#define SNMTS_k_FAIL_GRP_DVC  0u
/** group application */
#define SNMTS_k_FAIL_GRP_APL  1u
/** group parameter */
#define SNMTS_k_FAIL_GRP_PAR  2u
/** group vendor */
#define SNMTS_k_FAIL_GRP_VDR  3u
/** group openSAFETY Stack */
#define SNMTS_k_FAIL_GRP_STK  4u
/** group additional parameters */
#define SNMTS_k_FAIL_GRP_ADD  5u
/** @} */

/**
 * @name Fail error code
 * @{
 * These symbols represent the stack internal error code used to describe the reason for abortion of setting the
 * SN to operational state.
 *
 * The symbols are used in the SNMT Service Response &lt;SNMT_SN_FAIL&gt; and SNMT Service Request &lt;SNMT SN ACK&gt;.
 */
/** default error */
#define SNMTS_k_FAIL_ERR_DEFAULT                            0u
/** calculated and received CRC checksum are NOT equal */
#define SNMTS_k_FAIL_ERR_CRC_CHKSUM                         1u
/** RxSPDO mapping failed */
#define SNMTS_k_FAIL_ERR_MAP_FAILED_RX                      2u
/** TxSPDO mapping failed */
#define SNMTS_k_FAIL_ERR_MAP_FAILED_TX                      3u
/** Unexpected FSM event in the SNMT state machine*/
#define SNMTS_k_FAIL_ERR_UNEXPECTED_FSM_EVENT				4u
/** mask for additional parameter index */
#define SNMTS_k_FAIL_ERR_ADD_IDX_MASK						0x0Fu
/** mask for checking if header only is to be sent */
#define SNMTS_k_FAIL_ERR_ADD_HEAD_MASK						0xF0u
/** @} */

/**
 * @brief This function checks the guarding timer and the refreshing timer of the SN.
 *
 * The guarding timer is only checked in SN state OPERATIONAL. The refreshing timer is only checked in
 * SN state PRE-OPERATIONAL.
 *
 * @attention This function must be called at least once within the smallest value of the Refresh Time
 * (SOD index 0x100D sub-index 0x00) or Guarding Time (SOD index 0x100C sub-index 0x01) with number of
 * free management frame > 0.
 *
 * @param        b_instNum                     instance number (checked), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct                         consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @retval        pb_numFreeMngtFrms            IN: max. number of openSAFETY frames of type SNMT and SSDO. This number of
 *                       frames are free to transmit. (checked) , valid range: <> NULL, value > 0.
 *                       Returns the remaining number of free frames of type SNMT and SSDO. A value
 *                       of zero means that one or several SNMTS services were transmitted.
 *
 */
void SNMTS_TimerCheck(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT8 *pb_numFreeMngtFrms);

/**
 * @brief This function returns the current state of the Safety Node.
 *
 * @param       b_instNum              instance number (checked), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param       pe_actSnState          current state of the SN, (pointer checked), valid range: <> NULL, see SNMTS_t_SN_STATE_MAIN
 *
 * @return
 * - TRUE                - state valid
 * - FALSE               - state invalid
 */
BOOLEAN SNMTS_GetSnState(BYTE_B_INSTNUM_ SNMTS_t_SN_STATE_MAIN *pe_actSnState);

/**
 * @brief This function is a callback function which is provided by the openSAFETY Application.
 *
 * The function is called by the openSAFETY Stack (unit SNMTS) in case of reception of SNMT Service "SN set to OPERATIONAL".
 * This callback function requests the application of the SN to calculate the checksum of the current SOD.
 *
 * @attention To pass the calculated parameter checksum to the SNMTS the application must call the API function
 * SNMTS_PassParamChkSumValid(). This API function MUST NOT be called within this callback function.
 *
 * @param       b_instNum         instance number, valid range:  0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 */
void SAPL_SNMTS_CalcParamChkSumClbk(BYTE_B_INSTNUM);

/**
 * @brief This function is used to store the CRC parameter checksum validation status calculated by the SN's application.
 *
 * @attention This function has to be called by the openSAFETY Application after the checksum calculation was
 * requested by the openSAFETY Stack via the callback function SAPL_SNMTS_CalcParamChkSumClbk().
 *
 * @param        b_instNum                      instance number (checked) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        o_calcParamChkSumValid         calculated parameter checksum is valid or not
 *
 * @return
 * - TRUE                   - valid function call after calculation was request
 * - FALSE                  - invalid function call, calculation request pending
 */
BOOLEAN SNMTS_PassParamChkSumValid(BYTE_B_INSTNUM_
        BOOLEAN o_calcParamChkSumValid);

/**
 * @brief This function is a callback function which is provided by the openSAFETY Application.
 *
 * The function is called by the openSAFETY Stack (unit SNMTS) to indicate that a SN state transition from
 * PRE-OPERATIONAL to OPERATIONAL was requested by the SCM. To be able to respond the requested state
 * transition the openSAFETY Application has to admit the transition  by calling the function SNMTS_EnterOpState().
 *
 * @attention To inform the SNMTS about the API confirmation of switching into state OPERATIONAL the application
 * must call the API function SNMTS_EnterOpState(). This API function MUST NOT be called within this callback function.
 *
 * @param        b_instNum        instance number, valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 */
void SAPL_SNMTS_SwitchToOpReqClbk(BYTE_B_INSTNUM);

#if (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE)
/**
 * @brief Callback to signal a request for state change between PRE-OPERATIONAL to PRE-OPERATIONAL
 *
 * The function is called by the openSAFETY Stack (unit SNMTS) to indicate that a SN state transition from
 * PRE-OPERATIONAL to PRE-OPERATIONAL was requested by the SCM.
 *
 * @param        b_instNum        instance number, valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 */
void SAPL_SNMTS_SwitchPreOpPreOpClbk(BYTE_B_INSTNUM);

/**
 * @brief Callback to signal a request for state change between OPERATIONAL to PRE-OPERATIONAL
 *
 * The function is called by the openSAFETY Stack (unit SNMTS) to indicate that a SN state transition from
 * OPERATIONAL to PRE-OPERATIONAL was requested by the SCM, or the guarding timeout elapsed.
 *
 * @param        b_instNum        instance number, valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 */

void SAPL_SNMTS_SwitchOpPreOpClbk(BYTE_B_INSTNUM);

/**
 * @brief Callback to signal a nodeguarding timeout
 *
 * The function is called by the openSAFETY Stack (unit SNMTS) to indicate that the nodeguarding timeout
 * of the SN expired.
 *
 * @param        b_instNum        instance number, valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 */

BOOLEAN SAPL_SNMTS_NodeguardingExpiredClbk(BYTE_B_INSTNUM);

typedef enum
{
    /** The SNMTS reports error information to the SNMTM */
    SNMTS_k_FAIL,
    /**
     * The SNMTS signals to the SNMTM (SCM):
     * - waiting for API confirmation to switch to OP
     */
    SNMTS_k_BUSY,
    /** The SNMTS acknowledges the current state is OPERATIONAL */
    SNMTS_k_OP
}t_SNMTS_SWITCH_OP_ALLOWED;

/**
 * @brief Callback to signal a request for state change between OPERATIONAL to OPERATIONAL
 *
 * The function is called by the openSAFETY Stack (unit SNMTS) to indicate that a SN state transition from
 * OPERATIONAL to OPERATIONAL was requested by the SCM. It can be used to trigger a re-verification of an
 * additional parameter set, in the case of an SCM rebooting, without the SN falling back to a
 * PRE-OPERATIONAL state.
 *
 * @param        b_instNum        instance number, valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 * @param        pb_errorGroup    pointer to the classification of error &lt;SNMT_SN_FAIL&gt;, (not checked, any value is allowed), valid range: any 8 bit value
 * @param        pb_errorCode     pointer to the error code of &lt;SNMT_SN_FAIL&gt;, (not checked, any value is allowed), valid range: any 8 bit value
 *
 * @return
 * - SNMTS_k_FAIL              - state transition is not allowed, reason is contained in the error group and error code.
 * - SNMTS_k_BUSY              - state transition is to be delayed, calculation in progress
 * - SNMTS_k_OP                - state transition is allowed.
 */
t_SNMTS_SWITCH_OP_ALLOWED SAPL_SNMTS_SwitchOpOpClbk(BYTE_B_INSTNUM_ UINT8 * const pb_errorGroup, UINT8 * const pb_errorCode);
#endif /* (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE) */

/**
 * @brief This function is used to control the SN state transition into state OPERATIONAL.
 *
 * The function has to be called by the openSAFETY Application after the state transition was requested by the
 * openSAFETY Stack via the callback function SAPL_SNMTS_SwitchToOpReqClbk(). In case of transition to Operational
 * is forbidden, the application must specify a transition fail error and its assignment to an error group.
 *
 * @attention Error group SNMTS_k_FAIL_GRP_STK is reserved for openSAFETY Stack error codes
 *
 * @param        b_instNum             instance number (checked) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        o_enterOpState        TRUE : state transition to state OPERATIONAL is allowed FALSE: state transition to
 *                   state OPERATIONAL is NOT allowed (not checked, any value allowed)
 *
 * @param        b_errorGroup          group which the reported error is assigned to. Only valid in case of
 *                   o_enterOpState == FALSE (checked), valid range: see {fail error group} except SNMTS_k_FAIL_GRP_STK
 *
 * @param        b_errorCode           error reported by the application. Only valid in case of o_enterOpState == FALSE
 *                   (not checked), valid range: application specific
 *
 * @return
 * - TRUE                - valid function call after state transition was request
 * - FALSE               - invalid function call, no state transition request pending.
 */
BOOLEAN SNMTS_EnterOpState(BYTE_B_INSTNUM_ BOOLEAN o_enterOpState,
        UINT8 b_errorGroup, UINT8 b_errorCode);

/**
 * @brief This function is a callback function which is provided by the openSAFETY Application.
 *
 * The function is called by the openSAFETY Stack (unit SNMTS) in case of reception of SNMT Service "SNMT SN ack".
 * This callback function passes the acknowledge on a reported error to the SN's application.
 *
 * @param        b_instNum           instance number, valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        b_errorGroup        error group see {fail error group} (not checked) valid range: any 8 bit value
 *
 * @param        b_errorCode         error code see {fail error code} (not checked) valid range: any 8 bit value
 */
void SAPL_SNMTS_ErrorAckClbk(BYTE_B_INSTNUM_ UINT8 b_errorGroup,
        UINT8 b_errorCode);

/**
 * @brief This function forces the SNMT Slave to switch the current SN into state PREOPERATIONAL.
 *
 * @attention This function must be called after the initialization of the openSAFETY SW see function SSC_InitAll().
 * Before calling this function, values in the SOD may be updated by the application. This function must not be
 * called in state Preoperational. This function may be called in state Operational.
 *
 * @param        b_instNum        instance number (checked) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct            consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @return
 * - TRUE           - state transition executed
 * - FALSE          - state transition NOT executed
 */
BOOLEAN SNMTS_PerformTransPreOp(BYTE_B_INSTNUM_ UINT32 dw_ct);

/**
 * @brief This function is a callback function which is provided by the openSAFETY Application. The function is called by the
 * openSAFETY Stack (unit SNMTS) to check whether the "parameter set" downloaded by the SCM is already processed.
 *
 * @param        b_instNum        instance number, valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - "parameter set" is already processed
 * - FALSE          - "parameter set" is not processed
 */
BOOLEAN SAPL_SNMTS_ParameterSetProcessed(BYTE_B_INSTNUM);

#endif

/** @} */
