/**
 * @addtogroup SOD
 * @{
 *
 * @file SODcheck.c
 *
 * This file checks the Object Dictionaries.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SODcheck.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
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
#include "SODint.h"


/**
 * @def k_ATTR_INVALID
 * This symbol is used to check the invalid attributes
 *
 * The following combinations are invalid, depending on the number
 * of instances defined:
 * - EPLS_cfg_MAX_INSTANCES &lt;= 1:
 *      - SOD_k_ATTR_P2P
 *      - SOD_k_ATTR_AFT_WR
 *      - SOD_k_ATTR_BEF_WR
 *      - SOD_k_ATTR_BEF_RD
 *      - SOD_k_ATTR_PDO_MAP
 *      - SOD_k_ATTR_RO_CONST
 *      - SOD_k_ATTR_RO
 *      - SOD_k_ATTR_WO
 *      - SOD_k_ATTR_NO_LEN_CHK
 *      - SOD_k_ATTR_CRC
 * - EPLS_cfg_MAX_INSTANCES &gt; 1:
 *      - SOD_k_ATTR_P2P
 *      - SOD_k_ATTR_AFT_WR
 *      - SOD_k_ATTR_BEF_WR
 *      - SOD_k_ATTR_BEF_RD
 *      - SOD_k_ATTR_PDO_MAP
 *      - SOD_k_ATTR_RO_CONST
 *      - SOD_k_ATTR_RO
 *      - SOD_k_ATTR_WO
 *      - SOD_k_ATTR_CRC
 *      - SOD_k_ATTR_NO_LEN_CHK
 *      - SOD_k_ATTR_SHARED
 *
*/
#if (EPLS_cfg_MAX_INSTANCES > 1)
  #define k_ATTR_INVALID ((UINT16)(~(UINT16)(SOD_k_ATTR_P2P|\
                                             SOD_k_ATTR_AFT_WR|\
                                             SOD_k_ATTR_BEF_WR|\
                                             SOD_k_ATTR_BEF_RD|\
                                             SOD_k_ATTR_PDO_MAP|\
                                             SOD_k_ATTR_RO_CONST|\
                                             SOD_k_ATTR_RO|\
                                             SOD_k_ATTR_WO|\
                                             SOD_k_ATTR_CRC|\
                                             SOD_k_ATTR_NO_LEN_CHK|\
                                             SOD_k_ATTR_SHARED|\
                                             SOD_k_ATTR_ARRAY|\
                                             SOD_k_ATTR_SIMU)))
#else
  #define k_ATTR_INVALID ((UINT16)(~(UINT16)(SOD_k_ATTR_P2P|\
                                             SOD_k_ATTR_AFT_WR|\
                                             SOD_k_ATTR_BEF_WR|\
                                             SOD_k_ATTR_BEF_RD|\
                                             SOD_k_ATTR_PDO_MAP|\
                                             SOD_k_ATTR_RO_CONST|\
                                             SOD_k_ATTR_RO|\
                                             SOD_k_ATTR_WO|\
                                             SOD_k_ATTR_NO_LEN_CHK|\
                                             SOD_k_ATTR_CRC|\
                                             SOD_k_ATTR_SIMU)))

#endif

/**
 * This define is used to check whether the object attribute is valid.
*/
#define k_NO_ATTR 0U

/**
 * Macro for the checking of the range.
*/
#define CHECKING_OF_THE_RANGE(type) \
  {\
    /* if maximum value is less than minimum value */\
    if ( ((type *)(ps_obj->pv_range))[k_RANGE_HIGH_LIMIT] < \
         ((type *)(ps_obj->pv_range))[k_RANGE_LOW_LIMIT] )\
    {\
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_MAX_MIN_VALUE_ERROR,\
                    ((UINT32)(ps_obj->w_index)<<16)+\
                    ps_obj->b_subIndex);\
    }\
    /* else range is OK */\
    else\
    {\
      o_ret = TRUE;\
    }\
  }

