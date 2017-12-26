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
#include "dbginterface.h"

#ifdef GC_SMP
#include "gcsmppriv.h"
#else
#include "gcpriv.h"
#endif // GC_SMP

#include "remoting.h"
#include "comsynchronizable.h"
#include "comsystem.h"

#include "syncclean.hpp"


#include "corhost.h"

#include "eepolicy.h"

// The contract between GC and the EE, for starting and finishing a GC is as follows:
//
//  SuspendEE:
//      LockThreadStore
//      SetGCInProgress
//      SysSuspendForGCE
//
//      ... perform the GC ...
//
// RestartEE:
//      SetGCDone
//      SysResumeFromGC
//         calls UnlockThreadStore
//
// Note that this is intentionally *not* symmetrical.  The EE will assert that the
// GC does most of this stuff in the correct sequence.

// sets up vars for GC

#ifndef DACCESS_COMPILE

COUNTER_ONLY(PERF_COUNTER_TIMER_PRECISION g_TotalTimeInGC = 0);
COUNTER_ONLY(PERF_COUNTER_TIMER_PRECISION g_TotalTimeSinceLastGCEnd = 0);

void GCHeap::UpdatePreGCCounters()
{

#if defined(ENABLE_PERF_COUNTERS)
    size_t allocation_0 = 0;
    size_t allocation_3 = 0; 
    
    // Publish perf stats
    g_TotalTimeInGC = GET_CYCLE_COUNT();

    gc_heap* hp = pGenGCHeap;
    allocation_0 = 
        dd_desired_allocation (hp->dynamic_data_of (0))-
        dd_new_allocation (hp->dynamic_data_of (0));
    allocation_3 = 
        dd_desired_allocation (hp->dynamic_data_of (max_generation+1))-
        dd_new_allocation (hp->dynamic_data_of (max_generation+1));
        

    GetPrivatePerfCounters().m_GC.cbAlloc += allocation_0;
    GetPrivatePerfCounters().m_GC.cbAlloc += allocation_3;
    GetPrivatePerfCounters().m_GC.cbLargeAlloc += allocation_3;
    GetPrivatePerfCounters().m_GC.cPinnedObj = 0;

    // The following two counters are not a part of the memory object
    // They are reset here due to the lack of a heartbeat mechanism in the CLR
    // We use GCs as a hearbeat, since if the app is not doing gc maybe the perf of it
    // is not interesting?
    GetPrivatePerfCounters().m_Jit.timeInJit = 0;
    GetPrivatePerfCounters().m_Jit.timeInJitBase = 1; // To avoid divide by zero
    GetPrivatePerfCounters().m_Security.timeRTchecks = 0;
    GetPrivatePerfCounters().m_Security.timeRTchecksBase = 1; // To avoid divide by zero

#endif //ENABLE_PERF_COUNTERS

    if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
        ETW_IS_FLAG_ON(TRACE_FLAG_GC)) {
        ETW_GC_INFO Info;

        Info.GCStart.Count = GcCount;

#ifndef GC_SMP

        gc_heap* hp1 = pGenGCHeap;


        Info.GCStart.Reason = (ETW_GC_INFO::GC_REASON)((int)(hp1->settings.reason));
#else // GC_SMP
        Info.GCStart.Reason = (ETW_GC_INFO::GC_REASON)(-1);
#endif // GC_SMP

        g_pEtwTracer->EtwTraceEvent(&GCEventGuid,
                                    ETW_TYPE_GC_START_COLLECTION,
                                    &Info, sizeof(Info.GCStart));
    }
}   

void GCHeap::UpdatePostGCCounters()
{
    if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
        ETW_IS_FLAG_ON(TRACE_FLAG_GC)) {
        ETW_GC_INFO Info;

        gc_heap* hp1 = pGenGCHeap;

        Info.GCEnd.Depth = GcCondemnedGeneration;
        Info.GCEnd.Count = (ULONG)dd_collection_count (hp1->dynamic_data_of (0));
        g_pEtwTracer->EtwTraceEvent(&GCEventGuid,
                                    ETW_TYPE_GC_END_COLLECTION,
                                    &Info, sizeof(Info.GCEnd));
    }

#if defined(ENABLE_PERF_COUNTERS)
    // Publish Perf Data

    int xGen;
    ETW_GC_INFO HeapInfo;
    
    gc_heap* hp1 = pGenGCHeap;

    size_t promoted_finalization_mem = 0;

    for (xGen = 0; xGen < MAX_TRACKED_GENS; xGen++)
    {
        size_t gensize = 0;
        size_t promoted_mem = 0; 

        gensize = ((xGen == 0) ? 
                   dd_desired_allocation (hp1->dynamic_data_of (xGen)) :
                   hp1->generation_size(xGen));    
        if (xGen <= (int)GcCondemnedGeneration)
        {
            promoted_mem = dd_promoted_size (hp1->dynamic_data_of (xGen));
        }

        if (xGen == 0)
        {
            promoted_finalization_mem =  dd_freach_previous_promotion (hp1->dynamic_data_of (xGen));
        }


        if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
            ETW_IS_FLAG_ON(TRACE_FLAG_GC))
        {
            HeapInfo.HeapStats.GenInfo[xGen].GenerationSize = gensize;
            
        }


        if (xGen < (MAX_TRACKED_GENS - 1))
        {
            if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
                ETW_IS_FLAG_ON(TRACE_FLAG_GC))
            {
                HeapInfo.HeapStats.GenInfo[xGen].TotalPromotedSize = promoted_mem;
            }
            GetPrivatePerfCounters().m_GC.cbPromotedMem[xGen] = promoted_mem;
        }

        GetPrivatePerfCounters().m_GC.cGenHeapSize[xGen] = gensize;
        GetPrivatePerfCounters().m_GC.cGenCollections[xGen] =
            dd_collection_count (hp1->dynamic_data_of (xGen));
        
    }

    GetPrivatePerfCounters().m_GC.cbPromotedFinalizationMem = promoted_finalization_mem;
    HeapInfo.HeapStats.FinalizationPromotedSize = promoted_finalization_mem;
    HeapInfo.HeapStats.PinnedObjectCount = (ULONG)(GetPrivatePerfCounters().m_GC.cPinnedObj);
    HeapInfo.HeapStats.SinkBlockCount =  (ULONG)(GetPrivatePerfCounters().m_GC.cSinkBlocks);
    HeapInfo.HeapStats.GCHandleCount =  (ULONG)(GetPrivatePerfCounters().m_GC.cHandles);
        


    
    //Committed memory 
    {
        size_t committed_mem = 0;
        size_t reserved_mem = 0;
            gc_heap* hp2 = hp1;
            {
                heap_segment* seg = 
                    generation_start_segment (hp2->generation_of (max_generation));
                while (seg)
                {
                    committed_mem += heap_segment_committed (seg) - 
                        heap_segment_mem (seg);
                    reserved_mem += heap_segment_reserved (seg) - 
                        heap_segment_mem (seg);
                    seg = heap_segment_next (seg);
                }
                //same for large segments
                seg = 
                    generation_start_segment (hp2->generation_of (max_generation + 1));
                while (seg)
                {
                    committed_mem += heap_segment_committed (seg) - 
                        heap_segment_mem (seg);
                    reserved_mem += heap_segment_reserved (seg) - 
                        heap_segment_mem (seg);
                    seg = heap_segment_next (seg);
                }
        }

        GetPrivatePerfCounters().m_GC.cTotalCommittedBytes = 
            committed_mem;
        GetPrivatePerfCounters().m_GC.cTotalReservedBytes = 
            reserved_mem;

    }
            

    size_t gensize = hp1->generation_size (max_generation + 1);    
    if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
        ETW_IS_FLAG_ON(TRACE_FLAG_GC))
    {
        HeapInfo.HeapStats.GenInfo[max_generation + 1].GenerationSize = gensize;
        HeapInfo.HeapStats.FinalizationPromotedSize = GetFinalizablePromotedCount();
        HeapInfo.HeapStats.FinalizationPromotedCount = (ULONG)GetNumberFinalizableObjects();
        g_pEtwTracer->EtwTraceEvent(&GCEventGuid,
                                    ETW_TYPE_GC_HEAP,
                                    &HeapInfo, sizeof(HeapInfo.HeapStats));
    }

    GetPrivatePerfCounters().m_GC.cLrgObjSize = gensize;
    GetPrivatePerfCounters().m_GC.cSurviveFinalize = GetFinalizablePromotedCount();
    
    // Compute Time in GC
    PERF_COUNTER_TIMER_PRECISION _currentPerfCounterTimer = GET_CYCLE_COUNT();

    g_TotalTimeInGC = _currentPerfCounterTimer - g_TotalTimeInGC;
    PERF_COUNTER_TIMER_PRECISION _timeInGCBase = (_currentPerfCounterTimer - g_TotalTimeSinceLastGCEnd);

    if (_timeInGCBase < g_TotalTimeInGC)
        g_TotalTimeInGC = 0;        // isn't likely except on some SMP machines-- perhaps make sure that
                                    //  _timeInGCBase >= g_TotalTimeInGC by setting affinity in GET_CYCLE_COUNT
                                    
    while (_timeInGCBase > UINT_MAX) 
    {
        _timeInGCBase = _timeInGCBase >> 8;
        g_TotalTimeInGC = g_TotalTimeInGC >> 8;
    }

    // Update Total Time    
    GetPrivatePerfCounters().m_GC.timeInGC = (DWORD)g_TotalTimeInGC;
    GetPrivatePerfCounters().m_GC.timeInGCBase = (DWORD)_timeInGCBase;

    if (!GetPrivatePerfCounters().m_GC.cProcessID)
        GetPrivatePerfCounters().m_GC.cProcessID = (size_t)GetCurrentProcessId();
    
    g_TotalTimeSinceLastGCEnd = _currentPerfCounterTimer;

