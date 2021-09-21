/**
 * @addtogroup SSDOC
 * @{
 *
 * @file SSDOCservTimer.c
 *
 * This file contains functionality to check, start the service response timer.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SSDOCservTimer.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>15.03.2013</td><td>Hans Pill</td><td>added statistic counters</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>11.09.2014</td><td>Hans Pill</td><td>added functionality for preload</td></tr>
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
#include "SSDOCapi.h"
#include "SSDOCint.h"


/** SOD access to entry 0x1201, 0x01, SSDO response timeout */
static SOD_t_ACS_OBJECT_VIRT s_AccessTimeout SAFE_NO_INIT_SEKTOR;


/** SOD access to entry 0x1201, 0x02, SSDO response repetitions */
static SOD_t_ACS_OBJECT_VIRT s_AccessRetries SAFE_NO_INIT_SEKTOR;

/** Structured data type to check the timeout of the SSDO Requests. */
typedef struct
{
  /** point of time response timeout is set */
  UINT32 dw_respTimeout;
  /** response timeout in SOD for the retransmission */
  UINT32 dw_sodRespTimeout;
  /** number of request repetitions */
  UINT8   b_retries;
} t_TIMER_CHECK;

/**
 * Service response timer structure.
 *
 * For every Service FSM exists an service response timer structure.
*/
static t_TIMER_CHECK as_ServTimer[SSDOC_cfg_MAX_NUM_FSM] SAFE_NO_INIT_SEKTOR;


