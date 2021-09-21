/**
 * @addtogroup SOD
 * @{
 *
 * @file SODint.h
 *
 * This file is the internal header-file of the unit.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SODint.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
 */

#ifndef SODINT_H
#define SODINT_H


/**
 * This define is used to access the low limit of the object in the range array.
*/
#define k_RANGE_LOW_LIMIT  0

/**
 * This define is used to access the high limit of the object in the range array.
*/
#define k_RANGE_HIGH_LIMIT 1

/**
* @brief This function initializes the flags used to lock and disable the SOD.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*/
void SOD_InitFlags(BYTE_B_INSTNUM);

/**
* @brief This function initializes the addresses and number of entries.
*
* Start-, end- and actual address are set and number of SOD entries are calculated.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - TRUE            - success
* - FALSE           - failure
*/
BOOLEAN SOD_InitAddr(BYTE_B_INSTNUM);

/**
* @brief This function checks the handle of the SOD entry.
*
* @param        b_instNum            instance number (not checked, checked in SOD_ActualLenSet() or SOD_Write() or SOD_Read() or SOD_ActualLenGet()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_hdl               object handle (checked) valid range: UINT32
*
* @retval       ps_errRes
* - == NULL error is generate in the function
* - <> NULL error result, only relevant if the return value == FALSE, (pointer not checked, any value allowed), valid range : <> or == NULL
*
* @return
* - TRUE                 - SOD entry handle is valid
* - FALSE              - SOD entry handle is not valid, see ps_errRes
*/
BOOLEAN SOD_HdlValid(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl,
                     SOD_t_ERROR_RESULT *ps_errRes);

/**
* @brief This function gets a pointer to the first SOD object.
*
* @param      b_instNum      instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return     Pointer to the first SOD object.
*/
const SOD_t_OBJECT *SOD_GetSodStartAddr(BYTE_B_INSTNUM);

/**
* @brief This function checks the openSAFETY Object Dictionary (SOD), calculates and sets the length of the SOD.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - TRUE            - success
* - FALSE           - failure
*/
BOOLEAN SOD_Check(BYTE_B_INSTNUM);

/**
* @brief This function initializes the addresses and number of entries as well as the default values.
*
* Start-, end- and actual address are set and number of SOD entries are calculated.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
*/
void SOD_InitVirt(BYTE_B_INSTNUM);

/**
* @brief This function sets the actual values to the default values in the SOD.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - TRUE            - success
* - FALSE           - failure
*/
BOOLEAN SOD_SetDefaultValuesVirt(BYTE_B_INSTNUM);

/**
* @brief This function handles the write access of a read only object.
*
* If the object is read only and the object type is standard then the actual data and the written data
* will be compared. If the actual data and the written data are the same then no error is signaled,
* otherwise SOD_k_ERR_OBJ_NOT_WRITEABLE error is generated later.
*
* @param        pv_data            pointer to the data to be written (pointer not checked, checked in
*       SOD_Write()) valid range : <> NULL
*
* @param        ps_object          pointer to the module instance (pointer not checked, only called
*       with reference to struct in SOD_Write()) valid range : <> NULL
*
* @param        o_overwrite        if TRUE then a read only object can be overwritten by the application
*       (not checked, any value allowed) valid range : TRUE, FALSE
*
* @return
* - TRUE             - the actual data and the data to be written are the same
* - FALSE            - the actual data and the data to be written are not the same
*/
BOOLEAN SOD_ObjectReadOnly(const void *pv_data, const SOD_t_OBJECT *ps_object, BOOLEAN o_overwrite);

/**
* @brief This function checks whether the given object is writeable.
*
* @param        b_instNum          instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        ps_object          pointer to the module instance (pointer not checked,
*       only called with reference to struct in SOD_Write()) valid range : <> NULL
*
* @param        o_overwrite
* - TRUE - a read only object can be overwritten
* - FALSE - write access is denied (not checked, any value allowed) valid range : TRUE, FALSE
*
* @return
* - TRUE             - object is writeable
* - FALSE            - object is not writeable
*/
BOOLEAN SOD_ObjectWriteable(BYTE_B_INSTNUM_ const SOD_t_OBJECT *ps_object, BOOLEAN o_overwrite);

/**
* @brief This function checks whether the written data is in the range.
*
* If the data type is VISIBLE STRING or BOOLEAN then data will be checked. If the data type is
* DOMAIN or OCTET STRING then  data will not be checked. Otherwise the range checking will be
* accomplished if it is specified for the object.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll() or SOD_Write()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        pv_data           pointer to the data to be checked (pointer not checked, only called with
*       reference to variable in SetDefaultValues() or checked in SOD_Write()) valid range : <> NULL
*
* @param        ps_object         pointer to the SOD entry (pointer not checked, only called with reference to
*       struct in SetDefaultValues() or SOD_Write()) valid range : <> NULL
*
* @param        dw_offset         start offset in bytes of the segment within the data block (not checked, only
*       called with 0 or checked in SOD_Write()) valid range : UINT32
*
* @param        dw_size           size in bytes of the segment (not checked, only called with 0 or checked in
*       SOD_Write()) valid range : UINT32
*
* @return
* - TRUE            - data is valid
* - FALSE           - data is invalid
*/
BOOLEAN SOD_DataValid(BYTE_B_INSTNUM_ const void *pv_data,const SOD_t_OBJECT *ps_object, UINT32 dw_offset,UINT32 dw_size);

/**
* @brief This function calls the after write callback function.
*
* The callback function is called, if the SOD_k_ATTR_AFT_WR attribute is set and a reference to the callback function is defined.
*
* @param        b_instNum         instance number (not checked, checked in SOD_Write()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        ps_object         pointer to the SOD entry (pointer not checked, only called with reference to struct in SOD_Write()) valid range : <> NULL
*
* @param        dw_offset     start offset in bytes of the segment within the data block (not checked, checked in SOD_Write()) valid range : UINT32
*
* @param        dw_size           size in bytes of the segment (not checked, checked in SOD_Write()) valid range : UINT32
*
* @return
* - TRUE            - callback is completed successful
* - FALSE           - callback is not completed successful
*/
BOOLEAN SOD_CallAfterWriteClbk(BYTE_B_INSTNUM_ const SOD_t_OBJECT *ps_object, UINT32 dw_offset, UINT32 dw_size);

/**
* @brief This function copies the data segment into the OD.
*
* @param        pv_data           pointer to the data to be copied (pointer not checked, only called with reference to variable in SetDefaultValues() or checked in SOD_Write()) valid range : <> NULL
*
* @param        ps_object         pointer to the SOD entry (pointer not checked, only called with reference to struct in SetDefaultValues() or SOD_Write()) valid range : <> NULL
*
* @param        dw_offset         start offset in bytes of the segment within the data block (not checked, only called with 0 or checked in SOD_Write()) valid range : UINT32
*
* @param        dw_size     size in bytes of the segment (not checked, only called with 0 or checked in SOD_Write()) valid range : UINT32
*/
void SOD_DataCpy(const void *pv_data, const SOD_t_OBJECT *ps_object, UINT32 dw_offset, UINT32 dw_size);

#endif

/** @} */
