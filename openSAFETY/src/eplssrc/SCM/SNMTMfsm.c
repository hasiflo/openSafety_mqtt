/**
 * @addtogroup SNMTM
 * @{
 *
 * @file SNMTMfsm.c
 * The file contains the finite state machine of the unit SNMTM.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SNMTMfsm.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>15.03.2013</td><td>Hans Pill</td><td>added statistic counters</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>changes for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 *     <tr><td>19.04.2017</td><td>Roman Zwischelsberger</td><td>changes for better handling of SNMT timeouts</td></tr>
 *     <tr><td>05.09.2018</td><td>Stefan Innerhofer</td><td>allow SN Fails in all all SNMT states</td></tr>
 * </table>
 *
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
#include "SCM.h"
#include "SNMT.h"
#include "SNMTM.h"
#include "SNMTMint.h"
#include "SNMTMerr.h"

#include "sacun.h"

/**
 * This symbol is used as bit mask to set or clear bit0 of the id of a
 * SNMT Service Request/Response.
*/
#define k_REQ_RESP    (UINT8)0x01

/**
 * This symbol represents an invalid value of a registration number that is
 * assigned by the SCM to distinguish its different SCM finite state machines
*/
#define k_INVALID_REG_NUM              0xFFFFu

/** This symbol represents the maximum payload length of a SNMTM request. */
#define k_REQ_MAX_DATA_LEN        (UINT8)(SNMT_k_LEN_SERV_CMD + EPLS_k_UDID_LEN)

/**
 * This symbol represents the maximum value of valid response frame type of
 * extended services.
*/
#define k_MAX_RESP_EXT_SERV_FRM_TYPE   0x11u

/**
 * @name Range of 2-dim table
 * @{
 * This symbols are used to assign response command byte to request command byte.
*/
#define k_NUM_REQ                      9u
#define k_NUM_RESP                     9u
/** @} */

