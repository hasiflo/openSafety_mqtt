/*
 * \file SHNFnetwork.h
 * Network specific functionality
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
 * \author M.Molnar, IXXAT Automation GmbH (original Demo application)
 *
 */

#ifndef linux
#pragma warning (disable : 4115) /* This pragma is used to avoid a compiler
                                    warning (warning C4115: _RPC_ASYNC_STATE
                                    named type definition in parentheses) in
                                    rpcasync.h windows header */
#endif

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "UDPBufferHandling.h"

#include "EPLScfg.h"
#include "EPLStypes.h"

#include <SHNF.h>
#include "UDP.h"
#include "SHNF_Definitions.h"

#include "Time.h"

/* Internal storage for the loopback address (the nodes own address) */
static UINT16 w_LoopBackSadr;

/** RX queue size counter */
static int rxFrameQueueSize = 0;

/* Pointer to the first in queue */
static pt_openSAFETY_BUFFER * ps_firstInQueue;
/* Pointer to the last in queue */
static pt_openSAFETY_BUFFER * ps_lastInQueue;

/* Creating buffer space for transmitting packets */
CREATE_BUFFER(mutexTXBuffer);
/* Creating buffer space for receiving packets */
CREATE_BUFFER(mutexRXBuffer);

/**
 * \brief This function initializes the SHNF interface.
 *
 * \param w_loopBackSadr - loop back source address
 */
void SHNF_Init(BYTE_B_INSTNUM_ UINT16 w_loopBackSadr)
{
    UINT32 i = 0; /* loop counter */

    #if (EPLS_cfg_MAX_INSTANCES > 1)
        b_instNum = b_instNum; // to avoid warnings
    #endif
    w_LoopBackSadr = w_loopBackSadr;

    #ifdef SHNF_LOG_ACTIVATE
        GenerateErrorStr();

        LOG_Init_File(SHNF_LOG_FILE);
    #endif

    StartMessageReceiverThread();

    /** Initialize the transmit buffer */
    LOCK_BUFFER( mutexTXBuffer );
    for(i = 0; i < k_openSAFETY_FRAME_QUEUE_SIZE; i++)
    {
        as_openSAFETYFrames[i].o_allocated = FALSE;
        as_openSAFETYFrames[i].w_FrameSize = 0x00U;
        as_openSAFETYFrames[i].o_marked = FALSE;
    }
    UNLOCK_BUFFER( mutexTXBuffer );

    /** Initialize the receiving buffer, or to be exact the pointer to the list */
    LOCK_BUFFER( mutexRXBuffer );

    ps_firstInQueue = (pt_openSAFETY_BUFFER *)malloc(sizeof(pt_openSAFETY_BUFFER));
    *ps_firstInQueue = (pt_openSAFETY_BUFFER)NULL;

    ps_lastInQueue = (pt_openSAFETY_BUFFER *)malloc(sizeof(pt_openSAFETY_BUFFER));
    *ps_lastInQueue = (pt_openSAFETY_BUFFER)NULL;

    UNLOCK_BUFFER( mutexRXBuffer );
}

/*
 * This function provides a reference to an empty memory block. The memory block is
 * used to store an openSAFETY frame to be transmitted on the field bus. Frame header
 * and frame data are stored within this block. The frame data is filled in the specific
 * unit (SPDO, SNMTM, SNMTS, SSDOC, SSDOS) but the frame header is only build in unit SFS.
 *
 * After write access the memory block has to be marked as "ready to process".
 * To do so the function \see SHNF_MarkTxMemBlock has to be called.
 *
 * \see SHNF_MarkTxMemBlock
 *
 * \warning The first part of the memory block contains the openSAFETY sub-frame TWO
 *          and the second part the openSAFETY sub-frame ONE.
 *
 * \param B_INSTNUM instance number
 * \param w_blockSize size of the requested memory block in bytes
 * \param e_telType telegram type (SPDO / SSDO / SNMT)
 * \param w_txSpdoNum Number of the Tx SPDO (first SPDO number is 0). It is only
 *                 relevant, if telegram type is SPDO.
 *
 * \return - == NULL  - memory allocation failed, no memory available
 *         - <> NULL  - memory allocation succeeded, reference to requested memory
 */
