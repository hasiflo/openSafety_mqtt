/**
 * @addtogroup SSDOS
 * @{
 *
 *
 * @file SSDOSupDnwLd.c
 * This file contains functionality to process SSDOC upload and
 * download request and generate SSDOS response to the SSDOC request.
 *
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SSDOSupDnwLd.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>11.09.2014</td><td>Hans Pill</td><td>changes for preload</td></tr>
 *     <tr><td>28.11.2014</td><td>Roland Knall</td><td>Define for SSDO cmd header</td></tr>
 *     <tr><td>15.01.2015</td><td>Roland Knall</td><td>Change magic number to correct defines</td></tr>
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
#include "SSDOSerr.h"
#include "SSDOSint.h"
#include "SSDOint.h"
#include "SSDOSapi.h"

#include "sacun.h"

/** This symbol is used to access the end segment bit inside the SOD access command (SaCmd).
*/
#define k_ENDSEG_SACMD    (UINT8)0x40

/**
 * Structured data type to process the segmented SSDO transfer.
*/
typedef struct
{
    /**
     * Entry size
     *
     * - TRUE the entry size unknown
     * - otherwise FALSE
     */
  BOOLEAN o_objSizeUnknown;

  /** raw data length in the segment */
  UINT8 b_rawDataLen;
  /** reference to the raw data in the segment */
  const UINT8 *pb_rawData;

  /** length of the transferred data */
  UINT32 dw_tranfDataLen;
  /** SOD entry size in bytes */
  UINT32 dw_entrySize;
  /** SaNo of the command */
  UINT16 w_saNo;
  /** SaNo of the initiate upload command */
  UINT16 w_startSaNo;
  /** Start offset for preload upload */
  UINT32 dw_prelStartOffset;
} t_SEG_INFO;

