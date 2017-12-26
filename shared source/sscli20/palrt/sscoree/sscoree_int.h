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

#ifndef __SSCOREE_INT_H__
#define __SSCOREE_INT_H__

#include "rotor_pal.h"

FARPROC
SscoreeShimGetProcAddress (
    ShimmedSym SymIndex,
    LPCSTR      SymName);

ShimmedLib
FindSymbolsLib (
    ShimmedSym SymIndex);

HMODULE
SetupLib (
    ShimmedLib LibIndex);

#ifdef _DEBUG

BOOLEAN SscoreeVerifyLibrary (ShimmedLib LibIndex);
VOID    SscoreeDumpTables ();

#endif // _DEBUG

HRESULT HRESULT_FROM_GetLastError ();

#endif // __SSCOREE_INT_H__
