/**
 * @addtogroup SNMTS
 * @{
 *
 * @file SNMTSresponse.c
 *             This file contains all functions to generate a SNMTS response
 *             and to access the Main SADR in the SOD.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * <h2>History for SNMTSresponse.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>added SNMT command for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 * </table>
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
#include "SNMTSapi.h"
#include "SNMT.h"
#include "SNMTS.h"
#include "SNMTSint.h"
#include "SNMTSerr.h"

/**
 * This symbol represents a special Safety Domain Number, which is ONLY used
 * in the frame header of the SNMT Service "Reset node guarding time".
 *
 * It will be used in case the SCM did not configure this SN yet. Without configuration the SN
 * normally has no Safety Domain Number to use in frame header.
 */
#define k_RST_SCM_GUARD_SDN            0x0001u

/**
 * This symbol represents a special Safety Address, which is ONLY used
 * in frame header of the SNMT Service "Reset node guarding time".
 *
 * It will be used in case of the SCM did not configure this SN yet. Without configuration the SN
 * normally has no target address to use in frame header.
 */
#define k_RST_SCM_GUARD_SADR           0x0001u

/**
 * This symbol represents a special Safety Address, which is ONLY used
 * in frame header of the SNMT Service "Reset node guarding time".
 *
 * It will be used in case of the SCM did not configure this SN, yet. Without configuration the SN
 * normally has no main SADR (source address) to use in frame header.
 */
#define k_RST_SCM_GUARD_TADR           0x0001u

/**
 * @name SNMTS service response identities
 * @{
 */
/** SNMT Service Response to provide the UDID */
#define k_RESPONSE_UDID       1U
/** SNMT Service Resp. to signal SADR assign. */
#define k_SADR_ASSIGNED       3U
/** SNMT Service Response of extended request */
#define k_RESPONSE_EXT_SERV   5U
/** SNMT Service Response to forces the SCM to restart node guarding domain wide */
#define k_SN_RST_GUARD_SCM    7U
/** @} */

/**
 * @var ab_RespErrUdid
 * This array is used to indicate an error response with UDID.
 */
static const UINT8 ab_RespErrUdid[EPLS_k_UDID_LEN] SAFE_INIT_SEKTOR =
                { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U };

/**
 * @var s_AcsMainSadr
 * Data structure to manage access to the SOD entry 0x1C00, 0x01, which contains the "Main source address" of own SN.
 */
