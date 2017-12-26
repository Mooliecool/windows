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
// ---------------------------------------------------------------------------
// Clrex.cpp
// ---------------------------------------------------------------------------

#include "common.h"
#include "clrex.h"
#include "field.h"
#include "comstring.h"
#include "eetoprofinterfacewrapper.inl"
#include "sigformat.h"
#include "eeconfig.h"



// ---------------------------------------------------------------------------
// Helper function to get an exception from outside the exception.  
//  Create and return a LastThrownObjectException.  Its virtual destructor
//  will clean up properly.
void GetLastThrownObjectExceptionFromThread_Internal(Exception **ppException)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        SO_TOLERANT;    // no risk of an SO after we've allocated the object here
    }
    CONTRACTL_END;

    // If the Thread has been set up, then the LastThrownObject may make sense...
    if (GetThread())
    {
        // give back an object that knows about Threads and their exceptions.
        *ppException = new CLRLastThrownObjectException();
    }
    else
    {   
        // but if no Thread, don't pretend to know about LastThrownObject.
        *ppException = NULL;
    }

} // void GetLastThrownObjectExceptionFromThread_Internal()

// ---------------------------------------------------------------------------
// CLRException methods
// ---------------------------------------------------------------------------

CLRException::~CLRException()
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    OBJECTHANDLE throwableHandle = GetThrowableHandle();
    if (throwableHandle != NULL)
    {
        STRESS_LOG1(LF_EH, LL_INFO100, "CLRException::~CLRException destroying throwable: obj = %x\n", GetThrowableHandle());
        // clear the handle first, so if we SO on destroying it, we don't have a dangling reference
        SetThrowableHandle(NULL);
        DestroyHandle(throwableHandle);
    }
}

OBJECTREF CLRException::GetThrowable()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        NOTHROW;
        MODE_COOPERATIVE;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    OBJECTREF throwable = NULL;

    Thread *pThread = GetThread();

    if (pThread->IsRudeAbortInitiated()) {
        return GetPreallocatedRudeThreadAbortException();
    }

    if (this == GetSOException() || 
        (IsType(CLRLastThrownObjectException::GetType()) && pThread->LastThrownObject() == GetPreallocatedStackOverflowException()))
    {
        return GetPreallocatedStackOverflowException();
    }

    OBJECTHANDLE oh = GetThrowableHandle();
    if (oh != NULL)
    {
        return ObjectFromHandle(oh);
    }
   
    // We need to disable the backout stack validation at this point since GetThrowable can 
    // take arbitrarily large amounts of stack for different exception types; however we know 
    // for a fact that we will never go through this code path if the exception is a stack 
    // overflow exception since we already handled that case above with the pre-allocated SO exception.
    {
        DISABLE_BACKOUT_STACK_VALIDATION;
        
        GCPROTECT_BEGIN(throwable);

        EX_TRY
        {
            FAULT_NOT_FATAL();
            throwable = CreateThrowable();
        }
        EX_CATCH
        {

            Exception *pException = GET_EXCEPTION();

            if (GetHR() == COR_E_THREADABORTED)
            {
                // If creating a normal ThreadAbortException fails, due to OOM or StackOverflow,
                // use a pre-created one.
                // We do not won't to change a ThreadAbortException into OOM or StackOverflow, because
                // it will cause recursive call when escalation policy is on: 
                // Creating ThreadAbortException fails, we throw OOM.  Escalation leads to ThreadAbort.
                // The cycle repeats.
                throwable = GetPreallocatedThreadAbortException();
            }
            else if (GetInstanceType() != pException->GetInstanceType())
            {
                throwable = CLRException::GetThrowableFromException(pException);
            }
            else
            {
#if defined(_DEBUG)
                static int BreakOnExceptionInGetThrowable = -1;
                if (BreakOnExceptionInGetThrowable == -1)
                {
                    BreakOnExceptionInGetThrowable = REGUTIL::GetConfigDWORD(L"BreakOnExceptionInGetThrowable", 0);
                }
                if (BreakOnExceptionInGetThrowable)
                {
                    _ASSERTE(!"BreakOnExceptionInGetThrowable");
                }
                LOG((LF_EH, LL_INFO100, "GetThrowable: Exception in GetThrowable, translating to a preallocated exception.\n"));
#endif // _DEBUG
                // Look at the type of GET_EXCEPTION() and see if it is OOM or SO.
                if (pException->IsPreallocatedOOMException())
                {
                    throwable = GetPreallocatedOutOfMemoryException();
                }
                else
                if (pException->IsPreallocatedSOException())
                {
                    throwable = GetPreallocatedStackOverflowException();
                }
                else
                {
                    // We didn't recognize it, so use the EE exception.
                    throwable = GetPreallocatedExecutionEngineException();
                }
            }
        }
        EX_END_CATCH(SwallowAllExceptions)

        if (throwable == NULL)
        {
            STRESS_LOG0(LF_EH, LL_INFO100, "CLRException::GetThrowable: We have failed to track exceptions accurately through the system.\n");



            throwable = GetPreallocatedOutOfMemoryException();

        }

        EX_TRY
        {
            SetThrowableHandle(GetAppDomain()->CreateHandle(throwable));
            if (m_innerException != NULL && !CLRException::IsPreallocatedExceptionObject(throwable))
            {
                // Only set inner exception if the exception is not preallocated.
                FAULT_NOT_FATAL();

                // If inner exception is not empty, then set the managed exception's 
                // _innerException field properly
                OBJECTREF throwableValue = CLRException::GetThrowableFromException(m_innerException);
                ((EXCEPTIONREF)throwable)->SetInnerException(throwableValue);
            }

        }
        EX_CATCH
        {
            // No matter... we just don't get to cache the throwable.
        }
        EX_END_CATCH(SwallowAllExceptions)

        GCPROTECT_END();
    }

    return throwable;
}

HRESULT CLRException::GetHR()
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HRESULT hr = E_FAIL;

    GCX_COOP();

    EX_TRY
    {
        hr = GetExceptionHResult(GetThrowable());
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions)

    return hr;
}

IErrorInfo *CLRException::GetIErrorInfo()
{
    LEAF_CONTRACT;
    return NULL;
 }

void CLRException::GetMessage(SString &result)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    GCX_COOP();

    OBJECTREF e = GetThrowable();
    if (e != NULL)
    {
        _ASSERTE(IsException(e->GetMethodTable()));

        GCPROTECT_BEGIN (e);

        STRINGREF message = ((EXCEPTIONREF)e)->GetMessage();

        if (!message)
            result.Clear();
        else
            message->GetSString(result);

        GCPROTECT_END ();
    }
}

OBJECTREF CLRException::GetPreallocatedOutOfMemoryException()
{
    WRAPPER_CONTRACT;
    _ASSERTE(g_pPreallocatedOutOfMemoryException != NULL);
    return ObjectFromHandle(g_pPreallocatedOutOfMemoryException);
}

