/**
 * @addtogroup SDN
 * @{
 *
 * @file SDNmain.c
 *
 * This file contains functions to handle a Safety Domain Number.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SDNmain.c</h2>
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
#include "SODapi.h"
#include "SOD.h"
#include "SDN.h"
#include "SDNerr.h"

/**
 *  Data structure to manage access to the SOD entry 0x1200, 0x01, which contains the "Safety Domain Number" of own SN.
*/
static SOD_t_ACS_OBJECT_VIRT s_AcsOwnSdn[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * This modul global array of domain numbers works like a lookup table.
*/
static UINT16 *apw_SdnLookup[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;


/**
* @brief This function returns a reference to object 0x1200, 0x01 in the SOD and initializes the access structure for own SDN.
* This object is the Safety Domain Number of the specified instance. Also the attribute "before read" is checked.
*
* @param  b_instNum instance number (not checked, checked in SSC_InitAll()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @return
* - <> NULL - reference to SOD, object SDN
* - == NULL - SOD access failed
*/
static UINT16 * getCheckSdn(BYTE_B_INSTNUM);

/**
 * This function initializes the SDN module
 *
 * All domain numbers which will be managed by this module will be loaded
 * from the SOD
 *
 * @param b_instNum openSAFETY stack instance number (not checked, checked in SSC_InitAll())
 *                  valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 * @return
 *      - TRUE - initialization succeded
 *      - FALSE - initialization failed
 */
BOOLEAN SDN_Init(BYTE_B_INSTNUM)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  UINT16 *pw_sdn = (UINT16 *)NULL; /* ref to instance specific domain number */


  /* if SDN in current instance is NOT available */
  pw_sdn = getCheckSdn(B_INSTNUM);

  /* if SDN not available */
  if(pw_sdn == NULL)
  {
    /* error: SDN not available or invalid, error already reported,
              return with predefined FALSE */
  }
  else  /* SDN is valid */
  {
    /* store reference to SDN of current instance in pointer array */
    apw_SdnLookup[B_INSTNUMidx] = pw_sdn;
    o_return = TRUE;  /* prepare return value */
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function returns a domain number which is assigned to the specified instance.
*
* @param        b_instNum instance number (not checked, called with EPLS_k_SCM_INST_NUM or
*                         checked in SSC_ProcessSNMTSSDOFrame() or SPDO_BuildTxSpdo() ),
*                         valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @return
*               - == 0x0000 - error, NO domain number is assigned
*               - <> 0x0000 - valid domain number
*/
UINT16 SDN_GetSdn(BYTE_B_INSTNUM)
{
  /* return domain number assigned to specified instance */
  SCFM_TACK_PATH();
  return (*(apw_SdnLookup[B_INSTNUMidx]));
}

/**
* @brief This function sets the Safety Domain Number for the own SN.
*
* @param     b_instNum            instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param     w_ownSdn            own SDN to be assigned, (not checked, checked in checkRxAddrInfo()), valid range : k_MIN_SDN .. k_MAX_SDN
*
* @return
*  - TRUE            - success
*  - FALSE           - failure
*/
BOOLEAN SDN_SetSdn(BYTE_B_INSTNUM_ UINT16 w_ownSdn)
{
  SCFM_TACK_PATH();
  /* SDN (SOD Index : 0x1200, SOD Sub-index 0x01) is Read-only therefore
     SOD_k_OVERWRITE by the application */
  return SOD_WriteVirt(B_INSTNUM_ &s_AcsOwnSdn[B_INSTNUMidx],&w_ownSdn,SOD_k_OVERWRITE);
}

#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors
*               of the assigned unit.
*
* @param        w_errorCode  - error number
*
* @param        dw_addInfo   - additional error information
*
* @retval          pac_str      - empty buffer to build the error string.
*/
  void SDN_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str)
  {
    dw_addInfo = dw_addInfo;  /* avoid compiler error */

    /* choose the matching error string */
    switch(w_errorCode)
    {
      case SDN_k_ERR_ATTR_INV:
      {
        SPRINTF1(pac_str,
          "%#x - SDN_k_ERR_ATTR_INV: getCheckSdn():\n"
          "Object 0x1200, 0x01 (SDN) was accessed with attribute "
          "<SOD_k_ATTR_BEF_RD>. This attribute is illegal object SDN.\n",
          SDN_k_ERR_ATTR_INV);
        break;
      }
      default:
      {
        SPRINTF(pac_str, "Unknown error code was generated by the SDN\n");
      }
    }
    SCFM_TACK_PATH();
    return ;
  }
#endif


/**
* @name Static functions
* @{
**/

/**
* @brief This function returns a reference to object 0x1200, 0x01 in the SOD and initializes the access structure for own SDN.
* This object is the Safety Domain Number of the specified instance. Also the attribute "before read" is checked.
*
* @param  b_instNum instance number (not checked, checked in SSC_InitAll()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @return
* - <> NULL - reference to SOD, object SDN
* - == NULL - SOD access failed
*/
static UINT16 * getCheckSdn(BYTE_B_INSTNUM)
{
  UINT16 *pw_sdn = (UINT16 *)NULL;
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */
  SOD_t_ACS_OBJECT_VIRT *ps_acsOwnSdn = &s_AcsOwnSdn[B_INSTNUMidx]; /* pointer to the
                                                 own SDN SOD access structure */

  /* initiate SOD access data structure */
  ps_acsOwnSdn->s_obj.w_index = EPLS_k_IDX_COMMON_COM_PARAM;
  ps_acsOwnSdn->s_obj.b_subIndex = EPLS_k_SUBIDX_SDN;
  ps_acsOwnSdn->dw_segOfs = 0;
  ps_acsOwnSdn->dw_segSize = 0;

  /* if SOD access error happened */
  if(NULL == SOD_AttrGetVirt(B_INSTNUM_ ps_acsOwnSdn,&s_errRes))
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }
  /* else if attribute <SOD_k_ATTR_BEF_RD> is set */
  else if(EPLS_IS_BIT_SET(ps_acsOwnSdn->s_obj.s_attr.w_attr, SOD_k_ATTR_BEF_RD))
  {
    /* error: object SDN is not allowed to be accessed with attribute
              <SOD_k_ATTR_BEF_RD> set, return with NULL pointer */
    SERR_SetError(B_INSTNUM_ SDN_k_ERR_ATTR_INV, SERR_k_NO_ADD_INFO);
  }
  else  /* object "Safety Domain Number" is ready to be accessed */
  {
    /* get SDN from SOD */
    pw_sdn = (UINT16 *) SOD_ReadVirt(B_INSTNUM_ ps_acsOwnSdn, &s_errRes);
    /* if read access failed */
    if (pw_sdn == (UINT16 *)NULL)
    {
      SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                    (UINT32)(s_errRes.e_abortCode));
    }
  }
  SCFM_TACK_PATH();
  return pw_sdn;
}

/** @} */
/** @} */
