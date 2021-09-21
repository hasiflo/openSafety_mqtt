/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOint.h
 *
 * This file is the internal header-file of the unit.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * Each instance is assigned to one domain. The maximum number of entries in the list is limited to
 * &lt;EPLS_cfg_MAX_INSTANCES&gt. The domain number and instance can be requested from the module SDN.
 *
 * <h2>History for SPDOint.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>22.03.2011</td><td>Hans Pill</td><td>A&P257950 changes for different SPDO numbers of
 *         main instance and SDG instance</td></tr>
 *     <tr><td>05.04.2011</td><td>Hans Pill</td><td>A&P258030 added variable for reminding the CT
 *         of the last valid producer telegram and the last actual producer delta added variable to
 *         remind the propagation delay of the last sync</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 *     <tr><td>21.02.2013</td><td>Roman Zwischelsberger</td><td>(new)    A&P297705: t_SPDO_OBJ.aw_TxRxConn[]</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>03.07.2014</td><td>Hans Pill</td><td>changes for dynamic SCT</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>changes for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>17.11.2016</td><td>Stefan Innerhofer</td><td>changed one byte b_noFreeFrm to two byte w_noFreeFrm</td></tr>
 *     <tr><td>30.01.2017</td><td>Roman Zwischelsberger</td><td>support SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC</td></tr>
 * </table>
 *
 */

#ifndef SPDOINT_H
#define SPDOINT_H

/**
 * @name Definition for the dummy mapping indexes
 * @{
 */
/** minimum of the dummy index */
#define k_MIN_DUMMY_IDX 1U
/** maximum of the dummy index */
#define k_MAX_DUMMY_IDX 7U
/** @} */

/**
 * @def IS_DUMMY_INDEX
 *
 * The macro returns TRUE if the given index is dummy index otherwise returns
 *
 * @return
 * - TRUE if the given index is dummy index
 * - otherwise FALSE
 */
#define IS_DUMMY_INDEX(index) (!(((index)<k_MIN_DUMMY_IDX) || \
                                 ((index)>k_MAX_DUMMY_IDX)))

/**
 * @name Frame identifier definition of the SPDO frames.
 * @{
 */
/** Data only SPDO */
#define k_FRAME_ID_DATA_ONLY (UINT8)0x30
/** TReq SPDO */
#define k_FRAME_ID_TREQ      (UINT8)0x32
/** TRes SPDO */
#define k_FRAME_ID_TRES      (UINT8)0x34
/** @} */

/**
 * Connection valid bit
 */
#define k_FRAME_BIT_CONN_VALID (UINT8)0x01

#define k_FRAME_MASK_SPDO_TYPE (UINT8)(k_FRAME_ID_DATA_ONLY | k_FRAME_ID_TREQ | k_FRAME_ID_TRES)

/**
 * @name SPDO objects in the SOD
 * @{
 *
 * @brief Index definitions of the SPDO objects in the SOD
 *
 * SOD index definitions for the Tx and Rx SPDO mapping- and communication parameters.
 */
/** start index of the Tx comm. param. */
#define k_TX_COMM_START_IDX  0x1C00U
/** end index of the Tx comm. param. */
#define k_TX_COMM_END_IDX    0x1FFEU
/** start index of the Tx mapp param. */
#define k_TX_MAPP_START_IDX  0xC000U
/** end index of the Tx mapp. param. */
#define k_TX_MAPP_END_IDX    0xC3FEU
/** start index of the Rx comm. param. */
#define k_RX_COMM_START_IDX 0x1400U
/** end index of the Rx comm. param.  */
#define k_RX_COMM_END_IDX   0x17FEU
/** start index of the Rx mapp. param. */
#define k_RX_MAPP_START_IDX 0x1800U
/** end index of the Rx mapp. param. */
#define k_RX_MAPP_END_IDX   0x1BFEU
/** @} */

/**
 * This define represents the not used SPDO address
 */
#define k_NOT_USED_ADR 0U

/**
 * Invalid Tx SPDO number
 */
#define k_INVALID_SPDO_NUM ((UINT16)(0xFFFF))

/**
 * maximum number of Time Request counter to be stored for the sent TReq ([0..63] -> 63+1 -> k_MAX_TR+1)
 */
#define k_MAX_NO_STORED_TR_FOR_TREQ ((UINT8)(EPLS_k_MAX_TR + 1U))

/**
 * @def SPDO_NUM_TO_INTERNAL_SPDO_IDX
 *
 * Macro to convert the SPDO number in SOD to the SPDO index in the internal array of the Tx SPDO structures.
 */
#define SPDO_NUM_TO_INTERNAL_SPDO_IDX(spdoNum) ((UINT16)((spdoNum)-1U))

#if (SPDO_cfg_PROP_DELAY_STATISTIC == EPLS_k_ENABLE)
  /**
  * PROP_DELAY_DIVISOR defines the divisor for the SHNF_adwPropDelayStatistic
  * The resulting divisor is 1/(PROP_DELAY_DIVISOR**2)
  */
  #define PROP_DELAY_DIVISOR ((UINT8) 1)