#endif //ENABLE_PERF_COUNTERS
}

void ProfScanRootsHelper(Object*& object, ScanContext *pSC, DWORD dwFlags)
{
#ifdef GC_PROFILING
    Object *pObj = object;
#ifdef INTERIOR_POINTERS
    if (dwFlags & GC_CALL_INTERIOR)
    {
        BYTE *o = (BYTE*)object;
        gc_heap* hp = gc_heap::heap_of (o
#ifdef _DEBUG
                                        , !(dwFlags & GC_CALL_INTERIOR)
#endif //_DEBUG
                                       );

        if ((o < hp->gc_low) || (o >= hp->gc_high))
        {
            return;
        }
        pObj = (Object*) hp->find_object(o, hp->gc_low);
    }
#endif //INTERIOR_POINTERS
    ScanRootsHelper(pObj, pSC, dwFlags);
#endif // GC_PROFILING
}

void GCProfileWalkHeap()
{
#if defined (GC_PROFILING)
    if (CORProfilerTrackGC())
    {
        ProfilingScanContext SC;

        // Ask the vm to go over all of the roots
        CNameSpace::GcScanRoots(&ProfScanRootsHelper, max_generation, max_generation, &SC);

        // The finalizer queue is also a source of roots
        SC.rootKind = COR_PRF_GC_ROOT_FINALIZER;
        pGenGCHeap->finalize_queue->GcScanRoots(&ScanRootsHelper, 0, &SC);


        // Handles are kept independent of wks/svr/concurrent builds
        SC.rootKind = COR_PRF_GC_ROOT_HANDLE;
        CNameSpace::GcScanHandlesForProfiler(max_generation, &SC);

        // Indicate that root scanning is over, so we can flush the buffered roots
        // to the profiler
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->EndRootReferences2(&SC.pHeapId);
        }
        gc_heap::walk_heap (&HeapWalkHelper, 0, max_generation, TRUE);

    }
#endif //GC_PROFILING
}

void GCHeap::RestartEE(BOOL bFinishedGC, BOOL SuspendSucceded)
{
#ifdef TIME_SUSPEND
    g_SuspendStatistics.StartRestart();
#endif //TIME_SUSPEND

    if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
        ETW_IS_FLAG_ON(TRACE_FLAG_GC))
    {
        g_pEtwTracer->EtwTraceEvent(&GCEventGuid,
                                    ETW_TYPE_GC_RESUME,
                                    NULL, 0);
    }

    SyncClean::CleanUp();

#ifdef PROFILING_SUPPORTED
    if (CORProfilerTrackSuspends())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RuntimeResumeStarted((ThreadID)GetThread());
    }
#endif // PROFILING_SUPPORTED

    Thread  *thread = NULL;
    while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
    {
        thread->PrepareForEERestart();
    }

    // SetGCDone();
    ClrFlsClearThreadType (ThreadType_DynamicSuspendEE);
    GcInProgress = FALSE;
    
    // Note: this is the last barrier that keeps managed threads
    // from entering cooperative mode. If the sequence changes,
    // you may have to change routine GCHeap::SafeToRestartManagedThreads
    // as well.
    ThreadStore::TrapReturningThreads(FALSE);
    GcThread    = 0;
    WaitForGCEvent->Set();
    _ASSERTE(ThreadStore::HoldingThreadStore());

    Thread::SysResumeFromGC(bFinishedGC, SuspendSucceded);

#ifdef TIME_SUSPEND
    g_SuspendStatistics.EndRestart();
#endif //TIME_SUSPEND
}

void GCHeap::SuspendEE(SUSPEND_REASON reason)
{
#ifdef TIME_SUSPEND
    g_SuspendStatistics.StartSuspend();
#endif //TIME_SUSPEND

    BOOL gcOnTransitions;

    if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
        ETW_IS_FLAG_ON(TRACE_FLAG_GC))
    {
        ETW_GC_INFO Info;
        Info.SuspendEE.Reason = reason;
        g_pEtwTracer->EtwTraceEvent(&GCEventGuid,
                                    ETW_TYPE_GC_SUSPEND,
                                    &Info, sizeof(Info.SuspendEE));
    }

    LOG((LF_SYNC, INFO3, "Suspending the runtime for reason %d\n", reason));

    // lock the thread store which could take us out of our must
    // complete
    // Need the thread store lock here.  We take this lock before the thread
    // lock to avoid a deadlock condition where another thread suspends this
    // thread while it holds the heap lock.  While the thread store lock is
    // held, threads cannot be suspended.
    gcOnTransitions = GC_ON_TRANSITIONS(FALSE);        // dont do GC for GCStress 3

    Thread* pCurThread = GetThread();

    // Note: we need to make sure to re-set m_GCThreadAttemptingSuspend when we retry
    // due to the debugger case below!
retry_for_debugger:
    
    // Set variable to indicate that this thread is preforming a true GC
    // This is needed to overcome deadlock in taking the ThreadStore lock
    if (reason == GCHeap::SUSPEND_FOR_GC || reason == GCHeap::SUSPEND_FOR_GC_PREP)
    {
        m_GCThreadAttemptingSuspend = pCurThread;

    }

#ifdef TIME_SUSPEND
    DWORD startAcquire = g_SuspendStatistics.GetTime();
#endif

    ThreadStore::LockThreadStore(reason);

#ifdef TIME_SUSPEND
    g_SuspendStatistics.acquireTSL.Accumulate(SuspendStatistics::GetElapsed(startAcquire,
                                                                            g_SuspendStatistics.GetTime()));
