/**
 * @addtogroup SOD
 * @{
 *
 * @file SODapi.h
 *
 * This file is the application interface header-file of the unit.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SODapi.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
 */

#ifndef SODAPI_H
#define SODAPI_H

/**
 * This define must be used to sign the end of the SOD for the element w_index (see SOD_t_OBJECT)
 * @attention Object with index 0xFFFF must not be defined in the SOD.
 */
#define SOD_k_END_OF_THE_OD (UINT16)(0xFFFF)

/**
 * @name SOD Attributes
 * @{
 * @brief These symbolic constants represent the bits of the w_attr in SOD_t_ATTR.
 *
 * Each bit has its special meaning and must be set by the user depending on the application.
 *
 * @attention The following SOD-Attribute combinations are not allowed :
 * - SOD-Attributes must not be 0
 * - SOD_k_ATTR_BEF_RD, SOD_k_ATTR_BEF_WR or SOD_k_ATTR_AFT_WR without callback function
 * - SOD_k_ATTR_RO_CONST with SOD_k_ATTR_RO or/and SOD_k_ATTR_WO
 */

/** High byte */

/** service BEFORE_READ: the assigned callback function will be called before read access to the SOD */
#define SOD_k_ATTR_BEF_RD  ((UINT16)(0x0100))
/** service BEFORE_WRITE: the assigned callback function will be called before write access to the SOD */
#define SOD_k_ATTR_BEF_WR  ((UINT16)(0x0200))
/** service AFTER_WRITE: the assigned callback function will be called after write access to the SOD */
#define SOD_k_ATTR_AFT_WR  ((UINT16)(0x0400))
/** Pointer to Pointer: pv_objData in SOD_t_OBJECT is a pointer to pointer see SOD_t_OBJECT */
#define SOD_k_ATTR_P2P     ((UINT16)(0x0800))
/** No SPDO mapping length ckecking: the length checking at the SPDO mapping is decativated. */
#define SOD_k_ATTR_NO_LEN_CHK ((UINT16)(0x1000))
/** the whole entry is simulated by the callback function */
#define SOD_k_ATTR_SIMU    ((UINT16)(0x2000))
/** the entry is an array */
#define SOD_k_ATTR_ARRAY    ((UINT16)(0x4000))

/** Low byte */

/** object is mappable (into SPDO) */
#define SOD_k_ATTR_PDO_MAP  ((UINT16)(0x0001))
/** object data variable is constant and either the application or SSDO must not overwrite the object data*/
#define SOD_k_ATTR_RO_CONST ((UINT16)(0x0002))
/** object data variable is read only and can be overwritten by the application but not the SSDO */
#define SOD_k_ATTR_RO       ((UINT16)(0x0004))
/** object is write only */
#define SOD_k_ATTR_WO       ((UINT16)(0x0008))
/** object is relevant for CRC checking */
#define SOD_k_ATTR_CRC      ((UINT16)(0x0010))


/** object is read- and writeable */
#define SOD_k_ATTR_RW       (SOD_k_ATTR_RO | SOD_k_ATTR_WO)

#if (EPLS_cfg_MAX_INSTANCES > 1)
/**
 * @brief This attribute is used if an object is on several or all SOD instances available.
 *
 * A shared object must be defined on every instance where this object can be accessed and
 * the object definition for such an object in the SOD must be the same. For a write access
 * all instances must be in PRE-OPERATIONAL state.
 */
#define SOD_k_ATTR_SHARED ((UINT16)(0x0020))
#endif
/** @} */

/**
 * Enumeration of the abort codes.
 * For a detailed descriptions of the SOD abort codes, see the openSAFETY specification.
 */
typedef enum
{
    /** No error */
    SOD_ABT_NO_ERROR = 0x00000000,

    /* SOD_ABT_RESERVED1           = 0x05030000,    Reserved */

    /** SSDO protocol timed out */
    SOD_ABT_SSDO_TIME_OUT = 0x05040000,
    /** Client/server cmd ID not valid */
    SOD_ABT_CMD_ID_INVALID = 0x05040001,
    /** Invalid block size */
    SOD_ABT_BLOCK_SIZE_INVALID = 0x05040002,
    /** Invalid sequence number */
    SOD_ABT_SEQ_NUM_INVALID = 0x05040003,

    /* SOD_k_RESERVED2             = 0x05040004,    Reserved */

    /** Out of memory */
    SOD_ABT_OUT_OF_MEMORY = 0x05040005,
    /** Unsupported access to an object */
    SOD_ABT_UNSUPPORTED_ACCESS = 0x06010000,
    /** Attempt to read a write-only object */
    SOD_ABT_OBJ_NOT_READABLE = 0x06010001,
    /** Attempt to write a read-only object */
    SOD_ABT_OBJ_NOT_WRITEABLE = 0x06010002,
    /** Object does not exist */
    SOD_ABT_OBJ_DOES_NOT_EXIST = 0x06020000,
    /** Object is not mappable */
    SOD_ABT_OBJ_NOT_MAPPABLE = 0x06040041,
    /** The number and length of the objects to be mapped would exceed SPDO length. */
    SOD_ABT_SPDO_LENGTH_EXCEEDED = 0x06040042,
    /** General parameter incompatibility */
    SOD_ABT_GEN_PARAM_INCOMP = 0x06040043,
    /** General internal incompatibility in the device */
    SOD_ABT_GEN_INTERNAL_INCOMP = 0x06040047,
    /** Access failed due to a hardware error */
    SOD_ABT_HW_ERROR = 0x06060000,
    /** Data type does not match, length of service parameter does not match */
    SOD_ABT_LEN_DOES_NOT_MATCH = 0x06070010,
    /** Data type does not match, length of service parameter too high */
    SOD_ABT_LEN_IS_TOO_HIGH = 0x06070012,
    /** Data type does not match, length of service parameter too low */
    SOD_ABT_LEN_IS_TOO_LOW = 0x06070013,
    /** Sub-index does not exist */
    SOD_ABT_SUB_IDX_DOES_NOT_EXIST = 0x06090011,
    /** Value range of parameter exceeded (only for write access) */
    SOD_ABT_VALUE_RANGE_EXCEEDED = 0x06090030,
    /** Value of parameter written too high */
    SOD_ABT_VALUE_IS_TOO_HIGH = 0x06090031,
    /** Value of parameter written too low */
    SOD_ABT_VALUE_IS_TOO_LOW = 0x06090032,
    /** Maximum value is less than minimum value */
    SOD_ABT_MAX_MIN_VALUE_ERROR = 0x06090036,
    /** General error */
    SOD_ABT_GENERAL_ERROR = 0x08000000,
    /** Data cannot be transferred or stored to the application */
    SOD_ABT_DATA_CAN_NOT_STORED = 0x08000020,
    /** Data cannot be transferred or stored to the application because of local control */
    SOD_ABT_LOCAL_CONTROL = 0x08000021,
    /** Data cannot be transferred or stored to the application because of the present device state */
    SOD_ABT_PRESENT_DEVICE_STATE = 0x08000022,
    /** Data cannot be transferred or stored to the application because of the object data is not available now */
    SOD_ABT_OBJ_IS_BUSY = 0x08000023
} SOD_t_ABORT_CODES;

