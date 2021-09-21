/**
 * @addtogroup SSDOS
 * @{
 *
 *
 * @file SSDOSint.h
 * This file is the internal header-file of the unit.
 *
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SSDOsint.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>11.09.2014</td><td>Hans Pill</td><td>changes for preload</td></tr>
 *     <tr><td>11.01.2017</td><td>Stefan Innerhofer</td><td>add function prototype for SSDOS_SodAcsCheckAndReadObject()</td></tr>
 * </table>
 *
 */

#ifndef SSDOSINT_H
#define SSDOSINT_H


/**
 * @name Function prototypes for SSDOSupDwnLd.c
 * @{
 *
***/
/**
* @brief This function initializes the structure with the segment information.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll() or SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*/
void SSDOS_SegInfoInit(BYTE_B_INSTNUM);

/**
* @brief This function initializes the size of the SSDOS queue.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll() or SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*/
void SSDOS_QueueSizeInit(BYTE_B_INSTNUM);

/**
* @brief This function processes a SSDO expedited initiate download request.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )

* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed
*       (pointer not checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldInitExpReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf);

/**
* @brief This function processes a SSDO segmented initiate download request.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed
*       (pointer not checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldInitSegReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf);

/**
* @brief This function generates a SSDOS initiate segmented download response.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame  to be transmitted (pointer not checked, only
*       called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldInitSegRespSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                  EPLS_t_FRM *ps_txBuf);
/**
* @brief This function generates a SSDOS initiate pre-load segmented download response.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame  to be transmitted (pointer not checked, only
*       called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldInitPrelSegRespSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                  EPLS_t_FRM *ps_txBuf);

/**
* @brief This function generates a SSDOS initiate expedited download response.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame to be transmitted (pointer not checked, only called
*       with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldInitExpRespSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                  EPLS_t_FRM *ps_txBuf);

/**
* @brief This function processes a SSDO segmented middle download request.
*
* @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf           reference to received openSAFETY frame to be distributed (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL,
*       see EPLS_t_FRM
*
* @param        ps_txBuf           reference to openSAFETY frame to be transmitted (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        b_echoSaCmd        the SOD access command to be sent back (not checked, any value allowed),
*       valid range: UINT8
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldMidSegReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                EPLS_t_FRM *ps_txBuf, UINT8 b_echoSaCmd);
/**
* @brief This function processes a SSDO segmented middle download request.
*
* @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf           reference to received openSAFETY frame to be distributed (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL,
*       see EPLS_t_FRM
*
* @param        ps_txBuf           reference to openSAFETY frame to be transmitted (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        b_echoSaCmd        the SOD access command to be sent back (not checked, any value allowed),
*       valid range: UINT8
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldMidPrelSegReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                EPLS_t_FRM *ps_txBuf, UINT8 b_echoSaCmd);

/**
* @brief This function processes a SSDO segmented end download request.
*
* @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf           reference to received openSAFETY frame to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf           reference to openSAFETY frame to be transmitted (pointer not checked, only
*       called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        b_echoSaCmd        the SOD access command to be sent back (not checked, any value allowed), valid range: UINT8
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldEndSegReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                EPLS_t_FRM *ps_txBuf, UINT8 b_echoSaCmd);

/**
* @brief This function processes a SSDO segmented initiate upload request.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed (pointer
*       not checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @retval       po_seg           only relevant if SOD_ABT_NO_ERROR returned,
* - TRUE : segmented upload
* - FALSE : expedited upload (pointer not checked, only called with reference to variable in WfReqInitState()), valid range: TRUE, FALSE
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_UpldInitReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                             BOOLEAN *po_seg);

/**
* @brief This function generates a SSDOS initiate expedited upload response.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame  to be transmitted (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_UpldInitExpRespSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf);

/**
* @brief This function generates a SSDOS initiate segmented upload response.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed (pointer
*       not checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame  to be transmitted (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*/
void SSDOS_UpldInitSegRespSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                               EPLS_t_FRM *ps_txBuf);