/**
 * Array of segment information structures.
*/
STATIC t_SEG_INFO as_SegInfo[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * Array of queue sizes.
*/
STATIC UINT8 ab_QueueSize[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
* @brief This function initializes the structure with the segment information.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll() or SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*/
void SSDOS_SegInfoInit(BYTE_B_INSTNUM)
{
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */

  ps_segInfo->o_objSizeUnknown = FALSE;
  ps_segInfo->b_rawDataLen = 0x00U;
  ps_segInfo->pb_rawData = (UINT8 *)NULL;
  ps_segInfo->dw_tranfDataLen = 0x00UL;
  ps_segInfo->dw_entrySize = 0x00UL;

  SCFM_TACK_PATH();
  return;
}
/**
* @brief This function initializes the size of the SSDOS queue.
*
* @param        b_instNum        instance number (not checked, checked in SSC_InitAll() or SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*/
void SSDOS_QueueSizeInit(BYTE_B_INSTNUM)
{
  ab_QueueSize[B_INSTNUMidx] = k_QLEN_PREL_MIN;
  return;
}
/**
* @brief This function sets the size of the SSDOS queue.
*
* @param        b_instNum        instance number (checked) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
*
* @param        b_queueSize      size of the SSDO receive queue of this device (checked)
*                                valid range: k_QLEN_PREL_MIN .. k_QLEN_PREL_MAX
*
*/
BOOLEAN SSDOS_SetQueueSize(BYTE_B_INSTNUM_ UINT8 const b_queueSize)
{
  BOOLEAN o_retVal = FALSE;

#if (EPLS_cfg_MAX_INSTANCES > 1)
  /* if instance number is invalid */
  if(B_INSTNUMidx >= (UINT8)EPLS_cfg_MAX_INSTANCES)
  {
    SERR_SetError(EPLS_k_NO_INSTANCE, SSDOS_k_ERR_INST_NUM_1,
                  (UINT32)B_INSTNUMidx);
  }
  else
#endif
  if ((k_QLEN_PREL_MIN > b_queueSize) ||
      (k_QLEN_PREL_MAX < b_queueSize))
  {
    ab_QueueSize[B_INSTNUMidx] = k_QLEN_PREL_MIN;
  }
  else
  {
    ab_QueueSize[B_INSTNUMidx] = b_queueSize;
    o_retVal = TRUE;
  }

  return o_retVal;
}
/**
* @brief This function processes a SSDO expedited initiate download request.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1

* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed
*       (pointer not checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldInitExpReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf)
{
  UINT32 dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR; /* return value */
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */

  dw_abortCode = SSDOS_SodAcsAttrGet(B_INSTNUM_ ps_rxBuf->ab_frmData,
                                     FALSE, &ps_segInfo->dw_entrySize);

  /* if no error happened */
  if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  {
    /* if access to SOD entry is allowed */
    if(SSDOS_SodAcsWriteAllowed(B_INSTNUM_ FALSE))
    {
      dw_abortCode = (UINT32)SOD_ABT_NO_ERROR;
    }
    else  /* SOD access is not allowed */
    {
      dw_abortCode = SERR_GetLastAddInfo();
    }
  }
  /* no else : abort response will be sent */

  SCFM_TACK_PATH();
  return dw_abortCode;
}

/**
* @brief This function processes a SSDO segmented initiate download request.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed
*       (pointer not checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldInitSegReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf)
{
  UINT32 dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR; /* return value */
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */
  UINT32 dw_rxEntrySize; /* received SOD entry size */

  dw_abortCode = SSDOS_SodAcsAttrGet(B_INSTNUM_ ps_rxBuf->ab_frmData,
                                     FALSE, &ps_segInfo->dw_entrySize);

  /* if no error happened */
  if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  {
    /* if SOD access is allowed */
    if(SSDOS_SodAcsWriteAllowed(B_INSTNUM_ TRUE))
    {
      /* if at least saCmd, index and sub-index are received */
      if (ps_rxBuf->s_frmHdr.b_le >= (k_DATA_HDR_LEN + k_DATA_OBJ_LEN))
      {
        /* get received SOD entry size from SSDO Service Request */
        SFS_NET_CPY32(&dw_rxEntrySize, &(ps_rxBuf->ab_frmData[k_OFS_OBJ_LEN]));

        /* if received entry size is right */
        if(dw_rxEntrySize <= ps_segInfo->dw_entrySize)
        {
          /* if the SOD entry size to be downloaded is unknown */
          if (dw_rxEntrySize == k_OBJ_SIZE_UNKNOWN)
          {
            ps_segInfo->o_objSizeUnknown = TRUE;
          }
          else /* the SOD entry size to be downloaded is known */
          {
            ps_segInfo->o_objSizeUnknown = FALSE;
            /* store the received entry size */
            ps_segInfo->dw_entrySize = dw_rxEntrySize;
          }

          /* store ref to entry segment raw data */
          ps_segInfo->pb_rawData = &(ps_rxBuf->ab_frmData[k_OFS_PYLD_INIT_SEG]);
          /* store raw data length */
          ps_segInfo->b_rawDataLen = (UINT8)(ps_rxBuf->s_frmHdr.b_le -
                                      (k_DATA_HDR_LEN + k_DATA_OBJ_LEN));
          /* store SaNo - to be checked for preload download */
          ps_segInfo->w_saNo = ps_rxBuf->s_frmHdr.w_ct;

          dw_abortCode = (UINT32)SOD_ABT_NO_ERROR;
        }
        else  /* received entry size is wrong */
        {
          SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_DWNLD_SEG_LEN,
                        SERR_k_NO_ADD_INFO);
          dw_abortCode = (UINT32)SOD_ABT_BLOCK_SIZE_INVALID;
        }
      }
      else /* response payload length error */
      {
        SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_REQ_PYLD_LEN, SERR_k_NO_ADD_INFO);
        dw_abortCode = (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH;
      }
    }
    else /* SOD access is not allowed */
    {
      dw_abortCode = SERR_GetLastAddInfo();
    }
  }
  /* no else : abort response will be sent */

  SCFM_TACK_PATH();
  return dw_abortCode;
}

