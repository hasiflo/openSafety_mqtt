/**
 * @addtogroup SN
 * @{
 * @addtogroup SSC [SSC] Safety Stack Control
 * @{
 *
 * This file contains function declaration of openSAFETY Stack
 *             application interface of unit SSC.
 *
 * @file SSCapi.h SDN Module Interface
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
 */

#ifndef SSCAPI_H
#define SSCAPI_H

/**
 *    constants and macros
 **/

/**
 * @name Frame length limits
 * @{
 *
 * These limits represent limits of the openSAFETY frame.
 *
 * The limits are result of some calculations. The gap between 27
 * and 31 is in consequence of the calculations.
 */

/** Minimum frame length for openSAFETY Frame with 1 byte CRC */
#define SSC_k_MIN_TEL_LEN_SHORT        11u
/** Maximum frame length for openSAFETY Frame with 1 byte CRC */
#define SSC_k_MAX_TEL_LEN_SHORT        27u

/** Minimum frame length for openSAFETY Frame with 2 byte CRC */
#define SSC_k_MIN_TEL_LEN_LONG         31u
/** Maximum frame length for openSAFETY Frame with 2 byte CRC */
#define SSC_k_MAX_TEL_LEN_LONG        521u
/** @} */

/**
 * This enumeration is used as return value for the SSC function SSC_ProcessSNMTSSDOFrame().
 *
 * In case of a frame is currently processed, the function has to be called again to continue processing.
 */
typedef enum
{
    /** frame is completely processed, openSAFETY Stack is ready to process another received frame */
    SSC_k_OK,
    /** frame is currently processed, openSAFETY Stack is busy */
    SSC_k_BUSY
} SSC_t_PROCESS;


/**
 * @brief This function initializes the whole openSAFETY Stack with all of its instances.
 *
 * Each unit of the stack is affected. All finite state machines and all errors are reset.
 *
 * @attention The openSAFETY Stack uses functionality of the SHNF, thus at very first the
 * unit SHNF has to be initialized! This function has to be called to initialize the openSAFETY
 * Stack before any other openSAFETY Stack function is called.
 *
 * @return
 * - TRUE                 - initialization succeeded
 * - FALSE                - initialization failed,
 * @attention  If initialization failed, then the application must immediately switch to FAIL SAFE
 * state, because in some cases a minor error (no fail safe error) is reported.
 */
BOOLEAN SSC_InitAll(void);

/**
 * @brief This function processes a received SNMT or SSDO frame.
 *
 * For this purpose it evaluates the frame header and distributes the frame in dependence of its
 * frame type to the assigned unit (SSDOC, SSDOS, SNMTM, SNMTS). The return value of the function
 * signals whether the processing of the passed frame has finished. In case of return value
 * &lt;SSC_k_BUSY&gt; this function has to be recalled without reference to a new openSAFETY frame,
 * NULL has to be passed instead. In case of return value &lt;SSC_k_OK&gt; the processing of the
 * passed openSAFETY frame has finished and a new frame is able to be processed by the openSAFETY Stack.
 *
 * @attention This function must not be called in SNMTS_k_ST_INITIALIZATION state. At least one
 * SHNF memory block must be available for the response otherwise a FAIL SAFE error may be reported.
 *
 * @param        b_instNum         instance number (checked), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct             consecutive time, internal timer value (not checked, any value allowed), valid range: any value
 *
 * @param        pb_rxFrm          reference to the frame to be processed by the openSAFETY Stack (checked), valid range:
 * - <> NULL - received frame to be processed
 * - == NULL - continue frame processing
 *
 * @param        w_rxFrmLen        length of the received frame (in bytes), (checked), valid range: &lt;SSC_k_MIN_TEL_LEN_SHORT&gt; .. &lt;SSC_k_MAX_TEL_LEN_SHORT&gt; OR &lt;SSC_k_MIN_TEL_LEN_LONG&gt; .. &lt;SSC_k_MAX_TEL_LEN_LONG&gt; AND <= (EPLS_k_MAX_HDR_LEN +  (2 * EPLS_cfg_MAX_PYLD_LEN))
 *
 * @return
 * - SSC_k_OK        - frame is completely processed (error may be signaled via SERR_SetError())
 * - SSC_k_BUSY      - frame is currently processed
 */
SSC_t_PROCESS SSC_ProcessSNMTSSDOFrame(BYTE_B_INSTNUM_ UINT32 dw_ct,
        const UINT8 *pb_rxFrm,
        UINT16 w_rxFrmLen);

#endif

/** @} */
/** @} */
