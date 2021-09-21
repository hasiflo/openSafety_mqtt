/**
 * @addtogroup SNMTS
 * @{
 *
 * @file SNMTSint.h
 *
 * This file contains all defines and data types used for error handling.
 * Every error recognized by this unit is defined in here.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SNMTSsnstate.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>added event for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
 */

#ifndef SNMTSINT_H
#define SNMTSINT_H

/**
 * @name Valid TxSPDO number range
 * @{
 * @brief These symbols represent the valid range of a TxSPDO number that is received with the
 * SNMT Service Request "additional SADR assignment".
 *
 * @note TxSPDO number = 0x0000 is not defined, TxSPDO number = 0x0001 is reserved for main SADR.
 */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
#define k_NUM_OF_ADD_TX_SPDO           (SPDO_cfg_MAX_NO_TX_SPDO - 1)
#else /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */
#define k_NUM_OF_ADD_TX_SPDO           k_NO_ADD_TX_SPDO
#endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */
#define k_NO_ADD_TX_SPDO               0
#define k_MIN_ADD_SPDO_NUM             0x0002u
#define k_MAX_ADD_SPDO_NUM             (UINT16)SPDO_cfg_MAX_NO_TX_SPDO
/** @} */

/**
 * @def k_CT_NOT_USED_INT
 * @brief This symbol is used inside SNMTS for function call SNMTS_SnStateFsm().
 *
 * Internally this symbol represents a 32 bit variable. It used in case of the
 * parameter &lt;dw_ct&gt; is not used.
 */
#define k_CT_NOT_USED_INT              0x00UL

/**
 * @struct t_FAIL_ERROR
 * @brief This data structure is used to store an internal error code and its assigned error group.
 *
 * The structure describes a specific error that causes abortion of the sequence to set the SN to operational state.
 */
typedef struct
{
    /** error group */
    UINT8 b_group;
    /** error code */
    UINT8 b_code;
} t_FAIL_ERROR;

/**
 * @enum t_FSM_STATE
 * @brief This enumeration defines all necessary FSM states.
 */
typedef enum
{
    k_ST_INITIALIZATION,
    /** wait for SADR assignment step1 */
    k_ST_WF_SADR_ASS_STEP1,
    /** wait for SADR assignment step2 */
    k_ST_WF_SADR_ASS_STEP2,
    /** wait for UDID of SCM assignment */
    k_ST_WF_UDID_SCM_ASS,
    /** wait for 1st reception of "SN set to op" step1 */
    k_ST_WF_SET_TO_OP1_STEP1,
    /** wait for 1st reception of "SN set to op" step2 */
    k_ST_WF_SET_TO_OP1_STEP2,
    /** wait for checksum, calculated by SN app. */
    k_ST_WF_API_CHKSUM,
    /** wait for 2nd reception of "SN set to op" */
    k_ST_WF_SET_TO_OP2,
    /** wait for API confirmation to switch to op. */
    k_ST_WF_API_CONF,
    /** wait for 3rd reception of "SN set to op" */
    k_ST_WF_SET_TO_OP3,
    /** wait for 3rd reception of "SN set to op" - Step 1 */
    k_ST_OPERATIONAL_STEP1,
    /** wait for 3rd reception of "SN set to op" - Step 2 */
    k_ST_OPERATIONAL_STEP2
} t_FSM_STATE;

/**
 * @enum t_SN_EVENT
 * @brief This enumeration defines all necessary events that causes SN state transition
 */
typedef enum
{
    /** SNMT Service Request "SADR assignment" received */
    k_EVT_RX_SADR,
#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
    /** SNMT Service Request "Additional SADR assignm. */
    k_EVT_RX_SADR_ADD,
#endif
    /** SNMT Service Request "SN set to op" received */
    k_EVT_RX_SET_TO_OP,
    /** CRC16 Checksum available, provided by SN API */
    k_EVT_CHKSUM_AVAIL,
    /** confirmation available, provided by SN API */
    k_EVT_CONF_AVAIL,
    /** SNMT Service Request "SN set to preop" received */
    k_EVT_RX_SET_TO_PREOP,
    /** SN application forces transition into PRE-OP */
    k_EVT_API_SET_TO_PREOP,
    /** SNMT Service Request "Node guarding" received */
    k_EVT_RX_GUARDING,
    /** check guarding and refresh timer */
    k_EVT_CHK_TIMER,
    /** SNMT Service Request "SN Acknowledge" received */
    k_EVT_SN_ACK,
    /** SNMT Service Request "UDID of SCM assignment" received */
    k_EVT_RX_UDID_SCM,
    /** SNMT Service Request "Initialize CT" received */
    k_EVT_RX_INIT_CT
} t_SN_EVENT;

