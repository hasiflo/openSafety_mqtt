/**
 * @addtogroup SNMTM
 * @{
 *
 * @file SNMTMmain.c
 * The file contains functionality to initialize the unit, to process a received
 * openSAFETY frame and to repeat unacknowledged SNMT Service Requests.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * Further more this file provides error strings that describe all reported errors
 * of the unit.
 *
 * <h2>History for SNMTM.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>changes for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
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
#include "SNMT.h"
#include "SNMTMapi.h"
#include "SNMTM.h"
#include "SNMTMint.h"
#include "SNMTMerr.h"

/**
 * SOD access to entry 0x1200, 0x02, the main SADR of the Safety Configuration Manager.
*/
SOD_t_ACS_OBJECT_VIRT SNMTM_s_AccessSadrScm SAFE_NO_INIT_SEKTOR;

static BOOLEAN IsDsadrScmSadr(UINT16 w_rxDsadr);

/**
* @brief This function initializes all module global and global variables of the
* SNMT Master.
*
* Every SNMT Master FSM is reset into a initial state to be available to process
* communication with a single SNMT Slave.
*
* @return
* - TRUE  - initialization succeed
* - FALSE - initialization failed
*/
BOOLEAN SNMTM_Init(void)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */


  SNMTM_s_AccessSadrScm.s_obj.w_index = EPLS_k_IDX_COMMON_COM_PARAM;
  SNMTM_s_AccessSadrScm.s_obj.b_subIndex = EPLS_k_SUBIDX_SADR_SCM;
  SNMTM_s_AccessSadrScm.dw_segOfs = 0;
  SNMTM_s_AccessSadrScm.dw_segSize = 0;

  /* if write access to the SOD entry NOT possible */
  if(NULL == SOD_AttrGetVirt(EPLS_k_SCM_INST_NUM_ &SNMTM_s_AccessSadrScm, &s_errRes))
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* SADR of SCM is available in the SOD */
  {
    /* initialize the SNMTM state machine */
    o_return = SNMTM_InitFsm();
  }
  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function handles service request repetitions in case of a precedent service
* request was not responded by the specified SNMT Slave in time or not responded at all.
*
* Generally this function checks every SNMT Master FSM pausing in state "wait for response"
* for response timeout.
*
* @attention This function must not be called in SNMTS_k_ST_INITIALIZATION state. This
* function must be called at least once within the SNMT timeout (SOD index EPLS_k_IDX_SSDO_COM_PARAM,
* sub-index EPLS_k_SUBIDX_SNMT_TIMEOUT) with OUT value of number of free frame > 0.
*
* @param        dw_ct                     consecutive time (not checked any value allowed), valid range: any 32 bit value
*
* @retval       pb_numFreeFrms
* - IN: number of EPLS frames are free to transmit. (checked), valid range: <> NULL, value > 0
* - OUT: remaining number of free frames. If the value is zero, then not all SSDO channel may checked.
*/
void SNMTM_BuildRequest(UINT32 dw_ct, UINT8 *pb_numFreeFrms)
{
  BOOLEAN o_break = FALSE;   /* flag to break the while loop */
  UINT16 w_fsmNum = 0x0000u;  /* start value of loop counter through the list
                                 of FSM */

  /* if reference to number of free frames is invalid */
  if(pb_numFreeFrms == NULL)
  {
    /* error: reference to NULL */
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_REF_FREE_FRMS,
                  SERR_k_NO_ADD_INFO);
  }
  else  /* parameter from outside the stack are valid */
  {
    /* linear search through the list of FSMs, scanning for SNMT Master FSM
      waiting for response */
    do
    {
      /* if FSM processing in state "wait on response" fails */
      if(!(SNMTM_ProcessFsm(dw_ct, pb_numFreeFrms, w_fsmNum,
              k_EVT_CHK_TIMEOUT, 0U, (EPLS_t_FRM_HDR *)NULL, (UINT8 *)NULL)))
      {
        /* error: request could NOT be processed in FSM,
                  error is already reported */
        o_break = TRUE;  /* leave while loop ahead of time */
      }
      else  /* request processed sucessfully */
      {
        w_fsmNum++;       /* increment loop counter */
      }
    }
    while((w_fsmNum < SNMTM_cfg_MAX_NUM_FSM) && !o_break);
    /* (loop, while loop counter is less max. number of FSM)  AND
      (loop, while no error occures) */
  }

  SCFM_TACK_PATH();
}