/**
* @brief This function generates a SSDOS initiate segmented download response.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame  to be transmitted (pointer not checked, only
*       called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldInitSegRespSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                  EPLS_t_FRM *ps_txBuf)
{
  UINT32 dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR; /* return value */
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */

  /* SOD is locked */
  dw_abortCode = SSDOS_SodAcsLock(B_INSTNUM);
  /* if no error happened */
  if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  {
  #if ((EPLS_cfg_MAX_PYLD_LEN > (k_DATA_HDR_LEN + k_DATA_OBJ_LEN)) || (EPLS_cfg_MAX_PYLD_LEN_FAST_SSDO > (k_DATA_HDR_LEN + k_DATA_OBJ_LEN)))
    /* if number of bytes to be written is zero */
    if(ps_segInfo->b_rawDataLen == 0U)
    {
      /* NOTE: this is a special case for segmented download.
               If <EPLS_cfg_MAX_PYLD_LEN> is equal to 8, the first segment
               only contains 4 bytes of entry size (UINT32). Thus no write
               access is necessary in this special case */
      dw_abortCode = (UINT32)SOD_ABT_NO_ERROR;
    }
    else  /* request contains data to write into SOD entry */
    {
      dw_abortCode =
          SSDOS_SodAcsWrite(B_INSTNUM_ ps_segInfo->pb_rawData, 0x00UL,
                            (UINT32)ps_segInfo->b_rawDataLen, FALSE);
    }

    /* if no error happened */
    /* RSM_IGNORE_QUALITY_BEGIN Notice #22 - 'if' is not bound with scope
                                         braces {} */
    if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  #endif
    {
    /* RSM_IGNORE_QUALITY_END */

      /* generate SSDO Service Response */
      SSDOS_SodAcsResp(B_INSTNUM_ ps_rxBuf, ps_txBuf, k_DWNLD_INIT_SEG,
                       EPLS_k_TR_NOT_USED, k_DATA_HDR_LEN, TRUE);

      /* store the transferred data length */
      ps_segInfo->dw_tranfDataLen = ps_segInfo->b_rawDataLen;
    }

    /* no else : abort response will be sent */
  }
  /* no else : abort response will be sent */

  SCFM_TACK_PATH();
  return dw_abortCode;
}
/**
* @brief This function generates a SSDOS initiate pre-load segmented download response.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame  to be transmitted (pointer not checked, only
*       called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldInitPrelSegRespSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                  EPLS_t_FRM *ps_txBuf)
{
  UINT32 dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR; /* return value */
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */

  /* SOD is locked */
  dw_abortCode = SSDOS_SodAcsLock(B_INSTNUM);

  /* if no error happened */
  if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  {
  #if ((EPLS_cfg_MAX_PYLD_LEN > (k_DATA_HDR_LEN + k_DATA_OBJ_LEN)) || (EPLS_cfg_MAX_PYLD_LEN_FAST_SSDO > (k_DATA_HDR_LEN + k_DATA_OBJ_LEN)))
    /* if number of bytes to be written is zero */
    if(ps_segInfo->b_rawDataLen == 0U)
    {
      /* NOTE: this is a special case for segmented download.
               If <EPLS_cfg_MAX_PYLD_LEN> is equal to 8, the first segment
               only contains 4 bytes of entry size (UINT32). Thus no write
               access is necessary in this special case */
      dw_abortCode = (UINT32)SOD_ABT_NO_ERROR;
    }
    else  /* request contains data to write into SOD entry */
    {
      dw_abortCode =
          SSDOS_SodAcsWrite(B_INSTNUM_ ps_segInfo->pb_rawData, 0x00UL,
                            (UINT32)ps_segInfo->b_rawDataLen, FALSE);
    }

    /* if no error happened */
    /* RSM_IGNORE_QUALITY_BEGIN Notice #22 - 'if' is not bound with scope
                                         braces {} */
    if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  #endif
    {
    /* RSM_IGNORE_QUALITY_END */

      /* generate SSDO Service Response */
      SSDOS_SodAcsResp(B_INSTNUM_ ps_rxBuf, ps_txBuf, k_DWNLD_SEG_PRE_INIT,
                       ab_QueueSize[B_INSTNUMidx] | k_ERR_PREL_TR,k_DATA_HDR_LEN,TRUE);

      /* store the transferred data length */
      ps_segInfo->dw_tranfDataLen = ps_segInfo->b_rawDataLen;
    }

    /* no else : abort response will be sent */
  }
  /* no else : abort response will be sent */

  SCFM_TACK_PATH();
  return dw_abortCode;
}
/**
* @brief This function generates a SSDOS initiate expedited download response.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame to be transmitted (pointer not checked, only called
*       with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldInitExpRespSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                  EPLS_t_FRM *ps_txBuf)
{
  UINT32 dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR; /* return value */
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */
  UINT8 b_rawDataLen;  /* length of the raw data */
  EPLS_t_DATATYPE e_dataType;

  b_rawDataLen = (UINT8)(ps_rxBuf->s_frmHdr.b_le - k_DATA_HDR_LEN);

  e_dataType = SSDOS_SodAcsDataTypeGet(B_INSTNUM);

  /* if the access object data type is DOMAIN or VISIBLE STRING or
        OCTET STRING */
  if ((e_dataType == EPLS_k_DOMAIN) ||
      (e_dataType == EPLS_k_VISIBLE_STRING) ||
      (e_dataType == EPLS_k_OCTET_STRING))
  {
    /* if raw data length is smaller or equal to the maximum length of the
          object */
    if(b_rawDataLen <= ps_segInfo->dw_entrySize)
    {
      dw_abortCode =
          SSDOS_SodAcsWrite(B_INSTNUM_
                            &(ps_rxBuf->ab_frmData[k_OFS_PYLD_INIT_EXP]),
                            SOD_k_NO_OFFSET, (UINT32)b_rawDataLen, TRUE);
    }
    else /* wrong raw data length */
    {
      SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_DWNLD_EXP_LEN1, SERR_k_NO_ADD_INFO);
      dw_abortCode = (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH;
    }
  }
  else /* other (standard) data type */
  {
    /* if payload length of received entry is UNEVEN to SOD entry size */
    if(b_rawDataLen != ps_segInfo->dw_entrySize)
    {
      SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_DWNLD_EXP_LEN, SERR_k_NO_ADD_INFO);
      dw_abortCode = (UINT32)SOD_ABT_LEN_DOES_NOT_MATCH;
    }
    else /* payload length of received entry is EVEN to SOD entry size */
    {
      dw_abortCode =
          SSDOS_SodAcsWrite(B_INSTNUM_
                            &(ps_rxBuf->ab_frmData[k_OFS_PYLD_INIT_EXP]),
                            SOD_k_NO_OFFSET, SOD_k_LEN_NOT_NEEDED, FALSE);
    }
  }

  /* if no error happened */
  if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  {
    /* generate SSDO Service Response */
    SSDOS_SodAcsResp(B_INSTNUM_ ps_rxBuf, ps_txBuf, k_DWNLD_INIT_EXP,
                     EPLS_k_TR_NOT_USED,k_DATA_HDR_LEN,TRUE);
  }
  /* no else : abort response will be sent */

  SCFM_TACK_PATH();
  return dw_abortCode;
}