OBJECTREF CLRException::GetPreallocatedStackOverflowException()
{
    WRAPPER_CONTRACT;
    _ASSERTE(g_pPreallocatedStackOverflowException != NULL);
    return ObjectFromHandle(g_pPreallocatedStackOverflowException);
}

OBJECTREF CLRException::GetPreallocatedExecutionEngineException()
{
    WRAPPER_CONTRACT;
    _ASSERTE(g_pPreallocatedExecutionEngineException != NULL);
    return ObjectFromHandle(g_pPreallocatedExecutionEngineException);
}

OBJECTREF CLRException::GetPreallocatedRudeThreadAbortException()
{
    WRAPPER_CONTRACT;
    // When we are hosted, we pre-create this exception.
    // This function should be called only if the exception has been created.
    _ASSERTE(g_pPreallocatedRudeThreadAbortException);
    return ObjectFromHandle(g_pPreallocatedRudeThreadAbortException);
}

OBJECTREF CLRException::GetPreallocatedThreadAbortException()
{
    WRAPPER_CONTRACT;
    _ASSERTE(g_pPreallocatedThreadAbortException);
    return ObjectFromHandle(g_pPreallocatedThreadAbortException);
}

OBJECTHANDLE CLRException::GetPreallocatedOutOfMemoryExceptionHandle()
{
    LEAF_CONTRACT;
    _ASSERTE(g_pPreallocatedOutOfMemoryException != NULL);
    return g_pPreallocatedOutOfMemoryException;
}

OBJECTHANDLE CLRException::GetPreallocatedThreadAbortExceptionHandle()
{
    LEAF_CONTRACT;
    _ASSERTE(g_pPreallocatedThreadAbortException != NULL);
    return g_pPreallocatedThreadAbortException;
}

OBJECTHANDLE CLRException::GetPreallocatedRudeThreadAbortExceptionHandle()
{
    LEAF_CONTRACT;
    _ASSERTE(g_pPreallocatedRudeThreadAbortException != NULL);
    return g_pPreallocatedRudeThreadAbortException;
}

OBJECTHANDLE CLRException::GetPreallocatedStackOverflowExceptionHandle()
{
    LEAF_CONTRACT;
    _ASSERTE(g_pPreallocatedStackOverflowException != NULL);
    return g_pPreallocatedStackOverflowException;
}

OBJECTHANDLE CLRException::GetPreallocatedExecutionEngineExceptionHandle()
{
    LEAF_CONTRACT;
    _ASSERTE(g_pPreallocatedExecutionEngineException != NULL);
    return g_pPreallocatedExecutionEngineException;
}

//
// Returns TRUE if the given object ref is one of the preallocated exception objects.
//
BOOL CLRException::IsPreallocatedExceptionObject(OBJECTREF o)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if ((o == ObjectFromHandle(g_pPreallocatedOutOfMemoryException)) ||
        (o == ObjectFromHandle(g_pPreallocatedStackOverflowException)) ||
        (o == ObjectFromHandle(g_pPreallocatedExecutionEngineException)))
    {
        return TRUE;
    }

    // The preallocated rude thread abort exception is not always preallocated.
    if ((g_pPreallocatedRudeThreadAbortException != NULL) &&
        (o == ObjectFromHandle(g_pPreallocatedRudeThreadAbortException)))
    {
        return TRUE;
    }

    // The preallocated rude thread abort exception is not always preallocated.
    if ((g_pPreallocatedThreadAbortException != NULL) &&
        (o == ObjectFromHandle(g_pPreallocatedThreadAbortException)))
    {
        return TRUE;
    }

    return FALSE;
}

//
// Returns TRUE if the given object ref is one of the preallocated exception handles
//
BOOL CLRException::IsPreallocatedExceptionHandle(OBJECTHANDLE h)
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

    if ((h == g_pPreallocatedOutOfMemoryException) ||
        (h == g_pPreallocatedStackOverflowException) ||
        (h == g_pPreallocatedExecutionEngineException) ||
        (h == g_pPreallocatedThreadAbortException))
    {
        return TRUE;
    }

    // The preallocated rude thread abort exception is not always preallocated.
    if ((g_pPreallocatedRudeThreadAbortException != NULL) &&
        (h == g_pPreallocatedRudeThreadAbortException))
    {
        return TRUE;
    }

    return FALSE;
}

//
// Returns a preallocated handle to match a preallocated exception object, or NULL if the object isn't one of the
// preallocated exception objects.
//
OBJECTHANDLE CLRException::GetPreallocatedHandleForObject(OBJECTREF o)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (o == ObjectFromHandle(g_pPreallocatedOutOfMemoryException))
    {
        return g_pPreallocatedOutOfMemoryException;
    }
    else if (o == ObjectFromHandle(g_pPreallocatedStackOverflowException))
    {
        return g_pPreallocatedStackOverflowException;
    }
    else if (o == ObjectFromHandle(g_pPreallocatedExecutionEngineException))
    {
        return g_pPreallocatedExecutionEngineException;
    }
    else if (o == ObjectFromHandle(g_pPreallocatedThreadAbortException))
    {
        return g_pPreallocatedThreadAbortException;
    }

    // The preallocated rude thread abort exception is not always preallocated.
    if ((g_pPreallocatedRudeThreadAbortException != NULL) &&
        (o == ObjectFromHandle(g_pPreallocatedRudeThreadAbortException)))
    {
        return g_pPreallocatedRudeThreadAbortException;
    }

    return NULL;
}


