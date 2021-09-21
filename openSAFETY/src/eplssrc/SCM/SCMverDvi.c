/**
 * @addtogroup SCM
 * @{
 * @file SCMverDvi.c
 *
 * FSM state functions for the process "Verify DVI".
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
 * <h2>History for SCMverDvi.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SLV10</td></tr>
 *     <tr><td>23.01.2013</td><td>Hans Pill</td><td>changes for handling of the new timestamp and CRC domain</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>21.07.2016</td><td>Stefan Innerhofer</td><td>changes for legacy mode time stamp</td></tr>
 * </table>
 *
 * @addtogroup fsm_verify_dvi FSM - Verify DVI
 * @{
 */

#include "SCMint.h"

/**
* @brief This function represents the state SCM_k_WF_VENDOR_ID_RESP of the finite state machine ScmFsmProcess().
*
* @param   ps_fsmCb     Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param	w_snNum     FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param	dw_ct       Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfVendorIdResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  SSDOC_t_REQ s_transInfo;            /* used for SSDOC_ReadWriteReq() */
  BOOLEAN o_res = FALSE;              /* function result */
  UINT32 *pdw_venId = (UINT32 *)NULL; /* data pointer fort SOD access */

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SSDOC_RESP_RX)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* read SOD VendorID */
    pdw_venId = (UINT32 *)SCM_SodRead(SCM_k_IDX_VENDOR_ID(w_snNum));
    if(pdw_venId != NULL)
    {
      /* if received VendorID == VendorID from DVI list */
      if(ps_fsmCb->adw_respBuff[0] == *pdw_venId)
      {
        /* read request for the Product Code */
        s_transInfo.w_idx = EPLS_k_IDX_DEVICE_VEN_ID;
        s_transInfo.b_subIdx = SCM_k_SUB_IDX_PROD_CODE;
        s_transInfo.e_dataType = EPLS_k_UINT32;
        s_transInfo.b_payloadLen = (UINT8) ps_fsmCb->w_payLd;
        s_transInfo.pb_data = (UINT8 *)ps_fsmCb->adw_respBuff;
        s_transInfo.dw_dataLen = SCM_k_RESP_BUFF_SIZE;
        o_res = SSDOC_SendReadReq(ps_fsmCb->w_sadr, w_snNum, SCM_SsdocClbk,
                                  dw_ct, &s_transInfo);

        /* if no error happened */
        if (o_res)
        {
          ps_fsmCb->e_state = SCM_k_WF_PRODUCT_CODE_RESP;
          SCM_NumFreeFrmsDec();
        }
        /* no else : error happened */
      }
      else /* wrong received_VendorID */
      {
        /* set node state to INVALID */
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
    /* else: o_res = FALSE; */
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
      ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      ps_fsmCb->e_state = SCM_k_SEND_ASSIGN_SADR_REQ;
    }
  }
  /* else if error occurred */
  else if(ps_fsmCb->w_event == SCM_k_EVT_RESP_ERROR)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* set node state to INVALID */
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
* @brief This function represents the state SCM_k_WF_PRODUCT_CODE_RESP
*               of the finite state machine ScmFsmProcess().
*
* @param       ps_fsmCb         Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param       w_snNum          FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param       dw_ct            Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE         - Abort forcing error
*/
BOOLEAN SCM_WfProductCodeResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                              UINT32 dw_ct)
{
  SSDOC_t_REQ s_transInfo;          /* used for SSDOC_ReadWriteReq() */
  BOOLEAN o_res = FALSE;            /* function result */
  UINT32 *pdw_prodCode = (UINT32 *)NULL; /* data pointer fort SOD access */

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SSDOC_RESP_RX)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* read SOD ProductCode */
    pdw_prodCode = (UINT32 *)SCM_SodRead(SCM_k_IDX_PROD_CODE(w_snNum));
    if(pdw_prodCode != NULL)
    {
      /* if received ProductCode == ProductCode from DVI list */
      if(ps_fsmCb->adw_respBuff[0] == *pdw_prodCode)
      {
        /* read request for the Revision Number */
        s_transInfo.w_idx = EPLS_k_IDX_DEVICE_VEN_ID;
        s_transInfo.b_subIdx = SCM_k_SUB_IDX_REV_NUM;
        s_transInfo.e_dataType = EPLS_k_UINT32;
        s_transInfo.b_payloadLen = (UINT8) ps_fsmCb->w_payLd;
        s_transInfo.pb_data = (UINT8 *)ps_fsmCb->adw_respBuff;
        s_transInfo.dw_dataLen = SCM_k_RESP_BUFF_SIZE;
        o_res = SSDOC_SendReadReq(ps_fsmCb->w_sadr, w_snNum, SCM_SsdocClbk,
                                  dw_ct, &s_transInfo);

        /* if no error happened */
        if (o_res)
        {
          ps_fsmCb->e_state = SCM_k_WF_REVISION_NUMBER_RESP;
          SCM_NumFreeFrmsDec();
        }
        /* no else : error happened */
      }
      else /* wrong received ProductCode */
      {
        /* set node state to INVALID */
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
    /* else: o_res = FALSE */
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
      ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      ps_fsmCb->e_state = SCM_k_SEND_ASSIGN_SADR_REQ;
    }
  }
  /* else if error occurred */
  else if(ps_fsmCb->w_event == SCM_k_EVT_RESP_ERROR)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* set node state to INVALID */
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
* @brief This function represents the state
*               SCM_k_WF_REVISION_NUMBER_RESP of the finite state machine
*                ScmFsmProcess().
*
* @param    ps_fsmCb            Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param    w_snNum             FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param    dw_ct				Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfRevisionNumberResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                                 UINT32 dw_ct)
{
  SSDOC_t_REQ s_transInfo;             /* used for SSDOC_ReadWriteReq() */
  BOOLEAN o_res = FALSE;               /* function result */
  UINT32 *pdw_revNum = (UINT32 *)NULL; /* data pointer for SOD access of revision number*/
  UINT32 *pdw_optFeat = (UINT32 *)NULL; /* data pointer for SOD access of optional features*/

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SSDOC_RESP_RX)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* read the revision number from the DVI list */
    pdw_revNum = (UINT32 *)SCM_SodRead(SCM_k_IDX_REV_NUM(w_snNum));
    if(pdw_revNum != NULL)
    {
      /* if revision number is accepted by the application */
      if(SAPL_ScmRevisionNumberClbk(ps_fsmCb->w_sadr, *pdw_revNum,
                                    ps_fsmCb->adw_respBuff[0]))
      {
        /* get the optional feature flags*/
        pdw_optFeat = (UINT32*)SCM_SodRead(SCM_k_IDX_OPT_FEAT(w_snNum));

        /* if the "legacy mode time stamp" bit is set in the optional features
         * request the parameter timestamp otherwise request the parameter CRC*/
        if((pdw_optFeat != NULL) &&
          ((*pdw_optFeat & SCM_k_FEATURE_LEGACY_MODE_TS) != 0))
        {
          /* read request for the timestamp */
          s_transInfo.w_idx = EPLS_k_IDX_DEVICE_VEN_ID;
          s_transInfo.b_subIdx = EPLS_k_SUBIDX_PARAM_TS;
          s_transInfo.e_dataType = EPLS_k_UINT32;
          s_transInfo.b_payloadLen = (UINT8) ps_fsmCb->w_payLd;
          s_transInfo.pb_data = (UINT8 *)ps_fsmCb->adw_respBuff;
          s_transInfo.dw_dataLen = SCM_k_RESP_BUFF_SIZE;
        }
        else
        {
          /* read request for the parameter CRC */
          s_transInfo.w_idx = EPLS_k_IDX_DEVICE_VEN_ID;
          s_transInfo.b_subIdx = EPLS_k_SUBIDX_PARAM_CHKSUM;
          s_transInfo.e_dataType = EPLS_k_DOMAIN;
          s_transInfo.b_payloadLen = (UINT8) ps_fsmCb->w_payLd;
          s_transInfo.pb_data = ps_fsmCb->pb_remTimeCrc;
          s_transInfo.dw_dataLen = ps_fsmCb->dw_maxRemTimeCrcLen;
        }

        /* send the SSDO read request*/
        o_res = SSDOC_SendReadReq(ps_fsmCb->w_sadr, w_snNum, SCM_SsdocClbk,
                                            dw_ct, &s_transInfo);
        /* if no error happened */
        if (o_res)
        {
          ps_fsmCb->e_state = SCM_k_WF_TIMESTAMP;
          SCM_NumFreeFrmsDec();
        }
        /* no else : error happened */
      }
      else /* wrong RevisionNumber */
      {
        /* set node state to INVALID */
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
    /* else: o_res = FALSE; */
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
  /* else if error occurred */
  else if(ps_fsmCb->w_event == SCM_k_EVT_RESP_ERROR)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* set node state to INVALID */
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
