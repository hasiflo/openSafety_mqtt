/**
 * @addtogroup SSC
 * @{
 *
 * @file SSCprocess.c
 *
 * This file contains functions for processing SNMT and SSDO frames.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SSCprocess.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>15.03.2013</td><td>Hans Pill</td><td>added statistic counters</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>05.09.2018</td><td>Stefan Innerhofer</td><td>delete some obsolete code and improve the code formating</td></tr>
 * </table>
 *
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#if(EPLS_cfg_SCM == EPLS_k_ENABLE)
#include "SNMTM.h"
#include "SSDOC.h"
#endif

#include "SHNF.h"
#include "SERRapi.h"
#include "SERR.h"
#include "SFS.h"
#include "SCFMapi.h"
#include "SNMTS.h"
#include "SSDOS.h"

#include "SSCapi.h"
#include "SSCerr.h"
#include "SSCint.h"

/**
 * This define represents the maximum length of the SSDO and SNMT openSAFETY frame in byte.
 */
#define k_MAX_SSDO_SNMT_FRAME_LEN  ((UINT8)(EPLS_k_MAX_HDR_LEN + \
                                       (2U * ((UINT8)(EPLS_cfg_MAX_PYLD_LEN)))))

/**
 * This bit mask extracts the frame type (bit 5, 4, 3) and the direction
 * (bit 0 : req/resp) from the received frame ID.
 */
#define SSC_k_MAJ_MSK_DIR              (UINT8)0x39

/**
 * This symbol is used to identify SNMT Service Requests.
 */
#define SSC_k_ID_SNMT_REQ              (UINT8)0x28

/**
 * This symbol is used to identify SNMT Service Responses.
 */
#define SSC_k_ID_SNMT_RESP             (UINT8)0x29

/**
 * This symbol is used to identify SSDO Service Requests.
 */
#define SSC_k_ID_SSDO_REQ              (UINT8)0x38

/**
 * This symbol is used to identify SSDO Service Responses.
 */
#define SSC_k_ID_SSDO_RESP             (UINT8)0x39

/**
 * This symbol represents the SSDOC frame type Service Request Fast.
 */
#define k_SERVICE_REQ_FAST            0x3Au

#if(EPLS_cfg_SCM == EPLS_k_ENABLE)
/**
 * This symbol represents the SSDOC frame type Service Response Fast.
 */
#define k_SERVICE_RES_FAST            0x3Bu
#endif /* #if(EPLS_cfg_SCM == EPLS_k_ENABLE) */

/**
 * This bit mask extracts the frame type (bit 5, 4, 3) from the frame ID.
 */
#define SSC_k_MAJ_MSK_TYPE              (UINT8)0x38

/**
 * This bit field identifies the SSDO.
 */
#define SSC_k_TYPE_SSDO               (UINT8)0x38

/**
 * This bit identifies weather it is a slim SSDO or not.
 */
#define SSC_k_SSDO_SLIM_BIT             (UINT8)0x02

/**
 * Enumeration of all processing states provided by the openSAFETY Stack.
 */
typedef enum
{
    /** deserialization of received openSAFETY frame */
    k_STATE_DESERIALIZE,
    /** openSAFETY Service Request processing */
    k_STATE_REQ_PROC,
    /** serialization of the assembled response */
    k_STATE_SERIALIZE
} t_PROC_STATE;

/**
 * Structured data type to store all necessary info and data to process a received openSAFETY frame in 3 main steps:
 * -# frame deserialization,
 * -# frame processing and
 * -# frame serialization.
 */
typedef struct
{
    /** current state of SSC FSM */
    t_PROC_STATE e_actState;
#if(EPLS_cfg_SCM == EPLS_k_ENABLE)
    /** flag signals response available to serialize */
    BOOLEAN o_respAvail;
#endif
    /** flag signals frame is in process */
    BOOLEAN o_busy;
    /** telegram type used in SFS_GetMemBlock() */
    SHNF_t_TEL_TYPE e_telType;
    /** structure to store received openSAFETY frame */
    EPLS_t_FRM s_rxBuf;
    /** structure to store openSAFETY frame to be transmitted */
    EPLS_t_FRM s_txBuf;
} t_SSC_FSM;

