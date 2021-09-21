/*
 * \file SHNF_Definitions.h
 * Basic defintions used by all SHNF demo implementations.
 * \addtogroup Demo
 * \{
 *
 * SHNF, unit to simulate the interface to firmware driver.
 * For test purposes this unit provides all necessary
 * interfaces between EPLsafety Stack and firmware driver
 * ("Firmware close to the hardware").
 *
 * @author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 *****************************************************************************
 * Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
 *                      IXXAT Automation GmbH
 * All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the BSD license (according to License.txt).
 *****************************************************************************/

#ifndef SHNF_DEFINITIONS_H_
#define SHNF_DEFINITIONS_H_

/* Minimum length for frame 1 */
#define k_openSAFETY_MIN_FRAME_1_LENGTH 5

/* Minimum length for frame 2 */
#define k_openSAFETY_MIN_FRAME_2_LENGTH 6

/*
 * The shortest possible message is a message without payload, making
 * it 5 bytes for the first frame, and 6 bytes for the second frame
 */
#define k_openSAFETY_MIN_FRAME_LENGTH ( k_openSAFETY_MIN_FRAME_1_LENGTH + k_openSAFETY_MIN_FRAME_2_LENGTH )

/*
 * The length field of a message is a 8-bit integer, therefore the maximum
 * length of a safety payload is 255 bytes.
 */
#define k_openSAFETY_MAX_PAYLOAD_LENGTH ( ( 2 ^ 8 ) - 1)

/*
 * The maximum length of frame 1 and frame 2 combined is 2 * payload + min
 * frame length which results in 521 bytes. Using 10 bytes tolerance.
 */
#define k_openSAFETY_MAX_FRAME_LENGTH ( 2 * k_openSAFETY_MAX_PAYLOAD_LENGTH + k_openSAFETY_MIN_FRAME_LENGTH  + 10 )

/*
 * Maximum size of a safety frame can be 521 bytes, so that should be sufficient.
 *
 * Technically an Ethernet Frame can have more than 525 bytes. But the length of an
 * openSAFETY payload is defined by a 8-bit integer field, therefore making the
 * maximum length of one safety payload 255 bytes. Taking into consideration, that
 * the total length of the combined frames 1 & 2 is 2 * payload + 11
 *
 * Used in \see SHNF_GetEplsFrame and \see thread_MessageReceiver
 */
#define k_MAX_RECEIVING_BUFFER  ( k_openSAFETY_MAX_FRAME_LENGTH )

/* size of the EPLS frame buffer */
#define k_openSAFETY_BUFFER_SIZE k_MAX_RECEIVING_BUFFER

/* Size of the EPLS frame queue.
   NOTE : The maximum queue size need in TST_UNT_PreCon_WB() */
#define k_openSAFETY_FRAME_QUEUE_SIZE SCM_cfg_MAX_NUM_OF_NODES*2

/* Maximum number of receive failures */
#define k_openSAFETY_MAX_RX_FAILURE_COUNTER   15

/*
 * An address is stored in 10 bits. This macro retrieves the address given a
 * starting address in a byte field
 */
#define openSAFETY_FRAME_ADDRESS( bytefield, startposition )  \
    (bytefield[startposition] + ((UINT8)(bytefield[startposition + 1] << 6) << 2))


/* Mask for a SNMT */
#define openSAFETY_SNMT_FRAME_MASK        0xA0
/* Mask for a SPDO */
#define openSAFETY_SPDO_FRAME_MASK        0xC0
/* Mask for a SSDO */
#define openSAFETY_SSDO_FRAME_MASK        0xE0
/* Mask for a Slim SSDO */
#define openSAFETY_SLIM_SSDO_FRAME_MASK   0xE8

/* Determine if the given ID is a SNMT frame ID */
#define openSAFETY_IS_SNMT_FRAME_ID(bID)       ( ( bID >> 5 ) == ( openSAFETY_SNMT_FRAME_MASK >> 5 ) )
/* Determine if the given ID is a SPDO frame ID */
#define openSAFETY_IS_SPDO_FRAME_ID(bID)       ( ( bID >> 5 ) == ( openSAFETY_SPDO_FRAME_MASK >> 5 ) )
/* Determine if the given ID is a SSDO frame ID */
#define openSAFETY_IS_SSDO_FRAME_ID(bID)       ( ( bID >> 5 ) == ( openSAFETY_SSDO_FRAME_MASK >> 5 ) )
/* Determine if the given ID is a Slim SSDO frame ID */
#define openSAFETY_IS_SLIM_SSDO_FRAME_ID(bID)  ( ( bID >> 5 ) == ( openSAFETY_SLIM_SSDO_FRAME_MASK >> 5 ) )



/**
 * @brief TIM_t_TIME_BASE - Enumeration for the time base according to the SOD object with
 *    0x1200, 0x03.
 */
typedef enum
{
    k_openSAFETY_TIMEBASE_1US   = 0, /*   1 micro second  */
    k_openSAFETY_TIMEBASE_10US  = 1, /*  10 micro seconds */
    k_openSAFETY_TIMEBASE_100US = 2, /* 100 micro seconds */
    k_openSAFETY_TIMEBASE_1MS   = 3  /*   1 mili  second  */
}openSAFETY_TIMEBASE;


#endif /* SHNF_DEFINITIONS_H_ */

/** @} */
