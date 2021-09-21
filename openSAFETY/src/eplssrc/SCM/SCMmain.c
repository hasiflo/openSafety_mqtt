/**
 * @addtogroup SCM
 * @{
 * @file SCMapi.h
 *
 *  Definition of the unit export functions (API and stack internal).
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author Joachim Stolberg, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * The configuration manager works for its own, so it has only a small "interface".
 * SCM_Init() must be called once after startup and SCM_Trigger() cyclically. To
 * start/stop the SCM SCM_Activate()/SCM_Deactivate() can be used.
 *
 * <h2>History for SCMapi.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>20.07.2010</td><td>Hans Pill</td><td>array should be in initialized safety variables</td></tr>
 *     <tr><td>24.11.2010</td><td>Hans Pill</td><td>Review SL V20</td></tr>
 *     <tr><td>04.08.2011</td><td>Hans Pill</td><td>A&P267345 UDID must be unique, if the UDID is not used but present it is to be deleted</td></tr>
 *     <tr><td>06.09.2011</td><td>Hans Pill</td><td>A&P263905 only process node if allowed</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 *     <tr><td>12.03.2012</td><td>Hans Pill</td><td>A&P279170 do not set the node state to "missing" in case of a reset of the guard time</td></tr>
 *     <tr><td>23.01.2013</td><td>Hans Pill</td><td>changes for additional parameters according to openSafety specification</td></tr>
 *     <tr><td>07.03.2013</td><td>Hans Pill</td><td>changes for parameter checksum being written to the SN</td></tr>
 *     <tr><td>23.03.2013</td><td>Hans Pill</td><td>changes for handling of the new timestamp and CRC domain</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>changes for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>19.04.2017</td><td>Roman Zwischelsberger</td><td>changes for better handling of SNMT timeouts</td></tr>
 *     <tr><td>17.09.2018</td><td>Stefan Innerhofer</td><td>reset SCM if an unexpected event occurred</td></tr>
 * </table>
 */

#include "SCMint.h"

/** safety node guarding: time between two guardings */
UINT32 SCM_dw_SnGuardTime SAFE_NO_INIT_SEKTOR;
/** safety node guarding: timeout until module missing */
UINT32 SCM_dw_SnGuardTimeTotal SAFE_NO_INIT_SEKTOR;
/** To distinguish between ACM and MCM */
UINT8 SCM_b_ConfigMode SAFE_NO_INIT_SEKTOR;
/** initial CT value */
UINT64 SCM_ddw_initCt SAFE_NO_INIT_SEKTOR;
/** value needed for increasing the initial CT value */
static UINT32 dw_lastScmCt SAFE_NO_INIT_SEKTOR;

/**
 * Table over all FSM state functions, used by ScmFsmProcess().
 *
 * ScmFsmProcess() therefore uses no switch/case for the function call,
 * but directly the function pointers from this list.
 */
static t_FSM_FUNC as_FsmStateFunctions[SCM_k_NUM_CFG_FSM_STATES] SAFE_INIT_SEKTOR = {
  /* Sub-FSM Operational */
  { k_SNMTM_SLOT, SCM_SendAssignSadrReq},
  { k_SNMTM_SLOT, SCM_WfAssignSadrResp},
  { k_SSDOC_SNMTM_SLOT, SCM_WfAssignScmUdidResp},
  { k_NO_SLOT, SCM_WfUdidResp},
  { k_SSDOC_SLOT, SCM_WfInitExtCtResp},
  { k_NO_SLOT, SCM_WfOperatorAck},
  { k_NO_SLOT, SCM_Idle2},
  /* Sub-FSM Verify DVI */
  { k_SSDOC_SLOT, SCM_WfVendorIdResp},
  { k_SSDOC_SLOT, SCM_WfProductCodeResp},
  { k_SSDOC_SLOT, SCM_WfRevisionNumberResp},
  /* Sub-FSM Verify Parameters */
  { k_SNMTM_SLOT, SCM_WfTimestamp},
  /* Sub-FSM Download Parameters */
  { k_SSDOC_SLOT, SCM_WfPreOpResp},
  { k_NO_SLOT, SCM_WfParamDlResp},
  { k_SNMTM_SLOT, SCM_AssignAddSadr},
  { k_NO_SLOT, SCM_WfAddSadrResp},
  /* Sub-FSM Handle Single UDID Mismatch */
  { k_SNMTM_SLOT, SCM_WfAssignSadrResp2},
  { k_SNMTM_SLOT, SCM_VerifyUniqUdid},
  /* Sub-FSM Activate SN */
  { k_SNMTM_SLOT, SCM_SendPutToOp},
  { k_SSDOC_SLOT, SCM_SendReadTimestamp},
  { k_NO_SLOT, SCM_WfOpResp},
  { k_NO_SLOT, SCM_WfPollTimeout},
  { k_SNMTM_SLOT, SCM_WfSaplAck},
  /* Sub-FSM Node Guarding */
  { k_SNMTM_SLOT, SCM_WfGuardTimer},
  { k_NO_SLOT, SCM_WfGuardResp},
  /* Sub-FSM Additional Parameters */
  {k_SNMTM_SLOT, SCM_SendSaplAck},
  {k_SSDOC_SLOT, SCM_SendAddParam},
  {k_NO_SLOT, SCM_WfAddParamResp}
};

