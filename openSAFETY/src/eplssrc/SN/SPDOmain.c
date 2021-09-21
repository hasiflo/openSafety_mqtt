/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOmain.c
 *
 * This file exchanges cyclic safety process data objects (SPDO) and
*             checks timeouts.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SPDOmain.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>added statistic counters</td></tr>
 *     <tr><td>14.08.2012</td><td>Hans Pill</td><td>
 *     	Changed function SPDO_BuiltTxSpdo to first check the instance number and
 *     	afterwards retrieve the number of SPDOs to be processed
 *     	<br />
 *     	Changed function SPDO_CheckRxTimeout to first check the
 *     	instance number and afterwards retrieve the number of SPDOs to be processed
 *     </td></tr>
 *     <tr><td>21.02.2013</td><td>Roman Zwischelsberger</td><td>(new)	A&P297705: SPDO_BuildTxSpdoIx()</td></tr>
 *     <tr><td>15.03.2013</td><td>Hans Pill</td><td>added statistic counters</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>09.08.2013</td><td>Franz Dechant</td><td>added first #pragma CTC SKIP in function SPDO_ActivateRxSpdoMapping()</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>new error codes for SPDOs with 40 bit CT</td></tr>
 *     <tr><td>31.10.2014</td><td>Hans Pill</td><td>changes for virtual SOD</td></tr>
 *     <tr><td>22.09.2015</td><td>Roman Zwischelsberger</td><td>return value of SPDO_GetTxSpdoNo() was not checked</td></tr>
 *     <tr><td>17.11.2016</td><td>Stefan Innerhofer</td><td>changed one byte b_noFreeFrm to two byte w_noFreeFrm</td></tr>
 * </table>
 *
 *
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "EPLScfgCheck.h"

#include "SDN.h"

#include "SHNF.h"
#include "SFS.h"

#include "SCFMapi.h"

#include "SODapi.h"
#include "SOD.h"

#include "SERRapi.h"
#include "SERR.h"
#include "SPDOerr.h"

#include "SPDOapi.h"
#include "SPDO.h"
#include "SPDOint.h"


/**
 * This define represents the maximum length of the SPDO payload data in byte.
*/
#if (SPDO_cfg_MAX_NO_RX_SPDO == 0)
  #define k_MAX_SPDO_PYLD_LEN ((UINT8)(SPDO_cfg_MAX_LEN_OF_TX_SPDO))
#else
  #if (SPDO_cfg_MAX_LEN_OF_TX_SPDO < SPDO_cfg_MAX_LEN_OF_RX_SPDO)
    #define k_MAX_SPDO_PYLD_LEN ((UINT8)(SPDO_cfg_MAX_LEN_OF_RX_SPDO))
  #else
    #define k_MAX_SPDO_PYLD_LEN ((UINT8)(SPDO_cfg_MAX_LEN_OF_TX_SPDO))
  #endif
#endif

/**
 * This define represents the maximum length of the SPDO openSAFETY frame in byte.
*/
#define k_MAX_SPDO_FRAME_LEN (EPLS_k_MAX_HDR_LEN + (2U * k_MAX_SPDO_PYLD_LEN))

