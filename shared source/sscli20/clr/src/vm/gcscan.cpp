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
/*
 * GCSCAN.CPP 
 *
 * GC Root Scanning
 */

#include "common.h"
#include "object.h"
#include "threads.h"
#include "eetwain.h"
#include "eeconfig.h"
#include "gcscan.h"
#include "gc.h"
#include "corhost.h"
#include "threads.h"
#include "fieldmarshaler.h"
#include "interoputil.h"
#include "constrainedexecutionregion.h"

#include "excep.h"

#include "eeprofinterfaces.inl"


// Disable the "initialization of static local vars is no thread safe" error
#ifdef _MSC_VER
#pragma warning(disable : 4640)
#endif


#define ProfileTrackArrayAlloc(orObject) \
            OBJECTREF objref = ObjectToOBJECTREF((Object*)orObject);\
            GCPROTECT_BEGIN(objref);\
            ProfilerObjectAllocatedCallback(objref, (TypeID) orObject->GetTypeHandle().AsPtr());\
            GCPROTECT_END();\
            orObject = (ArrayBase *) OBJECTREFToObject(objref);

//#define CATCH_GC  //catches exception during GC
#ifdef DACCESS_COMPILE
SVAL_IMPL_INIT(LONG, CNameSpace, m_GcStructuresInvalidCnt, 1);
#else //DACCESS_COMPILE
volatile LONG CNameSpace::m_GcStructuresInvalidCnt = 1;
#endif //DACCESS_COMPILE

BOOL CNameSpace::GetGcRuntimeStructuresValid ()
{
    _ASSERTE ((LONG)m_GcStructuresInvalidCnt >= 0);
    return (LONG)m_GcStructuresInvalidCnt == 0;
}

#ifdef DACCESS_COMPILE

void
CNameSpace::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    m_GcStructuresInvalidCnt.EnumMem();
}

#else 

inline alloc_context* GetThreadAllocContext()
{
    assert(GCHeap::UseAllocationContexts());

    return & GetThread()->m_alloc_context;
}


#ifdef MAXALLOC
AllocRequestManager* GetGCAllocManager()
{
    CHECK_LOCAL_STATIC_VAR(static AllocRequestManager allocManager(L"AllocMaxGC"));

    return &allocManager;
}
#endif


inline Object* Alloc(size_t size, BOOL bFinalize, BOOL bContainsPointers )
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
    } CONTRACTL_END;

#ifdef MAXALLOC
    if (! GetGCAllocManager()->CheckRequest(size))
        ThrowOutOfMemory();
#endif

#ifdef _DEBUG
    if (g_pConfig->ShouldInjectFault(INJECTFAULT_GCHEAP))
    {
        char *a = new char;
        delete a;
    }
#endif

    DWORD flags = ((bContainsPointers ? GC_ALLOC_CONTAINS_REF : 0) |
                   (bFinalize ? GC_ALLOC_FINALIZE : 0));

    Object *retVal = NULL;

    // We don't want to throw an SO during the GC, so make sure we have plenty
    // of stack before calling in.
    INTERIOR_STACK_PROBE_FOR(GetThread(), DEFAULT_ENTRY_PROBE_AMOUNT * 1.5);
    if (GCHeap::UseAllocationContexts())
        retVal = GCHeap::GetGCHeap()->Alloc(GetThreadAllocContext(), size, flags);
    else
        retVal = GCHeap::GetGCHeap()->Alloc(size, flags);
    END_INTERIOR_STACK_PROBE;
    return retVal;
}

inline Object* AllocLHeap(size_t size, BOOL bFinalize, BOOL bContainsPointers )
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative (don't assume large heap doesn't compact!)
    } CONTRACTL_END;

#ifdef MAXALLOC
    if (! GetGCAllocManager()->CheckRequest(size))
        ThrowOutOfMemory();
#endif

#ifdef _DEBUG
    if (g_pConfig->ShouldInjectFault(INJECTFAULT_GCHEAP))
    {
        char *a = new char;
        delete a;
    }
#endif

    DWORD flags = ((bContainsPointers ? GC_ALLOC_CONTAINS_REF : 0) |
                   (bFinalize ? GC_ALLOC_FINALIZE : 0));

    Object *retVal = NULL;

    // We don't want to throw an SO during the GC, so make sure we have plenty
    // of stack before calling in.
    INTERIOR_STACK_PROBE_FOR(GetThread(), DEFAULT_ENTRY_PROBE_AMOUNT * 1.5);
    retVal = GCHeap::GetGCHeap()->AllocLHeap(size, flags);
    END_INTERIOR_STACK_PROBE;
    return retVal;
}


#ifdef  _LOGALLOC
int g_iNumAllocs = 0;

bool ToLogOrNotToLog(size_t size, const char *typeName)
{
    WRAPPER_CONTRACT;

    g_iNumAllocs++;

    if (g_iNumAllocs > g_pConfig->AllocNumThreshold())
        return true;

    if (size > (size_t)g_pConfig->AllocSizeThreshold())
        return true;

    if (g_pConfig->ShouldLogAlloc(typeName))
        return true;

    return false;

}


