/*
 * \file UDP_SN/SAPL/SAPLsod.c
 *
 * *****************************************************************************
** Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
**                      IXXAT Automation GmbH
** All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
**
** This source code is free software; you can redistribute it and/or modify it
** under the terms of the BSD license (according to License.txt).
********************************************************************************
**
**   Workfile: SAPLsod.c
**    Summary: SAPLsod - Safety object dictionary for the safety application
**             SAPL, safety object dictionary for the safety application.
**             This file contains the EPLsafety object dictionary.
**
**     Author: M. Molnar
**
********************************************************************************
********************************************************************************
**
**  Functions:
**
**    Remarks:
**
*******************************************************************************/

/*******************************************************************************
**    compiler directives
*******************************************************************************/

/*******************************************************************************
**    include-files
*******************************************************************************/
#include "../include/UDP.h"
#include "SODapi.h"

#include "SPDOapi.h"

#include "SAPL.h"
#include "SAPLsodCfgSpdo.h"

#include "datalogger.h"

/*******************************************************************************
**    global variables
*******************************************************************************/

/*******************************************************************************
**    static constants, types, macros, variables
*******************************************************************************/

/** \cond SOD_INTERFACE */

/** k_RAM:
    This symbol is used to map the SOD into RAM.
*/
#define k_RAM

/** SOD_cfg_TO_RAM_ROM:
    This symbol defines that the SOD is mapped into RAM or ROM
    allowed values : k_ROM, k_RAM
*/
#define SOD_cfg_TO_RAM_ROM k_RAM

/* Defines to increase the readability of the SOD */
#define CONST SOD_k_ATTR_RO_CONST // constant object
#define RO    SOD_k_ATTR_RO       // read only object
#define RW    SOD_k_ATTR_RW       // readable and writable object
#define CRC   SOD_k_ATTR_CRC      // CRC object
#define PDO   SOD_k_ATTR_PDO_MAP  // mappable object

/* Defines to increase the readability of the SOD */
#ifndef _lint /* not referenced */
  #define BEF_RD SOD_k_ATTR_BEF_RD  // Before read callback function is called for the object
#endif
#define BEF_WR SOD_k_ATTR_BEF_WR  // Before write callback function is called for the object
#define AFT_WR SOD_k_ATTR_AFT_WR  // After write callback function is called for the object

/* Defines to increase the readability of the SOD */
#define U8  EPLS_k_UINT8        // unsigned 8-bit object
#define U16 EPLS_k_UINT16       // unsigned 16-bit object
#define U32 EPLS_k_UINT32       // unsigned 32-bit object
#define I8  EPLS_k_INT8         // signed 8-bit object
#define OCT EPLS_k_OCTET_STRING // octet string object
#define DOM EPLS_k_DOMAIN       // domain object


/* Variable definition for the number of entries in the object dictionary.
   _noE_x : number of entries = x */
static UINT8 b_noE_2  SAFE_INIT_SEKTOR =  2U; // number of entries = 2
static UINT8 b_noE_3  SAFE_INIT_SEKTOR =  3U; // number of entries = 3
static UINT8 b_noE_7  SAFE_INIT_SEKTOR =  7U; // number of entries = 7
static UINT8 b_noE_12 SAFE_INIT_SEKTOR = 12U; // number of entries = 12

/* Variable definition to set the range in the object dictionary.
   _rg_min_max  : range checking structure (range : min <= data <= max) */
/* SAFE_INIT_SEKTOR can not be used for arrays */
static const INT8  ac_rg_0_3[2]   = {0,     3};
static const UINT8 ab_rg_1_63[2]  = {1U,  63U};
static const UINT8 ab_rg_0_253[2] = {0U, 253U};
static const UINT8 ab_rg_1_255[2] = {1U, 255U};

static const UINT16 aw_rg_0_1023[2]  = {0U,  1023U};
static const UINT16 aw_rg_1_1023[2]  = {1U,  1023U};
static const UINT16 aw_rg_1_32767[2] = {1U, 32767U};
static const UINT16 aw_rg_0_65535[2] = {0U, 65535U};
static const UINT16 aw_rg_1_65535[2] = {1U, 65535U};


#define PROP_DELAY_RANGE aw_rg_0_65535[0]

/* expected time stamp */
#define k_EXP_TIME_STAMP 0x8899AABBUL