static t_SPDO_OBJ as_Obj[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

static void ProcessTReq(BYTE_B_INSTNUM_ UINT32 dw_ct,
                        const EPLS_t_FRM_HDR *ps_rxSpdoHeader,
                        const UINT8 *pb_rxSpdoData);
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  static void ProcessTRes(BYTE_B_INSTNUM_ UINT32 dw_ct,
                          const EPLS_t_FRM_HDR *ps_rxSpdoHeader,
                          const UINT8 *pb_rxSpdoData);
#endif

/**
* @brief This function initializes the variables defined in the t_SPDO_OBJ structure and calls the SPDO unit initialization functions.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @return
* - TRUE           - success
* - FALSE          - failure
*/
BOOLEAN SPDO_Init(BYTE_B_INSTNUM)
{
  /* pointer the SPDO object structure */
  t_SPDO_OBJ *ps_spdoObj = &as_Obj[B_INSTNUMidx];

  /* flag is initialized */
  ps_spdoObj->o_spdoRunning = FALSE;

  /* SPDO index counters are initialized */
  ps_spdoObj->w_actRxSpdoIdx = 0U;
  ps_spdoObj->w_actTxSpdoIdx = 0U;

#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
  /* initialize the counters for the extended SPDOs */
  SPDO_InitExtCt(B_INSTNUM);
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return SPDO_InitAll(B_INSTNUM_ &(ps_spdoObj->w_noTxSpdo),
                      &(ps_spdoObj->w_noRxSpdo));
}

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  /**
  * @brief This function checks the SCT timeout for every RxSPDO.
  *
  * Safety Control Time (SCT) is defined for every RxSPDO in the SOD referenced by index 0x1400-0x17FE
  * and sub-index 0x02 (object name : SCT_U32)
  *
  * @attention The smallest Safety Control Time in the SOD object with index 0x1400-0x17FE and sub-index
  * 0x02 gives the call frequency of this function.
  *
  * @param        b_instNum        instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
  * @param        dw_ct            consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
  */
  void SPDO_CheckRxTimeout(BYTE_B_INSTNUM_ UINT32 dw_ct)
  {
    t_SPDO_OBJ *po_this; /* instance pointer */
    /* number of Rx SPDOs to be processed */
    UINT16 w_noProcRxSpdo;
    UINT16 w_rxSpdoIdx; /* loop counter for the Rx SPDOs */

    #if (EPLS_cfg_MAX_INSTANCES > 1)
      /* if b_instNum is wrong */
      if(B_INSTNUMidx >= (UINT8)EPLS_cfg_MAX_INSTANCES)
      {
        SERR_SetError(EPLS_k_NO_INSTANCE, SPDO_k_ERR_INST_NUM_1,
                      (UINT32)B_INSTNUMidx);
      }
      else /* else all parameter ok */
    #endif
      {
        /* number of Rx SPDOs to be processed */
    	w_noProcRxSpdo = SPDO_GetNoProcRxSpdo(B_INSTNUM);

        po_this = &as_Obj[B_INSTNUMidx];

        /* if the SPDO is active */
        if (po_this->o_spdoRunning)
        {
          /* for all Rx SPDOs to be processed */
          for(w_rxSpdoIdx=0U; w_rxSpdoIdx < w_noProcRxSpdo; w_rxSpdoIdx++)
          {
            /* Consumer state machine is called */
            SPDO_ConsSm(B_INSTNUM_ dw_ct, w_rxSpdoIdx, (EPLS_t_FRM_HDR *)NULL,
                        (UINT8 *)NULL, FALSE);
          }
        }
        /* no else : the SPDO is not active */
      }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
  }
#endif

/**
* @brief This function builds TxSPDOs to transmit.
*
* If a TxSPDO was built and sent then pw_noFreeFrm will be decremented.
*
* @attention The smallest Refresh Prescale time in the SOD object with index 0x1C00-0x1FFE and
* sub-index 0x02 gives the call frequency of this function.
*
* @param        b_instNum               instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
*
* @retval       pw_noFreeFrm            reference to the number of the free SPDO frames to be sent (checked) valid range: <> NULL
*/
void SPDO_BuildTxSpdo(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 *pw_noFreeFrm)
{
  t_SPDO_OBJ *po_this;    /* instance pointer */
  UINT16       w_spdoIdx; /* loop counter for the Tx and Rx SPDOs */
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  /* number of Rx SPDOs to be processed */
  UINT16 w_noProcRxSpdo;
#endif
  /* number of Tx SPDOs to be processed */
  UINT16 w_noProcTxSpdo;


  #if (EPLS_cfg_MAX_INSTANCES > 1)
    /* if instance number is invalid */
    if(B_INSTNUMidx >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      SERR_SetError(EPLS_k_NO_INSTANCE, SPDO_k_ERR_INST_NUM_2,
                    (UINT32)B_INSTNUMidx);
    }
    /* else if reference to number of free SPDOs is invalid */
    else
  #endif
    if(pw_noFreeFrm == NULL)
    {
      /* error: reference to NULL */
      SERR_SetError(B_INSTNUM_ SPDO_k_ERR_FREE_FRAME_PTR_1, SERR_k_NO_ADD_INFO);
    }
    else /* all parameter ok */
    {
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    	/* number of Rx SPDOs to be processed */
    	w_noProcRxSpdo = SPDO_GetNoProcRxSpdo(B_INSTNUM);
#endif
	  /* number of Tx SPDOs to be processed */
	  w_noProcTxSpdo = SPDO_GetNoProcTxSpdo(B_INSTNUM);

      /* instance pointer is set */
      po_this = &as_Obj[B_INSTNUMidx];

      /* if the SPDO is active */
      if (po_this->o_spdoRunning)
      {
        /* for all TxSPDO to be processed */
        for(w_spdoIdx = 0U; w_spdoIdx < w_noProcTxSpdo; w_spdoIdx++)
        {
          /* Check if Time Response SPDO is to be sent */
          SPDO_TimeSyncProdSm(B_INSTNUM_ dw_ct, w_spdoIdx,
                              (EPLS_t_FRM_HDR *)NULL, pw_noFreeFrm);
        }

      #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
        /* for all RxSPDO to be processed */
        for(w_spdoIdx = 0U; w_spdoIdx < w_noProcRxSpdo; w_spdoIdx++)
        {
          /* Check if Time Request SPDO is to be sent */
          SPDO_TimeSyncConsSm(B_INSTNUM_ dw_ct, w_spdoIdx,
                              (EPLS_t_FRM_HDR *)NULL, pw_noFreeFrm);
        }
      #endif

        /* for all TxSPDO to be processed */
        for(w_spdoIdx = 0U; w_spdoIdx < w_noProcTxSpdo; w_spdoIdx++)
        {
          /* Check if Data only SPDO is to be sent */
          SPDO_ProdSm(B_INSTNUM_ dw_ct, w_spdoIdx, pw_noFreeFrm);
        }
      }
      /* no else : the SPDO is not active */
    }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}
#if (SPDO_cfg_MAX_SYNC_RX_SPDO==1)
/**
* @brief This function is almost identical to SPDO_BuildTxSpdo()
*
* The difference being that the SPDO to be build is spezified by parameter
*
* @param        b_instNum               instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
*
* @param        w_spdoIdx            	index of the SPDO to be built (checked) valid range: >=SPDO_GetNoProcTxSpdo()
*
* @return
* - TRUE                    if the SPDO has been generated
* - FALSE	otherwise
*/
UINT8 SPDO_BuildTxSpdoIx(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT16 w_spdoIdx)
{
  t_SPDO_OBJ *po_this;    		/* instance pointer */

  UINT16 w_noFreeFrm=	1;		/* 1 TxSPDO shall be generated	*/

  #if (EPLS_cfg_MAX_INSTANCES > 1)
    /* if instance number is invalid */
    if(B_INSTNUMidx >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      SERR_SetError(EPLS_k_NO_INSTANCE, SPDO_k_ERR_INST_NUM_2,
                    (UINT32)B_INSTNUMidx);
    }
    /* else if reference to number of free SPDOs is invalid */
    else
  #endif
    if (w_spdoIdx>=SPDO_GetNoProcTxSpdo(B_INSTNUM))
    {
      /* error: reference to NULL */
      SERR_SetError(B_INSTNUM_ SPDO_k_ERR_FREE_FRAME_PTR_1, SERR_k_NO_ADD_INFO);
    }
    else /* all parameter ok */
    {
      /* instance pointer is set */
      po_this = &as_Obj[B_INSTNUMidx];

      /* if the SPDO is active */
      if (po_this->o_spdoRunning)
      {
        /* Check if Time Response SPDO is to be sent */
        SPDO_TimeSyncProdSm(B_INSTNUM_ dw_ct, w_spdoIdx,
                              (EPLS_t_FRM_HDR *)NULL, &w_noFreeFrm);

      #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
        if (w_noFreeFrm)
        {
          /* Check if Time Request SPDO is to be sent */
          SPDO_TimeSyncConsSm(B_INSTNUM_ dw_ct, po_this->aw_TxRxConn[w_spdoIdx],
        		  			  (EPLS_t_FRM_HDR *)NULL, &w_noFreeFrm);
        }
      #endif

        if (w_noFreeFrm)
        {
          /* Check if Data only SPDO is to be sent */
          SPDO_ProdSm(B_INSTNUM_ dw_ct, w_spdoIdx, &w_noFreeFrm);
        }
      } /* if spdoRunning */
      /* no else : the SPDO is not active */
    }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  if (w_noFreeFrm)
  {  return(0);				/* no PDO generated	*/
  }
  else
  {  return(1);				/* PDO generated	*/
  }
}
#endif /* SPDO_cfg_MAX_SYNC_RX_SPDO */


/**
* @brief This function processes RxSPDOs and must be called for every received SPDO.
*
* The given SPDO frame header is evaluated and the instance number is determined.
* Three valid Rx SPDO types can be processed depending on the SPDO frame header:
* - data only, no data for time synchronization within
* - data with time request, SPDO producer requests additional time synchronization data within the SPDO
* - data with time response, SPDO producer responses additional time synchronization data within the SPDO
*
* @attention The function must be called before SPDO_CheckRxTimeout() is called.
* The received process data is valid after SPDO_CheckRxTimeout() is called.
*
* @param        b_instNum        instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct            consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
*
* @param        pb_rxFrm         reference to the frame to be processed by the openSAFETY Stack (pointer checked) valid range: <> NULL
*
* @param        w_rxFrmLen       length of the received frame (in bytes) (not checked, checked in checkRxDataLength()) valid range: SSC_k_MIN_TEL_LEN_SHORT.. SSC_k_MAX_TEL_LEN_SHORT or SSC_k_MIN_TEL_LEN_LONG.. SSC_k_MAX_TEL_LEN_LONG AND <= (EPLS_k_MAX_HDR_LEN +  (2 * EPLS_cfg_MAX_PYLD_LEN))
*/
void SPDO_ProcessRxSpdo(BYTE_B_INSTNUM_ UINT32 dw_ct, UINT8 *pb_rxFrm,
                        UINT16 w_rxFrmLen)
{
  t_SPDO_OBJ *po_this; /* instance pointer */
  const UINT8 *pb_rxFrmData = (UINT8 *)NULL; /* reference to payload data */
  EPLS_t_FRM_HDR s_rxFrmHdr; /* Rx frame header */
  #if (SPDO_cfg_FRAME_CPY_INTERN == EPLS_k_ENABLE)
    /* buffer for the received openSAFETY frame */
    UINT32 adw_frameBuf[(k_MAX_SPDO_FRAME_LEN / 4U) + 1U];
  #endif
  #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    UINT16 w_rxSpdoIdx; /* temporary variable for the Rx SPDO index */
  #endif

#if (EPLS_cfg_MAX_INSTANCES > 1)
  /* if instance number is invalid */
  if(B_INSTNUMidx >= (UINT8)EPLS_cfg_MAX_INSTANCES)
  {
    SERR_SetError(EPLS_k_NO_INSTANCE, SPDO_k_ERR_INST_NUM_4,
                  (UINT32)B_INSTNUMidx);
  }
  else
#endif
  /* if reference to openSAFETY frame is NULL */
  if(pb_rxFrm == NULL)
  {
    SERR_SetError(EPLS_k_NO_INSTANCE_ SPDO_k_ERR_RX_FRAME_PTR,
                  SERR_k_NO_ADD_INFO);
  }
  else /* all parameter ok */
  {
    /* if the length of the received frame is OK */
    if (k_MAX_SPDO_FRAME_LEN >= w_rxFrmLen)
    {
      #if (SPDO_cfg_FRAME_CPY_INTERN == EPLS_k_ENABLE)
        /* copy the received frame from the SHNF frame buffer to the
           function local frame buffer */
        MEMCOPY(adw_frameBuf, pb_rxFrm, w_rxFrmLen);

        /* the received frame is deserialized and checked */
        pb_rxFrmData = SFS_FrmDeSerialize(B_INSTNUM_ (UINT8*)adw_frameBuf,
                                          w_rxFrmLen, &s_rxFrmHdr);
      #else
        /* the received frame is deserialized and checked */
        pb_rxFrmData = SFS_FrmDeSerialize(B_INSTNUM_ pb_rxFrm, w_rxFrmLen,
                                          &s_rxFrmHdr);
      #endif

      /* if the received frame is valid */
      if (pb_rxFrmData != NULL)
      {
        /* if the received domain number is unknown */
        if(SDN_GetSdn(B_INSTNUM) != s_rxFrmHdr.w_sdn)
        {
          /* SPDO got rejected */
          SERR_CountCommonEvt(B_INSTNUM_ SERR_k_CYC_REJECT);
          SERR_SetError(B_INSTNUM_ SPDO_k_ERR_UNKNOWN_SDN,
                        (UINT32)s_rxFrmHdr.w_sdn);
        }
        else /* the received domain number is known */
        {
          /* instance pointer is set */
          po_this = &as_Obj[B_INSTNUMidx];

          /* if the SPDO is active */
          if (po_this->o_spdoRunning)
          {
            /* if TReq */
            if ((s_rxFrmHdr.b_id & k_FRAME_MASK_SPDO_TYPE) == k_FRAME_ID_TREQ)
            {
              ProcessTReq(B_INSTNUM_ dw_ct, &s_rxFrmHdr, pb_rxFrmData);
            }
            #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
              /* else if TRes */
              else if ((s_rxFrmHdr.b_id & k_FRAME_MASK_SPDO_TYPE) == k_FRAME_ID_TRES)
              {
                ProcessTRes(B_INSTNUM_ dw_ct, &s_rxFrmHdr,
                                        pb_rxFrmData);
              }
              /* else Data only SPDO */
              else if ((s_rxFrmHdr.b_id & k_FRAME_MASK_SPDO_TYPE) == k_FRAME_ID_DATA_ONLY)
              {
                /* get rx SPDO index for the source address */
                w_rxSpdoIdx = SPDO_GetSpdoIdxForSAdr(B_INSTNUM_
                                                     s_rxFrmHdr.w_adr);
                /* if SPDO is relevant for data processing */
                if (w_rxSpdoIdx != k_INVALID_SPDO_NUM)
                {
                  /* if the CT in the received SPDO is valid (changed and
                    increased) */
                  if (SPDO_CtValid(B_INSTNUM_ w_rxSpdoIdx, s_rxFrmHdr.w_ct))
                  {
                    /* SPDO consumer state machine is called */
                    SPDO_ConsSm(B_INSTNUM_ dw_ct, w_rxSpdoIdx, &s_rxFrmHdr,
                                pb_rxFrmData, FALSE);
                  }
                  /* no else : CT is invalid, SPDO is ignored without an
                               error */
                  else
                  {
                    /* count received SPDOs where the CT check failed */
                    SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_INV_CT);
                  }
                }
                /* else : SPDO is not relevant for data processing */
                else
                {
                    /* count the invalid SPDOs */
                	SERR_CountCommonEvt(B_INSTNUM_ SERR_k_CYC_REJECT);
                }
              }
            #else
              /* else if TRes */
              else if ((s_rxFrmHdr.b_id & k_FRAME_MASK_SPDO_TYPE) == k_FRAME_ID_TRES)
              {
                /* SPDO is ignored */
              }
              /* else Data only SPDO */
              else if ((s_rxFrmHdr.b_id & k_FRAME_MASK_SPDO_TYPE) == k_FRAME_ID_DATA_ONLY)
              {
                /* SPDO is ignored */
              }
            #endif
              /* invalid SPDO frame ID */
              else
              {
                SERR_SetError(B_INSTNUM_ SPDO_k_ERR_INVALID_ID,
                              (UINT32)s_rxFrmHdr.b_id);
              }
          }
          /* no else : the SPDO is not active because the SN state is
                      Pre-operational and no error is generated */
        }
      }
      /* no else : the received frame is invalid, the error is already
                  generated by the SFS_FrmDeSerialize() */
    }
    else /* the length of the received frame is invalid */
    {
      SERR_SetError(B_INSTNUM_ SPDO_k_ERR_FRAME_LEN, (UINT32)w_rxFrmLen);
    }
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
} /*lint !e818 (Info -- Pointer parameter 'pb_rxFrm' (line 338) could be
                        declared as pointing to const [MISRA 2004 Rule 16.7]) */

/**
* @brief This function is called by the application to signal the openSAFETY Stack that the process
* data for the given Tx SPDO has changed.
*
* This Tx SPDO is immediately sent by the SPDO_BuildTxSpdo() before the refresh prescale timeout.
*
* @see          SPDO_BuildTxSpdo()
*
* @param        b_instNum          instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        w_txSpdoNum        Tx SPDO number (checked) valid range: 1..SPDO_cfg_MAX_NO_TX_SPDO
*/
void SPDO_TxDataChanged(BYTE_B_INSTNUM_ UINT16 w_txSpdoNum)
{
  #if (EPLS_cfg_MAX_INSTANCES > 1)
    /* if instance number is wrong */
    if(B_INSTNUMidx >= (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      SERR_SetError(EPLS_k_NO_INSTANCE, SPDO_k_ERR_INST_NUM_3,
                    (UINT32)B_INSTNUMidx);
    }
    else  /* else Instance number is ok */
  #endif
    /* if Tx SPDO number is ok */
    if ((w_txSpdoNum > 0U) && (w_txSpdoNum <= (UINT16)SPDO_cfg_MAX_NO_TX_SPDO))
    {
      SPDO_NewData(B_INSTNUM_ SPDO_NUM_TO_INTERNAL_SPDO_IDX(w_txSpdoNum));
    }
    else /* invalid Tx SPDO number */
    {
      SERR_SetError(B_INSTNUM_ SPDO_k_ERR_TX_SPDO_NUM_4, (UINT32)w_txSpdoNum);
    }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
* @brief This function is called by the SNMTS before the SPDO activation.
*
* The counters for the activation are reset and the SPDO assign tables are cleared.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll() or SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*/
void SPDO_ActivateStart(BYTE_B_INSTNUM)
{
  t_SPDO_OBJ *ps_spdoObj = &as_Obj[B_INSTNUMidx]; /* pointer the SPDO object
                                                     structure */

  /* SPDO assign tables are cleared */
  SPDO_InitAssign(B_INSTNUM);

  /* SPDO index counters are reset */
  ps_spdoObj->w_actRxSpdoIdx = 0U;
  ps_spdoObj->w_actTxSpdoIdx = 0U;
}

/**
* @brief This function activates the mapping of all Tx SPDO.
*
* The mapping- and the communication parameters of all the Tx SPDOs are checked.
*  It has to be called before transition from PreOperational to Operational.
*
* @param        b_instNum       instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @retval          po_busy          only relevant if the return value is TRUE,
* - TRUE  : not all Tx SPDO was activated
* - FALSE : all Tx SPDO was activated (not checked, only called with refernce to variable), valid range : <> NULL
*
* @return
* - TRUE           - SPDO mappings are activated.
* - FALSE          - SPDO mappings are not activated. Wrong SPDO mapping- or/and communication parameters.
*/
BOOLEAN SPDO_ActivateTxSpdoMapping(BYTE_B_INSTNUM_ BOOLEAN *po_busy)
{
  BOOLEAN o_ret = FALSE;       /* return value */
  t_SPDO_OBJ *ps_spdoObj = &as_Obj[B_INSTNUMidx]; /* pointer the SPDO object
                                                     structure */
  void *pv_data;               /* pointer to the SOD data */
  SOD_t_ERROR_RESULT s_errRes; /* SOD access error */
  UINT16 w_actTxSpdo = 0U;     /* activated Tx SPDOs per call */
  SOD_t_ACS_OBJECT_VIRT s_objAcs; /* structure to access the SOD */

  *po_busy = FALSE;

  /* if not all Tx SPDO was activated */
  if (ps_spdoObj->w_actTxSpdoIdx < ps_spdoObj->w_noTxSpdo)
  {
    do
    {
      o_ret = FALSE;

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
      if (ps_spdoObj->w_actTxSpdoIdx >= SPDO_cfg_MAX_NO_TX_SPDO)
      {
        SERR_SetError(B_INSTNUM_ SPDO_K_ERR_INV_INDEX, ps_spdoObj->w_actTxSpdoIdx);
      }
      else
      {
        SPDO_ClearConnectedRxSpdo(B_INSTNUM_ ps_spdoObj->w_actTxSpdoIdx);
#endif
        /* clear the ptrs of the connected RxSPDOs */
        s_objAcs.s_obj.w_index = (UINT16)(k_TX_MAPP_START_IDX + ps_spdoObj->w_actTxSpdoIdx);
        s_objAcs.s_obj.b_subIndex = 0;
        s_objAcs.dw_segOfs = 0;
        s_objAcs.dw_segSize = 0;
        /* if attribute of the mapping parameter sub 0 was read successful */
        if (SOD_AttrGetVirt(B_INSTNUM_ &s_objAcs, &s_errRes) != NULL)
        {
          pv_data = SOD_ReadVirt(B_INSTNUM_ &s_objAcs, &s_errRes);

          /* if mapping parameter sub-index 0 was read successful */
          if (pv_data != NULL)
          {
            /* if the Tx SPDO communication parameter is OK */
            if (SPDO_CheckTxCommPara(B_INSTNUM_ ps_spdoObj->w_actTxSpdoIdx))
            {
              /* activation of the Tx SPDO mapping */
              o_ret = SPDO_TxMappActivate(B_INSTNUM_ ps_spdoObj->w_actTxSpdoIdx,
                                        *((UINT8*)(pv_data)));
              *po_busy = TRUE;
            }
            /* no else : the Tx SPDO communication parameter is not OK */
          }
          /* else reading of the mapping parameter sub-index 0 failed */
          else
          {
            SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                          (UINT32)(s_errRes.e_abortCode));
          }
        }
        /* else reading of the attribute of the mapping parameter sub 0 failed */
        else
        {
          SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                        (UINT32)(s_errRes.e_abortCode));
        }

        ps_spdoObj->w_actTxSpdoIdx++;
        w_actTxSpdo++;
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
      }
#endif
    }
    #if (SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL == 1)
      #pragma CTC SKIP
    #endif
    /* while no error and not all Tx SPDO was activated and
             max. num. of Tx SPDO activation per call is not expired */
    while(o_ret && (ps_spdoObj->w_actTxSpdoIdx < ps_spdoObj->w_noTxSpdo) &&
          (w_actTxSpdo < (UINT16)SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL));
    #if (SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL == 1)
      #pragma CTC ENDSKIP
    #endif
  }
  else /* all Tx Spdo was activated */
  {
    *po_busy = FALSE;
    o_ret = TRUE;
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/**
* @brief This function activates the mapping of all Rx SPDO.
*
* The mapping- and the communication parameters of all the Rx SPDOs are checked. It has to be called before transition from
* PreOperational to Operational.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @retval       po_busy          only relevant if the return value is TRUE,
* - TRUE  : not all Rx SPDO was activated
* - FALSE : all Rx SPDO was activated (not checked, only called with refernce to variable), valid range : <> NULL
*
* @return
* - TRUE           - SPDO mappings are activated.
* - FALSE          - SPDO mappings are not activated. Wrong SPDO mapping- or/and communication parameters.
*/
BOOLEAN SPDO_ActivateRxSpdoMapping(BYTE_B_INSTNUM_  BOOLEAN *po_busy)
{
  BOOLEAN o_ret = FALSE;         /* return value */

  #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    t_SPDO_OBJ *ps_spdoObj = &as_Obj[B_INSTNUMidx]; /* pointer the SPDO object
                                                       structure */
    void *pv_data;               /* pointer to the SOD data*/
    SOD_t_ERROR_RESULT s_errRes; /* SOD access error */
    UINT16 w_actRxSpdo = 0U;     /* activated Rx SPDOs per call */
    SOD_t_ACS_OBJECT_VIRT s_objAcs; /* structure to access the SOD */

    *po_busy = FALSE;

    /* if not all Rx SPDO was activated */
    if (ps_spdoObj->w_actRxSpdoIdx < ps_spdoObj->w_noRxSpdo)
    {
      do
      {
        s_objAcs.s_obj.w_index = (UINT16)(k_RX_MAPP_START_IDX + ps_spdoObj->w_actRxSpdoIdx);
        s_objAcs.s_obj.b_subIndex = 0;
        s_objAcs.dw_segOfs = 0;
        s_objAcs.dw_segSize = 0;
        /* if attribute of the mapping parameter sub 0 was read successful */
        if (SOD_AttrGetVirt(B_INSTNUM_ &s_objAcs, &s_errRes) != NULL)
        {
          pv_data = SOD_ReadVirt(B_INSTNUM_ &s_objAcs, &s_errRes);

          /* if mapping parameter sub-index 0 was read successful */
          if (pv_data != NULL)
          {
            /* hook the Rx SPDO to the corresponding Tx SPDO */
            /* if the Rx SPDO communication parameter is OK */
            if (SPDO_ConsSmConnect(B_INSTNUM_ ps_spdoObj->w_actRxSpdoIdx) &&
              SPDO_CheckRxCommPara(B_INSTNUM_ ps_spdoObj->w_actRxSpdoIdx))
            {
              /* activation of the Rx SPDO mapping */
              o_ret = SPDO_RxMappActivate(B_INSTNUM_ ps_spdoObj->w_actRxSpdoIdx,
                                          *((UINT8*)(pv_data)));
#pragma CTC SKIP
              /* remember the RxSPDO that is connected to this TxSPDO */
              #if (SPDO_cfg_MAX_SYNC_RX_SPDO==1)
              {
                uint16_t txspdo = SPDO_GetTxSpdoNo(B_INSTNUM_ ps_spdoObj->w_actRxSpdoIdx);
                if ( ( txspdo > 0 ) && ( txspdo <= ps_spdoObj->w_noTxSpdo )
                   && ( ps_spdoObj->w_actRxSpdoIdx < ps_spdoObj->w_noRxSpdo ) )
                {
                  ps_spdoObj->aw_TxRxConn[ txspdo - 1 ] = ps_spdoObj->w_actRxSpdoIdx;
                }
              }
              #endif
#pragma CTC ENDSKIP
              *po_busy = TRUE;
            }
            /* no else : the Rx SPDO communication parameter is not OK */
          }
          /* else reading of the mapping parameter sub-index 0 failed */
          else
          {
            SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                          (UINT32)(s_errRes.e_abortCode));
          }
        }
        /* else reading of the attribute of the mapp. parameter sub 0 failed */
        else
        {
          SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                        (UINT32)(s_errRes.e_abortCode));
        }

        ps_spdoObj->w_actRxSpdoIdx++;
        w_actRxSpdo++;
      }
      #if (SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL == 1)
        #pragma CTC SKIP
      #endif
      /* while no error and not all Rx SPDO was activated and
               max. num. of Rx SPDO activation per call is not expired */
      while(o_ret && (ps_spdoObj->w_actRxSpdoIdx < ps_spdoObj->w_noRxSpdo) &&
            (w_actRxSpdo < (UINT16)SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL));
      #if (SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL == 1)
        #pragma CTC ENDSKIP
      #endif
    }
    else /* all Rx Spdo was activated */
  #endif
    {
      *po_busy = FALSE;
      o_ret = TRUE;
    }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/**
* @brief This function is called by the SNMTS, resets SPDO Rx and Tx state machines and activates the SPDO processing and building.
*
* @param        b_instNum       instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct            consecutive time, internal timer value (not checked, any value allowed), valid range: (UINT32)
*/
void SPDO_SetToOp(BYTE_B_INSTNUM_ UINT32 dw_ct)
{
  /* pointer the SPDO object structure */
  t_SPDO_OBJ *ps_spdoObj = &as_Obj[B_INSTNUMidx];

  /* SPDO processing and building are activated */
  ps_spdoObj->o_spdoRunning = TRUE;

  SPDO_ResetTxSm(B_INSTNUM_ dw_ct, SPDO_GetNoProcTxSpdo(B_INSTNUM));
  #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    /* All rx SPDO related data are set to the default value and
       the rx state machine is reset */
    SPDO_ResetRxSm(B_INSTNUM_ SPDO_GetNoProcRxSpdo(B_INSTNUM));
    /* reset all valid connections of this instance */
  #if (SPDO_cfg_CONNECTION_VALID_BIT_FIELD == EPLS_k_ENABLE)
    SPDO_ResetConnectionValidBitField(B_INSTNUM);
  #endif
  #endif

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/**
* @brief This function is called by the SNMTS, resets SPDO Rx state machines and deactivates the SPDO processing and building.
*
* @param        b_instNum       instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*/
void SPDO_SetToPreOp(BYTE_B_INSTNUM)
{
  /* pointer the SPDO object structure */
  t_SPDO_OBJ *ps_spdoObj = &as_Obj[B_INSTNUMidx];

  /* SPDO processing and building are deactivated */
  ps_spdoObj->o_spdoRunning = FALSE;

  /* It is not nessecary to call the SPDO_ResetTxSm(), because it will
     be called if the SN state is switch to Operational again */
  #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    /* All rx SPDO related data are set to the default value and
      the rx state machine is reset */
    SPDO_ResetRxSm(B_INSTNUM_ ps_spdoObj->w_noRxSpdo);
  #endif
  /* reset all valid connections of this instance */
  #if (SPDO_cfg_CONNECTION_VALID_BIT_FIELD == EPLS_k_ENABLE)
    SPDO_ResetConnectionValidBitField(B_INSTNUM);
  #endif

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

#if (EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
  /**
  * @brief This function returns a description of every available errors of the assigned unit.
  *
  * @param        w_errorCode        error number
  *
  * @param        dw_addInfo         additional error information
  *
  * @retval          pac_str            empty buffer to build the error string
  */
  void SPDO_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str)
  {
    /* Choose the matching error string */
    switch(w_errorCode)
    {
      /* SPDO_CheckRxTimeout */
      case SPDO_k_ERR_INST_NUM_1:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_INST_NUM_1: "
                        "SPDO_CheckRxTimeout():\n"
                        "(%lu) instance number is invalid.\n",
                SPDO_k_ERR_INST_NUM_1, dw_addInfo);
        break;
      }

      /* SPDO_BuildTxSpdo */
      case SPDO_k_ERR_INST_NUM_2:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_INST_NUM_2: SPDO_BuildTxSpdo():\n"
                        "(%lu) instance number is invalid.\n",
                SPDO_k_ERR_INST_NUM_2, dw_addInfo);
        break;
      }
      case SPDO_k_ERR_FREE_FRAME_PTR_1:
      {
        SPRINTF1(pac_str, "%#x - SPDO_k_ERR_FREE_FRAME_PTR_1: "
                        "SPDO_BuildTxSpdo():\n"
                        "Pointer to the number of free frames "
                        "counter is NULL.\n",
                SPDO_k_ERR_FREE_FRAME_PTR_1);
        break;
      }

      /* SPDO_ProcessRxSpdo */
      case SPDO_k_ERR_INST_NUM_4:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_INST_NUM_4: SPDO_ProcessRxSpdo():\n"
                        "(%lu) instance number is invalid.\n",
                SPDO_k_ERR_INST_NUM_4, dw_addInfo);
        break;
      }
      case SPDO_k_ERR_RX_FRAME_PTR:
      {
        SPRINTF1(pac_str, "%#x - SPDO_k_ERR_RX_FRAME_PTR: "
                        "SPDO_ProcessRxSpdo():\n"
                        "Pointer to the received frame is NULL.\n",
                SPDO_k_ERR_RX_FRAME_PTR);
        break;
      }
      case SPDO_k_ERR_UNKNOWN_SDN:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_UNKNOWN_SDN: "
                        "SPDO_ProcessRxSpdo():\n"
                        "The received safety domain number (%lu) is unknown.\n",
                SPDO_k_ERR_UNKNOWN_SDN, dw_addInfo);
        break;
      }
      case SPDO_k_ERR_INVALID_ID:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_INVALID_ID: SPDO_ProcessRxSpdo():\n"
                        "The received frame identifier (%#lx) is invalid.\n",
                SPDO_k_ERR_INVALID_ID, dw_addInfo);
        break;
      }
      case SPDO_k_ERR_FRAME_LEN:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_FRAME_LEN: SPDO_ProcessRxSpdo():\n"
                        "The length of the received frame (%lu) is invalid.\n",
                SPDO_k_ERR_FRAME_LEN, dw_addInfo);
        break;
      }

      /* SPDO_TxDataChanged */
      case SPDO_k_ERR_INST_NUM_3:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_INST_NUM_3: SPDO_TxDataChanged():\n"
                        "Instance number (%03lu) is invalid.\n",
                SPDO_k_ERR_INST_NUM_3, dw_addInfo);
        break;
      }
      case SPDO_k_ERR_TX_SPDO_NUM_4:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TX_SPDO_NUM_4: "
                        "SPDO_TxDataChanged():\n"
                        "Tx SPDO (%lu) is invalid.\n",
                        SPDO_k_ERR_TX_SPDO_NUM_4,
                        dw_addInfo);
        break;
      }


      /* InitTxSpdo */
      case SPDO_k_ERR_TX_SPDO_GAP:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TX_SPDO_GAP: InitTxSpdo():\n"
                        "There is a gap before the Tx SPDO "
                        "with SOD index (%#x).\n",
                SPDO_k_ERR_TX_SPDO_GAP, (UINT16)(dw_addInfo));
        break;
      }
      case SPDO_k_ERR_NO_TX_COMM_PARA:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_NO_TX_COMM_PARA: InitTxSpdo():\n"
                        "No Tx SPDO communication parameter is defined "
                        "with SOD index (%#x) sub-index (0) "
                        "for the Tx SPDO mapping parameter "
                        "with SOD index (%#x) sub-index (0).\n",
                SPDO_k_ERR_NO_TX_COMM_PARA, HIGH16(dw_addInfo),
                LOW16(dw_addInfo));
        break;
      }
      case SOD_k_ERR_MAN_TX_SPDO:
      {
        SPRINTF1(pac_str, "%#x - SOD_k_ERR_MAN_TX_SPDO: InitTxSpdo():\n"
                        "No mandatory Tx SPDO is defined.\n",
                SOD_k_ERR_MAN_TX_SPDO);
        break;
      }

      /* TxMappCommInit */
      case SPDO_k_ERR_NO_TX_SPDOS:
      {
        SPRINTF1(pac_str, "%#x - SPDO_k_ERR_NO_TX_SPDOS: TxMappCommInit():\n"
                        "Too many Tx SPDOs are defined.\n",
                SPDO_k_ERR_NO_TX_SPDOS);
        break;
      }
      case SPDO_k_ERR_BEF_WR_TX_SUB_0:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_BEF_WR_TX_SUB_0: "
                        "TxMappCommInit():\n"
                        "Before write flag is not set in the Tx SPDO attribute "
                        "with SOD index (%#x) sub-index (0).\n",
                SPDO_k_ERR_BEF_WR_TX_SUB_0, (UINT16)(dw_addInfo));
        break;
      }

      /* InitRxSpdo */
      case SPDO_k_ERR_RX_SPDO_GAP:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_RX_SPDO_GAP: InitRxSpdo():\n"
                        "There is a gap before the Rx SPDO "
                        "with SOD index (%#x).\n",
                SPDO_k_ERR_RX_SPDO_GAP, (UINT16)(dw_addInfo));
        break;
      }
      case SPDO_k_ERR_NO_RX_COMM_PARA:
      {
        SPRINTF3(pac_str, "%#x - : InitRxSpdo():\n"
                        "No Rx SPDO communication parameter is defined "
                        "with SOD index (%#x) sub-index (0) "
                        "for the Tx SPDO mapping parameter "
                        "with SOD index (%#x) sub-index (0).\n",
                SPDO_k_ERR_NO_RX_COMM_PARA, HIGH16(dw_addInfo),
                LOW16(dw_addInfo));
        break;
      }

      /* RxMappCommInit */
      case SPDO_k_ERR_NO_RX_SPDOS:
      {
        SPRINTF1(pac_str, "%#x - SPDO_k_ERR_NO_RX_SPDOS: RxMappCommInit():\n"
                        "Too many Rx SPDOs are defined.\n",
                SPDO_k_ERR_NO_RX_SPDOS);
        break;
      }
      case SPDO_k_ERR_BEF_WR_RX_SUB_0:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_BEF_WR_RX_SUB_0: "
                        "RxMappCommInit():\n"
                        "Before write flag is not set in the Rx SPDO attribute "
                        "with SOD index (%#x) sub-index (0).\n",
                SPDO_k_ERR_BEF_WR_RX_SUB_0, (UINT16)(dw_addInfo));
        break;
      }

      /* SPDO_CheckTxCommPara */
      case SPDO_k_ERR_TADR:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TADR: SPDO_CheckTxCommPara():\n"
                        "Target Address is already used or invalid. "
                        "Tx SPDO number (%#lx).\n",
                SPDO_k_ERR_TADR, dw_addInfo+1U);
        break;
      }

      /* SPDO_TimeSyncProdSm */
      case SPDO_k_ERR_TREQ_PROC:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TREQ_PROC: SPDO_TimeSyncProdSm():\n"
                        "Tx SPDO with SPDO number (%u) "
                        "is used only for sending SPDO Time Request "
                        "and not for sending SPDO Time Response.\n",
                SPDO_k_ERR_TREQ_PROC, (UINT16)(dw_addInfo)+1U);
        break;
      }

      /* SPDO_SendTxSpdo */
      case SPDO_k_ERR_SEND_TX_SPDO_1:
      {
        SPRINTF1(pac_str, "%#x - SPDO_k_ERR_SEND_TX_SPDO_1: "
                         "SPDO_SendTxSpdo():\n"
                         "Sending request type (k_SENDING_FREE) is invalid.\n",
                SPDO_k_ERR_SEND_TX_SPDO_1);
        break;
      }
      case SPDO_k_ERR_SEND_TX_SPDO_2:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_SEND_TX_SPDO_2: "
                         "SPDO_SendTxSpdo():\n"
                         "Sending request type (%lu) is invalid.\n",
                SPDO_k_ERR_SEND_TX_SPDO_2, dw_addInfo);
        break;
      }
      case SPDO_k_ERR_NULL_PTR:
      {
        SPRINTF1(pac_str, "%#x - SPDO_k_ERR_NULL_PTR: SPDO_SendTxSpdo():\n"
                        "Pointer to the counter of the free frames is NULL.\n",
                SPDO_k_ERR_NULL_PTR);
        break;
      }

      /* SetPtrToTxCommPara */
      case SPDO_k_ERR_BEF_RD_TX:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_BEF_RD_TX: SetPtrToTxCommPara():\n"
                        "Before read attribute must not be set for the "
                        "object with SOD index (%#x) sub-index (%#x).\n",
                        SPDO_k_ERR_BEF_RD_TX,
                        HIGH16(dw_addInfo), (UINT8)dw_addInfo);
        break;
      }
      case SPDO_k_ERR_TX_SPDO_OBJ:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_TX_SPDO_OBJ: "
                        "SetPtrToTxCommPara():\n"
                        "Tx SPDO object with SOD index (%#x) sub-index (%#x) "
                        "is not supported.\n",
                        SPDO_k_ERR_TX_SPDO_OBJ,
                        HIGH16(dw_addInfo), (UINT8)dw_addInfo);
        break;
      }

      /* SPDO_CheckRxCommPara */
      case SPDO_k_ERR_SADR:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_SADR: SPDO_CheckRxCommPara():\n"
                        "Source Address is already used or invalid. "
                        "Rx SPDO number (%#lx).\n",
                SPDO_k_ERR_SADR,
                dw_addInfo+1U);
        break;
      }

      /* SPDO_TimeSyncConsSm */
      case SPDO_k_ERR_TRES_TREQ:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TRES_TREQ: SPDO_TimeSyncConsSm():\n"
                        "SPDO Time Response was received without sending a "
                        "SPDO Time Request (late SPDO time response) "
                        "on the Rx SPDO with SPDO number (%u).\n",
                        SPDO_k_ERR_TRES_TREQ,
                        (UINT16)dw_addInfo+1U);
        break;
      }
      case SPDO_k_ERR_TR_EXPIRED:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TR_EXPIRED: "
                        "SPDO_TimeSyncConsSm():\n"
                        "Number of not answered TR expired at the Rx SPDO "
                        "with SPDO number (%u)\n",
                        SPDO_k_ERR_TR_EXPIRED,
                        (UINT16)dw_addInfo+1U);
        break;
      }
      case SPDO_k_ERR_TREQ_TIME_1:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TREQ_TIME_1: "
                        "SPDO_TimeSyncConsSm():\n"
                        "SPDO Time Request Cycle time is expired "
                        "for the Rx SPDO with SPDO number (%u) in the "
                        "k_STATE_WF_TRES state.\n",
                        SPDO_k_ERR_TREQ_TIME_1,
                        (UINT16)dw_addInfo+1U);
        break;
      }
      case SPDO_k_ERR_TREQ_TIME_2:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TREQ_TIME_2: "
                        "SPDO_TimeSyncConsSm():\n"
                        "SPDO Time Request Cycle time is expired "
                        "for the Rx SPDO with SPDO number (%u) in the "
                        "k_STATE_WF_NEXT_TREQ_BLOCK state.\n",
                        SPDO_k_ERR_TREQ_TIME_2,
                        (UINT16)dw_addInfo+1U);
        break;
      }
      case SPDO_k_ERR_LATE_TRES:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_LATE_TRES: SPDO_TimeSyncConsSm():\n"
                        "Late SPDO Time Response was received on the "
                        "Rx SPDO with SPDO number (%u)\n",
                        SPDO_k_ERR_LATE_TRES,
                        (UINT16)dw_addInfo+1U);
        break;
      }
      case SPDO_k_ERR_UNKNOWN_STATE:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_UNKNOWN_STATE: "
                        "SPDO_TimeSyncConsSm():\n"
                        "The time synchronization consumer state machine "
                        "of the Rx SPDO with SPDO number (%u) is in "
                        "unknown state\n",
                        SPDO_k_ERR_UNKNOWN_STATE,
                        (UINT16)dw_addInfo+1U);
        break;
      }

      /* SPDO_ConsSm */
      case SPDO_k_ERR_SCT_TIMER:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_SCT_TIMER: SPDO_ConsSm():\n"
                        "SCT timer is expired for the Rx SPDO "
                        "with SPDO number (%u)\n",
                        SPDO_k_ERR_SCT_TIMER,
                        (UINT16)dw_addInfo+1U);
        break;
      }

      /* ProcessData */
      case SPDO_k_ERR_DELAY_SHORT:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_DELAY_SHORT: ProcessData():\n"
                        "The SPDO propagation delay (%u) received on the "
                        "Rx SPDO with SPDO number (%u) is too short.\n",
                        SPDO_k_ERR_DELAY_SHORT,
                        HIGH16(dw_addInfo), LOW16(dw_addInfo)+1U);
        break;
      }
      case SPDO_k_ERR_DELAY_LONG:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_DELAY_LONG: ProcessData():\n"
                        "The SPDO propagation delay (%u) received on the "
                        "Rx SPDO with SPDO number (%u) is too long.\n",
                        SPDO_k_ERR_DELAY_LONG,
                        HIGH16(dw_addInfo), LOW16(dw_addInfo)+1U);
        break;
      }
