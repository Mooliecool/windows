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

    gc.h

--*/

#ifndef __GC_H
#define __GC_H

#ifdef PROFILING_SUPPORTED
#define GC_PROFILING       //Turn on profiling
#endif // PROFILING_SUPPORTED

/*
 * Promotion Function Prototypes
 */
typedef void enum_func (Object*);

// callback functions for heap walkers
typedef void object_callback_func(void * pvContext, void * pvDataLoc);

// stub type to abstract a heap segment
struct gc_heap_segment_stub;
typedef gc_heap_segment_stub *segment_handle;

struct segment_info
{
    LPVOID pvMem; // base of the allocation, not the first object (must add ibFirstObject)
    size_t ibFirstObject;   // offset to the base of the first object in the segment
    size_t ibAllocated; // limit of allocated memory in the segment (>= firstobject)
    size_t ibCommit; // limit of committed memory in the segment (>= alllocated)
    size_t ibReserved; // limit of reserved memory in the segment (>= commit)
};

/* forward declerations */
class CObjectHeader;
class Object;


/* misc defines */
#define LARGE_OBJECT_SIZE   85000

#ifndef DACCESS_COMPILE
extern "C" {
#endif
GPTR_DECL(BYTE,g_lowest_address);
GPTR_DECL(BYTE,g_highest_address);
GPTR_DECL(DWORD,g_card_table);
#ifndef DACCESS_COMPILE
}
#endif


#ifdef _DEBUG
#define  _LOGALLOC
#endif



#define MP_LOCKS

extern "C" BYTE* g_ephemeral_low;
extern "C" BYTE* g_ephemeral_high;

namespace WKS {
    ::GCHeap* CreateGCHeap();
    class GCHeap;
    class gc_heap;
    }

#ifndef GC_SMP
namespace SVR {
    ::GCHeap* CreateGCHeap();
    class GCHeap;
    class gc_heap;
}
#endif // GC_SMP

/*
 * Ephemeral Garbage Collected Heap Interface
 */


struct alloc_context 
{
#ifndef GC_SMP
    friend class WKS::gc_heap;
    friend class SVR::gc_heap;
    friend class SVR::GCHeap;
#endif // GC_SMP    
    friend struct ClassDumpInfo;

    BYTE*          alloc_ptr;
    BYTE*          alloc_limit;
    __int64        alloc_bytes; //Number of bytes allocated by this context
#ifndef GC_SMP
    SVR::GCHeap*   alloc_heap;
    SVR::GCHeap*   home_heap;
#endif // GC_SMP
    int            alloc_count;
public:

    void init()
    {
        LEAF_CONTRACT;

        alloc_ptr = 0;
        alloc_limit = 0;
        alloc_bytes = 0;
#ifndef GC_SMP
        alloc_heap = 0;
        home_heap = 0;
#endif // GC_SMP
        alloc_count = 0;
    }
};

struct ScanContext
{
    Thread* thread_under_crawl;
    int thread_number;
    BOOL promotion; //TRUE: Promotion, FALSE: Relocation.
    BOOL concurrent; //TRUE: concurrent scanning 
#if CHECK_APP_DOMAIN_LEAKS
    AppDomain *pCurrentDomain;
#endif

#ifdef GC_PROFILING
    MethodDesc *pMD;
    COR_PRF_GC_ROOT_KIND rootKind;
#endif //GC_PROFILING
    
    ScanContext()
    {
        LEAF_CONTRACT;

        thread_under_crawl = 0;
        thread_number = -1;
        promotion = FALSE;
        concurrent = FALSE;
#ifdef GC_PROFILING
        pMD = NULL;
        rootKind = COR_PRF_GC_ROOT_OTHER;
#endif //GC_PROFILING
    }
};

#ifdef GC_PROFILING

struct ProfilingScanContext : ScanContext
{
    void *pHeapId;
    
    ProfilingScanContext() : ScanContext()
    {
        LEAF_CONTRACT;

        pHeapId = NULL;
    }
};

typedef BOOL (* walk_fn)(Object*, void*);
typedef void (* gen_walk_fn)(void *context, int generation, BYTE *range_start, BYTE * range_end, BYTE *range_reserved);

#endif //GC_PROFILING

//dynamic data interface
struct gc_counters
{
    size_t current_size;
    size_t promoted_size;
    size_t collection_count;
};

//constants for the flags parameter to the gc call back

#define GC_CALL_INTERIOR            0x1
#define GC_CALL_PINNED              0x2
#define GC_CALL_CHECK_APP_DOMAIN    0x4

//flags for GCHeap::Alloc(...)
#define GC_ALLOC_FINALIZE 0x1
#define GC_ALLOC_CONTAINS_REF 0x2

class GCHeap {
#ifdef DACCESS_COMPILE
    friend class ClrDataAccess;
#endif
    
public:

