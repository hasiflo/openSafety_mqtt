/**
 * @addtogroup SN
 * @{
 * @addtogroup EPLS [EPLS] openSAFETY Support functionality
 * @{
 * @file EPLScfgCheck.h openSAFETY configuration check
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 * @author M. Molnar, IXXAT Automation GmbH
 * @author Hans Pill, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * @details
 * EPLScheck defines macros to check the configuration carried out by the openSAFETY Stack user.
 * The configuration is checked against internal limits, also defined as symbolic constants.
 *
 * <h2>History for EPLScfgCheck.h</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>03.06.2009</td><td>Hans Pill</td><td>Review SL V10</td></tr>
 *     <tr><td>01.04.2011</td><td>Hans Pill</td><td>A&P257950 added check for number of SPDOs in the SDG instances</td></tr>
 *     <tr><td>05.10.2011</td><td>Hans Pill</td><td>Review SL V22</td></tr>
 *     <tr><td>15.03.2013</td><td>Hans Pill</td><td>EPLS_cfg_ERROR_STATISTIC must be checked</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 *     <tr><td>02.10.2014</td><td>Hans Pill</td><td>added checks for 40 bit CT in SPDOs</td></tr>
 *     <tr><td>30.01.2017</td><td>Roman Zwischelsberger</td><td>added checks for SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC</td></tr>
 *     <tr><td>23.08.2017</td><td>Alexander Brunner</td><td>fixed multiple inclusion protection</td></tr>
 * </table>
 *
 */

#ifndef EPLS_CFG_CHECK_H
#define EPLS_CFG_CHECK_H

/**
 * Define to check if the EPLS software configuration was checked and avoid
 * PC-lint warning : error 766: (Info -- Header file not used in module)
 */
#define EPLS_SW_CONFIG_CHECKED

/***
*    Checking of the constant(s) (see EPLStypes.h)
***/
/* EPLS_k_MAX_SADR + 1 is k_LOOKUP_TABLE_SIZE in the SPDOassign.c. It must be
   1024 to be divisable by 4. */
#if (EPLS_k_MAX_SADR + 1 != 1024)
  #error EPLS_k_MAX_SADR is invalid
#endif

/***
*    Checking of the target configuration (see EPLStarget.h)
***/
#ifndef SAFE_INIT_SEKTOR
  #error SAFE_INIT_SEKTOR is not defined
#endif

#ifndef SAFE_NO_INIT_SEKTOR
  #error SAFE_NO_INIT_SEKTOR is not defined
#endif

#ifndef ENDIAN
  #error ENDIAN is not defined
#endif
#if ((ENDIAN != LITTLE) && (ENDIAN != BIG))
  #error ENDIAN is invalid
#endif

#ifndef TRUE
  #error TRUE is not defined
#endif

#ifndef FALSE
  #error FALSE is not defined
#endif

#ifndef NULL
  #error NULL is not defined
#endif

#ifndef BIG
  #error BIG is not defined
#endif
#ifndef LITTLE
  #error LITTLE is not defined
#endif
#if (BIG == LITTLE)
  #error BIG/LITTLE is invalid
#endif

#ifndef ADD_OFFSET
  #error ADD_OFFSET is not defined
#endif

#ifndef MEMCOPY
  #error MEMCOPY is not defined
#endif

#ifndef MEMCMP_IDENT
  #error MEMCMP_IDENT is not defined
#endif

#ifndef MEMCOMP
  #error MEMCOMP is not defined
#endif

#ifndef MEMSET
  #error MEMSET is not defined
#endif

#ifndef HIGH8
  #error HIGH8 is not defined
#endif

#ifndef LOW8
  #error LOW8 is not defined
#endif

#ifndef HIGH16
  #error HIGH16 is not defined
#endif

#ifndef LOW16
  #error LOW16 is not defined
#endif

/***
*    Checking of the global configuration (see EPLScfg.h)
***/
#ifndef EPLS_k_ENABLE
  #error EPLS_k_ENABLE is not defined
#endif
#ifndef EPLS_k_DISABLE
  #error EPLS_k_DISABLE is not defined
#endif
#if (EPLS_k_DISABLE != 0)
  #error EPLS_k_DISABLE is invalid
#endif
#if (EPLS_k_ENABLE != 1)
  #error EPLS_k_ENABLE is invalid
