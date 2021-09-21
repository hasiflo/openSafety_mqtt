/**
 *
 * \addtogroup Tools
 * \{
 * \defgroup oscrccheck openSAFETY CRC check utility
 * \{
 *
 * \brief Build process helper tool for ensuring the CRC checksum on each stack file.
 *
 * The oscrccheck utility may be used in one of two ways:
 *
 * - To check a file crc against a pre-defined checksum file
 * - To calculate the crc of a given file
 *
 * It is being used to ensure the pre-certification of stack sources, where the checksum
 * of a given stack source is checked against a pre-calculated value. The algorithm uses
 * the \ref CRC32_POLY for securing the files correctness.
 *
 * \see crc32Checksum
 * \see CRC32_POLY
 *
 * \file oscrccheck.h Main header definitions for the oscrccheck utility
 *
 * \copyright Copyright (c) 20011,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * \copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * \author Roland Knall, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 */

#ifndef OSCRCCHECK_H_
#define OSCRCCHECK_H_

#include <stdint.h>
#include <oscrccheckint.h>

#endif /* OSCRCCHECK_H_ */
/**
 * \}
 * \}
 */
