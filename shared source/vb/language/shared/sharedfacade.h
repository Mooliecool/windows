//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Precompiled header shared by all projects.
//
//-------------------------------------------------------------------------------------------------

#pragma once

//-------------------------------------------------------------------------------------------------
//
// C++ compiler configuration
//

#if !DEBUG
// We get better compiler perf if we throw these compilation switches.
// It's easier to do this here than in our sources file because
// here we know these settings will take precedence over all others.
#pragma optimize("t", on)

// No measurable performance gain from these:
//  #pragma auto_inline(on)
//  #pragma intrinsic(memcmp, memcpy, memset, strcmp, strcpy, strlen, strcat, _rotl, _rotr, _lrotl, _lrotr)
//  #pragma inline_depth(5)
#endif // !DEBUG

// We're using some custom exception-based error handling.  Disable this warning.
#pragma warning(disable: 4509)

// Level 2 prefast warning that is very noisy in the compiler code base due to
// the way the data structures are setup (i.e. compiler calls members off a NULL BCSYM).
// Prefix will generally catch any real issues
#pragma warning(disable:6011)

// We need the local unwind to execute destructors
#pragma warning(disable:6242)

// We have custom exception handling that may catch all exceptions
#pragma warning(disable:6320)

#ifndef _PREFAST_
#pragma warning(disable:4068)
#endif // _PREFAST_

#define VBAPI __declspec(dllexport)

//-------------------------------------------------------------------------------------------------
//
// #include configuration
//

#define WIN32_LEAN_AND_MEAN
#define _WIN32_DCOM

// Safe string handling routines:
#pragma warning(error:4995)
#define STRSAFE_NO_DEPRECATE

//-------------------------------------------------------------------------------------------------
//
// Language code base configuration settings.
//

// Whether we are using the test harness (enabled in all build flavors).
#define ID_TEST             1

// Collect memory statistics.
#define FV_TRACK_MEMORY     0

// Enable LINQ IntelliSense (
#define LINQ_INTELLISENSE   1


// make it multiple of 8  for IA64/amd64
#define WORD_BYTE_SIZE (sizeof(void*))
#ifdef _WIN64
#define GET_ALIGNED_SIZE(byteSize) ((byteSize+ 7) & ~7)
#else
#define GET_ALIGNED_SIZE(byteSize) ((byteSize+ 3) & ~3)
#endif //Win64

// COMPILE_ASSERT is a compile-time assertion mechanism.  Use any
// compile-time constant as 'expr' and the C++ compiler will complain
// if the expression evaluates to zero.  This works because it is
// an error to declare an array of length zero.
//
// This macro is great for catching logical errors at build time
// instead of run time.  We should probably promote some existing
// assertions into COMPILE_ASSERTs.  The drawback to this macro
// is that the error given by the compiler can be a little cryptic.
//
#if DEBUG
#define COMPILE_ASSERT(expr) extern int __compile_assert[(expr) != 0]
#else
#define COMPILE_ASSERT(expr)
#endif

// Used to signal parameters that are passed by reference
#define __ref 

//-------------------------------------------------------------------------------------------------
//
// Shared component-independent includes.
//

#include <ddcrtsafe.h>
#include <windows.h>
#include <vsassert.h>

// Setup to use VSAssert as our Assert machanism
#if DEBUG
#define _ASSERTE(expr) VSASSERT(expr, "")
#define ASSERT(expr, comment)  VSASSERT(expr, comment)

// for 
#define ATL_CCOMBSTR_ADDRESS_OF_ASSERT
#define _ATL_ALL_WARNINGS
#else
#define _ASSERTE(expr)
#define ASSERT(expr, comment)
#endif


#include <new>
#include "Allocation.h"

#include <functional>
#include <list>
#include <map>
#include <queue>

#include <safeint.h>
#include <specstrings.h>
#include <atldef.h>
#include <atlalloc.h>
#include <atlbase.h>
#include <strsafe.h>

// unpopped #pragma warning(push) in atlbase.h previously left this warning
// disabled.  that #pragma mismatch has been fixed so now disable it here.
#pragma warning(disable: 4291)

// Commonly used items
#include "Macros.h"
#include "Contract.h"
#include "CodeStyle.h"
#include "GuardBase.h"
#include "VBAllocator.h"
#include "VBMath.h"
#include "VBAllocWrapper.h"
#include "TemplateUtil.h"
#include "CComPtrEx.h"
#include "ComUtil.h"
#include "RefCountedPtr.h"
#include "RefCountedData.h"
#include "TinyLock.h"
#include "MapUtil.h"
#include "MultimapUtil.h"

// Iterator interfaces
#include "IConstIterator.h"
#include "IIterator.h"

// Smart Pointer classs
#include "VBHeapPtr.h"
#include "AutoPtr.h"
#include "AutoArray.h"
#include "AutoStackPtr.h"
#include "ArrayBuffer.h"

// Standard Includes
#include "DeclareEnum.h"
#include "ActionKind.h"
#include "BCVirtualAlloc.h"
#include "CollectionIterators.h"
#include "Collections.h"
#include "crc32.h"
#include "crc64.h"
#include "CriticalSection.h"
#include "DynamicArray.h"
#include "PageProtect.h"
#include "PageHeap.h"
#include "NorlsAllocator.h"
#include "NorlsAllocatorManager.h"
#include "NorlsAllocWrapper.h"
#include "PathUtilities.h"
#include "RawStringBuffer.h"
#include "Stack.h"
#include "TriState.h"
#include "Unicode.h"
#include "Utils.h"
#include "DebugUtil.h"
#include "Exception.h"
#include "SehGuard.h"

