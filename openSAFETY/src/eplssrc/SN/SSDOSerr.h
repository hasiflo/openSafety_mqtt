/**
 * @addtogroup SSDOS
 * @{
 *
 *
 * @file SSDOSerr.h
 * This file contains all defines and data types used for error
*              handling. Every error recognized by this unit is defined in
*              here.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH

 * <h2>History for SSDOerr.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>11.09.2014</td><td>Hans Pill</td><td>added error for wrong preload queue size instance</td></tr>
 * </table>
 *
 */

#ifndef SSDOSERR_H
#define SSDOSERR_H


/***
*    constants and macros
***/
/* SSDOS macro to build error codes */
#define SSDOS_ERR_FATAL(num) SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, \
                                              SSDOS_k_UNIT_ID, (num))
#define SSDOS_ERR_MINOR(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, \
                                              SERR_CLASS_MINOR, \
                                              SSDOS_k_UNIT_ID, (num))
#define SSDOS_ERR__INFO(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS,\
                                              SERR_CLASS_INFO,\
                                              SSDOS_k_UNIT_ID, (num))

/** SSDOSmain.c **/
/* SSDOS_ProcessRequest() */
#define SSDOS_k_ERR_REQ_DSADR       SSDOS_ERR__INFO(1)
#define SSDOS_k_ERR_TR              SSDOS_ERR_MINOR(2)
#define SSDOS_k_ERR_FSM_STATE       SSDOS_ERR_FATAL(3)
/* WfReqInitState() */
#define SSDOS_k_ERR_ABORT_RX        SSDOS_ERR_MINOR(4)
#define SSDOS_k_ERR_UPLD_INIT_LEN   SSDOS_ERR_MINOR(5)
#define SSDOS_k_ERR_ABORT_LEN       SSDOS_ERR_MINOR(6)
#define SSDOS_k_ERR_SACMD           SSDOS_ERR_MINOR(7)
#define SSDOS_k_ERR_PYLD_LEN        SSDOS_ERR_MINOR(8)
/* DwnldRespInitState() */
#define SSDOS_k_ERR_SACMD_INV       SSDOS_ERR_FATAL(9)
/* DwnldWfReqSegState() */
#define SSDOS_k_ERR_PYLD_LEN1       SSDOS_ERR_MINOR(10)
/* UpldRespInitState() */
#define SSDOS_k_ERR_SACMD_INV1      SSDOS_ERR_FATAL(11)
/* UpldWfReqSegState() */
#define SSDOS_k_ERR_PYLD_LEN2       SSDOS_ERR_MINOR(12)
#define SSDOS_k_ERR_PYLD_LEN3       SSDOS_ERR_MINOR(13)


/** SSDOSupDwnLd.c **/
/* SSDOS_DwnldInitExpRespSend() */
#define SSDOS_k_ERR_DWNLD_EXP_LEN   SSDOS_ERR_MINOR(14)
#define SSDOS_k_ERR_DWNLD_EXP_LEN1  SSDOS_ERR_MINOR(15)
/* SSDOS_DwnldInitSegReqProc() */
#define SSDOS_k_ERR_DWNLD_SEG_LEN   SSDOS_ERR_MINOR(16)
#define SSDOS_k_ERR_REQ_PYLD_LEN    SSDOS_ERR_MINOR(17)
/* SSDOS_DwnldMidSegReqProc() */
#define SSDOS_k_ERR_DWNLD_SEG_LEN2  SSDOS_ERR_MINOR(18)
/* SSDOS_DwnldEndSegReqProc() */
#define SSDOS_k_ERR_DWNLD_SEG_LEN1  SSDOS_ERR_MINOR(19)


/** SSDOSsodAcs.c **/
/* SSDOS_SodAcsReset() */
#define SSDOS_k_ERR_LOCK_WR_SEG     SSDOS_ERR_FATAL(20)
/* SSDOS_SodAcsReqValid() */
#define SSDOS_k_ERR_IDX_SIDX        SSDOS_ERR_MINOR(21)
#define SSDOS_k_ERR_TOGGLE_BIT      SSDOS_ERR_MINOR(22)
/* SodAcsWriteAllowed() */
#define SSDOS_k_ERR_WR_ACS1         SSDOS_ERR_MINOR(23)
#define SSDOS_k_ERR_WR_ACS2         SSDOS_ERR_MINOR(24)
#define SSDOS_k_ERR_WR_ACS3         SSDOS_ERR_MINOR(25)
#define SSDOS_k_ERR_WR_ACS4         SSDOS_ERR_MINOR(26)
/* SSDOS_SetQueueSize() */
#define SSDOS_k_ERR_INST_NUM_1      SSDOS_ERR_FATAL(27)


#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param w_errorCode error number
* @param dw_addInfo additional error information
*
* @retval pac_str empty buffer to build the error string.
*/
void SSDOS_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str);
#endif

#endif

/** @} */
