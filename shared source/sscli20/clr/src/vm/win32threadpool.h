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

    Win32ThreadPool.h

Abstract:

    This module is the header file for thread pools using Win32 APIs. 

Revision History:

--*/

#ifndef _WIN32THREADPOOL_H
#define _WIN32THREADPOOL_H

#include "delegateinfo.h"
#include "util.hpp"

typedef VOID (__stdcall *WAITORTIMERCALLBACK)(PVOID, BOOL);


#define MAX_WAITHANDLES 64

#define MAX_CACHED_EVENTS 40        // upper limit on number of wait events cached 

#define WAIT_REGISTERED     0x01
#define WAIT_ACTIVE         0x02
#define WAIT_DELETE         0x04

#define TIMER_REGISTERED    0x01
#define TIMER_ACTIVE        0x02
#define TIMER_DELETE        0x04

const int MaxLimitThreadsPerCPU=25;               //  upper limit on number of cp threads per CPU
const int MinLimitCPThreadsPerCPU=0;
const int MaxFreeCPThreadsPerCPU=2;                 //  upper limit on number of  free cp threads per CPU

const int CpuUtilizationHigh=95;                    // remove threads when above this
const int CpuUtilizationLow =80;                    // inject more threads if below this
const int CpuUtilizationVeryLow =20;                // start shrinking threadpool below this

#define CPU_UTILIZATION_SAMPLES 10                  // number of samples to average
#define LOW_RATE                    0.02            // 20/sec -- if rate lower than this, don't use injection logic
#define MEDIUM_RATE                 0.10            // 100/sec -- if rate lower, use low threshold
#define HIGH_RATE                   0.75            // 750/sec -- if rate lower, use medium threshold, if higher, use high
#define KEEP_THREAD_THRESHOLD_HIGH_RATE  1.02f     // if rate is greater than this factor, keep thread
#define KEEP_THREAD_THRESHOLD_MEDIUM_RATE  1.04f   // if rate is greater than this factor, keep thread
#define KEEP_THREAD_THRESHOLD_LOW_RATE  1.08f      // if rate is greater than this factor, keep thread
#define THREAD_BACKOFF_THRESHOLD    0.94            // if rate drops more than this, backoff on injection
#define INJECTION_SKIP_COUNT    2                   // how many samples to skip


#define FILETIME_TO_INT64(t) (*(__int64*)&(t))
#define MILLI_TO_100NANO(x)  (x * 10000)        // convert from milliseond to 100 nanosecond unit

/**
 * This type is supposed to be private to ThreadpoolMgr.
 * It's at global scope because Strike needs to be able to access its
 * definition.
 */
struct WorkRequest {
    WorkRequest*            next;
    LPTHREAD_START_ROUTINE  Function; 
    PVOID                   Context;

};

typedef DPTR(WorkRequest) PTR_WorkRequest;
typedef DPTR(class ThreadpoolMgr) PTR_ThreadpoolMgr;
class ThreadpoolMgr
{
    friend class ClrDataAccess;
    friend struct DelegateInfo;
    friend class ThreadPoolNative;
    friend class TimerNative;

public:

    // enumeration of different kinds of memory blocks that are recycled
    enum MemType
    {
        MEMTYPE_AsyncCallback   = 0,
        MEMTYPE_DelegateInfo    = 1,
        MEMTYPE_WorkRequest     = 2,
        MEMTYPE_PostRequest     = 3,        
        MEMTYPE_COUNT           = 4,
    };

    static BOOL Initialize();

    static BOOL SetMaxThreadsHelper(DWORD MaxWorkerThreads,
                                        DWORD MaxIOCompletionThreads);

    static BOOL SetMaxThreads(DWORD MaxWorkerThreads, 
                              DWORD MaxIOCompletionThreads);

    static BOOL GetMaxThreads(DWORD* MaxWorkerThreads, 
                              DWORD* MaxIOCompletionThreads);
    
    static BOOL SetMinThreads(DWORD MinWorkerThreads, 
                              DWORD MinIOCompletionThreads);
    
    static BOOL GetMinThreads(DWORD* MinWorkerThreads, 
                              DWORD* MinIOCompletionThreads);
 
