/*============================================================
**
** Header: error.h
**
** Purpose: Header file for test harness error functions.
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

#ifndef _ERROR_H
#define _ERROR_H

#include <stdarg.h>


void HarnessMessage(const char *Format, ...);
void HarnessError(const char *Format, ...);

#endif /* _ERROR_H */

