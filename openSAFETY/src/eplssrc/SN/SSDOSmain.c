/**
 * @addtogroup SSDOS
 * @{
 *
 *
 * @file SSDOSmain.c
 * This file contains functionality to process the SSDOS Transfer
*              FSM.
 *
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SSDOSmain.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>25.04.2013</td><td>Roland Knall</td><td>Ignore return value for FsmReset where it will be immediatly overwritten</td></tr>
 *     <tr><td>11.09.2014</td><td>Hans Pill</td><td>changes for preload</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
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
#include "SDN.h"
#include "SSDOS.h"
#include "SSDOSerr.h"
#include "SSDOint.h"
#include "SSDOSint.h"




/**
 * This symbol is used to access the segmentation bit inside the SOD access command (SaCmd).
*/
#define k_SEG_SACMD    (UINT8)0x08

/**
 * Enumeration of states of the SSDOS FSM.
*/
typedef enum
{
  /** download or upload transfer, request init state */
  k_WF_REQ_INIT,
  /** download transfer, response initialization */
  k_DWNLD_RESP_INIT,
  /** download transfer, wait for segment request */
  k_DWNLD_WF_REQ_SEG,
  /** upload transfer, response init state */
  k_UPLD_RESP_INIT,
  /** upload transfer, wait for segment request */
  k_UPLD_WF_REQ_SEG,
  /** transfer abortion state */
  k_RESP_ABORT
} t_SSDOS_FSM_STATES;

/**
 * Structured data type to process a received SSDO Service Request.
*/
typedef struct
{
    /** current state of FSM */
    t_SSDOS_FSM_STATES  e_actState;

  /** received SOD Access Command */
  UINT8 b_saCmd;
  /** abort code to be transferred */
  UINT32 dw_abortCode;
  /** pointer to the SN main SADR */
  UINT16 *pw_mainSadr;
} t_SSDOS_FSM;

/**
 * Module global FSM to process an expedited/segmented and up-/download SSDO Transfer.
*/
static t_SSDOS_FSM s_Fsm[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * This symbol represents the SSDOC frame type Service Request Fast.
*/
#define k_SERVICE_REQ_FAST            0x3Au

static void FsmInit(BYTE_B_INSTNUM);
static BOOLEAN FsmResetAbortSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf, UINT32 dw_abortCode);
static BOOLEAN FsmReset(BYTE_B_INSTNUM_ BOOLEAN o_abort);


static BOOLEAN WfReqInitState(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                              BOOLEAN *po_busy);
static BOOLEAN DwnldRespInitState(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                  EPLS_t_FRM *ps_txBuf);
static BOOLEAN DwnldWfReqSegState(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                  EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy);
static BOOLEAN UpldRespInitState(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf);
static BOOLEAN UpldWfReqSegState(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy);


