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
// EEPolicy.cpp
// ---------------------------------------------------------------------------

#include "common.h"
#include "eepolicy.h"
#include "corhost.h"
#include "dbginterface.h"
#include "eemessagebox.h"

#undef ExitProcess

BYTE g_EEPolicyInstance[sizeof(EEPolicy)];

void InitEEPolicy()
{
    WRAPPER_CONTRACT;
    new (g_EEPolicyInstance) EEPolicy();
}

EEPolicy::EEPolicy ()
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;
    
    int n;
    for (n = 0; n < MaxClrOperation; n++) {
        m_Timeout[n] = INFINITE;
        m_ActionOnTimeout[n] = eNoAction;
        m_DefaultAction[n] = eNoAction;
    }
    m_Timeout[OPR_ProcessExit] = 40000;
    m_ActionOnTimeout[OPR_ProcessExit] = eRudeExitProcess;
    m_ActionOnTimeout[OPR_ThreadAbort] = eAbortThread;
    m_ActionOnTimeout[OPR_ThreadRudeAbortInNonCriticalRegion] = eRudeAbortThread;
    m_ActionOnTimeout[OPR_ThreadRudeAbortInCriticalRegion] = eRudeAbortThread;

    m_DefaultAction[OPR_ThreadAbort] = eAbortThread;
    m_DefaultAction[OPR_ThreadRudeAbortInNonCriticalRegion] = eRudeAbortThread;
    m_DefaultAction[OPR_ThreadRudeAbortInCriticalRegion] = eRudeAbortThread;
    m_DefaultAction[OPR_AppDomainUnload] = eUnloadAppDomain;
    m_DefaultAction[OPR_AppDomainRudeUnload] = eRudeUnloadAppDomain;
    m_DefaultAction[OPR_ProcessExit] = eExitProcess;
    m_DefaultAction[OPR_FinalizerRun] = eNoAction;

    for (n = 0; n < MaxClrFailure; n++) {
        m_ActionOnFailure[n] = eNoAction;
    }
    m_ActionOnFailure[FAIL_CriticalResource] = eThrowException;
    m_ActionOnFailure[FAIL_NonCriticalResource] = eThrowException;
    m_ActionOnFailure[FAIL_OrphanedLock] = eNoAction;
    m_ActionOnFailure[FAIL_FatalRuntime] = eRudeExitProcess;
    m_ActionOnFailure[FAIL_StackOverflow] = eRudeExitProcess;
    m_unhandledExceptionPolicy = eRuntimeDeterminedPolicy;
}

BOOL EEPolicy::IsValidActionForOperation(EClrOperation operation, EPolicyAction action)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;
    
    switch (operation) {
    case OPR_ThreadAbort:
        return action >= eAbortThread &&
            action < MaxPolicyAction;
        break;
    case OPR_ThreadRudeAbortInNonCriticalRegion:
    case OPR_ThreadRudeAbortInCriticalRegion:
        return action >= eRudeAbortThread && action != eUnloadAppDomain &&
            action < MaxPolicyAction;
        break;
    case OPR_AppDomainUnload:
        return action >= eUnloadAppDomain &&
            action < MaxPolicyAction;
        break;
    case OPR_AppDomainRudeUnload:
        return action >= eRudeUnloadAppDomain &&
            action < MaxPolicyAction;
        break;
    case OPR_ProcessExit:
        return action >= eExitProcess &&
            action < MaxPolicyAction;
        break;
    case OPR_FinalizerRun:
        return action == eNoAction ||
            (action >= eAbortThread &&
             action < MaxPolicyAction);
        break;
    default:
        _ASSERT (!"Do not know valid action for this operation");
        break;
    }
    return FALSE;
}

BOOL EEPolicy::IsValidActionForTimeout(EClrOperation operation, EPolicyAction action)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;
    
    switch (operation) {
    case OPR_ThreadAbort:
        return action > eAbortThread &&
            action < MaxPolicyAction;
        break;
    case OPR_ThreadRudeAbortInNonCriticalRegion:
    case OPR_ThreadRudeAbortInCriticalRegion:
        return action > eRudeUnloadAppDomain &&
            action < MaxPolicyAction;
        break;
    case OPR_AppDomainUnload:
        return action > eUnloadAppDomain &&
            action < MaxPolicyAction;
        break;
    case OPR_AppDomainRudeUnload:
        return action > eRudeUnloadAppDomain &&
            action < MaxPolicyAction;
        break;
    case OPR_ProcessExit:
        return action > eExitProcess &&
            action < MaxPolicyAction;
        break;
    case OPR_FinalizerRun:
        return action == eNoAction ||
            (action >= eAbortThread &&
             action < MaxPolicyAction);
        break;
    default:
        _ASSERT (!"Do not know valid action for this operation");
        break;
    }
    return FALSE;
}

