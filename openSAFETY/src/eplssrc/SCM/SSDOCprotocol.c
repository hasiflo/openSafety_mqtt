/**
 * @addtogroup SSDOC
 * @{
 *
 * @file SSDOCprotocol.c
 *
 * This file contains functionality to process a segmented or expedited SSDO Transfer.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SSDOCprotocol.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SLV10</td></tr>
 *     <tr><td>24.06.2014</td><td>Hans Pill</td><td>length from SSDO upload needs to be forwarded to the callback function</td></tr>
 *     <tr><td>11.09.2014</td><td>Hans Pill</td><td>added functionality for preload</td></tr>
 *     <tr><td>25.09.2014</td><td>Hans Pill</td><td>preload is not used for every upload</td></tr>
 *     <tr><td>01.06.2016</td><td>Stefan Innerhofer</td><td>call API callback only if the pointer is set, to avoid dereferencing an NULL pointer</td></tr>
 *     <tr><td>01.06.2016</td><td>Stefan Innerhofer</td><td>to avoid dereferencing a null pointer, set the API callback to a stub callback, when the state machine gets initialized</td></tr>
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
#include "SSDOCapi.h"
#include "SSDOC.h"
#include "SSDOCint.h"
#include "SSDOint.h"
#include "SSDOCerr.h"

#include "sacun.h"


/**
 * Enumeration of all valid states of the protocol FSM of the SSDO Client.
*/
typedef enum
{
  /** wait for transfer req (up-/download) */
  k_ST_WF_REQ_TRANS,

  /** wait for init upload response */
  k_ST_WF_UPLD_INIT_RESP,
  /** wait for expedited download response */
  k_ST_WF_DWNLD_RESP_EXP,
  /** wait for segmented download response */
  k_ST_WF_DWNLD_RESP_INIT_SEG,

  /** wait for middle segment download response */
  k_ST_WF_DWNLD_RESP_MID_SEG,
  /** wait for middle segment upload response */
  k_ST_WF_UPLD_RESP_MID_SEG,

  /** wait for end segment download response */
  k_ST_WF_DWNLD_RESP_END_SEG,
  /** wait for end segment upload response */
  k_ST_WF_UPLD_RESP_END_SEG
} t_PROT_FSM_STATE;


/**
 * This sub data structure contains all necessary information to process the
 * segmentation of a data blocks.
*/
typedef struct
{
  /** internal toggle bit */
  BOOLEAN o_toggleBit;
  /** remaining number of data bytes */
  UINT32 dw_restData;
  /** data size of the uploaded object */
  UINT32 dw_uploadedObjSize;
} t_PROT_FSM_SEGM;

/**
 * This sub data structure contains all necessary information to process the
 * preload segmentation of data blocks.
 */
typedef struct
{
  /** pointer to the data to be transfered */
  UINT8 *pb_startData;
  /** SaNo of the first preload SSDO */
  UINT16 w_startSaNo;
  /** SaNo of the last preload SSDO being sent */
  UINT16 w_actSaNo;
  /** SaNo of the last received response */
  UINT16 w_recSaNo;
  /** SaNo of the errornous response */
  UINT16 w_errSaNo;
  /** queue depth of the SSDO server */
  UINT8 b_recQueueSize;
  /** actual queue depth of the SSDO server */
  UINT8 b_actQueueSize;
  /** maximum amount of consecutive erroneous responses before
   * error clearing activities have to be repeated
   */
  UINT8 b_maxErrRep;
  /** raw data length of one telegram */
  UINT8 b_rawDataLen;
  /** preload download in progress */
  BOOLEAN o_dldActive;
  /** error active */
  BOOLEAN o_errActive;
  /** error repetition active */
  BOOLEAN o_errRep;
}t_PROT_FSM_PREL;

/**
 * This structured data type is to store all necessary info and data to handle a
 * segmented data transfer to (download) or from (upload) a specified SN.
*/
typedef struct
{
  /** actual state of the protocol */
  t_PROT_FSM_STATE e_actState;

  /** data transfer req number */
  UINT16 w_reqNum;
  /** ref to callback function */
  SSDOC_t_RESP_CLBK pf_respClbk;

  /** data to be down-/uploaded */
  SSDOC_t_REQ s_data;
  /** segmentation parameter */
  t_PROT_FSM_SEGM s_segm;
  /** preload parameter */
  t_PROT_FSM_PREL s_prel;

} t_PROT_FSM;


/**
 * Module global data structure array of SSDOC_cfg_MAX_NUM_FSM protocol FSM of the
 * SSDO Client.
 *
 * Each FSM processes one object transfer, Thus &lt;n&gt; FSMs can hold &lt;n&gt;
 * communication channels to &lt;n&gt; SNs.
*/
STATIC t_PROT_FSM as_ProtFsm[SSDOC_cfg_MAX_NUM_FSM] SAFE_NO_INIT_SEKTOR;

static BOOLEAN checkExpUploadRespLen(UINT8 b_respDataLen,
                                     EPLS_t_DATATYPE e_dataType);
static BOOLEAN checkResponse(UINT16 w_fsmNum, UINT8 *pb_saCmd,
                             const UINT8 *pb_respData, UINT8 b_respDataLen);
static UINT16 getProtocolFsmFree(void);
static BOOLEAN processAbortResp(UINT16 w_fsmNum, const UINT8 *pb_respData,
                                UINT8 b_respDataLen, UINT8 b_saCmd);
static void protocolFsmInit(UINT16 w_fsmNum);
static BOOLEAN processUpldRespSeg(UINT32 dw_ct, UINT16 w_fsmNum,
                                  UINT8 b_payloadLen, UINT8 b_rawDataLen,
                                  const UINT8 *pb_rawData);
static BOOLEAN processDwnldRespSeg(UINT32 dw_ct, UINT16 w_fsmNum,
                                   UINT8 b_rawDataLen);

static BOOLEAN sendPreDldSeg(UINT32 dw_ct, UINT16 w_fsmNum);

static BOOLEAN processUnknownSizeEndSeg(UINT16 w_fsmNum,
                                        const UINT8 *pb_respData,
                                        UINT8 b_rawDataLen);
static void initPreDldSeg(UINT8 const b_queuSize,UINT16 w_fsmNum, UINT8 b_rawDataLen);

static BOOLEAN processDwnldPrelRespSeg(UINT16 w_fsmNum, UINT8 b_tr);

/**
* @brief This function initiates the SSDOC Protocol Layer and the SSDOC Service layer.
*
* @return
* - TRUE  - success
* - FALSE - failure
*/
BOOLEAN SSDOC_ProtocolLayerInit(void)
{
  UINT16 w_fsmNum = 0x0000u;  /* loop counter for FSM init. */

  /* init module global data structure of &lt;n&gt; SSDOC Protocol FSM */
  for(w_fsmNum = 0u ; w_fsmNum < (UINT16)SSDOC_cfg_MAX_NUM_FSM ; w_fsmNum++)
  {
    protocolFsmInit(w_fsmNum);
  }

  SCFM_TACK_PATH();
  return SSDOC_ServiceLayerInit();
}

