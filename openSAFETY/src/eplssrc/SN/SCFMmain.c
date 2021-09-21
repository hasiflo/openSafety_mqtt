/**
 * @addtogroup SCFM
 * @{
 * @file SCFMmain.c
 *
 * This file contains functions to observe the program flow of the openSAFETY Stack.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 */

#include "EPLStarget.h"
#include "SCFMapi.h"
#include "SCFM.h"

UINT32 SCFM_dw_PathProgress SAFE_NO_INIT_SEKTOR;

/**
* @brief This function initializes all module global and global variables defined in the unit SCFM.
*/
void SCFM_Init(void)
{
  SCFM_dw_PathProgress = 0x00UL;  /* reset 32 bit counter to zero */

  return;
}

/**
* @brief This function returns the number of function calls and significant program flow branches. Precondition is that every function call and every significant program flow branch was tacked before with SCFM_TACK_PATH(). Additionally the internal counter is reset to zero.
*
* @returns &lt;number of tacks&gt; - number of function calls/ flow branches
*/
UINT32 SCFM_GetResetPath(void)
{
  UINT32 dw_return = SCFM_dw_PathProgress;  /* prepare return value of number of
                                               function calls (and sig. program
                                               flow branches) */
  SCFM_dw_PathProgress = 0x00UL;            /* reset internal counter */

  return dw_return;
}


/** @} */