#endif

    if ( ThreadStore::s_hAbortEvtCache != NULL &&
        (reason == GCHeap::SUSPEND_FOR_GC || reason == GCHeap::SUSPEND_FOR_GC_PREP))
    {
        LOG((LF_SYNC, INFO3, "GC thread is backing out the suspend abort event.\n"));
        ThreadStore::s_hAbortEvt = NULL;

        LOG((LF_SYNC, INFO3, "GC thread is signalling the suspend abort event.\n"));
        ThreadStore::s_hAbortEvtCache->Set();
    }

    // Set variable to indicate that this thread is attempting the suspend because it
    // needs to perform a GC and, as such, it holds GC locks.
    if (reason == GCHeap::SUSPEND_FOR_GC || reason == GCHeap::SUSPEND_FOR_GC_PREP)
    {
        m_GCThreadAttemptingSuspend = NULL;
    }

    {
        // suspend for GC, set in progress after suspending
        // threads which have no must complete
        WaitForGCEvent->Reset();
        // SetGCInProgress();
        {
            GcThread = pCurThread;
            ThreadStore::TrapReturningThreads(TRUE);
            m_suspendReason = reason;

            GcInProgress = TRUE;
            MemoryBarrier();

            ClrFlsSetThreadType (ThreadType_DynamicSuspendEE);
        }

        HRESULT hr;
        {
            _ASSERTE(ThreadStore::HoldingThreadStore() || g_fProcessDetach);
            hr = Thread::SysSuspendForGC(reason);
            ASSERT( hr == S_OK || hr == ERROR_TIMEOUT);

#ifdef TIME_SUSPEND
            if (hr == ERROR_TIMEOUT)
                g_SuspendStatistics.cntCollideRetry++;
#endif
        }

        // If the debugging services are attached, then its possible
        // that there is a thread which appears to be stopped at a gc
        // safe point, but which really is not. If that is the case,
        // back off and try again.

        // If this is not the GC thread and another thread has triggered
        // a GC, then we may have bailed out of SysSuspendForGC, so we
        // must resume all of the threads and tell the GC that we are
        // at a safepoint - since this is the exact same behaviour
        // that the debugger needs, just use it's code.
        if ((hr == ERROR_TIMEOUT)
            || Thread::ThreadsAtUnsafePlaces()
#ifdef DEBUGGING_SUPPORTED
             || (CORDebuggerAttached() && 
                 g_pDebugInterface->ThreadsAtUnsafePlaces())
#endif // DEBUGGING_SUPPORTED
            )
        {
            // In this case, the debugger has stopped at least one
            // thread at an unsafe place.  The debugger will usually
            // have already requested that we stop.  If not, it will 
            // either do so shortly -- or resume the thread that is
            // at the unsafe place.
            //
            // Either way, we have to wait for the debugger to decide
            // what it wants to do.
            //
            // Note: we've still got the gc_lock lock held.

            LOG((LF_GCROOTS | LF_GC | LF_CORDB,
                 LL_INFO10,
                 "***** Giving up on current GC suspension due "
                 "to debugger or timeout *****\n"));            

            if (ThreadStore::s_hAbortEvtCache == NULL)
            {
                LOG((LF_SYNC, INFO3, "Creating suspend abort event.\n"));

                CLREvent * pEvent = NULL;

                EX_TRY 
                {
                    pEvent = new CLREvent();
                    pEvent->CreateManualEvent(FALSE);
                    ThreadStore::s_hAbortEvtCache = pEvent;
                }
                EX_CATCH
                {
                    // Bummer... couldn't init the abort event. Its a shame, but not fatal. We'll simply not use it
                    // on this iteration and try again next time.
                    if (pEvent) {
                        _ASSERTE(!pEvent->IsValid());
                        delete pEvent;
                    }
                }
                EX_END_CATCH(SwallowAllExceptions)
            }

            if (ThreadStore::s_hAbortEvtCache != NULL)
            {
                LOG((LF_SYNC, INFO3, "Using suspend abort event.\n"));
                ThreadStore::s_hAbortEvt = ThreadStore::s_hAbortEvtCache;
                ThreadStore::s_hAbortEvt->Reset();
            }
            
            // Mark that we're done with the gc, just like at the
            // end of this method.
            RestartEE(FALSE, FALSE);            
            
            LOG((LF_GCROOTS | LF_GC | LF_CORDB,
                 LL_INFO10, "The EE is free now...\n"));
            
            // Check if we're ready to go to suspend.
            if (pCurThread && pCurThread->CatchAtSafePoint())
            {
                _ASSERTE(pCurThread->PreemptiveGCDisabled());
                pCurThread->PulseGCMode();  // Go suspend myself.
            }
            else
            {
                __SwitchToThread (0); // Wait a little while, before retrying.
            }

            goto retry_for_debugger;
        }
    }
    GC_ON_TRANSITIONS(gcOnTransitions);

#ifdef TIME_SUSPEND
    g_SuspendStatistics.EndSuspend();
#endif //TIME_SUSPEND
}

void CallFinalizer(Object* obj)
{

    MethodTable     *pMT = obj->GetMethodTable();
    STRESS_LOG2(LF_GC, LL_INFO1000, "Finalizing object %p MT %pT\n", obj, pMT);
    LOG((LF_GC, LL_INFO1000, "Finalizing " LOG_OBJECT_CLASS(obj)));

    _ASSERTE(GetThread()->PreemptiveGCDisabled());
    // if we don't have a class, we can't call the finalizer
    // if the object has been marked run as finalizer run don't call either
    if (pMT)
    {
        if (!((obj->GetHeader()->GetBits()) & BIT_SBLK_FINALIZER_RUN))
        {
            if (pMT->IsContextful())
            {
                Object *proxy = OBJECTREFToObject(CRemotingServices::GetProxyFromObject(ObjectToOBJECTREF(obj)));

                _ASSERTE(proxy && "finalizing an object that was never wrapped?????");                
                if (proxy == NULL)
                {
                    // Quite possibly the app abruptly shutdown while a proxy
                    // was being setup for a contextful object. We will skip
                    // finalizing this object.
                    _ASSERTE (g_fEEShutDown);
                    return;
                }
                else
                {
                    // This saves us from the situation where an object gets GC-ed 
                    // after its Context. 
                    Object* stub = (Object *)proxy->GetPtrOffset(CTPMethodTable::GetOffsetOfStubData());
                    Context *pServerCtx = (Context *) stub->UnBox();
                    // Check if the context is valid             
                    if (!Context::ValidateContext(pServerCtx))
                    {
                        // Since the server context is gone (GC-ed)
                        // we will associate the server with the default 
                        // context for a good faith attempt to run 
                        // the finalizer
                        // We want to do this only if we are using RemotingProxy
                        // and not for other types of proxies (eg. SvcCompPrxy)
                        OBJECTREF orRP = ObjectToOBJECTREF(CRemotingServices::GetRealProxy(proxy));
                        if(CTPMethodTable::IsInstanceOfRemotingProxy(
                            orRP->GetMethodTable()))
                        {
                            *((Context **)stub->UnBox()) = (Context*) GetThread()->GetContext();
                        }
                    }
                    // call Finalize on the proxy of the server object.
                    obj = proxy;
                }
            }
            _ASSERTE(pMT->HasFinalizer());
            MethodTable::CallFinalizer(obj);
        }
        else
        {
            //reset the bit so the object can be put on the list 
            //with RegisterForFinalization
            obj->GetHeader()->ClrBit (BIT_SBLK_FINALIZER_RUN);
        }
    }
}

static char s_FinalizeObjectName[MAX_CLASSNAME_LENGTH+MAX_NAMESPACE_LENGTH+2];
static BOOL s_fSaveFinalizeObjectName = FALSE;