// Prefer a new OOM exception if we can make one.  If we cannot, then give back the pre-allocated one.
OBJECTREF CLRException::GetBestOutOfMemoryException()
{
    CONTRACTL
    {
        NOTHROW;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF retVal = NULL;

    EX_TRY
    {
        FAULT_NOT_FATAL();

        EXCEPTIONREF pOutOfMemory = (EXCEPTIONREF)AllocateObject(g_pOutOfMemoryExceptionClass);
        pOutOfMemory->SetHResult(COR_E_OUTOFMEMORY);
        pOutOfMemory->SetXCode(EXCEPTION_COMPLUS);

        retVal = pOutOfMemory; 
    }
    EX_CATCH
    {
        retVal = GetPreallocatedOutOfMemoryException();
    }
    EX_END_CATCH(SwallowAllExceptions)

    _ASSERTE(retVal != NULL);

    return retVal;
}


// Works on non-CLRExceptions as well
// static function
OBJECTREF CLRException::GetThrowableFromException(Exception *pException)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        NOTHROW;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    Thread* pThread = GetThread();

    // Can't have a throwable without a Thread.
    _ASSERTE(pThread != NULL);

    if (NULL == pException)
    {
        return pThread->LastThrownObject();
    }

    if (pException->IsType(CLRException::GetType()))
        return ((CLRException*)pException)->GetThrowable();

    if (pException->IsType(EEException::GetType()))
        return ((EEException*)pException)->GetThrowable();

    // Note: we are creating a throwable on the fly in this case - so 
    // multiple calls will return different objects.  If we really need identity,
    // we could store a throwable handle at the catch site, or store it
    // on the thread object.

    if (pException->IsType(SEHException::GetType()))
    {
        SEHException *pSEHException = (SEHException*)pException;

        switch (pSEHException->m_exception.ExceptionCode)
        {
        case EXCEPTION_COMPLUS:
            return pThread->LastThrownObject();

        case STATUS_NO_MEMORY:
            return GetBestOutOfMemoryException();

        case STATUS_STACK_OVERFLOW:
            return GetPreallocatedStackOverflowException();
        }

        DWORD exceptionCode = 
          MapWin32FaultToCOMPlusException(&pSEHException->m_exception);

        EEException e((RuntimeExceptionKind)exceptionCode);

        OBJECTREF throwable = e.GetThrowable();
        GCPROTECT_BEGIN (throwable);
        EX_TRY
        {
            SCAN_IGNORE_FAULT;
            if (throwable != NULL  && !CLRException::IsPreallocatedExceptionObject(throwable))
            {
                _ASSERTE(IsException(throwable->GetMethodTable()));

                // set the exception code
                ((EXCEPTIONREF)throwable)->SetXCode(pSEHException->m_exception.ExceptionCode);
            }
        }
        EX_CATCH
        {
        }
        EX_END_CATCH(SwallowAllExceptions)
        GCPROTECT_END ();
            
        return throwable;
    }
    else
    {
        OBJECTREF oRetVal = NULL;
        GCPROTECT_BEGIN(oRetVal);
        {
            EX_TRY
            {
                HRESULT hr = pException->GetHR();

                if (hr == E_OUTOFMEMORY || hr == HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY))
                {
                    oRetVal = GetBestOutOfMemoryException();
                }
                else if (hr == COR_E_STACKOVERFLOW)
                {
                    oRetVal = GetPreallocatedStackOverflowException();
                }
                else
                {
                    SafeComHolder<IErrorInfo> pErrInfo(pException->GetErrorInfo());

                    if (pErrInfo != NULL)
                    {
                        GetExceptionForHR(hr, pErrInfo, &oRetVal);
                    }
                    else
                    {
                        SString message;
                        pException->GetMessage(message);

                        EEMessageException e(hr, IDS_EE_GENERIC, message);

                        oRetVal = e.CreateThrowable();
                    }
                }
            }
            EX_CATCH
            {
                // We have caught an exception trying to get a Throwable for the pException we
                //  were given.  It is tempting to want to get the Throwable for the new
                //  exception, but that is dangerous, due to infinitely cascading 
                //  exceptions, leading to a stack overflow.

                // If we can see that the exception was OOM, return the preallocated OOM,
                //  if we can see that it is SO, return the preallocated SO, 
                //  if we can see that it is some other managed exception, return that
                //  exception, otherwise return the preallocated ExecutionEngineException.
                Exception *pNewException = GET_EXCEPTION();

                if (pNewException->IsPreallocatedOOMException())
                {   // It definitely was an OOM
                    STRESS_LOG0(LF_EH, LL_INFO100, "CLRException::GetThrowable: OOM creating throwable; getting pre-alloc'd OOM.\n");
                    if (oRetVal == NULL)
                        oRetVal = GetPreallocatedOutOfMemoryException();
                }
                else
                if (pNewException->IsPreallocatedSOException())
                {   // It definitely was an SO
                    STRESS_LOG0(LF_EH, LL_INFO100, "CLRException::GetThrowable: SO creating throwable; getting pre-alloc'd SO.\n");
                    if (oRetVal == NULL)
                        oRetVal = GetPreallocatedStackOverflowException();
                }
                else
                if (pNewException->IsType(CLRLastThrownObjectException::GetType()) &&
                    (pThread->LastThrownObject() != NULL))           
                {
                    STRESS_LOG0(LF_EH, LL_INFO100, "CLRException::GetThrowable: LTO Exception creating throwable; getting LastThrownObject.\n");
                    if (oRetVal == NULL)
                        oRetVal = pThread->LastThrownObject();
                }
                else
                {   // Don't know what it was -- it really shouldn't have happened.
                    STRESS_LOG0(LF_EH, LL_INFO100, "CLRException::GetThrowable: Unknown Exception creating throwable; getting EE Exception.\n");
                    if (oRetVal == NULL)
                        oRetVal = GetPreallocatedExecutionEngineException();
                }

            }
            EX_END_CATCH(SwallowAllExceptions)
        }
        GCPROTECT_END();

        return oRetVal;
    }
} // OBJECTREF CLRException::GetThrowable()

OBJECTREF CLRException::GetThrowableFromExceptionRecord(EXCEPTION_RECORD *pExceptionRecord)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    DWORD exceptionCode = pExceptionRecord->ExceptionCode;

    if (exceptionCode == EXCEPTION_COMPLUS)
    {
        return GetThread()->LastThrownObject();
    }

    return NULL;
}

CLRException::HandlerState::HandlerState()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    m_pThread = GetThread();
    if (m_pThread == NULL)
    {
        m_pFrame = NULL;
        m_fPreemptiveGCDisabled = FALSE;
    }
    else
    {
        m_pFrame = m_pThread->GetFrame();
        m_fPreemptiveGCDisabled = m_pThread->PreemptiveGCDisabled();
    }

#ifdef _X86_
    m_pDownlevelVectoredHandlerRecord = NULL;
#endif
}

void CLRException::HandlerState::CleanupTry()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;
    
#ifdef _X86_
    {
        // If we installed a record while setting up the try, then it should be at the top of the list at this
        // point if we're here because we've exited the try without throwing an exception. But if we're here
        // because we're on the unwind pass, then this handler isn't the one at the top. Unfortunatley, there's no
        // way to tell which case we're in, so we can't be more strict.
        if ((m_pDownlevelVectoredHandlerRecord != NULL) &&
            (GetCurrentSEHRecord() == m_pDownlevelVectoredHandlerRecord))
        {
            UNINSTALL_EXCEPTION_HANDLING_RECORD(m_pDownlevelVectoredHandlerRecord);
        }
        else
        {
            // eh dispatch will clean it up 
        }
    }
#endif // _X86_
    
    if (m_pThread != NULL)
    {
#if !defined(PAL_PORTABLE_SEH) || !defined(__GNUC__)
        // If there is no frame to unwind, UnwindFrameChain call is just an expensive NOP
        // due to setting up and tear down of EH records. So we avoid it if we can.
        if (m_pThread->GetFrame() < m_pFrame)
            UnwindFrameChain(m_pThread, m_pFrame);
#else
        // All managed frames have been unwound before the longjmp (see excep.h)
        // So a simple SetFrame is OK here.
        // gcc 3.1 will call this function with SP inside the function that setup the EX_TRY
        // So we can't refer to any managed frames at this point anyway
        Frame* pFrame = m_pThread->m_pFrame;
        if (pFrame < m_pFrame) {
            GCX_COOP_THREAD_EXISTS(m_pThread);
            m_pThread->SetFrame(m_pFrame);
        }
#endif

        if (m_fPreemptiveGCDisabled != m_pThread->PreemptiveGCDisabled())
        {
            if (m_fPreemptiveGCDisabled)
                m_pThread->DisablePreemptiveGC();
            else
                m_pThread->EnablePreemptiveGC();
        }
    }

    // Make sure to call the base class's CleanupTry so it can do whatever it wants to do.
    Exception::HandlerState::CleanupTry();
}