/**
 * @brief Structure definition for the SOD objects with actual length and pointer to the data.
 *
 * @attention This structure must be only used to initialize the pv_objData pointer
 * in the SOD_tag_OBJECT structure and only for SOD objects with data type
 * EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING or EPLS_k_OCTET_STRING.
 */
typedef struct
{
    /** actual length of the object */
    UINT32 dw_actLen;
    /** reference to the object data */
    void *pv_objData;
} SOD_t_ACT_LEN_PTR_DATA;

/**
 * @brief Attributes of a SOD entry.
 */
typedef struct
{
    /** attributes, see SOD-Attributes */
    UINT16 w_attr;
    /** data type of the object */
    EPLS_t_DATATYPE e_dataType;
    /** maximum length of the object */
    UINT32 dw_objLen;
    /** reference to default value */
    const void *pv_defValue;
} SOD_t_ATTR;

/**
 * @enum SOD_t_SERVICE
 * @brief Enumeration of services to be done/executed by the SOD callback function (see SOD_t_CLBK).
 */
typedef enum
{
  /**
   * The callback function is called with this service to get the attributes of an
   * object
   */
  SOD_k_SRV_READ_ATTR,
    /**
     * The callback function is called with this service before a read access to an
     * object is processed e.g. to update object data
     */
    SOD_k_SRV_BEFORE_READ,
    /**
     * The callback function is called with this service before a write access to an
     * object is processed e.g. to check the range
     */
    SOD_k_SRV_BEFORE_WRITE,
    /**
     * The callback function is called with this service after an object is written
     * e.g. to signal new data
     */
    SOD_k_SRV_AFTER_WRITE
} SOD_t_SERVICE;

/**
 * @brief This structured data type represents an entry in the safety object
 * dictionary (SOD) of the openSAFETY Stack.
 *
 * @attention Objects in the SOD must be sorted by index in ascending order and Objects
 * with the same index must be sorted by sub-index, otherwise the search algorithm does not work.
 *
 * @attention pv_range is a pointer to an array with two elements. The data type of the elements
 * in the array must be the same as the object data type. The first element is the low limit of
 * the data range and the second element is the high limit of the data range. If this pointer is
 * NULL then the range checking will be accomplished only for the objects with EPLS_k_BOOLEAN data
 * type and VISIBLE_STRING data type. This pointer must be NULL for the objects with EPLS_k_BOOLEAN,
 * EPLS_k_VISIBLE_STRING, EPLS_k_OCTET_STRING, EPLS_k_DOMAIN. If this pointer is not NULL, then
 * the written data must be inside of the given range.
 */
