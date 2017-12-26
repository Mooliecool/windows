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
#include "exinfo.h"
#include "dbginterface.h"

#ifndef DACCESS_COMPILE
//
// Destroy the handle within an ExInfo. This respects the fact that we can have preallocated global handles living
// in ExInfo's.
//
void ExInfo::DestroyExceptionHandle(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Never, ever destroy a preallocated exception handle.
    if ((m_hThrowable != NULL) && !CLRException::IsPreallocatedExceptionHandle(m_hThrowable))
    {
        DestroyHandle(m_hThrowable);
    }

    m_hThrowable = NULL;
}

//
// CopyAndClearSource copies the contents of the given ExInfo into the current ExInfo, then re-initializes the
// given ExInfo.
//
void ExInfo::CopyAndClearSource(ExInfo *from)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        if (GetThread() != NULL) MODE_COOPERATIVE; else MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;
    
#ifdef _X86_
    LOG((LF_EH, LL_INFO100, "In ExInfo::CopyAndClearSource: m_dEsp=%08x, %08x <- [%08x], stackAddress = 0x%p <- 0x%p\n", 
         from->m_dEsp, &(this->m_dEsp), &from->m_dEsp, this->m_StackAddress, from->m_StackAddress));
#endif // _X86_

    // If we have a handle to an exception object in this ExInfo already, then go ahead and destroy it before we
    // loose it.
    DestroyExceptionHandle();
    
    // The stack address is handled differently. Save the original value.
    void* stackAddress = this->m_StackAddress;

    // Blast the entire record. Note: we're copying the handle from the source ExInfo to this object. That's okay,
    // since we're going to clear out the source ExInfo right below this.
    memcpy(this, from, sizeof(ExInfo));

    // Preserve the stack address. It should never change.
    m_StackAddress = stackAddress;

    // This ExInfo just took ownership of the handle to the exception object, so clear out that handle in the
    // source ExInfo.
    from->m_hThrowable = NULL;

    // Finally, initialize the source ExInfo.
    from->Init();
}

void ExInfo::Init()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    m_ExceptionFlags.Init();
    m_StackTraceInfo.Init();
    m_DebuggerExState.Init();
    
    m_pSearchBoundary = NULL;
    m_pBottomMostHandler = NULL;
    m_pPrevNestedInfo = NULL;
    m_ExceptionCode = 0xcccccccc;
    m_pShadowSP = NULL;
    m_StackAddress = this;
    DestroyExceptionHandle();
    m_hThrowable = NULL;
}

ExInfo::ExInfo()
{
    WRAPPER_CONTRACT;

    m_hThrowable = NULL;
    Init();
}

//*******************************************************************************
// When we hit an endcatch or an unwind and have nested handler info, either 
//   1) we have contained a nested exception and will continue handling the original 
//      exception
//  - or -
//   2) the nested exception was not contained, and was thrown beyond the original 
//      bounds where the first exception occurred. 
//
// The way we can tell this is from the stack pointer. The topmost nested handler is 
//  installed at the point where the exception occurred.  For a nested exception to be 
//  contained, it must be caught within the scope of any code that is called after 
//  the nested handler is installed.  (remember: after is a lower stack address.)
//
// If it is caught by anything earlier on the stack, it was not contained, and we 
//  unwind the nested handlers until we get to one that is higher on the stack 
//  than the esp we will unwind to. 
//
// If we still have a nested handler, then we have successfully handled a nested 
//  exception and should restore the exception settings that we saved so that 
//  processing of the original exception can continue. 
// Otherwise the nested exception has gone beyond where the original exception was 
//  thrown and therefore replaces the original exception. 
// 
// We will always remove the current exception info from the chain.
//
void ExInfo::UnwindExInfo(VOID* limit)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        if (GetThread() != NULL) MODE_COOPERATIVE; else MODE_ANY;
        INJECT_FAULT(ThrowOutOfMemory(););
    }
    CONTRACTL_END;

    // We must be in cooperative mode to do the chaining below
    #ifdef DEBUGGING_SUPPORTED
    // The debugger thread will be using this, even though it has no
    // Thread object associated with it.
    _ASSERTE((GetThread() != NULL && GetThread()->PreemptiveGCDisabled()) || 
             ((g_pDebugInterface != NULL) && (g_pDebugInterface->GetRCThreadId() == GetCurrentThreadId())));
    #endif // DEBUGGING_SUPPORTED

    LOG((LF_EH, LL_INFO100, "UnwindExInfo: unwind limit is 0x%p, prevNested is 0x%p\n", limit, m_pPrevNestedInfo));
            
    ExInfo *pPrevNestedInfo = m_pPrevNestedInfo;

    // At first glance, you would think that each nested exception has
    // been unwound by it's corresponding NestedExceptionHandler.  But that's
    // not necessarily the case.  The following assertion cannot be made here,
    // and the loop is necessary.
    //
    //_ASSERTE(pPrevNestedInfo == 0 || (DWORD)pPrevNestedInfo >= limit);
    //
    // Make sure we've unwound any nested exceptions that we're going to skip over.
    //
    while (pPrevNestedInfo && pPrevNestedInfo->m_StackAddress < limit) 
    {
        STRESS_LOG1(LF_EH, LL_INFO100, "UnwindExInfo: PopExInfo(): popping nested ExInfo at 0x%p\n", pPrevNestedInfo->m_StackAddress);

        if (pPrevNestedInfo->m_hThrowable != NULL)  
        {
            pPrevNestedInfo->DestroyExceptionHandle();
        }

        pPrevNestedInfo->m_StackTraceInfo.FreeStackTrace();

        #ifdef DEBUGGING_SUPPORTED
        if (g_pDebugInterface != NULL)
        {
            g_pDebugInterface->DeleteInterceptContext(pPrevNestedInfo->m_DebuggerExState.GetDebuggerInterceptContext());
        }
        #endif

        ExInfo* pPrev = pPrevNestedInfo->m_pPrevNestedInfo;

        if (pPrevNestedInfo->IsHeapAllocated())
        {
            delete pPrevNestedInfo;
        }
        
        pPrevNestedInfo = pPrev;
    }

    // either clear the one we're about to copy over or the topmost one
    m_StackTraceInfo.FreeStackTrace();

    if (pPrevNestedInfo) 
    {
        // found nested handler info that is above the esp restore point so succesfully caught nested
        STRESS_LOG2(LF_EH, LL_INFO100, "UnwindExInfo: resetting nested ExInfo to 0x%p stackaddress:0x%p\n", pPrevNestedInfo, pPrevNestedInfo->m_StackAddress);

        // Remember if this ExInfo is heap allocated or not.
        BOOL isHeapAllocated = pPrevNestedInfo->IsHeapAllocated();
        
        // Copy pPrevNestedInfo to 'this', clearing pPrevNestedInfo in the process.
        CopyAndClearSource(pPrevNestedInfo);

        if (isHeapAllocated)
        {
            delete pPrevNestedInfo;         // Now delete the old record if we needed to.
        }
    } 
    else 
    {
        STRESS_LOG0(LF_EH, LL_INFO100, "UnwindExInfo: clearing topmost ExInfo\n");

        // We just do a basic Init of the current top ExInfo here.
        Init();
    }
}
#endif // DACCESS_COMPILE


#ifdef DACCESS_COMPILE

void
ExInfo::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    // ExInfo is embedded so don't enum 'this'.
    OBJECTHANDLE_EnumMemoryRegions(m_hThrowable);

}

#endif // #ifdef DACCESS_COMPILE


