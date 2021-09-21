/**
 * @addtogroup SSDOC
 * @{
 *
 * @file SSDOCapi.h
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SSDOCapi.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SLV10</td></tr>
 * </table>
 */

#ifndef SSDOCAPI_H
#define SSDOCAPI_H

/**
 * This symbol represents the maximum data length of a service request from
 * a SSDO Client.
 *
 * The data also includes SOD Access Command, index and sub-index of object
 * to be transferred.
 *
 * @note max. 254 service data bytes
*/
#define SSDOC_k_MAX_SERV_DATA_LEN      0xFEu


/**
 * This symbol represents the minimum data length of a service request from
 * a SSDO Client.
 *
 * The data includes SOD Access Command, index and sub-index of object beside
 * the payload data itself to be transferred.
 *
 * @note min. 8 service data bytes
*/
#define SSDOC_k_MIN_SERV_DATA_LEN      0x08u

/**
 * Prototype of the callback function to inform the requesting unit (SCM, APL)
 * that the request was responded by the SSDO server.
 *
 * To handle a failed communication, an abort code is provided in the callback
 * interface. This type is unique for all callback functions used to transfer
 * response data to the requesting unit
 *
 * @param w_reqNum 			service request number to assign the callback to the
 * 							previous request
 * @param dw_transDataLen 	number of transferred data bytes
 * @param dw_abortCode
 * 		- == SOD_ABT_NO_ERROR: no error present response buffer and dw_transDataLen
 * 		  are valid
 * 		- <> SOD_ABT_NO_ERROR: error present, dw_abortCode contains the abort code,
 * 		  response buffer and dw_transDataLen are not relevant
 */
typedef void (*SSDOC_t_RESP_CLBK) (UINT16 w_reqNum, UINT32 dw_transDataLen,
                                   UINT32 dw_abortCode);


/**
 * This data structure is used to pass several request information from the SCM to
 * the unit SSDOC.
 *
 * Every element of this data structure is part of API interface.
*/
typedef struct
{
  /**
   * Index of object to be accessed (not checked, any value allowed)
   * Valid range: (UINT16)
   */
  UINT16 w_idx;
  /**
   * sub index of object to be accessed (not checked, any value allowed),
   * valid range: (UINT8)
   */
  UINT8 b_subIdx;
  /** data type of data to be transferred (checked) see EPLS_t_DATATYPE */
  EPLS_t_DATATYPE e_dataType;
  /**
   * maximum number of payload data bytes to be transferred within a
   * single frame (checked), valid range:
   * SSDOC_k_MIN_SERV_DATA_LEN..SSDOC_k_MAX_SERV_DATA_LEN
   */
  UINT8 b_payloadLen;
  /**
   * - Read access  : pointer to the response buffer
   * - Write access : pointer to the data to be written (pointer checked), valid range: <> NULL
   */
  UINT8 *pb_data;
  /**
   * - Read access  : length of the response buffer, (checked) valid range :
   * 				  >= data type length for standard data type, otherwise > 0
   * - Write access : number of data bytes to be written into specified SOD.
   *                  Only relevant for data type:
   *                   - EPLS_k_VISIBLE_STRING,
   *                   - EPLS_k_DOMAIN,
   *                   - EPLS_k_OCTET_STRING
   *                  0 means that no data bytes are transferred and the actual
   *                  length of the object is set to 0 (checked) valid range: UINT32
   */

  UINT32 dw_dataLen;
} SSDOC_t_REQ;

