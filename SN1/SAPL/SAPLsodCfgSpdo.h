/*
 * \file UDP_SN/SAPL/SAPLsodCfgSpdo.h
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
**   Workfile: SAPLsodCfgSpdo.h
**    Summary: SAPLdemo - Safety application Demonstration
**             SAPL, application of the EPLsafety Stack
**             Header file for the SPDO configuration in the SOD.
**             This header file is included by the SNscm as well as the
**             SNstandard application.
**
**             SPDO configuration on the SNscm:
**               2 Rx and 2 Tx SPDO are configured and deactivated on the SCM. The 1st Rx and
**               the 1st Tx SPDO are activated and the 2nd Rx and the 2nd Tx
**               SPDO are deactivated.
**               The SNscm receives data on its own 1st Rx SPDO from the 1st Tx
**               SPDO of the SNstandard and transmits data from its own 1st Tx
**               SPDO to the 1st Rx SPDO of the SNstandard.
**             SPDO configuration on the SNstandard:
**               1 Rx and 1 Tx SPDO are configured on the SNstandard. The 1st Rx
**               and the 1st Tx SPDO are activated. The SNstandard receives data
**               on its own 1st Rx SPDO from the 1st Tx SPDO of the SNscm and
**               transmits data from its own 1st Tx SPDO to the 1st Rx SPDO of
**               the SNscm.
**
**     Author: M. Molnar
**
********************************************************************************
********************************************************************************
**
**  Functions: -
**
**    Remarks:
**
*******************************************************************************/

#ifndef SAPL_SPDO_CFG_H
#define SAPL_SPDO_CFG_H

/*******************************************************************************
**    constants and macros
*******************************************************************************/
/** SAPL_k_SCM_SN_ADD_SADR_1:
    This symbol represents the first additional SADR of the SCM SN.
*/
#define SAPL_k_SCM_SN1_ADD_SADR_1  3U

/** SAPL_k_SN_SANDARD_ADD_SADR_1:
    This symbol represents the first additional SADR of the SN standard.
*/
#define SAPL_k_SN2_ADD_SADR_1     4U



/** SAPL_k_NO_RX_SPDO_MAPP_ENTRIES:
    This symbol represents the number of the Rx SPDO mapping entries.
*/
#define SAPL_k_NO_RX_SPDO_MAPP_ENTRIES 1U

/** SAPL_k_NO_TX_SPDO_MAPP_ENTRIES:
    This symbol represents the number of the Tx SPDO mapping entries.
*/
#define SAPL_k_NO_TX_SPDO_MAPP_ENTRIES 1U

/** SAPL_k_SPDO_DEACTIVATED:
    This symbol represents that the SPDO mapping or the SPDO communication
    parameter is deactivated.
*/
#define SAPL_k_SPDO_DEACTIVATED 0U

/*******************************************************************************
**    SPDO configuration on the SNstandard
*******************************************************************************/
/** SAPL_k_SPDO_RX_COMM_PARA_DEF:
  0x1400-0x17FE RxSPDO Communication Parameter.
*/
#define SAPL_k_SPDO_RX_COMM_PARA_DEF  static \
/* 1 RX SPDO */\
       UINT16  w_0_act_1400_01 SAFE_NO_INIT_SEKTOR; /* SADR */ \