BOOL EEPolicy::IsValidActionForFailure(EClrFailure failure, EPolicyAction action)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;
    
    switch (failure) {
    case FAIL_NonCriticalResource:
        return action >= eThrowException &&
            action < MaxPolicyAction;
        break;
    case FAIL_CriticalResource:
        return action >= eThrowException &&
            action < MaxPolicyAction;
        break;
    case FAIL_FatalRuntime:
        return action >= eRudeExitProcess &&
            action < MaxPolicyAction;
        break;
    case FAIL_OrphanedLock:
        return action >= eUnloadAppDomain &&
            action < MaxPolicyAction;
    case FAIL_StackOverflow:
        return action >= eRudeUnloadAppDomain &&
            action < MaxPolicyAction;
    default:
        _ASSERTE (!"Do not know valid action for this failure");
        break;
    }

    return FALSE;
}

HRESULT EEPolicy::SetTimeout(EClrOperation operation, DWORD timeout)
{
    CONTRACTL 
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
    }CONTRACTL_END;


    if (operation >= MaxClrOperation || operation < (EClrOperation) 0)
        return E_INVALIDARG;
    m_Timeout[operation] = timeout;
    if (operation == OPR_FinalizerRun &&
        g_fEEStarted)
    {
        FastInterlockOr((DWORD*)&g_FinalizerWaiterStatus, FWS_WaitInterrupt);
        GCHeap::GetGCHeap()->SignalFinalizationDone(FALSE);
    }
    return S_OK;
}

HRESULT EEPolicy::SetActionOnTimeout(EClrOperation operation, EPolicyAction action)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;
    
    if (IsValidActionForTimeout(operation, action)) {
        m_ActionOnTimeout[operation] = action;
        return S_OK;
    }
    else
        return E_INVALIDARG;
}

EPolicyAction EEPolicy::GetFinalAction(EPolicyAction action, Thread *pThread)
{
    if (action < eAbortThread || action > eFastExitProcess)
    {
        return action;
    }

    EPolicyAction finalAction = action;

    while(TRUE)
    {
        finalAction = action;
        while(TRUE)
        {
            // Look at default action
            switch (finalAction)
            {
            case eAbortThread:
                finalAction = m_DefaultAction[OPR_ThreadAbort];
                break;
            case eRudeAbortThread:
                if (pThread && !pThread->HasLockInCurrentDomain())
                {
                    finalAction = m_DefaultAction[OPR_ThreadRudeAbortInNonCriticalRegion];
                }
                else
                {
                    finalAction = m_DefaultAction[OPR_ThreadRudeAbortInCriticalRegion];
                }
                break;
            case eUnloadAppDomain:
                finalAction = m_DefaultAction[OPR_AppDomainUnload];
                break;
            case eRudeUnloadAppDomain:
                finalAction = m_DefaultAction[OPR_AppDomainRudeUnload];
                break;
            case eExitProcess:
            case eFastExitProcess:
                {
                EPolicyAction tmp = m_DefaultAction[OPR_ProcessExit];
                if (tmp > finalAction)
                {
                    finalAction = tmp;
                }
                break;
            }
            default:
                break;
            }

            if (finalAction == action)
            {
                break;
            }
            else
            {
                _ASSERTE (finalAction > action);
                action = finalAction;
            }
        }


        if (finalAction == action)
        {
            break;
        }
        _ASSERTE (finalAction > action);
        action = finalAction;
    }

    return finalAction;
}

// Allow setting timeout and action in one call.
// If we decide to have atomical operation on Policy, we can use lock here
// while SetTimeout and SetActionOnTimeout can not.
HRESULT EEPolicy::SetTimeoutAndAction(EClrOperation operation, DWORD timeout, EPolicyAction action)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;
    
    if (operation >= MaxClrOperation || operation < (EClrOperation)0)
        return E_INVALIDARG;
    
    if (IsValidActionForTimeout(operation, action)) {
        m_ActionOnTimeout[operation] = action;
        m_Timeout[operation] = timeout;
        if (operation == OPR_FinalizerRun &&
            g_fEEStarted)
        {
            FastInterlockOr((DWORD*)&g_FinalizerWaiterStatus, FWS_WaitInterrupt);
            GCHeap::GetGCHeap()->SignalFinalizationDone(FALSE);
        }
        return S_OK;
    }
    else
        return E_INVALIDARG;
}

HRESULT EEPolicy::SetDefaultAction(EClrOperation operation, EPolicyAction action)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;
    
    if (IsValidActionForOperation(operation, action)) {
        m_DefaultAction[operation] = action;
        return S_OK;
    }
    else
        return E_INVALIDARG;
}

