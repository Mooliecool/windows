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
//*****************************************************************************
// stdafx.h
//
// Common include file for utility code.
//*****************************************************************************
#include <windows.h>
#include <crtdbg.h>
#include <malloc.h>

#include "debuggerutil.h"
#include "dshell.h"

#define MAX_CLASSNAME_LENGTH    1024

#ifdef _DEBUG

#define RELEASE(iptr)               \
    {                               \
        _ASSERTE(iptr);             \
        iptr->Release();            \
        iptr = NULL;                \
    }

#define VERIFY(stmt) _ASSERTE((stmt))

#else

// Since we null the pointer in debug, we have to do it in release.
#define RELEASE(iptr)               \
    iptr->Release();                \
    iptr = NULL;

#define VERIFY(stmt) (stmt)

#endif