/**
* @brief This function initializes the SSDOS FSM.
*
* This function initializes the SSDOS FSM, the segment information for the segmented upload and download variables of SOD access and sets a pointer to the Main SADR.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
*
* @return
* - TRUE           - initialization succeeded
* - FALSE          - initialization failed
*/
BOOLEAN SSDOS_Init(BYTE_B_INSTNUM)
{
  BOOLEAN o_return = FALSE;    /* return value */
  t_SSDOS_FSM *ps_fsm = &s_Fsm[B_INSTNUMidx]; /* pointer to the fsm structure */
  SOD_t_ACS_OBJECT_VIRT s_acsMainSadr;  /* handle to refer to main SADR */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get SOD entry "SN main SADR" */
  s_acsMainSadr.s_obj.w_index = EPLS_k_IDX_TXSPDO_COM_PARAM;
  s_acsMainSadr.s_obj.b_subIndex = EPLS_k_SUBIDX_TXSPDO_SADR;
  s_acsMainSadr.dw_segOfs = 0;
  s_acsMainSadr.dw_segSize = 0;

  /* if attribute is available */
  if(NULL != SOD_AttrGetVirt(B_INSTNUM_ &s_acsMainSadr, &s_errRes))
  {
    /* get main SADR of SN from SOD */
    ps_fsm->pw_mainSadr = (UINT16 *)SOD_ReadVirt(B_INSTNUM_ &s_acsMainSadr, &s_errRes);

    /* if read access succeeded */
    if(ps_fsm->pw_mainSadr != NULL)
    {
      FsmInit(B_INSTNUM);
      SSDOS_QueueSizeInit(B_INSTNUM);

      o_return = TRUE;  /* prepare return value */
    }
    else /* read access failed */
    {
      SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
    }
  }
  else /* attribute is not available */
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function processes SSDO Service Requests and responses them.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @retval        ps_txBuf         reference to openSAFETY frame to be transmitted (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
* @retval          po_busy          flag signals that SSDO Server is currently processing a service request:
* - TRUE:  Server busy
* - FALSE: Server ready to process (pointer not checked, only called with reference to variable in processStateReqProc()), valid range: <> NULL
*
* @return
* - TRUE           - SSDO Service Request successfully processed
* - FALSE          - SSDO Service Request processing failed (error was reported) or SSDOC Abort Frame successfully processed (no error was reported)
*
* @note FALSE is returned in order not to generate a response.
*/
BOOLEAN SSDOS_ProcessRequest(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                             EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  t_SSDOS_FSM *ps_fsm = &s_Fsm[B_INSTNUMidx]; /* pointer to the fsm structure */

  /* if the TR field in the openSAFETY frame is valid */
  if (ps_rxBuf->s_frmHdr.b_tr == EPLS_k_TR_NOT_USED)
  {
    /* if the main SADR matches to the address of the SSDOC to be responded and
      the received SDN is equal to the own SDN */
    if((ps_rxBuf->s_frmHdr.w_sdn == SDN_GetSdn(B_INSTNUM)) &&
      (ps_rxBuf->s_frmHdr.w_adr == (*ps_fsm->pw_mainSadr)))
    {
      /* process SSDOS FSM */
      switch(ps_fsm->e_actState)
      {
        case k_WF_REQ_INIT:
        {
          o_return = WfReqInitState(B_INSTNUM_ ps_rxBuf, po_busy);
          break;
        }
        case k_DWNLD_RESP_INIT:
        {

          *po_busy = FALSE;
          o_return = DwnldRespInitState(B_INSTNUM_ ps_rxBuf, ps_txBuf);
          break;
        }
        case k_DWNLD_WF_REQ_SEG:
        {

          o_return = DwnldWfReqSegState(B_INSTNUM_ ps_rxBuf, ps_txBuf, po_busy);
          break;
        }
        case k_UPLD_RESP_INIT:
        {
          *po_busy = FALSE;
          o_return = UpldRespInitState(B_INSTNUM_ ps_rxBuf, ps_txBuf);
          break;
        }
        case k_UPLD_WF_REQ_SEG:
        {
          o_return  = UpldWfReqSegState(B_INSTNUM_ ps_rxBuf, ps_txBuf, po_busy);
          break;
        }
        case k_RESP_ABORT:
        {
          *po_busy = FALSE;
          o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                      ps_fsm->dw_abortCode);
          break;
        }
        #pragma CTC SKIP
        default:
        {
          SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_FSM_STATE,
                        (UINT32)(ps_fsm->e_actState));
        }
        #pragma CTC ENDSKIP
      }
    }
    else /* SSDOC request is ignored */
    {
       SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_REQ_DSADR,
                    (((UINT32)(ps_rxBuf->s_frmHdr.w_adr))<<16) |
                    (UINT32)ps_rxBuf->s_frmHdr.w_sdn);
    }
  }
  else /* TR field is invalid */
  {
    SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_TR, (UINT32)(ps_rxBuf->s_frmHdr.b_tr));
  }

  /* if error happened */
  if (!o_return)
  {
    *po_busy = FALSE;
  }

  SCFM_TACK_PATH();
  return o_return;
}

