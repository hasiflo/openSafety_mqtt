/**
 * @addtogroup SFS
 * @{
 *
 * @file SFSmain.c
 *
 * The file implements all functionality of the unit SFS that is needed to get a memory
 * block to assemble an openSAFETY frame and to provide an error string to all error
 * that can occur in this unit.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 * @author K. Fahrion, IXXAT Automation GmbH
 *
 *
 * <h2>History for SFSmain.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>04.06.2010</td><td>Hans Pill</td><td>moved array to safe data segment</td></tr>
 *     <tr><td>24.11.2010</td><td>Hans Pill</td><td>Review SL V20</td></tr>
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

#include "SODapi.h"
#include "SOD.h"

#include "SFS.h"
#include "SFSint.h"
#include "SFSerr.h"


/**
 * @var pb_ScmUdid
 *
 * Array with pointers to the Udid of the SCM SOD object with index 0x1200 and sub-index 0x04 for the instances.
*/
static const UINT8 *pb_ScmUdid[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;


/**
 * @brief This function initializes the read pointer for the UDID of the SCM SOD object with index 0x1200 and sub-index 0x04.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - initialization succeed
 * - FALSE          - initialization failed
 */
BOOLEAN SFS_Init(BYTE_B_INSTNUM)
{
  BOOLEAN o_return = FALSE;  /* predefined return value */
  SOD_t_ACS_OBJECT_VIRT s_acsScmUdid; /* SOD access structure for the UDID of the SCM */
  SOD_t_ERROR_RESULT s_errRes; /* SOD error result */


  s_acsScmUdid.s_obj.w_index = EPLS_k_IDX_COMMON_COM_PARAM;
  s_acsScmUdid.s_obj.b_subIndex = EPLS_k_SUBIDX_UDID_SCM;
  s_acsScmUdid.dw_segOfs = 0;
  s_acsScmUdid.dw_segSize = 0;

  /* if the attribute access succeeded */
  if (NULL != SOD_AttrGetVirt(B_INSTNUM_ &s_acsScmUdid, &s_errRes))
  {
    /* if before read callback is not registered */
    if (EPLS_IS_BIT_RESET(s_acsScmUdid.s_obj.s_attr.w_attr, SOD_k_ATTR_BEF_RD))
    {
      /* get the reference to the UDID of the SCM */
      pb_ScmUdid[B_INSTNUMidx] = (UINT8 *)SOD_ReadVirt(B_INSTNUM_
                                                   &s_acsScmUdid,
                                                   &s_errRes);

      /* if the SOD read access succeeded */
      if (pb_ScmUdid[B_INSTNUMidx] != NULL)
      {
        o_return = TRUE;
      }
      else /* the SOD read access failed */
      {
        /* SOD error is reported */
        SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                      (UINT32)s_errRes.e_abortCode);
      }
    }
    else /* before read callback is registered */
    {
      SERR_SetError(B_INSTNUM_ SFS_k_ERR_BR_CLBK, SERR_k_NO_ADD_INFO);
    }
  }
  else /* the attribute access failed */
  {
    /* SOD error is reported */
    SERR_SetError(B_INSTNUM_ s_errRes.w_errorCode,
                  (UINT32)s_errRes.e_abortCode);
  }

  SCFM_TACK_PATH();
  return o_return;
}

/**
* @brief This function encodes or decodes the data of the sub frame TWO of SPDO and SSDO (not SNMT) frames with the
* UDID of the SCM using a logical XOR operation.
*
* @param        b_instNum            instance number (not checked, called with EPLS_k_SCM_INST_NUM or checked in
*       SSC_ProcessSNMTSSDOFrame() or SPDO_BuildTxSpdo())) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
*
* @param        b_frmId              frame identifier of the openSAFETY frame (not checked, checked in checkTxFrameHeader()),
*       valid range : UINT8
*
* @retval       pb_subFrmTwo         pointer to the openSAFETY sub frame TWO (not checked, only called with reference to
*       array in SFS_FrmSerialize() and SFS_FrmDeSerialize()), valid range: <> NULL
*/
void SFS_ScmUdidCode(BYTE_B_INSTNUM_ UINT8 b_frmId, UINT8 *pb_subFrmTwo)
{
  /* if no SNMT frame */
  if ((b_frmId & k_FRAME_ID_MASK) != k_SNMT_FRAME_ID)
  {
    /* logical XOR operation for every SCM UDID bytes */
    (*(ADD_OFFSET(pb_subFrmTwo, 0))) =
        (UINT8)(((*(ADD_OFFSET(pb_subFrmTwo, 0))) ^
                  (*(ADD_OFFSET(pb_ScmUdid[B_INSTNUMidx], 0)))));
    (*(ADD_OFFSET(pb_subFrmTwo, 1))) =
        (UINT8)(((*(ADD_OFFSET(pb_subFrmTwo, 1))) ^
                  (*(ADD_OFFSET(pb_ScmUdid[B_INSTNUMidx], 1)))));
    (*(ADD_OFFSET(pb_subFrmTwo, 2))) =
        (UINT8)(((*(ADD_OFFSET(pb_subFrmTwo, 2))) ^
                  (*(ADD_OFFSET(pb_ScmUdid[B_INSTNUMidx], 2)))));
    (*(ADD_OFFSET(pb_subFrmTwo, 3))) =
        (UINT8)(((*(ADD_OFFSET(pb_subFrmTwo, 3))) ^
                  (*(ADD_OFFSET(pb_ScmUdid[B_INSTNUMidx], 3)))));
    (*(ADD_OFFSET(pb_subFrmTwo, 4))) =
        (UINT8)(((*(ADD_OFFSET(pb_subFrmTwo, 4))) ^
                  (*(ADD_OFFSET(pb_ScmUdid[B_INSTNUMidx], 4)))));
    (*(ADD_OFFSET(pb_subFrmTwo, 5))) =
        (UINT8)(((*(ADD_OFFSET(pb_subFrmTwo, 5))) ^
                  (*(ADD_OFFSET(pb_ScmUdid[B_INSTNUMidx], 5)))));
  }
  /* no else : SNMT frame, XOR operation is not needed */

  SCFM_TACK_PATH();
}

