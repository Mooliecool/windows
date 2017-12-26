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
// File: cotask.cpp
// 
// ===========================================================================

#include "rotor_palrt.h"

STDAPI_(LPVOID) CoTaskMemAlloc(SIZE_T cb)
{
    return LocalAlloc(LMEM_FIXED, cb);
}

STDAPI_(void) CoTaskMemFree(LPVOID pv)
{
    LocalFree(pv);
}
