/**
 * @addtogroup SFS
 * @{
 *
 * @file SFScopy.c
 *
 * The file implements functions to copy data.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * At the same time addressing format (little endian / big endian) is taken into account.
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#include "SCFMapi.h"
#include "SHNF.h"
#include "SERRapi.h"
#include "SERR.h"

#include "SFS.h"
#include "SFSerr.h"

/**
 * @brief This function swaps the data if it is necessary and copies.
 *
 * The data with the given data type will be
 * copied from source pointer to the destination pointer dependent on the data type. If the host format defined
 * by ENDIAN in the EPLStarget.h is big endian then the data will be swapped. Parameter dw_len is only used in
 * case of data types which contains any kind of strings with unknown length.
 * @attention The parameters will not be checked by this function.
 *
 * @param        pv_dst        destination pointer (not checked), valid range: <> NULL
 *
 * @param        pv_src        source pointer (not checked), valid range: <>NULL
 *
 * @param        dw_len        length of the data to be copied in case of type: EPLS_k_VISIBLE_STRING,
 *                             EPLS_k_DOMAIN, EPLS_k_OCTET_STRING (not checked, any value allowed), valid range: any 32 bit value
 *
 * @param        e_type        data type of the data to be copied (checked) valid range: see EPLS_t_DATATYPE
 *
 * @return
 * - TRUE        - copy process succeeded
 * - FALSE       - copy process failed, data type is NOT supported
 */
BOOLEAN SFS_NetworkCopyGen(void *pv_dst, const void *pv_src, UINT32 dw_len,
                           EPLS_t_DATATYPE e_type)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */

  /* Switch for the object length */
  switch(e_type)
  {
    case EPLS_k_VISIBLE_STRING:
    case EPLS_k_DOMAIN:
    case EPLS_k_OCTET_STRING:
    {
      SFS_NET_CPY_DOMSTR(pv_dst, pv_src, dw_len);
      o_return = TRUE;
      break;
    }
    case EPLS_k_BOOLEAN:
    case EPLS_k_INT8:
    case EPLS_k_UINT8:
    {
      SFS_NET_CPY8(pv_dst, pv_src);
      o_return = TRUE;
      break;
    }

    case EPLS_k_INT16:
    case EPLS_k_UINT16:
    {
      SFS_NET_CPY16(pv_dst, pv_src);
      o_return = TRUE;
      break;
    }

    case EPLS_k_INT32:
    case EPLS_k_UINT32:
    case EPLS_k_REAL32:
    {
      SFS_NET_CPY32(pv_dst, pv_src);
      o_return = TRUE;
      break;
    }

    #if(EPLS_cfg_MAX_PYLD_LEN >= 12)
      case EPLS_k_INT64:
      case EPLS_k_UINT64:
      case EPLS_k_REAL64:
      {
        SFS_NET_CPY64(pv_dst, pv_src);
        o_return = TRUE;
        break;
      }

    #endif
    default:
    {
      /* error: data type is NOT defined */
      SERR_SetError(EPLS_k_NO_INSTANCE_ SFS_k_ERR_DATATYPE_NOT_DEF,
                    (UINT32)(e_type));
    }
  }

  SCFM_TACK_PATH();
  return o_return;
  /* RSM_IGNORE_QUALITY_BEGIN Notice #14 - 'case' conditions do not equal
                                           'break' */
}
  /* RSM_IGNORE_QUALITY_END */

/** @} */
