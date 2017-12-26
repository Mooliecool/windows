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

/*  THREADS.CPP:
 *
 */

#include "common.h"

#include "tls.h"
#include "frames.h"
#include "threads.h"
#include "stackwalk.h"
#include "excep.h"
#include "comsynchronizable.h"
#include "log.h"
#include "gcscan.h"
#include "gc.h"
#include "mscoree.h"
#include "dbginterface.h"
#include "corprof.h"                // profiling
#include "eeprofinterfaces.h"
#include "eeconfig.h"
#include "perfcounters.h"
#include "corhost.h"
#include "win32threadpool.h"
#include "comstring.h"
#include "jitinterface.h"
#include "threads.inl"
#include "appdomainstack.inl"
#include "appdomainhelper.h"
#include "comutilnative.h"
#include "fusion.h"
#include "wrappers.h"
#include "memoryreport.h"

#include "nativeoverlapped.h"

#include "mdaassistantsptr.h"
#include "appdomain.inl"
#include "vmholder.h"
#include "exceptmacros.h"



// from ntstatus.h
#define STATUS_SUSPEND_COUNT_EXCEEDED    ((NTSTATUS)0xC000004AL)


#ifndef CONTEXT_EXCEPTION_ACTIVE
#define CONTEXT_EXCEPTION_ACTIVE        0x8000000
#endif//CONTEXT_EXCEPTION_ACTIVE
#ifndef CONTEXT_SERVICE_ACTIVE
#define CONTEXT_SERVICE_ACTIVE          0x10000000
#endif//CONTEXT_SERVICE_ACTIVE
#ifndef CONTEXT_EXCEPTION_REPORTING
#define CONTEXT_EXCEPTION_REPORTING     0x80000000
#endif//CONTEXT_EXCEPTION_REPORTING
#ifndef CONTEXT_EXCEPTION_REQUEST
#define CONTEXT_EXCEPTION_REQUEST       0x40000000
#endif//CONTEXT_EXCEPTION_REQUEST

#if defined(_X86_) || defined(_AMD64_)
#define HIJACK_NONINTERRUPTIBLE_THREADS
#endif

SPTR_IMPL(ThreadStore, ThreadStore, s_pThreadStore);
CONTEXT *ThreadStore::s_pOSContext = NULL;
CLREvent *ThreadStore::s_pWaitForStackCrawlEvent;

#ifndef DACCESS_COMPILE

#include "constrainedexecutionregion.h"


CLREvent* ThreadStore::s_hAbortEvt = NULL;
CLREvent* ThreadStore::s_hAbortEvtCache = NULL;

BOOL Thread::s_fCleanFinalizedThread = FALSE;

bool          Thread::s_fSysSuspendInProgress = false;

CLREvent* Thread::g_pGCSuspendEvent = NULL;

CrstStatic g_DeadlockAwareCrst;

// Here starts the unmanaged portion of the compressed stack code.
// The mission of this code is to provide us with an intermediate
// step between the stackwalk that has to happen when we make an
// async call and the formation of the managed PermissionListSet
// object since the latter is a very expensive operation.
//
// The basic structure of the compressed stack at this point is
// a list of compressed stack entries, where each entry represents
// one piece of "interesting" information found during the stackwalk.
// At this time, the "interesting" bits are appdomain transitions,
// assembly security, descriptors, appdomain security descriptors,
// frame security descriptors, and other compressed stacks.  Of course,
// if that's all there was to it, there wouldn't be an explanatory
// comment even close to this size before you even started reading
// the code.  Since we need to form a compressed stack whenever an
// async operation is registered, it is a very perf critical piece
// of code.  As such, things get very much more complicated than
// the simple list of objects described above.  The special bonus
// feature is that we need to handle appdomain unloads since the
// list tracks appdomain specific data.  Keep reading to find out
// more.

#if defined(_X86_)
#define GetRedirectHandlerForGCThreadControl() (&Thread::RedirectedHandledJITCaseForGCThreadControl)
#define GetRedirectHandlerForDbgThreadControl() (&Thread::RedirectedHandledJITCaseForDbgThreadControl)
#define GetRedirectHandlerForUserSuspend() (&Thread::RedirectedHandledJITCaseForUserSuspend)
#define GetRedirectHandlerForYieldTask() (&Thread::RedirectedHandledJITCaseForYieldTask)
#endif // _IA64_ || _AMD64_


#if USE_INDIRECT_GET_THREAD_APPDOMAIN
EXTERN_C Thread* __stdcall GetThreadGeneric(VOID);
EXTERN_C AppDomain* __stdcall GetAppDomainGeneric(VOID);
#endif



// #define     NEW_TLS     1

#ifdef _DEBUG
void  Thread::SetFrame(Frame *pFrame) 
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        DEBUG_ONLY;
    }
    CONTRACTL_END;

    _ASSERTE(NULL != pFrame);

    m_pFrame = pFrame;
    _ASSERTE(PreemptiveGCDisabled());

    if (this != GetThread())
        return;

    // If stack overrun corruptions are expected, then skip this check
    // as the Frame chain may have been corrupted.
    if (g_pConfig->fAssertOnFailFast() == false)
        return;

    Frame* espVal = (Frame*)GetCurrentSP();

    while (pFrame != (Frame*) -1)
    {
        static Frame* stopFrame = 0;
        if (pFrame == stopFrame)
            _ASSERTE(!"SetFrame frame == stopFrame");

        _ASSERTE(espVal < pFrame);
        _ASSERTE(pFrame->GetFrameType() < Frame::TYPE_COUNT);

        pFrame = pFrame->m_Next;
    }
}

#endif // _DEBUG

//************************************************************************
// PRIVATE GLOBALS
//************************************************************************


#endif // #ifndef DACCESS_COMPILE

// This is really just a heuristic to detect if we are executing in an M2U IL stub or
// one of the marshaling methods it calls.  It doesn't deal with U2M IL stubs.
// We loop through the frame chain looking for an uninitialized TransitionFrame.  
// If there is one, then we are executing in an M2U IL stub or one of the methods it calls.  
// On the other hand, if there is an initialized TransitionFrame, then we are not.
// Also, if there is an HMF on the stack, then we stop.  This could be the case where
// an IL stub calls an FCALL which ends up in a managed method, and the debugger wants to 
// stop in those cases.  Some examples are COMException..ctor and custom marshalers.
bool Thread::DetectHandleILStubsForDebugger()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    Frame* pFrame = GetFrame();

    if (pFrame != NULL)
    {
        while (pFrame != FRAME_TOP)
        {
            // Check for HMF's.  See the comment at the beginning of this function.
            if (pFrame->GetTransitionType() == Frame::TT_InternalCall)
            {
                break;
            }
            // If there is an entry frame (i.e. U2M managed), we should break.
            else if (pFrame->GetFrameType() == Frame::TYPE_ENTRY)
            {
                break;
            }
            // Check for M2U transition frames.  See the comment at the beginning of this function.
            else if (pFrame->GetFrameType() == Frame::TYPE_EXIT)
            {
                if (pFrame->GetReturnAddress() == NULL)
                {
                    // If the return address is NULL, then the frame has not been initialized yet.
                    return true;
                }
                else
                {
                    // The frame is fully initialized.
                    return false;
                }
            }
            pFrame = pFrame->Next();
        }
    }
    return false;
}

extern "C" {
GVAL_IMPL_INIT(DWORD, gThreadTLSIndex, TLS_OUT_OF_INDEXES);      // index ( (-1) == uninitialized )
GVAL_IMPL_INIT(DWORD, gAppDomainTLSIndex, TLS_OUT_OF_INDEXES);   // index ( (-1) == uninitialized )
}

#ifndef DACCESS_COMPILE

#define ThreadInited()          (gThreadTLSIndex != TLS_OUT_OF_INDEXES)

// Every PING_JIT_TIMEOUT ms, check to see if a thread in JITted code has wandered
// into some fully interruptible code (or should have a different hijack to improve
// our chances of snagging it at a safe spot).
#define PING_JIT_TIMEOUT        250

// When we find a thread in a spot that's not safe to abort -- how long to wait before
// we try again.
#define ABORT_POLL_TIMEOUT      10
#ifdef _DEBUG
#define ABORT_FAIL_TIMEOUT      40000
#endif // _DEBUG

//
// CANNOT USE IsBad*Ptr() methods here.  They are *banned* APIs because of various
// reasons (see http://winweb/wincet/bannedapis.htm).
//
#define IS_VALID_WRITE_PTR(addr, size)      _ASSERTE(addr != NULL)
#define IS_VALID_CODE_PTR(addr)             _ASSERTE(addr != NULL)


void Thread::SetSysSuspendInProgress(StateHolderParam)
{
    _ASSERTE(ThreadStore::HoldingThreadStore() || g_fProcessDetach);
    _ASSERTE(!s_fSysSuspendInProgress || g_fProcessDetach);
    s_fSysSuspendInProgress = true;
}

void Thread::ResetSysSuspendInProgress(StateHolderParam)
{
    _ASSERTE(ThreadStore::HoldingThreadStore() || g_fProcessDetach);
    _ASSERTE(s_fSysSuspendInProgress || g_fProcessDetach);
    s_fSysSuspendInProgress = false;
}


BOOL Thread::Alert ()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    BOOL fRetVal = FALSE;
    HostComHolder<IHostTask> pHostTask(GetHostTaskWithAddRef());
    if (pHostTask && !HasThreadStateNC(TSNC_OSAlertableWait)) {
        HRESULT hr;

        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pHostTask->Alert();
        END_SO_TOLERANT_CODE_CALLING_HOST;
        fRetVal = SUCCEEDED(hr);
    }
    else
    {
        HANDLE handle = GetThreadHandle();
        if (handle != INVALID_HANDLE_VALUE && handle != SWITCHOUT_HANDLE_VALUE)
        {
            fRetVal = ::QueueUserAPC(UserInterruptAPC, handle, APC_Code);
        }
    }

    return fRetVal;
}

struct HostJoinOnThreadArgs
{
    IHostTask *pHostTask;
    WaitMode mode;
};

DWORD HostJoinOnThread (void *args, DWORD timeout, DWORD option)
{
    CONTRACTL {
        THROWS;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    HostJoinOnThreadArgs *joinArgs = (HostJoinOnThreadArgs*) args;
    IHostTask *pHostTask = joinArgs->pHostTask;
    if ((joinArgs->mode & WaitMode_InDeadlock) == 0)
    {
        option |= WAIT_NOTINDEADLOCK;
    }

    HRESULT hr;
    BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
    hr = pHostTask->Join(timeout, option);
    END_SO_TOLERANT_CODE_CALLING_HOST;
    if (hr == S_OK) {
        return WAIT_OBJECT_0;
    }
    else if (hr == HOST_E_TIMEOUT) {
        return WAIT_TIMEOUT;
    }
    else if (hr == HOST_E_INTERRUPTED) {
        _ASSERTE (option & WAIT_ALERTABLE);
        Thread *pThread = GetThread();
        if (pThread)
        {
            Thread::UserInterruptAPC(APC_Code);
        }
        return WAIT_IO_COMPLETION;
    }
    else if (hr == HOST_E_ABANDONED)
    {
        // The task died.
        return WAIT_OBJECT_0;
    }
    else if (hr == HOST_E_DEADLOCK)
    {
        _ASSERTE ((option & WAIT_NOTINDEADLOCK) == 0);
        RaiseDeadLockException();
    }
    _ASSERTE (!"Unknown host join status\n");
    return E_FAIL;
}


DWORD Thread::Join(DWORD timeout, BOOL alertable)
{
    WRAPPER_CONTRACT;
    return JoinEx(timeout,alertable?WaitMode_Alertable:WaitMode_None);
}
DWORD Thread::JoinEx(DWORD timeout, WaitMode mode)
{
    CONTRACTL {
        THROWS;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    BOOL alertable = (mode & WaitMode_Alertable)?TRUE:FALSE;

    Thread *pCurThread = GetThread();
    _ASSERTE(pCurThread || dbgOnly_IsSpecialEEThread());

    HostComHolder<IHostTask> pHostTask (GetHostTaskWithAddRef());

    if (pHostTask == NULL) {
        HANDLE handle = GetThreadHandle();
        if (handle == INVALID_HANDLE_VALUE) {
            return WAIT_FAILED;
        }
        if (pCurThread) {
            return pCurThread->DoAppropriateWait(1, &handle, FALSE, timeout, mode);
        }
        else {
            return WaitForSingleObjectEx(handle,timeout,alertable);
        }
    }
    else {
        HostJoinOnThreadArgs args = {pHostTask, mode};
        if (pCurThread) {
            return GetThread()->DoAppropriateWait(HostJoinOnThread, &args, timeout, mode);
        }
        else {
            return HostJoinOnThread (&args,timeout,alertable?WAIT_ALERTABLE:0);
        }
    }
}

extern INT32 MapFromNTPriority(INT32 NTPriority);

BOOL Thread::SetThreadPriority(
    int nPriority   // thread priority level
)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    BOOL fRet;
    HostComHolder<IHostTask> pHostTask (GetHostTaskWithAddRef());
    if (pHostTask != NULL) {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        fRet = (pHostTask->SetPriority(nPriority) == S_OK);
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }
    
    else
    {
        if (GetThreadHandle() == INVALID_HANDLE_VALUE) {
            // When the thread starts running, we will set the thread priority.
            fRet =  TRUE;
        }
        else
            fRet = ::SetThreadPriority(GetThreadHandle(), nPriority);
    }

    if (fRet)
    {
        GCX_COOP();
        THREADBASEREF pObject = (THREADBASEREF)ObjectFromHandle(m_ExposedObject);
        if (pObject != NULL)
        {
            pObject->SetPriority (MapFromNTPriority(nPriority));
        }
    }
    return fRet;
}

int Thread::GetThreadPriority()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    int nRetVal = -1;
    HostComHolder<IHostTask> pHostTask(GetHostTaskWithAddRef());
    if (pHostTask != NULL) {
        int nPriority;
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pHostTask->GetPriority(&nPriority);
        END_SO_TOLERANT_CODE_CALLING_HOST;

        nRetVal = (hr == S_OK)?nPriority:THREAD_PRIORITY_ERROR_RETURN;
    }

    else if (GetThreadHandle() == INVALID_HANDLE_VALUE) {
        nRetVal = FALSE;
    }
    else
        nRetVal = ::GetThreadPriority(GetThreadHandle());

    return nRetVal;
}

// When SuspendThread returns, target thread may still be executing user code.
// We can not access data, e.g. m_fPreemptiveGCDisabled, changed by target thread.
// But our code depends on reading these data.  To make this operation safe, we
// call GetThreadContext which returns only after target thread does not execute
// any user code.

// Message from David Cutler
/*
    After SuspendThread returns, can the suspended thread continue to execute code in user mode?

    [David Cutler] The suspended thread cannot execute any more user code, but it might be currently “running”
    on a logical processor whose other logical processor is currently actually executing another thread.
    In this case the target thread will not suspend until the hardware switches back to executing instructions
    on its logical processor. In this case even the memory barrier would not necessarily work – a better solution
    would be to use interlocked operations on the variable itself.

    After SuspendThread returns, does the store buffer of the CPU for the suspended thread still need to drain?

    Historically, we’ve assumed that the answer to both questions is No.  But on one 4/8 hyper-threaded machine
    running Win2K3 SP1 build 1421, we’ve seen two stress failures where SuspendThread returns while writes seem to still be in flight.

    Usually after we suspend a thread, we then call GetThreadContext.  This seems to guarantee consistency.
    But there are places we would like to avoid GetThreadContext, if it’s safe and legal.

    [David Cutler] Get context delivers a APC to the target thread and waits on an event that will be set
    when the target thread has delivered its context.

    Chris.
*/

// Message from Neill Clift
/*
    What SuspendThread does is insert an APC block into a target thread and request an inter-processor interrupt to
    do the APC interrupt. It doesn’t wait till the thread actually enters some state or the interrupt has been serviced.

    I took a quick look at the APIC spec in the Intel manuals this morning. Writing to the APIC posts a message on a bus.
    Processors accept messages and presumably queue the s/w interrupts at this time. We don’t wait for this acceptance
    when we send the IPI so at least on APIC machines when you suspend a thread it continues to execute code for some short time
    after the routine returns. We use other mechanisms for IPI and so it could work differently on different h/w.

*/
BOOL EnsureThreadIsSuspended (HANDLE hThread, Thread* pThread)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    WRAPPER_CONTRACT;

    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_INTEGER;
    BOOL ret;
    ret = ::GetThreadContext(hThread, &ctx);

    return ret;
}

FORCEINLINE VOID MyEnterLogLock(StateHolderParam)
{
    EnterLogLock();
}
FORCEINLINE VOID MyLeaveLogLock(StateHolderParam)
{
    LeaveLogLock();
}

// SuspendThread
//   Attempts to OS-suspend the thread, whichever GC mode it is in.
// Arguments:
//   fOneTryOnly - If TRUE, report failure if the thread has its
//     m_dwForbidSuspendThread flag set.  If FALSE, retry.
//   pdwSuspendCount - If non-NULL, will contain the return code
//     of the underlying OS SuspendThread call on success,
//     undefined on any kind of failure.
// Return value:
//   A SuspendThreadResult value indicating success or failure.
Thread::SuspendThreadResult Thread::SuspendThread(BOOL fOneTryOnly, DWORD *pdwSuspendCount)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef STRESS_LOG
    if (StressLog::StressLogOn(-1, 0))
    {
        // Make sure to create the stress log for the current thread
        // (if needed) before we suspend the target thread.  The target
        // thread may be holding the stress log lock when we suspend it,
        // which could cause a deadlock.
        if (StressLog::CreateThreadStressLog() == NULL)
        {
            return STR_NoStressLog;
        }
    }
#endif

    volatile HANDLE hThread;
    SuspendThreadResult str = (SuspendThreadResult) -1;
    DWORD dwSuspendCount = 0;
    DWORD tries = 1;

#if defined(_DEBUG)
    // Stop the stress log from allocating any new memory while in this function
    // as that can lead to deadlocks
    CantAllocHolder hldrCantAlloc;
#endif

    while (TRUE) {
        StateHolder<MyEnterLogLock, MyLeaveLogLock> LogLockHolder(FALSE);

        CounterHolder handleHolder(&m_dwThreadHandleBeingUsed);

        hThread = GetThreadHandle();
        if (hThread == INVALID_HANDLE_VALUE) {
            str = STR_UnstartedOrDead;
            break;
        }
        else if (hThread == SWITCHOUT_HANDLE_VALUE) {
            str = STR_SwitchedOut;
            break;
        }

        {
            // It is important to avoid two threads suspending each other.
            // Before a thread suspends another, it increments its own m_dwForbidSuspendThread count first,
            // then it checks the target thread's m_dwForbidSuspendThread.
            ForbidSuspendThreadHolder forbidSuspend;
            if ((m_dwForbidSuspendThread != 0))
            {
                    goto retry;
            }

            // We do not want to suspend the target thread while it is holding the log lock.
            // By acquiring the lock ourselves, we know that this is not the case.
            LogLockHolder.Acquire();
            dwSuspendCount = ::SuspendThread(hThread);
        }
        if ((int)dwSuspendCount >= 0)
        {
            if (!EnsureThreadIsSuspended(hThread, this))
            {
                ::ResumeThread(hThread);
                str = STR_Failure;
                break;
            }

            if (hThread == GetThreadHandle())
            {
                if (m_dwForbidSuspendThread != 0)
                {
                    ::ResumeThread(hThread);

                    goto retry;
                }
                // We suspend the right thread
#ifdef _DEBUG
                Thread *pCurThread = GetThread();
                if (pCurThread) {
                    pCurThread->m_dwSuspendThread ++;
                    _ASSERTE (pCurThread->m_dwSuspendThread > 0);
                }
#endif
                IncCantAllocCount();
                
                m_ThreadHandleForResume = hThread;
                str = STR_Success;
                break;
            }
            else
            {
                ::ResumeThread(hThread);
                continue;
            }
        }
        else {
            // We can get here either SuspendThread fails
            // Or the fiber thread dies after this fiber switched out.

            if ((int)dwSuspendCount != -1) {
                STRESS_LOG1(LF_SYNC, LL_INFO1000, "In Thread::SuspendThread ::SuspendThread returned %x\n", dwSuspendCount);
            }
            if (GetThreadHandle() == SWITCHOUT_HANDLE_VALUE) {
                str = STR_SwitchedOut;
                break;
            }
            else {
                // Our callers generally expect that STR_Failure means that
                // the thread has exited.
                str = STR_Failure;
                break;
            }
        }

retry:
        handleHolder.Release();
        LogLockHolder.Release();

        if (fOneTryOnly)
        {
            str = STR_Forbidden;
            break;
        }

        // When looking for deadlocks we need to allow the target thread to run in order to make some progress.
        // On multi processor machines we saw the suspending thread resuming immediately after the __SwitchToThread()
        // because it has another few processors available.  As a consequence the target thread was being Resumed and
        // Suspended right away, w/o a real chance to make any progress.
        if ((tries++) % 20 != 0) {
            YieldProcessor();           // play nice on hyperthreaded CPUs
        } else {
            __SwitchToThread(0);
        }
    }

#ifdef PROFILING_SUPPORTED
    if (CORProfilerTrackSuspends() && str == STR_Success)
    {
        // Must use wrapper to ensure forbid suspend count is incremented.
        // (See RuntimeTheadSuspendedWrapper in threads.h for more information.)
        RuntimeThreadSuspendedWrapper((ThreadID)this, (ThreadID)GetThread());
    }
#endif // PROFILING_SUPPORTED

    if (pdwSuspendCount != NULL)
    {
        *pdwSuspendCount = dwSuspendCount;
    }
    _ASSERTE(str != (SuspendThreadResult) -1);
    return str;
}

DWORD Thread::StartThread()
{
    WRAPPER_CONTRACT;
    DWORD dwRetVal = (DWORD) -1;
#ifdef _DEBUG
    _ASSERTE (m_Creater.IsSameThread());
    m_Creater.ResetThreadId();
#endif
    HostComHolder<IHostTask> pHostTask(GetHostTaskWithAddRef());
    if (pHostTask)
    {
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pHostTask->Start();
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (hr == S_OK) {
            dwRetVal = 1;
        }
        else
            dwRetVal = (DWORD) -1;
    }
    else
    {
        _ASSERTE (GetThreadHandle() != INVALID_HANDLE_VALUE &&
                  GetThreadHandle() != SWITCHOUT_HANDLE_VALUE);
        dwRetVal = ::ResumeThread(GetThreadHandle());
    }

    return dwRetVal;
}

DWORD Thread::ResumeThread()
{
    WRAPPER_CONTRACT;

    _ASSERTE (m_ThreadHandleForResume != INVALID_HANDLE_VALUE);

    _ASSERTE (m_pHostTask == 0 || GetThreadHandle() != SWITCHOUT_HANDLE_VALUE);
    //DWORD res = ::ResumeThread(GetThreadHandle());
    DWORD res = ::ResumeThread(m_ThreadHandleForResume);
    _ASSERTE (res != 0 && "Thread is not previously suspended");
#ifdef _DEBUG_IMPL
    _ASSERTE (!m_Creater.IsSameThread());
    Thread *pCurThread = GetThread();
    if (res != (DWORD) -1 && res != 0) {
        if (pCurThread) {
            _ASSERTE (pCurThread->m_dwSuspendThread > 0);
            pCurThread->m_dwSuspendThread --;
        }
    }
#endif
    if (res != (DWORD) -1 && res != 0) 
    {
        DecCantAllocCount ();
    }
    
    if (CORProfilerTrackSuspends() && (res != 0) && (res != (DWORD)-1))
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RuntimeThreadResumed((ThreadID)this, (ThreadID)GetThread());
    }

    return res;
}


// Class static data:
LONG    Thread::m_DebugWillSyncCount = -1;
LONG    Thread::m_DetachCount = 0;
LONG    Thread::m_ActiveDetachCount = 0;
int     Thread::m_offset_counter = 0;
volatile LONG Thread::m_threadsAtUnsafePlaces = 0;


//-------------------------------------------------------------------------
// Public function: SetupThreadNoThrow()
// Creates Thread for current thread if not previously created.
// Returns NULL for failure (usually due to out-of-memory.)
//-------------------------------------------------------------------------
Thread* SetupThreadNoThrow(HRESULT *pHR)
{
    CONTRACTL {
        NOTHROW;
        SO_TOLERANT;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    Thread *pThread = GetThread();
    if (pThread != NULL)
    {
        return pThread;
    }

    EX_TRY
    {
        CONTRACT_VIOLATION(SOToleranceViolation);
        pThread = SetupThread();
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    if (pHR)
    {
        *pHR = hr;
    }

    return pThread;
}

void DeleteThread(Thread* pThread)
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    //_ASSERTE (pThread == GetThread());
    UnsafeTlsSetValue(gThreadTLSIndex, NULL);
    UnsafeTlsSetValue(GetAppDomainTLSIndex(), NULL);

    if (pThread->HasThreadStateNC(Thread::TSNC_ExistInThreadStore))
    {
        pThread->DetachThread(FALSE);
    }
    else
    {
    FastInterlockOr((ULONG *)&pThread->m_State, Thread::TS_Dead);
    delete pThread;
}
}

void EnsurePreemptive(StateHolderParam)
{
    WRAPPER_CONTRACT;
    Thread *pThread = GetThread();
    if (pThread && pThread->PreemptiveGCDisabled())
    {
        pThread->EnablePreemptiveGC();
    }
}

typedef StateHolder<DoNothing, EnsurePreemptive> EnsurePreemptiveModeIfException;

Thread* SetupThread(BOOL fInternal)
{
    CONTRACTL {
        THROWS;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(SOToleranceViolation);

    _ASSERTE(ThreadInited());
    Thread* pThread;
    if ((pThread = GetThread()) != NULL)
        return pThread;

    // For interop debugging, we must mark that we're in a can't-stop region
    // b.c we may take Crsts here that may block the helper thread.
    // We're especially fragile here b/c we don't have a Thread object yet
    CantStopHolder hCantStop(TRUE);

    EnsurePreemptiveModeIfException ensurePreemptive;

#ifdef _DEBUG
    // Verify that for fiber mode, we do not have a thread that matches the current StackBase.
    if (CLRTaskHosted()) {
            ThreadStoreLockHolder TSLockHolder(TRUE);

            IHostTaskManager *provider = CorHost2::GetHostTaskManager();

            IHostTask *pHostTask = NULL;

            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
                provider->GetCurrentTask(&pHostTask);
            END_SO_TOLERANT_CODE_CALLING_HOST;

            if (pHostTask)
            {
                SafeComHolder<IHostTask> pHostTaskHolder(pHostTask);
                while ((pThread = ThreadStore::s_pThreadStore->GetAllThreadList(pThread, 0, 0)) != NULL)
                {
                    _ASSERTE ((pThread->m_State&Thread::TS_Unstarted) || pThread->GetHostTask() != pHostTask);
                }
            }
        }
#endif

#ifdef _DEBUG
    if (g_pConfig->SuppressChecks())
    {
        // EnterAssert will suppress any checks
        CHECK::EnterAssert();
    }
#endif

    // Normally, HasStarted is called from the thread's entrypoint to introduce it to
    // the runtime.  But sometimes that thread is used for DLL_THREAD_ATTACH notifications
    // that call into managed code.  In that case, a call to SetupThread here must
    // find the correct Thread object and install it into TLS.

    if (ThreadStore::s_pThreadStore->m_PendingThreadCount != 0)
    {
        DWORD  ourOSThreadId = ::GetCurrentThreadId();
        IHostTask *curHostTask = NULL;
        IHostTaskManager *hostTaskManager = CorHost2::GetHostTaskManager();
        if (hostTaskManager) {
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
            hostTaskManager->GetCurrentTask(&curHostTask);
            END_SO_TOLERANT_CODE_CALLING_HOST;
        }

        SafeComHolder<IHostTask> pHostTaskHolder(curHostTask);
        {
            ThreadStoreLockHolder TSLockHolder(TRUE);
            _ASSERTE(pThread == NULL);
            while ((pThread = ThreadStore::s_pThreadStore->GetAllThreadList(pThread, Thread::TS_Unstarted | Thread::TS_FailStarted, Thread::TS_Unstarted)) != NULL)
            {
                if (curHostTask)
                {
                    if (curHostTask == pThread->GetHostTask())
                    {
                        break;
                    }
                }
                else if (pThread->GetOSThreadId() == ourOSThreadId)
                {
                    break;
                }
            }

            if (pThread != NULL)
            {
                STRESS_LOG2(LF_SYNC, LL_INFO1000, "T::ST - recycling thread 0x%p (state: 0x%x)\n", pThread, pThread->m_State);
            }
        }

        // It's perfectly reasonable to not find this guy.  It's just an unrelated
        // thread spinning up.
        if (pThread)
        {
            BOOL fStatus = pThread->HasStarted();
            ensurePreemptive.SuppressRelease();
            return fStatus ? pThread : NULL;
        }
    }

    // First time we've seen this thread in the runtime:
    pThread = new Thread();

    Holder<Thread*,DoNothing<Thread*>,DeleteThread> threadHolder(pThread);

    CExecutionEngine::SetupTLSForThread(pThread);

    if (!pThread->InitThread(fInternal) ||
        !pThread->PrepareApartmentAndContext())
        ThrowOutOfMemory();

    // make sure we will not fail when we store in TLS in the future.
    if (!UnsafeTlsSetValue(gThreadTLSIndex, NULL))
    {
        ThrowOutOfMemory();
    }
    if (!UnsafeTlsSetValue(GetAppDomainTLSIndex(), NULL))
    {
        ThrowOutOfMemory();
    }

    // reset any unstarted bits on the thread object
    FastInterlockAnd((ULONG *) &pThread->m_State, ~Thread::TS_Unstarted);
    FastInterlockOr((ULONG *) &pThread->m_State, Thread::TS_LegalToJoin);

    ThreadStore::AddThread(pThread);

    BOOL fOK = UnsafeTlsSetValue(gThreadTLSIndex, (VOID*)pThread);
    _ASSERTE (fOK);
    fOK = UnsafeTlsSetValue(GetAppDomainTLSIndex(), (VOID*)pThread->GetDomain());
    _ASSERTE (fOK);

    // We now have a Thread object visable to the RS. unmark special status.
    hCantStop.Release();

    pThread->SetupThreadForHost();

    threadHolder.SuppressRelease();

#ifdef _DEBUG
    pThread->AddFiberInfo(Thread::ThreadTrackInfo_Lifetime);
#endif

#ifdef DEBUGGING_SUPPORTED
    //
    // If we're debugging, let the debugger know that this
    // thread is up and running now.
    //
    if (CORDebuggerAttached())
    {
        g_pDebugInterface->ThreadCreated(pThread);
    }
    else
    {
        LOG((LF_CORDB, LL_INFO10000, "ThreadCreated() not called due to CORDebuggerAttached() being FALSE for thread 0x%x\n", pThread->GetThreadId()));
    }
#endif // DEBUGGING_SUPPORTED

#ifdef PROFILING_SUPPORTED
    // If a profiler is present, then notify the profiler that a
    // thread has been created.
    if (CORProfilerTrackThreads())
    {
        {
            // Must be in preemptive mode, since the thread create / destroyed callbacks
            // into the profiler can and should block.   See comment above
            // g_profControlBlock.pProfInterface->ThreadDestroyed() in Thread::OnThreadTerminate
            GCX_PREEMP();

            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ThreadCreated(
                (ThreadID)pThread);
        }

        DWORD osThreadId = ::GetCurrentThreadId();
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->ThreadAssignedToOSThread(
            (ThreadID)pThread, osThreadId);
    }
#endif // PROFILING_SUPPORTED

    _ASSERTE(!pThread->IsBackground()); // doesn't matter, but worth checking
    pThread->SetBackground(TRUE);


    ensurePreemptive.SuppressRelease();

    return pThread;
}

//-------------------------------------------------------------------------
// Public function: SetupThreadPoolThread()
// Just like SetupThread, but also sets a bit to indicate that this is a threadpool thread
Thread* SetupThreadPoolThread(ThreadpoolThreadType typeTPThread)
{
    CONTRACTL {
        THROWS;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    _ASSERTE(ThreadInited());
    Thread* pThread;

    if (NULL == (pThread = GetThread()))
    {
        pThread = SetupInternalThread();
    }
    if ((pThread != NULL) && ((pThread->m_State & Thread::TS_ThreadPoolThread) == 0))
    {

        if (typeTPThread == WorkerThread)
        {
            FastInterlockOr((ULONG *) &pThread->m_State, Thread::TS_ThreadPoolThread | Thread::TS_TPWorkerThread);
        }
        else if (typeTPThread == CompletionPortThread)
        {
            FastInterlockOr ((ULONG *) &pThread->m_State, Thread::TS_ThreadPoolThread | Thread::TS_CompletionPortThread);
        }
        else
        {
            FastInterlockOr((ULONG *) &pThread->m_State, Thread::TS_ThreadPoolThread);
        }
    }
    if (pThread != NULL)
    {
        pThread->SetBackground(TRUE);
    }
    return pThread;
}

Thread* SetupThreadPoolThreadNoThrow(ThreadpoolThreadType typeTPThread, HRESULT *pHR)
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(SOToleranceViolation);

    Thread *pThread = NULL;
    if (pHR)
    {
        *pHR = S_OK;
    }

    EX_TRY
    {
        pThread = SetupThreadPoolThread(typeTPThread);
    }
    EX_CATCH
    {
        if (pHR)
        {
            *pHR = GET_EXCEPTION()->GetHR();
        }
    }
    EX_END_CATCH(SwallowAllExceptions);

    return pThread;
}

void STDMETHODCALLTYPE CorMarkThreadInThreadPool()
{
    LEAF_CONTRACT;
    BEGIN_ENTRYPOINT_VOIDRET;
    END_ENTRYPOINT_VOIDRET;

    // this is no longer needed after our switch to
    // the Win32 threadpool.
    // keeping in mscorwks for compat reasons and to keep rotor sscoree and
    // mscoree consistent.
}


//-------------------------------------------------------------------------
// Public function: SetupUnstartedThread()
// This sets up a Thread object for an exposed System.Thread that
// has not been started yet.  This allows us to properly enumerate all threads
// in the ThreadStore, so we can report on even unstarted threads.  Clearly
// there is no physical thread to match, yet.
//
// When there is, complete the setup with Thread::HasStarted()
//-------------------------------------------------------------------------
Thread* SetupUnstartedThread()
{
    CONTRACTL {
        THROWS;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    _ASSERTE(ThreadInited());
    Thread* pThread = new Thread();

    if (pThread)
    {
        FastInterlockOr((ULONG *) &pThread->m_State,
                        (Thread::TS_Unstarted | Thread::TS_WeOwn));

        ThreadStore::AddThread(pThread);
    }

    return pThread;
}


//-------------------------------------------------------------------------
// Public function: DestroyThread()
// Destroys the specified Thread object, for a thread which is about to die.
//-------------------------------------------------------------------------
void DestroyThread(Thread *th)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE (th == GetThread());

    _ASSERTE(g_fEEShutDown || th->m_dwLockCount == 0 || th->m_fRudeAborted);

    th->FinishSOWork();

    if (th->PreemptiveGCDisabled())
    {
        th->EnablePreemptiveGC();
    }

    if (th->IsAbortRequested()) {
        // Reset trapping count.
        th->UnmarkThreadForAbort(Thread::TAR_ALL);
    }


    if (th->GetHostTask() == NULL && g_fEEShutDown == 0) {
        th->OnThreadTerminate(FALSE);
    }
}

//-------------------------------------------------------------------------
// Public function: DetachThread()
// Marks the thread as needing to be destroyed, but doesn't destroy it yet.
//-------------------------------------------------------------------------
HRESULT Thread::DetachThread(BOOL fDLLThreadDetach)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    BEGIN_CONTRACT_VIOLATION(SOToleranceViolation);


    _ASSERTE(!PreemptiveGCDisabled());
    _ASSERTE(g_fEEShutDown || m_dwLockCount == 0 || m_fRudeAborted);

    _ASSERTE ((m_State & Thread::TS_Detached) == 0);

    _ASSERTE (this == GetThread());

    FinishSOWork();

    FastInterlockIncrement(&Thread::m_DetachCount);

    if (IsAbortRequested()) {
        // Reset trapping count.
        UnmarkThreadForAbort(Thread::TAR_ALL);
    }

    if (!IsBackground())
    {
        FastInterlockIncrement(&Thread::m_ActiveDetachCount);
        ThreadStore::CheckForEEShutdown();
    }

    END_CONTRACT_VIOLATION;

    InternalSwitchOut();


    FastInterlockOr((ULONG*)&m_State, (long) (Thread::TS_Detached | Thread::TS_ReportDead));
    // Do not touch Thread object any more.  It may be destroyed.

    return S_OK;
}


//-------------------------------------------------------------------------
// Public function: GetThread()
// Returns Thread for current thread. Cannot fail since it's illegal to call this
// without having called SetupThread.
//-------------------------------------------------------------------------
#if USE_INDIRECT_GET_THREAD_APPDOMAIN
Thread* __stdcall DummyGetThread();

Thread* __stdcall DummyGetThread()
{
    LEAF_CONTRACT;

    return NULL;
}

POPTIMIZEDTHREADGETTER GetThread = DummyGetThread;    // Points to platform-optimized GetThread() function.
#else // USE_INDIRECT_GET_THREAD_APPDOMAIN
EXTERN_C Thread* GetThread();
#endif // USE_INDIRECT_GET_THREAD_APPDOMAIN

//---------------------------------------------------------------------------
// Returns the TLS index for the Thread. This is strictly for the use of
// our ASM stub generators that generate inline code to access the Thread.
// Normally, you should use GetThread().
//---------------------------------------------------------------------------
DWORD GetThreadTLSIndex()
{
    LEAF_CONTRACT;

    return gThreadTLSIndex;
}

#if USE_INDIRECT_GET_THREAD_APPDOMAIN
//-------------------------------------------------------------------------
// Public function: GetAppDomain()
// Returns AppDomain for current thread. Cannot fail since it's illegal to call this
// without having called SetupThread.
//-------------------------------------------------------------------------
POPTIMIZEDAPPDOMAINGETTER GetAppDomain = NULL;   // Points to platform-optimized GetAppDomain() function.
#else // USE_INDIRECT_GET_THREAD_APPDOMAIN
EXTERN_C AppDomain* GetAppDomain();
#endif // USE_INDIRECT_GET_THREAD_APPDOMAIN

//---------------------------------------------------------------------------
// Returns the TLS index for the AppDomain. This is strictly for the use of
// our ASM stub generators that generate inline code to access the AppDomain.
// Normally, you should use GetAppDomain().
//---------------------------------------------------------------------------
DWORD GetAppDomainTLSIndex()
{
    LEAF_CONTRACT;

    return gAppDomainTLSIndex;
}

#ifdef _DEBUG
DWORD_PTR Thread::OBJREF_HASH = OBJREF_TABSIZE;
#endif


#if USE_INDIRECT_GET_THREAD_APPDOMAIN
#if !defined(_X86_) && !defined(_WIN64)
// The x86 equivalents are in i386\asmhelpers.asm
// The x64 equivalents are in amd64\TlsGetters.asm
// The IA64 equivalents are in ia64\TlsGetters.s
Thread* GetThreadGeneric()
{
    // Can not have contract here.  Contract depends on GetThreadGeneric.
    // Contract here causes stack overflow.
    //CONTRACTL {
    //    NOTHROW;
    //    GC_NOTRIGGER;
    //}
    //CONTRACTL_END;

    DWORD lastError = GetLastError();

    _ASSERTE(ThreadInited());

    Thread* pThread = (Thread*)UnsafeTlsGetValue(gThreadTLSIndex);

    SetLastError(lastError);

    return pThread;
}

AppDomain* GetAppDomainGeneric()
{
    // No contract.  This function is called during ExitTask.
    //CONTRACTL {
    //    NOTHROW;
    //    GC_NOTRIGGER;
    //}
    //CONTRACTL_END;

    DWORD lastError = GetLastError();

    _ASSERTE(ThreadInited());

    AppDomain* pAppDomain = (AppDomain*)UnsafeTlsGetValue(GetAppDomainTLSIndex());

    SetLastError(lastError);

    return pAppDomain;
}
#endif //!defined(_X86_) && !defined(_AMD64_)

#else // USE_INDIRECT_GET_THREAD_APPDOMAIN

// the only place where we are not using the indirect GetThread() and
// GetAppDomain() right now is AMD64, these need to be implemented for
// other platforms which use that.
#endif // !USE_INDIRECT_GET_THREAD_APPDOMAIN

//---------------------------------------------------------------------------
// One-time initialization. Called during Dll initialization. So
// be careful what you do in here!
//---------------------------------------------------------------------------
void InitThreadManager()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("Threads");

    _ASSERTE(gThreadTLSIndex == TLS_OUT_OF_INDEXES);
    _ASSERTE(g_TrapReturningThreads == 0);

#ifdef _DEBUG
    // Randomize OBJREF_HASH to handle hash collision.
    Thread::OBJREF_HASH = OBJREF_TABSIZE - (DbgGetEXETimeStamp()%10);
#endif // _DEBUG

    // No backout necessary - part of the one time global initialization
    gThreadTLSIndex = UnsafeTlsAlloc();
    if (gThreadTLSIndex == TLS_OUT_OF_INDEXES)
        COMPlusThrowWin32();

#if !USE_INDIRECT_GET_THREAD_APPDOMAIN
    DWORD dwOldProtect;
    if (!ClrVirtualProtect(GetThread, FAST_TLS_GETTER_MAXSIZE, PAGE_EXECUTE_READWRITE, &dwOldProtect))
    {
        ThrowLastError();
    }

    // remove the "return NULL" at the beginning of the generic GetThread();
    FixupGenericTlsGetter(GetThread, GetThread_PrologStart);

#if defined(_DEBUG)
    if (0 != g_pConfig->GetConfigDWORD(L"UseGenericTlsGetters", 0))
#endif // _DEBUG
    {
        _ASSERTE(((BYTE*)GetThread_PrologStart - (BYTE*)GetThread) >= FAST_TLS_GETTER_MAXSIZE);
        MakeOptimizedTlsGetter(gThreadTLSIndex, GetThread);
    }

    ClrVirtualProtect(GetThread, FAST_TLS_GETTER_MAXSIZE, dwOldProtect, &dwOldProtect);

    FlushInstructionCache(GetCurrentProcess(), GetThread, FAST_TLS_GETTER_MAXSIZE);

#else // USE_INDIRECT_GET_THREAD_APPDOMAIN
    POPTIMIZEDTHREADGETTER threadgetter;
    threadgetter = (POPTIMIZEDTHREADGETTER)MakeOptimizedTlsGetter(gThreadTLSIndex);

#ifdef _DEBUG
    if (threadgetter && (0 != g_pConfig->GetConfigDWORD(L"UseGenericTlsGetters", 0)))
    {
        FreeOptimizedTlsGetter((POPTIMIZEDTLSGETTER)threadgetter);
        threadgetter = NULL;
    }
#endif // _DEBUG

    if (!threadgetter)
        threadgetter = GetThreadGeneric;
    GetThread = threadgetter;
#endif // USE_INDIRECT_GET_THREAD_APPDOMAIN


    // No backout necessary - part of the one time global initialization
    gAppDomainTLSIndex = UnsafeTlsAlloc();
    if (gAppDomainTLSIndex == TLS_OUT_OF_INDEXES)
        COMPlusThrowWin32();

#if !USE_INDIRECT_GET_THREAD_APPDOMAIN
    dwOldProtect;
    if (!ClrVirtualProtect(GetAppDomain, FAST_TLS_GETTER_MAXSIZE, PAGE_EXECUTE_READWRITE, &dwOldProtect))
    {
        ThrowLastError();
    }

    // remove the "return NULL" at the beginning of the generic GetAppDomain();
    FixupGenericTlsGetter(GetAppDomain, GetAppDomain_PrologStart);

#if !defined(_DEBUG)
    if (0 != g_pConfig->GetConfigDWORD(L"UseGenericTlsGetters", 0))
#endif // _DEBUG
    {
        _ASSERTE(((BYTE*)GetAppDomain_PrologStart - (BYTE*)GetAppDomain) >= FAST_TLS_GETTER_MAXSIZE);
        MakeOptimizedTlsGetter(gAppDomainTLSIndex, GetAppDomain);
    }

    ClrVirtualProtect(GetAppDomain, FAST_TLS_GETTER_MAXSIZE, dwOldProtect, &dwOldProtect);

    FlushInstructionCache(GetCurrentProcess(), GetAppDomain, FAST_TLS_GETTER_MAXSIZE);

#else // USE_INDIRECT_GET_THREAD_APPDOMAIN
    POPTIMIZEDAPPDOMAINGETTER appdomaingetter;
    appdomaingetter = (POPTIMIZEDAPPDOMAINGETTER)MakeOptimizedTlsGetter(gAppDomainTLSIndex);

#ifdef _DEBUG
    if (appdomaingetter && (0 != g_pConfig->GetConfigDWORD(L"UseGenericTlsGetters", 0)))
    {
        FreeOptimizedTlsGetter((POPTIMIZEDTLSGETTER)appdomaingetter);
        appdomaingetter = NULL;
    }
#endif // _DEBUG

    if (!appdomaingetter)
        appdomaingetter = GetAppDomainGeneric;
    GetAppDomain = appdomaingetter;
#endif // USE_INDIRECT_GET_THREAD_APPDOMAIN


    ThreadStore::InitThreadStore();

    g_DeadlockAwareCrst.Init("Deadlock Detection Crst", CrstDeadlockDetection, CRST_UNSAFE_ANYMODE);
}


//************************************************************************
// Thread members
//************************************************************************


#if defined(_DEBUG) && defined(TRACK_SYNC)

// One outstanding synchronization held by this thread:
struct Dbg_TrackSyncEntry
{
    UINT_PTR     m_caller;
    AwareLock   *m_pAwareLock;

    BOOL        Equiv      (UINT_PTR caller, void *pAwareLock)
    {
        LEAF_CONTRACT;

        return (m_caller == caller) && (m_pAwareLock == pAwareLock);
    }

    BOOL        Equiv      (void *pAwareLock)
    {
        LEAF_CONTRACT;

        return (m_pAwareLock == pAwareLock);
    }
};

// Each thread has a stack that tracks all enter and leave requests
struct Dbg_TrackSyncStack : public Dbg_TrackSync
{
    enum
    {
        MAX_TRACK_SYNC  = 20,       // adjust stack depth as necessary
    };

    void    EnterSync  (UINT_PTR caller, void *pAwareLock);
    void    LeaveSync  (UINT_PTR caller, void *pAwareLock);

    Dbg_TrackSyncEntry  m_Stack [MAX_TRACK_SYNC];
    UINT_PTR            m_StackPointer;
    BOOL                m_Active;

    Dbg_TrackSyncStack() : m_StackPointer(0),
                           m_Active(TRUE)
    {
        LEAF_CONTRACT;
    }
};

// ensure that registers are preserved across this call
#ifdef _MSC_VER
#pragma optimize("", off)
#endif
// A pain to do all this from ASM, but watch out for trashed registers
EXTERN_C void EnterSyncHelper    (UINT_PTR caller, void *pAwareLock)
{
    BEGIN_ENTRYPOINT_THROWS;
    WRAPPER_CONTRACT;
    GetThread()->m_pTrackSync->EnterSync(caller, pAwareLock);
    END_ENTRYPOINT_THROWS;

}
EXTERN_C void LeaveSyncHelper    (UINT_PTR caller, void *pAwareLock)
{
    BEGIN_ENTRYPOINT_THROWS;
    WRAPPER_CONTRACT;
    GetThread()->m_pTrackSync->LeaveSync(caller, pAwareLock);
    END_ENTRYPOINT_THROWS;

}
#ifdef _MSC_VER
#pragma optimize("", on)
#endif

void Dbg_TrackSyncStack::EnterSync(UINT_PTR caller, void *pAwareLock)
{
    LEAF_CONTRACT;

    STRESS_LOG4(LF_SYNC, LL_INFO100, "Dbg_TrackSyncStack::EnterSync, IP=%p, Recursion=%d, MonitorHeld=%d, HoldingThread=%p.\n",
                    caller,
                    ((AwareLock*)pAwareLock)->m_Recursion,
                    ((AwareLock*)pAwareLock)->m_MonitorHeld,
                    ((AwareLock*)pAwareLock)->m_HoldingThread );

    if (m_Active)
    {
        if (m_StackPointer >= MAX_TRACK_SYNC)
        {
            _ASSERTE(!"Overflowed synchronization stack checking.  Disabling");
            m_Active = FALSE;
            return;
        }
    }
    m_Stack[m_StackPointer].m_caller = caller;
    m_Stack[m_StackPointer].m_pAwareLock = (AwareLock *) pAwareLock;

    m_StackPointer++;

}

void Dbg_TrackSyncStack::LeaveSync(UINT_PTR caller, void *pAwareLock)
{
    WRAPPER_CONTRACT;

    STRESS_LOG4(LF_SYNC, LL_INFO100, "Dbg_TrackSyncStack::LeaveSync, IP=%p, Recursion=%d, MonitorHeld=%d, HoldingThread=%p.\n",
                    caller,
                    ((AwareLock*)pAwareLock)->m_Recursion,
                    ((AwareLock*)pAwareLock)->m_MonitorHeld,
                    ((AwareLock*)pAwareLock)->m_HoldingThread );

    if (m_Active)
    {
        if (m_StackPointer == 0)
            _ASSERTE(!"Underflow in leaving synchronization");
        else
        if (m_Stack[m_StackPointer - 1].Equiv(pAwareLock))
        {
            m_StackPointer--;
        }
        else
        {
            for (int i=m_StackPointer - 2; i>=0; i--)
            {
                if (m_Stack[i].Equiv(pAwareLock))
                {
                    _ASSERTE(!"Locks are released out of order.  This might be okay...");
                    memcpy(&m_Stack[i], &m_Stack[i+1],
                           sizeof(m_Stack[0]) * (m_StackPointer - i - 1));

                    return;
                }
            }
            _ASSERTE(!"Trying to release a synchronization lock which isn't held");
        }
    }
}

#endif  // TRACK_SYNC


static  DWORD dwHashCodeSeed = 123456789;

void DisposeThreadId(DWORD id)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    g_pThinLockThreadIdDispenser->DisposeId(id);
}

#ifdef _DEBUG
void CheckADValidity(AppDomain* pDomain, DWORD ADValidityKind)
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(FaultViolation);
    if  (::GetAppDomain()==pDomain)
        return;
    if ((ADValidityKind &  ADV_DEFAULTAD) &&
        pDomain->IsDefaultDomain())
       return;
    if ((ADValidityKind &  ADV_ITERATOR) &&
        pDomain->IsHeldByIterator())
       return;
    if ((ADValidityKind &  ADV_CREATING) &&
        pDomain->IsBeingCreated())
       return;
    if ((ADValidityKind &  ADV_COMPILATION) &&
        pDomain->IsCompilationDomain())
       return;
    if ((ADValidityKind &  ADV_FINALIZER) &&
        IsFinalizerThread())
       return;
    if ((ADValidityKind &  ADV_ADUTHREAD) &&
        IsADUnloadHelperThread())
       return;
    if ((ADValidityKind &  ADV_RUNNINGIN) &&
        pDomain->IsRunningIn(GetThread()))
       return;
    if ((ADValidityKind &  ADV_REFTAKER) &&
        pDomain->IsHeldByRefTaker())
       return;

    _ASSERTE(!"Appdomain* can be invalid");
}
#endif


typedef Holder<DWORD,DoNothing<DWORD>,DisposeThreadId> IDHolder;

//--------------------------------------------------------------------
// Thread construction
//--------------------------------------------------------------------
Thread::Thread()
{
    CONTRACTL {
        THROWS;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    m_pFrame                = FRAME_TOP;
    m_pUnloadBoundaryFrame  = NULL;
    m_pRoot                 = NULL;

    m_fPreemptiveGCDisabled = 0;

#ifdef _DEBUG
    m_ulForbidTypeLoad      = 0;
    m_GCOnTransitionsOK     = TRUE;
#endif



    m_dwLockCount = 0;
    m_dwNonHostLockCount = 0;
    m_dwBeginLockCount = 0;
    m_dwBeginCriticalRegionCount = 0;
    m_dwCriticalRegionCount = 0;
    m_dwThreadAffinityCount = 0;
    m_dwDelayAbortCount = 0;

#ifdef _DEBUG
    m_dwSuspendThread = 0;
    m_Creater.ResetThreadId();
    m_dwUnbreakableLockCount = 0;
#endif

    m_dwForbidSuspendThread = 0;

    // Initialize lock state
    m_pHead = &m_embeddedEntry;
    m_embeddedEntry.pNext = m_pHead;
    m_embeddedEntry.pPrev = m_pHead;
    m_embeddedEntry.dwLLockID = 0;
    m_embeddedEntry.dwULockID = 0;
    m_embeddedEntry.wReaderLevel = 0;

    m_pBlockingLock = NULL;

    m_alloc_context.init();

    m_UserInterrupt = 0;
    m_WaitEventLink.m_Next = NULL;
    m_WaitEventLink.m_LinkSB.m_pNext = NULL;
    m_ThreadHandle = INVALID_HANDLE_VALUE;
    m_ThreadHandleForClose = INVALID_HANDLE_VALUE;
    m_ThreadHandleForResume = INVALID_HANDLE_VALUE;
    m_WeOwnThreadHandle = FALSE;
    m_ThreadId = g_pThinLockThreadIdDispenser->NewId(this);
    IDHolder idHolder(m_ThreadId);

    // Initialize this variable to a very different start value for each thread
    // Using linear congruential generator from Knuth Vol. 2, p. 102, line 24
    dwHashCodeSeed = dwHashCodeSeed * 1566083941 + 1;
    m_dwHashCodeSeed = dwHashCodeSeed;

    m_hijackLock = FALSE;

    m_OSThreadId = 0;
    m_Priority = INVALID_THREAD_PRIORITY;
    m_ExternalRefCount = 1;
    m_UnmanagedRefCount = 0;
    m_State = TS_Unstarted;
    m_StateNC = TSNC_Unknown;

    // It can't be a LongWeakHandle because we zero stuff out of the exposed
    // object as it is finalized.  At that point, calls to GetCurrentThread()
    // had better get a new one,!
    m_ExposedObject = CreateGlobalShortWeakHandle(NULL);

    GlobalShortWeakHandleHolder exposedObjectHolder(m_ExposedObject);

    m_StrongHndToExposedObject = CreateGlobalStrongHandle(NULL);
    GlobalStrongHandleHolder strongHndToExposedObjectHolder(m_StrongHndToExposedObject);

    m_LastThrownObjectHandle = NULL;


    m_AbortReason = NULL;

    m_debuggerFilterContext = NULL;
    m_debuggerCantStop = 0;
    m_debuggerWord = 0;
    m_fInteropDebuggingHijacked = FALSE;
    m_profilerCallbackState = 0;
    
    m_pProfilerFilterContext = NULL;

#ifdef _DEBUG
    m_pCleanedStackBase = NULL;

#ifdef STACK_GUARDS_DEBUG
    m_pCurrentStackGuard = NULL;
#endif

#endif

#if defined(_DEBUG) && defined(TRACK_SYNC)
    m_pTrackSync = new Dbg_TrackSyncStack;
    NewHolder<Dbg_TrackSyncStack> trackSyncHolder(static_cast<Dbg_TrackSyncStack*>(m_pTrackSync));
#endif  // TRACK_SYNC

    m_RequestedStackSize = 0;
    m_PreventAsync = 0;
    m_PreventThreadAbort = 0;
    m_nNestedMarshalingExceptions = 0;
    m_pDomain = NULL;
    m_Context = NULL;
    m_TraceCallCount = 0;
    m_ThrewControlForThread = 0;
    m_OSContext = NULL;
    m_ThreadTasks = (ThreadTasks)0;
    m_pLoadLimiter= NULL;
    m_pLoadingFile = NULL;

    Thread *pThread = GetThread();
    _ASSERTE(SystemDomain::System()->DefaultDomain()->GetDefaultContext());
    InitContext();
    _ASSERTE(m_Context);
    if (pThread)
    {
        _ASSERTE(pThread->GetDomain() && pThread->GetDomain()->GetDefaultContext());
        // Start off the new thread in the default context of
        // the creating thread's appDomain. This could be changed by SetDelegate
        SetKickOffDomainId(pThread->GetDomain()->GetId());
    } else
        SetKickOffDomainId((ADID)DefaultADID);

    // The state and the tasks must be 32-bit aligned for atomicity to be guaranteed.
    _ASSERTE((((size_t) &m_State) & 3) == 0);
    _ASSERTE((((size_t) &m_ThreadTasks) & 3) == 0);

    // Track perf counter for the logical thread object.
    COUNTER_ONLY(GetPrivatePerfCounters().m_LocksAndThreads.cCurrentThreadsLogical++);

#ifdef STRESS_HEAP
        // ON all callbacks, call the trap code, which we now have
        // wired to cause a GC.  THus we will do a GC on all Transition Frame Transitions (and more).
   if (g_pConfig->GetGCStressLevel() & EEConfig::GCSTRESS_TRANSITION)
        m_State = (ThreadState) (m_State | TS_GCOnTransitions);
#endif

    m_pSharedStaticData = NULL;
    m_pUnsharedStaticData = NULL;
    m_pStaticDataHash = NULL;
    m_pSDHCrst = NULL;
    m_pDLSHash = NULL;

    m_fSecurityStackwalk = FALSE;

    m_AbortType = EEPolicy::TA_None;
    m_AbortInfo = 0;
    m_AbortEndTime = MAXULONGLONG;
    m_RudeAbortEndTime = MAXULONGLONG;
    m_AbortController = 0;
    m_AbortRequestLock = 0;
    m_fRudeAbortInitiated = FALSE;

#ifdef _DEBUG
    m_fRudeAborted = FALSE;
    m_dwAbortPoint = 0;
#endif

#ifdef STRESS_THREAD
    m_stressThreadCount = -1;
#endif

    m_pHostTask = NULL;
    m_pFiberData = NULL;

    m_TaskId = INVALID_TASK_ID;
    m_dwConnectionId = INVALID_CONNECTION_ID;

#ifdef _DEBUG
    DWORD_PTR *ttInfo = NULL;
    size_t nBytes = MaxThreadRecord *
                  (sizeof(FiberSwitchInfo)-sizeof(size_t)+MaxStackDepth*sizeof(size_t));
    if (CLRTaskHosted() || g_pConfig->SaveThreadInfo()) {
        ttInfo = new DWORD_PTR[(nBytes/sizeof(DWORD_PTR))*ThreadTrackInfo_Max];
        memset(ttInfo,0,nBytes*ThreadTrackInfo_Max);
    }
    for (DWORD i = 0; i < ThreadTrackInfo_Max; i ++)
    {
        m_FiberInfoIndex[i] = 0;
        m_pFiberInfo[i] = (FiberSwitchInfo*)((DWORD_PTR)ttInfo + i*nBytes);
    }
    NewArrayHolder<DWORD_PTR> fiberInfoHolder(ttInfo);
#endif

    m_OSContext = new CONTEXT;
    NewHolder<CONTEXT> contextHolder(m_OSContext);

    if (CLRTaskHosted())
    {
        m_pSavedRedirectContext = new CONTEXT;
    }
    else
    {
        m_pSavedRedirectContext = NULL;
    }
    NewHolder<CONTEXT> savedRedirectContextHolder(m_pSavedRedirectContext);


    idHolder.SuppressRelease();
    exposedObjectHolder.SuppressRelease();
    strongHndToExposedObjectHolder.SuppressRelease();
#if defined(_DEBUG) && defined(TRACK_SYNC)
    trackSyncHolder.SuppressRelease();
#endif
#ifdef _DEBUG
    fiberInfoHolder.SuppressRelease();
#endif

    contextHolder.SuppressRelease();
    savedRedirectContextHolder.SuppressRelease();

    m_dwPrepareCer = 0;
#ifdef _DEBUG
    m_bGCStressing = FALSE;
    m_bUniqueStacking = FALSE;
#endif


    m_dwAVInRuntimeImplOkayCount = 0;


#ifdef _DEBUG
    m_pHelperMethodFrameCallerList = (HelperMethodFrameCallerList*)-1;
#endif

    m_dwHostTaskRefCount = 0;

    m_pExceptionDuringStartup = NULL;

#if defined(STRESS_HEAP) && defined(_DEBUG)
    m_pbDestCode = NULL;
    m_pbSrcCode = NULL;
#endif // defined(STRESS_HEAP) && defined(_DEBUG)

    m_fCompletionPortDrained = FALSE;

    m_SOExceptionInfo.ContextRecord = NULL;
    m_SOExceptionInfo.ExceptionRecord = NULL;

    m_WorkingOnThreadContext = NULL;
    m_dwThreadHandleBeingUsed = 0;
    SetProfilerCallbacksAllowed(TRUE);
}


//--------------------------------------------------------------------
// Failable initialization occurs here.
//--------------------------------------------------------------------
BOOL Thread::InitThread(BOOL fInternal)
{
    CONTRACTL {
        THROWS;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    IHostTaskManager *provider = CorHost2::GetHostTaskManager();
    if (provider) {
        if (m_pHostTask == NULL)
        {
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
            provider->GetCurrentTask(&m_pHostTask);
            END_SO_TOLERANT_CODE_CALLING_HOST;
        }
        if (m_pHostTask == NULL && !fInternal) {
            ThrowHR(HOST_E_INVALIDOPERATION);
        }
    }

    HANDLE  hDup = INVALID_HANDLE_VALUE;
    BOOL    ret = TRUE;

        // This message actually serves a purpose (which is why it is always run)
        // The Stress log is run during hijacking, when other threads can be suspended
        // at arbitrary locations (including when holding a lock that NT uses to serialize
        // all memory allocations).  By sending a message now, we insure that the stress
        // log will not allocate memory at these critical times an avoid deadlock.
    STRESS_LOG2(LF_ALWAYS, LL_ALWAYS, "SetupThread  managed Thread %p Thread Id = %x\n", this, m_ThreadId);

    COUNTER_ONLY(GetPrivatePerfCounters().m_LocksAndThreads.cRecognizedThreads++);


    if (m_ThreadHandle == INVALID_HANDLE_VALUE)
    {
        // For WinCE, all clients have the same handle for a thread.  Duplication is
        // not possible.  We make sure we never close this handle unless we created
        // the thread (TS_WeOwn).
        //
        // For Win32, each client has its own handle.  This is achieved by duplicating
        // the pseudo-handle from ::GetCurrentThread().  Unlike WinCE, this service
        // returns a pseudo-handle which is only useful for duplication.  In this case
        // each client is responsible for closing its own (duplicated) handle.
        //
        // We don't bother duplicating if WeOwn, because we created the handle in the
        // first place.
        // Thread is created when or after the physical thread started running
        HANDLE curProcess = ::GetCurrentProcess();


        if (::DuplicateHandle(curProcess, ::GetCurrentThread(), curProcess, &hDup,
                              0 /*ignored*/, FALSE /*inherit*/, DUPLICATE_SAME_ACCESS))
        {
            _ASSERTE(hDup != INVALID_HANDLE_VALUE);

            SetThreadHandle(hDup);
            m_WeOwnThreadHandle = TRUE;
        }
        else
        {
            ret = FALSE;
            goto leav;
        }
    }

    if ((m_State & TS_WeOwn) == 0) {
        if (!AllocHandles())
        {
            ret = FALSE;
            goto leav;
        }
    }

    _ASSERTE(HasValidThreadHandle());

    m_pExceptionList = PAL_GetBottommostRegistrationPtr();


leav:
    return ret;
}

// Allocate all the handles.  When we are kicking of a new thread, we can call
// here before the thread starts running.
BOOL Thread::AllocHandles()
{
    WRAPPER_CONTRACT;

    _ASSERTE(!m_SafeEvent.IsValid());
    _ASSERTE(!m_UserSuspendEvent.IsValid());
    _ASSERTE(!m_DebugSuspendEvent.IsValid());
    _ASSERTE(!m_EventWait.IsValid());

    BOOL fOK = TRUE;
    EX_TRY {
        // create a manual reset event for getting the thread to a safe point
        m_SafeEvent.CreateManualEvent(FALSE);
        m_UserSuspendEvent.CreateManualEvent(FALSE);
        m_DebugSuspendEvent.CreateManualEvent(FALSE);
        m_EventWait.CreateManualEvent(TRUE);
    }
    EX_CATCH {
        fOK = FALSE;
        if (!m_SafeEvent.IsValid()) {
            m_SafeEvent.CloseEvent();
        }

        if (!m_UserSuspendEvent.IsValid()) {
            m_UserSuspendEvent.CloseEvent();
        }

        if (!m_DebugSuspendEvent.IsValid()) {
            m_DebugSuspendEvent.CloseEvent();
        }

        if (!m_EventWait.IsValid()) {
            m_EventWait.CloseEvent();
        }
    }
    EX_END_CATCH(RethrowTerminalExceptions);

    return fOK;
}


//--------------------------------------------------------------------
// This is the alternate path to SetupThread/InitThread.  If we created
// an unstarted thread, we have SetupUnstartedThread/HasStarted.
//--------------------------------------------------------------------
BOOL Thread::HasStarted()
{
    CONTRACTL {
        NOTHROW;
        DISABLED(GC_NOTRIGGER);
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(SOToleranceViolation);

    _ASSERTE(!m_fPreemptiveGCDisabled);     // can't use PreemptiveGCDisabled() here

    // This is cheating a little.  There is a pathway here from SetupThread, but only
    // via IJW SystemDomain::RunDllMain.  Normally SetupThread returns a thread in
    // preemptive mode, ready for a transition.  But in the IJW case, it can return a
    // cooperative mode thread.  RunDllMain handles this "surprise" correctly.
    m_fPreemptiveGCDisabled = TRUE;

    // Normally, HasStarted is called from the thread's entrypoint to introduce it to
    // the runtime.  But sometimes that thread is used for DLL_THREAD_ATTACH notifications
    // that call into managed code.  In that case, the second HasStarted call is
    // redundant and should be ignored.
    if (GetThread() == this)
        return TRUE;


    _ASSERTE(GetThread() == 0);
    _ASSERTE(HasValidThreadHandle());

    BOOL    res = TRUE;
    BOOL    fKeepTLS = FALSE;

    // If any exception happens during HasStarted, we will cache the exception in Thread::m_pExceptionDuringStartup
    // which will be thrown in Thread.Start as an internal exception
    EX_TRY
    {
        CExecutionEngine::SetupTLSForThread(this);

        res = PrepareApartmentAndContext();
        if (res)
            res = InitThread(FALSE);

        if (UnsafeTlsSetValue(gThreadTLSIndex, (VOID*)this) == 0)
        {
            ThrowOutOfMemory();
        }
        if (UnsafeTlsSetValue(GetAppDomainTLSIndex(), (VOID*)m_pDomain) == 0)
        {
            ThrowOutOfMemory();
        }

#ifdef _DEBUG
        AddFiberInfo(Thread::ThreadTrackInfo_Lifetime);
#endif

        SetupThreadForHost();

        if (m_pHostTask)
        {
            // If we have notify host of ICLRTask, host will call ExitTask to release
            // its reference to ICLRTask.  Also host may call SwitchOut and SwitchIn.
            // ExitTask needs Thread in TLS.
            fKeepTLS = TRUE;
        }

        ThreadStore::TransferStartedThread(this);

    }
    EX_CATCH
    {
        if (!__pException.IsNull())
        {
            __pException.SuppressRelease();
            m_pExceptionDuringStartup = __pException;
        }
        res = FALSE;
    }
    EX_END_CATCH(SwallowAllExceptions);

    if (res == FALSE)
    {
        if (m_fPreemptiveGCDisabled)
        {
            m_fPreemptiveGCDisabled = FALSE;
        }
        _ASSERTE (HasThreadState(TS_Unstarted));
        if (!fKeepTLS)
        {
            FastInterlockDecrement(&ThreadStore::s_pThreadStore->m_PendingThreadCount);
            // One of the components of OtherThreadsComplete() has changed, so check whether
            // we should now exit the EE.
            ThreadStore::CheckForEEShutdown();
            DecExternalCount(FALSE);
            UnsafeTlsSetValue(gThreadTLSIndex, NULL);
            UnsafeTlsSetValue(GetAppDomainTLSIndex(), NULL);
        }
        SetThreadState(TS_FailStarted);
    }
    else
    {
#ifdef DEBUGGING_SUPPORTED
        //
        // If we're debugging, let the debugger know that this
        // thread is up and running now.
        //
        if (CORDebuggerAttached())
        {
            g_pDebugInterface->ThreadCreated(this);
        }
        else
        {
            LOG((LF_CORDB, LL_INFO10000, "ThreadCreated() not called due to CORDebuggerAttached() being FALSE for thread 0x%x\n", GetThreadId()));
        }

#endif // DEBUGGING_SUPPORTED

#ifdef PROFILING_SUPPORTED
        // If a profiler is running, let them know about the new thread.
        if (CORProfilerTrackThreads())
        {
            BOOL gcOnTransition = GC_ON_TRANSITIONS(FALSE);     // disable GCStress 2 to avoid the profiler receiving a RuntimeThreadSuspended notification even before the ThreadCreated notification
            {
                // Must be in preemptive mode, since the thread create / destroyed callbacks
                // into the profiler can and should block.   See comment above
                // g_profControlBlock.pProfInterface->ThreadDestroyed() in Thread::OnThreadTerminate
                GCX_PREEMP();
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->ThreadCreated((ThreadID) this);
            }
            GC_ON_TRANSITIONS(gcOnTransition);

            DWORD osThreadId = ::GetCurrentThreadId();
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ThreadAssignedToOSThread(
                (ThreadID) this, osThreadId);
        }
#endif // PROFILING_SUPPORTED

        // Is there a pending user suspension?
        if (m_State & TS_SuspendUnstarted)
        {
            BOOL    doSuspend = FALSE;

            {
                ThreadStoreLockHolder TSLockHolder(TRUE);

                // Perhaps we got resumed before it took effect?
                if (m_State & TS_SuspendUnstarted)
                {
                    FastInterlockAnd((ULONG *) &m_State, ~TS_SuspendUnstarted);
                    SetupForSuspension(TS_UserSuspendPending);
                    MarkForSuspension(TS_UserSuspendPending);
                    doSuspend = TRUE;
                }
            }

            if (doSuspend)
            {
                GCX_PREEMP();
                WaitSuspendEvents();
            }
        }
    }

    return res;
}


void Thread::HandleThreadStartupFailure()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(GetThread() != NULL);

    struct ProtectArgs
    {
        OBJECTREF pThrowable;
        OBJECTREF pReason;
    } args;
    memset(&args, 0, sizeof(ProtectArgs));

    GCPROTECT_BEGIN(args);

    MethodTable *pMT = g_Mscorlib.GetException(kThreadStartException);
    args.pThrowable = AllocateObject(pMT);

    MethodDescCallSite exceptionCtor(METHOD__THREAD_START_EXCEPTION__EX_CTOR);

    if (m_pExceptionDuringStartup)
    {
        args.pReason = CLRException::GetThrowableFromException(m_pExceptionDuringStartup);
        Exception::Delete(m_pExceptionDuringStartup);
        m_pExceptionDuringStartup = NULL;
    }

    ARG_SLOT args1[] = {
        ObjToArgSlot(args.pThrowable),
        ObjToArgSlot(args.pReason),
    };
    exceptionCtor.Call_RetArgSlot(args1);

    GCPROTECT_END(); //Prot

    RaiseTheExceptionInternalOnly(args.pThrowable, FALSE);
}



// We don't want ::CreateThread() calls scattered throughout the source.  So gather
// them all here.

BOOL Thread::CreateNewThread(SIZE_T stackSize, LPTHREAD_START_ROUTINE start, void *args)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;
    BOOL bRet;

    //This assert is here to prevent a bug in the future
    //  CreateTask currently takes a DWORD and we will downcast
    //  if that interface changes to take a SIZE_T this Assert needs to be removed.
    //
    _ASSERTE(stackSize <= 0xFFFFFFFF);


    m_StateNC = (ThreadStateNoConcurrency)((ULONG)m_StateNC | TSNC_CLRCreatedThread);
    if (!CLRTaskHosted()) {
        bRet = CreateNewOSThread(stackSize, start, args);
    }
    else {
        bRet = CreateNewHostTask(stackSize, start, args);
    }

    return bRet;
}


// This is to avoid the 64KB/1MB aliasing problem present on Pentium 4 processors,
// which can significantly impact performance with HyperThreading enabled
DWORD __stdcall Thread::intermediateThreadProc(PVOID arg)
{
    WRAPPER_CONTRACT;

    m_offset_counter++;
    if (m_offset_counter * offset_multiplier > PAGE_SIZE)
        m_offset_counter = 0;

    _alloca(m_offset_counter * offset_multiplier);

    intermediateThreadParam* param = (intermediateThreadParam*)arg;

    LPTHREAD_START_ROUTINE ThreadFcnPtr = param->lpThreadFunction;
    PVOID args = param->lpArg;
    delete param;

    return ThreadFcnPtr(args);
}


BOOL Thread::CreateNewOSThread(SIZE_T sizeToCommitOrReserve, LPTHREAD_START_ROUTINE start, void *args)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    DWORD   ourId = 0;
    HANDLE  h = NULL;
    DWORD dwCreationFlags = CREATE_SUSPENDED;


    intermediateThreadParam* lpThreadArgs = new (nothrow) intermediateThreadParam;
    if (lpThreadArgs == NULL)
    {
        return FALSE;
    }
    NewHolder<intermediateThreadParam> argHolder(lpThreadArgs);

    // Make sure we have all our handles, in case someone tries to suspend us
    // as we are starting up.
    if (!AllocHandles())
    {
        // OS is out of handles/memory?
        return FALSE;
    }

    lpThreadArgs->lpThreadFunction = start;
    lpThreadArgs->lpArg = args;
    h = ::CreateThread(NULL     /*=SECURITY_ATTRIBUTES*/,
                       sizeToCommitOrReserve,
                       intermediateThreadProc,
                       lpThreadArgs,
                       dwCreationFlags,
                       &ourId);

    if (h == NULL)
        return FALSE;

    argHolder.SuppressRelease();

    _ASSERTE(!m_fPreemptiveGCDisabled);     // leave in preemptive until HasStarted.

    SetThreadHandle(h);
    m_WeOwnThreadHandle = TRUE;

    // Before we do the resume, we need to take note of the new ThreadId.  This
    // is necessary because -- before the thread starts executing at KickofThread --
    // it may perform some DllMain DLL_THREAD_ATTACH notifications.  These could
    // call into managed code.  During the consequent SetupThread, we need to
    // perform the Thread::HasStarted call instead of going through the normal
    // 'new thread' pathway.
    _ASSERTE(GetOSThreadId() == 0);
    _ASSERTE(ourId != 0);

    m_OSThreadId = ourId;

    FastInterlockIncrement(&ThreadStore::s_pThreadStore->m_PendingThreadCount);

#ifdef _DEBUG
    m_Creater.SetThreadId();
#endif

    return TRUE;
}



BOOL Thread::CreateNewHostTask(SIZE_T stackSize, LPTHREAD_START_ROUTINE start, void *args)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Make sure we have all our handles, in case someone tries to suspend us
    // as we are starting up.

    if (!AllocHandles())
    {
        return FALSE;
    }

    IHostTask *pHostTask = NULL;

    if (CorHost2::GetHostTaskManager()) {
        //If you change this value to pass a SIZE_T stackSize you must
        //   remove this _ASSERTE(stackSize <= 0xFFFFFFFF); from
        //   CreateNewThread
        //
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = CorHost2::GetHostTaskManager()->CreateTask((DWORD)stackSize, start, args, &pHostTask);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (hr != S_OK)
        return FALSE;
    }

    _ASSERTE(!m_fPreemptiveGCDisabled);     // leave in preemptive until HasStarted.

    // Before we do the resume, we need to take note of the new ThreadId.  This
    // is necessary because -- before the thread starts executing at KickofThread --
    // it may perform some DllMain DLL_THREAD_ATTACH notifications.  These could
    // call into managed code.  During the consequent SetupThread, we need to
    // perform the Thread::HasStarted call instead of going through the normal
    // 'new thread' pathway.
    _ASSERTE(m_pHostTask == NULL);
    _ASSERTE(pHostTask != NULL);

    m_pHostTask = pHostTask;

    FastInterlockIncrement(&ThreadStore::s_pThreadStore->m_PendingThreadCount);

#ifdef _DEBUG
    m_Creater.SetThreadId();
#endif

    return TRUE;
}

// General comments on thread destruction.
//
// The C++ Thread object can survive beyond the time when the Win32 thread has died.
// This is important if an exposed object has been created for this thread.  The
// exposed object will survive until it is GC'ed.
//
// A client like an exposed object can place an external reference count on that
// object.  We also place a reference count on it when we construct it, and we lose
// that count when the thread finishes doing useful work (OnThreadTerminate).
//
// One way OnThreadTerminate() is called is when the thread finishes doing useful
// work.  This case always happens on the correct thread.
//
// The other way OnThreadTerminate()  is called is during product shutdown.  We do
// a "best effort" to eliminate all threads except the Main thread before shutdown
// happens.  But there may be some background threads or external threads still
// running.
//
// When the final reference count disappears, we destruct.  Until then, the thread
// remains in the ThreadStore, but is marked as "Dead".

int Thread::IncExternalCount()
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    Thread *pCurThread = GetThread();

    _ASSERTE(m_ExternalRefCount > 0);
    int retVal = FastInterlockIncrement((LONG*)&m_ExternalRefCount);
    // If we have an exposed object and the refcount is greater than one
    // we must make sure to keep a strong handle to the exposed object
    // so that we keep it alive even if nobody has a reference to it.
    if (pCurThread && ((*((void**)m_ExposedObject)) != NULL))
    {
        // The exposed object exists and needs a strong handle so check
        // to see if it has one.
        // Only a managed thread can setup StrongHnd.
        if ((*((void**)m_StrongHndToExposedObject)) == NULL)
        {
            GCX_COOP();
            // Store the object in the strong handle.
            StoreObjectInHandle(m_StrongHndToExposedObject, ObjectFromHandle(m_ExposedObject));
        }
    }

    return retVal;
}

int Thread::DecExternalCount(BOOL holdingLock)
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    // Note that it's possible to get here with a NULL current thread (during
    // shutdown of the thread manager).
    Thread *pCurThread = GetThread();
    _ASSERTE (pCurThread == NULL || g_fProcessDetach
              || (!holdingLock && !ThreadStore::HoldingThreadStore(pCurThread))
              || (holdingLock && ThreadStore::HoldingThreadStore(pCurThread)));

    BOOL ToggleGC = FALSE;
    BOOL SelfDelete = FALSE;

    int retVal;

    // Must synchronize count and exposed object handle manipulation. We use the
    // thread lock for this, which implies that we must be in pre-emptive mode
    // to begin with and avoid any activity that would invoke a GC (this
    // acquires the thread store lock).
    if (pCurThread)
    {
        ToggleGC = pCurThread->PreemptiveGCDisabled();
        if (ToggleGC)
            pCurThread->EnablePreemptiveGC();
    }
    ThreadStoreLockHolder tsLock(!holdingLock);

    _ASSERTE(m_ExternalRefCount >= 1);
    _ASSERTE(!holdingLock ||
             ThreadStore::s_pThreadStore->m_Crst.GetEnterCount() > 0 ||
             g_fProcessDetach);

    retVal = FastInterlockDecrement((LONG*)&m_ExternalRefCount);

    if (retVal == 0)
    {
        HANDLE h = GetThreadHandle();
        if (h == INVALID_HANDLE_VALUE)
        {
            h = m_ThreadHandleForClose;
            m_ThreadHandleForClose = INVALID_HANDLE_VALUE;
        }
        // Can not assert like this.  We have already removed the Unstarted bit.
        //_ASSERTE (IsUnstarted() || h != INVALID_HANDLE_VALUE);
        if (h != INVALID_HANDLE_VALUE && m_WeOwnThreadHandle)
        {
            ::CloseHandle(h);
            SetThreadHandle(INVALID_HANDLE_VALUE);
        }
        if (m_pHostTask) {
            ReleaseHostTask();
        }
        // Switch back to cooperative mode to manipulate the thread.
        if (pCurThread)
            pCurThread->DisablePreemptiveGC();

        // during process detach the thread might still be in the thread list
        // if it hasn't seen its DLL_THREAD_DETACH yet.  Use the following
        // tweak to decide if the thread has terminated yet.
        if (!HasValidThreadHandle())
        {
            SelfDelete = this == pCurThread;
            m_ExceptionState.FreeAllStackTraces();
            if (SelfDelete) {
                UnsafeTlsSetValue(gThreadTLSIndex, (VOID*)NULL);
#ifdef _DEBUG
                AddFiberInfo(ThreadTrackInfo_Lifetime);
#endif
            }
            delete this;
        }

        tsLock.Release();

        // It only makes sense to restore the GC mode if we didn't just destroy
        // our own thread object.
        if (pCurThread && !SelfDelete && !ToggleGC)
            pCurThread->EnablePreemptiveGC();

        return retVal;
    }
    else if (pCurThread == NULL)
    {
        // We're in shutdown, too late to be worrying about having a strong
        // handle to the exposed thread object, we've already performed our
        // final GC.
        tsLock.Release();

        return retVal;
    }
    else
    {
        // Check to see if the external ref count reaches exactly one. If this
        // is the case and we have an exposed object then it is that exposed object
        // that is holding a reference to us. To make sure that we are not the
        // ones keeping the exposed object alive we need to remove the strong
        // reference we have to it.
        if ((retVal == 1) && ((*((void**)m_StrongHndToExposedObject)) != NULL))
        {
            // Switch back to cooperative mode to manipulate the object.

            // Clear the handle and leave the lock.
            // We do not have to to DisablePreemptiveGC here, because
            // we just want to put NULL into a handle.
            StoreObjectInHandle(m_StrongHndToExposedObject, NULL);

            tsLock.Release();

            // Switch back to the initial GC mode.
            if (ToggleGC)
                pCurThread->DisablePreemptiveGC();

            return retVal;
        }
    }

    tsLock.Release();

    // Switch back to the initial GC mode.
    if (ToggleGC)
        pCurThread->DisablePreemptiveGC();

    return retVal;
}


//--------------------------------------------------------------------
// Destruction. This occurs after the associated native thread
// has died.
//--------------------------------------------------------------------
Thread::~Thread()
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    _ASSERTE(m_ThrewControlForThread == 0);

    // AbortRequest is coupled with TrapReturningThread.
    // We should have unmarked the thread for abort.
    // !!! Can not assert here.  If a thread has no managed code on stack
    // !!! we leave the g_TrapReturningThread set so that the thread will be
    // !!! aborted if it enters managed code.
    //_ASSERTE(!IsAbortRequested());

    // We should not have the Thread marked for abort.  But if we have
    // we need to unmark it so that g_TrapReturningThreads is decremented.
    if (IsAbortRequested())
    {
        UnmarkThreadForAbort(TAR_ALL);
    }

#if defined(_DEBUG) && defined(TRACK_SYNC)
    _ASSERTE(IsAtProcessExit() || ((Dbg_TrackSyncStack *) m_pTrackSync)->m_StackPointer == 0);
    delete m_pTrackSync;
#endif // TRACK_SYNC

    _ASSERTE(IsDead() || IsUnstarted() || IsAtProcessExit());

    if (m_WaitEventLink.m_Next != NULL && !IsAtProcessExit())
    {
        WaitEventLink *walk = &m_WaitEventLink;
        while (walk->m_Next) {
            ThreadQueue::RemoveThread(this, (SyncBlock*)((DWORD_PTR)walk->m_Next->m_WaitSB & ~1));
            StoreEventToEventStore (walk->m_Next->m_EventWait);
        }
        m_WaitEventLink.m_Next = NULL;
    }

    if (m_StateNC & TSNC_ExistInThreadStore) {
        BOOL ret;
        ret = ThreadStore::RemoveThread(this);
        _ASSERTE(ret);
    }

#ifdef _DEBUG
    m_pFrame = (Frame *)POISONC;
#endif

    // Update Perfmon counters.
    COUNTER_ONLY(GetPrivatePerfCounters().m_LocksAndThreads.cCurrentThreadsLogical--);

    // Current recognized threads are non-runtime threads that are alive and ran under the
    // runtime. Check whether this Thread was one of them.
    if ((m_State & TS_WeOwn) == 0)
    {
        COUNTER_ONLY(GetPrivatePerfCounters().m_LocksAndThreads.cRecognizedThreads--);
    }
    else
    {
        COUNTER_ONLY(GetPrivatePerfCounters().m_LocksAndThreads.cCurrentThreadsPhysical--);
    }

    // Normally we shouldn't get here with a valid thread handle; however if SetupThread
    // failed (due to an OOM for example) then we need to CloseHandle the thread
    // handle if we own it.
    if (m_WeOwnThreadHandle && (GetThreadHandle() != INVALID_HANDLE_VALUE))
    {
        CloseHandle(GetThreadHandle());
    }

    if (m_SafeEvent.IsValid())
    {
        m_SafeEvent.CloseEvent();
    }
    if (m_UserSuspendEvent.IsValid())
    {
        m_UserSuspendEvent.CloseEvent();
    }
    if (m_DebugSuspendEvent.IsValid())
    {
        m_DebugSuspendEvent.CloseEvent();
    }
    if (m_EventWait.IsValid())
    {
        m_EventWait.CloseEvent();
    }

    delete m_OSContext;

    if (GetSavedRedirectContext())
    {
        delete GetSavedRedirectContext();
        SetSavedRedirectContext(NULL);
    }


    if (m_pExceptionDuringStartup)
    {
        Exception::Delete (m_pExceptionDuringStartup);
    }

    ClearContext();

    if (!g_fProcessDetach)
    {
        // Destroy any handles that we're using to hold onto exception objects
        SafeSetThrowables(NULL);

        DestroyShortWeakHandle(m_ExposedObject);
        DestroyStrongHandle(m_StrongHndToExposedObject);
    }


    g_pThinLockThreadIdDispenser->DisposeId(m_ThreadId);

    //Ensure DeleteThreadStaticData was executed
    _ASSERTE(m_pSharedStaticData == NULL);
    _ASSERTE(m_pUnsharedStaticData == NULL);

    //Ensure RemoveAllDomainLocalStores was executed
    _ASSERTE(m_pDLSHash == NULL);

    if (m_pHostTask) {
        ReleaseHostTask();
    }


#ifdef _DEBUG
    if (m_pFiberInfo) {
        delete [] (DWORD_PTR*)m_pFiberInfo[0];
    }
#endif
    // Wait for another thread to leave its loop in DeadlockAwareLock::TryBeginEnterLock
    CrstHolder lock(&g_DeadlockAwareCrst);
}


void Thread::CleanupDetachedThreads(GCHeap::SUSPEND_REASON reason, EEIntHashTable *pDLSItems[], int cleanupArrSize)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(!ThreadStore::HoldingThreadStore());

    DLSLockHolder dlsh(TRUE);        
    ThreadStoreLockHolder threadStoreLockHolder(TRUE);
    
    int curIndex =0;

    Thread *thread = ThreadStore::GetAllThreadList(NULL, 0, 0);

    STRESS_LOG0(LF_SYNC, LL_INFO1000, "T::CDT called\n");

    //If the array is full, stop processing detached threads
    //   We'll continue processing more items the next time through
    while ((thread != NULL) && (curIndex < cleanupArrSize))
    {        
        Thread *next = ThreadStore::GetAllThreadList(thread, 0, 0);

        if (thread->IsDetached() && thread->m_UnmanagedRefCount == 0)
        {
            STRESS_LOG1(LF_SYNC, LL_INFO1000, "T::CDT - detaching thread 0x%p\n", thread);

            // Unmark that the thread is detached while we have the
            // thread store lock. This will ensure that no other
            // thread will race in here and try to delete it, too.
            FastInterlockAnd((ULONG*)&(thread->m_State), ~TS_Detached);
            FastInterlockDecrement(&m_DetachCount);
            if (!thread->IsBackground())
                FastInterlockDecrement(&m_ActiveDetachCount);

            //Only add non-null entries into the arrary.  A null entry signifies the end
            //   of the array in DoExtraWorkForFinalizer.
            if(thread->m_pDLSHash)
            {
                pDLSItems[curIndex++] = thread->m_pDLSHash;
                thread->m_pDLSHash = NULL;
            }

            // If the debugger is attached, then we need to unlock the
            // thread store before calling OnThreadTerminate. That
            // way, we won't be holding the thread store lock if we
            // need to block sending a detach thread event.
            BOOL debuggerAttached =
#ifdef DEBUGGING_SUPPORTED
                CORDebuggerAttached();
#else // !DEBUGGING_SUPPORTED
                FALSE;
#endif // !DEBUGGING_SUPPORTED

            if (debuggerAttached)
                ThreadStore::UnlockThreadStore();

            thread->OnThreadTerminate(debuggerAttached ? FALSE : TRUE);

#ifdef DEBUGGING_SUPPORTED
            if (debuggerAttached)
            {
                ThreadStore::LockThreadStore(reason);

                // We remember the next Thread in the thread store
                // list before deleting the current one. But we can't
                // use that Thread pointer now that we release the
                // thread store lock in the middle of the loop. We
                // have to start from the beginning of the list every
                // time. If two threads T1 and T2 race into
                // CleanupDetachedThreads, then T1 will grab the first
                // Thread on the list marked for deletion and release
                // the lock. T2 will grab the second one on the
                // list. T2 may complete destruction of its Thread,
                // then T1 might re-acquire the thread store lock and
                // try to use the next Thread in the thread store. But
                // T2 just deleted that next Thread.
                thread = ThreadStore::GetAllThreadList(NULL, 0, 0);
            }
            else
#endif // DEBUGGING_SUPPORTED
            {
                thread = next;
            }
        }
        else if (thread->HasThreadState(TS_Finalized))
        {
            STRESS_LOG1(LF_SYNC, LL_INFO1000, "T::CDT - finalized thread 0x%p\n", thread);

            thread->ResetThreadState(TS_Finalized);
            // We have finalized the managed Thread object.  Now it is time to clean up the unmanaged part
            thread->DecExternalCount(TRUE);
            thread = next;
        }
        else
        {
            thread = next;
        }
    }
    
    s_fCleanFinalizedThread = FALSE;
}


// See general comments on thread destruction above.
void Thread::OnThreadTerminate(BOOL holdingLock)
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    // Should not use OSThreadId:
    // OSThreadId may change for the current thread is the thread is blocked and rescheduled
    // by host.
    Thread *pCurrentThread = GetThread();
    DWORD CurrentThreadID = pCurrentThread?pCurrentThread->GetThreadId():0;
    DWORD ThisThreadID = GetThreadId();


    // We took a count during construction, and we rely on the count being
    // non-zero as we terminate the thread here.
    _ASSERTE(m_ExternalRefCount > 0);

    // The thread is no longer running.  It's important that we zero any general OBJECTHANDLE's
    // on this Thread object.  That's because we need the managed Thread object to be subject to
    // GC and yet any HANDLE is opaque to the GC when it comes to collecting cycles.  If e.g. the
    // Thread's AbortReason (which is an arbitrary object) contains transitively a reference back
    // to the Thread, then we have an uncollectible cycle.  When the thread is executing, nothing
    // can be collected anyway.  But now that we stop running the cycle concerns us.
    //
    // It's important that we only use OBJECTHANDLE's that are retrievable while the thread is
    // still running.  That's what allows us to zero them here with impunity:
    {
        // No handles to clean up in the m_ExceptionState
        _ASSERTE(!m_ExceptionState.IsExceptionInProgress());

        GCX_COOP();

        // Destroy the LastThrown handle (and anything that violates the above assert).
        SafeSetThrowables(NULL);

        // Cleaning up the AbortReason is tricky, since the handle is only valid if the ADID is valid
        // ...and we can only perform this operation if other threads aren't racing to update these
        // values on our thread asynchronously.
        ClearAbortReason();

        DeleteThreadStaticData();

        //If we're not holding the ThreadStore and we're running on the actual thread
        //   we can safely cleanup the DomainLocalStores.  We can't be holding locks since
        //   we need to run managed code to cleanup the DLS.  This is where DestroyThread
        //   code path will cleanup its DLS.        
        if(!holdingLock && (this == GetThread()))
        {
            RemoveAllDomainLocalStores(&this->m_pDLSHash, true);
        }

        //Clear the references which could create cycles
        //  This allows the GC to collect them
        THREADBASEREF thread = (THREADBASEREF) GetExposedObjectRaw();
        if (thread != NULL)
        {
            thread->ResetCurrentUserCulture();
            thread->ResetCurrentUICulture();
            thread->ResetCulture();
        }
    }

    if  (GCHeap::IsGCHeapInitialized())
    {
        // Guaranteed to NOT be a shutdown case, because we tear down the heap before
        // we tear down any threads during shutdown.
        if (ThisThreadID == CurrentThreadID)
        {
            GCX_COOP();
            GCHeap::GetGCHeap()->FixAllocContext(&m_alloc_context, FALSE, NULL, NULL);
            m_alloc_context.init();
        }
    }

    // We switch a thread to dead when it has finished doing useful work.  But it
    // remains in the thread store so long as someone keeps it alive.  An exposed
    // object will do this (it releases the refcount in its finalizer).  If the
    // thread is never released, we have another look during product shutdown and
    // account for the unreleased refcount of the uncollected exposed object:
    if (IsDead())
    {
        _ASSERTE(IsAtProcessExit());
        ClearContext();
        if (m_ExposedObject != NULL)
            DecExternalCount(holdingLock);             // may destruct now
    }
    else
    {
#ifdef DEBUGGING_SUPPORTED
        //
        // If we're debugging, let the debugger know that this thread is
        // gone.
        //
        if (CORDebuggerAttached())
        {
            _ASSERTE(!holdingLock); // Cannot hold TSL during this call
            g_pDebugInterface->DetachThread(this);
        }
#endif // DEBUGGING_SUPPORTED

#ifdef PROFILING_SUPPORTED
        // If a profiler is present, then notify the profiler of thread destroy
        if (CORProfilerTrackThreads())
        {
            // The profiler will typically block in the ThreadDestroyed callback, because
            // it must coordinate the use of this threadid amongst all profiler
            // threads.  For instance, if a separate thread A is walking "this" (via DoStackSnapshot),
            // then the profiler must block in ThreadDestroyed until A is finished.  Otherwise,
            // "this" will complete its destruction before A's walk is complete.
            //
            // Since the profiler will block indefinitely in ThreadDestroyed, we need
            // to switch to preemptive mode.  Otherwise, if another thread B needs to suspend
            // the runtime (due to appdomain unload, GC, etc.), thread B will block
            // waiting for "this" (assuming we allow "this" to remain in cooperative mode),
            // while the profiler forces "this" to block on thread A from
            // the example above.  And thread A may need to block on thread B, since
            // the stackwalking occasionally needs to switch to cooperative to access a
            // hash map (thus DoStackSnapshot forces the switch to cooperative up-front, before
            // the target thread to be walked gets suspended (yet another deadlock possibility)),
            // and switching to cooperative requires a wait until an in-progress GC or
            // EE suspension is complete.  In other words, allowing "this" to remain
            // in cooperative mode could lead to a 3-way deadlock:
            //      "this" waits on A
            //      A waits on B
            //      B waits on "this".
            GCX_PREEMP();
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ThreadDestroyed((ThreadID) this);
        }
#endif // PROFILING_SUPPORTED

        if (!holdingLock)
        {
            LOG((LF_SYNC, INFO3, "OnThreadTerminate obtain lock\n"));
            ThreadStore::LockThreadStore(GCHeap::SUSPEND_OTHER);

            if (ThisThreadID == CurrentThreadID && pCurrentThread)
            {
                // Before we call UnlockThreadStore, we remove out Thread from TLS
                // Therefore we will not dec the lock count on thread.
                pCurrentThread->DecLockCount();
            }
        }

        if  (GCHeap::IsGCHeapInitialized() && ThisThreadID != CurrentThreadID)
        {
            // We must be holding the ThreadStore lock in order to clean up alloc context.
            // We should never call FixAllocContext during GC.
            GCHeap::GetGCHeap()->FixAllocContext(&m_alloc_context, FALSE, NULL, NULL);
            m_alloc_context.init();
        }

        FastInterlockOr((ULONG *) &m_State, TS_Dead);
        ThreadStore::s_pThreadStore->m_DeadThreadCount++;

        if (IsUnstarted())
            ThreadStore::s_pThreadStore->m_UnstartedThreadCount--;
        else
        {
            if (IsBackground())
                ThreadStore::s_pThreadStore->m_BackgroundThreadCount--;
        }

        FastInterlockAnd((ULONG *) &m_State, ~(TS_Unstarted | TS_Background));

        //
        // If this thread was told to trip for debugging between the
        // sending of the detach event above and the locking of the
        // thread store lock, then remove the flag and decrement the
        // global trap returning threads count.
        //
        if (!IsAtProcessExit())
        {
            // A thread can't die during a GCPending, because the thread store's
            // lock is held by the GC thread.
            if (m_State & TS_DebugSuspendPending)
                UnmarkForSuspension(~TS_DebugSuspendPending);

            if (m_State & TS_UserSuspendPending)
                UnmarkForSuspension(~TS_UserSuspendPending);

            if (CurrentThreadID == ThisThreadID && IsAbortRequested())
            {
                UnmarkThreadForAbort(Thread::TAR_ALL);
            }
        }

        if (GetThreadHandle() != INVALID_HANDLE_VALUE)
        {
            if (m_ThreadHandleForClose == INVALID_HANDLE_VALUE)
            {
                m_ThreadHandleForClose = GetThreadHandle();
            }
            SetThreadHandle (INVALID_HANDLE_VALUE);
        }

        m_OSThreadId = 0;

        // If nobody else is holding onto the thread, we may destruct it here:
        ULONG   oldCount = DecExternalCount(TRUE);
        // If we are shutting down the process, we only have one thread active in the
        // system.  So we can disregard all the reasons that hold this thread alive --
        // TLS is about to be reclaimed anyway.
        if (IsAtProcessExit())
            while (oldCount > 0)
            {
                oldCount = DecExternalCount(TRUE);
            }

        // ASSUME THAT THE THREAD IS DELETED, FROM HERE ON

        _ASSERTE(ThreadStore::s_pThreadStore->m_ThreadCount >= 0);
        _ASSERTE(ThreadStore::s_pThreadStore->m_BackgroundThreadCount >= 0);
        _ASSERTE(ThreadStore::s_pThreadStore->m_ThreadCount >=
                 ThreadStore::s_pThreadStore->m_BackgroundThreadCount);
        _ASSERTE(ThreadStore::s_pThreadStore->m_ThreadCount >=
                 ThreadStore::s_pThreadStore->m_UnstartedThreadCount);
        _ASSERTE(ThreadStore::s_pThreadStore->m_ThreadCount >=
                 ThreadStore::s_pThreadStore->m_DeadThreadCount);

        // One of the components of OtherThreadsComplete() has changed, so check whether
        // we should now exit the EE.
        ThreadStore::CheckForEEShutdown();

        if (ThisThreadID == CurrentThreadID)
        {
            // NULL out the thread block  in the tls.  We can't do this if we aren't on the
            // right thread.  But this will only happen during a shutdown.  And we've made
            // a "best effort" to reduce to a single thread before we begin the shutdown.
            UnsafeTlsSetValue(gThreadTLSIndex, (VOID*)NULL);
            UnsafeTlsSetValue(GetAppDomainTLSIndex(), (VOID*)NULL);
        }

        if (!holdingLock)
        {
            LOG((LF_SYNC, INFO3, "OnThreadTerminate releasing lock\n"));
            ThreadStore::UnlockThreadStore(ThisThreadID == CurrentThreadID);
            _ASSERTE(ThreadStore::s_pThreadStore->m_HoldingThread != this || g_fProcessDetach);
        }
    }
}

// Helper functions to check for duplicate handles. we only do this check if
// a waitfor multiple fails.
int __cdecl compareHandles( const void *arg1, const void *arg2 )
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HANDLE h1 = *(HANDLE*)arg1;
    HANDLE h2 = *(HANDLE*)arg2;
    return  (h1 == h2) ? 0 : ((h1 < h2) ? -1 : 1);
}

BOOL CheckForDuplicateHandles(int countHandles, HANDLE *handles)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    qsort(handles,countHandles,sizeof(HANDLE),compareHandles);
    for (int i=0; i < countHandles-1; i++)
    {
        if (handles[i] == handles[i+1])
            return TRUE;
    }
    return FALSE;
}
//--------------------------------------------------------------------
// Based on whether this thread has a message pump, do the appropriate
// style of Wait.
//--------------------------------------------------------------------
DWORD Thread::DoAppropriateWait(int countHandles, HANDLE *handles, BOOL waitAll,
                                DWORD millis, WaitMode mode, PendingSync *syncState)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    INDEBUG(BOOL alertable = (mode & WaitMode_Alertable) != 0;);
    _ASSERTE(alertable || syncState == 0);

    DWORD dwRet = (DWORD) -1;

    EE_TRY_FOR_FINALLY {
        dwRet =DoAppropriateWaitWorker(countHandles, handles, waitAll, millis, mode);
    }
    EE_FINALLY {
        if (syncState) {
            if (!GOT_EXCEPTION() &&
                dwRet >= WAIT_OBJECT_0 && dwRet < (DWORD)(WAIT_OBJECT_0 + countHandles)) {
                // This thread has been removed from syncblk waiting list by the signalling thread
                syncState->Restore(FALSE);
            }
            else
                syncState->Restore(TRUE);
        }

        _ASSERTE (dwRet != WAIT_IO_COMPLETION);
    }
    EE_END_FINALLY;

    return(dwRet);
}

DWORD Thread::DoAppropriateWait(AppropriateWaitFunc func, void *args,
                                DWORD millis, WaitMode mode,
                                PendingSync *syncState)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    INDEBUG(BOOL alertable = (mode & WaitMode_Alertable) != 0;);
    _ASSERTE(alertable || syncState == 0);

    DWORD dwRet = (DWORD) -1;

    EE_TRY_FOR_FINALLY {
        dwRet = DoAppropriateWaitWorker(func, args, millis, mode);
    }
    EE_FINALLY {
        if (syncState) {
            if (!GOT_EXCEPTION() && WAIT_OBJECT_0 == dwRet) {
                // This thread has been removed from syncblk waiting list by the signalling thread
                syncState->Restore(FALSE);
            }
            else
                syncState->Restore(TRUE);
        }

        _ASSERTE (WAIT_IO_COMPLETION != dwRet);
    }
    EE_END_FINALLY;

    return(dwRet);
}

DWORD WaitForMultipleObjectsEx_SO_TOLERANT (DWORD nCount, HANDLE *lpHandles, BOOL bWaitAll,DWORD dwMilliseconds, BOOL bAlertable)
{
    DWORD dwRet = WAIT_FAILED;
    DWORD lastError = 0;
    BEGIN_SO_TOLERANT_CODE (GetThread ());
    dwRet = ::WaitForMultipleObjectsEx (nCount, lpHandles, bWaitAll, dwMilliseconds, bAlertable);
    lastError = ::GetLastError();
    END_SO_TOLERANT_CODE;

    // END_SO_TOLERANT_CODE overwrites lasterror.  Let's reset it.
    ::SetLastError(lastError);
    return dwRet;
}

//--------------------------------------------------------------------
// Do appropriate wait based on apartment state (STA or MTA)
DWORD Thread::DoAppropriateAptStateWait(int numWaiters, HANDLE* pHandles, BOOL bWaitAll,
                                         DWORD timeout, WaitMode mode)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    BOOL alertable = (mode&WaitMode_Alertable)!=0;

    return WaitForMultipleObjectsEx_SO_TOLERANT(numWaiters, pHandles,bWaitAll, timeout,alertable);
}

// A helper called by our two flavors of DoAppropriateWaitWorker
void Thread::DoAppropriateWaitWorkerAlertableHelper(WaitMode mode)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    // If thread abort is prevented, we do not want this thread to see thread abort and thread interrupt exception.
    if (IsThreadAbortPrevented())
    {
        return;
    }

    // A word about ordering for Interrupt.  If someone tries to interrupt a thread
    // that's in the interruptible state, we queue an APC.  But if they try to interrupt
    // a thread that's not in the interruptible state, we just record that fact.  So
    // we have to set TS_Interruptible before we test to see whether someone wants to
    // interrupt us or else we have a race condition that causes us to skip the APC.
    FastInterlockOr((ULONG *) &m_State, TS_Interruptible);

    if (HasThreadStateNC(TSNC_InRestoringSyncBlock))
    {
        // The thread is restoring SyncBlock for Object.Wait.
        ResetThreadStateNC(TSNC_InRestoringSyncBlock);
    }
    else
    {
    HandleThreadInterrupt((mode & WaitMode_ADUnload) != 0);

    // Safe to clear the interrupted state, no APC could have fired since we
    // reset m_UserInterrupt (which inhibits our APC callback from doing
    // anything).
    FastInterlockAnd((ULONG *) &m_State, ~TS_Interrupted);
}
}

void MarkOSAlertableWait(StateHolderParam)
{
    GetThread()->SetThreadStateNC (Thread::TSNC_OSAlertableWait);
}

void UnMarkOSAlertableWait(StateHolderParam)
{
    GetThread()->ResetThreadStateNC (Thread::TSNC_OSAlertableWait);
}

//--------------------------------------------------------------------
// Based on whether this thread has a message pump, do the appropriate
// style of Wait.
//--------------------------------------------------------------------
DWORD Thread::DoAppropriateWaitWorker(int countHandles, HANDLE *handles, BOOL waitAll,
                                      DWORD millis, WaitMode mode)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    DWORD ret = 0;

    BOOL alertable = (mode & WaitMode_Alertable)!= 0;
    BOOL ignoreSyncCtx = (mode & WaitMode_IgnoreSyncCtx)!= 0;

    // Unless the ignoreSyncCtx flag is set, first check to see if there is a synchronization
    // context on the current thread and if there is, dispatch to it to do the wait.
    // If  the wait is non alertable we cannot forward the call to the sync context
    // since fundamental parts of the system (such as the GC) rely on non alertable
    // waits not running any managed code. Also if we are past the point in shutdown were we
    // are allowed to run managed code then we can't forward the call to the sync context.
    if (!ignoreSyncCtx && alertable && CanRunManagedCode(FALSE))
    {
        GCX_COOP();

        BOOL fSyncCtxPresent = FALSE;
        OBJECTREF SyncCtxObj = NULL;
        GCPROTECT_BEGIN(SyncCtxObj)
        {
            GetSynchronizationContext(&SyncCtxObj);
            if (SyncCtxObj != NULL)
            {
                SYNCHRONIZATIONCONTEXTREF syncRef = (SYNCHRONIZATIONCONTEXTREF)SyncCtxObj;
                if (syncRef->IsWaitNotificationRequired())
                {
                    fSyncCtxPresent = TRUE;
                    ret = DoSyncContextWait(&SyncCtxObj, countHandles, handles, waitAll, millis);
                }
            }
        }
        GCPROTECT_END();

        if (fSyncCtxPresent)
            return ret;
    }

    GCX_PREEMP();

    if(alertable)
    {
        DoAppropriateWaitWorkerAlertableHelper(mode);
    }

    LeaveRuntimeHolder holder((size_t)WaitForMultipleObjectsEx);
    StateHolder<MarkOSAlertableWait,UnMarkOSAlertableWait> OSAlertableWait(alertable);

    ThreadStateHolder tsh(alertable, TS_Interruptible | TS_Interrupted);

    ULONGLONG dwStart = 0, dwEnd;
retry:
    if (millis != INFINITE)
    {
        dwStart = CLRGetTickCount64();
    }

    ret = DoAppropriateAptStateWait(countHandles, handles, waitAll, millis, mode);

    if (ret == WAIT_IO_COMPLETION)
    {
        _ASSERTE (alertable);

        if (m_State & TS_Interrupted)
        {
            HandleThreadInterrupt(mode & WaitMode_ADUnload);
        }
        // We could be woken by some spurious APC or an EE APC queued to
        // interrupt us. In the latter case the TS_Interrupted bit will be set
        // in the thread state bits. Otherwise we just go back to sleep again.
        if (millis != INFINITE)
        {
            dwEnd = CLRGetTickCount64();
            if (dwEnd >= dwStart + millis)
            {
                ret = WAIT_TIMEOUT;
                goto WaitCompleted;
            }
            else
            {
                millis -= (DWORD)(dwEnd - dwStart);
            }
        }
        goto retry;
    }
    _ASSERTE((ret >= WAIT_OBJECT_0  && ret < (WAIT_OBJECT_0  + (DWORD)countHandles)) ||
             (ret >= WAIT_ABANDONED && ret < (WAIT_ABANDONED + (DWORD)countHandles)) ||
             (ret == WAIT_TIMEOUT) || (ret == WAIT_FAILED));
    // countHandles is used as an unsigned -- it should never be negative.
    _ASSERTE(countHandles >= 0);

    if (ret == WAIT_FAILED)
    {
        DWORD errorCode = ::GetLastError();
        if (errorCode == ERROR_INVALID_PARAMETER)
        {
            if (CheckForDuplicateHandles(countHandles, handles))
                COMPlusThrow(kDuplicateWaitObjectException);
            else
                COMPlusThrowHR(HRESULT_FROM_WIN32(errorCode));
        }
        else if (errorCode == ERROR_ACCESS_DENIED)
        {
            // A Win32 ACL could prevent us from waiting on the handle.
            COMPlusThrow(kUnauthorizedAccessException);
        }

        _ASSERTE(errorCode == ERROR_INVALID_HANDLE);

        if (countHandles == 1)
            ret = WAIT_OBJECT_0;
        else if (waitAll)
        {
            // Probe all handles with a timeout of zero. When we find one that's
            // invalid, move it out of the list and retry the wait.
#ifdef _DEBUG
            BOOL fFoundInvalid = FALSE;
#endif
            for (int i = 0; i < countHandles; i++)
            {
                // WaitForSingleObject won't pump memssage; we already probe enough space
                // before calling this function and we don't want to fail here, so we don't
                // do a transition to tolerant code here
                DWORD subRet = WaitForSingleObject (handles[i], 0);
                if (subRet != WAIT_FAILED)
                    continue;
                _ASSERTE(::GetLastError() == ERROR_INVALID_HANDLE);
                if ((countHandles - i - 1) > 0)
                    memmove(&handles[i], &handles[i+1], (countHandles - i - 1) * sizeof(HANDLE));
                countHandles--;
#ifdef _DEBUG
                fFoundInvalid = TRUE;
#endif
                break;
            }
            _ASSERTE(fFoundInvalid);

            // Compute the new timeout value by assume that the timeout
            // is not large enough for more than one wrap
            dwEnd = CLRGetTickCount64();
            if (millis != INFINITE)
            {
                if (dwEnd >= dwStart + millis)
                {
                    ret = WAIT_TIMEOUT;
                    goto WaitCompleted;
                }
                else
                {
                    millis -= (DWORD)(dwEnd - dwStart);
                }
            }
            goto retry;
        }
        else
        {
            // Probe all handles with a timeout as zero, succeed with the first
            // handle that doesn't timeout.
            ret = WAIT_OBJECT_0;
            int i;
            for (i = 0; i < countHandles; i++)
            {
            TryAgain:
                // WaitForSingleObject won't pump memssage; we already probe enough space
                // before calling this function and we don't want to fail here, so we don't
                // do a transition to tolerant code here
                DWORD subRet = WaitForSingleObject (handles[i], 0);
                if ((subRet == WAIT_OBJECT_0) || (subRet == WAIT_FAILED))
                    break;
                if (subRet == WAIT_ABANDONED)
                {
                    ret = (ret - WAIT_OBJECT_0) + WAIT_ABANDONED;
                    break;
                }
                // If we get alerted it just masks the real state of the current
                // handle, so retry the wait.
                if (subRet == WAIT_IO_COMPLETION)
                    goto TryAgain;
                _ASSERTE(subRet == WAIT_TIMEOUT);
                ret++;
            }
            _ASSERTE(i != countHandles);
        }
    }

WaitCompleted:

    _ASSERTE((ret != WAIT_TIMEOUT) || (millis != INFINITE));

    return ret;
}


DWORD Thread::DoAppropriateWaitWorker(AppropriateWaitFunc func, void *args,
                                      DWORD millis, WaitMode mode)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    BOOL alertable = (mode & WaitMode_Alertable)!=0;

    GCX_PREEMP();

    DWORD ret;
    if(alertable)
    {
        DoAppropriateWaitWorkerAlertableHelper(mode);
    }

    DWORD option;
    if (alertable) {
        {
            option = WAIT_ALERTABLE;
        }
    }
    else
        option = 0;

    ThreadStateHolder tsh(alertable, TS_Interruptible | TS_Interrupted);

    ULONGLONG dwStart = 0;
    ULONGLONG dwEnd;

retry:
    if (millis != INFINITE)
    {
        dwStart = CLRGetTickCount64();
    }
    ret = func(args, millis, option);

    if (ret == WAIT_IO_COMPLETION)
    {
        _ASSERTE (alertable);

        if ((m_State & TS_Interrupted))
        {
            HandleThreadInterrupt(mode & WaitMode_ADUnload);
        }
        if (millis != INFINITE)
        {
            dwEnd = CLRGetTickCount64();
            if (dwEnd >= dwStart + millis)
            {
                ret = WAIT_TIMEOUT;
                goto WaitCompleted;
            }
            else
            {
                millis -= (DWORD)(dwEnd - dwStart);
            }
        }
        goto retry;
    }

WaitCompleted:
    _ASSERTE(ret == WAIT_OBJECT_0 ||
             ret == WAIT_ABANDONED ||
             ret == WAIT_TIMEOUT ||
             ret == WAIT_FAILED);

    _ASSERTE((ret != WAIT_TIMEOUT) || (millis != INFINITE));

    return ret;
}


DWORD Thread::DoSyncContextWait(OBJECTREF *pSyncCtxObj, int countHandles, HANDLE *handles, BOOL waitAll, DWORD millis)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(handles));
        PRECONDITION(IsProtectedByGCFrame (pSyncCtxObj));
    }
    CONTRACTL_END;

    MethodDescCallSite invokeWaitMethodHelper(METHOD__SYNCHRONIZATION_CONTEXT__INVOKE_WAIT_METHOD_HELPER);

    PTRARRAYREF handleArrayObj = (PTRARRAYREF)AllocatePrimitiveArray(ELEMENT_TYPE_I, countHandles);
    memcpyNoGCRefs(handleArrayObj->GetDataPtr(), handles, countHandles * sizeof(HANDLE));

    ARG_SLOT args[6] =
    {
        ObjToArgSlot(*pSyncCtxObj),
        ObjToArgSlot(handleArrayObj),
        (ARG_SLOT)waitAll,
        (ARG_SLOT)millis,
    };

    return invokeWaitMethodHelper.Call_RetI4(args);
}


// Called out of SyncBlock::Wait() to block this thread until the Notify occurs.
BOOL Thread::Block(INT32 timeOut, PendingSync *syncState)
{
    WRAPPER_CONTRACT;

    _ASSERTE(this == GetThread());

    // Before calling Block, the SyncBlock queued us onto it's list of waiting threads.
    // However, before calling Block the SyncBlock temporarily left the synchronized
    // region.  This allowed threads to enter the region and call Notify, in which
    // case we may have been signalled before we entered the Wait.  So we aren't in the
    // m_WaitSB list any longer.  Not a problem: the following Wait will return
    // immediately.  But it means we cannot enforce the following assertion:
//    _ASSERTE(m_WaitSB != NULL);

    return (Wait(syncState->m_WaitEventLink->m_Next->m_EventWait, timeOut, syncState) != WAIT_OBJECT_0);
}


// Return whether or not a timeout occured.  TRUE=>we waited successfully
DWORD Thread::Wait(HANDLE *objs, int cntObjs, INT32 timeOut, PendingSync *syncInfo)
{
    WRAPPER_CONTRACT;

    DWORD   dwResult;
    DWORD   dwTimeOut32;

    _ASSERTE(timeOut >= 0 || timeOut == INFINITE_TIMEOUT);

    dwTimeOut32 = (timeOut == INFINITE_TIMEOUT
                   ? INFINITE
                   : (DWORD) timeOut);

    dwResult = DoAppropriateWait(cntObjs, objs, FALSE /*=waitAll*/, dwTimeOut32,
                                 WaitMode_Alertable /*alertable*/,
                                 syncInfo);

    // Either we succeeded in the wait, or we timed out
    _ASSERTE((dwResult >= WAIT_OBJECT_0 && dwResult < (DWORD)(WAIT_OBJECT_0 + cntObjs)) ||
             (dwResult == WAIT_TIMEOUT));

    return dwResult;
}

// Return whether or not a timeout occured.  TRUE=>we waited successfully
DWORD Thread::Wait(CLREvent *pEvent, INT32 timeOut, PendingSync *syncInfo)
{
    WRAPPER_CONTRACT;

    DWORD   dwResult;
    DWORD   dwTimeOut32;

    _ASSERTE(timeOut >= 0 || timeOut == INFINITE_TIMEOUT);

    dwTimeOut32 = (timeOut == INFINITE_TIMEOUT
                   ? INFINITE
                   : (DWORD) timeOut);

    dwResult = pEvent->Wait(dwTimeOut32, TRUE /*alertable*/, syncInfo);

    // Either we succeeded in the wait, or we timed out
    _ASSERTE((dwResult == WAIT_OBJECT_0) ||
             (dwResult == WAIT_TIMEOUT));

    return dwResult;
}

void Thread::Wake(SyncBlock *psb)
{
    WRAPPER_CONTRACT;

    CLREvent* hEvent = NULL;
    WaitEventLink *walk = &m_WaitEventLink;
    while (walk->m_Next) {
        if (walk->m_Next->m_WaitSB == psb) {
            hEvent = walk->m_Next->m_EventWait;
            // We are guaranteed that only one thread can change walk->m_Next->m_WaitSB
            // since the thread is helding the syncblock.
            walk->m_Next->m_WaitSB = (SyncBlock*)((DWORD_PTR)walk->m_Next->m_WaitSB | 1);
            break;
        }
#ifdef _DEBUG
        else if ((SyncBlock*)((DWORD_PTR)walk->m_Next & ~1) == psb) {
            _ASSERTE (!"Can not wake a thread on the same SyncBlock more than once");
        }
#endif
    }
    PREFIX_ASSUME (hEvent != NULL);
    hEvent->Set();
}

#define WAIT_INTERRUPT_THREADABORT 0x1
#define WAIT_INTERRUPT_INTERRUPT 0x2
#define WAIT_INTERRUPT_OTHEREXCEPTION 0x4

// When we restore
DWORD EnterMonitorForRestore(SyncBlock *pSB)
{
    WRAPPER_CONTRACT;

    DWORD state = 0;
    EX_TRY
    {
        pSB->EnterMonitor();
    }
    EX_CATCH
    {
        // Assume it is a normal exception unless proven.
        state = WAIT_INTERRUPT_OTHEREXCEPTION;
        Thread *pThread = GetThread();
        if (pThread->IsAbortInitiated())
        {
            state = WAIT_INTERRUPT_THREADABORT;
        }
        else if (!__pException.IsNull())
        {
            if (__pException->GetHR() == COR_E_THREADINTERRUPTED)
            {
                state = WAIT_INTERRUPT_INTERRUPT;
            }
        }
    }
    EX_END_CATCH(SwallowAllExceptions);

    return state;
}

// This is the service that backs us out of a wait that we interrupted.  We must
// re-enter the monitor to the same extent the SyncBlock would, if we returned
// through it (instead of throwing through it).  And we need to cancel the wait,
// if it didn't get notified away while we are processing the interrupt.
void PendingSync::Restore(BOOL bRemoveFromSB)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(m_EnterCount);

    Thread      *pCurThread = GetThread();

    _ASSERTE (pCurThread == m_OwnerThread);

    WaitEventLink *pRealWaitEventLink = m_WaitEventLink->m_Next;

    pRealWaitEventLink->m_RefCount --;
    if (pRealWaitEventLink->m_RefCount == 0)
    {
        if (bRemoveFromSB) {
            ThreadQueue::RemoveThread(pCurThread, pRealWaitEventLink->m_WaitSB);
        }
        if (pRealWaitEventLink->m_EventWait != &pCurThread->m_EventWait) {
            // Put the event back to the pool.
            StoreEventToEventStore(pRealWaitEventLink->m_EventWait);
        }
        // Remove from the link.
        m_WaitEventLink->m_Next = m_WaitEventLink->m_Next->m_Next;
    }

    // Someone up the stack is responsible for keeping the syncblock alive by protecting
    // the object that owns it.  But this relies on assertions that EnterMonitor is only
    // called in cooperative mode.  Even though we are safe in preemptive, do the
    // switch.
    GCX_COOP_THREAD_EXISTS(pCurThread);
    // We need to make sure that EnterMonitor succeeds.  We may have code like
    // lock (a)
    // {
    // a.Wait
    // }
    // We need to make sure that the finally from lock is excuted with the lock owned.
    DWORD state = 0;
    SyncBlock *psb = (SyncBlock*)((DWORD_PTR)pRealWaitEventLink->m_WaitSB & ~1);
    for (LONG i=0; i < m_EnterCount;)
    {
        if ((state & (WAIT_INTERRUPT_THREADABORT | WAIT_INTERRUPT_INTERRUPT)) != 0)
        {
            // If the thread has been interrupted by Thread.Interrupt or Thread.Abort,
            // disable the check at the beginning of DoAppropriateWait
            pCurThread->SetThreadStateNC(Thread::TSNC_InRestoringSyncBlock);
        }
        DWORD result = EnterMonitorForRestore(psb);
        if (result == 0)
        {
            i++;
        }
        else
        {
            // We block the thread until the thread acquires the lock.
            // This is to make sure that when catch/finally is executed, the thread has the lock.
            // We do not want thread to run its catch/finally if the lock is not taken.
            state |= result;

            // If the thread is being rudely aborted, and the thread has
            // no Cer on stack, we will not run managed code to release the
            // lock, so we can terminate the loop.
            if (pCurThread->IsRudeAbortInitiated() &&
                !pCurThread->IsExecutingWithinCer())
            {
                break;
            }
        }
    }

    pCurThread->ResetThreadStateNC(Thread::TSNC_InRestoringSyncBlock);

    if ((state & WAIT_INTERRUPT_THREADABORT) != 0)
    {
        pCurThread->HandleThreadAbort();
    }
    else if ((state & WAIT_INTERRUPT_INTERRUPT) != 0)
    {
        COMPlusThrow(kThreadInterruptedException);
    }
}



// This is the callback from the OS, when we queue an APC to interrupt a waiting thread.
// The callback occurs on the thread we wish to interrupt.  It is a STATIC method.
void __stdcall Thread::UserInterruptAPC(ULONG_PTR data)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE(data == APC_Code);

    Thread *pCurThread = GetThread();
    if (pCurThread)
    {
        // We should only take action if an interrupt is currently being
        // requested (our synchronization does not guarantee that we won't fire
        // spuriously). It's safe to check the m_UserInterrupt field and then
        // set TS_Interrupted in a non-atomic fashion because m_UserInterrupt is
        // only cleared in this thread's context (though it may be set from any
        // context).
        if (pCurThread->IsUserInterrupted())
        {
            // Set bit to indicate this routine was called (as opposed to other
            // generic APCs).
            FastInterlockOr((ULONG *) &pCurThread->m_State, TS_Interrupted);
        }
    }
}

// This is the workhorse for Thread.Interrupt().
void Thread::UserInterrupt(ThreadInterruptMode mode)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    FastInterlockOr((DWORD*)&m_UserInterrupt, mode);

    if (HasValidThreadHandle() &&
        HasThreadState (TS_Interruptible))
    {
#ifdef _DEBUG
        AddFiberInfo(ThreadTrackInfo_Abort);
#endif
        Alert();
    }
}

// Implementation of Thread.Sleep().
void Thread::UserSleep(INT32 time)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    INCONTRACT(_ASSERTE(!GetThread()->GCNoTrigger()));

    DWORD   res;

    GCX_PREEMP();

    // A word about ordering for Interrupt.  If someone tries to interrupt a thread
    // that's in the interruptible state, we queue an APC.  But if they try to interrupt
    // a thread that's not in the interruptible state, we just record that fact.  So
    // we have to set TS_Interruptible before we test to see whether someone wants to
    // interrupt us or else we have a race condition that causes us to skip the APC.
    FastInterlockOr((ULONG *) &m_State, TS_Interruptible);

    // If someone has interrupted us, we should not enter the wait.
    if (IsUserInterrupted())
    {
        HandleThreadInterrupt(FALSE);
    }

    ThreadStateHolder tsh(TRUE, TS_Interruptible | TS_Interrupted);

    FastInterlockAnd((ULONG *) &m_State, ~TS_Interrupted);

retry:

    res = ClrSleepEx (time, TRUE);

    if (res == WAIT_IO_COMPLETION)
    {
        // We could be woken by some spurious APC or an EE APC queued to
        // interrupt us. In the latter case the TS_Interrupted bit will be set
        // in the thread state bits. Otherwise we just go back to sleep again.
        if ((m_State & TS_Interrupted))
        {
            HandleThreadInterrupt(FALSE);
        }

        // Don't bother with accurate accounting here.  Just ensure we make progress.
        // Note that this is not us doing the APC.
        if (time == 0)
        {
            res = WAIT_TIMEOUT;
        }
        else
        {
            if (time != (INT32) INFINITE)
            {
                time--;
            }

            goto retry;
        }
    }
    _ASSERTE(res == WAIT_TIMEOUT || res == WAIT_OBJECT_0);
}


// Correspondence between an EE Thread and an exposed System.Thread:
OBJECTREF Thread::GetExposedObject()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    TRIGGERSGC();

    Thread *pCurThread = GetThread();
    _ASSERTE (!(pCurThread == NULL || g_fProcessDetach));

    _ASSERTE(pCurThread->PreemptiveGCDisabled());

    if (ObjectFromHandle(m_ExposedObject) == NULL)
    {
        // Initialize ThreadNative::m_MT if it hasn't been done yet...
        ThreadNative::InitThread();

        // Allocate the exposed thread object.
        THREADBASEREF attempt = (THREADBASEREF) AllocateObject(ThreadNative::m_MT);
        GCPROTECT_BEGIN(attempt);

        BOOL fNeedThreadStore = (! ThreadStore::HoldingThreadStore(pCurThread));
        // Take a lock to make sure that only one thread creates the object.
        ThreadStoreLockHolder tsHolder(fNeedThreadStore);

        // Check to see if another thread has not already created the exposed object.
        if (ObjectFromHandle(m_ExposedObject) == NULL)
        {
            // Keep a weak reference to the exposed object.
            StoreObjectInHandle(m_ExposedObject, (OBJECTREF) attempt);

            ObjectInHandleHolder exposedHolder(m_ExposedObject);

            // Increase the external ref count. We can't call IncExternalCount because we
            // already hold the thread lock and IncExternalCount won't be able to take it.
            ULONG retVal = FastInterlockIncrement ((LONG*)&m_ExternalRefCount);

#ifdef _DEBUG
            AddFiberInfo(ThreadTrackInfo_Lifetime);
#endif
            // Check to see if we need to store a strong pointer to the object.
            if (retVal > 1)
                StoreObjectInHandle(m_StrongHndToExposedObject, (OBJECTREF) attempt);

            ObjectInHandleHolder strongHolder(m_StrongHndToExposedObject);

            // The exposed object keeps us alive until it is GC'ed.  This
            // doesn't mean the physical thread continues to run, of course.
            attempt->SetInternal(this);

            // Note that we are NOT calling the constructor on the Thread.  That's
            // because this is an internal create where we don't want a Start
            // address.  And we don't want to expose such a constructor for our
            // customers to accidentally call.  The following is in lieu of a true
            // constructor:
            attempt->InitExisting();

            exposedHolder.SuppressRelease();
            strongHolder.SuppressRelease();
        }

        GCPROTECT_END();
    }
    return ObjectFromHandle(m_ExposedObject);
}


// We only set non NULL exposed objects for unstarted threads that haven't exited
// their constructor yet.  So there are no race conditions.
void Thread::SetExposedObject(OBJECTREF exposed)
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    if (exposed != NULL)
    {
        _ASSERTE (GetThread() != this);
        _ASSERTE(IsUnstarted());
        _ASSERTE(ObjectFromHandle(m_ExposedObject) == NULL);
        // The exposed object keeps us alive until it is GC'ed.  This doesn't mean the
        // physical thread continues to run, of course.
        StoreObjectInHandle(m_ExposedObject, exposed);
        // This makes sure the contexts on the backing thread
        // and the managed thread start off in sync with each other.
        _ASSERTE(m_Context);
        ((THREADBASEREF)exposed)->SetExposedContext(m_Context->GetExposedObjectRaw());
        // BEWARE: the IncExternalCount call below may cause GC to happen.

        // IncExternalCount will store exposed in m_StrongHndToExposedObject which is in default domain.
        // If the creating thread is killed before the target thread is killed in Thread.Start, Thread object
        // will be kept alive forever.
        // Instead, IncExternalCount should be called after the target thread has been started in Thread.Start.
        // IncExternalCount();
    }
    else
    {
        // Simply set both of the handles to NULL. The GC of the old exposed thread
        // object will take care of decrementing the external ref count.
        StoreObjectInHandle(m_ExposedObject, NULL);
        StoreObjectInHandle(m_StrongHndToExposedObject, NULL);
    }
}

void Thread::SetLastThrownObject(OBJECTREF throwable)
{
    CONTRACTL
    {
        if ((throwable == NULL) || CLRException::IsPreallocatedExceptionObject(throwable)) NOTHROW; else THROWS; // From CreateHandle
        GC_NOTRIGGER;
        if (throwable == NULL) MODE_ANY; else MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    STRESS_LOG1(LF_EH, LL_INFO100, "in Thread::SetLastThrownObject: obj = %p\n", OBJECTREFToObject(throwable));

    if (m_LastThrownObjectHandle != NULL)
    {
        // We'll somtimes use a handle for a preallocated exception object. We should never, ever destroy one of
        // these handles... they'll be destroyed when the Runtime shuts down.
        if (!CLRException::IsPreallocatedExceptionHandle(m_LastThrownObjectHandle))
        {
            DestroyHandle(m_LastThrownObjectHandle);
        }

        m_LastThrownObjectHandle = NULL; // Make sure to set this to NULL here just in case we throw trying to make
                                         // a new handle below.
    }

    if (throwable != NULL)
    {
        _ASSERTE(this == GetThread());

        // Non-compliant exceptions are always wrapped.
        // The use of the ExceptionNative:: helper here (rather than the global ::IsException helper)
        // is hokey, but we need a GC_NOTRIGGER version and it's only for an ASSERT.
        _ASSERTE(ExceptionNative::IsException(throwable->GetMethodTable()));

        // If we're tracking one of the preallocated exception objects, then just use the global handle that
        // matches it rather than creating a new one.
        if (CLRException::IsPreallocatedExceptionObject(throwable))
        {
            m_LastThrownObjectHandle = CLRException::GetPreallocatedHandleForObject(throwable);
        }
        else
        {
            m_LastThrownObjectHandle = GetDomain()->CreateHandle(throwable);
        }

        _ASSERTE(m_LastThrownObjectHandle != NULL);
    }
}

//
// This is a nice wrapper for SetLastThrownObject which catches any exceptions caused by not being able to create
// the handle for the throwable, and setting the last thrown object to the preallocated out of memory exception
// instead.
//
OBJECTREF Thread::SafeSetLastThrownObject(OBJECTREF throwable)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        if (throwable == NULL) MODE_ANY; else MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF ret = throwable;

    EX_TRY
    {
        // Try to set the throwable.
        SetLastThrownObject(throwable);
    }
    EX_CATCH
    {
        ret = CLRException::GetPreallocatedOutOfMemoryException();
        SetLastThrownObject(ret);
    }
    EX_END_CATCH(SwallowAllExceptions);

    return ret;
}

//
// This is a nice wrapper for SetThrowable and SetLastThrownObject, which catches any exceptions caused by not
// being able to create the handle for the throwable, and sets the throwable to the preallocated out of memory
// exception instead. It also updates the last thrown object, which is always updated when the throwable is
// updated.
//
OBJECTREF Thread::SafeSetThrowables(OBJECTREF throwable DEBUG_ARG(ThreadExceptionState::SetThrowableErrorChecking stecFlags))
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        if (throwable == NULL) MODE_ANY; else MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF ret = throwable;

    EX_TRY
    {
        // Try to set the throwable.
        SetThrowable(throwable DEBUG_ARG(stecFlags));

        // Now, if the last thrown object is different, go ahead and update it. This makes sure that we re-throw
        // the right object when we rethrow.
        if (LastThrownObject() != throwable)
        {
            SetLastThrownObject(throwable);
        }
    }
    EX_CATCH
    {
        ret = CLRException::GetPreallocatedOutOfMemoryException();

        // Neither of these will throw because we're setting with a preallocated exception.
        SetThrowable(ret DEBUG_ARG(stecFlags));
        SetLastThrownObject(ret);
    }
    EX_END_CATCH(SwallowAllExceptions);

    return ret;
}

void Thread::SetLastThrownObjectHandle(OBJECTHANDLE h)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (m_LastThrownObjectHandle != NULL &&
        !CLRException::IsPreallocatedExceptionHandle(m_LastThrownObjectHandle))
    {
        DestroyHandle(m_LastThrownObjectHandle);
    }

    m_LastThrownObjectHandle = h;
}

//
// Create a duplicate handle of the current throwable and set the last thrown object to that. This ensures that the
// last thrown object and the current throwable have handles that are in the same app domain.
//
void Thread::SafeUpdateLastThrownObject(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTHANDLE hThrowable = GetThrowableAsHandle();

    if (hThrowable != NULL)
    {
        EX_TRY
        {
            // Using CreateDuplicateHandle here ensures that the AD of the last thrown object matches the domain of
            // the current throwable.
            SetLastThrownObjectHandle(CreateDuplicateHandle(hThrowable));
        }
        EX_CATCH
        {
            // If we can't create a duplicate handle, we set both throwables to the preallocated OOM exception.
            SafeSetThrowables(CLRException::GetPreallocatedOutOfMemoryException());
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
}

// The AbortReason must be cleared at the following times:
//
//  1.  When the application performs a ResetAbort.
//
//  2.  When the physical thread stops running.  That's because we must eliminate any
//      cycles that would otherwise be uncollectible, between the Reason and the Thread.
//      Nobody can retrieve the Reason after the thread stops running anyway.
//
//  We don't have to do any work when the AppDomain containing the Reason object is unloaded.
//  That's because the HANDLE is released as part of the tear-down.  The 'adid' prevents us
//  from ever using the trash handle value thereafter.

void Thread::ClearAbortReason()
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        NOTHROW;
    }
    CONTRACTL_END;

    OBJECTHANDLE oh;
    ADID adid;

    // Scope the lock to stashing and clearing the two fields on the Thread object.
    {
        // Atomically get the OBJECTHANDLE and ADID of the object, and then
        //  clear them.

        // NOTE: get the lock on this thread object, not on the executing thread.
        Thread::AbortRequestLockHolder lock(this);

        // Stash the fields so we can destroy the OBJECTHANDLE if appropriate.
        oh = m_AbortReason;
        adid = m_AbortReasonDomainID;

        // Clear the fields.
        m_AbortReason = 0;
        m_AbortReasonDomainID = ADID(INVALID_APPDOMAIN_ID);
    }

    // If there is an OBJECTHANDLE, try to clear it.
    if (oh != 0 && adid.m_dwId != 0)
    {   // See if the domain is still valid; if so, destroy the ObjectHandle
        AppDomainFromIDHolder ad(adid, TRUE);
        if (!ad.IsUnloaded())
        {   // Still a valid domain, so destroy the handle.
            DestroyHandle(oh);
        }
    }
}


BOOL Thread::IsAtProcessExit()
{
    LEAF_CONTRACT;

    return ((ThreadStore::s_pThreadStore->m_StoreState &
             ThreadStore::TSS_ShuttingDown) != 0);
}

// Context passed down through a stack crawl (see code below).
struct StackCrawlContext
{
    enum SCCType
    {
        SCC_CheckWithinEH   = 0x00000001,
        SCC_CheckWithinCer  = 0x00000002,
    };
    Thread* pAbortee;
    int         eType;
    BOOL        fUnprotectedCode;
    BOOL        fWithinEHClause;
    BOOL        fWithinCer;
    BOOL        fHasManagedCodeOnStack;
    BOOL        fWriteToStressLog;
};

// Crawl the stack looking for Thread Abort related information (whether we're executing inside a CER or an error handling clauses
// of some sort).
StackWalkAction TAStackCrawlCallBack(CrawlFrame* pCf, void* data)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    StackCrawlContext *pData = (StackCrawlContext *)data;
    _ASSERTE(pData->eType & (StackCrawlContext::SCC_CheckWithinCer | StackCrawlContext::SCC_CheckWithinEH));

    if(pCf->IsFrameless())
    {
        IJitManager* pJitManager = pCf->GetJitManager();
        _ASSERTE(pJitManager);
        if (pJitManager && !pData->fHasManagedCodeOnStack)
        {
            pData->fHasManagedCodeOnStack = TRUE;
        }
    }

    // Get the method for this frame if it exists (might not be a managed method, so check the explicit frame if that's what we're
    // looking at).
    MethodDesc *pMD = pCf->GetFunction();
    Frame *pFrame = pCf->GetFrame();
    if (pMD == NULL && pFrame != NULL)
        pMD = pFrame->GetFunction();

    // Non-method frames don't interest us.
    if (pMD == NULL)
        return SWA_CONTINUE;

    #if defined(_DEBUG)
    #define METHODNAME(pFunc) (pFunc?pFunc->m_pszDebugMethodName:"<n/a>")
    #else
    #define METHODNAME(pFunc) "<n/a>"
    #endif
    if (pData->fWriteToStressLog)
    {
        STRESS_LOG4(LF_EH, LL_INFO100, "TAStackCrawlCallBack: STACKCRAWL method:%pM ('%s'), Frame:%p, FrameVtable = %pV\n",
            pMD, METHODNAME(pMD), pFrame, pCf->IsFrameless()?0:(*(void**)pFrame));
    }
    #undef METHODNAME

    // If we're asking about CERs and we don't yet have a definite answer either way then take a closer look at the current method.
    if (pData->eType & StackCrawlContext::SCC_CheckWithinCer && !pData->fUnprotectedCode && !pData->fWithinCer)
    {
        // Check for CER root methods (these are never inlined). If we've found one of these at the root of a bunch of potential CER
        // methods (i.e. those with a compatible reliability contract) then we're executing in a CER.
        if (IsCerRootMethod(pMD))
            pData->fWithinCer = true;

        // Only need to look deeper if we couldn't decide if we're in a CER yet.
        if (!pData->fWithinCer)
        {
            // IL stubs are transparent to CERs.
            if (!pMD->IsILStub())
                // Check for reliability contracts on the method (and class and assembly). If it's high enough level to be included
                // in a CER then we can continue (hopefully finding a CER root method further down the stack). Otherwise we've got
                // at least one method that's not part of a CER on the top of the stack so we're definitely not executing within a
                // CER.
                if (CheckForReliabilityContract(pMD) < RCL_BASIC_CONTRACT)
                    pData->fUnprotectedCode = true;
        }
    }

    // If we weren't asked about EH clauses then we can return now (stop the stack trace if we have a definitive answer on the CER
    // question, move to the next frame otherwise).
    if ((pData->eType & StackCrawlContext::SCC_CheckWithinEH) == 0)
        return ((pData->fWithinCer || pData->fUnprotectedCode) && pData->fHasManagedCodeOnStack) ? SWA_ABORT : SWA_CONTINUE;

    // If we already discovered we're within an EH clause but are still processing (presumably to determine whether we're within a
    // CER), then we can just skip to the next frame straight away. Also terminate here if the current frame is not frameless since
    // there isn't any useful EH information for non-managed frames.
    if (pData->fWithinEHClause || !pCf->IsFrameless())
        return SWA_CONTINUE;

    IJitManager* pJitManager = pCf->GetJitManager();
    _ASSERTE(pJitManager);

    EH_CLAUSE_ENUMERATOR pEnumState;
    unsigned EHCount = pJitManager->InitializeEHEnumeration(pCf->GetMethodToken(), &pEnumState);
    if (EHCount == 0)
        // We do not have finally clause here.
        return SWA_CONTINUE;

    DWORD offs = (DWORD)pCf->GetRelOffset();

    if (!pCf->IsActiveFrame())
    {
        // If we aren't the topmost method, then our IP is a return address and
        // we can't use it to directly compare against the EH ranges because we
        // may be in an cloned finally which has a call as the last instruction.

        // IA64's stackwalker is always adjusting back to the
        // call instruction.

        offs--;

    }

    if (pData->fWriteToStressLog)
    {
        STRESS_LOG1(LF_EH, LL_INFO100, "TAStackCrawlCallBack: STACKCRAWL Offset 0x%x V\n", offs);
    }
    EE_ILEXCEPTION_CLAUSE EHClause, *EHClausePtr;

    for(ULONG i=0; i < EHCount; i++)
    {
        EHClausePtr = pJitManager->GetNextEHClause(pCf->GetMethodToken(),&pEnumState, &EHClause);
        _ASSERTE(IsValidClause(EHClausePtr));

        // !!! If this function is called on Aborter thread, we should check for finally only.
        // !!! Catch and filter clause are skipped.  In UserAbort, the first thing after ReadyForAbort
        // !!! is to check if the target thread is processing exception.
        // !!! If exception is in flight, we don't induce ThreadAbort.  Instead at the end of Jit_EndCatch
        // !!! we will handle abort.
        if (pData->pAbortee != GetThread() && !IsFaultOrFinally(EHClausePtr))
        {
            continue;
        }

        if (offs >= EHClausePtr->HandlerStartPC &&
            offs < EHClausePtr->HandlerEndPC)
        {
            pData->fWithinEHClause = true;
            // We're within an EH clause. If we're asking about CERs too then stop the stack walk if we've reached a conclusive
            // result or continue looking otherwise. Else we can stop the stackwalk now.
            if (pData->eType & StackCrawlContext::SCC_CheckWithinCer)
            {
                return (pData->fWithinCer || pData->fUnprotectedCode) ? SWA_ABORT : SWA_CONTINUE;
            }
            else
            {
                return SWA_ABORT;
        }
    }
    }

    return SWA_CONTINUE;
}

// Is the current thread currently executing within a constrained execution region?
BOOL Thread::IsExecutingWithinCer()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (!g_fEEStarted)
        return FALSE;

    Thread *pThread = GetThread();
    _ASSERTE (pThread);
    StackCrawlContext sContext = { pThread,
                                   StackCrawlContext::SCC_CheckWithinCer,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE};

    pThread->StackWalkFrames(TAStackCrawlCallBack, &sContext);

#ifdef STRESS_LOG
    if (sContext.fWithinCer && StressLog::StressLogOn(-1, 0))
    {
        // If stress log is on, write info to stress log
        StackCrawlContext sContext1 = { pThread,
                                        StackCrawlContext::SCC_CheckWithinCer,
            FALSE,
            FALSE,
            FALSE,
            FALSE,
            TRUE};

        pThread->StackWalkFrames(TAStackCrawlCallBack, &sContext1);
    }
#endif

    return sContext.fWithinCer;
}


#if defined(_X86_) || defined(_AMD64_)
// Context structure used during stack walks to determine whether a given method is executing within a CER.
struct CerStackCrawlContext
{
    MethodDesc *m_pStartMethod;         // First method we crawl (here for debug purposes)
    bool        m_fFirstFrame;          // True for first callback only
    bool        m_fWithinCer;           // The result
};

// Callback used on the stack crawl described above.
StackWalkAction CerStackCrawlCallBack(CrawlFrame *pCf, void *pData)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    CerStackCrawlContext *pCtx = (CerStackCrawlContext *)pData;

    // Skip initial frame which should be our target.
    if (pCtx->m_fFirstFrame)
    {
        _ASSERTE(pCtx->m_pStartMethod == pCf->GetFunction());
        pCtx->m_fFirstFrame = false;
        return SWA_CONTINUE;
    }

    // If we get this far we've located the target method and are scanning the calling tree to see if we have a chain of methods
    // marked with strong reliability contracts terminated by a CER root method.
    MethodDesc *pMD = pCf->GetFunction();
    _ASSERTE(pMD != NULL);

    if (IsCerRootMethod(pMD))
    {
        pCtx->m_fWithinCer = true;
        return SWA_ABORT;
    }

    if (CheckForReliabilityContract(pMD) < RCL_BASIC_CONTRACT)
        return SWA_ABORT;

    // Otherwise everything looks OK so far and we need to investigate the next frame.
    return SWA_CONTINUE;
}
#else
// Context structure used during stack walks to determine whether a given method is executing within a CER.
struct CerStackCrawlContext
{
    MethodDesc *m_pStartMethod;         // First method we crawl (here for debug purposes)
    LPVOID      m_pTargetSP;            // Used to identify the frame at which the scan begins
    LPVOID      m_pTargetBP;            // Ditto
    bool        m_fProlog;              // True until we hit the frame identified above
    bool        m_fWithinCer;           // The result
    bool        m_fTargetIsFunclet;     // If true then we can't exact match SP/BP to detect first frame
};

// Callback used on the stack crawl described above.
StackWalkAction CerStackCrawlCallBack(CrawlFrame *pCf, void *pData)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    CerStackCrawlContext *pCtx = (CerStackCrawlContext *)pData;
    REGDISPLAY *pRD = pCf->GetRegisterSet();

    // Skip initial frames above our real target.
    if (pCtx->m_fProlog)
    {
        // Check if we've hit the target frame (which should be the one that IsWithinCer started out on).
        // Funclets won't be reported back to us, so if the target frame is one we'll check whether we've hit a frame under it on
        // the stack (the first frame reported to us in such a case must be the parent of the funclet).
        if ((pCtx->m_pTargetBP == GetRegdisplayBSP(pRD) && pCtx->m_pTargetSP == (LPVOID)EECodeManager::GetCallerSp(pRD)) ||
            (pCtx->m_fTargetIsFunclet && pCtx->m_pTargetSP < (LPVOID)EECodeManager::GetCallerSp(pRD)))
        {
            _ASSERTE(pCtx->m_pStartMethod == pCf->GetFunction());
            pCtx->m_fProlog = false;
            // Start scanning for real at the very next frame...
        }
        return SWA_CONTINUE;
    }

    // If we get this far we've located the target method and are scanning the calling tree to see if we have a chain of methods
    // marked with strong reliability contracts terminated by a CER root method.
    MethodDesc *pMD = pCf->GetFunction();
    _ASSERTE(pMD != NULL);

    if (IsCerRootMethod(pMD))
    {
        pCtx->m_fWithinCer = true;
        return SWA_ABORT;
    }

    if (CheckForReliabilityContract(pMD) < RCL_BASIC_CONTRACT)
        return SWA_ABORT;

    // Otherwise everything looks OK so far and we need to investigate the next frame.
    return SWA_CONTINUE;
}
#endif

// Determine whether the method at the given depth in the thread's execution stack is executing within a CER.
BOOL Thread::IsWithinCer(CrawlFrame *pCf)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // There had better be a method associated with this frame.
    MethodDesc *pMD = pCf->GetFunction();
    _ASSERTE(pMD != NULL);

    // Try the cheap checks first (before resorting to an actual stackwalk).

    // Handle IL stubs specially. We get called for these guys and they always appear to have a strong reliability contract (due to
    // the System.StubHelpers class they're placed in) but the stack walking logic we have below will skip them (messing up our
    // accounting). For simplicitly and speed we'll just always say these guys are in a CER (we trust the code and it won't block
    // indefinitely so it's a safe guess).
    if (pMD->IsILStub())
        return TRUE;

    if (IsCerRootMethod(pMD))
        return TRUE;

    // Now look at reliability contracts on the method. If they're missing or very weak then this method cannot possibly be in a
    // CER.
    if (CheckForReliabilityContract(pMD) < RCL_BASIC_CONTRACT)
        return FALSE;

    // No way around it: this method has a good reliability contract but is not the root of a CER. We'll have to have to walk the
    // stack to determine whether it was called from a good root.

    // Now things get really tricky. We want to perform a recursive stackwalk (we're called as part of an ongoing stackwalk and we
    // wish to recursively look at one or more of the callers of the current frame).
    //
    // On x86 this is relatively straightforward -- we make a copy of the current crawl frame context (since walking the stack
    // updates the context) and build a new regdisplay around it. We can then start a new crawl from that context (ignoring the
    // first frame of course, because that's this frame).
    //
    // 64-bit is trickier because the context provided by the OS might not be (from our point of view) a valid current context. In
    // particular IA64 provides a mostly valid context except that the SP is from the caller. AMD64 on the other hand will always
    // provide a consistent context, but it may belong to either the current or caller frame. As noted above though, we're really
    // not all that interested in the current context, so as long as we can get to a consistent caller context we're happy.
    //
    // So for AMD64 we'll either have a complete current context and we'll use the the x86 algorithm or we have a complete caller
    // context and we can use more or less the x86 algorithm except we don't need to skip the first frame on the stackwalk callback.
    //
    // IA64 is trickier since it doesn't always give us a consistent context (current or caller). Here we'll have to bite the bullet
    // and perform a full stackwalk to build the context we're after. We'll use a combination of the caller SP and the current BSP
    // as a discriminator (to determine when the full stackwalk has synchronized with this frame and the real walk can begin, it's
    // the same discriminator the OS uses).

#if defined(_X86_) || defined(_AMD64_)
    REGDISPLAY *pCurrentRd = pCf->GetRegisterSet();
    REGDISPLAY  rd;
    CONTEXT     ctx;
    CerStackCrawlContext sContext = { pMD, true, false };

    {
        // On x86 or AMD64 where the OS gave us the current context we just copy that into a new regdisplay (our stackwalking
        // callback will skip the first (current) frame for us).
        CopyRegDisplay(pCurrentRd, &rd, &ctx);
    }

    // The stackwalker requires a starting frame as input. If we're currently inspecting an explicit frame then it's easy -- we just
    // pass that. Otherwise (we're on some frameless managed method) we look at all of the frames for the current thread and choose
    // the one that would synchronize us for walking to the next frame (match the frame ordering logic in
    // Thread::MakeExplicitFrameCallbacksForManagedFrame).
    Frame *pFrame;
    if (pCf->IsFrameless())
    {
#if defined(_X86_)
        LPVOID limitSP = GetRegdisplaySP(&rd);
#else
        LPVOID limitSP = (LPVOID)( EECodeManager::GetCallerSp(&rd) );
#endif
        pFrame = GetFrame();
        while (pFrame && pFrame < limitSP)
            pFrame = pFrame->Next();
    }
    else
        pFrame = pCf->GetFrame();

    StackWalkFramesEx(&rd, CerStackCrawlCallBack, &sContext, QUICKUNWIND | FUNCTIONSONLY, pFrame);

    _ASSERTE(!sContext.m_fFirstFrame);

    return sContext.m_fWithinCer;
#else
#error unsupported platform
#endif
}

// Context passed down through a stack crawl (see code below).
struct HasCerOnStackCrawlContext
{
    BOOL        fHasCeronStack;
};

StackWalkAction HasCerOnStackCrawlCallBack(CrawlFrame* pCf, void* data)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HasCerOnStackCrawlContext *pData = (HasCerOnStackCrawlContext *)data;

    // Get the method for this frame if it exists (might not be a managed method, so check the explicit frame if that's what we're
    // looking at).
    MethodDesc *pMD = pCf->GetFunction();
    if (pMD == NULL && pCf->GetFrame() != NULL)
        pMD = pCf->GetFrame()->GetFunction();

    // Non-method frames don't interest us.
    if (pMD == NULL)
        return SWA_CONTINUE;

    // Check for CER root methods (these are never inlined). If we've found one of these at the root of a bunch of potential CER
    // methods (i.e. those with a compatible reliability contract) then we're executing in a CER.
    if (IsCerRootMethod(pMD))
    {
        pData->fHasCeronStack = TRUE;
        return SWA_ABORT;
    }


    return SWA_CONTINUE;
}


// This call is used to decide if there is cer backout code on stack
BOOL Thread::HasCerOnStack()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (!g_fEEStarted)
        return FALSE;

    Thread *pThread = GetThread();
    _ASSERTE (pThread);
    HasCerOnStackCrawlContext sContext = { FALSE };

    pThread->StackWalkFrames(HasCerOnStackCrawlCallBack, &sContext);

    return sContext.fHasCeronStack;
}

#define CONTEXT_MIN_STACKWALK (CONTEXT_CONTROL)


BOOL Thread::ReadyForAsyncException(ThreadInterruptMode mode)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (((mode & TI_Abort) != 0 && !IsAbortRequested()) ||
        ((mode & TI_Interrupt) != 0 && (m_UserInterrupt & TI_Interrupt) == 0))
    {
        return FALSE;
    }

    if (IsAbortRequested() && HasThreadStateNC(TSNC_SOWorkNeeded))
    {
        return TRUE;
    }

    // This needs the probe with GenerateHardSO
    CONTRACT_VIOLATION(SOToleranceViolation);

    if (GetThread() == this && HasThreadStateNC (TSNC_PreparingAbort) && !IsRudeAbort() )
    {
        STRESS_LOG0(LF_APPDOMAIN, LL_INFO10, "in Thread::ReadyForAbort  PreparingAbort\n");
        // Avoid recursive call
        return FALSE;
    }

    if (IsThreadAbortPrevented())
    {
        STRESS_LOG0(LF_APPDOMAIN, LL_INFO10, "in Thread::ReadyForAbort  prevent abort\n");
        return FALSE;
    }

    // The thread requests not to be aborted.  Honor this for safe abort.
    if (!IsRudeAbort() && IsAsyncPrevented())
    {
        STRESS_LOG0(LF_APPDOMAIN, LL_INFO10, "in Thread::ReadyForAbort  AsyncPrevented\n");
        return FALSE;
    }

    // If we are doing safe abort, we can not abort a thread if it has locks.
    if (m_AbortType == EEPolicy::TA_Safe && HasLockInCurrentDomain()) {
        STRESS_LOG0(LF_APPDOMAIN, LL_INFO10, "in Thread::ReadyForAbort  HasLock\n");
        return FALSE;
    }

#ifdef _X86_
    // On X86 we don't abort threads that are performing a P/Invoke that returns
    // a SafeHandle or CriticalHandle (doing so could leak the native resource).
    // This is handled differently on platforms using IL stubs (we treat those
    // stubs as CERs). For non-fully optimized stubs (where we're doing at least
    // part of the marshaling via ML) we simply hold the prevent abort count
    // round the sensitive portions of code. For fully optimized stubs we try
    // for maximum performance and push the work onto the aborting thread (i.e.
    // here). We look at the first frame of the thread to see if it's one of the
    // type used for P/Invoke fully optimized stubs. If so and the
    // FLAG_SKIP_HANDLE_THREAD_ABORT is set, then we have a case were we should
    // not abort yet.
    Frame *pFrame = GetFrame();
    if (pFrame != 0 && pFrame != FRAME_TOP)
    {
        TADDR frameType = pFrame->GetVTablePtr();
        BOOL fReady = TRUE;
        if (frameType == NDirectMethodFrameStandalone::GetMethodFrameVPtr())
            fReady = (((NDirectMethodFrame*)pFrame)->GetNegInfo()->m_flags & FLAG_SKIP_HANDLE_THREAD_ABORT) == 0;
        else if (frameType == NDirectMethodFrameStandaloneCleanup::GetMethodFrameVPtr())
            fReady = (((NDirectMethodFrame*)pFrame)->GetNegInfo()->m_flags & FLAG_SKIP_HANDLE_THREAD_ABORT) == 0;
        if (!fReady)
        {
            STRESS_LOG0(LF_APPDOMAIN, LL_INFO10, "in Thread::ReadyForAbort  p/invoke returning safehandle\n");
            return FALSE;
        }
    }
#endif

    REGDISPLAY rd;

    if (ThrewControlForThread() == Thread::InducedThreadRedirect)
    {
        _ASSERTE(GetThread() == this);
        _ASSERTE(ExecutionManager::FindCodeMan((SLOT)GetIP(m_OSContext)));
        FillRegDisplay(&rd, m_OSContext);
    }
    else
    {
        if (GetFilterContext())
        {
            FillRegDisplay(&rd, GetFilterContext());
        }
        else
        {
             CONTEXT ctx;
             SetIP(&ctx, 0);
             SetSP(&ctx, 0);
             FillRegDisplay(&rd, &ctx);
        }
    }

#ifdef STRESS_LOG
    REGDISPLAY rd1;
    if (StressLog::StressLogOn(-1, 0))
    {
        CONTEXT ctx1;
        CopyRegDisplay(&rd, &rd1, &ctx1);
    }
#endif

    // Walk the stack to determine if we are running in Constrained Execution Region or finally EH clause (in the non-rude abort
    // case). We cannot initiate an abort in these circumstances.
    StackCrawlContext TAContext =
    {
        this,
        StackCrawlContext::SCC_CheckWithinCer | (IsRudeAbort() ? 0 : StackCrawlContext::SCC_CheckWithinEH),
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE
    };

    StackWalkFramesEx(&rd, TAStackCrawlCallBack, &TAContext, QUICKUNWIND, NULL);

    if (!TAContext.fHasManagedCodeOnStack && IsAbortInitiated() && GetThread() == this)
    {
        EEResetAbort(TAR_Thread);
        return FALSE;
    }

    if (TAContext.fWithinCer)
    {
        STRESS_LOG0(LF_APPDOMAIN, LL_INFO10, "in Thread::ReadyForAbort  RunningCer\n");
        return FALSE;
    }

#ifdef STRESS_LOG
    if (StressLog::StressLogOn(-1, 0) &&
        (IsRudeAbort() || !TAContext.fWithinEHClause))
    {
        //Save into stresslog.
        StackCrawlContext TAContext1 =
        {
            this,
            StackCrawlContext::SCC_CheckWithinCer | (IsRudeAbort() ? 0 : StackCrawlContext::SCC_CheckWithinEH),
            FALSE,
            FALSE,
            FALSE,
            FALSE,
            TRUE
        };

        StackWalkFramesEx(&rd1, TAStackCrawlCallBack, &TAContext1, QUICKUNWIND, NULL);
    }
#endif

    if (IsRudeAbort()) {
        // If it is rude abort, there is no additional restriction on abort.
        STRESS_LOG0(LF_APPDOMAIN, LL_INFO10, "in Thread::ReadyForAbort  RudeAbort\n");
        return TRUE;
    }

    if (TAContext.fWithinEHClause)
    {
        STRESS_LOG0(LF_APPDOMAIN, LL_INFO10, "in Thread::ReadyForAbort  RunningEHClause\n");
    }

    //if (m_AbortType == EEPolicy::TA_V1Compatible) {
    //    return TRUE;
    //}

    // If we are running finally, we can not abort for Safe Abort.
    return !TAContext.fWithinEHClause;
}

BOOL Thread::IsRudeAbort()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return (IsAbortRequested() && (m_AbortType == EEPolicy::TA_Rude));
}

BOOL Thread::IsRudeAbortOnlyForADUnload()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return (IsAbortRequested() &&
            (m_AbortInfo & TAI_ADUnloadRudeAbort) &&
            !(m_AbortInfo & (TAI_ThreadRudeAbort | TAI_FuncEvalRudeAbort))
           );
}

BOOL Thread::IsRudeUnload()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return (IsAbortRequested() && (m_AbortInfo & TAI_ADUnloadRudeAbort));
}

BOOL Thread::IsFuncEvalAbort()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return (IsAbortRequested() && (m_AbortInfo & TAI_AnyFuncEvalAbort));
}



void Thread::SetAbortEndTime(ULONGLONG endTime, BOOL fRudeAbort)
{
    LEAF_CONTRACT;

    {
        AbortRequestLockHolder lh(this);
        if (fRudeAbort)
        {
            if (endTime < m_RudeAbortEndTime)
            {
                m_RudeAbortEndTime = endTime;
            }
        }
        else
        {
            if (endTime < m_AbortEndTime)
            {
                m_AbortEndTime = endTime;
            }
        }
    }

}

HRESULT
Thread::UserAbort(ThreadAbortRequester requester,
                  EEPolicy::ThreadAbortTypes abortType,
                  DWORD timeout,
                  UserAbort_Client client
                 )
{
    CONTRACTL
    {
        THROWS;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    STRESS_LOG2(LF_SYNC | LF_APPDOMAIN, LL_INFO100, "UserAbort Thread %p Thread Id = %x\n", this, m_ThreadId);
#ifdef _DEBUG
    AddFiberInfo(ThreadTrackInfo_Abort);
#endif

    BOOL fHoldingThreadStoreLock = ThreadStore::HoldingThreadStore();

    // For SafeAbort from FuncEval abort, we do not apply escalation policy.  Debugger
    // tries SafeAbort first with a short timeout.  The thread will return to debugger.
    // After some break, the thread is going to do RudeAbort if abort has not finished.
    EClrOperation operation;
    if (abortType == EEPolicy::TA_Rude)
    {
        if (HasLockInCurrentDomain())
        {
            operation = OPR_ThreadRudeAbortInCriticalRegion;
        }
        else
        {
            operation = OPR_ThreadRudeAbortInNonCriticalRegion;
        }
    }
    else
    {
        operation = OPR_ThreadAbort;
    }

    // Debugger func-eval aborts (both rude + normal) don't have any escalation policy. They are invoked
    // by the debugger and the debugger handles the consequences. 
    // Furthermore, in interop-debugging, threads will be hard-suspened in preemptive mode while we try to abort them.
    // So any abort strategy that relies on a timeout and the target thread slipping is dangerous. Escalation policy would let a 
    // host circumvent the timeout and thus we may wait forever for the target thread to slip. We'd deadlock here. Since the escalation
    // policy doesn't let the host break this deadlock (and certianly doesn't let the debugger break the deadlock), it's unsafe
    // to have an escalation policy for func-eval aborts at all.
    BOOL fEscalation = (requester != TAR_FuncEval);
    if (fEscalation)
    {
        EPolicyAction action = GetEEPolicy()->GetDefaultAction(operation, this);
        switch (action)
        {
        case eAbortThread:
            GetEEPolicy()->NotifyHostOnDefaultAction(operation,action);
            break;
        case eRudeAbortThread:
            if (abortType != EEPolicy::TA_Rude)
            {
                abortType = EEPolicy::TA_Rude;
            }
            GetEEPolicy()->NotifyHostOnDefaultAction(operation,action);
            break;
        case eUnloadAppDomain:
            {
                AppDomain *pDomain = GetDomain();
                if (!pDomain->IsDefaultDomain())
                {
                    GetEEPolicy()->NotifyHostOnDefaultAction(operation,action);
                    pDomain->EnableADUnloadWorker(EEPolicy::ADU_Safe);
                }
            }
            // AD unload does not abort finalizer thread.
            if (this != GCHeap::GetGCHeap()->GetFinalizerThread())
            {
                if (this == GetThread())
                {
                    Join(INFINITE,TRUE);
                }
                return S_OK;
            }
            break;
        case eRudeUnloadAppDomain:
            {
                AppDomain *pDomain = GetDomain();
                if (!pDomain->IsDefaultDomain())
                {
                    GetEEPolicy()->NotifyHostOnDefaultAction(operation,action);
                    pDomain->EnableADUnloadWorker(EEPolicy::ADU_Rude);
                }
            }
            // AD unload does not abort finalizer thread.
            if (this != GCHeap::GetGCHeap()->GetFinalizerThread())
            {
                if (this == GetThread())
                {
                    Join(INFINITE,TRUE);
                }
                return S_OK;
            }
            break;
        case eExitProcess:
        case eFastExitProcess:
        case eRudeExitProcess:
        case eDisableRuntime:
            GetEEPolicy()->NotifyHostOnDefaultAction(operation,action);
            EEPolicy::HandleExitProcessFromEscalation(action, HOST_E_EXITPROCESS_THREADABORT);
            _ASSERTE (!"Should not reach here");
            break;
        default:
            _ASSERTE (!"unknown policy for thread abort");
        }

        DWORD timeoutFromPolicy;
        if (abortType != EEPolicy::TA_Rude)
        {
            timeoutFromPolicy = GetEEPolicy()->GetTimeout(OPR_ThreadAbort);
        }
        else if (!HasLockInCurrentDomain())
        {
            timeoutFromPolicy = GetEEPolicy()->GetTimeout(OPR_ThreadRudeAbortInNonCriticalRegion);
        }
        else
        {
            timeoutFromPolicy = GetEEPolicy()->GetTimeout(OPR_ThreadRudeAbortInCriticalRegion);
        }
        if (timeout > timeoutFromPolicy)
        {
            timeout = timeoutFromPolicy;
        }
    }

    AbortControlHolder AbortController(this);

    // Swap in timeout
    if (timeout != INFINITE)
    {
        ULONG64 curTime = CLRGetTickCount64();
        ULONG64 newEndTime = curTime + timeout;

        SetAbortEndTime(newEndTime, abortType == EEPolicy::TA_Rude);
    }

    MarkThreadForAbort(requester, abortType);

    Thread *pCurThread = GetThread();

    // If aboring self
    if (this == pCurThread)
    {
        SetAbortInitiated();
#ifdef _DEBUG
        m_dwAbortPoint = 1;
#endif

        if (CLRHosted() && GetAbortEndTime() != MAXULONGLONG)
        {
            AppDomain::EnableADUnloadWorkerForThreadAbort();
        }

        GCX_COOP();

        OBJECTREF exceptObj;

        if (IsRudeAbort())
        {
            exceptObj = CLRException::GetPreallocatedRudeThreadAbortException();
        }
        else
        {
            EEException eeExcept(kThreadAbortException);
            exceptObj = CLRException::GetThrowableFromException(&eeExcept);
        }

        RaiseTheExceptionInternalOnly(exceptObj, FALSE);
    }


    _ASSERTE(this != pCurThread);      // Aborting another thread.

    if (client == UAC_Host)
    {
        // A host may call ICLRTask::Abort on a critical thread.  We don't want to
        // block this thread.
        AppDomain::EnableADUnloadWorkerForThreadAbort();
        return S_OK;
    }

#ifdef _DEBUG
    DWORD elapsed_time = 0;
#endif

    ThreadAffinityHolder affinity;
    // We do not want this thread to be alerted.
    ThreadPreventAsyncHolder preventAsync(pCurThread != NULL);

#ifdef _DEBUG
    // If UserAbort times out, put up msgbox once.
    BOOL fAlreadyAssert = FALSE;
#endif

    BOOL fOneTryOnly = (client == UAC_WatchDog); 
    BOOL fFirstRun = TRUE;
    BOOL fNeedEscalation;

LRetry:
    fNeedEscalation = FALSE;
    for (;;)
    {
        if (fOneTryOnly)
        {
            if (!fFirstRun)
            {
                return S_OK;
            }
            fFirstRun = FALSE;
        }
        // Lock the thread store
        LOG((LF_SYNC, INFO3, "UserAbort obtain lock\n"));

        ULONGLONG abortEndTime = GetAbortEndTime();
        if (abortEndTime != MAXULONGLONG)
        {
            ULONGLONG now_time = CLRGetTickCount64();

            if (now_time >= abortEndTime)
            {
                EPolicyAction action1 = eNoAction;
                DWORD timeout1 = INFINITE;
                if (fEscalation)
                {
                    if (!IsRudeAbort())
                    {
                        action1 = GetEEPolicy()->GetActionOnTimeout(OPR_ThreadAbort, this);
                        timeout1 = GetEEPolicy()->GetTimeout(OPR_ThreadAbort);
                    }
                    else if (HasLockInCurrentDomain())
                    {
                        action1 = GetEEPolicy()->GetActionOnTimeout(OPR_ThreadRudeAbortInCriticalRegion, this);
                        timeout1 = GetEEPolicy()->GetTimeout(OPR_ThreadRudeAbortInCriticalRegion);
                    }
                    else
                    {
                        action1 = GetEEPolicy()->GetActionOnTimeout(OPR_ThreadRudeAbortInNonCriticalRegion, this);
                        timeout1 = GetEEPolicy()->GetTimeout(OPR_ThreadRudeAbortInNonCriticalRegion);
                    }
                }
                if (action1 == eNoAction)
                {
                    // timeout, but no action on timeout.
                    // Debugger can call this function to about func-eval with a timeout
                    return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
                }
                if (timeout1 != INFINITE)
                {
                    // There is an escalation policy.
                    fNeedEscalation = TRUE;
                    break;
                }
            }
        }

        // Thread abort needs to walk stack to decide if thread abort can proceed.
        // It is unsafe to crawl a stack of thread if the thread is OS-suspended which we do during
        // thread abort.  For example, Thread T1 aborts thread T2.  T2 is suspended by T1. Inside SQL
        // this means that no thread sharing the same scheduler with T2 can run.  If T1 needs a lock which
        // is owned by one thread on the scheduler, T1 will wait forever.
        // Our solution is to move T2 to a safe point, resume it, and then do stack crawl.

        // We need to make sure that ThreadStoreLock is released after CheckForAbort.  This makes sure
        // that ThreadAbort does not race against GC.
        class CheckForAbort
        {
        private:
            Thread *m_pThread;
            BOOL m_fHoldingThreadStoreLock;
            BOOL m_NeedRelease;
        public:
            CheckForAbort(Thread *pThread, BOOL fHoldingThreadStoreLock)
            : m_pThread(pThread),
              m_fHoldingThreadStoreLock(fHoldingThreadStoreLock),
              m_NeedRelease(TRUE)
            {
                if (!fHoldingThreadStoreLock)
                {
                    ThreadStore::LockThreadStore(GCHeap::SUSPEND_OTHER);
                }
                ThreadStore::ResetStackCrawlEvent();

                // The thread being aborted may clear the TS_AbortRequested bit and the matching increment
                // of g_TrapReturningThreads behind our back. Increment g_TrapReturningThreads here
                // to ensure that we stop for the stack crawl even if the TS_AbortRequested bit is cleared.
                ThreadStore::TrapReturningThreads(TRUE);
            }
            void NeedStackCrawl()
            {
                m_pThread->SetThreadState(Thread::TS_StackCrawlNeeded);
            }
            ~CheckForAbort()
            {
                Release();
            }
            void Release()
            {
                if (m_NeedRelease)
                {
                    m_NeedRelease = FALSE;
                    ThreadStore::TrapReturningThreads(FALSE);
                    ThreadStore::SetStackCrawlEvent();
                    m_pThread->ResetThreadState(TS_StackCrawlNeeded);
                    if (!m_fHoldingThreadStoreLock)
                    {
                        ThreadStore::UnlockThreadStore();
                    }
                }
            }
        };
        CheckForAbort checkForAbort(this, fHoldingThreadStoreLock);

        // We own TS lock.  The state of the Thread can not be changed.
        if (m_State & TS_Unstarted)
        {
            // This thread is not yet started.
#ifdef _DEBUG
            m_dwAbortPoint = 2;
#endif
            return S_OK;
        }

        if (GetThreadHandle() == INVALID_HANDLE_VALUE &&
            (m_State & TS_Unstarted) == 0)
        {
            // The thread is going to die or is already dead.
            UnmarkThreadForAbort(Thread::TAR_ALL);
#ifdef _DEBUG
            m_dwAbortPoint = 3;
#endif
            return S_OK;
        }

        // What if someone else has this thread suspended already?   It'll depend where the
        // thread got suspended.
        //
        // User Suspend:
        //     We'll just set the abort bit and hope for the best on the resume.
        //
        // GC Suspend:
        //    If it's suspended in jitted code, we'll hijack the IP.
        //    If it's suspended but not in jitted code, we'll get suspended for GC, the GC
        //    will complete, and then we'll abort the target thread.
        //

        // It's possible that the thread has completed the abort already.
        //
        if (!(m_State & TS_AbortRequested))
        {
#ifdef _DEBUG
            m_dwAbortPoint = 4;
#endif
            return S_OK;
        }

        // If a thread is Dead or Detached, abort is a NOP.
        //
        if (m_State & (TS_Dead | TS_Detached | TS_TaskReset))
        {
            UnmarkThreadForAbort(Thread::TAR_ALL);
#ifdef _DEBUG
            m_dwAbortPoint = 5;
#endif
            return S_OK;
        }

        // It's possible that some stub notices the AbortRequested bit -- even though we
        // haven't done any real magic yet.  If the thread has already started it's abort, we're
        // done.
        //
        // Two more cases can be folded in here as well.  If the thread is unstarted, it'll
        // abort when we start it.
        //
        // If the thread is user suspended (SyncSuspended) -- we're out of luck.  Set the bit and
        // hope for the best on resume.
        //
        if ((m_State & TS_AbortInitiated) && !IsRudeAbort())
        {
#ifdef _DEBUG
            m_dwAbortPoint = 6;
#endif
            break;
        }

        BOOL fOutOfRuntime = FALSE;
        BOOL fNeedStackCrawl = FALSE;

        // Win32 suspend the thread, so it isn't moving under us.
        SuspendThreadResult str = SuspendThread();
        switch (str)
        {
        case STR_Success:
            break;

        case STR_Failure:
        case STR_UnstartedOrDead:
        case STR_NoStressLog:
            checkForAbort.Release();
            __SwitchToThread(0);
            continue;

        case STR_SwitchedOut:
            // If the thread is in preemptive gc mode, we can erect a barrier to block the
            // thread to return to cooperative mode.  Then we can do stack crawl and make decision.
            if (!m_fPreemptiveGCDisabled)
            {
                checkForAbort.NeedStackCrawl();
                if (GetThreadHandle() != SWITCHOUT_HANDLE_VALUE || m_fPreemptiveGCDisabled)
                {
                    checkForAbort.Release();
                    __SwitchToThread(0);
                    continue;
                }
                else
                {
                    goto LStackCrawl;
                }
            }
            else
            {
                goto LPrepareRetry;
            }

        default:
            UNREACHABLE();
        }

        _ASSERTE(str == STR_Success);

        // Check whether some stub noticed the AbortRequested bit in-between our test above
        // and us suspending the thread.
        if ((m_State & TS_AbortInitiated) && !IsRudeAbort())
        {
            ResumeThread();
#ifdef _DEBUG
            m_dwAbortPoint = 65;
#endif
            break;
        }

        // If Threads is stopped under a managed debugger, it will have both
        // TS_DebugSuspendPending and TS_SyncSuspended, regardless of whether
        // the thread is actually suspended or not.
        // If it's suspended w/o the debugger (eg, by via Thread.Suspend), it will
        // also have TS_UserSuspendPending set.
        if (m_State & TS_SyncSuspended)
        {
            ResumeThread();
            checkForAbort.Release();
#ifdef _DEBUG
            m_dwAbortPoint = 7;
#endif

            //
            // If it's stopped by the debugger, we don't want to throw an exception.
            // Debugger suspension is to have no effect of the runtime behaviour.
            //
            if (m_State & TS_DebugSuspendPending)
            {
                return S_OK;
            }

            COMPlusThrow(kThreadStateException, IDS_EE_THREAD_ABORT_WHILE_SUSPEND);
        }

        // If the thread has no managed code on it's call stack, abort is a NOP.  We're about
        // to touch the unmanaged thread's stack -- for this to be safe, we can't be
        // Dead/Detached/Unstarted.
        //
        _ASSERTE(!(m_State & (  TS_Dead
                              | TS_Detached
                              | TS_Unstarted)));

#ifdef _X86_
        if ((m_pFrame == FRAME_TOP)
            && (GetFirstCOMPlusSEHRecord(this) == EXCEPTION_CHAIN_END)
           )
        {
            ResumeThread();
#ifdef _DEBUG
            m_dwAbortPoint = 8;
#endif
            return S_OK;
        }
#endif // _X86_


        if (!m_fPreemptiveGCDisabled)
        {
            if ((m_pFrame != FRAME_TOP) && m_pFrame->IsTransitionToNativeFrame()
#ifdef _X86_
                && ((size_t) GetFirstCOMPlusSEHRecord(this) > ((size_t) m_pFrame) - 20)
#endif // _X86_
                )
            {
                fOutOfRuntime = TRUE;
            }
        }

        checkForAbort.NeedStackCrawl();
        if (!m_fPreemptiveGCDisabled)
        {
            fNeedStackCrawl = TRUE;
        }

        // We should not walk the stack if the thread is cooperative.
        _ASSERTE (!fNeedStackCrawl || !m_fPreemptiveGCDisabled);

        // The thread is not suspended now.
        ResumeThread();

        if (!fNeedStackCrawl)
        {
            goto LPrepareRetry;
        }

LStackCrawl:
        if (!ReadyForAbort()) {
            goto LPrepareRetry;
        }

        // !!! Check for Exception in flight should happen before induced thread abort.
        // !!! ReadyForAbort skips catch and filter clause.

        // If an exception is currently being thrown, one of two things will happen.  Either, we'll
        // catch, and notice the abort request in our end-catch, or we'll not catch [in which case
        // we're leaving managed code anyway.  The top-most handler is responsible for resetting
        // the bit.
        //
        if (HasException() &&
            // For rude abort, we will initiated abort
            !IsRudeAbort())
        {
#ifdef _DEBUG
            m_dwAbortPoint = 9;
#endif
            break;
        }

        // If the thread is in sleep, wait, or join interrupt it
        // However, we do NOT want to interrupt if the thread is already processing an exception
        if (m_State & TS_Interruptible)
        {
            UserInterrupt(TI_Abort);        // if the user wakes up because of this, it will read the
                                            // abort requested bit and initiate the abort
#ifdef _DEBUG
            m_dwAbortPoint = 10;
#endif
            goto LPrepareRetry;
        }

        if (fOutOfRuntime)
        {
            // If the thread is running outside the EE, and is behind a stub that's going
            // to catch...
#ifdef _DEBUG
            m_dwAbortPoint = 11;
#endif
            break;
        }

        // Ok.  It's not in managed code, nor safely out behind a stub that's going to catch
        // it on the way in.  We have to poll.

LPrepareRetry:

        checkForAbort.Release();

        if (fOneTryOnly)
        {
            break;
        }

        // Don't do a Sleep.  It's possible that the thread we are trying to abort is
        // stuck in unmanaged code trying to get into the apartment that we are supposed
        // to be pumping!  Instead, ping the current thread's handle.  Obviously this
        // will time out, but it will pump if we need it to.
        if (pCurThread)
        {
            pCurThread->Join(ABORT_POLL_TIMEOUT, TRUE);
        }
        else
        {
            ClrSleepEx(ABORT_POLL_TIMEOUT, FALSE);
        }


#ifdef _DEBUG
        elapsed_time += ABORT_POLL_TIMEOUT;
        if (g_pConfig->GetGCStressLevel() == 0 && !fAlreadyAssert)
        {
            _ASSERTE(elapsed_time < ABORT_FAIL_TIMEOUT);
            fAlreadyAssert = TRUE;
        }
#endif

    } // for(;;)

    if (fOneTryOnly  && !fNeedEscalation)
    {
        return S_OK;
    }

    if ((GetAbortEndTime() != MAXULONGLONG)  && IsAbortRequested())
    {
        while (TRUE)
        {
            if (!IsAbortRequested())
            {
                return S_OK;
            }
            ULONGLONG curTime = CLRGetTickCount64();
            if (curTime >= GetAbortEndTime())
            {
                break;
            }

            if (pCurThread)
            {
                pCurThread->Join(100, TRUE);
            }
            else
            {
                ClrSleepEx(100, FALSE);
            }

        }

        if (IsAbortRequested() && fEscalation)
        {
            EPolicyAction action1;
            EClrOperation operation1;
            if (!IsRudeAbort())
            {
                operation1 = OPR_ThreadAbort;
            }
            else if (HasLockInCurrentDomain())
            {
                operation1 = OPR_ThreadRudeAbortInCriticalRegion;
            }
            else
            {
                operation1 = OPR_ThreadRudeAbortInNonCriticalRegion;
            }
            action1 = GetEEPolicy()->GetActionOnTimeout(operation1, this);
            switch (action1)
            {
            case eRudeAbortThread:
                GetEEPolicy()->NotifyHostOnTimeout(operation1, action1);
                MarkThreadForAbort(requester, EEPolicy::TA_Rude);
                SetRudeAbortEndTimeFromEEPolicy();
                goto LRetry;
            case eUnloadAppDomain:
                {
                    AppDomain *pDomain = GetDomain();
                    if (!pDomain->IsDefaultDomain())
                    {
                        GetEEPolicy()->NotifyHostOnTimeout(operation1, action1);
                        pDomain->EnableADUnloadWorker(EEPolicy::ADU_Safe);
                    }
                }
                // AD unload does not abort finalizer thread.
                if (this == GCHeap::GetGCHeap()->GetFinalizerThread())
                {
                    GetEEPolicy()->NotifyHostOnTimeout(operation1, action1);
                    MarkThreadForAbort(requester, EEPolicy::TA_Rude);
                    SetRudeAbortEndTimeFromEEPolicy();
                    goto LRetry;
                }
                else
                {
                    if (this == GetThread())
                    {
                        Join(INFINITE,TRUE);
                    }
                    return S_OK;
                }
                break;
            case eRudeUnloadAppDomain:
                {
                    AppDomain *pDomain = GetDomain();
                    if (!pDomain->IsDefaultDomain())
                    {
                        GetEEPolicy()->NotifyHostOnTimeout(operation1, action1);
                        pDomain->EnableADUnloadWorker(EEPolicy::ADU_Rude);
                    }
                }
                // AD unload does not abort finalizer thread.
                if (this == GCHeap::GetGCHeap()->GetFinalizerThread())
                {
                    MarkThreadForAbort(requester, EEPolicy::TA_Rude);
                    SetRudeAbortEndTimeFromEEPolicy();
                    goto LRetry;
                }
                else
                {
                    if (this == GetThread())
                    {
                        Join(INFINITE,TRUE);
                    }
                    return S_OK;
                }
                break;
            case eExitProcess:
            case eFastExitProcess:
            case eRudeExitProcess:
            case eDisableRuntime:
                GetEEPolicy()->NotifyHostOnTimeout(operation1, action1);
                EEPolicy::HandleExitProcessFromEscalation(action1, HOST_E_EXITPROCESS_TIMEOUT);
                _ASSERTE (!"Should not reach here");
                break;
            default:
            break;
            }
        }

        return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
    }

    return S_OK;
}

void Thread::SetRudeAbortEndTimeFromEEPolicy()
{
    DWORD timeout;
    if (HasLockInCurrentDomain())
    {
        timeout = GetEEPolicy()->GetTimeout(OPR_ThreadRudeAbortInCriticalRegion);
    }
    else
    {
        timeout = GetEEPolicy()->GetTimeout(OPR_ThreadRudeAbortInCriticalRegion);
    }
    ULONGLONG newEndTime;
    if (timeout == INFINITE)
    {
        newEndTime = MAXULONGLONG;
    }
    else
    {
        newEndTime = CLRGetTickCount64() + timeout;
    }

    SetAbortEndTime(newEndTime, TRUE);
}

ULONGLONG Thread::s_NextSelfAbortEndTime = MAXULONGLONG;

void Thread::ThreadAbortWatchDogHelper(Thread *pThread)
{
    CONTRACTL
    {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    if ((!pThread->IsAbortInitiated() ||
         pThread->IsRudeAbort()) &&
        (pThread->GetAbortEndTime() == MAXULONGLONG ||
         pThread->GetAbortEndTime() >= CLRGetTickCount64()))
    {
        EEPolicy::ThreadAbortTypes abortType = EEPolicy::TA_Safe;
        if (pThread->m_AbortInfo & TAI_ThreadRudeAbort)
        {
            abortType = EEPolicy::TA_Rude;
        }
        else if (pThread->m_AbortInfo & TAI_ThreadV1Abort)
        {
            abortType = EEPolicy::TA_V1Compatible;
        }
        else if (pThread->m_AbortInfo & TAI_ThreadAbort)
        {
            abortType = EEPolicy::TA_Safe;
        }
        else
        {
            return;
        }
        EX_TRY
        {
            pThread->UserAbort(Thread::TAR_Thread, abortType, INFINITE, Thread::UAC_WatchDog);            
        }
        EX_CATCH
        {
        }
        EX_END_CATCH(SwallowAllExceptions);
        return;
    }

    EPolicyAction action = eNoAction;
    EClrOperation operation = OPR_ThreadRudeAbortInNonCriticalRegion;
    if (!pThread->IsRudeAbort())
    {
        operation = OPR_ThreadAbort;
    }
    else if (pThread->HasLockInCurrentDomain())
    {
        operation = OPR_ThreadRudeAbortInCriticalRegion;
    }
    else
    {
        operation = OPR_ThreadRudeAbortInNonCriticalRegion;
    }
    action = GetEEPolicy()->GetActionOnTimeout(operation, pThread);
    // We only support escalation to rude abort

    EX_TRY {
        switch (action)
        {
        case eRudeAbortThread:
            GetEEPolicy()->NotifyHostOnTimeout(operation,action);
            pThread->UserAbort(Thread::TAR_Thread, EEPolicy::TA_Rude, INFINITE, Thread::UAC_WatchDog);
            break;
        case eUnloadAppDomain:
            {
                AppDomain *pDomain = pThread->GetDomain();
                if (!pDomain->IsDefaultDomain())
                {
                    GetEEPolicy()->NotifyHostOnTimeout(operation,action);
                    pDomain->EnableADUnloadWorker(EEPolicy::ADU_Safe);
                }
            }
            break;
        case eRudeUnloadAppDomain:
            {
                AppDomain *pDomain = pThread->GetDomain();
                if (!pDomain->IsDefaultDomain())
                {
                    GetEEPolicy()->NotifyHostOnTimeout(operation,action);
                    pDomain->EnableADUnloadWorker(EEPolicy::ADU_Rude);
                }
            }
            break;
        case eExitProcess:
        case eFastExitProcess:
        case eRudeExitProcess:
        case eDisableRuntime:
            // HandleExitProcessFromEscalation will try to grab ThreadStore again.
            _ASSERTE (ThreadStore::HoldingThreadStore());
            ThreadStore::UnlockThreadStore();
            GetEEPolicy()->NotifyHostOnTimeout(operation,action);
            EEPolicy::HandleExitProcessFromEscalation(action, HOST_E_EXITPROCESS_THREADABORT);
            _ASSERTE (!"Should not reach here");
            break;
        case eNoAction:
            break;
        default:
            _ASSERTE (!"unknown policy for thread abort");
        }
    }
    EX_CATCH {
    }
    EX_END_CATCH(SwallowAllExceptions);
}

// If a thread is self-aborted and has a timeout, we need to watch the thread
void Thread::ThreadAbortWatchDog()
{
    CONTRACTL
    {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    if (CLRHosted())
    {
        s_NextSelfAbortEndTime = MAXULONGLONG;

        Thread *thread = NULL;

        ULONGLONG curTime = CLRGetTickCount64();

        {
            ThreadStoreLockHolder tsLock(TRUE);

            while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
            {
                if (!thread->IsAbortRequested())
                {
                    continue;
                }
                ULONGLONG endTime = thread->GetAbortEndTime();

                if (!thread->IsAbortInitiated() ||
                    thread->IsRudeAbort())
                {
                    endTime = 0;
                }
                if (endTime != MAXULONGLONG)
                {
                    STRESS_LOG3(LF_ALWAYS, LL_ALWAYS, "ThreadAbortWatchDog for Thread %p Thread Id = %x with timeout %x\n",
                                thread, thread->GetThreadId(), endTime);
                    if (endTime < s_NextSelfAbortEndTime)
                    {
                        s_NextSelfAbortEndTime = endTime;
                    }
                    if (curTime >= endTime && thread->m_AbortController == 0)
                    {
                        ThreadAbortWatchDogHelper(thread);
                    }
                }
            }
        }
    }
}

void Thread::LockAbortRequest(Thread* pThread)
{
    WRAPPER_CONTRACT;

    while (TRUE) {
        for (unsigned i = 0; i < 10000; i ++) {
            if (*((volatile LONG*)&(pThread->m_AbortRequestLock)) == 0) {
                break;
            }
            YieldProcessor();               // indicate to the processor that we are spinning
        }
        if (FastInterlockCompareExchange(&(pThread->m_AbortRequestLock),1,0) == 0) {
            return;
        }
        __SwitchToThread(0);
    }
}

void Thread::UnlockAbortRequest(Thread *pThread)
{
    LEAF_CONTRACT;

    _ASSERTE (pThread->m_AbortRequestLock == 1);
    FastInterlockExchange(&pThread->m_AbortRequestLock, 0);
}

// returns 0 if the thread is already marked to be aborted.
void Thread::MarkThreadForAbort(ThreadAbortRequester requester, EEPolicy::ThreadAbortTypes abortType)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    AbortRequestLockHolder lh(this);

#ifdef _DEBUG
    if (abortType == EEPolicy::TA_Rude)
    {
        m_fRudeAborted = TRUE;
    }
#endif

    DWORD abortInfo = 0;

    if (requester & TAR_Thread)
    {
        if (abortType == EEPolicy::TA_Safe)
        {
            abortInfo |= TAI_ThreadAbort;
        }
        else if (abortType == EEPolicy::TA_Rude)
        {
            abortInfo |= TAI_ThreadRudeAbort;
        }
        else if (abortType == EEPolicy::TA_V1Compatible)
        {
            abortInfo |= TAI_ThreadV1Abort;
        }
    }

    if (requester & TAR_ADUnload)
    {
        if (abortType == EEPolicy::TA_Safe)
        {
            abortInfo |= TAI_ADUnloadAbort;
        }
        else if (abortType == EEPolicy::TA_Rude)
        {
            abortInfo |= TAI_ADUnloadRudeAbort;
        }
        else if (abortType == EEPolicy::TA_V1Compatible)
        {
            abortInfo |= TAI_ADUnloadV1Abort;
        }
        if (IsADUnloadHelperThread())
        {
            abortInfo |= TAI_ForADUnloadThread;
        }
    }

    if (requester & TAR_FuncEval)
    {
        if (abortType == EEPolicy::TA_Safe)
        {
            abortInfo |= TAI_FuncEvalAbort;
        }
        else if (abortType == EEPolicy::TA_Rude)
        {
            abortInfo |= TAI_FuncEvalRudeAbort;
        }
        else if (abortType == EEPolicy::TA_V1Compatible)
        {
            abortInfo |= TAI_FuncEvalV1Abort;
        }
    }

    if (abortInfo == 0)
    {
        ASSERT(!"Invalid abort information");
        return;
    }

    if (requester == TAR_Thread)
    {
        DWORD timeoutFromPolicy;
        if (abortType != EEPolicy::TA_Rude)
        {
            timeoutFromPolicy = GetEEPolicy()->GetTimeout(OPR_ThreadAbort);
        }
        else if (!HasLockInCurrentDomain())
        {
            timeoutFromPolicy = GetEEPolicy()->GetTimeout(OPR_ThreadRudeAbortInNonCriticalRegion);
        }
        else
        {
            timeoutFromPolicy = GetEEPolicy()->GetTimeout(OPR_ThreadRudeAbortInCriticalRegion);
        }
        if (timeoutFromPolicy != INFINITE)
        {
            ULONGLONG endTime = CLRGetTickCount64() + timeoutFromPolicy;
            if (abortType != EEPolicy::TA_Rude)
            {
                if (endTime < m_AbortEndTime)
                {
                    m_AbortEndTime = endTime;
                }
            }
            else if (endTime < m_RudeAbortEndTime)
            {
                m_RudeAbortEndTime = endTime;
            }
        }
        if (GetThread() == this)
        {
            AppDomain::EnableADUnloadWorkerForThreadAbort();
        }
    }

    if (abortInfo == (m_AbortInfo & abortInfo))
    {
        //
        // We are already doing this kind of abort.
        //
        return;
    }

    m_AbortInfo |= abortInfo;

    if (m_AbortType >= (DWORD)abortType)
    {
        // another thread is aborting at a higher level
        return;
    }

    m_AbortType = abortType;

    if (!IsAbortRequested())
    {
        // We must set this before we start flipping thread bits to avoid races where
        // trap returning threads is already high due to other reasons.

        // The thread is asked for abort the first time
        SetAbortRequestBit();

#ifdef _DEBUG
        AddFiberInfo(ThreadTrackInfo_Abort);
#endif
    }
    STRESS_LOG4(LF_APPDOMAIN, LL_ALWAYS, "Mark Thread %p Thread Id = %x for abort from requester %d (type %d)\n", this, m_ThreadId, requester, abortType);
}

void Thread::SetAbortRequestBit()
{
    WRAPPER_CONTRACT;
    while (TRUE)
    {
        volatile LONG curValue = (LONG)m_State;
        if ((curValue & TS_AbortRequested) != 0)
        {
            break;
        }
        if (FastInterlockCompareExchange((LONG*)&m_State, curValue|TS_AbortRequested, curValue) == curValue)
        {
            ThreadStore::TrapReturningThreads(TRUE);

            break;
        }
    }
}

void Thread::RemoveAbortRequestBit()
{
    WRAPPER_CONTRACT;

#ifdef _DEBUG
    // There's a race between removing the TS_AbortRequested bit and decrementing g_TrapReturningThreads
    // We may remove the bit, but before we have a chance to call ThreadStore::TrapReturningThreads(FALSE)
    // DbgFindThread() may execute, and find too few threads with the bit set.
    // To ensure the assert in DbgFindThread does not fire under such a race we set the ChgInFlight before hand.
    CounterHolder trtHolder(&g_trtChgInFlight);
#endif
    while (TRUE)
    {
        volatile LONG curValue = (LONG)m_State;
        if ((curValue & TS_AbortRequested) == 0)
        {
            break;
        }
        if (FastInterlockCompareExchange((LONG*)&m_State, curValue&(~TS_AbortRequested), curValue) == curValue)
        {
            ThreadStore::TrapReturningThreads(FALSE);

            break;
        }
    }
}

// Make sure that when AbortRequest bit is cleared, we also dec TrapReturningThreads count.
void Thread::UnmarkThreadForAbort(ThreadAbortRequester requester, BOOL fForce)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    AbortRequestLockHolder lh(this);

    //
    // Unmark the bits that are being turned off
    //
    if (requester & TAR_Thread)
    {
        if ((m_AbortInfo != TAI_ThreadRudeAbort) || fForce)
        {
            m_AbortInfo &= ~(TAI_ThreadAbort   |
                             TAI_ThreadV1Abort |
                             TAI_ThreadRudeAbort );
        }
    }

    if (requester & TAR_ADUnload)
    {
        m_AbortInfo &= ~(TAI_ADUnloadAbort   |
                         TAI_ADUnloadV1Abort |
                         TAI_ADUnloadRudeAbort);
    }

    if (requester & TAR_FuncEval)
    {
        m_AbortInfo &= ~(TAI_FuncEvalAbort   |
                         TAI_FuncEvalV1Abort |
                         TAI_FuncEvalRudeAbort);
    }

    //
    // Decide which type of abort to do based on the new bit field.
    //
    if (m_AbortInfo & TAI_AnyRudeAbort)
    {
        m_AbortType = EEPolicy::TA_Rude;
    }
    else if (m_AbortInfo & TAI_AnyV1Abort)
    {
        m_AbortType = EEPolicy::TA_V1Compatible;
        }
    else if (m_AbortInfo & TAI_AnySafeAbort)
    {
        m_AbortType = EEPolicy::TA_Safe;
    }
    else
    {
        m_AbortType = EEPolicy::TA_None;
    }

    //
    // If still aborting, do nothing
    //
    if (m_AbortType != EEPolicy::TA_None)
    {
        return;
    }

    m_AbortEndTime = MAXULONGLONG;
    m_RudeAbortEndTime = MAXULONGLONG;

    if (IsAbortRequested())
    {
        RemoveAbortRequestBit();
        FastInterlockAnd((DWORD*)&m_State,~(TS_AbortInitiated));
        m_fRudeAbortInitiated = FALSE;
        ResetUserInterrupted();

#ifdef _DEBUG
        AddFiberInfo(ThreadTrackInfo_Abort);
#endif
    }

    STRESS_LOG3(LF_APPDOMAIN, LL_ALWAYS, "Unmark Thread %p Thread Id = %x for abort from requester %d\n", this, m_ThreadId, requester);
}

// Make sure that when AbortRequest bit is cleared, we also dec TrapReturningThreads count.
void Thread::ResetBeginAbortedForADUnload()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    AbortRequestLockHolder lh(this);

    m_AbortInfo &= ~TAI_ForADUnloadThread;
}

void Thread::InternalResetAbort(ThreadAbortRequester requester, BOOL fResetRudeAbort)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(this == GetThread());
    _ASSERTE(!IsDead());

    // managed code can not reset Rude thread abort
    UnmarkThreadForAbort(requester, fResetRudeAbort);
#ifdef _DEBUG
    AddFiberInfo(ThreadTrackInfo_Abort);
#endif
}


// Throw a thread abort request when a suspended thread is resumed. Make sure you know what you
// are doing when you call this routine.
void Thread::SetAbortRequest(EEPolicy::ThreadAbortTypes abortType)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    MarkThreadForAbort(TAR_ADUnload, abortType);

    if (m_State & TS_Interruptible)
    {
        UserInterrupt(TI_Abort);
    }
}

// Background threads must be counted, because the EE should shut down when the
// last non-background thread terminates.  But we only count running ones.
void Thread::SetBackground(BOOL isBack)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    // booleanize IsBackground() which just returns bits
    if (isBack == !!IsBackground())
        return;

    LOG((LF_SYNC, INFO3, "SetBackground obtain lock\n"));
    ThreadStoreLockHolder TSLockHolder(TRUE);

    if (IsDead())
    {
        // This can only happen in a race condition, where the correct thing to do
        // is ignore it.  If it happens without the race condition, we throw an
        // exception.
    }
    else
    if (isBack)
    {
        if (!IsBackground())
        {
            FastInterlockOr((ULONG *) &m_State, TS_Background);

            // unstarted threads don't contribute to the background count
            if (!IsUnstarted())
                ThreadStore::s_pThreadStore->m_BackgroundThreadCount++;

            // If we put the main thread into a wait, until only background threads exist,
            // then we make that
            // main thread a background thread.  This cleanly handles the case where it
            // may or may not be one as it enters the wait.

            // One of the components of OtherThreadsComplete() has changed, so check whether
            // we should now exit the EE.
            ThreadStore::CheckForEEShutdown();
        }
    }
    else
    {
        if (IsBackground())
        {
            FastInterlockAnd((ULONG *) &m_State, ~TS_Background);

            // unstarted threads don't contribute to the background count
            if (!IsUnstarted())
                ThreadStore::s_pThreadStore->m_BackgroundThreadCount--;

            _ASSERTE(ThreadStore::s_pThreadStore->m_BackgroundThreadCount >= 0);
            _ASSERTE(ThreadStore::s_pThreadStore->m_BackgroundThreadCount <=
                     ThreadStore::s_pThreadStore->m_ThreadCount);
        }
    }
}

// When the thread starts running, make sure it is running in the correct apartment
// and context.
BOOL Thread::PrepareApartmentAndContext()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    m_OSThreadId = ::GetCurrentThreadId();

    return TRUE;
}




//----------------------------------------------------------------------------
//
//    ThreadStore Implementation
//
//----------------------------------------------------------------------------

ThreadStore::ThreadStore()
           : m_StoreState(TSS_Normal),
             m_Crst("ThreadStore", CrstThreadStore, (CrstFlags) (CRST_UNSAFE_ANYMODE | CRST_DEBUGGER_THREAD)),
             m_HashCrst("ThreadDLStore", CrstThreadDomainLocalStore),
             m_ThreadCount(0),
             m_MaxThreadCount(0),
             m_UnstartedThreadCount(0),
             m_BackgroundThreadCount(0),
             m_PendingThreadCount(0),
             m_DeadThreadCount(0),
             m_GuidCreated(FALSE),
             m_HoldingThread(0)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    m_TerminationEvent.CreateManualEvent(FALSE);
    _ASSERTE(m_TerminationEvent.IsValid());
}


void ThreadStore::InitThreadStore()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    s_pThreadStore = new ThreadStore;

    g_pThinLockThreadIdDispenser = new IdDispenser();

    Thread::g_pGCSuspendEvent = new CLREvent();
    Thread::g_pGCSuspendEvent->CreateManualEvent(FALSE);

#ifdef _DEBUG
    Thread::MaxThreadRecord = EEConfig::GetConfigDWORD(L"MaxThreadRecord",Thread::MaxThreadRecord);
    Thread::MaxStackDepth = EEConfig::GetConfigDWORD(L"MaxStackDepth",Thread::MaxStackDepth);
    if (Thread::MaxStackDepth > 100) {
        Thread::MaxStackDepth = 100;
    }
#endif

    s_pWaitForStackCrawlEvent = new CLREvent();
    s_pWaitForStackCrawlEvent->CreateManualEvent(FALSE);
}

extern void WaitForEndOfShutdown();

// Enter and leave the critical section around the thread store.  Clients should
// use LockThreadStore and UnlockThreadStore because ThreadStore lock has
// additional semantics well beyond a normal lock.
void ThreadStore::Enter()
{
    WRAPPER_CONTRACT;
    CHECK_ONE_STORE();
    m_Crst.Enter();

    // Threadstore needs special shutdown handling.
    if (g_fSuspendOnShutdown)
    {
        m_Crst.ReleaseAndBlockForShutdownIfNotSpecialThread();
    }
}

void ThreadStore::Leave()
{
    WRAPPER_CONTRACT;
    CHECK_ONE_STORE();
    m_Crst.Leave();
}


void ThreadStore::LockThreadStore(GCHeap::SUSPEND_REASON reason)
{
    CONTRACTL {
        NOTHROW;
        if ((GetThread() != NULL) && GetThread()->PreemptiveGCDisabled()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    // There's a nasty problem here.  Once we start shutting down because of a
    // process detach notification, threads are disappearing from under us.  There
    // are a surprising number of cases where the dying thread holds the ThreadStore
    // lock.  For example, the finalizer thread holds this during startup in about
    // 10 of our COM BVTs.
    if (!g_fProcessDetach)
    {
        BOOL gcOnTransitions;

        Thread *pCurThread = GetThread();

        gcOnTransitions = GC_ON_TRANSITIONS(FALSE);                // dont do GC for GCStress 3

        BOOL toggleGC = (   pCurThread != NULL
                         && pCurThread->PreemptiveGCDisabled()
                         && reason != GCHeap::SUSPEND_FOR_GC);

        // Note: there is logic in gc.cpp surrounding suspending all
        // runtime threads for a GC that depends on the fact that we
        // do an EnablePreemptiveGC and a DisablePreemptiveGC around
        // taking this lock.
        if (toggleGC)
            pCurThread->EnablePreemptiveGC();

        LOG((LF_SYNC, INFO3, "Locking thread store\n"));

        // Any thread that holds the thread store lock cannot be stopped by unmanaged breakpoints and exceptions when
        // we're doing managed/unmanaged debugging. Calling SetDebugCantStop(true) on the current thread helps us
        // remember that.
        if (pCurThread)
            pCurThread->SetDebugCantStop(true);

        // This is used to avoid thread starvation if non-GC threads are competing for
        // the thread store lock when there is a real GC-thread waiting to get in.
        // This is initialized lazily when the first non-GC thread backs out because of
        // a waiting GC thread.
        if (s_hAbortEvt != NULL &&
            !(reason == GCHeap::SUSPEND_FOR_GC || reason == GCHeap::SUSPEND_FOR_GC_PREP) &&
            GCHeap::GetGCHeap()->GetGCThreadAttemptingSuspend() != NULL &&
            GCHeap::GetGCHeap()->GetGCThreadAttemptingSuspend() != pCurThread)
        {
            CLREvent* hAbortEvt = s_hAbortEvt;

            if (hAbortEvt != NULL)
            {
                LOG((LF_SYNC, INFO3, "Performing suspend abort wait.\n"));
                hAbortEvt->Wait(INFINITE, FALSE);
                LOG((LF_SYNC, INFO3, "Release from suspend abort wait.\n"));
            }
        }

        // ThreadStoreLock is a critical lock used by GC, ThreadAbort, AD unload, Yield.
        // If a task is switched out while it owns ThreadStoreLock, it may not be able to
        // release it because the scheduler may be running managed code without yielding.
        Thread::BeginThreadAffinity();

        // This is shutdown aware. If we're in shutdown, and not helper/finalizer/shutdown
        // then this will not take the lock and just block forever.
        s_pThreadStore->Enter();


        _ASSERTE(s_pThreadStore->m_holderthreadid.IsUnknown());
        s_pThreadStore->m_holderthreadid.SetThreadId();

        LOG((LF_SYNC, INFO3, "Locked thread store\n"));

        // Established after we obtain the lock, so only useful for synchronous tests.
        // A thread attempting to suspend us asynchronously already holds this lock.
        s_pThreadStore->m_HoldingThread = pCurThread;

        if (toggleGC)
            pCurThread->DisablePreemptiveGC();

        GC_ON_TRANSITIONS(gcOnTransitions);
    }
#ifdef _DEBUG
    else
        LOG((LF_SYNC, INFO3, "Locking thread store skipped upon detach\n"));
#endif
}

void ThreadStore::UnlockThreadStore(BOOL bThreadDestroyed, GCHeap::SUSPEND_REASON reason)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // There's a nasty problem here.  Once we start shutting down because of a
    // process detach notification, threads are disappearing from under us.  There
    // are a surprising number of cases where the dying thread holds the ThreadStore
    // lock.  For example, the finalizer thread holds this during startup in about
    // 10 of our COM BVTs.
    if (!g_fProcessDetach)
    {
        Thread *pCurThread = GetThread();

        LOG((LF_SYNC, INFO3, "Unlocking thread store\n"));
        _ASSERTE(GetThread() == NULL || s_pThreadStore->m_HoldingThread == GetThread());

#ifdef _DEBUG
        // If Thread object has been destroyed, we need to reset the ownership info in Crst.
        _ASSERTE(!bThreadDestroyed || GetThread() == NULL);
        if (bThreadDestroyed) {
            s_pThreadStore->m_Crst.m_holderthreadid.SetThreadId();
        }
#endif

        s_pThreadStore->m_HoldingThread = NULL;
        s_pThreadStore->m_holderthreadid.ResetThreadId();
        s_pThreadStore->Leave();

        Thread::EndThreadAffinity();

        // We're out of the critical area for managed/unmanaged debugging.
        if (!bThreadDestroyed && pCurThread)
            pCurThread->SetDebugCantStop(false);
    }
#ifdef _DEBUG
    else
        LOG((LF_SYNC, INFO3, "Unlocking thread store skipped upon detach\n"));
#endif
}


void ThreadStore::LockDLSHash(StateHolderParam)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (!g_fProcessDetach)
    {
        LOG((LF_SYNC, INFO3, "Locking thread DLS hash\n"));
        GCX_PREEMP();
        s_pThreadStore->EnterDLSHashLock();
    }
#ifdef _DEBUG
    else
        LOG((LF_SYNC, INFO3, "Locking thread DLS hash skipped upon detach\n"));
#endif
}

void ThreadStore::UnlockDLSHash(StateHolderParam)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (!g_fProcessDetach)
    {
        LOG((LF_SYNC, INFO3, "Unlocking thread DLS hash\n"));
        s_pThreadStore->LeaveDLSHashLock();
    }

#ifdef _DEBUG
    else
        LOG((LF_SYNC, INFO3, "Unlocking thread DLS hash skipped upon detach\n"));
#endif
}

// AddThread adds 'newThread' to m_ThreadList
void ThreadStore::AddThread(Thread *newThread)
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    LOG((LF_SYNC, INFO3, "AddThread obtain lock\n"));

    ThreadStoreLockHolder TSLockHolder(TRUE);

    s_pThreadStore->m_ThreadList.InsertTail(newThread);

    s_pThreadStore->m_ThreadCount++;
    if (s_pThreadStore->m_MaxThreadCount < s_pThreadStore->m_ThreadCount)
        s_pThreadStore->m_MaxThreadCount = s_pThreadStore->m_ThreadCount;

    if (newThread->IsUnstarted())
        s_pThreadStore->m_UnstartedThreadCount++;

    newThread->SetThreadStateNC(Thread::TSNC_ExistInThreadStore);

    _ASSERTE(!newThread->IsBackground());
    _ASSERTE(!newThread->IsDead());
}


// Whenever one of the components of OtherThreadsComplete() has changed in the
// correct direction, see whether we can now shutdown the EE because only background
// threads are running.
void ThreadStore::CheckForEEShutdown()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (g_fWeControlLifetime &&
        s_pThreadStore->OtherThreadsComplete())
    {
        BOOL bRet;
        bRet = s_pThreadStore->m_TerminationEvent.Set();
        _ASSERTE(bRet);
    }
}


BOOL ThreadStore::RemoveThread(Thread *target)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    BOOL    found;
    Thread *ret;

    _ASSERTE(s_pThreadStore->m_Crst.GetEnterCount() > 0 ||
             g_fProcessDetach);
    _ASSERTE(s_pThreadStore->DbgFindThread(target));
    ret = s_pThreadStore->m_ThreadList.FindAndRemove(target);
    _ASSERTE(ret && ret == target);
    found = (ret != NULL);

    if (found)
    {
        target->ResetThreadStateNC(Thread::TSNC_ExistInThreadStore);

        s_pThreadStore->m_ThreadCount--;

        if (target->IsDead())
            s_pThreadStore->m_DeadThreadCount--;

        // Unstarted threads are not in the Background count:
        if (target->IsUnstarted())
            s_pThreadStore->m_UnstartedThreadCount--;
        else
        if (target->IsBackground())
            s_pThreadStore->m_BackgroundThreadCount--;


        _ASSERTE(s_pThreadStore->m_ThreadCount >= 0);
        _ASSERTE(s_pThreadStore->m_BackgroundThreadCount >= 0);
        _ASSERTE(s_pThreadStore->m_ThreadCount >=
                 s_pThreadStore->m_BackgroundThreadCount);
        _ASSERTE(s_pThreadStore->m_ThreadCount >=
                 s_pThreadStore->m_UnstartedThreadCount);
        _ASSERTE(s_pThreadStore->m_ThreadCount >=
                 s_pThreadStore->m_DeadThreadCount);

        // One of the components of OtherThreadsComplete() has changed, so check whether
        // we should now exit the EE.
        CheckForEEShutdown();
    }
    return found;
}


// When a thread is created as unstarted.  Later it may get started, in which case
// someone calls Thread::HasStarted() on that physical thread.  This completes
// the Setup and calls here.
void ThreadStore::TransferStartedThread(Thread *thread)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(GetThread() == thread);

    LOG((LF_SYNC, INFO3, "TransferUnstartedThread obtain lock\n"));
    ThreadStoreLockHolder TSLockHolder(TRUE);

    _ASSERTE(s_pThreadStore->DbgFindThread(thread));
    _ASSERTE(thread->HasValidThreadHandle());
    _ASSERTE(thread->m_State & Thread::TS_WeOwn);
    _ASSERTE(thread->IsUnstarted());
    _ASSERTE(!thread->IsDead());

    if (thread->m_State & Thread::TS_AbortRequested) {
        PAL_CPP_THROW(EEException *, new EEException(COR_E_THREADABORTED));
    }

    // Of course, m_ThreadCount is already correct since it includes started and
    // unstarted threads.

    s_pThreadStore->m_UnstartedThreadCount--;

    // We only count background threads that have been started
    if (thread->IsBackground())
        s_pThreadStore->m_BackgroundThreadCount++;

    _ASSERTE(s_pThreadStore->m_PendingThreadCount > 0);
    FastInterlockDecrement(&s_pThreadStore->m_PendingThreadCount);

    // As soon as we erase this bit, the thread becomes eligible for suspension,
    // stopping, interruption, etc.
    FastInterlockAnd((ULONG *) &thread->m_State, ~Thread::TS_Unstarted);
    FastInterlockOr((ULONG *) &thread->m_State, Thread::TS_LegalToJoin);

    // release ThreadStore Crst to avoid Crst Violation when calling HandleThreadAbort later
    TSLockHolder.Release();

    // One of the components of OtherThreadsComplete() has changed, so check whether
    // we should now exit the EE.
    CheckForEEShutdown();

}

#endif // #ifndef DACCESS_COMPILE


#if defined(USE_DBGHELP_TO_WALK_STACK_IN_DAC)
EXTERN_C void GetRuntimeStackWalkInfo(IN  ULONG64   ControlPc,
                                      OUT UINT_PTR* pModuleBase,
                                      OUT UINT_PTR* pFuncEntry);

HANDLE Thread::s_hFakeProcess = (HANDLE)(UINT_PTR)0xbaadf00d;
HANDLE Thread::s_hFakeThread  = (HANDLE)(UINT_PTR)0xdeadbeef;

HMODULE                 Thread::s_hDbgHelp         = NULL;
PDBGHELP__STACKWALK     Thread::s_pfnStackWalk     = NULL;
PDBGHELP__SYMINITIALIZE Thread::s_pfnSymInitialize = NULL;
PDBGHELP__SYMCLEANUP    Thread::s_pfnSymCleanup    = NULL;

// The dbghelp functions are not thread-safe!  They should only be used by DAC.
bool Thread::InitDbgHelp()
{
    WRAPPER_CONTRACT;

    if (s_hDbgHelp == NULL)
    {
        s_hDbgHelp = LoadDbgHelp();
        if (s_hDbgHelp == NULL)
        {
            return false;
        }

        s_pfnStackWalk     = (PDBGHELP__STACKWALK)GetProcAddress(s_hDbgHelp, "StackWalk64");
        s_pfnSymInitialize = (PDBGHELP__SYMINITIALIZE)GetProcAddress(s_hDbgHelp, "SymInitialize");
        s_pfnSymCleanup    = (PDBGHELP__SYMCLEANUP)GetProcAddress(s_hDbgHelp, "SymCleanup");

        if ( (s_pfnStackWalk     == NULL) ||
             (s_pfnSymInitialize == NULL) ||
             (s_pfnSymCleanup    == NULL) )
        {
            s_pfnStackWalk     = NULL;
            s_pfnSymInitialize = NULL;
            s_pfnSymCleanup    = NULL;

            FreeLibrary(s_hDbgHelp);
            s_hDbgHelp = NULL;
            return false;
        }
    }

    s_pfnSymInitialize(s_hFakeProcess, NULL, FALSE);
    return true;
}

void Thread::FreeDbgHelp()
{
    WRAPPER_CONTRACT;

    if (s_hDbgHelp != NULL)
    {
        _ASSERTE(s_pfnSymCleanup != NULL);
        s_pfnSymCleanup(s_hFakeProcess);
        FreeLibrary(s_hDbgHelp);
        s_hDbgHelp = NULL;
    }
}

//static
BOOL Thread::ReadMemory(HANDLE hProcess, DWORD64 lpBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead)
{
    WRAPPER_CONTRACT;

    _ASSERTE(lpNumberOfBytesRead != NULL);

    HRESULT hr = DacReadAll(lpBaseAddress, lpBuffer, nSize, false);
    if (SUCCEEDED(hr))
    {
        *lpNumberOfBytesRead = nSize;
        return TRUE;
    }
    else
    {
        *lpNumberOfBytesRead = 0;
        return FALSE;
    }
}

//static
PVOID Thread::FunctionTableAccess(HANDLE hProcess, DWORD64 AddrBase)
{
    LPVOID hFuncEntry = NULL;
    GetRuntimeStackWalkInfo(AddrBase, NULL, (UINT_PTR*)(&hFuncEntry));
    return hFuncEntry;
}

//static
DWORD64 Thread::GetModuleBase(HANDLE hProcess, DWORD64 Address)
{
    DWORD64 moduleBase = NULL;
    GetRuntimeStackWalkInfo(Address, (UINT_PTR*)&moduleBase, NULL);
    return moduleBase;
}
#endif // USE_DBGHELP_TO_WALK_STACK_IN_DAC

// Access the list of threads.  You must be inside a critical section, otherwise
// the "cursor" thread might disappear underneath you.  Pass in NULL for the
// cursor to begin at the start of the list.
Thread *ThreadStore::GetAllThreadList(Thread *cursor, ULONG mask, ULONG bits)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

#ifndef DACCESS_COMPILE
    _ASSERTE((s_pThreadStore->m_Crst.GetEnterCount() > 0) || g_fProcessDetach);
#endif

    while (TRUE)
    {
        cursor = (cursor
                  ? s_pThreadStore->m_ThreadList.GetNext(cursor)
                  : s_pThreadStore->m_ThreadList.GetHead());

        if (cursor == NULL)
            break;

        if ((cursor->m_State & mask) == bits)
            return cursor;
    }
    return NULL;
}

// Iterate over the threads that have been started
Thread *ThreadStore::GetThreadList(Thread *cursor)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return GetAllThreadList(cursor, (Thread::TS_Unstarted | Thread::TS_Dead), 0);
}

#ifndef DACCESS_COMPILE

BOOL CLREventWaitWithTry(CLREvent *pEvent, DWORD timeout, BOOL fAlertable, DWORD *pStatus)
{
    CONTRACTL
    {
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
    }
    CONTRACTL_END;

    BOOL fLoop = TRUE;
    EX_TRY
    {
        *pStatus = pEvent->Wait(timeout, fAlertable);
        fLoop = FALSE;
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

    return fLoop;
}

// We shut down the EE only when all the non-background threads have terminated
// (unless this is an exceptional termination).  So the main thread calls here to
// wait before tearing down the EE.
void ThreadStore::WaitForOtherThreads()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    CHECK_ONE_STORE();

    Thread      *pCurThread = GetThread();

    // Regardless of whether the main thread is a background thread or not, force
    // it to be one.  This simplifies our rules for counting non-background threads.
    pCurThread->SetBackground(TRUE);

    LOG((LF_SYNC, INFO3, "WaitForOtherThreads obtain lock\n"));
    ThreadStoreLockHolder TSLockHolder(TRUE);
    if (!OtherThreadsComplete())
    {
        TSLockHolder.Release();

        FastInterlockOr((ULONG *) &pCurThread->m_State, Thread::TS_ReportDead);

        DWORD ret = WAIT_OBJECT_0;
        while (CLREventWaitWithTry(&m_TerminationEvent, INFINITE, TRUE, &ret))
        {
        }
        _ASSERTE(ret == WAIT_OBJECT_0);
    }
}


// Every EE process can lazily create a GUID that uniquely identifies it (for
// purposes of remoting).
const GUID &ThreadStore::GetUniqueEEId()
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (!m_GuidCreated)
    {
        ThreadStoreLockHolder TSLockHolder(TRUE);
        if (!m_GuidCreated)
        {
            HRESULT hr = ::CoCreateGuid(&m_EEGuid);

            _ASSERTE(SUCCEEDED(hr));
            if (SUCCEEDED(hr))
                m_GuidCreated = TRUE;
        }

        if (!m_GuidCreated)
            return IID_NULL;
    }
    return m_EEGuid;
}


#ifdef _DEBUG
BOOL ThreadStore::DbgFindThread(Thread *target)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    CHECK_ONE_STORE();

    // Cache the current change stamp for g_TrapReturningThreads
    LONG chgStamp = g_trtChgStamp;
    STRESS_LOG3(LF_STORE, LL_INFO100, "ThreadStore::DbgFindThread - [thread=%p]. trt=%d. chgStamp=%d\n", GetThread(), g_TrapReturningThreads, chgStamp);

    BOOL    found = FALSE;
    Thread *cur = NULL;
    LONG    cnt = 0;
    LONG    cntBack = 0;
    LONG    cntUnstart = 0;
    LONG    cntDead = 0;
    LONG    cntReturn = 0;

    while ((cur = GetAllThreadList(cur, 0, 0)) != NULL)
    {
        cnt++;

        if (cur->IsDead())
            cntDead++;

        // Unstarted threads do not contribute to the count of background threads
        if (cur->IsUnstarted())
            cntUnstart++;
        else
        if (cur->IsBackground())
            cntBack++;

        if (cur == target)
            found = TRUE;

        // Note that (DebugSuspendPending | SuspendPending) implies a count of 2.
        // We don't count GCPending because a single trap is held for the entire
        // GC, instead of counting each interesting thread.
        if (cur->m_State & Thread::TS_DebugSuspendPending)
            cntReturn++;

        if (cur->m_State & Thread::TS_UserSuspendPending)
            cntReturn++;

        if (cur->m_TraceCallCount > 0)
            cntReturn++;

        if (cur->IsAbortRequested())
            cntReturn++;
    }

    _ASSERTE(cnt == m_ThreadCount);
    _ASSERTE(cntUnstart == m_UnstartedThreadCount);
    _ASSERTE(cntBack == m_BackgroundThreadCount);
    _ASSERTE(cntDead == m_DeadThreadCount);
    _ASSERTE(0 <= m_PendingThreadCount);


    STRESS_LOG4(LF_STORE, LL_INFO100, "ThreadStore::DbgFindThread - [thread=%p]. trt=%d. chg=%d. cnt=%d\n", GetThread(), g_TrapReturningThreads, g_trtChgStamp, cntReturn);

    // Because of race conditions and the fact that the GC places its
    // own count, I can't assert this precisely.  But I do want to be
    // sure that this count isn't wandering ever higher -- with a
    // nasty impact on the performance of GC mode changes and method
    // call chaining!
    //
    // We don't bother asserting this during process exit, because
    // during a shutdown we will quietly terminate threads that are
    // being waited on.  (If we aren't shutting down, we carefully
    // decrement our counts and alert anyone waiting for us to
    // return).
    //
    // Note: we don't actually assert this if
    // ThreadStore::TrapReturningThreads() updated g_TrapReturningThreads
    // between the beginning of this function and the moment of the assert.
    // *** The order of evaluation in the if condition is important ***
    _ASSERTE(
             (g_trtChgInFlight != 0 || (cntReturn + 2 >= g_TrapReturningThreads) || chgStamp != g_trtChgStamp) ||
             g_fEEShutDown);

    return found;
}

#endif // _DEBUG

void ThreadStore::AllocateOSContext()
{
    LEAF_CONTRACT;
    _ASSERTE(HoldingThreadStore());
    if (s_pOSContext == NULL
#ifdef _DEBUG
        || s_pOSContext == (CONTEXT*)0x1
#endif
       )
    {
        s_pOSContext = new (nothrow) CONTEXT;
    }
#ifdef _DEBUG
    if (s_pOSContext == NULL)
    {
        s_pOSContext = (CONTEXT*)0x1;
    }
#endif
}

CONTEXT *ThreadStore::GrabOSContext()
{
    LEAF_CONTRACT;
    _ASSERTE(HoldingThreadStore());
    CONTEXT *pContext = s_pOSContext;
    s_pOSContext = NULL;
#ifdef _DEBUG
    if (pContext == (CONTEXT*)0x1)
    {
        pContext = NULL;
    }
#endif
    return pContext;
}

#ifdef _DEBUG
BOOL ThreadStore::HasAllocatedContext()
{
    LEAF_CONTRACT;
    return s_pOSContext != NULL;
}
#endif

//
//

//
//
//
//


void Thread::RareDisablePreemptiveGC()
{
    DWORD dwLastError = GetLastError();
    CONTRACTL {
        NOTHROW;
        SO_TOLERANT;
        DISABLED(GC_TRIGGERS);
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(SOToleranceViolation);

    if (g_fProcessDetach)
    {
        goto lDone;
    }


#ifdef _DEBUG
    AddFiberInfo(ThreadTrackInfo_GCMode);
#endif

    // This should NEVER be called if the TSNC_UnsafeSkipEnterCooperative bit is set!
    _ASSERTE(!(m_StateNC & TSNC_UnsafeSkipEnterCooperative) && "DisablePreemptiveGC called while the TSNC_UnsafeSkipEnterCooperative bit is set");

    // Holding a spin lock in preemp mode and switch to coop mode could cause other threads spinning
    // waiting for GC
    _ASSERTE ((m_StateNC & Thread::TSNC_OwnsSpinLock) == 0);

    // If this thread is asked to yield
    if (m_State & TS_YieldRequested)
    {
        __SwitchToThread(0);
    }

    if (!GCHeap::IsGCHeapInitialized())
    {
        goto lDone;
    }

    if (((GCHeap::IsGCInProgress()  && (this != GCHeap::GetGCHeap()->GetGCThread())) ||
        (m_State & (TS_UserSuspendPending | TS_DebugSuspendPending | TS_StackCrawlNeeded))) &&
        (!g_fSuspendOnShutdown || IsFinalizerThread() || IsShutdownSpecialThread()))
    {
        if (!ThreadStore::HoldingThreadStore(this))
        {
            STRESS_LOG1(LF_SYNC, LL_INFO1000, "RareDisablePreemptiveGC: entering. Thread state = %x\n", m_State);

            do
            {
                EnablePreemptiveGC();

                // just wait until the GC is over.
                if (this != GCHeap::GetGCHeap()->GetGCThread())
                {
#ifdef PROFILING_SUPPORTED
                    // If profiler desires GC events, notify it that this thread is waiting until the GC is over
                    // Do not send suspend notifications for debugger suspensions
                    if (CORProfilerTrackSuspends() && !(m_State & TS_DebugSuspendPending))
                    {
                        // Must use wrapper to ensure forbid suspend count is incremented.
                        // (See RuntimeTheadSuspendedWrapper in threads.h for more information.)
                        RuntimeThreadSuspendedWrapper((ThreadID)this, (ThreadID)this);
                    }
#endif // PROFILING_SUPPORTED


                    // First, check to see if there's an IDbgThreadControl interface that needs
                    // notification of the suspension
                    if (m_State & TS_DebugSuspendPending)
                    {
                        IDebuggerThreadControl *pDbgThreadControl = CorHost::GetDebuggerThreadControl();

                        if (pDbgThreadControl)
                            pDbgThreadControl->ThreadIsBlockingForDebugger();

                    }

                    if (CLRGCHosted())
                    {
                        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
                        CorHost2::GetHostGCManager()->ThreadIsBlockingForSuspension();
                        END_SO_TOLERANT_CODE_CALLING_HOST;
                    }

                    // If not, check to see if there's an IGCThreadControl interface that needs
                    // notification of the suspension
                    IGCThreadControl *pGCThreadControl = CorHost::GetGCThreadControl();

                    if (pGCThreadControl)
                        pGCThreadControl->ThreadIsBlockingForSuspension();

                    DWORD status = S_OK;
                    SetThreadStateNC(TSNC_WaitUntilGCFinished);
                    status = GCHeap::GetGCHeap()->WaitUntilGCComplete();
                    ResetThreadStateNC(TSNC_WaitUntilGCFinished);

                    if (status == (DWORD)COR_E_STACKOVERFLOW)
                    {
                        // One of two things can happen here:
                        // 1. GC is suspending the process.  GC needs to wait.
                        // 2. GC is proceeding after suspension.  The current thread needs to spin.
                        SetThreadState(TS_BlockGCForSO);
                        while (GCHeap::IsGCInProgress() && m_fPreemptiveGCDisabled == 0)
                        {
#undef Sleep
                            // We can not go to a host for blocking operation due ot lack of stack.
                            // Instead we will spin here until
                            // 1. GC is finished; Or
                            // 2. GC lets this thread to run and will wait for it
                            Sleep(10);
#define Sleep(a) Dont_Use_Sleep(a)
                        }
                        ResetThreadState(TS_BlockGCForSO);
                        if (m_fPreemptiveGCDisabled == 1)
                        {
                            // GC suspension has allowed this thread to switch back to cooperative mode.
                            break;
                        }
                    }
                    if (!GCHeap::IsGCInProgress())
                    {
                        if (HasThreadState(TS_StackCrawlNeeded))
                        {
                            SetThreadStateNC(TSNC_WaitUntilGCFinished);
                            ThreadStore::WaitForStackCrawlEvent();
                            ResetThreadStateNC(TSNC_WaitUntilGCFinished);
                        }
                        else
                        {
                            __SwitchToThread(0);
                        }
                    }

#ifdef PROFILING_SUPPORTED
                    // Let the profiler know that this thread is resuming
                    if (CORProfilerTrackSuspends())
                    {
                        PROFILER_CALL;
                        g_profControlBlock.pProfInterface->RuntimeThreadResumed((ThreadID)this, (ThreadID)this);
                    }
#endif // PROFILING_SUPPORTED
                }

                FastInterlockOr(&m_fPreemptiveGCDisabled, 1);

                // The fact that we check whether 'this' is the GC thread may seem
                // strange.  After all, we determined this before entering the method.
                // However, it is possible for the current thread to become the GC
                // thread while in this loop.  This happens if you use the COM+
                // debugger to suspend this thread and then release it.

            } while ((GCHeap::IsGCInProgress()  && (this != GCHeap::GetGCHeap()->GetGCThread())) ||
                     (m_State & (TS_UserSuspendPending | TS_DebugSuspendPending | TS_StackCrawlNeeded)));
        }
        STRESS_LOG0(LF_SYNC, LL_INFO1000, "RareDisablePreemptiveGC: leaving\n");
    }

    // block all threads except finalizer and shutdown thread during shutdown.
    if (g_fSuspendOnShutdown &&
        !IsFinalizerThread() &&
        !IsShutdownSpecialThread())
    {
        STRESS_LOG1(LF_SYNC, LL_INFO1000, "RareDisablePreemptiveGC: entering. Thread state = %x\n", m_State);

        EnablePreemptiveGC();
#ifdef PROFILING_SUPPORTED
        // If profiler desires GC events, notify it that this thread is waiting until the GC is over
        // Do not send suspend notifications for debugger suspensions
        if (CORProfilerTrackSuspends() && !(m_State & TS_DebugSuspendPending))
        {
            // Must use wrapper to ensure forbid suspend count is incremented.
            // (See RuntimeTheadSuspendedWrapper in threads.h for more information.)
            RuntimeThreadSuspendedWrapper((ThreadID)this, (ThreadID)this);
        }
#endif // PROFILING_SUPPORTED


        // First, check to see if there's an IDbgThreadControl interface that needs
        // notification of the suspension
        if (m_State & TS_DebugSuspendPending)
        {
            IDebuggerThreadControl *pDbgThreadControl = CorHost::GetDebuggerThreadControl();

            if (pDbgThreadControl)
                pDbgThreadControl->ThreadIsBlockingForDebugger();

        }

        if (CLRGCHosted())
        {
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
            CorHost2::GetHostGCManager()->ThreadIsBlockingForSuspension();
            END_SO_TOLERANT_CODE_CALLING_HOST;
        }

        // If not, check to see if there's an IGCThreadControl interface that needs
        // notification of the suspension
        IGCThreadControl *pGCThreadControl = CorHost::GetGCThreadControl();

        if (pGCThreadControl)
            pGCThreadControl->ThreadIsBlockingForSuspension();

        // The thread is blocked for shutdown.  We do not concern for GC violation.
        CONTRACT_VIOLATION(GCViolation);

        WaitForEndOfShutdown();
        __SwitchToThread(INFINITE);
        _ASSERTE(!"Cannot reach here");
    }

lDone:
    SetLastError(dwLastError);
}

void Thread::HandleThreadInterrupt (BOOL fWaitForADUnload)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    BEGIN_SO_INTOLERANT_CODE(this);

    if ((m_UserInterrupt & TI_Abort) != 0)
    {
        // If the thread is waiting for AD unload to finish, and the thread is interrupted,
        // we can start aborting.
        HandleThreadAbort(fWaitForADUnload);
    }
    if ((m_UserInterrupt & TI_Interrupt) != 0)
    {
        if (ReadyForInterrupt())
        {
            ResetThreadState ((ThreadState)(TS_Interrupted | TS_Interruptible));
            FastInterlockAnd ((DWORD*)&m_UserInterrupt, ~TI_Interrupt);

#ifdef _DEBUG
            AddFiberInfo(ThreadTrackInfo_Abort);
#endif

            COMPlusThrow(kThreadInterruptedException);
        }
    }
    END_SO_INTOLERANT_CODE;
}

void Thread::HandleThreadAbortTimeout()
{
    WRAPPER_CONTRACT;

    EPolicyAction action = eNoAction;
    EClrOperation operation = OPR_ThreadRudeAbortInNonCriticalRegion;
    if (!IsRudeAbort())
    {
        if (IsFuncEvalAbort())
        {   // There can't be escalation policy for FuncEvalAbort timeout.
            return;
        }
        
        operation = OPR_ThreadAbort;
    }
    else if (HasLockInCurrentDomain())
    {
        operation = OPR_ThreadRudeAbortInCriticalRegion;
    }
    else
    {
        operation = OPR_ThreadRudeAbortInNonCriticalRegion;
    }
    action = GetEEPolicy()->GetActionOnTimeout(operation, this);
    // We only support escalation to rude abort

    EX_TRY {
        switch (action)
        {
        case eRudeAbortThread:
            GetEEPolicy()->NotifyHostOnTimeout(operation,action);
            MarkThreadForAbort(TAR_Thread, EEPolicy::TA_Rude);
            break;
        case eUnloadAppDomain:
            {
                AppDomain *pDomain = GetDomain();
                if (!pDomain->IsDefaultDomain())
                {
                    GetEEPolicy()->NotifyHostOnTimeout(operation,action);
                    pDomain->EnableADUnloadWorker(EEPolicy::ADU_Safe);
                }
            }
            break;
        case eRudeUnloadAppDomain:
            {
                AppDomain *pDomain = GetDomain();
                if (!pDomain->IsDefaultDomain())
                {
                    GetEEPolicy()->NotifyHostOnTimeout(operation,action);
                    pDomain->EnableADUnloadWorker(EEPolicy::ADU_Rude);
                }
            }
            break;
        case eExitProcess:
        case eFastExitProcess:
        case eRudeExitProcess:
        case eDisableRuntime:
            GetEEPolicy()->NotifyHostOnTimeout(operation,action);
            EEPolicy::HandleExitProcessFromEscalation(action, HOST_E_EXITPROCESS_THREADABORT);
            _ASSERTE (!"Should not reach here");
            break;
        case eNoAction:
            break;
        default:
            _ASSERTE (!"unknown policy for thread abort");
        }
    }
    EX_CATCH {
    }
    EX_END_CATCH(SwallowAllExceptions);
}

void Thread::HandleThreadAbort (BOOL fForce)
{
    DWORD dwLastError = GetLastError();

    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    BEGIN_SO_INTOLERANT_CODE(this);
    TESTHOOKCALL(AppDomainCanBeUnloaded(GetDomain()->GetId().m_dwId,FALSE));

    // It's possible we could go through here if we hit a hard SO and MC++ has called back
    // into the runtime on this thread

    FinishSOWork();
  
    if (IsAbortRequested() && GetAbortEndTime() < CLRGetTickCount64())
    {
        HandleThreadAbortTimeout();
    }



    if (fForce || ReadyForAbort())
    {
        ResetThreadState ((ThreadState)(TS_Interrupted | TS_Interruptible));
        // We are going to abort.  Abort satisfies Thread.Interrupt requirement.
        FastInterlockExchange (&m_UserInterrupt, 0);

        // generate either a ThreadAbort exception
        STRESS_LOG1(LF_APPDOMAIN, LL_INFO100, "Thread::HandleThreadAbort throwing abort for %x\n", GetThreadId());
        if (!PreemptiveGCDisabled())
        {
            DisablePreemptiveGC();
        }
        // Can not use holder.  GCX_COOP forces the thread back to the original state during
        // exception unwinding, which may put the thread back to cooperative mode.
        // GCX_COOP();

        if (!IsAbortInitiated() ||
            (IsRudeAbort() && !IsRudeAbortInitiated()))
        {
            PreWorkForThreadAbort();
        }

        PreparingAbortHolder paHolder;

        OBJECTREF exceptObj;

        if (IsRudeAbort())
        {
            exceptObj = CLRException::GetPreallocatedRudeThreadAbortException();
        }
        else
        {
            EEException eeExcept(kThreadAbortException);
            exceptObj = CLRException::GetThrowableFromException(&eeExcept);
        }

#ifdef _DEBUG
        AddFiberInfo(ThreadTrackInfo_Abort);
#endif
        RaiseTheExceptionInternalOnly(exceptObj, FALSE);
    }
    END_SO_INTOLERANT_CODE;

    SetLastError(dwLastError);
}

void Thread::PreWorkForThreadAbort()
{
    WRAPPER_CONTRACT;

    SetAbortInitiated();
    // if an abort and interrupt happen at the same time (e.g. on a sleeping thread),
    // the abort is favored. But we do need to reset the interrupt bits.
    FastInterlockAnd((ULONG *) &m_State, ~(TS_Interruptible | TS_Interrupted));
    ResetUserInterrupted();

    if (IsRudeAbort() && !(m_AbortInfo & (TAI_ADUnloadAbort |
                                          TAI_ADUnloadRudeAbort |
                                          TAI_ADUnloadV1Abort)
                          )) {
        if (HasLockInCurrentDomain()) {
            AppDomain *pDomain = GetAppDomain();
            // Cannot enable the following assertion.
            // We may take the lock, but the lock will be released during exception backout.
            //_ASSERTE(!pDomain->IsDefaultDomain());
            EPolicyAction action = GetEEPolicy()->GetDefaultAction(OPR_ThreadRudeAbortInCriticalRegion, this);
            switch (action)
            {
            case eRudeUnloadAppDomain:
                if (!pDomain->IsDefaultDomain())
                {
                    GetEEPolicy()->NotifyHostOnDefaultAction(OPR_ThreadRudeAbortInCriticalRegion,action);
                    pDomain->EnableADUnloadWorker(EEPolicy::ADU_Rude);
                }
                break;
            case eExitProcess:
            case eFastExitProcess:
            case eRudeExitProcess:
            case eDisableRuntime:
                GetEEPolicy()->NotifyHostOnDefaultAction(OPR_ThreadRudeAbortInCriticalRegion,action);
                GetEEPolicy()->HandleExitProcessFromEscalation(action,HOST_E_EXITPROCESS_ADUNLOAD);
                break;
            default:
                break;
            }
        }
    }
}

#ifdef _DEBUG
#define MAXSTACKBYTES (2 * PAGE_SIZE)
void CleanStackForFastGCStress ()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    size_t nBytes = MAXSTACKBYTES;
    nBytes &= ~sizeof (size_t);
    if (nBytes > MAXSTACKBYTES) {
        nBytes = MAXSTACKBYTES;
    }
    size_t* buffer = (size_t*) _alloca (nBytes);
    memset(buffer, 0, nBytes);
    GetThread()->m_pCleanedStackBase = &nBytes;
}

void Thread::ObjectRefFlush(Thread* thread)
{

    DWORD dwLastError = GetLastError();

    // The constructor and destructor of AutoCleanupSONotMainlineHolder (allocated by SO_NOT_MAINLINE_FUNCTION below)
    // may trash the last error, so we need to save and restore last error here.  Also, we need to add a scope here
    // because we can't let the destructor run after we call SetLastError().
    {
        // this is debug only code, so no need to validate
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_NOTRIGGER;
        STATIC_CONTRACT_ENTRY_POINT;

        _ASSERTE(thread->PreemptiveGCDisabled());  // Should have been in managed code
        memset(thread->dangerousObjRefs, 0, sizeof(thread->dangerousObjRefs));
        CLEANSTACKFORFASTGCSTRESS ();
    }

    SetLastError(dwLastError);

}
#endif

#if defined(STRESS_HEAP)

PtrHashMap *g_pUniqueStackMap = NULL;
Crst *g_pUniqueStackCrst = NULL;

#define UniqueStackDepth 8

BOOL StackCompare (UPTR val1, UPTR val2)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    size_t *p1 = (size_t *)(val1 << 1);
    size_t *p2 = (size_t *)val2;
    if (p1[0] != p2[0]) {
        return FALSE;
    }
    size_t nElem = p1[0];
    if (nElem >= UniqueStackDepth) {
        nElem = UniqueStackDepth;
    }
    p1 ++;
    p2 ++;

    for (size_t n = 0; n < nElem; n ++) {
        if (p1[n] != p2[n]) {
            return FALSE;
        }
    }

    return TRUE;
}

void UniqueStackSetupMap()
{
    WRAPPER_CONTRACT;

    if (g_pUniqueStackCrst == NULL)
    {
        Crst *Attempt = new Crst ("HashMap",
                                     CrstUniqueStack,
                                     CrstFlags(CRST_REENTRANCY | CRST_UNSAFE_ANYMODE));

        if (FastInterlockCompareExchangePointer((void **) &g_pUniqueStackCrst,
                                                Attempt,
                                                NULL) != NULL)
        {
            // We lost the race
            delete Attempt;
        }
    }

    // Now we have a Crst we can use to synchronize the remainder of the init.
    if (g_pUniqueStackMap == NULL)
    {
        CrstHolder ch(g_pUniqueStackCrst);

        if (g_pUniqueStackMap == NULL)
        {
            PtrHashMap *map = new (SystemDomain::System()->GetLowFrequencyHeap()) PtrHashMap ();
            LockOwner lock = {g_pUniqueStackCrst, IsOwnerOfCrst};
            map->Init (256, StackCompare, TRUE, &lock);
            g_pUniqueStackMap = map;
        }
    }
}

BOOL StartUniqueStackMapHelper()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    BOOL fOK = TRUE;
    EX_TRY
    {
        if (g_pUniqueStackMap == NULL)
        {
            UniqueStackSetupMap();
        }
    }
    EX_CATCH
    {
        fOK = FALSE;
    }
    EX_END_CATCH(SwallowAllExceptions);

    return fOK;
}

BOOL StartUniqueStackMap ()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return StartUniqueStackMapHelper();
}


#if defined(_DEBUG)

// This function is for GC stress testing.  Before we enable preemptive GC, let us do a GC
// because GC may happen while the thread is in preemptive GC mode.
void Thread::PerformPreemptiveGC()
{
    CONTRACTL {
        NOTHROW;
        DISABLED(GC_TRIGGERS);
        DEBUG_ONLY;
    }
    CONTRACTL_END;

    if (g_fProcessDetach)
        return;

    if (!(g_pConfig->GetGCStressLevel() & EEConfig::GCSTRESS_TRANSITION))
        return;

    if (!GCHeap::IsGCHeapInitialized())
        return;

    if (!m_GCOnTransitionsOK
        || g_fEEShutDown
        || GCHeap::IsGCInProgress(TRUE)
        || GCHeap::GetGCHeap()->GetGcCount() == 0    // Need something that works for isolated heap.
        || ThreadStore::HoldingThreadStore())
        return;

    if (Thread::ThreadsAtUnsafePlaces())
        return;

#ifdef DEBUGGING_SUPPORTED
    // Don't collect if the debugger is attach and either 1) there
    // are any threads held at unsafe places or 2) this thread is
    // under the control of the debugger's dispatch logic (as
    // evidenced by having a non-NULL filter context.)
    if ((CORDebuggerAttached() &&
        (g_pDebugInterface->ThreadsAtUnsafePlaces() ||
        (GetFilterContext() != NULL))))
        return;
#endif // DEBUGGING_SUPPORTED

    _ASSERTE(m_fPreemptiveGCDisabled == false);     // we are in preemptive mode when we call this

    m_GCOnTransitionsOK = FALSE;
    DisablePreemptiveGC();
    m_bGCStressing = TRUE;
    GCHeap::GetGCHeap()->StressHeap();
    m_bGCStressing = FALSE;
    EnablePreemptiveGC();
    m_GCOnTransitionsOK = TRUE;
}
#endif  // DEBUG
#endif // STRESS_HEAP

// To leave cooperative mode and enter preemptive mode, if a GC is in progress, we
// no longer care to suspend this thread.  But if we are trying to suspend the thread
// for other reasons (e.g. Thread.Suspend()), now is a good time.
//
// Note that it is possible for an N/Direct call to leave the EE without explicitly
// enabling preemptive GC.
void Thread::RareEnablePreemptiveGC()
{
    CONTRACTL {
        NOTHROW;
        DISABLED(GC_TRIGGERS);
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(GCViolation|FaultViolation|SOToleranceViolation);

    // If we have already received our PROCESS_DETACH during shutdown, there is only one thread in the
    // process and no coordination is necessary.
    if (g_fProcessDetach)
        return;

#ifdef _DEBUG
    AddFiberInfo(ThreadTrackInfo_GCMode);
#endif

    // EnablePreemptiveGC already set us to preemptive mode before triggering the Rare path.
    // Force other threads to see this update, since the Rare path implies that someone else
    // is observing us (e.g. SysSuspendForGC).

    _ASSERTE (!m_fPreemptiveGCDisabled);

    // holding a spin lock in coop mode and transit to preemp mode will cause deadlock on GC
    _ASSERTE ((m_StateNC & Thread::TSNC_OwnsSpinLock) == 0);

    FastInterlockOr (&m_fPreemptiveGCDisabled, 0);

#if defined(STRESS_HEAP) && defined(_DEBUG)
    if (!IsDetached())
        PerformPreemptiveGC();
#endif

    STRESS_LOG1(LF_SYNC, LL_INFO1000, "RareEnablePreemptiveGC: entering. Thread state = %x\n", m_State);
    if (!ThreadStore::HoldingThreadStore(this))
    {

        // wake up any threads waiting to suspend us, like the GC thread.
        SetSafeEvent();
        g_pGCSuspendEvent->Set();

        // for GC, the fact that we are leaving the EE means that it no longer needs to
        // suspend us.  But if we are doing a non-GC suspend, we need to block now.
        // Give the debugger precedence over user suspensions:
        while (m_State & (TS_DebugSuspendPending | TS_UserSuspendPending))
        {
#ifdef DEBUGGING_SUPPORTED
            // We don't notify the debugger that this thread is now suspended. We'll just
            // let the debugger's helper thread sweep and pick it up.
            // We also never take the TSL in here either.
            // Life's much simpler this way...


            // Check to see if there's an IDbgThreadControl interface that needs
            // notification of the suspension
            if (m_State & TS_DebugSuspendPending)
            {
                IDebuggerThreadControl *pDbgThreadControl = CorHost::GetDebuggerThreadControl();

                if (pDbgThreadControl)
                    pDbgThreadControl->ThreadIsBlockingForDebugger();

            }
#endif // DEBUGGING_SUPPORTED

#ifdef LOGGING
            if (!CorHost::IsDebuggerSpecialThread(m_ThreadId))
                LOG((LF_CORDB, LL_INFO1000, "[0x%x] SUSPEND: suspended while enabling gc.\n", m_ThreadId));

            else
                LOG((LF_CORDB, LL_INFO1000,
                     "[0x%x] ALERT: debugger special thread did not suspend while enabling gc.\n", m_ThreadId));
#endif

            WaitSuspendEvents(); // sets bits, too

        }
    }
    STRESS_LOG0(LF_SYNC, LL_INFO1000, " RareEnablePreemptiveGC: leaving.\n");
}


// Called out of CommonTripThread, we are passing through a Safe spot.  Do the right
// thing with this thread.  This may involve waiting for the GC to complete, or
// performing a pending suspension.
void Thread::PulseGCMode()
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(this == GetThread());

    if (PreemptiveGCDisabled() && CatchAtSafePoint())
    {
        EnablePreemptiveGC();
        DisablePreemptiveGC();
    }
}

// Indicate whether threads should be trapped when returning to the EE (i.e. disabling
// preemptive GC mode)
void ThreadStore::TrapReturningThreads(BOOL yes)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (yes)
    {
#ifdef _DEBUG
        CounterHolder trtHolder(&g_trtChgInFlight);
        FastInterlockIncrement(&g_trtChgStamp);
#endif

        FastInterlockIncrement(&g_TrapReturningThreads);
        _ASSERTE(g_TrapReturningThreads > 0);

#ifdef _DEBUG
        trtHolder.Release();
#endif
    }
    else
    {
        FastInterlockDecrement(&g_TrapReturningThreads);
        _ASSERTE(g_TrapReturningThreads >= 0);
    }
}


// Grab a consistent snapshot of the thread's state, for reporting purposes only.
Thread::ThreadState Thread::GetSnapshotState()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    ThreadState     res = m_State;

    if (res & TS_ReportDead)
        res = (ThreadState) (res | TS_Dead);

    return res;
}





//************************************************************************************
// The basic idea is to make a first pass while the threads are suspended at the OS
// level.  This pass marks each thread to indicate that it is requested to get to a
// safe spot.  Then the threads are resumed.  In a second pass, we actually wait for
// the threads to get to their safe spot and rendezvous with us.
HRESULT Thread::SysSuspendForGC(GCHeap::SUSPEND_REASON reason)
{
    CONTRACTL {
        NOTHROW;
        if (GetThread())
        {
            GC_TRIGGERS;            // CLREvent::Wait is GC_TRIGGERS
        }
        else
        {
            DISABLED(GC_TRIGGERS);
        }
    }
    CONTRACTL_END;

    Thread  *pCurThread = GetThread();
    Thread  *thread = NULL;
    LONG     countThreads = 0;
    DWORD    res;

    // Caller is expected to be holding the ThreadStore lock.  Also, caller must
    // have set GcInProgress before coming here, or things will break;
    _ASSERTE(ThreadStore::HoldingThreadStore() || g_fProcessDetach);
    _ASSERTE(GCHeap::IsGCInProgress() );

    STRESS_LOG1(LF_SYNC, LL_INFO1000, "Thread::SysSuspendForGC(reason=0x%x)\n", reason);

    // Alert the host that a GC is starting, in case the host is scheduling threads
    // for non-runtime tasks during GC.
    IGCThreadControl    *pGCThreadControl = CorHost::GetGCThreadControl();

    if (pGCThreadControl)
        pGCThreadControl->SuspensionStarting();

    if (CLRGCHosted())
    {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        CorHost2::GetHostGCManager()->SuspensionStarting();
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }

#ifdef PROFILING_SUPPORTED
    // If the profiler desires information about GCs, then let it know that one
    // is starting.
    if (CORProfilerTrackSuspends())
    {
        _ASSERTE(reason != GCHeap::SUSPEND_FOR_DEBUGGER);

        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->RuntimeSuspendStarted(
                (COR_PRF_SUSPEND_REASON)reason,
                (ThreadID)pCurThread);

        }
        if (pCurThread)
        {
            // Notify the profiler that the thread that is actually doing the GC is 'suspended',
            // meaning that it is doing stuff other than run the managed code it was before the
            // GC started.
            //
            // Must use wrapper to ensure forbid suspend count is incremented.
            // (See RuntimeTheadSuspendedWrapper in threads.h for more information.)
            RuntimeThreadSuspendedWrapper((ThreadID)pCurThread, (ThreadID)pCurThread);
        }
    }
#endif // PROFILING_SUPPORTED

    if (pCurThread)     // concurrent GC occurs on threads we don't know about
    {
        _ASSERTE(pCurThread->m_Priority == INVALID_THREAD_PRIORITY);
        int priority = pCurThread->GetThreadPriority();
        if (priority < THREAD_PRIORITY_NORMAL)
        {
            pCurThread->m_Priority = priority;
            pCurThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
        }
    }

    // If CLR is hosted with IHostTaskManager and escalation policy for StackOverflow,
    // we need to make sure a thread is never blocked with a small stack, because the thread can 
    // not be moved from scheduler in the host, and the scheduler may hold some resource needed by
    // suspension thread.

    // If we need to handle SO, GC will wait until a target thread has finished LeaveRuntime call.  At
    // this point, the thread is off scheduler.  If it hits SO, we will kill the process.  If the thread hits
    // SO while calling LeaveRuntime, we treat this as SO in managed code, and unload the domain instead.
    BOOL fConsiderSO = (CLRTaskHosted() && 
                        GetEEPolicy()->GetActionOnFailure(FAIL_StackOverflow) == eRudeUnloadAppDomain);

    SysSuspendInProgressHolder hldSysSuspendInProgress;



    CpuStoreBufferControl::FlushStoreBuffers();
    
    while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
    {
        if (thread->HasThreadState(TS_GCSuspendPending))
        {
            thread->ResetThreadState(TS_GCSuspendPending);
        }

        if (thread == pCurThread)
            continue;

        STRESS_LOG3(LF_SYNC, LL_INFO10000, "    Inspecting thread 0x%x ID 0x%x coop mode = %d\n",
            thread, thread->GetThreadId(), thread->m_fPreemptiveGCDisabled);

        // Nothing confusing left over from last time.
        _ASSERTE((thread->m_State & TS_GCSuspendPending) == 0);

        // Threads can be in Preemptive or Cooperative GC mode.  Threads cannot switch
        // to Cooperative mode without special treatment when a GC is happening.
        if (thread->m_fPreemptiveGCDisabled)
        {
            // Check a little more carefully.  Threads might sneak out without telling
            // us, because of inlined PInvoke which doesn't go through RareEnablePreemptiveGC.


            // We can not allocate memory after we suspend a thread.
            // Otherwise, we may deadlock the process when CLR is hosted.
            ThreadStore::AllocateOSContext();

#ifdef TIME_SUSPEND
            DWORD startSuspend = g_SuspendStatistics.GetTime();
#endif

            SuspendThreadResult str = thread->SuspendThread();

#ifdef TIME_SUSPEND
            g_SuspendStatistics.osSuspend.Accumulate(
                    SuspendStatistics::GetElapsed(startSuspend,
                                                  g_SuspendStatistics.GetTime()));

            if (str == STR_Success)
                g_SuspendStatistics.cntOSSuspendResume++;
            else
                g_SuspendStatistics.cntFailedSuspends++;
#endif

            if (str == STR_NoStressLog)
            {
                STRESS_LOG2(LF_SYNC, LL_ERROR, "    ERROR: Could not suspend thread 0x%x, result = %d\n", thread, str);
            }
            else
            if (thread->m_fPreemptiveGCDisabled)
            {

                FastInterlockOr((ULONG *) &thread->m_State, TS_GCSuspendPending);

                countThreads++;

                // Only resume if we actually suspended the thread above.
                if (str == STR_Success)
                    thread->ResumeThread();

                STRESS_LOG1(LF_SYNC, LL_INFO1000, "    Thread 0x%x is in cooperative needs to rendezvous\n", thread);
            }
            else
            if (str == STR_Success)
            {
                STRESS_LOG1(LF_SYNC, LL_WARNING, "    Inspecting thread 0x%x was in cooperative, but now is not\n", thread);
                // Oops.
                thread->ResumeThread();
            }
            else
            if (str == STR_SwitchedOut) {
                STRESS_LOG1(LF_SYNC, LL_WARNING, "    Inspecting thread 0x%x was in cooperative, but now is switched out\n", thread);
            }
            else {
                _ASSERTE(str == STR_Failure || str == STR_UnstartedOrDead);
                STRESS_LOG3(LF_SYNC, LL_ERROR, "    ERROR: Could not suspend thread 0x%x, result = %d, lastError = 0x%x\n", thread, str, GetLastError());
            }
        }
        if (thread->m_fPreemptiveGCDisabled == 0 && fConsiderSO)
        {
        }
    }

#ifdef _DEBUG

    {
        int     countCheck = 0;
        Thread *InnerThread = NULL;

        while ((InnerThread = ThreadStore::GetThreadList(InnerThread)) != NULL)
        {
            if (InnerThread != pCurThread &&
                (InnerThread->m_State & TS_GCSuspendPending) != 0)
            {
                countCheck++;
            }
        }
        _ASSERTE(countCheck == countThreads);
    }

#endif

    // Pass 2: Whip through the list again.

#if PING_AND_YIELD
    int yieldToLowPriority = 0;
#endif

    while (countThreads)
    {
        _ASSERTE (thread == NULL);
        STRESS_LOG1(LF_SYNC, LL_INFO1000, "    A total of %d threads need to rendezvous\n", countThreads);
        while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
        {
            if (thread == pCurThread)
                continue;

            if (thread->HasThreadState(TS_BlockGCForSO))
            {
                // The thread is trying to block for GC.  But we don't have enough stack to do
                // this operation.
                // We will let the thread switch back to cooperative mode, and continue running.
                if (thread->m_fPreemptiveGCDisabled == 0)
                {
                    if (!thread->HasThreadState(TS_GCSuspendPending))
                    {
                        thread->SetThreadState(TS_GCSuspendPending);
                        countThreads ++;
                    }
                    thread->ResetThreadState(TS_BlockGCForSO);
                    FastInterlockOr (&thread->m_fPreemptiveGCDisabled, 1);
                }
                continue;
            }
            if ((thread->m_State & TS_GCSuspendPending) == 0)
                continue;

            if (!thread->m_fPreemptiveGCDisabled)
            {
                {
                    STRESS_LOG1(LF_SYNC, LL_INFO1000, "    Thread %x went preemptive it is at a GC safe point\n", thread);
                    countThreads--;
                    thread->ResetThreadState(TS_GCSuspendPending);
                }
            }
        }

        if (countThreads == 0)
        {
            break;
        }

#ifdef _DEBUG
        DWORD dbgStartTimeout = GetTickCount();
#endif

        // If another thread is trying to do a GC, there is a chance of deadlock
        // because this thread holds the threadstore lock and the GC thread is stuck
        // trying to get it, so this thread must bail and do a retry after the GC completes.
        if (GCHeap::GetGCHeap()->GetGCThreadAttemptingSuspend() != NULL && GCHeap::GetGCHeap()->GetGCThreadAttemptingSuspend() != pCurThread)
        {
#ifdef PROFILING_SUPPORTED
            // Must let the profiler know that this thread is aborting its attempt at suspending
            if (CORProfilerTrackSuspends())
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->RuntimeSuspendAborted((ThreadID)pCurThread);
            }
#endif // PROFILING_SUPPORTED

            STRESS_LOG0(LF_SYNC, LL_ALWAYS, "Thread::SysSuspendForGC() - Timing out.\n");
            return (ERROR_TIMEOUT);
        }

#ifdef TIME_SUSPEND
        DWORD startWait = g_SuspendStatistics.GetTime();
#endif

#if PING_AND_YIELD
        // Don't yield unless we are on a single proc, or we want to open windows for low
        // priority threads to advance to their suspension points.

        if ((g_SystemInfo.dwNumberOfProcessors == 1) ||
            (++yieldToLowPriority % 5) == 0)
        {
            __SwitchToThread(5);
        }

        CONTRACT_VIOLATION(ThrowsViolation);
        res = g_pGCSuspendEvent->Wait(0, FALSE);

#else

        CONTRACT_VIOLATION(ThrowsViolation);
        res = g_pGCSuspendEvent->Wait(PING_JIT_TIMEOUT, FALSE);

#endif

#ifdef TIME_SUSPEND
        g_SuspendStatistics.wait.Accumulate(
                SuspendStatistics::GetElapsed(startWait,
                                              g_SuspendStatistics.GetTime()));

        g_SuspendStatistics.cntWaits++;
        if (res == WAIT_TIMEOUT)
            g_SuspendStatistics.cntWaitTimeouts++;
#endif

        if (res == WAIT_TIMEOUT || res == WAIT_IO_COMPLETION)
        {
            STRESS_LOG1(LF_SYNC, LL_INFO1000, "    Timed out waiting for rendezvous event %d threads remaining\n", countThreads);
#ifdef _DEBUG
            DWORD dbgEndTimeout = GetTickCount();

            if ((dbgEndTimeout > dbgStartTimeout) &&
                (dbgEndTimeout - dbgStartTimeout > g_pConfig->SuspendDeadlockTimeout()))
            {
                // Do not change this to _ASSERTE.
                // We want to catch the state of the machine at the
                // time when we can not suspend some threads.
                // It takes too long for _ASSERTE to stop the process.
                DebugBreak();
                _ASSERTE(!"Timed out trying to suspend EE due to thread");
                char message[256];
                _ASSERTE (thread == NULL);
                while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
                {
                    if (thread == pCurThread)
                        continue;

                    if ((thread->m_State & TS_GCSuspendPending) == 0)
                        continue;

                    if (thread->m_fPreemptiveGCDisabled)
                    {
                        DWORD id = thread->m_OSThreadId;
                        if (id == 0xbaadf00d)
                        {
                            sprintf_s (message, COUNTOF(message), "Thread CLR ID=%x cannot be suspended",
                                     thread->GetThreadId());
                        }
                        else
                        {
                            sprintf_s (message, COUNTOF(message), "Thread OS ID=%x cannot be suspended",
                                     id);
                        }
                        DbgAssertDialog(__FILE__, __LINE__, message);
                    }
                }
            }
#endif
            // all these threads should be in cooperative mode unless they have
            // set their SafeEvent on the way out.  But there's a race between
            // when we time out and when they toggle their mode, so sometimes
            // we will suspend a thread that has just left.
            _ASSERTE (thread == NULL);
            while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
            {
                if (thread == pCurThread)
                    continue;

                if ((thread->m_State & TS_GCSuspendPending) == 0)
                    continue;

                if (!thread->m_fPreemptiveGCDisabled)
                {
                    continue;
                }

                // We can not allocate memory after we suspend a thread.
                // Otherwise, we may deadlock the process when CLR is hosted.
                ThreadStore::AllocateOSContext();

#ifdef TIME_SUSPEND
                DWORD startSuspend = g_SuspendStatistics.GetTime();
#endif

                        SuspendThreadResult str = thread->SuspendThread();

#ifdef TIME_SUSPEND
                g_SuspendStatistics.osSuspend.Accumulate(
                    SuspendStatistics::GetElapsed(startSuspend,
                                                  g_SuspendStatistics.GetTime()));

                if (str == STR_Success)
                    g_SuspendStatistics.cntOSSuspendResume++;
                else
                    g_SuspendStatistics.cntFailedSuspends++;
#endif


                // Whether in cooperative mode & stubborn, or now in
                // preemptive mode because of inlined N/Direct, let this
                // thread go.
                if (str == STR_Success)
                    thread->ResumeThread();
            }
        }
        else
        if (res == WAIT_OBJECT_0)
        {
            g_pGCSuspendEvent->Reset();
            continue;
        }
        else
        {
            // No WAIT_FAILED, WAIT_ABANDONED, etc.
            _ASSERTE(!"unexpected wait termination during gc suspension");
        }
    }

#ifdef PROFILING_SUPPORTED
    // If a profiler is keeping track of GC events, notify it
    if (CORProfilerTrackSuspends())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RuntimeSuspendFinished((ThreadID)pCurThread);
    }
#endif // PROFILING_SUPPORTED

#ifdef _DEBUG
    if (reason == GCHeap::SUSPEND_FOR_GC) {
        thread = NULL;
        while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
        {
            thread->DisableStressHeap();
            _ASSERTE (!thread->HasThreadState(TS_GCSuspendPending));
        }
    }
#endif

    g_pGCSuspendEvent->Reset();

#if defined(STRESS_HEAP) && defined(_DEBUG)
    //
    // Now that the EE has been suspended, let's see if any oustanding
    // gcstress instruction updates need to occur.  Each thread can
    // have only one pending at a time.
    //
    thread = NULL;
    while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
    {
        thread->CommitGCStressInstructionUpdate();
    }
#endif // defined(STRESS_HEAP) && defined(_DEBUG)

    STRESS_LOG0(LF_SYNC, LL_INFO1000, "Thread::SysSuspendForGC() - Success\n");
    return S_OK;
}

#if defined(STRESS_HEAP) && defined(_DEBUG)

void Thread::CommitGCStressInstructionUpdate()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (HasPendingGCStressInstructionUpdate())
    {
#if defined(_X86_) || defined(_AMD64_)

        *m_pbDestCode = *m_pbSrcCode;

#else // defined(_X86_) || defined(_AMD64_)

        *m_pbDestCode = *m_pbSrcCode;

#endif // defined(_X86_) || defined(_AMD64_)

        ClearGCStressInstructionUpdate();
    }
}

#endif // defined(STRESS_HEAP) && defined(_DEBUG)


#ifdef _DEBUG
void EnableStressHeapHelper()
{
    WRAPPER_CONTRACT;
    ENABLESTRESSHEAP();
}
#endif

// We're done with our GC.  Let all the threads run again
void Thread::SysResumeFromGC(BOOL bFinishedGC, BOOL SuspendSucceded)
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    Thread  *pCurThread = GetThread();

    // Caller is expected to be holding the ThreadStore lock.  But they must have
    // reset GcInProgress, or threads will continue to suspend themselves and won't
    // be resumed until the next GC.
    _ASSERTE(ThreadStore::HoldingThreadStore());
    _ASSERTE(!GCHeap::IsGCInProgress() );

    STRESS_LOG2(LF_SYNC, LL_INFO1000, "Thread::SysResumeFromGC(finishedGC=%d, SuspendSucceeded=%d) - Start\n", bFinishedGC, SuspendSucceded);

    // Alert the host that a GC is ending, in case the host is scheduling threads
    // for non-runtime tasks during GC.
    IGCThreadControl    *pGCThreadControl = CorHost::GetGCThreadControl();

    if (pGCThreadControl)
    {
        // If we the suspension was for a GC, tell the host what generation GC.
        DWORD   Generation = (bFinishedGC
                              ? GCHeap::GetGCHeap()->GetCondemnedGeneration()
                              : ~0U);

        pGCThreadControl->SuspensionEnding(Generation);
    }

    if (CLRGCHosted())
    {
        // If we the suspension was for a GC, tell the host what generation GC.
        DWORD   Generation = (bFinishedGC
                              ? GCHeap::GetGCHeap()->GetCondemnedGeneration()
                              : ~0U);

        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        CorHost2::GetHostGCManager()->SuspensionEnding(Generation);
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }

#ifdef PROFILING_SUPPORTED
    // Need to give resume event for the GC thread
    if (CORProfilerTrackSuspends())
    {
        if (pCurThread)
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->RuntimeThreadResumed(
                (ThreadID)pCurThread, (ThreadID)pCurThread);
        }

        // If a profiler is keeping track suspend events, notify it
        if (CORProfilerTrackSuspends())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->RuntimeResumeFinished((ThreadID)pCurThread);
        }
    }
#endif // PROFILING_SUPPORTED

#ifdef TIME_SUSPEND
    DWORD startRelease = g_SuspendStatistics.GetTime();
#endif

    ThreadStore::UnlockThreadStore();

#ifdef TIME_SUSPEND
    g_SuspendStatistics.releaseTSL.Accumulate(SuspendStatistics::GetElapsed(startRelease,
                                                                            g_SuspendStatistics.GetTime()));
#endif

    if (pCurThread)
    {
        if (pCurThread->m_Priority != INVALID_THREAD_PRIORITY)
        {
            pCurThread->SetThreadPriority(pCurThread->m_Priority);
            pCurThread->m_Priority = INVALID_THREAD_PRIORITY;
        }
    }

    STRESS_LOG0(LF_SYNC, LL_INFO1000, "Thread::SysResumeFromGC() - End\n");
}



// Resume a thread at this location, to persuade it to throw a ThreadStop.  The
// exception handler needs a reasonable idea of how large this method is, so don't
// add lots of arbitrary code here.
void
ThrowControlForThread(
        )
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;

    Thread *pThread = GetThread();
    _ASSERTE(pThread);
    _ASSERTE(pThread->m_OSContext);

    _ASSERTE(pThread->PreemptiveGCDisabled());

    if (GetEEPolicy()->GetActionOnFailure(FAIL_StackOverflow) == eRudeUnloadAppDomain)
    {
#ifndef TOTALLY_DISBLE_STACK_GUARDS
        _ASSERTE (pThread->IsSOTolerant());
        RetailStackProbe(ADJUST_PROBE(DEFAULT_ENTRY_PROBE_AMOUNT), pThread);
#endif
    }

    // Check if we can start abort
    // We use InducedThreadRedirect as a marker to tell stackwalker that a thread is redirected from JIT code.
    // This is to distinguish a thread is in Preemptive mode and in JIT code.
    // After stackcrawl, we change to InducedThreadStop.
    if (pThread->ThrewControlForThread() == Thread::InducedThreadRedirect)
    {
    }

    FrameWithCookie<FaultingExceptionFrame> fef;
    FaultingExceptionFrame *pfef = &fef;
    pfef->InitAndLink(pThread->m_OSContext);

    // !!! Can not assert here.  Sometimes our EHInfo for catch clause extends beyond
    // !!! Jit_EndCatch.  Not sure if we have guarantee on catch clause.
    //_ASSERTE (pThread->ReadyForAbort());

#ifdef _X86_
#elif defined(_PPC_)
    CalleeSavedRegisters *pRegs = pfef->GetCalleeSavedRegisters();
    ZeroMemory(&(pRegs->r[0]), sizeof(INT32)*NUM_CALLEESAVED_REGISTERS);
#else
    PORTABILITY_ASSERT("ThrowControlForThread");
#endif

    STRESS_LOG0(LF_SYNC, LL_INFO100, "ThrowControlForThread Aborting\n");

    PAL_CPP_EHUNWIND_BEGIN;
    // Here we raise an exception.
    RaiseException(EXCEPTION_COMPLUS,
                    0,
                    0,
                    NULL);
    PAL_CPP_EHUNWIND_END;
}



PCONTEXT Thread::GetAbortContext ()
{
    LEAF_CONTRACT;
    return m_OSContext;
}


//****************************************************************************
// Return true if we've Suspended the runtime,
// False if we still need to sweep.
//****************************************************************************
bool Thread::SysStartSuspendForDebug(AppDomain *pAppDomain)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    Thread  *pCurThread = GetThread();
    Thread  *thread = NULL;

    if (g_fProcessDetach)
    {
        LOG((LF_CORDB, LL_INFO1000,
             "SUSPEND: skipping suspend due to process detach.\n"));
        return true;
    }

    LOG((LF_CORDB, LL_INFO1000, "[0x%x] SUSPEND: starting suspend.  Trap count: %d\n",
         pCurThread ? pCurThread->m_ThreadId : (DWORD) -1, g_TrapReturningThreads));

    // Caller is expected to be holding the ThreadStore lock
    _ASSERTE(ThreadStore::HoldingThreadStore() || g_fProcessDetach);

    // If there is a debugging thread control object, tell it we're suspending the Runtime.
    IDebuggerThreadControl *pDbgThreadControl = CorHost::GetDebuggerThreadControl();

    if (pDbgThreadControl)
        pDbgThreadControl->StartBlockingForDebugger(0);

    // NOTE::NOTE::NOTE::NOTE::NOTE
    // This function has parallel logic in SysSuspendForGC.  Please make
    // sure to make appropriate changes there as well.

    _ASSERTE(m_DebugWillSyncCount == -1);
    m_DebugWillSyncCount++;

    SysSuspendInProgressHolder hldSysSuspendInProgress;

    while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
    {

        // Don't try to suspend threads that you've left suspended.
        if (thread->m_StateNC & TSNC_DebuggerUserSuspend)
            continue;

        if (thread == pCurThread)
        {
            LOG((LF_CORDB, LL_INFO1000,
                 "[0x%x] SUSPEND: marking current thread.\n",
                 thread->m_ThreadId));

            _ASSERTE(!thread->m_fPreemptiveGCDisabled);

            // Mark this thread so it trips when it tries to re-enter
            // after completing this call.
            thread->SetupForSuspension(TS_DebugSuspendPending);
            thread->MarkForSuspension(TS_DebugSuspendPending);
            continue;
        }

        thread->SetupForSuspension(TS_DebugSuspendPending);

        // Threads can be in Preemptive or Cooperative GC mode.
        // Threads cannot switch to Cooperative mode without special
        // treatment when a GC is happening.  But they can certainly
        // switch back and forth during a debug suspension -- until we
        // can get their Pending bit set.

        // We can not allocate memory after we suspend a thread.
        // Otherwise, we may deadlock the process when CLR is hosted.
        ThreadStore::AllocateOSContext();

        SuspendThreadResult str = thread->SuspendThread();

        if (thread->m_fPreemptiveGCDisabled && str == STR_Success)
        {


            // Remember that this thread will be running to a safe point
            FastInterlockIncrement(&m_DebugWillSyncCount);

            // When the thread reaches a safe place, it will wait
            // on the DebugSuspendEvent which clients can set when they
            // want to release us.
            thread->MarkForSuspension(TS_DebugSuspendPending |
                                       TS_DebugWillSync
                      );

            // Resume the thread and let it run to a safe point
            thread->ResumeThread();

            LOG((LF_CORDB, LL_INFO1000,
                 "[0x%x] SUSPEND: gc disabled - will sync.\n",
                 thread->m_ThreadId));
        }
        else if (!thread->m_fPreemptiveGCDisabled)
        {
            // Mark threads that are outside the Runtime so that if
            // they attempt to re-enter they will trip.
            thread->MarkForSuspension(TS_DebugSuspendPending);

            if (str == STR_Success) {
                thread->ResumeThread();
            }

            LOG((LF_CORDB, LL_INFO1000,
                 "[0x%x] SUSPEND: gc enabled.\n", thread->m_ThreadId));
        }
    }

    //
    // Return true if all threads are synchronized now, otherwise the
    // debugge must wait for the SuspendComplete, called from the last
    // thread to sync.
    //

    if (FastInterlockDecrement(&m_DebugWillSyncCount) < 0)
    {
        LOG((LF_CORDB, LL_INFO1000,
             "SUSPEND: all threads sync before return.\n"));
        return true;
    }
    else
        return false;
}

//
// This method is called by the debugger helper thread when it times out waiting for a set of threads to
// synchronize. Its used to chase down threads that are not syncronizing quickly. It returns true if all the threads are
// now synchronized. This also means that we own the thread store lock.
//
// This can be safely called if we're already suspended.
bool Thread::SysSweepThreadsForDebug(bool forceSync)
{
    CONTRACT(bool) {
        NOTHROW;
        DISABLED(GC_TRIGGERS); // WaitUntilConcurrentGCComplete toggle GC mode, disabled because called by unmanaged thread

        // We assume that only the "real" helper thread ever calls this (not somebody doing helper thread duty).
        PRECONDITION(IsDbgHelperSpecialThread());
        PRECONDITION(GetThread() == NULL);

        // Iff we return true, then we have the TSL (or the aux lock used in hacks).
        POSTCONDITION(RETVAL == !!ThreadStore::HoldingThreadStore());
    }
    CONTRACT_END;

    _ASSERTE(!forceSync); // deprecated parameter

    Thread *thread = NULL;

    // NOTE::NOTE::NOTE::NOTE::NOTE
    // This function has parallel logic in SysSuspendForGC.  Please make
    // sure to make appropriate changes there as well.

    ThreadStore::LockThreadStore (GCHeap::SUSPEND_FOR_DEBUGGER);

    SysSuspendInProgressHolder hldSysSuspendInProgress;

    // Loop over the threads...
    while (((thread = ThreadStore::GetThreadList(thread)) != NULL) && (m_DebugWillSyncCount >= 0))
    {
        // Skip threads that we aren't waiting for to sync.
        if ((thread->m_State & TS_DebugWillSync) == 0)
            continue;

        // Suspend the thread
RetrySuspension:
        // We can not allocate memory after we suspend a thread.
        // Otherwise, we may deadlock the process when CLR is hosted.
        ThreadStore::AllocateOSContext();

        SuspendThreadResult str = thread->SuspendThread();

        if (str == STR_Failure || str == STR_UnstartedOrDead)
        {
            // The thread cannot actually be unstarted - if it was, we would not
            // have marked it with TS_DebugWillSync in the first phase.
            _ASSERTE(!(thread->m_State & TS_Unstarted));

            // If the thread has gone, we can't wait on it.
            goto Label_MarkThreadAsSynced;
        }
        else if (str == STR_SwitchedOut)
        {
            // The thread was switched b/c of fiber-mode stuff.
            if (!thread->m_fPreemptiveGCDisabled)
            {
                goto Label_MarkThreadAsSynced;
            }
            else
            {
                goto RetrySuspension;
            }
        }
        else if (str == STR_NoStressLog)
        {
            goto RetrySuspension;
        }
        else if (!thread->m_fPreemptiveGCDisabled)
        {
            // If the thread toggled to preemptive mode, then it's synced.

            // We can safely resume the thread here b/c it's in PreemptiveMode and the
            // EE will trap anybody trying to re-enter cooperative. So letting it run free
            // won't hurt the runtime.
            _ASSERTE(str == STR_Success);
            thread->ResumeThread();

            goto Label_MarkThreadAsSynced;
        }

        // If we didn't take the thread out of the set, then resume it and give it another chance to reach a safe
        // point.
        thread->ResumeThread();
        continue;

        // The thread is synced. Remove the sync bits and dec the sync count.
Label_MarkThreadAsSynced:
        FastInterlockAnd((ULONG *) &thread->m_State, ~TS_DebugWillSync);
        if (FastInterlockDecrement(&m_DebugWillSyncCount) < 0)
        {
            // If that was the last thread, then the CLR is synced.
            // We return while own the thread store lock. We return true now, which indicates this to the caller.
            RETURN true;
        }
        continue;

    } // end looping through Thread Store

    if (m_DebugWillSyncCount < 0)
    {
        RETURN true;
    }

    // The CLR is not yet synced. We release the threadstore lock and return false.
    hldSysSuspendInProgress.Release();
    ThreadStore::UnlockThreadStore();

    RETURN false;
}

void Thread::SysResumeFromDebug(AppDomain *pAppDomain)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    Thread  *thread = NULL;

    if (g_fProcessDetach)
    {
        LOG((LF_CORDB, LL_INFO1000,
             "RESUME: skipping resume due to process detach.\n"));
        return;
    }

    LOG((LF_CORDB, LL_INFO1000, "RESUME: starting resume AD:0x%x.\n", pAppDomain));

    // Notify the client that it should release any threads that it had doing work
    // while the runtime was debugger-suspended.
    IDebuggerThreadControl *pIDTC = CorHost::GetDebuggerThreadControl();
    if (pIDTC)
    {
        LOG((LF_CORDB, LL_INFO1000, "RESUME: notifying IDebuggerThreadControl client.\n"));
        pIDTC->ReleaseAllRuntimeThreads();
    }

    // Make sure we completed the previous sync
    _ASSERTE(m_DebugWillSyncCount == -1);

    // Caller is expected to be holding the ThreadStore lock
    _ASSERTE(ThreadStore::HoldingThreadStore() || g_fProcessDetach);

    while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
    {
        if (pAppDomain != NULL && thread->GetDomain() != pAppDomain)
        {
            LOG((LF_CORDB, LL_INFO1000, "RESUME: Not resuming thread 0x%x, since it's "
                "in appdomain 0x%x.\n", thread, pAppDomain));
            continue;
        }

        // If the user wants to keep the thread suspended, then
        // don't release the thread.
        if (!(thread->m_StateNC & TSNC_DebuggerUserSuspend))
        {
            // If we are still trying to suspend this thread, forget about it.
            if (thread->m_State & TS_DebugSuspendPending)
            {
                LOG((LF_CORDB, LL_INFO1000,
                     "[0x%x] RESUME: TS_DebugSuspendPending was set, but will be removed\n",
                     thread->m_ThreadId));

                // Note: we unmark for suspension _then_ set the suspend event.
                thread->ReleaseFromSuspension(TS_DebugSuspendPending);
            }

        }
        else
        {
            // Thread will remain suspended due to a request from the debugger.

            LOG((LF_CORDB,LL_INFO10000,"Didn't unsuspend thread 0x%x"
                "(ID:0x%x)\n", thread, thread->GetThreadId()));
            LOG((LF_CORDB,LL_INFO10000,"Suspending:0x%x\n",
                thread->m_State & TS_DebugSuspendPending));
            _ASSERTE((thread->m_State & TS_DebugWillSync) == 0);

        }
    }

    LOG((LF_CORDB, LL_INFO1000, "RESUME: resume complete. Trap count: %d\n", g_TrapReturningThreads));
}

// Suspend a thread at the system level.  We distinguish between user suspensions,
// and system suspensions so that a VB program cannot resume a thread we have
// suspended for GC.
//
// This service won't return until the suspension is complete.  This deserves some
// explanation.  The thread is considered to be suspended if it can make no further
// progress within the EE.  For example, a thread that has exited the EE via
// COM Interop or N/Direct is considered suspended -- if we've arranged it so that
// the thread cannot return back to the EE without blocking.
void Thread::UserSuspendThread()
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    // Read the general comments on thread suspension earlier, to understand why we
    // take these locks.

    // GC can occur in here:
    STRESS_LOG0(LF_SYNC, LL_INFO100, "UserSuspendThread obtain lock\n");
    ThreadStoreLockHolder tsl(TRUE);

    // User suspensions (e.g. from VB and C#) are distinguished from internal
    // suspensions so a poorly behaved program cannot resume a thread that the system
    // has suspended for GC.
    if (m_State & TS_UserSuspendPending)
    {
        // This thread is already experiencing a user suspension, so ignore the
        // new request.
        _ASSERTE(!ThreadStore::HoldingThreadStore(this));
    }
    else
    if (this != GetThread())
    {
        // First suspension of a thread other than the current one.
        if (m_State & TS_Unstarted)
        {
            // There is an important window in here.  T1 can call T2.Start() and then
            // T2.Suspend().  Suspend is disallowed on an unstarted thread.  But from T1's
            // point of view, T2 is started.  In reality, T2 hasn't been scheduled by the
            // OS, so it is still an unstarted thread.  We don't want to perform a normal
            // suspension on it in this case, because it is currently contributing to the
            // PendingThreadCount.  We want to get it fully started before we suspend it.
            // This is particularly important if its background status is changing
            // underneath us because otherwise we might not detect that the process should
            // be exited at the right time.
            //
            // It turns out that this is a simple situation to implement.  We are holding
            // the ThreadStoreLock.  TransferStartedThread will likewise acquire that
            // lock.  So if we detect it, we simply set a bit telling the thread to
            // suspend itself.  This is NOT the normal suspension request because we don't
            // want the thread to suspend until it has fully started.
            FastInterlockOr((ULONG *) &m_State, TS_SuspendUnstarted);
        }
        else if (m_State & (TS_Detached | TS_Dead))
        {
            return;
        }
        else
        {
            // We just want to trap this thread if it comes back into cooperative mode
            SetupForSuspension(TS_UserSuspendPending);
            m_SafeEvent.Reset();

            // Pause it so we can operate on it without it squirming under us.
RetrySuspension:
            // We can not allocate memory after we suspend a thread.
            // Otherwise, we may deadlock the process when CLR is hosted.
            ThreadStore::AllocateOSContext();

            SuspendThreadResult str = SuspendThread();

            // The only safe place to suspend a thread asynchronously is if it is in
            // fully interruptible cooperative JIT code.  Preemptive mode can hold all
            // kinds of locks that make it unsafe to suspend.  All other cases are
            // handled somewhat synchronously (e.g. through hijacks, GC mode toggles, etc.)
            //
            // For example, on a SMP if the thread is blocked waiting for the ThreadStore
            // lock, it can cause a deadlock if we suspend it (even though it is in
            // preemptive mode).
            //
            // If a thread is in preemptive mode (including the tricky optimized N/Direct
            // case), we can just mark it for suspension.  It will make no further progress
            // in the EE.
            if (str == STR_NoStressLog)
            {
                // We annot assume anything about the thread's current state.
                goto RetrySuspension;
            }
            else if (!m_fPreemptiveGCDisabled)
            {
                MarkForSuspension(TS_UserSuspendPending);

                // Let the thread run until it reaches a safe spot.
                if (str == STR_Success)
                {
                    ResumeThread();
                }
            }
            else if (str == STR_Failure || str == STR_UnstartedOrDead)
            {
                // The thread cannot be unstarted, as we have already
                // checked for that above.
                _ASSERTE(!(m_State & TS_Unstarted));

                // Nothing to do if the thread has already terminated.
            }
            else if (str == STR_SwitchedOut)
            {
                goto RetrySuspension;
            }
            else
            {
                _ASSERTE(str == STR_Success);

                // Thread is executing in cooperative mode.  We're going to have to
                // move it to a safe spot.
                MarkForSuspension(TS_UserSuspendPending);

                // Let the thread run until it reaches a safe spot.
                ResumeThread();

                // wait until it leaves cooperative GC mode or is JIT suspended
                FinishSuspendingThread();
            }
        }
    }
    else
    {
        GCX_PREEMP();
        SetupForSuspension(TS_UserSuspendPending);
        MarkForSuspension(TS_UserSuspendPending);

        // prepare to block ourselves
        tsl.Release();
        _ASSERTE(!ThreadStore::HoldingThreadStore(this));

        WaitSuspendEvents();
    }
}


// if the only suspension of this thread is user imposed, resume it.  But don't
// resume from any system suspensions (like GC).
BOOL Thread::UserResumeThread()
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    // If we are attempting to resume when we aren't in a user suspension,
    // its an error.
    BOOL    res = FALSE;

    // Note that the model does not count.  In other words, you can call Thread.Suspend()
    // five times and Thread.Resume() once.  The result is that the thread resumes.

    STRESS_LOG0(LF_SYNC, INFO3, "UserResumeThread obtain lock\n");
    ThreadStoreLockHolder TSLockHolder(TRUE);

    // If we have marked a thread for suspension, while that thread is still starting
    // up, simply remove the bit to resume it.
    if (m_State & TS_SuspendUnstarted)
    {
        _ASSERTE((m_State & TS_UserSuspendPending) == 0);
        FastInterlockAnd((ULONG *) &m_State, ~TS_SuspendUnstarted);
        res = TRUE;
    }

    // If we are still trying to suspend the thread, forget about it.
    if (m_State & TS_UserSuspendPending)
    {
        ReleaseFromSuspension(TS_UserSuspendPending);
        SetSafeEvent();
        res = TRUE;
    }

    return res;
}


// We are asynchronously trying to suspend this thread.  Stay here until we achieve
// that goal (in fully interruptible JIT code), or the thread dies, or it leaves
// the EE (in which case the Pending flag will cause it to synchronously suspend
// itself later, or if the thread tells us it is going to synchronously suspend
// itself because of hijack activity, etc.
void Thread::FinishSuspendingThread()
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    DWORD   res;

    // There are two threads of interest -- the current thread and the thread we are
    // going to wait for.  Since the current thread is about to wait, it's important
    // that it be in preemptive mode at this time.

#if _DEBUG
    DWORD   dbgTotalTimeout = 0;
#endif

    // Wait for us to enter the ping period, then check if we are in interruptible
    // JIT code.
    while (TRUE)
    {
        ThreadStore::UnlockThreadStore();
        res = m_SafeEvent.Wait(PING_JIT_TIMEOUT,FALSE);
        STRESS_LOG0(LF_SYNC, INFO3, "FinishSuspendingThread obtain lock\n");
        ThreadStore::LockThreadStore(GCHeap::SUSPEND_OTHER);

        if (res == WAIT_TIMEOUT)
        {
#ifdef _DEBUG
            if ((dbgTotalTimeout += PING_JIT_TIMEOUT) >= g_pConfig->SuspendDeadlockTimeout())
            {
                _ASSERTE(!"Timeout detected trying to synchronously suspend a thread");
                dbgTotalTimeout = 0;
            }
#endif
            // Suspend the thread and see if we are in interruptible code (placing
            // a hijack if warranted).
            // The thread is detached/dead.  Suspend is no op.
            if (m_State & (TS_Detached | TS_Dead))
            {
                return;
            }

            // We can not allocate memory after we suspend a thread.
            // Otherwise, we may deadlock the process when CLR is hosted.
            ThreadStore::AllocateOSContext();

            SuspendThreadResult str = SuspendThread();

            if (m_fPreemptiveGCDisabled && str == STR_Success)
            {
                // Keep trying...
                ResumeThread();
            }
            else if (!m_fPreemptiveGCDisabled)
            {
                // The thread has transitioned out of the EE.  It can't get back in
                // without synchronously suspending itself.  We can now return to our
                // caller since this thread cannot make further progress within the
                // EE.
                if (str == STR_Success)
                {
                    ResumeThread();
                }
                break;
            }
            else if (str == STR_SwitchedOut)
            {
                // The task has been switched out while in Cooperative GC mode.
                // We will wait for the thread again.
            }
        }
        else
        {
            // SafeEvent has been set so we don't need to actually suspend.  Either
            // the thread died, or it will enter a synchronous suspension based on
            // the UserSuspendPending bit.
            _ASSERTE(res == WAIT_OBJECT_0);
            _ASSERTE(!ThreadStore::HoldingThreadStore(this));
            break;
        }
    }
}


void Thread::SetSafeEvent()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    m_SafeEvent.Set();
}


/*
 *
 * WaitSuspendEventsHelper
 *
 * This function is a simple helper function for WaitSuspendEvents.  It is needed
 * because of the EX_TRY macro.  This macro does an alloca(), which allocates space
 * off the stack, not free'ing it.  Thus, doing a EX_TRY in a loop can easily result
 * in a stack overflow error.  By factoring out the EX_TRY into a separate function,
 * we recover that stack space.
 *
 * Parameters:
 *   None.
 *
 * Return:
 *   true if meant to continue, else false.
 *
 */
BOOL Thread::WaitSuspendEventsHelper(void)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    DWORD result = WAIT_FAILED;

    EX_TRY {

        if (m_State & TS_UserSuspendPending) {

            ThreadState oldState = m_State;

            while (oldState & TS_UserSuspendPending) {

                ThreadState newState = (ThreadState)(oldState | TS_SyncSuspended);
                if (FastInterlockCompareExchange((LONG *)&m_State, newState, oldState) == (LONG)oldState)
                {
                    result = m_UserSuspendEvent.Wait(INFINITE,FALSE);
#if _DEBUG
                    newState = m_State;
                    _ASSERTE(!(newState & TS_SyncSuspended) || (newState & TS_DebugSuspendPending));
#endif
                    break;
                }

                oldState = m_State;
            }


        } else if (m_State & TS_DebugSuspendPending) {

            ThreadState oldState = m_State;

            while (oldState & TS_DebugSuspendPending) {

                ThreadState newState = (ThreadState)(oldState | TS_SyncSuspended);
                if (FastInterlockCompareExchange((LONG *)&m_State, newState, oldState) == (LONG)oldState)
                {
                    result = m_DebugSuspendEvent.Wait(INFINITE,FALSE);
#if _DEBUG
                    newState = m_State;
                    _ASSERTE(!(newState & TS_SyncSuspended) || (newState & TS_UserSuspendPending));
#endif
                    break;
                }

                oldState = m_State;
            }
        }
    }
    EX_CATCH {
    }
    EX_END_CATCH(SwallowAllExceptions)

    return result != WAIT_OBJECT_0;
}


void Thread::WaitSuspendEvents(BOOL fDoWait)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    _ASSERTE(!PreemptiveGCDisabled());
    _ASSERTE((m_State & TS_SyncSuspended) == 0);

    // Let us do some useful work before suspending ourselves.

    // If we're required to perform a wait, do so.  Typically, this is
    // skipped if this thread is a Debugger Special Thread.
    if (fDoWait)
    {
        while (TRUE)
        {
            WaitSuspendEventsHelper();

            ThreadState oldState = m_State;

            //
            // If all reasons to suspend are off, we think we can exit
            // this loop, but we need to check atomically.
            //
            if ((oldState & (TS_UserSuspendPending | TS_DebugSuspendPending)) == 0)
            {
                //
                // Construct the destination state we desire - all suspension bits turned off.
                //
                ThreadState newState = (ThreadState)(oldState & ~(TS_UserSuspendPending |
                                                                  TS_DebugSuspendPending |
                                                                  TS_SyncSuspended));

                if (FastInterlockCompareExchange((LONG *)&m_State, newState, oldState) == (LONG)oldState)
                {
                    //
                    // We are done.
                    //
                    break;
                }
            }
        }
    }
}

#endif // #ifndef DACCESS_COMPILE

//
// InitRegDisplay: initializes a REGDISPLAY for a thread. If validContext
// is false, pRD is filled from the current context of the thread. The
// thread's current context is also filled in pctx. If validContext is true,
// pctx should point to a valid context and pRD is filled from that.
//
bool Thread::InitRegDisplay(const PREGDISPLAY pRD, PCONTEXT pctx, bool validContext)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (!validContext)
    {
        if (GetFilterContext()!= NULL)
        {
            pctx = GetFilterContext();
        }
        else
        {
            pctx->ContextFlags = CONTEXT_FULL;

            _ASSERTE(this != GetThread());  // do not call GetThreadContext on the active thread
            BOOL ret = ::GetThreadContext(GetThreadHandle(), pctx);

            if (!ret)
            {
                SetIP(pctx, 0);
#ifdef _X86_
                pRD->pPC  = (SLOT*)&(pctx->Eip);
                pRD->PCTAddr = (TADDR)&(pctx->Eip);
#elif defined(_PPC_)
                pctx->Iar = 0;
                pRD->pPC = (SLOT*)&(pctx->Iar);
#else
                PORTABILITY_ASSERT("NYI for platform Thread::InitRegDisplay");
#endif

                return false;
            }
        }
    }

    FillRegDisplay( pRD, pctx );

    return true;
}


void Thread::FillRegDisplay(const PREGDISPLAY pRD, PCONTEXT pctx)
{
    ::FillRegDisplay(pRD, pctx);

}




#ifndef DACCESS_COMPILE


#endif // #ifndef DACCESS_COMPILE


//                      Trip Functions
//                      ==============
// When a thread reaches a safe place, it will rendezvous back with us, via one of
// the following trip functions:

void __cdecl CommonTripThread()
{
#ifndef DACCESS_COMPILE
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    Thread  *thread = GetThread();

    thread->HandleThreadAbort ();

    if (thread->IsYieldRequested())
    {
        __SwitchToThread(0);
    }

    if (thread->CatchAtSafePoint())
    {
        _ASSERTE(!ThreadStore::HoldingThreadStore(thread));

        // Trap
        thread->PulseGCMode();
    }
#else
    DacNotImpl();
#endif // #ifndef DACCESS_COMPILE
}

#ifndef DACCESS_COMPILE

//
// Either the interpreter is executing a break opcode or a break instruction
// has been caught by the exception handling. In either case, we want to
// have this thread wait before continuing to execute. We do this with a
// PulseGCMode, which will trip the tread and leave it waiting on its suspend
// event. This case does not call CommonTripThread because we don't want
// to give the thread the chance to exit or otherwise suspend itself.
//
VOID OnDebuggerTripThread(void)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    Thread  *thread = GetThread();

    if (thread->m_State & thread->TS_DebugSuspendPending)
    {
        _ASSERTE(!ThreadStore::HoldingThreadStore(thread));
        thread->PulseGCMode();
    }
}

void Thread::SetFilterContext(CONTEXT *pContext)
{
    // SetFilterContext is like pushing a Frame onto the Frame chain.
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE; // Absolutely must be in coop to coordinate w/ Runtime suspension.
        PRECONDITION(GetThread() == this); // must be on current thread.
    } CONTRACTL_END;

    m_debuggerFilterContext = pContext;
}

#endif // #ifndef DACCESS_COMPILE

CONTEXT *Thread::GetFilterContext(void)
{
    LEAF_CONTRACT;

   return m_debuggerFilterContext;
}

#ifndef DACCESS_COMPILE

void Thread::SetDebugCantStop(bool fCantStop)
{
    LEAF_CONTRACT;

    if (fCantStop)
    {
        IncCantStopCount(StateHolderParamValue);
        m_debuggerCantStop++;
    }
    else
    {
        DecCantStopCount(StateHolderParamValue);
        m_debuggerCantStop--;
    }
}

bool Thread::GetDebugCantStop(void)
{
    LEAF_CONTRACT;

    return m_debuggerCantStop != 0;
}

#endif // #ifndef DACCESS_COMPILE


#ifndef DACCESS_COMPILE

//-----------------------------------------------------------------------------
// Call w/a  wrapper.
// We've already transitioned AppDomains here. This just places a 1st-pass filter to sniff
// for catch-handler found callbacks for the debugger.
//-----------------------------------------------------------------------------
void MakeADCallDebuggerWrapper(
    FPAPPDOMAINCALLBACK fpCallback,
    CtxTransitionBaseArgs * args,
    ContextTransitionFrame* pFrame)
{
    BYTE * pCatcherStackAddr = (BYTE*) pFrame;

    PAL_TRY
    {
        fpCallback(args);
    }
    PAL_EXCEPT_FILTER(NotifyOfCHFFilterWrapper, pCatcherStackAddr)
    {
        // Should never reach here b/c handler should always continue search.
        _ASSERTE(false);
    }
    PAL_ENDTRY
}


// Invoke a callback in another appdomain.
// Caller should have checked that we're actually transitioning domains here.
void MakeCallWithAppDomainTransition(
    ADID TargetDomain,
    FPAPPDOMAINCALLBACK fpCallback,
    CtxTransitionBaseArgs * args)
{
    DEBUG_ASSURE_NO_RETURN_BEGIN


    Thread*     _ctx_trans_pThread          = GetThread();
    TESTHOOKCALL(EnteringAppDomain((TargetDomain.m_dwId)));     
    AppDomainFromIDHolder pTargetDomain(TargetDomain, TRUE);
    pTargetDomain.ThrowIfUnloaded();
    _ASSERTE(_ctx_trans_pThread != NULL);
    _ASSERTE(_ctx_trans_pThread->GetDomain()->GetId()!= TargetDomain);

    bool        _ctx_trans_fRaiseNeeded     = false;
    Exception* _ctx_trans_pTargetDomainException=NULL;                   \

    FrameWithCookie<ContextTransitionFrame>  _ctx_trans_Frame;
    ContextTransitionFrame* _ctx_trans_pFrame = &_ctx_trans_Frame;
    _ctx_trans_pThread->EnterContextRestricted(
        pTargetDomain->GetDefaultContext(),
        _ctx_trans_pFrame);
    pTargetDomain.Release();
    args->pCtxFrame = _ctx_trans_pFrame;
    TESTHOOKCALL(EnteredAppDomain((TargetDomain.m_dwId))); 
    /* work around unreachable code warning */
    EX_TRY
    {
        // Invoke the callback
        if (CORDebuggerAttached())
        {
            // If a debugger is attached, do it through a wrapper that will sniff for CHF callbacks.
            MakeADCallDebuggerWrapper(fpCallback, args, GET_CTX_TRANSITION_FRAME());
        }
        else
        {
            // If no debugger is attached, call directly.
            fpCallback(args);
        }
    }
    EX_CATCH
    {
        LOG((LF_EH|LF_APPDOMAIN, LL_INFO1000, "ENTER_DOMAIN(%s, %s, %d): exception in flight\n",
            __FUNCTION__, __FILE__, __LINE__));

        _ctx_trans_pTargetDomainException=EXTRACT_EXCEPTION();;                   
        _ctx_trans_fRaiseNeeded = true;
    }
    /* SwallowAllExceptions is fine because we don't get to this point */
    /* unless fRaiseNeeded = true or no exception was thrown */
    EX_END_CATCH(SwallowAllExceptions);
    TESTHOOKCALL(LeavingAppDomain((TargetDomain.m_dwId)));     
    if (_ctx_trans_fRaiseNeeded)
    {
        LOG((LF_EH, LL_INFO1000, "RaiseCrossContextException(%s, %s, %d)\n",
            __FUNCTION__, __FILE__, __LINE__));
        _ctx_trans_pThread->RaiseCrossContextException(_ctx_trans_pTargetDomainException,_ctx_trans_pFrame);
    }

    LOG((LF_APPDOMAIN, LL_INFO1000, "LEAVE_DOMAIN(%s, %s, %d)\n",
            __FUNCTION__, __FILE__, __LINE__));

    _ctx_trans_pThread->ReturnToContext(_ctx_trans_pFrame);
    
    DEBUG_ASSURE_NO_RETURN_END
}


// Some simple helpers to keep track of the threads we are waiting for
void Thread::MarkForSuspension(ULONG bit)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(bit == TS_DebugSuspendPending ||
             bit == (TS_DebugSuspendPending | TS_DebugWillSync) ||
             bit == TS_UserSuspendPending);

    _ASSERTE(g_fProcessDetach || ThreadStore::HoldingThreadStore());

    _ASSERTE((m_State & bit) == 0);

    FastInterlockOr((ULONG *) &m_State, bit);
    ThreadStore::TrapReturningThreads(TRUE);
}

void Thread::UnmarkForSuspension(ULONG mask)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(mask == ~TS_DebugSuspendPending ||
             mask == ~TS_UserSuspendPending);

    _ASSERTE(g_fProcessDetach || ThreadStore::HoldingThreadStore());

    _ASSERTE((m_State & ~mask) != 0);

    // we decrement the global first to be able to satisfy the assert from DbgFindThread
    ThreadStore::TrapReturningThreads(FALSE);
    FastInterlockAnd((ULONG *) &m_State, mask);
}

void Thread::SetExposedContext(Context *c)
{

    // Set the ExposedContext ...

    // Note that we use GetxxRaw() here to cover our bootstrap case
    // for AppDomain proxy creation
    // Leaving the exposed object NULL lets us create the default
    // managed context just before we marshal a new AppDomain in
    // RemotingServices::CreateProxyForDomain.

    Thread* pThread = GetThread();
    if (!pThread)
        return;

    CONTRACTL {
        NOTHROW;
        MODE_COOPERATIVE;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if(m_ExposedObject != NULL) {
        THREADBASEREF threadObj = (THREADBASEREF) ObjectFromHandle(m_ExposedObject);
        if(threadObj != NULL)
        if (!c)
            threadObj->SetExposedContext(NULL);
        else
            threadObj->SetExposedContext(c->GetExposedObjectRaw());

    }
}


void Thread::InitContext()
{
    CONTRACTL {
        THROWS;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    // this should only be called when initializing a thread
    _ASSERTE(m_Context == NULL);
    _ASSERTE(m_pDomain == NULL);
    GCX_COOP_NO_THREAD_BROKEN();
    m_Context = SystemDomain::System()->DefaultDomain()->GetDefaultContext();
    SetExposedContext(m_Context);
    m_pDomain = m_Context->GetDomain();
    _ASSERTE(m_pDomain);
    m_pDomain->ThreadEnter(this, NULL);
    // Every thread starts in the default domain, so push it here.
    PushDomain((ADID)DefaultADID);
}

void Thread::ClearContext()
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    // if one is null, both must be
    _ASSERTE(m_pDomain && m_Context || ! (m_pDomain && m_Context));

    if (!m_pDomain)
        return;

    m_pDomain->ThreadExit(this, NULL);

    // must set exposed context to null first otherwise object verification
    // checks will fail AV when m_Context is null
    SetExposedContext(NULL);
    m_pDomain = NULL;
    m_Context = NULL;
    m_ADStack.ClearDomainStack();
}


void Thread::DoContextCallBack(ADID appDomain, Context *pContext, Context::ADCallBackFcnType pTarget, LPVOID args)
{
    //Do not deference pContext if it's not from the current appdomain

#ifdef _DEBUG
    LPVOID espVal = GetCurrentSP();

    LOG((LF_APPDOMAIN, LL_INFO100, "Thread::DoADCallBack Calling %p at esp %p in [%d]\n",
            pTarget, espVal, appDomain.m_dwId));
#endif
    _ASSERTE(GetThread()->GetContext() != pContext);
    Thread* pThread  = GetThread();

    // Get the default context for the current domain as well as for the
    // destination domain.
    AppDomain*  pCurrDomain     = pThread->GetContext()->GetDomain();
    Context*    pCurrDefCtx     = pCurrDomain->GetDefaultContext();
    BOOL  bDefaultTargetCtx=FALSE;

    {
        AppDomainFromIDHolder ad(appDomain, TRUE);
        ad.ThrowIfUnloaded();
        bDefaultTargetCtx=(ad->GetDefaultContext()==pContext);
    }

    if (pCurrDefCtx == pThread->GetContext() && bDefaultTargetCtx)
    {
        ENTER_DOMAIN_ID(appDomain);
        (pTarget)(args);
        END_DOMAIN_TRANSITION;
    }
    else
    {
        _ASSERTE(pContext->GetDomain()==::GetAppDomain());
        Context::ADCallBackArgs callTgtArgs = {pTarget, args};
        Context::CallBackInfo callBackInfo = {Context::ADTransition_callback, (void*) &callTgtArgs};
        Context::RequestCallBack(appDomain,pContext, (void*) &callBackInfo);
    }
    LOG((LF_APPDOMAIN, LL_INFO100, "Thread::DoADCallBack Done at esp %p\n", espVal));
}


void Thread::DoADCallBack(AppDomain* pDomain , Context::ADCallBackFcnType pTarget, LPVOID args, DWORD dwADV)
{


#ifdef _DEBUG
    LPVOID espVal = GetCurrentSP();

    LOG((LF_APPDOMAIN, LL_INFO100, "Thread::DoADCallBack Calling %p at esp %p in [%d]\n",
            pTarget, espVal, pDomain->GetId().m_dwId));
#endif
    Thread* pThread  = GetThread();

    // Get the default context for the current domain as well as for the
    // destination domain.
    AppDomain*  pCurrDomain     = pThread->GetContext()->GetDomain();

    if (pCurrDomain!=pDomain)
    {
        // use the target domain's default context as the target context
        // so that the actual call to a transparent proxy would enter the object into the correct context.

        BOOL fThrow = FALSE;

        ENTER_DOMAIN_PTR(pDomain,dwADV)
        {
            (pTarget)(args);

            // unloadBoundary is cleared by ReturnToContext, so get it now.
            Frame* unloadBoundaryFrame = pThread->GetUnloadBoundaryFrame();
            fThrow = pThread->ShouldChangeAbortToUnload(GET_CTX_TRANSITION_FRAME(), unloadBoundaryFrame);
        }
        END_DOMAIN_TRANSITION;
        // if someone caught the abort before it got back out to the AD transition (like DispatchEx_xxx does)
        // then need to turn the abort into an unload, as they're gonna keep seeing it anyway
        if (fThrow)
        {
            LOG((LF_APPDOMAIN, LL_INFO10, "Thread::DoADCallBack turning abort into unload\n"));
            COMPlusThrow(kAppDomainUnloadedException, L"Remoting_AppDomainUnloaded_ThreadUnwound");
        }
    }
    else
    {
        Context::ADCallBackArgs callTgtArgs = {pTarget, args};
        Context::CallBackInfo callBackInfo = {Context::ADTransition_callback, (void*) &callTgtArgs};
        Context::RequestCallBack(CURRENT_APPDOMAIN_ID, pCurrDomain->GetDefaultContext(), (void*) &callBackInfo);
    }
    LOG((LF_APPDOMAIN, LL_INFO100, "Thread::DoADCallBack Done at esp %p\n", espVal));
}



void Thread::DoADCallBack(ADID appDomainID , Context::ADCallBackFcnType pTarget, LPVOID args)
{


#ifdef _DEBUG
    LPVOID espVal = GetCurrentSP();

    LOG((LF_APPDOMAIN, LL_INFO100, "Thread::DoADCallBack Calling %p at esp %p in [%d]\n",
            pTarget, espVal, appDomainID.m_dwId));
#endif
    Thread* pThread  = GetThread();

    // Get the default context for the current domain as well as for the
    // destination domain.
    AppDomain*  pCurrDomain     = pThread->GetContext()->GetDomain();

    if (pCurrDomain->GetId()!=appDomainID)
    {
        // use the target domain's default context as the target context
        // so that the actual call to a transparent proxy would enter the object into the correct context.

        BOOL fThrow = FALSE;

        ENTER_DOMAIN_ID(appDomainID)
        {
            (pTarget)(args);

            // unloadBoundary is cleared by ReturnToContext, so get it now.
            Frame* unloadBoundaryFrame = pThread->GetUnloadBoundaryFrame();
            fThrow = pThread->ShouldChangeAbortToUnload(GET_CTX_TRANSITION_FRAME(), unloadBoundaryFrame);
        }
        END_DOMAIN_TRANSITION;
        // if someone caught the abort before it got back out to the AD transition (like DispatchEx_xxx does)
        // then need to turn the abort into an unload, as they're gonna keep seeing it anyway
        if (fThrow)
        {
            LOG((LF_APPDOMAIN, LL_INFO10, "Thread::DoADCallBack turning abort into unload\n"));
            COMPlusThrow(kAppDomainUnloadedException, L"Remoting_AppDomainUnloaded_ThreadUnwound");
        }
    }
    else
    {
        Context::ADCallBackArgs callTgtArgs = {pTarget, args};
        Context::CallBackInfo callBackInfo = {Context::ADTransition_callback, (void*) &callTgtArgs};
        Context::RequestCallBack(CURRENT_APPDOMAIN_ID, pCurrDomain->GetDefaultContext(), (void*) &callBackInfo);
    }
    LOG((LF_APPDOMAIN, LL_INFO100, "Thread::DoADCallBack Done at esp %p\n", espVal));
}

void Thread::EnterContextRestricted(Context *pContext, ContextTransitionFrame *pFrame)
{
    CONTRACTL {
        THROWS;
        MODE_COOPERATIVE;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(GetThread() == this);
    _ASSERTE(pContext);     // should never enter a null context
    _ASSERTE(m_Context);    // should always have a current context

    AppDomain *pPrevDomain = m_pDomain;
    AppDomain *pDomain = pContext->GetDomain();
    // and it should always have an AD set
    _ASSERTE(pDomain);

    if (m_pDomain != pDomain && !pDomain->CanThreadEnter(this))
    {
        pFrame->SetReturnContext(NULL);
        COMPlusThrow(kAppDomainUnloadedException);
    }

    pFrame->SetReturnContext(m_Context);
    pFrame->SetReturnExecutionContext(NULL);

    if (pPrevDomain != pDomain)
    {
    pFrame->SetLockCount(m_dwBeginLockCount, m_dwBeginCriticalRegionCount);
    m_dwBeginLockCount = m_dwLockCount;
    m_dwBeginCriticalRegionCount = m_dwCriticalRegionCount;
    }

    if (m_Context == pContext) {
        _ASSERTE(m_Context->GetDomain() == pContext->GetDomain());
        return;
    }

    LOG((LF_APPDOMAIN, LL_INFO1000, "%sThread::EnterContext from (%p) [%d] (count %d)\n",
            GCHeap::GetGCHeap()->IsCurrentThreadFinalizer() ? "FT: " : "",
            m_Context, m_Context->GetDomain()->GetId().m_dwId,
            m_Context->GetDomain()->GetThreadEnterCount()));
    LOG((LF_APPDOMAIN, LL_INFO1000, "                     into (%p) [%d] (count %d)\n", pContext,
                pContext->GetDomain()->GetId().m_dwId,
                pContext->GetDomain()->GetThreadEnterCount()));

#ifdef _DEBUG_ADUNLOAD
    printf("Thread::EnterContext %x from (%8.8x) [%d]\n", GetThreadId(), m_Context,
        m_Context ? m_Context->GetDomain()->GetId() : -1);
    printf("                     into (%8.8x) [%d] %S\n", pContext,
                pContext->GetDomain()->GetId());
#endif

    CantStopHolder hCantStop;

    bool fChangedDomains = m_pDomain != pDomain;
    if (fChangedDomains)
    {
        if (pDomain == SystemDomain::System()->DefaultDomain() &&
            GetEEPolicy()->GetActionOnFailure(FAIL_StackOverflow) == eRudeUnloadAppDomain)
        {
#ifndef TOTALLY_DISBLE_STACK_GUARDS
            // Make sure default domain does not see SO.
            // probe for our entry point amount and throw if not enough stack
            RetailStackProbe(ADJUST_PROBE(DEFAULT_ENTRY_PROBE_AMOUNT*2), this);
#endif
        }

        _ASSERTE(pFrame);

        MEMORY_REPORT_SET_APP_DOMAIN(pDomain);

        PushDomain(pDomain->GetId());
        STRESS_LOG1(LF_APPDOMAIN, LL_INFO100,"Entering into ADID=%d",pDomain->GetId().m_dwId);

        //
        // Push execution contexts (that could contain call context) into frame to avoid leaks
        //

        if (IsExposedObjectSet())
        {
            THREADBASEREF ref = (THREADBASEREF) ObjectFromHandle(m_ExposedObject);
            _ASSERTE(ref != NULL);
            if (ref->GetExecutionContext() != NULL)
            {
                pFrame->SetReturnExecutionContext(ref->GetExecutionContext());
                ref->SetExecutionContext(NULL);
            }
        }

        //
        // Store the last thrown object in the ContextTransitionFrame before we null it out
        // to prevent it from leaking into the domain we are transitionning into.
        //
        
        pFrame->SetLastThrownObjectInParentContext(LastThrownObject());
        SafeSetLastThrownObject(NULL);
    }

    m_Context = pContext;
    pFrame->Push();

#ifdef _DEBUG_ADUNLOAD
    printf("Thread::EnterContext %x,%8.8x push? %d current frame is %8.8x\n", GetThreadId(), this, 1, GetFrame());
#endif

    if (fChangedDomains)
    {
        pDomain->ThreadEnter(this, pFrame);

        // Setting static data store to null. On next access to static data, they'll be updated correctly
        m_pSharedStaticData = NULL;
        m_pUnsharedStaticData = NULL;

        m_pDomain = pDomain;
        UnsafeTlsSetValue(GetAppDomainTLSIndex(), (VOID*) m_pDomain);
    }
    SetExposedContext(pContext);
}

// main difference between EnterContext and ReturnToContext is that are allowed to return
// into a domain that is unloading but cannot enter a domain that is unloading
void Thread::ReturnToContext(ContextTransitionFrame *pFrame)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    _ASSERTE(GetThread() == this);

    Context *pReturnContext = pFrame->GetReturnContext();
    _ASSERTE(pReturnContext);

    ADID pADOnStack;

    AppDomain *pReturnDomain = pReturnContext->GetDomain();
    AppDomain* pCurrentDomain = m_pDomain;

    bool fChangedDomains = m_pDomain != pReturnDomain;

    if (fChangedDomains)
    {

        /*_ASSERTE ((IsRudeAbort() && !GetAppDomain()->IsDefaultDomain()) ||
                  (m_dwBeginLockCount == m_dwLockCount &&
                   m_dwBeginCriticalRegionCount == m_dwCriticalRegionCount));*/

        if(!(IsRudeAbort() && !GetAppDomain()->IsDefaultDomain()) &&
           (m_dwBeginLockCount != m_dwLockCount ||
            m_dwBeginCriticalRegionCount != m_dwCriticalRegionCount))
            STRESS_LOG0(LF_SYNC, LL_INFO1000, "Locks are orphaned while exiting a domain\n");

#ifdef _DEBUG
        if (!(IsRudeAbort() && !GetAppDomain()->IsDefaultDomain()) &&
            !(m_dwBeginLockCount == m_dwLockCount &&
             m_dwBeginCriticalRegionCount == m_dwCriticalRegionCount))
        {
            STRESS_LOG0 (LF_APPDOMAIN, LL_INFO10, "Thread::ReturnToContext Lock not released\n");
        }
#endif

        if (m_dwBeginLockCount != m_dwLockCount ||
            m_dwBeginCriticalRegionCount != m_dwCriticalRegionCount)
        {

            AppDomain *pFromDomain = GetAppDomain();

            // There is a race when EE Thread for a new thread is allocated in the place of the old EE Thread.
            // The lock accounting will get confused if there are orphaned locks. Set the flag that allows us to relax few asserts.
            pFromDomain->SetOrphanedLocks();

            if (!pFromDomain->IsDefaultDomain())
            {
                // If a Thread orphaned a lock, we don't want a host to recycle the Thread object,
                // since the lock count is reset when the thread leaves this domain.
                SetThreadStateNC(TSNC_CannotRecycle);
            }

            // It is a disaster if a lock leaks in default domain.  We can never unload default domain.
            // _ASSERTE (!pFromDomain->IsDefaultDomain());
            EPolicyAction action = GetEEPolicy()->GetActionOnFailure(FAIL_OrphanedLock);
            switch (action)
            {
            case eUnloadAppDomain:
                if (!pFromDomain->IsDefaultDomain())
                {
                    pFromDomain->EnableADUnloadWorker(EEPolicy::ADU_Safe);
                }
                break;
            case eRudeUnloadAppDomain:
                if (!pFromDomain->IsDefaultDomain())
                {
                    pFromDomain->EnableADUnloadWorker(EEPolicy::ADU_Rude);
                }
                break;
            case eExitProcess:
            case eFastExitProcess:
            case eRudeExitProcess:
            case eDisableRuntime:
                GetEEPolicy()->HandleExitProcessFromEscalation(action,HOST_E_EXITPROCESS_ADUNLOAD);
                break;
            default:
                break;
            }
        }

        m_dwLockCount = m_dwBeginLockCount;
        m_dwCriticalRegionCount = m_dwBeginCriticalRegionCount;

        pFrame->GetLockCount(&m_dwBeginLockCount, &m_dwBeginCriticalRegionCount);
    }

    #if defined(PRESERVE_WATSON_ACROSS_CONTEXTS) // Watson isn't implemented on Rotor or DAC
    if (fChangedDomains)
    {
        EX_TRY
        {   // Save Watson info now, because we may be about to lose the domain.  This will only do
            //  any significant processing if there is an exception in flight.
            this->GetExceptionState()->CaptureUnhandledInfoForWatson();
        }
        EX_CATCH
        {
            LOG((LF_APPDOMAIN|LF_EH, LL_INFO10, "Thread::ReturnToContext: CaptureUnhandledInfoForWatson threw exception\n"));
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
    #endif // rotor or dac

    if (m_Context == pReturnContext)
    {
        _ASSERTE(m_Context->GetDomain() == pReturnContext->GetDomain());
        return;
    }

    GCX_COOP();

    LOG((LF_APPDOMAIN, LL_INFO1000, "%sThread::ReturnToContext from (%p) [%d] (count %d)\n",
                GCHeap::GetGCHeap()->IsCurrentThreadFinalizer() ? "FT: " : "",
                m_Context, m_Context->GetDomain()->GetId().m_dwId,
                m_Context->GetDomain()->GetThreadEnterCount()));
    LOG((LF_APPDOMAIN, LL_INFO1000, "                        into (%p) [%d] (count %d)\n", pReturnContext,
                pReturnContext->GetDomain()->GetId().m_dwId,
                pReturnContext->GetDomain()->GetThreadEnterCount()));

#ifdef _DEBUG_ADUNLOAD
    printf("Thread::ReturnToContext %x from (%p) [%d]\n", GetThreadId(), m_Context,
                m_Context->GetDomain()->GetId(),
    printf("                        into (%p) [%d]\n", pReturnContext,
                pReturnContext->GetDomain()->GetId(),
                m_Context->GetDomain()->GetThreadEnterCount());
#endif

    CantStopHolder hCantStop;

    m_Context = pReturnContext;
    SetExposedContext(pReturnContext);

    if (fChangedDomains)
    {
        MEMORY_REPORT_SET_APP_DOMAIN(m_pDomain);

        pADOnStack = m_ADStack.PopDomain();
        STRESS_LOG2(LF_APPDOMAIN, LL_INFO100, "Returning from %d to %d\n", pReturnContext->GetDomain()->GetId().m_dwId, pADOnStack.m_dwId);

        _ASSERTE(pADOnStack == m_pDomain->GetId());

        _ASSERTE(pFrame);
        //_ASSERTE(!fLinkFrame || pThread->GetFrame() == pFrame);

        // Setting static data store to null. On next access to static data, they'll be updated correctly
        m_pSharedStaticData = NULL;
        m_pUnsharedStaticData = NULL;

        m_pDomain = pReturnDomain;
        UnsafeTlsSetValue(GetAppDomainTLSIndex(), (VOID*) pReturnDomain);

            if (pFrame == m_pUnloadBoundaryFrame)
        {
                m_pUnloadBoundaryFrame = NULL;
            if (IsAbortRequested())
            {
                EEResetAbort(TAR_ADUnload);
            }
            ResetBeginAbortedForADUnload();
        }

        // Restore the last thrown object to what it was before the AD transition. Note that if
        // an exception was thrown out of the AD we transitionned into, it will be raised in
        // RaiseCrossContextException and the EH system will store it as the last thrown 
        // object if it gets handled by an EX_CATCH.
        SafeSetLastThrownObject(pFrame->GetLastThrownObjectInParentContext());
    }

    pFrame->Pop();

    if (fChangedDomains)
    {
        //
        // Pop execution contexts (could contain call context) from frame if applicable
        //

        if (IsExposedObjectSet())
        {
            THREADBASEREF ref = (THREADBASEREF) ObjectFromHandle(m_ExposedObject);
            _ASSERTE(ref != NULL);
            ref->SetExecutionContext(pFrame->GetReturnExecutionContext());
        }

        // Do this last so that thread is not labeled as out of the domain until all cleanup is done.
        ADID adid=pCurrentDomain->GetId();
        pCurrentDomain->ThreadExit(this, pFrame);
    }

    if (fChangedDomains && IsAbortRequested() && HasLockInCurrentDomain())
    {
        EPolicyAction action = GetEEPolicy()->GetActionOnFailure(FAIL_CriticalResource);
        // It is a disaster if a lock leaks in default domain.  We can never unload default domain.
        // _ASSERTE (action == eThrowException || !pReturnDomain->IsDefaultDomain());
        switch (action)
        {
        case eUnloadAppDomain:
            if (!pReturnDomain->IsDefaultDomain())
            {
                pReturnDomain->EnableADUnloadWorker(EEPolicy::ADU_Safe);
            }
            break;
        case eRudeUnloadAppDomain:
            if (!pReturnDomain->IsDefaultDomain())
            {
                pReturnDomain->EnableADUnloadWorker(EEPolicy::ADU_Rude);
            }
            break;
        case eExitProcess:
        case eFastExitProcess:
        case eRudeExitProcess:
        case eDisableRuntime:
            GetEEPolicy()->HandleExitProcessFromEscalation(action,HOST_E_EXITPROCESS_ADUNLOAD);
            break;
        default:
            break;
        }
    }

#ifdef _DEBUG_ADUNLOAD
    printf("Thread::ReturnToContext %x,%8.8x pop? %d current frame is %8.8x\n", GetThreadId(), this, 1, GetFrame());
#endif

    return;
}


void Thread::ReturnToContextAndThrow(ContextTransitionFrame* pFrame, EEException* pEx, BOOL* pContextSwitched)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pContextSwitched));
    }
    CONTRACTL_END;
    ReturnToContext(pFrame);
    *pContextSwitched=TRUE;
    
    COMPlusThrow(CLRException::GetThrowableFromException(pEx));
}

void Thread::ReturnToContextAndOOM(ContextTransitionFrame* pFrame)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    ReturnToContext(pFrame);
    
    COMPlusThrowOM();
}

// for cases when marshaling is not needed
// throws it is able to take a shortcut, otherwise just returns
void Thread::RaiseCrossContextExceptionHelper(Exception* pEx, ContextTransitionFrame* pFrame)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;


#define RETURNANDTHROWNEWEXCEPTION(pOldException, Type, ExArgs)                 \
    {                                                                           \
    Exception::Delete(pOldException);                                           \
    SetLastThrownObject(NULL);                                                  \
    ReturnToContext(pFrame);                                                    \
    CONTRACT_VIOLATION(ThrowsViolation);                                        \
    TESTHOOKCALL(LeftAppDomain(adid.m_dwId));                                   \
    Type ex ExArgs;                                                             \
    COMPlusThrow(CLRException::GetThrowableFromException(&ex));                 \
    }

#define RETURNANDRETHROW(ex)                                                    \
    {                                                                           \
        SafeSetLastThrownObject (NULL);                                         \
        ReturnToContext(pFrame);                                                \
        CONTRACT_VIOLATION(ThrowsViolation);                                    \
        TESTHOOKCALL(LeftAppDomain(adid.m_dwId));                               \
        PAL_CPP_THROW(Exception*,ex);                                           \
    }

    CANNOTTHROWCOMPLUSEXCEPTION(); //no exceptions until returning to context

    Frame* pUnloadBoundary = GetUnloadBoundaryFrame();

    LOG((LF_EH, LL_INFO100, "Exception crossed into another context.  Rethrowing in new context.\n"));


    // will throw a kAppDomainUnloadedException if necessary
    if (ShouldChangeAbortToUnload(pFrame, pUnloadBoundary))
        RETURNANDTHROWNEWEXCEPTION(pEx,EEResourceException,(kAppDomainUnloadedException, L"Remoting_AppDomainUnloaded_ThreadUnwound"));

    // Can't marshal return value from unloaded appdomain.  Haven't
    // yet hit the boundary.  Throw a generic exception instead.
    // ThreadAbort is more consistent with what goes on elsewhere --
    // the AppDomainUnloaded is only introduced at the top-most boundary.
    //

    if (GetDomain() == SystemDomain::AppDomainBeingUnloaded()
        && GetThread()!=SystemDomain::System()->GetUnloadingThread() &&
            GetThread()!=GCHeap::GetGCHeap()->GetFinalizerThread())
    {
        if (pUnloadBoundary)
            RETURNANDTHROWNEWEXCEPTION(pEx,EEException,(kThreadAbortException))            
        else
            RETURNANDTHROWNEWEXCEPTION(pEx,EEResourceException,(kAppDomainUnloadedException, L"Remoting_AppDomainUnloaded_ThreadUnwound"));            
    }

    if (IsRudeAbort())
        RETURNANDTHROWNEWEXCEPTION(pEx,EEException,(kThreadAbortException));            


    // There are a few classes that have the potential to create
    // infinite loops if we try to marshal them.  For ThreadAbort,
    // ExecutionEngine, StackOverflow, and
    // OutOfMemory, throw a new exception of the same type.
    //
    //

    if(pEx && !pEx->IsDomainBound())
    {
        RETURNANDRETHROW(pEx);
    }
#undef RETURNANDTHROWNEWEXCEPTION
#undef RETURNANDRETHROW
}

Thread::RaiseCrossContextResult
Thread::TryRaiseCrossContextException(Exception **ppExOrig,
                                      Exception *pException,
                                      RuntimeExceptionKind *pKind,
                                      OBJECTREF *ppThrowable,
                                      U1ARRAYREF *pOrBlob)
{
    CONTRACTL
    {
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    BOOL bIsClassInitException = (pException->GetHR() == COR_E_TYPEINITIALIZATION);
    RaiseCrossContextResult result = RaiseCrossContextSuccess;
    
    int alreadyMarshaling = StartedMarshalingException();
 
    EX_TRY
    {
        //just in case something throws
        //!!!should be released before any call to ReturnToContext !!!
        ExceptionHolder exception(*ppExOrig);
        
        if (IsExceptionOfType(kOutOfMemoryException, pException))
            *pKind = kOutOfMemoryException;
        else
        if (IsExceptionOfType(kThreadAbortException, pException))
            *pKind = kThreadAbortException;
        else
        if (IsExceptionOfType(kStackOverflowException, pException))
            *pKind = kStackOverflowException;
        else
        if (IsExceptionOfType(kExecutionEngineException, pException))
            *pKind = kExecutionEngineException;
        else
        if (alreadyMarshaling)
        {
            *pKind = kExecutionEngineException;
        }
            
        // Serialize the exception
        if (*pKind == kLastException)
        {
            *ppThrowable = CLRException::GetThrowableFromException(exception);
            _ASSERTE(*ppThrowable != NULL);
            AppDomainHelper::MarshalObject(ppThrowable, pOrBlob);
        }
    }
    EX_CATCH
    {
        // We got a new Exception in original domain
        *ppExOrig = EXTRACT_EXCEPTION();
        // Got ClassInitException while marshaling ClassInitException. Class is unusable. Do not attempt anymore.
        if (bIsClassInitException && *ppExOrig && ((*ppExOrig)->GetHR() == COR_E_TYPEINITIALIZATION))
            result = RaiseCrossContextClassInit;
        else
            result = RaiseCrossContextRetry;
    }
    EX_END_CATCH(SwallowAllExceptions);

    FinishedMarshalingException();

    return result;
}

// * pEx should be deleted before popping the frame, except for one case
// * SafeSetLastThrownObject is called after pEx is deleted
void DECLSPEC_NORETURN Thread::RaiseCrossContextException(Exception* pExOrig, ContextTransitionFrame* pFrame)
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
    }
    CONTRACTL_END;

    
    GCX_COOP();

    // These are the only data transfered between the appdomains
    // Make sure that anything added here is appdomain agile
    RuntimeExceptionKind kind = kLastException;
    RaiseCrossContextResult result = RaiseCrossContextSuccess;
    U1ARRAYREF orBlob = NULL;


#define MAX_RAISE_RETRY_COUNT  256

    DWORD dwRaiseRetryCount;
    for (dwRaiseRetryCount = 0; dwRaiseRetryCount < MAX_RAISE_RETRY_COUNT; dwRaiseRetryCount++)
    {
        // pEx is NULL means that the exception is CLRLastThrownObjectException
        CLRLastThrownObjectException lastThrown;
        Exception* pException = pExOrig?pExOrig:&lastThrown;

        // Set the current frame
        SetFrame(pFrame);
        RaiseCrossContextExceptionHelper(pExOrig, pFrame);
        _ASSERTE(pFrame->GetReturnContext());

        struct _gc {
            OBJECTREF pThrowable;
            U1ARRAYREF orBlob;
        } gc;
        ZeroMemory(&gc, sizeof(_gc));

        GCPROTECT_BEGIN(gc);
        result = Thread::TryRaiseCrossContextException(&pExOrig, pException, &kind, &gc.pThrowable, &gc.orBlob);
        GCPROTECT_END();

        if (result != RaiseCrossContextRetry)
        {
            orBlob = gc.orBlob;
            break;
        }
 
        // We got a new exception and therefore need to retry marshaling it.
        Thread *pThread = GetThread();
        BOOL fToggle = !pThread->PreemptiveGCDisabled();
        if (fToggle)
        {
            pThread->DisablePreemptiveGC();
        }
    }

    if (dwRaiseRetryCount == MAX_RAISE_RETRY_COUNT)
        kind = kExecutionEngineException;

    // Return to caller domain
    {
        GCX_FORBID();
        ReturnToContext(pFrame);
    }

    {
        struct _gc {
            OBJECTREF pMarshaledInit;
            OBJECTREF pMarshaledThrowable;
            U1ARRAYREF orBlob;
        } gc;
        ZeroMemory(&gc, sizeof(_gc));

        gc.orBlob = orBlob;

        // Create the appropriate exception
        GCPROTECT_BEGIN(gc);
        if (result == RaiseCrossContextClassInit)
        {
            HRESULT hr=S_OK;
            EX_TRY
            {
                WCHAR wszTemplate[30];
                IfFailThrow(UtilLoadStringRC(IDS_EE_NAME_UNKNOWN,
                                             wszTemplate,
                                             sizeof(wszTemplate)/sizeof(wszTemplate[0]),
                                             FALSE));
                
                CreateTypeInitializationExceptionObject(wszTemplate, NULL, &gc.pMarshaledInit, &gc.pMarshaledThrowable);
            }
            EX_CATCH
            {
                // Unable to create ClassInitException in caller domain
                hr=COR_E_TYPEINITIALIZATION;
            }
            EX_END_CATCH(RethrowTransientExceptions);
            IfFailThrow(hr);
        }
        else
        {
            switch (kind)
            {
            case kLastException:
                AppDomainHelper::UnmarshalObject(GetAppDomain(), &gc.orBlob, &gc.pMarshaledThrowable);
                break;
            case kOutOfMemoryException:
                COMPlusThrowOM();
                break;
            case kStackOverflowException:
                gc.pMarshaledThrowable = CLRException::GetPreallocatedStackOverflowException();
                break;
            case kExecutionEngineException:
                gc.pMarshaledThrowable = CLRException::GetPreallocatedExecutionEngineException();
                break;
            default:
                {
                    EEException ex(kind);
                    gc.pMarshaledThrowable = CLRException::GetThrowableFromException(&ex);
                }
            }
        }

        // ... and throw it.
        VALIDATEOBJECTREF(gc.pMarshaledThrowable);
        COMPlusThrow(gc.pMarshaledThrowable);

        GCPROTECT_END();
    }
}

struct FindADCallbackType {
    AppDomain *pSearchDomain;
    AppDomain *pPrevDomain;
    Frame *pFrame;
    int count;
    enum TargetTransition
        {fFirstTransitionInto, fMostRecentTransitionInto}
    fTargetTransition;

    FindADCallbackType() : pSearchDomain(NULL), pPrevDomain(NULL), pFrame(NULL)
    {
        LEAF_CONTRACT;
    }
};

StackWalkAction StackWalkCallback_FindAD(CrawlFrame* pCF, void* data)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    FindADCallbackType *pData = (FindADCallbackType *)data;

    Frame *pFrame = pCF->GetFrame();

    if (!pFrame)
        return SWA_CONTINUE;

    AppDomain *pReturnDomain = pFrame->GetReturnDomain();
    if (!pReturnDomain || pReturnDomain == pData->pPrevDomain)
        return SWA_CONTINUE;

    LOG((LF_APPDOMAIN, LL_INFO100, "StackWalkCallback_FindAD transition frame %8.8x into AD [%d]\n",
            pFrame, pReturnDomain->GetId().m_dwId));

    if (pData->pPrevDomain == pData->pSearchDomain) {
                ++pData->count;
        // this is a transition into the domain we are unloading, so save it in case it is the first
        pData->pFrame = pFrame;
        if (pData->fTargetTransition == FindADCallbackType::fMostRecentTransitionInto)
            return SWA_ABORT;   // only need to find last transition, so bail now
    }

    pData->pPrevDomain = pReturnDomain;
    return SWA_CONTINUE;
}

// This determines if a thread is running in the given domain at any point on the stack
Frame *Thread::IsRunningIn(AppDomain *pDomain, int *count)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    FindADCallbackType fct;
    fct.pSearchDomain = pDomain;
    if (!fct.pSearchDomain)
        return FALSE;

    // set prev to current so if are currently running in the target domain,
    // we will detect the transition
    fct.pPrevDomain = m_pDomain;
    fct.fTargetTransition = FindADCallbackType::fMostRecentTransitionInto;
    fct.count = 0;

    // when this returns, if there is a transition into the AD, it will be in pFirstFrame
    StackWalkAction res;
    res = StackWalkFrames(StackWalkCallback_FindAD, (void*) &fct, ALLOW_ASYNC_STACK_WALK);
    if (count)
        *count = fct.count;
    return fct.pFrame;
}

// This finds the very first frame on the stack where the thread transitioned into the given domain
Frame *Thread::GetFirstTransitionInto(AppDomain *pDomain, int *count)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    FindADCallbackType fct;
    fct.pSearchDomain = pDomain;
    // set prev to current so if are currently running in the target domain,
    // we will detect the transition
    fct.pPrevDomain = m_pDomain;
    fct.fTargetTransition = FindADCallbackType::fFirstTransitionInto;
    fct.count = 0;

    // when this returns, if there is a transition into the AD, it will be in pFirstFrame
    StackWalkAction res;
    res = StackWalkFrames(StackWalkCallback_FindAD, (void*) &fct, ALLOW_ASYNC_STACK_WALK);
    if (count)
        *count = fct.count;
    return fct.pFrame;
}

// Get outermost (oldest) AppDomain for this thread (not counting the default
// domain every one starts in).
AppDomain *Thread::GetInitialDomain()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    AppDomain *pDomain = m_pDomain;
    AppDomain *pPrevDomain = NULL;
    Frame *pFrame = GetFrame();
    while (pFrame != FRAME_TOP)
    {
        if (pFrame->GetVTablePtr() == ContextTransitionFrame::GetMethodFrameVPtr())
        {
            if (pPrevDomain)
                pDomain = pPrevDomain;
            pPrevDomain = pFrame->GetReturnDomain();
        }
        pFrame = pFrame->Next();
    }
    return pDomain;
}

#ifndef DACCESS_COMPILE
void  Thread::SetUnloadBoundaryFrame(Frame *pFrame)
{
    LEAF_CONTRACT;
    _ASSERTE((this == GetThread() && PreemptiveGCDisabled()) ||
             ThreadStore::HoldingThreadStore());
    if ((ULONG_PTR)m_pUnloadBoundaryFrame < (ULONG_PTR)pFrame)
    {
        m_pUnloadBoundaryFrame = pFrame;
    }
    if (pFrame == NULL)
    {
        ResetBeginAbortedForADUnload();
    }
}

void  Thread::ResetUnloadBoundaryFrame()
{
    LEAF_CONTRACT;
    _ASSERTE(this == GetThread() && PreemptiveGCDisabled());
    m_pUnloadBoundaryFrame=NULL;
    ResetBeginAbortedForADUnload();
}

#endif

BOOL Thread::ShouldChangeAbortToUnload(Frame *pFrame, Frame *pUnloadBoundaryFrame)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (! pUnloadBoundaryFrame)
        pUnloadBoundaryFrame = GetUnloadBoundaryFrame();

    // turn the abort request into an AD unloaded exception when go past the boundary.
    if (pFrame != pUnloadBoundaryFrame)
        return FALSE;

    // Only time have an unloadboundaryframe is when have specifically marked that thread for aborting
    // during unload processing, so this won't trigger UnloadedException if have simply thrown a ThreadAbort
    // past an AD transition frame
    _ASSERTE (IsAbortRequested());

    EEResetAbort(TAR_ADUnload);

    if (m_AbortType == EEPolicy::TA_None)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL Thread::HaveExtraWorkForFinalizer()
{
    WRAPPER_CONTRACT;

    return m_ThreadTasks
        || GCInterface::IsCacheCleanupRequired()
        || OverlappedDataObject::CleanupNeededFromGC()
        || ThreadpoolMgr::HaveTimerInfosToFlush()
        || ExecutionManager::IsCacheCleanupRequired()
        || Thread::CleanupNeededForFinalizedThread()
        || (m_DetachCount > 0)
        || CExecutionEngine::HasDetachedTlsInfo()
        || AppDomain::HasWorkForFinalizerThread()
        || SystemDomain::System()->RequireAppDomainCleanup();
}

void Thread::DoExtraWorkForFinalizer()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(GetThread() == this);
    _ASSERTE(this == GCHeap::GetGCHeap()->GetFinalizerThread());


    if (AppDomain::HasWorkForFinalizerThread())
    {
        AppDomain::ProcessUnloadDomainEventOnFinalizeThread();
    }

    if (RequireSyncBlockCleanup())
    {
        SyncBlockCache::GetSyncBlockCache()->CleanupSyncBlocks();
    }
    if (SystemDomain::System()->RequireAppDomainCleanup())
    {
        SystemDomain::System()->ProcessDelayedUnloadDomains();
    }

    CExecutionEngine::CleanupDetachedTlsInfo();

    if(m_DetachCount > 0 || Thread::CleanupNeededForFinalizedThread())
    {
        //Default size
        const int CLEANUP_SIZE = 100;

        //m_DetachCount is a public static variable on the Thread object
        //   It's possible other threads may enter DetachThread and change
        //   the value.  Cache off the current value so it doesn't change underneath us.
        int tmpDetachCount = m_DetachCount;
        
        int cleanupBufferSize = CLEANUP_SIZE;

        EEIntHashTable* pDLSItems[CLEANUP_SIZE];
        EEIntHashTable** ppItems = pDLSItems;

        NewArrayHolder<EEIntHashTable*> pAllocatedDLSItems(NULL);

        //If the number of detached threads is very large, we're going to try to
        //   allocate a larger array.  This allows us to cleanup more items in one pass.
        //   If the allocation fails we fall back on the stack array and are simply slower
        //   to process all the detached threads.        
        if(tmpDetachCount > CLEANUP_SIZE)
        {
            pAllocatedDLSItems = new (nothrow) EEIntHashTable*[tmpDetachCount];
            if(pAllocatedDLSItems)
            {            
                ppItems = pAllocatedDLSItems;
                cleanupBufferSize = tmpDetachCount;
            }
        }

        for(int i=0;i<cleanupBufferSize;i++)
        {
            ppItems[i] = NULL;
        }

        Thread::CleanupDetachedThreads( GCHeap::SUSPEND_OTHER, ppItems, cleanupBufferSize);

        for(int i=0;i<cleanupBufferSize;i++)
        {
            //Only non-null items are added to the buffer, if we find a null entry before the end of the
            //    buffer we've reached the last item.
            if(!ppItems[i])
                break;
            
            RemoveAllDomainLocalStores(&ppItems[i], false);            
        }
    }
    
    if (GCInterface::IsCacheCleanupRequired() && GCHeap::GetGCHeap()->GetCondemnedGeneration()>=1) {
        GCInterface::CleanupCache();
    }
    if(ExecutionManager::IsCacheCleanupRequired() && GCHeap::GetGCHeap()->GetCondemnedGeneration()>=1)
    {
        ExecutionManager::ClearCaches();
    }

    OverlappedDataObject::RequestCleanupFromGC();

    // If there were any TimerInfos waiting to be released, they'll get flushed now
    ThreadpoolMgr::FlushQueueOfTimerInfos();
    
}


// HELPERS FOR THE BASE OF A MANAGED THREAD, INCLUDING AD TRANSITION SUPPORT

// We have numerous places where we start up a managed thread.  This includes several places in the
// ThreadPool, the 'new Thread(...).Start()' case, and the Finalizer.  Try to factor the code so our
// base exception handling behavior is consistent across those places.  The resulting code is convoluted,
// but it's better than the prior situation of each thread being on a different plan.

// We need Middle & Outer methods for the usual problem of combining C++ & SEH.

/* The effect of all this is that we get:

                Base of thread -- OS unhandled exception filter that we hook

                SEH handler from DispatchOuter
                C++ handler from DispatchMiddle

   And if there is an AppDomain transition before we call back to user code, we additionally get:

                AppDomain transition -- contains its own handlers to terminate the first pass
                                        and marshal the exception.

                SEH handler from DispatchOuter
                C++ handler from DispatchMiddle

   Regardless of whether or not there is an AppDomain transition, we then have:

                User code that obviously can throw.

   So if we don't have an AD transition, or we take a fault before we successfully transition the
   AppDomain, then the base-most DispatchOuter/Middle will deal with the exception.  This may
   involve swallowing exceptions or it may involve Watson & debugger attach.  It will always
   involve notifications to any AppDomain.UnhandledException event listeners.

   But if we did transition the AppDomain, then any Watson, debugger attach and UnhandledException
   events will occur in that AppDomain in the initial first pass.  So we get a good debugging
   experience and we get notifications to the host that show which AppDomain is allowing exceptions
   to go unhandled (so perhaps it can be unloaded or otherwise dealt with).

   The trick is that if the exception goes unhandled at the process level, we would normally try
   to fire AppDomain events and display the faulting exception on the console from two more
   places.  These are the base-most DispatchOuter/Middle pair and the hook of the OS unhandled
   exception handler at the base of the thread.

   This is redundant and messy.  (There's no concern with getting a 2nd Watson because we only
   do one of these per process anyway).  The solution for the base-most DispatchOuter/Middle is
   to use the ManagedThreadCallState.flags to control whether the exception has already been
   dealt with or not.  These flags cause the ThreadBaseRedirectingFilter to either do normal
   "base of the thread" exception handling, or to ignore the exception because it has already
   been reported in the AppDomain we transitioned to.

   But turning off the reporting in the OS unhandled exception filter is harder.  We don't want
   to flip a bit on the Thread to disable this, unless we can be sure we are only disabling
   something we already reported, and that this thread will never recover from that situation and
   start executing code again.  Here's the normal nightmare scenario with SEH:

   1)  exception of type A is thrown
   2)  All the filters in the 1st pass say they don't want an A
   3)  The exception gets all the way out and is considered unhandled.  We report this "fact".
   4)  Imagine we then set a bit that says this thread shouldn't report unhandled exceptions.
   5)  The 2nd pass starts.
   6)  Inside a finally, someone throws an exception of type B.
   7)  A new 1st pass starts from the point of the throw, with a type B.
   8)  Now a filter says "Yes, I will swallow a B."
   9)  We no longer have an unhandled exception, and execution continues merrily.

   This is an unavoidable consequence of the 2-pass model.  If you report unhandled exceptions
   in the 1st pass (for good debugging), you might find that this was premature and you don't
   have an unhandled exception when you get to the 2nd pass.

   But it would be truly awful if in step 4 we set a bit that says we should suppress normal
   notifications and reporting on this thread, believing that the process will terminate.

   The solution is to recognize that the base OS unhandled exception filter runs in two modes.
   In the first mode, it operates as today and serves as our backstop.  In the second mode
   it is fully redundant with the handlers pushed after the AppDomain transition, which are
   completely containing the exception to the AD that it occurred in (for purposes of reporting).
   So we just need a flag on the thread that says whether or not that set of handlers are pushed
   and functioning.  That flag enables / disables the base exception reporting and is called
   TSNC_AppDomainContainUnhandled

*/


enum ManagedThreadCallStateFlags
{
    MTCSF_NormalBase,
    MTCSF_ContainToAppDomain,
    MTCSF_SuppressDuplicate,
};

struct ManagedThreadCallState
{
    ADID                         pAppDomainId;
    AppDomain*                   pUnsafeAppDomain;
    BOOL                         bDomainIsAsID;

    Context::ADCallBackFcnType   pTarget;
    LPVOID                       args;
    UnhandledExceptionLocation   filterType;
    ManagedThreadCallStateFlags  flags;
    BOOL IsAppDomainEqual(AppDomain* pApp)
    {
        LEAF_CONTRACT;
        return bDomainIsAsID?(pApp->GetId()==pAppDomainId):(pUnsafeAppDomain==pApp);
    }
    ManagedThreadCallState(ADID AppDomainId,Context::ADCallBackFcnType Target,LPVOID Args,
                        UnhandledExceptionLocation   FilterType, ManagedThreadCallStateFlags  Flags):
          pAppDomainId(AppDomainId),
          pUnsafeAppDomain(NULL),
          bDomainIsAsID(TRUE),
          pTarget(Target),
          args(Args),
          filterType(FilterType),
          flags(Flags)
    {
        LEAF_CONTRACT;
    };
protected:
    ManagedThreadCallState(AppDomain* AppDomain,Context::ADCallBackFcnType Target,LPVOID Args,
                        UnhandledExceptionLocation   FilterType, ManagedThreadCallStateFlags  Flags):
          pAppDomainId(ADID(0)),
          pUnsafeAppDomain(AppDomain),
          bDomainIsAsID(FALSE),
          pTarget(Target),
          args(Args),
          filterType(FilterType),
          flags(Flags)
    {
        LEAF_CONTRACT;
    };
    void InitForFinalizer(AppDomain* AppDomain,Context::ADCallBackFcnType Target,LPVOID Args)
    {
        LEAF_CONTRACT;
        filterType=FinalizerThread;
        pUnsafeAppDomain=AppDomain;
        pTarget=Target;
        args=Args;
    };

    friend void ManagedThreadBase_NoADTransition(Context::ADCallBackFcnType pTarget,
                                             UnhandledExceptionLocation filterType);
    friend void ManagedThreadBase::FinalizerAppDomain(AppDomain* pAppDomain,
                                           Context::ADCallBackFcnType pTarget,
                                           LPVOID args,
                                           ManagedThreadCallState *pTurnAround);
};

// The following static helpers are outside of the ManagedThreadBase struct because I
// don't want to change threads.h whenever I change the mechanism for how unhandled
// exceptions works.  The ManagedThreadBase struct is for the public exposure of the
// API only.

static void ManagedThreadBase_DispatchOuter(ManagedThreadCallState *pCallState);


// Here's the tricky part.  *IF and only IF* we took an AppDomain transition at the base, then we
// now want to push another complete set of handlers above us.  The reason is that we want the
// Watson report and the unhandled exception event to occur in the target AppDomain.  If we don't
// do this apparently redundant push of handlers, then we will marshal back the exception to the
// handlers on the Default AppDomain side.  This will erase all the important exception state by
// unwinding (catch and rethrow) in DoADCallBack.  And it will cause all unhandled exceptions to
// be reported from the Default AppDomain, which is annoying to any AppDomain.UnhandledException
// event listeners.
//
// So why not skip the handlers that are in the Default AppDomain and just push the ones after the
// transition?  Well, transitioning out of the Default AppDomain into the target AppDomain could
// fail.  We need handlers pushed for that case.  And in that case it's perfectly reasonable to
// report the problem as occurring in the Default AppDomain, which is what the base handlers will
// do.

static void ManagedThreadBase_DispatchInCorrectAD(LPVOID args)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    ManagedThreadCallState *pCallState = (ManagedThreadCallState *) args;

    // Ensure we aren't going to infinitely recurse.
    _ASSERTE(pCallState->IsAppDomainEqual(GetThread()->GetDomain()));

    // And then go round one more time.  But this time we want to ensure that the filter contains
    // any exceptions that aren't swallowed.  These must be treated as unhandled, rather than
    // propagated through the AppDomain boundary in search of an outer handler.  Otherwise we
    // will not get correct Watson behavior.
    pCallState->flags = MTCSF_ContainToAppDomain;
    ManagedThreadBase_DispatchOuter(pCallState);
    pCallState->flags = MTCSF_NormalBase;
}

static void ManagedThreadBase_DispatchInner(ManagedThreadCallState *pCallState)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;


    Thread *pThread = GetThread();

    if (!pCallState->IsAppDomainEqual(pThread->GetDomain()))
    {
        if (pCallState->bDomainIsAsID)
            pThread->DoADCallBack(pCallState->pAppDomainId,
                              ManagedThreadBase_DispatchInCorrectAD,
                              pCallState);
        else
            pThread->DoADCallBack(pCallState->pUnsafeAppDomain,
                              ManagedThreadBase_DispatchInCorrectAD,
                               pCallState, ADV_FINALIZER);
    }
    else
    {
        // Since no AppDomain transition is necessary, we need no additional handlers pushed
        // *AFTER* the transition.  We now have adequate handlers below us.  Go ahead and
        // dispatch the call.
        (*pCallState->pTarget) (pCallState->args);
    }


}

static void ManagedThreadBase_DispatchMiddle(ManagedThreadCallState *pCallState)
{
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_SO_TOLERANT;

    EX_TRY
    {
        BEGIN_SO_INTOLERANT_CODE(GetThread());

        // During an unwind, we have some cleanup:
        //
        // 1)  We should no longer suppress any unhandled exception reporting at the base
        //     of the thread, because any handler that contained the exception to the AppDomain
        //     where it occurred is now being removed from the stack.
        //
        // 2)  We need to unwind the Frame chain.  We cannot do it when we get to the __except clause
        //     because at this point we are in the 2nd phase and the stack has been popped.  Any
        //     stack crawling from another thread will see a frame chain in a popped region of stack.
        //     Nor can we pop it in a filter, since this would destroy all the stack-walking information
        //     we need to perform the 2nd pass.  So doing it in a C++ destructor will ensure it happens
        //     during the 2nd pass but before the stack is actually popped.
        class Cleanup
        {
            Frame  *m_pEntryFrame;
            Thread *m_pThread;
        public:
            Cleanup(Thread* pThread)
            {
                m_pThread = pThread;
                m_pEntryFrame = pThread->m_pFrame;
            }
            ~Cleanup()
            {
                GCX_COOP();
                m_pThread->SetFrame(m_pEntryFrame);
                m_pThread->ResetThreadStateNC(Thread::TSNC_AppDomainContainUnhandled);
            }
        };

        Cleanup cleanup(GetThread());

        ManagedThreadBase_DispatchInner(pCallState);

        END_SO_INTOLERANT_CODE;
    }
    EX_CATCH_CPP_ONLY
    {
        GCX_COOP();
        Exception *pException = GET_EXCEPTION();

        if (SwallowUnhandledExceptions() ||
            IsExceptionOfType(kThreadAbortException, pException) ||
            IsExceptionOfType(kAppDomainUnloadedException, pException))
        {
            // Do nothing to swallow the exception
        }
        else if (pException == Exception::GetSOException())
        {
            // We have not notified the debugger on this thread yet.
            // We can not send notification to debugger.
            // This case should rarely happen, only when a thread has very
            // small stack.
            EEPolicy::HandleSoftStackOverflow(TRUE);
        }
        else
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions);
}


// Dispatch to the appropriate filter, based on the active CallState.
static LONG ThreadBaseRedirectingFilter(PEXCEPTION_POINTERS pExceptionInfo, PVOID _pCallState)
{


    LONG (*ptrFilter) (PEXCEPTION_POINTERS, PVOID);

    ManagedThreadCallStateFlags flags = ((ManagedThreadCallState *) _pCallState)->flags;

    if (flags == MTCSF_SuppressDuplicate)
    {
        GetThread()->SetThreadStateNC(Thread::TSNC_AppDomainContainUnhandled);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    LONG ret = -1;
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return EXCEPTION_CONTINUE_SEARCH;);

    if (flags == MTCSF_ContainToAppDomain)
        ptrFilter = ThreadBaseExceptionAppDomainFilter;
    else
    {
        _ASSERTE(flags == MTCSF_NormalBase);
        ptrFilter = ThreadBaseExceptionSwallowingFilter;
    }

    ret = (*ptrFilter) (pExceptionInfo, _pCallState);

    // If we are containing unhandled exceptions to the AppDomain we transitioned into, and the
    // exception is coming out, then this exception is going unhandled.  We have already done
    // Watson and managed events, so suppress all filters below us.  Otherwise we are swallowing
    // it and returning out of the AppDomain.
    if (flags == MTCSF_ContainToAppDomain)
        ((ManagedThreadCallState *) _pCallState)->flags = (ret == EXCEPTION_CONTINUE_SEARCH
                                                           ? MTCSF_SuppressDuplicate
                                                           : MTCSF_NormalBase);

    END_SO_INTOLERANT_CODE;
    return ret;
}

static void ManagedThreadBase_DispatchOuter(ManagedThreadCallState *pCallState)
{
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    // HasStarted() must have already been performed by our caller
    _ASSERTE(GetThread() != NULL);

    Thread *pThread = GetThread();

    PAL_TRY
    {
        ManagedThreadBase_DispatchMiddle(pCallState);
    }
    PAL_EXCEPT_FILTER(ThreadBaseRedirectingFilter, pCallState)
    {
        // Note: one of our C++ exceptions will never reach this filter because they're always caught by
        // the EX_CATCH in ManagedThreadBase_DispatchMiddle().
        //
        // If eCLRDeterminedPolicy, we only swallow for TA, RTA, and ADU exception.
        // For eHostDeterminedPolicy, we will swallow all the managed exception.

        // Fortunately, ThreadAbortExceptions are always
        if (pThread->IsAbortRequested())
            pThread->EEResetAbort(Thread::TAR_Thread);
    }
    PAL_ENDTRY;
}


// For the implementation, there are three variants of work possible:

// 1.  Establish the base of a managed thread, and switch to the correct AppDomain.
static void ManagedThreadBase_FullTransitionWithAD(ADID pAppDomain,
                                                   Context::ADCallBackFcnType pTarget,
                                                   LPVOID args,
                                                   UnhandledExceptionLocation filterType)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    ManagedThreadCallState CallState(pAppDomain, pTarget, args, filterType, MTCSF_NormalBase);
    ManagedThreadBase_DispatchOuter(&CallState);
}

// 2.  Establish the base of a managed thread, but the AppDomain transition must be
//     deferred until later.
void ManagedThreadBase_NoADTransition(Context::ADCallBackFcnType pTarget,
                                             UnhandledExceptionLocation filterType)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    AppDomain *pAppDomain = GetAppDomain();

    ManagedThreadCallState CallState(pAppDomain, pTarget, NULL, filterType, MTCSF_NormalBase);

    // self-describing, to create a pTurnAround data for eventual delivery to a subsequent AppDomain
    // transition.
    CallState.args = &CallState;

    ManagedThreadBase_DispatchOuter(&CallState);
}



// And here are the various exposed entrypoints for base thread behavior

// The 'new Thread(...).Start()' case from COMSynchronizable kickoff thread worker
void ManagedThreadBase::KickOff(ADID pAppDomain, Context::ADCallBackFcnType pTarget, LPVOID args)
{
    WRAPPER_CONTRACT;
    ManagedThreadBase_FullTransitionWithAD(pAppDomain, pTarget, args, ManagedThread);
}

// The IOCompletion, QueueUserWorkItem, AddTimer, RegisterWaitForSingleObject cases in the ThreadPool
void ManagedThreadBase::ThreadPool(ADID pAppDomain, Context::ADCallBackFcnType pTarget, LPVOID args)
{
    WRAPPER_CONTRACT;
    ManagedThreadBase_FullTransitionWithAD(pAppDomain, pTarget, args, ThreadPoolThread);
}

// The Finalizer thread establishes exception handling at its base, but defers all the AppDomain
// transitions.
void ManagedThreadBase::FinalizerBase(Context::ADCallBackFcnType pTarget)
{
    WRAPPER_CONTRACT;
    ManagedThreadBase_NoADTransition(pTarget, FinalizerThread);
}

void ManagedThreadBase::FinalizerAppDomain(AppDomain *pAppDomain,
                                           Context::ADCallBackFcnType pTarget,
                                           LPVOID args,
                                           ManagedThreadCallState *pTurnAround)
{
    WRAPPER_CONTRACT;
    pTurnAround->InitForFinalizer(pAppDomain,pTarget,args);
    _ASSERTE(pTurnAround->flags == MTCSF_NormalBase);
    ManagedThreadBase_DispatchInner(pTurnAround);
}

//+----------------------------------------------------------------------------
//
//  Method:     Thread::GetStaticFieldAddress   private
//
//  Synopsis:   Get the address of the field relative to the current thread.
//              If an address has not been assigned yet then create one.
//+----------------------------------------------------------------------------
LPVOID Thread::GetStaticFieldAddress(FieldDesc *pFD)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    STATIC_DATA *pData;
    LPVOID pvAddress = NULL;
    // for static field the MethodTable is exact even for generic classes
    MethodTable *pMT = pFD->GetEnclosingMethodTable();
    BOOL fIsShared = pMT->IsDomainNeutral();
    DWORD dwClassOffset = pMT->GetThreadStaticsOffset();
    DWORD currElem = 0;
    Thread *pThread = GetThread();
    AppDomain* pDomain = pThread->GetDomain();

    if (dwClassOffset == (DWORD)-1)
    {
        dwClassOffset = pMT->AllocateThreadStaticsOffset();
    }

    // NOTE: if you change this method, you must also change
    // GetStaticFieldAddrNoCreate below.

    _ASSERTE(NULL != pThread);
    if(!fIsShared)
    {
        pData = pThread->GetUnsharedStaticData();
    }
    else
    {
        pData = pThread->GetSharedStaticData();
    }

    if(NULL != pData)
    {
        currElem = pData->cElem;
    }

    // Check whether we have allocated space for storing a pointer to
    // this class' thread static store
    if(dwClassOffset >= currElem)
    {
        // Allocate space for storing pointers
        DWORD dwNewElem = (currElem == 0 ? 4 : currElem*2);

        // Ensure that we grow to a size beyond the index we intend to use
        while (dwNewElem <= dwClassOffset)
        {
            dwNewElem = dwNewElem*2;
        }

        STATIC_DATA *pNew = (STATIC_DATA *)new BYTE[sizeof(STATIC_DATA) + dwNewElem*sizeof(LPVOID)];

        // Set the new count.
        pNew->cElem = dwNewElem;

            if(NULL != pData)
            {
                // Copy the old data into the new data
            memcpy(&pNew->dataPtr[0], &pData->dataPtr[0], currElem*sizeof(LPVOID));

            // Delete the old data
            delete [] (BYTE*)pData;
        }

        // Zero init any new elements.
        ZeroMemory(&pNew->dataPtr[currElem], (dwNewElem-currElem)*sizeof(LPVOID));

            // Update the locals
            pData = pNew;

            // Reset the pointers in the thread object to point to the
            // new memory
            if(!fIsShared)
            {
                pThread->SetStaticData(pDomain, NULL, pData);
            }
            else
            {
                pThread->SetStaticData(pDomain, pData, NULL);
            }
        }

    _ASSERTE(NULL != pData);

    // Check whether we have to allocate space for
    // the thread local statics of this class
    if(NULL == pData->dataPtr[dwClassOffset])
    {
        DWORD dwSize = pMT->GetThreadStaticsSize();

        // Allocate memory for thread static fields with extra space in front for the class owning the storage.
        // We stash the class at the front of the allocated storage so that we can use
        // it to interpret the data on unload in DeleteThreadStaticClassData.
        LPBYTE pFields = new BYTE[sizeof(MethodTable*)+dwSize];

        *(MethodTable**)pFields = pMT;
        pFields += sizeof(MethodTable*);

            // Initialize the memory allocated for the fields
        ZeroMemory(pFields, dwSize);

        pData->dataPtr[dwClassOffset] = pFields;
        }

    _ASSERTE(NULL != pData->dataPtr[dwClassOffset]);

        // We have allocated static storage for this data
        // Just return the address by getting the offset into the data
    pvAddress = (LPVOID)((LPBYTE)pData->dataPtr[dwClassOffset] + pFD->GetOffset());

        // For object and value class fields we have to allocate storage in the
        // __StaticContainer class in the managed heap. Instead of pvAddress being
        // the actual address of the static, for such objects it holds the slot index
        // to the location in the __StaticContainer member.
        if(pFD->IsObjRef() || pFD->IsByValue())
        {
            // _ASSERTE(FALSE);
            // in this case *pvAddress == bucket|index
            int *pSlot = (int*)pvAddress;
            pvAddress = NULL;

            pThread->GetStaticFieldAddressSpecial(pFD, pMT, pSlot, &pvAddress, pDomain);

            if (pFD->IsByValue())
            {
                _ASSERTE(pvAddress != NULL);
                pvAddress = (*((OBJECTREF*)pvAddress))->GetData();
            }

            // ************************************************
            // ************** WARNING *************************
            // Do not provoke GC from here to the point JIT gets
            // pvAddress back
            // ************************************************
            _ASSERTE(*pSlot > 0);
        }

    _ASSERTE(NULL != pvAddress);

    return pvAddress;
}

#endif // #ifndef DACCESS_COMPILE

//+----------------------------------------------------------------------------
//
//  Method:     Thread::GetStaticFieldAddrNoCreate   private
//
//  Synopsis:   Get the address of the field relative to the thread.
//              If an address has not been assigned, return NULL.
//              No creating is allowed.
//+----------------------------------------------------------------------------
TADDR Thread::GetStaticFieldAddrNoCreate(FieldDesc *pFD)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    STATIC_DATA *pData;
    TADDR pvAddress = 0;
    // for static field the MethodTable is exact even for generic classes
    MethodTable *pMT = pFD->GetEnclosingMethodTable();
    BOOL fIsShared = pMT->IsDomainNeutral();

    DWORD dwClassOffset = pMT->GetThreadStaticsOffset();
    if (dwClassOffset == (DWORD)-1)
        return NULL;

    // Note: this function operates on 'this' Thread, not the
    // 'current' thread.

    // NOTE: if you change this method, you must also change
    // GetStaticFieldAddress above.

    if (!fIsShared)
        pData = GetUnsharedStaticData();
    else
        pData = GetSharedStaticData();


    if (NULL != pData)
    {
        // Check whether we have allocated space for storing a pointer
        // to this class' thread static store.
        if ((dwClassOffset < pData->cElem) && (NULL != pData->dataPtr[dwClassOffset]))
        {
            // We have allocated static storage for this data.  Just
            // return the address by getting the offset into the data.
            pvAddress = (TADDR)pData->dataPtr[dwClassOffset] + pFD->GetOffset();

            // For object and value class fields we have to allocate
            // storage in the __StaticContainer class in the managed
            // heap. If its not already allocated, return NULL
            // instead.
            if (pFD->IsObjRef() || pFD->IsByValue())
            {
                // if *pvAddress == NULL, it means we have to reserve a slot
                // for this static in the managed array.
                // (Slot #0 is never assigned to any static to support this.)
                if (!*PTR_int(pvAddress))
                {
                    pvAddress = 0;
                    LOG((LF_SYNC, LL_ALWAYS, "dbgr: pvAddress = NULL"));
                }
                else
                {
                    pvAddress = CalculateAddressForManagedStatic(*PTR_int(pvAddress));
                    LOG((LF_SYNC, LL_ALWAYS, "dbgr: pvAddress = %lx", pvAddress));
                    if (pFD->IsByValue())
                    {
                        _ASSERTE(pvAddress != NULL);
                        pvAddress = (TADDR)
                            (*PTR_UNCHECKED_OBJECTREF(pvAddress))->GetData();
                    }
                }
            }
        }
    }

    return pvAddress;
}

#ifndef DACCESS_COMPILE



#endif // #ifndef DACCESS_COMPILE

// This is used for thread relative statics that are object refs
// These are stored in a structure in the managed thread. The first
// time over an index and a bucket are determined and subsequently
// remembered in the location for the field in the per-thread-per-class
// data structure.
// Here we map back from the index to the address of the object ref.
TADDR Thread::CalculateAddressForManagedStatic(int slot)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

     _ASSERTE(slot > 0);

    PTRARRAYREF bucketArrayRef = NULL;

    bucketArrayRef = ((THREADBASEREF)GetExposedObjectRaw())->GetThreadStaticsBucketsHolder();

    PTRARRAYREF bucketRef = (PTRARRAYREF) bucketArrayRef->GetAt(slot/THREAD_STATICS_BUCKET_SIZE);
    PTR_UNCHECKED_OBJECTREF pObjRef   = (PTR_UNCHECKED_OBJECTREF((TADDR)bucketRef->GetDataPtr())) + slot%THREAD_STATICS_BUCKET_SIZE;
    return PTR_TO_TADDR(pObjRef);
}

#ifndef DACCESS_COMPILE

//
// NotifyFrameChainOfExceptionUnwind
// -----------------------------------------------------------
// This method will walk the Frame chain from pStartFrame to
// the last frame that is below pvLimitSP and will call each
// frame's ExceptionUnwind method.  It will return the first
// Frame that is above pvLimitSP.
//
Frame* Thread::NotifyFrameChainOfExceptionUnwind(Frame* pStartFrame, LPVOID pvLimitSP)
{
    CONTRACTL
    {
        NOTHROW;
        DISABLED(GC_TRIGGERS);  // due to UnwindFrameChain from NOTRIGGER areas
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pStartFrame));
        PRECONDITION(CheckPointer(pvLimitSP));
    }
    CONTRACTL_END;

    Frame* pFrame;

#ifdef _DEBUG
    //
    // assert that the specified Thread's Frame chain actually
    // contains the start Frame.
    //
    pFrame = m_pFrame;
    while ((pFrame != pStartFrame) &&
           (pFrame != FRAME_TOP))
    {
        pFrame = pFrame->Next();
    }
    CONSISTENCY_CHECK_MSG(pFrame == pStartFrame, "pStartFrame is not on pThread's Frame chain!");
#endif // _DEBUG

    pFrame = pStartFrame;
    while (pFrame < pvLimitSP)
    {
        CONSISTENCY_CHECK(pFrame != 0);
        CONSISTENCY_CHECK(((LPVOID)pFrame) > GetCurrentSP());
        pFrame->ExceptionUnwind();
        pFrame = pFrame->Next();
    }

    // return the frame after the last one notified of the unwind
    return pFrame;
}


// This is called during AD unload to set the bit corresponding to the managed
// thread static slot that has been freed. This way we can reassign the freed
// entry when some other domain needs one.
void Thread::FreeThreadStaticSlot(int slot)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Slot #0 is never assigned
    _ASSERTE(slot > 0);
    I4ARRAYREF bitArrayRef = NULL;
    THREADBASEREF threadRef = (THREADBASEREF)GetExposedObjectRaw();
    _ASSERTE(threadRef != NULL);

    bitArrayRef = threadRef->GetThreadStaticsBits();
    _ASSERTE(bitArrayRef != NULL);

    // Get to the correct set of 32 bits
    int *p32Bits = (slot/32 + (int*) bitArrayRef->GetDataPtr());
    // Turn on the bit corresponding to this slot
    *p32Bits |= (1<<slot%32);
}

//+----------------------------------------------------------------------------
//
//  Method:     Thread::GetStaticFieldAddressSpecial private
//
//  Synopsis:   Allocate an entry in the __StaticContainer class in the
//              managed heap for static objects and value classes
//+----------------------------------------------------------------------------

// NOTE: At one point we used to allocate these in the long lived handle table
// which is per-appdomain. However, that causes them to get rooted and not
// cleaned up until the appdomain gets unloaded. This is not very desirable
// since a thread static object may hold a reference to the thread itself
// causing a whole lot of garbage to float around.
// Now (2/13/01) these are allocated from a managed structure rooted in each
// managed thread.

void Thread::GetStaticFieldAddressSpecial(
    FieldDesc *pFD, MethodTable *pMT, int *pSlot, LPVOID *ppvAddress,  AppDomain* pDomain)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    OBJECTREF *pObjRef = NULL;
    BOOL bNewSlot = (*pSlot == 0);
    if (bNewSlot)
    {
        // ! this line will trigger a GC, don't move it down
        // ! without protecting the args[] and other OBJECTREFS
        MethodDescCallSite reserveSlot(METHOD__THREAD__RESERVE_SLOT);

        // We need to assign a location for this static field.
        // Call the managed helper
        ARG_SLOT args[1] = {
            ObjToArgSlot(GetExposedObject())
        };

        _ASSERTE(args[0] != 0);

        *pSlot = reserveSlot.Call_RetI4(args);

        _ASSERTE(*pSlot>0);

        // to a boxed version of the value class.  This allows the standard GC
        // algorithm to take care of internal pointers in the value class.
        if (pFD->IsByValue())
        {
            // Extract the type of the field
            TypeHandle  th = pFD->GetFieldTypeHandleThrowing();

            OBJECTHANDLE oh;
            OBJECTREF obj = pMT->AllocateStaticBox(th.GetMethodTable(), &oh);


            if (oh != NULL)
            {
                // We need to track handles as we dont want to keep them around until
                // appdomain unloads
                PTR_STATIC_DATA_LIST pNode=NULL;
                LPVOID pNodePtr = NULL;

                this->m_pStaticDataHash->GetValue( (void *)pDomain, (void **)&pNodePtr );
                pNode = PTR_STATIC_DATA_LIST((TADDR)pNodePtr);

                _ASSERTE(pNode);

                ObjectHandleList::NodeType* pNewNode = new ObjectHandleList::NodeType(oh);
                pNode->m_PinnedThreadStatics.LinkHead(pNewNode);
            }

            pObjRef = (OBJECTREF*)CalculateAddressForManagedStatic(*pSlot);
            SetObjectReference( pObjRef, obj, GetAppDomain() );


        }
        else
        {
            pObjRef = (OBJECTREF*)CalculateAddressForManagedStatic(*pSlot);
        }
    }
    else
    {
        // If the field already has a location assigned we go through here
        pObjRef = (OBJECTREF*)CalculateAddressForManagedStatic(*pSlot);
    }
    *(ULONG_PTR *)ppvAddress =  (ULONG_PTR)pObjRef;
}



//+----------------------------------------------------------------------------
//
//  Method:     Thread::SetStaticData   private
//
//  Synopsis:   Finds or creates an entry in the list which has the same domain
//              as the one given. These entries have pointers to the thread
//              local static data in each appdomain.
//              This function is called in two situations
//              (1) We are switching domains and need to set the pointers
//              to the static storage for the domain we are entering.
//              (2) We are accessing thread local storage for the current
//              domain and we need to set the pointer to the static storage
//              that we have created.
//+----------------------------------------------------------------------------


STATIC_DATA_LIST *Thread::SetStaticData(AppDomain *pDomain, STATIC_DATA *pSharedData,
                                        STATIC_DATA *pUnsharedData)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;


    // we need to make sure no preemptive mode threads get in here. Otherwise an appdomain unload
    // cannot simply stop the EE and delete entries from this list, assuming there are no threads
    // touching these structures. If preemptive mode threads get in here, we will have to do a
    // deferred cleanup like for the codemanager.
    _ASSERTE (GetThread()->PreemptiveGCDisabled());

    STATIC_DATA_LIST *pNode=NULL;
    
    // First, check to make sure that we have a lock for the hash
    if(m_pSDHCrst == NULL){
        Crst *pCrst = new Crst("ThreadStaticDataHashTable", CrstSyncHashLock, CRST_UNSAFE_ANYMODE);
        if (FastInterlockCompareExchangePointer((void**)&m_pSDHCrst, pCrst, NULL) != NULL)
            delete pCrst;
    }

    CrstHolder sCrstHolder(m_pSDHCrst);

    // Then, check to make sure that we have a hash
    if( m_pStaticDataHash == NULL ) {
        m_pStaticDataHash = new EEPtrHashTable();
        // CheckThreadSafety is FALSE because we ensure that it is always safe
        // operate on the hash without taking a lock. The potential race is
        // between a DeleteThreadStaticData and SetStaticData or reading the
        // static data. Reading is fine (see the EEHashTable implementation).
        // Delete is safe since the two cases where it is called are:1) Appdomain unload
        // where we have done an EESuspend and 2) Thread exit (possibly due to a thread detach)
        // in which case we have taken the thread store lock.
        // The original linked list implementation was also lock-free. The change to
        // a hash table preserves that semantics, this comment is merely to document the rationale.

        m_pStaticDataHash->Init( 4, NULL, NULL, FALSE /* CheckThreadSafety */);
    }

    // We have a hash, check to see if this appDom has an entry
    else {
        m_pStaticDataHash->GetValue( (void *)pDomain, (void **)&pNode );
    }

    // If we haven't found the data, then we need to create it and remember it
    if( pNode == NULL ) {

        pNode = new STATIC_DATA_LIST();

        m_pSharedStaticData = pNode->m_pSharedStaticData = pSharedData;
        m_pUnsharedStaticData = pNode->m_pUnsharedStaticData = pUnsharedData;

        // Add to the hash
        m_pStaticDataHash->InsertValue( (void *)pDomain, (void *)pNode );
    }
    else {
            if(NULL == pSharedData)
            {
            m_pSharedStaticData = pNode->m_pSharedStaticData;
            }
            else
            {
            m_pSharedStaticData = pNode->m_pSharedStaticData = pSharedData;
            }

            if(NULL == pUnsharedData)
            {
            m_pUnsharedStaticData = pNode->m_pUnsharedStaticData;
            }
            else
            {
            m_pUnsharedStaticData = pNode->m_pUnsharedStaticData = pUnsharedData;
        }
    }

    return pNode;
}

// A version of SetStaticData that is guaranteed not to throw.  This can be used in
// ReturnToContext where we're sure we don't have to allocate.
STATIC_DATA_LIST *Thread::SafeSetStaticData(AppDomain *pDomain, STATIC_DATA *pSharedData,
                                        STATIC_DATA *pUnsharedData)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    STATIC_DATA_LIST *result = NULL;

    EX_TRY
    {
        result = SetStaticData(pDomain, pSharedData, pUnsharedData);
    }
    EX_CATCH
    {
        _ASSERTE(!"Thread::SafeSetStaticData() got an unexpected exception");
        result = NULL;
    }
    EX_END_CATCH(SwallowAllExceptions)

    return result;
}

//+----------------------------------------------------------------------------
//
//  Method:     Thread::DeleteThreadStaticData   private
//
//  Synopsis:   Delete the static data for each appdomain that this thread
//              visited.
//
//+----------------------------------------------------------------------------


void Thread::DeleteThreadStaticData()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    STATIC_DATA             *shared;
    STATIC_DATA             *unshared;

    shared = m_pSharedStaticData;
    unshared = m_pUnsharedStaticData;

    if( m_pStaticDataHash != NULL ) {
        CrstHolder sCrstHolder(m_pSDHCrst);
        EEHashTableIteration    iterator;
        STATIC_DATA_LIST        *pNode=NULL;

        memset(&iterator, 0x00, sizeof(EEHashTableIteration));

        m_pStaticDataHash->IterateStart( &iterator );
        while ( m_pStaticDataHash->IterateNext( &iterator ) ) {

            pNode = (STATIC_DATA_LIST*)m_pStaticDataHash->IterateGetValue( &iterator );

            if (pNode->m_pSharedStaticData == shared)
                shared = NULL;

            DeleteThreadStaticClassData((_STATIC_DATA*)pNode->m_pSharedStaticData);

            if (pNode->m_pUnsharedStaticData == unshared)
                unshared = NULL;

            DeleteThreadStaticClassData((_STATIC_DATA*)pNode->m_pUnsharedStaticData);

            // Destroy pinning handles associated with this appdomain
            ObjectHandleList::NodeType* pHandleNode;
            while ( (pHandleNode = pNode->m_PinnedThreadStatics.UnlinkHead()) != NULL)
            {
                DestroyPinningHandle(pHandleNode->data);
                delete pHandleNode;
            }


            delete pNode;
        }

        delete m_pStaticDataHash;
        m_pStaticDataHash = NULL;
    }
    delete m_pSDHCrst;
    m_pSDHCrst = NULL;

    delete shared;
    delete unshared;

    m_pSharedStaticData = NULL;
    m_pUnsharedStaticData = NULL;

    return;
}

//+----------------------------------------------------------------------------
//
//  Method:     Thread::DeleteThreadStaticData   protected
//
//  Synopsis:   Delete the static data for the given appdomain. This is called
//              when the appdomain unloads.
//
//+----------------------------------------------------------------------------

void Thread::DeleteThreadStaticData(AppDomain *pDomain)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if( m_pStaticDataHash == NULL ) return;

    STATIC_DATA_LIST *pNode=NULL;

    m_pStaticDataHash->GetValue( (void *)pDomain, (void **)&pNode );

    // If we find the data node, then delete the
    // contents and then remove from the hash
    if( pNode != NULL ) {

        // Delete the shared static data
        if(pNode->m_pSharedStaticData == m_pSharedStaticData)
            m_pSharedStaticData = NULL;

        DeleteThreadStaticClassData((_STATIC_DATA*)pNode->m_pSharedStaticData);

        // Delete the unshared static data
        if(pNode->m_pUnsharedStaticData == m_pUnsharedStaticData)
            m_pUnsharedStaticData = NULL;

        DeleteThreadStaticClassData((_STATIC_DATA*)pNode->m_pUnsharedStaticData);

        // Remove the entry from the hash
        CrstHolder sCrstHolder(m_pSDHCrst);
        m_pStaticDataHash->DeleteValue( (void *)pDomain );
        // Destroy pinning handles associated with this appdomain
        ObjectHandleList::NodeType* pHandleNode;
        while ((pHandleNode = pNode->m_PinnedThreadStatics.UnlinkHead()) != NULL)
        {
            DestroyPinningHandle(pHandleNode->data);
            delete pHandleNode;
        }


        // delete the given domain's entry
        delete pNode;
    }

}

// for valuetype and reference thread statics, we use the entry in the pData->dataPtr array for
// the class to hold an index of a slot to index into the managed array hung off the thread where
// such statics are rooted. We need to find those objects and null out their slots so that they
// will be collected properly on an unload.
void Thread::DeleteThreadStaticClassData(_STATIC_DATA* pData)
{
    WRAPPER_CONTRACT;

    if (pData == NULL)
        return;

    for(WORD i = 0; i < pData->cElem; i++)
    {
        void *dataPtr = (void *)pData->dataPtr[i];
        if (! dataPtr)
            continue;

        // if thread doesn't have an ExposedObject (eg. could be dead), then nothing to clean up.
        if (GetExposedObjectRaw() != NULL)
        {
            MethodTable *pMT = *(MethodTable**)(((BYTE*)dataPtr) - sizeof(MethodTable*));
            _ASSERTE(pMT->ValidateWithPossibleAV());

            // iterate through each static field and get it's address in the managed thread
            // structure and clear it out.

            // get a field iterator
            // <NICE> This is the only place we iterate static fields.  After
            // we clean this function up we should make ApproxFieldDescIterator work
            // over instance fields only </NICE>
            if (pMT->HasGenericsStaticsInfo())
            {
                FieldDesc *pFields = pMT->GetGenericsStaticFieldDescs();
                WORD cStatics = pMT->GetNumStaticFields();
                for (WORD fieldIndex = 0; fieldIndex < cStatics; fieldIndex++)
                {
                    CheckAndDeleteStaticField(&pFields[fieldIndex], dataPtr);
                }
            }
            else
            {
                ApproxFieldDescIterator fdIterator(pMT, ApproxFieldDescIterator::STATIC_FIELDS);
                FieldDesc *pFD;

                while ((pFD = fdIterator.Next()) != NULL)
                {
                    CheckAndDeleteStaticField(pFD, dataPtr);
                }
            }
        }
        delete ((BYTE*)(dataPtr) - sizeof(MethodTable*));
    }
    delete pData;
}

void Thread::CheckAndDeleteStaticField(FieldDesc* pFD, void *dataPtr)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (!(pFD->IsThreadStatic() && (pFD->IsObjRef() || pFD->IsByValue())))
        return;

    int *pSlot = (int*)((LPBYTE)dataPtr + pFD->GetOffset());
    if (*pSlot == 0)
        return;

    // clear out the object in the managed structure rooted in the thread.
    OBJECTREF *pObjRef = (OBJECTREF*)CalculateAddressForManagedStatic(*pSlot);
    _ASSERT(pObjRef != 0);
    SetObjectReferenceUnchecked( pObjRef, NULL);
    // set the bit corresponding to this slot
    FreeThreadStaticSlot(*pSlot);
}

void Thread::ResetCultureForDomain(ADID id)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    THREADBASEREF thread = (THREADBASEREF) GetExposedObjectRaw();

    if (thread == NULL)
        return;

    CULTUREINFOBASEREF userCulture = thread->GetCurrentUserCulture();
    if (userCulture != NULL)
    {
        if (!userCulture->IsSafeCrossDomain() && userCulture->GetCreatedDomainID() == id)
            thread->ResetCurrentUserCulture();
    }

    CULTUREINFOBASEREF UICulture = thread->GetCurrentUICulture();
    if (UICulture != NULL)
    {
        if (!UICulture->IsSafeCrossDomain() && UICulture->GetCreatedDomainID() == id)
            thread->ResetCurrentUICulture();
    }
}

ARG_SLOT Thread::CallPropertyGet(BinderMethodID id, OBJECTREF pObject)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    if (!pObject) {
        return 0;
    }

    ARG_SLOT retVal;

    GCPROTECT_BEGIN(pObject);
    MethodDescCallSite propGet(id, &pObject);

    // Set up the Stack.
    ARG_SLOT pNewArgs = ObjToArgSlot(pObject);

    // Make the actual call.
    retVal = propGet.Call_RetArgSlot(&pNewArgs);
    GCPROTECT_END();

    return retVal;
}

ARG_SLOT Thread::CallPropertySet(BinderMethodID id, OBJECTREF pObject, OBJECTREF pValue)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    if (!pObject) {
        return 0;
    }

    ARG_SLOT retVal;

    GCPROTECT_BEGIN(pObject);
    GCPROTECT_BEGIN(pValue);
    MethodDescCallSite propSet(id, &pObject);

    // Set up the Stack.
    ARG_SLOT pNewArgs[] = {
        ObjToArgSlot(pObject),
        ObjToArgSlot(pValue)
    };

    // Make the actual call.
    retVal = propSet.Call_RetArgSlot(pNewArgs);
    GCPROTECT_END();
    GCPROTECT_END();

    return retVal;
}

OBJECTREF Thread::GetCulture(BOOL bUICulture)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    FieldDesc *         pFD;

    _ASSERTE(PreemptiveGCDisabled());

    // This is the case when we're building mscorlib and haven't yet created
    // the system assembly.
    if (SystemDomain::System()->SystemAssembly()==NULL || g_fForbidEnterEE) {
        return NULL;
    }

    // Get the actual thread culture.
    OBJECTREF pCurThreadObject = GetExposedObject();
    _ASSERTE(pCurThreadObject!=NULL);

    THREADBASEREF pThreadBase = (THREADBASEREF)(pCurThreadObject);
    OBJECTREF pCurrentCulture = bUICulture ? pThreadBase->GetCurrentUICulture() : pThreadBase->GetCurrentUserCulture();

    if (pCurrentCulture==NULL) {
        GCPROTECT_BEGIN(pThreadBase);
        if (bUICulture) {
            // Call the Getter for the CurrentUICulture.  This will cause it to populate the field.
            ARG_SLOT retVal = CallPropertyGet(METHOD__THREAD__GET_UI_CULTURE,
                                           (OBJECTREF)pThreadBase);
            pCurrentCulture = ArgSlotToObj(retVal);
        } else {
            //This is  faster than calling the property, because this is what the call does anyway.
            pFD = g_Mscorlib.GetField(FIELD__CULTURE_INFO__CURRENT_CULTURE);
            _ASSERTE(pFD);
            pCurrentCulture = pFD->GetStaticOBJECTREF();
            _ASSERTE(pCurrentCulture!=NULL);
        }
        GCPROTECT_END();
    }

    return pCurrentCulture;
}



// copy culture name into szBuffer and return length
int Thread::GetParentCultureName(__out_ecount(length) LPWSTR szBuffer, int length, BOOL bUICulture)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // This is the case when we're building mscorlib and haven't yet created
    // the system assembly.
    if (SystemDomain::System()->SystemAssembly()==NULL) {
        WCHAR *tempName = L"en";
        INT32 tempLength = (INT32)wcslen(tempName);
        _ASSERTE(length>=tempLength);
        memcpy(szBuffer, tempName, tempLength*sizeof(WCHAR));
        return tempLength;
    }

    ARG_SLOT Result = 0;
    INT32 retVal=0;
    WCHAR *buffer=NULL;
    INT32 bufferLength=0;
    STRINGREF cultureName = NULL;

    GCX_COOP();

    struct _gc {
        OBJECTREF pCurrentCulture;
        OBJECTREF pParentCulture;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    GCPROTECT_BEGIN(gc);

    gc.pCurrentCulture = GetCulture(bUICulture);
    if (gc.pCurrentCulture != NULL) {
        Result = CallPropertyGet(METHOD__CULTURE_INFO__GET_PARENT, gc.pCurrentCulture);
    }

    if (Result) {
        gc.pParentCulture = (OBJECTREF)(ArgSlotToObj(Result));
        if (gc.pParentCulture != NULL)
        {
            Result = 0;
            Result = CallPropertyGet(METHOD__CULTURE_INFO__GET_NAME, gc.pParentCulture);
        }
    }

    GCPROTECT_END();

    if (Result==0) {
        return 0;
    }


    // Extract the data out of the String.
    cultureName = (STRINGREF)(ArgSlotToObj(Result));
    RefInterpretGetStringValuesDangerousForGC(cultureName, (WCHAR**)&buffer, &bufferLength);

    if (bufferLength<length) {
        memcpy(szBuffer, buffer, bufferLength * sizeof (WCHAR));
        szBuffer[bufferLength]=0;
        retVal = bufferLength;
    }

    return retVal;
}




// copy culture name into szBuffer and return length
int Thread::GetCultureName(__out_ecount(length) LPWSTR szBuffer, int length, BOOL bUICulture)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // This is the case when we're building mscorlib and haven't yet created
    // the system assembly.
    if (SystemDomain::System()->SystemAssembly()==NULL || g_fForbidEnterEE) {
        WCHAR *tempName = L"en-US";
        INT32 tempLength = (INT32)wcslen(tempName);
        _ASSERTE(length>=tempLength);
        memcpy(szBuffer, tempName, tempLength*sizeof(WCHAR));
        return tempLength;
    }

    ARG_SLOT Result = 0;
    INT32 retVal=0;
    WCHAR *buffer=NULL;
    INT32 bufferLength=0;
    STRINGREF cultureName = NULL;

    GCX_COOP ();

    OBJECTREF pCurrentCulture = NULL;
    GCPROTECT_BEGIN(pCurrentCulture)
    {
        pCurrentCulture = GetCulture(bUICulture);
        if (pCurrentCulture != NULL)
            Result = CallPropertyGet(METHOD__CULTURE_INFO__GET_NAME, pCurrentCulture);
    }
    GCPROTECT_END();

    if (Result==0) {
        return 0;
    }

    // Extract the data out of the String.
    cultureName = (STRINGREF)(ArgSlotToObj(Result));
    RefInterpretGetStringValuesDangerousForGC(cultureName, (WCHAR**)&buffer, &bufferLength);

    if (bufferLength<length) {
        memcpy(szBuffer, buffer, bufferLength * sizeof (WCHAR));
        szBuffer[bufferLength]=0;
        retVal = bufferLength;
    }

    return retVal;
}

LCID GetThreadCultureIdNoThrow(Thread *pThread, BOOL bUICulture)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    LCID Result = UICULTUREID_DONTCARE;

    EX_TRY
    {
        Result = pThread->GetCultureId(bUICulture);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH (SwallowAllExceptions);

    return (INT32)Result;
}

// Return a language identifier.
LCID Thread::GetCultureId(BOOL bUICulture)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // This is the case when we're building mscorlib and haven't yet created
    // the system assembly.
    if (SystemDomain::System()->SystemAssembly()==NULL || g_fForbidEnterEE) {
        return (LCID) UICULTUREID_DONTCARE;
    }

    LCID Result = (LCID) UICULTUREID_DONTCARE;

    GCX_COOP();

    OBJECTREF pCurrentCulture = NULL;
    GCPROTECT_BEGIN(pCurrentCulture)
    {
        pCurrentCulture = GetCulture(bUICulture);
        if (pCurrentCulture != NULL)
            Result = (LCID)CallPropertyGet(METHOD__CULTURE_INFO__GET_ID, pCurrentCulture);
    }
    GCPROTECT_END();

    return Result;
}

void Thread::SetCultureId(LCID lcid, BOOL bUICulture)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    GCX_COOP();

    OBJECTREF CultureObj = NULL;
    GCPROTECT_BEGIN(CultureObj)
    {
        // Convert the LCID into a CultureInfo.
        GetCultureInfoForLCID(lcid, &CultureObj);

        // Set the newly created culture as the thread's culture.
        SetCulture(&CultureObj, bUICulture);
    }
    GCPROTECT_END();
}

void Thread::SetCulture(OBJECTREF *CultureObj, BOOL bUICulture)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    // Retrieve the exposed thread object.
    OBJECTREF pCurThreadObject = GetExposedObject();
    _ASSERTE(pCurThreadObject!=NULL);

    // Set the culture property on the thread.
    THREADBASEREF pThreadBase = (THREADBASEREF)(pCurThreadObject);
    CallPropertySet(bUICulture
                    ? METHOD__THREAD__SET_UI_CULTURE
                    : METHOD__THREAD__SET_CULTURE,
                    (OBJECTREF)pThreadBase, *CultureObj);
}


// The DLS hash lock should already have been taken before this call
LocalDataStore *Thread::RemoveDomainLocalStore(ADID iAppDomainId)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HashDatum Data = NULL;
    if (m_pDLSHash) {
        if (m_pDLSHash->GetValue(iAppDomainId.m_dwId, &Data))
            m_pDLSHash->DeleteValue(iAppDomainId.m_dwId);
    }

    return (LocalDataStore*) Data;
}

// EX_TRY does alloca.  This helper function avoids calling alloca in a loop.
void Thread::RemoveDLSFromListNoThrowHelper(LocalDataStore* pLDS, int adid)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    EX_TRY
    {
        CONTRACT_VIOLATION(ThrowsViolation); // Work around SCAN bug
        ENTER_DOMAIN_ID (ADID(adid));
        RemoveDLSFromListNoThrow(pLDS);
        END_DOMAIN_TRANSITION;
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);
}

void Thread::RemoveAllDomainLocalStores(EEIntHashTable **ppDLSHash, bool fNeedsLock)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    //RemoveAllDomainLocalStores can be called in two ways:
    //  1) We are trying to remove the DLS hash for the current thread.  In
    //  this case, ppDLSHash points to GetThread()->m_pDLSHash.  We must
    //  acquire the lock in this case.
    //  2) We are trying to remove the DLS hash for another thread.  In this
    //  caes, ppDLSHash is a pointer into a local array in
    //  DoExtraWorkForFinalizer.  In this case, fNeedsLock is false.
    _ASSERTE((GetThread() == NULL && !fNeedsLock)
             || (GetThread()
                 && (fNeedsLock == (&GetThread()->m_pDLSHash == ppDLSHash))));
    
    EEIntHashTable * pDLSHash;

    DLSLockHolder dlsh(FALSE);
    while (TRUE)
    {
        if(fNeedsLock)
            dlsh.Acquire();

        pDLSHash = *ppDLSHash;
        *ppDLSHash = NULL;

        if(fNeedsLock)
            dlsh.Release();

        // Don't bother cleaning this up if we're detaching
        if (pDLSHash == NULL || g_fProcessDetach)
        {
            return;
        }

        EEHashTableIteration iter;
        // This is to silence the assertion in hash table.
        // The HashTable is not local.  There is no race to access the table.
        INDEBUG(dlsh.Acquire());
        pDLSHash->IterateStart(&iter);
        while (pDLSHash->IterateNext(&iter))
        {
            INDEBUG(dlsh.Release());
            LocalDataStore* pLDS = (LocalDataStore*) pDLSHash->IterateGetValue(&iter);
            _ASSERTE(pLDS);
            if (!g_fProcessDetach)
            {
                int adid = pDLSHash->IterateGetKey(&iter);
                RemoveDLSFromListNoThrowHelper(pLDS, adid);
            }
            delete pLDS;
            INDEBUG(dlsh.Acquire());
        }
        INDEBUG(dlsh.Release());

        delete pDLSHash;
        pDLSHash = NULL;
    }
}
// The DLS hash lock should already have been taken before this call
// use this function if you're cleaning up and don't really care if it fails

HRESULT Thread::RemoveDLSFromListNoThrow(LocalDataStore* pLDS)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    HRESULT hr=S_OK;
    BEGIN_EXCEPTION_GLUE(&hr,NULL);
    RemoveDLSFromList(pLDS);
    END_EXCEPTION_GLUE;
    return hr;
}

// The DLS hash lock should already have been taken before this call
void Thread::RemoveDLSFromList(LocalDataStore* pLDS)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (!g_fProcessDetach)
    {
        MethodDescCallSite removeThreadDLStore(METHOD__THREAD__REMOVE_DLS);

        ARG_SLOT args[1] = {
            ObjToArgSlot(pLDS->GetRawExposedObject())
        };
        removeThreadDLStore.Call(args);
    }
}

void Thread::SetName(__in_ecount(length) WCHAR* name, DWORD length)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (!g_fProcessDetach)
    {
        struct _gc {
            OBJECTREF threadObj;
            STRINGREF nameObj;
        } gc;
        ZeroMemory(&gc, sizeof(gc));

        GCPROTECT_BEGIN(gc);

        gc.threadObj = GetExposedObject();

        _ASSERTE(wcslen(name) < 100);
        gc.nameObj = AllocateString(length);
        memcpyNoGCRefs(gc.nameObj->GetBuffer(), name, length*sizeof(WCHAR));

        MethodDescCallSite setName(METHOD__THREAD__SET_NAME, &gc.threadObj);

        ARG_SLOT args[] = {
            ObjToArgSlot(gc.threadObj),
            ObjToArgSlot(gc.nameObj)
        };

        setName.Call(args);

        GCPROTECT_END();
    }
}

void Thread::SetHasPromotedBytes ()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    m_fPromoted = TRUE;

    _ASSERTE(GCHeap::IsGCInProgress()  && IsGCThread ());

    if (!m_fPreemptiveGCDisabled)
    {
        if (FRAME_TOP == GetFrame())
            m_fPromoted = FALSE;
    }
}

BOOL ThreadStore::HoldingThreadStore(Thread *pThread)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pThread)
    {
        return (pThread == s_pThreadStore->m_HoldingThread);
    }
    else
    {
        return (s_pThreadStore->m_holderthreadid.IsSameThread());
    }
}

void Thread::SetupFiberData()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

}

#ifdef _DEBUG

int Thread::MaxThreadRecord = 20;
int Thread::MaxStackDepth = 20;


void Thread::AddFiberInfo(DWORD type)
{
    LEAF_CONTRACT;
}


#endif // _DEBUG

HRESULT Thread::SwitchIn(HANDLE threadHandle)
{
    WRAPPER_CONTRACT;

    //can't do heap allocation in this method
    CantAllocHolder caHolder;

    // !!! Can not use the following line, since it uses an object which .dctor calls
    // !!! FLS_SETVALUE, and a new FLS is created after SwitchOut.
    // CANNOTTHROWCOMPLUSEXCEPTION();

    // Case Cookie to thread object and add to tls
#ifdef _DEBUG
    Thread *pThread = GetThread();
    // If this is hit, we need to understand.
    // Sometimes we see the assert but the memory does not match the assert.
    if (pThread) {
        DebugBreak();
    }
    //_ASSERT(GetThread() == NULL);
#endif

    if (GetThread() != NULL) {
        return HOST_E_INVALIDOPERATION;
    }

    CExecutionEngine::SwitchIn();

    // !!! no contract for this class.
    // !!! We have not switched in tls block.
    class EnsureTlsData
    {
    private:
        Thread *m_pThread;
        BOOL m_fNeedReset;
    public:
        EnsureTlsData(Thread* pThread){m_pThread = pThread; m_fNeedReset = TRUE;}
        ~EnsureTlsData()
        {
            if (m_fNeedReset)
            {
                UnsafeTlsSetValue(GetThreadTLSIndex(), NULL);
                UnsafeTlsSetValue(GetAppDomainTLSIndex(), NULL);
                CExecutionEngine::SwitchOut();
            }
        }
        void SuppressRelease()
        {
            m_fNeedReset = FALSE;
        }
    };

    EnsureTlsData ensure(this);

#ifdef _DEBUG
    if (CLRTaskHosted()) {
        IHostTask *pTask = NULL;
        _ASSERTE (CorHost2::GetHostTaskManager()->GetCurrentTask(&pTask) == S_OK &&
                  (pTask == GetHostTask() || GetHostTask() == NULL));

        if (pTask)
            pTask->Release();
    }
#endif

    if (UnsafeTlsSetValue(GetThreadTLSIndex(), this))
    {
        Thread *pThread = GetThread();
        if (!pThread)
            return E_OUTOFMEMORY;

        // !!! make sure that we switchin TLS so that FLS is available for Contract etc.

        // We redundantly keep the domain in its own TLS slot, for faster access from
        // stubs
        if (!UnsafeTlsSetValue(GetAppDomainTLSIndex(), m_pDomainAtTaskSwitch))
        {
            return E_OUTOFMEMORY;
        }

        CANNOTTHROWCOMPLUSEXCEPTION();

        if (CLRTaskHosted() && GetHostTask() == NULL) {
            // Reset has been called on this task.
            HRESULT hr = CorHost2::GetHostTaskManager()->GetCurrentTask(&m_pHostTask);

            _ASSERTE (hr == S_OK && m_pHostTask);

#ifdef _DEBUG
            AddFiberInfo(ThreadTrackInfo_Lifetime);
#endif


            m_OSThreadId = ::GetCurrentThreadId();

            ResetThreadState(TS_TaskReset);
        }


        if (m_pFiberData)
        {
            // only set the m_OSThreadId to bad food in Fiber mode
            m_OSThreadId = ::GetCurrentThreadId();
#ifdef PROFILING_SUPPORTED
            // If a profiler is present, then notify the profiler that a
            // thread has been created.
            if (CORProfilerTrackThreads())
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->ThreadAssignedToOSThread(
                    (ThreadID)this, m_OSThreadId);
            }
#endif // PROFILING_SUPPORTED
        }
        SetThreadHandle(threadHandle);



#ifdef _DEBUG
        // For debugging purpose, we save callstack during task switch.  On Win64, the callstack
        // is done within OS loader lock, and obtaining managed callstack may cause fiber switch.
        SetThreadStateNC(TSNC_InTaskSwitch);
        AddFiberInfo(ThreadTrackInfo_Schedule);
        ResetThreadStateNC(TSNC_InTaskSwitch);
#endif

        ensure.SuppressRelease();
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}

HRESULT Thread::SwitchOut()
{
    LEAF_CONTRACT;

    return E_NOTIMPL;
}

void Thread::InternalSwitchOut()
{
    INDEBUG( BOOL fNoTLS = (CExecutionEngine::CheckThreadStateNoCreate(0) == NULL));

    WRAPPER_CONTRACT;

    //can't do heap allocation in this method
    CantAllocHolder caHolder;
    
    // !!! Can not use the following line, since it uses an object which .dctor calls
    // !!! FLS_SETVALUE, and a new FLS is created after SwitchOut.
    // CANNOTTHROWCOMPLUSEXCEPTION();

    _ASSERTE(GetThread() == this);

    _ASSERTE (!fNoTLS ||
              (CExecutionEngine::CheckThreadStateNoCreate(0) == NULL));

    _ASSERTE(!HasNonHostLockCount());
    // Can not assert here.  If a mutex is orphaned, the thread will have ThreadAffinity.
    //_ASSERTE(!HasThreadAffinity());

    _ASSERTE (!fNoTLS ||
              (CExecutionEngine::CheckThreadStateNoCreate(0) == NULL));

#ifdef _DEBUG
    // For debugging purpose, we save callstack during task switch.  On Win64, the callstack
    // is done within OS loader lock, and obtaining managed callstack may cause fiber switch.
    SetThreadStateNC(TSNC_InTaskSwitch);
    AddFiberInfo(ThreadTrackInfo_Schedule);
    ResetThreadStateNC(TSNC_InTaskSwitch);
#endif

    _ASSERTE (!fNoTLS ||
              (CExecutionEngine::CheckThreadStateNoCreate(0) == NULL));

    m_pDomainAtTaskSwitch = GetAppDomain();

    if (m_pFiberData)
    {
        // only set the m_OSThreadId to bad food in Fiber mode
        m_OSThreadId = SWITCHED_OUT_FIBER_OSID;
#ifdef PROFILING_SUPPORTED
        // If a profiler is present, then notify the profiler that a
        // thread has been created.
        if (CORProfilerTrackThreads())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ThreadAssignedToOSThread(
                (ThreadID)this, m_OSThreadId);
        }
#endif // PROFILING_SUPPORTED
    }

    _ASSERTE (!fNoTLS ||
              (CExecutionEngine::CheckThreadStateNoCreate(0) == NULL));

    HANDLE hThread = GetThreadHandle();

    SetThreadHandle (SWITCHOUT_HANDLE_VALUE);
    while (m_dwThreadHandleBeingUsed > 0)
    {
        // Another thread is using the handle now.
#undef Sleep
        // We can not call __SwitchToThread since we can not go back to host.
        ::Sleep(10);
#define Sleep(a) Dont_Use_Sleep(a)
    }

    if (m_WeOwnThreadHandle && m_ThreadHandleForClose == INVALID_HANDLE_VALUE) {
        m_ThreadHandleForClose = hThread;
    }

    // The host is getting control of this thread, so if we were trying
    // to yield this thread, we can stop those attempts now.
    ResetThreadState(TS_YieldRequested);

    _ASSERTE (!fNoTLS ||
              (CExecutionEngine::CheckThreadStateNoCreate(0) == NULL));

    CExecutionEngine::SwitchOut();

    // We need to make sure that TLS are touched last here.
    // Contract uses TLS.
    UnsafeTlsSetValue(GetThreadTLSIndex(),NULL);
    UnsafeTlsSetValue(GetAppDomainTLSIndex(), NULL);

    _ASSERTE (!fNoTLS ||
              (CExecutionEngine::CheckThreadStateNoCreate(0) == NULL));
}

HRESULT Thread::GetMemStats (COR_GC_THREAD_STATS *pStats)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Get the allocation context which contains this counter in it.
    alloc_context *p = &m_alloc_context;
    pStats->PerThreadAllocation = p->alloc_bytes;
    if (GetHasPromotedBytes())
        pStats->Flags = COR_GC_THREAD_HAS_PROMOTED_BYTES;

    return S_OK;
}

void Thread::InternalReset(BOOL fFull)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE (this == GetThread());
    _ASSERTE (m_pFrame == FRAME_TOP || this == GCHeap::GetGCHeap()->GetFinalizerThread());

    FinishSOWork();

    INT32 nPriority = ThreadNative::PRIORITY_NORMAL;
    if (this == GCHeap::GetGCHeap()->GetFinalizerThread())
    {
        nPriority = ThreadNative::PRIORITY_HIGHEST;
    }

    {
        GCX_COOP();
        THREADBASEREF pObject = (THREADBASEREF)ObjectFromHandle(m_ExposedObject);
        if (pObject != NULL)
        {
            pObject->ResetCulture();
            pObject->ResetName();
            nPriority = pObject->GetPriority();
        }
    }

    if (fFull)
    {
        GCX_COOP();
        RemoveAllDomainLocalStores(&this->m_pDLSHash, true);

        // We nned to put this thread in COOPERATIVE GC first to solve race between AppDomain::Unload
        // and Thread::Reset.  AppDomain::Unload does a full GC to collect all roots in one AppDomain.
        // ThreadStaticData and managed array of objects in Managed Thread object are coupled.  If we
        // remove ThreadStaticData from unmanaged Thread object through DeleteThreadStaticData, the part
        // in Managed Thread object is not deleted until ResetThreadStatics, thus GC will not collect
        // that part.
        GCX_FORBID();
        DeleteThreadStaticData();
        ResetSecurityInfo();
        THREADBASEREF pObject = (THREADBASEREF)ObjectFromHandle(m_ExposedObject);
        if (pObject != NULL)
        {
            pObject->ResetThreadStatics();
        }
        m_alloc_context.alloc_bytes = 0;
        m_fPromoted = FALSE;
    }

    _ASSERTE (m_dwCriticalRegionCount == 0);
    m_dwCriticalRegionCount = 0;

    _ASSERTE (m_dwThreadAffinityCount == 0);
    m_dwThreadAffinityCount = 0;

    //m_MarshalAlloc.Collapse(NULL);

    _ASSERTE (m_dwNonHostLockCount == 0);

    _ASSERTE (m_dwDelayAbortCount == 0);
    m_dwDelayAbortCount = 0;

    if (IsAbortRequested()) {
        UnmarkThreadForAbort(TAR_ALL);
    }

    if (IsThreadPoolThread())
    {
        SetBackground(TRUE);
        if (nPriority != ThreadNative::PRIORITY_NORMAL)
        {
            SetThreadPriority(THREAD_PRIORITY_NORMAL);
        }
    }
    else if (this == GCHeap::GetGCHeap()->GetFinalizerThread())
    {
        SetBackground(TRUE);
        if (nPriority != ThreadNative::PRIORITY_HIGHEST)
        {
            SetThreadPriority(THREAD_PRIORITY_HIGHEST);
        }
    }
}

HRESULT Thread::Reset(BOOL fFull)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_ENTRY_POINT;

    if ( !g_fEEStarted)
        return(E_FAIL);

    _ASSERTE (IsSOTolerant());

    HRESULT hr = S_OK;

    BEGIN_SO_INTOLERANT_CODE_NOPROBE;

#ifdef _DEBUG
    if (CLRTaskHosted()) {
        // Reset is a heavy operation.  We will call into SQL for lock and memory operations.
        // The host better keeps IHostTask alive.
        _ASSERTE (GetCurrentHostTask() == GetHostTask());
    }

    _ASSERTE (GetThread() == this);
#endif

    if (GetThread() != this)
    {
        IfFailGo(E_UNEXPECTED);
    }

    if (HasThreadState(Thread::TS_YieldRequested))
    {
        ResetThreadState(Thread::TS_YieldRequested);
    }

    _ASSERTE (!PreemptiveGCDisabled());
    _ASSERTE (m_pFrame == FRAME_TOP);
    // A host should not recycle a CLRTask if the task is created by us through CreateNewThread.
    // We need to make Thread.Join work for this case.
    if ((m_StateNC & (TSNC_CLRCreatedThread | TSNC_CannotRecycle)) != 0)
        IfFailGo(E_UNEXPECTED);


#ifdef _DEBUG
    AddFiberInfo(ThreadTrackInfo_Lifetime);
#endif

    SetThreadState(TS_TaskReset);

    if (IsAbortRequested())
        EEResetAbort(Thread::TAR_ALL);
    InternalReset(fFull);

    if (PreemptiveGCDisabled()) {
        EnablePreemptiveGC();
    }

    _ASSERTE (m_pHostTask);

    ReleaseHostTask();


    InternalSwitchOut();
    m_OSThreadId = SWITCHED_OUT_FIBER_OSID;

ErrExit:

    END_SO_INTOLERANT_CODE_NOPROBE;


    return hr;
}

HRESULT Thread::ExitTask ()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_ENTRY_POINT;

    if ( !g_fEEStarted)
        return(E_FAIL);

    HRESULT hr = S_OK;

    _ASSERTE (IsSOTolerant());

    BEGIN_CONTRACT_VIOLATION(SOToleranceViolation);

    //OnThreadTerminate(FALSE);
    _ASSERTE (this == GetThread());
    _ASSERTE (!PreemptiveGCDisabled());

    // Can not assert the following.  SQL may call ExitTask after addref and abort a task.
    //_ASSERTE (m_UnmanagedRefCount == 0);
    if (this != GetThread())
        IfFailGo(HOST_E_INVALIDOPERATION);

    if (HasThreadState(Thread::TS_YieldRequested))
    {
        ResetThreadState(Thread::TS_YieldRequested);
    }

    hr = DetachThread(FALSE);
    m_OSThreadId = SWITCHED_OUT_FIBER_OSID;

ErrExit:;

    END_CONTRACT_VIOLATION;

    return hr;
}

HRESULT Thread::Abort ()
{
    CONTRACTL
    {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW;);
    EX_TRY
    {
        UserAbort(TAR_Thread, EEPolicy::TA_Safe, INFINITE, Thread::UAC_Host);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);
    END_SO_INTOLERANT_CODE;

    return S_OK;
}

HRESULT Thread::RudeAbort()
{
    CONTRACTL
    {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);

    EX_TRY
    {
        UserAbort(TAR_Thread, EEPolicy::TA_Rude, INFINITE, Thread::UAC_Host);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_SO_INTOLERANT_CODE;

    return S_OK;
}

HRESULT Thread::NeedsPriorityScheduling(BOOL *pbNeedsPriorityScheduling)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    *pbNeedsPriorityScheduling = (m_fPreemptiveGCDisabled ||
                                  (g_fEEStarted && this == GCHeap::GetGCHeap()->GetFinalizerThread()));
    return S_OK;
}

HRESULT Thread::YieldTask()
{
#undef Sleep
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        SO_TOLERANT;
    }
    CONTRACTL_END;

    //can't do heap allocation in this method
    CantAllocHolder caHolder;
    _ASSERTE(CLRTaskHosted());

    CONTRACT_VIOLATION(SOToleranceViolation);

    if (this == GetThread())
    {
        // We will suspend the target thread.  If YieldTask is called on the current thread,
        // we will suspend the current thread forever.
        return HOST_E_INVALIDOPERATION;
    }

    FAULT_FORBID();

    // This function has been called by the host, and the host needs not
    // be reentrant.  Therefore, no code running below this function can
    // cause calls back into the host.
    ForbidCallsIntoHostOnThisThreadHolder forbidCallsIntoHostOnThisThread(TRUE /*dummy*/);
    while (!forbidCallsIntoHostOnThisThread.Acquired())
    {
        // We can not call __SwitchToThread since we can not go back to host.
        ::Sleep(10);
        forbidCallsIntoHostOnThisThread.Acquire();
    }

    // So that the thread can yield when it tries to switch to coop gc.
    CounterHolder trtHolder(&g_TrapReturningThreads);

    // One worker on a thread only.
    while (TRUE)
    {
        LONG curValue = m_State;
        if ((curValue & TS_YieldRequested) != 0)
        {
            // The host has previously called YieldTask for this thread,
            // and the thread has not cleared the flag yet.
            return S_FALSE;
        }
        else if ((curValue & TS_Unstarted) != 0)
        {
            return S_OK;
        }

        CONSISTENCY_CHECK(sizeof(m_State) == sizeof(LONG));
        if (FastInterlockCompareExchange((LONG*)&m_State, curValue | TS_YieldRequested, curValue) == curValue)
        {
            break;
        }
    }

#ifdef PROFILING_SUPPORTED
    if (CORProfilerTrackSuspends())
    {
        // Must use wrapper to ensure forbid suspend count is incremented.
        // (See RuntimeTheadSuspendedWrapper in threads.h for more information.)
        RuntimeThreadSuspendedWrapper((ThreadID)this, (ThreadID)GetThread());
    }
#endif // PROFILING_SUPPORTED

    while (m_State & TS_YieldRequested)
    {
        BOOL fDone = FALSE;

        if (m_State & (TS_Dead | TS_Detached))
        {
            // The thread is dead, in other words, yielded forever.
            // Don't bother clearing TS_YieldRequested, as nobody
            // is going to look at it any more.
            break;
        }

        CounterHolder handleHolder(&m_dwThreadHandleBeingUsed);
        HANDLE hThread = GetThreadHandle();
        if (hThread == INVALID_HANDLE_VALUE)
        {
            // The thread is dead, in other words, yielded forever.
            // Don't bother clearing TS_YieldRequested, as nobody
            // is going to look at it any more.
            break;
        }
        else if (hThread == SWITCHOUT_HANDLE_VALUE)
        {
            // The thread is currently switched out.
            // This means that the host has control of the thread,
            // so we can stop our attempts to yield it.  Note that
            // TS_YieldRequested is cleared in InternalSwitchOut.  (If we
            // were to clear it here, we could race against another
            // thread that is running YieldTask.)
            break;
        }

        DWORD dwSuspendCount = ::SuspendThread(hThread);
        if ((int)dwSuspendCount >= 0) 
        {
            if (!EnsureThreadIsSuspended(hThread, this))
            {
                goto Retry;
            }

            if (hThread == GetThreadHandle())
            {
                if (m_dwForbidSuspendThread != 0)
                {
                    goto Retry;
                }
            }
            else
            {
                ::ResumeThread(hThread);
                break;
            }
        }
        else
        {
            // We can get here either SuspendThread fails
            // Or the fiber thread dies after this fiber switched out.
            
            if ((int)dwSuspendCount != -1)
            {
                 STRESS_LOG1(LF_SYNC, LL_INFO1000, "In Thread::YieldTask ::SuspendThread returned %x \n", dwSuspendCount);
            }
            if (GetThreadHandle() == SWITCHOUT_HANDLE_VALUE)
            {
                // The thread was switched out while we tried to suspend it.
                // This means that the host has control of the thread,
                // so we can stop our attempts to yield it.  Note that
                // TS_YieldRequested is cleared in InternalSwitchOut.  (If we
                // were to clear it here, we could race against another
                // thread that is running YieldTask.)
                break;
            }
            else {
                continue;
            }
        }

        if (!m_fPreemptiveGCDisabled)
        {
            ::ResumeThread(hThread);
            break;
        }

Retry:
        ::ResumeThread(hThread);
        if (fDone)
        {
            // We managed to redirect the thread, so we know that it will yield.
            // We can let the actual yielding happen asynchronously.
            break;
        }
        handleHolder.Release();
        ::Sleep(1);
    }

    if (CORProfilerTrackSuspends())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RuntimeThreadResumed((ThreadID)this, (ThreadID)GetThread());
    }

    return S_OK;
#define Sleep(a) Dont_Use_Sleep(a)
}

HRESULT Thread::LocksHeld(SIZE_T *pLockCount)
{
    LEAF_CONTRACT;

    *pLockCount = m_dwLockCount + m_dwCriticalRegionCount;
    return S_OK;
}

HRESULT Thread::SetTaskIdentifier(TASKID asked)
{
    LEAF_CONTRACT;

    m_TaskId = asked;
    return S_OK;
}


// We release m_pHostTask during ICLRTask::Reset and ICLRTask::ExitTask call.
// This function allows us to synchronize obtaining m_pHostTask with Thread reset or exit.
IHostTask* Thread::GetHostTaskWithAddRef()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CounterIncrease(&m_dwHostTaskRefCount);
    IHostTask *pHostTask = m_pHostTask;
    if (pHostTask != NULL)
    {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        pHostTask->AddRef();
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }
    CounterDecrease(&m_dwHostTaskRefCount);
    return pHostTask;
}

void Thread::ReleaseHostTask()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (m_pHostTask == NULL)
    {
        return;
    }

    IHostTask *pHostTask = m_pHostTask;
    m_pHostTask = NULL;
    while (m_dwHostTaskRefCount > 0)
    {
        __SwitchToThread(0);
    }
    
    BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
    pHostTask->Release();
    END_SO_TOLERANT_CODE_CALLING_HOST;

}

ULONG Thread::AddRef()
{
    WRAPPER_CONTRACT;

    _ASSERTE(m_ExternalRefCount > 0);

    _ASSERTE (m_UnmanagedRefCount != (DWORD) -1);
    ULONG ref = FastInterlockIncrement((LONG*)&m_UnmanagedRefCount);

#ifdef _DEBUG
    AddFiberInfo(ThreadTrackInfo_Lifetime);
#endif
    return ref;
}

ULONG Thread::Release()
{
    WRAPPER_CONTRACT;

    _ASSERTE (m_ExternalRefCount > 0);
    _ASSERTE (m_UnmanagedRefCount > 0);
    ULONG ref = FastInterlockDecrement((LONG*)&m_UnmanagedRefCount);
#ifdef _DEBUG
    AddFiberInfo(ThreadTrackInfo_Lifetime);
#endif
    return ref;
}

HRESULT Thread::QueryInterface(REFIID riid, void **ppUnk)
{
    LEAF_CONTRACT;

    return E_NOINTERFACE;
}

BOOL IsHostedThread()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (!CLRTaskHosted())
    {
        return FALSE;
    }

    Thread *pThread = GetThread();
    if (pThread && pThread->GetHostTask() != NULL)
    {
        return TRUE;
    }

    IHostTaskManager *pManager = CorHost2::GetHostTaskManager();
    IHostTask *pHostTask = NULL;
    BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        pManager->GetCurrentTask(&pHostTask);
    END_SO_TOLERANT_CODE_CALLING_HOST;

    BOOL fRet = (pHostTask != NULL);
    if (pHostTask)
    {
        if (pThread)
        {
            _ASSERTE (pThread->GetHostTask() == NULL);
            pThread->m_pHostTask = pHostTask;
        }
        else
        {
            pHostTask->Release();
        }
    }

    return fRet;
}

IHostTask *GetCurrentHostTask()
{
    IHostTaskManager *provider = CorHost2::GetHostTaskManager();

    IHostTask *pHostTask = NULL;

    BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
    provider->GetCurrentTask(&pHostTask);
    END_SO_TOLERANT_CODE_CALLING_HOST;

    if (pHostTask)
    {
    pHostTask->Release();
    }

    return pHostTask;
}

void __stdcall Thread::LeaveRuntime(size_t target)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HRESULT hr = LeaveRuntimeNoThrow(target);
    if (FAILED(hr))
        ThrowHR(hr);
}

HRESULT Thread::LeaveRuntimeNoThrow(size_t target)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (!CLRTaskHosted())
    {
        return S_OK;
    }

    if (!IsHostedThread())
    {
        return S_OK;
    }

    HRESULT hr = S_OK;

    // A SQL thread can enter the runtime w/o a managed thread.
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(hr = COR_E_STACKOVERFLOW);

    IHostTaskManager *pManager = CorHost2::GetHostTaskManager();
    if (pManager)
    {
#ifdef _DEBUG
        Thread *pThread = GetThread();
        if (pThread)
        {
            pThread->AddFiberInfo(Thread::ThreadTrackInfo_UM_M);
        }
#endif
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pManager->LeaveRuntime(target);
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }
    END_SO_INTOLERANT_CODE;

    return hr;
}

void __stdcall Thread::LeaveRuntimeThrowComplus(size_t target)
{

    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    IHostTaskManager *pManager = NULL;


    if (!CLRTaskHosted())
    {
        goto Exit;
    }

    if (!IsHostedThread())
    {
        goto Exit;
    }

    pManager = CorHost2::GetHostTaskManager();
    if (pManager)
    {
#ifdef _DEBUG
        Thread *pThread = GetThread();
        if (pThread)
        {
            pThread->AddFiberInfo(Thread::ThreadTrackInfo_UM_M);
        }
#endif
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pManager->LeaveRuntime(target);
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }

    if (FAILED(hr))
    {
        INSTALL_UNWIND_AND_CONTINUE_HANDLER;
        ThrowHR(hr);
        UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
    }


Exit:
;

}

void __stdcall Thread::EnterRuntime()
{

    DWORD dwLastError = GetLastError();

    CONTRACTL {
        THROWS;
        ENTRY_POINT;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    //BEGIN_ENTRYPOINT_THROWS;

    HRESULT hr = EnterRuntimeNoThrow();
    if (FAILED(hr))
        ThrowHR(hr);

    SetLastError(dwLastError);
    //END_ENTRYPOINT_THROWS;

}

HRESULT Thread::EnterRuntimeNoThrow()
{
    DWORD dwLastError = GetLastError();

    // This function can be called during a hard SO when managed code has called out to native
    // which has SOd, so we can't probe here.  We already probe in LeaveRuntime, which will be
    // called at roughly the same stack level as LeaveRuntime, so we assume that the probe for
    // LeaveRuntime will cover us here.

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (!CLRTaskHosted() || !IsHostedThread())
    {
        SetLastError(dwLastError);
        return S_OK;
    }

    HRESULT hr = S_OK;

    IHostTaskManager *pManager = CorHost2::GetHostTaskManager();

    if (pManager)
    {
#ifdef _DEBUG
        // A SQL thread can enter the runtime w/o a managed thread.
        Thread *pThread = GetThread();
        if (pThread)
        {
            pThread->AddFiberInfo(Thread::ThreadTrackInfo_UM_M);
        }
#endif
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pManager->EnterRuntime();
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }

    SetLastError(dwLastError);

    return hr;
}

void Thread::ReverseEnterRuntime()
{
    WRAPPER_CONTRACT;

    HRESULT hr = ReverseEnterRuntimeNoThrow();

    if (hr != S_OK)
        ThrowHR(hr);
}

__declspec(noinline) void Thread::ReverseEnterRuntimeThrowComplusHelper(HRESULT hr)
{
    WRAPPER_CONTRACT;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER;
    ThrowHR(hr);
    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
}

void Thread::ReverseEnterRuntimeThrowComplus()
{
    WRAPPER_CONTRACT;

    HRESULT hr = ReverseEnterRuntimeNoThrow();

    if (hr != S_OK)
    {
        ReverseEnterRuntimeThrowComplusHelper(hr);
    }
}


HRESULT Thread::ReverseEnterRuntimeNoThrow()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (!CLRTaskHosted())
    {
        return S_OK;
    }

    if (!IsHostedThread())
    {
        return S_OK;
    }

    HRESULT hr = S_OK;


    IHostTaskManager *pManager = CorHost2::GetHostTaskManager();
    if (pManager)
    {
#ifdef _DEBUG
        // A SQL thread can enter the runtime w/o a managed thread.
        BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(hr = COR_E_STACKOVERFLOW);

        Thread *pThread = GetThread();
        if (pThread)
        {
            pThread->AddFiberInfo(Thread::ThreadTrackInfo_UM_M);
        }
        END_SO_INTOLERANT_CODE;

#endif
        hr = pManager->ReverseEnterRuntime();
    }
    return hr;
}

void Thread::ReverseLeaveRuntime()
{
    // This function can be called during a hard SO so we can't probe here.  We already probe in
    // ReverseEnterRuntime, which will be called at roughly the same stack level as ReverseLeaveRuntime,
    // so we assume that the probe for ReverseEnterRuntime will cover us here.

    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // SetupForComCallHR calls this inside a CATCH, but it triggers a THROWs violation
    CONTRACT_VIOLATION(ThrowsViolation);

    if (!CLRTaskHosted())
    {
        return;
    }

    if (!IsHostedThread())
    {
        return;
    }

    HRESULT hr = S_OK;

    IHostTaskManager *pManager = CorHost2::GetHostTaskManager();

    if (pManager)
    {
#ifdef _DEBUG
        // A SQL thread can enter the runtime w/o a managed thread.
        Thread *pThread = GetThread();
        if (pThread)
        {
        pThread->AddFiberInfo(Thread::ThreadTrackInfo_UM_M);
        }
#endif
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pManager->ReverseLeaveRuntime();
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }

    if (hr != S_OK)
        ThrowHR(hr);

}

// For OS EnterCriticalSection, call host to enable ThreadAffinity
void Thread::BeginThreadAffinity()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;    // no global state update
    }
    CONTRACTL_END;

    if (!CLRTaskHosted())
    {
        return;
    }

    if (IsGCSpecialThread() || IsDbgHelperSpecialThread())
    {
        return;
    }

    IHostTaskManager *pManager = CorHost2::GetHostTaskManager();

    HRESULT hr;

    BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
    hr = pManager->BeginThreadAffinity();
    END_SO_TOLERANT_CODE_CALLING_HOST;
    _ASSERTE (hr == S_OK);
    Thread *pThread = GetThread();

    if (pThread)
    {
        pThread->IncThreadAffinityCount();
#ifdef _DEBUG
        pThread->AddFiberInfo(Thread::ThreadTrackInfo_Affinity);
#endif
    }
}


// For OS EnterCriticalSection, call host to enable ThreadAffinity
void Thread::EndThreadAffinity()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;    // no global state update
    }
    CONTRACTL_END;

    if (!CLRTaskHosted())
    {
        return;
    }

    if (IsGCSpecialThread() || IsDbgHelperSpecialThread())
    {
        return;
    }

    IHostTaskManager *pManager = CorHost2::GetHostTaskManager();

    HRESULT hr;
    Thread *pThread = GetThread();
    if (pThread)
    {
        pThread->DecThreadAffinityCount ();
#ifdef _DEBUG
        pThread->AddFiberInfo(Thread::ThreadTrackInfo_Affinity);
#endif
    }
    BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
    hr = pManager->EndThreadAffinity();
    END_SO_TOLERANT_CODE_CALLING_HOST;

    _ASSERTE (hr == S_OK);
}

// Host does not allow nesting DelayAbort
void Thread::BeginDelayAbort()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE (this == GetThread());
    m_dwDelayAbortCount ++;
    _ASSERTE (m_dwDelayAbortCount > 0);
    if (m_dwDelayAbortCount == 1)
    {
        IHostTaskManager *pManager = CorHost2::GetHostTaskManager();
        if (pManager)
        {
#ifdef _DEBUG
            AddFiberInfo(ThreadTrackInfo_Abort);
#endif

            HRESULT hr;
            hr = pManager->BeginDelayAbort();

            _ASSERTE (hr == S_OK);
        }
    }
}

void Thread::EndDelayAbort()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE (this == GetThread());
    _ASSERTE (m_dwDelayAbortCount > 0);
    m_dwDelayAbortCount --;
    if (m_dwDelayAbortCount == 0)
    {
        IHostTaskManager *pManager = CorHost2::GetHostTaskManager();
        if (pManager)
        {
            HRESULT hr;
            hr = pManager->EndDelayAbort();

            _ASSERTE (hr == S_OK);
#ifdef _DEBUG
            AddFiberInfo(ThreadTrackInfo_Abort);
#endif
        }
    }
}

void Thread::SetupThreadForHost()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE (GetThread() == this);
    CONTRACT_VIOLATION(SOToleranceViolation);

    IHostTask *pHostTask = GetHostTask();
    if (pHostTask) {
        SetupFiberData();

        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pHostTask->SetCLRTask(this);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr))
        {
            ThrowHR(hr);
        }
        if (m_WeOwnThreadHandle)
        {
            // If host provides a thread handle, we do not need to own a handle.
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
            CorHost2::GetHostTaskManager()->SwitchToTask(0);
            END_SO_TOLERANT_CODE_CALLING_HOST;
            if (m_ThreadHandleForClose != INVALID_HANDLE_VALUE)
            {
                m_WeOwnThreadHandle = FALSE;
                CloseHandle(m_ThreadHandleForClose);
                m_ThreadHandleForClose = INVALID_HANDLE_VALUE;
            }
        }
    }
}


ETaskType GetCurrentTaskType()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_SO_TOLERANT;

    ETaskType TaskType = TT_UNKNOWN;
    size_t type = (size_t)ClrFlsGetValue (TlsIdx_ThreadType);
    if (type & ThreadType_DbgHelper)
    {
        TaskType = TT_DEBUGGERHELPER;
    }
    else if (type & ThreadType_GC)
    {
        TaskType = TT_GC;
    }
    else if (type & ThreadType_Finalizer)
    {
        TaskType = TT_FINALIZER;
    }
    else if (type & ThreadType_Timer)
    {
        TaskType = TT_THREADPOOL_TIMER;
    }
    else if (type & ThreadType_Gate)
    {
        TaskType = TT_THREADPOOL_GATE;
    }
    else if (type & ThreadType_Wait)
    {
        TaskType = TT_THREADPOOL_WAIT;
    }
    else if (type & ThreadType_ADUnloadHelper)
    {
        TaskType = TT_ADUNLOAD;
    }
    else if (type & ThreadType_Threadpool_IOCompletion)
    {
        TaskType = TT_THREADPOOL_IOCOMPLETION;
    }
    else if (type & ThreadType_Threadpool_Worker)
    {
        TaskType = TT_THREADPOOL_WORKER;
    }
    else
    {
        Thread *pThread = GetThread();
        if (pThread)
        {
            TaskType = TT_USER;
        }
    }

    return TaskType;
}

DeadlockAwareLock::DeadlockAwareLock(const char *description)
  : m_pHoldingThread(NULL)
#ifdef _DEBUG
    , m_description(description)
#endif
{
    LEAF_CONTRACT;
}

DeadlockAwareLock::~DeadlockAwareLock()
{
    LEAF_CONTRACT;
    // Wait for another thread to leave its loop in DeadlockAwareLock::TryBeginEnterLock
    CrstHolder lock(&g_DeadlockAwareCrst);
}

CHECK DeadlockAwareLock::CheckDeadlock(Thread *pThread)
{
    CONTRACTL
    {
        PRECONDITION(g_DeadlockAwareCrst.OwnedByCurrentThread());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Note that this check is recursive in order to produce descriptive check failure messages.

    if (pThread == m_pHoldingThread)
    {
        CHECK_FAILF(("Lock %p (%s) is held by thread %d", this, m_description, pThread));
    }

    if (m_pHoldingThread != NULL
        && m_pHoldingThread->m_pBlockingLock != NULL)
    {
        CHECK_MSGF(m_pHoldingThread->m_pBlockingLock->CheckDeadlock(pThread),
                   ("Deadlock: Lock %p (%s) is held by thread %d", this, m_description, m_pHoldingThread));
    }

    CHECK_OK;
}

BOOL DeadlockAwareLock::CanEnterLock(Thread *pThread)
{
    if (pThread == NULL)
        pThread = GetThread();

    CONSISTENCY_CHECK_MSG(pThread != NULL,
                          "Cannot do deadlock detection on non-EE thread");
    CONSISTENCY_CHECK_MSG(pThread->m_pBlockingLock == NULL,
                          "Cannot block on two locks at once");

    {
        CrstHolder lock(&g_DeadlockAwareCrst);

        // Look for deadlocks
        DeadlockAwareLock *pLock = this;

        while (TRUE)
        {
            Thread * volatile holdingThread = pLock->m_pHoldingThread;

            if (holdingThread == pThread)
            {
                // Deadlock!
                return FALSE;
            }
            if (holdingThread == NULL)
            {
                // Lock is unheld
                break;
            }

            pLock = holdingThread->m_pBlockingLock;

            if (pLock == NULL)
            {
                // Thread is running free
                break;
            }
        }

        return TRUE;
    }
}

BOOL DeadlockAwareLock::TryBeginEnterLock(Thread *pThread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (pThread == NULL)
        pThread = GetThread();

    CONSISTENCY_CHECK_MSG(pThread != NULL,
                          "Cannot do deadlock detection on non-EE thread");
    CONSISTENCY_CHECK_MSG(pThread->m_pBlockingLock == NULL,
                          "Cannot block on two locks at once");

    {
        CrstHolder lock(&g_DeadlockAwareCrst);

        // Look for deadlocks
        DeadlockAwareLock *pLock = this;

        while (TRUE)
        {
            Thread * volatile holdingThread = pLock->m_pHoldingThread;

            if (holdingThread == pThread)
            {
                // Deadlock!
                return FALSE;
            }
            if (holdingThread == NULL)
            {
                // Lock is unheld
                break;
            }

            pLock = holdingThread->m_pBlockingLock;

            if (pLock == NULL)
            {
                // Thread is running free
                break;
            }
        }

        pThread->m_pBlockingLock = this;
    }

    return TRUE;
};

void DeadlockAwareLock::BeginEnterLock(Thread *pThread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (pThread == NULL)
        pThread = GetThread();

    CONSISTENCY_CHECK_MSG(pThread != NULL,
                          "Cannot do deadlock detection on non-EE thread");
    CONSISTENCY_CHECK_MSG(pThread->m_pBlockingLock == NULL,
                          "Cannot block on two locks at once");

    {
        CrstHolder lock(&g_DeadlockAwareCrst);

        // Look for deadlock loop
        CONSISTENCY_CHECK_MSG(CheckDeadlock(pThread), "Deadlock detected!");

        pThread->m_pBlockingLock = this;
    }
};

void DeadlockAwareLock::EndEnterLock(Thread *pThread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (pThread == NULL)
        pThread = GetThread();

    CONSISTENCY_CHECK(m_pHoldingThread == NULL || m_pHoldingThread == pThread);
    CONSISTENCY_CHECK(pThread->m_pBlockingLock == this);

    // No need to take a lock when going from blocking to holding.  This
    // transition implies the lack of a deadlock that other threads can see.
    // (If they would see a deadlock after the transition, they would see
    // one before as well.)

    m_pHoldingThread = pThread;
}

void DeadlockAwareLock::LeaveLock()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    CONSISTENCY_CHECK(m_pHoldingThread == GetThread());
    CONSISTENCY_CHECK(GetThread()->m_pBlockingLock == NULL);

    m_pHoldingThread = NULL;
}


#ifdef _DEBUG

// Normally, any thread we operate on has a Thread block in its TLS.  But there are
// a few special threads we don't normally execute managed code on.
//
// There is a scenario where we run managed code on such a thread, which is when the
// DLL_THREAD_ATTACH notification of an (IJW?) module calls into managed code.  This
// is incredibly dangerous.  If a GC is provoked, the system may have trouble performing
// the GC because its threads aren't available yet.                                  
static DWORD SpecialEEThreads[10];
static LONG  cnt_SpecialEEThreads = 0;

void dbgOnly_IdentifySpecialEEThread()
{
    WRAPPER_CONTRACT;

    LONG  ourCount = FastInterlockIncrement(&cnt_SpecialEEThreads);

    _ASSERTE(ourCount < (LONG) NumItems(SpecialEEThreads));
    SpecialEEThreads[ourCount-1] = ::GetCurrentThreadId();
}

BOOL dbgOnly_IsSpecialEEThread()
{
    WRAPPER_CONTRACT;

    DWORD   ourId = ::GetCurrentThreadId();

    for (LONG i=0; i<cnt_SpecialEEThreads; i++)
        if (ourId == SpecialEEThreads[i])
            return TRUE;

    // If we have an EE thread doing helper thread duty, then it is temporarily
    // 'special' too.
    #ifdef DEBUGGING_SUPPORTED
    DWORD helperID = g_pDebugInterface->GetHelperThreadID();
    if (helperID == ourId)
        return TRUE;
    #endif

    if (GetThread() == NULL)
        return TRUE;


    return FALSE;
}

#endif // _DEBUG


// There is an MDA which can detect illegal reentrancy into the CLR.  For instance, if you call managed
// code from a native vectored exception handler, this might cause a reverse PInvoke to occur.  But if the
// exception was triggered from code that was executing in cooperative GC mode, we now have GC holes and
// general corruption.
BOOL ShouldCheckReentrancy()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // Because of how C++ generates code, we must use default initialization to
    // 0 here.  Any explicit initialization will result in thread-safety problems.
    static BOOL fInited;
    static BOOL fShouldCheck;

    if (fInited == FALSE)
    {
        fShouldCheck = FALSE;
        fInited = TRUE;
    }
    return fShouldCheck;
}


// Actually fire the Reentrancy probe, if warranted.
BOOL HasIllegalReentrancy()
{

    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        ENTRY_POINT;
        MODE_ANY;
    }
    CONTRACTL_END;

    BOOL retVal = FALSE;


    if (ShouldCheckReentrancy())
    {
        BEGIN_ENTRYPOINT_VOIDRET;
        END_ENTRYPOINT_VOIDRET;
    }

    return retVal;
}


#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void
STATIC_DATA::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    DAC_ENUM_STHIS(STATIC_DATA);
}

void
Thread::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    DAC_ENUM_VTHIS();
    if (flags != CLRDATA_ENUM_MEM_MINI)
    {
        if (m_pDomain.IsValid())
        {
            m_pDomain->EnumMemoryRegions(flags, true);
        }

        if (m_Context.IsValid())
        {
            m_Context->EnumMemoryRegions(flags);
        }
    }

    if (m_debuggerFilterContext.IsValid())
    {
        m_debuggerFilterContext.EnumMem();
    }

    OBJECTHANDLE_EnumMemoryRegions(m_LastThrownObjectHandle);

    m_ExceptionState.EnumChainMemoryRegions(flags);

    if (m_pUnsharedStaticData.IsValid())
    {
        m_pUnsharedStaticData->EnumMemoryRegions(flags);
    }
    if (m_pSharedStaticData.IsValid())
    {
        m_pSharedStaticData->EnumMemoryRegions(flags);
    }

    if (flags != CLRDATA_ENUM_MEM_MINI)
    {

        //
        // Allow all of the frames on the stack to enumerate
        // their memory.
        //

        PTR_Frame frame = m_pFrame;
        while (frame.IsValid() &&
               frame.GetAddr() != (TADDR)FRAME_TOP)
        {
            frame->EnumMemoryRegions(flags);
            frame = frame->m_Next;
        }
    }

    //
    // Try and do a stack trace and save information
    // for each part of the stack.  This is very vulnerable
    // to memory problems so ignore all exceptions here.
    //
    PAL_TRY
    {
        EnumMemoryRegionsWorker(flags);
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
    }
    PAL_ENDTRY
}

void
Thread::EnumMemoryRegionsWorker(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    if (IsUnstarted())
    {
        return;
    }

    CONTEXT context;
    BOOL DacGetThreadContext(Thread* thread, CONTEXT* context);
    REGDISPLAY regDisp;
    StackFrameIterator frameIter;

    if (GetFilterContext())
    {
        context = *GetFilterContext();
    }
    else
    {
        DacGetThreadContext(this, &context);
    }
    FillRegDisplay(&regDisp, &context);
    frameIter.Init(this, NULL, &regDisp, 0);
    while (frameIter.IsValid())
    {
        //
        // x86 stack walkers often end up having to guess
        // about what's a return address on the stack.
        // Doing so involves looking at the code at the
        // possible call site and seeing if it could
        // reach the callee.  Save enough code and around
        // the call site to allow this with a dump.
        //
        // For whatever reason 64-bit platforms require us to save
        // the instructions around the call sites on the stack as well.
        // Otherwise we cannnot show the stack in a minidump.
        //
        TADDR callEnd = taGetControlPC(&regDisp);
        DacEnumMemoryRegion(callEnd - MAX_INSTRUCTION_LENGTH, MAX_INSTRUCTION_LENGTH * 2);
#if defined(_X86_)
        // If it was an indirect call we also need
        // to save the data indirected through.
        PTR_BYTE callCode = PTR_BYTE(callEnd - 6);
        PTR_BYTE callMrm = PTR_BYTE(callEnd - 5);
        PTR_TADDR callInd = PTR_TADDR(callEnd - 4);
        if (callCode.IsValid() &&
            *callCode == 0xff &&
            callMrm.IsValid() &&
            (*callMrm & 0x30) == 0x10 &&
            callInd.IsValid())
        {
            DacEnumMemoryRegion(*callInd, sizeof(TADDR));
        }
#endif // #ifdef _X86_

        if (flags != CLRDATA_ENUM_MEM_MINI)
        {
            if (frameIter.m_crawl.GetAppDomain())
            {
                frameIter.m_crawl.GetAppDomain()->EnumMemoryRegions(flags, true);
            }
        }

        MethodDesc* pMD = frameIter.m_crawl.GetFunction();
        if (pMD != NULL)
        {
            pMD->EnumMemoryRegions(flags);
        }

        if (frameIter.Next() != SWA_CONTINUE)
        {
            break;
        }
    }
}

void
ThreadStore::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    // This will write out the context of the s_pThreadStore. ie
    // just the pointer
    //
    s_pThreadStore.EnumMem();
    if (s_pThreadStore.IsValid())
    {

        // write out the whole ThreadStore structure
        DacEnumHostDPtrMem(s_pThreadStore);

        // The thread list may be corrupt, so just
        // ignore exceptions during enumeration.
        PAL_TRY
        {
            Thread* thread       = s_pThreadStore->m_ThreadList.GetHead();
            LONG    dwNumThreads = s_pThreadStore->m_ThreadCount;

            for (LONG i = 0; (i < dwNumThreads) && (thread != NULL); i++)
            {
                thread->EnumMemoryRegions(flags);
                thread = s_pThreadStore->m_ThreadList.GetNext(thread);
            }
        }
        PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
        {
        }
        PAL_ENDTRY
    }
}

#endif // #ifdef DACCESS_COMPILE


#ifdef TIME_SUSPEND

// There is a current and a prior copy of the statistics.  This allows us to display deltas per reporting
// interval, as well as running totals.  The 'min' and 'max' values require special treatment.  They are
// Reset (zeroed) in the current statistics when we begin a new interval and they are updated via a
// comparison with the global min/max.
SuspendStatistics g_SuspendStatistics;
SuspendStatistics g_LastSuspendStatistics;

DWORD SuspendStatistics::GetTime()
{
    LARGE_INTEGER large;

    if (divisor == 0)
    {
        if (QueryPerformanceFrequency(&large) && (large.QuadPart != 0))
            divisor = large.QuadPart / (1000 * 1000);        // microseconds
        else
            divisor = 1;
    }

    if (QueryPerformanceCounter(&large))
        return (DWORD) (large.QuadPart / divisor);
    else
        return 0;
}

DWORD SuspendStatistics::GetElapsed(DWORD start, DWORD stop)
{
    if (stop > start)
        return stop - start;

    INT64 bigStop = stop;
    bigStop += 0x100000000;
    bigStop -= start;

    _ASSERTE(((INT64)(DWORD)bigStop) == bigStop);

    return (DWORD) bigStop;
}

// Called whenever our timers start to overflow
void SuspendStatistics::Initialize()
{
    printf("SUSP **** Initialize *****\n");
    memset(&g_SuspendStatistics, 0, sizeof(g_SuspendStatistics));   // erasing this!
    memset(&g_LastSuspendStatistics, 0, sizeof(g_LastSuspendStatistics));
}

// Top of SuspendEE
void SuspendStatistics::StartSuspend()
{
    startSuspend = GetTime();
}

// Bottom of SuspendEE
void SuspendStatistics::EndSuspend()
{
    DWORD time = GetElapsed(startSuspend, GetTime());

    suspend.Accumulate(time);
    cntSuspends++;
}

// Top of RestartEE
void SuspendStatistics::StartRestart()
{
    startRestart = GetTime();
}

// Bottom of RestartEE
void SuspendStatistics::EndRestart()
{
    DWORD timeNow = GetTime();

    restart.Accumulate(GetElapsed(startRestart, timeNow));
    cntRestarts++;

    paused.Accumulate(SuspendStatistics::GetElapsed(startSuspend, timeNow));

    // every so often, print a summary of our statistics
    DWORD ticksNow = GetTickCount();

    if (ticksNow - startTick > secondsToDisplay * 1000)
    {
        DisplayAndUpdate();

        startTick = GetTickCount();

        // Our counters are 32 bits and can count to 4 GB in microseconds or 4K in seconds.
        // Reset when we get close to overflowing
        if (++cntDisplay > 3900 / secondsToDisplay)
            Initialize();
    }
}

void SuspendStatistics::DisplayAndUpdate()
{
    printf("SUSP **** Summary ***** %d\n", cntDisplay);

    _ASSERTE(cntSuspends == cntRestarts);

    paused.DisplayAndUpdate    ("Paused ", &g_LastSuspendStatistics.paused,     cntSuspends, g_LastSuspendStatistics.cntSuspends);
    suspend.DisplayAndUpdate   ("Suspend", &g_LastSuspendStatistics.suspend,    cntSuspends, g_LastSuspendStatistics.cntSuspends);
    restart.DisplayAndUpdate   ("Restart", &g_LastSuspendStatistics.restart,    cntSuspends, g_LastSuspendStatistics.cntSuspends);
    acquireTSL.DisplayAndUpdate("LockTSL", &g_LastSuspendStatistics.acquireTSL, cntSuspends, g_LastSuspendStatistics.cntSuspends);
    releaseTSL.DisplayAndUpdate("Unlock ", &g_LastSuspendStatistics.releaseTSL, cntSuspends, g_LastSuspendStatistics.cntSuspends);
    osSuspend.DisplayAndUpdate ("OS Susp", &g_LastSuspendStatistics.osSuspend,  cntOSSuspendResume, g_LastSuspendStatistics.cntOSSuspendResume);
    crawl.DisplayAndUpdate     ("Crawl",   &g_LastSuspendStatistics.crawl,      cntHijackCrawl, g_LastSuspendStatistics.cntHijackCrawl);
    wait.DisplayAndUpdate      ("Wait",    &g_LastSuspendStatistics.wait,       cntWaits,    g_LastSuspendStatistics.cntWaits);

    printf("OS Suspend Failures %d (%d), Wait Timeouts %d (%d), Hijack traps %d (%d)\n",
           cntFailedSuspends - g_LastSuspendStatistics.cntFailedSuspends, cntFailedSuspends,
           cntWaitTimeouts - g_LastSuspendStatistics.cntWaitTimeouts, cntWaitTimeouts,
           cntHijackTrap - g_LastSuspendStatistics.cntHijackTrap, cntHijackTrap);

    printf("Redirected EIP Failures %d (%d), Collided GC/Debugger/ADUnload %d (%d)\n",
           cntFailedRedirections - g_LastSuspendStatistics.cntFailedRedirections, cntFailedRedirections,
           cntCollideRetry - g_LastSuspendStatistics.cntCollideRetry, cntCollideRetry);

    memcpy(&g_LastSuspendStatistics, this, sizeof(g_LastSuspendStatistics));

    suspend.Reset();
    restart.Reset();
    paused.Reset();
    acquireTSL.Reset();
    releaseTSL.Reset();
    osSuspend.Reset();
    crawl.Reset();
    wait.Reset();
}

void MinMaxTot::DisplayAndUpdate(__in_z char *pName, MinMaxTot *pLastOne, int fullCount, int priorCount)
{
    int delta = fullCount - priorCount;

    printf("%s  %u (%u) times for %u (%u) usec. Min %u (%u), Max %u (%u), Avg %u (%u)\n",
           pName,
           delta, fullCount,
           totVal - pLastOne->totVal, totVal,
           minVal, pLastOne->minVal,
           maxVal, pLastOne->maxVal,
           (delta == 0 ? 0 : (totVal - pLastOne->totVal) / delta),
           (fullCount == 0 ? 0 : totVal / fullCount));

    if (minVal > pLastOne->minVal && pLastOne->minVal != 0)
        minVal = pLastOne->minVal;

    if (maxVal < pLastOne->maxVal)
        maxVal = pLastOne->maxVal;
}

#endif

