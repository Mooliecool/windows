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
//*****************************************************************************
// File: thread.cpp
//
//*****************************************************************************
#include "stdafx.h"
#include "primitives.h"


// Stack-based holder for RSPTRs that we allocated to give to the LS.
// If LS successfully takes ownership of them, then call SuppressRelease().
// Else, dtor will free them up.
template <class T>
class RsPtrHolder
{
    T * m_pObject;
    RsPointer<T> m_ptr;
public:
    RsPtrHolder(T* pObject)
    {
        _ASSERTE(pObject != NULL);
        m_ptr.AllocHandle(pObject->GetProcess(), pObject);
        m_pObject = pObject;
    }

    // If owner didn't call SuppressRelease() to take ownership, then have dtor free it.
    ~RsPtrHolder()
    {
        if (!m_ptr.IsNull())
        {
            T* pObjTest = m_ptr.UnWrapAndRemove(m_pObject->GetProcess());
            _ASSERTE(pObjTest == m_pObject);
        }
    }

    RsPointer<T> Ptr()
    {
        return m_ptr;
    }
    void SuppressRelease()
    {
        m_ptr = RsPointer<T>::NullPtr();
    }

};

/* ------------------------------------------------------------------------- *
 * Managed Thread classes
 * ------------------------------------------------------------------------- */

CordbThread::CordbThread(CordbProcess *process, DWORD id, HANDLE handle)
  : CordbBase(process, id, enumCordbThread),
    m_pContext(NULL),
    m_contextFresh(false),
    m_pAppDomain(NULL),
    m_debugState(THREAD_RUN),
    m_framesFresh(false),
    m_exception(false),
    m_detached(false),
    m_handle(handle),
    m_cachedHandle(INVALID_HANDLE_VALUE),
    m_cachedOutOfProcHandle(INVALID_HANDLE_VALUE)
{
    m_debuggerThreadToken.Set(NULL);
    m_thrown.Set(NULL);

    // Clear more state
    MarkStackFramesDirty();




}

/*
    A list of which resources owned by this object are accounted for.

    UNKNOWN:
        void                 *m_pvLeftSideContext;
        void*                 m_debuggerThreadToken;
        void*                 m_stackBase;
        void*                 m_stackLimit;
        CorDebugThreadState   m_debugState;
        CorDebugUserState     m_userState;
        void                 *m_thrown;
        WCHAR                *m_pstrLogSwitch;
        WCHAR                *m_pstrLogMsg;
        Module               *m_pModuleSpecial;

    HANDLED:
        HANDLE                m_handle; // Closed in ~CordbThread()
        CONTEXT              *m_pContext; // Deleted in ~CordbThread()
        CordbProcess         *m_process; // This pointer created w/o AddRef()                            
        CordbAppDomain       *m_pAppDomain; // This pointer created w/o AddRef()                            
        void                 *m_firstExceptionHandler; //left-side pointer - fs:[0] on x86
        union  {
            Assembly        **m_pAssemblySpecialStack; // Deleted in ~CordbThread()
            Assembly         *m_pAssemblySpecial;
        };
*/

CordbThread::~CordbThread()
{
    _ASSERTE(IsNeutered());

    // Cleared in neuter
    _ASSERTE(m_pContext == NULL);
    _ASSERTE((m_handle == NULL) || (m_handle == SWITCHOUT_HANDLE_VALUE) || (m_handle == INVALID_HANDLE_VALUE));
    _ASSERTE(m_cachedHandle == INVALID_HANDLE_VALUE);
}

// Neutered by the CordbProcess
void CordbThread::Neuter(NeuterTicket ticket)
{
    if (IsNeutered())
    {
        return;
    }


    CleanupStack();


    if ((m_handle != NULL) && (m_handle != SWITCHOUT_HANDLE_VALUE) && (m_handle != INVALID_HANDLE_VALUE))
    {
        CloseHandle(m_handle);
        m_handle = NULL;
    }
    if (m_cachedHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_cachedHandle);
        m_cachedHandle = INVALID_HANDLE_VALUE;
    }

    if( m_pContext != NULL )
    {
        delete [] m_pContext;
        m_pContext = NULL;
    }


    CordbBase::Neuter(ticket);
}

HRESULT CordbThread::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugThread)
    {
        *pInterface = static_cast<ICorDebugThread*>(this);
    }
    else if (id == IID_ICorDebugThread2)
    {
        *pInterface = static_cast<ICorDebugThread2*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugThread*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbThread::GetProcess(ICorDebugProcess **ppProcess)
{
    PUBLIC_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppProcess, ICorDebugProcess **);
    FAIL_IF_NEUTERED(this);

    *ppProcess = GetProcess();
    GetProcess()->ExternalAddRef();

    return S_OK;
}

HRESULT CordbThread::GetID(DWORD *pdwThreadId)
{
    PUBLIC_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pdwThreadId, DWORD *);
    FAIL_IF_NEUTERED(this);

    *pdwThreadId = (DWORD)m_id;

    return S_OK;
}

HRESULT CordbThread::GetHandle(void** phThreadHandle)
{
    PUBLIC_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(phThreadHandle, void**);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;

    HANDLE handle;
    hr = InternalGetHandle(&handle);
    if (SUCCEEDED(hr))
    {
        *phThreadHandle = (void *) handle;
    }

    return hr;
}

// Note that we can return invalid handle
HRESULT CordbThread::InternalGetHandle(HANDLE *phandle)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    HRESULT hr = S_OK;
    if (CLRTaskHosted())
    {
        _ASSERTE(m_handle == SWITCHOUT_HANDLE_VALUE);
        hr = RefreshHandle(phandle);
    }
    else
    {
        *phandle = m_handle;
    }
    return hr;
}

bool CordbThread::CLRTaskHosted()
{
    if (m_handle == SWITCHOUT_HANDLE_VALUE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CordbThread::OwnsFrame(CordbFrame *pFrame)
{
    if ( (pFrame != NULL)           &&
         (!pFrame->IsNeutered())    &&
         (pFrame->m_thread == this)
       )
    {
        if (!m_stackFrames.IsEmpty())
        {
            FramePointer fpLeaf = m_stackFrames[0]->GetFramePointer();
            FramePointer fpRoot = m_stackFrames[m_stackFrames.Length() - 1]->GetFramePointer();

            FramePointer fpCurrent = pFrame->GetFramePointer();

            if (IsEqualOrCloserToLeaf(fpLeaf, fpCurrent) && IsEqualOrCloserToRoot(fpRoot, fpCurrent))
            {
                return true;
            }
        }
    }

    return false;
}

/*
 *
 * RefreshHandle
 *
 *  This routine is a internal helper function for ICorDebugThread2::GetTaskId.
 *
 * Parameters:
 *  phandle - return thread handle here after fetching from the left side. Can return SWITCHOUT_HANDLE_VALUE.
 *
 * Return Value:
 *  hr - It can fail with CORDBG_E_THREAD_NOT_SCHEDULED.
 *
 */
HRESULT CordbThread::RefreshHandle(HANDLE *phandle)
{
    // here is where we will put code in to fetch the thread handle from the left side.
    // This should only happen when CLRTask is hosted.
    // Make sure that we are setting the right HR when thread is being switched out.
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess());  //

    HRESULT hr = S_OK;
    DebuggerIPCEvent event;
    HANDLE handle;

    if (phandle == NULL)
        return E_INVALIDARG;
    *phandle = INVALID_HANDLE_VALUE;


    GetProcess()->InitIPCEvent(&event,
                               DB_IPCE_GET_THREADHANDLE,
                               true,
                               LSPTR_APPDOMAIN::NullPtr());
    event.GetThreadHandle.debuggerThreadToken = m_debuggerThreadToken;

    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event, sizeof(DebuggerIPCEvent));

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        goto exit;

    _ASSERTE(event.type == DB_IPCE_GET_THREADHANDLE_RESULT);

    hr = event.hr;

    handle = event.GetThreadHandleResult.handle;

    if (SUCCEEDED(hr) && handle == SWITCHOUT_HANDLE_VALUE)
    {
        *phandle = SWITCHOUT_HANDLE_VALUE;
        hr = CORDBG_E_THREAD_NOT_SCHEDULED;
    }
    else
    {
        _ASSERTE( handle != INVALID_HANDLE_VALUE);

        // need to dup handle here
        if (handle == m_cachedOutOfProcHandle)
        {
            *phandle = m_cachedHandle;
        }
        else
        {
            BOOL succ = TRUE;
            if (m_cachedHandle != INVALID_HANDLE_VALUE)
            {
                // clear the previous cache
                CloseHandle(m_cachedHandle);
                m_cachedOutOfProcHandle = INVALID_HANDLE_VALUE;
                m_cachedHandle = INVALID_HANDLE_VALUE;
            }

            // now duplicate the handle
            succ = DuplicateHandle(GetProcess()->m_handle,
                                        handle,
                                        GetCurrentProcess(),
                                        &m_cachedHandle,
                                        NULL, FALSE, DUPLICATE_SAME_ACCESS);
            *phandle = m_cachedHandle;

            if (succ)
            {
                m_cachedOutOfProcHandle = handle;
            }
            else
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
        }
    }
exit:
    return hr;
}   // CordbThread::RefreshHandle


HRESULT CordbThread::SetDebugState(CorDebugThreadState state)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    LOG((LF_CORDB, LL_INFO1000, "CT::SDS: thread=0x%08x 0x%x, state=%d\n", this, m_id, state));

    // The detached flag is set as soon as we Queue a ThreadDetach, so it acurately reflects the thread's 
    // actual state.  This check is sufficient because if the debuggee is stopped, either a thread-detach has been 
    // queued or not. If it's queued, then m_Detached == true and this check will catch it.
    // If it's not queued and we suspend the thread, the thread won't be able to slip forward to fire a detach event,
    // and so we won't get a detach until the thread is resumed.    
    if (this->m_detached)
    {
        return CORDBG_E_BAD_THREAD_STATE;
    }

    DebuggerIPCEvent event;
    GetProcess()->InitIPCEvent(&event,
                               DB_IPCE_SET_DEBUG_STATE,
                               true,
                               GetAppDomain()->GetADToken());
    event.SetDebugState.debuggerThreadToken = m_debuggerThreadToken;
    event.SetDebugState.debugState = state;

    HRESULT hr = GetProcess()->SendIPCEvent(&event, sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event.hr);

    if (SUCCEEDED(hr))
        m_debugState = event.SetDebugState.debugState;

    return hr;
}

HRESULT CordbThread::GetDebugState(CorDebugThreadState *pState)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT(pState, CorDebugThreadState *);

    (*pState) = m_debugState;

    return S_OK;;
}

HRESULT CordbThread::GetUserState(CorDebugUserState *pState)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pState, CorDebugUserState *);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = RefreshStack();
    if (FAILED(hr))
        return hr;

    *pState = m_userState;

    return S_OK;
}

HRESULT CordbThread::GetCurrentException(ICorDebugValue **ppExceptionObject)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    HRESULT hr = E_FAIL;

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    if (ppExceptionObject != NULL)
    {
        *ppExceptionObject = NULL;
    }

    if (!m_exception)
    {
        //
        // Go to the LS and retrieve any exception object.
        //
        DebuggerIPCEvent event;
        GetProcess()->InitIPCEvent(&event,
                                   DB_IPCE_GET_EXCEPTION,
                                   true,
                                   GetAppDomain()->GetADToken());
        event.GetException.threadToken = m_debuggerThreadToken;
        event.GetException.exceptionNumber = 0;  // get only the current exception

        hr = GetProcess()->SendIPCEvent(&event, sizeof(DebuggerIPCEvent));
        hr = WORST_HR(hr, event.hr);
        if (!SUCCEEDED(hr))
        {
            return hr;
        }

        _ASSERTE(event.type == DB_IPCE_GET_EXCEPTION_RESULT);

        if (event.GetExceptionResult.exceptionHandle == NULL)
        {
            return S_FALSE;
        }

        // Since we know an exception is in progress on this thread, our assumption about the
        // thread's current AppDomain should be correct
        CordbAppDomain *pDomain = GetAppDomain();
        _ASSERTE( pDomain->GetADToken() == event.appDomainToken );

        m_thrown = event.GetExceptionResult.exceptionHandle;
    }

    // We've believe this assert may fire in the wild.
    // We've seen m_thrown null in retail builds after stack overflow.
    _ASSERTE(m_thrown != NULL);

    VALIDATE_POINTER_TO_OBJECT(ppExceptionObject, ICorDebugValue **);

    ICorDebugReferenceValue * pRefValue = NULL;
    hr = CordbReferenceValue::BuildFromGCHandle(GetAppDomain(), m_thrown, &pRefValue);
    *ppExceptionObject = pRefValue;

    return hr;
}

HRESULT CordbThread::ClearCurrentException()
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    return S_OK;
}

HRESULT CordbThread::CreateStepper(ICorDebugStepper **ppStepper)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(ppStepper, ICorDebugStepper **);

    CordbStepper *stepper = new (nothrow) CordbStepper(this, NULL);

    if (stepper == NULL)
        return E_OUTOFMEMORY;

    stepper->ExternalAddRef();
    *ppStepper = stepper;

    return S_OK;
}

HRESULT CordbThread::EnumerateChains(ICorDebugChainEnum **ppChains)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    HRESULT hr = S_OK;

    VALIDATE_POINTER_TO_OBJECT(ppChains, ICorDebugChainEnum **);
    *ppChains = NULL;

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());


    CordbChainEnum *e = NULL;

    //
    // Refresh the stack frames for this thread.
    //
    hr = RefreshStack();

    if (FAILED(hr))
        goto LExit;

    //
    // Create and return a chain enumerator.
    //
    e = new (nothrow) CordbChainEnum(this);

    if (e != NULL)
    {
        *ppChains = static_cast<ICorDebugChainEnum*> (e);
        e->ExternalAddRef();
    }
    else
    {
        hr = E_OUTOFMEMORY;
        CleanupStack();
    }

LExit:

    return hr;
}

HRESULT CordbThread::GetActiveChain(ICorDebugChain **ppChain)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    HRESULT hr = S_OK;

    VALIDATE_POINTER_TO_OBJECT(ppChain, ICorDebugChain **);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    //
    // Refresh the stack frames for this thread.
    //
    hr = RefreshStack();

    if (FAILED(hr))
        goto LExit;

    if (m_stackChains.IsEmpty())
    {
        *ppChain = NULL;
    }
    else
    {
        CordbChain * pChain = m_stackChains[0];
        pChain->ExternalAddRef();
        (*ppChain) = static_cast<ICorDebugChain *> (pChain);
    }

LExit:
    return hr;
}

HRESULT CordbThread::GetActiveFrame(ICorDebugFrame **ppFrame)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    VALIDATE_POINTER_TO_OBJECT(ppFrame, ICorDebugFrame **);

    (*ppFrame) = NULL;

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;

    //
    // Refresh the stack frames for this thread.
    //
    hr = RefreshStack();

    if (FAILED(hr))
    {
        return hr;
    }

    CordbFrame * pFrame = GetActiveFrame();

    if (pFrame != NULL)
    {
        (*ppFrame) = static_cast<ICorDebugFrame*> (pFrame);
        pFrame->ExternalAddRef();
    }

    return hr;
}

// Internal Helper to get the leafmost frame.
// Only valid once we're stopped
CordbFrame * CordbThread::GetActiveFrame()
{
    INTERNAL_SYNC_API_ENTRY(GetProcess());

    if (m_stackFrames.IsEmpty() || (m_stackFrames[0]->m_chain != m_stackChains[0]))
    {
        return NULL;
    }
    
    return m_stackFrames[0];
}


HRESULT CordbThread::GetRegisterSet(ICorDebugRegisterSet **ppRegisters)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    VALIDATE_POINTER_TO_OBJECT(ppRegisters, ICorDebugRegisterSet **);

    *ppRegisters = NULL;

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;

    //
    // Refresh the stack frames for this thread.
    //
    hr = RefreshStack();

    if (FAILED(hr))
        goto LExit;

    _ASSERTE( !m_stackChains.IsEmpty());
    _ASSERTE( m_stackChains[0] != NULL );

    hr = m_stackChains[0]->GetRegisterSet( ppRegisters );

LExit:
    return hr;
}

HRESULT CordbThread::CreateEval(ICorDebugEval **ppEval)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(ppEval, ICorDebugEval **);

    CordbEval *eval = new (nothrow) CordbEval(this);

    if (eval == NULL)
        return E_OUTOFMEMORY;

    eval->ExternalAddRef();
    *ppEval = static_cast<ICorDebugEval*> (eval);

    return S_OK;
}

// DAC check

// Double check our results w/ DAC.
// This gives DAC some great coverage.
// Given an IP and the md token (that the RS obtained), use DAC to lookup the md token. Then
// we can compare DAC & the RS and make sure DACs working.
void CheckAgainstDAC(CordbFunction * pFunc, void * pIP, mdMethodDef mdExpected)
{
}


//-----------------------------------------------------------------------------
// Send a IPC events to the LS to build up the stack.
//-----------------------------------------------------------------------------
HRESULT CordbThread::RefreshStack(void)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    // Must have the Stop-Go lock to change our thread's stack-state.
    _ASSERTE(GetProcess()->GetStopGoLock()->HasLock());

    // Our caller should have guaranteed that we're synced. And b/c we hold the stop-go lock,
    // that shouldn't have changed.
    _ASSERTE(GetProcess()->GetSynchronized());


    HRESULT hr = S_OK;
    unsigned int totalTraceCount = 0;
    unsigned int inProgressFrameCount = 0; //so we can CleanupStack w/o bombing
    unsigned int inProgressChainCount = 0; //so we can CleanupStack w/o bombing
    bool wait = true;

    CordbChain *chain = NULL; // current chain

    

    if (m_framesFresh)
        return S_OK;

    if (CLRTaskHosted())
    {
        HANDLE  handle;
        hr = InternalGetHandle(&handle);
        if (FAILED(hr))
        {
            return hr;
        }
        if (handle == SWITCHOUT_HANDLE_VALUE)
        {
            // cannot get stack trace on the switched out thread.
            return CORDBG_E_THREAD_NOT_SCHEDULED;
        }
    }

    //
    // Clean up old snapshot.
    //
    CleanupStack();

    //
    // If we don't have a debugger thread token, then this thread has never
    // executed managed code and we have no frame information for it.
    //
    if (m_debuggerThreadToken == NULL)
        return E_FAIL;

    //
    // Send the stack trace event to the RC.
    //
    DebuggerIPCEvent *event =
      (DebuggerIPCEvent *) _alloca(CorDBIPC_BUFFER_SIZE);

    GetProcess()->InitIPCEvent(event,
                            DB_IPCE_STACK_TRACE,
                            false,
                            GetAppDomain()->GetADToken());
    event->StackTraceData.debuggerThreadToken = m_debuggerThreadToken;

    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), event,
                                          CorDBIPC_BUFFER_SIZE);
    hr = WORST_HR(hr, event->hr);
    //
    // Stop now if we can't even send the event.
    //
    if (!SUCCEEDED(hr))
        goto exit;

    m_userState = (CorDebugUserState)0;
    LOG((LF_CORDB,LL_INFO1000, "CT::RS:thread:0x%x zeroing out "
        "userThreadState:\n", m_id));


    //
    // Wait for events to return from the RC. We expect at least one
    // stack trace result event.
    //
    while (wait)
    {
         hr = ErrWrapper(GetProcess()->m_cordb->WaitForIPCEventFromProcess(GetProcess(),
                                                             GetAppDomain(),
                                                             event));

        if (!SUCCEEDED(hr))
        {
            CONSISTENCY_CHECK_MSGF(
             (hr == CORDBG_E_PROCESS_TERMINATED) ||
             (hr == CORDBG_E_BAD_THREAD_STATE), ("Failure: hr=0x%08x\n", hr));
             
            goto exit;
        }            

        //
        //
        _ASSERTE(event->type == DB_IPCE_STACK_TRACE_RESULT);

        //
        // If this is the first event back from the RC then create the
        // array to hold the frame pointers.
        //
        if (totalTraceCount == 0)
        {
            // This array will be filled w/ CordbNativeFrame and CordbInternalFrame
            // Note we may have 0 frames.
            if (!m_stackFrames.Alloc(event->StackTraceResultData.totalFrameCount))
            {
                hr = ErrWrapper(E_OUTOFMEMORY);
                goto exit;
            }

            _ASSERTE(inProgressFrameCount == 0);
            

            //
            //
            if (!m_stackChains.Alloc(event->StackTraceResultData.totalChainCount))
            {
                hr = ErrWrapper(E_OUTOFMEMORY);
                goto exit;
            }
            _ASSERTE(m_stackChains.Length() > 0); // should always have at least 1 for ThreadStart chain
            _ASSERTE(inProgressChainCount == 0);
            _ASSERTE(chain == NULL);

            //
            // Remember our context.
            //

            if (event->StackTraceResultData.pContext != NULL)
                m_pvLeftSideContext = event->StackTraceResultData.pContext;

            // While we're doing once-only work, remember the User state of the
            // thread
            m_userState = event->StackTraceResultData.threadUserState;
            LOG((LF_CORDB,LL_INFO1000, "CT::RS:thread:0x%x userThreadState:0x%x\n",
                m_id, m_userState));
        }

        //
        // Go through each returned frame in the event and build a
        // CordbFrame for it.
        //
        DebuggerIPCE_STRData* currentSTRData =
            &(event->StackTraceResultData.traceData);

        unsigned int traceCount = 0;

        while (traceCount < event->StackTraceResultData.traceCount)
        {
            // If we don't have a current chain object, create one now. We'll finish the construction
            // once we get a real chain marker.
            if (chain == NULL)
            {
                chain = new (nothrow) CordbChain(this, TRUE,
                                              m_stackFrames.UnsafeGetAddrOfIndex(inProgressFrameCount), 
                                              NULL, inProgressChainCount);

                if (chain==NULL)
                {
                    hr = ErrWrapper(E_OUTOFMEMORY);
                    goto exit;
                }

                // Ensure the LS isn't lying to us about buffer sizes.
                if (inProgressChainCount == m_stackChains.Length())
                {
                    _ASSERTE(!"Illegal data from LS");
                    hr = CORDBG_E_INCOMPATIBLE_PROTOCOL;
                    goto exit;
                }
                m_stackChains.Assign(inProgressChainCount, chain);                
                inProgressChainCount++;
            }


            if (currentSTRData->eType == DebuggerIPCE_STRData::cStubFrame)
            {
                CordbAppDomain * pAppDomain = NULL;
                if (currentSTRData->currentAppDomainToken != NULL)
                {
                    pAppDomain = (CordbAppDomain*) GetProcess()->m_appDomains.GetBase(
                        LsPtrToCookie(currentSTRData->currentAppDomainToken));
                    PREFIX_ASSUME(pAppDomain != NULL);
                }

                UINT iFrameInChain = (UINT)(m_stackFrames.UnsafeGetAddrOfIndex(inProgressFrameCount) - chain->m_start);
                // Create the Internal frame.
                CordbInternalFrame* pInternalFrame = new (nothrow) CordbInternalFrame(
                    chain,
                    currentSTRData->fp,
                    &(currentSTRData->rd),
                    iFrameInChain,
                    pAppDomain,
                    currentSTRData
                    );

                if (NULL == pInternalFrame)
                {
                    hr = ErrWrapper(E_OUTOFMEMORY);
                    goto exit;
                }

                // Ensure the LS isn't lying to us about buffer sizes.
                if (inProgressFrameCount == m_stackFrames.Length())
                {
                    _ASSERTE(!"Illegal data from LS,2");
                    hr = CORDBG_E_INCOMPATIBLE_PROTOCOL;
                    goto exit;
                }

                // Add this frame into the array
                m_stackFrames.Assign(inProgressFrameCount, pInternalFrame);
                inProgressFrameCount++;
            }
            else if (currentSTRData->eType == DebuggerIPCE_STRData::cChain)
            {
                chain->m_end = m_stackFrames.UnsafeGetAddrOfIndex(inProgressFrameCount);
                chain->m_reason = currentSTRData->u.chainReason;
                chain->m_managed = currentSTRData->u.managed;
                chain->m_context = PTR_TO_CORDB_ADDRESS(currentSTRData->u.context);

                // If we're a leaf UM chain, then get the RegDisplay from the thread's context.
                bool fLeafUMChain = (!chain->m_managed) && (traceCount == 0) && (totalTraceCount == 0);
                bool fIsInteropDebugging = ((GetProcess()->m_state & CordbProcess::PS_WIN32_ATTACHED) != 0);

                if (fLeafUMChain && fIsInteropDebugging)
                {       
                    // We can only get the live thread context while interop-debugging.
                    CONTEXT context;
                    context.ContextFlags = CONTEXT_FULL;
                    HRESULT hrCopy = GetProcess()->GetThreadContext((DWORD)m_id, sizeof(context), (BYTE*) &context);

                    // We're passing valid params so this should always succeed. 
                    // If for some wacky reason we fail, we don't propogate this HR out of RefreshStack
                    // because that would fail the overall stackwalk and then they'd have nothing.
                    // We can give just leave the leafmost context as garbage and give them the rest 
                    // of stack trace (eg, managed portions). in tact 
                    _ASSERTE(SUCCEEDED(hrCopy)); 
                    
                    // Copy live context into the RegDisplay.
                    CORDbgSetDebuggerREGDISPLAYFromContext(&chain->m_rd, &context);
                }
                else
                {
                    chain->m_rd = currentSTRData->rd;
                }
                
                chain->m_quicklyUnwound = currentSTRData->quicklyUnwound;
                chain->m_id = 0;
                chain->m_fp = currentSTRData->fp;

                // Clear out the current chain so that we'll create a new one next time through the loop.
                // The thread's chain array already has a reference to it.
                chain = NULL;
            }
            else
            {
                _ASSERTE(currentSTRData->eType == DebuggerIPCE_STRData::cMethodFrame);

                DebuggerIPCE_FuncData* currentFuncData = &currentSTRData->v.funcData;
                DebuggerIPCE_JITFuncData* currentJITFuncData = &currentSTRData->v.jitFuncData;


                // Lookup the appdomain that the thread was in when it was executing code for this frame. We pass this
                // to the frame when we create it so we can properly resolve locals in that frame later.
                CordbAppDomain *currentAppDomain = GetProcess()->m_appDomains.GetBase(
                                                                           LsPtrToCookie(currentSTRData->currentAppDomainToken));
                _ASSERTE(currentAppDomain != NULL);


                CordbFunction *pFunction = NULL;
                if (FAILED(hr = CordbFunction::LookupOrCreateFromFuncData(
                    GetProcess(), currentAppDomain, currentFuncData, currentJITFuncData->enCVersion, &pFunction)))
                {
                    goto exit;
                }

                CordbJITInfo *pInfo;
                if (FAILED(hr = CordbJITInfo::LookupOrCreateFromJITData(pFunction,currentFuncData, currentJITFuncData, &pInfo)))
                {
                    _ASSERTE( !"FAILURE" );
                    goto exit;
                }

                _ASSERTE(pInfo != NULL);

#if defined(_DEBUG)
                // Double check our results w/ DAC.
                CheckAgainstDAC(pFunction, (void*) currentSTRData->rd.PC, currentFuncData->funcMetadataToken);
#endif



                // Create the native frame.
                CordbNativeFrame* nativeFrame =
                    new (nothrow) CordbNativeFrame(chain,
                                                   currentSTRData->fp,
                                                   pInfo,
                                                   (UINT_PTR) currentJITFuncData->nativeOffset,
                                                   &(currentSTRData->rd),
                                                   currentSTRData->v.ambientESP,
                                                   currentSTRData->quicklyUnwound,
                                                   (UINT)(m_stackFrames.UnsafeGetAddrOfIndex(inProgressFrameCount) - chain->m_start),
                                                   currentAppDomain
                                                   WIN64_ARG(&misc));

                if (NULL == nativeFrame )
                {
                    hr = ErrWrapper(E_OUTOFMEMORY);
                    goto exit;
                }

                // Ensure the LS isn't lying to us about buffer sizes.
                if (inProgressFrameCount == m_stackFrames.Length())
                {
                    _ASSERTE(!"Illegal data from LS,3");
                    hr = CORDBG_E_INCOMPATIBLE_PROTOCOL;
                    goto exit;
                }

                // Add this frame into the array
                m_stackFrames.Assign(inProgressFrameCount, nativeFrame);
                inProgressFrameCount++;

                // Step 2. Find or create the IL Code, and the JITILFrame.
                if (currentSTRData->v.ILIP != NULL)
                {
                    CordbCode* pCode = NULL;
                        
                    // This should always work. If the function already has an IL code object, we get that
                    // else this will create a code-object and return that.
                    hr = pFunction->GetOrCreateILCode(
                        &pCode, // no reference added here
                        currentFuncData->ilStartAddress,
                        currentFuncData->ilSize,
                        currentJITFuncData->enCVersion
                    );
                    if (FAILED(hr))
                    {
                        _ASSERTE((hr == E_OUTOFMEMORY) || !"FAILURE" );
                        goto exit;
                    }
                    _ASSERTE(pCode != NULL);


                    CordbJITILFrame* JITILFrame =
                      new (nothrow) CordbJITILFrame(nativeFrame, pCode,
                                          (UINT_PTR) currentSTRData->v.ILIP
                                          - (UINT_PTR) currentFuncData->ilStartAddress,
                                          currentSTRData->v.mapping,
                                          (currentJITFuncData->isInstantiatedGeneric
                                           ? currentSTRData->v.exactGenericArgsToken
                                           : NULL),
                                          currentSTRData->v.varargs.fVarArgs,
                                          currentSTRData->v.varargs.rpSig,
                                          (ULONG)currentSTRData->v.varargs.cbSig,
                                          currentSTRData->v.varargs.rpFirstArg);

                    if (!JITILFrame)
                    {
                        hr = ErrWrapper(E_OUTOFMEMORY);
                        goto exit;
                    }

                    // Initialize the frame.
                    hr = JITILFrame->Init();

                    if (!SUCCEEDED(hr))
                    {
                        delete JITILFrame;
                        goto exit;
                    }

                    nativeFrame->m_JITILFrame.Assign(JITILFrame);
                } // end IL code


            } // end Method

            currentSTRData++;
            traceCount++;
        } // end while

        totalTraceCount += traceCount;

        S_UINT32 tmpTraceCount = S_UINT32( m_stackFrames.Length() ) + S_UINT32( m_stackChains.Length());
        if ( tmpTraceCount.IsOverflow() )
        {
            hr = E_OUTOFMEMORY;
            goto exit;
        }
        if (totalTraceCount >= tmpTraceCount.Value() )
        {
            wait = false;
        }
    }