UINT8 * SHNF_GetTxMemBlock(BYTE_B_INSTNUM_ UINT16 w_blockSize,
                           SHNF_t_TEL_TYPE e_telType, UINT16 w_txSpdoNum)
{
  UINT8 *pb_ret = (UINT8 *)NULL; /* return value */
  UINT32 i; /* loop counter */

  #if (EPLS_cfg_MAX_INSTANCES > 1)
    b_instNum = b_instNum; /* to avoid compiler warnings */
  #endif
  /* Avoid compiler warning */
  w_txSpdoNum = w_txSpdoNum;

  /* switch for the message type */
  switch (e_telType)
  {
    case SHNF_k_SSDO:
    case SHNF_k_SSDO_SLIM:
    case SHNF_k_SNMT:
    case SHNF_k_SPDO:
    {

        /* get a free EPLS buffer */
        LOCK_BUFFER( mutexTXBuffer );
        for(i = 0; i < k_openSAFETY_FRAME_QUEUE_SIZE; i++)
        {
            /* if the buffer was not allocated */
            if (!as_openSAFETYFrames[i].o_allocated)
            {
                as_openSAFETYFrames[i].o_allocated = TRUE;
                as_openSAFETYFrames[i].w_FrameSize = w_blockSize;
                as_openSAFETYFrames[i].e_telType = e_telType;
                pb_ret = as_openSAFETYFrames[i].ab_FrameBuffer;
                break;
            }
        }
        UNLOCK_BUFFER( mutexTXBuffer );

      break;
    }
    default:
    {
      /* NULL is returned */
      DATA_LOGGER_DEBUG("e_telType is unknown in the function SHNF_GetTxMemBlock");
      break;
    }
  }

  return pb_ret;

}

/*
 * This function marks a requested memory block as "ready to process". That means all
 * necessary data and frame header info for the openSAFETY frame was written into the
 * memory block by the openSAFETY Stack and the frame can be transmitted on the field bus.
 * To get a reference to an empty memory block the function \see SHNF_GetTxMemBlock()
 * has to be called before.
 *
 * \see SHNF_GetTxMemBlock
 *
 * \warning The first part of the memory block contains the openSAFETY sub-frame TWO
 *          and the second part the openSAFETY sub-frame ONE.
 *
 * \param B_INSTNUM instance number
 * \param pb_memBlock reference to the memory to be marked
 *
 * \return - TRUE  - memory block marked successfully
 *         - FALSE - memory block NOT marked, error returned
 */
BOOLEAN SHNF_MarkTxMemBlock(BYTE_B_INSTNUM_ const UINT8 *pb_memBlock)
{
    BOOLEAN o_ret = FALSE;    /* return value */
    UINT32 i = 0;             /* loop counter */
    UINT32 i_bytesSent = 0;   /* num of bytes received */
    UINT16 i_frame1Start = 0;
    UINT16 i_targetAddress = 0;
    UINT16 i_dataLength = 0;

    #if (EPLS_cfg_MAX_INSTANCES > 1)
        b_instNum = b_instNum; /* to avoid compiler warnings */
    #endif

    /* locking the transmission buffer */
    LOCK_BUFFER( mutexTXBuffer );

    i = 0;
    /* get buffers to be marked */
    do
    {
        /* if the buffer was allocated but not marked */
        if (as_openSAFETYFrames[i].o_allocated && !as_openSAFETYFrames[i].o_marked)
        {
            /* mark the buffer */
            as_openSAFETYFrames[i].o_marked = TRUE;

            i_bytesSent = 0;
            i_dataLength = (as_openSAFETYFrames[i].w_FrameSize);

            /* search for frame 1 start */
            i_frame1Start = findFrame1Start( i_dataLength, pb_memBlock );

            /* First octet & lower 2 bits of second octet contain the target address */
            i_targetAddress = openSAFETY_FRAME_ADDRESS(pb_memBlock, i_frame1Start);

            /* SPDO's are always broadcasted. */
            if ( openSAFETY_IS_SPDO_FRAME_ID(pb_memBlock[i_frame1Start + 1]) )
                i_targetAddress = 0;

            /* Let's send the package on the network */
            i_bytesSent = SendOnNetwork ( i_targetAddress, pb_memBlock, i_dataLength );
            if ( i_bytesSent < 0 ) {
                DATA_LOGGER2 ( "Error sending frame: %d - %s\n", errno, strerror(errno));
                o_ret = FALSE;
            }
            else
                o_ret = TRUE;
            break;
        }

        i++;
    }
    while(i < k_openSAFETY_FRAME_QUEUE_SIZE);
    UNLOCK_BUFFER( mutexTXBuffer );

    /*
     * Cleanup, we clear already sent packages out of the queue
     * This is somewhat of a time consumer, could be cleaned up considerably, but works for now
     * because the only station really sending a lot is the SCM+SN,
     * and here the bottleneck is not that massive
     */
    LOCK_BUFFER( mutexTXBuffer );
    i = 0;
    do
    {
        as_openSAFETYFrames[i].o_allocated = FALSE;
        as_openSAFETYFrames[i].o_marked = FALSE;
        memset ( as_openSAFETYFrames[i].ab_FrameBuffer, 0, as_openSAFETYFrames[i].w_FrameSize );
        as_openSAFETYFrames[i].w_FrameSize = 0;
        i++;
    }
    while ( ( i < k_openSAFETY_FRAME_QUEUE_SIZE ) && ( as_openSAFETYFrames[i].o_allocated && as_openSAFETYFrames[i].o_marked ) );
    UNLOCK_BUFFER( mutexTXBuffer );

    return o_ret;
}

