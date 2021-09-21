/**
 * @addtogroup SSDOC
 * @{
 *
 * @file SSDOCaccess.c
 *
 * This file contains functionality to access the SOD by SSDO Service Requests.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SSDOCaccess.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SLV10</td></tr>
 * </table>
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#include "SCFMapi.h"
#include "SERRapi.h"
#include "SERR.h"
#include "SSDOC.h"
#include "SSDOCapi.h"
#include "SSDOCint.h"
#include "SSDOint.h"
#include "SSDOCerr.h"

static BOOLEAN TypeLenReadValid(EPLS_t_DATATYPE e_dataType,
                                const UINT32 *pdw_dataLen, UINT8 b_payloadLen);
static BOOLEAN TypeLenValid(EPLS_t_DATATYPE e_dataType, UINT32 *pdw_dataLen,
                            UINT8 b_payloadLen);


/**
* @brief This function initializes SSDOC Protocol Layer and SSDOC Service Layer.
*
* @return
* - TRUE  - initialization succeeded
* - FALSE - initialization failed
*/
BOOLEAN SSDOC_Init(void)
{
  SCFM_TACK_PATH();
  return SSDOC_ProtocolLayerInit();
}

/**
* @brief This function transmits a request for write access to a specified SSDO Server.
*
* In dependence of the data length the transmission is segmented or unsegmented.
* The number of segments is also determined by the max. number of payload data
* bytes to be transferred within one segment. The SSDO server is addressed by its
* logical address (SADR). The SSDO client is able to communicate with many servers
* thus this function needs to request a communication channel internally. The memory
* that holds the data to be written is provided by the calling function.
*
* @attention This function must not be called in SNMTS_k_ST_INITIALIZATION state.
* The memory of the data to be written to has to be reserved until the response of
* the SSDO server has received. That means until the callback function is called.
* Parallel request to one SN is not allowed. The function must not be called if
* SSDOC_CheckFsmAvailable() returns FALSE otherwise FATAL error is generated.
*
* @see          SSDOC_CheckFsmAvailable()
*
* @param        w_sadr                target address of SN to be accessed
*                                     (checked) valid range:
*                                     EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
* @param         w_reqNum 			  service request number to assign the
*                                     request in the callback
*                                     (not checked, any value allowed)
*                                     valid range: UINT16
* @param        pf_respClbk           reference to a function that will be
*                                     called at the end of a transfer
*                                     (checked) valid range: <> NULL
* @param        dw_ct 				  consecutive time (not checked, any value
*                                     allowed),
*                                     valid range: UINT32
* @param       ps_req 			      request info of data to be process
*                                     (checked) valid range: <> NULL,
*                                     see SSDOC_t_REQ
* @param        o_fastDld             fast download requested
*
* @return
* - TRUE                - transmission of request successful
* - FALSE               - transmission of request failed
*/
BOOLEAN SSDOC_SendWriteReq(UINT16 w_sadr, UINT16 w_reqNum,
                           SSDOC_t_RESP_CLBK pf_respClbk, UINT32 dw_ct,
                           SSDOC_t_REQ *ps_req, BOOLEAN const o_fastDld)
{
  BOOLEAN o_return = FALSE;            /* predefined return value */

  /* if  reference to transfer information is invalid */
  if(ps_req == NULL)
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_WR_REQ_PTR,
                  SERR_k_NO_ADD_INFO);
  }
  /* else if logical target address of SN to be accessed to is NOT valid */
  else if((w_sadr < EPLS_k_MIN_SADR) || (w_sadr > EPLS_k_MAX_SADR))
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_WR_REQ_SADR,
                  (UINT32)(w_sadr));
  }
  /* else if pointer to the data is invalid */
  else if(ps_req->pb_data == NULL)
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_WR_DATA_PTR,
                  SERR_k_NO_ADD_INFO);
  }
  /* else if maximum number of payload data bytes is invalid */
  else if((ps_req->b_payloadLen > SSDOC_k_MAX_SERV_DATA_LEN) ||
          (ps_req->b_payloadLen < SSDOC_k_MIN_SERV_DATA_LEN))
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_WR_REQ_PYLDLEN,
                 (UINT32)(ps_req->b_payloadLen));
  }
  /* else if the data type or data length is not valid */
  else if(!TypeLenValid(ps_req->e_dataType, &(ps_req->dw_dataLen),
                        ps_req->b_payloadLen))
  {
    /* error is already reported */
  }
  /* else if the ref to the callback function is invalid */
  else if(pf_respClbk == NULL)
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_WR_REQ_CLBK_PTR,
                  SERR_k_NO_ADD_INFO);
  }
  else  /* parameters are valid */
  {
    /* if no SSDO transfer is running with this SN */
    if (!SSDOC_TransferRunning(w_sadr))
    {
      /* SSDO request is sent */
      o_return = SSDOC_SendReq(w_sadr, w_reqNum, pf_respClbk, dw_ct, ps_req,
                              TRUE,o_fastDld);
    }
    /* no else : error was already reported */
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function transmits a request for read access to a specified SSDO Server.
*
* In dependence of the data length the transmission is segmented or unsegmented. The
* number of segments is also determined by the max. number of payload data bytes to be
* transferred within one segment. Also the safety node which acts as a server is
* specified. The safety node server is addressed by its domain unique address (SADR).
* The client is able to communicate with many servers thus this function needs to request
* a communication channel internally.
*
* @attention This function must not be called in SNMTS_k_ST_INITIALIZATION state. The
* memory of the data to be read has to be reserved until the response of the SSDO server
* has received. That means until the callback function is called. Parallel request to
* one SN is not allowed. The function must not be called if SSDOC_CheckFsmAvailable()
* returns FALSE otherwise FATAL error is generated.
*
* @see          SSDOC_CheckFsmAvailable()
*
* @param        w_sadr 				   target address of SN to be accessed (checked), valid range: EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @param        w_reqNum               service request number to assign the request in the callback(not checked, any value allowed), valid range: UINT16
*
* @param        pf_respClbk            reference to a function that will be called at the end of a transfer (checked), valid range: <> NULL
*
* @param        dw_ct				   consecutive time (not checked, any value allowed), valid range: UINT32
*
* @param        ps_req                 request info of data to be process(checked) valid range: <> NULL, see {SSDOC_t_REQ}
*
* @return
* - TRUE                 - transmission of request successful
* - FALSE                - transmission of request failed
*/
BOOLEAN SSDOC_SendReadReq(UINT16 w_sadr, UINT16 w_reqNum,
                          SSDOC_t_RESP_CLBK pf_respClbk, UINT32 dw_ct,
                          const SSDOC_t_REQ *ps_req)
{
  BOOLEAN o_return = FALSE;            /* predefined return value */

  /* if reference to request information is invalid */
  if(ps_req == NULL)
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RD_REQ_PTR,
                  SERR_k_NO_ADD_INFO);
  }
  /* else if logical target address of SN to be accessed to is NOT valid */
  else if((w_sadr < EPLS_k_MIN_SADR) || (w_sadr > EPLS_k_MAX_SADR))
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RD_REQ_SADR,
                  (UINT32)(w_sadr));
  }
  /* else if pointer to the response buffer is invalid */
  else if(ps_req->pb_data == NULL)
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RD_DATA_PTR,
                  SERR_k_NO_ADD_INFO);
  }
  /* else if maximum number of payload data bytes is invalid */
  else if((ps_req->b_payloadLen > SSDOC_k_MAX_SERV_DATA_LEN) ||
          (ps_req->b_payloadLen < SSDOC_k_MIN_SERV_DATA_LEN))
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RD_REQ_PYLDLEN,
                  (UINT32)(ps_req->b_payloadLen));
  }
  /* else if the data type or data length is not valid */
  else if(!TypeLenReadValid(ps_req->e_dataType, &(ps_req->dw_dataLen),
                            ps_req->b_payloadLen))
  {
    /* error is already reported */
  }
  /* else if the ref to the callback function is invalid */
  else if(pf_respClbk == NULL)
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RD_REQ_CLBK_PTR,
                  SERR_k_NO_ADD_INFO);
  }
  else  /* parameters are valid */
  {
    /* if no SSDO transfer is running with this SN */
    if (!SSDOC_TransferRunning(w_sadr))
    {
      /* SSDO request is sent */
      o_return = SSDOC_SendReq(w_sadr, w_reqNum, pf_respClbk, dw_ct, ps_req,
                              FALSE, FALSE);
    }
    /* no else : error was already reported */
  }
  SCFM_TACK_PATH();
  return o_return;
}