void CLRException::HandlerState::SetupCatchCommon()
{
    LEAF_CONTRACT;
}

void CLRException::HandlerState::SetupCatch(bool fCaughtInternalCxxException)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    LOG((LF_EH, LL_INFO100, "EX_CATCH catching exception (CLRException::HandlerState)\n"));

    Exception::HandlerState::SetupCatch(fCaughtInternalCxxException);  
    

}

void CLRException::HandlerState::SucceedCatch(bool fCaughtInternalCxxException)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    LOG((LF_EH, LL_INFO100, "EX_CATCH catch succeeded (CLRException::HandlerState)\n"));

    //

    Exception::HandlerState::SucceedCatch(fCaughtInternalCxxException);
}

// ---------------------------------------------------------------------------
// EEException methods
// ---------------------------------------------------------------------------

//------------------------------------------------------------------------
// Array that is used to retrieve the right exception for a given HRESULT.
//------------------------------------------------------------------------
struct ExceptionHRInfo
{
    int cHRs;
    HRESULT *aHRs;
};

#define EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr) static HRESULT s_##reKind##HRs[] = {hr,
#define EXCEPTION_ADD_HR(hr) hr,
#define EXCEPTION_END_DEFINE() };
#include "rexcep.h"
#undef EXCEPTION_BEGIN_DEFINE
#undef EXCEPTION_ADD_HR
#undef EXCEPTION_END_DEFINE

static 
ExceptionHRInfo gExceptionHRInfos[] = {
#define EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr) {sizeof(s_##reKind##HRs) / sizeof(HRESULT), s_##reKind##HRs},
#define EXCEPTION_ADD_HR(hr)
#define EXCEPTION_END_DEFINE()
#include "rexcep.h"
#undef EXCEPTION_BEGIN_DEFINE
#undef EXCEPTION_ADD_HR
#undef EXCEPTION_END_DEFINE
};

static 
bool gShouldDisplayHR[] =
{   
#define EXCEPTION_BEGIN_DEFINE(ns, reKind, bHRformessage, hr) bHRformessage,
#define EXCEPTION_ADD_HR(hr)
#define EXCEPTION_END_DEFINE()
#include "rexcep.h"
#undef EXCEPTION_BEGIN_DEFINE
#undef EXCEPTION_ADD_HR
#undef EXCEPTION_END_DEFINE
};


/*static*/
HRESULT EEException::GetHRFromKind(RuntimeExceptionKind reKind)
{
    LEAF_CONTRACT;
    return gExceptionHRInfos[reKind].aHRs[0];
}

HRESULT EEException::GetHR() 
{ 
    LEAF_CONTRACT;

    return EEException::GetHRFromKind(m_kind);
}
    
IErrorInfo *EEException::GetErrorInfo()
{
    LEAF_CONTRACT;
    
    return NULL;
}

BOOL EEException::GetThrowableMessage(SString &result)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Return a meaningful HR message, if there is one.

    HRESULT hr = GetHR();


    // If the hr is more interesting than the kind, use that
    // for a message.

    if (hr != S_OK 
        && hr != E_FAIL
        && (gShouldDisplayHR[m_kind]
            || gExceptionHRInfos[m_kind].aHRs[0] != GetHR()))
    {
        // If it has only one HR, the original message should be good enough
        _ASSERTE(gExceptionHRInfos[m_kind].cHRs > 1 ||
                 gExceptionHRInfos[m_kind].aHRs[0] != GetHR());
        
        GenerateTopLevelHRExceptionMessage(GetHR(), result);
        return TRUE;
    }

    // No interesting hr - just keep the class default message.

    return FALSE;
}

void EEException::GetMessage(SString &result)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // First look for a specialized message
    if (GetThrowableMessage(result))
        return;

    // Otherwise, report the class's generic message
        result.SetLiteral(g_Mscorlib.GetExceptionName(m_kind));
}

OBJECTREF EEException::CreateThrowable()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    _ASSERTE(g_pPreallocatedOutOfMemoryException != NULL);
    static int allocCount = 0;

    MethodTable *pMT = g_Mscorlib.GetException(m_kind);

    ThreadPreventAsyncHolder preventAsyncHolder(m_kind == kThreadAbortException);

    OBJECTREF throwable = AllocateObject(pMT);
    allocCount++;
    GCPROTECT_BEGIN(throwable);

    {
        ThreadPreventAbortHolder preventAbort(m_kind == kThreadAbortException ||
                                              m_kind == kThreadInterruptedException);
        CallDefaultConstructor(throwable);
    }

    HRESULT hr = GetHR();
    ((EXCEPTIONREF)throwable)->SetHResult(hr);

    SString message;
    if (GetThrowableMessage(message))
    {
        // Set the message field. It is not safe doing this through the constructor
        // since the string constructor for some exceptions add a prefix to the message 
        // which we don't want.
        //
        // We only want to replace whatever the default constructor put there, if we
        // have something meaningful to add.
        
        STRINGREF s = COMString::NewString(message);
        ((EXCEPTIONREF)throwable)->SetMessage(s);
    }

    GCPROTECT_END();

    return throwable;
}

RuntimeExceptionKind EEException::GetKindFromHR(HRESULT hr)
{
    LEAF_CONTRACT;

    for (int i = 0; i < kLastException; i++)
    {
        for (int j = 0; j < gExceptionHRInfos[i].cHRs; j++)
        {
            if (gExceptionHRInfos[i].aHRs[j] == hr)
                return (RuntimeExceptionKind) i;
        }
    }

    return kCOMException;
} // RuntimeExceptionKind EEException::GetKindFromHR()

BOOL EEException::GetResourceMessage(UINT iResourceID, SString &result, 
                                     const SString &arg1, const SString &arg2,
                                     const SString &arg3, const SString &arg4,
                                     const SString &arg5, const SString &arg6,
                                     const SString &arg7, const SString &arg8,
                                     const SString &arg9, const SString &arg10)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    BOOL ok;

    StackSString temp;
    ok = temp.LoadResource(iResourceID, FALSE);
    if (ok)
        result.FormatMessage(FORMAT_MESSAGE_FROM_STRING,
         (LPCWSTR)temp, 0, 0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

    return ok;
}

// ---------------------------------------------------------------------------
// EEMessageException methods
// ---------------------------------------------------------------------------

HRESULT EEMessageException::GetHR()
{
    WRAPPER_CONTRACT;
    
    return m_hr;
}

BOOL EEMessageException::GetThrowableMessage(SString &result)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (m_resID != 0 && GetResourceMessage(m_resID, result))
        return TRUE;

    return EEException::GetThrowableMessage(result);
}