inline void LogAlloc(size_t size, MethodTable *pMT, Object* object)
{
    WRAPPER_CONTRACT;

    CONSISTENCY_CHECK(pMT->CheckInstanceActivated());

#ifdef LOGGING
    if (LoggingOn(LF_GCALLOC, LL_INFO10))
    {
        LogSpewAlways("Allocated %5d bytes for %s_TYPE" FMT_ADDR FMT_CLASS "\n",
                      size,
                      pMT->GetClass()->IsValueClass() ? "VAL" : "REF", 
                      DBG_ADDR(object),
                      DBG_CLASS_NAME_MT(pMT));

        if (LoggingOn(LF_GCALLOC, LL_INFO1000000)    || 
            (LoggingOn(LF_GCALLOC, LL_INFO100)   && 
             ToLogOrNotToLog(size, DBG_CLASS_NAME_MT(pMT))))
            {
                void LogStackTrace();
                LogStackTrace();
            }
        }
#endif
}
#else
#define LogAlloc(size, pMT, object)
#endif


/*
 * GcEnumObject()
 *
 * This is the JIT compiler (or any remote code manager)
 * GC enumeration callback
 */

void GcEnumObject(LPVOID pData, OBJECTREF *pObj, DWORD flags)
{
    Object ** ppObj = (Object **)pObj;
    GCCONTEXT   * pCtx  = (GCCONTEXT *) pData;

    // Since we may be asynchronously walking another thread's stack,
    // check (frequently) for stack-buffer-overrun corruptions after 
    // any long operation
    if (pCtx->cf != NULL)
        pCtx->cf->CheckGSCookies();

    //
    // Sanity check that the flags contain only these three values
    //
    assert((flags & ~(GC_CALL_INTERIOR|GC_CALL_PINNED|GC_CALL_CHECK_APP_DOMAIN)) == 0);

    // for interior pointers, we optimize the case in which
    //  it points into the current threads stack area
    //
    if (flags & GC_CALL_INTERIOR)
        PromoteCarefully (pCtx->f, *ppObj, pCtx->sc, flags);
    else
        (pCtx->f)( *ppObj, pCtx->sc, flags);
}

//-----------------------------------------------------------------------------

StackWalkAction GcStackCrawlCallBack(CrawlFrame* pCF, VOID* pData)
{
    Frame       *pFrame;
    GCCONTEXT   *gcctx = (GCCONTEXT*) pData;

#if CHECK_APP_DOMAIN_LEAKS
    gcctx->sc->pCurrentDomain = pCF->GetAppDomain();
#endif

#ifdef GC_PROFILING
    gcctx->sc->pMD = pCF->GetFunction();
#endif //GC_PROFILING

    ResetPointerHolder<CrawlFrame> rph(&gcctx->cf); // Clear it on exit so that we never have a stale CrawlFrame
    gcctx->cf = pCF; // put it somewhere so that GcEnumObject can get to it

    pFrame = pCF->GetFrame();

    if (pFrame != NULL)
    {
        STRESS_LOG3(LF_GCROOTS, LL_INFO1000, 
            "Scanning ExplicitFrame %p AssocMethod = %pM frameVTable = %pV\n", 
            pFrame, pFrame->GetFunction(), *((void**) pFrame));
        pFrame->GcScanRoots( gcctx->f, gcctx->sc);
    }
    else
    {
        ICodeManager * pCM = pCF->GetCodeManager();
        _ASSERTE(pCM != NULL);

        unsigned flags = pCF->GetCodeManagerFlags();
        
#ifdef _X86_
        STRESS_LOG3(LF_GCROOTS, LL_INFO1000, "Scanning Frameless method %pM EIP = %p &EIP = %p\n", 
            pCF->GetFunction(), *pCF->GetRegisterSet()->pPC, pCF->GetRegisterSet()->pPC);
#endif

#ifdef _DEBUG
        if (pCF->GetFunction() != 0)  
        {
            LOG((LF_GCROOTS, LL_INFO1000, "Scanning Frame for method %s:%s\n",
                 pCF->GetFunction()->m_pszDebugClassName, pCF->GetFunction()->m_pszDebugMethodName));
        }
#endif // _DEBUG

        EECodeInfo codeInfo(pCF->GetMethodToken(), pCF->GetJitManager());

        unsigned codeOffset = pCF->GetRelOffset();

        pCM->EnumGcRefs(pCF->GetRegisterSet(),
                        pCF->GetInfoBlock(),
                        &codeInfo,
                        codeOffset,
                        flags,
                        GcEnumObject,
                        pData);
    }

    // Since we may be asynchronously walking another thread's stack,
    // check (frequently) for stack-buffer-overrun corruptions after 
    // any long operation
    pCF->CheckGSCookies();

    return SWA_CONTINUE;
}

VOID CNameSpace::GcStartDoWork()
{
    // Update AppDomain stage here.
    SystemDomain::System()->ProcessClearingDomains();
}

/*
 * Scan for dead weak pointers
 */

VOID CNameSpace::GcWeakPtrScan( promote_func* fn, int condemned, int max_gen, ScanContext* sc )
{
    Ref_CheckReachable(condemned, max_gen, (LPARAM)sc);
}

static void CALLBACK CheckPromoted(_UNCHECKED_OBJECTREF *pObjRef, LPARAM *pExtraInfo, LPARAM lp1, LPARAM lp2)
{
    LOG((LF_GC, LL_INFO100000, LOG_HANDLE_OBJECT_CLASS("Checking referent of Weak-", pObjRef, "to ", *pObjRef)));

    Object **pRef = (Object **)pObjRef;
    if (!GCHeap::GetGCHeap()->IsPromoted(*pRef, (ScanContext *)lp1))
    {
        LOG((LF_GC, LL_INFO100, LOG_HANDLE_OBJECT_CLASS("Severing Weak-", pObjRef, "to unreachable ", *pObjRef)));

        *pRef = NULL;
    }
    else
    {
        LOG((LF_GC, LL_INFO1000000, "reachable " LOG_OBJECT_CLASS(*pObjRef)));
    }
}

