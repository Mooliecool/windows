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
// THREADS.H -
//
// Currently represents a logical and physical COM+ thread.  Later, these concepts
// will be separated.
//

#ifndef __threads_h__
#define __threads_h__

#include "vars.hpp"
#include "util.hpp"
#include "eventstore.hpp"
#include "argslot.h"
#include "context.h"
#include "regdisp.h"
#include "mscoree.h"
#include "appdomainstack.h"
#include "gc.h"
#include "binder.h"

class     Thread;
class     ThreadStore;
class     MethodDesc;
struct    PendingSync;
class     AppDomain;
class     NDirect;
class     Frame;
class     ThreadBaseObject;
class     LocalDataStore;
class     AppDomainStack;
class     CLRRootBase;
class     LoadLevelLimiter;
class     DomainFile;
class     DeadlockAwareLock;
struct    HelperMethodFrameCallerList;
class     ThreadLocalIBCInfo;
class     ICodeInfo;

#include "stackwalktypes.h"
#include "log.h"
#include "stackingallocator.h"
#include "excep.h"
#include "synch.h"
#include "exstate.h"
#if defined(USE_DBGHELP_TO_WALK_STACK_IN_DAC)
#include <imagehlp.h>
#endif

// NT thread priorities range from -15 to +15.
#define INVALID_THREAD_PRIORITY  ((DWORD)0x80000000)

// Has to be in sync with Thread.STATICS_BUCKET_SIZE (thread.cs)
#define THREAD_STATICS_BUCKET_SIZE 32

// For a fiber which swtiched out, we set its OSID to a special number
// Note: there's a copy of this macro in strike.cpp
#define SWITCHED_OUT_FIBER_OSID 0xbaadf00d;



// Capture all the synchronization requests, for debugging purposes
#if defined(_DEBUG) && defined(TRACK_SYNC)

// Each thread has a stack that tracks all enter and leave requests
struct Dbg_TrackSync
{
    virtual void EnterSync    (UINT_PTR caller, void *pAwareLock) = 0;
    virtual void LeaveSync    (UINT_PTR caller, void *pAwareLock) = 0;
};

EXTERN_C void EnterSyncHelper    (UINT_PTR caller, void *pAwareLock);
EXTERN_C void LeaveSyncHelper    (UINT_PTR caller, void *pAwareLock);

#endif  // TRACK_SYNC



#if USE_INDIRECT_GET_THREAD_APPDOMAIN
// Access to the Thread object in the TLS.
typedef Thread* (__stdcall *POPTIMIZEDTHREADGETTER)();
EXTERN_C POPTIMIZEDTHREADGETTER GetThread;

// Access to the AppDomain object in the TLS.
typedef AppDomain* (__stdcall *POPTIMIZEDAPPDOMAINGETTER)();
EXTERN_C POPTIMIZEDAPPDOMAINGETTER GetAppDomain;

#else
// These guys are implemented in ASM and won't touch any of the argument
// registers, the C++ compiler won't take advantage of this however.
EXTERN_C Thread*    GetThread();
EXTERN_C AppDomain* GetAppDomain();
#endif

//***************************************************************************
 #define HAS_TRACK_CXX_EXCEPTION_CODE_HACK 0

// manifest constant for waiting in the exposed classlibs
const INT32 INFINITE_TIMEOUT = -1;

/***************************************************************************/
// Public enum shared between thread and threadpool
// These are two kinds of threadpool thread that the threadpool mgr needs
// to keep track of
enum ThreadpoolThreadType
{
    WorkerThread,
    CompletionPortThread,
    WaitThread,
    TimerMgrThread
};
//***************************************************************************
// Public functions
//
//      Thread* GetThread()             - returns current Thread
//      Thread* SetupThread()           - creates new Thread.
//      Thread* SetupUnstartedThread()  - creates new unstarted Thread which
//                                        (obviously) isn't in a TLS.
//      void    DestroyThread()         - the underlying logical thread is going
//                                        away.
//      void    DetachThread()          - the underlying logical thread is going
//                                        away but we don't want to destroy it yet.
//
// Public functions for ASM code generators
//
//      int GetThreadTLSIndex()         - returns TLS index used to point to Thread
//      int GetAppDomainTLSIndex()      - returns TLS index used to point to AppDomain
//
// Public functions for one-time init/cleanup
//
//      void InitThreadManager()      - onetime init
//      void TerminateThreadManager() - onetime cleanup
//
// Public functions for taking control of a thread at a safe point
//
//      VOID OnHijackObjectTripThread() - we've hijacked a JIT object-ref return
//      VOID OnHijackScalarTripThread() - we've hijacked a JIT non-object ref return
//
//***************************************************************************


//***************************************************************************
// Public functions
//***************************************************************************

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
Thread* SetupThread(BOOL fInternal);
inline Thread* SetupThread()
{
    WRAPPER_CONTRACT;
    return SetupThread(FALSE);
}
// A host can deny a thread entering runtime by returning a NULL IHostTask.
// But we do want threads used by threadpool.
inline Thread* SetupInternalThread()
{
    WRAPPER_CONTRACT;
    return SetupThread(TRUE);
}
Thread* SetupThreadNoThrow(HRESULT *phresult = NULL);
Thread* SetupThreadPoolThread(ThreadpoolThreadType tpType);
Thread* SetupThreadPoolThreadNoThrow(ThreadpoolThreadType tpType, HRESULT *pHR = NULL);
Thread* SetupUnstartedThread();
void    DestroyThread(Thread *th);




//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
DWORD GetThreadTLSIndex();

DWORD GetAppDomainTLSIndex();


//---------------------------------------------------------------------------
// One-time initialization. Called during Dll initialization.
//---------------------------------------------------------------------------
void InitThreadManager();


// When we want to take control of a thread at a safe point, the thread will
// eventually come back to us in one of the following trip functions:

void __stdcall OnDebuggerTripThread();        // thread was asked to stop for the debugger


// The following were used entirely by a hijacking pathway, but it turns out that
// context proxies need identical services to protect their return values if they
// trip during an unwind.  Note that OnStubObjectWorker returns an OBJECTREF rather
// than a "void *" but we cannot declare it as such because structs are passed as
// secret arguments.
EXTERN_C void   __cdecl CommonTripThread();


// When we resume a thread at a new location, to get an exception thrown, we have to
// pretend the exception originated elsewhere.
EXTERN_C void ThrowControlForThread(
        );


// RWLock state inside TLS
class CRWLock;
typedef struct tagLockEntry
{
    tagLockEntry *pNext;    // next entry
    tagLockEntry *pPrev;    // prev entry
    LONG dwULockID;
    LONG dwLLockID;         // owning lock
    WORD wReaderLevel;      // reader nesting level
} LockEntry;




#include "threads.h"


typedef DWORD (*AppropriateWaitFunc) (void *args, DWORD timeout, DWORD option);

// The Thread class represents a managed thread.  This thread could be internal
// or external (i.e. it wandered in from outside the runtime).  For internal
// threads, it could correspond to an exposed System.Thread object or it
// could correspond to an internal worker thread of the runtime.
//
// If there's a physical Win32 thread underneath this object (i.e. it isn't an
// unstarted System.Thread), then this instance can be found in the TLS
// of that physical thread.





#ifdef TIME_SUSPEND

// running aggregations
struct MinMaxTot
{
    DWORD minVal, maxVal, totVal;

    void Accumulate(DWORD time)
    {
        if (time < minVal || minVal == 0)
            minVal = time;

        if (time > maxVal)
            maxVal = time;

        // We are supposed to anticipate overflow and clear our totals
        _ASSERTE(((DWORD) (totVal + time)) > ((DWORD) totVal));

        totVal += time;
    }

    void Reset()
    {
        minVal = maxVal = 0;
    }

    void DisplayAndUpdate(__in_z char *pName, MinMaxTot *pLastOne, int fullCount, int priorCount);
};


// A note about timings.  We use QueryPerformanceCounter to measure all timings in units.  During
// Initialization, we compute a divisor to convert those timings into microseconds.  This means
// that we can accumulate about 4,000 seconds (over one hour) of GC time into 32-bit quantities
// before we must reinitialize.

struct SuspendStatistics
{
    // display the statistics every 10 seconds.
    static const int secondsToDisplay = 10;

    // we must re-initialize after an hour of GC time, to avoid overflow.  It's more convenient to
    // re-initialize after an hour of wall-clock time, instead
    int cntDisplay;

    // number of times we call SuspendEE, RestartEE
    int cntSuspends, cntRestarts;

    // Times for current suspension & restart
    DWORD startSuspend, startRestart;

    // min, max and total time spent performing a Suspend, a Restart, or Paused from the start of
    // a Suspend to the end of a Restart.  We can compute 'avg' using 'cnt' and 'tot' values.
    MinMaxTot suspend, restart, paused;

    // We know there can be contention on acquiring the ThreadStoreLock, or yield points when hosted (like
    // BeginThreadAffinity on the leading edge and EndThreadAffinity on the trailing edge).
    MinMaxTot acquireTSL, releaseTSL;

    // And if we OS suspend a thread that is blocking or perhaps throwing an exception and is therefore
    // stuck in the kernel, it could take approximately a second.  So track the time taken for OS
    // suspends
    MinMaxTot osSuspend;

    // And if we place a hijack, we need to crawl a stack to do so.
    MinMaxTot crawl;

    // And waiting can be a significant part of the total suspension time.
    MinMaxTot wait;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // There are some interesting events that are worth counting, because they show where the time is going:

    // number of times we waited on g_pGCSuspendEvent while trying to suspend the EE
    int cntWaits;

    // and the number of times those Waits timed out rather than being signalled by a cooperating thread
    int cntWaitTimeouts;

    // number of times we did an OS (or hosted) suspend or resume on a thread
    int cntOSSuspendResume;

    // number of times we crawled a stack for a hijack
    int cntHijackCrawl;

    // and the number of times the hijack actually trapped a thread for us
    int cntHijackTrap;

    // the number of times we redirected a thread in fully interruptible code, by rewriting its EIP
    // so it will throw to a blocking point
    int cntRedirections;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // And there are some "failure" cases that should never or almost never occur.

    // number of times we have a collision between e.g. Debugger suspension & GC suspension.
    // In these cases, everyone yields to the GC but at some cost.
    int cntCollideRetry;

    // number of times the OS or Host was unable to ::SuspendThread a thread for us.  This count should be
    // approximately 0.
    int cntFailedSuspends;

    // number of times we were unable to redirect a thread by rewriting its register state in a
    // suspended context.  This count should be approximately 0.
    int cntFailedRedirections;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Internal mechanism:

    // convert all timings into microseconds
    DWORD divisor;
    DWORD GetTime();
    static DWORD GetElapsed(DWORD start, DWORD stop);

    void Initialize();
    void DisplayAndUpdate();

    // we want to print statistics every 10 seconds - this is to remember the start of the 10 sec interval.
    DWORD startTick;

    // Public API

    void StartSuspend();
    void EndSuspend();

    void StartRestart();
    void EndRestart();
};

extern SuspendStatistics g_SuspendStatistics;
extern SuspendStatistics g_LastSuspendStatistics;

#endif


#if defined(USE_DBGHELP_TO_WALK_STACK_IN_DAC)
typedef BOOL (*PDBGHELP__STACKWALK) (
    DWORD                            MachineType,
    HANDLE                           hProcess,
    HANDLE                           hThread,
    LPSTACKFRAME64                   StackFrame,
    PVOID                            ContextRecord,
    PREAD_PROCESS_MEMORY_ROUTINE64   ReadMemoryRoutine,
    PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
    PGET_MODULE_BASE_ROUTINE64       GetModuleBaseRoutine,
    PTRANSLATE_ADDRESS_ROUTINE64     TranslateAddress
    );

typedef BOOL (*PDBGHELP__SYMINITIALIZE) (
    HANDLE  hProcess,
    PCSTR   UserSearchPath,
    BOOL    fInvadeProcess
    );

typedef BOOL (*PDBGHELP__SYMCLEANUP) (
    HANDLE  hProcess
    );
#endif // USE_DBGHELP_TO_WALK_STACK_IN_DAC


// This is the code we pass around for Thread.Interrupt, mainly for assertions
#define APC_Code    0xEECEECEE

class Thread: public ICLRTask
{
    friend struct ThreadQueue;  // used to enqueue & dequeue threads onto SyncBlocks
    friend class  ThreadStore;
    friend class  SyncBlock;
    friend class  Context;
    friend struct PendingSync;
    friend class  AppDomain;
    friend class  ThreadNative;
    friend class  DeadlockAwareLock;
#ifdef _DEBUG
    friend class  EEContract;
#endif
#ifdef DACCESS_COMPILE
    friend class ClrDataAccess;
    friend class ClrDataTask;
#endif

    friend BOOL NTGetThreadContext(Thread *pThread, CONTEXT *pContext);
    friend BOOL NTSetThreadContext(Thread *pThread, const CONTEXT *pContext);

    friend void __cdecl CommonTripThread();


    friend void         InitThreadManager();
    friend void         ThreadBaseObject::SetDelegate(OBJECTREF delegate);

    friend void CallFinalizerOnThreadObject(Object *obj);

    friend class ContextTransitionFrame;  // To set m_dwBeginLockCount

    // Debug and Profiler caches ThreadHandle.
    friend class Debugger;                  // void Debugger::ThreadStarted(Thread* pRuntimeThread, BOOL fAttaching);
    friend class ProfToEEInterfaceImpl;     // HRESULT ProfToEEInterfaceImpl::GetHandleFromThread(ThreadID threadId, HANDLE *phThread);
    friend class CExecutionEngine;
    friend class UnC;
    friend class CheckAsmOffsets;

    friend class ExceptionTracker;
    friend class ThreadExceptionState;

    friend class DebuggerController;        // DebuggerController::[Un]ApplyTraceFlag(Thread *thread)
    friend class DebuggerPatchSkip;         // DebuggerPatchSkip::DebuggerPatchSkip

#if defined(DACCESS_COMPILE)
    friend class StackFrameIterator;
#endif // DACESS_COMPILE

    VPTR_BASE_CONCRETE_VTABLE_CLASS(Thread)



public:

    // If we are trying to suspend a thread, we set the appropriate pending bit to
    // indicate why we want to suspend it (TS_GCSuspendPending, TS_UserSuspendPending,
    // TS_DebugSuspendPending).
    //
    // If instead the thread has blocked itself, via WaitSuspendEvent, we indicate
    // this with TS_SyncSuspended.  However, we need to know whether the synchronous
    // suspension is for a user request, or for an internal one (GC & Debug).  That's
    // because a user request is not allowed to resume a thread suspended for
    // debugging or GC.  -- That's not stricly true.  It is allowed to resume such a
    // thread so long as it was ALSO suspended by the user.  In other words, this
    // ensures that user resumptions aren't unbalanced from user suspensions.
    //
    enum ThreadState
    {
        TS_Unknown                = 0x00000000,    // threads are initialized this way

        TS_AbortRequested         = 0x00000001,    // Abort the thread
        TS_GCSuspendPending       = 0x00000002,    // waiting to get to safe spot for GC
        TS_UserSuspendPending     = 0x00000004,    // user suspension at next opportunity
        TS_DebugSuspendPending    = 0x00000008,    // Is the debugger suspending threads?
        TS_GCOnTransitions        = 0x00000010,    // Force a GC on stub transitions (GCStress only)

        TS_LegalToJoin            = 0x00000020,    // Is it now legal to attempt a Join()

        TS_YieldRequested         = 0x00000040,    // The task should yield

        TS_BlockGCForSO           = 0x00000100,    // If a thread does not have enough stack, WaitUntilGCComplete may fail.
                                                   // Either GC suspension will wait until the thread has cleared this bit,
                                                   // Or the current thread is going to spin if GC has suspended all threads.
        TS_Background             = 0x00000200,    // Thread is a background thread
        TS_Unstarted              = 0x00000400,    // Thread has never been started
        TS_Dead                   = 0x00000800,    // Thread is dead

        TS_WeOwn                  = 0x00001000,    // Exposed object initiated this thread

        // Some bits that only have meaning for reporting the state to clients.
        TS_ReportDead             = 0x00010000,    // in WaitForOtherThreads()

        TS_TaskReset              = 0x00040000,    // The task is reset

        TS_SyncSuspended          = 0x00080000,    // Suspended via WaitSuspendEvent
        TS_DebugWillSync          = 0x00100000,    // Debugger will wait for this thread to sync

        TS_StackCrawlNeeded       = 0x00200000,    // A stackcrawl is needed on this thread, such as for thread abort
                                                   // See comment for s_pWaitForStackCrawlEvent for reason.

        TS_SuspendUnstarted       = 0x00400000,    // latch a user suspension on an unstarted thread

        TS_ThreadPoolThread       = 0x00800000,    // is this a threadpool thread?
        TS_TPWorkerThread         = 0x01000000,    // is this a threadpool worker thread?

        TS_Interruptible          = 0x02000000,    // sitting in a Sleep(), Wait(), Join()
        TS_Interrupted            = 0x04000000,    // was awakened by an interrupt APC. !!! This can be moved to TSNC

        TS_CompletionPortThread   = 0x08000000,    // Completion port thread

        TS_AbortInitiated         = 0x10000000,    // set when abort is begun

        TS_Finalized              = 0x20000000,    // The associated managed Thread object has been finalized.
                                                   // We can clean up the unmanaged part now.

        TS_FailStarted            = 0x40000000,    // The thread fails during startup.
        TS_Detached               = 0x80000000,    // Thread was detached by DllMain


        // We require (and assert) that the following bits are less than 0x100.
        TS_CatchAtSafePoint = (TS_UserSuspendPending | TS_AbortRequested |
                               TS_GCSuspendPending | TS_DebugSuspendPending | TS_GCOnTransitions | TS_YieldRequested),
    };

    // Thread flags that aren't really states in themselves but rather things the thread
    // has to do.
    enum ThreadTasks
    {
        TT_CleanupSyncBlock       = 0x00000001, // The synch block needs to be cleaned up.
    };

    // Thread flags that have no concurrency issues (i.e., they are only manipulated by the owning thread). Use these
    // state flags when you have a new thread state that doesn't belong in the ThreadState enum above. Note: though this
    // enum seems to hold only debugger-related bits right now, its purpose is to hold bits for any purpose.
    //
    enum ThreadStateNoConcurrency
    {
        TSNC_Unknown                    = 0x00000000, // threads are initialized this way

        TSNC_DebuggerUserSuspend        = 0x00000001, // marked "suspended" by the debugger
        TSNC_DebuggerReAbort            = 0x00000002, // thread needs to re-abort itself when resumed by the debugger
        TSNC_DebuggerIsStepping         = 0x00000004, // debugger is stepping this thread
        TSNC_DebuggerIsManagedException = 0x00000008, // EH is re-raising a managed exception.
        TSNC_WaitUntilGCFinished        = 0x00000010, // The current thread is waiting for GC.  If host returns
                                                      // SO during wait, we will either spin or make GC wait.
        TSNC_SOIntolerant               = 0x00000020, // The thread is in SO intolerant region
        TSNC_SOWorkNeeded               = 0x00000040, // The thread needs to wake up AD unload helper thread to finish SO work
        TSNC_CLRCreatedThread           = 0x00000080, // The thread was created through Thread::CreateNewThread
        TSNC_ExistInThreadStore         = 0x00000100, // For dtor to know if it needs to be removed from ThreadStore
        TSNC_UnsafeSkipEnterCooperative = 0x00000200, // This is a "fix" for deadlocks caused when cleaning up COM
        TSNC_OwnsSpinLock               = 0x00000400, // The thread owns a spinlock.
        TSNC_PreparingAbort             = 0x00000800, // Preparing abort.  This avoids recursive HandleThreadAbort call.
        TSNC_OSAlertableWait            = 0x00001000, // Preparing abort.  This avoids recursive HandleThreadAbort call.
        TSNC_ADUnloadHelper             = 0x00002000, // This thread is AD Unload helper.
        TSNC_CreatingTypeInitException  = 0x00004000, // Thread is trying to create a TypeInitException
        TSNC_InTaskSwitch               = 0x00008000, // A task is switching
        TSNC_AppDomainContainUnhandled  = 0x00010000, // Used to control how unhandled exception reporting occurs.
                                                      // See detailed explanation for this bit in threads.cpp
        TSNC_InRestoringSyncBlock       = 0x00020000, // The thread is restoring its SyncBlock for Object.Wait.
                                                      // After the thread is interrupted once, we turn off interruption
                                                      // at the beginning of wait.
        TSNC_DisableOleaut32Check       = 0x00040000, // Disable oleaut32 delay load check.  Oleaut32 has been loaded
        TSNC_CannotRecycle              = 0x00080000, // A host can not recycle this Thread object.  When a thread has orphaned lock, we will apply this.
        TSNC_RaiseUnloadEvent           = 0x00100000, // Finalize thread is raising managed unload event which may call AppDomain.Unload.
    };

    // Functions called by host
    STDMETHODIMP    QueryInterface(REFIID riid, void** ppv)
        DAC_EMPTY_RET(E_NOINTERFACE);
    STDMETHODIMP_(ULONG) AddRef(void)
        DAC_EMPTY_RET(0);
    STDMETHODIMP_(ULONG) Release(void)
        DAC_EMPTY_RET(0);
    STDMETHODIMP SwitchIn(HANDLE threadHandle)
        DAC_EMPTY_RET(E_FAIL);
    STDMETHODIMP SwitchOut()
        DAC_EMPTY_RET(E_FAIL);
    STDMETHODIMP Reset (BOOL fFull)
        DAC_EMPTY_RET(E_FAIL);
    STDMETHODIMP GetMemStats(COR_GC_THREAD_STATS *memUsage)
        DAC_EMPTY_RET(E_FAIL);
    STDMETHODIMP ExitTask()
        DAC_EMPTY_RET(E_FAIL);
    STDMETHODIMP Abort()
        DAC_EMPTY_RET(E_FAIL);
    STDMETHODIMP RudeAbort()
        DAC_EMPTY_RET(E_FAIL);
    STDMETHODIMP NeedsPriorityScheduling(BOOL *pbNeedsPriorityScheduling)
        DAC_EMPTY_RET(E_FAIL);