#endif

/**
 * @name SPDO Data types
 * @{
 *
 * These data types are also used for the SPDO unit test
 */

/**
 * Enumeration of the sending request type.
 */
typedef enum
{
    /** no sending request */
    k_SENDING_FREE,
    /** TReq sending request */
    k_SENDING_TREQ,
    /** TRes sending request */
    k_SENDING_TRES,
    /** TRes immediate sending request */
    k_SENDING_TRES_IMMEDIATE,
    /** Data only sending request */
    k_SENDING_DATA_ONLY
} t_SENDING_REQ_TYPE;

/** Type definitions for the SPDOrxsm.c **/
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
/**
 * Enumeration of states of the time synchronization consumer.
 */
typedef enum
{
    /** send a Time Request */
    k_STATE_SEND_TREQ,
    /** wait for TRes */
    k_STATE_WF_TRES,
    /** wait for the next Time Synchronization */
    k_STATE_WF_NEXT_TIME_SYNC,
    /** wait for the next Time Request block */
    k_STATE_WF_NEXT_TREQ_BLOCK
}t_TIME_SYNC_CONS_STATES;

/**
 * Structure for the RX SPDO Communication parameters.
 */
typedef struct
{
    /** 0x1400-17FE sub 1 defines from which the RxSPDO data is to be received. */
    const UINT16 *pw_sadr;
    /** 0x1400-17FE sub 2 provides the SCT. */
    const UINT32 *pdw_sct;
    /** 0x1400-17FE sub 3 provides the number of consecutive time request */
    const UINT8 *pb_noConsecutiveTReq;
    /** 0x1400-17FE sub 4 provides the time delay between two sets of consecutive time requests */
    const UINT32 *pdw_timeDelayTReq;
    /** 0x1400-17FE sub 5 provides the time delay between successful time synchronization and the next time request */
    const UINT32 *pdw_timeDelaySync;

    /** 0x1400-17FE sub 6 provides the minimum allowed propagation delay between time request and time response. */
    const UINT16 *pw_minTSyncPropDelay;
    /** 0x1400-17FE sub 7 provides the maximum allowed propagation delay between time request and time response. */
    const UINT16 *pw_maxTSyncPropDelay;
    /** 0x1400-17FE sub 8 provides the minimum allowed SPDO propagation delay. */
    const UINT16 *pw_minSPDOPropDelay;
    /** 0x1400-17FE sub 10 provides the best case delay between sending the time request and setting up the time response */
    const UINT16 *pw_bestCaseTResDelay;

    /** 0x1400-17FE sub 11 provides the time request cycle time */
    const UINT32 *pdw_timeRequestCycle;

    /** 0x1400-17FE sub 12 contains the TxSPDO number with which the time request is to be sent */
    const UINT16 *pw_txSpdoNo;

    /** safe reaction time */
    UINT32 dw_safeReactionTime;
}t_RX_SYNC_CONS_COMM_PARAM;

/**
 * Structure with counter for the RX SPDO Time Synchronization Consumer State Machine.
 */
typedef struct
{
    /** number of consecutive time request counter */
    UINT8 b_noConsecutiveTReq;
    /** Time Request counter for the next TReq to be sent in the TReq block */
    UINT8 b_cntTr;
    /** number of not answered TReq counter*/
    UINT16 w_noNotAnsweredTReq;
}t_RX_SYNC_CONS_CTR;

/**
 * Structure with timers for the RX SPDO Time Synchronization Consumer State Machine.
 */
typedef struct
{
    /** time request cycle timer */
    UINT32 dw_tReqCycle;
    /** timer for the td (time delay between two sets of consecutive time requests) timer */
    UINT32 dw_td;
    /** timer for the ts (time delay between successful time synchronization and the next time request) */
    UINT32 dw_ts;
    /** timer for the maximum propagation delay */
    UINT32 dw_maxProp;
}t_RX_SYNC_CONS_TIMERS;

/**
 * Structure with variables for the RX SPDO parameters.
 */
typedef struct
{
    /** state of the Time Synchronization Consumer State Machine */
    t_TIME_SYNC_CONS_STATES e_timeSyncConsState;

    /** ct values for the sent TReqs to be stored */
    UINT32 adw_ctForSentTReqs[k_MAX_NO_STORED_TR_FOR_TREQ];
    /** the first TR in the TReq block */
    UINT8 b_firstTr;

    /** structure with pointers to the Rx SPDO communication parameters in the SOD */
    t_RX_SYNC_CONS_COMM_PARAM s_rxCommPara;
    /** counters for the Rx SPDOs */
    t_RX_SYNC_CONS_CTR s_rxCtr;
    /** timers for the Rx SPDOs */
    t_RX_SYNC_CONS_TIMERS s_timers;
}t_RX_SYNC_CONS_SPDO;

/**
 * Object structure for the Synchronization Consumer State Machine.
 */
