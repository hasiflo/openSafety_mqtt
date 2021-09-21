/**
 * @addtogroup SOD
 * @{
 *
 * @file SODmain.c
 *
 * This file manages the access to Object Dictionaries.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SODmain.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 *     <tr><td>17.02.2017</td><td>Stefan Innerhofer</td><td>optimized SOD_SetDefaultValues()</td></tr>
 * </table>
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#include "SERRapi.h"
#include "SERR.h"

#include "SCFMapi.h"

#include "SODerr.h"
#include "SODapi.h"
#include "SOD.h"
#include "SODint.h"


/**
 * This define represents the zero valid string.
*/
#define k_VISIBLE_STR_BYTE_VALID        0x00U

/**
 * This define represents the low limit of a visible string byte.
*/
#define k_VISIBLE_STR_BYTE_LOW_LIMIT    0x20U

/**
 * This define represents the high limit of a visible string byte.
*/
#define k_VISIBLE_STR_BYTE_HIGH_LIMIT   0x7EU

/**
 * @def RANGE_CHECK
 * Macro for the range checking to increase the readability in the DataValid().
 * @param type type of the data to be checked
*/
#define RANGE_CHECK(type) \
{\
  /* if range checking is necessary */\
  if (ps_object->pv_range != NULL)\
  {\
    /* if data is too high */\
    if ( ((type *)(ps_object->pv_range))[k_RANGE_HIGH_LIMIT] < \
        *((type *)(pv_data)) )\
    {\
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_VALUE_IS_TOO_HIGH_1,\
                    (UINT32)(SOD_ABT_VALUE_IS_TOO_HIGH));\
    }\
    /* else if data is too low */\
    else if ( ((type *)(ps_object->pv_range))[k_RANGE_LOW_LIMIT] > \
              *((type *)(pv_data)) )\
    {\
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_VALUE_IS_TOO_LOW,\
                    (UINT32)(SOD_ABT_VALUE_IS_TOO_LOW));\
    }\
    /* else data is acceptable */\
    else\
    {\
      o_ret = TRUE;\
    }\
  }\
  /* else range checking is not necessary */\
  else\
  {\
    o_ret = TRUE;\
  }\
}\

/***
*    static function-prototypes
***/
static BOOLEAN SetDefaultValues(BYTE_B_INSTNUM);
static BOOLEAN VisibleStringCheck(BYTE_B_INSTNUM_ const void *pv_data,
                                  UINT32 dw_strLen);
static BOOLEAN CallBeforeReadClbk(BYTE_B_INSTNUM_ const SOD_t_OBJECT *ps_object,
                                  UINT32 dw_offset, UINT32 dw_size,
                                  SOD_t_ERROR_RESULT *ps_errRes);
static BOOLEAN CallBeforeWriteClbk(BYTE_B_INSTNUM_ const void *pv_data,
                                   const SOD_t_OBJECT *ps_object,
                                   UINT32 dw_offset, UINT32 dw_size);

/**
 * @brief This function initializes all module global and global variables defined in the unit SOD and checks the SOD.
 *
 * @param        b_instNum           instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return
 * - TRUE              - success
 * - FALSE             - failure
 */
BOOLEAN SOD_Init(BYTE_B_INSTNUM)
{
  BOOLEAN o_ret = FALSE; /* return value */

#if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
  /* if the default values of the application objects were not set
        successfully */
  if (!SAPL_SOD_DefaultValueSetClbk(B_INSTNUM))
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_SAPL_INIT, SERR_k_NO_ADD_INFO);
  }
  /* else the default values of the application objects were set
          successfully */
  else