/**
 * @name Function prototypes for SNMTSsnstate.c
 * @{
 */

/**
 * @brief This function initializes the access to the SOD entry 0x1019, 0x00, which contains the "Unique Device ID (UDID)" of this SN.
 *
 * @param       b_instNum		 instance number (not checked, checked in SSC_InitAll()),
 *                      valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 *  - TRUE           - success
 *  - FALSE          - failure
 */
BOOLEAN SNMTS_InitParamUdid(BYTE_B_INSTNUM);

/**
 * @brief This function initializes SN state machine.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 */
void SNMTS_InitSnState(BYTE_B_INSTNUM);

/**
 * @brief This function processes the SN FSM. It is called with an event that is to process.
 *
 * @param        b_instNum                     instance number (not checked, checked in SNMTS_TimerCheck() or SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct                         consecutive time (not checked, any value allowed), valid range: k_CT_NOT_USED_INT if the calling event is not k_EVT_CHK_TIMER and k_EVT_RX_SET_TO_OP and k_EVT_API_SET_TO_PREOP, otherwise consecutive time (UINT32)
 *
 * @param        e_evtSn                       event to be processed (checked), valid range: see {t_SN_EVENT}
 *
 * @param        ps_rxBuf                      reference to received openSAFETY frame to be distributed (checked), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf                      reference to openSAFETY frame to be transmitted (checked), valid range: see EPLS_t_FRM
 *
 * @retval       pb_numFreeMngtFrms            IN: max. number of openSAFETY frames of type SNMT and SSDO. This number of frames are free to transmit during one appl. cycle OUT: remaining number of free frames of type SNMT and SSDO. (not checked, checked in SNMTS_TimerCheck()), valid range: <> NULL if the calling event is k_EVT_CHK_TIMER, otherwise NULL (not used).
 *
 * @retval          po_busy                       flag signals that SNMT Slave is currently processing a service request:
 *      - TRUE:  Slave busy
 *      - FALSE: Slave ready to process (not checked, only called with refernce to variable) valid range : <> NULL
 *
 * @return
 * - TRUE           - current event processed successfully
 * - FALSE          - processing of current event failed
 */
BOOLEAN SNMTS_SnStateFsm(BYTE_B_INSTNUM_ UINT32 dw_ct, t_SN_EVENT e_evtSn,
        const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
        UINT8 *pb_numFreeMngtFrms, BOOLEAN *po_busy);

/**
 * @brief This function sets the current state of the Safety Node FSM. The function is only used for the unit tests.
 *
 * @param b_instNum             instance number (not checked), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @retval   e_newFsmState         new state of the FSM, valid range: see {t_FSM_STATE}
 */
void SNMTS_SetFsmState(BYTE_B_INSTNUM_ t_FSM_STATE e_newFsmState);

/**
 * @brief This function returns the current state of the SN FSM. The function is only used for the unit tests.
 *
 * @param        b_instNum              instance number (not checked), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return       current state of the SN FSM
 */
t_FSM_STATE SNMTS_GetFsmState(BYTE_B_INSTNUM);
/** @} */

/**
 * @name Function prototypes for SNMTSappInfo.c
 * @{
 */
/**
 * @brief This function initializes the paramters deliveres by the application and the access structure for the "parameter timestamp" SOD entry with index 0x1018 0x07.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure
 */
BOOLEAN SNMTS_AppInfoInit(BYTE_B_INSTNUM);

/**
 * @brief This function stores calculated parameter checksum delivered by the application.
 *
 * @param        b_instNum        instance number (not checked, checked in SNMTS_PassParamChkSumValid()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        o_chksumValid    valid status of the  parameter checksum
 */
void SNMTS_AppCalcParamStore(BYTE_B_INSTNUM_ BOOLEAN o_chksumValid);

/**
 * @brief This function checks if the parameters of the SOD are valid by checking if the checksum over the parameters is valid. Also the timestamp of the parameters within the SOD is checked against the timestamp provided by the SCM to ensure that the SN does have the parameter set being expected ba the SCM.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (not checked, checked in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @return
 * - TRUE           - response transmitted successfully
 * - FALSE          - response transmission failed
 */
BOOLEAN SNMTS_AppChkSumComp(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf);

