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
// MSCorDBC.cpp
//
// COM+ Debugging Services -- Runtime Controller DLL
//
// Dll* routines for entry points.
//
//*****************************************************************************
#include "stdafx.h"
#include "contract.h"

#ifdef PLATFORM_UNIX


class EEToProfInterface;
class ProfToEEInterface;

extern "C" void __cdecl GetEEToProfInterface(EEToProfInterface **ppEEProf);
extern "C" void __cdecl SetProfToEEInterface(ProfToEEInterface *pProfEE);

void (*g_UNUSEDGetEEToProfInterface)(EEToProfInterface **) = GetEEToProfInterface;
void (*g_UNUSEDSetProfToEEInterface)(ProfToEEInterface *) = SetProfToEEInterface;

#endif // PLATFORM_UNIX

//*****************************************************************************
// The main dll entry point for this module.  This routine is called by the
// OS when the dll gets loaded.  Nothing needs to be done for this DLL.
//*****************************************************************************

HINSTANCE g_hInst = NULL;

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _DEBUG
    DisableThrowCheck();
    //CONTRACT_VIOLATION(ThrowsViolation);
#endif


    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        g_hInst = hInstance;
        break;
    }

	OnUnicodeSystem();
    return TRUE;
}

HINSTANCE GetModuleInst()
{
    return (g_hInst);
}



