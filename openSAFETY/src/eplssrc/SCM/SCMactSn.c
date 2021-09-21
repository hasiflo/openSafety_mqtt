/**
 * @addtogroup SCM
 * @{
 * @file SCMactSn.c
 *
 * FSM state functions for the process "Activate SN".
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
 * <h2>History for SCMactSn.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>12.06.2009</td><td>Hans Pill</td><td>Report status to MFW before reporting SN_Fail</td></tr>
 *     <tr><td>16.07.2009</td><td>Hans Pill</td><td>Review SL V11</td></tr>
 *     <tr><td>17.01.2013</td><td>Hans Pill</td><td>changes for implementation of additional parameters according to the openSafety specification</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>22.02.2018</td><td>Stefan Innerhofer</td><td>in SCM_WfSaplAck() set the state to SCM_k_SEND_ASSIGN_SADR_REQ instead of SCM_k_SEND_READ_TIMESTAMP</td></tr>
 *     <tr><td>05.09.2018</td><td>Stefan Innerhofer</td><td>add handling of received SN Fails, as a reaction to an unexpected event in the SNMTS FSM, to the states SCM_k_WF_OP_RESP</td></tr>
 * </table>
 *
 * @addtogroup fsm_activate_sn FSM - Activate SN
 * @{
 *
 */

#include "SCMint.h"
#include "SNMTSapi.h"

/**
* @brief This function represents the state SCM_k_SEND_PUT_TO_OP of the finite state machine ScmFsmProcess().
*
* @param ps_fsmCb   Pointer to the current slot of the FSM control block (not checked,only called with reference to struct in ScmFsmProcess()).
*
* @param w_snNum    FSM slot number (=> index to the DVI list) (not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param dw_ct      Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE  	Abort forcing error
*/
BOOLEAN SCM_SendPutToOp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;                 /* function result */
  UINT32 *pdw_timeStmp = (UINT32 *)NULL; /* pointer to the time stamp */

  /* if generic event set */
  if(ps_fsmCb->w_event == SCM_k_EVT_GENERIC_EVENT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* read the SOD timestamp */
    pdw_timeStmp = (UINT32 *)SCM_SodRead(SCM_k_IDX_TIMESTAMP(w_snNum));
    if(pdw_timeStmp != NULL)
    {
      o_res = SNMTM_ReqSnTrans(dw_ct, w_snNum, ps_fsmCb->w_sadr,
                               SNMTM_k_PREOP_TO_OP, *pdw_timeStmp);
      /* if no error happened */
      if (o_res)
      {
        ps_fsmCb->e_state = SCM_k_WF_OP_RESP;
        SCM_NumFreeFrmsDec();
      }
      /* no else : error happened */
    }
  }
  else  /* other events ignored */
  {
    /* error is generated in ScmFsmProcess() */
    o_res = TRUE;
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}

/**
* @brief This function represents the state SCM_k_SEND_PUT_TO_PREOP of the finite
* state machine ScmFsmProcess().
*
* @param     ps_fsmCb           Pointer to the current slot of the FSM control block
* 	(not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param     w_snNum            FSM slot number (=> index to the DVI list)(not checked,
* 	checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param     dw_ct              Consecutive time (not checked, any value allowed).
* 	Valid range: UINT32
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_SendReadTimestamp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  SSDOC_t_REQ s_transInfo;             /* used for SSDOC_ReadWriteReq() */
  BOOLEAN o_res = FALSE;               /* function result */

  /* if generic event set */
  if(ps_fsmCb->w_event == SCM_k_EVT_GENERIC_EVENT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    /* read request for the timestamp */
    s_transInfo.w_idx = EPLS_k_IDX_DEVICE_VEN_ID;
    s_transInfo.b_subIdx = EPLS_k_SUBIDX_PARAM_TS;
    s_transInfo.e_dataType = EPLS_k_UINT32;
    s_transInfo.b_payloadLen = (UINT8) ps_fsmCb->w_payLd;
    s_transInfo.pb_data = (UINT8 *)ps_fsmCb->adw_respBuff;
    s_transInfo.dw_dataLen = SCM_k_RESP_BUFF_SIZE;
    o_res = SSDOC_SendReadReq(ps_fsmCb->w_sadr, w_snNum, SCM_SsdocClbk,
                              dw_ct, &s_transInfo);

    /* if no error happened */
    if (o_res)
    {
      ps_fsmCb->e_state = SCM_k_WF_TIMESTAMP;
      SCM_NumFreeFrmsDec();
    }
  }
  else  /* other events ignored */
  {
    /* error is generated in ScmFsmProcess() */
    o_res = TRUE;
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}

