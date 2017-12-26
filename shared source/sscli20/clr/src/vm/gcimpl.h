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
#ifndef GCIMPL_H_
#define GCIMPL_H_



#define PER_HEAP static



#define PER_HEAP_ISOLATED static


inline void initGCShadow() {}
inline void deleteGCShadow() {}
inline void updateGCShadow(Object** ptr, Object* val) {}
inline void checkGCWriteBarrier() {}

class GCHeap;
class gc_heap;

class GCHeap : public ::GCHeap
{
protected:

    #define pGenGCHeap ((gc_heap*)0)
    
    friend class CFinalize;
    friend class gc_heap;
    friend struct alloc_context;
    friend void EnterAllocLock();
    friend void LeaveAllocLock();
    friend void STDMETHODCALLTYPE EEShutDown(BOOL fIsDllUnloading);
    friend void ProfScanRootsHelper(Object*& object, ScanContext *pSC, DWORD dwFlags);
    friend void GCProfileWalkHeap();

public:
    static void UpdatePreGCCounters();
    static void UpdatePostGCCounters();

public:
    GCHeap(){};
    ~GCHeap(){};

    /* BaseGCHeap Methods*/
    PER_HEAP_ISOLATED   HRESULT Shutdown ();

    size_t  GetTotalBytesInUse ();

    void  TraceGCSegments ();    
    
    BOOL    IsGCInProgressHelper (BOOL bConsiderGCStart = FALSE);

    Thread* GetGCThread ()       
    { return GcThread; };

    Thread* GetGCThreadAttemptingSuspend()
    {
        return m_GCThreadAttemptingSuspend;
    }

    DWORD    WaitUntilGCComplete (BOOL bConsiderGCStart = FALSE);

    HRESULT Initialize ();

    //flags can be GC_ALLOC_CONTAINS_REF GC_ALLOC_FINALIZE
    Object*  Alloc (size_t size, DWORD flags);
    Object*  AllocLHeap (size_t size, DWORD flags);
    Object* Alloc (alloc_context* acontext, size_t size, DWORD flags);

    void FixAllocContext (alloc_context* acontext,
                                            BOOL lockp, void* arg, void *heap);


    int GetHomeHeapNumber ();
    bool IsThreadUsingAllocationContextHeap(alloc_context ac, int thread_number);
    int GetNumberOfHeaps ();
	void HideAllocContext(alloc_context*);
	void RevealAllocContext(alloc_context*);
   
    static BOOL IsLargeObject(MethodTable *mt);

    BOOL IsObjectInFixedHeap(Object *pObj);

    HRESULT GarbageCollect (int generation = -1, 
                                        BOOL collectClasses=FALSE);
    HRESULT GarbageCollectPing (int generation = -1, 
                                        BOOL collectClasses=FALSE);

    // Drain the queue of objects waiting to be finalized.
    void    FinalizerThreadWait(DWORD timeout = INFINITE);

    void    SignalFinalizationDone(BOOL fFinalizer);

    ////
    // GC callback functions
    // Check if an argument is promoted (ONLY CALL DURING
    // THE PROMOTIONSGRANTED CALLBACK.)
    BOOL    IsPromoted (Object *object, 
                                             ScanContext* sc);

    int CollectionCount (int generation);

    // promote an object
    PER_HEAP_ISOLATED void    Promote (Object*& object, 
                                          ScanContext* sc,
                                          DWORD flags=0);

    // Find the relocation address for an object
	PER_HEAP_ISOLATED void    Relocate (Object*& object,
                                           ScanContext* sc, 
                                           DWORD flags=0);


    HRESULT Init (size_t heapSize);

    //Register an object for finalization
    void    RegisterForFinalization (int gen, Object* obj); 
    
    //Unregister an object for finalization
    void    SetFinalizationRun (Object* obj); 
    
    //returns the generation number of an object (not valid during relocation)
    unsigned WhichGeneration (Object* object);
    // returns TRUE is the object is ephemeral 
    BOOL    IsEphemeral (Object* object);
#ifdef VERIFY_HEAP
    BOOL    IsHeapPointer (void* object, BOOL small_heap_only = FALSE);
	void    ValidateObjectMember (Object *obj);
#endif //_DEBUG

    PER_HEAP    size_t  ApproxTotalBytesInUse(BOOL small_heap_only = FALSE);
    PER_HEAP    size_t  ApproxFreeBytes();

    BOOL    HandlePageFault(void*);	//TRUE handled, FALSE propagate

    unsigned GetCondemnedGeneration()
    { return GcCondemnedGeneration;}

    PER_HEAP_ISOLATED     unsigned GetMaxGeneration();
 
    //suspend all threads

    void SuspendEE(SUSPEND_REASON reason);

    void RestartEE(BOOL bFinishedGC, BOOL SuspendSucceded); //resume threads. 

    inline void SetSuspendReason(SUSPEND_REASON suspendReason)
    { m_suspendReason = suspendReason; }

    Thread* GetFinalizerThread();

        //  Returns TRUE if the current thread is the finalizer thread.
    BOOL    IsCurrentThreadFinalizer();

    // allow finalizer thread to run
    void    EnableFinalization();

    // Start unloading app domain
    void    UnloadAppDomain( AppDomain *pDomain, BOOL fRunFinalizers ) 
      { UnloadingAppDomain = pDomain; fRunFinalizersOnUnload = fRunFinalizers; }

