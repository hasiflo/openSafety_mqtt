/**
 * @addtogroup SSC
 * @{
 *
 * @file SSCmain.c
 *
 * This file contains functions for initializing the openSAFETY Stack.
*             It also provides a function to return an error string.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 *
 * <h2>History for SSCmain.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 * </table>
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#include "EPLScfgCheck.h"

#if(EPLS_cfg_SCM == EPLS_k_ENABLE)
  #include "SCM.h"
  #include "SNMTM.h"
  #include "SSDOC.h"
#endif

#include "SERR.h"

#include "SCFMapi.h"
#include "SCFM.h"
#include "SHNF.h"

#include "SODapi.h"
#include "SOD.h"
#include "SFS.h"
#include "SDN.h"
/* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
#include "SPDO.h"
#endif /* (0 == SPDO_cfg_MAX_NO_RX_SPDO) */
#include "SNMTS.h"


#include "SSDOS.h"

#include "SSCapi.h"
#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
  #include "SERRapi.h"
  #include "SSCerr.h"
#endif
#include "SSCint.h"


#ifndef EPLS_SW_CONFIG_CHECKED
  #error EPLS Software Configuration is not checked!!!
#endif

static BOOLEAN initInst(BYTE_B_INSTNUM);


/**
 * @brief This function initializes the whole openSAFETY Stack with all of its instances.
 *
 * Each unit of the stack is affected. All finite state machines and all errors are reset.
 *
 * @attention The openSAFETY Stack uses functionality of the SHNF, thus at very first the
 * unit SHNF has to be initialized! This function has to be called to initialize the openSAFETY
 * Stack before any other openSAFETY Stack function is called.
 *
 * @return
 * - TRUE                 - initialization succeeded
 * - FALSE                - initialization failed,
 * @attention  If initialization failed, then the application must immediately switch to FAIL SAFE
 * state, because in some cases a minor error (no fail safe error) is reported.
 */
BOOLEAN SSC_InitAll(void)
{
  BOOLEAN o_return = TRUE;  /* predefined return value */
#if (EPLS_cfg_MAX_INSTANCES > 1)
  UINT8 b_instNum = 0x00u;   /* loop counter */
#endif


  /* initialization of unit SERR */
  SERR_Init();

  /* initialization of unit SCFM succeeded */
  SCFM_Init();

  /* initialization of SSC FSM */
  SSC_InitFsm();

#if (EPLS_cfg_MAX_INSTANCES > 1)
  /* init units of specified number of instances */
  do
  {
#endif
    /* initialization of an instance */
    o_return = initInst(B_INSTNUM);

#if (EPLS_cfg_MAX_INSTANCES > 1)
    b_instNum++;
  }
  while((b_instNum < (UINT8)EPLS_cfg_MAX_INSTANCES) && o_return);
#endif

#if(EPLS_cfg_SCM == EPLS_k_ENABLE)
  /* if NO error occurred during initialization until now */
  if(o_return)
  {
    /* if initialization of unit SSDOC failed */
    if(!(SSDOC_Init()))
    {
      /* error is already reported */
      o_return = FALSE;
    }
    /* else if initialization of unit SNMT Master failed */
    else if(!(SNMTM_Init()))
    {
      /* error is already reported */
      o_return = FALSE;
    }
    else  /* initialization of unit SNMT Master succeeded */
    {
      /* initialization of unit SCM */
      SCM_Init();
    }
  }
  /* else error occurred during init of multi instance units,
    return with o_return = FALSE */
#endif

  SCFM_TACK_PATH();
  return o_return;
}