#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
      case SPDO_k_ERR_DELTA_EXT_CT:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_DELTA_EXT_CT: ProcessData():\n"
                        "The delay (%u) between two received extended CT timers on the "
                        "Rx SPDO with SPDO number (%u) is too long.\n",
                        SPDO_k_ERR_DELTA_EXT_CT,
                        HIGH16(dw_addInfo), LOW16(dw_addInfo)+1U);
        break;
      }
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */

      /* SetPtrToRxCommPara */
      case SPDO_k_ERR_BEF_RD_RX:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_BEF_RD_RX: SetPtrToRxCommPara():\n"
                        "Before read attribute must not be set for the "
                        "object with SOD index (%#x) sub-index (%#x).\n",
                        SPDO_k_ERR_BEF_RD_RX,
                        HIGH16(dw_addInfo), (UINT8)dw_addInfo);
        break;
      }
      case SPDO_k_ERR_RX_SPDO_OBJ:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_RX_SPDO_OBJ: "
                        "SetPtrToRxCommPara():\n"
                        "Rx SPDO object with SOD index (%#x) sub-index (%#x) "
                        "is not supported.\n",
                        SPDO_k_ERR_RX_SPDO_OBJ,
                        HIGH16(dw_addInfo), (UINT8)dw_addInfo);
        break;
      }

      /* StateWaitForTres */
      case SPDO_k_ERR_SHORT_TSYNC:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_SHORT_TSYNC: StateWaitForTres():\n"
                        "SPDO time synchronization propagation "
                        "delay (%lu) is too short.\n",
                        SPDO_k_ERR_SHORT_TSYNC,
                        dw_addInfo);
        break;
      }
      case SPDO_k_ERR_LONG_TSYNC:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_LONG_TSYNC: StateWaitForTres():\n"
                        "SPDO time synchronization propagation "
                        "delay (%lu) is too long.\n",
                        SPDO_k_ERR_LONG_TSYNC,
                        dw_addInfo);
        break;
      }
      case SPDO_k_ERR_TR_IS_INVALID:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_TR_IS_INVALID: "
                        "StateWaitForTres():\n"
                        "TR (%u) in the SPDO Time Response received "
                        "on the Rx SPDO number (%u) is invalid.\n",
                        SPDO_k_ERR_TR_IS_INVALID,
                        (UINT8)dw_addInfo, HIGH16(dw_addInfo)+1U);
        break;
      }

      /* SPDO_SetTxMapp */
      case SPDO_k_ERR_TX_MAPP_ENTRY_1:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TX_MAPP_ENTRY_1: "
                        "SPDO_SetTxMapp():\n"
                        "Too many Tx SPDO mapping entries are defined "
                        "for the Tx SPDO with SOD index (%#x) sub-index (0).\n",
                        SPDO_k_ERR_TX_MAPP_ENTRY_1,
                        (UINT16)(dw_addInfo));
        break;
      }
      case SPDO_k_ERR_BEF_WR_TX_SUB_X:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_BEF_WR_TX_SUB_X: "
                        "SPDO_SetTxMapp():\n"
                        "Before write flag is not set for the Tx SPDO "
                        "object with SOD index (%#x) sub-index (%#x).\n",
                        SPDO_k_ERR_BEF_WR_TX_SUB_X,
                        HIGH16(dw_addInfo), (UINT8)dw_addInfo);
        break;
      }
      case SPDO_k_ERR_TX_MAP_OBJ:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TX_MAP_OBJ: SPDO_SetTxMapp():\n"
                        "SOD object is not mappable for the Tx SPDO. "
                        "SSDO abort code (%#lx).\n",
                SPDO_k_ERR_TX_MAP_OBJ,
                dw_addInfo);
        break;
      }

      /* SPDO_SetRxMapp */
      case SPDO_k_ERR_RX_MAPP_ENTRY_1:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_RX_MAPP_ENTRY_1: "
                        "SPDO_SetRxMapp():\n"
                        "Too many Rx SPDO mapping entries are defined "
                        "for the Rx SPDO with number (%#x) sub-index (0).\n",
                        SPDO_k_ERR_RX_MAPP_ENTRY_1,
                        (UINT16)(dw_addInfo)+1U);
        break;
      }
      case SPDO_k_ERR_BEF_WR_RX_SUB_X:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_BEF_WR_RX_SUB_X: "
                        "SPDO_SetRxMapp():\n"
                        "Before write flag is not set for the Rx SPDO "
                        "object with SOD index (%#x) sub-index (%#x).\n",
                        SPDO_k_ERR_BEF_WR_RX_SUB_X,
                        HIGH16(dw_addInfo), (UINT8)dw_addInfo);
        break;
      }
      case SPDO_k_ERR_RX_MAP_OBJ:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_RX_MAP_OBJ: SPDO_SetRxMapp():\n"
                        "SOD object is not mappable for the Rx SPDO. "
                        "SSDO abort code (%#lx).\n",
                SPDO_k_ERR_RX_MAP_OBJ,
                dw_addInfo);
        break;
      }

      /* SPDO_RxMappingProcess */
      case SPDO_k_ERR_PAYLOAD_SIZE:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_PAYLOAD_SIZE: "
                        "SPDO_RxMappingProcess():\n"
                        "Rx SPDO received with wrong payload size (%u) "
                        "for the Rx SPDO with SPDO number (%u).\n",
                        SPDO_k_ERR_PAYLOAD_SIZE,
                        (UINT8)(dw_addInfo), HIGH16(dw_addInfo)+1U);
        break;
      }

      /* SPDO_TxMappActivate */
      case SPDO_k_ERR_TX_TOO_LONG:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TX_TOO_LONG: "
                        "SPDO_TxMappActivate():\n"
                        "Payload size is too large for the Tx SPDO "
                        "with SPDO number (%u).\n",
                        SPDO_k_ERR_TX_TOO_LONG,
                        (UINT16)(dw_addInfo)+1U);
        break;
      }
      case SPDO_k_ERR_TX_OBJ_MAPP_1:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_TX_OBJ_MAPP_1: "
                        "SPDO_TxMappActivate():\n"
                        "SOD mapping entry at SOD sub-index (%u) for the "
                        "Tx SPDO with SPDO number (%u) is not mappable "
                        "because of a SOD error.\n",
                        SPDO_k_ERR_TX_OBJ_MAPP_1,
                        (UINT8)(dw_addInfo), HIGH16(dw_addInfo)+1U);
        break;
      }
      case SPDO_k_ERR_TX_MAPP_ENTRY_3:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TX_MAPP_ENTRY_3: "
                        "SPDO_TxMappActivate():\n"
                        "Number of the mapped entries is too large "
                        "for the Tx SPDO with SPDO number (%u).\n",
                        SPDO_k_ERR_TX_MAPP_ENTRY_3,
                        (UINT16)(dw_addInfo)+1U);
        break;
      }

      /* SPDO_RxMappActivate */
      case SPDO_k_ERR_RX_TOO_LONG:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_RX_TOO_LONG: "
                        "SPDO_RxMappActivate():\n"
                        "Payload size is too large for the Rx SPDO "
                        "with SPDO number (%u).\n",
                        SPDO_k_ERR_RX_TOO_LONG,
                        (UINT16)(dw_addInfo)+1U);
        break;
      }
      case SPDO_k_ERR_RX_OBJ_MAPP_1:
      {
        SPRINTF3(pac_str, "%#x - SPDO_k_ERR_RX_OBJ_MAPP_1: "
                        "SPDO_RxMappActivate():\n"
                        "SOD mapping entry at SOD sub-index (%u) for "
                        "the Rx SPDO with SPDO number (%u) is not mappable "
                        "because of a SOD error.\n",
                        SPDO_k_ERR_RX_OBJ_MAPP_1,
                        (UINT8)(dw_addInfo), HIGH16(dw_addInfo)+1U);
        break;
      }
      case SPDO_k_ERR_RX_MAPP_ENTRY_3:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_RX_MAPP_ENTRY_3: "
                        "SPDO_RxMappActivate():\n"
                        "Number of the mapped entries is too large "
                        "for the Rx SPDO with SPDO number (%u).\n",
                        SPDO_k_ERR_RX_MAPP_ENTRY_3,
                        (UINT16)(dw_addInfo)+1U);
        break;
      }

      /* SPDO_SOD_TxMappPara_CLBK */
      case SPDO_k_ERR_TX_SOD_SRV:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TX_SOD_SRV: "
                        "SPDO_SOD_TxMappPara_CLBK():\n"
                        "SOD service (%lu) is ignored.\n",
                        SPDO_k_ERR_TX_SOD_SRV,
                        dw_addInfo);
        break;
      }

      /* SPDO_SOD_RxMappPara_CLBK */
      case SPDO_k_ERR_RX_SOD_SRV:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_RX_SOD_SRV: "
                        "SPDO_SOD_RxMappPara_CLBK():\n"
                        "SOD service (%lu) is ignored.\n",
                        SPDO_k_ERR_RX_SOD_SRV,
                        dw_addInfo);
        break;
      }

      /* SPDO_TxSpdoIdxExists */
      case SPDO_k_ERR_TX_SPDO_NUM_1:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TX_SPDO_NUM_1: "
                        "SPDO_TxSpdoIdxExists():\n"
                        "Tx SPDO (%lu) is not configured.\n",
                        SPDO_k_ERR_TX_SPDO_NUM_1,
                        dw_addInfo);
        break;
      }
      case SPDO_k_ERR_TX_SPDO_NUM_2:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TX_SPDO_NUM_2: "
                        "SPDO_TxSpdoIdxExists():\n"
                        "Tx SPDO (%lu) is not defined in the SOD.\n",
                        SPDO_k_ERR_TX_SPDO_NUM_2,
                        dw_addInfo);
        break;
      }
      case SPDO_k_ERR_TX_SPDO_NUM_3:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TX_SPDO_NUM_3: "
                        "SPDO_TxSpdoIdxExists():\n"
                        "Tx SPDO (%lu) is invalid.\n",
                        SPDO_k_ERR_TX_SPDO_NUM_3,
                        dw_addInfo);
        break;
      }

      case SPDO_k_ERR_TX_NO_RX_CONN_1:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_TX_NO_RX_CONN_1: "
                        "SPDO_SyncOkConnect():\n"
                        "The Rx SPDO connection to the Tx SPDO (%lu) failed.\n",
                        SPDO_k_ERR_TX_NO_RX_CONN_1,
                        dw_addInfo);
        break;
      }