/**
 * @brief This function stores result of the application confirmation.
 *
 * @param        b_instNum         instance number (not checked, checked in SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        o_enterOp         TRUE  : transition to operational is allowed by the application FALSE : transition to operational is forbidden (not checked, any value allowed)
 *
 * @param        b_errGroup        only relevant if o_enterOp is FALSE group the error is assigned to (not checked, checked in SNMTS_EnterOpState()), valid range: 0..255
 *
 * @param        b_errNum          only relevant if o_enterOp is FALSE internal error number (not checked, any value allowed), valid range: 0 .. 255
 */
void SNMTS_AppConfRespSet(BYTE_B_INSTNUM_ BOOLEAN o_enterOp, UINT8 b_errGroup,
        UINT8 b_errNum);

/**
 * @brief This function gets the application confirmation response for the transition into Operational state.
 *
 * @param        b_instNum           instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not checked, checked in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked, checked in  processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @return
 * - TRUE              - transition granted
 * - FALSE             - transition forbidden, k_SN_FAIL response is already generated
 */
BOOLEAN SNMTS_AppConfRespGet(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
        EPLS_t_FRM *ps_txBuf);
/** @} */

/**
 * @name Function prototypes for SNMTSresponse.c
 * @{
 */
/**
 * @brief This function initializes the access to the SOD entry 0x1C00, 0x01, which contains the "Main source address" of this SN, the SOD entry 0x1200, 0x04, which contains the "UDID of the SCM" and the variables to surveillance the SN FAIL acknowledge.
 *
 * @param   b_instNum        instance number (not checked, checked in SSC_InitAll()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure
 */
BOOLEAN SNMTS_SodAcsInit(BYTE_B_INSTNUM);

/**
 * @brief This function gets the SOD entry 0x1C00, 0x01, which contains the "Main source address" of this SN.
 *
 * @param   b_instNum       instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return       Main SADR
 */
UINT16 SNMTS_MainSadrGet(BYTE_B_INSTNUM);

/**
 * @brief This function sets the SOD entry 0x1C00, 0x01, which contains the "Main source address" of this SN.
 *
 * @param        b_instNum         instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        w_mainSadr 	 	  main source address to be set (not checked, checked in checkRxAddrInfo()), valid range : EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
 *
 * @return
 * - TRUE            - success
 * - FALSE           - failure, error is already reported
 */
BOOLEAN SNMTS_MainSadrSet(BYTE_B_INSTNUM_ UINT16 w_mainSadr);

/**
 * @brief This function sets the SOD entry 0x1200, 0x04, which contains the "UDID of the SCM" of this SN.
 *
 * @param        b_instNum         instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        pb_udidScm        pointer the UDID of the SCM (pointer not checked, checked in SOD_Write()), valid range : <> NULL
 *
 * @return
 * - TRUE            - success
 * - FALSE           - failure, error is already reported
 */
BOOLEAN SNMTS_UdidScmSet(BYTE_B_INSTNUM_ const UINT8 *pb_udidScm);

/**
 * @brief This function prepares all data structures for response transmission to the request "SADR Assignment".
 *
 * @param	b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param    ps_rxBuf         reference to received openSAFETY frame to be distributed (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param    ps_txBuf         reference to openSAFETY frame to be transmitted (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 */
void SNMTS_TxRespSadrAssigned(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
        EPLS_t_FRM *ps_txBuf);

/**
 * @brief This function prepares all data structures for response transmission to the request "UDID". After preparation the response is transmitted.
 *
 * @param        b_instNum            instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        ps_rxBuf             reference to received openSAFETY frame to be distributed (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf             reference to openSAFETY frame to be transmitted (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param        ps_accessUdid        pointer to the SOD access structure to read the UDID from the SOD (not chekced, only called with reference to struct), valid range : <> NULL
 *
 * @return
 * - TRUE               - state processing succeeded
 * - FALSE              - state processing failed
 */
void SNMTS_TxRespUdid(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
        EPLS_t_FRM *ps_txBuf, const SOD_t_ACS_OBJECT_VIRT *ps_accessUdid);

/**
 * @brief This function collects all header info and data of a SNMTS Extended Service Response
 *
 * @param   b_instNum             instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param   ps_rxBuf              reference to received openSAFETY frame to be distributed (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param   ps_txBuf              reference to openSAFETY frame to be transmitted (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param   e_servRespType        service response type (not checked, only called with enum value), valid range: see {t_SNMTS_EXT_SERV_RESP}
 *
 * @param   b_errGroup            only relevant if e_servRespType is k_SN_FAIL, group the error is assigned to (not checked, any value allowed), valid range: 0 .. 255
 *
 * @param   b_errCode             only relevant if e_servRespType is k_SN_FAIL, internal error code (not checked, any value allowed), valid range: 0 .. 255
 *
 * @param   pb_udidScm            Only relevant if e_servRespType is SNMTS_k_UDID_SCM_ASSGN. Pointer to the UDID of the SCM, if the pointer is NULL then ab_respErrUdid is sent (pointer checked), valid range: <> NULL, == NULL
 */
