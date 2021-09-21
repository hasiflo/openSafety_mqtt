/**
 * @addtogroup SNMTS
 * @{
 *
 * @file SNMTSsnstate.c
 *
 *  The file contains the SN FSM, which controls the state transitions of a SN.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SNMTSsnstate.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>24.06.2014</td><td>Hans Pill</td><td>added callback functions to report state changes</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>changes for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 *     <tr><td>22.03.2017</td><td>Bernd Thiemann</td><td>SAPL callback if nodeguarding expired</td></tr>
 *     <tr><td>01.02.2018</td><td>Roman Zwischelsberger</td><td>SOD.h is required for SPDO_cfg_40_BIT_CT_SUPPORT also</td></tr>
 *     <tr><td>26.07.2018</td><td>Stefan Innerhofer</td><td>fix the handling of unexpected event in the SNMTS FSM, general code improvement</td></tr>
 * </table>
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "SCFMapi.h"
#include "SHNF.h"
#include "SERRapi.h"
#include "SERR.h"
#include "SFS.h"
#include "SODapi.h"
#include "SOD.h"
#include "SDN.h"
#if ( (0 != SPDO_cfg_MAX_NO_TX_SPDO) || (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) )
    #include "SPDO.h"
#endif
#include "SNMT.h"
#include "SNMTSapi.h"
#include "SNMTS.h"
#include "SNMTSint.h"
#include "SNMTSerr.h"

#include "sacun.h"

/**
 * @var ae_ActFsmState
 * Array to store the actual FSM state.
 */
static t_FSM_STATE ae_ActFsmState[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

static BOOLEAN InitializationState(BYTE_B_INSTNUM_ UINT32 dw_ct,
        t_SN_EVENT e_evtSn, const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf);
static BOOLEAN WfSadrAssStep1State(BYTE_B_INSTNUM_ UINT32 dw_ct,
                                   t_SN_EVENT e_evtSn,
                                   const EPLS_t_FRM *ps_rxBuf,
                                   EPLS_t_FRM *ps_txBuf,
                                   UINT8 *pb_numFreeMngtFrms, BOOLEAN *po_busy);
static BOOLEAN WfSadrAssStep2State(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                                   const EPLS_t_FRM *ps_rxBuf,
                                   EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy);
static BOOLEAN WfUdidScmAssState(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                                 const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy);
static BOOLEAN WfSetToOp1Step1State(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                                    const EPLS_t_FRM *ps_rxBuf,
                                    EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy);
/* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
static BOOLEAN WfSetToOp1Step2State(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                                    const EPLS_t_FRM *ps_rxBuf,
                                    EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy);
#endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */
static BOOLEAN WfApiChksumState(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                                const EPLS_t_FRM *ps_rxBuf,
                                EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy);
static BOOLEAN WfSetToOp2State(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                               const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
                               BOOLEAN *po_busy);
static BOOLEAN WfApiConfState(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                              const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
                              BOOLEAN *po_busy);
static BOOLEAN WfSetToOp3State(BYTE_B_INSTNUM_ UINT32 dw_ct, t_SN_EVENT e_evtSn,
                               const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
                               BOOLEAN *po_busy);
STATIC BOOLEAN OperationStep1State(BYTE_B_INSTNUM_ UINT32 dw_ct,
                                   t_SN_EVENT e_evtSn,
                                   const EPLS_t_FRM *ps_rxBuf,
                                   EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy);
static BOOLEAN OperationStep2State(BYTE_B_INSTNUM_ UINT32 dw_ct,
                                   t_SN_EVENT e_evtSn,
                                   const EPLS_t_FRM *ps_rxBuf,
                                   EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy);

static BOOLEAN checkParam(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                          const EPLS_t_FRM *ps_rxBuf,
                          const EPLS_t_FRM *ps_txBuf);

#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
static void enterAddSadr(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf);
#endif
static BOOLEAN HandleUnexpectedFsmEvent(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn, UINT32 eventCode,
                                       const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf);

/*This function handles an unexpected event in the SNMTS FSM
 *
 * @param        b_instNum        instance number (not checked, checked in SNMTS_TimerCheck() or
 *     SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *     SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        e_evtSn                       event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        eventCode                     evnetCode to signal the unexpected event
 *
 * @param        ps_rxBuf                      reference to received openSAFETY frame to be distributed (checked), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf                      reference to openSAFETY frame to be transmitted (checked), valid range: see EPLS_t_FRM
 *
 * @return
 * - TRUE                - a SN fail was sent to the SCM
 * - FALSE               - no SN fail was sent to the SCM
 */
static BOOLEAN HandleUnexpectedFsmEvent(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn, UINT32 eventCode,
                                       const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf)
{
	BOOLEAN o_return = FALSE;

	SERR_SetError(B_INSTNUM_ eventCode, (UINT32)(e_evtSn));

	if ((ps_rxBuf != NULL) && (ps_txBuf != NULL))
	{

		/*send sn fail so signal the SCM the unexpected event*/
		SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,SNMTS_k_SN_FAIL,SNMTS_k_FAIL_GRP_STK,SNMTS_k_FAIL_ERR_UNEXPECTED_FSM_EVENT,(UINT8 *) NULL );

		o_return = TRUE;
	}

	/* If one of the pointers is NULL it is not possible to sand a sn fail. To end the erroneous condition
	 * change the state anyway because the SCN will send assign SADR as next SNMT message*/
	SNMTS_SetFsmState(B_INSTNUM_ k_ST_WF_SADR_ASS_STEP1);

    return o_return;
}

#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
/**
 * @brief This function enters the initial value of the extended CT
 *
 *
 * @param        b_instNum        instance number (not checked, checked in SNMTS_TimerCheck() or
 *     SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *     SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 * @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (not
 *     checked, checked in checkParam()), valid range: see EPLS_t_FRM
 */
static void enterExtCt(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf)
{
  UINT64 ddw_initExtCt = 0;

  /* get the initial value from the openSAFETY frame */
  /* UDID of the SCM is copied from the received buffer */
  SFS_NET_CPY_DOMSTR(&ddw_initExtCt, &(ps_rxBuf->ab_frmData[SNMT_k_OFS_EXT_CT]),
                     EPLS_k_LEN_EXT_CT);

  SPDO_SetExtCtInitValue(B_INSTNUM_ ddw_initExtCt);

  SCFM_TACK_PATH();
  return;
}
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */

/**
 * @brief This function initializes SN state machine.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()),
 *       valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 */
void SNMTS_InitSnState(BYTE_B_INSTNUM)
{
    /* SN state is initialized */
    ae_ActFsmState[B_INSTNUMidx] = k_ST_INITIALIZATION;

    SCFM_TACK_PATH();
}

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
BOOLEAN SNMTS_GetSnState(BYTE_B_INSTNUM_ SNMTS_t_SN_STATE_MAIN *pe_actSnState)
{
    BOOLEAN o_return = FALSE;

#if(EPLS_cfg_MAX_INSTANCES > 1)
    /* if instance number is invalid */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
        /* error: instance number bigger than maximum */
        SERR_SetError(EPLS_k_NO_INSTANCE_ SNMTS_k_ERR_GET_SN_STATE_INST_INV,
                (UINT32)B_INSTNUM);
    }
    else /* instance number valid */
#endif
    /* if the state pointer is NULL */
    if (pe_actSnState == NULL )
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_PTR_INV, SERR_k_NO_ADD_INFO);
    }
    else /* all parameter are OK */
    {
        /* if the state of the FSM is k_ST_INITIALIZATION */
        if (ae_ActFsmState[B_INSTNUMidx] == k_ST_INITIALIZATION)
        {
            *pe_actSnState = SNMTS_k_ST_INITIALIZATION;
        }
        /* else if the state of the FSM is k_ST_OPERATIONAL_STEP1 or
         k_ST_OPERATIONAL_STEP2 */
        else if ((ae_ActFsmState[B_INSTNUMidx] == k_ST_OPERATIONAL_STEP1)||
               (ae_ActFsmState[B_INSTNUMidx] == k_ST_OPERATIONAL_STEP2))
      {
        *pe_actSnState = SNMTS_k_ST_OPERATIONAL;
      }
      else /* other FSM state */
      {
        *pe_actSnState = SNMTS_k_ST_PRE_OPERATIONAL;
      }
      o_return = TRUE;
    }
    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function processes the SN FSM. It is called with an event that is to process.
 *
 * @param        b_instNum                     instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct                         consecutive time (not checked, any value allowed),
 *       valid range: k_CT_NOT_USED_INT if the calling event is not k_EVT_CHK_TIMER and k_EVT_RX_SET_TO_OP
 *       and k_EVT_API_SET_TO_PREOP, otherwise consecutive time (UINT32)
 *
 * @param        e_evtSn                       event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        ps_rxBuf                      reference to received openSAFETY frame to be distributed (checked), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf                      reference to openSAFETY frame to be transmitted (checked), valid range: see EPLS_t_FRM
 *
 * @retval       pb_numFreeMngtFrms
 * - *input*: max. number of openSAFETY frames of type SNMT and SSDO. This number of frames are free to transmit during one application cycle
 * - *output*: remaining number of free frames of type SNMT and SSDO. (not checked, checked in SNMTS_TimerCheck()), valid range:
 *       - <> NULL if the calling event is k_EVT_CHK_TIMER,
 *       - otherwise NULL (not used).
 *
 * @retval          po_busy                    flag signals that SNMT Slave is currently processing a service request:
 * - TRUE:  Slave busy
 * - FALSE: Slave ready to process (not checked, only called with refernce to variable) valid range : <> NULL
 *
 * @return
 * - TRUE           - current event processed successfully
 * - FALSE          - processing of current event failed
 */
