/*
 * \file UDP_SCM/SAPL/SAPLtarget.c
 *
 * *****************************************************************************
** Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
**                      IXXAT Automation GmbH
** All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
**
** This source code is free software; you can redistribute it and/or modify it
** under the terms of the BSD license (according to License.txt).
********************************************************************************
**
**   Workfile: SAPLtarget.c
**    Summary: Target - Platform specific source code
**             Target, platform specific source code
**             This file contains the platform source code.
**
**     Author: M. Molnar
**
********************************************************************************
********************************************************************************
**
**  Functions: SAPL_InitTarget
**             SAPLDEMO_KbHit
**             SAPL_Exit
**
**    Remarks:
**
**
*******************************************************************************/

/*******************************************************************************
**    compiler directives
*******************************************************************************/

/*******************************************************************************
**    include-files
*******************************************************************************/
#include <stdlib.h>

#include "../include/UDP.h"
#include "SODapi.h"

#include "SAPL.h"

//#include "oscrcmarker.h"

#if defined(_WIN32)
#include "tsctime\tsctime.h"
#endif

/*******************************************************************************
**    global variables
*******************************************************************************/

/*******************************************************************************
**    static constants, types, macros, variables
*******************************************************************************/

/*******************************************************************************
**    static function-prototypes
*******************************************************************************/

/*******************************************************************************
**    global functions
*******************************************************************************/
/*******************************************************************************
**
** Function    : SAPL_InitTarget
**
** Description : This function initializes the the target platform.
**
** Parameters  : -
**
** Returnvalue : TRUE  - success
**               FALSE - failure
**
*******************************************************************************/
BOOLEAN SAPL_InitTarget(void)
{
#if defined(_WIN32)
    tsctime_recalibrate();
#endif
  return TRUE;
}

/*******************************************************************************
**
** Function    : SAPL_KbHit
**
** Description : This function is waiting for pressing any key.
**
** Parameters  : -
**
** Returnvalue : -
**
*******************************************************************************/
void SAPLDEMO_KbHit(void)
{
  DBG_PRINTF("Exiting ...");
}

/*******************************************************************************
**
** Function    : SAPL_Exit
**
** Description : This function finishes the program after a fail safe error.
**
** Parameters  : -
**
** Returnvalue : -
**
*******************************************************************************/
void SAPL_Exit(void)
{
  SAPLDEMO_KbHit();
  exit(0);
}


/*******************************************************************************
**    static functions
*******************************************************************************/

