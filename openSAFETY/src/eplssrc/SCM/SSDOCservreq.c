/**
 * @addtogroup SSDOC
 * @{
 *
 * @file SSDOCservreq.c
 *
 * This file contains functionality to transmit a SSDO Service Request.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 *
 * <h2>History for SSDOCservreq.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SLV10</td></tr>
 *     <tr><td>11.09.2014</td><td>Hans Pill</td><td>added functionality for preload</td></tr>
 *     <tr><td>25.09.2014</td><td>Hans Pill</td><td>preload is not used for every upload</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#include "SCFMapi.h"
#include "SERR.h"
#include "SHNF.h"
#include "SFS.h"
#include "SODapi.h"
#include "SOD.h"
#include "SDN.h"
#include "SSDOCapi.h"
#include "SSDOint.h"
#include "SSDOCint.h"


/**
 * This symbol represent the maximum value of the access request counter used
 * in SSDO transfers.
*/
#define k_ACC_REQ_CNT_MAX             0xFFFFu

/**
 * This symbol represent the minimum value of the access request counter used
 * in SSDO transfers. Value zero is excluded.
*/
#define k_ACC_REQ_CNT_MIN             0x0001u

/** This symbol represents the SSDOC frame type Service Request. */
#define k_SERVICE_REQ                 0x38u

/** This symbol represents the SSDOC frame type Service Request Fast. */
#define k_SERVICE_REQ_FAST            0x3Au

/** SOD access to entry 0x1200, 0x02, SCM SADR */
static SOD_t_ACS_OBJECT_VIRT s_AccessScmSadr SAFE_NO_INIT_SEKTOR;


static BOOLEAN getScmSadrSdn(UINT16 *pw_scmSadr);
static void incCt(UINT16 *pw_ct);