#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param              w_errorCode        error number
*
* @param              dw_addInfo         additional error information
*
* @retval              pac_str            empty buffer to build the error string dynamically in case of using add.
*       error info without interpretation of its value.
*/
  void SSC_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str)
  {
    /* choose the matching error string */
    switch(w_errorCode)
    {
      case SSC_k_ERR_PROC_INST_INV:
      {
        SPRINTF2(pac_str,
                "%#x - SSC_k_ERR_PROC_INST_INV: SSC_ProcessSNMTSSDOFrame():\n"
                "The delivered instance (%02lu) is bigger than "
                "<EPLS_cfg_MAX_INSTANCES>!\n",
                SSC_k_ERR_PROC_INST_INV, dw_addInfo);
        break;
      }
      case SSC_k_ERR_REF_MNGTFRMS_INV:
      {
        SPRINTF1(pac_str,
                "%#x - SSC_k_ERR_REF_MNGTFRMS_INV: SSC_BuildSNMTSSDOFrames():\n"
                "Pointer to number of free management frames refers to "
                "NULL!\n", SSC_k_ERR_REF_MNGTFRMS_INV);
        break;
      }
      case SSC_k_ERR_RX_TELTYP_INV:
      {
        SPRINTF2(pac_str,
                "%#x - SSC_k_ERR_RX_TELTYP_INV: processStateReqProc():\n"
                "Telegram type (%02lu) of received openSAFETY frame is not "
                "defined!\n", SSC_k_ERR_RX_TELTYP_INV, dw_addInfo);
        break;
      }
      case SSC_k_ERR_NO_SSDO_CLIENT:
      {
        SPRINTF2(pac_str,
                "%#x - SSC_k_ERR_NO_SSDO_CLIENT: processStateReqProc():\n"
                "No SCM present, not able to process SSDO Service Request "
                "with ID (%02lu)!\n", SSC_k_ERR_NO_SSDO_CLIENT,
                dw_addInfo);
        break;
      }
      case SSC_k_ERR_NO_SNMT_MASTER:
      {
        SPRINTF2(pac_str,
                "%#x - SSC_k_ERR_NO_SNMT_MASTER: processStateReqProc():\n"
                "No SCM present, not able to process SNMT Service Request "
                "with ID (%02lu)!\n", SSC_k_ERR_NO_SNMT_MASTER,
                dw_addInfo);
        break;
      }
      case SSC_k_ERR_NO_RXFRM_STACK_RDY:
      {
        SPRINTF1(pac_str,
                "%#x - SSC_k_ERR_NO_RXFRM_STACK_RDY: processStateDeSer():\n"
                "Stack called without openSAFETY frame to process!\n",
                SSC_k_ERR_NO_RXFRM_STACK_RDY);
        break;
      }
      case SSC_k_ERR_FRAME_LEN:
      {
        SPRINTF2(pac_str,
                "%#x - SSC_k_ERR_FRAME_LEN: processStateDeSer():\n"
                "The length of the received frame (%lu) is invalid.\n",
                SSC_k_ERR_FRAME_LEN, dw_addInfo);
        break;
      }
      case SSC_k_ERR_FRM_PROV_FRM_IN_PROC:
      {
        SPRINTF1(pac_str,
                "%#x - SSC_k_ERR_FRM_PROV_FRM_IN_PROC: checkFuncParamSt234():\n"
                "New openSAFETY frame cannot be processed, Stack is busy!\n",
                SSC_k_ERR_FRM_PROV_FRM_IN_PROC);
        break;
      }
      case SSC_k_ERR_PROC_STATE_INV:
      {
        SPRINTF2(pac_str,
                "%#x - SSC_k_ERR_PROC_STATE_INV: SSC_ProcessSNMTSSDOFrame():\n"
                "The SSC FSM switched into an undefined state (%02lu)!\n",
                SSC_k_ERR_PROC_STATE_INV, dw_addInfo);
        break;
      }
      default:
      {
        SPRINTF(pac_str, "Unknown error code was generated by the SSC\n");
      }
    }
    return;
  }
#endif /* (EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE) */


/**
* @brief This function initializes the specified instance.
*
* All finite state machines and all errors of the specified instance are reset. The function is also used
* by the function SSC_InitAll().
*
* @see          SSC_InitAll()
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @return
* - TRUE           - initialization succeeded
* - FALSE          - initialization failed
*/
static BOOLEAN initInst(BYTE_B_INSTNUM)
{
  BOOLEAN o_return = FALSE;  /* pre-initialized return value */


  /* if initialization of unit SOD succeeded */
  if(SOD_Init(B_INSTNUM))
  {
    /* if initialization of unit SFS succeeded */
    if (SFS_Init(B_INSTNUM))
    {
      /* if initialization of Unit SDN succeeded
        NOTE: unit SDN must be initialized AFTER initialization of unit SOD */
      if(SDN_Init(B_INSTNUM))
      {
        /* if initialization of unit SNMTS succeeded */
        if(SNMTS_Init(B_INSTNUM))
        {
/* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
          /* if initialization of unit SPDO succeeded */
          if(SPDO_Init(B_INSTNUM))
          {
#endif /* (0 == SPDO_cfg_MAX_NO_RX_SPDO) */
            /* if initialization of unit SSDOS succeeded */
            if(SSDOS_Init(B_INSTNUM))
            {
              o_return = TRUE;
            }
            /* else initialization of unit SSDOS failed,
              return value FALSE is predefined */
/* no SPDO active at all */
#if (0 != SPDO_cfg_MAX_NO_TX_SPDO)
          }
          /* else initialization of unit SPDO failed,
            return value FALSE is predefined */
#endif /* (0 == SPDO_cfg_MAX_NO_RX_SPDO) */
        }
        /* else initialization of unit SNMTS failed,
          return value FALSE is predefined */
      }
      /* else initialization of unit SDN failed,
        return value FALSE is predefined */
    }
    /* else initialization of unit SFS failed,
        return value FALSE is predefined */
  }
  /* else initialization of unit SOD failed,
     return value FALSE is predefined */
  SCFM_TACK_PATH();
  return o_return;
}

/** @} */
