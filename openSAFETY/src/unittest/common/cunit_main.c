/*
 * @file tstmain.c
 * This file defines the framework for unittests performed in the openSAFETY distribution
 *
 * @description
 * @{
 * ****************************************************************************
 * Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
 *                      IXXAT Automation GmbH
 * All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the BSD license (according to License.txt).
 ******************************************************************************
 * @}
 *
 * @author 2009 - Werner Abt, IXXAT Automation GmbH
 * @author 2011 - Bernecker & Rainer Industrie-Elektronik Ges.m.b.H
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <getopt.h>

#include <cunit/Automated.h>
#include <cunit/Basic.h>

#include <cunit_main.h>

extern void TST_AddTests(void);

static bool runDebug = FALSE;
bool CU_AllowDebug()
{
    return runDebug;
}


/****************************************************************************
**
** Function    : main
**
** Description : Test program entry point. Calls CUnit initialization routines,
**               adds all tests to the CUnit registry and executes them.
**
** Parameter   : -
**
** Returnvalue : always 0
**
** Remarks     : -
**
****************************************************************************/
int main(int argc, char **argv)
{
    unsigned int nrOfFailures = 0;

    bool printNrFailures = FALSE;
    bool runVerbose = TRUE;

    int c;

    while ( EOF != ( c = getopt ( argc, argv, "fdvq" ) ) )
    {
        switch( c )
        {
            case 'd':
                runDebug = TRUE;
                break;
            case 'f':
                printNrFailures = TRUE;
                break;
            case 'v':
                runVerbose = TRUE;
                break;
            case 'q':
                runVerbose = FALSE;
                break;
            default:
                break;
        }
    }

    // set test mode
    CU_basic_set_mode(runVerbose ? CU_BRM_VERBOSE : CU_BRM_SILENT);
    // exit on errors in the framework
    CU_set_error_action(CUEA_ABORT);

    if(CU_initialize_registry() == CUE_SUCCESS)
    {
        // Add all tests
        TST_AddTests();

#if CUNIT_AUTOMATED
        CU_automated_run_tests();
        CU_list_tests_to_file();
#else
        // execute tests
        if(CU_basic_run_tests() != CUE_SUCCESS)
            printf("\n Test run failed");

        nrOfFailures = CU_get_number_of_tests_failed();
#endif
        // cleanup CUnit registry
        CU_cleanup_registry();
    }

    if ( printNrFailures )
        printf ( "Nr of Failures: %d\n", nrOfFailures );

    return nrOfFailures;
}

/****************************************************************************
**
** Function    : TST_exit
**
** Description : Called by the function 'CU_set_error' if an internal CUnit
**               error occured. Exits the test run and shows abort condition
**               on the stdout.
**
** Parameter   : -
**
** Returnvalue : -
**
** Remarks     : -
**
****************************************************************************/
void TST_exit(void)
{
    // print out error on stdout
    printf("\nTest stopped by CUnit with internal error #%d: %s\n",
                CU_get_error(),
                CU_get_error_msg());
  // exit test program
    exit(CU_get_error());
}
