/**
 * @addtogroup EPLS
 * @{
 *
 * @file EPLStypes.h openSAFETY type definition
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 * @author M. Molnar, IXXAT Automation GmbH
 * @author Hans Pill, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * EPLStypes defines global definitions and macros for the openSAFETY stack
 *
 * <h2>History for EPLStypes.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>14.06.2010</td><td>Hans Pill</td><td>changed minor stack version because of new error message but full compatibility</td></tr>
 *     <tr><td>19.10.2010</td><td>Hans Pill</td><td>changed size of serialization/deserialization buffer to EPLS_cfg_MAX_PYLD_LEN_FAST_SSDO</td></tr>
 *     <tr><td>24.11.2010</td><td>Hans Pill</td><td>Review SL V20</td></tr>
 *     <tr><td>21.03.2011</td><td>Hans Pill</td><td>changed minor stack version because of new memory usage for mapping but full compatibility</td></tr>
 *     <tr><td>19.04.2011</td><td>Hans Pill</td><td>A&P258030 interface for SHNF to get the info of the RxSPDOs -> minor stack version changed</td></tr>
 *     <tr><td>06.09.2011</td><td>Hans Pill</td><td>A&P263905 extended bootup message interval by number of retries the subindex for the bootup message did change changed stack version</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 *     <tr><td>30.01.2013</td><td>Hans Pill</td><td>on request of Roland Knall all datatypes are enumerated all the time</td></tr>
 *     <tr><td>26.03.2013</td><td>Hans Pill</td><td>stack version changed to 1.4.0.0</td></tr>
 *     <tr><td>09.04.2013</td><td>Hans Pill</td><td>2 byte stack version and 32 bit stack CRC added</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>minor changes for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 *     <tr><td>09.06.2016</td><td>Stefan Innerhofer</td><td>add defines for the instance DVI handling object</td></tr>
 * </table>
 *
 */

#ifndef EPLSTYPES_H
#define EPLSTYPES_H

/**
 * @name Stack Definitions
 * These definitions specify the stack version in use
 * @{
 */

/**
 * This symbol represents the version of the openSAFETY stack and unittest.
 *
 * @attention Must not be changed
 */
#define EPLS_k_STACK_VERSION     "1.4.0.0"

/**
 * This symbol represents the version of the openSAFETY stack and unittest in the SOD format.
 *
 * @attention Must not be changed
 */
#define EPLS_k_STACK_VERSION_SOD 0x0104U

/**
 * This symbol represents the CRC32 of the openSAFETY stack.
 */
#define EPLS_k_STACK_CRC32       0x00000000UL

/**
 * This symbol represents the status of the EPLS stack and unittest.
 *
 * @attention Must not be changed
 */
#define EPLS_k_STACK_STATUS      "RELEASE"

/**
 * This symbol substitutes the parameter B_INSTNUM in case of the instance number itself lead to an error.
 *
 * This symbol is also used as return value in case of no assignment between a received Safety
 * Domain Number an a instance number could be found.
 */
#define EPLS_k_NO_INSTANCE       0xFFU

/**
 * This symbol represents the value of consecutive time CT-field is not used
 * inside this SNMT Service.
 */
#define EPLS_k_CT_NOT_USED       0x0000U

/**
 * This symbol represents the value of Time Request Distinctive Number-field
 * is not used inside this SNMT Service.
 */
#define EPLS_k_TR_NOT_USED       0x00U

/**
 * This symbol represents the length of the physical address (UDID) of a SN.
 */
#define EPLS_k_UDID_LEN          0x06u

/**
 * This symbol represents the length of the extended CT value.
 */
#define EPLS_k_LEN_EXT_CT        0x05u

/**
 * This symbol represents the length of zero in case of no payload data
 * is transmitted.
 */
#define EPLS_k_NO_DATA_LEN       0x00u

/**
 * This symbol represents the maximum length of the openSAFETY frame header
 * (4 header bytes in sub frame ONE + 2 CRC bytes + 5 header bytes in sub frame
 *  TWO + 2 CRC bytes).
 */
#define EPLS_k_MAX_HDR_LEN       13u

/* @} */

/**
 * @name Addressing parameter limits:
 * @{
 *   These symbols represent limits of parameter inside the frame header.
 */

