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
#include "SNMTSapi.h"

BOOLEAN StartMqttClient();
void mqttSend(const UINT8 * c_sendBuffer, UINT32 i_sendBufferLength);
void sendState(SNMTS_t_SN_STATE_MAIN e_actSnState);

#endif /* #ifndef  SHNFDEMO_H */

/** @} */
