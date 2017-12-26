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
#ifndef __strike_h__
#define __strike_h__

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof(x[0]))
#endif


#if defined(_MSC_VER)
#pragma warning(disable:4245)   // signed/unsigned mismatch
#pragma warning(disable:4100)   // unreferenced formal parameter
#pragma warning(disable:4201)   // nonstandard extension used : nameless struct/union
#pragma warning(disable:4127)   // conditional expression is constant
#endif


#include <rpc.h>
#include <unknwn.h>

// ROTORTODO - don't know if we want this to be permanent:
#define __uuidof(x) IID_ ## x
#define UNREFERENCED_PARAMETER(x)
    
#include <wchar.h>
//#include <heap.h>
//#include <ntsdexts.h>

#include <windows.h>

//#define NOEXTAPI
#define KDEXT_64BIT
#include <wdbgexts.h>
#undef DECLARE_API
#undef GetContext
#undef SetContext

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <malloc.h>
#include <stddef.h>


#define  CORHANDLE_MASK 0x1

// C_ASSERT() can be used to perform many compile-time assertions:
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

// portable exception handling

#include "exts.h"

extern BOOL CallStatus;

#endif // __strike_h__

