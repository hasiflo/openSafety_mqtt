/**
 * @addtogroup SNMTS
 * @{
 *
 * @file SNMTSprocess.c
 *
 * The file contains functionality to process SNMT Service
*              Requests, assemble SNMT Service Responses and access
*              the UDID in the SOD.
*
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SNMTSprocess.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>added SNMT command for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "SHNF.h"
#include "SCFMapi.h"
#include "SERRapi.h"
#include "SERR.h"
#include "SFS.h"
#include "SODapi.h"
#include "SOD.h"
#include "SDN.h"
#if(EPLS_cfg_SCM == EPLS_k_ENABLE)
  #include "SCMapi.h"
#endif
#include "SNMTS.h"
#include "SNMT.h"
#include "SNMTSint.h"
#include "SNMTSerr.h"

#include "sacun.h"

/**
 * @var s_AccessUdid
 * Data structure to manage access to the SOD entry 0x1019, 0x00, which
 * contains the "Unique Device ID (UDID)" of this SN.
*/
static SOD_t_ACS_OBJECT_VIRT s_AccessUdid[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;


static BOOLEAN processRequest(BYTE_B_INSTNUM_ UINT32 dw_ct,
                              const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
                              BOOLEAN *po_busy);
STATIC BOOLEAN processRequestExt(BYTE_B_INSTNUM_ UINT32 dw_ct,
                                 const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy);
static BOOLEAN checkRequest(BYTE_B_INSTNUM_ const EPLS_t_FRM_HDR *ps_reqHdr);
static BOOLEAN filterServReq(BYTE_B_INSTNUM_ UINT8 b_rxId, UINT16 w_rxSadr,
                             UINT16 w_rxSdn);
static BOOLEAN checkRxUdid(BYTE_B_INSTNUM_ const UINT8 *pb_rxUdid,
                           UINT8 b_rxUdidLen);


/**
 * @brief This function initializes the access to the SOD entry 0x1019, 0x00, which contains the "Unique Device ID (UDID)" of this SN.
 *
 * @param       b_instNum                instance number (not checked, checked in SSC_InitAll()),
 *                      valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 *  - TRUE           - success
 *  - FALSE          - failure
 */
BOOLEAN SNMTS_InitParamUdid(BYTE_B_INSTNUM)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get SOD entry "UDID" */
  s_AccessUdid[B_INSTNUMidx].s_obj.w_index = EPLS_k_IDX_UNIQUE_DEVICE_ID;
  s_AccessUdid[B_INSTNUMidx].s_obj.b_subIndex = EPLS_k_SUBIDX_UNIQUE_DEVICE_ID;
  s_AccessUdid[B_INSTNUMidx].dw_segOfs = 0;
  s_AccessUdid[B_INSTNUMidx].dw_segSize = 0;

  /* if SOD entry is available */
  if(NULL == SOD_AttrGetVirt(B_INSTNUM_ &s_AccessUdid[B_INSTNUMidx], &s_errRes))
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* SOD entry "UDID" available */
  {
    o_return = TRUE;
  }
  SCFM_TACK_PATH();
  return o_return;
}


/**
 * @brief This function processes a single received openSAFETY frame that was identified by the control unit (SSC)
 * as a network management service request.
 *
 * "Processing" means that the function also collects all frame header
 * information for the response and stores them into a data structure which is provided by the stack. The payload
 * to be transmitted inside the response is also stored. For that reason a memory block is requested from the unit
 * SHNF. The memory block is of size of the whole openSAFETY frame, but only payload of sub frame ONE is written into.
 * This function is called by the control unit (SSC) which provides a reference to an empty data structure for frame
 * header information to be filled inside this function.
 *
 * @note For timing improvement a SNMT Service Request is
 * processed in several steps. For that reason this function has to be called several times to process the request
 * completely.
 *
 * @param  b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
 *            valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @param        ps_rxBuf           reference to received openSAFETY frame to be distributed (not checked, only called with
 *                  reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf           reference to openSAFETY frame to be transmitted (not checked, only called with reference to
 *                  struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @retval       po_busy            flag signals that SNMT Slave is currently processing a request (not checked,
 *                    only called with reference to variable in processStateReqProc()), valid range:
 *                     - TRUE:  Slave busy
 *                     - FALSE: Slave ready to process
 *
 * @return
 * - TRUE             - request successfully processed
 * - FALSE            - request processing failed (error was reported) or SNMTS SN Ack successfully
 *                       processed (no error was reported) Note : FALSE is returned in order not to generate a response.
 */