typedef struct SOD_tag_OBJECT
{
    /** index of an object */
    UINT16 w_index;
    /** sub-index of an object */
    UINT8 b_subIndex;
    /** see SOD_t_ATTR */
    SOD_t_ATTR s_attr;
    /** reference to object data, (see SOD_t_ACT_LEN_PTR_DATA) */
    void *pv_objData;
    /** reference to min-/max values */
    const void *pv_range;

    /**
     * Definition of a function pointer used for the SOD callback function.
     * @relates SOD_t_CLBK
     * @see SOD_t_CLBK
     *
     *
     * The callback function is used to signal read or write accesses (see SOD_t_SERVICE). Valid
     * range for the parameters of the callback function is commented by the implementation
     * of the callback function, because it is depending on the implementation.
     *
     * @param b_instNum           instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
     * @param e_srvc              type of service, see SOD_t_SERVICE (not checked) valid range: SOD_t_SERVICE
     * @param ps_obj              pointer to a SOD entry
     * @param pv_data             pointer to data to be written, in case of SOD_k_SRV_BEFORE_WRITE, otherwise NULL
     * @param dw_offset           start offset in bytes of the segment
     * @param dw_size             size in bytes of the segment
     * @param pe_abortCode        abort code
     *
     * @return
     * - TRUE if the execution was successful
     * - FALSE otherwise
     *
     */
    BOOLEAN (*pf_clbk)(
            BYTE_B_INSTNUM_
            SOD_t_SERVICE e_srvc,
            const struct SOD_tag_OBJECT *ps_obj,
            const void *pv_data,
            UINT32 dw_offset,
            UINT32 dw_size,
            SOD_t_ABORT_CODES *pe_abortCode);
} SOD_t_OBJECT;
/**
 * @brief This structured data type represents an entry in the virtual safety object
 * dictionary (SOD) of the openSAFETY Stack.
 *
 * @attention Objects in the virtual SOD must be sorted by index in ascending order and Objects
 * with the same index must be sorted by sub-index, otherwise the search algorithm does not work.
 * The difference between the virtual SOD and the normal SOD is that one entry may represent
 * more than one object (object type array) or uses a simulated function to represent
 * a great number of various objects (range in index and sub-index).
 *
 * @attention pv_range is a pointer to an array with two elements. The data type of the elements
 * in the array must be the same as the object data type. The first element is the low limit of
 * the data range and the second element is the high limit of the data range. If this pointer is
 * NULL then the range checking will be accomplished only for the objects with EPLS_k_BOOLEAN data
 * type and EPLS_k_VISIBLE_STRING data type, both types will be checked implicitly. This pointer
 * must be NULL for the objects with EPLS_k_BOOLEAN, EPLS_k_VISIBLE_STRING, EPLS_k_OCTET_STRING,
 * EPLS_k_DOMAIN. If this pointer is not NULL, then the written data must be inside of the
 * given range.
 */
typedef struct
{
  /** see SOD_t_ATTR */
  SOD_t_ATTR s_attr;
  /** start index of the virtual SOD entry */
  UINT16 w_indexStart;
  /** end index of the virtual SOD entry */
  UINT16 w_indexEnd;
  /** start sub-index of the virtual SOD entry */
  UINT8 b_subIndexStart;
  /** end sub-index of the virtual SOD entry */
  UINT8 b_subIndexEnd;
  /** reference to object data, (see SOD_t_ACT_LEN_PTR_DATA) */
  void *pv_objData;
  /** reference to min-/max values */
  const void *pv_range;
  /**
   * Definition of a function pointer used for the SOD callback function.
   * @relates SOD_t_CLBK
   * @see SOD_t_CLBK
   *
   *
   * The callback function is used to signal read or write accesses (see SOD_t_SERVICE). Valid
   * range for the parameters of the callback function is commented by the implementation
   * of the callback function, because it is depending on the implementation.
   *
   * @param b_instNum           instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
   * @param e_srvc              type of service, see SOD_t_SERVICE (not checked) valid range: SOD_t_SERVICE
   * @param ps_obj              pointer to a SOD entry
   * @param pv_data             pointer to data to be written, in case of SOD_k_SRV_BEFORE_WRITE, otherwise NULL
   * @param dw_offset           start offset in bytes of the segment
   * @param dw_size             size in bytes of the segment
   * @param pe_abortCode        abort code
   *
   * @return
   * - TRUE if the execution was successful
   * - FALSE otherwise
   *
   */
  BOOLEAN (*pf_clbk)(
          BYTE_B_INSTNUM_
          SOD_t_SERVICE e_srvc,
          const struct SOD_tag_OBJECT *ps_obj,
          const void *pv_data,
          UINT32 dw_offset,
          UINT32 dw_size,
          SOD_t_ABORT_CODES *pe_abortCode);
}SOD_t_ENTRY_VIRT;

typedef struct
{
  /** structure for the object being accessed */
  SOD_t_OBJECT s_obj;
  /** segment offset for segmented access */
  UINT32 dw_segOfs;
  /** segment size for segmented access */
  UINT32 dw_segSize;
  /** handle for non virtual SOD entries */
  PTR_TYPE dw_hdl;
  /** object is a non virtual application object */
  BOOLEAN o_applObj;
}SOD_t_ACS_OBJECT_VIRT;


/**
 * @brief Type definition for the SOD callback function.
 *
 * The callback function is used to signal read or write accesses (see SOD_t_SERVICE). Valid
 * range for the parameters of the callback function is commented by the implementation
 * of the callback function, because it is depending on the implementation.
 *
 * @param b_instNum           instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 * @param e_srvc              type of service, see SOD_t_SERVICE (not checked) valid range: SOD_t_SERVICE
 * @param ps_obj              pointer to a SOD entry
 * @param pv_data             pointer to data to be written, in case of SOD_k_SRV_BEFORE_WRITE, otherwise NULL
 * @param dw_offset           start offset in bytes of the segment
 * @param dw_size             size in bytes of the segment
 * @param pe_abortCode        abort code
 *
 * @return
 * - TRUE if the execution was successful
 * - FALSE otherwise
 */
typedef BOOLEAN (*SOD_t_CLBK)(BYTE_B_INSTNUM_ SOD_t_SERVICE e_srvc,
        const struct SOD_tag_OBJECT *ps_obj,
        const void *pv_data,
        UINT32 dw_offset,
        UINT32 dw_size,
        SOD_t_ABORT_CODES *pe_abortCode);

/**
 * @def SOD_k_NO_CALLBACK
 * @brief This define is used if no SOD callback function is available for the SOD object.
 */
#define SOD_k_NO_CALLBACK ((SOD_t_CLBK)(0))

