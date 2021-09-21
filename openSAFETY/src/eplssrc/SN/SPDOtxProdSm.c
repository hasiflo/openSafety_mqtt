/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOtxProdSm.c
 *
 * This file contains the SPDO Producer state machine and manages
*             the Tx SPDO Communication Parameters.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SPDOtxProdSm.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>changes for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 *     <tr><td>17.11.2016</td><td>Stefan Innerhofer</td><td>changed one byte b_noFreeFrm to two byte w_noFreeFrm</td></tr>
 *     <tr><td>30.01.2017</td><td>Roman Zwischelsberger</td><td>support SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC; fix uninitialized variable</td></tr>
 * </table>
 *
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "EPLScfgCheck.h"

#include "SDN.h"

#include "SHNF.h"
#include "SFS.h"

#include "SCFMapi.h"

#include "SODapi.h"
#include "SOD.h"

#include "SERRapi.h"
#include "SERR.h"
#include "SPDOerr.h"
#include "SPDOint.h"

#include "sacun.h"

/**
 * @name SPDO Tx Communication Parameters
 * @{
 *
 * Defines for the sub-indexes of the SPDO Tx Communication parameters
 * in the SOD with index 0x1C00
 */
/** source address */
#define k_SIDX_TX_SADR          1U
/** refresh prescale */
#define k_SIDX_REFRESH_PRESCALE 2U
/** number of time response */
#define k_SIDX_NO_TRES          3U
/** @} */

/**
 * State machine instances
 */
STATIC t_TXSM_OBJ as_Obj[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;


STATIC BOOLEAN SendTxSpdo(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_txSpdoIdx,
                          UINT8 b_spdoId, UINT8 b_tr, UINT16 w_tAdr,
                          UINT16 *pw_noFreeFrm);

static BOOLEAN SetPtrToTxCommPara(BYTE_B_INSTNUM_ UINT16 w_index,
                                  UINT8 b_subIdx, void **ppv_internPointerAdr);

#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
  STATIC BOOLEAN GetExtCtAllowed(BYTE_B_INSTNUM_ UINT16 const w_txSpdoIdx);
#endif

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  STATIC BOOLEAN GetConnectionValid(BYTE_B_INSTNUM_ UINT16 const w_txSpdoIdx);

  /**
   * @brief This function clears the ptrs to the connected RxSPDOs
   *
   * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame())
   * @param        w_txSpdoIdx        TxSPDO index (not checked, created in SPDO_ActivateTxSpdoMapping())
   */
void SPDO_ClearConnectedRxSpdo(BYTE_B_INSTNUM_ UINT16 const w_txSpdoIdx)
{
  t_VARIABLE_FOR_TX_SPDO  *po_this = &as_Obj[B_INSTNUMidx].as_txSpdo[w_txSpdoIdx].s_txVar;
  (void)MEMSET((void*)&po_this->aps_consSm[0],0,sizeof(po_this->aps_consSm));
}

/**
 * @brief This function connects the RxSPDO state machine to the TxSPDO.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame())
 * @param        w_txSpdoIdx        TxSPDO index (not checked, created in SPDO_ActivateTxSpdoMapping())
 * @param  ps_consSm
 * @todo Document parameter ps_consSm
 * @return
 * - TRUE             - connection was possible
 * - FALSE            - all connections occupied
 */
BOOLEAN SPDO_SyncOkConnect(BYTE_B_INSTNUM_ UINT16 const w_txSpdoIdx, t_RX_CONS_SM const * const ps_consSm)
{
  t_VARIABLE_FOR_TX_SPDO  *po_this = &as_Obj[B_INSTNUMidx].as_txSpdo[w_txSpdoIdx].s_txVar;
  BOOLEAN         o_retVal = FALSE;
#if (1 < SPDO_cfg_MAX_SYNC_RX_SPDO)
  UINT16          i;
  /* if more than one RxSPDO is synchronized over a TxSPDO a loop is neccessary */
  for ( i = 0; i < SPDO_cfg_MAX_SYNC_RX_SPDO; i++)
  {
    if ( NULL == po_this->aps_consSm[i])
    {
      po_this->aps_consSm[i] = (t_RX_CONS_SM*)ps_consSm;
      o_retVal = TRUE;
      break;
    }
  }
  /* if no connection could be made */
  if ( FALSE == o_retVal)
  {
    SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TX_NO_RX_CONN_1, (UINT32)w_txSpdoIdx);
  }
#else
  /* if there is only one RxSPDO synchronized over a TxSPDO no loop is neccessary */
  if ( NULL == po_this->aps_consSm[0])
  {
    po_this->aps_consSm[0] = ps_consSm;
    o_retVal = TRUE;
  }
  else
  {
    SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TX_NO_RX_CONN_1, (UINT32)w_txSpdoIdx);
  }
