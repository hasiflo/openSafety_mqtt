#ifndef EPLS_TARGET_H
#define EPLS_TARGET_H

#include <stdio.h>   /* include for the printf and sprintf function */
#include <string.h>  /* include for the memcpy and memcmp function */
#include <stdint.h>  /* include for the UINT64 and INT64 */ 

/*******************************************************************************
**    constants DO NOT CHANGE!!!
*******************************************************************************/
/** BIG:
    Set {ENDIAN} constant to BIG, if the low byte is on the high address
    and the high byte is on the low address.
*/
#ifdef BIG
  #undef BIG
#endif
#define BIG    0

/** LITTLE:
    Set {ENDIAN} constant to LITTLE, if the low byte is on the low address
    and the high byte is on the high address.
*/
#ifdef LITTLE
  #undef LITTLE
#endif
#define LITTLE 1
/*******************************************************************************
**    constants can be customized by the user
*******************************************************************************/
/** EPLS_k_TARGET_STRING:
    This symbol represents the target system as a string.
*/
#define EPLS_k_TARGET_STRING "Cortex-M3"

/** SAFE_INIT_SEKTOR:
    This symbol is used to locate an initialized variable in the defined memory
    sector.
    E.g.: UINT8 b_noE_1 SAFE_INIT_SEKTOR = 1;
*/
#ifdef SAFE_INIT_SEKTOR
  #undef SAFE_INIT_SEKTOR
#endif
#define SAFE_INIT_SEKTOR //__attribute__ ((section (".safe_data")))

/** SAFE_NO_INIT_SEKTOR:
    This symbol is used to locate an uninitialized variable in the defined
    memory sector.
    E.g.: UINT8 b_noE_1 SAFE_NO_INIT_SEKTOR;
*/
#ifdef SAFE_NO_INIT_SEKTOR
  #undef SAFE_NO_INIT_SEKTOR
#endif
#define SAFE_NO_INIT_SEKTOR //__attribute__ ((section (".safe_bss")))

/** ENDIAN:
    This define specifies, whether the target is big endian (see {BIG}) or
    little endian (see {LITTLE}).
    allowed values : BIG, LITTLE
*/
#ifdef ENDIAN
  #undef ENDIAN
#endif
#define ENDIAN   LITTLE

/*******************************************************************************
**    data types can be customized by the user
*******************************************************************************/
/** BOOLEAN:
    Standard data type.
*/
#ifdef BOOLEAN
  #undef BOOLEAN
#endif
typedef unsigned char     BOOLEAN;

/** UINT8:
    Standard data type.
*/
#ifdef UINT8
  #undef UINT8
#endif
typedef unsigned char     UINT8;

/** UINT16:
    Standard data type.
*/
#ifdef UINT16
  #undef UINT16
#endif
typedef unsigned short    UINT16;

/** UINT32:
    Standard data type.
*/
#ifdef UINT32
  #undef UINT32
#endif
typedef unsigned int     UINT32;

/** UINT64:
    Standard data type.
*/
#ifdef UINT64
  #undef UINT64
#endif
typedef uint64_t  UINT64;

/** INT8:
    Standard data type.
*/
#ifdef INT8
  #undef INT8
#endif
typedef signed char       INT8;

/** INT16:
    Standard data type.
*/
#ifdef INT16
  #undef INT16
#endif
typedef signed short      INT16;

/** INT32:
    Standard data type.
*/
#ifdef INT32
  #undef INT32
#endif
typedef signed int       INT32;

/** INT64:
    Standard data type.
*/
#ifdef INT64
  #undef INT64
#endif
typedef int64_t         INT64;

/** REAL32:
    Standard data type.
*/
#ifdef REAL32
  #undef REAL32
#endif
typedef float             REAL32;

/** REAL64:
    Standard data type.
*/
#ifdef REAL64
  #undef REAL64
#endif
typedef double            REAL64;

/** CHAR:
    Standard data type.
*/
#ifdef CHAR
  #undef CHAR
#endif
typedef char       CHAR;

/** PTR_TYPE:
    This data type is relevant for the void data pointer of the SOD objects that
    points to an address (pointer to a pointer SOD object). This type is used to
    cast the address.
    E.g.: if the PTR_TYPE is UINT32 then a pointer consist of 4 bytes and the
    address range is 0-0xFFFFFFFF.
    allowed values : UINT8, UINT16, UINT32, UINT64
*/
#define PTR_TYPE      UINT64

/*******************************************************************************
**    constants DO NOT CHANGE!!!
*******************************************************************************/
/** TRUE:
    Standard constant.
*/
#ifdef TRUE
  #undef TRUE
#endif
#define TRUE    ((BOOLEAN)(1==1))

/** FALSE:
    Standard constant.
*/
#ifdef FALSE
  #undef FALSE
#endif
#define FALSE    ((BOOLEAN)(1==0))

/** NULL:
    Standard constant.
*/
#ifdef NULL
  #undef NULL
#endif
#define NULL    ((void *)(0))

/*******************************************************************************
**    macros can be customized by the user
*******************************************************************************/
/** ADD_OFFSET:
    This macro is used to increment the address of the given pointer with
    the given offset.
*/
#ifdef ADD_OFFSET
  #undef ADD_OFFSET
#endif
#define ADD_OFFSET(ptr, offset) (((UINT8 *)(ptr)) + (offset))

/** MEMCOPY:
    This macro is used to call the target specific memcpy function.
*/
#ifdef MEMCOPY
  #undef MEMCOPY
#endif
#define MEMCOPY(dst, src, len) memcpy((void *)(dst), (const void *)(src),\
                                      (size_t)(len))

