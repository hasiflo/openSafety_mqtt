/**
 * @addtogroup SN
 * @{
 * @addtogroup SFS [SFS] Module to serialize/ de-serialize openSAFETY frames.
 * @{
 *
 * This unit provides all functionality to process openSAFETY frames.
 *
 * @file SFS.h
 *
 * This unit provides functionality to handle stack internal errors.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author K. Fahrion, IXXAT Automation GmbH
 * @author M. Molnar, IXXAT Automation  GmbH
 *
 * To transfer frame header
 * information from/to the unit SFS a data structure is used. The memory for this structure is
 * held by the calling unit. A reference to the structure is passed to unit SFS. Payload data
 * is also transferred by reference, thus no data duplicating is needed. In case of serializing
 * (transmitting an openSAFETY frame) the calling unit provides a memory block to build the
 * openSAFETY frame in. In case of de-serializing (receiving an openSAFETY frame) the openSAFETY
 * Stack is not affected by memory management. De-serializing an serializing includes a
 * verification (frame header and data). Additionally the unit SFS provides one copy function to
 * transfer data between host (openSAFETY Stack) and network (HNF). The effort of this function
 * is regarding different address formats of network endian) and host (big endian or little endian).
 *
 */
#ifndef SFS_H
#define SFS_H

/**
 * @name Constants and macros
 * @{
 */

/**
 * @def SFS_k_FRM_DATA_OFFSET
 * @brief This symbol defines a byte offset inside sub frame TWO of an openSAFETY frame.
 *
 * It is the offset to the start of its payload data. The symbol is
 * used by every function that calls SFS_GetMemBlock(). The calling function
 * needs the offset to store its payload data into the memory block of the
 * openSAFETY frame. SFS_GetMemBlock() provides memory to build the whole
 *  openSAFETY frame (sub frame ONE and TWO).
 */
#define SFS_k_FRM_DATA_OFFSET  5u

/**
 * @def SFS_k_NO_SPDO
 * @brief This symbol is used for function call SFS_GetMemBlock() in case of unit
 *  SSDO or SNMT is calling. It is used for parameter 3 &lt;w_spdoNum&gt;
 */
#define SFS_k_NO_SPDO                  0x0000u

/**
 * @def SFS_GET_MIN_FRM_ID
 * @brief This macro extracts bit 0, 1, 2 from the frame ID.
 *
 * These bits encode the
 * telegram type (minor frame id) which specifies the different telegrams
 * the assigned unit has to process.
 */
#define SFS_GET_MIN_FRM_ID(id)          (UINT8)((id) & 0x07u)

/** @} */

/**
 * @brief This function initializes the read pointer for the UDID of the SCM SOD object with index 0x1200 and sub-index 0x04.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll()) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @return
 * - TRUE           - initialization succeed
 * - FALSE          - initialization failed
 */
BOOLEAN SFS_Init(BYTE_B_INSTNUM);

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
 * @param        b_dataLength 		payload data length (checked) valid range: <= k_MAX_DATA_LEN
 *
 * @return
 * - == NULL           - memory allocation failed
 * - <> NULL           - memory allocation succeeded, reference to a memory block that stores an openSAFETY frame that transfers payload data of specified size, SFS_k_FRM_DATA_OFFSET is used to address the start of payload data
 */
UINT8 * SFS_GetMemBlock(BYTE_B_INSTNUM_ SHNF_t_TEL_TYPE e_telType,
        UINT16 w_spdoNum, UINT8 b_dataLength);

/**
 * @brief This function builds an openSAFETY frame and provides it to the HNF, which is in charge of transmission.
 *
 * The calling function has to request the memory block by using {SFS_GetMemBlock()}. The memory block has
 * to be of size of the complete openSAFETY frame and data to be transmitted is stored at the place inside the
 * memory block where payload data of sub frame one is located. The memory block is provided by reference
 * to this function. The calling function also has to provide the header info by reference. The data structure
 * must be of type EPLS_t_FRM_HDR and assembles all necessary header info to build the openSAFETY frame.
 * All frame header info is checked before is is put into the right order and stored into sub frame one and two.
 * Frame data, already stored in sub frame one, is copied into sub frame two. To pass the openSAFETY frame to the
 * SHNF and so transmitting it the function SHNF_MarkTxMemBlock() is used.
 *
 * @see          EPLS_t_FRM_HDR
 *
 * @param        b_instNum         instance number (not checked, called with EPLS_k_SCM_INST_NUM or checked in
 *                    SSC_ProcessSNMTSSDOFrame() or SPDO_BuildTxSpdo())) valid range: 0 .. &lt;EPLS_cfg_MAX_INSTANCES&gt; - 1
 *
 * @param        ps_hdrInfo        reference to header info, stored into an internal data structure (not checked,
 *                    only called with reference to struct), valid range: <> NULL
 *
 * @param        pb_frame          reference to an openSAFETY frame (payload data is already stored in sub frame ONE).
 *                    Reference is provided by function SFS_GetMemBlock() (not checked, checked in transmitRequest()
 *                    or SNMTM_SnErrorAck() or SNMTS_TxRespRstGuardScm() or SendTxSpdo() processStateSer() or
 *                    reqBufferSend()), valid range: <> NULL
 *
 * @return
 * - TRUE            - openSAFETY frame serialized and passed to the SHNF successfully
 * - FALSE           - error during marking the memory block openSAFETY frame passing failed
 */