/** Index into FSM control blocks */
static UINT16 w_FsmIdx SAFE_NO_INIT_SEKTOR;
/** signals if SCM is Deactivated */
static BOOLEAN o_Stopped SAFE_NO_INIT_SEKTOR;
/** real number of available safety nodes */
static UINT16 w_SnRealNum SAFE_NO_INIT_SEKTOR;

static BOOLEAN ScmInitFsm(void);
static BOOLEAN SnSodEntryInit(UINT16 w_snNum, UINT16 w_cbIdx, BOOLEAN *po_break,
                              BOOLEAN *po_error);
static void CheckTimeout(SCM_t_FSM_CB *ps_fsmCb, UINT32 dw_ct);
static BOOLEAN ScmFsmPreCon(const SCM_t_FSM_CB *ps_fsmCb,
                            const UINT8 *pb_numFreeFrms);
static BOOLEAN ScmFsmProcess(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                             UINT32 dw_ct);

/**
* @brief This function is only used for testing and gets the pointer to the w_SnRealNum static variable.
*
* @return       pointer to the w_SnRealNum static variable
*/
UINT16 *SCM_GetPtrTo_w_SnRealNum(void)
{
  return &w_SnRealNum;
}

/**
* @brief This function is only used for testing and gets the pointer to the as_FsmStateFunctions static array.
*
* @return       pointer to the as_FsmStateFunctions array
*/
t_FSM_FUNC *SCM_GetFsmStateFunctionsPtr(void)
{
  return as_FsmStateFunctions;
}

