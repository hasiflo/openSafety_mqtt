/**
 * @addtogroup SCFM
 * @{
 * @file SCFMapi.h
 *
 * API interface for the SCFM module
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * @details
 * The function SCFM_TACK_PATH() is implemented as a macro due to optimisation.
 *
 */

#ifndef SCFMAPI_H
#define SCFMAPI_H

/**
 * @var SCFM_dw_PathProgress
 * This global variable records the branching of the program flow. It is
 * implemented as a counter.
 */
extern UINT32 SCFM_dw_PathProgress SAFE_NO_INIT_SEKTOR;

/**
 * @brief This function returns the number of function calls and significant program flow branches.
 * Precondition is that every function call and every significant program flow branch was
 * tacked before with SCFM_TACK_PATH(). Additionally the internal counter is reset to zero.
 *
 * @returns &lt;number of tacks&gt; - number of function calls/ flow branches
 */
UINT32 SCFM_GetResetPath(void);


/**
 * @brief This macro registers a sub function call or a significant branch in the program flow.
 */
#define SCFM_TACK_PATH()                          (SCFM_dw_PathProgress++)


#endif

/** @} */