VOID CNameSpace::GcWeakPtrScanBySingleThread( int condemned, int max_gen, ScanContext* sc )
{
    SyncBlockCache::GetSyncBlockCache()->GCWeakPtrScan(&CheckPromoted, (LPARAM)sc, 0);
}

VOID CNameSpace::GcShortWeakPtrScan(promote_func* fn,  int condemned, int max_gen, 
                                     ScanContext* sc)
{
    Ref_CheckAlive(condemned, max_gen, (LPARAM)sc);
}


//EE can perform post stack scanning action, while the 
// user threads are still suspended 
VOID CNameSpace::AfterGcScanRoots (int condemned, int max_gen,
                                   ScanContext* sc, GCHeap* Hp)
{
}

/*
 * Scan all stack roots in this 'namespace'
 */
 
VOID CNameSpace::GcScanRoots(promote_func* fn,  int condemned, int max_gen, 
                             ScanContext* sc, GCHeap* Hp )
{


    GCCONTEXT   gcctx;
    Thread*     pThread;

    gcctx.f  = fn;
    gcctx.sc = sc;
    gcctx.cf = NULL;

#if defined ( _DEBUG) && defined (CATCH_GC)
    //note that we can't use EX_TRY because the gc_thread isn't known
    PAL_TRY
#endif // _DEBUG && CATCH_GC
    {
        STRESS_LOG1(LF_GCROOTS, LL_INFO10, "GCScan: Promotion Phase = %d\n", sc->promotion);
        {
            ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

            // Either we are in a concurrent situation (in which case the thread is unknown to
            // us), or we are performing a synchronous GC and we are the GC thread, holding
            // the threadstore lock.
            
            _ASSERTE(dbgOnly_IsSpecialEEThread() ||
                     GetThread() == NULL ||
                     (GetThread() == GCHeap::GetGCHeap()->GetGCThread() && ThreadStore::HoldingThreadStore()));
            
            pThread = NULL;
            while ((pThread = ThreadStore::GetThreadList(pThread)) != NULL)
            {
                STRESS_LOG2(LF_GC|LF_GCROOTS, LL_INFO100, "{ Starting scan of Thread %p ID = %x\n", pThread, pThread->GetThreadId());
                if (GCHeap::GetGCHeap()->IsThreadUsingAllocationContextHeap(pThread->m_alloc_context, sc->thread_number))
                {
                    pThread->SetHasPromotedBytes();
#ifdef GC_PROFILING
                    gcctx.sc->rootKind = COR_PRF_GC_ROOT_STACK;
#endif //GC_PROFILING
                pThread->StackWalkFrames( GcStackCrawlCallBack, &gcctx, ALLOW_ASYNC_STACK_WALK | ALLOW_INVALID_OBJECTS);
#ifdef GC_PROFILING
                    sc->rootKind = COR_PRF_GC_ROOT_OTHER;
#endif //GC_PROFILING
                }
                STRESS_LOG2(LF_GC|LF_GCROOTS, LL_INFO100, "Ending scan of Thread %p ID = 0x%x }\n", pThread, pThread->GetThreadId());
            }

            // In server GC, we should be competing for marking the statics
            if (GCHeap::MarkShouldCompeteForStatics())
            {
                if (condemned == max_gen && sc->promotion)
                {
                    SystemDomain::EnumAllStaticGCRefs(GcEnumObject, &gcctx);                                
                }
            }

        }
    }

#if defined ( _DEBUG) && defined (CATCH_GC)
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        _ASSERTE (!"We got an exception during scan roots");
    }
    PAL_ENDTRY
#endif //_DEBUG
}

/*
 * Scan all handle roots in this 'namespace'
 */


VOID CNameSpace::GcScanHandles (promote_func* fn,  int condemned, int max_gen, 
                                ScanContext* sc)
{

#if defined ( _DEBUG) && defined (CATCH_GC)
    //note that we can't use EX_TRY because the gc_thread isn't known
    PAL_TRY
#endif // _DEBUG && CATCH_GC
    {
        STRESS_LOG1(LF_GC|LF_GCROOTS, LL_INFO10, "GcScanHandles (Promotion Phase = %d)\n", sc->promotion);
        if (sc->promotion == TRUE)
        {
            Ref_TracePinningRoots(condemned, max_gen, sc, fn);
            Ref_TraceNormalRoots(condemned, max_gen, sc, fn);
        }
        else
        {
            Ref_UpdatePointers(condemned, max_gen, sc, fn);
            Ref_UpdatePinnedPointers(condemned, max_gen, sc, fn);
        }
    }
    
#if defined ( _DEBUG) && defined (CATCH_GC)
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        _ASSERTE (!"We got an exception during scan roots");
    }
    PAL_ENDTRY
#endif //_DEBUG
}


#ifdef GC_PROFILING

/*
 * Scan all handle roots in this 'namespace' for profiling
 */

VOID CNameSpace::GcScanHandlesForProfiler (int max_gen, ScanContext* sc)
{

#if defined ( _DEBUG) && defined (CATCH_GC)
    //note that we can't use EX_TRY because the gc_thread isn't known
    PAL_TRY
#endif // _DEBUG && CATCH_GC
    {
        LOG((LF_GC|LF_GCROOTS, LL_INFO10, "Profiler Root Scan Phase, Handles\n"));
        Ref_ScanPointersForProfiler(max_gen, (LPARAM)sc);
    }
    
#if defined ( _DEBUG) && defined (CATCH_GC)
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        _ASSERTE (!"We got an exception during scan roots for the profiler");
    }
    PAL_ENDTRY
