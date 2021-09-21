/**
 * @addtogroup SNMTM
 * @{
 *
 * @file SNMTMapi.h
 * This file is the application interface header file of the unit.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 */


#ifndef SNMTMAPI_H
#define SNMTMAPI_H

/**
* @brief This function handles service request repetitions in case of a precedent service
* request was not responded by the specified SNMT Slave in time or not responded at all.
*
* Generally this function checks every SNMT Master FSM pausing in state "wait for response"
* for response timeout.
*
* @attention This function must not be called in SNMTS_k_ST_INITIALIZATION state. This
* function must be called at least once within the SNMT timeout (SOD index EPLS_k_IDX_SSDO_COM_PARAM,
* sub-index EPLS_k_SUBIDX_SNMT_TIMEOUT) with OUT value of number of free frame > 0.
*
* @param        dw_ct                     consecutive time (not checked any value allowed), valid range: any 32 bit value
*
* @retval       pb_numFreeFrms
* - IN: number of EPLS frames are free to transmit. (checked), valid range: <> NULL, value > 0
* - OUT: remaining number of free frames. If the value is zero, then not all SSDO channel may checked.
*/
void SNMTM_BuildRequest(UINT32 dw_ct, UINT8 *pb_numFreeFrms);

#endif /* #ifndef SNMTMAPI_H */

/** @} */