BOOL EEMessageException::GetResourceMessage(UINT iResourceID, SString &result)
{
    WRAPPER_CONTRACT;

    return EEException::GetResourceMessage(
        iResourceID, result, m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7, m_arg8, m_arg9, m_arg10);
}

// ---------------------------------------------------------------------------
// EEResourceException methods
// ---------------------------------------------------------------------------

void EEResourceException::GetMessage(SString &result)
{
    WRAPPER_CONTRACT; 

    result.Printf("%s (message resource %s)", 
                  g_Mscorlib.GetExceptionName(m_kind), m_resourceName.GetUnicode());
}

BOOL EEResourceException::GetThrowableMessage(SString &result)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    STRINGREF message = NULL;
    ResMgrGetString(m_resourceName, &message);

    if (message != NULL) 
    {
        message->GetSString(result);
        return TRUE;
    }

    return EEException::GetThrowableMessage(result);

}

// ---------------------------------------------------------------------------
// EEComException methods
// ---------------------------------------------------------------------------

static HRESULT Undefer(EXCEPINFO *pExcepInfo)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (pExcepInfo->pfnDeferredFillIn)
    {
        EXCEPINFO FilledInExcepInfo; 

        HRESULT hr = pExcepInfo->pfnDeferredFillIn(&FilledInExcepInfo);
        if (SUCCEEDED(hr))
        {
            // Free the strings in the original EXCEPINFO.
            if (pExcepInfo->bstrDescription)
            {
                SysFreeString(pExcepInfo->bstrDescription);
                pExcepInfo->bstrDescription = NULL;
            }
            if (pExcepInfo->bstrSource)
            {
                SysFreeString(pExcepInfo->bstrSource);
                pExcepInfo->bstrSource = NULL;
            }
            if (pExcepInfo->bstrHelpFile)
            {
                SysFreeString(pExcepInfo->bstrHelpFile);
                pExcepInfo->bstrHelpFile = NULL;
            }

            // Fill in the new data
            *pExcepInfo = FilledInExcepInfo;
        }
    }

    if (pExcepInfo->scode != 0)
        return pExcepInfo->scode;
    else
        return (HRESULT)pExcepInfo->wCode;
}

EECOMException::EECOMException(EXCEPINFO *pExcepInfo)
  : EEException(GetKindFromHR(Undefer(pExcepInfo)))
{
    LEAF_CONTRACT;

    if (pExcepInfo->scode != 0)
        m_ED.hr = pExcepInfo->scode;
    else
        m_ED.hr = (HRESULT)pExcepInfo->wCode;
    
    m_ED.bstrDescription = pExcepInfo->bstrDescription;
    m_ED.bstrSource = pExcepInfo->bstrSource;
    m_ED.bstrHelpFile = pExcepInfo->bstrHelpFile;
    m_ED.dwHelpContext = pExcepInfo->dwHelpContext;
    m_ED.guid = GUID_NULL;

    // Zero the EXCEPINFO.
    memset(pExcepInfo, NULL, sizeof(EXCEPINFO));
}

EECOMException::EECOMException(ExceptionData *pData)
  : EEException(GetKindFromHR(pData->hr))
{
    LEAF_CONTRACT;
    
    m_ED = *pData;

    // Zero the data.
    ZeroMemory(pData, sizeof(ExceptionData));
}    

EECOMException::EECOMException(HRESULT hr, IErrorInfo *pErrInfo)
  : EEException(GetKindFromHR(hr))
{
    WRAPPER_CONTRACT;
    

    m_ED.hr = hr;
    m_ED.bstrDescription = NULL;
    m_ED.bstrSource = NULL;
    m_ED.bstrHelpFile = NULL;
    m_ED.dwHelpContext = NULL;
    m_ED.guid = GUID_NULL;
    
    FillExceptionData(&m_ED, pErrInfo);
}

BOOL EECOMException::GetThrowableMessage(SString &result)
{
     CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (m_ED.bstrDescription != NULL)
    {
        // If m_ED.bstrDescription is empty result.Set will Clear() result.
        result.Set(m_ED.bstrDescription, SysStringLen(m_ED.bstrDescription));
        return TRUE;
    }

    GenerateTopLevelHRExceptionMessage(GetHR(), result);
    return TRUE;
}

EECOMException::~EECOMException()
{
    WRAPPER_CONTRACT;
    
    FreeExceptionData(&m_ED);
}

HRESULT EECOMException::GetHR()
{
    LEAF_CONTRACT;
    
    return m_ED.hr;
}

OBJECTREF EECOMException::CreateThrowable()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    OBJECTREF throwable = NULL;
    GCPROTECT_BEGIN(throwable);

    // Note that this will pick up the message from GetThrowableMessage
    throwable = EEException::CreateThrowable();

    // Set the _helpURL field in the exception.
    if (m_ED.bstrHelpFile) 
    {
        // Create the help link from the help file and the help context.
        STRINGREF helpStr = NULL;
        if (m_ED.dwHelpContext != 0)
        {
            // We have a non 0 help context so use it to form the help link.
            SString strMessage;
            strMessage.Printf(L"%s#%d", m_ED.bstrHelpFile, m_ED.dwHelpContext);
            helpStr = COMString::NewString(strMessage);
        }
        else
        {
            // The help context is 0 so we simply use the help file to from the help link.
            helpStr = COMString::NewString(m_ED.bstrHelpFile, SysStringLen(m_ED.bstrHelpFile));
        }

        ((EXCEPTIONREF)throwable)->SetHelpURL(helpStr);
    } 
        
    // Set the Source field in the exception.
    STRINGREF sourceStr = NULL;
    if (m_ED.bstrSource) 
    {
        sourceStr = COMString::NewString(m_ED.bstrSource, SysStringLen(m_ED.bstrSource));
    }
    else
    {
        // for now set a null source
        sourceStr = COMString::GetEmptyString();
    }
    ((EXCEPTIONREF)throwable)->SetSource(sourceStr);

    GCPROTECT_END();

    return throwable;
}

// ---------------------------------------------------------------------------
// EEFieldException is an EE exception subclass composed of a field
// ---------------------------------------------------------------------------

    
BOOL EEFieldException::GetThrowableMessage(SString &result)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    LPUTF8 szFullName;
    LPCUTF8 szClassName, szMember;
    szMember = m_pFD->GetName();
    DefineFullyQualifiedNameForClass();
    szClassName = GetFullyQualifiedNameForClass(m_pFD->GetEnclosingClass());
    MAKE_FULLY_QUALIFIED_MEMBER_NAME(szFullName, NULL, szClassName, szMember, NULL);
    result.SetUTF8(szFullName);

    return TRUE;
}

// ---------------------------------------------------------------------------
// EEMethodException is an EE exception subclass composed of a field
// ---------------------------------------------------------------------------

