// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/*============================================================
**
** File: rc.h
**
** Purpose: Header file for Resource Compiler
**
===========================================================*/

#ifndef __RC_H__
#define __RC_H__

#include <process.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>

typedef struct _STRING_TABLE_ENTRY{
  UINT StringID;
  UINT StringLen;
  LPSTR StringValue;

} STRING_TABLE_ENTRY, *PSTRING_TABLE_ENTRY;

#endif	/* __RC_H__ */