/**
 * This 2-dim array of constants assigns responses to their corresponding
 * requests.
 *
 * In case of SNMT Service Request SNMT_SCM_put_to_STOP and SNMT_SCM_put_to_OP
 * no response is planed.
 *
 * @attention Received frame ID from the response as well as frame ID of request
 * has to be shifted right by one bit. This is to reduce the cases of the lookup table.
*/
static const BOOLEAN ao_ReqRespTable[k_NUM_REQ][k_NUM_RESP] =
{
  /* request 0: SNMT_SN_put_to_PRE_OP */
  {
    TRUE,   /* response 0: SNMT_SN_status_PRE_OP */
    FALSE,  /* response 1: SNMT_SN_status_OP */
    FALSE,  /* response 2: SNMT_assign additional SADR */
    TRUE,   /* response 3: SNMT_SN_FAIL */
    FALSE,  /* response 4: SNMT_SN_BUSY */
    FALSE,  /* response 5: Reserved */
    FALSE,  /* response 6: Reserved */
    FALSE,  /* response 7: SNMT_assigned_UDID_SCM */
    FALSE   /* response 8: SNMT_assigned_Init_CT */
  },

  /* request 1: SNMT_SN_put_to_OP */
  {
    FALSE,  /* response 0: SNMT_SN_status_PRE_OP */
    TRUE,   /* response 1: SNMT_SN_status_OP */
    FALSE,  /* response 2: SNMT_assign additional SADR */
    TRUE ,  /* response 3: SNMT_SN_FAIL */
    TRUE,   /* response 4: SNMT_SN_BUSY */
    FALSE,  /* response 5: Reserved */
    FALSE,  /* response 6: Reserved */
    FALSE,  /* response 7: SNMT_assigned_UDID_SCM */
    FALSE   /* response 8: SNMT_assigned_Init_CT */
  },

  /* request 2: SNMT_SCM_put_to_STOP */
  {
    FALSE,  /* response 0: SNMT_SN_status_PRE_OP */
    FALSE,  /* response 1: SNMT_SN_status_OP */
    FALSE,  /* response 2: SNMT_assign additional SADR */
    TRUE,   /* response 3: SNMT_SN_FAIL */
    FALSE,  /* response 4: SNMT_SN_BUSY */
    FALSE,  /* response 5: Reserved */
    FALSE,  /* response 6: Reserved */
    FALSE,  /* response 7: SNMT_assigned_UDID_SCM */
    FALSE   /* response 8: SNMT_assigned_Init_CT */
  },

  /* request 3: SNMT_SCM_put_to_OP */
  {
    FALSE,  /* response 0: SNMT_SN_status_PRE_OP */
    FALSE,  /* response 1: SNMT_SN_status_OP */
    FALSE,  /* response 2: SNMT_assign additional SADR */
    TRUE,   /* response 3: SNMT_SN_FAIL */
    FALSE,  /* response 4: SNMT_SN_BUSY */
    FALSE,  /* response 5: Reserved */
    FALSE,  /* response 6: Reserved */
    FALSE,  /* response 7: SNMT_assigned_UDID_SCM */
    FALSE   /* response 8: SNMT_assigned_Init_CT */
  },

  /* request 4: SNMT_SCM_guard_SN */
  {
    TRUE,   /* response 0: SNMT_SN_status_PRE_OP */
    TRUE,   /* response 1: SNMT_SN_status_OP */
    FALSE,  /* response 2: SNMT_assign additional SADR */
    TRUE,   /* response 3: SNMT_SN_FAIL */
    FALSE,  /* response 4: SNMT_SN_BUSY */
    FALSE,  /* response 5: Reserved */
    FALSE,  /* response 6: Reserved */
    FALSE,  /* response 7: SNMT_assigned_UDID_SCM */
    FALSE   /* response 8: SNMT_assigned_Init_CT */
  },

  /* request 5: SNMT_assign_additional_SADR */
  {
    FALSE,  /* response 0: SNMT_SN_status_PRE_OP */
    FALSE,  /* response 1: SNMT_SN_status_OP */
    TRUE,   /* response 2: SNMT_assign additional SADR */
    TRUE,   /* response 3: SNMT_SN_FAIL */
    FALSE,  /* response 4: SNMT_SN_BUSY */
    FALSE,  /* response 5: Reserved */
    FALSE,  /* response 6: Reserved */
    FALSE,  /* response 7: SNMT_assigned_UDID_SCM */
    FALSE   /* response 8: SNMT_assigned_Init_CT */
  },
  /* request 6: SNMT_SN_ACK
                note: no answer is assigned to this service, because it just
                      acknowledges a reported error */
  {
    FALSE,  /* response 0: SNMT_SN_status_PRE_OP */
    FALSE,  /* response 1: SNMT_SN_status_OP */
    FALSE,  /* response 2: SNMT_assign additional SADR */
    TRUE,   /* response 3: SNMT_SN_FAIL */
    FALSE,  /* response 4: SNMT_SN_BUSY */
    FALSE,  /* response 5: Reserved */
    FALSE,  /* response 6: Reserved */
    FALSE,  /* response 7: SNMT_assigned_UDID_SCM */
    FALSE   /* response 8: SNMT_assigned_Init_CT */
  },
  /* request 7: SNMT_assign_UDID_SCM */
  {
    FALSE,  /* response 0: SNMT_SN_status_PRE_OP */
    FALSE,  /* response 1: SNMT_SN_status_OP */
    FALSE,  /* response 2: SNMT_assign additional SADR */
    TRUE,   /* response 3: SNMT_SN_FAIL */
    FALSE,  /* response 4: SNMT_SN_BUSY */
    FALSE,  /* response 5: Reserved */
    FALSE,  /* response 6: Reserved */
    TRUE,   /* response 7: SNMT_assigned_UDID_SCM */
    FALSE   /* response 8: SNMT_assigned_Init_CT */
  },
  /* request 8: SNMT_assign_Init_CT */
  {
    FALSE,  /* response 0: SNMT_SN_status_PRE_OP */
    FALSE,  /* response 1: SNMT_SN_status_OP */
    FALSE,  /* response 2: SNMT_assign additional SADR */
    TRUE,   /* response 3: SNMT_SN_FAIL */
    FALSE,  /* response 4: SNMT_SN_BUSY */
    FALSE,  /* response 5: Reserved */
    FALSE,  /* response 6: Reserved */
    FALSE,  /* response 7: SNMT_assigned_UDID_SCM */
    TRUE    /* response 8: SNMT_assigned_Init_CT */
  }
};

/**
 * Structured data type to store all necessary info and data to handle a
 * repetition of a service request transmission and processing the assigned
 * service response.
*/
typedef struct
{
  /** actual state of this FSM */
  BOOLEAN o_wfRequest;
  /** point of time when response timout is set */
  UINT32 dw_respTimeout;
  /** actual number of request repetitions */
  UINT8 b_actNumOfRep;
  /** nominal number of req repetitions*/
  UINT8 b_nomNumOfRep;
  /** registration number of the request */
  UINT16 w_regNum;
  /** sub structure to store all frame header info of request which is processed by this FSM */
  EPLS_t_FRM_HDR s_reqHdr;
  /* reference to payload data of request which is processed by this FSM */
  UINT8 ab_reqData[k_REQ_MAX_DATA_LEN];
} t_FSM;


