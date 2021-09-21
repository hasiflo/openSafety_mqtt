/**
 * @addtogroup SN
 * @{
 * @addtogroup SPDO [SPDO] Safety Proces Data Objects
 * @{
 *
 * SPDO, transmission and reception of process data objects.
 *
 *             This unit exchanges cyclic safety process data objects (SPDO) and
 *             verifies the network performance. Received SPDO's are mapped
 *             into an instance of a SOD and SPDO's to be transmitted are
 *             mapped from an instance of a SOD. Via cyclic time requests
 *             the unit gets the internal time of the server-SN's.
 *             Network performance of received SPDO's is compared to the
 *             limits defined in the SOD.
 *
 * @file SPDO.h
 *
 * This unit exchanges cyclic safety process data objects (SPDO) and
 *             verifies the network performance.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * Each instance is assigned to one domain. The maximum number of entries in the list is limited to
 * &lt;EPLS_cfg_MAX_INSTANCES&gt. The domain number and instance can be requested from the module SDN.
 *
 * <h2>History for SNMTM.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>added function for SPDOs with 40 bit CT</td></tr>
 * </table>
 */

#ifndef SPDO_H
#define SPDO_H

/**
* @brief This function initializes the variables defined in the t_SPDO_OBJ structure and calls the SPDO unit initialization functions.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return       TRUE           - success  FALSE          - failure
*/
BOOLEAN SPDO_Init(BYTE_B_INSTNUM);

/**
* @brief This function activates the mapping of all Tx SPDO.
*
* The mapping- and the communication parameters of all the Tx SPDOs are checked.
*  It has to be called before transition from PreOperational to Operational.
*
* @param        b_instNum       instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @retval          po_busy          only relevant if the return value is TRUE,
* - TRUE  : not all Tx SPDO was activated
* - FALSE : all Tx SPDO was activated (not checked, only called with refernce to variable), valid range : <> NULL
*
* @return
* - TRUE           - SPDO mappings are activated.
* - FALSE          - SPDO mappings are not activated. Wrong SPDO mapping- or/and communication parameters.
*/
BOOLEAN SPDO_ActivateTxSpdoMapping(BYTE_B_INSTNUM_ BOOLEAN *po_busy);

/**
* @brief This function activates the mapping of all Rx SPDO.
*
* The mapping- and the communication parameters of all the Rx SPDOs are checked. It has to be called before transition from
* PreOperational to Operational.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @retval       po_busy          only relevant if the return value is TRUE,
* - TRUE  : not all Rx SPDO was activated
* - FALSE : all Rx SPDO was activated (not checked, only called with refernce to variable), valid range : <> NULL
*
* @return
* - TRUE           - SPDO mappings are activated.
* - FALSE          - SPDO mappings are not activated. Wrong SPDO mapping- or/and communication parameters.
*/
BOOLEAN SPDO_ActivateRxSpdoMapping(BYTE_B_INSTNUM_  BOOLEAN *po_busy);

/**
* @brief This function is called by the SNMTS, resets SPDO Rx and Tx state machines and activates the SPDO processing and building.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct            consecutive time, internal timer value (not checked, any value allowed), valid range: (UINT32)
*
*/
void SPDO_SetToOp(BYTE_B_INSTNUM_ UINT32 dw_ct);

/**
* @brief This function is called by the SNMTS, resets SPDO Rx state machines and deactivates the SPDO processing and building.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
*/
void SPDO_SetToPreOp(BYTE_B_INSTNUM);

/**
* @brief This function is called by the SNMTS before the SPDO activation.
*
* The counters for the activation are reset and the SPDO assign tables are cleared.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll() or SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*/
void SPDO_ActivateStart(BYTE_B_INSTNUM);

void SPDO_SetExtCtInitValue(BYTE_B_INSTNUM_ UINT64 const ddw_ctInitVal);

#endif

/** @} */
/** @} */
