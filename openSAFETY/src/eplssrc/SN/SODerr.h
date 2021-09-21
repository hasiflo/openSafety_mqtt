/**
 * @addtogroup SOD
 * @{
 *
 * @file SODerr.h
 *
 * This file contains the error codes for the SOD module.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SODerr.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
 */

#ifndef SODERR_H
#define SODERR_H


/**
 * @name SOD macros
 * @brief Macros to build error code
 * @{
 */
#define SOD_ERR_FATAL(num) SERR_BUILD_ERROR(SERR_TYPE_FS, SERR_CLASS_FATAL,\
                                            SOD_k_UNIT_ID, (num))
#define SOD_ERR_MINOR(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_MINOR,\
                                            SOD_k_UNIT_ID, (num))
#define SOD_ERR__INFO(num) SERR_BUILD_ERROR(SERR_TYPE_NOT_FS, SERR_CLASS_INFO,\
                                            SOD_k_UNIT_ID, (num))
/** @} */

/**
 * @name SOD error codes
 * @brief Error codes for the SOD module.
 * @{
*/

/** SODmain.c **/
/** SOD_Read */
/* Generated abort code for the SSDOS and SPDO */
#define SOD_k_ERR_INST_NUM_3            SOD_ERR_FATAL(1)  /* (0800 0000h) */
#define SOD_k_ERR_TYPE_OFFSET_1         SOD_ERR_FATAL(2)  /* (0800 0000h) */
#define SOD_k_ERR_OFFSET_SIZE_1         SOD_ERR_FATAL(3)  /* (0607 0012h) */
#define SOD_k_ERR_SAPL_READ             SOD_ERR_FATAL(4)  /* Abort code
                                                             provided by the
                                                             application */
/** SOD_Write */
#define SOD_k_ERR_INST_NUM_4            SOD_ERR_FATAL(5)  /* (0800 0000h) */
#define SOD_k_ERR_DATA_POINTER_INVALID  SOD_ERR_FATAL(6)  /* (0800 0000h) */
#define SOD_k_ERR_SOD_WRITE_DISABLED    SOD_ERR_MINOR(7)  /* (0800 0022h) */
#define SOD_k_ERR_TYPE_OFFSET_2         SOD_ERR_FATAL(8)  /* (0800 0000h) */
#define SOD_k_ERR_OFFSET_SIZE_2         SOD_ERR_FATAL(9) /* (0607 0012h) */
#define SOD_k_ERR_SAPL_WRITE            SOD_ERR_MINOR(10) /* Abort code
                                                             provided by the
                                                             application */
/** SOD_ObjectWriteable */
#define SOD_k_ERR_OBJ_NOT_WRITEABLE     SOD_ERR_MINOR(11) /* (0601 0002h) */
/** VisibleStringCheck */
#define SOD_k_ERR_VISIBLE_STR_INVALID   SOD_ERR_MINOR(12) /* (0609 0030h) */
/** SOD_DataValid */
#define SOD_k_ERR_VALUE_IS_TOO_HIGH_1   SOD_ERR_MINOR(13) /* (0609 0031h) */
#define SOD_k_ERR_VALUE_IS_TOO_LOW      SOD_ERR_MINOR(14) /* (0609 0032h) */
#define SOD_k_ERR_VALUE_IS_TOO_HIGH_2   SOD_ERR_MINOR(15) /* (0609 0031h) */
#define SOD_k_ERR_TYPE_UNKNOWN          SOD_ERR_MINOR(16) /* (0800 0021h) */
/** CallBeforeReadClbk */
#define SOD_k_ERR_CLBK_ABORT_BR         SOD_ERR_MINOR(17)
/** CallBeforeWriteClbk */
#define SOD_k_ERR_CLBK_ABORT_BW         SOD_ERR_MINOR(18)
/** SOD_CallAfterWriteClbk */
#define SOD_k_ERR_CLBK_ABORT_AW         SOD_ERR_MINOR(19)


/** SODactLen.c **/
/** SOD_ActualLenSet */
#define SOD_k_ERR_INST_NUM_9            SOD_ERR_FATAL(20)  /* (0800 0000h) */
#define SOD_k_ERR_ACT_LEN_SET_APL       SOD_ERR_FATAL(21)  /* (0800 0000h) */
#define SOD_k_ERR_ACT_LEN               SOD_ERR_FATAL(22)  /* (0800 0000h) */
#define SOD_k_ERR_ACT_LEN_TYPE          SOD_ERR_FATAL(23)  /* (0800 0000h) */
/** SOD_ActualLenGet */
#define SOD_k_ERR_INST_NUM_10           SOD_ERR_FATAL(24)  /* (0800 0000h) */
#define SOD_k_ERR_ACT_LEN_PTR           SOD_ERR_FATAL(25)  /* (0800 0000h) */
#define SOD_k_ERR_ACT_LEN_GET_APL       SOD_ERR_FATAL(26)  /* (0800 0000h) */


