/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOrxSyncConsSm.c
 *
 * This file contains the RX SPDO Time Synchronization Consumer
 * State machine and manages the Rx SPDO Communication Parameters.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SPDOrxSyncConsSm.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>22.03.2011</td><td>Hans Pill</td><td>A&P257950 changes for different SPDO numbers of main instance and SDG instance</td></tr>
 *     <tr><td>04.04.2011</td><td>Hans Pill</td><td>added preprocessor condition because some lines of codes cannot get reached for non SDG modules</td></tr>
 *     <tr><td>08.04.2011</td><td>Hans Pill</td><td>A&P258030 the propagation delay of the last synchronization needs to be stored</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 *     <tr><td>15.03.2013</td><td>Hans Pill</td><td>added statistic counters</td></tr>
 *     <tr><td>10.06.2013</td><td>Hans Pill</td><td>variable now uses the safe segment</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>03.07.2014</td><td>Hans Pill</td><td>changes for dynamic SCT</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 *     <tr><td>17.11.2016</td><td>Stefan Innerhofer</td><td>changed one byte b_noFreeFrm to two byte w_noFreeFrm</td></tr>
 *     <tr><td>27.06.2016</td><td>Stefan Innerhofer</td><td>calculate propagation delay statistic only for instance 0</td></tr>
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
#include "SOD.h"

#include "SERRapi.h"
#include "SERR.h"
#include "SPDOerr.h"
#include "SPDOint.h"

#include "SHNF.h"

/**
 * @name SPDO Rx Communication parameter defines
 * @{
 * Defines for the sub-indexes of the SPDO Rx Communication parameters
 * in the SOD with index 0x1400
 */
/** source address */
#define k_SIDX_RX_SADR               1U
/** SCT timer */
#define k_SIDX_SCT                   2U
/** number of consecutive TReq */
#define k_SIDX_NO_CONSECUTIVE_TREQ   3U
 /** TReq time delay */
#define k_SIDX_TIME_DELAY_TREQ       4U
/** synchronization time delay */
#define k_SIDX_TIME_DELAY_SYNC       5U
/** minimum time synchronization propagation delay */
#define k_SIDX_MIN_T_SYNC_PROP_DELAY 6U
/** maximum time synchronization propagation delay */
#define k_SIDX_MAX_T_SYNC_PROP_DELAY 7U
/** minimum SPDO propagation delay */
#define k_SIDX_MIN_SPDO_PROP_DELAY   8U
/** maximum SPDO propagation delay */
#define k_SIDX_MAX_SPDO_PROP_DELAY   9U
/** best case TRes delay */
#define k_SIDX_BEST_CASE_TRES_DELAY  10U
/** time request cycle */
#define k_SIDX_TIME_REQUEST_CYCLE    11U
/** Tx SPDO number */
#define k_SIDX_TX_SPDO_NO            12U
/** @} */

/**
 * Object containing all time synchronization state machines.
 */
static t_RX_SYNC_CONS_SPDO as_TimeSyncConsSm[(EPLS_cfg_MAX_INSTANCES - 1)*SPDO_cfg_MAX_NO_RX_SPDO_SDG +
                                             SPDO_cfg_MAX_NO_RX_SPDO] SAFE_NO_INIT_SEKTOR;
/**
 * Object structure to control the Rx SPDOs.
*/
static t_RX_SYNC_CONS_SM_OBJ as_Obj[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;


static void TimeSyncConsSmInit(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx);
static BOOLEAN SetPtrToRxCommPara(BYTE_B_INSTNUM_ UINT16 w_index,
                                  UINT8 b_subIdx, void **ppv_internPointerAdr);

static void StateWaitForTres(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_rxSpdoIdx,
                             const EPLS_t_FRM_HDR *ps_rxSpdoHeader,
                             UINT16 *pw_noFreeFrm);
static BOOLEAN IsTrValid(UINT8 b_tr, UINT8 b_firstSentTr, UINT8 b_lastSentTr);
static void IncTReqCounters(UINT32 dw_ct, t_RX_SYNC_CONS_SPDO *ps_rxSpdo);


#if (SPDO_cfg_PROP_DELAY_STATISTIC == EPLS_k_ENABLE)
    /** Global field for the propagation delay statistic **/
    UINT32 SHNF_adwPropDelayStatistic[SPDO_cfg_MAX_NO_RX_SPDO] SAFE_NO_INIT_SEKTOR;
#endif

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)

/**
 * @brief This function initializes the SPDO rx sync pointer array.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 */
