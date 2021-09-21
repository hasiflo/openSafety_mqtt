/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOerr.h
 *
 * This file contains the error codes for the SPDO module.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SPDOerr.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>08.04.2011</td><td>Hans Pill</td><td>A&P258030 added errors for new SPDO function</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>new error codes for SPDOs with 40 bit CT</td></tr>
 * </table>
 *
 */

#ifndef SPDOERR_H
#define SPDOERR_H

/***
*    constants and macros
***/
/* SPDO macro to build error code */
#define SPDO_ERR_FATAL(num) SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, \
                                             SPDO_k_UNIT_ID, (num))
#define SPDO_ERR_MINOR(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, \
                                             SERR_CLASS_MINOR, \
                                             SPDO_k_UNIT_ID, (num))
#define SPDO_ERR__INFO(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_INFO,\
                                             SPDO_k_UNIT_ID, (num))

/* SPDO-error-codes:
    Error codes for the SPDO module.
*/
/* SPDO_CheckRxTimeout */
#define SPDO_k_ERR_INST_NUM_1          SPDO_ERR_FATAL(1)

/* SPDO_BuildTxSpdo */
#define SPDO_k_ERR_INST_NUM_2          SPDO_ERR_FATAL(2)
#define SPDO_k_ERR_FREE_FRAME_PTR_1    SPDO_ERR_FATAL(3)

/* SPDO_ProcessRxSpdo */
#define SPDO_k_ERR_INST_NUM_4          SPDO_ERR_FATAL(4)
#define SPDO_k_ERR_RX_FRAME_PTR        SPDO_ERR_FATAL(5)
#define SPDO_k_ERR_UNKNOWN_SDN         SPDO_ERR__INFO(6)
#define SPDO_k_ERR_INVALID_ID          SPDO_ERR_FATAL(7)
#define SPDO_k_ERR_FRAME_LEN           SPDO_ERR_FATAL(8)

/* SPDO_TxDataChanged */
#define SPDO_k_ERR_INST_NUM_3          SPDO_ERR_FATAL(9)
#define SPDO_k_ERR_TX_SPDO_NUM_4       SPDO_ERR_FATAL(10)

/* InitTxSpdo */
#define SPDO_k_ERR_TX_SPDO_GAP         SPDO_ERR_FATAL(11)
#define SPDO_k_ERR_NO_TX_COMM_PARA     SPDO_ERR_FATAL(12)
#define SOD_k_ERR_MAN_TX_SPDO          SPDO_ERR_FATAL(13)

/* TxMappCommInit */
#define SPDO_k_ERR_NO_TX_SPDOS         SPDO_ERR_FATAL(14)
#define SPDO_k_ERR_BEF_WR_TX_SUB_0     SPDO_ERR_FATAL(15)

/* InitRxSpdo */
#define SPDO_k_ERR_RX_SPDO_GAP         SPDO_ERR_FATAL(16)
#define SPDO_k_ERR_NO_RX_COMM_PARA     SPDO_ERR_FATAL(17)

/* RxMappCommInit */
#define SPDO_k_ERR_NO_RX_SPDOS         SPDO_ERR_FATAL(18)
#define SPDO_k_ERR_BEF_WR_RX_SUB_0     SPDO_ERR_FATAL(19)

/* SPDO_CheckTxCommPara */
#define SPDO_k_ERR_TADR                SPDO_ERR_FATAL(20)

/* SPDO_TimeSyncProdSm */
#define SPDO_k_ERR_TREQ_PROC           SPDO_ERR_FATAL(21)

/* SPDO_SendTxSpdo */
#define SPDO_k_ERR_SEND_TX_SPDO_1      SPDO_ERR_FATAL(22)
#define SPDO_k_ERR_SEND_TX_SPDO_2      SPDO_ERR_FATAL(23)
#define SPDO_k_ERR_NULL_PTR            SPDO_ERR_FATAL(24)

/* SetPtrToTxCommPara */
#define SPDO_k_ERR_BEF_RD_TX           SPDO_ERR_FATAL(25)
#define SPDO_k_ERR_TX_SPDO_OBJ         SPDO_ERR_FATAL(26)

/* SPDO_CheckRxCommPara */
#define SPDO_k_ERR_SADR                SPDO_ERR_FATAL(27)

/* SPDO_TimeSyncConsSm */
#define SPDO_k_ERR_TRES_TREQ           SPDO_ERR_MINOR(28)
#define SPDO_k_ERR_TR_EXPIRED          SPDO_ERR_MINOR(29)
#define SPDO_k_ERR_TREQ_TIME_1         SPDO_ERR_MINOR(30)
#define SPDO_k_ERR_TREQ_TIME_2         SPDO_ERR_MINOR(31)
#define SPDO_k_ERR_LATE_TRES           SPDO_ERR_MINOR(32)
#define SPDO_k_ERR_UNKNOWN_STATE       SPDO_ERR_FATAL(33)