BOOLEAN SNMTS_ProcessRequest(BYTE_B_INSTNUM_ UINT32 dw_ct,
                             const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
                             BOOLEAN *po_busy)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */

  /* if first processing step */
  if(!*po_busy)
  {
    /* if frame check of received service request succeeded */
    if(checkRequest(B_INSTNUM_ &(ps_rxBuf->s_frmHdr)))
    {
      /* if processing of received service request succeeded */
      if(processRequest(B_INSTNUM_ dw_ct, ps_rxBuf, ps_txBuf, po_busy))
      {
        /* NOTE: not every request takes that long, so two processing
                 steps are not necessary each time. To signal a expedited
                 request the busy flag is reset ahead of time */

        o_return = TRUE;
      }
      else /* else : processing of received SNMT Service Request failed,
                     error already reported, reject service request */
      {
        *po_busy = FALSE;
      }
    }
    /* no else : received service request is invalid, error already reported,
                 reject service request */
  }
  else /* second processing step */
  {
    /* if processing of received service request succeeded */
    if(processRequest(B_INSTNUM_ dw_ct, ps_rxBuf, ps_txBuf, po_busy))
    {
      o_return = TRUE;
    }
    else /* else : processing of received SNMT Service Request failed,
                     error already reported, reject service request */
    {
      *po_busy = FALSE;
    }
  }
  SCFM_TACK_PATH();
  return o_return;
}

/***
*    static functions
***/

/**
* @brief This function determines the SNMT Service request to be processed.
*
* For that reason the id of the received frame is analyzed.
*
* @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame())
*       valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        ps_rxBuf           reference to received openSAFETY frame to be distributed (not checked,
*       only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
*
* @param        ps_txBuf           reference to openSAFETY frame to be transmitted (not checked, only called
*       with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
*
* @retval       po_busy            flag signals that SNMT Slave is currently processing a request (not
* checked, only called with reference to variable in processStateReqProc()), valid range:
*       - TRUE:  Slave busy
*       - FALSE: Slave ready to process
* @return
* - TRUE             - request successfully processed
* - FALSE            - request processing failed
*/
static BOOLEAN processRequest(BYTE_B_INSTNUM_ UINT32 dw_ct,
                              const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
                              BOOLEAN *po_busy)
{
  BOOLEAN o_return = FALSE;           /* predefiend return value */

  *po_busy = FALSE;

  /* check telegram type to distinguish the different SNMT services */
  switch(SFS_GET_MIN_FRM_ID(ps_rxBuf->s_frmHdr.b_id))
  {
    case SNMT_k_UDID_REQ:
    {
      /* if value of LE-field inside received header info is invalid */
      if(ps_rxBuf->s_frmHdr.b_le != EPLS_k_NO_DATA_LEN)
      {
        /* error: LE-field contains wrong number of payload data bytes,
                  return with predefined FALSE */
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LE_UDID_REQ_INV,
                      (UINT32)(ps_rxBuf->s_frmHdr.b_le));
      }
      else  /* received header info contains correct payload length */
      {
        /* UDID response transmission */
        SNMTS_TxRespUdid(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                         &s_AccessUdid[B_INSTNUMidx]);
        o_return = TRUE;
      }
      break;
    }
    case SNMT_k_ASS_SADR:
    {
      /* if received UDID is wrong */
      if(!(checkRxUdid(B_INSTNUM_
                        &(ps_rxBuf->ab_frmData[SNMT_k_OFS_SERV_CMD]),
                        ps_rxBuf->s_frmHdr.b_le)))
      {
        /* error: error during UDID check, error already reported,
                  reject SNMT Service Request */
      }
      else  /* else no errors during UDID check */
      {
        /* if event <k_EVT_RX_SADR> failed to process */
        o_return = SNMTS_SnStateFsm(B_INSTNUM_ k_CT_NOT_USED_INT,
                                    k_EVT_RX_SADR, ps_rxBuf, ps_txBuf,
                                    (UINT8 *)NULL, po_busy);
      }
      break;
    }
    case SNMT_k_EXT_SER_REQ:
    {
      /* if no data byte is received */
      if (ps_rxBuf->s_frmHdr.b_le < SNMT_k_LEN_SERV_CMD)
      {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_EXT_REQ_LEN,
                      SERR_k_NO_ADD_INFO);
      }
      else /* at least one data byte is received */
      {
        o_return = processRequestExt(B_INSTNUM_ dw_ct, ps_rxBuf, ps_txBuf,
                                    po_busy);
      }
      break;
    }
    default:
    {
      /* error: received frame ID is NOT valid, reject service request */
      SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_RX_FRMID_REQ_PROC_INV,
                    (UINT32)ps_rxBuf->s_frmHdr.b_id);
      break;
    }
  }

  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function determines the extended SNMT Service request to be processed.
