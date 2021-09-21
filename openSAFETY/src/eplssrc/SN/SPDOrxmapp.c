/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOrxmapp.c
 *
 * This file contains functions for processing the Rx mapping.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SPDOrxmapp.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>21.03.2011</td><td>Hans Pill</td><td>A&P257950 changed variables for better usage of memory</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 *     <tr><td>15.03.2013</td><td>Hans Pill</td><td>added statistic counters</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>06.03.2018</td><td>Stefan Innerhofer</td><td>reset the connection valid bit in SPDO_RxSpdoToSafeState()</td></tr>
 * </table>
 *
 */

#include "EPLScfg.h"

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)

  #include "EPLStarget.h"
  #include "EPLStypes.h"
  #include "EPLScfgCheck.h"

  #include "SCFMapi.h"

  #include "SODapi.h"
  #include "SODerr.h"
  #include "SOD.h"

  #include "SERRapi.h"
  #include "SERR.h"
  #include "SPDOerr.h"

  #include "SPDOapi.h"
  #include "SPDOint.h"

  #include "SHNF.h"

  /* Lint error 767 : (Info -- macro 'xxx' was defined differently in another
                       module) */
  /**
   * @name Rx SPDO specific configuration defines
   * @{
   */
  /* RSM_IGNORE_QUALITY_BEGIN Notice #1 - Physical line length > 80 char. */
  #define SPDO_cfg_MAX_SPDO_MAPP_ENTRIES SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES //lint !e767
  /* RSM_IGNORE_QUALITY_END */
  #define SPDO_cfg_MAX_LEN_OF_SPDO       SPDO_cfg_MAX_LEN_OF_RX_SPDO//lint !e767
  #define SPDO_cfg_MAX_NO_SPDO           SPDO_cfg_MAX_NO_RX_SPDO //lint !e767
  #define SPDO_cfg_MAX_NO_SPDO_SDG       SPDO_cfg_MAX_NO_RX_SPDO_SDG //lint !e767
  /** @} */

  /**
   * @name Rx SPDO specific constants
   * @{
   */
  #define k_MAPP_START_IDX               k_RX_MAPP_START_IDX //lint !e767
  #define SPDO_k_ERR_BEF_WR_SUB_X        SPDO_k_ERR_BEF_WR_RX_SUB_X //lint !e767
  #define SPDO_k_ERR_MAPP_ENTRY_1        SPDO_k_ERR_RX_MAPP_ENTRY_1 //lint !e767
  #define SPDO_k_ERR_TOO_LONG            SPDO_k_ERR_RX_TOO_LONG //lint !e767
  #define SPDO_k_ERR_OBJ_MAPP_1          SPDO_k_ERR_RX_OBJ_MAPP_1 //lint !e767
  #define SPDO_k_ERR_MAPP_ENTRY_3        SPDO_k_ERR_RX_MAPP_ENTRY_3 //lint !e767
  #define SPDO_k_ERR_MAP_OBJ             SPDO_k_ERR_RX_MAP_OBJ //lint !e767
  #define SPDO_k_ERR_SOD_SRV             SPDO_k_ERR_RX_SOD_SRV //lint !e767
  /** @} */

  /**
   * @name Rx SPDO specific function names
   * @{
   */
  #define SPDO_InitMapp                  SPDO_InitRxMapp //lint !e767
  #define SPDO_SetMapp                   SPDO_SetRxMapp //lint !e767
  #define SPDO_SOD_MappPara_CLBK         SPDO_SOD_RxMappPara_CLBK //lint !e767
  #define SPDO_MappActivate              SPDO_RxMappActivate //lint !e767
  /** @} */


/**
 * @def RX_SPDO
 * The following define activates the Rx SPDO part of the included .cin file
 *
 * @note The .cin file "SPDOmappcom.cin" is included as the sources of the receive and transmit SPDO
   mapping is almost the same.
 */