/* SPDO_ConsSm */
#define SPDO_k_ERR_SCT_TIMER           SPDO_ERR_MINOR(34)

/* ProcessData */
#define SPDO_k_ERR_DELAY_SHORT         SPDO_ERR_MINOR(35)
#define SPDO_k_ERR_DELAY_LONG          SPDO_ERR__INFO(36)

/* SetPtrToRxCommPara */
#define SPDO_k_ERR_BEF_RD_RX           SPDO_ERR_FATAL(37)
#define SPDO_k_ERR_RX_SPDO_OBJ         SPDO_ERR_FATAL(38)

/* StateWaitForTres */
#define SPDO_k_ERR_SHORT_TSYNC         SPDO_ERR_MINOR(39)
#define SPDO_k_ERR_LONG_TSYNC          SPDO_ERR__INFO(40)
#define SPDO_k_ERR_TR_IS_INVALID       SPDO_ERR_MINOR(41)

/* SPDO_SetTxMapp */
#define SPDO_k_ERR_BEF_WR_TX_SUB_X     SPDO_ERR_FATAL(42)
#define SPDO_k_ERR_TX_MAPP_ENTRY_1     SPDO_ERR_FATAL(43)
#define SPDO_k_ERR_TX_MAP_OBJ          SPDO_ERR_FATAL(44)  /* (0604 0041h) */

/* SPDO_SetRxMapp */
#define SPDO_k_ERR_BEF_WR_RX_SUB_X     SPDO_ERR_FATAL(45)
#define SPDO_k_ERR_RX_MAPP_ENTRY_1     SPDO_ERR_FATAL(46)
#define SPDO_k_ERR_RX_MAP_OBJ          SPDO_ERR_FATAL(47)  /* (0604 0041h) */

/* SPDO_RxMappingProcess */
#define SPDO_k_ERR_PAYLOAD_SIZE        SPDO_ERR_MINOR(48)

/* SPDO_TxMappActivate */
#define SPDO_k_ERR_TX_TOO_LONG         SPDO_ERR_FATAL(49)
#define SPDO_k_ERR_TX_OBJ_MAPP_1       SPDO_ERR_FATAL(50)
#define SPDO_k_ERR_TX_MAPP_ENTRY_3     SPDO_ERR_FATAL(51)

/* SPDO_RxMappActivate */
#define SPDO_k_ERR_RX_TOO_LONG         SPDO_ERR_FATAL(52)
#define SPDO_k_ERR_RX_OBJ_MAPP_1       SPDO_ERR_FATAL(53)
#define SPDO_k_ERR_RX_MAPP_ENTRY_3     SPDO_ERR_FATAL(54)

/* SPDO_SOD_TxMappPara_CLBK */
#define SPDO_k_ERR_TX_SOD_SRV          SPDO_ERR__INFO(55)

/* SPDO_SOD_RxMappPara_CLBK */
#define SPDO_k_ERR_RX_SOD_SRV          SPDO_ERR__INFO(56)

/* SPDO_TxSpdoIdxExists */
#define SPDO_k_ERR_TX_SPDO_NUM_1       SPDO_ERR_FATAL(57)
#define SPDO_k_ERR_TX_SPDO_NUM_2       SPDO_ERR_FATAL(58)
#define SPDO_k_ERR_TX_SPDO_NUM_3       SPDO_ERR_FATAL(59)

/* SPDO_SyncOkConnect */
#define SPDO_k_ERR_TX_NO_RX_CONN_1     SPDO_ERR_FATAL(60)

/* SPDO_GetRxSpdoStatus */
#define SPDO_k_ERR_INST_INV            SPDO_ERR_FATAL(61)
#define SPDO_k_ERR_RX_SPDO_IDX         SPDO_ERR_FATAL(62)
#define SPDO_k_ERR_PTR_INV             SPDO_ERR_FATAL(63)

/* SPDO_UpdateExtCtValue */
#define SPDO_k_ERR_INST_NUM_5          SPDO_ERR_FATAL(64)

/* ProcessData */
#define SPDO_k_ERR_DELTA_EXT_CT        SPDO_ERR__INFO(65)

/* Invalid index of Tx SPDO structure */
#define SPDO_K_ERR_INV_INDEX           SPDO_ERR_FATAL(66)

/***
*    data types
***/

/***
*    global variables
***/

/***
*    function prototypes
***/
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param      : w_errorCode        error number dw_addInfo         additional error information pac_str            empty buffer to build the error string
*/
#if (EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
  void SPDO_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str);
#endif

#endif

/** @} */
