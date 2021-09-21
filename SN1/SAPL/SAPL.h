/**
 * \file UDP_SN/SAPL/SAPL.h
 * \addtogroup SAPL_UDP_SN
*/
/******************************************************************************
** Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
**                      IXXAT Automation GmbH
** All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
**
** This source code is free software; you can redistribute it and/or modify it
** under the terms of the BSD license (according to License.txt).
********************************************************************************
**
**   Workfile: SAPL.h
**    Summary: SAPLdemo - Safety application Demonstration
**             SAPL, application of the EPLsafety Stack
**             Header file for the application.
**
**     Author: M. Molnar
**
********************************************************************************
********************************************************************************
**
**  Functions: SAPL_InitTarget
**             SAPL_KbHit
**             SAPL_Exit
**             SAPL_CalculateCRC
**             SAPL_StoreSOD
**             SAPL_ssdocAccess
**             SAPL_SOD_ParameterSet_CLBK
**
**    Remarks:
**
*******************************************************************************/

#ifndef SAPL_H
#define SAPL_H

/*******************************************************************************
**    constants and macros
*******************************************************************************/
/** SAPL_k_MAX_PARAM_SET_LEN:
    Maximum length of the Parameter set.
*/
#define SAPL_k_MAX_PARAM_SET_LEN       100UL

/** SAPL_k_ACT_PARAM_SET_LEN:
    Actual length of the Parameter set.
*/
#define SAPL_k_ACT_PARAM_SET_LEN       100UL


/** SAPL_cfg_PARAM_DOWNLOAD:
    Define to enable or disable the parameter download.
    allowed values: EPLS_k_ENABLE, EPLS_k_DISABLE
*/
#define SAPL_cfg_PARAM_DOWNLOAD    EPLS_k_ENABLE

/** SAPL_cfg_CT_BASIS:
    This symbol represents the basis time for the internal timer.
    allowed values: see TIM_t_TIME_BASE enum
*/
#define SAPL_cfg_CT_BASIS k_openSAFETY_TIMEBASE_1MS /* this value has to be used for this
                                          windows demo */

/** SAPL_k_NO_CRCS:
    This symbol represents the number of the CRCs in the parameter checksum
    object (0x1018/0x06).
*/
#define SAPL_k_NO_CRCS    1

/** SAPL_t_PARAM_CHKSUM_DOM:
    This structure definition is used to realize the parameter checksum object
    (0x1018/0x06).
*/
typedef struct
{
  UINT32 dw_timeStamp;              /* time stamp */
  UINT32 adw_crcs[SAPL_k_NO_CRCS];  /* SOD CRCs */
} SAPL_t_PARAM_CHKSUM_DOM;

/*******************************************************************************
**    data types
*******************************************************************************/

/*******************************************************************************
**    global variables
*******************************************************************************/

/*******************************************************************************
**    function prototypes
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
BOOLEAN SAPL_InitTarget(void);

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
void SAPL_KbHit(void);

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
void SAPL_Exit(void);

/******************************************************************************
**
** Function    : SAPL_CalculateCRC
**
** Description : This function calculates the CRC after the function
**               SAPL_SNMTS_CalcParamChkSumClbk() was called.
**               The calculated CRC is passed by the function
**               SNMTS_PassParamChkSumValid() to the SNMTS.
**
** Parameters  : B_INSTNUM (IN) - instance number
**
** Returnvalue : -
**
******************************************************************************/
void SAPL_CalculateCRC(BYTE_B_INSTNUM);

/******************************************************************************
**
** Function    : SAPL_StoreSOD
**
** Description : This function stores the SOD into the non-volatile memory
**               after the function SAPL_SNMTS_SwitchToOpReqClbk() was called.
**               If the store process was successful then operational state
**               will be entered by calling the transition to the function
**               SNMTS_EnterOpState().
**
** Parameters  : B_INSTNUM (IN) - instance number
**
** Returnvalue : -
**
******************************************************************************/
void SAPL_StoreSOD(BYTE_B_INSTNUM);

/*******************************************************************************
**
** Function    : SAPL_ssdocAccess
**
** Description : This function starts SSDOC write and read access to the remote
**               SODs of the SNs.
**
** Parameters  : dw_ct (IN) - consecutive time
**
** Returnvalue : -
**
*******************************************************************************/
void SAPL_ssdocAccess(UINT32 dw_ct);

/*******************************************************************************
**
** Function    : SAPL_SOD_ParameterSet_CLBK
**
** Description : This function is called after the write access of the parameter
**               set object.Parameter set object has SOD index 0x101A
**               sub-index 0x00.
**               **Important:** The function must be provided by the
**               application.
**
** Parameters  : B_INSTNUM (IN)    - instance number (not checked)
**                                   valid range:
**                                   0..(EPLS_cfg_MAX_INSTANCES-1)
**               e_srvc (IN)       - type of service, see {SOD_t_SERVICE}
**                                   (not checked)
**                                   valid range: SOD_t_SERVICE
**               ps_obj (IN)       - pointer to a SOD entry
**                                   (pointer not checked)
**               pv_data (IN)      - pointer to data to be written, in case of
**                                   SOD_k_SRV_BEFORE_WRITE, otherwise NULL
**                                   (pointer not checked)
**               dw_offset (IN)    - start offset in bytes of the segment
**                                   within the data block
**                                   (not used)
**               dw_size (IN)      - size in bytes of the segment
**                                   (not used)
**               pe_abortCode (IN) - abort code for the SSDO
**                                   (pointer not checked)
**
** Returnvalue : TRUE              - success
**               FALSE             - failure
**
*******************************************************************************/
BOOLEAN SAPL_SOD_ParameterSet_CLBK(BYTE_B_INSTNUM_ SOD_t_SERVICE e_srvc,
                                   const SOD_t_OBJECT *ps_obj,
                                   const void *pv_data,
                                   UINT32 dw_offset, UINT32 dw_size,
                                   SOD_t_ABORT_CODES *pe_abortCode);

void PrintApplicationData();
#endif
