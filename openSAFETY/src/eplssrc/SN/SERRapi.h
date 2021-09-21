/**
 *
 * @addtogroup SERR
 * @{
 * @file SERRapi.h
 *
 * This file is the application interface header file of the unit.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 * @author H. Pill, LarSYS Automation  GmbH
 *
 * @par
 * @{
 * <h2>Error Coding</h2>
 *
 * This table shows the bit coding of the 16 bit error code of the openSAFETY Stack.
 * It includes error type, error class, unit that reports the error and the error code itself:
 *
 * <table>
 *   <tr><td>bit pos</td><td>15-14</td><td>13-12</td><td>11-08</td><td>07-00</td></tr>
 *   <tr><td>meaning</td><td>errType</td><td>errClass</td><td>unitId</td><td>unitErr</td></tr>
 * </table>
 *
 * Subsequently macros are provided to decode error type and error class.
 * @}
 *
 * <h2>History for SERRapi.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>15.03.2013</td><td>Hans Pill</td><td>added statistic counters</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>new error counter for SPDOs with 40 bit CT</td></tr>
 * </table>
 *
 */

#ifndef SERRAPI_H
#define SERRAPI_H

/**
 * @def SERR_GET_ERROR_TYPE
 * This macro returns 2 bit "error type" encoded within the 16 bit error code.
 * @param       w_errorCode         16 bit value that holds Error Class, Error ID and the error itselves (checked), valid range: any 16 bit value
 */
#define SERR_GET_ERROR_TYPE(w_errorCode) \
        ((((UINT16)(w_errorCode)) & (UINT16)(0xC000)) >> 14)

/**
 * @name Error Type
 * @{
 *
 * These symbols represent the type of an error. The openSAFETY Stack differs two error types:
 * - **fail safe**: a fatal error occurred within the openSAFETY Stack which cannot be handled by the openSAFETY Stack.
 * - **not fail safe**: a minor error or an information occurred within the openSAFETY Stack which can be handled by the openSAFETY Stack.
 */

/** error type "fail safe" */
#define SERR_TYPE_FS             3u
/** error type NOT "fail safe" */
#define SERR_TYPE_NOT_FS         0u
/** @} */

/**
 * @def SERR_GET_ERROR_CLASS
 * This macro returns 2 bits "error class" encoded within the 16 bit error code.
 * @param       w_errorCode         16 bit value that holds Error Class, Error ID and the error itselves (checked), valid range: any 16 bit value
 */
#define SERR_GET_ERROR_CLASS(w_errorCode) \
        ((((UINT16)(w_errorCode)) & (UINT16)(0x3000)) >> 12)

/**
 * @name Error Class
 * @{
 * These symbols represent the class of an error. The openSAFETY Stack differs three error classes:
 * - **fatal error**: a profoundly error occurred within the openSAFETY Stack.
 *   The openSAFETY Stack is in "fail safe". The application is requested to take an action.
 * - **minor error**: an error of minor defect occurred within the openSAFETY Stack.
 *   The regular processing is not affected. The application is NOT requested
 *   to take any action.
 * - **info**: the openSAFETY Stack gives information about unexpected circumstance.
 * Neither application nor openSAFETY Stack is requested to take any action.
 */
/** fatal error */
#define SERR_CLASS_FATAL         3u
/** minor error */
#define SERR_CLASS_MINOR         2u
/** information */
#define SERR_CLASS_INFO          1u
/** error class not used */
#define SERR_CLASS_NOT_USED      0u
/** @} */

/**
 * @name Error Data Types
 * @{
 */

/**
 * @enum SERR_t_STATEVT_COMMON
 * Enumeration of the common error events counted by the statistic counters.
 */
typedef enum
{
    /** data length does not match to the length field */
    SERR_k_SFS_LENGTH,
    /** safety frame is too long */
    SERR_k_SFS_TOO_LONG,
    /** different frame IDs in subframe 1 and 2 */
    SERR_k_SFS_FRM_ID,
    /** SADR is invalid */
    SERR_k_SFS_SADR_INV,
    /** SDN is invalid */
    SERR_k_SFS_SDN_INV,
    /** TADR is invalid */
    SERR_k_SFS_TADR_INV,
    /** crc1 is invalid */
    SERR_k_SFS_CRC1,
    /** crc2 is invalid */
    SERR_k_SFS_CRC2,
    /** data in subframe 1 is not the same as in subframe 2 */
    SERR_k_SFS_DATA,
    /** SPDO got rejected */
    SERR_k_CYC_REJECT,
    /** common SPDO error counter */
    SERR_k_CYC_ERROR,
    /** SSDO/SNMT rejected */
    SERR_k_ACYC_REJECT,
    /** SSDO/SNMT retry */
    SERR_k_ACYC_RETRY,
    /** number of common counters */
    SERR_k_NO_COMMON_CTR
} SERR_t_STATEVT_COMMON;

/**
 * @enum SERR_t_STATEVT_SPDO
 * Enumeration of the spdo error events counted by the statistic counters.
 */
