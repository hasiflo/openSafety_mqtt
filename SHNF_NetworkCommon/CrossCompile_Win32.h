/**
********************************************************************************
\file    CrossCompile_Win32.h

\brief    Header file with win32 definitions

\author    Josef Baumgartner - Bernecker & Rainer
\author    Roland Knall - Bernecker & Rainer

(C) BERNECKER + RAINER, AUSTRIA, A-5142 EGGELSBERG, B&R STRASSE 1

*******************************************************************************/

#ifndef WIN32_COMPAT_H
#define    WIN32_COMPAT_H

#include <winsock2.h>
#include <tchar.h>
#include <ws2tcpip.h>
#include <io.h>
#include <windows.h>
#include <stdio.h>
#include <pthread.h>

#include "osswin.h"

/******************************************************************************/
/* type definitions */

/* Mutex Handling */
/* Mutex Handling */
typedef pthread_mutex_t MUTEX_TYPE;

/* Thread Handling */
typedef pthread_t THREAD_TYPE;
typedef UINT32 THREAD_ID_TYPE;


/******************************************************************************/
/* compiler definitions */

/* Mutex Handling */
#define CREATE_BUFFER(buffer)           static pthread_mutex_t buffer = PTHREAD_MUTEX_INITIALIZER
#define LOCK_BUFFER(buffer)                pthread_mutex_lock( &buffer )
#define UNLOCK_BUFFER(buffer)           pthread_mutex_unlock( &buffer )



/******************************************************************************/
/* defines */


/* Thread Handling */
#define THREAD_FUNC                        void *
#define THREAD_FUNC_ARGUMENTS            void *
#define THREAD_RESULT_TYPE              int

#define THREAD_FAILURE(result)          (result != 0)
#define CREATE_THREAD(thread, threadroutine, result)  result = pthread_create ( &thread, NULL, threadroutine, NULL );

#define CLOSE_THREAD(thread)


/* Network Handling */
#ifndef INVALID_SOCKET
#define INVALID_SOCKET                  -1
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR                    -1
#endif
#ifndef RESULT_ERROR
#define RESULT_ERROR                    -1
#endif

#define SOCKET_RECV_TYPE                (char *)

#define INIT_SOCKET                     { \
                                            WSADATA wsaData; \
                                            /* initialize windows sockets */ \
                                            if (WSAStartup(0x0202, &wsaData) != 0) \
                                            { \
                                                printf ("Winsock start failed!\n"); \
                                                return (UINT32)RESULT_ERROR; \
                                            } \
                                        }
#define CLEANUP_SOCKET                  WSACleanup()


#define CLOSESOCKET                     closesocket

#define PRINT_SOCKET_EROR(X)            printf(" (%d)\n", WSAGetLastError(X));




/******************************************************************************/


#endif /* WIN32_COMPAT_H */