void SPDO_InitRxSyncCons(BYTE_B_INSTNUM)
{
#if EPLS_cfg_MAX_INSTANCES > 1 /* if more instances are configured */
	/* initialize the pointer array */
	if ( 0 == B_INSTNUMidx)
	{
		as_Obj[B_INSTNUMidx].ps_rxSpdo	= &as_TimeSyncConsSm[0];
	}
	else
	{
		as_Obj[B_INSTNUMidx].ps_rxSpdo	= &as_TimeSyncConsSm[SPDO_cfg_MAX_NO_RX_SPDO + (B_INSTNUMidx - 1)*SPDO_cfg_MAX_NO_RX_SPDO_SDG];
	}
#else /* EPLS_cfg_MAX_INSTANCES > 1 */
	as_Obj[B_INSTNUMidx].ps_rxSpdo	= &as_TimeSyncConsSm[0];
#endif /* EPLS_cfg_MAX_INSTANCES > 1 */
}
/**
* @brief This function gets the TxSPDO number for a RxSPDO.
*
* @param    b_instNum              instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param    w_rxSpdoIdx            index in the internal array of the Rx SPDO structures (not checked, checked in RxMappCommInit()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
*
* @return TxSPDO number
*/
UINT16 SPDO_GetTxSpdoNo(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx)
{
  UINT16 w_txSpdoNo;

  if ( 0 != *(as_Obj[B_INSTNUMidx].ps_rxSpdo + w_rxSpdoIdx)->s_rxCommPara.pw_sadr)
  {
    w_txSpdoNo = *(as_Obj[B_INSTNUMidx].ps_rxSpdo + w_rxSpdoIdx)->s_rxCommPara.pw_txSpdoNo;
  }
  else
  {
    w_txSpdoNo = 0;
  }
  return w_txSpdoNo;
}
#endif
/**
 * @brief This function initializes pointers of the t_RX_COMM_PARAM structure to the Rx communication parameters
 * and the variables for the Time Synchronization State Machine.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_rxSpdoIdx        index in the internal array of the Rx SPDO structures (not checked, checked in RxMappCommInit()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param        w_index            index of the SPDO communication parameter object (not checked, checked in InitRxSpdo()) valid range: k_RX_COMM_START_IDX..k_RX_COMM_END_IDX
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SPDO_SetPtrToRxCommPara(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx,
                                UINT16 w_index)
{
  BOOLEAN o_ret = FALSE; /* return value */
  t_RX_SYNC_CONS_SM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  /* pointer to the internal communication parameter structure */
  t_RX_SYNC_CONS_COMM_PARAM *ps_rxComm =
      &(po_this->ps_rxSpdo + w_rxSpdoIdx)->s_rxCommPara;
  const void *pv_sct; /* pointer to the SOD */
  const void *pv_minSPDOPropDelay; /* pointer to the SOD */
  const void *pv_maxSPDOPropDelay; /* pointer to the SOD */

  /* if SADR : 0x1400-0x17FE sub 1 is not OK */
  if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_RX_SADR,
                          (void **)(&ps_rxComm->pw_sadr)))
  {
    /* error was already signaled */
  }
  /* else if SCT : 0x1400-0x17FE sub 2 is not OK */
  else if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_SCT,
                               (void **)(&pv_sct)))
  {
    /* error was already signaled */
  }
  /* else if NumberOfConsecutiveTReq : 0x1400-0x17FE sub 3 is not OK */
  else if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_NO_CONSECUTIVE_TREQ,
                               (void **)(&ps_rxComm->pb_noConsecutiveTReq)))
  {
    /* error was already signaled */
  }
  /* else if TimeDelayTReq : 0x1400-0x17FE sub 4 is not OK */
  else if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_TIME_DELAY_TREQ,
                               (void **)(&ps_rxComm->pdw_timeDelayTReq)))
  {
    /* error was already signaled */
  }
  /* else if TimeDelaySync : 0x1400-0x17FE sub 5 is not OK */
  else if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_TIME_DELAY_SYNC,
                               (void **)(&ps_rxComm->pdw_timeDelaySync)))
  {
    /* error was already signaled */
  }
  /* else if MinTSyncPropagationDelay : 0x1400-0x17FE sub 6 is not OK */
  else if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_MIN_T_SYNC_PROP_DELAY,
                              (void **)(&ps_rxComm->pw_minTSyncPropDelay)))
  {
    /* error was already signaled */
  }
  /* else if MaxTSyncPropagationDelay : 0x1400-0x17FE sub 7 is not OK */
  else if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_MAX_T_SYNC_PROP_DELAY,
                               (void **)(&ps_rxComm->pw_maxTSyncPropDelay)))
  {
    /* error was already signaled */
  }
  /* else if MinSPDOPropagationDelay : 0x1400-0x17FE sub 8 is not OK */
  else if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_MIN_SPDO_PROP_DELAY,
                               (void **)(&pv_minSPDOPropDelay)))
  {
    /* error was already signaled */
  }
  /* else if MaxSPDOPropagationDelay : 0x1400-0x17FE sub 9 is not OK */
  else if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_MAX_SPDO_PROP_DELAY,
                               (void **)(&pv_maxSPDOPropDelay)))
  {
    /* error was already signaled */
  }
  /* else if BestCaseTResDelay : 0x1400-0x17FE sub 10 is not OK */
  else if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_BEST_CASE_TRES_DELAY,
                               (void **)(&ps_rxComm->pw_bestCaseTResDelay)))
  {
    /* error was already signaled */
  }
  /* else if TimeRequestCycle : 0x1400-0x17FE sub 11 is not OK */
  else if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_TIME_REQUEST_CYCLE,
                               (void **)(&ps_rxComm->pdw_timeRequestCycle)))
  {
    /* error was already signaled */
  }
  /* else if TxSPDONo : 0x1400-0x17FE sub 12 is not OK */
  else if (!SetPtrToRxCommPara(B_INSTNUM_ w_index, k_SIDX_TX_SPDO_NO,
                               (void **)(&ps_rxComm->pw_txSpdoNo)))
  {
    /* error was already signaled */
  }
  /* else no error */
  else
  {
    ps_rxComm->pdw_sct             = (UINT32 *)pv_sct;
    ps_rxComm->pw_minSPDOPropDelay = (UINT16 *)pv_minSPDOPropDelay;

    SPDO_ConsSmInit(B_INSTNUM_ w_rxSpdoIdx, (UINT32 *)pv_sct,
                    (UINT16 *)pv_minSPDOPropDelay,
                    (UINT16 *)pv_maxSPDOPropDelay,
                    &ps_rxComm->dw_safeReactionTime);

    TimeSyncConsSmInit(B_INSTNUM_ w_rxSpdoIdx);

    o_ret = TRUE;
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/**
 * @brief This function resets the state machines and the counters for the rx SPDOs.
 *
 * @param        b_instNum               instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_noRxSpdo              number of the Rx SPDOs (not checked, checked in RxMappCommInit()) valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 */
void SPDO_ResetRxSm(BYTE_B_INSTNUM_ UINT16 w_noRxSpdo)
{
  UINT16 w_spdoIdx; /* loop counter for the SPDO index */

  /* loop for all rx SPDO state machine */
  for(w_spdoIdx = 0U; w_spdoIdx < w_noRxSpdo; w_spdoIdx++)
  {
    TimeSyncConsSmInit(B_INSTNUM_ w_spdoIdx);

    /* All rx SPDO related data are set to the default value */
    SPDO_RxSpdoToSafeState(B_INSTNUM_ w_spdoIdx);

    SPDO_ConsSmResetRx(B_INSTNUM_ w_spdoIdx);
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
 * @brief This function checks the Rx SPDO communication parameters.
 *
 * The SADR and Tx SPDO number are checked and the SADR is inserted into the assign table.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 * @param        w_rxSpdoIdx        Rx SPDO index (not checked, checked in SPDO_ActivateRxSpdoMapping()) valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SPDO_CheckRxCommPara(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx)
{
  BOOLEAN o_ret = FALSE; /* return value */
  t_RX_SYNC_CONS_SM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  /* pointer to the internal communication parameter structure */
  t_RX_SYNC_CONS_COMM_PARAM *ps_rxComm =
      &(po_this->ps_rxSpdo + w_rxSpdoIdx)->s_rxCommPara;

  /* if source address was not inserted successful */
  if (!SPDO_AddSAdr(B_INSTNUM_ w_rxSpdoIdx, *ps_rxComm->pw_sadr))
  {
    SERR_SetError(B_INSTNUM_ SPDO_k_ERR_SADR, (UINT32)w_rxSpdoIdx);
  }
  /* else if Rx SPDO is not deactivated */
  else if (*ps_rxComm->pw_sadr != k_NOT_USED_ADR)
  {
    /* Check whether the Tx SPDO number exists */
    o_ret = SPDO_TxSpdoIdxExists(B_INSTNUM_ SPDO_NUM_TO_INTERNAL_SPDO_IDX(
                                 *ps_rxComm->pw_txSpdoNo));
  }
  /* Communication parameter is OK */
  else
  {
    o_ret = TRUE;
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_ret;
}

/**
 * @brief Realization of the "Time Synchronization Consumer" state machine.
 *
 * This state machine is responsible for the time synchronization of the Rx SPDO.
 * Therefore Time Request SPDOs are sent and Time Response SPDOs are expected by
 * this state machine. It is called by the SPDO_BuildTxSpdo() to send a Time Request
 * and the SPDO_ProcessRxSpdo() in case a Time Response received.
 *
 * @param        b_instNum               instance number (not checked, checked in SPDO_BuildTxSpdo() or SPDO_ProcessRxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
 *
 * @param        w_rxSpdoIdx             Rx SPDO index (not checked, checked in SPDO_BuildTxSpdo() or ProcessTRes() and SPDO_GetSpdoIdxForSAdr()) valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param        ps_rxSpdoHeader         reference to TRes header info (pointer checked) valid range: <> NULL, == NULL
 *
 * @retval       pw_noFreeFrm            number of the free frames to be sent (pointer not checked,
 * 	checked in SPDO_BuildTxSpdo() or only called with NULL in ProcessTRes()) valid range: <> NULL, == NULL
 */
void SPDO_TimeSyncConsSm(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_rxSpdoIdx,
                         const EPLS_t_FRM_HDR *ps_rxSpdoHeader,
                         UINT16 *pw_noFreeFrm)
{
  t_RX_SYNC_CONS_SM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  t_RX_SYNC_CONS_SPDO *ps_rxSpdo = po_this->ps_rxSpdo + w_rxSpdoIdx; /* pointer
                                           to the internal Rx SPDO structure */

  /* if Rx SPDO is active */
  if (*ps_rxSpdo->s_rxCommPara.pw_sadr != k_NOT_USED_ADR)
  {
    /* switch for actual state of the state machine */
    switch (ps_rxSpdo->e_timeSyncConsState)
    {
      case k_STATE_SEND_TREQ:
      {
        /* if TRes received (function called by SPDO_ProcessRxSpdo)*/
        if (ps_rxSpdoHeader != NULL)
        {
          /* count the time response too late events */
          SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_TR_LATE);
          /* It can happened that a Time Response is received too late and the
             state machine is not in the state k_STATE_WF_TRES but tries to
             send the next Time Request block. For this case, minor error is
             generated. */
          SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TRES_TREQ, (UINT32)w_rxSpdoIdx);
        }
        /* else no TRes received (function called by SPDO_BuildTxSpdo) */
        else
        {
          /* if the TReq sending request was accomplished */
          if (SPDO_SendTxSpdo(B_INSTNUM_ dw_ct, SPDO_NUM_TO_INTERNAL_SPDO_IDX(
                              *ps_rxSpdo->s_rxCommPara.pw_txSpdoNo),
                              k_SENDING_TREQ, ps_rxSpdo->s_rxCtr.b_cntTr,
                              *ps_rxSpdo->s_rxCommPara.pw_sadr,
                              pw_noFreeFrm) == TRUE)
          {
            /* time request cycle timer is started */
            ps_rxSpdo->s_timers.dw_tReqCycle = (UINT32)(dw_ct +
                *ps_rxSpdo->s_rxCommPara.pdw_timeRequestCycle);

            /* the first TR is stored */
            ps_rxSpdo->b_firstTr = ps_rxSpdo->s_rxCtr.b_cntTr;

            IncTReqCounters(dw_ct, ps_rxSpdo);

            /* change state */
            ps_rxSpdo->e_timeSyncConsState = k_STATE_WF_TRES;
          }
          /* no else : the TReq sending request was not accomplished */
        }
        break;
      }
      case k_STATE_WF_TRES:
      {
        /* if Time request cycle time expired or
           the number of not answered TR expired */
        if (EPLS_TIMEOUT(dw_ct, ps_rxSpdo->s_timers.dw_tReqCycle) ||
            (ps_rxSpdo->s_rxCtr.w_noNotAnsweredTReq ==
            (UINT16)SPDO_cfg_NO_NOT_ANSWERED_TR))
        {
          /* if the number of not answered TR expired */
          if (ps_rxSpdo->s_rxCtr.w_noNotAnsweredTReq ==
              (UINT16)SPDO_cfg_NO_NOT_ANSWERED_TR)
          {
            /* count the number of TR expired events */
            SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_TR_EXP);

            ps_rxSpdo->s_rxCtr.w_noNotAnsweredTReq = 0U;
            SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TR_EXPIRED,
                          (UINT32)w_rxSpdoIdx);
          }
          /* else Time request cycle time expired */
          else
          {
            /* count the TR time 1 expired events */
            SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_TR_TIME1);
            SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TREQ_TIME_1,
                          (UINT32)w_rxSpdoIdx);
          }

          /* change state */
          ps_rxSpdo->e_timeSyncConsState = k_STATE_SEND_TREQ;

          /* Time synchronization failure */
          SPDO_ConsSm(B_INSTNUM_ dw_ct, w_rxSpdoIdx, (EPLS_t_FRM_HDR *)NULL,
                      (UINT8 *)NULL, TRUE);
        }
        /* else no Time synchronization failure */
        else
        {
          /* if maximum propagation delay expired for the last TReq */
          if (EPLS_TIMEOUT(dw_ct, ps_rxSpdo->s_timers.dw_maxProp) &&
              (ps_rxSpdo->s_rxCtr.b_noConsecutiveTReq >=
              *ps_rxSpdo->s_rxCommPara.pb_noConsecutiveTReq))
          {
            /* sentTReq is reset */
            ps_rxSpdo->s_rxCtr.b_noConsecutiveTReq = 0U;
            /* td Timer start */
            ps_rxSpdo->s_timers.dw_td = (UINT32)(dw_ct+
                *ps_rxSpdo->s_rxCommPara.pdw_timeDelayTReq);
            /* change state */
            ps_rxSpdo->e_timeSyncConsState = k_STATE_WF_NEXT_TREQ_BLOCK;
          }
          /* else wait for TReq */
          else
          {
            StateWaitForTres(B_INSTNUM_ dw_ct, w_rxSpdoIdx, ps_rxSpdoHeader,
                             pw_noFreeFrm);
          }
        }
        break;
      }
      case k_STATE_WF_NEXT_TREQ_BLOCK:
      {
        /* if no TRes received (function called by SPDO_BuildTxSpdo)*/
        if (ps_rxSpdoHeader == NULL)
        {
          /* if Time request cycle time expired */
          if (EPLS_TIMEOUT(dw_ct,ps_rxSpdo->s_timers.dw_tReqCycle))
          {
            /* count the time request cycle time expired events */
            SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_TR_TIME2);
            SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TREQ_TIME_2,
                          (UINT32)w_rxSpdoIdx);

            /* change state */
            ps_rxSpdo->e_timeSyncConsState = k_STATE_SEND_TREQ;

            /* Time synchronization failure */
            SPDO_ConsSm(B_INSTNUM_ dw_ct, w_rxSpdoIdx, (EPLS_t_FRM_HDR *)NULL,
                        (UINT8 *)NULL, TRUE);
          }
          /* else Time request cycle time did not expire */
          else
          {
            /* if td expired*/
            if (EPLS_TIMEOUT(dw_ct,ps_rxSpdo->s_timers.dw_td))
            {
              /* if the TReq sending request was accomplished */
              if (SPDO_SendTxSpdo(B_INSTNUM_ dw_ct,
                                  SPDO_NUM_TO_INTERNAL_SPDO_IDX(
                                  *ps_rxSpdo->s_rxCommPara.pw_txSpdoNo),
                                  k_SENDING_TREQ, ps_rxSpdo->s_rxCtr.b_cntTr,
                                  *ps_rxSpdo->s_rxCommPara.pw_sadr,
                                  pw_noFreeFrm) == TRUE)
              {
                /* the first TR is stored */
                ps_rxSpdo->b_firstTr = ps_rxSpdo->s_rxCtr.b_cntTr;

                IncTReqCounters(dw_ct, ps_rxSpdo);

                /* change state */
                ps_rxSpdo->e_timeSyncConsState = k_STATE_WF_TRES;
              }
              /* no else : the TReq sending request was not accomplished */
            }
            /* no else : td did not expire */
          }
        }
        /* else TRes received (function called by SPDO_ProcessRxSpdo) */
        else
        {
          /* count the time response too late events */
          SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_TR_LATE);
          SERR_SetError(B_INSTNUM_ SPDO_k_ERR_LATE_TRES, (UINT32)w_rxSpdoIdx);
        }
        break;
      }
      case k_STATE_WF_NEXT_TIME_SYNC:
      {
        /* if no TRes received (function called by SPDO_BuildTxSpdo) */
        if (ps_rxSpdoHeader == NULL)
        {
          /* if ts expired */
          if (EPLS_TIMEOUT(dw_ct,ps_rxSpdo->s_timers.dw_ts))
          {
            /* if the TReq sending request was accomplished */
            if (SPDO_SendTxSpdo(B_INSTNUM_ dw_ct, SPDO_NUM_TO_INTERNAL_SPDO_IDX(
                                *ps_rxSpdo->s_rxCommPara.pw_txSpdoNo),
                                k_SENDING_TREQ, ps_rxSpdo->s_rxCtr.b_cntTr,
                                *ps_rxSpdo->s_rxCommPara.pw_sadr,
                                pw_noFreeFrm))
            {
              /* time request cycle timer is started */
              ps_rxSpdo->s_timers.dw_tReqCycle = (UINT32)(dw_ct +
                  *ps_rxSpdo->s_rxCommPara.pdw_timeRequestCycle);

              /* the first TR is stored */
              ps_rxSpdo->b_firstTr = ps_rxSpdo->s_rxCtr.b_cntTr;

              IncTReqCounters(dw_ct, ps_rxSpdo);

              /* change state */
              ps_rxSpdo->e_timeSyncConsState = k_STATE_WF_TRES;
            }
            /* else the TReq sending request was not accomplished */
            else
            {
              /* change state */
              ps_rxSpdo->e_timeSyncConsState = k_STATE_SEND_TREQ;
            }
          }
          /* no else : ts did not expire */
        }
        /* no else : TRes received (function called by SPDO_ProcessRxSpdo) */
        break;
      }
      /* unknown state */
      default:
      {
        /* change state */
        ps_rxSpdo->e_timeSyncConsState = k_STATE_SEND_TREQ;
        SERR_SetError(B_INSTNUM_ SPDO_k_ERR_UNKNOWN_STATE, (UINT32)w_rxSpdoIdx);
        break;
      }
    }
  }
  /* no else : Rx SPDO is not active */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */

#pragma CTC SKIP

/**
* @brief Returns a pointer to the internal object. This function is only called by the unit test.
*
* @note This function is used for unit tests.
*
* @param        b_instNum        instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - == NULL - memory allocation failed
* - <> NULL - memory allocation and the mapping processing  succeeded, reference to the openSAFETY frame
*/
const t_RX_SYNC_CONS_SM_OBJ* SPDO_GetRxSyncConsSmObj(BYTE_B_INSTNUM)
{
  const t_RX_SYNC_CONS_SM_OBJ* ps_obj = (t_RX_SYNC_CONS_SM_OBJ *)NULL;
  #if (EPLS_cfg_MAX_INSTANCES > 1)
    /* check range of instance number */
    if(B_INSTNUMidx < (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      /* get pointer to object */
      ps_obj = &as_Obj[B_INSTNUMidx];
    }
    /* else - do nothing */
  #else
    ps_obj = &as_Obj[B_INSTNUMidx];
  #endif

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return ps_obj;
}
#pragma CTC ENDSKIP

/**
* @brief This function initializes the variables for the Time Synchronization Consumer State Machine.
*
* @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame() or SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        w_rxSpdoIdx        Rx SPDO index (not checked, checked in SPDO_ResetRxSm() or RxMappCommInit()) valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
*/
static void TimeSyncConsSmInit(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx)
{
  t_RX_SYNC_CONS_SM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  t_RX_SYNC_CONS_SPDO *ps_rxSpdo = po_this->ps_rxSpdo + w_rxSpdoIdx; /* pointer
                                           to the internal Rx SPDO structure */

  ps_rxSpdo->e_timeSyncConsState = k_STATE_SEND_TREQ;

  /* adw_ctForSentTReqs array is not initialized because it takes too much time
     at the state changed from Pre-operational to Operational. Otherwise the
     initialization of this array is not important: The elements of the
     array are set if a TReq is sent. The elements of the array are accessed
     only if the response for the TReq is received */

  ps_rxSpdo->b_firstTr = 0U;

  ps_rxSpdo->s_rxCtr.b_noConsecutiveTReq = 0U;
  ps_rxSpdo->s_rxCtr.b_cntTr             = 0U;
  ps_rxSpdo->s_rxCtr.w_noNotAnsweredTReq = 0U;

  ps_rxSpdo->s_timers.dw_tReqCycle = 0UL;
  ps_rxSpdo->s_timers.dw_td        = 0UL;
  ps_rxSpdo->s_timers.dw_ts        = 0UL;
  ps_rxSpdo->s_timers.dw_maxProp   = 0UL;

  /* calculate the reaction time */
  ps_rxSpdo->s_rxCommPara.dw_safeReactionTime =
          (UINT32)(*ps_rxSpdo->s_rxCommPara.pw_maxTSyncPropDelay - *ps_rxSpdo->s_rxCommPara.pw_bestCaseTResDelay) +
          *ps_rxSpdo->s_rxCommPara.pdw_sct;
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
* @brief This function sets the internal pointers to the Rx SPDO communication parameter  in the SOD.
*
* @param        b_instNum                    instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        w_index                      index of the SPDO communication parameter object (not checked, checked in InitRxSpdo()) valid range: k_RX_COMM_START_IDX.. k_RX_COMM_END_IDX
*
* @param        b_subIdx                     sub-index of the SPDO communication parameter object (checked), valid range: k_SIDX_RX_SADR .. k_SIDX_TIME_REQUEST_CYCLE
*
* @retval          ppv_internPointerAdr         pointer to address of the given Rx SPDO communication parameter
* 	 in the SOD (pointer not checked, only called with reference to pointer in SPDO_SetPtrToRxCommPara()) valid range: <> NULL
*
* @return
* - TRUE                       - success
* - FALSE                      - failure
*/
static BOOLEAN SetPtrToRxCommPara(BYTE_B_INSTNUM_ UINT16 w_index,
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
      switch (b_subIdx)
      {
        /* SADR : 0x1400-0x17FE sub 1 */
        case k_SIDX_RX_SADR:
        /* TxSPDONo : 0x1400-0x17FE sub 12 */
        case k_SIDX_TX_SPDO_NO:
        /* SCT : 0x1400-0x17FE sub 2 */
        case k_SIDX_SCT:
        /* NumberOfConsecutiveTReq : 0x1400-0x17FE sub 3 */
        case k_SIDX_NO_CONSECUTIVE_TREQ:
        /* TimeDelayTReq : 0x1400-0x17FE sub 4 */
        case k_SIDX_TIME_DELAY_TREQ:
        /* TimeDelaySync : 0x1400-0x17FE sub 5 */
        case k_SIDX_TIME_DELAY_SYNC:
        /* MinTSyncPropagationDelay : 0x1400-0x17FE sub 6 */
        case k_SIDX_MIN_T_SYNC_PROP_DELAY:
        /* MaxTSyncPropagationDelay : 0x1400-0x17FE sub 7 */
        case k_SIDX_MAX_T_SYNC_PROP_DELAY:
        /* MinSPDOPropagationDelay : 0x1400-0x17FE sub 8 */
        case k_SIDX_MIN_SPDO_PROP_DELAY:
        /* MaxSPDOPropagationDelay : 0x1400-0x17FE sub 9 */
        case k_SIDX_MAX_SPDO_PROP_DELAY:
        /* BestCaseTResDelay : 0x1400-0x17FE sub 10 */
        case k_SIDX_BEST_CASE_TRES_DELAY:
        /* TimeRequestCycle : 0x1400-0x17FE sub 11 */
        case k_SIDX_TIME_REQUEST_CYCLE:
        {
          /* if "before read" is set */
          if (EPLS_IS_BIT_SET(s_objAcs.s_obj.s_attr.w_attr, SOD_k_ATTR_BEF_RD))
          {
            SERR_SetError(B_INSTNUM_ SPDO_k_ERR_BEF_RD_RX,
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
          SERR_SetError(B_INSTNUM_ SPDO_k_ERR_RX_SPDO_OBJ,
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
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15
                              Notice #14 - 'case' conditions do not equal
                                           'break' */
}
  /* RSM_IGNORE_QUALITY_END */

/**
* @brief This function realizes the k_STATE_WF_TRES state.
*
* @param        b_instNum               instance number (not checked, checked in SPDO_BuildTxSpdo() or SPDO_ProcessRxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
*
* @param        w_rxSpdoIdx             Rx SPDO index (not checked, checked in SPDO_BuildTxSpdo() or ProcessTRes() and SPDO_GetSpdoIdxForSAdr()) valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
*
* @param        ps_rxSpdoHeader         reference to TRes header info (pointer not checked) valid range: <> NULL, == NULL
*
* @retval       pw_noFreeFrm            number of the free frames to be sent (pointer not checked, checked in
* 	SPDO_BuildTxSpdo() or only called with NULL in ProcessTRes()) valid range: <> NULL, == NULL
*/
static void StateWaitForTres(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_rxSpdoIdx,
                             const EPLS_t_FRM_HDR *ps_rxSpdoHeader,
                             UINT16 *pw_noFreeFrm)
{
  t_RX_SYNC_CONS_SM_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  t_RX_SYNC_CONS_SPDO *ps_rxSpdo = po_this->ps_rxSpdo + w_rxSpdoIdx;  /* pointer
                                            to the Rx SPDO internal structure */
  UINT32 dw_sentTReqCtTmp; /* temporary variable for the sent TReq CT */
  UINT32 dw_propDelay;     /* temporary variable for the propagation delay */
  UINT16 w_tRefCons; /* time reference of the consumer */

  /* if TRes received (function called by SPDO_ProcessRxSpdo) */
  if (ps_rxSpdoHeader != NULL)
  {
    /* if TR in the TRes is valid (see software documentation)*/
    if (IsTrValid(ps_rxSpdoHeader->b_tr, ps_rxSpdo->b_firstTr,
                  ps_rxSpdo->s_rxCtr.b_cntTr))
    {
      /* CT of the TReq was answered by TRes */
      dw_sentTReqCtTmp = ps_rxSpdo->adw_ctForSentTReqs[ps_rxSpdoHeader->b_tr];

      /* propagation delay = (Rx TRes CT) - (Tx TReq CT) */
      dw_propDelay = dw_ct - dw_sentTReqCtTmp;

#if (SPDO_cfg_PROP_DELAY_STATISTIC == EPLS_k_ENABLE)
#if(EPLS_cfg_MAX_INSTANCES > 1)
    if (B_INSTNUM == 0)
#endif
    {
        /* Formula for the low pass filter aka calculation of the average:
         * y[n] = (1-ALPHA)*x[n] + ALPHA*x[n-1],
         * ALPHA = 1/(PROP_DELAY_DIVISOR**2)       */
        SHNF_adwPropDelayStatistic[w_rxSpdoIdx] =
               (dw_propDelay >> PROP_DELAY_DIVISOR) +
               (SHNF_adwPropDelayStatistic[w_rxSpdoIdx] >> PROP_DELAY_DIVISOR);
    }
#endif

      /* if the time synchronization propagation delay is too short */
      if ( *ps_rxSpdo->s_rxCommPara.pw_minTSyncPropDelay > dw_propDelay )
      {
        /* count the time response propagation delay too short events */
        SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_TR_PROP_SHORT);
        SERR_SetError(B_INSTNUM_ SPDO_k_ERR_SHORT_TSYNC, dw_propDelay);

        /* sentTReq is reset */
        ps_rxSpdo->s_rxCtr.b_noConsecutiveTReq = 0U;

        /* change state */
        ps_rxSpdo->e_timeSyncConsState = k_STATE_SEND_TREQ;

        /* Time synchronization failure */
        SPDO_ConsSm(B_INSTNUM_ dw_ct, w_rxSpdoIdx, (EPLS_t_FRM_HDR *)NULL,
                    (UINT8 *)NULL, TRUE);
      }
      /* else if the time synchronization propagation delay is OK */
      else if ( (ps_rxSpdo->s_rxCommPara.dw_safeReactionTime + *ps_rxSpdo->s_rxCommPara.pw_bestCaseTResDelay) >= dw_propDelay )
      {
        w_tRefCons = (UINT16)((UINT16)(dw_sentTReqCtTmp) +
                      *ps_rxSpdo->s_rxCommPara.pw_bestCaseTResDelay);
        SPDO_ConsSmTimeSyncSucceeded(B_INSTNUM_ w_rxSpdoIdx, w_tRefCons,
                                     ps_rxSpdoHeader->w_ct,(UINT16)dw_propDelay);

        /* sentTReq is reset */
        ps_rxSpdo->s_rxCtr.b_noConsecutiveTReq = 0U;
        /* the number of not answered TR is reset */
        ps_rxSpdo->s_rxCtr.w_noNotAnsweredTReq = 0U;

        /* ts timer is started */
        ps_rxSpdo->s_timers.dw_ts = (UINT32)(dw_ct +
            *ps_rxSpdo->s_rxCommPara.pdw_timeDelaySync);

        /* change state */
        ps_rxSpdo->e_timeSyncConsState = k_STATE_WF_NEXT_TIME_SYNC;
      }
      /* else if the time synchronization propagation delay is too long */
      else
      {
        /* count the time response propagation delay too long events */
        SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_TR_PROP_LONG);
        /* propagation delay is too long */
        SERR_SetError(B_INSTNUM_ SPDO_k_ERR_LONG_TSYNC, dw_propDelay);
      }
    }
    /* else TR is not valid */
    else
    {
      /* count the invalid TR events */
      SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_TR_INV);
      SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TR_IS_INVALID,
                    (((UINT32)(w_rxSpdoIdx))<<16) + ps_rxSpdoHeader->b_tr);
    }
  }
  /* else TRes did not receive (function called by SPDO_BuildTxSpdo) */
  else
  {
    /* if sent TReq < m */
    if (ps_rxSpdo->s_rxCtr.b_noConsecutiveTReq <
         *ps_rxSpdo->s_rxCommPara.pb_noConsecutiveTReq)
    {
      /* if the TReq sending request was accomplished */
      if (SPDO_SendTxSpdo(B_INSTNUM_ dw_ct, SPDO_NUM_TO_INTERNAL_SPDO_IDX(
                          *ps_rxSpdo->s_rxCommPara.pw_txSpdoNo),
                          k_SENDING_TREQ, ps_rxSpdo->s_rxCtr.b_cntTr,
                          *ps_rxSpdo->s_rxCommPara.pw_sadr,
                          pw_noFreeFrm) == TRUE)
      {
        IncTReqCounters(dw_ct, ps_rxSpdo);
      }
      /* no else : the TReq sending request was not accomplished */
    }
    /* no else : TReq SPDO is not sent */
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
* @brief This function checks whether the TR in the received TRes is  valid or not.
*
* @param        b_tr                 TR counter from the received TRes. (not checked, checked in checkRxFrameHeader()) valid range: EPLS_k_TR_NOT_USED..EPLS_k_MAX_TR
*
* @param        b_firstSentTr        TR that was sent in the first TRes in the TRes block (not checked, checked in IncTReqCounters()) valid range: EPLS_k_TR_NOT_USED..EPLS_k_MAX_TR
*
* @param        b_lastSentTr         TR that was sent in the last TRes (not checked, checked in IncTReqCounters()) valid range: EPLS_k_TR_NOT_USED..EPLS_k_MAX_TR
*
* @return
* - TRUE               - TR is valid
* - FALSE              - TR is not valid
*/
static BOOLEAN IsTrValid(UINT8 b_tr, UINT8 b_firstSentTr, UINT8 b_lastSentTr)
{
  BOOLEAN o_ret = FALSE; /* return value */

  /* NOTE : b_lastSentTr == b_firstSentTr can never happen, otherwise
            TR is not valid */

  /* if the received TR is between the TR sent in the first TRes and the TR sent
        in the last TRes (no TR overflow) */
  if ((b_lastSentTr > b_firstSentTr) &&
      (b_firstSentTr <= b_tr) && (b_tr < b_lastSentTr))
  {
    o_ret = TRUE;
  }
  /* else if the received TR is between the TR sent in the last TRes and the TR
             sent in the first TRes (TR overflow) */
  else if ((b_lastSentTr < b_firstSentTr) &&
           ((b_firstSentTr <= b_tr) || (b_tr < b_lastSentTr)))
  {
    o_ret = TRUE;
  }
  /* else TR is not valid */
  else
  {
    o_ret = FALSE;
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/**
* @brief This function increments the counters for the TReq.
*
* @param   dw_ct                 consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
*
* @param    ps_rxSpdo            pointer to the Rx Spdo structure (pointer not checked, only called with reference to struct in SPDO_TimeSyncConsSm() or StateWaitForTres())
*/
static void IncTReqCounters(UINT32 dw_ct, t_RX_SYNC_CONS_SPDO *ps_rxSpdo)
{
  /* the actual ct of the TReq is stored */
  ps_rxSpdo->adw_ctForSentTReqs[ps_rxSpdo->s_rxCtr.b_cntTr] = dw_ct;

  /* TR is incremented */
  ps_rxSpdo->s_rxCtr.b_cntTr++;
  /* if 63 is reached by the TR */
  if (ps_rxSpdo->s_rxCtr.b_cntTr == k_MAX_NO_STORED_TR_FOR_TREQ)
  {
    ps_rxSpdo->s_rxCtr.b_cntTr = 0U;
  }
  /* no else : 63 is not reached by the TR */

  /* number of consecutive TReq counter (m) is incremented */
  ps_rxSpdo->s_rxCtr.b_noConsecutiveTReq++;
  /* number of not answered TReq is incremented */
  ps_rxSpdo->s_rxCtr.w_noNotAnsweredTReq++;

  /* if the last TReq in the block was sent */
  if (ps_rxSpdo->s_rxCtr.b_noConsecutiveTReq ==
      *ps_rxSpdo->s_rxCommPara.pb_noConsecutiveTReq)
  {
    /* maximum propagation delay timeout is set */
    ps_rxSpdo->s_timers.dw_maxProp = (UINT32)(dw_ct +
        ps_rxSpdo->s_rxCommPara.dw_safeReactionTime +
        *ps_rxSpdo->s_rxCommPara.pw_bestCaseTResDelay);
  }
  /* no else : there are TReq to send */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}
#else
  /* This file is compiled with different configuration (EPLScfg.h).
     If SPDO_cfg_MAX_NO_RX_SPDO is 0 then a compiler warning can be generated.
     To avoid this warning, remove this file from the project. */
#endif
/** @} */
