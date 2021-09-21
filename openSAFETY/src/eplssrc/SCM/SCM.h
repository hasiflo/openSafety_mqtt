/**
 * @addtogroup SCM_part
 * @{
 * @addtogroup SCM [SCM] Safety Configuration Manager
 * @{
 *
 *
 * The aim of the SCM is to configure, verify and start all SNs of the assigned domain.
 * After the configuration of the SNs the SCM performs the node guarding for all SNs,
 * which have reached the state OPERATIONAL.
 *
 * @file SCM.h
 *
 * This unit is the configuration manager of the openSAFETY stack.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author Joachim Stolberg, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 */

#ifndef SCM_H
#define SCM_H

/**
* @brief This function initializes the SCM.
*
* The initialization is done via the API function SCM_Activate().
*/
void SCM_Init(void);

/**
* @brief This function is a callback function used by the SNMTM.
*
* Will be called for every SNMTM response frame based on a request initiated by the SCM.
* The response data are given via pb_data.
*
* @param	w_regNum	process handle, used to address the corresponding FSM
* 	slot (not checked, checked in SCM_Trigger()). valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param	w_tadr		TADR in the received response to check in case of the
* 	SADR assignment (not checked, checked in checkRxAddrInfo()), valid range:
* 	EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param    w_rxSdn		SDN in the received response to check in case of the
* 	SADR assignment (not checked, checked in checkRxAddrInfo()), valid range:
* 	k_MIN_SDN .. k_MAX_SDN
*
* @param    pb_data		Pointer to the received data bytes of the response frame,
* 	this data must be deserialized (not checked, only called with reference to
* 	struct if o_timeout == TRUE).
*
* @param   	o_timeout
* - TRUE  : timeout happened pb_data is invalid
* - FALSE : response received pb_data is valid (checked).
*/
void SCM_SNMTM_RespClbk(UINT16 w_regNum, UINT16 w_tadr, UINT16 w_rxSdn,
                        const UINT8 *pb_data, BOOLEAN o_timeout);

#endif

/** @} */
/** @} */
