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
#ifndef __SEM_H__
#define __SEM_H__

#include <windows.h>
#include "corhlpr.h"

// -------------------------------------------------------------
//                  GLOBAL HELPER FUNCTIONS
// -------------------------------------------------------------


/* ----------------------------------------------------------------------------
@class CSemExclusive:

    An instance of this class represents an exclusive lock. If one thread calls
    Lock(), it will wait until any other thread that has called Lock() calls
    Unlock().

    A thread MAY call Lock multiple times.  It needs to call Unlock a matching
    number of times before the object is available to other threads.

---------------------------------------------------------------------------- */
class CSemExclusive
{
public:
    // Parameter to constructor is the maximum number of times to spin when
    // attempting to acquire the lock. The thread will sleep if the lock does
    // not become available in this period.
    CSemExclusive (DWORD ulcSpinCount = 0);
#undef DeleteCriticalSection
#undef EnterCriticalSection
#undef LeaveCriticalSection
    ~CSemExclusive (void)           { DeleteCriticalSection (&m_csx); }
    void Lock (void)                { EnterCriticalSection (&m_csx); _ASSERTE(++m_iLocks > 0);}
    void UnLock (void)              { _ASSERTE(--m_iLocks >= 0);  LeaveCriticalSection (&m_csx); }
#ifdef Dont_Use_DeleteCriticalSection
#define DeleteCriticalSection   Dont_Use_DeleteCriticalSection
#endif
#ifdef Dont_Use_EnterCriticalSection
#define EnterCriticalSection    Dont_Use_EnterCriticalSection
#endif
#ifdef Dont_Use_LeaveCriticalSection
#define LeaveCriticalSection    Dont_Use_LeaveCriticalSection
#endif

#ifdef _DEBUG
    int IsLocked()                  { return (m_iLocks > 0); }
#endif

private:
    CRITICAL_SECTION m_csx;
#ifdef _DEBUG
    int             m_iLocks;           // Count of locks.
#endif
};  //end class CSemExclusive


#endif // __SEM_H__
