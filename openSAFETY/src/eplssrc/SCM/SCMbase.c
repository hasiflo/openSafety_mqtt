/**
 * @addtogroup SCM
 * @{
 * @file SCMbase.c
 *
 * SCM internal base functions for SOD access and event handling via callback functions.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author Joachim Stolberg, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SCMbase.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>14.06.2010</td><td>Hans Pill</td><td>added 2 new errors for better diagnosis</td></tr>
 *     <tr><td>24.11.2010</td><td>Hans Pill</td><td>Review SL V20</td></tr>
 *     <tr><td>23.01.2013</td><td>Hans Pill</td><td>changes for additional parameters according to openSafety specification</td></tr>
 *     <tr><td>23.03.2013</td><td>Hans Pill</td><td>changes for handling of the new timestamp and CRC domain</td></tr>
 *     <tr><td>05.06.2013</td><td>Hans Pill</td><td>changes for CUnit / CTC, not testable condition removed</td></tr>
 *     <tr><td>25.06.2013</td><td>Hans Pill</td><td>removed unused variable</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>08.08.2013</td><td>Hans Pill</td><td>added CTC SKIP for unreachable code when using no application objects</td></tr>
 *     <tr><td>28.04.2014</td><td>Roman Zwischelsberger</td><td>previous change generated compiler warning</td></tr>
 *     <tr><td>24.06.2014</td><td>Hans Pill</td><td>length from SSDO upload may get checked for better diagnosis</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
 */

#include "SCMint.h"

/**
 * Array of FSM control blocks
 */
SCM_t_FSM_CB SCM_as_FsmCb[SCM_cfg_MAX_NUM_OF_NODES] SAFE_NO_INIT_SEKTOR;

/**
 * @name Static constants, types, macros, variables
 * @{
 */
/** Pointer to the number of free EPLS frames */
static UINT8 *pb_NumFreeFrms SAFE_NO_INIT_SEKTOR;

/** List of main SADR from the SOD to speed up the searching for Additional SADRs */
static UINT16 aw_MainSadr[EPLS_k_MAX_SADR] SAFE_NO_INIT_SEKTOR;
/** @} */

/**
* @brief This function is a callback function used by the SNMTM.
*
* Will be called for every SNMTM response frame based on a request initiated by the SCM.
* The response data are given via pb_data.
*
* @param	w_regNum	process handle, used to address the corresponding FSM
* 	slot (not checked, checked in SCM_Trigger()). valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param	w_tadr		TADR in the received response to check in case of the
* 	SADR assignment (not checked, checked in checkRxAddrInfo()), valid range:
* 	EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param    w_rxSdn		SDN in the received response to check in case of the
* 	SADR assignment (not checked, checked in checkRxAddrInfo()), valid range:
* 	k_MIN_SDN .. k_MAX_SDN
*
* @param    pb_data		Pointer to the received data bytes of the response frame,
* 	this data must be deserialized (not checked, only called with reference to
* 	struct if o_timeout == TRUE).
*
* @param   	o_timeout
* - TRUE  : timeout happened pb_data is invalid
* - FALSE : response received pb_data is valid (checked).
*/
void SCM_SNMTM_RespClbk(UINT16 w_regNum, UINT16 w_tadr, UINT16 w_rxSdn,
                        const UINT8 *pb_data, BOOLEAN o_timeout)
{
  SCM_t_FSM_CB *ps_fsmCb = &SCM_as_FsmCb[w_regNum];   /* pointer to the FSM
                                                         control block */

  /* if no timeout occurred */
  if(!o_timeout)
  {
    ps_fsmCb->w_tadr = w_tadr;
    ps_fsmCb->w_rxSdn = w_rxSdn;
    MEMCOPY(ps_fsmCb->adw_respBuff, pb_data, SCM_k_RESP_BUFF_SIZE);
    EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_SNMTM_RESP_RX);
  }
  else /* timeout occurred */
  {
    EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_SNMTM_TIMEOUT);
  }

  SCFM_TACK_PATH();
}