static UINT16  w_0_def_1400_01 SAFE_INIT_SEKTOR = SAPL_k_SCM_SN1_SADR;\
static UINT32  w_0_act_1400_02 SAFE_NO_INIT_SEKTOR; /* SCT */ \
static UINT32  w_0_def_1400_02 SAFE_INIT_SEKTOR = 32000U;\
static UINT8   b_0_act_1400_03 SAFE_NO_INIT_SEKTOR; /* Number of consecutive TReq */\
static UINT8   b_0_def_1400_03 SAFE_INIT_SEKTOR = 10U;\
static UINT32 dw_0_act_1400_04 SAFE_NO_INIT_SEKTOR; /* Time delay TReq */\
static UINT32 dw_0_def_1400_04 SAFE_INIT_SEKTOR = 2500U;\
static UINT32 dw_0_act_1400_05 SAFE_NO_INIT_SEKTOR; /* Time delay sync. */\
static UINT32 dw_0_def_1400_05 SAFE_INIT_SEKTOR = 5000U;\
static UINT16  w_0_act_1400_06 SAFE_NO_INIT_SEKTOR; /* Min. TSync. propagation delay */\
static UINT16  w_0_def_1400_06 SAFE_INIT_SEKTOR = 0U;\
static UINT16  w_0_act_1400_07 SAFE_NO_INIT_SEKTOR; /* May. TSync. propagation delay original 500*/\
static UINT16  w_0_def_1400_07 SAFE_INIT_SEKTOR = 500U;\
static UINT16  w_0_act_1400_08 SAFE_NO_INIT_SEKTOR; /* Min. SPDO propagation delay */\
static UINT16  w_0_def_1400_08 SAFE_INIT_SEKTOR = 0U;\
static UINT16  w_0_act_1400_09 SAFE_NO_INIT_SEKTOR; /* Max. SPDO propagation delay original 500*/\
static UINT16  w_0_def_1400_09 SAFE_INIT_SEKTOR = 500U;\
static UINT16  w_0_act_1400_0A SAFE_NO_INIT_SEKTOR; /* Best case TRes delay */\
static UINT32  w_0_def_1400_0A SAFE_INIT_SEKTOR = 0U;\
static UINT32 dw_0_act_1400_0B SAFE_NO_INIT_SEKTOR; /* Time request cycle */\
static UINT32 dw_0_def_1400_0B SAFE_INIT_SEKTOR = 10000U;\
static UINT16  w_0_act_1400_0C SAFE_NO_INIT_SEKTOR; /* Tx SPDO number */\
static UINT16  w_0_def_1400_0C SAFE_INIT_SEKTOR = 1U;\
/* 2 RX SPDO */\
static UINT16  w_0_act_1401_01 SAFE_NO_INIT_SEKTOR; /* SADR */\
static UINT16  w_0_def_1401_01 SAFE_INIT_SEKTOR = SAPL_k_SCM_SN1_ADD_SADR_1;\
static UINT32  w_0_act_1401_02 SAFE_NO_INIT_SEKTOR; /* SCT */\
static UINT32  w_0_def_1401_02 SAFE_INIT_SEKTOR = 32000U;\
static UINT8   b_0_act_1401_03 SAFE_NO_INIT_SEKTOR; /* Number of consecutive TReq */\
static UINT8   b_0_def_1401_03 SAFE_INIT_SEKTOR = 10U;\
static UINT32 dw_0_act_1401_04 SAFE_NO_INIT_SEKTOR; /* Time delay TReq */\
static UINT32 dw_0_def_1401_04 SAFE_INIT_SEKTOR = 2500U;\
static UINT32 dw_0_act_1401_05 SAFE_NO_INIT_SEKTOR; /* Time delay sync. */\
static UINT32 dw_0_def_1401_05 SAFE_INIT_SEKTOR = 5000U;\
static UINT16  w_0_act_1401_06 SAFE_NO_INIT_SEKTOR; /* Min. TSync. propagation delay */\
static UINT16  w_0_def_1401_06 SAFE_INIT_SEKTOR = 0U;\
static UINT16  w_0_act_1401_07 SAFE_NO_INIT_SEKTOR; /* May. TSync. propagation delay  original 500*/\
static UINT16  w_0_def_1401_07 SAFE_INIT_SEKTOR = 500U;\
static UINT16  w_0_act_1401_08 SAFE_NO_INIT_SEKTOR; /* Min. SPDO propagation delay */\
static UINT16  w_0_def_1401_08 SAFE_INIT_SEKTOR = 0U;\
static UINT16  w_0_act_1401_09 SAFE_NO_INIT_SEKTOR; /* Max. SPDO propagation delay original 500 */\
static UINT16  w_0_def_1401_09 SAFE_INIT_SEKTOR = 500U;\
static UINT16  w_0_act_1401_0A SAFE_NO_INIT_SEKTOR; /* Best case TRes delay */\
static UINT32  w_0_def_1401_0A SAFE_INIT_SEKTOR = 0U;\
static UINT32 dw_0_act_1401_0B SAFE_NO_INIT_SEKTOR; /* Time request cycle */\
static UINT32 dw_0_def_1401_0B SAFE_INIT_SEKTOR = 10000U;\
static UINT16  w_0_act_1401_0C SAFE_NO_INIT_SEKTOR; /* Tx SPDO number */\
static UINT16  w_0_def_1401_0C SAFE_INIT_SEKTOR = 1U

/** SAPL_k_SPDO_RX_MAPP_PARA_DEF:
  0x1800-0x1BFE RxSPDO Mapping Parameter.
*/
#define SAPL_k_SPDO_RX_MAPP_PARA_DEF static\
/* 1 RX SPDO */\
       UINT8    b_0_act_1800_00 SAFE_NO_INIT_SEKTOR; /* Number of Rx SPDO mapping entries */\
