/**
 * @addtogroup SCM
 * @{
 * @file SCMudidMism.c
 *
 * FSM state functions for the process "UDID mismatch".
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
 * <h2>History for SCMudidMism.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>12.03.2012</td><td>Hans Pill</td><td>A&P281999	switch to valid after quit UDID mismatch</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 * </table>
 *
 * @addtogroup fsm_udid_mismatch FSM - UDID Mismatch
 * @{
 */

#include "SCMint.h"

/**
* @brief This function represents the state SCM_k_WF_ASSIGN_SADR_RESP2 of the finite state machine ScmFsmProcess().
*
* @param	ps_fsmCb    Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param	w_snNum		FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param    dw_ct       Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE       Abort forcing error
*/
BOOLEAN SCM_WfAssignSadrResp2(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                              UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;             /* function result */
  UINT8 *pb_scmUdid = (UINT8 *)NULL; /* UDID of the SCM */

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_RESP_RX)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* if received ADR == SADR and the received SDN == own SDN */
    if((ps_fsmCb->w_rxSdn == SDN_GetSdn(EPLS_k_SCM_INST_NUM)) &&
       (ps_fsmCb->w_tadr == ps_fsmCb->w_sadr))
    {
		/* if setting of the node state to valid succeeded */
		if (SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_VALID, FALSE))
		{
		  /* read UDID of the SCM from the SOD */
		  pb_scmUdid = (UINT8 *)SCM_SodRead(EPLS_k_IDX_COMMON_COM_PARAM,
											EPLS_k_SUBIDX_UDID_SCM);
		  /* if SOD read succeeded */
		  if(pb_scmUdid != NULL)
		  {
			/* if the SNMTM Udid of the SCM assignment request succeeded */
			if (SNMTM_ReqAssgnScmUdid(dw_ct, w_snNum, ps_fsmCb->w_sadr,
									  pb_scmUdid))
			{
			  SCM_NumFreeFrmsDec();
			  ps_fsmCb->e_state = SCM_k_WF_ASSIGN_SCM_UDID_RESP;
			  o_res = TRUE;
			}
			/* no else : the SSDOC Read request failed */
		  }
		  /* no else : SOD read failed */
		}
		/* no else : setting of the node state to valid failed */
    }
    else /* received SADR != SADR or SDN != own SDN */
    {
      /* set node state to wrong SADR */
      o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_WRONG_SADR,
                                ps_fsmCb->o_reportSnSts);

      /* if no error happened */
      if (o_res)
      {
        /* set the timer to the guard frequency */
        ps_fsmCb->dw_timer = dw_ct + SCM_dw_SnGuardTime;
        ps_fsmCb->e_state = SCM_k_IDLE2;
      }
      /* no else : error happened */
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
* @brief This function represents the state SCM_k_VERIFY_UNIQ_UDID of the finite state machine ScmFsmProcess().
*
* @param	ps_fsmCb	Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param    w_snNum 	FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param    dw_ct       Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE         - Abort forcing error
*/
BOOLEAN SCM_VerifyUniqUdid(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                           UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;        /* function result */

  /* if generic event set */
  if(ps_fsmCb->w_event == SCM_k_EVT_GENERIC_EVENT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    /* if the new UDID is unique */
    if(SCM_UniqueUdid((const UINT8 *)ps_fsmCb->adw_newUdid))
    {
      /* the new UDID is written into the SOD */
      MEMCOPY(ps_fsmCb->pb_snUdid, ps_fsmCb->adw_newUdid, EPLS_k_UDID_LEN);

      o_res = SNMTM_ReqAssgnSadr(dw_ct, w_snNum, ps_fsmCb->w_sadr,
                                  (const UINT8 *)ps_fsmCb->adw_newUdid);

      /* if no error happened */
      if (o_res)
      {
        /* UDID is used by a SN */
        ps_fsmCb->o_udidUsed = TRUE;

        ps_fsmCb->e_state = SCM_k_WF_ASSIGN_SADR_RESP2;
        SCM_NumFreeFrmsDec();
      }
      /* no else : error happened */
    }
    else /* the new UDID is not unique */
    {
      /* set node status to INVALID */
      o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_INVALID,
                                ps_fsmCb->o_reportSnSts);

      /* if no error happened */
      if (o_res)
      {
        /* set the timer to the guard frequency */
        ps_fsmCb->dw_timer = dw_ct + SCM_dw_SnGuardTime;
        ps_fsmCb->e_state = SCM_k_IDLE2;
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

/** @} */
/** @} */