/** SODattr.c **/
/** SOD_InitAddr */
#define SOD_k_ERR_OD_REFERENCE          SOD_ERR_FATAL(27)
#define SOD_k_ERR_OD_IS_EMPTY           SOD_ERR_FATAL(28)
/** SOD_AttrGet */
#define SOD_k_ERR_INST_NUM_1            SOD_ERR_FATAL(29) /* (0800 0000h) */
#define SOD_k_ERR_OBJ_HDL_1             SOD_ERR_FATAL(30) /* (0800 0000h) */
#define SOD_k_ERR_APP_OBJ_1             SOD_ERR_FATAL(31) /* (0800 0000h) */
#define SOD_k_ERR_SAPL_ATTR_GET         SOD_ERR_FATAL(32)
#define SOD_k_ERR_ATTR_GET              SOD_ERR_FATAL(33)
/** SOD_AttrGetNext */
#define SOD_k_ERR_INST_NUM_2            SOD_ERR_FATAL(34) /* (0800 0000h) */
#define SOD_k_ERR_OBJ_HDL_2             SOD_ERR_FATAL(35) /* (0800 0000h) */
/** SOD_HdlValid */
#define SOD_k_ERR_OBJ_HDL_INVALID       SOD_ERR_FATAL(36) /* (0800 0021h) */


/** SODlock.c **/
/** SOD_Lock */
#define SOD_k_ERR_INST_NUM_5            SOD_ERR_FATAL(37) /* (0800 0000h) */
#define SOD_k_ERR_LOCKING_FAILED        SOD_ERR__INFO(38) /* (0800 0022h) */
#define SOD_k_ERR_SAPL_LOCKING_FAILED   SOD_ERR__INFO(39) /* (0800 0022h) */
/** SOD_Unlock */
#define SOD_k_ERR_INST_NUM_6            SOD_ERR_FATAL(40) /* (0800 0000h) */
#define SOD_k_ERR_UNLOCK_WITHOUT_LOCK   SOD_ERR_FATAL(41) /* (0800 0022h) */
#define SOD_k_ERR_SAPL_UNLOCK_FAILED    SOD_ERR_FATAL(42) /* (0800 0022h) */
/** SOD_EnableSodWrite */
#define SOD_k_ERR_INST_NUM_7            SOD_ERR_FATAL(43) /* (0800 0000h) */
#define SOD_k_ERR_ENABLE_WR_FAILED      SOD_ERR_FATAL(44)
/** SOD_DisableSodWrite */
#define SOD_k_ERR_INST_NUM_8            SOD_ERR_FATAL(45) /* (0800 0000h) */
#define SOD_k_ERR_DISABLE_WR_FAILED     SOD_ERR__INFO(46)

/** SODcheck.c **/
/** SOD_Init */
#define SOD_k_ERR_SAPL_INIT             SOD_ERR_FATAL(47)
/** SOD_Check */
#define SOD_k_ERR_NO_RANGE_CHECKING     SOD_ERR_FATAL(48)
#define SOD_k_ERR_NO_OBJECT_DATA        SOD_ERR_FATAL(49)
#define SOD_k_ERR_NO_P2P_OBJECT_DATA    SOD_ERR_FATAL(50)
#define SOD_k_ERR_SOD_NOT_SORTED_1      SOD_ERR_FATAL(51)
#define SOD_k_ERR_SOD_NOT_SORTED_2      SOD_ERR_FATAL(52)
#define SOD_k_ERR_SOD_ENTRY_DOUBLE      SOD_ERR_FATAL(91)
/** AttrCheck */
#define SOD_k_ERR_ATTR_IS_WRONG_1       SOD_ERR_FATAL(53)
#define SOD_k_ERR_ATTR_IS_WRONG_2       SOD_ERR_FATAL(54)
#define SOD_k_ERR_ATTR_IS_WRONG_3       SOD_ERR_FATAL(55)
#define SOD_k_ERR_NO_DEFAULT_VALUE      SOD_ERR_FATAL(56)
#define SOD_k_ERR_OBJ_CLBK_1            SOD_ERR_FATAL(57)
#define SOD_k_ERR_OBJ_CLBK_2            SOD_ERR_FATAL(58)
/** DataTypeCheck */
#define SOD_k_ERR_MAX_MIN_VALUE_ERROR   SOD_ERR_FATAL(59)
#define SOD_k_ERR_TYPE_LEN_1            SOD_ERR_FATAL(60)
#define SOD_k_ERR_TYPE_LEN_2            SOD_ERR_FATAL(61)
#define SOD_k_ERR_TYPE_LEN_3            SOD_ERR_FATAL(62)
#define SOD_k_ERR_TYPE_LEN_4            SOD_ERR_FATAL(63)
#define SOD_k_ERR_TYPE_LEN_5            SOD_ERR_FATAL(64)
#define SOD_k_ERR_TYPE_NOT_SUPPORTED    SOD_ERR_FATAL(65)

