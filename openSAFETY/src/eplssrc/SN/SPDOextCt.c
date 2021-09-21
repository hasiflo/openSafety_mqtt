/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOextCt.c
 *
 * This file contains the functions for the 40-bit CT value.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author H.Pill, LARsys GmbH
 *
 * <h2>History for SPDOextCt.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
  *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>Created for SPDOs with 40 bit CT</td></tr>
 * </table>
 *
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "EPLScfgCheck.h"

#include "SPDOint.h"
#include "SERRapi.h"
#include "SERR.h"
#include "SPDOerr.h"

#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)

/** This array contains the variables for the extended (40-bit) CT value */
t_EXT_CT as_extCt[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
* @brief This function initializes the extended CT counters.
*
* @param        b_instNum        instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
*/
void SPDO_InitExtCt(BYTE_B_INSTNUM)
{
	t_EXT_CT* ps_extCt = &as_extCt[B_INSTNUMidx];
	/* set initial values */
	ps_extCt->ddw_initValue = 0;
	ps_extCt->ddw_actCt = 0;
	ps_extCt->dw_lastCt = 0;
	ps_extCt->o_allowed = FALSE;
	/* no return value */
	return;
}

/**
* @brief This function is used to set the initial CT value for the given instance.
*
* @param        b_instNum        instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        ddw_ctInitVal    initial CT value (not checked) valid range: 0..0x000000FFFFFFFFFFULL
*
*/
void SPDO_SetExtCtInitValue(BYTE_B_INSTNUM_ UINT64 const ddw_ctInitVal)
{
	t_EXT_CT* ps_extCt = &as_extCt[B_INSTNUMidx];
	/* remind initial value and also set the allowed bit */
	ps_extCt->ddw_initValue = ddw_ctInitVal;
	ps_extCt->ddw_actCt = ddw_ctInitVal;
	ps_extCt->dw_lastCt = 0;
	ps_extCt->o_allowed = TRUE;
  /* no return value */
  return;
}


/**
* @brief This function is used to get a reference to the extended CT counters of the given instance.
*
* @param        b_instNum        instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - t_EXT_CT* - reference to the variables
*/
t_EXT_CT* SPDO_GetExtCt(BYTE_B_INSTNUM)
{
  /* return a the reference */
	return &as_extCt[B_INSTNUMidx];
}

/**
* @brief This function is to be called periodically to inflate the 32-bit CT value of the system to a 64-bit value.
*
* @param        b_instNum        instance number valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct            system CT value (not checked) valid range: 0..UINT32
*
*/
void SPDO_UpdateExtCtValue(BYTE_B_INSTNUM_ UINT32 const dw_ct)
{
	UINT32 dw_diffCt;
	t_EXT_CT* ps_extCt;
#if (EPLS_cfg_MAX_INSTANCES > 1)
  /* if instance number is invalid */
  if(B_INSTNUMidx >= (UINT8)EPLS_cfg_MAX_INSTANCES)
  {
    SERR_SetError(EPLS_k_NO_INSTANCE, SPDO_k_ERR_INST_NUM_5,
                  (UINT32)B_INSTNUMidx);
  }
  else
#endif
  {
    ps_extCt = &as_extCt[B_INSTNUMidx];
    /* calculate the elapsed time in CT */
    dw_diffCt = dw_ct - ps_extCt->dw_lastCt;
    /* update the 64 bit CT timer */
    ps_extCt->ddw_actCt += dw_diffCt;
    /* update the "last" CT timer */
    ps_extCt->dw_lastCt = dw_ct;
  }
  /* no return value */
  return;
}
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */
/** @} */