void  CallFinalizerHelper(Object* fobj)
{
    DefineFullyQualifiedNameForClassOnStack();
    strcat_s(s_FinalizeObjectName, COUNTOF(s_FinalizeObjectName),
             GetFullyQualifiedNameForClass(fobj->GetClass()));
}

void  CallFinalizer(Thread* FinalizerThread, 
                    Object* fobj)
{
    if (s_fSaveFinalizeObjectName) {
        CallFinalizerHelper(fobj);
    }
    CallFinalizer(fobj);
    if (s_fSaveFinalizeObjectName) {
        s_FinalizeObjectName[0] = '\0';
    }
    // we might want to do some extra work on the finalizer thread
    // check and do it
    if (FinalizerThread->HaveExtraWorkForFinalizer())
    {
        FinalizerThread->DoExtraWorkForFinalizer();
    }

    // if someone is trying to stop us, open the gates
    FinalizerThread->PulseGCMode();
}

struct FinalizeAllObjects_Args {
    OBJECTREF fobj;
    int bitToCheck;
};

static Object *FinalizeAllObjects(Object* fobj, int bitToCheck);

static void FinalizeAllObjects_Wrapper(void *ptr)
{
    FinalizeAllObjects_Args *args = (FinalizeAllObjects_Args *) ptr;
    _ASSERTE(args->fobj);
    Object *fobj = OBJECTREFToObject(args->fobj);
    args->fobj = NULL;      // don't want to do this guy again, if we take an exception here:
    args->fobj = ObjectToOBJECTREF(FinalizeAllObjects(fobj, args->bitToCheck));
}

// The following is inadequate when we have multiple Finalizer threads in some future release.
// Instead, we will have to store this in TLS or pass it through the call tree of finalization.
// It is used to tie together the base exception handling and the AppDomain transition exception
// handling for this thread.
static struct ManagedThreadCallState *pThreadTurnAround;

static Object *DoOneFinalization(Object* fobj, Thread* pThread,int bitToCheck,bool *pbTerminate)
{
    bool fTerminate=false;
    Object *pReturnObject = NULL;
    

    AppDomain* targetAppDomain = fobj->GetAppDomain();
    AppDomain* currentDomain = pThread->GetDomain();
    if (! targetAppDomain || ! targetAppDomain->CanThreadEnter(pThread))
    {
        // if can't get into domain to finalize it, then it must be agile so finalize in current domain
        targetAppDomain = currentDomain;
#if CHECK_APP_DOMAIN_LEAKS
        {
        // object must be agile if can't get into it's domain
        if (g_pConfig->AppDomainLeaks() && !fobj->TrySetAppDomainAgile(FALSE))   
            _ASSERTE(!"Found non-agile GC object which should have been finalized during app domain unload.");
        }
#endif
    }

    if (targetAppDomain == currentDomain)
    {
        if (!targetAppDomain->IsRudeUnload() ||
            fobj->GetMethodTable()->HasCriticalFinalizer())
        {
            if (CLRHosted())
            {
                g_ObjFinalizeStartTime = CLRGetTickCount64();
            }
            {
                ThreadLocaleHolder localeHolder;
                CallFinalizer(fobj);
            }
            if (CLRHosted())
            {
                g_ObjFinalizeStartTime = 0;
            }
            pThread->InternalReset(FALSE);
        }
    } 
    else 
    {
        if (! targetAppDomain->GetDefaultContext())
        {
            // can no longer enter domain becuase the handle containing the context has been
            // nuked so just bail. Should only get this if are at the stage of nuking the
            // handles in the domain if it's still open.
            _ASSERTE(targetAppDomain->IsUnloading() && targetAppDomain->ShouldHaveFinalization());
        }
        else if (!currentDomain->IsDefaultDomain())
        {
            // this means we are in some other domain, so need to return back out through the DoADCallback
            // and handle the object from there in another domain.
            pReturnObject = fobj;
            fTerminate = true;
        } 
        else
        {
            // otherwise call back to ourselves to process as many as we can in that other domain
            FinalizeAllObjects_Args args;
            args.fobj = ObjectToOBJECTREF(fobj);
            args.bitToCheck = bitToCheck;
            GCPROTECT_BEGIN(args.fobj);
            {
                ThreadLocaleHolder localeHolder;
                _ASSERTE(pThreadTurnAround != NULL);
                ManagedThreadBase::FinalizerAppDomain(targetAppDomain,
                                                      FinalizeAllObjects_Wrapper,
                                                      &args,
                                                      pThreadTurnAround);
            }
            pThread->InternalReset(FALSE);
            // process the object we got back or be done if we got back null
            pReturnObject = OBJECTREFToObject(args.fobj);
            GCPROTECT_END();
        }
    }        
        
    *pbTerminate = fTerminate;
    return pReturnObject;
}

static Object *FinalizeAllObjects(Object* fobj, int bitToCheck)
{
    unsigned int fcount = 0; 
    bool fTerminate = false;

    if (fobj == NULL)
    {
        if (AppDomain::HasWorkForFinalizerThread())
        {
            return NULL;
        }
        fobj = GCHeap::GetNextFinalizableObject();
    }

    Thread *pThread = GetThread();
    // Finalize everyone
    while (fobj)
    {
        if (fobj->GetHeader()->GetBits() & bitToCheck)
        {
            if (AppDomain::HasWorkForFinalizerThread())
            {
                return NULL;
            }
            fobj = GCHeap::GetNextFinalizableObject();
        }
        else
        {
            fcount++;
            fobj = DoOneFinalization(fobj, pThread, bitToCheck,&fTerminate);
            if (fTerminate)
            {
                break;
            }

            if (fobj == NULL)
            {
                if (AppDomain::HasWorkForFinalizerThread())
                {
                    return NULL;
                }
                fobj = GCHeap::GetNextFinalizableObject();
            }            
        }
    }
    if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
        ETW_IS_FLAG_ON(TRACE_FLAG_GC)) {
        ETW_GC_INFO Info;
        Info.GCFinalizers.Count = fcount;
        g_pEtwTracer->EtwTraceEvent(&GCEventGuid,
                                    ETW_TYPE_GC_FINALIZED,
                                    &Info, sizeof(Info.GCFinalizers));
    }
    
    return fobj;
}

BOOL GCHeap::IsGCInProgressHelper (BOOL bConsiderGCStart)
{
#ifndef GC_SMP
    return GcInProgress || (bConsiderGCStart? gc_heap::gc_started : FALSE);
#else
    return GcInProgress;
#endif
}

DWORD GCHeap::WaitUntilGCComplete(BOOL bConsiderGCStart)
{
#ifndef GC_SMP
    if (bConsiderGCStart)
    {
        if (gc_heap::gc_started)
        {
            gc_heap::wait_for_gc_done();
        }
    }
#endif

    DWORD dwWaitResult = NOERROR;

    if (GcInProgress) {
        ASSERT( WaitForGCEvent->IsValid() );
        ASSERT( GetThread() != GcThread );

#ifdef DETECT_DEADLOCK
        // wait for GC to complete
BlockAgain:
        dwWaitResult = WaitForGCEvent->Wait(DETECT_DEADLOCK_TIMEOUT, FALSE );

        if (dwWaitResult == WAIT_TIMEOUT) {
            //  Even in retail, stop in the debugger if available.  Ideally, the
            //  following would use DebugBreak, but debspew.h makes this a null
            //  macro in retail.  Note that in debug, we don't use the debspew.h
            //  macros because these take a critical section that may have been
            //  taken by a suspended thread.
            FreeBuildDebugBreak();
            goto BlockAgain;
        }

#else  //DETECT_DEADLOCK
        
        dwWaitResult = WaitForGCEvent->Wait(INFINITE, FALSE );
        
#endif //DETECT_DEADLOCK
    }

    return dwWaitResult;
}

