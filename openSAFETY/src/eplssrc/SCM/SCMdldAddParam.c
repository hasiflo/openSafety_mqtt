/**
 * @addtogroup SCM
 * @{
 * @file SCMdldAddParam.c
 *
 * FSM state functions for the process "Download Additional Parameters".
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author Hans Pill, LaRSys Automation GmbH
 *
 * <h2>History for SCMdldAddParam.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>18.01.2013</td><td>Hans Pill</td><td>created</td></tr>
 *     <tr><td>07.06.2013</td><td>Hans Pill</td><td>change for CUnit / CTC test</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>06.02.2016</td><td>Stefan Innerhofer</td><td>use sadr instead of snNum to access additional parameter</td></tr>
 * </table>
 *
 * @addtogroup fsm_download_add_parameter FSM - Download Additional Parameters
 * @{
 *
 */

#include "SCMint.h"
#include "SNMTSapi.h"

/**
 * Header structure for additional parameters
 */
typedef struct
{
    /** index of the domain */
    UINT8 b_id;
    /** version of the header */
    UINT8 b_version;
    /** SADR of the device to be loaded to */
    UINT16 w_sadr;
    /** size of the data (excluding the header) */
    UINT32 dw_size;
    /** crc */
    UINT32 dw_crc;
    /** data creation timestamp */
    UINT32 dw_timestamp;
}SCM_t_ADD_PAR_HEADER;

/** allowed header version */
#define SCM_k_ADD_PARAM_VERSION		1