exit:    
    if (SUCCEEDED(hr))
    {
        // If the LS sends us corrupted data, then we may not finish.
        // This should never happen unless we have a bug in the LS.
        if (inProgressChainCount != m_stackChains.Length())
        {
            CONSISTENCY_CHECK_MSGF(false, ("bad chains in RefreshStack:%d of %d\n", inProgressChainCount, m_stackChains.Length()));
            STRESS_LOG2(LF_CORDB, LL_INFO1000, "bad chains in RefreshStack:%d of %d\n", inProgressChainCount, m_stackChains.Length());
            hr = CORDBG_E_INCOMPATIBLE_PROTOCOL;
        }
        if (inProgressFrameCount != m_stackFrames.Length())
        {
            CONSISTENCY_CHECK_MSGF(false, ("bad frames in RefreshStack:%d of %d\n", inProgressFrameCount, m_stackFrames.Length()));
            STRESS_LOG2(LF_CORDB, LL_INFO1000, "bad frames  in RefreshStack:%d of %d\n", inProgressFrameCount, m_stackFrames.Length());
            hr = CORDBG_E_INCOMPATIBLE_PROTOCOL;
        }
    }
    
    if (!SUCCEEDED(hr))
    {        
        CleanupStack(); // sets frames fresh to false
    }
    else
    {
        _ASSERTE(totalTraceCount == inProgressFrameCount + inProgressChainCount);

        m_framesFresh = true;
    }

    return hr;
}


void CordbThread::CleanupStack()
{
    // Must have the Stop-Go lock to change our thread's stack-state.
    NeuterTicket ticket(GetProcess());

    // Neuter outstanding CordbChainEnum, CordbFrameEnum and some CordbValueEnums.
    m_RefreshStackNeuterList.NeuterAndClear(ticket);

    m_stackFrames.NeuterAndClear(ticket);
    m_stackChains.NeuterAndClear(ticket);

    // If the stack is old, then the CONTEXT (if any) is out of date
    // as well.
    m_contextFresh = false;
    m_pvLeftSideContext.Set(NULL);
    m_framesFresh = false;
}

// Notifying the thread that the process is being continued.
// this will causwe our caches to get invalidated.
void CordbThread::MarkStackFramesDirty(void)
{
    LEAF_CONTRACT;

    m_framesFresh = false;

    // If we've setup an outstanding func-eval, then continung will eventually bring
    // us back to our exception (when the func-eval exits). So we leave
    m_exception = false;

    m_contextFresh = false;
    m_pvLeftSideContext.Set(NULL);
    m_EnCRemapFunctionIP = NULL;
}

// Set that there's an outstanding exception on this thread.
// This can be called when the process object receives an exception notification.
// This is cleared in MarkStackFramesDirty.
void CordbThread::SetExInfo(LSPTR_OBJECTHANDLE ohThrown)
{
    _ASSERTE(GetProcess()->GetSynchronized());

    m_exception = true;
    m_thrown = ohThrown;

    // CordbThread::GetCurrentException assumes that we always have a m_thrown when at an exception.
    // Push that assert up here.
    _ASSERTE(m_thrown != NULL);
}


// Given a FramePointer, find the matching CordbFrame.
HRESULT CordbThread::FindFrame(CordbFrame** ppFrame, FramePointer fp)
{
    FAIL_IF_NEUTERED(this);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    _ASSERTE(ppFrame != NULL);
    *ppFrame = NULL;

    //
    // Refresh the stack frames for this thread and find pFrame on it.
    //
    if (FAILED(RefreshStack()))
    {
        return E_FAIL;
    }

    for (unsigned int index = 0; index < m_stackFrames.Length(); index++)
    {
        if (m_stackFrames[index]->IsContainedInFrame(fp))
        {
            *ppFrame = m_stackFrames[index];
            return S_OK;
        }
    }

    // Cannot find the frame.
    return E_FAIL;
}



const bool SetIP_fCanSetIPOnly = TRUE;
const bool SetIP_fSetIP = FALSE;

const bool SetIP_fIL = TRUE;
const bool SetIP_fNative = FALSE;

HRESULT CordbThread::SetIP( bool fCanSetIPOnly,
                            LSPTR_DMODULE debuggerModule,
                            mdMethodDef mdMethod,
                            LSPTR_DJI nativeCodeJITInfoToken,
                            SIZE_T offset,
                            bool fIsIL)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);


    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());


    _ASSERTE(m_firstExceptionHandler != NULL);
    _ASSERTE(debuggerModule != NULL);

    // If this thread is stopped due to an exception, never allow SetIP
    if (m_exception)
    {
       return (CORDBG_E_SET_IP_NOT_ALLOWED_ON_EXCEPTION);
    }

    DebuggerIPCEvent *event = (DebuggerIPCEvent*) _alloca(CorDBIPC_BUFFER_SIZE);
    GetProcess()->InitIPCEvent(event,
                            DB_IPCE_SET_IP,
                            true,
                            GetAppDomain()->GetADToken());
    event->SetIP.fCanSetIPOnly = fCanSetIPOnly;
    event->SetIP.debuggerThreadToken = m_debuggerThreadToken;
    event->SetIP.debuggerModule = debuggerModule;
    event->SetIP.mdMethod = mdMethod;
    event->SetIP.nativeCodeJITInfoToken = nativeCodeJITInfoToken;
    event->SetIP.offset = offset;
    event->SetIP.fIsIL = fIsIL;
    event->SetIP.firstExceptionHandler = m_firstExceptionHandler;

    LOG((LF_CORDB, LL_INFO10000, "[%x] CT::SIP: Info:thread:0x%x"
        "mod:0x%x  MethodDef:0x%x NativeTok:0x%x offset:0x%x  il?:0x%x\n",
        GetCurrentThreadId(),LsPtrToCookie(m_debuggerThreadToken), LsPtrToCookie(debuggerModule),
        mdMethod, LsPtrToCookie(nativeCodeJITInfoToken),offset, fIsIL));

    LOG((LF_CORDB, LL_INFO10000, "[%x] CT::SIP: sizeof(DebuggerIPCEvent):0x%x **********\n",
        sizeof(DebuggerIPCEvent)));

    HRESULT hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), event,
                                                  sizeof(DebuggerIPCEvent));

    if (FAILED( hr ) )
        return hr;

    _ASSERTE(event->type == DB_IPCE_SET_IP);

    if (!fCanSetIPOnly && SUCCEEDED(event->hr))
    {
        m_framesFresh = false;
        hr = RefreshStack();
        if (FAILED(hr))
            return hr;
    }

    return ErrWrapper(event->hr);
}

// Get the context from a thread in managed code.
// This thread should be stopped gracefully by the LS in managed code.
HRESULT CordbThread::GetManagedContext(CONTEXT **ppContext)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess());

    if (ppContext == NULL)
        return E_INVALIDARG;

    *ppContext = NULL;
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // Each CordbThread object allocates the m_pContext's CONTEXT structure only once, the first time GetContext is
    // invoked.
    if(m_pContext == NULL)
    {
        m_pContext = (CONTEXT*) new (nothrow) BYTE[sizeof(CONTEXT)];

        if (m_pContext == NULL)
        {
            return E_OUTOFMEMORY;
        }
    }

    HRESULT hr = S_OK;

    if (m_contextFresh == false)
    {

        hr = RefreshStack();

        if (FAILED(hr))
            return hr;

        if (m_pvLeftSideContext == NULL)
        {
            // We don't have a context in managed code.
            return CORDBG_E_CONTEXT_UNVAILABLE;
        }
        else
        {
            LOG((LF_CORDB, LL_INFO1000, "CT::GC: getting context from left side pointer.\n"));

            // The thread we're examining IS handling an exception, So grab the CONTEXT of the exception, NOT the
            // currently executing thread's CONTEXT (which would be the context of the exception handler.)
            hr = GetProcess()->SafeReadThreadContext(m_pvLeftSideContext, m_pContext);
        }

        // m_contextFresh should be marked false when CleanupStack, MarkAllFramesAsDirty, etc get called.
        if (SUCCEEDED(hr))
            m_contextFresh = true;
    }

    if (SUCCEEDED(hr))
    {
        (*ppContext) = m_pContext;
    }

    return hr;
}

HRESULT CordbThread::SetManagedContext(CONTEXT *pContext)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    if(pContext == NULL)
        return E_INVALIDARG;

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = RefreshStack();

    if (FAILED(hr))
        return hr;

    if (m_pvLeftSideContext == NULL)
    {
        return CORDBG_E_CONTEXT_UNVAILABLE;
    }
    else
    {
        // The thread we're examining IS handling an exception, So set the CONTEXT of the exception, NOT the currently
        // executing thread's CONTEXT (which would be the context of the exception handler.)
        //
        // Note: we read the remote context and merge the new one in, then write it back. This ensures that we don't
        // write too much information into the remote process.
        CONTEXT tempContext;
        hr = GetProcess()->SafeReadThreadContext(m_pvLeftSideContext, &tempContext);

        if (SUCCEEDED(hr))
        {
            CORDbgCopyThreadContext(&tempContext, pContext);

            hr = GetProcess()->SafeWriteThreadContext(m_pvLeftSideContext, &tempContext);
        }

    }

    if (SUCCEEDED(hr) && m_contextFresh && (m_pContext != NULL))
    {
        *m_pContext = *pContext;
    }

    return hr;
}


HRESULT CordbThread::GetAppDomain(ICorDebugAppDomain **ppAppDomain)
{
    // We don't use the cached m_pAppDomain pointer here because it might be incorrect
    // if the thread has transitioned to another domain but we haven't received any events
    // from it yet.  So we need to ask the left-side for the current domain.
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    VALIDATE_POINTER_TO_OBJECT(ppAppDomain, ICorDebugAppDomain **);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    CordbAppDomain* pAppDomain = NULL;
    HRESULT hr = GetCurrentAppDomain( &pAppDomain );
    if( !SUCCEEDED(hr) )
    {
        return hr;
    }
    _ASSERTE( pAppDomain != NULL );
    
    *ppAppDomain = static_cast<ICorDebugAppDomain *> (pAppDomain);
    pAppDomain->ExternalAddRef();
    
    return S_OK;
}

HRESULT CordbThread::GetCurrentAppDomain(CordbAppDomain** ppAppDomain)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess());

    *ppAppDomain = NULL;
    
    DebuggerIPCEvent event;
    GetProcess()->InitIPCEvent(&event,
                            DB_IPCE_GET_CURRENT_APPDOMAIN,
                            true,
                            LSPTR_APPDOMAIN::NullPtr() );

    event.GetCurrentAppDomain.threadToken = m_debuggerThreadToken;

    // Note: two-way event here...
    HRESULT hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event,
                                          sizeof(DebuggerIPCEvent));

    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_GET_CURRENT_APPDOMAIN_RESULT);

    if (!SUCCEEDED(event.hr))
    {
        return event.hr;
    }

    CordbAppDomain* pAppDomain = NULL;
    if (event.GetCurrentAppDomainResult.currentAppDomainToken != NULL)
    {
        pAppDomain = GetProcess()->m_appDomains.GetBase(
            LsPtrToCookie(event.GetCurrentAppDomainResult.currentAppDomainToken));
        _ASSERTE( pAppDomain != NULL );     // we should be aware of all AppDomains
    }
    if( pAppDomain == NULL )
    {
        // If we don't know where the thread is then fail.
        // I'm not sure if this should ever happen, but it accurately reflects the state
        // of the left side (don't want to lie).
        return E_FAIL;
    }

    *ppAppDomain = pAppDomain;
    return S_OK;
}

HRESULT CordbThread::GetObject(ICorDebugValue **ppThreadObject)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    HRESULT hr;

    VALIDATE_POINTER_TO_OBJECT(ppThreadObject, ICorDebugObjectValue **);

    // Default to NULL
    *ppThreadObject = NULL;

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    if (m_detached)
        return CORDBG_E_BAD_THREAD_STATE;

    // Get the address of this thread's managed object from the
    // left side.
    DebuggerIPCEvent event;

    GetProcess()->InitIPCEvent(&event,
                            DB_IPCE_GET_THREAD_OBJECT,
                            true,
                            LSPTR_APPDOMAIN::NullPtr());

    event.ObjectRef.debuggerObjectToken = m_debuggerThreadToken;

    // Note: two-way event here...
    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event,
                                          sizeof(DebuggerIPCEvent));

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        return hr;

    _ASSERTE(event.type == DB_IPCE_THREAD_OBJECT_RESULT);

    if (!SUCCEEDED(event.hr))
        return event.hr;

    LSPTR_OBJECTHANDLE pObjectHandle = event.ObjectRef.managedObject;
    if (pObjectHandle.IsNull())
    {
        return E_FAIL;
    }

    // We create the object relative to the current AppDomain of the thread
    // Thread objects aren't really agile (eg. their m_Context field is domain-bound and
    // fixed up manually during transitions).  This means that a thread object can only
    // be used in the domain the thread was in when the object was created.
    _ASSERTE( event.appDomainToken != NULL);
    CordbAppDomain* pThreadCurrentDomain = 
        GetProcess()->m_appDomains.GetBase( LsPtrToCookie(event.appDomainToken) );
    _ASSERTE( pThreadCurrentDomain != NULL );     // we should be aware of all AppDomains
    if( pThreadCurrentDomain == NULL )
    {
        // fall back to some domain to avoid crashes in retail - safe enough for getting the name of the thread etc.
        pThreadCurrentDomain = GetProcess()->GetDefaultAppDomain();
    }
    
    ICorDebugReferenceValue * pRefValue = NULL;
    hr = CordbReferenceValue::BuildFromGCHandle(pThreadCurrentDomain, pObjectHandle, &pRefValue);
    *ppThreadObject = pRefValue;

    // Don't return a null pointer with S_OK.
    _ASSERTE(!(hr == S_OK && *ppThreadObject == NULL));
    return hr;
}

/*
 *
 * GetActiveFunctions
 *
 *  This routine is the interface function for ICorDebugThread2::GetActiveFunctions.
 *
 * Parameters:
 *  cFunctions - the count of the number of COR_ACTIVE_FUNCTION in pFunctions.  Zero
 *               indicates no pFunctions buffer.
 *  pcFunctions - pointer to storage for the count of elements filled in to pFunctions, or
 *                count that would be needed to fill pFunctions, if cFunctions is 0.
 *  pFunctions - buffer to store results.  May be NULL.
 *
 * Return Value:
 *  HRESULT from the helper routine.
 *
 */

HRESULT CordbThread::GetActiveFunctions(
    ULONG32 cFunctions,
    ULONG32 *pcFunctions,
    COR_ACTIVE_FUNCTION pFunctions[])
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    HRESULT hr;
    ULONG32 index;
    ULONG32 iRealIndex;
    ULONG32 last;

    if (((cFunctions != 0) && (pFunctions == NULL)) || (pcFunctions == NULL))
    {
        return E_INVALIDARG;
    }

    //
    // Default to 0
    //
    *pcFunctions = 0;

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());


    if (m_detached)
    {
        //
        // Return zero active functions on this thread.
        //
        return S_OK;
    }


    //
    // Refresh the stack frames for this thread.
    //

    hr = RefreshStack();

    if (FAILED(hr))
    {

        if (hr == CORDBG_E_BAD_THREAD_STATE)
        {
            hr = S_OK;
        }
        goto LExit;
    }


    // Only return NativeFrames (not internal frames). Need to go through and count.
    {
        ULONG32 cTotal = 0;
        for (index = 0; index < m_stackFrames.Length(); index++)
        {
            if (m_stackFrames[index]->GetAsNativeFrame())
            {
                cTotal++;
            }
        }
        *pcFunctions = cTotal;
    }


    //
    // If all we want is the count, then return that.
    //
    if ((pFunctions == NULL) || (cFunctions == 0))
    {
        hr = S_OK;
        goto LExit;

    }


    //
    // Now go down list of frames, storing information
    //
    last = (cFunctions < m_stackFrames.Length()) ? cFunctions : m_stackFrames.Length();
    iRealIndex = 0;
    index =0;

    while((index < last) && (iRealIndex < m_stackFrames.Length()))
    {
        CordbFrame * pThisFrame = m_stackFrames[iRealIndex];
        CordbNativeFrame * pNativeFrame = pThisFrame->GetAsNativeFrame();

        iRealIndex++;
        if (pNativeFrame == NULL)
        {
            _ASSERTE(pThisFrame->GetAsInternalFrame() != NULL);

            // This is an internal frame, so skip it.
            continue;
        }

        //
        // Fill in the easy stuff.
        //
        CordbFunction *pFunction;

        pFunction = (static_cast<CordbFrame*> (pNativeFrame))->GetFunction();
        ASSERT(pFunction != NULL);

        hr = pFunction->QueryInterface(IID_ICorDebugFunction2, (void **)&(pFunctions[index].pFunction));
        ASSERT(!FAILED(hr));

        CordbModule * pModule = pFunction->GetModule();
        pFunctions[index].pModule = pModule;
        pModule->ExternalAddRef();

        CordbAppDomain * pAppDomain = pNativeFrame->GetCurrentAppDomain();
        pFunctions[index].pAppDomain = pAppDomain;
        pAppDomain->ExternalAddRef();

        pFunctions[index].flags = 0;

        //
        // Now go to the IL frame (if one exists) to the get the offset.
        //
        CordbJITILFrame *pJITILFrame;

        pJITILFrame = pNativeFrame->m_JITILFrame;

        if (pJITILFrame != NULL)
        {

            hr = pJITILFrame->GetIP(&(pFunctions[index].ilOffset), NULL);
            ASSERT(!FAILED(hr));

        } else {

            pFunctions[index].ilOffset = (DWORD) NO_MAPPING;

        }

        // Update to the next count.
        index++;
    }