#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
      /* SPDO_UpdateExtCtValue */
      case SPDO_k_ERR_INST_NUM_5:
      {
        SPRINTF2(pac_str, "%#x - SPDO_k_ERR_INST_NUM_5: SPDO_UpdateExtCtValue():\n"
                        "(%lu) instance number is invalid.\n",
                SPDO_k_ERR_INST_NUM_5, dw_addInfo);
        break;
      }
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */

      default:
      {
        SPRINTF(pac_str, "Unknown error code was generated by the SPDO\n");
        break;
      }
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
    /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15
                                Notice #17 - Function comment content less than
                                             10.0%
                                Notice #18 - Function eLOC > maximum 200 eLOC */
  }
    /* RSM_IGNORE_QUALITY_END */
#endif

#pragma CTC SKIP
  /**
  * @brief Returns a pointer to the internal object of unit SPDOmain.c Only for unit test.
  *
  * @note This function is used for unit tests.
  *
  * @param        b_instNum        instance number (not checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
  *
  * @return
  * - == NULL - memory allocation failed
  * - <> NULL - memory allocation and the mapping processing succeeded, reference to the openSAFETY frame
  */

const t_SPDO_OBJ* SPDO_GetMainObj(BYTE_B_INSTNUM)
{
  const t_SPDO_OBJ* ps_obj = (t_SPDO_OBJ *)NULL;

  #if (EPLS_cfg_MAX_INSTANCES > 1)
    // check range of instance number
    if( B_INSTNUMidx < (UINT8)EPLS_cfg_MAX_INSTANCES)
    {
      // get pointer to object
      ps_obj = &as_Obj[B_INSTNUMidx];
    }
    // else - do nothing
  #else
    ps_obj = &as_Obj[B_INSTNUMidx];
  #endif

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return ps_obj;
}
#pragma CTC ENDSKIP