HRESULT EEPolicy::SetActionOnFailure(EClrFailure failure, EPolicyAction action)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;

    if (IsValidActionForFailure(failure, action)) {
        m_ActionOnFailure[failure] = action;
        return S_OK;
    }
    else
        return E_INVALIDARG;
}

EPolicyAction EEPolicy::GetActionOnFailure(EClrFailure failure)
{
    CONTRACTL 
    {
        SO_TOLERANT;
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
    }CONTRACTL_END;

    _ASSERTE (failure < MaxClrFailure);
    if (failure == FAIL_StackOverflow)
    {
        return m_ActionOnFailure[failure];
    }

    EPolicyAction finalAction = GetFinalAction(m_ActionOnFailure[failure], GetThread());
    IHostPolicyManager *pHostPolicyManager = CorHost2::GetHostPolicyManager();
    if (pHostPolicyManager)
    {
#ifdef _DEBUG
        Thread* pThread = GetThread();
        if (pThread)
        {
            pThread->AddFiberInfo(Thread::ThreadTrackInfo_Escalation);
        }
#endif
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        pHostPolicyManager->OnFailure(failure, finalAction);
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }
    return finalAction;
}


void EEPolicy::NotifyHostOnTimeout(EClrOperation operation, EPolicyAction action)
{
    WRAPPER_CONTRACT;
    IHostPolicyManager *pHostPolicyManager = CorHost2::GetHostPolicyManager();
    if (pHostPolicyManager)
    {
#ifdef _DEBUG
        Thread* pThread = GetThread();
        if (pThread)
        {
            pThread->AddFiberInfo(Thread::ThreadTrackInfo_Escalation);
        }
#endif
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        pHostPolicyManager->OnTimeout(operation, action);
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }
}


void EEPolicy::NotifyHostOnDefaultAction(EClrOperation operation, EPolicyAction action)
{
    WRAPPER_CONTRACT;
    IHostPolicyManager *pHostPolicyManager = CorHost2::GetHostPolicyManager();
    if (pHostPolicyManager)
    {
#ifdef _DEBUG
        Thread* pThread = GetThread();
        if (pThread)
        {
            pThread->AddFiberInfo(Thread::ThreadTrackInfo_Escalation);
        }
#endif
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        pHostPolicyManager->OnDefaultAction(operation, action);
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }
}

void SafeExitProcess(UINT exitCode)
{
    STRESS_LOG1(LF_SYNC, LL_INFO10, "SafeExitProcesses: exitcode = %d\n", exitCode);
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        NOTHROW;
    }
    CONTRACTL_END;

    // The runtime must be in the appropriate thread mode when we exit, so that we
    // aren't surprised by the thread mode when our DLL_PROCESS_DETACH occurs, or when
    // other DLLs call Release() on us in their detach [dangerous!], etc.
    Thread *pThread = GetThread();
    if (pThread && pThread->PreemptiveGCDisabled())
    {
        pThread->EnablePreemptiveGC();
    }
    
    FastInterlockExchange((LONG*)&g_fForbidEnterEE, TRUE);
    
    ProcessEventForHost(Event_ClrDisabled, NULL);
    
    // Note that for free and retail builds StressLog must also be enabled
    if (g_pConfig && g_pConfig->StressLog())
    {
        if (REGUTIL::GetConfigDWORD(L"BreakOnBadExit", 0))
        {
            // Workaround for aspnet bug
            WCHAR  wszFilename[_MAX_PATH];
            bool bShouldAssert = true;
            if (WszGetModuleFileName(NULL, wszFilename, _MAX_PATH))
            {
                _wcslwr_s(wszFilename, COUNTOF(wszFilename));
                
                if (wcsstr(wszFilename, L"aspnet_compiler")) 
                {
                    bShouldAssert = false;
                }                   
            }
            
            unsigned goodExit = REGUTIL::GetConfigDWORD(L"SuccessExit", 0);
            if (bShouldAssert && exitCode != goodExit)
            {
                _ASSERTE(!"Bad Exit value");
                FAULT_NOT_FATAL();      // if we OOM we can simply give up
                EEMessageBox(IDS_EE_ERRORMESSAGETEMPLATE, IDS_EE_ERRORTITLE, MB_OK | MB_ICONEXCLAMATION, exitCode, L"BreakOnBadExit: returning bad exit code");
            }
        }
    }
    
    // If we call ExitProcess, other threads will be torn down 
    // so we don't get to debug their state.  Stop this!
#ifdef _DEBUG
    if (_DbgBreakCount)
        _ASSERTE(!"In SafeExitProcess: An assert was hit on some other thread");
