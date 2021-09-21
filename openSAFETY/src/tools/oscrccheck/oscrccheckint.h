/**
 * \file oscrccheckint.h
 *
 * \author knallr
 * \copyright {
 *   Copyright (c) 2015,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *   All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * }
 *
 * \version 17.02.2015
 */
#ifndef TOOLS_OSCRCCHECK_OSCRCCHECKINT_H_
#define TOOLS_OSCRCCHECK_OSCRCCHECKINT_H_


/** \brief Maximal length for a character buffer */
#define BUFFER_LENGTH 150

#if (WIN32)
/** \brief Shell color definition for colored output */
#define COLOR_RED ""
/** \brief Shell color definition for colored output */
#define COLOR_GREEN ""
/** \brief Shell color definition for colored output */
#define COLOR_RESET ""
#else
/** \brief Shell color definition for colored output */
#define COLOR_RED "\e[1;31m"
/** \brief Shell color definition for colored output */
#define COLOR_GREEN "\e[0;32m"
/** \brief Shell color definition for colored output */
#define COLOR_RESET "\e[0m"
#endif

/** \brief Definition of the output format */
static const char* const outSyntax = "%.8x";

/** \brief Help output to display to the caller */
static const char* const helpstring =
"\n\
Call: CRCCheck [OPTION] [ARGUMENT]...\n\
To calculate the CRC and verify files with the CRC value\n\
\n\
  -f    file for which the CRC32 should be calculated\n\
  -l    loads the checksum from the given list\n\
  -c    checksum of file\n\
  -q    no output on succes or failure, just return value\n\
  -x    color the output\n\
  -h    print this text\n\
\n\
\n";

/** \brief Help output to display to the caller */
static const char* const helpstring2 =
"\n\
Call: libcrccalc <output_dir> <calc_file>\n \
\n";

/** \brief Possible error codes for checksum check */
enum e_CRCErrorCodes
{
    /** \brief Everything went ok */
    SYS_OK = 0,
    /** \brief The file could not be read */
    ERR_SYS_FILE_LOAD,
    /** \brief Not enough memory to load the file */
    ERR_SYS_MEMORY,
    /** \brief The given file could not be read */
    ERR_SYS_FILE_READ,
    /** \brief Crc32 algorithm error */
    ERR_PROG_CRC,
    /** \brief Arguments error/complication */
    ERR_ARG,
    /** \brief Too few arguments given */
    ERR_ARG_FEW,
    /** \brief Unknown arguments given */
    ERR_ARG_UNKNOWN,
    /** \brief CRC values do not match */
    ERR_CRC,
    /** \brief CRC value not found for file */
    ERR_CRC_NOT_FOUND_IN_LIST
};

/** \brief Readable error values for the enum \ref e_CRCErrorCodes */
static const char* const errorTable[] =
{
    "",
    "CRCCheck: The file could not be accessed",
    "CRCCheck: Insufficient memory",
    "CRCCheck: The given file could not be read",
    "CRCCheck: Crc32 algorithm error",
    "CRCCheck: Arguments error/complication",
    "CRCCheck: Too few arguments given",
    "CRCCheck: Unknown arguments given",
    "CRCCheck: CRC values do not match",
    "CRCCheck: CRC value not found for file"
};


#endif /* TOOLS_OSCRCCHECK_OSCRCCHECKINT_H_ */
