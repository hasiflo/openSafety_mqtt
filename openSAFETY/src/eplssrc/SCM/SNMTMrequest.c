/**
 * @addtogroup SNMTM
 * @{
 *
 * @file SNMTMrequest.c
 * The file contains for each SNMT Service Requests a single function to call.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SNMTMrequest.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>new function to distribute the initial 40 bit CT</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
 *
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "SCFMapi.h"
#include "SHNF.h"
#include "SERRapi.h"
#include "SERR.h"
#include "SFS.h"
#include "SODapi.h"
#include "SOD.h"
#include "SDN.h"
#include "SNMT.h"
#include "SNMTM.h"
#include "SNMTMint.h"
#include "SNMTMerr.h"


static BOOLEAN ScmSadrGet(UINT16 *pw_scmSadr);

/**
* @brief This function initiates the SNMT Service Request UDID Request to request the
* physical address (UDID) of a specific SN inside the Safety Domain.
*
* This SN is specified by its logical address (SADR). The function is used by the SCM.
* Requested data is transferred from the SNMT Master to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @see SNMTM_CheckFsmAvailable()
*
* @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum           registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr             logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @return
* - TRUE             - request transmission successfully
* - FALSE            - request transmission failed
*/
BOOLEAN SNMTM_ReqUdid(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr)
{
  BOOLEAN o_return = FALSE;             /* predefined return value */
  UINT16 w_fsmNum = k_INVALID_FSM_NUM;  /* FSM number  to process request */
  EPLS_t_FRM_HDR s_txBufHdr;            /* local frame header struct */
  /* initialize local data array to store payload data of individual request */
  UINT8 ab_reqData[EPLS_k_UDID_LEN] = {0x00u, 0x00u, 0x00u, 0x00u,
                                       0x00u, 0x00u};

  /* get SNMT Master FSM number of one free FSM
      note: a free FSM is ALWAYS in state WF_REQUEST */
  w_fsmNum = SNMTM_GetFsmFree();

  /* if NO FSM is available */
  if(w_fsmNum == k_INVALID_FSM_NUM)
  {
    /* no action defined, because the calling function has to ensure
        availability of a FSM, see function SNMTM_CheckFsmAvailable()*/
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_NO_FSM_AVAIL_REQ_UDID,
                  SERR_k_NO_ADD_INFO);
  }
  else  /* SNMT Master FSM is available */
  {
    /* if access to the SCM SADR succeeded */
    if (ScmSadrGet(&s_txBufHdr.w_tadr))
    {
      /* collect header info and store it into modul global data struct */
      s_txBufHdr.w_adr = w_sadr;
      s_txBufHdr.w_sdn = SDN_GetSdn(EPLS_k_SCM_INST_NUM);
      s_txBufHdr.b_id  = SNMT_k_UDID_REQ | SNMT_k_FRM_TYPE;
      s_txBufHdr.b_le  = k_REQ_NO_DATA_LEN ;
      s_txBufHdr.w_ct  = EPLS_k_CT_NOT_USED;
      s_txBufHdr.b_tr  = EPLS_k_TR_NOT_USED;

      /* NO request payload data to assemble for this request */

      /* process request */
      o_return = SNMTM_ProcessFsm(dw_ct, (UINT8 *)NULL, w_fsmNum,
                                  k_EVT_REQ_SEND, w_regNum, &s_txBufHdr,
                                  ab_reqData);
    }
    /* no else : access to the SCM SADR failed */
  }

  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function initiates the SNMT Service Request SNMT SADR assignment to
