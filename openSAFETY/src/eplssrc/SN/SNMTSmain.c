/**
 * @addtogroup SNMTS
 * @{
 *
 * @file SNMTSmain.c
 *
 * The file contains functionality to initialize the unit SNMTS and to provide an error string to
 * all error that can occur in this unit. Also the API-functions are implemented in this file.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SNMTSmain.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>error message for 40 bit CT</td></tr>
 * </table>
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#include "SCFMapi.h"
#include "SERRapi.h"
#include "SERR.h"
#include "SODapi.h"
#include "SOD.h"
#include "SNMTSapi.h"
#include "SNMTS.h"
#include "SNMTSint.h"
#include "SNMTSerr.h"

/**
 * @brief This function calls the SNMTS initialization function.
 *
 * @param       b_instNum                instance number (not checked, checked in SSC_InitAll()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - initialization succeed
 * - FALSE          - initialization failed
 */
BOOLEAN SNMTS_Init(BYTE_B_INSTNUM)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */

  /* if refresh time initialization failed */
  if(!(SNMTS_RefreshTimeInit(B_INSTNUM)))
  {
    /* error: initialization failed, error already reported,
              return with predefined FALSE */
  }
  /* else if life time initialization failed */
  else if(!(SNMTS_LifeTimeInit(B_INSTNUM)))
  {
    /* error: initialization failed, error already reported,
              return with predefined FALSE */
  }
  /* else if access to parameter UDID failed */
  else if(!SNMTS_InitParamUdid(B_INSTNUM))
  {
    /* error: initialization failed, error already reported,
              return with predefined FALSE */
  }
  /* else if access to Main SADR failed */
  else if(!SNMTS_SodAcsInit(B_INSTNUM))
  {
    /* error: initialization failed, error already reported,
              return with predefined FALSE */
  }
  /* else if application information init failed */
  else if(!SNMTS_AppInfoInit(B_INSTNUM))
  {
    /* error: initialization failed, error already reported,
              return with predefined FALSE */
  }
  else  /* the initialization succeeded */
  {
    /* init sn state */
    SNMTS_InitSnState(B_INSTNUM);

    o_return = TRUE;
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
 * @brief This function checks the guarding timer and the refreshing timer of the SN.
 *
 * The guarding timer is only checked in SN state OPERATIONAL. The refreshing timer is only checked in
 * SN state PRE-OPERATIONAL.
 *
 * @attention This function must be called at least once within the smallest value of the Refresh Time
 * (SOD index 0x100D sub-index 0x00) or Guarding Time (SOD index 0x100C sub-index 0x01) with number of
 * free management frame > 0.
 *
 * @param        b_instNum                     instance number (checked), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct                         consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @retval        pb_numFreeMngtFrms            IN: max. number of openSAFETY frames of type SNMT and SSDO. This number of
 *                       frames are free to transmit. (checked) , valid range: <> NULL, value > 0.
 *                       Returns the remaining number of free frames of type SNMT and SSDO. A value
 *                       of zero means that one or several SNMTS services were transmitted.
 *
 */
void SNMTS_TimerCheck(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT8 *pb_numFreeMngtFrms)
{
  BOOLEAN o_busy; /* output paramter of the SN state machine (not evaluated) */

  #if(EPLS_cfg_MAX_INSTANCES > 1)
    /* if instance number is invalid */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      /* error: instance number bigger than maximum */
      SERR_SetError(EPLS_k_NO_INSTANCE_ SNMTS_k_ERR_TIMER_CHK_INST_INV,
                    (UINT32)B_INSTNUM);
    }
    /* else if reference to number of free management frames is invalid */
    else

  #endif
    {
      /* if reference to number of free management frames is invalid */
      if(pb_numFreeMngtFrms == NULL)
      {
        /* error: reference to NULL */
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_REF_MNGT_FRMS_TC_INV,
                      SERR_k_NO_ADD_INFO);
      }
      /* else if NO management frames are available */
      else if((*pb_numFreeMngtFrms) == 0u)
      {
        /* NO free management frames available. The SNMT Slave reports an error
          and aborts immediately */
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_NO_MNGT_FRMS_AVAIL,
                      SERR_k_NO_ADD_INFO);
      }
      else  /* at least one management frame available */
      {
        /* if SN timer check failed */
        if(!(SNMTS_SnStateFsm(B_INSTNUM_ dw_ct, k_EVT_CHK_TIMER,
                              (EPLS_t_FRM *)NULL, (EPLS_t_FRM *)NULL,
                              pb_numFreeMngtFrms, &o_busy)))
        {
          /* error: processing of event k_EVT_CHK_TIMER failed,
                    error already reported */
        }
        else /* SN timer check succeeded */
        {

        }
      }
    }

  SCFM_TACK_PATH();
  return;
}