BOOLEAN SFS_FrmSerialize(BYTE_B_INSTNUM_ const EPLS_t_FRM_HDR *ps_hdrInfo,
        UINT8 *pb_frame);

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
        UINT16 w_frmLen, EPLS_t_FRM_HDR *ps_hdrInfo);

/**
 * @brief This function swaps the data if it is necessary and copies.
 *
 * The data with the given data type will be
 * copied from source pointer to the destination pointer dependent on the data type. If the host format defined
 * by ENDIAN in the EPLStarget.h is big endian then the data will be swapped. Parameter dw_len is only used in
 * case of data types which contains any kind of strings with unknown length.
 * @attention The parameters will not be checked by this function.
 *
 * @param        pv_dst        destination pointer (not checked), valid range: <> NULL
 *
 * @param        pv_src        source pointer (not checked), valid range: <>NULL
 *
 * @param        dw_len        length of the data to be copied in case of type: EPLS_k_VISIBLE_STRING,
 *                             EPLS_k_DOMAIN, EPLS_k_OCTET_STRING (not checked, any value allowed), valid range: any 32 bit value
 *
 * @param        e_type        data type of the data to be copied (checked) valid range: see EPLS_t_DATATYPE
 *
 * @return
 * - TRUE        - copy process succeeded
 * - FALSE       - copy process failed, data type is NOT supported
 */
BOOLEAN SFS_NetworkCopyGen(void *pv_dst, const void *pv_src, UINT32 dw_len,
        EPLS_t_DATATYPE e_type);

/**
 * @brief This function copies the data of any 8 bit type from the source pointer to the destination pointer.
 * @attention The parameters will not be checked by this function. The function is implemented as a macro due to optimization.
 *
 * @param        pv_dst 		  destination pointer (not checked) valid range: <> NULL
 *
 * @param        pv_src        source pointer (not checked) valid range: <> NULL
 */
#define SFS_NET_CPY8(pv_dst, pv_src) \
        ((*((UINT8*)(pv_dst))) = (*((UINT8*)(pv_src))))

/**
 * @brief This function swaps the 16 bit data if it is necessary and copies.
 *
 * The data will be copied from the source pointer to the destination pointer. If the host format
 * defined by ENDIAN in the EPLStarget.h is big endian then the data will be swapped.
 * @attention The parameters will not be checked by this function. The function is implemented as a macro due to optimization.
 *
 * @param        pv_dst 		  destination pointer (not checked), valid range: <> NULL
 *
 * @param        pv_src        source pointer (not checked), valid range: <> NULL
 */
#if(ENDIAN == BIG)
#define SFS_NET_CPY16(pv_dst, pv_src)                             \
          ((*(((UINT8*)(pv_dst)) + 0u)) = (*(((UINT8*)(pv_src)) + 1u))); \
          ((*(((UINT8*)(pv_dst)) + 1u)) = (*(((UINT8*)(pv_src)) + 0u)))
#else  /* (ENDIAN == LITTLE) */
#define SFS_NET_CPY16(pv_dst, pv_src)                             \
          ((*(((UINT8*)(pv_dst)) + 0u)) = (*(((UINT8*)(pv_src)) + 0u))); \
          ((*(((UINT8*)(pv_dst)) + 1u)) = (*(((UINT8*)(pv_src)) + 1u)))
#endif  /* (ENDIAN == BIG) */

/**
 * @brief This function swaps the 32 bit data if it is necessary and copies.
 *
 * The data will be copied from the source pointer to the destination pointer.
 * If the host format defined by ENDIAN in the EPLStarget.h is big endian then the data will be swapped.
 * @attention The parameters will not be checked by this function. The function is implemented as a macro due to optimization.
 *
 * @param        pv_dst        destination pointer (not checked), valid range: <> NULL
 *
 * @param        pv_src        source pointer (not checked), valid range: <> NULL
 */
#if(ENDIAN == BIG)
#define SFS_NET_CPY32(pv_dst, pv_src)                              \
          ((*(((UINT8*)(pv_dst)) + 0u)) = (*(((UINT8*)(pv_src)) + 3u))); \
          ((*(((UINT8*)(pv_dst)) + 1u)) = (*(((UINT8*)(pv_src)) + 2u))); \
          ((*(((UINT8*)(pv_dst)) + 2u)) = (*(((UINT8*)(pv_src)) + 1u))); \
          ((*(((UINT8*)(pv_dst)) + 3u)) = (*(((UINT8*)(pv_src)) + 0u)))
