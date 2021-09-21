/**
 * @addtogroup SN
 * @{
 * @addtogroup SHNF [SHNF] Safety hardware near firmware
 * @{
 *
 * The SHNF module provides the HAL interface between the openSAFETY stack and the underlying fieldbus implementation.
 *
 * This unit provides functionality for memory management and CRC calculation.
 * The unit provides the interface between openSAFETY Stack and firmware driver
 * ("Firmware close to the hardware").
 *
 * @file SHNF.h
 *
 * This unit is used to simulate the interface to firmware driver.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 *
 * <h2>History for SHNF.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>10.03.2011</td><td>Hans Pill</td><td>changed CRC16 calculation routine for openSafety stack<br />no code change within the stack required</td></tr>
 *     <tr><td>15.03.2011</td><td>Hans Pill</td><td>changes for new CRC polynomial A&P254590</td></tr>
 *     <tr><td>16.03.2011</td><td>Hans Pill</td><td>Review SL V21</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>added optional bit field for SPDOs with 40 bit CT</td></tr>
 * </table>
 */

#ifndef SHNF_H
#define SHNF_H

/**
 * @enum SHNF_t_TEL_TYPE
 * @brief Enumeration of telegram types.
 *
 * Depending on the telegram type the SHNF allocates synchron or asynchon buffers in the memory.
 */
typedef enum
{
    /** telegram type for process data objects */
    SHNF_k_SPDO = 1,
    /** telegram type service data objects */
    SHNF_k_SSDO = 2,
    /** telegram type service data objects fast */
    SHNF_k_SSDO_SLIM = 3,
    /** telegram type network management telegrams */
    SHNF_k_SNMT = 4
} SHNF_t_TEL_TYPE;

/*----------------------------------------------------------------------------*/
/* const defines                                                              */
/*----------------------------------------------------------------------------*/
#define SLIM_FRAME_MAX_CRC8_LEN           19

/* Position of subframe 1 in slim frame */
#define SLIM_FRAME_SUB1_POS_CRC8           6
#define SLIM_FRAME_SUB1_POS_CRC16          7

/**
 * @var SHNF_aaulConnValidBit
 * @brief This variable contains the connection valid bit of the SPDOs.
 *
 * It is a bit field which contains one bit each RxSPDO. It is to be hosted by the SHNF.
 */
#if (SPDO_cfg_CONNECTION_VALID_BIT_FIELD == EPLS_k_ENABLE)
  extern UINT32 SHNF_aaulConnValidBit[EPLS_cfg_MAX_INSTANCES]                 \
                                     [(SPDO_cfg_MAX_NO_RX_SPDO + 31) / 32];
#endif

/**
 * @var SHNF_aaulExtCtBit
 * @brief This variable contains the extended CT bit of the SPDOs.
 *
 * It is a bit field which contains one bit each RxSPDO. It is to be hosted by the SHNF.
 */
#if (SPDO_cfg_EXTENDED_CT_BIT_FIELD == EPLS_k_ENABLE)
  extern UINT32 SHNF_aaulExtCtBit[EPLS_cfg_MAX_INSTANCES]                     \
                                     [(SPDO_cfg_MAX_NO_RX_SPDO + 31) / 32];
#endif

/**
 * @var SHNF_aaulConnValidStatistic
 * @brief This variable is a statistic counter.
 *
 * This counter is incremented in case a connection turns from valid to invalid.
 * It is a bit field which contains one bit each RxSPDO. It is to be hosted by the SHNF.
 */
#if (SPDO_cfg_CONNECTION_VALID_BIT_FIELD == EPLS_k_ENABLE)
#if (SPDO_cfg_CONNECTION_VALID_STATISTIC == EPLS_k_ENABLE)
  extern UINT32 SHNF_aaulConnValidStatistic[EPLS_cfg_MAX_INSTANCES]           \
                                           [SPDO_cfg_MAX_NO_RX_SPDO];
#endif
#endif


#if (SPDO_cfg_PROP_DELAY_STATISTIC == EPLS_k_ENABLE)
  /**
  * @var SHNF_aulPropDelayStatistic
  * @brief This variable shows the propagation delay
  *
  * This variable delivers an average over the last cycle and actual cycle propagation delay.
  */
  extern UINT32 SHNF_adwPropDelayStatistic[SPDO_cfg_MAX_NO_RX_SPDO];
#endif