#endif //_DEBUG
}

#endif // GC_PROFILING

void CNameSpace::GcRuntimeStructuresValid (BOOL bValid)
{
    if (!bValid)
    {
        LONG result = InterlockedIncrement (&m_GcStructuresInvalidCnt);
        _ASSERTE (result > 0);
    }
    else
    {
        LONG result = InterlockedDecrement (&m_GcStructuresInvalidCnt);
        _ASSERTE (result >= 0);
    }
}

void CNameSpace::GcStartWork (int condemned, int max_gen)
{
    HostCodeHeap::CleanupCodeHeaps();
}

void CNameSpace::GcDemote (int condemned, int max_gen, ScanContext* sc)
{
    Ref_RejuvenateHandles (condemned, max_gen, (LPARAM)sc);
    if (!GCHeap::IsServerHeap() || sc->thread_number == 0)
        SyncBlockCache::GetSyncBlockCache()->GCDone(TRUE, max_gen);
}

void CNameSpace::GcPromotionsGranted (int condemned, int max_gen, ScanContext* sc)
{
    Ref_AgeHandles(condemned, max_gen, (LPARAM)sc);
    if (!GCHeap::IsServerHeap() || sc->thread_number == 0)
        SyncBlockCache::GetSyncBlockCache()->GCDone(FALSE, max_gen);
}


void CNameSpace::GcFixAllocContexts (void* arg, void *heap)
{
    if (GCHeap::UseAllocationContexts())
    {
        Thread  *thread;

        thread = NULL;
        while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
        {
            GCHeap::GetGCHeap()->FixAllocContext (&thread->m_alloc_context, FALSE, arg, heap);
        }
    }
}

void CNameSpace::GcEnumAllocContexts (enum_alloc_context_func* fn, void* arg)
{
    if (GCHeap::UseAllocationContexts())
    {
        Thread  *thread;

        thread = NULL;
        while ((thread = ThreadStore::GetThreadList(thread)) != NULL)
        {
            (*fn) (&thread->m_alloc_context, arg);
        }
    }
}


size_t CNameSpace::AskForMoreReservedMemory (size_t old_size, size_t need_size)
{
    // call the host....

    IGCHostControl *pGCHostControl = CorHost::GetGCHostControl();

    if (pGCHostControl)
    {
        size_t new_max_limit_size = need_size;
        pGCHostControl->RequestVirtualMemLimit (old_size, 
                                                (SIZE_T*)&new_max_limit_size);
        return new_max_limit_size;
    }
    else
        return old_size + need_size;
}

void CNameSpace::VerifyHandleTable(int condemned, int max_gen)
{
    Ref_VerifyHandleTable(condemned, max_gen);
}


// PromoteCarefully
//
// Clients who know they MAY have an interior pointer should come through here.  We
// can efficiently check whether our object lives on the current stack.  If so, our
// reference to it is not an interior pointer.  This is more efficient than asking
// the heap to verify whether our reference is interior, since it would have to
// check all the heap segments, including those containing large objects.
//
//
// The flags must indicate that the have an interior pointer GC_CALL_INTERIOR
// additionally the flags may indicate that we also have a pinned local byref
// 
void PromoteCarefully(promote_func  fn, 
                      Object *& obj, 
                      ScanContext*  sc, 
                      DWORD         flags /* = GC_CALL_INTERIOR*/ )
{
    //
    // Sanity check that the flags contain only these three values
    //
    assert((flags & ~(GC_CALL_INTERIOR|GC_CALL_PINNED|GC_CALL_CHECK_APP_DOMAIN)) == 0);

    //
    // Sanity check that GC_CALL_INTERIOR FLAG is set
    //
    assert(flags & GC_CALL_INTERIOR);


    (*fn) (obj, sc, flags);
}

OBJECTREF AllocateValueSzArray(TypeHandle elementType, INT32 length)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative        
    } CONTRACTL_END;

    return AllocateArrayEx(elementType.MakeSZArray(), &length, 1, FALSE);
}

//
// Handles arrays of arbitrary dimensions
//
// If dwNumArgs is set to greater than 1 for a SZARRAY this function will recursively 
// allocate sub-arrays and fill them in.  
//
// For arrays with lower bounds, pBounds is <lower bound 1>, <count 1>, <lower bound 2>, ...
OBJECTREF AllocateArrayEx(TypeHandle arrayType, INT32 *pArgs, DWORD dwNumArgs, BOOL bAllocateInLargeHeap) 
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pArgs));
        PRECONDITION(dwNumArgs > 0);
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
    } CONTRACTL_END;

#ifdef _DEBUG
    if (g_pConfig->ShouldInjectFault(INJECTFAULT_GCHEAP))
    {
        char *a = new char;
        delete a;
    }
