/**
 * \file UDP_SN.h
 * \addtogroup Demo
 * \{
 * \defgroup UDP_SN UDP based SN
 * Implementation for a SN using an UDP based SHNF
 * \{
 * \defgroup SAPL_UDP_SN UDP Demo Safety Application
 * Implementation of the basic safety application for the UDP demo
 *
 * \defgroup SHNF_UDP_SN UDP Demo Safety Hardware Communication Layer
 * Implementation of the HAL between the openSAFETY framework and the UDP network
 *
 * \defgroup Config_UDP_SN UDP Demo Safety Configuration Manager
 * Implementation of the SCM specific parts for the UDP demo
 *
 * \author Roland Knall <roland.knall@br-automation.com>
 *
 * \version 1.0
 *
 * \}
 * \}
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"


/** Allow node to receive it's own messages */
#define k_UDP_NETWORK_ALLOW_LOOPBACK 0


#define BUR_MAC_VENDOR_ID     0x00U,0x60U,0x65U

#define NR_OF_DEMO_NODES 2

/** SAPL_k_SCM_SN1_SADR:
    Source address of the SN1 with SCM functionality
*/
#define SAPL_k_SCM_SN1_SADR        0x01U

#define SAPL_k_SCM_UDID     {BUR_MAC_VENDOR_ID,0x00U,0x00U,SAPL_k_SCM_SN1_SADR}

/** SAPL_k_SN2_SADR:
    Source address of the SN2 without SCM functionality
*/
#define SAPL_k_SN2_SADR            0x02U

#define SAPL_k_SN2_UDID     {BUR_MAC_VENDOR_ID,0x00U,0x00U,SAPL_k_SN2_SADR}