/**
 * @brief This function is used to store the CRC parameter checksum validation status calculated by the SN's application.
 *
 * @attention This function has to be called by the openSAFETY Application after the checksum calculation was
 * requested by the openSAFETY Stack via the callback function SAPL_SNMTS_CalcParamChkSumClbk().
 *
 * @param        b_instNum                      instance number (checked) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        o_calcParamChkSumValid         calculated parameter checksum is valid or not
 *
 * @return
 * - TRUE                   - valid function call after calculation was request
 * - FALSE                  - invalid function call, calculation request pending
 */
BOOLEAN SNMTS_PassParamChkSumValid(BYTE_B_INSTNUM_ BOOLEAN o_calcParamChkSumValid)
{
  BOOLEAN o_return = FALSE;           /* predefined return value */
  BOOLEAN o_busy; /* output paramter of the SN state machine (not evaluated) */

  #if(EPLS_cfg_MAX_INSTANCES > 1)  /* if multiple instances are configured */
    /* if instance number is invalid */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      /* error: instance number bigger than maximum */
      SERR_SetError(EPLS_k_NO_INSTANCE_ SNMTS_k_ERR_PASS_PARAM_INST_INV,
                    (UINT32)B_INSTNUMidx);
    }
    /* else if the API function call is not expected by the SNMTS */
    else
  #endif
    {
      /* if event <k_EVT_CHKSUM_AVAIL> succeeded to process */
      if(SNMTS_SnStateFsm(B_INSTNUM_ k_CT_NOT_USED_INT, k_EVT_CHKSUM_AVAIL,
                          (EPLS_t_FRM *)NULL, (EPLS_t_FRM *)NULL,
                          (UINT8 *)NULL, &o_busy))

      {
        /* store calculated parameter checksum */
        SNMTS_AppCalcParamStore(B_INSTNUM_ o_calcParamChkSumValid);
        o_return = TRUE;
      }
      /* no else : event <k_EVT_CHKSUM_AVAIL> failed to process, FATAL error
                    is already reported by the SNMTS_SnStateFsm() */
    }

  SCFM_TACK_PATH();
  return o_return;
}


/**
 * @brief This function is used to control the SN state transition into state OPERATIONAL.
 *
 * The function has to be called by the openSAFETY Application after the state transition was requested by the
 * openSAFETY Stack via the callback function SAPL_SNMTS_SwitchToOpReqClbk(). In case of transition to Operational
 * is forbidden, the application must specify a transition fail error and its assignment to an error group.
 *
 * @attention Error group SNMTS_k_FAIL_GRP_STK is reserved for openSAFETY Stack error codes
 *
 * @param        b_instNum             instance number (checked) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        o_enterOpState        TRUE : state transition to state OPERATIONAL is allowed FALSE: state transition to
 *                   state OPERATIONAL is NOT allowed (not checked, any value allowed)
 *
 * @param        b_errorGroup          group which the reported error is assigned to. Only valid in case of
 *                   o_enterOpState == FALSE (checked), valid range: see {fail error group} except SNMTS_k_FAIL_GRP_STK
 *
 * @param        b_errorCode           error reported by the application. Only valid in case of o_enterOpState == FALSE
 *                   (not checked), valid range: application specific
 *
 * @return
 * - TRUE                - valid function call after state transition was request
 * - FALSE               - invalid function call, no state transition request pending.
 */