/**
* @brief This function processes a SSDO segmented middle download request.
*
* @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf           reference to received openSAFETY frame to be distributed (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL,
*       see EPLS_t_FRM
*
* @param        ps_txBuf           reference to openSAFETY frame to be transmitted (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        b_echoSaCmd        the SOD access command to be sent back (not checked, any value allowed),
*       valid range: UINT8
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldMidSegReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                EPLS_t_FRM *ps_txBuf, UINT8 b_echoSaCmd)
{
  UINT32 dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR; /* return value */
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */
  UINT32 dw_calcTranfDataLen; /* calculated transferred data length */


  /* calculate the raw data length */
  ps_segInfo->b_rawDataLen = (UINT8)(ps_rxBuf->s_frmHdr.b_le -
                                     k_DATA_HDR_LEN_SEG);

  /* calculate the transferred data length */
  dw_calcTranfDataLen = ps_segInfo->dw_tranfDataLen +
                        ps_segInfo->b_rawDataLen;

  /* if the segment size is valid */
  if (dw_calcTranfDataLen < ps_segInfo->dw_entrySize)
  {
    dw_abortCode =
        SSDOS_SodAcsWrite(B_INSTNUM_
                          &(ps_rxBuf->ab_frmData[k_OFS_PYLD_SEG]),
                          ps_segInfo->dw_tranfDataLen,
                          (UINT32)ps_segInfo->b_rawDataLen, FALSE);

    /* if no error happened */
    if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
    {
      /* store the transferred data length */
      ps_segInfo->dw_tranfDataLen = dw_calcTranfDataLen;

      /* generate SSDOS response */
      SSDOS_SodAcsResp(B_INSTNUM_ ps_rxBuf, ps_txBuf, b_echoSaCmd,
                       EPLS_k_TR_NOT_USED,k_DATA_HDR_LEN_SEG,FALSE);
    }
    /* no else : abort response will be sent */
  }
  else /* invalid segment size */
  {
    SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_DWNLD_SEG_LEN2,
                  SERR_k_NO_ADD_INFO);

    dw_abortCode = (UINT32)SOD_ABT_BLOCK_SIZE_INVALID;
  }

  SCFM_TACK_PATH();
  return dw_abortCode;
}
/**
* @brief This function processes a SSDO segmented preload middle download request.
*
* @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf           reference to received openSAFETY frame to be distributed (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL,
*       see EPLS_t_FRM
*
* @param        ps_txBuf           reference to openSAFETY frame to be transmitted (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        b_echoSaCmd        the SOD access command to be sent back (not checked, any value allowed),
*       valid range: UINT8
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldMidPrelSegReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                EPLS_t_FRM *ps_txBuf, UINT8 b_echoSaCmd)
{
  UINT32 dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR; /* return value */
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */
  UINT32 dw_calcTranfDataLen; /* calculated transferred data length */
  /* check if SaNo matches */
  if ((ps_segInfo->w_saNo + 1) == ps_rxBuf->s_frmHdr.w_ct)
  {
      ps_segInfo->w_saNo = ps_rxBuf->s_frmHdr.w_ct;
      /* calculate the raw data length */
      ps_segInfo->b_rawDataLen = (UINT8)(ps_rxBuf->s_frmHdr.b_le -
                                         k_DATA_HDR_LEN_SEG);

      /* calculate the transferred data length */
      dw_calcTranfDataLen = ps_segInfo->dw_tranfDataLen +
                            ps_segInfo->b_rawDataLen;

      /* if the segment size is valid */
      if (dw_calcTranfDataLen < ps_segInfo->dw_entrySize)
      {
        dw_abortCode =
            SSDOS_SodAcsWrite(B_INSTNUM_
                              &(ps_rxBuf->ab_frmData[k_OFS_PYLD_SEG]),
                              ps_segInfo->dw_tranfDataLen,
                              (UINT32)ps_segInfo->b_rawDataLen, FALSE);

        /* if no error happened */
#pragma CTC SKIP
        if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
#pragma CTC ENDSKIP
        {
          /* store the transferred data length */
          ps_segInfo->dw_tranfDataLen = dw_calcTranfDataLen;

          /* generate SSDOS response */
          SSDOS_SodAcsResp(B_INSTNUM_ ps_rxBuf, ps_txBuf, b_echoSaCmd,
                  EPLS_k_TR_NOT_USED,k_DATA_HDR_LEN_SEG,FALSE);
        }
        /* no else : abort response will be sent */
      }
      else /* invalid segment size */
      {
        SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_DWNLD_SEG_LEN2,
                      SERR_k_NO_ADD_INFO);

        dw_abortCode = (UINT32)SOD_ABT_BLOCK_SIZE_INVALID;
      }
  }
  /* generate a response containing the next required SaNo */
  else
  {
      dw_abortCode = (UINT32)SOD_ABT_NO_ERROR;
      /* generate SSDOS response with expected SaNo
       * therefore the rx buffer must be manipulated */
      ((EPLS_t_FRM *)ps_rxBuf)->s_frmHdr.w_ct = ps_segInfo->w_saNo + 1;
      SSDOS_SodAcsResp(B_INSTNUM_ ps_rxBuf, ps_txBuf, b_echoSaCmd,
              k_ERR_PREL_TR,k_DATA_HDR_LEN_SEG,FALSE);
  }

  SCFM_TACK_PATH();
  return dw_abortCode;
}

