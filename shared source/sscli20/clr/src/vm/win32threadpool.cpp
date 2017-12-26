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
/*++

Module Name:

    Win32ThreadPool.cpp

Abstract:

    This module implements Threadpool support using Win32 APIs


Revision History:
    December 1999 -                                           - Created

--*/

#include "common.h"
#include "log.h"
#include "win32threadpool.h"
#include "delegateinfo.h"
#include "eeconfig.h"
#include "dbginterface.h"
#include "corhost.h"
#include "eventtrace.h"
#include "threads.h"
#include "appdomain.inl"
#include "nativeoverlapped.h"


//--//

//#define THREADPOOL_PRIVATELOG


#define THREADPOOLLOGGING_EnsureInitialized()
#define THREADPOOLLOGGING_Log(level,a)


//--//

#define TPL_PRIV   1 // Send the entry through the local infrastructure.
#define TPL_STRESS 2 // Send the entry through the stress infrastructure.
#define TPL_LOG    4 // Send the entry through the log infrastructure.

#define THREADPOOL_LOG0(filter, level, msg)                                                                                                 \
    do                                                                                                                                      \
    {                                                                                                                                       \
        if((filter) & TPL_PRIV  ) { THREADPOOLLOGGING_Log(                level, (msg)); }                                                  \
        if((filter) & TPL_STRESS) { STRESS_LOG0          ( LF_THREADPOOL, level,  msg ); }                                                  \
        if((filter) & TPL_LOG   ) { LOG                  ((LF_THREADPOOL, level,  msg)); }                                                  \
    } while(0)

#define THREADPOOL_LOG1(filter, level, msg, data1)                                                                                          \
    do                                                                                                                                      \
    {                                                                                                                                       \
        if((filter) & TPL_PRIV  ) { THREADPOOLLOGGING_Log(                level, (msg, data1)); }                                           \
        if((filter) & TPL_STRESS) { STRESS_LOG1          ( LF_THREADPOOL, level,  msg, data1 ); }                                           \
        if((filter) & TPL_LOG   ) { LOG                  ((LF_THREADPOOL, level,  msg, data1)); }                                           \
    } while(0)

#define THREADPOOL_LOG2(filter, level, msg, data1, data2)                                                                                   \
    do                                                                                                                                      \
    {                                                                                                                                       \
        if((filter) & TPL_PRIV  ) { THREADPOOLLOGGING_Log(                level, (msg, data1, data2)); }                                    \
        if((filter) & TPL_STRESS) { STRESS_LOG2          ( LF_THREADPOOL, level,  msg, data1, data2 ); }                                    \
        if((filter) & TPL_LOG   ) { LOG                  ((LF_THREADPOOL, level,  msg, data1, data2)); }                                    \
    } while(0)

#define THREADPOOL_LOG3(filter, level, msg, data1, data2, data3)                                                                            \
    do                                                                                                                                      \
    {                                                                                                                                       \
        if((filter) & TPL_PRIV  ) { THREADPOOLLOGGING_Log(                level, (msg, data1, data2, data3)); }                             \
        if((filter) & TPL_STRESS) { STRESS_LOG3          ( LF_THREADPOOL, level,  msg, data1, data2, data3 ); }                             \
        if((filter) & TPL_LOG   ) { LOG                  ((LF_THREADPOOL, level,  msg, data1, data2, data3)); }                             \
    } while(0)

#define THREADPOOL_LOG4(filter, level, msg, data1, data2, data3, data4)                                                                     \
    do                                                                                                                                      \
    {                                                                                                                                       \
        if((filter) & TPL_PRIV  ) { THREADPOOLLOGGING_Log(                level, (msg, data1, data2, data3, data4)); }                      \
        if((filter) & TPL_STRESS) { STRESS_LOG4          ( LF_THREADPOOL, level,  msg, data1, data2, data3, data4 ); }                      \
        if((filter) & TPL_LOG   ) { LOG                  ((LF_THREADPOOL, level,  msg, data1, data2, data3, data4)); }                      \
    } while(0)

#define THREADPOOL_LOG5(filter, level, msg, data1, data2, data3, data4, data5)                                                              \
    do                                                                                                                                      \
    {                                                                                                                                       \
        if((filter) & TPL_PRIV  ) { THREADPOOLLOGGING_Log(                level, (msg, data1, data2, data3, data4, data5)); }               \
        if((filter) & TPL_STRESS) { STRESS_LOG5          ( LF_THREADPOOL, level,  msg, data1, data2, data3, data4, data5 ); }               \
        if((filter) & TPL_LOG   ) { LOG                  ((LF_THREADPOOL, level,  msg, data1, data2, data3, data4, data5)); }               \
    } while(0)

#define THREADPOOL_LOG6(filter, level, msg, data1, data2, data3, data4, data5, data6)                                                       \
    do                                                                                                                                      \
    {                                                                                                                                       \
        if((filter) & TPL_PRIV  ) { THREADPOOLLOGGING_Log(                level, (msg, data1, data2, data3, data4, data5, data6)); }        \
        if((filter) & TPL_STRESS) { STRESS_LOG6          ( LF_THREADPOOL, level,  msg, data1, data2, data3, data4, data5, data6 ); }        \
        if((filter) & TPL_LOG   ) { LOG                  ((LF_THREADPOOL, level,  msg, data1, data2, data3, data4, data5, data6)); }        \
    } while(0)

#define THREADPOOL_LOG7(filter, level, msg, data1, data2, data3, data4, data5, data6, data7)                                                \
    do                                                                                                                                      \
    {                                                                                                                                       \
        if((filter) & TPL_PRIV  ) { THREADPOOLLOGGING_Log(                level, (msg, data1, data2, data3, data4, data5, data6, data7)); } \
        if((filter) & TPL_STRESS) { STRESS_LOG7          ( LF_THREADPOOL, level,  msg, data1, data2, data3, data4, data5, data6, data7 ); } \
        if((filter) & TPL_LOG   ) { LOG                  ((LF_THREADPOOL, level,  msg, data1, data2, data3, data4, data5, data6, data7)); } \
    } while(0)

//--//

#ifndef DACCESS_COMPILE
BOOL ThreadpoolMgr::InitCompletionPortThreadpool = FALSE;
HANDLE ThreadpoolMgr::GlobalCompletionPort;                 // used for binding io completions on file handles

#endif // !DACCESS_COMPILE

SVAL_IMPL(LONG,ThreadpoolMgr,NumCPThreads);                          // number of completion port threads
SVAL_IMPL_INIT(LONG,ThreadpoolMgr,MaxLimitTotalCPThreads,1000);           // = MaxLimitCPThreadsPerCPU * number of CPUS
SVAL_IMPL(LONG,ThreadpoolMgr,CurrentLimitTotalCPThreads);            // current limit on total number of CP threads
SVAL_IMPL(LONG,ThreadpoolMgr,MinLimitTotalCPThreads);                // = MinLimitCPThreadsPerCPU * number of CPUS
SVAL_IMPL(LONG,ThreadpoolMgr,NumFreeCPThreads);                      // number of cp threads waiting on the port
SVAL_IMPL(LONG,ThreadpoolMgr,MaxFreeCPThreads);                      // = MaxFreeCPThreadsPerCPU * Number of CPUS
SVAL_IMPL(LONG,ThreadpoolMgr,NumRetiredCPThreads);
LONG  ThreadpoolMgr::NumRetiredWorkerThreads;               // is included in NumIdleWorkerThreads

SVAL_IMPL(int,ThreadpoolMgr,NumWorkerThreads);          // total number of worker threads created
SVAL_IMPL(int,ThreadpoolMgr,MinLimitTotalWorkerThreads);            // = MaxLimitCPThreadsPerCPU * number of CPUS
SVAL_IMPL(DWORD,ThreadpoolMgr,MaxLimitTotalWorkerThreads);              // = MaxLimitCPThreadsPerCPU * number of CPUS
SVAL_IMPL(int,ThreadpoolMgr,NumIdleWorkerThreads);
SVAL_IMPL(LONG,ThreadpoolMgr,NumQueuedWorkRequests);     // number of queued work requests
SVAL_IMPL(DWORD,ThreadpoolMgr,NumTimers);                                          // number of timers in timer queue

SVAL_IMPL(long,ThreadpoolMgr,cpuUtilization);
long    ThreadpoolMgr::cpuUtilizationAverage = 0;
DWORD    ThreadpoolMgr::CompletedWorkRequests = 0;
DWORD    ThreadpoolMgr::PriorCompletedWorkRequests = 0;
float   ThreadpoolMgr::PriorWorkRequestRate = 0;

#ifndef DACCESS_COMPILE

#define INVALID_HANDLE ((HANDLE) -1)
#define NEW_THREAD_THRESHOLD            7       // Number of requests outstanding before we start a new thread
#define CP_THREAD_PENDINGIO_WAIT 5000           // polling interval when thread is retired but has a pending io
#define GATE_THREAD_DELAY 500 /*milliseconds*/
#define DELAY_BETWEEN_SUSPENDS 5000 + GATE_THREAD_DELAY // time to delay between suspensions
#define SUSPEND_TIME GATE_THREAD_DELAY+100      // milliseconds to suspend during SuspendProcessing

LONG ThreadpoolMgr::Initialization=0;           // indicator of whether the threadpool is initialized.
LONG ThreadpoolMgr::ThreadInSuspend=0;          // indicates if a thread has suspended processing
int ThreadpoolMgr::LastRecordedQueueLength;     // captured by GateThread, used on Win9x to detect thread starvation
unsigned int ThreadpoolMgr::LastDequeueTime;    // used to determine if work items are getting thread starved
unsigned int ThreadpoolMgr::LastCompletionTime; // used to determine if io completions are getting thread starved
unsigned int ThreadpoolMgr::LastSuspendTime;    // to prevent multiple threads from suspending in a short time period
BOOL ThreadpoolMgr::MonitorWorkRequestsQueue=0; // if 1, the gate thread monitors progress of WorkRequestQueue to prevent starvation due to blocked worker threads
int ThreadpoolMgr::offset_counter = 0;

unsigned int ThreadpoolMgr::LastWorkerStateTime;    // time of last decision
ThreadpoolMgr::WorkerCreationState ThreadpoolMgr::LastWorkerCreationState = ThreadpoolMgr::WORKER_None;
unsigned int ThreadpoolMgr::SequentialRateDecreaseCount = 0;  // used to backoff on thread injection if no progress

#endif //!DACCESS_COMPILE

SPTR_IMPL(WorkRequest,ThreadpoolMgr,WorkRequestHead);        // Head of work request queue
SPTR_IMPL(WorkRequest,ThreadpoolMgr,WorkRequestTail);        // Head of work request queue

SVAL_IMPL(ThreadpoolMgr::LIST_ENTRY,ThreadpoolMgr,TimerQueue);  // queue of timers

#ifndef DACCESS_COMPILE

//unsigned int ThreadpoolMgr::LastCpuSamplingTime=0;      //  last time cpu utilization was sampled by gate thread
unsigned int ThreadpoolMgr::LastWorkerThreadCreation=0; //  last time a worker thread was created
unsigned int ThreadpoolMgr::LastCPThreadCreation=0;     //  last time a completion port thread was created
unsigned int ThreadpoolMgr::NumberOfProcessors; // = NumberOfWorkerThreads - no. of blocked threads


CrstStatic ThreadpoolMgr::WorkerCriticalSection;
CLREvent * ThreadpoolMgr::WorkRequestNotification;
CLREvent * ThreadpoolMgr::RetiredCPWakeupEvent;       // wakeup event for completion port threads
CLREvent * ThreadpoolMgr::RetiredWorkerWakeupEvent;   // wakeup event for worker threads
CrstStatic ThreadpoolMgr::WaitThreadsCriticalSection;
ThreadpoolMgr::LIST_ENTRY ThreadpoolMgr::WaitThreadsHead;
CrstStatic ThreadpoolMgr::EventCacheCriticalSection;
ThreadpoolMgr::LIST_ENTRY ThreadpoolMgr::EventCache;                       // queue of cached events
DWORD ThreadpoolMgr::NumUnusedEvents=0;                                    // number of events in cache

CrstStatic ThreadpoolMgr::TimerQueueCriticalSection;
HANDLE ThreadpoolMgr::TimerThread=NULL;
Thread *ThreadpoolMgr::pTimerThread=NULL;
DWORD ThreadpoolMgr::LastTickCount;
#ifdef _DEBUG
DWORD ThreadpoolMgr::TickCountAdjustment=0;
#endif

LONG  ThreadpoolMgr::GateThreadCreated=GATE_THREAD_STATUS_NOTCREATED;

ThreadpoolMgr::RecycledListsWrapper ThreadpoolMgr::RecycledLists;

ThreadpoolMgr::TimerInfo *ThreadpoolMgr::TimerInfosToBeRecycled = NULL;




// Macros for inserting/deleting from doubly linked list

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

//
// these are named the same as slightly different macros in the NT headers
//
#undef RemoveHeadList
#undef RemoveEntryList
#undef InsertTailList
#undef InsertHeadList

#define RemoveHeadList(ListHead,FirstEntry) \
    {\
    FirstEntry = (LIST_ENTRY*) (ListHead)->Flink;\
    ((LIST_ENTRY*)FirstEntry->Flink)->Blink = (ListHead);\
    (ListHead)->Flink = FirstEntry->Flink;\
    }

#define RemoveEntryList(Entry) {\
    LIST_ENTRY* _EX_Entry;\
        _EX_Entry = (Entry);\
        ((LIST_ENTRY*) _EX_Entry->Blink)->Flink = _EX_Entry->Flink;\
        ((LIST_ENTRY*) _EX_Entry->Flink)->Blink = _EX_Entry->Blink;\
    }

#define InsertTailList(ListHead,Entry) \
    (Entry)->Flink = (ListHead);\
    (Entry)->Blink = (ListHead)->Blink;\
    ((LIST_ENTRY*)(ListHead)->Blink)->Flink = (Entry);\
    (ListHead)->Blink = (Entry);

#define InsertHeadList(ListHead,Entry) {\
    LIST_ENTRY* _EX_Flink;\
    LIST_ENTRY* _EX_ListHead;\
    _EX_ListHead = (LIST_ENTRY*)(ListHead);\
    _EX_Flink = (LIST_ENTRY*) _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

#define SetLastHRError(hr) \
    if (HRESULT_FACILITY(hr) == FACILITY_WIN32)\
        SetLastError(HRESULT_CODE(hr));\
    else \
        SetLastError(ERROR_INVALID_DATA);\

/************************************************************************/


ULONG WINAPI ThreadpoolMgr::RecycledListsWrapper::FallbackGetCurrentProcessorNumber()
{
    LEAF_CONTRACT;

    return 0;
}


void ThreadpoolMgr::RecycledListsWrapper::Initialize( unsigned int numProcs )
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    pRecycledListPerProcessor = new RecycledListInfo[numProcs * MEMTYPE_COUNT];


    pGetCurrentProcessorNumber = &FallbackGetCurrentProcessorNumber;

}

//--//

