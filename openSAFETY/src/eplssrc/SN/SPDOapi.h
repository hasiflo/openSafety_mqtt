/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOapi.h
 *
 * This file is the application interface header-file of the unit.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SPDOapi.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>08.04.2011</td><td>Hans Pill</td><td>A&P258030 interface for SHNF to get the info of the RxSPDOs</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 *     <tr><td>21.02.2013</td><td>Roman Zwischelsberger</td><td>(new)	A&P297705: SPDO_BuildTxSpdoIx()</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>17.11.2016</td><td>Stefan Innerhofer</td><td>changed one byte b_noFreeFrm to two byte w_noFreeFrm</td></tr>
 * </table>
 *
 *
 */


#ifndef SPDOAPI_H
#define SPDOAPI_H

#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
/**
* @brief This function is to be called periodically to inflate the 32-bit CT value of the system to a 64-bit value.
*
* @param        b_instNum        instance number valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct            system CT value (not checked) valid range: 0..UINT32
*
*/
void SPDO_UpdateExtCtValue(BYTE_B_INSTNUM_ UINT32 const dw_ct);
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */

/**
* @brief This function builds TxSPDOs to transmit.
*
* If a TxSPDO was built and sent then pw_noFreeFrm will be decremented.
*
* @attention The smallest Refresh Prescale time in the SOD object with index 0x1C00-0x1FFE and
* sub-index 0x02 gives the call frequency of this function.
*
* @param        b_instNum               instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
*
* @retval       pw_noFreeFrm            reference to the number of the free SPDO frames to be sent (checked) valid range: <> NULL
*/
void SPDO_BuildTxSpdo(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 *pw_noFreeFrm);

/**
* @brief This function is almost identical to SPDO_BuildTxSpdo()
*
* The difference being that the SPDO to be build is spezified by parameter
*
* @param        b_instNum               instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
*
* @param        w_spdoIdx            	index of the SPDO to be built (checked) valid range: >=SPDO_GetNoProcTxSpdo()
*
* @return
* - TRUE                    if the SPDO has been generated
* - FALSE	otherwise
*/
UINT8 SPDO_BuildTxSpdoIx(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_spdoIdx);

/**
* @brief This function processes RxSPDOs and must be called for every received SPDO.
*
* The given SPDO frame header is evaluated and the instance number is determined.
* Three valid Rx SPDO types can be processed depending on the SPDO frame header:
* - data only, no data for time synchronization within
* - data with time request, SPDO producer requests additional time synchronization data within the SPDO
* - data with time response, SPDO producer responses additional time synchronization data within the SPDO
*
* @attention The function must be called before SPDO_CheckRxTimeout() is called.
* The received process data is valid after SPDO_CheckRxTimeout() is called.
*
* @param        b_instNum        instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct            consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
*
* @param        pb_rxFrm         reference to the frame to be processed by the openSAFETY Stack (pointer checked) valid range: <> NULL
*
* @param        w_rxFrmLen       length of the received frame (in bytes) (not checked, checked in checkRxDataLength()) valid range: SSC_k_MIN_TEL_LEN_SHORT.. SSC_k_MAX_TEL_LEN_SHORT or SSC_k_MIN_TEL_LEN_LONG.. SSC_k_MAX_TEL_LEN_LONG AND <= (EPLS_k_MAX_HDR_LEN +  (2 * EPLS_cfg_MAX_PYLD_LEN))
*/
void SPDO_ProcessRxSpdo(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT8 *pb_rxFrm,
                        UINT16 w_rxFrmLen);

/**
* @brief This function is called by the application to signal the openSAFETY Stack that the process
* data for the given Tx SPDO has changed.
*
* This Tx SPDO is immediately sent by the SPDO_BuildTxSpdo() before the refresh prescale timeout.
*
* @see          SPDO_BuildTxSpdo()
*
* @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        w_txSpdoNum        Tx SPDO number (checked) valid range: 1..SPDO_cfg_MAX_NO_TX_SPDO
*/
void SPDO_TxDataChanged(BYTE_B_INSTNUM_ UINT16 w_txSpdoNum);

/**
* @brief This function checks the SCT timeout for every RxSPDO.
*
* Safety Control Time (SCT) is defined for every RxSPDO in the SOD referenced by index 0x1400-0x17FE
* and sub-index 0x02 (object name : SCT_U32)
*
* @attention The smallest Safety Control Time in the SOD object with index 0x1400-0x17FE and sub-index
* 0x02 gives the call frequency of this function.
*
* @param        b_instNum        instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param        dw_ct            consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
*/
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  void SPDO_CheckRxTimeout(BYTE_B_INSTNUM_ UINT32 dw_ct);
#endif
  /**
  * @brief This function reads the status of the SPDO.
  *
  * @param       b_instNum           instance number, valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
  *
  * @param       w_rxSpdoIdx         Rx SPDO index, valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
  *
  * @param       dw_ct                actual consecutive time, any value allowed, valid range: UINT16
  *
  * @retval         pw_dCt              time between the last two valid RxSPDOs:
  *
  * @retval         pdw_ageSpdo         age of the current data
  *
  * @retval         pw_propDel          propagation delay of the last synchronization
  *
  * @return
  * - TRUE             - data of RxSPDO is valid
  * - FALSE            - data of RxSPDO is not valid
  */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
