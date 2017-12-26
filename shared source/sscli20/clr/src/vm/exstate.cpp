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
#include "exstate.h"
#include "exinfo.h"


#ifdef _DEBUG
#include "comutilnative.h"      // for assertions only
#endif

OBJECTHANDLE ThreadExceptionState::GetThrowableAsHandle()
{
    WRAPPER_CONTRACT;
    
    return m_currentExInfo.m_hThrowable;
}


ThreadExceptionState::ThreadExceptionState()
{

    #if defined(PRESERVE_WATSON_ACROSS_CONTEXTS) // Watson isn't implemented on Rotor or DAC
    // Set the tag to "empty", and the address to 0.
    m_UnhandledInfoTag = UHI_empty;
    m_UnhandledInfo.m_UnhandledIp = 0;
    #endif // rotor or dac

    m_flag = TEF_None;
}

ThreadExceptionState::~ThreadExceptionState()
{
    #if defined(PRESERVE_WATSON_ACROSS_CONTEXTS) // Watson isn't implemented on Rotor or DAC
    // If there are any stashed Watson buckets, free them now.
    ClearUnhandledInfoForWatson();
    #endif // rotor or dac
}

#if defined(PRESERVE_WATSON_ACROSS_CONTEXTS) // Watson isn't implemented on Rotor or DAC
//------------------------------------------------------------------------------
// Description
//   These functions managed information about an exception that may go unhandled.
//     Of course, any exception may go unhandled, meaning that these manage some
//     information about every exception.  In particular, they keep track of the
//     address of a managed exception, even after the exception has been caught.
//     Now, the reason that an "unhandled" exception can be "caught" is that it may
//     be thrown in one app domain, caught and rethrown at the appdomain boundary,
//     and go unhandled in the original appdomain.  At the point that the exception
//     is caught at the appdomain boundary, we lose the original exception information,
//     at least in the thrown exception.  So we keep just a bit of info "off to the
//     side" so that if the exception goes unhandled, we can build better
//     bucket parameters for Watson.
//     A similar thing happens when code catches and rethrows an exception.
//
//  Functions:
// 
//  CaptureUnhandledInfoForWatson
//     Converts a raw IP to watson buckets.  We do this when crossing an appdomain
//     boundary, because the original appdomain may be getting unloaded.  This
//     will cause memory allocation.
//
//  ClearUnhandledInfoForWatson
//     Cleans up.  If any memory was allocated, it gets freed here.
// 
//  RetrieveUnhandldInfoForWatson
//     If the unhandled info has been converted to watson buckets, return
//     the buckets.  Otherwise, return NULL.
//
//  SaveIpForUnhandledInfo
//     Save the IP of a managed exception, if there is not one already saved.
//
//  RetrieveIpForUnhandledInfo
//     If the unhandled info is still an IP, return it.  Otherwise, return NULL.
//------------------------------------------------------------------------------
void ThreadExceptionState::CaptureUnhandledInfoForWatson()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Only capture if there is an address.
    if ((m_UnhandledInfoTag == UHI_IP) && (m_UnhandledInfo.m_UnhandledIp != 0))
    {
        void * pBuckets = GetBucketParametersForManagedException(m_UnhandledInfo.m_UnhandledIp);
        LOG((LF_EH, LL_INFO1000, "ThreadExceptionState::CaptureUnhandledInfoForWatson (%p, %p)\n", this, pBuckets));

        // If we got something back, great.  But if NULL, then we never will get anything back.  So, either way,
        //  save what we got, and flip the tag.
        m_UnhandledInfoTag = UHI_Buckets;
        m_UnhandledInfo.m_pUnhandledBuckets = pBuckets;
    }

} // void ThreadExceptionState::CaptureUnhandledInfoForWatson()

void ThreadExceptionState::ClearUnhandledInfoForWatson()
{
    WRAPPER_CONTRACT;

    // If the union holds Watson buckets, clear them.
    if ((m_UnhandledInfoTag == UHI_Buckets) && (m_UnhandledInfo.m_pUnhandledBuckets != NULL))
    {
        LOG((LF_EH, LL_INFO1000, "ThreadExceptionState::ClearUnhandledInfoForWatson (%p, %p)\n", this, m_UnhandledInfo.m_pUnhandledBuckets));
        FreeBucketParametersForManagedException(m_UnhandledInfo.m_pUnhandledBuckets);
    }

    m_UnhandledInfoTag = UHI_empty;
    m_UnhandledInfo.m_UnhandledIp = 0;
} // void ThreadExceptionState::ClearUnhandledInfoForWatson()