/**
* @brief This function processes a single Service Response that was identified by the
* control unit (SSC) as a Network Management Response from a SNMT Slave.
*
* The SNMT Master can only process SNMT Slave Service Responses. To pass the requested
* info from the SNMT Master to the SCM, callback functions are called by the SNMT Master.
* The callback functions has to be provided by the SCM. This function searches for the
* matching SNMT Master FSM which is waiting for the received service response and passes
* the service response to the matching FSM. In case of receiving a service response of
* type SNMT_SN_status_PRE_OP, a special callback function is used to inform the SCM or
* the application about the SN status, because no precedent service request initiated
* this type of SNMT Service Response.
*
* @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        ps_respHdr         reference to response header information (not checked, only called with refernece to struct in processStateReqProc()), valid range: <> NULL
*
* @param        pb_respData        reference to response data (not checked, only called with refernece to struct in processStateReqProc()), valid range: <> NULL
*
* @return
* - TRUE             - response successfully processed
* - FALSE            - response processing failed
*/
BOOLEAN SNMTM_ProcessResponse(UINT32 dw_ct, const EPLS_t_FRM_HDR *ps_respHdr,
                              const UINT8 *pb_respData)
{
  BOOLEAN o_return = FALSE;             /* predefined return value */
  UINT16 w_fsmNum = k_INVALID_FSM_NUM;  /* number of FSM processing the
                                           received response */

  /* if TR-field is NOT zero */
  if(ps_respHdr->b_tr != EPLS_k_TR_NOT_USED)
  {
    /* error: frame header of received response is NOT valid,
              TR-field NOT zero, reject frame */
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_RESP_TR_FIELD_NOT_UNUSED,
                  (UINT32)(ps_respHdr->b_tr));
    o_return = TRUE;  /* prepare return value */
  }
  /* else if the echo of the SOD access req number (SANo) is wrong */
  else if(ps_respHdr->w_ct != EPLS_k_CT_NOT_USED)
  {
    /* error: SOD access req number in resp different to req,
              call application, reject frame */
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_RESP_CT_FIELD_NOT_UNUSED,
                  (UINT32)(ps_respHdr->w_ct));
    o_return = TRUE;  /* prepare return value */
  }
  /* else if received SNMT Service response is "reset node garding" service */
  else if(SFS_GET_MIN_FRM_ID(ps_respHdr->b_id) == SNMT_k_RST_GUARD)
  {
    /* process the "reset node garding" service */
    SCM_SNMTM_ResetGuardClbk();
    /* no further action defined */
    o_return = TRUE;
  }
  /* else if received SNMTM Service Response is to reject or FATAL error */
  else if(!(IsDsadrScmSadr(ps_respHdr->w_adr)))
  {
    /* response is rejected or filtering failed,
       FATAL error already reported, return with predefiend FALSE */
  }
  else  /* no error during filtering */
  {
    /* get number of FSM which processes the currently received response */
    w_fsmNum =
      SNMTM_AssignResponse(ps_respHdr->b_id, pb_respData, ps_respHdr->w_tadr,
                            ps_respHdr->w_sdn);

    /* if aquired FSM number is invalid */
    if(w_fsmNum == k_INVALID_FSM_NUM)
    {
      /* error: received response could NOT be assigned to a request */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_RESP_NOT_ASSIGNED,
                    (UINT32)(ps_respHdr->b_id));
    }
    else  /* aquired FSM number is valid */
    {
      /* process received frame */
      o_return =
        SNMTM_ProcessFsm(dw_ct, (UINT8 *)NULL, w_fsmNum, k_EVT_RESP_RECEIVED,
                          0U, ps_respHdr, pb_respData);
    }
  }
  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function rejects received SNMTM Service Response in case of the target