BOOLEAN SPDO_GetRxSpdoStatus(BYTE_B_INSTNUM_ UINT16 const w_rxSpdoIdx, UINT32 const dw_ct,
	UINT16 * const pw_dCt, UINT32 * const pdw_ageSpdo, UINT16 * const pw_propDel);
#endif
/**
 * This is a callback function ro rx & tx mapping parameter entries sub-indicex 0-253
*
* This function is a callback function for objects with index 0x1800-0x1BFE (rx mapping parameters)
* sub-index 0-253 and 0xC000-0xC3FE (tx mapping parameters) sub-index 0-253. If 0 is written on
* sub-index 0 (NumberOfEntries), then the SPDO mapping for the corresponding SPDO is deactivated
* otherwise the SPDO mapping for the corresponding SPDO is activated. If another sub-index is
* written (SPDOMapping_U32), then the mapped object is checked whether the mapped object is mappable or not.
*
* @attention Objects with index 0x1800-0x1BFE (rx mapping parameters) sub-index 0-253 and
* 0xC000-0xC3FE (tx mapping parameters) sub-index 0-253 have to be defined in the SOD with
* SOD_k_ATTR_BEF_WR attribute and callback function reference to this function.
*
* @param b_instNum instance number (not checked, checked in SOD_Read() or SOD_Write()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param e_srvc type of service, see SOD_t_SERVICE (not checked, only called with enum value in CallBeforeReadClbk() or CallBeforeWriteClbk() or SOD_CallAfterWriteClbk()) valid range: SOD_t_SERVICE
* @param ps_obj pointer to a SOD entry, see SOD_t_OBJECT (pointer not checked, only called with reference to struct in SOD_Read() or SOD_Write()) valid range: pointer to a SOD_t_OBJECT
* @param pv_data pointer to data to be written, in case of SOD_k_SRV_BEFORE_WRITE, otherwise NULL (pointer not checked, checked in SOD_Write()) valid range: pointer to data to be written, in case of  SOD_k_SRV_BEFORE_WRITE, otherwise NULL
* @param dw_offset start offset in bytes of the segment within the data block (not used)
* @param dw_size size in bytes of the segment (not used)
* @retval pe_abortCode abort code has to be set for the SSDO if the return value is FALSE. (pointer not checked, only called with reference to variable) valid range: pointer to the SOD_t_ABORT_CODES
*
* @return
* - TRUE               - success
* - FALSE              - failure
*
*/
BOOLEAN SPDO_SOD_TxMappPara_CLBK(BYTE_B_INSTNUM_ SOD_t_SERVICE e_srvc,
                                 const SOD_t_OBJECT *ps_obj,
                                 const void *pv_data,
                                 UINT32 dw_offset, UINT32 dw_size,
                                 SOD_t_ABORT_CODES *pe_abortCode);

/**
 * This is a callback function ro rx & tx mapping parameter entries sub-indicex 0-253
*
* This function is a callback function for objects with index 0x1800-0x1BFE (rx mapping parameters)
* sub-index 0-253 and 0xC000-0xC3FE (tx mapping parameters) sub-index 0-253. If 0 is written on
* sub-index 0 (NumberOfEntries), then the SPDO mapping for the corresponding SPDO is deactivated
* otherwise the SPDO mapping for the corresponding SPDO is activated. If another sub-index is
* written (SPDOMapping_U32), then the mapped object is checked whether the mapped object is mappable or not.
*
* @attention Objects with index 0x1800-0x1BFE (rx mapping parameters) sub-index 0-253 and
* 0xC000-0xC3FE (tx mapping parameters) sub-index 0-253 have to be defined in the SOD with
* SOD_k_ATTR_BEF_WR attribute and callback function reference to this function.
*
* @param b_instNum instance number (not checked, checked in SOD_Read() or SOD_Write()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param e_srvc type of service, see SOD_t_SERVICE (not checked, only called with enum value in CallBeforeReadClbk() or CallBeforeWriteClbk() or SOD_CallAfterWriteClbk()) valid range: SOD_t_SERVICE
* @param ps_obj pointer to a SOD entry, see SOD_t_OBJECT (pointer not checked, only called with reference to struct in SOD_Read() or SOD_Write()) valid range: pointer to a SOD_t_OBJECT
* @param pv_data pointer to data to be written, in case of SOD_k_SRV_BEFORE_WRITE, otherwise NULL (pointer not checked, checked in SOD_Write()) valid range: pointer to data to be written, in case of  SOD_k_SRV_BEFORE_WRITE, otherwise NULL
* @param dw_offset start offset in bytes of the segment within the data block (not used)
* @param dw_size size in bytes of the segment (not used)
* @retval pe_abortCode abort code has to be set for the SSDO if the return value is FALSE. (pointer not checked, only called with reference to variable) valid range: pointer to the SOD_t_ABORT_CODES
*
* @return
* - TRUE               - success
* - FALSE              - failure
*
*/
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  BOOLEAN SPDO_SOD_RxMappPara_CLBK(BYTE_B_INSTNUM_ SOD_t_SERVICE e_srvc,
                                   const SOD_t_OBJECT *ps_obj,
                                   const void *pv_data,
                                   UINT32 dw_offset, UINT32 dw_size,
                                   SOD_t_ABORT_CODES *pe_abortCode);
#endif


#endif

/** @} */