/**
 * This function returns a reference to a received openSAFETY frame.
 *
 * \retval pw_frameLength the number of bytes for the received openSAFETY frame
 * \retval e_telType the @see SHNF_t_TEL_TYPE type of the received openSAFETY frame
 *
 * \return - == NULL - no openSAFETY frame available
 *         - != NULL - pointer to the received openSAFETY frame
 */
UINT8 * SHNF_GetEplsFrame(UINT16 *pw_frameLength, SHNF_t_TEL_TYPE *e_telType)
{
    /* return buffer for the frame, allocation will happen in the switchFrame function */
    UINT8 *pb_ret = (UINT8 *)NULL;
    UINT8  frame2Start = 0;
    UINT16 taddr = 0;
    pt_openSAFETY_BUFFER queue;

    LOCK_BUFFER( mutexRXBuffer );

    /* Find the next free frame to collect, and return if there is none */
    queue = *ps_firstInQueue;
    if ( queue == NULL )
    {
        UNLOCK_BUFFER( mutexRXBuffer );
        return (UINT8 *)NULL;
    }

    /** Let's find a package, which has not been collected yet */
    while ( queue->o_marked == TRUE ) {
        if ( queue->ps_next == NULL )
            break;
        queue = queue->ps_next;
    }

    /** If no element has been found, or ALL elements have been marked, return
     * queue if not null points to the last element in the queue here */
    if ( queue == NULL || queue->o_marked == TRUE )
    {
        UNLOCK_BUFFER( mutexRXBuffer );
        return (UINT8 *)NULL;
    }

    /* If the element has 0 size, or the buffer is null, return. In such a case,
     * something went wrong in inserIntoRXBuffer, and the error will occur somewhere
     * else. This is just a precaution here, it will never be used */
    if ( queue->w_FrameSize == 0 || queue->ab_FrameBuffer == NULL )
    {
        UNLOCK_BUFFER( mutexRXBuffer );
        return (UINT8 *)NULL;
    }

    /** Setting the element as marked, indicating it was collected */
    queue->o_marked = TRUE;

    /** For security reasons, we check if the received frame could be invalid */
    if ( queue->w_FrameSize > k_MAX_RECEIVING_BUFFER )
    {
        UNLOCK_BUFFER(mutexRXBuffer);
        return (UINT8 *)NULL;
    }

    /** We could unlock here, but then ReleaseEPLSFrame might be called, before
     *  we've finished copying the frame. To be on the safe side, we copy first,
     *  and then unlock */

    /** Copy the data into the return buffer */
    *pw_frameLength = queue->w_FrameSize;
    *e_telType = queue->e_telType;
    pb_ret = (UINT8 *) switchFrames(queue->w_FrameSize, queue->ab_FrameBuffer);

    /** If we have an SNMT frame, we can determine sender->ipaddr relation */
    if ( openSAFETY_IS_SNMT_FRAME_ID(pb_ret[1]) )
    {
        frame2Start = (pb_ret[2] + (pb_ret[2] > 8 ? 1 : 0) + k_openSAFETY_MIN_FRAME_1_LENGTH);
        taddr = openSAFETY_FRAME_ADDRESS(pb_ret, frame2Start + 3);
        /** the taddr  */
        if ( taddr > 0 && taddr < 1024 )
            StoreIPForSN ( queue->sender, taddr );
    }

    /* We don't have to take care of max queue size, this will be done in insertIntoRXBuffer */
    UNLOCK_BUFFER( mutexRXBuffer );

    return pb_ret;
}