#else  /* (ENDIAN == LITTLE) */
#define SFS_NET_CPY32(pv_dst, pv_src)                              \
          ((*(((UINT8*)(pv_dst)) + 0u)) = (*(((UINT8*)(pv_src)) + 0u))); \
          ((*(((UINT8*)(pv_dst)) + 1u)) = (*(((UINT8*)(pv_src)) + 1u))); \
          ((*(((UINT8*)(pv_dst)) + 2u)) = (*(((UINT8*)(pv_src)) + 2u))); \
          ((*(((UINT8*)(pv_dst)) + 3u)) = (*(((UINT8*)(pv_src)) + 3u)))
#endif  /* (ENDIAN == BIG) */

#if(EPLS_cfg_MAX_PYLD_LEN >= 12)
/**
 * @brief This function swaps the 64 bit data if it is necessary and copies.
 *
 * The data will be copied from the source pointer to the destination pointer. If the host format
 * defined by ENDIAN in the EPLStarget.h is big endian then the data will be swapped.
 * @attention The parameters will not be checked by this function. The function is implemented as a macro due to optimization.
 *
 * @param        pv_dst        destination pointer (not checked), valid range: <> NULL
 *
 * @param        pv_src        source pointer (not checked), valid range: <> NULL
 */
#if(ENDIAN == BIG)
#define SFS_NET_CPY64(pv_dst, pv_src)                        \
      ((*(((UINT8*)(pv_dst)) + 0u)) = (*(((UINT8*)(pv_src)) + 7u))); \
      ((*(((UINT8*)(pv_dst)) + 1u)) = (*(((UINT8*)(pv_src)) + 6u))); \
      ((*(((UINT8*)(pv_dst)) + 2u)) = (*(((UINT8*)(pv_src)) + 5u))); \
      ((*(((UINT8*)(pv_dst)) + 3u)) = (*(((UINT8*)(pv_src)) + 4u))); \
      ((*(((UINT8*)(pv_dst)) + 4u)) = (*(((UINT8*)(pv_src)) + 3u))); \
      ((*(((UINT8*)(pv_dst)) + 5u)) = (*(((UINT8*)(pv_src)) + 2u))); \
      ((*(((UINT8*)(pv_dst)) + 6u)) = (*(((UINT8*)(pv_src)) + 1u))); \
      ((*(((UINT8*)(pv_dst)) + 7u)) = (*(((UINT8*)(pv_src)) + 0u)))
#else  /* (ENDIAN == LITTLE) */
#define SFS_NET_CPY64(pv_dst, pv_src)                        \
      ((*(((UINT8*)(pv_dst)) + 0u)) = (*(((UINT8*)(pv_src)) + 0u))); \
      ((*(((UINT8*)(pv_dst)) + 1u)) = (*(((UINT8*)(pv_src)) + 1u))); \
      ((*(((UINT8*)(pv_dst)) + 2u)) = (*(((UINT8*)(pv_src)) + 2u))); \
      ((*(((UINT8*)(pv_dst)) + 3u)) = (*(((UINT8*)(pv_src)) + 3u))); \
      ((*(((UINT8*)(pv_dst)) + 4u)) = (*(((UINT8*)(pv_src)) + 4u))); \
      ((*(((UINT8*)(pv_dst)) + 5u)) = (*(((UINT8*)(pv_src)) + 5u))); \
      ((*(((UINT8*)(pv_dst)) + 6u)) = (*(((UINT8*)(pv_src)) + 6u))); \
      ((*(((UINT8*)(pv_dst)) + 7u)) = (*(((UINT8*)(pv_src)) + 7u)))
#endif  /* (ENDIAN == BIG) */
#endif  /* (EPLS_cfg_MAX_PYLD_LEN >= 12) */

/**
 * @def SFS_NET_CPY_DOMSTR
 * @brief This function copies data of type EPLS_k_VISIBLE_STRING, EPLS_k_DOMAIN and EPLS_k_OCTET_STRING.
 *
 * Type EPLS_k_VISIBLE_STRING and EPLS_k_OCTET_STRING are byte oriented and thus a swapping not necessary.
 * Type EPLS_k_DOMAIN is not specified within the openSAFETY Stack. For this reason an adaption to the
 * network format of data is not possible.
 *
 * @attention The parameters will not be checked by this function.
 * The function is implemented as a macro due to optimization.
 *
 * @param        pv_dst        destination pointer (not checked), valid range: <> NULL
 *
 * @param        pv_src        source pointer (not checked), valid range: <>NULL
 *
 * @param        dw_len        length of the data to be copied in case of type:
 *                             EPLS_k_VISIBLE_STRING, EPLS_k_DOMAIN, EPLS_k_OCTET_STRING (not checked), valid range: any 32 bit value
 */
#define SFS_NET_CPY_DOMSTR(pv_dst, pv_src, dw_len) \
        MEMCOPY(pv_dst, pv_src, dw_len)

#endif

/** @} */
/** @} */
