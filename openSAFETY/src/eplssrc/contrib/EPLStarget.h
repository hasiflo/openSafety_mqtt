/**
 * @addtogroup Target
 * @{
 *
 * @file EPLStarget.h openSAFETY Target specific configuration
 *
 * This file contains the platform specific defines according to the EPLS_k_TARGET_STRING.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 * @author M. Molnar, IXXAT Automation GmbH
 * @author Hans Pill, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 **/

#ifndef EPLS_TARGET_H
#define EPLS_TARGET_H

#include <stdint.h>   /* include for the printf and sprintf function */
#include <stdio.h>
#include <string.h>  /* include for the memcpy and memcmp function */

//#pragma warning (disable : 4068) /* This pragma is used to avoid a compiler
//                                    warning C4068: unknown pragma */


/*******************************************************************************
**    constants DO NOT CHANGE!!!
*******************************************************************************/
/**
 * @name Endianess setting
 * @{
 */

/**
 * Set ENDIAN constant to BIG, if the low byte is on the high address and the high byte is on the low address.
*/
#ifdef BIG
  #undef BIG
#endif
#define BIG    0

/**
 * Set ENDIAN constant to LITTLE, if the low byte is on the low address and the high byte is on the high address.
*/
#ifdef LITTLE
  #undef LITTLE
#endif
#define LITTLE 1

/** @} */

/*******************************************************************************
**    constants can be customized by the user
*******************************************************************************/
/**
 * This symbol represents the target system as a string.
 *
 * @note Must be customized by user
*/
#define EPLS_k_TARGET_STRING "Unit Tests"

/**
 * This symbol is used to locate an initialized variable in the defined memory sector.
 *
 * E.g.: UINT8 b_noE_1 SAFE_INIT_SEKTOR = 1;
*/
#ifdef SAFE_INIT_SEKTOR
  #undef SAFE_INIT_SEKTOR
#endif
#define SAFE_INIT_SEKTOR

/**
 * This symbol is used to locate an uninitialized variable in the defined memory sector.
 *
 * E.g.: UINT8 b_noE_1 SAFE_NO_INIT_SEKTOR;
*/
#ifdef SAFE_NO_INIT_SEKTOR
  #undef SAFE_NO_INIT_SEKTOR
#endif
#define SAFE_NO_INIT_SEKTOR

/**
 * This define specifies, whether the target is big endian (see BIG) or little endian (see LITTLE).
 *
 * Allowed values : BIG, LITTLE
*/
#ifdef ENDIAN
  #undef ENDIAN
#endif
#define ENDIAN   LITTLE

/**
 * Define to set the corresponding pin for the timemeasurement.
*/
#ifdef SET_PIN
  #undef SET_PIN
#endif
#define SET_PIN

/**
 * Define to reset the corresponding pin for the timemeasurement.
*/
#ifdef RESET_PIN
  #undef RESET_PIN
#endif
#define RESET_PIN

/*******************************************************************************
**    data types can be customized by the user
*******************************************************************************/

/**
 * @name Customizable Data Types
 * @{
 *
 * To enable the openSAFETY stack to be truly cross-plattform compatible, data types may
 * be defined to be target specific.
 *
 * @note The openSAFETY stack implementation uses only the data types defined in this header file
 */


#ifdef BOOLEAN
  #undef BOOLEAN
#endif
/**
 * Boolean data type
 */
typedef unsigned char    BOOLEAN;

#ifdef UINT8
  #undef UINT8
#endif
/**
 * Unsigned 8-bit integer
 */
typedef uint8_t     UINT8;

#ifdef UINT16
  #undef UINT16
#endif
/**
 * Unsigned 16-bit integer
 */
typedef uint16_t    UINT16;

#ifdef UINT32
  #undef UINT32
#endif
/**
 * Unsigned 32-bit integer
 */
typedef uint32_t     UINT32;

#ifdef UINT64
  #undef UINT64
#endif
/**
 * Unsigned 64-bit integer
 *
 * @note Implementation and Availability of this type may depend on EPLS_cfg_MAX_PYLD_LEN
 */
typedef uint64_t  UINT64;

#ifdef INT8
  #undef INT8
#endif
/**
 * Signed 8-bit integer
 */
typedef int8_t       INT8;

#ifdef INT16
  #undef INT16
#endif
/**
 * Signed 16-bit integer
 */
typedef int16_t      INT16;

#ifdef INT32
  #undef INT32
#endif
/**
 * Signed 32-bit integer
 */
typedef int32_t       INT32;

#ifdef INT64
  #undef INT64
#endif
/**
 * Signed 64-bit integer
 *
 * @note Implementation and Availability of this type may depend on EPLS_cfg_MAX_PYLD_LEN
 */
typedef int64_t           INT64;

#ifdef REAL32
  #undef REAL32
