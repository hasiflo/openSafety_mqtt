/**
 * @addtogroup SSDO
 * @{
 * @addtogroup SSDOS [SSDOS] Safety Service Data Object Server
 * @{
 *
 *
 * @file SSDOS.h
 * This unit provides functionality of a SSDO Server. Thus service
 * requests are received and processed by this unit.
 *
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 */

#ifndef SSDOS_H
#define SSDOS_H

/**
* @brief This function initializes the SSDOS FSM.
*
* This function initializes the SSDOS FSM, the segment information for the segmented upload and download variables of SOD access and sets a pointer to the Main SADR.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
*
* @return
* - TRUE           - initialization succeeded
* - FALSE          - initialization failed
*/
BOOLEAN SSDOS_Init(BYTE_B_INSTNUM);

/**
* @brief This function processes SSDO Service Requests and responses them.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
* @retval        ps_txBuf         reference to openSAFETY frame to be transmitted (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
* @retval          po_busy          flag signals that SSDO Server is currently processing a service request:
* - TRUE:  Server busy
* - FALSE: Server ready to process (pointer not checked, only called with reference to variable in processStateReqProc()), valid range: <> NULL
*
* @return
* - TRUE           - SSDO Service Request successfully processed
* - FALSE          - SSDO Service Request processing failed (error was reported) or SSDOC Abort Frame successfully processed (no error was reported)
*
* @note FALSE is returned in order not to generate a response.
*/
BOOLEAN SSDOS_ProcessRequest(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                             EPLS_t_FRM *ps_txBuf, BOOLEAN *po_busy);



#endif
/** @} */
/** @} */
