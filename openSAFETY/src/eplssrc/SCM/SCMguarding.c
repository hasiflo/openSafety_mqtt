/**
 * @addtogroup SCM
 * @{
 * @file SCMguarding.c
 *
 * FSM state functions for the process "Node Guarding".
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author Joachim Stolberg, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * @addtogroup fsm_node_guarding FSM - Node Guarding
 * @{
 *
 * <h2>History for SCMguarding.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>19.04.2017</td><td>Roman Zwischelsberger</td><td>changes for better handling of SNMT timeouts</td></tr>
 * </table>
 */


#include "SCMint.h"


/**
* @brief This function represents the state SCM_k_WF_GUARD_TIMER of the finite state machine ScmFsmProcess().
*
* @param ps_fsmCb               Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param w_snNum                FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param dw_ct                  Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfGuardTimer(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res;        /* function result */

  /* if guard timer expired */
  if(ps_fsmCb->w_event == SCM_k_EVT_TIMEOUT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    o_res = SNMTM_ReqGuarding(dw_ct, w_snNum, ps_fsmCb->w_sadr);

    /* if no error happened */
    if (o_res)
    {
      ps_fsmCb->e_state = SCM_k_WF_GUARD_RESP;
      SCM_NumFreeFrmsDec();
    }
    /* no else : error happened */
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
* @brief This function represents the state SCM_k_WF_GUARD_RESP of the finite state machine ScmFsmProcess().
*
* @param	ps_fsmCb		Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param 	w_snNum    		FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param 	dw_ct        	Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfGuardResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE; /* function result */
  UINT8 b_snStatus;      /* deserialized received SN status */

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_RESP_RX)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    /* deserialize the received SN status */
    SFS_NET_CPY8(&b_snStatus, ADD_OFFSET(ps_fsmCb->adw_respBuff, 0));
    /* if status_OP received */
    if(b_snStatus == (UINT8)SNMTS_k_SN_STS_OP)
    {
      o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_OK,
                                ps_fsmCb->o_reportSnSts);

      /* if no error happened */
      if (o_res)
      {
        /* set the timer to the guard frequency */
        ps_fsmCb->dw_timer = dw_ct + SCM_dw_SnGuardTime;
        ps_fsmCb->dw_timerTotal = dw_ct + SCM_dw_SnGuardTimeTotal;
        ps_fsmCb->e_state = SCM_k_WF_GUARD_TIMER;
      }
    }
    /* if status_PRE_OP received */
    else if(b_snStatus == (UINT8)SNMTS_k_SN_STS_PREOP)
    {
      o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_MISSING, FALSE);

      /* if no error happened */
      if (o_res)
      {
        ps_fsmCb->e_state = SCM_k_SEND_ASSIGN_SADR_REQ;
        ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      }
    }
    else /* invalid response is checked by the SNMTM */
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_INVALID_SNMT_STATE1,
                    (UINT32)(b_snStatus));
    }
  }
  /* else if SNMTM timeout occurred */
  else if (ps_fsmCb->w_event == SCM_k_EVT_SNMTM_TIMEOUT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    /* check if total life time has elapsed */
    if (EPLS_TIMEOUT(dw_ct, ps_fsmCb->dw_timerTotal))
    {
      /* set node state to MISSING and restart FSM */
      o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_MISSING, FALSE);

      /* if no error happened */
      if (o_res)
      {
        ps_fsmCb->e_state = SCM_k_SEND_ASSIGN_SADR_REQ;
        ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      }
    }
    else
    {
      /* leave module state in op but try next guarding */
      ps_fsmCb->dw_timer = dw_ct + SCM_dw_SnGuardTime;
      ps_fsmCb->e_state = SCM_k_WF_GUARD_TIMER;
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

/** @} */
/** @} */