static UINT8    b_0_def_1800_00 SAFE_INIT_SEKTOR = SAPL_k_NO_RX_SPDO_MAPP_ENTRIES;\
static UINT32 adw_0_act_1800_01[SAPL_k_NO_RX_SPDO_MAPP_ENTRIES] SAFE_NO_INIT_SEKTOR; /* Rx SPDO mapping entries */\
static UINT32 adw_0_def_1800_01[SAPL_k_NO_RX_SPDO_MAPP_ENTRIES] SAFE_INIT_SEKTOR = {0x60000108UL};\
/* 2 RX SPDO */\
static UINT8    b_0_act_1801_00 SAFE_NO_INIT_SEKTOR; /* Number of Rx SPDO mapping entries*/\
static UINT8    b_0_def_1801_00 SAFE_INIT_SEKTOR = SAPL_k_SPDO_DEACTIVATED;\
static UINT32 adw_0_act_1801_01[SAPL_k_NO_RX_SPDO_MAPP_ENTRIES] SAFE_NO_INIT_SEKTOR; /* Rx SPDO mapping entries */\
static UINT32 adw_0_def_1801_01[SAPL_k_NO_RX_SPDO_MAPP_ENTRIES] SAFE_INIT_SEKTOR = {0x60000108UL}

/** SAPL_k_SPDO_TX_COMM_PARA_DEF:
  0x1C00-0x1FFE TxSPDO Communication Parameter.
*/
#define SAPL_k_SPDO_TX_COMM_PARA_DEF static\
/* 1 TX SPDO */\
       UINT16 w_0_act_1C00_01 SAFE_NO_INIT_SEKTOR; /* Tx SPDO SADR */\
static UINT16 w_0_def_1C00_01 SAFE_INIT_SEKTOR = SAPL_k_SN2_ADD_SADR_1;\
static UINT16 w_0_act_1C00_02 SAFE_NO_INIT_SEKTOR; /* Refresh prescale */\
static UINT16 w_0_def_1C00_02 SAFE_INIT_SEKTOR = 1000U; /*original 1000 */\
static UINT8  b_0_act_1C00_03 SAFE_NO_INIT_SEKTOR; /* Number of TRes */\
static UINT8  b_0_def_1C00_03 SAFE_INIT_SEKTOR = 1U;\
/* 2 TX SPDO */\
static UINT16 w_0_act_1C01_01 SAFE_NO_INIT_SEKTOR; /* Tx SPDO SADR */\
static UINT16 w_0_def_1C01_01 SAFE_INIT_SEKTOR = SAPL_k_SPDO_DEACTIVATED;\
static UINT16 w_0_act_1C01_02 SAFE_NO_INIT_SEKTOR; /* Refresh prescale */\
static UINT16 w_0_def_1C01_02 SAFE_INIT_SEKTOR = 1000U; /*original 1000 */\
static UINT8  b_0_act_1C01_03 SAFE_NO_INIT_SEKTOR; /* Number of TRes */\
static UINT8  b_0_def_1C01_03 SAFE_INIT_SEKTOR = 1U

/** SAPL_k_SPDO_TX_MAPP_PARA_DEF:
  0xC000-0xC3FE TxSPDO Mapping Parameter.
*/
#define SAPL_k_SPDO_TX_MAPP_PARA_DEF static\
/* 1 TX SPDO */\
       UINT8    b_0_act_C000_00 SAFE_NO_INIT_SEKTOR; /* Number of Tx SPDO mapping entries */\
static UINT8    b_0_def_C000_00 SAFE_INIT_SEKTOR = SAPL_k_NO_TX_SPDO_MAPP_ENTRIES;\
static UINT32 adw_0_act_C000_01[SAPL_k_NO_TX_SPDO_MAPP_ENTRIES] SAFE_NO_INIT_SEKTOR; /* Tx SPDO mapping entries */\
static UINT32 adw_0_def_C000_01[SAPL_k_NO_TX_SPDO_MAPP_ENTRIES] SAFE_INIT_SEKTOR = {0x60000208UL};\
/* 2 TX SPDO */\
static UINT8    b_0_act_C001_00 SAFE_NO_INIT_SEKTOR; /* Number of Tx SPDO mapping entries */\
static UINT8    b_0_def_C001_00 SAFE_INIT_SEKTOR = SAPL_k_SPDO_DEACTIVATED;\
static UINT32 adw_0_act_C001_01[SAPL_k_NO_TX_SPDO_MAPP_ENTRIES] SAFE_NO_INIT_SEKTOR; /* Tx SPDO mapping entries */\
static UINT32 adw_0_def_C001_01[SAPL_k_NO_TX_SPDO_MAPP_ENTRIES] SAFE_INIT_SEKTOR = {0x60000208UL}



#endif

