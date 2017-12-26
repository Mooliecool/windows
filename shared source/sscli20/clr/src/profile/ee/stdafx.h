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

// Since we're compiling profiler interface implementations here, we don't want
// RuntimeThreadSuspended to get #defined as a macro warning people to use
// RuntimeTheadSuspendedWrapper (that should only happen when compiling
// code that consumes the interfaces).
// (See RuntimeTheadSuspendedWrapper in threads.h for more information.)
#define PREVENT_RUNTIMETHREADSUSPENDED_REDEF

#include <stdio.h>

#include <windows.h>
#include <winwrap.h>
#include <sem.h>
#include <cor.h>
#include <corerror.h>
#include <utilcode.h>

#ifdef _DEBUG
#define LOGGING
#endif

#include <log.h>

#include "profile.h"
#include "corprof.h"

#ifdef _DEBUG

#define RELEASE(iptr)               \
    {                               \
        _ASSERTE(iptr);             \
        iptr->Release();            \
        iptr = NULL;                \
    }

#define VERIFY(stmt) _ASSERTE((stmt))

#else

#define RELEASE(iptr)               \
    iptr->Release();

#define VERIFY(stmt) (stmt)

#endif