/**
 * Data structure array of SNMTM_cfg_MAX_NUM_FSM SNMT Master FSM.
 *
 * Each FSM processes one service request, thus n FSMs can hold n
 * communication channels to n SNs.
*/
STATIC t_FSM as_Fsm[SNMTM_cfg_MAX_NUM_FSM] SAFE_NO_INIT_SEKTOR;


/** SOD access to entry 0x1202, 0x01, SNMT response timeout */
static SOD_t_ACS_OBJECT_VIRT s_AccessTimeout SAFE_NO_INIT_SEKTOR;


/** SOD access to entry 0x1202, 0x02, SNMT response repetitions */
static SOD_t_ACS_OBJECT_VIRT s_AccessRetries SAFE_NO_INIT_SEKTOR;

static BOOLEAN sendRequest(UINT32 dw_ct, UINT16 w_fsmNum, UINT16 w_regNum,
                              const EPLS_t_FRM_HDR *ps_hdrInfo,
                              const UINT8 *pb_data);
static BOOLEAN processTimeoutChk(UINT32 dw_ct, UINT8* pb_numMngtFrms,
                                 UINT16 w_fsmNum);
static BOOLEAN transmitRequest(UINT16 w_fsmNum, UINT32 dw_ct);
static BOOLEAN checkCmdByte(UINT8 b_reqCmd, UINT8 b_respCmd);
static void resetFsm(UINT16 w_fsmNum);

/**
* @brief This function initializes all SNMT Master FSM and the access variables to the
* SNMTM timeout and retries objects.
*
* @return
* - TRUE  - success
* - FALSE - failure
*/
BOOLEAN SNMTM_InitFsm(void)
{
  BOOLEAN o_return = FALSE;  /* predefiened return value */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */
  UINT16 w_fsmNum = 0x0000u;  /* loop counter for FSM init. */


  s_AccessTimeout.s_obj.w_index = EPLS_k_IDX_SNMT_COM_PARAM;
  s_AccessTimeout.s_obj.b_subIndex = EPLS_k_SUBIDX_SNMT_TIMEOUT;
  s_AccessTimeout.dw_segOfs = 0;
  s_AccessTimeout.dw_segSize = 0;

  /* if write access to the SOD entry NOT possible */
  if(NULL == SOD_AttrGetVirt(EPLS_k_SCM_INST_NUM_ &s_AccessTimeout, &s_errRes))
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* SOD entry response timeout is available */
  {
    s_AccessRetries.s_obj.w_index = EPLS_k_IDX_SNMT_COM_PARAM;
    s_AccessRetries.s_obj.b_subIndex = EPLS_k_SUBIDX_SNMT_RETRIES;
    s_AccessRetries.dw_segOfs = 0;
    s_AccessRetries.dw_segSize = 0;

    /* if write access to the SOD entry NOT possible */
    if(NULL == SOD_AttrGetVirt(EPLS_k_SCM_INST_NUM_ &s_AccessRetries, &s_errRes))
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
    }
    else  /* SOD entry response retries is available */
    {
      /* initialize modul global data structure of n SNMT Master FSM */
      for(w_fsmNum = 0U; w_fsmNum < SNMTM_cfg_MAX_NUM_FSM; w_fsmNum++)
      {
        resetFsm(w_fsmNum);
      }

      o_return = TRUE;
    }
  }
  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function processes the SNMT Master FSM.
*
* It is called by several superior function with focus on initiating a request
* transmission or focus on repeating a request transmission or processing a
* received response. Thus the interface of this function is multi-functional.

