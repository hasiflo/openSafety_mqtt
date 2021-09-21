/**
 * \file datalogger.h
 * \addtogroup Tools
 * \{
 * \addtogroup datalogger DataLogger library
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


#ifndef DATALOGGER_H_
#define DATALOGGER_H_

#ifdef WIN32
#include <windows.h>
#endif

#ifdef linux
/* Using for type va_list */
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <stdio.h>
#include <stdint.h>

#if defined (_WIN32)
  #if defined(datalogger_EXPORTS)
/** \brief Defines the export definition for the shared library */
    #define  DATALOGGER_EXPORT extern __declspec(dllexport)
  #else
/** \brief Defines the export definition for the shared library */
    #define  DATALOGGER_EXPORT extern __declspec(dllimport)
  #endif /* datalogger_EXPORTS */
#else /* defined (_WIN32) */
/** \brief Defines the export definition for the shared library */
 #define DATALOGGER_EXPORT extern
#endif

#pragma pack(push)
#pragma pack(4)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief DataLogger function for printing the message to string and to the debug output file.
 * This function only creates an output if DATA_LOGGER_PRINT_DEBUG is set
 *
 * \param format the format parameter for the output string
 *
 * \return the same return value printf would return
 */
DATALOGGER_EXPORT uint32_t DataLoggerDebug(char  * format, ...);


/**
 * \brief DataLogger function for printing the message to the debug output file.
 *
 * \param szFilename the name for the debug output file
 * \param format the format parameter for the output string
 *
 * \return the same return value printf would return
 */
DATALOGGER_EXPORT uint32_t DataLoggerFile(char * szFilename, char  * format, ...);

/**
 * \brief DataLogger function for printing the message to string and, depending on the configuration
 * to the debug output file.
 *
 * \param format the format parameter for the output string
 *
 * \return the same return value printf would return
 */
DATALOGGER_EXPORT uint32_t DataLoggerStdOut(char  * format, ...);

/** \brief Definitions for the output method */
#define DATA_LOGGER_OUTPUT
#undef DATA_LOGGER_OUTPUT_TO_FILE
#undef DATA_LOGGER_PRINT_DEBUG

#ifdef DATA_LOGGER_OUTPUT
#ifdef DATA_LOGGER_OUTPUT_TO_FILE
#define DATA_LOGGER_OUTPUT "debug.log"
#define DATA_LOGGER(fmt)            { DataLoggerStdOut(fmt); DataLoggerFile(fmt); }
#define DATA_LOGGER1(fmt,a)         { DataLoggerStdOut(fmt,a); DataLoggerFile(fmt,a); }
#define DATA_LOGGER2(fmt,a,b)       { DataLoggerStdOut(fmt,a,b); DataLoggerFile(fmt,a,b); }
#define DATA_LOGGER3(fmt,a,b,c)     { DataLoggerStdOut(fmt,a,b,c); DataLoggerFile(fmt,a,b,c); }
#define DATA_LOGGER4(fmt,a,b,c,d)   { DataLoggerStdOut(fmt,a,b,c,d); DataLoggerFile(fmt,a,b,c,d); }
#else
/** \brief Data logger for a single string */
#define DATA_LOGGER(fmt)            DataLoggerStdOut(fmt)
/** \brief Data logger for a single string and one argument */
#define DATA_LOGGER1(fmt,a)         DataLoggerStdOut(fmt,a)
/** \brief Data logger for a single string and two arguments */
#define DATA_LOGGER2(fmt,a,b)       DataLoggerStdOut(fmt,a,b)
/** \brief Data logger for a single string and three arguments */
#define DATA_LOGGER3(fmt,a,b,c)     DataLoggerStdOut(fmt,a,b,c)
/** \brief Data logger for a single string and four arguments  */
#define DATA_LOGGER4(fmt,a,b,c,d)   DataLoggerStdOut(fmt,a,b,c,d)
#endif
#else
/** \brief Data logger for a single string */
#define DATA_LOGGER(fmt)
/** \brief Data logger for a single string and one argument */
#define DATA_LOGGER1(fmt,a)
/** \brief Data logger for a single string and two arguments */
#define DATA_LOGGER2(fmt,a,b)
/** \brief Data logger for a single string and three arguments */
#define DATA_LOGGER3(fmt,a,b,c)
/** \brief Data logger for a single string and four arguments  */
#define DATA_LOGGER4(fmt,a,b,c,d)
#endif

#ifdef DATA_LOGGER_PRINT_DEBUG
/** \brief Debug data logger for a single string */
#define DATA_LOGGER_DEBUG(fmt)          DATA_LOGGER(fmt)
/** \brief Debug data logger for a single string and one argument */
#define DATA_LOGGER_DEBUG1(fmt,a)       DATA_LOGGER1(fmt,a)
/** \brief Debug data logger for a single string and two arguments */
#define DATA_LOGGER_DEBUG2(fmt,a,b)     DATA_LOGGER2(fmt,a,b)
/** \brief Debug data logger for a single string and three arguments */
#define DATA_LOGGER_DEBUG3(fmt,a,b,c)   DATA_LOGGER3(fmt,a,b,c)
/** \brief Debug data logger for a single string and four arguments  */
#define DATA_LOGGER_DEBUG4(fmt,a,b,c,d) DATA_LOGGER4(fmt,a,b,c,d)
#else
/** \brief Debug data logger for a single string */
#define DATA_LOGGER_DEBUG(fmt)
/** \brief Debug data logger for a single string and one argument */
#define DATA_LOGGER_DEBUG1(fmt,a)
/** \brief Debug data logger for a single string and two arguments */
#define DATA_LOGGER_DEBUG2(fmt,a,b)
/** \brief Debug data logger for a single string and three arguments */
#define DATA_LOGGER_DEBUG3(fmt,a,b,c)
/** \brief Debug data logger for a single string and four arguments */
#define DATA_LOGGER_DEBUG4(fmt,a,b,c,d)
#endif

#ifdef __cplusplus
}
#endif

#pragma pack(pop)


#endif /* DATALOGGER_H_ */

/**
 * \}
 * \}
 */
