/**
 * @addtogroup SSDOC
 * @{
 *
 * @file SSDOCservice.c
 *
 * This file contains functionality to process a SSDO Service Request and process
 * the assigned SSDO Service Response.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * In case of no SSDO Service Response is received (timeout), the current
 * SSDO Service Request is repeated &lt;n&gt; times before an error is
 * reported.
 *
 * <h2>History for SSDOCservice.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SLV10</td></tr>
 *     <tr><td>11.09.2014</td><td>Hans Pill</td><td>added functionality for preload</td></tr>
 *     <tr><td>01.06.2016</td><td>Stefan Innerhofer</td><td>reset serviceFsm if an abort was sent, so the abort is sent only once</td></tr>
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
#include "SDN.h"
#include "SSDOCapi.h"
#include "SSDOCint.h"
#include "SSDOCerr.h"
#include "SSDOint.h"

#include "sacun.h"

/**
 * This symbol is used as bit masks to set/reset bit 1 inside the SOD access command.
*/
#define k_DIR_BIT              (UINT8)0x01  /* 0000|0001 */

/**
 * Structured data type to store all necessary info and data to handle a
 * repetition of a service req transmission and processing the assigned
 * service resp.
*/
typedef struct
{
  /**
   * Actual state
   * - TRUE  : FSM waiting for the request
   * - FALSE : FSM waiting for the resp.
   */
  BOOLEAN o_waitForReq;
  /**
   * Preload in progress
   * - TRUE  : FSM sends requests and receives responses
   * - FALSE : FSM uses o_waitForReq
   */
  BOOLEAN o_preload;
  /** request buffer */
  t_REQ_BUFFER s_reqBuf;
} t_SERV_FSM;

/**
 * Module global data structure array of SSDOC_cfg_MAX_NUM_FSM service FSM
 * of the SSDO Client.
 *
 * Each FSM processes one service request, thus &lt;n&gt; FSMs can hold &lt;n&gt;
 * communication channels to &lt;n&gt; SNs.
*/
STATIC t_SERV_FSM as_ServFsm[SSDOC_cfg_MAX_NUM_FSM] SAFE_NO_INIT_SEKTOR;

/** This symbol represents the SSDOC frame type Service Request Fast. */
#define k_SERVICE_REQ_FAST            0x3Au


static void serviceFsmInit(UINT16 w_fsmNum);
static BOOLEAN processResponse(UINT16 w_fsmNum, UINT16 w_respCt,
                               UINT8 b_respTr);
static BOOLEAN reqBufferSend(UINT16 w_fsmNum);