BOOLEAN SNMTS_EnterOpState(BYTE_B_INSTNUM_ BOOLEAN o_enterOpState,
                           UINT8 b_errorGroup, UINT8 b_errorCode)
{
  BOOLEAN o_return = FALSE;               /* predefined return value */
  BOOLEAN o_busy; /* output paramter of the SN state machine (not evaluated) */

  #if(EPLS_cfg_MAX_INSTANCES > 1)  /* if multiple instances are configured */
    /* if instance number is invalid */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      /* error: instance number bigger than maximum */
      SERR_SetError(EPLS_k_NO_INSTANCE_ SNMTS_k_ERR_ENTER_OP_INST_INV,
                    (UINT32)B_INSTNUM);
    }
    else
  #endif  /* if multiple instances are configured */
    /* else if transition to Operational is forbidden and
               the error code is SNMTS_k_FAIL_GRP_STK */
    if ((!o_enterOpState) && (b_errorGroup == SNMTS_k_FAIL_GRP_STK))
    {
      /* error: error group <SNMTS_k_FAIL_GRP_STK> is reserved for
                stack internal errors */
      SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_ERR_GRP_INV, SERR_k_NO_ADD_INFO);
    }
    else
    {
      /* if event <k_EVT_CONF_AVAIL> succeeded to process */
      if (SNMTS_SnStateFsm(B_INSTNUM_ k_CT_NOT_USED_INT, k_EVT_CONF_AVAIL,
                          (EPLS_t_FRM *)NULL, (EPLS_t_FRM *)NULL,
                          (UINT8 *)NULL, &o_busy))
      {
        /* store error number and error group passed by the application */
        SNMTS_AppConfRespSet(B_INSTNUM_ o_enterOpState, b_errorGroup,
                            b_errorCode);
        o_return = TRUE;
      }
      /* no else : reject service request, error already reported */
    }

  SCFM_TACK_PATH();
  return o_return;
}


/**
 * @brief This function forces the SNMT Slave to switch the current SN into state PREOPERATIONAL.
 *
 * @attention This function must be called after the initialization of the openSAFETY SW see function SSC_InitAll().
 * Before calling this function, values in the SOD may be updated by the application. This function must not be
 * called in state Preoperational. This function may be called in state Operational.
 *
 * @param        b_instNum        instance number (checked) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct            consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @return
 * - TRUE           - state transition executed
 * - FALSE          - state transition NOT executed
 */
BOOLEAN SNMTS_PerformTransPreOp(BYTE_B_INSTNUM_ UINT32 dw_ct)
{
  BOOLEAN o_return = FALSE;                     /* predefined return value */
  BOOLEAN o_busy; /* output paramter of the SN state machine (not evaluated) */

  #if(EPLS_cfg_MAX_INSTANCES > 1)
    /* if instance number is invalid */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      /* error: instance number bigger than maximum */
      SERR_SetError(EPLS_k_NO_INSTANCE_ SNMTS_k_ERR_PERF_TRANS_PREOP_INST_INV,
                    (UINT32)B_INSTNUM);
    }
    else  /* instance number is valid */

  #endif
    {
      /* event <k_EVT_API_SET_TO_PREOP> processing */
      o_return = SNMTS_SnStateFsm(B_INSTNUM_ dw_ct, k_EVT_API_SET_TO_PREOP,
                                  (EPLS_t_FRM *)NULL, (EPLS_t_FRM *)NULL,
                                  (UINT8 *)NULL, &o_busy);
    }

  SCFM_TACK_PATH();
  return o_return;
}


