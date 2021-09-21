/*
 * \addtogroup SAPL_UDP_SCM
 * *****************************************************************************
** Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
**                      IXXAT Automation GmbH
** All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
**
** This source code is free software; you can redistribute it and/or modify it
** under the terms of the BSD license (according to License.txt).
********************************************************************************
**
**   Workfile: SAPLscmClbk.c
**    Summary: SAPLdemo - Safety application Demonstration
**             SAPL, application of the EPLsafety Stack
**             This file contains SCM callback functions.
**
**     Author: M. Molnar
**
********************************************************************************
********************************************************************************
**
**  Functions: SAPL_ScmSnFailClbk
**             SAPL_ScmNodeStatusChangedClbk
**             SAPL_ScmUdidMismatchClbk
**             SAPL_ScmRevisionNumberClbk
**
**    Remarks:
**
**
*******************************************************************************/


/*******************************************************************************
**    compiler directives
*******************************************************************************/


/*******************************************************************************
**    include-files
*******************************************************************************/
#include "../include/UDP.h"
#include "SCMapi.h"
#include "SCFMapi.h"

/*******************************************************************************
**    global variables
*******************************************************************************/

/*******************************************************************************
**    static constants, types, macros, variables
*******************************************************************************/
/** APP_DBG_PRINT_ACTIVE:
    If this define is defined then all application printfs will be activated
    otherwise all application printfs are deactivated
*/
#define APP_DBG_PRINT_ACTIVATE

#ifdef APP_DBG_PRINT_ACTIVATE
  #define APP_DBG_PRINTF  DBG_PRINTF
  #define APP_DBG_PRINTF1 DBG_PRINTF1
  #define APP_DBG_PRINTF2 DBG_PRINTF2
#else
  #define APP_DBG_PRINTF
  #define APP_DBG_PRINTF1
  #define APP_DBG_PRINTF2
  #define APP_DBG_PRINTF3
#endif

/*******************************************************************************
**    static function-prototypes
*******************************************************************************/