    static BOOL GetAvailableThreads(DWORD* AvailableWorkerThreads, 
                                 DWORD* AvailableIOCompletionThreads);

    static BOOL QueueUserWorkItem(LPTHREAD_START_ROUTINE Function, 
                                  PVOID Context,
                                  ULONG Flags);

    static BOOL PostQueuedCompletionStatus(LPOVERLAPPED lpOverlapped,
                                  LPOVERLAPPED_COMPLETION_ROUTINE Function);

    inline static BOOL IsCompletionPortInitialized()
    {
        LEAF_CONTRACT;
        return GlobalCompletionPort != NULL;
    }

    static BOOL DrainCompletionPortQueue();

    static BOOL RegisterWaitForSingleObject(PHANDLE phNewWaitObject,
                                            HANDLE hWaitObject,
                                            WAITORTIMERCALLBACK Callback,
                                            PVOID Context,
                                            ULONG timeout,
                                            DWORD dwFlag);

    static BOOL UnregisterWaitEx(HANDLE hWaitObject,HANDLE CompletionEvent);
    static void WaitHandleCleanup(HANDLE hWaitObject);

    static BOOL BindIoCompletionCallback(HANDLE FileHandle,
                                            LPOVERLAPPED_COMPLETION_ROUTINE Function,
                                            ULONG Flags,
                                            DWORD& errorCode);

    static void WaitIOCompletionCallback(DWORD dwErrorCode,
                                            DWORD numBytesTransferred,
                                            LPOVERLAPPED lpOverlapped);

    static VOID CallbackForInitiateDrainageOfCompletionPortQueue(
        DWORD dwErrorCode,
        DWORD dwNumberOfBytesTransfered,
        LPOVERLAPPED lpOverlapped
    );

    static VOID CallbackForContinueDrainageOfCompletionPortQueue(
        DWORD dwErrorCode,
        DWORD dwNumberOfBytesTransfered,
        LPOVERLAPPED lpOverlapped
    );

    static BOOL CreateTimerQueueTimer(PHANDLE phNewTimer,
                                        WAITORTIMERCALLBACK Callback,
                                        PVOID Parameter,
                                        DWORD DueTime,
                                        DWORD Period,
                                        ULONG Flags);

    static BOOL ChangeTimerQueueTimer(HANDLE Timer,
                                      ULONG DueTime,
                                      ULONG Period);
    static BOOL DeleteTimerQueueTimer(HANDLE Timer,
                                      HANDLE CompletionEvent);

    static void RecycleMemory(LPVOID mem, enum MemType memType);

    static void FlushQueueOfTimerInfos();

