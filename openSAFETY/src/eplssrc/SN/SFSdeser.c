/**
 * @addtogroup SFS
 * @{
 *
 * @file SFSdeser.c
 *
 * The file implements all functionality of the unit SFS that is
 * needed to decompose an openSAFETY frame into its single values.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 * @author K. Fahrion, IXXAT Automation GmbH
 *
 * - check frame header information (rx)
 * - check frame payload data and CRC (rx)
 * - provide frame header info and data to the stack (de-serialization of an openSAFETY frame)
 *
 * <h2>History for SFSmain.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>15.03.2011</td><td>Hans Pill</td><td>changes for new CRC polynomial A&P254590</td></tr>
 *     <tr><td>16.03.2011</td><td>Hans Pill</td><td>Review SL V21</td></tr>
 *     <tr><td>15.03.2013</td><td>Hans Pill</td><td>added statistic counters</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>changes for SPDOs with 40 bit CT</td></tr>
 * </table>
 *
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "EPLScfgCheck.h"

#include "SCFMapi.h"
#include "SHNF.h"
#include "SERRapi.h"
#include "SERR.h"
#include "SFS.h"
#include "SFSint.h"
#include "SFSerr.h"

#include "SPDOint.h"
#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
  #include "SDN.h"
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */


/**
 * Frame ID of the SPDO data only frame to check the target address exception.
*/
#define k_SPDO_DATA_ONLY    0x30U

/**
 * This bit represents the valid connection in the ID field of the SPDO.
*/
#define k_FRAME_BIT_CONN_VALID        (UINT8)0x01  /* connection valid bit */

/**
 * This symbol represents the SSDOC frame type Service Request Fast.
*/
#define k_SERVICE_REQ_FAST            0x3Au  /* frame type service req */

/**
 * This symbol represents the SSDOC frame type Service Response Fast.
*/
#define k_SERVICE_RES_FAST            0x3Bu  /* frame type service res */

/**
 * @brief This function provides frame header info and payload data of a received openSAFETY frame.
 *
 * Header info is
 * provided by using a data structure of type EPLS_t_FRM_HDR. The calling function has to allocate memory to
 * hold this data structure. Data is provided by reference to payload data of sub frame one. This function also
 * verifies payload data and header info of the received openSAFETY frame. Only valid data is provided to the calling
 * function. The memory block of the received openSAFETY frame is released by the application after return from stack call.
 *
 * @see          EPLS_t_FRM_HDR
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame() or SPDO_ProcessRxSpdo()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        pb_frame           reference to a received openSAFETY frame (not checked, checked in SPDO_ProcessRxSpdo() and processStateDeSer()) valid range: <> NULL
 *
 * @param        w_frmLen           received openSAFETY frame length provided by SAPL (checked) valid range: k_MIN_TEL_LEN_SHORT .. k_MAX_TEL_LEN_SHORT k_MIN_TEL_LEN_LONG .. k_MAX_TEL_LEN_LONG
 *
 * @param        ps_hdrInfo         reference to verified frame header info. (not checked, only called with reference to struct) valid range: <> NULL
 *
 * @return
 *  - == NULL          - openSAFETY frame de-serialized failed
 *  - <> NULL          - openSAFETY frame de-serialized successfully, reference to verified frame payload data
 */
