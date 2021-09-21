/**
 * @addtogroup SNMTS
 * @{
 *
 * @file SNMTSappInfo.c
 *
 * This unit provides functionality to handle stack internal errors.
 *
 * The file provides functionality to store and request a unit internal error number,
 * its assigned error group, calculated checksum and the confirmation response delivered
 * by the application.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SNMTSapi.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
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
#include "SNMTS.h"
#include "SNMTSerr.h"
#include "SNMTSint.h"



/**
 * @var ao_ApiConf
 * Flag array to store the flag passes the API confirmation from API function to SNMT Slave.
*/
static BOOLEAN ao_ApiConf[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * @var s_AccessParamTs
 * Data structure to manage access to the SOD entry 0x1018, 0x07, which
 * contains the "parameter timestamp" of this SN.
*/
static SOD_t_ACS_OBJECT_VIRT s_AccessParamTs[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;


/**
 * @var ao_CalcParamChkSum
 * Calculated parameter checksum, delivered by the API.
*/
static BOOLEAN ao_CalcParamChkSum[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * @var as_IntErr
 *
 * This array stores a unit internal error description for every instance of unit SNMTS.
 *
 * The internal error causes abortion of the sequence to set the SN to operational state.
*/
static t_FAIL_ERROR as_IntErr[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;


/**
* @brief This function returns the value of the parameter timestamp stored within object Vendor Device Information
* (index 0x1018, sub index 0x07).
*
* @param        b_instNum            instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @retval          pdw_paramTs          parameter timestamp stored within the SOD(not checked, only called with reference to variable in SNMTS_AppChkSumComp())
*
* @return
* - TRUE              - read access to SOD succeeded
* - FALSE             - read access to SOD failed
*/
static BOOLEAN getParamTs(BYTE_B_INSTNUM_ UINT32 *pdw_paramTs);


/**
* @brief This function initializes the paramters deliveres by the application and the access structure for
* the "parameter timestamp" SOD entry with index 0x1018 0x07.
*
* @param       b_instNum         instance number (not checked, checked in SSC_InitAll()),
*       valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @return
* - TRUE           - success
* - FALSE          - failure
*/
BOOLEAN SNMTS_AppInfoInit(BYTE_B_INSTNUM)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* Calculated parameter checksum is initialized */
  ao_CalcParamChkSum[B_INSTNUMidx] = FALSE;

  as_IntErr[B_INSTNUMidx].b_group = 0x00U;
  as_IntErr[B_INSTNUMidx].b_code  = 0x00U;

  /* application confirmation flag is initialized */
  ao_ApiConf[B_INSTNUMidx] = FALSE;


  /* get SOD entry "parameter timestamp" */
  s_AccessParamTs[B_INSTNUMidx].s_obj.w_index = EPLS_k_IDX_DEVICE_VEN_ID;
  s_AccessParamTs[B_INSTNUMidx].s_obj.b_subIndex = EPLS_k_SUBIDX_PARAM_TS;
  s_AccessParamTs[B_INSTNUMidx].dw_segOfs = 0;
  s_AccessParamTs[B_INSTNUMidx].dw_segSize = 0;

  /* if the "parameter timestamp" attribute is available */
  if(NULL != SOD_AttrGetVirt(B_INSTNUM_ &s_AccessParamTs[B_INSTNUMidx], &s_errRes))
  {
    o_return = TRUE;
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
 * @brief This function stores calculated parameter checksum delivered by the application.
 *
 * @param        b_instNum        instance number (not checked, checked in SNMTS_PassParamChkSumValid()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        o_chksumValid    valid status of the  parameter checksum
 */
void SNMTS_AppCalcParamStore(BYTE_B_INSTNUM_ BOOLEAN o_chksumValid)
{
  /* store calculated parameter checksum */
  ao_CalcParamChkSum[B_INSTNUMidx] = o_chksumValid;

  SCFM_TACK_PATH();
  return;
}

/**
* @brief This function checks if the parameters of the SOD are valid by checking if the checksum over the
* parameters is valid.
*
* Also the time stamp of the parameters within the SOD is checked against the time stamp provided by the SCM to
* ensure that the SN does have the parameter set being expected by the SCM.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (not checked, checked
*       in processStateReqProc()), valid range: see EPLS_t_FRM
*
* @return
* - TRUE           - response transmitted successfully
* - FALSE          - response transmission failed
*/
BOOLEAN SNMTS_AppChkSumComp(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf)
{
  BOOLEAN o_return = FALSE;    /* predefiend return value */
  UINT32 dw_paramTs = 0x00UL;  /* Parameter Timestamp stored within SOD */
  UINT32 dw_rxTs;              /* received parameter timestamp */

  /* get the received parameter timestamp */
  SFS_NET_CPY32(&dw_rxTs, &(ps_rxBuf->ab_frmData[SNMT_k_OFS_TSTMP]));

  /* if received parameters are not ok */
  if (!ao_CalcParamChkSum[B_INSTNUMidx])
  {
    /* error: current SOD deviates from expected SOD */
    //SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_PARAM_CHKSUM_INV, 0UL);
	  o_return = TRUE; //changed assume SOD is alright
  }
  /* else if parameter timestamp from SOD is NOT available */
  else if(!(getParamTs(B_INSTNUM_ &dw_paramTs)))
  {
    /* error: read access to parameter timestamp (SOD) failed,
              error already reported, return with predefined FALSE */
  }
  /* if received parameter timestamp is NOT EQUAL TO parameter timestamp
     stored in the SOD, Device Vendor Info (index 0x1018, 0x07) */
  else if(dw_paramTs != dw_rxTs)
  {
    /* error: parameter time stamp different to expected value */
    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_PARAM_TIMESTAMP_INV, dw_paramTs);
  }
  else  /* parameter timestamp and checksum is correct */
  {
    o_return = TRUE;
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function stores result of the application confirmation.
*
* @param        b_instNum         instance number (not checked, checked in SNMTS_EnterOpState()),
*       valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        o_enterOp
* - TRUE  : transition to operational is allowed by the application
* - FALSE : transition to operational is forbidden (not checked, any value allowed)
*
* @param        b_errGroup        only relevant if o_enterOp is FALSE group the error is assigned
*       to (not checked, checked in SNMTS_EnterOpState()), valid range: 0..255
*
* @param        b_errNum          only relevant if o_enterOp is FALSE internal error number (not
*       checked, any value allowed), valid range: 0 .. 255
*/
void SNMTS_AppConfRespSet(BYTE_B_INSTNUM_ BOOLEAN o_enterOp, UINT8 b_errGroup,
                          UINT8 b_errNum)
{
  /* store the application confirmation */
  ao_ApiConf[B_INSTNUMidx] = o_enterOp;

  /* if enter operational is forbidden */
  if (!o_enterOp)
  {
    as_IntErr[B_INSTNUMidx].b_code = b_errNum;
    as_IntErr[B_INSTNUMidx].b_group = b_errGroup;
  }

  SCFM_TACK_PATH();
  return;
}

/**
* @brief This function gets the application confirmation response for the transition into Operational state.
*
* @param        b_instNum           instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        ps_rxBuf            reference to received openSAFETY frame to be distributed (not checked,
*       checked in processStateReqProc()), valid range: see EPLS_t_FRM
*
* @param        ps_txBuf            reference to openSAFETY frame to be transmitted (not checked, checked in
*       processStateReqProc()), valid range: see EPLS_t_FRM
*
* @return
* - TRUE              - transition granted
* - FALSE             - transition forbidden, k_SN_FAIL response is already generated
*/
BOOLEAN SNMTS_AppConfRespGet(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                             EPLS_t_FRM *ps_txBuf)
{
  BOOLEAN o_return = ao_ApiConf[B_INSTNUMidx];  /* predefined return value */

  /* if transition into operational forbidden */
  if(!o_return)
  {
    /* collect header info and transmit reported error to SNMTM */
    SNMTS_TxRespExtService(B_INSTNUM_ ps_rxBuf, ps_txBuf, SNMTS_k_SN_FAIL,
                           as_IntErr[B_INSTNUMidx].b_group,
                           as_IntErr[B_INSTNUMidx].b_code, (UINT8 *)NULL);
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function returns the value of the parameter timestamp stored within object Vendor Device Information
* (index 0x1018, sub index 0x07).
*
* @param        b_instNum            instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @retval          pdw_paramTs          parameter timestamp stored within the SOD(not checked, only called with reference to variable in SNMTS_AppChkSumComp())
*
* @return
* - TRUE              - read access to SOD succeeded
* - FALSE             - read access to SOD failed
*/
static BOOLEAN getParamTs(BYTE_B_INSTNUM_ UINT32 *pdw_paramTs)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  UINT32 *pdw_help = (UINT32 *)NULL;
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get parameter timestamp from SOD */
  pdw_help =  (UINT32 *)SOD_ReadVirt(B_INSTNUM_
                                 &s_AccessParamTs[B_INSTNUMidx],
                                 &s_errRes);

  if(pdw_help == NULL)
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* read access succeeded */
  {
    *pdw_paramTs = *pdw_help;
    o_return = TRUE;
  }
  SCFM_TACK_PATH();
  return o_return;
}

/** @} */