#endif

    ArrayTypeDesc* arrayDesc = arrayType.AsArray();
    MethodTable* pArrayMT = arrayDesc->GetMethodTable();
    PREFIX_ASSUME(pArrayMT != NULL);
    CorElementType kind = arrayType.GetInternalCorElementType();
    _ASSERTE(kind == ELEMENT_TYPE_ARRAY || kind == ELEMENT_TYPE_SZARRAY);
    
    CorElementType elemType = arrayDesc->GetTypeParam().GetInternalCorElementType();
    // Disallow the creation of void[,] (a multi-dim  array of System.Void)
    if (elemType == ELEMENT_TYPE_VOID)
        COMPlusThrow(kArgumentException);

    // Calculate the total number of elements in the array
    INT32 cElements = pArgs[0];
    bool providedLowerBounds = false;
    unsigned rank;

    if (kind == ELEMENT_TYPE_ARRAY)
    {
        rank = arrayDesc->GetRank();
        _ASSERTE(dwNumArgs == rank || dwNumArgs == 2*rank);

        // Morph a ARRAY rank 1 with 0 lower bound into an SZARRAY
        if (rank == 1 && (dwNumArgs == 1 || pArgs[0] == 0)) 
        {   // lower bound is zero

            // This recursive call doesn't go any farther, because the dwNumArgs will be 1,
            //  so don't bother with stack probe.
            TypeHandle szArrayType = ClassLoader::LoadArrayTypeThrowing(arrayDesc->GetArrayElementTypeHandle(), ELEMENT_TYPE_SZARRAY, 1);
            return AllocateArrayEx(szArrayType, &pArgs[dwNumArgs - 1], 1, bAllocateInLargeHeap);
        }

        providedLowerBounds = (dwNumArgs == 2*rank);
        cElements = 1;
        for (unsigned i = 0; i < dwNumArgs; i++)
        {
            int lowerBound = 0;
            if (providedLowerBounds)
            {
                lowerBound = pArgs[i];
                i++;
            }
            int length = pArgs[i];
            if (length < 0)
                COMPlusThrow(kOverflowException);
            if (lowerBound + length < lowerBound)
                COMPlusThrow(kArgumentOutOfRangeException, L"ArgumentOutOfRange_ArrayLBAndLength");
            unsigned __int64 tempElement = (unsigned __int64) cElements * unsigned(length);
            if ((tempElement >> 32) != 0)              // watch for wrap around
                ThrowOutOfMemory();
            cElements = (unsigned) tempElement;
        }
    } 
    else if (cElements < 0)
        COMPlusThrow(kOverflowException);

    // Allocate the space from the GC heap
    g_IBCLogger.LogMethodTableAccess(pArrayMT);

    S_SIZE_T safeTotalSize = S_SIZE_T(cElements) * S_SIZE_T(pArrayMT->GetComponentSize()) + S_SIZE_T(pArrayMT->GetBaseSize());
    if (safeTotalSize.IsOverflow())
        ThrowOutOfMemory();

    size_t totalSize = safeTotalSize.Value();

    if ((elemType == ELEMENT_TYPE_R8) && 
        (totalSize >= g_pConfig->GetDoubleArrayToLargeObjectHeap()))
    {
        STRESS_LOG1(LF_GC, LL_INFO10, "Allocating double MD array of size %d to large object heap\n", totalSize);
        bAllocateInLargeHeap = TRUE;
    }

    ArrayBase* orObject = NULL;
    if (bAllocateInLargeHeap)
        orObject = (ArrayBase *) AllocLHeap(totalSize, FALSE, pArrayMT->ContainsPointers());
    else
        orObject = (ArrayBase *) Alloc(totalSize, FALSE, pArrayMT->ContainsPointers());

        // Initialize Object
    orObject->SetMethodTable(pArrayMT);
    orObject->m_NumComponents = cElements;

    if (pArrayMT->IsSharedByReferenceArrayTypes())
    {
#ifdef  _LOGALLOC
#ifdef LOGGING
        if (LoggingOn(LF_GCALLOC, LL_INFO10))
        {
            StackSString ssClassName;
            StackScratchBuffer scratch;
            LPCUTF8 szClassName = NULL;

            EX_TRY
            {
                arrayDesc->GetArrayElementTypeHandle().GetName(ssClassName);
                szClassName = ssClassName.GetUTF8(scratch);
            }
            EX_CATCH
            {
                szClassName = "Unknown Class (out-of-memory attempting to lookup class name)";
            }
            EX_END_CATCH(SwallowAllExceptions)

            LogSpewAlways("Allocated %5d bytes for %s_TYPE" FMT_ADDR "%s[]\n",
                          totalSize, 
                          pArrayMT->GetClass()->IsValueClass() ? "VAL" : "REF",
                          DBG_ADDR(orObject), szClassName);

            if (LoggingOn(LF_GCALLOC, LL_INFO1000000)    || 
                (LoggingOn(LF_GCALLOC, LL_INFO100)   && 
                 ToLogOrNotToLog(totalSize, szClassName)))
            {
                    void LogStackTrace();
                    LogStackTrace();
                }
            }
#endif
#endif
        orObject->SetArrayElementTypeHandle(arrayDesc->GetArrayElementTypeHandle());
    }
    else
        LogAlloc(totalSize, pArrayMT, orObject);


#ifdef _DEBUG
    // Ensure the typehandle has been interned prior to allocation.
    // This is important for OOM reliability.
    OBJECTREF objref = ObjectToOBJECTREF((Object*)orObject);
    GCPROTECT_BEGIN(objref);

    orObject->GetTypeHandle(); 

    GCPROTECT_END();    
    orObject = (ArrayBase *) OBJECTREFToObject(objref); 
#endif

#if CHECK_APP_DOMAIN_LEAKS
    if (g_pConfig->AppDomainLeaks())
        orObject->SetAppDomain();