/***
*    static functions
***/
/**
* @brief This function processes a Time Request SPDO by calling the time synchronization producer state machine and the
* SPDO consumer state machine.
*
* @param        b_instNum              instance number (not checked, checked in SPDO_ProcessRxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed) valid range: (UINT32)
*
* @param        ps_rxSpdoHeader         reference to the header info of the RxSPDO (pointer not checked, only called with reference to struct in SPDO_ProcessRxSpdo()) valid range: <> NULL
*
* @param        pb_rxSpdoData           reference to the data of the RxSPDO (pointer not checked, checked in SPDO_ProcessRxSpdo()) valid range: <> NULL
*
* @return
* - TRUE                  - RxSPDO data entered successful
* - FALSE                 - RxSPDO data failed to enter
*/
static void ProcessTReq(BYTE_B_INSTNUM_ UINT32 dw_ct,
                        const EPLS_t_FRM_HDR *ps_rxSpdoHeader,
                        const UINT8 *pb_rxSpdoData)
{
  UINT16 w_txSpdoIdx; /* temporary variable to store the Tx SPDO index */
  #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    UINT16 w_rxSpdoIdx; /* temporary variable to store the Rx SPDO index */
  #else
    pb_rxSpdoData = pb_rxSpdoData; /* to avoid compiler warning */
  #endif

  /* get tx SPDO index for the target address */
  w_txSpdoIdx = SPDO_GetSpdoIdxForTAdr(B_INSTNUM_ ps_rxSpdoHeader->w_tadr);
  /* if SPDO is relevant for the time synchronization */
  if (w_txSpdoIdx != k_INVALID_SPDO_NUM)
  {
    /* Time synchronization producer state machine is called */
    SPDO_TimeSyncProdSm(B_INSTNUM_ dw_ct, w_txSpdoIdx, ps_rxSpdoHeader,
                        (UINT16 *)NULL);
  }
  /* no else : SPDO is not relevant for the time synchronization */

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  /* get rx SPDO index for the source address */
  w_rxSpdoIdx = SPDO_GetSpdoIdxForSAdr(B_INSTNUM_ ps_rxSpdoHeader->w_adr);
  /* if SPDO is relevant for data processing */
  if (w_rxSpdoIdx != k_INVALID_SPDO_NUM)
  {
    /* if the CT in the received SPDO is valid (changed and increased) */
    if (SPDO_CtValid(B_INSTNUM_ w_rxSpdoIdx, ps_rxSpdoHeader->w_ct))
    {
      /* SPDO consumer state machine is called */
      SPDO_ConsSm(B_INSTNUM_ dw_ct, w_rxSpdoIdx, ps_rxSpdoHeader,
                  pb_rxSpdoData, FALSE);
    }
    /* no else : CT is invalid, SPDO is ignored without an error */
    else
    {
      /* count received SPDOs where the CT check failed */
      SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_INV_CT);
    }
  }
  /* else : SPDO is not relevant for data processing */
  else
  {
    /* count the invalid SPDOs */
  	SERR_CountCommonEvt(B_INSTNUM_ SERR_k_CYC_REJECT);
  }