/**
* @brief This function initializes the SCM.
*
* The initialization is done via the API function SCM_Activate().
*/
void SCM_Init(void)
{
  o_Stopped = TRUE;    /* SCM is deactivated */
  /* read the initial CT value */
  //TODO: not implemented for now
  //SCM_ddw_initCt = SAPL_SCM_GetInitialCtValueClbk();
  SCM_ddw_initCt = 0;
  /* magic number for 40 bit mask */
  if ( 0 != (0xFFFFFF0000000000ULL & SCM_ddw_initCt))
  {
    /* error: more than 40 bits are used in the random number */
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_ERR_INIT_CT,
                  SERR_k_NO_ADD_INFO);
  }
  dw_lastScmCt = 0;
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
* @brief This function triggers the node configuration respectively the node guarding FSM.
*
* @attention This function must not be called in SNMTS_k_ST_INITIALIZATION state.
* This function must be called with OUT value of number of free frame > 0 at least
* once within the smallest of the following timeouts (SOD index, sub-index)
* - life guarding:
*   - (EPLS_k_IDX_LIFE_GUARDING, EPLS_k_SUBIDX_GUARD_TIME)
* - reset guarding:
*   - (EPLS_k_IDX_PRE_OP_SIGNAL, EPLS_k_SUBIDX_REFRESH_TIME)
*
* @param       dw_ct    consecutive time (not checked, any value allowed)
*  valid range: UINT32
*
* @retval      pb_numFreeFrms
* - IN: number of EPLS frames are free to transmit. (checked),
*   valid range: <> NULL, value > 0
* - OUT: remaining number of free frames. If the value is zero, then not all SN may
*   triggered.
*/
void SCM_Trigger(UINT32 dw_ct, UINT8 *pb_numFreeFrms)
{
  SCM_t_FSM_CB *ps_fsmCb = (SCM_t_FSM_CB *)NULL;  /* pointer to the FSM control
                                                     block */
  BOOLEAN o_break = FALSE;    /* flag to break the while loop */
  UINT16 w_idx = 0U;

  /* if reference to number of free frames is invalid */
  if(pb_numFreeFrms == NULL)
  {
    /* error: reference to NULL */
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_ERR_REF_FREE_FRMS,
                  SERR_k_NO_ADD_INFO);
  }
  else  /* parameter from outside the stack are valid */
  {
    /* increase the initial CT value */
    SCM_ddw_initCt = SCM_ddw_initCt + (dw_ct - dw_lastScmCt);
    dw_lastScmCt = dw_ct;

    /* if SCM is activated */
    if(o_Stopped == FALSE)
    {
      /* call the FSM for all SNs until the number of
        maximum processed nodes per function call is reached */
      while((w_idx < (UINT16)SCM_cfg_NUM_OF_PROCESSED_NODES_PER_CALL) &&
            !o_break)
      {
        /* determine next FSM slot */
        ps_fsmCb = &SCM_as_FsmCb[w_FsmIdx];
        /* check if node may be processed */
        //TODO: not implemented now
        //if (SAPL_ScmProcessSn(ps_fsmCb->w_sadr))
        //{
			/* plausibility check */
			if((UINT16)ps_fsmCb->e_state < (UINT16)SCM_k_NUM_CFG_FSM_STATES)
			{
			  /* check for expired guarding time */
			  CheckTimeout(ps_fsmCb, dw_ct);
			  /* if all preconditions for this slot are fulfilled */
			  if(ScmFsmPreCon(ps_fsmCb, pb_numFreeFrms) == TRUE)
			  {
				/* store for later use via FSM state functions */
				SCM_NumFreeFrmsSet(pb_numFreeFrms);
				/* if FSM Processing failed */
				if (!ScmFsmProcess(ps_fsmCb, w_FsmIdx, dw_ct))
				{
				  o_break = TRUE;
				}
				/* no else : FSM processing succeeded */
			  }
//			}
//			else
//			{
//			  SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_STATE_ERR,
//							(UINT32)ps_fsmCb->e_state);
//			  o_break = TRUE;
//			}
        }

        /* Next FSM will be processed. (w_FsmIdx is incremented in the range
          from 0 to SCM_w_SnRealNum-1) */
        w_FsmIdx = (UINT16)((w_FsmIdx + 1U) % w_SnRealNum);
        w_idx++;
      }
    }
    /* else: SCM is deactivated */
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

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
BOOLEAN SCM_Activate(void)
{
  o_Stopped = FALSE;      /* SCM is activated */
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return ScmInitFsm();        /* init FSM */
}

/**
* @brief This function deactivates the safety configuration manager inclusive the
* node guarding for all nodes.
*
* @see SCM_Activate()
*/
void SCM_Deactivate(void)
{
  o_Stopped = TRUE;       /* SCM is deactivated */
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}


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
void SCM_ResetNodeGuarding(void)
{
  SCM_t_FSM_CB *ps_fsmCb = (SCM_t_FSM_CB *)NULL;  /* pointer to the FSM control
                                                     block */
  UINT16 w_idx;

  for(w_idx = 0U; w_idx < w_SnRealNum; w_idx++)
  {
    ps_fsmCb = &SCM_as_FsmCb[w_idx];

    /* Unchanged SN status shall be reported */
    ps_fsmCb->o_reportSnSts = TRUE;

    /* if guarding timer is running */
    if (ps_fsmCb->e_state == SCM_k_WF_GUARD_TIMER)
    {
      /* signal that time is elapsed */
      EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_TIMEOUT);
    }
    /* else if the state of the SCM is IDLE2 */
    else if (ps_fsmCb->e_state == SCM_k_IDLE2)
    {
      /* signal that time is elapsed */
      EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_TIMEOUT);
    }
    /* else if the state of the SCM is SCM_k_WF_OPERATOR_ACK */
    else if(ps_fsmCb->e_state == SCM_k_WF_OPERATOR_ACK)
    {
      /* signal the reset node guarding event */
      EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_RESET_NODE_GRD);
    }
    else /* else: other state */
    {
      /* reset node guarding is ignored */
    }
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

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
BOOLEAN SCM_OperatorAck(UINT16 w_hdl)
{
  SCM_t_FSM_CB *ps_fsmCb = (SCM_t_FSM_CB *)NULL;  /* pointer to the FSM control
                                                     block */
  BOOLEAN o_res = FALSE;    /* function result */

  if(w_hdl >= (UINT16)SCM_cfg_MAX_NUM_OF_NODES)
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_PARAM1_ERR, (UINT32)w_hdl);
  }
  else
  {
    ps_fsmCb = &SCM_as_FsmCb[w_hdl];
    if(ps_fsmCb->e_state != SCM_k_WF_OPERATOR_ACK)
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_PARAM2_ERR, (UINT32)w_hdl);
    }
    else
    {
      EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_ACK_RECEIVED);
      o_res = TRUE;
    }
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}

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
BOOLEAN SCM_SnFailAck(UINT16 w_hdl, UINT8 b_errGroup, UINT8 b_errCode)
{
  SCM_t_FSM_CB *ps_fsmCb = &SCM_as_FsmCb[w_hdl];  /* pointer to the FSM control
                                                     block */
  BOOLEAN o_res = FALSE;          /* function result */

  /* if parameter is invalid */
  if(w_hdl >= (UINT16)SCM_cfg_MAX_NUM_OF_NODES)
  {
    /* parameter error */
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_PARAM3_ERR, (UINT32)w_hdl);
  }
  else if(ps_fsmCb->e_state != SCM_k_WF_SAPL_ACK)
  {
    /* FSM is in an invalid state */
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_PARAM4_ERR, (UINT32)w_hdl);
  }
  else
  {
    /* store parameters and signal the FSM, that it can go on */
    *ADD_OFFSET(ps_fsmCb->adw_respBuff, 0) = b_errGroup;
    *ADD_OFFSET(ps_fsmCb->adw_respBuff, 1) = b_errCode;
    EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_ACK_RECEIVED);
    o_res = TRUE;
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}

