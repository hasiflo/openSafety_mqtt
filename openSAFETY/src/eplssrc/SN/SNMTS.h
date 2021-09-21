/**
 * @addtogroup SNMT
 * @{
 * @addtogroup SNMTS [SNMTS] Safety Network Management Slave
 * @{
 *
 * SNMTS, the network management slave unit of the openSAFETY Stack.
 *
 * The SNMT Slave response SNMT Service Requests from the SNMT Master.
 *
 * @file SNMTS.h
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
 * <h2>History for SNMTM.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>added SNMT command for SPDOs with 40 bit CT</td></tr>
 * </table>
 */

#ifndef SNMTS_H
#define SNMTS_H

/**
 * @enum t_SNMTS_EXT_SERV_RESP
 * @brief Enumeration of all extended service responses.
 */
typedef enum
{
    /** The SNMTS acknowledges the state transition on this node into state PRE-OPERATIONAL */
    SNMTS_k_SN_STS_PREOP = 1,
    /** The SNMTS acknowledges the current state is OPERATIONAL */
    SNMTS_k_SN_STS_OP = 3,
    /** The SNMTS acknowledges the assignment of an additional SADR on this node */
    SNMTS_k_ADD_SADR_ASSGN = 5,
    /** The SNMTS reports error information to the SNMTM */
    SNMTS_k_SN_FAIL = 7,
    /**
     * The SNMTS signals to the SNMTM (SCM):
     * -  waiting for CRC checksum from API
     * - waiting for API confirmation to switch to OP
     */
    SNMTS_k_SN_STS_BUSY = 9,
    /** The SCM response the assignment of the UDID of SCM */
    SNMTS_k_UDID_SCM_ASSGN = 15,
    /** The SCM response the initialization of the CT */
    SNMTS_k_EXT_CT_INITIALIZE = 17
} t_SNMTS_EXT_SERV_RESP;

/**
 * @brief This function calls the SNMTS initialization function.
 *
 * @param       b_instNum  		 instance number (not checked, checked in SSC_InitAll()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - initialization succeed
 * - FALSE          - initialization failed
 */
BOOLEAN SNMTS_Init(BYTE_B_INSTNUM);

/**
 * @brief This function processes a single received openSAFETY frame that was identified by the control unit (SSC)
 * as a network management service request.
 *
 * "Processing" means that the function also collects all frame header
 * information for the response and stores them into a data structure which is provided by the stack. The payload
 * to be transmitted inside the response is also stored. For that reason a memory block is requested from the unit
 * SHNF. The memory block is of size of the whole openSAFETY frame, but only payload of sub frame ONE is written into.
 * This function is called by the control unit (SSC) which provides a reference to an empty data structure for frame
 * header information to be filled inside this function.
 *
 * @note For timing improvement a SNMT Service Request is
 * processed in several steps. For that reason this function has to be called several times to process the request
 * completely.
 *
 * @param  b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
 *            valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
 *
 * @param        ps_rxBuf           reference to received openSAFETY frame to be distributed (not checked, only called with
 *                  reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf           reference to openSAFETY frame to be transmitted (not checked, only called with reference to
 *                  struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @retval       po_busy            flag signals that SNMT Slave is currently processing a request (not checked,
 *                    only called with reference to variable in processStateReqProc()), valid range:
 *                     - TRUE:  Slave busy
 *                     - FALSE: Slave ready to process
 *
 * @return
 * - TRUE             - request successfully processed
 * - FALSE            - request processing failed (error was reported) or SNMTS SN Ack successfully
 *                       processed (no error was reported) Note : FALSE is returned in order not to generate a response.
 */
BOOLEAN SNMTS_ProcessRequest(BYTE_B_INSTNUM_ UINT32 dw_ct,
        const EPLS_t_FRM *ps_rxBuf, EPLS_t_FRM *ps_txBuf,
        BOOLEAN *po_busy);

#endif

/** @} */
/** @} */