#endif
  return o_retVal;
}
#endif
/**
* @brief This function initializes structure for the communication parameters.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*/
void SPDO_InitTxSm(BYTE_B_INSTNUM)
{
  t_TXSM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  UINT32 i; /* loop counter */

  for(i = 0U; i < (UINT32)SPDO_cfg_MAX_NO_TX_SPDO; i++)
  {
    po_this->as_txSpdo[i].s_txCommPara.pw_sadr = ((UINT16 *)(NULL));
    po_this->as_txSpdo[i].s_txCommPara.pw_refreshPrescale = ((UINT16 *)(NULL));
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
 * @brief This function initializes pointers of the t_TX_COMM_PARAM structure to the Tx communication parameters.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_txSpdoIdx        index in the internal array of the Tx SPDO structures (not
 *  checked, checked in TxMappCommInit()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 * @param        w_index            index of the SPDO communication parameter object (not checked,
 *  checked in InitTxSpdo()) valid range: k_TX_COMM_START_IDX..k_TX_COMM_END_IDX
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SPDO_SetPtrToTxCommPara(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx,
                                UINT16 w_index)
{
  BOOLEAN o_ret = FALSE; /* return value */
  t_TXSM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  void *pv_noTres = NULL;

  /* pointer to the Tx communication parameter */
  t_TX_COMM_PARAM *ps_txComm = &po_this->as_txSpdo[w_txSpdoIdx].s_txCommPara;

  /* if SADR : 0x1C00-0x1FFE sub 1 is OK */
  if (SetPtrToTxCommPara(B_INSTNUM_ w_index, k_SIDX_TX_SADR,
                         (void **)(&ps_txComm->pw_sadr)))
  {
    /* if RefreshPrescaler : 0x1C00-0x1FFE sub 2 is OK */
    if (SetPtrToTxCommPara(B_INSTNUM_ w_index, k_SIDX_REFRESH_PRESCALE,
                           (void **)(&ps_txComm->pw_refreshPrescale)))
    {
      /* if NumberOfTRes : 0x1C00-0x1FFE sub 3 is OK */
      o_ret = SetPtrToTxCommPara(B_INSTNUM_ w_index, k_SIDX_NO_TRES,
                                 (void **)(&pv_noTres));
      SPDO_TimeSyncProdSmNoTresSet(B_INSTNUM_ w_txSpdoIdx, (UINT8 *)pv_noTres);
    }
    /* no else : RefreshPrescaler definition is wrong, error was signaled */
  }
  /* no else : SADR definition is wrong, error was signaled */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/**
 * @brief This function resets the SPDO time synchronization producer state machine, the counter and variables for Tx SPDOs.
 *
 * @param        b_instNum         instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_ct             consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
 *
 * @param        w_noTxSpdo        number of the tx SPDOs (not checked, checked in TxMappCommInit() and InitTxSpdo()) valid range: 1..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 */
void SPDO_ResetTxSm(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_noTxSpdo)
{
  t_TXSM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  t_TX_SPDO *ps_txSpdo; /* pointer to the Tx SPDO internal structure */
  UINT16 w_txSpdoIdx; /* loop counter */

  /* loop for all tx SPDO state machine */
  for(w_txSpdoIdx = 0U; w_txSpdoIdx < w_noTxSpdo; w_txSpdoIdx++)
  {
    ps_txSpdo = &po_this->as_txSpdo[w_txSpdoIdx];

#if ( SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC == EPLS_k_DISABLE )
    ps_txSpdo->s_txVar.e_sendingReqType = k_SENDING_FREE;
#endif

   /* s_txVar is initialized */
    ps_txSpdo->s_txVar.dw_timeoutDT = dw_ct; /* timeout is set to expired for
                                                the first time */
    ps_txSpdo->s_txVar.o_newData = TRUE;

    ps_txSpdo->s_txVar.o_lastCtChk = FALSE;
    ps_txSpdo->s_txVar.dw_lastCt = 0UL;

    SPDO_TimeSyncProdSmReset(B_INSTNUM_ w_txSpdoIdx);
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
 * @brief This function checks the Tx SPDO communication parameters. The SADR is checked and inserted into the assign table.
 *
 * @param     b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_txSpdoIdx        Tx SPDO index (not checked, checked in SPDO_ActivateTxSpdoMapping) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SPDO_CheckTxCommPara(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx)
{
  BOOLEAN o_ret = FALSE; /* return value */
  t_TXSM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  /* pointer to the Tx communication parameter */
  t_TX_COMM_PARAM *ps_txComm = &po_this->as_txSpdo[w_txSpdoIdx].s_txCommPara;

  /* if target address was inserted successful */
  if (SPDO_AddTAdr(B_INSTNUM_ w_txSpdoIdx, *ps_txComm->pw_sadr))
  {
    o_ret = TRUE;
  }
  /* else wrong target address */
  else
  {
    SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TADR, (UINT32)w_txSpdoIdx);
  }

  return o_ret;
}


/**
 * @brief Realization of the "SPDO Producer" state machine.
 *
 * This state machine has only one state (the other state is a transient state). In this state, if the Tx SPDO data
 * was not sent by Time Request or Time Response then a data only SPDO is sent.
 *
 * @param        b_instNum               instance number (not checked, checked in SPDO_BuildTxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
 *
 * @param        w_txSpdoIdx             Tx SPDO index (not checked, checked in SPDO_BuildTxSpdo()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 * @retval       pw_noFreeFrm            number of the free frames to be sent (pointer not checked, checked in SPDO_BuildTxSpdo()) valid range: <> NULL
 */
void SPDO_ProdSm(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_txSpdoIdx,
                 UINT16 *pw_noFreeFrm)
{
  t_TXSM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */

  /* if Tx SPDO is active */
  if (*po_this->as_txSpdo[w_txSpdoIdx].s_txCommPara.pw_sadr != k_NOT_USED_ADR)
  {
    /* if the Data only sending request was accomplished */
    if (SPDO_SendTxSpdo(B_INSTNUM_ dw_ct, w_txSpdoIdx, k_SENDING_DATA_ONLY,
                        0U, 0U, pw_noFreeFrm))
    {
      /* do nothing */
    }
    /* no else : the Data only sending request was not accomplished */
  }
  /* no else : data only SPDO was not sent */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
 * @brief This function sends a Tx SPDO
 *
 * This function sends a Tx SPDO if the following sending requirements are fulfilled:
 * - Data of the corresponding Tx SPDO was not sent.
 * - Internal timer value has been changed.
 * - Refresh prescale time is expired or new SPDO data are available or Time Response has to be sent immediately
 *
 * @param        b_instNum               instance number (not checked, checked in SPDO_BuildTxSpdo() or SPDO_ProcessRxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
 *
 * @param        w_txSpdoIdx             Tx SPDO index (not checked, checked in SPDO_BuildTxSpdo() or SPDO_TxSpdoIdxExists() or ProcessTReq() and SPDO_GetSpdoIdxForTAdr()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 * @param        e_sendingReqType        type of the sending request (checked)
 *
 * @param        b_tr                    internal time request counter (not checked, checked in IncTReqCounters()) valid range: 0..(EPLS_k_MAX_TR)
 *
 * @param        w_tAdr                  target address (not checked, only called with 0 in SPDO_ProdSm() or only called with adr in SPDO_TimeSyncProdSm() checked in checkRxAddrInfo() or only called with adr in SPDO_TimeSyncConsSm(), StateWaitForTres() checked in SPDO_CheckRxCommPara()) valid range: 0..(EPLS_k_MAX_SADR)
 *
 * @retval       pw_noFreeFrm            number of the free frames to be sent (pointer checked) valid range: <> NULL
 *
 * @return
 * - TRUE                  - Sending request is accomplished
 * - FALSE                 - Sending request is not accomplished
 */
BOOLEAN SPDO_SendTxSpdo(BYTE_B_INSTNUM_  UINT32 dw_ct, UINT16 w_txSpdoIdx,
                        t_SENDING_REQ_TYPE e_sendingReqType, UINT8 b_tr,
                        UINT16 w_tAdr, UINT16 *pw_noFreeFrm)
{
  BOOLEAN o_ret = FALSE; /* return value */
  t_TXSM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  t_TX_SPDO *ps_txSpdo = &po_this->as_txSpdo[w_txSpdoIdx]; /* pointer to
                                              the Tx SPDO internal structure */
  t_VARIABLE_FOR_TX_SPDO *ps_txVar = &ps_txSpdo->s_txVar;

  /* if sending request is invalid */
  if (e_sendingReqType == k_SENDING_FREE)
  {
    SERR_SetError(B_INSTNUM_ SPDO_k_ERR_SEND_TX_SPDO_1, (UINT32)k_SENDING_FREE);
  }
  /* else sending request is valid */
  else
  {
#if ( SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC == EPLS_k_DISABLE )
    /* if the Tx SPDO is free (no Tx SPDO was sent) */
    if (ps_txVar->e_sendingReqType == k_SENDING_FREE)
#endif
    {
      /* if free frame pointer is valid */
      if (pw_noFreeFrm != NULL)
      {
        /* if a free frame is available */
        if ((*pw_noFreeFrm) != 0U)
        {
          /* if the ct has not to be checked or ct was changed */
          if ((!ps_txVar->o_lastCtChk) || (ps_txVar->dw_lastCt != dw_ct))
          {
            /* if TResponse transmission is requested */
            if (e_sendingReqType == k_SENDING_TRES_IMMEDIATE)
            {
              o_ret = SendTxSpdo(B_INSTNUM_ dw_ct, w_txSpdoIdx,
                                  k_FRAME_ID_TRES, b_tr, w_tAdr, pw_noFreeFrm);
            }
            /* else other Tx SPDO transmission is requested */
            else
            {
              /* if Tx SPDO refresh time expired or new data is available */
              if ((EPLS_TIMEOUT(dw_ct, ps_txVar->dw_timeoutDT)) ||
                  (ps_txVar->o_newData))
              {
                /* if TRequest transmission is requested */
                if (e_sendingReqType == k_SENDING_TREQ)
                {
                  o_ret = SendTxSpdo(B_INSTNUM_ dw_ct, w_txSpdoIdx,
                                     k_FRAME_ID_TREQ, b_tr, w_tAdr,
                                     pw_noFreeFrm);
                }
                /* else if TResponse transmission is requested */
                else if (e_sendingReqType == k_SENDING_TRES)
                {
                  o_ret = SendTxSpdo(B_INSTNUM_ dw_ct, w_txSpdoIdx,
                                     k_FRAME_ID_TRES, b_tr, w_tAdr,
                                     pw_noFreeFrm);
                }
                /* else if Data only transmission is requested */
                else if (e_sendingReqType == k_SENDING_DATA_ONLY)
                {
                  o_ret = SendTxSpdo(B_INSTNUM_ dw_ct, w_txSpdoIdx,
                                    k_FRAME_ID_DATA_ONLY, b_tr,w_tAdr,
                                    pw_noFreeFrm);
                }
                /* else sending request is invalid */
                else
                {
                  SERR_SetError(B_INSTNUM_ SPDO_k_ERR_SEND_TX_SPDO_2,
                                (UINT32)e_sendingReqType);
                }
              }
              /* no else Tx SPDO refresh time did not expire */
            }

            /* if SPDO was sent */
            if (o_ret)
            {
              /* sending request type is set for this Tx SPDO */
              #if ( SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC == EPLS_k_DISABLE )
                ps_txVar->e_sendingReqType = e_sendingReqType;
              #endif
              /* Tx Ct is stored */
              ps_txVar->dw_lastCt = dw_ct;
              /* new data flag is cleared */
              ps_txVar->o_newData = FALSE;
              /* memorize that the first SPDO was sent and the dw_lastCt
                 has to be checked */
              ps_txVar->o_lastCtChk = TRUE;

              /* if Tx SPDO refresh time expired */
              if (EPLS_TIMEOUT(dw_ct, ps_txVar->dw_timeoutDT))
              {
                /* Tx SPDO refresh time is reloaded */
                ps_txVar->dw_timeoutDT = dw_ct +
                                    *ps_txSpdo->s_txCommPara.pw_refreshPrescale;
              }
            }
            /* no else : Tx SPDO was not sent */
          }
          /* no else : Tx SPDO was not sent */
        }
        /* no else : Tx SPDO was not sent */
      }
      /* else free frame pointer is invalid */
      else
      {
        SERR_SetError(B_INSTNUM_ SPDO_k_ERR_NULL_PTR, SERR_k_NO_ADD_INFO);
      }
    }
    /* no else : Tx SPDO transmission has already been accomplished */
  }

#if ( SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC == EPLS_k_DISABLE )
  /* if Data only transmission is requested */
  if (e_sendingReqType == k_SENDING_DATA_ONLY)
  {
    /* Tx SPDO is free for sending */
    ps_txVar->e_sendingReqType = k_SENDING_FREE;
  }
#endif // SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */

/**
 * @brief This function checks whether the given tx SPDO index exists or not.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame() or SPDO_TxDataChanged()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 * @param        w_txSpdoIdx        index in the internal array of the Tx SPDO structures (checked) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 * @return
 * - TRUE             - Tx SPDO number exists
 * - FALSE            - Tx SPDO number does not exist
 */
BOOLEAN SPDO_TxSpdoIdxExists(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx)
{
  BOOLEAN o_ret = FALSE; /* return value */
  t_TXSM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */

  /* if SPDO index is valid */
  if (w_txSpdoIdx < (UINT16)SPDO_cfg_MAX_NO_TX_SPDO)
  {
    /* if the Tx SPDO is defined */
    if (po_this->as_txSpdo[w_txSpdoIdx].s_txCommPara.pw_sadr != NULL)
    {
      /* if the Tx SPDO is configured */
      if (*po_this->as_txSpdo[w_txSpdoIdx].s_txCommPara.pw_sadr !=
          k_NOT_USED_ADR)
      {
        o_ret = TRUE;
      }
      /* else the Tx SPDO is not configured */
      else
      {
        SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TX_SPDO_NUM_1, (UINT32)w_txSpdoIdx);
      }
    }
    /* else Tx SPDO is not defined */
    else
    {
      SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TX_SPDO_NUM_2, (UINT32)w_txSpdoIdx);
    }
  }
  /* else invalid SPDO number */
  else
  {
    SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TX_SPDO_NUM_3, (UINT32)w_txSpdoIdx);
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/**
 * @brief This function sets the new data flag for the given Tx SPDO.
 *
 * @param        b_instNum          instance number (not checked, checked in SPDO_TxDataChanged()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_txSpdoIdx        index in the internal array of the Tx SPDO structures (not checked, checked in SPDO_TxSpdoIdxExists()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 */
void SPDO_NewData(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx)
{
  t_TXSM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */

  /* if Tx Spdo exists */
  if (SPDO_TxSpdoIdxExists(B_INSTNUM_ w_txSpdoIdx))
  {
    po_this->as_txSpdo[w_txSpdoIdx].s_txVar.o_newData = TRUE;
  }
  /* no else : Tx Spdo does not exist, error was already signaled */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

#pragma CTC SKIP
/**
* @brief Returns a pointer to the internal object.
*
* @note This function is used for unit tests.
*
* @param        b_instNum        instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - == NULL - function failed
* - <> NULL - function succeeded, reference to the internal object
*/
const t_TXSM_OBJ* SPDO_GetTxProdSmObj(BYTE_B_INSTNUM)
{
  const t_TXSM_OBJ* ps_obj = (t_TXSM_OBJ*)NULL;
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


/**
* @brief This function sends a Tx SPDO.
*
* @param        b_instNum               instance number (not checked, checked in SPDO_BuildTxSpdo() or SPDO_ProcessRxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
* @param        w_txSpdoIdx             Tx SPDO index (not checked, checked in SPDO_BuildTxSpdo() or SPDO_TxSpdoIdxExists() or ProcessTReq() and SPDO_GetSpdoIdxForTAdr()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
* @param        b_spdoId                SPDO identifier (not checked, only called with define)
* @param        b_tr                    internal time request counter (not checked, checked in IncTReqCounters()) valid range: 0..(EPLS_k_MAX_TR)
* @param        w_tAdr                  target address (not checked, only called with 0 in SPDO_ProdSm() or only called with adr in SPDO_TimeSyncProdSm() checked in checkRxAddrInfo() or only called with adr in SPDO_TimeSyncConsSm(), StateWaitForTres() checked in SPDO_CheckRxCommPara()) valid range: 0..(EPLS_k_MAX_SADR)
* @retval       pw_noFreeFrm            number of the free frames to be sent (pointer not checked, checked in SPDO_SendTxSpdo()) valid range: <> NULL
*
* @return
* - TRUE                  - TxSPDO was sent
* - FALSE                 - failure
*/
STATIC BOOLEAN SendTxSpdo(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_txSpdoIdx,
                          UINT8 b_spdoId, UINT8 b_tr, UINT16 w_tAdr,
                          UINT16 *pw_noFreeFrm)
{
  BOOLEAN o_ret = FALSE; /* return value */
  t_TXSM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  UINT8 *pb_eplsFrame; /* pointer to the openSAFETY frame */
  /* buffer for the tx SPDO frame header */
  static EPLS_t_FRM_HDR s_txSpdoHeaderBuffer SAFE_NO_INIT_SEKTOR;

#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
  t_EXT_CT* ps_extCt = SPDO_GetExtCt(B_INSTNUM);

  /* To satistfy the compiler, dw_ct get's self assigned, as it is not used
   * with this configuration option anymore. */
  dw_ct = dw_ct;
#endif

  /* SPDO frame header is initialized */
  /* openSAFETY domain number */
  s_txSpdoHeaderBuffer.w_sdn = SDN_GetSdn(B_INSTNUM);
  /* source address */
  s_txSpdoHeaderBuffer.w_adr = *po_this->as_txSpdo[w_txSpdoIdx].
                              s_txCommPara.pw_sadr;
  /* SPDO id is set */
  s_txSpdoHeaderBuffer.b_id = b_spdoId;
  /* if no Rx SPDOs are available, set the bit anyway */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  if (GetConnectionValid(B_INSTNUM_ w_txSpdoIdx))
#endif
  {
    s_txSpdoHeaderBuffer.b_id = s_txSpdoHeaderBuffer.b_id | k_FRAME_BIT_CONN_VALID;
  }
  /* TR (internal time request counter) */
  s_txSpdoHeaderBuffer.b_tr = b_tr;
  /* add upper 24 bit of the 40 bit CT value if supported and enabled */
#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
  /* internal time value of SN */
  s_txSpdoHeaderBuffer.w_ct = (UINT16)ps_extCt->ddw_actCt;
  s_txSpdoHeaderBuffer.dw_extCt = 0;

  if ( k_FRAME_ID_DATA_ONLY == b_spdoId)
  {
      /* if extended CT SPDOs are allowed signalize this in the TR field */
      if (ps_extCt->o_allowed)
      {
          /* if the extended CT is supported by all consumers */
          if ( GetExtCtAllowed(B_INSTNUM_ w_txSpdoIdx))
          {
              s_txSpdoHeaderBuffer.dw_extCt = (UINT32)(ps_extCt->ddw_actCt >> 16);
              s_txSpdoHeaderBuffer.b_tr |= (k_TR_EXT_CT_AVAILABLE | k_TR_EXT_CT_USED);
          }
          /* only set the "available" bit */
          else
          {
              s_txSpdoHeaderBuffer.b_tr |= k_TR_EXT_CT_AVAILABLE;
          }
      }
  }
#else
  /* internal time value of SN */
  s_txSpdoHeaderBuffer.w_ct = (UINT16)dw_ct;
#endif
  /* TADR (time request address) */
  s_txSpdoHeaderBuffer.w_tadr = w_tAdr;

  /* SPDO mapping is processed */
  pb_eplsFrame = SPDO_TxMappingProcess(B_INSTNUM_ w_txSpdoIdx,
                                       &(s_txSpdoHeaderBuffer.b_le));

  /* if the SPDO was allocated successful */
  if (pb_eplsFrame != NULL)
  {
    /* if EPLS Frame to SHNF was successful */
    if (SFS_FrmSerialize(B_INSTNUM_ &s_txSpdoHeaderBuffer, pb_eplsFrame))
    {
      /* Free frame counter is decremented */
      (*pw_noFreeFrm)--;
      /* Tx SPDO was sent */
      o_ret = TRUE;
    }
    /* no else : error */
  }
  /* no else : error */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/**
* @brief This function sets the internal pointers to the Tx SPDO communication parameter in the SOD.
*
* @param        b_instNum                    instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param        w_index                      index of the SPDO communication parameter object (not checked, checked in InitTxSpdo()) valid range: k_TX_COMM_START_IDX.. k_TX_COMM_END_IDX
* @param        b_subIdx                     sub-index of the SPDO communication parameter object (checked) valid range: k_SIDX_TX_SADR .. k_SIDX_NO_TRES
* @retval          ppv_internPointerAdr         pointer to address of the given Tx SPDO communication parameter in the
*   SOD (pointer not checked, only called with reference to pointer in SPDO_SetPtrToTxCommPara()) valid range: <> NULL
*
* @return
* - TRUE                       - success
* - FALSE                      - failure
*/
static BOOLEAN SetPtrToTxCommPara(BYTE_B_INSTNUM_ UINT16 w_index,
                                  UINT8 b_subIdx, void **ppv_internPointerAdr)
{
  BOOLEAN o_ret = FALSE; /* return value */
  void *pv_data; /* pointer to the data in the SOD */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */
  SOD_t_ACS_OBJECT_VIRT s_objAcs; /* structure to access the SOD */

  s_objAcs.s_obj.w_index = w_index;
  s_objAcs.s_obj.b_subIndex = b_subIdx;
  s_objAcs.dw_segOfs = 0;
  s_objAcs.dw_segSize = 0;

  /* if object access was OK */
  if (NULL != SOD_AttrGetVirt(B_INSTNUM_ &s_objAcs, &s_errRes))
  {
    /* data is read */
    pv_data = SOD_ReadVirt(B_INSTNUM_ &s_objAcs, &s_errRes);

    /* if object read access was OK */
    if (pv_data != NULL)
    {
      /* switch for the sub-index */
      switch (b_subIdx)
      {
        /* SADR : 0x1C00-0x1FFE sub 1 */
        case k_SIDX_TX_SADR:
        /* RefreshPrescaler : 0x1C00-0x1FFE sub 2 */
        case k_SIDX_REFRESH_PRESCALE:
        /* NumberOfTRes : 0x1C00-0x1FFE sub 3 */
        case k_SIDX_NO_TRES:
        {
          /* if "before read" is set */
          if (EPLS_IS_BIT_SET(s_objAcs.s_obj.s_attr.w_attr, SOD_k_ATTR_BEF_RD))
          {
            SERR_SetError(B_INSTNUM_ SPDO_k_ERR_BEF_RD_TX,
                          (((UINT32)(w_index))<<16) + b_subIdx);
          }
          /* else "before read" is not set */
          else
          {
            *ppv_internPointerAdr = pv_data;
            o_ret = TRUE;
          }
          break;
        }
        #pragma CTC SKIP
        default:
        {
          SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TX_SPDO_OBJ,
                        (((UINT32)(w_index))<<16) + b_subIdx);
        }
        #pragma CTC ENDSKIP
      }
    }
    else /* read access failed */
    {
      SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
    }
  }
  else /* SOD access error */
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #14 - 'case' conditions do not equal
                                           'break' */
}
  /* RSM_IGNORE_QUALITY_END */

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
/**
* @brief This function gets the connection valid bit for the given Tx SPDO index.
*
* @param        b_instNum          instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param        w_txSpdoIdx        Tx SPDO index (not checked, checked in SPDO_BuildTxSpdo() or SPDO_TxSpdoIdxExists() or ProcessTReq() and SPDO_GetSpdoIdxForTAdr()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
*
* @return
* - TRUE             - connection valid bit = 1
* - FALSE            - connection valid bit = 0
*/
STATIC BOOLEAN GetConnectionValid(BYTE_B_INSTNUM_ UINT16 const w_txSpdoIdx)
{
  t_VARIABLE_FOR_TX_SPDO  *po_this = &as_Obj[B_INSTNUMidx].as_txSpdo[w_txSpdoIdx].s_txVar;
  BOOLEAN o_retVal = TRUE;
#if (1 < SPDO_cfg_MAX_SYNC_RX_SPDO)
  UINT16 i;
  for (i = 0; i < SPDO_cfg_MAX_SYNC_RX_SPDO; i++)
  {
    if ((NULL != po_this->aps_consSm[i]) &&
      (!po_this->aps_consSm[i]->o_timeSyncOk
#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
     || po_this->aps_consSm[i]->o_extCtError
     || po_this->aps_consSm[i]->o_extCtUnknown
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */
      ))
    {
      o_retVal = FALSE;
    }
  }
#else
  if ((NULL != po_this->aps_consSm[0]) &&
    (!po_this->aps_consSm[0]->o_timeSyncOk
#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
     || po_this->aps_consSm[0]->o_extCtError
     || po_this->aps_consSm[0]->o_extCtUnknown
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */
     ))
  {
    o_retVal = FALSE;
  }
#endif
  return o_retVal;
}
#endif /* #if (SPDO_cfg_MAX_NO_RX_SPDO != 0) */

#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
/**
* @brief This function checks weather 40 bit SPDOs are allowed.
*
* @param        b_instNum          instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param        w_txSpdoIdx        Tx SPDO index (not checked, checked in SPDO_BuildTxSpdo() or SPDO_TxSpdoIdxExists() or ProcessTReq() and SPDO_GetSpdoIdxForTAdr()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
*
* @return
* - TRUE             - 40-bit SPDOs are allowed
* - FALSE            - 40-bit SPDOs are not allowed
*/
STATIC BOOLEAN GetExtCtAllowed(BYTE_B_INSTNUM_ UINT16 const w_txSpdoIdx)
{
  t_VARIABLE_FOR_TX_SPDO  *po_this = &as_Obj[B_INSTNUMidx].as_txSpdo[w_txSpdoIdx].s_txVar;
  BOOLEAN o_retVal = TRUE;
#if (1 < SPDO_cfg_MAX_SYNC_RX_SPDO)
  UINT16 i;
  for (i = 0; i < SPDO_cfg_MAX_SYNC_RX_SPDO; i++)
  {
    if ((NULL != po_this->aps_consSm[i]) &&
      (!po_this->aps_consSm[i]->o_extCtAvail))
    {
      o_retVal = FALSE;
    }
  }
#else
  if ((NULL != po_this->aps_consSm[0]) &&
    (!po_this->aps_consSm[0]->o_extCtAvail))
  {
    o_retVal = FALSE;
  }
#endif
  return o_retVal;
}
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */

/** @} */