/**
* @brief This function is a callback function which is provided by unit SCM.
*
* The function is called by unit SNMTM to signal a reset guarding was received from a
* specific SN.
*/
void SCM_SNMTM_ResetGuardClbk(void)
{
  SCM_t_FSM_CB *ps_fsmCb = (SCM_t_FSM_CB *)NULL;  /* pointer to the FSM control
                                                     block */
  UINT16 w_idx;

  for(w_idx = 0U; w_idx < w_SnRealNum; w_idx++)
  {
    ps_fsmCb = &SCM_as_FsmCb[w_idx];

    /* if the SCM state is SCM_k_WF_GUARD_TIMER */
    if (ps_fsmCb->e_state == SCM_k_WF_GUARD_TIMER)
    {
      /* signal that time is elapsed */
      EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_TIMEOUT);
    }
    /* else if the SCM state is SCM_k_IDLE2 */
    else if (ps_fsmCb->e_state == SCM_k_IDLE2)
    {
      /* signal that time is elapsed */
      EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_TIMEOUT);
    }
    /* else: guarding timer is not running */
    else
    {
      /* do nothing */
    }
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
* @brief Check in the SADR-UDID-List (index CC01 + x, subindex of the active UDID) if the
* given UDID is not already assigned.
*
* This function is only called in case of ACM (one device single UDID for each SN with a
* specific SADR), therefore only sub-index 1 is checked in the SADR-UDID-List.
*
* @param	pb_newUdid UDID to be checked. (not checked, only called with reference to
* 	array in SCM_VerifyUniqUdid())
*
* @return
* - TRUE                - UDID is unique
* - FALSE               - UDID is not unique
*/
BOOLEAN SCM_UniqueUdid(const UINT8 *pb_newUdid)
{
  BOOLEAN o_uniqueUdid = TRUE;  /* return value */
  UINT16 w_snNum = 0U;          /* SN number in the control block */
  SCM_t_FSM_CB *ps_fsmCb = (SCM_t_FSM_CB *)NULL;  /* pointer to the FSM control
                                                     block */

  /* search through the list for the given UDID */
  do
  {
    ps_fsmCb = &SCM_as_FsmCb[w_snNum];

    /* if UDID is not unique */
    if (MEMCOMP(pb_newUdid, ps_fsmCb->pb_snUdid, EPLS_k_UDID_LEN)
                == MEMCMP_IDENT)
    {
        /* if the UDID is already used */
        if (ps_fsmCb->o_udidUsed)
        {
            o_uniqueUdid = FALSE;
        }
        else
        {
        	/* clear the UDID */
        	(void)MEMSET(ps_fsmCb->pb_snUdid,0,EPLS_k_UDID_LEN);
            /* next SN UDID */
            w_snNum++;
        }
    }
    else
    {
        /* next SN UDID */
        w_snNum++;
    }
  }
  while( (w_snNum < w_SnRealNum) && o_uniqueUdid);

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_uniqueUdid;
}

