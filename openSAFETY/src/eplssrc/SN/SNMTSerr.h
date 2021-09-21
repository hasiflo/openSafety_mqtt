/**
 * @addtogroup SNMTS
 * @{
 *
 * @file SNMTSerr.h
 *
 * This file contains all defines and data types used for error handling.
 * Every error recognized by this unit is defined in here.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SNMTSerr.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>24.06.2014</td><td>Hans Pill</td><td>added error code for op2op callback function</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>added error code for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>26.07.2018</td><td>Stefan Innerhofer</td><td>change SNMTS_k_ERR_EVENT_UNDEF1 to SNMTS_k_ERR_EVENT_UNDEF12 from fatal to minor errors</td></tr>
 * </table>
 */

#ifndef SNMTSERR_H
#define SNMTSERR_H

/**
 * @name SNMTS macro to build error codes
 * @{
 */
#define SNMTS_ERR_FATAL(num) SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, \
                                              SNMTS_k_UNIT_ID, (num))
#define SNMTS_ERR_MINOR(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, \
                                              SERR_CLASS_MINOR, \
                                              SNMTS_k_UNIT_ID, (num))
#define SNMTS_ERR__INFO(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, \
                                              SERR_CLASS_INFO,\
                                              SNMTS_k_UNIT_ID, (num))
/** @} */

/**
 * @name SNMTSmain.c Function error codes
 * @{
 */
/* SNMTS_TimerCheck() */
#define SNMTS_k_ERR_TIMER_CHK_INST_INV           SNMTS_ERR_FATAL(1)
#define SNMTS_k_ERR_REF_MNGT_FRMS_TC_INV         SNMTS_ERR_FATAL(2)
#define SNMTS_k_ERR_NO_MNGT_FRMS_AVAIL           SNMTS_ERR__INFO(3)
/* SNMTS_PassParamChkSumValid() */
#define SNMTS_k_ERR_PASS_PARAM_INST_INV          SNMTS_ERR_FATAL(4)
/* SNMTS_EnterOpState() */
#define SNMTS_k_ERR_ENTER_OP_INST_INV            SNMTS_ERR_FATAL(5)
#define SNMTS_k_ERR_ERR_GRP_INV                  SNMTS_ERR_FATAL(6)
/* SNMTS_PerformTransPreOp() */
#define SNMTS_k_ERR_PERF_TRANS_PREOP_INST_INV    SNMTS_ERR_FATAL(7)
/** @} */

/**
 * @name SNMTSprocess.c Function error codes
 * @{
 */
/* processRequest() */
#define SNMTS_k_ERR_LE_UDID_REQ_INV              SNMTS_ERR_MINOR(8)
#define SNMTS_k_ERR_EXT_REQ_LEN                  SNMTS_ERR_MINOR(9)
#define SNMTS_k_ERR_RX_FRMID_REQ_PROC_INV        SNMTS_ERR_MINOR(10)
/* processRequestExt() */
#define SNMTS_k_ERR_LE_SET_TO_PREOP_INV          SNMTS_ERR_MINOR(11)
#define SNMTS_k_ERR_LE_SET_TO_OP_INV             SNMTS_ERR_MINOR(12)
#define SNMTS_k_ERR_LE_SCM_SET_TO_STOP_INV       SNMTS_ERR_MINOR(13)
#define SNMTS_k_ERR_LE_SCM_SET_TO_OP_INV         SNMTS_ERR_MINOR(14)
#define SNMTS_k_ERR_LE_GUARD_INV                 SNMTS_ERR_MINOR(15)
#define SNMTS_k_ERR_LE_ADD_SADR_ASS_INV          SNMTS_ERR_MINOR(16)
#define SNMTS_k_ERR_ADD_SADR_NOT_SUPPO           SNMTS_ERR__INFO(17)
#define SNMTS_k_ERR_LE_ERR_ACK_INV               SNMTS_ERR_MINOR(18)
#define SNMTS_k_ERR_LE_UDID_SCM                  SNMTS_ERR_MINOR(19)
#define SNMTS_k_ERR_REQ_EXT_FRMTYP_INV1          SNMTS_ERR_MINOR(20)
/* checkRequest() */
#define SNMTS_k_ERR_CT_FIELD_NOT_UNUSED          SNMTS_ERR__INFO(21)
#define SNMTS_k_ERR_TR_FIELD_NOT_UNUSED          SNMTS_ERR__INFO(22)
/* filterServReq() */
#define SNMTS_k_ERR_REQ_REJ_INV_DSADR            SNMTS_ERR__INFO(23)
/* checkRxUdid() */
#define SNMTS_k_ERR_LE_SADR_ASS_INV              SNMTS_ERR_MINOR(24)
#define SNMTS_k_ERR_RX_UDID_UNEVEN               SNMTS_ERR__INFO(25)
/** @} */