    static GCHeap *GetGCHeap()
    {
        LEAF_CONTRACT;

        _ASSERTE(g_pGCHeap != NULL);
        return g_pGCHeap;
    }
    
#ifndef DACCESS_COMPILE
    static void SetGCHeap(GCHeap * gcHeap)
    {
        LEAF_CONTRACT;

        g_pGCHeap = gcHeap;
    }
#endif //DACCESS_COMPILE
    
    static BOOL IsGCHeapInitialized()
    {
        LEAF_CONTRACT;

        return (g_pGCHeap != NULL);
    }
    static BOOL IsGCInProgress(BOOL bConsiderGCStart = FALSE)
    {
        WRAPPER_CONTRACT;

        return (IsGCHeapInitialized() ? GetGCHeap()->IsGCInProgressHelper(bConsiderGCStart) : false);
    }   

    static void WaitForGCCompletion(BOOL bConsiderGCStart = FALSE)
    {
        WRAPPER_CONTRACT;

        if (IsGCHeapInitialized())
            GetGCHeap()->WaitUntilGCComplete(bConsiderGCStart);
    }   

    // The runtime needs to know whether we're using workstation or server GC 
    // long before the GCHeap is created.  So IsServerHeap cannot be a virtual 
    // method on GCHeap.  Instead we make it a static method and initialize 
    // gcHeapType before any of the calls to IsServerHeap.  Note that this also 
    // has the advantage of getting the answer without an indirection
    // (virtual call), which is important for perf critical codepaths.

    #ifndef DACCESS_COMPILE
    static void InitializeHeapType(bool bServerHeap)
    {
        LEAF_CONTRACT;

        gcHeapType = bServerHeap ? GC_HEAP_SVR : GC_HEAP_WKS;
    }
    #endif
    
    static BOOL IsValidSegmentSize(size_t cbSize)
    {
        //Must be aligned on a Mb and greater than 4Mb
        return (((cbSize & (1024*1024-1)) ==0) && (cbSize >> 22) && (cbSize <= (1024*1024*1024)));
    }

    static BOOL IsValidGen0MaxSize(size_t cbSize)
    {
        return (cbSize >= 64*1024);
    }

    inline static bool IsServerHeap()
    {
        LEAF_CONTRACT;

        _ASSERTE(gcHeapType != GC_HEAP_INVALID);
        return (gcHeapType == GC_HEAP_SVR);
    }

    inline static bool UseAllocationContexts()
    {
        WRAPPER_CONTRACT;

        return ((IsServerHeap() ? true : (g_SystemInfo.dwNumberOfProcessors >= 2)));
    }

   inline static bool MarkShouldCompeteForStatics()
    {
        WRAPPER_CONTRACT;

        return IsServerHeap() && g_SystemInfo.dwNumberOfProcessors >= 2;
    }
    
    static GCHeap * CreateGCHeap()
    {
        WRAPPER_CONTRACT;

#ifndef GC_SMP
        return (IsServerHeap() ? SVR::CreateGCHeap() : WKS::CreateGCHeap());
#else
        return WKS::CreateGCHeap();
#endif // GC_SMP
    }

private:
    typedef enum
    {
        GC_HEAP_INVALID = 0,
        GC_HEAP_WKS     = 1,
        GC_HEAP_SVR     = 2
    } GC_HEAP_TYPE;
    
    SVAL_DECL(DWORD,gcHeapType);

public:
    typedef enum
    {
        SUSPEND_OTHER                   = 0,
        SUSPEND_FOR_GC                  = 1,
        SUSPEND_FOR_APPDOMAIN_SHUTDOWN  = 2,
        SUSPEND_FOR_CODE_PITCHING       = 3,
        SUSPEND_FOR_SHUTDOWN            = 4,
        SUSPEND_FOR_DEBUGGER            = 5,
        SUSPEND_FOR_GC_PREP             = 6
    } SUSPEND_REASON;


    virtual Thread* GetGCThread () = 0;
    virtual Thread* GetGCThreadAttemptingSuspend() = 0;
    virtual BOOL    IsGCInProgressHelper (BOOL bConsiderGCStart = FALSE) = 0;
    virtual DWORD    WaitUntilGCComplete (BOOL bConsiderGCStart = FALSE) = 0;
    virtual void SuspendEE(SUSPEND_REASON reason) = 0;
    virtual void RestartEE(BOOL bFinishedGC, BOOL SuspendSucceded) = 0;

    virtual Thread* GetFinalizerThread() = 0;
    virtual BOOL    IsCurrentThreadFinalizer() = 0;
    virtual BOOL    FinalizerThreadWatchDog() = 0;
    virtual void    FinalizerThreadWait(DWORD timeout = INFINITE) = 0;
    virtual void    EnableFinalization() = 0;
    // We wake up a wait for finaliation for two reasons:
    // if fFinalizer=TRUE, we have finished finalization.
    // if fFinalizer=FALSE, the timeout for finalization is changed, and AD unload helper thread is notified.
    virtual void    SignalFinalizationDone(BOOL fFinalizer) = 0;
    virtual void    SetFinalizationRun (Object* obj) = 0;
    virtual void    UnloadAppDomain( AppDomain *pDomain, BOOL fRunFinalizers ) = 0;
    virtual AppDomain*  GetUnloadingAppDomain() = 0;