#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param w_errorCode 	error number
* @param dw_addInfo 	additional error information
* @retval pac_str 		empty buffer to build the error string.
*/
  void SCM_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str)
  {
    switch(w_errorCode)
    {
      case SCM_k_PARAM1_ERR:
      {
        /* Error string */
        SPRINTF2(pac_str,
                "%#x - SCM_k_PARAM1_ERR: SCM_OperatorAck():\n"
                "Invalid parameter 'w_hdl' (%u).\n",
                SCM_k_PARAM1_ERR, (UINT16)(dw_addInfo));
        break;
      }
      case SCM_k_PARAM2_ERR:
      {
        /* Error string */
        SPRINTF2(pac_str,
                "%#x - SCM_k_PARAM2_ERR: SCM_OperatorAck():\n"
                "Invalid FSM state (%u).\n", SCM_k_PARAM2_ERR,
                (UINT16)(dw_addInfo));
        break;
      }
      case SCM_k_PARAM3_ERR:
      {
        /* Error string */
        SPRINTF2(pac_str,
                "%#x - SCM_k_PARAM3_ERR: SCM_SnFailAck():\n"
                "Invalid parameter 'w_hdl' (%u).\n", SCM_k_PARAM3_ERR,
                (UINT16)(dw_addInfo));
        break;
      }
      case SCM_k_PARAM4_ERR:
      {
        /* Error string */
        SPRINTF2(pac_str,
                "%#x - SCM_k_PARAM4_ERR: SCM_SnFailAck():\n"
                "Invalid FSM state (%u).\n", SCM_k_PARAM4_ERR,
                (UINT16)(dw_addInfo));
        break;
      }
      case SCM_k_UNVALID_EVENT_ERR:
      {
        /* Error string */
        SPRINTF2(pac_str,
                "%#x - SCM_k_UNVALID_EVENT_ERR: ScmFsmProcess():\n"
                "Invalid event occurred (%Xh).\n", SCM_k_UNVALID_EVENT_ERR,
                (UINT8)dw_addInfo);
        break;
      }
      case SCM_k_STATE_ERR:
      {
        /* Error string */
        SPRINTF2(pac_str,
                "%#x - SCM_k_STATE_ERR: SCM_Trigger():\n"
                "FSM state error (%u).\n", SCM_k_STATE_ERR,
                (UINT8)dw_addInfo);
        break;
      }
      case SCM_k_SOD_READ_ERR:
      {
        /* Error string */
        SPRINTF1(pac_str,
                "%#x - SCM_k_SOD_READ_ERR: ScmInitFsm():\n"
                "SOD read error.\n", SCM_k_SOD_READ_ERR);
        break;
      }
      case SCM_k_SOD_WRITE_ERR:
      {
        /* Error string */
        SPRINTF1(pac_str,
                "%#x - SCM_k_SOD_WRITE_ERR: SCM_SodWrite():\n"
                "SOD write error in the function.\n", SCM_k_SOD_WRITE_ERR);
        break;
      }
      case SCM_k_INVALID_PRECOND:
      {
        /* Error string */
        SPRINTF2(pac_str,
                "%#x - SCM_k_INVALID_PRECOND: ScmFsmPreCon():\n"
                "Invalid precondition (%u).\n", SCM_k_INVALID_PRECOND,
                (UINT8)dw_addInfo);
        break;
      }
      case SCM_k_INVALID_SNMT_STATE:
      {
        /* Error string */
        SPRINTF2(pac_str,
                "%#x - SCM_k_INVALID_SNMT_STATE: SCM_WfOpResp():\n"
                "Invalid SNMT State (%u) received from SNMTM.\n",
                SCM_k_INVALID_SNMT_STATE, (UINT8)dw_addInfo);
        break;
      }
      case SCM_k_INVALID_SNMT_STATE1:
      {
        /* Error string */
        SPRINTF2(pac_str,
                "%#x - SCM_k_INVALID_SNMT_STATE1: SCM_WfGuardResp():\n"
                "Invalid SNMT State (%u) received from SNMTM.\n",
                SCM_k_INVALID_SNMT_STATE1, (UINT8)dw_addInfo);
        break;
      }
      case SCM_k_OBJ_CLBK_ERR:
      {
        /* Error string */
        SPRINTF2(pac_str,
                "%#x - SCM_k_OBJ_CLBK_ERR: SCM_SodPtrAcs():\n"
                "Callback function must not be implemented for "
                "the object with SOD index (%Xh).\n",
                SCM_k_OBJ_CLBK_ERR, (UINT16)dw_addInfo);
        break;
      }
      case SCM_k_ERR_REF_FREE_FRMS:
      {
        /* Error string */
        SPRINTF1(pac_str,
                "%#x - SCM_k_ERR_REF_FREE_FRMS: SCM_Trigger():\n"
                "Invalid reference to the number of free frames.\n",
                SCM_k_ERR_REF_FREE_FRMS);
        break;
      }
      default:
      {
        SPRINTF(pac_str, "Unknown error code was generated by the SCM\n");
        break;
      }
    }
    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
  }
#endif