/*******************************************************************************
**
** Default value and actual value variable definition for the object
** dictionary of the instance 0
**
** _0_    : instance 0
** _def_  : default value
** _act_  : actual value
** _xxxx_ : xxxx [16 bit] index
** _xx_   : xx [8 bit] sub-index
**
*******************************************************************************/
/* 0x1001 Error Register */
static UINT8 b_0_act_1001_00 SAFE_NO_INIT_SEKTOR;

/* 0x100C Life Guarding */
static UINT32 dw_0_act_100C_01 SAFE_NO_INIT_SEKTOR; // Guard time
static UINT32 dw_0_def_100C_01 SAFE_INIT_SEKTOR = 1000U; //original 1000
static UINT8   b_0_act_100C_02 SAFE_NO_INIT_SEKTOR; // LifeTimeFactor
static UINT8   b_0_def_100C_02 SAFE_INIT_SEKTOR = 5U;

/* 0x100D Pre-Operational signal - e.g. Timeout f. SN reset guarding SCM*/
static UINT32 dw_0_act_100D_00               SAFE_NO_INIT_SEKTOR;
static UINT32 dw_0_def_100D_00 SAFE_INIT_SEKTOR = 1000UL; //original 10000
static UINT8 b_0_act_100E_00               SAFE_NO_INIT_SEKTOR;
static UINT8 b_0_def_100E_00   SAFE_INIT_SEKTOR = 5;

/* 0x1018 Device Vendor Information */
static UINT32 dw_0_act_1018_01 SAFE_NO_INIT_SEKTOR; // Vendor ID
static UINT32 dw_0_def_1018_01 SAFE_INIT_SEKTOR = 0x12345678UL;
static UINT32 dw_0_act_1018_02 SAFE_NO_INIT_SEKTOR; // Product Code
static UINT32 dw_0_def_1018_02 SAFE_INIT_SEKTOR = 0x11223344UL;
static UINT32 dw_0_act_1018_03 SAFE_NO_INIT_SEKTOR; // Revision Number
static UINT32 dw_0_def_1018_03 SAFE_INIT_SEKTOR = 0xAABBCCDDUL;
static UINT32 dw_0_act_1018_04 SAFE_NO_INIT_SEKTOR; // Serial Number
static UINT32 dw_0_def_1018_04 SAFE_INIT_SEKTOR = 0x5A5A5A5AUL;
static UINT32 dw_0_act_1018_05 SAFE_NO_INIT_SEKTOR; // Firmware Checksum
static UINT32 dw_0_def_1018_05 SAFE_INIT_SEKTOR = 0x5A5A5A5AUL;
// Begin of the parameter checksum definition
// length of the parameter checksum domain
#define k_LEN_PARAM_CHKSUM_DOM    sizeof(SAPL_t_PARAM_CHKSUM_DOM)

static SAPL_t_PARAM_CHKSUM_DOM s_0_act_dom_1018_06 SAFE_NO_INIT_SEKTOR;
static SOD_t_ACT_LEN_PTR_DATA s_0_act_1018_06 = { k_LEN_PARAM_CHKSUM_DOM,
                                                  &s_0_act_dom_1018_06};
static SAPL_t_PARAM_CHKSUM_DOM s_0_def_1018_06 SAFE_INIT_SEKTOR = { 0UL,
                                                                    0UL };
// End of the parameter checksum definition
static UINT32 dw_0_act_1018_07 SAFE_NO_INIT_SEKTOR; // Parameter Timestamp
#if (SAPL_cfg_PARAM_DOWNLOAD == EPLS_k_DISABLE)
static UINT32 dw_0_def_1018_07 SAFE_INIT_SEKTOR = k_EXP_TIME_STAMP;
#else
static UINT32 dw_0_def_1018_07 SAFE_INIT_SEKTOR = k_EXP_TIME_STAMP+1U;
#endif

/* 0x1019 Unique Device ID */
static UINT8 ab_0_act_1019_00[EPLS_k_UDID_LEN] SAFE_NO_INIT_SEKTOR;
static UINT8 ab_0_def_1019_00[EPLS_k_UDID_LEN] SAFE_INIT_SEKTOR = SAPL_k_SN2_UDID;
static SOD_t_ACT_LEN_PTR_DATA s_0_act_1019_00 SAFE_INIT_SEKTOR = { EPLS_k_UDID_LEN,
                                                                   ab_0_act_1019_00};

