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
// ===========================================================================
// File: JITinterfaceGen.CPP
//
// ===========================================================================

// This contains generic C versions of some of the routines
// required by JITinterface.cpp. They are modeled after
// X86 specific routines found in JIThelp.asm or JITinterfaceX86.cpp
// More and more we're making AMD64 and IA64 specific versions of
// the helpers as well, JitInterfaceGen.cpp sticks around for rotor...

#include "common.h"
#include "clrtypes.h"
#include "jitinterface.h"
#include "eeconfig.h"
#include "excep.h"
#include "comdelegate.h"
#include "remoting.h" // create context bound and remote class instances
#include "field.h"
#include "gcscan.h"
#include "ecall.h"





/*********************************************************************/ 
// Initialize the part of the JIT helpers that require very little of
// EE infrastructure to be in place.
/*********************************************************************/
#ifndef _X86_


#define __ARRAYSIZE(x) (sizeof(x)/sizeof(*x))



#undef __ARRAYSIZE


void InitJITHelpers1()
{
    LEAF_CONTRACT;

    // Init GetThread function
    _ASSERTE(GetThread != NULL);

#if USE_INDIRECT_GET_THREAD_APPDOMAIN
    SetJitHelperFunction(CORINFO_HELP_GET_THREAD, (void *) GetThread);
#endif

    _ASSERTE(g_SystemInfo.dwNumberOfProcessors != 0);

}



#endif // !_X86_
