/******************************************************************************
** Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
**                      IXXAT Automation GmbH
** All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
**
** This source code is free software; you can redistribute it and/or modify it
** under the terms of the BSD license (according to License.txt).
********************************************************************************
**
**   Workfile: SAPLssdoc.c
**    Summary: SAPLssdoc - Safety application Demonstration
**             SAPL, application of the EPLsafety Stack
**             This file demonstrates the SSDOC read and write access to the
**             remote SODs of the SNs.
**
**     Author: M. Molnar
**
********************************************************************************
********************************************************************************
**
**  Functions: SAPL_ssdocAccess
**
**             ScmSn1RespClbk
**             Sn2RespClbk
**
**    Remarks:
**
**
*******************************************************************************/


/*******************************************************************************
**    compiler directives
*******************************************************************************/


/*******************************************************************************
**    include-files
*******************************************************************************/
#include "../include/UDP.h"
#include "SERRapi.h"

#include "SODapi.h"
#include "SOD.h"
#include "SSDOCapi.h"
#include "SCFMapi.h"

#include "SAPL.h"

/*******************************************************************************
**    global variables
*******************************************************************************/

/*******************************************************************************
**    static constants, types, macros, variables
*******************************************************************************/
/** cfg_SSDOC_WRITE_ACCESS_IN_OPERATIONAL:
    If this define is enabled then a SSDOC write access is accomplished cyclic
    in operational state. Therfore a minor error is generated cyclic.
*/
//#define cfg_SSDOC_WRITE_ACCESS_IN_OPERATIONAL


typedef struct
{
  UINT16          w_idx;        /* Index of the object */
  UINT8           b_subIdx;     /* Sub-index of the object */
  EPLS_t_DATATYPE e_dataType;   /* data type of the object */
  BOOLEAN         o_readAccess; /* TRUE  : read access is started
                                   FALSE : write access is started */
} t_ACCESSED_OBJECT;

/* maximum length of the data that will be read */
#define k_MAX_DATA_LENGTH 100

/* data buffer for the SCM SN1 */
static UINT8 ab_RdWrDataScmSn1[k_MAX_DATA_LENGTH];
/* data buffer for the SN2 */
static UINT8 ab_RdWrDataSn2[k_MAX_DATA_LENGTH];

/* flag to signal that the SSDOC access is finished or not to the SCM SN1 */
static BOOLEAN o_AccessFinishedScmSn1 = TRUE;
/* flag to signal that the SSDOC access is finished or not to SN2 */
static BOOLEAN o_AccessFinishedSn2 = TRUE;


/* Array for the objects that will be accessed */
static t_ACCESSED_OBJECT as_AccessedObjs[] =
{
/*  Index, Sub-index,     Data type, Access type (TRUE : RD; FALSE : WR) */
  {0x101AU,      0x00U,  EPLS_k_DOMAIN,        TRUE},
  {0x2100U,      0x01U,  EPLS_k_UINT8,         TRUE},
  {0x2200U,      0x01U,  EPLS_k_DOMAIN,        TRUE},
  {0x2100U,      0x02U,  EPLS_k_UINT8,         TRUE},
  {0x2101U,      0x01U,  EPLS_k_UINT8,         TRUE},
  #ifdef cfg_SSDOC_WRITE_ACCESS_IN_OPERATIONAL
    {0x2100U,      0x01U,  EPLS_k_UINT8,       FALSE},
  #endif
  {0xFFFFU, 0xFFU, EPLS_k_UINT8, FALSE}, /* End of the table */
};

/*******************************************************************************
**    static function-prototypes
*******************************************************************************/
static void ScmSn1RespClbk(UINT16 w_reqNum, UINT32 dw_transDataLen,
                           UINT32 dw_abortCode);
static void Sn2RespClbk(UINT16 w_reqNum, UINT32 dw_transDataLen,
                        UINT32 dw_abortCode);

