/**
 * \file osddparser.h
 *
 * \addtogroup CROSSWIN32
 * \{
 * \addtogroup osswin
 * \{
 * \details
 * \{
 * Copyright 2003-2009 by Michael Sweet.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * \}
 * \author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 */

#ifndef OSSWIN_H
#define OSSWIN_H

#include "config.h"

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>		/* needed to define AF_ values on UNIX */
#endif

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>	/* needed to define AF_ values on Windows */
#if _MSC_VER < 1600	/* errno.h defines EAFNOSUPPORT in Windows VC10 (and presumably eventually in VC11 ...) */
#define EAFNOSUPPORT    WSAEAFNOSUPPORT
#endif
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_ARPA_NAMESER_H
#include <arpa/nameser.h>
#endif

#include <string.h>
#include <errno.h>

#ifndef OSSWIN_EXPORT
	#if defined (_WIN32)
		#if defined(osswin_EXPORTS)
			#define OSSWIN_EXPORT extern __declspec(dllexport)
		#else
			#if defined(OSSWIN_STATIC)
				#define OSSWIN_EXPORT
			#else
				#define OSSWIN_EXPORT extern __declspec(dllimport)
			#endif
		#endif /* osswin_EXPORTS */
	#else /* defined (_WIN32) */
		#define OSSWIN_EXPORT extern
	#endif
#endif

#pragma pack(push)
#pragma pack(4)

#ifdef __cplusplus
extern "C" {
#endif

struct in_addr;
OSSWIN_EXPORT int inet_aton(const char* cp_arg, struct in_addr *addr);


/**
 * inet_pton(af, src, dst)
 *	convert from presentation format (which usually means ASCII printable)
 *	to network format (which is usually some kind of binary format).
 * \return
 *	1 if the address was valid for the specified address family
 *	0 if the address wasn't valid (`dst' is untouched in this case)
 *	-1 if some other error occurred (`dst' is untouched in this case, too)
 * \author Paul Vixie, 1996.
 */
OSSWIN_EXPORT int inet_pton(int af, const char *src, void *dst);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif //OSSWIN_H

/**
 * \}
 * \}
 */
