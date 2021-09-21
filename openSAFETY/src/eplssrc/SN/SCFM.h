/**
 * @addtogroup SN
 * @{
 * @addtogroup SCFM [SCFM] Safety Control Flow Monitoring
 * @{
 * @brief This unit provides functionality to register stack internal function calls and significant program flow branches.
 *
 * @file SCFM.h
 *
 * This unit provides functionality to register stack internal function calls and significant program flow branches.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * @details
 * The registration of these points during program flow is made by
 * tacking these points.
 */

#ifndef SCFM_H
#define SCFM_H

/**
* @brief This function initializes all module global and global variables defined in the unit @sa SCFM.
*/
void SCFM_Init(void);


#endif

/** @} */
/** @} */