LExit:
    return hr;
}

/*
 *
 * InterceptCurrentException
 *
 *  This routine is the interface function for ICorDebugThread2::InterceptCurrentException
 *
 * Parameters:
 *  pFrame - The stack frame to intercept on.
 *
 * Return Value:
 *  HRESULT from the helper routine.
 *
 */

HRESULT CordbThread::InterceptCurrentException(
    ICorDebugFrame *pFrame)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    
    HRESULT hr;
    DebuggerIPCEvent event;

    if (pFrame == NULL)
    {
        return E_INVALIDARG;
    }

    //
    // Verify we were passed a real stack frame, and not an internal
    // CLR mocked up one.
    //
    {
        RSExtSmartPtr<ICorDebugInternalFrame> pInternalFrame;
        hr = pFrame->QueryInterface(IID_ICorDebugInternalFrame, (void **)&pInternalFrame);

        if (!FAILED(hr))
        {
            return E_INVALIDARG;
        }
    }        


    if (m_detached)
    {
        //
        // Return zero active functions on this thread.
        //
        return E_FAIL;
    }

    //
    // Refresh the stack frames for this thread and verify pFrame is on it.
    //

    hr = RefreshStack();

    if (FAILED(hr))
    {
        return hr;
    }

    //
    // Now check if the frame is valid
    //

    CordbFrame* pRealFrame = CordbFrame::GetCordbFrameFromInterface(pFrame);
    if (!OwnsFrame(pRealFrame))
    {
        hr = E_INVALIDARG;
        goto LExit;
    }

    //
    // pFrame is on the stack - good.  Now tell the LS to intercept at that frame.
    //

    GetProcess()->InitIPCEvent(&event,
                               DB_IPCE_INTERCEPT_EXCEPTION,
                               true,
                               LSPTR_APPDOMAIN::NullPtr());

    event.InterceptException.threadToken = m_debuggerThreadToken;
    event.InterceptException.frameToken  = pRealFrame->GetFramePointer();

    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event, sizeof(DebuggerIPCEvent));

    //
    // Stop now if we can't even send the event.
    //
    if (!SUCCEEDED(hr))
    {
        goto LExit;
    }

    _ASSERTE(event.type == DB_IPCE_INTERCEPT_EXCEPTION_RESULT);

    hr = event.hr;

LExit:

    return hr;
}


/*
 *
 * SetRemapIP
 *
 *  This routine communicate the remap IP to the LS by writing it to process memory using
 *  the pointer that was set in the thread. If the address is null, then we haven't seen
 *  a RemapBreakpoint call for this frame/function combo yet, so invalid to Remap the function.
 *
 * Parameters:
 *  offset - the IL offset to set the IP to
 *
 * Return Value:
 *  S_OK or CORDBG_E_NO_REMAP_BREAKPIONT.
 *
 */
HRESULT CordbThread::SetRemapIP(SIZE_T offset)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    if (! m_EnCRemapFunctionIP)
    {
        return CORDBG_E_NO_REMAP_BREAKPIONT;
    }

    BOOL succ = WriteProcessMemory(
                GetProcess()->m_handle,
                m_EnCRemapFunctionIP,
                &offset,
                sizeof(SIZE_T),
                NULL);

    // if we don't get a RemapFunction, this member will be cleared in MarkStackFramesDirty
    // when Continue is called
    m_EnCRemapFunctionIP = NULL;


    if (! succ)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
}


/*
 *
 * GetConnectionId
 *
 *  This routine is the interface function for ICorDebugThread2::GetConnectionId.
 *
 * Parameters:
 *  pdwConnectionId - return connection id set on the thread. Can return INVALID_CONNECTION_ID
 *
 * Return Value:
 *
 */
HRESULT CordbThread::GetConnectionID(CONNID *pdwConnectionId)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;
    DebuggerIPCEvent event;

    // two way event
    GetProcess()->InitIPCEvent(&event,
                               DB_IPCE_GET_CONNECTIONID,
                               true,
                               LSPTR_APPDOMAIN::NullPtr());
    event.GetConnectionId.debuggerThreadToken = m_debuggerThreadToken;

    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event, sizeof(DebuggerIPCEvent));

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        goto exit;

    _ASSERTE(event.type == DB_IPCE_GET_CONNECTIONID_RESULT);

    hr = event.hr;

    // now retrieve the connection id
    *pdwConnectionId = event.GetConnectionIdResult.connectionId;

    if (SUCCEEDED(hr) && *pdwConnectionId == INVALID_CONNECTION_ID)
    {
        hr = S_FALSE;
    }
exit:
    return hr;
}   // CordbThread::GetConnectionId

/*
 *
 * GetTaskId
 *
 *  This routine is the interface function for ICorDebugThread2::GetTaskId.
 *
 * Parameters:
 *  pTaskId - return task id set on the thread. Can return INVALID_TASK_ID
 *
 * Return Value:
 *
 */
HRESULT CordbThread::GetTaskID(TASKID *pTaskId)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;
    DebuggerIPCEvent event;

    GetProcess()->InitIPCEvent(&event,
                               DB_IPCE_GET_TASKID,
                               true,
                               LSPTR_APPDOMAIN::NullPtr());
    event.GetTaskId.debuggerThreadToken = m_debuggerThreadToken;

    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event, sizeof(DebuggerIPCEvent));

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        goto exit;

    _ASSERTE(event.type == DB_IPCE_GET_TASKID_RESULT);

    hr = event.hr;

    // now retrieve the task id
    *pTaskId = event.GetTaskIdResult.taskId;
    if (SUCCEEDED(hr) && *pTaskId == INVALID_TASK_ID)
    {
        hr = S_FALSE;
    }
exit:
    return hr;
}   // CordbThread::GetTaskId


/*
 *
 * GetOSThreadId
 *
 *  This routine is the interface function for ICorDebugThread2::GetOSThreadId.
 *
 * Parameters:
 *  pdwTid - return os thread id
 *
 * Return Value:
 *
 */
HRESULT CordbThread::GetVolatileOSThreadID(DWORD *pdwTid)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;
    DebuggerIPCEvent event;


    GetProcess()->InitIPCEvent(&event,
                               DB_IPCE_GET_OSTHREADID,
                               true,
                               LSPTR_APPDOMAIN::NullPtr());
    event.GetOSThreadId.debuggerThreadToken = m_debuggerThreadToken;

    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event, sizeof(DebuggerIPCEvent));

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        goto exit;

    _ASSERTE(event.type == DB_IPCE_GET_OSTHREADID_RESULT);

    hr = event.hr;

    // now retrieve the task id
    *pdwTid = event.GetOSThreadIdResult.osThreadId;

    if (SUCCEEDED(hr) && event.GetOSThreadIdResult.osThreadId == 0xbaadf00d)
        hr = S_FALSE;

exit:
    return hr;
}   //  CordbThread::GetOSThreadId


/* ------------------------------------------------------------------------- *
 * Unmanaged Thread classes
 * ------------------------------------------------------------------------- */

CordbUnmanagedThread::CordbUnmanagedThread(CordbProcess *pProcess, DWORD dwThreadId, HANDLE hThread, void *lpThreadLocalBase)
  : CordbBase(pProcess, dwThreadId, enumCordbUnmanagedThread),
    m_handle(hThread),
    m_threadLocalBase(lpThreadLocalBase),
    m_pTLSArray(NULL),
    m_state(CUTS_None),
    m_originalHandler(NULL),
    m_continueCountCached(0)
{
    m_pLeftSideContext.Set(NULL);

    IBEvent()->m_state = CUES_None;
    IBEvent()->m_next = NULL;
    IBEvent()->m_owner = this;

    IBEvent2()->m_state = CUES_None;
    IBEvent2()->m_next = NULL;
    IBEvent2()->m_owner = this;

    OOBEvent()->m_state = CUES_None;
    OOBEvent()->m_next = NULL;
    OOBEvent()->m_owner = this;

    m_pPatchSkipAddress = NULL;
}

CordbUnmanagedThread::~CordbUnmanagedThread()
{
    // CordbUnmanagedThread objects will:
    // - never send IPC events.
    // - never be exposed to the public. (we assert external-ref is always == 0)
    // - always manipulated on W32ET (where we can't do IPC stuff)

    UnsafeNeuterDeadObject();

    _ASSERTE(this->IsNeutered());

}


//-----------------------------------------------------------------------------
// Inject an int3 at the given remote address
//-----------------------------------------------------------------------------

// This flavor is assuming our caller already knows the opcode.
HRESULT ApplyRemotePatch(HANDLE hPid, const void * pRemoteAddress)
{
#if defined(_X86_)
    // Put the patch back in.
    SIZE_T cbWritten;
    const BYTE patch = CORDbg_BREAK_INSTRUCTION;
    BOOL succ;
    succ = WriteProcessMemory(
        hPid,
        (void*) pRemoteAddress, (void*) &patch, sizeof(patch), &cbWritten);

    SIMPLIFYING_ASSUMPTION(succ && (cbWritten == sizeof(patch)));
#endif
    return S_OK;
}


// Get the opcode that we're replacing.
HRESULT ApplyRemotePatch(HANDLE hPid, const void * pRemoteAddress, PRD_TYPE * pOpcode)
{
#if defined(_X86_)
    // Read out opcode. 1 byte on x86
    BYTE opcode;
    SIZE_T cbRead;
    BOOL succ = ReadProcessMemoryI(hPid, (void*) pRemoteAddress, (void*) &opcode, 1, &cbRead);

    if (!succ || (cbRead != 1))
    {
        return HRESULT_FROM_GetLastError();
    }
    *pOpcode = (PRD_TYPE) opcode;
#endif
    ApplyRemotePatch(hPid, pRemoteAddress);
    return S_OK;
}

//-----------------------------------------------------------------------------
// Remove the int3 from the remote address
//-----------------------------------------------------------------------------
HRESULT RemoveRemotePatch(HANDLE hPid, const void * pRemoteAddress, PRD_TYPE opcode)
{
#if defined(_X86_)
    // Replace the BP w/ the opcode.
    SIZE_T cbWritten;
    BYTE opcode2 = (BYTE) opcode;
    BOOL succ;
    succ = WriteProcessMemory(
        hPid,
        (void*) pRemoteAddress,
        &opcode2, sizeof(opcode2),
        &cbWritten);

    //SIMPLIFYING_ASSUMPTION(succ && (cbWritten == sizeof(opcode2)));
#endif
    return S_OK;
}

/* ------------------------------------------------------------------------- *
 * Chain class
 * ------------------------------------------------------------------------- */

CordbChain::CordbChain(CordbThread* thread, bool managed,
                       CordbFrame **start, CordbFrame **end, UINT iChainInThread)
  : CordbBase(thread->GetProcess(), 0, enumCordbChain),
    m_thread(thread),
    m_iThisChain(iChainInThread),
    m_caller(NULL),m_callee(NULL),
    m_managed(managed),
    m_start(start), m_end(end)
{

}

/*
    A list of which resources owned by this object are accounted for.

    UNRESOLVED:
        CordbChain              *m_caller, *m_callee;
        CordbFrame             **m_start, **m_end;

    RESOLVED:
        CordbThread             *m_thread;              // Neutered
*/

CordbChain::~CordbChain()
{
    _ASSERTE(IsNeutered());
}

// Neutered by CordbThread::CleanupStack
void CordbChain::Neuter(NeuterTicket ticket)
{
    if (IsNeutered())
    {
        return;
    }

    RSSmartPtr<CordbChain> pRef(this);
    {
        CordbBase::Neuter(ticket);
    }
}


HRESULT CordbChain::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugChain)
        *pInterface = static_cast<ICorDebugChain*>(this);
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugChain*>(this));
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbChain::GetThread(ICorDebugThread **ppThread)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    VALIDATE_POINTER_TO_OBJECT(ppThread, ICorDebugThread **);

    *ppThread = static_cast<ICorDebugThread*> (m_thread);
    m_thread->ExternalAddRef();

    return S_OK;
}

inline CORDB_ADDRESS GetSPFromDebuggerREGDISPLAY(DebuggerREGDISPLAY* pDRD)
{
    return pDRD->SP;

}

// Get the range on the stack that this chain matches against.
// pStart is the leafmost; pEnd is the rootmost.
// This is particularly used in interop-debugging to get native stack traces
// for the UM portions of the stack
HRESULT CordbChain::GetStackRange(CORDB_ADDRESS *pStart, CORDB_ADDRESS *pEnd)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    return E_NOTIMPL;
}

HRESULT CordbChain::GetContext(ICorDebugContext **ppContext)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppContext, ICorDebugContext **);
    /* !!! */

    return E_NOTIMPL;
}

HRESULT CordbChain::GetCaller(ICorDebugChain **ppChain)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppChain, ICorDebugChain **);

    // For now, just return the next chain

    HRESULT hr = GetNext(ppChain);

    return hr;
}

HRESULT CordbChain::GetCallee(ICorDebugChain **ppChain)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppChain, ICorDebugChain **);

    // For now, just return the previous chain

    HRESULT hr = GetPrevious(ppChain);

    return hr;
}

HRESULT CordbChain::GetPrevious(ICorDebugChain **ppChain)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppChain, ICorDebugChain **);

    *ppChain = NULL;

    if (m_iThisChain != 0)
        *ppChain = m_thread->m_stackChains[m_iThisChain-1];

    if (*ppChain != NULL )
        (*ppChain)->AddRef();

    return S_OK;
}

HRESULT CordbChain::GetNext(ICorDebugChain **ppChain)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppChain, ICorDebugChain **);

    *ppChain = NULL;

    if (m_iThisChain+1 != m_thread->m_stackChains.Length())
        *ppChain = m_thread->m_stackChains[m_iThisChain+1];

    if (*ppChain != NULL )
        (*ppChain)->AddRef();

    return S_OK;
}

HRESULT CordbChain::IsManaged(BOOL *pManaged)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pManaged, BOOL *);

    *pManaged = m_managed;

    return S_OK;
}

HRESULT CordbChain::EnumerateFrames(ICorDebugFrameEnum **ppFrames)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppFrames, ICorDebugFrameEnum **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());


    CordbFrameEnum* e = new (nothrow) CordbFrameEnum(this);

    if (e != NULL)
    {
        *ppFrames = (ICorDebugFrameEnum*)e;
        e->AddRef();
    }
    else
        return E_OUTOFMEMORY;

    return S_OK;
}

HRESULT CordbChain::GetActiveFrame(ICorDebugFrame **ppFrame)
{
    PUBLIC_API_ENTRY(this);
    {
        FAIL_IF_NEUTERED(this);
    }
    VALIDATE_POINTER_TO_OBJECT(ppFrame, ICorDebugFrame **);
    (*ppFrame) = NULL;

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;

    //
    // Refresh the stack frames for this thread.
    //
    hr = m_thread->RefreshStack();

    if (FAILED(hr))
        goto LExit;

    // if the RefreshStack caused us to update the stack we will be neutered now
    // and shouldn't be calling this fcn when stack out of date.
    {
        FAIL_IF_NEUTERED(this);
    }

    if (m_end <= m_start
        || m_start == NULL
        || m_start == m_end
        || *m_start == NULL
        )
        *ppFrame = NULL;
    else
    {
        (*ppFrame) = (ICorDebugFrame*)*m_start;
        (*ppFrame)->AddRef();
    }


LExit:
    return hr;
}

HRESULT CordbChain::GetRegisterSet(ICorDebugRegisterSet **ppRegisters)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(ppRegisters, ICorDebugRegisterSet **);

    CordbThread *thread = m_thread;

    CordbRegisterSet *pRegisterSet
      = new (nothrow) CordbRegisterSet( &m_rd, thread,
                              m_iThisChain == 0,
                              m_quicklyUnwound);

    if( pRegisterSet == NULL )
        return E_OUTOFMEMORY;

    pRegisterSet->ExternalAddRef();

    (*ppRegisters) = static_cast<ICorDebugRegisterSet *> (pRegisterSet);
    return S_OK;
}

HRESULT CordbChain::GetReason(CorDebugChainReason *pReason)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pReason, CorDebugChainReason *);

    *pReason = m_reason;

    return S_OK;
}

/* ------------------------------------------------------------------------- *
 * Chain Enumerator class
 * ------------------------------------------------------------------------- */

CordbChainEnum::CordbChainEnum(CordbThread *thread)
  : CordbBase(thread->GetProcess(), 0, enumCordbChainEnum),
    m_thread(thread),
    m_currentChain(0)
{
    _ASSERTE(thread != NULL);

    // Add to neuter-list.
    thread->GetRefreshStackNeuterList()->Add(this);

}

CordbChainEnum::~CordbChainEnum()
{
    // Should be on Thread's "RefreshStack" neuter list.
    _ASSERTE(this->IsNeutered());
}

void CordbChainEnum::Neuter(NeuterTicket ticket)
{
    m_thread = NULL;

    CordbBase::Neuter(ticket);
}


HRESULT CordbChainEnum::Reset(void)
{
    PUBLIC_API_ENTRY(this);

    m_currentChain = 0;

    return S_OK;
}

HRESULT CordbChainEnum::Clone(ICorDebugEnum **ppEnum)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(ppEnum, ICorDebugEnum **);

    HRESULT hr = S_OK;

    CordbChainEnum *e = new (nothrow) CordbChainEnum(m_thread);

    if (e == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto LExit;
    }

    e->m_currentChain = m_currentChain;
    e->ExternalAddRef();

    *ppEnum = (ICorDebugEnum*)e;

LExit:
    return hr;
}

HRESULT CordbChainEnum::GetCount(ULONG *pcelt)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT(pcelt, ULONG *);

    *pcelt = m_thread->m_stackChains.Length();
    return S_OK;
}

HRESULT CordbChainEnum::Next(ULONG celt, ICorDebugChain *chains[],
                             ULONG *pceltFetched)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT_ARRAY(chains, ICorDebugChain *,
        celt, true, true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pceltFetched, ULONG *);

    if ((pceltFetched == NULL) && (celt != 1))
    {
        return E_INVALIDARG;
    }

    if (celt == 0)
    {
        if (pceltFetched != NULL)
        {
            *pceltFetched = 0;
        }
        return S_OK;
    }

    ICorDebugChain **c = chains;

    while ((m_currentChain < m_thread->m_stackChains.Length()) &&
           (celt > 0))
    {
        *c = (ICorDebugChain*) m_thread->m_stackChains[m_currentChain];
        (*c)->AddRef();
        c++;
        m_currentChain++;
        celt--;
    }

    if (pceltFetched != NULL)
    {
        *pceltFetched = (ULONG)(c - chains);
    }

    //
    // If we reached the end of the enumeration, but not the end
    // of the number of requested items, we return S_FALSE.
    //
    if (celt > 0)
    {
        return S_FALSE;
    }

    return S_OK;
}

HRESULT CordbChainEnum::Skip(ULONG celt)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    m_currentChain += celt;
    return S_OK;
}

HRESULT CordbChainEnum::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugChainEnum)
        *pInterface = static_cast<ICorDebugChainEnum*>(this);
    else if (id == IID_ICorDebugEnum)
        *pInterface = static_cast<ICorDebugEnum*>(static_cast<ICorDebugChainEnum*>(this));
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugChainEnum*>(this));
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

HRESULT CordbContext::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugContext)
        *pInterface = static_cast<ICorDebugContext*>(this);
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugContext*>(this));
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


/* ------------------------------------------------------------------------- *
 * Frame class
 * ------------------------------------------------------------------------- */

CordbFrame::CordbFrame(CordbChain *chain, FramePointer fp,
                       SIZE_T ip, UINT iFrameInChain,
                       CordbAppDomain *currentAppDomain)
  : CordbBase(chain->GetProcess(), 0, enumCordbFrame),
    m_ip(ip),
    m_thread(chain->m_thread),
    m_chain(chain),
    m_iThisFrame(iFrameInChain),
    m_currentAppDomain(currentAppDomain),
    m_fp(fp)
{

#ifdef _DEBUG
    // For debugging purposes, track what Continue session these frames were created in.
    m_DbgContinueCounter = GetProcess()->m_continueCounter;
#endif
}

/*
    A list of which resources owned by this object are accounted for.

    UNKNOWN:
        CordbThread            *m_thread;
        CordbChain             *m_chain;
        CordbAppDomain         *m_currentAppDomain;

*/

CordbFrame::~CordbFrame()
{
    _ASSERTE(IsNeutered());
}

// Neutered by DerivedClasses
void CordbFrame::Neuter(NeuterTicket ticket)
{
    if (IsNeutered())
    {
        return;
    }

    AddRef();
    {
        CordbBase::Neuter(ticket);
    }
    Release();
}


HRESULT CordbFrame::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugFrame)
        *pInterface = static_cast<ICorDebugFrame*>(this);
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugFrame*>(this));
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbFrame::GetChain(ICorDebugChain **ppChain)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppChain, ICorDebugChain **);

    *ppChain = (ICorDebugChain*)m_chain;
    m_chain->ExternalAddRef();

    return S_OK;
}

HRESULT CordbFrame::GetStackRange(CORDB_ADDRESS *pStart, CORDB_ADDRESS *pEnd)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pStart, CORDB_ADDRESS *);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pEnd, CORDB_ADDRESS *);
    FAIL_IF_NEUTERED(this);

    return E_NOTIMPL;
}

HRESULT CordbFrame::GetFunction(ICorDebugFunction **ppFunction)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppFunction, ICorDebugFunction **);
    FAIL_IF_NEUTERED(this);

    CordbFunction *pFunc = this->GetFunction();

    if (pFunc == NULL)
    {
        return CORDBG_E_CODE_NOT_AVAILABLE;
    }
    *ppFunction = static_cast<ICorDebugFunction*> (pFunc);
    pFunc->ExternalAddRef();

    return S_OK;
}

HRESULT CordbFrame::GetFunctionToken(mdMethodDef *pToken)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(pToken, mdMethodDef *);
    FAIL_IF_NEUTERED(this);

    CordbFunction *pFunc = GetFunction();
    if (pFunc == NULL)
    {
        return CORDBG_E_CODE_NOT_AVAILABLE;
    }

    *pToken = pFunc->m_token;

    return S_OK;
}

HRESULT CordbFrame::GetCaller(ICorDebugFrame **ppFrame)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppFrame, ICorDebugFrame **);
    FAIL_IF_NEUTERED(this);

    *ppFrame = NULL;

    CordbFrame **nextFrame = m_chain->m_start + m_iThisFrame + 1;
    if (nextFrame < m_chain->m_end)
        *ppFrame = *nextFrame;

    if (*ppFrame != NULL )
        (*ppFrame)->AddRef();

    return S_OK;
}

HRESULT CordbFrame::GetCallee(ICorDebugFrame **ppFrame)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppFrame, ICorDebugFrame **);
    FAIL_IF_NEUTERED(this);

    *ppFrame = NULL;

    if (m_iThisFrame == 0)
        *ppFrame = NULL;
    else
        *ppFrame = m_chain->m_start[m_iThisFrame - 1];

    if (*ppFrame != NULL )
        (*ppFrame)->AddRef();

    return S_OK;
}

HRESULT CordbFrame::CreateStepper(ICorDebugStepper **ppStepper)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    VALIDATE_POINTER_TO_OBJECT(ppStepper, ICorDebugStepper **);

    CordbStepper *stepper = new (nothrow) CordbStepper(m_chain->m_thread, this);

    if (stepper == NULL)
        return E_OUTOFMEMORY;

    stepper->ExternalAddRef();
    *ppStepper = stepper;

    return S_OK;
}