/**
* @brief This function processes a SSDO segmented end download request.
*
* @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()), valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf           reference to received openSAFETY frame to be distributed (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf           reference to openSAFETY frame to be transmitted (pointer not checked, only
*       called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        b_echoSaCmd        the SOD access command to be sent back (not checked, any value allowed), valid range: UINT8
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_DwnldEndSegReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                EPLS_t_FRM *ps_txBuf, UINT8 b_echoSaCmd)
{
  UINT32 dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR; /* return value */
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */
  UINT32 dw_calcTranfDataLen; /* calculated transferred data length */

  /* calculate the number of payload data bytes received within the request */
  ps_segInfo->b_rawDataLen = (UINT8)(ps_rxBuf->s_frmHdr.b_le -
                                     k_DATA_HDR_LEN_SEG);

  /* calculate the transferred data length */
  dw_calcTranfDataLen = ps_segInfo->dw_tranfDataLen +
                        ps_segInfo->b_rawDataLen;

  /* if the received SOD entry size is unknown and the end segment size is
        smaller equal to the local SOD entry size */
  if ((ps_segInfo->o_objSizeUnknown) &&
      (dw_calcTranfDataLen <= ps_segInfo->dw_entrySize))
  {
    dw_abortCode =
        SSDOS_SodAcsWrite(B_INSTNUM_
                          &(ps_rxBuf->ab_frmData[k_OFS_PYLD_SEG]),
                          ps_segInfo->dw_tranfDataLen,
                          (UINT32)ps_segInfo->b_rawDataLen, TRUE);
  }
  /* else if the received SOD entry size is known and the end segment size is
        equal to the local SOD entry size */
  else if (!(ps_segInfo->o_objSizeUnknown) &&
           (dw_calcTranfDataLen == ps_segInfo->dw_entrySize))
  {
    dw_abortCode =
        SSDOS_SodAcsWrite(B_INSTNUM_
                          &(ps_rxBuf->ab_frmData[k_OFS_PYLD_SEG]),
                          ps_segInfo->dw_tranfDataLen,
                          (UINT32)ps_segInfo->b_rawDataLen, TRUE);
  }
  else /* segment size is invalid */
  {
    SERR_SetError(B_INSTNUM_ SSDOS_k_ERR_DWNLD_SEG_LEN1,
                    SERR_k_NO_ADD_INFO);

    dw_abortCode = (UINT32)SOD_ABT_BLOCK_SIZE_INVALID;
  }

  /* if no error happened */
  if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  {
    /* generate SSDOS response */
    SSDOS_SodAcsResp(B_INSTNUM_ ps_rxBuf, ps_txBuf, b_echoSaCmd,
                     EPLS_k_TR_NOT_USED,k_DATA_HDR_LEN_SEG,FALSE);

    dw_abortCode = (UINT32)SOD_ABT_NO_ERROR;
  }
  /* no else : abort response will be sent */

  SCFM_TACK_PATH();
  return dw_abortCode;
}