/**
* @brief This function handles service request repetitions in case of a precedent
* service request was not responded by the specified SSDO Server in time or not
* responded at all.
*
* Generally this function checks every SSDOC Client service FSM pausing in state
* "wait for response" for response timeout.
*
* @attention This function must not be called in SNMTS_k_ST_INITIALIZATION
* state. This function must be called at least once within the  SSDO timeout (SOD index
* EPLS_k_IDX_SSDO_COM_PARAM, sub-index EPLS_k_SUBIDX_SNMT_TIMEOUT) with OUT value
* of number of free frame > 0.
*
* @param dw_ct consecutive time, (not checked, any value allowed), valid range: any 32 bit value
*
* @retval pb_numFreeFrms
* - IN: number of openSAFETY frames are free to transmit. (checked), valid range: <> NULL, value > 0
* - OUT: remaining number of free frames. If the value is zero, then not all SSDO channel may checked.
*/
void SSDOC_BuildRequest(UINT32 dw_ct, UINT8 *pb_numFreeFrms)
{
  BOOLEAN o_break = FALSE;    /* flag to break the while loop */
  t_PROT_FSM *ps_protFsm;     /* pointer to the actual protocol FSM */
  BOOLEAN o_abort;            /* flag to signal an abort */
  UINT16 w_fsmNum = 0x0000u;  /* start value of loop counter through
                                 the list of FSM */

  /* if reference to number of free frames is invalid */
  if(pb_numFreeFrms == NULL)
  {
    /* error: reference to NULL */
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_REF_FREE_FRMS,
                  SERR_k_NO_ADD_INFO);
  }
  else  /* parameter from outside the stack are valid */
  {
    /* linear search through the list, scanning for SSDO Client FSM waiting for
      resp */
    do
    {
      /* handle preload up-/download */
      sendPreDldSeg(dw_ct,w_fsmNum);
      /* if timeout checking succeeded */
      if (SSDOC_CheckTimeout(dw_ct, pb_numFreeFrms, w_fsmNum, &o_abort))
      {
        /* if abort frame has to be sent */
        if (o_abort)
        {
          /* if transmission of the SSDO Abort Service succeeded */
          if(SSDOC_ServiceAbort(w_fsmNum, (UINT32)SOD_ABT_SSDO_TIME_OUT))
          {
            /* decrement the free number of management frames */
            (*pb_numFreeFrms)--;
          }
          else /* else : transmission failed,
                        FATAL error was reported by the SFS */
          {
            o_break = TRUE;
          }

          ps_protFsm = &as_ProtFsm[w_fsmNum];

          /* call API callback function with transfer abortion */
          if (ps_protFsm->pf_respClbk != ((SSDOC_t_RESP_CLBK)0))
          {
            ps_protFsm->pf_respClbk(ps_protFsm->w_reqNum, (UINT32)k_ABORT_LEN,
                                    (UINT32)SOD_ABT_SSDO_TIME_OUT);
          }
          else
          {
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_REF_CALLBACK_NULL,
                          w_fsmNum);
          }

          /* init protocol FSM */
          protocolFsmInit(w_fsmNum);
        }
        /* no else no abort frame has to be sent */

        w_fsmNum++; /* increment loop counter */
      }
      else /* the timeout checking failed */
      {
        /* init protocol FSM */
        protocolFsmInit(w_fsmNum);

        o_break = TRUE;  /* leave while loop ahead of time */
      }
    }
    while((w_fsmNum < (UINT16)SSDOC_cfg_MAX_NUM_FSM) && !o_break);
      /* (loop, while loop counter is less max. number of busy FSM)  AND
        (loop, while no error occurs) */
  }

  SCFM_TACK_PATH();
}