* assigning a logical address (SADR) to a specific SN inside the Safety Domain.
*
* The function is used by the SCM. Requested data is transferred from the SNMT Master
* to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @param      dw_ct 			   consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param      w_regNum             registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param      w_sadr               logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param      pb_udid              reference to the physical address (UDID)(checked), valid range: <> NULL
*
* @return
* - TRUE             - request transmission successfully
* - FALSE            - request transmission failed
*/
BOOLEAN SNMTM_ReqAssgnSadr(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr,
                           const UINT8 *pb_udid)
{
  BOOLEAN o_return = FALSE ;            /* predefined return value */
  UINT16 w_fsmNum = k_INVALID_FSM_NUM;  /* FSM number  to process request */
  EPLS_t_FRM_HDR s_txBufHdr;            /* local frame header struct */
  /* initialize local data array to store payload data of individual request */
  UINT8 ab_reqData[EPLS_k_UDID_LEN] = { 0x00u, 0x00u, 0x00u, 0x00u,
                                        0x00u, 0x00u};


  /* if reference to UDID is NOT valid */
  if(pb_udid == NULL)
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_REF_REQ_DATA_ASS_SADR_INV,
                  SERR_k_NO_ADD_INFO);
  }
  else  /* reference to UDID is valid */
  {
    /* get SNMT Master FSM number of one free FSM
       note: a free FSM is ALWAYS in state WF_REQUEST */
    w_fsmNum = SNMTM_GetFsmFree();

    /* if NO FSM is available */
    if(w_fsmNum == k_INVALID_FSM_NUM)
    {
      /* no action defined, because the calling function has to ensure
         availability of a FSM, see function SNMTM_CheckFsmAvailable()*/
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_NO_FSM_AVAIL_ASS_SADR,
                    SERR_k_NO_ADD_INFO);
    }
    else  /* SNMT Master FSM is available */
    {
      /* if access to the SCM SADR succeeded */
      if (ScmSadrGet(&s_txBufHdr.w_tadr))
      {
        /* collect header info and store it into modul global data struct */
        s_txBufHdr.w_adr = w_sadr;
        s_txBufHdr.w_sdn = SDN_GetSdn(EPLS_k_SCM_INST_NUM);
        s_txBufHdr.b_id  = SNMT_k_ASS_SADR | SNMT_k_FRM_TYPE;
        s_txBufHdr.b_le  = EPLS_k_UDID_LEN;
        s_txBufHdr.w_ct  = EPLS_k_CT_NOT_USED;
        s_txBufHdr.b_tr  = EPLS_k_TR_NOT_USED;

        /* assemble UDID as request payload data */
        SFS_NET_CPY_DOMSTR(ab_reqData, pb_udid, EPLS_k_UDID_LEN);

        /* process request */
        o_return = SNMTM_ProcessFsm(dw_ct, (UINT8 *)NULL, w_fsmNum,
                                    k_EVT_REQ_SEND, w_regNum, &s_txBufHdr,
                                    ab_reqData);
      }
      /* no else : access to the SCM SADR failed */
    }
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function initiates the SNMT Service Request UDID of the SCM assignment
* to assign the UDID of the SCM.
*
* The function is called by the SCM. Requested data is transferred from the SNMT Master
* to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @see							   SNMTM_CheckFsmAvailable()
*
* @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum           registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr             logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        pb_udidScm         reference to the physical address (UDID) of the SCM (not checked, only called with reference to array in SCM_WfAssignSadrResp2() and SCM_WfAssignSadrResp()), valid range: <> NULL
*
* @return
* - TRUE             - request transmission successfully
* - FALSE            - request transmission failed
*/
BOOLEAN SNMTM_ReqAssgnScmUdid(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr,
                              const UINT8 *pb_udidScm)
{
  BOOLEAN o_return = FALSE;             /* predefined return value */
  UINT16 w_fsmNum = k_INVALID_FSM_NUM;  /* FSM number  to process request */
  EPLS_t_FRM_HDR s_txBufHdr;            /* local frame header struct */
  UINT8 b_saCmd = (UINT8)SNMT_k_ASSGN_UDID_SCM; /* set SOD Access Command */
  /* initialize local data array to store payload data of individual request */
  UINT8 ab_reqData[SNMT_k_LEN_SERV_CMD + EPLS_k_UDID_LEN] = {0x00u,
                                               0x00u, 0x00u, 0x00u, 0x00u,
                                               0x00u, 0x00u};

  /* get SNMT Master FSM number of one free FSM
      note: a free FSM is ALWAYS in state WF_REQUEST */
  w_fsmNum = SNMTM_GetFsmFree();

  /* if NO FSM is available */
  if(w_fsmNum == k_INVALID_FSM_NUM)
  {
    /* no action defined, because the calling function has to ensure
        availability of a FSM, see function SNMTM_CheckFsmAvailable()*/
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_NO_FSM_AVAIL_REQ_SCM_UDID,
                  SERR_k_NO_ADD_INFO);
  }
  else  /* SNMT Master FSM is available */
  {
    /* if access to the SCM SADR succeeded */
    if (ScmSadrGet(&s_txBufHdr.w_tadr))
    {
      /* collect header info and store it into modul global data struct */
      s_txBufHdr.w_adr = w_sadr;
      s_txBufHdr.w_sdn = SDN_GetSdn(EPLS_k_SCM_INST_NUM);
      s_txBufHdr.b_id  = SNMT_k_EXT_SER_REQ | SNMT_k_FRM_TYPE;
      s_txBufHdr.b_le  = SNMT_k_LEN_SERV_CMD + EPLS_k_UDID_LEN;
      s_txBufHdr.w_ct  = EPLS_k_CT_NOT_USED;
      s_txBufHdr.b_tr  = EPLS_k_TR_NOT_USED;

      /* copy the service command byte into the request buffer */
      SFS_NET_CPY8(&ab_reqData[SNMT_k_OFS_SERV_CMD], &b_saCmd);
      /* copy the UDID of the SCM into the request buffer */
      SFS_NET_CPY_DOMSTR(&ab_reqData[SNMT_k_OFS_UDID_SCM], pb_udidScm,
                         EPLS_k_UDID_LEN);

      /* process request */
      o_return = SNMTM_ProcessFsm(dw_ct, (UINT8 *)NULL, w_fsmNum,
                                  k_EVT_REQ_SEND, w_regNum, &s_txBufHdr,
                                  ab_reqData);
    }
    /* no else : access to the SCM SADR failed */
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function initiates the initialization of the extended CT value.
*
* The function is called by the SCM. Requested data is transferred from the SNMT Master
* to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @see SNMTM_CheckFsmAvailable()
*
* @param        dw_ct              consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum           registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr             logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        pb_extCt           reference to the extended CT value of the SCM (not checked, only called with reference to array in SCM_WfAssignSadrResp2() and SCM_WfAssignSadrResp()), valid range: <> NULL
*
* @return
* - TRUE             - request transmission successfully
* - FALSE            - request transmission failed
*/
BOOLEAN SNMTM_ReqInitializeCtSn(UINT32 const dw_ct, UINT16 const w_regNum, UINT16 const w_sadr,
                               UINT8 const * const pb_extCt)
{
  BOOLEAN o_return = FALSE;             /* predefined return value */
  UINT16 w_fsmNum = k_INVALID_FSM_NUM;  /* FSM number  to process request */
  EPLS_t_FRM_HDR s_txBufHdr;            /* local frame header struct */
  UINT8 b_saCmd = (UINT8)SNMT_k_INITIALIZE_EXT_CT; /* set SOD Access Command */
  /* initialize local data array to store payload data of individual request */
  UINT8 ab_reqData[SNMT_k_LEN_SERV_CMD + EPLS_k_LEN_EXT_CT] = {0x00u,
                                               0x00u, 0x00u, 0x00u, 0x00u,
                                               0x00u};

  /* get SNMT Master FSM number of one free FSM
      note: a free FSM is ALWAYS in state WF_REQUEST */
  w_fsmNum = SNMTM_GetFsmFree();

  /* if NO FSM is available */
  if(w_fsmNum == k_INVALID_FSM_NUM)
  {
    /* no action defined, because the calling function has to ensure
        availability of a FSM, see function SNMTM_CheckFsmAvailable()*/
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_NO_FSM_AVAIL_INIT_CT,
                  SERR_k_NO_ADD_INFO);
  }
  else  /* SNMT Master FSM is available */
  {
    /* if access to the SCM SADR succeeded */
    if (ScmSadrGet(&s_txBufHdr.w_tadr))
    {
      /* collect header info and store it into module global data struct */
      s_txBufHdr.w_adr = w_sadr;
      s_txBufHdr.w_sdn = SDN_GetSdn(EPLS_k_SCM_INST_NUM);
      s_txBufHdr.b_id  = SNMT_k_EXT_SER_REQ | SNMT_k_FRM_TYPE;
      s_txBufHdr.b_le  = SNMT_k_LEN_SERV_CMD + EPLS_k_LEN_EXT_CT;
      s_txBufHdr.w_ct  = EPLS_k_CT_NOT_USED;
      s_txBufHdr.b_tr  = EPLS_k_TR_NOT_USED;

      /* copy the service command byte into the request buffer */
      SFS_NET_CPY8(&ab_reqData[SNMT_k_OFS_SERV_CMD], &b_saCmd);
      /* copy the initial CT value into the request buffer */
      SFS_NET_CPY_DOMSTR(&ab_reqData[SNMT_k_OFS_EXT_CT], pb_extCt,
          EPLS_k_LEN_EXT_CT);

      /* process request */
      o_return = SNMTM_ProcessFsm(dw_ct, (UINT8 *)NULL, w_fsmNum,
                                  k_EVT_REQ_SEND, w_regNum, &s_txBufHdr,
                                  ab_reqData);
    }
    /* no else : access to the SCM SADR failed */
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function initiates the extended SNMT Service Request "Additional SADR
* assignment" to assigning additional logical addresses (SADR) to a specific SN inside
* the Safety Domain.
*
* The SN is addressed by its physical address (UDID). The function is used by the SCM.
* Requested data is transferred from the SNMT Master to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @param        dw_ct 		 		  consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum              registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr                logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        w_sadrAssgn 		  SADR to be assigned to the specified SN (checked), valid range: 2 .. 1023
*
* @param        w_spdoNumAssgn        TxSPDO number to be assigned to the additional SADR (checked), valid range: 2 .. 1023
*
* @return
* - TRUE                - request transmission successfully
* - FALSE               - request transmission failed
*/
BOOLEAN SNMTM_ReqAssgnAddSadr(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr,
                              UINT16 w_sadrAssgn, UINT16 w_spdoNumAssgn)
{
  BOOLEAN o_return = FALSE ;                /* predefined return value */
  EPLS_t_FRM_HDR s_txBufHdr;                /* local frame header struct */
  UINT16 w_fsmNum = k_INVALID_FSM_NUM;      /* FSM number  to process request */
  UINT8 b_saCmd = (UINT8)SNMT_k_ASSGN_ADD_SADR; /* set SOD Access Command */
  /* initialize local data array to store payload data of individual request */
  UINT8 ab_reqData[SNMT_k_LEN_SERV_CMD + SNMT_k_LEN_SADR + SNMT_k_LEN_TXSPDO] =
        { 0x00u, 0x00u, 0x00u, 0x00u, 0x00u };

  /* if additional SADR is NOT valid */
  if((w_sadrAssgn < EPLS_k_MIN_ADD_SADR) || (w_sadrAssgn > EPLS_k_MAX_ADD_SADR))
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_ADD_SADR_INV,
                  (UINT32)w_sadrAssgn);
  }
  /* else if number of SPDO is NOT valid */
  else if((w_spdoNumAssgn < EPLS_k_MIN_SPDO_NUM) ||
          (w_spdoNumAssgn > EPLS_k_MAX_SPDO_NUM))
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_SPDO_NUM_INV,
                  (UINT32)w_spdoNumAssgn);
  }
  else  /* state transition is valid */
  {
    /* get SNMT Master FSM number of one free FSM
       note: a free FSM is ALWAYS in state WF_REQUEST */
    w_fsmNum = SNMTM_GetFsmFree();

    /* if NO FSM is available */
    if(w_fsmNum == k_INVALID_FSM_NUM)
    {
      /* no action defined, because the calling function has to ensure
         availability of a FSM, see function SNMTM_CheckFsmAvailable()*/
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_NO_FSM_AVAIL_ASS_ADD_SADR,
                    SERR_k_NO_ADD_INFO);
    }
    else  /* SNMT Master FSM is available */
    {
      /* if access to the SCM SADR succeeded */
      if (ScmSadrGet(&s_txBufHdr.w_tadr))
      {
        /* collect header info and store it into modul global data struct */
        s_txBufHdr.w_adr = w_sadr;
        s_txBufHdr.w_sdn = SDN_GetSdn(EPLS_k_SCM_INST_NUM);
        s_txBufHdr.b_id  = SNMT_k_EXT_SER_REQ | SNMT_k_FRM_TYPE;
        s_txBufHdr.b_le  = SNMT_k_LEN_SERV_CMD + SNMT_k_LEN_SADR +
                          SNMT_k_LEN_TXSPDO;
        s_txBufHdr.w_ct  = EPLS_k_CT_NOT_USED;
        s_txBufHdr.b_tr  = EPLS_k_TR_NOT_USED;

        /* assemble payload data: CMD */
        SFS_NET_CPY8(&ab_reqData[SNMT_k_OFS_SERV_CMD], &b_saCmd);
        SFS_NET_CPY16(&ab_reqData[SNMT_k_OFS_SADR], &w_sadrAssgn);
        SFS_NET_CPY16(&ab_reqData[SNMT_k_OFS_TXSPDO], &w_spdoNumAssgn);

        o_return = SNMTM_ProcessFsm(dw_ct, (UINT8 *)NULL, w_fsmNum,
                                    k_EVT_REQ_SEND, w_regNum, &s_txBufHdr,
                                    ab_reqData);
      }
      /* no else : access to the SCM SADR failed */
    }
  }
  SCFM_TACK_PATH();
  return o_return;
}