/*******************************************************************************
**    global functions
*******************************************************************************/
/*******************************************************************************
**
** Function    : SAPL_ssdocAccess
**
** Description : This function starts SSDOC write and read access to the remote
**               SODs of the SNs.
**
** Parameters  : dw_ct (IN) - consecutive time
**
** Returnvalue : -
**
*******************************************************************************/
void SAPL_ssdocAccess(UINT32 dw_ct)
{
  SSDOC_t_REQ s_transInfo; /* transition information */
  static UINT16 w_accObjOfs = 0U; /* offset of the object in the
                                     as_AccessedObjs that will be accessed */
  SOD_t_ACCESS s_sodAcs;
  SOD_t_ERROR_RESULT s_errRes;
  UINT8 *pb_snStatus1;
  UINT8 *pb_snStatus2;

  /* if the attribute of the SN status is available */
  if (SOD_AttrGet(EPLS_k_SCM_INST_NUM_ 0xC400U, 0x05U, &s_sodAcs.dw_hdl,
                  &s_sodAcs.o_applObj, &s_errRes) != NULL)
  {
    pb_snStatus1 = (UINT8 *)SOD_Read(EPLS_k_SCM_INST_NUM_ s_sodAcs.dw_hdl,
                                     s_sodAcs.o_applObj, SOD_k_NO_OFFSET,
                                     SOD_k_LEN_NOT_NEEDED, &s_errRes);

    /* if the read access succeeded */
    if (pb_snStatus1 != NULL)
    {
      /* if the attribute of the SN status is available */
      if (SOD_AttrGet(EPLS_k_SCM_INST_NUM_ 0xC401U, 0x05U, &s_sodAcs.dw_hdl,
                  &s_sodAcs.o_applObj, &s_errRes) != NULL)
      {
        pb_snStatus2 = (UINT8 *)SOD_Read(EPLS_k_SCM_INST_NUM_ s_sodAcs.dw_hdl,
                                         s_sodAcs.o_applObj, SOD_k_NO_OFFSET,
                                         SOD_k_LEN_NOT_NEEDED, &s_errRes);

        /* if the read access succeeded */
        if (pb_snStatus2 != NULL)
        {

          /* if the as_AccessedObjs is not empty and
                the status of the SNs is OK */
          if ((as_AccessedObjs[0].w_idx != 0xFFFFU) && ((*pb_snStatus1) == 6U)
              && ((*pb_snStatus2) == 6U))
          {
            /* if the end of the table is reached */
            if (as_AccessedObjs[w_accObjOfs].w_idx == 0xFFFFU)
            {
              /* start again */
              w_accObjOfs = 0U;
            }

            s_transInfo.w_idx      = as_AccessedObjs[w_accObjOfs].w_idx;
            s_transInfo.b_subIdx   = as_AccessedObjs[w_accObjOfs].b_subIdx;
            s_transInfo.e_dataType = as_AccessedObjs[w_accObjOfs].e_dataType;
            s_transInfo.dw_dataLen = (UINT32)k_MAX_DATA_LENGTH;

            /* if FSM is available to start an SSDOC access and
                  the access is finished */
            if ((SSDOC_CheckFsmAvailable()) && (o_AccessFinishedScmSn1))
            {
              s_transInfo.b_payloadLen = 12U;
              s_transInfo.pb_data = ab_RdWrDataScmSn1;

              /* if read access is started */
              if (as_AccessedObjs[w_accObjOfs].o_readAccess)
              {
                /* if the read reguest was started */
                if (SSDOC_SendReadReq(SAPL_k_SCM_SN1_SADR, w_accObjOfs,
                                      ScmSn1RespClbk, dw_ct, &s_transInfo))
                {
                  o_AccessFinishedScmSn1 = FALSE;
                }
                /* no else : fail safe error */
              }
              else /* write access is started */
              {
                /* if the read reguest was started */
                if (SSDOC_SendWriteReq(SAPL_k_SCM_SN1_SADR, w_accObjOfs,
                                       ScmSn1RespClbk, dw_ct, &s_transInfo, FALSE))
                {
                  o_AccessFinishedScmSn1 = FALSE;
                }
                /* no else : fail safe error */
              }
            }
            /* else FSM is not available to start an SSDOC access and/or
                    the access is not finished yet */


            /* if FSM is available to start an SSDOC access and
                  the access is finished */
            if ((SSDOC_CheckFsmAvailable()) && (o_AccessFinishedSn2))
            {
              s_transInfo.b_payloadLen = 8U;
              s_transInfo.pb_data = ab_RdWrDataSn2;

              /* if read access is started */
              if (as_AccessedObjs[w_accObjOfs].o_readAccess)
              {
                /* if the read reguest was started */
                if (SSDOC_SendReadReq(SAPL_k_SN2_SADR, w_accObjOfs, Sn2RespClbk,
                                      dw_ct, &s_transInfo))
                {
                  w_accObjOfs++;
                  o_AccessFinishedSn2 = FALSE;
                }
                /* no else : fail safe error */
              }
              /* else write access is started */
              else
              {
                /* if the read reguest was started */
                if (SSDOC_SendWriteReq(SAPL_k_SN2_SADR, w_accObjOfs,
                                       Sn2RespClbk, dw_ct, &s_transInfo, FALSE))
                {
                  o_AccessFinishedSn2 = FALSE;
                  w_accObjOfs++;
                }
                /* no else : fail safe error */
              }
            }
            /* else FSM is not available to start an SSDOC access and/or
                    the access is not finished yet */
          }
          /* no else : SSDOaccess is started later */
        }
        else /* SOD error */
        {
          SAPL_SERR_SignalErrorClbk(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                                    (UINT32)(s_errRes.e_abortCode));
        }
      }
      else /* SOD error */
      {
        SAPL_SERR_SignalErrorClbk(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                                  (UINT32)(s_errRes.e_abortCode));
      }
    }
    else /* SOD error */
    {
      SAPL_SERR_SignalErrorClbk(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                                (UINT32)(s_errRes.e_abortCode));
    }
  }
  else /* SOD error */
  {
    SAPL_SERR_SignalErrorClbk(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                              (UINT32)(s_errRes.e_abortCode));
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/*******************************************************************************
**    static functions
*******************************************************************************/
/*******************************************************************************
**
** Function    : ScmSn1RespClbk
**
** Description : This function is the response callback function of the SSDOC
**               access of the SCM SN1.
**
** Parameters  : w_reqNum (IN)        - offset of the object in the
**                                      as_AccessedObjs that has been accessed
**               dw_transDataLen (IN) - number of received data bytes
**               dw_abortCode (IN)    - == SOD_ABT_NO_ERROR: no error present
**                                      reponse buffer and dw_transDataLen are
**                                      valid
**                                      <> SOD_ABT_NO_ERROR: error present,
**                                      dw_abortCode contains the abort code,
**                                      reponse buffer and dw_transDataLen are
**                                      not relevant
**
** Returnvalue : -
**
*******************************************************************************/
static void ScmSn1RespClbk(UINT16 w_reqNum, UINT32 dw_transDataLen,
                           UINT32 dw_abortCode)
{
  UINT32 i; /* loop counter */

  /* if read access has been started */
  if (as_AccessedObjs[w_reqNum].o_readAccess)
  {
    DBG_PRINTF2("\n 0x%X 0x%X Object was read on the Scm Sn1. ",
                as_AccessedObjs[w_reqNum].w_idx,
                as_AccessedObjs[w_reqNum].b_subIdx);
  }
  /* else write access has been started */
  else
  {
    DBG_PRINTF2("\n 0x%X 0x%X Object was written on the Scm Sn1. ",
                as_AccessedObjs[w_reqNum].w_idx,
                as_AccessedObjs[w_reqNum].b_subIdx);
  }

  /* if the SSDOC access was successful */
  if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  {
    DBG_PRINTF("\n    Data : ");
    for(i = 0UL; i < dw_transDataLen; i++)
    {
      DBG_PRINTF1("0x%X ", ab_RdWrDataScmSn1[i]);
    }
    DBG_PRINTF("\n");
  }
  /* else : the SSDOC access was not successful */
  else
  {
    DBG_PRINTF1("Abort code : 0x%lX\n", dw_abortCode);
  }

  /* signal that the SSDOC access finished */
  o_AccessFinishedScmSn1 = TRUE;

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/*******************************************************************************
**
** Function    : Sn2RespClbk
**
** Description : This function is the response callback function of the SSDOC
**               access of the SN2.
**
** Parameters  : w_reqNum (IN)          - offset of the object in the
**                                        as_AccessedObjs that has been accessed
**               dw_transDataLen (IN)   - number of received data bytes
**               dw_abortCode (IN)      - == SOD_ABT_NO_ERROR: no error present
**                                        Response buffer and dw_transDataLen are
**                                        valid
**                                        <> SOD_ABT_NO_ERROR: error present,
**                                        dw_abortCode contains the abort code,
**                                        Response buffer and dw_transDataLen are
**                                        not relevant
**
** Returnvalue : -
**
*******************************************************************************/
static void Sn2RespClbk(UINT16 w_reqNum, UINT32 dw_transDataLen,
                        UINT32 dw_abortCode)
{
  UINT32 i; /* loop counter */

  /* if read access has been started */
  if (as_AccessedObjs[w_reqNum].o_readAccess)
  {
    DBG_PRINTF2("\n 0x%X 0x%X Object was read on the Sn2. ",
                as_AccessedObjs[w_reqNum].w_idx,
                as_AccessedObjs[w_reqNum].b_subIdx);
  }
  /* else write access has been started */
  else
  {
    DBG_PRINTF2("\n 0x%X 0x%X Object was written on the Sn2. ",
                as_AccessedObjs[w_reqNum].w_idx,
                as_AccessedObjs[w_reqNum].b_subIdx);
  }

  /* if the SSDOC access was successful */
  if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  {
    DBG_PRINTF("\n    Data : ");
    for(i = 0UL; i < dw_transDataLen; i++)
    {
      DBG_PRINTF1("0x%X ", ab_RdWrDataSn2[i]);
    }
    DBG_PRINTF("\n");
  }
  /* else : the SSDOC access was not succesful */
  else
  {
    DBG_PRINTF1("Abort code : 0x%lX\n", dw_abortCode);
  }

  /* signal that the SSDOC access finished */
  o_AccessFinishedSn2 = TRUE;

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

