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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include <oschecksum/crc.h>
#include <oscrccheck/oscrccheck.h>

#if defined (_WIN32)
#define DIR_DELIMITER "\\"
#else
#define DIR_DELIMITER "/"
#endif

int calcCheckSumForFile(FILE *filePointer);
char * checkSumForFile(FILE * filePointer);

/**
 * \brief The main function
 * \param argc the number of command line arguments given
 * \param argv the command line arguments as a character array
 */
int main(int argc, char *argv[])
{
    FILE *fpFile = NULL;
    FILE *outFile = NULL;
    char * szFile = NULL;
    char * listFile = NULL, * libFile = NULL;
    int i = 0, j = 0;

	/* We have at least 2 arguments */
    if (argc != 3)
    {
        fprintf(stderr, "%s: Too few arguments given\n", argv[0]);
        exit(ERR_ARG_FEW);
    }

    j = (int) strlen(argv[1]) + (int) strlen(argv[2]) + (int) strlen("bur.crc") + 2;
    listFile = (char *)malloc(sizeof(char) * (j + 1) );
    sprintf ( listFile, "%s%s%s%s%s\0", argv[1], DIR_DELIMITER, argv[2], DIR_DELIMITER, "bur.crc" );

    outFile = fopen ( listFile, "wb" );
    if ( ! outFile )
    {
		printf ( "%s: bur.crc could not be opened for writing at %s%s%s\n", argv[0], argv[1], DIR_DELIMITER, argv[2] );
		exit(1);
    }

    fprintf(outFile, "# CRC database for B&R files \r\n" );
    fprintf(outFile, "# created with %s v1.1 \r\n", argv[0]);

    free( listFile );

    j = (int) strlen(argv[1]) + (int) strlen(argv[2]) + (int) strlen("Library.xml") + 2;
    listFile = (char *)malloc(sizeof(char) * (j + 1) );
    sprintf ( listFile, "%s%s%s%s%s\0", argv[1], DIR_DELIMITER, argv[2], DIR_DELIMITER, "Library.xml" );

	fpFile = fopen ( listFile, "rb" );
	if ( ! fpFile )
    {
		printf ( "%s: Library.xml not found at %s%s%s\n", argv[0], argv[1], DIR_DELIMITER, argv[2] );
		if ( outFile != NULL ) fclose(outFile);
		exit(2);
    }
	fprintf(outFile, "C\tPLC\\fw_lib\\%s\\%s\t%s \r\n",argv[2], "Library.xml" ,checkSumForFile(fpFile));
	if ( fpFile != NULL ) fclose(fpFile);

    j = (int) strlen(argv[1]) + 2 * (int) strlen(argv[2]) + (int) strlen("Lib.a") + 2;
    listFile = (char *)malloc(sizeof(char) * (j + 1) );
    sprintf ( listFile, "%s%s%s%s%s%s%s\0", argv[1], DIR_DELIMITER, argv[2], DIR_DELIMITER, "Lib", argv[2], ".a" );

	fpFile = fopen ( listFile, "rb" );
    if ( ! fpFile )
    {
		printf ( "%s: Lib%s.a not found at %s%s%s%sLib%s.a\n", argv[0], argv[2], argv[1], DIR_DELIMITER,
				argv[2], DIR_DELIMITER, argv[2] );
		if ( outFile != NULL ) fclose(outFile);
		exit(3);
    }

    //printf ( "gc: %d ; lc: %d\n", gcheck, lcheck);
   	fprintf(outFile, "C\tPLC\\fw_lib\\%s\\%s%s%s\t%s ",argv[2], "Lib", argv[2] , ".a" ,checkSumForFile(fpFile));

   	if ( fpFile != NULL ) fclose(fpFile);

    if ( outFile != NULL ) fclose(outFile);

    return 0;
}

unsigned int crc32Checksum(unsigned int len, const unsigned char * pBuffer, unsigned int initCRC)
{
	unsigned int crc = initCRC;
	unsigned int i = 0;

	for ( i = 0; i < len; i++ )
		crc = PrecompiledCRC32PNG[(int) (((crc ^ pBuffer[i]) & ((unsigned int) 0xffL)))] ^ (crc >> 8);
	crc ^= initCRC;
	return crc;
}

/**
 * \brief Returns the CRC32 checksum for the given file as an integer
 * \param filePointer a pointer to an already opened file
 * \return the CRC32 checksum for the buffer as a character array
 */
int calcCheckSumForFile(FILE *filePointer)
{
	int crc = 0;
	unsigned int lSize;
	unsigned char * pBuffer;
	size_t result;
	rewind (filePointer);

	fseek (filePointer , 0 , SEEK_END);
	lSize = ftell (filePointer);
	rewind (filePointer);
	if (lSize > 0)
	{
		pBuffer = (unsigned char*) malloc(lSize);
		if (pBuffer == NULL)
		{
			exit(ERR_SYS_MEMORY);
		}
		result = fread(pBuffer,1,lSize,filePointer);
		if (result != (size_t) lSize)
		{
			//doExit(ERR_SYS_MEMORY);
		}
		crc = crc32Checksum(lSize, (unsigned char*) pBuffer, 0xFFFFFFFF);
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

	sprintf(crcHex, "%08X", crc);

	szOut = (char *)malloc ( sizeof ( char ) * 10 );
	strcpy ( szOut, crcHex );

	return szOut;
}


/**
 * \}
 * \}
 */