/** Minimum source address */
#define EPLS_k_MIN_SADR          0x0001U
/** Maximum source address */
#define EPLS_k_MAX_SADR          0x03FFU
/** min. additional source adr. */
#define EPLS_k_MIN_ADD_SADR      0x0001U
/** max. additional source adr. */
#define EPLS_k_MAX_ADD_SADR      0x03FFU
/** min. SPDO number */
#define EPLS_k_MIN_SPDO_NUM      0x0002U
/** max. SPDO number */
#define EPLS_k_MAX_SPDO_NUM      0x03FFU
/** max. Time Request counter */
#define EPLS_k_MAX_TR            0x3FU
/* @} */

/**
 * @name Unit-identifiers:
 * @{
 * These symbols represent all the units that are available
 */

/** SCFM unit identifier */
#define SCFM_k_UNIT_ID            0U
/** SCM  unit identifier */
#define SCM_k_UNIT_ID             1U
/** SDN  unit identifier */
#define SDN_k_UNIT_ID             2U
/** SFS unit identifier   */
#define SFS_k_UNIT_ID             3U
/** SHNF unit identifier */
#define SHNF_k_UNIT_ID            4U
/** SNMTM unit identifier */
#define SNMTM_k_UNIT_ID           5U
/** SNMTS unit identifier */
#define SNMTS_k_UNIT_ID           6U
/** SOD unit identifier */
#define SOD_k_UNIT_ID             7U
/** SPDO unit identifier  */
#define SPDO_k_UNIT_ID            8U
/** SSC unit identifier */
#define SSC_k_UNIT_ID             9U
/** SSDOC unit identifier */
#define SSDOC_k_UNIT_ID          10U
/** SSDOS unit identifier */
#define SSDOS_k_UNIT_ID          11U
/* @\ */

/**
 * @name SOD index and sub-index definitions
 * @{
 * These symbols are used to access the SOD.
 */
/** initial value of the SOD index */
#define EPLS_k_IDX_UNKNOWN               0x0000U
/** initial value of the SOD sub-index */
#define EPLS_k_SUBIDX_UNKNOWN            0x00U
/** Common communication  parameter */
#define EPLS_k_IDX_COMMON_COM_PARAM      0x1200U
/** Safety Domain Number */
#define EPLS_k_SUBIDX_SDN                0x01U
/** SADR of the SCM */
#define EPLS_k_SUBIDX_SADR_SCM           0x02U
/** UDID of the SCM */
#define EPLS_k_SUBIDX_UDID_SCM           0x04U
/** SNMT communication parameter */
#define EPLS_k_IDX_SNMT_COM_PARAM        0x1202U
/** SNMT response timeout */
#define EPLS_k_SUBIDX_SNMT_TIMEOUT       0x01U
/** Number of response repetitions */
#define EPLS_k_SUBIDX_SNMT_RETRIES       0x02U
/** SSDO communication parameter */
#define EPLS_k_IDX_SSDO_COM_PARAM        0x1201U
/** SSDO response timeout */
#define EPLS_k_SUBIDX_SSDO_TIMEOUT       0x01U
/** Number of response repetitions */
#define EPLS_k_SUBIDX_SSDO_RETRIES       0x02U
/** TxSPDO communication parameter */
#define EPLS_k_IDX_TXSPDO_COM_PARAM      0x1C00U
/** SADR TxSPDO 1 (main SADR)*/
#define EPLS_k_SUBIDX_TXSPDO_SADR        0x01U
/** Refresh prescaler TxSPDO */
#define EPLS_k_SUBIDX_TXSPDO_REFR_PRESC  0x02U
/** Number of consecutive TRes */
#define EPLS_k_SUBIDX_TXSPDO_NUM_TRES    0x03U
/** Life guarding parameter */
#define EPLS_k_IDX_LIFE_GUARDING         0x100CU
/** Guard time protocol */
#define EPLS_k_SUBIDX_GUARD_TIME         0x01U
/** Life time factor */
#define EPLS_k_SUBIDX_LIFE_TIME_FACTOR   0x02U
/** Preoperational parameter */
#define EPLS_k_IDX_PRE_OP_SIGNAL         0x100DU
/** Refresh time (timeout) */
#define EPLS_k_SUBIDX_REFRESH_TIME       0x00U
/** Preoperational parameter */
#define EPLS_k_IDX_PRE_OP_RETRIES        0x100EU
/** number of retries */
#define EPLS_k_SUBIDX_NO_RETRIES         0x00U
/** Device vendor ID */
#define EPLS_k_IDX_DEVICE_VEN_ID         0x1018U
/** DVI product code*/
#define EPLS_k_SUBIDX_PRODUCT_CODE       0x02U
/** Parameter checksum */
#define EPLS_k_SUBIDX_PARAM_CHKSUM       0x06U
/** Parameter timestamp */
#define EPLS_k_SUBIDX_PARAM_TS           0x07U
/** Unique device id */
#define EPLS_k_IDX_UNIQUE_DEVICE_ID      0x1019U
/** Unique device id */
#define EPLS_k_SUBIDX_UNIQUE_DEVICE_ID   0x00U
/** instance DVI handling */
#define EPLS_k_IDX_INSTANCE_DVI_HANDLING   0xE100U