*
* @param dw_ct consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @retval pb_numFreeMngtFrms Is only used in state "wait on response", valid range: <> NULL
* - IN: max. number of openSAFETY frames of type SNMT and SSDO. This number of frames are
*   free to transmit during one appl. cycle
* - OUT: remaining number of free frames of type SNMT and SSDO. A value of zero means that
*   all required services were transm. (not checked, checked in SNMTM_BuildRequest())
*
*
* @param w_fsmNum finite state machine number, assigned internally (not checked, checked in SNMTM_BuildRequest() or SNMTM_AssignResponse() or SNMTM_GetFsmFree())), valid range: 0 .. <SNMTM_cfg_MAX_NUM_FSM-1>
*
* @param e_fsmEvent event which shall be processed by the SNMT Master FSM. (not checked, only called with enum value), valid range: see {t_FSM_EVENT}
*
* @param w_regNum Is only used in state "wait on request". registration number of the request (not checked checked in SCM_Trigger()), valid range: UINT16
*
* @param          ps_hdrInfo Reference to either request/response header info (not checked, called only with reference to struct), valid range: <> NULL
*
* @param          pb_data Reference to either to request/response data (not checked, only called with reference to byte array or checked in processStateReqProc()), valid range: <> NULL
*
* @return
* - TRUE                        - FSM processing successful
* - FALSE                       - FSM processing failed
*/
BOOLEAN SNMTM_ProcessFsm(UINT32 dw_ct, UINT8 *pb_numFreeMngtFrms,
          UINT16 w_fsmNum, t_FSM_EVENT e_fsmEvent, UINT16 w_regNum,
          const EPLS_t_FRM_HDR *ps_hdrInfo, const UINT8 *pb_data)
{
  BOOLEAN o_return = FALSE;  /* predefiened return value */


  /* NOTE: pb_numFreeMngtFrms is not used in state "wait for request" */
  /* if current state is "wait for request" */
  if(as_Fsm[w_fsmNum].o_wfRequest)
  {
    /* if calling event is <k_EVT_REQ_SEND> */
    if(e_fsmEvent == k_EVT_REQ_SEND)
    {
      /* process SNMT Service Request */
      o_return =
        sendRequest(dw_ct, w_fsmNum, w_regNum, ps_hdrInfo, pb_data);
    }
    /* else if the calling event is "check timeout"
       NOTE: SNMTM_ProcessFsm() is called with NO reference to frame header info
       or data, because NO response received */
    else if(e_fsmEvent == k_EVT_CHK_TIMEOUT)
    {
      o_return = TRUE;
    }
    else /* unexpected calling event */
    {
      /* error: invalid FSM event */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_EVT_INV,
                    (UINT32)(e_fsmEvent));
    }
  }
  else  /* not as_Fsm[w_fsmNum].o_wfRequest */
  {
    /* if the calling event is "response received",
       note: SNMTM_ProcessFsm() is called with response header and response data
             in this case */
    if(e_fsmEvent == k_EVT_RESP_RECEIVED)
    {
      /* call back the registered function of the request */
      SCM_SNMTM_RespClbk(as_Fsm[w_fsmNum].w_regNum, ps_hdrInfo->w_tadr,
                         ps_hdrInfo->w_sdn, pb_data, FALSE);

      /* reinitialize the current FSM and switch to state WF_REQUEST */
      resetFsm(w_fsmNum);
      o_return = TRUE;
    }
    /* else if the calling event is "check timeout"
       NOTE: SNMTM_ProcessFsm() is called with NO reference to frame header info
       or data, because NO response received */
    else if(e_fsmEvent == k_EVT_CHK_TIMEOUT)
    {
      /* if free management frames available */
      if(*pb_numFreeMngtFrms > 0u)
      {
        /* check response timeout */
        o_return = processTimeoutChk(dw_ct, pb_numFreeMngtFrms, w_fsmNum);
      }
      else  /* no timeout check */
      {
        o_return = TRUE;
      }
    }
    else  /* the calling event is NOT defined */
    {
      /* error: FSM was called with an invalid event */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_EVENT_WFRES_INV,
                    (UINT32)e_fsmEvent);
    }
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function returns the number of the next free FSM.
*
* If no FSM is free to process a new request, k_INVALID_FSM_NUM is returned.
*
* @return       0 .. SNMTM_cfg_MAX_NUM_FSM-1 - free FSM number k_INVALID_FSM_NUM              - no free FSM available
*/
UINT16 SNMTM_GetFsmFree(void)
{
  UINT16 w_return = k_INVALID_FSM_NUM;  /* predefined invalid number */
  UINT16 w_fsmNum = 0x0000u;            /* start value of search index */
  BOOLEAN o_nextFreeFsmFound = FALSE;   /* start value of result flag */


  /* linear search for the next free FSM */
  do
  {
    /* if state of current FSM is "wait for request" */
    if(as_Fsm[w_fsmNum].o_wfRequest)
    {
      o_nextFreeFsmFound = TRUE;  /* next free FSM found */
      w_return = w_fsmNum;        /* prepare return value */
    }
    else /* current FSM is in state "wait for response" and NOT available */
    {
      w_fsmNum++;  /* increment search index, FSM NOT found */
    }
  }
  /* search while next free FSM NOT found or max. number of FSM not reached */
  while((!o_nextFreeFsmFound) && (w_fsmNum < SNMTM_cfg_MAX_NUM_FSM));

  SCFM_TACK_PATH();
  return w_return;
}


/**
* @brief This function checks whether a single FSM is available to process transmission of one service request and reception of the  matching service response. *Attention:* This function must be called to ensure availability of a FSM to process a SNMT Service Request.
*
* @return
* - TRUE  - at least one SNMT Master FSM available
* - FALSE - no SNMT Master FSM available
*/
BOOLEAN SNMTM_CheckFsmAvailable(void)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */


  /* if at least one FSM is free */
  if(SNMTM_GetFsmFree() != k_INVALID_FSM_NUM)
  {
    o_return = TRUE;
  }
  /* else every FSM is busy */
  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function assigns a single received response to the matching request that