/**
 * @struct SOD_t_ERROR_RESULT
 * @brief Structure for the error result of the SOD_AttrGet(), SOD_AttrGetNext() and SOD_Read() functions.
 */
typedef struct
{
    /** error code to generate an error */
    UINT16 w_errorCode;
    /** abort code to send an abort response */
    SOD_t_ABORT_CODES e_abortCode;
} SOD_t_ERROR_RESULT;

/**
 * @def SOD_k_ERR_NO
 * If no error happened in the SOD_AttrGet(), SOD_AttrGetNext() and
 * SOD_Read() functions, then the w_errorCode in SOD_t_ERROR_RESULT is set
 * to the value of this define and the e_abortCode in SOD_t_ERROR_RESULT is
 * set to SOD_ABT_NO_ERROR.
 */
#define SOD_k_ERR_NO  0xFFFFU

/**
 * @brief This function returns the attributes and the access handle of the via index and sub-index specified object.
 *
 * The handle and application object flag are necessary for all read/write accesses.
 *
 * @attention The returned data is READ-ONLY.
 *
 * @note This function is reentrant, therefore the Control Flow Monitoring is not active and SERR_SetError() is not called.
 * To make this function save, the return value ps_ret is initialized with NULL at the beginning of this function.
 * Additionally the error result is returned via ps_ret and ps_errRes.

 * @see          SOD_AttrGetNext()
 *
 * @param        b_instNum         instance number (checked), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_idx             index of object attributes to be accessed (checked), valid range: (UINT16)
 *
 * @param        b_subIdx          sub-index of object attributes to be accessed (checked), valid range: (UINT8)
 *
 * @retval          pdw_hdl           handle to the specified SOD entry (pointer checked), valid range : <> NULL
 *
 * @retval          po_appObj         application object flag
 * - TRUE  : application object
 * - FALSE : SOD object if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE) (pointer checked) if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE) (not used)
 *
 * @retval          ps_errRes         error result, only relevant if the return value == NULL, (checked), valid range : <> NULL
 *
 * @return
 * - <> NULL         - pointer to the attributes of the SOD entry
 * - == NULL         - SOD entry not available or failure. For error  result see ps_errRes
 *
 */
const SOD_t_ATTR *SOD_AttrGet(BYTE_B_INSTNUM_ UINT16 w_idx, UINT8 b_subIdx,
		PTR_TYPE *pdw_hdl, BOOLEAN *po_appObj,
        SOD_t_ERROR_RESULT *ps_errRes);

/**
 * @brief This function returns the attributes and the access handle of the via 'o_firstObj' specified SOD object.
 *
 * @attention The returned data is READ-ONLY. Application objects are not accessed.
 *
 * @note This function is NOT called by the openSAFETY stack and is NOT reentrant. The Control Flow Monitoring is not active
 * and SERR_SetError() is not called. To make this function save, the return value ps_ret is initialized with NULL at the
 * beginning of this function. Additionally the error result is returned via ps_ret and ps_errRes.

 * @see          SOD_AttrGet()
 *
 * @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 * @param        o_firstObj
 * - TRUE : the return value points to the attribute of the first object.
 * - FALSE : the return value points to the attribute of the next object. (not checked, any value allowed) valid range: BOOLEAN
 * @retval          pdw_hdl            handle to the specified SOD entry (pointer checked) valid range: <> NULL
 * @retval          ps_errRes          error result, only relevant if the return value == NULL, (checked), valid range : <> NULL
 *
 * @return
 * - <> NULL          - pointer to the attributes of the SOD entry
 * - == NULL          - end of SOD reached ps_errRes =  SOD_k_ERR_NO, SOD_ABT_NO_ERROR or  failure. For error result see ps_errRes
 */
const SOD_t_ATTR *SOD_AttrGetNext(BYTE_B_INSTNUM_ BOOLEAN o_firstObj,
		PTR_TYPE *pdw_hdl,
        SOD_t_ERROR_RESULT *ps_errRes);

/**
 * @brief This function has to be used for reading the SOD entries in the openSAFETY object dictionary.
 *
 * It returns a pointer to the data (segment) of the SOD entry. The segment position and size have to be specified via
 * dw_offset and dw_size in case of segmented access. dw_offset and dw_size are used only for  domains, visible strings
 * and octet strings (e.g. domain is read from flash), otherwise they have to be set to 0.
 *
 * @attention To get the actual length of the object, SOD_ActualLenGet() must be called.
 *
 * @note This function is reentrant, therefore the Control Flow Monitoring is not active and SERR_SetError() is not called.
 * To make this function save, the return value pv_ret is initialized with NULL at the beginning of this function. Additionally
 * the error result is returned via pv_ret and ps_errRes.
 *
 * @see SOD_AttrGet()
 * @see SOD_AttrGetNext()
 * @see SOD_ActualLenSet()
 *
 * @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_hdl             handle/reference to the SOD entry  got by SOD_AttrGetNext() or SOD_AttrGet() (checked) valid range: (UINT32)
 *
 * @param        o_appObj           application object flag got by SOD_AttrGet() and FALSE after SOD_AttrGetNext() call if (SOD_cfg_APPLICATION_OBJ ==  EPLS_k_ENABLE) (checked) if (SOD_cfg_APPLICATION_OBJ ==  EPLS_k_DISABLE) (not used) (not checked, any value allowed) valid range: TRUE, FALSE
 *
 * @param        dw_offset          start offset in bytes of the segment within the data block (checked) valid range: (UINT32)
 *
 * @param        dw_size            size in bytes of the segment (checked) valid range: (UINT32)
 *
 * @retval          ps_errRes          error result, only relevant if the return value == NULL, (checked), valid range : <> NULL
 *
 * @return
 * - <> NULL          - pointer to the data segment in the SOD
 * - == NULL          - failure, for error result see ps_errRes
 *
 */