/* @} */

/**
 * @def EPLS_TIMEOUT(dw_actTime, dw_toTime)
 * @brief Macro for timeout checking.
 *
 * The maximum value for the timeout offset (k_TIMEOUT_VALUE) is 0x80000000 (2^31).
 *
 * @return
 * - TRUE if the timeout time is reached or overshoot
 * - FALSE if the timeout time is not reached.
 *
 * @param dw_actTime (UINT32) Actual time (current 'dw_ct' value).
 * @param dw_toTime  (UINT32) Timeout time, calculated via: dw_toTime = dw_ct + k_TIMEOUT_VALUE.
 */
#define EPLS_TIMEOUT(dw_actTime, dw_toTime)  \
                    ((UINT32)((dw_actTime) - (dw_toTime)) < 0x80000000UL)

/**
 * @brief Macro to check whether a bit is set.

 * @return
 * - TRUE if the bit is set
 * - FALSE otherwise
 *
 * @param var Variable in which the bit is checked.
 * @param bit Bit that has to be checked in the given variable.
 */
#define EPLS_IS_BIT_SET(var,bit) ((BOOLEAN)(((var) & (bit))>0U))

/**
 * @brief Macro to check whether a bit is cleared (not set).
 * If the bit is not set then the macro is TRUE, otherwise FALSE.

 * @param var Variable in which the bit is checked.
 * @param bit Bit that has to be checked in the given variable.
 *
 * @return
 * - TRUE if the bit is not set
 * - FALSE otherwise
 */
#define EPLS_IS_BIT_RESET(var,bit) ((BOOLEAN)(((var) & (bit))==0U))

/**
 * @brief Macro to set a bit in a unsigned 8-bit variable.
 *
 * @param var (UINT8) Variable (8 bit) in which the bit has to be set.
 * @param bit (UINT8) Bit with the right position to be set (8 bit).
 */
#define EPLS_BIT_SET_U8(var,bit) ((var) = (UINT8)((var) | (bit)))

/**
 * @brief Macro to reset a bit in a unsigned 8-bit variable.
 *
 * @param var (UINT8) Variable (8 bit) in which the bit has to be cleared.
 * @param bit (UINT8) Bit with the right position to be set (8 bit).
 */
#define EPLS_BIT_RESET_U8(var,bit) ((var) = (UINT8)((var) & ((UINT8)(~(bit)))))

/**
 * @def EPLS_BIT_SET_U16
 * @brief Macro to set a bit in a unsigned 16-bit variable.
 * @param var (UINT16) Variable (16 bit) in which the bit has to be set.
 * @param bit (UINT16) Bit with the right position to be set (16 bit).
 */
#define EPLS_BIT_SET_U16(var,bit) ((var) = (UINT16)((var) | (bit)))

/**
 * @name Instance macros
 *
 * b_instNum function parameter used for multiple instances and eliminated by the
 * following instance macros if only one instance is present.
 *
 * @attention Is only set if more than one instance is configured
 *
 * @{
 */
#if EPLS_cfg_MAX_INSTANCES > 1 /* if more instances are configured */
/** function prototype: b_instNum is only function parameter */
#define BYTE_B_INSTNUM       UINT8 b_instNum
/** function prototype: b_instNum is first function parameter */
#define BYTE_B_INSTNUM_      UINT8 b_instNum,
/** function call: b_instNum is the parameter */
#define B_INSTNUM            b_instNum
/** function call: b_instNum is the first function parameter */
#define B_INSTNUM_           b_instNum,
/** b_instNum is index into a table */
#define B_INSTNUMidx         b_instNum

