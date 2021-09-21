/**
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
 * \author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 */

#ifndef SHNFNETWORK_H
#define SHNFNETWORK_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#include <tchar.h>
#endif

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#include "UDP_Configuration.h"

#include <SHNF.h>

#include "UDP.h"

#if linux
#include "CrossCompile_Linux.h"
#else
#include "CrossCompile_Win32.h"
#endif


/* various network functions define a failure state by returning -1 */
#define NETWORK_FAILURE ((UINT8)-1)

/**
 * \brief This function receives a frame from the network
 * \param i_ReceiveBufferLength IN length of the buffer
 * \retval c_ReceiveBuffer OUT pointer to the temporary buffer, where the received frame will be stored
 * \retval connectionData OUT data containing information about the frame sender, like IP Address, source port, ...
 * \return
 *   - success - the number of bytes received
 *   - failure - -1
 */
UINT32 ReceiveFromNetwork ( UINT8 * c_ReceiveBuffer,
                            UINT32 i_ReceiveBufferLength,
                            struct sockaddr * connectionData );

/**
 * \brief This function sends a single frame from the sendBuffer in a single UDP frame on the network
 *
 * \param c_sendBuffer IN pointer to the temporary buffer, containing the information to be send
 * \param i_sendBufferLength IN the length of the data in the temporary buffer
 * \param i_targetAddress IN the target SN address, or 0 for broadcast
 *
 * \return an integer containing either
 *    - the number of bytes received or
 *    - -1 if an error occured
 */
UINT32 SendOnNetwork ( UINT16 i_targetAddress,
                       const UINT8 * c_sendBuffer,
                       UINT32 i_sendBufferLength );

/**
 * \brief Closes the connection and cleans up the socket
 */
void CleanUpSocket();

static UINT32 i_socketSender = 0;
static UINT32 i_socketReceiver = 0;

#ifdef SHNF_DBG_PRINT_ACTIVE
char * format_bytes(UINT8 *pBuffer, UINT32 length);
#endif

#endif /* #ifndef  SHNFNETWORK_H */

/** \} */