#ifndef GC_SMP
HANDLE MHandles[2];
#endif // !GC_SMP

void WaitForFinalizerEvent (CLREvent *event)
{
    event->Wait(INFINITE, FALSE);
}



static BOOL s_FinalizerThreadOK = FALSE;



VOID GCHeap::FinalizerThreadWorker(void *args)
{
    SCAN_IGNORE_THROW;
    SCAN_IGNORE_TRIGGER;

    // This is used to stitch together the exception handling at the base of our thread with
    // any eventual transitions into different AppDomains for finalization.
    _ASSERTE(args != NULL);
    pThreadTurnAround = (ManagedThreadCallState *) args;

    BOOL bPriorityBoosted = FALSE;

    while (!fQuitFinalizer)
    {
        // Wait for work to do...

        _ASSERTE(FinalizerThread->PreemptiveGCDisabled());
#ifdef _DEBUG
        if (g_pConfig->FastGCStressLevel())
        {
            FinalizerThread->m_GCOnTransitionsOK = FALSE;
        }
#endif
        FinalizerThread->EnablePreemptiveGC();
#ifdef _DEBUG
        if (g_pConfig->FastGCStressLevel())
        {
            FinalizerThread->m_GCOnTransitionsOK = TRUE;
        }
#endif

        WaitForFinalizerEvent (GCHeap::hEventFinalizer);

        if (!bPriorityBoosted)
        {
            if (FinalizerThread->SetThreadPriority(THREAD_PRIORITY_HIGHEST))
                bPriorityBoosted = TRUE;
        }

        FinalizerThread->DisablePreemptiveGC();


#ifdef _DEBUG
        if (g_pConfig->GetGCStressLevel() > 1)
        {
            int last_gc_count;
            do
            {
                last_gc_count = gc_count;
                FinalizerThread->m_GCOnTransitionsOK = FALSE; 
                FinalizerThread->EnablePreemptiveGC();
                __SwitchToThread (0);
                FinalizerThread->DisablePreemptiveGC();             
                // If no GCs happended, then we assume we are quiescent
                FinalizerThread->m_GCOnTransitionsOK = TRUE; 
            } while (gc_count - last_gc_count > 0);
        }
#endif //_DEBUG

        // we might want to do some extra work on the finalizer thread
        // check and do it
        if (FinalizerThread->HaveExtraWorkForFinalizer())
        {
            FinalizerThread->DoExtraWorkForFinalizer();
        }
        LOG((LF_GC, LL_INFO100, "***** Calling Finalizers\n"));
        FastInterlockExchange ((LONG*)&g_FinalizerIsRunning, TRUE);
        AppDomain::EnableADUnloadWorkerForFinalizer();

        do
        {
            FinalizeAllObjects(NULL, 0);
            _ASSERTE(FinalizerThread->GetDomain()->IsDefaultDomain());

            if (AppDomain::HasWorkForFinalizerThread())
            {
                AppDomain::ProcessUnloadDomainEventOnFinalizeThread();                
            }
            else if (GCHeap::UnloadingAppDomain == NULL)
                break;
            else if (!FinalizeAppDomain(GCHeap::UnloadingAppDomain, GCHeap::fRunFinalizersOnUnload))
            {
                break;
            }
            // Now schedule any objects from an unloading app domain for finalization 
            // on the next pass (even if they are reachable.)
            // Note that it may take several passes to complete the unload, if new objects are created during
            // finalization.
        }
        while(TRUE);

        if (GCHeap::UnloadingAppDomain != NULL)
        {
            SyncBlockCache::GetSyncBlockCache()->CleanupSyncBlocksInAppDomain(GCHeap::UnloadingAppDomain);

            GCHeap::UnloadingAppDomain->SetFinalized(); // All finalizers have run except for FinalizableAndAgile objects
            GCHeap::UnloadingAppDomain = NULL;
        }

        FastInterlockExchange ((LONG*)&g_FinalizerIsRunning, FALSE);

        GCHeap::GetGCHeap()->SignalFinalizationDone(TRUE);
    }
}


// During shutdown, finalize all objects that haven't been run yet... whether reachable or not.
void GCHeap::FinalizeObjectsOnShutdown(LPVOID args)
{
    // This is used to stitch together the exception handling at the base of our thread with
    // any eventual transitions into different AppDomains for finalization.
    _ASSERTE(args != NULL);
    pThreadTurnAround = (ManagedThreadCallState *) args;

    FinalizeAllObjects(NULL, BIT_SBLK_FINALIZER_RUN);
}


DWORD __stdcall GCHeap::FinalizerThreadStart(void *args)
{
    ClrFlsSetThreadType (ThreadType_Finalizer);

    ASSERT(args == 0);
    ASSERT(GCHeap::hEventFinalizer->IsValid());

    SCAN_IGNORE_THROW;
    SCAN_IGNORE_TRIGGER;

    LOG((LF_GC, LL_INFO10, "Finalizer thread starting..."));

    _ASSERTE(FinalizerThread->GetDomain()->IsDefaultDomain());

    s_FinalizerThreadOK = FinalizerThread->HasStarted();

    _ASSERTE(s_FinalizerThreadOK);
    _ASSERTE(GetThread() == FinalizerThread);

    if (CLRTaskHosted())
    {
        GCHeap::GetGCHeap()->SignalFinalizationDone(TRUE);
        // SQL's scheduler may give finalizer thread a very small slice of CPU if finalizer thread
        // shares a scheduler with other tasks.  This can cause severe problem for finalizer thread.
        // To reduce pain here, we move finalizer thread off SQL's scheduler.
        // But SQL's scheduler does not support IHostTask::Alert on a task off scheduler, so we need
        // to return finalizer thread back to scheduler when we wait alertably.
        // FinalizerThread->LeaveRuntime((size_t)SetupThreadNoThrow);
    }

    // finalizer should always park in default domain

    if (s_FinalizerThreadOK)
    {
#ifdef _DEBUG       // The only purpose of this try/finally is to trigger an assertion
        EE_TRY_FOR_FINALLY
        {
#endif
            FinalizerThread->SetBackground(TRUE);

            while (!fQuitFinalizer)
            {
                // This will apply any policy for swallowing exceptions during normal
                // processing, without allowing the finalizer thread to disappear on us.
                ManagedThreadBase::FinalizerBase(FinalizerThreadWorker);

                // If we came out on an exception, then we probably lost the signal that
                // there are objects in the queue ready to finalize.  The safest thing is
                // to reenable finalization.
                if (!fQuitFinalizer)
                    GetGCHeap()->EnableFinalization();
            }

            // Tell shutdown thread we are done with finalizing dead objects.
            GCHeap::hEventFinalizerToShutDown->Set();

            // Wait for shutdown thread to signal us.
            FinalizerThread->EnablePreemptiveGC();
            GCHeap::hEventShutDownToFinalizer->Wait(INFINITE,FALSE);
            FinalizerThread->DisablePreemptiveGC();

            AppDomain::RaiseExitProcessEvent();

            GCHeap::hEventFinalizerToShutDown->Set();

            // Phase 1 ends.
            // Now wait for Phase 2 signal.

            // Wait for shutdown thread to signal us.
            FinalizerThread->EnablePreemptiveGC();
            GCHeap::hEventShutDownToFinalizer->Wait(INFINITE,FALSE);
            FinalizerThread->DisablePreemptiveGC();

            SetFinalizeQueueForShutdown (FALSE);

            // Finalize all registered objects during shutdown, even they are still reachable.
            // we have been asked to quit, so must be shutting down      
            _ASSERTE(g_fEEShutDown);
            _ASSERTE(FinalizerThread->PreemptiveGCDisabled());

            // This will apply any policy for swallowing exceptions during normal
            // processing, without allowing the finalizer thread to disappear on us.
            ManagedThreadBase::FinalizerBase(FinalizeObjectsOnShutdown);

            _ASSERTE(FinalizerThread->GetDomain()->IsDefaultDomain());

            // we might want to do some extra work on the finalizer thread
            // check and do it
            if (FinalizerThread->HaveExtraWorkForFinalizer())
            {
                FinalizerThread->DoExtraWorkForFinalizer();
            }

            GCHeap::hEventFinalizerToShutDown->Set();

            // Wait for shutdown thread to signal us.
            FinalizerThread->EnablePreemptiveGC();
            GCHeap::hEventShutDownToFinalizer->Wait(INFINITE,FALSE);
            FinalizerThread->DisablePreemptiveGC();


            GCHeap::hEventFinalizerToShutDown->Set();

#ifdef _DEBUG       // The only purpose of this try/finally is to trigger an assertion
        }
        EE_FINALLY
        {
            // We can have exception to reach here if policy tells us to 
            // let exception go on finalizer thread.
            //
            if (GOT_EXCEPTION() && SwallowUnhandledExceptions())
                _ASSERTE(!"Exception in the finalizer thread!");

        }
        EE_END_FINALLY;
#endif
    }
    // finalizer should always park in default domain
    _ASSERTE(GetThread()->GetDomain()->IsDefaultDomain());

    LOG((LF_GC, LL_INFO10, "Finalizer thread done."));

    // Enable pre-emptive GC before we leave so that anybody trying to suspend
    // us will not end up waiting forever. Don't do a DestroyThread because this
    // will happen soon when we tear down the thread store.
    FinalizerThread->EnablePreemptiveGC();

    // We do not want to tear Finalizer thread,
    // since doing so will cause OLE32 to CoUninitalize.
    while (1)
    {
        PAL_TRY
        {
            __SwitchToThread(INFINITE);
        }
        PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
        {
        }
        PAL_ENDTRY
    }

    return 0;
}