void ThreadpoolMgr::EnsureInitialized()
{
    CONTRACTL
    {
        THROWS;         // Initialize can throw
        MODE_ANY;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (IsInitialized())
        return;

retry:
    if (InterlockedCompareExchange(&Initialization, 1, 0) == 0)
    {
        if (Initialize())
            Initialization = -1;
        else
        {
            Initialization = 0;
            COMPlusThrowOM();
        }
    }
    else // someone has already begun initializing.
    {
        // wait until it finishes
        while (Initialization != -1)
        {
            __SwitchToThread(0);
            goto retry;
        }
    }
}

BOOL ThreadpoolMgr::Initialize()
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    BOOL bRet = FALSE;
    BOOL bExceptionCaught = FALSE;

    NumberOfProcessors = GetCurrentProcessCpuCount();
    InitPlatformVariables();

    EX_TRY
    {
        WorkerCriticalSection.Init("Threadpool Worker", CrstThreadpoolWorker);
        WaitThreadsCriticalSection.Init("Wait Thread", CrstThreadpoolWaitThreads);
        EventCacheCriticalSection.Init("Threadpool Event Cache", CrstThreadpoolEventCache);
        TimerQueueCriticalSection.Init("Timer Queue", CrstThreadpoolTimerQueue);

        // initialize WaitThreadsHead
        InitializeListHead(&WaitThreadsHead);

        // initialize EventCache
        InitializeListHead(&EventCache);

        // initialize TimerQueue
        InitializeListHead(&TimerQueue);

        WorkRequestNotification = new CLREvent();
        WorkRequestNotification->CreateManualEvent(FALSE);
        _ASSERTE(WorkRequestNotification->IsValid());

        RetiredCPWakeupEvent = new CLREvent();
        RetiredCPWakeupEvent->CreateAutoEvent(FALSE);
        _ASSERTE(RetiredCPWakeupEvent->IsValid());

        RetiredWorkerWakeupEvent = new CLREvent();
        RetiredWorkerWakeupEvent->CreateAutoEvent(FALSE);
        _ASSERTE(RetiredWorkerWakeupEvent->IsValid());

        THREADPOOLLOGGING_EnsureInitialized();


        {
            SYSTEM_INFO sysInfo;

            ::GetSystemInfo( &sysInfo );

            RecycledLists.Initialize( sysInfo.dwNumberOfProcessors );
        }
    }
    EX_CATCH
    {
        if (WorkRequestNotification)
        {
            delete WorkRequestNotification;
            WorkRequestNotification = NULL;
        }
        if (RetiredCPWakeupEvent)
        {
            delete RetiredCPWakeupEvent;
            RetiredCPWakeupEvent = NULL;
        }
        if (RetiredWorkerWakeupEvent)
        {
            delete RetiredWorkerWakeupEvent;
            RetiredWorkerWakeupEvent = NULL;
        }

        // Note: It is fine to call Destroy on unitialized critical sections
        WorkerCriticalSection.Destroy();
        WaitThreadsCriticalSection.Destroy();
        EventCacheCriticalSection.Destroy();
        TimerQueueCriticalSection.Destroy();

        bExceptionCaught = TRUE;
    }
    EX_END_CATCH(SwallowAllExceptions);

    if (bExceptionCaught)
    {
        goto end;
    }

    // initialize Worker and CP thread settings

    MinLimitTotalWorkerThreads = NumberOfProcessors;
    MaxLimitTotalWorkerThreads = NumberOfProcessors*MaxLimitThreadsPerCPU;

#ifdef _DEBUG
    TickCountAdjustment = EEConfig::GetConfigDWORD(L"ThreadpoolTickCountAdjustment",0);
#endif

#ifdef _LOGTOFILE
    logfile = fopen("c:\\tpool.log","w");
#endif

    // initialize CP thread settings
    MinLimitTotalCPThreads = MinLimitTotalWorkerThreads;

    CurrentLimitTotalCPThreads = 0;
#ifdef PLATFORM_UNIX
    // The current default value for MaxLimitTotalCPThreads is 1000
    // which does not scale well on Unix.
    // Set the max limit based on number of processors for Unix platforms.
    // Also, if MaxLimitTotalCPThreads needs to be changed,
    // make sure it is changed in ThreadpoolMgr::GetMaxThreads too.
    MaxLimitTotalCPThreads = NumberOfProcessors*MaxLimitThreadsPerCPU;
#endif // PLATFORM_UNIX
    MaxFreeCPThreads = NumberOfProcessors*MaxFreeCPThreadsPerCPU;
    NumCPThreads = 0;
    NumFreeCPThreads = 0;
    NumRetiredCPThreads = 0;
    NumRetiredWorkerThreads = 0;
    LastCompletionTime = GetTickCount();
    if (!CLRIoCompletionHosted())
    {
        GlobalCompletionPort = PAL_CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                                                      NULL,
                                                      0,        /*ignored for invalid handle value*/
                                                      NumberOfProcessors);
    }
    bRet = TRUE;
end:
    return bRet;
}


void ThreadpoolMgr::InitPlatformVariables()
{
    LEAF_CONTRACT;

}

BOOL ThreadpoolMgr::SetMaxThreadsHelper(DWORD MaxWorkerThreads,
                                        DWORD MaxIOCompletionThreads)
{
    CONTRACTL
    {
        THROWS;     // Crst can throw and toggle GC mode
        MODE_ANY;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    BOOL result = FALSE;

    // doesn't need to be WorkerCS, but using it to avoid race condition between setting min and max, and didn't want to create a new CS.
    CrstHolder csh(&WorkerCriticalSection);

    if (MaxWorkerThreads >= (DWORD)MinLimitTotalWorkerThreads &&
       MaxIOCompletionThreads >= (DWORD)MinLimitTotalCPThreads)
    {
        MaxLimitTotalWorkerThreads = MaxWorkerThreads;
        MaxLimitTotalCPThreads     = MaxIOCompletionThreads;
        result = TRUE;
    }

    return result;
 }

/************************************************************************/
BOOL ThreadpoolMgr::SetMaxThreads(DWORD MaxWorkerThreads,
                                     DWORD MaxIOCompletionThreads)
{
    CONTRACTL
    {
        THROWS;     // SetMaxThreadsHelper can throw and toggle GC mode
        MODE_ANY;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    IHostThreadpoolManager *threadpoolProvider = CorHost2::GetHostThreadpoolManager();
    if (threadpoolProvider) {
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = threadpoolProvider->SetMaxThreads(MaxWorkerThreads);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr))
        {
            SetLastHRError(hr);
            return FALSE;
        }
    }

    IHostIoCompletionManager *ioCompletionProvider = CorHost2::GetHostIoCompletionManager();
    if (ioCompletionProvider) {
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = ioCompletionProvider->SetMaxThreads(MaxIOCompletionThreads);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr))
        {
            SetLastHRError(hr);
            return FALSE;
        }
    }

    if (threadpoolProvider && ioCompletionProvider) {
        return TRUE;
    }

    if (IsInitialized())
    {
        return SetMaxThreadsHelper(MaxWorkerThreads, MaxIOCompletionThreads);
    }

    if (InterlockedCompareExchange(&Initialization, 1, 0) == 0)
    {
        Initialize();

        BOOL helper_result = FALSE;
        helper_result = SetMaxThreadsHelper(MaxWorkerThreads, MaxIOCompletionThreads);

        Initialization = -1;
        return helper_result;
    }
    else // someone else is initializing. Too late, return false
    {
        return FALSE;
    }

}

BOOL ThreadpoolMgr::GetMaxThreads(DWORD* MaxWorkerThreads,
                                     DWORD* MaxIOCompletionThreads)
{
    LEAF_CONTRACT;

    HRESULT hr = S_OK;

    IHostThreadpoolManager *threadpoolProvider = CorHost2::GetHostThreadpoolManager();
    if (threadpoolProvider) {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = threadpoolProvider->GetMaxThreads(MaxWorkerThreads);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr))
        {
            SetLastHRError(hr);
            return FALSE;
        }
    }
    IHostIoCompletionManager *ioCompletionProvider = CorHost2::GetHostIoCompletionManager();
    if (ioCompletionProvider) {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = ioCompletionProvider->GetMaxThreads(MaxIOCompletionThreads);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr))
        {
            SetLastHRError(hr);
            return FALSE;
        }
    }

    if (threadpoolProvider && ioCompletionProvider) {
        return TRUE;
    }

    if (!MaxWorkerThreads || !MaxIOCompletionThreads)
    {
        SetLastHRError(ERROR_INVALID_DATA);
        return FALSE;
    }

    if (IsInitialized())
    {
        *MaxWorkerThreads = MaxLimitTotalWorkerThreads;
        *MaxIOCompletionThreads = MaxLimitTotalCPThreads;
    }
    else
    {
        NumberOfProcessors = GetCurrentProcessCpuCount();

        *MaxWorkerThreads = NumberOfProcessors*MaxLimitThreadsPerCPU;
#ifdef PLATFORM_UNIX
        // MaxLimitTotalCPThreads is adjusted for Unix platform
        // in ThreadpoolMgr::Initialize().
        // Since this method may be called before ThreadpoolMgr::Initialize(),
        // we need to adjust the limit for Unix platform here to make sure
        // the adjusted MaxLimitTotalCPThreads is returned to the caller.
        // Also, if MaxLimitTotalCPThreads needs to be changed,
        // make sure it is changed in ThreadpoolMgr::Initialize() too.
        MaxLimitTotalCPThreads = NumberOfProcessors*MaxLimitThreadsPerCPU;
#endif // PLATFORM_UNIX
        *MaxIOCompletionThreads = MaxLimitTotalCPThreads;
    }
    return TRUE;
}

BOOL ThreadpoolMgr::SetMinThreads(DWORD MinWorkerThreads,
                                     DWORD MinIOCompletionThreads)
{
    CONTRACTL
    {
        THROWS;     // Crst can throw and toggle GC mode
        MODE_ANY;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    IHostThreadpoolManager *threadpoolProvider = CorHost2::GetHostThreadpoolManager();
    if (threadpoolProvider) {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = threadpoolProvider->SetMinThreads(MinWorkerThreads);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr))
        {
            SetLastHRError(hr);
            return FALSE;
        }
    }
    IHostIoCompletionManager *ioCompletionProvider = CorHost2::GetHostIoCompletionManager();
    if (ioCompletionProvider) {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = ioCompletionProvider->SetMinThreads(MinIOCompletionThreads);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr))
        {
            SetLastHRError(hr);
            return FALSE;
        }
    }
    if (threadpoolProvider && ioCompletionProvider) {
        return TRUE;
    }

    if (!IsInitialized())
    {
        if (InterlockedCompareExchange(&Initialization, 1, 0) == 0)
        {
            Initialize();
            Initialization = -1;
        }
    }

    if (IsInitialized())
    {
        // doesn't need to be WorkerCS, but using it to avoid race condition between setting min and max, and didn't want to create a new CS.
        CrstHolder csh(&WorkerCriticalSection);

        BOOL init_result = false;

        if (MinWorkerThreads >= 0 && MinIOCompletionThreads >= 0 &&
            MinWorkerThreads <= (DWORD) MaxLimitTotalWorkerThreads &&
            MinIOCompletionThreads <= (DWORD) MaxLimitTotalCPThreads)
        {
            MinLimitTotalWorkerThreads = MinWorkerThreads;
            MinLimitTotalCPThreads     = MinIOCompletionThreads;
            init_result = TRUE;
        }

        return init_result;
    }
    // someone else is initializing. Too late, return false
    return FALSE;

}

BOOL ThreadpoolMgr::GetMinThreads(DWORD* MinWorkerThreads,
                                     DWORD* MinIOCompletionThreads)
{
    LEAF_CONTRACT;

    HRESULT hr = S_OK;

    IHostThreadpoolManager *threadpoolProvider = CorHost2::GetHostThreadpoolManager();
    if (threadpoolProvider) {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = threadpoolProvider->GetMinThreads(MinWorkerThreads);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr))
        {
            SetLastHRError(hr);
            return FALSE;
        }
    }
    IHostIoCompletionManager *ioCompletionProvider = CorHost2::GetHostIoCompletionManager();
    if (ioCompletionProvider) {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = ioCompletionProvider->GetMinThreads(MinIOCompletionThreads);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr))
        {
            SetLastHRError(hr);
            return FALSE;
        }
    }

    if (threadpoolProvider && ioCompletionProvider) {
        return TRUE;
    }

    if (!MinWorkerThreads || !MinIOCompletionThreads)
    {
        SetLastHRError(ERROR_INVALID_DATA);
        return FALSE;
    }

    if (IsInitialized())
    {
        *MinWorkerThreads = MinLimitTotalWorkerThreads;
        *MinIOCompletionThreads = MinLimitTotalCPThreads;
    }
    else
    {
        NumberOfProcessors = GetCurrentProcessCpuCount();
        *MinWorkerThreads = NumberOfProcessors;
        *MinIOCompletionThreads = NumberOfProcessors;
    }
    return TRUE;
}

BOOL ThreadpoolMgr::GetAvailableThreads(DWORD* AvailableWorkerThreads,
                                        DWORD* AvailableIOCompletionThreads)
{
    LEAF_CONTRACT;

    HRESULT hr = S_OK;

    if (IsInitialized())
    {
        IHostThreadpoolManager *threadpoolProvider = CorHost2::GetHostThreadpoolManager();
        if (threadpoolProvider) {
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
            hr = threadpoolProvider->GetAvailableThreads(AvailableWorkerThreads);
            END_SO_TOLERANT_CODE_CALLING_HOST;
            if (FAILED(hr))
            {
                SetLastHRError(hr);
                return FALSE;
            }
        }

        IHostIoCompletionManager *ioCompletionProvider = CorHost2::GetHostIoCompletionManager();
        if (ioCompletionProvider) {
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
            hr = ioCompletionProvider->GetAvailableThreads(AvailableIOCompletionThreads);
            END_SO_TOLERANT_CODE_CALLING_HOST;
            if (FAILED(hr))
            {
                SetLastHRError(hr);
                return FALSE;
            }
        }

        if (threadpoolProvider && ioCompletionProvider) {
            return TRUE;
        }

        if (!AvailableWorkerThreads || !AvailableIOCompletionThreads)
        {
            SetLastHRError(ERROR_INVALID_DATA);
            return FALSE;
        }

        if (MaxLimitTotalWorkerThreads < (DWORD)NumWorkerThreads)
        {
            *AvailableWorkerThreads = NumIdleWorkerThreads;
        }
        else
        {
            *AvailableWorkerThreads = (MaxLimitTotalWorkerThreads - NumWorkerThreads)  /*threads yet to be created */
                                   + NumIdleWorkerThreads;
        }

        if (MaxLimitTotalCPThreads < NumCPThreads)
        {
            *AvailableIOCompletionThreads = NumFreeCPThreads;
        }
        else
        {
            *AvailableIOCompletionThreads = (MaxLimitTotalCPThreads - NumCPThreads) /*threads yet to be created */
                                   + NumFreeCPThreads;
        }
    }
    else
    {
        GetMaxThreads(AvailableWorkerThreads,AvailableIOCompletionThreads);
    }
    return TRUE;
}

void QueueUserWorkItemHelp(LPTHREAD_START_ROUTINE Function, PVOID Context)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;
    /* Cannot use contract here because of SEH
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;*/

    PAL_TRY
    {
        THREADPOOL_LOG2(TPL_PRIV | TPL_STRESS, LL_INFO1000, "QueueUserWorkItemHelp: Calling work request (Function= %x, Context = %x)\n", Function, Context);

        (Function)(Context);

        THREADPOOL_LOG2(TPL_PRIV | TPL_STRESS, LL_INFO1000, "QueueUserWorkItemHelp: Returned from work request (Function= %x, Context = %x)\n", Function, Context);
    }
    PAL_EXCEPT_FILTER(DefaultCatchNoSwallowFilter, COMPLUS_EXCEPTION_EXECUTE_HANDLER)
    {
        THREADPOOL_LOG2(TPL_PRIV | TPL_STRESS, LL_INFO1000, "QueueUserWorkItemHelp: WARNING: Unhandled exception from work request (Function= %x, Context = %x)\n", Function, Context);
    }
    PAL_ENDTRY

    Thread *pThread = GetThread();
    if (pThread) {
        if (pThread->IsAbortRequested())
            pThread->EEResetAbort(Thread::TAR_ALL);
        pThread->InternalReset(FALSE);
    }
}

/************************************************************************/

BOOL ThreadpoolMgr::QueueUserWorkItem(LPTHREAD_START_ROUTINE Function,
                                      PVOID Context,
                                      DWORD Flags)
{
    CONTRACTL
    {
        THROWS;     // EnsureInitialized, EnqueueWorkRequest can throw OOM
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    EnsureInitialized();

    IHostThreadpoolManager *provider = CorHost2::GetHostThreadpoolManager();
    if (provider) {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = provider->QueueUserWorkItem(Function, Context, Flags);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(hr))
        {
            SetLastHRError(hr);
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    BOOL shouldWakeupRetiredThread = FALSE;

    if (Flags == CALL_OR_QUEUE)
    {
        // we've been asked to call this directly if the thread pressure is not too high

        int MinimumAvailableCPThreads = (NumberOfProcessors < 3) ? 3 : NumberOfProcessors;
        // It would be nice to assert that this is a completion port thread, but
        // there is no easy way to do that.
        if ((MaxLimitTotalCPThreads - NumCPThreads) >= MinimumAvailableCPThreads )
        {
            ThreadLocaleHolder localeHolder;
            QueueUserWorkItemHelp(Function, Context);
            return TRUE;
        }

    }

    WorkRequest* workRequest = MakeWorkRequest(Function,Context);
    LONG lRequestsQueued = 0;

    if (workRequest)
    {
        {
            CrstHolder csh(&WorkerCriticalSection);
            lRequestsQueued = EnqueueWorkRequest(workRequest);
        }

        // see if we need to grow the worker thread pool, but don't bother if GC is in progress
        if (ShouldGrowWorkerThreadPool() &&
            !(GCHeap::IsGCInProgress(TRUE)
#ifdef _DEBUG
#ifdef STRESS_HEAP
              && g_pConfig->GetGCStressLevel() == 0
#endif
#endif
              ))
        {
            CrstHolder csh(&WorkerCriticalSection);
            if (ShouldGrowWorkerThreadPool())
            {
                if (NumRetiredWorkerThreads == 0)
                {
                    THREADPOOL_LOG0(TPL_PRIV, LL_INFO100, "QUWI:CreateWorker\n");
                    // creating worker thread inside crst since we do not expect this to
                    // be a frequent event except during warmup. If this turns out to be a
                    // problem, we can move it out.
                    CreateWorkerThread();
                }
                else
                    shouldWakeupRetiredThread = TRUE;
            }
        }
        else
        // else we did not grow the worker pool, so make sure there is a gate thread
        // that monitors the WorkRequest queue and spawns new threads if no progress is
        // being made
        {
            EnsureGateThreadCreated(GATE_THREAD_STATUS_NOWORKERTHREAD);
            MonitorWorkRequestsQueue = 1;
        }

        // Between the above enqueue and here, it's possible an active worker thread dequeued the
        // workitem and reset the event, in which case setting the event here causes extra work with
        // no benefit.  Ensure there's actually work in the queue before setting.
        if (lRequestsQueued == 1 && NumQueuedWorkRequests != 0)
            WorkRequestNotification->Set();
   }

    if (shouldWakeupRetiredThread)
        RetiredWorkerWakeupEvent->Set();


    return (workRequest != NULL);
}


BOOL ThreadpoolMgr::PostQueuedCompletionStatus(LPOVERLAPPED lpOverlapped,
                                      LPOVERLAPPED_COMPLETION_ROUTINE Function)
{
    CONTRACTL
    {
        THROWS;     // EnsureInitialized can throw OOM
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;


    return FALSE;
}


void ThreadpoolMgr::WaitIOCompletionCallback(
    DWORD dwErrorCode,
    DWORD numBytesTransferred,
    LPOVERLAPPED lpOverlapped)
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
    }
    CONTRACTL_END;

}



VOID ThreadpoolMgr::CallbackForContinueDrainageOfCompletionPortQueue(
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPOVERLAPPED lpOverlapped
    )
{
}


VOID
ThreadpoolMgr::CallbackForInitiateDrainageOfCompletionPortQueue(
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPOVERLAPPED lpOverlapped
    )
{
}

extern void WINAPI BindIoCompletionCallbackStub(DWORD ErrorCode,
                                            DWORD numBytesTransferred,
                                            LPOVERLAPPED lpOverlapped);

void HostIOCompletionCallback(
    DWORD ErrorCode,
    DWORD numBytesTransferred,
    LPOVERLAPPED lpOverlapped)
{
}

BOOL ThreadpoolMgr::DrainCompletionPortQueue()
{
    return FALSE;
}


DWORD __stdcall ThreadpoolMgr::QUWIPostCompletion(PVOID pArgs)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    PostRequest* postRequest = (PostRequest*) pArgs;

    EX_TRY
    {
        (postRequest->Function)(postRequest->errorCode, postRequest->numBytesTransferred, postRequest->lpOverlapped);
    }
    EX_CATCH
    {
        RecycleMemory( postRequest, MEMTYPE_PostRequest );
        if (!SwallowUnhandledExceptions())
            EX_RETHROW;
    }
    EX_END_CATCH(SwallowAllExceptions);
    return ERROR_SUCCESS;

}


