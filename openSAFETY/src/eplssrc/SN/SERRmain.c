/**
 * @addtogroup SERR
 * @{
 *
 * @file SERRmain.c
 *
 * Main implementation for the SERR error interface
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 *
 * <h2>History for SERRmain.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>15.03.2013</td><td>Hans Pill</td><td>added statistic counters</td></tr>
 *     <tr><td>07.06.2013</td><td>Hans Pill</td><td>added check for SPDO index related to the instance number</td></tr>
 *     <tr><td>09.07.2013</td><td>Roman Zwischelsberger</td><td>Lint</td></tr>
 *     <tr><td>31.07.2013</td><td>Hans Pill</td><td>adapted for modules without RxSPDO e.g. SCM only</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>06.10.2014</td><td>Hans Pill</td><td>non SCM nodes considered</td></tr>
 * </table>
 *
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#include "SERRapi.h"
#include "SERR.h"
#include "SCFMapi.h"

#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
#if(EPLS_cfg_SCM == EPLS_k_ENABLE)
#include "SCMerr.h"
#include "SNMTMerr.h"
#include "SSDOCerr.h"
#endif

#include "SSCerr.h"
#include "SFSerr.h"
#include "SNMTSerr.h"
#include "SDNerr.h"
#include "SODerr.h"
#include "SPDOerr.h"
#include "SSDOSerr.h"
#endif

#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
 * This symbol represents the maximum string length of the description of an
 * error including the null termination '\0'.
 */
#define SERR_k_ERROR_STR_LEN       2000u

/**
 * This array is module global and stores a zero terminated error string of &lt;SERR_k_ERROR_STR_LEN&gt; characters.
 *
 * The array is used for error strings that contains variable additional error information like instance.
 * The composition of the error string is processed in this array.
 */
static CHAR ac_DynErrorStr[SERR_k_ERROR_STR_LEN] SAFE_NO_INIT_SEKTOR;
#endif /* (EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE) */

/**
 * @var dw_AddErrorInfo
 * This module global variable stores the additional error information of the
 * last stack internal error that has occurred.
 */
static UINT32 dw_AddErrorInfo SAFE_NO_INIT_SEKTOR;

/**
 * @var SERR_aadwCommonEvtCtr
 * This module global variable array contains the common statistic error counters.
 */
UINT32 SERR_aadwCommonEvtCtr[EPLS_cfg_MAX_INSTANCES][SERR_k_NO_COMMON_CTR] SAFE_NO_INIT_SEKTOR;

#if (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE)
#if (0 != SPDO_cfg_MAX_NO_RX_SPDO)
/**
 * Event counters for one RxSPDO.
 */
typedef struct
{
    UINT32 adw_spdoEvtCtr[SERR_k_NO_SPDO_CTR];
} SERR_t_SPDO_EVT_CTR;

/**
 * @var ao_spdoEvtCtr
 * This module global variable array contains the spdo specific statistic error counters.
 */
SERR_t_SPDO_EVT_CTR ao_spdoEvtCtr[(EPLS_cfg_MAX_INSTANCES - 1)*SPDO_cfg_MAX_NO_RX_SPDO_SDG + SPDO_cfg_MAX_NO_RX_SPDO] SAFE_NO_INIT_SEKTOR;
/**
 * @var apo_SpdoEvtCtr
 * This module global pointer array contains the pointer to the first SPDO counter of each instance.
 */