#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
 * @brief This function returns a description of every available errors of the assigned unit.
 *
 * @param        w_errorCode         error number
 *
 * @param        dw_addInfo          additional error information
 *
 * @retval       pac_str             empty buffer to build the error string
 */
  void SNMTS_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str)
  {
    UINT16 w_expErr;
    UINT16 w_recErr;

    /* choose the matching error string */
    switch(w_errorCode)
    {

      /** SNMTSmain.c **/
      case SNMTS_k_ERR_TIMER_CHK_INST_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_TIMER_CHK_INST_INV: SNMTS_TimerCheck():\n"
          "The provided instance number (%02lu) is bigger than the "
          "maximum value.\n",
          SNMTS_k_ERR_TIMER_CHK_INST_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_REF_MNGT_FRMS_TC_INV:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_REF_MNGT_FRMS_TC_INV: SNMTS_TimerCheck():\n"
          "NULL pointer passed instead of reference to "
          "<number of free management frames>.\n",
          SNMTS_k_ERR_REF_MNGT_FRMS_TC_INV);
        break;
      }
      case SNMTS_k_ERR_NO_MNGT_FRMS_AVAIL:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_NO_MNGT_FRMS_AVAIL: SNMTS_TimerCheck():\n"
          "NO management frames available to build and transmit.\n",
          SNMTS_k_ERR_NO_MNGT_FRMS_AVAIL);
        break;
      }
      case SNMTS_k_ERR_PASS_PARAM_INST_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_PASS_PARAM_INST_INV: SNMTS_PassParamChkSumValid():\n"
          "The provided instance number (%02lu) is bigger than the "
          "maximum value.\n",
          SNMTS_k_ERR_PASS_PARAM_INST_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_ENTER_OP_INST_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_ENTER_OP_INST_INV: SNMTS_EnterOpState():\n"
          "The provided instance number (%02lu) is bigger than the "
          "maximum value.\n",
          SNMTS_k_ERR_ENTER_OP_INST_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_ERR_GRP_INV:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_ERR_GRP_INV: SNMTS_EnterOpState():\n"
          "Error group 4 was passed by the application."
          "(Reserved for EPLS Stack internal errors).\n",
          SNMTS_k_ERR_ERR_GRP_INV);
        break;
      }
      case SNMTS_k_ERR_PERF_TRANS_PREOP_INST_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_PERF_TRANS_PREOP_INST_INV: "
          "SNMTS_PerformTransPreOp():\n"
          "The provided instance number (%02lu) is bigger than the "
          "maximum value.\n",
          SNMTS_k_ERR_PERF_TRANS_PREOP_INST_INV, dw_addInfo);
        break;
      }


      /** SNMTSprocess.c **/
      case SNMTS_k_ERR_LE_UDID_REQ_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_LE_UDID_REQ_INV: processRequest():\n"
          "The value in LE field (%02lu) of the received Service "
          "<UDID Request> is NOT valid.\n",
          SNMTS_k_ERR_LE_UDID_REQ_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EXT_REQ_LEN:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EXT_REQ_LEN: processRequest():\n"
          "Payload length of the extended SNMT service request is 0.\n",
          SNMTS_k_ERR_EXT_REQ_LEN);
        break;
      }
      case SNMTS_k_ERR_RX_FRMID_REQ_PROC_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_RX_FRMID_REQ_PROC_INV: processRequest():\n"
          "The frame ID (%02lu) of the received SNMT Service is NOT defined.\n",
          SNMTS_k_ERR_RX_FRMID_REQ_PROC_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_LE_SET_TO_PREOP_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_LE_SET_TO_PREOP_INV: processRequestExt():\n"
          "The value in LE field (%02lu) of the received Service "
          "<SN set to PRE-OP> is NOT valid.\n",
          SNMTS_k_ERR_LE_SET_TO_PREOP_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_LE_SET_TO_OP_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_LE_SET_TO_OP_INV: processRequestExt():\n"
          "The value in LE field (%02lu) of the received Service "
          "<SN set to OP> is NOT valid.\n",
          SNMTS_k_ERR_LE_SET_TO_OP_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_LE_SCM_SET_TO_STOP_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_LE_SCM_SET_TO_STOP_INV: processRequestExt():\n"
          "The value in LE field (%02lu) of the received Service "
          "<SCM set to STOP> is NOT valid.\n",
          SNMTS_k_ERR_LE_SCM_SET_TO_STOP_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_LE_SCM_SET_TO_OP_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_LE_SCM_SET_TO_OP_INV: processRequestExt():\n"
          "The value in LE field (%02lu) of the received Service "
          "<SCM set to OP> is NOT valid.\n",
          SNMTS_k_ERR_LE_SCM_SET_TO_OP_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_LE_GUARD_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_LE_GUARD_INV: processRequestExt():\n"
          "The value in LE field (%02lu) of the received Service "
          "<Node Guarding> is NOT valid.\n",
          SNMTS_k_ERR_LE_GUARD_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_LE_ADD_SADR_ASS_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_LE_ADD_SADR_ASS_INV: processRequestExt():\n"
          "The value in LE field (%02lu) of the received Service "
          "<Add. SADR Assignment> is NOT valid.\n",
          SNMTS_k_ERR_LE_ADD_SADR_ASS_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_ADD_SADR_NOT_SUPPO:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_ADD_SADR_NOT_SUPPO: processRequestExt():\n"
          "It is only one transmit SPDO configurable. Assignment of the "
          "additional SADR can not be accomplished\n",
          SNMTS_k_ERR_ADD_SADR_NOT_SUPPO);
        break;
      }
      case SNMTS_k_ERR_LE_ERR_ACK_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_LE_ERR_ACK_INV: processRequestExt():\n"
          "The value in LE field (%02lu) of the received Service "
          "<Error Acknowledge> is NOT valid.\n",
          SNMTS_k_ERR_LE_ERR_ACK_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_LE_UDID_SCM:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_LE_UDID_SCM: processRequestExt():\n"
          "The value in LE field (%02lu) of the received Service "
          "<UDID of SCM assignment> is NOT valid.\n",
          SNMTS_k_ERR_LE_UDID_SCM, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_REQ_EXT_FRMTYP_INV1:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_REQ_EXT_FRMTYP_INV1: processRequest():\n"
          "The extended service type (%02lu) of the received SNMT Service "
          "is NOT defined.\n",
          SNMTS_k_ERR_REQ_EXT_FRMTYP_INV1, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_CT_FIELD_NOT_UNUSED:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_CT_FIELD_NOT_UNUSED: checkRequest():\n"
          "The value in CT field (%04lu) of the received frame is NOT zero.\n",
          SNMTS_k_ERR_CT_FIELD_NOT_UNUSED, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_TR_FIELD_NOT_UNUSED:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_TR_FIELD_NOT_UNUSED: checkRequest():\n"
          "The value in TR field (%02lu) of the received frame is NOT zero.\n",
          SNMTS_k_ERR_TR_FIELD_NOT_UNUSED, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_REQ_REJ_INV_DSADR:
      {
        SPRINTF3(pac_str, "%#x - SNMTS_k_ERR_REQ_REJ_INV_DSADR: "
                "filterServReq():\n"
                "The received SNMT Service was rejected by SNMTS filter - "
                "target address: %#x "
                "safety domain number: %#x\n",
                SNMTS_k_ERR_REQ_REJ_INV_DSADR,
                HIGH16(dw_addInfo),(UINT16)dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_LE_SADR_ASS_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_LE_SADR_ASS_INV: checkRxUdid():\n"
          "The value in LE field (%02lu) of the received Service "
          "<SADR Assignment> is NOT valid.\n",
          SNMTS_k_ERR_LE_SADR_ASS_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_RX_UDID_UNEVEN:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_RX_UDID_UNEVEN: checkRxUdid():\n"
          "Received UDID is NOT equal to the UDID of this SN.\n",
          SNMTS_k_ERR_RX_UDID_UNEVEN);
        break;
      }
      case SNMTS_k_ERR_LE_EXT_CT:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_LE_EXT_CT: processRequestExt():\n"
            "The value in LE field (%02lu) of the received Service "
            "<assign initial CT> is NOT valid.\n",
          SNMTS_k_ERR_LE_EXT_CT, dw_addInfo);
        break;
      }


      /** SNMTSsnstate.c **/
      case SNMTS_k_ERR_GET_SN_STATE_INST_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_GET_SN_STATE_INST_INV: SNMTS_GetSnState():\n"
          "The provided instance number (%02lu) is bigger than the "
          "maximum value.\n",
          SNMTS_k_ERR_GET_SN_STATE_INST_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_PTR_INV:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_PTR_INV: SNMTS_GetSnState():\n"
          "The provided state pointer is NULL.\n",
          SNMTS_k_ERR_PTR_INV);
        break;
      }
      case SNMTS_k_ERR_SN_STATE_MAIN_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_SN_STATE_MAIN_INV: SNMTS_SnStateFsm():\n"
          "Undefined FSM state (%02lu) occurred.\n",
          SNMTS_k_ERR_SN_STATE_MAIN_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF1:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF1: InitializationState():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF1, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT1:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT1: WfSadrAssStep1State():\n"
          "k_EVT_RX_SET_TO_OP event is unexpected.\n",
          SNMTS_k_ERR_EVENT1);
        break;
      }
      case SNMTS_k_ERR_EVENT15:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT15: WfSadrAssStep1State():\n"
          "k_EVT_RX_UDID_SCM event is unexpected.\n",
          SNMTS_k_ERR_EVENT15);
        break;
      }
      case SNMTS_k_ERR_EVENT2:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT2: WfSadrAssStep1State():\n"
          "k_EVT_RX_SADR_ADD event is unexpected.\n",
          SNMTS_k_ERR_EVENT2);
        break;
      }
      case SNMTS_k_ERR_EVENT14:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT14: WfSadrAssStep1State():\n"
          "k_EVT_RX_SET_TO_PREOP event is unexpected.\n",
          SNMTS_k_ERR_EVENT14);
        break;
      }
      case SNMTS_k_ERR_EVENT13:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT13: WfSadrAssStep1State():\n"
          "k_EVT_SN_ACK event is unexpected.\n",
          SNMTS_k_ERR_EVENT13);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF2:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF2: WfSadrAssStep1State():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF2, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF3:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF3: WfSadrAssStep2State():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF3, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT16:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT16: WfUdidScmAssState():\n"
          "k_EVT_RX_SET_TO_PREOP event is unexpected.\n",
          SNMTS_k_ERR_EVENT16);
        break;
      }
      case SNMTS_k_ERR_EVENT17:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT17: WfUdidScmAssState():\n"
          "k_EVT_SN_ACK event is unexpected.\n",
          SNMTS_k_ERR_EVENT17);
        break;
      }
      case SNMTS_k_ERR_EVENT18:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT18: WfUdidScmAssState():\n"
          "k_EVT_RX_SET_TO_OP event is unexpected.\n",
          SNMTS_k_ERR_EVENT18);
        break;
      }
      case SNMTS_k_ERR_EVENT19:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT19: WfUdidScmAssState():\n"
          "k_EVT_RX_SADR_ADD event is unexpected.\n",
          SNMTS_k_ERR_EVENT19);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF12:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF12: WfUdidScmAssState():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF12, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT20:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT20: WfSetToOp1Step1State():\n"
          "k_EVT_RX_UDID_SCM event is unexpected.\n",
          SNMTS_k_ERR_EVENT20);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF4:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF4: WfSetToOp1Step1State():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF4, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT27:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT27: WfSetToOp1Step2State():\n"
          "k_EVT_RX_SADR_ADD event is unexpected.\n",
          SNMTS_k_ERR_EVENT27);
        break;
      }
      case SNMTS_k_ERR_EVENT21:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT21: WfSetToOp1Step2State():\n"
          "k_EVT_RX_UDID_SCM event is unexpected.\n",
          SNMTS_k_ERR_EVENT21);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF5:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF5: WfSetToOp1Step2State():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF5, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT3:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT3: WfApiChksumState():\n"
          "k_EVT_RX_SADR_ADD event is unexpected.\n",
          SNMTS_k_ERR_EVENT3);
        break;
      }
      case SNMTS_k_ERR_EVENT9:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT9: WfApiChksumState():\n"
          "k_EVT_RX_SET_TO_PREOP event is unexpected.\n",
          SNMTS_k_ERR_EVENT9);
        break;
      }
      case SNMTS_k_ERR_EVENT22:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT22: WfApiChksumState():\n"
          "k_EVT_RX_UDID_SCM event is unexpected.\n",
          SNMTS_k_ERR_EVENT22);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF6:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF6: WfApiChksumState():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF6, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT4:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT4: WfSetToOp2State():\n"
          "k_EVT_RX_SADR_ADD event is unexpected.\n",
          SNMTS_k_ERR_EVENT4);
        break;
      }
      case SNMTS_k_ERR_EVENT10:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT10: WfSetToOp2State():\n"
          "k_EVT_RX_SET_TO_PREOP event is unexpected.\n",
          SNMTS_k_ERR_EVENT10);
        break;
      }
      case SNMTS_k_ERR_EVENT23:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT23: WfSetToOp2State():\n"
          "k_EVT_RX_UDID_SCM event is unexpected.\n",
          SNMTS_k_ERR_EVENT23);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF7:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF7: WfSetToOp2State():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF7, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT5:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT5: WfApiConfState():\n"
          "k_EVT_RX_SADR_ADD event is unexpected.\n",
          SNMTS_k_ERR_EVENT5);
        break;
      }
      case SNMTS_k_ERR_EVENT11:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT11: WfApiConfState():\n"
          "k_EVT_RX_SET_TO_PREOP event is unexpected.\n",
          SNMTS_k_ERR_EVENT11);
        break;
      }
      case SNMTS_k_ERR_EVENT24:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT24: WfApiConfState():\n"
          "k_EVT_RX_UDID_SCM event is unexpected.\n",
          SNMTS_k_ERR_EVENT24);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF8:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF8: WfApiConfState():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF8, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT6:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT6: WfSetToOp3State():\n"
          "k_EVT_RX_SADR_ADD event is unexpected.\n",
          SNMTS_k_ERR_EVENT6);
        break;
      }
      case SNMTS_k_ERR_EVENT12:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT12: WfSetToOp3State():\n"
          "k_EVT_RX_SET_TO_PREOP event is unexpected.\n",
          SNMTS_k_ERR_EVENT12);
        break;
      }
      case SNMTS_k_ERR_EVENT25:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT25: WfSetToOp3State():\n"
          "k_EVT_RX_UDID_SCM event is unexpected.\n",
          SNMTS_k_ERR_EVENT25);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF9:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF9: WfSetToOp3State():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF9, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT7:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT7: OperationStep1State():\n"
          "k_EVT_RX_SADR_ADD event is unexpected.\n",
          SNMTS_k_ERR_EVENT7);
        break;
      }
      case SNMTS_k_ERR_EVENT8:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_EVENT8: OperationStep1State():\n"
          "k_EVT_SN_ACK event is unexpected.\n",
          SNMTS_k_ERR_EVENT8);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF10:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF10: OperationStep1State():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF10, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_EVENT_UNDEF11:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_EVENT_UNDEF11: OperationStep2State():\n"
          "Unexpected FSM event (%02lu) occurred.\n",
          SNMTS_k_ERR_EVENT_UNDEF11, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_RX_TX_BUFF_NULL:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_RX_TX_BUFF_NULL: checkParam():\n"
          "The rx and the tx buffer pointer must not be NULL with "
          "the (%02lu) calling event.\n",
          SNMTS_k_ERR_RX_TX_BUFF_NULL, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_TXSPDO_NUM_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_TXSPDO_NUM_INV: enterAddSadr():\n"
          "The received SPDO number (%02lu) is NOT in valid range.\n",
          SNMTS_k_ERR_TXSPDO_NUM_INV, dw_addInfo);
        break;
      }
      case SNMTS_k_ERR_SPDO_NUM:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_SPDO_NUM: enterAddSadr():\n"
          "No SOD index can be found for the SPDO number (%02lu).\n",
          SNMTS_k_ERR_SPDO_NUM, dw_addInfo);
        break;
      }