// This is either made by a worker thread or a CP thread
// indicated by threadTypeStatus
void ThreadpoolMgr::EnsureGateThreadCreated(SSIZE_T threadTypeStatus)
{
    LEAF_CONTRACT;

    SSIZE_T oldstatus;

    while (true)
    {
        oldstatus = GateThreadCreated;

        if (oldstatus == GATE_THREAD_STATUS_NOTCREATED)
        {
            CreateGateThread(threadTypeStatus);
            return;
        }
        else if (oldstatus & threadTypeStatus)
        {
            _ASSERTE(oldstatus & GATE_THREAD_STATUS_CREATED);

            if (FastInterlockCompareExchange(&GateThreadCreated, oldstatus & ~threadTypeStatus, oldstatus)
                == oldstatus)
                break;
        }

        else
        {
            _ASSERTE(oldstatus & GATE_THREAD_STATUS_CREATED);
            break;

        }
    }

    return;

}
//************************************************************************
LONG ThreadpoolMgr::EnqueueWorkRequest(WorkRequest* workRequest)
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    THREADPOOL_LOG2(TPL_PRIV | TPL_LOG, LL_INFO1000, "Enqueue work request (Function= %x, Context = %x)\n", workRequest->Function, workRequest->Context);
    AppendWorkRequest(workRequest);
    if (NumQueuedWorkRequests == 1)
        return TRUE;
    else
        return FALSE;
}

WorkRequest* ThreadpoolMgr::DequeueWorkRequest()
{
    WorkRequest* entry = NULL;
    CONTRACT(WorkRequest*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;

        POSTCONDITION(CheckPointer(entry, NULL_OK));
    } CONTRACT_END;

    entry = RemoveWorkRequest();
    if (NumQueuedWorkRequests == 0)
        WorkRequestNotification->Reset();
    if (entry)
    {
        LastDequeueTime = GetTickCount();
        THREADPOOL_LOG2(TPL_PRIV | TPL_LOG, LL_INFO1000, "Dequeue work request (Function= %x, Context = %x)\n", entry->Function, entry->Context);
    }
    RETURN entry;
}


void ThreadpoolMgr::ExecuteWorkRequest(WorkRequest* workRequest)
{
    CONTRACTL
    {
        THROWS;     // QueueUserWorkItem can throw
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    LPTHREAD_START_ROUTINE wrFunction = workRequest->Function;
    LPVOID                 wrContext  = workRequest->Context;

    // First delete the workRequest then call the function to
    // prevent leaks in apps that call functions that never return

    THREADPOOL_LOG2(TPL_PRIV | TPL_STRESS, LL_INFO1000, "ExecuteWorkRequest: Starting work request (Function= %x, Context = %x)\n", wrFunction, wrContext);

    RecycleMemory( workRequest, MEMTYPE_WorkRequest ); //delete workRequest;
    (wrFunction)(wrContext);

    CompletedWorkRequests++;

    THREADPOOL_LOG2(TPL_PRIV | TPL_STRESS, LL_INFO1000, "ExecuteWorkRequest: Finished work request (Function= %x, Context = %x)\n", wrFunction, wrContext);
}


// Remove a block from the appropriate recycleList and return.
// If recycleList is empty, fall back to new.
LPVOID ThreadpoolMgr::GetRecycledMemory(enum MemType memType)
{
    LPVOID result = NULL;
    CONTRACT(LPVOID)
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        POSTCONDITION(CheckPointer(result));
    } CONTRACT_END;

    if(RecycledLists.IsInitialized())
    {
        RecycledListInfo& list = RecycledLists.GetRecycleMemoryInfo( memType );

        result = list.Remove();
    }

    if(result == NULL)
    {
        switch (memType)
        {
            case MEMTYPE_DelegateInfo:
                result =  new DelegateInfo;
                break;
            case MEMTYPE_AsyncCallback:
                result =  new AsyncCallback;
                break;
            case MEMTYPE_WorkRequest:
                result =  new WorkRequest;
                break;
            case MEMTYPE_PostRequest:
                result =  new WorkRequest;
                break;
            default:
                _ASSERTE(!"Unknown Memtype");
                result = NULL;
                break;
        }
    }

    RETURN result;
}

// Insert freed block in recycle list. If list is full, return to system heap
void ThreadpoolMgr::RecycleMemory(LPVOID mem, enum MemType memType)
{
    LEAF_CONTRACT;

    if(RecycledLists.IsInitialized())
    {
        RecycledListInfo& list = RecycledLists.GetRecycleMemoryInfo( memType );

        if(list.CanInsert())
        {
            list.Insert( mem );
            return;
        }
    }

    switch (memType)
    {
        case MEMTYPE_DelegateInfo:
            delete (DelegateInfo*) mem;
            break;
        case MEMTYPE_AsyncCallback:
            delete (AsyncCallback*) mem;
            break;
        case MEMTYPE_WorkRequest:
            delete (WorkRequest*) mem;
            break;
        case MEMTYPE_PostRequest:
            delete (PostRequest*) mem;
            break;
        default:
            _ASSERTE(!"Unknown Memtype");

    }
}

//************************************************************************


BOOL ThreadpoolMgr::ShouldGrowWorkerThreadPool()
{
    LEAF_CONTRACT;

    // we only want to grow the worker thread pool if there are less than n threads, where n= no. of processors
    // and more requests than the number of idle threads and number of worker threads not at max ceiling
    return (NumWorkerThreads < MinLimitTotalWorkerThreads &&
            NumIdleWorkerThreads < NumQueuedWorkRequests &&
            (DWORD)NumWorkerThreads < MaxLimitTotalWorkerThreads);

}

#define THROTTLE_RATE  0.10 /* rate by which we increase the delay as number of threads increase */


// On Win9x, there are no apis to get cpu utilization, so we fall back on
// other heuristics
void ThreadpoolMgr::GrowWorkerThreadPoolIfStarvationSimple()
{

    CONTRACTL
    {
        NOTHROW;
        if (GetThread()) { GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        if (GetThread()) { MODE_PREEMPTIVE;} else { DISABLED(MODE_ANY);}
    }
    CONTRACTL_END;

    if (NumQueuedWorkRequests == 0 ||
        (DWORD)NumWorkerThreads >= MaxLimitTotalWorkerThreads)
        return;

    int  lastQueueLength = LastRecordedQueueLength;
    LastRecordedQueueLength = NumQueuedWorkRequests;

    // else, we have queued work items, and we haven't
    // hit the upper limit on the number of worker threads

    // if the queue length has decreased since our last time
    // or not enough delay since we last created a worker thread
    if ((NumQueuedWorkRequests < lastQueueLength) ||
    !SufficientDelaySinceLastSample(LastWorkerThreadCreation,NumWorkerThreads, THROTTLE_RATE))
        return;

    BOOL shouldWakeupWorkerThread = FALSE;
    {
        CrstHolder csh(&WorkerCriticalSection);
        if ((NumQueuedWorkRequests >= lastQueueLength) && ((DWORD)NumWorkerThreads < MaxLimitTotalWorkerThreads) &&
            (NumIdleWorkerThreads == NumRetiredWorkerThreads))
        {
            if (NumRetiredWorkerThreads > 0)
            {
                shouldWakeupWorkerThread = TRUE;
            }
            else
            {
                CreateWorkerThread();
            }
        }
    }

    if (shouldWakeupWorkerThread)
        RetiredWorkerWakeupEvent->Set();

}

// This is to avoid the 64KB/1MB aliasing problem present on Pentium 4 processors,
// which can significantly impact performance with HyperThreading enabled
DWORD __stdcall ThreadpoolMgr::intermediateThreadProc(PVOID arg)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_INTOLERANT;

    offset_counter++;
    if (offset_counter * offset_multiplier > PAGE_SIZE)
        offset_counter = 0;

    _alloca(offset_counter * offset_multiplier);

    intermediateThreadParam* param = (intermediateThreadParam*)arg;

    LPTHREAD_START_ROUTINE ThreadFcnPtr = param->lpThreadFunction;
    PVOID args = param->lpArg;
    delete param;

    return ThreadFcnPtr(args);
}

Thread* ThreadpoolMgr::CreateUnimpersonatedThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpArgs, BOOL *pIsCLRThread)
{
    STATIC_CONTRACT_NOTHROW;
    if (GetThread()) { STATIC_CONTRACT_GC_TRIGGERS;} else {DISABLED(STATIC_CONTRACT_GC_NOTRIGGER);}
    STATIC_CONTRACT_MODE_ANY;
    /* cannot use contract because of SEH
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;*/

    Thread* pThread = NULL;

    if (g_fEEStarted) {
        *pIsCLRThread = TRUE;
    }
    else
        *pIsCLRThread = FALSE;
    if (*pIsCLRThread) {
        EX_TRY
        {
            pThread = SetupUnstartedThread();
        }
        EX_CATCH
        {
            pThread = NULL;
        }
        EX_END_CATCH(SwallowAllExceptions);
        if (pThread == NULL) {
            return NULL;
        }
    }
    DWORD threadId;
    BOOL bOK = FALSE;
    HANDLE threadHandle = NULL;

    if (*pIsCLRThread) {
        // CreateNewThread takes care of reverting any impersonation - so dont do anything here.
        bOK = pThread->CreateNewThread(NULL,               // security descriptor
                                       lpStartAddress,
                                       lpArgs                //arguments
                                       );
    }
    else {
        intermediateThreadParam* lpThreadArgs = new (nothrow) intermediateThreadParam;
        if (lpThreadArgs != NULL)
        {
            lpThreadArgs->lpThreadFunction = lpStartAddress;
            lpThreadArgs->lpArg = lpArgs;
            threadHandle = CreateThread(NULL,               // security descriptor
                                        0,                  // default stack size
                                        intermediateThreadProc,
                                        lpThreadArgs,       // arguments
                                        CREATE_SUSPENDED,
                                        &threadId);
        }
    }

    if (*pIsCLRThread && !bOK)
    {
        pThread->DecExternalCount(FALSE);
        pThread = NULL;
    }

    if (*pIsCLRThread) {
        return pThread;
    }
    else
        return (Thread*)threadHandle;
}

// returns if the pending io completed (TRUE) or new work has arrived (FALSE)
BOOL ThreadpoolMgr::EnterRetirement()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    THREADPOOL_LOG1(TPL_PRIV, LL_INFO100, "EnterRetirement, TID:%x\n", GetCurrentThreadId());
    InterlockedIncrement((LPLONG)&NumRetiredWorkerThreads);

    if ( ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) ) {
        ETW_THREADPOOL_INFO Info;
        Info.WorkerThread.Count = NumWorkerThreads;
        Info.WorkerThread.NumRetired = NumRetiredWorkerThreads;
        g_pEtwTracer->EtwTraceEvent(&WorkerThreadRetireGuid,
                                    ETW_TYPE_THREADPOOL_RETIRE_WORKER,
                                    &Info, sizeof(Info.WorkerThread));
    }

    DWORD shouldTerminate = TRUE;
    DWORD waitInterval = CP_THREAD_PENDINGIO_WAIT;
    const DWORD  MaxPendingIoWait = (CP_THREAD_PENDINGIO_WAIT * 60);
    DWORD status;

    while (true)
    {
        status = SafeWait(RetiredWorkerWakeupEvent,waitInterval,FALSE);
        _ASSERTE(status == WAIT_TIMEOUT || status == WAIT_OBJECT_0);
        if ((status == WAIT_TIMEOUT) && (waitInterval < MaxPendingIoWait))
        {
            if (!IsIoPending())
            {
                _ASSERTE(shouldTerminate);
                shouldTerminate = TRUE;
                break;
            }
            else
            {
                waitInterval = min(2*waitInterval, MaxPendingIoWait);
                continue;
            }
        }
        else
        {
            shouldTerminate = FALSE;
            break;
        }

    }

    InterlockedDecrement((LPLONG)&NumRetiredWorkerThreads);

    if ( ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) ) {
        ETW_THREADPOOL_INFO Info;
        Info.WorkerThread.Count = NumWorkerThreads;
        Info.WorkerThread.NumRetired = NumRetiredWorkerThreads;
        g_pEtwTracer->EtwTraceEvent(&WorkerThreadRetireGuid,
                                    ETW_TYPE_THREADPOOL_UNRETIRE_WORKER,
                                    &Info, sizeof(Info.WorkerThread));
    }

    THREADPOOL_LOG1(TPL_PRIV, LL_INFO100, "LeavingRetirement, TID:%x\n", GetCurrentThreadId());

    return shouldTerminate;

}

BOOL ThreadpoolMgr::CreateWorkerThread()
{

    CONTRACTL
    {
        if (GetThread()) { GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        NOTHROW;
        MODE_ANY;   // We may try to add a worker thread while queuing a work item thru an fcall
    }
    CONTRACTL_END;

    Thread *pThread;
    BOOL fIsCLRThread;
    if ((pThread = CreateUnimpersonatedThread(WorkerThreadStart, NULL, &fIsCLRThread)) != NULL)
    {
        LastWorkerThreadCreation = GetTickCount();  // record this for use by logic to spawn additional threads

        NumWorkerThreads++;
        IncNumIdleWorkerThreads();
        if (fIsCLRThread) {
            pThread->StartThread();
        }
        else {
            DWORD status;
            status = ResumeThread((HANDLE)pThread);
            _ASSERTE(status != (DWORD) (-1));
            CloseHandle((HANDLE)pThread);          // we don't need this anymore
        }
        if ( ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) ) {
            ETW_THREADPOOL_INFO Info;
            Info.WorkerThread.Count = NumWorkerThreads;
            Info.WorkerThread.NumRetired = NumRetiredWorkerThreads;
            g_pEtwTracer->EtwTraceEvent(&WorkerThreadCreateGuid,
                                        ETW_TYPE_THREADPOOL_CREATE_WORKER,
                                        &Info, sizeof(Info.WorkerThread));
        }
        THREADPOOL_LOG1(TPL_PRIV | TPL_LOG, LL_INFO100, "Worker thread created (NumWorkerThreads=%d)\n", NumWorkerThreads);
    }
    // dont return failure if we have at least one running thread, since we can service the request
    return (NumWorkerThreads > 0);
}