/*******************************************************************************
**    global functions
*******************************************************************************/
/*******************************************************************************
**
** Function    : SAPL_ScmSnFailClbk
**
** Description : This function is a callback function which must be provided
**               by the EPLS Application. This function is called after
**               every 'SN FAIL' during the process 'Activate SN'.
**               The application may acknowledge this report be means
**               of {SCM_SnFailAck()}.
**
** See Also    : SCM_SnFailAck()
**
** Parameters  : w_sadr (IN)     - SADR of the SN with the 'SN FAIL'.
**               b_errGroup (IN) - error group from the SNMT_SN_FAIL response
**                                 frame.
**               b_errCode (IN)  - error code from the SNMT_SN_FAIL response
**                                 frame.
**               w_hdl (IN)      - Handle to the SN with the UDID mismatch,
**                                 necessary for {SCM_SnFailAck()}.
**
** Returnvalue : -
**
*******************************************************************************/
void SAPL_ScmSnFailClbk(UINT16 w_sadr, UINT8 b_errGroup, UINT8 b_errCode,
                        UINT16 w_hdl)
{
  APP_DBG_PRINTF1("\n\n!!! ERROR HAPPENED !!! Configuration process for SN (SADR = %u): failed\n",
                  w_sadr);

  /* to avoid PC-lint info */
  w_sadr = w_sadr;

  /* if application acknowledge failed */
  if (!SCM_SnFailAck(w_hdl, b_errGroup, b_errCode))
  {
    /* error was already reported by the SCM */
  }

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/*******************************************************************************
**
** Function    : SAPL_ScmNodeStatusChangedClbk
**
** Description : This function is a callback function which must be provided
**               by the EPLS Application. This function is called for
**               every Safety Node status change.
**
** Parameters  : w_sadr (IN)       - SADR of the SN with the status change.
**               b_oldStatus (IN)  - The old SN status
**               b_newStatus (IN)  - The new SN status
** Returnvalue : -
**
*******************************************************************************/
void SAPL_ScmNodeStatusChangedClbk(UINT16 w_sadr, UINT8 b_oldStatus,
                                   UINT8 b_newStatus)
{
	  UINT8 ab_snStatusToStr[129][30]= /* lookup table for SN status */
	  {
	    "MISSING",
	    "INVALID",
	    "WRONG SADR",
	    "UDID MISMATCH",
	    "WRONG PARAMETERS",
		"WRONG ADD PARAMETERS",
		"INCOMPATIBEL VERSION",
		"ERROR INIT CT"

	  };
	  sprintf(ab_snStatusToStr[128], "VALID");
	  sprintf(ab_snStatusToStr[129], "OK");

  UINT8 ab_snSadrToStr[2][30]= /* lookup table for SN SADR */
  {
    "SN on the SCM",
    "SN standard"
  };

  APP_DBG_PRINTF2("SN status has changed.\n"
                  "  - SADR : %u, (%s)\n", w_sadr,
                  ab_snSadrToStr[w_sadr-1]);
  APP_DBG_PRINTF2("  - Old node status : %s\n"
                  "  - New node status : %s\n\n",
                  ab_snStatusToStr[b_oldStatus],
                  ab_snStatusToStr[b_newStatus]);

  /* to avoid PC-lint Info */
  w_sadr = w_sadr;
  b_oldStatus = b_oldStatus;
  b_newStatus = b_newStatus;
  ab_snStatusToStr[0][0] = ab_snStatusToStr[0][0];
  ab_snSadrToStr[0][0] = ab_snSadrToStr[0][0];

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/*******************************************************************************
**
** Function    : SAPL_ScmUdidMismatchClbk
**
** Description : This function is a callback function which must be provided
**               by the EPLS Application. This function is called for every
**               UDID mismatch. The operator has to acknowledge this mismatch
**               be means of {SCM_OperatorAck()}.
**
** See Also    : SCM_OperatorAck()
**
** Parameters  : w_sadr (IN)     - SADR of the SN with the UDID mismatch.
**               pa_newUdid (IN) - Pointer to the UDID, which is received
**                                 from the SN.
**               w_hdl (IN)      - Handle to the FSM with the UDID mismatch,
**                                 necessary for {SCM_OperatorAck()}.
**
** Returnvalue : -
**
*******************************************************************************/
void SAPL_ScmUdidMismatchClbk(UINT16 w_sadr, const UINT8 *pa_newUdid,
                              UINT16 w_hdl)
{
  /* to avoid compiler warning */
  w_sadr = w_sadr;
  w_hdl = w_hdl;
  pa_newUdid = pa_newUdid;

  APP_DBG_PRINTF("\n\n!!! ERROR HAPPENED !!! Unique Device ID mismatch.\n");

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();
}

/*******************************************************************************
**
** Function    : SAPL_ScmRevisionNumberClbk
**
** Description : This function is a callback function which must be provided
**               by the EPLS Application. This function is called, after the
**               revision number from the SN is received. The application has
**               to check if the revision number should be accepted or not.
**
** Parameters  : w_sadr (IN)       - SADR of the SN with the revision
**                                   number.
**               dw_expRevNum (IN) - The expected revision number of the SN.
**               dw_recRevNum (IN) - The received revision number of the SN.
**
** Returnvalue : TRUE              - The revision number will be accepted.
**               FALSE             - The revision number will NOT be accepted.
**
*******************************************************************************/
BOOLEAN SAPL_ScmRevisionNumberClbk(UINT16 w_sadr, UINT32 dw_expRevNum,
                                   UINT32 dw_recRevNum)
{
  BOOLEAN o_ret = FALSE; /* return value */

  /* if the received and the expected Revision number are the same */
  if (dw_recRevNum == dw_expRevNum)
  {
    APP_DBG_PRINTF("Expected revision number was received from the SN ");
    APP_DBG_PRINTF1("with ID %u\n\n", w_sadr);
    o_ret = TRUE;
  }
  /* else unexpected Revision number */
  else
  {
    APP_DBG_PRINTF1("SN with ID %u has a wrong revision number.\n", w_sadr);
    APP_DBG_PRINTF2("(Received = %x, Expected = %x)\n\n",
                    dw_recRevNum, dw_expRevNum);
  }

  /* to aviod PC-lint Info */
  w_sadr = w_sadr;

  /* call the Control Flow Monitoring */
  SCFM_TACK_PATH();

  return o_ret;
}

/*******************************************************************************
**    static functions
*******************************************************************************/
