/**
 * @addtogroup SSDOC
 * @{
 *
 * @file SSDOCint.h
 *
 * This file is the internal header-file of the unit.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SSDOint.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>11.09.2014</td><td>Hans Pill</td><td>changes for preload</td></tr>
 *     <tr><td>25.09.2014</td><td>Hans Pill</td><td>preload is not used for every upload</td></tr>
 * </table>
 *
 */

#ifndef SSDOCINT_H
#define SSDOCINT_H

/**
 * This symbol represents the maximum number of SSDO transfers that can be
 * process quasi parallel.
*/
#define SSDOC_cfg_MAX_NUM_FSM          SCM_cfg_MAX_NUM_OF_NODES


/**
 * This symbol represents an invalid value of a SSDOC FSM number.
 *
 * @attention The maximum number of SN to be configured concurrent must
 * be less than k_INVALID_FSM_NUM.
*/
#define k_INVALID_FSM_NUM             0xFFFFu


/**
 * @name Unused Symbols
 * @{
 *
 * @brief These symbols are used in case of the specific parameter is not used in
 * the function call.
*/
/** ct is not used */
#define k_CT_NOT_USED              0UL
/** ref to frame header info */
#define k_REF_HDR_INFO_NOT_USED    (EPLS_t_FRM_HDR *)NULL
/** @} */


/**
 * Enumeration of events the service FSM of the SSDO Client is able to process.
*/
typedef enum
{
  /** SSDO Service is requested */
  k_EVT_REQ,
  /** resp from SSDO Server received */
  k_EVT_RX_RESP,
  /** request buffer retransmission */
  k_EVT_RETRANS,
  /** abort frame is sent */
  k_EVT_ABORT_SEND
} t_SERV_FSM_EVENT;

/**
 * Structured data type to store the request header and the payload data of
 * the request for the (re)transmission.
*/
typedef struct
{
  /** frame header info of the request */
  EPLS_t_FRM_HDR s_hdr;
  /** data buffer of the request */
  UINT8 ab_data[SSDOC_k_MAX_SERV_DATA_LEN];
} t_REQ_BUFFER;

/**
* @brief This function initiates the SSDOC Protocol Layer and the SSDOC Service layer.
*
* @return
* - TRUE  - success
* - FALSE - failure
*/
BOOLEAN SSDOC_ProtocolLayerInit(void);

/**
* @brief This function processes the SSDO Server responses and sends the next SSDOC
* request if it is necessary.
*
* @param        dw_ct                consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_fsmNum             protocol FSM number (not checked, checked in SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        pb_respData          ref to resp data (not checked, only called with reference to array in processStateReqProc()), valid range: <> NULL
*
* @param        b_respDataLen        number of received data bytes (not checked, checked in checkResponse() and checkRxDataLength()), valid range: k_DATA_HDR_LEN .. k_MAX_DATA_LEN
*
* @param        b_tr                 tr field of the response frame
*
* @return
* - TRUE              - FSM processing successful
* - FALSE             - FSM processing failed
*/
BOOLEAN SSDOC_ProtocolFsmProcess(UINT32 dw_ct, UINT16 w_fsmNum,
                                 const UINT8 *pb_respData, UINT8 b_respDataLen,
                                 UINT8 b_tr);

/**
* @brief This function reserves a free protocol FSM, stores the request information
* for the FSM, sends a SSDO request and sets the FSM into the corresponding "wait for
* response" state.
*
* @param        w_sadr                 target address of SN to be accessed (not checked,
* 	checked in SSDOC_SendReadReq() or SSDOC_SendWriteReq()), valid range:
* 	EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @param        w_reqNum               service request number (handle)(not checked, any
* 	value allowed), valid range: UINT16
*
* @param        pf_respClbk            reference to a function that will be called at the
* 	end of a transfer (not checked checked in SSDOC_SendReadReq() or SSDOC_SendWriteReq()),
* 	valid range: <> NULL
*
* @param        dw_ct                  consecutive time (not checked, any value allowed),
* 	valid range: UINT32
*
* @param        ps_req                 request info of data to be process (not checked,
* 	checked in SSDOC_SendReadReq() or SSDOC_SendWriteReq()), valid range: see SSDOC_t_REQ
*
* @param        o_downloadReq
*  - TRUE  - download request is started
*  - FALSE - upload request is started (not checked, any value allowed)
*
* @param        o_fastDld              fast download requested
*
* @return
* - TRUE                 - success
* - FALSE                - failure
*/
BOOLEAN SSDOC_SendReq(UINT16 w_sadr, UINT16 w_reqNum,
                      SSDOC_t_RESP_CLBK pf_respClbk, UINT32 dw_ct,
                      const SSDOC_t_REQ *ps_req, BOOLEAN o_downloadReq,
                      BOOLEAN const o_fastDld);