/* 0x101A Parameter download */
static UINT8 ab_0_act_101A_00[SAPL_k_MAX_PARAM_SET_LEN] SAFE_NO_INIT_SEKTOR;
static SOD_t_ACT_LEN_PTR_DATA s_0_act_101A_00 SAFE_INIT_SEKTOR = { 0U,              /* actual length */
                                                                   ab_0_act_101A_00 /* pointer to the object data */ };


/* 0x1200 Common Communication Parameter */
static UINT16 w_0_act_1200_01 SAFE_NO_INIT_SEKTOR;  // SDN
static UINT16 w_0_def_1200_01 SAFE_INIT_SEKTOR = 1U;
static UINT16 w_0_act_1200_02 SAFE_NO_INIT_SEKTOR;  // SCM SADR
static UINT16 w_0_def_1200_02 SAFE_INIT_SEKTOR = SAPL_k_SCM_SN1_SADR;
static INT8   c_0_act_1200_03 SAFE_NO_INIT_SEKTOR;  // Consecutive time base
static INT8   c_0_def_1200_03 SAFE_INIT_SEKTOR = 2; // 0 : 1us
                                                    // 1 : 10us
                                                    // 2 : 100us
                                                    // 3 : 1000us

static UINT8 ab_0_act_1200_04[EPLS_k_UDID_LEN] SAFE_NO_INIT_SEKTOR;
static UINT8 ab_0_def_1200_04[EPLS_k_UDID_LEN] SAFE_INIT_SEKTOR = SAPL_k_SN2_UDID;
static SOD_t_ACT_LEN_PTR_DATA s_0_act_1200_04 SAFE_INIT_SEKTOR = { EPLS_k_UDID_LEN,
                                                                   ab_0_act_1200_04};

/* 0x1400-0x17FE RxSPDO Communication Parameter */
SAPL_k_SPDO_RX_COMM_PARA_DEF;

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  /* 0x1800-0x1BFE RxSPDO Mapping Parameter */
  SAPL_k_SPDO_RX_MAPP_PARA_DEF;
#endif
/* 0x1C00-0x1FFE TxSPDO Communication Parameter */
SAPL_k_SPDO_TX_COMM_PARA_DEF;

/* 0xC000-0xC3FE TxSPDO Mapping Parameter */
SAPL_k_SPDO_TX_MAPP_PARA_DEF;

/* 0x6000 SPDO mapped objects */
#define k_DATA_BYTES 2
static UINT8 ab_0_act_ProcessData[k_DATA_BYTES] SAFE_NO_INIT_SEKTOR;
static UINT8 ab_0_def_ProcessData[k_DATA_BYTES] SAFE_INIT_SEKTOR = {1U,1U};

/* 0x2000 Manufactorer specific CRC objects */
#define k_CRC_OBJECTS_LENGTH 2
static UINT8 ab_0_act_2000[k_CRC_OBJECTS_LENGTH] SAFE_NO_INIT_SEKTOR;


