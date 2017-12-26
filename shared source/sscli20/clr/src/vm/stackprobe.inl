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
//-----------------------------------------------------------------------------
// Stack Probe Header for inline functions
// Used to setup stack guards
//-----------------------------------------------------------------------------
#ifndef __STACKPROBE_inl__
#define __STACKPROBE_inl__

#include "stackprobe.h"
#include "common.h"

#ifndef TOTALLY_DISBLE_STACK_GUARDS

// want to inline in retail, but out of line into stackprobe.cpp in debug
#if !defined(_DEBUG) || defined(INCLUDE_RETAIL_STACK_PROBE)

#ifndef _DEBUG
#define INLINE_NONDEBUG_ONLY FORCEINLINE
#else
#define INLINE_NONDEBUG_ONLY
#endif

INLINE_NONDEBUG_ONLY RestoreSOState::RestoreSOState (Thread *pThread)
{
    m_pThread = pThread;
    m_fSOIntolerantAtEntry = m_pThread && !m_pThread->IsSOTolerant();
}

INLINE_NONDEBUG_ONLY RestoreSOState::~RestoreSOState()
{
    if (m_pThread)
    {
        if (m_fSOIntolerantAtEntry)
        {
            m_pThread->SetThreadStateNC(Thread::TSNC_SOIntolerant);
        }
        else
        {
            m_pThread->ResetThreadStateNC(Thread::TSNC_SOIntolerant);
        }
    }
}

INLINE_NONDEBUG_ONLY BOOL ShouldProbeOnThisThread()
{
    // we only want to probe on user threads, not any of our special threads
    return GetCurrentTaskType() == TT_USER;
}

#ifdef _DEBUG 

void DebugSOTolerantTransitionHandler::EnterSOTolerantCode(Thread *pThread) 
{
    if (pThread)
    {
        m_clrDebugState = pThread->GetClrDebugState();
    }
    else
    {
        m_clrDebugState = GetClrDebugState();
    }
    if (m_clrDebugState)
        m_prevSOTolerantState = m_clrDebugState->BeginSOTolerant();
}

void DebugSOTolerantTransitionHandler::ReturnFromSOTolerantCode()
{
    if (m_clrDebugState)
        m_clrDebugState->SetSOTolerance(m_prevSOTolerantState);
}


#endif

INLINE_NONDEBUG_ONLY 
BOOL RetailStackProbeNoThrow(unsigned int n, Thread *pThread)
{
    STATIC_CONTRACT_NOTHROW;

    if (pThread == NULL)
        return TRUE;

    UINT_PTR probeAddress = (UINT_PTR)(&pThread) - (n * OS_PAGE_SIZE);

    // If the address we want to probe to is beyond the precalculated limit we fail
    // Note that we don't check for stack probing being disabled.  This is encoded in
    // the value returned from GetProbeLimit, which will be 0 if probing is disabled.
    if (probeAddress < pThread->GetProbeLimit())
    {
        // ASSERT that the value we're checking is in fact the right value for the current
        // thread in the face of thread recycling etc.
        _ASSERTE(pThread->GetProbeLimit() == pThread->GetLastNormalStackAddress());
        return FALSE;
    }
    

    return TRUE;
    
}

INLINE_NONDEBUG_ONLY 
void RetailStackProbe(unsigned int n, Thread *pThread)
{
    STATIC_CONTRACT_THROWS;

    if (RetailStackProbeNoThrow(n, pThread))
    {
        return;
    }
    ReportStackOverflow(pThread);
}

INLINE_NONDEBUG_ONLY 
void RetailStackProbe(unsigned int n)
{
    STATIC_CONTRACT_THROWS;

    RetailStackProbe(n, GetThread());
}

#endif
#endif


#endif  // __STACKPROBE_inl__