BOOLEAN SNMTS_SnStateFsm(BYTE_B_INSTNUM_ UINT32 dw_ct, t_SN_EVENT e_evtSn,
        const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
        UINT8 *pb_numFreeMngtFrms, BOOLEAN *po_busy)
{
    BOOLEAN o_return = FALSE; /* predefined return value */

    /* if checking of the parameters succeeded */
    if (checkParam(B_INSTNUM_ e_evtSn, ps_rxBuf, ps_txBuf))
    {
        /* process current event in Main FSM */
        switch (ae_ActFsmState[B_INSTNUMidx])
        {
        case k_ST_INITIALIZATION:
        {
            o_return = InitializationState(B_INSTNUM_ dw_ct, e_evtSn, ps_rxBuf, ps_txBuf);
            break;
        }
        case k_ST_WF_SADR_ASS_STEP1:
        {
            o_return = WfSadrAssStep1State(B_INSTNUM_ dw_ct, e_evtSn, ps_rxBuf,
                    ps_txBuf, pb_numFreeMngtFrms, po_busy);
            break;
        }
        case k_ST_WF_SADR_ASS_STEP2:
        {
        o_return = WfSadrAssStep2State(B_INSTNUM_ e_evtSn, ps_rxBuf, ps_txBuf,
                                       po_busy);
            break;
        }
        case k_ST_WF_UDID_SCM_ASS:
        {
            o_return = WfUdidScmAssState(B_INSTNUM_ e_evtSn, ps_rxBuf, ps_txBuf,
                    po_busy);
            break;
        }
        case k_ST_WF_SET_TO_OP1_STEP1:
        {
        o_return = WfSetToOp1Step1State(B_INSTNUM_ e_evtSn, ps_rxBuf, ps_txBuf,
                                        po_busy);
            break;
        }
      /* no SPDO active at all */
      #if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
        case k_ST_WF_SET_TO_OP1_STEP2:
        {
        o_return = WfSetToOp1Step2State(B_INSTNUM_ e_evtSn, ps_rxBuf, ps_txBuf,
                                        po_busy);
            break;
        }
      #endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */
        case k_ST_WF_API_CHKSUM:
        {
            o_return = WfApiChksumState(B_INSTNUM_ e_evtSn, ps_rxBuf, ps_txBuf,
                    po_busy);
            break;
        }
      case k_ST_WF_SET_TO_OP2:
      {
            o_return = WfSetToOp2State(B_INSTNUM_ e_evtSn, ps_rxBuf, ps_txBuf,
                    po_busy);
            break;
        }
      case k_ST_WF_API_CONF:
      {
            o_return = WfApiConfState(B_INSTNUM_ e_evtSn, ps_rxBuf, ps_txBuf,
                    po_busy);
            break;
        }
      case k_ST_WF_SET_TO_OP3:
      {
            o_return = WfSetToOp3State(B_INSTNUM_ dw_ct, e_evtSn, ps_rxBuf,
                    ps_txBuf, po_busy);
            break;
        }
      case k_ST_OPERATIONAL_STEP1:
      {
            o_return = OperationStep1State(B_INSTNUM_ dw_ct, e_evtSn, ps_rxBuf,
                    ps_txBuf, po_busy);
            break;
        }
      case k_ST_OPERATIONAL_STEP2:
      {
            o_return = OperationStep2State(B_INSTNUM_ dw_ct, e_evtSn, ps_rxBuf,
                    ps_txBuf, po_busy);
            break;
        }
      default:
      {
            /* error: invalid  */
            SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_SN_STATE_MAIN_INV,
            (UINT32)(ae_ActFsmState[B_INSTNUMidx]));
            break;
        }
    }
}
            /* no else : parameter error, FATAL error is already reported */

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function sets the current state of the Safety Node FSM. The function is only used for the unit tests.
 *
 * @param            b_instNum             instance number (not checked), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @retval              e_newFsmState         new state of the FSM, valid range: see {t_FSM_STATE}
 */
void SNMTS_SetFsmState(BYTE_B_INSTNUM_ t_FSM_STATE e_newFsmState)
{
    ae_ActFsmState[B_INSTNUMidx] = e_newFsmState;
}

/**
 * @brief This function returns the current state of the SN FSM. The function is only used for the unit tests.
 *
 * @param        b_instNum                 instance number (not checked), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return       current state of the SN FSM
 */
t_FSM_STATE SNMTS_GetFsmState(BYTE_B_INSTNUM)
{
    return ae_ActFsmState[B_INSTNUMidx];
}

/**
 * @brief This function processes the given event in the k_ST_INITIALIZATION state.
 *
 * @param        b_instNum         instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp()
 *       or SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct             consecutive time (not checked, any value allowed),
 *       valid range: k_CT_NOT_USED_INT if the calling event is not k_EVT_RX_SET_TO_OP or
 *       k_EVT_API_SET_TO_PREOP k_EVT_CHK_TIMER or k_EVT_RX_GUARDING, otherwise consecutive time (UINT32)
 *
 * @param        e_evtSn           event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure
 */
static BOOLEAN InitializationState(BYTE_B_INSTNUM_ UINT32 dw_ct,
        t_SN_EVENT e_evtSn, const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf)
{
    BOOLEAN o_return = FALSE; /* predefined return value */

    /* if calling event is "API forces transition into state PRE-OP" */
    if (e_evtSn == k_EVT_API_SET_TO_PREOP)
    {
        /* if refresh time start succeeded */
        if (SNMTS_RefreshTimeStart(B_INSTNUM_ dw_ct))
        {
            ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP1;
            o_return = TRUE;
        }
        /* no else : start of SCM Refresh Time failed, error already reported */
    }
    else /* unexpected or invalid event */
    {
        o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ (UINT32)(e_evtSn), SNMTS_k_ERR_EVENT_UNDEF1,ps_rxBuf,ps_txBuf);
    }

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function processes the given event in the k_ST_WF_SADR_ASS_STEP1 state.
 *
 * @param        b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct               consecutive time (not checked, any value allowed), valid range:
 *       k_CT_NOT_USED_INT if the calling event is not k_EVT_CHK_TIMER and k_EVT_RX_SET_TO_OP and
 *       k_EVT_RX_GUARDING k_EVT_API_SET_TO_PREOP, otherwise consecutive time (UINT32)
 *
 * @param        e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not checked,
 *       checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked, checked
 *       in checkParam()), valid range: see EPLS_t_FRM
 *
 * @retval       pb_numFreeMngtFrms
 * - *input*: max. number of openSAFETY frames of type SNMT and SSDO. This number of frames are free to transmit during one appl. cycle
 * - *output*: remaining number of free frames of type SNMT and SSDO. (not checked, checked in SNMTS_TimerCheck()), valid range:
 *       - <> NULL if the calling event is k_EVT_CHK_TIMER,
 *       - otherwise NULL (not used).
 *
 * @retval          po_busy             flag signals that SNMT Slave is currently processing a service request:
 * - TRUE:  Slave busy
 * - FALSE: Slave ready to process (not checked, only called with refernce to  variable), valid range : <> NULL
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure, or do not send response
 */
