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
#include "NetHelper.h"

#ifndef k_UDP_TARGET_NETWORK
  #error The compiler define 'k_UDP_TARGET_NETWORK' has to be set in the target main header file
#endif

void StoreLocalAddress();

/**
 * \brief The local IP address will be stored, so that ReceiveFromNetwork will
 * ignore any message comming from this address
 */
static struct in_addr * sia_localSystemIPAddress = (struct in_addr *)NULL;

/**
 * \brief The local broadcast address will be stored, so that ReceiveFromNetwork will
 * ignore any message comming from this address
 */
static struct in_addr * sia_localBroadcastAddress = (struct in_addr *)NULL;

/**
 * \brief The name of the local network adapter used
 */
static char * c_AdapterName = (char *)NULL;

/**
 * This function returns the address on the network as well as the name of the adapter used
 *
 * \param  i_bufLength - the maximum length of the character buffer
 * \retval adapterDescription - the name for the interface used
 * \retval localIPAddress - the in_addr structure for the address
 * \retval broadcast - the in_addr structure for the broadcast address
 */
void AdapterOnNetwork ( UINT16 i_bufLength, char ** adapterDescription, struct in_addr * localIPAddress, struct in_addr * broadcast )
{
    /** Let's determine the network adapter and broadcast address for sending */
    if ( sia_localSystemIPAddress == (struct in_addr*) NULL )
        StoreLocalAddress();

    memcpy(*adapterDescription, c_AdapterName, (strlen(c_AdapterName) > i_bufLength ? i_bufLength : strlen(c_AdapterName)));
    memcpy(localIPAddress, sia_localSystemIPAddress, sizeof(struct in_addr));
    memcpy(broadcast, sia_localBroadcastAddress, sizeof(struct in_addr));
}

/**
 * This function returns the address on the network, we are going to send to for the
 * given target. If the given target is 0xFFFF, we are returning the local adapter address
 *
 * \param i_targetAddress - the address for the intended target, or 0xFFFF for the local adapter
 *                          interface
 * \retval localIPAddress - the in_addr structure for the address
 */
void TargetOnNetwork ( UINT16 i_targetAddress, struct in_addr * localIPAddress )
{
    /** Let's determine the network adapter and broadcast address for sending */
    if ( sia_localSystemIPAddress == (struct in_addr*) NULL )
        StoreLocalAddress();

    /** If no address could be found, exit the application */
    if ( sia_localSystemIPAddress == (struct in_addr*) NULL )
    {
        DBG_PRINTF1("No network could be found for sending. The configured network %s was not available.\n", k_UDP_TARGET_NETWORK );
        exit (1);
    }

    /* If the given target address is 0, we return the broadcast */
    if ( i_targetAddress == 0x00 )
    {
        BroadcastNetwork(localIPAddress);
        return;
    }

    /* We return always the network on linux, on windows we return the system ip */
    if ( i_targetAddress == 0xFFFF )
    {
#if linux
        inet_pton(AF_INET, k_UDP_TARGET_NETWORK, localIPAddress );
#else
        memcpy(localIPAddress, sia_localSystemIPAddress, sizeof(struct in_addr));
#endif
        return;
    }

    /** Lookup for IP is not implemented yet */
    BroadcastNetwork(localIPAddress);
}

/**
 * This function returns the broadcast address on the network, we are going to send to for the
 * given target. Under linux it always returns the network address instead, as otherwise, the
 * broadcast packages would not be received
 *
 * \retval localIPAddress - the in_addr structure for the broadcast address
 */
void BroadcastNetwork ( struct in_addr * localIPAddress )
{
    if ( sia_localBroadcastAddress == (struct in_addr*) NULL )
        StoreLocalAddress();

    memcpy(localIPAddress, sia_localBroadcastAddress, sizeof(struct in_addr));
}

void StoreIPForSN ( struct in_addr snAddress, UINT16 senderAddress )
{

}

/**
 * This method determines the network interface for which we are called, and sets
 * the broadcast and network interface address accordingly. This will not be used under
 * linux, but windows will rely on it
 */