/**
* @brief This function processes a SSDO segmented upload request.
*
* @param      b_instNum            instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param      ps_rxBuf             reference to received openSAFETY frame  to be distributed (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param      ps_txBuf             reference to openSAFETY frame  to be transmitted (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @retval        po_end               only relevant if SOD_ABT_NO_ERROR returned,
* - TRUE : end segment
* - FALSE : middle segment (pointer not checked, only called with reference to variable), valid range: TRUE, FALSE
*
* @param      b_echoSaCmd          the SOD access command to be sent back (not checked, any value allowed), valid range: UINT8
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_UpldSegReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                            EPLS_t_FRM *ps_txBuf, BOOLEAN *po_end,
                            UINT8 b_echoSaCmd);

/** @} */

/**
 * @name Function prototypes for SSDOSsodAcs.c
 * @{
 */

/**
* @brief This function initializes SOD access structure.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll() or SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
*/
void SSDOS_SodAcsInit(BYTE_B_INSTNUM);

/**
* @brief This function clears the object data in case of an abort of the segmented download and calls the SOD_Unlock() if the SOD was locked.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
* @param        o_abort
* - TRUE : reset because an abort frame was sent or received
* - FALSE : reset because of end of the transfer (not checked, any value allowed), valid range: TRUE, FALSE
*
* @return
* - TRUE           - reset of SOD access succeeded
* - FALSE          - reset of SOD access failed, FATAL error is already reported
*/
BOOLEAN SSDOS_SodAcsReset(BYTE_B_INSTNUM_ BOOLEAN o_abort);

/**
* @brief This function checks whether the SOD access request in the SSDOC request is valid.
*
* Toggle bit, SOD index and SOD sub-index are checked.
*
* @param        b_instNum            instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxPyldData        reference to the payload data in the received openSAFETY frame
*       (pointer not checked, only called with reference to array in DwnldWfReqSegState() or UpldWfReqSegState()),
*       valid range: see EPLS_t_FRM
*
* @param        o_checkObj           objet index subindex is to be checked
*
* @return
* - TRUE               - SSDOC Request is valid
* - FALSE              - SSDOC Request is invalid
*/
BOOLEAN SSDOS_SodAcsReqValid(BYTE_B_INSTNUM_ const UINT8 *ps_rxPyldData, BOOLEAN const o_checkObj);

/**
* @brief This function gets the data type of the accessed object.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @return       data type of the object see EPLS_t_DATATYPE
*/
EPLS_t_DATATYPE SSDOS_SodAcsDataTypeGet(BYTE_B_INSTNUM);

/**
* @brief This function copies SOD index and SOD sub-index from the received SSDOC request.
*
* @param        b_instNum            instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        ps_rxPyldData        reference to the payload data in the received openSAFETY frame
*       (pointer not checked, only called with reference to array in WfReqInitState() or SSDOS_UpldInitReqProc()
*       or SSDOS_DwnldInitSegReqProc() or SSDOS_DwnldInitExpReqProc()), valid range: see EPLS_t_FRM
*/
void SSDOS_SodAcsIdxCopy(BYTE_B_INSTNUM_ const UINT8 *ps_rxPyldData);

/**
* @brief This function gets the SOD attributes for the corresponding SOD index and SOD sub-index.
*
* @param        b_instNum            instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxPyldData        reference to the payload data in the received openSAFETY frame
*       (pointer not checked, only called with reference to array in SSDOS_UpldInitReqProc() or SSDOS_DwnldInitSegReqProc()
*       or SSDOS_DwnldInitExpReqProc()), valid range: see EPLS_t_FRM
*
* @param        o_readAcs
* - TRUE  : read access is requested
* - FALSE : write access is requested (not checked, any value allowed), valid range: TRUE, FALSE
*
* @param        pdw_objSize          length of the object (pointer not checked, only called with reference to variable in SSDOS_UpldInitReqProc() or SSDOS_DwnldInitSegReqProc() or SSDOS_DwnldInitExpReqProc()), valid range: <> NULL
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_SodAcsAttrGet(BYTE_B_INSTNUM_ const UINT8 *ps_rxPyldData,
                           BOOLEAN o_readAcs, UINT32 *pdw_objSize);

/**
* @brief This function checks the current SN state. Only in state PREOPERATIONAL an SOD write access is allowed.
*
* In state OPERATIONAL only SOD read access is possible. In case of a shared SOD object to be accessed all instances
* are checked for state OPERATIONAL. Write access to a shared object is only possible in case of no instance of SNs
* is in state OPERATIONAL.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
* @param        o_seg
* - TRUE : segmented download
* - FALSE : expedited download (not checked, any value allowed), valid range: TRUE, FALSE
*
* @return
* - TRUE           - SOD write access is allowed
* - FALSE          - SOD write access is not allowed
*/
BOOLEAN SSDOS_SodAcsWriteAllowed(BYTE_B_INSTNUM_ BOOLEAN o_seg);