/**
 * @brief This function requests a memory block from the SHNF to build an EPLsaftey frame for transmission.
 *
 *
 * The frame length (=memory block size) depends on the payload data length, because 1 byte CRC is used for
 * payload data length <= 8 byte. 2 byte CRC is used for payload data length > 8 byte. The allocated memory
 * is marked as "ready to process" by the function SFS_FrmSerialize.
 *
 * @see          SFS_FrmSerialize()
 *
 * @param        b_instNum           instance number (not checked, called with EPLS_k_SCM_INST_NUM or checked
 *                  in SSC_ProcessSNMTSSDOFrame() or SPDO_BuildTxSpdo()), valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        e_telType           telegram type (SPDO / SSDO / SNMT)(not checked, only called with enum value)
 *                  valid range: see SHNF_t_TEL_TYPE
 *
 * @param        w_spdoNum           SPDO number in case of telegram type SPDO, (not checked, checked in
 *                  SPDO_BuildTxSpdo()) in case of telegram type SSDO/SNMT use SFS_k_NO_SPDO instead valid range: any 16 bit value
 *
 * @param        b_dataLength           payload data length (checked) valid range: <= k_MAX_DATA_LEN
 *
 * @return
 * - == NULL           - memory allocation failed
 * - <> NULL           - memory allocation succeeded, reference to a memory block that stores an openSAFETY frame that transfers payload data of specified size, SFS_k_FRM_DATA_OFFSET is used to address the start of payload data
 */
UINT8 * SFS_GetMemBlock(BYTE_B_INSTNUM_ SHNF_t_TEL_TYPE e_telType,
                        UINT16 w_spdoNum, UINT8 b_dataLength)
{
  UINT16 w_calcFrmLen = 0x0000u;   /* calculated openSAFETY frame length */
  UINT8 *pb_frame =(UINT8 *) NULL; /* reference to an openSAFETY frame that stores the
                                      specified number of payload data bytes */


  /* if payload data length is valid */
  if(b_dataLength <= k_MAX_DATA_LEN)
  {

    if ( SHNF_k_SSDO_SLIM == e_telType)
    {
      /* if payload data length is less <k_MIN_DATA_LEN_LONG> bytes,
         use 1 byte CRC for frame length calculation. */
      if(b_dataLength < k_MIN_DATA_LEN_LONG)
      {
        w_calcFrmLen =
          (UINT16)(k_FRM_LEN_FIX_SHORT + b_dataLength);
      }
      /* else payload data length is greater 8 byte, use 2 byte CRC for frame
         length calculation. */
      else
      {
        w_calcFrmLen =
          (UINT16)(k_FRM_LEN_FIX_LONG + b_dataLength);
      }
    }
    else
    {
      /* if payload data length is less <k_MIN_DATA_LEN_LONG> bytes,
         use 1 byte CRC for frame length calculation. */
      if(b_dataLength < k_MIN_DATA_LEN_LONG)
      {
        w_calcFrmLen =
          (UINT16)(k_FRM_LEN_FIX_SHORT + b_dataLength + b_dataLength);
      }
      /* else payload data length is greater 8 byte, use 2 byte CRC for frame
         length calculation. */
      else
      {
        w_calcFrmLen =
          (UINT16)(k_FRM_LEN_FIX_LONG + b_dataLength + b_dataLength);
      }
    }

    /* get memory block of calculated size from unit SHNF */
    pb_frame =
      SHNF_GetTxMemBlock(B_INSTNUM_ w_calcFrmLen, e_telType, w_spdoNum);

    /* if reference from SHNF is NOT valid */
    if(pb_frame == NULL)
    {
      /* error: SHNF could NOT deliver a valid reference to the requested
                memory block */
      SERR_SetError(B_INSTNUM_ SFS_k_ERR_NO_MEM_FROM_SHNF, SERR_k_NO_ADD_INFO);
    }
    /* else reference to requested memory block is valid */
  }
  else  /* payload data length is NOT in range */
  {
    /* error: the maximum payload data length is 254 bytes. */
    SERR_SetError(B_INSTNUM_ SFS_k_ERR_DATALEN_INV, (UINT32)b_dataLength);
  }
  SCFM_TACK_PATH();
  return pb_frame;
}


