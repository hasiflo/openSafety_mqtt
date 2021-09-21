/**
 * @addtogroup SCM_part
 * @{
 * @addtogroup SSDOC [SSDOC] Safety Service Data Object Client
 * @{
 *
 * SSDOC, provides client functionality to process Service Data.
 *
 * The SSDO Client works closely with the Safety Configuration Manager. The Client
 * provides one service to write object data into the SOD of a specified SN. Further
 * more the Client provides one service to read object data from the SOD of a specified
 * SN. Every SN disposes of a SSDO Server to communicate with the SSDO Client. The
 * SSDO Client disposes of many finite state machine to process communication with
 * several SSDO Servers quasi concurrent.
 *
 * Same as Safety Configuration Manager (SCM), only a single SSDO Client is available
 * in the network.
 *
 * @file SSDOC.h
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 */

#ifndef SSDOC_H
#define SSDOC_H

/**
* @brief This function initializes SSDOC Protocol Layer and SSDOC Service Layer.
*
* @return
* - TRUE  - initialization succeeded
* - FALSE - initialization failed
*/
BOOLEAN SSDOC_Init(void);

/**
* @brief This function processes a single service response that was identified by the
* control unit (SSC) as a Service Data Object Response from a SSDO Server.
*
* The SSDO Client can only process SSDO Server Service Responses. This function searches
* for the matching SSDO Client Protocol FSM which is waiting for the received service
* response and passes the service response to the matching SSDO Client Protocol FSM.
*
* @param dw_ct consecutive time, (not checked, any value allowed), valid range: any 32 bit value
*
* @param ps_respHdr ref to resp header information (not checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL
*
* @param pb_respData ref to resp data (not checked, only called with reference to array in processStateReqProc()), valid range: <> NULL
*
* @return
* - TRUE             - resp successfully processed
* - FALSE            - resp failed to process
*/
BOOLEAN SSDOC_ProcessResponse(UINT32 dw_ct, const EPLS_t_FRM_HDR *ps_respHdr,
                              const UINT8 *pb_respData);


#endif

/** @} */
/** @} */
