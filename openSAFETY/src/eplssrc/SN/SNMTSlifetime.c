/**
 * @addtogroup SNMTS
 * @{
 *
 * @file SNMTSlifetime.c
 *
 * The file contains all functions to process the SN Life Time. The
*              SN Life Time is composed of &lt;n&gt; guarding timeouts which results
*              in a SN Life Time of &lt;n&gt; * Guarding Time Intervals.
*
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SNMTSlifetime.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
 *
 */


#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "SCFMapi.h"
#include "SODapi.h"
#include "SOD.h"
#include "SERRapi.h"
#include "SERR.h"
#include "SNMTS.h"
#include "SNMTSint.h"
#include "SNMTSerr.h"


/**
 * @var as_AccessGuardingTime
 * SOD access structure array to get the guarding time from the SOD.
*/
static SOD_t_ACS_OBJECT_VIRT as_AccessGuardingTime[EPLS_cfg_MAX_INSTANCES]
                      SAFE_NO_INIT_SEKTOR;

/**
 * @var as_AccessLifeTime
 * SOD access structure array to get the life time factor from the SOD.
*/
static SOD_t_ACS_OBJECT_VIRT as_AccessLifeTime[EPLS_cfg_MAX_INSTANCES]
                      SAFE_NO_INIT_SEKTOR;

