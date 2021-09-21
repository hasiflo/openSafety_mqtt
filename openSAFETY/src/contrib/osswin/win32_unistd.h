/**
 * \file win32_unistd.h
 * This file intended to serve as a drop-in replacement for unistd.h on Windows
 * \addtogroup Tools
 * \{
 * \addtogroup osdd2sod openSAFETY Device Description to SOD converter
 * \{
 * \details
 * \{
 * Copyright (c) 2011,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the BSD license (according to License.txt).
 * \}
 * \author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \version 1.0
 */


#ifndef _UNISTD_H
#define _UNISTD_H    1

#if defined (_WIN32)

/* This file intended to serve as a drop-in replacement for 
 *  unistd.h on Windows
 *  Please add functionality as neeeded 
 */

#include <stdlib.h>
#include <io.h>

#include "osswin.h"
#include <getopt.h> /* getopt from: http://www.pwilson.net/sample.html. */

/** \brief Definition for the srandom command to be used */
#define srandom srand
/** \brief Definition for the random command to be used */
#define random rand


/** \brief Definition for the access command to be used */
#define access _access
/** \brief Definition for the ftruncate command to be used */
#define ftruncate _chsize

/** \brief Definition for the size datatype */
#define ssize_t int

/** \brief Definition for the STDIN file number */
#define STDIN_FILENO 0
/** \brief Definition for the STDOUT file number */
#define STDOUT_FILENO 1
/** \brief Definition for the STDERR file number */
#define STDERR_FILENO 2

/* should be in some equivalent to <sys/types.h> */
typedef __int8            int8_t;
typedef __int16           int16_t; 
typedef __int32           int32_t;
typedef __int64           int64_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef unsigned __int64  uint64_t;

#endif

// Copied from fcntl.h

#include <sys/stat.h>

#define F_DUPFD		1
#define F_GETFD		2
#define F_GETFL		3
#define F_GETLK		4
#define F_SETFD		5
#define F_SETFL		6
#define F_SETLK		7
#define F_SETLKW	8
#define F_GETLK64	9
#define F_SETLK64	10
#define F_SETLKW64	11

#define F_UNLCK		0
#define F_RDLCK		1
#define F_WRLCK		2

#define O_RDONLY	0x0000
#define O_WRONLY	0x0001
#define O_RDWR		0x0002
#define O_ACCMODE	0x0003

#define O_BINARY	0x0004	/* must fit in char, reserved by dos */
#define O_TEXT		0x0008	/* must fit in char, reserved by dos */
#define O_NOINHERIT	0x0080	/* DOS-specific */

#define O_CREAT		0x0100	/* second byte, away from DOS bits */
#define O_EXCL		0x0200
#define O_NOCTTY	0x0400
#define O_TRUNC		0x0800
#define O_APPEND	0x1000
#define O_NONBLOCK	0x2000

/* Additional non-POSIX flags for open(). */
/* They are present on GNU libc. */
#define O_NOLINK        0x4000
#define O_NOFOLLOW      0x8000

#define O_TEMPORARY	0x10000 /* Delete on close.  */

#define SH_COMPAT	0x0000
#define SH_DENYRW	0x0010
#define SH_DENYWR	0x0020
#define SH_DENYRD	0x0030
#define SH_DENYNO	0x0040

#define _SH_COMPAT	SH_COMPAT
#define _SH_DENYRW	SH_DENYRW
#define _SH_DENYWR	SH_DENYWR
#define _SH_DENYRD	SH_DENYRD
#define _SH_DENYNO	SH_DENYNO

/*
 *  For compatibility with other DOS C compilers.
 */

#define _O_RDONLY       O_RDONLY
#define _O_WRONLY       O_WRONLY
#define _O_RDWR         O_RDWR
#define _O_APPEND       O_APPEND
#define _O_CREAT        O_CREAT
#define _O_TRUNC        O_TRUNC
#define _O_EXCL         O_EXCL
#define _O_TEXT         O_TEXT
#define _O_BINARY       O_BINARY
#define _O_NOINHERIT    O_NOINHERIT

/*
 * Support for advanced filesystems (Windows 9x VFAT, NTFS, LFN etc.)
 */

#define _FILESYS_UNKNOWN	0x80000000U
#define _FILESYS_CASE_SENSITIVE	0x0001
#define _FILESYS_CASE_PRESERVED	0x0002
#define _FILESYS_UNICODE	0x0004
#define _FILESYS_LFN_SUPPORTED	0x4000
#define _FILESYS_VOL_COMPRESSED	0x8000

OSSWIN_EXPORT int mkstemp (char *tmpl);

#endif /* unistd.h  */

/* \} */

/* \} */