typedef struct
{
    /** Rx SPDO structure */
    t_RX_SYNC_CONS_SPDO *ps_rxSpdo;
}t_RX_SYNC_CONS_SM_OBJ;

/**
 * Structure with variables for the Rx SPDO Consumer State Machine.
 */
typedef struct
{
    /** state of the SPDO Consumer State Machine */
    BOOLEAN o_safeState;

    /** last CT value of the last received Rx SPDO with valid data */
    UINT16 w_lastValidRxSpdoCt;
    /** TRUE if the last CT value has to be checked, otherwise ignored */
    BOOLEAN o_lastValidRxSpdoCtChk;

    /** TRUE if the time synchronization was successful, otherwise FALSE */
    BOOLEAN o_timeSyncOk;
    /** time reference of the consumer */
    UINT16 w_tRefCons;
    /** time reference of the producer */
    UINT16 w_tRefProd;
    /** delta CT of the producer */
    UINT16 w_dCtProd;
    /** propagation delay of the last time synchronization */
    UINT16 w_tPropDel;
    /** safe reaction time */
    const UINT32 *pdw_safeReactionTime;
    /** 0x1400-17FE sub 8 provides the minimum allowed SPDO propagation delay. */
    const UINT16 *pw_minSPDOPropDelay;
    /** 0x1400-17FE sub 9 provides the maximum allowed SPDO propagation delay. */
    const UINT16 *pw_maxSPDOPropDelay;

    /** 0x1400-17FE sub 2 defines the SCT timer for the data from this RxSPDO. */
    const UINT32 *pdw_sct;
    /** Safety Control timer (SCT) */
    UINT32 dw_sct;
    /** CT when the last RxSPDO got received */
    UINT32 dw_CtLastReceived;

#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
    /** extended CT of the producer */
    UINT64 ddw_CtProd;
    /** local timestamp of the last received extended CT */
    UINT32 dw_extCtLastReceived;
    /** extended CT value is available */
    BOOLEAN o_extCtAvail;
    /** extended CT value is used */
    BOOLEAN o_extCtUsed;
    /** extended CT valid */
    BOOLEAN o_extCtValid;
    /** extended CT error */
    BOOLEAN o_extCtError;
    /** extended CT status unknown */
    BOOLEAN o_extCtUnknown;
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */

}t_RX_CONS_SM;

#endif

/**
 * Control structure for the processing of the Time Request and the sending
 * of the Time Request.
 */
typedef struct
{
    /** state of the Time Synchronization Producer State Machine */
    BOOLEAN o_waitForTReq;

    /** memorizes the TR of the answered TReq */
    UINT8 b_echoTR;
    /** memorizes the target address of the answered TReq */
    UINT16 w_echoTAdr;
    /** signal the beginning of a TRes block */
    BOOLEAN o_startTResBlock;

    /** number of consecutive TRes for a received TReq */
    UINT8 b_noTRes;
    /**
     * Pointer to the SOD entry with 0x1C00-1FFE sub 3 that defines the number of
     * consecutive TRes for a received TReq.
     *
     * If the parameter is zero the TxSPDO can only be used for TReq telegrams
     * (for devices which are consumers only)
     */
    UINT8 *pb_noTRes;
} t_TX_SYNC_PROD_SM;

/**
 * Object structure for "Time Synchronization Producer" state machine.
 */
typedef struct
{
    /** sturcture array for the "Time Synchronization Producer" state machine */
    t_TX_SYNC_PROD_SM as_txSyncProdSm[SPDO_cfg_MAX_NO_TX_SPDO];
} t_TX_SYNC_PROD_SM_OBJ;

/**
 * Structure for the TX SPDO Communication parameters.
 */
typedef struct
{
    /**
     * 0x1C00-1FFE sub 1 defines the SADR that is used when broadcasting the data of
     * the corresponding TxSPDO within the network
     */
    UINT16 *pw_sadr;
    /**
     * 0x1C00-1FFE sub 2 provides the refresh time of the Producer */
    UINT16 *pw_refreshPrescale;
} t_TX_COMM_PARAM;

/**
 * Structure with variables for the TX SPDO processing.
 */
typedef struct
{
  #if ( SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC == EPLS_k_DISABLE )
    /** SPDO type for the sending request */
    t_SENDING_REQ_TYPE e_sendingReqType;
  #endif
    /** timeout for the delta t (SPDO cycle time) */
    UINT32 dw_timeoutDT;
    /** if TRUE then new Tx PDO data are available */
    BOOLEAN o_newData;
    /**
     * Flag for SPDO last Ct check
     *
     * - TRUE  : if the first SPDO was already sent and the dw_lastCt has to be checked
     * - FALSE : if the first SPDO was not sent yet and the dw_lastCt has not to be checked
     */
    BOOLEAN o_lastCtChk;
    /** memorizes the last CT value of the SPDO was sent */
    UINT32 dw_lastCt;
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    /**
     * Array of ptrs to the consume state machine of the RxSPDOs to be syncronized over the TxSPDO */
    t_RX_CONS_SM const * aps_consSm[SPDO_cfg_MAX_SYNC_RX_SPDO];
#endif
} t_VARIABLE_FOR_TX_SPDO;

