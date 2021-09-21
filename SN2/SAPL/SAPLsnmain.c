/******************************************************************************
 ** Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
 **                      IXXAT Automation GmbH
 ** All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 **
 ** This source code is free software; you can redistribute it and/or modify it
 ** under the terms of the BSD license (according to License.txt).
 ********************************************************************************
 **
 **   Workfile: SAPLsnmain.c
 **    Summary: SAPLdemo - Safety application Demonstration
 **             SAPL, application of the EPLsafety Stack
 **             This file contains a minimum application for demonstration of
 **             the general functionality of the EPLsafety Stack.
 **
 **     Author: M. Molnar
 **
 ********************************************************************************
 ********************************************************************************
 **
 **  Functions: main
 **
 **             InitCt
 **             InitEPLS
 **             Build
 **             Process
 **             PrintSnState
 **             OutputHeader
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
#ifdef linux
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif
#include "../include/UDP.h"

#include "EPLScfgCheck.h"

#include "SERRapi.h"

#include "SSCapi.h"
#include "SNMTSapi.h"
#include "SODapi.h"
#include "SPDOapi.h"
#include "SCFMapi.h"

#include "Time.h"
#include "SAPL.h"
#include "MessageHandler.h"



#include "datalogger.h"
#include "SAPL.h"

#ifndef EPLS_SW_CONFIG_CHECKED
#error EPLS Software Configuration is not checked!!!
#endif

/*******************************************************************************
 **    global variables
 *******************************************************************************/
//for printing the Application Output
static UINT32 printDelay= 10000000UL;
static UINT32 printCnt = 0L;
static UINT8  applData0_old = 0;

static char logfile[] = "logfile.log";

/*******************************************************************************
 **    static constants, types, macros, variables
 *******************************************************************************/

/** dw_Ct:
    Variable to store the consecutive time.
 */
static UINT32 dw_Ct = 0UL;

/** e_LastSnState:
    Variable to store the last SN state.
 */
static SNMTS_t_SN_STATE_MAIN e_LastSnState;

/*******************************************************************************
 **    static function-prototypes
 *******************************************************************************/
static BOOLEAN InitCt(void);
static BOOLEAN InitEPLS(void);
static void Build(void);
static void Process(void);

static void PrintSnState(SNMTS_t_SN_STATE_MAIN e_snState);
static void OutputHeader(void);

/*******************************************************************************
 **    global functions
 *******************************************************************************/
/*******************************************************************************
 **
 ** Function    : main
 **
 ** Description : See OutputHeader().
 **
 **
 ** Parameters  : -
 **
 ** Returnvalue : -
 **
 *******************************************************************************/
int main(void) /*lint !e970 (Note -- Use of modifier or type 'int' outside of a
                                     typedef [MISRA 2004 Rule 6.3]) */
{
    BOOLEAN o_break = TRUE; /* to avoid the warning C4127: conditional
                             expression is constant; while(1) */
    SNMTS_t_SN_STATE_MAIN e_actSnState; /* to get the actual SN state */

    /* if the initialization of the target was successful */
    if (SAPL_InitTarget())
    {
        /* print out headline of demo application */
        OutputHeader();

        /* The consecutive time value must be the same on both safety controller
       for the initialization */
        dw_Ct = SHNFTime_getTime(SAPL_cfg_CT_BASIS);


        /* initialize unit SHNF */
        SHNF_Init(SAPL_k_SN3_SADR);

        /* if the initialization of the EPLS stack was successful */
        if (InitEPLS())
        {
        	PrintApplicationData();
        	//ab_0_act_ProcessData[0]= 1U;
        	//ab_0_act_ProcessData[1] = 1U;
        	ProcessApplicationData();
            /* After the EPLS stack initialization, the application may copy data from
         non volatile memory to the SOD
         E.g.: Consecutive time is initialized */
            if (InitCt())
            {

                /* EPLS application main while loop */
                while(o_break)
                {
                    /* The consecutive time value must be the same on both safety
             controller and the same value during one application loop */
                    dw_Ct = SHNFTime_getTime(SAPL_cfg_CT_BASIS);
                    //DataLoggerFile(logfile, "CT:  %d\n", dw_Ct);

                    /* calculation of the CRC */
                    SAPL_CalculateCRC();

                    /* store of the SOD */
                    SAPL_StoreSOD();

                    Build();

                    Process();

                    UINT8 applData0 = getProcessData0();
                    if(applData0 != applData0_old){
                    	applData0_old = applData0;
                    	PrintApplicationData();
                    }

                    if(printCnt >= printDelay){
                    	UINT32 avg = getAVG();
                    	UINT32 cnt = getCNT();
                    	UINT32 maxDel = getMaxPropDelay();
                    	UINT32 minDel = getMinPropDelay();
                    	//increaseAppData();
                    	DATA_LOGGER1("Dw_Ct_next: %d\n", dw_Ct);
                    	printf("PropDelay: %d  CNT: %zu \n ",avg, cnt);
                    	printf("Max PropDelay: %zu \n ",maxDel);
                    	printf("Min PropDelay: %zu \n ",minDel);
						PrintSnState(e_actSnState);

						printCnt = 0;
					}else{
						printCnt++;
					}

                    /* if the actual SN state update succeeded */
                    if (SNMTS_GetSnState(&e_actSnState))
                    {
                        /* if the SN state changed */
                        if (e_actSnState != e_LastSnState)
                        {
                            /* print the actual SN state */
                            PrintSnState(e_actSnState);
                            /*store the last SN state */
                            e_LastSnState = e_actSnState;
                        }
                    }
                }
                /* no else : the error was already printed in the
                    SAPL_SERR_SignalErrorClbk() */
            }
            /* no else : the error was already printed in the
                  SAPL_SERR_SignalErrorClbk() */
        }
        /* no else : the error was already printed in the
                SAPL_SERR_SignalErrorClbk() */
    }

    SHNF_CleanUp();

    return(0);
}