/**
* @brief This function is a callback function used by the SSDOC.
*
* This function will be called for every SSDOC response frame based on a request
* initiated by the SCM. The response is copied to the address specified via the
* request service function of the SSDOC and is not part of the following parameters.
*
* @param	w_regNum		process handle, used to address the corresponding FSM slot (not checked checked in SCM_Trigger()). valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param    dw_dataLen		 Number of received data bytes(not used). valid range: 0..8
*
* @param    dw_abortCode
* - == SOD_ABT_NO_ERROR: no error present reponse buffer and dw_transDataLen are valid
* - <> SOD_ABT_NO_ERROR: error present, dw_abortCode contains the abort code, reponse buffer and dw_transDataLen are not relevant (checked)
*/
void SCM_SsdocClbk(UINT16 w_regNum, UINT32 dw_dataLen, UINT32 dw_abortCode)
{
  SCM_t_FSM_CB *ps_fsmCb = &SCM_as_FsmCb[w_regNum];   /* pointer to the FSM
                                                         control block */
  /* if abort code is available */
  if(dw_abortCode != (UINT32)SOD_ABT_NO_ERROR)
  {
    ps_fsmCb->dw_SsdocUploadLen = dw_dataLen;

    /* if timeout occurred */
    if(dw_abortCode == (UINT32)SOD_ABT_SSDO_TIME_OUT)
    {
      EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_SSDOC_TIMEOUT);
    }
    else /* other error */
    {
      EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_RESP_ERROR);
      /* abort code is store to process later depending on the FSM state */
      ps_fsmCb->dw_SsdocAbortCode = dw_abortCode;
      /* signalize error to application */
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_ERR_SDO_ABORT_NODE,
                    (UINT32)ps_fsmCb->w_sadr | ((UINT32)ps_fsmCb->e_state << 16));
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_ERR_SDO_ABORT,
                    dw_abortCode);
    }
  }
  else /* abort code is not available */
  {
    EPLS_BIT_SET_U16(ps_fsmCb->w_event, SCM_k_EVT_SSDOC_RESP_RX);
    ps_fsmCb->dw_SsdocUploadLen = dw_dataLen;
  }
  SCFM_TACK_PATH();
}

/**
* @brief SCM unit local function used by the FSM state functions to simplify the SOD access.
*
* @param        w_idx 				 SOD index (not checked, checked in SOD_AttrGet()).
*
* @param        b_subidx 			 SOD sub-index (not checked, checked in SOD_AttrGet()).
*
* @return
* - <> NULL    - pointer to the SOD entry
* - == NULL    - SOD entry not available or failure
*/
void *SCM_SodRead(UINT16 w_idx, UINT8 b_subidx)
{
  void *pv_data = NULL;           /* pointer to the SOD entry */
  SOD_t_ERROR_RESULT s_errRes;    /* error result */
  SOD_t_ACS_OBJECT_VIRT s_sodAcs; /* SOD access structure */

  s_sodAcs.s_obj.w_index = w_idx;
  s_sodAcs.s_obj.b_subIndex = b_subidx;
  s_sodAcs.dw_segOfs = 0;
  s_sodAcs.dw_segSize = 0;
  /* if attribute is available */
  if(SOD_AttrGetVirt(EPLS_k_SCM_INST_NUM_ &s_sodAcs,&s_errRes) != NULL)
  {
    pv_data = SOD_ReadVirt(EPLS_k_SCM_INST_NUM_ &s_sodAcs, &s_errRes);

    /* if read access failed */
    if (pv_data == NULL)
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
    }
  }
  else /* attribute is not available */
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return pv_data;
}

/**
 * @brief SCM unit local function used by the FSM state functions to simplify the
 * SOD access.
 *
 * @param w_idx SOD index (not checked, checked in SOD_AttrGet ).
 * @param b_subidx SOD sub-index (not checked, checked in SOD_AttrGet ).
 *
 * @see SOD_AttrGet
 *
 * @return
 * - <> NULL - pointer to the SOD entry
 * - == NULL - SOD entry not available or failure
 */