#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param w_errorCode error number
* @param dw_addInfo additional error information
*
* @retval pac_str empty buffer to build the error string.
*/
  void SSDOS_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str)
  {

    /* choose the matching error string */
    switch(w_errorCode)
    {
      /** SSDOSmain.c **/
      case SSDOS_k_ERR_REQ_DSADR:
      {
        SPRINTF3(pac_str, "%#x - SSDOS_k_ERR_REQ_DSADR: "
                "SSDOS_ProcessRequest():\n"
                "The received SSDO Service was rejected by SSDOS filter - "
                "target address: %#x "
                "safety domain number: %#x\n",
                SSDOS_k_ERR_REQ_DSADR,
                HIGH16(dw_addInfo),(UINT16)dw_addInfo);

        break;
      }
      case SSDOS_k_ERR_TR:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_TR: SSDOS_ProcessRequest():\n"
                "The TR field (%#x) in the received SSDO Service is invalid.",
                SSDOS_k_ERR_TR, (UINT8)(dw_addInfo));

        break;
      }
      case SSDOS_k_ERR_FSM_STATE:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_FSM_STATE: "
                "SSDOS_ProcessRequest():\n"
                "Undefined FSM state (%02lu) in FSM call.\n",
                SSDOS_k_ERR_FSM_STATE, dw_addInfo);
        break;
      }
      case SSDOS_k_ERR_ABORT_RX:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_ABORT_RX: WfReqInitState():\n"
                "Abort frame received from the SSDOC "
                "with abort code : (%02lu).\n",
                SSDOS_k_ERR_ABORT_RX, dw_addInfo);
        break;
      }
      case SSDOS_k_ERR_UPLD_INIT_LEN:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_UPLD_INIT_LEN: WfReqInitState():\n"
                "The payload length (%02lu) in the initiate upload request "
                "frame received from the SSDOC is wrong.\n",
                SSDOS_k_ERR_UPLD_INIT_LEN, dw_addInfo);
        break;
      }
      case SSDOS_k_ERR_ABORT_LEN:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_ABORT_LEN: WfReqInitState():\n"
                "The payload length (%02lu) in the abort frame received "
                "from the SSDOC is wrong.\n",
                SSDOS_k_ERR_ABORT_LEN, dw_addInfo);
        break;
      }
      case SSDOS_k_ERR_SACMD:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_SACMD: WfReqInitState():\n"
                "Wrong SOD access command (%02lu) received from the SSDOC.\n",
                SSDOS_k_ERR_SACMD, dw_addInfo);
        break;
      }
      case SSDOS_k_ERR_PYLD_LEN:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_PYLD_LEN: WfReqInitState():\n"
                "The payload length (%02lu) in the SSDO frame received "
                "from the SSDOC is wrong.\n",
                SSDOS_k_ERR_PYLD_LEN, dw_addInfo);
        break;
      }
      case SSDOS_k_ERR_SACMD_INV:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_SACMD_INV: DwnldRespInitState():\n"
                "Invalid SOD access command (%02lu) was set by the EPLS SW.\n",
                SSDOS_k_ERR_SACMD_INV, dw_addInfo);
        break;
      }
      case SSDOS_k_ERR_PYLD_LEN1:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_PYLD_LEN1: DwnldWfReqSegState():\n"
                "The payload length (%02lu) in the SSDO frame received "
                "from the SSDOC is wrong.\n",
                SSDOS_k_ERR_PYLD_LEN1, dw_addInfo);
        break;
      }
      case SSDOS_k_ERR_SACMD_INV1:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_SACMD_INV1: UpldRespInitState():\n"
                "Invalid SOD access command (%02lu) was set by the EPLS SW.\n",
                SSDOS_k_ERR_SACMD_INV1, dw_addInfo);
        break;
      }
      case SSDOS_k_ERR_PYLD_LEN2:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_PYLD_LEN2: UpldWfReqSegState():\n"
                "The payload length (%02lu) in the SSDO frame received "
                "from the SSDOC is wrong.\n",
                SSDOS_k_ERR_PYLD_LEN2, dw_addInfo);
        break;
      }
      case SSDOS_k_ERR_PYLD_LEN3:
      {
        SPRINTF2(pac_str, "%#x - SSDOS_k_ERR_PYLD_LEN3: UpldWfReqSegState():\n"
                "The payload length (%02lu) in the SSDO frame received "
                "from the SSDOC is not equal to 4.\n",
                SSDOS_k_ERR_PYLD_LEN3, dw_addInfo);
        break;
      }


      /** SSDOSupDwnLd.c **/
      case SSDOS_k_ERR_DWNLD_EXP_LEN:
      {
        SPRINTF1(pac_str, "%#x - SSDOS_k_ERR_DWNLD_EXP_LEN: "
                "SSDOS_DwnldInitExpRespSend():\n"
                "The addressed SOD entry is not matching to the number "
                "of payload bytes of the requested expedited transfer\n.",
                SSDOS_k_ERR_DWNLD_EXP_LEN);
        break;
      }
      case SSDOS_k_ERR_DWNLD_EXP_LEN1:
      {
        SPRINTF1(pac_str, "%#x - SSDOS_k_ERR_DWNLD_EXP_LEN1: "
                "SSDOS_DwnldInitExpRespSend():\n"
                "The maximum length of the addressed SOD entry is smaller "
                "than the length of the data to be written\n.",
                SSDOS_k_ERR_DWNLD_EXP_LEN1);
        break;
      }
      case SSDOS_k_ERR_DWNLD_SEG_LEN:
      {
        SPRINTF1(pac_str, "%#x - SSDOS_k_ERR_DWNLD_SEG_LEN: "
                "SSDOS_DwnldInitSegReqProc():\n"
                "The addressed SOD entry is not matching to the number "
                "of payload bytes of the requested segmented transfer\n.",
                SSDOS_k_ERR_DWNLD_SEG_LEN);
        break;
      }
      case SSDOS_k_ERR_REQ_PYLD_LEN:
      {
        SPRINTF1(pac_str, "%#x - SSDOS_k_ERR_REQ_PYLD_LEN: "
                "SSDOS_DwnldInitSegReqProc():\n"
                "The SSDOC request payload length is too small.\n",
                SSDOS_k_ERR_REQ_PYLD_LEN);
        break;
      }
      case SSDOS_k_ERR_DWNLD_SEG_LEN2:
      {
        SPRINTF1(pac_str,"%#x - SSDOS_k_ERR_DWNLD_SEG_LEN2: "
                "SSDOS_DwnldMidSegReqProc():\n"
                "Raw data length in the download middle segment is wrong\n",
                SSDOS_k_ERR_DWNLD_SEG_LEN2);
        break;
      }
      case SSDOS_k_ERR_DWNLD_SEG_LEN1:
      {
        SPRINTF1(pac_str,"%#x - SSDOS_k_ERR_DWNLD_SEG_LEN1: "
                "SSDOS_DwnldEndSegReqProc():\n"
                "Raw data length in the download end segment is wrong\n",
                SSDOS_k_ERR_DWNLD_SEG_LEN1);
        break;
      }

      /** SSDOSsodAcs.c **/
      case SSDOS_k_ERR_LOCK_WR_SEG:
      {
        SPRINTF1(pac_str, "%#x - SSDOS_k_ERR_LOCK_WR_SEG: "
                "SSDOS_SodAcsReset():\n"
                "SOD_Write() was called to write a segment "
                "without locking the SOD\n",
                SSDOS_k_ERR_LOCK_WR_SEG);
        break;
      }
      case SSDOS_k_ERR_IDX_SIDX:
      {
        SPRINTF1(pac_str, "%#x - SSDOS_k_ERR_IDX_SIDX: "
                "SSDOS_SodAcsReqValid():\n"
                "SSDO Service Request received with inconsistant "
                "index or sub index.\n", SSDOS_k_ERR_IDX_SIDX);
        break;
      }
      case SSDOS_k_ERR_TOGGLE_BIT:
      {
        SPRINTF1(pac_str, "%#x - SSDOS_k_ERR_TOGGLE_BIT: "
                "SSDOS_SodAcsReqValid():\n"
                "SSDO Service Request with invalid toggle bit received.\n",
                SSDOS_k_ERR_TOGGLE_BIT);
        break;
      }
      case SSDOS_k_ERR_WR_ACS1:
      {
        SPRINTF1(pac_str, "%#x - SSDOS_k_ERR_WR_ACS1: SodAcsWriteAllowed():\n"
                "SSDO Service Request for download received, because "
                "the write access is disabled.\n",
                SSDOS_k_ERR_WR_ACS1);
        break;
      }
      case SSDOS_k_ERR_WR_ACS2:
      {
        SPRINTF1(pac_str, "%#x - SSDOS_k_ERR_WR_ACS2: SodAcsWriteAllowed():\n"
                "SSDO Service Request for download received, because "
                "the SOD is locked.\n", SSDOS_k_ERR_WR_ACS2);
        break;
      }
      case SSDOS_k_ERR_WR_ACS3:
      {
        SPRINTF1(pac_str, "%#x - SSDOS_k_ERR_WR_ACS3: SodAcsWriteAllowed():\n"
                "SSDO Service Request for download received, because "
                "the active instance is not in PRE_OPERATIONAL state.\n",
                SSDOS_k_ERR_WR_ACS3);
        break;
      }
      case SSDOS_k_ERR_WR_ACS4:
      {
        SPRINTF1(pac_str, "%#x - SSDOS_k_ERR_WR_ACS4: SodAcsWriteAllowed():\n"
                "SSDO Service Request for download received, because "
                "an other instance is in OPERATIONAL state.\n",
                SSDOS_k_ERR_WR_ACS4);
        break;
      }
      default:
      {
        SPRINTF(pac_str, "Unknown error code was generated by the SSDOS\n");
      }
    }

    SCFM_TACK_PATH();
    return;
    /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15
                                Notice #17 - Function comment content less than
                                             10.0% */
  }
    /* RSM_IGNORE_QUALITY_END */