BOOL EEMethodException::GetThrowableMessage(SString &result)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    LPUTF8 szFullName;
    LPCUTF8 szClassName, szMember;
    szMember = m_pMD->GetName();
    DefineFullyQualifiedNameForClass();
    szClassName = GetFullyQualifiedNameForClass(m_pMD->GetClass());
    MetaSig tmp(m_pMD);
    SigFormat sigFormatter(tmp, szMember);
    const char * sigStr = sigFormatter.GetCStringParmsOnly();
    MAKE_FULLY_QUALIFIED_MEMBER_NAME(szFullName, NULL, szClassName, szMember, sigStr);
    result.SetUTF8(szFullName);

    return TRUE;
}

// ---------------------------------------------------------------------------
// EEArgumentException is an EE exception subclass representing a bad argument
// ---------------------------------------------------------------------------

typedef struct {
    OBJECTREF pThrowable;
    STRINGREF s1;
    OBJECTREF pTmpThrowable;
} ProtectArgsStruct;

OBJECTREF EEArgumentException::CreateThrowable()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    _ASSERTE(GetThread() != NULL);

    ProtectArgsStruct prot;
    memset(&prot, 0, sizeof(ProtectArgsStruct));
    ResMgrGetString(m_resourceName, &prot.s1);
    GCPROTECT_BEGIN(prot);

    MethodTable *pMT = g_Mscorlib.GetException(m_kind);
    prot.pThrowable = AllocateObject(pMT);

    MethodDesc* pMD = prot.pThrowable->GetTrueMethodTable()->GetClass()->FindMethod(
                            COR_CTOR_METHOD_NAME, &gsig_IM_Str_Str_RetVoid);

    if (!pMD)
    {
        MAKE_WIDEPTR_FROMUTF8(wzMethodName, COR_CTOR_METHOD_NAME);
        COMPlusThrowNonLocalized(kMissingMethodException, wzMethodName);
    }

    MethodDescCallSite exceptionCtor(pMD);

    STRINGREF argName = COMString::NewString(m_argumentName);

    if (m_kind == kArgumentException)
    {
        ARG_SLOT args1[] = { 
            ObjToArgSlot(prot.pThrowable),
            ObjToArgSlot(prot.s1),
            ObjToArgSlot(argName),
        };
        exceptionCtor.Call_RetArgSlot(args1);
    }
    else
    {
        ARG_SLOT args1[] = { 
            ObjToArgSlot(prot.pThrowable),
            ObjToArgSlot(argName),
            ObjToArgSlot(prot.s1),
        };
        exceptionCtor.Call_RetArgSlot(args1);
    }

    GCPROTECT_END(); //Prot

    return prot.pThrowable;
}


// ---------------------------------------------------------------------------
// EETypeLoadException is an EE exception subclass representing a type loading
// error
// ---------------------------------------------------------------------------

EETypeLoadException::EETypeLoadException(LPCUTF8 pszNameSpace, LPCUTF8 pTypeName, 
                    LPCWSTR pAssemblyName, LPCUTF8 pMessageArg, UINT resIDWhy)
  : EEException(kTypeLoadException),
    m_pAssemblyName(pAssemblyName),
    m_pMessageArg(SString::Utf8, pMessageArg),
    m_resIDWhy(resIDWhy)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if(pszNameSpace)
        m_fullName.MakeFullNamespacePath(SString(SString::Utf8, pszNameSpace),
                                         SString(SString::Utf8, pTypeName));
    else if (pTypeName)
        m_fullName.SetUTF8(pTypeName);
    else {
        WCHAR wszTemplate[30];
        if (FAILED(UtilLoadStringRC(IDS_EE_NAME_UNKNOWN,
                                    wszTemplate,
                                    sizeof(wszTemplate)/sizeof(wszTemplate[0]),
                                    FALSE)))
            wszTemplate[0] = L'\0';
        MAKE_UTF8PTR_FROMWIDE(name, wszTemplate);
        m_fullName.SetUTF8(name);
    }
}

EETypeLoadException::EETypeLoadException(LPCWSTR pFullName,
                                         LPCWSTR pAssemblyName, 
                                         LPCUTF8 pMessageArg, 
                                         UINT resIDWhy)
  : EEException(kTypeLoadException),
    m_pAssemblyName(pAssemblyName),
    m_pMessageArg(SString::Utf8, pMessageArg),
    m_resIDWhy(resIDWhy)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    MAKE_UTF8PTR_FROMWIDE(name, pFullName);
    m_fullName.SetUTF8(name);
}

void EETypeLoadException::GetMessage(SString &result)
{
    WRAPPER_CONTRACT;
    GetResourceMessage(IDS_CLASSLOAD_GENERAL, result,
                       m_fullName, m_pAssemblyName, m_pMessageArg); 
}

OBJECTREF EETypeLoadException::CreateThrowable()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    COUNTER_ONLY(GetPrivatePerfCounters().m_Loading.cLoadFailures++);

    MethodTable *pMT = g_Mscorlib.GetException(kTypeLoadException);

    struct _gc {
        OBJECTREF pNewException;
        STRINGREF pNewAssemblyString;
        STRINGREF pNewClassString;
        STRINGREF pNewMessageArgString;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    GCPROTECT_BEGIN(gc);

    gc.pNewClassString = COMString::NewString(m_fullName);

    if (!m_pMessageArg.IsEmpty())
        gc.pNewMessageArgString = COMString::NewString(m_pMessageArg);

    if (!m_pAssemblyName.IsEmpty())
        gc.pNewAssemblyString = COMString::NewString(m_pAssemblyName);

    gc.pNewException = AllocateObject(pMT);

    MethodDesc* pMD = gc.pNewException->GetTrueMethodTable()->GetClass()->FindMethod(
                            COR_CTOR_METHOD_NAME, &gsig_IM_Str_Str_Str_Int_RetVoid);

    if (!pMD)
    {
        MAKE_WIDEPTR_FROMUTF8(wzMethodName, COR_CTOR_METHOD_NAME);
        COMPlusThrowNonLocalized(kMissingMethodException, wzMethodName);
    }

    MethodDescCallSite exceptionCtor(pMD);

    ARG_SLOT args[] = {
        ObjToArgSlot(gc.pNewException),
        ObjToArgSlot(gc.pNewClassString),
        ObjToArgSlot(gc.pNewAssemblyString),
        ObjToArgSlot(gc.pNewMessageArgString),
        (ARG_SLOT)m_resIDWhy,
    };
    
    exceptionCtor.Call_RetArgSlot(args);

    GCPROTECT_END();
        
    return gc.pNewException;
}

// ---------------------------------------------------------------------------
// EEFileLoadException is an EE exception subclass representing a file loading
// error
// ---------------------------------------------------------------------------

