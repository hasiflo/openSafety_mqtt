/**
 * \file SHNF_Time/Time.h
 * \brief This file implements the consecutive timer, as is needed by the
 * openSAFETY stack
 *
 * \author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \author M.Molnar, IXXAT Automation GmbH (original Demo application)
  *
 * ****************************************************************************
 * Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
 *                      IXXAT Automation GmbH
 * All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the BSD license (according to License.txt).
 *******************************************************************************
 **/

#ifndef TIM_H
#define TIM_H

#include "SHNF_Definitions.h"

#include "EPLStarget.h"

/**
 * \brief This function returns a timer value with the specified time base (resolution)
 *
 * \param e_timeBase (IN) the time base (\see openSAFETY_TIMEBASE)
 *
 * \return 32 bit timer value
 */
UINT32 SHNFTime_getTime(openSAFETY_TIMEBASE e_timeBase);

#endif

