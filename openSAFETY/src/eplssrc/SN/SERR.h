/**
 * @addtogroup SN
 * @{
 * @addtogroup SERR [SERR] Error module of the openSAFETY Stack
 * @{
 *
 * A module to handle internal errors and openSAFETY errors.
 *
 * @file SERR.h
 *
 * This unit provides functionality to handle stack internal errors.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * All stack
 * internal errors are passed via callback to the application.
 *
 */
#ifndef SERR_H
#define SERR_H

/**
 * @def SERR_k_NO_ADD_INFO
 * No additional error information is available.
 */
#define SERR_k_NO_ADD_INFO         0x00UL


/**
 * @def SERR_BUILD_ERROR
 * This macro builds an error. An error contains the error type, error class,
 * the unit id (see EPLStypes.h) and the unit error. The encoding of the
 * values in a 16 bit error code is shown in file SERRapi.h.
 */
#define SERR_BUILD_ERROR(errType, errClass, unitId, unitErr)  \
          ((UINT16)((UINT16)((UINT16)(errType)  << 14) |      \
                    (UINT16)((UINT16)(errClass) << 12) |      \
                    (UINT16)((UINT16)(unitId)   <<  8) | (UINT16)(unitErr)))


/**
 * @def SERR_MASK_UNIT_ID
 * This bit pattern used with a logical AND operation clears all bits but
 * bit 11, 10, 9, 8 inside the error code. These 4 bits encode maximum 16
 * different units which can report errors.
 *
 * 0000|1111|0000|0000
 */
#define SERR_MASK_UNIT_ID          0x0F00u


/**
 * @def SERR_GET_UNIT_ID
 * This macro returns the 4 bit unit Id encoded within the 16 bit error code.
 * @param w_errorCode 16 bit value that holds Error Class, Error ID and the error itselves (checked), valid range: any 16 bit value
 */
#define SERR_GET_UNIT_ID(w_errorCode)  \
          ((((UINT16)(w_errorCode)) & SERR_MASK_UNIT_ID) >> 8)


/**
 * @brief This function initializes all module global and global variables defined in the unit SERR.
 */
void SERR_Init(void);


/**
* @brief This function stores an internal error that occurred inside the EPLS Stack and calls the application callback
* function SAPL_SERR_SignalErrorClbk.
*
* @see         SERR_GetLastAddInfo()
*
* @param       b_instNum          instance number (not checked, checked by the API), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        w_error           error that occurred in the EPLS stack (not checked, only called with define),
*                                 valid range: any UINT16 value
*
* @param        dw_addInfo        additional error information (not checked, any value allowed) valid range: any UINT32 value
*/
void SERR_SetError(BYTE_B_INSTNUM_ UINT16 w_error, UINT32 dw_addInfo);

/**
* @brief This function returns the last additional error info that occurred in the openSAFETY stack that was set by SERR_SetError.
* This function may be called immediately after a stack internal function does return with FALSE. Otherwise a previous non
* relevant additional error info (the last info) is returned.
*
* @see          SERR_SetError()
*
* @return       last additional error info
*/
UINT32 SERR_GetLastAddInfo(void);


#endif

/** @} */
/** @} */