    STDMETHODIMP YieldTask()
        DAC_EMPTY_RET(E_FAIL);
    STDMETHODIMP LocksHeld(SIZE_T *pLockCount)
        DAC_EMPTY_RET(E_FAIL);
    STDMETHODIMP SetTaskIdentifier(TASKID asked)
        DAC_EMPTY_RET(E_FAIL);

    STDMETHODIMP SetLocale(LCID lcid);
    STDMETHODIMP SetUILocale(LCID lcid);

    void InternalReset (BOOL fFull);
    void InternalSwitchOut();

    HRESULT DetachThread(BOOL fDLLThreadDetach);

    void SetThreadState(ThreadState ts)
    {
        LEAF_CONTRACT;
        FastInterlockOr((DWORD*)&m_State, ts);
    }

    void ResetThreadState(ThreadState ts)
    {
        LEAF_CONTRACT;
        FastInterlockAnd((DWORD*)&m_State, ~ts);
    }

    BOOL HasThreadState(ThreadState ts)
    {
        LEAF_CONTRACT;
        return ((DWORD)m_State & ts);
    }

    void SetThreadStateNC(ThreadStateNoConcurrency tsnc)
    {
        LEAF_CONTRACT;
#ifdef _DEBUG
#ifndef DACCESS_COMPILE
        if (tsnc & TSNC_SOIntolerant)
        {
            AddFiberInfo(Thread::ThreadTrackInfo_SO);
        }
#endif
#endif
        m_StateNC = (ThreadStateNoConcurrency)((DWORD)m_StateNC | tsnc);
    }

    void ResetThreadStateNC(ThreadStateNoConcurrency tsnc)
    {
        LEAF_CONTRACT;
#ifdef _DEBUG
#ifndef DACCESS_COMPILE
        if (tsnc & TSNC_SOIntolerant)
        {
            AddFiberInfo(Thread::ThreadTrackInfo_SO);
        }
#endif
#endif
        m_StateNC = (ThreadStateNoConcurrency)((DWORD)m_StateNC & ~tsnc);
    }

    BOOL HasThreadStateNC(ThreadStateNoConcurrency tsnc)
    {
        LEAF_CONTRACT;
        return ((DWORD)m_StateNC & tsnc);
    }

    DWORD RequireSyncBlockCleanup()
    {
        LEAF_CONTRACT;
        return (m_ThreadTasks & TT_CleanupSyncBlock);
    }

    void SetSyncBlockCleanup()
    {
        LEAF_CONTRACT;
        FastInterlockOr((ULONG *)&m_ThreadTasks, TT_CleanupSyncBlock);
    }

    void ResetSyncBlockCleanup()
    {
        LEAF_CONTRACT;
        FastInterlockAnd((ULONG *)&m_ThreadTasks, ~TT_CleanupSyncBlock);
    }


    // returns if there is some extra work for the finalizer thread.
    BOOL HaveExtraWorkForFinalizer();

    // do the extra finalizer work.
    void DoExtraWorkForFinalizer();

#ifndef DACCESS_COMPILE
    DWORD CatchAtSafePoint()
    {
        LEAF_CONTRACT;
        return (m_State & TS_CatchAtSafePoint) || g_fSuspendOnShutdown;
    }
#endif // DACCESS_COMPILE

    DWORD IsBackground()
    {
        LEAF_CONTRACT;
        return (m_State & TS_Background);
    }

    DWORD IsUnstarted()
    {
        LEAF_CONTRACT;
        return (m_State & TS_Unstarted);
    }

    DWORD IsDead()
    {
        LEAF_CONTRACT;
        return (m_State & TS_Dead);
    }

    DWORD DoWeOwn()
    {
        LEAF_CONTRACT;
        return (m_State & TS_WeOwn);
    }

    // For reporting purposes, grab a consistent snapshot of the thread's state
    ThreadState GetSnapshotState();

    // For delayed destruction of threads
    DWORD           IsDetached()
    {
        LEAF_CONTRACT;
        return (m_State & TS_Detached);
    }

    BOOL IsSOTolerant()
    {
        return !HasThreadStateNC(TSNC_SOIntolerant);
    }

    static LONG     m_DetachCount;
    static LONG     m_ActiveDetachCount;  // Count how many non-background detached

    static volatile LONG     m_threadsAtUnsafePlaces;

    // Offsets for the following variables need to fit in 1 byte, so keep near
    // the top of the object.  Also, we want cache line filling to work for us
    // so the critical stuff is ordered based on frequency of use.

    volatile ThreadState m_State;   // Bits for the state of the thread

    // If TRUE, GC is scheduled cooperatively with this thread.
    // NOTE: This "byte" is actually a boolean - we don't allow
    // recursive disables.
    volatile ULONG       m_fPreemptiveGCDisabled;

    PTR_Frame            m_pFrame;  // The Current Frame
    PTR_Frame            m_pUnloadBoundaryFrame;

    // Roots to scan
    CLRRootBase         *m_pRoot;

    // Track the number of locks (critical section, spin lock, syncblock lock,
    // EE Crst, GC lock) held by the current thread.
    DWORD                m_dwLockCount;

    DWORD                m_ThreadId;

    // This member used to be declared further down but that resulted in varying
    // offsets depending on various conditions (debug/release, STUBS_AS_IL, etc.)
    // and a fixed offset was needed to allow it to be hard-coded into stubs.
    IHostTask           *m_pHostTask;


    // RWLock state
    LockEntry           *m_pHead;
    LockEntry            m_embeddedEntry;

#ifndef DACCESS_COMPILE
    Frame* NotifyFrameChainOfExceptionUnwind(Frame* pStartFrame, LPVOID pvLimitSP);
#endif // DACCESS_COMPILE



    // The context within which this thread is executing.  As the thread crosses
    // context boundaries, the context mechanism adjusts this so it's always
    // current.
    // The address of the context object is also used as the ContextID!
    PTR_Context          m_Context;

    // on MP systems, each thread has its own allocation chunk so we can avoid
    // lock prefixes and expensive MP cache snooping stuff
    alloc_context        m_alloc_context;


    // Allocator used during marshaling for temporary buffers, much faster than
    // heap allocation.
    //
    // Uses of this allocator should be effectively statically scoped, i.e. a "region"
    // is started using a CheckPointHolder and GetCheckpoint, and this region can then be used for allocations
    // from that point onwards, and then all memory is reclaimed when the static scope for the
    // checkpoint is exited by the running thread.
    StackingAllocator    m_MarshalAlloc;

    // Flags used to indicate tasks the thread has to do.
    ThreadTasks          m_ThreadTasks;

    // Flags for thread states that have no concurrency issues.
    ThreadStateNoConcurrency m_StateNC;

#ifdef CALLDESCR_RETBUF
    BYTE            m_SmallVCRetVal[ENREGISTERED_RETURNTYPE_MAXSIZE];
#endif

    inline void IncLockCount();
    inline void DecLockCount();

private:
    DWORD m_dwBeginLockCount;  // lock count when the thread enters current domain
    DWORD m_dwBeginCriticalRegionCount;  // lock count when the thread enters current domain
    DWORD m_dwNonHostLockCount;
    DWORD m_dwCriticalRegionCount;
    DWORD m_dwDelayAbortCount;

    DWORD m_dwThreadAffinityCount;

#ifdef _DEBUG
    DWORD m_dwSuspendThread;
    EEThreadId m_Creater;
#endif

    // After we suspend a thread, we may need to call EEJitManager::JitCodeToMethodInfo
    // or StressLog which may waits on a spinlock.  It is unsafe to suspend a thread while it
    // is in this state.
    volatile LONG m_dwForbidSuspendThread;

public:
    
    static void IncForbidSuspendThread(StateHolderParam)
    {
        Thread *pThread = GetThread();
        if (pThread)
        {
            _ASSERTE (pThread->m_dwForbidSuspendThread != MAXLONG);
#ifdef _DEBUG
            STRESS_LOG2(LF_SYNC, LL_INFO1000, "Set forbid suspend [%d] for thread %p.\n", pThread->m_dwForbidSuspendThread, pThread);
#endif
            FastInterlockIncrement(&pThread->m_dwForbidSuspendThread);
        }
    }

    static void DecForbidSuspendThread(StateHolderParam)
    {
        Thread *pThread = GetThread();
        if (pThread)
        {
            _ASSERTE (pThread->m_dwForbidSuspendThread != 0);
            FastInterlockDecrement(&pThread->m_dwForbidSuspendThread);
#ifdef _DEBUG
            STRESS_LOG2(LF_SYNC, LL_INFO1000, "Reset forbid suspend [%d] for thread %p.\n", pThread->m_dwForbidSuspendThread, pThread);
#endif
        }
    }
    
    bool IsInForbidSuspendRegion()
    {
        return m_dwForbidSuspendThread != 0;
    }
    
    // The ForbidSuspendThreadHolder is used during the initialization of the stack marker infrastructure so
    // it can't do any backout stack validation (which is why we pass in VALIDATION_TYPE=HSV_NoValidation).
    typedef StateHolder<Thread::IncForbidSuspendThread, Thread::DecForbidSuspendThread, HSV_NoValidation> ForbidSuspendThreadHolder;

private:
    // Per thread counter to dispense hash code - kept in the thread so we don't need a lock
    // or interlocked operations to get a new hash code;
    DWORD m_dwHashCodeSeed;

    // Lock thread is trying to acquire
    DeadlockAwareLock   * volatile m_pBlockingLock;

public:

    inline BOOL HasLockInCurrentDomain()
    {
        LEAF_CONTRACT;

        DWORD count = m_dwLockCount + m_dwCriticalRegionCount - m_dwBeginLockCount - m_dwBeginCriticalRegionCount;

        _ASSERTE (count >= 0);

        return count;
    }

    inline void IncNonHostLockCount()
    {
        LEAF_CONTRACT;
        _ASSERTE (GetThread() == this);
        m_dwNonHostLockCount ++;
        _ASSERTE (m_dwNonHostLockCount != 0);
    }
    inline void DecNonHostLockCount()
    {
        LEAF_CONTRACT;
        _ASSERTE (GetThread() == this);
        _ASSERTE (m_dwNonHostLockCount > 0);
        m_dwNonHostLockCount --;
    }
    inline BOOL HasNonHostLockCount()
    {
        LEAF_CONTRACT;
        return m_dwNonHostLockCount != 0;
    }

    inline void BeginCriticalRegion()
    {
        LEAF_CONTRACT;
        _ASSERTE (GetThread() == this);
        m_dwCriticalRegionCount ++;
        _ASSERTE (m_dwCriticalRegionCount != 0);
    }
    inline void EndCriticalRegion()
    {
        LEAF_CONTRACT;
        _ASSERTE (GetThread() == this);
        _ASSERTE (m_dwCriticalRegionCount > 0);
        m_dwCriticalRegionCount --;
    }
    inline BOOL HasCriticalRegion()
    {
        LEAF_CONTRACT;
        return m_dwCriticalRegionCount != 0;
    }

    inline DWORD GetNewHashCode()
    {
        LEAF_CONTRACT;
        // Every thread has its own generator for hash codes so that we won't get into a situation
        // where two threads consistently give out the same hash codes.
        // Choice of multiplier guarantees period of 2**32 - see Knuth Vol 2 p16 (3.2.1.2 Theorem A).
        DWORD multiplier = m_ThreadId*4 + 5;
        m_dwHashCodeSeed = m_dwHashCodeSeed*multiplier + 1;
        return m_dwHashCodeSeed;
    }

#ifdef _DEBUG
    // If the current thread suspends other threads, we need to make sure that the thread
    // only allocates memory if the suspended threads do not have OS Heap lock.
    static BOOL AllowCallout()
    {
        LEAF_CONTRACT;
        Thread *pThread = GetThread();
        return pThread == NULL || pThread->m_dwSuspendThread == 0;
    }

    // Returns number of threads that are currently suspended by the current thread 
    DWORD GetSuspendeeCount()
    {
        LEAF_CONTRACT;
        return m_dwSuspendThread;
    }
    
#endif

public:
    static void __stdcall LeaveRuntime(size_t target);
    static HRESULT LeaveRuntimeNoThrow(size_t target);
    static void __stdcall LeaveRuntimeThrowComplus(size_t target);
    static void __stdcall EnterRuntime();
    static HRESULT EnterRuntimeNoThrow();

    // Reverse PInvoke hook for host
    static void ReverseEnterRuntime();
    static HRESULT ReverseEnterRuntimeNoThrow();
    static void ReverseEnterRuntimeThrowComplusHelper(HRESULT hr);
    static void ReverseEnterRuntimeThrowComplus();
    static void ReverseLeaveRuntime();

    static void ReverseEnterRuntimeForHolder(StateHolderParam)
    {
        WRAPPER_CONTRACT;
        ReverseEnterRuntime();
    }
    static void ReverseLeaveRuntimeForHolder(StateHolderParam)
    {
        WRAPPER_CONTRACT;
        ReverseLeaveRuntime();
    }

    inline void IncThreadAffinityCount()
    {
        LEAF_CONTRACT;
        _ASSERTE (GetThread() == this);
        m_dwThreadAffinityCount++;
        _ASSERTE (m_dwThreadAffinityCount > 0);
    }
    inline void DecThreadAffinityCount()
    {
        LEAF_CONTRACT;
        _ASSERTE (GetThread() == this);
        _ASSERTE (m_dwThreadAffinityCount > 0);
        m_dwThreadAffinityCount --;
    }

    // Hook for OS Critical Section, Mutex, and others that require thread affinity
    static void BeginThreadAffinity();
    static void EndThreadAffinity();

    static void BeginThreadAffinityForHolder(StateHolderParam)
    {
        WRAPPER_CONTRACT;
        BeginThreadAffinity();
    }
    static void EndThreadAffinityForHolder(StateHolderParam)
    {
        WRAPPER_CONTRACT;
        EndThreadAffinity();
    }

    static void BeginThreadAffinityAndCriticalRegion()
    {
        WRAPPER_CONTRACT;
        BeginThreadAffinity();
        GetThread()->BeginCriticalRegion();
    }

    static void BeginThreadAffinityAndCriticalRegionForHolder(StateHolderParam)
    {
        WRAPPER_CONTRACT;
        BeginThreadAffinity();
        GetThread()->BeginCriticalRegion();
    }

    static void EndThreadAffinityAndCriticalRegionForHolder(StateHolderParam)
    {
        WRAPPER_CONTRACT;
        GetThread()->EndCriticalRegion();
        EndThreadAffinity();
    }

    BOOL HasThreadAffinity()
    {
        LEAF_CONTRACT;
        return m_dwThreadAffinityCount > 0;
    }

    void BeginDelayAbort();
    void EndDelayAbort();
    BOOL DelayAbortEnabled() const
    {
        LEAF_CONTRACT;
        return m_dwDelayAbortCount != 0;
    }

 private:
    LoadLevelLimiter *m_pLoadLimiter;

 public:
    LoadLevelLimiter *GetLoadLevelLimiter()
    {
        LEAF_CONTRACT;
        return m_pLoadLimiter;
    }

    void SetLoadLevelLimiter(LoadLevelLimiter *limiter)
    {
        LEAF_CONTRACT;
        m_pLoadLimiter = limiter;
    }



public:
    inline BOOL  IsWorkerThread()
    {
        LEAF_CONTRACT;
        return (m_State & TS_TPWorkerThread);
    }

    //--------------------------------------------------------------
    // Constructor.
    //--------------------------------------------------------------
#ifndef DACCESS_COMPILE
    Thread();
#endif

    //--------------------------------------------------------------
    // Failable initialization occurs here.
    //--------------------------------------------------------------
    BOOL InitThread(BOOL fInternal);
    BOOL AllocHandles();

    void SetupThreadForHost();

    //--------------------------------------------------------------
    // If the thread was setup through SetupUnstartedThread, rather
    // than SetupThread, complete the setup here when the thread is
    // actually running.
    //--------------------------------------------------------------
    BOOL HasStarted();

    // We don't want ::CreateThread() calls scattered throughout the source.
    // Create all new threads here.  The thread is created as suspended, so
    // you must ::ResumeThread to kick it off.  It is guaranteed to create the
    // thread, or throw.
    BOOL CreateNewThread(SIZE_T stackSize, LPTHREAD_START_ROUTINE start, void *args);


    //--------------------------------------------------------------
    // Destructor
    //--------------------------------------------------------------
#ifndef DACCESS_COMPILE
    ~Thread();
#endif


    void        OnThreadTerminate(BOOL holdingLock);

    static void CleanupDetachedThreads(GCHeap::SUSPEND_REASON reason, EEIntHashTable *pDLSItems[], int cleanupArrSize);
    //--------------------------------------------------------------
    // Returns innermost active Frame.
    //--------------------------------------------------------------
    Frame *GetFrame()
    {
#ifndef DACCESS_COMPILE
#ifdef _DEBUG_IMPL
        WRAPPER_CONTRACT;
        if (this == GetThread())
        {
            void* curSP;
            curSP = GetCurrentSP();
            _ASSERTE(curSP <= m_pFrame || m_pFrame == (Frame*) -1);
        }
#else
        LEAF_CONTRACT;
        _ASSERTE(!"NYI");
#endif
#endif // #ifndef DACCESS_COMPILE
        return m_pFrame;
    }

    //--------------------------------------------------------------
    // Replaces innermost active Frames.
    //--------------------------------------------------------------
#ifndef DACCESS_COMPILE
    void  SetFrame(Frame *pFrame)
#ifdef _DEBUG
        ;
#else
    {
        LEAF_CONTRACT;
        m_pFrame = pFrame;
    }
#endif
    ;
#endif
    inline Frame* FindFrame(SIZE_T StackPointer);

    bool DetectHandleILStubsForDebugger();

#ifndef DACCESS_COMPILE
    void  SetUnloadBoundaryFrame(Frame *pFrame);
    void  ResetUnloadBoundaryFrame();
#endif

    Frame *GetUnloadBoundaryFrame()
    {
        LEAF_CONTRACT;
        return m_pUnloadBoundaryFrame;
    }

    void SetWin32FaultAddress(DWORD eip)
    {
        LEAF_CONTRACT;
        m_Win32FaultAddress = eip;
    }

    void SetWin32FaultCode(DWORD code)
    {
        LEAF_CONTRACT;
        m_Win32FaultCode = code;
    }

    DWORD GetWin32FaultAddress()
    {
        LEAF_CONTRACT;
        return m_Win32FaultAddress;
    }

    DWORD GetWin32FaultCode()
    {
        LEAF_CONTRACT;
        return m_Win32FaultCode;
    }


    //**************************************************************
    // GC interaction
    //**************************************************************

    //--------------------------------------------------------------
    // Enter cooperative GC mode. NOT NESTABLE.
    //--------------------------------------------------------------
    void DisablePreemptiveGC()
    {
#ifndef DACCESS_COMPILE
        WRAPPER_CONTRACT;
        _ASSERTE(this == GetThread());
        _ASSERTE(!m_fPreemptiveGCDisabled);
        // holding a spin lock in preemp mode and transit to coop mode will cause other threads
        // spinning waiting for GC
        _ASSERTE ((m_StateNC & Thread::TSNC_OwnsSpinLock) == 0);


        // Logically, we just want to check whether a GC is in progress and halt
        // at the boundary if it is -- before we disable preemptive GC.  However
        // this opens up a race condition where the GC starts after we make the
        // check.  SysSuspendForGC will ignore such a thread because it saw it as
        // outside the EE.  So the thread would run wild during the GC.
        //
        // Instead, enter cooperative mode and then check if a GC is in progress.
        // If so, go back out and try again.  The reason we go back out before we
        // try again, is that SysSuspendForGC might have seen us as being in
        // cooperative mode if it checks us between the next two statements.
        // In that case, it will be trying to move us to a safe spot.  If
        // we don't let it see us leave, it will keep waiting on us indefinitely.

        // ------------------------------------------------------------------------
        //   ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING **  |
        // ------------------------------------------------------------------------
        //
        //   DO NOT CHANGE THIS METHOD WITHOUT VISITING ALL THE STUB GENERATORS
        //   THAT EFFECTIVELY INLINE IT INTO THEIR STUBS
        //
        // ------------------------------------------------------------------------
        //   ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING **  |
        // ------------------------------------------------------------------------

        m_fPreemptiveGCDisabled = 1;

        if (g_TrapReturningThreads)
        {
            RareDisablePreemptiveGC();
        }
#else
        LEAF_CONTRACT;
#endif
    }

    void RareDisablePreemptiveGC();

    void HandleThreadAbort()
    {
        HandleThreadAbort(FALSE);
    }
    void HandleThreadAbort(BOOL fForce);  // fForce=TRUE only for a thread waiting to start AD unload

    void PreWorkForThreadAbort();

private:
    void HandleThreadAbortTimeout();

public:
    //--------------------------------------------------------------
    // Leave cooperative GC mode. NOT NESTABLE.
    //--------------------------------------------------------------
    void EnablePreemptiveGC()
    {
#ifndef DACCESS_COMPILE
        WRAPPER_CONTRACT;
        _ASSERTE(this == GetThread());
        _ASSERTE(m_fPreemptiveGCDisabled);
        // holding a spin lock in coop mode and transit to preemp mode will cause deadlock on GC
        _ASSERTE ((m_StateNC & Thread::TSNC_OwnsSpinLock) == 0);


        // ------------------------------------------------------------------------
        //   ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING **  |
        // ------------------------------------------------------------------------
        //
        //   DO NOT CHANGE THIS METHOD WITHOUT VISITING ALL THE STUB GENERATORS
        //   THAT EFFECTIVELY INLINE IT INTO THEIR STUBS
        //
        // ------------------------------------------------------------------------
        //   ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING **  |
        // ------------------------------------------------------------------------

        m_fPreemptiveGCDisabled = 0;

        if (CatchAtSafePoint())
            RareEnablePreemptiveGC();
#else
        LEAF_CONTRACT;
#endif
    }

#if defined(STRESS_HEAP) && defined(_DEBUG_IMPL)
    void PerformPreemptiveGC();
#endif
    void RareEnablePreemptiveGC();
    void PulseGCMode();

