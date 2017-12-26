/*============================================================
**
** Source: error.c
**
** Purpose: Error functions.
**
** 
**  
**   Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
**  
**   The use and distribution terms for this software are contained in the file
**   named license.txt, which can be found in the root of this distribution.
**   By using this software in any fashion, you are agreeing to be bound by the
**   terms of this license.
**  
**   You must not remove this notice, or any other, from this software.
**  
** 
**
**=========================================================*/

#include "testharness.h"
#include "error.h"

/*
 * Generic printf style message.  Prints to stdout.
 */
void HarnessMessage(const char *Format, ...)
{
    va_list ap;

    va_start(ap, Format);
    vfprintf(stdout, Format, ap);
    va_end(ap);
} 

/*
 * Generic printf style message to stderr, followed by exit(1).
 */
void HarnessError(const char * Format, ...)
{
    va_list ap;

    va_start(ap, Format);
    vfprintf(stderr, Format, ap);
    va_end(ap);

    exit(1);
}