*
* For that reason the first data byte of the received frame is analyzed.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame())
*       valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
* @param        dw_ct            consecutive time (not checked, any value allowed), valid range: any 32 bit value
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (not checked, only
*       called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
* @param        ps_txBuf         reference to openSAFETY frame to be transmitted (not checked, only called
*       with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
* @retval          po_busy          flag signals that SNMT Slave is currently processing a request (not
* checked, only called with reference to variable in processStateReqProc()), valid range:
* - TRUE:  Slave busy
* - FALSE: Slave ready to process
*
* @return
* - TRUE           - request successfully processed
* - FALSE          - request processing failed
*/
STATIC BOOLEAN processRequestExt(BYTE_B_INSTNUM_ UINT32 dw_ct,
                                 const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy)
{
  BOOLEAN o_return = FALSE;   /* predefined return value */
  UINT8 b_reqTypExt = 0x00u;  /* received extended service type (DB0) */
  #if(k_NUM_OF_ADD_TX_SPDO == k_NO_ADD_TX_SPDO)
    UINT16 w_zeroSadr = 0x0000U;
  #endif

  /* get the request command byte of the SNMTS frame from the
     payload data of sub frame ONE (data byte 0). */
  SFS_NET_CPY8(&b_reqTypExt, &(ps_rxBuf->ab_frmData[SNMT_k_OFS_SERV_CMD]));


  /* evaluate the received request command to distinguish the different
     SNMT Request Commands. */
  switch(b_reqTypExt)
  {
    case SNMT_k_SN_SET_TO_PREOP:
    {
      /* if value of LE-field inside received header info is wrong */
      if(ps_rxBuf->s_frmHdr.b_le != SNMT_k_LEN_SERV_CMD)
      {
        /* error: LE-field contains wrong number of payload data bytes*/
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LE_SET_TO_PREOP_INV,
                      (UINT32)(ps_rxBuf->s_frmHdr.b_le));
        /* return with predefined FALSE */
      }
      else  /* received header info contains correct payload length */
      {
        /* process event <k_EVT_RX_SET_TO_PREOP> */
        o_return = SNMTS_SnStateFsm(B_INSTNUM_ k_CT_NOT_USED_INT,
                                    k_EVT_RX_SET_TO_PREOP, ps_rxBuf, ps_txBuf,
                                    (UINT8 *)NULL, po_busy);
      }
      break;
    }
    case SNMT_k_SN_SET_TO_OP:
    {
      /* if value of LE-field inside received header info is wrong */
      if(ps_rxBuf->s_frmHdr.b_le != (SNMT_k_LEN_SERV_CMD +
                                     SNMT_k_LEN_TSTMP))
      {
        /* error: LE-field contains wrong number of payload data bytes,
                  return with predefiend FALSE */
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LE_SET_TO_OP_INV,
                      (UINT32)(ps_rxBuf->s_frmHdr.b_le));
      }
      else  /* value of LE-field inside received header info is OK */
      {
        o_return = SNMTS_SnStateFsm(B_INSTNUM_ dw_ct, k_EVT_RX_SET_TO_OP,
                                    ps_rxBuf, ps_txBuf, (UINT8 *)NULL, po_busy);
      }
      break;
    }
    #if(EPLS_cfg_SCM == EPLS_k_ENABLE)
      case SNMT_k_SCM_SET_TO_STOP:
      {
        /* if value of LE-field inside received header is wrong */
        if(ps_rxBuf->s_frmHdr.b_le != SNMT_k_LEN_SERV_CMD)
        {
          /* error: LE-field contains wrong number of payload data bytes,
                    return with predefined FALSE */
          SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LE_SCM_SET_TO_STOP_INV,
                        (UINT32)(ps_rxBuf->s_frmHdr.b_le));
        }
        else  /* received header info contains correct payload length */
        {
          /* initiate a SCM state transition from state OPERATIONAL to state
            STOPPED using unit SCM itself.
            NOTE: this request is NOT responded or acknowledged! */
          SCM_Deactivate();
          o_return = TRUE;
        }
        break;
      }
      case SNMT_k_SCM_SET_TO_OP:
      {
        /* if value of LE-field inside received header is wrong */
        if(ps_rxBuf->s_frmHdr.b_le != SNMT_k_LEN_SERV_CMD)
        {
          /* error: LE-field contains wrong number of payload bytes,
                    return with predefined FALSE */
          SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LE_SCM_SET_TO_OP_INV,
                        (UINT32)(ps_rxBuf->s_frmHdr.b_le));
        }
        else  /* received header contains correct payload length */
        {
          /* initiate SCM state transition from state STOPPED to state
             OPERATIONAL. NOTE: this request is NOT responded */
          o_return = SCM_Activate();
        }
        break;
      }

    #endif
    case SNMT_k_SCM_GUARD_SN:
    {
      /* if value of LE-field inside received header info is wrong */
      if(ps_rxBuf->s_frmHdr.b_le != SNMT_k_LEN_SERV_CMD)
      {
        /* error: LE-field contains wrong number of payload data bytes,
                  return with predefined FALSE */
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LE_GUARD_INV,
                      (UINT32)(ps_rxBuf->s_frmHdr.b_le));
      }
      else  /* SNMT Service Request valid */
      {
        o_return = SNMTS_SnStateFsm(B_INSTNUM_ dw_ct, k_EVT_RX_GUARDING,
                                    ps_rxBuf, ps_txBuf, (UINT8 *)NULL, po_busy);
      }
      break;
    }
    case SNMT_k_ASSGN_ADD_SADR:
    {
      /* if value of LE-field inside received header info is wrong */
      if(ps_rxBuf->s_frmHdr.b_le !=
         (SNMT_k_LEN_SERV_CMD + SNMT_k_LEN_SADR + SNMT_k_LEN_TXSPDO))
      {
        /* error: LE-field contains wrong number of payload data bytes
                  return with predefined FALSE */
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LE_ADD_SADR_ASS_INV,
                      (UINT32)(ps_rxBuf->s_frmHdr.b_le));
      }
      else /* SNMT Service Request valid */
      {
        #if(k_NUM_OF_ADD_TX_SPDO == k_NO_ADD_TX_SPDO)
          /* SNMT Slave supports only one TxSPDO (assigned to mainSadr),
              respond with assigned SADR = 0x0000 and received TxSPDO number */
          SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_ADD_SADR_NOT_SUPPO,
                        SERR_k_NO_ADD_INFO);

          /* The additional SADR set to 0 in the rx buffer to indicate that
             the additional SADR can not be assigned. (SNMTS_TxRespExtService()
             copied the Tx SPDO number and the additional SADR from the rx to
             the tx buffer to generate the response. ) */
          SFS_NET_CPY16(&ps_rxBuf->ab_frmData[SNMT_k_OFS_SADR], &w_zeroSadr);

          /* collect header info and transmit response */
          SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                 SNMTS_k_ADD_SADR_ASSGN, 0U ,0U, (UINT8 *)NULL);

          o_return = TRUE;
        #else
          o_return = SNMTS_SnStateFsm(B_INSTNUM_ k_CT_NOT_USED_INT,
                                      k_EVT_RX_SADR_ADD, ps_rxBuf, ps_txBuf,
                                      (UINT8 *)NULL, po_busy);
        #endif
      }
      break;
    }
    case SNMT_k_SN_ACK:
    {
      /* if value of LE-field inside received header info is wrong */
      if(ps_rxBuf->s_frmHdr.b_le != (SNMT_k_LEN_SERV_CMD + SNMT_k_LEN_ERR_CODE +
                                     SNMT_k_LEN_ERR_GROUP))
      {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LE_ERR_ACK_INV,
                      (UINT32)(ps_rxBuf->s_frmHdr.b_le));
      }
      else /* SNMT Service Request valid */
      {
        /* process extended service "error acknowledge" */
        o_return = SNMTS_SnStateFsm(B_INSTNUM_ k_CT_NOT_USED_INT, k_EVT_SN_ACK,
                                    ps_rxBuf, ps_txBuf, (UINT8 *)NULL, po_busy);
      }
      break;
    }
    case SNMT_k_ASSGN_UDID_SCM: /* UDID of SCM assignment */
    {
      /* if value of LE-field inside received header info is right */
      if (ps_rxBuf->s_frmHdr.b_le == (SNMT_k_LEN_SERV_CMD + EPLS_k_UDID_LEN))
      {
        /* process extended service */
        o_return = SNMTS_SnStateFsm(B_INSTNUM_ k_CT_NOT_USED_INT,
                                    k_EVT_RX_UDID_SCM, ps_rxBuf, ps_txBuf,
                                    (UINT8 *)NULL, po_busy);
      }
      else /* value of LE-field inside received header info is wrong */
      {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LE_UDID_SCM,
                      (UINT32)(ps_rxBuf->s_frmHdr.b_le));
      }
      break;
    }