/**
* @brief This function initializes all slots of the configuration FSM.
*
* @return
* - TRUE - function successfully finished
* - FALSE - abort forcing error
*/
static BOOLEAN ScmInitFsm(void)
{
  SCM_t_FSM_CB *ps_fsmCb = (SCM_t_FSM_CB *)NULL;  /* pointer to the FSM control
                                                     blocks */
  BOOLEAN o_ret = FALSE;    /* return value */
  BOOLEAN o_error = TRUE;   /* for SCM_SodQueryRead errors */
  BOOLEAN o_break = FALSE;  /* for SN SADR Init */
  UINT16 w_snNum = 0U;      /* counter to increment the SOD index */
  UINT32 i;                 /* loop counter */
  void *pv_data = NULL;     /* pointer to the data for SCM_SodQueryRead */

  SCM_NumFreeFrmsSet((UINT8 *)NULL);

  /* initialization of variables relevant for all nodes */
  w_SnRealNum = 0U;
  w_FsmIdx = 0U; /* reset index into the FSM for SCM_Trigger() */

  /* read the SN guard time from the SOD (0x100C, 0x01)*/
  pv_data = SCM_SodRead(SCM_k_IDX_GUARD_TIME);
  /* if SOD Read succeeded */
  if(pv_data != NULL)
  {
    SCM_dw_SnGuardTime = *(PTR_TYPE*)pv_data;

    /* read the SN life time factor the SOD (0x100C, 0x02)*/
    pv_data = SCM_SodRead(SCM_k_IDX_LIFE_TIME_FACTOR);
    /* if SOD Read succeeded */
    if(pv_data != NULL)
    {
      SCM_dw_SnGuardTimeTotal = (*(UINT8*)pv_data) * SCM_dw_SnGuardTime;

      /* read the SCM configuration mode from the SOD (0x101B, 0x01)*/
      pv_data = SCM_SodRead(SCM_k_IDX_CONFIG_MODE);

      /* if SOD Read succeeded */
      if(pv_data != NULL)
      {
        SCM_b_ConfigMode = *(UINT8*)pv_data;

        /* if function call succeeded */
        if (SCM_BuildLocalSadrList())
        {
          o_error = FALSE;
        }
      }
      /* no else : SOD Read failed */
    }
    /* no else : SOD Read failed */
  }
  /* no else : SOD Read failed */

  /* initialization of variables in the SCM control block */
  while((w_snNum < (UINT16)SCM_cfg_MAX_NUM_OF_NODES) && !o_break && !o_error)
  {
    /* if the initialization of SOD objects succeeded */
    if (SnSodEntryInit(w_snNum, w_SnRealNum, &o_break, &o_error))
    {
      /* occupy FSM slot */
      ps_fsmCb = &SCM_as_FsmCb[w_SnRealNum];

      /** Variables to store the state machine information for a SN **/
      ps_fsmCb->e_state = SCM_k_SEND_ASSIGN_SADR_REQ;
      ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      ps_fsmCb->b_snStatus = SCM_k_NS_MISSING;
      ps_fsmCb->o_reportSnSts = TRUE;
      ps_fsmCb->dw_timer = 0UL;
      ps_fsmCb->dw_timerTotal = 0UL;
      ps_fsmCb->w_addSadr = 0x0000U;

      /** Variables to store the SN response information **/
      ps_fsmCb->dw_SsdocAbortCode = 0UL;
      ps_fsmCb->dw_SsdocUploadLen = 0UL;
      ps_fsmCb->w_tadr = 0x0000U;
      ps_fsmCb->w_rxSdn = 0x0000U;
      for(i = 0U; i< (UINT32)((SCM_k_RESP_BUFF_SIZE/4U)+1U); i++)
      {
        ps_fsmCb->adw_respBuff[i] = 0UL;
        ps_fsmCb->adw_newUdid[i]  = 0UL;
      }

      w_SnRealNum++;
    }
    /* no else : initialization of the SN SADR failed */

    /* next entry in the SADR-DVI list */
    w_snNum++;
  }

  /* if no error happened */
  if (!o_error)
  {
    /* if the initialization succeeded */
    if (w_SnRealNum > 0U)
    {
      o_ret = TRUE;
    }
    else /* SOD read error */
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_SOD_READ_ERR,
                    SERR_k_NO_ADD_INFO);
    }
  }
  /* no else : error happened */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_ret;
}


