/*
 * \file datalogger.c
 */

#if defined(_WIN32)
#pragma warning(disable:4996)
#include <windows.h>
#else
/* Using for type va_list */
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <stdio.h>


#include <datalogger.h>

uint32_t DataLoggerDebug(char * format, ...)
{
#ifdef DATA_LOGGER_PRINT_DEBUG
    int ret;
    char szBuffer[512];
    va_list args;

    /* Start with the first argument on the stack */
    va_start(args, format);

    /* parse the string into the return Buffer */
    ret = vsprintf(szBuffer, format, args);

    /* close variable arguments handling */
    va_end(args);

    /* print the string to file */
    DataLoggerFile(DATA_LOGGER_OUTPUT, "%s", szBuffer);

    /* print the string to screen */
    DataLoggerStdOut(DATA_LOGGER_OUTPUT, "%s", szBuffer);

    return ret;
#else
    return 0;
#endif
}

uint32_t DataLoggerFile(char * szFilename, char  * format, ...)
{
    int ret;
    char szBuffer[512];
    va_list args;
    FILE * fileOutput;

    /* Trying to open the file for writing */
    if ( ( fileOutput = fopen ( szFilename, "w" ) ) != 0 )
        return -1;

    /* Start with the first argument on the stack */
    va_start(args, format);

    /* parse the string into the return Buffer */
    ret = vsprintf(szBuffer, format, args);

    /* close variable arguments handling */
    va_end(args);

    /* print the string to file */
    fprintf ( fileOutput, "%s\n", szBuffer );
    fclose ( fileOutput );

    /* print the string to screen */
    printf ( "%s", szBuffer );

    return ret;
}

uint32_t DataLoggerStdOut(char  * format, ...)
{
    int ret;
    char szBuffer[512];
    va_list args;

    /* Start with the first argument on the stack */
    va_start(args, format);

    /* parse the string into the return Buffer */
    ret = vsprintf(szBuffer, format, args);

    /* close variable arguments handling */
    va_end(args);

    /* print the string to screen */
    printf ( "%s", szBuffer );

    return ret;
}