/**
 * @var adw_GuardingTimeout
 * Variable array to set the guarding timeout.
*/
static UINT32 adw_GuardingTimeout[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * @var ab_CntLifeTimeElapsed
 * Counter array for the life timer checking.
*/
static UINT8 ab_CntLifeTimeElapsed[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * @var ao_GuardTimerRunning
 * Flag array to check whether the guard timer is running.
*/
static UINT8 ao_GuardTimerRunning[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

static BOOLEAN guardTimeStart(BYTE_B_INSTNUM_ UINT32 dw_ct);

/**
 * @brief This function initializes SOD access to the guarding time, and the life factor of the SN. The guarding timeout, life time factor counter and the guard timer running flag are initialized also.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - initialization succeeded
 * - FALSE          - initialization failed
 */
BOOLEAN SNMTS_LifeTimeInit(BYTE_B_INSTNUM)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  ao_GuardTimerRunning[B_INSTNUMidx] = FALSE;
  ab_CntLifeTimeElapsed[B_INSTNUMidx] = 0x00U;
  adw_GuardingTimeout[B_INSTNUMidx] = 0x00UL;

  /* get SOD entry "Guard Time" */
  as_AccessGuardingTime[B_INSTNUMidx].s_obj.w_index = EPLS_k_IDX_LIFE_GUARDING;
  as_AccessGuardingTime[B_INSTNUMidx].s_obj.b_subIndex = EPLS_k_SUBIDX_GUARD_TIME;
  as_AccessGuardingTime[B_INSTNUMidx].dw_segOfs = 0;
  as_AccessGuardingTime[B_INSTNUMidx].dw_segSize = 0;

  /* if SOD entry is not available */
  if(NULL == SOD_AttrGetVirt(B_INSTNUM_ &as_AccessGuardingTime[B_INSTNUMidx], &s_errRes))
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* SOD entry "Guard Time" available */
  {
    /* get SOD entry "Life Time Factor" */
    as_AccessLifeTime[B_INSTNUMidx].s_obj.w_index = EPLS_k_IDX_LIFE_GUARDING;
    as_AccessLifeTime[B_INSTNUMidx].s_obj.b_subIndex = EPLS_k_SUBIDX_LIFE_TIME_FACTOR;
    as_AccessLifeTime[B_INSTNUMidx].dw_segOfs = 0;
    as_AccessLifeTime[B_INSTNUMidx].dw_segSize = 0;

    /* if SOD entry is not available */
    if(NULL == SOD_AttrGetVirt(B_INSTNUM_ &as_AccessLifeTime[B_INSTNUMidx], &s_errRes))
    {
      SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
    }
    else  /* SOD entry "Life Time Factor" available */
    {
      o_return = TRUE;
    }
  }
  SCFM_TACK_PATH();
  return o_return;
}


/**
 * @brief This function stops the "Life Time"- mechanism of the unit SNMTS.
 *
 * @see SNMTS_LifeTimeCheck(), SNMTS_LifeTimeStart()
 *
 * @param b_instNum       instance number (not checked, checked in SNMTS_TimerCheck()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 */
void SNMTS_LifeTimeStop(BYTE_B_INSTNUM)
{
  /* timer is stopped */
  ao_GuardTimerRunning[B_INSTNUMidx] = FALSE;
  SCFM_TACK_PATH();
}

/**
 * @brief This function starts the "Life Time"- mechanism of the unit SNMTS.
 *
 * @see          SNMTS_LifeTimeCheck(), SNMTS_LifeTimeStop()
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct            consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @return
 * - TRUE           - life time started
 * - FALSE          - error occurred, life time not started
 */
BOOLEAN SNMTS_LifeTimeStart(BYTE_B_INSTNUM_ UINT32 dw_ct)
{
  BOOLEAN o_return = FALSE;          /* predefined return value */
  UINT8 *pb_lifeTimeFactor = (UINT8 *)NULL;   /* ref to Life Time Factor in SOD,
                                                 only valid directly after SOD
                                                 read access */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* if guarding timer start succeeded */
  if (guardTimeStart(B_INSTNUM_ dw_ct))
  {
    /* get Life Time Factor from SOD */
    pb_lifeTimeFactor =
       (UINT8 *)SOD_ReadVirt(B_INSTNUM_ &as_AccessLifeTime[B_INSTNUMidx], &s_errRes);

    /* if read access failed */
    if(pb_lifeTimeFactor == NULL)
    {
      SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
    }
    else  /* read access succeeded */
    {
      /* set Safety Node Lifetime Counter */
      ab_CntLifeTimeElapsed[B_INSTNUMidx] = *pb_lifeTimeFactor;

      /* decrement Safety Node Lifetime Counter */
      ab_CntLifeTimeElapsed[B_INSTNUMidx]--;

      /* timer is started */
      ao_GuardTimerRunning[B_INSTNUMidx] = TRUE;

      o_return = TRUE;
    }
  }
  /* no else : guarding timer start failed, error was reported by the SOD */

  SCFM_TACK_PATH();
  return o_return;
}


/**
 * @brief This function checks the "Life Time" of a SN. As a result the SN life time can be elapsed or not. If the life time is not elapsed, the guarding timer is restarted
 *
 * @see           SNMTS_LifeTimeStart(), SNMTS_LifeTimeStop()
 *
 * @param        b_instNum          instance number (not checked, checked in SNMTS_TimerCheck()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @retval          po_elapsed         result of life time check (not checked, only called with reference to variable), valid range: TRUE, FALSE
 *
 * @return
 * - TRUE             - life time checked successfully
 * - FALSE            - error during life time check
 */
BOOLEAN SNMTS_LifeTimeCheck(BYTE_B_INSTNUM_ UINT32 dw_ct, BOOLEAN* po_elapsed)
{
  BOOLEAN o_return = FALSE;          /* predefined return value */

  /* if the timer running */
  if (ao_GuardTimerRunning[B_INSTNUMidx])
  {
    /* if the "Guarding Interval" elapsed */
    if(EPLS_TIMEOUT(dw_ct, adw_GuardingTimeout[B_INSTNUMidx]))
    {
      /* if "Safety Node Life Time" is elapsed */
      if(ab_CntLifeTimeElapsed[B_INSTNUMidx] == 0U)
      {
        /* Application is reported */
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_LIFE_TIME_ELAPSED,
                      SERR_k_NO_ADD_INFO);

        /* LIFETIME STATUS: elapsed */
        *po_elapsed = TRUE;
        o_return = TRUE;
      }
      else  /* "Safety Node Life Time" is running, still */
      {
        /* LIFETIME STATUS: running */
        *po_elapsed = FALSE;

        /* if guarding timer restart succeeded */
        if (guardTimeStart(B_INSTNUM_ dw_ct))
        {
          /* decrement Safety Node Lifetime Counter */
          ab_CntLifeTimeElapsed[B_INSTNUMidx]--;

          o_return = TRUE;
        }
        else /* else : guarding timer restart failed,
                       error was reported by the SOD */
        {
          /* timer is stopped */
          SNMTS_LifeTimeStop(B_INSTNUM);
        }
      }
    }
    else  /* "Guarding Interval" is running, still */
    {
      /* LIFETIME STATUS: running */
      *po_elapsed = FALSE;
      o_return = TRUE;
    }
  }
  else /* timer was not started */
  {
    SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_TIMER, SERR_k_NO_ADD_INFO);
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function starts the guarding timer.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame() or
* SNMTS_TimerCheck()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        dw_ct            consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @return
* - TRUE           - guarding timer started
* - FALSE          - error occurred, guarding timer not started error is already reported by the SOD
*/
static BOOLEAN guardTimeStart(BYTE_B_INSTNUM_ UINT32 dw_ct)
{
  BOOLEAN o_return = FALSE;          /* predefined return value */
  UINT32 *pdw_guardInterval = (UINT32 *)NULL;  /* ref to guard time in SOD, only
                                                  valid directly after SOD read
                                                  access */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get reference to guarding time in SOD */
  pdw_guardInterval =
            (UINT32 *)SOD_ReadVirt(B_INSTNUM_
                               &as_AccessGuardingTime[B_INSTNUMidx],
                               &s_errRes);

  /* if guarding time not available in SOD */
  if(pdw_guardInterval == NULL)
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  else  /* guarding time is available in SOD */
  {
    /* recalculate life guarding timeout value */
    adw_GuardingTimeout[B_INSTNUMidx] = dw_ct + *pdw_guardInterval;

    o_return = TRUE;
  }

  SCFM_TACK_PATH();
  return o_return;
}
/** @} */