void *SOD_Read(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl, BOOLEAN o_appObj,
        UINT32 dw_offset, UINT32 dw_size,
        SOD_t_ERROR_RESULT *ps_errRes);

/**
 * @brief This function writes the specified data (segment) into the SOD.
 *
 * The segment position and size have to be specified via  dw_offset and dw_size in case of segmented access.
 * dw_offset and dw_size are used only for domains, visible strings and octet strings (e.g. domain is written into flash),
 * otherwise they have to be set to 0. In case of segmented access, the "after write callback" is only called,
 * if the (dw_offset+dw_size) equal the the actual length of the object. Therefore the actual length may be set before
 * the last segment is written.
 *
 * @attention To set the actual length of the object with data type EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING and
 * EPLS_k_OCTET_STRING, SOD_ActualLenSet() must be called.
 *
 * @see SOD_AttrGet()
 * @see SOD_AttrGetNext()
 * @see SOD_ActualLenSet()
 *
 * @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_hdl             handle/reference to the SOD entry  got by SOD_AttrGetNext() or SOD_AttrGet() (checked) valid range: (UINT32)
 *
 * @param        o_appObj           application object flag got by SOD_AttrGet() and FALSE after SOD_AttrGetNext() call if (SOD_cfg_APPLICATION_OBJ ==  EPLS_k_ENABLE) (checked) if (SOD_cfg_APPLICATION_OBJ ==  EPLS_k_DISABLE) (not used) (not checked, any value allowed) valid range: TRUE, FALSE
 *
 * @param        pv_data            reference to the data to be written (pointer checked) valid range: <> NULL
 *
 * @param        o_overwrite        TRUE - a read only object can be overwritten by the application FALSE - a read only object can not be overwritten by the application (checked) valid range: TRUE, FALSE
 *
 * @param        dw_offset          start offset in bytes of the segment within the data block (checked) valid range: (UINT32)
 *
 * @param        dw_size            size in bytes of the segment (checked) valid range: (UINT32)
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SOD_Write(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl, BOOLEAN o_appObj,
        const void *pv_data, BOOLEAN o_overwrite, UINT32 dw_offset,
        UINT32 dw_size);

/**
 * @brief This function locks the access to the segmented objects.
 *
 * An object access is segmented, if SOD_Read() or SOD_Write() is called and the dw_size
 * or/and dw_offset parameter are not 0.
 *
 * @attention This function must be called before SOD_Read() or SOD_Write() in case
 * segmented access. SOD_Unlock() must be called, after SOD_Read() or SOD_Write() if
 * SOD_Lock() was called.
 *
 * @note dw_hdl for the objects in the object dictionary is not used, rather all segmented
 * objects will be locked by  this function. In future it is possible to lock individually
 * segmented object referenced via dw_hdl. For the objects managed by the application, the
 * application lock function (SAPL_SOD_LockClbk()) will be called with the given dw_hdl.
 *
 * @todo Lock individually segmented object referenced via dw_hdl
 *
 * @see          SOD_Unlock()
 *
 * @param        b_instNum         instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_hdl            handle/reference to the SOD entry  got by SOD_AttrGetNext() or
 *      SOD_AttrGet() (only used for application objects, see Remarks) (checked) valid range: (UINT32)
 *
 * @param        o_appObj          application object flag got by SOD_AttrGetNext() or SOD_AttrGet()
 *      if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE) (checked) if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE)
 *      (not used) valid range: TRUE, FALSE
 *
 * @return
 * - TRUE            - locking succeeded
 * - FALSE           - locking failed (in case of SOD_k_ERR_LOCKING_FAILED the SOD was locked by another
 *      process and the segmented entries can not be accessed now)
 */
BOOLEAN SOD_Lock(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl, BOOLEAN o_appObj);

/**
 * @brief This function unlocks the access to the segmented objects.
 *
 * An object access is segmented, if SOD_Read() or SOD_Write() is called and the dw_size or/and dw_offset
 * parameter are not 0.
 *
 * @note dw_hdl is not used, rather all segmented objects will be unlocked by this function. In future
 * it is possible to unlock individually segmented object referenced via dw_hdl. For the objects managed
 * by the application, the application unlock function (SAPL_SOD_UnlockClbk) will be called with the given dw_hdl.
 *
 * @todo Unlock individually segmented object referenced via dw_hdl
 *
 * @see          SOD_Lock()
 *
 * @param        b_instNum         instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_hdl            handle/reference to the SOD entry  got by SOD_AttrGetNext() or
 *      SOD_AttrGet() (only used for application objects,  see Remarks) (checked) valid range: (UINT32)
 *
 * @param        o_appObj          application object flag got by SOD_AttrGetNext() or SOD_AttrGet()
 *      if (SOD_cfg_APPLICATION_OBJ ==  EPLS_k_ENABLE) (checked) if (SOD_cfg_APPLICATION_OBJ ==  EPLS_k_DISABLE)
 *      (not used) valid range: TRUE, FALSE
 *
 * @return
 * - TRUE            - unlocking succeeded
 * - FALSE           - unlocking failed
 */
BOOLEAN SOD_Unlock(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl, BOOLEAN o_appObj);

/**
 * @brief This function enables the SOD write access.
 *
 * @note e.g.: After the generation of the CRC or the parameter storage.
 *
 * @see          SOD_DisableSodWrite()
 *
 * @param        b_instNum         instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return
 * - TRUE            - success
 * - FALSE           - failure
 */