#endif

    if (kind == ELEMENT_TYPE_ARRAY)
    {
        INT32 *pCountsPtr      = (INT32 *) orObject->GetBoundsPtr();
        INT32 *pLowerBoundsPtr = (INT32 *) orObject->GetLowerBoundsPtr();
        for (unsigned i = 0; i < dwNumArgs; i++)
        {
            if (providedLowerBounds)
                *pLowerBoundsPtr++ = pArgs[i++];        // if not stated, lower bound becomes 0
            *pCountsPtr++ = pArgs[i];
        }

        // Notify the profiler of the allocation
        // do this after initializing bounds so callback has size information
        if (TrackAllocations())
        {
            ProfileTrackArrayAlloc(orObject);
        }
    }
    else
    {
        // Notify the profiler of the allocation
        if (TrackAllocations())
        {
            ProfileTrackArrayAlloc(orObject);
        }

        // Handle allocating multiple jagged array dimensions at once
        if (dwNumArgs > 1)
        {
            PTRARRAYREF pOuterArray = (PTRARRAYREF) ObjectToOBJECTREF((Object*)orObject);
            PTRARRAYREF ret;
            GCPROTECT_BEGIN(pOuterArray);

            #ifdef STRESS_HEAP
            // Turn off GC stress, it is of little value here
            int gcStress = g_pConfig->GetGCStressLevel();
            g_pConfig->SetGCStressLevel(0);
            #endif //STRESS_HEAP
            
            // Allocate dwProvidedBounds arrays
            if (!arrayDesc->GetArrayElementTypeHandle().IsArray()) {
                ret = NULL;
            } 
            else 
            {
                _ASSERTE(GetThread());
                INTERIOR_STACK_PROBE(GetThread());
            
                TypeHandle subArrayType = arrayDesc->GetArrayElementTypeHandle();
                for (INT32 i = 0; i < cElements; i++)
                {
                    OBJECTREF obj = AllocateArrayEx(subArrayType, &pArgs[1], dwNumArgs-1, bAllocateInLargeHeap);
                    pOuterArray->SetAt(i, obj);
                }

                #ifdef STRESS_HEAP
                g_pConfig->SetGCStressLevel(gcStress);      // restore GCStress
                #endif // STRESS_HEAP
                
                END_INTERIOR_STACK_PROBE
                ret = pOuterArray;                          // have to pass it in another var since GCPROTECTE_END zaps it
            }
            GCPROTECT_END();
            return (OBJECTREF) ret;
        }
    }

    return( ObjectToOBJECTREF((Object*)orObject) );
}

/*
 * Allocates a single dimensional array of primitive types.
 */
OBJECTREF   AllocatePrimitiveArray(CorElementType type, DWORD cElements, BOOL bAllocateInLargeHeap)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_COOPERATIVE;  // returns an objref without pinning it => cooperative
    }
    CONTRACTL_END

    _ASSERTE(CorTypeInfo::IsPrimitiveType(type));

    // Fetch the proper array type
    if (g_pPredefinedArrayTypes[type] == NULL)
    {
        TypeHandle elemType = ElementTypeToTypeHandle(type);
        TypeHandle typHnd = ClassLoader::LoadArrayTypeThrowing(elemType, ELEMENT_TYPE_SZARRAY, 0);
        g_pPredefinedArrayTypes[type] = typHnd.AsArray();
    }
    return FastAllocatePrimitiveArray(g_pPredefinedArrayTypes[type]->GetMethodTable(), cElements, bAllocateInLargeHeap);
}

/*
 * Allocates a single dimensional array of primitive types.
 */

OBJECTREF   FastAllocatePrimitiveArray(MethodTable* pMT, DWORD cElements, BOOL bAllocateInLargeHeap)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
    } CONTRACTL_END;

#ifdef _DEBUG
    if (g_pConfig->ShouldInjectFault(INJECTFAULT_GCHEAP))
    {
        char *a = new char;
        delete a;
    }