#ifdef _MSC_VER
#pragma warning(disable:4702)       // Unreachable code
#endif
DWORD __stdcall ThreadpoolMgr::WorkerThreadStart(LPVOID lpArgs)
{
    ClrFlsSetThreadType (ThreadType_Threadpool_Worker);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    #define IDLE_WORKER_TIMEOUT (40*1000) // milliseonds
    #define NOWORK_TIMEOUT (10*1000) // milliseonds

    _ASSERTE (!CLRThreadpoolHosted());

    DWORD SleepTime = IDLE_WORKER_TIMEOUT;
    unsigned int LastThreadDequeueTime = GetTickCount();

    LastSuspendTime = GetTickCount();

    BOOL fThreadInit = FALSE;
    Thread *pThread = NULL;
    if (g_fEEStarted)
    {
        pThread = SetupThreadPoolThreadNoThrow(WorkerThread);
        if (pThread == NULL) {
            return 0;
        }
        fThreadInit = TRUE;
    }


    THREADPOOL_LOG1(TPL_PRIV | TPL_STRESS, LL_INFO100, "Worker Thread %x started\n", GetCurrentThreadId());

/*
#ifndef FEATURE_PAL
    HANDLE hThread = NULL;
    hThread = OpenThread(THREAD_SET_INFORMATION, FALSE, GetCurrentThreadId());
    if (hThread)
    {
        SetThreadPriorityBoost(hThread, TRUE);
        CloseHandle(hThread);
    }
#endif //FEATURE_PAL
*/

    for (;;)
    {
        if (!fThreadInit) {
            if (g_fEEStarted) {
                pThread = SetupThreadPoolThreadNoThrow(WorkerThread);
                if (pThread == NULL) {
                    __SwitchToThread(0);
                    continue;
                }


                fThreadInit = TRUE;
            }
        }

        if (pThread && pThread->PreemptiveGCDisabled()) {
            pThread->EnablePreemptiveGC();
        }
        _ASSERTE(pThread == NULL || !pThread->PreemptiveGCDisabled());
        _ASSERTE(NumWorkerThreads > 0);

        DWORD status = WAIT_OBJECT_0;

        {

            status = SafeWait(WorkRequestNotification,SleepTime, FALSE);
            _ASSERTE(status == WAIT_TIMEOUT || status == WAIT_OBJECT_0);
        }


        BOOL shouldTerminate = FALSE;

        if ( status == WAIT_TIMEOUT)
        {
        // The thread terminates if there are > 1 threads and the queue is small
        // OR if there is only 1 thread and there is no request pending
            if (NumWorkerThreads > 1)
            {
                ULONG Threshold = NEW_THREAD_THRESHOLD * (NumWorkerThreads-1);

                if ((NumQueuedWorkRequests < (int) Threshold) &&
                    (SleepTime >= 2*IDLE_WORKER_TIMEOUT))
                {
                    THREADPOOL_LOG3(TPL_PRIV, LL_INFO100, "WorkerThreadStart: retire worker: Requests: %d Threshold: %d\n", NumQueuedWorkRequests, Threshold, Sleep);
                    shouldTerminate = true;
                }
                else
                {
                    SleepTime <<= 1 ;
                    SleepTime += 1000; // to prevent wraparound to 0
                }
            }
            else // this is the only worker thread
            {
                if (NumQueuedWorkRequests == 0)
                {
                    // delay termination of last thread
                    if (SleepTime < 4*IDLE_WORKER_TIMEOUT)
                    {
                        SleepTime <<= 1 ;
                        SleepTime += 1000; // to prevent wraparound to 0
                    }
                    else
                    {
                        THREADPOOL_LOG0(TPL_PRIV, LL_INFO100, "WorkerThreadStart: retire last worker\n");
                        shouldTerminate = true;
                    }
                }
            }

        }
        else
        {
            // woke up because of a new work request arrival
            WorkRequest* workRequest = NULL;

            if (NumQueuedWorkRequests != 0)
            {
                {
                    CrstHolder csh(&WorkerCriticalSection);

                    if ( ( workRequest = DequeueWorkRequest() ) != NULL)
                    {
                        _ASSERTE(NumIdleWorkerThreads > 0);
                        DecNumIdleWorkerThreads(); // we found work, decrease the number of idle threads
                    }

                    // the dequeue operation also resets the WorkRequestNotification event
                }
            }

            if (!workRequest)
            {
                // we woke up because of a work arrival, but there was no work
                // this implies too many idle threads and not enough work
                if ((GetTickCount() - LastThreadDequeueTime >= (NOWORK_TIMEOUT)) &&
                    ((unsigned) NumWorkerThreads > NumberOfProcessors))
                {
                    THREADPOOL_LOG0(TPL_PRIV, LL_INFO100, "WorkerThreadStart: retire idle worker\n");
                    shouldTerminate = TRUE;
                }
            }

            // both workRequest and shouldTerminate cannot be true at the same time
            _ASSERTE((workRequest && shouldTerminate) == FALSE);

            while (workRequest)
            {
                if (GCHeap::IsGCInProgress(TRUE))
                {
                    // GC is imminent, so wait until GC is complete before executing next request.
                    // this reduces in-flight objects allocated right before GC, easing the GC's work
                    GCHeap::WaitForGCCompletion(TRUE);
                }

                {
                    ThreadLocaleHolder localeHolder;
                    ExecuteWorkRequest(workRequest);
                }

                // if we have too many threads, suspend processing for a while
                if (RunningOnWinNT() &&     // win9x can't obtain cpu utilization
                    ThreadInSuspend == 0)
                {
                    // last thread to be injected didn't increase throughput, so terminate
                    if (LastWorkerCreationState == WORKER_RateDecrease)
                    {
                        if (InterlockedCompareExchange(&ThreadInSuspend, 1, 0) == 0)
                        {
                            THREADPOOL_LOG0(TPL_PRIV, LL_INFO100, "WorkerThreadStart: state WORKER_RateDecrease, setting WORKER_PauseInjection and terminating thread\n");
                            LastWorkerCreationState = WORKER_PauseInjection;
                            LastWorkerStateTime = GetTickCount();
                            shouldTerminate = TRUE;
                            ThreadInSuspend = 0;
                        }
                    }
                    else
                    if (((NumWorkerThreads - NumIdleWorkerThreads) > 2 * MinLimitTotalWorkerThreads) &&
                        (cpuUtilizationAverage >= CpuUtilizationHigh) &&
                        (GetTickCount() > (LastSuspendTime + DELAY_BETWEEN_SUSPENDS)))
                    {
                        // one thread at a time to avoid many threads sleeping simultaneously and dropping throughput
                        if (InterlockedCompareExchange(&ThreadInSuspend, 1, 0) == 0)
                        {
                            _ASSERTE(ThreadInSuspend == 1);
                            shouldTerminate = SuspendProcessing();
                            LastSuspendTime = GetTickCount();
                            ThreadInSuspend = 0;
                        }
                        // else someone else is already suspending so try again later
                    }
                }

                if (shouldTerminate)
                {
                    CrstHolder csh(&WorkerCriticalSection);
                    IncNumIdleWorkerThreads();
                    workRequest = NULL;
                }
                else
                {
                    LastThreadDequeueTime = GetTickCount();
                    {
                        CrstHolder csh(&WorkerCriticalSection);

                        workRequest = DequeueWorkRequest();
                        // the dequeue operation resets the WorkRequestNotification event

                        if (workRequest == NULL)
                        {
                            IncNumIdleWorkerThreads(); // no more work, increase the number of idle threads
                        }
                    }
                }

                // Reset TLS etc. for next WorkRequest.
                if (pThread == NULL) {
                    pThread = GetThread();
                }
                if (pThread) {
                    if (pThread->IsAbortRequested())
                        pThread->EEResetAbort(Thread::TAR_ALL);
                    pThread->InternalReset(FALSE);
                }

            }

        }

        if (shouldTerminate)
        {
            if (IsIoPending())
                shouldTerminate = EnterRetirement();

            if (shouldTerminate)
            {
                _ASSERTE(!IsIoPending());
                // recheck NumQueuedWorkRequest since a new one may have arrived while we are checking it
                {
                    CrstHolder csh(&WorkerCriticalSection);
                    if (NumWorkerThreads == 1)
                    {

                        if (NumQueuedWorkRequests == 0)
                        {
                            // this is the last worker thread, so inform the GateThread it is ok to go away
                            while (true)
                            {
                                SSIZE_T oldvalue = GateThreadCreated;
                                if ((oldvalue == GATE_THREAD_STATUS_NOTCREATED) ||
                                    (FastInterlockCompareExchange(&GateThreadCreated,
                                                                 GATE_THREAD_STATUS_NOWORKERTHREAD | oldvalue,
                                                                 oldvalue) == oldvalue))

                                break;  // out of for loop
                            }
                        }
                        else
                        {
                            // last thread, but non-empty queue, so we are not going to terminate
                            continue;  // outer for loop
                        }
                    }



                    // it really is zero, so terminate this thread
                    NumWorkerThreads--;     // protected by WorkerCriticalSection
                    DecNumIdleWorkerThreads(); //   ditto
                    _ASSERTE(NumWorkerThreads >= 0 && NumIdleWorkerThreads >= 0);

                    THREADPOOL_LOG1(TPL_PRIV | TPL_LOG, LL_INFO100, "Worker thread terminated (NumWorkerThreads=%d)\n", NumWorkerThreads);

                    break;   // out of for loop
                } //  scope of WorkerCriticalSection
            }
        }

    } // for(;;)

    if ( ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) ) {
        ETW_THREADPOOL_INFO Info;
        Info.WorkerThread.Count = NumWorkerThreads;
        Info.WorkerThread.NumRetired = NumRetiredWorkerThreads;
        g_pEtwTracer->EtwTraceEvent(&WorkerThreadCreateGuid,
                                    ETW_TYPE_THREADPOOL_TERM_WORKER,
                                    &Info, sizeof(Info.WorkerThread));
    }

    if (pThread) {
        DestroyThread(pThread);
    }

    THREADPOOL_LOG2(TPL_PRIV | TPL_STRESS, LL_INFO100, "Worker Thread %x terminated.  Thread stack at roughly %p\n", GetCurrentThreadId(), &lpArgs);

    return 0;
}
#ifdef _MSC_VER
#pragma warning(default:4702)
#endif

BOOL ThreadpoolMgr::SuspendProcessing()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;

    THREADPOOL_LOG2(TPL_PRIV, LL_INFO100, "SuspendProcessing, TID:%x, util:%d\n", GetCurrentThreadId(), cpuUtilization);

    if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
        ETW_IS_FLAG_ON(TRACE_FLAG_THREADPOOL_QUEUE)) {
        ETW_THREADPOOL_INFO Info;
        Info.SuspendResume.ThreadID = GetCurrentThreadId();
        Info.SuspendResume.CPUUtil = cpuUtilization;
        g_pEtwTracer->EtwTraceEvent(&ThreadpoolSuspendGuid,
                                    ETW_TYPE_THREADPOOL_SUSPENDTHREAD,
                                    &Info, sizeof(Info.SuspendResume));
    }

    BOOL shouldRetire = TRUE;
    DWORD sleepInterval = SUSPEND_TIME;
    long oldCpuUtilization = cpuUtilization;
    for (int i = 0; i < shouldRetire; i++)
    {
        __SwitchToThread(sleepInterval);
        if ((cpuUtilization <= (oldCpuUtilization - 4)))
        {   // if cpu util. dips by 4% or more, then put it back in circulation
            shouldRetire = FALSE;
            break;
        }
    }

    THREADPOOL_LOG3(TPL_PRIV, LL_INFO100, "Leave SuspendProcessing, TID:%x, util:%d, %s\n", GetCurrentThreadId(), cpuUtilization, shouldRetire ? " retire" : " work");

    if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
        ETW_IS_FLAG_ON(TRACE_FLAG_THREADPOOL_QUEUE)) {
        ETW_THREADPOOL_INFO Info;
        Info.SuspendResume.ThreadID = GetCurrentThreadId();
        Info.SuspendResume.CPUUtil = cpuUtilization;
        g_pEtwTracer->EtwTraceEvent(&ThreadpoolSuspendGuid,
                                    ETW_TYPE_THREADPOOL_RESUMETHREAD,
                                    &Info, sizeof(Info.SuspendResume));
    }

    return shouldRetire;
}


// this should only be called by unmanaged thread (i.e. there should be no mgd
// caller on the stack) since we are swallowing terminal exceptions
DWORD ThreadpoolMgr::SafeWait(CLREvent * ev, DWORD sleepTime, BOOL alertable)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_PREEMPTIVE;
    /* cannot use contract because of SEH
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;*/

    DWORD status = WAIT_TIMEOUT;
    EX_TRY
    {
        status = ev->Wait(sleepTime,FALSE);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions)
    return status;
}

/************************************************************************/

BOOL ThreadpoolMgr::RegisterWaitForSingleObject(PHANDLE phNewWaitObject,
                                                HANDLE hWaitObject,
                                                WAITORTIMERCALLBACK Callback,
                                                PVOID Context,
                                                ULONG timeout,
                                                DWORD dwFlag )
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        if (GetThread()) { GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;
    EnsureInitialized();

    ThreadCB* threadCB;
    {
        CrstHolder csh(&WaitThreadsCriticalSection);

        threadCB = FindWaitThread();
    }

    *phNewWaitObject = NULL;

    if (threadCB)
    {
        WaitInfo* waitInfo = new (nothrow) WaitInfo;

        if (waitInfo == NULL)
            return FALSE;

        waitInfo->waitHandle = hWaitObject;
        waitInfo->Callback = Callback;
        waitInfo->Context = Context;
        waitInfo->timeout = timeout;
        waitInfo->flag = dwFlag;
        waitInfo->threadCB = threadCB;
        waitInfo->state = 0;
        waitInfo->refCount = 1;     // safe to do this since no wait has yet been queued, so no other thread could be modifying this
        waitInfo->ExternalCompletionEvent = INVALID_HANDLE;
        waitInfo->ExternalEventSafeHandle = NULL;
        waitInfo->handleOwningAD = (ADID) 0;
        waitInfo->bReleaseEventIfADUnloaded = FALSE;

        waitInfo->timer.startTime = GetTickCount();
        waitInfo->timer.remainingTime = timeout;

        *phNewWaitObject = waitInfo;

        THREADPOOL_LOG3(TPL_PRIV | TPL_LOG, LL_INFO1000, "Registering wait for handle %x, Callback=%x, Context=%x \n", hWaitObject, Callback, Context);

        BOOL status = QueueUserAPC((PAPCFUNC)InsertNewWaitForSelf, threadCB->threadHandle, (size_t) waitInfo);

        if (status == FALSE)
        {
            *phNewWaitObject = NULL;
            delete waitInfo;
        }

        return status;
    }

    return FALSE;
}


// Returns a wait thread that can accomodate another wait request. The
// caller is responsible for synchronizing access to the WaitThreadsHead
ThreadpoolMgr::ThreadCB* ThreadpoolMgr::FindWaitThread()
{
    CONTRACTL
    {
        THROWS;     // CreateWaitThread can throw
        MODE_ANY;
        GC_TRIGGERS;
    }
    CONTRACTL_END;
    do
    {
        for (LIST_ENTRY* Node = (LIST_ENTRY*) WaitThreadsHead.Flink ;
             Node != &WaitThreadsHead ;
             Node = (LIST_ENTRY*)Node->Flink)
        {
            _ASSERTE(offsetof(WaitThreadInfo,link) == 0);

            ThreadCB*  threadCB = ((WaitThreadInfo*) Node)->threadCB;

            if (threadCB->NumWaitHandles < MAX_WAITHANDLES)         // this test and following ...

            {
                InterlockedIncrement(&threadCB->NumWaitHandles);    // ... increment are protected by WaitThreadsCriticalSection.
                                                                    // but there might be a concurrent decrement in DeactivateWait
                                                                    // or InsertNewWaitForSelf, hence the interlock
                return threadCB;
            }
        }

        // if reached here, there are no wait threads available, so need to create a new one
        if (!CreateWaitThread())
            return NULL;


        // Now loop back
    } while (TRUE);

}

BOOL ThreadpoolMgr::CreateWaitThread()
{
    CONTRACTL
    {
        THROWS; // CLREvent::CreateAutoEvent can throw OOM
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;
    DWORD threadId;

    if (g_fEEShutDown & ShutDown_Finalize2){
        // The process is shutting down.  Shutdown thread has ThreadStore lock,
        // wait thread is blocked on the lock.
        return FALSE;
    }

    NewHolder<WaitThreadInfo> waitThreadInfo(new (nothrow) WaitThreadInfo);
    if (waitThreadInfo == NULL)
        return FALSE;

    NewHolder<ThreadCB> threadCB(new (nothrow) ThreadCB);

    if (threadCB == NULL)
    {
        return FALSE;
    }

    threadCB->startEvent.CreateAutoEvent(FALSE);
    HANDLE threadHandle = CreateThread(NULL,                // security descriptor
                                       0,                   // default stack size
                                       WaitThreadStart,     //
                                       (LPVOID) threadCB,   // thread control block is passed as argument
                                       CREATE_SUSPENDED,    // start immediately
                                       &threadId);

    if (threadHandle == NULL)
    {
        threadCB->startEvent.CloseEvent();
        return FALSE;
    }

    waitThreadInfo.SuppressRelease();
    threadCB.SuppressRelease();
    threadCB->threadHandle = threadHandle;
    threadCB->threadId = threadId;              // may be useful for debugging otherwise not used
    threadCB->NumWaitHandles = 0;
    threadCB->NumActiveWaits = 0;
    for (int i=0; i< MAX_WAITHANDLES; i++)
    {
        InitializeListHead(&(threadCB->waitPointer[i]));
    }

    waitThreadInfo->threadCB = threadCB;

    DWORD status = ResumeThread(threadHandle);

    {
        // We will QueueUserAPC on the newly created thread.
        // Let us wait until the thread starts running.
        GCX_PREEMP();
        DWORD timeout=500;
        while (TRUE) {
            if (g_fEEShutDown & ShutDown_Finalize2){
                // The process is shutting down.  Shutdown thread has ThreadStore lock,
                // wait thread is blocked on the lock.
                return FALSE;
            }
            DWORD wait_status = threadCB->startEvent.Wait(timeout, FALSE);
            if (wait_status == WAIT_OBJECT_0) {
                break;
            }
        }
    }
    threadCB->startEvent.CloseEvent();

    // check to see if setup succeeded
    if (threadCB->threadHandle == NULL)
        return FALSE;

    InsertHeadList(&WaitThreadsHead,&waitThreadInfo->link);

    _ASSERTE(status != (DWORD) (-1));

    THREADPOOL_LOG0(TPL_PRIV | TPL_LOG, LL_INFO100, "Created wait thread \n");

    return (status != (DWORD) (-1));

}

// Executed as an APC on a WaitThread. Add the wait specified in pArg to the list of objects it is waiting on
void ThreadpoolMgr::InsertNewWaitForSelf(WaitInfo* pArgs)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_INTOLERANT;

    WaitInfo* waitInfo = pArgs;

    // the following is safe since only this thread is allowed to change the state
    if (!(waitInfo->state & WAIT_DELETE))
    {
        waitInfo->state =  (WAIT_REGISTERED | WAIT_ACTIVE);
    }
    else
    {
        // some thread unregistered the wait
        DeleteWait(waitInfo);
        return;
    }


    ThreadCB* threadCB = waitInfo->threadCB;

    _ASSERTE(threadCB->NumActiveWaits <= threadCB->NumWaitHandles);

    int index = FindWaitIndex(threadCB, waitInfo->waitHandle);
    _ASSERTE(index >= 0 && index <= threadCB->NumActiveWaits);

    if (index == threadCB->NumActiveWaits)
    {
        threadCB->waitHandle[threadCB->NumActiveWaits] = waitInfo->waitHandle;
        threadCB->NumActiveWaits++;
    }
    else
    {
        // this is a duplicate waithandle, so the increment in FindWaitThread
        // wasn't strictly necessary.  This will avoid unnecessary thread creation.
        InterlockedDecrement(&threadCB->NumWaitHandles);
    }

    _ASSERTE(offsetof(WaitInfo, link) == 0);
    InsertTailList(&(threadCB->waitPointer[index]), (&waitInfo->link));

    return;
}