void StoreLocalAddress()
{
    /*
     * Let's determine, which network adapter we will be using, and which IP address
     * it is registered too. This is not needed on linux, as actually it will not
     * work on linux, sending broadcast packages using a specific interface. But for
     * windows the socket must be bound to a given network adapter.
     */
    if ( sia_localSystemIPAddress == (struct in_addr*) NULL )
    {
#if linux
        struct sockaddr servaddr;
        socklen_t sl_addressLength;

        UINT8 ifconf_Buffer [ 1024 ]; /* interface configuration buffer */
        struct ifreq *ifr; /* interface reqistry */
        struct ifreq *item;
        UINT32 i_Interfaces; /* number of available interfaces */
        UINT32 i = 0; /* loop counter */
        struct ifconf ifc; /* interface configuration */
        UINT32 fd_ioctlSocket;
        struct in_addr network;
        struct in_addr netmask;
        struct in_addr ipaddress;
        struct in_addr broadcast;

        /* Open a socket for receiving data */
        if ( ( fd_ioctlSocket = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == NETWORK_FAILURE )
        {
            DBG_PRINTF1 ( "Error obtaining socket [%d] for message receiver thread!\n", errno);
            return;
        }

        /* Query available interfaces. */
        ifc.ifc_len = sizeof(ifconf_Buffer);
        ifc.ifc_buf = ifconf_Buffer;
        if(ioctl(fd_ioctlSocket, SIOCGIFCONF, &ifc) < 0)
        {
            DBG_PRINTF2 ( "Error obtaining interface information [%d]\n\t%s\n",
                          errno, strerror ( errno ));
            close (fd_ioctlSocket);
            return;
        }

        /* Iterate through the list of interfaces. */
        ifr = ifc.ifc_req;
        i_Interfaces = ifc.ifc_len / sizeof(struct ifreq);
        for ( i = 0; i < i_Interfaces; i++ )
        {
            item = &ifr[i];

            /* Get the IP netmask */
            if(ioctl(fd_ioctlSocket, SIOCGIFNETMASK, item) == NETWORK_FAILURE)
                continue;
            netmask = ((struct sockaddr_in *)&item->ifr_addr)->sin_addr;

            /* Get the IP address */
            if(ioctl(fd_ioctlSocket, SIOCGIFADDR, item) == NETWORK_FAILURE)
                continue;
            ipaddress = ((struct sockaddr_in *)&item->ifr_addr)->sin_addr;

            /* Calculating network address */
            network.s_addr = ipaddress.s_addr & netmask.s_addr;

            /* If the netmask does not match the target network, continue */
            if ( strcmp ( inet_ntoa(network), (char *)k_UDP_TARGET_NETWORK ) != 0 )
            {
                /* It could be that we operate on a TUN/TAP adapter. For instance, if
                 * the communication operates on a VPN connection. In such a case
                 * often the netmask is 255.255.255.255, which will lead the algorithm
                 * useless, as the & operation above will just return the ip address.
                 */

                continue;
            }
            printf ( "b\n" );
            /* store the local system ip address */
            sia_localSystemIPAddress = (struct in_addr *)malloc ( sizeof ( struct in_addr ) );
            memset ( sia_localSystemIPAddress, 0, sizeof ( struct in_addr ));
            memcpy( sia_localSystemIPAddress, &(ipaddress), sizeof(struct in_addr));

            /* Calculating broadcast address */
            broadcast.s_addr = network.s_addr + ( 0xFFFFFFFF ^ netmask.s_addr );

            /* store the local broadcast address */
            sia_localBroadcastAddress = (struct in_addr *)malloc ( sizeof ( struct in_addr ) );
            memset ( sia_localBroadcastAddress, 0, sizeof ( struct in_addr ));
            memcpy( sia_localBroadcastAddress, &broadcast, sizeof(struct in_addr));

            c_AdapterName = (char *)malloc ( sizeof ( char ) * ( strlen ( item->ifr_name) + 1 ) );
            memset ( c_AdapterName, 0, sizeof ( char ) * ( strlen ( item->ifr_name) + 1 ) );
            strcpy ( c_AdapterName, item->ifr_name );
            break;
        }

        close(fd_ioctlSocket);
#else
        unsigned long ipaddress, netmask, broadcast, network;
        struct in_addr address_struct;
        PIP_ADAPTER_INFO pAdapterInfo;
        DWORD dwBufLen;
        DWORD dwStatus;

        IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information for up to 16 NICs
        dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

        // Call GetAdapterInfo,  [out] buffer to receive data, [in] size of receive data buffer
        dwStatus = GetAdaptersInfo( AdapterInfo, &dwBufLen);
        if ( dwStatus != ERROR_SUCCESS )
            return;

        pAdapterInfo = AdapterInfo; // Contains pointer to
                                                    // current adapter info
        do {

            ipaddress = inet_addr ( pAdapterInfo->IpAddressList.IpAddress.String );
            netmask = inet_addr ( pAdapterInfo->IpAddressList.IpMask.String );
            network = ipaddress & netmask;

            // pack the address into the struct inet_ntoa expects
            address_struct.s_addr = network;

            if ( strcmp ( inet_ntoa(address_struct), k_UDP_TARGET_NETWORK ) != 0 )
            {
                pAdapterInfo = pAdapterInfo->Next;    // Progress through
                continue;
            }

            /* Calculating broadcast address */
            broadcast = network + ( 0xFFFFFFFF ^ netmask );

            // pack the address into the struct inet_ntoa expects
            address_struct.s_addr = ipaddress;

            /* store the local system ip address */
            sia_localSystemIPAddress = (struct in_addr *)malloc ( sizeof ( struct in_addr ) );
            memset ( sia_localSystemIPAddress, 0, sizeof ( struct in_addr ));
            memcpy( sia_localSystemIPAddress, (void *)&address_struct, sizeof(struct in_addr));

            // pack the address into the struct inet_ntoa expects
            address_struct.s_addr = broadcast;

            /* store the local broadcast address */
            sia_localBroadcastAddress = (struct in_addr *)malloc ( sizeof ( struct in_addr ) );
            memset ( sia_localBroadcastAddress, 0, sizeof ( struct in_addr ));
            memcpy( sia_localBroadcastAddress, (void *)&address_struct, sizeof(struct in_addr));

            c_AdapterName = (char *)malloc ( sizeof ( char ) * ( strlen ( pAdapterInfo->Description) + 1 ) );
            memset ( c_AdapterName, 0, sizeof ( char ) * ( strlen ( pAdapterInfo->Description) + 1 ) );
            strcpy ( c_AdapterName, pAdapterInfo->Description );
            break;
        }
        while(pAdapterInfo);                    // Terminate if last adapter
#endif
    }

    if ( sia_localSystemIPAddress == (struct in_addr*) NULL )
    {
        DBG_PRINTF1("SHFNNethelper.c::StoreLocalAddress() - No network could be found for sending.\n" \
                "\tThe configured network %s was not available.\n\n", k_UDP_TARGET_NETWORK );
        exit (1);
    }

    if ( sia_localBroadcastAddress == (struct in_addr*) NULL )
    {
        DBG_PRINTF1("SHFNNethelper.c::StoreLocalAddress() - The specified network does not have a broadcast address.\n" \
                "\tThe configured network %s can not be used for sending.\n\n", k_UDP_TARGET_NETWORK );
        exit (1);
    }
}

/* \} */