#endif

    _ASSERTE(pMT && pMT->IsArray());
    _ASSERTE(pMT->IsRestored());
    _ASSERTE(CorTypeInfo::IsPrimitiveType(pMT->GetArrayElementType()) &&
             g_pPredefinedArrayTypes[pMT->GetArrayElementType()] != NULL);
    
    g_IBCLogger.LogMethodTableAccess(pMT);

    S_SIZE_T safeTotalSize = S_SIZE_T(cElements) * S_SIZE_T(pMT->GetComponentSize()) + S_SIZE_T(pMT->GetBaseSize());
    if (safeTotalSize.IsOverflow())
        ThrowOutOfMemory();

    size_t totalSize = safeTotalSize.Value();

    ArrayBase* orObject;
    if (bAllocateInLargeHeap)
    {
        orObject = (ArrayBase*) AllocLHeap(totalSize, FALSE, FALSE);
    }
    else 
    {
        ArrayTypeDesc *pArrayR8TypeDesc = g_pPredefinedArrayTypes[ELEMENT_TYPE_R8];
        if (DATA_ALIGNMENT < sizeof(double) && pArrayR8TypeDesc != NULL && pMT == pArrayR8TypeDesc->GetMethodTable() && totalSize < LARGE_OBJECT_SIZE - MIN_OBJECT_SIZE) 
        {
            // Creation of an array of doubles, not in the large object heap.
            // We want to align the doubles to 8 byte boundaries, but the GC gives us pointers aligned
            // to 4 bytes only (on 32 bit platforms). To align, we ask for 12 bytes more to fill with a
            // dummy object.
            // If the GC gives us a 8 byte aligned address, we use it for the array and place the dummy
            // object after the array, otherwise we put the dummy object first, shifting the base of
            // the array to an 8 byte aligned address.
            // Note: on 64 bit platforms, the GC always returns 8 byte aligned addresses, and we don't
            // execute this code because DATA_ALIGNMENT < sizeof(double) is false.

            _ASSERTE(DATA_ALIGNMENT == sizeof(double)/2);
            _ASSERTE((MIN_OBJECT_SIZE % sizeof(double)) == DATA_ALIGNMENT);   // used to change alignment
            _ASSERTE(pMT->GetComponentSize() == sizeof(double));
            _ASSERTE(g_pObjectClass->GetBaseSize() == MIN_OBJECT_SIZE);
            _ASSERTE(totalSize < totalSize + MIN_OBJECT_SIZE);
            orObject = (ArrayBase*) Alloc(totalSize + MIN_OBJECT_SIZE, FALSE, FALSE);

            Object *orDummyObject;
            if((size_t)orObject % sizeof(double))
            {
                orDummyObject = orObject;
                orObject = (ArrayBase*) ((size_t)orObject + MIN_OBJECT_SIZE);
            }
            else
            {
                orDummyObject = (Object*) ((size_t)orObject + totalSize);
            }
            _ASSERTE(((size_t)orObject % sizeof(double)) == 0);
            orDummyObject->SetMethodTable(g_pObjectClass);
        }
        else
        {
            orObject = (ArrayBase*) Alloc(totalSize, FALSE, FALSE);
        }
    }


    // Initialize Object
    orObject->SetMethodTable( pMT );
    _ASSERTE(orObject->GetMethodTable() != NULL);
    orObject->m_NumComponents = cElements;

    // Notify the profiler of the allocation
    if (TrackAllocations())
    {
        OBJECTREF objref = ObjectToOBJECTREF((Object*)orObject);
        GCPROTECT_BEGIN(objref);
        ProfilerObjectAllocatedCallback(objref, (TypeID) orObject->GetTypeHandle().AsPtr());
        GCPROTECT_END();
        
        orObject = (ArrayBase *) OBJECTREFToObject(objref); 
    }

    LogAlloc(totalSize, pMT, orObject);

#if CHECK_APP_DOMAIN_LEAKS
    if (g_pConfig->AppDomainLeaks())
        orObject->SetAppDomain();
#endif

    return( ObjectToOBJECTREF((Object*)orObject) );
}

//
// Allocate an array which is the same size as pRef.  However, do not zero out the array.
//
OBJECTREF   DupArrayForCloning(BASEARRAYREF pRef, BOOL bAllocateInLargeHeap)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
    } CONTRACTL_END;

    ArrayTypeDesc arrayType(pRef->GetMethodTable(), pRef->GetArrayElementTypeHandle());
    unsigned rank = arrayType.GetRank();

    DWORD numArgs =  rank*2;
    INT32* args = (INT32*) _alloca(sizeof(INT32)*numArgs);

    if (arrayType.GetInternalCorElementType() == ELEMENT_TYPE_ARRAY)
    {
        const INT32* bounds = pRef->GetBoundsPtr();
        const INT32* lowerBounds = pRef->GetLowerBoundsPtr();
        for(unsigned int i=0; i < rank; i++) 
        {
            args[2*i]   = lowerBounds[i];
            args[2*i+1] = bounds[i];
        }
    }
    else
    {
        numArgs = 1;
        args[0] = pRef->GetNumComponents();
    }
    return AllocateArrayEx(TypeHandle(&arrayType), args, numArgs, bAllocateInLargeHeap);
}

#if defined(_X86_)
// The fast version always allocates in the normal heap
OBJECTREF AllocateObjectArray(DWORD cElements, TypeHandle ElementType)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
    } CONTRACTL_END;

    // We must call this here to ensure the typehandle for this object is
    // interned before the object is allocated. As soon as the object is allocated,
    // the profiler could do a heapwalk and it expects to find an interned
    // typehandle for every object in the heap.
    ClassLoader::LoadArrayTypeThrowing(ElementType);

    return OBJECTREF( HCCALL2(fastObjectArrayAllocator, ElementType.AsPtr(), cElements));
}
#endif

//
// Helper for parts of the EE which are allocating arrays
//
OBJECTREF   AllocateObjectArray(DWORD cElements, TypeHandle elementType, BOOL bAllocateInLargeHeap)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
    } CONTRACTL_END;

    // The object array class is loaded at startup.
    _ASSERTE(g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT] != NULL);

#ifdef _DEBUG
    ArrayTypeDesc arrayType(g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT]->GetMethodTable(), elementType);
    _ASSERTE(arrayType.GetRank() == 1);
    _ASSERTE(arrayType.GetInternalCorElementType() == ELEMENT_TYPE_SZARRAY);
#endif //_DEBUG

    return AllocateArrayEx(ClassLoader::LoadArrayTypeThrowing(elementType),
                           (INT32 *)(&cElements),
                           1,
                           bAllocateInLargeHeap);
}


STRINGREF SlowAllocateString( DWORD cchArrayLength )
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
    } CONTRACTL_END;

    StringObject    *orObject  = NULL;
    DWORD           ObjectSize;

#ifdef _DEBUG
    if (g_pConfig->ShouldInjectFault(INJECTFAULT_GCHEAP))
    {
        char *a = new char;
        delete a;
    }