/**
* @brief This function processes a single service response that was identified by the
* control unit (SSC) as a Service Data Object Response from a SSDO Server.
*
* The SSDO Client can only process SSDO Server Service Responses. This function searches
* for the matching SSDO Client Protocol FSM which is waiting for the received service
* response and passes the service response to the matching SSDO Client Protocol FSM.
*
* @param dw_ct consecutive time, (not checked, any value allowed), valid range: any 32 bit value
*
* @param ps_respHdr ref to resp header information (not checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL
*
* @param pb_respData ref to resp data (not checked, only called with reference to array in processStateReqProc()), valid range: <> NULL
*
* @return
* - TRUE             - resp successfully processed
* - FALSE            - resp failed to process
*/
BOOLEAN SSDOC_ProcessResponse(UINT32 dw_ct, const EPLS_t_FRM_HDR *ps_respHdr,
                              const UINT8 *pb_respData)
{
  BOOLEAN o_return = FALSE;             /* predefined return value */
  UINT16 w_fsmNum = k_INVALID_FSM_NUM;  /* number of FSM processing the
                                           received resp */

  /* get number of FSM which processes the currently received resp */
  w_fsmNum = SSDOC_ServiceResponseAssign(ps_respHdr->b_id, ps_respHdr->w_adr,
                                         ps_respHdr->w_tadr, ps_respHdr->w_sdn);

  /* if the acquired FSM number is NOT valid */
  if(w_fsmNum == k_INVALID_FSM_NUM)
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_NOT_ASSIGNED,
                  (UINT32)(ps_respHdr->b_id));
  }
  else  /* received resp is valid and can be processed by the service FSM */
  {
    /* if the received response is valid */
    if (SSDOC_ServiceFsmProcess(dw_ct, w_fsmNum, k_EVT_RX_RESP,
                                ps_respHdr->w_ct, ps_respHdr->b_tr))
    {
      /* call back the registered function of the req */
      if (!SSDOC_ProtocolFsmProcess(dw_ct, w_fsmNum, pb_respData,
                                    ps_respHdr->b_le,ps_respHdr->b_tr))
      {
        /* init protocol FSM */
        protocolFsmInit(w_fsmNum);
      }
      else
      {
        o_return = TRUE;
      }
    }
    /* no else : response is invalid and is ignored */
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function checks whether one FSM is available to process SSDO data transfer.
*
* @see SSDOC_SendReadReq()
* @see SSDOC_SendWriteReq()
*
* @return
* - TRUE  - at least one SSDO Client FSM available
* - FALSE - no SSDO Client FSM available
*/

BOOLEAN SSDOC_CheckFsmAvailable(void)
{
  BOOLEAN o_return = FALSE;            /* predefined return value */

  /* if at least one SSDO Client FSM available */
  if (getProtocolFsmFree() != k_INVALID_FSM_NUM)
  {
    o_return = TRUE;
  }
  /* no else : no SSDO Client FSM available */

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function processes the SSDO Server responses and sends the next SSDOC
* request if it is necessary.
*
* @param        dw_ct                consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_fsmNum             protocol FSM number (not checked, checked in SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        pb_respData          ref to resp data (not checked, only called with reference to array in processStateReqProc()), valid range: <> NULL
*
* @param        b_respDataLen        number of received data bytes (not checked, checked in checkResponse() and checkRxDataLength()), valid range: k_DATA_HDR_LEN .. k_MAX_DATA_LEN
*
* @param        b_tr                 tr field of the response frame
*
* @return
* - TRUE              - FSM processing successful
* - FALSE             - FSM processing failed
*/
BOOLEAN SSDOC_ProtocolFsmProcess(UINT32 dw_ct, UINT16 w_fsmNum,
                                 const UINT8 *pb_respData, UINT8 b_respDataLen,
                                 UINT8 b_tr)
{
  BOOLEAN o_return = FALSE;  /* return value of sub functions */
  t_PROT_FSM *ps_fsm = &as_ProtFsm[w_fsmNum]; /* pointer to the actual protocol
                                                 FSM */
  UINT8 b_saCmd;  /* SOD access command byte */
  UINT8 b_rawDataLen; /* variable to calculate the length of the raw data */
  UINT32 dw_rxObjSize; /* variable to calculate the length of the raw data */

  /* if response is valid */
  if (checkResponse(w_fsmNum, &b_saCmd, pb_respData, b_respDataLen))
  {
    /* reset the toggle (toggle bit was already checked) */
    EPLS_BIT_RESET_U8(b_saCmd, k_TOGGLE_SACMD);

    /* check current state of FSM */
    switch(as_ProtFsm[w_fsmNum].e_actState)
    {
      #pragma CTC SKIP
      case k_ST_WF_REQ_TRANS:
      {
        SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_PROT_FSM_STATE,
                      SERR_k_NO_ADD_INFO);
        break;
      }
      #pragma CTC ENDSKIP
      case k_ST_WF_UPLD_INIT_RESP: /* wait for upload init response */
      {
        /* if expedited upload init response received */
        if (b_saCmd == k_UPLD_INIT_EXP)
        {
          /* if the response data length is OK */
          if (checkExpUploadRespLen(b_respDataLen, ps_fsm->s_data.e_dataType))
          {
            b_rawDataLen = (UINT8)(b_respDataLen - k_DATA_HDR_LEN);
            /* if read data is copied into memory block of application */
            if(SFS_NetworkCopyGen(ps_fsm->s_data.pb_data,
                                  &(pb_respData[k_OFS_PYLD_INIT_EXP]),
                                  (UINT32)b_rawDataLen,
                                  ps_fsm->s_data.e_dataType))
            {
              /* call API callback function to pass the read SOD object data to
                the application. */
              ps_fsm->pf_respClbk(ps_fsm->w_reqNum, (UINT32)b_rawDataLen,
                                  (UINT32)SOD_ABT_NO_ERROR);

              /* init protocol FSM */
              protocolFsmInit(w_fsmNum);
              o_return = TRUE;
            }
            /* no else :  FATAL error, type of data to be copied is not
                          supported, reject SSDO service request */
          }
          else /* response data length is invalid, frame is ignored */
          {
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN1,
                          (UINT32)b_respDataLen);

            /* call API callback function */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                  (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
          }
        }
        /* else if segmented upload init response received */
        else if (b_saCmd == k_UPLD_INIT_SEG)
        {
          /* if response data length is smaller than max SSDO payload size and
                at least one raw data byte received */
          if ((b_respDataLen <= ps_fsm->s_data.b_payloadLen) &&
              (b_respDataLen >= (k_DATA_HDR_LEN + k_DATA_OBJ_LEN)))
          {
            /* read and store SOD object length from data header */
            SFS_NET_CPY32(&(dw_rxObjSize), &(pb_respData[k_OFS_OBJ_LEN]));

            /* if received data is smaller equal than the response buffer */
            if (dw_rxObjSize <= ps_fsm->s_data.dw_dataLen)
            {
              /* if the data length is unknown (zero) */
              if (dw_rxObjSize == k_OBJ_SIZE_UNKNOWN)
              {
                /* expected data length is the response buffer size */
                ps_fsm->s_segm.dw_restData = ps_fsm->s_data.dw_dataLen;
                ps_fsm->s_data.dw_dataLen = k_OBJ_SIZE_UNKNOWN;
              }
              else /* the data length is known */
              {
                /* expected data length is the received object size */
                ps_fsm->s_segm.dw_restData = dw_rxObjSize;
              }

              b_rawDataLen = (UINT8)(b_respDataLen -
                                     (k_DATA_HDR_LEN + k_DATA_OBJ_LEN));

              o_return = processUpldRespSeg(dw_ct, w_fsmNum, b_respDataLen,
                                            b_rawDataLen, &(pb_respData
                                            [k_OFS_PYLD_INIT_SEG]));
            }
            else /* received data is larger than the response buffer */
            {
              /* if transmission of the SSDO Abort Service succeeded */
              if(SSDOC_ServiceAbort(w_fsmNum,
                                    (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH))
              {
                SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RX_OBJ_SIZE,
                            dw_rxObjSize);

                /* call API callback function */
                ps_fsm->pf_respClbk(ps_fsm->w_reqNum, dw_rxObjSize,
                                    (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
              }
              /* no else : transmission failed,
                          FATAL error was reported by the SFS */
            }
          }
          else /* response data length is invalid, frame is ignored */
          {
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN2,
                          (UINT32)b_respDataLen);

                        /* call API callback function */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                  (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
          }
        }
        else /* abort response or invalid response received */
        {
          o_return = processAbortResp(w_fsmNum, pb_respData, b_respDataLen,
                                      b_saCmd);
        }
        break;
      }
      case k_ST_WF_UPLD_RESP_MID_SEG:
      {
        /* if upload middle segment response received */
        if (b_saCmd == k_UPLD_SEG_MID)
        {
          /* if response data length is smaller than max SSDO payload size and
                at least one raw data byte received */
          if ((b_respDataLen <= ps_fsm->s_data.b_payloadLen) &&
              (b_respDataLen > (UINT8)k_DATA_HDR_LEN_SEG))
          {
              b_rawDataLen = (UINT8)(b_respDataLen - k_DATA_HDR_LEN_SEG);

              o_return =  processUpldRespSeg(dw_ct, w_fsmNum, b_respDataLen,
                                             b_rawDataLen,
                                             &(pb_respData[k_OFS_PYLD_SEG]));
          }
          else /* response data length is invalid, frame is ignored */
          {
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN3,
                          (UINT32)b_respDataLen);

            /* call API callback function */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
          }
        }
        /* else if upload end segment response received */
        else if (b_saCmd == k_UPLD_SEG_END)
        {
          /* if unknown object size was received in the init segment */
          if (ps_fsm->s_data.dw_dataLen == k_OBJ_SIZE_UNKNOWN)
          {
            /* if response data length is smaller than max SSDO payload size and
                  at least one raw data byte received */
            if ((b_respDataLen <= ps_fsm->s_data.b_payloadLen) &&
                (b_respDataLen > (UINT8)k_DATA_HDR_LEN_SEG))
            {
              b_rawDataLen = (UINT8)(b_respDataLen - k_DATA_HDR_LEN_SEG);

              o_return = processUnknownSizeEndSeg(w_fsmNum, pb_respData,
                                                  b_rawDataLen);
            }
            else /* response data length is invalid, frame is ignored */
            {
              SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN11,
                            (UINT32)b_respDataLen);

              /* call API callback function */
              ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                  (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
            }
          }
          else /* unexpected response received, frame is ignored */
          {
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_SACMD_INV1,
                          (UINT32)SOD_ABT_CMD_ID_INVALID);

            /* call API callback function */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                (UINT32)SOD_ABT_CMD_ID_INVALID);
          }
        }
        else /* abort response or invalid response received */
        {
          o_return = processAbortResp(w_fsmNum, pb_respData, b_respDataLen,
                                      b_saCmd);
        }
        break;
      }
      case k_ST_WF_UPLD_RESP_END_SEG:
      {
        /* if upload end segment response received */
        if (b_saCmd == k_UPLD_SEG_END)
        {
          /* if response data length is smaller than max SSDO payload size and
                at least one raw data byte received */
          if ((b_respDataLen <= ps_fsm->s_data.b_payloadLen) &&
              (b_respDataLen > (UINT8)k_DATA_HDR_LEN_SEG))
          {
            b_rawDataLen = (UINT8)(b_respDataLen - k_DATA_HDR_LEN_SEG);

            /* if unknown object size was received in the init segment */
            if (ps_fsm->s_data.dw_dataLen == k_OBJ_SIZE_UNKNOWN)
            {
              o_return = processUnknownSizeEndSeg(w_fsmNum, pb_respData,
                                                  b_rawDataLen);
            }
            else /* object size is known */
            {
              /* if all data bytes was exact uploaded */
              if (ps_fsm->s_segm.dw_restData == b_rawDataLen)
              {
                ps_fsm->s_segm.dw_uploadedObjSize =
                    ps_fsm->s_segm.dw_uploadedObjSize + b_rawDataLen;

                /* store read/ received SOD object data via reference to data
                  segment into provided memory block */
                SFS_NET_CPY_DOMSTR(ps_fsm->s_data.pb_data,
                                  &(pb_respData[k_OFS_PYLD_SEG]), b_rawDataLen);

                /* call API callback function to pass the read SOD object data
                   to the application */
                ps_fsm->pf_respClbk(ps_fsm->w_reqNum,
                                    ps_fsm->s_segm.dw_uploadedObjSize,
                                    (UINT32)SOD_ABT_NO_ERROR);

                /* init protocol FSM */
                protocolFsmInit(w_fsmNum);

                o_return = TRUE;
              }
              else /* not all data bytes was exact uploaded */
              {
                SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_REST_DATA_LEN2,
                              (UINT32)b_rawDataLen);

                /* call API callback function */
                ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                    (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
              }
            }
          }
          else /* response data length is invalid, frame is ignored */
          {
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN4,
                          (UINT32)b_respDataLen);

            /* call API callback function */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
          }
        }
        else /* abort response or invalid response received */
        {
          o_return = processAbortResp(w_fsmNum, pb_respData, b_respDataLen,
                                      b_saCmd);
        }
        break;
      }

      case k_ST_WF_DWNLD_RESP_EXP:
      {
        /* if download init expedited response received */
        if (b_saCmd == k_DWNLD_INIT_EXP)
        {
          /* if response data length is valid */
          if (b_respDataLen == k_DATA_HDR_LEN)
          {
            /* call API callback function to acknowledge SOD object data is
              written. */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, ps_fsm->s_data.dw_dataLen,
                                (UINT32)SOD_ABT_NO_ERROR);

            /* init protocol FSM */
            protocolFsmInit(w_fsmNum);

            o_return = TRUE;
          }
          else /* response data length is invalid, frame is ignored */
          {
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN5,
                          (UINT32)b_respDataLen);

            /* call API callback function */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
          }
        }
        else /* abort response or invalid response received */
        {
          o_return = processAbortResp(w_fsmNum, pb_respData, b_respDataLen,
                                      b_saCmd);
        }
        break;
      }
      case k_ST_WF_DWNLD_RESP_INIT_SEG:
      {
        /* if download init segmented response received */
        if (b_saCmd == k_DWNLD_INIT_SEG)
        {
          /* if response data length is valid */
          if (b_respDataLen == k_DATA_HDR_LEN)
          {
            b_rawDataLen = (UINT8)(ps_fsm->s_data.b_payloadLen -
                                   (k_DATA_HDR_LEN + k_DATA_OBJ_LEN));
            o_return = processDwnldRespSeg(dw_ct, w_fsmNum, b_rawDataLen);
          }
          else /* response data length is invalid, frame is ignored */
          {
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN6,
                          (UINT32)b_respDataLen);

            /* call API callback function */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
          }
        }
        /* if download init segmented response received */
        else if (b_saCmd == k_DWNLD_SEG_PRE_INIT)
        {
          /* if response data length is valid */
          if (b_respDataLen == k_DATA_HDR_LEN)
          {
            b_rawDataLen = (UINT8)(ps_fsm->s_data.b_payloadLen -
                                   (k_DATA_HDR_LEN + k_DATA_OBJ_LEN));
            initPreDldSeg(b_tr & k_QLEN_PREL_TR,w_fsmNum,b_rawDataLen);
            o_return = TRUE;
          }
          else /* response data length is invalid, frame is ignored */
          {
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN6,
                          (UINT32)b_respDataLen);

            /* call API callback function */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
          }
        }
        else /* abort response or invalid response received */
        {
          o_return = processAbortResp(w_fsmNum, pb_respData, b_respDataLen,
                                      b_saCmd);
        }
        break;
      }
      case k_ST_WF_DWNLD_RESP_MID_SEG:
      {
        /* if download middle segmented response received */
        if (b_saCmd == k_DWNLD_SEG_MID)
        {
          /* if response data length is valid */
          if (b_respDataLen == k_DATA_HDR_LEN_SEG)
          {
            o_return = processDwnldRespSeg(dw_ct, w_fsmNum,
                                           (UINT8)(ps_fsm->s_data.b_payloadLen -
                                           k_DATA_HDR_LEN_SEG));
          }
          else /* response data length is invalid, frame is ignored */
          {
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN7,
                          (UINT32)b_respDataLen);

            /* call API callback function */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
          }
        }
        /* if download pre-load middle segment response is received */
        else if (b_saCmd == k_DWNLD_SEG_PRE_MID)
        {
            /* if response data length is valid */
            if (b_respDataLen == k_DATA_HDR_LEN_SEG)
            {
              o_return = processDwnldPrelRespSeg(w_fsmNum,b_tr);
            }
            else /* response data length is invalid, frame is ignored */
            {
              SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN7,
                            (UINT32)b_respDataLen);

              /* call API callback function */
              ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                  (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
            }
        }
        else /* abort response or invalid response received */
        {
          o_return = processAbortResp(w_fsmNum, pb_respData, b_respDataLen,
                                      b_saCmd);
        }
        break;
      }
      case k_ST_WF_DWNLD_RESP_END_SEG:
      {
        /* if download end segmented response received */
        if (b_saCmd == k_DWNLD_SEG_END)
        {
          /* if response data length is valid */
          if (b_respDataLen == k_DATA_HDR_LEN_SEG)
          {
            /* call API callback function to acknowledge SOD object data is
               written. */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, ps_fsm->s_data.dw_dataLen,
                                (UINT32)SOD_ABT_NO_ERROR);

            /* init protocol FSM */
            protocolFsmInit(w_fsmNum);

            o_return = TRUE;
          }
          else /* response data length is invalid, frame is ignored */
          {
            SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN8,
                          (UINT32)b_respDataLen);

            /* call API callback function */
            ps_fsm->pf_respClbk(ps_fsm->w_reqNum, 0UL,
                                (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
          }
        }
        else /* abort response or invalid response received */
        {
          o_return = processAbortResp(w_fsmNum, pb_respData, b_respDataLen,
                                      b_saCmd);
        }
        break;
      }
      #pragma CTC SKIP
      default:
      {
        SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_PROT_STATE_INV,
                      (UINT32)(ps_fsm->e_actState));
      }
      #pragma CTC ENDSKIP
    }
  }
  else /* response is invalid, error was reported, frame is ignored */
  {
    o_return = TRUE;
  }

  SCFM_TACK_PATH();
  return o_return;

  /* RSM_IGNORE_QUALITY_BEGIN Notice #28   - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */


/**
* @brief This function reserves a free protocol FSM, stores the request information
* for the FSM, sends a SSDO request and sets the FSM into the corresponding "wait for
* response" state.
*
* @param        w_sadr                 target address of SN to be accessed (not checked,
* 	checked in SSDOC_SendReadReq() or SSDOC_SendWriteReq()), valid range:
* 	EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @param        w_reqNum               service request number (handle)(not checked, any
* 	value allowed), valid range: UINT16
*
* @param        pf_respClbk            reference to a function that will be called at the
* 	end of a transfer (not checked checked in SSDOC_SendReadReq() or SSDOC_SendWriteReq()),
* 	valid range: <> NULL
*
* @param        dw_ct                  consecutive time (not checked, any value allowed),
* 	valid range: UINT32
*
* @param        ps_req                 request info of data to be process (not checked,
* 	checked in SSDOC_SendReadReq() or SSDOC_SendWriteReq()), valid range: see SSDOC_t_REQ
*
* @param        o_downloadReq
*  - TRUE  - download request is started
*  - FALSE - upload request is started (not checked, any value allowed)
*
* @param        o_fastDld              fast download requested
*
* @return
* - TRUE                 - success
* - FALSE                - failure
*/
BOOLEAN SSDOC_SendReq(UINT16 w_sadr, UINT16 w_reqNum,
                      SSDOC_t_RESP_CLBK pf_respClbk, UINT32 dw_ct,
                      const SSDOC_t_REQ *ps_req, BOOLEAN o_downloadReq,
                      BOOLEAN const o_fastDld)
{
  BOOLEAN o_return = FALSE;    /* predefine return value */
  t_PROT_FSM *ps_protFsm; /* pointer to the actual protocol FSM */
  UINT16 w_fsmNum;  /* index of the free protocol FSM */

  /* get a free protocol FSM */
  w_fsmNum = getProtocolFsmFree();

  /* if no free protocol FSM is available */
  if(w_fsmNum == k_INVALID_FSM_NUM)
  {
    /* error: no free protocol FSM */
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_NO_PROT_FSM,
                  SERR_k_NO_ADD_INFO);
  }
  else  /* protocol FSM is available */
  {
    ps_protFsm = &as_ProtFsm[w_fsmNum];

    /* store data to call the API callback function */
    ps_protFsm->w_reqNum = w_reqNum;
    ps_protFsm->pf_respClbk = pf_respClbk;

    /* store control parameter into FSM */
    /* reset internal toggle bit at the beginning of a transfer */
    ps_protFsm->s_segm.o_toggleBit = FALSE;

    /* store data/payload into FSM */
    ps_protFsm->s_data = *ps_req;

    /* if download request is started */
    if(o_downloadReq)
    {
      /* if data type of object to be accessed is EPLS_k_VISIBLE_STRING or
        EPLS_k_DOMAIN or EPLS_k_OCTET_STRING */
      if((ps_protFsm->s_data.e_dataType == EPLS_k_VISIBLE_STRING) ||
         (ps_protFsm->s_data.e_dataType == EPLS_k_DOMAIN) ||
         (ps_protFsm->s_data.e_dataType == EPLS_k_OCTET_STRING))
      {

        /* if object length is BIGGER THAN maximum number of payload bytes to be
          transferred into a single frame */
        if(ps_protFsm->s_data.dw_dataLen >
           ((UINT32)(ps_protFsm->s_data.b_payloadLen) - k_DATA_HDR_LEN))
        {
          /* set the counter of remaining data bytes to the object size */
          ps_protFsm->s_segm.dw_restData = ps_protFsm->s_data.dw_dataLen;

          /* if initialization of segmented download succeeded */
          if(SSDOC_ServiceDwnldInit(w_fsmNum, dw_ct, w_sadr,
                                    FALSE, &ps_protFsm->s_data, o_fastDld))
          {
            /* wait for resp of initiate transfer */
            ps_protFsm->e_actState = k_ST_WF_DWNLD_RESP_INIT_SEG;
            o_return = TRUE;
          }
          /* no else : error, is already reported, return with predefined */
        }
        else
        {
          ps_protFsm->s_data.b_payloadLen =
              (UINT8)(ps_protFsm->s_data.dw_dataLen + k_DATA_HDR_LEN);

          /* if initialization of expedited download succeeded */
          if(SSDOC_ServiceDwnldInit(w_fsmNum, dw_ct, w_sadr,
                                    TRUE, &ps_protFsm->s_data, o_fastDld))
          {
            /* wait for resp of initiate transfer */
            ps_protFsm->e_actState = k_ST_WF_DWNLD_RESP_EXP;
            o_return = TRUE;
          }
          /* no else : error, is already reported */
        }
      }
      else  /* object can be transferred in exactly one SINGLE EPLS frame
              (max. 250 bytes) */
      {
        ps_protFsm->s_data.b_payloadLen =
            (UINT8)(ps_protFsm->s_data.dw_dataLen + k_DATA_HDR_LEN);

        /* if initialization of expedited download succeeded */
        if(SSDOC_ServiceDwnldInit(w_fsmNum, dw_ct, w_sadr,
                                  TRUE, &ps_protFsm->s_data, FALSE))
        {
          /* wait for resp of initiate transfer */
          ps_protFsm->e_actState = k_ST_WF_DWNLD_RESP_EXP;
          o_return = TRUE;
        }
        /* no else : error, is already reported, return with predefined FALSE */
      }
    }
    /* else upload request is started */
    else
    {
      /* if initialization of expedited upload succeeded */
      if(SSDOC_ServiceUpldInit(w_fsmNum, dw_ct, w_sadr, ps_req))
      {
        /* wait for resp of initiate transfer */
        ps_protFsm->e_actState = k_ST_WF_UPLD_INIT_RESP;
        o_return = TRUE;
      }
      /* no else : error, is already reported, return with predefined */
    }

    /* if init of SSDO Transfer (expedited or segmented) causes an error */
    if(!o_return)
    {
      /* init Protocol FSM in case of a SW error is returned to the SCM */
      protocolFsmInit(w_fsmNum);
    }
    /* no else */
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function checks the SSDO expedited upload response length.
*
* @param   b_respDataLen             response data length to be checked (checked,
* 	additional checked in checkRxDataLength())
* @param   e_dataType                data type of the transferred object (not checked,
* 	checked in TypeLenValid())
*
* @return
* - TRUE               - success
* - FALSE              - failure
*/
static BOOLEAN checkExpUploadRespLen(UINT8 b_respDataLen,
                                     EPLS_t_DATATYPE e_dataType)
{
  BOOLEAN o_return = FALSE;  /* return value of sub functions */
  UINT8 b_rawDataLen; /* length of the raw data */

  /* the response data length (b_respDataLen >= k_DATA_HDR_LEN) is checked in
     checkResponse() */

  /* if standard data type was requested */
  if ((e_dataType != EPLS_k_DOMAIN) && (e_dataType != EPLS_k_OCTET_STRING) &&
      (e_dataType != EPLS_k_VISIBLE_STRING))
  {
    b_rawDataLen = (UINT8)(b_respDataLen - k_DATA_HDR_LEN);

    switch (b_rawDataLen)
    {
      case 1: /* response data length is 1 byte */
      {
        /* if data type is OK */
        if ((e_dataType == EPLS_k_INT8) || (e_dataType == EPLS_k_UINT8) ||
            (e_dataType == EPLS_k_BOOLEAN))
        {
          o_return = TRUE;
        }
        break;
      }
      case 2: /* response data length is 2 byte */
      {
        /* if data type is OK */
        if ((e_dataType == EPLS_k_INT16) || (e_dataType == EPLS_k_UINT16))
        {
          o_return = TRUE;
        }
        break;
      }
      case 4: /* response data length is 4 byte */
      {
        /* if data type is OK */
        if ((e_dataType == EPLS_k_INT32) || (e_dataType == EPLS_k_UINT32) ||
            (e_dataType == EPLS_k_REAL32))
        {
          o_return = TRUE;
        }
        break;
      }
      #if (EPLS_cfg_MAX_PYLD_LEN >= 12)
        case 8: /* response data length is 8 byte */
        {
          /* if data type is OK */
          if ((e_dataType == EPLS_k_INT64) || (e_dataType == EPLS_k_UINT64) ||
              (e_dataType == EPLS_k_REAL64))
          {
            o_return = TRUE;
          }
          break;
        }
      #endif
      default:
      {
        /* invalid response data length */
        break;
      }
    }
  }
  else /* response data length is OK */
  {
    o_return = TRUE;
  }

  SCFM_TACK_PATH();
  return o_return;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */

/**
* @brief This function checks the SSDO response frame.
*
* Toggle bit, data length, index and sub-index are checked.
*
* @param        w_fsmNum             protocol FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()),
* 	valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @retval          pb_saCmd             pointer to the SOD access command byte (pointer
* 	not checked, only called with reference to variable), valid range : <> NULL
*
* @param        pb_respData          ref to resp data (not checked, only called with
* 	reference to array in processStateReqProc()), valid range: <> NULL
*
* @param        b_respDataLen        number of received data bytes (not checked,
* 	checked in checkResponse() and checkRxDataLength()),
* 	valid range: k_DATA_HDR_LEN .. k_MAX_DATA_LEN
*
* @return
* - TRUE               - success
* - FALSE              - failure
*/
static BOOLEAN checkResponse(UINT16 w_fsmNum, UINT8 *pb_saCmd,
                             const UINT8 *pb_respData, UINT8 b_respDataLen)
{
  BOOLEAN o_return = FALSE;  /* return value of sub functions */
  t_PROT_FSM *ps_protFsm = &as_ProtFsm[w_fsmNum]; /* pointer to the actual
                                                     protocol FSM */
  UINT16 w_idx;
  UINT8 b_subIdx;
  UINT8 b_saCmd;
  /* get SaCmd byte because for uploading segments there are special rules */
  SFS_NET_CPY8(&b_saCmd, &pb_respData[k_OFS_SACMD]);
  b_saCmd = b_saCmd & ~k_TOGGLE_SACMD;
  /* if preload is used, telegrams may get lost sometimes,
   * therefore there is not toggle bit check, but the SaNo gets validated */
  if (((b_respDataLen == k_DATA_HDR_LEN_SEG) &&
        (b_saCmd == k_DWNLD_SEG_PRE_MID)))
  {
    /* deserialize SOD command byte */
    SFS_NET_CPY8(pb_saCmd, &pb_respData[k_OFS_SACMD]);
    o_return = TRUE;
  }
  /* segmented download does have no idx/sidx in the response
   * segmented upload also has none of this */
  else if ((b_respDataLen == k_DATA_HDR_LEN_SEG) ||
           ((b_respDataLen >= k_DATA_HDR_LEN_SEG) &&
            ((k_UPLD_SEG_MID == b_saCmd) ||
             (k_UPLD_SEG_END == b_saCmd))))
  {
    /* deserialize SOD command byte, index and subindex */
    SFS_NET_CPY8(pb_saCmd, &pb_respData[k_OFS_SACMD]);

    /* if the toggle bit is invalid */
    if(EPLS_IS_BIT_SET((*pb_saCmd), k_TOGGLE_SACMD) &&
       !ps_protFsm->s_segm.o_toggleBit)
    {
      /* error: toggle bit of received SOD Access Command is wrong */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_TB_WRONG,
                    (UINT32)SOD_ABT_CMD_ID_INVALID);

      /* call API callback function */
      ps_protFsm->pf_respClbk(ps_protFsm->w_reqNum, 0UL,
                              (UINT32)SOD_ABT_CMD_ID_INVALID);
    }
    else
    {
      o_return = TRUE;
    }
  }
  /* if response data is valid */
  else if (b_respDataLen >= k_DATA_HDR_LEN)
  {
    /* deserialize SOD command byte, index and subindex */
    SFS_NET_CPY8(pb_saCmd, &pb_respData[k_OFS_SACMD]);
    SFS_NET_CPY16(&(w_idx), &pb_respData[k_OFS_IDX]);
    SFS_NET_CPY8(&(b_subIdx), &pb_respData[k_OFS_SIDX]);

    /* if the toggle bit is invalid */
    if(EPLS_IS_BIT_SET((*pb_saCmd), k_TOGGLE_SACMD) &&
       !ps_protFsm->s_segm.o_toggleBit)
    {
      /* error: toggle bit of received SOD Access Command is wrong */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_TB_WRONG,
                    (UINT32)SOD_ABT_CMD_ID_INVALID);

      /* call API callback function */
      ps_protFsm->pf_respClbk(ps_protFsm->w_reqNum, 0UL,
                              (UINT32)SOD_ABT_CMD_ID_INVALID);
    }
    /* else if response index number is different to request index number */
    else if(w_idx != ps_protFsm->s_data.w_idx)
    {
      /* error: index of the received SOD object is wrong */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_IDX_WRONG,
                    (UINT32)SOD_ABT_GEN_PARAM_INCOMP);

      /* call API callback function */
      ps_protFsm->pf_respClbk(ps_protFsm->w_reqNum, 0UL,
                              (UINT32)SOD_ABT_GEN_PARAM_INCOMP);
    }
    /* else if response sub index number is different to req sub index number */
    else if(b_subIdx != ps_protFsm->s_data.b_subIdx)
    {
      /* error: sub index of the received SOD object is wrong */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_SUBIDX_WRONG,
                    (UINT32)SOD_ABT_GEN_PARAM_INCOMP);

      /* call API callback function */
      ps_protFsm->pf_respClbk(ps_protFsm->w_reqNum, 0UL,
                              (UINT32)SOD_ABT_GEN_PARAM_INCOMP);
    }
    else
    {
      o_return = TRUE;
    }
  }
  else /* response data length is invalid, frame is ignored */
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN9,
                  (UINT32)b_respDataLen);

    /* call API callback function */
    ps_protFsm->pf_respClbk(ps_protFsm->w_reqNum, 0UL,
                            (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function returns the number of the next free protocol FSM.
*
* If no FSM is free to process a new transfer k_INVALID_FSM_NUM; is returned.
*
* @return       0 .. SSDOC_cfg_MAX_NUM_FSM - 1  - number of free FSM k_INVALID_FSM_NUM                 - no free FSM available
*/
static UINT16 getProtocolFsmFree(void)
{
  UINT16 w_return = k_INVALID_FSM_NUM;  /* predefined invalid number */
  UINT16 w_fsmNum = 0x0000u;           /* start value of search index */

  /* linear search for the next free FSM */
  do
  {
    /* if state of current FSM is "wait for transfer req" */
    if(as_ProtFsm[w_fsmNum].e_actState == k_ST_WF_REQ_TRANS)
    {
      w_return = w_fsmNum;        /* prepare return value */
    }
    else  /* current FSM is in state "wait for resp" and NOT available */
    {
      w_fsmNum++;  /* increment search index, FSM NOT found */
    }
  }
  /* search while next free FSM NOT found or max. number of FSM not reached */
  while((w_return == k_INVALID_FSM_NUM) &&
        (w_fsmNum < (UINT16)SSDOC_cfg_MAX_NUM_FSM));

  SCFM_TACK_PATH();
  return w_return;
}

/**
* @brief This function processes an SSDO abort response.
*
* @param        w_fsmNum             protocol FSM number (not checked, checked
* 	in SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()),
* 	valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        pb_respData          ref to resp data (not checked, only called
* 	with reference to array in processStateReqProc()), valid range: <> NULL
*
* @param        b_respDataLen        number of received data bytes (not checked,
* 	checked in checkResponse() and checkRxDataLength()),
* 	valid range: k_DATA_HDR_LEN .. k_MAX_DATA_LEN
*
* @param        b_saCmd              SOD access command byte (checked),
* 	valid range : k_ABORT
*
* @return
* - TRUE               - FSM processing successful
* - FALSE              - FSM processing failed
*/
static BOOLEAN processAbortResp(UINT16 w_fsmNum, const UINT8 *pb_respData,
                                UINT8 b_respDataLen, UINT8 b_saCmd)
{
  BOOLEAN o_return = FALSE;  /* return value of sub functions */
  BOOLEAN o_transferAbort = FALSE; /* check-value for abort situation */
  t_PROT_FSM *ps_protFsm = &as_ProtFsm[w_fsmNum]; /* pointer to the actual
                                                     protocol FSM */
  UINT32 dw_abortCode; /* temporary variable for the abort code */

  /* if SSDO abort response received */
  if (b_saCmd == k_ABORT)
  {
    /* if response data length is valid */
    if (b_respDataLen == (k_DATA_HDR_LEN + k_ABORT_LEN))
    {
      /* copy abort code */
      SFS_NET_CPY32(&dw_abortCode, &pb_respData[k_OFS_ABORT_CODE]);
      /* switch off preload if the command is not known by the SSDO server */
      if (dw_abortCode == (UINT32)SOD_ABT_CMD_ID_INVALID)
      {
        /* switch off preload if the command is not known by the SSDO server */
        if (SSDOC_ReqBufferPreLoadOff(w_fsmNum))
        {
          SSDOC_TimerToTimeout(w_fsmNum);
          SSDOC_ServiceFsmSetWaitForReq(w_fsmNum,FALSE);
          o_transferAbort = FALSE;
        }
        /* transfer abort, this is a valid ssdo abort */
        else
        {
          o_transferAbort = TRUE;
        }
      }
      /* transfer abort */
      else
      {
        o_transferAbort = TRUE;
      }

      /* perform ssdo abort */
      if ( o_transferAbort == TRUE )
      {
          /* call SCM because of transfer abortion of the SSDO server */
          ps_protFsm->pf_respClbk(ps_protFsm->w_reqNum, 0x00UL, dw_abortCode);

          /* switch to "wait for request transmission" state */
          ps_protFsm->e_actState = k_ST_WF_REQ_TRANS;
      }

      o_return = TRUE;
    }
    else /* response data length is invalid, frame is ignored */
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_DATA_LEN10,
                    (UINT32)b_respDataLen);

      /* call API callback function */
      ps_protFsm->pf_respClbk(ps_protFsm->w_reqNum, 0UL,
                              (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
    }
  }
  else /* NOT specified SOD Access Command */
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RESP_SACMD_INV,
                  (UINT32)SOD_ABT_CMD_ID_INVALID);

    /* call API callback function */
    ps_protFsm->pf_respClbk(ps_protFsm->w_reqNum, 0UL,
                            (UINT32)SOD_ABT_CMD_ID_INVALID);
  }

  SCFM_TACK_PATH();
  return o_return;
}
#pragma CTC SKIP
/* stup  callback function to avoid dereferencing a NULL pointer
 * Because it is just a stub function it dont has to be tested*/