/** MEMMOVE:
    This macro is used to call the target specific memmove function.
*/
#ifdef MEMMOVE
  #undef MEMMOVE
#endif
#define MEMMOVE(dst, src, len) memmove((void *)(dst), (const void *)(src),\
                                     (size_t)(len))

/** MEMCMP_IDENT:
    This macro is used to check the return value of the MEMCOMP macro.
*/
#define MEMCMP_IDENT  0

/** MEMCOMP:
    This macro is used to call the target specific memcmp function. If the
    return value is MEMCMP_IDENT, then the buf1 is identical to buf2.
*/
#ifdef MEMCOMP
  #undef MEMCOMP
#endif
#define MEMCOMP(buf1, buf2, len) memcmp((const void *)(buf1),\
                                        (const void *)(buf2),\
                                        (size_t)(len))


/** MEMSET:
    This macro is used to call the target specific memset function.
*/
#ifdef MEMSET
  #undef MEMSET
#endif
#define MEMSET(dst, c, count) memset((void *)(dst), (int)(c), (size_t)(count))

/** HIGH8:
    Standard macro. Gets high 8-bit-value of an 16-bit-value dependent on the
    endian of the target. (See {ENDIAN})
*/
#ifdef HIGH8
  #undef HIGH8
#endif
#if(ENDIAN == BIG)
  #define  HIGH8(wert)  (*( (UINT8 *) &(wert)))
#elif(ENDIAN == LITTLE)
  #define  HIGH8(wert)  (*(((UINT8 *) &(wert)) + 1U))
#endif

/** LOW8:
    Standard macro. Gets low 8-bit-value of an 16-bit-value dependent on the
    endian of the target. (See {ENDIAN})
*/
#ifdef LOW8
  #undef LOW8
#endif
#if(ENDIAN == BIG)
  #define  LOW8(wert)  (*(((UINT8 *) &(wert)) + 1U))
#elif(ENDIAN == LITTLE)
  #define  LOW8(wert)  (*( (UINT8 *) &(wert)))
#endif

/** HIGH16:
    Standard macro. Gets high 16-bit-value of an 32-bit-value dependent on the
    endian of the target. (See {ENDIAN})
*/
#ifdef HIGH16
  #undef HIGH16
#endif
#if(ENDIAN == BIG)
  #define  HIGH16(wert)  ((UINT16)((wert)>>16U))
#elif(ENDIAN == LITTLE)
  #define  HIGH16(wert)  ((UINT16)((wert)>>16U))
#endif

/** LOW16:
    Standard macro. Gets low 16-bit-value of an 32-bit-value dependent on the
    endian of the target. (See {ENDIAN})
*/
#ifdef LOW16
  #undef LOW16
#endif
#if(ENDIAN == BIG)
  #define  LOW16(wert)  ((UINT16)(wert))
#elif(ENDIAN == LITTLE)
  #define  LOW16(wert)  ((UINT16)(wert))
#endif

/** DBG_PRINT:
    If this define is defined then the debug screen outputs will be activated,
    otherwise they will be deactivated.
*/
#define DBG_PRINT_ACTIVATED
#undef DBG_PRINT_ACTIVATED

#ifdef DBG_PRINT_ACTIVATED
  /** DBG_PRINTF:
      Macro for debug screen output purposes.
      Parameters

        str
            Debug screen output.
  */
  #define DBG_PRINTF(str)  diag_printf((str))

  /** DBG_PRINTFx:
      Macro for debug screen output purposes with x parameters.
      Parameters

        str
            Debug screen output.
        param1
            1st parameter to be printed.
        paramx
            xth parameter to be printed.

  */
  #define DBG_PRINTF1(str, param1)                  diag_printf((str), (param1))
  #define DBG_PRINTF2(str, param1, param2)          diag_printf((str), (param1), (param2))
  #define DBG_PRINTF3(str, param1, param2, param3)  diag_printf((str), (param1), (param2), (param3))
#else
  /* DBG_PRINTF, and DBG_PRINTFx are deactivated */
  #define DBG_PRINTF(str)  printf((str))
  #define DBG_PRINTF1(str, param1) printf((str), (param1))
  #define DBG_PRINTF2(str, param1, param2) printf((str), (param1), (param2))
  #define DBG_PRINTF3(str, param1, param2, param3) printf((str), (param1), (param2), (param3))
  //#define DBG_PRINT3(str, param1, param2) printf((str), (param1), (param2))
#endif

/** SPRINTF:
    Macro for writing formatted data to a string.
    Parameters

      buffer
          Storage location for output.
      format
          Format-control string.
*/
#define SPRINTF(buffer, format)  sprintf((buffer), (format))

/** SPRINTFx:
    Macro for writing formatted data to a string with x parameters.
    Parameters

      buffer
          Storage location for output.
      format
          Format-control string.
      param1
          1st parameter to be printed.
      paramx
          xth parameter to be printed.

*/
#define SPRINTF1(buffer, format, param1)                                  sprintf((buffer), (format), (param1))
#define SPRINTF2(buffer, format, param1, param2)                          sprintf((buffer), (format), (param1), (param2))
#define SPRINTF3(buffer, format, param1, param2, param3)                  sprintf((buffer), (format), (param1), (param2), (param3))
#define SPRINTF4(buffer, format, param1, param2, param3, param4)          sprintf((buffer), (format), (param1), (param2), (param3), (param4))
#define SPRINTF5(buffer, format, param1, param2, param3, param4, param5)  sprintf((buffer), (format), (param1), (param2), (param3), (param4), (param5))


#endif /* #ifndef EPLS_TARGET_H */