#endif

#ifndef EPLS_k_NOT_APPLICABLE
  #error EPLS_k_NOT_APPLICABLE is not defined
#endif
#if (EPLS_k_NOT_APPLICABLE != 2000)
  #error EPLS_k_NOT_APPLICABLE is invalid
#endif


#ifndef EPLS_cfg_MAX_INSTANCES
  #error EPLS_cfg_MAX_INSTANCES is not defined
#endif
#if ((EPLS_cfg_MAX_INSTANCES < 1) || \
     (EPLS_cfg_MAX_INSTANCES > 255))
  #error EPLS_cfg_MAX_INSTANCES is invalid
#endif

#if ((EPLS_cfg_MAX_INSTANCES == 1) && \
	 ((SPDO_cfg_MAX_NO_RX_SPDO_SDG != 0) || \
	  (SPDO_cfg_MAX_NO_TX_SPDO_SDG != 0)))
  #error number of SDG SPDOs is not correct, must be set to zero
#endif

#if ((EPLS_cfg_MAX_INSTANCES > 1) && \
	 ((SPDO_cfg_MAX_NO_RX_SPDO_SDG < 1) || \
	  (SPDO_cfg_MAX_NO_TX_SPDO_SDG < 1)))
  #error number of SDG SPDOs is not correct, must be set to >= 1
#endif

#ifndef EPLS_cfg_ERROR_STRING
  #error EPLS_cfg_ERROR_STRING is not defined
#endif
#if ((EPLS_cfg_ERROR_STRING != EPLS_k_DISABLE) && \
     (EPLS_cfg_ERROR_STRING != EPLS_k_ENABLE))
  #error EPLS_cfg_ERROR_STRING is invalid
#endif

#ifndef EPLS_cfg_ERROR_STATISTIC
  #error EPLS_cfg_ERROR_STATISTIC is not defined
#endif
#if ((EPLS_cfg_ERROR_STATISTIC != EPLS_k_DISABLE) && \
     (EPLS_cfg_ERROR_STATISTIC != EPLS_k_ENABLE))
  #error EPLS_cfg_ERROR_STATISTIC is invalid
#endif

#ifndef EPLS_cfg_SCM
  #error EPLS_cfg_SCM is not defined
#endif
#if ((EPLS_cfg_SCM != EPLS_k_DISABLE) && \
     (EPLS_cfg_SCM != EPLS_k_ENABLE))
  #error EPLS_cfg_SCM is invalid
#endif

#ifndef EPLS_cfg_MAX_PYLD_LEN
  #error EPLS_cfg_MAX_PYLD_LEN is not defined
#endif
#if ((EPLS_cfg_MAX_PYLD_LEN < 8) || \
     (EPLS_cfg_MAX_PYLD_LEN > 254))
  #error EPLS_cfg_MAX_PYLD_LEN is invalid
#endif



/***
*    Checking of the SCM configuration (see EPLScfg.h)
***/
#ifndef SCM_cfg_MAX_NUM_OF_NODES
  #error SCM_cfg_MAX_NUM_OF_NODES is not defined
#endif

#ifndef SCM_cfg_NUM_OF_PROCESSED_NODES_PER_CALL
  #error SCM_cfg_NUM_OF_PROCESSED_NODES_PER_CALL is not defined
#endif

#ifndef SCM_cfg_MAX_SADR_VALUE
  #error SCM_cfg_MAX_SADR_VALUE is not defined
#endif


#if (EPLS_cfg_SCM  == EPLS_k_ENABLE)
  #if ((SCM_cfg_MAX_NUM_OF_NODES < 2) || \
       (SCM_cfg_MAX_NUM_OF_NODES > 1023))
    #error SCM_cfg_MAX_NUM_OF_NODES is invalid
  #endif

  #if ((SCM_cfg_NUM_OF_PROCESSED_NODES_PER_CALL < 1) || \
       (SCM_cfg_NUM_OF_PROCESSED_NODES_PER_CALL > 1023))
    #error SCM_cfg_NUM_OF_PROCESSED_NODES_PER_CALL is invalid
  #endif

  #if ((SCM_cfg_MAX_SADR_VALUE < 2) || \
       (SCM_cfg_MAX_SADR_VALUE > 1023))
    #error SCM_cfg_MAX_SADR_VALUE is invalid
  #endif
