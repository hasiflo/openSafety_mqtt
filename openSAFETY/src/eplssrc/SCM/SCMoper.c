/**
 * @addtogroup SCM
 * @{
 * @file SCMoper.c
 *
 * FSM state functions for the process "Safety address Verification",
 * "Wait for Operator Acknowledge" and "IDLE2" states.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author Joachim Stolberg, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * @defgroup fsm_idle2 FSM - Idle2
 * @defgroup fsm_wait_op_ack FSM - Wait for Operator Acknowledge
 * @defgroup fsm_safety_address_verify FSM - Safety address verification
 *
 * <h2>History for SCMoper.c</h2>
 * <table>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>changes for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>25.04.2017</td><td>Stefan Innerhofer</td><td>changes to use the same 40 Bit CT initial value in case of reinitialization</td></tr>
 *     <tr><td>05.09.2018</td><td>Stefan Innerhofer</td><td>add handling of received SN Fails, as a reaction to an unexpected event in the SNMTS FSM, to the states SCM_k_WF_ASSIGN_SADR_RESP, SCM_k_WF_ASSIGN_SCM_UDID_RESP and SCM_k_WF_INIT_CT_RESP</td></tr>
 * </table>
 */


#include "SCMint.h"

static BOOLEAN CheckSnUdid(SCM_t_FSM_CB *ps_fsmCb, BOOLEAN *po_equal);

/**
* @brief This function checks if a SN fails was received
*
* @param ps_fsmCb       Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param w_snNum        FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @ingroup fsm_safety_address_verify
*
* @return
* - TRUE  - a SN fail was received
* - FALSE - otherwise
*/
BOOLEAN SCM_CheckForSnFail(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  UINT8 b_snStatus;        /* deserialized received SN status */
  UINT8 b_errGroup;        /* deserialized error group */
  UINT8 b_errCode;         /* deserialized error code */
  BOOLEAN o_res = FALSE;   /* function result */

  SFS_NET_CPY8(&b_snStatus, ADD_OFFSET(ps_fsmCb->adw_respBuff, 0));

  if(b_snStatus == SNMTS_k_SN_FAIL)
  {
     /* deserialize the received error group */
     SFS_NET_CPY8(&b_errGroup, ADD_OFFSET(ps_fsmCb->adw_respBuff, 1));
     /* deserialize the received error code */
     SFS_NET_CPY8(&b_errCode, ADD_OFFSET(ps_fsmCb->adw_respBuff, 2));

     /* change state to SCM_k_SEND_ASSIGN_SADR_REQ  */
     ps_fsmCb->b_errGroup = b_errGroup;
     ps_fsmCb->b_errCode = b_errCode;
     ps_fsmCb->w_event = SCM_k_EVT_ACK_RECEIVED;

     ps_fsmCb->e_state = SCM_k_WF_SAPL_ACK;

     /*signal SN fail to application*/
     SAPL_ScmSnFailClbk(ps_fsmCb->w_sadr, b_errGroup, b_errCode, w_snNum);

     o_res = TRUE;
  }//if(b_snStatus == SNMTS_k_SN_FAIL)

  return o_res;
}