* is processed in a single FSM waiting for response.
*
* As result the number of the FSM, which processes the request/response is returned.
*
* @param        b_respId          frame ID of received response (checked), valid range: see {t_SNMTM_SERV_REQ} and {t_SNMTM_EXT_SERV_REQ}
*
* @param        pb_respCmd        ref to command byte of received service response (not checked, checked in processStateReqProc()), valid range: <> NULL
*
* @param        w_respTadr        address of node who transmitted the response (not checked, checked in checkRxAddrInfo()), valid range: EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @param        w_rxSdn           received SDN, (not checked, checked in checkRxAddrInfo()), valid range : k_MIN_SDN .. k_MAX_SDN
*
* @return       0 .. (SNMTM_cfg_MAX_NUM_FSM - 1)
*/
UINT16 SNMTM_AssignResponse(UINT8 b_respId, const UINT8 *pb_respCmd,
                            UINT16 w_respTadr, UINT16 w_rxSdn)
{
  UINT16 w_return = k_INVALID_FSM_NUM; /* pre defined return value */
  UINT16 w_fsmNum = 0x0000u;           /* start value of loop counter */
  UINT8 *pb_reqUdid = (UINT8 *)NULL;   /* ref to Udid transmitted in request
                                          and stored into FSM */
  const UINT8 *pb_respUdid = (UINT8 *)NULL; /* ref to Udid received in resp. */
  BOOLEAN o_quitLoop = FALSE;          /* start value of flag to drop out */
  UINT8 b_deserRespCmd;                /* deserialized response command */
  UINT8 b_deserReqCmd;                 /* deserialized request command */

  /* Request/Response bit is reset in the response ID to check against the
     request ID */
  EPLS_BIT_RESET_U8(b_respId, k_REQ_RESP);

  /* linear search throught the list of SNMT Master FSM, scanning for the
     matching address between request destination and response source */
  do
  {
    /* if the n-th FSM is currently in state "wait for response" */
    if(!(as_Fsm[w_fsmNum].o_wfRequest))
    {
      /* if the frame ID of the n-th FSM (request) matches to the frame ID
         of the received response */
      if(as_Fsm[w_fsmNum].s_reqHdr.b_id == b_respId)
      {
        /* switch for the SNMT Service Request */
        switch (SFS_GET_MIN_FRM_ID(as_Fsm[w_fsmNum].s_reqHdr.b_id))
        {
          case SNMT_k_EXT_SER_REQ: /* SNMT Extended Service Request */
          {
            /* Request command is deserialized */
            SFS_NET_CPY8(&b_deserReqCmd,
                         &as_Fsm[w_fsmNum].ab_reqData[SNMT_k_OFS_SERV_CMD]);
            /* Response command is deserialized */
            SFS_NET_CPY8(&b_deserRespCmd, pb_respCmd);
            /* if the command byte of the service request of the n-th FSM
               matches to the command byte of the received response */
            if(checkCmdByte(b_deserReqCmd, b_deserRespCmd))
            {
              /* if the SADR from the n-th FSM (request) is equal to the TADR
                 from te received response and the requested SDN number is
                 responded */
              if((as_Fsm[w_fsmNum].s_reqHdr.w_adr == w_respTadr) &&
                 (as_Fsm[w_fsmNum].s_reqHdr.w_sdn == w_rxSdn))
              {
                w_return = w_fsmNum;  /* TADR successfully assigned to SADR */
                o_quitLoop = TRUE;    /* leave while loop ahead of time */
              }
              else  /* address info of current n-th FSM is NOT matching */
              {
                w_fsmNum++;  /* increase loop counter */
              }
            }
            else  /* command byte of current n-th FSM is NOT matching */
            {
              w_fsmNum++;  /* increase loop counter */
            }
            break;
          }
          case SNMT_k_UDID_REQ: /* SNMT_Request_UDID */
          {
            /* if the SADR from n-th FSM (request) is equal to the TADR from
               the received response */
            if(as_Fsm[w_fsmNum].s_reqHdr.w_adr == w_respTadr)
            {
              w_return = w_fsmNum;  /* TADR successfully assigned to SADR */
              o_quitLoop = TRUE;    /* leave while loop ahead of time */
            }
            else  /* address info of current n-th FSM is NOT matching */
            {
              w_fsmNum++;  /* increase loop counter */
            }
            break;
          }
          case SNMT_k_ASS_SADR: /* SNMT_Assign_SADR */
          {
            /* NOTE : SADR and SDN are checked by the SCM */

            /* get ref to Udid that was sent with service SNMT_Assign_SADR */
            pb_reqUdid = &(as_Fsm[w_fsmNum].ab_reqData[0]);
            /* ref to Udid received with response is already available */
            pb_respUdid = pb_respCmd;

            /* if the Udid from n-th FSM (request) is equal to the Udid from
               the received response */
            if(MEMCOMP(pb_reqUdid, pb_respUdid, EPLS_k_UDID_LEN) ==
                                                                   MEMCMP_IDENT)
            {
              w_return = w_fsmNum;  /* received Udid successfully assigned to
                                       Udid of the request */
              o_quitLoop = TRUE;    /* leave while loop ahead of time */
            }
            else  /* physical address (Udid) of current n-th FSM is NOT
                     matching to physical address of response */
            {
              w_fsmNum++;  /* increase loop counter */
            }
            break;
          }
          #pragma CTC SKIP
          default: /* the received frame ID is NOT valid */
          {
            /* Code does not reached because the request ID is checked against
               the response ID */


            /* error: received frame ID is NOT valid,
                      return with predefined FALSE */
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_RX_FRM_ID_INV,
                          (UINT32)(as_Fsm[w_fsmNum].s_reqHdr.b_id));
            o_quitLoop = TRUE;    /* leave while loop ahead of time */
          }
          #pragma CTC ENDSKIP
        }
      }
      else  /* the frame ID of current n-th FSM is NOT matching */
      {
        w_fsmNum++;  /* increase loop counter */
      }
    }
    else  /* the n-th FSM is currently in state "wait for request" */
    {
      w_fsmNum++;  /* increase loop counter */
    }
  } while((!o_quitLoop) && (w_fsmNum < SNMTM_cfg_MAX_NUM_FSM));
  /* (loop, while loop counter is less max. number of FSM) AND
     (loop, while quit-flag is equal FALSE) */
  SCFM_TACK_PATH();
  return w_return;
}

