/******************************************************************************
 ** Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
 **                      IXXAT Automation GmbH
 ** All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 **
 ** This source code is free software; you can redistribute it and/or modify it
 ** under the terms of the BSD license (according to License.txt).
 ********************************************************************************
 **
 **   Workfile: SAPLscmmain.c
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
 **             GetExpTimeStamp
 **             WriteParameterSet
 **             InitParamSet
 **             SpdoLoopbackInit
 **             SpdoLoopbackDataChk
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
#include "EPLStypes.h"

#include "SERRapi.h"

#include "SCMapi.h"
#include "SSDOCapi.h"
#include "SNMTMapi.h"

#include "SSCapi.h"
#include "SNMTSapi.h"
#include "SODapi.h"
#include "SPDOapi.h"
#include "SCFMapi.h"
#include "SHNF.h"

#include "datalogger.h"

#include "Time.h"
#include "SAPL.h"
#include "MQTT.h"
#include "SHNF_Definitions.h"

#include "datalogger.h"


#include "MessageHandler.h"

#include "SAPL.h"

#include "MQTT.h"

#ifndef EPLS_SW_CONFIG_CHECKED
#error EPLS Software Configuration is not checked!!!
#endif


/** dw_Ct:
    Variable to store the consecutive time.
 */
static UINT32 dw_Ct = 0UL;

/** e_LastSnState:
    Variable to store the last SN state.
 */
static SNMTS_t_SN_STATE_MAIN e_LastSnState;

//for printing the Application Output
static UINT32 printDelay= 10000000UL;
static UINT32 printCnt = 0L;


/** k_SINGLE_INST_NUM:
    This symbol represents instance number 0. Only one instance is used for
    the demo application although the EPLS Stack is encoded as multi instance
 */
#if (EPLS_cfg_MAX_INSTANCES > 1)
#define k_SINGLE_INST_NUM     (UINT8)0
#define k_SINGLE_INST_NUM_     k_SINGLE_INST_NUM,
#else
#define k_SINGLE_INST_NUM_
#endif


/** index, sub-index of mappad object in the TxSPDO:
    These symbols represent the index and sub-index of the mandatory transmit
    process data object (TxSPDO) in the SOD. The configuration of the Basic
    Safety Node arranges only one TxSPDO.
 */
#define k_MAPPED_TXSPDO_OBJ_IDX    0x6000U
#define k_MAPPED_TXSPDO_OBJ_SUBIDX 0x01U

/** index, sub-index of RxSPDO:
    These symbols represent the index and sub-index of the mandatory receive
    process data object (RxSPDO) in the SOD. The configuration of the Basic
    Safety Node arranges only one RxSPDO.
 */
#define k_MAPPED_RXSPDO_OBJ_IDX    0x6000U
#define k_MAPPED_RXSPDO_OBJ_SUBIDX 0x02U

#define SCM_DEBUG 1 //for debugging SCM

/* Variable definition for the SOD access */
static PTR_TYPE  dw_TxSpdoHdl;   /* SOD handle of TxSPDO data */
static PTR_TYPE  dw_RxSpdoHdl;   /* SOD handle of RxSPDO data */
static BOOLEAN o_TxSpdoAppObj; /* flag to signal access to an application
                                  object */
static BOOLEAN o_RxSpdoAppObj; /* flag to signal access to an application
                                  object */

/*******************************************************************************
 **    static function-prototypes
 *******************************************************************************/
static BOOLEAN InitCt(void);
static BOOLEAN InitEPLS(void);

static void Build(void);
static void Process(void);

static void PrintSnState(SNMTS_t_SN_STATE_MAIN e_snState);
static void OutputHeader(void);

static UINT8 *GetExpTimeStamp(UINT16 w_idx);
static BOOLEAN WriteParameterSet(UINT16 w_idx, UINT8 b_crc1, UINT8 b_crc2,
        const UINT8 *pb_timeStamp);
