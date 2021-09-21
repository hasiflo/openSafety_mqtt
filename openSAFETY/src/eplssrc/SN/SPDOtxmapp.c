/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOtxmapp.c
 *
 * This file contains functions for processing the Tx mapping.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SPDOtxmapp.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>21.03.2011</td><td>Hans Pill</td><td>A&P257950 changed variables for better usage of memory</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 * </table>
 *
 */


#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "EPLScfgCheck.h"

#include "SHNF.h"
#include "SFS.h"

#include "SCFMapi.h"

#include "SODapi.h"
#include "SODerr.h"
#include "SOD.h"

#include "SERRapi.h"
#include "SERR.h"
#include "SPDOerr.h"

#include "SPDOapi.h"
#include "SPDOint.h"

/* Lint error 767 : (Info -- macro 'xxx' was defined differently in another
                             module) */
/**
 * @name Tx SPDO specific configuration defines
 * @{
 */
/* RSM_IGNORE_QUALITY_BEGIN Notice #1 - Physical line length > 80 char. */
#define SPDO_cfg_MAX_SPDO_MAPP_ENTRIES SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES //lint !e767
/* RSM_IGNORE_QUALITY_END */
#define SPDO_cfg_MAX_LEN_OF_SPDO       SPDO_cfg_MAX_LEN_OF_TX_SPDO //lint !e767
#define SPDO_cfg_MAX_NO_SPDO           SPDO_cfg_MAX_NO_TX_SPDO //lint !e767
#define SPDO_cfg_MAX_NO_SPDO_SDG       SPDO_cfg_MAX_NO_TX_SPDO_SDG //lint !e767
/** @} */

/**
 * @name Tx SPDO specific constants
 * @{
 */
#define k_MAPP_START_IDX               k_TX_MAPP_START_IDX //lint !e767
#define SPDO_k_ERR_BEF_WR_SUB_X        SPDO_k_ERR_BEF_WR_TX_SUB_X //lint !e767
#define SPDO_k_ERR_MAPP_ENTRY_1        SPDO_k_ERR_TX_MAPP_ENTRY_1 //lint !e767
#define SPDO_k_ERR_TOO_LONG            SPDO_k_ERR_TX_TOO_LONG //lint !e767
#define SPDO_k_ERR_OBJ_MAPP_1          SPDO_k_ERR_TX_OBJ_MAPP_1 //lint !e767
#define SPDO_k_ERR_MAPP_ENTRY_3        SPDO_k_ERR_TX_MAPP_ENTRY_3 //lint !e767
#define SPDO_k_ERR_MAP_OBJ             SPDO_k_ERR_TX_MAP_OBJ //lint !e767
#define SPDO_k_ERR_SOD_SRV             SPDO_k_ERR_TX_SOD_SRV //lint !e767
/** @} */

/**
 * @name Tx SPDO specific function names
 * @{
 */
#define SPDO_InitMapp                  SPDO_InitTxMapp //lint !e767
#define SPDO_SetMapp                   SPDO_SetTxMapp //lint !e767
#define SPDO_SOD_MappPara_CLBK         SPDO_SOD_TxMappPara_CLBK //lint !e767
#define SPDO_MappActivate              SPDO_TxMappActivate //lint !e767
/** @} */

/**
 * @def TX_SPDO
 * The following define activates the Tx SPDO part of the included .cin file
 *
 * @note The .cin file "SPDOmappcom.cin" is included as the sources of the receive and transmit SPDO
   mapping is almost the same.
 */
#define TX_SPDO

#include "SPDOmappcom.cin"

/**
* @brief This function allocates an openSAFETY frame and copies the SPDO data from the SOD into the openSAFETY frame.
*
* @param    b_instNum              instance number (not checked, checked in SPDO_BuildTxSpdo() or SPDO_ProcessRxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param    w_txSpdoIdx            Tx SPDO index (not checked, checked in SPDO_BuildTxSpdo() or SPDO_TxSpdoIdxExists() or ProcessTReq() and SPDO_GetSpdoIdxForTAdr()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
* @retval      pb_len                 reference to the length of the TxSPDO (pointer not checked, only called with reference to variable), valid range: <> NULL
*
* @return
* - == NULL          - memory allocation failed
* - <> NULL          - memory allocation and the mapping processing  succeeded, reference to the openSAFETY frame
*/
UINT8 *SPDO_TxMappingProcess(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx, UINT8 *pb_len)
{
  UINT8 *pb_ret = (UINT8 *)NULL; /* return value */
  t_MAPP_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  t_MAPP_PARAM *ps_txSpdo = po_this->ps_mappPara + w_txSpdoIdx; /* pointer to
                                              the Tx SPDO internal structure */
  UINT8 *pb_payloadData; /* temporary pointer to set the payload data */
  UINT32 i; /* loop counter */

  /* if there are objects  mapped */
  if (ps_txSpdo->b_noMappedObject != 0U)
  {
    /* openSAFETY frame is allocated */
    pb_ret = SFS_GetMemBlock(B_INSTNUM_ SHNF_k_SPDO, w_txSpdoIdx,
                             ps_txSpdo->b_lenOfSpdo);

    /* if the openSAFETY frame is allocated successfully */
    if (pb_ret != NULL)
    {
      /* pointer is set to the payload data */
      pb_payloadData = ADD_OFFSET(pb_ret, SFS_k_FRM_DATA_OFFSET);

      /* number of payload data */
      *pb_len = ps_txSpdo->b_lenOfSpdo;
    
      /* loop from 0 to length of the SPDO */
      for (i = 0U; i < ps_txSpdo->b_lenOfSpdo; i++)
      {
        /* copy one Byte from the pointer byte array into the SPDO */
        *ADD_OFFSET(pb_payloadData, i) = *(ps_txSpdo->apb_DataForSpdo[i]);
      }
    }
    /* no else : error */
  }
  /* else mapping is not active */
  else
  {
    /* openSAFETY frame is allocated */
    pb_ret = SFS_GetMemBlock(B_INSTNUM_ SHNF_k_SPDO, w_txSpdoIdx, 0U);
    
    /* number of payload data */
    *pb_len = 0U;
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return pb_ret;
}

/** @} */