/**
 * This symbol defines the instance number used by the Safety Configuration Manager (SCM).
 */
#define EPLS_k_SCM_INST_NUM  0U
/** function call: EPLS_k_SCM_INST_NUM is the first function parameter */
#define EPLS_k_SCM_INST_NUM_ EPLS_k_SCM_INST_NUM,
/** function call: EPLS_k_NO_INSTANCE is the first function parameter */
#define EPLS_k_NO_INSTANCE_  EPLS_k_NO_INSTANCE,
#else /* else only one instance is configured */
/** function prototype: b_instNum is only function parameter */
#define BYTE_B_INSTNUM   void
/** function prototype: b_instNum is first function parameter */
#define BYTE_B_INSTNUM_

/** function call: b_instNum is the parameter */
#define B_INSTNUM
/** function call: b_instNum is the first function parameter */
#define B_INSTNUM_
/** b_instNum is index into a table */
#define B_INSTNUMidx     0U

/**
 * This symbol defines the instance number used by the Safety Configuration Manager (SCM).
 */
#define EPLS_k_SCM_INST_NUM
/** function call: EPLS_k_SCM_INST_NUM is the first function parameter */
#define EPLS_k_SCM_INST_NUM_
/** function call: EPLS_k_NO_INSTANCE is the first function parameter */
#define EPLS_k_NO_INSTANCE_
#endif

/* @} */

/**
 * @name Data types
 * @{
 */

/**
 * @brief Structured data type to transfer frame header information.
 */
typedef struct
{
    /** 10 bit value of ADR field in the frame header */
    UINT16 w_adr;
    /* frame information which consists of frame type (bit 3,4,5) and telegram type (bit 0,1,2) */
    UINT8 b_id;
    /* 10 bit value of openSAFETY domain number */
    UINT16 w_sdn;
    /* number of payload data within the frame */
    UINT8 b_le;
    /* consecutive time, internal timer value of the safety node */
    UINT16 w_ct;
    /* additional address for different purposes depending on frame type */
    UINT16 w_tadr;
    /* time request distinctive number (0 .. 63) */
    UINT8 b_tr;
#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
    /* upper 24 bit of the CT value */
    UINT32 dw_extCt;
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */
} EPLS_t_FRM_HDR;

/**
 * @brief Structured data type to transfer complete openSAFETY frame.
 */
typedef struct
{
    /** frame header information */
    EPLS_t_FRM_HDR s_frmHdr;
    /** frame data */
    UINT8 ab_frmData[EPLS_cfg_MAX_PYLD_LEN_FAST_SSDO];
} EPLS_t_FRM;

/**
 * Enumeration of the supported openSAFETY data types, necessary for
 * serialization / deserialization of data from/into network format.
 */
typedef enum
{
    /** data type boolean (TRUE / FALSE) */
    EPLS_k_BOOLEAN,
    /** data type 8 bit signed integer */
    EPLS_k_INT8,
    /** data type 16 bit signed integer */
    EPLS_k_INT16,
    /** data type 32 bit signed integer */
    EPLS_k_INT32,
    /** data type 64 bit signed integer */
    EPLS_k_INT64,
    /** data type 8 bit unsigned integer */
    EPLS_k_UINT8,
    /** data type 16 bit unsigned integer */
    EPLS_k_UINT16,
    /** data type 32 bit unsigned integer */
    EPLS_k_UINT32,
    /** data type 64 bit unsigned integer */
    EPLS_k_UINT64,
    /** data type 32 bit floating point */
    EPLS_k_REAL32,
    /** data type 64 bit floating point */
    EPLS_k_REAL64,
    /**
     * data type array of 7-bit characters encoded as
     * 8 bit unsigned integer (ISO 646-1973),
     * 00h is not necessary to terminate the string
     */
    EPLS_k_VISIBLE_STRING,
    /**
     * application specific data type for data transfer
     * of client-server communication model
     */
    EPLS_k_DOMAIN,
    /** data type array of 8 bit unsigned integer */
    EPLS_k_OCTET_STRING,
    /** unknown data type - used for simulated entries */
    EPLS_k_UNKNOWN
} EPLS_t_DATATYPE;
#endif
/* @} */

/* @} */