/**
* @brief This function checks the SaNo of a received SSDO telegram.
*
* @param         w_recSaNo           SaNo of the received telegram,
*     valid range: any 16 bit value
*
* @param         w_lastSentSaNo      SaNo of last sent SSDO telegram,
*     valid range: any 16 bit value
*
* @param        w_fsmNum            protocol FSM number (not checked, checked in
*     SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()),
*     valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @return
* - TRUE              - SaNo is valid
* - FALSE             - SaNo is invalid
*/
BOOLEAN SSDOC_CheckSaNo(UINT16 const w_recSaNo, UINT16 const w_lastSentSaNo, UINT16 const w_fsmNum);

/**
* @brief This function initiates the SSDOC Service Layer.
*
* This includes the SSDOC Service FSM, SSDOC Service Timer and the logical
* address (SADR) of the SCM from the SOD.
*
* @return
* - TRUE  - initialization succeeded
* - FALSE - initialization failed
*/
BOOLEAN SSDOC_ServiceLayerInit(void);

/**
* @brief This function initiates the SSDOC Service Request "SSDO Initiate Download"
* to start download transfer to a specific SN (SOD) inside the Safety Domain.
*
* @param        w_fsmNum           FSM number (not checked, checked in SSDOC_SendReq()
* 	and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        dw_ct              consecutive time (not checked, any value allowed),
* 	valid range: any 32 bit value
*
* @param        w_sadr             logical address of SN which shall respond its
* 	physical address (UDID)(not checked, checked in SSDOC_SendReadReq() or
* 	SSDOC_SendWriteReq()), valid range: EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @param        o_expedited
* 	- TRUE : expedited download service is sent
* 	- FALSE : segmented download service is sent (not checked, any value allowed)
*
* @param        ps_data            ref to all values that are stored into the frame
* 	data area (not checked, only called with reference to struct in SSDOC_SendReq()),
* 	valid range: <> NULL
*
* @param        o_fastDld          fast download requested
*
* @return
* - TRUE             - req transmission successfully
* - FALSE            - req transmission failed
*/
BOOLEAN SSDOC_ServiceDwnldInit(UINT16 w_fsmNum, UINT32 dw_ct, UINT16 w_sadr,
                               BOOLEAN o_expedited, const SSDOC_t_REQ *ps_data,
                               BOOLEAN const o_fastDld);

/**
* @brief This function initiates the SSDOC Service Request "SSDO Segment Download" to
* continue the download transfer.
*
* @param        w_fsmNum        FSM number (not checked, checked in SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        dw_ct           consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        b_saCmd 		SOD access command byte to be sent (not checked, only called with define in processDwnldRespSeg()), valid range : k_ST_WF_DWNLD_RESP_MID_SEG or k_ST_WF_DWNLD_RESP_END_SEG
*
* @param        ps_data         ref to all values that are stored into the frame data area (not checked, only called with reference to struct), valid range: <> NULL
*
* @return
* - TRUE          - req transmission successfully
* - FALSE         - req transmission failed
*/
BOOLEAN SSDOC_ServiceDwnldSeg(UINT16 w_fsmNum, UINT32 dw_ct, UINT8 b_saCmd,
                              const SSDOC_t_REQ *ps_data);

/**
* @brief This function initiates the SSDOC Service Request "SSDO Initiate Upload" to start
* Upload transfer to a specific SN (SOD) inside the Safety Domain.
*
* @param        w_fsmNum        FSM number (not checked, checked in SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        dw_ct           consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_sadr          logical address of SN which shall resp. its physical address (UDID) (not checked, checked in SSDOC_SendReadReq() or SSDOC_SendWriteReq()), valid range: EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @param        ps_data         ref to all values that are stored into the frame data area (not checked, only called with reference to struct), valid range: <> NULL
*
* @return
* - TRUE          - req transmission successfully
* - FALSE         - req transmission failed
*/
BOOLEAN SSDOC_ServiceUpldInit(UINT16 w_fsmNum, UINT32 dw_ct, UINT16 w_sadr,
                              const SSDOC_t_REQ *ps_data);

/**
* @brief This function initiates the SSDOC Service Request "SSDO Segment Upload" to
* continue the Upload transfer.
*
* @param        w_fsmNum        FSM number (not checked, checked in SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        dw_ct           consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        b_saCmd         SOD access command byte to be sent (not checked, only called with define in processUpldRespSeg()), valid range : k_ST_WF_UPLD_RESP_MID_SEG or k_ST_WF_UPLD_RESP_END_SEG
*
* @return
* - TRUE          - req transmission successfully
* - FALSE         - req transmission failed
*/
BOOLEAN SSDOC_ServiceUpldSeg(UINT16 w_fsmNum, UINT32 dw_ct, UINT8 b_saCmd);