/**
 * This data structure represent the FSM of the unit SSC.
 *
 * It splits the processing of a received openSAFETY frame into several time slots.
 */
static t_SSC_FSM s_Fsm SAFE_NO_INIT_SEKTOR;

/**
 *    static function-prototypes
 **/

static BOOLEAN processStateDeSer(BYTE_B_INSTNUM_ const UINT8* pb_rxFrm,
        UINT16 w_rxFrmLen);
static BOOLEAN processStateReqProc(BYTE_B_INSTNUM_ UINT32 dw_ct,
        const UINT8* pb_rxFrm);
static BOOLEAN processStateSer(BYTE_B_INSTNUM_ const UINT8* pb_rxFrm);

/**
 * @brief This function processes a received SNMT or SSDO frame.
 *
 * For this purpose it evaluates the frame header and distributes the frame in dependence of its
 * frame type to the assigned unit (SSDOC, SSDOS, SNMTM, SNMTS). The return value of the function
 * signals whether the processing of the passed frame has finished. In case of return value
 * &lt;SSC_k_BUSY&gt; this function has to be recalled without reference to a new openSAFETY frame,
 * NULL has to be passed instead. In case of return value &lt;SSC_k_OK&gt; the processing of the
 * passed openSAFETY frame has finished and a new frame is able to be processed by the openSAFETY Stack.
 *
 * @attention This function must not be called in SNMTS_k_ST_INITIALIZATION state. At least one
 * SHNF memory block must be available for the response otherwise a FAIL SAFE error may be reported.
 *
 * @param        b_instNum         instance number (checked), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct             consecutive time, internal timer value (not checked, any value allowed), valid range: any value
 *
 * @param        pb_rxFrm          reference to the frame to be processed by the openSAFETY Stack (checked), valid range:
 * - <> NULL - received frame to be processed
 * - == NULL - continue frame processing
 *
 * @param        w_rxFrmLen        length of the received frame (in bytes), (checked), valid range: &lt;SSC_k_MIN_TEL_LEN_SHORT&gt; .. &lt;SSC_k_MAX_TEL_LEN_SHORT&gt; OR &lt;SSC_k_MIN_TEL_LEN_LONG&gt; .. &lt;SSC_k_MAX_TEL_LEN_LONG&gt; AND <= (EPLS_k_MAX_HDR_LEN +  (2 * EPLS_cfg_MAX_PYLD_LEN))
 *
 * @return
 * - SSC_k_OK        - frame is completely processed (error may be signaled via SERR_SetError())
 * - SSC_k_BUSY      - frame is currently processed
 */