DWORD GCHeap::FinalizerThreadCreate()
{
    DWORD   dwRet = 0;

    SCAN_IGNORE_THROW;

    hEventFinalizerDone = new CLREvent();
    hEventFinalizerDone->CreateManualEvent(FALSE);
    hEventFinalizer = new CLREvent();
    hEventFinalizer->CreateAutoEvent(FALSE);
    hEventFinalizerToShutDown = new CLREvent();
    hEventFinalizerToShutDown->CreateAutoEvent(FALSE);
    hEventShutDownToFinalizer = new CLREvent();
    hEventShutDownToFinalizer->CreateAutoEvent(FALSE);

    _ASSERTE(FinalizerThread == 0);
    FinalizerThread = SetupUnstartedThread();
    if (FinalizerThread == 0) {
        return 0;
    }

    // We don't want the thread block disappearing under us -- even if the
    // actual thread terminates.
    FinalizerThread->IncExternalCount();

    if (FinalizerThread->CreateNewThread(0, &FinalizerThreadStart, NULL))
    {
        dwRet = FinalizerThread->StartThread();

        _ASSERTE(dwRet == 1);
        
        if (CLRTaskHosted()) {
            GCHeap::GetGCHeap()->FinalizerThreadWait(INFINITE);
            if (!s_FinalizerThreadOK)
            {
                dwRet = 0;
            }
        }
    }

    return dwRet;
}

void GCHeap::SignalFinalizationDone(BOOL fFinalizer)
{
    if (fFinalizer)
    {
        FastInterlockAnd((DWORD*)&g_FinalizerWaiterStatus, ~FWS_WaitInterrupt);
    }
    hEventFinalizerDone->Set();
}

// Wait for the finalizer thread to complete one pass.
void GCHeap::FinalizerThreadWait(DWORD timeout)
{
    ASSERT(hEventFinalizerDone->IsValid());
    ASSERT(hEventFinalizer->IsValid());
    ASSERT(FinalizerThread);

    // Can't call this from within a finalized method.
    if (!IsCurrentThreadFinalizer())
    {

        GCX_PREEMP();

        Thread *pThread = GetThread();
        BOOL fADUnloadHelper = (pThread && pThread->HasThreadStateNC(Thread::TSNC_ADUnloadHelper));

        ULONGLONG startTime = CLRGetTickCount64();
        ULONGLONG endTime;
        if (timeout == INFINITE)
        {
            endTime = MAXULONGLONG;
        }
        else
        {
            endTime = timeout + startTime;
        }

        while (TRUE)
        {
            hEventFinalizerDone->Reset();
            EnableFinalization();

            //----------------------------------------------------
            // Do appropriate wait and pump messages if necessary
            //----------------------------------------------------
            //WaitForSingleObject(hEventFinalizerDone, INFINITE);

            if (fADUnloadHelper)
            {
                timeout = GetEEPolicy()->GetTimeout(OPR_FinalizerRun);
            }

            DWORD status = hEventFinalizerDone->Wait(timeout,TRUE);
            if (status != WAIT_TIMEOUT && !(g_FinalizerWaiterStatus & FWS_WaitInterrupt))
            {
                return;
            }
            if (!fADUnloadHelper)
            {
                // recalculate timeout
                if (timeout != INFINITE)
                {
                    ULONGLONG curTime = CLRGetTickCount64();
                    if (curTime >= endTime)
                    {
                        return;
                    }
                    else
                    {
                        timeout = (DWORD)(endTime - curTime);
                    }
                }
            }
            else
            {
                if (status == WAIT_TIMEOUT)
                {
                    DWORD finalizeStartTime = GetObjFinalizeStartTime();
                    if (finalizeStartTime || AppDomain::HasWorkForFinalizerThread())
                    {
                        if (CLRGetTickCount64() >= finalizeStartTime+timeout)
                        {
                            GCX_COOP();
                            FinalizerThreadAbortOnTimeout();
                        }
                    }
                }
                if (endTime != MAXULONGLONG)
                {
                    ULONGLONG curTime = CLRGetTickCount64();
                    if (curTime >= endTime)
                    {
                        return;
                    }
                }
            }
        }
    }
}


#ifdef _DEBUG
#define FINALIZER_WAIT_TIMEOUT 250
#else
#define FINALIZER_WAIT_TIMEOUT 200
#endif
#define FINALIZER_TOTAL_WAIT 2000

static BOOL s_fRaiseExitProcessEvent = FALSE;
static DWORD dwBreakOnFinalizeTimeOut = (DWORD) -1;

void GCHeap::WaitUntilConcurrentGCComplete()
{
}

BOOL GCHeap::IsConcurrentGCInProgress()
{
    return FALSE;
}


static ULONGLONG ShutdownEnd;