/**
 * Control structure for the processing of the TX SPDOs
 */
typedef struct
{
    /**
     * Structures with pointers to the Tx SPDO communication parameters in the SOD
     */
    t_TX_COMM_PARAM s_txCommPara;
    /** variables for the Tx SPDOs */
    t_VARIABLE_FOR_TX_SPDO s_txVar;
} t_TX_SPDO;

/**
 * Object structure for the SPDO Producer state machine.
 */
typedef struct
{
    /** Tx SPDO structure */
    t_TX_SPDO as_txSpdo[SPDO_cfg_MAX_NO_TX_SPDO];
} t_TXSM_OBJ;

/**
 * Object structure for SPDOmain.c file.
 */
typedef struct
{
    /** to store whether SPDO is running or not */
    BOOLEAN o_spdoRunning;

    /** number of TX SPDOs (1..SPDO_cfg_MAX_NO_TX_SPDO) */
    UINT16 w_noTxSpdo;
    /** number of RX SPDOs (1..SPDO_cfg_MAX_NO_RX_SPDO) */
    UINT16 w_noRxSpdo;

    /** Rx SPDO index to be activated, counter for the SPDO activation */
    UINT16 w_actRxSpdoIdx;
    /** Tx SPDO index to be activated, counter for the SPDO activation */
    UINT16 w_actTxSpdoIdx;

#if (SPDO_cfg_MAX_SYNC_RX_SPDO == 1)
    /**
     * @todo only simple implementation up to now
     */
    /** list of RxSPDOs connected to TxSPDOs    */
    UINT16 aw_TxRxConn[SPDO_cfg_MAX_NO_TX_SPDO];
#endif
} t_SPDO_OBJ;

/** @} */

/**
 * @name Function prototypes for the SPDOinit.c
 * @{
 */

/**
 * @brief This function initializes the number of the Tx SPDOs and Rx SPDOs.
 *
 * This function initializes the number of the Tx SPDOs and Rx SPDOs defined in the SOD and calls the SPDO unit initialization functions.
 *
 * @param        b_instNum           instance number (not checked, checked in SSC_InitAll())
 *     valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @retval          pw_noTxSpdo         pointer to the number of the Tx SPDOs defined in the SOD.
 *     (pointer not checked, only called with reference to variable in SPDO_Init()) valid range: <> NULL
 *
 * @retval          pw_noRxSpdo         pointer to the number of the Rx SPDOs defined in the SOD.
 *     (pointer not checked, only called with reference to variable in SPDO_Init()) valid range: <> NULL
 *
 * @return
 * - TRUE              - success
 * - FALSE             - failure
 */
BOOLEAN SPDO_InitAll(BYTE_B_INSTNUM_ UINT16 *pw_noTxSpdo, UINT16 *pw_noRxSpdo);
/** @} */

/**
 * @name Function prototypes for the SPDOtxProdSm.c
 * @{
 */
/**
 * @brief This function clears the ptrs to the connected RxSPDOs
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame())
 * @param        w_txSpdoIdx        TxSPDO index (not checked, created in SPDO_ActivateTxSpdoMapping())
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
void SPDO_ClearConnectedRxSpdo(BYTE_B_INSTNUM_ UINT16 const w_txSpdoIdx);
#endif
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
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
BOOLEAN SPDO_SyncOkConnect(BYTE_B_INSTNUM_ UINT16 const w_txSpdoIdx, t_RX_CONS_SM const * const ps_consSm);
#endif

/**
 * @brief This function initializes structure for the communication parameters.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 */
void SPDO_InitTxSm(BYTE_B_INSTNUM);

/**
 * @brief This function initializes pointers of the t_TX_COMM_PARAM structure to the Tx communication parameters.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_txSpdoIdx        index in the internal array of the Tx SPDO structures (not
 *     checked, checked in TxMappCommInit()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 * @param        w_index            index of the SPDO communication parameter object (not checked,
 *     checked in InitTxSpdo()) valid range: k_TX_COMM_START_IDX..k_TX_COMM_END_IDX
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SPDO_SetPtrToTxCommPara(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx,
        UINT16 w_index);

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
void SPDO_ResetTxSm(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_noTxSpdo);

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
BOOLEAN SPDO_CheckTxCommPara(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx);

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
BOOLEAN SPDO_TxSpdoIdxExists(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx);

/**
 * @brief This function sets the new data flag for the given Tx SPDO.
 *
 * @param        b_instNum          instance number (not checked, checked in SPDO_TxDataChanged()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_txSpdoIdx        index in the internal array of the Tx SPDO structures (not checked, checked in SPDO_TxSpdoIdxExists()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 */