/**
 * @name SNMTSsnstate.c Function error codes
 * @{
 */
/* SNMTS_GetSnState() */
#define SNMTS_k_ERR_GET_SN_STATE_INST_INV        SNMTS_ERR_FATAL(26)
#define SNMTS_k_ERR_PTR_INV                      SNMTS_ERR_FATAL(27)
/* SNMTS_SnStateFsm() */
#define SNMTS_k_ERR_SN_STATE_MAIN_INV            SNMTS_ERR_FATAL(28)
/* InitializationState() */
#define SNMTS_k_ERR_EVENT_UNDEF1                 SNMTS_ERR_MINOR(29)
/* WfSadrAssStep1State() */
#define SNMTS_k_ERR_EVENT1                       SNMTS_ERR_MINOR(30)
#define SNMTS_k_ERR_EVENT15                      SNMTS_ERR_MINOR(31)
#define SNMTS_k_ERR_EVENT2                       SNMTS_ERR_MINOR(32)
#define SNMTS_k_ERR_EVENT13                      SNMTS_ERR_MINOR(33)
#define SNMTS_k_ERR_EVENT14                      SNMTS_ERR_MINOR(34)
#define SNMTS_k_ERR_EVENT_UNDEF2                 SNMTS_ERR_MINOR(35)
/* WfSadrAssStep2State() */
#define SNMTS_k_ERR_EVENT27                      SNMTS_ERR_MINOR(36)
#define SNMTS_k_ERR_EVENT_UNDEF3                 SNMTS_ERR_MINOR(37)
/* WfUdidScmAssState() */
#define SNMTS_k_ERR_EVENT16                      SNMTS_ERR_MINOR(38)
#define SNMTS_k_ERR_EVENT17                      SNMTS_ERR_MINOR(39)
#define SNMTS_k_ERR_EVENT18                      SNMTS_ERR_MINOR(40)
#define SNMTS_k_ERR_EVENT19                      SNMTS_ERR_MINOR(41)
#define SNMTS_k_ERR_EVENT_UNDEF12                SNMTS_ERR_MINOR(42)
/* WfSetToOp1Step1State() */
#define SNMTS_k_ERR_EVENT20                      SNMTS_ERR_MINOR(43)
#define SNMTS_k_ERR_EVENT_UNDEF4                 SNMTS_ERR_MINOR(44)
/* WfSetToOp1Step2State() */
#define SNMTS_k_ERR_EVENT21                      SNMTS_ERR_MINOR(45)
#define SNMTS_k_ERR_EVENT_UNDEF5                 SNMTS_ERR_MINOR(46)
/* WfApiChksumState() */
#define SNMTS_k_ERR_EVENT3                       SNMTS_ERR_MINOR(47)
#define SNMTS_k_ERR_EVENT9                       SNMTS_ERR_MINOR(48)
#define SNMTS_k_ERR_EVENT22                      SNMTS_ERR_MINOR(49)
#define SNMTS_k_ERR_EVENT_UNDEF6                 SNMTS_ERR_MINOR(50)
/* WfSetToOp2State() */
#define SNMTS_k_ERR_EVENT4                       SNMTS_ERR_MINOR(51)
#define SNMTS_k_ERR_EVENT10                      SNMTS_ERR_MINOR(52)
#define SNMTS_k_ERR_EVENT23                      SNMTS_ERR_MINOR(53)
#define SNMTS_k_ERR_EVENT_UNDEF7                 SNMTS_ERR_MINOR(54)
/* WfApiConfState() */
#define SNMTS_k_ERR_EVENT5                       SNMTS_ERR_MINOR(55)
#define SNMTS_k_ERR_EVENT11                      SNMTS_ERR_MINOR(56)
#define SNMTS_k_ERR_EVENT24                      SNMTS_ERR_MINOR(57)
#define SNMTS_k_ERR_EVENT_UNDEF8                 SNMTS_ERR_MINOR(58)
/* WfSetToOp3State() */
#define SNMTS_k_ERR_EVENT6                       SNMTS_ERR_MINOR(59)
#define SNMTS_k_ERR_EVENT12                      SNMTS_ERR_MINOR(60)
#define SNMTS_k_ERR_EVENT25                      SNMTS_ERR_MINOR(61)
#define SNMTS_k_ERR_EVENT_UNDEF9                 SNMTS_ERR_MINOR(62)
/* OperationStep1State() */
#define SNMTS_k_ERR_EVENT7                       SNMTS_ERR_MINOR(63)
#define SNMTS_k_ERR_EVENT8                       SNMTS_ERR_MINOR(64)
#define SNMTS_k_ERR_EVENT_UNDEF10                SNMTS_ERR_MINOR(65)
/* OperationStep2State() */
#define SNMTS_k_ERR_EVENT_UNDEF11                SNMTS_ERR_MINOR(66)
/* checkParam() */
#define SNMTS_k_ERR_RX_TX_BUFF_NULL              SNMTS_ERR_FATAL(67)
/* enterAddSadr() */
#define SNMTS_k_ERR_TXSPDO_NUM_INV               SNMTS_ERR_MINOR(68)
#define SNMTS_k_ERR_SPDO_NUM                     SNMTS_ERR_MINOR(69)
/** @} */