/**
* @brief This function initiates the SSDOC Service Request "SSDO Abort" to inform a
* specific SN inside the Safety Domain about data transfer abortion.
*
* @param        w_fsmNum            number of FSM that processes a transfer to be aborted (not checked, checked in SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @param        dw_abortCode        abort code to be sent (not checked, only called with enum in SSDOC_BuildRequest() or SSDOC_ProtocolFsmProcess()), valid range: see SOD_t_ABORT_CODES
*
* @return
* - TRUE              - request transmission successfully
* - FALSE             - request transmission failed
*/
BOOLEAN SSDOC_ServiceAbort(UINT16 w_fsmNum, UINT32 dw_abortCode);

/**
* @brief This function initiates the SSDOC Service FSM and the SSDOC
* Service timer.
*
* @return
* - TRUE  - initialization succeeded
* - FALSE - initialization failed
*/
BOOLEAN SSDOC_ServiceFsmInit(void);

/**
* @brief This function sets the "wait for request"bit to the desired value.
*
* @param        w_fsmNum        SSDO Client FSM number (not checked, checked in
*     SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_SendReq() and
*     getProtocolFsmFree() or SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        o_waitForReq    desired value for the bit
*/
void SSDOC_ServiceFsmSetWaitForReq(UINT16 const w_fsmNum, BOOLEAN const o_waitForReq);

/**
* @brief This function sets the "preload" bit to the desired value.
*
* @param        w_fsmNum        SSDO Client FSM number (not checked, checked in
*     SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_SendReq() and
*     getProtocolFsmFree() or SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        o_preload    desired value for the bit
*/
void SSDOC_ServiceFsmSetPreload(UINT16 const w_fsmNum, BOOLEAN const o_preload);

/**
* @brief This function gets a reference to the request buffer in the service FSM.
*
* @param        w_fsmNum        SSDO Client FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_SendReq() and
* 	getProtocolFsmFree() or SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @return       pointer to the request buffer
*/
t_REQ_BUFFER *SSDOC_GetReqBuffer(UINT16 w_fsmNum);

/**
* @brief This function checks the response timeout of SSDO Service Requests.
*
* Only if a response failed to receive more often an error is reported to the protocol
* FSM. If no management frames are available the response timeout is not checked.
*
* @param        dw_ct                         consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @retval       pb_numFreeMngtFrms            valid range: <> NULL
* - IN: max. number of EPLS frames of type SNMT and SSDO. This number of frames are free
*   to transmit during one appl. cycle
* - OUT: remaining number of free frames of type SNMT and SSDO. A value of zero means
*   that all required services were transm. (not checked, checked in SSDOC_BuildRequest())
*
* @param        w_fsmNum                       service FSM number, assigned internally (not checked, checked in SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM-1
*
* @retval          po_abort                      only relevant if return value=TRUE , valid range : <> NULL
* - TRUE : abort frame has to be sent
* - FALSE : no abort frame has to be sent (not checked, only called with reference to
*   variable in SSDOC_BuildRequest())
*
* @return
* - TRUE                        - response timeout check succeeded
* - FALSE                       - response timeout check failed
*/
BOOLEAN SSDOC_CheckTimeout(UINT32 dw_ct, UINT8 *pb_numFreeMngtFrms,
                           UINT16 w_fsmNum, BOOLEAN *po_abort);

/**
* @brief This function processes the SSDO Client Service FSM.
*
* If a response received then it is call by the protocol layer with "response received"
* event. If a timeout happened then it is called with retransmission event. Request
* event is generated by the service request functions (SSDOC_ServiceDwnldInit(),
* SSDOC_ServiceDwnldSeg(), SSDOC_ServiceUpldInit() SSDOC_ServiceUpldSeg()). Send abort
* event is generated by the service abort request function (SSDOC_ServiceAbort()).
*
* @param        dw_ct             consecutive time (not checked, any value allowed),
* 	valid range: any 32 bit value
*
* @param        w_fsmNum          SSDO Client FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest()
* 	or SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        e_fsmEvent        event which shall be processed by the FSM. (checked),
* 	valid range: see t_SERV_FSM_EVENT
*
* @param        w_respCt          only relevant if the event is  k_EVT_RESP_RECEIVED,
* 	otherwise 0 consecutive time in the response header (checked), valid range: request Ct
*
* @param        b_respTr          only relevant if the event is k_EVT_RESP_RECEIVED,
* 	otherwise 0 time request distinctive number (checked), valid range: EPLS_k_TR_NOT_USED
*
* @return
* - TRUE            - If the calling event is k_EVT_RESP_RECEIVED then the received frame
*   processing succeeded, otherwise the request transmission succeeded.
* - FALSE           - If the calling event is k_EVT_RESP_RECEIVED then the received frame
*   processing failed, Minor error is generated and frame is ignored, otherwise the request
*   transmission failed, FSM is reinitialized and FATAL error is already generated by the
*   SFS.
*/
BOOLEAN SSDOC_ServiceFsmProcess(UINT32 dw_ct, UINT16 w_fsmNum,
                                t_SERV_FSM_EVENT e_fsmEvent, UINT16 w_respCt,
                                UINT8 b_respTr);