/**
* @brief This function stores all frame header info and payload data of the SNMT
* Service Request into the FSM.
*
* Afterwards this function transmits the request.
*
* @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_fsmNum           finite state machine number, assigned internally (not checked, checked in or SNMTM_GetFsmFree())), valid range: 0 .. <SNMTM_cfg_MAX_NUM_FSM-1>
*
* @param        w_regNum 		   registration number of the request (not checked checked in SCM_Trigger()), valid range: UINT16
*
* @param        ps_hdrInfo         reference to request/response header info (not checked, called only with reference to struct), valid range: <> NULL
*
* @param        pb_data            reference to request/response data (not checked, only called with reference to byte array or checked in processStateReqProc()), valid range: <> NULL
*
* @return
* - TRUE             - request processed successfully
* - FALSE            - request processing failed
*/
static BOOLEAN sendRequest(UINT32 dw_ct, UINT16 w_fsmNum, UINT16 w_regNum,
                              const EPLS_t_FRM_HDR *ps_hdrInfo,
                              const UINT8 *pb_data)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */


  /* store request header info into data structure of FSM */
  as_Fsm[w_fsmNum].s_reqHdr = *ps_hdrInfo;

  /* if request payload data available */
  if(as_Fsm[w_fsmNum].s_reqHdr.b_le != k_REQ_NO_DATA_LEN)
  {
    /* store request payload data into FSM */
    MEMCOPY(&as_Fsm[w_fsmNum].ab_reqData[0], pb_data,
           as_Fsm[w_fsmNum].s_reqHdr.b_le);
  }
  /* else NO request payload data transfer necessary */

  /* store request parameter into FSM */
  as_Fsm[w_fsmNum].w_regNum = w_regNum;

  /* if transmission of the service request was successful */
  if(transmitRequest(w_fsmNum, dw_ct))
  {
    /* the subsequent state is WF_RESPONSE */
    as_Fsm[w_fsmNum].o_wfRequest = FALSE;
    o_return = TRUE;
  }
  /* else transmission of the service request failed,
     error is alredy processed and reported */

  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function checks the response timeout.
