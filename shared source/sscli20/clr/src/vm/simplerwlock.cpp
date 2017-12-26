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
#include "common.h"
#include "simplerwlock.hpp"

BOOL SimpleRWLock::TryEnterRead()
{

    LEAF_CONTRACT;

#ifdef _DEBUG
    if (m_gcMode == PREEMPTIVE)
        _ASSERTE(!GetThread() || !GetThread()->PreemptiveGCDisabled());
    else if (m_gcMode == COOPERATIVE)
        _ASSERTE(!GetThread() || GetThread()->PreemptiveGCDisabled());
#endif

    LONG RWLock;

    do {
        RWLock = m_RWLock;
        if( RWLock == -1 ) return FALSE;
        _ASSERTE (RWLock >= 0);
    } while( RWLock != InterlockedCompareExchange( &m_RWLock, RWLock+1, RWLock ));

    INCTHREADLOCKCOUNT();
    
    return TRUE;
}

//=====================================================================        
void SimpleRWLock::EnterRead()
{
    LEAF_CONTRACT;

#ifdef _DEBUG
    if (m_gcMode == PREEMPTIVE)
        _ASSERTE(!GetThread() || !GetThread()->PreemptiveGCDisabled());
    else if (m_gcMode == COOPERATIVE)
        _ASSERTE(!GetThread() || GetThread()->PreemptiveGCDisabled());
#endif

    while (TRUE)
    {
        // prevent writers from being starved. This assumes that writers are rare and 
        // dont hold the lock for a long time. 
        while (IsWriterWaiting())
        {
            int spinCount = m_spinCount;
            while (spinCount > 0) {
                spinCount--;
                YieldProcessor();
            }
            __SwitchToThread(0);
        }

        if (TryEnterRead())
        {
            return;
        }

        DWORD i = 50;
        do
        {
            if (TryEnterRead())
            {
                return;
            }

            if (g_SystemInfo.dwNumberOfProcessors <= 1)
            {
                break;
            }
            int sum = 0;
            for (int delayCount = i; --delayCount; ) 
            {
                sum += delayCount;
                YieldProcessor();           // indicate to the processor that we are spining 
            }
            if (sum == 0)
            {
                // never executed, just to fool the compiler into thinking sum is live here,
                // so that it won't optimize away the loop.
                static char dummy;
                dummy++;
            }
            // exponential backoff: wait 3 times as long in the next iteration
            i = i*3;
        }
        while (i < 20000*g_SystemInfo.dwNumberOfProcessors);

        extern BOOL __SwitchToThread (DWORD dwSleepMSec);
        __SwitchToThread(0);
    }
}

//=====================================================================        
BOOL SimpleRWLock::TryEnterWrite()
{
    LEAF_CONTRACT;

#ifdef _DEBUG
    if (m_gcMode == PREEMPTIVE)
        _ASSERTE(!GetThread() || !GetThread()->PreemptiveGCDisabled());
    else if (m_gcMode == COOPERATIVE)
        _ASSERTE(!GetThread() || GetThread()->PreemptiveGCDisabled());
#endif

    LONG RWLock = InterlockedCompareExchange( &m_RWLock, -1, 0 );

    _ASSERTE (RWLock >= 0 || RWLock == -1);
    
    if( RWLock ) {
        return FALSE;
    }
    
    INCTHREADLOCKCOUNT();
    
    ResetWriterWaiting();
    
    return TRUE;
}

//=====================================================================        
void SimpleRWLock::EnterWrite()
{
    LEAF_CONTRACT;

    #ifdef _DEBUG
    if (m_gcMode == PREEMPTIVE)
        _ASSERTE(!GetThread() || !GetThread()->PreemptiveGCDisabled());
    else if (m_gcMode == COOPERATIVE)
        _ASSERTE(!GetThread() || GetThread()->PreemptiveGCDisabled());
#endif

    BOOL set = FALSE;

    while (TRUE)
    {
        if (TryEnterWrite())
        {
            return;
        }

        // set the writer waiting word, if not already set, to notify potential
        // readers to wait. Remember, if the word is set, so it can be reset later.
        if (!IsWriterWaiting())
        {
            SetWriterWaiting();
            set = TRUE;
        }

        DWORD i = 50;
        do
        {
            if (TryEnterWrite())
            {
                return;
            }

            if (g_SystemInfo.dwNumberOfProcessors <= 1)
            {
                break;
            }
            int sum = 0;
            for (int delayCount = i; --delayCount; ) 
            {
                sum += delayCount;
                YieldProcessor();           // indicate to the processor that we are spining 
            }
            if (sum == 0)
            {
                // never executed, just to fool the compiler into thinking sum is live here,
                // so that it won't optimize away the loop.
                static char dummy;
                dummy++;
            }
            // exponential backoff: wait 3 times as long in the next iteration
            i = i*3;
        }
        while (i < 20000*g_SystemInfo.dwNumberOfProcessors);

        extern BOOL __SwitchToThread (DWORD dwSleepMSec);
        __SwitchToThread(0);
    }
}

