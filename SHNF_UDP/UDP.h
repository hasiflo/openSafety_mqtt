/*
 * \file SHNF_UDP.h
 * The UDP based SHNF implements a basic SHNF interface using UDP data payloads
 * to transmit openSAFETY information.
 * \addtogroup Demo
 * \addtogroup UDPSHNF
 * \{
 *
 *
 * SHNF, unit to simulate the interface to firmware driver.
 * For test purposes this unit provides all necessary
 * interfaces between EPLsafety Stack and firmware driver
 * ("Firmware close to the hardware").
 *
 * \author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \author M.Molnar, IXXAT Automation GmbH (original Demo application)
 *
 *****************************************************************************
 * Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and
 *                      IXXAT Automation GmbH
 * All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the BSD license (according to License.txt).
 *****************************************************************************/

#ifndef SHNFDEMO_H
#define SHNFDEMO_H

#include "EPLStarget.h"

#include "EPLScfg.h"
#include "EPLStypes.h"

#include "SHNF.h"
#include "SHNF_Definitions.h"

/**
 * \brief This function initializes the SHNF interface.
 *
 * \param w_loopBackSadr - loop back source address
 */
void SHNF_Init(BYTE_B_INSTNUM_ UINT16 w_loopBackSadr);

/**
 * This function returns a reference to a received openSAFETY frame.
 *
 * The reserved space for each received frame can be freed by calling the
 * \see SHNF_ReleaseEplsFrame function.
 *
 * \retval pw_frameLength the number of bytes for the received openSAFETY frame
 * \retval e_telType the @see SHNF_t_TEL_TYPE type of the received openSAFETY frame
 *
 * \return - == NULL - no openSAFETY frame available
 *         - != NULL - pointer to the received openSAFETY frame
 */
UINT8 * SHNF_GetEplsFrame(UINT16 *pw_frameLength, SHNF_t_TEL_TYPE *e_telType);

/**
 * \brief This function releases an EPLS frame which is no longer needed.
 * \see SHNF_GetEplsFrame
 */
void SHNF_ReleaseEplsFrame(void);

/**
 * \brief This function provides cleanup functionality for the network interface.
 */
void SHNF_CleanUp();

#endif /* #ifndef  SHNFDEMO_H */

/** @} */