/**
* @brief This function reads the SADR, the poll interval, the maximum SSDO payload size
* and the place holder for the remote timestamp and crc domain for every SN from the SOD
* and stores in the SCM control block.
*
* @param w_snNum 	index in the SADR-DVI list (0xC400-0xC7FE)(not checked, checked in in ScmInitFsm()), valid range : 0 .. SCM_cfg_MAX_NUM_OF_NODES -1
*
* @param w_cbIdx  	actual index in the SCM control block array(not checked, checked in in ScmInitFsm()), valid range : 0 .. SCM_cfg_MAX_NUM_OF_NODES -1
*
* @param po_break	only relevant if the return value is FALSE
* - TRUE  : break the initialization
* - FALSE : continue the initialization with the next index in the SADR-DVI list (not checked, only called with reference in ScmInitFsm()).
*
* @retval po_error
* - TRUE  : error happened
* - FALSE : no error happened (not checked, only called with reference in ScmInitFsm()).
*
* @return
* - TRUE           - the SN SADR was initialized in the SCM control block
* - FALSE          - the SN SADR was not initialized in the SCM control block (see po_break)
*/
static BOOLEAN SnSodEntryInit(UINT16 w_snNum, UINT16 w_cbIdx, BOOLEAN *po_break,
                              BOOLEAN *po_error)
{
  BOOLEAN o_ret = FALSE;             /* return value */
  UINT16 *pw_sadr = (UINT16 *)NULL;  /* pointer to the SN SADR */
  UINT16 *pw_payLd = (UINT16 *)NULL; /* pointer to the max. SSDO payload size */
  UINT32 *pdw_pollInt = (UINT32 *)NULL; /* pointer to the poll interval */
  SCM_t_FSM_CB *ps_fsmCb = &SCM_as_FsmCb[w_cbIdx]; /* pointer to the FSM control
                                                      blocks */

  /* read SN SADR from the SADR-DVI List in the SOD (0xC400-0xC7FE, 0x01) */
  pw_sadr = (UINT16 *)SCM_SodQueryRead(SCM_k_IDX_SADR(w_snNum), po_error);
  if(pw_sadr != NULL)
  {
    *po_break = FALSE;

    /* if the SN SADR is used */
    if ((*pw_sadr <= (UINT16)SCM_cfg_MAX_SADR_VALUE) &&
        (*pw_sadr >= EPLS_k_MIN_SADR))
    {
      /* read the maximum payload length from the SADR-DVI List in the SOD
         (0xC400-0xC7FE, 0x08) */
      pw_payLd = (UINT16 *)SCM_SodRead(SCM_k_IDX_MAX_SSDO_PAYLD(w_snNum));
      if(pw_payLd != NULL)
      {
        /* read SnmtCrcPollInterval_U32 from the SADR-DVI List in the SOD
           (0xC400-0xC7FE, 0x09) */
        pdw_pollInt = (UINT32 *)SCM_SodRead(SCM_k_IDX_POLL_INTERVALL(w_snNum));
        if(pdw_pollInt != NULL)
        {
          /* always the UDID with sub-index 1 in the SADR-UDID-List is used for
             the SADR assignment at the first time */

          /* read and store the pointer to the UDID of the SN from the SADR-UDID
             List in the SOD (0xCC01 0x01) */
          ps_fsmCb->pb_snUdid =
                    (UINT8 *)SCM_SodPtrAcs(SCM_k_IDX_UDID_LIST((*pw_sadr), 1U));
          if(ps_fsmCb->pb_snUdid != NULL)
          {
        	  //o_ret = TRUE; //TODO: not original...delete
        	/* read the store pointer for the remote timestamp and crc domain */
            ps_fsmCb->pb_remTimeCrc =
                   (UINT8 *)SCM_SodPtrAcs(SCM_k_IDX_REM_PARA_CHKSM(w_snNum));
            if(ps_fsmCb->pb_remTimeCrc != NULL)
            {
              /* read the maximum allowed length */
              ps_fsmCb->dw_maxRemTimeCrcLen = SCM_SodGetActLen(
            		  SCM_k_IDX_REM_PARA_CHKSM(w_snNum));
              /* all SN UDID are unused after the initialization */
              ps_fsmCb->o_udidUsed = FALSE;
              /* store the SN SARD in the SCM control block */
              ps_fsmCb->w_sadr = *pw_sadr;
              /* store the max. SSDO payload data for the SN */
              ps_fsmCb->w_payLd = *pw_payLd;
              /* store the poll interval for the SN */
              ps_fsmCb->dw_pollInterval = *pdw_pollInt;

              o_ret = TRUE;
            }
            else /* SOD error */
            {
              *po_error = TRUE;
            }
          }
          else /* SOD error */
          {
            *po_error = TRUE;
          }
        }
        else /* SOD error */
        {
          *po_error = TRUE;
        }
      }
      else /* SOD error */
      {
        *po_error = TRUE;
      }
    }
    /* no else : ignore this SADR-DVI entry and check the next one */
  }
  else /* no more SN SADR is available */
  {
    *po_break = TRUE;
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_ret;
}


/**
* @brief This function checks if the SN guarding or polling time is elapsed and sets the
* corresponding event flag if necessary.
*
* @param ps_fsmCb	pointer to the current slot of the FSM control block (not checked, only called with reference in SCM_Trigger()).
*
* @param dw_ct 		consecutive time (not checked, any value allowed), valid range: UINT32
*/
static void CheckTimeout(SCM_t_FSM_CB *ps_fsmCb, UINT32 dw_ct)
{
  /* Hans Pill: also the waiting for operator acknowledge if SN has failed does have a timeout */
  /* if guarding or poll timer running */
  if((ps_fsmCb->e_state == SCM_k_WF_GUARD_TIMER) ||
     (ps_fsmCb->e_state == SCM_k_WF_POLL_TIMEOUT) ||
     (ps_fsmCb->e_state == SCM_k_IDLE2) ||
     (ps_fsmCb->e_state == SCM_k_WF_SAPL_ACK))
  {
    /* if the timer elapsed */
    if(EPLS_TIMEOUT(dw_ct, ps_fsmCb->dw_timer))
    {
      /* signal timeout event to the FSM */
      EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_TIMEOUT);
    }
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
* @brief This function returns TRUE, if all preconditions for the execution of the FSM in
* the current state are fulfilled.
*
* Preconditions are:
* - available SNMTM/SSDOC slot
* - pb_numFreeFrms > 0
*
* Only if these preconditions are fulfilled, the current FSM state function will be called.
*
* @param ps_fsmCb			pointer to the current slot of the FSM control block (not checked, only called with reference in SCM_Trigger()).
*
* @param pb_numFreeFrms 	number of available free frames for management messages (not checked, checked in SCM_Trigger())) valid range: UINT8.
*
* @return
* - TRUE           - All preconditions are fulfilled.
* - FALSE          - The preconditions are not fulfilled.
*/
static BOOLEAN ScmFsmPreCon(const SCM_t_FSM_CB *ps_fsmCb,
                            const UINT8 *pb_numFreeFrms)
{
  BOOLEAN o_res = FALSE;    /* function result */

  /* determine the type of state function */
  UINT8 b_preCon = as_FsmStateFunctions[ps_fsmCb->e_state].b_preCon;

  /* if function needs a SNMT master slot */
  if(b_preCon == k_SNMTM_SLOT)
  {
    /* if FSM slot and frame block available */
    if(SNMTM_CheckFsmAvailable() && (*pb_numFreeFrms > 0U))
    {
      o_res = TRUE;
    }
    /* no else : preconditions are not fulfilled */
  }
  /* else if function needs a SSDO client slot */
  else if(b_preCon == k_SSDOC_SLOT)
  {
    /* if FSM slot and frame block available */
    if(SSDOC_CheckFsmAvailable() && (*pb_numFreeFrms > 0U))
    {
      o_res = TRUE;
    }
    /* no else : preconditions are not fulfilled */
  }
  /* else if function needs a SSDO client slot or a SNMT master slot*/
  else if(b_preCon == k_SSDOC_SNMTM_SLOT)
  {
    /* if FSM slot and frame block available */
    if((SSDOC_CheckFsmAvailable() && (*pb_numFreeFrms > 0U)) &&
       (SNMTM_CheckFsmAvailable() && (*pb_numFreeFrms > 0U)))
    {
      o_res = TRUE;
    }
    /* no else : preconditions are not fulfilled */
  }
  /* else if function needs no slot */
  else if(b_preCon == k_NO_SLOT)
  {
    o_res = TRUE;
  }
  else /* invalid precondition */
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_INVALID_PRECOND, (UINT32)b_preCon);
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}