#endif

    // Turn off exception processing, because if some other random DLL has a
    //  fault in DLL_PROCESS_DETACH, we could get called for exception handling.
    //  Since we've turned off part of the runtime, we can't, for instance,
    //  properly execute the GC that handling an exception might trigger.
    g_fNoExceptions = true;
    LOG((LF_EH, LL_INFO10, "SafeExitProcess: turning off exceptions\n"));

    ExitProcess(exitCode);
}

void DisableRuntime()
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        NOTHROW;
    }
    CONTRACTL_END;

    FastInterlockExchange((LONG*)&g_fForbidEnterEE, TRUE);
    
    if (!g_fSuspendOnShutdown)
    {
        if (!IsGCThread())
        {
            if (ThreadStore::HoldingThreadStore(GetThread()))
            {
                ThreadStore::UnlockThreadStore();
            }
            GCHeap::GetGCHeap()->SuspendEE(GCHeap::SUSPEND_FOR_SHUTDOWN);
        }

        if (!g_fSuspendOnShutdown)
        {
            ThreadStore::TrapReturningThreads(TRUE);
            g_fSuspendOnShutdown = TRUE;
            ClrFlsSetThreadType(ThreadType_Shutdown);
        }

        // Don't restart runtime.  CLR is disabled.
    }
    Thread *pThread = GetThread();
    if (pThread && pThread->PreemptiveGCDisabled())
    {
        pThread->EnablePreemptiveGC();
    }
    
    ProcessEventForHost(Event_ClrDisabled, NULL);
    ClrFlsClearThreadType(ThreadType_Shutdown);

    if (g_pDebugInterface != NULL)
    {
        g_pDebugInterface->DisableDebugger();
    }

    __SwitchToThread(INFINITE);
    _ASSERTE (!"Should not reach here");
    SafeExitProcess(0);
}

// If g_fFastExitProcess is 0, normal shutdown
// If g_fFastExitProcess is 1, fast shutdown.  Only doing log.
// If g_fFastExitProcess is 2, do not run EEShutDown.
DWORD g_fFastExitProcess = 0;

extern void STDMETHODCALLTYPE EEShutDown(BOOL fIsDllUnloading);

static void HandleExitProcessHelper(EPolicyAction action, UINT exitCode)
{
    WRAPPER_CONTRACT;
    
    switch (action) {
    case eFastExitProcess:
        g_fFastExitProcess = 1;
    case eExitProcess:
        if (g_fEEStarted)
        {
            EEShutDown(FALSE);
        }
        if (exitCode == 0)
        {
            exitCode = GetLatchedExitCode();
        }
        SafeExitProcess(exitCode);
        break;
    case eRudeExitProcess:
        g_fFastExitProcess = 2;
        SafeExitProcess(exitCode);
        break;
    case eDisableRuntime:
        DisableRuntime();
        break;
    default:
        _ASSERTE (!"Invalid policy");
        break;
    }
}


EPolicyAction EEPolicy::DetermineResourceConstraintAction(Thread *pThread)
{
    WRAPPER_CONTRACT;

    EPolicyAction action;
    if (pThread->HasLockInCurrentDomain()) {
        action = GetEEPolicy()->GetActionOnFailure(FAIL_CriticalResource);
    }
    else
        action = GetEEPolicy()->GetActionOnFailure(FAIL_NonCriticalResource);

    AppDomain *pDomain = GetAppDomain();
    // If it is default domain, we can not unload the appdomain 
    if (pDomain == SystemDomain::System()->DefaultDomain() &&
        (action == eUnloadAppDomain || action == eRudeUnloadAppDomain))
    {
        action = eThrowException;
    }
    // If the current thread is AD unload helper thread, it should not block itself.
    else if (pThread->HasThreadStateNC(Thread::TSNC_ADUnloadHelper) &&
        action < eExitProcess) 
    {
        action = eThrowException;
    }
    return action;
}


void EEPolicy::PerformADUnloadAction(EPolicyAction action, BOOL haveStack, BOOL forStackOverflow)
{
    WRAPPER_CONTRACT;
    
    STRESS_LOG0(LF_EH, LL_INFO100, "In EEPolicy::PerformADUnloadAction\n");       

    Thread *pThread = GetThread();

    AppDomain *pDomain = GetAppDomain();

    if (!IsFinalizerThread())
    {
        int count = 0;
        Frame *pFrame = pThread->GetFirstTransitionInto(GetAppDomain(), &count);
        {
            GCX_COOP();
            pThread->SetUnloadBoundaryFrame(pFrame);
        }
    }

    pDomain->EnableADUnloadWorker(action==eUnloadAppDomain? ADU_Safe : ADU_Rude);
    // Can't perform a join when we are handling a true SO.  We need to enable the unload woker but let the thread continue running
    // through EH processing so that we can recover the stack and reset the guard page. 
    if (haveStack)
    {
        pThread->SetAbortRequest(action==eUnloadAppDomain? EEPolicy::TA_V1Compatible : EEPolicy::TA_Rude);
        if (forStackOverflow)
        {
            OBJECTREF exceptObj = CLRException::GetPreallocatedRudeThreadAbortException();
            pThread->SetAbortInitiated();
            RaiseTheExceptionInternalOnly(exceptObj, FALSE, TRUE);
        }
            OBJECTREF exceptObj = CLRException::GetPreallocatedThreadAbortException();
            pThread->SetAbortInitiated();
            RaiseTheExceptionInternalOnly(exceptObj, FALSE, FALSE);
    }
}