#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
    case SNMT_k_INITIALIZE_EXT_CT:
    {
      /* if value of LE-field inside received header info is right */
      if (ps_rxBuf->s_frmHdr.b_le == (SNMT_k_LEN_SERV_CMD + EPLS_k_LEN_EXT_CT))
      {
        /* process extended service */
        o_return = SNMTS_SnStateFsm(B_INSTNUM_ k_CT_NOT_USED_INT,
                                    k_EVT_RX_INIT_CT, ps_rxBuf, ps_txBuf,
                                    (UINT8 *)NULL, po_busy);
      }
      else /* value of LE-field inside received header info is wrong */
      {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LE_EXT_CT,
                      (UINT32)(ps_rxBuf->s_frmHdr.b_le));
      }
      break;
    }
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */
    default:
    {
      /* error: received frame type of an extended service request is
                not defined and thus invalid, reject service request */
      SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_REQ_EXT_FRMTYP_INV1,
                    (UINT32)(b_reqTypExt));
      break;
    }
  }

  SCFM_TACK_PATH();
  return o_return;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */



/**
* @brief This function checks the requests.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        ps_reqHdr        reference to the header information (not checked, only called with
*       reference to struct in SNMTS_ProcessRequest()), valid range: <> NULL
*
* @return
* - TRUE           - frame check succeeded
* - FALSE          - error during frame check
*/
static BOOLEAN checkRequest(BYTE_B_INSTNUM_ const EPLS_t_FRM_HDR *ps_reqHdr)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */

  /* if consecutive time (CT-field) is NOT marked as unused (zero) */
  if(ps_reqHdr->w_ct != EPLS_k_CT_NOT_USED)
  {
    /* error: CT-field is not set to zero because it is NOT used */
    /* return with predefiend FALSE */
    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_CT_FIELD_NOT_UNUSED,
                  (UINT32)(ps_reqHdr->w_ct));
  }
  /* else if the TR-field is NOT marked as unused (zero)*/
  else if(ps_reqHdr->b_tr != EPLS_k_TR_NOT_USED)
  {
    /* error: TR-field is not set to zero because it is NOT used */
    /* return with predefiend FALSE */
    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_TR_FIELD_NOT_UNUSED,
                  (UINT32)(ps_reqHdr->b_tr));
  }
  /* else if received SNMTS Service Request is to reject */
  else if(!(filterServReq(B_INSTNUM_ ps_reqHdr->b_id, ps_reqHdr->w_adr,
                          ps_reqHdr->w_sdn)))
  {
    /* received SNMT Service Request is NOT addressed to this SN,
       return predefined FALSE to reject frame */
  }
  else  /* received SNMTS Service Request is to process */
  {
    o_return = TRUE;
  }
  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function checks whether a received SNMTS Service Request is to reject.