/**
 * This function releases an EPLS frame which is no longer needed.
 *
 * \see SHNF_GetEplsFrame
 */
void SHNF_ReleaseEplsFrame(void)
{
    pt_openSAFETY_BUFFER queue;

    LOCK_BUFFER( mutexRXBuffer );
    /** Get the first queue element */
    queue = *ps_firstInQueue;

    DATA_LOGGER_DEBUG3 ( "Release :: q:[%p] f:[%p] l:[%p] \n", queue, *ps_firstInQueue, *ps_lastInQueue );

    /** No frame has been collected, we have been called by mistake, return */
    if ( ( queue == NULL ) || ( queue->o_marked != TRUE ) )
    {
        DATA_LOGGER_DEBUG("Unexpected SHNF_ReleaseEplsFrame() call, all EPLS frame are already released\n");
        UNLOCK_BUFFER( mutexRXBuffer );
        return;
    }

    /** We set the queue pointer to the next element, and reset the lastInQueue pointer if necessary */
    *ps_firstInQueue = queue->ps_next;
    if ( queue->ps_next == NULL )
        *ps_lastInQueue = queue->ps_next;

    /** One element has been taken */
    if ( rxFrameQueueSize > 0 )
        rxFrameQueueSize--;

    /** Set pointer to NULL, after this, the element space is free to be destroyed. This is just a precaution */
    queue = (t_openSAFETY_BUFFER *)NULL;
    if ( rxFrameQueueSize <= 0 )
        *ps_firstInQueue = (t_openSAFETY_BUFFER *)NULL;

    free ( queue );

    /** We can unlock here, because all pointers point to the correct queue element, the old one is gone */
    UNLOCK_BUFFER( mutexRXBuffer );
}


/**
 * @var SHNF_aaulConnValidBit
 * @brief This variable contains the connection valid bit of the SPDOs.
 *
 * It is a bit field which contains one bit each RxSPDO. It is to be hosted by the SHNF.
 */
UINT32 SHNF_aaulConnValidBit[EPLS_cfg_MAX_INSTANCES][(SPDO_cfg_MAX_NO_RX_SPDO + 31) / 32];

/**
 * This function returns a CRC checksum over a sub frame with payload data of
 * of maximum 8 bytes. The result is a 8 bit CRC.
 *
 * \param b_initCrc initial CRC (always called with 0 by the EPLS software)
 * \param l_subFrameLength subframe length in bytes
 * \param pv_subFrame reference to subframe
 *
 * \return 0 .. 255 - 8 bit CRC check sum
 */
UINT8 HNFiff_Crc8CalcSwp(UINT8 b_initCrc, INT32 l_subFrameLength,
                         const void *pv_subFrame)
{
    return crc8Checksum(l_subFrameLength, (UINT8*)pv_subFrame, b_initCrc);
}


/**
 * This function returns a CRC checksum over a sub frame with payload data of
 * minimum 9 bytes and maximum 254 bytes. The result is a 16 bit CRC.
 *
 * \param w_initCrc initial CRC (always called with 0 by the EPLS software)
 * \param l_subFrameLength subframe length in bytes
 * \param pv_subFrame reference to subframe
 *
 * \return 0 .. 65535 - 16 bit CRC check sum
 */
UINT16 HNFiff_Crc16CalcSwp(UINT16 w_initCrc, INT32 l_subFrameLength,
                           const void *pv_subFrame)
{
    return crc16Checksum(l_subFrameLength, (UINT8*)pv_subFrame, w_initCrc);
}

/**
 * This function returns a CRC checksum over a sub frame with payload data of
 * minimum 9 bytes and maximum 254 bytes. The result is a 16 bit CRC.
 *
 * \param w_initCrc initial CRC (always called with 0 by the EPLS software)
 * \param l_subFrameLength subframe length in bytes
 * \param pv_subFrame reference to subframe
 *
 * \return 0 .. 65535 - 16 bit CRC check sum
 */