static BOOLEAN InitParamSet(void);
static void SpdoLoopbackInit(void);
static void SpdoLoopbackDataChk(void);


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
        SHNF_Init(k_SINGLE_INST_NUM_ SAPL_k_SCM_SN1_SADR);

        /* if the initialization of the EPLS stack was successful */
        if (InitEPLS())
        {
            /* After the EPLS stack initialization, the application may copy data from
         non volatile memory to the SOD
         E.g.: Consecutive time is initialized */
            if (InitCt())
            {
                /* After the EPLS stack initialization, the application may copy data from
           non volatile memory to the SOD
           E.g.: Parameter set is initialized */
                if (InitParamSet())
                {
                    /* if the SCM was activated successfully */
                    if (SCM_Activate())
                    {
                        /* no action defined */
                    }
                    /* no else : fail safe error happened */


                    /* EPLS application main while loop */
                    while(o_break)
                    {
                        /* The consecutive time value must be the same on both safety
               controller and the same value during one application loop */
                        dw_Ct = SHNFTime_getTime(SAPL_cfg_CT_BASIS);

                        /* calculation of the CRC */
                        SAPL_CalculateCRC(k_SINGLE_INST_NUM);

                        /* store of the SOD */
                        SAPL_StoreSOD(k_SINGLE_INST_NUM);

                        Build();

                        Process();

                        if(printCnt >= printDelay){
                        	PrintApplicationData();
                        	printCnt = 0;
                        }else{
                        	printCnt++;
                        }


                        /* if the actual SN state update succeeded */
                        if (SNMTS_GetSnState(k_SINGLE_INST_NUM_ &e_actSnState))
                        {
                            /* if the SN state changed */
                            if (e_actSnState != e_LastSnState)
                            {
                                /* print the actual SN state */
                                PrintSnState(e_actSnState);
                                //sendState(e_actSnState);
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
    if (SOD_AttrGet(k_SINGLE_INST_NUM_ 0x1200U, 0x03U, &dw_sodHdl, &o_appObj,
            &s_errRes) != NULL)
    {
        o_ret = SOD_Write(k_SINGLE_INST_NUM_ dw_sodHdl, o_appObj, &c_ct, FALSE,
                0UL, 0UL);
    }
    else /* SOD access error */
    {
        SAPL_SERR_SignalErrorClbk(k_SINGLE_INST_NUM_ s_errRes.w_errorCode,
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
 ** Description : This function initializes the EPLS stack, gets the attributes
 **               to the process data objects and switches the SN to
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
    BOOLEAN o_ret = FALSE;  /* return value */

    /* if initialization of the EPLS Stack failed */
    if(!SSC_InitAll())
    {
        /* the error was already printed in the SAPL_SERR_SignalErrorClbk() */
    }
    /* initialization of the EPLS Stack succeeded */
    else
    {
        /* if the initialisation of the last SN state succeeded */
        if (SNMTS_GetSnState(k_SINGLE_INST_NUM_ &e_LastSnState))
        {
            PrintSnState(e_LastSnState);

            SpdoLoopbackInit();

            /* transition to PreOperational */
            o_ret = SNMTS_PerformTransPreOp(k_SINGLE_INST_NUM_ dw_Ct);
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
 ** Description : This function calls the SPDO_CheckTimeout function to check
 **               timeouts and the SSC_BuildTxFrames function to build the EPLS
 **               frames.
 **
 ** Parameters  : -
 **
 ** Returnvalue : TRUE  - success
 **               FALSE - failure
 **
 *******************************************************************************/
static void Build(void)
{
    UINT16 b_numFreeSpdoFrms = 1U; /* number of free SPDO frames can be sent per
                                   call of the SSC_BuildTxFrames */
    UINT8 b_numFreeMngtFrms = 2U; /* number of free management frames can be sent
                                   per call of the SSC_BuildTxFrames */

    /* Guard timeout is checked */
    SNMTS_TimerCheck(k_SINGLE_INST_NUM_ dw_Ct, &b_numFreeMngtFrms);

    /* SPDO frames are built */
    SPDO_BuildTxSpdo(k_SINGLE_INST_NUM_ dw_Ct, &b_numFreeSpdoFrms);

    /* SNMT frame is built */
    SNMTM_BuildRequest(dw_Ct, &b_numFreeMngtFrms);

    /* Trigger SCM */
    SCM_Trigger(dw_Ct, &b_numFreeMngtFrms);

    /* SSDO frame is built */
    SSDOC_BuildRequest(dw_Ct, &b_numFreeMngtFrms);

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
}

/*******************************************************************************
 **
 ** Function    : Process
 **
 ** Description : This function gets a EPLS frame from the SHNF, calls the
 **               SSC_ProcessRxFrame function to process this frame, checks
 **               whether the Tx process data equals to the Rx process data and
 **               in this case increases the Tx process data.
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
            SPDO_ProcessRxSpdo(k_SINGLE_INST_NUM_ dw_Ct, pb_rxFrame, w_frameLength);

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
            /* SCT timeout is checked */
            SPDO_CheckRxTimeout(k_SINGLE_INST_NUM_ dw_Ct);
#endif

            //SpdoLoopbackDataChk();
        }
        /* else other frame was received */
        else
        {
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
            /* SCT timeout is checked */
            SPDO_CheckRxTimeout(k_SINGLE_INST_NUM_ dw_Ct);
#endif


            /* while the frame processing is busy and no fail safe error happened */
            while ((SSC_ProcessSNMTSSDOFrame(k_SINGLE_INST_NUM_ dw_Ct, pb_rxFrame,
                    w_frameLength) == SSC_k_BUSY))
            {
                pb_rxFrame = (UINT8 *)NULL;
                w_frameLength = 0U;
            }
        }

        /* release frame */
        SHNF_ReleaseEplsFrame();
    }
    else /* no EPLS frame was received */
    {
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
        /* SCT timeout is checked */
        SPDO_CheckRxTimeout(k_SINGLE_INST_NUM_ dw_Ct);
#endif
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
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

    DATA_LOGGER ( "\n\topenSAFETY UDP Demo \n\n ");
    DATA_LOGGER1 ( "\tStack version: \t\t%s\n", EPLS_k_STACK_VERSION );
    DATA_LOGGER1 ( "\tConfiguration: \t\t%s\n", EPLS_k_CONFIGURATION_STRING );
    DATA_LOGGER1 ( "\tTarget System: \t\t%s\n", EPLS_k_TARGET_STRING );
    DATA_LOGGER1 ( "\tTarget SADDR: \t\t0x%03X\n\n", b_OWN_SADR );
    DATA_LOGGER1 ( "\tNetwork Adapter: \t%s\n", adapterName );
    DATA_LOGGER1 ( "\tNetwork Address: \t%s\n", inet_ntoa(adapterAddress) );
    DATA_LOGGER1 ( "\tNetwork Broadcast: \t%s\n\n", inet_ntoa(broadcast) );
    DATA_LOGGER ("********************************************************************\n");

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
}

/*******************************************************************************
 **
 ** Function    : GetExpTimeStamp
 **
 ** Description : This function reads the time stamp object from the SOD.
 **
 ** Parameters  : w_idx (IN) - SOD index in the SADR-DVI list.
 **
 ** Returnvalue : <> NULL    - pointer to the time stamp in the SOD
 **               == NULL    - failure
 **
 *******************************************************************************/
static UINT8 *GetExpTimeStamp(UINT16 w_idx)
{
    UINT8 *pb_timeStamp = (UINT8 *)NULL; /* return value
                                          (pointer to the time stamp) */
    PTR_TYPE dw_timeStampHdl; /* handle for the time stamp */
    BOOLEAN o_appObj;       /* flag to signal access to an application object */
    SOD_t_ERROR_RESULT s_errRes; /* SOD access error */


    /* if the time stamp object is available */
    if (SOD_AttrGet(k_SINGLE_INST_NUM_ w_idx, 0x07U, &dw_timeStampHdl, &o_appObj,
            &s_errRes) != NULL)
    {
        pb_timeStamp = (UINT8 *)SOD_Read(k_SINGLE_INST_NUM_ dw_timeStampHdl,
                o_appObj, 0UL, 0UL, &s_errRes);

        /* if read access failed */
        if (pb_timeStamp == NULL)
        {
            SAPL_SERR_SignalErrorClbk(k_SINGLE_INST_NUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
        }
    }
    else /* SOD access error */
    {
        SAPL_SERR_SignalErrorClbk(k_SINGLE_INST_NUM_ s_errRes.w_errorCode,
                (UINT32)(s_errRes.e_abortCode));
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return pb_timeStamp;
}

/*******************************************************************************
 **
 ** Function    : WriteParameterSet
 **
 ** Description : This function writes the parameter set in the SOD.
 **               The first two bytes are initialized for two CRC object, the
 **               following four bytes are used for the timestamp and the
 **               remaining bytes are initialized but unused.
 **
 ** Parameters  : w_idx (IN)        - SOD index in the SADR-DVI list.
 **               b_crc1 (IN)       - value of the 1st CRC object which will be
 **                                   written into the parameter set
 **               b_crc2 (IN)       - value of the 2nd CRC object which will be
 **                                   written into the parameter set
 **               pb_timeStamp (IN) - pointer to the time stamp value which will
 **                                   be written into the parameter set
 **
 ** Returnvalue : TRUE  - success
 **               FALSE - failure
 **
 *******************************************************************************/
static BOOLEAN WriteParameterSet(UINT16 w_idx, UINT8 b_crc1, UINT8 b_crc2,
        const UINT8 *pb_timeStamp)
{
    BOOLEAN o_ret = FALSE; /* return value */
    PTR_TYPE dw_paramSetHdl; /* handle for the parameter set */
    BOOLEAN o_appObj;      /* flag to signal access to an application object */
    UINT8 ab_paramSetBuffer[SAPL_k_MAX_PARAM_SET_LEN]; /* parameter set buffer */
    UINT32 i; /* loop counter */
    SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

    /* Parameter set is initialized */
    for (i = 0UL; i < SAPL_k_MAX_PARAM_SET_LEN; i++)
    {
        ab_paramSetBuffer[i] = (UINT8)i;
    }

    ab_paramSetBuffer[0] = b_crc1; /* 1st CRC object */
    ab_paramSetBuffer[1] = b_crc2; /* 2nd CRC object */

    /*
     ** Parameter checksum
     ** ==================
     **
     ** ATTENTION:
     **     The application must adapt the initialization of the parameter
     ** checksum on the SNs!!!
     **     The parameter checksum consist of the timestamp and the SOD CRCs
     ** created by the configuration tool. These values can be directly transferred
     ** by the configuration tool to SNs (e.g. via Serial Port) or indirectly by
     ** the SCM within the parameter set during the parameter download.
     **     In this implementation, the expected timestamp and the expected SOD CRC
     ** are set within the parameter set to demonstrate the functionality.
     **
     */

    /* Time Stamp */
    ab_paramSetBuffer[2] = pb_timeStamp[0];
    ab_paramSetBuffer[3] = pb_timeStamp[1];
    ab_paramSetBuffer[4] = pb_timeStamp[2];
    ab_paramSetBuffer[5] = pb_timeStamp[3];

    /* SOD CRC */
    ab_paramSetBuffer[6] = 0xFF;
    ab_paramSetBuffer[7] = 0xFF;
    ab_paramSetBuffer[8] = 0xFF;
    ab_paramSetBuffer[9] = 0xFF;

    /* if the parameter set is available */
    if (SOD_AttrGet(k_SINGLE_INST_NUM_ w_idx, 0x0BU, &dw_paramSetHdl, &o_appObj,
            &s_errRes) != NULL)
    {
        /* if the actual length of the object was set successfully */
        if (SOD_ActualLenSet(k_SINGLE_INST_NUM_ dw_paramSetHdl, o_appObj,
                SAPL_k_ACT_PARAM_SET_LEN))
        {
            o_ret = SOD_Write(k_SINGLE_INST_NUM_ dw_paramSetHdl, FALSE,
                    ab_paramSetBuffer, FALSE, 0UL, 0UL);
        }
        /* no else : The error has already been signaled */
    }
    else /* SOD access error */
    {
        SAPL_SERR_SignalErrorClbk(k_SINGLE_INST_NUM_ s_errRes.w_errorCode,
                (UINT32)(s_errRes.e_abortCode));
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return o_ret;
}

/*******************************************************************************
 **
 ** Function    : InitParamSet
 **
 ** Description : This function initializes the parameter set in the SOD.
 **               The parameter set is written for the SCM SN and the Standard
 **               SN.
 **
 ** Parameters  : -
 **
 ** Returnvalue : TRUE  - success
 **               FALSE - failure
 **
 *******************************************************************************/
static BOOLEAN InitParamSet(void)
{
    BOOLEAN o_ret = FALSE;  /* return value */
    UINT8 *pb_timeStamp; /* pointer to the time stamp */

    /* get time stamp value */
    pb_timeStamp = GetExpTimeStamp(0xC400U);

    /* if the read access was OK */
    if (pb_timeStamp != NULL)
    {
        /* if the parameter set was written successfully for the SCM SN*/
        if (WriteParameterSet(0xC400U, 0x12U, 0x34U, pb_timeStamp))
        {
            pb_timeStamp = GetExpTimeStamp(0xC401U);

            /* if the read access was OK */
            if (pb_timeStamp != NULL)
            {
                /* the parameter set is written for the Standard SN */
                o_ret = WriteParameterSet(0xC401U, 0x56U, 0x78U, pb_timeStamp);
            }
            /* no else : The error has already been signaled */
        }
        /* no else : The error has already been signaled */
    }
    /* no else : The error has already been signaled */

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return o_ret;
}

/*******************************************************************************
 **
 ** Function    : SpdoLoopbackInit
 **
 ** Description : This function initialized the variables for the SPDO loop-back
 **               application.
 **
 ** Parameters  : -
 **
 ** Returnvalue : -
 **
 *******************************************************************************/
static void SpdoLoopbackInit(void)
{
    const SOD_t_ATTR *ps_txSpdoAttr; /* reference to the attributes of the
                                      TxSPDO data */
    const SOD_t_ATTR *ps_rxSpdoAttr; /* reference to the attributes of the
                                      RxSPDO data */
    SOD_t_ERROR_RESULT s_errRes; /* SOD access error */


    /* get attributes and handle of object mapped into the RxSPDO */
    ps_rxSpdoAttr = SOD_AttrGet(k_SINGLE_INST_NUM_ k_MAPPED_RXSPDO_OBJ_IDX,
            k_MAPPED_RXSPDO_OBJ_SUBIDX, &dw_RxSpdoHdl,
            &o_RxSpdoAppObj, &s_errRes);

    /* if no attribute is available */
    if (ps_rxSpdoAttr == NULL)
    {
        SAPL_SERR_SignalErrorClbk(k_SINGLE_INST_NUM_ s_errRes.w_errorCode,
                (UINT32)(s_errRes.e_abortCode));
    }
    /* else attribute is available */
    else
    {
        /* get attributes and handle of object mapped into the TxSPDO */
        ps_txSpdoAttr = SOD_AttrGet(k_SINGLE_INST_NUM_ k_MAPPED_TXSPDO_OBJ_IDX,
                k_MAPPED_TXSPDO_OBJ_SUBIDX,
                &dw_TxSpdoHdl, &o_TxSpdoAppObj, &s_errRes);

        /* if no attribute is available */
        if (ps_txSpdoAttr == NULL)
        {
            SAPL_SERR_SignalErrorClbk(k_SINGLE_INST_NUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
        }
        /* no else : attribute is available */
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
}

/*******************************************************************************
 **
 ** Function    : SpdoLoopbackDataChk
 **
 ** Description : This function checks and prints the loop-back SPDO data.
 **
 ** Parameters  : -
 **
 ** Returnvalue : -
 **
 *******************************************************************************/
static void SpdoLoopbackDataChk(void)
{
    static UINT8 b_txTestData = 0x01U; /* data to transmit via TxSPDO */
    UINT8 *pb_rxTestData; /* pointer to the data received via RxSPDO */
    SOD_t_ERROR_RESULT s_errRes; /* SOD access error */


    /* read the object mapped into Rx SPDO */
    pb_rxTestData = (UINT8 *)SOD_Read(k_SINGLE_INST_NUM_ dw_RxSpdoHdl,
            o_RxSpdoAppObj, 0UL, 0UL, &s_errRes);

    /* if read access failed */
    if(pb_rxTestData == NULL)
    {
        SAPL_SERR_SignalErrorClbk(k_SINGLE_INST_NUM_ s_errRes.w_errorCode,
                (UINT32)(s_errRes.e_abortCode));
    }
    /* else read access succeeded */
    else
    {
        /* compare data of RxSPDO and TxSPDO */
        if (*pb_rxTestData == b_txTestData)
        {
            DATA_LOGGER1("\nTx data was received correctly : 0x%x\n",
                    b_txTestData);
            b_txTestData++;

            /* SSDOC access request is started */
            SAPL_ssdocAccess(dw_Ct);

            /* if write access failed */
            if(!SOD_Write(k_SINGLE_INST_NUM_ dw_TxSpdoHdl, o_TxSpdoAppObj,
                    &b_txTestData, TRUE, 0UL, 0UL))
            {
                /* the error was already printed in the
           SAPL_SERR_SignalErrorClbk() */
            }
            /* no else write access succeeded */
        }
        /* no else : expected Tx SPDO data was not received yet */
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
}