static BOOLEAN WfSadrAssStep1State(BYTE_B_INSTNUM_ UINT32 dw_ct,
                                   t_SN_EVENT e_evtSn,
                                   const EPLS_t_FRM *ps_rxBuf,
                                   EPLS_t_FRM *ps_txBuf,
                                   UINT8 *pb_numFreeMngtFrms, BOOLEAN *po_busy)
{
    BOOLEAN o_return = FALSE; /* predefined return value */
    BOOLEAN o_refreshTimeElapsed = FALSE; /* flag signals SCM Refresh Time has
     elapsed */

    *po_busy = FALSE;

    /* if calling event is "SADR received" */
    if (e_evtSn == k_EVT_RX_SADR)
    {
        /* if setting of the Main SADR failed */
        if (!SNMTS_MainSadrSet(B_INSTNUM_ ps_rxBuf->s_frmHdr.w_adr))
        {
            /* error: write access of main SADR failed, error already reported.
             NOTE: the SNMT Slave will respond with default value for
             SOD entry 0x1C00, 0x01 (main SADR) of 0x00 or will
             respond with the previous set main SADR */
        }
        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP2;
        *po_busy = TRUE;
        o_return = TRUE;
    }
    /* else if calling event is "check guarding and refresh timer" */
    else if (e_evtSn == k_EVT_CHK_TIMER)
    {
        /* if refresh time check succeeded */
        if (SNMTS_RefreshTimeCheck(B_INSTNUM_ dw_ct, &o_refreshTimeElapsed))
        {
            /* if refresh time elapsed ( timer is already restarted
             in SNMTS_RefreshTimeCheck() ) */
            if (o_refreshTimeElapsed)
            {
                /* if transmission successful */
                if (SNMTS_TxRespRstGuardScm(B_INSTNUM))
                {
                    /* decrement number of frames allowed to transmit */
                    (*pb_numFreeMngtFrms)--;
                    o_return = TRUE;
                }
                /* no else: transmission of SNMT Service "SN reset guarding SCM"
                 failed, error already reported */
            }
            else /* refresh time did not elapse */
            {
                o_return = TRUE;
            }
        }
        /* no else : error, refresh time check failed, error already reported */
    }
    /* else if calling event is "SNMT_Node_Guarding received" */
    else if (e_evtSn == k_EVT_RX_GUARDING)
    {
        /* collect header info and transmit response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_PREOP,
                               0U, 0U, (UINT8 *) NULL );
        o_return = TRUE;
    }
    /* else if calling event is "API forces transition into state PRE-OP" */
    else if (e_evtSn == k_EVT_API_SET_TO_PREOP)
    {
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_SN_set_to_PREOP received" */
    else if (e_evtSn == k_EVT_RX_SET_TO_PREOP)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT14, SERR_k_NO_ADD_INFO);
    }
    /* else if callig event is "SNMT_SN_Ack received" */
    else if(e_evtSn == k_EVT_SN_ACK)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT13, SERR_k_NO_ADD_INFO);
    }
    /* else if calling event is "SN set to op received" */
    else if(e_evtSn == k_EVT_RX_SET_TO_OP)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT1, SERR_k_NO_ADD_INFO);
    }
    /* else if calling event is "UDID of SCM assignment" */
    else if(e_evtSn == k_EVT_RX_UDID_SCM)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT15, SERR_k_NO_ADD_INFO);

        /* Send "UDID of the SCM assignment" Error Response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf,ps_txBuf,
                SNMTS_k_UDID_SCM_ASSGN, 0U, 0U, (UINT8 *)NULL);
        o_return = TRUE;
    }
#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
                /* else if calling event is "SNMTS Request Additional SADR assignm." */
                else if(e_evtSn == k_EVT_RX_SADR_ADD)
                {
                    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT2,SERR_k_NO_ADD_INFO);
  }
#endif
  else /* unexpected or invalid event */
  {
    o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ e_evtSn, SNMTS_k_ERR_EVENT_UNDEF2, ps_rxBuf, ps_txBuf);
  }

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function processes the given event in the k_ST_WF_SADR_ASS_STEP2 state.
 *
 * @param        b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not checked,
 *       checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked, checked
 *       in checkParam()), valid range: see EPLS_t_FRM
 *
 * @retval          po_busy             flag signals that SNMT Slave is currently processing a service request:
 * - TRUE:  Slave busy
 * - FALSE: Slave ready to process (not checked, only called with refernce to variable), valid range : <> NULL
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure
 *
 */
static BOOLEAN WfSadrAssStep2State(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                                   const EPLS_t_FRM *ps_rxBuf,
                                   EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy)
{
    BOOLEAN o_return = TRUE; /* predefined return value */

    *po_busy = FALSE;

    /* if calling event is "SADR received" */
    if (e_evtSn == k_EVT_RX_SADR)
    {
        /* if setting of the own SDN failed */
        if (!SDN_SetSdn(B_INSTNUM_ ps_rxBuf->s_frmHdr.w_sdn))
        {
            /* actual SDN is sent back */
        }

        /* response transmission */
        SNMTS_TxRespSadrAssigned(B_INSTNUM_ ps_rxBuf, ps_txBuf);
        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_UDID_SCM_ASS;
    }
    /* else if calling event is accepted */
    else if (e_evtSn == k_EVT_CHK_TIMER)
    {
        /* FSM is called by the SNMTS_TimerCheck(), return TRUE to signal that no
         timeout checking is needed */
    }
    /* else if calling event is "API forces transition into state PRE-OP" */
    else if (e_evtSn == k_EVT_API_SET_TO_PREOP)
    {
        /* nothing to do */
    }
    else /* unexpected event */
    {
      o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ e_evtSn, SNMTS_k_ERR_EVENT_UNDEF3, ps_rxBuf, ps_txBuf);
    }

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function processes the given event in the k_ST_WF_UDID_SCM_ASS state.
 *
 * @param              b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param              e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param              ps_rxBuf            reference to received openSAFETY frame to be distributed (not checked,
 *       checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @param              ps_txBuf            reference to openSAFETY frame to be transmitted (not checked, checked
 *       in checkParam()), valid range: see EPLS_t_FRM

 * @retval                po_busy             flag signals that SNMT Slave is currently processing a service request:
 *       - TRUE:  Slave busy
 *       - FALSE: Slave ready to process (not checked, only called with refernce to variable), valid range : <> NULL
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure, or do not send response
 */
static BOOLEAN WfUdidScmAssState(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                                 const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy)
{
    BOOLEAN o_return = FALSE; /* predefined return value */
    UINT8 ab_udidScm[EPLS_k_UDID_LEN]; /* buffer for the received SCM UDID */

    *po_busy = FALSE;

    /* if calling event is "UDID of SCM assignment" */
    if (e_evtSn == k_EVT_RX_UDID_SCM)
    {
        /* UDID of the SCM is copied from the received buffer */
        SFS_NET_CPY_DOMSTR(ab_udidScm, &(ps_rxBuf->ab_frmData[SNMT_k_OFS_UDID_SCM]),
                           EPLS_k_UDID_LEN);

        /* if the setting of the UDID of the SCM succeeded */
        if (SNMTS_UdidScmSet(B_INSTNUM_ ab_udidScm))
        {
            /* Send "UDID of the SCM assignment" Response */
            SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                    SNMTS_k_UDID_SCM_ASSGN, 0U, 0U, ab_udidScm);
            ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP1_STEP1;
        }
        /* else setting of the UDID of the SCM falied */
        else
        {
            /* Send "UDID of the SCM assignment" Error Response */
            SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                    SNMTS_k_UDID_SCM_ASSGN, 0U, 0U, (UINT8 *) NULL );
        }
        o_return = TRUE;
    }
    /* else if calling event is "SADR received" */
    else if (e_evtSn == k_EVT_RX_SADR)
    {
        /* if setting of the Main SADR failed */
        if (!SNMTS_MainSadrSet(B_INSTNUM_ ps_rxBuf->s_frmHdr.w_adr))
        {
            /* error: write access of main SADR failed, error already reported.
             NOTE: the SNMT Slave will respond with default value for
             SOD entry 0x1C00, 0x01 (main SADR) of 0x00 or will
             respond with the previous set main SADR */
        }
        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP2;
        *po_busy = TRUE;
        o_return = TRUE;
    }
    /* else if calling event is "check guarding and refresh timer" */
    else if (e_evtSn == k_EVT_CHK_TIMER)
    {
        /* FSM is called by the SNMTS_TimerCheck(), return TRUE to signal that no
         timeout checking is needed */
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_Node_Guarding received" */
    else if (e_evtSn == k_EVT_RX_GUARDING)
    {
        /* collect header info and transmit response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_PREOP,
                               0U, 0U, (UINT8 *) NULL );
        o_return = TRUE;
    }
    /* else if calling event is "API forces transition into state PRE-OP" */
    else if (e_evtSn == k_EVT_API_SET_TO_PREOP)
    {
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_SN_set_to_PREOP received" */
    else if (e_evtSn == k_EVT_RX_SET_TO_PREOP)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT16, SERR_k_NO_ADD_INFO);
    }
    /* else if callig event is "SNMT_SN_Ack received" */
    else if(e_evtSn == k_EVT_SN_ACK)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT17, SERR_k_NO_ADD_INFO);
    }
    /* else if calling event is "SNMT Service Request "SN set to op" received" */
    else if(e_evtSn == k_EVT_RX_SET_TO_OP)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT18, SERR_k_NO_ADD_INFO);
    }
#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
                /* else if calling event is "SNMTS Request Additional SADR assignm." */
                else if(e_evtSn == k_EVT_RX_SADR_ADD)
                {
                    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT19,SERR_k_NO_ADD_INFO);
  }
