/*
 * \file UDP_SN\SAPL\SAPLsnClbk.c
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
**   Workfile: SAPLsnClbk.c
**    Summary: SAPLdemo - Safety application Demonstration
**             SAPL, application of the EPLsafety Stack
**             This file contains SNMTS, SOD and SERR callback functions.
**
**     Author: M. Molnar
**
********************************************************************************
********************************************************************************
**
**  Functions: SAPL_SNMTS_ErrorAckClbk
**             SAPL_SNMTS_CalcParamChkSumClbk
**             SAPL_SNMTS_SwitchToOpReqClbk
**             SAPL_SERR_SignalErrorClbk
**             SAPL_CalculateCRC
**             SAPL_StoreSOD
**             SAPL_SOD_ParameterSet_CLBK
**             SAPL_SNMTS_ParameterSetProcessed
**
**             ReadObj
**             WriteObj
**             WriteDomObj
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
#include "../include/UDP.h"
#include "SODapi.h"
#include "SNMTSapi.h"
#include "SERRapi.h"

#include "SAPL.h"

/*******************************************************************************
**    global variables
*******************************************************************************/

/*******************************************************************************
**    static constants, types, macros, variables
*******************************************************************************/
/** APP_DBG_PRINT_ACTIVE:
    If this define is defined then all application printfs will be activated
    otherwise all application printfs are deactivated
*/
#define APP_DBG_PRINT_ACTIVATE

#ifdef APP_DBG_PRINT_ACTIVATE
  #define APP_DBG_PRINTF  DBG_PRINTF
  #define APP_DBG_PRINTF1 DBG_PRINTF1
/*  #define APP_DBG_PRINTF2 DBG_PRINTF2 not referenced */
/*  #define APP_DBG_PRINTF3 DBG_PRINTF3 not referenced */
#else
  #define APP_DBG_PRINTF
  #define APP_DBG_PRINTF1
  #define APP_DBG_PRINTF2
  #define APP_DBG_PRINTF3
#endif

/** o_CalculateCRC:
    Flag to signal the starting of the CRC calculation.
*/
static BOOLEAN o_CalculateCRC = FALSE;

/** o_StoreSOD:
    Flag to signal the starting of the SOD data storing.
*/
static BOOLEAN o_StoreSOD = FALSE;

/*******************************************************************************
**    static function-prototypes
*******************************************************************************/
static void *ReadObj(BYTE_B_INSTNUM_ UINT16 w_idx, UINT8 b_subIdx);
static BOOLEAN WriteObj(BYTE_B_INSTNUM_ UINT16 w_idx, UINT8 b_subIdx,
                        BOOLEAN o_overWrite, const void *pv_data);
static BOOLEAN WriteDomObj(BYTE_B_INSTNUM_ UINT16 w_idx, UINT8 b_subIdx,
                           BOOLEAN o_overWrite, const void *pv_data,
                           UINT32 dw_size);
static UINT32 SAPL_Crc32Calc(UINT32 dw_initCrc, UINT32 dw_sodLen,
                             const void *pv_sod);

/*******************************************************************************
**    global functions
*******************************************************************************/
/*******************************************************************************
**
** Function    : SAPL_SNMTS_ErrorAckClbk
**
** Description : This function is a callback function which is provided by
**               the EPLS Application. The function is called by the EPLS
**               Stack (unit SNMTS) in case of reception of SNMT Service
**               "SNMT SN ack". This callback function passes the acknowledge
**               on a reported error to the SN's application.
**
** Parameters  : B_INSTNUM (IN)    - instance number, valid range:
**                                   0 .. <EPLS_cfg_MAX_INSTANCES> - 1
**               b_errorGroup (IN) - error group see {fail error group}
**                                   (not checked) valid range: any 8 bit value
**               b_errorCode (IN)  - error code see {fail error code}
**                                   (not checked) valid range: any 8 bit value
**
** Returnvalue : -
**
*******************************************************************************/
void SAPL_SNMTS_ErrorAckClbk(BYTE_B_INSTNUM_ UINT8 b_errorGroup,
                             UINT8 b_errorCode)
{
  APP_DBG_PRINTF("\n\nSNMT SN ack received :\n");
  APP_DBG_PRINTF1("    Instance number = %u\n",B_INSTNUMidx);
  APP_DBG_PRINTF1("    Error group = 0x%X\n",b_errorGroup);
  APP_DBG_PRINTF1("    Error code = 0x%X\n\n",b_errorCode);

  /* to avoid PC-lint Info */
  #if (EPLS_cfg_MAX_INSTANCES > 1)
    b_instNum = b_instNum;
  #endif
  b_errorCode = b_errorCode;
  b_errorGroup = b_errorGroup;
}