/**
 * @name SNMTSappInfo.c Function error codes
 * @{
 */
/* SNMTS_AppChkSumComp() */
#define SNMTS_k_ERR_PARAM_CHKSUM_INV             SNMTS_ERR_MINOR(70)
#define SNMTS_k_ERR_PARAM_TIMESTAMP_INV          SNMTS_ERR_MINOR(71)
/** @} */

/**
 * @name SNMTSlifetime.c Function error codes
 * @{
 */
/* SNMTS_LifeTimeCheck() */
#define SNMTS_k_ERR_TIMER                        SNMTS_ERR_FATAL(72)
#define SNMTS_k_ERR_LIFE_TIME_ELAPSED            SNMTS_ERR__INFO(73)
/** @} */

/**
 * @name SNMTSrefresh.c Function error codes
 * @{
 */
/* SNMTS_RefreshTimeCheck() */
#define SNMTS_k_ERR_REFR_TIME_ELAPSED            SNMTS_ERR__INFO(74)
/** @} */

/**
 * @name SNMTSresponse.c Function error codes
 * @{
 */
/* SNMTS_SnAckRespCheck() */
#define SNMTS_k_ERR_SN_ACK1                      SNMTS_ERR_FATAL(75)
#define SNMTS_k_ERR_SN_ACK2                      SNMTS_ERR_FATAL(76)
/** @} */

#if (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE)
/**
 * @name SNMTSsnstate.c Function error codes
 * @{
 */
/* OperationStep1State() */
#define SNMTS_k_ERR_INV_RET                      SNMTS_ERR_FATAL(77)
/** @} */
#endif /* (EPLS_cfg_SAPL_REPORT_STATE_CHANGE == EPLS_k_ENABLE) */

/**
 * @name SNMTSprocess.c Function error codes
 * @{
 */
/* processRequestExt() */
#define SNMTS_k_ERR_LE_EXT_CT                    SNMTS_ERR_MINOR(78)
/** @} */

#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
 * @brief This function returns a description of every available errors of the assigned unit.
 *
 * @param        w_errorCode         error number
 *
 * @param        dw_addInfo          additional error information
 *
 * @retval       pac_str             empty buffer to build the error string
 */
void SNMTS_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str);
#endif

#endif

/** @} */
