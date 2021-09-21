/**
 * @addtogroup SCM
 * @{
 * @file SCMint.h
 *
 * This module holds all unit internal definitions.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author Joachim Stolberg, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation GmbH
 *
 *
 * <h2>History for SCMint.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td> Review SL V10</td></tr>
 *     <tr><td>23.01.2013</td><td>Hans Pill</td><td>changes for additional parameters according to openSafety specification</td></tr>
 *     <tr><td>07.03.2013</td><td>Hans Pill</td><td>changes for parameter checksum being written to the SN</td></tr>
 *     <tr><td>23.03.2013</td><td>Hans Pill</td><td>changes for handling of the new timestamp and CRC domain</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>changes for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>21.07.2016</td><td>Stefan Innerhofer</td><td>changes for legacy mode time stamp</td></tr>
 *     <tr><td>06.02.2016</td><td>Stefan Innerhofer</td><td>use sadr instead of snNum to access additional parameter</td></tr>
 *     <tr><td>19.04.2017</td><td>Roman Zwischelsberger</td><td>changes for better handling of SNMT timeouts</td></tr>
 * </table>
 *
 */

#ifndef SCMINT_H
#define SCMINT_H

/***
*    include-files
***/
#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "SERR.h"
#include "SERRapi.h"
#include "SCM.h"
#include "SCMapi.h"
#include "SCMerr.h"
#include "SNMTM.h"
#include "SNMTS.h"
#include "SODerr.h"
#include "SODapi.h"
#include "SHNF.h"
#include "SFS.h"
#include "SOD.h"
#include "SDN.h"
#include "SCFMapi.h"
#include "SNMTMerr.h"
#include "SSDOCapi.h"
#include "SSDOCerr.h"

/**
 * SCM Debug Printf
 */

/* #define SCM_DEBUG */

#ifdef SCM_DEBUG
  #define SCM_PRINT3(a,b,c) DBG_PRINT3(a,b,c)
#else
  #define SCM_PRINT3(a,b,c) //DBG_PRINT3(a,b,c)
#endif

/** Size of the response buffer (max. = SNMT UDID of the SCM) */
#define SCM_k_RESP_BUFF_SIZE      (1U+EPLS_k_UDID_LEN)
/** number of FSM state functions */
#define SCM_k_NUM_CFG_FSM_STATES  27U

/**
 * @name optional features
 * @{
 */
#define SCM_k_FEATURE_40_BIT_SPDO        0x00000002UL
#define SCM_k_FEATURE_LEGACY_MODE_TS     0x80000000UL
/** @} */

/**
 * @name SOD object sub-index definitions
 * @{
 */
#define SCM_k_SUB_IDX_VENDOR_ID     ((UINT8)(0x01))
#define SCM_k_SUB_IDX_PROD_CODE     ((UINT8)(0x02))
#define SCM_k_SUB_IDX_REV_NUM       ((UINT8)(0x03))
#define SCM_k_SUB_IDX_CHECK_SUM     ((UINT8)(0x06))
#define SCM_k_SUB_IDX_DVI_TIMESTAMP ((UINT8)(0x07))
#define SCM_k_SUB_IDX_DVI_CHECK_SUM ((UINT8)(0x0E))

#define	SCM_k_IDX_DVI               ((UINT16)(0xC400U))
/** @} */

/**
 * @name ConfigFSM events (bit coded)
 * @{
 */
/** default value */
#define SCM_k_EVT_NO_EVT_OCCURRED ((UINT16)  0)
/** event for transient states */
#define SCM_k_EVT_GENERIC_EVENT   ((UINT16)  1)
 /** valid response received */
#define SCM_k_EVT_SSDOC_RESP_RX   ((UINT16)  2)
 /** valid response received */
#define SCM_k_EVT_SNMTM_RESP_RX   ((UINT16)  4)
 /** operator or application ACK*/
#define SCM_k_EVT_ACK_RECEIVED    ((UINT16)  8)
 /** SNMTM timeout occurred */
#define SCM_k_EVT_SNMTM_TIMEOUT   ((UINT16) 16)
 /** SSDOC timeout occurred */
#define SCM_k_EVT_SSDOC_TIMEOUT   ((UINT16) 32)
 /** response error occurred */
#define SCM_k_EVT_RESP_ERROR      ((UINT16) 64)
 /** guarding or poll timeout */