static BOOLEAN AttrCheck(BYTE_B_INSTNUM_ const SOD_t_OBJECT *ps_obj);
static BOOLEAN DataTypeCheck(BYTE_B_INSTNUM_ const SOD_t_OBJECT *ps_obj);
static BOOLEAN DataPointerCheck(BYTE_B_INSTNUM_ const SOD_t_OBJECT *ps_obj);

/**
* @brief This function checks the openSAFETY Object Dictionary (SOD), calculates and sets the length of the SOD.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - TRUE            - success
* - FALSE           - failure
*/
BOOLEAN SOD_Check(BYTE_B_INSTNUM)
{
  BOOLEAN o_ret = FALSE; /* return value */
  BOOLEAN o_break = FALSE; /* break flag */
  BOOLEAN o_first = TRUE; /* first object entry */
  const SOD_t_OBJECT *ps_tmpSodAddr; /* temporary pointer for the
                                       object dictionary checking */
  const SOD_t_OBJECT *ps_tmpSodAddrObj; /* temporary pointer for the
                                       object dictionary checking */
  SOD_t_ACS_OBJECT_VIRT s_objAcs;
  const SOD_t_ATTR  *ps_sodVirtAttr;
  SOD_t_ERROR_RESULT  e_errorResult;
  UINT16 w_lastIndex = 0; /* temporary variable to store SOD index to be checked */
  UINT8 b_lastSubindex = 0; /* temp. variable to store SOD sub-index to be checked */

  ps_tmpSodAddrObj = SOD_GetSodStartAddr(B_INSTNUM);
  ps_sodVirtAttr = SOD_AttrGetNextVirt(B_INSTNUM_ TRUE,&s_objAcs,&e_errorResult);


  /* for all objects in the object dictionary */
  while (((SOD_k_END_OF_THE_OD != ps_tmpSodAddrObj->w_index) ||
          (NULL != ps_sodVirtAttr)) &&
          !o_break)
  {
    /* end of static SOD reached */

    if ( SOD_k_END_OF_THE_OD == ps_tmpSodAddrObj->w_index)
    {
      ps_tmpSodAddr = &s_objAcs.s_obj;
    }
    /* end of virtual SOD reached */
    else if ( NULL == ps_sodVirtAttr)
    {
      ps_tmpSodAddr = ps_tmpSodAddrObj;
    }
    /* both SODs available */
    /* static SOD does have the lower object index */
    else if ((s_objAcs.s_obj.w_index > ps_tmpSodAddrObj->w_index) ||
             ((s_objAcs.s_obj.w_index == ps_tmpSodAddrObj->w_index) &&
              (s_objAcs.s_obj.b_subIndex > ps_tmpSodAddrObj->b_subIndex)))
    {
      ps_tmpSodAddr = ps_tmpSodAddrObj;
    }
    /* virtual SOD does have the lower object index */
    else
    {
      ps_tmpSodAddr = &s_objAcs.s_obj;
    }
    /* check for equal objects in both SODs */
    if ((SOD_k_END_OF_THE_OD != ps_tmpSodAddr->w_index) &&
        (NULL != ps_sodVirtAttr) &&
        (s_objAcs.s_obj.w_index == ps_tmpSodAddrObj->w_index) &&
        (s_objAcs.s_obj.b_subIndex == ps_tmpSodAddrObj->b_subIndex))
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_SOD_ENTRY_DOUBLE,
                    ((UINT32)(ps_tmpSodAddr->w_index)<<16)+
                    ps_tmpSodAddr->b_subIndex);
      /* while loop will be left because of an error */
      o_break = TRUE;
    }
    /* if the data type check failed */
    if (!DataTypeCheck(B_INSTNUM_ ps_tmpSodAddr))
    {
      /* while loop will be left because of an error */
      o_break = TRUE;
    }
    /* else if object type is the following and range pointer must be NULL */
    else if (((ps_tmpSodAddr->s_attr.e_dataType == EPLS_k_BOOLEAN) ||
              (ps_tmpSodAddr->s_attr.e_dataType == EPLS_k_VISIBLE_STRING) ||
              (ps_tmpSodAddr->s_attr.e_dataType == EPLS_k_OCTET_STRING) ||
              (ps_tmpSodAddr->s_attr.e_dataType == EPLS_k_DOMAIN)) &&
              (ps_tmpSodAddr->pv_range != NULL))
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_NO_RANGE_CHECKING,
                    ((UINT32)(ps_tmpSodAddr->w_index)<<16)+
                    ps_tmpSodAddr->b_subIndex);
      /* while loop will be left because of an error */
      o_break = TRUE;
    }
    /* else if attribute check failed */
    else if (!AttrCheck(B_INSTNUM_ ps_tmpSodAddr))
    {
      /* while loop will be left because of an error */
      o_break = TRUE;
    }
    /* else if the pointer to the object data is invalid */
    else if (ps_tmpSodAddr->pv_objData == NULL)
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_NO_OBJECT_DATA,
                    ((UINT32)(ps_tmpSodAddr->w_index)<<16)+
                    ps_tmpSodAddr->b_subIndex);
      /* while loop will be left because of an error */
      o_break = TRUE;
    }
    /* else if SOD_k_ATTR_P2P is set and the pointer to the address is
       invalid */
    else if (!DataPointerCheck(B_INSTNUM_ ps_tmpSodAddr))
    {

      /* while loop will be left because of an error */
      o_break = TRUE;
    }
    /* else no error */
    else
    {
      if (o_first)
      {
        o_first = FALSE;
      }
      else
      {
        /* objects are in the wrong order */
        if (w_lastIndex > ps_tmpSodAddr->w_index)
        {
          SERR_SetError(B_INSTNUM_ SOD_k_ERR_SOD_NOT_SORTED_1,
                        ((UINT32)(ps_tmpSodAddr->w_index)<<16)+
                        ps_tmpSodAddr->b_subIndex);
          o_break = TRUE;
        }
        else if ((w_lastIndex == ps_tmpSodAddr->w_index) &&
                 (b_lastSubindex >= ps_tmpSodAddr->b_subIndex))
        {
          SERR_SetError(B_INSTNUM_ SOD_k_ERR_SOD_NOT_SORTED_2,
                        ((UINT32)(ps_tmpSodAddr->w_index)<<16)+
                        ps_tmpSodAddr->b_subIndex);
          o_break = TRUE;
        }
      }
      w_lastIndex = ps_tmpSodAddr->w_index;
      b_lastSubindex = ps_tmpSodAddr->b_subIndex;

      /* step to the next object in the object dictionary */
      if (ps_tmpSodAddr == ps_tmpSodAddrObj)
      {
        ps_tmpSodAddrObj++;
      }
      else
      {
        ps_sodVirtAttr = SOD_AttrGetNextVirt(B_INSTNUM_ FALSE,&s_objAcs,&e_errorResult);
      }
    }
  }

  /* if the end of the SOD is reached and no error has been found */
  if ((ps_tmpSodAddrObj->w_index == SOD_k_END_OF_THE_OD) &&
      (NULL == ps_sodVirtAttr) &&
      (!o_break))
  {
    /* SOD is OK */
    o_ret = TRUE;
  }
  /* no else : an error has been found in the SOD */

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */

/***
*    static functions
***/
/**
* @brief This function checks the attribute of an object.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param        ps_obj            pointer to the object (pointer not checked, only called with reference to struct)
*
* @return
* - TRUE            - success
* - FALSE           - failure
*/
static BOOLEAN AttrCheck(BYTE_B_INSTNUM_ const SOD_t_OBJECT *ps_obj)
{
  BOOLEAN o_ret = FALSE; /* return value */

  /* if attribute is invalid */
  if ((ps_obj->s_attr.w_attr == k_NO_ATTR) ||
      ((UINT16)(ps_obj->s_attr.w_attr & k_ATTR_INVALID) != k_NO_ATTR))
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_ATTR_IS_WRONG_1,
                  ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
  }
  /* else if RO_CONST and RO and WO is not set */
  else if (EPLS_IS_BIT_RESET(ps_obj->s_attr.w_attr, SOD_k_ATTR_RO_CONST) &&
          (EPLS_IS_BIT_RESET(ps_obj->s_attr.w_attr, SOD_k_ATTR_RO) &&
           EPLS_IS_BIT_RESET(ps_obj->s_attr.w_attr, SOD_k_ATTR_WO)))
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_ATTR_IS_WRONG_2,
                  ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
  }
  /* else if attribute is RO_CONST and (RO or WO) is set */
  else if (EPLS_IS_BIT_SET(ps_obj->s_attr.w_attr, SOD_k_ATTR_RO_CONST) &&
          (EPLS_IS_BIT_SET(ps_obj->s_attr.w_attr, SOD_k_ATTR_RO) ||
           EPLS_IS_BIT_SET(ps_obj->s_attr.w_attr, SOD_k_ATTR_WO)))
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_ATTR_IS_WRONG_3,
                  ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
  }
  /* else if attribute is PDO_MAP and WO is set and
     there is no default value (Rx SPDO must have a default value)*/
  else if ((EPLS_IS_BIT_SET(ps_obj->s_attr.w_attr, SOD_k_ATTR_PDO_MAP)) &&
           (EPLS_IS_BIT_SET(ps_obj->s_attr.w_attr, SOD_k_ATTR_WO)) &&
           (ps_obj->s_attr.pv_defValue == NULL))
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_NO_DEFAULT_VALUE,
                  ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
  }
  /* else if SOD_k_ATTR_BEF_RD or SOD_k_ATTR_BEF_WR or SOD_k_ATTR_AFT_WR
     are set and there is no object callback */
  else if (EPLS_IS_BIT_SET(ps_obj->s_attr.w_attr,
           (SOD_k_ATTR_BEF_RD|SOD_k_ATTR_BEF_WR|SOD_k_ATTR_AFT_WR)) &&
           (ps_obj->pf_clbk == SOD_k_NO_CALLBACK))
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_OBJ_CLBK_1,
                  ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
  }
  /* else if SOD_k_ATTR_BEF_RD and SOD_k_ATTR_BEF_WR and SOD_k_ATTR_AFT_WR
      are not set and there is object callback*/
  else if (EPLS_IS_BIT_RESET(ps_obj->s_attr.w_attr,SOD_k_ATTR_BEF_RD) &&
           EPLS_IS_BIT_RESET(ps_obj->s_attr.w_attr,SOD_k_ATTR_BEF_WR) &&
           EPLS_IS_BIT_RESET(ps_obj->s_attr.w_attr,SOD_k_ATTR_AFT_WR) &&
           EPLS_IS_BIT_RESET(ps_obj->s_attr.w_attr,SOD_k_ATTR_SIMU) &&
           (ps_obj->pf_clbk != SOD_k_NO_CALLBACK))
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_OBJ_CLBK_2,
                  ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
  }
  /* else attribute is ok */
  else
  {
    o_ret = TRUE;
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */


/**
* @brief This function checks the data type of an object.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param        ps_obj            pointer to the object (pointer not checked, only called with reference to struct)
*
* @return
* - TRUE            - success
* - FALSE           - failure
*/
static BOOLEAN DataTypeCheck(BYTE_B_INSTNUM_ const SOD_t_OBJECT *ps_obj)
{
  BOOLEAN o_ret = FALSE; /* return value */

  /* if object type : EPLS_k_BOOLEAN,EPLS_k_INT8,EPLS_k_UINT8 */
  if ((ps_obj->s_attr.e_dataType == EPLS_k_BOOLEAN) ||
      (ps_obj->s_attr.e_dataType == EPLS_k_INT8) ||
      (ps_obj->s_attr.e_dataType == EPLS_k_UINT8))
  {
    /* if the object length is not OK */
    if (ps_obj->s_attr.dw_objLen != 1U)
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_TYPE_LEN_1,
                    ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
    }
    /* else the object length is OK */
    else
    {
      /* if range is given */
      if (ps_obj->pv_range != NULL)
      {
        /* if object type : EPLS_k_INT8 */
        if (ps_obj->s_attr.e_dataType == EPLS_k_INT8)
        {
          CHECKING_OF_THE_RANGE(INT8)
        }
        /* else if object type : EPLS_k_UINT8 */
        else if (ps_obj->s_attr.e_dataType == EPLS_k_UINT8)
        {
          CHECKING_OF_THE_RANGE(UINT8)
        }
        /* else object type : EPLS_k_BOOLEAN */
        else
        {
          /* range checking is always accomplished for Boolean */
          o_ret = TRUE;
        }
      }
      /* else no range is given */
      else
      {
        o_ret = TRUE;
      }
    }
  }
  /* else if object type : EPLS_k_INT16,EPLS_k_UINT16 */
  else if ((ps_obj->s_attr.e_dataType == EPLS_k_INT16) ||
           (ps_obj->s_attr.e_dataType == EPLS_k_UINT16))
  {
    if (ps_obj->s_attr.dw_objLen != 2U)
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_TYPE_LEN_2,
                    ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
    }
    else
    {
      /* if range is given */
      if (ps_obj->pv_range != NULL)
      {
        if (ps_obj->s_attr.e_dataType == EPLS_k_INT16)
        {
          CHECKING_OF_THE_RANGE(INT16)
        }
        else /* (ps_obj->s_attr.e_dataType == EPLS_k_UINT16) */
        {
          CHECKING_OF_THE_RANGE(UINT16)
        }
      }
      /* else no range is given */
      else
      {
        o_ret = TRUE;
      }
    }
  }
  /* else if object type : EPLS_k_INT32,EPLS_k_UINT32,EPLS_k_REAL32 */
  else if ((ps_obj->s_attr.e_dataType == EPLS_k_INT32) ||
           (ps_obj->s_attr.e_dataType == EPLS_k_UINT32) ||
           (ps_obj->s_attr.e_dataType == EPLS_k_REAL32))
  {
    /* if the object length is not OK */
    if (ps_obj->s_attr.dw_objLen != 4U)
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_TYPE_LEN_3,
                    ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
    }
    /* else the object length is OK */
    else
    {
      /* if range is given */
      if (ps_obj->pv_range != NULL)
      {
        /* if the data type EPLS_k_INT32 */
        if (ps_obj->s_attr.e_dataType == EPLS_k_INT32)
        {
          CHECKING_OF_THE_RANGE(INT32)
        }
        /* else if the data type EPLS_k_UINT32 */
        else if (ps_obj->s_attr.e_dataType == EPLS_k_UINT32)
        {
          CHECKING_OF_THE_RANGE(UINT32)
        }
        /* else the data type EPLS_k_REAL32 */
        else
        {
          CHECKING_OF_THE_RANGE(REAL32)
        }
      }
      /* else no range is given */
      else
      {
        o_ret = TRUE;
      }
    }
  }
  #if (EPLS_cfg_MAX_PYLD_LEN >= 12)
    /* else if object type : EPLS_k_INT64,EPLS_k_UINT64,EPLS_k_REAL64 */
    else if ((ps_obj->s_attr.e_dataType == EPLS_k_INT64) ||
            (ps_obj->s_attr.e_dataType == EPLS_k_UINT64) ||
            (ps_obj->s_attr.e_dataType == EPLS_k_REAL64))
    {
      /* if the object length is not OK */
      if (ps_obj->s_attr.dw_objLen != 8U)
      {
        SERR_SetError(B_INSTNUM_ SOD_k_ERR_TYPE_LEN_4,
                      ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
      }
      /* else the object length is OK */
      else
      {
        /* if range is given */
        if (ps_obj->pv_range != NULL)
        {
          /* if the data type EPLS_k_INT64 */
          if (ps_obj->s_attr.e_dataType == EPLS_k_INT64)
          {
            CHECKING_OF_THE_RANGE(INT64)
          }
          /* else if the data type EPLS_k_UINT64 */
          else if (ps_obj->s_attr.e_dataType == EPLS_k_UINT64)
          {
            CHECKING_OF_THE_RANGE(UINT64)
          }
          /* else the data type EPLS_k_REAL64 */
          else
          {
            CHECKING_OF_THE_RANGE(REAL64)
          }
        }
        /* else no range is given */
        else
        {
          o_ret = TRUE;
        }
      }
    }
  #endif
    /* else if object type : EPLS_k_VISIBLE_STRING,EPLS_k_DOMAIN,
       EPLS_k_OCTET_STRING */
    else if ((ps_obj->s_attr.e_dataType == EPLS_k_VISIBLE_STRING) ||
            (ps_obj->s_attr.e_dataType == EPLS_k_DOMAIN) ||
            (ps_obj->s_attr.e_dataType == EPLS_k_OCTET_STRING))
    {
      /* if the object length is not OK */
      if (ps_obj->s_attr.dw_objLen == 0U)
      {
        SERR_SetError(B_INSTNUM_ SOD_k_ERR_TYPE_LEN_5,
                      ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
      }
      /* else the object length is OK */
      else
      {
        o_ret = TRUE;
      }
    }
    /* else other data type */
    else
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_TYPE_NOT_SUPPORTED,
                    ((UINT32)(ps_obj->w_index)<<16)+ps_obj->b_subIndex);
    }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15 */
}
  /* RSM_IGNORE_QUALITY_END */