typedef enum
{
    /** CT invalid or repetition */
    SERR_k_SPDO_INV_CT,
    /** SCT timeout */
    SERR_k_SPDO_SCT_TOUT,
    /** propagation delay too short */
    SERR_k_SPDO_PROP_SHORT,
    /** propagation delay too long */
    SERR_k_SPDO_PROP_LONG,
    /** wrong payload size */
    SERR_k_SPDO_PAYLOAD,
    /** number of not answered TR expired */
    SERR_k_SPDO_TR_EXP,
    /** TR time 1 expired */
    SERR_k_SPDO_TR_TIME1,
    /** time request cycle time expired */
    SERR_k_SPDO_TR_TIME2,
    /** time response too late */
    SERR_k_SPDO_TR_LATE,
    /** invalid TR received */
    SERR_k_SPDO_TR_INV,
    /** time request propagation delay too long */
    SERR_k_SPDO_TR_PROP_LONG,
    /** time request propagation delay too short */
    SERR_k_SPDO_TR_PROP_SHORT,
    /** extended CT counter invalid */
    SERR_k_SPDO_EXT_CT,
    /** number of spdo counters */
    SERR_k_NO_SPDO_CTR
} SERR_t_STATEVT_SPDO;

/**
 * @enum SERR_t_STATEVT_ACYC
 * Enumeration of the acyclic error events counted by the statistic counters.
 */
typedef enum
{
    /** SSDO timeout */
    SERR_k_SSDO_TOUT,
    /** SSDO retry */
    SERR_k_SSDO_RETRY,
    /** SNMT timeout */
    SERR_k_SNMT_TOUT,
    /** SNMT retry */
    SERR_k_SNMT_RETRY,
    /** number of spdo counters */
    SERR_k_NO_ACYC_CTR
} SERR_t_STATEVT_ACYC;

/** @} */

/**
 * @var SERR_aadwCommonEvtCtr
 * This module global variable array contains the common statistic error counters.
 */
extern UINT32 SERR_aadwCommonEvtCtr[EPLS_cfg_MAX_INSTANCES][SERR_k_NO_COMMON_CTR];

/**
 * @brief This function is a callback function which is provided by the EPLS Application. The function is called by the EPLS Stack (unit SERR) to signal an internal error of the EPLS Stack.
 *
 * @param        b_instNum          instance number w_errorCode error class, unit in which the error occured and the error
 * @param       w_errorCode         16 bit value that holds Error Class, Error ID and the error itselves (checked), valid range: any 16 bit value
 *
 * @param        dw_addInfo         additional error information
 */
void SAPL_SERR_SignalErrorClbk(BYTE_B_INSTNUM_ UINT16 w_errorCode,
        UINT32 dw_addInfo);

/**
 * @brief This function returns a reference to an error string which describes the error exactly. Every unit stores its error strings and returns a reference to them if requested.
 *
 * @param       w_errorCode         16 bit value that holds Error Class, Error ID and the error itselves (checked), valid range: any 16 bit value
 *
 * @param       dw_addInfo          additional error information (not checked, any value allowed), valid range: any 32 bit value
 *
 * @return       <> NULL           - pointer to error string
 */
#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
CHAR * SERR_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo);
#endif
/**
 * @brief This function increases the statistic error counter.
 *
 * @param       b_instNum          instance number (not checked, checked by the API), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param       e_evt              statistic event which needs to be counted
 */
void SERR_CountCommonEvt(BYTE_B_INSTNUM_ SERR_t_STATEVT_COMMON const e_evt);
/**
 * @brief This function increases the statistic SPDO error counter.
 *
 * @param       b_instNum          instance number (not checked, checked by the API), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param       w_spdoIdx        Rx SPDO index (not checked, checked in SPDO_CheckRxTimeout() or SPDO_BuildTxSpdo() or ProcessTReq() or ProcessTRes() or SPDO_ProcessRxSpdo() and SPDO_GetSpdoIdxForSAdr()), valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param       e_evt              statistic event which needs to be counted
 */
void SERR_CountSpdoEvt(BYTE_B_INSTNUM_ UINT16 const w_spdoIdx, SERR_t_STATEVT_SPDO const e_evt);
/**
 * @brief This function increases the statistic acyclic error counter.
 *
 * @param       w_fsmNum           finite state machine number, assigned internally (not checked, checked in SNMTM_BuildRequest()), valid range:	0 .. <SNMTM_cfg_MAX_NUM_FSM-1>
 *
 * @param       e_evt              statistic event which needs to be counted
 */
void SERR_CountAcycEvt(UINT16 const w_fsmNum, SERR_t_STATEVT_ACYC const e_evt);

#if (EPLS_cfg_ERROR_STATISTIC == EPLS_k_ENABLE)
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
BOOLEAN SERR_GetSpdoStatCtr(BYTE_B_INSTNUM_ UINT16 const w_spdoIdx, SERR_t_STATEVT_SPDO const e_evt,UINT32 * const pdw_statCtr);
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
BOOLEAN SERR_GetAcycStatCtr(UINT16 const us_snIdx, SERR_t_STATEVT_ACYC const e_evt,UINT32 * const pdw_statCtr);
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
BOOLEAN SERR_GetCommonStatCtr(BYTE_B_INSTNUM_ SERR_t_STATEVT_COMMON const e_evt,UINT32 * const pdw_statCtr);
#endif

/** @} */