EEFileLoadException::EEFileLoadException(const SString &name, HRESULT hr, IFusionBindLog *pFusionLog, Exception *pInnerException/* = NULL*/)
  : EEException(GetFileLoadKind(hr)),
    m_name(name),
    m_pFusionLog(pFusionLog),
    m_hr(hr)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // We don't want to wrap IsTransient() exceptions. The caller should really have checked this
    // before invoking the ctor. 
    _ASSERTE(pInnerException == NULL || !(pInnerException->IsTransient()));
    m_innerException = pInnerException ? pInnerException->DomainBoundClone() : NULL;

    if (m_name.IsEmpty())
    {
        WCHAR wszTemplate[30];
        if (FAILED(UtilLoadStringRC(IDS_EE_NAME_UNKNOWN,
                                    wszTemplate,
                                    sizeof(wszTemplate)/sizeof(wszTemplate[0]),
                                    FALSE)))
        {
            wszTemplate[0] = L'\0';
        }

        m_name.Set(wszTemplate);
    }

    if (m_pFusionLog != NULL)
        m_pFusionLog->AddRef();
}

EEFileLoadException::~EEFileLoadException()
{
    LEAF_CONTRACT;
    
    if (m_pFusionLog)
        m_pFusionLog->Release();
}



void EEFileLoadException::SetFileName(const SString &fileName, BOOL removePath)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (removePath)
    {
        SString::CIterator i = fileName.End();
        
        if (fileName.FindBack(i, L'\\'))
            i++;

        if (fileName.FindBack(i, L'/'))
            i++;

        m_name.Set(fileName, i, fileName.End());
    }
    else
        m_name.Set(fileName);
}

void EEFileLoadException::GetMessage(SString &result)
{
    WRAPPER_CONTRACT;

    SString sHR;
    GetHRMsg(m_hr, sHR);
    GetResourceMessage(GetResourceIDForFileLoadExceptionHR(m_hr), result, m_name, sHR);
}

/* static */
RuntimeExceptionKind EEFileLoadException::GetFileLoadKind(HRESULT hr)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (Assembly::FileNotFound(hr))
        return kFileNotFoundException;
    else
    {
        // Make sure this matches the list in rexcep.h
        if ((hr == COR_E_BADIMAGEFORMAT) ||
            (hr == CLDB_E_FILE_OLDVER)   ||
            (hr == CLDB_E_FILE_CORRUPT)   ||
            (hr == COR_E_NEWER_RUNTIME)   ||
            (hr == COR_E_ASSEMBLYEXPECTED)   ||
            (hr == HRESULT_FROM_WIN32(ERROR_BAD_EXE_FORMAT)) ||
            (hr == HRESULT_FROM_WIN32(ERROR_EXE_MARKED_INVALID)) ||
            (hr == CORSEC_E_INVALID_IMAGE_FORMAT) ||
            (hr == HRESULT_FROM_WIN32(ERROR_NOACCESS)) ||
            (hr == HRESULT_FROM_WIN32(ERROR_INVALID_DLL))   ||
            (hr == HRESULT_FROM_WIN32(ERROR_INVALID_DLL)) || 
            (hr == HRESULT_FROM_WIN32(ERROR_FILE_CORRUPT)) ||
            (hr == (HRESULT) IDS_CLASSLOAD_32BITCLRLOADING64BITASSEMBLY))
            return kBadImageFormatException;
        else 
        {
            if ((hr == E_OUTOFMEMORY) || (hr == NTE_NO_MEMORY))
                return kOutOfMemoryException;
            else
                return kFileLoadException;
        }
    }
}

OBJECTREF EEFileLoadException::CreateThrowable()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    COUNTER_ONLY(GetPrivatePerfCounters().m_Loading.cLoadFailures++);

    // Fetch any log info from the fusion log
    SString logText;
    if (m_pFusionLog != NULL)
    {
        DWORD dwSize = 0;
        HRESULT hr = m_pFusionLog->GetBindLog(0,0,NULL,&dwSize);
        if (hr==HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) 
        {
            WCHAR *buffer = logText.OpenUnicodeBuffer(dwSize);
            hr=m_pFusionLog->GetBindLog(0,0,buffer, &dwSize);
            logText.CloseBuffer();
        }
    }

    struct _gc {
        OBJECTREF pNewException;
        STRINGREF pNewFileString;
        STRINGREF pFusLogString;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    GCPROTECT_BEGIN(gc);

    gc.pNewFileString = COMString::NewString(m_name);
    gc.pFusLogString = COMString::NewString(logText);
    gc.pNewException = AllocateObject(g_Mscorlib.GetException(m_kind));

    MethodDesc* pMD = gc.pNewException->GetTrueMethodTable()->GetClass()->FindMethod(
                            COR_CTOR_METHOD_NAME, &gsig_IM_Str_Str_Int_RetVoid);

    if (!pMD)
    {
        MAKE_WIDEPTR_FROMUTF8(wzMethodName, COR_CTOR_METHOD_NAME);
        COMPlusThrowNonLocalized(kMissingMethodException, wzMethodName);
    }

    MethodDescCallSite  exceptionCtor(pMD);

    ARG_SLOT args[] = {
        ObjToArgSlot(gc.pNewException),
        ObjToArgSlot(gc.pNewFileString),
        ObjToArgSlot(gc.pFusLogString),
        (ARG_SLOT) m_hr
    };

    exceptionCtor.Call_RetArgSlot(args);

    GCPROTECT_END();

    return gc.pNewException;
}


/* static */
BOOL EEFileLoadException::CheckType(Exception* ex)
{
    RuntimeExceptionKind kind;
    if (ex->IsType(EEException::GetType()))
        kind=((EEException*)ex)->m_kind;
    else
        kind=EEException::GetKindFromHR(ex->GetHR()); 
    
    switch(kind)
    {
        case kFileLoadException:
        case kFileNotFoundException:
        case kBadImageFormatException:
            return TRUE;
        default:
            return FALSE;
    }
};



/* static */
void DECLSPEC_NORETURN EEFileLoadException::Throw(AssemblySpec *pSpec, IFusionBindLog *pFusionLog, HRESULT hr, Exception *pInnerException/* = NULL*/)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (hr == COR_E_THREADABORTED)
        COMPlusThrow(kThreadAbortException);
    if (hr == E_OUTOFMEMORY)
        COMPlusThrowOM();

    StackSString name;
    pSpec->GetFileOrDisplayName(0, name);

    EX_THROW_WITH_INNER(EEFileLoadException, (name, hr, pFusionLog), pInnerException);
}

/* static */
void DECLSPEC_NORETURN EEFileLoadException::Throw(AssemblySpec *pSpec, HRESULT hr, Exception *pInnerException/* = NULL*/)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (hr == COR_E_THREADABORTED)
        COMPlusThrow(kThreadAbortException);
    if (hr == E_OUTOFMEMORY)
        COMPlusThrowOM();

    StackSString name;
    pSpec->GetFileOrDisplayName(0, name);

    EX_THROW_WITH_INNER(EEFileLoadException, (name, hr), pInnerException);
}