#define SCM_k_EVT_TIMEOUT         ((UINT16)128)
 /** reset node guarding */
#define SCM_k_EVT_RESET_NODE_GRD  ((UINT16)256)
/** @} */

/**
 * @name macros for SSDOC services (index , sub-index)
 * @{
 */
#define SCM_k_IDX_VENDOR_ID_REQ  (UINT16) EPLS_k_IDX_DEVICE_VEN_ID, 1U
#define SCM_k_IDX_PARA_SET_REQ   (UINT16) 0x101A, 0U
/** @} */

/**
 * @name macros for local SOD access
 * @{
 */
#define SCM_k_IDX_GUARD_TIME  EPLS_k_IDX_LIFE_GUARDING, EPLS_k_SUBIDX_GUARD_TIME
#define SCM_k_IDX_LIFE_TIME_FACTOR  EPLS_k_IDX_LIFE_GUARDING, EPLS_k_SUBIDX_LIFE_TIME_FACTOR
#define SCM_k_IDX_CONFIG_MODE           (UINT16) 0x101B, (UINT8)1
#define SCM_k_IDX_SADR(snNum)           (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)1
#define SCM_k_IDX_VENDOR_ID(snNum)      (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)2
#define SCM_k_IDX_PROD_CODE(snNum)      (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)3
#define SCM_k_IDX_REV_NUM(snNum)        (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)4
#define SCM_k_IDX_SN_STATUS(snNum)      (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)5
#define SCM_k_IDX_CHECKSUM(snNum)       (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)6
#define SCM_k_IDX_TIMESTAMP(snNum)      (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)7
#define SCM_k_IDX_MAX_SSDO_PAYLD(snNum) (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)8
#define SCM_k_IDX_POLL_INTERVALL(snNum) (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)9
#define SCM_k_IDX_PARA_LEN(snNum)       (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)10
#define SCM_k_IDX_PARA_SET(snNum)       (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)11
#define SCM_k_IDX_OPT_FEAT(snNum)       (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)12
#define SCM_k_IDX_PARA_CHKSM(snNum)     (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)14
#define SCM_k_IDX_REM_PARA_CHKSM(snNum) (UINT16)(SCM_k_IDX_DVI + (snNum)), (UINT8)15
#define SCM_k_IDX_UDID_LIST(sAdr, sIdx) (UINT16)(0xCC00U + (sAdr)), \
                                        ((UINT8)(sIdx))
#define SCM_k_IDX_ADD_PARA_SET(sadr,errCode)	(UINT16)(0xE400 + (sadr) - 1), \
                                        (UINT8)(((errCode) & SNMTS_k_FAIL_ERR_ADD_IDX_MASK) + 1)
#define SCM_k_ADD_PARA_ERR_INFO(sadr,errCode)	((UINT32)(0xE400 + (sadr) - 1) | \
                                        ((UINT32)(((errCode) & SNMTS_k_FAIL_ERR_ADD_IDX_MASK) + 1) << 16))
/** @} */

/**
 * @name SCM Configuration Modes (Object 101Bh: SCM Parameters)
 * @{
 */
/** ACM */
#define k_SCM_ACM                 0U
/** MCM */
#define k_SCM_MCM                 1U

/**
 * @name Necessary preconditions for the call of a FSM state function
 * @{
 *
 * These defines are used in the SCM unit test
 */
/** no precondition necessary */
#define  k_NO_SLOT      0U
/** free SNMTM slot is necessary */
#define  k_SNMTM_SLOT   1U
/** free SSDOC slot is necessary */
#define  k_SSDOC_SLOT   2U
/** both slots need to be free */
#define  k_SSDOC_SNMTM_SLOT    3u
/** @} */

/**
 * States of the SCM FSM
 */