    //--------------------------------------------------------------
    // Query mode
    //--------------------------------------------------------------
    BOOL PreemptiveGCDisabled()
    {
        WRAPPER_CONTRACT;
        _ASSERTE(this == GetThread());
        return (PreemptiveGCDisabledOther());
    }

    BOOL PreemptiveGCDisabledOther()
    {
        LEAF_CONTRACT;
        return (m_fPreemptiveGCDisabled);
    }


    //---------------------------------------------------------------
    // Expose key offsets and values for stub generation.
    //---------------------------------------------------------------
    static BYTE GetOffsetOfCurrentFrame()
    {
        LEAF_CONTRACT;
        size_t ofs = offsetof(class Thread, m_pFrame);
        _ASSERTE(FitsInI1(ofs));
        return (BYTE)ofs;
    }

    static BYTE GetOffsetOfState()
    {
        LEAF_CONTRACT;
        size_t ofs = offsetof(class Thread, m_State);
        _ASSERTE(FitsInI1(ofs));
        return (BYTE)ofs;
    }

    static BYTE GetOffsetOfGCFlag()
    {
        LEAF_CONTRACT;
        size_t ofs = offsetof(class Thread, m_fPreemptiveGCDisabled);
        _ASSERTE(FitsInI1(ofs));
        return (BYTE)ofs;
    }

    // The address of the context object is also used as the ContextID
    static BYTE GetOffsetOfContextID()
    {
        LEAF_CONTRACT;
        size_t ofs = offsetof(class Thread, m_Context);
        _ASSERTE(FitsInI1(ofs));
        return (BYTE)ofs;
    }

    static void StaticDisablePreemptiveGC( Thread *pThread)
    {
        WRAPPER_CONTRACT;
        _ASSERTE(pThread != NULL);
        pThread->DisablePreemptiveGC();
    }

    static void StaticEnablePreemptiveGC( Thread *pThread)
    {
        WRAPPER_CONTRACT;
        _ASSERTE(pThread != NULL);
        pThread->EnablePreemptiveGC();
    }


    //---------------------------------------------------------------
    // Expose offset of the app domain word for the interop and delegate callback
    //---------------------------------------------------------------
    static SIZE_T GetOffsetOfAppDomain()
    {
        LEAF_CONTRACT;
        return (SIZE_T)(offsetof(class Thread, m_pDomain));
    }

    //---------------------------------------------------------------
    // Expose offset of the place for storing the filter context for the debugger.
    //---------------------------------------------------------------
    static SIZE_T GetOffsetOfDebuggerFilterContext()
    {
        LEAF_CONTRACT;
        return (SIZE_T)(offsetof(class Thread, m_debuggerFilterContext));
    }

    //---------------------------------------------------------------
    // Expose offset of the debugger word for the debugger
    //---------------------------------------------------------------
    static SIZE_T GetOffsetOfDebuggerWord()
    {
        LEAF_CONTRACT;
        return (SIZE_T)(offsetof(class Thread, m_debuggerWord));
    }

    //---------------------------------------------------------------
    // Expose offset of the debugger cant stop count for the debugger
    //---------------------------------------------------------------
    static SIZE_T GetOffsetOfCantStop()
    {
        LEAF_CONTRACT;
        return (SIZE_T)(offsetof(class Thread, m_debuggerCantStop));
    }

    //---------------------------------------------------------------
    // Expose offset of m_StateNC
    //---------------------------------------------------------------
    static SIZE_T GetOffsetOfStateNC()
    {
        LEAF_CONTRACT;
        return (SIZE_T)(offsetof(class Thread, m_StateNC));
    }

    //---------------------------------------------------------------
    // Last exception to be thrown
    //---------------------------------------------------------------
    inline void SetThrowable(OBJECTREF pThrowable 
                             DEBUG_ARG(ThreadExceptionState::SetThrowableErrorChecking stecFlags = ThreadExceptionState::STEC_All));

    OBJECTREF GetThrowable()
    {
        WRAPPER_CONTRACT;

        return m_ExceptionState.GetThrowable();
    }