void EEPolicy::PerformResourceConstraintAction(Thread *pThread, EPolicyAction action, UINT exitCode, BOOL haveStack)
    {
    WRAPPER_CONTRACT;

    _ASSERTE(GetAppDomain() != NULL);

    switch (action) {
    case eThrowException:
        // Caller is going to rethrow.
        return;
        break;
    case eAbortThread:
        pThread->UserAbort(Thread::TAR_Thread, TA_Safe, GetEEPolicy()->GetTimeout(OPR_ThreadAbort), Thread::UAC_Normal);
        break;
    case eRudeAbortThread:
        pThread->UserAbort(Thread::TAR_Thread, TA_Rude, GetEEPolicy()->GetTimeout(OPR_ThreadAbort), Thread::UAC_Normal);
        break;
    case eUnloadAppDomain:
    case eRudeUnloadAppDomain:
        PerformADUnloadAction(action,haveStack);
        break;
    case eExitProcess:
    case eFastExitProcess:
    case eRudeExitProcess:
    case eDisableRuntime:
        HandleExitProcessFromEscalation(action, exitCode);
        break;
    default:
        _ASSERTE (!"Invalid policy");
        break;
    }
}

void EEPolicy::HandleOutOfMemory()
{
    WRAPPER_CONTRACT;

    _ASSERTE (g_pOutOfMemoryExceptionClass);

    Thread *pThread = GetThread();
    _ASSERTE (pThread);

    EPolicyAction action = DetermineResourceConstraintAction(pThread);
    
    // Check if we are executing in the context of a Constrained Execution Region.
    if (action != eThrowException && Thread::IsExecutingWithinCer())
    {
        // Hitting OOM in a CER region should throw the OOM without regard to the escalation policy 
        // since the CER author has declared they are hardened against such failures. That’s 
        // the whole point of CERs, to denote regions where code knows exactly how to deal with 
        // failures in an attempt to minimize the need for rollback or recycling.
        return;
    }

    PerformResourceConstraintAction(pThread, action, HOST_E_EXITPROCESS_OUTOFMEMORY, TRUE);
}

