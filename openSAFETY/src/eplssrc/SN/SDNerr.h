/**
 * @addtogroup SDN
 * @{

 * @file SDNerr.h
 *
 * This file contains all errors that can occure inside the unit SDN.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * This file contains all errors that can occure inside the unit SDN.
 *
 * Also one function is declared inside this file. It is the function to get
 * an error string matching to a specific error number, because only the error
 * modul need to see this function declaration
 *
 */
#ifndef SDNERR_H
#define SDNERR_H


/**
 * @def SDN_k_ERR_ATTR_INV
 * This error means that the object 0x1200, 0x01 was accessed with attribute
 * &lt;SOD_k_ATTR_BEF_RD&gt; set. This abject is not allowed to be accesed with
 * this attribute.
*/
#define SDN_k_ERR_ATTR_INV  \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SDN_k_UNIT_ID, 1u)


#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors
*               of the assigned unit.
*
* @param        w_errorCode  - error number
*
* @param        dw_addInfo   - additional error information
*
* @retval          pac_str      - empty buffer to build the error string.
*/
void SDN_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str);
#endif

#endif

/** @} */
