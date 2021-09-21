/**
 * @addtogroup SCM
 * @{
 * @file SCMverParam.c
 *
 * FSM state functions for the process "Verify Parameters".
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
 * <h2>History for SCMverParam.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SLV10</td></tr>
 *     <tr><td>07.03.2013</td><td>Hans Pill</td><td>changes for parameter checksum being written to the SN</td></tr>
 *     <tr><td>23.03.2013</td><td>Hans Pill</td><td>changes for handling of the new timestamp and CRC domain</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>24.06.2014</td><td>Hans Pill</td><td>verify the length of the uploaded timestamp and CRC parameter</td></tr>
 *     <tr><td>21.07.2016</td><td>Stefan Innerhofer</td><td>changes for legacy mode time stamp</td></tr>
 *     <tr><td>23.02.2017</td><td>Christopher Neuwirt and Stefan Innerhofer</td><td>changes to allow a change of the number of parameter CRCs</td></tr>
 * </table>
 *
 * @addtogroup fsm_verify_param FSM - Verify Parameters
 * @{
 */

#include "SCMint.h"

/**
* @brief This function represents the state SCM_k_WF_TIMESTAMP of the finite state machine ScmFsmProcess().
*
* @param         ps_fsmCb       Pointer to the current slot of the FSM control block (not checked, only called with reference to struct in ScmFsmProcess()).
*
* @param        w_snNum         FSM slot number (=> index to the DVI list)(not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param        dw_ct           Consecutive time (not checked, any value allowed). Valid range: UINT32
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_WfTimestamp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;                 /* function result */
  UINT8 *pb_timeCrc = (UINT8 *)NULL;     /* pointer to the timestamp and CRC domain */
  UINT32 dw_timeCrcLen = 0;              /* length of the timestamp and CRC domain */
  UINT32 *pdw_optFeat = (UINT32*)NULL;   /* pointer to the optional feature flags*/

  /* Depending if the "legacy mode time stamp" bit is set in the optional features
   * only the parameter timestamp or the parameter CRC has to be checked.
   * Assume the bit is not set and pre select the required parameters accordingly*/
  uint16_t w_indexToRead = SCM_k_IDX_DVI + w_snNum;        /* index where to read the timestamp or CRC from*/
  uint8_t  b_subIndexToRead = SCM_k_SUB_IDX_DVI_CHECK_SUM; /* subindex where to read the check sum from*/
  UINT8 *pb_memToComp = ps_fsmCb->pb_remTimeCrc;           /* point to the memory where the compare data is stored*/

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SSDOC_RESP_RX)
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    /* read the optional reatures*/
    pdw_optFeat = (UINT32*)SCM_SodRead(SCM_k_IDX_OPT_FEAT(w_snNum));

    /* if the "legacy mode time stamp" bit is set in the optional features
     * change subindex to read, and pointer to compare*/
    if((pdw_optFeat != NULL) &&
      ((*pdw_optFeat & SCM_k_FEATURE_LEGACY_MODE_TS) != 0))
    {
      b_subIndexToRead = SCM_k_SUB_IDX_DVI_TIMESTAMP;
      pb_memToComp = (UINT8*)ps_fsmCb->adw_respBuff;
    }

    /*read the data from the SOD*/
    pb_timeCrc = (UINT8 *)SCM_SodRead(w_indexToRead,b_subIndexToRead);

    if(pb_timeCrc != NULL)
    {
      /* try to read the parameter set length */
      dw_timeCrcLen = SCM_SodGetActLen(w_indexToRead,b_subIndexToRead);

      /* compare the CRC only if received size is the same as the length of the compare buffer*/
      if ((dw_timeCrcLen == ps_fsmCb->dw_SsdocUploadLen) &&
               (0 == MEMCOMP(pb_timeCrc,pb_memToComp,dw_timeCrcLen)))
      {
        ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
        ps_fsmCb->e_state = SCM_k_SEND_PUT_TO_OP;
        o_res = TRUE;
      }
      /*in any other case the SN has be be reconfigured*/
      else
      {
        /* set SN to PRE_OP */
        o_res = SNMTM_ReqSnTrans(dw_ct, w_snNum, ps_fsmCb->w_sadr,
                                 SNMTM_k_OP_TO_PREOP, 0U);

        /*if there is a size mismatch set a logger entry*/
		if ((dw_timeCrcLen != ps_fsmCb->dw_SsdocUploadLen))
		{
			SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_PARA_CHKSM_SIZE_INFO,
									ps_fsmCb->dw_SsdocUploadLen | ((UINT32)w_snNum << 16));
		}

        /* if no error happened */
        if (o_res)
        {
          ps_fsmCb->e_state = SCM_k_WF_PRE_OP_RESP;
          SCM_NumFreeFrmsDec();
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
      /* CRC could not get loaded because the object length does not match */
      if (SOD_ABT_LEN_DOES_NOT_MATCH == ps_fsmCb->dw_SsdocAbortCode)
      {
        SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_PARA_CHKSM_SIZE,
                      ps_fsmCb->dw_SsdocUploadLen | ((UINT32)w_snNum << 16));
      }
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

/* @} */
/* @} */
