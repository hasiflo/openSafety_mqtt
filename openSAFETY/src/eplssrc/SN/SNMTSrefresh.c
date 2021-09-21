/**
 * @addtogroup SNMTS
 * @{
 *
 * @file SNMTSrefresh.c
 *
 * The file contains all functions to process the Refresh Timer.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K.Fahrion, IXXAT Automation  GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SNMTSrefresh.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>06.09.2011</td><td>Hans Pill</td><td>A&P263905 number of bootup messages may be limited</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
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
#include "SNMTS.h"
#include "SNMTSint.h"
#include "SNMTSerr.h"


/**
 * Default setting for continuous retries
 */
#define EPLS_k_CONTINUOUS_RETRIES	0xFF

/**
 * @var as_AccessRefreshTime
 * SOD access structure array to get the refresh time from the SOD.
*/
static SOD_t_ACS_OBJECT_VIRT as_AccessRefreshTime[EPLS_cfg_MAX_INSTANCES]
                      SAFE_NO_INIT_SEKTOR;
/**
 * @var as_AccessRetries
 * SOD access structure array to get the refresh time from the SOD.
*/
static SOD_t_ACS_OBJECT_VIRT as_AccessRetries[EPLS_cfg_MAX_INSTANCES]
                      SAFE_NO_INIT_SEKTOR;