void stubCallback(UINT16 w_regNum, UINT32 dw_dataLen, UINT32 dw_abortCode)
{

}
#pragma CTC ENDSKIP
/**
* @brief      This function initializes the specified protocol FSM of the SSDO Client.
*
* This function is called after successful processing the request or after any error
* which causes the SCM to abort.
*
* @param      w_fsmNum    SSDO Client FSM number (not checked, checked in
* 	SSDOC_SendReq() and getProtocolFsmFree() or SSDOC_ServiceResponseAssign() and
* 	SSDOC_ProcessResponse() or SSDOC_ProtocolLayerInit() or SSDOC_BuildRequest()),
* 	valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*/
static void protocolFsmInit(UINT16 w_fsmNum)
{
  /* initialize parameter */
  as_ProtFsm[w_fsmNum].e_actState = k_ST_WF_REQ_TRANS;

  as_ProtFsm[w_fsmNum].w_reqNum = 0x0000U;
  as_ProtFsm[w_fsmNum].pf_respClbk = stubCallback;

  as_ProtFsm[w_fsmNum].s_data.w_idx = 0x0000U;
  as_ProtFsm[w_fsmNum].s_data.b_subIdx = 0x00U;
  as_ProtFsm[w_fsmNum].s_data.dw_dataLen = 0x00UL;
  as_ProtFsm[w_fsmNum].s_data.e_dataType = EPLS_k_UINT8;
  as_ProtFsm[w_fsmNum].s_data.pb_data = (UINT8 *)NULL;
  as_ProtFsm[w_fsmNum].s_data.b_payloadLen = 0x00U;

  as_ProtFsm[w_fsmNum].s_segm.o_toggleBit = FALSE;
  as_ProtFsm[w_fsmNum].s_segm.dw_restData = 0x00UL;
  as_ProtFsm[w_fsmNum].s_segm.dw_uploadedObjSize = 0x00UL;

  SCFM_TACK_PATH();
  return;
}