#endif
  else /* unexpected or invalid event */
  {
    o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ e_evtSn, SNMTS_k_ERR_EVENT_UNDEF12, ps_rxBuf, ps_txBuf);
  }

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function processes the given event in the k_ST_WF_SET_TO_OP1_STEP1 state.
 *
 * @param        b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not checked,
 *       checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked, checked
 *       in checkParam()), valid range: see EPLS_t_FRM
 *
 * @retval          po_busy             flag signals that SNMT Slave is currently processing a service request:
 *       - TRUE:  Slave busy
 *       - FALSE: Slave ready to process (not checked, only called with refernce to variable), valid range : <> NULL
 *
 * @return
 * - TRUE              - success
 * - FALSE             - failure
 */
static BOOLEAN WfSetToOp1Step1State(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                                    const EPLS_t_FRM *ps_rxBuf,
                                    EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy)
{
    BOOLEAN o_return = FALSE; /* predefined return value */

    *po_busy = FALSE;

    /* if calling event is "SN set to op received " */
    if (e_evtSn == k_EVT_RX_SET_TO_OP)
    {
        /* if the parameter set is already processed */
        if (SAPL_SNMTS_ParameterSetProcessed(B_INSTNUM))
        {
/* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
            /* activation of SPDO mapping is started */
            SPDO_ActivateStart(B_INSTNUM);

            /* collect header info and transmit response with status BUSY */
            SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_BUSY,
                    0U, 0U, (UINT8 *) NULL );

            ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP1_STEP2;
#else /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */
      /* no SPDOs to be activated - calculate the checksum */
      SAPL_SNMTS_CalcParamChkSumClbk(B_INSTNUM);
      /* collect header info and transmit response with status BUSY */
      SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_BUSY,
                             0U, 0U, (UINT8 *)NULL);
      ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_API_CHKSUM;
#endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */
        }
        else /* parameter set is not processed */
        {
            /* status Busy is sent */
            SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                    SNMTS_k_SN_STS_BUSY, 0U, 0U, (UINT8 *) NULL );
        }
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_Node_Guarding received" */
    else if (e_evtSn == k_EVT_RX_GUARDING)
    {
        /* collect header info and transmit response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_PREOP,
                               0U, 0U, (UINT8 *) NULL );
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_SN_set_to_PREOP received" */
    else if (e_evtSn == k_EVT_RX_SET_TO_PREOP)
    {
#if (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE)
        SAPL_SNMTS_SwitchPreOpPreOpClbk(B_INSTNUM);
#endif
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_PREOP,
                               0U, 0U, (UINT8 *) NULL );
        o_return = TRUE;
    }
#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
    /* else if calling event is "SNMTS Request Additional SADR assignm." */
    else if(e_evtSn == k_EVT_RX_SADR_ADD)
    {
        /* entereing of additional SADR */
        enterAddSadr(B_INSTNUM_ ps_rxBuf);

        /* collect header info and transmit response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                SNMTS_k_ADD_SADR_ASSGN, 0U, 0U, (UINT8 *)NULL);
        o_return = TRUE;
    }
#endif
    /* else if callig event is "SNMT_SN_Ack received" */
    else if (e_evtSn == k_EVT_SN_ACK)
    {
        SNMTS_SnAckRespCheck(B_INSTNUM_ ps_rxBuf);
    }
    /* else if calling event is "SADR received" */
    else if (e_evtSn == k_EVT_RX_SADR)
    {
        /* if setting of the Main SADR failed */
        if (!SNMTS_MainSadrSet(B_INSTNUM_ ps_rxBuf->s_frmHdr.w_adr))
        {
            /* error: write access of main SADR failed, error already reported.
             NOTE: the SNMT Slave will respond with default value for
             SOD entry 0x1C00, 0x01 (main SADR) of 0x00 or will
             respond with the previous set main SADR */
        }

        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP2;
        *po_busy = TRUE;
        o_return = TRUE;
    }
    /* else if calling event is accepted */
    else if (e_evtSn == k_EVT_CHK_TIMER)
    {
        /* FSM is called by the SNMTS_TimerCheck(), return TRUE to signal that no
         timeout checking is needed */
        o_return = TRUE;
    }
    /* else if calling event is "API forces transition into state PRE-OP" */
    else if (e_evtSn == k_EVT_API_SET_TO_PREOP)
    {
        o_return = TRUE;
    }
    /* else if calling event is "UDID of SCM assignment" */
    else if (e_evtSn == k_EVT_RX_UDID_SCM)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT20, SERR_k_NO_ADD_INFO);

        /* Send "UDID of the SCM assignment" Error Response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf,ps_txBuf,
                SNMTS_k_UDID_SCM_ASSGN, 0U, 0U, (UINT8 *)NULL);
        o_return = TRUE;
    }
#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
    /* else if calling event is extended CT initialization */
    else if(e_evtSn == k_EVT_RX_INIT_CT)
    {
        /* entering of additional SADR */
        enterExtCt(B_INSTNUM_ ps_rxBuf);

        /* collect header info and transmit response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,
            SNMTS_k_EXT_CT_INITIALIZE, 0U, 0U, (UINT8 *)NULL);
        o_return = TRUE;
    }
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */
    else /* unexpected or invalid event */
    {
      o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ e_evtSn, SNMTS_k_ERR_EVENT_UNDEF4, ps_rxBuf, ps_txBuf);
    }

    SCFM_TACK_PATH();
    return o_return;
}

/* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)

/**
 * @brief This function processes the given event in the k_ST_WF_SET_TO_OP1_STEP2 state.
 *
 * @param        b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not
 *       checked, checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked,
 *       checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @retval          po_busy             flag signals that SNMT Slave is currently processing a service request:
 * - TRUE:  Slave busy
 * - FALSE: Slave ready to process (not checked, only called with refernce to variable), valid range : <> NULL
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure
 */
static BOOLEAN WfSetToOp1Step2State(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                                    const EPLS_t_FRM *ps_rxBuf,
                                    EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy)
{
    BOOLEAN o_return = FALSE; /* predefined return value */
    BOOLEAN o_busySpdo = FALSE; /* busy flag for the SPDO mapping activation */

    *po_busy = FALSE;

    /* if calling event is "SN set to op received " */
    if (e_evtSn == k_EVT_RX_SET_TO_OP)
    {
        o_return = TRUE;

        /* if Tx SPDO mapping activation succeeded */
        if (SPDO_ActivateTxSpdoMapping(B_INSTNUM_ &o_busySpdo))
        {
            /* if not all Tx SPDO mapping was activated */
            if (o_busySpdo)
            {
                /* collect header info and transmit response with status BUSY */
                SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                        SNMTS_k_SN_STS_BUSY, 0U, 0U, (UINT8 *) NULL );
            }
            else /* all Tx SPDO mapping was activated */
            {
                /* if Rx SPDO mapping activation succeeded */
                if (SPDO_ActivateRxSpdoMapping(B_INSTNUM_ &o_busySpdo))
                {
                    /* if all Tx SPDO mapping was activated */
                    if (!o_busySpdo)
                    {
                        SAPL_SNMTS_CalcParamChkSumClbk(B_INSTNUM);

                        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_API_CHKSUM;
                    }

                    /* collect header info and transmit response with status BUSY */
                    SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                            SNMTS_k_SN_STS_BUSY, 0U, 0U, (UINT8 *) NULL );
                }
                else /* Rx SPDO mapping activation failed */
                {
                    /* collect header info and transmit reported error to SNMTM */
                    SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_FAIL,
                            SNMTS_k_FAIL_GRP_STK,
                            SNMTS_k_FAIL_ERR_MAP_FAILED_RX, (UINT8 *) NULL );

                    ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP1_STEP1;
                }
            }
        }
        else /* Tx SPDO mapping activation failed */
        {
            /* collect header info and transmit reported error to SNMTM */
            SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_FAIL,
                    SNMTS_k_FAIL_GRP_STK,
                    SNMTS_k_FAIL_ERR_MAP_FAILED_TX, (UINT8 *) NULL );

            ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP1_STEP1;
        }
    }
    /* else if calling event is "SNMT_Node_Guarding received" */
    else if (e_evtSn == k_EVT_RX_GUARDING)
    {
        /* collect header info and transmit response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_PREOP,
                               0U, 0U, (UINT8 *) NULL );
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_SN_set_to_PREOP received" */
    else if (e_evtSn == k_EVT_RX_SET_TO_PREOP)
    {
#if (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE)
        SAPL_SNMTS_SwitchPreOpPreOpClbk(B_INSTNUM);
#endif
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_PREOP,
                               0U, 0U, (UINT8 *) NULL );
        o_return = TRUE;
    }
#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
    /* else if calling event is "SNMTS Request Additional SADR assignm." */
    else if(e_evtSn == k_EVT_RX_SADR_ADD)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT27, SERR_k_NO_ADD_INFO);
    }