/* ------------------------------------------------------------------------- *
 * Frame Enumerator class
 * ------------------------------------------------------------------------- */

CordbFrameEnum::CordbFrameEnum(CordbChain *chain)
  : CordbBase(chain->GetProcess(), 0, enumCordbFrameEnum),
    m_chain(chain),
    m_currentFrame(NULL)
{
    // We don't take a strong reference to the Chain object b/c we'll add ourselves
    // to the neuter-list instead. Thus when the chain becomes invalid, we'll
    // get neutered.
    _ASSERTE(m_chain != NULL);

    // Add to neuter-list.
    m_chain->m_thread->GetRefreshStackNeuterList()->Add(this);

    m_currentFrame = m_chain->m_start;
}

CordbFrameEnum::~CordbFrameEnum()
{
    // Owning thread should have neutered us.
    _ASSERTE(this->IsNeutered());

    _ASSERTE (NULL == m_chain); // already neutered
}

void CordbFrameEnum::Neuter(NeuterTicket ticket)
{
    // Never took a ref to the chain / frame, so don't release it.
    m_chain = NULL;
    m_currentFrame = NULL;

    CordbBase::Neuter(ticket);
}


HRESULT CordbFrameEnum::Reset(void)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    _ASSERTE(m_chain != NULL);
    m_currentFrame = m_chain->m_start;

    return S_OK;
}

HRESULT CordbFrameEnum::Clone(ICorDebugEnum **ppEnum)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(ppEnum, ICorDebugEnum **);

    HRESULT hr = S_OK;

    CordbFrameEnum *e = new (nothrow) CordbFrameEnum(m_chain);

    if (e == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto LExit;
    }

    e->m_currentFrame = m_currentFrame;
    e->ExternalAddRef();

    *ppEnum = (ICorDebugEnum*)e;

LExit:
    return hr;
}

HRESULT CordbFrameEnum::GetCount(ULONG *pcelt)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(pcelt, ULONG *);

    *pcelt = (ULONG)(m_chain->m_end - m_chain->m_start);

    return S_OK;
}

HRESULT CordbFrameEnum::Next(ULONG celt, ICorDebugFrame *frames[],
                             ULONG *pceltFetched)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT_ARRAY(frames, ICorDebugFrame *,
        celt, true, true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pceltFetched, ULONG *);

    if ((pceltFetched == NULL) && (celt != 1))
    {
        return E_INVALIDARG;
    }

    if (celt == 0)
    {
        if (pceltFetched != NULL)
        {
            *pceltFetched = 0;
        }
        return S_OK;
    }

    ICorDebugFrame **f = frames;

    while ((m_currentFrame < m_chain->m_end) && (celt > 0))
    {
        *f = (ICorDebugFrame*) *m_currentFrame;
        (*f)->AddRef();
        f++;
        m_currentFrame++;
        celt--;
    }

    if (pceltFetched != NULL)
    {
        *pceltFetched = (ULONG)(f - frames);
    }

    //
    // If we reached the end of the enumeration, but not the end
    // of the number of requested items, we return S_FALSE.
    //
    if (celt > 0)
    {
        return S_FALSE;
    }

    return S_OK;
}

HRESULT CordbFrameEnum::Skip(ULONG celt)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    while ((m_currentFrame < m_chain->m_end) && (celt-- > 0))
        m_currentFrame++;

    return S_OK;
}

HRESULT CordbFrameEnum::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugFrameEnum)
        *pInterface = static_cast<ICorDebugFrameEnum*>(this);
    else if (id == IID_ICorDebugEnum)
        *pInterface = static_cast<ICorDebugEnum*>(static_cast<ICorDebugFrameEnum*>(this));
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugFrameEnum*>(this));
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

/* ------------------------------------------------------------------------- *

 * Value Enumerator class
 *
 * Used by CordbJITILFrame for EnumLocalVars & EnumArgs.
 * NOTE NOTE NOTE WE ASSUME that the 'frame' argument is actually the
 * CordbJITILFrame's native frame member variable.
 * ------------------------------------------------------------------------- */

CordbValueEnum::CordbValueEnum(CordbNativeFrame *frame, ValueEnumMode mode) :
    CordbBase(frame->GetProcess(), 0)
{
    _ASSERTE( frame != NULL );
    _ASSERTE( mode == LOCAL_VARS || mode == ARGS);

    m_frame = frame;
    m_mode = mode;
    m_iCurrent = 0;
    m_iMax = 0;
}

/*
 * CordbValueEnum::Init
 *
 * Initialize a CordbValueEnum object. Must be called after allocating the object and before using it. If Init
 * fails, then destroy the object and release the memory.
 *
 * Parameters:
 *     none.
 *
 * Returns:
 *    HRESULT for success or failure.
 *
 */
HRESULT CordbValueEnum::Init()
{
    HRESULT hr = S_OK;

    switch (m_mode)
    {
    case ARGS:
        {
            // Get the function signature
            CordbFunction *func = m_frame->GetFunction();
            ULONG methodArgCount;

            IfFailRet(func->GetSig(NULL, &methodArgCount, NULL));

            // Grab the argument count for the size of the enumeration.
            m_iMax = methodArgCount;

            CordbNativeFrame *nil = m_frame;
            CordbJITILFrame *jil = nil->m_JITILFrame;

            if (jil->m_fVarArgFnx && jil->m_sigBuf != NULL)
            {
                m_iMax = jil->m_argCount;
            }
            break;
        }
    case LOCAL_VARS:
        {
            // Get the locals signature.
            CordbFunction *func = m_frame->GetFunction();
            ULONG localsCount;

            IfFailRet(func->GetLocalVarSig(NULL, &localsCount));

            // Grab the number of locals for the size of the enumeration.
            m_iMax = localsCount;
            break;
        }
    }

    // Everything worked okay, so add this object to the neuter list for objects that are tied to the stack trace.
    m_frame->m_thread->GetRefreshStackNeuterList()->Add(this);

    return hr;
}

CordbValueEnum::~CordbValueEnum()
{
    _ASSERTE(this->IsNeutered());
    _ASSERTE(m_frame == NULL);
}

void CordbValueEnum::Neuter(NeuterTicket ticket)
{
    m_frame = NULL;
    CordbBase::Neuter(ticket);
}



HRESULT CordbValueEnum::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugEnum)
        *pInterface = static_cast<ICorDebugEnum*>(this);
    else if (id == IID_ICorDebugValueEnum)
        *pInterface = static_cast<ICorDebugValueEnum*>(this);
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugValueEnum*>(this));
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbValueEnum::Skip(ULONG celt)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = E_FAIL;
    if ( (m_iCurrent+celt) < m_iMax ||
         celt == 0)
    {
        m_iCurrent += celt;
        hr = S_OK;
    }

    return hr;
}

HRESULT CordbValueEnum::Reset(void)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    m_iCurrent = 0;
    return S_OK;
}

HRESULT CordbValueEnum::Clone(ICorDebugEnum **ppEnum)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(ppEnum, ICorDebugEnum **);

    HRESULT hr = S_OK;

    CordbValueEnum *pCVE = new (nothrow) CordbValueEnum(m_frame, m_mode);

    if (pCVE == NULL)
    {
        (*ppEnum) = NULL;
        hr = E_OUTOFMEMORY;
        goto LExit;
    }

    // Initialize the new enum
    hr = pCVE->Init();

    if (SUCCEEDED(hr))
    {
        pCVE->ExternalAddRef();
        (*ppEnum) = static_cast<ICorDebugEnum*> (pCVE);
    }
    else
    {
        // Neuter the new object we've been working on.
        NeuterTicket ticket(GetProcess());
        pCVE->Neuter(ticket);

        (*ppEnum) = NULL;
        delete pCVE;
    }

LExit:
    return hr;
}

HRESULT CordbValueEnum::GetCount(ULONG *pcelt)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(pcelt, ULONG *);

    if( pcelt == NULL)
    {
        return E_INVALIDARG;
    }

    (*pcelt) = m_iMax;
    return S_OK;
}

//
// In the event of failure, the current pointer will be left at
// one element past the troublesome element.  Thus, if one were
// to repeatedly ask for one element to iterate through the
// array, you would iterate exactly m_iMax times, regardless
// of individual failures.
HRESULT CordbValueEnum::Next(ULONG celt, ICorDebugValue *values[], ULONG *pceltFetched)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT_ARRAY(values, ICorDebugValue *,
        celt, true, true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pceltFetched, ULONG *);

    if ((pceltFetched == NULL) && (celt != 1))
    {
        return E_INVALIDARG;
    }

    if (celt == 0)
    {
        if (pceltFetched != NULL)
        {
            *pceltFetched = 0;
        }
        return S_OK;
    }

    HRESULT hr = S_OK;

    int iMax = min( m_iMax, m_iCurrent+celt);
    int i;
    for (i = m_iCurrent; i< iMax;i++)
    {
        switch ( m_mode )
        {
        case ARGS:
            {
                hr = m_frame->m_JITILFrame->GetArgument( i, &(values[i-m_iCurrent]) );
                break;
            }
        case LOCAL_VARS:
            {
                hr = m_frame->m_JITILFrame->GetLocalVariable( i, &(values[i-m_iCurrent]) );
                break;
            }
        }
        if ( FAILED( hr ) )
        {
            break;
        }
    }

    int count = (i - m_iCurrent);

    if ( FAILED( hr ) )
    {
        //
        // we failed: +1 pushes us past troublesome element
        //
        m_iCurrent += 1 + count;
    }
    else
    {
        m_iCurrent += count;
    }

    if (pceltFetched != NULL)
    {
        *pceltFetched = count;
    }

    if (FAILED(hr))
    {
        return hr;
    }


    //
    // If we reached the end of the enumeration, but not the end
    // of the number of requested items, we return S_FALSE.
    //
    if (((ULONG)count) < celt)
    {
        return S_FALSE;
    }

    return hr;
}


//-----------------------------------------------------------------------------
// CordbInternalFrame
//-----------------------------------------------------------------------------
CordbInternalFrame::CordbInternalFrame(
    CordbChain *chain,
    FramePointer fp,
    DebuggerREGDISPLAY *rd,
    UINT iFrameInChain,
    CordbAppDomain *pAppDomain,
    DebuggerIPCE_STRData * pData
) :
    CordbFrame(chain, fp, 0, iFrameInChain, pAppDomain)
{
    m_rd = *rd;
    m_eFrameType = pData->stubFrame.frameType;
    m_funcMetadataToken = pData->stubFrame.funcMetadataToken;

    CordbFunction * pFunc = NULL;

    // Some internal frames may not have a Function associated w/ them.
    if (!IsNilToken(m_funcMetadataToken))
    {
        // Find the module of the function.  Note that this module isn't necessarily in the same domain as our frame.
        // FuncEval frames can point to methods they are going to invoke in another domain.
        CordbModule * pModule = NULL;
        pModule = pAppDomain->GetProcess()->LookupModule(pData->stubFrame.funcDebuggerModuleToken);
        _ASSERTE(pModule != NULL);

        if( pModule != NULL )
        {
            _ASSERTE( (pModule->GetAppDomain() == pAppDomain) || (m_eFrameType == STUBFRAME_FUNC_EVAL) );

            ICorDebugFunction * pIFunction = NULL;

            mdMethodDef token = pData->stubFrame.funcMetadataToken;
            HRESULT hr;
            hr = pModule->GetFunctionFromToken(token, &pIFunction);
            _ASSERTE(SUCCEEDED(hr));

            pFunc = static_cast<CordbFunction*> (pIFunction);
        }
    }
    m_function.Assign(pFunc);
}


HRESULT CordbInternalFrame::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugFrame)
    {
        *pInterface = static_cast<ICorDebugFrame*>(static_cast<ICorDebugInternalFrame*>(this));
    }
    else if (id == IID_ICorDebugInternalFrame)
    {
        *pInterface = static_cast<ICorDebugInternalFrame*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugInternalFrame*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

void CordbInternalFrame::Neuter(NeuterTicket ticket)
{
    m_function.Assign(NULL);
    CordbBase::Neuter(ticket);
}


HRESULT CordbInternalFrame::GetStackRange(CORDB_ADDRESS *pStart,
                                        CORDB_ADDRESS *pEnd)
{
    PUBLIC_REENTRANT_API_ENTRY(this);

    // Callers explicit require GetStackRange() to be callable when neutered so that they
    // can line up ICorDebugFrame objects across continues. We only return stack ranges
    // here and don't access any special data. 
    OK_IF_NEUTERED(this);

    // Use our FramePointer for the stack range.
    CORDB_ADDRESS pFrame = PTR_TO_CORDB_ADDRESS(GetFramePointer().GetSPValue());

    _ASSERTE(m_id == 0);

    if (pStart)
    {
        // For dynamic methods, we want to treat them like normal managed methods
        // and report the actual stack range.  For other types of internal frames,
        // since they map to physical frames, we can only trust their frame pointers.
        if (m_eFrameType == STUBFRAME_LIGHTWEIGHT_FUNCTION)
        {
            *pStart = GetSPFromDebuggerREGDISPLAY(&m_rd);
        }
        else
        {
        *pStart = pFrame;
    }
    }

    if (pEnd)
    {
        *pEnd = pFrame;
    }

    return S_OK;
}


// This may return NULL if there's no Method associated w/ this Frame.
// For FuncEval frames, the function returned might also be in a different AppDomain 
// than the frame itself.
CordbFunction * CordbInternalFrame::GetFunction()
{
    return m_function;
}


/* ------------------------------------------------------------------------- *
 * Native Frame class
 * ------------------------------------------------------------------------- */


CordbNativeFrame::CordbNativeFrame(CordbChain *chain, FramePointer fp,
                                   CordbJITInfo *jitinfo,
                                   SIZE_T ip, DebuggerREGDISPLAY* rd,
                                   void * ambientESP,
                                   bool quicklyUnwound,
                                   UINT iFrameInChain,
                                   CordbAppDomain *currentAppDomain,
                                   CordbMiscFrame *pMisc)
  : CordbFrame(chain, fp, ip, iFrameInChain, currentAppDomain),
    m_rd(*rd), m_quicklyUnwound(quicklyUnwound), m_JITILFrame(NULL),
    m_jitinfo(jitinfo), // implicit InternalAddRef
    m_ambientESP(ambientESP)
{

    _ASSERTE(pMisc == NULL);

    if (pMisc != NULL)
        m_misc = *pMisc;
}

/*
    A list of which resources owned by this object are accounted for.

    RESOLVED:
        CordbJITILFrame*   m_JITILFrame; // Neutered
*/

CordbNativeFrame::~CordbNativeFrame()
{
    _ASSERTE(IsNeutered());
}

// Neutered by CordbThread::CleanupStack
void CordbNativeFrame::Neuter(NeuterTicket ticket)
{
    // Neuter may be called multiple times so be sure to set ptrs to NULL so that we don't
    // double release them.
    if (IsNeutered())
    {
        return;
    }

    AddRef();
    {

        m_jitinfo.Clear();

        if (m_JITILFrame != NULL)
        {
            m_JITILFrame->Neuter(ticket);
            m_JITILFrame.Clear();
        }

        CordbFrame::Neuter(ticket);
    }
    Release();
}

// CordbNativeFrame::QueryInterface
//
// Description
//  interface query for this COM object
//
//  NOTE: the COM object associated with this CordbNativeFrame may consist of
//  two C++ objects (the CordbNativeFrame and the CordbJITILFrame).
//
// Parameters
//      id              the GUID associated with the requested interface
//      pInterface      [out] the interface pointer
//
// Returns
//  HRESULT
//      S_OK            If this CordbJITILFrame supports the interface
//      E_NOINTERFACE   If this object does not support the interface
//
// Exceptions
//  None
//
//
HRESULT CordbNativeFrame::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugFrame)
    {
        *pInterface = static_cast<ICorDebugFrame*>(static_cast<ICorDebugNativeFrame*>(this));
    }
    else if (id == IID_ICorDebugNativeFrame)
    {
        *pInterface = static_cast<ICorDebugNativeFrame*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugNativeFrame*>(this));
    }
    else
    {
        // might be searching for an IL Frame. delegate that search to the
        // JITILFrame
        if (m_JITILFrame != NULL)
        {
            return m_JITILFrame->QueryInterfaceInternal(id, pInterface);
        }
        else
        {
            *pInterface = NULL;
            return E_NOINTERFACE;
        }
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbNativeFrame::GetCode(ICorDebugCode **ppCode)
{
    PUBLIC_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppCode, ICorDebugCode **);
    FAIL_IF_NEUTERED(this);

    CordbCode * pCode = GetCode();
    *ppCode = static_cast<ICorDebugCode*> (pCode);
    pCode->ExternalAddRef();

    return S_OK;;
}

// Internal helper.
CordbCode * CordbNativeFrame::GetCode()
{
    return this->m_jitinfo->m_nativecode;
}

CordbFunction *CordbNativeFrame::GetFunction()
{
    return this->m_jitinfo->m_function;
}



HRESULT CordbNativeFrame::GetIP(ULONG32 *pnOffset)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pnOffset, ULONG32 *);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    *pnOffset = (ULONG32)m_ip;

    return S_OK;
}

HRESULT CordbNativeFrame::CanSetIP(ULONG32 nOffset)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    _ASSERTE(!m_chain->m_thread->m_stackFrames.IsEmpty() &&
             !m_chain->m_thread->m_stackChains.IsEmpty());

    if (m_chain->m_thread->m_stackFrames[0] != this ||
        m_chain->m_thread->m_stackChains[0] != m_chain)
    {
        return CORDBG_E_SET_IP_NOT_ALLOWED_ON_NONLEAF_FRAME;
    }

    HRESULT hr = m_chain->m_thread->SetIP(
                    SetIP_fCanSetIPOnly,
                    m_jitinfo->m_function->m_module->m_debuggerModuleToken,
                    m_jitinfo->m_function->m_token,
                    m_jitinfo->m_nativecode->m_nativeCodeJITInfoToken,
                    nOffset,
                    SetIP_fNative );

    return hr;
}

HRESULT CordbNativeFrame::SetIP(ULONG32 nOffset)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    _ASSERTE(!m_chain->m_thread->m_stackFrames.IsEmpty() &&
             !m_chain->m_thread->m_stackChains.IsEmpty());

    if (m_chain->m_thread->m_stackFrames[0] != this ||
        m_chain->m_thread->m_stackChains[0] != m_chain)
    {
        return CORDBG_E_SET_IP_NOT_ALLOWED_ON_NONLEAF_FRAME;
    }

    HRESULT hr = m_chain->m_thread->SetIP(
                    SetIP_fSetIP,
                    m_jitinfo->m_function->m_module->m_debuggerModuleToken,
                    m_jitinfo->m_function->m_token,
                    m_jitinfo->m_nativecode->m_nativeCodeJITInfoToken,
                    nOffset,
                    SetIP_fNative );

    return hr;
}


// Given a (register,offset) description of a stack location, compute
// the real memory address for it.
// This will also handle ambient SP values (which are encoded with regNum == REGNUM_AMBIENT_SP).
CORDB_ADDRESS CordbNativeFrame::GetLSStackAddress(
    ICorJitInfo::RegNum regNum,
    signed offset)
{
    UINT_PTR *pRegAddr;

    CORDB_ADDRESS pRemoteValue;

    if (regNum != ICorJitInfo::REGNUM_AMBIENT_SP)
    {
        {
            pRegAddr = this->GetAddressOfRegister(
                    ConvertRegNumToCorDebugRegister(regNum));
        }

        // This should never be null as long as regNum is a member of the RegNum enum.
        // If it is, an AV dereferencing a null-pointer in retail builds, or an assert in debug
        // builds is exactly the behavior we want.
        PREFIX_ASSUME(pRegAddr != NULL);

        pRemoteValue = PTR_TO_CORDB_ADDRESS(*pRegAddr + offset);
    }
    else
    {
        // Use the ambient ESP. At this point we're decoding an ambient-sp var, so
        // we should definitely have an ambient-sp. If this is null, then the jit
        // likely gave us an inconsistent data.
        UINT_PTR pAmbient = (UINT_PTR) this->GetAmbientESP();
        _ASSERTE(pAmbient != NULL);

        pRemoteValue = PTR_TO_CORDB_ADDRESS(pAmbient + offset);
    }

    return pRemoteValue;
}


HRESULT CordbNativeFrame::GetStackRange(CORDB_ADDRESS *pStart,
                                        CORDB_ADDRESS *pEnd)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    // Callers explicit require GetStackRange() to be callable when neutered so that they
    // can line up ICorDebugFrame objects across continues. We only return stack ranges
    // here and don't access any special data. 
    OK_IF_NEUTERED(this);

    if (pStart)
    {
        *pStart = GetSPFromDebuggerREGDISPLAY(&m_rd);
    }

    _ASSERTE(m_id == 0);

    if (pEnd)
    {
        *pEnd = PTR_TO_CORDB_ADDRESS(GetFramePointer().GetSPValue());
    }

    return S_OK;
}

HRESULT CordbNativeFrame::GetRegisterSet(ICorDebugRegisterSet **ppRegisters)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(ppRegisters, ICorDebugRegisterSet **);

    CordbThread *thread = m_chain->m_thread;

    CordbRegisterSet *pRegisterSet
      = new (nothrow) CordbRegisterSet( &m_rd, thread,
                              m_iThisFrame == 0
                              && m_chain->m_iThisChain == 0,
                              m_quicklyUnwound);

    if( pRegisterSet == NULL )
        return E_OUTOFMEMORY;

    pRegisterSet->ExternalAddRef();
    (*ppRegisters) = static_cast<ICorDebugRegisterSet *> (pRegisterSet);
    return S_OK;
}

//
// GetAddressOfRegister returns the address of the given register in the
// frames current register display (eg, a local address). This is usually used to build a
// ICorDebugValue from.
//
UINT_PTR* CordbNativeFrame::GetAddressOfRegister(CorDebugRegister regNum)
{
    UINT_PTR* ret = NULL;

    switch (regNum)
    {
    case REGISTER_STACK_POINTER:
        ret = (UINT_PTR*)GetSPAddress(&m_rd);
        break;

    case REGISTER_FRAME_POINTER:
        ret = (UINT_PTR*)GetFPAddress(&m_rd);
        break;

#if defined(_X86_)
    case REGISTER_X86_EAX:
        ret = (UINT_PTR*)&m_rd.Eax;
        break;

    case REGISTER_X86_ECX:
        ret = (UINT_PTR*)&m_rd.Ecx;
        break;

    case REGISTER_X86_EDX:
        ret = (UINT_PTR*)&m_rd.Edx;
        break;

    case REGISTER_X86_EBX:
        ret = (UINT_PTR*)&m_rd.Ebx;
        break;

    case REGISTER_X86_ESI:
        ret = (UINT_PTR*)&m_rd.Esi;
        break;

    case REGISTER_X86_EDI:
        ret = (UINT_PTR*)&m_rd.Edi;
        break;

#endif

    default:
        _ASSERT(!"Invalid register number!");
    }
    return ret;
}

//
// GetLeftSideAddressOfRegister returns the Left Side address of the given register in the frames current register
// display.
//
void *CordbNativeFrame::GetLeftSideAddressOfRegister(CorDebugRegister regNum)
{
    void* ret = 0;

    switch (regNum)
    {

    case REGISTER_FRAME_POINTER:
        ret = m_rd.pFP;
        break;

#if defined(_X86_)
    case REGISTER_X86_EAX:
        ret = m_rd.pEax;
        break;

    case REGISTER_X86_ECX:
        ret = m_rd.pEcx;
        break;

    case REGISTER_X86_EDX:
        ret = m_rd.pEdx;
        break;

    case REGISTER_X86_EBX:
        ret = m_rd.pEbx;
        break;

    case REGISTER_X86_ESI:
        ret = m_rd.pEsi;
        break;

    case REGISTER_X86_EDI:
        ret = m_rd.pEdi;
        break;

#endif
    default:
        _ASSERT(!"Invalid register number!");
    }

    return ret;
}