/**
* @brief This function processes the SSDO Service Response "SSDO Segmented Upload".
*
* The segment is a middle segment of the upload.
*
* @param       dw_ct				consecutive time (not checked, any value allowed),
* 	valid range: any 32 bit value
*
* @param       w_fsmNum 			protocol FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()),
* 	valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param       b_payloadLen		    length of the payload data (not checked, checked
* 	in checkResponse() and checkRxDataLength()), valid range: 8..254
*
* @param       b_rawDataLen 		length of the raw data (not checked, any value
* 	allowed), valid range: UINT8
*
* @param       pb_rawData 			pointer to the raw data (not checked, only called
* 	with reference to array in SSDOC_ProtocolFsmProcess()), valid range : <> NULL
*
* @return
* - TRUE              - response processed successfully
* - FALSE             - response processing failed
*/
static BOOLEAN processUpldRespSeg(UINT32 dw_ct, UINT16 w_fsmNum,
                                  UINT8 b_payloadLen, UINT8 b_rawDataLen,
                                  const UINT8 *pb_rawData)
{
  BOOLEAN o_return = FALSE;  /* predefine return value */
  t_PROT_FSM *ps_protFsm = &as_ProtFsm[w_fsmNum]; /* pointer to the actual
                                                     protocol FSM */
  UINT8 b_saCmd; /* SOD Access Command */
  t_PROT_FSM_STATE e_nextState;

  /* change internal toggle bit */
  ps_protFsm->s_segm.o_toggleBit = (BOOLEAN)(!(ps_protFsm->s_segm.o_toggleBit));

  /* NOTE: The raw data length does not exceed the response buffer size or
     object size, otherwise end segment is excepted. */

  /* store read/ received SOD object data via reference to data
    segment into provided memory block */
  SFS_NET_CPY_DOMSTR(ps_protFsm->s_data.pb_data, pb_rawData, b_rawDataLen);
  /* change segment reference to following data segment */
  ps_protFsm->s_data.pb_data = ps_protFsm->s_data.pb_data + b_rawDataLen;

  /* calculate the number of remaining data bytes */
  ps_protFsm->s_segm.dw_restData = ps_protFsm->s_segm.dw_restData -
                                    b_rawDataLen;

  /* calculate the uploaded object size */
  ps_protFsm->s_segm.dw_uploadedObjSize =
      ps_protFsm->s_segm.dw_uploadedObjSize + b_rawDataLen;

  /* if the subsequent segment is the LAST segment to upload */
  if(ps_protFsm->s_segm.dw_restData <=
      ((UINT32)(b_payloadLen) - k_DATA_HDR_LEN_SEG))
  {
    /* The client sends always Upload middle segment */
    b_saCmd = k_UPLD_SEG_MID;
    e_nextState = k_ST_WF_UPLD_RESP_END_SEG;
  }
  else  /* the subsequent segment is a MIDDLE segment */
  {
    b_saCmd = k_UPLD_SEG_MID;
    e_nextState = k_ST_WF_UPLD_RESP_MID_SEG;
  }

  /* if toggle bit must be set */
  if(ps_protFsm->s_segm.o_toggleBit)
  {
    /* set toggle bit in SOD Access Command */
    EPLS_BIT_SET_U8(b_saCmd, k_TOGGLE_SACMD);
  }

  /* if transmission of the MIDDLE or LAST segment succeeded */
  if(SSDOC_ServiceUpldSeg(w_fsmNum, dw_ct, b_saCmd))
  {
    /* set next state */
    ps_protFsm->e_actState = e_nextState;
    o_return = TRUE;  /* prepare return value */
  }
  /* no else : error, is already reported, return with predefined */

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function processes the SSDO Service Response "SSDO Segmented Download" and
* sends the next request if it is necessary.
*
* @param 		dw_ct               consecutive time (not checked, any value allowed),
* 	valid range: any 32 bit value
*
* @param        w_fsmNum            protocol FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()),
* 	valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        b_rawDataLen        length of the raw data (not checked, any value
* 	allowed), valid range: UINT8
*
* @return
* - TRUE              - response processed successfully
* - FALSE             - response processing failed
*/
static BOOLEAN processDwnldRespSeg(UINT32 dw_ct, UINT16 w_fsmNum,
                                   UINT8 b_rawDataLen)
{
  BOOLEAN o_return = FALSE;  /* predefine return value */
  t_PROT_FSM *ps_protFsm = &as_ProtFsm[w_fsmNum]; /* pointer to the actual
                                                     protocol FSM */
  UINT8 b_saCmd;  /* SOD Access Command to be transmitted */
  t_PROT_FSM_STATE e_nextState;

  /* change internal toggle bit */
  ps_protFsm->s_segm.o_toggleBit = (BOOLEAN)(!(ps_protFsm->s_segm.o_toggleBit));

  /* change segment reference to following data segment */
  ps_protFsm->s_data.pb_data = ps_protFsm->s_data.pb_data + b_rawDataLen;

  /* calculate remaining data bytes to write after the subsequent
     segment transmission */
  ps_protFsm->s_segm.dw_restData = ps_protFsm->s_segm.dw_restData -
                                   b_rawDataLen;

  /* if subsequent segment is the LAST segment to download */
  if(ps_protFsm->s_segm.dw_restData <=
     ((UINT32)(ps_protFsm->s_data.b_payloadLen) - k_DATA_HDR_LEN_SEG))
  {
    ps_protFsm->s_data.b_payloadLen = (UINT8)(ps_protFsm->s_segm.dw_restData +
                                              k_DATA_HDR_LEN_SEG);

    b_saCmd = k_DWNLD_SEG_END;
    e_nextState = k_ST_WF_DWNLD_RESP_END_SEG;
  }
  else  /* the subsequent segment is a MIDDLE segment */
  {
    b_saCmd = k_DWNLD_SEG_MID;
    e_nextState = k_ST_WF_DWNLD_RESP_MID_SEG;
  }

  if (ps_protFsm->s_segm.o_toggleBit)
  {
    /* set toggle bit in SOD Access Command */
    EPLS_BIT_SET_U8(b_saCmd, k_TOGGLE_SACMD);
  }

  /* if transmission of a MIDDLE or LAST segment succeeded */
  if(SSDOC_ServiceDwnldSeg(w_fsmNum, dw_ct, b_saCmd, &ps_protFsm->s_data))
  {
    /* set next state */
    ps_protFsm->e_actState = e_nextState;
    o_return = TRUE;  /* prepare return value */
  }
  /* no else : error, is already reported, return with predefined */

  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function initializes the "SSDO Segmented Preload Download".
*
* @param b_queuSize size of the receive queue of the SSDO server device, valid
* range: any 8 bit value
* @param w_fsmNum protocol FSM number (not checked, checked in
* SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()),
* valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
* @param b_dataSent length of the data (not checked, any value  allowed), that
* has been sent in the initiate request, valid range: UINT8
*/
static void initPreDldSeg(UINT8 const b_queuSize,UINT16 const w_fsmNum, UINT8 const b_dataSent)
{
  t_PROT_FSM *ps_protFsm = &as_ProtFsm[w_fsmNum]; /* pointer to the actual
                                                     protocol FSM */
  t_REQ_BUFFER *ps_reqBuf; /* pointer to the request buffer */

  ps_reqBuf = SSDOC_GetReqBuffer(w_fsmNum);

  /* write the start values to the related variables */

  /* set the max queue size and the remaining queue size to the initial values */
  ps_protFsm->s_prel.b_recQueueSize = b_queuSize;
  ps_protFsm->s_prel.b_actQueueSize = b_queuSize;

  /* set the data pointer to the data next to be sent
   * in the initiate download telegram some data may already got sent */
  ps_protFsm->s_data.pb_data  = ps_protFsm->s_data.pb_data + b_dataSent;
  ps_protFsm->s_segm.dw_restData = ps_protFsm->s_segm.dw_restData - b_dataSent;

  /* remind where the data for the preload download begins */
  ps_protFsm->s_prel.pb_startData = ps_protFsm->s_data.pb_data;

  /* remind the CT value for recognizing telegram loss */
  ps_protFsm->s_prel.w_startSaNo = ps_reqBuf->s_hdr.w_ct;
  ps_protFsm->s_prel.w_actSaNo  = ps_reqBuf->s_hdr.w_ct;
  ps_protFsm->s_prel.w_recSaNo  = ps_reqBuf->s_hdr.w_ct;

  /* activate the preload download */
  ps_protFsm->s_prel.o_dldActive = TRUE;
  ps_protFsm->s_prel.o_errActive = FALSE;
  ps_protFsm->s_prel.o_errRep = FALSE;

  /* set the service bit to allow requests and responses at the same time */
  SSDOC_ServiceFsmSetPreload(w_fsmNum,TRUE);
}

/**
* @brief This function processes the SSDO Service Response "SSDO Segmented Preload Download" and
* handles received errors.
*
* @param        w_fsmNum            protocol FSM number (not checked, checked in
*     SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()),
*     valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        b_tr                value of the TR field of the received frame
*
* @return
* - TRUE              - response processed successfully
* - FALSE             - response processing failed
*/
static BOOLEAN processDwnldPrelRespSeg(UINT16 w_fsmNum,
                                   UINT8 b_tr)
{
      BOOLEAN o_return = FALSE;  /* predefine return value */
      t_PROT_FSM *ps_protFsm = &as_ProtFsm[w_fsmNum]; /* pointer to the actual
                                                         protocol FSM */

      /* no error reported */
      if ( EPLS_k_TR_NOT_USED == b_tr)
      {
          o_return = TRUE;
          ps_protFsm->s_prel.o_errActive = FALSE;
          ps_protFsm->s_prel.o_errRep = FALSE;
      }
      /* error reported - telegram missing */
      else if (k_ERR_PREL_TR == b_tr)
      {
          /* check for repetition of the error */
          if (!ps_protFsm->s_prel.o_errRep ||
              (0 == ps_protFsm->s_prel.b_maxErrRep))
          {
              /* correct the next SaNo if the received error was no repetition */
              ps_protFsm->s_prel.b_maxErrRep = ps_protFsm->s_prel.b_recQueueSize - ps_protFsm->s_prel.b_actQueueSize;
              SSDOC_GetReqBuffer(w_fsmNum)->s_hdr.w_ct = ps_protFsm->s_prel.w_recSaNo - 1;
          }
          else
          {
              /* decrease the number of repetitions to be ignored */
              ps_protFsm->s_prel.b_maxErrRep--;
          }
          /* set the return value and remind the SaNo */
          o_return = TRUE;
          ps_protFsm->s_prel.w_errSaNo = ps_protFsm->s_prel.w_recSaNo;
          ps_protFsm->s_prel.o_errActive = TRUE;
      }

      return o_return;
}

/**
* @brief This function checks the SaNo of a received SSDO telegram.
*
* @param         w_recSaNo           SaNo of the received telegram,
*     valid range: any 16 bit value
*
* @param         w_lastSentSaNo      SaNo of last sent SSDO telegram,
*     valid range: any 16 bit value
*
* @param        w_fsmNum            protocol FSM number (not checked, checked in
*     SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()),
*     valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @return
* - TRUE              - SaNo is valid
* - FALSE             - SaNo is invalid
*/
BOOLEAN SSDOC_CheckSaNo(UINT16 const w_recSaNo, UINT16 const w_lastSentSaNo, UINT16 const w_fsmNum)
{
      BOOLEAN o_return = TRUE;  /* predefine return value */
      t_PROT_FSM *ps_protFsm = &as_ProtFsm[w_fsmNum]; /* pointer to the actual
                                                         protocol FSM */
      INT16 i_saNoDiff;

      /* download is active */
      if (ps_protFsm->s_prel.o_dldActive)
      {
          i_saNoDiff = (INT16)(w_recSaNo - ps_protFsm->s_prel.w_recSaNo);
          /* error handling
           * an erroneous telegram may get repeated several times*/
          if (ps_protFsm->s_prel.o_errActive &&
              (w_recSaNo == ps_protFsm->s_prel.w_errSaNo))
          {
              o_return = TRUE;
              ps_protFsm->s_prel.o_errRep = TRUE;
              ps_protFsm->s_prel.b_actQueueSize++;
          }
          /* if pre-load is active the incoming SaNo must be higher than the last received,
           * but lower or equal the last sent */
          else if ((0 > i_saNoDiff) ||
                   (ps_protFsm->s_prel.w_actSaNo < w_recSaNo))
          {
              o_return = FALSE;
          }
          else
          {
              ps_protFsm->s_prel.o_errRep = FALSE;
              ps_protFsm->s_prel.b_actQueueSize = ps_protFsm->s_prel.b_recQueueSize - (ps_protFsm->s_prel.w_actSaNo - w_recSaNo);
              ps_protFsm->s_prel.w_recSaNo = w_recSaNo;
          }
      }
      /* if pre-load is not active the incoming SaNo must match the last sent */
      else
      {
          if (w_recSaNo != w_lastSentSaNo)
          {
              o_return = FALSE;
          }
      }


      SCFM_TACK_PATH();
      return o_return;
}

/**
* @brief This function sends the requests for "SSDO Segmented Preload Download".
*
* @param         dw_ct               consecutive time (not checked, any value allowed),
*     valid range: any 32 bit value
*
* @param        w_fsmNum            protocol FSM number (not checked, checked in
*     SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()),
*     valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @return
* - TRUE              - request sent successfully
* - FALSE             - no request sent
*/
static BOOLEAN sendPreDldSeg(UINT32 const dw_ct, UINT16 const w_fsmNum)
{
  BOOLEAN o_return = FALSE;  /* predefine return value */
  t_PROT_FSM *ps_protFsm = &as_ProtFsm[w_fsmNum]; /* pointer to the actual
                                                     protocol FSM */
  UINT8 b_saCmd;  /* SOD Access Command to be transmitted */
  t_PROT_FSM_STATE e_nextState;
  UINT8 b_rawDataLen = ps_protFsm->s_data.b_payloadLen - k_DATA_HDR_LEN_SEG;
  UINT32 dw_offset = 0;
  UINT32 dw_remData = 0;

  if ( ps_protFsm->s_prel.o_dldActive &&
       (0 < ps_protFsm->s_prel.b_actQueueSize))
  {
      /* calculate offset */
      ps_protFsm->s_prel.w_actSaNo = SSDOC_GetReqBuffer(w_fsmNum)->s_hdr.w_ct;
      dw_offset = b_rawDataLen*(ps_protFsm->s_prel.w_actSaNo - ps_protFsm->s_prel.w_startSaNo);
      /* calculate remaining data size */
      dw_remData = ps_protFsm->s_segm.dw_restData - dw_offset;

      if ((b_rawDataLen < dw_remData) ||
          (ps_protFsm->s_prel.b_actQueueSize == ps_protFsm->s_prel.b_recQueueSize))
      {

          /* change internal toggle bit */
          ps_protFsm->s_segm.o_toggleBit = (BOOLEAN)(!(ps_protFsm->s_segm.o_toggleBit));
          /* set data ptr */
          ps_protFsm->s_data.pb_data = ps_protFsm->s_prel.pb_startData + dw_offset;
          /* the subsequent segment is an END segment */
          if (dw_remData <= b_rawDataLen)
          {
            ps_protFsm->s_data.b_payloadLen = (UINT8)(dw_remData + k_DATA_HDR_LEN_SEG);
            b_saCmd = k_DWNLD_SEG_END;
            e_nextState = k_ST_WF_DWNLD_RESP_END_SEG;
            ps_protFsm->s_prel.o_dldActive    = FALSE;
            SSDOC_ServiceFsmSetPreload(w_fsmNum,FALSE);
          }
          else  /* the subsequent segment is a MIDDLE segment */
          {
            b_saCmd = k_DWNLD_SEG_PRE_MID;
            e_nextState = k_ST_WF_DWNLD_RESP_MID_SEG;
          }

          if (ps_protFsm->s_segm.o_toggleBit)
          {
            /* set toggle bit in SOD Access Command */
            EPLS_BIT_SET_U8(b_saCmd, k_TOGGLE_SACMD);
          }

          /* if transmission of a MIDDLE or LAST segment succeeded */
          if(SSDOC_ServiceDwnldSeg(w_fsmNum, dw_ct, b_saCmd, &ps_protFsm->s_data))
          {
            ps_protFsm->s_prel.w_actSaNo++;
            /* decrease queue size */
            ps_protFsm->s_prel.b_actQueueSize--;
            /* set next state */
            ps_protFsm->e_actState = e_nextState;
            /* prepare return value */
            o_return = TRUE;
          }
      }
  }
  /* no else : error, is already reported, return with predefined */

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function processes an upload end segment response with  unknown object size.
*
* @param        w_fsmNum            protocol FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()),
* 	valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        pb_respData         ref to resp data (not checked, only called with
* 	reference to array in processStateReqProc()), valid range: <> NULL
*
* @param        b_rawDataLen        length of the raw data (not checked, any value
* 	allowed), valid range: UINT8
*
* @return
* - TRUE              - response timeout check succeeded
* - FALSE             - response timeout check failed
*/
static BOOLEAN processUnknownSizeEndSeg(UINT16 w_fsmNum,
                                        const UINT8 *pb_respData,
                                        UINT8 b_rawDataLen)
{
  BOOLEAN o_return = FALSE;  /* return value of sub functions */
  t_PROT_FSM *ps_protFsm = &as_ProtFsm[w_fsmNum]; /* pointer to the actual
                                                     protocol FSM */

  /* if response buffer is large enough to store the last data bytes */
  if (ps_protFsm->s_segm.dw_restData >= b_rawDataLen)
  {
    ps_protFsm->s_segm.dw_uploadedObjSize =
      ps_protFsm->s_segm.dw_uploadedObjSize + b_rawDataLen;

    /* store read/ received SOD object data via reference to data
      segment into provided memory block */
    SFS_NET_CPY_DOMSTR(ps_protFsm->s_data.pb_data,
                       &(pb_respData[k_OFS_PYLD_SEG]), b_rawDataLen);

    /* call API callback function to pass the read SOD object data
        to the application */
    ps_protFsm->pf_respClbk(ps_protFsm->w_reqNum,
                            ps_protFsm->s_segm.dw_uploadedObjSize,
                            (UINT32)SOD_ABT_NO_ERROR);

    /* init protocol FSM */
    protocolFsmInit(w_fsmNum);

    o_return = TRUE;
  }
  else /* not all data bytes was exact uploaded */
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_REST_DATA_LEN,
                  (UINT32)b_rawDataLen);

    /* call API callback function */
    ps_protFsm->pf_respClbk(ps_protFsm->w_reqNum, 0UL,
                            (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
  }

  SCFM_TACK_PATH();
  return o_return;
}

/** @} */