/*******************************************************************************
**
** Function    : SAPL_SNMTS_CalcParamChkSumClbk
**
** Description : This function is a callback function which is provided by
**               the EPLS Application. The function is called by the EPLS
**               Stack (unit SNMTS) in case of reception of SNMT Service
**               "SN set to OPERATIONAL". This callback function requests
**               the application of the SN to calculate the CRC16 checksum
**               of the current SOD.
**
**               **Attention:**
**
**               To pass the calculated parameter checksum to the SNMTS the
**               application must call the API function
**               SNMTS_PassParamChkSumValid().
**               This API function MUST NOT be called within this callback
**               function.
**
** See Also    : SNMTS_PassParamChkSumValid()
**
** Parameters  : B_INSTNUM (IN) - instance number, valid range:
**                                0 .. <EPLS_cfg_MAX_INSTANCES> - 1
**
** Returnvalue : -
**
*******************************************************************************/
void SAPL_SNMTS_CalcParamChkSumClbk(BYTE_B_INSTNUM)
{
  #if (EPLS_cfg_MAX_INSTANCES > 1)
    b_instNum = b_instNum; /* To avoid warnings */
  #endif
  /* Start the CRC calculation in the main while loop */
  o_CalculateCRC = TRUE;
}

/*******************************************************************************
**
** Function    : SAPL_SNMTS_SwitchToOpReqClbk
**
** Description : This function is a callback function which is provided by
**               the EPLS Application. The function is called by the EPLS
**               Stack (unit SNMTS) to indicate that a SN state transition
**               from PRE-OPERATIONAL to OPERATIONAL was requested by the SCM.
**               To be able to respond the requested state transition the EPLS
**               Application has to admit the transition  by calling the
**               function SNMTS_EnterOpState().
**
**               **Attention:**
**
**               To inform the SNMTS about the API confirmation of switching
**               into state OPERATIONAL application must call the API function
**               SNMTS_EnterOpState(). This API function MUST NOT be called
**               within this callback function.
**
** See Also    : SNMTS_EnterOpState()
**
** Parameters  : B_INSTNUM (IN) - instance number, valid range:
**                                0 .. <EPLS_cfg_MAX_INSTANCES> - 1
**
** Returnvalue : -
**
*******************************************************************************/
void SAPL_SNMTS_SwitchToOpReqClbk(BYTE_B_INSTNUM)
{
  #if (EPLS_cfg_MAX_INSTANCES > 1)
    b_instNum = b_instNum; /* To avoid warnings */
  #endif
  /* Start the SOD store process in the main while loop */
  o_StoreSOD=TRUE;
}

