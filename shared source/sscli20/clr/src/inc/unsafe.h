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

#ifndef __UNSAFE_H__
#define __UNSAFE_H__

// should we just check proper inclusion?
#include <winwrap.h>
#include "staticcontract.h"

inline VOID UnsafeEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    STATIC_CONTRACT_LEAF;
    EnterCriticalSection(lpCriticalSection);
}

inline VOID UnsafeLeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    STATIC_CONTRACT_LEAF;
    LeaveCriticalSection(lpCriticalSection);
}

inline VOID UnsafeInitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    STATIC_CONTRACT_LEAF;
    InitializeCriticalSection(lpCriticalSection);
}

inline VOID UnsafeDeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    STATIC_CONTRACT_LEAF;
    DeleteCriticalSection(lpCriticalSection);
}

inline HANDLE UnsafeCreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName)
{
    STATIC_CONTRACT_WRAPPER;
    return WszCreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);
}

inline BOOL UnsafeSetEvent(HANDLE hEvent)
{
    STATIC_CONTRACT_LEAF;
    return SetEvent(hEvent);
}

inline BOOL UnsafeResetEvent(HANDLE hEvent)
{
    STATIC_CONTRACT_LEAF;
    return ResetEvent(hEvent);
}

inline HANDLE UnsafeCreateSemaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpName)
{
    STATIC_CONTRACT_WRAPPER;
    return WszCreateSemaphore(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);
}

inline BOOL UnsafeReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount)
{
    STATIC_CONTRACT_LEAF;
    return ReleaseSemaphore(hSemaphore, lReleaseCount, lpPreviousCount);
}

inline LPVOID UnsafeTlsGetValue(DWORD dwTlsIndex)
{
    STATIC_CONTRACT_LEAF;
    return TlsGetValue(dwTlsIndex);
}

inline BOOL UnsafeTlsSetValue(DWORD dwTlsIndex, LPVOID lpTlsValue)
{
    STATIC_CONTRACT_LEAF;
    return TlsSetValue(dwTlsIndex, lpTlsValue);
}

inline DWORD UnsafeTlsAlloc(void) 
{
    STATIC_CONTRACT_LEAF;
    return TlsAlloc();
}

inline BOOL UnsafeTlsFree(DWORD dwTlsIndex) 
{
    STATIC_CONTRACT_LEAF;
    return TlsFree(dwTlsIndex);
}

#endif