* address is not equal to SCM main SADR.
*
* @param        w_rxDsadr        DSADR in the received response, (checked), valid range: SCM main SADR
*
* @return
* - TRUE           - Scm SADR received in the DSADR field
* - FALSE          - no Scm SADR received or the filtering failed and
* 	FATAL error is generated
*/
static BOOLEAN IsDsadrScmSadr(UINT16 w_rxDsadr)
{
  BOOLEAN o_return = FALSE;   /* predefined return value */
  UINT16 *pw_scmSadr = (UINT16 *)NULL;  /* ref to main Sadr of SN in SOD, only
                                           valid directly after SOD read */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */


  /* get main SADR of SN from SOD */
  pw_scmSadr =
    (UINT16 *)SOD_ReadVirt(EPLS_k_SCM_INST_NUM_ &SNMTM_s_AccessSadrScm, &s_errRes);

  /* if read access failed */
  if(pw_scmSadr == NULL)
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* read access succeeded */
  {
    /* if the received SNMTM Service Response is addressed to this SN */
    if(w_rxDsadr == (*pw_scmSadr))
    {
      /* process received SNMTM Service Response */
      o_return = TRUE;     /* prepare return value */
    }
    else  /* received SNMTM Service Response is NOT addressed to this SN */
    {
      /* the received SNMTM Service Response is not adressed to this SN,
         report error and reject service */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_RESP_REJ_WRONG_DSADR,
                    (UINT32)(w_rxDsadr));
    }
  }
  SCFM_TACK_PATH();
  return o_return;
}


