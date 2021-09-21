/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOinit.c
 *
 * This file contains the initialization functions.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SPDOinit.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>22.03.2011</td><td>Hans Pill</td><td>A&P257950 changes for different SPDO numbers of main instance and SDG instance</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
 *
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "EPLScfgCheck.h"

#include "SCFMapi.h"

#include "SODapi.h"
#include "SOD.h"

#include "SERRapi.h"
#include "SERR.h"
#include "SPDOerr.h"

#include "SPDOint.h"

static BOOLEAN InitTxSpdo(BYTE_B_INSTNUM_ UINT16 *pw_noTxSpdo);
static BOOLEAN TxMappCommInit(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx,
                              UINT16 w_txCommIdx, UINT16 w_txMappIdx);

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  static BOOLEAN InitRxSpdo(BYTE_B_INSTNUM_ UINT16 *pw_noRxSpdo);
  static BOOLEAN RxMappCommInit(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx,
                                UINT16 w_rxCommIdx, UINT16 w_rxMappIdx);
#endif

  /**
   * @brief This function initializes the number of the Tx SPDOs and Rx SPDOs.
   *
   * This function initializes the number of the Tx SPDOs and Rx SPDOs defined in the SOD and calls the SPDO unit initialization functions.
   *
   * @param        b_instNum           instance number (not checked, checked in SSC_InitAll())
   * 	valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
   *
   * @retval          pw_noTxSpdo         pointer to the number of the Tx SPDOs defined in the SOD.
   * 	(pointer not checked, only called with reference to variable in SPDO_Init()) valid range: <> NULL
   *
   * @retval          pw_noRxSpdo         pointer to the number of the Rx SPDOs defined in the SOD.
   * 	(pointer not checked, only called with reference to variable in SPDO_Init()) valid range: <> NULL
   *
   * @return
   * - TRUE              - success
   * - FALSE             - failure
   */