/*******************************************************************************
 **    static functions
 *******************************************************************************/
/*******************************************************************************
 **
 ** Function    : InitCt
 **
 ** Description : This function initializes the consecutive time SOD object.
 **
 ** Parameters  : -
 **
 ** Returnvalue : TRUE  - success
 **               FALSE - failure
 **
 *******************************************************************************/
static BOOLEAN InitCt(void)
{
    BOOLEAN o_ret = FALSE;  /* return value */
    PTR_TYPE dw_sodHdl; /* SOD handle */
    BOOLEAN o_appObj; /* flag to signal access to an application object */
    INT8 c_ct = (INT8)SAPL_cfg_CT_BASIS; /* ct basis */
    SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

    /* if the ct SOD object is available */
    if (SOD_AttrGet(0x1200U, 0x03U, &dw_sodHdl, &o_appObj,
            &s_errRes) != NULL)
    {
        o_ret = SOD_Write(dw_sodHdl, o_appObj, &c_ct, FALSE,
                0UL, 0UL);
    }
    else /* SOD access error */
    {
        SAPL_SERR_SignalErrorClbk(s_errRes.w_errorCode,
                (UINT32)(s_errRes.e_abortCode));
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return o_ret;
}

/*******************************************************************************
 **
 ** Function    : InitEPLS
 **
 ** Description : This function initializes the EPLS stack and switches the SN to
 **               Pre-operational state.
 **
 ** Parameters  : -
 **
 ** Returnvalue : TRUE  - success
 **               FALSE - failure
 **
 *******************************************************************************/
static BOOLEAN InitEPLS(void)
{
    BOOLEAN o_ret = FALSE; /* return value */

    /* if initialization of the EPLS Stack failed */
    if(!SSC_InitAll())
    {
        /* the error was already printed in the SAPL_SERR_SignalErrorClbk() */
    }
    /* initialization of the EPLS Stack succeeded */
    else
    {
        /* if the initialisation of the last SN state succeeded */
        if (SNMTS_GetSnState(&e_LastSnState))
        {
            PrintSnState(e_LastSnState);

            /* transition to PreOperational */
            o_ret = SNMTS_PerformTransPreOp(dw_Ct);
        }
        /* no else : the initialization of the last SN state failed */
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return o_ret;
}

/*******************************************************************************
 **
 ** Function    : Build
 **
 ** Description : This function checks timeouts and builds SPDO frames.
 **
 ** Parameters  : -
 **
 ** Returnvalue : -
 **
 *******************************************************************************/
static void Build(void)
{
    UINT8 b_numFreeSpdoFrms = 1U; /* number of free SPDO frames can be sent per
                                  call of the SSC_BuildTxFrames */
    UINT8 b_numFreeMngtFrms = 1U; /* number of free management frames can be sent
                                  per call of the SSC_BuildTxFrames */

    /* Guard timeout is checked */
    SNMTS_TimerCheck(dw_Ct, &b_numFreeMngtFrms);

    /* SPDO frames are built */
    SPDO_BuildTxSpdo(dw_Ct, &b_numFreeSpdoFrms);

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
}

/*******************************************************************************
 **
 ** Function    : Process
 **
 ** Description : This function gets a EPLS frame from the SHNF and calls the
 **               corresponding process function.
 **
 ** Parameters  : -
 **
 ** Returnvalue : -
 **
 *******************************************************************************/
static void Process(void)
{
    UINT16  w_frameLength; /* length of received EPLS frame */
    UINT8  *pb_rxFrame;    /* reference to a received EPLS frame */
    SHNF_t_TEL_TYPE e_telType;

    /* get reference to received EPLS frame */
    pb_rxFrame = SHNF_GetEplsFrame(&w_frameLength, &e_telType);

    /* if EPLS frame was received */
    if(pb_rxFrame != NULL)
    {
        /* if the received frame is SPDO frame */
        if (e_telType == SHNF_k_SPDO)
        {
            /* The received SPDO frame is processed */
            SPDO_ProcessRxSpdo(dw_Ct, pb_rxFrame, w_frameLength);
        }
        /* else other frame was received */
        else
        {
            /* while the frame processing is busy and no fail safe error happened */
            while(SSC_ProcessSNMTSSDOFrame(dw_Ct, pb_rxFrame,
                    w_frameLength) == SSC_k_BUSY)
            {
                pb_rxFrame = (UINT8 *)NULL;
                w_frameLength = 0U;
            }
        }

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
        /* SCT timeout is checked */
        SPDO_CheckRxTimeout(dw_Ct); // for thiscase, dont check timeout
#endif

        /* release frame */
        SHNF_ReleaseEplsFrame();
    }
    /* no else : no EPLS frame was received */

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
}
void ProcessApplicationData(){
	//DATA_LOGGER1("ProcessData[0] Tx Mapping on 0x6000/1 is %d\n\n", ab_0_act_ProcessData[0]);
	//DATA_LOGGER1("ProcessData[1] Rx Mapping on 0x6000/2 is %d\n\n", ab_0_act_ProcessData[1]);
}

/*******************************************************************************
 **
 ** Function    : PrintSnState
 **
 ** Description : This function prints the state of the SN.
 **
 ** Parameters  : e_snState (IN) - actual state of the SN
 **
 ** Returnvalue : -
 **
 *******************************************************************************/
static void PrintSnState(SNMTS_t_SN_STATE_MAIN e_snState)
{
    switch (e_snState)
    {
    case SNMTS_k_ST_INITIALIZATION:
    {
        DATA_LOGGER("Actual SN state : INITIALIZATION\n\n");
        break;
    }
    case SNMTS_k_ST_PRE_OPERATIONAL:
    {
        DATA_LOGGER("Actual SN state : PRE-OPERATIONAL\n\n");
        break;
    }
    case SNMTS_k_ST_OPERATIONAL:
    {
        DATA_LOGGER("Actual SN state : OPERATIONAL\n\n");
        break;
    }
    default:
    {
        DATA_LOGGER("FATAL ERROR in PrintSnState()");
        SAPL_Exit();
    }
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
}


/*******************************************************************************
 **
 ** Function    : OutputHeader
 **
 ** Description : This function prints the demo header.
 **
 ** Parameters  : -
 **
 ** Returnvalue : -
 **
 *******************************************************************************/
static void OutputHeader(void)
{
    struct in_addr adapterAddress, broadcast;
    char * adapterName;

    adapterName = (char *)malloc ( k_MAX_RECEIVING_BUFFER * sizeof(char));
    memset(adapterName, (int) 0, (size_t)k_MAX_RECEIVING_BUFFER * sizeof(char));

    //AdapterOnNetwork(k_MAX_RECEIVING_BUFFER * sizeof(char), &adapterName, &adapterAddress, &broadcast);

    DATA_LOGGER  ( "\n\topenSAFETY UDP Demo \n\n ");
    DATA_LOGGER1 ( "\tStack version: \t\t%s\n", EPLS_k_STACK_VERSION );
    DATA_LOGGER1 ( "\tConfiguration: \t\t%s\n", EPLS_k_CONFIGURATION_STRING );
    DATA_LOGGER1 ( "\tTarget System: \t\t%s\n", EPLS_k_TARGET_STRING );
    //DATA_LOGGER1 ( "\tTarget SADDR: \t\t0x%03X\n\n", b_OWN_SADR );
    DATA_LOGGER1 ( "\tNetwork Adapter: \t%s\n", adapterName );
    DATA_LOGGER1 ( "\tNetwork Address: \t%s\n", inet_ntoa(adapterAddress) );
    DATA_LOGGER1 ( "\tNetwork Broadcast: \t%s\n\n", inet_ntoa(broadcast) );
    DATA_LOGGER  ("********************************************************************\n");

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
}