#endif
    /* else if callig event is "SNMT_SN_Ack received" */
    else if (e_evtSn == k_EVT_SN_ACK)
    {
        SNMTS_SnAckRespCheck(B_INSTNUM_ ps_rxBuf);
    }
    /* else if calling event is "SADR received" */
    else if (e_evtSn == k_EVT_RX_SADR)
    {
        /* if setting of the Main SADR failed */
        if (!SNMTS_MainSadrSet(B_INSTNUM_ ps_rxBuf->s_frmHdr.w_adr))
        {
            /* error: write access of main SADR failed, error already reported.
             NOTE: the SNMT Slave will respond with default value for
             SOD entry 0x1C00, 0x01 (main SADR) of 0x00 or will
             respond with the previous set main SADR */
        }

        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP2;
        *po_busy = TRUE;
        o_return = TRUE;
    }
    /* else if calling event is accepted */
    else if (e_evtSn == k_EVT_CHK_TIMER)
    {
        /* FSM is called by the SNMTS_TimerCheck(), return TRUE to signal that no
         timeout checking is needed */
        o_return = TRUE;
    }
    /* else if calling event is "UDID of SCM assignment" */
    else if (e_evtSn == k_EVT_RX_UDID_SCM)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT21, SERR_k_NO_ADD_INFO);

        /* Send "UDID of the SCM assignment" Error Response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf,ps_txBuf,
                SNMTS_k_UDID_SCM_ASSGN, 0U, 0U, (UINT8 *)NULL);
        o_return = TRUE;
    }
    /* else if calling event is "API forces transition into state PRE-OP" */
    else if (e_evtSn == k_EVT_API_SET_TO_PREOP)
    {
        o_return = TRUE;
    }
    else /* unexpected event */
    {
      o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ e_evtSn, SNMTS_k_ERR_EVENT_UNDEF5, ps_rxBuf, ps_txBuf);
    }

    SCFM_TACK_PATH();
    return o_return;
}
#endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */

/**
 * @brief This function processes the given event in the k_ST_WF_API_CHKSUM state.
 *
 * @param        b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1

 * @param        e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT

 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not
 *       checked, checked in checkParam()), valid range: see EPLS_t_FRM

 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked,
 *       checked in checkParam()), valid range: see EPLS_t_FRM

 * @retval          po_busy             flag signals that SNMT Slave is currently processing a service request:
 * - TRUE:  Slave busy
 * - FALSE: Slave ready to process (not checked, only called with refernce to variable), valid range : <> NULL
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure
 */
static BOOLEAN WfApiChksumState(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                                const EPLS_t_FRM *ps_rxBuf,
                                EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy)
{
    BOOLEAN o_return = FALSE; /* predefined return value */

    *po_busy = FALSE;

    /* if calling event is "SN set to op received " */
    if (e_evtSn == k_EVT_RX_SET_TO_OP)
    {
        /* collect header info and transmit response with status BUSY */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_BUSY,
                               0U, 0U, (UINT8 *) NULL );
        o_return = TRUE;
    }
    /* else if calling event is "checksum available" */
    else if (e_evtSn == k_EVT_CHKSUM_AVAIL)
    {
        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP2;
        o_return = TRUE;
    }
    /* else if calling event is "SADR received" */
    else if (e_evtSn == k_EVT_RX_SADR)
    {
        /* if setting of the Main SADR failed */
        if (!SNMTS_MainSadrSet(B_INSTNUM_ ps_rxBuf->s_frmHdr.w_adr))
        {
            /* error: write access of main SADR failed, error already reported.
             NOTE: the SNMT Slave will respond with default value for
             SOD entry 0x1C00, 0x01 (main SADR) of 0x00 or will
             respond with the previous set main SADR */
        }

        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP2;
        *po_busy = TRUE;
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_SN_set_to_PREOP received" */
    else if (e_evtSn == k_EVT_RX_SET_TO_PREOP)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT9, SERR_k_NO_ADD_INFO);
    }
    /* else if calling event is "SNMT_Node_Guarding received" */
    else if(e_evtSn == k_EVT_RX_GUARDING)
    {
        /* collect header info and transmit response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,SNMTS_k_SN_STS_PREOP,
                           0U, 0U, (UINT8 *)NULL);
    o_return = TRUE;
  }
  /* else if callig event is "SNMT_SN_Ack received" */
  else if(e_evtSn == k_EVT_SN_ACK)
  {
    SNMTS_SnAckRespCheck(B_INSTNUM_ ps_rxBuf);
  }
  /* else if calling event is accepted */
  else if (e_evtSn == k_EVT_CHK_TIMER)
  {
    /* FSM is called by the SNMTS_TimerCheck(), return TRUE to signal that no
       timeout checking is needed */
    o_return = TRUE;
  }
#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
  /* else if calling event is "SNMTS Request Additional SADR assignm." */
  else if(e_evtSn == k_EVT_RX_SADR_ADD)
  {
    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT3, SERR_k_NO_ADD_INFO);
  }
#endif
  /* else if calling event is "UDID of SCM assignment" */
  else if(e_evtSn == k_EVT_RX_UDID_SCM)
  {
    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT22, SERR_k_NO_ADD_INFO);

    /* Send "UDID of the SCM assignment" Error Response */
    SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf,ps_txBuf,
                           SNMTS_k_UDID_SCM_ASSGN, 0U, 0U, (UINT8 *)NULL);
    o_return = TRUE;
  }
  /* else if calling event is "API forces transition into state PRE-OP" */
  else if (e_evtSn == k_EVT_API_SET_TO_PREOP)
  {
    o_return = TRUE;
  }
  else /* unexpected or invalid event */
  {
    o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ e_evtSn, SNMTS_k_ERR_EVENT_UNDEF6, ps_rxBuf, ps_txBuf);
  }

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function processes the given event in the k_ST_WF_SET_TO_OP2 state.
 *
 * @param        b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not
 *       checked, checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked,
 *       checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @retval          po_busy             flag signals that SNMT Slave is currently processing a service request:
 * - TRUE:  Slave busy
 * - FALSE: Slave ready to process (not checked, only called with refernce to variable), valid range : <> NULL
 *
 * @return
 * - TRUE              - success
 * - FALSE             - failure
 */
static BOOLEAN WfSetToOp2State(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                               const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
                               BOOLEAN *po_busy)
{
    BOOLEAN o_return = FALSE; /* predefined return value */

    *po_busy = FALSE;

    /* if calling event is "SN set to op received " */
    if (e_evtSn == k_EVT_RX_SET_TO_OP)
    {
        /* if received and calculated parameter checksum differs */
        if (!(SNMTS_AppChkSumComp(B_INSTNUM_ ps_rxBuf)))
        {
            /* collect header info and transmit reported error to SNMTM */
            SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_FAIL,
                                   SNMTS_k_FAIL_GRP_STK, SNMTS_k_FAIL_ERR_CRC_CHKSUM,
                                   (UINT8 *) NULL );

            ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP1_STEP1;
        }
        else /* received parameter checksum is valid */
        {
            /* callback application for state transition confirmation */
            SAPL_SNMTS_SwitchToOpReqClbk(B_INSTNUM);

            /* collect header info and transmit response with status BUSY */
            SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_BUSY,
                                   0U, 0U, (UINT8 *) NULL );

            ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_API_CONF;
        }
        o_return = TRUE;
    }
    /* else if calling event is "SADR received" */
    else if (e_evtSn == k_EVT_RX_SADR)
    {
        /* if setting of the Main SADR failed */
        if (!SNMTS_MainSadrSet(B_INSTNUM_ ps_rxBuf->s_frmHdr.w_adr))
        {
            /* error: write access of main SADR failed, error already reported.
             NOTE: the SNMT Slave will respond with default value for
             SOD entry 0x1C00, 0x01 (main SADR) of 0x00 or will
             respond with the previous set main SADR */
        }

        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP2;
        *po_busy = TRUE;
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_Node_Guarding received" */
    else if (e_evtSn == k_EVT_RX_GUARDING)
    {
        /* collect header info and transmit response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_PREOP,
                               0U, 0U, (UINT8 *) NULL );
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_SN_set_to_PREOP received" */
    else if (e_evtSn == k_EVT_RX_SET_TO_PREOP)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT10, SERR_k_NO_ADD_INFO);
    }
    /* else if callig event is "SNMT_SN_Ack received" */
    else if(e_evtSn == k_EVT_SN_ACK)
    {
        SNMTS_SnAckRespCheck(B_INSTNUM_ ps_rxBuf);
    }
    /* else if calling event is accepted */
    else if (e_evtSn == k_EVT_CHK_TIMER)
    {
        /* FSM is called by the SNMTS_TimerCheck(), return TRUE to signal that no
         timeout checking is needed */
        o_return = TRUE;
    }
#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
                /* else if calling event is "SNMTS Request Additional SADR assignm." */
                else if(e_evtSn == k_EVT_RX_SADR_ADD)
                {
                    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT4,SERR_k_NO_ADD_INFO);
  }