    // An unmnaged thread can check if a managed is processing an exception
    BOOL HasException()
    {
        LEAF_CONTRACT;
        OBJECTHANDLE pThrowable = m_ExceptionState.GetThrowableAsHandle();
        if (pThrowable && *PTR_UNCHECKED_OBJECTREF(pThrowable))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    OBJECTHANDLE GetThrowableAsHandle()
    {
        LEAF_CONTRACT;
        return m_ExceptionState.GetThrowableAsHandle();
    }

    BOOL IsThrowableNull()
    {
        WRAPPER_CONTRACT;
        return IsHandleNullUnchecked(m_ExceptionState.GetThrowableAsHandle());
    }

    BOOL IsExceptionInProgress()
    {
        LEAF_CONTRACT;
        return m_ExceptionState.IsExceptionInProgress();
    }


    //---------------------------------------------------------------
    // Per-thread information used by handler
    //---------------------------------------------------------------
    // exception handling info stored in thread
    // can't allocate this as needed because can't make exception-handling depend upon memory allocation

    ThreadExceptionState* GetExceptionState()
    {
        LEAF_CONTRACT;
        return &m_ExceptionState;
    }

    // Access to the Context this thread is executing in.
    Context *GetContext()
    {
        LEAF_CONTRACT;
#ifndef DACCESS_COMPILE

        // if another thread is asking about our thread, we could be in the middle of an AD transition so
        // the context and AD may not match if have set one but not the other. Can live without checking when
        // another thread is asking it as this method is mostly called on our own thread so will mostly get the
        // checking. If are int the middle of a transition, this could return either the old or the new AD.
        // But no matter what we do, such as lock on the transition, by the time are done could still have
        // changed right after we asked, so really no point.
        _ASSERTE(this != GetThread() || (m_Context == NULL && m_pDomain == NULL) || m_Context->GetDomain() == m_pDomain || g_fEEShutDown);
#endif // DACCESS_COMPILE
        return m_Context;
    }

    void SetExposedContext(Context *c);

    // This callback is used when we are executing in the EE and discover that we need
    // to switch appdomains.
    void DoADCallBack(ADID appDomain , Context::ADCallBackFcnType pTarget, LPVOID args);
    void DoADCallBack(AppDomain* pDomain , Context::ADCallBackFcnType pTarget, LPVOID args, DWORD dwADV);
    void DoContextCallBack(ADID appDomain, Context* c , Context::ADCallBackFcnType pTarget, LPVOID args);

    // Except for security and the call in from the remoting code in mscorlib, you should never do an
    // AppDomain transition directly through these functions. Rather, you should use DoADCallBack above
    // to call into managed code to perform the transition for you so that the correct policy code etc
    // is run on the transition,
    void EnterContextRestricted(Context* c, ContextTransitionFrame* pFrame);
    void ReturnToContext(ContextTransitionFrame *pFrame);

private:
    typedef enum {
        RaiseCrossContextSuccess,
        RaiseCrossContextRetry,
        RaiseCrossContextClassInit
    } RaiseCrossContextResult;

    RaiseCrossContextResult TryRaiseCrossContextException(Exception **ppExOrig,
                                                          Exception *pException,
                                                          RuntimeExceptionKind *pKind,
                                                          OBJECTREF *ppThrowable,
                                                          U1ARRAYREF *pOrBlob);
public:

    void DECLSPEC_NORETURN RaiseCrossContextException(Exception* pEx, ContextTransitionFrame* pFrame);
    void RaiseCrossContextExceptionHelper(Exception* pEx,ContextTransitionFrame* pFrame);
    

    // ClearContext are to be called only during shutdown
    void ClearContext();

    // Used by security to prevent recursive stackwalking.
    BOOL IsSecurityStackwalkInProgess()
    {
        LEAF_CONTRACT;
        return m_fSecurityStackwalk;
    }

    void SetSecurityStackwalkInProgress(BOOL fSecurityStackwalk)
    {
        LEAF_CONTRACT;
        m_fSecurityStackwalk = fSecurityStackwalk;
    }

private:
    void ReturnToContextAndThrow(ContextTransitionFrame* pFrame, EEException* pEx, BOOL* pContextSwitched);
    void ReturnToContextAndOOM(ContextTransitionFrame* pFrame);

private:
    // don't ever call these except when creating thread!!!!!
    void InitContext();

    BOOL m_fSecurityStackwalk;

public:
    AppDomain* GetDomain(INDEBUG(BOOL fMidContextTransitionOK = FALSE))
    {
        STATIC_CONTRACT_SO_TOLERANT;
        // if another thread is asking about our thread, we could be in the middle of an AD transition so
        // the context and AD may not match if have set one but not the other. Can live without checking when
        // another thread is asking it as this method is mostly called on our own thread so will mostly get the
        // checking. If are int the middle of a transition, this could return either the old or the new AD.
        // But no matter what we do, such as lock on the transition, by the time are done could still have
        // changed right after we asked, so really no point.
#ifdef _DEBUG_IMPL
        WRAPPER_CONTRACT;
        if (!g_fEEShutDown && this == GetThread())
        {
            if (!fMidContextTransitionOK)
            {
                // We also want to suppress the "domain on context == domain on thread" check if this might
                // be called during a context or AD transition (in which case fMidContextTransitionOK is nonzero).
                // A profiler stackwalk can occur at arbitrary times, including during these transitions, but
                // the stackwalk is still safe to do at this point, so we don't want to trigger this assert.
                _ASSERTE((m_Context == NULL && m_pDomain == NULL) || m_Context->GetDomain() == m_pDomain);
            }
            AppDomain* valueInTLSSlot = GetAppDomain();
            _ASSERTE(valueInTLSSlot == 0 || valueInTLSSlot == m_pDomain);
        }
#else
        LEAF_CONTRACT;
#endif
        return m_pDomain;
    }

    Frame *IsRunningIn(AppDomain* pDomain, int *count);
    Frame *GetFirstTransitionInto(AppDomain *pDomain, int *count);

    BOOL ShouldChangeAbortToUnload(Frame *pFrame, Frame *pUnloadBoundaryFrame=NULL);

    // Get outermost (oldest) AppDomain for this thread.
    AppDomain *GetInitialDomain();

    //---------------------------------------------------------------
    // Track use of the thread block.  See the general comments on
    // thread destruction in threads.cpp, for details.
    //---------------------------------------------------------------
    int         IncExternalCount();
    int         DecExternalCount(BOOL holdingLock);


    //---------------------------------------------------------------
    // !!!! THESE ARE NOT SAFE FOR GENERAL USE  !!!!
    //      IncExternalCountDANGEROUSProfilerOnly()
    //      DecExternalCountDANGEROUSProfilerOnly()
    // Currently only the profiler API should be using these
    // functions, because the profiler is responsible for ensuring
    // that the thread exists, undestroyed, before operating on it.
    // All other clients should use IncExternalCount/DecExternalCount
    // instead
    //---------------------------------------------------------------
    int         IncExternalCountDANGEROUSProfilerOnly()
    {
        LEAF_CONTRACT;

#ifdef _DEBUG
        int cRefs =
#else   // _DEBUG
        return
#endif //_DEBUG
            FastInterlockIncrement((LONG*)&m_ExternalRefCount);

#ifdef _DEBUG
        // This should never be called on a thread being destroyed
        _ASSERTE(cRefs != 1);
        return cRefs;
#endif //_DEBUG
    }

    int         DecExternalCountDANGEROUSProfilerOnly()
    {
        LEAF_CONTRACT;
#ifdef _DEBUG
        int cRefs =
#else   // _DEBUG
        return
#endif //_DEBUG

            FastInterlockDecrement((LONG*)&m_ExternalRefCount);

#ifdef _DEBUG
        // This should never cause the last reference on the thread to be released
        _ASSERTE(cRefs != 0);
        return cRefs;
#endif //_DEBUG
    }

    // Get and Set the exposed System.Thread object which corresponds to
    // this thread.  Also the thread handle and Id.
    OBJECTREF   GetExposedObject();
    OBJECTREF   GetExposedObjectRaw();
    void        SetExposedObject(OBJECTREF exposed);
    OBJECTHANDLE GetExposedObjectHandleForDebugger()
    {
        LEAF_CONTRACT;
        return m_ExposedObject;
    }

    // Query whether the exposed object exists
    BOOL IsExposedObjectSet()
    {
        WRAPPER_CONTRACT;
        return (ObjectFromHandle(m_ExposedObject) != NULL) ;
    }

    void GetSynchronizationContext(OBJECTREF *pSyncContextObj)
    {
        CONTRACTL
        {
            MODE_COOPERATIVE;
            GC_NOTRIGGER;
            NOTHROW;
            PRECONDITION(CheckPointer(pSyncContextObj));
        }
        CONTRACTL_END;

        *pSyncContextObj = NULL;

        THREADBASEREF ExposedThreadObj = (THREADBASEREF)GetExposedObjectRaw();
        if (ExposedThreadObj != NULL)
            *pSyncContextObj = ExposedThreadObj->GetSynchronizationContext();
    }

    OBJECTREF GetCompressedStack()
    {
        CONTRACTL
        {
            MODE_COOPERATIVE;
            GC_NOTRIGGER;
            NOTHROW;
        }
        CONTRACTL_END;
        THREADBASEREF ExposedThreadObj = (THREADBASEREF)GetExposedObjectRaw();
        if (ExposedThreadObj != NULL)
            return (OBJECTREF)(ExposedThreadObj->GetCompressedStack());
        return NULL;
    }

    // When we create a managed thread, the thread is suspended.  We call StartThread to get
    // the thread start.
    DWORD StartThread();

    // The result of attempting to OS-suspend an EE thread.
    enum SuspendThreadResult
    {
        // We successfully suspended the thread.  This is the only
        // case where the caller should subsequently call ResumeThread.
        STR_Success,

        // The underlying call to the operating system's SuspendThread
        // or GetThreadContext failed.  This is usually taken to mean
        // that the OS thread has exited.  (This can possibly also mean
        // that the suspension count exceeded the allowed maximum, but
        // Thread::SuspendThread asserts that does not happen.)
        STR_Failure,

        // The thread handle is invalid.  This means that the thread
        // is dead (or dying), or that the object has been created for
        // an exposed System.Thread that has not been started yet.
        STR_UnstartedOrDead,

        // The fOneTryOnly flag was set, and we managed to OS suspend the
        // thread, but we found that it had its m_dwForbidSuspendThread
        // flag set.  If fOneTryOnly is not set, Thread::Suspend will
        // retry in this case.
        STR_Forbidden,

        // Stress logging is turned on, but no stress log had been created
        // for the thread yet, and we failed to create one.  This can mean
        // that either we are not allowed to call into the host, or we ran
        // out of memory.
        STR_NoStressLog,

        // The EE thread is currently switched out.  This can only happen
        // if we are hosted and the host schedules EE threads on fibers.
        STR_SwitchedOut,
    };

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
    SuspendThreadResult SuspendThread(BOOL fOneTryOnly = FALSE, DWORD *pdwSuspendCount = NULL);

    DWORD ResumeThread();

    int GetThreadPriority();
    BOOL SetThreadPriority(
        int nPriority   // thread priority level
    );
    BOOL Alert ();
    DWORD Join(DWORD timeout, BOOL alertable);
    DWORD JoinEx(DWORD timeout, WaitMode mode);

    BOOL GetThreadContext(
        LPCONTEXT lpContext   // context structure
    )
    {
        WRAPPER_CONTRACT;
        _ASSERTE (m_pHostTask == NULL || GetThreadHandle() != SWITCHOUT_HANDLE_VALUE);
         return ::GetThreadContext (GetThreadHandle(), lpContext);
    }
    BOOL SetThreadContext(
        CONST CONTEXT *lpContext   // context structure
    )
    {
        WRAPPER_CONTRACT;
        _ASSERTE (m_pHostTask == NULL || GetThreadHandle() != SWITCHOUT_HANDLE_VALUE);
         return ::SetThreadContext (GetThreadHandle(), lpContext);
    }

    BOOL HasValidThreadHandle ()
    {
        WRAPPER_CONTRACT;
        return m_pHostTask != NULL ||
            GetThreadHandle() != INVALID_HANDLE_VALUE;
    }

    DWORD       GetThreadId()
    {
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;
        return m_ThreadId;
    }

    DWORD       GetOSThreadId()
    {
        LEAF_CONTRACT;
#ifndef DACCESS_COMPILE
        _ASSERTE (m_OSThreadId != 0xbaadf00d);
#endif // !DACCESS_COMPILE
        return m_OSThreadId;
    }

    // This API is to be used for Debugger only.
    // We need to be able to return the true value of m_OSThreadId.
    //
    DWORD       GetOSThreadIdForDebugger()
    {
        LEAF_CONTRACT;
        return m_OSThreadId;
    }

    DWORD       GetTaskId()
    {
        LEAF_CONTRACT;
        return m_TaskId;
    }
    CONNID      GetConnectionId()
    {
        LEAF_CONTRACT;
        return m_dwConnectionId;
    }

    IHostTask* GetHostTask() const
    {
        LEAF_CONTRACT;
        return m_pHostTask;
    }

    IHostTask* GetHostTaskWithAddRef();

    void ReleaseHostTask();

    void SetConnectionId(CONNID dwConnectionId)
    {
        LEAF_CONTRACT;
        m_dwConnectionId = dwConnectionId;
    }

    BOOL        IsThreadPoolThread()
    {
        LEAF_CONTRACT;
        return (m_State & TS_ThreadPoolThread);
    }

    // public suspend functions.  System ones are internal, like for GC.  User ones
    // correspond to suspend/resume calls on the exposed System.Thread object.
    void           UserSuspendThread();
    static HRESULT SysSuspendForGC(GCHeap::SUSPEND_REASON reason);
    static void    SysResumeFromGC(BOOL bFinishedGC, BOOL SuspendSucceded);
    static bool    SysStartSuspendForDebug(AppDomain *pAppDomain);
    static bool    SysSweepThreadsForDebug(bool forceSync);
    static void    SysResumeFromDebug(AppDomain *pAppDomain);
    BOOL           UserResumeThread();

    void           UserSleep(INT32 time);

private:
    //
    static bool     s_fSysSuspendInProgress;

    static void SetSysSuspendInProgress(StateHolderParam);
    static void ResetSysSuspendInProgress(StateHolderParam);

    typedef StateHolder<Thread::SetSysSuspendInProgress, Thread::ResetSysSuspendInProgress> SysSuspendInProgressHolder;

public:
    static bool SysIsSuspendInProgress() { return s_fSysSuspendInProgress; }

    // AD unload uses ThreadAbort support.  We need to distinguish pure ThreadAbort and AD unload
    // cases.
    enum ThreadAbortRequester
    {
        TAR_Thread =      0x00000001,   // Request by Thread
        TAR_ADUnload =    0x00000002,   // Request by AD unload
        TAR_FuncEval =    0x00000004,   // Request by Func-Eval
        TAR_ALL = 0xFFFFFFFF,
    };

private:

    //
    // Bit mask for tracking which aborts came in and why.
    //
    enum ThreadAbortInfo
    {
        TAI_ThreadAbort       = 0x00000001,
        TAI_ThreadV1Abort     = 0x00000002,
        TAI_ThreadRudeAbort   = 0x00000004,
        TAI_ADUnloadAbort     = 0x00000008,
        TAI_ADUnloadV1Abort   = 0x00000010,
        TAI_ADUnloadRudeAbort = 0x00000020,
        TAI_FuncEvalAbort     = 0x00000040,
        TAI_FuncEvalV1Abort   = 0x00000080,
        TAI_FuncEvalRudeAbort = 0x00000100,
        TAI_ForADUnloadThread = 0x10000000,     // AD unload thread is working on the thread
    };

    static const DWORD TAI_AnySafeAbort = (TAI_ThreadAbort   |
                                           TAI_ADUnloadAbort |
                                           TAI_FuncEvalAbort
                                          );

    static const DWORD TAI_AnyV1Abort   = (TAI_ThreadV1Abort   |
                                           TAI_ADUnloadV1Abort |
                                           TAI_FuncEvalV1Abort
                                          );

    static const DWORD TAI_AnyRudeAbort = (TAI_ThreadRudeAbort   |
                                           TAI_ADUnloadRudeAbort |
                                           TAI_FuncEvalRudeAbort
                                          );

    static const DWORD TAI_AnyFuncEvalAbort = (TAI_FuncEvalAbort   |
                                           TAI_FuncEvalV1Abort |
                                           TAI_FuncEvalRudeAbort
                                          );


    // Specifies type of thread abort.
    DWORD  m_AbortInfo;
    DWORD  m_AbortType;
    ULONGLONG  m_AbortEndTime;
    ULONGLONG  m_RudeAbortEndTime;
    BOOL   m_fRudeAbortInitiated;
    LONG   m_AbortController;

    static ULONGLONG s_NextSelfAbortEndTime;

    void SetRudeAbortEndTimeFromEEPolicy();

    // This is a spin lock to serialize setting/resetting of AbortType and AbortRequest.
    LONG  m_AbortRequestLock;

    static void  LockAbortRequest(Thread *pThread);
    static void  UnlockAbortRequest(Thread *pThread);

    typedef Holder<Thread*, Thread::LockAbortRequest, Thread::UnlockAbortRequest> AbortRequestLockHolder;

    static void AcquireAbortControl(Thread *pThread)
    {
        LEAF_CONTRACT;
        FastInterlockIncrement (&pThread->m_AbortController);
    }

    static void ReleaseAbortControl(Thread *pThread)
    {
        LEAF_CONTRACT;
        _ASSERTE (pThread->m_AbortController > 0);
        FastInterlockDecrement (&pThread->m_AbortController);
    }

    typedef Holder<Thread*, Thread::AcquireAbortControl, Thread::ReleaseAbortControl> AbortControlHolder;

    BOOL IsBeingAbortedForADUnload()
    {
        LEAF_CONTRACT;
        return (m_AbortInfo & TAI_ForADUnloadThread) != 0;
    }

    void ResetBeginAbortedForADUnload();

public:
#ifdef _DEBUG
    BOOL           m_fRudeAborted;
    DWORD          m_dwAbortPoint;
#endif


public:
    enum UserAbort_Client
    {
        UAC_Normal,
        UAC_Host,       // Called by host through IClrTask::Abort
        UAC_WatchDog,   // Called by ADUnload helper thread
    };

    HRESULT        UserAbort(ThreadAbortRequester requester,
                             EEPolicy::ThreadAbortTypes abortType,
                             DWORD timeout,
                             UserAbort_Client client
                            );


    void           UserResetAbort(ThreadAbortRequester requester)
    {
        InternalResetAbort(requester, FALSE);
    }
    void           EEResetAbort(ThreadAbortRequester requester)
    {
        InternalResetAbort(requester, TRUE);
    }

private:
    void           InternalResetAbort(ThreadAbortRequester requester, BOOL fResetRudeAbort);

    void SetAbortEndTime(ULONGLONG endTime, BOOL fRudeAbort);

public:

    ULONGLONG      GetAbortEndTime()
    {
        return IsRudeAbort()?m_RudeAbortEndTime:m_AbortEndTime;
    }

    // We distinguish interrupting a thread between Thread.Interrupt and other usage.
    // For Thread.Interrupt usage, we will interrupt an alertable wait using the same
    // rule as ReadyForAbort.  Wait in EH clause or CER region is not interrupted.
    // For other usage, we will try to Abort the thread.
    // If we can not do the operation, we will delay until next wait.
    enum ThreadInterruptMode
    {
        TI_Interrupt = 0x00000001,     // Requested by Thread.Interrupt
        TI_Abort     = 0x00000002,     // Requested by Thread.Abort or AppDomain.Unload
    };

private:
    BOOL           ReadyForInterrupt()
    {
        return ReadyForAsyncException(TI_Interrupt);
    }

    BOOL           ReadyForAsyncException(ThreadInterruptMode mode);

public:
    inline BOOL IsYieldRequested()
    {
        LEAF_CONTRACT;
        return (m_State & TS_YieldRequested);
    }

    void           UserInterrupt(ThreadInterruptMode mode);

    void           SetAbortRequest(EEPolicy::ThreadAbortTypes abortType);  // Should only be called by ADUnload
    BOOL           ReadyForAbort()
    {
        return ReadyForAsyncException(TI_Abort);
    }

    BOOL           IsRudeAbort();
    BOOL           IsRudeAbortOnlyForADUnload();
    BOOL           IsRudeUnload();
    BOOL           IsFuncEvalAbort();


    inline BOOL IsAbortRequested()
    {
        LEAF_CONTRACT;
        return (m_State & TS_AbortRequested);
    }

    inline BOOL IsAbortInitiated()
    {
        LEAF_CONTRACT;
        return (m_State & TS_AbortInitiated);
    }

    inline BOOL IsRudeAbortInitiated()
    {
        LEAF_CONTRACT;
        return IsAbortRequested() && m_fRudeAbortInitiated;
    }

    inline void SetAbortInitiated()
    {
        WRAPPER_CONTRACT;
        if (IsRudeAbort()) {
            m_fRudeAbortInitiated = TRUE;
        }
        FastInterlockOr((ULONG *)&m_State, TS_AbortInitiated);
        // The following should be factored better, but I'm looking for a minimal V1 change.
        ResetUserInterrupted();
    }

    inline void ResetAbortInitiated()
    {
        LEAF_CONTRACT;
        FastInterlockAnd((ULONG *)&m_State, ~TS_AbortInitiated);
        m_fRudeAbortInitiated = FALSE;
    }

    inline void SetPreparingAbort()
    {
        WRAPPER_CONTRACT;
        SetThreadStateNC(TSNC_PreparingAbort);
    }

    inline void ResetPreparingAbort()
    {
        WRAPPER_CONTRACT;
        ResetThreadStateNC(TSNC_PreparingAbort);
    }

private:
    inline static void SetPreparingAbortForHolder(StateHolderParam)
    {
        GetThread()->SetPreparingAbort();
    }
    inline static void ResetPreparingAbortForHolder(StateHolderParam)
    {
        GetThread()->ResetPreparingAbort();
    }
    typedef StateHolder<Thread::SetPreparingAbortForHolder, Thread::ResetPreparingAbortForHolder> PreparingAbortHolder;

public:

    inline void SetIsCreatingTypeInitException()
    {
        WRAPPER_CONTRACT;
        SetThreadStateNC(TSNC_CreatingTypeInitException);
    }

    inline void ResetIsCreatingTypeInitException()
    {
        WRAPPER_CONTRACT;
        ResetThreadStateNC(TSNC_CreatingTypeInitException);
    }

    inline BOOL IsCreatingTypeInitException()
    {
        WRAPPER_CONTRACT;
        return HasThreadStateNC(TSNC_CreatingTypeInitException);
    }

private:
    void SetAbortRequestBit();

    void RemoveAbortRequestBit();

public:
    void MarkThreadForAbort(ThreadAbortRequester requester, EEPolicy::ThreadAbortTypes abortType);
    void UnmarkThreadForAbort(ThreadAbortRequester requester, BOOL fForce = TRUE);

private:
    static void ThreadAbortWatchDogHelper(Thread *pThread);

public:
    static void ThreadAbortWatchDog();

    static ULONGLONG GetNextSelfAbortEndTime()
    {
        LEAF_CONTRACT;
        return s_NextSelfAbortEndTime;
    }


    enum InducedThrowReason {
        InducedThreadStop = 1,
        InducedStackOverflow = 2,
        InducedThreadRedirect = 3
    };

    DWORD          m_ThrewControlForThread;     // flag that is set when the thread deliberately raises an exception for stop/abort

    inline DWORD ThrewControlForThread()
    {
        LEAF_CONTRACT;
        return m_ThrewControlForThread;
    }

    inline void SetThrowControlForThread(InducedThrowReason reason)
    {
        LEAF_CONTRACT;
        m_ThrewControlForThread = reason;
    }

    inline void ResetThrowControlForThread()
    {
        LEAF_CONTRACT;
        m_ThrewControlForThread = 0;
    }

    PTR_CONTEXT m_OSContext;    // ptr to a Context structure used to record the OS specific ThreadContext for a thread
                                // this is used for thread stop/abort and is intialized on demand

    PCONTEXT GetAbortContext ();

    // These will only ever be called from the debugger's helper
    // thread.
    //
    // When a thread is being created after a debug suspension has
    // started, we get the event on the debugger helper thread. It
    // will turn around and call this to set the debug suspend pending
    // flag on the newly created flag, since it was missed by
    // SysStartSuspendForGC as it didn't exist when that function was
    // run.
    void           MarkForDebugSuspend();

    // When the debugger uses the trace flag to single step a thread,
    // it also calls this function to mark this info in the thread's
    // state. The out-of-process portion of the debugger will read the
    // thread's state for a variety of reasons, including looking for
    // this flag.
    void           MarkDebuggerIsStepping(bool onOff)
    {
        WRAPPER_CONTRACT;
        if (onOff)
            SetThreadStateNC(Thread::TSNC_DebuggerIsStepping);
        else
            ResetThreadStateNC(Thread::TSNC_DebuggerIsStepping);
    }


    // Indicate whether this thread should run in the background.  Background threads
    // don't interfere with the EE shutting down.  Whereas a running non-background
    // thread prevents us from shutting down (except through System.Exit(), of course)
    void           SetBackground(BOOL isBack);

    // When the thread starts running, make sure it is running in the correct apartment
    // and context.
    BOOL           PrepareApartmentAndContext();


    // Either perform WaitForSingleObject or MsgWaitForSingleObject as appropriate.
    DWORD          DoAppropriateWait(int countHandles, HANDLE *handles, BOOL waitAll,
                                     DWORD millis, WaitMode mode,
                                     PendingSync *syncInfo = 0);

    DWORD          DoAppropriateWait(AppropriateWaitFunc func, void *args, DWORD millis,
                                     WaitMode mode, PendingSync *syncInfo = 0);
private:
    void           DoAppropriateWaitWorkerAlertableHelper(WaitMode mode);
    DWORD          DoAppropriateWaitWorker(int countHandles, HANDLE *handles, BOOL waitAll,
                                           DWORD millis, WaitMode mode);
    DWORD          DoAppropriateWaitWorker(AppropriateWaitFunc func, void *args,
                                           DWORD millis, WaitMode mode);
    DWORD          DoAppropriateAptStateWait(int numWaiters, HANDLE* pHandles, BOOL bWaitAll, DWORD timeout, WaitMode mode);

    DWORD          DoSyncContextWait(OBJECTREF *pSyncCtxObj, int countHandles, HANDLE *handles, BOOL waitAll, DWORD millis);

public:

    //************************************************************************
    // Enumerate all frames.
    //************************************************************************

    /* Flags used for StackWalkFramesEx */

    // FUNCTIONSONLY excludes all functionless frames and all funclets
    #define FUNCTIONSONLY                   0x0001

    // SKIPFUNCLETS includes functionless frames but excludes all funclets and everything between funclets and their parent methods
    #define SKIPFUNCLETS                    0x0002

    #define POPFRAMES                       0x0004

    /* use the following  flag only if you REALLY know what you are doing !!! */
    #define QUICKUNWIND                     0x0008 // do not restore all registers during unwind

    #define HANDLESKIPPEDFRAMES             0x0010 // temporary to handle skipped frames for appdomain unload
                                                   // stack crawl. Eventually need to always do this but it
                                                   // breaks the debugger right now.

    #define LIGHTUNWIND                     0x0020 // allow using cache schema (see StackwalkCache class)

    #define NOTIFY_ON_U2M_TRANSITIONS       0x0040 // Provide a callback for native transitions.
                                                   // This is only useful to a debugger trying to find native code
                                                   // in the stack.

    #define DISABLE_MISSING_FRAME_DETECTION 0x0080 // disable detection of missing TransitionFrames

    // One thread may be walking the stack of another thread
    // If you need to use this, you may also need to put a call to CrawlFrame::CheckGSCookies
    // in your callback routine if it does any potentially time-consuming activity.
    #define ALLOW_ASYNC_STACK_WALK          0x0100

    #define THREAD_IS_SUSPENDED             0x0200 // Be careful not to cause deadlocks, this thread is suspended

    // Stackwalk tries to verify some objects, but it could be called in relocate phase of GC,
    // where objects could be in invalid state, this flag is to tell stackwalk to skip the validation
    #define ALLOW_INVALID_OBJECTS           0x0400

    // Caller has verified that the thread to be walked is in the middle of executing
    // JITd or NGENd code, according to the thread's current context (or seeded
    // context if one was provided).  The caller ensures this when the stackwalk
    // is initiated by a profiler.
    #define THREAD_EXECUTING_MANAGED_CODE   0x0800

    // This stackwalk is due to the DoStackSnapshot profiler API
    #define PROFILER_DO_STACK_SNAPSHOT   0x1000


    StackWalkAction StackWalkFramesEx(
                        PREGDISPLAY pRD,        // virtual register set at crawl start
                        PSTACKWALKFRAMESCALLBACK pCallback,
                        VOID *pData,
                        unsigned flags,
                        Frame *pStartFrame = NULL);

private:
    // private helpers used by StackWalkFramesEx and StackFrameIterator
    IJitManager*    FillInCrawlFrameFromControlPC(CrawlFrame* pCF, PBYTE ControlPC);

    StackWalkAction HandleSpecialFramelessStub(CrawlFrame* pCF, 
                                               PSTACKWALKFRAMESCALLBACK pCallback, 
                                               VOID* pData, 
                                               UINT flags,
                                               bool fMakeCallbacks
                                               DEBUG_ARG(UINT32 uLoopIteration));

    StackWalkAction MakeExplicitFrameCallbacksForManagedFrame(CrawlFrame* pCF, 
                                                              PSTACKWALKFRAMESCALLBACK pCallback, 
                                                              VOID* pData, UINT flags, 
                                                              bool fUseCodeInfo, 
                                                              bool fMakeCallbacks  
                                                              DEBUG_ARG(UINT32 uLoopIteration));

    StackWalkAction MakeStackwalkerCallback(CrawlFrame* pCF, PSTACKWALKFRAMESCALLBACK pCallback, VOID* pData DEBUG_ARG(UINT32 uLoopIteration));

#ifdef _DEBUG
    void            DebugLogStackWalkInfo(CrawlFrame* pCF, __in_z LPSTR pszTag, UINT32 uLoopIteration);
#endif // _DEBUG

public:

    StackWalkAction StackWalkFrames(
                        PSTACKWALKFRAMESCALLBACK pCallback,
                        VOID *pData,
                        unsigned flags = 0,
                        Frame *pStartFrame = NULL);

    bool InitRegDisplay(const PREGDISPLAY, const PCONTEXT, bool validContext);
    void FillRegDisplay(const PREGDISPLAY pRD, PCONTEXT pctx);


    // During a <clinit>, this thread must not be asynchronously
    // stopped or interrupted.  That would leave the class unavailable
    // and is therefore a security hole.
    static void        IncPreventAsync(StateHolderParam)
    {
        WRAPPER_CONTRACT;
        Thread *pThread = GetThread();
        FastInterlockIncrement((LONG*)&pThread->m_PreventAsync);
    }
    static void        DecPreventAsync(StateHolderParam)
    {
        WRAPPER_CONTRACT;
        Thread *pThread = GetThread();
        FastInterlockDecrement((LONG*)&pThread->m_PreventAsync);
    }

    bool IsAsyncPrevented()
    {
        return m_PreventAsync != 0;
    }

    typedef StateHolder<Thread::IncPreventAsync, Thread::DecPreventAsync> ThreadPreventAsyncHolder;

    // During a <clinit>, this thread must not be asynchronously
    // stopped or interrupted.  That would leave the class unavailable
    // and is therefore a security hole.
    static void        IncPreventThreadAbort(StateHolderParam)
    {
        WRAPPER_CONTRACT;
        Thread *pThread = GetThread();
        FastInterlockIncrement((LONG*)&pThread->m_PreventThreadAbort);
    }
    static void        DecPreventThreadAbort(StateHolderParam)
    {
        WRAPPER_CONTRACT;
        Thread *pThread = GetThread();
        FastInterlockDecrement((LONG*)&pThread->m_PreventThreadAbort);
    }

    BOOL IsThreadAbortPrevented()
    {
        return m_PreventThreadAbort != 0;
    }

    typedef StateHolder<Thread::IncPreventThreadAbort, Thread::DecPreventThreadAbort> ThreadPreventAbortHolder;

    // The ThreadStore manages a list of all the threads in the system.  I
    // can't figure out how to expand the ThreadList template type without
    // making m_LinkStore public.
    SLink       m_LinkStore;
    
    // For N/Direct calls with the "setLastError" bit, this field stores
    // the errorcode from that call.
    DWORD       m_dwLastError;

    // Debugger per-thread flag for enabling notification on "manual"
    // method calls,  for stepping logic
    void IncrementTraceCallCount();
    void DecrementTraceCallCount();

    FORCEINLINE int IsTraceCall()
    {
        LEAF_CONTRACT;
        return m_TraceCallCount;
    }

    // Functions to get culture information for thread.
    int GetParentCultureName(__out_ecount(length) LPWSTR szBuffer, int length, BOOL bUICulture);
    int GetCultureName(__out_ecount(length) LPWSTR szBuffer, int length, BOOL bUICulture);
    LCID GetCultureId(BOOL bUICulture);
    OBJECTREF GetCulture(BOOL bUICulture);

    // Release user cultures that can't survive appdomain unload
    void ResetCultureForDomain(ADID id);

    // Functions to set the culture on the thread.
    void SetCultureId(LCID lcid, BOOL bUICulture);
    void SetCulture(OBJECTREF *CultureObj, BOOL bUICulture);

private:

    // Used by the culture accesors.
    ARG_SLOT CallPropertyGet(BinderMethodID id, OBJECTREF pObject);
    ARG_SLOT CallPropertySet(BinderMethodID id, OBJECTREF pObject, OBJECTREF pValue);




private:
    //-------------------------------------------------------------
    // Waiting & Synchronization
    //-------------------------------------------------------------

    // For suspends.  The thread waits on this event.  A client sets the event to cause
    // the thread to resume.
    void    WaitSuspendEvents(BOOL fDoWait = TRUE);
    BOOL    WaitSuspendEventsHelper(void);

    // Helpers to ensure that the bits for suspension and the number of active
    // traps remain coordinated.
    void    MarkForSuspension(ULONG bit);
    void    UnmarkForSuspension(ULONG bit);

    void    SetupForSuspension(ULONG bit)
    {
        WRAPPER_CONTRACT;
        if (bit & TS_UserSuspendPending) {
            m_UserSuspendEvent.Reset();
        }
        if (bit & TS_DebugSuspendPending) {
            m_DebugSuspendEvent.Reset();
        }
    }

    void    ReleaseFromSuspension(ULONG bit)
    {
        WRAPPER_CONTRACT;

        UnmarkForSuspension(~bit);

        //
        // If the thread is set free, mark it as not-suspended now
        //
        ThreadState oldState = m_State;

        while ((oldState & (TS_UserSuspendPending | TS_DebugSuspendPending)) == 0)
        {
            //
            // Construct the destination state we desire - all suspension bits turned off.
            //
            ThreadState newState = (ThreadState)(oldState & ~(TS_UserSuspendPending |
                                                              TS_DebugSuspendPending |
                                                              TS_SyncSuspended));

            if (FastInterlockCompareExchange((LONG *)&m_State, newState, oldState) == (LONG)oldState)
            {
                break;
            }

            //
            // The state changed underneath us, refresh it and try again.
            //
            oldState = m_State;
        }

        if (bit & TS_UserSuspendPending) {
            m_UserSuspendEvent.Set();
        }

        if (bit & TS_DebugSuspendPending) {
            m_DebugSuspendEvent.Set();
        }

    }

    // For getting a thread to a safe point.  A client waits on the event, which is
    // set by the thread when it reaches a safe spot.
    void    FinishSuspendingThread();
    void    SetSafeEvent();

    static CLREvent * g_pGCSuspendEvent;



    DWORD       m_Win32FaultAddress;
    DWORD       m_Win32FaultCode;


    // Support for Wait/Notify
    BOOL        Block(INT32 timeOut, PendingSync *syncInfo);
    void        Wake(SyncBlock *psb);
    DWORD       Wait(HANDLE *objs, int cntObjs, INT32 timeOut, PendingSync *syncInfo);
    DWORD       Wait(CLREvent* pEvent, INT32 timeOut, PendingSync *syncInfo);

    // support for Thread.Interrupt() which breaks out of Waits, Sleeps, Joins
    LONG        m_UserInterrupt;
    DWORD       IsUserInterrupted()
    {
        LEAF_CONTRACT;
        return m_UserInterrupt;
    }
    void        ResetUserInterrupted()
    {
        LEAF_CONTRACT;
        FastInterlockExchange(&m_UserInterrupt, 0);
    }

    void        HandleThreadInterrupt(BOOL fWaitForADUnload);

public:
    static void __stdcall UserInterruptAPC(ULONG_PTR ignore);

#if defined(_DEBUG) && defined(TRACK_SYNC)

// Each thread has a stack that tracks all enter and leave requests
public:
    Dbg_TrackSync   *m_pTrackSync;

#endif // TRACK_SYNC

private:





private:
    // For suspends:
    CLREvent        m_SafeEvent;
    CLREvent        m_UserSuspendEvent;
    CLREvent        m_DebugSuspendEvent;

    // For Object::Wait, Notify and NotifyAll, we use an Event inside the
    // thread and we queue the threads onto the SyncBlock of the object they
    // are waiting for.
    CLREvent        m_EventWait;
    WaitEventLink   m_WaitEventLink;
    WaitEventLink* WaitEventLinkForSyncBlock (SyncBlock *psb)
    {
        LEAF_CONTRACT;
        WaitEventLink *walk = &m_WaitEventLink;
        while (walk->m_Next) {
            _ASSERTE (walk->m_Next->m_Thread == this);
            if ((SyncBlock*)(((DWORD_PTR)walk->m_Next->m_WaitSB) & ~1)== psb) {
                break;
            }
            walk = walk->m_Next;
        }
        return walk;
    }

    // Access to thread handle and ThreadId.
    HANDLE      GetThreadHandle()
    {
        LEAF_CONTRACT;
        return m_ThreadHandle;
    }

    void        SetThreadHandle(HANDLE h)
    {
        LEAF_CONTRACT;
        FastInterlockExchangePointer(&m_ThreadHandle, h);
    }

    // We maintain a correspondence between this object, the ThreadId and ThreadHandle
    // in Win32, and the exposed Thread object.
    HANDLE          m_ThreadHandle;

    HANDLE          m_ThreadHandleForClose;
    HANDLE          m_ThreadHandleForResume;
    BOOL            m_WeOwnThreadHandle;
    DWORD           m_OSThreadId;

    BOOL CreateNewOSThread(SIZE_T stackSize, LPTHREAD_START_ROUTINE start, void *args);
    BOOL CreateNewHostTask(SIZE_T stackSize, LPTHREAD_START_ROUTINE start, void *args);

    OBJECTHANDLE    m_ExposedObject;
    OBJECTHANDLE    m_StrongHndToExposedObject;

    DWORD           m_Priority;     // initialized to INVALID_THREAD_PRIORITY, set to actual priority when a
                                    // thread does a busy wait for GC, reset to INVALID_THREAD_PRIORITY after wait is over
    friend class NDirect; // Quick access to thread stub creation


    ULONG           m_ExternalRefCount;

    ULONG           m_UnmanagedRefCount;

    LONG            m_TraceCallCount;

    //-----------------------------------------------------------
    // Bytes promoted on this thread since the last GC?
    //-----------------------------------------------------------
    DWORD           m_fPromoted;
public:
    void SetHasPromotedBytes ();
    DWORD GetHasPromotedBytes ()
    {
        LEAF_CONTRACT;
        return m_fPromoted;
    }

private:
    //-----------------------------------------------------------
    // Last exception to be thrown.
    //-----------------------------------------------------------
    friend class EEDbgInterfaceImpl;

private:
    // Stores the most recently thrown exception. We need to have a handle in case a GC occurs before
    // we catch so we don't lose the object. Having a static allows others to catch outside of COM+ w/o leaking
    // a handler and allows rethrow outside of COM+ too.
    // Differs from m_pThrowable in that it doesn't stack on nested exceptions.
    OBJECTHANDLE m_LastThrownObjectHandle;      // Unsafe to use directly.  Use accessors instead.

    friend void DECLSPEC_NORETURN EEPolicy::HandleFatalStackOverflow(EXCEPTION_POINTERS *pExceptionInfo, BOOL fSkipDebugger);

public:

    BOOL IsLastThrownObjectNull() { WRAPPER_CONTRACT; return (m_LastThrownObjectHandle == NULL); }

    OBJECTREF LastThrownObject()
    {
        WRAPPER_CONTRACT;

        if (m_LastThrownObjectHandle == NULL)
        {
            return NULL;
        }
        else
        {
            // We only have a handle if we have an object to keep in it.
            _ASSERTE(ObjectFromHandle(m_LastThrownObjectHandle) != NULL);
            return ObjectFromHandle(m_LastThrownObjectHandle);
        }
    }

    void SetLastThrownObject(OBJECTREF throwable);
    OBJECTREF SafeSetLastThrownObject(OBJECTREF throwable);
    void SafeUpdateLastThrownObject(void);
    OBJECTREF SafeSetThrowables(OBJECTREF pThrowable 
                                DEBUG_ARG(ThreadExceptionState::SetThrowableErrorChecking stecFlags = ThreadExceptionState::STEC_All));

    bool IsLastThrownObjectStackOverflowException()
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK(NULL != g_pPreallocatedStackOverflowException);

        return (m_LastThrownObjectHandle == g_pPreallocatedStackOverflowException);
    }

