/**
 * @addtogroup SPDO
 * @{
 *
 * @file SPDOassign.c
 *
 * This file contains the table(s) to assign an SN address to the
*             SPDO number.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author M. Molnar, IXXAT Automation GmbH
 *
 * <h2>History for SPDOassign.c</h2>
 * <table>
 *     <tr><th>Date</th><th>Author</th><th>Change Description</th></tr>
 *     <tr><td>15.03.2013</td><td>Hans Pill</td><td>added statistic counters</td></tr>
 *     <tr><td>09.08.2013</td><td>Hans Pill</td><td>Review OpenSAFETY 1.4.0.0</td></tr>
 * </table>
 *
 */

#include "EPLStarget.h"
#include "EPLScfg.h"
#include "EPLStypes.h"
#include "EPLScfgCheck.h"

#include "SCFMapi.h"

#include "SPDOint.h"
#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
#include "SERRapi.h"
#include "SERR.h"
#endif


#if (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE == EPLS_k_DISABLE)
  /**
   * Structure for the SPDO number and SN address assignment table.
  */
  typedef struct
  {
	  /** SN address */
    UINT16 w_adr;
    /** SPDO index in the internal array */
    UINT16 w_spdoIdx;
  } t_ADR_TO_SPDO_NUM;

#else
  /**
   * The lookup table is addressed by the SN address.
   *
   * SN valid address range is 1..1023 and it is EPLS_k_MAX_SADR entry in the lookup table.
   * +1 is for the  SN address 0. It has always invalid SPDO number value. It means that the
   * lookup table address range is 0..1023 and it has 1024 entries.
  */
  #define k_LOOKUP_TABLE_SIZE ((UINT16)(EPLS_k_MAX_SADR+1U))
#endif

/**
 * Object structure for the SPDO index and SN address assignment tables that
 * are used to filter the SPDOs and assign the SN address to a SPDO index
 * in the internal array of the SPDO structures.
*/
typedef struct
{
  #if (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE == EPLS_k_DISABLE)
    #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
      /** SADR to Rx SPDO index table */
      t_ADR_TO_SPDO_NUM as_sAdrToRxSpdoIdx[SPDO_cfg_MAX_NO_RX_SPDO];
    #endif
    /** TADR to Tx SPDO index table */
    t_ADR_TO_SPDO_NUM as_tAdrToTxSpdoIdx[SPDO_cfg_MAX_NO_TX_SPDO];
  #else
    #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
      /** SADR to Rx SPDO index table */
      UINT16 aw_sAdrToRxSpdoIdx[k_LOOKUP_TABLE_SIZE]; /* Lookup table */
    #endif
    /** TADR to Tx SPDO index table */
    UINT16 aw_tAdrToTxSpdoIdx[k_LOOKUP_TABLE_SIZE]; /* Lookup table */
  #endif

  #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    /** number of Rx SPDO to be processed */
    UINT16 w_noProcRxSpdo;
  #endif
  /** number of Tx SPDO to be processed */
  UINT16 w_noProcTxSpdo;
}t_ASSIGN_OBJ;

static t_ASSIGN_OBJ as_Obj[EPLS_cfg_MAX_INSTANCES] SAFE_NO_INIT_SEKTOR;

/**
 * @brief This function initializes the assignment tables for the SPDOs.
 *
 * @param        b_instNum        instance number (not checked, checked in SSC_InitAll() or SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 */