// How is stack overflow handles?
// If stack overflows in non-hosted case, we terminate the process.
// For hosted case with escalation policy
// 1. If stack overflows in managed code, or in VM before switching to SO intolerant region, and the GC mode is Cooperative 
//    the domain is rudely unloaded, or the process is terminated if the current domain is default domain.
//    a. This action is done through BEGIN_SO_TOLERANT_CODE if there is one.
//    b. If there is not this macro on the stack, we mark the domain being unload requested, and when the thread
//       dies or is recycled, we finish the AD unload.
// 2. If stack overflows in SO tolerant region, but the GC mode is Preemptive, the process is killed in vector handler, or our
//    managed exception handler (COMPlusFrameHandler or ProcessCLRException).
// 3. If stack overflows in SO intolerant region, the process is killed as soon as the exception is seen by our vector handler, or
//    our managed exception handler.
void EEPolicy::HandleStackOverflow(StackOverflowDetector detector)
{
    WRAPPER_CONTRACT;
    
    STRESS_LOG0(LF_EH, LL_INFO100, "In EEPolicy::HandleStackOverflow\n");

    Thread *pThread = GetThread();

    if (pThread == NULL)
    {
        //_ASSERTE (detector != SOD_ManagedFrameHandler);
        // ProcessSOEventForHost(NULL, FALSE);

        // For security reason, it is not safe to continue execution if stack overflow happens
        // unless a host tells us to do something different.
        // EEPolicy::HandleFatalStackOverflow(NULL);
        return;
    }

    // We only process SO once at
    // 1. VectoredExceptionHandler if SO in mscorwks
    // 2. managed exception handler
    // 3. SO_Tolerant transition handler
    if (pThread->HasThreadStateNC(Thread::TSNC_SOWorkNeeded))
    {
        return;
    }

    // Since this Thread has taken an SO, there may be state left-over after we
    //  short-circuited exception or other error handling, and so we don't want 
    //  to risk recycling it.
    pThread->SetThreadStateNC(Thread::TSNC_CannotRecycle);

    BOOL fInSoTolerant = pThread->IsSOTolerant();
    EXCEPTION_POINTERS *pExceptionInfo = pThread->GetSOExceptionInfo();
    AppDomain *pCurrentDomain = ::GetAppDomain();
    BOOL fInDefaultDomain = (pCurrentDomain == SystemDomain::System()->DefaultDomain());
    BOOL fInCLR = FALSE;

    if (fInDefaultDomain)
    {
        // SO in default domain is fatal
        fInSoTolerant = FALSE;
    }

    switch (detector)
    {
    case SOD_ManagedFrameHandler:
        if (!pThread->PreemptiveGCDisabled() && !fInCLR && fInSoTolerant &&
            pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_STACK_OVERFLOW 
            )
        {
            // Managed exception handler detects SO, but the thread is in preemptive GC mode,
            // and the IP is outside CLR.  This means we are inside a PINVOKE call.
            fInSoTolerant = FALSE;
        }

        if (fInDefaultDomain)
        {
            // StackOverflow in default domain is fatal
            fInSoTolerant = FALSE;
        }
        break;
    case SOD_VectoredExceptionHandler:
        {
            // On X86, we do not use OS VectoredExceptionHandler support.  When this function is called,
            // we are inside VM, so we can escalate.
            // On Win64, this function is called wheneven there is an exception, even though the exception may
            // be handled.

            // If this is a soft so, then we'll always handle it.
            
            EXCEPTION_POINTERS* SOexceptionInfo = pThread->GetSOExceptionInfo();

            if (SOexceptionInfo != NULL &&
                SOexceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SOFTSO)
            {
                fInSoTolerant = TRUE;
            }
            else if (!fInCLR)
            {
#ifdef _X86_
                fInSoTolerant = FALSE;
#else
                return;
#endif
            }
        break;
        }
    case SOD_SOIntolerantTransitor:
        // If hard SO is handled by SOIntolerantTransitor, it is always fatal.
        // If soft SO is handled, treat it as in tolerant.
        {
            fInSoTolerant = FALSE;
        }
        break;
    case SOD_SOTolerantTransitor:
        if (!fInCLR)
        {
            // If SO happens outside of CLR, and it is not detected by managed frame handler,
            // it is fatal
            fInSoTolerant = FALSE;
        }
        break;

    case SOD_SOTolerantTransitorGotSoftSO:
        if (!fInDefaultDomain)
        {
            // A soft stack overflow happened. The process should still be in a consistent
            // state.
            fInSoTolerant = TRUE;
        }
        break;
        
    default:
        _ASSERTE(!"should not get here");
    }

    ProcessSOEventForHost(pExceptionInfo, fInSoTolerant);

    if (!CLRHosted() || GetEEPolicy()->GetActionOnFailure(FAIL_StackOverflow) != eRudeUnloadAppDomain)
    {
        // For security reason, it is not safe to continue execution if stack overflow happens
        // unless a host tells us to do something different.
        EEPolicy::HandleFatalStackOverflow(pExceptionInfo);
    }

    if (!fInSoTolerant)
    {
        EEPolicy::HandleFatalStackOverflow(pExceptionInfo);
    }
    else
    {
        // EnableADUnloadWorker is SO_Intolerant.
        // But here we know that if we have only one page, we will only update states of the Domain.
        CONTRACT_VIOLATION(SOToleranceViolation);
        // Mark the current domain requested for rude unload
        pCurrentDomain->EnableADUnloadWorker(ADU_Rude, FALSE);
        pThread->MarkThreadForAbort(Thread::TAR_Thread,EEPolicy::TA_Rude);
        pThread->SetSOWorkNeeded();
    }
}