SSC_t_PROCESS SSC_ProcessSNMTSSDOFrame(BYTE_B_INSTNUM_ UINT32 dw_ct, const UINT8 *pb_rxFrm, UINT16 w_rxFrmLen)
{
    SSC_t_PROCESS e_return = SSC_k_BUSY; /* predefined return value */

#if(EPLS_cfg_MAX_INSTANCES > 1)
    /* if instance number is invalid */
    if(b_instNum >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
        /* error: instance number bigger than maximum */
        SERR_SetError(EPLS_k_NO_INSTANCE_ SSC_k_ERR_PROC_INST_INV,
                (UINT32)b_instNum);
        e_return = SSC_k_OK;
    }
    else
#endif
    {
        /* process received frame in 3 processing steps */
        switch (s_Fsm.e_actState)
        {
        case k_STATE_DESERIALIZE:
        {
            /* if frame deserialization failed */
            if (!(processStateDeSer(B_INSTNUM_ pb_rxFrm, w_rxFrmLen)))
            {
                /* error: received frame is invalid, error already reported,
                 reject invalid frame */
                e_return = SSC_k_OK;
            }
            else /* frame deserialization succeeded */
            {
                /* switch to subsequent state */
                s_Fsm.e_actState = k_STATE_REQ_PROC;
                e_return = SSC_k_BUSY;
            }
            break;
        }
        case k_STATE_REQ_PROC:
        {
            /* if frame processing failed */
            if (!(processStateReqProc(B_INSTNUM_ dw_ct, pb_rxFrm)))
            {
                /* increase statistic counter */
                SERR_CountCommonEvt(B_INSTNUM_ SERR_k_ACYC_REJECT);
                /* error: frame could not be processed, error already reported,
                 reject received frame */
                s_Fsm.e_actState = k_STATE_DESERIALIZE;
                s_Fsm.o_busy = FALSE;
                e_return = SSC_k_OK;
            }
            else /* received frame processed successfully */
            {
                /* if received frame is processed completely */
                if (!s_Fsm.o_busy)
                {
#if(EPLS_cfg_SCM == EPLS_k_ENABLE)
                    /* if response is not available in SSC FSM to be serialized */
                    if(!s_Fsm.o_respAvail)
                    {
                        /* frame processing finished */
                        s_Fsm.e_actState = k_STATE_DESERIALIZE;
                        e_return = SSC_k_OK;
                    }
                    else /* response available in SSC FSM to be serialized */
#endif
                    {
                        /* switch to next state */
                        s_Fsm.e_actState = k_STATE_SERIALIZE;
                        e_return = SSC_k_BUSY;
                    }
                }
                else /* received frame is in process */
                {
                    /* stay in current state until frame is  processed completely */
                    s_Fsm.e_actState = k_STATE_REQ_PROC;
                    e_return = SSC_k_BUSY;
                }
            }
            break;
        }
        case k_STATE_SERIALIZE:
        {
            if (!(processStateSer(B_INSTNUM_ pb_rxFrm)))
            {
                /* error: serialization failed, error already reported,
                 reject frame, switch to first state */
            }
            /* else frame serialized successfully */

            /* switch to first state */
            s_Fsm.e_actState = k_STATE_DESERIALIZE;
            e_return = SSC_k_OK;
            break;
        }
#pragma CTC SKIP
        default:
        {
            /* error: FSM state is unknown and not defined */
            SERR_SetError(B_INSTNUM_ SSC_k_ERR_PROC_STATE_INV,
            (UINT32)(s_Fsm.e_actState));
            s_Fsm.e_actState = k_STATE_DESERIALIZE;
            e_return = SSC_k_OK;
        }
#pragma CTC ENDSKIP
                }
            }

    SCFM_TACK_PATH();
    return e_return;
}

/**
 * @brief This function initializes the SSC FSM.
 */
void SSC_InitFsm(void)
{
    UINT8 b_byteIdx = 0x00u; /* loop counter service data byte array */

    s_Fsm.e_telType = SHNF_k_SPDO;
#if(EPLS_cfg_SCM == EPLS_k_ENABLE)
    s_Fsm.o_respAvail = FALSE;
#endif
    s_Fsm.o_busy = FALSE;
    s_Fsm.e_actState = k_STATE_DESERIALIZE;

    s_Fsm.s_rxBuf.s_frmHdr.w_adr = 0x0000u;
    s_Fsm.s_rxBuf.s_frmHdr.b_id = 0x00u;
    s_Fsm.s_rxBuf.s_frmHdr.w_sdn = 0x0000u;
    s_Fsm.s_rxBuf.s_frmHdr.b_le = 0x00u;
    s_Fsm.s_rxBuf.s_frmHdr.w_ct = 0x0000u;
    s_Fsm.s_rxBuf.s_frmHdr.w_tadr = 0x00u;
    s_Fsm.s_rxBuf.s_frmHdr.b_tr = 0x00u;

    s_Fsm.s_txBuf.s_frmHdr.w_adr = 0x0000u;
    s_Fsm.s_txBuf.s_frmHdr.b_id = 0x00u;
    s_Fsm.s_txBuf.s_frmHdr.w_sdn = 0x0000u;
    s_Fsm.s_txBuf.s_frmHdr.b_le = 0x00u;
    s_Fsm.s_txBuf.s_frmHdr.w_ct = 0x0000u;
    s_Fsm.s_txBuf.s_frmHdr.w_tadr = 0x00u;
    s_Fsm.s_txBuf.s_frmHdr.b_tr = 0x00u;

    for (b_byteIdx = 0x00u; b_byteIdx < (UINT8) EPLS_cfg_MAX_PYLD_LEN; b_byteIdx++)
    {
        s_Fsm.s_rxBuf.ab_frmData[b_byteIdx] = 0x00u;
        s_Fsm.s_txBuf.ab_frmData[b_byteIdx] = 0x00u;
    }
    return;
}

