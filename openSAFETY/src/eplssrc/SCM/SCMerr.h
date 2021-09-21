/**
 * @addtogroup SCM
 * @{
 * @file SCMerr.h
 *
 * This file contains the error codes for the SCM module.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author Joachim Stolberg, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH

 *
 * <h2>History for SCMerr.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>14.06.2010</td><td>Hans Pill</td><td>added 2 new errors for better diagnosis</td></tr>
 *     <tr><td>24.11.2010</td><td>Hans Pill</td><td>Review SL V20</td></tr>
 *     <tr><td>23.01.2013</td><td>Hans Pill</td><td>changes for additional parameters according to openSafety specification</td></tr>
 *     <tr><td>07.03.2013</td><td>Hans Pill</td><td>changes for parameter checksum being written to the SN</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>24.06.2014</td><td>Hans Pill</td><td>new error code for not matching timestamp and crc parameter</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>added new error code for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>23.02.2017</td><td>Christopher Neuwirt and Stefan Innerhofer</td><td>added new error code for checksum size mismatch</td></tr>
 *     <tr><td>17.09.2018</td><td>Stefan Innerhofer</td><td>change SCM_k_UNVALID_EVENT_ERR from a fatal to a minor error</td></tr>
 * </table>
 *
 */

#ifndef SCMERR_H
#define SCMERR_H

/**
 * @name SCM macros to build error code
 * @{
 */
#define SCM_ERR_FATAL(num) SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL,\
                                            SCM_k_UNIT_ID, (num))
#define SCM_ERR_MINOR(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR,\
                                            SCM_k_UNIT_ID, (num))
#define SCM_ERR_INFO(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_INFO,\
                                            SCM_k_UNIT_ID, (num))
/** @} */

/**
 * @name SCM-error-codes
 * @{
 * @brief Error codes for the SCM module.
*/
/** invalid parameter 'w_hdl' in function SCM_OperatorAck() */
#define SCM_k_PARAM1_ERR              SCM_ERR_FATAL(1)

/** invalid FSM state in function SCM_OperatorAck() */
#define SCM_k_PARAM2_ERR              SCM_ERR_FATAL(2)

/** invalid parameter 'w_hdl' in function SCM_SnFailAck() */
#define SCM_k_PARAM3_ERR              SCM_ERR_FATAL(3)

/** invalid FSM state in function SCM_SnFailAck() */
#define SCM_k_PARAM4_ERR              SCM_ERR_FATAL(4)

/** invalid event in function ScmFsmProcess() */
#define SCM_k_UNVALID_EVENT_ERR       SCM_ERR_MINOR(5)

/** FSM state error in function SCM_Trigger() */
#define SCM_k_STATE_ERR               SCM_ERR_FATAL(6)

/** SOD read error in function ScmInitFsm() */
#define SCM_k_SOD_READ_ERR            SCM_ERR_FATAL(7)

/** SOD write error in function SCM_SodWrite() */
#define SCM_k_SOD_WRITE_ERR           SCM_ERR_FATAL(8)

/** invalid precondition in function ScmFsmPreCon() */
#define SCM_k_INVALID_PRECOND         SCM_ERR_FATAL(9)

/** invalid SNMT State received from SNMTM */
#define SCM_k_INVALID_SNMT_STATE      SCM_ERR_FATAL(10)

/** invalid SNMT State received from SNMTM */
#define SCM_k_INVALID_SNMT_STATE1     SCM_ERR_FATAL(11)

/** Callback function must not be implemented for this object */
#define SCM_k_OBJ_CLBK_ERR            SCM_ERR_FATAL(12)

/** Invalid refernce to the number of free frames */
#define SCM_k_ERR_REF_FREE_FRMS       SCM_ERR_FATAL(13)

/** SDO abort code received */
#define SCM_k_ERR_SDO_ABORT           SCM_ERR_MINOR(14)

/** SDO abort code received from node x */
#define SCM_k_ERR_SDO_ABORT_NODE      SCM_ERR_MINOR(15)

/** additional parameters with invalid version found */
#define SCM_k_ADD_PARAM1_ERR          SCM_ERR_MINOR(16)

/** additional parameters for wrong SADR found */
#define SCM_k_ADD_PARAM2_ERR          SCM_ERR_MINOR(17)

/** additional parameters are too short/not available */
#define SCM_k_ADD_PARAM3_ERR          SCM_ERR_MINOR(18)

/** requested additional parameters are not available */
#define SCM_k_ADD_PARAM4_ERR          SCM_ERR_MINOR(19)

/** parameter checksums are not available */
#define SCM_k_PARA_CHKSM_ERR          SCM_ERR_MINOR(20)

/** parameter checksums sizes do not match */
#define SCM_k_PARA_CHKSM_SIZE         SCM_ERR_MINOR(21)

/** random number is too high */
#define SCM_k_ERR_INIT_CT             SCM_ERR_FATAL(22)

/** parameter checksum sizes do not match */
#define SCM_k_PARA_CHKSM_SIZE_INFO    SCM_ERR_INFO(23)
/** @} */

#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param w_errorCode 	error number
* @param dw_addInfo 	additional error information
* @retval pac_str 		empty buffer to build the error string.
*/
void SCM_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str);
#endif

#endif
/** @} */
