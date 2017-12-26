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
/*============================================================
**
** Header:  Threads.inl
**
** Purpose: Implements Thread inline functions
**
** Date:  August 7, 2000
**
===========================================================*/
#ifndef _THREADS_INL
#define _THREADS_INL

#include "threads.h"
#include "appdomain.hpp"

inline void Thread::IncLockCount()
{
    LEAF_CONTRACT;
    _ASSERTE (GetThread() == this);
    m_dwLockCount ++;
    _ASSERTE (m_dwLockCount != 0 || GetDomain()->OkToIgnoreOrphanedLocks());
}

inline void Thread::DecLockCount()
{
    LEAF_CONTRACT;
    _ASSERTE (GetThread() == this);
    _ASSERTE (m_dwLockCount > 0 || GetDomain()->OkToIgnoreOrphanedLocks());
    m_dwLockCount --;
}

inline
Frame* Thread::FindFrame(SIZE_T StackPointer)
{
    Frame* pFrame = GetFrame();

    while ((SIZE_T)pFrame < StackPointer)
    {
        pFrame = pFrame->Next();
    }

    return pFrame;
}

inline void Thread::SetThrowable(OBJECTREF pThrowable DEBUG_ARG(ThreadExceptionState::SetThrowableErrorChecking stecFlags))
{
    WRAPPER_CONTRACT;
    
    m_ExceptionState.SetThrowable(pThrowable DEBUG_ARG(stecFlags));
}

inline void Thread::SetKickOffDomainId(ADID ad)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    m_pKickOffDomainId = ad;
}


inline ADID Thread::GetKickOffDomainId()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE(m_pKickOffDomainId.m_dwId != 0);
    return m_pKickOffDomainId;
}


inline OBJECTREF Thread::GetExposedObjectRaw()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return ObjectFromHandle(m_ExposedObject);
}

inline void Thread::FinishSOWork()
{
    WRAPPER_CONTRACT;

    if (HasThreadStateNC(TSNC_SOWorkNeeded))
    {
        ResetThreadStateNC(TSNC_SOWorkNeeded);
        // Wake up AD unload thread to finish SO work that is delayed due to limit stack
        AppDomain::EnableADUnloadWorkerForThreadAbort();
    }
}

inline DWORD Thread::IncrementOverridesCount()
{
    WRAPPER_CONTRACT;
    return m_ADStack.IncrementOverridesCount();
}

inline DWORD Thread::DecrementOverridesCount()
{
    WRAPPER_CONTRACT;
    return m_ADStack.DecrementOverridesCount();
}

inline DWORD Thread::GetOverridesCount()
{
    WRAPPER_CONTRACT;
    return m_ADStack.GetOverridesCount();
}

inline DWORD Thread::IncrementAssertCount()
{
    WRAPPER_CONTRACT;
    return m_ADStack.IncrementAssertCount();
}

inline DWORD Thread::DecrementAssertCount()
{
    WRAPPER_CONTRACT;
    return m_ADStack.DecrementAssertCount();
}

inline DWORD Thread::GetAssertCount()
{
    LEAF_CONTRACT;
    return m_ADStack.GetAssertCount();
}

#ifndef DACCESS_COMPILE
inline void Thread::PushDomain(ADID pDomain)
{
    WRAPPER_CONTRACT;
    m_ADStack.PushDomain(pDomain);
}

inline ADID Thread::PopDomain()
{
    WRAPPER_CONTRACT;
    return m_ADStack.PopDomain();
}
#endif // DACCESS_COMPILE

inline DWORD Thread::GetNumAppDomainsOnThread()
{
    WRAPPER_CONTRACT;
    return m_ADStack.GetNumDomains();
}

inline BOOL Thread::CheckThreadWideSpecialFlag(DWORD flags)
{
    WRAPPER_CONTRACT;
    return m_ADStack.GetThreadWideSpecialFlag() & flags;
}

inline void Thread::InitDomainIteration(DWORD *pIndex)
{
    WRAPPER_CONTRACT;
    m_ADStack.InitDomainIteration(pIndex);
}

inline ADID Thread::GetNextDomainOnStack(DWORD *pIndex, DWORD *pOverrides, DWORD *pAsserts)
{
    WRAPPER_CONTRACT;
    return m_ADStack.GetNextDomainOnStack(pIndex, pOverrides, pAsserts);
}

inline void Thread::UpdateDomainOnStack(DWORD pIndex, DWORD asserts, DWORD overrides)
{
    WRAPPER_CONTRACT;
    return m_ADStack.UpdateDomainOnStack(pIndex, asserts, overrides);
}

#include "appdomainstack.inl"

#endif