UINT32 SCM_SodGetActLen(UINT16 w_idx, UINT8 b_subidx)
{
  SOD_t_ERROR_RESULT s_errRes;    /* error result */
  SOD_t_ACS_OBJECT_VIRT s_sodAcs; /* SOD access structure */
  UINT32 dw_len = 0UL;        /* length of the object */

  s_sodAcs.s_obj.w_index = w_idx;
  s_sodAcs.s_obj.b_subIndex = b_subidx;
  s_sodAcs.dw_segOfs = 0;
  s_sodAcs.dw_segSize = 0;
  /* if attribute is available */
  if(SOD_AttrGetVirt(EPLS_k_SCM_INST_NUM_ &s_sodAcs,
                 &s_errRes) != NULL)
  {
    /* function cannot return false for SCM objects */
    (void)SOD_ActualLenGetVirt(EPLS_k_SCM_INST_NUM_ &s_sodAcs, &dw_len);
  }
  else /* attribute is not available */
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return dw_len;
}

/**
* @brief This function gets a pointer to the given object.
*
* The given object can be accessed trough this pointer, therefore it is checked that no
* callback function is implemented.
*
* @param	w_idx		SOD index (not checked, checked in SOD_AttrGet()).
*
* @param	b_subidx	SOD sub-index (not checked, checked in SOD_AttrGet()).
*
* @return
* - <> NULL    - pointer to the SOD entry
* - == NULL    - SOD entry not available or failure
*/
void *SCM_SodPtrAcs(UINT16 w_idx, UINT8 b_subidx)
{
  void *pv_data = NULL;         /* pointer to the SOD entry */
  const SOD_t_ATTR *ps_sodAttr; /* pointer to the SOD attributes */
  SOD_t_ERROR_RESULT s_errRes;  /* SOD error result */
  SOD_t_ACS_OBJECT_VIRT s_sodAcs; /* SOD access structure */

  s_sodAcs.s_obj.w_index = w_idx;
  s_sodAcs.s_obj.b_subIndex = b_subidx;
  s_sodAcs.dw_segOfs = 0;
  s_sodAcs.dw_segSize = 0;

  /* if attribute is available */
  ps_sodAttr = SOD_AttrGetVirt(EPLS_k_SCM_INST_NUM_ &s_sodAcs,&s_errRes);

  /* if attribute is available */
  if(ps_sodAttr != NULL)
  {
    /* if no callback is implemented */
    if (EPLS_IS_BIT_RESET(ps_sodAttr->w_attr, SOD_k_ATTR_BEF_RD) &&
        EPLS_IS_BIT_RESET(ps_sodAttr->w_attr, SOD_k_ATTR_BEF_WR) &&
        EPLS_IS_BIT_RESET(ps_sodAttr->w_attr, SOD_k_ATTR_AFT_WR))
    {
      pv_data = SOD_ReadVirt(EPLS_k_SCM_INST_NUM_ &s_sodAcs, &s_errRes);

      /* if read access failed */
      if (pv_data == NULL)
      {
        SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                      (UINT32)(s_errRes.e_abortCode));
      }
    }
    else /* Callback function must not be implemented */
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_OBJ_CLBK_ERR, (UINT32)(w_idx));
    }
  }
  else /* attribute is not available */
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return pv_data;
}



/**
* @brief SCM unit local function used by the FSM state functions to
*               entry is available or not.
*
* @param      w_idx			SOD index (not checked, checked in SOD_AttrGet()).
*
* @param      b_subidx		SOD sub-index (not checked, checked in SOD_AttrGet()).
*
* @retval        po_error		If the return value is NULL, this out parameter signals if an error is occurred (TRUE) or if the entry is not available (FALSE), (not checked, only called with reference to variable).
*
* @return
* - <> NULL    - pointer to the SOD entry
* - == NULL    - SOD entry not available or error occurred
*/
void *SCM_SodQueryRead(UINT16 w_idx, UINT8 b_subidx, BOOLEAN *po_error)
{
  void *pv_data = NULL;                 /* pointer to the SOD entry */
  SOD_t_ERROR_RESULT s_errRes;          /* SOD access error */
  SOD_t_ACS_OBJECT_VIRT s_sodAcs; /* SOD access structure */

  s_sodAcs.s_obj.w_index = w_idx;
  s_sodAcs.s_obj.b_subIndex = b_subidx;
  s_sodAcs.dw_segOfs = 0;
  s_sodAcs.dw_segSize = 0;

  /* if attribute is available */
  if(SOD_AttrGetVirt(EPLS_k_SCM_INST_NUM_ &s_sodAcs,&s_errRes) != NULL)
  {
    pv_data = SOD_ReadVirt(EPLS_k_SCM_INST_NUM_ &s_sodAcs, &s_errRes);
    /* if error in SOD_Read */
    if(pv_data == NULL)
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
      *po_error = TRUE;           /* another SOD error */
    }
  }
  else  /* return value is a NULL pointer */
  {
    *po_error = FALSE;
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return pv_data;
}

