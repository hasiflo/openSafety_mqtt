/**
 * @addtogroup SSC
 * @{
 *
 * @file SSCerr.h
 *
 * This file contains all defines and datatypes used for error  handling.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 *  Every error recognized by this unit is defined in here. This file is exported to the application to
 *  provide all error codes.
 *
 */

#ifndef SSCERR_H
#define SSCERR_H

/**
 * This error means that the reference to the number of free management frames points to NULL and thus is not valid.
*/
#define SSC_k_ERR_REF_MNGTFRMS_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SSC_k_UNIT_ID, 1)


/**
 * This error means that the ID of the received frame is NOT defined.
*/
#define SSC_k_ERR_RX_TELTYP_INV \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SSC_k_UNIT_ID, 2)


/**
 * This error means that the a SSDO Service was received which is
 * addressed to a SSDO Client, but the SN is not configured as SCM.
*/
#define SSC_k_ERR_NO_SSDO_CLIENT \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SSC_k_UNIT_ID, 3)


/**
 * This error means that the a SNMT Service was received which is addressed to a SNMT Master,
 * but the SN is not configured as SCM
*/
#define SSC_k_ERR_NO_SNMT_MASTER \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SSC_k_UNIT_ID, 4)


/**
 * This error means that the EPLS Stack is ready to process a received frame,
 * but the Stack is called without a valid reference to a frame.
*/
#define SSC_k_ERR_NO_RXFRM_STACK_RDY \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SSC_k_UNIT_ID, 5)

/**
 * This error means that the length of the received frame is invalid.
*/
#define SSC_k_ERR_FRAME_LEN \
        SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR, SSC_k_UNIT_ID, 6)


/**
 * This error means that a received frame is provided by the API, while another frame is already in process.
*/
#define SSC_k_ERR_FRM_PROV_FRM_IN_PROC \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SSC_k_UNIT_ID, 7)


/**
 * This error means that the SSC FSM is in an undefined state.
*/
#define SSC_k_ERR_PROC_STATE_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SSC_k_UNIT_ID, 8)

/**
 * This error means that the delivered instance is bigger than the maximum value.
*/
#define SSC_k_ERR_PROC_INST_INV \
        SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL, SSC_k_UNIT_ID, 9)


#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param              w_errorCode        error number
*
* @param              dw_addInfo         additional error information
*
* @retval              pac_str            empty buffer to build the error string dynamically in case of using add.
*       error info without interpretation of its value.
*/
void SSC_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str);
#endif



#endif

/** @} */