/** SODvirt.c **/
/** SOD_AttrGetVirt */
/* Generated abort code for the SSDOS and SPDO */
#define SOD_k_ERR_INST_NUM_11           SOD_ERR_FATAL(66)  /* (0800 0000h) */
#define SOD_k_ERR_ACS_OBJ_1             SOD_ERR_FATAL(67)  /* (0800 0000h) */
#define SOD_k_ERR_SIMU_FCT_1            SOD_ERR_FATAL(68)
#define SOD_k_ERR_ATTR_GET_VIRT         SOD_ERR_FATAL(69)
/** SOD_AttrGetNextVirt */
#define SOD_k_ERR_INST_NUM_12           SOD_ERR_FATAL(70)  /* (0800 0000h) */
#define SOD_k_ERR_ACS_OBJ_2             SOD_ERR_FATAL(71)  /* (0800 0000h) */
/** SOD_ReadVirt */
#define SOD_k_ERR_INST_NUM_13           SOD_ERR_FATAL(72)  /* (0800 0000h) */
#define SOD_k_ERR_ACS_OBJ_3             SOD_ERR_FATAL(73)  /* (0800 0000h) */
#define SOD_k_ERR_TYPE_OFFSET_3         SOD_ERR_FATAL(74)  /* (0800 0000h) */
#define SOD_k_ERR_CLBK_ABORT_BR_2       SOD_ERR_MINOR(75)
#define SOD_k_ERR_OFFSET_SIZE_3         SOD_ERR_FATAL(76)  /* (0607 0012h) */
/** SOD_WriteVirt */
#define SOD_k_ERR_INST_NUM_14           SOD_ERR_FATAL(77)  /* (0800 0000h) */
#define SOD_k_ERR_ACS_OBJ_4             SOD_ERR_FATAL(78)  /* (0800 0000h) */
#define SOD_k_ERR_DATA_POINTER_INVALID_2  SOD_ERR_FATAL(79)  /* (0800 0000h) */
#define SOD_k_ERR_SOD_WRITE_DISABLED_2  SOD_ERR_MINOR(80)  /* (0800 0022h) */
#define SOD_k_ERR_TYPE_OFFSET_4         SOD_ERR_FATAL(81)  /* (0800 0000h) */
#define SOD_k_ERR_CLBK_ABORT_BW_2       SOD_ERR_MINOR(82)
#define SOD_k_ERR_CLBK_ABORT_BW_3       SOD_ERR_MINOR(83)
/** SOD_ActualLenSetVirt */
#define SOD_k_ERR_INST_NUM_15           SOD_ERR_FATAL(84)  /* (0800 0000h) */
#define SOD_k_ERR_ACS_OBJ_5             SOD_ERR_FATAL(85)  /* (0800 0000h) */
#define SOD_k_ERR_ACT_LEN_2             SOD_ERR_FATAL(86)  /* (0800 0000h) */
#define SOD_k_ERR_ACT_LEN_TYPE_2        SOD_ERR_FATAL(87)  /* (0800 0000h) */
/** SOD_ActualLenGetVirt */
#define SOD_k_ERR_INST_NUM_16           SOD_ERR_FATAL(88)  /* (0800 0000h) */
#define SOD_k_ERR_ACS_OBJ_6             SOD_ERR_FATAL(89)  /* (0800 0000h) */
#define SOD_k_ERR_ACT_LEN_PTR_2         SOD_ERR_FATAL(90)  /* (0800 0000h) */

/** @} */

/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param              w_errorCode        error number
* @param              dw_addInfo         additional error information
* @retval                pac_str            empty buffer to build the error string
*/
#if (EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
  void SOD_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str);
#endif

#endif

/** @} */