/*******************************************************************************
**
** Function    : SAPL_SERR_SignalErrorClbk
**
** Description : This function is a callback function which is provided by
**               the EPLS Application. The function is called by the EPLS
**               Stack (unit SERR) to signal an internal error of the EPLS
**               Stack.
**
** Parameters  : B_INSTNUM (IN)   - instance number
**               w_errorCode (IN) - error class, unit in which the error
**                                  occured and the error
**               dw_addInfo (IN)  - additional error information
**
** Returnvalue : -
**
*******************************************************************************/
void SAPL_SERR_SignalErrorClbk(BYTE_B_INSTNUM_ UINT16 w_errorCode,
                               UINT32 dw_addInfo)
{
  /* if Minor error happened */
  if (SERR_GET_ERROR_CLASS(w_errorCode) == SERR_CLASS_MINOR)
  {
    #if (EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
      APP_DBG_PRINTF("\n\n!!! Minor error happened !!!\n");
      APP_DBG_PRINTF1("    %s\n", SERR_GetErrorStr(w_errorCode, dw_addInfo));
      APP_DBG_PRINTF1("    Instance number = %u\n\n",B_INSTNUMidx);
    #else
      APP_DBG_PRINTF("\n\n!!! Minor error happened !!!\n");
      APP_DBG_PRINTF1("    Instance number = %u\n",B_INSTNUMidx);
      APP_DBG_PRINTF1("    Error code = 0x%X\n",w_errorCode);
      APP_DBG_PRINTF1("    Additional information = 0x%X\n\n",dw_addInfo);
    #endif
  }
  /* else if information was signaled */
  else if (SERR_GET_ERROR_CLASS(w_errorCode) == SERR_CLASS_INFO)
  {
    #if (EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
      APP_DBG_PRINTF("\n\n!!! Information !!!\n");
      APP_DBG_PRINTF1("    %s\n", SERR_GetErrorStr(w_errorCode, dw_addInfo));
      APP_DBG_PRINTF1("    Instance number = %u\n\n",B_INSTNUMidx);
    #else
      APP_DBG_PRINTF("\n\n!!! Information !!!\n");
      APP_DBG_PRINTF1("    Instance number = %u\n",B_INSTNUMidx);
      APP_DBG_PRINTF1("    Error code = 0x%X\n",w_errorCode);
      APP_DBG_PRINTF1("    Additional information = 0x%X\n\n",dw_addInfo);
    #endif
  }
  /* else FATAL error happened */
  else
  {
    #if (EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
      APP_DBG_PRINTF("\n\n!!! FATAL ERROR HAPPENED !!!\n");
      APP_DBG_PRINTF1("    %s\n", SERR_GetErrorStr(w_errorCode, dw_addInfo));
      APP_DBG_PRINTF1("    Instance number = %u\n\n",B_INSTNUMidx);
    #else
      APP_DBG_PRINTF("\n\n!!! FATAL ERROR HAPPENED !!!\n");
      APP_DBG_PRINTF1("    Instance number = %u\n",B_INSTNUMidx);
      APP_DBG_PRINTF1("    Error code = 0x%X\n",w_errorCode);
      APP_DBG_PRINTF1("    Additional information = 0x%X\n\n",dw_addInfo);
    #endif
    SAPL_Exit();
  }

  /* To avoid PC-lint Info */
#if (EPLS_cfg_MAX_INSTANCES > 1)
  b_instNum = b_instNum;
#endif
  dw_addInfo = dw_addInfo;
}

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
void SAPL_CalculateCRC(BYTE_B_INSTNUM)
{
  UINT8 *pb_crc1Data; /* pointer to the 1st CRC object */
  UINT32 dw_calculatedCRC; /* calculated CRC */
  SAPL_t_PARAM_CHKSUM_DOM *ps_paramChksum;
  UINT32 dw_expectedCRC;   /* expected CRC */
  BOOLEAN o_crcCalcOk = FALSE; /* flag for the result of the CRC calculation */

  /* if the CRC has to be calculated */
  if (o_CalculateCRC)
  {
    /* if SOD was disabled successfully */
    if (SOD_DisableSodWrite(B_INSTNUM))
    {
      /* get pointer to the 1st CRC object */
      pb_crc1Data = (UINT8 *)ReadObj(B_INSTNUM_ 0x2000U, 0x01U);

      /* if the 1st CRC object is available */
      if (pb_crc1Data != NULL)
      {
        /* CRC calculation process begin */

        dw_calculatedCRC = SAPL_Crc32Calc(0U, 2, (void *)pb_crc1Data);

        /* CRC calculation process end */

        /* if SOD was enabled successfully */
        if (SOD_EnableSodWrite(B_INSTNUM))
        {
          o_CalculateCRC = FALSE;

          /* read the expected CRC value from the SOD */
          ps_paramChksum =
              (SAPL_t_PARAM_CHKSUM_DOM *)ReadObj(B_INSTNUM_ 0x1018U, 0x06U);

          /* if the parameter checksum object data is available */
          if (ps_paramChksum != NULL)
          {
            dw_expectedCRC = ps_paramChksum->adw_crcs[0];

            /* if the CRC calculation is OK */
            if (dw_expectedCRC == dw_calculatedCRC)
            {
              o_crcCalcOk = TRUE;
            }

            /* The calculated CRC is passed to the SNMTS */
            if (SNMTS_PassParamChkSumValid(B_INSTNUM_ o_crcCalcOk))
            {
            }
          }
          /* no else : the parameter checksum object data is not available */
        }
        /* no else : the error was already signaled */
      }
      /* no else : the error was already signaled */
    }
    /* no else : SOD has already been disabled, try again later */
  }
  /* no else : CRC has not to be calculated */
}

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
void SAPL_StoreSOD(BYTE_B_INSTNUM)
{
  /* if the new SOD data have to be stored */
  if (o_StoreSOD)
  {
    /* if SOD was disabled successfully */
    if (SOD_DisableSodWrite(B_INSTNUM))
    {
      /* Store process begin */

      /* Store process end */

      /* if SOD was enabled successfully */
      if (SOD_EnableSodWrite(B_INSTNUM))
      {
        /* if transition to the operational state succeeded */
        if (SNMTS_EnterOpState(B_INSTNUM_ TRUE, 0U, 0U))
        {
           /* Transition to Operational state succeeded */
        }
        else /* transition to the operational state failed */
        {
          /* Transition to Operational state failed */
        }

        o_StoreSOD = FALSE;
      }
      /* no else : the error was already signaled */
    }
    /* SOD has already been disabled */
    else
    {
      /* Try again later */
    }
  }
  /* no else : SOD data have not to be stored */
}

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
                                   SOD_t_ABORT_CODES *pe_abortCode)
{
  BOOLEAN o_ret = FALSE; /* return value */

  UINT8 b_noEntries = 1U;
  UINT32 dw_mappEntry1 = 0x60000108UL;
  UINT32 dw_mappEntry2 = 0U;
  void *pv_objData;

  #if (EPLS_cfg_MAX_INSTANCES > 1)
    b_instNum = b_instNum; /* to avoid warnings */
  #endif


  /* to avoid compiler warning */
  dw_offset = dw_offset;
  dw_size = dw_size;
  pv_data = pv_data;
  ps_obj = ps_obj;

  pv_objData = ((SOD_t_ACT_LEN_PTR_DATA *)(ps_obj->pv_objData))->pv_objData;

  /* if "after write" */
  if (e_srvc == SOD_k_SRV_AFTER_WRITE)
  {
    /* if the 1st CRC object write access was not successful */
    if (!WriteObj(B_INSTNUM_ 0x2000U, 0x01U, FALSE, pv_objData))
    {
      *pe_abortCode = SOD_ABT_GENERAL_ERROR;
    }
    /* else if the 2nd CRC object write access was not successful */
    else if (!WriteObj(B_INSTNUM_ 0x2000U, 0x02U, FALSE,
                       ((UINT8 *)(pv_objData)+1)))
    {
      *pe_abortCode = SOD_ABT_GENERAL_ERROR;
    }
    /* else if the time stamp object write access was not successful */
    else if (!WriteObj(B_INSTNUM_ 0x1018U, 0x07U, TRUE,
                        ((UINT8 *)(pv_objData)+2U)))
    {
      *pe_abortCode = SOD_ABT_GENERAL_ERROR;
    }

    /* else if the parameter checksum write access failed */
    else if (!WriteDomObj(B_INSTNUM_ 0x1018U, 0x06U, FALSE,
                          ((UINT8 *)(pv_objData)+2U),
                          sizeof(SAPL_t_PARAM_CHKSUM_DOM)))
    {
      *pe_abortCode = SOD_ABT_GENERAL_ERROR;
    }


    /* else if SPDO */
    else if (!WriteObj(B_INSTNUM_ 0xC000U, 0x00U, TRUE, &b_noEntries))
    {
      *pe_abortCode = SOD_ABT_GENERAL_ERROR;
    }

    /* else if SPDO */
    else if (!WriteObj(B_INSTNUM_ 0xC000U, 0x01U, TRUE, &dw_mappEntry1))
    {
      *pe_abortCode = SOD_ABT_GENERAL_ERROR;
    }
    /* else if SPDO */
    else if (!WriteObj(B_INSTNUM_ 0xC000U, 0x02U, TRUE, &dw_mappEntry2))
    {
      *pe_abortCode = SOD_ABT_GENERAL_ERROR;
    }


    /* else the parameter set was writen */
    else
    {
      APP_DBG_PRINTF ("Parameter set download was finished sucessfully.\n");
      APP_DBG_PRINTF1("    1st CRC object : %#x\n",
                      *((UINT8 *)(pv_objData)));
      APP_DBG_PRINTF1("    2nd CRC object : %#x\n\n",
                      *(((UINT8 *)(pv_objData))+1));
      o_ret = TRUE;
    }
  }

  return o_ret;
}

