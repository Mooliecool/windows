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
// MSCorDBI.cpp
//
// COM+ Debugging Services -- Debugger Interface DLL
//
// Dll* routines for entry points, and support for COM framework.  
//
//*****************************************************************************
#include "stdafx.h"

extern BOOL STDMETHODCALLTYPE DbgDllMain(HINSTANCE hInstance, DWORD dwReason,
                                         LPVOID lpReserved);

//*****************************************************************************
// The main dll entry point for this module.  This routine is called by the
// OS when the dll gets loaded.  Control is simply deferred to the main code.
//*****************************************************************************
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{

	// Defer to the main debugging code.
    return DbgDllMain(hInstance, dwReason, lpReserved);
}