#define RX_SPDO

  #include "SPDOmappcom.cin"

  /**
  * @brief This function copies the SPDO data from the openSAFETY frame into the SOD.
  *
  * @param        b_instNum            instance number (not checked, checked in SPDO_CheckRxTimeout() or SPDO_BuildTxSpdo() and SPDO_ProcessRxSpdo()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
  *
  * @param        w_rxSpdoIdx          Rx SPDO index (not checked, checked in SPDO_CheckRxTimeout() or SPDO_BuildTxSpdo() or ProcessTReq() or ProcessTRes() or SPDO_ProcessRxSpdo() and SPDO_GetSpdoIdxForSAdr()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
  *
  * @param        b_payloadSize        payload size (checked) valid value: length of the SPDO defined by the mapping (ps_rxSpdo->b_lenOfSpdo)
  *
  * @param        pv_data              pointer to the received data (pointer not checked, checked in SPDO_ProcessRxSpdo()) valid range: <> NULL
  *
  * @return
  * - TRUE               - payload size is right
  * - FALSE              - payload size is wrong
  */
  BOOLEAN SPDO_RxMappingProcess(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx,
                                UINT8 b_payloadSize, const void *pv_data)
  {
    BOOLEAN o_ret = FALSE; /* return value */
    t_MAPP_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
    t_MAPP_PARAM *ps_rxSpdo = po_this->ps_mappPara + w_rxSpdoIdx; /* pointer to
                                              the Rx SPDO internal structure */
    UINT32 i; /* loop counter */

    /* if payload size is right */
    if (ps_rxSpdo->b_lenOfSpdo == b_payloadSize)
    {
      o_ret = TRUE;
      /* loop from 0 to length of the SPDO */
      for (i = 0U; i < ps_rxSpdo->b_lenOfSpdo; i++)
      {
        /* copy one Byte from the SPDO into the SOD */
        *(ps_rxSpdo->apb_DataForSpdo[i]) = *ADD_OFFSET(pv_data,i);
      }
    }
    /* else wrong payload size */
    else
    {
      /* count the wrong payload size events */
      SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_PAYLOAD);
      SERR_SetError(B_INSTNUM_ SPDO_k_ERR_PAYLOAD_SIZE,
                    (((UINT32)(w_rxSpdoIdx))<<16) + b_payloadSize);
    }
    
    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
    
    return o_ret;
  }

  /**
  * @brief This function sets all Rx SPDO related data to the default value.
  *
  * @param      b_instNum            instance number (not checked, checked in SPDO_CheckRxTimeout() or SSC_ProcessSNMTSSDOFrame() or SPDO_BuildTxSpdo() and SPDO_ProcessRxSpdo()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
  *
  * @param      w_rxSpdoIdx          Rx SPDO index (not checked, checked in SPDO_CheckRxTimeout() or  SPDO_BuildTxSpdo() or SPDO_ResetRxSm() ProcessTReq() or ProcessTRes() or SPDO_ProcessRxSpdo() and SPDO_GetSpdoIdxForSAdr()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
  */
  void SPDO_RxSpdoToSafeState(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx)
  {
    t_MAPP_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
    t_MAPP_PARAM *ps_rxSpdo = po_this->ps_mappPara + w_rxSpdoIdx; /* pointer to
                                              the Rx SPDO internal structure */
    UINT32 i; /* loop counter */

    /* loop from 0 to number of mapped objects */
    for (i=0U; i < ps_rxSpdo->b_noMappedObject; i++)
    {
      /* all data is set to the default value */
      MEMCOPY(ps_rxSpdo->as_mappedObject[i].pv_data,
             ps_rxSpdo->as_mappedObject[i].pv_defValue,
             ps_rxSpdo->as_mappedObject[i].b_dataLen);
    }

#if (SPDO_cfg_CONNECTION_VALID_BIT_FIELD == EPLS_k_ENABLE)
    #if (SPDO_cfg_CONNECTION_VALID_STATISTIC == EPLS_k_ENABLE)
      /* increase the statistic counter in case a connection turns from valid to invalid */
      if ( SHNF_aaulConnValidBit[B_INSTNUMidx][w_rxSpdoIdx / 32] & (0x00000001UL << (w_rxSpdoIdx % 32)))
      {
          SHNF_aaulConnValidStatistic[B_INSTNUMidx][w_rxSpdoIdx]++;
      }
    #endif
    /* reset the connection valid bit */
    SHNF_aaulConnValidBit[B_INSTNUMidx][w_rxSpdoIdx / 32] &= (~(0x00000001UL << (w_rxSpdoIdx % 32)));
#endif

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
  }

#else
  /* This file is compiled with different configuration (EPLScfg.h).
     If SPDO_cfg_MAX_NO_RX_SPDO is 0 then a compiler warning can be generated.
     To avoid this warning, remove this file from the project. */
#endif
/** @} */