    // Return current unloading app domain (NULL when unload is finished.)
    AppDomain*  GetUnloadingAppDomain() { return UnloadingAppDomain; }

    // Lock for allocation Public because of the 
    // fast allocation helper

    unsigned GetGcCount() { return GcCount; }

    PER_HEAP_ISOLATED HRESULT GetGcCounters(int gen, gc_counters* counters);

    size_t GetValidSegmentSize(BOOL large_seg = FALSE);

    static size_t GetValidGen0MaxSize(size_t seg_size);

    void SetReservedVMLimit (size_t vmlimit);

    PER_HEAP_ISOLATED Object* GetNextFinalizableObject();
    PER_HEAP_ISOLATED size_t GetNumberFinalizableObjects();
    PER_HEAP_ISOLATED size_t GetFinalizablePromotedCount();
    PER_HEAP_ISOLATED BOOL FinalizeAppDomain(AppDomain *pDomain, BOOL fRunFinalizers);
    PER_HEAP_ISOLATED void SetFinalizeQueueForShutdown(BOOL fHasLock);

	void SetCardsAfterBulkCopy( Object**, size_t);
	void ErectWriteBarrier(OBJECTREF *location, OBJECTREF ref);
#ifdef GC_PROFILING
	void WalkObject (Object* obj, walk_fn fn, void* context);
#endif

    void CheckedWriteBarrierHelper(Object **dst, Object *ref);
    void WriteBarrierHelper(Object **dst, Object *ref);

public:	// FIX 

    // Lock for finalization
    PER_HEAP_ISOLATED   
        volatile        LONG    m_GCFLock;

    PER_HEAP_ISOLATED   BOOL    GcCollectClasses;
    PER_HEAP_ISOLATED
        volatile        BOOL    GcInProgress;       // used for syncing w/GC
    PER_HEAP_ISOLATED
              SUSPEND_REASON    m_suspendReason;    // This contains the reason
                                                    // that the runtime was suspended
    PER_HEAP_ISOLATED   Thread* m_GCThreadAttemptingSuspend;
    PER_HEAP_ISOLATED   volatile unsigned GcCount;
    PER_HEAP_ISOLATED   unsigned GcCondemnedGeneration;

    
    // Use only for GC tracing.
    PER_HEAP    unsigned long GcDuration;

      size_t  GarbageCollectGeneration (unsigned int gen=0, 
                                                  BOOL collectClasses = FALSE);
    // Interface with gc_heap
    size_t  GarbageCollectTry (int generation, 
                                        BOOL collectClasses=FALSE);



    // Finalizer thread stuff.
    
    BOOL    FinalizerThreadWatchDog();
    void      WaitUntilConcurrentGCComplete ();
    BOOL     IsConcurrentGCInProgress();
  
    PER_HEAP_ISOLATED   DWORD   FinalizerThreadCreate();
    PER_HEAP_ISOLATED   BOOL    FinalizerThreadWatchDogHelper();
    PER_HEAP_ISOLATED   VOID    FinalizerThreadWorker(void *args);
    PER_HEAP_ISOLATED   VOID    FinalizeObjectsOnShutdown(void *args);
    PER_HEAP_ISOLATED   DWORD   __stdcall FinalizerThreadStart(void *args);
    PER_HEAP_ISOLATED   CLREvent *WaitForGCEvent;     // used for syncing w/GC
    PER_HEAP_ISOLATED   CLREvent *hEventFinalizer;
    PER_HEAP_ISOLATED   CLREvent *hEventFinalizerDone;
    PER_HEAP_ISOLATED   CLREvent *hEventFinalizerToShutDown;
    PER_HEAP_ISOLATED   CLREvent *hEventShutDownToFinalizer;
    PER_HEAP_ISOLATED   BOOL    fQuitFinalizer;
    PER_HEAP_ISOLATED   AppDomain *UnloadingAppDomain;
    PER_HEAP_ISOLATED   BOOL    fRunFinalizersOnUnload;

    PER_HEAP_ISOLATED    CFinalize* m_Finalize;

    PER_HEAP_ISOLATED   gc_heap* Getgc_heap();

private:
    static bool SafeToRestartManagedThreads()
    {
        // Note: this routine should return true when the last barrier
        // to threads returning to cooperative mode is down after gc.
        // In other words, if the sequence in GCHeap::RestartEE changes,
        // the condition here may have to change as well.
        return g_TrapReturningThreads == 0;
    }

#ifdef STRESS_HEAP 
public:
    //return TRUE if GC actually happens, otherwise FALSE
    BOOL    StressHeap(alloc_context * acontext = 0);
protected:

    // handles to hold the string objects that will force GC movement
    enum { NUM_HEAP_STRESS_OBJS = 8 };
    PER_HEAP OBJECTHANDLE m_StressObjs[NUM_HEAP_STRESS_OBJS];
    PER_HEAP int m_CurStressObj;
#endif  // STRESS_HEAP 

#ifdef GC_PROFILING
    virtual void DescrGenerationsToProfiler (gen_walk_fn fn, void *context);
#endif // GC_PROFILING

#ifdef VERIFY_HEAP
public:
    Object * NextObj (Object * object);
    BOOL IsInFrozenSegment (Object * object);
#endif //VERIFY_HEAP     
};

#endif  // GCIMPL_H_
