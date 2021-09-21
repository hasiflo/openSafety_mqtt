/**
 * \file oscrccheck/oscrccheck.c
 * Main program for the command-line crc32 checker
 * \addtogroup Tools
 * \{
 * \addtogroup oscrccheck openSAFETY CRC check utility
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if defined (DO_CRC_MARK)
#include "oscrcmarker.h"
#endif

#include <getopt.h>

#include <oschecksum/crc.h>
#include <oscrccheck/oscrccheck.h>

/** \brief Helper definition which defines bool as an int */
#define bool int
/** \brief Helper definition which defines false as a bool 0 */
#define false (bool)0
/** \brief Helper definition which defines true as a bool 1 */
#define true 1

#if defined (_WIN32)
int calcCheckSumForFile(FILE *filePointer);
#endif
void ParseOpt(int argc, char *argv[], char ** listFile, char ** sourceFile);
void loadChecksumFromFile(const char * fileName, const char * testName);
int calcCheckSumForFile(FILE *filePointer);
char * checkSumForFile(FILE * filePointer);
int strCompare ( const char * word1, const char * word2 );
int stringIndex ( char haystack[], char needle[] );
void help();

/** \brief Generate a checksum for the given file */
bool gcheck = false;
/** \brief When the generated checksum gets printed, just print the checksum and nothing else */
bool greadable = false;
/** \brief Check the file against a checksum stored in a listfile */
bool lcheck = false;
/** \brief Do not print anything, just fail or succeed (by returning 0)*/
bool quiet = false;
/** \brief Color the output */
bool color = false;
/** \brief Always return with error code 0, the error code will be printed on stdout */
bool allways_be_succesful = false;

/** \brief Global variable for storing the calculated checksum */
char * gchecksum = NULL;

#if defined (_WIN32)
char * str_replace ( const char * strbuf, const char *strold, const char *strnew)
{
    char *strret, *p = NULL;
    char *posnews, *posold;
    size_t szold = strlen(strold);
    size_t sznew = strlen(strnew);
    size_t n = 1;

    if(!strbuf)
        return NULL;
    if(!strold || !strnew || !(p = strstr(strbuf, strold)))
        return strdup(strbuf);

    while(n > 0)
    {
        if(!(p = strstr(p+1, strold)))
            break;
        n++;
    }

    strret = (char*)malloc(strlen(strbuf)-(n*szold)+(n*sznew)+1);

    p = strstr(strbuf, strold);

    strncpy(strret, strbuf, (p-strbuf));
    strret[p-strbuf] = 0;
    posold = p+szold;
    posnews = strret+(p-strbuf);
    strcpy(posnews, strnew);
    posnews += sznew;

    while(n > 0)
    {
        if(!(p = strstr(posold, strold)))
            break;

        strncpy(posnews, posold, p-posold);
        posnews[p-posold] = 0;
        posnews += (p-posold);
        strcpy(posnews, strnew);
        posnews += sznew;
        posold = p+szold;
    }

    strcpy(posnews, posold);
    return strret;
}
#endif

/**
 * \brief This function will exit the program safely and taking the
 * state of \ref allways_be_succesful into consideration
 *
 * \param exitCode The exitcode to print or return
 */
void doExit ( int exitCode )
{
    //fprintf ( stdout, "Error Code: %d\n", exitCode);
    //fputs(errorTable[exitCode],stderr);
    exit ( exitCode );
}

/**
 * \brief The main function
 * \param argc the number of command line arguments given
 * \param argv the command line arguments as a character array
 */
int main(int argc, char *argv[])
{
    FILE *fpFile = NULL;
    char * szFile = NULL;
    char * listFile = NULL;

    /* We have at least 2 arguments */
    if (argc < 2)
    {
        fputs(errorTable[ERR_ARG_FEW],stderr);
        help();
        doExit(ERR_ARG_FEW);
    }

    ParseOpt(argc,argv, &listFile, &szFile);
    //printf ( "File: %s\n", szFile );
    if (NULL == (fpFile = fopen(szFile,"rb")))
    {
        if ( ! quiet ) fputs(errorTable[ERR_SYS_FILE_LOAD],stderr);
        doExit(ERR_SYS_FILE_LOAD);
    }
    //printf ( "gc: %d ; lc: %d\n", gcheck, lcheck);
    if (gcheck == false && lcheck == false)
    {
        if ( ! quiet )
        {
            if ( ! greadable )
                printf("CRCCheck: %s\t%s\n",checkSumForFile(fpFile),szFile);
            else
                printf("%s\n",checkSumForFile(fpFile));
        }
    }
    else
    {
        if ( lcheck == true ) {
            loadChecksumFromFile(listFile, szFile);
        } else if ( gcheck == true ) {
            if (strCompare(gchecksum, checkSumForFile(fpFile)) == 0)
            {
                if ( ! quiet ) fputs(errorTable[ERR_CRC],stderr);
                doExit(ERR_CRC);
            }
            else
                if ( ! quiet ) printf("ok");
        }
    }
    if ( fpFile != NULL ) fclose(fpFile);
    doExit(SYS_OK); /* We have to exit here, to perform a given stdout print of the error code */
    //printf ( "End of programm\n" );
    return 0;
}