SERR_t_SPDO_EVT_CTR* apo_SpdoEvtCtr[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;
#endif /* (0 != SPDO_cfg_MAX_NO_RX_SPDO) */

/**
 * @var aadw_AcycEvtCtr
 * This module global variable array contains the acyclic statistic error counters.
 */
#if (SCM_cfg_MAX_NUM_OF_NODES == EPLS_k_NOT_APPLICABLE)
UINT32 aadw_AcycEvtCtr[1][SERR_k_NO_COMMON_CTR] SAFE_NO_INIT_SEKTOR;
#else /* (SCM_cfg_MAX_NUM_OF_NODES == EPLS_k_NOT_APPLICABLE) */
UINT32 aadw_AcycEvtCtr[SCM_cfg_MAX_NUM_OF_NODES][SERR_k_NO_COMMON_CTR] SAFE_NO_INIT_SEKTOR;
#endif /* (SCM_cfg_MAX_NUM_OF_NODES == EPLS_k_NOT_APPLICABLE) */
#endif /* (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE) */

/**
 * @brief This function initializes all module global and global variables defined in the unit SERR.
 */
void SERR_Init(void)
{
#if (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE)
#if (0 != SPDO_cfg_MAX_NO_RX_SPDO)
    UINT32 i;
#endif /* (0 != SPDO_cfg_MAX_NO_RX_SPDO) */
#endif
    /* reset module global error code and additional info */
    dw_AddErrorInfo = 0x00UL;
#if (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE)
#if (0 != SPDO_cfg_MAX_NO_RX_SPDO)
    /* initialize the spdo specific error counters */
    /* first instance */
    apo_SpdoEvtCtr[0] = &ao_spdoEvtCtr[0];
    /* initialize all other instances */
  for ( i = 1; i < EPLS_cfg_MAX_INSTANCES; i++)
  {
        /* i starts with one  therefore subtracting one never produces an overflow */
	  apo_SpdoEvtCtr[i] = &ao_spdoEvtCtr[SPDO_cfg_MAX_NO_RX_SPDO + ((i-1) * SPDO_cfg_MAX_NO_RX_SPDO_SDG)];
    }
#endif /* (0 != SPDO_cfg_MAX_NO_RX_SPDO) */
#endif /* (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE) */
    SCFM_TACK_PATH();
}

/**
* @brief This function stores an internal error that occurred inside the EPLS Stack and calls the application callback
* function SAPL_SERR_SignalErrorClbk.
*
* @see         SERR_GetLastAddInfo()
*
* @param       b_instNum          instance number (not checked, checked by the API), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        w_error           error that occurred in the EPLS stack (not checked, only called with define),
*                                 valid range: any UINT16 value
*
* @param        dw_addInfo        additional error information (not checked, any value allowed) valid range: any UINT32 value
*/
void SERR_SetError(BYTE_B_INSTNUM_ UINT16 w_error, UINT32 dw_addInfo)
{
    /* store the last additional error information */
    dw_AddErrorInfo = dw_addInfo;

    /* report error to application */
    SAPL_SERR_SignalErrorClbk(B_INSTNUM_ w_error, dw_addInfo);
    SCFM_TACK_PATH();
}

/**
* @brief This function returns the last additional error info that occurred in the openSAFETY stack that was set by SERR_SetError.
* This function may be called immediately after a stack internal function does return with FALSE. Otherwise a previous non
* relevant additional error info (the last info) is returned.
*
* @see          SERR_SetError()
*
* @return       last additional error info
*/
UINT32 SERR_GetLastAddInfo(void)
{
    SCFM_TACK_PATH();
    return dw_AddErrorInfo;
}

#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
 * @brief This function returns a reference to an error string which describes the error exactly. Every unit stores its error strings and returns a reference to them if requested.
 *
 * @param       w_errorCode         16 bit value that holds Error Class, Error ID and the error itselves (checked), valid range: any 16 bit value
 *
 * @param       dw_addInfo          additional error information (not checked, any value allowed), valid range: any 32 bit value
 *
 * @return       <> NULL           - pointer to error string
 */
CHAR * SERR_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo)
{
    /* distribute the error to the assigned unit */
    switch(SERR_GET_UNIT_ID(w_errorCode))
    {
        case SSC_k_UNIT_ID:
        {
            SSC_GetErrorStr(w_errorCode, dw_addInfo, &ac_DynErrorStr[0]);
            break;
        }
        case SFS_k_UNIT_ID:
        {
            SFS_GetErrorStr(w_errorCode, dw_addInfo, &ac_DynErrorStr[0]);
            break;
        }
        case SNMTS_k_UNIT_ID:
        {
            SNMTS_GetErrorStr(w_errorCode, dw_addInfo, &ac_DynErrorStr[0]);
            break;
        }

#if(EPLS_cfg_SCM == EPLS_k_ENABLE)
        case SCM_k_UNIT_ID:
        {
            SCM_GetErrorStr(w_errorCode, dw_addInfo, &ac_DynErrorStr[0]);
            break;
        }
        case SNMTM_k_UNIT_ID:
        {
            SNMTM_GetErrorStr(w_errorCode, dw_addInfo, &ac_DynErrorStr[0]);
            break;
        }
        case SSDOC_k_UNIT_ID:
        {
            SSDOC_GetErrorStr(w_errorCode, dw_addInfo, &ac_DynErrorStr[0]);
            break;
        }

#endif
        case SPDO_k_UNIT_ID:
        {
            SPDO_GetErrorStr(w_errorCode, dw_addInfo, &ac_DynErrorStr[0]);
            break;
        }
        case SSDOS_k_UNIT_ID:
        {
            SSDOS_GetErrorStr(w_errorCode, dw_addInfo, &ac_DynErrorStr[0]);
            break;
        }
        case SOD_k_UNIT_ID:
        {
            SOD_GetErrorStr(w_errorCode, dw_addInfo, &ac_DynErrorStr[0]);
            break;
        }
        case SDN_k_UNIT_ID:
        {
            SDN_GetErrorStr(w_errorCode, dw_addInfo, &ac_DynErrorStr[0]);
            break;
        }
        default:
        {
            SPRINTF(&ac_DynErrorStr[0], "Unknown unit ID");
        }
    }
    SCFM_TACK_PATH();
    return &ac_DynErrorStr[0];
}
#endif

