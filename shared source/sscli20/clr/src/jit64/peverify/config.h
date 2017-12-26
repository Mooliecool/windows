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

/*++

Module Name: config.h

Abstract:

    Contains the configuration settings that control how the code is
    to be compiled. Should be force-included into all modules (or
    explicitly included in a common header).

--*/

#ifndef __CONFIG__H
#define __CONFIG__H

// Set the host processor
#define INLINE

//
// Defines specific to the IA64-hosted environment
//

#define _SIZE_T_DEFINED

#include <windows.h>
#include <stdio.h>

#define INJITDLL            // used in corjit.h

#include "corjit.h"
#include "malloc.h"         // for _alloca

#define JIT64_BUILD
#include "palclr.h"

extern ICorJitInfo* logCallback;

#ifndef _ASSERTE
#ifdef _DEBUG
#define DbgBreak() 	DebugBreak();

#define _ASSERTE(expr) 		\
        do { if (!(expr) && logCallback->doAssert(__FILE__, __LINE__, #expr)) \
			 DbgBreak(); } while (0)
#else
#define _ASSERTE(expr)  0
#endif
#endif // !_ASSERTE

#endif // __CONFIG__H