typedef enum{
  /* Sub-FSM Operational */
  SCM_k_SEND_ASSIGN_SADR_REQ    = 0,
  SCM_k_WF_ASSIGN_SADR_RESP     = 1,
  SCM_k_WF_ASSIGN_SCM_UDID_RESP = 2,
  SCM_k_WF_UDID_RESP            = 3,
  SCM_k_WF_INIT_CT_RESP         = 4,
  SCM_k_WF_OPERATOR_ACK         = 5,
  SCM_k_IDLE2                   = 6,
  /* Sub-FSM Verify DVI */
  SCM_k_WF_VENDOR_ID_RESP       = 7,
  SCM_k_WF_PRODUCT_CODE_RESP    = 8,
  SCM_k_WF_REVISION_NUMBER_RESP = 9,
  /* Sub-FSM Verify Parameters */
  SCM_k_WF_TIMESTAMP            = 10,
  /* Sub-FSM Download Parameters */
  SCM_k_WF_PRE_OP_RESP          = 11,
  SCM_k_WF_PARAM_DL_RESP        = 12,
  SCM_k_ASSIGN_ADD_SADR         = 13,
  SCM_k_WF_ADD_SADR_RESP        = 14,
  /* Sub-FSM Handle Single UDID Mismatch */
  SCM_k_WF_ASSIGN_SADR_RESP2    = 15,
  SCM_k_VERIFY_UNIQ_UDID        = 16,
  /* Sub-FSM Activate SN */
  SCM_k_SEND_PUT_TO_OP          = 17,
  SCM_k_SEND_READ_TIMESTAMP     = 18,
  SCM_k_WF_OP_RESP              = 19,
  SCM_k_WF_POLL_TIMEOUT         = 20,
  SCM_k_WF_SAPL_ACK             = 21,
  /* Sub-FSM Node Guarding */
  SCM_k_WF_GUARD_TIMER          = 22,
  SCM_k_WF_GUARD_RESP           = 23,
  /* Sub-FSM Additional Parameters */
  SCM_k_SEND_SAPL_ACK           = 24,
  SCM_k_SEND_ADD_PAR            = 25,
  SCM_k_WF_ADD_PAR_DL_RESP      = 26
  /* RSM_IGNORE_QUALITY_END */
}SCM_t_FSM_STATE;

/**
 * Control block structure of the SCM state machine
 */
typedef struct
{
  /** Variables for the SOD objects **/

  /** pointer to the UDID of the UDID list (0xCC01, 0x01) in the SOD */
  UINT8  *pb_snUdid;
  /**
   * Is the UDID already in use
   *
   * - TRUE  : if the UDID is already used by an SN
   * - FALSE : if the UDID is not used by any SN
   */
  BOOLEAN  o_udidUsed;
  /** SADR of the SN, SADR_U16 (0xC400, 0x01) */
  UINT16   w_sadr;
  /** MaximumSsdoPayloadLen_U16 (0xC400, 0x08) */
  UINT16   w_payLd;
  /** SnmtCrcPollInterval_U32 (0xC400, 0x09) */
  UINT32  dw_pollInterval;

  /** Variables to store the state machine information for a SN **/

  /** FSM state */
  SCM_t_FSM_STATE  e_state;
  /**
   * Occurred events.
   *
   * If an event occurred then only the corresponding bit is set in this word.
   * Normally, only one bit may be set, the other bits must be reset. Therefore
   * the whole w_event word (not only one bit) is checked and reset or set in
   * the SCM state functions. If two or more bits are set then no event bit is
   * reset or set in the SCM state functions and a fail safe error is generated
   * in ScmFsmProcess(). Except the SCM_k_WF_OPERATOR_ACK state (see in
   * SCM_WfOperatorAck())
   */
  UINT16   w_event;
  /** Safety Node status */
  UINT8    b_snStatus;
  /**
   * Report change of Safety Node status
   *
   * - TRUE  : Unchanged Safety Node status shall be reported.
   * - FALSE : Unchanged Safety Node status is not reported.
   */
  BOOLEAN  o_reportSnSts;
  /** variable to check timeout of a single SNMT request */
  UINT32   dw_timer;
  /** variable to check total life time timeout */
  UINT32   dw_timerTotal;
  /** variable to store the last assigned additional SADR */
  UINT16   w_addSadr;

  /** Error Group Variable for auto acknowledge of SN_Fail */
  UINT8 b_errGroup;
  /** Error Code Variable for auto acknowledge of SN_Fail */
  UINT8 b_errCode;

  /** Variables to store the SN response information **/

  /** Used to store SSDOC abort codes */
  UINT32  dw_SsdocAbortCode;
  /** Length of the uploaded data */
  UINT32  dw_SsdocUploadLen;
  /** TADR of received responses */
  UINT16   w_tadr;
  /** SDN of received responses */
  UINT16   w_rxSdn;
  /** response buffer to enforce UINT32 alignment */
  UINT32 adw_respBuff[(SCM_k_RESP_BUFF_SIZE/4U)+1U];
  /** for UDID_MISMATCH handling, enforce UINT32 alignment */
  UINT32 adw_newUdid[(SCM_k_RESP_BUFF_SIZE/4U)+1U];
  /** initial CT value */
  UINT8 ab_initCtVal[EPLS_k_LEN_EXT_CT];

  /** variables for comparing the timestamp checksum domain */

  /** pointer where to store the remote timestamp crc domain */
  UINT8* pb_remTimeCrc;
  /** maximum allowed length for the remote timestamp and crc domain */
  UINT32 dw_maxRemTimeCrcLen;
}SCM_t_FSM_CB;