#endif
/**
 * 32-bit floating point
 */
typedef float            REAL32;

#ifdef REAL64
  #undef REAL64
#endif
/**
 * 64-bit floating point
 *
 * @note Implementation and Availability of this type may depend on EPLS_cfg_MAX_PYLD_LEN
 */
typedef double            REAL64;

#ifdef CHAR
  #undef CHAR
#endif
/**
 * character data type
 */
typedef char       CHAR;

/**
 * This data type is relevant for the void data pointer of the SOD objects that points to an
 * address (pointer to a pointer SOD object).
 *
 * This type is used to cast the address.
 *
 * allowed values : UINT8, UINT16, UINT32, UINT64
 *
 * @note if the PTR_TYPE is UINT32 then a pointer consist of 4 bytes and the address range is 0-0xFFFFFFFF.
 *
*/
#define PTR_TYPE      UINT64

/**
 * Macro to handle unused parameter.
 *
 * Some methods (e.g. SOD_Lock(), SOD_Unlock()) do not use parameters provided to them.
 * For reasons like callbacks, those parameters must exist, and cannot be avoided. Also
 * some compiler options may hide parameters for one implementation, but enable it for
 * another. In such cases, variables may be declared as unused.
 *
 * @note If the parameter will still be used in the function, this operation does not
 * prohibit or change the value of the parameter
 *
 * @attention MSVC2010 will generate a warning, as the value is not used This is
 * accepted, as the target compilers in our case are gcc or similair derivates.
 *
 * Taken from: http://stackoverflow.com/questions/4851075/universally-compiler-independant-way-of-implementing-an-unused-macro-in-c-c/4851173
 */
#define oS_UNUSED_VAR(x) ((void)(sizeof((x), 0)))


/** @} */

/*******************************************************************************
**    constants DO NOT CHANGE!!!
*******************************************************************************/
#ifdef TRUE
  #undef TRUE
#endif
/**
 * Boolean constant for TRUE
 */
#define TRUE    ((BOOLEAN)(1==1))

#ifdef FALSE
  #undef FALSE
#endif
/**
 * Boolean constant for FALSE
 */
#define FALSE    ((BOOLEAN)(1==0))

#ifdef NULL
  #undef NULL
#endif
/**
 * Standard constant for a zero void pointer
 */
#define NULL    ((void *)(0))

/*******************************************************************************
**    macros can be customized by the user
*******************************************************************************/
#ifdef ADD_OFFSET
  #undef ADD_OFFSET
#endif
/**
 * This macro is used to increment the address of the given pointer with the given offset.
*/
#define ADD_OFFSET(ptr, offset) (((UINT8 *)(ptr)) + (offset))
 
#ifdef MEMCOPY
  #undef MEMCOPY
#endif
/** This macro is used to call the target specific memcpy function. */
#define MEMCOPY(dst, src, len) memcpy((void *)(dst), (const void *)(src),\
                                     (size_t)(len))

#ifdef MEMMOVE
  #undef MEMMOVE
#endif
/** This macro is used to call the target specific memmove function. */
#define MEMMOVE(dst, src, len) memmove((void *)(dst), (const void *)(src),\
                                     (size_t)(len))

/** This macro is used to check the return value of the MEMCOMP macro. */
#define MEMCMP_IDENT  0

#ifdef MEMCOMP
  #undef MEMCOMP
#endif
/**
 * This macro is used to call the target specific memcmp function. If the
 * return value is MEMCMP_IDENT, then the buf1 is identical to buf2.
*/
#define MEMCOMP(buf1, buf2, len) memcmp((const void *)(buf1),\
                                       (const void *)(buf2),\
                                       (size_t)(len))

#ifdef MEMSET
  #undef MEMSET
#endif
/**
 * This macro is used to call the target specific memset function.
*/
#define MEMSET(dst, c, count) memset((void *)(dst), (int)(c), (size_t)(count))

/**
 * Gets a high 8-bit-value of an 16-bit-value dependent on the endianess of the target.
 *
 * @see ENDIAN
*/
#ifdef HIGH8
  #undef HIGH8
#endif
#if(ENDIAN == BIG)
  #define  HIGH8(wert)  (*( (UINT8 *) &(wert)))
#elif(ENDIAN == LITTLE)
  #define  HIGH8(wert)  (*(((UINT8 *) &(wert)) + 1U))
#endif

/**
 * Gets a low 8-bit-value of an 16-bit-value dependent on the endianess of the target.
 *
 * @see ENDIAN
*/
#ifdef LOW8
  #undef LOW8
#endif
#if(ENDIAN == BIG)
  #define  LOW8(wert)  (*(((UINT8 *) &(wert)) + 1U))
#elif(ENDIAN == LITTLE)
  #define  LOW8(wert)  (*( (UINT8 *) &(wert)))
#endif