/**
 * @var adw_RefreshTimeout
 * Variable array to set the refresh timeout.
*/
static UINT32 adw_RefreshTimeout[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * @var auc_ElapsedRetries
 * Variable array to store the elapsted retries number.
*/
static UINT8 auc_ElapsedRetries[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;


static BOOLEAN SNMTS_RefreshTimeUpdate(BYTE_B_INSTNUM_ UINT32 dw_ct);

/**
 * @brief This function initializes the "Refresh Time"- mechanism of the unit SNMTS.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - initialization succeeded
 * - FALSE          - initialization failed
 */
BOOLEAN SNMTS_RefreshTimeInit(BYTE_B_INSTNUM)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  adw_RefreshTimeout[B_INSTNUMidx] = 0x00UL;
  auc_ElapsedRetries[B_INSTNUMidx] = 0x00;

  /* get SOD entry "Refresh Time" */
  as_AccessRefreshTime[B_INSTNUMidx].s_obj.w_index = EPLS_k_IDX_PRE_OP_SIGNAL;
  as_AccessRefreshTime[B_INSTNUMidx].s_obj.b_subIndex = EPLS_k_SUBIDX_REFRESH_TIME;
  as_AccessRefreshTime[B_INSTNUMidx].dw_segOfs = 0;
  as_AccessRefreshTime[B_INSTNUMidx].dw_segSize = 0;

  /* if access to the SOD entry NOT possible */
  if(NULL == SOD_AttrGetVirt(B_INSTNUM_ &as_AccessRefreshTime[B_INSTNUMidx], &s_errRes))
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* SOD entry "Refresh Time" available */
  {
	  /* get SOD entry "Retries" */
    as_AccessRetries[B_INSTNUMidx].s_obj.w_index = EPLS_k_IDX_PRE_OP_RETRIES;
    as_AccessRetries[B_INSTNUMidx].s_obj.b_subIndex = EPLS_k_SUBIDX_NO_RETRIES;
    as_AccessRetries[B_INSTNUMidx].dw_segOfs = 0;
    as_AccessRetries[B_INSTNUMidx].dw_segSize = 0;

	  /* if access to the SOD entry NOT possible */
	  if(NULL == SOD_AttrGetVirt(B_INSTNUM_ &as_AccessRetries[B_INSTNUMidx], &s_errRes))
	  {
		SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
					  (UINT32)(s_errRes.e_abortCode));
	  }
	  else  /* SOD entry "Retries" available */
	  {
		o_return = TRUE;
	  }
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function starts the "Refresh Time"- interval of unit SNMTS.
*
* @param        b_instNum        instance number (not checked, checked in SNMTS_TimerCheck()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        dw_ct 		     consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @return
* - TRUE           - refresh time started
* - FALSE          - error occurred, refresh time not started
*/
static BOOLEAN SNMTS_RefreshTimeUpdate(BYTE_B_INSTNUM_ UINT32 dw_ct)
{
  BOOLEAN o_return = FALSE;            /* predefined return value */
  UINT32 *pdw_refreshInterval = (UINT32*)NULL;  /* ref to Refresh Time Interval
                                                   of SN in SOD, only valid
                                                   directly after SOD read
                                                   access */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get Refresh Time from SOD */
  pdw_refreshInterval = (UINT32 *)SOD_ReadVirt(B_INSTNUM_
                                   &as_AccessRefreshTime[B_INSTNUMidx],
                                   &s_errRes);

  /* if read access failed */
  if(pdw_refreshInterval == NULL)
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* read access succeeded */
  {
    /* restart the refreshing interval */
    adw_RefreshTimeout[B_INSTNUMidx] = dw_ct + *pdw_refreshInterval;
     o_return = TRUE;
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
 * @brief This function starts the "Refresh Time"- interval of unit SNMTS.
 *
 * @see          SNMTS_RefreshTimeCheck()
 *
 * @param        b_instNum        instance number (not checked, checked in SNMTS_TimerCheck()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct            consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @return
 * - TRUE           - refresh time started
 * - FALSE          - error occurred, refresh time not started
 */
BOOLEAN SNMTS_RefreshTimeStart(BYTE_B_INSTNUM_ UINT32 dw_ct)
{
	BOOLEAN o_return = FALSE;            /* predefined return value */
	auc_ElapsedRetries[B_INSTNUMidx] = 0x00;
	o_return = SNMTS_RefreshTimeUpdate(B_INSTNUM_ dw_ct);
	return o_return;
}

/**
 * @brief This function checks the "Refresh Timer" of a SN. As a result the SN refresh time can be elapsed or not.
 * If the refresh time elapsed, the refresh timer is restarted.
 *
 * @param        b_instNum          instance number (not checked, checked in SNMTS_TimerCheck()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @retval          po_elapsed         result of refresh time check (not checked, only called with reference to variable in WfSadrAssStep1State()), valid range: TRUE, FALSE
 *
 * @return
 * - TRUE             - refresh time checked successfully
 * - FALSE            - error during refresh time check
 *
 */
BOOLEAN SNMTS_RefreshTimeCheck(BYTE_B_INSTNUM_ UINT32 dw_ct,
                               BOOLEAN* po_elapsed)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  UINT8 *pucMaxRetries = (UINT8*)NULL;
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get Refresh Time from SOD */
  pucMaxRetries = (UINT8 *)SOD_ReadVirt(B_INSTNUM_
		  &as_AccessRetries[B_INSTNUMidx],
          &s_errRes);

  /* if read access failed */
  if(pucMaxRetries == NULL)
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
    *po_elapsed = FALSE;  /* set out parameter */
    o_return = FALSE;
  }
  /* if number of retries has reached the limit */
  else if ((EPLS_k_CONTINUOUS_RETRIES != *pucMaxRetries) &&
	       (auc_ElapsedRetries[B_INSTNUMidx] >= *pucMaxRetries))
  {
	    *po_elapsed = FALSE;  /* set out parameter */
	    o_return = TRUE;
  }
  /* if the refresh time interval has elapsed */
  else if(EPLS_TIMEOUT(dw_ct, adw_RefreshTimeout[B_INSTNUMidx]))
  {
    /* Application is reported */
    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_REFR_TIME_ELAPSED, SERR_k_NO_ADD_INFO);
    /* increment the elapsed retries */
    auc_ElapsedRetries[B_INSTNUMidx]++;
    *po_elapsed = TRUE;  /* set out parameter */
    o_return = SNMTS_RefreshTimeUpdate(B_INSTNUM_ dw_ct);
  }
  else /* refresh time is still running */
  {
    *po_elapsed = FALSE;  /* set out parameter */
    o_return = TRUE;
  }
  SCFM_TACK_PATH();
  return o_return;
}


/** @} */