HRESULT CordbNativeFrame::GetLocalRegisterValue(CorDebugRegister reg,
                                     ULONG cbSigBlob,
                                     PCCOR_SIGNATURE pvSigBlob,
                                     ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    VALIDATE_POINTER_TO_OBJECT_ARRAY(pvSigBlob, BYTE, cbSigBlob, true, false);
    CordbType *typ;

    SigParser sigParser(pvSigBlob, cbSigBlob);

    HRESULT hr = CordbType::SigToType(m_JITILFrame->GetModule(), &sigParser, Instantiation(), &typ);

    if (FAILED(hr))
    {
        return hr;
    }

    return GetLocalRegisterValue(reg, typ, ppValue);
}

HRESULT CordbNativeFrame::GetLocalDoubleRegisterValue(CorDebugRegister highWordReg,
                                           CorDebugRegister lowWordReg,
                                           ULONG cbSigBlob,
                                           PCCOR_SIGNATURE pvSigBlob,
                                           ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    if (cbSigBlob == 0)
    {
        return E_INVALIDARG;
    }

    CordbType *typ;

    SigParser sigParser(pvSigBlob, cbSigBlob);

    HRESULT hr = CordbType::SigToType(m_JITILFrame->GetModule(), &sigParser, Instantiation(), &typ);

    if (FAILED(hr))
    {
        return hr;
    }

    //VALIDATE_POINTER_TO_OBJECT_ARRAY(pvSigBlob, BYTE, cbSigBlob, true, false);
    return GetLocalDoubleRegisterValue(highWordReg, lowWordReg, typ, ppValue);
}

HRESULT CordbNativeFrame::GetLocalMemoryValue(CORDB_ADDRESS address,
                                   ULONG cbSigBlob,
                                   PCCOR_SIGNATURE pvSigBlob,
                                   ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    VALIDATE_POINTER_TO_OBJECT_ARRAY(pvSigBlob, BYTE, cbSigBlob, true, false);

    CordbType *typ;
    SigParser sigParser(pvSigBlob, cbSigBlob);

    HRESULT hr = CordbType::SigToType(m_JITILFrame->GetModule(), &sigParser, Instantiation(), &typ);

    if (FAILED(hr))
    {
        return hr;
    }

    return GetLocalMemoryValue(address, typ, ppValue);
}

HRESULT CordbNativeFrame::GetLocalRegisterMemoryValue(CorDebugRegister highWordReg,
                                           CORDB_ADDRESS lowWordAddress,
                                           ULONG cbSigBlob,
                                           PCCOR_SIGNATURE pvSigBlob,
                                           ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    if (cbSigBlob == 0)
    {
        return E_INVALIDARG;
    }

    VALIDATE_POINTER_TO_OBJECT_ARRAY(pvSigBlob, BYTE, cbSigBlob, true, true);

    CordbType *typ;

    SigParser sigParser(pvSigBlob, cbSigBlob);

    HRESULT hr = CordbType::SigToType(m_JITILFrame->GetModule(), &sigParser, Instantiation(), &typ);

    if (FAILED(hr))
    {
        return hr;
    }

    return GetLocalRegisterMemoryValue(highWordReg, lowWordAddress, typ, ppValue);
}


HRESULT CordbNativeFrame::GetLocalMemoryRegisterValue(CORDB_ADDRESS highWordAddress,
                                           CorDebugRegister lowWordRegister,
                                           ULONG cbSigBlob,
                                           PCCOR_SIGNATURE pvSigBlob,
                                           ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this->GetProcess());

    if (cbSigBlob == 0)
    {
        return E_INVALIDARG;
    }

    VALIDATE_POINTER_TO_OBJECT_ARRAY(pvSigBlob, BYTE, cbSigBlob, true, true);

    CordbType *typ;

    SigParser sigParser(pvSigBlob, cbSigBlob);

    HRESULT hr = CordbType::SigToType(m_JITILFrame->GetModule(), &sigParser, Instantiation(), &typ);

    if (FAILED(hr))
    {
        return hr;
    }

    return GetLocalMemoryRegisterValue(highWordAddress, lowWordRegister, typ, ppValue);
}



HRESULT CordbNativeFrame::GetLocalRegisterValue(CorDebugRegister reg,
                                                CordbType *type,
                                                ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

#if defined(_X86_) || defined(_WIN64)
#if defined(_X86_)
    if ((reg >= REGISTER_X86_FPSTACK_0) && (reg <= REGISTER_X86_FPSTACK_7))
#endif
    {
        return E_NOTIMPL;
    }
#endif

    // The address of the given register is the address of the value
    // in this process. We have no remote address here.
    void *pLocalValue = (void*)GetAddressOfRegister(reg);

    // Remember the register info as we create the value.
    RemoteAddress ra;
    ra.kind = RAK_REG;
    ra.reg1 = reg;
    ra.reg1Addr = GetLeftSideAddressOfRegister(reg);
    ra.frame = this;

    ICorDebugValue *pValue;
    HRESULT hr = CordbValue::CreateValueByType(GetCurrentAppDomain(),
                                               type,
                                               false,
                                               NULL, pLocalValue,
                                               &ra,
                                               (IUnknown*)(ICorDebugNativeFrame*)this,
                                               &pValue);

    if (SUCCEEDED(hr))
        *ppValue = pValue;

    return hr;
}

HRESULT CordbNativeFrame::GetLocalDoubleRegisterValue(
                                            CorDebugRegister highWordReg,
                                            CorDebugRegister lowWordReg,
                                            CordbType *type,
                                            ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // Remember the register info as we create the value.
    RemoteAddress ra;
    ra.kind = RAK_REGREG;
    ra.reg1 = highWordReg;
    ra.reg1Addr = GetLeftSideAddressOfRegister(highWordReg);
    ra.u.reg2 = lowWordReg;
    ra.u.reg2Addr = GetLeftSideAddressOfRegister(lowWordReg);
    ra.frame = this;

    IUnknown* pUnk;
    HRESULT hr =
        this->QueryInterface(IID_IUnknown, reinterpret_cast< void **>(&pUnk));
    _ASSERTE(SUCCEEDED(hr));

    hr = CordbValue::CreateValueByType(GetCurrentAppDomain(), type, false,
                                       NULL, NULL,
                                       &ra, pUnk, ppValue);
#ifdef _DEBUG
    {
        // sanity check object size
        if (SUCCEEDED(hr))
        {
            ULONG32 objectSize;
            hr = (*ppValue)->GetSize(&objectSize);
            _ASSERTE(SUCCEEDED(hr));
            //
            // nickbe
            // 10/31/2002 11:09:42
            //
            // This assert assumes that the JIT will only partially enregister
            // objects that have a size equal to twice the size of a register.
            //
            _ASSERTE(objectSize == 2 * sizeof(void*));
        }
    }
#endif
    pUnk->Release();
    return hr;
}

HRESULT
CordbNativeFrame::GetLocalMemoryValue(CORDB_ADDRESS address,
                                      CordbType *type,
                                      ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    _ASSERTE(m_jitinfo->m_function != NULL);

    ICorDebugValue *pValue;
    HRESULT hr = CordbValue::CreateValueByType(GetCurrentAppDomain(),
                                               type,
                                               false,
                                               (REMOTE_PTR) CORDB_ADDRESS_TO_PTR(address), NULL,
                                               NULL,
                                               (IUnknown*)(ICorDebugNativeFrame*)this,
                                               &pValue);

    if (SUCCEEDED(hr))
        *ppValue = pValue;

    return hr;
}

HRESULT
CordbNativeFrame::GetLocalByRefMemoryValue(CORDB_ADDRESS address,
                                           CordbType *type,
                                           ICorDebugValue **ppValue)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    LPVOID actualAddress = NULL;
    BOOL success = ReadProcessMemoryI(GetProcess()->m_handle,
                                      reinterpret_cast<LPVOID>CORDB_ADDRESS_TO_PTR(address),
                                      &actualAddress, sizeof(actualAddress), NULL);
    if (!success)
    {
        return HRESULT_FROM_GetLastError();
    }

    return GetLocalMemoryValue(PTR_TO_CORDB_ADDRESS(actualAddress), type, ppValue);
}

HRESULT
CordbNativeFrame::GetLocalRegisterMemoryValue(CorDebugRegister highWordReg,
                                              CORDB_ADDRESS lowWordAddress,
                                              CordbType *type,
                                              ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // Remember the register info as we create the value.
    RemoteAddress ra;
    ra.kind = RAK_REGMEM;
    ra.reg1 = highWordReg;
    ra.reg1Addr = GetLeftSideAddressOfRegister(highWordReg);
    ra.addr = lowWordAddress;
    ra.frame = this;

    IUnknown* pUnk;
    HRESULT hr =
        this->QueryInterface(IID_IUnknown, reinterpret_cast< void **>(&pUnk));
    _ASSERTE(SUCCEEDED(hr));

    hr = CordbValue::CreateValueByType(GetCurrentAppDomain(), type, false,
                                       NULL, NULL,
                                       &ra, pUnk, ppValue);
#ifdef _DEBUG
    {
        if (SUCCEEDED(hr))
        {
            ULONG32 objectSize;
            hr = (*ppValue)->GetSize(&objectSize);
            _ASSERTE(SUCCEEDED(hr));
            // See the comment in CordbNativeFrame::GetLocalDoubleRegisterValue
            // for more information on this assertion
            _ASSERTE(objectSize == 2 * sizeof(void*));
        }
    }
#endif
    pUnk->Release();
    return hr;
}

HRESULT
CordbNativeFrame::GetLocalMemoryRegisterValue(CORDB_ADDRESS highWordAddress,
                                              CorDebugRegister lowWordRegister,
                                              CordbType *type,
                                              ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // Remember the register info as we create the value.
    RemoteAddress ra;
    ra.kind = RAK_MEMREG;
    ra.reg1 = lowWordRegister;
    ra.reg1Addr = GetLeftSideAddressOfRegister(lowWordRegister);
    ra.addr = highWordAddress;
    ra.frame = this;

    IUnknown* pUnk;
    HRESULT hr =
        this->QueryInterface(IID_IUnknown, reinterpret_cast< void **>(&pUnk));
    _ASSERTE(SUCCEEDED(hr));

    hr = CordbValue::CreateValueByType(GetCurrentAppDomain(), type, false,
                                       NULL, NULL,
                                       &ra, pUnk, ppValue);
#ifdef _DEBUG
    {
        if (SUCCEEDED(hr))
        {
            ULONG32 objectSize;
            hr = (*ppValue)->GetSize(&objectSize);
            _ASSERTE(SUCCEEDED(hr));
            // See the comment in CordbNativeFrame::GetLocalDoubleRegisterValue
            // for more information on this assertion
            _ASSERTE(objectSize == 2 * sizeof(void*));
        }
    }
#endif
    pUnk->Release();
    return hr;
}


SIZE_T CordbNativeFrame::GetInspectionIP()
{
    return m_ip;
}



/* ------------------------------------------------------------------------- *
 * JIT-IL Frame class
 * ------------------------------------------------------------------------- */

CordbJITILFrame::CordbJITILFrame(CordbNativeFrame *nativeFrame,
                                 CordbCode* code,
                                 UINT_PTR ip,
                                 CorDebugMappingResult mapping,
                                 void *exactGenericArgsToken,
                                 bool fVarArgFnx,
                                 void *sig,
                                 ULONG cbSig,
                                 void *rpFirstArg)
  : CordbBase(nativeFrame->GetProcess(), 0, enumCordbJITILFrame), 
    m_nativeFrame(nativeFrame), m_ilCode(code), m_ip(ip),
    m_mapping(mapping),
    m_fVarArgFnx(fVarArgFnx),
    m_argCount(0),
    m_sigBuf((BYTE *)sig),
    m_cbSigBuf(cbSig),
    m_rpFirstArg(rpFirstArg),
    m_rgNVI(NULL),
    m_genericArgs(),
    m_genericArgsLoaded(false),
    m_frameParamsToken(exactGenericArgsToken)
{
    //
    // m_sigBuf is initially a LS pointer, the Init() routine will bring that across
    // into a local buffer and create the correct SigParser.
    //
    m_sigParserCached = SigParser(NULL, 0);
}

/*
 * CordbJITILFrame::Init
 *
 * Initialize a CordbValueEnum object. Must be called after allocating the object and before using it. If Init
 * fails, then destroy the object and release the memory.
 *
 * Parameters:
 *     none.
 *
 * Returns:
 *    HRESULT for success or failure.
 *
 */
HRESULT CordbJITILFrame::Init()
{
    HRESULT hr = S_OK;

    _ASSERTE(m_ilCode != NULL);

    if (m_fVarArgFnx == true)
    {
        BOOL methodIsStatic;

        IfFailRet(m_ilCode->m_function->GetSig(NULL, NULL, &methodIsStatic));

        // m_sigBuf is initially a remote value - copy it over
        if (m_sigBuf != NULL)
        {
            SIZE_T cbRead;
            BYTE *pbBuf = new (nothrow) BYTE [m_cbSigBuf];

            if (pbBuf == NULL)
            {
                return E_OUTOFMEMORY;
            }

            if (!ReadProcessMemory(GetProcess()->m_handle,
                                   m_sigBuf,
                                   pbBuf, //overwrite
                                   m_cbSigBuf,
                                   (SIZE_T *)&cbRead)
                || cbRead != m_cbSigBuf)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());

                LOG((LF_CORDB,LL_INFO1000, "Failed to grab left side varargs!"));

                delete [] pbBuf;
                m_sigBuf = NULL;

                return hr;
            }

            m_sigBuf = (BYTE *)pbBuf;

            _ASSERTE(m_cbSigBuf > 0);

            m_sigParserCached = SigParser(m_sigBuf, m_cbSigBuf);

            SigParser sigParser = m_sigParserCached;

            // get the actual count of arguments
            IfFailRet(sigParser.SkipMethodHeaderSignature(&m_argCount));

            if (!methodIsStatic)
            {
                m_argCount++; //hidden argument 'This'
            }

            m_rgNVI = new (nothrow) ICorJitInfo::NativeVarInfo[m_argCount];

            if (m_rgNVI != NULL)
            {
                _ASSERTE(ICorDebugInfo::VLT_COUNT <= ICorDebugInfo::VLT_INVALID);

                for (ULONG i = 0; i < m_argCount; i++)
                {
                    m_rgNVI[i].loc.vlType = ICorDebugInfo::VLT_INVALID;
                }
            }
            else
            {
                hr = E_OUTOFMEMORY;
            }
        }
    }

    return hr;
}

/*
    A list of which resources owned by this object are accounted for.

    UNKNOWN:
        CordbNativeFrame* m_nativeFrame;
        CordbCode*        m_ilCode;
        CorDebugMappingResult m_mapping;
        void *            m_rpFirstArg;
        BYTE *            m_sigBuf; // Deleted in neuter
        ICorJitInfo::NativeVarInfo * m_rgNVI; // Deleted in neuter
        CordbClass **m_genericArgs;
*/

CordbJITILFrame::~CordbJITILFrame()
{
    _ASSERTE(IsNeutered());
}

// Neutered by CordbNativeFrame
void CordbJITILFrame::Neuter(NeuterTicket ticket)
{
    // Since neutering here calls Release directly, we don't want to double-release
    // if neuter is called multiple times.
    if (IsNeutered())
    {
        return;
    }

    RSSmartPtr<CordbJITILFrame> pRef(this);
    {
        // If this class ever inherits from the CordbFrame we'll need a call
        // to CordbFrame::Neuter() here instead of to CordbBase::Neuter();
        CordbBase::Neuter(ticket);
        // Frames include pointers across to other types that specify the
        // representation instantiation - reduce the reference counts on these....
        for (unsigned int i = 0; i < m_genericArgs.m_cInst; i++) {
            m_genericArgs.m_ppInst[i]->Release();
        }

        if (m_sigBuf != NULL)
        {
            delete [] m_sigBuf;
            m_sigBuf = NULL;
        }

        if (m_rgNVI != NULL)
        {
            delete [] m_rgNVI;
            m_rgNVI = NULL;
        }
    }
}

// Load the real type parameters for the frame if possible
HRESULT CordbJITILFrame::LoadGenericArgs()
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    // The case where there are no type parameters, or the case where we've
    // already feched the realInst, is easy.
    if (m_genericArgsLoaded)
        return S_OK;

    _ASSERTE(m_nativeFrame->m_jitinfo != NULL);

    if (!m_nativeFrame->m_jitinfo->m_isInstantiatedGeneric)
    {
        m_genericArgs = Instantiation(0, NULL,0);
        m_genericArgsLoaded = true;
        return S_OK;
    }

    // Find the exact generic arguments for a frame that is executing
    // a generic method.  The left-side will fetch these from arguments
    // given on the stack and/or from the IP.


    HRESULT hr = S_OK;
    unsigned int genericArgsCount = 0;
    unsigned int genericClassArgsCount = 0;
    CordbType **ppGenericArgs = NULL;
    DebuggerIPCEvent *retEvent = NULL;
    bool wait;
    bool fFirstEvent;
    unsigned int currIndex;

    DebuggerIPCEvent event;
    // Collect up all the type parameters for the type context of the method,
    // including both class and method type parameters if it is a generic method
    // in a generic class.
    GetProcess()->InitIPCEvent(&event,
                               DB_IPCE_GET_METHOD_DESC_PARAMS,
                               false,
                               GetCurrentAppDomain()->GetADToken());
    event.GetMethodDescParams.methodDesc = m_nativeFrame->m_jitinfo->m_nativecode->m_nativeCodeMethodDescToken;
    event.GetMethodDescParams.exactGenericArgsToken = m_frameParamsToken;

    hr = GetProcess()->m_cordb->SendIPCEvent(GetProcess(), &event,sizeof(DebuggerIPCEvent));

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr)) {
        _ASSERTE( !"FAILURE" );
        goto exit;
    }

    hr = event.hr;

    // Wait for events to return from the RC. We expect at least one
    // class info result event.
    retEvent = (DebuggerIPCEvent *) _alloca(CorDBIPC_BUFFER_SIZE);

    wait = true;
    fFirstEvent = true;
    currIndex = 0;

    while (wait)
    {

        hr = GetProcess()->m_cordb->WaitForIPCEventFromProcess(GetProcess(),
                                                               GetCurrentAppDomain(),
                                                               retEvent);

        if (!SUCCEEDED(hr))
            goto exit;

        _ASSERTE(retEvent->type == DB_IPCE_GET_METHOD_DESC_PARAMS_RESULT);

        if (fFirstEvent)
        {
            fFirstEvent = false;
            genericArgsCount = retEvent->GetMethodDescParamsResult.totalGenericArgsCount;
            genericClassArgsCount = retEvent->GetMethodDescParamsResult.totalGenericClassArgsCount;

            ppGenericArgs = new (nothrow) CordbType *[genericArgsCount];
            if (ppGenericArgs == NULL) {
                hr = E_OUTOFMEMORY;
                goto exit;
            }
        }

        DebuggerIPCE_ExpandedTypeData *currentTyParData =
            &(retEvent->GetMethodDescParamsResult.genericArgs);


        for (unsigned int i = 0; i < retEvent->GetMethodDescParamsResult.genericArgsCount;i++)
        {
            hr = CordbType::TypeDataToType(GetCurrentAppDomain(), currentTyParData, &ppGenericArgs[currIndex]);
            if (!SUCCEEDED(hr))
                goto exit;
            // We add a ref as the instantiation will be stored away in the
            // ref-counted data structure associated with the JITILFrame
            ppGenericArgs[currIndex]->AddRef();

            currIndex++;
            currentTyParData++;
        }

        if (currIndex >= genericArgsCount)
            wait = false;
    }

    m_genericArgs = Instantiation(genericArgsCount, ppGenericArgs,genericClassArgsCount);
    m_genericArgsLoaded = true;

    _ASSERTE(hr == S_OK);
exit:
    if (FAILED(hr))
    {
        if (ppGenericArgs != NULL)
        {
            delete [] ppGenericArgs;
        }
    }
    return hr;

}


//
// CordbJITILFrame::QueryInterface
//
// Description
//  Interface query for this COM object
//
//  NOTE: the COM object associated with this CordbJITILFrame may consist of two
//  C++ objects (a CordbJITILFrame and its associated CordbNativeFrame)
//
// Parameters
//      id              the GUID associated with the requested interface
//      pInterface      [out] the interface pointer
//
// Returns
//  HRESULT
//      S_OK            If this CordbJITILFrame supports the interface
//      E_NOINTERFACE   If this object does not support the interface
//
// Exceptions
//  None
//
HRESULT CordbJITILFrame::QueryInterface(REFIID id, void **pInterface)
{
    if (NULL != m_nativeFrame)
    {
        // If the native frame does not support the requested interface, then
        // the native fram is responsible for delegating the query back to this
        // object through QueryInterfaceInternal(...)
        return m_nativeFrame->QueryInterface(id, pInterface);
    }

    // no native frame. Check for interfaces common to CordbNativeFrame and
    // CordbJITILFrame
    if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugILFrame*>(this));
    }
    else if (id == IID_ICorDebugFrame)
    {
        *pInterface = static_cast<ICorDebugFrame*>(this);
    }
    else
    {
        // didn't find an interface yet. Since there's no native frame
        // associated with this IL frame, go ahead and check for the IL frame
        return this->QueryInterfaceInternal(id, pInterface);
    }

    ExternalAddRef();
    return S_OK;
}

//
// CordbJITILFrame::QueryInterfaceInternal
//
// Description
//  Interface query for interfaces implemented ONLY by CordbJITILFrame (as
//  opposed to interfaces implemented by both CordbNativeFrame and
//  CordbJITILFrame)
//
// Parameters
//      id              the GUID associated with the requested interface
//      pInterface      [out] the interface pointer
//  NOTE:   id must not be IUnknown or ICorDebugFrame
//  NOTE:   if this object is in "forward compatibility mode", passing in
//          IID_ICorDebugILFrame2 for the id will result in a failure (returns
//          E_NOINTERFACE)
//
// Returns
//  HRESULT
//      S_OK            If this CordbJITILFrame supports the interface
//      E_NOINTERFACE   If this object does not support the interface
//
// Exceptions
//  None
//
HRESULT
CordbJITILFrame::QueryInterfaceInternal(REFIID id, void** pInterface)
{
    _ASSERTE(IID_ICorDebugFrame != id);
    _ASSERTE(IID_IUnknown != id);

    // don't query for IUnknown or ICorDebugFrame! Someone else should have
    // already taken care of that.
    if (id == IID_ICorDebugILFrame)
    {
        *pInterface = static_cast<ICorDebugILFrame*>(this);
    }
    else if (id == IID_ICorDebugILFrame2)
    {
        *pInterface = static_cast<ICorDebugILFrame2*>(this);
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}


HRESULT CordbJITILFrame::EnumerateTypeParameters(ICorDebugTypeEnum **ppTypeParameterEnum)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppTypeParameterEnum, ICorDebugTypeEnum **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr;
    IfFailRet(this->LoadGenericArgs());
    CordbTypeEnum * pEnum = CordbTypeEnum::Build(GetCurrentAppDomain(), this->m_genericArgs.m_cInst, this->m_genericArgs.m_ppInst);
    if ( pEnum == NULL )
    {
        (*ppTypeParameterEnum) = NULL;
        return E_OUTOFMEMORY;
    }

    // This Enum is enumerating the current Frame's type-parameters.
    // Neuter it at the same time the current frame gets neutered.
    // It will also be on the AD neuter list.
    if (m_nativeFrame != NULL)
    {
        this->m_nativeFrame->m_thread->GetRefreshStackNeuterList()->Add(pEnum);
    }

    ICorDebugTypeEnum *icdTPE = pEnum;
    (*ppTypeParameterEnum) = icdTPE;
    pEnum->ExternalAddRef();
    return S_OK;
}