#else
  #if (SCM_cfg_MAX_NUM_OF_NODES != EPLS_k_NOT_APPLICABLE)
    #error SCM_cfg_MAX_NUM_OF_NODES is not EPLS_k_NOT_APPLICABLE
  #endif

  #if (SCM_cfg_NUM_OF_PROCESSED_NODES_PER_CALL != EPLS_k_NOT_APPLICABLE)
    #error SCM_cfg_NUM_OF_PROCESSED_NODES_PER_CALL is not EPLS_k_NOT_APPLICABLE
  #endif

  #if (SCM_cfg_MAX_SADR_VALUE != EPLS_k_NOT_APPLICABLE)
    #error SCM_cfg_MAX_SADR_VALUE is not EPLS_k_NOT_APPLICABLE
  #endif
#endif



/***
*    Checking of the SPDO configuration (see EPLScfg.h)
***/
#ifndef SPDO_cfg_40_BIT_CT_SUPPORT
  #error SPDO_cfg_40_BIT_CT_SUPPORT is not defined
#endif
#if ((SPDO_cfg_40_BIT_CT_SUPPORT != EPLS_k_ENABLE) && \
     (SPDO_cfg_40_BIT_CT_SUPPORT != EPLS_k_DISABLE))
  #error SPDO_cfg_40_BIT_CT_SUPPORT is invalid
#endif

#ifndef SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC
  #define SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC EPLS_k_DISABLE
#endif
#if ((SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC != EPLS_k_ENABLE) && \
     (SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC != EPLS_k_DISABLE))
  #error SPDO_cfg_IMMEDIATE_DATA_ONLY_AFTER_TSYNC is invalid
#endif

#ifndef SPDO_cfg_FRAME_CPY_INTERN
  #error SPDO_cfg_FRAME_CPY_INTERN is not defined
#endif
#if ((SPDO_cfg_FRAME_CPY_INTERN != EPLS_k_ENABLE) && \
     (SPDO_cfg_FRAME_CPY_INTERN != EPLS_k_DISABLE))
  #error SPDO_cfg_FRAME_CPY_INTERN is invalid
#endif

#ifndef SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE
  #error SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE is not defined
#endif
#if ((SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE != EPLS_k_ENABLE) && \
     (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE != EPLS_k_DISABLE))
  #error SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE is invalid
#endif

#ifndef SPDO_cfg_MAX_NO_TX_SPDO
  #error SPDO_cfg_MAX_NO_TX_SPDO is not defined
#endif
#if ((SPDO_cfg_MAX_NO_TX_SPDO < 0) || \
     (SPDO_cfg_MAX_NO_TX_SPDO > 1023))
  #error SPDO_cfg_MAX_NO_TX_SPDO is invalid
#endif

#ifndef SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL
  #error SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL is not defined
#endif
#if ((SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL < 0) || \
     (SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL > SPDO_cfg_MAX_NO_TX_SPDO))
  #error SPDO_cfg_NO_TX_SPDO_ACT_PER_CALL is invalid
#endif

#ifndef SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES
  #error SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES is not defined
#endif
#if ((SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES < 0) || \
     (SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES > 253))
  #error SPDO_cfg_MAX_TX_SPDO_MAPP_ENTRIES is invalid
#endif

#ifndef SPDO_cfg_MAX_LEN_OF_TX_SPDO
  #error SPDO_cfg_MAX_LEN_OF_TX_SPDO is not defined
#endif
#if ((SPDO_cfg_MAX_LEN_OF_TX_SPDO < 0) || \
     (SPDO_cfg_MAX_LEN_OF_TX_SPDO > 254))
  #error SPDO_cfg_MAX_LEN_OF_TX_SPDO is invalid
#endif

#ifndef SPDO_cfg_MAX_NO_RX_SPDO
  #error SPDO_cfg_MAX_NO_RX_SPDO is not defined
#endif

#ifndef SPDO_cfg_MAX_SYNC_RX_SPDO
  #error SPDO_cfg_MAX_SYNC_RX_SPDO is not defined
#endif

#ifndef SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL
  #error SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL is not defined
#endif

#ifndef SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES
  #error SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES is not defined
#endif