void SPDO_NewData(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx);

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
        UINT16 *pw_noFreeFrm);

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
BOOLEAN SPDO_SendTxSpdo(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_txSpdoIdx,
        t_SENDING_REQ_TYPE e_sendingReqType, UINT8 b_tr,
        UINT16 w_tAdr, UINT16 *pw_noFreeFrm);
/** @} */


/**
 * @name Function prototypes for the SPDOtxSyncProdSm.c
 * @{
 */
/**
 * @brief This function resets the given SPDO time synchronization producer state machine.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_txSpdoIdx        Tx SPDO index (not checked, checked in SPDO_ResetTxSm()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 */
void SPDO_TimeSyncProdSmReset(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx);

/**
 * @brief This function initializes the pointer to the number of time response SOD object (0x1C00-1FFE sub 3).
 *
 * @param    b_instNum          instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_txSpdoIdx        Tx SPDO index (not checked, checked in TxMappCommInit()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 * @param        pb_noTres          pointer to the number of time response entry (pointer not checked, only called with
 *     reference to variable in SPDO_SetPtrToTxCommPara()) valid range: <> NULL
 */
void SPDO_TimeSyncProdSmNoTresSet(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx,
        UINT8 *pb_noTres);

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
 *     pointer range : <> NULL, == NULL valid value range: > 0
 */
void SPDO_TimeSyncProdSm(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_txSpdoIdx,
        const EPLS_t_FRM_HDR *ps_rxSpdoHeader,
        UINT16 *pw_noFreeFrm);
/** @} */

/**
 * @name Function prototypes for the SPDOrxConsSm.c
 * @{
 */
/**
 * @brief This function connects a RxSPDO to the TxSPDO in which the synchronization request is sent.
 *
 * @param        b_instNum                  instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_rxSpdoIdx                index in the internal array of the Rx SPDO structures (not checked,
 *     created in SPDO_ActivateRxSpdoMapping()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @return
 * - TRUE if the operation was successful
 * - FALSE otherwise
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
BOOLEAN SPDO_ConsSmConnect(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx);
#endif
/**
 * @brief This function initializes pointers of the t_RX_COMM_PARAM structure to the Rx communication parameters and the
 * variables for the SPDO Consumer State Machine.
 *
 * @param        b_instNum                  instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_rxSpdoIdx                index in the internal array of the Rx SPDO structures (not checked, checked in RxMappCommInit()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param        pdw_sct                    pointer to the SCT in the SOD (not checked, only called with reference to variable in SPDO_SetPtrToRxCommPara()) valid range: <> NULL
 *
 * @param        pw_minSPDOPropDelay        pointer to the minimum prop. delay in the SOD
 *     (not checked, only called with reference to variable in SPDO_SetPtrToRxCommPara()) valid range: <> NULL
 *
 * @param        pw_maxSPDOPropDelay        pointer to the maximum prop. delay in the SOD
 *     (not checked, only called with reference to variable in SPDO_SetPtrToRxCommPara()) valid range: <> NULL
 *
 * @param        pdw_safeReactionTime       pointer to the safe reaction time
 *     (not checked, only called with reference to variable in SPDO_SetPtrToRxCommPara()) valid range: <> NULL
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
void SPDO_ConsSmInit(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx, const UINT32 *pdw_sct,
        UINT16 const * const pw_minSPDOPropDelay,
        UINT16 const * const pw_maxSPDOPropDelay,
        UINT32 const * const pdw_safeReactionTime);
#endif

/**
 * @brief This function resets the SPDO Consumer State Machines.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame() or SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_rxSpdoIdx        index in the internal array of the Rx SPDO structures
 *     (not checked, checked in RxMappCommInit() or SPDO_ResetRxSm()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
void SPDO_ConsSmResetRx(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx);
#endif

/**
 * @brief This function signals that the time synchronization for the SPDO Consumer State Machines succeeded.
 *
 * @param        b_instNum          instance number (not checked, checked in SPDO_ProcessRxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_rxSpdoIdx        index in the internal array of the Rx SPDO structures (not checked, checked in SPDO_GetSpdoIdxForSAdr() and ProcessTRes()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param        w_tRefCons         time reference of the consumer (not checked, any value allowed), valid range: (UINT16)
 *
 * @param        w_tRefProd         time reference of the producer (not checked, any value allowed), valid range: (UINT16)
 *
 * @param        w_tPropDelay       propagation delay of the synchronization (not checked, any value allowed), valid range: (UINT16)
 *
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
void SPDO_ConsSmTimeSyncSucceeded(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx,
        UINT16 w_tRefCons, UINT16 w_tRefProd,
        UINT16 w_tPropDelay);
#endif

/**
 * @brief Realization of the "SPDO Consumer" state machine.
 *
 * This state machine processes the payload data in the received SPDOs, checks the safety control timer and manages
 * the time synchronization failure.
 *
 * @param        b_instNum                instance number (not checked, checked in SPDO_CheckRxTimeout() or SPDO_BuildTxSpdo() or SPDO_ProcessRxSpdo()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_ct                    consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
 *
 * @param        w_rxSpdoIdx              Rx SPDO index (not checked, checked in SPDO_CheckRxTimeout() or SPDO_BuildTxSpdo() or ProcessTReq() or ProcessTRes() or SPDO_ProcessRxSpdo() and SPDO_GetSpdoIdxForSAdr()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param        ps_rxSpdoHeader          reference to RxSPDO header info (pointer checked) valid range: <> NULL, == NULL
 *
 * @param        pb_rxSpdoData            reference to the data of the RxSPDO (pointer not checked, only called with NULL in SPDO_CheckRxTimeout() or SPDO_TimeSyncConsSm() or StateWaitForTres(), checked in SPDO_ProcessRxSpdo()) valid range: <> NULL, == NULL
 *
 * @param        o_timeSyncFailure        TRUE : time synchronization failure FALSE : no time synchronization failure (checked) valid range: TRUE,FALSE
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
void SPDO_ConsSm(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_rxSpdoIdx,
        const EPLS_t_FRM_HDR *ps_rxSpdoHeader,
        const UINT8 *pb_rxSpdoData, BOOLEAN o_timeSyncFailure);
#endif

/**
 * @brief This function clears all valid connections
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 */
#if (SPDO_cfg_CONNECTION_VALID_BIT_FIELD == EPLS_k_ENABLE)
void SPDO_ResetConnectionValidBitField(BYTE_B_INSTNUM);
#endif
/**
 * @brief This function checks whether the CT in the received SPDO is valid or not.
 *
 * @param        b_instNum          instance number (not checked, checked in SPDO_ProcessRxSpdo()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_rxSpdoIdx        Rx SPDO index (not checked, checked in SPDO_ProcessRxSpdo() or ProcessTReq() or ProcessTRes() and SPDO_GetSpdoIdxForSAdr()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param        w_rxSpdoCt         received consecutive time (not checked, any value allowed),  valid range: UINT16
 *
 * @return
 * - TRUE             - CT is valid (changed and increased)
 * - FALSE            - CT is invalid
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
BOOLEAN SPDO_CtValid(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx, UINT16 w_rxSpdoCt);
#endif

/** @} */