/**
* @brief This function initiates the extended SNMT Service Request "Node Guarding"
* to keep a specific SN inside the Safety Domain in state OPERATIONAL.
*
* The addressed SN will respond either by SNMT Service Response "SNMT_SN_status_OP"
* or "SNMT_SN_status_PRE_OP":
* - SNMT_SN_status_OP,     requested SN in operational state
* - SNMT_SN_status_PRE_OP, requested SN in preoperational state Requested data is
*   transferred from the SNMT Master to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @param        dw_ct                 consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum              registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr                logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @return
* - TRUE  - request transmission successfully
* - FALSE - request transmission failed
*/
BOOLEAN SNMTM_ReqGuarding(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr)
{
  BOOLEAN o_return = FALSE;              /* predefined return value */
  EPLS_t_FRM_HDR s_txBufHdr;              /* local frame header struct */
  UINT16 w_fsmNum = k_INVALID_FSM_NUM;    /* FSM to process request */
  UINT8 b_saCmd = (UINT8)SNMT_k_SCM_GUARD_SN; /* set SOD Access Command */
  UINT8 b_reqData = 0x00u;                /* one byte request payload data */

  /* get SNMT Master FSM number of one free FSM
      note: a free FSM is ALWAYS in state "wait for request" */
  w_fsmNum = SNMTM_GetFsmFree();

  /* if NO FSM is available */
  if(w_fsmNum == k_INVALID_FSM_NUM)
  {
    /* error: the calling function (SCM) has to ensure availability of a FSM,
              see function SNMTM_CheckFsmAvailable()*/
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_NO_FSM_AVAIL_REQ_GUARD,
                  SERR_k_NO_ADD_INFO);
  }
  else  /* SNMT Master FSM is available */
  {
    /* if access to the SCM SADR succeeded */
    if (ScmSadrGet(&s_txBufHdr.w_tadr))
    {
      /* collect header info and store it into modul global data struct */
      s_txBufHdr.w_adr = w_sadr;
      s_txBufHdr.w_sdn = SDN_GetSdn(EPLS_k_SCM_INST_NUM);
      s_txBufHdr.b_id  = SNMT_k_EXT_SER_REQ | SNMT_k_FRM_TYPE;
      s_txBufHdr.b_le  = SNMT_k_LEN_SERV_CMD;
      s_txBufHdr.w_ct  = EPLS_k_CT_NOT_USED;
      s_txBufHdr.b_tr  = EPLS_k_TR_NOT_USED;

      /* store command byte */
      SFS_NET_CPY8(&b_reqData, &b_saCmd);

      o_return =
        SNMTM_ProcessFsm(dw_ct, (UINT8 *)NULL, w_fsmNum, k_EVT_REQ_SEND,
                         w_regNum, &s_txBufHdr, &b_reqData);
    }
    /* no else : access to the SCM SADR failed */
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function initiates the extended SNMT Service Request "SN set to
* preoperational" and "SN set to operational" to handle state transitions of a
* specific SN inside the Safety Domain.
*
* A SN can be switched from preoperational to operational or from operational to
* preoperational. Both services need to be acknowledged by the SNMT Slave. Parameter
* timestamp is only used for transition to state operational. The Requested acknowledge
* is transferred from the SNMT Master to the SCM via callback.
*
* @attention The calling function has to ensure availability of a free FSM by calling
* function SNMTM_CheckFsmAvailable().
*
* @param        dw_ct                 consecutive time (not checked, any value allowed), valid range: any 32 bit value
*
* @param        w_regNum              registration number of request/response (not checked, checked in SCM_Trigger()), valid range: any 16 bit value
*
* @param        w_sadr                logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        e_snTrans             transition to be processed (checked), valid range: &lt;SNMTM_k_PREOP_TO_OP&gt;, &lt;SNMTM_k_OP_TO_PREOP&gt;
*
* @param        dw_paramTStmp         parameter timestamp, created by the configuration tool, (not checked, any value allowed), valid range: any 32 bit value
*
* @return
* - TRUE                - request transmission successfully FALSE               - request transmission failed
*/
BOOLEAN SNMTM_ReqSnTrans(UINT32 dw_ct, UINT16 w_regNum, UINT16 w_sadr,
                         SNMTM_t_SN_TRANS e_snTrans, UINT32 dw_paramTStmp)
{
  BOOLEAN o_return = FALSE;               /* predefined return value */
  UINT16 w_fsmNum = k_INVALID_FSM_NUM;    /* FSM number  to process request */
  EPLS_t_FRM_HDR s_txBufHdr;              /* local frame header struct */
  UINT8 b_saCmd = (UINT8)SNMT_k_SN_SET_TO_OP; /* set SOD Access Command */
  /* byte array for req payload data */
  UINT8 ab_reqData[SNMT_k_LEN_SERV_CMD + SNMT_k_LEN_TSTMP] =
                  {0x00U, 0x00U, 0x00U, 0x00U, 0x00U};

  /* if SN state transition in NOT valid */
  if((e_snTrans != SNMTM_k_PREOP_TO_OP) && (e_snTrans != SNMTM_k_OP_TO_PREOP))
  {
    /* error: invalid SN state transition */
    SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_SN_TRANS_INV,
                  (UINT32)e_snTrans);
  }
  else  /* SN state transition is valid */
  {
    /* get SNMT Master FSM number of one free FSM
       note: a free FSM is ALWAYS in state WF_REQUEST */
    w_fsmNum = SNMTM_GetFsmFree();

    /* if NO FSM is available */
    if(w_fsmNum == k_INVALID_FSM_NUM)
    {
      /* no action defined, because the calling function has to ensure
         availability of a FSM, see function SNMTM_CheckFsmAvailable()*/
      SERR_SetError(EPLS_k_SCM_INST_NUM_ SNMTM_k_ERR_NO_FSM_AVAIL_SN_TRANS,
                    SERR_k_NO_ADD_INFO);
    }
    else  /* SNMT Master FSM is available */
    {
      /* if access to the SCM SADR succeeded */
      if (ScmSadrGet(&s_txBufHdr.w_tadr))
      {
        /* collect header info and store it into modul global data struct */
        s_txBufHdr.w_adr = w_sadr;
        s_txBufHdr.w_sdn = SDN_GetSdn(EPLS_k_SCM_INST_NUM);
        s_txBufHdr.b_id  = SNMT_k_EXT_SER_REQ | SNMT_k_FRM_TYPE;
        s_txBufHdr.w_ct  = EPLS_k_CT_NOT_USED;
        s_txBufHdr.b_tr  = EPLS_k_TR_NOT_USED;

        /* if transition from PRE-OPERATIONAL to OPERATIONAL is requested */
        if(e_snTrans == SNMTM_k_PREOP_TO_OP)
        {
          b_saCmd = (UINT8)SNMT_k_SN_SET_TO_OP;
          s_txBufHdr.b_le = SNMT_k_LEN_SERV_CMD +
                            SNMT_k_LEN_TSTMP;

          /* store payload data */
          SFS_NET_CPY8(&ab_reqData[SNMT_k_OFS_SERV_CMD], &b_saCmd);
          SFS_NET_CPY32(&ab_reqData[SNMT_k_OFS_TSTMP], &(dw_paramTStmp));
        }
        else  /* trans. from OPERATIONAL to PRE-OPERATIONAL is requested */
        {
          /* set SOD Access Command byte */
          b_saCmd = (UINT8)SNMT_k_SN_SET_TO_PREOP;
          /* store payload data length */
          s_txBufHdr.b_le = SNMT_k_LEN_SERV_CMD;

          /* store payload data */
          SFS_NET_CPY8(&ab_reqData[SNMT_k_OFS_SERV_CMD], &b_saCmd);
        }

        o_return =
          SNMTM_ProcessFsm(dw_ct, (UINT8*)NULL, w_fsmNum, k_EVT_REQ_SEND,
            w_regNum, &s_txBufHdr, &ab_reqData[SNMT_k_OFS_SERV_CMD]);
      }
      /* no else : access to the SCM SADR failed */
    }
  }
  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function initiates the SNMT Service Request "SNMT SN ack".
*
* The service is used by the SCM to acknowledge an error reported by the SNMT Slave that
* receives the acknowledge telegram. For acknowledge the SCM returns the reported error
* back.
*
* @param        w_sadr              logical address of SN which shall response its physical address (UDID) (not checked, checked in ScmInitFsm()), valid range: EPLS_k_MIN_SADR .. EPLS_k_MAX_SADR
*
* @param        b_errorGroup        classification of error &lt;SNMT_SN_FAIL&gt;, (not checked, any value is allowed), valid range: any 8 bit value
*
* @param        b_errorCode         error code of &lt;SNMT_SN_FAIL&gt;, (not checked, any value is allowed), valid range: any 8 bit value
*
* @return
* - TRUE              - request transmission successfully
* - FALSE             - request transmission failed
*/
BOOLEAN SNMTM_SnErrorAck(UINT16 w_sadr, UINT8 b_errorGroup, UINT8 b_errorCode)
{
  BOOLEAN o_return = FALSE ;  /* predefined return value */
  UINT8 *pb_txFrame = (UINT8 *)NULL;   /* ref to memory block (SHNF) */
  EPLS_t_FRM_HDR s_txBufHdr;  /* local structure to collect frame header info */
  UINT8 b_saCmd = (UINT8)SNMT_k_SN_ACK; /* set SOD Access Command */

  /* if access to the SCM SADR succeeded */
  if (ScmSadrGet(&s_txBufHdr.w_tadr))
  {
    /* collect header info and store it into local data structure */
    s_txBufHdr.w_adr = w_sadr;
    s_txBufHdr.w_sdn = SDN_GetSdn(EPLS_k_SCM_INST_NUM);
    s_txBufHdr.b_id  = SNMT_k_EXT_SER_REQ | SNMT_k_FRM_TYPE;
    s_txBufHdr.b_le  = (SNMT_k_LEN_SERV_CMD + SNMT_k_LEN_ERR_GROUP +
                        SNMT_k_LEN_ERR_CODE);
    s_txBufHdr.w_ct  = EPLS_k_CT_NOT_USED;
    s_txBufHdr.b_tr  = EPLS_k_TR_NOT_USED;

    /* allocate a memory block of n bytes (SFS_GetMemBlock) */
    pb_txFrame =
      SFS_GetMemBlock(EPLS_k_SCM_INST_NUM_ SHNF_k_SNMT, SFS_k_NO_SPDO,
                      s_txBufHdr.b_le);

    /* if memory allocation failed */
    if(pb_txFrame == NULL)
    {
      /* error: no memory block available, return value pre
         initialized FALSE, error is already reported */
    }
    else  /* memory allocation succeeded */
    {
      /* write SACmd into response */
      SFS_NET_CPY8(&pb_txFrame[SFS_k_FRM_DATA_OFFSET + SNMT_k_OFS_SERV_CMD],
                   &b_saCmd);
      /* write error group into negative response */
      SFS_NET_CPY8(&pb_txFrame[SFS_k_FRM_DATA_OFFSET + SNMT_k_OFS_ERR_GROUP],
                   &b_errorGroup);
      /* write error code into negative response */
      SFS_NET_CPY8(&pb_txFrame[SFS_k_FRM_DATA_OFFSET + SNMT_k_OFS_ERR_CODE],
                   &b_errorCode);

      o_return =
        SFS_FrmSerialize(EPLS_k_SCM_INST_NUM_ &s_txBufHdr, pb_txFrame);
    }
  }
  /* no else : access to the SCM SADR failed */

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function returns the SADR of the SCM.
*
* @param pw_scmSadr pointer to the SADR of the SCM (pointer not checked, only
*   called with reference to variable), valid range: <> NULL
*
* @return Success indicator
* - TRUE - success
* - FALSE - failure
*/
static BOOLEAN ScmSadrGet(UINT16 *pw_scmSadr)
{
  BOOLEAN o_return = FALSE;   /* predefined return value */
  void *pv_sodScmSadr = NULL; /* local ref to the SOD entry 0x1200, 0x02,
                                 which contains the SADR of the SCM. */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get SADR of the SCM from SOD */
  pv_sodScmSadr = SOD_ReadVirt(EPLS_k_SCM_INST_NUM_ &SNMTM_s_AccessSadrScm, &s_errRes);

  /* if access to the SADR of the SCM succeeded */
  if (pv_sodScmSadr != NULL)
  {
    *pw_scmSadr = *((UINT16 *)pv_sodScmSadr);
    o_return = TRUE;
  }
  else /* access to the SADR of the SCM failed */
  {
    SERR_SetError(EPLS_k_SCM_INST_NUM_ s_errRes.w_errorCode,
                  (UINT32)(s_errRes.e_abortCode));
  }

  SCFM_TACK_PATH();
  return o_return;
}

/** @} */