/**
* @brief This function initiates the SSDOC Service FSM and the SSDOC
* Service timer.
*
* @return
* - TRUE  - initialization succeeded
* - FALSE - initialization failed
*/
BOOLEAN SSDOC_ServiceFsmInit(void)
{
   BOOLEAN o_return = FALSE;  /* predefined return value */
   UINT16 w_fsmNum = 0u;       /* loop counter for FSM init. */

  /* if the timer initialization succeeded */
  if (SSDOC_TimerInit())
  {
    /* init module global data structure of &lt;n&gt; the SSDOC Service FSM */
    for(w_fsmNum = 0u ; w_fsmNum < (UINT16)SSDOC_cfg_MAX_NUM_FSM ; w_fsmNum++)
    {
      serviceFsmInit(w_fsmNum);
    }
    o_return = TRUE;
  }
  /* no else : error was already reported by the SOD */

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function sets the "wait for request"bit to the desired value.
*
* @param        w_fsmNum        SSDO Client FSM number (not checked, checked in
*     SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_SendReq() and
*     getProtocolFsmFree() or SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        o_waitForReq    desired value for the bit
*/
void SSDOC_ServiceFsmSetWaitForReq(UINT16 const w_fsmNum, BOOLEAN const o_waitForReq)
{
    as_ServFsm[w_fsmNum].o_waitForReq = o_waitForReq;
}

/**
* @brief This function sets the "preload" bit to the desired value.
*
* @param        w_fsmNum        SSDO Client FSM number (not checked, checked in
*     SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_SendReq() and
*     getProtocolFsmFree() or SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        o_preload    desired value for the bit
*/
void SSDOC_ServiceFsmSetPreload(UINT16 const w_fsmNum, BOOLEAN const o_preload)
{
    as_ServFsm[w_fsmNum].o_preload = o_preload;
}

/**
* @brief This function gets a reference to the request buffer in the service FSM.
*
* @param        w_fsmNum        SSDO Client FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_SendReq() and
* 	getProtocolFsmFree() or SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @return       pointer to the request buffer
*/

t_REQ_BUFFER *SSDOC_GetReqBuffer(UINT16 w_fsmNum)
{
  SCFM_TACK_PATH();
  return &as_ServFsm[w_fsmNum].s_reqBuf;
}

/**
* @brief This function checks the response timeout of SSDO Service Requests.
*
* Only if a response failed to receive more often an error is reported to the protocol
* FSM. If no management frames are available the response timeout is not checked.
*
* @param        dw_ct                         consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @retval       pb_numFreeMngtFrms            valid range: <> NULL
* - IN: max. number of EPLS frames of type SNMT and SSDO. This number of frames are free
*   to transmit during one appl. cycle
* - OUT: remaining number of free frames of type SNMT and SSDO. A value of zero means
*   that all required services were transm. (not checked, checked in SSDOC_BuildRequest())
*
* @param        w_fsmNum                       service FSM number, assigned internally (not checked, checked in SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @retval          po_abort                      only relevant if return value=TRUE , valid range : <> NULL
* - TRUE : abort frame has to be sent
* - FALSE : no abort frame has to be sent (not checked, only called with reference to
*   variable in SSDOC_BuildRequest())
*
* @return
* - TRUE                        - response timeout check succeeded
* - FALSE                       - response timeout check failed
*/
BOOLEAN SSDOC_CheckTimeout(UINT32 dw_ct, UINT8 *pb_numFreeMngtFrms,
                           UINT16 w_fsmNum, BOOLEAN *po_abort)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  BOOLEAN o_timeout; /* flag for timeout checking */
  BOOLEAN o_retrans; /* flag for retransmission checking */

  *po_abort = FALSE;

  /* if the actual state is "wait for response" */
  if ((!as_ServFsm[w_fsmNum].o_waitForReq) ||
       (as_ServFsm[w_fsmNum].o_preload))
  {
    /* if there is a free frame available */
    if ((*pb_numFreeMngtFrms) > 0U)
    {
      /* Check timer */
      SSDOC_TimerCheck(w_fsmNum, dw_ct, &o_timeout, &o_retrans);

      /* if resp time has elapsed */
      if(o_timeout)
      {
        /* if retransmission is required */
        if(o_retrans)
        {
          /* if retransmission of the service request was successful */
          if(SSDOC_ServiceFsmProcess(dw_ct, w_fsmNum, k_EVT_RETRANS, 0U, 0U))
          {
            /* decrement the free number of management frames */
            (*pb_numFreeMngtFrms)--;

            o_return = TRUE;
          }
          /* no else : retransmission failed,
                       FATAL error was reported by the SFS */
        }
        else /* retransmission is not required */
        {
          *po_abort = TRUE;
          o_return = TRUE;
        }
      }
      else /* resp time has NOT elapsed, yet */
      {
        o_return = TRUE;  /* prepare return value */
      }
    }
    else /* no free frame is available */
    {
      /* timeout checking is ignored */
      o_return = TRUE;
    }
  }
  /* else : no timeout checking is required */
  else
  {
    o_return = TRUE;
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function processes the SSDO Client Service FSM.
*
* If a response received then it is call by the protocol layer with "response received"
* event. If a timeout happened then it is called with retransmission event. Request
* event is generated by the service request functions (SSDOC_ServiceDwnldInit(),
* SSDOC_ServiceDwnldSeg(), SSDOC_ServiceUpldInit() SSDOC_ServiceUpldSeg()). Send abort
* event is generated by the service abort request function (SSDOC_ServiceAbort()).
*
* @param        dw_ct             consecutive time (not checked, any value allowed),
* 	valid range: any 32 bit value
*
* @param        w_fsmNum          SSDO Client FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest()
* 	or SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        e_fsmEvent        event which shall be processed by the FSM. (checked),
* 	valid range: see t_SERV_FSM_EVENT
*
* @param        w_respCt          only relevant if the event is  k_EVT_RESP_RECEIVED,
* 	otherwise 0 consecutive time in the response header (checked), valid range: request Ct
*
* @param        b_respTr          only relevant if the event is k_EVT_RESP_RECEIVED,
* 	otherwise 0 time request distinctive number (checked), valid range: EPLS_k_TR_NOT_USED
*
* @return
* - TRUE            - If the calling event is k_EVT_RESP_RECEIVED then the received frame
*   processing succeeded, otherwise the request transmission succeeded.
* - FALSE           - If the calling event is k_EVT_RESP_RECEIVED then the received frame
*   processing failed, Minor error is generated and frame is ignored, otherwise the request
*   transmission failed, FSM is reinitialized and FATAL error is already generated by the
*   SFS.
*/
BOOLEAN SSDOC_ServiceFsmProcess(UINT32 dw_ct, UINT16 w_fsmNum,
                                t_SERV_FSM_EVENT e_fsmEvent, UINT16 w_respCt,
                                UINT8 b_respTr)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */

  /* preload is active */
  if (as_ServFsm[w_fsmNum].o_preload)
  {
        /* if the calling event is "SSDO Service is requested" */
        if (e_fsmEvent == k_EVT_REQ)
        {
          /* if the timer start succeeded */
          if (SSDOC_TimerStart(w_fsmNum, dw_ct))
          {
            /* if transmission of the service request was successful */
            if(reqBufferSend(w_fsmNum))
            {
              o_return = TRUE;
            }
            else /* transmission failed, FATAL error was reported by the SFS */
            {
              serviceFsmInit(w_fsmNum);
            }
          }
          else /* timer start failed, error was reported by the SOD */
          {
            serviceFsmInit(w_fsmNum);
          }
        }
        /* else if the calling event is "abort send" */
        else if(e_fsmEvent == k_EVT_ABORT_SEND)
        {
          /* if transmission of the service abort request failed */
            if(reqBufferSend(w_fsmNum))
            {
                o_return = TRUE;
            }
            /* no else required, FATAL error is already reported */
            serviceFsmInit(w_fsmNum);
        }
        /* if the calling event is "response received" */
        else if(e_fsmEvent == k_EVT_RX_RESP)
        {
          /* if received response is valid */
          if (processResponse(w_fsmNum, w_respCt, b_respTr))
          {
            o_return = TRUE;
          }
          /* no else : response is invalid and is ignored */
        }
        /* else if the calling event is "request buffer retransmission" */
        else if(e_fsmEvent == k_EVT_RETRANS)
        {
          /* response timer is restarted */
          SSDOC_TimerReStart(w_fsmNum, dw_ct);

          /* if retransmission of the service request was successful */
          if(reqBufferSend(w_fsmNum))
          {
            o_return = TRUE;
          }
          else /* retransmission failed, FATAL error was reported by the SFS */
          {
            serviceFsmInit(w_fsmNum);
          }
        }
        else /* the calling event is NOT defined */
        {
          serviceFsmInit(w_fsmNum);

          /* error: FSM was called with an invalid event */
          SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_SERV_FSM_EVT_INV2,
                        (UINT32)(e_fsmEvent));
        }
  }
  /* if state is "wait for request" */
  else if(as_ServFsm[w_fsmNum].o_waitForReq)
  {
    /* if the calling event is "SSDO Service is requested" */
    if (e_fsmEvent == k_EVT_REQ)
    {
      /* if the timer start succeeded */
      if (SSDOC_TimerStart(w_fsmNum, dw_ct))
      {
        /* if transmission of the service request was successful */
        if(reqBufferSend(w_fsmNum))
        {
          /* switch to "wait for response" state */
          as_ServFsm[w_fsmNum].o_waitForReq = FALSE;

          o_return = TRUE;
        }
        else /* transmission failed, FATAL error was reported by the SFS */
        {
          serviceFsmInit(w_fsmNum);
        }
      }
      else /* timer start failed, error was reported by the SOD */
      {
        serviceFsmInit(w_fsmNum);
      }
    }
    /* else if the calling event is "abort send" */
    else if(e_fsmEvent == k_EVT_ABORT_SEND)
    {
      /* if transmission of the service abort request failed */
        if(reqBufferSend(w_fsmNum))
        {
            o_return = TRUE;
        }
        /* no else required, FATAL error is already reported */
        serviceFsmInit(w_fsmNum);
    }
    else /* event is invalid */
    {
      serviceFsmInit(w_fsmNum);

      SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_SERV_FSM_EVT_INV1,
                    (UINT32)(e_fsmEvent));
    }
  }
  else  /* state is "wait for response" */
  {
    /* if the calling event is "response received" */
    if(e_fsmEvent == k_EVT_RX_RESP)
    {
      /* if received response is valid */
      if (processResponse(w_fsmNum, w_respCt, b_respTr))
      {
        /* switch to "wait for request" state */
        as_ServFsm[w_fsmNum].o_waitForReq = TRUE;

        o_return = TRUE;
      }
      /* no else : response is invalid and is ignored */
    }
    /* else if the calling event is "request buffer retransmission" */
    else if(e_fsmEvent == k_EVT_RETRANS)
    {
      /* response timer is restarted */
      SSDOC_TimerReStart(w_fsmNum, dw_ct);

      /* if retransmission of the service request was successful */
      if(reqBufferSend(w_fsmNum))
      {
        o_return = TRUE;
      }
      else /* retransmission failed, FATAL error was reported by the SFS */
      {
        serviceFsmInit(w_fsmNum);
      }
    }
    /* else if the calling event is "abort send" */
    else if(e_fsmEvent == k_EVT_ABORT_SEND)
    {
      /* if transmission of the service abort request was successful */
      if(reqBufferSend(w_fsmNum))
      {
        /* switch to WF_REQUEST state */
        o_return = TRUE;
      }
      serviceFsmInit(w_fsmNum);
    }
    else /* the calling event is NOT defined */
    {
      serviceFsmInit(w_fsmNum);

      /* error: FSM was called with an invalid event */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_SERV_FSM_EVT_INV2,
                    (UINT32)(e_fsmEvent));
    }
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function checks whether a SSDO transfer is already running with the given SN.
*
* @param        w_sadr        target address of SN to be accessed (not checked, checked
*   in SSDOC_SendReadReq() and SSDOC_SendWriteReq()) valid range: EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @return
* - TRUE        - SSDO tranfer is already running and minor error was reported.
* - FALSE       - SSDO tranfer is not running.
*/
BOOLEAN SSDOC_TransferRunning(UINT16 w_sadr)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  UINT16 w_fsmNum = 0x0000u; /* start value of loop counter */

  do
  {
    /* if n-th FSM is currently in state "wait for response" */
    if(!as_ServFsm[w_fsmNum].o_waitForReq)
    {
      /* if SSDO transfer is running with this SN */
      if(as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.w_adr == w_sadr)
      {
        SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_TRANSFER_RUNNING,
                      (UINT32)w_sadr);
        o_return = TRUE;
      }
      else /* no SSDO transfer is running with this SN */
      {
        w_fsmNum++;  /* increase loop counter */
      }
    }
    else /* the n-th FSM is in state "wait for request" */
    {
      w_fsmNum++;  /* increase loop counter */
    }
  } while((!o_return) && (w_fsmNum < (UINT16)SSDOC_cfg_MAX_NUM_FSM));
  /* (loop, while loop counter is less max. number of FSM) AND
     (loop, while o_return is equal FALSE) */

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function assigns a single received response to the matching request that
* is processed in one of the SSDOC service FSM waiting for response.
*
* As result the number of the FSM which processes the request/response is returned.
*
* @param        b_respId          frame ID of the received response (not checked, any value allowed), valid range: (UINT8)
*
* @param        w_respSadr 		  address of the node who received the resp
* 	(target address resp) (not checked, checked in checkRxAddrInfo()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        w_respTadr        address of the node who transmitted the resp
* 	(source address resp) (not checked, checked in checkRxAddrInfo()), valid range:
* 	EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param w_respSdn SDN in the received response (not checked, checked in
*   checkRxAddrInfo()), valid range: k_MIN_SDN .. k_MAX_SDN
*
* @return       0 .. (SSDOC_cfg_MAX_NUM_FSM - 1) - FSM number, response has been assigned
*   k_INVALID_FSM_NUM - response can not be assigned
*/
UINT16 SSDOC_ServiceResponseAssign(UINT8 b_respId, UINT16 w_respSadr,
                                   UINT16 w_respTadr, UINT16 w_respSdn)
{
  UINT16 w_return = k_INVALID_FSM_NUM; /* pre defined return value */
  UINT16 w_fsmNum = 0x0000u;           /* start value of loop counter */
  BOOLEAN o_quitLoop = FALSE;          /* start value of flag to drop out */

  /* direction bit is reset */
  EPLS_BIT_RESET_U8(b_respId, k_DIR_BIT);

  /* linear search through the list of SSDO Client FSM, scanning for the
     matching address between req destination and resp source */
  do
  {
    /* if n-th FSM is currently in state "wait for response" */
    if(!as_ServFsm[w_fsmNum].o_waitForReq ||
            as_ServFsm[w_fsmNum].o_preload)
    {
      /* if the frame ID of the n-th FSM request matches to the frame ID of
         the received response and received SDN is equal to the own SDN */
      if((w_respSdn == SDN_GetSdn(EPLS_k_SCM_INST_NUM)) &&
         (as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.b_id == b_respId))
      {
        /* if the SADR from n-th FSM (req) is equal to the TADR from the
              received resp and TADR from n-th FSM req is equal to the SADR
              from the received resp */
        if ((as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.w_adr == w_respTadr) &&
            (as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.w_tadr == w_respSadr))
        {
          w_return = w_fsmNum;  /* TADR successfully assigned to SADR */
          o_quitLoop = TRUE;    /* leave while loop ahead of time */
        }
        else /* address info of current n-th FSM is NOT matching */
        {
          w_fsmNum++;  /* increase loop counter */
        }
      }
      else /* the frame ID of current n-th FSM is NOT matching */
      {
        w_fsmNum++;  /* increase loop counter */
      }
    }
    else /* the n-th FSM is in state "wait for request" */
    {
      w_fsmNum++;  /* increase loop counter */
    }
  } while((!o_quitLoop) && (w_fsmNum < (UINT16)SSDOC_cfg_MAX_NUM_FSM));
  /* (loop, while loop counter is less max. number of FSM) AND
     (loop, while quit-flag is equal FALSE) */

  SCFM_TACK_PATH();
  return w_return;
}

/**
* @brief This function changes the download type from preload to normal.
*
* @param w_fsmNum SSDO Client FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest() or
* 	SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @return
* - TRUE            - preload was turned off
* - FALSE           - preload could not get turned off
*/
BOOLEAN SSDOC_ReqBufferPreLoadOff(UINT16 const w_fsmNum)
{
  BOOLEAN o_return = FALSE; /* predefined return value */
  t_REQ_BUFFER *ps_reqBuf = &as_ServFsm[w_fsmNum].s_reqBuf; /* pointer to the
                                                               request buffer */
  UINT8 b_saCmd; /* SOD access command byte */
  /* load SOD access request command from the request buffer */
  SFS_NET_CPY8(&b_saCmd,&ps_reqBuf->ab_data[k_OFS_SACMD]);
  /* check if last telegram was an initiate preload download */
  if ( k_DWNLD_SEG_PRE_INIT == b_saCmd)
  {
    o_return = TRUE;
    b_saCmd = k_DWNLD_INIT_SEG;
    /* store SOD access request command into the request buffer */
    SFS_NET_CPY8(&ps_reqBuf->ab_data[k_OFS_SACMD], &b_saCmd);
  }
  return o_return;
}

/**
* @brief This function initializes the specified SSDO Client Service FSM.
*
* @param  w_fsmNum  SSDO Client FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest() or
* 	SSDOC_SendReq() and getProtocolFsmFree() or SSDOC_ServiceFsmInit()),
* 	valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*/
static void serviceFsmInit(UINT16 w_fsmNum)
{
  UINT8 b_index;  /* loop counter for data initialization */

  /* initialize control parameter */
  as_ServFsm[w_fsmNum].o_waitForReq = TRUE;  /* wait for request */
  as_ServFsm[w_fsmNum].o_preload = FALSE;  /* preload */

  /* initialize array of payload data */
  for(b_index = 0U ; b_index < SSDOC_k_MAX_SERV_DATA_LEN ; b_index++)
  {
    as_ServFsm[w_fsmNum].s_reqBuf.ab_data[b_index] = 0x00U;  /* data zero */
  }

  /* initialize frame header info */
  as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.w_adr  = 0x0000U; /* invalid address */
  as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.b_id   = 0x00U;   /* invalid ID */
  as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.w_sdn  = 0x0000U; /* invalid domain num*/
  as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.b_le   = 0x00U;   /* zero */
  as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.w_ct   = 0x0000U; /* zero */
  as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.w_tadr = 0x00U;   /* "not used" */
  as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.b_tr  = 0x00U;    /* "not used" */

  SCFM_TACK_PATH();
}

/**
* @brief This function processes the received SSDO Response.
*
* For that reason header info is checked. In case of an error the protocol FSM is called
* to process the error. Thus this function returns always TRUE.
*
* @param w_fsmNum SSDO Client FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest() or
* 	SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param w_respCt consecutive time in the response header (checked),
* 	valid range: request Ct
*
* @param b_respTr time request distinctive number (checked),
* 	valid range: EPLS_k_TR_NOT_USED
*
* @return
* - TRUE            - response is valid and can be processed
* - FALSE           - response is invalid, is ignored and Minor error is reported
*/
static BOOLEAN processResponse(UINT16 w_fsmNum, UINT16 w_respCt,
                               UINT8 b_respTr)
{
  BOOLEAN o_return = FALSE; /* predefined return value */

  /* check SaNo */
  if (!SSDOC_CheckSaNo(w_respCt,as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.w_ct,w_fsmNum))
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_SANO_DIFF,
                  (((UINT32)w_respCt)<<16) +
                  as_ServFsm[w_fsmNum].s_reqBuf.s_hdr.w_ct);
  }
  else  /* received response is valid */
  {
    o_return = TRUE;
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function transmits the request in the request buffer.
*
* @param w_fsmNum FSM number (not checked, checked in SSDOC_ServiceResponseAssign() and
* 	SSDOC_ProcessResponse() or SSDOC_BuildRequest() or SSDOC_SendReq() and
* 	getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @return
* - TRUE - transmission successful
* - FALSE - transmission failed, fatal error is reported by the SFS
*/
static BOOLEAN reqBufferSend(UINT16 w_fsmNum)
{
  BOOLEAN o_return = FALSE; /* predefined return value */
  UINT8 *pb_txFrame = (UINT8 *)NULL; /* ref to memory block */
  t_REQ_BUFFER *ps_reqBuf = &as_ServFsm[w_fsmNum].s_reqBuf; /* pointer to the
                                                               request buffer */
  /* slim SSDOs do have a different frame type */
  if ( k_SERVICE_REQ_FAST == ps_reqBuf->s_hdr.b_id)
  {
    /* allocate a memory block */
    pb_txFrame = SFS_GetMemBlock(EPLS_k_SCM_INST_NUM_ SHNF_k_SSDO_SLIM, SFS_k_NO_SPDO,
                  ps_reqBuf->s_hdr.b_le);
  }
  else
  {
    /* allocate a memory block */
    pb_txFrame = SFS_GetMemBlock(EPLS_k_SCM_INST_NUM_ SHNF_k_SSDO, SFS_k_NO_SPDO,
                  ps_reqBuf->s_hdr.b_le);
  }

  /* if memory allocation failed */
  if(pb_txFrame != NULL)
  {
    /* copy payload into memory block */
    MEMCOPY(&pb_txFrame[SFS_k_FRM_DATA_OFFSET], &(ps_reqBuf->ab_data[0]),
           ps_reqBuf->s_hdr.b_le);

    /* frame serializing / transmission */
    o_return = SFS_FrmSerialize(EPLS_k_SCM_INST_NUM_ &(ps_reqBuf->s_hdr),
                                pb_txFrame);
  }
  /* no else : memory allocation failed, error is already reported by the SFS */

  SCFM_TACK_PATH();
  return o_return;
}


/** @} */
