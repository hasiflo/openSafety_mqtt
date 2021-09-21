/*
 * \file SHNF_UDPBufferHandling.h
 * Network specific functionality for handling the receive thread and adding to the buffer queue
 * \addtogroup Demo
 * \addtogroup UDPSHNF
 * \{
 * \brief SHNF - Safety "Firmware close to the hardware"
 *
 * SHNF, unit to simulate the interface to firmware driver.
 * For test purposes this unit provides all necessary
 * interfaces between EPLsafety Stack and firmware driver
 * ("Firmware close to the hardware").
 *
 ******************************************************************************
 * Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
 *                      IXXAT Automation GmbH
 * All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the BSD license (according to License.txt).
 *******************************************************************************
 *
 * \author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 */

#ifndef SHNF_UDPBUFFERHANDLING_H_
#define SHNF_UDPBUFFERHANDLING_H_

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>


#include <SHNF.h>
#include "MQTT.h"
#include "SHNF_Definitions.h"


#include "crc.h"
#include "datalogger.h"

#include <pthread.h>
#if linux
#include "CrossCompile_Linux.h"
#else
#include "CrossCompile_Win32.h"
#endif


/* Type definition for an openSAFETY buffer. */
typedef struct s_openSAFETY_BUFFER t_openSAFETY_BUFFER;
/* Type definition for a pointer to an openSAFETY buffer. */
typedef t_openSAFETY_BUFFER * pt_openSAFETY_BUFFER;
/* Struct for storing the buffer of received frames */
struct s_openSAFETY_BUFFER
{
   BOOLEAN           o_allocated;                /* if TRUE then the openSAFETY frame was allocated */
   BOOLEAN           o_marked;                   /* if TRUE then the openSAFETY frame was marked */
   UINT16            w_FrameSize;      /* size of the openSAFETY frame */
   UINT8             ab_FrameBuffer[k_openSAFETY_BUFFER_SIZE]; /* buffer for the openSAFETY frame */
   SHNF_t_TEL_TYPE   e_telType;                  /* telegram type for detecting slim SSDOs */
   struct in_addr    sender;                     /* the IP address of the sender for this package */
   pt_openSAFETY_BUFFER    ps_next;              /* pointer for the next in line */
}; /* one buffer for an EPLS frame */

/* openSAFETY buffers in SHNF */
t_openSAFETY_BUFFER  as_openSAFETYFrames[k_openSAFETY_FRAME_QUEUE_SIZE];

/**
 * This function starts the thread, which will receive data from the network.
 * This function relies heavily on cross-compilation macros. The real code is in
 * \file CrossCompile_Linux.h or \file CrossCompile_Win32.h , so go look there for
 * - \see CREATE_THREAD
 * - \see THREAD_FAILURE
 * - \see THREAD_TYPE
 *
 * \return - TRUE on success
 *         - FALSE on failure
 */
BOOLEAN StartMqttClient();

/**
 * This is the thread runner function for receiving data from the network.
 * The argument is not being parsed
 *
 * \param queueArguments unused
 *
 * \return - TRUE on success
 *         - FALSE on failure
 *
 */
THREAD_FUNC thread_MessageReceiver ( THREAD_FUNC_ARGUMENTS queueArguments );

/**
 * This function writes the received information into the RX buffer from where it
 * can later be processed by the stack.
 *
 * \param c_receivingBuffer the buffer containing the received data
 * \param ui_numberOfBytes the length of the received data
 * \param sender the sender which has sent the data in the containing buffer
 */
void InsertIntoRXBuffer ( const UINT8 * c_receivingBuffer, UINT16 ui_numberOfBytes);

/*
 * \brief This function determines the position of the start of Frame 1, always assuming,
 * frame 1 starts AFTER frame 2.
 *
 * This function determines the position of the start of Frame 1, always assuming,
 * frame 1 starts AFTER frame 2. Please note, that the assumed position is always a
 * guess. To be completely sure, the CRC checksum of frame 1 should be checked,
 * this would normally result in correct detection.
 *
 * However, using EPL as SHNF protocol, SoC packages also get detected sometimes.
 * This just to remind, that this detection might result in a wrongly detected
 * byte stream, and the result has to be checked further down the line.
 *
 * \warning This function ALWAYS assumes, that Frame 2 is stored before Frame 1, and the
 * result is ALWAYS a guess, never a proven fact.
 *
 * \param i_dataLength   the length of data in the given bytestream
 * \param pb_byteStream  the bytefield containing the frame data
 *
 * \return a value indicating the byte position in the stream where frame 1 starts, or -1 in case of an error
 */
UINT16 findFrame1Start ( UINT16 i_dataLength, const UINT8 pb_byteStream[] );

/**
 * \brief This function switches the second openSAFETY frame with the first
 *
 * \warning This function ALWAYS assumes, that Frame 2 is stored before Frame 1
 *
 * \param i_dataLength   the length of data in the given bytestream
 * \param pb_byteStream  the bytefield containing the frame data
 *
 * \return a bytefield of the same length as @b i_dataLength containing the frames in changed order
 */
UINT8 * switchFrames ( UINT16 i_dataLength, const UINT8 pb_byteStream[] );


#endif /* SHNF_UDPBUFFERHANDLING_H_ */

/* \} */