/**
* @brief SCM unit local function used by the FSM state functions to simplify the SOD access.
*
* Used to write a domain to the SOD.
*
* @param        w_idx            SOD index (not checked, checked in SOD_AttrGet()).
*
* @param        b_subidx         SOD sub-index (not checked, checked in SOD_AttrGet()).
*
* @param        pb_data          pointer to the data (not checked, checked in SOD_Write()).
*
* @return
* - FALSE     - Abort forcing error
*/
BOOLEAN SCM_SodWrite(UINT16 w_idx, UINT8 b_subidx, const UINT8 *pb_data)
{
  BOOLEAN o_res = FALSE;                  /* function result */
  SOD_t_ERROR_RESULT s_errRes;            /* SOD access error */
  SOD_t_ACS_OBJECT_VIRT s_sodAcs; /* SOD access structure */

  s_sodAcs.s_obj.w_index = w_idx;
  s_sodAcs.s_obj.b_subIndex = b_subidx;
  s_sodAcs.dw_segOfs = 0;
  s_sodAcs.dw_segSize = 0;

  /* if attribute is available */
  if(SOD_AttrGetVirt(EPLS_k_SCM_INST_NUM_ &s_sodAcs,&s_errRes))
  {
    /* if SOD access succeeded */
    if (SOD_WriteVirt(EPLS_k_SCM_INST_NUM_ &s_sodAcs, pb_data, TRUE))
    {
      o_res =  TRUE;
    }
    else /* SOD write error */
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SCM_k_SOD_WRITE_ERR,
                    SERR_k_NO_ADD_INFO);
    }
  }
  else /* SOD access error */
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}

/**
* @brief This function sets the given status in the SOD.
*
* If the status is changed (compared to the local FSM control block) or the unchanged
* status shall be signaled, an application callback function is additionally called.
*
* @param        ps_fsmCb             Pointer to the current slot of the FSM control block (not checked, only called with reference to struct).
*
* @param        w_snNum              Safety Node number (not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param        b_nodeStatus         node status (not checked, only called with define).
*
* @param        o_reportSnSts
* - TRUE  : Unchanged SN status shall be reported.
* - FALSE : Unchanged SN status is not reported. (not checked, any value allowed)
*
* @return
* - FALSE     - Abort forcing error
*/
BOOLEAN SCM_SetNodeStatus(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                          UINT8 b_nodeStatus, BOOLEAN o_reportSnSts)
{
  BOOLEAN o_res = FALSE;          /* function result */

  /* if SOD write succeeded */
  if(SCM_SodWrite(SCM_k_IDX_SN_STATUS(w_snNum), &b_nodeStatus))
  {
    /* if the unchanged SN status shall be reported */
    if(o_reportSnSts)
    {
      /* inform application about the status change */
      SAPL_ScmNodeStatusChangedClbk(ps_fsmCb->w_sadr, ps_fsmCb->b_snStatus,
                                    b_nodeStatus);

      /* unchanged SN state was reported */
      ps_fsmCb->o_reportSnSts = FALSE;
    }
    else /* unchanged SN status is not reported */
    {
      /* if status changed */
      if(b_nodeStatus != ps_fsmCb->b_snStatus)
      {
        /* inform application about the status change */
        SAPL_ScmNodeStatusChangedClbk(ps_fsmCb->w_sadr, ps_fsmCb->b_snStatus,
                                      b_nodeStatus);
      }
    }

    /* if the Node status is missing */
    if (b_nodeStatus == SCM_k_NS_MISSING)
    {
      /* UDID is marked as not used */
      ps_fsmCb->o_udidUsed = FALSE;
    }

    ps_fsmCb->b_snStatus = b_nodeStatus;
    o_res = TRUE;
  }
  /* no else : SOD write error */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_res;
}