void SPDO_InitAssign(BYTE_B_INSTNUM)
{
  t_ASSIGN_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  UINT32 i; /* loop counter */

  #if (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE == EPLS_k_DISABLE)
    #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
      /* SADR to Rx SPDO index table is initialized */
      for(i = 0U; i < (UINT32)SPDO_cfg_MAX_NO_RX_SPDO; i++)
      {
        po_this->as_sAdrToRxSpdoIdx[i].w_spdoIdx = k_INVALID_SPDO_NUM;
        po_this->as_sAdrToRxSpdoIdx[i].w_adr = k_NOT_USED_ADR;
      }
    #endif

    /* TADR to Tx SPDO index table is initialized */
    for(i = 0U; i < (UINT32)SPDO_cfg_MAX_NO_TX_SPDO; i++)
    {
      po_this->as_tAdrToTxSpdoIdx[i].w_spdoIdx = k_INVALID_SPDO_NUM;
      po_this->as_tAdrToTxSpdoIdx[i].w_adr = k_NOT_USED_ADR;
    }
  #else
    /* The TADR to Tx SPDO index table and the SADR to Rx SPDO index
       lookup tables are initialized (k_LOOKUP_TABLE_SIZE is divisible by 4
       checked in EPLScfgCkeck.h) */
    for(i = 0U; i < k_LOOKUP_TABLE_SIZE; i=i+4U)
    {
      #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
        po_this->aw_sAdrToRxSpdoIdx[i]    = k_INVALID_SPDO_NUM;

        /*lint -e{661} (Warning -- Possible access of out-of-bounds pointer
                                   by operator '[' */
        po_this->aw_sAdrToRxSpdoIdx[i+1U] = k_INVALID_SPDO_NUM;

        /*lint -e{661} -e{662} (Warning -- Possible access of out-of-bounds
                                           pointer by operator '[' */
        po_this->aw_sAdrToRxSpdoIdx[i+2U] = k_INVALID_SPDO_NUM;

        /*lint -e{661} -e{662} (Warning -- Possible access of out-of-bounds
                                           pointer by operator '[' */
        po_this->aw_sAdrToRxSpdoIdx[i+3U] = k_INVALID_SPDO_NUM;
      #endif
      po_this->aw_tAdrToTxSpdoIdx[i]    = k_INVALID_SPDO_NUM;

      /*lint -e{661} (Warning -- Possible access of out-of-bounds pointer
                                 by operator '[' */
      po_this->aw_tAdrToTxSpdoIdx[i+1U] = k_INVALID_SPDO_NUM;

      /*lint -e{661} -e{662} (Warning -- Possible access of out-of-bounds
                                         pointer by operator '[' */
      po_this->aw_tAdrToTxSpdoIdx[i+2U] = k_INVALID_SPDO_NUM;

      /*lint -e{661} -e{662} (Warning -- Possible access of out-of-bounds
                                         pointer by operator '[' */
      po_this->aw_tAdrToTxSpdoIdx[i+3U] = k_INVALID_SPDO_NUM;
    }
  #endif

  #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    po_this->w_noProcRxSpdo = 0U;
  #endif
  po_this->w_noProcTxSpdo = 0U;

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
/**
 * @brief This function insert a new source address with Rx SPDO index into the assignment array (Address 0 is ignored).
 *
 * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_rxSpdoIdx        Rx SPDO index (not checked, checked in SPDO_ActivateRxSpdoMapping) valid range: 0..(SPDO_cfg_MAX_NO_RX_SPDO-1)
 *
 * @param        w_sAdr             source SN address (checked) valid range: 0..(EPLS_k_MAX_SADR)
 *
 * @return
 * - TRUE             - success
 * - FALSE            - failure, SERR_SetError() is not called by this function, but a FATAL error must be reported by the calling function.
 */
  BOOLEAN SPDO_AddSAdr(BYTE_B_INSTNUM_ UINT16 w_rxSpdoIdx, UINT16 w_sAdr)
  {
    BOOLEAN o_ret = FALSE; /* return value */
    t_ASSIGN_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
    UINT16 w_spdoIdx; /* temporary variable for SPDO index */
    #if (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE == EPLS_k_DISABLE)
      BOOLEAN o_break = FALSE; /* flag for breaking the for cycle */
      UINT32 rx; /* loop counter */
      t_ADR_TO_SPDO_NUM *ps_rxTableEntry; /* pointer to access the rx SPDO
                                             number table */
    #endif

    /* if SPDO is not used */
    if (w_sAdr == k_NOT_USED_ADR)
    {
      o_ret = TRUE;
    }
    /* else if SPDO is invalid */
    else if (w_sAdr > EPLS_k_MAX_SADR)
    {
      /* error happened (See Returnvalue) */
    }
    /* SPDO is used and valid */
    else
    {
      /* source address is searched in the target address table */
      w_spdoIdx = SPDO_GetSpdoIdxForTAdr(B_INSTNUM_ w_sAdr);

      /* if source address was not found in the target address table */
      if (w_spdoIdx == k_INVALID_SPDO_NUM)
      {
        #if (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE == EPLS_k_DISABLE)
          rx = 0U;
          /* SADR to Rx SPDO number is inserted */
          while ((rx < (UINT32)SPDO_cfg_MAX_NO_RX_SPDO) && (!o_break))
          {
            ps_rxTableEntry = &po_this->as_sAdrToRxSpdoIdx[rx];
            /* if address has been already inserted */
            if (ps_rxTableEntry->w_adr == w_sAdr)
            {
              /* error happened (See Returnvalue) */
              o_break = TRUE;
            }
            /* else address was not found */
            else
            {
              /* if address is free */
              if (ps_rxTableEntry->w_adr == k_NOT_USED_ADR)
              {
                ps_rxTableEntry->w_adr = w_sAdr;
                ps_rxTableEntry->w_spdoIdx = w_rxSpdoIdx;

                /* the last and the largest SPDO index is stored */
                po_this->w_noProcRxSpdo = (UINT16)(w_rxSpdoIdx + 1U);

                o_ret = TRUE;
                o_break = TRUE;
              }
              /* no else : next entry */
              rx++;
            }
          }
        #else
          /* address is free */
          if (po_this->aw_sAdrToRxSpdoIdx[w_sAdr] == k_INVALID_SPDO_NUM)
          {
            po_this->aw_sAdrToRxSpdoIdx[w_sAdr] = w_rxSpdoIdx;

            /* the last and the largest SPDO index is stored */
            po_this->w_noProcRxSpdo = (UINT16)(w_rxSpdoIdx + 1U);

            o_ret = TRUE;
          }
          /* no else address is not free error happened (See Returnvalue) */
        #endif
      }
      /* no else the source address has been already defined
         error happened (See Returnvalue) */
    }

    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return o_ret;
  }
#endif

  /**
   * @brief This function inserts a new target address with Tx SPDO index into the assignment array (Address 0 is ignored).
   *
   * @param        b_instNum          instance number (not checked, checked in SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
   *
   * @param        w_txSpdoIdx        Tx SPDO index (not checked, checked in SPDO_ActivateTxSpdoMapping) valid range: 0..(SPDO_cfg_MAX_NO_TX_SPDO-1)
   *
   * @param        w_tAdr             target SN address (checked) valid range: 0..(EPLS_k_MAX_SADR)
   *
   * @return
   * - TRUE             - success
   * - FALSE            - failure, SERR_SetError() is not called by this function, but a FATAL error must be reported by the calling function.
   */
BOOLEAN SPDO_AddTAdr(BYTE_B_INSTNUM_ UINT16 w_txSpdoIdx, UINT16 w_tAdr)
{
  BOOLEAN o_ret = FALSE; /* return value */
  t_ASSIGN_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    UINT16 w_spdoIdx; /* temporary variable for SPDO index */
  #endif
  #if (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE == EPLS_k_DISABLE)
    BOOLEAN o_break = FALSE; /* flag for breaking the for cycle */
    UINT32 tx; /* loop counter */
    t_ADR_TO_SPDO_NUM *ps_txTableEntry; /* pointer to access the tx SPDO number
                                           table */
  #endif

  /* if SPDO is not used */
  if (w_tAdr == k_NOT_USED_ADR)
  {
    o_ret = TRUE;
  }
  /* else if SPDO is invalid */
  else if (w_tAdr > EPLS_k_MAX_SADR)
  {
    /* error happened (See Returnvalue) */
  }
  /* SPDO is used and valid */
  else
  {
  #if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
    /* target address is searched in the source address table */
    w_spdoIdx = SPDO_GetSpdoIdxForSAdr(B_INSTNUM_ w_tAdr);

    /* if target address was not found in the source address table */
    /* RSM_IGNORE_QUALITY_BEGIN Notice #22 - 'if' is not bound with scope
                                         braces {} */
    if (w_spdoIdx == k_INVALID_SPDO_NUM)
  #endif
    {
    /* RSM_IGNORE_QUALITY_END */

      #if (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE == EPLS_k_DISABLE)
        tx = 0U;
        /* TADR to Tx SPDO index is inserted */
        while ((tx < (UINT32)SPDO_cfg_MAX_NO_TX_SPDO) && (!o_break))
        {
          ps_txTableEntry = &po_this->as_tAdrToTxSpdoIdx[tx];
          /* if address is not free */
          if (ps_txTableEntry->w_adr == w_tAdr)
          {
            /* error happened (See Returnvalue) */
            o_break = TRUE;
          }
          /* else address was not found */
          else
          {
            /* if address is free */
            if (ps_txTableEntry->w_adr == k_NOT_USED_ADR)
            {
              ps_txTableEntry->w_adr = w_tAdr;
              ps_txTableEntry->w_spdoIdx = w_txSpdoIdx;

              /* the last and the largest SPDO index is stored */
              po_this->w_noProcTxSpdo = (UINT16)(w_txSpdoIdx + 1U);

              o_ret = TRUE;
              o_break = TRUE;
            }
            /* no else : next entry */
            tx++;
          }
        }
      #else
        /* address is free */
        if (po_this->aw_tAdrToTxSpdoIdx[w_tAdr] == k_INVALID_SPDO_NUM)
        {
          po_this->aw_tAdrToTxSpdoIdx[w_tAdr] = w_txSpdoIdx;

          /* the last and the largest SPDO index is stored */
          po_this->w_noProcTxSpdo = (UINT16)(w_txSpdoIdx + 1U);

          o_ret = TRUE;
        }
        /* no else : address is not free error happened (See Returnvalue) */
      #endif
    }
    /* no else : the target address has been already defined
       error happened (See Returnvalue) */
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}


#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
/**
 * @brief Gets the Rx SPDO index for the given source address
 *
 * @param        b_instNum            instance number (not checked, checked in SPDO_ProcessRxSpdo() or SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @param        w_sAdr               source SN address (not checked, checked in SPDO_AddTAdr() or checkRxAddrInfo()) valid range: 0..(EPLS_k_MAX_SADR)
 *
 * @return       0..1022            - valid Rx SPDO index   k_INVALID_SPDO_NUM - invalid SPDO index
 */
  UINT16 SPDO_GetSpdoIdxForSAdr(BYTE_B_INSTNUM_ UINT16 w_sAdr)
  {
    UINT16 w_ret = k_INVALID_SPDO_NUM; /* return value */
    t_ASSIGN_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
    #if (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE == EPLS_k_DISABLE)
      BOOLEAN o_break = FALSE; /* flag for breaking the for cycle */
      UINT32 i; /* loop counter */
    #endif

    #if (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE == EPLS_k_DISABLE)
      i = 0U;
      /* search SADR */
      while ((i < (UINT32)SPDO_cfg_MAX_NO_RX_SPDO) && (!o_break))
      {
        /* if address is found */
        if (po_this->as_sAdrToRxSpdoIdx[i].w_adr == w_sAdr)
        {
          w_ret = po_this->as_sAdrToRxSpdoIdx[i].w_spdoIdx;
          o_break = TRUE;
        }
        /* else address is not found */
        else
        {
          /* if end of the source address table */
          if (po_this->as_sAdrToRxSpdoIdx[i].w_adr == k_NOT_USED_ADR)
          {
            o_break = TRUE;
          }
          /* no else : next entry */
          i++;
        }
      }
    #else
      /* get Spdo index */
      w_ret = po_this->aw_sAdrToRxSpdoIdx[w_sAdr];
    #endif
    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return w_ret;
  }
#endif

  /**
   * @brief Gets the Tx SPDO index for the given target address
   *
   * @param        b_instNum            instance number (not checked, checked in SPDO_ProcessRxSpdo() or SSC_ProcessSNMTSSDOFrame()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
   *
   * @param        w_tAdr               target SN address (not checked, checked in SPDO_AddSAdr() or checkRxAddrInfo()) valid range: 0..(EPLS_k_MAX_SADR)
   *
   * @return       0..1022            - valid Tx SPDO index  k_INVALID_SPDO_NUM - invalid SPDO index
   */
UINT16 SPDO_GetSpdoIdxForTAdr(BYTE_B_INSTNUM_ UINT16 w_tAdr)
{
  UINT16 w_ret = k_INVALID_SPDO_NUM; /* return value */
  t_ASSIGN_OBJ *po_this = &as_Obj[B_INSTNUMidx]; /* instance pointer */
  #if (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE == EPLS_k_DISABLE)
    BOOLEAN o_break = FALSE; /* flag for breaking the for cycle */
    UINT32 i; /* loop counter */
  #endif

  #if (SPDO_cfg_SPDO_NUM_LOOK_UP_TABLE == EPLS_k_DISABLE)
    i = 0U;
    /* search SADR */
    while ((i < (UINT32)SPDO_cfg_MAX_NO_TX_SPDO) && (!o_break))
    {
      /* if address is found */
      if (po_this->as_tAdrToTxSpdoIdx[i].w_adr == w_tAdr)
      {
        w_ret = po_this->as_tAdrToTxSpdoIdx[i].w_spdoIdx;
        o_break = TRUE;
      }
      /* else address is not found */
      else
      {
        /* if end of the target address table */
        if (po_this->as_tAdrToTxSpdoIdx[i].w_adr == k_NOT_USED_ADR)
        {
          o_break = TRUE;
        }
        /* no else : next entry */
        i++;
      }
    }
  #else
    /* get Spdo index */
    w_ret = po_this->aw_tAdrToTxSpdoIdx[w_tAdr];
  #endif

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return w_ret;
}

#if (SPDO_cfg_MAX_NO_RX_SPDO != 0)
/**
 * @brief This function gets the number of Rx SPDOs to be processed.
 *
 * @param        b_instNum            instance number (not checked, checked in SPDO_CheckRxTimeout() or SPDO_BuildTxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
 *
 * @return       number of Rx SPDOs to be processed
 */
  UINT16 SPDO_GetNoProcRxSpdo(BYTE_B_INSTNUM)
  {
    /* call the Control Flow Monitoring */
    SCFM_TACK_PATH();

    return as_Obj[B_INSTNUMidx].w_noProcRxSpdo;
  }
#endif

  /**
   * @brief This function gets the number of Tx SPDOs to be processed.
   *
   * @param        b_instNum            instance number (not checked, checked in SPDO_BuildTxSpdo()) valid range: 0..(EPLS_cfg_MAX_INSTANCES-1)
   *
   * @return       number of Tx SPDOs to be processed
   */
UINT16 SPDO_GetNoProcTxSpdo(BYTE_B_INSTNUM)
{
  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return as_Obj[B_INSTNUMidx].w_noProcTxSpdo;
}

/** @} */