#endif
  /* else if calling event is "UDID of SCM assignment" */
  else if(e_evtSn == k_EVT_RX_UDID_SCM)
  {
    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT23, SERR_k_NO_ADD_INFO);

    /* Send "UDID of the SCM assignment" Error Response */
    SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf,ps_txBuf,
                           SNMTS_k_UDID_SCM_ASSGN, 0U, 0U, (UINT8 *)NULL);
    o_return = TRUE;
  }
  /* else if calling event is "API forces transition into state PRE-OP" */
  else if (e_evtSn == k_EVT_API_SET_TO_PREOP)
  {
    o_return = TRUE;
  }
  else /* unexpected or invalid event */
  {
    o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ e_evtSn, SNMTS_k_ERR_EVENT_UNDEF7, ps_rxBuf, ps_txBuf);
  }

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function processes the given event in the k_ST_WF_API_CONF state.
 *
 * @param        b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not
 *       checked, checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked,
 *       checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @retval          po_busy             flag signals that SNMT Slave is currently processing a service request:
 * - TRUE:  Slave busy
 * - FALSE: Slave ready to process (not checked, only called with refernce to variable), valid range : <> NULL
 *
 * @return
 * - TRUE              - success
 * - FALSE             - failure
 */
static BOOLEAN WfApiConfState(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                              const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
                              BOOLEAN *po_busy)
{
    BOOLEAN o_return = FALSE; /* predefined return value */

    *po_busy = FALSE;

    /* if calling event is "SN set to op received " */
    if (e_evtSn == k_EVT_RX_SET_TO_OP)
    {
        /* collect header info and transmit response with status BUSY */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_BUSY,
                               0U, 0U, (UINT8 *) NULL );

        o_return = TRUE;
    }
    /* else if calling event is "API confirmation available" */
    else if (e_evtSn == k_EVT_CONF_AVAIL)
    {
        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP3;
        o_return = TRUE;
    }
    /* else if calling event is "SADR received" */
    else if (e_evtSn == k_EVT_RX_SADR)
    {
        /* if setting of the Main SADR failed */
        if (!SNMTS_MainSadrSet(B_INSTNUM_ ps_rxBuf->s_frmHdr.w_adr))
        {
            /* error: write access of main SADR failed, error already reported.
             NOTE: the SNMT Slave will respond with default value for
             SOD entry 0x1C00, 0x01 (main SADR) of 0x00 or will
             respond with the previous set main SADR */
        }

        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP2;
        *po_busy = TRUE;
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_Node_Guarding received" */
    else if (e_evtSn == k_EVT_RX_GUARDING)
    {
        /* collect header info and transmit response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_PREOP,
                               0U, 0U, (UINT8 *) NULL );
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_SN_set_to_PREOP received" */
    else if (e_evtSn == k_EVT_RX_SET_TO_PREOP)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT11, SERR_k_NO_ADD_INFO);
    }
    /* else if callig event is "SNMT_SN_Ack received" */
    else if(e_evtSn == k_EVT_SN_ACK)
    {
        SNMTS_SnAckRespCheck(B_INSTNUM_ ps_rxBuf);
    }
    /* else if calling event is accepted */
    else if (e_evtSn == k_EVT_CHK_TIMER)
    {
        /* FSM is called by the SNMTS_TimerCheck(), return TRUE to signal that no
         timeout checking is needed */
        o_return = TRUE;
    }
    /* else if calling event is "API forces transition into state PRE-OP" */
    else if (e_evtSn == k_EVT_API_SET_TO_PREOP)
    {
        o_return = TRUE;
    }
#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
                /* else if calling event is "SNMTS Request Additional SADR assignm." */
                else if(e_evtSn == k_EVT_RX_SADR_ADD)
                {
                    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT5,SERR_k_NO_ADD_INFO);
  }
#endif
  /* else if calling event is "UDID of SCM assignment" */
  else if(e_evtSn == k_EVT_RX_UDID_SCM)
  {
    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT24, SERR_k_NO_ADD_INFO);

    /* Send "UDID of the SCM assignment" Error Response */
    SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf,ps_txBuf,
                           SNMTS_k_UDID_SCM_ASSGN, 0U, 0U, (UINT8 *)NULL);
    o_return = TRUE;
  }
  else /* unexpected or invalid event */
  {
    o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ e_evtSn, SNMTS_k_ERR_EVENT_UNDEF8, ps_rxBuf, ps_txBuf);
  }

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function processes the given event in the k_ST_WF_SET_TO_OP3 state.
 *
 * @param        b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct               consecutive time (not checked, any value allowed), valid range:
 *       k_CT_NOT_USED_INT if the calling event is not k_EVT_CHK_TIMER and k_EVT_RX_SET_TO_OP and
 *       k_EVT_RX_GUARDING k_EVT_API_SET_TO_PREOP, otherwise consecutive time (UINT32)
 *
 * @param        e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not
 *       checked, checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked,
 *       checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @retval          po_busy             flag signals that SNMT Slave is currently processing a service request:
 * - TRUE:  Slave busy
 * - FALSE: Slave ready to process (not checked, only called with refernce to variable), valid range : <> NULL
 *
 * @return
 * - TRUE              - success
 * - FALSE             - failure
 */
static BOOLEAN WfSetToOp3State(BYTE_B_INSTNUM_ UINT32 dw_ct, t_SN_EVENT e_evtSn,
                               const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
                               BOOLEAN *po_busy)
{
    BOOLEAN o_return = FALSE; /* predefined return value */

    *po_busy = FALSE;

    /* if calling event is "SN set to op received " */
    if (e_evtSn == k_EVT_RX_SET_TO_OP)
    {
        /* check confirmation of SN API */
        /* if transition granted */
        if (SNMTS_AppConfRespGet(B_INSTNUM_ ps_rxBuf, ps_txBuf))
        {
            /* if start of life time succeeded */
            if (SNMTS_LifeTimeStart(B_INSTNUM_ dw_ct))
            {
                /* collect header info and transmit response with status OP */
                SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_OP,
                                       0U, 0U, (UINT8 *) NULL );

/* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
        SPDO_SetToOp(B_INSTNUM_ dw_ct);
#endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */

                ae_ActFsmState[B_INSTNUMidx] = k_ST_OPERATIONAL_STEP1;

                o_return = TRUE;
            }
            /* no else : start of SN Life Time failed, error already reported */
        }
        else /* transition into OP forbidden */
        {
            /* response with SN_FAIL status was sent in SNMTS_AppConfRespGet() */
            ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP1_STEP1;

            o_return = TRUE;
        }
    }
    /* else if calling event is "SADR received" */
    else if (e_evtSn == k_EVT_RX_SADR)
    {
        /* if setting of the Main SADR failed */
        if (!SNMTS_MainSadrSet(B_INSTNUM_ ps_rxBuf->s_frmHdr.w_adr))
        {
            /* error: write access of main SADR failed, error already reported.
             NOTE: the SNMT Slave will respond with default value for
             SOD entry 0x1C00, 0x01 (main SADR) of 0x00 or will
             respond with the previous set main SADR */
        }

        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP2;
        *po_busy = TRUE;
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_Node_Guarding received" */
    else if (e_evtSn == k_EVT_RX_GUARDING)
    {
        /* collect header info and transmit response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_PREOP,
                               0U, 0U, (UINT8 *) NULL );
        o_return = TRUE;
    }
    /* else if calling event is "SNMT_SN_set_to_PREOP received" */
    else if (e_evtSn == k_EVT_RX_SET_TO_PREOP)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT12, SERR_k_NO_ADD_INFO);
    }
    /* else if callig event is "SNMT_SN_Ack received" */
    else if(e_evtSn == k_EVT_SN_ACK)
    {
        SNMTS_SnAckRespCheck(B_INSTNUM_ ps_rxBuf);
    }
    /* else if calling event is accepted */
    else if (e_evtSn == k_EVT_CHK_TIMER)
    {
        /* FSM is called by the SNMTS_TimerCheck(), return TRUE to signal that no
         timeout checking is needed */
        o_return = TRUE;
    }
    /* else if calling event is "API forces transition into state PRE-OP" */
    else if (e_evtSn == k_EVT_API_SET_TO_PREOP)
    {
        o_return = TRUE;
    }
#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
                /* else if calling event is "SNMTS Request Additional SADR assignm." */
                else if(e_evtSn == k_EVT_RX_SADR_ADD)
                {
                    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT6,SERR_k_NO_ADD_INFO);
  }