BOOL GCHeap::FinalizerThreadWatchDog()
{
    
    Thread *pThread = GetThread();

    if (dwBreakOnFinalizeTimeOut == (DWORD) -1) {
        dwBreakOnFinalizeTimeOut = g_pConfig->GetConfigDWORD(L"BreakOnFinalizeTimeOut", 0);
    }

    // Do not wait for FinalizerThread if the current one is FinalizerThread.
    if (pThread == FinalizerThread)
        return TRUE;

    // If finalizer thread is gone, just return.
    if (FinalizerThread->Join (0, FALSE) != WAIT_TIMEOUT)
        return TRUE;

    // *** This is the first call ShutDown -> Finalizer to Finilize dead objects ***
    if ((g_fEEShutDown & ShutDown_Finalize1) &&
        !(g_fEEShutDown & ShutDown_Finalize2)) {
        ShutdownEnd = CLRGetTickCount64() + GetEEPolicy()->GetTimeout(OPR_ProcessExit);
        // Wait for the finalizer...
        LOG((LF_GC, LL_INFO10, "Signalling finalizer to quit..."));

        fQuitFinalizer = TRUE;
        hEventFinalizerDone->Reset();
        EnableFinalization();

        LOG((LF_GC, LL_INFO10, "Waiting for finalizer to quit..."));
        
        if (pThread)
        {
        pThread->EnablePreemptiveGC();
        }
        BOOL fTimeOut = FinalizerThreadWatchDogHelper();
        
        if (!fTimeOut) {
            hEventShutDownToFinalizer->Set();

            // Wait for finalizer thread to finish raising ExitProcess Event.
            s_fRaiseExitProcessEvent = TRUE;
            fTimeOut = FinalizerThreadWatchDogHelper();
            s_fRaiseExitProcessEvent = FALSE;
        }
        
        if (pThread)
        {
        pThread->DisablePreemptiveGC();
        }
        
        // Can not call ExitProcess here if we are in a hosting environment.
        // The host does not expect that we terminate the process.
        //if (fTimeOut)
        //{
            //::ExitProcess (GetLatchedExitCode());
        //}
        
        return !fTimeOut;
    }

    // *** This is the second call ShutDown -> Finalizer to ***
    // suspend the Runtime and Finilize live objects
    if ( g_fEEShutDown & ShutDown_Finalize2 &&
        !(g_fEEShutDown & ShutDown_COM) ) {
        

        _ASSERTE ((g_fEEShutDown & ShutDown_Finalize1) || g_fFastExitProcess);
        SuspendEE(GCHeap::SUSPEND_FOR_SHUTDOWN);

        g_fSuspendOnShutdown = TRUE;
        
        // Do not balance the trap returning threads.
        // We are shutting down CLR.  Only Finalizer/Shutdown threads can
        // return from DisablePreemptiveGC.
        ThreadStore::TrapReturningThreads(TRUE);

        RestartEE(FALSE, TRUE);

        if (g_fFastExitProcess)
        {
            return TRUE;
        }

        // !!! Before we wake up Finalizer thread, we need to enable preemptive gc on the
        // !!! shutdown thread.  Otherwise we may see a deadlock during debug test.
        if (pThread)
        {
            pThread->EnablePreemptiveGC();
        }
        
        g_fFinalizerRunOnShutDown = TRUE;
        
        // Wait for finalizer thread to finish finalizing all objects.
        GCHeap::hEventShutDownToFinalizer->Set();
        BOOL fTimeOut = FinalizerThreadWatchDogHelper();

        if (!fTimeOut) {
            g_fFinalizerRunOnShutDown = FALSE;
        }
        
        // Can not call ExitProcess here if we are in a hosting environment.
        // The host does not expect that we terminate the process.
        //if (fTimeOut) {
        //    ::ExitProcess (GetLatchedExitCode());
        //}

        if (pThread)
        {
        pThread->DisablePreemptiveGC();
        }
        return !fTimeOut;
    }

    // *** This is the third call ShutDown -> Finalizer ***
    // to do additional cleanup
    if (g_fEEShutDown & ShutDown_COM) {
        _ASSERTE (g_fEEShutDown & (ShutDown_Finalize2 | ShutDown_Finalize1));

        if (pThread)
        {
            pThread->EnablePreemptiveGC();
        }
        g_fFinalizerRunOnShutDown = TRUE;
        
        GCHeap::hEventShutDownToFinalizer->Set();
        DWORD status = WAIT_OBJECT_0;
        while (CLREventWaitWithTry(hEventFinalizerToShutDown, FINALIZER_WAIT_TIMEOUT, TRUE, &status))
        {
        }
        
        BOOL fTimeOut = (status == WAIT_TIMEOUT) ? TRUE : FALSE;

        if (fTimeOut) 
        {
            if (dwBreakOnFinalizeTimeOut) {
                LOG((LF_GC, LL_INFO10, "Finalizer took too long to clean up COM IP's.\n"));
                DebugBreak();
            }
        }

        if (pThread)
        {
            pThread->DisablePreemptiveGC();
        }

        return !fTimeOut;
    }

    _ASSERTE(!"Should never reach this point");
    return FALSE;
}

BOOL GCHeap::FinalizerThreadWatchDogHelper()
{
    // Since our thread is blocking waiting for the finalizer thread, we must be in preemptive GC
    // so that we don't in turn block the finalizer on us in a GC.
    Thread *pCurrentThread = GetThread();
    _ASSERTE (pCurrentThread == NULL || !pCurrentThread->PreemptiveGCDisabled());

    // We're monitoring the finalizer thread.
    Thread *pThread = GCHeap::GetGCHeap()->GetFinalizerThread(); 
    _ASSERTE(pThread != pCurrentThread);
    
    ULONGLONG dwBeginTickCount = CLRGetTickCount64();
    
    size_t prevCount;
    size_t curCount;
    BOOL fTimeOut = FALSE;
    DWORD nTry = 0;
    DWORD maxTotalWait = (DWORD)(ShutdownEnd - dwBeginTickCount);
    DWORD totalWaitTimeout;
    totalWaitTimeout = GetEEPolicy()->GetTimeout(OPR_FinalizerRun);
    if (totalWaitTimeout == (DWORD)-1)
    {
        totalWaitTimeout = FINALIZER_TOTAL_WAIT;
    }

    if (s_fRaiseExitProcessEvent)
    {
        DWORD tmp = maxTotalWait/20;  // Normally we assume 2 seconds timeout if total timeout is 40 seconds.
        if (tmp > totalWaitTimeout)
        {
            totalWaitTimeout = tmp;
        }
        prevCount = MAXLONG;
    }
    else
    {
        prevCount = GetNumberFinalizableObjects();
    }

    DWORD maxTry = (DWORD)(totalWaitTimeout*1.0/FINALIZER_WAIT_TIMEOUT + 0.5);
    BOOL bAlertable = TRUE; //(g_fEEShutDown & ShutDown_Finalize2) ? FALSE:TRUE;

    if (dwBreakOnFinalizeTimeOut == (DWORD) -1) {
        dwBreakOnFinalizeTimeOut = g_pConfig->GetConfigDWORD(L"BreakOnFinalizeTimeOut", 0);
    }

    DWORD dwTimeout = FINALIZER_WAIT_TIMEOUT;

    // This used to set the dwTimeout to infinite, but this can cause a hang when shutting down
    // if a finalizer tries to take a lock that another suspended managed thread already has.
    // This results in the hang because the other managed thread is never going to be resumed
    // because we're in shutdown.  So we make a compromise here - make the timeout for every
    // iteration 10 times longer and make the total wait infinite - so if things hang we will
    // eventually shutdown but we also give things a chance to finish if they're running slower
    // because of the profiler.
#ifdef PROFILING_SUPPORTED
    if (CORProfilerPresent())
    {
        dwTimeout *= 10;
        maxTotalWait = INFINITE;
    }
#endif // PROFILING_SUPPORTED

    while (1) {
        DWORD status = 0;
        PAL_TRY
        {
            status = hEventFinalizerToShutDown->Wait(dwTimeout, bAlertable);
        }
        PAL_EXCEPT (EXCEPTION_EXECUTE_HANDLER)
        {
            status = WAIT_TIMEOUT;
        }
        PAL_ENDTRY

        if (status != WAIT_TIMEOUT) {
            break;
        }
        nTry ++;
        // ExitProcessEventCount is incremental
        // FinalizableObjects is decremental
        if (s_fRaiseExitProcessEvent)
        {
            curCount = MAXLONG - GetProcessedExitProcessEventCount();
        }
        else
        {
            curCount = GetNumberFinalizableObjects();
        }

        if ((prevCount <= curCount)
#if defined(_DEBUG) || defined(SYNCHRONIZATION_STATS)
            && gc_heap::gc_lock.lock == -1
#else
            && gc_heap::gc_lock == -1
#endif
            && (pThread == NULL || !(pThread->m_State & (Thread::TS_UserSuspendPending | Thread::TS_DebugSuspendPending)))){
            if (nTry == maxTry) {
                if (!s_fRaiseExitProcessEvent) {
                LOG((LF_GC, LL_INFO10, "Finalizer took too long on one object.\n"));
                }
                else
                    LOG((LF_GC, LL_INFO10, "Finalizer took too long to process ExitProcess event.\n"));

                fTimeOut = TRUE;
                if (dwBreakOnFinalizeTimeOut != 2) {
                    break;
                }
            }
        }
        else
        {
            nTry = 0;
            prevCount = curCount;
        }
        ULONGLONG dwCurTickCount = CLRGetTickCount64();
        if (pThread && pThread->m_State & (Thread::TS_UserSuspendPending | Thread::TS_DebugSuspendPending)) {
            dwBeginTickCount = dwCurTickCount;
        }
        if (dwCurTickCount - dwBeginTickCount >= maxTotalWait)
        {
            LOG((LF_GC, LL_INFO10, "Finalizer took too long on shutdown.\n"));
            fTimeOut = TRUE;
            if (dwBreakOnFinalizeTimeOut != 2) {
                break;
            }
        }
    }

    if (fTimeOut) 
    {
        if (dwBreakOnFinalizeTimeOut){
            DebugBreak();
        }
        if (!s_fRaiseExitProcessEvent && s_FinalizeObjectName[0] != '\0') {
            LOG((LF_GC, LL_INFO10, "Currently running finalizer on object of %s\n", 
                 s_FinalizeObjectName));
        }
    }
    return fTimeOut;
}

