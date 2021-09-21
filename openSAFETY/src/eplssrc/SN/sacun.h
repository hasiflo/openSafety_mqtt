/**
 * @addtogroup SN
 * @{
 * @brief This unit provides additional functionality test static variables in Unit-Tests.
 *
 * @file sacun.h
 *
 * This unit provides additional functionality test static variables in Unit-Tests.
 *
 * @copyright Copyright (c) 2009,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H and IXXAT Automation GmbH
 * @copyright All rights reserved, Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 * @copyright This source code is free software; you can redistribute it and/or modify it under the terms of the BSD license (according to License.txt).
 *
 * @author B. Thiemann,  Bernecker + Rainer Industrie-Elektronik Ges.m.b.H
 *
 * @details
 * The definition of the keyword "STATIC" eases the Unit-Tests. It allows access to static variables of the stack.
 */

#ifndef SACUN
#define SACUN

#ifdef __SACUN__
#warning "Warning: CUnit Test activated"
#    define STATIC
#else
#    define STATIC static
#endif

#endif
