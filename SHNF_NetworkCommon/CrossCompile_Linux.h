/**
*******************************************************************************
\file   CrossCompile_Linux.h

\brief  Header file with linux specific definitions

\author    Josef Baumgartner - Bernecker & Rainer
\author    Roland Knall -  Bernecker & Rainer

(C) BERNECKER + RAINER, AUSTRIA, A-5142 EGGELSBERG, B&R STRASSE 1

This header file contains linux specific definitons for the B&R trace tool.
******************************************************************************/

#ifndef LINUX_COMPAT_H
#define    LINUX_COMPAT_H

#ifdef linux

/******************************************************************************/
/* linux specific includes */

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <errno.h>


/******************************************************************************/
/* type definitions */

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
/* type definitions */

typedef int SOCKET;

typedef struct sockaddr_in SOCKADDR_IN;

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

#define SOCKET_RECV_TYPE

#define INIT_SOCKET
#define CLEANUP_SOCKET

#define CLOSESOCKET                     close

#define PRINT_SOCKET_EROR(X)            printf(" (%s)\n", strerror(errno));


#endif /* linux */

#endif /* LINUX_COMPAT_H */