/**
* @brief This function checks whether a SSDO transfer is already running with the given SN.
*
* @param        w_sadr        target address of SN to be accessed (not checked, checked
*   in SSDOC_SendReadReq() and SSDOC_SendWriteReq()) valid range: EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
*
* @return
* - TRUE        - SSDO tranfer is already running and minor error was reported.
* - FALSE       - SSDO tranfer is not running.
*/
BOOLEAN SSDOC_TransferRunning(UINT16 w_sadr);

/**
* @brief This function assigns a single received response to the matching request that
* is processed in one of the SSDOC service FSM waiting for response.
*
* As result the number of the FSM which processes the request/response is returned.
*
* @param        b_respId          frame ID of the received response (not checked, any value allowed), valid range: (UINT8)
*
* @param        w_respSadr 		  address of the node who received the resp
* 	(target address resp) (not checked, checked in checkRxAddrInfo()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        w_respTadr        address of the node who transmitted the resp
* 	(source address resp) (not checked, checked in checkRxAddrInfo()), valid range:
* 	EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param w_respSdn SDN in the received response (not checked, checked in
*   checkRxAddrInfo()), valid range: k_MIN_SDN .. k_MAX_SDN
*
* @return       0 .. (SSDOC_cfg_MAX_NUM_FSM - 1) - FSM number, response has been assigned
*   k_INVALID_FSM_NUM - response can not be assigned
*/
UINT16 SSDOC_ServiceResponseAssign(UINT8 b_respId, UINT16 w_respSadr,
                                   UINT16 w_respTadr, UINT16 w_respSdn);

/**
* @brief This function changes the download type from preload to normal.
*
* @param w_fsmNum SSDO Client FSM number (not checked, checked in
*     SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest() or
*     SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @return
* - TRUE            - preload was turned off
* - FALSE           - preload could not get turned off
*/
BOOLEAN SSDOC_ReqBufferPreLoadOff(UINT16 const w_fsmNum);

/**
* @brief This function initializes the SOD access structures and
*               the as_ServTimer structure.
*
* @return
* - TRUE           - success
* - FALSE          - failure
*/
BOOLEAN SSDOC_TimerInit(void);

/**
* @brief This function starts the given response timer.
*
* @param        w_fsmNum        FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest()
* 	or SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        dw_ct           consecutive time (not checked, any value allowed),
* 	valid range: any 32 bit value
*
* @return
* - TRUE          - success
* - FALSE         - failure
*/
BOOLEAN SSDOC_TimerStart(UINT16 w_fsmNum, UINT32 dw_ct);

/**
* @brief This function restarts the given response timer.
*
* @param        w_fsmNum        FSM number (not checked, checked in
* 	SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest()
* 	or SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        dw_ct           consecutive time (not checked, any value allowed),
* 	valid range: any 32 bit value
*/
void SSDOC_TimerReStart(UINT16 w_fsmNum, UINT32 dw_ct);

/**
* @brief This function sets the given response timer to timeout.
*
* @param        w_fsmNum        FSM number (not checked, checked in
*     SSDOC_ServiceResponseAssign() and SSDOC_ProcessResponse() or SSDOC_BuildRequest()
*     or SSDOC_SendReq() and getProtocolFsmFree()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*/
void SSDOC_TimerToTimeout(UINT16 const w_fsmNum);

/**
* @brief This function checks the given response timer.
*
* @param        w_fsmNum           FSM number (not checked, checked in
* 	SSDOC_BuildRequest()), valid range: 0 .. SSDOC_cfg_MAX_NUM_FSM - 1
*
* @param        dw_ct              consecutive time (not checked, any value allowed),
*   valid range: any 32 bit value
*
* @retval        po_timeout
*  - TRUE : timeout
*  - FALSE : no timeout (pointer not checked, only called with reference to
*    variable in SSDOC_CheckTimeout()), valid range: <> NULL
* @retval        po_retrans           only relevant if timeout happened (po_timeout = TRUE), valid range: <> NULL
*  - TRUE : retransmission is required
*  - FALSE : retransmission is not required, abort code has to be sent (pointer not
*    checked, only called with reference to variable in SSDOC_CheckTimeout())
*/
void SSDOC_TimerCheck(UINT16 w_fsmNum, UINT32 dw_ct, BOOLEAN *po_timeout,
                      BOOLEAN *po_retrans);

#endif


/** @} */