void ThreadExceptionState::SaveIpForUnhandledInfo(
    UINT_PTR    ip)                     // The new IP.
{
    WRAPPER_CONTRACT;

    // If there are already buckets, or a non-zero address, don't save anything now.
    if ((m_UnhandledInfoTag == UHI_Buckets) || (m_UnhandledInfo.m_UnhandledIp != 0))
    {
        LOG((LF_EH, LL_INFO10000, "ThreadExceptionState::SaveIpForUnhandledInfo - already set, not saving (%p, %p, tag:%d, new ip:%p)\n", 
             this, m_UnhandledInfo.m_UnhandledIp, m_UnhandledInfoTag, ip));
        return;
    }
    
    LOG((LF_EH, LL_INFO1000, "ThreadExceptionState::SaveIpForUnhandledInfo (%p, %p)\n", this, ip));
    
    m_UnhandledInfoTag = UHI_IP;
    m_UnhandledInfo.m_UnhandledIp = ip;

} // void ThreadExceptionState::SaveIpForUnhandledInfo()

UINT_PTR ThreadExceptionState::RetrieveIpForUnhandledInfo()
{
    LEAF_CONTRACT;

    LOG((LF_EH, LL_INFO1000, "ThreadExceptionState::RetrieveIpForUnhandledInfo (%p, %p, tag:%d)\n", this, m_UnhandledInfo.m_pUnhandledBuckets, m_UnhandledInfoTag));
    
    // If the union holds an IP return it; otherwise return NULL.
    return (m_UnhandledInfoTag == UHI_IP) ? m_UnhandledInfo.m_UnhandledIp : NULL;
} // UINT_PTR ThreadExceptionState::RetrieveIpForUnhandledInfo()

#if defined(_DEBUG)
void ThreadExceptionState::AssertStackTraceInfo(StackTraceInfo *pSTI)
{
    LEAF_CONTRACT;

    _ASSERTE(pSTI == &(m_currentExInfo.m_StackTraceInfo));

} // void ThreadExceptionState::AssertStackTraceInfo()
#endif // _debug

#endif // rotor or dac


void ThreadExceptionState::MaybeEagerlyCaptureUnhandledInfoForWatson()
{
#if defined(PRESERVE_WATSON_ACROSS_CONTEXTS) // Watson isn't implemented on Rotor or DAC
    static int bEager = -1;
    if (bEager == -1) 
    {   // Read the switch one time only
        bEager = REGUTIL::GetConfigDWORD(L"EagerWatsonBuckets", 0);
    }
    if (bEager)
    {   // If configured to eagerly capture watson info, so so now.  Generally used in
        //  stress labs so that !WatsonBuckets will show where a mini-dump would have gone.
        CaptureUnhandledInfoForWatson();
    }
#endif
} // void ThreadExceptionState::MaybeEagerlyCaptureUnhandledInfoForWatson()


#ifndef DACCESS_COMPILE

Thread* ThreadExceptionState::GetMyThread()
{
    return (Thread*)(((BYTE*)this) - offsetof(Thread, m_ExceptionState));
}


void ThreadExceptionState::FreeAllStackTraces()
{
    WRAPPER_CONTRACT;

    ExInfo*           pNode = &m_currentExInfo;

    for ( ;
          pNode != NULL;
          pNode = pNode->m_pPrevNestedInfo)
    {
        pNode->m_StackTraceInfo.FreeStackTrace();
    }
}

void ThreadExceptionState::ClearThrowablesForUnload(HandleTableBucket* pHndTblBucket)
{
    WRAPPER_CONTRACT;

    ExInfo*           pNode = &m_currentExInfo;

    for ( ;
          pNode != NULL;
          pNode = pNode->m_pPrevNestedInfo)
    {
        if (pHndTblBucket->Contains(pNode->m_hThrowable))
        {
            pNode->DestroyExceptionHandle();
        }
    }
}


// After unwinding from an SO, there may be stale exception state.
void ThreadExceptionState::ClearExceptionStateAfterSO(void* pStackFrameSP)
{
    WRAPPER_CONTRACT;

        // 
        //
        m_currentExInfo.Init();
} // void ThreadExceptionState::ClearExceptionStateAfterSO()

OBJECTREF ThreadExceptionState::GetThrowable()
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    if (m_currentExInfo.m_hThrowable)
    {
        return ObjectFromHandle(m_currentExInfo.m_hThrowable);
    }

    return NULL;
}