HRESULT CordbJITILFrame::GetChain(ICorDebugChain **ppChain)
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppChain, ICorDebugChain **);

    CordbChain * pChain = m_nativeFrame->m_chain;
    *ppChain = static_cast<ICorDebugChain *> (pChain);
    pChain->ExternalAddRef();

    return S_OK;
}

HRESULT CordbJITILFrame::GetCode(ICorDebugCode **ppCode)
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppCode, ICorDebugCode **);

    *ppCode = static_cast<ICorDebugCode*> (m_ilCode);
    m_ilCode->ExternalAddRef();

    return S_OK;;
}

HRESULT CordbJITILFrame::GetFunction(ICorDebugFunction **ppFunction)
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppFunction, ICorDebugFunction **);

    CordbFunction * pFunc = m_nativeFrame->m_jitinfo->m_function;
    *ppFunction = static_cast<ICorDebugFunction*> (pFunc);
    pFunc->ExternalAddRef();

    return S_OK;
}

HRESULT CordbJITILFrame::GetFunctionToken(mdMethodDef *pToken)
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pToken, mdMethodDef *);

    *pToken = m_nativeFrame->m_jitinfo->m_function->m_token;

    return S_OK;
}

HRESULT CordbJITILFrame::GetStackRange(CORDB_ADDRESS *pStart, CORDB_ADDRESS *pEnd)
{
    PUBLIC_REENTRANT_API_ENTRY(this);

    // The access of m_nativeFrame is not safe here. It's a weak reference.
    OK_IF_NEUTERED(this);
    return m_nativeFrame->GetStackRange(pStart, pEnd);
}

HRESULT CordbJITILFrame::GetCaller(ICorDebugFrame **ppFrame)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    return m_nativeFrame->GetCaller(ppFrame);
}

HRESULT CordbJITILFrame::GetCallee(ICorDebugFrame **ppFrame)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    return m_nativeFrame->GetCallee(ppFrame);
}

HRESULT CordbJITILFrame::CreateStepper(ICorDebugStepper **ppStepper)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    //
    // !!! should this stepper somehow remember that it does IL->native mapping?
    //
    return m_nativeFrame->CreateStepper(ppStepper);
}

HRESULT CordbJITILFrame::GetIP(ULONG32 *pnOffset,
                               CorDebugMappingResult *pMappingResult)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pnOffset, ULONG32 *);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pMappingResult, CorDebugMappingResult *);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    *pnOffset = (ULONG32)m_ip;
    if (pMappingResult)
        *pMappingResult = m_mapping;

    return S_OK;
}

// Quick accessor to tell if we're the leaf.
bool CordbFrame::IsLeafFrame()
{
    // Should only be called by non-neutered stuff.
    // If we're an existing non-neutered frame, then we expect both m_stackChains & m_stackFrames to be non-null
    // and have at least 1 element.
    // Also, since we're not neutered, we know we have a Thread object, and we know it's state is current.
    _ASSERTE(!this->IsNeutered());


    _ASSERTE(m_thread != NULL); // not neutered, so should have a thread
    CorDebugUserState state = m_thread->m_userState;
    if ((state & USER_WAIT_SLEEP_JOIN) != 0)
    {
        return false;
    }

    CordbChain * pThisChain = this->m_chain;
    CordbChain * pLeafChain = pThisChain->m_thread->m_stackChains[0];
    if (pThisChain != pLeafChain)
    {
        return false;
    }

    CordbFrame *pLeafFrame = this->m_chain->m_thread->m_stackFrames[0];
    return (pLeafFrame == this);
}

bool CordbFrame::IsContainedInFrame(FramePointer fp)
{
    CORDB_ADDRESS stackStart;
    CORDB_ADDRESS stackEnd;

    HRESULT hr;
    hr = GetStackRange(&stackStart, &stackEnd);
    _ASSERTE(SUCCEEDED(hr));

    CORDB_ADDRESS sp  = PTR_TO_CORDB_ADDRESS(fp.GetSPValue());

    if ((stackStart <= sp) && (sp <= stackEnd)
        IA64_ONLY(&& (PTR_TO_CORDB_ADDRESS(GetFramePointer().GetBSPValue()) == bsp))
       )
    {
        return true;
    }
    else
    {
        return false;
    }
}

// static
CordbFrame* CordbFrame::GetCordbFrameFromInterface(ICorDebugFrame *pFrame)
{
    CordbFrame* pTargetFrame = NULL;
    {
        RSExtSmartPtr<ICorDebugNativeFrame> pNativeFrame;
        pFrame->QueryInterface(IID_ICorDebugNativeFrame, (void**)&pNativeFrame);
        if (pNativeFrame != NULL)
        {
            pTargetFrame = static_cast<CordbFrame*>(static_cast<CordbNativeFrame*>(pNativeFrame.GetValue()));
        }
        else
        {
            RSExtSmartPtr<ICorDebugILFrame> pILFrame;
            pFrame->QueryInterface(IID_ICorDebugILFrame, (void**)&pILFrame);
            if (pILFrame != NULL)
            {
                pTargetFrame = (static_cast<CordbJITILFrame*>(pILFrame.GetValue()))->m_nativeFrame;
            }
            else
            {
                RSExtSmartPtr<ICorDebugInternalFrame> pInternalFrame;
                pFrame->QueryInterface(IID_ICorDebugInternalFrame, (void**)&pInternalFrame);
                if (pInternalFrame != NULL)
                {
                    pTargetFrame = static_cast<CordbFrame*>(static_cast<CordbInternalFrame*>(pInternalFrame.GetValue()));
                }
                else
                {
                    pTargetFrame = static_cast<CordbFrame*>(pFrame);
                }
            }
        }
    }
    return pTargetFrame;
}


HRESULT CordbJITILFrame::CanSetIP(ULONG32 nOffset)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    _ASSERTE(!m_nativeFrame->m_chain->m_thread->m_stackFrames.IsEmpty() &&
             !m_nativeFrame->m_chain->m_thread->m_stackChains.IsEmpty());

    // Check to see that this is a leaf frame
    if (!m_nativeFrame->IsLeafFrame())
    {
        return CORDBG_E_SET_IP_NOT_ALLOWED_ON_NONLEAF_FRAME;
    }

    HRESULT hr = m_nativeFrame->m_chain->m_thread->SetIP(
                    SetIP_fCanSetIPOnly,
                    m_nativeFrame->m_jitinfo->m_function->m_module->m_debuggerModuleToken,
                    m_nativeFrame->m_jitinfo->m_function->m_token,
                    m_nativeFrame->m_jitinfo->m_nativecode->m_nativeCodeJITInfoToken,
                    nOffset,
                    SetIP_fIL );

    return hr;
}

HRESULT CordbJITILFrame::SetIP(ULONG32 nOffset)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    _ASSERTE(!m_nativeFrame->m_chain->m_thread->m_stackFrames.IsEmpty() &&
             !m_nativeFrame->m_chain->m_thread->m_stackChains.IsEmpty());

// Check to see that this is a leaf frame
    if (!m_nativeFrame->IsLeafFrame())
    {
        return CORDBG_E_SET_IP_NOT_ALLOWED_ON_NONLEAF_FRAME;
    }

    HRESULT hr = m_nativeFrame->m_chain->m_thread->SetIP(
                    SetIP_fSetIP,
                    m_nativeFrame->m_jitinfo->m_function->m_module->m_debuggerModuleToken,
                    m_nativeFrame->m_jitinfo->m_function->m_token,
                    m_nativeFrame->m_jitinfo->m_nativecode->m_nativeCodeJITInfoToken,
                    nOffset,
                    SetIP_fIL );

    return hr;
}

HRESULT CordbJITILFrame::FabricateNativeInfo(DWORD dwIndex,
                                      ICorJitInfo::NativeVarInfo **ppNativeInfo)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(this->GetProcess()); //
    _ASSERTE( m_fVarArgFnx );

    if (m_rgNVI == NULL)
        return E_OUTOFMEMORY;

    HRESULT hr = S_OK;

    // If we already made it.
    if (m_rgNVI[dwIndex].loc.vlType != ICorDebugInfo::VLT_INVALID)
    {
        (*ppNativeInfo) = &m_rgNVI[dwIndex];
        return S_OK;
    }

    // We'll initialize everything at once
    ULONG cbArchitectureMin;
    BYTE *rpCur = (BYTE *)m_rpFirstArg;

    cbArchitectureMin = 4;

    SigParser sigParser = m_sigParserCached;

    IfFailRet(sigParser.SkipMethodHeaderSignature(NULL));

    ULONG32 cbType;

    CordbType *argType;

    IfFailRet( this->LoadGenericArgs() );

    IfFailRet( CordbType::SigToType(GetModule(), &sigParser, this->m_genericArgs, &argType) );

    hr = argType->GetUnboxedObjectSize(&cbType);

    if (FAILED(hr))
        return hr;

#ifdef _X86_ // STACK_GROWS_DOWN_ON_ARGS_WALK
    // The the rpCur pointer starts off in the right spot for the
    // first argument, but thereafter we have to decrement it
    // before getting the variable's location from it.  So increment
    // it here to be consistent later.
    rpCur += max(cbType, cbArchitectureMin);
#endif

    // Grab the IL code's function's method signature so we can see if it's static.
    BOOL methodIsStatic;

    IfFailRet(m_ilCode->m_function->GetSig(NULL, NULL, &methodIsStatic));

    ULONG i;
    if (methodIsStatic)
        i = 0;
    else
        i = 1;

    for ( ; i < m_argCount; i++)
    {
        m_rgNVI[i].startOffset = 0;
        m_rgNVI[i].endOffset = 0xFFffFFff;
        m_rgNVI[i].varNumber = i;
        m_rgNVI[i].loc.vlType = ICorDebugInfo::VLT_FIXED_VA;

        IfFailRet(this->LoadGenericArgs());

        hr = CordbType::SigToType(GetModule(), &sigParser, this->m_genericArgs, &argType);
        if (FAILED(hr))
            return hr;

        hr = argType->GetUnboxedObjectSize(&cbType);
        if (FAILED(hr))
            return hr;

#ifdef _X86_ // STACK_GROWS_DOWN_ON_ARGS_WALK
        rpCur -= max(cbType, cbArchitectureMin);
        m_rgNVI[i].loc.vlFixedVarArg.vlfvOffset = (unsigned int)((BYTE *)m_rpFirstArg - rpCur);

        // Since the JIT adds in the size of this field, we do too to
        // be consistent.
        m_rgNVI[i].loc.vlFixedVarArg.vlfvOffset +=
            sizeof(((CORINFO_VarArgInfo*)0)->argBytes);
#else // STACK_GROWS_UP_ON_ARGS_WALK
        m_rgNVI[i].loc.vlFixedVarArg.vlfvOffset = (unsigned)(rpCur - (BYTE *)m_rpFirstArg);
        rpCur += max(cbType, cbArchitectureMin);
#endif

        IfFailRet(sigParser.SkipExactlyOne());
    }

    (*ppNativeInfo) = &m_rgNVI[dwIndex];
    return S_OK;
}

HRESULT CordbJITILFrame::ILVariableToNative(DWORD dwIndex,
                                            ICorJitInfo::NativeVarInfo **ppNativeInfo)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    // We keep the fixed argument native var infos in the
    // CordbFunction, which only is an issue for var args info:
    if (!m_fVarArgFnx || //not  a var args function
        (dwIndex < m_nativeFrame->m_jitinfo->m_argumentCount) || // var args,fixed arg
           // note that this include the implicit 'this' for nonstatic fnxs
        (dwIndex >= m_argCount) ||// var args, local variable
        (m_sigBuf == NULL)) //we don't have any VA info
    {
        // If we're in a var args fnx, but we're actually looking
        // for a local variable, then we want to use the variable
        // index as the function sees it - fixed (but not var)
        // args are added to local var number to get native info
        if (m_fVarArgFnx && (dwIndex >= m_argCount) && (m_sigBuf != NULL))
        {
            dwIndex -= m_argCount;
            dwIndex += m_nativeFrame->m_jitinfo->m_argumentCount;
        }

        return m_nativeFrame->m_jitinfo->ILVariableToNative(dwIndex,
                                             m_nativeFrame->GetInspectionIP(),
                                             ppNativeInfo);
    }

    return FabricateNativeInfo(dwIndex,ppNativeInfo);
}

HRESULT CordbJITILFrame::GetArgumentType(DWORD dwIndex,
                                         CordbType **res)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    HRESULT hr;
    IfFailRet(this->LoadGenericArgs());
    if (m_fVarArgFnx && (m_sigBuf != NULL))
    {
        SigParser sigParser = m_sigParserCached;

        IfFailRet(sigParser.SkipMethodHeaderSignature(NULL));

        // Grab the IL code's function's method signature so we can see if it's static.
        BOOL methodIsStatic;

        IfFailRet(m_ilCode->m_function->GetSig(NULL, NULL, &methodIsStatic));

        if (!methodIsStatic)
        {
            if (dwIndex == 0)
            {
                // Return the signature for the 'this' pointer for the
                // class this method is in.

                return m_ilCode->m_function->m_class->GetThisType(this->m_genericArgs, res);
            }
            else
                dwIndex--;
        }

        for (ULONG i = 0; i < dwIndex; i++)
        {
            IfFailRet(sigParser.SkipExactlyOne());
        }

        IfFailRet(sigParser.SkipFunkyAndCustomModifiers());

        IfFailRet(sigParser.SkipAnyVASentinel());

        return CordbType::SigToType(GetModule(), &sigParser, this->m_genericArgs, res);
    }
    else
    {
        return m_ilCode->m_function->GetArgumentType(dwIndex,
                                                     this->m_genericArgs,
                                                     res);
    }
}

//
// GetNativeVariable uses the JIT variable information to delegate to
// the native frame when the value is really created.
//
HRESULT CordbJITILFrame::GetNativeVariable(CordbType *type,
                                           ICorJitInfo::NativeVarInfo *pJITInfo,
                                           ICorDebugValue **ppValue)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    HRESULT hr = S_OK;


    switch (pJITInfo->loc.vlType)
    {
    case ICorJitInfo::VLT_REG:
        hr = m_nativeFrame->GetLocalRegisterValue(
                                 ConvertRegNumToCorDebugRegister(pJITInfo->loc.vlReg.vlrReg),
                                 type, ppValue);
        break;

    case ICorJitInfo::VLT_REG_BYREF:
        {
            CORDB_ADDRESS pRemoteByRefAddr = PTR_TO_CORDB_ADDRESS(
                *( m_nativeFrame->GetAddressOfRegister(ConvertRegNumToCorDebugRegister(pJITInfo->loc.vlReg.vlrReg))) );

            hr = m_nativeFrame->GetLocalMemoryValue(pRemoteByRefAddr,
                                                    type,
                                                    ppValue);
        }
        break;


    case ICorJitInfo::VLT_STK_BYREF:
        {
            CORDB_ADDRESS pRemoteByRefAddr = m_nativeFrame->GetLSStackAddress(
                pJITInfo->loc.vlStk.vlsBaseReg, pJITInfo->loc.vlStk.vlsOffset) ;

            hr = m_nativeFrame->GetLocalByRefMemoryValue(pRemoteByRefAddr,
                                                         type,
                                                         ppValue);
        }
        break;

    case ICorJitInfo::VLT_STK:
        {
            CORDB_ADDRESS pRemoteValue = m_nativeFrame->GetLSStackAddress(
                pJITInfo->loc.vlStk.vlsBaseReg, pJITInfo->loc.vlStk.vlsOffset) ;

            hr = m_nativeFrame->GetLocalMemoryValue(pRemoteValue,
                                                    type,
                                                    ppValue);
        }
        break;

    case ICorJitInfo::VLT_REG_REG:
        hr = m_nativeFrame->GetLocalDoubleRegisterValue(
                            ConvertRegNumToCorDebugRegister(pJITInfo->loc.vlRegReg.vlrrReg2),
                            ConvertRegNumToCorDebugRegister(pJITInfo->loc.vlRegReg.vlrrReg1),
                            type, ppValue);
        break;

    case ICorJitInfo::VLT_REG_STK:
        {
            CORDB_ADDRESS pRemoteValue = m_nativeFrame->GetLSStackAddress(
                pJITInfo->loc.vlRegStk.vlrsStk.vlrssBaseReg, pJITInfo->loc.vlRegStk.vlrsStk.vlrssOffset);

            hr = m_nativeFrame->GetLocalMemoryRegisterValue(
                          pRemoteValue,
                          ConvertRegNumToCorDebugRegister(pJITInfo->loc.vlRegStk.vlrsReg),
                          type, ppValue);
        }
        break;

    case ICorJitInfo::VLT_STK_REG:
        {
            CORDB_ADDRESS pRemoteValue = m_nativeFrame->GetLSStackAddress(
                pJITInfo->loc.vlStkReg.vlsrStk.vlsrsBaseReg,  pJITInfo->loc.vlStkReg.vlsrStk.vlsrsOffset);

            hr = m_nativeFrame->GetLocalRegisterMemoryValue(
                          ConvertRegNumToCorDebugRegister(pJITInfo->loc.vlStkReg.vlsrReg),
                          pRemoteValue, type, ppValue);
        }
        break;

    case ICorJitInfo::VLT_STK2:
        {
            CORDB_ADDRESS pRemoteValue = m_nativeFrame->GetLSStackAddress(
                pJITInfo->loc.vlStk2.vls2BaseReg, pJITInfo->loc.vlStk2.vls2Offset);

            hr = m_nativeFrame->GetLocalMemoryValue(pRemoteValue,
                                                    type,
                                                    ppValue);
        }
        break;

    case ICorJitInfo::VLT_FPSTK:
        hr = E_NOTIMPL;
        break;

    case ICorJitInfo::VLT_FIXED_VA:
        if (m_sigBuf == NULL) //no var args info
            return CORDBG_E_IL_VAR_NOT_AVAILABLE;

        CORDB_ADDRESS pRemoteValue;


#ifdef _X86_ // STACK_GROWS_DOWN_ON_ARGS_WALK
        pRemoteValue = PTR_TO_CORDB_ADDRESS((BYTE*)m_rpFirstArg -
                                    pJITInfo->loc.vlFixedVarArg.vlfvOffset);
        // Remember to subtract out this amount
        pRemoteValue += sizeof(((CORINFO_VarArgInfo*)0)->argBytes);
#else // STACK_GROWS_UP_ON_ARGS_WALK
        pRemoteValue = PTR_TO_CORDB_ADDRESS((BYTE*)m_rpFirstArg +
                                    pJITInfo->loc.vlFixedVarArg.vlfvOffset);
#endif

        hr = m_nativeFrame->GetLocalMemoryValue(pRemoteValue,
                                                type,
                                                ppValue);

        break;


    default:
        _ASSERTE(!"Invalid locVarType");
        hr = E_FAIL;
        break;
    }

    return hr;
}

HRESULT CordbJITILFrame::EnumerateLocalVariables(ICorDebugValueEnum **ppValueEnum)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValueEnum, ICorDebugValueEnum **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;

    CordbValueEnum *cdVE = new (nothrow) CordbValueEnum(m_nativeFrame, CordbValueEnum::LOCAL_VARS);

    if (cdVE == NULL)
    {
        (*ppValueEnum) = NULL;
        return E_OUTOFMEMORY;
    }

    // Initialize the new enum
    hr = cdVE->Init();

    if (SUCCEEDED(hr))
    {
        cdVE->ExternalAddRef();
        (*ppValueEnum) = static_cast<ICorDebugValueEnum*>(cdVE);
    }
    else
    {
        // Neuter the new object we've been working on.
        NeuterTicket ticket(GetProcess());
        cdVE->Neuter(ticket);

        (*ppValueEnum) = NULL;
        delete cdVE;
    }

    return hr;
}

HRESULT CordbJITILFrame::GetLocalVariable(DWORD dwIndex,
                                          ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    CordbFunction *pFunction = m_ilCode->m_function;
    ICorJitInfo::NativeVarInfo *pNativeInfo;

    //
    // First, make sure that we've got the jitted variable location data
    // loaded from the left side.
    //

    HRESULT hr = m_nativeFrame->m_jitinfo->LoadNativeInfo();

    if (SUCCEEDED(hr))
    {
        ULONG cArgs;
        if (m_fVarArgFnx && (m_sigBuf != NULL))
        {
            cArgs = m_argCount;
        }
        else
        {
            cArgs = m_nativeFrame->m_jitinfo->m_argumentCount;
        }

        hr = ILVariableToNative(dwIndex + cArgs, &pNativeInfo);

        if (SUCCEEDED(hr))
        {
            IfFailRet(this->LoadGenericArgs());

            // Get the type of this argument from the function
            CordbType *type;
            hr = pFunction->GetLocalVariableType(dwIndex,this->m_genericArgs,&type);

            if (SUCCEEDED(hr))
                hr = GetNativeVariable(type, pNativeInfo, ppValue);
        }
    }

    return hr;
}


HRESULT CordbJITILFrame::EnumerateArguments(ICorDebugValueEnum **ppValueEnum)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValueEnum, ICorDebugValueEnum **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;

    CordbValueEnum *cdVE = new (nothrow) CordbValueEnum(m_nativeFrame, CordbValueEnum::ARGS);

    if (cdVE == NULL)
    {
        (*ppValueEnum) = NULL;
        return E_OUTOFMEMORY;
    }

    // Initialize the new enum
    hr = cdVE->Init();

    if (SUCCEEDED(hr))
    {
        cdVE->ExternalAddRef();
        (*ppValueEnum) = static_cast<ICorDebugValueEnum*>(cdVE);
    }
    else
    {
        // Neuter the new object we've been working on.
        NeuterTicket ticket(GetProcess());
        cdVE->Neuter(ticket);

        (*ppValueEnum) = NULL;
        delete cdVE;
    }

    return hr;
}

HRESULT CordbJITILFrame::GetArgument(DWORD dwIndex, ICorDebugValue **ppValue)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    ICorJitInfo::NativeVarInfo *pNativeInfo;

    //
    // First, make sure that we've got the jitted variable location data
    // loaded from the left side.
    //
    HRESULT hr = m_nativeFrame->m_jitinfo->LoadNativeInfo();

    if (SUCCEEDED(hr))
    {
        hr = ILVariableToNative(dwIndex, &pNativeInfo);

        if (SUCCEEDED(hr))
        {
            // Get the type of this argument from the function
            CordbType *type;

            hr = GetArgumentType(dwIndex, &type);

            if (SUCCEEDED(hr))
                hr = GetNativeVariable(type, pNativeInfo, ppValue);
        }
    }

    return hr;
}


HRESULT CordbJITILFrame::GetStackDepth(ULONG32 *pDepth)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pDepth, ULONG32 *);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());


    /* !!! */

    return E_NOTIMPL;
}

HRESULT CordbJITILFrame::GetStackValue(DWORD dwIndex, ICorDebugValue **ppValue)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    /* !!! */

    return E_NOTIMPL;
}

// Called in EnC when at RemapOpportunity callback.
HRESULT CordbJITILFrame::RemapFunction(ULONG32 nOffset)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    return E_NOTIMPL;

    // Can only be called on leaf frame.
    if (!m_nativeFrame->IsLeafFrame())
    {
        return E_INVALIDARG;
    }


    // mark frames as not fresh, because this frame has been updated.
    m_nativeFrame->m_thread->m_framesFresh = FALSE;

    // Since we may have overwritten anything (objects, code, etc), we should mark
    // everything as needing to be re-cached.
    m_nativeFrame->m_thread->GetProcess()->m_continueCounter++;

    return m_nativeFrame->m_thread->SetRemapIP(nOffset);
}





/* ------------------------------------------------------------------------- *
 * Eval class
 * ------------------------------------------------------------------------- */