*
* Only if the request was repeated n-times, the SNMT Master reports a missing
* response to a specific request.
*
* @param		dw_ct                    	consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @retval     	pb_numMngtFrms
* - IN: max. number of EPLS frames of type SNMT and SSDO. This number of frames are free to	transmit during one appl. cycle
* - OUT: remaining number of free	frames of type SNMT and SSDO.	A value of zero means that all required services were transm. (not checked, checked in SNMTM_BuildRequest()),	valid range: <> NULL
*
* @param  		w_fsmNum                  	finite state machine number, assigned internally (not checked, checked in SNMTM_BuildRequest()), valid range:	0 .. <SNMTM_cfg_MAX_NUM_FSM-1>
*
* @return
* - TRUE                    - response timeout checked successfully
* - FALSE                   - response timeout checked with errors
*/
static BOOLEAN processTimeoutChk(UINT32 dw_ct, UINT8* pb_numMngtFrms,
                                 UINT16 w_fsmNum)
{
  BOOLEAN o_return = FALSE;      /* predefined return value */


  /* if response time has elapsed */
  if(EPLS_TIMEOUT(dw_ct, as_Fsm[w_fsmNum].dw_respTimeout))
  {
    /* if repetition counter of the specified FSM exceeded max. */
    if(as_Fsm[w_fsmNum].b_actNumOfRep >=
       as_Fsm[w_fsmNum].b_nomNumOfRep)
    {
      /* report timeout */
      SERR_CountAcycEvt(w_fsmNum,SERR_k_SNMT_TOUT);
      /* error: maximum repetitions reached, response cannot be received */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_NO_RESP_RECEIVED,
                    (UINT32)(as_Fsm[w_fsmNum].s_reqHdr.w_adr));

      /* call back the registered function of the request */
      SCM_SNMTM_RespClbk(as_Fsm[w_fsmNum].w_regNum,
                         as_Fsm[w_fsmNum].s_reqHdr.w_adr,
                         as_Fsm[w_fsmNum].s_reqHdr.w_sdn, (UINT8 *)NULL, TRUE);

      /* reinitialize the current FSM and switch to state WF_REQUEST*/
      resetFsm(w_fsmNum);
      o_return = TRUE;
    }
    else  /* repetition counter has NOT reached maximum */
    {
      /* if transmission of the service request was successful */
      if(transmitRequest(w_fsmNum, dw_ct))
      {
        /* report repetition */
    	SERR_CountAcycEvt(w_fsmNum,SERR_k_SNMT_RETRY);
        /* increment repetition counter of specified FSM */
        as_Fsm[w_fsmNum].b_actNumOfRep++;
        /* decrement the free number of management frames */
        (*pb_numMngtFrms)--;
        /* stay in this state because "waiting for response" */
        o_return = TRUE;
      }
      /* else transmission of the service request failed,
         error is already processed and reported, return predefined FALSE */
    }
  }
  else /* response time has NOT elapsed, yet */
  {
    /* wait for response, while timeout has not ellapsed, yet. */
    /* no action defined */
    o_return = TRUE;
  }
  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function transmits a SNMT Master service request.
*
* For that reason it gets an empty memory block, fills it with frame header info and
* frame data (serializing) and passes the frame to the SHNF for transmission.
*
* @param  	 	w_fsmNum  		finite state machine number, assigned internally (not checked, checked in SNMTM_BuildRequest() or SNMTM_AssignResponse() or SNMTM_GetFsmFree())), valid range: 0 .. <SNMTM_cfg_MAX_NUM_FSM-1>
*
* @param    	dw_ct 			consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @return
* - TRUE          - transmission successful
* - FALSE         - transmission failed
*/
static BOOLEAN transmitRequest(UINT16 w_fsmNum, UINT32 dw_ct)
{
  BOOLEAN o_return = FALSE;        /* predefined return value */
  UINT8 *pb_txFrame = (UINT8 *)NULL; /* reference to memory block */
  UINT32 *pdw_respTimeout = (UINT32 *)NULL;  /* ref to response timout, only
                                                valid after SOD read access */
  UINT8 *pb_reqRetries = (UINT8 *)NULL; /* ref to max. number of request
                                           retries, only valid after SOD read
                                           access */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */


  /* allocate a memory block of <m> byte (SFS_GetMemBlock) */
  pb_txFrame =
    SFS_GetMemBlock(EPLS_k_SCM_INST_NUM_ SHNF_k_SNMT, SFS_k_NO_SPDO,
                    as_Fsm[w_fsmNum].s_reqHdr.b_le);

  /* if memory allocation failed */
  if(pb_txFrame == NULL)
  {
    /* error: memory allocation failed, error is already,
           reported return value is predefined with FALSE */
  }
  else  /* memory allocation succeeded */
  {
    /* copy frame data into memory block */
    MEMCOPY(&(pb_txFrame[SFS_k_FRM_DATA_OFFSET]),
           &as_Fsm[w_fsmNum].ab_reqData[0],
           as_Fsm[w_fsmNum].s_reqHdr.b_le);

    /* get response timeout value from SOD */
    pdw_respTimeout =
      (UINT32 *)SOD_ReadVirt(EPLS_k_SCM_INST_NUM_ &s_AccessTimeout, &s_errRes);

    /* if read access to SOD failed */
    if(pdw_respTimeout == NULL)
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
    }
    else  /* response timeout available */
    {
      /* set response timeout */
      as_Fsm[w_fsmNum].dw_respTimeout = dw_ct + *pdw_respTimeout;

      /* get response timeout value from SOD */
      pb_reqRetries =
        (UINT8 *)SOD_ReadVirt(EPLS_k_SCM_INST_NUM_ &s_AccessRetries, &s_errRes);

      /* if read access to SOD failed */
      if(pb_reqRetries == NULL)
      {
        SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                      (UINT32)(s_errRes.e_abortCode));
      }
      else  /* response timeout available */
      {
        /* set maximum number of request repetitions */
        as_Fsm[w_fsmNum].b_nomNumOfRep = *pb_reqRetries;

        o_return =
          SFS_FrmSerialize(EPLS_k_SCM_INST_NUM_ &(as_Fsm[w_fsmNum].s_reqHdr),
                           pb_txFrame);
      }
    }
  }
  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function checks if the command byte from received response is in valid range.