// returns the index of the entry that matches waitHandle or next free entry if not found
int ThreadpoolMgr::FindWaitIndex(const ThreadCB* threadCB, const HANDLE waitHandle)
{
    LEAF_CONTRACT;

    for (int i=0;i<threadCB->NumActiveWaits; i++)
        if (threadCB->waitHandle[i] == waitHandle)
            return i;

    // else not found
    return threadCB->NumActiveWaits;
}


// if no wraparound that the timer is expired if duetime is less than current time
// if wraparound occurred, then the timer expired if dueTime was greater than last time or dueTime is less equal to current time
#define TimeExpired(last,now,duetime) (last <= now ? \
                                       (duetime <= now && duetime >= last): \
                                       (duetime >= last || duetime <= now))

#define TimeInterval(end,start) ( end > start ? (end - start) : ((0xffffffff - start) + end + 1)   )

// Returns the minimum of the remaining time to reach a timeout among all the waits
DWORD ThreadpoolMgr::MinimumRemainingWait(LIST_ENTRY* waitInfo, unsigned int numWaits)
{
    LEAF_CONTRACT;

    unsigned int min = (unsigned int) -1;
    DWORD currentTime = GetTickCount();

    for (unsigned i=0; i < numWaits ; i++)
    {
        WaitInfo* waitInfoPtr = (WaitInfo*) (waitInfo[i].Flink);
        PVOID waitInfoHead = &(waitInfo[i]);
        do
        {
            if (waitInfoPtr->timeout != INFINITE)
            {
                // compute remaining time
                DWORD elapsedTime = TimeInterval(currentTime,waitInfoPtr->timer.startTime );

                __int64 remainingTime = (__int64) (waitInfoPtr->timeout) - (__int64) elapsedTime;

                // update remaining time
                waitInfoPtr->timer.remainingTime =  remainingTime > 0 ? (int) remainingTime : 0;

                // ... and min
                if (waitInfoPtr->timer.remainingTime < min)
                    min = waitInfoPtr->timer.remainingTime;
            }

            waitInfoPtr = (WaitInfo*) (waitInfoPtr->link.Flink);

        } while ((PVOID) waitInfoPtr != waitInfoHead);

    }
    return min;
}

#ifdef _MSC_VER
#pragma warning (disable : 4702)    // Unreachable code due to END_SO_INTOLERANT_CODE
#pragma warning (disable : 4715)
#endif

DWORD __stdcall ThreadpoolMgr::WaitThreadStart(LPVOID lpArgs)
{
    ClrFlsSetThreadType (ThreadType_Wait);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    ThreadCB* threadCB = (ThreadCB*) lpArgs;

    Thread* pThread = SetupThreadPoolThreadNoThrow(WaitThread);


    if (pThread == NULL)
    {
        _ASSERTE(threadCB->threadHandle != NULL);
        threadCB->threadHandle = NULL;
    }

    threadCB->startEvent.Set();

    if (pThread == NULL)
        return 0;

    BEGIN_SO_INTOLERANT_CODE(pThread);  // we probe at the top of the thread so we can safely call anything below here.

    // wait threads never die. (Why?)
    for (;;)
    {
        DWORD status;
        DWORD timeout = 0;

        if (threadCB->NumActiveWaits == 0)
        {

#undef SleepEx
            status = SleepEx(INFINITE,TRUE);
#define SleepEx(a,b) Dont_Use_SleepEx(a,b)

            _ASSERTE(status == WAIT_IO_COMPLETION);
        }
        else
        {
            // compute minimum timeout. this call also updates the remainingTime field for each wait
            timeout = MinimumRemainingWait(threadCB->waitPointer,threadCB->NumActiveWaits);

            status = WaitForMultipleObjectsEx(  threadCB->NumActiveWaits,
                                                threadCB->waitHandle,
                                                FALSE,                      // waitall
                                                timeout,
                                                TRUE  );                    // alertable

            _ASSERTE( (status == WAIT_TIMEOUT) ||
                      (status == WAIT_IO_COMPLETION) ||
                      (status >= WAIT_OBJECT_0 && status < (DWORD)(WAIT_OBJECT_0 + threadCB->NumActiveWaits))  ||
                      (status == WAIT_FAILED));
        }

        if (status == WAIT_IO_COMPLETION)
            continue;

        if (status == WAIT_TIMEOUT)
        {
            for (int i=0; i< threadCB->NumActiveWaits; i++)
            {
                WaitInfo* waitInfo = (WaitInfo*) (threadCB->waitPointer[i]).Flink;
                PVOID waitInfoHead = &(threadCB->waitPointer[i]);

                do
                {
                    _ASSERTE(waitInfo->timer.remainingTime >= timeout);

                    WaitInfo* wTemp = (WaitInfo*) waitInfo->link.Flink;

                    if (waitInfo->timer.remainingTime == timeout)
                    {
                        ProcessWaitCompletion(waitInfo,i,TRUE);
                    }

                    waitInfo = wTemp;

                } while ((PVOID) waitInfo != waitInfoHead);
            }
        }
        else if (status >= WAIT_OBJECT_0 && status < (DWORD)(WAIT_OBJECT_0 + threadCB->NumActiveWaits))
        {
            unsigned index = status - WAIT_OBJECT_0;
            WaitInfo* waitInfo = (WaitInfo*) (threadCB->waitPointer[index]).Flink;
            PVOID waitInfoHead = &(threadCB->waitPointer[index]);
            BOOL isAutoReset;

            // Setting to unconditional TRUE is inefficient since we will re-enter the wait and release
            // the next waiter, but short of using undocumented NT apis is the only solution.
            // Querying the state with a WaitForSingleObject is not an option as it will reset an
            // auto reset event if it has been signalled since the previous wait.
            isAutoReset = TRUE;

            do
            {
                WaitInfo* wTemp = (WaitInfo*) waitInfo->link.Flink;
                ProcessWaitCompletion(waitInfo,index,FALSE);

                waitInfo = wTemp;

            } while (((PVOID) waitInfo != waitInfoHead) && !isAutoReset);

            // If an app registers a recurring wait for an event that is always signalled (!),
            // then no apc's will be executed since the thread never enters the alertable state.
            // This can be fixed by doing the following:
            //     SleepEx(0,TRUE);
            // However, it causes an unnecessary context switch. It is not worth penalizing well
            // behaved apps to protect poorly written apps.


        }
        else
        {
            _ASSERTE(status == WAIT_FAILED);
            // wait failed: application error
            // find out which wait handle caused the wait to fail
            for (int i = 0; i < threadCB->NumActiveWaits; i++)
            {
                DWORD subRet = WaitForSingleObject(threadCB->waitHandle[i], 0);

                if (subRet != WAIT_FAILED)
                    continue;

                // remove all waits associated with this wait handle

                WaitInfo* waitInfo = (WaitInfo*) (threadCB->waitPointer[i]).Flink;
                PVOID waitInfoHead = &(threadCB->waitPointer[i]);

                do
                {
                    WaitInfo* temp  = (WaitInfo*) waitInfo->link.Flink;

                    DeactivateNthWait(waitInfo,i);


            // Note, we cannot cleanup here since there is no way to suppress finalization
            // we will just leak, and rely on the finalizer to clean up the memory
                    //if (InterlockedDecrement(&waitInfo->refCount) == 0)
                    //    DeleteWait(waitInfo);


                    waitInfo = temp;

                } while ((PVOID) waitInfo != waitInfoHead);

                break;
            }
        }
    }
    END_SO_INTOLERANT_CODE;
}

#ifdef _MSC_VER
#pragma warning (default : 4715)
#pragma warning (default : 4702)
#endif

void ThreadpoolMgr::ProcessWaitCompletion(WaitInfo* waitInfo,
                                          unsigned index,
                                          BOOL waitTimedOut
                                         )
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_PREEMPTIVE;
    /* cannot use contract because of SEH
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;*/

    AsyncCallback* asyncCallback = NULL;
    EX_TRY{
        if ( waitInfo->flag & WAIT_SINGLE_EXECUTION)
        {
            DeactivateNthWait (waitInfo,index) ;
        }
        else
        {   // reactivate wait by resetting timer
            waitInfo->timer.startTime = GetTickCount();
        }

        asyncCallback = MakeAsyncCallback();
        if (asyncCallback)
        {
            asyncCallback->wait = waitInfo;
            asyncCallback->waitTimedOut = waitTimedOut;

            InterlockedIncrement(&waitInfo->refCount);

            {
                if (FALSE == QueueUserWorkItem(AsyncCallbackCompletion, asyncCallback, QUEUE_ONLY))
                    ReleaseAsyncCallback(asyncCallback);
            }
        }
    }
    EX_CATCH {
        if (asyncCallback)
            ReleaseAsyncCallback(asyncCallback);

        if (SwallowUnhandledExceptions())
        {
            // Do nothing to swallow the exception
        }
        else
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions);

}


