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
#ifndef SHNFNETHELPER_H
#define SHNFNETHELPER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#if WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#else
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#endif

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"

#include "UDP_Configuration.h"

#ifndef NETWORK_FAILURE
#define NETWORK_FAILURE   ((int)-1)
#endif

#ifndef k_UDP_TARGET_NETWORK
  #error The compiler define 'k_UDP_TARGET_NETWORK' has to be set in the target main header file
#endif

/**
 * This function returns the address on the network as well as the name of the adapter used
 *
 * \param  i_bufLength - the maximum length of the character buffer
 * \retval adapterDescription - the name for the interface used
 * \retval localIPAddress - the in_addr structure for the address
 * \retval broadcast - the in_addr structure for the broadcast address
 */
void AdapterOnNetwork ( UINT16 i_bufLength, char ** adapterDescription, struct in_addr * localIPAddress, struct in_addr * broadcast );

/**
 * This function returns the address on the network, we are going to send to for the
 * given target. If the given target is 0xFFFF, we are returning the local adapter address
 *
 * \param i_targetAddress - the address for the intended target, or 0xFFFF for the local adapter
 *                          interface
 * \retval localIPAddress - the in_addr structure for the address
 */
void TargetOnNetwork ( UINT16 i_targetAddress, struct in_addr * localIPAddress );

/**
 * This functions saves the assignment of the given snAddress to the safety sender address,
 * therefore allowing to send to specific targets. If the given ip address is already stored
 * for another target, the senderAddress get's reassigned
 */
void StoreIPForSN ( struct in_addr snAddress, UINT16 senderAddress );

/**
 * This function returns the broadcast address on the network, we are going to send to for the
 * given target. Under linux it always returns the network address instead, as otherwise, the
 * broadcast packages would not be received
 *
 * \retval localIPAddress - the in_addr structure for the broadcast address
 */
void BroadcastNetwork ( struct in_addr * localIPAddress );

#endif /* #ifndef  SHNFNETHELPER_H */

/** \} */