/**
* @brief This function represents the state SCM_k_WF_OP_RESP of the finite state machine
* ScmFsmProcess().
*
* @param        ps_fsmCb        Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param w_snNum FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param dw_ct Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfOpResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE; /* function result */
  UINT8 b_snStatus;      /* deserialized received SN status */
  UINT8 b_errGroup;      /* deserialized error group */
  UINT8 b_errCode;       /* deserialized error code */

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_RESP_RX)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    /* deserialize the received SN status */
    SFS_NET_CPY8(&b_snStatus, ADD_OFFSET(ps_fsmCb->adw_respBuff, 0));
    /* if reveived node status == OPERATIONAL */
    if(b_snStatus == (UINT8)SNMTS_k_SN_STS_OP)
    {
      o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_OK,
                                ps_fsmCb->o_reportSnSts);

      /* if no error happened */
      if (o_res)
      {
        /* set the timer to the guard frequency */
        ps_fsmCb->dw_timer = dw_ct + SCM_dw_SnGuardTime;
        ps_fsmCb->e_state = SCM_k_WF_GUARD_TIMER;
      }
      /* no else : error happened */
    }
    else if(b_snStatus == (UINT8)SNMTS_k_SN_FAIL)
    {
      /* deserialize the received error group */
      SFS_NET_CPY8(&b_errGroup, ADD_OFFSET(ps_fsmCb->adw_respBuff, 1));
      /* deserialize the received error code */
      SFS_NET_CPY8(&b_errCode, ADD_OFFSET(ps_fsmCb->adw_respBuff, 2));
      /* handle unexpected FSM event in the SNMT state machine */
      if((SNMTS_k_FAIL_GRP_STK == b_errGroup) &&
         (SNMTS_k_FAIL_ERR_UNEXPECTED_FSM_EVENT == b_errCode))
      {
        /* error is to be acknowledged automatically, but has to be signaled to the application */
        ps_fsmCb->e_state = SCM_k_WF_SAPL_ACK;
        ps_fsmCb->b_errGroup = b_errGroup;
        ps_fsmCb->b_errCode = b_errCode;
        ps_fsmCb->w_event = SCM_k_EVT_ACK_RECEIVED;

        /*signal SN fail to application*/
        SAPL_ScmSnFailClbk(ps_fsmCb->w_sadr, b_errGroup, b_errCode, w_snNum);
      }
      /* handle additional parameters */
      else if(SNMTS_k_FAIL_GRP_ADD == b_errGroup)
      {
        /* error is to be acknowledged automatically */
        ps_fsmCb->e_state = SCM_k_SEND_SAPL_ACK;
        ps_fsmCb->b_errGroup = b_errGroup;
        ps_fsmCb->b_errCode = b_errCode;
        ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      }
      else
      {
        /* all other errors have to be acknowledged by the application */
        ps_fsmCb->e_state = SCM_k_WF_SAPL_ACK;
        /* Hans Pill: report status again to MFW to ensure that the status is set */
        /* inform application about the status change */
        SAPL_ScmNodeStatusChangedClbk(ps_fsmCb->w_sadr, ps_fsmCb->b_snStatus,
                                    ps_fsmCb->b_snStatus);
        SAPL_ScmSnFailClbk(ps_fsmCb->w_sadr, b_errGroup, b_errCode, w_snNum);
        /* Hans Pill: set the timer to the poll frequency to call with timer event */
        ps_fsmCb->dw_timer = dw_ct + ps_fsmCb->dw_pollInterval;
      }
      o_res = TRUE;
    }
    else if(b_snStatus == (UINT8)SNMTS_k_SN_STS_BUSY)
    {
      /* set the timer to the poll frequency */
      ps_fsmCb->dw_timer = dw_ct + ps_fsmCb->dw_pollInterval;
      ps_fsmCb->e_state = SCM_k_WF_POLL_TIMEOUT;
      o_res = TRUE;
    }
    else /* invalid response is checked by the SNMTM */
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_INVALID_SNMT_STATE,
                    (UINT32)(b_snStatus));
    }
  }
  /* else if SNMTM timeout occurred */
  else if (ps_fsmCb->w_event == SCM_k_EVT_SNMTM_TIMEOUT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* set node state to MISSING and restart FSM */
    o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_MISSING, FALSE);

    /* if no error happened */
    if (o_res)
    {
      ps_fsmCb->e_state = SCM_k_SEND_ASSIGN_SADR_REQ;
      ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
    }
  }
  else  /* other events ignored */
  {
    /* error is generated in ScmFsmProcess() */
    o_res = TRUE;
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}