/**
 * \brief Loads the checksum from a listfile and determines if it is correct or not
 * \param Testname The name of the file containing the list
 * \param fileName The name of the file, which shall be checked
 */
void loadChecksumFromFile(const char * fileName, const char * Testname)
{
    FILE *crcList = NULL;
    char * buffer = NULL;
    bool found = false;
    bool fileFound = false;
    char * listChecksum = NULL;
    char * listFile = NULL;
    char * testchecksum = NULL;
    uint32_t fileChecksum = 0;

#if defined(DO_CRC_MARK)
    if ( OSMARKER_CRC == 0 )
    {
        if ( ! quiet ) fputs ( "No marker has been stored!\n", stderr );
        doExit ( 10 );
    }
#endif

    buffer = (char *)malloc( BUFFER_LENGTH + 1 );

    if (NULL == (crcList = fopen(Testname,"rb")))
    {
        if ( ! quiet ) fputs ( errorTable[ERR_SYS_FILE_LOAD], stderr );
        exit ( ERR_SYS_FILE_LOAD );
        return;
    }

    /* Calculate CRC of file */
    fileChecksum = calcCheckSumForFile(crcList);
    if ( crcList != NULL ) fclose ( crcList );
    testchecksum = (char*)malloc ( sizeof(char) * 9 );
    memset ( testchecksum, '\0', sizeof(char) * 9 );
    sprintf ( testchecksum, outSyntax, fileChecksum );

    /* Open list for search */
    if (NULL == (crcList = fopen(fileName,"rb")))
    {
        free(testchecksum);
        if ( ! quiet ) fputs ( errorTable[ERR_SYS_FILE_LOAD], stderr );
        exit ( ERR_SYS_FILE_LOAD );
        return;
    }

#if defined(DO_CRC_MARK)
    /* Calculate CRC of list-file */
    fileChecksum = calcCheckSumForFile(crcList);
    if ( fileChecksum != OSMARKER_CRC )
    {
        if ( ! quiet ) fprintf ( stderr, "The stored marker [%.8X] is not equal to the list file checksum [%.8X]!\n",
                OSMARKER_CRC, fileChecksum );
        doExit ( 11 );
    }
#endif

    fseek ( crcList, 0, SEEK_SET );
    while ( ! found && ( 0 != fgets ( buffer, BUFFER_LENGTH, crcList ) ) )
    {
        listChecksum = strtok(buffer, ";");
        if ( listChecksum == NULL )
            break;
        listFile = strtok ( NULL, ";" );
        if ( listFile == NULL )
            break;

        if (strcmp(testchecksum, listChecksum) == 0)
            found = true;

        /** removing new line */
        if ( strchr (listFile, '\n') != NULL ) listFile[strlen ( listFile )-1] = '\0';
        /** removing carriage feed */
        if ( strchr (listFile, '\r') != NULL ) listFile[strlen ( listFile )-1] = '\0';

#if defined(_WIN32)
        if ( (int) stringIndex(str_replace ( Testname, "\\", "/" ), listFile) > 0 )
            fileFound = true;
#else
        if ( (int) stringIndex((char *)Testname, listFile) > 0 )
            fileFound = true;
#endif
    }
    if ( crcList != NULL )
    {
        free ( buffer );
        if ( testchecksum != NULL )
            free ( testchecksum );
        fclose(crcList);
    }

    if ( ! fileFound )
    {
        // File not found in list;
        if ( color ) fprintf ( stderr, "%s", COLOR_RED );
        fprintf( stderr, "CRC checksum not found in list for %s\n", Testname);
        if ( color ) fprintf ( stderr, "%s", COLOR_RESET );
        exit(ERR_CRC_NOT_FOUND_IN_LIST);
    }

    if ( ! found )
    {
        // Valid CRC Not Found
        if ( color ) fprintf ( stderr, "%s", COLOR_RED );
        fprintf( stderr, "CRC check failed for %s\n", Testname);
        if ( color ) fprintf ( stderr, "%s", COLOR_RESET );
        exit(ERR_CRC);
    } else {
        // Valid CRC found
        if ( color ) fprintf ( stderr, "%s", COLOR_GREEN );
        if ( ! quiet ) fprintf( stderr, "CRC check valid for %s\n", Testname);
        if ( color ) fprintf ( stderr, "%s", COLOR_RESET );
    }
}

/**
 * \brief Parses the given options and sets the global variables accordingly
 *
 * \param argc The number of given arguments
 * \param argv The arguments themselves
 * \retval listFile this will return the list for CRC32 codes, if 'l' was given
 * \retval sourceFile Only for the option 'f' an useful return will be given, in such case it will be
 *      the given filename
 */