    static BOOL HaveTimerInfosToFlush() { return TimerInfosToBeRecycled != NULL; }

private:

#ifndef DACCESS_COMPILE
    inline static WorkRequest* MakeWorkRequest(LPTHREAD_START_ROUTINE  function, PVOID context)
    {
        CONTRACTL
        {
            THROWS;     
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;;
        
        WorkRequest* wr = (WorkRequest*) GetRecycledMemory(MEMTYPE_WorkRequest);
        _ASSERTE(wr);
		if (NULL == wr)
			return NULL;
        wr->Function = function;
        wr->Context = context;
        wr->next = NULL;
        return wr;
    }

    struct PostRequest {
        LPOVERLAPPED_COMPLETION_ROUTINE Function;
        DWORD                           errorCode;
        DWORD                           numBytesTransferred;
        LPOVERLAPPED                    lpOverlapped;
    };


    inline static PostRequest* MakePostRequest(LPOVERLAPPED_COMPLETION_ROUTINE function, LPOVERLAPPED overlapped)
    {
        CONTRACTL
        {
            THROWS;     
            GC_NOTRIGGER;
            MODE_ANY;
        }
        CONTRACTL_END;;
        
        PostRequest* pr = (PostRequest*) GetRecycledMemory(MEMTYPE_PostRequest);
        _ASSERTE(pr);
		if (NULL == pr)
			return NULL;
        pr->Function = function;
        pr->errorCode = 0;
        pr->numBytesTransferred = 0;
        pr->lpOverlapped = overlapped;
        
        return pr;
    }

#endif // #ifndef DACCESS_COMPILE

    typedef struct {
        DWORD           numBytes;
        ULONG_PTR      *key;
        LPOVERLAPPED    pOverlapped;
        DWORD           errorCode;
    } QueuedStatus;

    typedef DPTR(struct _LIST_ENTRY)                        PTR_LIST_ENTRY;
    typedef struct _LIST_ENTRY {
        struct _LIST_ENTRY *Flink;
        struct _LIST_ENTRY *Blink;
    } LIST_ENTRY, *PLIST_ENTRY;    
    
    struct WaitInfo;

    typedef struct {
        HANDLE          threadHandle;
        DWORD           threadId;
        CLREvent        startEvent;
        LONG            NumWaitHandles;                 // number of wait objects registered to the thread <=64
        LONG            NumActiveWaits;                 // number of objects, thread is actually waiting on (this may be less than
                                                           // NumWaitHandles since the thread may not have activated some waits
        HANDLE          waitHandle[MAX_WAITHANDLES];    // array of wait handles (copied from waitInfo since 
                                                           // we need them to be contiguous)
        LIST_ENTRY      waitPointer[MAX_WAITHANDLES];   // array of doubly linked list of corresponding waitinfo 
    } ThreadCB;


    typedef struct {
        ULONG               startTime;          // time at which wait was started
                                                // endTime = startTime+timeout
        ULONG               remainingTime;      // endTime - currentTime
    } WaitTimerInfo;

    struct  WaitInfo {
        LIST_ENTRY          link;               // Win9x does not allow duplicate waithandles, so we need to
                                                // group all waits on a single waithandle using this linked list
        HANDLE              waitHandle;
        WAITORTIMERCALLBACK Callback;
        PVOID               Context;
        ULONG               timeout;                
        WaitTimerInfo       timer;              
        DWORD               flag;
        DWORD               state;
        ThreadCB*           threadCB;
        LONG                refCount;                // when this reaches 0, the waitInfo can be safely deleted
        CLREvent            PartialCompletionEvent;  // used to synchronize deactivation of a wait
        CLREvent            InternalCompletionEvent; // only one of InternalCompletion or ExternalCompletion is used
                                                     // but I cant make a union since CLREvent has a non-default constructor
        HANDLE              ExternalCompletionEvent; // they are signalled when all callbacks have completed (refCount=0)
        ADID                handleOwningAD;
        OBJECTHANDLE        ExternalEventSafeHandle;
        BOOL                bReleaseEventIfADUnloaded;

    } ;

    // structure used to maintain global information about wait threads. Protected by WaitThreadsCriticalSection
    typedef struct WaitThreadTag {
        LIST_ENTRY      link;
        ThreadCB*       threadCB;
    } WaitThreadInfo;


    struct AsyncCallback{   
        WaitInfo*   wait;
        BOOL        waitTimedOut;
    } ;

#ifndef DACCESS_COMPILE

    static VOID
    AcquireAsyncCallback(AsyncCallback *pAsyncCB)
    {
        LEAF_CONTRACT;
    }

    static VOID
    ReleaseAsyncCallback(AsyncCallback *pAsyncCB)
    {
        CONTRACTL
        {
            THROWS;     
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

            WaitInfo *waitInfo = pAsyncCB->wait;
            ThreadpoolMgr::RecycleMemory((LPVOID*)pAsyncCB, ThreadpoolMgr::MEMTYPE_AsyncCallback);
            
            // if this was a single execution, we now need to stop rooting registeredWaitHandle  
            // in a GC handle. This will cause the finalizer to pick it up and call the cleanup
            // routine.
            if ( (waitInfo->flag & WAIT_SINGLE_EXECUTION)  && (waitInfo->flag & WAIT_FREE_CONTEXT))
            {

                DelegateInfo* pDelegate = (DelegateInfo*) waitInfo->Context;

                _ASSERTE(pDelegate->m_registeredWaitHandle);

                {
                    GCX_COOP();
                    AppDomainFromIDHolder ad(pDelegate->m_appDomainId, TRUE);
                    if (!ad.IsUnloaded())
                        // if no domain then handle already gone or about to go.
                        StoreObjectInHandle(pDelegate->m_registeredWaitHandle, NULL);
                }
            }

            if (InterlockedDecrement(&waitInfo->refCount) == 0)
                ThreadpoolMgr::DeleteWait(waitInfo);

    }

    typedef Holder<AsyncCallback *, ThreadpoolMgr::AcquireAsyncCallback, ThreadpoolMgr::ReleaseAsyncCallback> AsyncCallbackHolder;
    inline static AsyncCallback* MakeAsyncCallback()
    {
        WRAPPER_CONTRACT;
        return (AsyncCallback*) GetRecycledMemory(MEMTYPE_AsyncCallback);
    }

    static VOID ReleaseInfo(OBJECTHANDLE& hndSafeHandle, 
        ADID& owningAD, 
        HANDLE hndNativeHandle,
        BOOL    bOwnNativeHandle)
    {
        CONTRACTL
        {
            NOTHROW;
            MODE_ANY;
            GC_TRIGGERS;
        }
        CONTRACTL_END

// Use of EX_TRY, GCPROTECT etc in the same function is causing prefast to complain about local variables with
// same name masking each other (#246). The error could not be suppressed with "#pragma PREFAST_SUPPRESS"

        CONTRACT_VIOLATION(ThrowsViolation); // Work around SCAN bug
        if (hndSafeHandle != NULL)
        {

            SAFEHANDLEREF refSH = NULL;
            BOOL bReleaseNativeHandle = TRUE;

            GCX_COOP();
            GCPROTECT_BEGIN(refSH);

            {
                EX_TRY
                {
                    ENTER_DOMAIN_ID(owningAD);
                    {
                        // Read the GC handle
                        refSH = (SAFEHANDLEREF) ObjectToOBJECTREF(ObjectFromHandle(hndSafeHandle));

                        // Destroy the GC handle
                        DestroyHandle(hndSafeHandle);

                        if (refSH != NULL)
                        {
                            // Release the safe handle
                            refSH->Release();
                            bReleaseNativeHandle = FALSE;
                        }
                    }
                    END_DOMAIN_TRANSITION;
                }
                EX_CATCH
                {
                }
                EX_END_CATCH(SwallowAllExceptions);
            }

            
            if (bOwnNativeHandle && bReleaseNativeHandle)
            {
                // The appdomain the safehandle belongs to, is gone. So we have no way to get to the safehandle and release it. 
                // This can cause us to leak the native handle the safehandle was holding on to (because we addref'ed but never
                // got a chance to release it). So we call CloseHandle on the native handle directly
                CloseHandle(hndNativeHandle);
            }

            GCPROTECT_END();
            
            hndSafeHandle = NULL;
            owningAD = (ADID) 0;
        }
    }

#endif // #ifndef DACCESS_COMPILE

    typedef struct {
        LIST_ENTRY  link;
        HANDLE      Handle;
    } WaitEvent ;

    // Timer 
    typedef struct {
        LIST_ENTRY  link;           // doubly linked list of timers
        ULONG FiringTime;           // TickCount of when to fire next
        PVOID Function;             // Function to call when timer fires
        PVOID Context;              // Context to pass to function when timer fires
        ULONG Period;
        DWORD flag;                 // How do we deal with the context
        DWORD state;
        LONG refCount;
        HANDLE ExternalCompletionEvent;     // only one of this is used, but cant do a union since CLREvent has a non-default constructor
        CLREvent InternalCompletionEvent;   // flags indicates which one is being used
        OBJECTHANDLE    ExternalEventSafeHandle;
        ADID    handleOwningAD;
        BOOL    bReleaseEventIfADUnloaded;
    } TimerInfo;

    static VOID AcquireWaitInfo(WaitInfo *pInfo)
    {
    }
    static VOID ReleaseWaitInfo(WaitInfo *pInfo)
    {
        WRAPPER_CONTRACT;
#ifndef DACCESS_COMPILE
        ReleaseInfo(pInfo->ExternalEventSafeHandle, 
        pInfo->handleOwningAD, 
        pInfo->ExternalCompletionEvent,
        pInfo->bReleaseEventIfADUnloaded);
#endif
    }
    static VOID AcquireTimerInfo(TimerInfo *pInfo)
    {
    }
    static VOID ReleaseTimerInfo(TimerInfo *pInfo)
    {
        WRAPPER_CONTRACT;
#ifndef DACCESS_COMPILE
        ReleaseInfo(pInfo->ExternalEventSafeHandle, 
        pInfo->handleOwningAD, 
        pInfo->ExternalCompletionEvent,
        pInfo->bReleaseEventIfADUnloaded);
#endif
    }

    typedef Holder<WaitInfo *, ThreadpoolMgr::AcquireWaitInfo, ThreadpoolMgr::ReleaseWaitInfo> WaitInfoHolder;
    typedef Holder<TimerInfo *, ThreadpoolMgr::AcquireTimerInfo, ThreadpoolMgr::ReleaseTimerInfo> TimerInfoHolder;

    typedef struct {
        TimerInfo* Timer;           // timer to be updated
        ULONG DueTime ;             // new due time
        ULONG Period ;              // new period
    } TimerUpdateInfo;

    // Definitions and data structures to support recycling of high-frequency 
    // memory blocks. We use a spin-lock to access the list

#ifndef DACCESS_COMPILE

    class RecycledListInfo
    {
        static const unsigned int MaxCachedEntries = 40;

        struct Entry
	    {
	        Entry* next;
	    };

        volatile LONG lock;   		// this is the spin lock
        DWORD         count;  		// count of number of elements in the list
        Entry*        root;   		// ptr to first element of recycled list
		DWORD         filler;       // Pad the structure to a multiple of the 16.

		//--//

public:
		RecycledListInfo()
		{
            LEAF_CONTRACT;

            lock  = 0;
            root  = NULL;
            count = 0;
		}

		FORCEINLINE bool CanInsert()
		{
            LEAF_CONTRACT;

			return count < MaxCachedEntries;
		}

	    FORCEINLINE LPVOID Remove()
	    {
	    	LEAF_CONTRACT;

			if(root == NULL) return NULL; // No need for acquiring the lock, there's nothing to remove.

	        AcquireLock();

	        Entry* ret = (Entry*)root;

	        if(ret)
	        {
	            root   = ret->next;
	            count -= 1;
	        }

	        ReleaseLock();

	        return ret;
	    }

	    FORCEINLINE void Insert( LPVOID mem )
	    {
	    	LEAF_CONTRACT;

		    AcquireLock();

	        Entry* entry = (Entry*)mem;

	        entry->next = root;

	        root   = entry;
	        count += 1;

	        ReleaseLock();
	    }

	private:
	    FORCEINLINE void AcquireLock()
	    {
	    	LEAF_CONTRACT;

	        unsigned int rounds = 0;

	        while(lock != 0 || FastInterlockExchange( &lock, 1 ) != 0)
	        {
                YieldProcessor();           // indicate to the processor that we are spinning

	            rounds++;
	            
	            if((rounds % 32) == 0)
	            {
	                __SwitchToThread( 0 );
	            }
	        }
	    }

	    FORCEINLINE void ReleaseLock()
	    {
	    	LEAF_CONTRACT;

	    	lock = 0;
	    }
	};

    //
    // It's critical that we ensure these pointers are allocated by the linker away from
    // variables that are modified a lot at runtime.
    //
    // The use of the CacheGuard is a temporary solution,
    // the thread pool has to be refactor away from static variable and
    // toward a single global structure, where we can control the locality of variables.
    //
    class RecycledListsWrapper
    {
	    typedef ULONG (WINAPI *fpnGetCurrentProcessorNumber)(VOID);

        DWORD                        CacheGuardPre[64/sizeof(DWORD)];
        
        fpnGetCurrentProcessorNumber pGetCurrentProcessorNumber;
        RecycledListInfo* 		     pRecycledListPerProcessor;  // There are MEMTYPE_COUNT lists per processor.


        DWORD                        CacheGuardPost[64/sizeof(DWORD)];

    public:
        void Initialize( unsigned int numProcs );

        FORCEINLINE bool IsInitialized()
        {
            LEAF_CONTRACT;

            return pGetCurrentProcessorNumber != NULL;
        }
        
    	FORCEINLINE RecycledListInfo& GetRecycleMemoryInfo( enum MemType memType )
        {
            LEAF_CONTRACT;

        	return pRecycledListPerProcessor[pGetCurrentProcessorNumber() * MEMTYPE_COUNT + memType];
    	}

        static ULONG WINAPI FallbackGetCurrentProcessorNumber();
    };

#endif

// The state transitions are: 000->001  when a work item is queued or a io thread begins executing a callback 
//                            x01->x11  when the last worker thread dies
//                            0x1->1x1  when there's only one io thread that has been inactive for a while
//                            111->000  timeout after state 111 is reached
#define GATE_THREAD_STATUS_NOTCREATED        0
#define GATE_THREAD_STATUS_CREATED           1
#define GATE_THREAD_STATUS_NOWORKERTHREAD    2
#define GATE_THREAD_STATUS_NOCPTHREAD        4
#define GATE_THREAD_STATUS_MASK              6      /*  (GATE_THREAD_STATUS_NOWORKERTHREAD | GATE_THREAD_STATUS_NOCPTHREAD) */ 

    // Private methods
    static BOOL ShouldGrowWorkerThreadPool();

    static DWORD __stdcall intermediateThreadProc(PVOID arg);

    typedef struct {
        LPTHREAD_START_ROUTINE  lpThreadFunction;
        PVOID                   lpArg;        
    } intermediateThreadParam;

    static Thread* CreateUnimpersonatedThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpArgs, BOOL *pIsCLRThread);

    static BOOL CreateWorkerThread();

    static LONG EnqueueWorkRequest(WorkRequest* wr);

    static WorkRequest* DequeueWorkRequest();

    static void ExecuteWorkRequest(WorkRequest* workRequest);

#ifndef DACCESS_COMPILE

    static inline void IncNumQueuedWorkRequests()
	{
	    LEAF_CONTRACT;
	    
		NumQueuedWorkRequests++;
	}

	static inline void DecNumQueuedWorkRequests()
	{
	   LEAF_CONTRACT;
	   
		NumQueuedWorkRequests--;
	}

 
    inline static void AppendWorkRequest(WorkRequest* entry)
    {
        LEAF_CONTRACT;
        if (WorkRequestTail)
        {
            _ASSERTE(WorkRequestHead != NULL && NumQueuedWorkRequests >= 0);
            WorkRequestTail->next = entry;
        }
        else
        {
            _ASSERTE(WorkRequestHead == NULL && NumQueuedWorkRequests == 0);
            WorkRequestHead = entry;
        }

        WorkRequestTail = entry;
        _ASSERTE(WorkRequestTail->next == NULL);

        IncNumQueuedWorkRequests();
    }

    inline static WorkRequest* RemoveWorkRequest()
    {
        LEAF_CONTRACT;
        WorkRequest* entry = NULL;
        if (WorkRequestHead)
        {
            entry = WorkRequestHead;
            WorkRequestHead = entry->next;
            if (WorkRequestHead == NULL)
                WorkRequestTail = NULL;
            _ASSERTE(NumQueuedWorkRequests > 0);
            DecNumQueuedWorkRequests();
        }
        return entry;
    }

    static void EnsureInitialized();
    static void InitPlatformVariables();

    inline static BOOL IsInitialized()
    {
        LEAF_CONTRACT;
        return Initialization == -1;
    }

    static BOOL EnterRetirement();
    static void GrowWorkerThreadPoolIfStarvationSimple();

    static BOOL SuspendProcessing();

    static DWORD SafeWait(CLREvent * ev, DWORD sleepTime, BOOL alertable);

    static DWORD __stdcall WorkerThreadStart(LPVOID lpArgs);

    static BOOL AddWaitRequest(HANDLE waitHandle, WaitInfo* waitInfo);


    static ThreadCB* FindWaitThread();              // returns a wait thread that can accomodate another wait request

    static BOOL CreateWaitThread();

    static void InsertNewWaitForSelf(WaitInfo* pArg);

    static int FindWaitIndex(const ThreadCB* threadCB, const HANDLE waitHandle);

    static DWORD MinimumRemainingWait(LIST_ENTRY* waitInfo, unsigned int numWaits);

    static void ProcessWaitCompletion( WaitInfo* waitInfo,
                                unsigned index,      // array index 
                                BOOL waitTimedOut);

    static DWORD __stdcall WaitThreadStart(LPVOID lpArgs);

    static DWORD __stdcall AsyncCallbackCompletion(PVOID pArgs);

    static void QueueTimerInfoForRelease(TimerInfo *pTimerInfo);

    static DWORD __stdcall QUWIPostCompletion(PVOID pArgs);

    static void DeactivateWait(WaitInfo* waitInfo);
    static void DeactivateNthWait(WaitInfo* waitInfo, DWORD index);

    static void DeleteWait(WaitInfo* waitInfo);


    inline static void ShiftWaitArray( ThreadCB* threadCB, 
                                       ULONG SrcIndex, 
                                       ULONG DestIndex, 
                                       ULONG count)
    {
        LEAF_CONTRACT;
        memcpy(&threadCB->waitHandle[DestIndex],
               &threadCB->waitHandle[SrcIndex],
               count * sizeof(HANDLE));
        memcpy(&threadCB->waitPointer[DestIndex],
               &threadCB->waitPointer[SrcIndex],
               count * sizeof(LIST_ENTRY));
    }

    static void DeregisterWait(WaitInfo* pArgs);

	static inline void IncNumIdleWorkerThreads()
	{
        LEAF_CONTRACT;
		NumIdleWorkerThreads++;
	}

	static inline void DecNumIdleWorkerThreads()
	{
        LEAF_CONTRACT;
		NumIdleWorkerThreads--;
	}
 

    static int GetCPUBusyTime_NT(PAL_IOCP_CPU_INFORMATION* pOldInfo);

    static BOOL CreateCompletionPortThread(LPVOID lpArgs);
    static DWORD __stdcall CompletionPortThreadStart(LPVOID lpArgs);
    static void GrowCompletionPortThreadpoolIfNeeded();
    static BOOL ShouldExitThread();
    static BOOL IsIoPending();

    static BOOL CreateGateThread(SSIZE_T threadTypeStatus);
    static void EnsureGateThreadCreated(SSIZE_T threadTypeStatus);
    static DWORD __stdcall GateThreadStart(LPVOID lpArgs);
    static BOOL SufficientDelaySinceLastSample(unsigned int LastThreadCreationTime, 
                                               unsigned NumThreads, // total number of threads of that type (worker or CP)
                                               double   throttleRate=0.0 // the delay is increased by this percentage for each extra thread
                                               );
    static BOOL SufficientDelaySinceLastDequeue();
    //static BOOL SufficientDelaySinceLastCompletion();

    static LPVOID   GetRecycledMemory(enum MemType memType);

    static DWORD __stdcall TimerThreadStart(LPVOID args);
    static void TimerThreadFire(); // helper method used by TimerThreadStart
    static void InsertNewTimer(TimerInfo* pArg);
    static DWORD FireTimers();
    static DWORD __stdcall AsyncTimerCallbackCompletion(PVOID pArgs);
    static void DeactivateTimer(TimerInfo* timerInfo);
    static void DeleteTimer(TimerInfo* timerInfo);
    static void UpdateTimer(TimerUpdateInfo* pArgs);

    static void DeregisterTimer(TimerInfo* pArgs);

#ifdef _DEBUG
    inline static DWORD GetTickCount()
    {
        LEAF_CONTRACT;
        return ::GetTickCount() + TickCountAdjustment;
    }
#endif 

#endif // #ifndef DACCESS_COMPILE
    // Private variables

    static LONG Initialization;                     // indicator of whether the threadpool is initialized.
    static LONG ThreadInSuspend;                    // indicates if a thread has suspended processing

    SVAL_DECL(int,NumWorkerThreads);                    // total number of worker threads created
    SVAL_DECL(int,MinLimitTotalWorkerThreads);          // same as MinLimitTotalCPThreads
    SVAL_DECL(DWORD,MaxLimitTotalWorkerThreads);          // same as MaxLimitTotalCPThreads
    SVAL_DECL(int,NumIdleWorkerThreads);                // Threads waiting for work

    static BOOL MonitorWorkRequestsQueue;           // indicator to gate thread to monitor progress of WorkRequestQueue to prevent starvation due to blocked worker threads

    SVAL_DECL(LONG,NumQueuedWorkRequests);               // number of queued work requests

    static int LastRecordedQueueLength;				// captured by GateThread, used on Win9x to detect thread starvation 
    static unsigned int LastDequeueTime;			// used to determine if work items are getting thread starved 
    static unsigned int LastCompletionTime;			// used to determine if last thread can be terminated 
    static unsigned int LastSuspendTime;            // used to determine if it's time for a thread to suspend
    static unsigned int LastWorkerStateTime;        // used to determine whether to create a thread or not
    static DWORD CompletedWorkRequests;
    static DWORD PriorCompletedWorkRequests;
    static float PriorWorkRequestRate;
    static unsigned int SequentialRateDecreaseCount;

    enum WorkerCreationState
    {
        WORKER_None = 0,                            // no activity during last state check
        WORKER_RateIncrease = 1,                    // thread added during last check
        WORKER_RateDecrease = 2,                    // marks worker thread for deletion
        WORKER_Starvation = 3,                      // starvation occurred during last state check
        WORKER_LowCPU = 4,                          // low cpu detected during last state check
        WORKER_PauseInjection = 5,                  // backoff on injection logic
    };
    static WorkerCreationState LastWorkerCreationState;

    SPTR_DECL(WorkRequest,WorkRequestHead);            // Head of work request queue
    SPTR_DECL(WorkRequest,WorkRequestTail);            // Head of work request queue

    //static unsigned int LastCpuSamplingTime;		// last time cpu utilization was sampled by gate thread
    static unsigned int LastWorkerThreadCreation;   // last time a worker thread was created
    static unsigned int LastCPThreadCreation;		// last time a completion port thread was created
    static unsigned int NumberOfProcessors;         // = NumberOfWorkerThreads - no. of blocked threads


    static CrstStatic WorkerCriticalSection;

    static CLREvent * WorkRequestNotification;
    static CLREvent * RetiredCPWakeupEvent;    
    static CLREvent * RetiredWorkerWakeupEvent;    
    
    static CrstStatic WaitThreadsCriticalSection;
    static LIST_ENTRY WaitThreadsHead;                  // queue of wait threads, each thread can handle upto 64 waits

    static CrstStatic EventCacheCriticalSection;
    static LIST_ENTRY EventCache;                      // queue of cached events
    static DWORD NumUnusedEvents;                      // number of events in cache


    static TimerInfo *TimerInfosToBeRecycled;    // list of delegate infos associated with deleted timers
    static CrstStatic TimerQueueCriticalSection;  // critical section to synchronize timer queue access
    SVAL_DECL(LIST_ENTRY,TimerQueue);                       // queue of timers
    SVAL_DECL(DWORD,NumTimers);                             // number of timers in queue
    static HANDLE TimerThread;                          // Currently we only have one timer thread
    static Thread*  pTimerThread;
    static DWORD LastTickCount;                         // the count just before timer thread goes to sleep

    static BOOL InitCompletionPortThreadpool;           // flag indicating whether completion port threadpool has been initialized
    static HANDLE GlobalCompletionPort;                 // used for binding io completions on file handles
    SVAL_DECL(LONG,NumCPThreads);                         // number of completion port threads

    SVAL_DECL(LONG,MaxLimitTotalCPThreads);               // = MaxLimitCPThreadsPerCPU * number of CPUS
    SVAL_DECL(LONG,CurrentLimitTotalCPThreads);           // current limit on total number of CP threads
    SVAL_DECL(LONG,MinLimitTotalCPThreads);               // = MinLimitCPThreadsPerCPU * number of CPUS

    static LONG   NumRetiredWorkerThreads;              // number of Worker threads in "retired" pool with pending io
    SVAL_DECL(LONG,NumFreeCPThreads);                     // number of cp threads waiting on the port
    SVAL_DECL(LONG,MaxFreeCPThreads);                     // = MaxFreeCPThreadsPerCPU * Number of CPUS
    SVAL_DECL(LONG,NumRetiredCPThreads);              // number of threads in "retired" pool with pending io
    static LONG   GateThreadCreated;                    // Set to 1 after the thread is created

    SVAL_DECL(long,cpuUtilization);
    static long cpuUtilizationAverage;

#ifndef DACCESS_COMPILE
    static RecycledListsWrapper RecycledLists;
#endif

#ifdef _DEBUG
    static DWORD   TickCountAdjustment;                 // add this value to value returned by GetTickCount
#endif

    static int offset_counter;
    static const int offset_multiplier = 128;

};




#endif // _WIN32THREADPOOL_H