/**
 * @brief This function increases the statistic common error counter.
 *
 * @param       b_instNum          instance number (not checked, checked by the API), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param       e_evt              statistic event which needs to be counted
 */
void SERR_CountCommonEvt(BYTE_B_INSTNUM_ SERR_t_STATEVT_COMMON const e_evt)
{
    SERR_aadwCommonEvtCtr[B_INSTNUMidx][e_evt]++;
    return;
}

/**
 * @brief This function increases the statistic SPDO error counter.
 *
 * @param       b_instNum          instance number (not checked, checked by the API), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param       w_spdoIdx        Rx SPDO index (not checked, checked in SPDO_CheckRxTimeout() or SPDO_BuildTxSpdo() or ProcessTReq() or ProcessTRes() or SPDO_ProcessRxSpdo() and SPDO_GetSpdoIdxForSAdr()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param       e_evt              statistic event which needs to be counted
 */
void SERR_CountSpdoEvt(BYTE_B_INSTNUM_ UINT16 const w_spdoIdx, SERR_t_STATEVT_SPDO const e_evt)
{
#if (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE)
#if (0 != SPDO_cfg_MAX_NO_RX_SPDO)
    SERR_t_SPDO_EVT_CTR* po_spdoEvtCtr = NULL;
    po_spdoEvtCtr = apo_SpdoEvtCtr[B_INSTNUMidx] + w_spdoIdx;
    po_spdoEvtCtr->adw_spdoEvtCtr[e_evt]++;
#endif /* (0 != SPDO_cfg_MAX_NO_RX_SPDO) */
#endif /* (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE) */
    SERR_aadwCommonEvtCtr[B_INSTNUMidx][SERR_k_CYC_ERROR]++;
    return;
}
/**
 * @brief This function increases the statistic acyclic error counter.
 *
 * @param       w_fsmNum           finite state machine number, assigned internally (not checked, checked in SNMTM_BuildRequest()), valid range:	0 .. <SNMTM_cfg_MAX_NUM_FSM-1>
 *
 * @param       e_evt              statistic event which needs to be counted
 */
void SERR_CountAcycEvt(UINT16 const w_fsmNum, SERR_t_STATEVT_ACYC const e_evt)
{
#if (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE)
    aadw_AcycEvtCtr[w_fsmNum][e_evt]++;
#endif /* (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE) */
    /* those errors only occur at instance 0 at the moment */
    SERR_aadwCommonEvtCtr[0][SERR_k_ACYC_RETRY]++;
    return;
}
/**
 *    static functions
 **/

#if (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE)
#if (0 != SPDO_cfg_MAX_NO_RX_SPDO)