#endif
  /* else if calling event is "UDID of SCM assignment" */
  else if(e_evtSn == k_EVT_RX_UDID_SCM)
  {
    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT25, SERR_k_NO_ADD_INFO);

    /* Send "UDID of the SCM assignment" Error Response */
    SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf,ps_txBuf,
                           SNMTS_k_UDID_SCM_ASSGN, 0U, 0U, (UINT8 *)NULL);
    o_return = TRUE;
  }
  else /* unexpected or invalid event */
  {
    o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ e_evtSn, SNMTS_k_ERR_EVENT_UNDEF9, ps_rxBuf, ps_txBuf);
  }

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function processes the given event in the k_ST_OPERATIONAL_STEP1 state.
 *
 * @param        b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct               consecutive time (not checked, any value allowed), valid range:
 *       k_CT_NOT_USED_INT if the calling event is not k_EVT_CHK_TIMER and  k_EVT_RX_SET_TO_OP and
 *       k_EVT_RX_GUARDING k_EVT_API_SET_TO_PREOP, otherwise consecutive time (UINT32)
 *
 * @param        e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not
 *       checked, checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked,
 *       checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @retval          po_busy             flag signals that SNMT Slave is currently processing a service request:
 * - TRUE:  Slave busy
 * - FALSE: Slave ready to process (not checked, only called with refernce to variable), valid range : <> NULL
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure
 */
STATIC BOOLEAN OperationStep1State(BYTE_B_INSTNUM_ UINT32 dw_ct,
                                   t_SN_EVENT e_evtSn,
                                   const EPLS_t_FRM *ps_rxBuf,
                                   EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy)
{
    BOOLEAN o_return = FALSE; /* predefined return value */
    BOOLEAN o_lifeTimeElapsed; /* flag signals SN Life Time has elapsed */
    UINT8 ab_udidScm[EPLS_k_UDID_LEN]; /* buffer for the received SCM UDID */
#if (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE)
    UINT8 b_errorGroup;
    UINT8 b_errorCode;
#endif /* (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE) */

    *po_busy = FALSE;

    /* if calling event is "Node guarding received" */
    if (e_evtSn == k_EVT_RX_GUARDING)
    {
        /* if start of life time succeeded */
        if (SNMTS_LifeTimeStart(B_INSTNUM_ dw_ct))
        {
            /* collect header info and transmit response */
            SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_OP,
                                   0U, 0U, (UINT8 *) NULL );
            o_return = TRUE;
        }
        /* no else : start of SN Life Time failed, error already reported and
         * if the life time timer is checked with event <k_EVT_CHK_TIMER>
         * then FATAL error is reported because the timer was not
         * started. */
    }
    /* else if calling event is "SN set to preop" */
    else if (e_evtSn == k_EVT_RX_SET_TO_PREOP)
    {
#if (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE)
        SAPL_SNMTS_SwitchOpPreOpClbk(B_INSTNUM);
#endif
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_PREOP,
                               0U, 0U, (UINT8 *) NULL );

/* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
        SPDO_SetToPreOp(B_INSTNUM);
#endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */

        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP1_STEP1;

        o_return = TRUE;
    }
    /* else if calling event is "check guarding and refresh timer" */
    else if (e_evtSn == k_EVT_CHK_TIMER)
    {
        /* if life time check succeeded */
        if (SNMTS_LifeTimeCheck(B_INSTNUM_ dw_ct, &o_lifeTimeElapsed))
        {
            /* if life time elapsed */
            if (o_lifeTimeElapsed)
            {
                /* if refresh time start succeeded */
                if (SNMTS_RefreshTimeStart(B_INSTNUM_ dw_ct))
                {

#if (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE)
                    (void) SAPL_SNMTS_NodeguardingExpiredClbk(B_INSTNUM);
                    SAPL_SNMTS_SwitchOpPreOpClbk(B_INSTNUM);
#endif
/* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
                  SPDO_SetToPreOp(B_INSTNUM);
#endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */
                  ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP1_STEP1;
                  SNMTS_LifeTimeStop(B_INSTNUM);
                  o_return = TRUE;
                }
                /* no else start of SCM Refresh Time failed, error already reported */
            }
            else /* life time did not elapse */
            {
                o_return = TRUE;
            }
       }
       /* no else : life time check failed, error already reported */
    }
    /* else if calling event is "SADR received" */
    else if (e_evtSn == k_EVT_RX_SADR)
    {
        /* the actual SADR and SDN is sent back */
        ae_ActFsmState[B_INSTNUMidx] = k_ST_OPERATIONAL_STEP2;
        *po_busy = TRUE;
        o_return = TRUE;
    }
    /* else if calling event is "UDID of SCM assignment" */
    else if (e_evtSn == k_EVT_RX_UDID_SCM)
    {
        /* UDID of the SCM is copied from the received buffer */
        SFS_NET_CPY_DOMSTR(ab_udidScm, &(ps_rxBuf->ab_frmData[SNMT_k_OFS_UDID_SCM]),
                           EPLS_k_UDID_LEN);

        /* if the setting of the UDID of the SCM succeeded */
        if (SNMTS_UdidScmSet(B_INSTNUM_ ab_udidScm))
        {
            /* Send "UDID of the SCM assignment" Response */
            SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                   SNMTS_k_UDID_SCM_ASSGN, 0U, 0U, ab_udidScm);
        }
        /* else setting of the UDID of the SCM falied */
        else
        {
            /* Send "UDID of the SCM assignment" Error Response */
            SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                   SNMTS_k_UDID_SCM_ASSGN, 0U, 0U, (UINT8 *) NULL );
            ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP1;
        }
        o_return = TRUE;
    }
    /* else if calling event is "API forces transition into state PRE-OP */
    else if (e_evtSn == k_EVT_API_SET_TO_PREOP)
    {
        /* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
        SPDO_SetToPreOp(B_INSTNUM);
#endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */
        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP1_STEP1;
        o_return = TRUE;
    }
    /* else if calling event is "SN set to op received" */
    else if (e_evtSn == k_EVT_RX_SET_TO_OP)
    {
#if (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE)
        switch(SAPL_SNMTS_SwitchOpOpClbk(B_INSTNUM_ &b_errorGroup, &b_errorCode))
        {
            case SNMTS_k_FAIL:
                /* collect header info and transmit reported error to SNMTM */
                SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_FAIL,
                                       b_errorGroup, b_errorCode, (UINT8 *) NULL );

                /* check if not only header information is to be sent */
                if ((SNMTS_k_FAIL_GRP_ADD != b_errorGroup) ||
                    (SNMTS_k_FAIL_ERR_ADD_HEAD_MASK == (SNMTS_k_FAIL_ERR_ADD_HEAD_MASK & b_errorCode)))
                {
                    /* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
                    SPDO_SetToPreOp(B_INSTNUM);
#endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */
                    ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SET_TO_OP1_STEP1;

                    o_return = TRUE;
                 }
            break;

            case SNMTS_k_BUSY:
                /* collect header info and transmit response with status OP */
                SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_BUSY,
                                       0U, 0U, (UINT8 *) NULL );
            break;

            case SNMTS_k_OP:
#endif /* (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE) */
                /* collect header info and transmit response with status OP */
                SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_STS_OP,
                                       0U, 0U, (UINT8 *) NULL );
#if (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE)
            break;

            default:
                SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_INV_RET, SERR_k_NO_ADD_INFO);
            break;
        }
        /* collect header info and transmit response with status OP */
#endif /* (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE) */

        o_return = TRUE;
    }
#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
    /* else if calling event is "SNMTS Request Additional SADR assignm." */
    else if(e_evtSn == k_EVT_RX_SADR_ADD)
    {
      SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT7, SERR_k_NO_ADD_INFO);
    }
#endif
    /* else if calling event is "SNMT_SN_Ack received" */
    else if (e_evtSn == k_EVT_SN_ACK)
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EVENT8, SERR_k_NO_ADD_INFO);
    }
#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
    /* else if calling event is extended CT initialization */
    else if(e_evtSn == k_EVT_RX_INIT_CT)
    {
        /* entering of additional SADR */
        enterExtCt(B_INSTNUM_ ps_rxBuf);

        /* collect header info and transmit response */
        SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                               SNMTS_k_EXT_CT_INITIALIZE, 0U, 0U, (UINT8 *)NULL);
        o_return = TRUE;
    }
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */
    else /* unexpected or invalid event */
    {
      o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ e_evtSn, SNMTS_k_ERR_EVENT_UNDEF10, ps_rxBuf, ps_txBuf);
    }

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function processes the given event in the k_ST_OPERATIONAL_STEP2 state.
 *
 * @param        b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct               consecutive time (not checked, any value allowed), valid range:
 *       k_CT_NOT_USED_INT if the calling event is not k_EVT_CHK_TIMER and  k_EVT_RX_SET_TO_OP and
 *       k_EVT_RX_GUARDING k_EVT_API_SET_TO_PREOP, otherwise consecutive time (UINT32)
 *
 * @param        e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not
 *       checked, checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked,
 *       checked in checkParam()), valid range: see EPLS_t_FRM
 *
 * @retval          po_busy             flag signals that SNMT Slave is currently processing a service request:
 * - TRUE:  Slave busy
 * - FALSE: Slave ready to process (not checked, only called with refernce to variable), valid range : <> NULL
 *
 * @return
 * - TRUE                success
 * - FALSE          - failure
 */