//      case SNMTS_k_ERR_INV_RET:
//      {
//        SPRINTF2(pac_str,
//          "%#x - SNMTS_k_ERR_INV_RET: OperationStep1State():\n"
//          "Unexpected return value (%04u) of function \n"
//          "SAPL_SNMTS_SwitchOpOpClbk() received .\n",
//          SNMTS_k_ERR_INV_RET, dw_addInfo);
//        break;
//      }


      /** SNMTSappInfo.c **/
      case SNMTS_k_ERR_PARAM_CHKSUM_INV:
      {
        SPRINTF3(pac_str,
          "%#x - SNMTS_k_ERR_PARAM_CHKSUM_INV: SNMTS_AppChkSumComp():\n"
          "The received checksum (%04u) is NOT equal to calculated "
          "checksum (%04u).\n",
          SNMTS_k_ERR_PARAM_CHKSUM_INV, HIGH16(dw_addInfo), LOW16(dw_addInfo));
        break;
      }
      case SNMTS_k_ERR_PARAM_TIMESTAMP_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SNMTS_k_ERR_PARAM_TIMESTAMP_INV: SNMTS_AppChkSumComp():\n"
          "The received parameter timestamp (%08lu) is wrong.\n",
          SNMTS_k_ERR_PARAM_TIMESTAMP_INV, dw_addInfo);
        break;
      }


      /** SNMTSlifetime.c **/
      case SNMTS_k_ERR_LIFE_TIME_ELAPSED:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_LIFE_TIME_ELAPSED: SNMTS_LifeTimeCheck():\n"
          "The lifetime elapsed.\n",
          SNMTS_k_ERR_LIFE_TIME_ELAPSED);
        break;
      }
      case SNMTS_k_ERR_TIMER:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_TIMER: SNMTS_LifeTimeCheck():\n"
          "The guard timer was not started but checked.\n",
          SNMTS_k_ERR_TIMER);
        break;
      }

      /** SNMTSrefresh.c **/
      case SNMTS_k_ERR_REFR_TIME_ELAPSED:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_REFR_TIME_ELAPSED: SNMTS_RefreshTimeCheck():\n"
          "The refresh time elapsed.\n",
          SNMTS_k_ERR_REFR_TIME_ELAPSED);
        break;
      }

      /** SNMTSresponse.c **/
      case SNMTS_k_ERR_SN_ACK1:
      {
        w_expErr = HIGH16(dw_addInfo);
        w_recErr = LOW16(dw_addInfo);
        SPRINTF5(pac_str,
          "%#x - SNMTS_k_ERR_SN_ACK1: SNMTS_SnAckRespCheck():\n"
          "Unexpected error group and/or error code were received. "
          "Exp. error group : (%02u) error code : (%02u) "
          "Rec. error group : (%02u) error code : (%02u)\n",
          SNMTS_k_ERR_SN_ACK1,
          HIGH8(w_expErr), LOW8(w_expErr), HIGH8(w_recErr), LOW8(w_recErr));
        break;
      }
      case SNMTS_k_ERR_SN_ACK2:
      {
        SPRINTF1(pac_str,
          "%#x - SNMTS_k_ERR_SN_ACK2: SNMTS_SnAckRespCheck():\n"
          "Unexpected SN ack received (no SN FAIL was sent).\n",
          SNMTS_k_ERR_SN_ACK2);
        break;
      }
      default:
      {
        SPRINTF(pac_str, "Unknown error code was generated by the SNMTS\n");
      }
    }

    SCFM_TACK_PATH();
    return;
    /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15
                                Notice #18 - Function eLOC > maximum 200 eLOC
                                Notice #17 - Function comment content less than
                                             10.0% */
  }
    /* RSM_IGNORE_QUALITY_END */
#endif


/** @} */