#endif
  {
    SOD_InitFlags(B_INSTNUM);

    /* if the initialization function finished successfully */
    if (SOD_InitAddr(B_INSTNUM))
    {
      /* if the initialization function of the virtual SOD finished successfully */
      SOD_InitVirt(B_INSTNUM);
      /* if SOD checking was successful */
      if (SOD_Check(B_INSTNUM))
      {
        /* set the SOD default values */
        if (SetDefaultValues(B_INSTNUM))
        {
          /* set the virtual SOD default values */
          o_ret = SOD_SetDefaultValuesVirt(B_INSTNUM);
        }
      }
      /* no else : error happened, error has been already signaled */
    }
    /* no else : error happened, error has been already signaled */
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

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
               UINT32 dw_offset, UINT32 dw_size, SOD_t_ERROR_RESULT *ps_errRes)
{
  void *pv_ret = NULL; /* return value */
  void *pv_objData; /* pointer to the object data */
  const SOD_t_OBJECT *ps_object; /* pointer to an object in the OD */
  #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE)
    o_appObj = o_appObj; /* to avoid compiler warning */
  #endif

    /* if the reference to the error result struture is NULL */
    if (ps_errRes == NULL)
    {
      /* return NULL, the error can not be signaled because the error result
         structure is NULL */
    }
  #if (EPLS_cfg_MAX_INSTANCES > 1)
    /* else if b_instNum is wrong */
    else if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      ps_errRes->w_errorCode = SOD_k_ERR_INST_NUM_3;
      ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
    }
  #endif
    else  /* else all parameter ok */
    {
      #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
        /* if application object */
        if (o_appObj)
        {
          ps_errRes->w_errorCode = SOD_k_ERR_SAPL_READ;
          ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;

          pv_ret = SAPL_SOD_ReadClbk(B_INSTNUM_ dw_hdl, dw_offset, dw_size,
                                     &ps_errRes->e_abortCode);

          /* if the access was OK */
          if (pv_ret != NULL)
          {
            ps_errRes->w_errorCode = SOD_k_ERR_NO;
            ps_errRes->e_abortCode = SOD_ABT_NO_ERROR;
          }
          /* no else : access of the application object failed */
        }
        /* else not application object */
        else
      #endif
        {
          /* if dw_hdl is valid */
          if (SOD_HdlValid(B_INSTNUM_ dw_hdl, ps_errRes))
          {
            /* set a pointer to the object */
            ps_object = (const SOD_t_OBJECT *)(dw_hdl); /*lint !e923
                                    Note 923: cast from unsigned long to
                                    pointer [MISRA 2004 Rule 11.3] */

            /* if offset and size is not 0 for the standard data types */
            if (((dw_offset != 0U) || (dw_size != 0U)) &&
                (ps_object->s_attr.e_dataType != EPLS_k_DOMAIN) &&
                (ps_object->s_attr.e_dataType != EPLS_k_VISIBLE_STRING) &&
                (ps_object->s_attr.e_dataType != EPLS_k_OCTET_STRING))
            {
              ps_errRes->w_errorCode = SOD_k_ERR_TYPE_OFFSET_1;
              ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
            }
            /* else offset and size match to the data type */
            else
            {
              /* if offset and size are valid (dw_offset + dw_size)
                 <= ps_object->s_attr.dw_objLen does not work because of
                 UINT32 overflow */
              if ((dw_offset < ps_object->s_attr.dw_objLen) &&
                  (dw_size <= (ps_object->s_attr.dw_objLen-dw_offset)))
              {
                /* if object callback with SOD_k_SRV_BEFORE_READ
                    service completed successful */
                if (CallBeforeReadClbk(B_INSTNUM_ ps_object, dw_offset,
                                       dw_size, ps_errRes))
                {
                  /* if the object data type is DOMAIN or VISIBLE STRING or
                        OCTET STRING */
                  if ((ps_object->s_attr.e_dataType ==
                       EPLS_k_VISIBLE_STRING) ||
                      (ps_object->s_attr.e_dataType == EPLS_k_OCTET_STRING) ||
                      (ps_object->s_attr.e_dataType == EPLS_k_DOMAIN))
                  {
                    pv_objData = ((SOD_t_ACT_LEN_PTR_DATA *)
                                         (ps_object->pv_objData))->pv_objData;
                  }
                  else /* other data type */
                  {
                    pv_objData = ps_object->pv_objData;
                  }

                  /* if SOD_k_ATTR_P2P is set */
                  if (EPLS_IS_BIT_SET(ps_object->s_attr.w_attr,
                                      SOD_k_ATTR_P2P))
                  {
                    /* pointer to the data is solved */
                    pv_ret =
                        (void *)(*((PTR_TYPE *)(pv_objData))); /*lint !e923
                                       Note 923: cast from unsigned long to
                                       pointer [MISRA 2004 Rule 11.3] */
                  }
                  /* else SOD_k_ATTR_P2P is not set */
                  else
                  {
                    /* pointer to the data is set */
                    pv_ret = pv_objData;
                  }
                  pv_ret = (void *)ADD_OFFSET(pv_ret, dw_offset);

                  ps_errRes->w_errorCode = SOD_k_ERR_NO;
                  ps_errRes->e_abortCode = SOD_ABT_NO_ERROR;
                }
                /* no else : callback failed */
              }
              /* else offset or/and size are invalid */
              else
              {
                ps_errRes->w_errorCode = SOD_k_ERR_OFFSET_SIZE_1;
                ps_errRes->e_abortCode = SOD_ABT_LEN_IS_TOO_HIGH;
              }
            }
          }
          /* no else : dw_hdl is invalid */
        }
    }

  return pv_ret;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */

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
                  UINT32 dw_size)
{
  BOOLEAN o_ret = FALSE; /* return value */
  const SOD_t_OBJECT *ps_object; /* pointer to an object in the OD */
  #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_DISABLE)
    o_appObj = o_appObj; /* to avoid compiler warning */
  #endif
  #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
    SOD_t_ABORT_CODES e_saplError;
  #endif

  #if (EPLS_cfg_MAX_INSTANCES > 1)
    /* if b_instNum is wrong */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      SERR_SetError(EPLS_k_NO_INSTANCE, SOD_k_ERR_INST_NUM_4,
                    (UINT32)SOD_ABT_GENERAL_ERROR);
    }
    else
  #endif
    /* else if the pointer to the data is invalid */
    if (pv_data == NULL)
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_DATA_POINTER_INVALID,
                    (UINT32)SOD_ABT_GENERAL_ERROR);
    }
    /* else all parameter ok */
    else
    {
      /* if the SOD write access is disabled */
      if (!SOD_WriteAccess(B_INSTNUM))
      {
        SERR_SetError(B_INSTNUM_ SOD_k_ERR_SOD_WRITE_DISABLED,
                      (UINT32)SOD_ABT_PRESENT_DEVICE_STATE);
      }
      /* else the SOD write access is enabled */
      else
      {
        #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
          /* if application object */
          if (o_appObj)
          {
            e_saplError = SAPL_SOD_WriteClbk(B_INSTNUM_ dw_hdl, pv_data,
                                            o_overwrite, dw_offset, dw_size);
            /* if the access was OK */
            if (e_saplError == SOD_ABT_NO_ERROR)
            {
              o_ret = TRUE;
            }
            /* if the access was not OK */
            else
            {
              SERR_SetError(B_INSTNUM_ SOD_k_ERR_SAPL_WRITE,
                            (UINT32)e_saplError);
            }
          }
          /* else not application object */
          else
        #endif
          {
            /* if dw_hdl is valid */
            if (SOD_HdlValid(B_INSTNUM_ dw_hdl, (SOD_t_ERROR_RESULT *)NULL))
            {
              /* set a pointer to the object */
              ps_object = (const SOD_t_OBJECT *)(dw_hdl); /*lint !e923
                                        Note 923: cast from unsigned long to
                                        pointer [MISRA 2004 Rule 11.3] */

              /* if offset and size is not 0 for the standard data types */
              if (((dw_offset != 0U) || (dw_size != 0U)) &&
                  (ps_object->s_attr.e_dataType != EPLS_k_DOMAIN) &&
                  (ps_object->s_attr.e_dataType != EPLS_k_VISIBLE_STRING) &&
                  (ps_object->s_attr.e_dataType != EPLS_k_OCTET_STRING))
              {
                SERR_SetError(B_INSTNUM_ SOD_k_ERR_TYPE_OFFSET_2,
                              (UINT32)SOD_ABT_GENERAL_ERROR);
              }
              /* else offset and size match to the data type */
              else
              {
                /* if offset and size are valid (dw_offset + dw_size)
                   <= ps_object->s_attr.dw_objLen does not work because of
                   UINT32 overflow */
                if ((dw_offset < ps_object->s_attr.dw_objLen) &&
                    (dw_size <= (ps_object->s_attr.dw_objLen-dw_offset)))
                {
                  /* if the read only write access is accepted */
                  if (SOD_ObjectReadOnly(pv_data, ps_object, o_overwrite))
                  {
                    o_ret = TRUE;
                  }
                  /* else the write access has to be checked */
                  else
                  {
                    /* if the object is writeable */
                    if (SOD_ObjectWriteable(B_INSTNUM_ ps_object, o_overwrite))
                    {
                      /* if data is valid */
                      if (SOD_DataValid(B_INSTNUM_ pv_data, ps_object, dw_offset,
                                    dw_size))
                      {
                        /* if the object callback with SOD_k_SRV_BEFORE_WRITE
                            service completed successfully */
                        if (CallBeforeWriteClbk(B_INSTNUM_ pv_data, ps_object,
                                              dw_offset, dw_size))
                        /* RSM_IGNORE_QUALITY_BEGIN Notice #55 - Scope level
                                              exceeds the defined limit of 10 */
                        {
                        /* RSM_IGNORE_QUALITY_END */
                          /* data is copied */
                          SOD_DataCpy(pv_data, ps_object, dw_offset, dw_size);

                          /* the segmented object callback will be called with
                             SOD_k_SRV_AFTER_WRITE service*/
                          o_ret = SOD_CallAfterWriteClbk(B_INSTNUM_ ps_object,
                                                     dw_offset, dw_size);
                        }
                        /* no else : callback function failed, error has already
                                     been signaled */
                      }
                      /* no else : data is not valid, error has already
                                 been signaled */
                    }
                    /* no else : object is not writeable, error has already
                                 been signaled */
                  }
                }
                /* else offset or/and size are invalid */
                else
                {
                  SERR_SetError(B_INSTNUM_ SOD_k_ERR_OFFSET_SIZE_2,
                                (UINT32)SOD_ABT_LEN_IS_TOO_HIGH);
                }
              }
            }
            /* no else : dw_hdl is invalid, error has already been signaled */
          }
      }
    }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */

#if (EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param              w_errorCode        error number
* @param              dw_addInfo         additional error information
* @retval                pac_str            empty buffer to build the error string
*/
  void SOD_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str)
  {

    /* Choose the matching error string */
    switch(w_errorCode)
    {
      /** SODmain.c **/
      /* SOD_Read */
      case SOD_k_ERR_INST_NUM_3:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_3: SOD_Read():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_INST_NUM_3, dw_addInfo);
        break;
      }
      case SOD_k_ERR_TYPE_OFFSET_1:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_TYPE_OFFSET_1: SOD_Read():\n"
                        "The object with standard data type "
                        "(UINT8/16/32, INT8/16/32, REAL32, BOOLEAN), "
                        "must not be accessed segmented "
                        "(offset and size must be 0). "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_TYPE_OFFSET_1, dw_addInfo);
        break;
      }
      case SOD_k_ERR_OFFSET_SIZE_1:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_OFFSET_SIZE_1: SOD_Read():\n"
                        "The byte position is outside of the object "
                        "data range (offset or/and size are wrong). "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_OFFSET_SIZE_1, dw_addInfo);
        break;
      }
      case SOD_k_ERR_SAPL_READ:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_SAPL_READ: SOD_Read():\n"
                        "The SAPL_SOD_ReadClbk() application callback function "
                        "has signaled error. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_SAPL_READ, dw_addInfo);
        break;
      }

      /* SOD_Write */
      case SOD_k_ERR_INST_NUM_4:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_4: SOD_Write():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_INST_NUM_4, dw_addInfo);
        break;
      }
      case SOD_k_ERR_DATA_POINTER_INVALID:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_DATA_POINTER_INVALID: SOD_Write():\n"
                        "Pointer to the data to be written is NULL. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_DATA_POINTER_INVALID, dw_addInfo);
        break;
      }
      case SOD_k_ERR_SOD_WRITE_DISABLED:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_SOD_WRITE_DISABLED: SOD_Write():\n"
                        "The SOD write access is disabled by the application. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_SOD_WRITE_DISABLED, dw_addInfo);
        break;
      }
      case SOD_k_ERR_TYPE_OFFSET_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_TYPE_OFFSET_2: SOD_Write():\n"
                        "The object with standard data type "
                        "(UINT8/16/32, INT8/16/32, REAL32, BOOLEAN), "
                        "must not be accessed segmented "
                        "(offset and size must be 0). "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_TYPE_OFFSET_2, dw_addInfo);
        break;
      }
      case SOD_k_ERR_OFFSET_SIZE_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_OFFSET_SIZE_2: SOD_Write():\n"
                        "The byte position is outside of the object "
                        "data range (offset or/and size are wrong). "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_OFFSET_SIZE_2, dw_addInfo);
        break;
      }
      case SOD_k_ERR_SAPL_WRITE:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_SAPL_WRITE: SOD_Write():\n"
                        "The SAPL_SOD_WriteClbk() application callback "
                        "function has signaled error. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_SAPL_WRITE, dw_addInfo);
        break;
      }

      /* SOD_ObjectWriteable */
      case SOD_k_ERR_OBJ_NOT_WRITEABLE:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_OBJ_NOT_WRITEABLE: "
                        "SOD_ObjectWriteable():\n"
                        "The object is not writeable. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_OBJ_NOT_WRITEABLE, dw_addInfo);
        break;
      }

      /* VisibleStringCheck */
      case SOD_k_ERR_VISIBLE_STR_INVALID:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_VISIBLE_STR_INVALID: "
                        "VisibleStringCheck():\n"
                        "Invalid visible string. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_VISIBLE_STR_INVALID, dw_addInfo);
        break;
      }

      /* SOD_DataValid */
      case SOD_k_ERR_VALUE_IS_TOO_HIGH_1:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_VALUE_IS_TOO_HIGH_1: SOD_DataValid():\n"
                        "The value to be written is too high. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_VALUE_IS_TOO_HIGH_1, dw_addInfo);
        break;
      }
      case SOD_k_ERR_VALUE_IS_TOO_LOW:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_VALUE_IS_TOO_LOW: SOD_DataValid():\n"
                        "The value to be written is too low. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_VALUE_IS_TOO_LOW, dw_addInfo);
        break;
      }
      case SOD_k_ERR_VALUE_IS_TOO_HIGH_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_VALUE_IS_TOO_HIGH_2: SOD_DataValid():\n"
                        "The value to be written is not BOOLEAN "
                        "value (0 or 1). "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_VALUE_IS_TOO_HIGH_2, dw_addInfo);
        break;
      }
      case SOD_k_ERR_TYPE_UNKNOWN:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_TYPE_UNKNOWN: SOD_DataValid():\n"
                        "Data type is unknown. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_TYPE_UNKNOWN, dw_addInfo);
        break;
      }

      /* CallBeforeReadClbk */
      case SOD_k_ERR_CLBK_ABORT_BR:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_CLBK_ABORT_BR: "
                        "CallBeforeReadClbk():\n"
                        "Before Read SOD Callback was aborted. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_CLBK_ABORT_BR, dw_addInfo);
        break;
      }

      /* CallBeforeWriteClbk */
      case SOD_k_ERR_CLBK_ABORT_BW:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_CLBK_ABORT_BW: "
                        "CallBeforeWriteClbk():\n"
                        "Before Write SOD Callback was aborted. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_CLBK_ABORT_BW, dw_addInfo);
        break;
      }

      /* SOD_CallAfterWriteClbk */
      case SOD_k_ERR_CLBK_ABORT_AW:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_CLBK_ABORT_AW: "
                        "SOD_CallAfterWriteClbk():\n"
                        "After Write SOD Callback was aborted. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_CLBK_ABORT_AW, dw_addInfo);
        break;
      }

      /** SODactLen.c **/
      case SOD_k_ERR_INST_NUM_9:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_9: SOD_ActualLenSet():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_INST_NUM_9, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACT_LEN_SET_APL:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACT_LEN_SET_APL: "
                "SOD_ActualLenSet():\n"
                "Actual length of the SOD entry can not be "
                "set by the application. "
                "SSDO abort code : %#lx\n",
                SOD_k_ERR_ACT_LEN_SET_APL, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACT_LEN:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACT_LEN: SOD_ActualLenSet():\n"
                "Actual length to be set is larger than the maximum "
                "length of the object. "
                "SSDO abort code : %#lx\n",
                SOD_k_ERR_ACT_LEN, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACT_LEN_TYPE:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACT_LEN_TYPE: SOD_ActualLenSet():\n"
                "Actual length is tried to set for an object "
                "with standard data type. Actual length can be set only "
                "for the objects with data type DOMAIN, OCTET STRING "
                "or VISIBLE STRING. "
                "SSDO abort code : %#lx\n",
                SOD_k_ERR_ACT_LEN_TYPE, dw_addInfo);
        break;
      }
      case SOD_k_ERR_INST_NUM_10:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_10: SOD_ActualLenGet():\n"
                "Instance number is invalid. "
                "SSDO abort code : %#lx\n",
                SOD_k_ERR_INST_NUM_10, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACT_LEN_PTR:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACT_LEN_PTR: SOD_ActualLenGet():\n"
                "Pointer to the actual length is NULL. "
                "SSDO abort code : %#lx\n",
                SOD_k_ERR_ACT_LEN_PTR, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACT_LEN_GET_APL:
      {
        SPRINTF2(pac_str,"%#x - SOD_k_ERR_ACT_LEN_GET_APL: "
                "SOD_ActualLenGet():\n"
                "Actual length of the SOD entry can not be "
                "get by the application. "
                "SSDO abort code : %#lx\n",
                SOD_k_ERR_ACT_LEN_GET_APL, dw_addInfo);
        break;
      }



      /** SODattr.c **/
      /* SOD_InitAddr */
      case SOD_k_ERR_OD_REFERENCE:
      {
        SPRINTF1(pac_str, "%#x - SOD_k_ERR_OD_REFERENCE: SOD_InitAddr():\n"
                        "Reference to the object dictionary is NULL.\n",
                SOD_k_ERR_OD_REFERENCE);
        break;
      }
      case SOD_k_ERR_OD_IS_EMPTY:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_OD_IS_EMPTY: SOD_InitAddr():\n"
                        "Object dictionary is empty at the instance (%lx).\n",
                SOD_k_ERR_OD_IS_EMPTY, dw_addInfo);
        break;
      }

      /* SOD_AttrGet */
      case SOD_k_ERR_INST_NUM_1:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_1: SOD_AttrGet():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_INST_NUM_1, dw_addInfo);
        break;
      }
      case SOD_k_ERR_OBJ_HDL_1:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_OBJ_HDL_1: SOD_AttrGet():\n"
                        "Pointer to the object handle is NULL. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_OBJ_HDL_1, dw_addInfo);
        break;
      }
      case SOD_k_ERR_APP_OBJ_1:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_APP_OBJ_1: SOD_AttrGet():\n"
                        "Pointer to the application object is NULL. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_APP_OBJ_1, dw_addInfo);
        break;
      }
      case SOD_k_ERR_SAPL_ATTR_GET:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_SAPL_ATTR_GET: SOD_AttrGet():\n"
                        "The SAPL_SOD_AttrGetClbk() application "
                        "callback function has signaled error. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_SAPL_ATTR_GET, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ATTR_GET:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ATTR_GET: SOD_AttrGet():\n"
                        "The SOD object was not found. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_ATTR_GET, dw_addInfo);
        break;
      }

      /* SOD_AttrGetNext */
      case SOD_k_ERR_INST_NUM_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_2: SOD_AttrGetNext():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_INST_NUM_2, dw_addInfo);
        break;
      }
      case SOD_k_ERR_OBJ_HDL_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_OBJ_HDL_2: SOD_AttrGetNext():\n"
                        "Pointer to the object handle is NULL. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_OBJ_HDL_2, dw_addInfo);
        break;
      }

      /* SOD_HdlValid */
      case SOD_k_ERR_OBJ_HDL_INVALID:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_OBJ_HDL_INVALID: SOD_HdlValid():\n"
                        "Object handle address does not point "
                        "to a SOD object. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_OBJ_HDL_INVALID, dw_addInfo);
        break;
      }



      /** SODlock.c **/
      /* SOD_Lock */
      case SOD_k_ERR_INST_NUM_5:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_5: SOD_Lock():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_INST_NUM_5, dw_addInfo);
        break;
      }
      case SOD_k_ERR_LOCKING_FAILED:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_LOCKING_FAILED: SOD_Lock():\n"
                        "Access to the SOD object is locked. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_LOCKING_FAILED, dw_addInfo);
        break;
      }
      case SOD_k_ERR_SAPL_LOCKING_FAILED:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_SAPL_LOCKING_FAILED: SOD_Lock():\n"
                        "Application object is not able to be locked. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_SAPL_LOCKING_FAILED, dw_addInfo);
        break;
      }

      /* SOD_Unlock */
      case SOD_k_ERR_INST_NUM_6:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_6: SOD_Unlock():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_INST_NUM_6, dw_addInfo);
        break;
      }
      case SOD_k_ERR_UNLOCK_WITHOUT_LOCK:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_UNLOCK_WITHOUT_LOCK: SOD_Unlock():\n"
                        "SOD_Unlock() was called without SOD_Lock(). "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_UNLOCK_WITHOUT_LOCK, dw_addInfo);
        break;
      }
      case SOD_k_ERR_SAPL_UNLOCK_FAILED:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_SAPL_UNLOCK_FAILED: SOD_Unlock():\n"
                        "Application object is not able to be unlocked. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_SAPL_UNLOCK_FAILED, dw_addInfo);
        break;
      }

      /* SOD_EnableSodWrite */
      case SOD_k_ERR_INST_NUM_7:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_7: SOD_EnableSodWrite():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_INST_NUM_7, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ENABLE_WR_FAILED:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ENABLE_WR_FAILED: "
                        "SOD_EnableSodWrite():\n"
                        "SOD_EnableSodWrite() was called without "
                        "SOD_DisableSodWrite(). "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_ENABLE_WR_FAILED, dw_addInfo);
        break;
      }

      /* SOD_DisableSodWrite */
      case SOD_k_ERR_INST_NUM_8:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_8: SOD_DisableSodWrite():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_INST_NUM_8, dw_addInfo);
        break;
      }
      case SOD_k_ERR_DISABLE_WR_FAILED:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_DISABLE_WR_FAILED: "
                        "SOD_DisableSodWrite():\n"
                        "SOD write access is already disabled. "
                        "SSDO abort code : %#lx\n",
                SOD_k_ERR_DISABLE_WR_FAILED, dw_addInfo);
        break;
      }


      /** SODcheck.c **/
      /* SOD_Check */
      case SOD_k_ERR_SAPL_INIT:
      {
        SPRINTF1(pac_str, "%#x - SOD_k_ERR_SAPL_INIT: SOD_Check():\n"
                        "The SAPL_SOD_DefaultValueSetClbk() application "
                        "callback function has signaled error.\n",
                SOD_k_ERR_SAPL_INIT);
        break;
      }
      case SOD_k_ERR_NO_RANGE_CHECKING:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_NO_RANGE_CHECKING: SOD_Check():\n"
                        "Range checking is not supported for the object "
                        "with SOD index (%#x) sub-index (%#x).\n",
                SOD_k_ERR_NO_RANGE_CHECKING,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }
      case SOD_k_ERR_NO_OBJECT_DATA:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_NO_OBJECT_DATA: SOD_Check():\n"
                        "No object data pointer has been defined "
                        "for the object with SOD index (%#x) sub-index(%#x).\n",
                SOD_k_ERR_NO_OBJECT_DATA,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }
      case SOD_k_ERR_NO_P2P_OBJECT_DATA:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_NO_P2P_OBJECT_DATA: "
                        "DataPointerCheck():\n"
                        "No pointer to pointer to object data has been "
                        "defined for the object with SOD index (%#x) "
                        "sub-index (%#x).\n",
                SOD_k_ERR_NO_P2P_OBJECT_DATA,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }
      case SOD_k_ERR_SOD_NOT_SORTED_1:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_SOD_NOT_SORTED_1: SOD_Check():\n"
                        "The object with SOD index (%#x) sub-index (%#x) "
                        "is not sorted by SOD index.\n",
                SOD_k_ERR_SOD_NOT_SORTED_1,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }
      case SOD_k_ERR_SOD_NOT_SORTED_2:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_SOD_NOT_SORTED_2: SOD_Check():\n"
                        "The object with SOD index (%#x) sub-index (%#x) "
                        "is not sorted by SOD sub-index.\n",
                SOD_k_ERR_SOD_NOT_SORTED_2,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }
      case SOD_k_ERR_SOD_ENTRY_DOUBLE:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_SOD_ENTRY_DOUBLE: SOD_Check():\n"
                        "The object with SOD index (%#x) sub-index (%#x) "
                        "exists in more than one SOD.\n",
                        SOD_k_ERR_SOD_ENTRY_DOUBLE,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }

      /* AttrCheck */
      case SOD_k_ERR_ATTR_IS_WRONG_1:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_ATTR_IS_WRONG_1: AttrCheck():\n"
                        "The attribute of the object with SOD index "
                        "(%#x) sub-index (%#x) is invalid.\n",
                SOD_k_ERR_ATTR_IS_WRONG_1,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }
      case SOD_k_ERR_ATTR_IS_WRONG_2:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_ATTR_IS_WRONG_2: AttrCheck():\n"
                        "No access flag (RO_CONST,RO,WO) in the attribute of "
                        "the object with SOD index (%#x) sub-index (%#x) "
                        "is set.\n",
                SOD_k_ERR_ATTR_IS_WRONG_2,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }
      case SOD_k_ERR_ATTR_IS_WRONG_3:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_ATTR_IS_WRONG_3: AttrCheck():\n"
                        "Access flags (RO_CONST,RO,WO) in the attribute of "
                        "the object with SOD index (%#x) sub-index (%#x) "
                        "is set.\n",
                SOD_k_ERR_ATTR_IS_WRONG_3,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }
      case SOD_k_ERR_NO_DEFAULT_VALUE:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_NO_DEFAULT_VALUE: AttrCheck():\n"
                        "No default value has been given for the object "
                        "with SOD index (%#x) sub-index (%#x).\n",
                SOD_k_ERR_NO_DEFAULT_VALUE,
                HIGH16(dw_addInfo), (UINT8)(dw_addInfo));
        break;
      }
      case SOD_k_ERR_OBJ_CLBK_1:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_OBJ_CLBK_1: AttrCheck():\n"
                        "Flag to call a callback function is set in the "
                        "attribute of the object with SOD index (%#x) "
                        "sub-index (%#x) but no callback function "
                        "pointer is given.\n",
                SOD_k_ERR_OBJ_CLBK_1,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }
      case SOD_k_ERR_OBJ_CLBK_2:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_OBJ_CLBK_2: AttrCheck():\n"
                        "Callback function pointer is given for the object "
                        "with SOD index (%#x) sub-index (%#x) but no flag "
                        "to call a callback function is set in the "
                        "attribute entry.\n",
                SOD_k_ERR_OBJ_CLBK_2,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }

      /* DataTypeCheck */
      case SOD_k_ERR_MAX_MIN_VALUE_ERROR:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_MAX_MIN_VALUE_ERROR: "
                        "DataTypeCheck():\n"
                        "The given range for the object with SOD index (%#x) "
                        "sub-index (%#x) is wrong, because the maximum value "
                        "is smaller the minimum value.\n",
                SOD_k_ERR_MAX_MIN_VALUE_ERROR,
                HIGH16(dw_addInfo),(UINT8)dw_addInfo);
        break;
      }
      case SOD_k_ERR_TYPE_LEN_1:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_TYPE_LEN_1: DataTypeCheck():\n"
                        "Data type is (UINT8 or INT8 or BOOLEAN) "
                        "of the object with SOD index (%#x) sub-index (%#x) "
                        "but the data length is not 1.\n",
                SOD_k_ERR_TYPE_LEN_1,
                HIGH16(dw_addInfo), (UINT8)(dw_addInfo));
        break;
      }
      case SOD_k_ERR_TYPE_LEN_2:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_TYPE_LEN_2: DataTypeCheck():\n"
                        "Data type is (UINT16 or INT16) "
                        "of the object with SOD index (%#x) sub-index (%#x) "
                        "but the data length is not 2.\n",
                SOD_k_ERR_TYPE_LEN_2,
                HIGH16(dw_addInfo), (UINT8)(dw_addInfo));
        break;
      }
      case SOD_k_ERR_TYPE_LEN_3:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_TYPE_LEN_3: DataTypeCheck():\n"
                        "Data type is (UINT32 or INT32 or REAL32) "
                        "of the object with SOD index (%#x) sub-index (%#x) "
                        "but the data length is not 4.\n",
                SOD_k_ERR_TYPE_LEN_3,
                HIGH16(dw_addInfo), (UINT8)(dw_addInfo));
        break;
      }
      case SOD_k_ERR_TYPE_LEN_4:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_TYPE_LEN_4: DataTypeCheck():\n"
                        "Data type is (DOMAIN or VISIBLE/OCTET STRING) "
                        "of the object with SOD index (%#x) sub-index (%#x) "
                        "but the maximum data length is 0.\n",
                SOD_k_ERR_TYPE_LEN_4,
                HIGH16(dw_addInfo), (UINT8)(dw_addInfo));
        break;
      }
      case SOD_k_ERR_TYPE_LEN_5:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_TYPE_LEN_5: DataTypeCheck():\n"
                        "Data type is (UINT64 or INT64 or REAL64) "
                        "of the object with SOD index (%#x) sub-index (%#x) "
                        "but the data length is not 8.\n",
                SOD_k_ERR_TYPE_LEN_5,
                HIGH16(dw_addInfo), (UINT8)(dw_addInfo));
        break;
      }
      case SOD_k_ERR_TYPE_NOT_SUPPORTED:
      {
        SPRINTF3(pac_str, "%#x - SOD_k_ERR_TYPE_NOT_SUPPORTED: "
                        "DataTypeCheck():\n"
                        "Data type of the object with SOD index (%#x) "
                        "sub-index (%#x) is not supported .\n",
                SOD_k_ERR_TYPE_NOT_SUPPORTED,
                HIGH16(dw_addInfo), (UINT8)(dw_addInfo));
        break;
      }

      /** SODvirt.c **/
      /* SOD_AttrGetVirt */
      case SOD_k_ERR_INST_NUM_11:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_11: SOD_AttrGetVirt():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_INST_NUM_11, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACS_OBJ_1:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACS_OBJ_1: SOD_AttrGetVirt():\n"
                        "Pointer to the SOD access object is NULL. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_ACS_OBJ_1, dw_addInfo);
        break;
      }
      case SOD_k_ERR_SIMU_FCT_1:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_SIMU_FCT_1: "
                        "SOD_AttrGetVirt():\n"
                        "Simulated SOD Callback was aborted. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_SIMU_FCT_1, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ATTR_GET_VIRT:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ATTR_GET_VIRT: SOD_AttrGetVirt():\n"
                        "The SOD object could not be found. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_ATTR_GET_VIRT, dw_addInfo);
        break;
      }
      /* SOD_AttrGetNextVirt */
      case SOD_k_ERR_INST_NUM_12:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_12: SOD_AttrGetNextVirt():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_INST_NUM_12, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACS_OBJ_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACS_OBJ_2: SOD_AttrGetNextVirt():\n"
                        "Pointer to the SOD access object is NULL. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_ACS_OBJ_2, dw_addInfo);
        break;
      }
      /** SOD_ReadVirt */
      case SOD_k_ERR_INST_NUM_13:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_13: SOD_ReadVirt():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_INST_NUM_13, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACS_OBJ_3:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACS_OBJ_3: SOD_ReadVirt():\n"
                        "Pointer to the SOD access object is NULL. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_ACS_OBJ_3, dw_addInfo);
        break;
      }
      case SOD_k_ERR_TYPE_OFFSET_3:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_TYPE_OFFSET_3: SOD_ReadVirt():\n"
                        "The object with standard data type "
                        "(UINT8/16/32, INT8/16/32, REAL32, BOOLEAN), "
                        "must not be accessed segmented "
                        "(offset and size must be 0). "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_TYPE_OFFSET_3, dw_addInfo);
        break;
      }
      case SOD_k_ERR_CLBK_ABORT_BR_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_CLBK_ABORT_BR_2: "
                        "SOD_ReadVirt():\n"
                        "Before Read SOD Callback was aborted. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_CLBK_ABORT_BR_2, dw_addInfo);
        break;
      }
      case SOD_k_ERR_OFFSET_SIZE_3:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_OFFSET_SIZE_3: SOD_ReadVirt():\n"
                        "The byte position is outside of the object "
                        "data range (offset or/and size are wrong). "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_OFFSET_SIZE_3, dw_addInfo);
        break;
      }
      /** SOD_WriteVirt */
      case SOD_k_ERR_INST_NUM_14:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_14: SOD_WriteVirt():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_INST_NUM_14, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACS_OBJ_4:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACS_OBJ_4: SOD_WriteVirt():\n"
                        "Pointer to the SOD access object is NULL. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_ACS_OBJ_4, dw_addInfo);
        break;
      }
      case SOD_k_ERR_DATA_POINTER_INVALID_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_DATA_POINTER_INVALID_2: SOD_WriteVirt():\n"
                        "Pointer to the data to be written is NULL. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_DATA_POINTER_INVALID_2, dw_addInfo);
        break;
      }
      case SOD_k_ERR_SOD_WRITE_DISABLED_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_SOD_WRITE_DISABLED_2: SOD_WriteVirt():\n"
                        "The SOD write access is disabled by the application. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_SOD_WRITE_DISABLED_2, dw_addInfo);
        break;
      }
      case SOD_k_ERR_TYPE_OFFSET_4:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_TYPE_OFFSET_4: SOD_WriteVirt():\n"
                        "The object with standard data type "
                        "(UINT8/16/32, INT8/16/32, REAL32, BOOLEAN), "
                        "must not be accessed segmented "
                        "(offset and size must be 0). "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_TYPE_OFFSET_4, dw_addInfo);
        break;
      }
      case SOD_k_ERR_CLBK_ABORT_BW_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_CLBK_ABORT_BW_2: "
                        "SOD_WriteVirt():\n"
                        "Simulated Before Write SOD Callback was aborted. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_CLBK_ABORT_BW_2, dw_addInfo);
        break;
      }
      case SOD_k_ERR_CLBK_ABORT_BW_3:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_CLBK_ABORT_BW_3: "
                        "SOD_WriteVirt():\n"
                        "Before Write SOD Callback was aborted. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_CLBK_ABORT_BW_3, dw_addInfo);
        break;
      }
      /** SOD_ActualLenSetVirt */
      case SOD_k_ERR_INST_NUM_15:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_15: SOD_ActualLenSetVirt():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_INST_NUM_15, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACS_OBJ_5:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACS_OBJ_5: SOD_ActualLenSetVirt():\n"
                        "Pointer to the SOD access object is NULL. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_ACS_OBJ_5, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACT_LEN_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACT_LEN_2: SOD_ActualLenSetVirt():\n"
                "Actual length to be set is larger than the maximum "
                "length of the object. "
                "SSDO abort code : %#lx\n",
                SOD_k_ERR_ACT_LEN_2, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACT_LEN_TYPE_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACT_LEN_TYPE_2: SOD_ActualLenSetVirt():\n"
                "Actual length is tried to set for an object "
                "with standard data type. Actual length can be set only "
                "for the objects with data type DOMAIN, OCTET STRING "
                "or VISIBLE STRING. "
                "SSDO abort code : %#lx\n",
                SOD_k_ERR_ACT_LEN_TYPE_2, dw_addInfo);
        break;
      }
      /** SOD_ActualLenGetVirt */
      case SOD_k_ERR_INST_NUM_16:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_INST_NUM_16: SOD_ActualLenGetVirt():\n"
                        "Instance number is invalid. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_INST_NUM_16, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACS_OBJ_6:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACS_OBJ_6: SOD_ActualLenGetVirt():\n"
                        "Pointer to the SOD access object is NULL. "
                        "SSDO abort code : %#lx\n",
                        SOD_k_ERR_ACS_OBJ_6, dw_addInfo);
        break;
      }
      case SOD_k_ERR_ACT_LEN_PTR_2:
      {
        SPRINTF2(pac_str, "%#x - SOD_k_ERR_ACT_LEN_PTR_2: SOD_ActualLenGetVirt():\n"
                "Pointer to the actual length is NULL. "
                "SSDO abort code : %#lx\n",
                SOD_k_ERR_ACT_LEN_PTR_2, dw_addInfo);
        break;
      }

      default:
      {
        SPRINTF(pac_str, "Unknown error code was generated by the SOD\n");
      }
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
    /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15
                                Notice #17 - Function comment content less than
                                             10.0%
                                Notice #18 - Function eLOC > maximum 200 eLOC */
  }
    /* RSM_IGNORE_QUALITY_END */