/**
 *    static functions
 **/

/**
 * @brief This function processes deserialization of a received SNMT and SSDO frames.
 *
 * @param        b_instNum         instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        pb_rxFrm          reference to the frame to be processed by the openSAFETY Stack (checked), valid range: <> NULL - ref to received frame to be processed == NULL - continue frame processing
 * @param        w_rxFrmLen        length of the received frame (in bytes), (checked), valid range: &lt;SSC_k_MIN_TEL_LEN_SHORT&gt; .. &lt;SSC_k_MAX_TEL_LEN_SHORT&gt; OR &lt;SSC_k_MIN_TEL_LEN_LONG&gt; .. &lt;SSC_k_MAX_TEL_LEN_LONG&gt; AND <= (EPLS_k_MAX_HDR_LEN +  (2 * EPLS_cfg_MAX_PYLD_LEN))
 *
 * @return
 * - TRUE             - state processed successfully
 * - FALSE            - state processing failed
 */
static BOOLEAN processStateDeSer(BYTE_B_INSTNUM_ const UINT8* pb_rxFrm,
        UINT16 w_rxFrmLen)
{
    BOOLEAN o_return = FALSE; /* predefined return value */
    const UINT8 *pb_rxFrmData = (UINT8 *) NULL; /* reference to payload data */


    /* if EPLS Stack is ready but NO received frame is provided */
    if (pb_rxFrm == NULL )
    {
        /* error: NO frame provided to process */
        SERR_SetError(B_INSTNUM_ SSC_k_ERR_NO_RXFRM_STACK_RDY, SERR_k_NO_ADD_INFO);
    }
    else /* function call is valid to process a received openSAFETY frame */
    {
        /* if the length of the received frame is OK */
        if (k_MAX_SSDO_SNMT_FRAME_LEN >= w_rxFrmLen)
        {

            /* check openSAFETY frame header and frame data and deserialize and store
             frame header info into SSC FSM */
            pb_rxFrmData = SFS_FrmDeSerialize(B_INSTNUM_ (UINT8*)pb_rxFrm,
                    w_rxFrmLen, &(s_Fsm.s_rxBuf.s_frmHdr));

            /* if received frame is NOT valid */
            if(pb_rxFrmData == NULL)
            {
                /* error: received frame is invalid, the error is already reported */
            }
            else /* the received frame was deserialized successfully */
            {
                /* store received service data into FSM */
                MEMCOPY(&(s_Fsm.s_rxBuf.ab_frmData[0]), pb_rxFrmData,
                        s_Fsm.s_rxBuf.s_frmHdr.b_le);
                o_return = TRUE;
            }
        }
        else /* the length of the received frame is invalid */
        {
            SERR_SetError(B_INSTNUM_ SSC_k_ERR_FRAME_LEN, (UINT32)w_rxFrmLen);
        }
    }
    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function distributes the received openSAFETY frame to the matching unit and processes it.
 *
 * @param        b_instNum         instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        dw_ct             consecutive time, internal timer value (not checked, any value allowed), valid range: any value
 *
 * @param        pb_rxFrm          reference to the frame to be processed by the openSAFETY Stack (checked), valid range: NULL
 *
 * @return
 * - TRUE             - state processed successfully
 * - FALSE            - state processing failed
 */
static BOOLEAN processStateReqProc(BYTE_B_INSTNUM_ UINT32 dw_ct,
        const UINT8* pb_rxFrm)
{
    BOOLEAN o_return = FALSE; /* predefined return value */
    EPLS_t_FRM *ps_rxBuf = &(s_Fsm.s_rxBuf); /* Receive buffer */
    EPLS_t_FRM *ps_txBuf = &(s_Fsm.s_txBuf); /* Transmit buffer */
    BOOLEAN *po_busy = &(s_Fsm.o_busy); /* busy flag */

    /* if EPLS Stack is busy but another received frame is provided */
    if (pb_rxFrm != NULL )
    {
        /* error: received frame is provided altought the parent frame is
         in process */
        SERR_SetError(B_INSTNUM_ SSC_k_ERR_FRM_PROV_FRM_IN_PROC,
        SERR_k_NO_ADD_INFO);
    }
    else /* function call is valid */
    {
        /* if received frame is to process from SNMT Slave */
      if(((ps_rxBuf->s_frmHdr.b_id) & SSC_k_MAJ_MSK_DIR) ==SSC_k_ID_SNMT_REQ)
      {
        /* call SNMT Slave to processed the request */
        o_return =
          SNMTS_ProcessRequest(B_INSTNUM_ dw_ct, ps_rxBuf, ps_txBuf, po_busy);
        #if(EPLS_cfg_SCM == EPLS_k_ENABLE)
          s_Fsm.o_respAvail = TRUE;  /* response to be serialized in SSC FSM */
        #endif
        s_Fsm.e_telType = SHNF_k_SNMT;  /* store telegram type for serialization*/
      }
      /* else if received frame is to process from SSDO Server */
      else if(((ps_rxBuf->s_frmHdr.b_id) & SSC_k_MAJ_MSK_DIR)
              == SSC_k_ID_SSDO_REQ)
      {
        /* call SSDO Server to process the received request */
        o_return =
          SSDOS_ProcessRequest(B_INSTNUM_ ps_rxBuf, ps_txBuf, po_busy);
        #if(EPLS_cfg_SCM == EPLS_k_ENABLE)
          s_Fsm.o_respAvail = TRUE;  /* response to be serialized in SSC FSM */
        #endif

        if ( k_SERVICE_REQ_FAST == ps_rxBuf->s_frmHdr.b_id)
        {
          s_Fsm.e_telType = SHNF_k_SSDO_SLIM;  /* store telegram type for serializ. */
        }
        else
        {
          s_Fsm.e_telType = SHNF_k_SSDO;  /* store telegram type for serializ. */
        }
      }
      /* else if received frame is to process from SNMT Master */
      else if(((ps_rxBuf->s_frmHdr.b_id) & SSC_k_MAJ_MSK_DIR)
               == SSC_k_ID_SNMT_RESP)
      {
        #if(EPLS_cfg_SCM == EPLS_k_ENABLE)
          /* SNMT Master processes a service response in a single step */
          *po_busy = FALSE;

          /* call SNMT Master to process the received response */
          o_return =
            SNMTM_ProcessResponse(dw_ct, &(ps_rxBuf->s_frmHdr),
              &(ps_rxBuf->ab_frmData[0]));

          /* response NOT to be serialized in SSC FSM */
          s_Fsm.o_respAvail = FALSE;  /* no response to be serialized */
          s_Fsm.e_telType = SHNF_k_SNMT;   /* store telegram type for serializ. */

        #else
          /* error: SNMT Master was tried to call on a simple Safety Node */
          SERR_SetError(B_INSTNUM_ SSC_k_ERR_NO_SNMT_MASTER,
                        (UINT32)(ps_rxBuf->s_frmHdr.b_id));

        #endif
      }
      /* else if the received frame is to process from SSDO Client */
      else if(((ps_rxBuf->s_frmHdr.b_id) & SSC_k_MAJ_MSK_DIR)
              == SSC_k_ID_SSDO_RESP)
      {
        #if(EPLS_cfg_SCM == EPLS_k_ENABLE)
          /* SSDO Client processes a service response in a single step */
          *po_busy = FALSE;

          /* call SSDO Client to process the received response */
          o_return =
            SSDOC_ProcessResponse(dw_ct, &(ps_rxBuf->s_frmHdr),
              &(ps_rxBuf->ab_frmData[0]));

          /* response NOT to be serialized in SSC FSM */
          s_Fsm.o_respAvail = FALSE;  /* no response to be serialized */

          if ( k_SERVICE_RES_FAST == ps_rxBuf->s_frmHdr.b_id)
          {
            s_Fsm.e_telType = SHNF_k_SSDO_SLIM;  /* store telegram type for serializ. */
          }
          else
          {
            s_Fsm.e_telType = SHNF_k_SSDO;  /* store telegram type for serializ. */
          }

        #else  /* EPLS_cfg_SCM == EPLS_k_DISABLE */
          /* error: SSDO Client was tried to call on a simple Safety Node */
          SERR_SetError(B_INSTNUM_ SSC_k_ERR_NO_SSDO_CLIENT,
                        (UINT32)(ps_rxBuf->s_frmHdr.b_id));

        #endif
      }
      else  /* the received frame ID is NOT defined */
      {
        /* error: the received telegram type (minor frame ID) is NOT defined,
                  return with predefined FALSE */
        SERR_SetError(B_INSTNUM_ SSC_k_ERR_RX_TELTYP_INV,
                      (UINT32)(ps_rxBuf->s_frmHdr.b_id));
      }
    }
    SCFM_TACK_PATH();
    return o_return;
}

/**
 * @brief This function processes serialization of the service response.
 *
 * @param        b_instNum         instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        pb_rxFrm          reference to the frame to be processed by the openSAFETY Stack (checked), valid range: NULL
 *
 * @return
 * - TRUE             - state processed successfully
 * - FALSE            - state processing failed
 */
static BOOLEAN processStateSer(BYTE_B_INSTNUM_ const UINT8* pb_rxFrm)
{
    BOOLEAN o_return = FALSE; /* predefined return value */
    UINT8 *pb_txFrame = (UINT8 *) NULL; /* ref to memory block */

    /* if EPLS Stack is busy but another received frame is provided */
    if (pb_rxFrm != NULL )
    {
        /* error: received frame is provided altought the parent frame is
         in process */
        SERR_SetError(B_INSTNUM_ SSC_k_ERR_FRM_PROV_FRM_IN_PROC,
        SERR_k_NO_ADD_INFO);
    }
    else /* function call is valid */
    {
        /* allocate a memory block */
        /* NOTE: the header structure of SSC FSM is used for request and response
         header info. Thus in state k_STATE_SERIALIZE the SSC FSM header
         structure contains header info of the response to be serialized */
        pb_txFrame =
        SFS_GetMemBlock(B_INSTNUM_ s_Fsm.e_telType, SFS_k_NO_SPDO,
        s_Fsm.s_txBuf.s_frmHdr.b_le);

        /* if memory allocation failed */
        if(pb_txFrame == NULL)
        {
            /* error: memory allocation failed - no memory available for response
             transmission, error already reported,
             return with predefined FALSE */
        }
        else /* memory block available */
        {
            /* if the telegram type is for slim SSDOs set the fast bit */
            if ((SHNF_k_SSDO_SLIM == s_Fsm.e_telType) &&
            (SSC_k_TYPE_SSDO == (s_Fsm.s_txBuf.s_frmHdr.b_id & SSC_k_MAJ_MSK_TYPE)))
            {
                s_Fsm.s_txBuf.s_frmHdr.b_id = s_Fsm.s_txBuf.s_frmHdr.b_id | SSC_k_SSDO_SLIM_BIT;
            }
            /* copy response data into memory block */
            /* NOTE: the data array of SSC FSM is used for request and response
             data. Thus in state k_STATE_SERIALIZE the SSC FSM data array
             contains response data to be serialized */
            MEMCOPY(&pb_txFrame[SFS_k_FRM_DATA_OFFSET],
            &(s_Fsm.s_txBuf.ab_frmData[0]), s_Fsm.s_txBuf.s_frmHdr.b_le);

            /* serialize and transmit frame */
            o_return =
            SFS_FrmSerialize(B_INSTNUM_ &(s_Fsm.s_txBuf.s_frmHdr), pb_txFrame);
        }
    }
    SCFM_TACK_PATH();
    return o_return;
}

/** @} */