/**
* @brief This function processes a SSDO segmented initiate upload request.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed (pointer
*       not checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @retval       po_seg           only relevant if SOD_ABT_NO_ERROR returned,
* - TRUE : segmented upload
* - FALSE : expedited upload (pointer not checked, only called with reference to variable in WfReqInitState()), valid range: TRUE, FALSE
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_UpldInitReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                             BOOLEAN *po_seg)
{
  UINT32 dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR; /* return value */
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */

  dw_abortCode = SSDOS_SodAcsAttrGet(B_INSTNUM_ ps_rxBuf->ab_frmData,
                                     TRUE, &ps_segInfo->dw_entrySize);

  /* if no error happened */
  if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  {
    /* if SOD entry has to be transferred SEGMENTED */
    if(ps_segInfo->dw_entrySize >
        (UINT8)((UINT8)EPLS_cfg_MAX_PYLD_LEN - k_DATA_HDR_LEN))
    {
      *po_seg = TRUE;

      dw_abortCode = SSDOS_SodAcsLock(B_INSTNUM);

      /* if no error happened */
      if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
      {
        /* store the maximum segment length */
        ps_segInfo->b_rawDataLen =
            (UINT8)((UINT8)EPLS_cfg_MAX_PYLD_LEN -
                    (k_DATA_HDR_LEN + k_DATA_OBJ_LEN)); /*lint !e778 Info --
                    Constant expression evaluates to 0 in operation '-'
                    only if (EPLS_cfg_MAX_PYLD_LEN == 8) */

        ps_segInfo->pb_rawData =
           SSDOS_SodAcsRead(B_INSTNUM_ 0x00UL,
                            (UINT32)ps_segInfo->b_rawDataLen, &dw_abortCode);
      }
      /* no else : abort response will be sent */
    }
    else  /* EXPEDITED upload can be processed */
    {
      *po_seg = FALSE;

      ps_segInfo->pb_rawData = SSDOS_SodAcsRead(B_INSTNUM_ SOD_k_NO_OFFSET,
                                                SOD_k_LEN_NOT_NEEDED,
                                                &dw_abortCode);

      /* store the segment length */
      ps_segInfo->b_rawDataLen = (UINT8)ps_segInfo->dw_entrySize;
    }
  }
  /* no else : abort response will be sent */

  SCFM_TACK_PATH();
  return dw_abortCode;
}