/**
* @brief This function converts the data from network format to host format and writes into the SOD.
*
* @param        b_instNum          instance number (not checked, checked in
*                                  SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
* @param        pb_data            pointer to the data to be written, (pointer not checked, only called
*       with reference to array in SSDOS_DwnldMidSegReqProc() or SSDOS_DwnldEndSegReqProc or SSDOS_DwnldInitExpRespSend()
*       or SSDOS_DwnldInitSegRespSend()), valid range: <> NULL
*
* @param        dw_offset          start offset in bytes of the segment within the data block (not checked,
*       checked in SOD_Write()) valid range: UINT32
*
* @param        dw_size            size in bytes of the segment (not checked, checked in SOD_Write()) valid range: (UINT32)
*
* @param        o_actLenSet
* - TRUE  : set actual length
* - FALSE : do not set actual length (not checked, any value allowed), valid range: TRUE, FALSE
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code.
*/
UINT32 SSDOS_SodAcsWrite(BYTE_B_INSTNUM_ const UINT8 *pb_data, UINT32 dw_offset,
                         UINT32 dw_size, BOOLEAN o_actLenSet);

/**
* @brief This function reads the data the SOD.
*
* @param        b_instNum             instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
* @param        dw_offset             start offset in bytes of the segment within the data block
*       (not checked, checked in SOD_Read()), valid range: (UINT32)
* @param        dw_size               size in bytes of the segment (not checked, checked in
*       SOD_Read()), valid range: (UINT32)
* @retval          pdw_abortCode         abort code, if SOD_ABT_NO_ERROR no abort response has to be sent,
*       otherwise abort response has to be sent with the this abort code. (pointer not checked, only called
*       with reference to variable in SSDOS_UpldInitReqProc() or SSDOS_UpldSegReqProc()) valid range: <> NULL, see SOD_t_ABORT_CODES
*
* @return
* - <> NULL             - pointer to the data, if pdw_abortCode is SOD_ABT_NO_ERROR
* - == NULL             - read access failed, see pdw_abortCode
*/
UINT8 *SSDOS_SodAcsRead(BYTE_B_INSTNUM_ UINT32 dw_offset, UINT32 dw_size,
                        UINT32 *pdw_abortCode);

/**
* @brief This function locks the SOD.
*
* @param        b_instNum        instance number (not checked, checked in  SSC_ProcessSNMTSSDOFrame()), valid range:  0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has  to be sent with the returned abort code.
*/
UINT32 SSDOS_SodAcsLock(BYTE_B_INSTNUM);

/**
* @brief This function assembles a SSDO Service Response for the SOD access except the raw data.
*
* @param b_instNum - instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame  to be transmitted (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        b_saCmd          SOD access command for the response (not checked, any value allowed), valid range: (UINT8)
*
* @param        b_tr             TR field for the response (not checked, any value allowed), valid range: (UINT8)
*
* @param        b_respLen        length of the response (not checked, any value allowed), valid range: (UINT8)
*
* @param        o_insObjInfo     index and subindex information is needed in the response
*/
void SSDOS_SodAcsResp(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                      EPLS_t_FRM *ps_txBuf, UINT8 b_saCmd, UINT8 b_tr,
                      UINT8 b_respLen, BOOLEAN o_insObjInfo);

/**
* @brief This function checks if a DVI handling object exists for the given instance number.
*        If yes the instance DVI handling object is read instead of the requested one
*
* @param        b_instNum             instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
* @param        ps_acs                pointer to the access structure (pointer checked) for the object
* @param        ps_errRes             error result, only relevant if return value == NULL, (checked), valid range : <> NULL
*
* @return
* - <> NULL             - pointer to the data
* - == NULL             - read access failed
*/
UINT8* SSDOS_SodAcsCheckAndReadObject(BYTE_B_INSTNUM_ SOD_t_ACS_OBJECT_VIRT * ps_acs, SOD_t_ERROR_RESULT * const ps_errRes);
/** @} */

#endif

/** @} */