void ThreadExceptionState::SetThrowable(OBJECTREF throwable DEBUG_ARG(SetThrowableErrorChecking stecFlags))
{
    CONTRACTL
    {
        if ((throwable == NULL) || CLRException::IsPreallocatedExceptionObject(throwable)) NOTHROW; else THROWS; // From CreateHandle
        GC_NOTRIGGER;
        if (throwable == NULL) MODE_ANY; else MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    m_currentExInfo.DestroyExceptionHandle();
    
    if (throwable != NULL)
    {
        // Non-compliant exceptions are always wrapped.
        // The use of the ExceptionNative:: helper here (rather than the global ::IsException helper)
        // is hokey, but we need a GC_NOTRIGGER version and it's only for an ASSERT.
        _ASSERTE(ExceptionNative::IsException(throwable->GetMethodTable()));

        OBJECTHANDLE hNewThrowable;
        
        // If we're tracking one of the preallocated exception objects, then just use the global handle that
        // matches it rather than creating a new one.
        if (CLRException::IsPreallocatedExceptionObject(throwable))
        {
            hNewThrowable = CLRException::GetPreallocatedHandleForObject(throwable);
        }
        else
        {
            AppDomain* pDomain = GetMyThread()->GetDomain();
            PREFIX_ASSUME(pDomain != NULL);
            hNewThrowable = pDomain->CreateHandle(throwable);
        }

        m_currentExInfo.m_hThrowable = hNewThrowable;
    }
}

DWORD ThreadExceptionState::GetExceptionCode()
{
    LEAF_CONTRACT;
    
    return m_currentExInfo.m_ExceptionCode;
}

BOOL ThreadExceptionState::IsExceptionInProgress()
{
    LEAF_CONTRACT;
    
    return (m_currentExInfo.m_pBottomMostHandler != NULL);
}

void ThreadExceptionState::GetLeafFrameInfo(StackTraceElement* pStackTraceElement)
{
    WRAPPER_CONTRACT;

    m_currentExInfo.m_StackTraceInfo.GetLeafFrameInfo(pStackTraceElement);
}

EXCEPTION_POINTERS* ThreadExceptionState::GetExceptionPointers()
{
    LEAF_CONTRACT;
    
    return NULL;
}

//-----------------------------------------------------------------------------
// SetExceptionPointers -- accessor to set pointer to EXCEPTION_POINTERS
//   member.
//
//  only x86
//

EXCEPTION_RECORD* ThreadExceptionState::GetExceptionRecord()
{
    LEAF_CONTRACT;
    
    return &m_currentExInfo.m_ExceptionRecord;
}

CONTEXT* ThreadExceptionState::GetContextRecord()
{
    LEAF_CONTRACT;
    
    return m_currentExInfo.m_pContext;
}

ExceptionFlags* ThreadExceptionState::GetFlags()
{

    return &(m_currentExInfo.m_ExceptionFlags);

}

#ifdef DEBUGGING_SUPPORTED    
DebuggerExState*    ThreadExceptionState::GetDebuggerState()
{
    return &(m_currentExInfo.m_DebuggerExState);
}

BOOL ThreadExceptionState::IsDebuggerInterceptable()
{
    LEAF_CONTRACT;
    DWORD ExceptionCode = GetExceptionCode();
    return (BOOL)((ExceptionCode != STATUS_STACK_OVERFLOW) &&
                  (ExceptionCode != EXCEPTION_BREAKPOINT) &&
                  (ExceptionCode != EXCEPTION_SINGLE_STEP) &&
                  !GetFlags()->UnwindHasStarted() &&
                  !GetFlags()->DebuggerInterceptNotPossible());
}

#ifdef _X86_
PEXCEPTION_REGISTRATION_RECORD GetClrSEHRecordServicingStackPointer(Thread *pThread, void *pStackPointer);
#endif // _X86_

BOOL DebuggerExState::SetDebuggerInterceptInfo(IJitManager *pJitManager,
                                      Thread *pThread,
                                      METHODTOKEN methodToken,
                                      MethodDesc *pFunc,
                                      ULONG_PTR natOffset,
                                      void *stackPointer
                                      IA64_ARG(void *backingStorePointer),
                                      ExceptionFlags* pFlags)
{
    WRAPPER_CONTRACT;

    //
    // Verify parameters are non-NULL
    //
    if ((pJitManager == NULL) ||
        (pThread == NULL) ||
        (methodToken == NULL) ||
        (pFunc == NULL) ||
        (natOffset == (TADDR)0) ||
        (stackPointer == NULL))
    {
        return FALSE;
    }

    //
    // You can only call this function on the currently active exception.
    //
    if (this != pThread->GetExceptionState()->GetDebuggerState())
    {
        return FALSE;
    }

    //
    // Check that the stack pointer is less than as far as we have searched so far.
    //
    if ((stackPointer > m_pDebuggerIndicatedStackPointer)
        IA64_ONLY(|| ( (stackPointer        == m_pDebuggerIndicatedStackPointer ) && 
                       (backingStorePointer <  m_pDebuggerIndicatedBStorePointer) ) )
       )
    {
        return FALSE;
    }

#if defined(_X86_)
    //
    // Get the SEH frame that covers this location on the stack. Note: we pass a skip count of 1. We know that when
    // this is called, there is a nested exception handler on pThread's stack that is only there during exception
    // processing, and it won't be there when we go to do the interception. Therefore, we skip that nested record,
    // and pick the next valid record above it.
    //
    m_pDebuggerInterceptFrame = GetClrSEHRecordServicingStackPointer(pThread, stackPointer);

#else  // !_X86_
    PORTABILITY_ASSERT("SetDebuggerInterceptInfo() (ExState.cpp) - continuable exceptions NYI\n");
    m_pDebuggerInterceptFrame = EXCEPTION_CHAIN_END;

#endif // !_X86_

    if (m_pDebuggerInterceptFrame == EXCEPTION_CHAIN_END)
    {
        return FALSE;
    }

    //
    // Now we need to search and find the function information for this entry on the stack.
    //
    int nestingLevel = ComputeEnclosingHandlerNestingLevel(pJitManager,
                                                           methodToken,
                                                           natOffset);

    //
    // These values will over-ride the other tct values in GetDebuggerInterceptInfo
    //
    m_pDebuggerInterceptFunc = pFunc;
    m_dDebuggerInterceptHandlerDepth  = nestingLevel;
    m_pDebuggerInterceptStackPointer  = stackPointer;
    IA64_ONLY(m_pDebuggerInterceptBStorePointer = backingStorePointer);
    m_pDebuggerInterceptNativeOffset  = natOffset;

    pFlags->SetDebuggerInterceptInfo();
    return TRUE;
}
#endif // DEBUGGING_SUPPORTED

EHClauseInfo* ThreadExceptionState::GetCurrentEHClauseInfo()
{
    return &(m_currentExInfo.m_EHClauseInfo);
}

#endif // DACCESS_COMPILE

void ThreadExceptionState::SetThreadExceptionFlag(ThreadExceptionFlag flag)
{
    LEAF_CONTRACT;

    m_flag = (ThreadExceptionFlag)((DWORD)m_flag | flag);
}

void ThreadExceptionState::ResetThreadExceptionFlag(ThreadExceptionFlag flag)
{
    LEAF_CONTRACT;

    m_flag = (ThreadExceptionFlag)((DWORD)m_flag & ~flag);
}

BOOL ThreadExceptionState::HasThreadExceptionFlag(ThreadExceptionFlag flag)
{
    LEAF_CONTRACT;

    return ((DWORD)m_flag & flag);
}

ThreadExceptionFlagHolder::ThreadExceptionFlagHolder(ThreadExceptionState::ThreadExceptionFlag flag)
{
    WRAPPER_CONTRACT;

    Thread* pThread = GetThread();
    _ASSERTE(pThread);

    m_pExState = pThread->GetExceptionState();

    m_flag = flag;
    m_pExState->SetThreadExceptionFlag(m_flag);
}

ThreadExceptionFlagHolder::~ThreadExceptionFlagHolder()
{
    WRAPPER_CONTRACT;

    _ASSERTE(m_pExState);
    m_pExState->ResetThreadExceptionFlag(m_flag);
}

#ifdef DACCESS_COMPILE

void
ThreadExceptionState::EnumChainMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    ExInfo*           head = &m_currentExInfo;
    
    for (;;)
    {
        head->EnumMemoryRegions(flags);

        if (!head->m_pPrevNestedInfo.IsValid())
        {
            break;
        }
        
        head->m_pPrevNestedInfo.EnumMem();
        head = head->m_pPrevNestedInfo;
    }
}

#endif // DACCESS_COMPILE



