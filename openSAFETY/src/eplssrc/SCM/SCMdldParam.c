/**
 * @addtogroup SCM
 * @{
 * @file SCMdldParam.c
 *
 * FSM state functions for the process "Download Parameters".
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author Joachim Stolberg, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 *
 * <h2>History for SCMdldParam.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 * </table>
 *
 * @addtogroup fsm_download_parameter FSM - Download Parameters
 * @{
 *
 */

#include "SCMint.h"

/**
* @brief This function represents the state SCM_k_WF_PRE_OP_RESP of the finite state
* machine ScmFsmProcess().
*
* @param        ps_fsmCb        Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param        w_snNum         FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param        dw_ct            Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE          Abort forcing error
*/
BOOLEAN SCM_WfPreOpResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  SSDOC_t_REQ s_transInfo;             /* used for SSDOC_SendWriteReq() */
  BOOLEAN o_res = FALSE;               /* function result */
  BOOLEAN o_error = FALSE;             /* error flag */
  UINT32 *pdw_dataLen = (UINT32*)NULL; /* pointer to data length for SOD read
                                          accesses */
  UINT8 *pb_paraSet = (UINT8*)NULL;    /* pointer to parameter set for SOD read
                                          accesses */


  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_RESP_RX)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* try to read the parameter set length */
    pdw_dataLen = (UINT32 *)SCM_SodQueryRead(SCM_k_IDX_PARA_LEN(w_snNum),
                                             &o_error);
    if(pdw_dataLen != NULL)
    {
      pb_paraSet = (UINT8 *)SCM_SodRead(SCM_k_IDX_PARA_SET(w_snNum));

      /* if SOD Read succeeded */
      if(pb_paraSet != NULL)
      {
        s_transInfo.w_idx = 0x101AU;
        s_transInfo.b_subIdx = 0U;
        s_transInfo.e_dataType = EPLS_k_DOMAIN;
        /* the amount of payload data is extended because slim SSDO traffic is to be used 
         * payload data is only in subframe 1 */
        s_transInfo.b_payloadLen = (UINT8) ((ps_fsmCb->w_payLd - 1)*2);
        s_transInfo.pb_data = pb_paraSet;
        s_transInfo.dw_dataLen = *pdw_dataLen;
        o_res = SSDOC_SendWriteReq(ps_fsmCb->w_sadr, w_snNum, SCM_SsdocClbk,
                                   dw_ct, &s_transInfo, TRUE);

        /* if no error happened */
        if (o_res)
        {
          ps_fsmCb->e_state = SCM_k_WF_PARAM_DL_RESP;
          SCM_NumFreeFrmsDec();
        }
        /* no else : error happened */
      }
      /* no else : SOD Read failed */
    }
    /* if no parameter set available */
    else if(o_error == FALSE)
    {
      /* set node state to wrong Parameters */
      o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_WRONG_PARAM,
                                ps_fsmCb->o_reportSnSts);

      /* if no error happened */
      if (o_res)
      {
        /* set the timer to the guard frequency */
        ps_fsmCb->dw_timer = dw_ct + SCM_dw_SnGuardTime;
        ps_fsmCb->e_state = SCM_k_IDLE2;
      }
      /* no else : SOD Read failed */
    }
    else
    {
      o_res = FALSE;              /* SOD access error */
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
* @brief This function represents the state SCM_k_WF_PARAM_DL_RESP of the finite state
* machine ScmFsmProcess().
*
* @param        ps_fsmCb        Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param        w_snNum         FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param        dw_ct           Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE         - Abort forcing error
**/
BOOLEAN SCM_WfParamDlResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;        /* function result */

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SSDOC_RESP_RX)
  {
    /* reset the event and set the new event */
    ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
    ps_fsmCb->e_state = SCM_k_ASSIGN_ADD_SADR;

    ps_fsmCb->w_addSadr = 0U;      /* reset "Additional SADR" search position */

    o_res = TRUE;
  }
  /* else if SSDOC timeout occurred */
  else if (ps_fsmCb->w_event == SCM_k_EVT_SSDOC_TIMEOUT)
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
  /* else if response error occurred */
  else if(ps_fsmCb->w_event == SCM_k_EVT_RESP_ERROR)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* set node state to wrong Parameters */
    o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_WRONG_PARAM,
                              ps_fsmCb->o_reportSnSts);

    /* if no error happened */
    if (o_res)
    {
      /* set the timer to the guard frequency */
      ps_fsmCb->dw_timer = dw_ct + SCM_dw_SnGuardTime;
      ps_fsmCb->e_state = SCM_k_IDLE2;
    }
    /* no else : SOD Read failed */
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
* @brief This function represents the state SCM_k_ASSIGN_ADD_SADR of the finite state
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
BOOLEAN SCM_AssignAddSadr(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  UINT16 *pw_txSpdoNum = (UINT16 *)NULL; /* data pointer for SOD access */
  BOOLEAN o_res = FALSE;                 /* function result */

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_GENERIC_EVENT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    /* read the next Additional SADR from the "Additional SADR List" */
    ps_fsmCb->w_addSadr = SCM_GetNextAddSadr(ps_fsmCb->w_sadr,
                                             ps_fsmCb->w_addSadr);
    /* if additional SADR id valid */
    if(ps_fsmCb->w_addSadr != 0U)
    {
      /* read the correspondung TxSPDO number */
      pw_txSpdoNum = (UINT16 *)SCM_SodRead((UINT16)(0xC800U +
                                                    ps_fsmCb->w_addSadr), 2U);
      if(pw_txSpdoNum != NULL)
      {
        o_res = SNMTM_ReqAssgnAddSadr(dw_ct, w_snNum, ps_fsmCb->w_sadr,
                                      ps_fsmCb->w_addSadr, *pw_txSpdoNum);

        /* if no error happened */
        if (o_res)
        {
          ps_fsmCb->e_state = SCM_k_WF_ADD_SADR_RESP;
          SCM_NumFreeFrmsDec();
        }
        /* no else : error happened */
      }
      /* else: o_res = FALSE; */
    }
    else /* no further additional SADRs */
    {
      ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      ps_fsmCb->e_state = SCM_k_SEND_PUT_TO_OP;
      o_res = TRUE;
    }
  }
  /* other event */
  else
  {
    /* error is generated in ScmFsmProcess() */
    o_res = TRUE;
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}

/**
* @brief This function represents the state SCM_k_WF_ADD_SADR_RESP
*               of the finite state machine ScmFsmProcess().
*
* @param		ps_fsmCb	Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param        w_snNum  FSM slot number (=> index to the DVI list) (not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param              dw_ct Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE         - Abort forcing error
*/
BOOLEAN SCM_WfAddSadrResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;        /* function result */
  UINT16 w_sadr;

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_RESP_RX)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* get the received SARD */
    SFS_NET_CPY16(&w_sadr, ADD_OFFSET(ps_fsmCb->adw_respBuff, 1));
    /* if received add SADR equal to sent add SADR*/
    if(w_sadr == ps_fsmCb->w_addSadr)
    {
      ps_fsmCb->e_state = SCM_k_ASSIGN_ADD_SADR;
      ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      o_res = TRUE;
    }
    else /* unvalid received add SADR */
    {
      /* set node state to wrong Parameters */
      o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_WRONG_PARAM,
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


/** @} */
/** @} */