    void SetKickOffDomainId(ADID ad);
    ADID GetKickOffDomainId();

private:
    void SetLastThrownObjectHandle(OBJECTHANDLE h);

    ADID m_pKickOffDomainId;

    ThreadExceptionState  m_ExceptionState;

    //-----------------------------------------------------------
    // For stack probing.  These are the last allowable addresses that a thread
    // can touch.  Going beyond is a stack overflow.  The ProbeLimit will be
    // set based on whether SO probing is enabled.  The LastAllowableAddress
    // will always represent the true stack limit.
    //-----------------------------------------------------------
    UINT_PTR             m_ProbeLimit;
    IA64_ONLY(UINT_PTR m_BSPProbeLimit);


    UINT_PTR             m_LastAllowableStackAddress;
    IA64_ONLY(UINT_PTR m_LastAllowableBackingStoreStackAddress);

    //-----------------------------------------------------------
    // If the thread has wandered in from the outside this is
    // its Domain. This is temporary until domains are true contexts
    //-----------------------------------------------------------
    PTR_AppDomain m_pDomain;

    // Save the domain when a task is switched out, and restore it when
    // the task is switched in.
    PTR_AppDomain m_pDomainAtTaskSwitch;

    //---------------------------------------------------------------
    // m_debuggerFilterContext holds the thread's "filter context" for the
    // debugger.  This filter context is used by the debugger to seed
    // stack walks on the thread.
    //---------------------------------------------------------------
    PTR_CONTEXT m_debuggerFilterContext;

    //---------------------------------------------------------------
    // m_profilerFilterContext holds an additional context for the
    // case when a (sampling) profiler wishes to hijack the thread
    // and do a stack walk on the same thread.
    //---------------------------------------------------------------
    CONTEXT *m_pProfilerFilterContext;

    //---------------------------------------------------------------
    // m_hijackLock holds a BOOL that is used for mutual exclusion
    // between profiler stack walks and thread hijacks (bashing 
    // return addresses on the stack)
    //---------------------------------------------------------------
    volatile LONG m_hijackLock;
    //---------------------------------------------------------------
    // m_debuggerCantStop holds a count of entries into "can't stop"
    // areas that the Interop Debugging Services must know about.
    //---------------------------------------------------------------
    DWORD m_debuggerCantStop;

    //---------------------------------------------------------------
    // A word reserved for use by the CLR Debugging Services during
    // managed/unmanaged debugging.
    //---------------------------------------------------------------
    DWORD    m_debuggerWord;

    //---------------------------------------------------------------
    // For Interop-Debugging; track if a thread is hijacked.
    //---------------------------------------------------------------
    BOOL    m_fInteropDebuggingHijacked;

    //---------------------------------------------------------------
    // Bitmask to remember per-thread state useful for the profiler API.  See
    // COR_PRF_CALLBACKSTATE_* flags in clr\src\inc\ProfilePriv.h for bit values.
    //---------------------------------------------------------------
    DWORD m_profilerCallbackState;

private:

    //-------------------------------------------------------------------------
    // AppDomains on the current call stack
    //-------------------------------------------------------------------------
    AppDomainStack  m_ADStack;

    //-------------------------------------------------------------------------
    // Support creation of assemblies in DllMain (see ceemain.cpp)
    //-------------------------------------------------------------------------
    DomainFile* m_pLoadingFile;


    // The ThreadAbort reason (Get/Set/ClearExceptionStateInfo on the managed thread) is
    // held here as an OBJECTHANDLE and the ADID of the AppDomain in which it is valid.
    // Atomic updates of this state use the Thread's Crst.

    OBJECTHANDLE    m_AbortReason;
    ADID            m_AbortReasonDomainID;

    void            ClearAbortReason();

protected:
    // Hash table that maps a domain id to a LocalDataStore*
    EEIntHashTable* m_pDLSHash;

public:

    void SetInteropDebuggingHijacked(BOOL f)
    {
        LEAF_CONTRACT;
        m_fInteropDebuggingHijacked = f;
    }
    BOOL GetInteropDebuggingHijacked()
    {
        LEAF_CONTRACT;
        return m_fInteropDebuggingHijacked;
    }

    inline DWORD IncrementOverridesCount();
    inline DWORD DecrementOverridesCount();
    inline DWORD GetOverridesCount();
    inline DWORD IncrementAssertCount();
    inline DWORD DecrementAssertCount();
    inline DWORD GetAssertCount();
    inline void PushDomain(ADID pDomain);
    inline ADID PopDomain();
    inline DWORD GetNumAppDomainsOnThread();
    inline BOOL CheckThreadWideSpecialFlag(DWORD flags);
    inline void InitDomainIteration(DWORD *pIndex);
    inline ADID GetNextDomainOnStack(DWORD *pIndex, DWORD *pOverrides, DWORD *pAsserts);
    inline void UpdateDomainOnStack(DWORD pIndex, DWORD asserts, DWORD overrides);

    BOOL IsDefaultSecurityInfo(void)
    {
        WRAPPER_CONTRACT;
        return m_ADStack.IsDefaultSecurityInfo();
    }

    BOOL AllDomainsHomogeneousWithNoStackModifiers(void)
    {
        WRAPPER_CONTRACT;
        return m_ADStack.AllDomainsHomogeneousWithNoStackModifiers();
    }
    
    const AppDomainStack& GetAppDomainStack(void)
    {
        LEAF_CONTRACT;
        return m_ADStack;
    }
    AppDomainStack* GetAppDomainStackPointer(void)
    {
        LEAF_CONTRACT;
        return &m_ADStack;
    }

    void SetAppDomainStack(const AppDomainStack& appDomainStack)
    {
        WRAPPER_CONTRACT;
        m_ADStack = appDomainStack; // this is a function call, massive operator=
    }

    void ResetSecurityInfo( void )
    {
        WRAPPER_CONTRACT;
        m_ADStack.ClearDomainStack();
    }

    void SetFilterContext(CONTEXT *pContext);
    CONTEXT *GetFilterContext(void);

    void SetProfilerFilterContext(CONTEXT *pContext)
    {
        LEAF_CONTRACT;

        m_pProfilerFilterContext = pContext;
    }

    // Used by the profiler API to find which flags have been set on the Thread object,
    // in order to authorize a profiler's call into ICorProfilerInfo(2).
    DWORD GetProfilerCallbackFullState()
    {
        LEAF_CONTRACT;
        _ASSERTE(GetThread() == this);
        
        return m_profilerCallbackState;
    }

    // Used by profiler API to set at once all callback flag bits stored on the Thread object.
    // Used to reinstate the previous state that had been modified by a previous call to
    // SetProfilerCallbackStateFlags
    void SetProfilerCallbackFullState(DWORD dwFullState)
    {
        LEAF_CONTRACT;
        _ASSERTE(GetThread() == this);
        
        m_profilerCallbackState = dwFullState;
    }
    
    // Used by profiler API to set individual callback flags on the Thread object.
    // Returns the previous state of all flags.
    DWORD SetProfilerCallbackStateFlags(DWORD dwFlags)
    {
        LEAF_CONTRACT;
        _ASSERTE(GetThread() == this);
        
        DWORD dwRet = m_profilerCallbackState;
        m_profilerCallbackState |= dwFlags;
        return dwRet;
    }

    CONTEXT *GetProfilerFilterContext(void)
    {
        LEAF_CONTRACT;

        return m_pProfilerFilterContext;
    }

    //-------------------------------------------------------------------------
    // The hijack lock enforces that a thread on which a profiler is currently
    // performing a stack walk cannot be hijacked.
    //
    // Note that the hijack lock cannot be managed by the host (i.e., this
    // cannot be a Crst), because this could lead to a deadlock:  YieldTask,
    // which is called by the host, may need to hijack, for which it would
    // need to take this lock - but since the host needs not be reentrant,
    // taking the lock cannot cause a call back into the host.
    //-------------------------------------------------------------------------
    static BOOL EnterHijackLock(Thread *pThread)
    {
        LEAF_CONTRACT;

        return ::InterlockedCompareExchange(&(pThread->m_hijackLock), TRUE, FALSE) == FALSE;
    }

    static void LeaveHijackLock(Thread *pThread)
    {
        LEAF_CONTRACT;

        pThread->m_hijackLock = FALSE;
    }

    typedef ConditionalStateHolder<Thread *, Thread::EnterHijackLock, Thread::LeaveHijackLock> HijackLockHolder;
    //-------------------------------------------------------------------------

    static bool ThreadsAtUnsafePlaces(void)
    {
        LEAF_CONTRACT;

        return (m_threadsAtUnsafePlaces != 0);
    }

    static void IncThreadsAtUnsafePlaces(void)
    {
        LEAF_CONTRACT;
        InterlockedIncrement((LONG *)&m_threadsAtUnsafePlaces);
    }

    static void DecThreadsAtUnsafePlaces(void)
    {
        LEAF_CONTRACT;
        InterlockedDecrement((LONG *)&m_threadsAtUnsafePlaces);
    }

    void PrepareForEERestart()
    {
        WRAPPER_CONTRACT;


        ResetThreadState(TS_GCSuspendPending);
    }


    void SetDebugCantStop(bool fCantStop);
    bool GetDebugCantStop(void);

    static LPVOID GetStaticFieldAddress(FieldDesc *pFD);
    TADDR GetStaticFieldAddrNoCreate(FieldDesc *pFD);

    void SetLoadingFile(DomainFile *pFile)
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK(m_pLoadingFile == NULL);
        m_pLoadingFile = pFile;
    }

    void ClearLoadingFile()
    {
        LEAF_CONTRACT;
        m_pLoadingFile = NULL;
    }

    DomainFile *GetLoadingFile()
    {
        LEAF_CONTRACT;
        return m_pLoadingFile;
    }

 private:

    static void LoadingFileRelease(Thread *pThread)
    {
        WRAPPER_CONTRACT;
        pThread->ClearLoadingFile();
    }

 public:

     typedef Holder<Thread *, DoNothing, Thread::LoadingFileRelease> LoadingFileHolder;

private:
    // Don't allow a thread to be asynchronously stopped or interrupted (e.g. because
    // it is performing a <clinit>)
    int         m_PreventAsync;
    int         m_PreventThreadAbort;
    int         m_nNestedMarshalingExceptions;
    BOOL IsMarshalingException()
    {
        LEAF_CONTRACT;
        return (m_nNestedMarshalingExceptions != 0);
    }
    int StartedMarshalingException()
    {
        LEAF_CONTRACT;
        return m_nNestedMarshalingExceptions++;
    }
    void FinishedMarshalingException()
    {
        LEAF_CONTRACT;
        _ASSERTE(m_nNestedMarshalingExceptions > 0);
        m_nNestedMarshalingExceptions--;
    }

    static LONG m_DebugWillSyncCount;

    // Are we shutting down the process?
    static BOOL    IsAtProcessExit();

    // IP cache used by QueueCleanupIP.
    #define CLEANUP_IPS_PER_CHUNK 4
    struct CleanupIPs {
        IUnknown    *m_Slots[CLEANUP_IPS_PER_CHUNK];
        CleanupIPs  *m_Next;
        CleanupIPs() {LEAF_CONTRACT; memset(this, 0, sizeof(*this)); }
    };
    CleanupIPs   m_CleanupIPs;

#define BEGIN_FORBID_TYPELOAD() _ASSERTE(GetThread() == 0 || ++GetThread()->m_ulForbidTypeLoad)
#define END_FORBID_TYPELOAD()   _ASSERTE(GetThread() == 0 || GetThread()->m_ulForbidTypeLoad--)
#define TRIGGERS_TYPELOAD()     _ASSERTE(GetThread() == 0 || !GetThread()->m_ulForbidTypeLoad)
#ifdef _DEBUG
#define IS_TYPELOAD_FORBIDDEN() (GetThread() && GetThread()->m_ulForbidTypeLoad)
#else
#define IS_TYPELOAD_FORBIDDEN() FALSE
#endif

#ifdef _DEBUG
public:
        DWORD m_GCOnTransitionsOK;
    ULONG  m_ulForbidTypeLoad;


/****************************************************************************/
/* The code below an attempt to catch people who don't protect GC pointers that
   they should be protecting.  Basically, OBJECTREF's constructor, adds the slot
   to a table.   When we protect a slot, we remove it from the table.  When GC
   could happen, all entries in the table are marked as bad.  When access to
   an OBJECTREF happens (the -> operator) we assert the slot is not bad.  To make
   this fast, the table is not perfect (there can be collisions), but this should
   not cause false positives, but it may allow errors to go undetected  */

#define OBJREF_HASH_SHIFT_AMOUNT 2

        // For debugging, you may want to make this number very large, (8K)
        // should basically insure that no collisions happen
#define OBJREF_TABSIZE              256
        DWORD_PTR dangerousObjRefs[OBJREF_TABSIZE];      // Really objectRefs with lower bit stolen

        static DWORD_PTR OBJREF_HASH;
        // Remembers that this object ref pointer is 'alive' and unprotected (Bad if GC happens)
        static void ObjectRefNew(const OBJECTREF* ref) {
            WRAPPER_CONTRACT;
            Thread* curThread = GetThread();
            if (curThread == 0) return;

            curThread->dangerousObjRefs[((size_t)ref >> OBJREF_HASH_SHIFT_AMOUNT) % OBJREF_HASH] = (size_t)ref;
        }

        static void ObjectRefAssign(const OBJECTREF* ref) {
            WRAPPER_CONTRACT;
            Thread* curThread = GetThread();
            if (curThread == 0) return;

            DWORD_PTR* slot = &curThread->dangerousObjRefs[((DWORD_PTR) ref >> OBJREF_HASH_SHIFT_AMOUNT) % OBJREF_HASH];
            if ((*slot & ~3) == (size_t) ref)
                *slot = *slot & ~1;                  // Don't care about GC's that have happened
        }

        // If an object is protected, it can be removed from the 'dangerous table'
        static void ObjectRefProtected(const OBJECTREF* ref) {
#ifdef USE_CHECKED_OBJECTREFS
            WRAPPER_CONTRACT;
            _ASSERTE(IsObjRefValid(ref));
            Thread* curThread = GetThread();
            if (curThread == 0) return;

            DWORD_PTR* slot = &curThread->dangerousObjRefs[((DWORD_PTR) ref >> OBJREF_HASH_SHIFT_AMOUNT) % OBJREF_HASH];
            if ((*slot & ~3) == (DWORD_PTR) ref)
                *slot = (size_t) ref | 2;                             // mark has being protected
#else
            LEAF_CONTRACT;
#endif
        }

        static bool IsObjRefValid(const OBJECTREF* ref) {
            WRAPPER_CONTRACT;
            Thread* curThread = GetThread();
            if (curThread == 0) return(true);

            // If the object ref is NULL, we'll let it pass.
            if (*((DWORD_PTR*) ref) == 0)
                return(true);

            DWORD_PTR val = curThread->dangerousObjRefs[((DWORD_PTR) ref >> OBJREF_HASH_SHIFT_AMOUNT) % OBJREF_HASH];
            // if not in the table, or not the case that it was unprotected and GC happened, return true.
            if((val & ~3) != (size_t) ref || (val & 3) != 1)
                return(true);
            // If the pointer lives in the GC heap, than it is protected, and thus valid.
            if ((DWORD_PTR)(BYTE *)g_lowest_address <= val && val < (DWORD_PTR)(BYTE *)g_highest_address)
                return(true);
            return(false);
        }

        // Clears the table.  Useful to do when crossing the managed-code - EE boundary
        // as you ususally only care about OBJECTREFS that have been created after that
        static void __stdcall ObjectRefFlush(Thread* thread);



#endif // _DEBUG

private:
        PEXCEPTION_REGISTRATION_RECORD * m_pExceptionList;
public:
        PEXCEPTION_REGISTRATION_RECORD * GetExceptionListPtr() {
            LEAF_CONTRACT;
            return m_pExceptionList;
        }

private:
    PTR_CONTEXT m_pSavedRedirectContext;

public:
    PCONTEXT GetSavedRedirectContext()
    {
        LEAF_CONTRACT;
        return (m_pSavedRedirectContext);
    }

#ifndef DACCESS_COMPILE
    void     SetSavedRedirectContext(PCONTEXT pCtx)
    {
        LEAF_CONTRACT;
        m_pSavedRedirectContext = pCtx;
    }
#endif

    void EnsurePreallocatedContext();

    inline STATIC_DATA *GetSharedStaticData()
    {
        WRAPPER_CONTRACT;

        if (m_pSharedStaticData)
            return m_pSharedStaticData;

        if (!m_pStaticDataHash)
            return NULL;

        PTR_STATIC_DATA_LIST pNode=NULL;
        LPVOID pNodePtr = NULL;
        m_pStaticDataHash->GetValue( (void *) PTR_TO_TADDR(m_pDomain), &pNodePtr);
        pNode = PTR_STATIC_DATA_LIST((TADDR)pNodePtr);
        if (pNode)
#ifndef DACCESS_COMPILE
            m_pSharedStaticData = pNode->m_pSharedStaticData;
#else
            return pNode->m_pSharedStaticData;
#endif // !DACCESS_COMPILE

        return m_pSharedStaticData;
    }

    inline STATIC_DATA *GetUnsharedStaticData()
    {
        WRAPPER_CONTRACT;

        if (m_pUnsharedStaticData)
            return m_pUnsharedStaticData;

        if (!m_pStaticDataHash)
            return NULL;

        PTR_STATIC_DATA_LIST pNode=NULL;
        LPVOID pNodePtr = NULL;
        m_pStaticDataHash->GetValue( (void *) PTR_TO_TADDR(m_pDomain), &pNodePtr);
        pNode = PTR_STATIC_DATA_LIST((TADDR)pNodePtr);
        if (pNode)
#ifndef DACCESS_COMPILE
            m_pUnsharedStaticData = pNode->m_pUnsharedStaticData;
#else
            return pNode->m_pUnsharedStaticData;
#endif // !DACCESS_COMPILE
        return m_pUnsharedStaticData;
    }

    void SetName(__in_ecount(length) WCHAR* name, DWORD length);

protected:
    static MethodDesc *GetDLSRemoveMethod();
    LocalDataStore *RemoveDomainLocalStore(ADID iAppDomainId);
    static void RemoveAllDomainLocalStores(EEIntHashTable **ppDLSHash, bool fNeedsLock);
    static void RemoveDLSFromList(LocalDataStore* pLDS);
    static HRESULT RemoveDLSFromListNoThrow(LocalDataStore* pLDS);
    static void RemoveDLSFromListNoThrowHelper(LocalDataStore* pLDS, int adid);
    void DeleteThreadStaticData(AppDomain *pDomain);
    void DeleteThreadStaticClassData(_STATIC_DATA* pData);

private:

    // The following variables are used to cache thread local static data (original
    // 'copy' lives in m_pStaticDataHash hash table. On each appdomain switch they get set.
    //

    PTR_STATIC_DATA  m_pUnsharedStaticData;
    PTR_STATIC_DATA  m_pSharedStaticData;

    PTR_EEPtrHashTable m_pStaticDataHash;
    Crst               *m_pSDHCrst;           // Mutex protecting m_pStaticDataHash

    void FreeThreadStaticSlot(int slot);
    void GetStaticFieldAddressSpecial(FieldDesc *pFD, MethodTable *pMT, int *pSlot, LPVOID *ppvAddress, AppDomain* pDomain);
    STATIC_DATA_LIST *SetStaticData(AppDomain *pDomain, STATIC_DATA *pSharedData, STATIC_DATA *pUnsharedData);
    STATIC_DATA_LIST *SafeSetStaticData(AppDomain *pDomain, STATIC_DATA *pSharedData, STATIC_DATA *pUnsharedData);
    void DeleteThreadStaticData();
    void CheckAndDeleteStaticField(FieldDesc* pFD, void *dataPtr);

public:
    TADDR CalculateAddressForManagedStatic(int slot);

#ifdef _DEBUG
private:
    // When we create an object, or create an OBJECTREF, or create an Interior Pointer, or enter EE from managed
    // code, we will set this flag.
    // Inside GCHeap::StressHeap, we only do GC if this flag is TRUE.  Then we reset it to zero.
    BOOL m_fStressHeapCount;