#endif


/**
* @brief This function initializes the SSDOS FSM, the segment information for the segmented upload and download variables of SOD access.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll() or SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*/
static void FsmInit(BYTE_B_INSTNUM)
{
  t_SSDOS_FSM *ps_fsm = &s_Fsm[B_INSTNUMidx]; /* pointer to the fsm structure */

  ps_fsm->e_actState = k_WF_REQ_INIT;
  ps_fsm->b_saCmd = 0x00U;
  ps_fsm->dw_abortCode = (UINT32)SOD_ABT_NO_ERROR;

  /* The reference to the main SADR SOD entry is initialized once in
     SSDOS_Init(). This reference does not have to be reset */

  SSDOS_SegInfoInit(B_INSTNUM);
  SSDOS_SodAcsInit(B_INSTNUM);

  SCFM_TACK_PATH();
  return;
}

/**
* @brief This function sends an SSDOS abort response, resets the SOD access and calls FsmInit().
*
* @param        b_instNum		    instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        ps_rxBuf reference to received openSAFETY frame to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf reference to openSAFETY frame to be transmitted (pointer not checked, only called
*       with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        dw_abortCode abort code to be sent, (not checked, only called with enum) valid range:  see SOD_t_ABORT_CODES
*
* @return
* - TRUE              - reset of SOD access succeeded
* - FALSE             - reset of SOD access failed
*/
static BOOLEAN FsmResetAbortSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf, UINT32 dw_abortCode)
{
  BOOLEAN o_return = FALSE;            /* predefined return value */

  /* send abort response */
  SFS_NET_CPY32(&(ps_txBuf->ab_frmData[k_OFS_ABORT_CODE]), &dw_abortCode);
  SSDOS_SodAcsResp(B_INSTNUM_ ps_rxBuf, ps_txBuf, k_ABORT, EPLS_k_TR_NOT_USED,
                   (k_DATA_HDR_LEN + k_ABORT_LEN),TRUE);

  /* reset the SSDOS FSM */
  o_return = FsmReset(B_INSTNUM_ TRUE);

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function reset the SOD access and calls FsmInit().
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        o_abort
* - TRUE : reset because an abort frame was sent or received
* - FALSE : reset because of end of the transfer (not checked, any value allowed), valid range: TRUE, FALSE
*
* @return
* - TRUE           - reset of SOD access succeeded
* - FALSE          - reset of SOD access failed
*/
static BOOLEAN FsmReset(BYTE_B_INSTNUM_ BOOLEAN o_abort)
{
  BOOLEAN o_return = FALSE;            /* predefined return value */

  o_return = SSDOS_SodAcsReset(B_INSTNUM_ o_abort);
  FsmInit(B_INSTNUM);

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function processes state k_WF_REQ_INIT.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @retval          po_busy          flag signals that SSDO Server is currently processing a service request, valid range: <> NULL
* - TRUE:  Server busy
* - FALSE: Server ready to process (pointer not checked, only called with reference to variable in processStateReqProc())
*
* @return
* - TRUE           - State processed successfully
* - FALSE          - State processing failed
*/
static BOOLEAN WfReqInitState(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                              BOOLEAN *po_busy)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  t_SSDOS_FSM *ps_fsm = &s_Fsm[B_INSTNUMidx]; /* pointer to the fsm structure */
  BOOLEAN o_seg;       /* flag to signal the segmented upload */
  UINT32 dw_abortCode; /* variable to copy the abort code  */

  *po_busy = TRUE;

  /* if at least saCmd, index and sub-index are received and
        the payload length is smaller equal to the maximum payload length */
  if ((ps_rxBuf->s_frmHdr.b_le >= k_DATA_HDR_LEN) &&
      ((ps_rxBuf->s_frmHdr.b_le <= (UINT8)EPLS_cfg_MAX_PYLD_LEN) ||
       ((ps_rxBuf->s_frmHdr.b_id == k_SERVICE_REQ_FAST) &&
        (ps_rxBuf->s_frmHdr.b_le <= (UINT8)EPLS_cfg_MAX_PYLD_LEN_FAST_SSDO))))
  {
    /* get info from payload data area of received SSDO Service */
    SFS_NET_CPY8(&(ps_fsm->b_saCmd), &(ps_rxBuf->ab_frmData[k_OFS_SACMD]));

    /* if expedited download initiate */
    if (ps_fsm->b_saCmd == k_DWNLD_INIT_EXP)
    {
      /* process SSDOC request */
      ps_fsm->dw_abortCode = SSDOS_DwnldInitExpReqProc(B_INSTNUM_ ps_rxBuf);

      /* if no error happened */
      if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
      {
        ps_fsm->e_actState = k_DWNLD_RESP_INIT;
      }
      else /* error happened, abort code has to be sent */
      {
        ps_fsm->e_actState = k_RESP_ABORT;
      }
      o_return = TRUE;
    }
    /* else if segmented download initiate */
    else if (ps_fsm->b_saCmd == k_DWNLD_INIT_SEG)
    {
      /* process SSDOC request */
      ps_fsm->dw_abortCode = SSDOS_DwnldInitSegReqProc(B_INSTNUM_ ps_rxBuf);

      /* if no error happened */
      if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
      {
        ps_fsm->e_actState = k_DWNLD_RESP_INIT;
      }
      else /* error happened, abort code has to be sent */
      {
        ps_fsm->e_actState = k_RESP_ABORT;
      }
      o_return = TRUE;
    }
    /* else if segmented preload download initiate */
    else if (ps_fsm->b_saCmd == k_DWNLD_SEG_PRE_INIT)
    {

      /* process SSDOC request */
      ps_fsm->dw_abortCode = SSDOS_DwnldInitSegReqProc(B_INSTNUM_ ps_rxBuf);

      /* if no error happened */
      if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
      {
        ps_fsm->e_actState = k_DWNLD_RESP_INIT;
      }
      else /* error happened, abort code has to be sent */
      {
        ps_fsm->e_actState = k_RESP_ABORT;
      }
      o_return = TRUE;
    }
    /* else if upload initiate */
    else if (ps_fsm->b_saCmd == k_UPLD_INIT_EXP)
    {
      /* if the upload init request payload length is valid */
      if (ps_rxBuf->s_frmHdr.b_le == k_DATA_HDR_LEN)
      {
        /* process SSDOC request */
        ps_fsm->dw_abortCode =
            SSDOS_UpldInitReqProc(B_INSTNUM_ ps_rxBuf, &o_seg);

        /* if no error happened */
        if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
        {
          /* if segmented upload initiate received */
          if (o_seg)
          {
            /* prepare received SOD Access Command for response assembling */
            EPLS_BIT_SET_U8(ps_fsm->b_saCmd, k_SEG_SACMD);
          }
          /* no else : expedited upload initiate received */

          ps_fsm->e_actState = k_UPLD_RESP_INIT;
        }
        else /* error happened, abort code has to be sent */
        {
          ps_fsm->e_actState = k_RESP_ABORT;
        }
        o_return = TRUE;
      }
      else /* the upload init request payload length is invalid */
      {
        SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_UPLD_INIT_LEN,
                      (UINT32)ps_rxBuf->s_frmHdr.b_le);
      }
    }
    /* else if abort frame received */
    else if (ps_fsm->b_saCmd == k_ABORT)
    {
      /* if the abort frame length is valid */
      if (ps_rxBuf->s_frmHdr.b_le == (k_DATA_HDR_LEN + k_ABORT_LEN))
      {
        /* abort code is copied */
        SFS_NET_CPY32(&dw_abortCode, &ps_rxBuf->ab_frmData[k_OFS_ABORT_CODE]);

        SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_ABORT_RX, dw_abortCode);

        FsmReset(B_INSTNUM_ TRUE);
        o_return = FALSE; /* po_busy is set at the end of the
                             SSDOS_ProcessRequest() */
      }
      else /* the abort frame length is invalid, frame is ignored */
      {
        SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_ABORT_LEN,
                      (UINT32)ps_rxBuf->s_frmHdr.b_le);
      }
    }
    else /* unexpected or invalid SOD access command */
    {

      SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_SACMD, (UINT32)ps_fsm->b_saCmd);

      /* get the SOD index and sub-index for the abort code */
      SSDOS_SodAcsIdxCopy(B_INSTNUM_ ps_rxBuf->ab_frmData);

      ps_fsm->dw_abortCode = (UINT32)SOD_ABT_CMD_ID_INVALID;
      ps_fsm->e_actState = k_RESP_ABORT;

      o_return = TRUE;
    }
  }
  else /* invalid payload length, SSDOC request is ignored */
  {
    SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_PYLD_LEN,
                  (UINT32)ps_rxBuf->s_frmHdr.b_le);

    /* No abort frame is sent, because SOD index and sub-index are unknown */
    FsmReset(B_INSTNUM_ FALSE);
    o_return = FALSE;
  }

  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function processes state k_DWNLD_RESP_INIT.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame to be transmitted (pointer not checked, only called
*       with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - TRUE           - State processed successfully
* - FALSE          - State processing failed
*/
static BOOLEAN DwnldRespInitState(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                  EPLS_t_FRM *ps_txBuf)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  t_SSDOS_FSM *ps_fsm = &s_Fsm[B_INSTNUMidx]; /* pointer to the fsm structure */

  /* switch for the SOD access command */
  switch (ps_fsm->b_saCmd)
  {
    case k_DWNLD_INIT_SEG: /* download initiate segmented response send */
    {
      /* send response */
      ps_fsm->dw_abortCode = SSDOS_DwnldInitSegRespSend(B_INSTNUM_ ps_rxBuf,
                                                        ps_txBuf);

      /* if no error happened */
      if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
      {
        ps_fsm->e_actState = k_DWNLD_WF_REQ_SEG;
        o_return = TRUE;
      }
      else /* error happened, abort response is sent */
      {
        o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                      ps_fsm->dw_abortCode);
      }
      break;
    }
    case k_DWNLD_SEG_PRE_INIT: /* download initiate segmented response send */
    {
      /* send response */
      ps_fsm->dw_abortCode = SSDOS_DwnldInitPrelSegRespSend(B_INSTNUM_ ps_rxBuf,
                                                        ps_txBuf);

      /* if no error happened */
      if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
      {
        ps_fsm->e_actState = k_DWNLD_WF_REQ_SEG;
        o_return = TRUE;
      }
      else /* error happened, abort response is sent */
      {
        o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                      ps_fsm->dw_abortCode);
      }
      break;
    }
    case k_DWNLD_INIT_EXP: /* download initiate expedited response send */
    {
      /* send response */
      ps_fsm->dw_abortCode = SSDOS_DwnldInitExpRespSend(B_INSTNUM_ ps_rxBuf,
                                                        ps_txBuf);

      /* if no error happened */
      if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
      {
        o_return = FsmReset(B_INSTNUM_ FALSE);
      }
      else /* error happened, abort response is sent */
      {
        o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                      ps_fsm->dw_abortCode);
      }
      break;
    }
    #pragma CTC SKIP
    default: /* invalid SOD access command */
    {
      /* FATAL error is generated because this function is called in the
         2nd step with SaCmd set internally */
      SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_SACMD_INV, (UINT32)ps_fsm->b_saCmd);

      FsmReset(B_INSTNUM_ TRUE);
      o_return = FALSE;
    }
    #pragma CTC ENDSKIP
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function processes state k_DWNLD_WF_REQ_SEG.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame to be transmitted (pointer not checked, only
*       called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @retval          po_busy          flag signals that SSDO Server is currently processing a service request, valid range: <> NULL
* - TRUE:  Server busy
* - FALSE: Server ready to process (pointer not checked, only called with reference to variable in processStateReqProc())
*
* @return
* - TRUE           - State processed successfully
* - FALSE          - State processing failed
*/
static BOOLEAN DwnldWfReqSegState(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                  EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy)
{
   BOOLEAN o_return = FALSE;  /* predefined return value */
  t_SSDOS_FSM *ps_fsm = &s_Fsm[B_INSTNUMidx]; /* pointer to the fsm structure */
  BOOLEAN o_invPyldLen = TRUE;

  *po_busy = FALSE;

  /* if at least saCmd is received and
        the payload length is smaller equal to the maximum payload length */
  if (((ps_rxBuf->s_frmHdr.b_le <= (UINT8)EPLS_cfg_MAX_PYLD_LEN) ||
       ((ps_rxBuf->s_frmHdr.b_id == k_SERVICE_REQ_FAST) &&
        (ps_rxBuf->s_frmHdr.b_le <= (UINT8)EPLS_cfg_MAX_PYLD_LEN_FAST_SSDO))) &&
      (ps_rxBuf->s_frmHdr.b_le > k_DATA_HDR_LEN_SEG))
  {
    o_invPyldLen = FALSE;
  }

  if (!o_invPyldLen)
  {
    /* get info from payload data area of received SSDO Service */
    SFS_NET_CPY8(&ps_fsm->b_saCmd, &(ps_rxBuf->ab_frmData[k_OFS_SACMD]));

    /* if download middle segment received */
    if ((ps_fsm->b_saCmd & ((UINT8)(~k_TOGGLE_SACMD))) == k_DWNLD_SEG_MID)
    {
      /* if SSDOC request is valid */
      if (SSDOS_SodAcsReqValid(B_INSTNUM_ ps_rxBuf->ab_frmData, FALSE))
      {
        /* transfer SSDO Download Segment */
        ps_fsm->dw_abortCode = SSDOS_DwnldMidSegReqProc(B_INSTNUM_ ps_rxBuf,
                                                        ps_txBuf,
                                                        ps_fsm->b_saCmd);

        /* if no error happened */
        if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
        {
          o_return = TRUE;
        }
        else /* error happened, abort response is sent */
        {
          o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                       ps_fsm->dw_abortCode);
        }
      }
      /* no else : SSDOC request is ignored */
    }
    /* if download middle segment received */
    else if ((ps_fsm->b_saCmd & ((UINT8)(~k_TOGGLE_SACMD))) == k_DWNLD_SEG_PRE_MID)
    {
      /* if SSDOC request is valid */
#pragma CTC SKIP
      if (SSDOS_SodAcsReqValid(B_INSTNUM_ ps_rxBuf->ab_frmData, FALSE))
#pragma CTC ENDSKIP
      {
        /* transfer SSDO Download Segment */
        ps_fsm->dw_abortCode = SSDOS_DwnldMidPrelSegReqProc(B_INSTNUM_ ps_rxBuf,
                                                        ps_txBuf,
                                                        ps_fsm->b_saCmd);

        /* if no error happened */
#pragma CTC SKIP
        if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
#pragma CTC ENDSKIP
        {
          o_return = TRUE;
        }
        else /* error happened, abort response is sent */
        {
          o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                       ps_fsm->dw_abortCode);
        }
      }
      /* no else : SSDOC request is ignored */
    }    /* else if download end segment received */
    else if ((ps_fsm->b_saCmd & ((UINT8)(~k_TOGGLE_SACMD))) == k_DWNLD_SEG_END)
    {
      /* if SSDOC request is valid */
      if (SSDOS_SodAcsReqValid(B_INSTNUM_ ps_rxBuf->ab_frmData, FALSE))
      {
        /* transfer SSDO Download Segment */
        ps_fsm->dw_abortCode = SSDOS_DwnldEndSegReqProc(B_INSTNUM_ ps_rxBuf,
                                                        ps_txBuf,
                                                        ps_fsm->b_saCmd);

        /* if no error happened */
        if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
        {
          o_return = FsmReset(B_INSTNUM_ FALSE);
        }
        else /* error happened, abort response is sent */
        {
          o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                       ps_fsm->dw_abortCode);
        }
      }
      /* no else : SSDOC request is ignored */
    }
    else /* saCmd processed in the WfReqInitState() */
    {
      /* if abort frame received */
      if (ps_fsm->b_saCmd == k_ABORT)
      {
        /* if the abort request is valid */
        if (SSDOS_SodAcsReqValid(B_INSTNUM_ ps_rxBuf->ab_frmData, TRUE))
        {
          /* if reset succeeded */
          if (FsmReset(B_INSTNUM_ TRUE))
          {
            o_return = WfReqInitState(B_INSTNUM_ ps_rxBuf, po_busy);
          }
          /* no else : error happened */
        }
        /* no else : abort frame is ignored */
      }
      else /* other saCmd received */
      {
        /* if reset succeeded */
        if (FsmReset(B_INSTNUM_ TRUE))
        {
          o_return = WfReqInitState(B_INSTNUM_ ps_rxBuf, po_busy);
        }
        /* no else : error happened */
      }
    }
  }
  /* else : invalid payload length received */
  else
  {
    SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_PYLD_LEN1,
                    (UINT32)ps_rxBuf->s_frmHdr.b_le);

    o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                    (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
  }

  SCFM_TACK_PATH();
  return o_return;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */

/**
* @brief This function processes state k_UPLD_RESP_INIT.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame to be transmitted (pointer not checked, only
*       called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - TRUE           - State processed successfully
* - FALSE          - State processing failed
*/
static BOOLEAN UpldRespInitState(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  t_SSDOS_FSM *ps_fsm = &s_Fsm[B_INSTNUMidx]; /* pointer to the fsm structure */

  /* switch for the SOD access command */
  switch (ps_fsm->b_saCmd)
  {
    case k_UPLD_INIT_SEG: /* upload initiate segmented response send */
    {
      /* send response */
      SSDOS_UpldInitSegRespSend(B_INSTNUM_ ps_rxBuf, ps_txBuf);
      ps_fsm->e_actState = k_UPLD_WF_REQ_SEG;
      o_return = TRUE;
      break;
    }
    case k_UPLD_INIT_EXP: /* upload initiate expedited response send */
    {
      /* send response */
      ps_fsm->dw_abortCode = SSDOS_UpldInitExpRespSend(B_INSTNUM_ ps_rxBuf,
                                                        ps_txBuf);

      /* if no error happened */
      if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
      {
        o_return = FsmReset(B_INSTNUM_ FALSE);
      }
      else /* error happened, abort response is sent */
      {
        o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                     ps_fsm->dw_abortCode);
      }
      break;
    }
    #pragma CTC SKIP
    default: /* invalid SOD access command */
    {
      /* FATAL error is generated because this function is called in the
         2nd step with SaCmd set internally */
      SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_SACMD_INV1, (UINT32)ps_fsm->b_saCmd);

      FsmReset(B_INSTNUM_ TRUE);
      o_return = FALSE;
    }
    #pragma CTC ENDSKIP
  }

  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function processes state k_UPLD_WF_REQ_SEG.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame to be transmitted (pointer not checked, only
*       called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @retval         po_busy          flag signals that SSDO Server is currently processing a service request, valid range: <> NULL
* - TRUE:  Server busy
* - FALSE: Server ready to process (pointer not checked, only called with reference to variable in processStateReqProc())
*
* @return
* - TRUE           - State processed successfully
* - FALSE          - State processing failed
*/
static BOOLEAN UpldWfReqSegState(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  t_SSDOS_FSM *ps_fsm = &s_Fsm[B_INSTNUMidx]; /* pointer to the fsm structure */
  BOOLEAN o_end; /* flag to signal the end segment */

  *po_busy = FALSE;

  /* if the payload length is smaller equal to the maximum payload length and
        saCmd, index and sub-index are received*/
  if (((ps_rxBuf->s_frmHdr.b_le <= (UINT8)EPLS_cfg_MAX_PYLD_LEN) ||
       ((ps_rxBuf->s_frmHdr.b_id == k_SERVICE_REQ_FAST) &&
        (ps_rxBuf->s_frmHdr.b_le <= (UINT8)EPLS_cfg_MAX_PYLD_LEN_FAST_SSDO))) &&
      (ps_rxBuf->s_frmHdr.b_le >= k_DATA_HDR_LEN))
  {
    /* get info from payload data area of received SSDO Service */
    SFS_NET_CPY8(&(ps_fsm->b_saCmd), &(ps_rxBuf->ab_frmData[k_OFS_SACMD]));

    /* if upload middle segment received */
    if ((ps_fsm->b_saCmd & ((UINT8)(~(k_TOGGLE_SACMD)))) == k_UPLD_SEG_MID)
    {
      /* if the payload length is equal to the header length */
      if (ps_rxBuf->s_frmHdr.b_le == k_DATA_HDR_LEN)
      {
        /* if SSDOC request is valid */
        if (SSDOS_SodAcsReqValid(B_INSTNUM_ ps_rxBuf->ab_frmData, TRUE))
        {
          /* transfer SSDO Upload Segment */
          ps_fsm->dw_abortCode = SSDOS_UpldSegReqProc(B_INSTNUM_ ps_rxBuf,
                                                      ps_txBuf, &o_end,
                                                      ps_fsm->b_saCmd);

          /* if no error happened */
          if (ps_fsm->dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
          {
            /* if end segment was sent */
            if (o_end)
            {
              o_return = FsmReset(B_INSTNUM_ FALSE);
            }
            else /* middle segment was sent */
            {
              o_return = TRUE;
            }
          }
          else /* error happened, abort response is sent */
          {
            o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                         ps_fsm->dw_abortCode);
          }
        }
        /* no else : SSDOC request is ignored */
      }
      else /* invalid payload length */
      {
        SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_PYLD_LEN3,
                      (UINT32)ps_rxBuf->s_frmHdr.b_le);

        o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                      (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
      }
    }
    else /* saCmd processed in the WfReqInitState() */
    {
      /* if abort frame received */
      if (ps_fsm->b_saCmd == k_ABORT)
      {
        /* if the abort request is valid */
        if (SSDOS_SodAcsReqValid(B_INSTNUM_ ps_rxBuf->ab_frmData, TRUE))
        {
          /* if reset succeeded */
          if (FsmReset(B_INSTNUM_ TRUE))
          {
            o_return = WfReqInitState(B_INSTNUM_ ps_rxBuf, po_busy);
          }
          /* no else : error happened */
        }
        /* no else : abort frame is ignored */
      }
      else /* other saCmd received */
      {
        /* if reset succeeded */
        if (FsmReset(B_INSTNUM_ TRUE))
        {
          o_return = WfReqInitState(B_INSTNUM_ ps_rxBuf, po_busy);
        }
        /* no else : error happened */
      }
    }
  }
  else /* invalid payload length */
  {
    SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_PYLD_LEN2,
                  (UINT32)ps_rxBuf->s_frmHdr.b_le);

    o_return = FsmResetAbortSend(B_INSTNUM_ ps_rxBuf, ps_txBuf,
                                  (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH);
  }

  SCFM_TACK_PATH();
  return o_return;
}



/** @} */