/**
* @brief This function transmits a request for write access to a specified SSDO Server.
*
* In dependence of the data length the transmission is segmented or unsegmented.
* The number of segments is also determined by the max. number of payload data
* bytes to be transferred within one segment. The SSDO server is addressed by its
* logical address (SADR). The SSDO client is able to communicate with many servers
* thus this function needs to request a communication channel internally. The memory
* that holds the data to be written is provided by the calling function.
*
* @attention This function must not be called in SNMTS_k_ST_INITIALIZATION state.
* The memory of the data to be written to has to be reserved until the response of
* the SSDO server has received. That means until the callback function is called.
* Parallel request to one SN is not allowed. The function must not be called if
* SSDOC_CheckFsmAvailable() returns FALSE otherwise FATAL error is generated.
*
* @see          SSDOC_CheckFsmAvailable()
*
* @param        w_sadr                target address of SN to be accessed
*                                     (checked) valid range:
*                                     EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
* @param         w_reqNum 			  service request number to assign the
*                                     request in the callback
*                                     (not checked, any value allowed)
*                                     valid range: UINT16
* @param        pf_respClbk           reference to a function that will be
*                                     called at the end of a transfer
*                                     (checked) valid range: <> NULL
* @param        dw_ct 				  consecutive time (not checked, any value
*                                     allowed),
*                                     valid range: UINT32
* @param       ps_req 			      request info of data to be process
*                                     (checked) valid range: <> NULL,
*                                     see SSDOC_t_REQ
* @param        o_fastDld             fast download requested
*
* @return
* - TRUE                - transmission of request successful
* - FALSE               - transmission of request failed
*/
BOOLEAN SSDOC_SendWriteReq(UINT16 w_sadr, UINT16 w_reqNum,
                           SSDOC_t_RESP_CLBK pf_respClbk, UINT32 dw_ct,
                           SSDOC_t_REQ *ps_req, BOOLEAN const o_fastDld);


/**
* @brief This function transmits a request for read access to a specified SSDO Server.
*
* In dependence of the data length the transmission is segmented or unsegmented. The
* number of segments is also determined by the max. number of payload data bytes to be
* transferred within one segment. Also the safety node which acts as a server is
* specified. The safety node server is addressed by its domain unique address (SADR).
* The client is able to communicate with many servers thus this function needs to request
* a communication channel internally.
*
* @attention This function must not be called in SNMTS_k_ST_INITIALIZATION state. The
* memory of the data to be read has to be reserved until the response of the SSDO server
* has received. That means until the callback function is called. Parallel request to
* one SN is not allowed. The function must not be called if SSDOC_CheckFsmAvailable()
* returns FALSE otherwise FATAL error is generated.
*
* @see          SSDOC_CheckFsmAvailable()
*
* @param        w_sadr 				   target address of SN to be accessed (checked), valid range: EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @param        w_reqNum               service request number to assign the request in the callback(not checked, any value allowed), valid range: UINT16
*
* @param        pf_respClbk            reference to a function that will be called at the end of a transfer (checked), valid range: <> NULL
*
* @param        dw_ct				   consecutive time (not checked, any value allowed), valid range: UINT32
*
* @param        ps_req                 request info of data to be process(checked) valid range: <> NULL, see {SSDOC_t_REQ}
*
* @return
* - TRUE                 - transmission of request successful
* - FALSE                - transmission of request failed
*/
BOOLEAN SSDOC_SendReadReq(UINT16 w_sadr, UINT16 w_reqNum,
                          SSDOC_t_RESP_CLBK pf_respClbk, UINT32 dw_ct,
                          const SSDOC_t_REQ *ps_req);

/**
* @brief This function checks whether one FSM is available to process SSDO data transfer.
*
* @see SSDOC_SendReadReq()
* @see SSDOC_SendWriteReq()
*
* @return
* - TRUE  - at least one SSDO Client FSM available
* - FALSE - no SSDO Client FSM available
*/
BOOLEAN SSDOC_CheckFsmAvailable(void);


/**
* @brief This function handles service request repetitions in case of a precedent
* service request was not responded by the specified SSDO Server in time or not
* responded at all.
*
* Generally this function checks every SSDOC Client service FSM pausing in state
* "wait for response" for response timeout.
*
* @attention This function must not be called in SNMTS_k_ST_INITIALIZATION
* state. This function must be called at least once within the  SSDO timeout (SOD index
* EPLS_k_IDX_SSDO_COM_PARAM, sub-index EPLS_k_SUBIDX_SNMT_TIMEOUT) with OUT value
* of number of free frame > 0.
*
* @param dw_ct consecutive time, (not checked, any value allowed), valid range: any 32 bit value
*
* @retval pb_numFreeFrms
* - IN: number of openSAFETY frames are free to transmit. (checked), valid range: <> NULL, value > 0
* - OUT: remaining number of free frames. If the value is zero, then not all SSDO channel may checked.
*/
void SSDOC_BuildRequest(UINT32 dw_ct, UINT8 *pb_numFreeFrms);

#endif

/** @} */