#endif

/***
*    static functions
***/
/**
* @brief This function sets the actual values to the default values in the SOD.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - TRUE            - success
* - FALSE           - failure
*/
static BOOLEAN SetDefaultValues(BYTE_B_INSTNUM)
{
  BOOLEAN o_ret = FALSE; /* return value */
  BOOLEAN o_break = FALSE; /* break flag */
  const SOD_t_OBJECT *ps_tmpSodAddr; /* temporary pointer for the
                                       object dictionary checking */
  void *pv_data; /* pointer to the actual value of the object */
  const UINT64 ddw_noDefVal=0UL; /* the object that has not default value
                                    is initialized with this constant */


  ps_tmpSodAddr = SOD_GetSodStartAddr(B_INSTNUM);

  /* for all objects in the object dictionary */
  while ((ps_tmpSodAddr->w_index != SOD_k_END_OF_THE_OD) && !o_break)
  {
    /* if there is no default value and
       there is no range checking */
    if ((ps_tmpSodAddr->s_attr.pv_defValue == NULL) &&
        (ps_tmpSodAddr->pv_range == NULL))
    {
      /* if Domain, Octet or Visible string */
      if ((ps_tmpSodAddr->s_attr.e_dataType == EPLS_k_DOMAIN) ||
          (ps_tmpSodAddr->s_attr.e_dataType == EPLS_k_OCTET_STRING) ||
          (ps_tmpSodAddr->s_attr.e_dataType == EPLS_k_VISIBLE_STRING))
      {
        /* destination pointer is set */
        pv_data =
            ((SOD_t_ACT_LEN_PTR_DATA *)(ps_tmpSodAddr->pv_objData))->pv_objData;

        /* if SOD_k_ATTR_P2P is set */
        if (EPLS_IS_BIT_SET(ps_tmpSodAddr->s_attr.w_attr,SOD_k_ATTR_P2P))
        {
          /* pointer to the data is solved */
          pv_data = (void *)(*((PTR_TYPE *)(pv_data))); /*lint !e923  Note 923:
                                               cast from unsigned long to
                                               pointer [MISRA 2004 Rule 11.3] */
        }
        /* no else : SOD_k_ATTR_P2P is not set */

        MEMSET(pv_data, 0, ps_tmpSodAddr->s_attr.dw_objLen);
      }
      /* other object type */
      else
      {
        /* actual value is set to 0 */
        SOD_DataCpy((void*)(&ddw_noDefVal), ps_tmpSodAddr, 0U, 0U);
      }
    }
    /* else if there is no default value and
       there is range checking */
    else if ((ps_tmpSodAddr->s_attr.pv_defValue == NULL) &&
             (ps_tmpSodAddr->pv_range != NULL))
    {
      /* if the default value 0 is not valid */
      if (!SOD_DataValid(B_INSTNUM_(void*)(&ddw_noDefVal), ps_tmpSodAddr, 0U, 0U))
      {
        /* while loop is left because of error */
        o_break = TRUE;
      }
      /* else the default value 0 is valid */
      else
      {
        /* actual value is set to 0 */
        SOD_DataCpy((void*)(&ddw_noDefVal), ps_tmpSodAddr, 0U, 0U);
      }
    }
    /* else there is default value */
    else
    {
      /* if the default value is not in range */
      if (!SOD_DataValid(B_INSTNUM_ ps_tmpSodAddr->s_attr.pv_defValue,
                     ps_tmpSodAddr, 0U, 0U))
      {
        /* while loop is left because of error */
        o_break = TRUE;
      }
      /* else the default value is in range */
      else
      {
        /* actual value is set to the default value */
        SOD_DataCpy(ps_tmpSodAddr->s_attr.pv_defValue, ps_tmpSodAddr, 0U, 0U);
      }
    }

    ps_tmpSodAddr++;
  }

  /* if all default values in the SOD has been set successfully */
  if (!o_break)
  {
    o_ret = TRUE;
  }
  /* no else : error happened */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

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
BOOLEAN SOD_ObjectReadOnly(const void *pv_data,
                              const SOD_t_OBJECT *ps_object,
                              BOOLEAN o_overwrite)
{
  BOOLEAN o_ret = FALSE; /* return value */

  /* if the attribute of the given object is read only and
        overwrite flag is FALSE*/
  if ((EPLS_IS_BIT_SET(ps_object->s_attr.w_attr,SOD_k_ATTR_RO)) &&
      (EPLS_IS_BIT_RESET(ps_object->s_attr.w_attr,SOD_k_ATTR_WO)) &&
      (!o_overwrite))
  {
    /* if data type is not EPLS_k_VISIBLE_STRING, EPLS_k_DOMAIN and
          EPLS_k_OCTET_STRING */
    if ((ps_object->s_attr.e_dataType != EPLS_k_VISIBLE_STRING) &&
        (ps_object->s_attr.e_dataType != EPLS_k_DOMAIN) &&
        (ps_object->s_attr.e_dataType != EPLS_k_OCTET_STRING))
    {
      /* if the actual data and the data to be written are equal */
      if (MEMCOMP(pv_data, ps_object->pv_objData,
                 ps_object->s_attr.dw_objLen) == MEMCMP_IDENT)
      {
        o_ret = TRUE;
      }
      /* no else : the actual data and the data to be written are not equal */
    }
    /* no else : The object type is not standard data type. */
  }
  /* no else : the attribute of the given object is not read only or
               overwrite flag is TRUE */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

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
BOOLEAN SOD_ObjectWriteable(BYTE_B_INSTNUM_ const SOD_t_OBJECT *ps_object,
                               BOOLEAN o_overwrite)
{
  BOOLEAN o_ret = FALSE; /* return value */

  /* if the object can be overwritten and is not constant */
  if (o_overwrite && EPLS_IS_BIT_RESET(ps_object->s_attr.w_attr,
      SOD_k_ATTR_RO_CONST))
  {
    o_ret = TRUE;
  }
  /* else the object can be overwritten by the application */
  else
  {
    /* if the object is writeable */
    if (EPLS_IS_BIT_SET(ps_object->s_attr.w_attr,SOD_k_ATTR_WO))
    {
      o_ret = TRUE;
    }
    /* else the object is not writeable */
    else
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_OBJ_NOT_WRITEABLE,
                    (UINT32)SOD_ABT_OBJ_NOT_WRITEABLE);
    }
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/**
* @brief This function checks whether the visible string is valid.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll() or SOD_Write()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        pv_data           pointer to the data to be checked (pointer not checked, only called with reference to variable in SetDefaultValues() or checked in SOD_Write()) valid range : <> NULL
*
* @param        dw_strLen    	  length of the visible string (not checked, any value allowed) valid range : UINT32
*
* @return
* - TRUE            - visible string is valid
* - FALSE           - visible string is invalid
*/
static BOOLEAN VisibleStringCheck(BYTE_B_INSTNUM_ const void *pv_data,
                                  UINT32 dw_strLen)
{
  BOOLEAN o_ret = TRUE; /* return value */
  UINT32 dw_charIdx = 0U; /* counter for the bytes of the visible string */

  /* while end of the visible string */
  while(dw_charIdx < dw_strLen)
  {
    /* if the corresponding char is not visible char */
    if ((*ADD_OFFSET(pv_data, dw_charIdx) != k_VISIBLE_STR_BYTE_VALID) &&
        ((*ADD_OFFSET(pv_data, dw_charIdx) < k_VISIBLE_STR_BYTE_LOW_LIMIT) ||
         (*ADD_OFFSET(pv_data, dw_charIdx) > k_VISIBLE_STR_BYTE_HIGH_LIMIT)))
    {
      /* the string is not visible string */
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_VISIBLE_STR_INVALID,
                    (UINT32)SOD_ABT_VALUE_RANGE_EXCEEDED);

      /* jump to the end of the visible string */
      dw_charIdx = dw_strLen;
      o_ret = FALSE;
    }
    /* else : the corresponding char is visible char */
    else
    {
      /* next char */
      dw_charIdx++;
    }
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

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
BOOLEAN SOD_DataValid(BYTE_B_INSTNUM_ const void *pv_data,
                         const SOD_t_OBJECT *ps_object, UINT32 dw_offset,
                         UINT32 dw_size)
{
  BOOLEAN o_ret = FALSE; /* return value */

  /* switch for the data type */
  switch (ps_object->s_attr.e_dataType)
  {
    case EPLS_k_BOOLEAN: /* BOOLEAN */
    {
      /* if BOOLEAN is in range */
      if ((*((BOOLEAN *)(pv_data)) == 1U) || (*((BOOLEAN *)(pv_data)) == 0U))
      {
        o_ret = TRUE;
      }
      /* else BOOLEAN is not in range */
      else
      {
        SERR_SetError(B_INSTNUM_ SOD_k_ERR_VALUE_IS_TOO_HIGH_2,
                      (UINT32)(SOD_ABT_VALUE_IS_TOO_HIGH));
      }
      break;
    }
    case EPLS_k_UINT8: /* UINT8 */
    {
      RANGE_CHECK(UINT8)
      break;
    }
    case EPLS_k_UINT16: /* UINT16 */
    {
      RANGE_CHECK(UINT16) /*lint !e826 error 826: (Info -- Suspicious
                            pointer-to-pointer conversion (area too small)) */
      break;
    }
    case EPLS_k_UINT32: /* UINT32 */
    {
      RANGE_CHECK(UINT32) /*lint !e826 error 826: (Info -- Suspicious
                            pointer-to-pointer conversion (area too small)) */
      break;
    }
    case EPLS_k_INT8: /* INT8 */
    {
      RANGE_CHECK(INT8)
      break;
    }
    case EPLS_k_INT16: /* INT16 */
    {
      RANGE_CHECK(INT16) /*lint !e826 error 826: (Info -- Suspicious
                            pointer-to-pointer conversion (area too small)) */
      break;
    }
    case EPLS_k_INT32: /* INT32 */
    {
      RANGE_CHECK(INT32) /*lint !e826 error 826: (Info -- Suspicious
                           pointer-to-pointer conversion (area too small)) */
      break;
    }
    case EPLS_k_REAL32: /* REAL32 */
    {
      RANGE_CHECK(REAL32) /*lint !e826 error 826: (Info -- Suspicious
                            pointer-to-pointer conversion (area too small)) */
      break;
    }
    #if (EPLS_cfg_MAX_PYLD_LEN >= 12)
      case EPLS_k_UINT64: /* UINT64 */
      {
        RANGE_CHECK(UINT64) /*lint !e826 error 826: (Info -- Suspicious
                              pointer-to-pointer conversion (area too small)) */
        break;
      }
      case EPLS_k_INT64: /* INT64 */
      {
        RANGE_CHECK(INT64) /*lint !e826 error 826: (Info -- Suspicious
                             pointer-to-pointer conversion (area too small)) */
        break;
      }
      case EPLS_k_REAL64: /* REAL64 */
      {
        RANGE_CHECK(REAL64) /*lint !e826 error 826: (Info -- Suspicious
                              pointer-to-pointer conversion (area too small)) */
        break;
      }
    #endif
    case EPLS_k_VISIBLE_STRING: /* VISIBLE STRING */
    {
      /* if not segmented access */
      if ((dw_offset == 0U) && (dw_size == 0U))
      {
        o_ret = VisibleStringCheck(B_INSTNUM_ pv_data,
                                   ps_object->s_attr.dw_objLen);
      }
      /* else segmented access */
      else
      {
        o_ret = VisibleStringCheck(B_INSTNUM_ pv_data, dw_size);
      }
      break;
    }
    case EPLS_k_DOMAIN: /* DOMIAN */
    case EPLS_k_OCTET_STRING: /* OCTET STRING */
    {
        /* no range checking */
        o_ret = TRUE;
        break;
    }
    default:
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_TYPE_UNKNOWN,
                    (UINT32)(SOD_ABT_LOCAL_CONTROL));
      break;
    }
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */



/**
* @brief This function calls the before read callback function.
*
* The callback function is called, if the SOD_k_ATTR_BEF_RD attribute is set and a reference to the callback function is defined.
*
* @param        b_instNum         instance number (not checked, checked in SOD_Read()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        ps_object         pointer to the SOD entry (pointer not checked, only called
*       with reference to struct in SOD_Read()) valid range : <> NULL
*
* @param        dw_offset         start offset in bytes of the segment within the data block
*       (not checked, checked in SOD_Read()) valid range : UINT32
*
* @param        dw_size           size in bytes of the segment (not checked, checked in
*       SOD_Read()) valid range : UINT32
*
* @retval          ps_errRes         error result, only relevant if the return value == FALSE, (not checked,
*       checked in SOD_Read()), valid range : <> NULL
*
* @return
* - TRUE            - callback is completed successful
* - FALSE           - callback is not completed successful, see ps_errRes
*/
static BOOLEAN CallBeforeReadClbk(BYTE_B_INSTNUM_ const SOD_t_OBJECT *ps_object,
                                  UINT32 dw_offset, UINT32 dw_size,
                                  SOD_t_ERROR_RESULT *ps_errRes)
{
  BOOLEAN o_ret = FALSE; /* return value */

  /* if the corresponding attribute is set */
  if (EPLS_IS_BIT_SET(ps_object->s_attr.w_attr, SOD_k_ATTR_BEF_RD))
  {
     /* the object callback is called with SOD_k_SRV_BEFORE_READ service*/
     o_ret = (ps_object->pf_clbk)(B_INSTNUM_ SOD_k_SRV_BEFORE_READ, ps_object,
                                  NULL, dw_offset, dw_size,
                                  &ps_errRes->e_abortCode);

    /* if the callback returned error */
    if (!o_ret)
    {
      ps_errRes->w_errorCode = SOD_k_ERR_CLBK_ABORT_BR;
    }
    /* no else : callback function finished successfully */
  }
  /* else the corresponding attribute is not set */
  else
  {
    o_ret = TRUE;
  }

  return o_ret;
}

/**
* @brief This function calls the before write callback function.
*
* The callback function is called, if the SOD_k_ATTR_BEF_WR attribute is set and a reference to the callback function is defined.
*
* @param        b_instNum         instance number (not checked, checked in SOD_Write()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        pv_data           pointer to the data to be checked (pointer not checked,
*       checked in SOD_Write()) valid range : <> NULL
*
* @param        ps_object         pointer to the SOD entry (pointer not checked, only called
*       with reference to struct in SOD_Write()) valid range : <> NULL
*
* @param        dw_offset         start offset in bytes of the segment within the data block (not
*       checked, checked in SOD_Write()) valid range : UINT32
*
* @param        dw_size 		  size in bytes of the segment (not checked, checked in SOD_Write()) valid range : UINT32
*
* @return
* - TRUE            - callback is completed successful
* - FALSE           - callback is not completed successful
*/
static BOOLEAN CallBeforeWriteClbk(BYTE_B_INSTNUM_ const void *pv_data,
                                   const SOD_t_OBJECT *ps_object,
                                   UINT32 dw_offset, UINT32 dw_size)
{
  BOOLEAN o_ret = FALSE; /* return value */
  SOD_t_ABORT_CODES e_abortCode; /* abort code */

  /* if the corresponding attribute is set */
  if (EPLS_IS_BIT_SET(ps_object->s_attr.w_attr, SOD_k_ATTR_BEF_WR))
  {
    /* the object callback is called with SOD_k_SRV_BEFORE_WRITE service */
    o_ret = (ps_object->pf_clbk)(B_INSTNUM_ SOD_k_SRV_BEFORE_WRITE, ps_object,
                                 pv_data, dw_offset, dw_size, &e_abortCode);

    /* if the callback returned error */
    if (!o_ret)
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_CLBK_ABORT_BW, (UINT32)e_abortCode);
    }
    /* no else : callback function finished successfully */
  }
  /* else the corresponding attribute is not set */
  else
  {
    o_ret = TRUE;
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/**
* @brief This function calls the after write callback function.
*
* The callback function is called, if the SOD_k_ATTR_AFT_WR attribute is set and a reference to the callback function is defined.
*
* @param        b_instNum         instance number (not checked, checked in SOD_Write()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        ps_object         pointer to the SOD entry (pointer not checked, only called with reference to struct in SOD_Write()) valid range : <> NULL
*
* @param        dw_offset 	  start offset in bytes of the segment within the data block (not checked, checked in SOD_Write()) valid range : UINT32
*
* @param        dw_size           size in bytes of the segment (not checked, checked in SOD_Write()) valid range : UINT32
*
* @return
* - TRUE            - callback is completed successful
* - FALSE           - callback is not completed successful
*/
BOOLEAN SOD_CallAfterWriteClbk(BYTE_B_INSTNUM_
                                  const SOD_t_OBJECT *ps_object,
                                  UINT32 dw_offset, UINT32 dw_size)
{
  BOOLEAN o_ret = FALSE; /* return value */
  SOD_t_ABORT_CODES e_abortCode;
  UINT32 dw_actLen; /* actual length */

  /* if the corresponding attribute is set */
  if (EPLS_IS_BIT_SET(ps_object->s_attr.w_attr, SOD_k_ATTR_AFT_WR))
  {
    /* if the object data type is DOMAIN or VISIBLE STRING or OCTET STRING */
    if ((ps_object->s_attr.e_dataType == EPLS_k_VISIBLE_STRING) ||
        (ps_object->s_attr.e_dataType == EPLS_k_OCTET_STRING) ||
        (ps_object->s_attr.e_dataType == EPLS_k_DOMAIN))
    {
      dw_actLen =
          ((SOD_t_ACT_LEN_PTR_DATA *)(ps_object->pv_objData))->dw_actLen;
    }
    else /* other (standard) data type */
    {
      dw_actLen = ps_object->s_attr.dw_objLen;
    }

    /* if the access is not segmented or the last segment was written
        (offset + segment length == object length) */
    if (((dw_offset == 0U) && (dw_size == 0U)) ||
        ((dw_offset+dw_size) == dw_actLen))
    {
      /* the object callback is called with SOD_k_SRV_AFTER_WRITE service */
      o_ret = (ps_object->pf_clbk)(B_INSTNUM_ SOD_k_SRV_AFTER_WRITE, ps_object,
                                   NULL, dw_offset, dw_size, &e_abortCode);

      /* if the callback returned error */
      if (!o_ret)
      {
        SERR_SetError(B_INSTNUM_ SOD_k_ERR_CLBK_ABORT_AW, (UINT32)e_abortCode);
      }
      /* no else : callback function finished successfully */
    }
    /* else not the last segment was written */
    else
    {
      o_ret = TRUE;
    }
  }
  /* else the corresponding attribute is not set */
  else
  {
    o_ret = TRUE;
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/**
* @brief This function copies the data segment into the OD.
*
* @param        pv_data           pointer to the data to be copied (pointer not checked, only called with reference to variable in SetDefaultValues() or checked in SOD_Write()) valid range : <> NULL
*
* @param        ps_object         pointer to the SOD entry (pointer not checked, only called with reference to struct in SetDefaultValues() or SOD_Write()) valid range : <> NULL
*
* @param        dw_offset         start offset in bytes of the segment within the data block (not checked, only called with 0 or checked in SOD_Write()) valid range : UINT32
*
* @param        dw_size 	  size in bytes of the segment (not checked, only called with 0 or checked in SOD_Write()) valid range : UINT32
*/
void SOD_DataCpy(const void *pv_data, const SOD_t_OBJECT *ps_object,
                    UINT32 dw_offset, UINT32 dw_size)
{
  void *pv_dst; /* temporary destination pointer to increase the readablity */
  void *pv_objData; /* pointer to the object data */

  pv_objData = ps_object->pv_objData;

  /* if the object data type is DOMAIN or VISIBLE STRING or OCTET STRING */
  if ((ps_object->s_attr.e_dataType == EPLS_k_VISIBLE_STRING) ||
      (ps_object->s_attr.e_dataType == EPLS_k_OCTET_STRING) ||
      (ps_object->s_attr.e_dataType == EPLS_k_DOMAIN))
  {
    pv_objData = ((SOD_t_ACT_LEN_PTR_DATA *)(pv_objData))->pv_objData;
  }
  /* no else : other (standard) data type */

  /* if SOD_k_ATTR_P2P is set */
  if (EPLS_IS_BIT_SET(ps_object->s_attr.w_attr, SOD_k_ATTR_P2P))
  {
    /* pointer to the data is solved */
    pv_dst = (void *)(*((PTR_TYPE *)(pv_objData))); /*lint !e923  Note 923: cast
                                                   from unsigned long to pointer
                                                   [MISRA 2004 Rule 11.3] */
  }
  /* else SOD_k_ATTR_P2P is not set */
  else
  {
    /* destination pointer is set */
    pv_dst = pv_objData;
  }

  /* if the whole object is copied */
  if ((dw_size == 0U) && (dw_offset == 0U))
  {
    /* the whole data is copied into the object dictionary */
    MEMCOPY(pv_dst, pv_data, ps_object->s_attr.dw_objLen);
  }
  /* else a segment of the object is copied */
  else
  {
    /* the data segment is copied into the object dictionary */
    MEMCOPY((void *)(ADD_OFFSET(pv_dst, dw_offset)), pv_data, dw_size);
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}
/** @} */