#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param		w_errorCode			error number
*
* @param         dw_addInfo			additional error information
*
* @retval           pac_str				empty buffer to build the error string.
*/
  void SSDOC_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str)
  {
    /* choose the matching error string */
    switch(w_errorCode)
    {
      case SSDOC_k_ERR_WR_REQ_PTR:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_WR_REQ_PTR: SSDOC_SendWriteReq():\n"
          "NULL pointer passed instead of reference to "
          "<request struct of transfer data>.\n",
          SSDOC_k_ERR_WR_REQ_PTR);
        break;
      }
      case SSDOC_k_ERR_WR_REQ_SADR:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_WR_REQ_SADR: SSDOC_SendWriteReq():\n"
          "Function call with invalid target address (SADR) "
          "of SSDO Service Request (%04lu).\n",
          SSDOC_k_ERR_WR_REQ_SADR, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_WR_DATA_PTR:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_WR_DATA_PTR: SSDOC_SendWriteReq():\n"
          "NULL pointer passed instead of reference to "
          "<data to be written into a SOD>.\n",
          SSDOC_k_ERR_WR_DATA_PTR);
        break;
      }
      case SSDOC_k_ERR_WR_REQ_PYLDLEN:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_WR_REQ_PYLDLEN: SSDOC_SendWriteReq():\n"
          "Function call with invalid maximum payload data length (%02lu).\n",
          SSDOC_k_ERR_WR_REQ_PYLDLEN, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_WR_REQ_CLBK_PTR:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_WR_REQ_CLBK_PTR: SSDOC_SendWriteReq():\n"
          "NULL pointer passed instead of reference to "
          "<Response callback function>.\n", SSDOC_k_ERR_WR_REQ_CLBK_PTR);
        break;
      }
      case SSDOC_k_ERR_RD_REQ_PTR:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_RD_REQ_PTR: SSDOC_SendReadReq():\n"
          "NULL pointer passed instead of reference to "
          "<request struct of transfer data>.\n", SSDOC_k_ERR_RD_REQ_PTR);
        break;
      }
      case SSDOC_k_ERR_RD_REQ_SADR:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RD_REQ_SADR: SSDOC_SendReadReq():\n"
          "Function call with invalid target address (SADR) "
          "of SSDO Service Request (%04lu).\n",
          SSDOC_k_ERR_RD_REQ_SADR, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RD_DATA_PTR:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_RD_DATA_PTR: SSDOC_SendReadReq():\n"
          "Reference to the response buffer is NULL.\n",
          SSDOC_k_ERR_RD_DATA_PTR);
        break;
      }
      case SSDOC_k_ERR_RD_REQ_PYLDLEN:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RD_REQ_PYLDLEN: SSDOC_SendReadReq():\n"
          "Function call with invalid maximum payload data length (%02lu).\n",
          SSDOC_k_ERR_RD_REQ_PYLDLEN, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RD_REQ_CLBK_PTR:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_RD_REQ_CLBK_PTR: SSDOC_SendReadReq():\n"
          "NULL pointer passed instead of reference to "
          "<Response callback function>.\n", SSDOC_k_ERR_RD_REQ_CLBK_PTR);
        break;
      }
      case SSDOC_k_ERR_RD_DATA_LEN:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RD_DATA_LEN: TypeLenReadValid():\n"
          "Length of the response buffer size (%04lu) is invalid.\n",
          SSDOC_k_ERR_RD_DATA_LEN, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_DATA_LEN:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_DATA_LEN: TypeLenValid():\n"
          "The b_payloadLen is smaller then 12 byte and the "
          "UINT64, INT64 and REAL64 data type must not be segmented.\n",
          SSDOC_k_ERR_DATA_LEN);
        break;
      }
      case SSDOC_k_ERR_DATA_TYPE:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_DATA_TYPE: TypeLenValid():\n"
          "Function call with invalid data type "
          "of SSDO Service Request (%02lu).\n",
          SSDOC_k_ERR_DATA_TYPE, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_NOT_ASSIGNED:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_NOT_ASSIGNED: SSDOC_ProcessResponse():\n"
          "The received SSDO Service Response with ID %02u "
          "could NOT be assigned.\n",
          SSDOC_k_ERR_RESP_NOT_ASSIGNED, (UINT16)dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_PROT_FSM_STATE:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_PROT_FSM_STATE: SSDOC_ProtocolFsmProcess():\n"
          "The protocol FSM was called in the k_ST_WF_REQ_TRANS state\n",
          SSDOC_k_ERR_PROT_FSM_STATE);
        break;
      }
      case SSDOC_k_ERR_RESP_DATA_LEN1:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_DATA_LEN1: SSDOC_ProtocolFsmProcess():\n"
          "The data length (%02lu) of the expedited init upload response "
          "frame is invalid. Frame is ignored.\n",
          SSDOC_k_ERR_RESP_DATA_LEN1, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RX_OBJ_SIZE:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RX_OBJ_SIZE: SSDOC_ProtocolFsmProcess():\n"
          "The object size (%02lu) in the init segmented upload response "
          "frame is invalid. Frame is ignored.\n",
          SSDOC_k_ERR_RX_OBJ_SIZE, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_DATA_LEN2:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_DATA_LEN2: SSDOC_ProtocolFsmProcess():\n"
          "The data length (%02lu) of the init segmented upload response "
          "frame is invalid. Frame is ignored.\n",
          SSDOC_k_ERR_RESP_DATA_LEN2, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_DATA_LEN3:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_DATA_LEN3: SSDOC_ProtocolFsmProcess():\n"
          "The data length (%02lu) of the middle segmented upload response "
          "frame is invalid. Frame is ignored.\n",
          SSDOC_k_ERR_RESP_DATA_LEN3, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_DATA_LEN11:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_DATA_LEN11: SSDOC_ProtocolFsmProcess():\n"
          "The data length (%02lu) of the end segmented upload response "
          "frame is invalid. Frame is ignored.\n",
          SSDOC_k_ERR_RESP_DATA_LEN11, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_SACMD_INV1:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_RESP_SACMD_INV1: SSDOC_ProtocolFsmProcess():\n"
          "Upload end segment response (k_UPLD_SEG_END) received in the state "
          "'wait for middle segment' (k_ST_WF_UPLD_RESP_MID_SEG).\n",
          SSDOC_k_ERR_RESP_SACMD_INV1);
        break;
      }
      case SSDOC_k_ERR_REST_DATA_LEN2:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_REST_DATA_LEN2: SSDOC_ProtocolFsmProcess():\n"
          "The rest raw data length (%02lu) in the end segmented upload "
          "response is wrong. Frame is ignored.\n",
          SSDOC_k_ERR_REST_DATA_LEN2, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_DATA_LEN4:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_DATA_LEN4: SSDOC_ProtocolFsmProcess():\n"
          "The data length (%02lu) of the end segmented upload response "
          "frame is invalid. Frame is ignored.\n",
          SSDOC_k_ERR_RESP_DATA_LEN4, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_DATA_LEN5:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_DATA_LEN5: SSDOC_ProtocolFsmProcess():\n"
          "The data length (%02lu) of the expedited download response "
          "frame is invalid. Frame is ignored.\n",
          SSDOC_k_ERR_RESP_DATA_LEN5, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_DATA_LEN6:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_DATA_LEN6: SSDOC_ProtocolFsmProcess():\n"
          "The data length (%02lu) of the init segmented download response "
          "frame is invalid. Frame is ignored.\n",
          SSDOC_k_ERR_RESP_DATA_LEN6, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_DATA_LEN7:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_DATA_LEN7: SSDOC_ProtocolFsmProcess():\n"
          "The data length (%02lu) of the middle segmented download response "
          "frame is invalid. Frame is ignored.\n",
          SSDOC_k_ERR_RESP_DATA_LEN7, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_DATA_LEN8:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_DATA_LEN8: SSDOC_ProtocolFsmProcess():\n"
          "The data length (%02lu) of the end segmented download response "
          "frame is invalid. Frame is ignored.\n",
          SSDOC_k_ERR_RESP_DATA_LEN8, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_PROT_STATE_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_PROT_STATE_INV: SSDOC_ProtocolFsmProcess():\n"
          "Undefined Protocol FSM state (%02lu).\n",
          SSDOC_k_ERR_PROT_STATE_INV, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_NO_PROT_FSM:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_NO_PROT_FSM: SSDOC_SendReq():\n"
          "NO Protocol FSM is available, actually.\n",
          SSDOC_k_ERR_NO_PROT_FSM);
        break;
      }
      case SSDOC_k_ERR_RESP_TB_WRONG:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_RESP_TB_WRONG: checkResponse():\n"
          "SSDO Service Response with invalid toggle bit in "
          "SOD Access Command (SACmd) received.\n",
          SSDOC_k_ERR_RESP_TB_WRONG);
        break;
      }
      case SSDOC_k_ERR_RESP_IDX_WRONG:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_RESP_IDX_WRONG: checkResponse():\n"
          "SSDO Service Response with invalid object index received.\n",
          SSDOC_k_ERR_RESP_IDX_WRONG);
        break;
      }
      case SSDOC_k_ERR_RESP_SUBIDX_WRONG:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_RESP_SUBIDX_WRONG: checkResponse():\n"
          "SSDO Service Response with invalid object sub index "
          "received.\n", SSDOC_k_ERR_RESP_SUBIDX_WRONG);
        break;
      }
      case SSDOC_k_ERR_RESP_DATA_LEN9:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_DATA_LEN9: checkResponse():\n"
          "The data length (%02lu) of the response "
          "frame is invalid (< k_DATA_HDR_LEN). Frame is ignored.\n",
          SSDOC_k_ERR_RESP_DATA_LEN9, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_DATA_LEN10:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_RESP_DATA_LEN10: processAbortResp():\n"
          "The data length (%02lu) of the abort response "
          "frame is invalid. Frame is ignored.\n",
          SSDOC_k_ERR_RESP_DATA_LEN10, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_SACMD_INV:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_RESP_SACMD_INV: processAbortResp():\n"
          "SSDO Service Response with invalid SOD Access Command "
          "(SACmd) received.\n", SSDOC_k_ERR_RESP_SACMD_INV);
        break;
      }
      case SSDOC_k_ERR_REST_DATA_LEN:
      {
        SPRINTF2(pac_str,
          "%#x - SSDOC_k_ERR_REST_DATA_LEN: processUnknownSizeEndSeg():\n"
          "The rest raw data length (%02lu) in the end segmented upload "
          "response is wrong. Frame is ignored.\n",
          SSDOC_k_ERR_REST_DATA_LEN, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_SERV_FSM_EVT_INV1:
      {
        SPRINTF2(pac_str, "%#x - SSDOC_k_ERR_SERV_FSM_EVT_INV1: "
                         "SSDOC_ServiceFsmProcess():\n"
                         "Undefined FSM event (%02lu) in Service FSM call.\n",
                         SSDOC_k_ERR_SERV_FSM_EVT_INV1, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_SERV_FSM_EVT_INV2:
      {
        SPRINTF2(pac_str, "%#x - SSDOC_k_ERR_SERV_FSM_EVT_INV2: "
                         "SSDOC_ServiceFsmProcess():\n"
                         "Undefined FSM event (%02lu) in Service FSM call.\n",
                         SSDOC_k_ERR_SERV_FSM_EVT_INV2, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_TRANSFER_RUNNING:
      {
        SPRINTF2(pac_str, "%#x - SSDOC_k_ERR_TRANSFER_RUNNING: "
                         "SSDOC_TransferRunning():\n"
                         "SSDO transfer is already running with the SN "
                         "(source address = %02lu).\n",
                         SSDOC_k_ERR_TRANSFER_RUNNING, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_TR_FIELD_USED:
      {
        SPRINTF2(pac_str, "%#x - SSDOC_k_ERR_RESP_TR_FIELD_USED: "
                         "processResponse():\n"
                         "The value in TR field (%02lu) of the received SSDO "
                         "frame is NOT zero.\n",
                         SSDOC_k_ERR_RESP_TR_FIELD_USED, dw_addInfo);
        break;
      }
      case SSDOC_k_ERR_RESP_SANO_DIFF:
      {
        SPRINTF3(pac_str, "%#x - SSDOC_k_ERR_RESP_SANO_DIFF: "
                         "processResponse():\n"
                         "SSDO Service Response with invalid SOD Access "
                         "Request number (SANo) received "
                         "(exp : %02lu rec : %02lu).\n",
                         SSDOC_k_ERR_RESP_SANO_DIFF,
                         (UINT32)(UINT16)dw_addInfo,
                         (UINT32)HIGH16(dw_addInfo));
        break;
      }
      case SSDOC_k_ERR_REF_FREE_FRMS:
      {
        SPRINTF1(pac_str,
          "%#x - SSDOC_k_ERR_REF_FREE_FRMS: SSDOC_BuildRequest():\n"
          "Invalid reference to the number of free frames.\n",
          SSDOC_k_ERR_REF_FREE_FRMS);
        break;
      }
      default:
      {
        SPRINTF(pac_str, "Unknown error code was generated by the SSDOC\n");
      }
    }
    SCFM_TACK_PATH();
    return;
   /* RSM_IGNORE_QUALITY_BEGIN Notice #17   - Function comment content less than 10.0%
                               Notice #18   - Function eLOC > maximum 200 eLOC
                               Notice #28   - Cyclomatic complexity > 15 */
  }
  /* RSM_IGNORE_QUALITY_END */

#endif /* (EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE) */

/**
* @brief This function checks the data type and the data length.
*
* @param e_dataType    	        data type (not checked, checked in TypeLenValid()),
* 	valid range: see {EPLS_t_DATATYPE}
*
* @retval pdw_dataLen
*   - IN: pointer to the data length of a EPLS_k_VISIBLE_STRING,
* 	  EPLS_k_DOMAIN, EPLS_k_OCTET_STRING (pointer not checked, only called with reference
* 	  to variable in SSDOC_SendReadReq(), data checked), valid range: UINT32
* 	- OUT: pointer to the data length of a standard data type
*
* @param b_payloadLen           maximum number of payload data bytes to be transferred
* 	within a single frame (not checked, checked in SSDOC_SendReadReq()), valid range:
* 	SSDOC_k_MIN_SERV_DATA_LEN .. SSDOC_k_MAX_SERV_DATA_LEN
*
* @return
* - TRUE                 - success
* - FALSE                - failure
*/
static BOOLEAN TypeLenReadValid(EPLS_t_DATATYPE e_dataType,
                                const UINT32 *pdw_dataLen, UINT8 b_payloadLen)
{
  BOOLEAN o_return = FALSE;            /* predefined return value */
  UINT32 dw_tmpDatalen = *pdw_dataLen; /* temporary variable for the data
                                          length */

  /* if data type checking succeeded */
  if (TypeLenValid(e_dataType, &dw_tmpDatalen, b_payloadLen))
  {
    /* if maximum number of data bytes to be read is OK */
    if ((dw_tmpDatalen <= *pdw_dataLen) && (*pdw_dataLen > 0x00UL))
    {
      o_return = TRUE;
    }
    else /* length of the response buffer is invalid */
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_RD_DATA_LEN, *pdw_dataLen);
    }
  }
  /* no else : FATAL error is already reported */

  return o_return;
}

/**
* @brief This function checks the data type.
*
* If the data type is standard data type then it sets the data length otherwise it
* checks the data length.
*
* @param e_dataType		data type (checked), see EPLS_t_DATATYPE
*
* @retval pdw_dataLen
* - IN : pointer to the data length of a EPLS_k_VISIBLE_STRING, EPLS_k_DOMAIN,
*   EPLS_k_OCTET_STRING (pointer not checked, only called with reference to
*   variable in SSDOC_SendReadReq(), data checked), valid range: UINT32
* - OUT: pointer to the data length of a standard data type
*
* @param b_payloadLen	maximum number of payload data bytes to be transferred
* 	within a single frame (checked), valid range: SSDOC_k_MIN_SERV_DATA_LEN ..
* 	SSDOC_k_MAX_SERV_DATA_LEN
*
* @return
* - TRUE                 - success
* - FALSE                - failure
*/
static BOOLEAN TypeLenValid(EPLS_t_DATATYPE e_dataType, UINT32 *pdw_dataLen,
                            UINT8 b_payloadLen)
{
  BOOLEAN o_return = FALSE;            /* predefined return value */

  #if (EPLS_cfg_MAX_PYLD_LEN < 12)
    b_payloadLen = b_payloadLen; /* to avoid compiler warning */
  #endif

  switch (e_dataType)
  {
    case EPLS_k_BOOLEAN:
    case EPLS_k_UINT8:
    case EPLS_k_INT8:
    {
      *pdw_dataLen = 0x01UL;
      o_return = TRUE;
      break;
    }
    case EPLS_k_UINT16:
    case EPLS_k_INT16:
    {
      *pdw_dataLen = 0x02UL;
      o_return = TRUE;
      break;
    }
    case EPLS_k_UINT32:
    case EPLS_k_INT32:
    case EPLS_k_REAL32:
    {
      *pdw_dataLen = 0x04UL;
      o_return = TRUE;
      break;
    }
    #if (EPLS_cfg_MAX_PYLD_LEN >= 12)
      case EPLS_k_UINT64:
      case EPLS_k_INT64:
      case EPLS_k_REAL64:
      {
        *pdw_dataLen = 0x08UL;

        /* if the standard data type can be transferred within a single frame */
        if (b_payloadLen >= (k_DATA_HDR_LEN + *pdw_dataLen))
        {
          o_return = TRUE;
        }
        else /* standard data type can not be transferred within a single
                frame */
        {
          SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_DATA_LEN,
                        SERR_k_NO_ADD_INFO);
        }
        break;
      }
    #endif
    case EPLS_k_VISIBLE_STRING:
    case EPLS_k_OCTET_STRING:
    case EPLS_k_DOMAIN:
    {
      o_return = TRUE;
      break;
    }
    default:
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SSDOC_k_ERR_DATA_TYPE,
                    (UINT32)(e_dataType));
    }
  }

  SCFM_TACK_PATH();
  return o_return;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15
                              Notice #14 - 'case' conditions do not equal
                                           'break' */
}
  /* RSM_IGNORE_QUALITY_END */

/** @} */