BOOLEAN SOD_EnableSodWrite(BYTE_B_INSTNUM);

/**
 * @brief This function disables the SOD write access.
 *
 * @note e.g. Before the generation of the CRC or the parameter storage.
 *
 * @see SOD_EnableSodWrite()
 *
 * @param        b_instNum         instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return
 * - TRUE            - success
 * - FALSE           - failure (in case of SOD_k_ERR_ENABLE_WR_FAILED the SOD write is already disabled by another process)
 */
BOOLEAN SOD_DisableSodWrite(BYTE_B_INSTNUM);

/**
 * @brief This function sets the default values for the application objects.
 *
 * @attention The function must be provided by the application, if SOD_cfg_APPLICATION_OBJ are enabled.
 *
 * @param        b_instNum          instance number
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure, error is generated by the SOD unit
 */
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
BOOLEAN SAPL_SOD_DefaultValueSetClbk(BYTE_B_INSTNUM);
#endif

/**
 * @brief This function returns the attributes and the access handle of the via index/sub-index specified application object.
 *
 * @attention This is the prototype of a callback function, which will be called by the SOD module if the index
 * in the SOD is not available. The function must be provided by the application, if SOD_cfg_APPLICATION_OBJ are enabled.
 *
 * @param        b_instNum            instance number
 *
 * @param        w_idx                index of object to be read
 *
 * @param        b_subIdx             sub-index of object to be read
 *
 * @retval          pdw_hdl             handle to specified SOD entry.
 *
 * @retval          pe_saplError        error code for the application object access that has to be set by the application
 *
 * @return
 * - <> NULL           - pointer to the attributes of the SOD entry
 * - == NULL           - SOD entry not available or failure, error is generated by the SOD unit
 */
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
const SOD_t_ATTR *SAPL_SOD_AttrGetClbk(BYTE_B_INSTNUM_ UINT16 w_idx,
        UINT8 b_subIdx, PTR_TYPE *pdw_hdl,
        SOD_t_ABORT_CODES *pe_saplError);
#endif

/**
 * @brief This function is called for reading the objects managed by the application.
 *
 * It returns a pointer to the data (segment) of the application object. The segment position and
 * size have to be specified via dw_offset and dw_size in case of segmented access.
 *
 * @attention This is the prototype of a callback function, which will be called by the SOD module. The
 * function must be provided by the application, if SOD_cfg_APPLICATION_OBJ are enabled.
 *
 * @param       b_instNum            instance number
 *
 * @param       dw_hdl               handle/reference to the SOD entry
 *
 * @param       dw_offset               start offset in bytes of the segment within the data block
 *
 * @param       dw_size              size in bytes of the segment
 *
 * @retval         pe_saplError         error code for the application object access that has to be set by the application
 *
 * @return
 * - <> NULL            - pointer to the data segment in the SOD
 * - == NULL            - failure, error is generated by the SOD unit
 */
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
void *SAPL_SOD_ReadClbk(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl, UINT32 dw_offset,
        UINT32 dw_size, SOD_t_ABORT_CODES *pe_saplError);
#endif

/**
 * @brief This function is called for writing the objects managed by the application.
 *
 * It writes the specified data (segment) into the SOD. The segment position and size have to be specified via
 * dw_offset and dw_size in case of segmented access.
 *
 * @attention This is the prototype of a callback function, which will be called by the SOD module.
 * The function must be provided by the application, if SOD_cfg_APPLICATION_OBJ are enabled.
 *
 * @param        b_instNum          instance number
 *
 * @param        dw_hdl             handle/reference to the SOD entry
 *
 * @param        pv_data            reference to the data to be written
 *
 * @param        o_overwrite        if TRUE then a read only object can be overwritten by the application
 *
 * @param        dw_offset          start offset in bytes of the segment within the data block
 *
 * @param        dw_size            size in bytes of the segment
 *
 * @return       Abort code       - See SOD_t_ABORT_CODES, error is generated by the SOD unit
 */
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
SOD_t_ABORT_CODES SAPL_SOD_WriteClbk(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl,
        const void *pv_data,
        BOOLEAN o_overwrite, UINT32 dw_offset,
        UINT32 dw_size);
#endif

/**
 * @brief This function locks the access to the application objects.
 *
 * @attention This function will be called from SOD_Lock() function. The function must be provided by
 * the application, if SOD_cfg_APPLICATION_OBJ are enabled.
 *
 * @see          SAPL_SOD_UnlockClbk()
 *
 * @param        b_instNum         instance number
 *
 * @param        dw_hdl            handle/reference to the application object got by SOD_AttrGetNext() or SOD_AttrGet()
 *
 * @return
 * - TRUE            - locking succeeded
 * - FALSE           - locking failed, error is generated by the SOD unit
 */
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
BOOLEAN SAPL_SOD_LockClbk(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl);
#endif

/**
 * @brief This function unlocks the access to the application objects.
 *
 * @attention This function will be called from SOD_Unlock() function. The function must be provided
 * by the application, if SOD_cfg_APPLICATION_OBJ are enabled.
 *
 * @see          SAPL_SOD_LockClbk()
 *
 * @param         b_instNum         instance number
 *
 * @param         dw_hdl            handle/reference to the application object got by SOD_AttrGetNext() or SOD_AttrGet()
 *
 * @return
 * - TRUE            - unlocking succeeded
 * - FALSE           - unlocking failed, error is generated by the SOD unit
 */
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
BOOLEAN SAPL_SOD_UnlockClbk(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl);
#endif