public:
    void EnableStressHeap()
    {
        LEAF_CONTRACT;
        m_fStressHeapCount = TRUE;
    }
    void DisableStressHeap()
    {
        LEAF_CONTRACT;
        m_fStressHeapCount = FALSE;
    }
    BOOL StressHeapIsEnabled()
    {
        LEAF_CONTRACT;
        return m_fStressHeapCount;
    }

    size_t *m_pCleanedStackBase;
#endif

#ifdef STRESS_THREAD
public:
    LONG  m_stressThreadCount;
#endif

private:
    // This member is declared up above to give it a nice constant offset since it
    // needs to be hard-coded in the 64-bit PInvoke stubs for speed.
    //IHostTask *m_pHostTask;
    PVOID      m_pFiberData;

    TASKID     m_TaskId;
    CONNID     m_dwConnectionId;

public:
    void SetupFiberData();

#ifdef _DEBUG
public:
    void AddFiberInfo(DWORD type);
    enum {
        ThreadTrackInfo_Lifetime=0x1,   // creation, destruction, ref-count
        ThreadTrackInfo_Schedule=0x2,   // switch in/out
        ThreadTrackInfo_UM_M=0x4,       // Unmanaged <-> managed transtion
        ThreadTrackInfo_Abort=0x8,      // Thread abort
        ThreadTrackInfo_Affinity=0x10,  // Thread's affinity
        ThreadTrackInfo_GCMode=0x20,
        ThreadTrackInfo_Escalation=0x40,// escalation point
        ThreadTrackInfo_SO=0x80,
        ThreadTrackInfo_Max=8
    };
private:
    static int MaxThreadRecord;
    static int MaxStackDepth;
    static const int MaxThreadTrackInfo;
    struct FiberSwitchInfo
    {
        unsigned __int64 timeStamp;
        DWORD threadID;
        size_t callStack[1];
    };
    FiberSwitchInfo *m_pFiberInfo[ThreadTrackInfo_Max];
    DWORD m_FiberInfoIndex[ThreadTrackInfo_Max];
#endif

#ifdef DACCESS_COMPILE
public:
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
    void EnumMemoryRegionsWorker(CLRDataEnumMemoryFlags flags);
#endif

private:
    DWORD   m_dwPrepareCer;

public:
    // Hook for OS Critical Section, Mutex, and others that require thread affinity

    // Support for tracking the fact that constrained region preparation is taking place on this thread (used to avoid recursion).
    static void BeginPrepareCer(StateHolderParam)
    {
        LEAF_CONTRACT;
        Thread *pThread = GetThread();
        _ASSERTE (pThread && pThread->m_dwPrepareCer != (DWORD)-1);
        pThread->m_dwPrepareCer++;
    }

    static void EndPrepareCer(StateHolderParam)
    {
        LEAF_CONTRACT;
        Thread *pThread = GetThread();
        _ASSERTE (pThread && pThread->m_dwPrepareCer > 0);
        pThread->m_dwPrepareCer--;
    }

    BOOL IsPreparingCer()
    {
        LEAF_CONTRACT;
        return m_dwPrepareCer > 0;
    }

    typedef StateHolder<Thread::BeginPrepareCer, Thread::EndPrepareCer> ThreadPreparingCerHolder;

    // Is the current thread currently executing within a constrained execution region?
    static BOOL IsExecutingWithinCer();

    // Determine whether the method at the given frame in the thread's execution stack is executing within a CER.
    BOOL IsWithinCer(CrawlFrame *pCf);

    // Determine if the current stack has Cer methods on stack.
    BOOL HasCerOnStack();

private:
    // used to pad stack on thread creation to avoid aliasing penalty in P4 HyperThread scenarios

    static DWORD __stdcall intermediateThreadProc(PVOID arg);
    static int m_offset_counter;
    static const int offset_multiplier = 128;

    typedef struct {
        LPTHREAD_START_ROUTINE  lpThreadFunction;
        PVOID lpArg;
    } intermediateThreadParam;

#ifdef _DEBUG
// when the thread is doing a stressing GC, some Crst violation could be ignored, by a non-elegant solution.
private:
    BOOL m_bGCStressing; // the flag to indicate if the thread is doing a stressing GC
    BOOL m_bUniqueStacking; // the flag to indicate if the thread is doing a UniqueStack
public:
    BOOL GetGCStressing ()
    {
        return m_bGCStressing;
    }
    BOOL GetUniqueStacking ()
    {
        return m_bUniqueStacking;
    }
#endif

private:
    //-----------------------------------------------------------------------------
    // AVInRuntimeImplOkay : its okay to have an AV in Runtime implemetation while
    // this holder is in effect.
    //
    //  {
    //      AVInRuntimeImplOkayHolder foo();
    //  } // make AV's in the Runtime illegal on out of scope.
    //-----------------------------------------------------------------------------
    DWORD m_dwAVInRuntimeImplOkayCount;

    static void AVInRuntimeImplOkayAcquire(StateHolderParam)
    {
        LEAF_CONTRACT;

        Thread *pThread = GetThread();

        if (pThread)
        {
            _ASSERTE(pThread->m_dwAVInRuntimeImplOkayCount != (DWORD)-1);
            pThread->m_dwAVInRuntimeImplOkayCount++;
        }
    }

    static void AVInRuntimeImplOkayRelease(StateHolderParam)
    {
        LEAF_CONTRACT;

        Thread *pThread = GetThread();

        if (pThread)
        {
            _ASSERTE(pThread->m_dwAVInRuntimeImplOkayCount > 0);
            pThread->m_dwAVInRuntimeImplOkayCount--;
        }
    }

public:
    static BOOL AVInRuntimeImplOkay(void)
    {
        LEAF_CONTRACT;

        Thread *pThread = GetThread();

        if (pThread)
        {
            return (pThread->m_dwAVInRuntimeImplOkayCount > 0);
        }
        else
        {
            return FALSE;
        }
    }

    typedef StateHolder<Thread::AVInRuntimeImplOkayAcquire, Thread::AVInRuntimeImplOkayRelease> AVInRuntimeImplOkayHolder;

#ifdef _DEBUG
private:
    DWORD m_dwUnbreakableLockCount;
public:
    void IncUnbreakableLockCount()
    {
        LEAF_CONTRACT;
        _ASSERTE (m_dwUnbreakableLockCount != (DWORD)-1);
        m_dwUnbreakableLockCount ++;
    }
    void DecUnbreakableLockCount()
    {
        LEAF_CONTRACT;
        _ASSERTE (m_dwUnbreakableLockCount > 0);
        m_dwUnbreakableLockCount --;
    }
    BOOL HasUnbreakableLock() const
    {
        LEAF_CONTRACT;
        return m_dwUnbreakableLockCount != 0;
    }
    DWORD GetUnbreakableLockCount() const
    {
        LEAF_CONTRACT;
        return m_dwUnbreakableLockCount;
    }
#endif // _DEBUG

#ifdef _DEBUG
private:
    friend class FCallTransitionState;
    friend class PermitHelperMethodFrameState;
    friend class CompletedFCallTransitionState;
    HelperMethodFrameCallerList *m_pHelperMethodFrameCallerList;
#endif // _DEBUG

private:
    LONG m_dwHostTaskRefCount;

private:
    // If HasStarted fails, we cache the exception here, and rethrow on the thread which
    // calls Thread.Start.
    Exception* m_pExceptionDuringStartup;

#if defined(USE_DBGHELP_TO_WALK_STACK_IN_DAC)
private:
    static HMODULE                 s_hDbgHelp;
    static PDBGHELP__STACKWALK     s_pfnStackWalk;
    static PDBGHELP__SYMINITIALIZE s_pfnSymInitialize;
    static PDBGHELP__SYMCLEANUP    s_pfnSymCleanup;

    static HANDLE s_hFakeProcess;
    static HANDLE s_hFakeThread;

    bool InitDbgHelp();
    void FreeDbgHelp();

    static BOOL    Thread::ReadMemory(HANDLE  hProcess,
                                      DWORD64 lpBaseAddress,
                                      PVOID   lpBuffer,
                                      DWORD   nSize,
                                      LPDWORD lpNumberOfBytesRead);
    static PVOID   Thread::FunctionTableAccess(HANDLE hProcess, DWORD64 AddrBase);
    static DWORD64 Thread::GetModuleBase(HANDLE hProcess, DWORD64 Address);
#endif // USE_DBGHELP_TO_WALK_STACK_IN_DAC

public:
    void HandleThreadStartupFailure();

#if defined(STRESS_HEAP) && defined(_DEBUG)
private:
    BYTE* m_pbDestCode;
    BYTE* m_pbSrcCode;

public:
    void CommitGCStressInstructionUpdate();
    void PostGCStressInstructionUpdate(BYTE* pbDestCode, BYTE* pbSrcCode)
    {
        LEAF_CONTRACT;
        PRECONDITION(!HasPendingGCStressInstructionUpdate());

        m_pbDestCode = pbDestCode;
        m_pbSrcCode = pbSrcCode;
    }
    bool HasPendingGCStressInstructionUpdate()
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK((NULL == m_pbDestCode) == (NULL == m_pbSrcCode));
        return m_pbDestCode != NULL;
    }
    void ClearGCStressInstructionUpdate()
    {
        LEAF_CONTRACT;
        PRECONDITION(HasPendingGCStressInstructionUpdate());

        m_pbDestCode = NULL;
        m_pbSrcCode = NULL;
    }
#endif // defined(STRESS_HEAP) && defined(_DEBUG)

#if defined(STACK_GUARDS_DEBUG)
class ::BaseStackGuard;
private:
    // This field is used for debugging purposes to allow easy access to the stack guard
    // chain and also in SO-tolerance checking to quickly determine if a guard is in place.
    BaseStackGuard *m_pCurrentStackGuard;

public:
    BaseStackGuard *GetCurrentStackGuard()
    {
        LEAF_CONTRACT;
        return m_pCurrentStackGuard;
    }

    void SetCurrentStackGuard(BaseStackGuard *pGuard)
    {
        LEAF_CONTRACT;
        m_pCurrentStackGuard = pGuard;
    }
#endif

private:
    BOOL m_fCompletionPortDrained;
public:
    void MarkCompletionPortDrained()
    {
        LEAF_CONTRACT;
        FastInterlockExchange ((LONG*)&m_fCompletionPortDrained, TRUE);
    }
    void UnmarkCompletionPortDrained()
    {
        LEAF_CONTRACT;
        FastInterlockExchange ((LONG*)&m_fCompletionPortDrained, FALSE);
    }
    BOOL IsCompletionPortDrained()
    {
        LEAF_CONTRACT;
        return m_fCompletionPortDrained;
    }

    // --------------------------------
    //  Store the maxReservedStackSize
    //  This is passed in from managed code in the thread constructor
    // ---------------------------------
private:
    SIZE_T m_RequestedStackSize;

public:

    // Get the MaxStackSize
    SIZE_T RequestedThreadStackSize()
    {
        LEAF_CONTRACT;
        return (m_RequestedStackSize);
    }

    // Set the MaxStackSize
    void RequestedThreadStackSize(SIZE_T requestedStackSize)
    {
        LEAF_CONTRACT;
        m_RequestedStackSize = requestedStackSize;
    }

private:
    // YieldTask, ThreadAbort, GC all change thread context.  ThreadAbort and GC uses ThreadStore lock to synchronize.  But YieldTask can
    // not block.  We use a counter to allow one thread to change thread context.

    volatile PVOID m_WorkingOnThreadContext;

    static BOOL EnterWorkingOnThreadContext(Thread *pThread)
    {
        LEAF_CONTRACT;

        if (CLRTaskHosted())
        {
            PVOID myID = ClrTeb::GetFiberPtrId();
            PVOID id = FastInterlockCompareExchangePointer(&pThread->m_WorkingOnThreadContext, myID, NULL);
            return id == NULL || id == myID;
        }
        else
        {
            return TRUE;
        }
    }

    static void LeaveWorkingOnThreadContext(Thread *pThread)
    {
        LEAF_CONTRACT;

        if (pThread->m_WorkingOnThreadContext == ClrTeb::GetFiberPtrId())
        {
            pThread->m_WorkingOnThreadContext = NULL;
        }
    }

    typedef ConditionalStateHolder<Thread *, Thread::EnterWorkingOnThreadContext, Thread::LeaveWorkingOnThreadContext> WorkingOnThreadContextHolder;

    BOOL WorkingOnThreadContext()
    {
        LEAF_CONTRACT;

        return !CLRTaskHosted() || m_WorkingOnThreadContext == ClrTeb::GetFiberPtrId();
    }

private:
    EXCEPTION_POINTERS m_SOExceptionInfo;
public:
    void SetSOExceptionInfo(PEXCEPTION_RECORD ExceptionRecord, PCONTEXT ContextRecord)
    {
        LEAF_CONTRACT;
        m_SOExceptionInfo.ExceptionRecord = ExceptionRecord;
        m_SOExceptionInfo.ContextRecord = ContextRecord;
        if (ExceptionRecord == NULL)
        {
            ResetThreadStateNC(TSNC_SOWorkNeeded);
        }
        else
        {
            ResetThrowControlForThread();

            // Since this Thread has taken an SO, there may be state left-over after we
            //  short-circuited exception or other error handling, and so we don't want
            //  to risk recycling it.
            SetThreadStateNC(TSNC_CannotRecycle);

        }
    }
    EXCEPTION_POINTERS * GetSOExceptionInfo()
    {
        _ASSERTE (m_SOExceptionInfo.ExceptionRecord);
        return &m_SOExceptionInfo;
    }

    void SetSOWorkNeeded()
    {
        SetAbortRequestBit();
        SetThreadStateNC(TSNC_SOWorkNeeded);
    }

    void FinishSOWork();

    void ClearExceptionStateAfterSO(void* pStackFrameSP)
    {
        WRAPPER_CONTRACT;

        // Clear any stale exception state.
        m_ExceptionState.ClearExceptionStateAfterSO(pStackFrameSP);
    }

private:
    BOOL m_fAllowProfilerCallbacks;

public:
    //
    // These two methods are for profiler support.  The profiler clears the allowed
    // value once it has delivered a ThreadDestroyed callback, so that it does not
    // deliver any notifications to the profiler afterwards which reference this 
    // thread.  Callbacks on this thread which do not reference this thread are 
    // allowable.
    //
    BOOL ProfilerCallbacksAllowed(void)
    {
        return m_fAllowProfilerCallbacks;
    }

    void SetProfilerCallbacksAllowed(BOOL fValue)
    {
        m_fAllowProfilerCallbacks = fValue;
    }


private:
    // Inside a host, we don't own a thread handle, and we avoid DuplicateHandle call.
    // If a thread is dying after we obtain the thread handle, our SuspendThread may fail
    // because the handle may be closed and reused for a completely different type of handle.
    // To solve this problem, we have a counter m_dwThreadHandleBeingUsed.  Before we grab
    // the thread handle, we increment the counter.  Before we return a thread back to SQL
    // in Reset and ExitTask, we wait until the counter drops to 0.
    volatile LONG m_dwThreadHandleBeingUsed;


private:
    static BOOL s_fCleanFinalizedThread;

public:
    static void SetCleanupNeededForFinalizedThread()
    {
        LEAF_CONTRACT;
        _ASSERTE (IsFinalizerThread());
        s_fCleanFinalizedThread = TRUE;
    }

    static BOOL CleanupNeededForFinalizedThread()
    {
        LEAF_CONTRACT;
        return s_fCleanFinalizedThread;
    }
};

LCID GetThreadCultureIdNoThrow(Thread *pThread, BOOL bUICulture);

// Request/Remove Thread Affinity for the current thread
typedef StateHolder<Thread::BeginThreadAffinityAndCriticalRegionForHolder, Thread::EndThreadAffinityAndCriticalRegionForHolder> ThreadAffinityAndCriticalRegionHolder;
typedef StateHolder<Thread::BeginThreadAffinityForHolder, Thread::EndThreadAffinityForHolder> ThreadAffinityHolder;
typedef Thread::ForbidSuspendThreadHolder ForbidSuspendThreadHolder;
typedef Thread::ThreadPreventAsyncHolder ThreadPreventAsyncHolder;
typedef Thread::ThreadPreventAbortHolder ThreadPreventAbortHolder;
typedef StateHolder<Thread::ReverseEnterRuntimeForHolder, Thread::ReverseLeaveRuntimeForHolder> ReverseEnterRuntimeHolder;
typedef Thread::ThreadPreparingCerHolder ThreadPreparingCerHolder;

#ifdef PROFILING_SUPPORTED
#undef RuntimeThreadSuspended
inline HRESULT RuntimeThreadSuspendedWrapper(ThreadID suspendedThreadId, ThreadID    threadId)
{
    //
    //
    //
    ForbidSuspendThreadHolder forbidSuspendThread(suspendedThreadId == threadId);
    PROFILER_CALL;
    return g_profControlBlock.pProfInterface->RuntimeThreadSuspended(suspendedThreadId, threadId);
}
#define RuntimeThreadSuspended(x, y)  Use_RuntimeThreadSuspendedWrapper_instead
#endif // PROFILING_SUPPORTED


ETaskType GetCurrentTaskType();

class LeaveRuntimeHolder
{
public:
    LeaveRuntimeHolder(size_t target)
    {
        Thread::LeaveRuntime(target);
    }
    ~LeaveRuntimeHolder()
    {
        Thread::EnterRuntime();
    }
};

class LeaveRuntimeHolderNoThrow
{
public:
    LeaveRuntimeHolderNoThrow(size_t target)
    {
        hr = Thread::LeaveRuntimeNoThrow(target);
    }
    ~LeaveRuntimeHolderNoThrow()
    {
        hr = Thread::EnterRuntimeNoThrow();
    }

    HRESULT GetHR()
    {
        LEAF_CONTRACT;
        return hr;
    }


private:
    HRESULT hr;
};

IHostTask *GetCurrentHostTask();

class ThreadStateHolder
{
public:
    ThreadStateHolder (BOOL fNeed, DWORD state)
    {
        _ASSERTE (GetThread());
        m_fNeed = fNeed;
        m_state = state;
    }
    ~ThreadStateHolder ()
    {
        if (m_fNeed)
        {
            Thread *pThread = GetThread();
            _ASSERTE (pThread);
            FastInterlockAnd((ULONG *) &pThread->m_State, ~m_state);

        }
    }
private:
    BOOL m_fNeed;
    DWORD m_state;
};

typedef Thread::AVInRuntimeImplOkayHolder AVInRuntimeImplOkayHolder;


// ---------------------------------------------------------------------------
//
//      The ThreadStore manages all the threads in the system.
//
// There is one ThreadStore in the system, available through
// ThreadStore::m_pThreadStore.
// ---------------------------------------------------------------------------

typedef SList<Thread, offsetof(Thread, m_LinkStore), false, PTR_Thread> ThreadList;


// The ThreadStore is a singleton class
#define CHECK_ONE_STORE()       _ASSERTE(this == ThreadStore::s_pThreadStore);

typedef DPTR(class ThreadStore) PTR_ThreadStore;
typedef DPTR(class ExceptionTracker) PTR_ExceptionTracker;

class ThreadStore
{
    friend class Thread;
    friend Thread* SetupThread(BOOL);
    friend class AppDomain;
#ifdef DACCESS_COMPILE
    friend class ClrDataAccess;
    friend Thread* __stdcall DacGetThread(void);
    friend Thread* __stdcall DacGetThread(ULONG32 osThreadID);
#endif

public:

    ThreadStore();

    static void InitThreadStore();
    static void LockThreadStore(GCHeap::SUSPEND_REASON reason);
    static void UnlockThreadStore(BOOL bThreadDestroyed = FALSE,
                                  GCHeap::SUSPEND_REASON reason = GCHeap::SUSPEND_OTHER);

    // NOTE!!! use holder class - DLSLockHolder
    static void LockDLSHash(StateHolderParam);
    static void UnlockDLSHash(StateHolderParam);

    // Add a Thread to the ThreadStore
    static void AddThread(Thread *newThread);

    // RemoveThread finds the thread in the ThreadStore and discards it.
    static BOOL RemoveThread(Thread *target);

    // Transfer a thread from the unstarted to the started list.
    static void TransferStartedThread(Thread *target);

    // Before using the thread list, be sure to take the critical section.  Otherwise
    // it can change underneath you, perhaps leading to an exception after Remove.
    // Prev==NULL to get the first entry in the list.
    static Thread *GetAllThreadList(Thread *Prev, ULONG mask, ULONG bits);
    static Thread *GetThreadList(Thread *Prev);

    // Every EE process can lazily create a GUID that uniquely identifies it (for
    // purposes of remoting).
    const GUID    &GetUniqueEEId();

    enum ThreadStoreState
    {
        TSS_Normal       = 0,
        TSS_ShuttingDown = 1,

    }              m_StoreState;

    // We shut down the EE when the last non-background thread terminates.  This event
    // is used to signal the main thread when this condition occurs.
    void            WaitForOtherThreads();
    static void     CheckForEEShutdown();
    CLREvent        m_TerminationEvent;

    // Have all the foreground threads completed?  In other words, can we release
    // the main thread?
    BOOL        OtherThreadsComplete()
    {
        LEAF_CONTRACT;
        _ASSERTE(m_ThreadCount - m_UnstartedThreadCount - m_DeadThreadCount - Thread::m_ActiveDetachCount + m_PendingThreadCount >= m_BackgroundThreadCount);

        return (m_ThreadCount - m_UnstartedThreadCount - m_DeadThreadCount
                - Thread::m_ActiveDetachCount + m_PendingThreadCount
                == m_BackgroundThreadCount);
    }

    // If you want to trap threads re-entering the EE (be this for GC, or debugging,
    // or Thread.Suspend() or whatever, you need to TrapReturningThreads(TRUE).  When
    // you are finished snagging threads, call TrapReturningThreads(FALSE).  This
    // counts internally.
    //
    // Of course, you must also fix RareDisablePreemptiveGC to do the right thing
    // when the trap occurs.
    static void     TrapReturningThreads(BOOL yes);