/**
 * @brief This returns whether a specific SPDO statistic counter value is available and in case it is it also returns its value.
 *
 * @param       b_instNum          instance number, valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param       w_spdoIdx        Rx SPDO index, valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param       e_evt              statistic event counter which is requested
 *
 * @param       pdw_statCtr        pointer where to write the statistic counter value to
 *
 * @return
 * - TRUE the SPDO statistic counter is available
 * - FALSE the counter is not available
 */
BOOLEAN SERR_GetSpdoStatCtr(BYTE_B_INSTNUM_ UINT16 const w_spdoIdx, SERR_t_STATEVT_SPDO const e_evt,UINT32 * const pdw_statCtr)
{
    SERR_t_SPDO_EVT_CTR* po_spdoEvtCtr = NULL;
    BOOLEAN b_RetVal = FALSE;
    /* check parameters */
  if ((B_INSTNUMidx < (UINT8)EPLS_cfg_MAX_INSTANCES) &&
      ((UINT32)SERR_k_NO_SPDO_CTR > (UINT32)e_evt) &&
      (NULL != pdw_statCtr) &&
      (((SPDO_cfg_MAX_NO_RX_SPDO > w_spdoIdx) &&
    	(0 == B_INSTNUMidx)) ||
       ((SPDO_cfg_MAX_NO_RX_SPDO_SDG > w_spdoIdx) &&
    	(0 != B_INSTNUMidx))))
  {
        /* only read the statistic counter value if the parameters are checked */
        po_spdoEvtCtr = apo_SpdoEvtCtr[B_INSTNUMidx] + w_spdoIdx;
        *pdw_statCtr = po_spdoEvtCtr->adw_spdoEvtCtr[e_evt];
        b_RetVal = TRUE;
    }
    return b_RetVal;
}
#endif /* (0 != SPDO_cfg_MAX_NO_RX_SPDO) */

/**
 * @brief This returns whether a specific acyclic statistic counter value is available and in case it is it also returns its value.
 *
 * @param       us_snIdx           SN index, valid range: 0..(SCM_cfg_MAX_NUM_OF_NODES-1)
 *
 * @param       e_evt              statistic event counter which is requested
 *
 * @param       pdw_statCtr        pointer where to write the statistic counter value to
 *
 * @return
 * - TRUE the acyclic counter is available
 * - FALSE the counter is not available
 */
BOOLEAN SERR_GetAcycStatCtr(UINT16 const us_snIdx, SERR_t_STATEVT_ACYC const e_evt,UINT32 * const pdw_statCtr)
{
    BOOLEAN b_RetVal = FALSE;
    /* check parameters */
  if ((SCM_cfg_MAX_NUM_OF_NODES > us_snIdx) &&
      ((UINT32)SERR_k_NO_ACYC_CTR > (UINT32)e_evt) &&
      (NULL != pdw_statCtr))
  {
        *pdw_statCtr = aadw_AcycEvtCtr[us_snIdx][e_evt];
        b_RetVal = TRUE;
    }
    return b_RetVal;
}

#endif /* (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE) */
/**
 * @brief This returns whether a specific common statistic counter value is available and in case it is it also returns its value.
 *
 * @param       b_instNum          instance number, valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param       e_evt              statistic event counter which is requested
 *
 * @param       pdw_statCtr        pointer where to write the statistic counter value to
 *
 * @return
 * - TRUE the acyclic counter is available
 * - FALSE the counter is not available
 */
BOOLEAN SERR_GetCommonStatCtr(BYTE_B_INSTNUM_ SERR_t_STATEVT_COMMON const e_evt, UINT32 * const pdw_statCtr)
{
    BOOLEAN b_RetVal = FALSE;
    /* check parameters */
#pragma CTC SKIP
  if ((B_INSTNUMidx < (UINT8)EPLS_cfg_MAX_INSTANCES) &&
      ((UINT32)SERR_k_NO_COMMON_CTR > (UINT32)e_evt) &&
      (NULL != pdw_statCtr))
  {
        *pdw_statCtr = SERR_aadwCommonEvtCtr[B_INSTNUMidx][e_evt];
        b_RetVal = TRUE;
    }
#pragma CTC ENDSKIP
    return b_RetVal;
}

/** @} */