/**
* @brief This function represents the state SCM_k_SEND_SAPL_ACK of the finite state
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
BOOLEAN SCM_SendSaplAck(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;        /* function result */

  /* check occurred event */
  if ( EPLS_IS_BIT_SET(ps_fsmCb->w_event,~(SCM_k_EVT_GENERIC_EVENT)))
  {
    /* error is generated in ScmFsmProcess() */
    o_res = TRUE;
  }
  else
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;
    /* the error to be acknowledged is stored in the fsmCb variable */
    o_res = SNMTM_SnErrorAck(ps_fsmCb->w_sadr,
  		  ps_fsmCb->b_errGroup,
  		  ps_fsmCb->b_errCode);
    /* if no error happened */
    if (o_res)
    {
	  ps_fsmCb->e_state = SCM_k_SEND_ADD_PAR;
      ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
      SCM_NumFreeFrmsDec();
    }
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}
/**
* @brief This function represents the state SCM_k_SEND_ADD_PAR of the finite state
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
BOOLEAN SCM_SendAddParam(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;        /* function result */
  BOOLEAN o_ok = FALSE;			/* function is OK */
  UINT8 *pb_paraSet = (UINT8*)NULL;    /* pointer to parameter set for SOD read
                                          accesses */
  UINT32 dw_paraSetLen = 0UL;          /* length of the additional parameter set */
  UINT16 w_sadrParaSet = (UINT16)0UL;
  SSDOC_t_REQ s_transInfo;             /* used for SSDOC_ReadWriteReq() */

  /* check occurred event */
  if ( EPLS_IS_BIT_SET(ps_fsmCb->w_event,~(SCM_k_EVT_GENERIC_EVENT)))
  {
    /* error is generated in ScmFsmProcess() */
    o_res = TRUE;
    o_ok = TRUE;
  }
  else
  {
    /* reset the event */
    ps_fsmCb->w_event = SCM_k_EVT_NO_EVT_OCCURRED;

    /* try to read the parameter set */
    pb_paraSet = (UINT8 *)SCM_SodRead(SCM_k_IDX_ADD_PARA_SET(ps_fsmCb->w_sadr,ps_fsmCb->b_errCode));

    if(pb_paraSet != NULL)
    {
	  /* try to read the parameter set length */
      dw_paraSetLen = SCM_SodGetActLen(SCM_k_IDX_ADD_PARA_SET(ps_fsmCb->w_sadr,ps_fsmCb->b_errCode));
      /* if parameters are long enough */
      if(dw_paraSetLen >= sizeof(SCM_t_ADD_PAR_HEADER))
      {
        /* read SADR from the parameter set - may be not aligned -> use memcopy */
        (void)MEMCOPY(&w_sadrParaSet,&((SCM_t_ADD_PAR_HEADER*)pb_paraSet)->w_sadr,sizeof(UINT16));
        /* check for valid parameter version */
        if ( SCM_k_ADD_PARAM_VERSION != ((SCM_t_ADD_PAR_HEADER*)pb_paraSet)->b_version)
        {
      	  SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_ADD_PARAM1_ERR,
        			SCM_k_ADD_PARA_ERR_INFO(ps_fsmCb->w_sadr,ps_fsmCb->b_errCode));

        }
        /* additional parameters are not for this SADR */
        else if (w_sadrParaSet != ps_fsmCb->w_sadr)
        {
          SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_ADD_PARAM2_ERR,
                SCM_k_ADD_PARA_ERR_INFO(ps_fsmCb->w_sadr,ps_fsmCb->b_errCode));
        }
        else
        {
	      s_transInfo.w_idx = 0x101AU;
	      s_transInfo.b_subIdx = 0U;
	      s_transInfo.e_dataType = EPLS_k_DOMAIN;
	      /* the amount of payload data is extended because slim SSDO traffic is to be used
	     * payload data is only in subframe 1 */
	      s_transInfo.b_payloadLen = (UINT8) ((ps_fsmCb->w_payLd - 1)*2);
	      s_transInfo.pb_data = pb_paraSet;
	      if ( SNMTS_k_FAIL_ERR_ADD_HEAD_MASK == (ps_fsmCb->b_errCode & SNMTS_k_FAIL_ERR_ADD_HEAD_MASK))
	      {
	  	    s_transInfo.dw_dataLen = dw_paraSetLen;
	      }
	      else
	      {
	  	    s_transInfo.dw_dataLen = sizeof(SCM_t_ADD_PAR_HEADER);
	      }
	      o_ok = TRUE;
          o_res = SSDOC_SendWriteReq(ps_fsmCb->w_sadr, w_snNum, SCM_SsdocClbk,
	  								 dw_ct, &s_transInfo, TRUE);

	      /* if no error happened */
	      if (o_res)
	      {
	        ps_fsmCb->e_state = SCM_k_WF_ADD_PAR_DL_RESP;
	  	    SCM_NumFreeFrmsDec();
          }
          /* no else : error happened */
        }
      }
      /* additional parameters are too short */
      else
      {
      	SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_ADD_PARAM3_ERR,
      			SCM_k_ADD_PARA_ERR_INFO(ps_fsmCb->w_sadr,ps_fsmCb->b_errCode));

      }
    }
    else
    {
    	SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_ADD_PARAM4_ERR,
    			SCM_k_ADD_PARA_ERR_INFO(ps_fsmCb->w_sadr,ps_fsmCb->b_errCode));
    }
    /* if any error happened */
    if (!o_ok)
    {
      /* set node state to wrong Parameters */
      o_res = SCM_SetNodeStatus(ps_fsmCb, w_snNum, SCM_k_NS_WRONG_ADD_PARAM,
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
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}
/**
* @brief This function represents the state SCM_k_WF_ADD_PAR_DL_RESP of the finite
* state machine ScmFsmProcess().
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
BOOLEAN SCM_WfAddParamResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct)
{
  BOOLEAN o_res = FALSE;        /* function result */

  /* if response received */
  if(ps_fsmCb->w_event == SCM_k_EVT_SSDOC_RESP_RX)
  {
    /* reset the event and set the new event */
    ps_fsmCb->w_event = SCM_k_EVT_GENERIC_EVENT;
    ps_fsmCb->e_state = SCM_k_SEND_PUT_TO_OP;
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

/** @} */
/** @} */