// This function may be called on a thread before debugger is notified of the thread, like in 
// ManagedThreadBase_DispatchMiddle.  Currently we can not notify managed debugger, because 
// RS requires that notification is sent first.
void EEPolicy::HandleSoftStackOverflow(BOOL fSkipDebugger)
{
    WRAPPER_CONTRACT;

    // If we trigger a SO while handling the soft stack overflow,
    // we'll rip the process
    BEGIN_SO_INTOLERANT_CODE_NOPROBE;
    
    AppDomain *pCurrentDomain = ::GetAppDomain();

    if (GetEEPolicy()->GetActionOnFailure(FAIL_StackOverflow) != eRudeUnloadAppDomain ||
        pCurrentDomain == SystemDomain::System()->DefaultDomain())
    {
        // We may not be able to build a context on stack
        ProcessSOEventForHost(NULL, FALSE);

        // We provide WatsonLastChance with a SO exception record. The ExceptionAddress is set to 0
        // here.  This ExceptionPointers struct is handed off to the debugger as is. A copy of this struct
        // is made before invoking Watson and the ExceptionAddress is set by inspecting the stack. Note
        // that the ExceptionContext member is unused and so it's ok to set it to NULL.
        static EXCEPTION_RECORD g_SOExceptionRecord = {
                       STATUS_STACK_OVERFLOW, // ExceptionCode
                       0,                     // ExceptionFlags
                       NULL,                  // ExceptionRecord
                       0,                     // ExceptionAddress
                       0,                     // NumberOfParameters
                       NULL};                 // ExceptionInformation
                       
        static EXCEPTION_POINTERS g_SOExceptionPointers = {&g_SOExceptionRecord, NULL};
        
        EEPolicy::HandleFatalStackOverflow(&g_SOExceptionPointers, fSkipDebugger);
    }
    //else if (pCurrentDomain == SystemDomain::System()->DefaultDomain())
    //{
        // We hit soft SO in Default domain, but default domain can not be unloaded.
        // Soft SO can happen in default domain, eg. GetResourceString, or EnsureGrantSetSerialized.
        // So the caller is going to throw a managed exception.
    //    RaiseException(EXCEPTION_SOFTSO, 0, 0, NULL);
    //}
    else
    {
        Thread* pThread = GetThread();
        
        if (pThread && pThread->PreemptiveGCDisabled())
        {
            // Mark the current domain requested for rude unload
            EEPolicy::PerformADUnloadAction(eRudeUnloadAppDomain, TRUE, TRUE);
        }

        // We are leaving VM boundary, either entering managed code, or entering
        // non-VM unmanaged code.
        // We should not throw internal C++ exception.  Instead we throw an exception
        // with EXCEPTION_SOFTSO code.
        RaiseException(EXCEPTION_SOFTSO, 0, 0, NULL);
    }

    END_SO_INTOLERANT_CODE_NOPROBE;
    
}


void EEPolicy::HandleExitProcess()
{
    WRAPPER_CONTRACT;    

    STRESS_LOG0(LF_EH, LL_INFO100, "In EEPolicy::HandleExitProcess\n");
    
    EPolicyAction action = GetEEPolicy()->GetDefaultAction(OPR_ProcessExit, NULL);
    GetEEPolicy()->NotifyHostOnDefaultAction(OPR_ProcessExit,action);
    HandleExitProcessHelper (action, 0);
}

//
// Log an error to the event log if possible, then throw up a dialog box.
//

void EEPolicy::LogFatalError(UINT exitCode, UINT_PTR address, LPCWSTR pszMessage /* = NULL */, PEXCEPTION_POINTERS pExceptionInfo /* = NULL */)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;


    if (!CLRHosted())
    {
#ifdef DEBUGGING_SUPPORTED
        // Finally, give a managed debugger a chance if this fatal error is on a managed thread.
        Thread *pThread = GetThread();
        
        if (pThread)
        {
            GCX_COOP();

            // We can't do this update here on WIN64.  See UpdateCurrentThrowable() for more information.
            // The managed debugger likes to have a throwable set before calling LastChanceManagedException (its
            // crazy that way.) We go ahead and set one of the preallocated exception objects as the current
            // exception, then give the managed debugger a chance at it.
            if (exitCode == (UINT)COR_E_STACKOVERFLOW)
            {
                // If we're going down because of stack overflow, go ahead and use the preallocated SO exception.
                pThread->SafeSetThrowables(CLRException::GetPreallocatedStackOverflowException());
            }
            else
            {
                pThread->SafeSetThrowables(CLRException::GetPreallocatedExecutionEngineException());
            }

            // If a managed debugger is already attached, and if that debugger is thinking it might be inclined to
            // try to intercept this excepiton, then tell it that's not possible.
            if (pThread->IsExceptionInProgress())
            {
                pThread->GetExceptionState()->GetFlags()->SetDebuggerInterceptNotPossible();
            }

            EXCEPTION_RECORD exceptionRecord;
            EXCEPTION_POINTERS exceptionPointers;
            CONTEXT context;
            
            if (pExceptionInfo == NULL)
            {
                memset(&exceptionRecord, 0, sizeof(exceptionRecord));
            
                exceptionRecord.ExceptionCode = exitCode;
                exceptionRecord.ExceptionAddress = reinterpret_cast< PVOID >(address);

                memset(&exceptionPointers, 0, sizeof(exceptionPointers));

                memset(&context, 0, sizeof(context));

                exceptionPointers.ExceptionRecord = &exceptionRecord;
                exceptionPointers.ContextRecord = &context;
                pExceptionInfo = &exceptionPointers;
            }

            if  (EXCEPTION_CONTINUE_EXECUTION == WatsonLastChance(pThread, pExceptionInfo, TypeOfReportedError::FatalError))
            {
                LOG((LF_EH, LL_INFO100, "EEPolicy::LogFatalError: debugger ==> EXCEPTION_CONTINUE_EXECUTION\n"));
                _ASSERTE(!"Debugger should not have returned ContinueExecution");
            }
        }
#endif // DEBUGGING_SUPPORTED
    }
}