/**
 * @name Function prototypes for the SPDOrxSyncConsSm.c
 * @{
 */

/**
 * @brief This function initializes the SPDO rx sync pointer array.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
void SPDO_InitRxSyncCons(BYTE_B_INSTNUM);
#endif
/**
* @brief This function gets the TxSPDO number for a RxSPDO.
*
* @param    b_instNum              instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param    w_rxSpdoIdx            index in the internal array of the Rx SPDO structures (not checked, checked in RxMappCommInit()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
*
* @return TxSPDO number
*/
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
UINT16 SPDO_GetTxSpdoNo(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx);
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
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
BOOLEAN SPDO_SetPtrToRxCommPara(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx,
        UINT16 w_index);
#endif

/**
 * @brief This function resets the state machines and the counters for the rx SPDOs.
 *
 * @param        b_instNum               instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_noRxSpdo              number of the Rx SPDOs (not checked, checked in RxMappCommInit()) valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
void SPDO_ResetRxSm(BYTE_B_INSTNUM_ UINT16 w_noRxSpdo);
#endif

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
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
BOOLEAN SPDO_CheckRxCommPara(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx);
#endif

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
 *     checked in SPDO_BuildTxSpdo() or only called with NULL in ProcessTRes()) valid range: <> NULL, == NULL
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
void SPDO_TimeSyncConsSm(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_rxSpdoIdx,
        const EPLS_t_FRM_HDR *ps_rxSpdoHeader,
        UINT16 *pw_noFreeFrm);
#endif
/** @} */


/**
 * @name Function prototypes for the SPDOtxmapp.c
 * @{
 */
/**
 * @brief This function initializes the pointers to the data of the mapped objects.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 */
void SPDO_InitTxMapp(BYTE_B_INSTNUM);

/**
 * @brief This function initializes the default mapping for one Tx SPDO.
 *
 * The mapping entries are read from the object dictionary. The mapped objects are checked and the reference
 * to these objects are stored into the internal SPDO structure.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_txSpdoIdx        index in the internal array of the Tx SPDO structures (not checked, checked in TxMappCommInit() or RxMappCommInit()) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 * @param        w_index            index of the SPDO communication parameter object (not checked, checked in InitTxSpdo() or InitRxSpdo()), valid range: k_TX_MAPP_START_IDX..k_TX_MAPP_END_IDX
 *
 * @param        b_noEntries        number of entries (checked) valid range: 0..SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SPDO_SetTxMapp(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx, UINT16 w_index,
        UINT8 b_noEntries);

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
UINT8 *SPDO_TxMappingProcess(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx, UINT8 *pb_len);

/**
 * @brief This function activates the SPDO mapping, assembles the byte pointer array for the
 * mapped objects and checks the length of the SPDO mapping.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_spdoIdx          SPDO index (not checked, checked in SPDO_ActivateTxSpdoMapping()) valid range: 0..(SPDO_cfg_MAX_NO_SPDO-1)
 *
 * @param        b_noEntries        number of entries (checked) valid range: 0..SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SPDO_TxMappActivate(BYTE_B_INSTNUM_ UINT16 w_spdoIdx,
        UINT8 b_noEntries);
/** @} */