void gc_heap::user_thread_wait (CLREvent *event, BOOL no_mode_change)
{
    Thread* pCurThread = NULL;
    BOOL mode = FALSE;
    
    if (!no_mode_change)
    {
        pCurThread = GetThread();
        mode = pCurThread ? pCurThread->PreemptiveGCDisabled() : FALSE;
        if (mode)
        {
            pCurThread->EnablePreemptiveGC();
        }
    }

    event->Wait(INFINITE, FALSE);

    if (!no_mode_change && mode)
    {
        pCurThread->DisablePreemptiveGC();
    }
}


#if defined(GC_SMP)
#define CARDS_PER_BYTE 1  // GC Smp stores one card per byte
#else
#define CARDS_PER_BYTE 8
#endif

// DO NOT USE: instead use ErectWriteBarrier(dst, ref) (see gc.h); it uses fast helper for Win64
// This function sets the card table with the granularity of 1 byte, to avoid ghost updates
//    that could occur if multiple threads were trying to set different bits in the same card.
// The alternative would be to use FastInterlockedOr, but that is too slow.
void GCHeap::ErectWriteBarrier(OBJECTREF *dst, OBJECTREF ref)
{
    // if the dst is outside of the heap (unboxed value classes) then we
    //      simply exit
    if (((*(BYTE**)&dst) < g_lowest_address) || ((*(BYTE**)&dst) >= g_highest_address))
        return;
    
#ifdef _DEBUG
    updateGCShadow((Object**) dst, OBJECTREFToObject(ref));     // support debugging write barrier
#endif
    
    if((BYTE*) OBJECTREFToObject(ref) >= g_ephemeral_low && (BYTE*) OBJECTREFToObject(ref) < g_ephemeral_high)
    {
        size_t card = gcard_of((BYTE*)dst);
        BYTE* pCardByte = ((BYTE*) g_card_table) + card / CARDS_PER_BYTE;
        BYTE bitMask = (BYTE) (1 << (card % 8));
        if( !((*pCardByte) & bitMask) )
        {
            *pCardByte = 0xFF;
        }
    }
}        


/* These helpers basically replicate the ErectWriteBarrier code inline */

void GCHeap::CheckedWriteBarrierHelper(Object **dst, Object *ref)
{
    *dst = ref;

    // if the dst is outside of the heap (unboxed value classes) then we
    //      simply exit
    if (((*(BYTE**)&dst) < g_lowest_address) || ((*(BYTE**)&dst) >= g_highest_address))
        return;
    
#ifdef _DEBUG
    updateGCShadow(dst, ref);     // support debugging write barrier
#endif
    
    if((BYTE*) ref >= g_ephemeral_low && (BYTE*) ref < g_ephemeral_high)
    {
        size_t card = gcard_of((BYTE*)dst);
        BYTE* pCardByte = ((BYTE*) g_card_table) + card / CARDS_PER_BYTE;
        BYTE bitMask = (BYTE) (1 << (card % 8));
        if( !((*pCardByte) & bitMask) )
        {
            *pCardByte = 0xFF;
        }
    }
}

// Use this version when you know that the destination field location is in the gc heap

void GCHeap::WriteBarrierHelper(Object **dst, Object *ref)
{
    *dst = ref;

#ifdef _DEBUG
    updateGCShadow(dst, ref);     // support debugging write barrier
#endif
    
    if((BYTE*) ref >= g_ephemeral_low && (BYTE*) ref < g_ephemeral_high)
    {
        size_t card = gcard_of((BYTE*)dst);
        BYTE* pCardByte = ((BYTE*) g_card_table) + card / CARDS_PER_BYTE;
        BYTE bitMask = (BYTE) (1 << (card % 8));
        if( !((*pCardByte) & bitMask) )
        {
            *pCardByte = 0xFF;
        }
    }
}


/******************************************************************************/
::GCHeap* CreateGCHeap() {
    return new(nothrow) GCHeap();   // we return wks or svr 
}

void GCHeap::TraceGCSegments()
{
    heap_segment* seg = 0;
    {
        gc_heap* h = pGenGCHeap;

        for (seg = generation_start_segment (h->generation_of (max_generation)); seg != 0; seg = heap_segment_next(seg))
        {
            if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
                ETW_IS_FLAG_ON(TRACE_FLAG_GC)) {
                ETW_GC_INFO Info;
                Info.GCCreateSegment.Address = (size_t)heap_segment_mem(seg);
                Info.GCCreateSegment.Size = (size_t)(heap_segment_reserved (seg) - heap_segment_mem(seg));
                g_pEtwTracer->EtwTraceEvent(&GCEventGuid,
                                            ETW_TYPE_GC_CREATE_SEGMENT,
                                            &Info, sizeof(Info.GCCreateSegment));
            }
        }

        // large obj segments
        for (seg = generation_start_segment (h->generation_of (max_generation+1)); seg != 0; seg = heap_segment_next(seg))
        {
            if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
                ETW_IS_FLAG_ON(TRACE_FLAG_GC)) {
                ETW_GC_INFO Info;
                Info.GCCreateSegment.Address = (size_t)heap_segment_mem(seg);
                Info.GCCreateSegment.Size = (size_t)(heap_segment_reserved (seg) - heap_segment_mem(seg));
                g_pEtwTracer->EtwTraceEvent(&GCEventGuid,
                                            ETW_TYPE_GC_CREATE_SEGMENT,
                                            &Info, sizeof(Info.GCCreateSegment));
            }
        }
    }
}


#endif // !DACCESS_COMPILE












#ifdef GC_PROFILING
void GCHeap::DescrGenerationsToProfiler (gen_walk_fn fn, void *context)
{
#ifndef GC_SMP
    pGenGCHeap->descr_generations_to_profiler(fn, context);
#endif
}
#endif