void SNMTS_TxRespExtService(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
        EPLS_t_FRM *ps_txBuf,
        t_SNMTS_EXT_SERV_RESP e_servRespType,
        UINT8 b_errGroup, UINT8 b_errCode,
        const UINT8 *pb_udidScm);

/**
 * @brief This function checks the SN acknowledge and calls the error application callback function if the checking succeeded.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 */
void SNMTS_SnAckRespCheck(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf);

/**
 * @brief This function prepares all data structures for response transmission to the request "Reset node guarding time". After preparation the response is transmitted.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - transmission successful
 * - FALSE          - transmission failed
 */
BOOLEAN SNMTS_TxRespRstGuardScm(BYTE_B_INSTNUM);
/** @} */

/**
 * @name Function prototypes for SNMTSrefresh.c
 * @{
 */
/**
 * @brief This function initializes the "Refresh Time"- mechanism of the unit SNMTS.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - initialization succeeded
 * - FALSE          - initialization failed
 */
BOOLEAN SNMTS_RefreshTimeInit(BYTE_B_INSTNUM);

/**
 * @brief This function starts the "Refresh Time"- interval of unit SNMTS.
 *
 * @see          SNMTS_RefreshTimeCheck()
 *
 * @param        b_instNum        instance number (not checked, checked in SNMTS_TimerCheck()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct            consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @return
 * - TRUE           - refresh time started
 * - FALSE          - error occurred, refresh time not started
 */
BOOLEAN SNMTS_RefreshTimeStart(BYTE_B_INSTNUM_ UINT32 dw_ct);

/**
 * @brief This function checks the "Refresh Timer" of a SN. As a result the SN refresh time can be elapsed or not.
 * If the refresh time elapsed, the refresh timer is restarted.
 *
 * @param        b_instNum          instance number (not checked, checked in SNMTS_TimerCheck()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @retval          po_elapsed         result of refresh time check (not checked, only called with reference to variable in WfSadrAssStep1State()), valid range: TRUE, FALSE
 *
 * @return
 * - TRUE             - refresh time checked successfully
 * - FALSE            - error during refresh time check
 *
 */
BOOLEAN SNMTS_RefreshTimeCheck(BYTE_B_INSTNUM_ UINT32 dw_ct,
        BOOLEAN* po_elapsed);

/**
 * @name Function prototypes for SNMTSlifetime.c
 * @{
 */
/**
 * @brief This function initializes SOD access to the guarding time, and the life factor of the SN. The guarding timeout, life time factor counter and the guard timer running flag are initialized also.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - initialization succeeded
 * - FALSE          - initialization failed
 */
BOOLEAN SNMTS_LifeTimeInit(BYTE_B_INSTNUM);

/**
 * @brief This function stops the "Life Time"- mechanism of the unit SNMTS.
 *
 * @see SNMTS_LifeTimeCheck(), SNMTS_LifeTimeStart()
 *
 * @param b_instNum       instance number (not checked, checked in SNMTS_TimerCheck()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 */
void SNMTS_LifeTimeStop(BYTE_B_INSTNUM);

/**
 * @brief This function starts the "Life Time"- mechanism of the unit SNMTS.
 *
 * @see          SNMTS_LifeTimeCheck(), SNMTS_LifeTimeStop()
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct            consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @return
 * - TRUE           - life time started
 * - FALSE          - error occurred, life time not started
 */
BOOLEAN SNMTS_LifeTimeStart(BYTE_B_INSTNUM_ UINT32 dw_ct);

/**
 * @brief This function checks the "Life Time" of a SN. As a result the SN life time can be elapsed or not. If the life time is not elapsed, the guarding timer is restarted
 *
 * @see           SNMTS_LifeTimeStart(), SNMTS_LifeTimeStop()
 *
 * @param        b_instNum          instance number (not checked, checked in SNMTS_TimerCheck()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @retval          po_elapsed         result of life time check (not checked, only called with reference to variable), valid range: TRUE, FALSE
 *
 * @return
 * - TRUE             - life time checked successfully
 * - FALSE            - error during life time check
 */
BOOLEAN SNMTS_LifeTimeCheck(BYTE_B_INSTNUM_ UINT32 dw_ct, BOOLEAN* po_elapsed);
/** @} */

#endif

/** @} */