/**
* @brief This function processes one slot of the configuration manager FSM.
*
* Based on the FSM state the corresponding FSM state function will be called.
*
* @param ps_fsmCb        pointer to the current slot of the FSM control block (not checked, only called with reference in SCM_Trigger()).
*
* @param w_snNum         FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). valid range: 0..SCM_cfg_MAX_NUM_OF_NODES-1
*
* @param dw_ct           consecutive time (not checked, any value allowed). valid range: UINT32
*
* @return
* - TRUE          - function successfully finished
* - FALSE         - abort forcing error
*/
static BOOLEAN ScmFsmProcess(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                             UINT32 dw_ct)
{
  BOOLEAN o_res = TRUE;             /* function result */

  /* if an event occurs */
  if(ps_fsmCb->w_event != SCM_k_EVT_NO_EVT_OCCURRED)
  {

    SCM_PRINT3("events[%u] = %x", w_snNum, ps_fsmCb->w_event);
    /* call the corresponding state function */
    o_res = as_FsmStateFunctions[ps_fsmCb->e_state].pf_state(ps_fsmCb,
                                                             w_snNum, dw_ct);
    /* if no error occurred but any event is still pending
        (with the exception of the "generic event" and "ACK event") */
    if((o_res == TRUE) && (ps_fsmCb->w_event != SCM_k_EVT_NO_EVT_OCCURRED) &&
       (ps_fsmCb->w_event != SCM_k_EVT_GENERIC_EVENT) &&
       (ps_fsmCb->w_event != SCM_k_EVT_ACK_RECEIVED))
    {
      /* unexpected event occurred */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_UNVALID_EVENT_ERR,
                   (UINT32)((ps_fsmCb->w_event<<16) |  ps_fsmCb->w_sadr));

      ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      ps_fsmCb->e_state = SCM_k_SEND_ASSIGN_SADR_REQ;
      o_res = FALSE;
    }
    /* no else : event processing succeeded */

    SCM_PRINT3(" -> state[%u] = %u\n", w_snNum, ps_fsmCb->e_state);
  }
  /* no else: no FSM event occurred */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}
/** @} */
