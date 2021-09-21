/**
 * \file SHNF_Time/Time.c
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

#include <time.h>
#include "datalogger.h"

#if WIN32
#pragma warning (disable : 4115) /* This pragma is used to avoid a compiler
                                    warning (warning C4115: _RPC_ASYNC_STATE
                                    named type definition in parentheses) in
                                    rpcasync.h windows header */
#include <windows.h>
#include "tsctime\tsctime.h"
#endif

#include <stdlib.h>

#include "Time.h"

/**
 * \brief For demo purposes, we will increment the timer, starting with zero at system start,
 * Therefore the initial start value of the system will be stored
 */
UINT64 *i_initialTimerValue = 0;

/**
 * \brief This function returns a timer value with the specified time base (resolution)
 *
 * \param e_timeBase the time base (\see openSAFETY_TIMEBASE)
 *
 * \return 32 bit timer value
 **/
UINT32 SHNFTime_getTime(openSAFETY_TIMEBASE e_timeBase)
{
    UINT64 d_elapsedTime;
    UINT32 u_timeFactor;

#if WIN32

    u_timeFactor = 10;

    if ( i_initialTimerValue == 0 )
    {
        i_initialTimerValue = (UINT64 *)malloc(sizeof(long));
        *i_initialTimerValue = gethectonanotime_last();
    }

    d_elapsedTime = ( gethectonanotime_first() - *i_initialTimerValue ) / 10000;

    Sleep(100);

#else
  struct timespec l_clockTime;

  // u_timeFactor corresponds to the fact, that the result of
  // the time calculation leads to a 1 musec value
  u_timeFactor = 10;

  // get the current system time (the resolution is 100ns)
  if ( clock_gettime( CLOCK_MONOTONIC, &l_clockTime) == -1 ) {
      return 0;
  }

  d_elapsedTime = (UINT64)((UINT64)l_clockTime.tv_sec * 10000000 ) + ((UINT64)l_clockTime.tv_nsec / 100 );


  if ( i_initialTimerValue == 0 )
  {
      i_initialTimerValue = (UINT64 *)malloc(sizeof(long));
      *i_initialTimerValue = d_elapsedTime;
      return 0;
  }
  if (d_elapsedTime < *i_initialTimerValue){
	  DATA_LOGGER("Time is negative\n");
  }

  d_elapsedTime = ( d_elapsedTime - *i_initialTimerValue ) / u_timeFactor;

#endif

  switch(e_timeBase)
  {
    case k_openSAFETY_TIMEBASE_1US:
    {
      d_elapsedTime = d_elapsedTime;
      break;
    }
    case k_openSAFETY_TIMEBASE_10US:
    {
      d_elapsedTime = d_elapsedTime / 10;
      break;
    }
    case k_openSAFETY_TIMEBASE_100US:
    {
      d_elapsedTime = d_elapsedTime / 100;
      break;
    }
    case k_openSAFETY_TIMEBASE_1MS:
    {
      d_elapsedTime = d_elapsedTime / 1000;
      break;
    }
    default: /* 100 nano sec */
    {
      d_elapsedTime = d_elapsedTime / 1;
      break;
    }
  }

  return (UINT32) d_elapsedTime ;
}