BOOLEAN SPDO_InitAll(BYTE_B_INSTNUM_ UINT16 *pw_noTxSpdo, UINT16 *pw_noRxSpdo)
{
  BOOLEAN o_ret = FALSE; /* return value */

  SPDO_InitTxSm(B_INSTNUM);
  SPDO_InitTxMapp(B_INSTNUM);
  #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    SPDO_InitRxMapp(B_INSTNUM);
    SPDO_InitRxSyncCons(B_INSTNUM);
  #endif

  SPDO_InitAssign(B_INSTNUM);

  /* if the initialization of the Tx SPDOs was successful */
  if (InitTxSpdo(B_INSTNUM_ pw_noTxSpdo))
  {
    #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
      /* Initialization of the Rx SPDOs */
      o_ret = InitRxSpdo(B_INSTNUM_ pw_noRxSpdo);
    #else
      pw_noRxSpdo = pw_noRxSpdo;
      o_ret = TRUE;
    #endif
  }
  /* no else the initialization of the Tx SPDOs failed */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/***
*    static functions
***/
/**
* @brief This function checks if Tx SPDO mapping parameter exist for every Tx SPDO communication parameter
* and Tx SPDO communication parameter exists for every Tx SPDO mapping  parameter.
*
* This function checks if Tx SPDO mapping parameter exist for every Tx SPDO communication parameter
* and Tx SPDO communication parameter exists for every Tx SPDO mapping  parameter in the SOD. Additionally it
* checks if there is no gap between the  SPDO indexes and calls initialization function  for every Tx SPDO.
*
* @param        b_instNum           instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @retval          pw_noTxSpdo         pointer to the number of the Tx SPDOs defined in the SOD.
* 	(pointer not checked, only called with reference to variable in SPDO_Init()) valid range: <> NULL
*
* @return
* - TRUE              - success
* - FALSE             - failure
*/
static BOOLEAN InitTxSpdo(BYTE_B_INSTNUM_ UINT16 *pw_noTxSpdo)
{
  BOOLEAN o_ret = FALSE; /* return value */
  BOOLEAN o_break = FALSE; /* break flag */
  UINT16 w_commIdx; /* counter for the SPDO Communication parameter index */
  UINT16 w_mappIdx; /* counter for the SPDO Mapping parameter index */
  BOOLEAN o_spdoGap = FALSE; /* flag to signal a gap in the SPDO list */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */
  SOD_t_ACS_OBJECT_VIRT s_objAcs; /* structure to access the SOD */

  *pw_noTxSpdo = 0U;
  w_commIdx = k_TX_COMM_START_IDX;
  w_mappIdx = k_TX_MAPP_START_IDX;

  /* Tx SPDOs are checked */
  do
  {
    s_objAcs.s_obj.w_index = w_commIdx;
    s_objAcs.s_obj.b_subIndex = 0U;
    s_objAcs.dw_segOfs = 0;
    s_objAcs.dw_segSize = 0;
    /* if the SPDO communication parameter with this index exists */
    if (SOD_AttrGetVirt(B_INSTNUM_ &s_objAcs, &s_errRes) != NULL)
    {
      /* if there is a gap in the SPDO parameter object definition */
      if (o_spdoGap)
      {
        SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TX_SPDO_GAP, (UINT32)w_commIdx);
        o_break = TRUE;
      }
      /* else : there is no gap */
      else
      {
        /* if the function call failed */
        if (!TxMappCommInit(B_INSTNUM_ (*pw_noTxSpdo), w_commIdx, w_mappIdx))
        {
          o_break = TRUE;
        }
        /* else the function call succeeded */
        else
        {
          /* set the next Tx SPDO */
          (*pw_noTxSpdo)++;
        }
      }
    }
    /* else the SPDO communication parameter with this index does not exist */
    else
    {
      o_spdoGap = TRUE;

      s_objAcs.s_obj.w_index = w_mappIdx;
      s_objAcs.s_obj.b_subIndex = 0U;
      s_objAcs.dw_segOfs = 0;
      s_objAcs.dw_segSize = 0;
      /* if SPDO mapping parameter does not exist for this SPDO comm.
            parameter */
      if (SOD_AttrGetVirt(B_INSTNUM_ &s_objAcs, &s_errRes) != NULL)
      {
        SERR_SetError(B_INSTNUM_ SPDO_k_ERR_NO_TX_COMM_PARA,
                      (((UINT32)(w_commIdx))<<16) + w_mappIdx);

        o_break = TRUE;
      }
    }

    w_commIdx++;
    w_mappIdx++;
  } /* while not all index were checked or no error happened */
  while ((w_commIdx <= k_TX_COMM_END_IDX) && (!o_break));

  /* if the loop finished successfully */
  if (!o_break)
  {
    /* if there is no Tx SPDO defined */
    if ((*pw_noTxSpdo) == 0U)
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_MAN_TX_SPDO, SERR_k_NO_ADD_INFO);
    }
    /* else Tx SPDO is defined */
    else
    {
      o_ret = TRUE;
    }
  }
  /* no else : the loop did not finish successfully */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/***
*
*
* @brief This function initializes the pointers to the communication and mapping parameters.
*
* @param        b_instNum           instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        w_txSpdoIdx         index in the internal array of the Tx
*                                   SPDO structures (checked) valid range: 0..<SPDO_cfg_MAX_NO_TX_SPDO-1>
* @param        w_txCommIdx         Tx SPDO Communication parameter index in the SOD
* 	(not checked, checked in InitTxSpdo()) valid range: k_TX_COMM_START_IDX..k_TX_COMM_END_IDX
* @param        w_txMappIdx         Tx SPDO Mapping parameter index in the SOD
* 	(not checked, checked in InitTxSpdo()) valid range: k_TX_MAPP_START_IDX..k_TX_MAPP_END_IDX
*
* @return
* - TRUE              - success
* - FALSE             - failed
*/
static BOOLEAN TxMappCommInit(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx,
                              UINT16 w_txCommIdx, UINT16 w_txMappIdx)
{
  BOOLEAN o_ret = FALSE; /* return value */
  void *pv_data; /* pointer to the data in the SOD */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */
  SOD_t_ACS_OBJECT_VIRT s_objAcs; /* SOD access structure */

#if EPLS_cfg_MAX_INSTANCES > 1 /* if more instances are configured */
  /* if tx SPDO index is too high */
  if (((B_INSTNUM == 0) &&
	   (w_txSpdoIdx >= (UINT16)SPDO_cfg_MAX_NO_TX_SPDO)) ||
	  ((B_INSTNUM != 0) &&
	   (w_txSpdoIdx >= (UINT16)SPDO_cfg_MAX_NO_TX_SPDO_SDG)))
  {
    SERR_SetError(B_INSTNUM_ SPDO_k_ERR_NO_TX_SPDOS,
                  ((((UINT32)(w_txCommIdx))<<16) | w_txMappIdx));
  }
#else /* else only one instance is configured */
  /* if tx SPDO index is too high */
  if (w_txSpdoIdx >= (UINT16)SPDO_cfg_MAX_NO_TX_SPDO)
  {
    SERR_SetError(B_INSTNUM_ SPDO_k_ERR_NO_TX_SPDOS,
                  ((((UINT32)(w_txCommIdx))<<16) | w_txMappIdx));
  }
#endif
  /* else if initialization of the SPDO communication parameter failed */
  else if (!SPDO_SetPtrToTxCommPara(B_INSTNUM_ w_txSpdoIdx, w_txCommIdx))
  {
    /* error has already been signaled */
  }
  /* else initialization of the SPDO communication parameter succeeded */
  else
  {
    s_objAcs.s_obj.w_index = w_txMappIdx;
    s_objAcs.s_obj.b_subIndex = 0U;
    s_objAcs.dw_segOfs = 0;
    s_objAcs.dw_segSize = 0;

    /* SPDO mapping parameter does not exist for this SPDO comm. parameter */
    if (NULL == SOD_AttrGetVirt(B_INSTNUM_ &s_objAcs, &s_errRes))
    {
      SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
    }
    /* else if BEFORE WRITE flag is not set for the number of entries */
    else if (EPLS_IS_BIT_RESET(s_objAcs.s_obj.s_attr.w_attr, SOD_k_ATTR_BEF_WR))
    {
      SERR_SetError(B_INSTNUM_ SPDO_k_ERR_BEF_WR_TX_SUB_0, (UINT32)w_txMappIdx);
    }
    /* else the attribute of the number of entries is right */
    else
    {
      /* the number of mapping entries is read */
      pv_data = SOD_ReadVirt(B_INSTNUM_ &s_objAcs, &s_errRes);

      #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE)
        #pragma CTC SKIP
      #endif
      /* if read access failed */
      if (pv_data == NULL)
      {
        SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                      (UINT32)(s_errRes.e_abortCode));
      }
      else /* read access succeeded */
      #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE)
        #pragma CTC ENDSKIP
      #endif
      {
        /* Initialization of the Tx SPDO mapping */
        o_ret = SPDO_SetTxMapp(B_INSTNUM_ w_txSpdoIdx, w_txMappIdx,
                               (*((UINT8 *)(pv_data))));
      }
    }
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  /**
  * @brief This function checks if Rx SPDO mapping parameter exist for every Rx SPDO communication parameter and
  * Rx SPDO communication parameter exists for every Rx SPDO mapping parameter in the SOD.
  *
  * Additionally it checks if there is no gap between the SPDO indexes and calls initialization function for every Tx SPDO.
  *
  * @param        b_instNum           instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
  *
  * @retval          pw_noRxSpdo         pointer to the number of the Rx SPDOs defined in the SOD
  * 	(pointer not checked, only called with reference to variable in SPDO_Init()) valid range: <> NULL
  *
  * @return
  * - TRUE                  - success
  * - FALSE                 - failure
  */
  static BOOLEAN InitRxSpdo(BYTE_B_INSTNUM_ UINT16 *pw_noRxSpdo)
  {
    BOOLEAN o_ret = FALSE; /* return value */
    BOOLEAN o_break = FALSE; /* break flag */
    UINT16 w_commIdx; /* counter for the SPDO Communication parameter index */
    UINT16 w_mappIdx; /* counter for the SPDO Mapping parameter index */
    BOOLEAN o_spdoGap = FALSE; /* flag to signal a gap in the SPDO list */
    SOD_t_ERROR_RESULT s_errRes; /* SOD access error */
    SOD_t_ACS_OBJECT_VIRT s_objAcs; /* structure to access the SOD */

    *pw_noRxSpdo = 0U;
    w_commIdx = k_RX_COMM_START_IDX;
    w_mappIdx = k_RX_MAPP_START_IDX;

    /* Rx SPDOs are checked */
    do
    {
      s_objAcs.s_obj.w_index = w_commIdx;
      s_objAcs.s_obj.b_subIndex = 0;
      s_objAcs.dw_segOfs = 0;
      s_objAcs.dw_segSize = 0;
      /* if the SPDO communication parameter with this index exists */
      if (SOD_AttrGetVirt(B_INSTNUM_ &s_objAcs, &s_errRes) != NULL)
      {
        /* if there is a gap in the SPDO parameter object definition */
        if (o_spdoGap)
        {
          SERR_SetError(B_INSTNUM_ SPDO_k_ERR_RX_SPDO_GAP, (UINT32)w_commIdx);
          o_break = TRUE;
        }
        /* else : there is no gap */
        else
        {
          /* if the function call failed */
          if (!RxMappCommInit(B_INSTNUM_ (*pw_noRxSpdo), w_commIdx, w_mappIdx))
          {
            o_break = TRUE;
          }
          /* else the function call succeeded */
          else
          {
            /* set the next Rx SPDO */
            (*pw_noRxSpdo)++;
          }
        }
      }
      /* else the SPDO comm. parameter with this index does not exist */
      else
      {
        o_spdoGap = TRUE;
        s_objAcs.s_obj.w_index = w_mappIdx;
        s_objAcs.s_obj.b_subIndex = 0;
        s_objAcs.dw_segOfs = 0;
        s_objAcs.dw_segSize = 0;

        /* if SPDO mapping parameter exists for this SPDO comm. parameter */
        if (SOD_AttrGetVirt(B_INSTNUM_ &s_objAcs, &s_errRes) != NULL)
        {
          SERR_SetError(B_INSTNUM_ SPDO_k_ERR_NO_RX_COMM_PARA,
                        (((UINT32)(w_commIdx))<<16) + w_mappIdx);
          o_break = TRUE;
        }
      }

      w_commIdx++;
      w_mappIdx++;
    } /* while not all index were checked or no error happened */
    while ((w_commIdx <= k_RX_COMM_END_IDX) && (!o_break));

    /* if the loop finished successfully */
    if (!o_break)
    {
      o_ret = TRUE;
    }
    /* no else : the loop did not finish successfully */

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return o_ret;
  }
#endif

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  /**
  * @brief This function initializes the pointers to the communication and mapping parameters.
  *
  * @param        b_instNum          instance number (not checked, checked in SSC_InitAll()),
  * 	valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
  * @param        w_rxSpdoIdx        index in the internal array of the Rx SPDO structures (checked)
  * 	valid range: 0..<SPDO_cfg_MAX_NO_TX_SPDO-1>
  * @param        w_rxCommIdx        Rx SPDO Communication parameter index in the SOD
  * 	(not checked, checked in InitRxSpdo()) valid range: k_RX_COMM_START_IDX..k_RX_COMM_END_IDX
  * @param        w_rxMappIdx        Rx SPDO Mapping parameter index in the SOD (not checked,
  * 	checked in InitRxSpdo()) valid range: k_RX_MAPP_START_IDX..k_RX_MAPP_END_IDX
  *
  * @return
  * - TRUE             - success
  * - FALSE            - failed
  */
  static BOOLEAN RxMappCommInit(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx,
                                UINT16 w_rxCommIdx, UINT16 w_rxMappIdx)
  {
    BOOLEAN o_ret = FALSE; /* return value */
    void *pv_data; /* pointer to the data in the SOD */
    SOD_t_ERROR_RESULT s_errRes; /* SOD access error */
    SOD_t_ACS_OBJECT_VIRT s_objAcs; /* structure to access the SOD */

#if EPLS_cfg_MAX_INSTANCES > 1 /* if more instances are configured */
    /* if rx SPDO index is too high */
    if (((B_INSTNUM == 0) &&
    	 (w_rxSpdoIdx >= (UINT16)SPDO_cfg_MAX_NO_RX_SPDO)) ||
    	((B_INSTNUM != 0) &&
    	 (w_rxSpdoIdx >= (UINT16)SPDO_cfg_MAX_NO_RX_SPDO_SDG)))
    {
        SERR_SetError(B_INSTNUM_ SPDO_k_ERR_NO_RX_SPDOS,
                      ((((UINT32)(w_rxCommIdx))<<16) | w_rxMappIdx));
    }
#else /* else only one instance is configured */
    /* if rx SPDO index is too high */
    if (w_rxSpdoIdx >= (UINT16)SPDO_cfg_MAX_NO_RX_SPDO)
    {
      SERR_SetError(B_INSTNUM_ SPDO_k_ERR_NO_RX_SPDOS,
                    ((((UINT32)(w_rxCommIdx))<<16) | w_rxMappIdx));
    }
#endif
    /* else if initialization of the SPDO communication parameter failed */
    else if (!SPDO_SetPtrToRxCommPara(B_INSTNUM_ w_rxSpdoIdx, w_rxCommIdx))
    {
      /* error has already been signaled */
    }
    /* else initialization of the SPDO communication parameter succeeded */
    else
    {
      s_objAcs.s_obj.w_index = w_rxMappIdx;
      s_objAcs.s_obj.b_subIndex = 0;
      s_objAcs.dw_segOfs = 0;
      s_objAcs.dw_segSize = 0;
      /* if SPDO mapp. parameter does not exist for this SPDO Comm. parameter */
      if (NULL == SOD_AttrGetVirt(B_INSTNUM_ &s_objAcs,&s_errRes))
      {
        SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                      (UINT32)(s_errRes.e_abortCode));
      }
      /* else if BEFORE WRITE flag is not set for the number of entries */
      else if (EPLS_IS_BIT_RESET(s_objAcs.s_obj.s_attr.w_attr, SOD_k_ATTR_BEF_WR))
      {
        SERR_SetError(B_INSTNUM_ SPDO_k_ERR_BEF_WR_RX_SUB_0,
                      (UINT32)w_rxMappIdx);
      }
      /* else the attribute of the number of entries is right */
      else
      {
        /* the number of mapping entries is read */
        pv_data = SOD_ReadVirt(B_INSTNUM_ &s_objAcs, &s_errRes);

        #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE)
          #pragma CTC SKIP
        #endif
        /* if read access failed */
        if (pv_data == NULL)
        {
          SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                        (UINT32)(s_errRes.e_abortCode));
        }
        #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE)
          #pragma CTC ENDSKIP
        #endif
        else /* read access succeeded */
        {
          /* Initialization of the Rx SPDO mapping */
          o_ret = SPDO_SetRxMapp(B_INSTNUM_ w_rxSpdoIdx, w_rxMappIdx,
                                (*((UINT8 *)(pv_data))));
        }
      }
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
    return o_ret;
  }
#endif

/** @} */