/**
 * @brief This function sets the actual length of the given SOD object.
 *
 * Actual length is only available at EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING and EPLS_k_OCTET_STRING SOD objects,
 * otherwise FATAL error must be generated.
 *
 * @attention The function must be provided by the application, if SOD_cfg_APPLICATION_OBJ are enabled.
 *
 * @see SAPL_SOD_ActualLenGet()
 *
 * @param        b_instNum        instance number
 *
 * @param        dw_hdl           handle/reference to the SOD entry  got by SOD_AttrGetNext() or SOD_AttrGet()
 *
 * @param        dw_actLen        actual length to be set
 *
 * @return       Abort code     - See SOD_t_ABORT_CODES, error is generated by the SOD unit
 */
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
SOD_t_ABORT_CODES SAPL_SOD_ActualLenSet(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl,
        UINT32 dw_actLen);
#endif

/**
 * @brief This function gets the actual length of the given SOD object.
 *
 * Actual length is only available at EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING and EPLS_k_OCTET_STRING SOD objects,
 * otherwise the maximum length object is returned.
 *
 * @attention The function must be provided by the application, if SOD_cfg_APPLICATION_OBJ are enabled.
 *
 * @see SAPL_SOD_ActualLenSet()
 *
 * @param        b_instNum          instance number
 *
 * @param        dw_hdl             handle/reference to the SOD entry  got by
 *                                  SOD_AttrGetNext() or SOD_AttrGet()
 * @retval          pdw_objLen         reference to the object length, if the data type is EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING or EPLS_k_OCTET_STRING then actual length, otherwise maximum length.
 *
 * @return       Abort code       - See SOD_t_ABORT_CODES, error is generated by the SOD unit
 */
#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
SOD_t_ABORT_CODES SAPL_SOD_ActualLenGet(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl,
        UINT32 *pdw_objLen);
#endif

/**
 * @brief This function sets the actual length of the given SOD object.
 *
 * Actual length is only available at EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING and EPLS_k_OCTET_STRING SOD objects,
 * otherwise FATAL error is generated.
 *
 * @see          SOD_ActualLenGet(), SOD_Read(), SOD_Write()
 *
 * @param        b_instNum           instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_hdl             handle/reference to the SOD entry  got by SOD_AttrGetNext() or SOD_AttrGet() (checked), valid range: (UINT32)
 *
 * @param        o_appObj           application object flag got by SOD_AttrGet() and FALSE after SOD_AttrGetNext() call if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE) (checked) if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE) (not used) valid range: TRUE, FALSE
 *
 * @param        dw_actLen          actual length to be set (checked), valid range: < maximum length of the given SOD object
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SOD_ActualLenSet(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl, BOOLEAN o_appObj,
        UINT32 dw_actLen);

/**
 * @brief This function gets the actual length of the given SOD object.
 *
 * Actual length is only available at EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING and EPLS_k_OCTET_STRING SOD objects,
 * otherwise the maximum length object is returned.
 *
 * @see          SOD_ActualLenSet()
 *
 * @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        dw_hdl             handle/reference to the SOD entry  got by SOD_AttrGetNext() or SOD_AttrGet() (checked), valid range: (UINT32)
 *
 * @param        o_appObj           application object flag got by SOD_AttrGet() and FALSE after SOD_AttrGetNext() call if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE) (checked) if (SOD_cfg_APPLICATION_OBJ ==  EPLS_k_DISABLE) (not used) valid range: TRUE, FALSE
 *
 * @retval          pdw_objLen         reference to the object length, if the data type is EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING or EPLS_k_OCTET_STRING then actual length, otherwise maximum length. (pointer checked), valid range: <> NULL
 *
 * @return
 * -TRUE             - success
 * -FALSE            - failure
 */
BOOLEAN SOD_ActualLenGet(BYTE_B_INSTNUM_ PTR_TYPE dw_hdl, BOOLEAN o_appObj,
        UINT32 *pdw_objLen);


/**
 * @brief This function returns the attributes and the access object of the via index and sub-index specified object.
 *
 * The access object is necessary for all read/write accesses.
 *
 * @attention The returned data is READ-ONLY.
 *
 * @note This function is reentrant, therefore the Control Flow Monitoring is not active and SERR_SetError() is not called.
 * To make this function save, the return value ps_ret is initialized with NULL at the beginning of this function.
 * Additionally the error result is returned via ps_ret and ps_errRes.

 * @see          SOD_AttrGetNextVirt()
 *
 * @param        ps_acs            pointer to the access structure (pointer checked) for the object
 *
 * @retval       ps_errRes         error result, only relevant if the return value == NULL, (checked), valid range : <> NULL
 *
 * @return
 * - <> NULL         - pointer to the attributes of the SOD entry
 * - == NULL         - SOD entry not available or failure. For error  result see ps_errRes
 *
 */
SOD_t_ATTR const * const SOD_AttrGetVirt(BYTE_B_INSTNUM_ SOD_t_ACS_OBJECT_VIRT * const ps_acs,
    SOD_t_ERROR_RESULT * const ps_errRes);
