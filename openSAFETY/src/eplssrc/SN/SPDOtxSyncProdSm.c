/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOtxSyncProdSm.c
 *
 * This file contains the Time Synchronization Producer state machine.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 *<h2>History forSPDOtxSyncProdSm.c</h2>
 * <table>
 *     <tr><td>17.11.2016</td><td>Stefan Innerhofer</td><td>changed one byte b_noFreeFrm to two byte w_noFreeFrm</td></tr>
 * </table>
 *
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "EPLScfgCheck.h"

#include "SCFMapi.h"

#include "SERRapi.h"
#include "SERR.h"
#include "SPDOerr.h"
#include "SPDOint.h"


/**
 * Internal Instanve storage
 */
static t_TX_SYNC_PROD_SM_OBJ as_Obj[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * @brief This function resets the given SPDO time synchronization producer state machine.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_txSpdoIdx        Tx SPDO index (not checked, checked in SPDO_ResetTxSm()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 */
void SPDO_TimeSyncProdSmReset(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx)
{
  t_TX_SYNC_PROD_SM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  t_TX_SYNC_PROD_SM *ps_txSyncProdSm; /* pointer to the Tx SPDO internal
                                          structure */

  ps_txSyncProdSm = &po_this->as_txSyncProdSm[w_txSpdoIdx];

  ps_txSyncProdSm->o_waitForTReq = TRUE;
  ps_txSyncProdSm->b_echoTR = 0U;
  ps_txSyncProdSm->w_echoTAdr = 0U;
  ps_txSyncProdSm->o_startTResBlock = FALSE;
  ps_txSyncProdSm->b_noTRes = *ps_txSyncProdSm->pb_noTRes;
  /* the pb_noTRes pointer is initialized once in the
     SPDO_TimeSyncProdSmNoTresSet() and it must not be reset */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
 * @brief This function initializes the pointer to the number of time response SOD object (0x1C00-1FFE sub 3).
 *
 * @param    b_instNum          instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_txSpdoIdx        Tx SPDO index (not checked, checked in TxMappCommInit()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 * @param        pb_noTres          pointer to the number of time response entry (pointer not checked, only called with
 * 	reference to variable in SPDO_SetPtrToTxCommPara()) valid range: <> NULL
 */
void SPDO_TimeSyncProdSmNoTresSet(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx,
                                  UINT8 *pb_noTres)
{
  t_TX_SYNC_PROD_SM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */

  po_this->as_txSyncProdSm[w_txSpdoIdx].pb_noTRes = pb_noTres;

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
 * @brief Realization of the "Time Synchronization Producer" state machine.
 *
 * This state machine is waiting for Time Request SPDOs and if the corresponding Time Request SPDO is received
 * then the state of the state machine is switched and a Time Response block is sent. It is called by the
 * SPDO_BuildTxSpdo() to send a Time Response and the SPDO_ProcessRxSpdo() in case a Time Request received.
 *
 * @param        b_instNum               instance number (not checked, checked in SPDO_BuildTxSpdo() or SPDO_ProcessRxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
 *
 * @param        w_txSpdoIdx             Tx SPDO index (not checked, checked in SPDO_BuildTxSpdo() or ProcessTReq() and SPDO_GetSpdoIdxForTAdr()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 * @param        ps_rxSpdoHeader         reference to TRes header info
 * - <> NULL : function called by the SPDO_ProcessRxSpdo()
 * - == NULL : function called by the SPDO_BuildTxSpdo() (pointer not checked, checked in SPDO_ProcessRxSpdo()) valid range: <> NULL, == NULL
 *
 * @retval         pw_noFreeFrm            number of the free frames to be sent
 * - <> NULL : function called by the SPDO_BuildTxSpdo()
 * - == NULL : function called by the SPDO_ProcessRxSpdo() (pointer checked, value checked) valid
 * 	pointer range : <> NULL, == NULL valid value range: > 0
 */
void SPDO_TimeSyncProdSm(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_txSpdoIdx,
                         const EPLS_t_FRM_HDR *ps_rxSpdoHeader,
                         UINT16 *pw_noFreeFrm)
{
  t_TX_SYNC_PROD_SM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */

  /* pointer to the Tx SPDO internal structure */
  t_TX_SYNC_PROD_SM *ps_txSyncProdSm = &po_this->as_txSyncProdSm[w_txSpdoIdx];
  t_SENDING_REQ_TYPE e_sendingReqType; /* sending request type */

  /* if the state machine is waiting for TRequest */
  if (ps_txSyncProdSm->o_waitForTReq)
  {
    /* if TReq was received (function called by SPDO_ProcessRxSpdo) */
    if (ps_rxSpdoHeader != NULL)
    {
      /* if the Tx SPDO is only for the TRequests */
      if (ps_txSyncProdSm->b_noTRes == 0U)
      {
        SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TREQ_PROC, (UINT32)w_txSpdoIdx);
      }
      /* else not only for the TRequests */
      else
      {
        /* TR in the received TReq is stored */
        ps_txSyncProdSm->b_echoTR = ps_rxSpdoHeader->b_tr;
        /* source address in the received TReq is stored to use as target
          address in the TResp*/
        ps_txSyncProdSm->w_echoTAdr = ps_rxSpdoHeader->w_adr;
        /* change state */
        ps_txSyncProdSm->o_waitForTReq = FALSE;
        /* signal the beginning of the TRes block */
        ps_txSyncProdSm->o_startTResBlock = TRUE;
      }
    }
    /* no else : TReq was not received (function called by SPDO_BuildTxSpdo) */
  }
  /* else the state machine is not waiting for TRequest (sending TResponse) */
  else
  {
    /* if TReq was not received (function called by SPDO_BuildTxSpdo) */
    if ((ps_rxSpdoHeader == NULL) && (pw_noFreeFrm != NULL))
    {
      /* if TResp block is started*/
      if (ps_txSyncProdSm->o_startTResBlock)
      {
        e_sendingReqType = k_SENDING_TRES_IMMEDIATE;
      }
      /* else TResp block was started */
      else
      {
        e_sendingReqType = k_SENDING_TRES;
      }

      /* if the TRes sending request was accomplished */
      if (SPDO_SendTxSpdo(B_INSTNUM_ dw_ct, w_txSpdoIdx, e_sendingReqType,
                          ps_txSyncProdSm->b_echoTR,
                          ps_txSyncProdSm->w_echoTAdr, pw_noFreeFrm))
      {
        /* first TResp was sent */
        ps_txSyncProdSm->o_startTResBlock = FALSE;

        /* number of sent TRes is decremented */
        ps_txSyncProdSm->b_noTRes--;

        /* if all TRes were sent */
        if (ps_txSyncProdSm->b_noTRes == 0U)
        {
          /* change state */
          ps_txSyncProdSm->o_waitForTReq = TRUE;
          /* TResponse counter is reloaded */
          ps_txSyncProdSm->b_noTRes = *ps_txSyncProdSm->pb_noTRes;
        }
        /* no else : there are TRes to send */
      }
      /* no else : the TRes sending request was not accomplished */
    }
    /* no else : TReq was received (function called by SPDO_ProcessRxSpdo)
                 TReq is ignored in this state and no error is generated */
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

#pragma CTC SKIP
/**
* @brief Returns a pointer to the internal object
*
* @param        b_instNum        instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - == NULL - function failed
* - <> NULL - function succeeded, reference to the internal object
*/
const t_TX_SYNC_PROD_SM_OBJ* SPDO_GetTxSyncProdSmObj(BYTE_B_INSTNUM)
{
  const t_TX_SYNC_PROD_SM_OBJ* ps_obj = (t_TX_SYNC_PROD_SM_OBJ *)NULL;
  #if (EPLS_cfg_MAX_INSTANCES > 1)
    // check range of instance number
    if(B_INSTNUMidx < (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      // get pointer to object
      ps_obj = &as_Obj[B_INSTNUMidx];
    }
    // else - do nothing
  #else
    ps_obj = &as_Obj[B_INSTNUMidx];
  #endif

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return ps_obj;
}
#pragma CTC ENDSKIP


/** @} */