/*******************************************************************************
**
** Function    : SAPL_SNMTS_ParameterSetProcessed
**
** Description : This function is a callback function which is provided by
**               the EPLS Application. The function is called by the EPLS
**               Stack (unit SNMTS) to check whether the "parameter set"
**               downloaded by the SCM is already processed.
**
** Parameters  : B_INSTNUM (IN) - instance number, valid range:
**                                0 .. <EPLS_cfg_MAX_INSTANCES> - 1
**
** Returnvalue : TRUE           - "parameter set" is already processed
**               FALSE          - "parameter set" is not processed
**
*******************************************************************************/
BOOLEAN SAPL_SNMTS_ParameterSetProcessed(BYTE_B_INSTNUM)
{
  static UINT8 ctr = 2U;

  #if (EPLS_cfg_MAX_INSTANCES > 1)
    b_instNum = b_instNum; /* to avoid warnings */
  #endif

  if (ctr == 0U)
  {
    return TRUE;
  }
  else
  {
    ctr--;
    return FALSE;
  }
}

/*******************************************************************************
**    static functions
*******************************************************************************/
/*******************************************************************************
**
** Function    : ReadObj
**
** Description : This function reads a SOD object.
**
** Parameters  : B_INSTNUM (IN) - instance number
**               w_idx (IN)     - index of the SOD object.
**               b_subIdx (IN)  - sub-index of the SOD object.
**
** Returnvalue : <> NULL    - pointer to the object data in the SOD
**               == NULL    - failure
**
*******************************************************************************/
static void *ReadObj(BYTE_B_INSTNUM_ UINT16 w_idx, UINT8 b_subIdx)
{
  void *pv_objData = NULL; /* return value (pointer to the SOD object data) */
  PTR_TYPE dw_sodHdl; /* handle for the SOD access */
  BOOLEAN o_appObjFlag; /* application object flag */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* if the given SOD object is available */
  if (SOD_AttrGet(B_INSTNUM_ w_idx, b_subIdx, &dw_sodHdl, &o_appObjFlag,
                  &s_errRes) != NULL)
  {
    pv_objData = SOD_Read(B_INSTNUM_ dw_sodHdl, FALSE, 0U, 0U, &s_errRes);

    /* if read access failed */
    if (pv_objData == NULL)
    {
      SAPL_SERR_SignalErrorClbk(B_INSTNUM_ s_errRes.w_errorCode,
                                (UINT32)(s_errRes.e_abortCode));
    }
  }
  else /* SOD access error */
  {
    SAPL_SERR_SignalErrorClbk(B_INSTNUM_ s_errRes.w_errorCode,
                              (UINT32)(s_errRes.e_abortCode));
  }

  return pv_objData;
}