/**
* @brief This function initiates the SSDOC Service Layer.
*
* This includes the SSDOC Service FSM, SSDOC Service Timer and the logical
* address (SADR) of the SCM from the SOD.
*
* @return
* - TRUE  - initialization succeeded
* - FALSE - initialization failed
*/
BOOLEAN SSDOC_ServiceLayerInit(void)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get handle to SOD entry of SCM SADR */
  s_AccessScmSadr.s_obj.w_index = EPLS_k_IDX_COMMON_COM_PARAM;
  s_AccessScmSadr.s_obj.b_subIndex = EPLS_k_SUBIDX_SADR_SCM;
  s_AccessScmSadr.dw_segOfs = 0;
  s_AccessScmSadr.dw_segSize = 0;

  /* if write access to the SOD entry NOT possible */
  if(NULL == SOD_AttrGetVirt(EPLS_k_SCM_INST_NUM_ &s_AccessScmSadr, &s_errRes))
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* SOD entry main SADR is available */
  {
    /* Service Fsm and Timer are initialized */
    o_return = SSDOC_ServiceFsmInit();
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function initiates the SSDOC Service Request "SSDO Initiate Download"
* to start download transfer to a specific SN (SOD) inside the Safety Domain.
*
* @param        w_fsmNum           FSM number (not checked, checked in SSDOC_SendReq()
* 	and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        dw_ct              consecutive time (not checked, any value allowed),
* 	valid range: any 32 bit value
*
* @param        w_sadr             logical address of SN which shall respond its
* 	physical address (UDID)(not checked, checked in SSDOC_SendReadReq() or
* 	SSDOC_SendWriteReq()), valid range: EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @param        o_expedited
* 	- TRUE : expedited download service is sent
* 	- FALSE : segmented download service is sent (not checked, any value allowed)
*
* @param        ps_data            ref to all values that are stored into the frame
* 	data area (not checked, only called with reference to struct in SSDOC_SendReq()),
* 	valid range: <> NULL
*
* @param        o_fastDld          fast download requested
*
* @return
* - TRUE             - req transmission successfully
* - FALSE            - req transmission failed
*/
BOOLEAN SSDOC_ServiceDwnldInit(UINT16 w_fsmNum, UINT32 dw_ct, UINT16 w_sadr,
                               BOOLEAN o_expedited, const SSDOC_t_REQ *ps_data,
                               BOOLEAN const o_fastDld)
{
  BOOLEAN o_return = FALSE ;  /* predefined return value */
  UINT16 w_scmSadr;  /* main SADR in SOD, only valid after SOD read access */
  UINT8 b_rawDataLen; /* variable to calculate the raw data length */
  UINT8 *pb_rawData; /* pointer to the raw data */
  UINT8 b_saCmd; /* SOD access command byte */
  t_REQ_BUFFER *ps_reqBuf; /* pointer to the request buffer */

  /* if SADR of SCM or SDN or both are NOT available */
  if(!getScmSadrSdn(&w_scmSadr))
  {
    /* error: SADR of SCM or SDN or both are NOT available,
              error already reported */
  }
  else  /* SADR of SCM and SDN available */
  {
    ps_reqBuf = SSDOC_GetReqBuffer(w_fsmNum);

    /* store target address into ADR field  */
    ps_reqBuf->s_hdr.w_adr = w_sadr;
    /* store Safety Domain Number */
    ps_reqBuf->s_hdr.w_sdn = SDN_GetSdn(EPLS_k_SCM_INST_NUM);

    /* check which frame ID is to be sent */
    if ( o_fastDld)
    {
      /* telegram type is SSDO fast service req */
      ps_reqBuf->s_hdr.b_id = k_SERVICE_REQ_FAST;
    }
    else
    {
    /* telegram type is SSDO service req */
      ps_reqBuf->s_hdr.b_id = k_SERVICE_REQ;
    }

    /* increment SANo */
    incCt(&ps_reqBuf->s_hdr.w_ct);
    /* store source address into TADR field */
    ps_reqBuf->s_hdr.w_tadr = w_scmSadr;
    /* time req distinctive number not used */
    ps_reqBuf->s_hdr.b_tr = EPLS_k_TR_NOT_USED;
    /* payload data length */
    ps_reqBuf->s_hdr.b_le = ps_data->b_payloadLen;

    /* store index and sub index into the request buffer */
    SFS_NET_CPY16(&ps_reqBuf->ab_data[k_OFS_IDX], &(ps_data->w_idx));
    SFS_NET_CPY8(&ps_reqBuf->ab_data[k_OFS_SIDX], &(ps_data->b_subIdx));

    /* if expedited transfer is requested */
    if(o_expedited)
    {
      b_saCmd = k_DWNLD_INIT_EXP;

      /* store SOD access request command into the request buffer */
      SFS_NET_CPY8(&ps_reqBuf->ab_data[k_OFS_SACMD], &b_saCmd);

      /* raw data length is set
         (payload length = raw data length + header length) */
      b_rawDataLen = (UINT8)(ps_data->b_payloadLen - k_DATA_HDR_LEN);

      /* pointer to the raw data is set */
      pb_rawData = &ps_reqBuf->ab_data[k_OFS_PYLD_INIT_EXP];

      /* if payload data is copied into the request buffer successfully */
      if(SFS_NetworkCopyGen(pb_rawData, ps_data->pb_data, (UINT32)b_rawDataLen,
                            ps_data->e_dataType))
      {
        o_return = SSDOC_ServiceFsmProcess(dw_ct, w_fsmNum, k_EVT_REQ, 0U, 0U);
      }
      /* no else : error, payload data NOT copied, error already reported */
    }
    else /* segmented transfer requested */
    {
      /* preload */
      b_saCmd = k_DWNLD_SEG_PRE_INIT;

      /* store SOD access request command into the request buffer */
      SFS_NET_CPY8(&ps_reqBuf->ab_data[k_OFS_SACMD], &b_saCmd);

      /* store size of object to be transmitted via segmented transfer */
      SFS_NET_CPY32(&ps_reqBuf->ab_data[k_OFS_OBJ_LEN], &(ps_data->dw_dataLen));

      /* raw data length is set (payload length =
        raw data length + header length + length of the object size field) */
      b_rawDataLen = (UINT8)(ps_data->b_payloadLen -
                             (k_DATA_HDR_LEN + k_DATA_OBJ_LEN));

      /* pointer to the raw data is set */
      pb_rawData = &ps_reqBuf->ab_data[k_OFS_PYLD_INIT_SEG];

      /* if payload data is copied into the request buffer successfully */
      SFS_NET_CPY_DOMSTR(pb_rawData, ps_data->pb_data, b_rawDataLen);

      o_return = SSDOC_ServiceFsmProcess(dw_ct, w_fsmNum, k_EVT_REQ, 0U, 0U);
    }
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function initiates the SSDOC Service Request "SSDO Segment Download" to
* continue the download transfer.
*
* @param        w_fsmNum        FSM number (not checked, checked in SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        dw_ct           consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        b_saCmd 		SOD access command byte to be sent (not checked, only called with define in processDwnldRespSeg()), valid range : k_ST_WF_DWNLD_RESP_MID_SEG or k_ST_WF_DWNLD_RESP_END_SEG
*
* @param        ps_data         ref to all values that are stored into the frame data area (not checked, only called with reference to struct), valid range: <> NULL
*
* @return
* - TRUE          - req transmission successfully
* - FALSE         - req transmission failed
*/
BOOLEAN SSDOC_ServiceDwnldSeg(UINT16 w_fsmNum, UINT32 dw_ct, UINT8 b_saCmd,
                              const SSDOC_t_REQ *ps_data)
{
  BOOLEAN o_return = FALSE ;  /* predefined return value */
  UINT8 b_rawDataLen; /* variable to calculate the raw data length */
  UINT8 *pb_rawData; /* pointer to the raw data */
  t_REQ_BUFFER *ps_reqBuf; /* pointer to the request buffer */

  ps_reqBuf = SSDOC_GetReqBuffer(w_fsmNum);

  /* w_adr, w_sdn, b_id, w_tadr, b_tr were already set in
     SSDOC_ServiceUpldInit() */

  /* increment SANo */
  incCt(&ps_reqBuf->s_hdr.w_ct);
  /* payload data length */
  ps_reqBuf->s_hdr.b_le = ps_data->b_payloadLen;

  /* store SOD access request command into local data array */
  SFS_NET_CPY8(&ps_reqBuf->ab_data[k_OFS_SACMD], &b_saCmd);

  /* index and sub index were already set */

  /* raw data length is set
    (payload length = raw data length + header length) */
  b_rawDataLen = (UINT8)(ps_data->b_payloadLen - k_DATA_HDR_LEN_SEG);

  /* pointer to the raw data is set */
  pb_rawData = &ps_reqBuf->ab_data[k_OFS_PYLD_SEG];

  /* if payload data is copied into local data array successfully */
  SFS_NET_CPY_DOMSTR(pb_rawData, ps_data->pb_data, b_rawDataLen);

  o_return = SSDOC_ServiceFsmProcess(dw_ct, w_fsmNum, k_EVT_REQ, 0U, 0U);

  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function initiates the SSDOC Service Request "SSDO Initiate Upload" to start
* Upload transfer to a specific SN (SOD) inside the Safety Domain.
*
* @param        w_fsmNum        FSM number (not checked, checked in SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        dw_ct           consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_sadr          logical address of SN which shall resp. its physical address (UDID) (not checked, checked in SSDOC_SendReadReq() or SSDOC_SendWriteReq()), valid range: EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @param        ps_data         ref to all values that are stored into the frame data area (not checked, only called with reference to struct), valid range: <> NULL
*
* @return
* - TRUE          - req transmission successfully
* - FALSE         - req transmission failed
*/
BOOLEAN SSDOC_ServiceUpldInit(UINT16 w_fsmNum, UINT32 dw_ct, UINT16 w_sadr,
                              const SSDOC_t_REQ *ps_data)
{
  BOOLEAN o_return = FALSE ;  /* predefined return value */
  UINT16 w_scmSadr;  /* main SADR in SOD, only valid after SOD read access */
  t_REQ_BUFFER *ps_reqBuf; /* pointer to the request buffer */
  UINT8 b_saCmd = k_UPLD_INIT_EXP; /* SOD access command byte */

  /* if SADR of SCM or SDN or both are NOT available */
  if(!getScmSadrSdn(&w_scmSadr))
  {
    /* error: SADR of SCM or SDN or both are NOT available,
              error already reported */
  }
  else  /* SADR of SCM and SDN available */
  {
    ps_reqBuf = SSDOC_GetReqBuffer(w_fsmNum);

    /* collect header info and store it into module global data structure */
    /* store target address into ADR field  */
    ps_reqBuf->s_hdr.w_adr = w_sadr;
    /* store Safety Domain Number */
    ps_reqBuf->s_hdr.w_sdn = SDN_GetSdn(EPLS_k_SCM_INST_NUM);
    /* telegram type is SSDO service req */
    ps_reqBuf->s_hdr.b_id = k_SERVICE_REQ;
    /* store payload data length */
    ps_reqBuf->s_hdr.b_le = k_DATA_HDR_LEN;
    /* increment SANo */
    incCt(&ps_reqBuf->s_hdr.w_ct);
    /* store source address into TADR field */
    ps_reqBuf->s_hdr.w_tadr = w_scmSadr;
    /* time req destinctive number not used */
    ps_reqBuf->s_hdr.b_tr = EPLS_k_TR_NOT_USED;

    /* store SOD access command byte */
    SFS_NET_CPY8(&ps_reqBuf->ab_data[k_OFS_SACMD], &b_saCmd);

    /* store index and sub index into local data array */
    SFS_NET_CPY16(&ps_reqBuf->ab_data[k_OFS_IDX], &(ps_data->w_idx));
    SFS_NET_CPY8(&ps_reqBuf->ab_data[k_OFS_SIDX], &(ps_data->b_subIdx));

    o_return = SSDOC_ServiceFsmProcess(dw_ct, w_fsmNum, k_EVT_REQ, 0U, 0U);
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function initiates the SSDOC Service Request "SSDO Segment Upload" to
* continue the Upload transfer.
*
* @param        w_fsmNum        FSM number (not checked, checked in SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        dw_ct           consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        b_saCmd         SOD access command byte to be sent (not checked, only called with define in processUpldRespSeg()), valid range : k_ST_WF_UPLD_RESP_MID_SEG or k_ST_WF_UPLD_RESP_END_SEG
*
* @return
* - TRUE          - req transmission successfully
* - FALSE         - req transmission failed
*/
BOOLEAN SSDOC_ServiceUpldSeg(UINT16 w_fsmNum, UINT32 dw_ct, UINT8 b_saCmd)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  t_REQ_BUFFER *ps_reqBuf; /* pointer to the request buffer */

  ps_reqBuf = SSDOC_GetReqBuffer(w_fsmNum);

  /* w_adr, w_sdn, b_id, b_le, w_tadr, b_tr were already set in
     SSDOC_ServiceUpldInit() */

  /* increment SANo */
  incCt(&ps_reqBuf->s_hdr.w_ct);

  /* store SOD access command byte */
  SFS_NET_CPY8(&ps_reqBuf->ab_data[k_OFS_SACMD], &b_saCmd);

  /* index and sub index were already set */

  o_return = SSDOC_ServiceFsmProcess(dw_ct, w_fsmNum, k_EVT_REQ, 0U, 0U);

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function initiates the SSDOC Service Request "SSDO Abort" to inform a
* specific SN inside the Safety Domain about data transfer abortion.
*
* @param        w_fsmNum            number of FSM that processes a transfer to be aborted (not checked, checked in SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        dw_abortCode        abort code to be sent (not checked, only called with enum in SSDOC_BuildRequest() or SSDOC_ProtocolFsmProcess()), valid range: see SOD_t_ABORT_CODES
*
* @return
* - TRUE              - request transmission successfully
* - FALSE             - request transmission failed
*/
BOOLEAN SSDOC_ServiceAbort(UINT16 w_fsmNum, UINT32 dw_abortCode)
{
  BOOLEAN o_return = FALSE ;  /* predefined return value */
  t_REQ_BUFFER *ps_reqBuf; /* pointer to the request buffer */
  /* SOD Access Command to be transferred */
  UINT8 b_saCmd = k_ABORT;

  ps_reqBuf = SSDOC_GetReqBuffer(w_fsmNum);

  /* w_adr, w_sdn, b_id, w_tadr, b_tr, w_ct were already set in
     SSDOC_ServiceUpldInit() or SSDOC_ServiceDwnldInit()
     SSDOC_ServiceUpldSeg() or SSDOC_ServiceDwnldSeg() */

  /* store payload data length */
  ps_reqBuf->s_hdr.b_le = k_DATA_HDR_LEN + k_ABORT_LEN;

  /* store SOD access command byte */
  SFS_NET_CPY8(&ps_reqBuf->ab_data[k_OFS_SACMD], &b_saCmd);

  /* index and sub index were already set */

  /* abort code is copied into the request buffer */
  SFS_NET_CPY32(&ps_reqBuf->ab_data[k_OFS_ABORT_CODE], &dw_abortCode);

  o_return = SSDOC_ServiceFsmProcess(k_CT_NOT_USED, w_fsmNum, k_EVT_ABORT_SEND,
                                     0U, 0U);

  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function reads the logical address (SADR) of the SCM from the SOD.
*
* @param pw_scmSadr reference to the SOD entry which stores the SADR of the SCM
* 	(not checked, only called with reference to variable in SSDOC_ServiceUpldInit() or
* 	SSDOC_ServiceDwnldInit()), valid range: <> NULL
*
* @return
* - TRUE             - success
* - FALSE            - failure
*/
static BOOLEAN getScmSadrSdn(UINT16 *pw_scmSadr)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */

  /* ref to main SADR in SOD, only valid after SOD read access */
  UINT16 *pw_tmpScmSadr = (UINT16 *)NULL;
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */


  /* get SADR of SCM from SOD */
  pw_tmpScmSadr = (UINT16 *)SOD_ReadVirt(EPLS_k_SCM_INST_NUM_
                                     &s_AccessScmSadr,
                                     &s_errRes);

  /* if read access to SOD failed */
  if(pw_tmpScmSadr == NULL)
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* SADR of SCM is available */
  {
    *pw_scmSadr = *pw_tmpScmSadr;
    o_return = TRUE;
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function increments the access request counter.
*
* @param pw_ct reference to the access request counter (not checked, only called
* 	with reference to variable in SSDOC_ServiceDwnldSeg() or SSDOC_ServiceDwnldInit() or
* 	SSDOC_ServiceUpldSeg() or SSDOC_ServiceUpldInit()), valid range: <> NULL
*/
static void incCt(UINT16 *pw_ct)
{
  /* if access request counter has reached its maximum */
  if(*pw_ct == k_ACC_REQ_CNT_MAX)
  {
    /* set counter to 1, 0x0000 is not valid */
    *pw_ct = k_ACC_REQ_CNT_MIN;
  }
  else
  {
    /* increment SANo */
    (*pw_ct)++;
  }

  SCFM_TACK_PATH();
}

/** @} */