/**
* @brief This function represents the state SCM_k_WF_POLL_TIMEOUT of the finite state
* machine ScmFsmProcess().
*
* @param        ps_fsmCb        Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param        w_snNum         FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param        dw_ct           Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfPollTimeout(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  dw_ct = dw_ct;        /* to avoid warnings */
  w_snNum = w_snNum;
  /* if poll timer expired */
  if(ps_fsmCb->w_event == SCM_k_EVT_TIMEOUT)
  {
    /* switch to k_SEND_PUT_TO_OP */
    ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
    ps_fsmCb->e_state = SCM_k_SEND_PUT_TO_OP;
  }
  /* no else : error is generated in ScmFsmProcess() */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return TRUE;
}

/**
* @brief This function represents the state SCM_k_WF_SAPL_ACK of the finite state machine ScmFsmProcess().
*
* @param        ps_fsmCb        Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param        w_snNum         FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param        dw_ct           Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfSaplAck(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;        /* function result */
  UINT8 b_errGroup;      /* deserialized error group */
  UINT8 b_errCode;       /* deserialized error code */

  /* unknown event */
  if ( EPLS_IS_BIT_SET(ps_fsmCb->w_event,~(SCM_k_EVT_ACK_RECEIVED | SCM_k_EVT_TIMEOUT)))
  {
    /* error is generated in ScmFsmProcess() */
    o_res = TRUE;
  }
  /* if acknowledge received */
  else if ( EPLS_IS_BIT_SET(ps_fsmCb->w_event,SCM_k_EVT_ACK_RECEIVED))
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* the response buffer is set by the application in host format */
    o_res = SNMTM_SnErrorAck(ps_fsmCb->w_sadr,
                             *ADD_OFFSET(ps_fsmCb->adw_respBuff, 0),
                             *ADD_OFFSET(ps_fsmCb->adw_respBuff, 1));

    /* if no error happened */
    if (o_res)
    {
      ps_fsmCb->e_state = SCM_k_SEND_ASSIGN_SADR_REQ;
      ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      SCM_NumFreeFrmsDec();
    }
    /* no else : error happened */
  }
  else  /* other events ignored */
  {
    /* Hans Pill: report status including SN failure again */
    if ( ps_fsmCb->o_reportSnSts)
    {
        /* inform application about the status change */
        SAPL_ScmNodeStatusChangedClbk(ps_fsmCb->w_sadr, ps_fsmCb->b_snStatus,
                                      ps_fsmCb->b_snStatus);
        /* deserialize the received error group */
        SFS_NET_CPY8(&b_errGroup, ADD_OFFSET(ps_fsmCb->adw_respBuff, 1));
        /* deserialize the received error code */
        SFS_NET_CPY8(&b_errCode, ADD_OFFSET(ps_fsmCb->adw_respBuff, 2));
        SAPL_ScmSnFailClbk(ps_fsmCb->w_sadr, b_errGroup, b_errCode, w_snNum);

        ps_fsmCb->o_reportSnSts = FALSE;
    }
    /* Hans Pill: set the timer to the poll frequency to call with timer event */
    ps_fsmCb->dw_timer = dw_ct + ps_fsmCb->dw_pollInterval;
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    o_res = TRUE;
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}

/** @} */
/** @} */