/*******************************************************************************
**
** Function    : WriteObj
**
** Description : This function writes a SOD object.
**
** Parameters  : B_INSTNUM (IN)   - instance number
**               w_idx (IN)       - index of the SOD object.
**               b_subIdx (IN)    - sub-index of the SOD object.
**               o_overWrite (IN) - overwrite flag
**               pv_data (IN)     - pointer to the data to be written
**
** Returnvalue : TRUE             - success
**               FALSE            - failure
**
*******************************************************************************/
static BOOLEAN WriteObj(BYTE_B_INSTNUM_ UINT16 w_idx, UINT8 b_subIdx,
                        BOOLEAN o_overWrite, const void *pv_data)
{
  BOOLEAN o_ret = FALSE; /* return value */
  PTR_TYPE dw_sodHdl; /* handle for the SOD access */
  BOOLEAN o_appObjFlag; /* application object flag */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* if the given SOD object is available */
  if (SOD_AttrGet(B_INSTNUM_ w_idx, b_subIdx, &dw_sodHdl, &o_appObjFlag,
                  &s_errRes) != NULL)
  {
    o_ret = SOD_Write(B_INSTNUM_ dw_sodHdl, o_appObjFlag, pv_data, o_overWrite,
                      0U, 0U);
  }
  else /* SOD access error */
  {
    SAPL_SERR_SignalErrorClbk(B_INSTNUM_ s_errRes.w_errorCode,
                              (UINT32)(s_errRes.e_abortCode));
  }

  return o_ret;
}