#endif
   
    ObjectSize = g_pStringClass->GetBaseSize() + (cchArrayLength * sizeof(WCHAR));

    //Check for overflow.
    if (ObjectSize < cchArrayLength) 
        ThrowOutOfMemory();

    orObject = (StringObject *)Alloc( ObjectSize, FALSE, FALSE );

    // Object is zero-init already
    _ASSERTE( orObject->HasEmptySyncBlockInfo() );

    // Initialize Object
    orObject->SetMethodTable( g_pStringClass );
    orObject->SetArrayLength( cchArrayLength );

    // Notify the profiler of the allocation
    if (TrackAllocations())
    {
        OBJECTREF objref = ObjectToOBJECTREF((Object*)orObject);
        GCPROTECT_BEGIN(objref);
        ProfilerObjectAllocatedCallback(objref, (TypeID) orObject->GetTypeHandle().AsPtr());
        GCPROTECT_END();
        
        orObject = (StringObject *) OBJECTREFToObject(objref); 
    }

    LogAlloc(ObjectSize, g_pStringClass, orObject);

#if CHECK_APP_DOMAIN_LEAKS
    if (g_pConfig->AppDomainLeaks())
        orObject->SetAppDomain(); 
#endif

    return( ObjectToSTRINGREF(orObject) );
}


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//  As FastAllocateObject and AllocateObject drift apart, be sure to update
//  CEEJitInfo::canUseFastNew() so that it understands when to use which service
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// AllocateObjectSpecial will throw OutOfMemoryException so don't need to check
// for NULL return value from it.
OBJECTREF AllocateObjectSpecial( MethodTable *pMT )
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(pMT->CheckInstanceActivated());
    } CONTRACTL_END;

    Object     *orObject = NULL;
    // use unchecked oref here to avoid triggering assert in Validate that the AD is
    // not set becuase it isn't until near the end of the fcn at which point we can allow
    // the check.
    _UNCHECKED_OBJECTREF oref;

    g_IBCLogger.LogMethodTableAccess(pMT);
    if (pMT->HasCriticalFinalizer())
        PrepareCriticalFinalizerObject(pMT);

    {   
        orObject = (Object *) Alloc(pMT->GetBaseSize(),
                                    pMT->HasFinalizer(),
                                    pMT->ContainsPointers());

        // verify zero'd memory (at least for sync block)
        _ASSERTE( orObject->HasEmptySyncBlockInfo() );

        orObject->SetMethodTable(pMT);

#if CHECK_APP_DOMAIN_LEAKS
        if (g_pConfig->AppDomainLeaks())
            orObject->SetAppDomain(); 
        else
#endif
        if (pMT->HasFinalizer())
            orObject->SetAppDomain(); 

        // Notify the profiler of the allocation
        if (TrackAllocations())
        {
            OBJECTREF objref = ObjectToOBJECTREF((Object*)orObject);
            GCPROTECT_BEGIN(objref);
            ProfilerObjectAllocatedCallback(objref, (TypeID) orObject->GetTypeHandle().AsPtr());
            GCPROTECT_END();

            orObject = (Object *) OBJECTREFToObject(objref); 
        }

        LogAlloc(pMT->GetBaseSize(), pMT, orObject);

        oref = OBJECTREF_TO_UNCHECKED_OBJECTREF(orObject);
    }

    return UNCHECKED_OBJECTREF_TO_OBJECTREF(oref);
}

// AllocateObject will throw OutOfMemoryException so don't need to check
// for NULL return value from it.
OBJECTREF AllocateObject( MethodTable *pMT )
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(pMT->CheckInstanceActivated());
    } CONTRACTL_END;

    OBJECTREF oref = AllocateObjectSpecial(pMT);


    return oref;
}


// The JIT compiles calls to FastAllocateObject instead of AllocateObject if it
// can prove that the caller and calleee are guaranteed to be in the same context.
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//  As FastAllocateObject and AllocateObject drift apart, be sure to update
//  CEEJitInfo::canUseFastNew() so that it understands when to use which service
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING ** WARNING
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// FastAllocateObject will throw OutOfMemoryException so don't need to check
// for NULL return value from it.
OBJECTREF FastAllocateObject( MethodTable *pMT )
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE; // returns an objref without pinning it => cooperative
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(pMT->CheckInstanceActivated());
    } CONTRACTL_END;

    Object     *orObject = NULL;

    if (pMT->HasCriticalFinalizer())
        PrepareCriticalFinalizerObject(pMT);

    orObject = (Object *) Alloc(pMT->GetBaseSize(),
                                pMT->HasFinalizer(),
                                pMT->ContainsPointers());

    // verify zero'd memory (at least for sync block)
    _ASSERTE( orObject->HasEmptySyncBlockInfo() );

    orObject->SetMethodTable(pMT);

    // Notify the profiler of the allocation
    if (TrackAllocations())
    {
        OBJECTREF objref = ObjectToOBJECTREF((Object*)orObject);
        GCPROTECT_BEGIN(objref);
        ProfilerObjectAllocatedCallback(objref, (TypeID) orObject->GetTypeHandle().AsPtr());
        GCPROTECT_END();
        
        orObject = (Object *) OBJECTREFToObject(objref); 
    }

    LogAlloc(pMT->GetBaseSize(), pMT, orObject);

#if CHECK_APP_DOMAIN_LEAKS
    if (g_pConfig->AppDomainLeaks())
        orObject->SetAppDomain(); 
    else
#endif
    if (pMT->HasFinalizer())
        orObject->SetAppDomain(); 

    return( ObjectToOBJECTREF(orObject) );
}

#endif // DACCESS_COMPILE

#ifdef _MSC_VER
#pragma warning(default : 4640)
#endif