#ifndef SPDO_cfg_MAX_LEN_OF_RX_SPDO
  #error SPDO_cfg_MAX_LEN_OF_RX_SPDO is not defined
#endif

#ifndef SPDO_cfg_NO_NOT_ANSWERED_TR
  #error SPDO_cfg_NO_NOT_ANSWERED_TR is not defined
#endif

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  #if ((SPDO_cfg_MAX_NO_RX_SPDO < 0) || \
       (SPDO_cfg_MAX_NO_RX_SPDO > 1023))
    #error SPDO_cfg_MAX_NO_RX_SPDO is invalid
  #endif

  #if ((SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL < 0) || \
       (SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL > SPDO_cfg_MAX_NO_RX_SPDO))
    #error SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL is invalid
  #endif

  #if ((SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES < 1) || \
       (SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES > 253))
    #error SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES is invalid
  #endif

  #if ((SPDO_cfg_MAX_LEN_OF_RX_SPDO < 1) || \
       (SPDO_cfg_MAX_LEN_OF_RX_SPDO > 254))
    #error SPDO_cfg_MAX_LEN_OF_RX_SPDO is invalid
  #endif

  #if ((SPDO_cfg_NO_NOT_ANSWERED_TR < 1) || \
       (SPDO_cfg_NO_NOT_ANSWERED_TR > 65535))
    #error SPDO_cfg_NO_NOT_ANSWERED_TR is invalid
  #endif

  #if ((SPDO_cfg_CONNECTION_VALID_STATISTIC != EPLS_k_DISABLE) && \
       (SPDO_cfg_CONNECTION_VALID_BIT_FIELD != EPLS_k_ENABLE))
    #error SPDO_cfg_CONNECTION_VALID_BIT_FIELD is not defined
  #endif

  #if ((SPDO_cfg_EXTENDED_CT_BIT_FIELD != EPLS_k_DISABLE) && \
       (SPDO_cfg_EXTENDED_CT_BIT_FIELD != EPLS_k_ENABLE))
    #error SPDO_cfg_EXTENDED_CT_BIT_FIELD is not defined
  #endif

#else
  #if (SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL != EPLS_k_NOT_APPLICABLE)
    #error SPDO_cfg_NO_RX_SPDO_ACT_PER_CALL is not EPLS_k_NOT_APPLICABLE
  #endif

  #if (SPDO_cfg_CONNECTION_VALID_BIT_FIELD != EPLS_k_DISABLE)
    #error SPDO_cfg_CONNECTION_VALID_BIT_FIELD is not disabled
  #endif

  #if (SPDO_cfg_EXTENDED_CT_BIT_FIELD != EPLS_k_DISABLE)
    #error SPDO_cfg_EXTENDED_CT_BIT_FIELD is not disabled
  #endif

  #if (SPDO_cfg_CONNECTION_VALID_STATISTIC != EPLS_k_DISABLE)
    #error SPDO_cfg_CONNECTION_VALID_STATISTIC is not disabled
  #endif

  #if (SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES != EPLS_k_NOT_APPLICABLE)
    #error SPDO_cfg_MAX_RX_SPDO_MAPP_ENTRIES is not EPLS_k_NOT_APPLICABLE
  #endif

  #if (SPDO_cfg_MAX_LEN_OF_RX_SPDO != EPLS_k_NOT_APPLICABLE)
    #error SPDO_cfg_MAX_LEN_OF_RX_SPDO is not EPLS_k_NOT_APPLICABLE
  #endif

  #if (SPDO_cfg_NO_NOT_ANSWERED_TR != EPLS_k_NOT_APPLICABLE)
    #error SPDO_cfg_NO_NOT_ANSWERED_TR is not EPLS_k_NOT_APPLICABLE
  #endif
#endif



/***
*    Checking of the SOD configuration (see EPLScfg.h)
***/
#ifndef SOD_cfg_APPLICATION_OBJ
  #error SOD_cfg_APPLICATION_OBJ is not defined
#endif
#if ((SOD_cfg_APPLICATION_OBJ != EPLS_k_ENABLE) && \
     (SOD_cfg_APPLICATION_OBJ != EPLS_k_DISABLE))
  #error SOD_cfg_APPLICATION_OBJ is invalid
#endif


#endif

/** @} */
/** @} */