*
* For that reason the destination address of the received request is compared with the main
* SADR of the receiving SN. SDN in the received request is compared with the SDN number in the
* SOD.
*
* @attention The following SNMTS Service Request passes the filter without rejection:
* - UDID Request
* - SADR assignment
*
* @param        b_instNum           instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        b_rxId              received frame identification to handle the exception, (checked in
*       case of the exception), valid range : SNMT_k_UDID_REQ, SNMT_k_ASS_SADR
*
* @param        w_rxSadr            received SADR, (checked), valid range : EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        w_rxSdn             received SDN, (checked), valid range : k_MIN_SDN .. k_MAX_SDN
*
* @return
* - TRUE               - received SNMTS Service to be processed
* - FALSE              - received SNMTS Service to be rejected
*/
static BOOLEAN filterServReq(BYTE_B_INSTNUM_ UINT8 b_rxId, UINT16 w_rxSadr,
                             UINT16 w_rxSdn)
{
  BOOLEAN o_return = FALSE;    /* predefined return value */
  UINT16 w_ownMainSadr = 0x0000U; /* own main Sadr in SOD */
  UINT16 w_ownSdn      = 0x0000U; /* own SDN in SOD */

  /* get the own main SADR from SOD */
  w_ownMainSadr = SNMTS_MainSadrGet(B_INSTNUM);
  /* get the own SDN from SOD */
  w_ownSdn = SDN_GetSdn(B_INSTNUM);

  /* if main SADR or SDN is not assigned, yet or
      the received request is not addressed to this SN */
  if((w_ownMainSadr == 0x0000U) || (w_ownMainSadr != w_rxSadr) ||
     (w_ownSdn == 0x0000U)      || (w_ownSdn != w_rxSdn))
  {
    /* if Service Request is of type "SADR assignment" or "Udid Request" */
    if((SFS_GET_MIN_FRM_ID(b_rxId) == SNMT_k_UDID_REQ) ||
       (SFS_GET_MIN_FRM_ID(b_rxId) == SNMT_k_ASS_SADR))
    {
      /* EXCEPTION case: reject every SNMT Service except "SADR assignment"
                          and "Udid Request" */
      o_return = TRUE;  /* prepare return value */
    }
    else  /* telegram type is NOT UDID request OR SADR assignment */
    {
      /* the received SNMTS Service Request is not addressed to this SN,
          report error and reject service */
      SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_REQ_REJ_INV_DSADR,
                    (((UINT32)(w_rxSadr))<<16)|(UINT32)w_rxSdn);
    }
  }
  else  /* the received SNMTS Service Request is addressed to this SN */
  {
    /* STANDARD case: process received SNMTS Service Request */
    o_return = TRUE;     /* prepare return value */
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function checks the received UDID, transmitted in SNMTS Service Request SADR assignment.
*
* It compares the received UDID with the SNs UDID.
*
* @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range:
*
* @param        pb_rxUdid          received UDID (pointer not checked, only called with reference to
*       array in processRequest(), data checked), valid range: <> NULL
*
* @param        b_rxUdidLen        number of bytes of UDID (checked), valid range: 8 bytes UDID length
*
* @return
* - TRUE             - UDID check succeeded
* - FALSE            - UDID check failed
*/
static BOOLEAN checkRxUdid(BYTE_B_INSTNUM_ const UINT8 *pb_rxUdid,
                           UINT8 b_rxUdidLen)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  UINT8 *pb_udid = (UINT8 *)NULL; /* ref to the 8 byte UDID of the SN */
  UINT8 ab_udidBuf[EPLS_k_UDID_LEN];
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get Udid from SOD */
  pb_udid = (UINT8 *)SOD_ReadVirt(B_INSTNUM_ &s_AccessUdid[B_INSTNUMidx], &s_errRes);

  /* if read access failed */
  if(pb_udid == NULL)
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* SOD access to UDID succeeded */
  {
    /* if received number of bytes of UDID is wrong */
    if(b_rxUdidLen != EPLS_k_UDID_LEN)
    {
      /* error: LE field contains wrong number of payload data bytes,
                return with predefiend FALSE */
      SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LE_SADR_ASS_INV,
                    (UINT32)(b_rxUdidLen));
    }
    else  /* received number of bytes of UDID is correct */
    {
      /* convert the UDID into host format */
      SFS_NET_CPY_DOMSTR(ab_udidBuf, pb_rxUdid, EPLS_k_UDID_LEN);

      /* if received UDID is NOT matching to UDID of SN */
      if(MEMCOMP(pb_udid, ab_udidBuf, EPLS_k_UDID_LEN) != MEMCMP_IDENT)
      {
        /* this SNMT Service Request was not received correctly */
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_RX_UDID_UNEVEN,
                      SERR_k_NO_ADD_INFO);
      }
      else  /* received UDID is matching to UDID of SN */
      {
        o_return = TRUE;    /* prepare return value */
      }
    }
  }
  SCFM_TACK_PATH();
  return o_return;
}

/** @} */