/**
 * @brief This function provides a reference to an empty memory block.
 *
 * The memory block is used to store an openSAFETY frame to be transmitted on the field bus.
 * Frame header and frame data are stored within this block. The frame data is filled in the
 * specific unit (SPDO, SNMTM, SNMTS, SSDOC, SSDOS) but the frame header is only build in unit
 * SFS. After write access the memory block has to be marked as "ready to process". To do so
 * the function SHNF_MarkTxMemBlock() has to be called.
 *
 * @see          SHNF_MarkTxMemBlock()
 *
 * @param        b_instNum           instance number
 *
 * @param        w_blockSize         size of the requested memory block in bytes
 *
 * @param        e_telType           telegram type (SPDO / SSDO / SNMT)
 *
 * @param        w_txSpdoNum         Number of the Tx SPDO (first SPDO number is 0). It is only relevant, if telegram type is SPDO.
 *
 * @return
 *  - == NULL             memory allocation failed, no memory available
 *  - <> NULL           - memory allocation succeeded, reference to requested memory
 */
extern UINT8 * SHNF_GetTxMemBlock(BYTE_B_INSTNUM_ UINT16 w_blockSize,
        SHNF_t_TEL_TYPE e_telType,
        UINT16 w_txSpdoNum);


/**
 * @brief This function marks a requested memory block as "ready to process".
 *
 * That means all necessary data and frame header info for the openSAFETY frame was written into the
 * memory block by the openSAFETY Stack and the frame can be transmitted on the field bus.
 * To get a reference to an empty memory block the function SHNF_GetTxMemBlock() has to be called before.
 *
 * @see          SHNF_GetTxMemBlock()
 *
 * @param        b_instNum          instance number
 *
 * @param        pb_memBlock        reference to the memory to be marked *Attention* The first part of the
 *                     memory block contains the openSAFETY sub-frame TWO and the second part the openSAFETY sub-frame ONE.
 *
 * @return
 *  - TRUE             - memory block marked successfully
 *  - FALSE            - memory block NOT marked, error returned
 */
extern BOOLEAN SHNF_MarkTxMemBlock(BYTE_B_INSTNUM_ const UINT8 *pb_memBlock);

/**
 * @brief This function returns a CRC checksum over a sub frame with payload data of maximum 8 bytes. The result is a 8 bit CRC.
 *
 * @param        b_initCrc               initial CRC (always called with 0 by the openSAFETY software)
 *
 * @param        l_subFrameLength        subframe length in bytes
 *
 * @param        pv_subFrame             reference to subframe
 *
 * @return       0 .. 255                8 bit CRC check sum
 */
#define SHNF_Crc8Calc(a,b,c)	HNFiff_Crc8CalcSwp(a,b,c)
extern UINT8 SHNF_Crc8Calc(UINT8 b_initCrc, INT32 l_subFrameLength,
        const void *pv_subFrame);

/**
 * @brief This function returns a CRC checksum over a sub frame with payload data of minimum 9 bytes and maximum 254 bytes.
 * The result is a 16 bit CRC.
 *
 * @param        w_initCrc               initial CRC (always called with 0 by the openSAFETY software)
 *
 * @param        l_subFrameLength        subframe length in bytes
 *
 * @param        pv_subFrame             reference to subframe
 *
 * @return       0 .. 65535            - 16 bit CRC check sum
 */
#define SHNF_Crc16Calc(a,b,c)	HNFiff_Crc16_755B_CalcSwp(a,b,c)
extern UINT16 SHNF_Crc16Calc(UINT16 w_initCrc, INT32 l_subFrameLength,
        const void *pv_subFrame);

/**
 * @brief This function returns a CRC checksum over a sub frame with payload data of minimum 9 bytes and maximum 254 bytes.
 * The result is a 16 bit CRC.
 *
 * @param        w_initCrc               initial CRC (always called with 0 by the openSAFETY software)
 *
 * @param        l_subFrameLength        subframe length in bytes
 *
 * @param        pv_subFrame             reference to subframe
 *
 * @return       0 .. 65535            - 16 bit CRC check sum
 */
#define SHNF_Crc16CalcSlim(a,b,c)	HNFiff_Crc16CalcSwp(a,b,c)
extern UINT16 SHNF_Crc16CalcSlim(UINT16 w_initCrc, INT32 l_subFrameLength,
        const void *pv_subFrame);

#endif

/** @} */
/** @} */
