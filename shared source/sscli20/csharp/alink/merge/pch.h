// ==++==
// 
//  
//   Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//  
//   The use and distribution terms for this software are contained in the file
//   named license.txt, which can be found in the root of this distribution.
//   By using this software in any fashion, you are agreeing to be bound by the
//   terms of this license.
//  
//   You must not remove this notice, or any other, from this software.
//  
// 
// ==--==
// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__ADDBC8E1_02FF_4CB4_9306_3AD3DAE21F7F__INCLUDED_)
#define AFX_STDAFX_H__ADDBC8E1_02FF_4CB4_9306_3AD3DAE21F7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <atl.h>

#include <vsassert.h>

#define COR_ILEXCEPTION_OFFSETLEN_SUPPORTED 1
#define _META_DATA_NO_SCOPE_ 1
#include <cor.h>
#include <corsym.h>
#include <iceefilegen.h>
#include <strongname.h>
#include <mscoree.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include <strsafe.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#define ASSERT(expr) VSASSERT((expr), #expr)

#undef VSReallocZero
#define VSReallocZero not_portable

// Define lengthof macro - length of an array.
#define lengthof(a) (sizeof(a) / sizeof((a)[0]))

// portable helpers for access to unaligned pointers
#ifdef _MSC_VER
// Get unaligned values from a potentially unaligned object
#define GET_UNALIGNED_16(_pObject)  (*(UINT16 UNALIGNED *)(_pObject))
#define GET_UNALIGNED_32(_pObject)  (*(UINT32 UNALIGNED *)(_pObject))
#define GET_UNALIGNED_64(_pObject)  (*(UINT64 UNALIGNED *)(_pObject))

// Set Value on an potentially unaligned object 
#define SET_UNALIGNED_16(_pObject, _Value)  (*(UNALIGNED UINT16 *)(_pObject)) = (UINT16)(_Value)
#define SET_UNALIGNED_32(_pObject, _Value)  (*(UNALIGNED UINT32 *)(_pObject)) = (UINT32)(_Value)
#define SET_UNALIGNED_64(_pObject, _Value)  (*(UNALIGNED UINT64 *)(_pObject)) = (UINT64)(_Value) 
#endif

// helpers for bigendian swapping
#if !BIGENDIAN

#define VAL16(x) x
#define VAL32(x) x
#define VAL64(x) x

#define GET_UNALIGNED_VAL16(_pObject) VAL16(GET_UNALIGNED_16(_pObject))
#define GET_UNALIGNED_VAL32(_pObject) VAL32(GET_UNALIGNED_32(_pObject))
#define GET_UNALIGNED_VAL64(_pObject) VAL64(GET_UNALIGNED_64(_pObject))

#define SET_UNALIGNED_VAL16(_pObject, _Value) SET_UNALIGNED_16(_pObject, VAL16((UINT16)_Value))
#define SET_UNALIGNED_VAL32(_pObject, _Value) SET_UNALIGNED_32(_pObject, VAL32((UINT32)_Value))
#define SET_UNALIGNED_VAL64(_pObject, _Value) SET_UNALIGNED_64(_pObject, VAL64((UINT64)_Value))

#endif

#include "array.h"
#include "list.h"
#include "tree.h"

#endif // !defined(AFX_STDAFX_H__ADDBC8E1_02FF_4CB4_9306_3AD3DAE21F7F__INCLUDED)