#if(EPLS_cfg_ERROR_STRING == EPLS_k_ENABLE)
/**
* @brief This function returns a description of every available errors of the assigned unit.
*
* @param        w_errorCode        error number
*
* @param        dw_addInfo         additional error information
*
* @param        pac_str            empty buffer to build the error string
*
*/
  void SFS_GetErrorStr(UINT16 w_errorCode, UINT32 dw_addInfo, CHAR *pac_str)
  {
    /* Choose the matching error string */
    switch(w_errorCode)
    {
      case SFS_k_ERR_RXFRM_ID_INV:
      {
        SPRINTF3(pac_str,
          "%#x - SFS_k_ERR_RXFRM_ID_INV: checkRxDataLength():\n"
          "The received openSAFETY frame contains different frame Id's "
          "in sub frame ONE (%#x) and in sub frame TWO (%#x).",
          SFS_k_ERR_RXFRM_ID_INV,
          HIGH16(dw_addInfo), LOW16(dw_addInfo));
        break;
      }
      case SFS_k_ERR_RXFRM_CRC8_DATALEN_INCONS:
      {
        SPRINTF3(pac_str,
          "%#x - : checkRxDataLength():\n"
          "Inconsistence between data length (%02u bytes) and "
          "frame length (%02u bytes), if the frame uses CRC8.\n",
          SFS_k_ERR_RXFRM_CRC8_DATALEN_INCONS,
          HIGH16(dw_addInfo), LOW16(dw_addInfo));
        break;
      }
      case SFS_k_ERR_RXFRM_CRC16_DATALEN_INCONS:
      {
        SPRINTF3(pac_str,
          "%#x - SFS_k_ERR_RXFRM_CRC16_DATALEN_INCONS: checkRxDataLength():\n"
          "Inconsistence between data length (%02u bytes) and "
          "frame length (%02u bytes), if the frame uses CRC16.\n",
          SFS_k_ERR_RXFRM_CRC16_DATALEN_INCONS,
          HIGH16(dw_addInfo), LOW16(dw_addInfo));
        break;
      }
      case SFS_k_ERR_RXFRM_DATALEN_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_RXFRM_DATALEN_INV: checkRxDataLength():\n"
          "The value in the LE field (%02lu)of the received frame "
          "exceed valid range.\n", SFS_k_ERR_RXFRM_DATALEN_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_DATALEN_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_DATALEN_INV: SFS_GetMemBlock():\n"
          "The value in the LE field (%02lu) of the received frame "
          "exceeds valid range (<= k_MAX_DATA_LEN).\n", SFS_k_ERR_DATALEN_INV,
          dw_addInfo);
        break;
      }
      case SFS_k_ERR_RXDATA_INV:
      {
        SPRINTF1(pac_str,
          "%#x - SFS_k_ERR_RXDATA_INV: checkRxFrameData()\n"
          "The payload data of sub frame one and two is not equal.\n",
          SFS_k_ERR_RXDATA_INV);
        break;
      }
      case SFS_k_ERR_SF1_RX_CRC1_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_SF1_RX_CRC1_INV: checkRxCrc8():\n"
          "The checksum (CRC8) of sub frame one (%02lu) is NOT correct.\n",
          SFS_k_ERR_SF1_RX_CRC1_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_SF1_RX_CRC2_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_SF1_RX_CRC2_INV: checkRxCrc16():\n"
          "The checksum (CRC16) of sub frame one (%04lu) is NOT correct.\n",
          SFS_k_ERR_SF1_RX_CRC2_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_SF2_RX_CRC1_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_SF2_RX_CRC1_INV: checkRxCrc8():\n"
          "The checksum (CRC8) of sub frame two (%02lu) is NOT correct.\n",
          SFS_k_ERR_SF2_RX_CRC1_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_SF2_RX_CRC2_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_SF2_RX_CRC2_INV: checkRxCrc16():\n"
          "The checksum (CRC16) of sub frame two (%04lu) is NOT correct.\n",
          SFS_k_ERR_SF2_RX_CRC2_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_DATATYPE_NOT_DEF:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_DATATYPE_NOT_DEF: SFS_NetworkCopyGen():\n"
          "The type of data (%02lu) to be copied is NOT defined.\n",
          SFS_k_ERR_DATATYPE_NOT_DEF, dw_addInfo);
        break;
      }
      case SFS_k_ERR_RX_TELLEN_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_RX_TELLEN_INV: checkRxDataLength():\n"
          "The received frame length (%04lu) is NOT correct.\n",
          SFS_k_ERR_RX_TELLEN_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_RX_SADR_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_RX_SADR_INV: checkRxAddrInfo():\n"
          "The received address (%02lu) in the ADR field of EPLS "
          "sub frame one is invalid.\n",
          SFS_k_ERR_RX_SADR_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_RX_TADR_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_RX_TADR_INV: checkRxAddrInfo():\n"
          "The received address (%02lu) in the TADR field of EPLS "
          "sub frame one is invalid.\n",
          SFS_k_ERR_RX_TADR_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_RX_SDN_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_RX_SDN_INV: checkRxAddrInfo():\n"
          "The received domain number (%04lu) in the ADR field of EPLS "
          "sub frame two is invalid.\n",
          SFS_k_ERR_RX_SDN_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_TX_SADR_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_TX_SADR_INV: checkTxFrameHeader():\n"
          "The value in the ADR field (%04lu) of the frame to be "
          "transmitted is NOT correct.\n",
          SFS_k_ERR_TX_SADR_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_TX_FRMID_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_TX_FRMID_INV: checkTxFrameHeader():\n"
          "The frame ID (%04lu) of the frame to be transmitted "
          "is NOT correct.\n",
          SFS_k_ERR_TX_FRMID_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_TX_SDN_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_TX_SDN_INV: checkTxFrameHeader():\n"
          "The domain number (%04lu) of the frame to be transmitted "
          "is NOT correct.\n",
          SFS_k_ERR_TX_SDN_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_TX_LE_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_TX_LE_INV: checkTxFrameHeader():\n"
          "The payload data length (%04lu) of the frame to be transmitted "
          "is NOT correct.\n",
          SFS_k_ERR_TX_LE_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_TX_TADR_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_TX_TADR_INV: checkTxFrameHeader():\n"
          "The time request address (%04lu) of the frame to be transmitted "
          "is NOT correct.\n",
          SFS_k_ERR_TX_TADR_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_TX_TR_INV:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_TX_TR_INV: checkTxFrameHeader():\n"
          "The time request distinctive number (%02lu) of the frame "
          "to be transmitted is NOT correct.\n",
          SFS_k_ERR_TX_TR_INV, dw_addInfo);
        break;
      }
      case SFS_k_ERR_RX_FRMLEN_EVEN:
      {
        SPRINTF2(pac_str,
          "%#x - SFS_k_ERR_RX_FRMLEN_EVEN: checkRxDataLength():\n"
          "The frame length (%04lu) provided by the application is an "
          "even number, which is NOT possible.\n",
          SFS_k_ERR_RX_FRMLEN_EVEN, dw_addInfo);
        break;
      }
      case SFS_k_ERR_NO_MEM_FROM_SHNF:
      {
        SPRINTF1(pac_str,
          "%#x - SFS_k_ERR_NO_MEM_FROM_SHNF: SFS_GetMemBlock():\n"
          "No memory block for frame transmission available from unit SHNF.\n",
          SFS_k_ERR_NO_MEM_FROM_SHNF);
        break;
      }
      case SFS_k_ERR_SHNF_CANNOT_MARK_MEM_BLK:
      {
        SPRINTF1(pac_str,
          "%#x - SFS_k_ERR_SHNF_CANNOT_MARK_MEM_BLK: SFS_FrmSerialize():\n"
          "Memory block for frame transmission cannot be passed"
          " to unit SHNF.\n", SFS_k_ERR_SHNF_CANNOT_MARK_MEM_BLK);
        break;
      }
      case SFS_k_ERR_BR_CLBK:
      {
        SPRINTF1(pac_str,
          "%#x - SFS_k_ERR_BR_CLBK: SFS_Init():\n"
          "Before read callback is registered for the UDID of the SCM SOD "
          "object with index 0x1200 and sub-index 0x04\n", SFS_k_ERR_BR_CLBK);
        break;
      }
      default:
      {
        SPRINTF(pac_str, "Unknown error code was generated by the SFS\n");
      }
    }

    SCFM_TACK_PATH();
    return ;
    /* RSM_IGNORE_QUALITY_BEGIN Notice #28 - Cyclomatic complexity > 15
     	 	 	 	 	 	    Notice #17 - Function comment content less than
                                             10.0% */
  }
    /* RSM_IGNORE_QUALITY_END */
#endif


  /** @} */
  /** @} */
