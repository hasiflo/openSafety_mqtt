/**
 * \file getopt.h
 *
 * \addtogroup CROSSWIN32
 * \{
 * \addtogroup osswin
 * \{
 * \details
 * \{
 * \}
 * \author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 */


#ifndef GETOPT_H
#define GETOPT_H

#include "osswin.h"

#include <string.h>
#include <errno.h>

OSSWIN_EXPORT char * optarg;		// global argument pointer
OSSWIN_EXPORT int optind; 	// global argv index
OSSWIN_EXPORT int opterr;
OSSWIN_EXPORT int optopt;

OSSWIN_EXPORT int getopt(int nargc, char* const nargv[], const char* ostr);

#endif //GETOPT_H

/**
 * \}
 * \}
 */
