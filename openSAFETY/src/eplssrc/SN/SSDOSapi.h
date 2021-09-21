/**
 * @addtogroup SSDOS
 * @{
 *
 * @file SSDOSapi.h API functions to be implemented by a SN if preload is being used
 *
 * @copyright Copyright (c) 2014,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author H. Pill, LARsys Automation GmbH
 *
 * <h2>History for SSDOCapi.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>25.09.2014</td><td>Hans Pill</td><td>API function for preload queue size</td></tr>
 * </table>
 */

#ifndef SSDOSAPI_H
#define SSDOSAPI_H

/**
* @brief This function initializes the size of the SSDOS queue.
*
* @param        b_instNum        instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        b_queueSize      size of the SSDO receive queue of this device (checked)
*                                valid range: k_QLEN_PREL_MIN .. k_QLEN_PREL_MAX
*
*/
BOOLEAN SSDOS_SetQueueSize(BYTE_B_INSTNUM_ UINT8 const b_queueSize);

#endif

/** @} */
