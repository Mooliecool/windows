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
// File: stdafx.h
//  include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
// ===========================================================================

#if !defined(AFX_STDAFX_H__1FB1B429_B183_11D2_88B7_00C04F990355__INCLUDED_)
#define AFX_STDAFX_H__1FB1B429_B183_11D2_88B7_00C04F990355__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_DCOM

//#define TRACKMEM


// Reference additional headers your program requires here
#include <windows.h>

#include <vsassert.h>

#ifndef ASSERT
#define ASSERT(x) VSASSERT(x, "")
#endif

#undef VSAlloc
#undef VSAllocZero
#undef VSRealloc
#undef VSFree
#undef VSSize


#define VSAlloc(cb)       HeapAlloc(GetProcessHeap(), 0, cb)
#define VSAllocZero(cb)   HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cb)
#define VSRealloc(pv, cb) HeapReAlloc(GetProcessHeap(), 0, pv, cb)
#define VSFree(pv)        HeapFree(GetProcessHeap(), 0, pv)
#define VSSize(pv)        HeapSize(GetProcessHeap(), 0, pv)


#ifndef IfFailRet
#define IfFailRet(EXPR) do { hr = (EXPR); if(FAILED(hr)) return (hr); } while (0)
#endif
// Define lengthof macro - length of an array.
#define lengthof(a) (sizeof(a) / sizeof((a)[0]))


#ifndef CP_UNICODE
#define CP_UNICODE          1200 // Unicode
#endif

#ifndef CP_WINUNICODE
#define CP_WINUNICODE       CP_UNICODE
#endif

#define CP_UNICODESWAP      1201 // Unicode Big-Endian

// portable 64-bit constants
#ifndef I64
#define I64(x)    x ## i64
#endif
#ifndef UI64
#define UI64(x)   x ## ui64
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include <strsafe.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // !defined(AFX_STDAFX_H__1FB1B429_B183_11D2_88B7_00C04F990355__INCLUDED_)