/**
 * Definition of a FSM state function.
 *
 * All FSM state functions must correspond to this prototype because they are listed
 * in apf_FsmStateFunctions
 */
typedef BOOLEAN (*t_STS_FUNC)(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                              UINT32 dw_ct);

/** Definition of a FSM state function with pre-conditions attributes */
typedef struct{
  /** necessary state function pre-conditions */
  UINT8 b_preCon;
  /** function pointer to the state function */
  t_STS_FUNC pf_state;
}t_FSM_FUNC;

/** Safety Node Guarding Time */
extern UINT32 SCM_dw_SnGuardTime SAFE_NO_INIT_SEKTOR;
extern UINT32 SCM_dw_SnGuardTimeTotal SAFE_NO_INIT_SEKTOR;

/**
 * Selected SCM Configuration Mode
 *
 * @see k_SCM_ACM
 * @see k_SCM_MCM
 */
extern UINT8 SCM_b_ConfigMode SAFE_NO_INIT_SEKTOR;
extern UINT64 SCM_ddw_initCt SAFE_NO_INIT_SEKTOR;

/** Array of FSM control blocks */
extern SCM_t_FSM_CB SCM_as_FsmCb[SCM_cfg_MAX_NUM_OF_NODES] SAFE_NO_INIT_SEKTOR;

/**
* @brief This function builds a local list with main-SADRs from the "Additional SADR List".
*
* This list is necessary for the "Additional SADR Assignment" process. All UDID are marked
* as not used.
*
* @return
* - FALSE - Abort forcing error
*/
BOOLEAN SCM_BuildLocalSadrList(void);

/**
* @brief Check in the SADR-UDID-List (index CC01 + x, subindex of the active UDID) if the
* given UDID is not already assigned.
*
* This function is only called in case of ACM (one device single UDID for each SN with a
* specific SADR), therefore only sub-index 1 is checked in the SADR-UDID-List.
*
* @param	pb_newUdid UDID to be checked. (not checked, only called with reference to
* 	array in SCM_VerifyUniqUdid())
*
* @return
* - TRUE                - UDID is unique
* - FALSE               - UDID is not unique
*/
BOOLEAN SCM_UniqueUdid(const UINT8 *pb_newUdid);

/**
* @brief This function searches in the local SADR-List for the next g1iven main SADR value
* and returns the corresponding additional SADR.
*
* Because one SN can have more than one additional SADR, the function must be called until
* zero is returned. The searches through the complete list until the end of the list is
* reached.
*
* @param	w_mainSadr		Main SADR (not checked, checked in ScmInitFsm()).
* @param    w_addSadr		Old "Additional SADR" as current search position.
* 	Must be zero at the beginning (not checked, checked in SCM_WfParamDlResp()).
*
* @return
* - > 0:   - valid additional SADR
* - = 0:   - end of list reached
*/
UINT16 SCM_GetNextAddSadr(UINT16 w_mainSadr, UINT16 w_addSadr);

