/**
 * @addtogroup SOD
 * @{
 *
 * @file SODvirt.c
 *
 * This file manages the access to Object Dictionaries.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author H. Pill, LARsys Automation  GmbH
 *
 *  * <h2>History for SODmain.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>17.02.2017</td><td>Stefan Innerhofer</td><td>optimized SOD_SetDefaultValuesVirt()</td></tr>
 * </table>
 *
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
 * Object structure for searching the SOD.
 */
typedef struct
{
  /** start address of the object dictionary */
  SOD_t_ENTRY_VIRT *ps_startSodAddr;
  /** end address of the object dictionary */
  SOD_t_ENTRY_VIRT *ps_endSodAddr;
  /** actual address in the object dictionary */
  SOD_t_ENTRY_VIRT *ps_actSodAddr;
  /** number of entries in the object  dictionary */
  UINT32 dw_noSodEntries;
} t_SOD_VIRT_ATTR_OBJ;

/**
 * Variable containing the object structures for each instance.
 */
t_SOD_VIRT_ATTR_OBJ as_SodVirt[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;
/**
 * @var SAPL_ps_OD
 *  reference to the object dictionaries
 */
extern SOD_t_ENTRY_VIRT const * const SAPL_ps_SOD_VIRT[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * Static function prototypes.
 */
static BOOLEAN const SOD_EntryMatch(SOD_t_ENTRY_VIRT const * const ps_entry,  UINT16 const w_index,
    UINT8 const b_subIndex);

static SOD_t_ENTRY_VIRT const * const SOD_Search(t_SOD_VIRT_ATTR_OBJ * const po_this,
    UINT16 const w_index, UINT8 const b_subIndex,
    SOD_t_ABORT_CODES * const pe_abortCode);







/**
* @brief This function initializes the addresses and number of entries as well as the default values.
*
* Start-, end- and actual address are set and number of SOD entries are calculated.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
*/
void SOD_InitVirt(BYTE_B_INSTNUM)
{
  t_SOD_VIRT_ATTR_OBJ* po_this = &as_SodVirt[B_INSTNUMidx];
  SOD_t_ENTRY_VIRT const *ps_tmpSodAddr;

  /* if there is an object dictionary referenced */
  if ( NULL != SAPL_ps_SOD_VIRT[B_INSTNUMidx])
  {
    po_this->ps_startSodAddr = (SOD_t_ENTRY_VIRT*)SAPL_ps_SOD_VIRT[B_INSTNUMidx];
    /* actual address is initialized with start address of the SOD */
    po_this->ps_actSodAddr = po_this->ps_startSodAddr;
    /* set the number of entries to zero */
    po_this->dw_noSodEntries = 0U;

    ps_tmpSodAddr = po_this->ps_startSodAddr;
    /* while the end of the SOD reached */
    while (ps_tmpSodAddr->w_indexStart != SOD_k_END_OF_THE_OD )
    {
        /* increase the w_noEntriesInOD for calculating the length of the OD */
        po_this->dw_noSodEntries++;
        /* step to the next object in the object dictionary */
        ps_tmpSodAddr++;
    }
    /* end address of the object dictionary is initialized */
    po_this->ps_endSodAddr = po_this->ps_startSodAddr + po_this->dw_noSodEntries;
  }
}


/**
* @brief This function checks weather the SOD entry matches the searched one.
*
* The SOD entry given may have a range of objects it contains, therefore it has to be
* checked weather the searched object is among them.
*
* @param        ps_entry         given SOD entry
*
* @param        w_index          index of the searched SOD entry
*
* @param        b_subIndex       sub-index of the searched SOD entry
*
* @return
* - TRUE            - entry matches
* - FALSE           - entry does NOT match
*/
static BOOLEAN const SOD_EntryMatch(SOD_t_ENTRY_VIRT const * const ps_entry,  UINT16 const w_index,
    UINT8 const b_subIndex)
{
  /* check assumptions */
  if ((w_index >= ps_entry->w_indexStart) &&
      (w_index <= ps_entry->w_indexEnd) &&
      (b_subIndex >= ps_entry->b_subIndexStart) &&
      (b_subIndex <= ps_entry->b_subIndexEnd))
  {
    return TRUE;
  }
  return FALSE;
}

/**
 * @brief This function searches a via index/sub-index specified object in the SOD and returns a pointer to the object.
 *
 * @param        po_this              pointer to the module instance(not checked)valid range: <> NULL
 *
 * @param        w_idx                index of object attributes to be read(not checked, any value allowed) valid range: (UINT16)
 *
 * @param        b_subIdx             sub-index of object attributes to be read(not checked, any value allowed) valid range: (UINT8)
 *
 * @retval          pe_abortCode         pointer to the abort code(pointer not checked, only called with reference to enum in SOD_AttrGet()) valid range: <> NULL
 *
 * @return
 * - <> NULL            - pointer to the SOD entry
 * - == NULL            - SOD entry not available, see pe_abortCode
 */
static SOD_t_ENTRY_VIRT const * const SOD_Search(t_SOD_VIRT_ATTR_OBJ * const po_this,
    UINT16 const w_index, UINT8 const b_subIndex,
    SOD_t_ABORT_CODES * const pe_abortCode)
{
  SOD_t_ENTRY_VIRT const *ps_ret = (SOD_t_ENTRY_VIRT*) NULL;
  SOD_t_ENTRY_VIRT const *ps_tmpEntry = po_this->ps_startSodAddr;
  UINT32 dw_start;
  UINT32 dw_end;
  UINT32 dw_mid;
  UINT32 dw_noElements = dw_end = po_this->dw_noSodEntries;
  BOOLEAN o_found = FALSE;
  BOOLEAN o_idxFound = FALSE;

  dw_start = 0;
  dw_end = po_this->dw_noSodEntries;
  dw_mid = (dw_start + dw_end) / 2;

  /** loop through the SOD */
  while ((!o_found) &&
         (0 < dw_noElements))
  {
    /* number of elements is divided by two */
    dw_noElements /= 2;
    /* get the temporary entry for checking the SOD */
    ps_tmpEntry = po_this->ps_startSodAddr + dw_mid;

    /* check if the entry matches */
    o_found = SOD_EntryMatch(ps_tmpEntry,w_index,b_subIndex);
    /* if entry was not found */
    if (!o_found)
    {
      /* check if at least the correct index was found */
      if ( w_index == ps_tmpEntry->w_indexStart)
      {
        o_idxFound = TRUE;
      }
      /* check if requested object is to be found before or after the temporary object */
      if ((w_index > ps_tmpEntry->w_indexStart) ||
          ((w_index == ps_tmpEntry->w_indexStart) &&
           (b_subIndex > ps_tmpEntry->b_subIndexEnd)))
      {
        dw_start = dw_mid;
      }
      else
      {
        dw_end = dw_mid;
      }
      dw_mid = (dw_start + dw_end) / 2;
    }
  }
  /* if the SOD entry was found */
  if (o_found)
  {
    ps_ret = ps_tmpEntry;
    *pe_abortCode = SOD_ABT_NO_ERROR;
  }
  /* if only the object index was found but the sub-index is NOT available */
  else if (o_idxFound)
  {
    *pe_abortCode = SOD_ABT_SUB_IDX_DOES_NOT_EXIST;
  }
  /* the whole object is missing */
  else
  {
    *pe_abortCode = SOD_ABT_OBJ_DOES_NOT_EXIST;
  }

  return ps_ret;
}

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
    SOD_t_ERROR_RESULT * const ps_errRes)
{
  PTR_TYPE dw_hdl = 0;
  SOD_t_ATTR const *ps_sodAttr = (SOD_t_ATTR*)NULL;
  SOD_t_ENTRY_VIRT const *ps_sodVirtAttr = (SOD_t_ENTRY_VIRT*)NULL;
  t_SOD_VIRT_ATTR_OBJ* po_this = &as_SodVirt[B_INSTNUMidx];

  /* if the reference to the error result structure is NULL */
  if (ps_errRes == NULL)
  {
    /* return NULL, the error can not be signaled because the error result
       structure is NULL */
  }
#if (EPLS_cfg_MAX_INSTANCES > 1)
  /* else if b_instNum is wrong */
  else if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
  {
    ps_errRes->w_errorCode = SOD_k_ERR_INST_NUM_11;
    ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
  }
#endif
  else if ( NULL == ps_acs)
  {
    ps_errRes->w_errorCode = SOD_k_ERR_ACS_OBJ_1;
    ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
  }
  else
  {
    /* try to find the object in the static SOD */
    ps_sodAttr = SOD_AttrGet(B_INSTNUM_ ps_acs->s_obj.w_index, ps_acs->s_obj.b_subIndex, &dw_hdl, &ps_acs->o_applObj, ps_errRes);

    if ( (SOD_t_ATTR*)NULL != ps_sodAttr)
    {
      /* copy the content of the SOD entry to the external variable */

      //(void)MEMCOPY((void*)&ps_acs->s_obj,(void*)&ps_acs_test->s_obj,sizeof(SOD_t_OBJECT));
      (void)MEMCOPY((void*)&ps_acs->s_obj,(void*)dw_hdl,sizeof(SOD_t_OBJECT));
      /* copy the handle into the new virtual object for further access */
      ps_acs->dw_hdl  = dw_hdl;
    }
    else
    {
      /* set the handle to zero */
      ps_acs->dw_hdl  = 0UL;

      /* search in virtual object dictionary */
      ps_sodVirtAttr = SOD_Search(po_this,
          ps_acs->s_obj.w_index, ps_acs->s_obj.b_subIndex,&ps_errRes->e_abortCode);
      /* check if object was found */
      if ( NULL != ps_sodVirtAttr)
      {
        ps_sodAttr = &ps_acs->s_obj.s_attr;
        /* copy data to the ptr supplied by the calling fct */
        (void)MEMCOPY((void*)&ps_acs->s_obj.s_attr,(void*)&ps_sodVirtAttr->s_attr,sizeof(ps_acs->s_obj.s_attr));

        ps_acs->s_obj.pv_range = ps_sodVirtAttr->pv_range;
        ps_acs->s_obj.pv_objData = ps_sodVirtAttr->pv_objData;
        ps_acs->s_obj.pf_clbk = ps_sodVirtAttr->pf_clbk;
        ps_acs->o_applObj = FALSE;
        /* if the object is an array correct the data ptr */
        if (EPLS_IS_BIT_SET(ps_acs->s_obj.s_attr.w_attr, SOD_k_ATTR_ARRAY))
        {
          /* array objects are all within the same index */
          ps_acs->s_obj.pv_objData = (UINT8*)ps_acs->s_obj.pv_objData + ((ps_acs->s_obj.b_subIndex - ps_sodVirtAttr->b_subIndexStart)*ps_sodVirtAttr->s_attr.dw_objLen);
        }
        else if (EPLS_IS_BIT_SET(ps_acs->s_obj.s_attr.w_attr, SOD_k_ATTR_SIMU))
        {
          if (!(ps_sodVirtAttr->pf_clbk)(B_INSTNUM_ SOD_k_SRV_READ_ATTR,
              &ps_acs->s_obj,NULL,ps_acs->dw_segOfs,ps_acs->dw_segSize,
              &ps_errRes->e_abortCode))
          {
            ps_errRes->w_errorCode = SOD_k_ERR_SIMU_FCT_1;
          }
        }
      }
      else
      {
        ps_errRes->w_errorCode = SOD_k_ERR_ATTR_GET_VIRT;
      }
    }
  }
  return ps_sodAttr;
}
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
                                  SOD_t_ERROR_RESULT *ps_errRes)
{
  const SOD_t_ATTR *ps_ret = (SOD_t_ATTR *) NULL; /* return value */
  t_SOD_VIRT_ATTR_OBJ* po_this;

  /* if the reference to the error result structure is NULL */
  if (ps_errRes == NULL )
  {
    /* return NULL, the error can not be signaled because the error result
     structure is NULL */
  }
#if (EPLS_cfg_MAX_INSTANCES > 1)
  /* else if b_instNum is wrong */
  else if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
  {
    ps_errRes->w_errorCode = SOD_k_ERR_INST_NUM_12;
    ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
  }
#endif
  /* else if invalid handle */
  else if (NULL == ps_acs)
  {
    ps_errRes->w_errorCode = SOD_k_ERR_ACS_OBJ_2;
    ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
  }
  else /* else all parameter ok */
  {
    ps_errRes->w_errorCode = SOD_k_ERR_NO;
    ps_errRes->e_abortCode = SOD_ABT_NO_ERROR;

    /* get the correct SOD */
    po_this = &as_SodVirt[B_INSTNUMidx];

    /* if first object */
    if (o_firstObj)
    {
      /* set the actual address to the start address */
      po_this->ps_actSodAddr = po_this->ps_startSodAddr;
      /* get the object data from the first SOD entry */
      ps_acs->s_obj.w_index = po_this->ps_actSodAddr->w_indexStart;
      ps_acs->s_obj.b_subIndex = po_this->ps_actSodAddr->b_subIndexStart;
      ps_acs->dw_segOfs = 0;
      ps_acs->dw_segSize = 0;
    }
    /* else next object */
    else
    {
      /* first try is to only increase the subindex */
      if (ps_acs->s_obj.b_subIndex < po_this->ps_actSodAddr->b_subIndexEnd)
      {
        ps_acs->s_obj.b_subIndex++;
      }
      /* next try to increase the index */
      else if (ps_acs->s_obj.w_index < po_this->ps_actSodAddr->w_indexEnd)
      {
        ps_acs->s_obj.w_index++;
        ps_acs->s_obj.b_subIndex = po_this->ps_actSodAddr->b_subIndexStart;
      }
      /* last switch to next SOD entry (if one more is available) */
      else if (po_this->ps_actSodAddr < po_this->ps_endSodAddr)
      {
        po_this->ps_actSodAddr++;
        ps_acs->s_obj.w_index = po_this->ps_actSodAddr->w_indexStart;
        ps_acs->s_obj.b_subIndex = po_this->ps_actSodAddr->b_subIndexStart;
      }
    }
    /* return initial zero if end of SOD is reached, otherwise the pointer to the attributes */
    if (po_this->ps_actSodAddr != po_this->ps_endSodAddr)
    {
      ps_ret = SOD_AttrGetVirt(B_INSTNUM_ ps_acs,ps_errRes);
    }
  }

  return ps_ret;
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
        SOD_t_ERROR_RESULT * const ps_errRes)
{
  void *pv_ret = NULL; /* return value */
  void *pv_objData; /* pointer to the object data */
  BOOLEAN o_ret = TRUE;
  /* if the reference to the error result structure is NULL */
  if (ps_errRes == NULL)
  {
    /* return NULL, the error can not be signaled because the error result
       structure is NULL */
  }
#if (EPLS_cfg_MAX_INSTANCES > 1)
  /* else if b_instNum is wrong */
  else if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
  {
    ps_errRes->w_errorCode = SOD_k_ERR_INST_NUM_13;
    ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
  }
#endif
  /* else if invalid handle */
  else if (NULL == ps_acs)
  {
    ps_errRes->w_errorCode = SOD_k_ERR_ACS_OBJ_3;
    ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
  }
  else
  {
    /* if it is an object out of the static SOD use the related access function */
    if ((0 != ps_acs->dw_hdl) ||
        (ps_acs->o_applObj))
    {
      pv_ret = SOD_Read(B_INSTNUM_ ps_acs->dw_hdl,ps_acs->o_applObj,ps_acs->dw_segOfs,ps_acs->dw_segSize,ps_errRes);
    }
    else
    {
      /* if offset and size is not 0 for the standard data types */
      if (((0U != ps_acs->dw_segOfs) || (0U != ps_acs->dw_segSize)) &&
          (EPLS_k_DOMAIN != ps_acs->s_obj.s_attr.e_dataType) &&
          (EPLS_k_VISIBLE_STRING != ps_acs->s_obj.s_attr.e_dataType) &&
          (EPLS_k_OCTET_STRING != ps_acs->s_obj.s_attr.e_dataType))
      {
        ps_errRes->w_errorCode = SOD_k_ERR_TYPE_OFFSET_3;
        ps_errRes->e_abortCode = SOD_ABT_GENERAL_ERROR;
      }
      /* else offset and size match to the data type */
      else
      {
        /* if offset and size are valid (dw_offset + dw_size)
           <= ps_object->s_attr.dw_objLen does not work because of
           UINT32 overflow */
        if ((ps_acs->dw_segOfs < ps_acs->s_obj.s_attr.dw_objLen) &&
            (ps_acs->dw_segSize <= (ps_acs->s_obj.s_attr.dw_objLen-ps_acs->dw_segOfs)))
        {
          /* callback before read is requested - e.g. for updating/latching the data */
          /* object is simulated */
          if (EPLS_IS_BIT_SET(ps_acs->s_obj.s_attr.w_attr, SOD_k_ATTR_BEF_RD) ||
              EPLS_IS_BIT_SET(ps_acs->s_obj.s_attr.w_attr, SOD_k_ATTR_SIMU))
          {
            /* the object callback is called with SOD_k_SRV_BEFORE_READ service*/
            o_ret = (ps_acs->s_obj.pf_clbk)(B_INSTNUM_ SOD_k_SRV_BEFORE_READ, &ps_acs->s_obj,
                                         NULL, ps_acs->dw_segOfs, ps_acs->dw_segSize,
                                         &ps_errRes->e_abortCode);
            if (!o_ret)
            {
              ps_errRes->w_errorCode = SOD_k_ERR_CLBK_ABORT_BR_2;
            }
          }
          /* either no callback was necessary or the callback was successful */
          if (o_ret)
          {
            /* if the object data type is DOMAIN or VISIBLE STRING or
                  OCTET STRING */
            if ((EPLS_k_VISIBLE_STRING == ps_acs->s_obj.s_attr.e_dataType) ||
                (EPLS_k_OCTET_STRING == ps_acs->s_obj.s_attr.e_dataType) ||
                (EPLS_k_DOMAIN == ps_acs->s_obj.s_attr.e_dataType))
            {
              pv_objData = ((SOD_t_ACT_LEN_PTR_DATA *)
                                   (ps_acs->s_obj.pv_objData))->pv_objData;
            }
            else /* other data type */
            {
              pv_objData = ps_acs->s_obj.pv_objData;
            }

            /* if SOD_k_ATTR_P2P is set */
            if (EPLS_IS_BIT_SET(ps_acs->s_obj.s_attr.w_attr,
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
            pv_ret = (void *)ADD_OFFSET(pv_ret, ps_acs->dw_segOfs);

            ps_errRes->w_errorCode = SOD_k_ERR_NO;
            ps_errRes->e_abortCode = SOD_ABT_NO_ERROR;
          }
        }
        /* else offset or/and size are invalid */
        else
        {
          ps_errRes->w_errorCode = SOD_k_ERR_OFFSET_SIZE_3;
          ps_errRes->e_abortCode = SOD_ABT_LEN_IS_TOO_HIGH;
        }
      }
    }
  }
  return pv_ret;
}
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
        void const * const pv_data, BOOLEAN const o_overwrite)
{
  BOOLEAN o_ret = FALSE; /* return value */
  SOD_t_ABORT_CODES e_abortCode;
#if (EPLS_cfg_MAX_INSTANCES > 1)
  /* if b_instNum is wrong */
  if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
  {
    SERR_SetError(EPLS_k_NO_INSTANCE, SOD_k_ERR_INST_NUM_14,
                  (UINT32)SOD_ABT_GENERAL_ERROR);
  }
  else
#endif
  /* else if invalid handle */
  if (NULL == ps_acs)
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_ACS_OBJ_4,
                  (UINT32)SOD_ABT_GENERAL_ERROR);
  }
  /* else if the pointer to the data is invalid */
  else if (pv_data == NULL)
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_DATA_POINTER_INVALID_2,
                  (UINT32)SOD_ABT_GENERAL_ERROR);
  }
  /* else all parameter ok */
  else
  {
    /* if it is an object out of the static SOD use the related access function */
    if ((0 != ps_acs->dw_hdl) ||
        (ps_acs->o_applObj))
    {
      o_ret = SOD_Write(B_INSTNUM_ ps_acs->dw_hdl,ps_acs->o_applObj,pv_data,o_overwrite,
          ps_acs->dw_segOfs,ps_acs->dw_segSize);
    }
    else
    {
      /* if the SOD write access is disabled */
      if (!SOD_WriteAccess(B_INSTNUM))
      {
        SERR_SetError(B_INSTNUM_ SOD_k_ERR_SOD_WRITE_DISABLED_2,
                      (UINT32)SOD_ABT_PRESENT_DEVICE_STATE);
      }
      /* else the SOD write access is enabled */
      else
      {
        /* if offset and size is not 0 for the standard data types */
        if (((0U != ps_acs->dw_segOfs) || (0U != ps_acs->dw_segSize)) &&
            (EPLS_k_DOMAIN != ps_acs->s_obj.s_attr.e_dataType) &&
            (EPLS_k_VISIBLE_STRING != ps_acs->s_obj.s_attr.e_dataType) &&
            (EPLS_k_OCTET_STRING != ps_acs->s_obj.s_attr.e_dataType))
        {
          SERR_SetError(B_INSTNUM_ SOD_k_ERR_TYPE_OFFSET_4,
                        (UINT32)ps_acs->s_obj.w_index | ((UINT32)ps_acs->s_obj.b_subIndex << 16) | 0xFF000000UL);
        }
        /* else offset and size match to the data type */
        else
        {
          /* if offset and size are valid (dw_offset + dw_size)
             <= ps_object->s_attr.dw_objLen does not work because of
             UINT32 overflow */
          if ((ps_acs->dw_segOfs < ps_acs->s_obj.s_attr.dw_objLen) &&
              (ps_acs->dw_segSize <= (ps_acs->s_obj.s_attr.dw_objLen-ps_acs->dw_segOfs)))
          {
            /* if the read only write access is accepted */
            if (SOD_ObjectReadOnly(pv_data, &ps_acs->s_obj, o_overwrite))
            {
              o_ret = TRUE;
            }
            /* else the write access has to be checked */
            else
            {
              /* if the object is writeable */
              if (SOD_ObjectWriteable(B_INSTNUM_ &ps_acs->s_obj, o_overwrite))
              {
                /* if data is valid */
                if (SOD_DataValid(B_INSTNUM_ pv_data, &ps_acs->s_obj, ps_acs->dw_segOfs,
                    ps_acs->dw_segSize))
                {
                  o_ret = TRUE;

                  /* if the corresponding attribute is set */
                  if (EPLS_IS_BIT_SET(ps_acs->s_obj.s_attr.w_attr, SOD_k_ATTR_SIMU))
                  {
                    /* the object callback is called with SOD_k_SRV_BEFORE_WRITE service */
                    o_ret = (ps_acs->s_obj.pf_clbk)(B_INSTNUM_ SOD_k_SRV_BEFORE_WRITE, &ps_acs->s_obj,
                                                 pv_data, ps_acs->dw_segOfs, ps_acs->dw_segSize, &e_abortCode);

                    /* if the callback returned error */
                    if (!o_ret)
                    {
                      SERR_SetError(B_INSTNUM_ SOD_k_ERR_CLBK_ABORT_BW_2, (UINT32)e_abortCode);
                    }
                  }
                  /* if the corresponding attribute is set */
                  else if (EPLS_IS_BIT_SET(ps_acs->s_obj.s_attr.w_attr, SOD_k_ATTR_BEF_WR))
                  {
                    /* the object callback is called with SOD_k_SRV_BEFORE_WRITE service */
                    o_ret = (ps_acs->s_obj.pf_clbk)(B_INSTNUM_ SOD_k_SRV_BEFORE_WRITE, &ps_acs->s_obj,
                                                 pv_data, ps_acs->dw_segOfs, ps_acs->dw_segSize, &e_abortCode);

                    /* if the callback returned error */
                    if (!o_ret)
                    {
                      SERR_SetError(B_INSTNUM_ SOD_k_ERR_CLBK_ABORT_BW_3, (UINT32)e_abortCode);
                    }
                  }
                  /* RSM_IGNORE_QUALITY_BEGIN Notice #55 - Scope level
                                        exceeds the defined limit of 10 */
                  if (o_ret)
                  {
                  /* RSM_IGNORE_QUALITY_END */
                    /* data is copied */
                    SOD_DataCpy(pv_data, &ps_acs->s_obj, ps_acs->dw_segOfs, ps_acs->dw_segSize);

                    /* the segmented object callback will be called with
                       SOD_k_SRV_AFTER_WRITE service*/
                    o_ret = SOD_CallAfterWriteClbk(B_INSTNUM_ &ps_acs->s_obj,
                        ps_acs->dw_segOfs, ps_acs->dw_segSize);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return o_ret;
}

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
                         UINT32 const dw_actLen)
{
  BOOLEAN o_ret = FALSE; /* return value */
  const SOD_t_OBJECT *ps_object; /* pointer to an object in the OD */
  SOD_t_ACT_LEN_PTR_DATA *ps_actLenPtrData; /* pointer to the structure with
                                               the actual length */
#if (EPLS_cfg_MAX_INSTANCES > 1)
  /* if b_instNum is wrong */
  if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
  {
    SERR_SetError(EPLS_k_NO_INSTANCE, SOD_k_ERR_INST_NUM_15,
                  (UINT32)SOD_ABT_GENERAL_ERROR);
  }
  else  /* else all parameter ok */
#endif
  /* else if invalid handle */
  if (NULL == ps_acs)
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_ACS_OBJ_5,
                  (UINT32)SOD_ABT_GENERAL_ERROR);
  }
  else
  {
    /* set the length of a static SOD object */
    if ((0 != ps_acs->dw_hdl) ||
        (ps_acs->o_applObj))
    {
      o_ret = SOD_ActualLenSet(B_INSTNUM_ ps_acs->dw_hdl, ps_acs->o_applObj, dw_actLen);
    }
    else
    {
      ps_object = &ps_acs->s_obj;
      /* if data type is EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING or
            EPLS_k_OCTET_STRING */
      if ((ps_object->s_attr.e_dataType == EPLS_k_DOMAIN) ||
          (ps_object->s_attr.e_dataType == EPLS_k_VISIBLE_STRING) ||
          (ps_object->s_attr.e_dataType == EPLS_k_OCTET_STRING))
      {
        /* set the pointer to the structure with the actual length */
        ps_actLenPtrData =
            (SOD_t_ACT_LEN_PTR_DATA *)ps_object->pv_objData;

        /* if the actual length to be set is smaller or equal to
              the maximum length of the object */
        if (dw_actLen <= ps_object->s_attr.dw_objLen)
        {
          /* actual length is set */
          ps_actLenPtrData->dw_actLen = dw_actLen;

          o_ret = TRUE;
        }
        else /* actual length to be set is to large */
        {
          SERR_SetError(B_INSTNUM_ SOD_k_ERR_ACT_LEN_2,
                        (UINT32)SOD_ABT_GENERAL_ERROR);
        }
      }
      else /* other (standard) data type */
      {
        SERR_SetError(B_INSTNUM_ SOD_k_ERR_ACT_LEN_TYPE_2,
                      (UINT32)SOD_ABT_GENERAL_ERROR);
      }
    }
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_ret;
}

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
                         UINT32 * const pdw_objLen)
{
  BOOLEAN o_ret = FALSE; /* return value */
  const SOD_t_OBJECT *ps_object; /* pointer to an object in the OD */
  SOD_t_ACT_LEN_PTR_DATA *ps_actLenPtrData; /* pointer to the structure with
                                               the actual length */
#if (EPLS_cfg_MAX_INSTANCES > 1)
  /* if b_instNum is wrong */
  if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
  {
    SERR_SetError(EPLS_k_NO_INSTANCE, SOD_k_ERR_INST_NUM_16,
                  (UINT32)SOD_ABT_GENERAL_ERROR);
  }
  else
#endif
  /* else if invalid handle */
  if (NULL == ps_acs)
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_ACS_OBJ_6,
                  (UINT32)SOD_ABT_GENERAL_ERROR);
  }
  /* else if the pointer to the object length is invalid */
  else if (pdw_objLen == (UINT32 *)NULL)
  {
    SERR_SetError(B_INSTNUM_ SOD_k_ERR_ACT_LEN_PTR_2,
                  (UINT32)SOD_ABT_GENERAL_ERROR);
  }
  /* else all parameter ok */
  else
  {
    *pdw_objLen = 0x00UL;

    /* get the length of a static SOD object */
    if ((0 != ps_acs->dw_hdl) ||
        (ps_acs->o_applObj))
    {
      o_ret = SOD_ActualLenGet(B_INSTNUM_ ps_acs->dw_hdl, ps_acs->o_applObj, pdw_objLen);
    }
    else
    {
      ps_object = &ps_acs->s_obj;
      /* if data type is EPLS_k_DOMAIN, EPLS_k_VISIBLE_STRING or
            EPLS_k_OCTET_STRING */
      if ((ps_object->s_attr.e_dataType == EPLS_k_DOMAIN) ||
          (ps_object->s_attr.e_dataType == EPLS_k_VISIBLE_STRING) ||
          (ps_object->s_attr.e_dataType == EPLS_k_OCTET_STRING))
      {
        /* set the pointer to the structure with the actual legnth */
        ps_actLenPtrData =
            (SOD_t_ACT_LEN_PTR_DATA *)ps_object->pv_objData;

        *pdw_objLen = ps_actLenPtrData->dw_actLen;
      }
      else /* other (standard) data type */
      {
        *pdw_objLen = ps_object->s_attr.dw_objLen;
      }
      o_ret = TRUE;
    }
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_ret;
}

/**
* @brief This function sets the actual values to the default values in the SOD.
*
* @param        b_instNum         instance number (not checked, checked in SSC_InitAll()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - TRUE            - success
* - FALSE           - failure
*/
BOOLEAN SOD_SetDefaultValuesVirt(BYTE_B_INSTNUM)
{
  BOOLEAN o_ret = FALSE; /* return value */
  BOOLEAN o_break = FALSE; /* break flag */
  const SOD_t_OBJECT *ps_tmpSodAddr; /* temporary pointer for the
                                       object dictionary checking */

  SOD_t_ATTR * ps_sodAttr = NULL; /* pointer to the SOD entry attribute */
  SOD_t_ACS_OBJECT_VIRT s_acs; /* SOD access structure */
  SOD_t_ERROR_RESULT s_errRes;  /* error being returned when accessing the SOD */
  void *pv_data; /* pointer to the actual value of the object */
  const UINT64 ddw_noDefVal=0UL; /* the object that has not default value
                                    is initialized with this constant */

  ps_sodAttr = (SOD_t_ATTR*)SOD_AttrGetNextVirt(B_INSTNUM_ TRUE,&s_acs,&s_errRes);

  while ((!o_break) && (NULL != ps_sodAttr))
  {
    ps_tmpSodAddr = &s_acs.s_obj;
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

        MEMSET(pv_data,0,ps_tmpSodAddr->s_attr.dw_objLen);
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
    /* get next object */
    ps_sodAttr = (SOD_t_ATTR*)SOD_AttrGetNextVirt(B_INSTNUM_ FALSE,&s_acs,&s_errRes);
  }

  /* check if stopped because of an error */
  if (!o_break)
  {
    o_ret = TRUE;
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}
/** @} */