/**
* @brief This function initializes the SOD access structures and
*               the as_ServTimer structure.
*
* @return
* - TRUE           - success
* - FALSE          - failure
*/
BOOLEAN SSDOC_TimerInit(void)
{
  BOOLEAN o_return = FALSE; /* predefined return value */
  UINT32 i; /* loop counter */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */


  /* get handle to SOD entry of SSDO Client response timeout */
  s_AccessTimeout.s_obj.w_index = EPLS_k_IDX_SSDO_COM_PARAM;
  s_AccessTimeout.s_obj.b_subIndex = EPLS_k_SUBIDX_SSDO_TIMEOUT;
  s_AccessTimeout.dw_segOfs = 0;
  s_AccessTimeout.dw_segSize = 0;

  /* if the SOD entry is available */
  if(NULL != SOD_AttrGetVirt(EPLS_k_SCM_INST_NUM_ &s_AccessTimeout,&s_errRes))
  {
    s_AccessRetries.s_obj.w_index = EPLS_k_IDX_SSDO_COM_PARAM;
    s_AccessRetries.s_obj.b_subIndex = EPLS_k_SUBIDX_SSDO_RETRIES;
    s_AccessRetries.dw_segOfs = 0;
    s_AccessRetries.dw_segSize = 0;
    /* get handle to SOD entry of SSDO Client response retries */

    /* if the SOD entry is available */
    if(NULL != SOD_AttrGetVirt(EPLS_k_SCM_INST_NUM_ &s_AccessRetries, &s_errRes))
    {
      /* init module global data structure of &lt;n&gt; the SSDOC Service FSM */
      for(i = 0UL ; i < (UINT32)SSDOC_cfg_MAX_NUM_FSM ; i++)
      {
        as_ServTimer[i].dw_sodRespTimeout = 0x00UL; /* no timeout set */
        as_ServTimer[i].dw_respTimeout = 0x00UL; /* no timeout set */
        as_ServTimer[i].b_retries = 0x00u;       /* no repetitions */
      }
      o_return = TRUE;
    }
    else /* else : SOD entry not available */
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
    }
  }
  else /* else : SOD entry not available */
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function starts the given response timer.
*
* @param        w_fsmNum        FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest()
* 	or SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        dw_ct           consecutive time (not checked, any value allowed),
*   valid range: any 32 bit value
*
* @return
* - TRUE          - success
* - FALSE         - failure
*/
BOOLEAN SSDOC_TimerStart(UINT16 w_fsmNum, UINT32 dw_ct)
{
  BOOLEAN o_return = FALSE;        /* predefined return value */
  UINT32 *pdw_respTimeout = (UINT32 *)NULL;  /* ref to response timeout, only
                                                valid after SOD read access */
  UINT8 *pb_reqRetries = (UINT8 *)NULL;      /* ref to max. number of request
                                                retries, only valid after SOD
                                                read access */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get response timeout value from SOD */
  pdw_respTimeout = (UINT32 *)SOD_ReadVirt(EPLS_k_SCM_INST_NUM_
                                       &s_AccessTimeout,
                                       &s_errRes);

  /* if read access to SOD succeeded */
  if(pdw_respTimeout != NULL)
  {
    /* set response timeout */
    as_ServTimer[w_fsmNum].dw_respTimeout = dw_ct + *pdw_respTimeout;
    /* Timeout value in the SOD is stored for the retransmission */
    as_ServTimer[w_fsmNum].dw_sodRespTimeout = *pdw_respTimeout;

    /* get response timeout value from SOD */
    pb_reqRetries = (UINT8 *)SOD_ReadVirt(EPLS_k_SCM_INST_NUM_
                                      &s_AccessRetries,
                                      &s_errRes);

    /* if read access to SOD succeeded */
    if(pb_reqRetries != NULL)
    {
      /* set maximum number of request repetitions */
      as_ServTimer[w_fsmNum].b_retries = *pb_reqRetries;

      o_return = TRUE;
    }
    else /* read access failed */
    {
      SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
    }
  }
  else /* read access failed*/
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function restarts the given response timer.
*
* @param        w_fsmNum        FSM number (not checked, checked in
*   SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest()
*   or SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        dw_ct           consecutive time (not checked, any value allowed),
* 	valid range: any 32 bit value
*/
void SSDOC_TimerReStart(UINT16 w_fsmNum, UINT32 dw_ct)
{
  /* set response timeout for the retransmission */
  as_ServTimer[w_fsmNum].dw_respTimeout =
      dw_ct + as_ServTimer[w_fsmNum].dw_sodRespTimeout;

  SCFM_TACK_PATH();
}
/**
* @brief This function sets the given response timer to timeout.
*
* @param        w_fsmNum        FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest()
* 	or SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*/
void SSDOC_TimerToTimeout(UINT16 const w_fsmNum)
{
  /* set response timeout for the retransmission */
  as_ServTimer[w_fsmNum].dw_respTimeout -= as_ServTimer[w_fsmNum].dw_sodRespTimeout;
  /* retransmission is no real timeout, therefore increase the number of retransmissions by one */
  as_ServTimer[w_fsmNum].b_retries++;
  SCFM_TACK_PATH();
}

/**
* @brief This function checks the given response timer.
*
* @param        w_fsmNum           FSM number (not checked, checked in
*   SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        dw_ct              consecutive time (not checked, any value allowed),
*   valid range: any 32 bit value
*
* @retval        po_timeout
*  - TRUE : timeout
*  - FALSE : no timeout (pointer not checked, only called with reference to
*    variable in SSDOC_CheckTimeout()), valid range: <> NULL
* @retval        po_retrans           only relevant if timeout happened (po_timeout = TRUE), valid range: <> NULL
*  - TRUE : retransmission is required
*  - FALSE : retransmission is not required, abort code has to be sent (pointer not
*    checked, only called with reference to variable in SSDOC_CheckTimeout())
*/
void SSDOC_TimerCheck(UINT16 w_fsmNum, UINT32 dw_ct, BOOLEAN *po_timeout,
                      BOOLEAN *po_retrans)
{
  /* if response time has elapsed */
  if(EPLS_TIMEOUT(dw_ct, as_ServTimer[w_fsmNum].dw_respTimeout))
  {
    *po_timeout = TRUE;

    /* if repetition counter reached 0 */
    if(as_ServTimer[w_fsmNum].b_retries == 0U)
    {
      /* report timeout */
      SERR_CountAcycEvt(w_fsmNum,SERR_k_SSDO_TOUT);
      *po_retrans = FALSE;
    }
    else /* repetition counter did not reached 0 */
    {
      /* report repetition */
      SERR_CountAcycEvt(w_fsmNum,SERR_k_SSDO_RETRY);
      as_ServTimer[w_fsmNum].b_retries--;
      *po_retrans = TRUE;
    }
  }
  else /* response time has not elapsed */
  {
    *po_timeout = FALSE;
  }

  SCFM_TACK_PATH();
}

/** @} */