    // This is used to avoid thread starvation if non-GC threads are competing for
    // the thread store lock when there is a real GC-thread waiting to get in.
    // This is initialized lazily when the first non-GC thread backs out because of
    // a waiting GC thread.  The s_hAbortEvtCache is used to store the handle when
    // it is not being used.
    static CLREvent *s_hAbortEvt;
    static CLREvent *s_hAbortEvtCache;

    Crst *GetDLSHashCrst()
    {
        LEAF_CONTRACT;
#ifndef _DEBUG
        return NULL;
#else
        return &m_HashCrst;
#endif
    }

private:

    // Enter and leave the critical section around the thread store.  Clients should
    // use LockThreadStore and UnlockThreadStore.
    void Enter();
    void Leave();

    // Critical section for adding and removing threads to the store
    Crst        m_Crst;

    // Critical section for adding and removing domain local stores for
    // a thread's hash table.
    Crst        m_HashCrst;
    void EnterDLSHashLock()
    {
        WRAPPER_CONTRACT;
        CHECK_ONE_STORE();
        m_HashCrst.Enter();
    }

    void LeaveDLSHashLock()
    {
        WRAPPER_CONTRACT;
        CHECK_ONE_STORE();
        m_HashCrst.Leave();
    }

    // List of all the threads known to the ThreadStore (started & unstarted).
    ThreadList  m_ThreadList;

    // m_ThreadCount is the count of all threads in m_ThreadList.  This includes
    // background threads / unstarted threads / whatever.
    //
    // m_UnstartedThreadCount is the subset of m_ThreadCount that have not yet been
    // started.
    //
    // m_BackgroundThreadCount is the subset of m_ThreadCount that have been started
    // but which are running in the background.  So this is a misnomer in the sense
    // that unstarted background threads are not reflected in this count.
    //
    // m_PendingThreadCount is used to solve a race condition.  The main thread could
    // start another thread running and then exit.  The main thread might then start
    // tearing down the EE before the new thread moves itself out of m_UnstartedThread-
    // Count in TransferUnstartedThread.  This count is atomically bumped in
    // CreateNewThread, and atomically reduced within a locked thread store.
    //
    // m_DeadThreadCount is the subset of m_ThreadCount which have died.  The Win32
    // thread has disappeared, but something (like the exposed object) has kept the
    // refcount non-zero so we can't destruct yet.
    //
    // m_MaxThreadCount is the maximum value of m_ThreadCount. ie. the largest number
    // of simultaneously active threads

protected:
    LONG        m_ThreadCount;
    LONG        m_MaxThreadCount;
#if defined(_DEBUG) || defined(DACCESS_COMPILE)
public:
    LONG        ThreadCountInEE ()
    {
        LEAF_CONTRACT;
        return m_ThreadCount;
    }
    LONG        MaxThreadCountInEE ()
    {
        LEAF_CONTRACT;
        return m_MaxThreadCount;
    }
#endif
private:
    LONG        m_UnstartedThreadCount;
    LONG        m_BackgroundThreadCount;
    LONG        m_PendingThreadCount;

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
    public:
#endif
    LONG        m_DeadThreadCount;

private:
    // Space for the lazily-created GUID.
    GUID        m_EEGuid;
    BOOL        m_GuidCreated;

    // Even in the release product, we need to know what thread holds the lock on
    // the ThreadStore.  This is so we never deadlock when the GC thread halts a
    // thread that holds this lock.
    Thread     *m_HoldingThread;
    EEThreadId  m_holderthreadid;   // current holder (or NULL)

public:

    static BOOL HoldingThreadStore()
    {
        WRAPPER_CONTRACT;
        // Note that GetThread() may be 0 if it is the debugger thread
        // or perhaps a concurrent GC thread.
        return HoldingThreadStore(GetThread());
    }

    static BOOL HoldingThreadStore(Thread *pThread);

#ifdef DACCESS_COMPILE
    static void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    SPTR_DECL(ThreadStore, s_pThreadStore);

#ifdef _DEBUG
public:
    BOOL        DbgFindThread(Thread *target);
    LONG        DbgBackgroundThreadCount()
    {
        LEAF_CONTRACT;
        return m_BackgroundThreadCount;
    }

    BOOL IsCrstForThreadStore (const CrstBase* const pCrstBase)
    {
        LEAF_CONTRACT;
        return (void *)pCrstBase == (void*)&m_Crst;
    }

#endif
private:
    static CONTEXT *s_pOSContext;
public:
    // We can not do any memory allocation after we suspend a thread in order ot
    // avoid deadlock situation.
    static void AllocateOSContext();
    static CONTEXT *GrabOSContext();
#ifdef _DEBUG
    static BOOL HasAllocatedContext();
#endif

private:
    // Thread abort needs to walk stack to decide if thread abort can proceed.
    // It is unsafe to crawl a stack of thread if the thread is OS-suspended which we do during
    // thread abort.  For example, Thread T1 aborts thread T2.  T2 is suspended by T1. Inside SQL
    // this means that no thread sharing the same scheduler with T2 can run.  If T1 needs a lock which
    // is owned by one thread on the scheduler, T1 will wait forever.
    // Our solution is to move T2 to a safe point, resume it, and then do stack crawl.
    static CLREvent *s_pWaitForStackCrawlEvent;
public:
    static void WaitForStackCrawlEvent()
    {
        LEAF_CONTRACT;
        s_pWaitForStackCrawlEvent->Wait(INFINITE,FALSE);
    }
    static void SetStackCrawlEvent()
    {
        LEAF_CONTRACT;
        s_pWaitForStackCrawlEvent->Set();
    }
    static void ResetStackCrawlEvent()
    {
        LEAF_CONTRACT;
        s_pWaitForStackCrawlEvent->Reset();
    }
};

inline void LockThreadStore(StateHolderParam)
{
    WRAPPER_CONTRACT;
    ThreadStore::LockThreadStore(GCHeap::SUSPEND_OTHER);
}

inline void UnlockThreadStore(StateHolderParam)
{
    WRAPPER_CONTRACT;
    ThreadStore::UnlockThreadStore();
}

typedef StateHolder<LockThreadStore,UnlockThreadStore> ThreadStoreLockHolder;
typedef StateHolder<ThreadStore::LockDLSHash, ThreadStore::UnlockDLSHash> DLSLockHolder;

// This class dispenses small thread ids for the thin lock mechanism
class IdDispenser
{
private:
    SIZE_T      m_highestId;          // highest id given out so far
    SIZE_T      m_recycleBin;         // link list to chain all ids returning to us
    Crst        m_Crst;               // lock to protect our data structures
    DPTR(PTR_Thread)    m_idToThread;         // map thread ids to threads
    DWORD       m_idToThreadCapacity; // capacity of the map

    void GrowIdToThread()
    {
        WRAPPER_CONTRACT;

#ifndef DACCESS_COMPILE
        DWORD newCapacity = m_idToThreadCapacity == 0 ? 16 : m_idToThreadCapacity*2;
        Thread **newIdToThread = new Thread*[newCapacity];

        newIdToThread[0] = NULL;

        for (DWORD i = 1; i < m_idToThreadCapacity; i++)
        {
            newIdToThread[i] = m_idToThread[i];
        }
        for (DWORD j = m_idToThreadCapacity; j < newCapacity; j++)
        {
            newIdToThread[j] = NULL;
        }
        delete m_idToThread;
        m_idToThread = newIdToThread;
        m_idToThreadCapacity = newCapacity;
#else
        DacNotImpl();
#endif // !DACCESS_COMPILE

    }

public:
    IdDispenser() : m_Crst("ThreadIdDispenser", CrstThreadIdDispenser, CRST_UNSAFE_ANYMODE)
    {
        WRAPPER_CONTRACT;
        m_highestId = 0;
        m_recycleBin = 0;
        m_idToThreadCapacity = 0;
        m_idToThread = NULL;
    }

    ~IdDispenser()
    {
        LEAF_CONTRACT;
        delete[] m_idToThread;
    }

    bool IsValidId(DWORD id)
    {
        LEAF_CONTRACT;
        return (id > 0) && (id <= m_highestId);
    }

    DWORD NewId(Thread *pThread)
    {
#ifndef DACCESS_COMPILE
        WRAPPER_CONTRACT;
        DWORD result;
        CrstHolder ch(&m_Crst);

        if (m_recycleBin != 0)
        {
            result = m_recycleBin;
            m_recycleBin = (SIZE_T)m_idToThread[m_recycleBin];
        }
        else
        {
            // we make sure ids don't wrap around - before they do, we always return the highest possible
            // one and rely on our caller to detect this situation
            if (m_highestId + 1 > m_highestId)
                m_highestId = m_highestId + 1;
            result = m_highestId;
            if (result >= m_idToThreadCapacity)
                GrowIdToThread();
        }

        _ASSERTE(result < m_idToThreadCapacity);
        if (result < m_idToThreadCapacity)
            m_idToThread[result] = pThread;

        return result;
#else
        DacNotImpl();
        return 0;
#endif // !DACCESS_COMPILE
    }

    void DisposeId(DWORD id)
    {
#ifndef DACCESS_COMPILE
        LEAF_CONTRACT;
        CrstHolder ch(&m_Crst);

        _ASSERTE(IsValidId(id));
        if (id == m_highestId)
        {
            m_highestId--;
        }
        else
        {
            m_idToThread[id] = (Thread*)m_recycleBin;
            m_recycleBin = id;
        }
#else
        DacNotImpl();
#endif // !DACCESS_COMPILE
    }

    Thread *IdToThread(DWORD id)
    {
        LEAF_CONTRACT;
        CrstHolder ch(&m_Crst);

        Thread *result = NULL;
        if (id < m_idToThreadCapacity)
            result = m_idToThread[id];
        // m_idToThread may have Thread*, or the next free slot
        _ASSERTE ((size_t)result > m_idToThreadCapacity);

        return result;
    }

    Thread *IdToThreadWithValidation(DWORD id)
    {
        WRAPPER_CONTRACT;

        CrstHolder ch(&m_Crst);

        Thread *result = NULL;
        if (id < m_idToThreadCapacity)
            result = m_idToThread[id];
        // m_idToThread may have Thread*, or the next free slot
        if ((size_t)result <= m_idToThreadCapacity)
            result = NULL;
        return result;
    }
};
typedef DPTR(IdDispenser) PTR_IdDispenser;

// Dispenser of small thread ids for thin lock mechanism
GPTR_DECL(IdDispenser,g_pThinLockThreadIdDispenser);


// forward declaration
DWORD MsgWaitHelper(int numWaiters, HANDLE* phEvent, BOOL bWaitAll, DWORD millis, BOOL alertable = FALSE);

// When a thread is being created after a debug suspension has started, it sends an event up to the
// debugger. Afterwards, with the Debugger Lock still held, it will check to see if we had already asked to suspend the
// Runtime. If we have, then it will turn around and call this to set the debug suspend pending flag on the newly
// created thread, since it was missed by SysStartSuspendForDebug as it didn't exist when that function was run.
//
inline void Thread::MarkForDebugSuspend(void)
{
    WRAPPER_CONTRACT;
    if (!(m_State & TS_DebugSuspendPending))
    {
        FastInterlockOr((ULONG *) &m_State, TS_DebugSuspendPending);
        ThreadStore::TrapReturningThreads(TRUE);
    }
}

// Debugger per-thread flag for enabling notification on "manual"
// method calls, for stepping logic.

inline void Thread::IncrementTraceCallCount()
{
    WRAPPER_CONTRACT;
    FastInterlockIncrement(&m_TraceCallCount);
    ThreadStore::TrapReturningThreads(TRUE);
}

inline void Thread::DecrementTraceCallCount()
{
    WRAPPER_CONTRACT;
    ThreadStore::TrapReturningThreads(FALSE);
    FastInterlockDecrement(&m_TraceCallCount);
}

// When we enter an Object.Wait() we are logically inside the synchronized
// region of that object.  Of course, we've actually completely left the region,
// or else nobody could Notify us.  But if we throw ThreadInterruptedException to
// break out of the Wait, all the catchers are going to expect the synchronized
// state to be correct.  So we carry it around in case we need to restore it.
struct PendingSync
{
    LONG            m_EnterCount;
    WaitEventLink  *m_WaitEventLink;
#ifdef _DEBUG
    Thread         *m_OwnerThread;
#endif

    PendingSync(WaitEventLink *s) : m_WaitEventLink(s)
    {
        WRAPPER_CONTRACT;
#ifdef _DEBUG
        m_OwnerThread = GetThread();
#endif
    }
    void Restore(BOOL bRemoveFromSB);
};

// Per-domain local data store
class LocalDataStore
{
public:
    friend class ThreadNative;

    LocalDataStore()
    {
        WRAPPER_CONTRACT;
        m_ExposedTypeObject = CreateGlobalHandle(NULL);
    }

    ~LocalDataStore()
    {
        WRAPPER_CONTRACT;
        // Destroy the class object...
        if(m_ExposedTypeObject != NULL) {
            DestroyGlobalHandle(m_ExposedTypeObject);
            m_ExposedTypeObject = NULL;
        }
    }

    OBJECTREF GetRawExposedObject()
    {
        WRAPPER_CONTRACT;
        return ObjectFromHandle(m_ExposedTypeObject);
    }

protected:

    OBJECTHANDLE   m_ExposedTypeObject;
};

#define INCTHREADLOCKCOUNT()                                    \
{                                                               \
        Thread *thread = GetThread();                           \
        if (thread)                                             \
            thread->IncLockCount();                             \
}

#define DECTHREADLOCKCOUNT( )                                   \
{                                                               \
        Thread *thread = GetThread();                           \
        if (thread)                                             \
            thread->DecLockCount();                             \
}

inline void INCTHREADNONHOSTLOCKCOUNT(Thread *thread)
{
    WRAPPER_CONTRACT;
    if (thread)
        thread->IncNonHostLockCount();
}

inline void DECTHREADNONHOSTLOCKCOUNT(Thread *thread)
{
    WRAPPER_CONTRACT;
    if (thread)
        thread->DecNonHostLockCount();
}

// Holder class for NonHostLockCount in Thread.
class NonHostLockCountHolder : public Holder<Thread *,
                                      INCTHREADNONHOSTLOCKCOUNT,
                                      DECTHREADNONHOSTLOCKCOUNT>
{
public:
    NonHostLockCountHolder(Thread *pThread)
        : Holder<Thread *,
                  INCTHREADNONHOSTLOCKCOUNT,
                  DECTHREADNONHOSTLOCKCOUNT>(pThread, TRUE)
    {
        WRAPPER_CONTRACT;
        _ASSERTE(pThread);
    }
};


// --------------------------------------------------------------------------------
// GCHolder is used to implement the normal GCX_ macros.
//
// GCHolder is normally used indirectly through GCX_ convenience macros, but can be used
// directly if needed (e.g. due to multiple holders in one scope, or to use
// in class definitions).
//
// GCHolder (or derived types) should only be instantiated as automatic variables
// --------------------------------------------------------------------------------

template <BOOL COOPERATIVE, BOOL THREAD_EXISTS, BOOL HACK_NO_THREAD = FALSE>
class GCHolder
{
public:
    // conditional parameter allows a runtime switch on whether to actually affect the gc mode or not,
    // as it can be awkward to conditionally instantiate a holder object

    GCHolder(BOOL conditional = TRUE
           )
    {
        WRAPPER_CONTRACT;


        // Get thread.
        if (THREAD_EXISTS)
        {
            _ASSERTE(GetThread() != NULL);
        }
        m_Thread = GetThread();

        // If you're switching to cooperative, you must have a thread setup.
        // Ignore the HACK_NO_THREAD business. It's going away.

        if (THREAD_EXISTS || m_Thread != NULL)
        {
            m_WasCoop = m_Thread->PreemptiveGCDisabled();

            if (conditional && (COOPERATIVE ? !m_WasCoop : m_WasCoop))
            {
                if (COOPERATIVE)
                    m_Thread->DisablePreemptiveGC();
                else
                    m_Thread->EnablePreemptiveGC();

                _ASSERTE(COOPERATIVE == !!(m_Thread->PreemptiveGCDisabled()));
            }
        }
        else
            m_WasCoop = FALSE;
    }

    // Special perf ctor: only use with THREAD_EXISTS=TRUE and HACK_NO_THREAD=FALSE.
    GCHolder(Thread *pThread
           , BOOL conditional = TRUE
            )
    {
        WRAPPER_CONTRACT;
        _ASSERTE(THREAD_EXISTS && !HACK_NO_THREAD);


        // This is the perf version. So we deliberately restrict the calls
        // to already setup threads to avoid the null check.
        _ASSERTE(pThread);

        // We only required the caller to give us the thread so that the
        // GetThread() call could be hoisted out.
        _ASSERTE(pThread == GetThread());





        m_Thread = pThread;

        m_WasCoop = m_Thread->PreemptiveGCDisabled();

        if (conditional && (COOPERATIVE ? !m_WasCoop : m_WasCoop))
        {
            if (COOPERATIVE)
                m_Thread->DisablePreemptiveGC();
            else
                m_Thread->EnablePreemptiveGC();

            _ASSERTE(COOPERATIVE == !!(m_Thread->PreemptiveGCDisabled()));
        }

    }

    ~GCHolder()
    {
        WRAPPER_CONTRACT;
        Pop();

        if (THREAD_EXISTS || m_Thread != NULL)
        {
            _ASSERTE(m_WasCoop == !!(m_Thread->PreemptiveGCDisabled()));
        }
    }

    void Pop()
    {
        WRAPPER_CONTRACT;


        if (m_WasCoop)
        {
            _ASSERTE(m_Thread != NULL);  // Cannot switch to cooperative with no thread

            if (!m_Thread->PreemptiveGCDisabled())
                m_Thread->DisablePreemptiveGC();
        }
        else
        {
            if (THREAD_EXISTS || m_Thread != NULL)
            {
                if (m_Thread->PreemptiveGCDisabled())
                    m_Thread->EnablePreemptiveGC();
            }
        }
    }


private:
    Thread *m_Thread;
    BOOL    m_WasCoop;

};

// --------------------------------------------------------------------------------
// GCAssert is used to implement the assert GCX_ macros. Usage is similar to GCHolder.
//
// GCAsserting for preemptive mode automatically passes on unmanaged threads.
//
// Note that the assert is "2 sided"; it happens on entering and on leaving scope, to
// help ensure mode integrity.
//
// GCAssert is a noop in a free build
// --------------------------------------------------------------------------------

template<BOOL COOPERATIVE>
class GCAssert
{
#ifdef _DEBUG_IMPL
    public:
    GCAssert()
    {
        WRAPPER_CONTRACT;
        DoCheck();
    }
    ~GCAssert()
    {
        WRAPPER_CONTRACT;
        //
    }

    private:
    void DoCheck()
    {
        WRAPPER_CONTRACT;
        Thread *pThread = GetThread();
        if (COOPERATIVE)
        {
            _ASSERTE(pThread != NULL);
            _ASSERTE(pThread->PreemptiveGCDisabled());
        }
        else
        {
            _ASSERTE(pThread == NULL || !(pThread->PreemptiveGCDisabled()));
        }
    }
#endif
};

// --------------------------------------------------------------------------------
// GCForbid is used to add ForbidGC semantics to the current GC mode.  Note that
// it requires the thread to be in cooperative mode already.
//
// GCForbid is a noop in a free build
// --------------------------------------------------------------------------------

class GCForbid : GCAssert<TRUE>
{
};

// --------------------------------------------------------------------------------
// GCNoTrigger is used to add NoTriggerGC semantics to the current GC mode.  Unlike
// GCForbid, it does not require a thread to be in cooperative mode.
//
// GCNoTrigger is a noop in a free build
// --------------------------------------------------------------------------------

class GCNoTrigger
{
};


#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(Frame*);
template BOOL CompareDefault(Frame*,Frame*);
#endif
typedef Holder<Frame*, DoNothing, COMPlusCooperativeTransitionHandler> CoopTransitionHolder;

// --------------------------------------------------------------------------------
// GCX macros - see util.hpp
// --------------------------------------------------------------------------------

#ifdef _DEBUG_IMPL

// Normally, any thread we operate on has a Thread block in its TLS.  But there are
// a few special threads we don't normally execute managed code on.
BOOL dbgOnly_IsSpecialEEThread();
void dbgOnly_IdentifySpecialEEThread();

#ifdef USE_CHECKED_OBJECTREFS
#define ASSERT_PROTECTED(objRef)        Thread::ObjectRefProtected(objRef)
#else
#define ASSERT_PROTECTED(objRef)
#endif

#else

#define ASSERT_PROTECTED(objRef)

#endif


#define BEGINFORBIDGC()
#define ENDFORBIDGC()
#define TRIGGERSGC_NOSTOMP() ANNOTATION_GC_TRIGGERS
#define TRIGGERSGC() ANNOTATION_GC_TRIGGERS

inline BOOL GC_ON_TRANSITIONS(BOOL val) {
        return FALSE;
}


#ifdef _DEBUG
inline void ENABLESTRESSHEAP() {
    WRAPPER_CONTRACT;
    Thread *thread = GetThread();
    if (thread) {
        thread->EnableStressHeap();
    }
}

void CleanStackForFastGCStress ();
#define CLEANSTACKFORFASTGCSTRESS()                                         \
if (g_pConfig->GetGCStressLevel() && g_pConfig->FastGCStressLevel() > 1) {   \
    CleanStackForFastGCStress ();                                            \
}

#else   // _DEBUG
#define CLEANSTACKFORFASTGCSTRESS()

#endif  // _DEBUG



inline void DoAcquireCheckpoint(void*)
{
    LEAF_CONTRACT;
}

inline void DoReleaseCheckpoint(void *checkPointMarker)
{
    WRAPPER_CONTRACT;
    GetThread()->m_MarshalAlloc.Collapse(checkPointMarker);
}


// CheckPointHolder : Back out to a checkpoint on the thread allocator.
typedef Holder<void*, DoAcquireCheckpoint, DoReleaseCheckpoint> CheckPointHolder;