/** SAPL_s_OD_INST_0:
    Object dictionary for intance 0. E.g. for one object entry:
    see {SOD_t_OBJECT}
    1   : Object index
    2   : Object sub-index
    3-6 : see {SOD_t_ATTR}
      3 : attributes, see {SOD-Attributes}
      4 : datatype of the object
      5 : maximum length of the object
      6 : reference to the default value
    7   : reference to object data
    8   : reference to min-/max values
    9   : reference to a callback function
   |  1   |  2  |        3    | 4  |  5   |      6          |         7         |        8      |         9               |
   {0x1400, 0x01, {RW | BEF_WR, U16, 0x2UL, &w_0_def_1400_01},  &w_0_act_1400_01, &s_rg_w_0_1023, SPDO_RxCommPara_SOD_CLBK},
*/
SOD_cfg_TO_RAM_ROM SOD_t_OBJECT SAPL_s_OD_INST_0[]=
{
  /* Error Register */
  {0x1001U, 0x00U, {RO, U8, 0x1UL, NULL}, &b_0_act_1001_00, NULL, SOD_k_NO_CALLBACK},

  /* Life Guarding */
  {0x100CU, 0x00U, {CONST, U8 , 0x1UL, &b_noE_2         }, &b_noE_2         , NULL       , SOD_k_NO_CALLBACK},
  {0x100CU, 0x01U, {RW   , U32, 0x4UL, &dw_0_def_100C_01}, &dw_0_act_100C_01, NULL       , SOD_k_NO_CALLBACK},
  {0x100CU, 0x02U, {RW   , U8 , 0x1UL,  &b_0_def_100C_02}, &b_0_act_100C_02 , ab_rg_1_255, SOD_k_NO_CALLBACK},

  /* Pre-Operational signal */
  {0x100DU, 0x00U, {RW, U32, 0x4UL, &dw_0_def_100D_00}, &dw_0_act_100D_00, NULL, SOD_k_NO_CALLBACK},
  {0x100E, 0x00, {RW , U8 , 0x1UL, &b_0_def_100E_00 }, &b_0_act_100E_00 , NULL , SOD_k_NO_CALLBACK},

  /* Device Vendor Information */
  {0x1018U, 0x00U, {CONST, U8 ,                  0x1UL, &b_noE_7         }, &b_noE_7         , NULL, SOD_k_NO_CALLBACK},
  {0x1018U, 0x01U, {RO   , U32,                  0x4UL, &dw_0_def_1018_01}, &dw_0_act_1018_01, NULL, SOD_k_NO_CALLBACK},
  {0x1018U, 0x02U, {RO   , U32,                  0x4UL, &dw_0_def_1018_02}, &dw_0_act_1018_02, NULL, SOD_k_NO_CALLBACK},
  {0x1018U, 0x03U, {RO   , U32,                  0x4UL, &dw_0_def_1018_03}, &dw_0_act_1018_03, NULL, SOD_k_NO_CALLBACK},
  {0x1018U, 0x04U, {RO   , U32,                  0x4UL, &dw_0_def_1018_04}, &dw_0_act_1018_04, NULL, SOD_k_NO_CALLBACK},
  {0x1018U, 0x05U, {RO   , U32,                  0x4UL, &dw_0_def_1018_05}, &dw_0_act_1018_05, NULL, SOD_k_NO_CALLBACK},
  {0x1018U, 0x06U, {RW   , DOM, k_LEN_PARAM_CHKSUM_DOM,  &s_0_def_1018_06},  &s_0_act_1018_06, NULL, SOD_k_NO_CALLBACK},
  {0x1018U, 0x07U, {RO   , U32,                  0x4UL, &dw_0_def_1018_07}, &dw_0_act_1018_07, NULL, SOD_k_NO_CALLBACK},

  /* Unique Device ID */
  {0x1019U, 0x00U, {CONST, OCT, EPLS_k_UDID_LEN, ab_0_def_1019_00}, &s_0_act_1019_00, NULL, SOD_k_NO_CALLBACK},

  /* Parameter download */
  {0x101AU, 0x00U, {RW | AFT_WR, DOM, SAPL_k_MAX_PARAM_SET_LEN, NULL}, &s_0_act_101A_00, NULL, SAPL_SOD_ParameterSet_CLBK},

  /* Common Communication Parameter */
  {0x1200U, 0x00U, {CONST,U8 , 0x1UL     , &b_noE_3        }, &b_noE_3        , NULL        , SOD_k_NO_CALLBACK},
  {0x1200U, 0x01U, {RO   ,U16, 0x2UL     , &w_0_def_1200_01}, &w_0_act_1200_01, aw_rg_0_1023, SOD_k_NO_CALLBACK},
  {0x1200U, 0x02U, {RO   ,U16, 0x2UL     , &w_0_def_1200_02}, &w_0_act_1200_02, aw_rg_1_1023, SOD_k_NO_CALLBACK},
  {0x1200U, 0x03U, {RW   ,I8 , 0x1UL     , &c_0_def_1200_03}, &c_0_act_1200_03, ac_rg_0_3   , SOD_k_NO_CALLBACK},
  {0x1200U, 0x04U, {RW   ,OCT, EPLS_k_UDID_LEN, ab_0_def_1200_04}, &s_0_act_1200_04, NULL        , SOD_k_NO_CALLBACK},

  /* 0x1400-0x17FE RxSPDO Communication Parameter */
  {0x1400U, 0x00U, {CONST, U8 , 0x1UL, &b_noE_12        }, &b_noE_12        , NULL             , SOD_k_NO_CALLBACK},
  {0x1400U, 0x01U, {RW   , U16, 0x2UL,  &w_0_def_1400_01},  &w_0_act_1400_01, aw_rg_0_1023     , SOD_k_NO_CALLBACK},
  {0x1400U, 0x02U, {RW   , U32, 0x4UL,  &w_0_def_1400_02},  &w_0_act_1400_02, NULL			    , SOD_k_NO_CALLBACK},
  {0x1400U, 0x03U, {RW   , U8 , 0x1UL,  &b_0_def_1400_03},  &b_0_act_1400_03, ab_rg_1_63       , SOD_k_NO_CALLBACK},
  {0x1400U, 0x04U, {RW   , U32, 0x4UL, &dw_0_def_1400_04}, &dw_0_act_1400_04, NULL             , SOD_k_NO_CALLBACK},
  {0x1400U, 0x05U, {RW   , U32, 0x4UL, &dw_0_def_1400_05}, &dw_0_act_1400_05, NULL             , SOD_k_NO_CALLBACK},
  {0x1400U, 0x06U, {RW   , U16, 0x2UL,  &w_0_def_1400_06},  &w_0_act_1400_06, &PROP_DELAY_RANGE, SOD_k_NO_CALLBACK},
  {0x1400U, 0x07U, {RW   , U16, 0x2UL,  &w_0_def_1400_07},  &w_0_act_1400_07, aw_rg_1_65535    , SOD_k_NO_CALLBACK},
  {0x1400U, 0x08U, {RW   , U16, 0x2UL,  &w_0_def_1400_08},  &w_0_act_1400_08, &PROP_DELAY_RANGE, SOD_k_NO_CALLBACK},
  {0x1400U, 0x09U, {RW   , U16, 0x2UL,  &w_0_def_1400_09},  &w_0_act_1400_09, aw_rg_1_65535    , SOD_k_NO_CALLBACK},
  {0x1400U, 0x0AU, {RW   , U16, 0x2UL,  &w_0_def_1400_0A},  &w_0_act_1400_0A, aw_rg_0_65535    , SOD_k_NO_CALLBACK},
  {0x1400U, 0x0BU, {RW   , U32, 0x4UL, &dw_0_def_1400_0B}, &dw_0_act_1400_0B, NULL             , SOD_k_NO_CALLBACK},
  {0x1400U, 0x0CU, {RW   , U16, 0x2UL,  &w_0_def_1400_0C},  &w_0_act_1400_0C, aw_rg_1_1023     , SOD_k_NO_CALLBACK},

  /* 0x1400-0x17FE RxSPDO Communication Parameter */
  {0x1401U, 0x00U, {CONST, U8 , 0x1UL, &b_noE_12        }, &b_noE_12        , NULL             , SOD_k_NO_CALLBACK},
  {0x1401U, 0x01U, {RW   , U16, 0x2UL,  &w_0_def_1401_01},  &w_0_act_1401_01, aw_rg_0_1023     , SOD_k_NO_CALLBACK},
  {0x1401U, 0x02U, {RW   , U32, 0x4UL,  &w_0_def_1401_02},  &w_0_act_1401_02, NULL			    , SOD_k_NO_CALLBACK},
  {0x1401U, 0x03U, {RW   , U8 , 0x1UL,  &b_0_def_1401_03},  &b_0_act_1401_03, ab_rg_1_63       , SOD_k_NO_CALLBACK},
  {0x1401U, 0x04U, {RW   , U32, 0x4UL, &dw_0_def_1401_04}, &dw_0_act_1401_04, NULL             , SOD_k_NO_CALLBACK},
  {0x1401U, 0x05U, {RW   , U32, 0x4UL, &dw_0_def_1401_05}, &dw_0_act_1401_05, NULL             , SOD_k_NO_CALLBACK},
  {0x1401U, 0x06U, {RW   , U16, 0x2UL,  &w_0_def_1401_06},  &w_0_act_1401_06, &PROP_DELAY_RANGE, SOD_k_NO_CALLBACK},
  {0x1401U, 0x07U, {RW   , U16, 0x2UL,  &w_0_def_1401_07},  &w_0_act_1401_07, aw_rg_1_65535    , SOD_k_NO_CALLBACK},
  {0x1401U, 0x08U, {RW   , U16, 0x2UL,  &w_0_def_1401_08},  &w_0_act_1401_08, &PROP_DELAY_RANGE, SOD_k_NO_CALLBACK},
  {0x1401U, 0x09U, {RW   , U16, 0x2UL,  &w_0_def_1401_09},  &w_0_act_1401_09, aw_rg_1_65535    , SOD_k_NO_CALLBACK},
  {0x1401U, 0x0AU, {RW   , U16, 0x2UL,  &w_0_def_1401_0A},  &w_0_act_1401_0A, aw_rg_0_65535    , SOD_k_NO_CALLBACK},
  {0x1401U, 0x0BU, {RW   , U32, 0x4UL, &dw_0_def_1401_0B}, &dw_0_act_1401_0B, NULL             , SOD_k_NO_CALLBACK},
  {0x1401U, 0x0CU, {RW   , U16, 0x2UL,  &w_0_def_1401_0C},  &w_0_act_1401_0C, aw_rg_1_1023     , SOD_k_NO_CALLBACK},

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
  /* 0x1800-0x1BFE RxSPDO Mapping Parameter */
  {0x1800U, 0x00U, {RW|BEF_WR, U8 , 0x1UL,   &b_0_def_1800_00   },   &b_0_act_1800_00   , ab_rg_0_253, SPDO_SOD_RxMappPara_CLBK},
  {0x1800U, 0x01U, {RW|BEF_WR, U32, 0x4UL, &adw_0_def_1800_01[0]}, &adw_0_act_1800_01[0], NULL       , SPDO_SOD_RxMappPara_CLBK},
  //{0x1800U, 0x02U, {RW|BEF_WR, U32, 0x4UL, &adw_0_def_1800_01[1]}, &adw_0_act_1800_01[1], NULL       , SPDO_SOD_RxMappPara_CLBK},

  /* 0x1800-0x1BFE RxSPDO Mapping Parameter */
  {0x1801U, 0x00U, {RW|BEF_WR, U8 , 0x1UL,   &b_0_def_1801_00   },   &b_0_act_1801_00   , ab_rg_0_253, SPDO_SOD_RxMappPara_CLBK},
  {0x1801U, 0x01U, {RW|BEF_WR, U32, 0x4UL, &adw_0_def_1801_01[0]}, &adw_0_act_1801_01[0], NULL       , SPDO_SOD_RxMappPara_CLBK},
#endif

  /* TxSPDO Communication Parameter */
  {0x1C00U, 0x00U, {CONST, U8 , 0x1UL, &b_noE_3        }, &b_noE_3        , NULL         , SOD_k_NO_CALLBACK},
  {0x1C00U, 0x01U, {RW   , U16, 0x2UL, &w_0_def_1C00_01}, &w_0_act_1C00_01, aw_rg_0_1023 , SOD_k_NO_CALLBACK},
  {0x1C00U, 0x02U, {RW   , U16, 0x2UL, &w_0_def_1C00_02}, &w_0_act_1C00_02, aw_rg_1_32767, SOD_k_NO_CALLBACK},
  {0x1C00U, 0x03U, {RW   , U8 , 0x1UL, &b_0_def_1C00_03}, &b_0_act_1C00_03, NULL         , SOD_k_NO_CALLBACK},

  /* TxSPDO Communication Parameter */
  {0x1C01U, 0x00U, {CONST, U8 , 0x1UL, &b_noE_3        }, &b_noE_3        , NULL         , SOD_k_NO_CALLBACK},
  {0x1C01U, 0x01U, {RW   , U16, 0x2UL, &w_0_def_1C01_01}, &w_0_act_1C01_01, aw_rg_0_1023 , SOD_k_NO_CALLBACK},
  {0x1C01U, 0x02U, {RW   , U16, 0x2UL, &w_0_def_1C01_02}, &w_0_act_1C01_02, aw_rg_1_32767, SOD_k_NO_CALLBACK},
  {0x1C01U, 0x03U, {RW   , U8 , 0x1UL, &b_0_def_1C01_03}, &b_0_act_1C01_03, NULL         , SOD_k_NO_CALLBACK},

  /* CRC objects */
  {0x2000U, 0x01U, {RW | CRC, U8, 0x1UL, NULL}, &ab_0_act_2000[0], NULL , SOD_k_NO_CALLBACK},
  {0x2000U, 0x02U, {RW | CRC, U8, 0x1UL, NULL}, &ab_0_act_2000[1], NULL , SOD_k_NO_CALLBACK},

  {0x6000U, 0x01U, {RW|PDO ,U8 ,0x1UL, &ab_0_def_ProcessData[0]}, &ab_0_act_ProcessData[0], NULL, SOD_k_NO_CALLBACK},
  {0x6000U, 0x02U, {RW|PDO ,U8 ,0x1UL, &ab_0_def_ProcessData[1]}, &ab_0_act_ProcessData[1], NULL, SOD_k_NO_CALLBACK},

  /* 0xC000-0xC3FE TxSPDO Mapping Parameter */
  {0xC000U, 0x00U, {RW|BEF_WR, U8 , 0x1UL,   &b_0_def_C000_00   },   &b_0_act_C000_00   , ab_rg_0_253, SPDO_SOD_TxMappPara_CLBK},
  {0xC000U, 0x01U, {RW|BEF_WR, U32, 0x4UL, &adw_0_def_C000_01[0]}, &adw_0_act_C000_01[0], NULL       , SPDO_SOD_TxMappPara_CLBK},
  //{0xC000U, 0x02U, {RW|BEF_WR, U32, 0x4UL, &adw_0_def_C000_01[1]}, &adw_0_act_C000_01[1], NULL       , SPDO_SOD_TxMappPara_CLBK},

  /* 0xC000-0xC3FE TxSPDO Mapping Parameter */
  {0xC001U, 0x00U, {RW|BEF_WR, U8 , 0x1UL,   &b_0_def_C001_00   },   &b_0_act_C001_00   , ab_rg_0_253, SPDO_SOD_TxMappPara_CLBK},
  {0xC001U, 0x01U, {RW|BEF_WR, U32, 0x4UL, &adw_0_def_C001_01[0]}, &adw_0_act_C001_01[0], NULL       , SPDO_SOD_TxMappPara_CLBK},

  /* end of SOD */
  {SOD_k_END_OF_THE_OD, 0xFFU, {0U, EPLS_k_BOOLEAN, 0x1UL, NULL}, NULL, NULL, SOD_k_NO_CALLBACK}
};

