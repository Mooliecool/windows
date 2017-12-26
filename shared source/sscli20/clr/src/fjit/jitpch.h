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

#if defined(_PPC_) || defined(_SPARC_)  // This is for testing purposes only  
#undef _X86_
#endif

#define FEATURE_NO_HOST

#include <windows.h>

#include "corjit.h"
#include "malloc.h"         // for _alloca

extern ICorJitInfo* logCallback;

#ifdef _DEBUG
#if defined(_X86_) && defined(_MSC_VER)
#define DbgBreak() 	__asm { int 3 }		// This is nicer as it breaks at the assert code
#else
#define DbgBreak() 	DebugBreak();
#endif

#ifndef _ASSERTE
#define _ASSERTE(expr) 		\
        do { if (!(expr) && logCallback->doAssert(__FILE__, __LINE__, #expr)) \
			 DbgBreak(); } while (0)
#endif

#else

#ifndef _ASSERTE
#define _ASSERTE(expr)  0
#endif

#endif

#ifdef _DEBUG
#include "utilcode.h"		// for Config* classes
#define LOGGING
#endif

#include "palclr.h" 		// for PAL_TRY/PAL_EXCEPT
