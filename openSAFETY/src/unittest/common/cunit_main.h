/*
 * tstmain.h
 *
 *  Created on: 13.12.2012
 *      Author: knallr
 */

#pragma once

#include <stdint.h>

#ifndef bool
typedef uint8_t bool;
#endif

#ifndef TRUE
#define TRUE ((bool) 1)
#define FALSE ((bool) 0)
#endif

bool CU_AllowDebug();

