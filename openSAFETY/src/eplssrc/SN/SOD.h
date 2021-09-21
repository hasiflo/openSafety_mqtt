/**
 * @addtogroup SN
 * @{
 * @addtogroup SOD [SOD] Safety Object Dictionary
 * @{
 *
 * This unit manages the access to one or to several instances of the SOD.
 *
 * @file SOD.h
 *
 * In case of an openSAFETY Domain Gateway this unit has to manage many and at least two Object Dictionaries
 * to provide gateway functionality.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 *
 * Two different kinds of addressing an entry are supported:
 * - addressing one dedicated entry by means of index/sub-index via SOD_AttrGet()
 * - sequential addressing from the beginning of the SOD up to the end via SOD_AttrGetNext()
 *   (e. g. for storing/re-storing all SOD entries)
 *
 */

#ifndef SOD_H
#define SOD_H

/**
 * @name Overwriting SOD entry
 * @{
 * @brief These symbols are used for SOD write access of the openSAFETY Stack.
 * SOD_k_OVERWRITE enables and SOD_k_NO_OVERWRITE disables the overwriting of
 * a read-only object.
 */
#define SOD_k_NO_OVERWRITE                 FALSE
#define SOD_k_OVERWRITE                    TRUE
/** @} */

/**
 * @def SOD_k_NO_OFFSET
 * @brief This symbol is used if the offset parameter in the SOD_Read() or SOD_Write() is not needed.
 */
#define SOD_k_NO_OFFSET                 0x00UL

/**
 * @def SOD_k_LEN_NOT_NEEDED
 * @brief This symbol is used if the length parameter in the SOD_Read() is not needed.
 */
#define SOD_k_LEN_NOT_NEEDED            0x00UL

/**
 * @struct SOD_t_ACCESS
 * @brief This data structure combines all necessary variables to get access to a specific SOD entry of the own SOD.
 */
typedef struct
{
    /** handle to the specified entry */
	PTR_TYPE dw_hdl;
    /** reference to the entries's attributes */
    const SOD_t_ATTR *ps_attr;
    /** flag signals an application entry, implemented via application callback function */
    BOOLEAN o_applObj;
} SOD_t_ACCESS;

/**
 * @brief This function initializes all module global and global variables defined in the unit SOD and checks the SOD.
 *
 * @param        b_instNum           instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return
 * - TRUE              - success
 * - FALSE             - failure
 */
BOOLEAN SOD_Init(BYTE_B_INSTNUM);

/**
 * @brief This function gets whether the write access to the SOD is enabled or disabled.
 *
 * @param        b_instNum         instance number (not checked, checked in SOD_Write() or SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return
 * - TRUE            - SOD write access is enabled
 * - FALSE           - SOD write access is disabled
 */
BOOLEAN SOD_WriteAccess(BYTE_B_INSTNUM);

/**
 * @brief This function gets whether the SOD is locked or not.
 *
 * @param   b_instNum         instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return
 * - TRUE            - SOD is locked
 * - FALSE           - SOD is not locked
 */
BOOLEAN SOD_IsLocked(BYTE_B_INSTNUM);

#endif

/** @} */
/** @} */
