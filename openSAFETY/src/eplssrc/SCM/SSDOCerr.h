/**
 * @addtogroup SSDOC
 * @{
 *
 * @file SSDOCerr.h
 *
 * This file contains all defines and data types used for error handling. Every error
 * recognized by this unit is defined in here.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SSDOCerr.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>01.06.2016</td><td>Stefan Innerhofer</td><td>add new error code if pointer to the API callback function is NULL</td></tr>
 *     <tr><td>17.09.2018</td><td>Stefan Innerhofer</td><td>change SSDOC_k_ERR_PROT_FSM_STATE from a fatal to a minor error</td></tr>
 * </table>
 *
 */


#ifndef SSDOCERR_H
#define SSDOCERR_H

/**
 * @name SSDOC macro to build error codes
 * @{
 */
#define SSDOC_ERR_FATAL(num) SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, \
                                              SSDOC_k_UNIT_ID, (num))
#define SSDOC_ERR_MINOR(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, \
                                              SERR_CLASS_MINOR, \
                                              SSDOC_k_UNIT_ID, (num))
#define SSDOC_ERR__INFO(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS,\
                                              SERR_CLASS_INFO,\
                                              SSDOC_k_UNIT_ID, (num))
/** @} */

/**
 * @name Error macros for SSDOCaccess.c
 * @{
 */
/* SSDOC_SendWriteReq() */
#define SSDOC_k_ERR_WR_REQ_PTR           SSDOC_ERR_FATAL(1)
#define SSDOC_k_ERR_WR_REQ_SADR          SSDOC_ERR_FATAL(2)
#define SSDOC_k_ERR_WR_DATA_PTR          SSDOC_ERR_FATAL(3)
#define SSDOC_k_ERR_WR_REQ_PYLDLEN       SSDOC_ERR_FATAL(4)
#define SSDOC_k_ERR_WR_REQ_CLBK_PTR      SSDOC_ERR_FATAL(5)
/* SSDOC_SendReadReq() */
#define SSDOC_k_ERR_RD_REQ_PTR           SSDOC_ERR_FATAL(6)
#define SSDOC_k_ERR_RD_REQ_SADR          SSDOC_ERR_FATAL(7)
#define SSDOC_k_ERR_RD_DATA_PTR          SSDOC_ERR_FATAL(8)
#define SSDOC_k_ERR_RD_REQ_PYLDLEN       SSDOC_ERR_FATAL(9)
#define SSDOC_k_ERR_RD_REQ_CLBK_PTR      SSDOC_ERR_FATAL(10)
/* TypeLenReadValid() */
#define SSDOC_k_ERR_RD_DATA_LEN          SSDOC_ERR_FATAL(11)
/* TypeLenValid() */
#define SSDOC_k_ERR_DATA_LEN             SSDOC_ERR_FATAL(12)
#define SSDOC_k_ERR_DATA_TYPE            SSDOC_ERR_FATAL(13)
/** @} */

/**
 * @name Error macros for SSDOCprotocol.c
 * @{
 */
/* SSDOC_ProcessResponse() */
#define SSDOC_k_ERR_RESP_NOT_ASSIGNED    SSDOC_ERR_MINOR(14)
/* SSDOC_ProtocolFsmProcess() */
#define SSDOC_k_ERR_PROT_FSM_STATE       SSDOC_ERR_MINOR(15)
#define SSDOC_k_ERR_RESP_DATA_LEN1       SSDOC_ERR_MINOR(16)
#define SSDOC_k_ERR_RX_OBJ_SIZE          SSDOC_ERR_MINOR(17)
#define SSDOC_k_ERR_RESP_DATA_LEN2       SSDOC_ERR_MINOR(18)
#define SSDOC_k_ERR_RESP_DATA_LEN3       SSDOC_ERR_MINOR(19)
#define SSDOC_k_ERR_RESP_DATA_LEN11      SSDOC_ERR_MINOR(20)
#define SSDOC_k_ERR_RESP_SACMD_INV1      SSDOC_ERR_MINOR(21)
#define SSDOC_k_ERR_REST_DATA_LEN2       SSDOC_ERR_MINOR(22)
#define SSDOC_k_ERR_RESP_DATA_LEN4       SSDOC_ERR_MINOR(23)
#define SSDOC_k_ERR_RESP_DATA_LEN5       SSDOC_ERR_MINOR(24)
#define SSDOC_k_ERR_RESP_DATA_LEN6       SSDOC_ERR_MINOR(25)
#define SSDOC_k_ERR_RESP_DATA_LEN7       SSDOC_ERR_MINOR(26)
#define SSDOC_k_ERR_RESP_DATA_LEN8       SSDOC_ERR_MINOR(27)
#define SSDOC_k_ERR_PROT_STATE_INV       SSDOC_ERR_FATAL(28)
/* SSDOC_SendReq() */
#define SSDOC_k_ERR_NO_PROT_FSM          SSDOC_ERR_FATAL(29)
/* checkResponse() */
#define SSDOC_k_ERR_RESP_TB_WRONG        SSDOC_ERR_MINOR(30)
#define SSDOC_k_ERR_RESP_IDX_WRONG       SSDOC_ERR_MINOR(31)
#define SSDOC_k_ERR_RESP_SUBIDX_WRONG    SSDOC_ERR_MINOR(32)
#define SSDOC_k_ERR_RESP_DATA_LEN9       SSDOC_ERR_MINOR(33)
/* processAbortResp() */
#define SSDOC_k_ERR_RESP_DATA_LEN10      SSDOC_ERR_MINOR(34)
#define SSDOC_k_ERR_RESP_SACMD_INV       SSDOC_ERR_MINOR(35)
/* processUnknownSizeEndSeg() */
#define SSDOC_k_ERR_REST_DATA_LEN        SSDOC_ERR_MINOR(36)
/* SSDOC_BuildRequest() */
#define SSDOC_k_ERR_REF_FREE_FRMS        SSDOC_ERR_FATAL(37)
#define SSDOC_k_ERR_REF_CALLBACK_NULL    SSDOC_ERR_FATAL(43)
/** @} */

/**
 * @name Error macros for SSDOCservice.c
 * @{
 */
/* SSDOC_ServiceFsmProcess() */
#define SSDOC_k_ERR_SERV_FSM_EVT_INV1    SSDOC_ERR_FATAL(38)
#define SSDOC_k_ERR_SERV_FSM_EVT_INV2    SSDOC_ERR_FATAL(39)
/* SSDOC_TransferRunning() */
#define SSDOC_k_ERR_TRANSFER_RUNNING     SSDOC_ERR_FATAL(40)
/* processResponse() */
#define SSDOC_k_ERR_RESP_TR_FIELD_USED   SSDOC_ERR_MINOR(41)
#define SSDOC_k_ERR_RESP_SANO_DIFF       SSDOC_ERR_MINOR(42)
/** @} */

#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param		w_errorCode			error number
*
* @param         dw_addInfo			additional error information
*
* @retval           pac_str				empty buffer to build the error string.
*/
void SSDOC_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str);
#endif

#endif


/** @} */