/**
* @brief This function checks the pointer to the data if the attribute pointer-to-pointer is set.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
* @param        ps_obj            pointer to the object (pointer not checked, only called with reference to struct)
*
* @return
* - TRUE            - success
* - FALSE           - failure
*/
static BOOLEAN DataPointerCheck(BYTE_B_INSTNUM_ const SOD_t_OBJECT *ps_obj)
{
  BOOLEAN o_ret = FALSE; /* return value */

  /* if pointer-to-pointer attribute is set */
  if (EPLS_IS_BIT_SET(ps_obj->s_attr.w_attr, SOD_k_ATTR_P2P))
  {
    /* if the data pointer point to the actual length structure */
    if ((ps_obj->s_attr.e_dataType == EPLS_k_VISIBLE_STRING) ||
        (ps_obj->s_attr.e_dataType == EPLS_k_OCTET_STRING) ||
        (ps_obj->s_attr.e_dataType == EPLS_k_DOMAIN))
    {
      /* if the pointer in the SOD_t_ACT_LEN_PTR_DATA is not NULL */
      if (((SOD_t_ACT_LEN_PTR_DATA *)(ps_obj->pv_objData))->pv_objData != NULL)
      {
        /* if the pointer to the data is not NULL */
        if ((void *)(*((PTR_TYPE *)(((SOD_t_ACT_LEN_PTR_DATA *)
            (ps_obj->pv_objData))->pv_objData))) != NULL) /*lint !e923
                                  Note 923: cast from pointer to unsigned long
                                  [MISRA 2004 Rule 11.3] */

        {
          o_ret = TRUE;
        }
        /* no else : error is generated at the end of the function */
      }
    }
    else /* the data pointer point to the data pointer */
    {
      if ((void *)(*((PTR_TYPE *)(ps_obj->pv_objData))) != NULL) /*lint !e923
                                  Note 923: cast from pointer to unsigned long
                                  [MISRA 2004 Rule 11.3] */
      {
        o_ret = TRUE;
      }
      /* no else : error is generated at the end of the function */
    }
  }
  else /* pointer-to-pointer attribute is not set */
  {
    /* if the data pointer point to the actual length structure */
    if ((ps_obj->s_attr.e_dataType == EPLS_k_VISIBLE_STRING) ||
        (ps_obj->s_attr.e_dataType == EPLS_k_OCTET_STRING) ||
        (ps_obj->s_attr.e_dataType == EPLS_k_DOMAIN))
    {
      if (((SOD_t_ACT_LEN_PTR_DATA *)(ps_obj->pv_objData))->pv_objData != NULL)
      {
        o_ret = TRUE;
      }
    }
    else /* the data pointer point to the data */
    {
      o_ret = TRUE;
    }
  }

  /* if error happened */
  if (!o_ret)
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_NO_P2P_OBJECT_DATA,
                  ((UINT32)(ps_obj->w_index)<<16)+
                  ps_obj->b_subIndex);
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}


/** @} */