/**
* @brief This function builds a local list with main-SADRs from the "Additional SADR List".
*
* This list is necessary for the "Additional SADR Assignment" process. All UDID are marked
* as not used.
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_BuildLocalSadrList(void)
{
  UINT16 w_idx = 0U;                /* loop counter */
  BOOLEAN o_error = FALSE;          /* for the error from the SOD access */
  UINT16 *pw_sadr = (UINT16 *)NULL; /* pointer to the SADR for SOD access */

  /* Search through the complete list independent if entries are
     available or not. */
  while((w_idx < EPLS_k_MAX_SADR) && (o_error == FALSE))
  {
    pw_sadr = (UINT16 *)SCM_SodQueryRead((UINT16)(0xc801U + w_idx), 1U,
                                         &o_error);
    /* if valid entry */
    if(pw_sadr != NULL)
    {
      /* if additional SADR */
      if ((w_idx+1U) != *pw_sadr)
      {
        aw_MainSadr[w_idx] = *pw_sadr;
      }
      /* else Main SADR */
      else
      {
        aw_MainSadr[w_idx] = 0U; /* mark as "not available" */
      }
    }
    /* if SOD entry is not available */
    else if(o_error == FALSE)
    {
      aw_MainSadr[w_idx] = 0U;  /* mark as "not available" */
    }
    else  /* o_error == TRUE */
    {
      /* other SOD error */
    }

    w_idx++;
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return (BOOLEAN)(o_error == FALSE);
}

/**
* @brief This function searches in the local SADR-List for the next g1iven main SADR value
* and returns the corresponding additional SADR.
*
* Because one SN can have more than one additional SADR, the function must be called until
* zero is returned. The searches through the complete list until the end of the list is
* reached.
*
* @param	w_mainSadr		Main SADR (not checked, checked in ScmInitFsm()).
* @param    w_addSadr		Old "Additional SADR" as current search position.
* 	Must be zero at the beginning (not checked, checked in SCM_WfParamDlResp()).
*
* @return
* - > 0:   - valid additional SADR
* - = 0:   - end of list reached
*/
UINT16 SCM_GetNextAddSadr(UINT16 w_mainSadr, UINT16 w_addSadr)
{
  /* search for the next value in the list, which is equal to the main SADR */
  while((w_addSadr < EPLS_k_MAX_SADR) &&        /* not end-of-list */
        (aw_MainSadr[w_addSadr] != w_mainSadr)) /* and not the same main-SADR */
  {
    w_addSadr++;
  }

  /* if end-of-list found */
  if (w_addSadr == EPLS_k_MAX_SADR)
  {
    /* return "end of list reached" */
    w_addSadr = 0U;
  }
  else /* else: next valid add-SADR found */
  {
    w_addSadr++;  /* add-SADR = index + 1 */
  }
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return w_addSadr;
}

/**
* @brief Store the free frames pointer for later use.
*
* @see	  SCM_NumFreeFrmsDec()
*
* @param  pb_numFreeFrms	Pointer to the number of free frames. (not checked, checked in SCM_Trigger())
*/
void SCM_NumFreeFrmsSet(UINT8 *pb_numFreeFrms)
{
  pb_NumFreeFrms = pb_numFreeFrms;
  SCFM_TACK_PATH();
}

/**
* @brief Decrement the value behind the stored pb_numFreeFrms.
*
* @see SCM_NumFreeFrmsSet()
*/
void SCM_NumFreeFrmsDec(void)
{
  /* In case of a not initialized call of this function a decrement
   * would lead to undefined behavior */
  (*pb_NumFreeFrms)--;
  SCFM_TACK_PATH();
}

/** @} */