DWORD __stdcall ThreadpoolMgr::AsyncCallbackCompletion(PVOID pArgs)
{

    HRESULT hr;
    Thread* pThread = GetThread();
    if (pThread == NULL)
    {
        pThread = SetupThreadPoolThreadNoThrow(WorkerThread, &hr);
        if (pThread == NULL)
            return hr;
    }

    CONTRACTL
    {
        THROWS;
        MODE_PREEMPTIVE;
        GC_TRIGGERS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BEGIN_SO_INTOLERANT_CODE_NOTHROW(pThread, return ERROR_STACK_OVERFLOW);

    AsyncCallback* asyncCallback = (AsyncCallback*) pArgs;

    WaitInfo* waitInfo = asyncCallback->wait;

    AsyncCallbackHolder asyncCBHolder;
    asyncCBHolder.Assign(asyncCallback);

    THREADPOOL_LOG3(TPL_PRIV | TPL_LOG, LL_INFO1000, "Doing async callback, Function= %x, Context= %x, Timeout= %2d\n", waitInfo->Callback, waitInfo->Context, asyncCallback->waitTimedOut);

    {
        // the user callback can throw, the host must be prepared to handle it.
        // SQL is ok, since they have a top-level SEH handler. However, there's
        // no easy way to verify it

        ((WAITORTIMERCALLBACKFUNC) waitInfo->Callback)
                                    ( waitInfo->Context, asyncCallback->waitTimedOut);
    }

    THREADPOOL_LOG3(TPL_PRIV | TPL_LOG, LL_INFO1000, "Done async callback, Function= %x, Context= %x, Timeout= %2d\n", waitInfo->Callback, waitInfo->Context, asyncCallback->waitTimedOut);

    END_SO_INTOLERANT_CODE;

    return ERROR_SUCCESS;
}

void ThreadpoolMgr::DeactivateWait(WaitInfo* waitInfo)
{
    LEAF_CONTRACT;

    ThreadCB* threadCB = waitInfo->threadCB;
    DWORD endIndex = threadCB->NumActiveWaits-1;
    DWORD index;

    for (index = 0;  index <= endIndex; index++)
    {
        LIST_ENTRY* head = &(threadCB->waitPointer[index]);
        LIST_ENTRY* current = head;
        do {
            if (current->Flink == (PVOID) waitInfo)
                goto FOUND;

            current = (LIST_ENTRY*) current->Flink;

        } while (current != head);
    }

FOUND:
    _ASSERTE(index <= endIndex);

    DeactivateNthWait(waitInfo, index);
}


void ThreadpoolMgr::DeactivateNthWait(WaitInfo* waitInfo, DWORD index)
{
    LEAF_CONTRACT;

    ThreadCB* threadCB = waitInfo->threadCB;

    if (waitInfo->link.Flink != waitInfo->link.Blink)
    {
        RemoveEntryList(&(waitInfo->link));
    }
    else
    {

        ULONG EndIndex = threadCB->NumActiveWaits -1;

        // Move the remaining ActiveWaitArray left.

        ShiftWaitArray( threadCB, index+1, index,EndIndex - index ) ;

        // repair the blink and flink of the first and last elements in the list
        for (unsigned int i = 0; i< EndIndex-index; i++)
        {
            WaitInfo* firstWaitInfo = (WaitInfo*) threadCB->waitPointer[index+i].Flink;
            WaitInfo* lastWaitInfo = (WaitInfo*) threadCB->waitPointer[index+i].Blink;
            firstWaitInfo->link.Blink =  &(threadCB->waitPointer[index+i]);
            lastWaitInfo->link.Flink =  &(threadCB->waitPointer[index+i]);
        }
        // initialize the entry just freed
        InitializeListHead(&(threadCB->waitPointer[EndIndex]));

        threadCB->NumActiveWaits-- ;
        InterlockedDecrement(&threadCB->NumWaitHandles);
    }

    waitInfo->state &= ~WAIT_ACTIVE ;

}

void ThreadpoolMgr::DeleteWait(WaitInfo* waitInfo)
{
    CONTRACTL
    {
        if (waitInfo->ExternalCompletionEvent != INVALID_HANDLE) { THROWS;} else { NOTHROW; }
        MODE_ANY;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
    }
    CONTRACTL_END;

    if(waitInfo->Context && (waitInfo->flag & WAIT_FREE_CONTEXT)) {
        DelegateInfo* pDelegate = (DelegateInfo*) waitInfo->Context;

        // Since the delegate release destroys a handle, we need to be in
        // co-operative mode
        {
            GCX_COOP();
            pDelegate->Release();
        }

        RecycleMemory( pDelegate, MEMTYPE_DelegateInfo );
    }

    if (waitInfo->flag & WAIT_INTERNAL_COMPLETION)
    {
        waitInfo->InternalCompletionEvent.Set();
        return;  // waitInfo will be deleted by the thread that's waiting on this event
    }
    else if (waitInfo->ExternalCompletionEvent != INVALID_HANDLE)
    {
        UnsafeSetEvent(waitInfo->ExternalCompletionEvent);

        if(waitInfo->ExternalEventSafeHandle != NULL)
            // Release the safe handle and the GC handle holding it
            ReleaseWaitInfo(waitInfo);
    }

    delete waitInfo;


}



/************************************************************************/
BOOL ThreadpoolMgr::UnregisterWaitEx(HANDLE hWaitObject,HANDLE Event)
{
    CONTRACTL
    {
        THROWS; //NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE(IsInitialized());              // cannot call unregister before first registering

    const BOOL Blocking = (Event == (HANDLE) -1);
    WaitInfo* waitInfo = (WaitInfo*) hWaitObject;

    if (!hWaitObject)
    {
        return FALSE;
    }

    // we do not allow callbacks to run in the wait thread, hence the assert
    _ASSERTE(GetCurrentThreadId() != waitInfo->threadCB->threadId);


    if (Blocking)
    {
        waitInfo->InternalCompletionEvent.CreateAutoEvent(FALSE);
        waitInfo->flag |= WAIT_INTERNAL_COMPLETION;

    }
    else
    {
        waitInfo->ExternalCompletionEvent = (Event ? Event : INVALID_HANDLE);
        _ASSERTE((waitInfo->flag & WAIT_INTERNAL_COMPLETION) == 0);
        // we still want to block until the wait has been deactivated
        waitInfo->PartialCompletionEvent.CreateAutoEvent(FALSE);
    }

#ifdef _DEBUG
    // When its time to release the safehandle, if we find that the owning appdomain no longer exists
    // then we cannot get to the safehandle and release it. This means we'll be leaking the native handle its
    // holding. To prevent that, if the owning AD is gone, we call CloseHandle on the native handle
    // that we have access to.. ie waitInfo->ExternalCompletionEvent
    if (waitInfo->ExternalEventSafeHandle != NULL)
    {
        GCX_COOP();
        SAFEHANDLEREF refSH = (SAFEHANDLEREF) ObjectToOBJECTREF(ObjectFromHandle(waitInfo->ExternalEventSafeHandle));
        _ASSERTE(refSH->GetHandle() == waitInfo->ExternalCompletionEvent);
    }
#endif

    THREADPOOL_LOG2(TPL_PRIV | TPL_LOG, LL_INFO1000, "Unregistering wait, waitHandle=%x, Context=%x\n", waitInfo->waitHandle, waitInfo->Context);

    BOOL status = QueueUserAPC((PAPCFUNC)DeregisterWait,
                               waitInfo->threadCB->threadHandle,
                               (size_t) waitInfo);

    if (status == 0)
    {
        if (Blocking)
            waitInfo->InternalCompletionEvent.CloseEvent();
        else
            waitInfo->PartialCompletionEvent.CloseEvent();
        return FALSE;
    }

    if (!Blocking)
    {
        waitInfo->PartialCompletionEvent.Wait(INFINITE,TRUE);
        waitInfo->PartialCompletionEvent.CloseEvent();
        // we cannot do DeleteWait in DeregisterWait, since the DeleteWait could happen before
        // we close the event. So, the code has been moved here.
        if (InterlockedDecrement(&waitInfo->refCount) == 0)
        {
            DeleteWait(waitInfo);
        }
    }

    else        // i.e. blocking
    {
        _ASSERTE(waitInfo->flag & WAIT_INTERNAL_COMPLETION);
        _ASSERTE(waitInfo->ExternalEventSafeHandle == NULL);

        waitInfo->InternalCompletionEvent.Wait(INFINITE,TRUE);
        waitInfo->InternalCompletionEvent.CloseEvent();
        delete waitInfo;  // if WAIT_INTERNAL_COMPLETION is not set, waitInfo will be deleted in DeleteWait
    }
    return TRUE;
}


void ThreadpoolMgr::DeregisterWait(WaitInfo* pArgs)
{

    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_INTOLERANT;

    WaitInfo* waitInfo = pArgs;

    if ( ! (waitInfo->state & WAIT_REGISTERED) )
    {
        // set state to deleted, so that it does not get registered
        waitInfo->state |= WAIT_DELETE ;

        // since the wait has not even been registered, we dont need an interlock to decrease the RefCount
        waitInfo->refCount--;

        if (waitInfo->PartialCompletionEvent.IsValid())
        {
            waitInfo->PartialCompletionEvent.Set();
        }
        return;
    }

    if (waitInfo->state & WAIT_ACTIVE)
    {
        DeactivateWait(waitInfo);
    }

    if ( waitInfo->PartialCompletionEvent.IsValid())
    {
        waitInfo->PartialCompletionEvent.Set();
        return;     // we cannot delete the wait here since the PartialCompletionEvent
                    // may not have been closed yet. so, we return and rely on the waiter of PartialCompletionEvent
                    // to do the close
    }

    if (InterlockedDecrement(&waitInfo->refCount) == 0)
    {
        DeleteWait(waitInfo);
    }
    return;
}


/* This gets called in a finalizer thread ONLY IF an app does not deregister the
   the wait. Note that just because the registeredWaitHandle is collected by GC
   does not mean it is safe to delete the wait. The refcount tells us when it is
   safe.
*/
void ThreadpoolMgr::WaitHandleCleanup(HANDLE hWaitObject)
{
    LEAF_CONTRACT;

    WaitInfo* waitInfo = (WaitInfo*) hWaitObject;
    _ASSERTE(waitInfo->refCount > 0);

    // If g_fSuspendOnShutdown is true, the wait thread is
    // waiting for end of EE shutdown and there is no need to
    // QueueUserAPC to the wait thread because the wait thread
    // will go back to wait for end of EE shutdown if it is
    // waked up by the QueueUserAPC.
    if (g_fSuspendOnShutdown)
        return;

    QueueUserAPC((PAPCFUNC)DeregisterWait,
                     waitInfo->threadCB->threadHandle,
                     (size_t) waitInfo);
}

BOOL ThreadpoolMgr::CreateGateThread(SSIZE_T threadTypeStatus)
{
    LEAF_CONTRACT;

    DWORD threadId;

    _ASSERTE ((threadTypeStatus == GATE_THREAD_STATUS_NOWORKERTHREAD) ||
              (threadTypeStatus == GATE_THREAD_STATUS_NOCPTHREAD));

    SSIZE_T newstatus;
    SSIZE_T result;

    do
    {
        newstatus = GATE_THREAD_STATUS_CREATED | (~threadTypeStatus & GATE_THREAD_STATUS_MASK);

        result = FastInterlockCompareExchange(&GateThreadCreated, newstatus, GATE_THREAD_STATUS_NOTCREATED);

        if (result != GATE_THREAD_STATUS_NOTCREATED)
        {
           // cmpxchg failed implies that thread is already created. However, we need to make sure
           // that the thread is not going away
           if ((result & threadTypeStatus) == 0)        // thread cannot go away because of my threadtype, so return
                return TRUE;
           else
           {
               // turn off the bit for my threadtype to prevent the gate thread going away
               if (FastInterlockCompareExchange(&GateThreadCreated,result & ~threadTypeStatus,result) == result)
               {
                   return TRUE;
               }
               else
               {
                  // someone changed the status on us. Try again.
               }
           }
        }
        else
            break;
    } while (true);

    HANDLE threadHandle = CreateThread(NULL,                // security descriptor
                                       0,                   // default stack size
                                       GateThreadStart, //
                                       NULL,                //no arguments
                                       CREATE_SUSPENDED,    // start immediately
                                       &threadId);

    if (threadHandle)
    {
        DWORD status;
        status = ResumeThread(threadHandle);
        _ASSERTE(status != (DWORD) -1);

        THREADPOOL_LOG0(TPL_PRIV | TPL_LOG, LL_INFO100, "Gate thread started\n");

        CloseHandle(threadHandle);  //we don't need this anymore
        return TRUE;
    }

    return FALSE;
}



/************************************************************************/

BOOL ThreadpoolMgr::BindIoCompletionCallback(HANDLE FileHandle,
                                            LPOVERLAPPED_COMPLETION_ROUTINE Function,
                                            ULONG Flags,
                                            DWORD& errCode)
{

    CONTRACTL
    {
        THROWS;     // EnsureInitialized can throw
        if (GetThread()) { GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        MODE_ANY;
    }
    CONTRACTL_END;

    errCode = S_OK;


    EnsureInitialized();

    IHostIoCompletionManager *provider = CorHost2::GetHostIoCompletionManager();
    if (provider) {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        errCode = provider->Bind(GlobalCompletionPort, FileHandle);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (FAILED(errCode))
        {
            SetLastHRError(errCode);
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    _ASSERTE(GlobalCompletionPort != NULL);

    // there could be a race here, but at worst we will have N threads starting up where N = number of CPUs
    if (!InitCompletionPortThreadpool)
    {
        InitCompletionPortThreadpool = TRUE;
        CreateCompletionPortThread(NULL);
        EnsureGateThreadCreated(GATE_THREAD_STATUS_NOCPTHREAD);
    }
    else
    {
        GrowCompletionPortThreadpoolIfNeeded();
    }

    HANDLE h = PAL_CreateIoCompletionPort(FileHandle,
                                        GlobalCompletionPort,
                                        (ULONG_PTR) Function,
                                        NumberOfProcessors);

    if (h == NULL)
    {
        errCode = GetLastError();
        return FALSE;
    }

    _ASSERTE(h == GlobalCompletionPort);

    THREADPOOL_LOG2(TPL_PRIV | TPL_LOG, LL_INFO1000, "Bind IOCompletion callback, fileHandle=%x, Function=%x\n", FileHandle, Function);

    return TRUE;

}

BOOL ThreadpoolMgr::CreateCompletionPortThread(LPVOID lpArgs)
{
    CONTRACTL
    {
        NOTHROW;
        if (GetThread()) { GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        MODE_ANY;
    }
    CONTRACTL_END;

    Thread *pThread;
    BOOL fIsCLRThread;
    if ((pThread = CreateUnimpersonatedThread(CompletionPortThreadStart, lpArgs, &fIsCLRThread)) != NULL)
    {
        LastCPThreadCreation = GetTickCount();          // record this for use by logic to spawn additional threads
        InterlockedIncrement(&NumCPThreads);

        if (fIsCLRThread) {
            pThread->StartThread();
        }
        else {
            DWORD status;
            status = ResumeThread((HANDLE)pThread);
            _ASSERTE(status != (DWORD) (-1));
            CloseHandle((HANDLE)pThread);          // we don't need this anymore
        }

        if ( ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) ) {
            ETW_THREADPOOL_INFO Info;
            Info.IOThread.Count = NumCPThreads;
            Info.IOThread.NumRetired = NumRetiredCPThreads;
            g_pEtwTracer->EtwTraceEvent(&IOThreadCreateGuid,
                                        ETW_TYPE_THREADPOOL_CREATE_IO,
                                        &Info, sizeof(Info.IOThread));
        }

        THREADPOOL_LOG1(TPL_PRIV | TPL_LOG, LL_INFO100, "Completion port thread created (NumCPThreads=%d)\n", NumCPThreads);

        return TRUE;
    }


    return FALSE;
}

DWORD __stdcall ThreadpoolMgr::CompletionPortThreadStart(LPVOID lpArgs)
{
    ClrFlsSetThreadType (ThreadType_Threadpool_IOCompletion);

    CONTRACTL
    {
        THROWS;
        if (GetThread()) { MODE_PREEMPTIVE;} else { DISABLED(MODE_ANY);}
        if (GetThread()) { GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE (!CLRIoCompletionHosted());

    BOOL status;
    DWORD numBytes;
    size_t key;
    LPOVERLAPPED pOverlapped = NULL;
    DWORD errorCode;

    #define CP_THREAD_WAIT 15000 /* milliseconds */
    #define CP_THREAD_POOL_TIMEOUT  60000  // 1 minutes

    _ASSERTE(GlobalCompletionPort != NULL);

    THREADPOOL_LOG0(TPL_PRIV | TPL_LOG, LL_INFO100, "Completion port thread started\n");

    BOOL fThreadInit = FALSE;
    Thread *pThread = NULL;
    if (g_fEEStarted) {
        pThread = SetupThreadPoolThreadNoThrow(CompletionPortThread);
        if (pThread == NULL) {
            return 0;
        }
        fThreadInit = TRUE;
    }


    DWORD cpThreadWait = CP_THREAD_WAIT;
CompletionPortThreadStartLoop:
    for (;; )
    {
        if (!fThreadInit) {
            if (g_fEEStarted) {
                pThread = SetupThreadPoolThreadNoThrow(CompletionPortThread);
                if (pThread == NULL) {
                    break;
                }


                fThreadInit = TRUE;
            }
        }

        if (pThread && pThread->PreemptiveGCDisabled()) {
            pThread->EnablePreemptiveGC();
        }

        InterlockedIncrement(&NumFreeCPThreads);

        errorCode = S_OK;

        if (lpArgs == NULL)
        {
            CONTRACT_VIOLATION(ThrowsViolation);
            LeaveRuntimeHolder holder((size_t)GetQueuedCompletionStatus);
            status = GetQueuedCompletionStatus(
                        GlobalCompletionPort,
                        &numBytes,
                        (PULONG_PTR)&key,
                        &pOverlapped,
                        cpThreadWait
                        );
        }
        else
        {
            status = 1;     // non-0 equals success

            QueuedStatus *CompletionStatus = (QueuedStatus*)lpArgs;
            numBytes = CompletionStatus->numBytes;
            key = (size_t)CompletionStatus->key;
            pOverlapped = CompletionStatus->pOverlapped;
            errorCode = CompletionStatus->errorCode;
            delete CompletionStatus;
            lpArgs = NULL;  // one-time deal for initial CP packet
        }

        InterlockedDecrement(&NumFreeCPThreads);

        // Check if the thread needs to exit
        if (status == 0)
        {
            errorCode = GetLastError();
        }

        if (errorCode == WAIT_TIMEOUT)
        {
            if (ShouldExitThread())
            {
                // if I'm the last thread, don't die until no activity for certain time period.
                // A race condition may occur when there are multiple CP worker threads time out
                // on waiting on the IOCP. Since the actual decrement to NumCPThreads
                // “InterlockedDecrement(&NumCPThreads);” occurs at the end of this function,
                // none of the time out CP worker thread thinks it is the last thread,
                // so all of them exit and cause NumCPThreads to become zero.
                // An additional code block is added at the end of this function to check if
                // NumCPThreads == 0 before the last thread exits.
                if (NumCPThreads == 1 && ((GetTickCount() - LastCompletionTime) > CP_THREAD_POOL_TIMEOUT))
                {
                    SSIZE_T oldvalue = GateThreadCreated;
                    _ASSERTE((oldvalue & GATE_THREAD_STATUS_NOCPTHREAD) == 0);
                    if ((oldvalue == GATE_THREAD_STATUS_NOTCREATED) ||
                        (FastInterlockCompareExchange(&GateThreadCreated,
                                                     oldvalue | GATE_THREAD_STATUS_NOCPTHREAD,
                                                     oldvalue) == oldvalue))
                    {
                        cpThreadWait = (DWORD)INFINITE_TIMEOUT;
                    }
                    // else we failed for some reason, we'll try again next time we timeout

                    continue;   // put back into rotation
                }
                break;  // exit thread
            }

            // the fact that we're here means we can't exit due to pending io, or we're not the last real thread
            // (there may be retired threads sitting around).  If there are other available threads able to pick
            // up a request, then we'll retire, otherwise put back into rotation.
            if (NumFreeCPThreads == 0)
                continue;

            BOOL bExit = FALSE;
            InterlockedIncrement((LPLONG)&NumRetiredCPThreads);
            for (;;)
            {
                // now in "retired mode" waiting for pending io to complete
                if ( ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) ) {
                    ETW_THREADPOOL_INFO Info;
                    Info.IOThread.Count = NumCPThreads;
                    Info.IOThread.NumRetired = NumRetiredCPThreads;
                    g_pEtwTracer->EtwTraceEvent(&IOThreadRetireGuid,
                                                ETW_TYPE_THREADPOOL_RETIRE_IO,
                                                &Info, sizeof(Info.IOThread));
                }

                status = SafeWait(RetiredCPWakeupEvent,CP_THREAD_PENDINGIO_WAIT,FALSE);
                _ASSERTE(status == WAIT_TIMEOUT || status == WAIT_OBJECT_0);

                if (status == WAIT_TIMEOUT)
                {
                    if (ShouldExitThread())
                    {
                        // if I'm the last thread, don't die
                        if (NumCPThreads > 1)
                            bExit = TRUE;
                        else
                            bExit = FALSE;
                        InterlockedDecrement((LPLONG)&NumRetiredCPThreads);
                        break; // inner for
                    }
                    else
                        continue;   // keep waiting
                }
                else
                {
                    // put back into rotation -- we need a thread
                    bExit = FALSE;
                    InterlockedDecrement((LPLONG)&NumRetiredCPThreads);
                    break; // inner for
                }

            }

            if ( ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) ) {
                ETW_THREADPOOL_INFO Info;
                Info.IOThread.Count = NumCPThreads;
                Info.IOThread.NumRetired = NumRetiredCPThreads;
                g_pEtwTracer->EtwTraceEvent(&IOThreadRetireGuid,
                                            ETW_TYPE_THREADPOOL_UNRETIRE_IO,
                                            &Info, sizeof(Info.IOThread));
            }

            if (bExit == TRUE)
            {
                break; // outer for, exit thread
            }
            else continue;  // outer for, wait for new work

        }

        // else, we woke up becaue of an io, reset the wait time out (in case it had been changed to infinite)
        if (cpThreadWait == (DWORD)INFINITE_TIMEOUT)
        {
            cpThreadWait = CP_THREAD_WAIT;

            EnsureGateThreadCreated(GATE_THREAD_STATUS_NOCPTHREAD);
        }

        // The following check is not needed, and can probably go away
        else if (GateThreadCreated == GATE_THREAD_STATUS_NOTCREATED)
        {
            EnsureGateThreadCreated(GATE_THREAD_STATUS_NOCPTHREAD);
        }


        // We can not assert here.  If stdin/stdout/stderr of child process are redirected based on
        // async io, GetQueuedCompletionStatus returns when child process operates on its stdin/stdout/stderr.
        // Parent process does not issue any ReadFile/WriteFile, and hence pOverlapped is going to be NULL.
        //_ASSERTE(pOverlapped != NULL);

        if (pOverlapped != NULL)
        {
            _ASSERTE(key != 0);  // should be a valid function address

            if (key != 0)
            {
                if (GCHeap::IsGCInProgress(TRUE))
                {
                    // GC is imminent, so wait until GC is complete before executing next request.
                    // this reduces in-flight objects allocated right before GC, easing the GC's work
                    GCHeap::WaitForGCCompletion(TRUE);
                }

                GrowCompletionPortThreadpoolIfNeeded();

                LastCompletionTime = GetTickCount();

                THREADPOOL_LOG1(TPL_PRIV | TPL_LOG, LL_INFO1000, "Doing IO completion callback, function = %x\n", key);

                {
                    CONTRACT_VIOLATION(ThrowsViolation);
                    ThreadLocaleHolder localeHolder;
                    ((LPOVERLAPPED_COMPLETION_ROUTINE) key)(errorCode, numBytes, pOverlapped);
                }

                THREADPOOL_LOG1(TPL_PRIV | TPL_LOG, LL_INFO1000, "Returned from IO completion callback, function = %x\n", key);
                if (pThread == NULL) {
                    pThread = GetThread();
                }
                if (pThread) {
                    if (pThread->IsAbortRequested())
                        pThread->EEResetAbort(Thread::TAR_ALL);
                    pThread->InternalReset(FALSE);
                }
            }
            else
            {
                // Application bug - can't do much, just ignore it
            }

        }

    }   // for (;;)

    // exiting, so decrement target number of threads
    if (CurrentLimitTotalCPThreads >= NumCPThreads)
    {
        SSIZE_T limit = CurrentLimitTotalCPThreads;
        InterlockedCompareExchange(&CurrentLimitTotalCPThreads, limit-1, limit);
    }

    InterlockedDecrement(&NumCPThreads);

    // When there are multiple CP worker threads time out on waiting on the IOCP,
    // NumCPThreads may become 0 in race condition.
    // Make the last effort to keep at least 1 CP worker thread alive.
    if (NumCPThreads == 0)
    {
        if (InterlockedCompareExchange(&NumCPThreads, 1, 0) == 0)
        {
            goto CompletionPortThreadStartLoop;
        }
    }

    if ( ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) ) {
        ETW_THREADPOOL_INFO Info;
        Info.IOThread.Count = NumCPThreads;
        Info.IOThread.NumRetired = NumRetiredCPThreads;
        g_pEtwTracer->EtwTraceEvent(&IOThreadCreateGuid,
                                    ETW_TYPE_THREADPOOL_TERM_IO,
                                    &Info, sizeof(Info.IOThread));
    }

    if (pThread) {
        DestroyThread(pThread);
    }
    THREADPOOL_LOG1(TPL_PRIV | TPL_STRESS, LL_INFO100, "ThreadPool thread dying (completionPort case).  Thread stack at roughly %p\n", &lpArgs);
    return 0;
}


BOOL ThreadpoolMgr::ShouldExitThread()
{
    LEAF_CONTRACT;
    if (IsIoPending())

        return FALSE;

    else
//        return ((NumCPThreads > CurrentLimitTotalCPThreads) || (NumFreeCPThreads > MaxFreeCPThreads));
        return TRUE;

}


void ThreadpoolMgr::GrowCompletionPortThreadpoolIfNeeded()
{
    CONTRACTL
    {
        if (GetThread()) { GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        NOTHROW;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (NumFreeCPThreads == 0)
    {
        // adjust limit if neeeded

        if (NumRetiredCPThreads == 0 && NumCPThreads >= CurrentLimitTotalCPThreads)
        {
            int limit = CurrentLimitTotalCPThreads;

            if (limit < MaxLimitTotalCPThreads &&
                cpuUtilization < CpuUtilizationLow)

            {
                // add one more check to make sure that we haven't fired off a new
                // thread since the last time time we checked the cpu utilization.
                // However, don't bother if we haven't reached the MinLimit (2*number of cpus)
                if ((NumCPThreads < MinLimitTotalCPThreads) ||
                    SufficientDelaySinceLastSample(LastCPThreadCreation,NumCPThreads))
                {
                    FastInterlockCompareExchange(&CurrentLimitTotalCPThreads, limit+1, limit);
                }
            }
        }

        // create new CP thread if under limit, but don't bother if gc in progress
        if (!GCHeap::IsGCInProgress(TRUE) )
        {
            if (NumRetiredCPThreads > 0)
            {
                // wakeup retired thread instead
                RetiredCPWakeupEvent->Set();
            }
            else if (NumCPThreads < CurrentLimitTotalCPThreads)
            {
                THREADPOOL_LOG4(TPL_PRIV, LL_INFO100, "GateThread: spawn worker: Free:%d Retired:%d Total:%d Limit:%d\n", NumFreeCPThreads, NumRetiredCPThreads, NumCPThreads, CurrentLimitTotalCPThreads);
                CreateCompletionPortThread(NULL);
            }
        }

    }
}


// On NT4 and Win2K returns true if there is pending io on the thread.
// On Win9x return true.  Since there is no OS support for checking it,
// we err on the side of safety even though it results in a
// high water mark of threads.

BOOL ThreadpoolMgr::IsIoPending()
{
    LEAF_CONTRACT;
    return PAL_GetIOCPThreadIoPendingFlag();
}


int ThreadpoolMgr::GetCPUBusyTime_NT(PAL_IOCP_CPU_INFORMATION* pOldInfo)
{
    return PAL_GetCPUBusyTime(pOldInfo);
}




DWORD __stdcall ThreadpoolMgr::GateThreadStart(LPVOID lpArgs)
{
    ClrFlsSetThreadType (ThreadType_Gate);

    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

#define MAX_INACTIVE_PERIOD (30*1000)       // if there's no interesting activity for this long, terminate the gate thread
#define GATE_THREAD_TERMINATION_COUNTER (MAX_INACTIVE_PERIOD/GATE_THREAD_DELAY)

    __SwitchToThread(GATE_THREAD_DELAY);            // delay getting initial CPU reading

    PAL_IOCP_CPU_INFORMATION prevCPUInfo;
    GetCPUBusyTime_NT(&prevCPUInfo);                  // ignore return value the first time

    BOOL IgnoreNextSample = FALSE;
    unsigned countDownToExit = GATE_THREAD_TERMINATION_COUNTER;
    int     cpuUtilizationArraySlot = 0;
    long    cpuUtilizationArray[CPU_UTILIZATION_SAMPLES];
    long    cpuUtilizationTemp;

    memset(cpuUtilizationArray, 0, sizeof(long)*CPU_UTILIZATION_SAMPLES);

    for (;;)
    {
        __SwitchToThread(GATE_THREAD_DELAY);


#ifdef DEBUGGING_SUPPORTED
        // if we are stopped at a debug breakpoint, go back to sleep
        if (CORDebuggerAttached() && g_pDebugInterface->IsStopped())
            continue;
#endif // DEBUGGING_SUPPORTED


        {

            if (!GCHeap::IsGCInProgress(FALSE) )
            {
                if (IgnoreNextSample)
                {
                    IgnoreNextSample = FALSE;
                    cpuUtilizationTemp = GetCPUBusyTime_NT(&prevCPUInfo);            // updates prevCPUInfo as side effect
                    // don't artificially drive down average if cpu is high
                    if (cpuUtilizationTemp <= CpuUtilizationLow)
                        cpuUtilization = CpuUtilizationLow + 1;
                    else
                        cpuUtilization = cpuUtilizationTemp;
                }
                else
                {
                    cpuUtilization = (long) GetCPUBusyTime_NT(&prevCPUInfo);            // updates prevCPUInfo as side effect
                }
            }
            else
            {
                cpuUtilizationTemp = GetCPUBusyTime_NT(&prevCPUInfo);            // updates prevCPUInfo as side effect
                // don't artificially drive down average if cpu is high
                if (cpuUtilizationTemp <= CpuUtilizationLow)
                    cpuUtilization = CpuUtilizationLow + 1;
                else
                    cpuUtilization = cpuUtilizationTemp;
                IgnoreNextSample = TRUE;
            }

            _ASSERTE(cpuUtilizationArraySlot < CPU_UTILIZATION_SAMPLES);
            cpuUtilizationArray[cpuUtilizationArraySlot] = cpuUtilization;
            if (cpuUtilizationArraySlot == CPU_UTILIZATION_SAMPLES - 1)
                cpuUtilizationArraySlot = 0;
            else
                cpuUtilizationArraySlot++;

            long averageCpuCalc = 0;
            for (int i = 0; i < CPU_UTILIZATION_SAMPLES; i++)
            {
                averageCpuCalc += cpuUtilizationArray[i];
            }
            cpuUtilizationAverage = averageCpuCalc / CPU_UTILIZATION_SAMPLES;


            SSIZE_T oldLimit = CurrentLimitTotalCPThreads;
            SSIZE_T newLimit = CurrentLimitTotalCPThreads;

            // don't mess with CP thread pool settings if not initialized yet
            if (InitCompletionPortThreadpool)
            {
                _ASSERTE (!CLRIoCompletionHosted());

                if (NumFreeCPThreads == 0 &&
                    NumRetiredCPThreads == 0 &&
                    NumCPThreads < MaxLimitTotalCPThreads &&
                    !GCHeap::IsGCInProgress(TRUE))

                {
                    BOOL status;
                    DWORD numBytes;
                    size_t key;
                    LPOVERLAPPED pOverlapped;
                    DWORD errorCode;

                    errorCode = S_OK;

                    status = GetQueuedCompletionStatus(
                                GlobalCompletionPort,
                                &numBytes,
                                (PULONG_PTR)&key,
                                &pOverlapped,
                                0 // immediate return
                                );

                    if (status == 0)
                    {
                        errorCode = GetLastError();
                    }

                    if (errorCode != WAIT_TIMEOUT)
                    {
                        QueuedStatus *CompletionStatus = NULL;

                        // loop, retrying until memory is allocated.  Under such conditions the gate
                        // thread is not useful anyway, so I feel comfortable with this behavior
                        do
                        {
                            // make sure to free mem later in thread
                            CompletionStatus = new (nothrow) QueuedStatus;
                            if (CompletionStatus == NULL)
                            {
                                __SwitchToThread(GATE_THREAD_DELAY);
                            }
                        }
                        while (CompletionStatus == NULL);

                        CompletionStatus->numBytes = numBytes;
                        CompletionStatus->key = (PULONG_PTR)key;
                        CompletionStatus->pOverlapped = pOverlapped;
                        CompletionStatus->errorCode = errorCode;

                        // loop, retrying until thread is created.
                        while (!CreateCompletionPortThread((LPVOID)CompletionStatus))
                        {
                            __SwitchToThread(GATE_THREAD_DELAY);
                        }

                        if (CurrentLimitTotalCPThreads < MaxLimitTotalCPThreads)
                        {
                            // increment after thread start to reduce chance of extra thread creation
                            FastInterlockCompareExchange(&CurrentLimitTotalCPThreads, oldLimit+1, oldLimit);
                        }
                    }
                }
                else if (cpuUtilization > CpuUtilizationHigh)
                {
                    if (oldLimit > MinLimitTotalCPThreads)
                        newLimit = oldLimit-1;
                }
                else if (cpuUtilization < CpuUtilizationLow)
                {
                    // this could be an indication that threads might be getting blocked or there is no work
                    if (oldLimit < MaxLimitTotalCPThreads &&    // don't go beyond the hard upper limit
                        NumFreeCPThreads == 0 &&                // don't bump the limit if there are already free threads
                        NumCPThreads >= oldLimit)               // don't bump the limit if the number of threads haven't caught up to the old limit yet
                    {
                        // if we need to add a new thread, wake up retired threads instead of creating new ones
                        if (NumRetiredCPThreads > 0)
                            RetiredCPWakeupEvent->Set();
                        else
                            newLimit = oldLimit+1;
                    }
                }


                if (newLimit != oldLimit)
                {
                    THREADPOOL_LOG2(TPL_PRIV, LL_INFO1000, "GateThread: new limit: %d -> %d\n", oldLimit, newLimit);
                    FastInterlockCompareExchange(&CurrentLimitTotalCPThreads, newLimit, oldLimit);
                }

                if (newLimit > oldLimit ||
                    NumCPThreads < oldLimit)
                {
                    GrowCompletionPortThreadpoolIfNeeded();
                }
            }
        }

        if (MonitorWorkRequestsQueue)
        {
            GrowWorkerThreadPoolIfStarvationSimple();
        }

        // check to see if gate thread needs to exit
        // the conditions for terminating the gate thread are:
        // a) there are no worker threads to monitor
        // b) there are no cp threads to monitor (or non-initialization of cp threadpool)

        SSIZE_T oldvalue = GateThreadCreated;

        if ((oldvalue & GATE_THREAD_STATUS_NOWORKERTHREAD) &&
            (oldvalue & GATE_THREAD_STATUS_NOCPTHREAD))
        {
            countDownToExit--;
        }
        else
        {
            // reset the counter
            countDownToExit = GATE_THREAD_TERMINATION_COUNTER;
        }

        if (countDownToExit == 0)
        {
            if (FastInterlockCompareExchange(&GateThreadCreated,
                                             GATE_THREAD_STATUS_NOTCREATED,
                                             oldvalue)
                        == oldvalue)
                return 0;
            else // else, the status changed, reset the counter
                countDownToExit = GATE_THREAD_TERMINATION_COUNTER;
        }

    }       // for (;;)
}

// called by logic to spawn a new completion port thread.
// return false if not enough time has elapsed since the last
// time we sampled the cpu utilization.
BOOL ThreadpoolMgr::SufficientDelaySinceLastSample(unsigned int LastThreadCreationTime,
                                                   unsigned NumThreads,   // total number of threads of that type (worker or CP)
                                                   double    throttleRate // the delay is increased by this percentage for each extra thread
                                                   )
{
    LEAF_CONTRACT;

    unsigned dwCurrentTickCount =  GetTickCount();

    unsigned delaySinceLastThreadCreation = dwCurrentTickCount - LastThreadCreationTime;

    unsigned minWaitBetweenThreadCreation =  GATE_THREAD_DELAY;

    if (throttleRate > 0.0)
    {
        _ASSERTE(throttleRate <= 1.0);

        unsigned adjustedThreadCount = NumThreads > NumberOfProcessors ? (NumThreads - NumberOfProcessors) : 0;

        minWaitBetweenThreadCreation = (unsigned) (GATE_THREAD_DELAY * pow((1.0 + throttleRate),(double)adjustedThreadCount));
    }
    // the amount of time to wait should grow up as the number of threads is increased

    return (delaySinceLastThreadCreation > minWaitBetweenThreadCreation);

}


// called by logic to spawn new worker threads, return true if it's been too long
// since the last dequeue operation - takes number of worker threads into account
// in deciding "too long"
BOOL ThreadpoolMgr::SufficientDelaySinceLastDequeue()
{
    LEAF_CONTRACT;

    #define DEQUEUE_DELAY_THRESHOLD (GATE_THREAD_DELAY * 2)

    unsigned delay = GetTickCount() - LastDequeueTime;

    unsigned tooLong = NumWorkerThreads * DEQUEUE_DELAY_THRESHOLD;

    return (delay > tooLong);

}

#ifdef _MSC_VER
#pragma warning (default : 4715)
#endif

/************************************************************************/

struct CreateTimerThreadParams {
    CLREvent    event;
    BOOL        setupSucceeded;
};

BOOL ThreadpoolMgr::CreateTimerQueueTimer(PHANDLE phNewTimer,
                                          WAITORTIMERCALLBACK Callback,
                                          PVOID Parameter,
                                          DWORD DueTime,
                                          DWORD Period,
                                          ULONG Flag)
{
    CONTRACTL
    {
        THROWS;     // EnsureInitialized, CreateAutoEvent can throw
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}  // There can be calls thru ICorThreadpool
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    EnsureInitialized();

    if (NULL == TimerThread)
    {
        CrstHolder csh(&TimerQueueCriticalSection);

        // check again
        if (NULL == TimerThread)
        {
            DWORD threadId;
            CreateTimerThreadParams params;
            params.event.CreateAutoEvent(FALSE);
            params.setupSucceeded = FALSE;

            HANDLE TimerThreadHandle
                        = CreateThread(NULL,                // security descriptor
                                       0,                   // default stack size
                                       TimerThreadStart,        //
                                       &params,
                                       0,
                                       &threadId);
            if (TimerThreadHandle == NULL)
            {
                params.event.CloseEvent();
                ThrowOutOfMemory();
            }

            {
                GCX_PREEMP();
                for(;;)
                {
                    // if a host throws because it couldnt allocate another thread,
                    // just retry the wait.
                    if (SafeWait(&params.event,INFINITE, FALSE) != WAIT_TIMEOUT)
                        break;
                }
            }
            params.event.CloseEvent();

            if (!params.setupSucceeded)
            {
                CloseHandle(TimerThreadHandle);
                return FALSE;
            }

            TimerThread = TimerThreadHandle;
            THREADPOOL_LOG0(TPL_PRIV | TPL_LOG, LL_INFO100, "Timer thread started\n");
        }

    }


    NewHolder<TimerInfo> timerInfoHolder;
    TimerInfo * timerInfo = new (nothrow) TimerInfo;
    *phNewTimer = (HANDLE) timerInfo;

    if (NULL == timerInfo)
        ThrowOutOfMemory();

    timerInfoHolder.Assign(timerInfo);

    timerInfo->FiringTime = DueTime;
    timerInfo->Function = (LPVOID) Callback;
    timerInfo->Context = Parameter;
    timerInfo->Period = Period;
    timerInfo->state = 0;
    timerInfo->flag = Flag;
    timerInfo->ExternalCompletionEvent = INVALID_HANDLE;
    timerInfo->ExternalEventSafeHandle = NULL;
    timerInfo->handleOwningAD = (ADID) 0;
    timerInfo->bReleaseEventIfADUnloaded = FALSE;

    BOOL status = QueueUserAPC((PAPCFUNC)InsertNewTimer,TimerThread,(size_t)timerInfo);
    if (FALSE == status)
    {
        return FALSE;
    }

    timerInfoHolder.SuppressRelease();
    return TRUE;
}

#ifdef _MSC_VER
#pragma warning (disable : 4715)
#endif
DWORD __stdcall ThreadpoolMgr::TimerThreadStart(LPVOID p)
{
    ClrFlsSetThreadType (ThreadType_Timer);

    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;        // due to SetApartment
    STATIC_CONTRACT_MODE_PREEMPTIVE;
    STATIC_CONTRACT_SO_INTOLERANT;
    /* cannot use contract because of SEH
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;*/

    CreateTimerThreadParams* params = (CreateTimerThreadParams*)p;

    Thread* pThread = SetupThreadPoolThreadNoThrow(TimerMgrThread);

    params->setupSucceeded = (pThread == NULL) ? 0 : 1;
    params->event.Set();

    if (pThread == NULL)
        return 0;

    pTimerThread = pThread;
    // Timer threads never die

    LastTickCount = GetTickCount();


    for (;;)
    {
         // moved to its own function since EX_TRY consumes stack
#ifdef _MSC_VER
#pragma inline_depth (0) // the function containing EX_TRY can't be inlined here
#endif
        TimerThreadFire();
#ifdef _MSC_VER
#pragma inline_depth (20)
#endif
    }

}

void ThreadpoolMgr::TimerThreadFire()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;

    EX_TRY {
        DWORD timeout = FireTimers();

#undef SleepEx
        SleepEx(timeout, TRUE);
#define SleepEx(a,b) Dont_Use_SleepEx(a,b)

        // the thread could wake up either because an APC completed or the sleep timeout
        // in both case, we need to sweep the timer queue, firing timers, and readjusting
        // the next firing time

    }
    EX_CATCH {
        if (SwallowUnhandledExceptions())
        {
            // Do nothing to swallow the exception
        }
        else
        {
            EX_RETHROW;
        }
    }
    EX_END_CATCH(SwallowAllExceptions);
}

#ifdef _MSC_VER
#pragma warning (default : 4715)
#endif

// Executed as an APC in timer thread
void ThreadpoolMgr::InsertNewTimer(TimerInfo* pArg)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    STATIC_CONTRACT_SO_INTOLERANT;

    _ASSERTE(pArg);
    TimerInfo * timerInfo = pArg;

    if (timerInfo->state & TIMER_DELETE)
    {   // timer was deleted before it could be registered
        DeleteTimer(timerInfo);
        return;
    }

    // set the firing time = current time + due time (note initially firing time = due time)
    DWORD currentTime = GetTickCount();
    if (timerInfo->FiringTime == (ULONG) -1)
    {
        timerInfo->state = TIMER_REGISTERED;
        timerInfo->refCount = 1;

    }
    else
    {
        timerInfo->FiringTime += currentTime;

        timerInfo->state = (TIMER_REGISTERED | TIMER_ACTIVE);
        timerInfo->refCount = 1;

        // insert the timer in the queue
        InsertTailList(&TimerQueue,(&timerInfo->link));
    }

    THREADPOOL_LOG2(TPL_PRIV | TPL_LOG, LL_INFO1000, "Timer created, period= %x, Function = %x\n", timerInfo->Period, timerInfo->Function);

    return;
}


// executed by the Timer thread
// sweeps through the list of timers, readjusting the firing times, queueing APCs for
// those that have expired, and returns the next firing time interval
DWORD ThreadpoolMgr::FireTimers()
{
    CONTRACTL
    {
        THROWS;     // QueueUserWorkItem can throw
        if (GetThread()) { GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        if (GetThread()) { MODE_PREEMPTIVE;} else { DISABLED(MODE_ANY);}
    }
    CONTRACTL_END;

    DWORD currentTime = GetTickCount();

    DWORD nextFiringInterval = (DWORD) -1;

    for (LIST_ENTRY* node = (LIST_ENTRY*) TimerQueue.Flink;
         node != &TimerQueue;
        )
    {
        TimerInfo* timerInfo = (TimerInfo*) node;
        node = (LIST_ENTRY*) node->Flink;

        if (TimeExpired(LastTickCount, currentTime, timerInfo->FiringTime))
        {
            if (timerInfo->Period == 0 || timerInfo->Period == (ULONG) -1)
            {
                DeactivateTimer(timerInfo);
            }

            InterlockedIncrement(&timerInfo->refCount);

            QueueUserWorkItem(AsyncTimerCallbackCompletion,
                              timerInfo,
                              QUEUE_ONLY /* TimerInfo take care of deleting*/);

            timerInfo->FiringTime = currentTime+timerInfo->Period;

            if ((timerInfo->Period != 0) && (timerInfo->Period != (ULONG) -1) && (nextFiringInterval > timerInfo->Period))
                nextFiringInterval = timerInfo->Period;
        }

        else
        {
            DWORD firingInterval = TimeInterval(timerInfo->FiringTime,currentTime);
            if (firingInterval < nextFiringInterval)
                nextFiringInterval = firingInterval;
        }
    }

    LastTickCount = currentTime;

    return nextFiringInterval;
}

DWORD __stdcall ThreadpoolMgr::AsyncTimerCallbackCompletion(PVOID pArgs)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HRESULT hr;
    Thread* pThread = GetThread();
    if (pThread == NULL)
    {
        pThread = SetupThreadPoolThreadNoThrow(WorkerThread, &hr);
        if (pThread == NULL)
            return hr;
    }

    BEGIN_SO_INTOLERANT_CODE(pThread);

    TimerInfo* timerInfo = (TimerInfo*) pArgs;
    ((WAITORTIMERCALLBACKFUNC) timerInfo->Function) (timerInfo->Context, TRUE) ;

    if (InterlockedDecrement(&timerInfo->refCount) == 0)
        DeleteTimer(timerInfo);

    END_SO_INTOLERANT_CODE;

    return ERROR_SUCCESS;
}


// removes the timer from the timer queue, thereby cancelling it
// there may still be pending callbacks that haven't completed
void ThreadpoolMgr::DeactivateTimer(TimerInfo* timerInfo)
{
    LEAF_CONTRACT;

    RemoveEntryList((LIST_ENTRY*) timerInfo);

    // This timer info could go into another linked list of timer infos
    // waiting to be released. Reinitialize the list pointers
    InitializeListHead(&timerInfo->link);
    timerInfo->state = timerInfo->state & ~TIMER_ACTIVE;
}

void ThreadpoolMgr::DeleteTimer(TimerInfo* timerInfo)
{
    CONTRACTL
    {
        if (GetThread() == pTimerThread) { NOTHROW; } else { THROWS; }
        if (GetThread() == pTimerThread) { GC_NOTRIGGER; } else { GC_TRIGGERS; }
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE((timerInfo->state & TIMER_ACTIVE) == 0);

    _ASSERTE(!(timerInfo->flag & WAIT_FREE_CONTEXT));

    if (timerInfo->flag & WAIT_INTERNAL_COMPLETION)
    {
        timerInfo->InternalCompletionEvent.Set();
        return; // the timerInfo will be deleted by the thread that's waiting on InternalCompletionEvent
    }

    BOOL bDeleteTimerInfo = TRUE;
    if (timerInfo->ExternalCompletionEvent != INVALID_HANDLE)
    {
        UnsafeSetEvent(timerInfo->ExternalCompletionEvent);
    }

    if (GetThread() == pTimerThread)
    {
        // Releasing GC handles can block. So we wont do this on the timer thread.
        // We'll put it in a list which will be processed by a worker thread
        if (timerInfo->Context != NULL ||
            timerInfo->ExternalEventSafeHandle != NULL)
        {
            QueueTimerInfoForRelease(timerInfo);
            bDeleteTimerInfo = FALSE;
        }
    }
    else
    {
        GCX_COOP();
        if (timerInfo->Context != NULL)
        {
            DelegateInfo *pDelInfo = (DelegateInfo *)timerInfo->Context;
            pDelInfo->Release();
            RecycleMemory( pDelInfo, MEMTYPE_DelegateInfo );
        }
        if (timerInfo->ExternalEventSafeHandle != NULL)
        {
            ReleaseTimerInfo(timerInfo);
        }
    }

    if (bDeleteTimerInfo)
        delete timerInfo;

}

// We add TimerInfos from deleted timers into a linked list.
// A worker thread will later release the handles held by the TimerInfo
// and recycle them if possible (See DelegateInfo::MakeDelegateInfo)
void ThreadpoolMgr::QueueTimerInfoForRelease(TimerInfo *pTimerInfo)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    // The synchronization in this method depends on the fact that
    //  - There is only one timer thread
    //  - The one and only timer thread is executing this method.
    //  - This function wont go into an alertable state. That could trigger another APC.
    // Else two threads can be queueing timerinfos and a race could
    // lead to leaked memory and handles
    _ASSERTE(GetThread());
    _ASSERTE(pTimerThread == GetThread());
    TimerInfo *pHead = NULL;

    // Make sure this timer info has been deactivated and removed from any other lists
    _ASSERTE((pTimerInfo->state & TIMER_ACTIVE) == 0);
    //_ASSERTE(pTimerInfo->link.Blink == &(pTimerInfo->link) &&
    //    pTimerInfo->link.Flink == &(pTimerInfo->link));
    // Make sure "link" is the first field in TimerInfo
    _ASSERTE(pTimerInfo == (PVOID)&pTimerInfo->link);

    // Grab any previously published list
    if ((pHead = (TimerInfo *)InterlockedExchangePointer((PVOID*)&TimerInfosToBeRecycled, NULL)) != NULL)
    {
        // If there already is a list, just append
        InsertTailList((LIST_ENTRY *)pHead, &pTimerInfo->link);
        pTimerInfo = pHead;
    }
    else
        // If this is the head, make its next and previous ptrs point to itself
        InitializeListHead((LIST_ENTRY*)&pTimerInfo->link);

    // Publish the list
    InterlockedExchangePointer((PVOID*)&TimerInfosToBeRecycled, pTimerInfo);

}

void ThreadpoolMgr::FlushQueueOfTimerInfos()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    TimerInfo *pHeadTimerInfo = NULL, *pCurrTimerInfo = NULL;
    LIST_ENTRY *pNextInfo = NULL;

    if ((pHeadTimerInfo = (TimerInfo *)InterlockedExchangePointer((PVOID*)&TimerInfosToBeRecycled, NULL)) == NULL)
        return;

    do
    {
        RemoveHeadList((LIST_ENTRY *)pHeadTimerInfo, pNextInfo);
        _ASSERTE(pNextInfo != NULL);

        pCurrTimerInfo = (TimerInfo *) pNextInfo;

        GCX_COOP();
        if (pCurrTimerInfo->Context != NULL)
        {
            DelegateInfo *pCurrDelInfo = (DelegateInfo *) pCurrTimerInfo->Context;
            pCurrDelInfo->Release();

            RecycleMemory( pCurrDelInfo, MEMTYPE_DelegateInfo );
        }

        if (pCurrTimerInfo->ExternalEventSafeHandle != NULL)
        {
            ReleaseTimerInfo(pCurrTimerInfo);
        }

        delete pCurrTimerInfo;

    }
    while (pCurrTimerInfo != pHeadTimerInfo);
}

/************************************************************************/
BOOL ThreadpoolMgr::ChangeTimerQueueTimer(
                                        HANDLE Timer,
                                        ULONG DueTime,
                                        ULONG Period)
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    _ASSERTE(IsInitialized());
    _ASSERTE(Timer);                    // not possible to give invalid handle in managed code

    NewHolder<TimerUpdateInfo> updateInfoHolder;
    TimerUpdateInfo *updateInfo = new TimerUpdateInfo;
    updateInfoHolder.Assign(updateInfo);

    updateInfo->Timer = (TimerInfo*) Timer;
    updateInfo->DueTime = DueTime;
    updateInfo->Period = Period;

    BOOL status = QueueUserAPC((PAPCFUNC)UpdateTimer,
                               TimerThread,
                               (size_t) updateInfo);

    if (status)
        updateInfoHolder.SuppressRelease();

    return(status);
}

void ThreadpoolMgr::UpdateTimer(TimerUpdateInfo* pArgs)
{
    WRAPPER_CONTRACT;

    TimerUpdateInfo* updateInfo = (TimerUpdateInfo*) pArgs;
    TimerInfo* timerInfo = updateInfo->Timer;

    timerInfo->Period = updateInfo->Period;

    if (updateInfo->DueTime == (ULONG) -1)
    {
        if (timerInfo->state & TIMER_ACTIVE)
        {
            DeactivateTimer(timerInfo);
        }
        // else, noop (the timer was already inactive)
        _ASSERTE((timerInfo->state & TIMER_ACTIVE) == 0);
        THREADPOOL_LOG2(TPL_PRIV | TPL_LOG, LL_INFO1000, "Timer inactive, period= %x, Function = %x\n", timerInfo->Period, timerInfo->Function);

        delete updateInfo;
        return;
    }

    DWORD currentTime = GetTickCount();
    timerInfo->FiringTime = currentTime + updateInfo->DueTime;

    delete updateInfo;

    if (! (timerInfo->state & TIMER_ACTIVE))
    {
        // timer not active (probably a one shot timer that has expired), so activate it
        timerInfo->state |= TIMER_ACTIVE;
        _ASSERTE(timerInfo->refCount >= 1);
        // insert the timer in the queue
        InsertTailList(&TimerQueue,(&timerInfo->link));

    }

    THREADPOOL_LOG2(TPL_PRIV | TPL_LOG, LL_INFO1000, "Timer changed, period= %x, Function = %x\n", timerInfo->Period, timerInfo->Function);

    return;
}

/************************************************************************/
BOOL ThreadpoolMgr::DeleteTimerQueueTimer(
                                        HANDLE Timer,
                                        HANDLE Event)
{
    CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(IsInitialized());          // cannot call delete before creating timer
    _ASSERTE(Timer);                    // not possible to give invalid handle in managed code

    // make volatile to avoid compiler reordering check after async call.
    // otherwise, DeregisterTimer could delete timerInfo before the comparison.
    TimerInfo* volatile timerInfo = (TimerInfo*) Timer;

    if (Event == (HANDLE) -1)
    {
        //CONTRACT_VIOLATION(ThrowsViolation);
        timerInfo->InternalCompletionEvent.CreateAutoEvent(FALSE);
        timerInfo->flag |= WAIT_INTERNAL_COMPLETION;
    }
    else if (Event)
    {
        timerInfo->ExternalCompletionEvent = Event;
    }
    else /* Event == NULL */
    {
        timerInfo->ExternalCompletionEvent = INVALID_HANDLE;
    }

    BOOL isBlocking = timerInfo->flag & WAIT_INTERNAL_COMPLETION;

#ifdef _DEBUG
    // When its time to release the safehandle, if we find that the owning appdomain no longer exists
    // then we cannot get to the safehandle and release it. This means we'll be leaking the native handle its
    // holding. To prevent that, if the owning AD is gone, we call CloseHandle on the native handle
    // that we have access to.. ie timerInfo->ExternalCompletionEvent
    if (timerInfo->ExternalEventSafeHandle != NULL)
    {
        GCX_COOP();
        SAFEHANDLEREF refSH = (SAFEHANDLEREF) ObjectToOBJECTREF(ObjectFromHandle(timerInfo->ExternalEventSafeHandle));
        _ASSERTE(refSH->GetHandle() == timerInfo->ExternalCompletionEvent);
    }
#endif

    BOOL status = QueueUserAPC((PAPCFUNC)DeregisterTimer,
                               TimerThread,
                               (size_t) timerInfo);

    if (FALSE == status)
    {
        if (isBlocking)
            timerInfo->InternalCompletionEvent.CloseEvent();
        return FALSE;
    }

    if (isBlocking)
    {
        _ASSERTE(timerInfo->ExternalEventSafeHandle == NULL);
        _ASSERTE(timerInfo->ExternalCompletionEvent == INVALID_HANDLE);
        _ASSERTE(GetThread() != pTimerThread);

        timerInfo->InternalCompletionEvent.Wait(INFINITE,TRUE /*alertable*/);
        timerInfo->InternalCompletionEvent.CloseEvent();
        // Release handles and delete TimerInfo
        _ASSERTE(timerInfo->refCount == 0);
        // if WAIT_INTERNAL_COMPLETION flag is not set, timerInfo will be deleted in DeleteTimer.
        timerInfo->flag &= ~WAIT_INTERNAL_COMPLETION;
        DeleteTimer(timerInfo);
    }
    return status;
}

void ThreadpoolMgr::DeregisterTimer(TimerInfo* pArgs)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    TimerInfo* timerInfo = (TimerInfo*) pArgs;

    if (! (timerInfo->state & TIMER_REGISTERED) )
    {
        // set state to deleted, so that it does not get registered
        timerInfo->state |= TIMER_DELETE ;

        // since the timer has not even been registered, we dont need an interlock to decrease the RefCount
        timerInfo->refCount--;

        return;
    }

    if (timerInfo->state & TIMER_ACTIVE)
    {
        DeactivateTimer(timerInfo);
    }

    THREADPOOL_LOG0(TPL_PRIV | TPL_LOG, LL_INFO1000, "Timer deregistered\n");

    if (InterlockedDecrement(&timerInfo->refCount) == 0 )
    {
        DeleteTimer(timerInfo);
    }
    return;
}


#endif // !DACCESS_COMPILE