/*******************************************************************************
**
** Function    : WriteDomObj
**
** Description : This function writes a SOD domain object.
**
** Parameters  : B_INSTNUM (IN)   - instance number
**               w_idx (IN)       - index of the SOD object.
**               b_subIdx (IN)    - sub-index of the SOD object.
**               o_overWrite (IN) - overwrite flag
**               pv_data (IN)     - pointer to the data to be written
**               dw_size (IN)     - size of the data to be written
**
** Returnvalue : TRUE             - success
**               FALSE            - failure
**
*******************************************************************************/
static BOOLEAN WriteDomObj(BYTE_B_INSTNUM_ UINT16 w_idx, UINT8 b_subIdx,
                           BOOLEAN o_overWrite, const void *pv_data,
                           UINT32 dw_size)
{
  BOOLEAN o_ret = FALSE; /* return value */
  PTR_TYPE dw_sodHdl; /* handle for the SOD access */
  BOOLEAN o_appObjFlag; /* application object flag */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* if the given SOD object is available */
  if (SOD_AttrGet(B_INSTNUM_ w_idx, b_subIdx, &dw_sodHdl, &o_appObjFlag,
                  &s_errRes) != NULL)
  {
    o_ret = SOD_Write(B_INSTNUM_ dw_sodHdl, o_appObjFlag, pv_data, o_overWrite,
                      0U, dw_size);
  }
  else /* SOD access error */
  {
    SAPL_SERR_SignalErrorClbk(B_INSTNUM_ s_errRes.w_errorCode,
                              (UINT32)(s_errRes.e_abortCode));
  }

  return o_ret;
}

/*******************************************************************************
**
** Function    : SAPL_Crc32Calc
**
** Description : This function returns a 32 bit CRC checksum over SOD.
**
** Parameters  : dw_initCrc (IN) - initial CRC (always called with 0 by
**                                 the EPLS software)
**               dw_sodLen (IN)  - length of the SOD
**               pv_sod  (IN)    - pointer to the first CRC object in the SOD
**
** Returnvalue : 0 .. 0xFFFFFFFF - 32 bit CRC check sum
**
*******************************************************************************/
static UINT32 SAPL_Crc32Calc(UINT32 dw_initCrc, UINT32 dw_sodLen,
                             const void *pv_sod)
{
  dw_initCrc = dw_initCrc;
  dw_sodLen  = dw_sodLen;
  pv_sod     = pv_sod;

  return 0xFFFFFFFF;
}