/**
 * @brief This function returns the attributes and the access object of the via 'o_firstObj' specified SOD object.
 *
 * @attention The returned data is READ-ONLY. Application objects are not accessed.
 *
 * @note This function is NOT called by the openSAFETY stack and is NOT reentrant. The Control Flow Monitoring is not active
 * and SERR_SetError() is not called. To make this function save, the return value ps_ret is initialized with NULL at the
 * beginning of this function. Additionally the error result is returned via ps_ret and ps_errRes.

 * @see          SOD_AttrGetVirt()
 *
 * @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 * @param        o_firstObj
 * - TRUE : the return value points to the attribute of the first object.
 * - FALSE : the return value points to the attribute of the next object. (not checked, any value allowed) valid range: BOOLEAN
 * @param        ps_acs            pointer to the access structure (pointer checked) for the object
 *
 * @retval       ps_errRes          error result, only relevant if the return value == NULL, (checked), valid range : <> NULL
 *
 * @return
 * - <> NULL          - pointer to the attributes of the SOD entry
 * - == NULL          - end of SOD reached ps_errRes =  SOD_k_ERR_NO, SOD_ABT_NO_ERROR or  failure. For error result see ps_errRes
 */
SOD_t_ATTR const * const SOD_AttrGetNextVirt(BYTE_B_INSTNUM_ BOOLEAN o_firstObj,
                                  SOD_t_ACS_OBJECT_VIRT * const ps_acs,
                                  SOD_t_ERROR_RESULT *ps_errRes);
/**
 * @brief This function has to be used for reading the SOD entries in the openSAFETY object dictionary.
 *
 * It returns a pointer to the data (segment) of the SOD entry. The segment position and size have to be specified via
 * dw_offset and dw_size in case of segmented access. dw_offset and dw_size are used only for  domains, visible strings
 * and octet strings (e.g. domain is read from flash), otherwise they have to be set to 0.
 *
 * @attention To get the actual length of the object, SOD_ActualLenGet() must be called.
 *
 * @note This function is reentrant, therefore the Control Flow Monitoring is not active and SERR_SetError() is not called.
 * To make this function save, the return value pv_ret is initialized with NULL at the beginning of this function. Additionally
 * the error result is returned via pv_ret and ps_errRes.
 *
 * @see SOD_AttrGetVirt()
 * @see SOD_AttrGetNextVirt()
 * @see SOD_ActualLenSetVirt()
 *
 * @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        ps_acs            pointer to the access structure (pointer checked) for the object
 *
 * @retval          ps_errRes          error result, only relevant if the return value == NULL, (checked), valid range : <> NULL
 *
 * @return
 * - <> NULL          - pointer to the data segment in the SOD
 * - == NULL          - failure, for error result see ps_errRes
 *
 */
void *SOD_ReadVirt(BYTE_B_INSTNUM_ SOD_t_ACS_OBJECT_VIRT * const ps_acs,
        SOD_t_ERROR_RESULT * const ps_errRes);
/**
 * @brief This function writes the specified data (segment) into the SOD.
 *
 * The segment position and size have to be specified via  dw_offset and dw_size in case of segmented access.
 * dw_offset and dw_size are used only for domains, visible strings and octet strings (e.g. domain is written into flash),
 * otherwise they have to be set to 0. In case of segmented access, the "after write callback" is only called,
 * if the (dw_offset+dw_size) equal the the actual length of the object. Therefore the actual length may be set before
 * the last segment is written.
 *
 * @attention To set the actual length of the object with data type EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING and
 * EPLS_k_OCTET_STRING, SOD_ActualLenSet() must be called.
 *
 * @see SOD_AttrGet()
 * @see SOD_AttrGetNext()
 * @see SOD_ActualLenSet()
 *
 * @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        ps_acs            pointer to the access structure (pointer checked) for the object
 *
 * @param        pv_data            reference to the data to be written (pointer checked) valid range: <> NULL
 *
 * @param        o_overwrite        TRUE - a read only object can be overwritten by the application FALSE - a read only object can not be overwritten by the application (checked) valid range: TRUE, FALSE
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SOD_WriteVirt(BYTE_B_INSTNUM_ SOD_t_ACS_OBJECT_VIRT const * const ps_acs,
        void const * const pv_data, BOOLEAN const o_overwrite);
/**
 * @brief This function sets the actual length of the given SOD object.
 *
 * Actual length is only available at EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING and EPLS_k_OCTET_STRING SOD objects,
 * otherwise FATAL error is generated.
 *
 * @see          SOD_ActualLenGetVirt(), SOD_ReadVirt(), SOD_WriteVirt()
 *
 * @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        ps_acs             pointer to the access structure  (pointer checked) for the object
 *
 * @param        dw_actLen          actual length to be set (checked), valid range: < maximum length of the given SOD object
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure
 */
BOOLEAN SOD_ActualLenSetVirt(BYTE_B_INSTNUM_ SOD_t_ACS_OBJECT_VIRT const * const ps_acs,
                         UINT32 const dw_actLen);
/**
 * @brief This function gets the actual length of the given SOD object.
 *
 * Actual length is only available at EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING and EPLS_k_OCTET_STRING SOD objects,
 * otherwise the maximum length object is returned.
 *
 * @see          SOD_ActualLenSetVirt()
 *
 * @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        ps_acs             pointer to the access structure  (pointer checked) for the object
 *
 * @retval       pdw_objLen         reference to the object length, if the data type is EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING or EPLS_k_OCTET_STRING then actual length, otherwise maximum length. (pointer checked), valid range: <> NULL
 *
 * @return
 * -TRUE             - success
 * -FALSE            - failure
 */
BOOLEAN SOD_ActualLenGetVirt(BYTE_B_INSTNUM_ SOD_t_ACS_OBJECT_VIRT const * const ps_acs,
                         UINT32 * const pdw_objLen);
#endif

/** @} */