void ParseOpt(int argc, char *argv[], char **listFile, char **sourceFile)
{
    int     c;

    while( EOF != (c = getopt(argc, argv, "f:hsxCqc:l:")) )
    {
        switch( c )
        {
            case 'f':       //file
            {
                *sourceFile = (char *) malloc ( ( strlen(optarg) + 1 ) * sizeof(char) );
                memset(*sourceFile, '\0', ( strlen(optarg) + 1 ) * sizeof(char));
                strcpy ( *sourceFile, optarg );
                break;
            }
            case 'l':       //file
            {
                *listFile = (char* )malloc( ( strlen(optarg) + 1 ) * sizeof(char) );
                memset(*listFile, '\0', ( strlen(optarg) + 1 ) * sizeof(char));
                strcpy(*listFile, optarg);
                lcheck = true;
                break;
            }
            case 'c':
            {
                lcheck = false;
                gcheck = true;
                greadable = true;
                gchecksum = optarg;
                break;
            }
            case 'C':
            {
                lcheck = false;
                gcheck = false;
                greadable = true;
                break;
            }
            case 'q':
            {
                quiet = true;
                break;
            }
            case 'x':
            {
                color = true;
                break;
            }
            case 's':
            {
                allways_be_succesful = true;
                break;
            }
            case 'h':       //help
            {
                help();
                exit(0);
            }
            default:        //false parameter
            {
                help();
                exit(ERR_ARG_UNKNOWN);
            }
        }
    }
}

/** \brief Displays the helpstring */
void help() { printf("%s", helpstring); }

/**
 * \brief Returns the CRC32 checksum for the given file as an integer
 * \param filePointer a pointer to an already opened file
 * \return the CRC32 checksum for the buffer as a character array
 */
int calcCheckSumForFile(FILE *filePointer)
{
    int crc = 0;
    uint32_t lSize;
    uint8_t * pBuffer;
    size_t result;

    fseek (filePointer , 0 , SEEK_END);
    lSize = ftell (filePointer);
    rewind (filePointer);
    if (lSize > 0)
    {
        pBuffer = (uint8_t*) malloc(lSize);
        if (pBuffer == NULL)
        {
            doExit(ERR_SYS_MEMORY);
        }
        result = fread(pBuffer,1,lSize,filePointer);
        if (result != (size_t) lSize)
        {
            doExit(ERR_SYS_MEMORY);
        }
        crc = crc32Checksum(lSize, (uint8_t*) pBuffer, 0);
        free (pBuffer);
    }

    return crc;
}

/**
 * \brief Returns the CRC32 checksum for the given file
 * \param filePointer a pointer to an already opened file
 * \return the CRC32 checksum for the buffer as a character array
 */
char * checkSumForFile(FILE *filePointer)
{
    int crc = 0;
    char * szOut = NULL;

    char crcHex[9];

    crc = calcCheckSumForFile(filePointer);
    if ( crc == 0 )
    {
        fputs(errorTable[ERR_PROG_CRC], stderr);
        exit(ERR_PROG_CRC);
    }

    sprintf(crcHex,outSyntax,crc);

    szOut = (char *)malloc ( sizeof ( char ) * 10 );
    strcpy ( szOut, crcHex );

    return szOut;
}

/**
 * \brief Compares two given strings, and either returns 0 if they are identical, or the number of characters they do have in common
 *
 * \param word1 first string for comparison
 * \param word2 second string for comparison
 * \return either one of the following
 *            - 0 if no similarities have been found
 *         - the length to which both strings are equal
 */
int strCompare ( const char * word1, const char * word2 )
{
    int i;

    /* If either word has zero length, return 0 */
    if ( strlen ( word1 ) <= 0 || strlen ( word2 ) <= 0 ) return 0;

    /* If both words are identical, return length of either one */
    if ( strcmp ( word1, word2 ) == 0 ) return ((int) strlen ( word1 ));

    /* Walk the string checking for comparison */
    for ( i = 0; (*word1 != '\0' && *word2 != '\0'); i++ )
    {
        if ( *word1 != *word2 )
        { i = -1; break; }
        word1++; word2++;
    }

    return i < 0 ? 0 : i;
}

/**
 * \brief Search for a given string in a larger string
 *
 * \param haystack the string which contains the string we are looking for
 * \param needle the string we are searching for
 * \return - -1 - if the string has not been found
 *         - pos - the position of the string in the haystack
 */
int stringIndex (char haystack[], char needle[])
{
  int i, j, k;

  for (i = 0; haystack[i] != '\0'; i++) {
    for (j = i, k = 0; needle[k] != '\0' && haystack[j] == needle[k]; j++, k++);

    if (needle[k] == '\0')
        return(i);
  }

  return(-1);
}

/**
 * \}
 * \}
 */