/**
* @brief This function generates a SSDOS initiate expedited upload response.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame  to be transmitted (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_UpldInitExpRespSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                                 EPLS_t_FRM *ps_txBuf)
{
  UINT32 dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR; /* return value */
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */
  UINT8 b_pyldLenResp; /* response payload length */
  EPLS_t_DATATYPE e_dataType; /* data type of the SOD entry */

  /* get data type of the SOD entry */
  e_dataType = SSDOS_SodAcsDataTypeGet(B_INSTNUM);

  /* if the byte order conversion succeeded */
  if(SFS_NetworkCopyGen((void *)&(ps_txBuf->ab_frmData[k_OFS_PYLD_INIT_EXP]),
                        ps_segInfo->pb_rawData,
                        (UINT32)ps_segInfo->b_rawDataLen, e_dataType))
  {
    b_pyldLenResp = (UINT8)(k_DATA_HDR_LEN + ps_segInfo->b_rawDataLen);

    /* generate SSDO Service Response */
    SSDOS_SodAcsResp(B_INSTNUM_ ps_rxBuf, ps_txBuf, k_UPLD_INIT_EXP,
                     EPLS_k_TR_NOT_USED,b_pyldLenResp,TRUE);

    dw_abortCode = (UINT32)SOD_ABT_NO_ERROR;
  }
  else /* the byte order conversion failed */
  {
    dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR;
  }

  SCFM_TACK_PATH();
  return dw_abortCode;
}

/**
* @brief This function generates a SSDOS initiate segmented upload response.
*
* @param        b_instNum        instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param        ps_rxBuf         reference to received openSAFETY frame  to be distributed (pointer
*       not checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param        ps_txBuf         reference to openSAFETY frame  to be transmitted (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*/
void SSDOS_UpldInitSegRespSend(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                               EPLS_t_FRM *ps_txBuf)
{
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */

  #if (EPLS_cfg_MAX_PYLD_LEN > (k_DATA_HDR_LEN + k_DATA_OBJ_LEN))
    /* copy SOD entry data into SSC FSM */
    SFS_NET_CPY_DOMSTR(&(ps_txBuf->ab_frmData[k_OFS_PYLD_INIT_SEG]),
                       ps_segInfo->pb_rawData, ps_segInfo->b_rawDataLen);
  #endif

  /* copy entry size into SSC FSM */
  SFS_NET_CPY32(&(ps_txBuf->ab_frmData[k_OFS_OBJ_LEN]),
                &ps_segInfo->dw_entrySize);

  /* transmit SSDO Service Response of upload initialization */
  SSDOS_SodAcsResp(B_INSTNUM_ ps_rxBuf, ps_txBuf, k_UPLD_INIT_SEG,
                   EPLS_k_TR_NOT_USED,(UINT8)EPLS_cfg_MAX_PYLD_LEN,TRUE);

  /* stored the transferred data length */
  ps_segInfo->dw_tranfDataLen = ps_segInfo->b_rawDataLen;

  SCFM_TACK_PATH();
}