UINT16 HNFiff_Crc16_755B_CalcSwp(UINT16 w_initCrc, INT32 l_subFrameLength,
                                 const void *pv_subFrame)
{
    return crc16Checksum(l_subFrameLength, (UINT8*)pv_subFrame, w_initCrc);
}



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
BOOLEAN StartMessageReceiverThread()
{
    THREAD_TYPE thread;
    THREAD_RESULT_TYPE i_return = 0;

    /* Creating the thread and starting it */
    CREATE_THREAD(thread, thread_MessageReceiver, i_return);

    /* Check for failure */
    if ( THREAD_FAILURE(i_return) )
    {
        DATA_LOGGER1 ( "Return %d\n", i_return );
        DATA_LOGGER ( "Message Receiver could not be started!\n" );
        i_return = (THREAD_RESULT_TYPE) -1;
    }
    else
        DATA_LOGGER_DEBUG ( "Message Receiver started successfully!\n" );

    return THREAD_FAILURE(i_return);
}

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
THREAD_FUNC thread_MessageReceiver ( THREAD_FUNC_ARGUMENTS queueArguments )
{

    UINT8 * c_recvBuffer;    /* receiving buffer */
    struct sockaddr_in servaddr;
    int i_numBytesReceived = 0;
    int i_failureCounter = 0;

    /* Prevent compiler warning with msvc */
    queueArguments = queueArguments;

    /** Allocating memory */
    c_recvBuffer = (UINT8*) malloc ( sizeof ( UINT8 ) * ( k_MAX_RECEIVING_BUFFER + 1 ) );

    /** Starting the receiver loop */
    for ( ; ; )
    {
        /* The receiveBuffer is cleared, to avoid any data contamination */
        memset ( c_recvBuffer, 0, sizeof ( UINT8 ) * ( k_MAX_RECEIVING_BUFFER + 1 ) );

        if ( ( i_numBytesReceived = ReceiveFromNetwork ( c_recvBuffer, k_MAX_RECEIVING_BUFFER,
            (struct sockaddr *) &servaddr ) ) == NETWORK_FAILURE )
        {
            DATA_LOGGER1 ( "Error receiving from socket [%d] for message receiver!\n", errno);
            /*
             * We do not break immediatly, as it could take a while after system start to
             * be able to receive on the socket. e.g. Linux-> first send before able to receive
             */
            i_failureCounter++;
            if ( i_failureCounter > k_openSAFETY_MAX_RX_FAILURE_COUNTER )
                break;
        }
        DATA_LOGGER_DEBUG1("Package received from %s\n",inet_ntoa(servaddr.sin_addr));

        /* The received data is written into the buffer */
        if ( i_numBytesReceived > 0 )
        {
            /** We decrease the failure counter for every packet received. This way, fluctuations
             *  on the net won't interfere with the overall operation, and the failure counter,
             *  won't build up over time, leading to a hanging system after some time running
             */
            if ( i_failureCounter > 0 )
                i_failureCounter--;

            /** Insert packages into queue */
            InsertIntoRXBuffer ( c_recvBuffer, (UINT16) i_numBytesReceived, servaddr.sin_addr );
        }
    }

    free ( c_recvBuffer );
    return (THREAD_FUNC) 0;
}

/**
 * This function writes the received information into the RX buffer from where it
 * can later be processed by the stack.
 *
 * \param c_receivingBuffer the buffer containing the received data
 * \param ui_numberOfBytes the length of the received data
 * \param sender the sender which has sent the data in the containing buffer
 */