*
* Mainly it is checked if the command byte from the received response correspond to the
* command byte of the request, which is process into a FSM.
*
* @param        b_reqCmd         command byte of request processed in the FSM (not checked, set in SNMTM_ReqUdid() or SNMTM_ReqAssgnSadr() or SNMTM_ReqAssgnAddSadr() or SNMTM_ReqGuarding() SNMTM_ReqGuarding() or SNMTM_ReqSnTrans()), valid range: see {SNMT_t_EXT_SERV_REQ}
*
* @param        b_respCmd        command byte of received response (checked), valid range: <= k_MAX_RESP_EXT_SERV_FRM_TYPE
*
* @return
* - TRUE           - command byte of received response matches to request processed into
*   a FSM
* - FALSE          - command byte of received response does NOT match to request processed
*   into a FSM
*/
static BOOLEAN checkCmdByte(UINT8 b_reqCmd, UINT8 b_respCmd)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */


  /* if the received extended service frame type is in valid range */
  if (b_respCmd <= k_MAX_RESP_EXT_SERV_FRM_TYPE)
  {
    /* shift right one bit to get the value range that is used inside the
       2-dim lookup table. The value range was reduced in comparison to the
       original values the reduce the number of cases of the lookup table */
    o_return = ao_ReqRespTable[b_reqCmd>>1][b_respCmd >>1];
  }
  /* no else : received Extended Service frame type (DB0) exceed valid range
               error generated in SNMTM_ProcessResponse() later as received
               response could NOT be assigned to a request */

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function reset the specified SNMT Master FSM.
*
* This function is called after successful processing the request or after any error
* which causes the SCM to abort.
*
* @param        w_fsmNum        finite state machine number, assigned internally (not checked, checked in SNMTM_BuildRequest() or SNMTM_AssignResponse() or SNMTM_GetFsmFree() or SNMTM_InitFsm()), valid range: see SNMTM_cfg_MAX_NUM_FSM
*/
static void resetFsm(UINT16 w_fsmNum)
{
  UINT8 b_index = 0x00u;     /* loop counter for data initialization */

  /* initialize control parameter */
  as_Fsm[w_fsmNum].o_wfRequest    = TRUE;    /* "wait for request" */
  as_Fsm[w_fsmNum].dw_respTimeout = 0x00UL;  /* no timeout is set */
  as_Fsm[w_fsmNum].b_actNumOfRep  = 0x00u;   /* no repetitions */
  as_Fsm[w_fsmNum].b_nomNumOfRep  = 0x00u;   /* no repetitions */

  /* initialize array of payload data */
  for(b_index = 0x00u ; b_index<k_REQ_MAX_DATA_LEN ; b_index++)
  {
    as_Fsm[w_fsmNum].ab_reqData[b_index] = 0x00u;  /* data to zero */
  }

  /* initialize request parameter */
  as_Fsm[w_fsmNum].w_regNum = k_INVALID_REG_NUM;

  /* initialize frame header info */
  as_Fsm[w_fsmNum].s_reqHdr.w_adr  = 0x0000u;  /* invalid address */
  as_Fsm[w_fsmNum].s_reqHdr.b_id   = 0x00u;    /* invalid ID */
  as_Fsm[w_fsmNum].s_reqHdr.w_sdn  = 0x0000u;  /* invalid domain num. */
  as_Fsm[w_fsmNum].s_reqHdr.b_le   = 0x00u;    /* zero */
  as_Fsm[w_fsmNum].s_reqHdr.w_ct   = 0x0000u;  /* zero */
  as_Fsm[w_fsmNum].s_reqHdr.w_tadr = 0x00u;    /* "not used" */
  as_Fsm[w_fsmNum].s_reqHdr.b_tr   = 0x00u;    /* "not used" */

  SCFM_TACK_PATH();
}


/** @} */
