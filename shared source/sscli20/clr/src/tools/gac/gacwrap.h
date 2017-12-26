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
// GacWrap.h
//
// This file contains wrapper functions for Win32 API's that take strings.
//
//*****************************************************************************

//********** Macros. **********************************************************

#define HIWORD64        HIWORD

#include <windows.h>
#include <assert.h>
#include <cor.h>

#define SAFEDELETEARRAY(p) if ((p) != NULL) { delete [] p; (p) = NULL; }
#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

#ifndef offsetof
#define offsetof(s,m)   ((size_t)&(((s*)0)->m))
#endif

BOOL OnUnicodeSystem();


#define WszFormatMessage FormatMessageW
#define WszCreateFile CreateFileW
#define WszGetFullPathName  GetFullPathNameW
#define WszLoadString   PAL_LoadSatelliteStringW


#define MSG_FOR_URT_HR(hr) (0x6000 + (HRESULT_CODE(hr)))
#define MAX_URT_HRESULT_CODE 0x3000