#ifdef _DEBUG_IMPL
// Holder for incrementing the ForbidGCLoaderUse counter.
class GCForbidLoaderUseHolder
{
 public:
    GCForbidLoaderUseHolder()
    {
        WRAPPER_CONTRACT;
        ClrFlsSetValue(TlsIdx_ForbidGCLoaderUseCount, (void*)(((UINT_PTR)(ClrFlsGetValue(TlsIdx_ForbidGCLoaderUseCount))) + 1));
        }

    ~GCForbidLoaderUseHolder()
    {
        WRAPPER_CONTRACT;
        ClrFlsSetValue(TlsIdx_ForbidGCLoaderUseCount, (void*)(((UINT_PTR)(ClrFlsGetValue(TlsIdx_ForbidGCLoaderUseCount))) - 1));
        }
};
#endif

// Declaring this macro turns off the upfront GC_TRIGGERS and THROWS assertion in LoadTypeHandle.
// If you do this, you must either set TokenNotToLoad to tdAllTypes or restrict
// your use of the loader only to retrieve TypeHandles for types that have already
// been loaded and resolved. If you fail to observe this restriction, you will
// reach a GC_TRIGGERS point somewhere in the loader and assert. If you're lucky, that is.
// (If you're not lucky, you will introduce a GC hole.)
//
// I know of four legitimate uses of this macro:
//
//   - The stack crawl during GC itself (must crawl signatures and retrieve typehandles
//     for valuetypes in method parameters.)
//
//   - Resolving managed EH clauses. We're in a forbidgc situation here.
//     We must retrieve the typehandle of the EH type so we can compare it to the
//     thrown object's type. But if that type isn't loaded, we can safely ignore it
//     and move on.
//
//   - Debug stack walking. We only need to update the reg display which again
//     requires sig walking and passive typehandle retrieval. But the whole runtime
//     is suspended so the debugger is allowed to enter this code in all kinds of
//     thread configurations that would normally disallow it.
//
//   - Generics: This one isn't actually entering the loader per se, but it
//     enters MetaSig::GetTypeHandle() which has the same kind of contract. It
//     does so for the purpose of parsing ELEMENT_TYPE_VAR sig elements and
//     retrieving a TypeHandle from an array of pre-resolved TypeHandles.
//
#define ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE()    ;
// This macro lets us define a conditional CONTRACT for the GC_TRIGGERS behavior.
// This is for the benefit of a select group of callers that use the loader
// in ForbidGC mode strictly to retrieve existing TypeHandles. The reason
// we use a threadstate rather than an extra parameter is that these annoying
// callers call the loader through intermediaries (MetaSig) and it proved to be too
// cumbersome to pass this state down through all those callers.
//
// Don't make GC_TRIGGERS conditional just because your function ends up calling
// LoadTypeHandle indirectly. We don't want to proliferate conditonal contracts more
// than necessary so declare such functions as GC_TRIGGERS until the need
// for the conditional contract is actually proven through code inspection or
// coverage.
#ifdef _DEBUG_IMPL
#define FORBIDGC_LOADER_USE_ENABLED() (ClrFlsGetValue(TlsIdx_ForbidGCLoaderUseCount))
#else   // _DEBUG_IMPL

// If you got an error about FORBIDGC_LOADER_USE_ENABLED being undefined, it's because you tried
// to use this predicate in a free build outside of a CONTRACT or ASSERT.
//
#define FORBIDGC_LOADER_USE_ENABLED() (sizeof(YouCannotUseThisHere) != 0)
#endif  // _DEBUG_IMPL

#ifdef _DEBUG_IMPL
inline void NO_FORBIDGC_LOADER_USE_ThrowSO()
{
    WRAPPER_CONTRACT;
    if (FORBIDGC_LOADER_USE_ENABLED())
    {
        _ASSERTE(!"Unexpected SO, please read the comment");
    }
    else
        COMPlusThrowSO();
}
#else
inline void NO_FORBIDGC_LOADER_USE_ThrowSO()
{
        COMPlusThrowSO();
}
#endif

// There is an MDA which can detect illegal reentrancy into the CLR.  For instance, if you call managed
// code from a native vectored exception handler, this might cause a reverse PInvoke to occur.  But if the
// exception was triggered from code that was executing in cooperative GC mode, we now have GC holes and
// general corruption.
BOOL ShouldCheckReentrancy();
BOOL HasIllegalReentrancy();


// This class can be used to "schedule" a culture setting,
//  kicking in when leaving scope or during exception unwinding.
//  Note: during destruction, this can throw.  You have been warned.
class ReturnCultureHolder
{
public:
    ReturnCultureHolder(Thread* pThread, OBJECTREF* culture, BOOL bUICulture)
    {
        CONTRACTL
        {
            WRAPPER(NOTHROW);
            WRAPPER(GC_NOTRIGGER);
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(pThread));
        }
        CONTRACTL_END;

        m_pThread = pThread;
        m_culture = culture;
        m_bUICulture = bUICulture;
        m_acquired = TRUE;
    }

    FORCEINLINE void SuppressRelease()
    {
        m_acquired = FALSE;
    }

    ~ReturnCultureHolder()
    {
        CONTRACTL
        {
            WRAPPER(THROWS);
            WRAPPER(GC_TRIGGERS);
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;

        if (m_acquired)
            m_pThread->SetCulture(m_culture, m_bUICulture);
    }

private:
    ReturnCultureHolder()
    {
        LEAF_CONTRACT;
    }

    Thread* m_pThread;
    OBJECTREF* m_culture;
    BOOL m_bUICulture;
    BOOL m_acquired;
};


//

//
#define ENTER_DOMAIN_SETUPVARS(_pThread, _predicate_expr)                                       \
{                                                                                               \
    DEBUG_ASSURE_NO_RETURN_BEGIN                                                                \
                                                                                                \
    Thread*     _ctx_trans_pThread          = (_pThread);                                       \
    bool        _ctx_trans_fTransitioned    = false;                                            \
    bool        _ctx_trans_fPredicate       = (_predicate_expr);                                \
    bool        _ctx_trans_fRaiseNeeded     = false;                                            \
    Exception* _ctx_trans_pTargetDomainException=NULL;                   \
    ADID _ctx_trans_pDestDomainId=ADID(0);                                               \
                                                                                                \
    FrameWithCookie<ContextTransitionFrame> _ctx_trans_Frame;                                                   \
    ContextTransitionFrame* _ctx_trans_pFrame = &_ctx_trans_Frame;                              \

#define ENTER_DOMAIN_SWITCH_CTX_BY_ADID(_pCurrDomainPtr,_pDestDomainId,_bUnsafePoint)           \
    AppDomain* _ctx_trans_pCurrDomain=_pCurrDomainPtr;                                          \
    _ctx_trans_pDestDomainId=(ADID)_pDestDomainId;                                               \
    BOOL _ctx_trans_bUnsafePoint=_bUnsafePoint;                                                 \
    if (_ctx_trans_fPredicate &&                                                                \
        (_ctx_trans_pCurrDomain==NULL ||                                                        \
            (_ctx_trans_pCurrDomain->GetId() != _ctx_trans_pDestDomainId)))                     \
    {                                                                                           \
        AppDomainFromIDHolder _ctx_trans_ad(_ctx_trans_pDestDomainId,_ctx_trans_bUnsafePoint);  \
        _ctx_trans_ad.ThrowIfUnloaded();                                                        \
                                                                                                \
        _ctx_trans_ad->EnterContext(_ctx_trans_pThread,                                         \
            _ctx_trans_ad->GetDefaultContext(),                                                 \
            _ctx_trans_pFrame);                                                                 \
                                                                                                \
        _ctx_trans_ad.Release();                                                                \
        _ctx_trans_fTransitioned = true;                                                        \
    }

#define ENTER_DOMAIN_SWITCH_CTX_BY_ADPTR(_pCurrDomain,_pDestDomain)                             \
    AppDomain* _ctx_trans_pCurrDomain=_pCurrDomain;                                             \
    AppDomain* _ctx_trans_pDestDomain=_pDestDomain;                                             \
    _ctx_trans_pDestDomainId=_ctx_trans_pDestDomain->GetId();                  \
                                                                                                \
    if (_ctx_trans_fPredicate && (_ctx_trans_pCurrDomain != _ctx_trans_pDestDomain))            \
    {                                                                                           \
        TESTHOOKCALL(AppDomainCanBeUnloaded(_ctx_trans_pDestDomain->GetId().m_dwId,FALSE));		\
        GCX_FORBID();                                                                           \
        if (!_ctx_trans_pDestDomain->CanThreadEnter(_ctx_trans_pThread))                        \
            COMPlusThrow(kAppDomainUnloadedException);                                          \
                                                                                                \
        _ctx_trans_pThread->EnterContextRestricted(                                             \
            _ctx_trans_pDestDomain->GetDefaultContext(),                                                                 \
            _ctx_trans_pFrame);                                                                 \
                                                                                                \
        _ctx_trans_fTransitioned = true;                                                        \
    }




#define ENTER_DOMAIN_SETUP_EH                                                                   \
    /* work around unreachable code warning */                                                  \
    if (true) EX_TRY                                                                            \
    {                                                                                           \
        LOG((LF_APPDOMAIN, LL_INFO1000, "ENTER_DOMAIN(%s, %s, %d): %s\n",                              \
            __FUNCTION__, __FILE__, __LINE__,                                                   \
            _ctx_trans_fTransitioned ? "ENTERED" : "NOP"));


#define END_DOMAIN_TRANSITION                                                                   \
        TESTHOOKCALL(LeavingAppDomain(::GetAppDomain()->GetId().m_dwId)); \
    }                                                                                           \
    EX_CATCH                                                                                    \
    {                                                                                           \
        LOG((LF_EH|LF_APPDOMAIN, LL_INFO1000, "ENTER_DOMAIN(%s, %s, %d): exception in flight\n",             \
            __FUNCTION__, __FILE__, __LINE__));                                                 \
                                                                                                \
        if (!_ctx_trans_fTransitioned)                                                          \
        {                                                                                       \
            if (_ctx_trans_pThread->PreemptiveGCDisabled())                                     \
            {                                                                                   \
                _ctx_trans_pThread->EnablePreemptiveGC();                                       \
            }                                                                                   \
                                                                                                \
             EX_RETHROW;                                                                         \
        }                                                                                       \
                                                                                                \
        _ctx_trans_pTargetDomainException=EXTRACT_EXCEPTION();                                                           \
        _ctx_trans_fRaiseNeeded = true;                                                         \
    }                                                                                           \
    /* SwallowAllExceptions is fine because we don't get to this point */                       \
    /* unless fRaiseNeeded = true or no exception was thrown */                                 \
    EX_END_CATCH(SwallowAllExceptions);                                                         \
    if (_ctx_trans_fRaiseNeeded)                                                                \
    {                                                                                           \
        LOG((LF_EH, LL_INFO1000, "RaiseCrossContextException(%s, %s, %d)\n",                    \
            __FUNCTION__, __FILE__, __LINE__));                                                 \
        _ctx_trans_pThread->RaiseCrossContextException(_ctx_trans_pTargetDomainException, _ctx_trans_pFrame);                       \
    }                                                                                           \
                                                                                                \
    LOG((LF_APPDOMAIN, LL_INFO1000, "LEAVE_DOMAIN(%s, %s, %d)\n",                                      \
            __FUNCTION__, __FILE__, __LINE__));                                                 \
                                                                                                \
    if (_ctx_trans_fTransitioned)                                                               \
    {                                                                                           \
        GCX_FORBID();                                                                           \
        _ctx_trans_pThread->ReturnToContext(_ctx_trans_pFrame);                                 \
    }                                                                                           \
    TESTHOOKCALL(LeftAppDomain(_ctx_trans_pDestDomainId.m_dwId));                                           \
    DEBUG_ASSURE_NO_RETURN_END                                                                  \
}

//current ad, always safe
#define ADV_CURRENTAD   0
//default ad, never unloaded
#define ADV_DEFAULTAD   1
// held by iterator, iterator holds a ref
#define ADV_ITERATOR    2
// the appdomain is on the stack
#define ADV_RUNNINGIN   4
// we're in process of creating the appdomain, refcount guaranteed to be >0
#define ADV_CREATING    8
// compilation domain - ngen guarantees it won't be unloaded until everyone left
#define ADV_COMPILATION  0x10
// finalizer thread - synchronized with ADU
#define ADV_FINALIZER     0x40
// adu thread - cannot race with itself
#define ADV_ADUTHREAD   0x80
// held by AppDomainRefTaker
#define ADV_REFTAKER    0x100


#ifdef _DEBUG
void CheckADValidity(AppDomain* pDomain, DWORD ADValidityKind);
#else
#define CheckADValidity(pDomain,ADValidityKind)
#endif

#define ENTER_DOMAIN_ID_PREDICATED(_pDestDomain,_predicate_expr) \
    TESTHOOKCALL(EnteringAppDomain(_pDestDomain.m_dwId))	;	\
    ENTER_DOMAIN_SETUPVARS(GetThread(), _predicate_expr) \
    ENTER_DOMAIN_SWITCH_CTX_BY_ADID(_ctx_trans_pThread->GetDomain(), _pDestDomain, FALSE) \
    ENTER_DOMAIN_SETUP_EH	\
    TESTHOOKCALL(EnteredAppDomain(_pDestDomain.m_dwId));

#define ENTER_DOMAIN_PTR_PREDICATED(_pDestDomain,ADValidityKind,_predicate_expr) \
    TESTHOOKCALL(EnteringAppDomain((_pDestDomain)->GetId().m_dwId)); \
    ENTER_DOMAIN_SETUPVARS(GetThread(), _predicate_expr) \
    CheckADValidity(_ctx_trans_fPredicate?(_pDestDomain):GetAppDomain(),ADValidityKind);      \
    ENTER_DOMAIN_SWITCH_CTX_BY_ADPTR(_ctx_trans_pThread->GetDomain(), _pDestDomain) \
    ENTER_DOMAIN_SETUP_EH	\
    TESTHOOKCALL(EnteredAppDomain((_pDestDomain)->GetId().m_dwId));


#define ENTER_DOMAIN_PTR(_pDestDomain,ADValidityKind) \
    TESTHOOKCALL(EnteringAppDomain((_pDestDomain)->GetId().m_dwId)); \
    CheckADValidity(_pDestDomain,ADValidityKind);      \
    ENTER_DOMAIN_SETUPVARS(GetThread(), true) \
    ENTER_DOMAIN_SWITCH_CTX_BY_ADPTR(_ctx_trans_pThread->GetDomain(), _pDestDomain) \
    ENTER_DOMAIN_SETUP_EH   \
    TESTHOOKCALL(EnteredAppDomain((_pDestDomain)->GetId().m_dwId));

#define ENTER_DOMAIN_ID(_pDestDomain) \
    ENTER_DOMAIN_ID_PREDICATED(_pDestDomain,true)


#define GET_CTX_TRANSITION_FRAME() \
    (_ctx_trans_pFrame)

//-----------------------------------------------------------------------------
// System to make Cross-Appdomain calls.
//
// Cross-AppDomain calls are made via a callback + args. This gives us the flexibility
// to check if a transition is needed, and take fast vs. slow paths for the debugger.
//
// Example usage:
//   struct FooArgs : public CtxTransitionBaseArgs { ... } args (...); // load up args
//   MakeCallWithPossibleAppDomainTransition(pNewDomain, MyFooFunc, &args);
//
// MyFooFunc is always executed in pNewDomain.
// If we're already in pNewDomain, then that just becomes MyFooFunc(&args);
// else we'll switch ADs, and do the proper Try/Catch/Rethrow.
//-----------------------------------------------------------------------------

// All Arg structs should derive from this. This makes certain standard args
// are available (such as the context-transition frame).
// The ADCallback helpers will fill in these base args.
struct CtxTransitionBaseArgs;

// Pointer type for the AppDomain callback function.
typedef void (*FPAPPDOMAINCALLBACK)(
    CtxTransitionBaseArgs*             pData     // Caller's private data
);


//-----------------------------------------------------------------------------
// Call w/a  wrapper.
// We've already transitioned AppDomains here. This just places a 1st-pass filter to sniff
// for catch-handler found callbacks for the debugger.
//-----------------------------------------------------------------------------
void MakeADCallDebuggerWrapper(
    FPAPPDOMAINCALLBACK fpCallback,
    CtxTransitionBaseArgs * args,
    ContextTransitionFrame* pFrame);

// Invoke a callback in another appdomain.
// Caller should have checked that we're actually transitioning domains here.
void MakeCallWithAppDomainTransition(
    ADID pTargetDomain,
    FPAPPDOMAINCALLBACK fpCallback,
    CtxTransitionBaseArgs * args);

// Invoke the callback in the AppDomain.
// Ensure that predicate only gets evaluted once!!
#define MakePredicatedCallWithPossibleAppDomainTransition(pTargetDomain, fPredicate, fpCallback, args) \
{ \
    Thread*     _ctx_trans_pThread          = GetThread(); \
    _ASSERTE(_ctx_trans_pThread != NULL); \
    ADID  _ctx_trans_pCurrDomain      = _ctx_trans_pThread->GetDomain()->GetId(); \
    ADID  _ctx_trans_pDestDomain      = (pTargetDomain);                                   \
    \
    if (fPredicate && (_ctx_trans_pCurrDomain != _ctx_trans_pDestDomain)) \
    { \
        /* Transition domains and make the call */ \
        MakeCallWithAppDomainTransition(pTargetDomain, (FPAPPDOMAINCALLBACK) fpCallback, args); \
    } \
    else      \
    { \
        /* No transition needed. Just call directly.  */ \
        (fpCallback)(args); \
    }\
}

// Invoke the callback in the AppDomain.
#define MakeCallWithPossibleAppDomainTransition(pTargetDomain, fpCallback, args) \
    MakePredicatedCallWithPossibleAppDomainTransition(pTargetDomain, true, fpCallback, args)


struct CtxTransitionBaseArgs
{
    // This function fills out the private base args.
    friend void MakeCallWithAppDomainTransition(
        ADID pTargetDomain,
        FPAPPDOMAINCALLBACK fpCallback,
        CtxTransitionBaseArgs * args);

public:
    CtxTransitionBaseArgs() { pCtxFrame = NULL; }
    // This will be NULL if we didn't actually transition.
    ContextTransitionFrame* GetCtxTransitionFrame() { return pCtxFrame; }
private:
    ContextTransitionFrame* pCtxFrame;
};


// We have numerous places where we start up a managed thread.  This includes several places in the
// ThreadPool, the 'new Thread(...).Start()' case, and the Finalizer.  Try to factor the code so our
// base exception handling behavior is consistent across those places.  The resulting code is convoluted,
// but it's better than the prior situation of each thread being on a different plan.

// If you add a new kind of managed thread (i.e. thread proc) to the system, you must:
//
// 1) Call HasStarted() before calling any ManagedThreadBase_* routine.
// 2) Define a ManagedThreadBase_* routine for your scenario and declare it below.
// 3) Always perform any AD transitions through the ManagedThreadBase_* mechanism.
// 4) Allow the ManagedThreadBase_* mechanism to perform all your exception handling, including
//    dispatching of unhandled exception events, deciding what to swallow, etc.
// 5) If you must separate your base thread proc behavior from your AD transitioning behavior,
//    define a second ManagedThreadADCall_* helper and declare it below.
// 6) Never decide this is too much work and that you will roll your own thread proc code.

// intentionally opaque.
struct ManagedThreadCallState;

struct ManagedThreadBase
{
    // The 'new Thread(...).Start()' case from COMSynchronizable kickoff thread worker
    static void KickOff(ADID pAppDomain,
                        Context::ADCallBackFcnType pTarget,
                        LPVOID args);

    // The IOCompletion, QueueUserWorkItem, AddTimer, RegisterWaitForSingleObject cases in
    // the ThreadPool
    static void ThreadPool(ADID pAppDomain, Context::ADCallBackFcnType pTarget, LPVOID args);

    // The Finalizer thread separates the tasks of establishing exception handling at its
    // base and transitioning into AppDomains.  The turnaround structure that ties the 2 calls together
    // is the ManagedThreadCallState.


    // For the case (like Finalization) where the base transition and the AppDomain transition are
    // separated, an opaque structure is used to tie together the two calls.

    static void FinalizerBase(Context::ADCallBackFcnType pTarget);
    static void FinalizerAppDomain(AppDomain* pAppDomain,
                                   Context::ADCallBackFcnType pTarget,
                                   LPVOID args,
                                   ManagedThreadCallState *pTurnAround);
};


// DeadlockAwareLock is a base for building deadlock-aware locks.
// Note that DeadlockAwareLock only works if ALL locks involved in the deadlock are deadlock aware.

class DeadlockAwareLock
{
 private:
    Thread   * volatile m_pHoldingThread;
#ifdef _DEBUG
    const char  *m_description;
#endif

 public:
    DeadlockAwareLock(const char *description = NULL);
    ~DeadlockAwareLock();

    // Test for deadlock
    BOOL CanEnterLock(Thread *pThread = NULL);

    // Call BeginEnterLock before attempting to acquire the lock
    BOOL TryBeginEnterLock(Thread *pThread = NULL); // returns FALSE if deadlock
    void BeginEnterLock(Thread *pThread = NULL); // Asserts if deadlock

    // Call EndEnterLock after acquiring the lock
    void EndEnterLock(Thread *pThread = NULL);

    // Call LeaveLock after releasing the lock
    void LeaveLock();

    const char *GetDescription();

 private:
    CHECK CheckDeadlock(Thread *pThread);

    static void ReleaseBlockingLock(StateHolderParam)
    {
        Thread *pThread = GetThread();
        _ASSERTE (pThread);
        pThread->m_pBlockingLock = NULL;
    }
public:
    typedef StateHolder<DoNothing,DeadlockAwareLock::ReleaseBlockingLock> BlockingLockHolder;
};

inline Context* GetCurrentContext()
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return GetThread()->GetContext();
}


#endif //__threads_h__