CordbEval::CordbEval(CordbThread *pThread)
    : CordbBase(pThread->GetProcess(), 0, enumCordbEval),
      m_thread(pThread), // implicit InternalAddRef
      m_function(NULL),
      m_complete(false),
      m_successful(false),
      m_aborted(false),
      m_resultAddr(NULL),
      m_evalDuringException(false)
{
    m_objectHandle.Set(NULL);
    m_debuggerEvalKey = LSPTR_DEBUGGEREVAL::NullPtr();

    m_resultType.elementType = ELEMENT_TYPE_VOID;
    m_resultAppDomainToken.Set(NULL);


#ifdef _DEBUG
    // Remember what AD we started in so that we can check that we finish there too.
    m_DbgAppDomainStarted = m_thread->GetAppDomain();
#endif

    // Place ourselves on the processes neuter-list.
    GetProcess()->AddToNeuterOnExitList(this);
}

CordbEval::~CordbEval()
{
    _ASSERTE(IsNeutered());
}

// We place ourselves on a neuter list. This gets called when the neuterlist sweeps.
void CordbEval::Neuter(NeuterTicket ticket)
{
    SendCleanup();
    m_thread.Assign(NULL);

    CordbBase::Neuter(ticket);
}

HRESULT CordbEval::SendCleanup(void)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    HRESULT hr = S_OK;

    // Send a message to the left side to release the eval object over
    // there if one exists.
    if ((m_debuggerEvalKey != NULL) &&
        GetProcess()->IsSafeToSendEvents())
    {
        // Call Abort() before doing new CallFunction()
        if (!m_complete)
            return CORDBG_E_FUNC_EVAL_NOT_COMPLETE;

        // Release the left side handle to the object
        DebuggerIPCEvent event;

        GetProcess()->InitIPCEvent(
                                &event,
                                DB_IPCE_FUNC_EVAL_CLEANUP,
                                true,
                                m_thread->GetAppDomain()->GetADToken());

        event.FuncEvalCleanup.debuggerEvalKey = m_debuggerEvalKey;

        hr = GetProcess()->SendIPCEvent(&event,
                                                  sizeof(DebuggerIPCEvent));

#if _DEBUG
        if (SUCCEEDED(hr))
            _ASSERTE(event.type == DB_IPCE_FUNC_EVAL_CLEANUP_RESULT);
#endif

        // Null out the key so we don't try to do this again.
        m_debuggerEvalKey = LSPTR_DEBUGGEREVAL::NullPtr();

        hr = event.hr;
    }

    m_pHandleValue.Clear();


    return hr;
}

HRESULT CordbEval::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugEval)
    {
        *pInterface = static_cast<ICorDebugEval*>(this);
    }
    else if (id == IID_ICorDebugEval2)
    {
        *pInterface = static_cast<ICorDebugEval2*>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugEval*>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

//
// Gather data about an argument to either CallFunction or NewObject
// and place it into a DebuggerIPCE_FuncEvalArgData struct for passing
// to the Left Side.
//
HRESULT CordbEval::GatherArgInfo(ICorDebugValue *pValue,
                                 DebuggerIPCE_FuncEvalArgData *argData)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //

    HRESULT hr;
    CORDB_ADDRESS addr;
    CorElementType ty;
    bool needRelease = false;

    pValue->GetType(&ty);

    // Note: if the value passed in is in fact a byref, then we need to dereference it to get to the real thing. Passing
    // a byref as a byref to a func eval is never right.
    if ((ty == ELEMENT_TYPE_BYREF) || (ty == ELEMENT_TYPE_TYPEDBYREF))
    {
        ICorDebugReferenceValue *prv = NULL;

        IfFailRet(pValue->QueryInterface(IID_ICorDebugReferenceValue, (void**)&prv));

        // This really should always work for a byref, unless we're out of memory.
        hr = prv->Dereference(&pValue);
        prv->Release();

        IfFailRet(hr);

        // Make sure to get the type we were referencing for use below.
        pValue->GetType(&ty);
        needRelease = true;
    }

    // We should never have a byref by this point.
    _ASSERTE((ty != ELEMENT_TYPE_BYREF) && (ty != ELEMENT_TYPE_TYPEDBYREF));

    pValue->GetAddress(&addr);

    argData->argAddr = CORDB_ADDRESS_TO_PTR(addr);
    argData->argElementType = ty;

    argData->argIsHandleValue = false;
    argData->argIsLiteral = false;
    argData->fullArgType = NULL;
    argData->fullArgTypeNodeCount = 0;

    // We have to have knowledge of our value implementation here,
    // which it would nice if we didn't have to know.
    CordbValue *cv = NULL;

    switch(ty)
    {

    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_STRING:
    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_ARRAY:
    case ELEMENT_TYPE_SZARRAY:
        {
            ICorDebugHandleValue *pHandle = NULL;
            pValue->QueryInterface(IID_ICorDebugHandleValue, (void **) &pHandle);
            if (pHandle == NULL)
            {
                // A reference value
                cv = static_cast<CordbValue*> (static_cast<CordbReferenceValue*> (pValue));
                argData->argIsHandleValue = !(((CordbReferenceValue*)pValue)->m_objectHandle.IsNull());

                // Is this a literal value? If, we'll copy the data to the
                // buffer area so the left side can get it.
                CordbReferenceValue *rv;
                rv = static_cast<CordbReferenceValue*>(pValue);
                argData->argIsLiteral = rv->CopyLiteralData(argData->argLiteralData);
            }
            else
            {
                argData->argIsHandleValue = true;
                argData->argIsLiteral = false;
                pHandle->Release();
            }
        }
        break;

    case ELEMENT_TYPE_VALUETYPE:  // OK: this E_T_VALUETYPE comes ICorDebugValue::GetType

        // A value class object
        cv = static_cast<CordbValue*> (static_cast<CordbVCObjectValue*>(static_cast<ICorDebugObjectValue*> (pValue)));

        // The EE does not guarantee to have exact type information
        // available for all struct types, so we indicate the type by using a
        // DebuggerIPCE_TypeArgData serialization of a type.
        //
        // At the moment the LHS only cares about this data
        // when boxing the "this" pointer.
        {
            unsigned int fullArgTypeNodeCount = 0;
            CordbType::CountTypeDataNodes(cv->m_type,&fullArgTypeNodeCount);

            _ASSERTE(fullArgTypeNodeCount > 0);
            unsigned int bufferSize = sizeof(DebuggerIPCE_TypeArgData) * fullArgTypeNodeCount;
            DebuggerIPCE_TypeArgData *bufferFrom = (DebuggerIPCE_TypeArgData *) _alloca(bufferSize);

            DebuggerIPCE_TypeArgData *curr = bufferFrom;
            CordbType::GatherTypeData(cv->m_type, &curr);

            void *buffer = NULL;
            IfFailRet(m_thread->GetProcess()->GetAndWriteRemoteBuffer(m_thread->GetAppDomain(), bufferSize, bufferFrom, &buffer));

            argData->fullArgType = buffer;
            argData->fullArgTypeNodeCount = fullArgTypeNodeCount;
        }
        break;

    default:

        // A generic value
        cv = static_cast<CordbValue*> (static_cast<CordbGenericValue*> (pValue));

        // Is this a literal value? If, we'll copy the data to the
        // buffer area so the left side can get it.
        CordbGenericValue *gv = (CordbGenericValue*)pValue;
        argData->argIsLiteral = gv->CopyLiteralData(argData->argLiteralData);

        break;
    }

    // Is it enregistered?
    if (addr == NULL)
        cv->GetRegisterInfo(argData);

    // Release pValue if we got it via a dereference from above.
    if (needRelease)
        pValue->Release();

    return S_OK;
}


HRESULT CordbEval::SendFuncEval(unsigned int genericArgsCount,
                                ICorDebugType *genericArgs[],
                                void *argData1, unsigned int argData1Size,
                                void *argData2, unsigned int argData2Size,
                                DebuggerIPCEvent * event)
{
    FAIL_IF_NEUTERED(this);
    INTERNAL_SYNC_API_ENTRY(GetProcess()); //
    unsigned int genericArgsNodeCount = 0;

    DebuggerIPCE_TypeArgData *tyargData = NULL;
    CordbType::CountTypeDataNodesForInstantiation(genericArgsCount,genericArgs,&genericArgsNodeCount);

    unsigned int tyargDataSize = sizeof(DebuggerIPCE_TypeArgData) * genericArgsNodeCount;

    if (genericArgsNodeCount > 0)
    {
        tyargData = new (nothrow) DebuggerIPCE_TypeArgData[genericArgsNodeCount];
        if (tyargData == NULL)
        {
            return E_OUTOFMEMORY;
        }

        DebuggerIPCE_TypeArgData *curr_tyargData = tyargData;
        CordbType::GatherTypeDataForInstantiation(genericArgsCount, genericArgs, &curr_tyargData);

    }
    event->FuncEval.genericArgsNodeCount = genericArgsNodeCount;


    // Are we doing an eval during an exception? If so, we need to remember
    // that over here and also tell the Left Side.
    m_evalDuringException = event->FuncEval.evalDuringException = m_thread->HasException();
    m_ThreadOldExceptionHandle = m_thread->GetThreadExceptionRawObjectHandle();

    // Corresponding Release() on DB_IPCE_FUNC_EVAL_COMPLETE.
    // If a func eval is aborted, the LHS may not complete the abort
    // immediately and hence we cant do a SendCleanup(). Hence, we maintain
    // an extra ref-count to determine when this can be done.
    AddRef();

    HRESULT hr = m_thread->GetProcess()->SendIPCEvent(event, sizeof(DebuggerIPCEvent));

    // If the send failed, return that failure.
    if (FAILED(hr))
        goto LExit;

    _ASSERTE(event->type == DB_IPCE_FUNC_EVAL_SETUP_RESULT);

    hr = event->hr;

    // Memory has been allocated to hold info about each argument on
    // the left side now, so copy the argument data over to the left
    // side. No need to send another event, since the left side won't
    // take any more action on this evaluation until the process is
    // continued anyway.
    //
    // The type arguments come first, followed by up to two blobs of data
    // for other arguments.
    if (SUCCEEDED(hr))
    {
        BYTE *argdata = event->FuncEvalSetupComplete.argDataArea;

        if (SUCCEEDED(hr) && (tyargData != NULL) && (tyargDataSize != 0))
        {
            if (!WriteProcessMemory(m_thread->GetProcess()->m_handle,
                                    argdata,
                                    tyargData,
                                    tyargDataSize,
                                    NULL))
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
            argdata += tyargDataSize;
        }

        if (SUCCEEDED(hr) && (argData1 != NULL) && (argData1Size != 0))
        {
            if (!WriteProcessMemory(m_thread->GetProcess()->m_handle,
                                    argdata,
                                    argData1,
                                    argData1Size,
                                    NULL))
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
            argdata += argData1Size;
        }

        if (SUCCEEDED(hr) && (argData2 != NULL) && (argData2Size != 0))
        {
            if (!WriteProcessMemory(m_thread->GetProcess()->m_handle,
                                    argdata,
                                    argData2,
                                    argData2Size,
                                    NULL))
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
             argdata += argData2Size;
        }
    }

LExit:
    if (tyargData)
    {
        delete [] tyargData;
    }

    // Save the key to the eval on the left side for future reference.
    if (SUCCEEDED(hr))
    {
        m_debuggerEvalKey = event->FuncEvalSetupComplete.debuggerEvalKey;
        m_thread->GetProcess()->IncrementOutstandingEvalCount();
    }
    else
    {
        // We dont expect to receive a DB_IPCE_FUNC_EVAL_COMPLETE, so just release here
        Release();
    }

    return hr;
}


// Get the AppDomain that an object lives in.
// This does not adjust any reference counts.
// Returns NULL if we can't determine the appdomain, or if the value is known to be agile.
CordbAppDomain * GetAppDomainFromValue(ICorDebugValue * pValue)
{

    {
        RSExtSmartPtr<ICorDebugHandleValue> handleP;
        pValue->QueryInterface(IID_ICorDebugHandleValue, (void**)&handleP);
        if (handleP != NULL)
        {
            CordbHandleValue * chp = static_cast<CordbHandleValue *> (handleP.GetValue());
            return chp->GetAppDomain();
        }
    }

    {
        RSExtSmartPtr<ICorDebugReferenceValue> refP;
        pValue->QueryInterface(IID_ICorDebugReferenceValue, (void**)&refP);
        if (refP != NULL)
        {
            CordbReferenceValue * crp = static_cast<CordbReferenceValue *> (refP.GetValue());
            return crp->GetAppDomain();
        }
    }

    {
        RSExtSmartPtr<ICorDebugObjectValue> objP;
        pValue->QueryInterface(IID_ICorDebugObjectValue, (void**)&objP);
        if (objP != NULL)
        {
            CordbVCObjectValue * crp = static_cast<CordbVCObjectValue*> (objP.GetValue());
            return crp->GetAppDomain();
        }
    }

    // Assume nothing else has AD affinity.
    return NULL;
}

HRESULT CordbEval::CallFunction(ICorDebugFunction *pFunction,
                                ULONG32 nArgs,
                                ICorDebugValue *pArgs[])
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    return  CallParameterizedFunction(pFunction,0,NULL,nArgs,pArgs);
}

//-----------------------------------------------------------------------------
// See if we can convert general Func-eval failure HRs (which are usually based on EE-invariants that
// may be meaningless to the user) into a more specific user-friendly hr.
// Doing the conversions here in the RS (instead of in the LS) makes it more clear that these
// HRs definitely map to concepts described by the ICorDebugAPI instead of EE-invariants.
// It also lets us clearly prioritize the HRs in case of ambiguity.
//-----------------------------------------------------------------------------
HRESULT CordbEval::FilterHR(HRESULT hr)
{
    // Currently, we only make CORDBG_E_ILLEGAL_AT_GC_UNSAFE_POINT more specific.
    // If it's not that HR, then shortcut our work.
    if (hr != CORDBG_E_ILLEGAL_AT_GC_UNSAFE_POINT)
    {
        return hr;
    }

    // In the case of conflicting HRs (if the func-eval fails for multiple reasons),
    // we'll try to give priority to the more general HR.
    // This communicates the quickest action for the user to be able to get to a
    // func-eval friendly spot. It also means less churn in the hrs we return
    // because specific hrs are more likely to change than general ones.

    // If we got CORDBG_E_ILLEGAL_AT_GC_UNSAFE_POINT, check the common reasons.
    // We'll use the Right-Side's intimate knowledge of the Left-Side to guess _why_
    // it's a GC-unsafe spot, and then we'll communicate that back w/ a more meaningful HR.
    // If GC safe-spots change, then these errors should be updated.


    // Most likely is if we're in native code. Check that first.
    _ASSERTE(!m_thread->m_stackChains.IsEmpty());

    CordbChain * pLeafChain = m_thread->m_stackChains[0];
    _ASSERTE(pLeafChain != NULL );

    if (!pLeafChain->m_managed)
    {
        return CORDBG_E_ILLEGAL_IN_NATIVE_CODE;
    }

    // Next, check if we're in optimized code.
    // Optimized code doesn't directly mean that func-evals are illegal; but it greatly
    // increases the odds of being at a GC-unsafe point.
    // We give this failure higher precedence than the "Is in prolog" failure.

    HRESULT hrTemp = S_OK;
    CordbFrame * pFrame = m_thread->GetActiveFrame();
    if (pFrame != NULL)
    {
       CordbNativeFrame * pNativeFrame = pFrame->GetAsNativeFrame();
       if (pNativeFrame != NULL)
       {
            CordbCode * pCode = pNativeFrame->GetCode();
            if (pCode != NULL)
            {
                DWORD flags;
                hrTemp = pCode->GetCompilerFlagsInternal(&flags);

                if (SUCCEEDED(hrTemp))
                {
                    if ((flags & CORDEBUG_JIT_DISABLE_OPTIMIZATION) != CORDEBUG_JIT_DISABLE_OPTIMIZATION)
                    {
                        return CORDBG_E_ILLEGAL_IN_OPTIMIZED_CODE;
                    }

                } // GetCompilerFlags
            } // Code

            CordbJITILFrame * pILFrame = pNativeFrame->m_JITILFrame;
            if (pILFrame != NULL)
            {
                if (pILFrame->m_mapping == MAPPING_PROLOG)
                {
                    return CORDBG_E_ILLEGAL_IN_PROLOG;
                }
            }
       } // Native Frame
    }

    // No filtering.
    return hr;

}

HRESULT CordbEval::CallParameterizedFunction(ICorDebugFunction *pFunction,
                                             ULONG32 nTypeArgs,
                                             ICorDebugType *ppTypeArgs[],
                                             ULONG32 nArgs,
                                             ICorDebugValue *pArgs[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    VALIDATE_POINTER_TO_OBJECT(pFunction, ICorDebugFunction *);
    if (nArgs > 0)
    {
        VALIDATE_POINTER_TO_OBJECT_ARRAY(pArgs, ICorDebugValue *, nArgs, true, true);
    }
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // The LS will assume that all of the ICorDebugValues and ICorDebugTypes are in
    // the same appdomain as the function.  Verify this.
    CordbAppDomain * pMethodAppDomain = (static_cast<CordbFunction*> (pFunction))->GetAppDomain();
    if( !DoAppDomainsMatch( pMethodAppDomain, nTypeArgs, ppTypeArgs, nArgs, pArgs ) ) {
        return ErrWrapper(CORDBG_E_APPDOMAIN_MISMATCH);
    }

    // Callers are free to reuse an ICorDebugEval object for multiple
    // evals. Since we create a Left Side eval representation each
    // time, we need to be sure to clean it up now that we know we're
    // done with it.
    HRESULT hr = SendCleanup();

    if (FAILED(hr))
        return hr;


    RsPtrHolder<CordbEval> hFuncEval(this);
    if (hFuncEval.Ptr().IsNull())
    {
        return E_OUTOFMEMORY;
    }


    // Remember the function that we're evaluating.
    m_function = (CordbFunction*)pFunction;
    m_evalType = DB_IPCE_FET_NORMAL;


    DebuggerIPCE_FuncEvalArgData *argData = NULL;

    if (nArgs > 0)
    {
        // We need to make the same type of array that the left side
        // holds.
        argData = new (nothrow) DebuggerIPCE_FuncEvalArgData[nArgs];

        if (argData == NULL)
            return E_OUTOFMEMORY;

        // For each argument, convert its home into something the left
        // side can understand.
        for (unsigned int i = 0; i < nArgs; i++)
        {
            hr = GatherArgInfo(pArgs[i], &(argData[i]));

            if (FAILED(hr))
            {
                delete [] argData;
                    return hr;
            }
        }
    }

    // Send over to the left side and get it to setup this eval.
    DebuggerIPCEvent event;
    m_thread->GetProcess()->InitIPCEvent(&event,
                                         DB_IPCE_FUNC_EVAL,
                                         true,
                                         m_thread->GetAppDomain()->GetADToken());
    event.FuncEval.funcDebuggerThreadToken = m_thread->m_debuggerThreadToken;
    event.FuncEval.funcEvalType = m_evalType;
    event.FuncEval.funcMetadataToken = m_function->m_token;
    event.FuncEval.funcDebuggerModuleToken = m_function->GetModule()->m_debuggerModuleToken;
    event.FuncEval.funcEvalKey = hFuncEval.Ptr();
    event.FuncEval.argCount = nArgs;
    event.FuncEval.genericArgsCount = nTypeArgs;

    hr = SendFuncEval(nTypeArgs,ppTypeArgs,
                      (void *)argData,
                      sizeof(DebuggerIPCE_FuncEvalArgData) * nArgs,
                      NULL,0,
                      &event);

    // Convert from LS EE-centric failure code to something more friendly to end-users.
    hr = FilterHR(hr);

    // Cleanup

    if (argData)
        delete [] argData;


    if (SUCCEEDED(hr))
    {
        hFuncEval.SuppressRelease(); // Now LS owns.
    }
    // Return any failure the Left Side may have told us about.
    return hr;
}

BOOL CordbEval::DoAppDomainsMatch( CordbAppDomain * pAppDomain,
                                            ULONG32 nTypes,
                                             ICorDebugType *pTypes[],
                                            ULONG32 nValues,
                                            ICorDebugValue *pValues[] )
{
    _ASSERTE( !(pTypes == NULL && nTypes != 0) );
    _ASSERTE( !(pValues == NULL && nValues != 0) );

    // Make sure each value is in the appdomain.
    for(unsigned int i = 0; i < nValues; i++)
    {
        // Assuming that only Ref Values have AD affinity
        CordbAppDomain * pValueAppDomain = GetAppDomainFromValue( pValues[i] );

        if ((pValueAppDomain != NULL) && (pValueAppDomain != pAppDomain))
        {
            LOG((LF_CORDB,LL_INFO1000, "CordbEval::DADM - AD mismatch. appDomain=0x%08x, param #%d=0x%08x, must fail.\n",
                pAppDomain, i, pValueAppDomain));
            return FALSE;
        }
    }

    for(unsigned int i = 0; i < nTypes; i++ )
    {
        CordbType* t = static_cast<CordbType*>( pTypes[i] );
        CordbAppDomain * pTypeAppDomain = t->GetAppDomain();

        if( pTypeAppDomain != NULL && pTypeAppDomain != pAppDomain )
        {
            LOG((LF_CORDB,LL_INFO1000, "CordbEval::DADM - AD mismatch. appDomain=0x%08x, type param #%d=0x%08x, must fail.\n",
                pAppDomain, i, pTypeAppDomain));
            return FALSE;
        }
    }

    return TRUE;
}

HRESULT CordbEval::NewObject(ICorDebugFunction *pConstructor,
                             ULONG32 nArgs,
                             ICorDebugValue *pArgs[])
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    return NewParameterizedObject(pConstructor,0,NULL,nArgs,pArgs);
}

HRESULT CordbEval::NewParameterizedObject(ICorDebugFunction *pConstructor,
                                          ULONG32 nTypeArgs,
                                          ICorDebugType *ppTypeArgs[],
                                          ULONG32 nArgs,
                                          ICorDebugValue *pArgs[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pConstructor, ICorDebugFunction *);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(pArgs, ICorDebugValue *, nArgs, true, true);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // The LS will assume that all of the ICorDebugValues and ICorDebugTypes are in
    // the same appdomain as the constructor.  Verify this.
    CordbAppDomain * pConstructorAppDomain = (static_cast<CordbFunction*> (pConstructor))->GetAppDomain();
    if( !DoAppDomainsMatch( pConstructorAppDomain, nTypeArgs, ppTypeArgs, nArgs, pArgs ) ) {
        return ErrWrapper(CORDBG_E_APPDOMAIN_MISMATCH);
    }

    // Callers are free to reuse an ICorDebugEval object for multiple
    // evals. Since we create a Left Side eval representation each
    // time, we need to be sure to clean it up now that we know we're
    // done with it.
    HRESULT hr = SendCleanup();

    if (FAILED(hr))
        return hr;

    RsPtrHolder<CordbEval> hFuncEval(this);
    if (hFuncEval.Ptr().IsNull())
    {
        return E_OUTOFMEMORY;
    }

    // Remember the function that we're evaluating.
    m_function = (CordbFunction*)pConstructor;
    m_evalType = DB_IPCE_FET_NEW_OBJECT;

    DebuggerIPCE_FuncEvalArgData *argData = NULL;

    if (nArgs > 0)
    {
        // We need to make the same type of array that the left side
        // holds.
        argData = new (nothrow) DebuggerIPCE_FuncEvalArgData[nArgs];

        if (argData == NULL)
            return E_OUTOFMEMORY;

        // For each argument, convert its home into something the left
        // side can understand.
        for (unsigned int i = 0; i < nArgs; i++)
        {
            hr = GatherArgInfo(pArgs[i], &(argData[i]));

            if (FAILED(hr))
                return hr;
        }
    }

    // Send over to the left side and get it to setup this eval.
    DebuggerIPCEvent event;
    m_thread->GetProcess()->InitIPCEvent(&event,
                                         DB_IPCE_FUNC_EVAL,
                                         true,
                                         m_thread->GetAppDomain()->GetADToken());
    event.FuncEval.funcDebuggerThreadToken = m_thread->m_debuggerThreadToken;
    event.FuncEval.funcEvalType = m_evalType;
    event.FuncEval.funcMetadataToken = m_function->m_token;
    event.FuncEval.funcDebuggerModuleToken = m_function->GetModule()->m_debuggerModuleToken;
    event.FuncEval.funcEvalKey = hFuncEval.Ptr();
    event.FuncEval.argCount = nArgs;
    event.FuncEval.genericArgsCount = nTypeArgs;

    hr = SendFuncEval(nTypeArgs,ppTypeArgs,
                      (void *)argData,
                      sizeof(DebuggerIPCE_FuncEvalArgData) * nArgs,
                      NULL,0,
                      &event);

    // Cleanup

    if (argData)
        delete [] argData;

    if (SUCCEEDED(hr))
    {
        hFuncEval.SuppressRelease(); // Now LS owns.
    }


    // Return any failure the Left Side may have told us about.
    return hr;
}