void InsertIntoRXBuffer ( const UINT8 * c_receivingBuffer, UINT16 ui_numberOfBytes, struct in_addr sender )
{
    pt_openSAFETY_BUFFER queue;    /* Queue worker variable */
    UINT16 b_ADDR, frame1Pos;
    SHNF_t_TEL_TYPE e_telType;

    LOCK_BUFFER( mutexRXBuffer );

    /* Get the first element of the queue */
    queue  = ( *ps_firstInQueue != NULL ? *ps_firstInQueue : (pt_openSAFETY_BUFFER) NULL );

    /** Find position of the first frame */
    frame1Pos = findFrame1Start ( ui_numberOfBytes, c_receivingBuffer );
    /** Frame 1 Position can never be negative (UINT16), but we check anyway to be on the safe side */
    if ( frame1Pos <= 0 )
    {
        /* No frame position has been found, returning */
        DATA_LOGGER_DEBUG ( "Discarding frame, as no position for frame 1 has been found!\n" );

        UNLOCK_BUFFER ( mutexRXBuffer );
        return;
    }

    /** Address field */
    b_ADDR = openSAFETY_FRAME_ADDRESS (c_receivingBuffer, frame1Pos );

    /* Initializing the telType for windows cl compiler only */
    e_telType = SHNF_k_SNMT;
    /* Determine telegram type */
    if ( openSAFETY_IS_SLIM_SSDO_FRAME_ID(c_receivingBuffer[ ( frame1Pos + 1 ) ]) )
        e_telType = SHNF_k_SSDO_SLIM;
    else if ( openSAFETY_IS_SSDO_FRAME_ID(c_receivingBuffer[ ( frame1Pos + 1 ) ]) )
        e_telType = SHNF_k_SSDO;
    else if ( openSAFETY_IS_SPDO_FRAME_ID(c_receivingBuffer[ ( frame1Pos + 1 ) ]) )
        e_telType = SHNF_k_SPDO;
    else if ( openSAFETY_IS_SNMT_FRAME_ID(c_receivingBuffer[ ( frame1Pos + 1 ) ]) )
        e_telType = SHNF_k_SNMT;
    else
    {
        /* Telegram Type can not be resolved, invalid package, break */
        UNLOCK_BUFFER( mutexRXBuffer );
        return;
    }

    /* If we are the sender of the SPDO package, discard the package,
     * can't be moved before above if/else because, otherwise get's
     * wrongly used for ssdo and ssdo slim */
    if ( ( e_telType != SHNF_k_SPDO ) && ( b_ADDR != w_LoopBackSadr ) )
    {
        UNLOCK_BUFFER( mutexRXBuffer );
        return;
    }

    /* if queue is still empty => create new queue, otherwise go to last element */
    if ( queue == NULL )
    {
        queue = (pt_openSAFETY_BUFFER) malloc ( sizeof ( t_openSAFETY_BUFFER ) );
        *ps_firstInQueue = queue;
        *ps_lastInQueue = queue;
        queue->ps_next = ( pt_openSAFETY_BUFFER ) NULL;
    }
    else
    {
        queue = *ps_lastInQueue;
    }

    /* Something went wrong, return */
    if ( queue == NULL )
    {
        DATA_LOGGER_DEBUG ( "Error inserting into RX buffer!\n" );
        UNLOCK_BUFFER( mutexRXBuffer );
        return;
    }

    /** Apparently we have reached the end of the queue, and the current element
     * has been collected, but not released, we cannot override, so print error and return. */
    if ( queue->o_marked == TRUE )
    {
        DATA_LOGGER_DEBUG ( "Error inserting into RX buffer, last element is already allocated!\n" );
        UNLOCK_BUFFER( mutexRXBuffer );
        return;
    }

    /** Filling the queue element */
    queue->o_allocated = FALSE;
    queue->o_marked = FALSE;
    queue->w_FrameSize = ui_numberOfBytes;
    queue->sender = sender;
    queue->e_telType = e_telType;

    /** Copying receive buffer into queue, afterwards it can be freed */
    memcpy(queue->ab_FrameBuffer, c_receivingBuffer, ui_numberOfBytes);

    /** We create the next element, which is always empty, except when we have reached the max number
     *  of elements, in which case the next element will be NULL, and we don't move the last pointer
     */
    if ( rxFrameQueueSize < k_openSAFETY_FRAME_QUEUE_SIZE )
    {
        queue->ps_next = (pt_openSAFETY_BUFFER)malloc(sizeof(t_openSAFETY_BUFFER));
        queue->ps_next->o_marked = FALSE; /* to be on the safe side (see ReleaseEplsFrame) */

        *ps_lastInQueue = queue->ps_next;
        rxFrameQueueSize++;
    } else {
        queue->ps_next = (pt_openSAFETY_BUFFER)NULL;
    }

    UNLOCK_BUFFER( mutexRXBuffer );
}

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
UINT16 findFrame1Start ( UINT16 i_dataLength, const UINT8 pb_byteStream[] )
{
    UINT16 i_wFrame1Position, i_payloadLength, i_calculatedLength, i_offset = 0;
    UINT8 b_tempByte = 0;

    /*
     * First, a normal package is assumed. Calculation of frame 1 position is
     * pretty easy, because the length of the whole package is 11 + 2*n + 2*o, which
     * results in frame 1 start at (6 + n + o), which is length / 2 + 1
     */
    i_wFrame1Position = i_dataLength / 2 + 1;

    /* Two bytes from starting position will be the payload length */
    i_payloadLength = pb_byteStream [ i_wFrame1Position + 2 ];

    /* Calculating the assumed frame length, taking CRC8/CRC16 into account */
    i_calculatedLength = i_payloadLength * 2 + k_openSAFETY_MIN_FRAME_LENGTH + 2 * (i_payloadLength > 8 ? 1 : 0);

    /* If the calculated length differs from the given length, this might be a slim package */
    if ( i_calculatedLength != i_dataLength )
    {
        /*
         * This is a possible slim package. Resetting the position for frame 1 to zero. If
         * this is not a slim package, frame 1 position zero indicates that the search for
         * the frame position was not successful
         */
        i_wFrame1Position = 0;

        /*
         * Slim packages have a fixed sublength of either 6 bytes for frame 2 in
         * case of crc8 and 7 bytes in case of crc16. In case of CRC8, the maximum
         * length of frame 2 is therefore 19 (5 + 6 + 8).
         */
        i_offset = k_openSAFETY_MIN_FRAME_2_LENGTH + ( i_dataLength < 20 ? 0 : 1 );

        /*
         *  Last 2 digits belong to addr, therefore have to be cleared, the 3rd last digit indicates request or
         * response. To ease detection, we clear it as well
         */
        b_tempByte = ( pb_byteStream [ i_offset + 1 ] >> 3 ) << 3;

        /* If the tempByte is a correct slim ssdo frame id, we have a slim package */
        if ( openSAFETY_IS_SLIM_SSDO_FRAME_ID( b_tempByte ) )
        {
            /* Slim package found at offset i_offset */
            i_wFrame1Position = i_offset;
        }
    }

#if DATA_LOGGER_PRINT_DEBUG
    /** Catching the error for debuging purposes */
    if ( i_wFrame1Position >= i_dataLength )
    {
        DATA_LOGGER_DEBUG3("findFrame1Start returned an unusual value: %d\nBytes: %s\n",
                i_wFrame1Position, format_bytes( (UINT8 *)pb_byteStream, i_dataLength ) );
    }
#endif

    /* If either the found position is 0 or bigger than the given length, 0 get's returned */
    if ( i_wFrame1Position == 0 || i_wFrame1Position >= i_dataLength )
        return (UINT16) 0;

    return i_wFrame1Position;
}

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
UINT8 * switchFrames ( UINT16 i_dataLength, const UINT8 pb_byteStream[] )
{
    UINT8 * pb_sendMemBlock;
    UINT16 i_wFrame1Position = 0;

    /** we are going two switch both frames now, because on udp, frame 1 get's sent first */
    pb_sendMemBlock = (UINT8*) malloc ( sizeof(UINT8) * i_dataLength );

    /** We search for start of frame 1 */
    i_wFrame1Position = findFrame1Start ( i_dataLength, pb_byteStream );

    /** if the frame position is zero (< just to be on safe side, as impossible (UINT)), or
     *  the frame position is greater than the datalength provided (indicating an error in
     *  the detection algorithm), we return the provided bytestream as-is. */
    if ( i_wFrame1Position <= 0 || i_wFrame1Position >= i_dataLength )
    {
        /** No valid frame position has been found, the frame will be returned as-is */
        memcpy ( pb_sendMemBlock, pb_byteStream, i_dataLength);
    }
    else
    {
        /** a valid frame position has been found, the frames will be switched */
        memcpy ( pb_sendMemBlock, (pb_byteStream + i_wFrame1Position ), ( i_dataLength - i_wFrame1Position ) );
        memcpy ( (pb_sendMemBlock + ( i_dataLength - i_wFrame1Position ) ), pb_byteStream, i_wFrame1Position );
    }

    return pb_sendMemBlock;
}

/**
 * \brief Closes the connection and cleans up the socket
 */
void SHNF_CleanUp()
{
    CleanUpSocket();
}

/* \} */
