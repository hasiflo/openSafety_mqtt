/**
 * @addtogroup SN
 * @{
 * @defgroup SDN [SDN] Safety Domain Number
 * @{
 *
 * Safety domain number managment and registration
 *
 * @file SDN.h SDN Module Interface
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
 */

#ifndef SDN_H
#define SDN_H

/**
 * This function initializes the SDN module
 *
 * All domain numbers which will be managed by this module will be loaded
 * from the SOD
 *
 * @param b_instNum openSAFETY stack instance number (not checked, checked in SSC_InitAll())
 *                  valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 * @return
 *      - TRUE - initialization succeded
 *      - FALSE - initialization failed
 */
BOOLEAN SDN_Init(BYTE_B_INSTNUM);

/**
* @brief This function returns a domain number which is assigned to the specified instance.
*
* @param        b_instNum instance number (not checked, called with EPLS_k_SCM_INST_NUM or
*                         checked in SSC_ProcessSNMTSSDOFrame() or SPDO_BuildTxSpdo() ),
*                         valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @return
*               - == 0x0000 - error, NO domain number is assigned
*               - <> 0x0000 - valid domain number
*/
UINT16 SDN_GetSdn(BYTE_B_INSTNUM);

/**
* @brief This function sets the Safety Domain Number for the own SN.
*
* @param     b_instNum            instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param     w_ownSdn            own SDN to be assigned, (not checked, checked in checkRxAddrInfo()), valid range : k_MIN_SDN .. k_MAX_SDN
*
* @return
*  - TRUE            - success
*  - FALSE           - failure
*/
BOOLEAN SDN_SetSdn(BYTE_B_INSTNUM_ UINT16 w_ownSdn);

#endif

/** @} */
/** @} */