/* static */
void DECLSPEC_NORETURN EEFileLoadException::Throw(PEFile *pFile, HRESULT hr, Exception *pInnerException /* = NULL*/)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (hr == COR_E_THREADABORTED)
        COMPlusThrow(kThreadAbortException);
    if (hr == E_OUTOFMEMORY)
        COMPlusThrowOM();

    StackSString name;

    if (pFile->IsAssembly())
        ((PEAssembly*)pFile)->GetDisplayName(name);
    else
        name = StackSString(SString::Utf8, pFile->GetSimpleName());

    EX_THROW_WITH_INNER(EEFileLoadException, (name, hr), pInnerException);
}

/* static */
void DECLSPEC_NORETURN EEFileLoadException::Throw(LPCWSTR path, HRESULT hr, Exception *pInnerException/* = NULL*/)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (hr == COR_E_THREADABORTED)
        COMPlusThrow(kThreadAbortException);
    if (hr == E_OUTOFMEMORY)
        COMPlusThrowOM();

    // Remove path - location must be hidden for security purposes

    LPCWSTR pStart = wcsrchr(path, '\\');
    if (pStart != NULL)
        pStart++;
    else
        pStart = path;

    EX_THROW_WITH_INNER(EEFileLoadException, (StackSString(pStart), hr), pInnerException);
}

/* static */
void DECLSPEC_NORETURN EEFileLoadException::Throw(IAssembly *pIAssembly, IHostAssembly *pIHostAssembly, HRESULT hr, Exception *pInnerException/* = NULL*/)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (hr == COR_E_THREADABORTED)
        COMPlusThrow(kThreadAbortException);
    if (hr == E_OUTOFMEMORY || hr == HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY))
        COMPlusThrowOM();

    StackSString name;

    {
        SafeComHolder<IAssemblyName> pName;
    
        HRESULT newHr;
        
        if (pIAssembly)
            newHr = pIAssembly->GetAssemblyNameDef(&pName);
        else
            newHr = pIHostAssembly->GetAssemblyNameDef(&pName);

        if (SUCCEEDED(newHr))
            FusionBind::GetAssemblyNameDisplayName(pName, name, 0);
    }
        
    EX_THROW_WITH_INNER(EEFileLoadException, (name, hr), pInnerException);
}

/* static */
void DECLSPEC_NORETURN EEFileLoadException::Throw(PEAssembly *parent, 
                                                  const void *memory, COUNT_T size, HRESULT hr, Exception *pInnerException/* = NULL*/)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (hr == COR_E_THREADABORTED)
        COMPlusThrow(kThreadAbortException);
    if (hr == E_OUTOFMEMORY)
        COMPlusThrowOM();

    StackSString name;
    name.Printf("%d bytes loaded from ", size);

    StackSString parentName;
    parent->GetDisplayName(parentName);

    name.Append(parentName);

    EX_THROW_WITH_INNER(EEFileLoadException, (name, hr), pInnerException);
}


// ---------------------------------------------------------------------------
// ObjrefException methods
// ---------------------------------------------------------------------------

ObjrefException::ObjrefException()
{
    LEAF_CONTRACT;
}

ObjrefException::ObjrefException(OBJECTREF throwable)
{
    WRAPPER_CONTRACT;

    SetThrowableHandle(GetAppDomain()->CreateHandle(throwable));
}

Exception *ObjrefException::DomainBoundCloneHelper()
{
    WRAPPER_CONTRACT;
    GCX_COOP();
    return new ObjrefException(GetThrowable());
}


// ---------------------------------------------------------------------------
// CLRLastThrownException methods
// ---------------------------------------------------------------------------

CLRLastThrownObjectException::CLRLastThrownObjectException()
{
    LEAF_CONTRACT;
}

Exception *CLRLastThrownObjectException::CloneHelper()
 {
    WRAPPER_CONTRACT;
    GCX_COOP();
    return new ObjrefException(GetThrowable());
}
  

Exception *CLRLastThrownObjectException::DomainBoundCloneHelper()
{
    WRAPPER_CONTRACT;
    GCX_COOP();
    return new ObjrefException(GetThrowable());
}

OBJECTREF CLRLastThrownObjectException::CreateThrowable()
{
    WRAPPER_CONTRACT;

    DEBUG_STMT(Validate());

    return GetThread()->LastThrownObject();
} // OBJECTREF CLRLastThrownObjectException::CreateThrowable()

#if defined(_DEBUG)
CLRLastThrownObjectException* CLRLastThrownObjectException::Validate()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        DEBUG_ONLY;
    }
    CONTRACTL_END;
    
    // Have to be in coop for GCPROTECT_BEGIN.
    GCX_COOP();

    OBJECTREF throwable = NULL;

    GCPROTECT_BEGIN(throwable);

    throwable = GetThread()->LastThrownObject();

    
    if (throwable == NULL)
    {   // If there isn't a LastThrownObject at all, that's a problem for GetLastThrownObject
        // We've lost track of the exception's type.  Raise an assert.  (This is configurable to allow
        //  stress labs to turn off the assert.)

        static int iSuppress = -1;
        if (iSuppress == -1) 
            iSuppress = REGUTIL::GetConfigDWORD(L"SuppressLostExceptionTypeAssert", 0);
        if (!iSuppress)
        {   
            // Raising an assert message can  cause a mode violation.
            CONTRACT_VIOLATION(ModeViolation);

            // Use DbgAssertDialog to get the formatting right.
            DbgAssertDialog(__FILE__, __LINE__, 
                "The 'LastThrownObject' should not be, but is, NULL.\nThe runtime may have lost track of the type of an exception in flight.\n"
                "  Please get a good stack trace, find the caller of Validate, and file a bug against the owner.\n\n"
                "To suppress this assert 'set COMPLUS_SuppressLostExceptionTypeAssert=1'");
        }
    }
    else
    {   // If there IS a LastThrownObject, then, for
        //  exceptions other than the pre-allocated ones...
        if (!CLRException::IsPreallocatedExceptionObject(throwable))
        {   // ...check that the exception is from the current appdomain.
            if (!throwable->CheckAppDomain(GetAppDomain()))
            {   // We've lost track of the exception's type.  Raise an assert.  (This is configurable to allow
                //  stress labs to turn off the assert.)
    
                static int iSuppress = -1;
                if (iSuppress == -1) 
                    iSuppress = REGUTIL::GetConfigDWORD(L"SuppressLostExceptionTypeAssert", 0);
                if (!iSuppress)
                {   
                    // Raising an assert message can  cause a mode violation.
                    CONTRACT_VIOLATION(ModeViolation);

                    // Use DbgAssertDialog to get the formatting right.
                    DbgAssertDialog(__FILE__, __LINE__, 
                        "The 'LastThrownObject' does not belong to the current appdomain.\nThe runtime may have lost track of the type of an exception in flight.\n"
                        "  Please get a good stack trace, find the caller of Validate, and file a bug against the owner.\n\n"
                        "To suppress this assert 'set COMPLUS_SuppressLostExceptionTypeAssert=1'");
                }
            }
        }
    }

    GCPROTECT_END();

    return this;
} // CLRLastThrownObjectException* CLRLastThrownObjectException::Validate()
#endif // _DEBUG


