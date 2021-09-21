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

#include "Network.h"
#include "NetHelper.h"
#include "datalogger.h"

/**
 * \brief This function retrieves and/or opens the global socket. Additionally it
 *        determines which IP Address will be used for this instance.
 *
 * \remarks This function sets the \see sia_localSystemIPAddress variable to the
 *        selected IP address
 *
 * \return
 *     - NETWORK_FAILURE (-1) if obtaining a socket descriptor failed or
 *     - the file descriptor for the socket
 */
UINT32 GetSocketForSender ()
{
    struct sockaddr_in sockaddr;
    UINT32 fd_udpSocket;
    UINT32 broadcast;  /* broadcast flag */

    if ( i_socketSender != 0 )
        return i_socketSender;

    INIT_SOCKET;

    /* Open a socket for receiving data */
    if ( ( fd_udpSocket = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == INVALID_SOCKET )
    {
        DATA_LOGGER1 ( "SHNFnetwork::GetSocketForSender() - Error obtaining socket [%d] for message receiver thread!\n", errno);
        exit(1);
        return NETWORK_FAILURE;
    }

    memset ( &sockaddr, 0, sizeof ( sockaddr ) );
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons ( k_UDP_SERVICE_PORT );
    TargetOnNetwork(0xFFFF, &sockaddr.sin_addr);

    /* Setting the capability to send and receive broadcast messages */
    broadcast = 1;
    if ( ( setsockopt ( fd_udpSocket,SOL_SOCKET,SO_BROADCAST, SOCKET_RECV_TYPE &broadcast, sizeof ( broadcast ) ) ) == SOCKET_ERROR)
    {
        DATA_LOGGER ( "SHNFnetwork::GetSocketForSender() - Error setting broadcast availability\n");
        CLOSESOCKET(fd_udpSocket);
        exit(1);
        return NETWORK_FAILURE;
    }

    i_socketSender = fd_udpSocket;
    return fd_udpSocket;
}

/**
 * \brief This function retrieves and/or opens the global socket. Additionally it
 *        determines which IP Address will be used for this instance.
 *
 * \remarks This function sets the \see sia_localSystemIPAddress variable to the
 *        selected IP address
 *
 * \return
 *     - NETWORK_FAILURE (-1) if obtaining a socket descriptor failed or
 *     - the file descriptor for the socket
 */
UINT32 GetSocketForReceiver ()
{
    struct sockaddr_in sockaddr;
    UINT32 fd_udpSocket;
    UINT32 broadcast;  /* broadcast flag */

    if ( i_socketReceiver != 0 )
        return i_socketReceiver;

    INIT_SOCKET;

    /* Open a socket for receiving data */
    if ( ( fd_udpSocket = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == INVALID_SOCKET )
    {
        DATA_LOGGER1 ( "SHNFnetwork::GetSocketForReceiver() - Error obtaining socket [%d] for message receiver thread!\n", errno);
        exit(1);
        return NETWORK_FAILURE;
    }

    memset ( &sockaddr, 0, sizeof ( sockaddr ) );
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons ( k_UDP_SERVICE_PORT );

    /*
     * Binding the socket to the port has failed. This can happen for various reasons:
     *  - Port is invalid or not accepted -> Port number too high or negative
     *  - Port is already bound to another socket -> another program is running using this port
     *  - Insuficcient permission -> user has no right to use this port (too low in Linux)
     */
    if ( bind ( fd_udpSocket, (struct sockaddr *) &sockaddr, sizeof ( struct sockaddr ) ) == SOCKET_ERROR ) {
        DATA_LOGGER3 ( "SHNFnetwork::GetSocketForReceiver() - Error binding socket [%d] to port [%d] for message receiver!\n\t%s\n", errno,
                      k_UDP_SERVICE_PORT, strerror(errno));
        CLOSESOCKET(fd_udpSocket);
        exit(1);
        return NETWORK_FAILURE;
    }

    /* Setting the capability to send and receive broadcast messages */
    broadcast = 1;
    if ( ( setsockopt ( fd_udpSocket,SOL_SOCKET, SO_BROADCAST, SOCKET_RECV_TYPE &broadcast, sizeof ( broadcast ) ) ) == SOCKET_ERROR)
    {
        DATA_LOGGER ( "SHNFnetwork::GetSocketForReceiver() - Error setting broadcast availability\n");
        CLOSESOCKET(fd_udpSocket);
        exit(1);
        return NETWORK_FAILURE;
    }

    i_socketReceiver = fd_udpSocket;
    return fd_udpSocket;
}

/**
 * \brief Closes the connection and cleans up the socket
 */
void CleanUpSocket()
{
    CLOSESOCKET(GetSocketForSender());
    CLOSESOCKET(GetSocketForReceiver());
    CLEANUP_SOCKET;
}

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
                            struct sockaddr * connectionData )
{
    UINT8 * c_recvBuffer;
    struct sockaddr_in servaddr;
    socklen_t sl_addressLength;
    UINT32 i_numBytesReceived = 0;
    UINT32 fd_udpSocket;

    /* Allocating the memory for the buffer */
    c_recvBuffer = (UINT8*) malloc ( sizeof(UINT8) * ( i_ReceiveBufferLength + 1 ) );
    /* Zero'ing the buffer to prevent ghost memory */
    memset ( c_recvBuffer, 0, sizeof(UINT8) * ( i_ReceiveBufferLength + 1 ) );

    /* Open a socket for receiving data */
    if ( ( fd_udpSocket = GetSocketForReceiver() ) == NETWORK_FAILURE )
    {
        DATA_LOGGER1 ( "Error obtaining socket [%d] for message receiver thread!\n", errno);
        return NETWORK_FAILURE;
    }

    servaddr.sin_port = 0;
    sl_addressLength = sizeof(struct sockaddr);
    i_numBytesReceived = 0;

    while ( i_numBytesReceived == 0 )
    {
        if ( ( i_numBytesReceived = recvfrom ( fd_udpSocket, SOCKET_RECV_TYPE c_recvBuffer, i_ReceiveBufferLength,
                (UINT8) 0, (struct sockaddr *) &servaddr, &sl_addressLength ) ) == NETWORK_FAILURE )
        {
            DATA_LOGGER2 ( "Error receiving from socket [%d] - %s!\n", errno, strerror ( errno ) );
            // i_numBytesReceived = NETWORK_FAILURE; - obsolete, as it is already set to this value
            break;
        }
    }

    /* If the loop broke because of a failure, we have to return */
    if ( i_numBytesReceived == NETWORK_FAILURE )
    {
        free ( c_recvBuffer );
        return NETWORK_FAILURE;
    }

#if linux
    DATA_LOGGER_DEBUG2("Package [%d bytes] received from %s\n",
                (int) i_numBytesReceived,
                inet_ntoa(servaddr.sin_addr));
#endif

    /* Return the data as well as the connection information */
    memcpy ( c_ReceiveBuffer, c_recvBuffer, i_numBytesReceived );
    memcpy ( connectionData, &servaddr, sizeof(struct sockaddr) );

    /** If memset is left out here, the call to free may destroy the received packet on Linux.
     *  So we call memset first, and free afterwards.  */
    memset ( c_recvBuffer, 0, i_numBytesReceived );
    free ( c_recvBuffer );

    return i_numBytesReceived;
}

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
UINT32 SendOnNetwork ( UINT16 i_targetAddress, const UINT8 * c_sendBuffer, UINT32 i_sendBufferLength )
{
    UINT32 i_NumOfBytes = 0;
    UINT32 fd_udpSocket = 0;
    socklen_t servlen;
    struct sockaddr_in servaddr;

    /* Open a socket for receiving data */
    if ( ( fd_udpSocket = GetSocketForSender() ) == NETWORK_FAILURE )
    {
        DATA_LOGGER1 ( "Error obtaining socket [%d] for message receiver thread!\n", errno);
        return NETWORK_FAILURE;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons ( k_UDP_SERVICE_PORT );
    /** Determine the sending address */
    TargetOnNetwork(i_targetAddress, &servaddr.sin_addr);

    servlen = sizeof( servaddr );

    DATA_LOGGER_DEBUG1 ( "Sending Frame ... [%d bytes] ", (int) i_sendBufferLength );
    if ( ( i_NumOfBytes = sendto ( fd_udpSocket, SOCKET_RECV_TYPE c_sendBuffer, i_sendBufferLength,
                     0, (struct sockaddr * )&servaddr, servlen ) ) <= 0 )
    {
        DATA_LOGGER_DEBUG ( "- " );
        DATA_LOGGER2 ( "Error sending to socket [%d] - %s!\n", errno, strerror ( errno ) );
        return NETWORK_FAILURE;
    }
    else
    {
        DATA_LOGGER_DEBUG1 ( "%d bytes sent\n", (int) i_NumOfBytes );
    }

    return i_NumOfBytes;
}
/* \}*/