#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param        w_errorCode        error number
*
* @param        dw_addInfo         additional error information
*
* @retval        pac_str            empty buffer to build the error string.
*/
  void SNMTM_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str)
  {
    /* choose the matching error string */
    switch(w_errorCode)
    {
      case SNMTM_k_ERR_EVENT_WFRES_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTM_k_ERR_EVENT_WFRES_INV: SNMTM_ProcessFsm():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTM_k_ERR_EVENT_WFRES_INV, dw_addInfo);
        break;
      }
      case SNMTM_k_ERR_SN_TRANS_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTM_k_ERR_SN_TRANS_INV: SNMTM_ReqSnTrans():\n"
          "An Invalid transition into SN state %02u is requested.\n",
          SNMTM_k_ERR_SN_TRANS_INV, (UINT16)dw_addInfo);
        break;
      }
      case SNMTM_k_ERR_REF_REQ_DATA_ASS_SADR_INV:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTM_k_ERR_REF_REQ_DATA_ASS_SADR_INV: SNMTM_ReqAssgnSadr():\n"
          "NULL pointer passed instead of reference to <UDID>.\n",
          SNMTM_k_ERR_REF_REQ_DATA_ASS_SADR_INV);
        break;
      }
      case SNMTM_k_ERR_ADD_SADR_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTM_k_ERR_ADD_SADR_INV: SNMTM_ReqAssgnAddSadr():\n"
          "Function call with invalid additional SADR (%04lu).\n",
          SNMTM_k_ERR_ADD_SADR_INV, dw_addInfo);
        break;
      }
      case SNMTM_k_ERR_SPDO_NUM_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTM_k_ERR_SPDO_NUM_INV: SNMTM_ReqAssgnAddSadr():\n"
          "Function call with invalid SPDO number (%04lu).\n",
          SNMTM_k_ERR_SPDO_NUM_INV, dw_addInfo);
        break;
      }
      case SNMTM_k_ERR_NO_FSM_AVAIL_REQ_SCM_UDID:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTM_k_ERR_NO_FSM_AVAIL_REQ_SCM_UDID: "
          "SNMTM_ReqAssgnScmUdid():\n"
          "No FSM available to process the SNMT Request, actually.\n",
          SNMTM_k_ERR_NO_FSM_AVAIL_REQ_SCM_UDID);
        break;
      }
      case SNMTM_k_ERR_NO_FSM_AVAIL_REQ_UDID:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTM_k_ERR_NO_FSM_AVAIL_REQ_UDID: SNMTM_ReqUdid():\n"
          "No FSM available to process the SNMT Request, actually.\n",
          SNMTM_k_ERR_NO_FSM_AVAIL_REQ_UDID);
        break;
      }
      case SNMTM_k_ERR_NO_FSM_AVAIL_ASS_SADR:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTM_k_ERR_NO_FSM_AVAIL_ASS_SADR: SNMTM_ReqAssgnSadr():\n"
          "No FSM available to process the SNMT Request, actually.\n",
          SNMTM_k_ERR_NO_FSM_AVAIL_ASS_SADR);
        break;
      }
      case SNMTM_k_ERR_NO_FSM_AVAIL_ASS_ADD_SADR:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTM_k_ERR_NO_FSM_AVAIL_ASS_ADD_SADR: "
          "SNMTM_ReqAssgnAddSadr():\n"
          "No FSM available to process the SNMT Request, actually.\n",
          SNMTM_k_ERR_NO_FSM_AVAIL_ASS_ADD_SADR);
        break;
      }
      case SNMTM_k_ERR_NO_FSM_AVAIL_REQ_GUARD:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTM_k_ERR_NO_FSM_AVAIL_REQ_GUARD: SNMTM_ReqGuarding():\n"
          "No FSM available to process the SNMT Request, actually.\n",
          SNMTM_k_ERR_NO_FSM_AVAIL_REQ_GUARD);
        break;
      }
      case SNMTM_k_ERR_NO_FSM_AVAIL_SN_TRANS:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTM_k_ERR_NO_FSM_AVAIL_SN_TRANS: SNMTM_ReqSnTrans():\n"
          "No FSM available to process the SNMT Request, actually.\n",
          SNMTM_k_ERR_NO_FSM_AVAIL_SN_TRANS);
        break;
      }
      case SNMTM_k_ERR_NO_RESP_RECEIVED:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTM_k_ERR_NO_RESP_RECEIVED: processTimeoutChk():\n"
          "Maximum number of SNMT Service Request repetitions of "
          "service (%02u) reached.\n",
          SNMTM_k_ERR_NO_RESP_RECEIVED, (UINT16)dw_addInfo);
        break;
      }
      case SNMTM_k_ERR_RESP_NOT_ASSIGNED:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTM_k_ERR_RESP_NOT_ASSIGNED: SNMTM_ProcessResponse():\n"
          "The received SNMT Service Response with ID %02u "
          "could NOT be assigned.\n",
          SNMTM_k_ERR_RESP_NOT_ASSIGNED, (UINT16)dw_addInfo);
        break;
      }
      case SNMTM_k_ERR_RX_FRM_ID_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTM_k_ERR_RX_FRM_ID_INV: SNMTM_AssignResponse():\n"
          "The received SNMT Service Response contains an invalid "
          "frame ID (%02lu).\n",
          SNMTM_k_ERR_RX_FRM_ID_INV, dw_addInfo);
        break;
      }
      case SNMTM_k_ERR_RESP_REJ_WRONG_DSADR:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTM_k_ERR_RESP_REJ_WRONG_DSADR: IsDsadrScmSadr():\n"
          "The received SNMT Service Response was rejected by "
          "SNMTM filter - target address: %04u.\n",
          SNMTM_k_ERR_RESP_REJ_WRONG_DSADR, (UINT16)dw_addInfo);
        break;
      }
      case SNMTM_k_ERR_RESP_TR_FIELD_NOT_UNUSED:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTM_k_ERR_RESP_TR_FIELD_NOT_UNUSED: "
          "SNMTM_ProcessResponse():\n"
          "The value in TR field (%02lu) of the received frame is NOT zero.\n",
          SNMTM_k_ERR_RESP_TR_FIELD_NOT_UNUSED, dw_addInfo);
        break;
      }
      case SNMTM_k_ERR_RESP_CT_FIELD_NOT_UNUSED:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTM_k_ERR_RESP_CT_FIELD_NOT_UNUSED: "
          "SNMTM_ProcessResponse():\n"
          "The value in CT field (%04lu) of the received frame is NOT zero.\n",
          SNMTM_k_ERR_RESP_CT_FIELD_NOT_UNUSED, dw_addInfo);
        break;
      }
      case SNMTM_k_ERR_EVT_INV:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTM_k_ERR_EVT_INV: SNMTM_ProcessFsm():\n"
          "FSM was called with invalid event in state <wait for requets>.\n",
          SNMTM_k_ERR_EVT_INV);
        break;
      }
      case SNMTM_k_ERR_REF_FREE_FRMS:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTM_k_ERR_REF_FREE_FRMS: SNMTM_BuildRequest():\n"
          "Invalid reference to the number of free frames.\n",
          SNMTM_k_ERR_REF_FREE_FRMS);
        break;
      }
      case SNMTM_k_ERR_NO_FSM_AVAIL_INIT_CT:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTM_k_ERR_NO_FSM_AVAIL_INIT_CT: "
          "SNMTM_ReqInitializeCtSn():\n"
          "No FSM available to process the SNMT Request, actually.\n",
          SNMTM_k_ERR_NO_FSM_AVAIL_INIT_CT);
        break;
      }
      default:
      {
        SPRINTF(pac_str, "Unknown error code was generated by the SNMTM\n");
      }
    }
    SCFM_TACK_PATH();
    return;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28   - Cyclomatic complexity > 15 */
  }
  /* RSM_IGNORE_QUALITY_END Notice #28   - Cyclomatic complexity > 15 */
#endif




/** @} */