#endif

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  /**
  * @brief This function processes a Time Response SPDO by calling the time synchronization consumer state machine and
  * the SPDO consumer state machine.
  *
  * @param        b_instNum              instance number (not checked, checked in SPDO_ProcessRxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
  *
  * @param        dw_ct                   consecutive time, internal timer value (not checked, any value allowed), valid range: (UINT32)
  *
  * @param        ps_rxSpdoHeader         reference to the header info of the RxSPDO (pointer not checked, only called with reference to struct in SPDO_ProcessRxSpdo()) valid range: <> NULL
  *
  * @param        pb_rxSpdoData           reference to the data of the RxSPDO (pointer not checked, checked in SPDO_ProcessRxSpdo()) valid range: <> NULL
  *
  * @return
  * - TRUE                  - RxSPDO data entered successful
  * - FALSE                 - RxSPDO data failed to enter
  */
  static void ProcessTRes(BYTE_B_INSTNUM_ UINT32 dw_ct,
                          const EPLS_t_FRM_HDR *ps_rxSpdoHeader,
                          const UINT8 *pb_rxSpdoData)
  {
    UINT16 w_rxSpdoIdx; /* temporary variable to store the Rx SPDO index */

    /* get rx SPDO index for the source address */
    w_rxSpdoIdx = SPDO_GetSpdoIdxForSAdr(B_INSTNUM_ ps_rxSpdoHeader->w_adr);

    /* if SPDO is relevant for the processing */
    if (w_rxSpdoIdx != k_INVALID_SPDO_NUM)
    {
      /* if the CT in the received SPDO is valid (changed and increased) */
      if (SPDO_CtValid(B_INSTNUM_ w_rxSpdoIdx, ps_rxSpdoHeader->w_ct))
      {
        /* SPDO is relevant for the time synchronization */
        if (SPDO_GetSpdoIdxForTAdr(B_INSTNUM_ ps_rxSpdoHeader->w_tadr) !=
                                  k_INVALID_SPDO_NUM)
        {
          /* Time synchronization consumer state machine is called */
          SPDO_TimeSyncConsSm(B_INSTNUM_ dw_ct, w_rxSpdoIdx, ps_rxSpdoHeader,
                              (UINT16 *)NULL);
        }
        /* no else : SPDO is not relevant for the time synchronization */

        /* SPDO consumer state machine is called */
        SPDO_ConsSm(B_INSTNUM_ dw_ct, w_rxSpdoIdx, ps_rxSpdoHeader,
                    pb_rxSpdoData, FALSE);
      }
      /* no else : CT is invalid, SPDO is ignored without an error */
      else
      {
        /* count received SPDOs where the CT check failed */
        SERR_CountSpdoEvt(B_INSTNUM_ w_rxSpdoIdx,SERR_k_SPDO_INV_CT);
      }
    }
    /* else : SPDO is not relevant for data processing */
    else
    {
        /* count the invalid SPDOs */
    	SERR_CountCommonEvt(B_INSTNUM_ SERR_k_CYC_REJECT);
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();
  }
#endif
#if (SPDO_cfg_CONNECTION_VALID_BIT_FIELD == EPLS_k_ENABLE)
/**
* @brief This function clears all valid connections
*
* @param        b_instNum         instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*/
void SPDO_ResetConnectionValidBitField(BYTE_B_INSTNUM)
{
  /* clear all valid connections */
  (void) MEMSET((void*)&SHNF_aaulConnValidBit[B_INSTNUMidx][0],0,sizeof(SHNF_aaulConnValidBit[B_INSTNUMidx]));
}
#endif
/** @} */