HRESULT CordbEval::NewObjectNoConstructor(ICorDebugClass *pClass)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    return NewParameterizedObjectNoConstructor(pClass,0,NULL);
}

HRESULT CordbEval::NewParameterizedObjectNoConstructor(ICorDebugClass *pClass,
                                                       ULONG32 nTypeArgs,
                                                       ICorDebugType *ppTypeArgs[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pClass, ICorDebugClass *);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // The LS will assume that all of the ICorDebugTypes are in
    // the same appdomain as the class.  Verify this.
    CordbAppDomain * pClassAppDomain = (static_cast<CordbClass*> (pClass))->GetAppDomain();
    if( !DoAppDomainsMatch( pClassAppDomain, nTypeArgs, ppTypeArgs, 0, NULL ) ) {
        return ErrWrapper(CORDBG_E_APPDOMAIN_MISMATCH);
    }

    // Callers are free to reuse an ICorDebugEval object for multiple
    // evals. Since we create a Left Side eval representation each
    // time, we need to be sure to clean it up now that we know we're
    // done with it.
    HRESULT hr = SendCleanup();

    if (FAILED(hr))
        return hr;

    RsPtrHolder<CordbEval> hFuncEval(this);
    if (hFuncEval.Ptr().IsNull())
    {
        return E_OUTOFMEMORY;
    }

    // Remember the function that we're evaluating.
    m_class = (CordbClass*)pClass;
    m_evalType = DB_IPCE_FET_NEW_OBJECT_NC;

    // Send over to the left side and get it to setup this eval.
    DebuggerIPCEvent event;
    m_thread->GetProcess()->InitIPCEvent(&event,
                                         DB_IPCE_FUNC_EVAL,
                                         true,
                                         m_thread->GetAppDomain()->GetADToken());
    event.FuncEval.funcDebuggerThreadToken = m_thread->m_debuggerThreadToken;
    event.FuncEval.funcEvalType = m_evalType;
    event.FuncEval.funcMetadataToken = mdMethodDefNil;
    event.FuncEval.funcClassMetadataToken = (mdTypeDef)m_class->m_id;
    event.FuncEval.funcDebuggerModuleToken = m_class->GetModule()->m_debuggerModuleToken;
    event.FuncEval.funcEvalKey = hFuncEval.Ptr();
    event.FuncEval.argCount = 0;
    event.FuncEval.genericArgsCount = nTypeArgs;

    hr = SendFuncEval(nTypeArgs,ppTypeArgs,NULL,0,NULL,0,&event);

    if (SUCCEEDED(hr))
    {
        hFuncEval.SuppressRelease(); // Now LS owns.
    }

    // Return any failure the Left Side may have told us about.
    return hr;
}

/*
 *
 * NewString
 *
 *  This routine is the interface function for ICorDebugEval::NewString
 *
 * Parameters:
 *  string - the string to create - must be null-terminated
 *
 * Return Value:
 *  HRESULT from the helper routines on RS and LS.
 *
 */
HRESULT CordbEval::NewString(LPCWSTR string)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    return NewStringWithLength(string, (UINT)wcslen(string));
}

/*
 *
 * NewStringWithLength
 *
 *  This routine is the interface function for ICorDebugEval::NewStringWithLength.
 *
 * Parameters:
 *  string - the string to create
 *  uiLength - the number of characters that you want to create. Can include embedded nulls.
 *
 * Return Value:
 *  HRESULT from the helper routines on RS and LS.
 *
 */

HRESULT CordbEval::NewStringWithLength(LPCWSTR string, UINT uiLength)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(string, LPCWSTR); // Gotta have a string...
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // Callers are free to reuse an ICorDebugEval object for multiple
    // evals. Since we create a Left Side eval representation each
    // time, we need to be sure to clean it up now that we know we're
    // done with it.
    HRESULT hr = SendCleanup();

    if (FAILED(hr))
        return hr;

    RsPtrHolder<CordbEval> hFuncEval(this);
    if (hFuncEval.Ptr().IsNull())
    {
        return E_OUTOFMEMORY;
    }


    // Length of the string? Don't account for null as COMString::NewString is length-based
    SIZE_T strSize = uiLength * sizeof(WCHAR);

    // Remember that we're doing a func eval for a new string.
    m_function = NULL;
    m_evalType = DB_IPCE_FET_NEW_STRING;

    // Send over to the left side and get it to setup this eval.
    DebuggerIPCEvent event;
    m_thread->GetProcess()->InitIPCEvent(&event,
                                         DB_IPCE_FUNC_EVAL,
                                         true,
                                         m_thread->GetAppDomain()->GetADToken());
    event.FuncEval.funcDebuggerThreadToken = m_thread->m_debuggerThreadToken;
    event.FuncEval.funcEvalType = m_evalType;
    event.FuncEval.funcEvalKey = hFuncEval.Ptr();
    event.FuncEval.stringSize = strSize;

    // Note: no function or module here...
    event.FuncEval.funcMetadataToken = mdMethodDefNil;
    event.FuncEval.funcClassMetadataToken = mdTypeDefNil;
    event.FuncEval.funcDebuggerModuleToken.Set(NULL);
    event.FuncEval.argCount = 0;
    event.FuncEval.genericArgsCount = 0;
    event.FuncEval.genericArgsNodeCount = 0;

    hr = SendFuncEval(0,NULL,(void*)string,(unsigned int)strSize,NULL,0,&event);

    if (SUCCEEDED(hr))
    {
        hFuncEval.SuppressRelease(); // Now LS owns.
    }

    // Return any failure the Left Side may have told us about.
    return hr;
}

HRESULT CordbEval::NewArray(CorElementType elementType,
                            ICorDebugClass *pElementClass,
                            ULONG32 rank,
                            ULONG32 dims[],
                            ULONG32 lowBounds[])
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pElementClass, ICorDebugClass *);

    // If you want a class, you gotta pass a class.
    if ((elementType == ELEMENT_TYPE_CLASS) && (pElementClass == NULL))
        return E_INVALIDARG;

    // If you want an array of objects, then why pass a class?
    if ((elementType == ELEMENT_TYPE_OBJECT) && (pElementClass != NULL))
        return E_INVALIDARG;

    // Arg check...
    if (elementType == ELEMENT_TYPE_VOID)
        return E_INVALIDARG;

  CordbType *typ;
  HRESULT hr = S_OK;
  hr = CordbType::MkUnparameterizedType(m_thread->GetAppDomain(), elementType, (CordbClass *) pElementClass, &typ);

  if (FAILED(hr))
    return hr;

  return NewParameterizedArray(typ, rank,dims,lowBounds);

}


HRESULT CordbEval::NewParameterizedArray(ICorDebugType *pElementType,
                                         ULONG32 rank,
                                         ULONG32 dims[],
                                         ULONG32 lowBounds[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pElementType, ICorDebugType *);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());


    // Callers are free to reuse an ICorDebugEval object for multiple evals. Since we create a Left Side eval
    // representation each time, we need to be sure to clean it up now that we know we're done with it.
    HRESULT hr = SendCleanup();

    if (FAILED(hr))
        return hr;

    // Arg check...
    if ((rank == 0) || (dims == NULL))
        return E_INVALIDARG;

    RsPtrHolder<CordbEval> hFuncEval(this);
    if (hFuncEval.Ptr().IsNull())
    {
        return E_OUTOFMEMORY;
    }


    // Remember that we're doing a func eval for a new string.
    m_function = NULL;
    m_evalType = DB_IPCE_FET_NEW_ARRAY;

    // Send over to the left side and get it to setup this eval.
    DebuggerIPCEvent event;
    m_thread->GetProcess()->InitIPCEvent(&event,
                                         DB_IPCE_FUNC_EVAL,
                                         true,
                                         m_thread->GetAppDomain()->GetADToken());
    event.FuncEval.funcDebuggerThreadToken = m_thread->m_debuggerThreadToken;
    event.FuncEval.funcEvalType = m_evalType;
    event.FuncEval.funcEvalKey = hFuncEval.Ptr();

    event.FuncEval.arrayRank = rank;

    // Note: no function or module here...
    event.FuncEval.funcMetadataToken = mdMethodDefNil;
    event.FuncEval.funcClassMetadataToken = mdTypeDefNil;
    event.FuncEval.funcDebuggerModuleToken.Set(NULL);
    event.FuncEval.argCount = 0;
    event.FuncEval.genericArgsCount = 1;

    // Prefast overflow sanity check.
    S_UINT32 allocSize = S_UINT32( rank ) * S_UINT32( sizeof(SIZE_T) );
    if( allocSize.IsOverflow() )
    {
        return E_INVALIDARG;
    }

    // Just in case sizeof(SIZE_T) != sizeof(ULONG32)
    SIZE_T *dimsSIZE_T = (SIZE_T *) _alloca( allocSize.Value() );
    for (unsigned int i = 0; i < rank; i++)
      dimsSIZE_T[i] = dims[i];
    ICorDebugType *genericArgs[1];
    genericArgs[0] = pElementType;

    hr = SendFuncEval(1,genericArgs,
                      (void*)dimsSIZE_T,
                      rank * sizeof(SIZE_T),
                      NULL, // (void*)lowBounds,
                      0, // ((lowBounds == NULL) ? 0 : rank * sizeof(SIZE_T)),
                      &event);

    if (SUCCEEDED(hr))
    {
        hFuncEval.SuppressRelease(); // Now LS owns.
    }

    // Return any failure the Left Side may have told us about.
    return hr;
}

HRESULT CordbEval::IsActive(BOOL *pbActive)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pbActive, BOOL *);

    *pbActive = (m_complete == true);
    return S_OK;
}

/*
 * This routine submits an abort request to the LS.
 *
 * Parameters:
 *     None.
 *
 * Returns:
 *     The HRESULT as returned by the LS.
 *
 */

HRESULT
CordbEval::Abort(
    void
    )
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    ATT_ALLOW_LIVE_DO_STOPGO(GetProcess());


    //
    // No need to abort if its already completed.
    //
    if (m_complete)
    {
        return S_OK;
    }


    //
    // Can't abort if its never even been started.
    //
    if (m_debuggerEvalKey == NULL)
    {
        return E_INVALIDARG;
    }

    CORDBRequireProcessStateOK(m_thread->GetProcess());

    //
    // Send over to the left side to get the eval aborted.
    //
    DebuggerIPCEvent event;

    m_thread->GetProcess()->InitIPCEvent(&event,
                                         DB_IPCE_FUNC_EVAL_ABORT,
                                         true,
                                         m_thread->GetAppDomain()->GetADToken()
                                        );

    event.FuncEvalAbort.debuggerEvalKey = m_debuggerEvalKey;

    HRESULT hr = m_thread->GetProcess()->SendIPCEvent(&event,
                                                      sizeof(DebuggerIPCEvent)
                                                     );


    //
    // If the send failed, return that failure.
    //
    if (FAILED(hr))
    {
        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_FUNC_EVAL_ABORT_RESULT);

    //
    // Since we may have
    // overwritten anything (objects, code, etc), we should mark
    // everything as needing to be re-cached.
    //
    m_thread->GetProcess()->m_continueCounter++;

    hr = event.hr;

    return hr;
}

HRESULT CordbEval::GetResult(ICorDebugValue **ppResult)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppResult, ICorDebugValue **);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    // Is the evaluation complete?
    if (!m_complete)
        return CORDBG_E_FUNC_EVAL_NOT_COMPLETE;

    if (m_aborted)
        return CORDBG_S_FUNC_EVAL_ABORTED;

    // Does the evaluation have a result?
    if (m_resultType.elementType == ELEMENT_TYPE_VOID)
    {
        *ppResult = NULL;
        return CORDBG_S_FUNC_EVAL_HAS_NO_RESULT;
    }

    HRESULT hr = S_OK;

    // Make a ICorDebugValue out of the result.
    CordbAppDomain *appdomain;

    if (m_resultAppDomainToken != NULL)
    {
        appdomain = m_thread->GetProcess()->m_appDomains.GetBase(LsPtrToCookie(m_resultAppDomainToken));
        _ASSERTE(appdomain != NULL);
    }
    else
    {
        appdomain = m_thread->GetAppDomain();
    }
    PREFIX_ASSUME(appdomain != NULL);

    CordbType *typ = NULL;
    hr = CordbType::TypeDataToType(appdomain, &m_resultType, &typ);
    if (FAILED(hr))
        return hr;

    bool resultInHandle =
        ((m_resultType.elementType == ELEMENT_TYPE_CLASS) ||
        (m_resultType.elementType == ELEMENT_TYPE_SZARRAY) ||
        (m_resultType.elementType == ELEMENT_TYPE_OBJECT) ||
        (m_resultType.elementType == ELEMENT_TYPE_ARRAY) ||
        (m_resultType.elementType == ELEMENT_TYPE_STRING));

    if (resultInHandle)
    {
        _ASSERTE(m_objectHandle != NULL);

        if (m_pHandleValue == NULL)
        {
            // Create CordbHandleValue for result
           CordbHandleValue *chv = new (nothrow) CordbHandleValue(appdomain, typ, HANDLE_STRONG);

            if (chv == NULL)
            {
                return E_OUTOFMEMORY;
            }

            // Initialize the handle value object.
            hr = chv->Init(m_objectHandle);

            if (!SUCCEEDED(hr))
            {
                // Neuter the new object we've been working on. This will call Dispose(), and that will go back to the left side and free
                // the handle that we got above.
                NeuterTicket ticket(GetProcess());
                chv->Neuter(ticket);

                // Cleanup the memory and return.
                delete chv;
                return hr;
            }

            m_pHandleValue.Assign(chv);
        }

        // This AddRef is for caller to release
        *ppResult = m_pHandleValue;
        m_pHandleValue->ExternalAddRef();
    }
    else
    {
        // Now that we have the module, go ahead and create the result.
        hr = CordbValue::CreateValueByType(appdomain,
                                           typ,
                                           true,
                                           m_resultAddr,
                                           NULL,
                                           NULL,
                                           (IUnknown*)(ICorDebugEval*)this,
                                           ppResult);
    }
    return hr;
}

HRESULT CordbEval::GetThread(ICorDebugThread **ppThread)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppThread, ICorDebugThread **);

    *ppThread = static_cast<ICorDebugThread*> (m_thread);
    m_thread->ExternalAddRef();

    return S_OK;
}

HRESULT CordbEval::CreateValue(CorElementType elementType,
                               ICorDebugClass *pElementClass,
                               ICorDebugValue **ppValue)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    CordbType *typ;

    if (((elementType < ELEMENT_TYPE_BOOLEAN) ||
         (elementType > ELEMENT_TYPE_R8)) &&
        !(elementType == ELEMENT_TYPE_CLASS))
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    // MkUnparameterizedType now works if you give it ELEMENT_TYPE_CLASS and
    // a null pElementClass - it returns the type for ELEMENT_TYPE_OBJECT.
    //
    // -- dsyme Mon May 20 09:41:00 2002
    //

    hr = CordbType::MkUnparameterizedType(m_thread->GetAppDomain(), elementType, (CordbClass *) pElementClass, &typ);

    if (FAILED(hr))
        return hr;

    return CreateValueForType(typ, ppValue);
}

HRESULT CordbEval::CreateValueForType(ICorDebugType *pIType,
                                      ICorDebugValue **ppValue)
{
    HRESULT hr = S_OK;

    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(ppValue, ICorDebugValue **);
    VALIDATE_POINTER_TO_OBJECT(pIType, ICorDebugType*);

    *ppValue = NULL;
    CordbType *pType = static_cast<CordbType *> (pIType);

    CorElementType elementType = pType->m_elementType;
    if (((elementType < ELEMENT_TYPE_BOOLEAN) ||
         (elementType > ELEMENT_TYPE_R8)) &&
        !((elementType == ELEMENT_TYPE_CLASS) || (elementType == ELEMENT_TYPE_OBJECT)))
        return E_INVALIDARG;

    // Note: ELEMENT_TYPE_OBJECT is what we'll get for the null reference case, so allow that.
    if ((elementType == ELEMENT_TYPE_CLASS) || (elementType == ELEMENT_TYPE_OBJECT))
    {
        CordbReferenceValue *rv = new (nothrow) CordbReferenceValue(pType);

        if (rv)
        {
            HRESULT hr2 = rv->InitRef();

            if (SUCCEEDED(hr2))
            {
                rv->ExternalAddRef();
                *ppValue = (ICorDebugValue*)(ICorDebugReferenceValue*)rv;
            }
            else
                delete rv;
        }
        else
            hr = E_OUTOFMEMORY;
    }
    else
    {
        // Create a generic value.
        CordbGenericValue *gv = new (nothrow) CordbGenericValue(pType);

        if (gv)
        {
            HRESULT hr2 = gv->Init();

            if (SUCCEEDED(hr2))
            {
                gv->ExternalAddRef();
                *ppValue = (ICorDebugValue*)(ICorDebugGenericValue*)gv;
            }
            else
                delete gv;
        }
        else
            hr = E_OUTOFMEMORY;
    }

    return hr;
}


/* ------------------------------------------------------------------------- *
 * CordbEval2
 *
 *   Extentions to the CordbEval class for Whidbey
 *
 * ------------------------------------------------------------------------- */


/*
 * This routine submits a rude abort request to the LS.
 *
 * Parameters:
 *     None.
 *
 * Returns:
 *     The HRESULT as returned by the LS.
 *
 */

HRESULT
CordbEval::RudeAbort(
    void
    )
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);


    ATT_ALLOW_LIVE_DO_STOPGO(GetProcess());

    //
    // No need to abort if its already completed.
    //
    if (m_complete)
    {
        return S_OK;
    }

    //
    // Can't abort if its never even been started.
    //
    if (m_debuggerEvalKey == NULL)
    {
        return E_INVALIDARG;
    }

    CORDBRequireProcessStateOK(m_thread->GetProcess());

    //
    // Send over to the left side to get the eval aborted.
    //
    DebuggerIPCEvent event;

    m_thread->GetProcess()->InitIPCEvent(&event,
                                         DB_IPCE_FUNC_EVAL_RUDE_ABORT,
                                         true,
                                         m_thread->GetAppDomain()->GetADToken()
                                        );

    event.FuncEvalRudeAbort.debuggerEvalKey = m_debuggerEvalKey;

    HRESULT hr = m_thread->GetProcess()->SendIPCEvent(&event,
                                                      sizeof(DebuggerIPCEvent)
                                                     );

    //
    // If the send failed, return that failure.
    //
    if (FAILED(hr))
    {
        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_FUNC_EVAL_RUDE_ABORT_RESULT);

    //
    // Since we may have
    // overwritten anything (objects, code, etc), we should mark
    // everything as needing to be re-cached.
    //
    m_thread->GetProcess()->m_continueCounter++;

    hr = event.hr;

    return hr;
}




/* ------------------------------------------------------------------------- *
 * CodeParameter Enumerator class
 * ------------------------------------------------------------------------- */

CordbCodeEnum::CordbCodeEnum(unsigned int cCodes, RSSmartPtr<CordbCode> * ppCodes) :
    CordbBase(NULL, 0)
{
    // Because the array is of smart-ptrs, the elements are already reffed
    // We now take ownership of the array itself too.
    m_ppCodes = ppCodes;

    m_iCurrent = 0;
    m_iMax = cCodes;
}


CordbCodeEnum::~CordbCodeEnum()
{
    // This will invoke the SmartPtr dtors on each element and call release.
    delete [] m_ppCodes;
}

HRESULT CordbCodeEnum::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugEnum)
        *pInterface = static_cast<ICorDebugEnum*>(this);
    else if (id == IID_ICorDebugCodeEnum)
        *pInterface = static_cast<ICorDebugCodeEnum*>(this);
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugCodeEnum*>(this));
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbCodeEnum::Skip(ULONG celt)
{
    HRESULT hr = E_FAIL;
    if ( (m_iCurrent+celt) < m_iMax ||
         celt == 0)
    {
        m_iCurrent += celt;
        hr = S_OK;
    }

    return hr;
}

HRESULT CordbCodeEnum::Reset(void)
{
    m_iCurrent = 0;
    return S_OK;
}

HRESULT CordbCodeEnum::Clone(ICorDebugEnum **ppEnum)
{
    VALIDATE_POINTER_TO_OBJECT(ppEnum, ICorDebugEnum **);
    (*ppEnum) = NULL;

    HRESULT hr = S_OK;

    // Create a new copy of the array because the CordbCodeEnum will
    // take ownership of it.
    RSSmartPtr<CordbCode> * ppCodes = new (nothrow) RSSmartPtr<CordbCode> [m_iMax];
    if (ppCodes == NULL)
    {
        return E_OUTOFMEMORY;
    }
    for(UINT i = 0; i < m_iMax; i++)
    {
        ppCodes[i].Assign(m_ppCodes[i]);
    }


    CordbCodeEnum *pCVE = new (nothrow) CordbCodeEnum( m_iMax, ppCodes);
    if ( pCVE == NULL )
    {
        delete [] ppCodes;
        hr = E_OUTOFMEMORY;
        goto LExit;
    }

    pCVE->ExternalAddRef();
    (*ppEnum) = (ICorDebugEnum*)pCVE;

LExit:
    return hr;
}

HRESULT CordbCodeEnum::GetCount(ULONG *pcelt)
{
    VALIDATE_POINTER_TO_OBJECT(pcelt, ULONG *);

    if( pcelt == NULL)
        return E_INVALIDARG;

    (*pcelt) = m_iMax;
    return S_OK;
}

//
// In the event of failure, the current pointer will be left at
// one element past the troublesome element.  Thus, if one were
// to repeatedly ask for one element to iterate through the
// array, you would iterate exactly m_iMax times, regardless
// of individual failures.
HRESULT CordbCodeEnum::Next(ULONG celt, ICorDebugCode *values[], ULONG *pceltFetched)
{
    VALIDATE_POINTER_TO_OBJECT_ARRAY(values, ICorDebugClass *,
        celt, true, true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pceltFetched, ULONG *);

    if ((pceltFetched == NULL) && (celt != 1))
    {
        return E_INVALIDARG;
    }

    if (celt == 0)
    {
        if (pceltFetched != NULL)
        {
            *pceltFetched = 0;
        }
        return S_OK;
    }

    HRESULT hr = S_OK;

    int iMax = min( m_iMax, m_iCurrent+celt);
    int i;

    for (i = m_iCurrent; i < iMax; i++)
    {
        values[i-m_iCurrent] = m_ppCodes[i];
        values[i-m_iCurrent]->AddRef();
    }

    int count = (i - m_iCurrent);

    if ( FAILED( hr ) )
    {   //we failed: +1 pushes us past troublesome element
        m_iCurrent += 1 + count;
    }
    else
    {
        m_iCurrent += count;
    }

    if (pceltFetched != NULL)
    {
        *pceltFetched = count;
    }

    //
    // If we reached the end of the enumeration, but not the end
    // of the number of requested items, we return S_FALSE.
    //
    if (((ULONG)count) < celt)
    {
        return S_FALSE;
    }

    return hr;
}