const UINT8 * SFS_FrmDeSerialize(BYTE_B_INSTNUM_ UINT8 *pb_frame,
                                 UINT16 w_frmLen, EPLS_t_FRM_HDR *ps_hdrInfo)
{
  const UINT8 *pb_frmData = (UINT8 *)NULL; /* preparation of return value */

  /* 1st check is the frame header */
  UINT8 b_id1;
  UINT8 b_id2;
  UINT8 b_crc1 = 0;
  UINT8 b_crc2 = 0;
  UINT8 *pb_sub1;
  UINT8 *pb_sub2;
  UINT16  w_subFrm1Len;
  UINT16  w_subFrm2Len;
  UINT16  w_calcCrc1 = 0;
  UINT16  w_calcCrc2 = 0;
  UINT16  w_crc1 = 0;
  UINT16  w_crc2 = 0;
  UINT16  w_calcFrameLength;
  BOOLEAN o_doublePayload = TRUE;

#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
  UINT16 w_expSdn;
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */

  /* store received value from LE-field */
  SFS_NET_CPY8(&(ps_hdrInfo->b_le), &(pb_frame[k_SFRM1_HDR_LE]));

  /* get frame ID from sub frame ONE */
  SFS_NET_CPY8(&b_id1, &(pb_frame[k_SFRM1_HDR_ID]));
  b_id1 = (UINT8)(b_id1 >> 2u);

  /* payload data only once */
  if ((k_SERVICE_REQ_FAST == b_id1) ||
    (k_SERVICE_RES_FAST == b_id1))
  {
    o_doublePayload     = FALSE;
    w_subFrm1Len      = k_SFRM1_HDR_LEN + ps_hdrInfo->b_le;
    w_subFrm2Len      = k_SFRM2_HDR_LEN;
    pb_sub1         = pb_frame;

    /* check frame length */
      if ( k_MAX_DATA_LEN_SHORT >= ps_hdrInfo->b_le)
      {
        pb_sub2           = pb_frame + w_subFrm1Len + k_CRC_LEN_SHORT;
        w_calcFrameLength = k_SFRM1_HDR_LEN + k_SFRM2_HDR_LEN + (2*k_CRC_LEN_SHORT) + ps_hdrInfo->b_le;
      }
      else
      {
        pb_sub2           = pb_frame + w_subFrm1Len + k_CRC_LEN_LONG;
        w_calcFrameLength = k_SFRM1_HDR_LEN + k_SFRM2_HDR_LEN + (2*k_CRC_LEN_LONG) + ps_hdrInfo->b_le;
      }
  }
  /* payload data twice */
  else
  {
    o_doublePayload     = TRUE;
    w_subFrm1Len      = k_SFRM1_HDR_LEN + ps_hdrInfo->b_le;
    w_subFrm2Len      = k_SFRM2_HDR_LEN + ps_hdrInfo->b_le;
    pb_sub1         = pb_frame;

    /* check frame length */
      if ( k_MAX_DATA_LEN_SHORT >= ps_hdrInfo->b_le)
      {
        pb_sub2       = pb_frame + w_subFrm1Len + k_CRC_LEN_SHORT;
        w_calcFrameLength = k_SFRM1_HDR_LEN + k_SFRM2_HDR_LEN + (2*k_CRC_LEN_SHORT) + (2*ps_hdrInfo->b_le);
      }
      else
      {
        pb_sub2       = pb_frame + w_subFrm1Len + k_CRC_LEN_LONG;
        w_calcFrameLength = k_SFRM1_HDR_LEN + k_SFRM2_HDR_LEN + (2*k_CRC_LEN_LONG) + (2*ps_hdrInfo->b_le);
      }
  }
  /* check for errors */
  if ( w_frmLen != w_calcFrameLength)
  {
	/* count all errornous telegrams */
    SERR_CountCommonEvt(B_INSTNUM_ SERR_k_SFS_LENGTH);
    if ( k_MAX_DATA_LEN_SHORT >= ps_hdrInfo->b_le)
    {
      /* Error: inconsistence between number of payload data bytes and
       * telegram size! */
      SERR_SetError(B_INSTNUM_ SFS_k_ERR_RXFRM_CRC8_DATALEN_INCONS,
              (UINT32)ps_hdrInfo->b_le | ((UINT32)b_id1 << 8));
    }
    else
    {
      /* Error: inconsistence between number of payload data bytes and
       * telegram size! */
      SERR_SetError(B_INSTNUM_ SFS_k_ERR_RXFRM_CRC16_DATALEN_INCONS,
              (UINT32)ps_hdrInfo->b_le | ((UINT32)b_id1 << 8));
    }
  }
  /* Code stays in just to point out that the checks have to be done.
   * Those checks are done implicitely by checking the frame length
   * against the calculated frame length.
   * That way the even/odd check is already done.
   * Also the frame may never be shorter than the minimum.
   * And to ensure that the frame is not bigger than the maximum
   * the b_le byte is checked.
   * With this byte the expected frame length is calculated.
   * therefore it cannot happen that the frame length and the b_le byte do not match. */
//  else if ((o_doublePayload) &&
//       (0 == (w_frmLen % 2u)))
//  {
//    /* error: the received number of payload data bytes is incorrect! */
//    SERR_SetError(B_INSTNUM_ SFS_k_ERR_RXFRM_DATALEN_INV, (UINT32)ps_hdrInfo->b_le);
//  }
//  else if ((k_MAX_TEL_LEN_LONG < w_frmLen) ||
//       (k_MIN_TEL_LEN_SHORT > w_frmLen))
//  {
//    /* error: telegram length is invalid.
//     * Valid values are {11 .. 27} or {31 .. 521} */
//    SERR_SetError(B_INSTNUM_ SFS_k_ERR_RX_TELLEN_INV, (UINT32)w_frmLen);
//  }
  else if ( k_MAX_DATA_LEN < ps_hdrInfo->b_le)
  {
	/* count all errornous telegrams */
    SERR_CountCommonEvt(B_INSTNUM_ SERR_k_SFS_TOO_LONG);
    /* error: the received number of payload data bytes is incorrect! */
    SERR_SetError(B_INSTNUM_ SFS_k_ERR_RXFRM_DATALEN_INV, (UINT32)ps_hdrInfo->b_le);
  }
  else
  {
    /* UDID of the SCM decoding */
    SFS_ScmUdidCode(B_INSTNUM_ b_id1, pb_sub2);

    /* store Time Request Distinctive Number (TR) encoded into sub
     * frame TWO, subsequent to the Time Request Address (TADR) */
    SFS_NET_CPY8(&ps_hdrInfo->b_tr, &pb_sub2[k_SFRM2_HDR_TR]);
    ps_hdrInfo->b_tr = (UINT8)(ps_hdrInfo->b_tr & k_TR_MASK_AND);
    ps_hdrInfo->b_tr = (UINT8)(ps_hdrInfo->b_tr >> 2u);

#if (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE)
    /* only for SPDO data only frames
     * the value in the TR field does NOT need to be checked because
     * if the extended CT is not used the value in the extended CT field is zero*/
    if ((k_FRAME_ID_DATA_ONLY == (b_id1 & ~k_FRAME_BIT_CONN_VALID)) &&
        ((k_TR_EXT_CT_AVAILABLE | k_TR_EXT_CT_USED) == (ps_hdrInfo->b_tr & k_TR_EXT_CT_MASK)))
    {
        w_expSdn = SDN_GetSdn(B_INSTNUM);
        /* get the extended CT value */
        *((UINT8*)&ps_hdrInfo->dw_extCt + 0) = *((UINT8*)&pb_sub2[k_SFRM2_HDR_ADR]) ^ (UINT8)(w_expSdn >> 0) ^ *((UINT8*)&pb_sub1[k_SFRM1_HDR_ADR]);
        *((UINT8*)&ps_hdrInfo->dw_extCt + 1) = *((UINT8*)&pb_sub2[k_SFRM2_HDR_ID]) ^ (UINT8)(w_expSdn >> 8) ^ *((UINT8*)&pb_sub1[k_SFRM1_HDR_ID]);
        *((UINT8*)&ps_hdrInfo->dw_extCt + 2) = *((UINT8*)&pb_sub2[k_SFRM2_HDR_TADR]);
        *((UINT8*)&ps_hdrInfo->dw_extCt + 3) = 0;

        /* correct the frame for further processing */
        *((UINT8*)&pb_sub2[k_SFRM2_HDR_ADR]) ^= *((UINT8*)&ps_hdrInfo->dw_extCt + 0);
        *((UINT8*)&pb_sub2[k_SFRM2_HDR_ID]) ^= *((UINT8*)&ps_hdrInfo->dw_extCt + 1);
        *((UINT8*)&pb_sub2[k_SFRM2_HDR_TADR]) = 0;
    }
    else
    {
        ps_hdrInfo->dw_extCt = 0;
    }
#endif /* (SPDO_cfg_40_BIT_CT_SUPPORT == EPLS_k_ENABLE) */

    /* get the ID from sub-Frame 2 */
    SFS_NET_CPY8(&b_id2, &(pb_sub2[k_SFRM2_HDR_ID]));
    b_id2 = (UINT8)(b_id2 >> 2u);

    /* frame seemst to be OK so far -> get more data and do additional checks */

    /* store received value from ID-field */
    ps_hdrInfo->b_id = b_id1;

    /* store received logical address (SADR) of destination SN */
    SFS_NET_CPY16(&(ps_hdrInfo->w_adr), &(pb_sub1[k_SFRM1_HDR_ADR]));
    ps_hdrInfo->w_adr = (UINT16)(ps_hdrInfo->w_adr & k_ADR_MASK_AND);
    /* store Safety Domain Number (SDN) encoded in sub frame TWO */
    SFS_NET_CPY8(&LOW8(ps_hdrInfo->w_sdn), &(pb_sub2[k_SFRM2_HDR_ADR]));
    SFS_NET_CPY8(&HIGH8(ps_hdrInfo->w_sdn),&(pb_sub2[k_SFRM2_HDR_ID]));
    HIGH8(ps_hdrInfo->w_sdn) =
      (UINT8)(HIGH8(ps_hdrInfo->w_sdn) & k_SDN_HB_MASK_AND);
    ps_hdrInfo->w_sdn = (UINT16)(ps_hdrInfo->w_sdn ^ ps_hdrInfo->w_adr);

    /* store Consecutive Time seperated into sub frame ONE and TWO */
    SFS_NET_CPY8(&LOW8(ps_hdrInfo->w_ct), &(pb_sub1[k_SFRM1_HDR_CT]));
    SFS_NET_CPY8(&HIGH8(ps_hdrInfo->w_ct), &(pb_sub2[k_SFRM2_HDR_CT]));
    /* store Time Request Address (TADR) encoded into sub frame TWO,
     * subsequent to the high byte of the Consecutive Time (CT) */
    SFS_NET_CPY8(&LOW8(ps_hdrInfo->w_tadr), &(pb_sub2[k_SFRM2_HDR_TADR]));
    SFS_NET_CPY8(&HIGH8(ps_hdrInfo->w_tadr), &(pb_sub2[k_SFRM2_HDR_TR]));
    HIGH8(ps_hdrInfo->w_tadr) = (UINT8)(HIGH8(ps_hdrInfo->w_tadr) &
                                         k_TADR_MASK_AND);

    /* if SADR is valid */
    if ((EPLS_k_MIN_SADR > ps_hdrInfo->w_adr) ||
      (EPLS_k_MAX_SADR < ps_hdrInfo->w_adr))
    {
      /* count all errornous telegrams */
      SERR_CountCommonEvt(B_INSTNUM_ SERR_k_SFS_SADR_INV);
      SERR_SetError(B_INSTNUM_ SFS_k_ERR_RX_SADR_INV, (UINT32)(ps_hdrInfo->w_adr));
    }
    else if ( b_id1 != b_id2)
    {
      /* count all errornous telegrams */
      SERR_CountCommonEvt(B_INSTNUM_ SERR_k_SFS_FRM_ID);
      /* error: different frame Id's received in sub frame ONE and TWO
       * within the received openSAFETY frame */
      SERR_SetError(B_INSTNUM_ SFS_k_ERR_RXFRM_ID_INV,
              (((UINT32)(b_id1))<<16u) + b_id2);
    }
    else if ((k_MIN_SDN > ps_hdrInfo->w_sdn) ||
         (k_MAX_SDN < ps_hdrInfo->w_sdn))
    {
      /* count all errornous telegrams */
      SERR_CountCommonEvt(B_INSTNUM_ SERR_k_SFS_SDN_INV);
      SERR_SetError(B_INSTNUM_ SFS_k_ERR_RX_SDN_INV, (UINT32)(ps_hdrInfo->w_sdn));
    }
    else if (((EPLS_k_MIN_SADR > ps_hdrInfo->w_tadr) ||
          (EPLS_k_MAX_SADR < ps_hdrInfo->w_tadr)) &&
         ((0 != ps_hdrInfo->w_tadr) ||
          (k_SPDO_DATA_ONLY != (b_id1 & ~k_FRAME_BIT_CONN_VALID))))
    {
      /* count all errornous telegrams */
      SERR_CountCommonEvt(B_INSTNUM_ SERR_k_SFS_TADR_INV);
      SERR_SetError(B_INSTNUM_ SFS_k_ERR_RX_TADR_INV, (UINT32)(ps_hdrInfo->w_tadr));
    }
    else
    {
      /* frame header OK -> check frame CRC */
      if ( k_MAX_DATA_LEN_SHORT >= ps_hdrInfo->b_le)
      {
        /* copy 8 bit CRC and take network addressing format into account */
        /* use UINT8 vars to prevent high-byte access on big endian targets */
        SFS_NET_CPY8(&b_crc1, ADD_OFFSET(&pb_sub1[0], w_subFrm1Len));
        w_crc1 = b_crc1;
        SFS_NET_CPY8(&b_crc2, ADD_OFFSET(&pb_sub2[0], w_subFrm2Len));
        w_crc2 = b_crc2;

        /* calculate the CRC */
        w_calcCrc1 = SHNF_Crc8Calc(0U, (INT32)w_subFrm1Len, (void *)&pb_sub1[0]);
        w_calcCrc2 = SHNF_Crc8Calc(0U, (INT32)w_subFrm2Len, (void *)&pb_sub2[0]);

      }
      else
      {
        /* copy 8 bit CRC and take network addressing format into account */
        SFS_NET_CPY16(&w_crc1, ADD_OFFSET(&pb_sub1[0], w_subFrm1Len));
        SFS_NET_CPY16(&w_crc2, ADD_OFFSET(&pb_sub2[0], w_subFrm2Len));

        /* calc slim SSDOs with different CRC16 */
        if ((k_SERVICE_REQ_FAST == b_id1) ||
            (k_SERVICE_RES_FAST == b_id1))
        {
			/* calculate the CRC */
			w_calcCrc1 = SHNF_Crc16CalcSlim(0U, (INT32)w_subFrm1Len, (void *)&pb_sub1[0]);
			w_calcCrc2 = SHNF_Crc16CalcSlim(0U, (INT32)w_subFrm2Len, (void *)&pb_sub2[0]);
        }
        else
        {
			/* calculate the CRC */
			w_calcCrc1 = SHNF_Crc16Calc(0U, (INT32)w_subFrm1Len, (void *)&pb_sub1[0]);
			w_calcCrc2 = SHNF_Crc16Calc(0U, (INT32)w_subFrm2Len, (void *)&pb_sub2[0]);
        }
      }

      if ( w_crc1 != w_calcCrc1)
      {
        /* count all errornous telegrams */
        SERR_CountCommonEvt(B_INSTNUM_ SERR_k_SFS_CRC1);
        if ( k_MAX_DATA_LEN_SHORT >= ps_hdrInfo->b_le)
        {
          /* error: received CRC in sub frame ONE is wrong */
          SERR_SetError(EPLS_k_NO_INSTANCE_ SFS_k_ERR_SF1_RX_CRC1_INV,
                        (UINT32)(w_crc1));
        }
        else
        {
          /* error: received CRC in sub frame ONE is wrong */
          SERR_SetError(EPLS_k_NO_INSTANCE_ SFS_k_ERR_SF1_RX_CRC2_INV,
                        (UINT32)(w_crc1));
        }
      }
      else if ( w_crc2 != w_calcCrc2)
      {
        /* count all errornous telegrams */
        SERR_CountCommonEvt(B_INSTNUM_ SERR_k_SFS_CRC2);
        if ( k_MAX_DATA_LEN_SHORT >= ps_hdrInfo->b_le)
        {
          /* error: received CRC in sub frame ONE is wrong */
          SERR_SetError(EPLS_k_NO_INSTANCE_ SFS_k_ERR_SF2_RX_CRC1_INV,
                        (UINT32)(w_crc2));
        }
        else
        {
          /* error: received CRC in sub frame ONE is wrong */
          SERR_SetError(EPLS_k_NO_INSTANCE_ SFS_k_ERR_SF2_RX_CRC2_INV,
                        (UINT32)(w_crc2));
        }
      }
      else if ((o_doublePayload) &&
           (MEMCMP_IDENT != MEMCOMP(pb_sub1 + k_SFRM1_DATA, pb_sub2 + k_SFRM2_DATA, ps_hdrInfo->b_le)))
      {
        /* count all errornous telegrams */
        SERR_CountCommonEvt(B_INSTNUM_ SERR_k_SFS_DATA);
        /* error: payload data of received openSAFETY NOT correct! */
        SERR_SetError(B_INSTNUM_ SFS_k_ERR_RXDATA_INV, SERR_k_NO_ADD_INFO);
      }
      else
      {
        /* frame correct */
        pb_frmData = pb_sub1 + k_SFRM1_DATA;
        SCFM_TACK_PATH();
      }
    }
  }

  SCFM_TACK_PATH();
  return pb_frmData;
}

/** @} */