/**
 * @name Function prototypes for the SPDOrxmapp.c
 * @{
 ***/
/**
 * @brief This function initializes the pointers to the data of the mapped objects.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
void SPDO_InitRxMapp(BYTE_B_INSTNUM);
#endif

/**
 * @brief This function initializes the default mapping for a Rx SPDO.
 *
 * The mapping entries are read from the object dictionary. The mapped objects are checked and
 * the reference to these objects are stored into the internal SPDO structure.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 * @param        w_rxSpdoIdx        index in the internal array of the Rx SPDO structures (not checked, checked in TxMappCommInit() or RxMappCommInit()) valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 * @param        w_index            index of the SPDO communication parameter object (not checked, checked in InitTxSpdo() or InitRxSpdo()), valid range: k_RX_MAPP_START_IDX..k_RX_MAPP_END_IDX
 * @param        b_noEntries        number of entries (checked) valid range: 0..SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
BOOLEAN SPDO_SetRxMapp(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx, UINT16 w_index,
        UINT8 b_noEntries);
#endif

/**
 * @brief This function sets all rx SPDO related data to the default value.
 *
 * @param        b_instNum          instance number (not checked, checked in SPDO_CheckRxTimeout() or SSC_ProcessSNMTSSDOFrame() or SPDO_BuildTxSpdo() and SPDO_ProcessRxSpdo()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_rxSpdoIdx        Rx SPDO index (not checked, checked in SPDO_CheckRxTimeout() or SPDO_BuildTxSpdo() or SPDO_ResetRxSm() ProcessTReq() or ProcessTRes() or SPDO_ProcessRxSpdo() and SPDO_GetSpdoIdxForSAdr()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
void SPDO_RxSpdoToSafeState(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx);
#endif

/**
 * @brief This function copies the SPDO data from the openSAFETY frame into the SOD.
 *
 * @param        b_instNum            instance number (not checked, checked in SPDO_CheckRxTimeout() or SPDO_BuildTxSpdo() and SPDO_ProcessRxSpdo()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_rxSpdoIdx          Rx SPDO index (not checked, checked in SPDO_CheckRxTimeout() or SPDO_BuildTxSpdo() or ProcessTReq() or ProcessTRes() or SPDO_ProcessRxSpdo() and SPDO_GetSpdoIdxForSAdr()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param        b_payloadSize        payload size (checked) valid value: length of the SPDO defined by the mapping (ps_rxSpdo->b_lenOfSpdo)
 *
 * @param        pv_data              pointer to the received data (pointer not checked, checked in SPDO_ProcessRxSpdo())  valid range: <> NULL
 *
 * @return
 * - TRUE               - payload size is right
 * - FALSE              - payload size is wrong
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
BOOLEAN SPDO_RxMappingProcess(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx,
        UINT8 b_payloadSize, const void *pv_data);
#endif

/**
 * @brief This function activates the SPDO mapping, assembles the byte pointer array for the mapped
 * objects and checks the length of the SPDO mapping.
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_spdoIdx          SPDO index (not checked, checked in SPDO_ActivateRxSpdoMapping()) valid range: 0..(SPDO_cfg_MAX_NO_SPDO-1)
 *
 * @param        b_noEntries        number of entries (checked) valid range: 0..SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SPDO_RxMappActivate(BYTE_B_INSTNUM_ UINT16 w_spdoIdx,
        UINT8 b_noEntries);
/** @} */

/**
 * @name Function prototypes for the SPDOassign.c
 * @{
 */
/**
 * @brief This function initializes the assignment tables for the SPDOs.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll() or SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 */
void SPDO_InitAssign(BYTE_B_INSTNUM);

/**
 * @brief This function insert a new source address with Rx SPDO index into the assignment array (Address 0 is ignored).
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_rxSpdoIdx        Rx SPDO index (not checked, checked in SPDO_ActivateRxSpdoMapping) valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param        w_sAdr             source SN address (checked) valid range: 0..(EPLS_k_MAX_SADR)
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure, SERR_SetError() is not called by this function, but a FATAL error must be reported by the calling function.
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
BOOLEAN SPDO_AddSAdr(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx, UINT16 w_sAdr);
#endif

/**
 * @brief This function inserts a new target address with Tx SPDO index into the assignment array (Address 0 is ignored).
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_txSpdoIdx        Tx SPDO index (not checked, checked in SPDO_ActivateTxSpdoMapping) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
 *
 * @param        w_tAdr             target SN address (checked) valid range: 0..(EPLS_k_MAX_SADR)
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure, SERR_SetError() is not called by this function, but a FATAL error must be reported by the calling function.
 */
