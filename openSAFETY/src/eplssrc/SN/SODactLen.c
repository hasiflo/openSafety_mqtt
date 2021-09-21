/**
 * @addtogroup SOD
 * @{
 *
 * @file SODactLen.c
 *
 * This file manages the actual and maximum length of the SOD
*             objects.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation  GmbH
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
#include "SODint.h"

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
                         UINT32 dw_actLen)
{
  BOOLEAN o_ret = FALSE; /* return value */
  const SOD_t_OBJECT *ps_object; /* pointer to an object in the OD */
  SOD_t_ACT_LEN_PTR_DATA *ps_actLenPtrData; /* pointer to the structure with
                                               the actual length */
  #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
    SOD_t_ABORT_CODES e_saplResult; /* result of the SAPL callback */
  #else
    o_appObj = o_appObj;
  #endif

  #if (EPLS_cfg_MAX_INSTANCES > 1)
    /* if b_instNum is wrong */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      SERR_SetError(EPLS_k_NO_INSTANCE, SOD_k_ERR_INST_NUM_9,
                    (UINT32)SOD_ABT_GENERAL_ERROR);
    }
    else  /* else all parameter ok */
  #endif
    {
      #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
        /* if application object */
        if (o_appObj)
        {
          e_saplResult = SAPL_SOD_ActualLenSet(B_INSTNUM_ dw_hdl, dw_actLen);

          /* if no error happened */
          if (e_saplResult == SOD_ABT_NO_ERROR)
          {
            o_ret = TRUE;
          }
          else /* error happened */
          {
            SERR_SetError(B_INSTNUM_ SOD_k_ERR_ACT_LEN_SET_APL,
                          (UINT32)SOD_ABT_GENERAL_ERROR);
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
                SERR_SetError(B_INSTNUM_ SOD_k_ERR_ACT_LEN,
                              (UINT32)SOD_ABT_GENERAL_ERROR);
              }
            }
            else /* other (standard) data type */
            {
              SERR_SetError(B_INSTNUM_ SOD_k_ERR_ACT_LEN_TYPE,
                            (UINT32)SOD_ABT_GENERAL_ERROR);
            }
          }
          /* no else : dw_hdl is invalid */
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
                         UINT32 *pdw_objLen)
{
  BOOLEAN o_ret = FALSE; /* return value */
  const SOD_t_OBJECT *ps_object; /* pointer to an object in the OD */
  SOD_t_ACT_LEN_PTR_DATA *ps_actLenPtrData; /* pointer to the structure with
                                               the actual length */
  #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
    SOD_t_ABORT_CODES e_saplResult; /* result of the SAPL callback */
  #else
    o_appObj = o_appObj;
  #endif

  #if (EPLS_cfg_MAX_INSTANCES > 1)
    /* if b_instNum is wrong */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      SERR_SetError(EPLS_k_NO_INSTANCE, SOD_k_ERR_INST_NUM_10,
                    (UINT32)SOD_ABT_GENERAL_ERROR);
    }
    else
  #endif
    /* else if the pointer to the object length is invalid */
    if (pdw_objLen == (UINT32 *)NULL)
    {
      SERR_SetError(B_INSTNUM_ SOD_k_ERR_ACT_LEN_PTR,
                    (UINT32)SOD_ABT_GENERAL_ERROR);
    }
    /* else all parameter ok */
    else
    {
      *pdw_objLen = 0x00UL;

      #if (SOD_cfg_APPLICATION_OBJ == EPLS_k_ENABLE)
        /* if application object */
        if (o_appObj)
        {
          e_saplResult = SAPL_SOD_ActualLenGet(B_INSTNUM_ dw_hdl, pdw_objLen);

          /* if no error happened */
          if (e_saplResult == SOD_ABT_NO_ERROR)
          {
            o_ret = TRUE;
          }
          else /* error happened */
          {
            SERR_SetError(B_INSTNUM_ SOD_k_ERR_ACT_LEN_GET_APL,
                          (UINT32)SOD_ABT_GENERAL_ERROR);
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
          /* no else : dw_hdl is invalid */
        }
    }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
  return o_ret;
}


/** @} */