/**
* @brief This function processes a SSDO segmented upload request.
*
* @param      b_instNum            instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()),
*       valid range: 0 .. ( EPLS_cfg_MAX_INSTANCES - 1 )
*
* @param      ps_rxBuf             reference to received openSAFETY frame  to be distributed (pointer not
*       checked, only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @param      ps_txBuf             reference to openSAFETY frame  to be transmitted (pointer not checked,
*       only called with reference to struct in processStateReqProc()), valid range: <> NULL, see EPLS_t_FRM
*
* @retval        po_end               only relevant if SOD_ABT_NO_ERROR returned,
* - TRUE : end segment
* - FALSE : middle segment (pointer not checked, only called with reference to variable), valid range: TRUE, FALSE
*
* @param      b_echoSaCmd          the SOD access command to be sent back (not checked, any value allowed), valid range: UINT8
*
* @return
* - if SOD_ABT_NO_ERROR no abort response has to be sent,
* - otherwise abort response has to be sent with the returned abort code. See SOD_t_ABORT_CODES
*/
UINT32 SSDOS_UpldSegReqProc(BYTE_B_INSTNUM_ const EPLS_t_FRM *ps_rxBuf,
                            EPLS_t_FRM *ps_txBuf, BOOLEAN *po_end,
                            UINT8 b_echoSaCmd)
{
  UINT32 dw_abortCode = (UINT32)SOD_ABT_GENERAL_ERROR; /* return value */
  t_SEG_INFO *ps_segInfo = &as_SegInfo[B_INSTNUMidx]; /* pointer to the segment
                                                         info structure */
  UINT8 b_pyldLenResp; /* response payload length */
  UINT32 dw_restDataLen; /* rest data length */

  /* calculate rest data length */
  dw_restDataLen = ps_segInfo->dw_entrySize - ps_segInfo->dw_tranfDataLen;

  /* if middle segment has to be sent */
  if(dw_restDataLen > (UINT32)((UINT8)EPLS_cfg_MAX_PYLD_LEN - k_DATA_HDR_LEN_SEG))
  {
    /* set payload size to maximum number of SOD entry data bytes to be
      transmitted into MIDDLE segment */
    ps_segInfo->b_rawDataLen = (UINT8)((UINT8)EPLS_cfg_MAX_PYLD_LEN -
                                       k_DATA_HDR_LEN_SEG);
    *po_end = FALSE;
  }
  else /* end segment has to be sent */
  {
    /* set payload size to remaining number of SOD entry data bytes
       to be transmitted into END segment */
    ps_segInfo->b_rawDataLen = (UINT8)(dw_restDataLen);

    *po_end = TRUE;

    /* set the end bit in the SOD access command */
    EPLS_BIT_SET_U8(b_echoSaCmd, k_ENDSEG_SACMD);
  }

  ps_segInfo->pb_rawData =
      SSDOS_SodAcsRead(B_INSTNUM_ ps_segInfo->dw_tranfDataLen,
                       (UINT32)ps_segInfo->b_rawDataLen, &dw_abortCode);

  /* if no error happened */
  if (dw_abortCode == (UINT32)SOD_ABT_NO_ERROR)
  {
    /* if response data cannot be copied to SSC FSM */
    SFS_NET_CPY_DOMSTR(&(ps_txBuf->ab_frmData[k_OFS_PYLD_SEG]),
                       ps_segInfo->pb_rawData, ps_segInfo->b_rawDataLen);

    b_pyldLenResp = (UINT8)(k_DATA_HDR_LEN_SEG + ps_segInfo->b_rawDataLen);

    /* transmit SSDO Service Response of upload initialization */
    SSDOS_SodAcsResp(B_INSTNUM_ ps_rxBuf, ps_txBuf, b_echoSaCmd,
                     EPLS_k_TR_NOT_USED,b_pyldLenResp,FALSE);

    /* stored the transferred data length */
    ps_segInfo->dw_tranfDataLen = ps_segInfo->dw_tranfDataLen +
                                  ps_segInfo->b_rawDataLen;
  }
  /* no else : abort response will be sent */
  SCFM_TACK_PATH();
  return dw_abortCode;
}

/** @} */