/**
* @brief This function represents the state SCM_k_SEND_ASSIGN_SADR_REQ of the finite state machine ScmFsmProcess().
*
* @param ps_fsmCb		Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param w_snNum		FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param dw_ct   		Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @ingroup fsm_safety_address_verify
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_SendAssignSadrReq(SCM_t_FSM_CB *ps_fsmCb,
		                         UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;          /* function result */

  if(ps_fsmCb->w_event == SCM_k_EVT_GENERIC_EVENT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    o_res = SNMTM_ReqAssgnSadr(dw_ct, w_snNum, ps_fsmCb->w_sadr,
                               ps_fsmCb->pb_snUdid);

    /* if no error happened */
    if (o_res)
    {
      ps_fsmCb->e_state = SCM_k_WF_ASSIGN_SADR_RESP;
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
* @brief This function represents the state SCM_k_WF_ASSIGN_SADR_RESP of the finite state machine ScmFsmProcess().
*
* @param ps_fsmCb	Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param w_snNum 	FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param dw_ct		Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @ingroup fsm_safety_address_verify
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfAssignSadrResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                             UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;             /* function result */
  UINT8 *pb_scmUdid = (UINT8 *)NULL; /* UDID of the SCM */


  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_RESP_RX)
  {
    /* check if a sn fail was received */
    if(SCM_CheckForSnFail(ps_fsmCb, w_snNum, dw_ct) == FALSE)
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
              /* UDID is used by a SN */
              ps_fsmCb->o_udidUsed = TRUE;

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
    /*no else if SN Fail occued, handled in SCM_CheckForSnFail()*/
  }
  /* else if timeout occurred */
  else if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_TIMEOUT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    o_res = SNMTM_ReqUdid(dw_ct, w_snNum, ps_fsmCb->w_sadr);

    /* if no error happened */
    if (o_res)
    {
      ps_fsmCb->e_state = SCM_k_WF_UDID_RESP;
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
* @brief This function represents the state SCM_k_WF_ASSIGN_SCM_UDID_RESP of the finite state machine ScmFsmProcess().
*
* @param ps_fsmCb	Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param w_snNum 	FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param dw_ct      Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @ingroup fsm_safety_address_verify
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfAssignScmUdidResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                                UINT32 dw_ct)
{
  SSDOC_t_REQ s_transInfo;           /* used for SSDOC_ReadWriteReq() */
  BOOLEAN o_res = FALSE;             /* function result */
  UINT8 *pb_scmUdid = (UINT8 *)NULL; /* UDID of the SCM */
  UINT32 *pul_optFeat = (UINT32 *)NULL; /* optional features of the SN */
  UINT32 adw_respScmUdid[(EPLS_k_UDID_LEN/4U)+1U]; /* deserialized SCM UDID
                                                      response */
  UINT8 ab_zeroInitCtVal[EPLS_k_LEN_EXT_CT] = {0,0,0,0,0}; /*empty array to check if a 40 CT
                                                             init value was set already*/

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_RESP_RX)
  {
    /* check if a sn fail was received */
    if(SCM_CheckForSnFail(ps_fsmCb, w_snNum, dw_ct) == FALSE)
    {
      /* reset the event */
      ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

      /* read UDID of the SCM from the SOD */
      pb_scmUdid = (UINT8 *)SCM_SodRead(EPLS_k_IDX_COMMON_COM_PARAM,
                                        EPLS_k_SUBIDX_UDID_SCM);
      /* if SOD read succeeded */
      if(pb_scmUdid != NULL)
      {
        /* deserialize the SCM UDID response */
        SFS_NET_CPY_DOMSTR(adw_respScmUdid,
                           ADD_OFFSET(ps_fsmCb->adw_respBuff, 1U),
                           EPLS_k_UDID_LEN);
        /* if the UDID of the SCM matches */
        if(MEMCOMP(pb_scmUdid, adw_respScmUdid, EPLS_k_UDID_LEN) == MEMCMP_IDENT)
        {
          /* if received ADR == SADR and the received SDN == own SDN */
          if((ps_fsmCb->w_rxSdn == SDN_GetSdn(EPLS_k_SCM_INST_NUM)) &&
             (ps_fsmCb->w_tadr == ps_fsmCb->w_sadr))
          {
            /* read optional features from the SOD */
            pul_optFeat = (UINT32 *)SCM_SodRead(SCM_k_IDX_OPT_FEAT(w_snNum));
            if ( NULL != pul_optFeat)
            {
              /* if 40 bit SPDOs are enabled for the node */
              if ( 0 != (*pul_optFeat & SCM_k_FEATURE_40_BIT_SPDO))
              {
                /* use always the same initial value for one node,
                 * therefore check if the initial value was set already*/
                if (MEMCOMP(&ps_fsmCb->ab_initCtVal[0],&ab_zeroInitCtVal[0],sizeof(ps_fsmCb->ab_initCtVal)) == MEMCMP_IDENT)
                {
                  MEMCOPY(&ps_fsmCb->ab_initCtVal[0],
                                     &SCM_ddw_initCt,
                                     EPLS_k_LEN_EXT_CT);
                }
                /* request to send the initial CT value to the SN */
                if (SNMTM_ReqInitializeCtSn(dw_ct, w_snNum, ps_fsmCb->w_sadr,
                    (UINT8*)&ps_fsmCb->ab_initCtVal[0]))
                {
                  SCM_NumFreeFrmsDec();
                  ps_fsmCb->e_state = SCM_k_WF_INIT_CT_RESP;
                  o_res = TRUE;
                }
              }
              else
              {
                /* send Vendor-ID read request */
                s_transInfo.w_idx = EPLS_k_IDX_DEVICE_VEN_ID;
                s_transInfo.b_subIdx = SCM_k_SUB_IDX_VENDOR_ID;
                s_transInfo.e_dataType = EPLS_k_UINT32;
                s_transInfo.b_payloadLen = (UINT8) ps_fsmCb->w_payLd;
                s_transInfo.pb_data = (UINT8 *)ps_fsmCb->adw_respBuff;
                s_transInfo.dw_dataLen = SCM_k_RESP_BUFF_SIZE;

                /* if the SSDOC Read request succeeded */
                if (SSDOC_SendReadReq(ps_fsmCb->w_sadr, w_snNum, SCM_SsdocClbk, dw_ct,
                                      &s_transInfo))
                {
                  SCM_NumFreeFrmsDec();
                  ps_fsmCb->e_state = SCM_k_WF_VENDOR_ID_RESP;
                }
                /* no else : the SSDOC Read request failed */
              }
            }

          }
          else /* received SADR != SADR or SDN != own SDN */
          {
            /* set node state to missing */
            o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_MISSING,
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
        else /* received UDID of the SCM does not match */
        {
          /* set node state to missing */
          o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_MISSING,
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
    /* no else : SOD read failed */
    }
    /*no else if SN Fail occued, handled in SCM_CheckForSnFail()*/
  }
  /* else if timeout occurred */
  else if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_TIMEOUT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    /* set node state to missing */
    o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_MISSING,
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
* @brief This function represents the state SCM_k_WF_INIT_CT_RESP of the finite state machine ScmFsmProcess().
*
* @param ps_fsmCb Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param w_snNum  FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param dw_ct      Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @ingroup fsm_safety_address_verify
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfInitExtCtResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                                UINT32 dw_ct)
{
  SSDOC_t_REQ s_transInfo;           /* used for SSDOC_ReadWriteReq() */
  BOOLEAN o_res = FALSE;             /* function result */
  UINT32 adw_respExtCt[(EPLS_k_LEN_EXT_CT/4U)+1U]; /* deserialized extended CT
                                                      response */

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_RESP_RX)
  {
    /* check if a sn fail was received */
    if(SCM_CheckForSnFail(ps_fsmCb, w_snNum, dw_ct) == FALSE)
    {
      /* reset the event */
      ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
      /* deserialize the SCM UDID response */
      SFS_NET_CPY_DOMSTR(adw_respExtCt,
                           ADD_OFFSET(ps_fsmCb->adw_respBuff, 1U),
                           EPLS_k_LEN_EXT_CT);

      /* if the UDID of the SCM matches */
      if(MEMCOMP(&ps_fsmCb->ab_initCtVal[0], adw_respExtCt, EPLS_k_LEN_EXT_CT) == MEMCMP_IDENT)
      {
        /* if received ADR == SADR and the received SDN == own SDN */
        if((ps_fsmCb->w_rxSdn == SDN_GetSdn(EPLS_k_SCM_INST_NUM)) &&
           (ps_fsmCb->w_tadr == ps_fsmCb->w_sadr))
        {
          /* send Vendor-ID read request */
          s_transInfo.w_idx = EPLS_k_IDX_DEVICE_VEN_ID;
          s_transInfo.b_subIdx = SCM_k_SUB_IDX_VENDOR_ID;
          s_transInfo.e_dataType = EPLS_k_UINT32;
          s_transInfo.b_payloadLen = (UINT8) ps_fsmCb->w_payLd;
          s_transInfo.pb_data = (UINT8 *)ps_fsmCb->adw_respBuff;
          s_transInfo.dw_dataLen = SCM_k_RESP_BUFF_SIZE;

          /* if the SSDOC Read request succeeded */
          if (SSDOC_SendReadReq(ps_fsmCb->w_sadr, w_snNum, SCM_SsdocClbk, dw_ct,
                                &s_transInfo))
          {
            SCM_NumFreeFrmsDec();
            ps_fsmCb->e_state = SCM_k_WF_VENDOR_ID_RESP;
          }
          /* no else : the SSDOC Read request failed */
        }
        else /* received SADR != SADR or SDN != own SDN */
        {
          /* set node state to missing */
          o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_ERR_INIT_CT,
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
      else /* received extended CT does not match */
      {
        /* set node state to missing */
        o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_ERR_INIT_CT,
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
     /*no else if SN Fail occued, handled in SCM_CheckForSnFail()*/
  }
  /* else if timeout occurred */
  else if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_TIMEOUT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    /* set node state to missing */
    o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_ERR_INIT_CT,
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
* @brief This function represents the state SCM_k_WF_UDID_RESP of the finite state machine ScmFsmProcess().
*
* @param ps_fsmCb 	Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param w_snNum 	FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param dw_ct		Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @ingroup fsm_safety_address_verify
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfUdidResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;          /* function result */
  BOOLEAN o_equal = FALSE;        /* while loop abort condition */

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_RESP_RX)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    /* if UDID checking succeeded */
    if (CheckSnUdid(ps_fsmCb, &o_equal))
    {
      /* if received_UDID matches the UDID from the SADR-UDID-List */
      if(o_equal)
      {
        ps_fsmCb->e_state = SCM_k_SEND_ASSIGN_SADR_REQ; /* try again */
        ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      }
      else /* UDID mismatch */
      {
        /* store received UDID for later use */
        SFS_NET_CPY_DOMSTR(ps_fsmCb->adw_newUdid, ps_fsmCb->adw_respBuff,
                           EPLS_k_UDID_LEN);
        /* set node state to UDID mismatch */
        o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_UDID_MISMATCH,
                                  ps_fsmCb->o_reportSnSts);

        /* if no error happened */
        if(o_res == TRUE)
        {
          if(SCM_b_ConfigMode == k_SCM_ACM)
          {
            ps_fsmCb->e_state = SCM_k_WF_OPERATOR_ACK;
          }
          else /* configuration mode = MCM */
          {
            /* set the timer to the guard frequency */
            ps_fsmCb->dw_timer = dw_ct + SCM_dw_SnGuardTime;
            ps_fsmCb->e_state = SCM_k_IDLE2;
          }
          /* signal UDID mismatch to application */
          SAPL_ScmUdidMismatchClbk(ps_fsmCb->w_sadr,
                                   (UINT8 *)ps_fsmCb->adw_newUdid, w_snNum);
        }
        /* no else : error happened */
      }
    }
    /* no else : UDID checking failed */
  }
  /* else if timeout occurred */
  else if(ps_fsmCb->w_event == SCM_k_EVT_SNMTM_TIMEOUT)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_MISSING,
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
* @brief This function represents the state SCM_k_WF_OPERATOR_ACKof the finite state machine ScmFsmProcess().
*
* @param ps_fsmCb 	Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param w_snNum 	FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param dw_ct      Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @ingroup fsm_wait_op_ack
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfOperatorAck(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;        /* function result */

  dw_ct = dw_ct; /* avoid compiler warning */

  /* if acknowledge received or
        acknowledge received and SCM_ResetNodeGuarding() was called */
  if((ps_fsmCb->w_event == SCM_k_EVT_ACK_RECEIVED) ||
     (ps_fsmCb->w_event == (SCM_k_EVT_ACK_RECEIVED | SCM_k_EVT_RESET_NODE_GRD)))
  {
    /* reset the event and set the new event */
    ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
    ps_fsmCb->e_state = SCM_k_VERIFY_UNIQ_UDID;

    o_res = TRUE;
  }
  /* else if only SCM_ResetNodeGuarding() was called */
  else if(ps_fsmCb->w_event == SCM_k_EVT_RESET_NODE_GRD)
  {
    /* reset the event and set the new event */
    ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
    ps_fsmCb->e_state = SCM_k_SEND_ASSIGN_SADR_REQ;

    /* setting node status to MISSING */
    o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_MISSING, FALSE);
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
* @brief This function represents the state SCM_k_IDLE2 of the finite state machine ScmFsmProcess().
*
* @param ps_fsmCb	Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param w_snNum 	FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param dw_ct 		Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @note value is necessary because all FSM state functions should have the same function prototype.
*
* @ingroup fsm_idle2
*
* @return Allways TRUE
*/
BOOLEAN SCM_Idle2(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  dw_ct = dw_ct;                /* to avoid warnings */
  w_snNum = w_snNum;            /* to avoid warnings */

  /* if guard timer expired */
  if(ps_fsmCb->w_event == SCM_k_EVT_TIMEOUT)
  {
    /* reset the event and set the new event */
    ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
    ps_fsmCb->e_state = SCM_k_SEND_ASSIGN_SADR_REQ;
  }
  /* no else: error is generated in ScmFsmProcess() */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return TRUE;
}

/**
* @brief This function searches the received UDID in the UDID list.
*
* @param ps_fsmCb	Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @retval po_equal
* - TRUE : UDID is valid and sub-index of the UDID in the SADR-UDID-List is stored
* - FALSE : UDID is invalid (pointer not checked, only called with reference to variable in SCM_WfUdidResp()). Valid range: <> NULL
*
* @return
* - FALSE         - Abort forcing error
*/
static BOOLEAN CheckSnUdid(SCM_t_FSM_CB *ps_fsmCb, BOOLEAN *po_equal)
{
  UINT8 b_idx = (UINT8)0U;                    /* loop counter */
  UINT8 *pb_noE = (UINT8 *)NULL;  /* data pointer for SOD read */
  UINT8 *pb_udid = (UINT8 *)NULL; /* UDID from the SADR-UDID-List */
  BOOLEAN o_res = FALSE;          /* function result */
  UINT32 adw_respUdid[(EPLS_k_UDID_LEN/4U)+1U]; /* deserialized UDID response */

  *po_equal = FALSE;

  /* deserialize the UDID response */
  SFS_NET_CPY_DOMSTR(adw_respUdid, ps_fsmCb->adw_respBuff, EPLS_k_UDID_LEN);

  /* if Auto Configuration Mode */
  if(SCM_b_ConfigMode == k_SCM_ACM)
  {
    /* check received UDID against the SADR-UDID-List */
    *po_equal = (BOOLEAN)(MEMCOMP(ps_fsmCb->pb_snUdid, adw_respUdid,
                                  EPLS_k_UDID_LEN) == MEMCMP_IDENT);
    o_res = TRUE;
  }
  else  /* Manual Configuration Mode */
  {
    /* check received UDID against the SADR-UDID-List */
    /* check all subindexes for a valid UDID, therefore read subindex 0 */
    pb_noE = (UINT8 *)SCM_SodRead(SCM_k_IDX_UDID_LIST(ps_fsmCb->w_sadr, 0));

    /* if SOD read succeeded */
    if(pb_noE != NULL)
    {
      b_idx = (UINT8)1U;
      o_res = TRUE;
      /* search in the SADR-UDID-List for an UDID match */
      while((b_idx <= *pb_noE) && (o_res == TRUE) && (*po_equal == FALSE))
      {
        pb_udid = (UINT8 *)SCM_SodRead(SCM_k_IDX_UDID_LIST(ps_fsmCb->w_sadr,
                                       b_idx));
        /*if SOD read succeeded */
        if(pb_udid != NULL)
        {
          /* if UDID matches */
          if (MEMCOMP(pb_udid, adw_respUdid, EPLS_k_UDID_LEN) == MEMCMP_IDENT)
          {
            /* pointer to the SN Udid in the SCM control block is set to the
               new UDID */
            ps_fsmCb->pb_snUdid = pb_udid;
            *po_equal = TRUE;
          }
          else /* else UDID does not match */
          {
            *po_equal = FALSE;
            b_idx++;
          }
        }
        else /* else SOD read failed */
        {
          o_res = FALSE;
        }
      }
    }
    /* no else : SOD read failed */
  }

  SCFM_TACK_PATH();
  return o_res;
}
/** @} */