/** \endcond */

SOD_t_ENTRY_VIRT SAPL_s_SOD_VIRT_INST =
{
    {0, EPLS_k_BOOLEAN, 0x1UL, NULL}, SOD_k_END_OF_THE_OD, SOD_k_END_OF_THE_OD, 0xFF, 0xFF, NULL, NULL, (SOD_t_CLBK)0
};


/* dummy for virtual SOD */
const SOD_t_ENTRY_VIRT *const SAPL_ps_SOD_VIRT[EPLS_cfg_MAX_INSTANCES] =
{
    &SAPL_s_SOD_VIRT_INST
	#if (EPLS_cfg_MAX_INSTANCES > 1)
		,&SAPL_s_SOD_VIRT_INST
	#endif
};

/* global structure for all object dictionaries */
const SOD_cfg_TO_RAM_ROM SOD_t_OBJECT *SAPL_ps_OD[EPLS_cfg_MAX_INSTANCES]=
{
  SAPL_s_OD_INST_0
  #if (EPLS_cfg_MAX_INSTANCES > 1)
    ,SAPL_s_OD_INST_0
  #endif
  #if (EPLS_cfg_MAX_INSTANCES > 2)
    ,SAPL_s_OD_INST_0
  #endif
  #if (EPLS_cfg_MAX_INSTANCES > 3)
    ,SAPL_s_OD_INST_0
  #endif
  #if (EPLS_cfg_MAX_INSTANCES > 4)
    ,SAPL_s_OD_INST_0
  #endif
  #if (EPLS_cfg_MAX_INSTANCES > 5)
    ,SAPL_s_OD_INST_0
  #endif
  #if (EPLS_cfg_MAX_INSTANCES > 6)
    ,SAPL_s_OD_INST_0
  #endif
  #if (EPLS_cfg_MAX_INSTANCES > 7)
    ,SAPL_s_OD_INST_0
  #endif
  #if (EPLS_cfg_MAX_INSTANCES > 8)
    ,SAPL_s_OD_INST_0
  #endif
  #if (EPLS_cfg_MAX_INSTANCES > 9)
    ,SAPL_s_OD_INST_0
  #endif
  #if (EPLS_cfg_MAX_INSTANCES > 10)
    ,SAPL_s_OD_INST_0
  #endif
};

void PrintApplicationData(){
	DATA_LOGGER1("ProcessData[0] Rx Mapping on 0x6000/1 is %d\n\n", ab_0_act_ProcessData[0]);
	DATA_LOGGER1("ProcessData[1] Tx Mapping on 0x6000/2 is %d\n\n", ab_0_act_ProcessData[1]);
}

void increaseAppData(){
	ab_0_act_ProcessData[1]++;
}