static BOOLEAN OperationStep2State(BYTE_B_INSTNUM_ UINT32 dw_ct,
                                   t_SN_EVENT e_evtSn,
                                   const EPLS_t_FRM *ps_rxBuf,
                                   EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy)
{
    BOOLEAN o_return = FALSE; /* predefined return value */
    BOOLEAN o_lifeTimeElapsed; /* flag signals SN Life Time has elapsed */

    *po_busy = FALSE;

    /* if calling event is "SADR received" */
    if (e_evtSn == k_EVT_RX_SADR)
    {
        /* response transmission */
        SNMTS_TxRespSadrAssigned(B_INSTNUM_ ps_rxBuf, ps_txBuf);
        ae_ActFsmState[B_INSTNUMidx] = k_ST_OPERATIONAL_STEP1;
        o_return = TRUE;
    }
    /* else if calling event is "check guarding and refresh timer" */
    else if (e_evtSn == k_EVT_CHK_TIMER)
    {
        /* if life time check succeeded */
        if (SNMTS_LifeTimeCheck(B_INSTNUM_ dw_ct, &o_lifeTimeElapsed))
        {
            /* if life time elapsed */
            if (o_lifeTimeElapsed)
            {
                /* if refresh time start succeeded */
                if (SNMTS_RefreshTimeStart(B_INSTNUM_ dw_ct))
                {
#if (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE)
                    (void) SAPL_SNMTS_NodeguardingExpiredClbk(B_INSTNUM);
                    SAPL_SNMTS_SwitchOpPreOpClbk(B_INSTNUM);
#endif
/* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
          SPDO_SetToPreOp(B_INSTNUM);
#endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */
                    ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP2;
                    SNMTS_LifeTimeStop(B_INSTNUM);
                    o_return = TRUE;
                }
                /* no else start of SCM Refresh Time failed, error already reported */
            }
            else /* life time did not elapse */
            {
                o_return = TRUE;
            }
        }
        /* no else : life time check failed, error already reported */
    }
    /* else if calling event is "API forces transition into state PRE-OP */
    else if (e_evtSn == k_EVT_API_SET_TO_PREOP)
    {
/* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
    SPDO_SetToPreOp(B_INSTNUM);
#endif /* (0 != SPDO_cfg_MAX_NO_TX_SPDO) */
        ae_ActFsmState[B_INSTNUMidx] = k_ST_WF_SADR_ASS_STEP2;
        o_return = TRUE;
    }
    else /* unexpected or invalid event */
    {
      o_return = HandleUnexpectedFsmEvent(B_INSTNUM_ e_evtSn, SNMTS_k_ERR_EVENT_UNDEF11, ps_rxBuf, ps_txBuf);
    }

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function checks the parameters of the SNMTS_SnStateFsm().
 *
 * @param        b_instNum           instance number (not checked, checked in SNMTS_TimerCheck() or
 *       SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *       SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        e_evtSn             event to be processed (checked), valid range: see t_SN_EVENT
 *
 * @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (checked), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf            reference to openSAFETY frame to be transmitted (checked), valid range: see EPLS_t_FRM
 *
 * @return
 * - TRUE              - success
 * - FALSE             - failure, FATAL error is already reported
 */
static BOOLEAN checkParam(BYTE_B_INSTNUM_ t_SN_EVENT e_evtSn,
                          const EPLS_t_FRM *ps_rxBuf,
                          const EPLS_t_FRM *ps_txBuf)
{
    BOOLEAN o_return = FALSE; /* predefined return value */

    /* if pointer to the rx and tx buffer is not NULL */
    if ((ps_rxBuf != NULL )&& (ps_txBuf != NULL))
    {
        o_return = TRUE;
    }
    else
    {
        /* if the calling event is k_EVT_CHK_TIMER or k_EVT_CHKSUM_AVAIL or
         k_EVT_CONF_AVAIL or k_EVT_API_SET_TO_PREOP */
        if ((e_evtSn == k_EVT_CHK_TIMER) || (e_evtSn == k_EVT_CHKSUM_AVAIL) ||
            (e_evtSn == k_EVT_CONF_AVAIL) || (e_evtSn == k_EVT_API_SET_TO_PREOP))
        {
            o_return = TRUE;
        }
        else /* other event */
        {
            SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_RX_TX_BUFF_NULL, (UINT32)(e_evtSn));
        }
    }

    SCFM_TACK_PATH();
    return o_return;
}

#if(k_NUM_OF_ADD_TX_SPDO != k_NO_ADD_TX_SPDO)
/**
 * @brief This function enters an additional SADR into the SOD.
 *
 * This additional address is assigned to the received TxSPDO number.
 *
 * @param        b_instNum        instance number (not checked, checked in SNMTS_TimerCheck() or
 *     SSC_ProcessSNMTSSDOFrame() or SNMTS_PassParamChkSumValid() or SNMTS_PerformTransPreOp() or
 *     SNMTS_EnterOpState()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 * @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (not
 *     checked, checked in checkParam()), valid range: see EPLS_t_FRM
 */
static void enterAddSadr(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf)
{
    UINT16 w_idxAddSadr = 0U; /* index of additional SADR in list */
    UINT16 w_txSpdoNo = 0U; /* tx SPDO number */
    UINT16 w_addSadr = 0U; /* additional SADR */
    SOD_t_ERROR_RESULT s_errRes; /* SOD access error */
    SOD_t_ACS_OBJECT_VIRT s_acsSod; /* structure to enter the SOD */

    /* get the add. SADR from the openSAFETY frame */
    SFS_NET_CPY16(&w_addSadr, &ps_rxBuf->ab_frmData[SNMT_k_OFS_SADR]);
    /* get the Tx SPDO number from the openSAFETY frame */
    SFS_NET_CPY16(&w_txSpdoNo, &ps_rxBuf->ab_frmData[SNMT_k_OFS_TXSPDO]);

    /* if received TxSPDO number is NOT in valid range */
    if((w_txSpdoNo < k_MIN_ADD_SPDO_NUM) || (w_txSpdoNo > k_MAX_ADD_SPDO_NUM))
    {
        /* error: TxSPDO number is incorrect, no response transmitted,
         return with predefined FALSE */
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_TXSPDO_NUM_INV, (UINT32)w_txSpdoNo);

        /* zero instead of received additional SADR */
        w_addSadr = 0x0000U;
    }
    else /* received TxSPDO number is valid */
    {
        /* calculate index of the SOD entry that stores the received add. SADR */
        w_idxAddSadr = (UINT16)(EPLS_k_IDX_TXSPDO_COM_PARAM + (w_txSpdoNo - 1U));

        s_acsSod.s_obj.w_index = w_idxAddSadr;
        s_acsSod.s_obj.b_subIndex = EPLS_k_SUBIDX_TXSPDO_SADR;
        s_acsSod.dw_segOfs = 0;
        s_acsSod.dw_segSize = 0;
        /* get handle to object "Common Communication Parameters,
         "additional SADR" to enter additional SADR.
         NOTE: TxSPDO number is connected with the index of object
         TxSPDO-No.: 1 --> object index 0x1C00
         TxSPDO-No.: 2 --> object index 0x1C01 */
        /* if handle to object of calculated index is NOT available */
        if(SOD_AttrGetVirt(B_INSTNUM_ &s_acsSod, &s_errRes) == NULL)
        {
            /* TxSPDO Number cannot be entered into SOD */
            /* zero instead of entered additional SADR */
            w_addSadr = 0x0000U;

            /* if search error happened */
            if ((s_errRes.e_abortCode == SOD_ABT_OBJ_DOES_NOT_EXIST) ||
                    (s_errRes.e_abortCode == SOD_ABT_SUB_IDX_DOES_NOT_EXIST))
            {
                SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_SPDO_NUM, (UINT32)w_txSpdoNo);
            }
            else /* no search error */
            {
                SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                        (UINT32)(s_errRes.e_abortCode));
            }
        }
        else /* TxSPDO Number can be used */
        {
            /* if additional SADR cannot be assigned */
            if(!(SOD_WriteVirt(B_INSTNUM_ &s_acsSod, &w_addSadr,
                                    SOD_k_NO_OVERWRITE)))
            {
                /* error: additional SADR could NOT be entered into SOD,
                 error already reported */
                /* zero instead of received additional SADR */
                w_addSadr = 0x0000U;
            }
            /* no else : additional SADR was entered into SOD */
        }
    }

    /* The additional SADR set in the rx buffer.
     ( The SNMTS_TxRespExtService() copied the Tx SPDO number and the
     additional SADR from the rx to the tx buffer to generate the
     response. ) */
    SFS_NET_CPY16(&ps_rxBuf->ab_frmData[SNMT_k_OFS_SADR], &w_addSadr);

    SCFM_TACK_PATH();
    return;
}
#endif


/** @} */