/**
 * @name SCM FSM state functions
 * @{
 *
* @brief The following functions represent the states of the finite state machine
* ScmFsmProcess().
*
* The documentation for this internal functions can be found at their respective
* implementations.
*
*/
/* Sub-FSM Operational */
BOOLEAN SCM_SendAssignSadrReq(SCM_t_FSM_CB *ps_fsmCb,
                              UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfAssignSadrResp(SCM_t_FSM_CB *ps_fsmCb,
                             UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfAssignScmUdidResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                                UINT32 dw_ct);
BOOLEAN SCM_WfInitExtCtResp(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                                UINT32 dw_ct);
BOOLEAN SCM_WfUdidResp(SCM_t_FSM_CB *ps_fsmCb,
                       UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfOperatorAck(SCM_t_FSM_CB *ps_fsmCb,
                          UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_Idle2(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum, UINT32 dw_ct);
/* Sub-FSM Verify DVI */
BOOLEAN SCM_WfVendorIdResp(SCM_t_FSM_CB *ps_fsmCb,
                           UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfProductCodeResp(SCM_t_FSM_CB *ps_fsmCb,
                              UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfRevisionNumberResp(SCM_t_FSM_CB *ps_fsmCb,
                                 UINT16 w_snNum, UINT32 dw_ct);
/* Sub-FSM Verify Parameters */
BOOLEAN SCM_WfTimestamp(SCM_t_FSM_CB *ps_fsmCb,
                        UINT16 w_snNum, UINT32 dw_ct);
/* Sub-FSM Download Parameters */
BOOLEAN SCM_WfPreOpResp(SCM_t_FSM_CB *ps_fsmCb,
                        UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfParamDlResp(SCM_t_FSM_CB *ps_fsmCb,
                          UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_AssignAddSadr(SCM_t_FSM_CB *ps_fsmCb,
                          UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfAddSadrResp(SCM_t_FSM_CB *ps_fsmCb,
                          UINT16 w_snNum, UINT32 dw_ct);
/* Sub-FSM Handle Single UDID Mismatch */
BOOLEAN SCM_WfAssignSadrResp2(SCM_t_FSM_CB *ps_fsmCb,
                              UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_VerifyUniqUdid(SCM_t_FSM_CB *ps_fsmCb,
                           UINT16 w_snNum, UINT32 dw_ct);
/* Sub-FSM Activate SN */
BOOLEAN SCM_SendPutToOp(SCM_t_FSM_CB *ps_fsmCb,
                        UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_SendReadTimestamp(SCM_t_FSM_CB *ps_fsmCb,
                        UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfOpResp(SCM_t_FSM_CB *ps_fsmCb,
                      UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfPollTimeout(SCM_t_FSM_CB *ps_fsmCb,
                          UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfSaplAck(SCM_t_FSM_CB *ps_fsmCb,
                      UINT16 w_snNum, UINT32 dw_ct);
/* Sub-FSM Node Guarding */
BOOLEAN SCM_WfGuardTimer(SCM_t_FSM_CB *ps_fsmCb,
                         UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfGuardResp(SCM_t_FSM_CB *ps_fsmCb,
                        UINT16 w_snNum, UINT32 dw_ct);
/* Sub-FSM Additional Parameters */
BOOLEAN SCM_SendSaplAck(SCM_t_FSM_CB *ps_fsmCb,
		                 UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_SendAddParam(SCM_t_FSM_CB *ps_fsmCb,
		                 UINT16 w_snNum, UINT32 dw_ct);
BOOLEAN SCM_WfAddParamResp(SCM_t_FSM_CB *ps_fsmCb,
		                 UINT16 w_snNum, UINT32 dw_ct);
/** @} */

/**
* @brief SCM unit local function used by the FSM state functions to simplify the SOD access.
*
* @param        w_idx 				 SOD index (not checked, checked in SOD_AttrGet()).
*
* @param        b_subidx 			 SOD sub-index (not checked, checked in SOD_AttrGet()).
*
* @return
* - <> NULL    - pointer to the SOD entry
* - == NULL    - SOD entry not available or failure
*/
void *SCM_SodRead(UINT16 w_idx, UINT8 b_subidx);

/**
 * @brief SCM unit local function used by the FSM state functions to simplify the
 * SOD access.
 *
 * @param w_idx SOD index (not checked, checked in SOD_AttrGet ).
 * @param b_subidx SOD sub-index (not checked, checked in SOD_AttrGet ).
 *
 * @see SOD_AttrGet
 *
 * @return
 * - <> NULL - pointer to the SOD entry
 * - == NULL - SOD entry not available or failure
 */
UINT32 SCM_SodGetActLen(UINT16 w_idx, UINT8 b_subidx);

/**
* @brief This function gets a pointer to the given object.
*
* The given object can be accessed trough this pointer, therefore it is checked that no
* callback function is implemented.
*
* @param	w_idx		SOD index (not checked, checked in SOD_AttrGet()).
*
* @param	b_subidx	SOD sub-index (not checked, checked in SOD_AttrGet()).
*
* @return
* - <> NULL    - pointer to the SOD entry
* - == NULL    - SOD entry not available or failure
*/
void *SCM_SodPtrAcs(UINT16 w_idx, UINT8 b_subidx);

/**
* @brief SCM unit local function used by the FSM state functions to
*               entry is available or not.
*
* @param      w_idx			SOD index (not checked, checked in SOD_AttrGet()).
*
* @param      b_subidx		SOD sub-index (not checked, checked in SOD_AttrGet()).
*
* @retval        po_error		If the return value is NULL, this out parameter signals if an error is occurred (TRUE) or if the entry is not available (FALSE), (not checked, only called with reference to variable).
*
* @return
* - <> NULL    - pointer to the SOD entry
* - == NULL    - SOD entry not available or error occurred
*/
void *SCM_SodQueryRead(UINT16 w_idx, UINT8 b_subidx, BOOLEAN *po_error);

/**
* @brief SCM unit local function used by the FSM state functions to simplify the SOD access.
*
* Used to write a domain to the SOD.
*
* @param        w_idx            SOD index (not checked, checked in SOD_AttrGet()).
*
* @param        b_subidx         SOD sub-index (not checked, checked in SOD_AttrGet()).
*
* @param        pb_data          pointer to the data (not checked, checked in SOD_Write()).
*
* @return
* - FALSE     - Abort forcing error
*/
BOOLEAN SCM_SodWrite(UINT16 w_idx, UINT8 b_subidx, const UINT8 *pb_data);

/**
* @brief This function sets the given status in the SOD.
*
* If the status is changed (compared to the local FSM control block) or the unchanged
* status shall be signaled, an application callback function is additionally called.
*
* @param        ps_fsmCb             Pointer to the current slot of the FSM control block (not checked, only called with reference to struct).
*
* @param        w_snNum              Safety Node number (not checked, checked in SCM_Trigger()). Valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param        b_nodeStatus         node status (not checked, only called with define).
*
* @param        o_reportSnSts
* - TRUE  : Unchanged SN status shall be reported.
* - FALSE : Unchanged SN status is not reported. (not checked, any value allowed)
*
* @return
* - FALSE     - Abort forcing error
*/
BOOLEAN SCM_SetNodeStatus(SCM_t_FSM_CB *ps_fsmCb, UINT16 w_snNum,
                          UINT8 b_nodeStatus, BOOLEAN o_reportSnSts);

/**
* @brief This function is a callback function used by the SSDOC.
*
* This function will be called for every SSDOC response frame based on a request
* initiated by the SCM. The response is copied to the address specified via the
* request service function of the SSDOC and is not part of the following parameters.
*
* @param	w_regNum		process handle, used to address the corresponding FSM slot (not checked checked in SCM_Trigger()). valid range: < SCM_cfg_MAX_NUM_OF_NODES
*
* @param    dw_dataLen		 Number of received data bytes(not used). valid range: 0..8
*
* @param    dw_abortCode
* - == SOD_ABT_NO_ERROR: no error present reponse buffer and dw_transDataLen are valid
* - <> SOD_ABT_NO_ERROR: error present, dw_abortCode contains the abort code, reponse buffer and dw_transDataLen are not relevant (checked)
*/
void SCM_SsdocClbk(UINT16 w_regNum, UINT32 dw_dataLen, UINT32 dw_abortCode);

/**
* @brief Store the free frames pointer for later use.
*
* @see	  SCM_NumFreeFrmsDec()
*
* @param  pb_numFreeFrms	Pointer to the number of free frames. (not checked, checked in SCM_Trigger())
*/
void SCM_NumFreeFrmsSet(UINT8 *pb_numFreeFrms);

/**
* @brief Decrement the value behind the stored pb_numFreeFrms.
*
* @see SCM_NumFreeFrmsSet()
*/
void SCM_NumFreeFrmsDec(void);

/**
* @brief This function is only used for testing and gets the pointer to the as_FsmStateFunctions static array.
*
* @return       pointer to the as_FsmStateFunctions array
*/
t_FSM_FUNC *SCM_GetFsmStateFunctionsPtr(void);

/**
* @brief This function is only used for testing and gets the pointer to the w_SnRealNum static variable.
*
* @return       pointer to the w_SnRealNum static variable
*/
UINT16 *SCM_GetPtrTo_w_SnRealNum(void);

#endif

/** @} */