BOOLEAN SPDO_AddTAdr(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx, UINT16 w_tAdr);

/**
 * @brief Gets the Rx SPDO index for the given source address
 *
 * @param        b_instNum            instance number (not checked, checked in SPDO_ProcessRxSpdo() or SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_sAdr               source SN address (not checked, checked in SPDO_AddTAdr() or checkRxAddrInfo()) valid range: 0..(EPLS_k_MAX_SADR)
 *
 * @return       0..1022            - valid Rx SPDO index   k_INVALID_SPDO_NUM - invalid SPDO index
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
UINT16 SPDO_GetSpdoIdxForSAdr(BYTE_B_INSTNUM_ UINT16 w_sAdr);
#endif

/**
 * @brief Gets the Tx SPDO index for the given target address
 *
 * @param        b_instNum            instance number (not checked, checked in SPDO_ProcessRxSpdo() or SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_tAdr               target SN address (not checked, checked in SPDO_AddSAdr() or checkRxAddrInfo()) valid range: 0..(EPLS_k_MAX_SADR)
 *
 * @return       0..1022            - valid Tx SPDO index  k_INVALID_SPDO_NUM - invalid SPDO index
 */
UINT16 SPDO_GetSpdoIdxForTAdr(BYTE_B_INSTNUM_ UINT16 w_tAdr);

/**
 * @brief This function gets the number of Rx SPDOs to be processed.
 *
 * @param        b_instNum            instance number (not checked, checked in SPDO_CheckRxTimeout() or SPDO_BuildTxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return       number of Rx SPDOs to be processed
 */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
UINT16 SPDO_GetNoProcRxSpdo(BYTE_B_INSTNUM);
#endif

/**
 * @brief This function gets the number of Tx SPDOs to be processed.
 *
 * @param        b_instNum            instance number (not checked, checked in SPDO_BuildTxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return       number of Tx SPDOs to be processed
 */
UINT16 SPDO_GetNoProcTxSpdo(BYTE_B_INSTNUM);

/** @} */

/**
 * @name Function prototypes for the unit test
 * @{
 */
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
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
const t_RX_SYNC_CONS_SM_OBJ* SPDO_GetRxSyncConsSmObj(BYTE_B_INSTNUM);
#endif

/**
* @brief Returns a pointer to the internal object. This function is only called by the unit test.
*
* @note This function is used for unit tests.
*
* @param        b_instNum          instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param        w_rxSpdoIdx        Rx SPDO index (not checked) valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
*
* @return
* - == NULL - memory allocation failed
* - <> NULL - memory allocation and the mapping processing succeeded, reference to the openSAFETY frame
*/
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
const t_RX_CONS_SM* SPDO_GetRxConsSmObj(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx);
#endif

/**
* @brief Returns a pointer to the internal object of unit SPDOmain.c Only for unit test.
*
* @note This function is used for unit tests.
*
* @param        b_instNum        instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - == NULL - memory allocation failed
* - <> NULL - memory allocation and the mapping processing succeeded, reference to the openSAFETY frame
*/
const t_SPDO_OBJ* SPDO_GetMainObj(BYTE_B_INSTNUM);

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
const t_TXSM_OBJ* SPDO_GetTxProdSmObj(BYTE_B_INSTNUM);

/**
* @brief Returns a pointer to the internal object
*
* @param        b_instNum        instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - == NULL - function failed
* - <> NULL - function succeeded, reference to the internal object
*/
const t_TX_SYNC_PROD_SM_OBJ* SPDO_GetTxSyncProdSmObj(BYTE_B_INSTNUM);


#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
/**
 * extended CT SPDO is available
 */
#define k_TR_EXT_CT_AVAILABLE    (UINT8)0x20
/**
 * extended CT SPDO is used
 */
#define k_TR_EXT_CT_USED         (UINT8)0x10
/**
 * extended CT bit mask
 */
#define k_TR_EXT_CT_MASK         (k_TR_EXT_CT_AVAILABLE | k_TR_EXT_CT_USED)
/**
 * Structure for the extended CT variables.
 */
typedef struct
{
  /** actual 64 bit CT value */
    UINT64 ddw_actCt;
  /** initial 64 bit CT value */
    UINT64 ddw_initValue;
  /** last 32 bit CT value */
    UINT32 dw_lastCt;
  /** usage of the 64 bit value is allowed */
    BOOLEAN o_allowed;
}t_EXT_CT;

/**
* @brief This function initializes the extended CT counters.
*
* @param        b_instNum        instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
*/
void SPDO_InitExtCt(BYTE_B_INSTNUM);

/**
* @brief This function is used to get a reference to the extended CT counters of the given instance.
*
* @param        b_instNum        instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - t_EXT_CT* - reference to the variables
*/
t_EXT_CT* SPDO_GetExtCt(BYTE_B_INSTNUM);

#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */
/** @} */
#endif

/** @} */