/**
 * Gets a high 16-bit-value of an 32-bit-value dependent on the endianess of the target.
 *
 * @see ENDIAN
*/
#ifdef HIGH16
  #undef HIGH16
#endif
#if(ENDIAN == BIG)
  #define  HIGH16(wert)  ((UINT16)((wert)>>16U))
#elif(ENDIAN == LITTLE)
  #define  HIGH16(wert)  ((UINT16)((wert)>>16U))
#endif

/**
 * Gets a low 16-bit-value of an 32-bit-value dependent on the endianess of the target.
 *
 * @see ENDIAN
*/
#ifdef LOW16
  #undef LOW16
#endif
#if(ENDIAN == BIG)
  #define  LOW16(wert)  ((UINT16)(wert))
#elif(ENDIAN == LITTLE)
  #define  LOW16(wert)  ((UINT16)(wert))
#endif

/**
 * If this define is defined then the debug screen outputs will be activated, otherwise they will be deactivated.
*/
#define DBG_PRINT_ACTIVATED

#ifdef DBG_PRINT_ACTIVATED
  /**
   * Macro for debug screen output purposes.
   *
   * @param str Debug screen output.
  */
  #define DBG_PRINTF(str)  printf((str))
  
  /**
   * Macro for debug screen output purposes with x parameters.
   *
   * @param str Debug screen output.
   * @param param1 1st parameter to be printed.
  */
  #define DBG_PRINTF1(str, param1)                  printf((str), (param1))
/**
 * Macro for debug screen output purposes with x parameters.
 *
 * @param str Debug screen output.
 * @param param1 1st parameter to be printed.
 * @param param2 2nd parameter to be printed.
*/
  #define DBG_PRINTF2(str, param1, param2)          printf((str), (param1), (param2))
/**
 * Macro for debug screen output purposes with x parameters.
 *
 * @param str Debug screen output.
 * @param param1 1st parameter to be printed.
 * @param param2 2nd parameter to be printed.
 * @param param3 3rd parameter to be printed.
*/
  #define DBG_PRINTF3(str, param1, param2, param3)  printf((str), (param1), (param2), (param3))
#else
  /* DBG_PRINTF, and DBG_PRINTFx are deactivated */
  #define DBG_PRINTF(str)
  #define DBG_PRINTF1(str, param1)
  #define DBG_PRINTF2(str, param1, param2)
  #define DBG_PRINTF3(str, param1, param2, param3)
#endif

/**
 * Macro for writing formatted data to a string with x parameters.
 *
 * @param buffer Storage location for output.
 * @param format Format-control string.
*/
#define SPRINTF(buffer, format)  sprintf((buffer), (format))

/**
 * Macro for writing formatted data to a string with x parameters.
 *
 * @param buffer Storage location for output.
 * @param format Format-control string.
 * @param param1 1st parameter to be printed.
*/
#define SPRINTF1(buffer, format, param1)                                  sprintf((buffer), (format), (param1))
/**
 * Macro for writing formatted data to a string with x parameters.
 *
 * @param buffer Storage location for output.
 * @param format Format-control string.
 * @param param1 1st parameter to be printed.
 * @param param2 2nd parameter to be printed.
*/
#define SPRINTF2(buffer, format, param1, param2)                          sprintf((buffer), (format), (param1), (param2))
/**
 * Macro for writing formatted data to a string with x parameters.
 *
 * @param buffer Storage location for output.
 * @param format Format-control string.
 * @param param1 1st parameter to be printed.
 * @param param2 2nd parameter to be printed.
 * @param param3 3rd parameter to be printed.
*/
#define SPRINTF3(buffer, format, param1, param2, param3)                  sprintf((buffer), (format), (param1), (param2), (param3))
/**
 * Macro for writing formatted data to a string with x parameters.
 *
 * @param buffer Storage location for output.
 * @param format Format-control string.
 * @param param1 1st parameter to be printed.
 * @param param2 2nd parameter to be printed.
 * @param param3 3rd parameter to be printed.
 * @param param4 4th parameter to be printed.
*/
#define SPRINTF4(buffer, format, param1, param2, param3, param4)          sprintf((buffer), (format), (param1), (param2), (param3), (param4))
/**
 * Macro for writing formatted data to a string with x parameters.
 *
 * @param buffer Storage location for output.
 * @param format Format-control string.
 * @param param1 1st parameter to be printed.
 * @param param2 2nd parameter to be printed.
 * @param param3 3rd parameter to be printed.
 * @param param4 4th parameter to be printed.
 * @param param5 5th parameter to be printed.
*/
#define SPRINTF5(buffer, format, param1, param2, param3, param4, param5)  sprintf((buffer), (format), (param1), (param2), (param3), (param4), (param5))

#endif /* #ifndef EPLS_TARGET_H */


/** @} */