static SOD_t_ACS_OBJECT_VIRT s_AcsMainSadr[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * @var s_AcsUdidScm
 * Data structure to manage access to the SOD entry 0x1200, 0x04, which contains the "UDID of the SCM".
 */
static SOD_t_ACS_OBJECT_VIRT s_AcsUdidScm[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * @var apw_MainSadr
 * Pointer array to the Main SADR in the SOD.
 */
static UINT16 *apw_MainSadr[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * @var ao_SnFailSent
 * Flag array to store that an SN FAIL SNMTS service was sent.
 */
static BOOLEAN ao_SnFailSent[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * @var as_LastSnFail
 * This array stores the error group and the error code of the last sent SN FAIL SNMTS service.
 */
static t_FAIL_ERROR as_LastSnFail[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

static BOOLEAN MainSadrAcsInit(BYTE_B_INSTNUM);
static BOOLEAN UdidScmAcsInit(BYTE_B_INSTNUM);

/**
 * @brief This function initializes the access to the SOD entry 0x1C00, 0x01, which contains the "Main source address" of this SN, the SOD entry 0x1200, 0x04, which contains the "UDID of the SCM" and the variables to surveillance the SN FAIL acknowledge.
 *
 * @param   b_instNum        instance number (not checked, checked in SSC_InitAll()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure
 */
BOOLEAN SNMTS_SodAcsInit(BYTE_B_INSTNUM)
{
    BOOLEAN o_return = FALSE; /* predefined return value */

    ao_SnFailSent[B_INSTNUMidx] = FALSE;
    as_LastSnFail[B_INSTNUMidx].b_group = 0x00U;
    as_LastSnFail[B_INSTNUMidx].b_code = 0x00U;

    /* if the initializaiton of the Main SADR access succeeded */
    if (MainSadrAcsInit(B_INSTNUM))
	{
        /* UDID of the SCM SOD access initialization */
        o_return = UdidScmAcsInit(B_INSTNUM);
    }

    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function gets the SOD entry 0x1C00, 0x01, which contains the "Main source address" of this SN.
 *
 * @param   b_instNum       instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return       Main SADR
 */
UINT16 SNMTS_MainSadrGet(BYTE_B_INSTNUM)
{
    SCFM_TACK_PATH();
    return *(apw_MainSadr[B_INSTNUMidx]);
}

/**
 * @brief This function sets the SOD entry 0x1C00, 0x01, which contains the "Main source address" of this SN.
 *
 * @param        b_instNum         instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        w_mainSadr               main source address to be set (not checked, checked in checkRxAddrInfo()), valid range : EPLS_k_MIN_SADR..EPLS_k_MAX_SADR
 *
 * @return
 * - TRUE            - success
 * - FALSE           - failure, error is already reported
 */
BOOLEAN SNMTS_MainSadrSet(BYTE_B_INSTNUM_ UINT16 w_mainSadr)
{
    SCFM_TACK_PATH();
    return SOD_WriteVirt(B_INSTNUM_ &s_AcsMainSadr[B_INSTNUMidx], &w_mainSadr,
            SOD_k_NO_OVERWRITE);
}

/**
 * @brief This function sets the SOD entry 0x1200, 0x04, which contains the "UDID of the SCM" of this SN.
 *
 * @param        b_instNum         instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        pb_udidScm        pointer the UDID of the SCM (pointer not checked, checked in SOD_Write()), valid range : <> NULL
 *
 * @return
 * - TRUE            - success
 * - FALSE           - failure, error is already reported
 */
BOOLEAN SNMTS_UdidScmSet(BYTE_B_INSTNUM_ const UINT8 *pb_udidScm)
{
    SCFM_TACK_PATH();
    return SOD_WriteVirt(B_INSTNUM_ &s_AcsUdidScm[B_INSTNUMidx], pb_udidScm,
            SOD_k_NO_OVERWRITE);
}

/**
 * @brief This function prepares all data structures for response transmission to the request "SADR Assignment".
 *
 * @param       b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param    ps_rxBuf         reference to received openSAFETY frame to be distributed (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param    ps_txBuf         reference to openSAFETY frame to be transmitted (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 */
void SNMTS_TxRespSadrAssigned(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
        EPLS_t_FRM *ps_txBuf)
        {
    /* collect and store frame header info into module global data structure
     of frame header info */
    ps_txBuf->s_frmHdr.b_id = (k_SADR_ASSIGNED | SNMT_k_FRM_TYPE);
    ps_txBuf->s_frmHdr.w_adr = ps_rxBuf->s_frmHdr.w_tadr;
    ps_txBuf->s_frmHdr.w_sdn = SDN_GetSdn(B_INSTNUM);
    ps_txBuf->s_frmHdr.b_le = EPLS_k_UDID_LEN;
    ps_txBuf->s_frmHdr.w_ct = EPLS_k_CT_NOT_USED;
    ps_txBuf->s_frmHdr.w_tadr = SNMTS_MainSadrGet(B_INSTNUM);
    ps_txBuf->s_frmHdr.b_tr = EPLS_k_TR_NOT_USED;

    /* copy UDID (8 data bytes) into frame memory block */
    SFS_NET_CPY_DOMSTR(&(ps_txBuf->ab_frmData[SNMT_k_OFS_SERV_CMD]),
            &(ps_rxBuf->ab_frmData[SNMT_k_OFS_SERV_CMD]),
            EPLS_k_UDID_LEN);

    SCFM_TACK_PATH();
}

/**
 * @brief This function prepares all data structures for response transmission to the request "UDID". After preparation the response is transmitted.
 *
 * @param        b_instNum            instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        ps_rxBuf             reference to received openSAFETY frame to be distributed (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param        ps_txBuf             reference to openSAFETY frame to be transmitted (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param        ps_accessUdid        pointer to the SOD access structure to read the UDID from the SOD (not chekced, only called with reference to struct), valid range : <> NULL
 *
 * @return
 * - TRUE               - state processing succeeded
 * - FALSE              - state processing failed
 */
void SNMTS_TxRespUdid(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
        EPLS_t_FRM *ps_txBuf, const SOD_t_ACS_OBJECT_VIRT *ps_accessUdid)
{
    UINT8 *pb_udid = (UINT8 *) NULL; /* reference to the 8 byte UDID of the SN */
    SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

    /* collect and store frame header info into modul global data structure
     of frame header info */
    ps_txBuf->s_frmHdr.b_id = (k_RESPONSE_UDID | SNMT_k_FRM_TYPE);
    ps_txBuf->s_frmHdr.w_adr = ps_rxBuf->s_frmHdr.w_tadr;
    ps_txBuf->s_frmHdr.w_sdn = ps_rxBuf->s_frmHdr.w_sdn;
    ps_txBuf->s_frmHdr.b_le = EPLS_k_UDID_LEN;
    ps_txBuf->s_frmHdr.w_ct = EPLS_k_CT_NOT_USED;
    ps_txBuf->s_frmHdr.w_tadr = ps_rxBuf->s_frmHdr.w_adr;
    ps_txBuf->s_frmHdr.b_tr = EPLS_k_TR_NOT_USED;

    /* get Udid from SOD */
    pb_udid = (UINT8 *) SOD_Read(B_INSTNUM_ ps_accessUdid->dw_hdl,
                                 ps_accessUdid->o_applObj, SOD_k_NO_OFFSET,
                                 SOD_k_LEN_NOT_NEEDED, &s_errRes);

    /* if read access failed */
    if (pb_udid == NULL )
    {
        SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                (UINT32) (s_errRes.e_abortCode));

        /* generate error response */
        SFS_NET_CPY_DOMSTR(&(ps_txBuf->ab_frmData[SNMT_k_OFS_SERV_CMD]),
                ab_RespErrUdid, EPLS_k_UDID_LEN);
    }
    else  /* read access succeeded */
    {
        /* copy UDID (8 data bytes) into frame memory block */
        SFS_NET_CPY_DOMSTR(&(ps_txBuf->ab_frmData[SNMT_k_OFS_SERV_CMD]),
                pb_udid, EPLS_k_UDID_LEN);
    }

    SCFM_TACK_PATH();
}

/**
 * @brief This function collects all header info and data of a SNMTS Extended Service Response
 *
 * @param   b_instNum             instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param   ps_rxBuf              reference to received openSAFETY frame to be distributed (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param   ps_txBuf              reference to openSAFETY frame to be transmitted (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 *
 * @param   e_servRespType        service response type (not checked, only called with enum value), valid range: see {t_SNMTS_EXT_SERV_RESP}
 *
 * @param   b_errGroup            only relevant if e_servRespType is k_SN_FAIL, group the error is assigned to (not checked, any value allowed), valid range: 0 .. 255
 *
 * @param   b_errCode             only relevant if e_servRespType is k_SN_FAIL, internal error code (not checked, any value allowed), valid range: 0 .. 255
 *
 * @param   pb_udidScm            Only relevant if e_servRespType is SNMTS_k_UDID_SCM_ASSGN. Pointer to the UDID of the SCM, if the pointer is NULL then ab_respErrUdid is sent (pointer checked), valid range: <> NULL, == NULL
 */
void SNMTS_TxRespExtService(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
        EPLS_t_FRM *ps_txBuf, t_SNMTS_EXT_SERV_RESP e_servRespType,
        UINT8 b_errGroup, UINT8 b_errCode, const UINT8 *pb_udidScm)
{
    UINT8 b_extFrmTyp = 0x00u; /* extended frame type (DB0) */

    /* collect and store frame header info into module global data
     structure of frame header info */
    ps_txBuf->s_frmHdr.b_id = (k_RESPONSE_EXT_SERV | SNMT_k_FRM_TYPE);
    ps_txBuf->s_frmHdr.w_adr = ps_rxBuf->s_frmHdr.w_tadr;
    ps_txBuf->s_frmHdr.w_sdn = ps_rxBuf->s_frmHdr.w_sdn;
    ps_txBuf->s_frmHdr.w_ct = EPLS_k_CT_NOT_USED;
    ps_txBuf->s_frmHdr.b_tr = EPLS_k_TR_NOT_USED;
    ps_txBuf->s_frmHdr.w_tadr = ps_rxBuf->s_frmHdr.w_adr;

    /* write service command response byte into payload area */
    b_extFrmTyp = (UINT8) e_servRespType;
    SFS_NET_CPY8(&(ps_txBuf->ab_frmData[SNMT_k_OFS_SERV_CMD]), &b_extFrmTyp);

    /* if response is the acknowledge of additional SADR assignment */
    if (e_servRespType == SNMTS_k_ADD_SADR_ASSGN)
    {
        /* store service payload data length */
    ps_txBuf->s_frmHdr.b_le = (UINT8)(SNMT_k_LEN_SERV_CMD + SNMT_k_LEN_SADR +
                                      SNMT_k_LEN_TXSPDO);

        /* write add. SADR and TxSPDO number that was written into the SOD,
         into response */
        SFS_NET_CPY_DOMSTR(&(ps_txBuf->ab_frmData[SNMT_k_OFS_SADR]),
                &(ps_rxBuf->ab_frmData[SNMT_k_OFS_SADR]),
                (UINT32)(SNMT_k_LEN_SADR + SNMT_k_LEN_TXSPDO));
    }
    /* else if response is the UDID of the SCM */
    else if (e_servRespType == SNMTS_k_UDID_SCM_ASSGN)
    {
        /* store service payload data length */
        ps_txBuf->s_frmHdr.b_le = (UINT8)(SNMT_k_LEN_SERV_CMD + EPLS_k_UDID_LEN);

        if (pb_udidScm == NULL )
        {
            /* generate error response */
            SFS_NET_CPY_DOMSTR(&(ps_txBuf->ab_frmData[SNMT_k_OFS_UDID_SCM]),
                    ab_RespErrUdid, (UINT32)(EPLS_k_UDID_LEN));
        }
        else
        {
            /* write the response UDID into response buffer */
            SFS_NET_CPY_DOMSTR(&(ps_txBuf->ab_frmData[SNMT_k_OFS_UDID_SCM]),
                    pb_udidScm, (UINT32)(EPLS_k_UDID_LEN));
        }
    }
    /* else if response is the negative response to the request */
    else if (e_servRespType == SNMTS_k_SN_FAIL)
    {
        /* store service payload data length */
        ps_txBuf->s_frmHdr.b_le = (UINT8)(SNMT_k_LEN_SERV_CMD + SNMT_k_LEN_ERR_CODE
                                      + SNMT_k_LEN_ERR_GROUP);

        /* write error group into negative response, DB1 */
        SFS_NET_CPY8(&(ps_txBuf->ab_frmData[SNMT_k_OFS_SERV_CMD +
                                        SNMT_k_OFS_ERR_GROUP]), &b_errGroup);

        /* write error code into negative response, DB2 */
        SFS_NET_CPY8(&(ps_txBuf->ab_frmData[SNMT_k_OFS_SERV_CMD +
                                        SNMT_k_OFS_ERR_CODE]), &b_errCode);

        ao_SnFailSent[B_INSTNUMidx] = TRUE;
        /* store the information of the last SN FAIL */
        as_LastSnFail[B_INSTNUMidx].b_group = b_errGroup;
        as_LastSnFail[B_INSTNUMidx].b_code = b_errCode;
    }
    /* else if response is the initial value of the CT */
    else if (e_servRespType == SNMTS_k_EXT_CT_INITIALIZE)
    {
        /* store service payload data length */
        ps_txBuf->s_frmHdr.b_le = (UINT8)(SNMT_k_LEN_SERV_CMD + EPLS_k_LEN_EXT_CT);

        /* write write initial CT number into response */
        SFS_NET_CPY_DOMSTR(&(ps_txBuf->ab_frmData[SNMT_k_OFS_EXT_CT]),
                           &(ps_rxBuf->ab_frmData[SNMT_k_OFS_EXT_CT]),
                           (UINT32)(EPLS_k_LEN_EXT_CT));
    }
    else  /* response is any of the other service responses */
    {
        /* store service payload data length */
        ps_txBuf->s_frmHdr.b_le = SNMT_k_LEN_SERV_CMD;
    }

    SCFM_TACK_PATH();
}

/**
 * @brief This function checks the SN acknowledge and calls the error application callback function if the checking succeeded.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (not checked, only called with reference to struct in processStateReqProc()), valid range: see EPLS_t_FRM
 */
void SNMTS_SnAckRespCheck(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf)
{
    UINT8 b_errorCode = 0x00u; /* reported error code */
    UINT8 b_errorGroup = 0x00u; /* reported error group */

    /* SN FAIL SNMTS service was sent */
    if (ao_SnFailSent[B_INSTNUMidx])
    {
        /* copy acknowledged error */
        SFS_NET_CPY8(&b_errorGroup, &(ps_rxBuf->ab_frmData[SNMT_k_OFS_ERR_GROUP]));
        SFS_NET_CPY8(&b_errorCode, &(ps_rxBuf->ab_frmData[SNMT_k_OFS_ERR_CODE]));

        /** if the error group and the error code in the last sent SN fail are equal
         * to the error group and the error code in the SN ack */
        if ((as_LastSnFail[B_INSTNUMidx].b_group == b_errorGroup) &&
            (as_LastSnFail[B_INSTNUMidx].b_code == b_errorCode))
        {
            ao_SnFailSent[B_INSTNUMidx] = FALSE;
            /* call application to acknowledge the error */
            SAPL_SNMTS_ErrorAckClbk(B_INSTNUM_ b_errorGroup, b_errorCode);
        }
        else /* unexpected error group and/or error code */
        {
            SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_SN_ACK1,
            (((UINT32)(as_LastSnFail[B_INSTNUMidx].b_group))<<24)|
                    (((UINT32)(as_LastSnFail[B_INSTNUMidx].b_code))<<16)|
                    (((UINT32)(b_errorGroup))<<8)|
                    ((UINT32)(b_errorCode)));
        }
    }
    else /* SN FAIL was not sent, unexpected SN acknowledge */
    {
        SERR_SetError(B_INSTNUM_ SNMTS_k_ERR_SN_ACK2, SERR_k_NO_ADD_INFO);
    }
}

/**
 * @brief This function prepares all data structures for response transmission to the request "Reset node guarding time". After preparation the response is transmitted.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - transmission successful
 * - FALSE          - transmission failed
 */
BOOLEAN SNMTS_TxRespRstGuardScm(BYTE_B_INSTNUM)
{
    BOOLEAN o_return = FALSE; /* predefined return value */
    EPLS_t_FRM_HDR s_txBufHdr; /* local header struct to transmit frame */
    UINT8 * pb_txFrame = (UINT8 *) NULL; /* reference to memory block for frame
     building */

    /* collect and store frame header info into module global data structure
     of frame header info */
    s_txBufHdr.b_id = (k_SN_RST_GUARD_SCM | SNMT_k_FRM_TYPE);
    s_txBufHdr.w_adr = k_RST_SCM_GUARD_SADR;
    s_txBufHdr.w_sdn = k_RST_SCM_GUARD_SDN;
    s_txBufHdr.b_le = EPLS_k_NO_DATA_LEN;
    s_txBufHdr.w_ct = EPLS_k_CT_NOT_USED;
    s_txBufHdr.w_tadr = k_RST_SCM_GUARD_TADR;
    s_txBufHdr.b_tr = EPLS_k_TR_NOT_USED;

    /* allocate a memory block */
    pb_txFrame = SFS_GetMemBlock(B_INSTNUM_ SHNF_k_SNMT, SFS_k_NO_SPDO,
            EPLS_k_NO_DATA_LEN);

    /* if memory allocation failed */
    if(pb_txFrame == NULL)
    {
        /* error: return value is predefined with FALSE,
         error already reported */
    }
    else  /* memory allocation succeeded */
    {
        /* serialize and transmit service to reset the SCMs node guarding time */
        o_return = SFS_FrmSerialize(B_INSTNUM_ &s_txBufHdr, pb_txFrame);
    }
    SCFM_TACK_PATH();
    return o_return;
}


/**
 * @brief This function initializes the access to the SOD entry 0x1C00, 0x01, which contains the "Main source address".
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure
 */
static BOOLEAN MainSadrAcsInit(BYTE_B_INSTNUM)
{
  BOOLEAN o_return = FALSE; /* predefined return value */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get SOD entry "SN main SADR" */
  s_AcsMainSadr[B_INSTNUMidx].s_obj.w_index = EPLS_k_IDX_TXSPDO_COM_PARAM;
  s_AcsMainSadr[B_INSTNUMidx].s_obj.b_subIndex = EPLS_k_SUBIDX_TXSPDO_SADR;
  s_AcsMainSadr[B_INSTNUMidx].dw_segOfs = 0;
  s_AcsMainSadr[B_INSTNUMidx].dw_segSize = 0;

  /* if SOD access failed */
  if (NULL == SOD_AttrGetVirt(B_INSTNUM_ &s_AcsMainSadr[B_INSTNUMidx],&s_errRes))
	{
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
            (UINT32) (s_errRes.e_abortCode));
  }
  else  /* SOD entry "SN main SADR" available */
  {
    apw_MainSadr[B_INSTNUMidx] =
        (UINT16 *)SOD_ReadVirt(B_INSTNUM_ &s_AcsMainSadr[B_INSTNUMidx],&s_errRes);

    /* if the SOD read access succeeded */
    if (apw_MainSadr[B_INSTNUMidx] != NULL )
    {
      o_return = TRUE;
    }
    else /* SOD read access failed */
    {
      SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
              (UINT32) (s_errRes.e_abortCode));
    }
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
 * @brief This function initializes the access to the SOD entry 0x1200, 0x04, which contains the "UDID of the SCM".
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - success
 * - FALSE          - failure
 */
static BOOLEAN UdidScmAcsInit(BYTE_B_INSTNUM)
{
  BOOLEAN o_return = FALSE; /* predefined return value */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */

  /* get SOD entry "UDID of the SCM" */
  s_AcsUdidScm[B_INSTNUMidx].s_obj.w_index = EPLS_k_IDX_COMMON_COM_PARAM;
  s_AcsUdidScm[B_INSTNUMidx].s_obj.b_subIndex = EPLS_k_SUBIDX_UDID_SCM;
  s_AcsUdidScm[B_INSTNUMidx].dw_segOfs = 0;
  s_AcsUdidScm[B_INSTNUMidx].dw_segSize = 0;

  /* if the SOD access failed */
  if (NULL == SOD_AttrGetVirt(B_INSTNUM_ &s_AcsUdidScm[B_INSTNUMidx],&s_errRes))
  {
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
            (UINT32) (s_errRes.e_abortCode));
  }
  else /* SOD access succeeded */
  {
    o_return = TRUE;
  }

  SCFM_TACK_PATH();
  return o_return;

}

/** @} */