void DisplayStackOverflowException()
{
    PrintToStdErrA("\n");

    PrintToStdErrA("Process is terminated due to StackOverflowException.\n");
}

void DECLSPEC_NORETURN EEPolicy::HandleFatalStackOverflow(EXCEPTION_POINTERS *pExceptionInfo, BOOL fSkipDebugger)
{
    // This is fatal error.  We do not care about SO mode any more.
    // All of the code from here on out is robust to any failures in any API's that are called.
    CONTRACT_VIOLATION(GCViolation | ModeViolation | SOToleranceViolation | FaultNotFatal);

    WRAPPER_CONTRACT;

    STRESS_LOG0(LF_EH, LL_INFO100, "In EEPolicy::HandleFatalStackOverflow\n");

    if (!fSkipDebugger)
    {
        Thread *pThread = GetThread();
        if (pThread)
        {
            GCX_COOP();
            pThread->SafeSetThrowables(ObjectFromHandle(CLRException::GetPreallocatedStackOverflowExceptionHandle()) 
                                       DEBUG_ARG(ThreadExceptionState::STEC_CurrentTrackerEqualNullOkHackForFatalStackOverflow));
        }
        FrameWithCookie<FaultingExceptionFrame> fef;
        if (pExceptionInfo && pExceptionInfo->ContextRecord)
        {
            GCX_COOP();
            fef.InitAndLink(pExceptionInfo->ContextRecord);
        }
        WatsonLastChance(pThread, pExceptionInfo, TypeOfReportedError::UnhandledException);
    }

    DisplayStackOverflowException();

    TerminateProcess(GetCurrentProcess(), COR_E_STACKOVERFLOW);
    UNREACHABLE();
}

void DECLSPEC_NORETURN EEPolicy::HandleFatalError(UINT exitCode, UINT_PTR address, LPCWSTR pszMessage /* = NULL */, PEXCEPTION_POINTERS pExceptionInfo /* = NULL */)
{
    WRAPPER_CONTRACT;

    // All of the code from here on out is robust to any failures in any API's that are called.
    FAULT_NOT_FATAL();

    // All of the code from here on out is allowed to trigger a GC, even if we're in a no-trigger region. We're
    // ripping the process down due to a fatal error... our invariants are already gone.
    {
        // This is fatal error.  We do not care about SO mode any more.
        // All of the code from here on out is robust to any failures in any API's that are called.
        CONTRACT_VIOLATION(GCViolation | ModeViolation | SOToleranceViolation | FaultNotFatal);
    
        g_fFastExitProcess = 2;
    
    switch (GetEEPolicy()->GetActionOnFailure(FAIL_FatalRuntime))
    {
    case eRudeExitProcess:
        LogFatalError(exitCode, address, pszMessage, pExceptionInfo);
        SafeExitProcess(exitCode);
        break;
    case eDisableRuntime:
        LogFatalError(exitCode, address, pszMessage, pExceptionInfo);
        DisableRuntime();
        break;
    default:
        _ASSERTE(!"Invalid action for FAIL_FatalRuntime");
        break;
        }
    }

    UNREACHABLE();
}

//
// Really annoying: we need to export EEPolicy::HandleFatalError to src\dlls\mscoree\mscoree.cpp. This is a
// straight C function to make that easy. Please, only use this from mscoree.cpp, and don't add it to EEPolicy.h.
//
void DECLSPEC_NORETURN EEPolicy_HandleFatalError(UINT exitCode, UINT_PTR address)
{
    EEPolicy::HandleFatalError(exitCode, address);
}

void EEPolicy::HandleExitProcessFromEscalation(EPolicyAction action, UINT exitCode)
{
    WRAPPER_CONTRACT;
    CONTRACT_VIOLATION(GCViolation); 

    _ASSERTE (action >= eExitProcess);
    // If policy for ExitProcess is not default action, i.e. ExitProcess, we will use it.
    // Otherwise overwrite it with passing arg action;
    EPolicyAction todo = GetEEPolicy()->GetDefaultAction(OPR_ProcessExit, NULL);
    if (todo == eExitProcess)
    {
        todo = action;
    }
    GetEEPolicy()->NotifyHostOnDefaultAction(OPR_ProcessExit,todo);

    HandleExitProcessHelper(todo, exitCode);
}