    //wait for concurrent GC to finish
    virtual void WaitUntilConcurrentGCComplete () = 0;
    virtual BOOL IsConcurrentGCInProgress() = 0;
    

    virtual void FixAllocContext (alloc_context* acontext, BOOL lockp, void* arg, void *heap) = 0;
    virtual Object* Alloc (alloc_context* acontext, size_t size, DWORD flags) = 0;

    virtual HRESULT Initialize () = 0;


    virtual HRESULT GarbageCollect (int generation = -1, BOOL collectClasses=FALSE) = 0;
    virtual Object*  Alloc (size_t size, DWORD flags) = 0;
    virtual Object*  AllocLHeap (size_t size, DWORD flags) = 0;
    virtual BOOL     HandlePageFault(void*) = 0;
    virtual void     SetReservedVMLimit (size_t vmlimit) = 0;
    virtual void SetCardsAfterBulkCopy( Object**, size_t ) = 0;
    virtual void ErectWriteBarrier (OBJECTREF *location, OBJECTREF ref) = 0;
#ifdef GC_PROFILING
    virtual void WalkObject (Object* obj, walk_fn fn, void* context) = 0;
#endif

    virtual bool IsThreadUsingAllocationContextHeap(alloc_context ac, int thread_number) = 0;
    virtual int GetNumberOfHeaps () = 0; 
    virtual int GetHomeHeapNumber () = 0;
    
    virtual int CollectionCount (int generation) = 0;

        // Finalizer queue stuff (should stay)
    virtual void    RegisterForFinalization (int gen, Object* obj) = 0;

        // General queries to the GC
    virtual BOOL    IsPromoted (Object *object, ScanContext* sc) = 0;
    virtual unsigned WhichGeneration (Object* object) = 0;
    virtual BOOL    IsEphemeral (Object* object) = 0;

    virtual unsigned GetCondemnedGeneration() = 0;
    virtual BOOL IsObjectInFixedHeap(Object *pObj) = 0;
    virtual size_t  GetTotalBytesInUse () = 0;
    virtual unsigned GetGcCount() = 0;
    virtual void TraceGCSegments() = 0;

        // static if since restricting for all heaps is fine
    virtual size_t GetValidSegmentSize(BOOL large_seg = FALSE) = 0;

    static BOOL IsLargeObject(MethodTable *mt) { 
        WRAPPER_CONTRACT;

        return mt->GetBaseSize() >= LARGE_OBJECT_SIZE; 
    }
    
    static unsigned GetMaxGeneration() {
        LEAF_CONTRACT;  
        return max_generation;
    }

private:
    enum {
        max_generation  = 2,
    };
    
public:



        // debug support 
#ifdef STRESS_HEAP 
    //return TRUE if GC actually happens, otherwise FALSE
    virtual BOOL    StressHeap(alloc_context * acontext = 0) = 0;
#endif
#ifdef VERIFY_HEAP
    virtual BOOL    IsHeapPointer (void* object, BOOL small_heap_only = FALSE) = 0;
    virtual void    ValidateObjectMember (Object *obj) = 0;
#endif

#ifdef GC_PROFILING
    virtual void DescrGenerationsToProfiler (gen_walk_fn fn, void *context) = 0;
#endif

    virtual void CheckedWriteBarrierHelper(Object **dst, Object *ref) = 0;
    virtual void WriteBarrierHelper(Object **dst, Object *ref) = 0;

protected: 
    SPTR_DECL(Thread,GcThread);           // thread running GC
    SPTR_DECL(Thread,FinalizerThread);
#ifdef VERIFY_HEAP
public:
    // Return NULL if can't find next object. When EE is not suspended,
    // the result is not accurate: if the input arg is in gen0, the function could 
    // return zeroed out memory as next object
    virtual Object * NextObj (Object * object) = 0;
    // Return TRUE if object lives in frozen segment
    virtual BOOL IsInFrozenSegment (Object * object) = 0;
#endif //VERIFY_HEAP    
};

    extern volatile LONG m_GCLock;

// Go through and touch (read) each page straddled by a memory block.
void TouchPages(LPVOID pStart, UINT cb);

// For low memory notification from host
extern LONG g_bLowMemoryFromHost;

inline void ErectWriteBarrier(OBJECTREF* dst, OBJECTREF ref) { GCHeap::GetGCHeap()->ErectWriteBarrier(dst, ref); }

// the method table and offset for the WeakReference class
extern MethodTable  *pWeakReferenceMT;
extern const size_t  pWeakRefHandleFO;

#endif // __GC_H
