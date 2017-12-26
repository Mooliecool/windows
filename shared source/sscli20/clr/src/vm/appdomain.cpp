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


#include "appdomain.hpp"
#include "peimagelayout.inl"
#include "field.h"
#include "security.h"
#include "securitydescriptor.h"
#include "comstring.h"
#include "comstringbuffer.h"
#include "excep.h"
#include "eeconfig.h"
#include "gc.h"
#include "assemblysink.h"
#include "perfcounters.h"
#include "assemblyname.hpp"
#include "fusion.h"
#include "eeprofinterfaces.h"
#include "dbginterface.h"
#ifndef DACCESS_COMPILE
#include "eedbginterfaceimpl.h"
#endif
#include "comdynamic.h"
#include "mlinfo.h"
#include "remoting.h"
#include "posterror.h"
#include "assemblynative.hpp"
#include "fusionbind.h"
#include "shimload.h"
#include "stringliteralmap.h"
#include "timeline.h"
#include "appdomainhelper.h"
#include "codeman.h"
#include "comcallablewrapper.h"
#include "virtualcallstub.h"
#include "apithreadstress.h"
#include "memoryreport.h"
#include "eventtrace.h"
#include "comdelegate.h"
#include "siginfo.hpp"

#include "objectclone.h"
#include "typekey.h"



#include "listlock.inl"
#include "threads.inl"
#include "appdomain.inl"
#include "typeparse.h"
#include "mdaassistantsptr.h"
#include "stackcompressor.h"
#include "crossdomaincalls.h"

#include "nativeoverlapped.h"

#include "compatibilityflags.h"


// this file handles string conversion errors for itself
#undef  MAKE_TRANSLATIONFAILED

// Define these macro's to do strict validation for jit lock and class
// init entry leaks.  This defines determine if the asserts that
// verify for these leaks are defined or not.  These asserts can
// sometimes go off even if no entries have been leaked so this
// defines should be used with caution.
//
// If we are inside a .cctor when the application shut's down then the
// class init lock's head will be set and this will cause the assert
// to go off.
//
// If we are jitting a method when the application shut's down then
// the jit lock's head will be set causing the assert to go off.

//#define STRICT_CLSINITLOCK_ENTRY_LEAK_DETECTION

static const WCHAR DEFAULT_DOMAIN_FRIENDLY_NAME[] = L"DefaultDomain";
static const WCHAR OTHER_DOMAIN_FRIENDLY_NAME_PREFIX[] = L"Domain";

#define STATIC_OBJECT_TABLE_BUCKET_SIZE 1020

//#define _DEBUG_ADUNLOAD 1

HRESULT RunDllMain(MethodDesc *pMD, HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved); // clsload.cpp





// Statics

SPTR_IMPL(SystemDomain, SystemDomain, m_pSystemDomain);
SVAL_IMPL(ArrayListStatic, SystemDomain, m_appDomainIndexList);
SPTR_IMPL(SharedDomain, SharedDomain, m_pSharedDomain);

#ifndef DACCESS_COMPILE

// Static fields in BaseDomain
BOOL                BaseDomain::m_fShadowCopy                = FALSE;
BOOL                BaseDomain::m_fExecutable                = FALSE;

CrstStatic          BaseDomain::m_SpecialStaticsCrst;

// Shared Domain Statics
static BYTE         g_pSharedDomainMemory[sizeof(SharedDomain)];

// System Domain Statics
GlobalStringLiteralMap* SystemDomain::m_pGlobalStringLiteralMap = NULL;

static BYTE         g_pSystemDomainMemory[sizeof(SystemDomain)];

CrstStatic          SystemDomain::m_SystemDomainCrst;
CrstStatic          SystemDomain::m_DelayedUnloadCrst;

ULONG               SystemDomain::s_dNumAppDomains = 0;

AppDomain *         SystemDomain::m_pAppDomainBeingUnloaded = NULL;
ADIndex             SystemDomain::m_dwIndexOfAppDomainBeingUnloaded;
Thread            *SystemDomain::m_pAppDomainUnloadRequestingThread = 0;
Thread            *SystemDomain::m_pAppDomainUnloadingThread = 0;

ArrayListStatic     SystemDomain::m_appDomainIdList;


DWORD               SystemDomain::m_dwLowestFreeIndex        = 0;

BOOL                SystemDomain::s_fForceDebug = FALSE;
BOOL                SystemDomain::s_fForceProfiling = FALSE;
BOOL                SystemDomain::s_fForceInstrument = FALSE;

// comparison function to be used for matching clsids in our clsid hash table
BOOL CompareCLSID(UPTR u1, UPTR u2)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_INTOLERANT;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    GUID *pguid = (GUID *)(u1 << 1);
    _ASSERTE(pguid != NULL);

    MethodTable *pMT= (MethodTable *)u2;
    _ASSERTE(pMT!= NULL);

    GUID guid;
    pMT->GetGuid(&guid, TRUE);
    if (!IsEqualIID(guid, *pguid))
        return FALSE;

    return TRUE;
}


// Constructor for the LargeHeapHandleBucket class.
LargeHeapHandleBucket::LargeHeapHandleBucket(LargeHeapHandleBucket *pNext, DWORD Size, BaseDomain *pDomain)
: m_pNext(pNext)
, m_ArraySize(Size)
, m_CurrentPos(0)
, m_CurrentEmbeddedFreePos(0) // hint for where to start a search for an embedded free item
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pDomain));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    // Allocate the array in the large object heap.
    PTRARRAYREF HandleArrayObj = (PTRARRAYREF)AllocateObjectArray(Size, g_pObjectClass, TRUE);

    // Retrieve the pointer to the data inside the array. This is legal since the array
    // is located in the large object heap and is guaranteed not to move.
    m_pArrayDataPtr = (OBJECTREF *)HandleArrayObj->GetDataPtr();

    // Store the array in a strong handle to keep it alive.
    m_hndHandleArray = pDomain->CreatePinningHandle((OBJECTREF)HandleArrayObj);
}


// Destructor for the LargeHeapHandleBucket class.
LargeHeapHandleBucket::~LargeHeapHandleBucket()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (m_hndHandleArray)
    {
        DestroyPinningHandle(m_hndHandleArray);
        m_hndHandleArray = NULL;
    }
}


// Allocate handles from the bucket.
OBJECTREF *LargeHeapHandleBucket::AllocateHandles(DWORD nRequested)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    _ASSERTE(nRequested > 0 && nRequested <= GetNumRemainingHandles());
    _ASSERTE(m_pArrayDataPtr == (OBJECTREF*)((PTRARRAYREF)ObjectFromHandle(m_hndHandleArray))->GetDataPtr());

    // Store the handles in the buffer that was passed in
    OBJECTREF* ret = &m_pArrayDataPtr[m_CurrentPos];
    m_CurrentPos += nRequested;

    return ret;
}

// look for a free item embedded in the table
OBJECTREF *LargeHeapHandleBucket::TryAllocateEmbeddedFreeHandle()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    OBJECTREF pPreallocatedSentinalObject = ObjectFromHandle(g_pPreallocatedSentinelObject);
    _ASSERTE(pPreallocatedSentinalObject  != NULL);

    for (int  i = m_CurrentEmbeddedFreePos; i < m_CurrentPos; i++)
    {
        if (m_pArrayDataPtr[i] == pPreallocatedSentinalObject)
        {
            m_CurrentEmbeddedFreePos = i;
            m_pArrayDataPtr[i] = NULL;
            return &m_pArrayDataPtr[i];
        }
    }

    // didn't find it (we don't bother wrapping around for a full search, it's not worth it to try that hard, we'll get it next time)

    m_CurrentEmbeddedFreePos = 0;
    return NULL;
}


// Constructor for the LargeHeapHandleTable class.
LargeHeapHandleTable::LargeHeapHandleTable(BaseDomain *pDomain, DWORD BucketSize)
: m_pDomain(pDomain)
, m_BucketSize(BucketSize)
, m_pFreeSearchHint(NULL)
, m_cEmbeddedFree(0)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pDomain));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

#ifdef _DEBUG
     m_pCrstDebug = NULL;
#endif

    m_pHead = new LargeHeapHandleBucket(NULL, BucketSize, pDomain);
}


// Destructor for the LargeHeapHandleTable class.
LargeHeapHandleTable::~LargeHeapHandleTable()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    // Delete the buckets.
    while (m_pHead)
    {
        LargeHeapHandleBucket *pOld = m_pHead;
        m_pHead = pOld->GetNext();
        delete pOld;
    }
}

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//


OBJECTREF* LargeHeapHandleTable::AllocateHandles(DWORD nRequested)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(nRequested > 0);
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // SEE "LOCKING RULES FOR AllocateHandles() and ReleaseHandles()" above

    // the lock must be registered and already held by the caller per contract
    _ASSERTE(m_pCrstDebug != NULL);
    _ASSERTE(m_pCrstDebug->OwnedByCurrentThread());

    if (nRequested == 1 && m_cEmbeddedFree != 0)
    {
        // special casing singleton requests to look for slots that can be re-used

        // we need to do this because string literals are allocated one at a time and then sometimes
        // released.  we do not wish for the number of handles consumed by string literals to
        // increase forever as assemblies are loaded and unloaded

        if (m_pFreeSearchHint == NULL)
            m_pFreeSearchHint = m_pHead;

        while (m_pFreeSearchHint)
        {
            OBJECTREF* pObjRef = m_pFreeSearchHint->TryAllocateEmbeddedFreeHandle();
            if (pObjRef != NULL)
            {
                // the slot is to have been prepared with a null ready to go
                _ASSERTE(*pObjRef == NULL);
                m_cEmbeddedFree--;
                return pObjRef;
            }
            m_pFreeSearchHint = m_pFreeSearchHint->GetNext();
        }

        // the search doesn't wrap around so it's possible that we might have embedded free items
        // and not find them but that's ok, we'll get them on the next alloc... all we're trying to do
        // is to not have big leaks over time.
    }


    // Retrieve the remaining number of handles in the bucket.
    DWORD NumRemainingHandlesInBucket = m_pHead->GetNumRemainingHandles();

    // create a new block if this request doesn't fit in the current block
    if (nRequested > NumRemainingHandlesInBucket)
    {
        // mark the handles in that remaining region as available for re-use
        ReleaseHandles(m_pHead->CurrentPos(), NumRemainingHandlesInBucket);

        // mark what's left as having been used
        m_pHead->ConsumeRemaining();

        // create a new bucket for this allocation

        DWORD dwDefaultBucketSize = m_BucketSize;

        // We need a block big enough to hold the requested handles
        DWORD NewBucketSize = max(dwDefaultBucketSize, nRequested);

        m_pHead = new LargeHeapHandleBucket(m_pHead, NewBucketSize, m_pDomain);
    }

    return m_pHead->AllocateHandles(nRequested);
}

//*****************************************************************************
// Release object handles allocated using AllocateHandles().
void LargeHeapHandleTable::ReleaseHandles(OBJECTREF *pObjRef, DWORD nReleased)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pObjRef));
    }
    CONTRACTL_END;

    // SEE "LOCKING RULES FOR AllocateHandles() and ReleaseHandles()" above

    // the lock must be registered and already held by the caller per contract
    _ASSERTE(m_pCrstDebug != NULL);
    _ASSERTE(m_pCrstDebug->OwnedByCurrentThread());

    OBJECTREF pPreallocatedSentinalObject = ObjectFromHandle(g_pPreallocatedSentinelObject);
    _ASSERTE(pPreallocatedSentinalObject  != NULL);


    // Add the released handles to the list of available handles.
    for (DWORD i = 0; i < nReleased; i++)
    {
         SetObjectReference(&pObjRef[i], pPreallocatedSentinalObject, NULL);
    }

    m_cEmbeddedFree += nReleased;
}


//*****************************************************************************
// BaseDomain
//*****************************************************************************
BaseDomain::BaseDomain()
{
     // initialize fields so the domain can be safely destructed
    // shouldn't call anything that can fail here - use ::Init instead
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;
    // initialize all members up front to NULL so that short-circuit failure won't cause invalid values
    m_InitialReservedMemForLoaderHeaps = NULL;
    m_pLowFrequencyHeap = NULL;
    m_pHighFrequencyHeap = NULL;
    m_pStubHeap = NULL;
    m_pFuncPtrStubs=NULL;

    m_pFusionContext = NULL;

    // Make sure the container is set to NULL so that it gets loaded when it is used.
    m_pLargeHeapHandleTable = NULL;

    // Note that m_hHandleTableBucket is overridden by app domains
    m_hHandleTableBucket = g_HandleTableMap.pBuckets[0];

    m_pStringLiteralMap = NULL;
    m_pMarshalingData = NULL;

    m_dwThreadStatics = 0;
    m_dwContextStatics = 0;

    m_pVirtualCallStubManager = NULL;
    m_FileLoadLock.PreInit();
    m_JITLock.PreInit();
    m_ClassInitLock.PreInit();
}

//*****************************************************************************
// Used by BaseDomain::Init for easier readability.
#ifdef PROFILING_SUPPORTED
#define LOADERHEAP_PROFILE_COUNTER (&(GetPrivatePerfCounters().m_Loading.cbLoaderHeapSize))
#else
#define LOADERHEAP_PROFILE_COUNTER (NULL)
#endif

//*****************************************************************************
void BaseDomain::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    //
    // Initialize the heaps
    //

    DWORD dwTotalReserveMemSize = LOW_FREQUENCY_HEAP_RESERVE_SIZE
                                + HIGH_FREQUENCY_HEAP_RESERVE_SIZE
                                + STUB_HEAP_RESERVE_SIZE;

    dwTotalReserveMemSize = (DWORD) ALIGN_UP(dwTotalReserveMemSize, VIRTUAL_ALLOC_RESERVE_GRANULARITY);

    BYTE * initReservedMem = ClrVirtualAllocExecutable(dwTotalReserveMemSize, MEM_RESERVE, PAGE_NOACCESS);

    m_InitialReservedMemForLoaderHeaps = initReservedMem;

    if (initReservedMem == NULL)
        COMPlusThrowOM();

    m_pLowFrequencyHeap = new (&m_LowFreqHeapInstance) LoaderHeap(LOW_FREQUENCY_HEAP_RESERVE_SIZE,
                                                                  LOW_FREQUENCY_HEAP_COMMIT_SIZE,
                                                                  initReservedMem,
                                                                  LOW_FREQUENCY_HEAP_RESERVE_SIZE,
                                                                  LOADERHEAP_PROFILE_COUNTER);
    initReservedMem += LOW_FREQUENCY_HEAP_RESERVE_SIZE;

    if (m_pLowFrequencyHeap == NULL)
        COMPlusThrowOM();

    m_pHighFrequencyHeap = new (&m_HighFreqHeapInstance) LoaderHeap(HIGH_FREQUENCY_HEAP_RESERVE_SIZE,
                                                                    HIGH_FREQUENCY_HEAP_COMMIT_SIZE,
                                                                    initReservedMem,
                                                                    HIGH_FREQUENCY_HEAP_RESERVE_SIZE,
                                                                    LOADERHEAP_PROFILE_COUNTER,
                                                                    MethodDescPrestubManager::g_pManager->GetRangeList(),
                                                                    TRUE);
    initReservedMem += HIGH_FREQUENCY_HEAP_RESERVE_SIZE;

    if (m_pHighFrequencyHeap == NULL)
        COMPlusThrowOM();

#if defined(_DEBUG) && defined(STUBLINKER_GENERATES_UNWIND_INFO)
    m_pHighFrequencyHeap->m_fPermitStubsWithUnwindInfo = TRUE;
#endif

    m_pStubHeap = new (&m_StubHeapInstance) LoaderHeap(STUB_HEAP_RESERVE_SIZE,
                                                       STUB_HEAP_COMMIT_SIZE,
                                                       initReservedMem,
                                                       STUB_HEAP_RESERVE_SIZE,
                                                       LOADERHEAP_PROFILE_COUNTER,
                                                       StubLinkStubManager::g_pManager->GetRangeList(),
                                                       TRUE);

    initReservedMem += STUB_HEAP_RESERVE_SIZE;

    if (m_pStubHeap == NULL)
        COMPlusThrowOM();

#if defined(_DEBUG) && defined(STUBLINKER_GENERATES_UNWIND_INFO)
    m_pStubHeap->m_fPermitStubsWithUnwindInfo = TRUE;
#endif

    // Used by GetMultiCallableAddrOfCode
    m_pFuncPtrStubs = new FuncPtrStubs();

    //
    // Initialize the domain locks
    //

    if (this == (BaseDomain*) g_pSharedDomainMemory)
        m_DomainCrst.Init("SharedBaseDomain", CrstSharedBaseDomain);
    else if (this == (BaseDomain*) g_pSystemDomainMemory)
        m_DomainCrst.Init("SystemBaseDomain", CrstSystemBaseDomain);
    else
        m_DomainCrst.Init("BaseDomain", CrstBaseDomain);

    m_DomainCacheCrst.Init("AppDomainCache", CrstAppDomainCache);
    m_DomainLocalBlockCrst.Init("DomainLocalBlock", CrstDomainLocalBlock, CRST_REENTRANCY);

    m_InteropDataCrst.Init("InteropData", CrstInteropData, CRST_REENTRANCY);

    m_FileLoadLock.Init("AppDomainAssembly", CrstAssemblyLoader,
                        CrstFlags(CRST_UNSAFE_COOPGC | CRST_HOST_BREAKABLE), TRUE);

    m_JITLock.Init("JitLock", CrstClassInit, CrstFlags(CRST_REENTRANCY | CRST_UNSAFE_SAMELEVEL), TRUE);
    m_ClassInitLock.Init("ClassInitLock", CrstClassInit, CrstFlags(CRST_REENTRANCY | CRST_UNSAFE_SAMELEVEL), TRUE);


    // Large heap handle table CRST.
    m_LargeHeapHandleTableCrst.Init("CrstAppDomainLargeHeapHandleTable", CrstAppDomainHandleTable);

    // Initialize the EE marshaling data to NULL.
    m_pMarshalingData = NULL;



    // Init the COM Interop data hash
    {
        LockOwner lock = {&m_InteropDataCrst, IsOwnerOfCrst};
        m_interopDataHash.Init(0, NULL, false, &lock);
    }
}

#undef LOADERHEAP_PROFILE_COUNTER

//*****************************************************************************
void BaseDomain::LazyInitStringLiteralMap()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // The AppDomain specific string literal map.
    AppDomainStringLiteralMap *pStringLiteralMap = new AppDomainStringLiteralMap(this);

    pStringLiteralMap->Init();
    if (FastInterlockCompareExchangePointer((void**)&m_pStringLiteralMap, pStringLiteralMap, NULL) != NULL)
    {
        // Somone beat us to it, clean up this instance
        delete pStringLiteralMap;
    }
}

//*****************************************************************************
void BaseDomain::Terminate()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

//     LOG((
//         LF_CLASSLOADER,
//         INFO3,
//         "Deleting Domain %x\n"
//         "LowFrequencyHeap:    %10d bytes\n"
//         "  >Loaderheap waste: %10d bytes\n"
//         "HighFrequencyHeap:   %10d bytes\n"
//         "  >Loaderheap waste: %10d bytes\n",
//         "StubHeap:            %10d bytes\n"
//         "  >Loaderheap waste: %10d bytes\n",
//         this,
//         m_pLowFrequencyHeap->m_dwDebugTotalAlloc,
//         m_pLowFrequencyHeap->DebugGetWastedBytes(),
//         m_pHighFrequencyHeap->m_dwDebugTotalAlloc,
//         m_pHighFrequencyHeap->DebugGetWastedBytes(),
//         m_pStubHeap->m_dwDebugTotalAlloc,
//         m_pStubHeap->DebugGetWastedBytes()
//     ));


    if (m_pLowFrequencyHeap != NULL)
    {
        delete(m_pLowFrequencyHeap);
        m_pLowFrequencyHeap = NULL;
    }

    if (m_pHighFrequencyHeap != NULL)
    {
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
        UnregisterUnwindInfoInLoaderHeap(m_pHighFrequencyHeap);
#endif

        delete(m_pHighFrequencyHeap);
        m_pHighFrequencyHeap = NULL;
    }

    if (m_pStubHeap != NULL)
    {
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
        UnregisterUnwindInfoInLoaderHeap(m_pStubHeap);
#endif

        delete(m_pStubHeap);
        m_pStubHeap = NULL;
    }

    if (m_pFuncPtrStubs != NULL)
    {
        delete m_pFuncPtrStubs;
        m_pFuncPtrStubs = NULL;
    }

    m_DomainCrst.Destroy();
    m_DomainCacheCrst.Destroy();
    m_DomainLocalBlockCrst.Destroy();
    m_InteropDataCrst.Destroy();

    ListLockEntry* pElement;

    // All the threads that are in this domain had better be stopped by this
    // point.
    //
    // We might be jitting or running a .cctor so we need to empty that queue.
    pElement = m_JITLock.Pop(TRUE);
    while (pElement)
    {
#ifdef STRICT_JITLOCK_ENTRY_LEAK_DETECTION
        _ASSERTE ((m_JITLock.m_pHead->m_dwRefCount == 1
            && m_JITLock.m_pHead->m_hrResultCode == E_FAIL) ||
            dbg_fDrasticShutdown || g_fInControlC);
#endif // STRICT_JITLOCK_ENTRY_LEAK_DETECTION
        delete(pElement);
        pElement = m_JITLock.Pop(TRUE);

    }
    m_JITLock.Destroy();

    pElement = m_ClassInitLock.Pop(TRUE);
    while (pElement)
    {
#ifdef STRICT_CLSINITLOCK_ENTRY_LEAK_DETECTION
        _ASSERTE (dbg_fDrasticShutdown || g_fInControlC);
#endif
        delete(pElement);
        pElement = m_ClassInitLock.Pop(TRUE);
    }
    m_ClassInitLock.Destroy();

    FileLoadLock* pFileElement;
    pFileElement = (FileLoadLock*) m_FileLoadLock.Pop(TRUE);
    while (pFileElement)
    {
#ifdef STRICT_CLSINITLOCK_ENTRY_LEAK_DETECTION
        _ASSERTE (dbg_fDrasticShutdown || g_fInControlC);
#endif
        pFileElement->Release();
        pFileElement = (FileLoadLock*) m_FileLoadLock.Pop(TRUE);
    }
    m_FileLoadLock.Destroy();



    m_LargeHeapHandleTableCrst.Destroy();

    if (m_pLargeHeapHandleTable != NULL)
    {
        delete m_pLargeHeapHandleTable;
        m_pLargeHeapHandleTable = NULL;
    }

    if (!IsAppDomain())
    {

        if (m_pStringLiteralMap != NULL)
        {
            delete m_pStringLiteralMap;
            m_pStringLiteralMap = NULL;
        }
    }



    // This was the block reserved by BaseDomain::Init for the loaderheaps.
    if (m_InitialReservedMemForLoaderHeaps)
        ClrVirtualFree (m_InitialReservedMemForLoaderHeaps, 0, MEM_RELEASE);

    ClearFusionContext();

}

void BaseDomain::InitVirtualCallStubManager()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    m_typeIDMap.Init(IsSharedDomain() ? TypeIDMap::STARTING_SHARED_DOMAIN_ID :
                                        TypeIDMap::STARTING_UNSHARED_DOMAIN_ID,
                     2);

    NewHolder<VirtualCallStubManager> pMgr(new  VirtualCallStubManager());

    // Init the manager, including all heaps and such.
    pMgr->Init(this);

    // Set the pointer. No need to AddRef, as it comes pre-AddRef'd when
    // created. It will be Release'd when we destruct the AppDomain.
    SetVirtualCallStubManager(pMgr);

    // Successfully created the manager.
    pMgr.SuppressRelease();
}

BOOL BaseDomain::ContainsOBJECTHANDLE(OBJECTHANDLE handle)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return Ref_ContainHandle(m_hHandleTableBucket,handle);
}

DWORD BaseDomain::AllocateThreadOrContextStaticsOffset(DWORD* pOffsetSlot, BOOL fContextStatics)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    CrstPreempHolder ch(&m_SpecialStaticsCrst);

    DWORD dwOffset = *pOffsetSlot;

    if (dwOffset == (DWORD)-1)
    {
        // Allocate the slot
        if (fContextStatics)
            dwOffset = m_dwContextStatics++;
        else
            dwOffset = m_dwThreadStatics++;

        *pOffsetSlot = dwOffset;
    }

    return dwOffset;
}

void BaseDomain::ClearFusionContext()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END;

    if(m_pFusionContext) {
        m_pFusionContext->Release();
        m_pFusionContext = NULL;
    }
}


void AppDomain::ShutdownAssemblies()
{
    LEAF_CONTRACT;
    // Shutdown assemblies
    AssemblyIterator i = IterateAssembliesEx( (AssemblyIterationFlags)(kIncludeLoaded | kIncludeLoading | kIncludeExecution | kIncludeIntrospection | kIncludeFailedToLoad) );

    while (i.Next())
    {
        delete i.GetDomainAssembly();
    }

    m_Assemblies.Clear();
}

void AppDomain::ReleaseDomainBoundInfo()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;;
    // Shutdown assemblies
    m_AssemblyCache.OnAppDomainUnload();

    AssemblyIterator i = IterateAssembliesEx( (AssemblyIterationFlags)(kIncludeFailedToLoad) );

    while (i.Next())
       i.GetDomainAssembly()->ReleaseManagedData();
}

void AppDomain::ReleaseFiles()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;;
    // Shutdown assemblies
    AssemblyIterator i = IterateAssembliesEx( (AssemblyIterationFlags)(kIncludeLoaded  | kIncludeExecution | kIncludeIntrospection | kIncludeFailedToLoad| kIncludeLoading) );

    while (i.Next())
    {
        DomainAssembly* pAsm=i.GetDomainAssembly();
        if (pAsm->GetCurrentAssembly()==NULL)
        {
            //might be domain neutral or not, but should have no live objects as it has not been
            // really loaded yet. Just nuke it
            m_Assemblies.Set(i.GetIndex(),NULL);
            delete pAsm;
        }
        else 
        if(!pAsm->GetCurrentAssembly()->IsDomainNeutral())
             pAsm->ReleaseFiles();
    }
}


OBJECTREF* BaseDomain::AllocateObjRefPtrsInLargeTable(int nRequested, OBJECTREF** ppLazyAllocate)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION((nRequested > 0));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (ppLazyAllocate && *ppLazyAllocate)
    {
        // Allocation already happened
        return *ppLazyAllocate;
    }

    // Enter preemptive state, take the lock and go back to cooperative mode.
    {
        CrstPreempHolder ch(&m_LargeHeapHandleTableCrst);
        GCX_COOP();

        if (ppLazyAllocate && *ppLazyAllocate)
        {
            // Allocation already happened
            return *ppLazyAllocate;
        }

        // Make sure the large heap handle table is initialized.
        if (!m_pLargeHeapHandleTable)
            InitLargeHeapHandleTable();

        // Allocate the handles.
        OBJECTREF* result = m_pLargeHeapHandleTable->AllocateHandles(nRequested);

        if (ppLazyAllocate)
        {
            *ppLazyAllocate = result;
        }

        return result;
    }
}

#endif // !DACCESS_COMPILE

/*static*/
BaseDomain* BaseDomain::ComputeBaseDomain(BaseDomain *pGenericDefinitionDomain,   // the domain that owns the generic type or method
                                          DWORD numGenericClassArgs,              // the number of type arguments to the type
                                          TypeHandle *pGenericClassArgs,          // the type arguments to the type (if any)
                                          DWORD numGenericMethodArgs,             // the number of type arguments to the method
                                          TypeHandle *pGenericMethodArgs)         // the type arguments to the method (if any)
{
    CONTRACT(BaseDomain*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
        PRECONDITION(CheckPointer(pGenericDefinitionDomain, NULL_OK));
        PRECONDITION(CheckPointer(pGenericClassArgs, NULL_OK));
        PRECONDITION(CheckPointer(pGenericMethodArgs, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END

    if (pGenericDefinitionDomain && pGenericDefinitionDomain->IsAppDomain())
        RETURN pGenericDefinitionDomain;

    for (DWORD i = 0; i < numGenericClassArgs; i++)
    {
        BaseDomain *pArgDomain = pGenericClassArgs[i].GetDomain();
        if (pArgDomain->IsAppDomain())
            RETURN pArgDomain;
    }

    for (DWORD i = 0; i < numGenericMethodArgs; i++)
    {
        BaseDomain *pArgDomain = pGenericMethodArgs[i].GetDomain();
        if (pArgDomain->IsAppDomain())
            RETURN pArgDomain;
    }
    RETURN (pGenericDefinitionDomain ? pGenericDefinitionDomain : SystemDomain::System());
}

BaseDomain* BaseDomain::ComputeBaseDomain(TypeKey *pKey)
{
    WRAPPER_CONTRACT;

    if (pKey->GetKind() == ELEMENT_TYPE_CLASS)
        return BaseDomain::ComputeBaseDomain(pKey->GetModule()->GetDomain(),pKey->GetNumGenericArgs(),pKey->GetInstantiation());
    else if (pKey->GetKind() != ELEMENT_TYPE_FNPTR)
        return pKey->GetElementType().GetDomain();
    else
        return BaseDomain::ComputeBaseDomain(NULL,pKey->GetNumArgs()+1,pKey->GetRetAndArgTypes());
}





#ifndef DACCESS_COMPILE



// Insert class in the hash table
void AppDomain::InsertClassForCLSID(MethodTable* pMT, BOOL fForceInsert /*=FALSE*/)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_ANY;
        THROWS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    CVID cvid;

    // Ensure that registered classes are activated for allocation
    pMT->EnsureInstanceActive();

    // Note that it is possible for multiple classes to claim the same CLSID, and in such a
    // case it is arbitrary which one we will return for a future query for a given app domain.

    pMT->GetGuid(&cvid, fForceInsert);

    if (!IsEqualIID(cvid, GUID_NULL))
    {
        LPVOID val = (LPVOID)pMT;
        {
            LockHolder lh(this);

            if (LookupClass(cvid) != pMT)
            {
                m_clsidHash.InsertValue(GetKeyFromGUID(&cvid), val);
            }
        }
    }
}

void AppDomain::InsertClassForCLSID(MethodTable* pMT, GUID *pGuid)
{
    CONTRACT_VOID
    {
        NOTHROW;
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(CheckPointer(pGuid));
    }
    CONTRACT_END;

    LPVOID val = (LPVOID)pMT;
    {
        LockHolder lh(this);

        CVID* cvid = pGuid;
        if (LookupClass(*cvid) != pMT)
        {
            m_clsidHash.InsertValue(GetKeyFromGUID(pGuid), val);
        }
    }

    RETURN;
}

EEMarshalingData *BaseDomain::GetMarshalingData()
{
    CONTRACT (EEMarshalingData*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        POSTCONDITION(CheckPointer(m_pMarshalingData));
    }
    CONTRACT_END;

    if (!m_pMarshalingData)
    {
        // Take the lock
        CrstHolder holder(&m_InteropDataCrst);

        if (!m_pMarshalingData)
        {
            LoaderHeap *pHeap = GetLowFrequencyHeap();
            m_pMarshalingData = new (pHeap) EEMarshalingData(this, pHeap, &m_DomainCrst);
        }
    }

    RETURN m_pMarshalingData;
}

STRINGREF *BaseDomain::GetStringObjRefPtrFromUnicodeString(EEStringData *pStringData)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pStringData));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    if (m_pStringLiteralMap == NULL)
    {
        LazyInitStringLiteralMap();
    }
    _ASSERTE(m_pStringLiteralMap);
    return m_pStringLiteralMap->GetStringLiteral(pStringData, TRUE, !CanUnload() /* bAppDOmainWontUnload */);
}

STRINGREF *BaseDomain::IsStringInterned(STRINGREF *pString)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pString));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    if (m_pStringLiteralMap == NULL)
    {
        LazyInitStringLiteralMap();
    }
    _ASSERTE(m_pStringLiteralMap);
    return m_pStringLiteralMap->GetInternedString(pString, FALSE, !CanUnload() /* bAppDOmainWontUnload */);
}

STRINGREF *BaseDomain::GetOrInternString(STRINGREF *pString)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pString));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    if (m_pStringLiteralMap == NULL)
    {
        LazyInitStringLiteralMap();
    }
    _ASSERTE(m_pStringLiteralMap);
    return m_pStringLiteralMap->GetInternedString(pString, TRUE, !CanUnload() /* bAppDOmainWontUnload */);
}

void BaseDomain::InitLargeHeapHandleTable()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(m_pLargeHeapHandleTable==NULL);
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    m_pLargeHeapHandleTable = new LargeHeapHandleTable(this, STATIC_OBJECT_TABLE_BUCKET_SIZE);

#ifdef _DEBUG
    m_pLargeHeapHandleTable->RegisterCrstDebug(&m_LargeHeapHandleTableCrst);
#endif
}


//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

void *SystemDomain::operator new(size_t size, void *pInPlace)
{
    LEAF_CONTRACT;
    return pInPlace;
}


void SystemDomain::operator delete(void *pMem)
{
    LEAF_CONTRACT;
    // Do nothing - new() was in-place
}


void    SystemDomain::SetCompilationOverrides(BOOL fForceDebug,
                                            BOOL fForceProfiling,
                                            BOOL fForceInstrument)
{
    LEAF_CONTRACT;
    s_fForceDebug = fForceDebug;
    s_fForceProfiling = fForceProfiling;
    s_fForceInstrument = fForceInstrument;
}

void    SystemDomain::GetCompilationOverrides(BOOL * fForceDebug,
                                            BOOL * fForceProfiling,
                                            BOOL * fForceInstrument)
{
    LEAF_CONTRACT;
    *fForceDebug = s_fForceDebug;
    *fForceProfiling = s_fForceProfiling;
    *fForceInstrument = s_fForceInstrument;
}

void SystemDomain::Attach()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(m_pSystemDomain == NULL);
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if(m_pSystemDomain != NULL)
       COMPlusThrowHR(COR_E_EXECUTIONENGINE);

    // Initialize stub managers
    MethodDescPrestubManager::Init();
    DelegateInvokeStubManager::Init();
    JumpStubStubManager::Init();
    StubLinkStubManager::Init();


    ThunkHeapStubManager::Init();

    m_appDomainIndexList.Init();
    m_appDomainIdList.Init();

    m_SystemDomainCrst.Init("SystemDomain", CrstSystemDomain, CRST_REENTRANCY);
    m_DelayedUnloadCrst.Init("SystemDomainDelayedUnloadList", CrstSystemDomainDelayedUnloadList, CRST_UNSAFE_COOPGC);


    m_SpecialStaticsCrst.Init("SpecialStatics", CrstSpecialStatics);

    // Create the global SystemDomain and initialize it.
    m_pSystemDomain = new (&g_pSystemDomainMemory) SystemDomain();
    // No way it can fail since g_pSystemDomainMemory is a static array.
    CONSISTENCY_CHECK(CheckPointer(m_pSystemDomain));

    LOG((LF_CLASSLOADER,
         LL_INFO10,
         "Created system domain at %x\n",
         m_pSystemDomain));

    // We need to initialize the memory pools etc. for the system domain.
    m_pSystemDomain->BaseDomain::Init(); // Setup the memory heaps

    // Create the default domain
    m_pSystemDomain->CreateDefaultDomain();
    SharedDomain::Attach();
 }


void SystemDomain::DetachBegin()
{
    WRAPPER_CONTRACT;

    if (GetThread() == NULL)
    {
        return;
    }

    if(m_pSystemDomain)
        m_pSystemDomain->Stop();
}

void SystemDomain::DetachEnd()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    // Shut down the domain and its children (but don't deallocate anything just
    // yet).
    if(m_pSystemDomain)
    {
        GCX_PREEMP();
        m_pSystemDomain->ClearFusionContext();
        if (m_pSystemDomain->m_pDefaultDomain)
            m_pSystemDomain->m_pDefaultDomain->ClearFusionContext();
    }
}

void SystemDomain::Stop()
{
    WRAPPER_CONTRACT;
    AppDomainIterator i(TRUE);

    while (i.Next())
        if (i.GetDomain()->m_Stage < AppDomain::STAGE_CLEARED)
            i.GetDomain()->Stop();
}


void SystemDomain::Terminate() // bNotifyProfiler is ignored
{
    // This ignores the refences and terminates the appdomains
    AppDomainIterator i(FALSE);

    while (i.Next())
    {
        delete i.GetDomain();
        // Keep the iterator from Releasing the current domain
        i.m_pCurrent = NULL;
    }

    if (m_pSystemFile != NULL) {
        m_pSystemFile->Release();
        m_pSystemFile = NULL;
    }

    m_pSystemAssembly = NULL;

    if(m_pwDevpath) {
        delete[] m_pwDevpath;
        m_pwDevpath = NULL;
    }
    m_dwDevpath = 0;
    m_fDevpath = FALSE;

    if (m_pGlobalStringLiteralMap) {
        delete m_pGlobalStringLiteralMap;
        m_pGlobalStringLiteralMap = NULL;
    }


    SharedDomain::Detach();

    BaseDomain::Terminate();

}


void SystemDomain::PreallocateSpecialObjects()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (g_pPreallocatedSentinelObject == NULL)
        g_pPreallocatedSentinelObject = CreatePinningHandle( NULL );

    if (ObjectFromHandle(g_pPreallocatedSentinelObject) == NULL)
    {
        OBJECTREF pPreallocatedSentinalObject = AllocateObject(g_pObjectClass);
#if CHECK_APP_DOMAIN_LEAKS
        pPreallocatedSentinalObject->SetSyncBlockAppDomainAgile();
#endif
        StoreObjectInHandle(g_pPreallocatedSentinelObject, pPreallocatedSentinalObject);
    }
}

void SystemDomain::CreatePreallocatedExceptions()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    EXCEPTIONREF pOutOfMemory = (EXCEPTIONREF)AllocateObject(g_pOutOfMemoryExceptionClass);
    pOutOfMemory->SetHResult(COR_E_OUTOFMEMORY);
    pOutOfMemory->SetXCode(EXCEPTION_COMPLUS);

    _ASSERTE(g_pPreallocatedOutOfMemoryException == NULL);
    g_pPreallocatedOutOfMemoryException = CreateHandle(pOutOfMemory);


    EXCEPTIONREF pStackOverflow = (EXCEPTIONREF)AllocateObject(g_pStackOverflowExceptionClass);
    pStackOverflow->SetHResult(COR_E_STACKOVERFLOW);
    pStackOverflow->SetXCode(EXCEPTION_COMPLUS);

    _ASSERTE(g_pPreallocatedStackOverflowException == NULL);
    g_pPreallocatedStackOverflowException = CreateHandle(pStackOverflow);


    EXCEPTIONREF pExecutionEngine = (EXCEPTIONREF)AllocateObject(g_pExecutionEngineExceptionClass);
    pExecutionEngine->SetHResult(COR_E_EXECUTIONENGINE);
    pExecutionEngine->SetXCode(EXCEPTION_COMPLUS);

    _ASSERTE(g_pPreallocatedExecutionEngineException == NULL);
    g_pPreallocatedExecutionEngineException = CreateHandle(pExecutionEngine);


    EXCEPTIONREF pRudeAbortException = (EXCEPTIONREF)AllocateObject(g_pThreadAbortExceptionClass);
#if CHECK_APP_DOMAIN_LEAKS
    pRudeAbortException->SetSyncBlockAppDomainAgile();
#endif
    pRudeAbortException->SetHResult(COR_E_THREADABORTED);
    pRudeAbortException->SetXCode(EXCEPTION_COMPLUS);

    _ASSERTE(g_pPreallocatedRudeThreadAbortException == NULL);
    g_pPreallocatedRudeThreadAbortException = CreateHandle(pRudeAbortException);


    EXCEPTIONREF pAbortException = (EXCEPTIONREF)AllocateObject(g_pThreadAbortExceptionClass);
#if CHECK_APP_DOMAIN_LEAKS
    pAbortException->SetSyncBlockAppDomainAgile();
#endif
    pAbortException->SetHResult(COR_E_THREADABORTED);
    pAbortException->SetXCode(EXCEPTION_COMPLUS);

    _ASSERTE(g_pPreallocatedThreadAbortException == NULL);
    g_pPreallocatedThreadAbortException = CreateHandle( pAbortException );
}


void SystemDomain::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

#ifdef _DEBUG
    LOG((
        LF_EEMEM,
        LL_INFO10,
        "sizeof(EEClass)     = %d\n"
        "sizeof(MethodTable) = %d\n"
        "sizeof(MethodDesc)= %d\n"
        "sizeof(FieldDesc)   = %d\n"
        "sizeof(Module)      = %d\n",
        sizeof(EEClass),
        sizeof(MethodTable),
        sizeof(MethodDesc),
        sizeof(FieldDesc),
        sizeof(Module)
    ));
#endif // _DEBUG

    // The base domain is initialized in SystemDomain::Attach()
    // to allow stub caches to use the memory pool. Do not
    // initialze it here!

#ifdef _DEBUG
    Context     *curCtx = GetCurrentContext();
#endif
    _ASSERTE(curCtx);
    _ASSERTE(curCtx->GetDomain() != NULL);

#ifdef _DEBUG
    g_fVerifierOff = g_pConfig->IsVerifierOff();
#endif

    m_pSystemFile = NULL;
    m_pSystemAssembly = NULL;

    // Get the install directory so we can find mscorlib
    DWORD size = 0;
    hr = GetInternalSystemDirectory(NULL, &size);
    if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
        ThrowHR(hr);

    // GetInternalSystemDirectory returns a size, including the null!
    WCHAR *buffer = m_SystemDirectory.OpenUnicodeBuffer(size-1);
    IfFailThrow(GetInternalSystemDirectory(buffer, &size));
    m_SystemDirectory.CloseBuffer();
    m_SystemDirectory.Normalize();

    m_BaseLibrary.Append(m_SystemDirectory);
    m_BaseLibrary.Append(g_pwBaseLibrary);
    m_BaseLibrary.Normalize();

    {
        // We are about to start allocating objects, so we must be in cooperative mode.
        // However, many of the entrypoints to the system (DllGetClassObject and all
        // N/Direct exports) get called multiple times.  Sometimes they initialize the EE,
        // but generally they remain in preemptive mode.  So we really want to push/pop
        // the state here:
        GCX_COOP();

        LoadBaseSystemClasses();

        CreatePreallocatedExceptions();

        PreallocateSpecialObjects();

        // Finish loading mscorlib now.
        m_pSystemAssembly->GetDomainAssembly()->EnsureActive();
    }

#ifdef _DEBUG
    BOOL fPause = EEConfig::GetConfigDWORD(L"PauseOnLoad", FALSE);

    while(fPause)
    {
        ClrSleepEx(20, TRUE);
    }
#endif // _DEBUG
}

void SystemDomain::LazyInitGlobalStringLiteralMap()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Allocate the global string literal map.
    GlobalStringLiteralMap *pGlobalStringLiteralMap = new GlobalStringLiteralMap();

    // Initialize the global string literal map.
    pGlobalStringLiteralMap->Init();

    if (FastInterlockCompareExchangePointer((void**)&m_pGlobalStringLiteralMap, pGlobalStringLiteralMap, NULL) != NULL)
    {
        // Somone beat us to it, clean up this instance
        delete pGlobalStringLiteralMap;
    }
}

void AppDomain::CreateADUnloadStartEvent()
{
    WRAPPER_CONTRACT;

    g_pUnloadStartEvent = new CLREvent();
    g_pUnloadStartEvent->CreateAutoEvent(FALSE);
}



/*static*/ void SystemDomain::EnumAllStaticGCRefs(GCEnumCallback  pCallback, LPVOID hCallBack)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        if (GetThread() != NULL) {MODE_COOPERATIVE;} else {MODE_ANY;}
    }
    CONTRACT_END;

    // We don't do a normal AppDomainIterator because we can't take the SystemDomain lock from
    // here.
    // We're only supposed to call this from a Server GC. We're walking here m_appDomainIdList
    // m_appDomainIdList will have an AppDomain* or will be NULL. So the only danger is if we
    // Fetch an AppDomain and then in some other thread the AppDomain is deleted.
    //
    // If the thread deleting the AppDomain (AppDomain::~AppDomain)was in Preemptive mode
    // while doing SystemDomain::EnumAllStaticGCRefs we will issue a GCX_COOP(), which will wait
    // for the GC to finish, so we are safe
    //
    // If the thread is in cooperative mode, it must have been suspended for the GC so a delete
    // can't happen.

    _ASSERTE(GCHeap::IsGCInProgress() &&
             GCHeap::IsServerHeap()   &&
             IsGCSpecialThread());

    SystemDomain* sysDomain = SystemDomain::System();
    if (sysDomain)
    {
        DWORD i;
        DWORD count = (DWORD) m_appDomainIdList.GetCount();
        for (i = 0 ; i < count ; i++)
        {
            AppDomain* pAppDomain = (AppDomain *)m_appDomainIdList.Get(i);
            if (pAppDomain && pAppDomain->IsActive() && !pAppDomain->IsUnloading())
            {
                pAppDomain->EnumStaticGCRefs(pCallback, hCallBack);
            }
        }
    }

    RETURN;
}

void SystemDomain::LoadBaseSystemClasses()
{
    CONTRACT_VOID
    {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    ETWTraceStartup trace(ETW_TYPE_STARTUP_LD_SYS_BASES);
    TIMELINE_AUTO(STARTUP, "SystemDomain::LoadBaseSystemClasses");

    MEMORY_REPORT_CONTEXT_SCOPE("LoadBaseSystemClasses");

    {
        ETWTraceStartup traceFusion(ETW_TYPE_STARTUP_FUSIONAPPCTX);
        // Setup fusion context for the system domain - this is used for binding mscorlib.
        IfFailThrow(FusionBind::SetupFusionContext(m_SystemDirectory, NULL, &m_pFusionContext));

        m_pSystemFile = PEAssembly::OpenSystem();
    }
    // Only partially load the system assembly. Other parts of the code will want to access
    // the globals in this function before finishing the load.
    m_pSystemAssembly = DefaultDomain()->LoadDomainAssembly(NULL, m_pSystemFile, FILE_LOAD_POST_LOADLIBRARY,
                                                            NULL, NULL)->GetCurrentAssembly();
    m_pSystemFile->LoadLibrary();

    // must set this to null before loading classes because class loader will use it
    g_pDelegateClass = NULL;
    g_pMultiDelegateClass = NULL;

    // Set up binder for mscorlib
    Binder::StartupMscorlib(m_pSystemAssembly->GetManifestModule());

    // Load Object
    g_pObjectClass = g_Mscorlib.FetchClass(CLASS__OBJECT);

    // get the Object::.ctor method desc so we can special-case it
    g_pObjectCtorMD = g_Mscorlib.FetchMethod(METHOD__OBJECT__CTOR);

    // Now that ObjectClass is loaded, we can set up
    // the system for finalizers.  There is no point in deferring this, since we need
    // to know this before we allocate our first object.
    g_pObjectFinalizerMD = g_Mscorlib.FetchMethod(METHOD__OBJECT__FINALIZE);


    g_pHiddenMethodTableClass = g_Mscorlib.FetchClass(CLASS____CANON);

    // NOTE: !!!IMPORTANT!!! ValueType and Enum MUST be loaded one immediately after
    //                       the other, because we have coded MethodTable::IsChildValueType
    //                       in such a way that it depends on this behaviour.
    // Load the ValueType class
    g_pValueTypeClass = g_Mscorlib.FetchClass(CLASS__VALUE_TYPE);

    // Load the enum class
    g_pEnumClass = g_Mscorlib.FetchClass(CLASS__ENUM);
    _ASSERTE(!g_pEnumClass->IsValueClass());

    // Load Array class
    g_pArrayClass = g_Mscorlib.FetchClass(CLASS__ARRAY);

    // Load Nullable class
    g_pNullableEEClass = g_Mscorlib.FetchClass(CLASS__NULLABLE)->GetClass();

    // Load the Object array class.
    g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT] = g_Mscorlib.FetchType(TYPE__OBJECT_ARRAY).AsArray();

    // We have delayed allocation of mscorlib's static handles until we load the object class
    m_pSystemAssembly->GetManifestModule()->AllocateStaticHandles(DefaultDomain());

    // Load String
    g_pStringClass = g_Mscorlib.FetchClass(CLASS__STRING);

    _ASSERTE(g_pStringClass->GetBaseSize() == ObjSizeOf(StringObject));
    _ASSERTE(g_pStringClass->GetComponentSize() == 2);

    ECall::PopulateManagedStringConstructors();
    ECall::PopulateDynamicallyAssignedFCalls();

    if (CLRIoCompletionHosted())
    {
        g_pOverlappedDataClass = g_Mscorlib.FetchClass(CLASS__OVERLAPPEDDATA);
        _ASSERTE (g_pOverlappedDataClass);
#ifndef DACCESS_COMPILE
        if (CorHost2::GetHostOverlappedExtensionSize() != 0)
        {
            // Overlapped may have an extension if a host hosts IO completion subsystem
            DWORD instanceFieldBytes = g_pOverlappedDataClass->GetNumInstanceFieldBytes() + CorHost2::GetHostOverlappedExtensionSize();
            g_pOverlappedDataClass->GetClass()->SetNumInstanceFieldBytes(instanceFieldBytes);
            _ASSERTE (instanceFieldBytes + ObjSizeOf(Object) >= MIN_OBJECT_SIZE);
            DWORD baseSize = (DWORD) (instanceFieldBytes + ObjSizeOf(Object));
            baseSize = (baseSize + ALLOC_ALIGN_CONSTANT) & ~ALLOC_ALIGN_CONSTANT;  // m_BaseSize must be aligned
            DWORD adjustSize = baseSize - g_pOverlappedDataClass->GetBaseSize();
            CGCDesc* map = CGCDesc::GetCGCDescFromMT(g_pOverlappedDataClass);
            CGCDescSeries * cur = map->GetHighestSeries();
            _ASSERTE ((SSIZE_T)map->GetNumSeries() == 1);
            cur->SetSeriesSize(cur->GetSeriesSize() - adjustSize);
            g_pOverlappedDataClass->SetBaseSize(baseSize);
        }
#endif // !DACCESS_COMPILE
    }

    IfFailThrow(COMStringBuffer::LoadStringBuffer());

    g_pExceptionClass = g_Mscorlib.FetchClass(CLASS__EXCEPTION);
    g_pOutOfMemoryExceptionClass = g_Mscorlib.GetException(kOutOfMemoryException);
    g_pStackOverflowExceptionClass = g_Mscorlib.GetException(kStackOverflowException);
    g_pExecutionEngineExceptionClass = g_Mscorlib.GetException(kExecutionEngineException);
    g_pThreadAbortExceptionClass = g_Mscorlib.GetException(kThreadAbortException);

    g_pDelegateClass = g_Mscorlib.FetchClass(CLASS__DELEGATE);
    g_pMultiDelegateClass = g_Mscorlib.FetchClass(CLASS__MULTICAST_DELEGATE);

    // Used for determining whether a class has a critical finalizer
    // To determine whether a class has a critical finalizer, we
    // currently will simply see if it's parent class has a critical
    // finalizer. To introduce a class with a critical finalizer,
    // we'll explicitly load CriticalFinalizerObject and set the bit
    // here.
    g_pCriticalFinalizerObjectClass = g_Mscorlib.FetchClass(CLASS__CRITICAL_FINALIZER_OBJECT);
    _ASSERTE(g_pCriticalFinalizerObjectClass->HasCriticalFinalizer());

    // used by gc to handle predefined agility checking
    g_pThreadClass = g_Mscorlib.FetchClass(CLASS__THREAD);

    // Load a special marker method used to detect Constrained Execution Regions
    // at jit time.
    g_pPrepareConstrainedRegionsMethod = g_Mscorlib.FetchMethod(METHOD__RUNTIME_HELPERS__PREPARE_CONSTRAINED_REGIONS);
    g_pPrepareConstrainedRegionsNoOpMethod = g_Mscorlib.FetchMethod(METHOD__RUNTIME_HELPERS__PREPARE_CONSTRAINED_REGIONS_NOOP);
    g_pExecuteBackoutCodeHelperMethod = g_Mscorlib.FetchMethod(METHOD__RUNTIME_HELPERS__EXECUTE_BACKOUT_CODE_HELPER);

#ifdef _DEBUG
    Binder::CheckMscorlib();
#endif


    RETURN;
}

/*static*/
void SystemDomain::LoadDomain(AppDomain *pDomain)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(System()));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    pDomain->SetCanUnload();    // by default can unload any domain
    SystemDomain::System()->AddDomain(pDomain);
}



ADIndex SystemDomain::GetNewAppDomainIndex(AppDomain *pAppDomain)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        if (!g_fEEInit) { MODE_COOPERATIVE;} else { DISABLED(MODE_ANY);}
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    DWORD count = m_appDomainIndexList.GetCount();
    DWORD i;

#ifdef _DEBUG
    if (count < 2000)
    {
        // So that we can keep AD index inside object header.
        // We do not want to create syncblock unless needed.
        i = count;
    }
    else
    {
#endif // _DEBUG
        //
        // Look for an unused index.  Note that in a checked build,
        // we never reuse indexes - this makes it easier to tell
        // when we are looking at a stale app domain.
        //

        i = m_appDomainIndexList.FindElement(m_dwLowestFreeIndex, NULL);
        if (i == (DWORD) ArrayList::NOT_FOUND)
            i = count;
        m_dwLowestFreeIndex = i+1;
#ifdef _DEBUG
        if (m_dwLowestFreeIndex >= 2000)
        {
            m_dwLowestFreeIndex = 0;
        }
    }
#endif // _DEBUG

    if (i == count)
        IfFailThrow(m_appDomainIndexList.Append(pAppDomain));
    else
        m_appDomainIndexList.Set(i, pAppDomain);

    _ASSERTE(i < m_appDomainIndexList.GetCount());

    // Note that index 0 means domain agile.
    return ADIndex(i+1);
}

void SystemDomain::ReleaseAppDomainIndex(ADIndex index)
{
    LEAF_CONTRACT;
    SystemDomain::LockHolder lh;
    // Note that index 0 means domain agile.
    index.m_dwIndex--;

    _ASSERTE(m_appDomainIndexList.Get(index.m_dwIndex) != NULL);

    m_appDomainIndexList.Set(index.m_dwIndex, NULL);

#ifndef _DEBUG
    if (index.m_dwIndex < m_dwLowestFreeIndex)
        m_dwLowestFreeIndex = index.m_dwIndex;
#endif // !_DEBUG
}

#endif // !DACCESS_COMPILE

AppDomain *SystemDomain::GetAppDomainAtIndex(ADIndex index)
{
    LEAF_CONTRACT;
    _ASSERTE(index.m_dwIndex != 0);

    AppDomain *pAppDomain = TestGetAppDomainAtIndex(index);

    _ASSERTE(pAppDomain || !"Attempt to access unloaded app domain");

    return pAppDomain;
}

AppDomain *SystemDomain::TestGetAppDomainAtIndex(ADIndex index)
{
    LEAF_CONTRACT;
    _ASSERTE(index.m_dwIndex != 0);
    index.m_dwIndex--;

#ifndef DACCESS_COMPILE
    _ASSERTE(index.m_dwIndex < (DWORD)m_appDomainIndexList.GetCount());
    AppDomain *pAppDomain = (AppDomain*) m_appDomainIndexList.Get(index.m_dwIndex);
#else // DACCESS_COMPILE
    ArrayListStatic *pList = &m_appDomainIndexList;
    AppDomain *pAppDomain = PTR_AppDomain((TADDR)pList->Get(index.m_dwIndex));
#endif // DACCESS_COMPILE
    return pAppDomain;
}

#ifndef DACCESS_COMPILE

ADID SystemDomain::GetNewAppDomainId(AppDomain *pAppDomain)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    DWORD i = m_appDomainIdList.GetCount();

    IfFailThrow(m_appDomainIdList.Append(pAppDomain));

    _ASSERTE(i < m_appDomainIdList.GetCount());

    return ADID(i+1);
}

AppDomain *SystemDomain::GetAppDomainAtId(ADID index)
{
    CONTRACTL
    {
#ifdef _DEBUG
        if (!SystemDomain::IsUnderDomainLock()) { MODE_COOPERATIVE;} else { DISABLED(MODE_ANY);}
#endif
        GC_NOTRIGGER;
        SO_TOLERANT;
        NOTHROW;
    }
    CONTRACTL_END;

    if(index.m_dwId == 0)
        return NULL;
    DWORD requestedID = index.m_dwId - 1;

    if(requestedID  >= (DWORD)m_appDomainIdList.GetCount())
        return NULL;

    AppDomain * result = (AppDomain *)m_appDomainIdList.Get(requestedID);

    if(result==NULL && GetThread() == GCHeap::GetGCHeap()->GetFinalizerThread() &&
        SystemDomain::System()->AppDomainBeingUnloaded()!=NULL &&
        SystemDomain::System()->AppDomainBeingUnloaded()->GetId()==index)
        result=SystemDomain::System()->AppDomainBeingUnloaded();
    // If the current thread can't enter the AppDomain, then don't return it.
    if (!result || !result->CanThreadEnter(GetThread()))
        return NULL;

    return result;
}


void SystemDomain::ReleaseAppDomainId(ADID index)
{
    LEAF_CONTRACT;
    index.m_dwId--;

    _ASSERTE(index.m_dwId < (DWORD)m_appDomainIdList.GetCount());

    m_appDomainIdList.Set(index.m_dwId, NULL);
}


#ifdef _DEBUG
int g_fMainThreadApartmentStateSet = 0;
#endif


// Looks in all the modules for the DefaultDomain attribute
// The order is assembly and then the modules. It is first
// come, first serve.
void SystemDomain::SetDefaultDomainAttributes(IMDInternalImport* pScope, mdMethodDef mdMethod)
{
    CONTRACTL
    {
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    HRESULT hr;

    //
    // Check to see if the assembly has the LoaderOptimization attribute set.
    //

    DWORD cbVal;
    BYTE *pVal;
    IfFailThrow(hr = pScope->GetCustomAttributeByName(mdMethod,
                                                      DEFAULTDOMAIN_LOADEROPTIMIZATION_TYPE,
                                                      (const void**)&pVal, &cbVal));

    if (hr == S_OK) {
        // Using evil knowledge of serialization, we know that the byte
        // value is in the third byte.
        _ASSERTE(pVal && cbVal > 3);

        DWORD policy = (*(pVal+2)) & AppDomain::SHARE_POLICY_MASK;

        g_dwGlobalSharePolicy = policy;

        System()->DefaultDomain()->SetupLoaderOptimization(g_dwGlobalSharePolicy);
    }
}

void SystemDomain::SetupDefaultDomain()
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;


    Thread *pThread = GetThread();
    _ASSERTE(pThread);

    AppDomain *pDomain = pThread->GetDomain();
    _ASSERTE(pDomain);

    GCX_COOP();

    ENTER_DOMAIN_PTR(SystemDomain::System()->DefaultDomain(),ADV_DEFAULTAD)
    {
        // Push this frame around loading the main assembly to ensure the
        // debugger can properly recgonize any managed code that gets run
        // as "class initializaion" code.
        FrameWithCookie<DebuggerClassInitMarkFrame> __dcimf;

        InitializeDefaultDomain(TRUE);

        __dcimf.Pop();
    }
    END_DOMAIN_TRANSITION;

    RETURN;
}

HRESULT SystemDomain::SetupDefaultDomainNoThrow()
{
    CONTRACTL
    {
        NOTHROW;
        MODE_ANY;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    BEGIN_EXCEPTION_GLUE(&hr, NULL)
    {
        SystemDomain::SetupDefaultDomain();
    }
    END_EXCEPTION_GLUE;

    return hr;
}

#ifdef _DEBUG
int g_fInitializingInitialAD = 0;
#endif

// This routine completes the initialization of the default domaine.
// After this call mananged code can be executed.
void SystemDomain::InitializeDefaultDomain(BOOL allowRedirects)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    WCHAR* pwsConfig = NULL;
    WCHAR* pwsPath = NULL;


    // Setup the default AppDomain.

#ifdef _DEBUG
    g_fInitializingInitialAD++;
#endif

    AppDomain* pDefaultDomain = SystemDomain::System()->DefaultDomain();
    pDefaultDomain->InitializeDomainContext(allowRedirects, pwsPath, pwsConfig);
    pDefaultDomain->InitializeDefaultDomainManager();

    MethodDesc *pRemoteServiceUnloadMD = g_Mscorlib.GetMethod(METHOD__REMOTING_SERVICES__DOMAIN_UNLOADED);
    CONSISTENCY_CHECK(AppDomain::s_remoteServiceUnloadMD == NULL ||
                      AppDomain::s_remoteServiceUnloadMD == pRemoteServiceUnloadMD);
    AppDomain::s_remoteServiceUnloadMD = pRemoteServiceUnloadMD;

#ifdef _DEBUG
    g_fInitializingInitialAD--;
#endif
    TESTHOOKCALL(RuntimeStarted(RTS_DEFAULTADREADY));
    RETURN;
}


#ifdef _DEBUG
int g_fInExecuteMainMethod = 0;
#endif

void SystemDomain::ExecuteMainMethod(HMODULE hMod, __in LPWSTR path /*=NULL*/)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(hMod));
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

#ifdef _DEBUG
    g_fInExecuteMainMethod++;
#endif

    Thread *pThread = GetThread();
    _ASSERTE(pThread);

    GCX_COOP();

    FrameWithCookie<ContextTransitionFrame> frame;
    pThread->EnterContextRestricted(SystemDomain::System()->DefaultDomain()->GetDefaultContext(), &frame);
    _ASSERTE(pThread->GetDomain());
    //ENTER_DOMAIN(SystemDomain::System()->DefaultDomain())
    {
        AppDomain *pDomain = GetAppDomain();
        _ASSERTE(pDomain);

        // Push this frame around loading the main assembly to ensure the
        // debugger can properly recognize any managed code that gets run
        // as "class initializaion" code.
        FrameWithCookie<DebuggerClassInitMarkFrame> __dcimf;

        PEImageHolder pTempImage(PEImage::OpenImage(path));
        if (!pTempImage->CheckILFormat())
        {
            ThrowHR(COR_E_BADIMAGEFORMAT);
        }
        PEFileHolder pTempFile(PEFile::Open(pTempImage.Extract()));

        // Check for CustomAttributes - Set up the DefaultDomain and the main thread
        // Note that this has to be done before ExplicitBind() as it
        // affects the bind
        mdToken tkEntryPoint = pTempFile->GetEntryPointToken();
            ReleaseHolder<IMDInternalImport> scope(pTempFile->GetMDImportWithRef());


        // This can potentially run managed code.
        InitializeDefaultDomain(FALSE);


        if((!IsNilToken(tkEntryPoint)) && (TypeFromToken(tkEntryPoint) == mdtMethodDef))
            SystemDomain::SetDefaultDomainAttributes(scope, tkEntryPoint);


        NewHolder<PEFileSecurityDescriptor> pSecDesc(new PEFileSecurityDescriptor(pDomain, pTempFile));
        Security::Resolve(pSecDesc);
        if (Security::AllowBindingRedirects(pSecDesc))
            pDomain->TurnOnBindingRedirects();

        SafeComHolder<IAssembly> pFusionAssembly;
        SafeComHolder<IFusionBindLog> pFusionLog;

        IfFailThrow(ExplicitBind(path, pDomain->GetFusionContext(),
                             EXPLICITBIND_FLAGS_EXE,
                             NULL, &pFusionAssembly, NULL, &pFusionLog));
        PEAssemblyHolder pFile(PEAssembly::Open(pFusionAssembly, NULL, pFusionLog));

        pDomain->m_pRootAssembly = GetAppDomain()->LoadAssembly(NULL, pFile, FILE_ACTIVE);
        if (CorCommandLine::m_pwszAppFullName == NULL) {
            StackSString friendlyName;
            StackSString assemblyPath = pFile->GetPath();
            SString::Iterator i = assemblyPath.End();

            if (PEAssembly::FindLastPathSeparator(assemblyPath, i)) {
                i++;
                friendlyName.Set(assemblyPath, i, assemblyPath.End());
            }
            else
                friendlyName.Set(assemblyPath);

            pDomain->SetFriendlyName(friendlyName, TRUE);
        }

        __dcimf.Pop();

        {
            GCX_PREEMP();

            LOG((LF_CLASSLOADER | LF_CORDB,
                 LL_INFO10,
                 "Created domain for an executable at %#x\n",
                 (pDomain->m_pRootAssembly ? pDomain->m_pRootAssembly->Parent() : NULL)));
            TESTHOOKCALL(RuntimeStarted(RTS_CALLINGENTRYPOINT));
            pDomain->m_pRootAssembly->ExecuteMainMethod(NULL);
        }
    }
    //END_DOMAIN_TRANSITION;
    pThread->ReturnToContext(&frame);


#ifdef _DEBUG
    g_fInExecuteMainMethod--;
#endif
}

void SystemDomain::ActivateApplication(int *pReturnValue)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    struct _gc {
        OBJECTREF   orThis;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCX_COOP();
    GCPROTECT_BEGIN(gc);

    gc.orThis = SystemDomain::System()->DefaultDomain()->GetExposedObject();

    MethodDescCallSite activateApp(METHOD__APP_DOMAIN__ACTIVATE_APPLICATION, &gc.orThis);

    ARG_SLOT args[] = {
        ObjToArgSlot(gc.orThis),
    };
    int retval = activateApp.Call_RetI4(args);
    if (pReturnValue)
        *pReturnValue = retval;

    GCPROTECT_END();
}

static HRESULT RunDllMainHelper(HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved, Thread* pThread, bool bReenablePreemptive)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_FAULT;

    MethodDesc  *pMD;
    AppDomain   *pDomain;
    Module      *pModule;
    HRESULT     hr = S_FALSE;           // Assume no entry point.
    CONTRACT_VIOLATION(ThrowsViolation); // scanner bug: scan complains about Thread::RaiseCrossContextException but all this is in a try/catch!
    // Setup the thread state to cooperative to run managed code.

    // Get the old domain from the thread.  Legacy dll entry points must always
    // be run from the default domain.
    //
    // We cannot support legacy dlls getting loaded into all domains!!
    EX_TRY
    {
        ENTER_DOMAIN_PTR(SystemDomain::System()->DefaultDomain(),ADV_DEFAULTAD)
        {
        pDomain = pThread->GetDomain();

        // The module needs to be in the current list if you are coming here.
            pModule = pDomain->GetIJWModule(hInst);
        if (!pModule)
            goto ErrExit;

        // See if there even is an entry point.
        pMD = pModule->GetDllEntryPoint();
        if (!pMD)
            goto ErrExit;

        // We're actually going to run some managed code.  There may be a customer
        // debug probe enabled, that prevents execution in the loader lock.
        CanRunManagedCode(hInst);


        // Enter cooperative mode
        if (!pThread->PreemptiveGCDisabled())
        {
            pThread->DisablePreemptiveGC();
        }

        // Run through the helper which will do exception handling for us.
        hr = ::RunDllMain(pMD, hInst, dwReason, lpReserved);

            // Update thread state for the case where we are returning to unmanaged code.
        if (bReenablePreemptive && pThread->PreemptiveGCDisabled())
        {
            pThread->EnablePreemptiveGC();
        }


ErrExit: ;
        // does not throw exception
        }
        END_DOMAIN_TRANSITION;

    }
    EX_CATCH
    {
        hr = Security::MapToHR(GETTHROWABLE());
    }
    EX_END_CATCH(SwallowAllExceptions)

    return (hr);
}

//*****************************************************************************
// This guy will set up the proper thread state, look for the module given
// the hinstance, and then run the entry point if there is one.
//*****************************************************************************
HRESULT SystemDomain::RunDllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved)
{

    CONTRACTL
    {
        NOTHROW;
        if (GetThread() && !lpReserved) {MODE_PREEMPTIVE;} else {DISABLED(MODE_PREEMPTIVE);};
        if(GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);};
    }
    CONTRACTL_END;


    Thread      *pThread = NULL;
    BOOL        fEnterCoop = FALSE;
    HRESULT     hr = S_FALSE;           // Assume no entry point.

    pThread = GetThread();
    if ((!pThread && (dwReason == DLL_PROCESS_DETACH || dwReason == DLL_THREAD_DETACH)) ||
        g_fEEShutDown)
        return S_OK;

    // ExitProcess is called while a thread is doing GC.
    if (dwReason == DLL_PROCESS_DETACH && GCHeap::IsGCInProgress())
        return S_OK;

    // ExitProcess is called on a thread that we don't know about
    if (dwReason == DLL_PROCESS_DETACH && GetThread() == NULL)
        return S_OK;

    // Need to setup the thread since this might be the first time the EE has
    // seen it if the thread was created in unmanaged code and this is a thread
    // attach event.
    if (pThread)
        fEnterCoop = pThread->PreemptiveGCDisabled();
    else {
        pThread = SetupThreadNoThrow(&hr);
        if (pThread == NULL)
            return hr;
        }



    NonHostLockCountHolder nonHostLock(pThread);

    hr = RunDllMainHelper(hInst, dwReason, lpReserved, pThread, !fEnterCoop);

    return hr;
}


// Helper function to load an assembly. This is called from LoadCOMClass.
/* static */

Assembly *AppDomain::LoadAssemblyHelper(LPCWSTR wszAssembly,
                                        LPCWSTR wszCodeBase)
{
    CONTRACT(Assembly *)
    {
        THROWS;
        POSTCONDITION(CheckPointer(RETVAL));
        PRECONDITION(wszAssembly || wszCodeBase);
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

        AssemblySpec spec;

    if(wszAssembly) {
        #define MAKE_TRANSLATIONFAILED  { ThrowOutOfMemory(); }
        MAKE_UTF8PTR_FROMWIDE(szAssembly,wszAssembly);
        #undef  MAKE_TRANSLATIONFAILED
        IfFailThrow(spec.Init(szAssembly));
    }

    if (wszCodeBase) {
        spec.SetCodeBase(wszCodeBase, (DWORD)(wcslen(wszCodeBase)+1));
    }

    RETURN spec.LoadAssembly(FILE_LOADED);
}


struct CallersDataWithStackMark
{
    StackCrawlMark* stackMark;
    BOOL foundMe;
    BOOL skippingRemoting;
    MethodDesc* pFoundMethod;
    MethodDesc* pPrevMethod;
    AppDomain*  pAppDomain;
};

/*static*/
MethodDesc* SystemDomain::GetCallersMethod(StackCrawlMark* stackMark,
                                       AppDomain **ppAppDomain/*=NULL*/)

{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    CallersDataWithStackMark cdata;
    ZeroMemory(&cdata, sizeof(CallersDataWithStackMark));
    cdata.stackMark = stackMark;

    GetThread()->StackWalkFrames(CallersMethodCallbackWithStackMark, &cdata, FUNCTIONSONLY | LIGHTUNWIND);

    if(cdata.pFoundMethod) {
        if (ppAppDomain)
            *ppAppDomain = cdata.pAppDomain;
        return cdata.pFoundMethod;
    } else
        return NULL;
}

/*static*/
MethodTable* SystemDomain::GetCallersType(StackCrawlMark* stackMark,
                                       AppDomain **ppAppDomain/*=NULL*/)

{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    CallersDataWithStackMark cdata;
    ZeroMemory(&cdata, sizeof(CallersDataWithStackMark));
    cdata.stackMark = stackMark;

    GetThread()->StackWalkFrames(CallersMethodCallbackWithStackMark, &cdata, FUNCTIONSONLY | LIGHTUNWIND);

    if(cdata.pFoundMethod) {
        if (ppAppDomain)
            *ppAppDomain = cdata.pAppDomain;
        return cdata.pFoundMethod->GetMethodTable();
    } else
        return NULL;
}

/*static*/
Module* SystemDomain::GetCallersModule(StackCrawlMark* stackMark,
                                       AppDomain **ppAppDomain/*=NULL*/)

{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    CallersDataWithStackMark cdata;
    ZeroMemory(&cdata, sizeof(CallersDataWithStackMark));
    cdata.stackMark = stackMark;

    GetThread()->StackWalkFrames(CallersMethodCallbackWithStackMark, &cdata, FUNCTIONSONLY | LIGHTUNWIND);

    if(cdata.pFoundMethod) {
        if (ppAppDomain)
            *ppAppDomain = cdata.pAppDomain;
        return cdata.pFoundMethod->GetModule();
    } else
        return NULL;
}

struct CallersData
{
    int skip;
    MethodDesc* pMethod;
};

/*static*/
Assembly* SystemDomain::GetCallersAssembly(StackCrawlMark *stackMark,
                                           AppDomain **ppAppDomain/*=NULL*/)
{
    WRAPPER_CONTRACT;
    Module* mod = GetCallersModule(stackMark, ppAppDomain);
    if (mod)
        return mod->GetAssembly();
    return NULL;
}

/*static*/
Module* SystemDomain::GetCallersModule(int skip)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    CallersData cdata;
    ZeroMemory(&cdata, sizeof(CallersData));
    cdata.skip = skip;

    StackWalkFunctions(GetThread(), CallersMethodCallback, &cdata);

    if(cdata.pMethod)
        return cdata.pMethod->GetModule();
    else
        return NULL;
}

/*static*/
Assembly* SystemDomain::GetCallersAssembly(int skip)
{
    WRAPPER_CONTRACT;
    Module* mod = GetCallersModule(skip);
    if (mod)
        return mod->GetAssembly();
    return NULL;
}

/*private static*/
StackWalkAction SystemDomain::CallersMethodCallbackWithStackMark(CrawlFrame* pCf, VOID* data)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_INTOLERANT;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    MethodDesc *pFunc = pCf->GetFunction();

    /* We asked to be called back only for functions */
    _ASSERTE(pFunc);

    CallersDataWithStackMark* pCaller = (CallersDataWithStackMark*) data;
    if (pCaller->stackMark)
    {
        PREGDISPLAY regs = pCf->GetRegisterSet();
        if (!IsInCalleesFrames(regs, pCaller->stackMark))
        {
            // save the current in case it is the one we want
            pCaller->pPrevMethod = pFunc;
            pCaller->pAppDomain = pCf->GetAppDomain();
            return SWA_CONTINUE;
        }

        // LookForMe stack crawl marks needn't worry about reflection or
        // remoting frames on the stack. Each frame above (newer than) the
        // target will be captured by the logic above. Once we transition to
        // finding the stack mark below the AofRA, we know that we hit the
        // target last time round and immediately exit with the cached result.

        if (*(pCaller->stackMark) == LookForMe)
        {
            pCaller->pFoundMethod = pCaller->pPrevMethod;
            return SWA_ABORT;
    }
    }

    // Skip reflection and remoting frames that could lie between a stack marked
    // method and its true caller (or that caller and its own caller). These
    // frames are infrastructure and logically transparent to the stack crawling
    // algorithm.

    // Skipping remoting frames. We always skip entire client to server spans
    // (though we see them in the order server then client during a stack crawl
    // obviously).

    // We spot the server dispatcher end because all calls are dispatched
    // through a single method: StackBuilderSink.PrivateProcessMessage.


    if (pFunc == g_Mscorlib.GetMethod(METHOD__STACK_BUILDER_SINK__PRIVATE_PROCESS_MESSAGE))
    {
        _ASSERTE(!pCaller->skippingRemoting);
        pCaller->skippingRemoting = true;
        return SWA_CONTINUE;
    }

    // And we spot the client end because there's a transparent proxy transition
    // frame pushed.
    if (!pCf->IsFrameless() && pCf->GetFrame()->GetFrameType() == Frame::TYPE_TP_METHOD_FRAME)
    {
        pCaller->skippingRemoting = false;
        return SWA_CONTINUE;
    }

    // Skip any frames into between the server and client remoting endpoints.
    if (pCaller->skippingRemoting)
        return SWA_CONTINUE;


    // Skipping reflection frames. We don't need to be quite as exhaustive here
    // as the security or reflection stack walking code since we know this logic
    // is only invoked for selected methods in mscorlib itself. So we're
    // reasonably sure we won't have any sensitive methods late bound invoked on
    // constructors, properties or events. This leaves being invoked via
    // MethodInfo, Type or Delegate (and depending on which invoke overload is
    // being used, several different reflection classes may be involved).
    MethodTable *pMT = pFunc->GetMethodTable();

    g_IBCLogger.LogMethodDescAccess(pFunc);

    if (g_Mscorlib.IsClass(pMT, CLASS__METHOD) ||
        g_Mscorlib.IsClass(pMT, CLASS__METHOD_BASE) ||
        g_Mscorlib.IsClass(pMT, CLASS__CLASS) ||
        g_Mscorlib.IsClass(pMT, CLASS__TYPE) ||
        g_Mscorlib.IsClass(pMT, CLASS__METHOD_HANDLE) ||
        pMT->IsAnyDelegateClass() ||
        pMT->IsAnyDelegateExact())
        return SWA_CONTINUE;

    // Return the first non-reflection/remoting frame if no stack mark was
    // supplied.
    if (!pCaller->stackMark)
    {
        pCaller->pFoundMethod = pFunc;
        pCaller->pAppDomain = pCf->GetAppDomain();
        return SWA_ABORT;
    }

    // When looking for caller's caller, we delay returning results for another
    // round (the way this is structured, we will still be able to skip
    // reflection and remoting frames between the caller and the caller's
    // caller).

    if ((*(pCaller->stackMark) == LookForMyCallersCaller) &&
         (pCaller->pFoundMethod == NULL))
    {
        pCaller->pFoundMethod = pFunc;
        return SWA_CONTINUE;
    }

    // We must either be looking for the caller, or the caller's caller when
    // we've already found the caller (we used a non-null value in pFoundMethod
    // simply as a flag, the correct method to return in both case is the
    // current method).
    pCaller->pFoundMethod = pFunc;
    pCaller->pAppDomain = pCf->GetAppDomain();
    return SWA_ABORT;
}

/*private static*/
StackWalkAction SystemDomain::CallersMethodCallback(CrawlFrame* pCf, VOID* data)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    MethodDesc *pFunc = pCf->GetFunction();

    /* We asked to be called back only for functions */
    _ASSERTE(pFunc);

    // Ignore intercepted frames
    if(pFunc->IsInterceptedForDeclSecurity())
        return SWA_CONTINUE;

    CallersData* pCaller = (CallersData*) data;
    if(pCaller->skip == 0) {
        pCaller->pMethod = pFunc;
        return SWA_ABORT;
    }
    else {
        pCaller->skip--;
        return SWA_CONTINUE;
    }

}



void SystemDomain::CreateDefaultDomain()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    if (m_pDefaultDomain != NULL)
        return;

    AppDomainRefHolder pDomain(new AppDomain());

    SystemDomain::LockHolder lh;
    pDomain->Init();

    Security::SetDefaultAppDomainProperty(pDomain->GetSecurityDescriptor());

    MEMORY_REPORT_SET_APP_DOMAIN(pDomain);

    // need to make this assignment here since we'll be releasing
    // the lock before calling AddDomain. So any other thread
    // grabbing this lock after we release it will find that
    // the COM Domain has already been created
    m_pDefaultDomain = pDomain;
    _ASSERTE (pDomain->GetId().m_dwId == DefaultADID);

    // allocate a Virtual Call Stub Manager for the default domain
    m_pDefaultDomain->InitVirtualCallStubManager();

    pDomain->SetStage(AppDomain::STAGE_OPEN);
    pDomain.SuppressRelease();

    LOG((LF_CLASSLOADER | LF_CORDB,
         LL_INFO10,
         "Created default domain at %#x\n", m_pDefaultDomain));
}

#ifdef DEBUGGING_SUPPORTED

void SystemDomain::PublishAppDomainAndInformDebugger (AppDomain *pDomain)
{
    CONTRACTL
    {
        if(!g_fEEInit) {THROWS;} else {DISABLED(NOTHROW);};
        if(!g_fEEInit) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);};
        MODE_ANY;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO100, "SD::PADAID: Adding 0x%x\n", pDomain));
    // The DefaultDomain is a special case since it gets created before any
    // assemblies, etc. have been loaded yet. Don't send an event for it
    // if the EE is not yet initialized.
    if ((pDomain == m_pSystemDomain->m_pDefaultDomain) && g_fEEInit == TRUE)
    {
        LOG((LF_CORDB, LL_INFO1000, "SD::PADAID:Returning early b/c of init!\n"));
        return;
    }

    // Indication (for the debugger) that this app domain is being created
    pDomain->SetDomainBeingCreated (TRUE);

    // Call the publisher API to add this appdomain entry to the list
    // The publisher will handle failures, so we don't care if this succeeds or fails.
    _ASSERTE (g_pDebugInterface != NULL);
    g_pDebugInterface->AddAppDomainToIPC(pDomain);


    // Indication (for the debugger) that the app domain is finished being created
    pDomain->SetDomainBeingCreated (FALSE);
}

#endif // DEBUGGING_SUPPORTED

void SystemDomain::AddDomain(AppDomain* pDomain)
{
    CONTRACTL
    {
        NOTHROW;
        MODE_ANY;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer((pDomain)));
    }
    CONTRACTL_END;

    {
        LockHolder lh;

        _ASSERTE (pDomain->m_Stage != AppDomain::STAGE_CREATING);
        if (pDomain->m_Stage == AppDomain::STAGE_READYFORMANAGEDCODE ||
            pDomain->m_Stage == AppDomain::STAGE_ACTIVE)
        {
            pDomain->SetStage(AppDomain::STAGE_OPEN);
            IncrementNumAppDomains(); // Maintain a count of app domains added to the list.
        }
    }

    // Note that if you add another path that can reach here without calling
    // PublishAppDomainAndInformDebugger, then you should go back & make sure
    // that PADAID gets called.  Right after this call, if not sooner.
    LOG((LF_CORDB, LL_INFO1000, "SD::AD:Would have added domain here! 0x%x\n",
        pDomain));
}

BOOL SystemDomain::RemoveDomain(AppDomain* pDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pDomain));
        PRECONDITION(!pDomain->IsDefaultDomain());    
    }
    CONTRACTL_END;

    // You can not remove the default domain.


    if (!pDomain->IsActive())
        return FALSE;

    pDomain->Release();

    return TRUE;
}


#ifdef PROFILING_SUPPORTED
void SystemDomain::NotifyProfilerStartup()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    GCX_PREEMP();

    PROFILER_CALL;

    if (CORProfilerTrackAppDomainLoads())
    {
        _ASSERTE(System());
        g_profControlBlock.pProfInterface->AppDomainCreationStarted((ThreadID) GetThread(), (AppDomainID) System());
    }

    if (CORProfilerTrackAppDomainLoads())
    {
        _ASSERTE(System());
        g_profControlBlock.pProfInterface->AppDomainCreationFinished((ThreadID) GetThread(), (AppDomainID) System(), S_OK);
    }

    if (CORProfilerTrackAppDomainLoads())
    {
        _ASSERTE(System()->DefaultDomain());
        g_profControlBlock.pProfInterface->AppDomainCreationStarted((ThreadID) GetThread(), (AppDomainID) System()->DefaultDomain());
    }

    if (CORProfilerTrackAppDomainLoads())
    {
        _ASSERTE(System()->DefaultDomain());
        g_profControlBlock.pProfInterface->AppDomainCreationFinished((ThreadID) GetThread(), (AppDomainID) System()->DefaultDomain(), S_OK);
    }

    if (CORProfilerTrackAppDomainLoads())
    {
        _ASSERTE(SharedDomain::GetDomain());
        g_profControlBlock.pProfInterface->AppDomainCreationStarted((ThreadID) GetThread(), (AppDomainID) SharedDomain::GetDomain());
    }

    if (CORProfilerTrackAppDomainLoads())
    {
        _ASSERTE(SharedDomain::GetDomain());
        g_profControlBlock.pProfInterface->AppDomainCreationFinished((ThreadID) GetThread(), (AppDomainID) SharedDomain::GetDomain(), S_OK);
    }
}

HRESULT SystemDomain::NotifyProfilerShutdown()
{
    PROFILER_CALL;
    if (CORProfilerTrackAppDomainLoads())
    {
        _ASSERTE(System());
        g_profControlBlock.pProfInterface->AppDomainShutdownStarted((ThreadID) GetThread(), (AppDomainID) System());
    }

    if (CORProfilerTrackAppDomainLoads())
    {
        _ASSERTE(System());
        g_profControlBlock.pProfInterface->AppDomainShutdownFinished((ThreadID) GetThread(), (AppDomainID) System(), S_OK);
    }

    if (CORProfilerTrackAppDomainLoads())
    {
        _ASSERTE(System()->DefaultDomain());
        g_profControlBlock.pProfInterface->AppDomainShutdownStarted((ThreadID) GetThread(), (AppDomainID) System()->DefaultDomain());
    }

    if (CORProfilerTrackAppDomainLoads())
    {
        _ASSERTE(System()->DefaultDomain());
        g_profControlBlock.pProfInterface->AppDomainShutdownFinished((ThreadID) GetThread(), (AppDomainID) System()->DefaultDomain(), S_OK);
    }
    return (S_OK);
}
#endif // PROFILING_SUPPORTED


// Get the developers path from the environment. This can only be set through the environment and
// cannot be added through configuration files, registry etc. This would make it to easy for
// developers to deploy apps that are not side by side. The environment variable should only
// be used on developers machines where exact matching to versions makes build and testing to
// difficult.
void SystemDomain::GetDevpathW(__out_ecount_opt(1) LPWSTR* pDevpath, DWORD* pdwDevpath)
{
   CONTRACTL
    {
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    GCX_PREEMP();

    if(g_pConfig->DeveloperInstallation() && m_fDevpath == FALSE) {

        LockHolder lh;

        if(m_fDevpath == FALSE) {
            DWORD dwPath = 0;
            dwPath = WszGetEnvironmentVariable(APPENV_DEVPATH, 0, 0);
            if(dwPath) {
                m_pwDevpath = (WCHAR*) new WCHAR[dwPath];
                m_dwDevpath = WszGetEnvironmentVariable(APPENV_DEVPATH,
                                                            m_pwDevpath,
                                                            dwPath);
            }

            m_fDevpath = TRUE;
        }
        // lh out of scope here
    }

    if(pDevpath) *pDevpath = m_pwDevpath;
    if(pdwDevpath) *pdwDevpath = m_dwDevpath;
    return;
}


#ifdef _DEBUG
struct AppDomain::ThreadTrackInfo {
    Thread *pThread;
    CDynArray<Frame *> frameStack;
};
#endif // _DEBUG

AppDomain::AppDomain()
{
    // initialize fields so the appdomain can be safely destructed
    // shouldn't call anything that can fail here - use ::Init instead
    WRAPPER_CONTRACT;

    m_cRef=1;
    m_pNextInDelayedUnloadList = NULL;
    m_pSecContext = NULL;
    m_pMetaSigCache = NULL;
    m_metaSigCacheState = NULL;
    m_fRudeUnload = FALSE;
    m_pUnloadRequestThread = NULL;
    m_ADUnloadSink=NULL;

    // Initialize Shared state. Assemblies are loaded
    // into each domain by default.
    m_SharePolicy = SHARE_POLICY_UNSPECIFIED;

    m_pRootAssembly = NULL;

    m_pwDynamicDir = NULL;

    m_dwFlags = 0;
    m_pSecDesc = NULL;
    m_pDefaultContext = NULL;


    m_pAsyncPool = NULL;
    m_hHandleTableBucket = NULL;
    SetExecutable(TRUE);

    m_ExposedObject = NULL;
    m_pComIPForExposedObject = NULL;

 #ifdef _DEBUG
    m_pThreadTrackInfoList = NULL;
    m_TrackSpinLock = 0;
#endif // _DEBUG

    m_dwThreadEnterCount = 0;
    m_dwThreadsStillInAppDomain = (ULONG)-1;

    m_pSecDesc = NULL;
    m_hHandleTableBucket=NULL;

    m_ExposedObject = NULL;


    m_pRefClassFactHash = NULL;

    SetStage(STAGE_CREATING);

    m_bForceGCOnUnload=FALSE;
    m_bUnloadingFromUnloadEvent=FALSE;
}


AppDomain::~AppDomain()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    if (m_dwId.m_dwId!=0)
        SystemDomain::ReleaseAppDomainId(m_dwId);

    m_AssemblyCache.Clear();

    if (m_ADUnloadSink)
        m_ADUnloadSink->Release();


    if(!g_fEEInit)
        Terminate();

    if (m_pSecContext)
        delete m_pSecContext;

    if (m_pMetaSigCache)
        delete[] m_pMetaSigCache;


    {
        GCX_COOP();         // See SystemDomain::EnumAllStaticGCRefs if you are removing this
    CrossDomainTypeMap::FlushStaleEntries();
#if defined(_X86_) || defined(_WIN64)
    CrossDomainFieldMap::FlushStaleEntries();
#endif // _X86_ || _WIN64
    }

#ifdef _DEBUG
    // If we were tracking thread AD transitions, nuke the list on shutdown
    if (m_pThreadTrackInfoList)
    {
        while (m_pThreadTrackInfoList->Count() > 0)
        {
            // Get the very last element
            ThreadTrackInfo *pElem = *(m_pThreadTrackInfoList->Get(m_pThreadTrackInfoList->Count() - 1));
            _ASSERTE(pElem);

            // Free the memory
            delete pElem;

            // Remove pointer entry from the list
            m_pThreadTrackInfoList->Delete(m_pThreadTrackInfoList->Count() - 1);
        }

        // Now delete the list itself
        delete m_pThreadTrackInfoList;
        m_pThreadTrackInfoList = NULL;
    }
#endif // _DEBUG
}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
#ifdef _DEBUG
#include "handletablepriv.h"
#endif



void AppDomain::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(SystemDomain::IsUnderDomainLock());
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
#ifdef _DEBUG
    m_dwIterHolders=0;
    m_dwRefTakers=0;
#endif

    SetStage( STAGE_CREATING);
    m_dwIndex = SystemDomain::GetNewAppDomainIndex(this);

    m_dwId = SystemDomain::GetNewAppDomainId(this);

    m_pSecContext = new SecurityContext();
    m_ADUnloadSink=new ADUnloadSink();

    MEMORY_REPORT_APP_DOMAIN_SCOPE(this);

    BaseDomain::Init();

// Set up the binding caches
    m_AssemblyCache.Init(&m_DomainCacheCrst, GetHighFrequencyHeap());
    m_UnmanagedCache.InitializeTable(this, &m_DomainCacheCrst);

    m_sDomainLocalBlock.Init(this);

    // Default domain reuses the handletablemap that was created during EEStartup since
    // default domain cannot be unloaded.
    if (GetId().m_dwId == DefaultADID)
    {
        m_hHandleTableBucket = g_HandleTableMap.pBuckets[0];
    }
    else
    {
        m_hHandleTableBucket = Ref_CreateHandleTableBucket(m_dwIndex);
    }

#ifdef _DEBUG
    if (((HandleTable *)(m_hHandleTableBucket->pTable[0]))->uADIndex != m_dwIndex)
        _ASSERTE (!"AD index mismatch");
#endif // _DEBUG



    m_ReflectionCrst.Init("CrstReflection", CrstReflection, CRST_UNSAFE_ANYMODE);
    m_RefClassFactCrst.Init("CrstClassFactInfoHash", CrstClassFactInfoHash);

    {
        LockOwner lock = {&m_DomainCrst, IsOwnerOfCrst};
        m_clsidHash.Init(0,&CompareCLSID,true, &lock); // init hash table
    }

    SetStage(STAGE_READYFORMANAGEDCODE);

    m_pDefaultContext = Context::SetupDefaultContext(this);
    m_ExposedObject = CreateHandle(NULL);

    // Create the Application Security Descriptor
    CreateSecurityDescriptor();

    COUNTER_ONLY(GetPrivatePerfCounters().m_Loading.cAppDomains++);

}

void AppDomain::Stop()
{
    CONTRACTL
    {
        NOTHROW;
        MODE_ANY;
        GC_TRIGGERS;
    }
    CONTRACTL_END;


#ifdef DEBUGGING_SUPPORTED
    if (IsDebuggerAttached())
        NotifyDebuggerUnload();
#endif // DEBUGGING_SUPPORTED

    m_pRootAssembly = NULL; // This assembly is in the assembly list;

    if (m_pSecDesc != NULL)
    {
        delete m_pSecDesc;
        m_pSecDesc = NULL;
    }

#ifdef DEBUGGING_SUPPORTED
    _ASSERTE(NULL != g_pDebugInterface);

    // Call the publisher API to delete this appdomain entry from the list
    CONTRACT_VIOLATION(ThrowsViolation);
    g_pDebugInterface->RemoveAppDomainFromIPC (this);
#endif // DEBUGGING_SUPPORTED
}

void AppDomain::Terminate()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    GCX_PREEMP();


    _ASSERTE(m_dwThreadEnterCount == 0 || IsDefaultDomain());

    if (m_pComIPForExposedObject)
    {
        m_pComIPForExposedObject->Release();
        m_pComIPForExposedObject = NULL;
    }

    Context::CleanupDefaultContext(this);
    m_pDefaultContext = NULL;



    if(m_pAsyncPool != NULL)
    {
        delete m_pAsyncPool;
        m_pAsyncPool = NULL;
    }


    if (!g_fProcessDetach)
    {
        // if we're not shutting down everything then clean up the string literals associated
        // with this appdomain -- note that is no longer needs to happen while suspended
        // because the appropriate locks are taken in the GlobalStringLiteralMap
        // this is important as this locks have a higher lock number than does the
        // thread-store lock which is taken when we suspend.

        if (m_pStringLiteralMap)
        {
            delete m_pStringLiteralMap;
            m_pStringLiteralMap = NULL;
        }

        // Suspend the EE to do some clean up that can only occur
        // while no threads are running.
        GCX_COOP (); // SuspendEE may require current thread to be in Coop mode
        GCHeap::GetGCHeap()->SuspendEE(GCHeap::SUSPEND_FOR_APPDOMAIN_SHUTDOWN);
    }


    if (GetVirtualCallStubManager() != NULL)
    {
        GetVirtualCallStubManager()->Uninit();
        delete GetVirtualCallStubManager();
        ClearVirtualCallStubManager();
    }
    MethodTable::ClearMethodDataCache();
    ClearJitGenericHandleCache(this);


    if (!g_fProcessDetach)
    {
        // Resume the EE.
        GCHeap::GetGCHeap()->RestartEE(FALSE, TRUE);
    }

    ShutdownAssemblies();

    if (m_pRefClassFactHash)
    {
        m_pRefClassFactHash->Destroy();
        // storage for m_pRefClassFactHash itself is allocated on the loader heap
    }

    m_ReflectionCrst.Destroy();
    m_RefClassFactCrst.Destroy();


    BaseDomain::Terminate();

#ifdef _DEBUG
    if (m_hHandleTableBucket &&
        m_hHandleTableBucket->pTable &&
        ((HandleTable *)(m_hHandleTableBucket->pTable[0]))->uADIndex != m_dwIndex)
        _ASSERTE (!"AD index mismatch");
#endif // _DEBUG

    if (m_hHandleTableBucket) {
        Ref_DestroyHandleTableBucket(m_hHandleTableBucket);
        m_hHandleTableBucket = NULL;
    }


    if(m_dwIndex.m_dwIndex != 0)
        SystemDomain::ReleaseAppDomainIndex(m_dwIndex);
}



void AppDomain::CloseDomain()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;


    BOOL bADRemoved=FALSE;;

    AddRef();  // Hold a reference
    AppDomainRefHolder AdHolder(this);
    {
        SystemDomain::LockHolder lh;

        SystemDomain::System()->DecrementNumAppDomains(); // Maintain a count of app domains added to the list.
        bADRemoved = SystemDomain::System()->RemoveDomain(this);
    }

    if(bADRemoved)
        Stop();
}

/*********************************************************************/

BOOL AppDomain::IsCompilationDomain()
{
    LEAF_CONTRACT;

    BOOL isCompilationDomain = (m_dwFlags & COMPILATION_DOMAIN) != 0;
    return isCompilationDomain;
}

/*********************************************************************/

struct GetExposedObject_Args
{
    AppDomain *pDomain;
    OBJECTREF *ref;
};

static void GetExposedObject_Wrapper(LPVOID ptr)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_INTOLERANT;
    GetExposedObject_Args *args = (GetExposedObject_Args *) ptr;
    *(args->ref) = args->pDomain->GetExposedObject();
}


OBJECTREF AppDomain::GetExposedObject()
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    OBJECTREF ref = GetRawExposedObject();
    if (ref == NULL)
    {
        APPDOMAINREF obj = NULL;

        Thread *pThread = GetThread();
        if (pThread->GetDomain() != this)
        {
            GCPROTECT_BEGIN(ref);
            GetExposedObject_Args args = {this, &ref};
            // call through DoCallBack with a domain transition
            pThread->DoADCallBack(this,GetExposedObject_Wrapper, &args,ADV_CREATING|ADV_RUNNINGIN);
            GCPROTECT_END();
            return ref;
        }
        MethodTable *pMT = g_Mscorlib.GetClass(CLASS__APP_DOMAIN);

        // Create the module object
        obj = (APPDOMAINREF) AllocateObject(pMT);
        obj->SetDomain(this);

        if(StoreFirstObjectInHandle(m_ExposedObject, (OBJECTREF) obj) == FALSE) {
            obj = (APPDOMAINREF) GetRawExposedObject();
            _ASSERTE(obj);
        }

        return (OBJECTREF) obj;
    }

    return ref;
}

HRESULT AppDomain::GetComIPForExposedObject(IUnknown **pComIP)
{
    // Assumption: This function is called for AppDomain's that the current
    //             thread is in or has entered, or the AppDomain is kept alive.
    //
    // Assumption: This function can now throw.  The caller is responsible for any
    //             BEGIN_EXTERNAL_ENTRYPOINT, BEGIN_EXCEPTION_GLUE, or other
    //             techniques to convert to a COM HRESULT protocol.
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    Thread *pThread = GetThread();
    if (m_pComIPForExposedObject)
    {
        GCX_PREEMP_THREAD_EXISTS(pThread);
        m_pComIPForExposedObject->AddRef();
        *pComIP = m_pComIPForExposedObject;
        return S_OK;
    }

    IUnknown* punk = NULL;

    OBJECTREF ref = NULL;
    GCPROTECT_BEGIN(ref);

    EnsureComStarted();


        ENTER_DOMAIN_PTR(this,ADV_DEFAULTAD)
    {
        ref = GetExposedObject();
        punk = GetComIPFromObjectRef(&ref);
        if (FastInterlockCompareExchangePointer ((void**)&m_pComIPForExposedObject, punk, NULL) == NULL)
        {
            pThread->EnablePreemptiveGC();
            m_pComIPForExposedObject->AddRef();
            pThread->DisablePreemptiveGC();
        }
    }
    END_DOMAIN_TRANSITION;
    CONTRACT_VIOLATION(ThrowsViolation);  // Scanner bug: SCAN seems to have trouble with the nested EH going on in this method

    GCPROTECT_END();

    if(SUCCEEDED(hr))
    {
        *pComIP = m_pComIPForExposedObject;
    }

    return hr;
}

void AppDomain::CreateSecurityDescriptor()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_pSecDesc != NULL)
        delete m_pSecDesc;

    m_pSecDesc=NULL;

    m_pSecDesc = new ApplicationSecurityDescriptor(this);
}


void AppDomain::AddAssembly(DomainAssembly* assem)
{
    CONTRACTL
   {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
   }
    CONTRACTL_END;

    IfFailThrow(m_Assemblies.Append(assem));
}

BOOL AppDomain::ContainsAssembly(Assembly *assem)
{
    WRAPPER_CONTRACT;
    AssemblyIterator i = IterateAssembliesEx((AssemblyIterationFlags)(kIncludeLoaded | (assem->IsIntrospectionOnly() ? kIncludeIntrospection : kIncludeExecution)));;

    while (i.Next())
    {
        if (i.GetAssembly() == assem)
            return TRUE;
    }

    return FALSE;
}

BOOL AppDomain::HasSetSecurityPolicy()
{
    CONTRACT(BOOL)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    GCX_COOP();

    RETURN ((APPDOMAINREF)GetExposedObject())->HasSetPolicy();
}


BOOL AppDomain::ApplySharePolicy(DomainAssembly *pFile)
{
    CONTRACT(BOOL)
    {
        PRECONDITION(CheckPointer(pFile));
        THROWS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    if (!pFile->GetFile()->IsShareable())
        RETURN FALSE;

    if (ApplySharePolicyFlag(pFile))
        RETURN TRUE;

    RETURN FALSE;
}

BOOL AppDomain::ApplySharePolicyFlag(DomainAssembly *pFile)
{
    CONTRACT(BOOL)
    {
        PRECONDITION(CheckPointer(pFile));
        THROWS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    switch(GetSharePolicy()) {

    case SHARE_POLICY_ALWAYS:
        RETURN TRUE;

    case SHARE_POLICY_GAC:
    {
        RETURN pFile->IsClosedInGAC();
    }

    case SHARE_POLICY_NEVER:
        RETURN pFile->IsSystem();

    default:
        UNREACHABLE_MSG("Unknown share policy");
    }
}

EEClassFactoryInfoHashTable* AppDomain::SetupClassFactHash()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    CrstHolder ch(&m_ReflectionCrst);

    if (m_pRefClassFactHash == NULL)
    {
        AllocMemHolder<void> pCache(m_pLowFrequencyHeap->AllocMem(sizeof (EEClassFactoryInfoHashTable)));
        EEClassFactoryInfoHashTable *tmp = new (pCache) EEClassFactoryInfoHashTable;
        LockOwner lock = {&m_RefClassFactCrst,IsOwnerOfCrst};
        if (!tmp->Init(20, &lock))
            COMPlusThrowOM();
        pCache.SuppressRelease();
        m_pRefClassFactHash = tmp;
    }

    return m_pRefClassFactHash;
}


FileLoadLock *FileLoadLock::Create(PEFileListLock *pLock, PEFile *pFile, DomainFile *pDomainFile)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        PRECONDITION(pLock->HasLock());
        PRECONDITION(pLock->FindFileLock(pFile) == NULL);
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(GCViolation); //Need to stop using UNSAFE_COOPGC mode for LoadLock, then turn this functions contract into a GC_TRIGGER

    NewHolder<FileLoadLock> result(new FileLoadLock(pLock, pFile, pDomainFile));

    pLock->AddElement(result);
    result->AddRef(); // Add one ref on behalf of the ListLock's reference

    return result.Extract();
}

FileLoadLock::~FileLoadLock()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    ((PEFile *) m_pData)->Release();
}

DomainFile *FileLoadLock::GetDomainFile()
{
    LEAF_CONTRACT;
    return m_pDomainFile;
}

FileLoadLevel FileLoadLock::GetLoadLevel()
{
    LEAF_CONTRACT;
    return m_level;
}

ADID FileLoadLock::GetAppDomainId()
{
    LEAF_CONTRACT;
    return m_AppDomainId;
}

// Acquire will return FALSE and not take the lock if the file
// has already been loaded to the target level.  Otherwise,
// it will return TRUE and take the lock.
//
// Note that the taker must release the lock via IncrementLoadLevel.

BOOL FileLoadLock::Acquire(FileLoadLevel targetLevel)
{
    WRAPPER_CONTRACT;

    // If we are already loaded to the desired level, the lock is "free".
    if (m_level >= targetLevel)
        return FALSE;

    if (!DeadlockAwareEnter())
    {
        // We failed to get the lock due to a deadlock.
        return FALSE;
    }

    if (m_level >= targetLevel)
    {
        Leave();
        return FALSE;
    }

    return TRUE;
}

BOOL FileLoadLock::CanAcquire(FileLoadLevel targetLevel)
{
    // If we are already loaded to the desired level, the lock is "free".
    if (m_level >= targetLevel)
        return FALSE;

    return CanDeadlockAwareEnter();
}

BOOL FileLoadLock::CompleteLoadLevel(FileLoadLevel level, BOOL success)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS;
        THROWS;
        PRECONDITION(HasLock());
    }
    CONTRACTL_END;

    // Increment may happen more than once if reentrancy occurs (e.g. LoadLibrary)
    if (level > m_level)
    {
       // Must complete each level in turn, unless we have an error
        CONSISTENCY_CHECK(m_pDomainFile->IsError() || (level == (m_level+1)));
       // Remove the lock from the list if the load is completed
        if (level >= FILE_ACTIVE)
        {
            {
                GCX_COOP();
                PEFileListLockHolder lock((PEFileListLock*)m_pList);

                m_pList->Unlink(this);

                m_pDomainFile->ClearLoading();

                CONSISTENCY_CHECK(m_dwRefCount > 1); // Caller should have a refcount
            }


            Release(); // Release List's refcount on this lock

        }

        m_level = (FileLoadLevel)level;

        if (success)
            m_pDomainFile->SetLoadLevel(level);


        return TRUE;
    }
    else
        return FALSE;
}

void FileLoadLock::SetError(Exception *ex)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS;
        THROWS;
        PRECONDITION(CheckPointer(ex));
        PRECONDITION(HasLock());
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

        m_cachedHR = ex->GetHR();

    LOG((LF_LOADER, LL_WARNING, "LOADER: %x:***%s*\t!!!Non-transient error 0x%x\n",
         m_pDomainFile->GetAppDomain(), m_pDomainFile->GetSimpleName(), m_cachedHR));

    m_pDomainFile->SetError(ex);

    CompleteLoadLevel(FILE_ACTIVE, FALSE);
}

void FileLoadLock::AddRef()
{
    LEAF_CONTRACT;
    FastInterlockIncrement((LONG *) &m_dwRefCount);
}

UINT32 FileLoadLock::Release()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    LONG count = FastInterlockDecrement((LONG *) &m_dwRefCount);
    if (count == 0)
        delete this;

    return count;
}

FileLoadLock::FileLoadLock(PEFileListLock *pLock, PEFile *pFile, DomainFile *pDomainFile)
  : ListLockEntry(pLock, pFile, "File load lock"),
    m_level((FileLoadLevel) (FILE_LOAD_CREATE)),
    m_pDomainFile(pDomainFile),
    m_cachedHR(S_OK),
    m_AppDomainId(pDomainFile->GetAppDomain()->GetId())
{
    WRAPPER_CONTRACT;
    pFile->AddRef();
}

void FileLoadLock::HolderLeave(FileLoadLock *pThis)
{
    LEAF_CONTRACT;
    pThis->Leave();
}






//
// Assembly loading:
//
// Assembly loading is carefully layered to avoid deadlocks in the
// presence of circular loading dependencies.
// A LoadLevel is associated with each assembly as it is being loaded.  During the
// act of loading (abstractly, increasing its load level), its lock is
// held, and the current load level is stored on the thread.  Any
// recursive loads during that period are automatically restricted to
// only partially load the dependent assembly to the same level as the
// caller (or to one short of that level in the presence of a deadlock
// loop.)
//
// Each loading stage must be carfully constructed so that
// this constraint is expected and can be dealt with.
//
// Note that there is one case where this still doesn't handle recursion, and that is the
// security subsytem. The security system runs managed code, and thus must typically fully
// initialize assemblies of permission sets it is trying to use. (And of course, these may be used
// while those assemblies are initializing.)  This is dealt with in the historical manner - namely
// the security system passes in a special flag which says that it will deal with null return values
// in the case where a load cannot be safely completed due to such issues.
//

void AppDomain::LoadSystemAssemblies()
{
    // The only reason to make an assembly a "system assembly" is if the EE is caching
    // pointers to stuff in the assembly.  Because this is going on, we need to preserve
    // the invariant that the assembly is loaded into every app domain.
            //
    // Right now we have only one system assembly. We shouldn't need to add any more.

    WRAPPER_CONTRACT;
    LoadAssembly(NULL, SystemDomain::System()->SystemFile(), FILE_ACTIVE);
}

FileLoadLevel AppDomain::GetDomainFileLoadLevel(DomainFile *pFile)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    LoadLockHolder lock(this);

    FileLoadLock* pLockEntry = (FileLoadLock *) lock->FindFileLock(pFile->GetFile());

    if (pLockEntry == NULL)
        return pFile->GetLoadLevel();
    else
        return pLockEntry->GetLoadLevel();
}

// This checks if the thread has initiated (or completed) loading at the given level.  A false guarantees that
// (a) The current thread (or a thread blocking on the current thread) has not started loading the file
//      at the given level, and
// (b) No other thread had started loading the file at this level at the start of this function call.

// Note that another thread may start loading the file at that level in a race with the completion of
// this function.  However, the caller still has the guarantee that such a load started after this
// function was called (and e.g. any state in place before the function call will be seen by the other thread.)
//
// Conversely, a true guarantees that either the current thread has started the load step, or another
// thread has completed the load step.
//

BOOL AppDomain::IsLoading(DomainFile *pFile, FileLoadLevel level)
{
    // Cheap out
    if (pFile->GetLoadLevel() < level)
    {
        FileLoadLock *pLock = NULL;
        {
            LoadLockHolder lock(this);

            pLock = (FileLoadLock *) lock->FindFileLock(pFile->GetFile());

            if (pLock == NULL)
            {
                // No thread involved with loading
                return pFile->GetLoadLevel() >= level;
            }

            pLock->AddRef();
        }

        FileLoadLockRefHolder lockRef(pLock);

        if (pLock->Acquire(level))
        {
            // We got the lock - therefore no other thread has started this loading step yet.
            pLock->Leave();
            return FALSE;
        }

        // We didn't get the lock - either this thread is already doing the load,
        // or else the load has already finished.
    }
    return TRUE;
}

// CheckLoading is a weaker form of IsLoading, which will not block on
// other threads waiting for their status.  This is appropriate for asserts.
CHECK AppDomain::CheckLoading(DomainFile *pFile, FileLoadLevel level)
{
    // Cheap out
    if (pFile->GetLoadLevel() < level)
    {
        FileLoadLock *pLock = NULL;

        LoadLockHolder lock(this);

        pLock = (FileLoadLock *) lock->FindFileLock(pFile->GetFile());

        if (pLock != NULL
            && pLock->CanAcquire(level))
        {
            // We can get the lock - therefore no other thread has started this loading step yet.
            CHECK_FAILF(("Loading step %d has not been initiated yet", level));
        }

        // We didn't get the lock - either this thread is already doing the load,
        // or else the load has already finished.
    }

    CHECK_OK;
}

CHECK AppDomain::CheckCanLoadTypes(Assembly *pAssembly)
{
    CHECK_MSG(CheckValidModule(pAssembly->GetManifestModule()),
              "Type loading can occur only when executing in the assembly's app domain");
    CHECK_OK;
}

CHECK AppDomain::CheckCanExecuteManagedCode(MethodDesc* pMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    Module* pModule=pMD->GetModule();

        CHECK_MSG(CheckValidModule(pModule),
               "Managed code can only run when executing in the module's app domain");

    if (!pMD->IsInterface() || pMD->IsStatic()) //interfaces require no activation for instance methods
    {
        //cctor could have been interupted by ADU
        CHECK_MSG(HasUnloadStarted() || pModule->CheckActivated(),
              "Managed code can only run when its module has been activated in the current app domain");
    }

    CHECK_MSG(!IsPassiveDomain() || pModule->CanExecuteCode(),
              "Executing managed code from an unsafe assembly in a Passive AppDomain");

    CHECK_OK;
}

#endif // !DACCESS_COMPILE

void AppDomain::LoadDomainFile(DomainFile *pFile,
                                    FileLoadLevel targetLevel)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    // Handle the error case
    pFile->ThrowIfError(targetLevel);

    // Quick exit if finished
    if (pFile->GetLoadLevel() >= targetLevel)
        RETURN;

#ifndef DACCESS_COMPILE

    if (pFile->IsLoading())
    {
        // Load some more if appropriate
        LoadLockHolder lock(this);

        FileLoadLock* pLockEntry = (FileLoadLock *) lock->FindFileLock(pFile->GetFile());
        if (pLockEntry == NULL)
        {
            _ASSERTE (!pFile->IsLoading());
            RETURN;
        }

        pLockEntry->AddRef();

        lock.Release();

        LoadDomainFile(pLockEntry, targetLevel);
    }

#else // DACCESS_COMPILE
    DacNotImpl();
#endif // DACCESS_COMPILE

    RETURN;
}

#ifndef DACCESS_COMPILE

#ifdef LOGGING
static const char *fileLoadLevelName[] =
{
      "CREATE",
      "BEGIN",
      "FIND_NATIVE_IMAGE",
      "VERIFY_NATIVE_IMAGE_DEPENDENCIES",
      "ALLOCATE",
      "ADD_DEPENDENCIES",
      "PRE_LOADLIBRARY",
      "LOADLIBRARY",
      "POST_LOADLIBRARY",
      "EAGER_FIXUPS",
      "DELIVER_EVENTS",
      "LOADED",
      "VERIFY_EXECUTION",
      "ACTIVE",
};
#endif // LOGGING

FileLoadLevel AppDomain::GetThreadFileLoadLevel()
{
    WRAPPER_CONTRACT;
    if (GetThread()->GetLoadLevelLimiter() == NULL)
        return FILE_ACTIVE;
    else
        return (FileLoadLevel)(GetThread()->GetLoadLevelLimiter()->GetLoadLevel()-1);
}


Assembly *AppDomain::LoadAssembly(AssemblySpec* pIdentity, PEAssembly *pFile,
                                  FileLoadLevel targetLevel,
                                  OBJECTREF* pEvidence/*=NULL*/,
                                  OBJECTREF *pExtraEvidence/*=NULL*/,
                                  BOOL fDelayPolicyResolution)
{
    CONTRACT(Assembly *)
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pFile));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK)); // May be NULL in recursive load case
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    DomainAssembly *pAssembly = LoadDomainAssembly(pIdentity, pFile, targetLevel, pEvidence, pExtraEvidence, fDelayPolicyResolution);
    PREFIX_ASSUME(pAssembly != NULL);

    RETURN pAssembly->GetAssembly();
}

// Thread stress
class LoadDomainAssemblyStress : APIThreadStress
{
public:
    AppDomain *pThis;
    AssemblySpec* pSpec;
    PEAssembly *pFile;
    OBJECTREF* pEvidence;
    OBJECTREF *pExtraEvidence;
    FileLoadLevel targetLevel;

    LoadDomainAssemblyStress(AppDomain *pThis, AssemblySpec* pSpec, PEAssembly *pFile, FileLoadLevel targetLevel, OBJECTREF* pEvidence, OBJECTREF *pExtraEvidence)
        : pThis(pThis), pSpec(pSpec), pFile(pFile), pEvidence(pEvidence), pExtraEvidence(pExtraEvidence), targetLevel(targetLevel) {LEAF_CONTRACT;}

    void Invoke()
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_INTOLERANT;
        SetupThread();
        pThis->LoadDomainAssembly(pSpec, pFile, targetLevel, pEvidence, pExtraEvidence);
    }
};


DomainAssembly *AppDomain::LoadDomainAssembly(AssemblySpec* pIdentity, PEAssembly *pFile, FileLoadLevel targetLevel,
                                              OBJECTREF* pEvidence, OBJECTREF *pExtraEvidence, BOOL fDelayPolicyResolution)
{
    CONTRACT(DomainAssembly *)
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pFile));
        PRECONDITION(CheckPointer(pEvidence, NULL_OK));
        PRECONDITION(CheckPointer(pExtraEvidence, NULL_OK));
        PRECONDITION(pFile->IsSystem() || ::GetAppDomain()==this);
        POSTCONDITION(CheckPointer(RETVAL));
        POSTCONDITION(RETVAL->GetLoadLevel() >= GetThreadFileLoadLevel()
                      || RETVAL->GetLoadLevel() >= targetLevel);
        POSTCONDITION(RETVAL->CheckNoError(targetLevel));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;


        DomainAssembly *result;

        LoadDomainAssemblyStress ts (this, pIdentity, pFile, targetLevel, pEvidence, pExtraEvidence);

        // Go into preemptive mode since this may take a while.
        GCX_PREEMP();
        // Check for existing fully loaded assembly
        result = FindAssembly(pFile);
        if (result == NULL)
        {
            // Allocate the DomainAssembly a bit early to avoid GC mode problems. We could potentially avoid
            // a rare redundant allocation by moving this closer to FileLoadLock::Create, but it's not worth it.

            NewHolder<DomainAssembly> pDomainAssembly;
            pDomainAssembly = new DomainAssembly(this, pFile, pEvidence, pExtraEvidence, fDelayPolicyResolution);

            LoadLockHolder lock(this);

            // Find the list lock entry
            FileLoadLock * fileLock = (FileLoadLock *) lock->FindFileLock(pFile);
            if (fileLock == NULL)
            {
                // Check again in case we were racing
                result = FindAssembly(pFile);
                if (result == NULL)
                {
                    // We are the first one in - create the DomainAssembly
                    fileLock = FileLoadLock::Create(lock, pFile, pDomainAssembly);
                    pDomainAssembly.SuppressRelease();
                }
                else
                {
                    RETURN result;
                }
            }
            else
                fileLock->AddRef();

            lock.Release();

            // We pass our ref on fileLock to LoadDomainFile to release.

            // Note that if we throw here, we will poison fileLock with an error condition,
            // so it will not be removed until app domain unload.  So there is no need
            // to release our ref count.
            result = (DomainAssembly *) LoadDomainFile(fileLock, targetLevel);
            if(pIdentity)
                GetAppDomain()->AddAssemblyToCache(pIdentity, result);

        }
        else
            result->EnsureLoadLevel(targetLevel);

    RETURN result;
}

// Thread stress
class LoadDomainModuleStress : APIThreadStress
{
public:
    AppDomain *pThis;
    DomainAssembly *pAssembly;
    PEModule *pFile;
    FileLoadLevel targetLevel;

    LoadDomainModuleStress(AppDomain *pThis, DomainAssembly *pAssembly, PEModule *pFile, FileLoadLevel targetLevel)
        : pThis(pThis), pAssembly(pAssembly), pFile(pFile), targetLevel(targetLevel) {LEAF_CONTRACT;}

    void Invoke()
        {
            WRAPPER_CONTRACT;
            STATIC_CONTRACT_SO_INTOLERANT;
            SetupThread();
            pThis->LoadDomainModule(pAssembly, pFile, targetLevel);
        }
};

DomainModule *AppDomain::LoadDomainModule(DomainAssembly *pAssembly, PEModule *pFile,
                                          FileLoadLevel targetLevel)
{
    CONTRACT(DomainModule *)
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pAssembly));
        PRECONDITION(CheckPointer(pFile));
        POSTCONDITION(CheckPointer(RETVAL));
        POSTCONDITION(RETVAL->GetLoadLevel() >= GetThreadFileLoadLevel()
                      || RETVAL->GetLoadLevel() >= targetLevel);
        POSTCONDITION(RETVAL->CheckNoError(targetLevel));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    GCX_PREEMP();

    // Thread stress
    LoadDomainModuleStress ts (this, pAssembly, pFile, targetLevel);

    // Check for existing fully loaded assembly
    DomainModule *result = pAssembly->FindModule(pFile);
    if (result == NULL)
    {
        LoadLockHolder lock(this);

        // Check again in case we were racing
        result = pAssembly->FindModule(pFile);
        if (result == NULL)
        {
            // Find the list lock entry
            FileLoadLock *fileLock = (FileLoadLock *) lock->FindFileLock(pFile);
            if (fileLock == NULL)
            {
                // We are the first one in - create the DomainModule
                NewHolder<DomainModule> pDomainModule(new DomainModule(this, pAssembly, pFile));
                fileLock = FileLoadLock::Create(lock, pFile, pDomainModule);
                pDomainModule.SuppressRelease();
            }
            else
                fileLock->AddRef();

            lock.Release();

            // We pass our ref on fileLock to LoadDomainFile to release.

            // Note that if we throw here, we will poison fileLock with an error condition,
            // so it will not be removed until app domain unload.  So there is no need
            // to release our ref count.

            result = (DomainModule *) LoadDomainFile(fileLock, targetLevel);
        }
        else
        {
            lock.Release();
            result->EnsureLoadLevel(targetLevel);
        }

    }
    else
        result->EnsureLoadLevel(targetLevel);

    RETURN result;
}

struct LoadFileArgs
{
    FileLoadLock *pLock;
    FileLoadLevel targetLevel;
    DomainFile *result;
};

static void LoadDomainFile_Wrapper(void *ptr)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_INTOLERANT;
    LoadFileArgs *args = (LoadFileArgs *) ptr;
    args->result = GetAppDomain()->LoadDomainFile(args->pLock, args->targetLevel);
}

DomainFile *AppDomain::LoadDomainFile(FileLoadLock *pLock, FileLoadLevel targetLevel)
{
    CONTRACT(DomainFile *)
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pLock));
        PRECONDITION(pLock->GetDomainFile()->GetAppDomain() == this);
        POSTCONDITION(RETVAL->GetLoadLevel() >= GetThreadFileLoadLevel()
                      || RETVAL->GetLoadLevel() >= targetLevel);
        POSTCONDITION(RETVAL->CheckNoError(targetLevel));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;


    if(!CanLoadCode())
          COMPlusThrow(kAppDomainUnloadedException);

    // Thread stress
    APIThreadStress::SyncThreadStress();

    TIMELINE_START(LOADER, ("LoadFile %s", pLock->GetDomainFile()->GetFile()->GetSimpleName()));

    MEMORY_REPORT_CONTEXT_SCOPE("LoadDomainFile");

    DomainFile *pFile = pLock->GetDomainFile();

    // Make sure we release the lock on exit
    FileLoadLockRefHolder lockRef(pLock);

    // We need to perform the early steps of loading mscorlib without a domain transition.  This is
    // important for bootstrapping purposes - we need to get mscorlib at least partially loaded
    // into a domain before we can run serialization code to do the transition.
    //
    // Note that we cannot do this in general for all assemblies, because some of the security computations
    // require the managed exposed object, which must be created in the correct app domain.

    if (this != GetAppDomain()
        && pFile->GetFile()->IsSystem()
        && targetLevel > FILE_LOAD_ALLOCATE)
    {
        // Re-call the routine with a limited load level. This will cause the first part of the load to
        // get performed in the current app domain.

        pLock->AddRef();
        LoadDomainFile(pLock, targetLevel > FILE_LOAD_ALLOCATE ? FILE_LOAD_ALLOCATE : targetLevel);

        // Now continue on to complete the rest of the load, if any.
    }

    // Do a quick out check for the already loaded case.
    if (pLock->GetLoadLevel() >= targetLevel)
    {
        pFile->ThrowIfError(targetLevel);

        RETURN pFile;
    }

    // Make sure we are in the right domain.  Many of the load operations require the target domain
    // to be the current app domain, most notably anything involving managed code or managed object
    // creation.
    if (this != GetAppDomain()
        && (!pFile->GetFile()->IsSystem() || targetLevel > FILE_LOAD_ALLOCATE))
    {
        // Transition to the correct app domain and perform the load there.
        GCX_COOP();

        // we will release the lock in the other app domain
        lockRef.SuppressRelease();

        if(!CanLoadCode() || GetDefaultContext() ==NULL)
            COMPlusThrow(kAppDomainUnloadedException);
        LoadFileArgs args = {pLock, targetLevel, NULL};
        GetThread()->DoADCallBack(this, LoadDomainFile_Wrapper, (void *) &args, ADV_CREATING);

        RETURN args.result;
    }

    // Initialize a loading queue.  This will hold any loads which are triggered recursively but
    // which cannot be immediately satisfied due to anti-deadlock constraints.

    // PendingLoadQueues are allocated on the stack during a load, and
    // shared with all nested loads on the same thread. (Note that we won't use
    // "candidate" if we are in a recursive load; that's OK since they are cheap to
    // construct.)
    FileLoadLevel immediateTargetLevel = targetLevel;
    {
        LoadLevelLimiter limit;
        limit.Activate();

        // We cannot set a target level higher than that allowed by the limiter currently.
        // This is because of anti-deadlock constraints.
        if (immediateTargetLevel > limit.GetLoadLevel())
            immediateTargetLevel = limit.GetLoadLevel();

         LOG((LF_LOADER, LL_INFO100, "LOADER: %x:***%s*\t>>>Load initiated, %s/%s\n",
             pFile->GetAppDomain(), pFile->GetSimpleName(),
             fileLoadLevelName[immediateTargetLevel], fileLoadLevelName[targetLevel]));

        // Now loop and do the load incrementally to the target level.
        if (pLock->GetLoadLevel() < immediateTargetLevel)
        {
            // Thread stress
            APIThreadStress::SyncThreadStress();

            while (pLock->Acquire(immediateTargetLevel))
            {
                FileLoadLevel workLevel;
                {
                    FileLoadLockHolder fileLock(pLock);

                    // Work level is next step to do
                    workLevel = (FileLoadLevel)(fileLock->GetLoadLevel()+1);

                    // Set up the anti-deadlock constraint: we cannot safely recursively load any assemblies
                    // on this thread to a higher level than this assembly is being loaded now.
                    // Note that we do allow work at a parallel level; any deadlocks caused here will
                    // be resolved by the deadlock detection in the FileLoadLocks.
                    limit.SetLoadLevel(workLevel);

                    LOG((LF_LOADER,
                         (workLevel == FILE_LOAD_BEGIN
                          || workLevel == FILE_LOADED
                          || workLevel == FILE_ACTIVE)
                         ? LL_INFO10 : LL_INFO1000,
                         "LOADER: %x:***%s*\t   loading at level %s\n",
                         this, pFile->GetSimpleName(), fileLoadLevelName[workLevel]));

                    TryIncrementalLoad(pFile, workLevel, fileLock);
                }
                TESTHOOKCALL(CompletedFileLoadLevel(GetId().m_dwId,pFile,workLevel));
            }

            if (pLock->GetLoadLevel() == immediateTargetLevel-1)
            {
                LOG((LF_LOADER, LL_INFO100, "LOADER: %x:***%s*\t<<<Load limited due to detected deadlock, %s\n",
                     pFile->GetAppDomain(), pFile->GetSimpleName(),
                     fileLoadLevelName[immediateTargetLevel-1]));
            }
        }

        LOG((LF_LOADER, LL_INFO100, "LOADER: %x:***%s*\t<<<Load completed, %s\n",
             pFile->GetAppDomain(), pFile->GetSimpleName(),
             fileLoadLevelName[pLock->GetLoadLevel()]));

    }

    // There may have been an error stored on the domain file by another thread, or from a previous load
    pFile->ThrowIfError(targetLevel);

    // There are two normal results from the above loop.
    //
    // 1. We succeeded in loading the file to the current thread's load level.
    // 2. We succeeded in loading the file to the current thread's load level - 1, due
    //      to deadlock condition with another thread loading the same assembly.
    //
    // Either of these are considered satisfactory results, as code inside a load must expect
    // a parial load result.
    //
    // However, if load level elevation has occurred, then it is possible for a deadlock to
    // prevent us from loading an assembly which was loading before the elevation at a radically
    // lower level.  In such a case, we throw an exception which transiently fails the current
    // load, since it is likely we have not satisfied the caller.
    // (An alternate, and possibly preferable, strategy here would be for all callers to explicitly
    // identify the minimum load level acceptable via CheckLoadDomainFile and throw from there.)

    pFile->RequireLoadLevel((FileLoadLevel)(immediateTargetLevel-1));


    RETURN pFile;
}

void AppDomain::TryIncrementalLoad(DomainFile *pFile, FileLoadLevel workLevel, FileLoadLockHolder &lockHolder)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // This is factored out so we don't call EX_TRY in a loop (EX_TRY can _alloca)

    BOOL released = FALSE;

    EX_TRY
    {
          // Special case: for LoadLibrary, we cannot hold the lock during the
          // actual LoadLibrary call, because we might get a callback from _CorDllMain on any
          // other thread.  (Note that this requires DomainFile's LoadLibrary to be independently threadsafe.)

          if (workLevel == FILE_LOAD_LOADLIBRARY)
        {
              lockHolder.Release();
              released = TRUE;
          }

          // Do the work
          TESTHOOKCALL(NextFileLoadLevel(GetId().m_dwId,pFile,workLevel));
          BOOL success = pFile->DoIncrementalLoad(workLevel);
          TESTHOOKCALL(CompletingFileLoadLevel(GetId().m_dwId,pFile,workLevel));
          if (released)
          {
              // Reobtain lock to increment level. (Note that another thread may
              // have already done it which is OK.
              if (lockHolder->Acquire(workLevel))
              {
                  // note lockHolder.Acquire isn't wired up to actually take the lock
                  lockHolder.Acquire();
                  released = FALSE;
              }
          }

          if (!released)
          {
              // Complete the level.
              if (lockHolder->CompleteLoadLevel(workLevel, success) &&
                lockHolder->GetLoadLevel()==FILE_LOAD_DELIVER_EVENTS)
              {
                    lockHolder.Release();
                    released=FALSE;
                    pFile->DeliverAsyncEvents();
              };
          }
    }
    EX_HOOK
    {
        Exception *pEx = GET_EXCEPTION();


       //We will cache this error and wire this load to forever fail,
       // unless the exception is transient or the file is loaded OK but just cannot execute
        if (!pEx->IsTransient() && !pFile->IsLoaded())
        {

              if (released)
              {
                  // Reobtain lock to increment level. (Note that another thread may
                  // have already done it which is OK.
                  if (lockHolder->Acquire(workLevel)) // note pLockHolder->Acquire isn't wired up to actually take the lock
                  {
                      // note lockHolder.Acquire isn't wired up to actually take the lock
                      lockHolder.Acquire();
                      released = FALSE;
                  }
              }

              if (!released)
              {
                  // Report the error in the lock
                  lockHolder->SetError(pEx);
              }

            if (!EEFileLoadException::CheckType(pEx))
                EEFileLoadException::Throw(pFile->GetFile(), pEx->GetHR(), pEx);
        }

    }
    EX_END_HOOK;
}

// Checks whether the module is valid to be in the given app domain (need not be yet loaded)
CHECK AppDomain::CheckValidModule(Module *pModule)
{
    if (pModule->FindDomainFile(this) != NULL)
        CHECK_OK;

    CCHECK_START
    {
        Assembly *pAssembly = pModule->GetAssembly();

        CCHECK(pAssembly->IsDomainNeutral());

        Assembly *pSharedAssembly = NULL;
        _ASSERTE(this==::GetAppDomain());
        {
            SharedAssemblyLocator locator(pAssembly->GetManifestFile());
            pSharedAssembly = SharedDomain::GetDomain()->FindShareableAssembly(&locator);
        }

        CCHECK(pAssembly == pSharedAssembly);
    }
    CCHECK_END;

    CHECK_OK;
}

// Loads an existing Module into an AppDomain
// WARNING: this can only be done in a very limited scenario - the Module must be an unloaded domain neutral
// dependency in the app domain in question.  Normal code should not call this!
DomainFile *AppDomain::LoadDomainNeutralModuleDependency(Module *pModule, FileLoadLevel targetLevel)
{
    CONTRACT(DomainFile *)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(::GetAppDomain()==this);
        PRECONDITION(CheckPointer(pModule));
        POSTCONDITION(CheckValidModule(pModule));
        POSTCONDITION(CheckPointer(RETVAL));
        POSTCONDITION(RETVAL->GetModule() == pModule);
    }
    CONTRACT_END;

    DomainFile *pDomainFile = pModule->FindDomainFile(this);

    if (pDomainFile == NULL)
    {
        Assembly *pAssembly = pModule->GetAssembly();

        DomainAssembly *pDomainAssembly = pAssembly->FindDomainAssembly(this);
        if (pDomainAssembly == NULL)
        {
            AssemblySpec spec(this);
            spec.InitializeSpec(pAssembly->GetManifestFile());

            pDomainAssembly = spec.LoadDomainAssembly(targetLevel);
        }
        else
        {
            //if the domain assembly already exists, we need to load it to the target level
            pDomainAssembly->EnsureLoadLevel (targetLevel);
        }

        if(pAssembly != pDomainAssembly->GetAssembly())
        {
               ThrowHR(SECURITY_E_INCOMPATIBLE_SHARE);
        }

        if (pModule == pAssembly->GetManifestModule())
            pDomainFile = pDomainAssembly;
        else
        {
            pDomainFile = LoadDomainModule(pDomainAssembly, (PEModule*) pModule->GetFile(), targetLevel);
            STRESS_LOG4(LF_CLASSLOADER, LL_INFO100,"LDNMD:  DF: for %08x[%08x/%08x] is %08x",
                                     pModule,pDomainAssembly,pModule->GetFile(),pDomainFile);
        }
    }

    RETURN pDomainFile;
}


void AppDomain::SetSharePolicy(SharePolicy policy)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if ((int)policy > SHARE_POLICY_COUNT)
        COMPlusThrow(kArgumentException,L"Argument_InvalidValue");

    // We cannot make all code domain neutral and still provide complete compatibility with regard
    // to using custom security policy and assembly evidence.
    //
    // In particular, if you try to do either of the above AFTER loading a domain neutral assembly
    // out of the GAC, we will now throw an exception.  The remedy would be to either not use SHARE_POLICY_ALWAYS
    // (change LoaderOptimizationMultiDomain to LoaderOptimizationMultiDomainHost), or change the loading order
    // in the app domain to do the policy set or evidence load earlier (which BTW will have the effect of
    // automatically using MDH rather than MD, for the same result.)
    //
    // We include a compatibility flag here to preserve old functionality if necessary - this has the effect
    // of never using SHARE_POLICY_ALWAYS.

    if (policy == SHARE_POLICY_ALWAYS &&
        (HasSetSecurityPolicy()
         || GetCompatibilityFlag(compatOnlyGACDomainNeutral)))
    {
        // Never share assemblies not in the GAC
        policy = SHARE_POLICY_GAC;
    }

    if (policy != m_SharePolicy)
    {


        m_SharePolicy = policy;
    }

    return;
}

BOOL AppDomain::ReduceSharePolicyFromAlways()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // We may have already committed to always sharing - this is the case if
    // we have already loaded non-GAC-bound assemblies as domain neutral.

    if (GetSharePolicy() == SHARE_POLICY_ALWAYS)
    {
        AppDomain::AssemblyIterator i = IterateAssembliesEx((AssemblyIterationFlags)(kIncludeLoaded | kIncludeLoading | kIncludeExecution));

        // If we have loaded any non-GAC assemblies, we cannot set app domain policy as we have
        // already committed to the process-wide policy.

        while (i.Next())
        {
            DomainAssembly *pDomainAssembly = i.GetDomainAssembly();

            if (pDomainAssembly->ShouldLoadDomainNeutral()
                && ! pDomainAssembly->IsClosedInGAC() )
            {
                // This assembly has been loaded domain neutral because of SHARE_POLICY_ALWAYS. We
                // can't reverse that decision now, so we have to fail the sharing policy change.
                return FALSE;
            }
        }

        // We haven't loaded any non-GAC assemblies yet - scale back to SHARE_POLICY_GAC so
        // future non-GAC assemblies won't be loaded as domain neutral.
        SetSharePolicy(SHARE_POLICY_GAC);
    }

    return TRUE;
}

AppDomain::SharePolicy AppDomain::GetSharePolicy()
{
    LEAF_CONTRACT;
    // If the policy has been explicitly set for
    // the domain, use that.
    SharePolicy policy = m_SharePolicy;

    // Pick up the a specified config policy
    if (policy == SHARE_POLICY_UNSPECIFIED)
        policy = (SharePolicy) g_pConfig->DefaultSharePolicy();

    // Next, honor a host's request for global policy.
    if (policy == SHARE_POLICY_UNSPECIFIED)
        policy = (SharePolicy) g_dwGlobalSharePolicy;

    // If all else fails, use the hardwired default policy.
    if (policy == SHARE_POLICY_UNSPECIFIED)
        policy = SHARE_POLICY_DEFAULT;

    return policy;
}

void AppDomain::SetupSharedStatics()
{
    LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: SetupSharedStatics()"));

    // don't do any work in init stage. If not init only do work in non-shared case if are default domain
    _ASSERTE(!g_fEEInit);

   CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    // Because we are allocating/referencing objects, need to be in cooperative mode
    GCX_COOP();
    static DomainLocalModule *pSharedLocalModule = NULL;
    static OBJECTHANDLE hSharedStaticsHandle;

    MethodTable *pMT = g_Mscorlib.GetClass(CLASS__SHARED_STATICS);
    FieldDesc *pFD = g_Mscorlib.GetField(FIELD__SHARED_STATICS__SHARED_STATICS);

    if (pSharedLocalModule == NULL) {
        // Note that there is no race here since the default domain is always set up first
        _ASSERTE(IsDefaultDomain());

        hSharedStaticsHandle = CreateGlobalHandle(NULL);
        OBJECTREF pSharedStaticsInstance = AllocateObject(pMT);
        StoreObjectInHandle(hSharedStaticsHandle, pSharedStaticsInstance);

        DomainLocalBlock *pLocalBlock = GetDomainLocalBlock();
        pSharedLocalModule = pLocalBlock->GetModuleSlot(pMT->GetModule()->GetModuleID());

        pFD->SetStaticOBJECTREF(ObjectFromHandle(hSharedStaticsHandle));

        pMT->SetClassInited();

    }
    else
    {
        // Get shared data from shared appdomain
        ENTER_DOMAIN_PTR(this,ADV_CREATING)
        {
            DomainLocalModule *pLocalModule = pMT->GetDomainLocalModule();
            pLocalModule->PopulateClass(pMT);

            OBJECTREF* pHandle = (OBJECTREF*)
                                  (pLocalModule->GetGCStaticsBasePointer(pMT)+pFD->GetOffset());
            SetObjectReference( pHandle, ObjectFromHandle(hSharedStaticsHandle), ::GetAppDomain() );

            // Mark the class as initialized
            if (!pLocalModule->IsClassInitialized(pMT))
            {
                pLocalModule->SetClassInitialized(pMT);
            }
        }
        END_DOMAIN_TRANSITION;
    }

    return;
}

Module *AppDomain::FindModule(PEFile *pFile, BOOL includeLoading)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    AssemblyIterator i = IterateAssembliesEx((AssemblyIterationFlags)(kIncludeLoaded |
                                                                      (includeLoading ? kIncludeLoading : 0) |
                                                                      (pFile->IsIntrospectionOnly() ? kIncludeIntrospection : kIncludeExecution)
                                                                      ));
        while (i.Next())
        {
        Module *result = i.GetAssembly()->FindModule(pFile);

            if (result != NULL)
            return result;
        }

    return NULL;
}

DomainAssembly *AppDomain::FindAssembly(PEAssembly *pFile, BOOL includeLoading)
{
    WRAPPER_CONTRACT;
    AssemblyIterator i = IterateAssembliesEx((AssemblyIterationFlags)(kIncludeLoaded |
                                                                      (includeLoading ? kIncludeLoading : 0) |
                                                                      (pFile->IsIntrospectionOnly() ? kIncludeIntrospection : kIncludeExecution)
                                                                      ));
    while (i.Next())
    {
        DomainAssembly *pDomainAssembly = i.GetDomainAssembly();

        PEFile *pManifestFile = pDomainAssembly->GetFile();
        if (pManifestFile &&
            !pManifestFile->IsResource() &&
            pManifestFile->Equals(pFile)
           )
        {
            return i.GetDomainAssembly();
        }
    }

    return NULL;
}

static const AssemblyIterationFlags STANDARD_IJW_ITERATOR_FLAGS =
    (AssemblyIterationFlags)(kIncludeLoaded | kIncludeLoading | kIncludeExecution);

Module *AppDomain::GetIJWModule(HMODULE hMod)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    AssemblyIterator i = IterateAssembliesEx(STANDARD_IJW_ITERATOR_FLAGS);
    while (i.Next())
    {
        DomainFile *result = i.GetDomainAssembly()->FindIJWModule(hMod);

        if (result == NULL)
            continue;
        result->EnsureAllocated();
        return result->GetLoadedModule();
    }

    return NULL;
}

DomainFile *AppDomain::FindIJWDomainFile(HMODULE hMod, const SString &path)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    AssemblyIterator i = IterateAssembliesEx(STANDARD_IJW_ITERATOR_FLAGS);
    while (i.Next())
    {
        DomainAssembly* pasm=i.GetDomainAssembly();
        if(pasm->GetCurrentAssembly()==NULL)
            continue;

        DomainFile *result = pasm->GetCurrentAssembly()->FindIJWDomainFile(hMod, path);

        if (result != NULL)
            return result;
    }

    return NULL;
}

void AppDomain::SetFriendlyName(LPCWSTR pwzFriendlyName, BOOL fDebuggerCares/*=TRUE*/)
{
    CONTRACTL
    {
        THROWS;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Do all computations into a temporary until we're ensured of success
    SString tmpFriendlyName;


    if (pwzFriendlyName)
        tmpFriendlyName.Set(pwzFriendlyName);
    else
    {
        // If there is an assembly, try to get the name from it.
        // If no assembly, but if it's the DefaultDomain, then give it a name

        if (m_pRootAssembly)
        {
            tmpFriendlyName.SetUTF8(m_pRootAssembly->GetSimpleName());

            SString::Iterator i = tmpFriendlyName.End();
            if (tmpFriendlyName.FindBack(i, '.'))
                tmpFriendlyName.Truncate(i);
        }
        else
        {
            if (IsDefaultDomain())
                tmpFriendlyName.Set(DEFAULT_DOMAIN_FRIENDLY_NAME);

            // This is for the profiler - if they call GetFriendlyName on an AppdomainCreateStarted
            // event, then we want to give them a temporary name they can use.
            else if (GetId().m_dwId != 0)
            {
                tmpFriendlyName.Clear();
                tmpFriendlyName.Printf(L"%s %d", OTHER_DOMAIN_FRIENDLY_NAME_PREFIX, GetId().m_dwId);
            }
        }

    }

    tmpFriendlyName.Normalize();


    m_friendlyName = tmpFriendlyName;
    m_friendlyName.Normalize();

#ifdef DEBUGGING_SUPPORTED
    //    if(!g_pDebugInterface) return;
    _ASSERTE(NULL != g_pDebugInterface);

    // update the name in the IPC publishing block
    if (SUCCEEDED(g_pDebugInterface->UpdateAppDomainEntryInIPC(this)))
    {
        // inform the attached debugger that the name of this appdomain has changed.
        if (IsDebuggerAttached() && fDebuggerCares)
            g_pDebugInterface->NameChangeEvent(this, NULL);
    }

#endif // DEBUGGING_SUPPORTED
}

void AppDomain::ResetFriendlyName(BOOL fDebuggerCares/*=TRUE*/)
{
    WRAPPER_CONTRACT;
    SetFriendlyName(NULL, fDebuggerCares);
}

LPCWSTR AppDomain::GetFriendlyName(BOOL fDebuggerCares/*=TRUE*/)
{
    CONTRACT (LPCWSTR)
    {
        THROWS;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

#if _DEBUG
    // Handle NULL this pointer - this happens sometimes when printing log messages
    // but in general shouldn't occur in real code
    if (this == NULL)
        RETURN NULL;
#endif // _DEBUG

    if (m_friendlyName.IsEmpty())
        SetFriendlyName(NULL, fDebuggerCares);

    RETURN m_friendlyName;
}

LPCWSTR AppDomain::GetFriendlyNameForLogging()
{
    CONTRACT(LPWSTR)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL,NULL_OK));
    }
    CONTRACT_END;
#if _DEBUG
    // Handle NULL this pointer - this happens sometimes when printing log messages
    // but in general shouldn't occur in real code
    if (this == NULL)
        RETURN NULL;
#endif // _DEBUG
    RETURN (m_friendlyName.IsEmpty() ?L"":(LPCWSTR)m_friendlyName);
}

LPCWSTR AppDomain::GetFriendlyNameForDebugger()
{
    CONTRACT (LPCWSTR)
    {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_NOTRIGGER);}
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;


    if (m_friendlyName.IsEmpty())
    {
        BOOL fSuccess = FALSE;

        EX_TRY
        {
            SetFriendlyName(NULL);

            fSuccess = TRUE;
        }
        EX_CATCH
        {
            // Gobble all exceptions.
        }
        EX_END_CATCH(SwallowAllExceptions);

        if (!fSuccess)
        {
            RETURN L"";
        }
    }

    RETURN m_friendlyName;
}


#endif // !DACCESS_COMPILE

#ifdef DACCESS_COMPILE

PVOID AppDomain::GetFriendlyNameNoSet(bool* isUtf8)
{
    if (!m_friendlyName.IsEmpty())
    {
        *isUtf8 = false;
        return m_friendlyName.DacGetRawContent();
    }
    else if (m_pRootAssembly)
    {
        *isUtf8 = true;
        return (PVOID)m_pRootAssembly->GetSimpleName();
    }
    else if (PTR_HOST_TO_TADDR(this) ==
             PTR_HOST_TO_TADDR(SystemDomain::System()->DefaultDomain()))
    {
        *isUtf8 = false;
        return (PVOID)DEFAULT_DOMAIN_FRIENDLY_NAME;
    }
    else
    {
        return NULL;
    }
}

#endif // DACCESS_COMPILE

#define MAX_URL_LENGTH        2084 // same as INTERNET_MAX_URL_LENGTH

void AppDomain::CacheStringsForDAC()
{
    WRAPPER_CONTRACT;

    //
    // If the application base, private bin paths, and configuration file are
    // available, cache them so DAC can read them out of memory
    //
    if (m_pFusionContext)
    {
        CQuickBytes qb;
        LPWSTR ssz = (LPWSTR) qb.AllocThrows(MAX_URL_LENGTH * sizeof(WCHAR));

        DWORD dwSize;

        // application base
        ssz[0] = '\0';
        dwSize = MAX_URL_LENGTH * sizeof(WCHAR);
        m_pFusionContext->Get(ACTAG_APP_BASE_URL, ssz, &dwSize, 0);
        m_applicationBase.Set(ssz);

        // private bin paths
        ssz[0] = '\0';
        dwSize = MAX_URL_LENGTH * sizeof(WCHAR);
        m_pFusionContext->Get(ACTAG_APP_PRIVATE_BINPATH, ssz, &dwSize, 0);
        m_privateBinPaths.Set(ssz);

        // configuration file
        ssz[0] = '\0';
        dwSize = MAX_URL_LENGTH * sizeof(WCHAR);
        m_pFusionContext->Get(ACTAG_APP_CONFIG_FILE, ssz, &dwSize, 0);
        m_configFile.Set(ssz);
    }
}

#ifndef DACCESS_COMPILE

BOOL AppDomain::AddFileToCache(AssemblySpec* pSpec, PEAssembly *pFile)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    CrstPreempHolder holder(&m_DomainCacheCrst);
    // !!! suppress exceptions
    if(!m_AssemblyCache.StoreFile(pSpec, pFile, TRUE))
    {
    _ASSERTE(FALSE);
        EEFileLoadException::Throw(pSpec, FUSION_E_CACHEFILE_FAILED, NULL);
    }

    return TRUE;
}

BOOL AppDomain::AddAssemblyToCache(AssemblySpec* pSpec, DomainAssembly *pAssembly)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    CrstPreempHolder holder(&m_DomainCacheCrst);
    // !!! suppress exceptions
    return m_AssemblyCache.StoreAssembly(pSpec, pAssembly, TRUE);
}

BOOL AppDomain::AddExceptionToCache(AssemblySpec* pSpec, Exception *ex)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (ex->IsTransient())
        return TRUE;

    CrstPreempHolder holder(&m_DomainCacheCrst);
    // !!! suppress exceptions
    return m_AssemblyCache.StoreException(pSpec, ex, TRUE);
}

void AppDomain::AddUnmanagedImageToCache(LPCWSTR libraryName, HMODULE hMod)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(CheckPointer(libraryName));
        INJECT_FAULT(COMPlusThrowOM(););
        }
        CONTRACTL_END;

    if(libraryName) {
        AssemblySpec spec;
        spec.SetCodeBase(libraryName, (DWORD)(wcslen(libraryName)+1));
        m_UnmanagedCache.InsertEntry(&spec, hMod);
    }
    return ;
}

HMODULE AppDomain::FindUnmanagedImageInCache(LPCWSTR libraryName)
{
    CONTRACT(HMODULE)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(libraryName,NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL,NULL_OK));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    if(libraryName == NULL) RETURN NULL;

    AssemblySpec spec;
    spec.SetCodeBase(libraryName, (DWORD)(wcslen(libraryName) + 1));
    RETURN (HMODULE) m_UnmanagedCache.LookupEntry(&spec, 0);
}


BOOL AppDomain::IsCached(AssemblySpec *pSpec)
{
    WRAPPER_CONTRACT;

    // Check to see if this fits our rather loose idea of a reference to mscorlib.
    // If so, don't use fusion to bind it - do it ourselves.
    if (pSpec->IsMscorlib())
        return TRUE;

    return m_AssemblyCache.Contains(pSpec);
}


PEAssembly* AppDomain::FindCachedFile(AssemblySpec* pSpec)
{
    WRAPPER_CONTRACT;

    // Check to see if this fits our rather loose idea of a reference to mscorlib.
    // If so, don't use fusion to bind it - do it ourselves.
    if (pSpec->IsMscorlib()) {
        CONSISTENCY_CHECK(SystemDomain::System()->SystemAssembly() != NULL);
        PEAssembly *pFile = SystemDomain::System()->SystemFile();
        pFile->AddRef();
        return pFile;
    }

    return m_AssemblyCache.LookupFile(pSpec);
}



PEAssembly *AppDomain::BindAssemblySpec(AssemblySpec *pSpec, BOOL fThrowOnFileNotFound, BOOL fRaisePrebindEvents, StackCrawlMark *pCallerStackMark)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    PRECONDITION(CheckPointer(pSpec));
    PRECONDITION(pSpec->GetAppDomain() == this);
    PRECONDITION(this==::GetAppDomain());


    PEAssemblyHolder result;
    EX_TRY
    {
        if (!IsCached(pSpec))
        {
            if (fRaisePrebindEvents
                && (result = TryResolveAssembly(pSpec, TRUE /*fPreBind*/)) != NULL)
            {
                AddFileToCache(pSpec, result);
            }
            else
            {
                SafeComHolder<IAssembly> pIAssembly;
                SafeComHolder<IAssembly> pNativeFusionAssembly;
                SafeComHolder<IHostAssembly> pIHostAssembly;
                SafeComHolder<IFusionBindLog> pFusionLog;

                BOOL fIsWellKnown = pSpec->FindAssemblyFile(this, fThrowOnFileNotFound,
                                                            &pIAssembly, &pIHostAssembly, &pNativeFusionAssembly,
                                                            &pFusionLog,
                                                            pCallerStackMark);
                if (pIAssembly || pIHostAssembly) {

                    if (fIsWellKnown &&
                        m_pRootAssembly &&
                        pIAssembly == m_pRootAssembly->GetFusionAssembly())
                    {
                        // This is a shortcut to avoid opening another copy of the process exe.
                        // In fact, we have other similar cases where we've called
                        // ExplicitBind() rather than normal binding, which aren't covered here.


                        result = m_pRootAssembly->GetManifestFile();
                        result.SuppressRelease(); // Didn't get a refcount
                    }
                    else
                    {
                        BOOL isSystemAssembly = pSpec->IsMscorlibSatellite();
                        BOOL isIntrospectionOnly = pSpec->IsIntrospectionOnly();
                        if (pIAssembly)
                            result = PEAssembly::Open(pIAssembly, pNativeFusionAssembly, pFusionLog,
                                                      isSystemAssembly, isIntrospectionOnly);
                        else
                            result = PEAssembly::Open(pIHostAssembly, isSystemAssembly,
                                                      isIntrospectionOnly);
                    }


                    if (pSpec->IsIntrospectionOnly() && pSpec->GetCodeBase()->m_pszCodeBase != NULL)
                    {
                        IAssemblyName *pIAssemblyName = result->GetFusionAssemblyName();

                        AppDomain::AssemblyIterator i = IterateAssembliesEx( (AssemblyIterationFlags)(kIncludeLoaded | kIncludeIntrospection) );
                        while (i.Next())
                        {
                            DomainAssembly *pCachedDomainAssembly = i.GetDomainAssembly();
                            IAssemblyName *pCachedAssemblyName = pCachedDomainAssembly->GetAssembly()->GetFusionAssemblyName();
                            if (S_OK == (pCachedAssemblyName->IsEqual(pIAssemblyName, ASM_CMPF_IL_ALL)))
                            {
                                if (!(pCachedDomainAssembly->GetAssembly()->GetManifestModule()->GetFile()->Equals(result)))
                                {
                                    COMPlusThrow(kFileLoadException, IDS_EE_REFLECTIONONLY_LOADFROM, pSpec->GetCodeBase()->m_pszCodeBase);

                                }
                            }
                        }
                    }

                    AddFileToCache(pSpec, result);
                }
            }
        }
    }
    EX_CATCH
    {
        Exception *ex = GET_EXCEPTION();

        AssemblySpec NewSpec(this);
        BOOL fNewSpecUsed = FALSE;

        // Let transient exceptions propagate
        if (ex->IsTransient())
        {
            EX_RETHROW;
        }

        {
            // This is not executed for SO exceptions so we need to disable the backout
            // stack validation to prevent false violations from being reported.
            DISABLE_BACKOUT_STACK_VALIDATION;

            BOOL bFileNotFoundException = (EEFileLoadException::GetFileLoadKind(ex->GetHR()) == kFileNotFoundException);

            {
                // Fusion policy could have been applied,
                // so failed assembly could be not exactly what we ordered
                IAssemblyName *pRetName = pSpec->GetNameAfterPolicy();
                if(pRetName != NULL)
                {
                    NewSpec.InitializeSpec(pRetName, NULL, pSpec->IsIntrospectionOnly());
                    pSpec->ReleaseNameAfterPolicy();
                    fNewSpecUsed = !NewSpec.CompareEx(pSpec);
                }
            }

            // Non-"file not found" exception also propagate
            BOOL fFailure = TRUE;
            AssemblySpec* pFailedSpec = fNewSpecUsed ? &NewSpec : pSpec;
            if (bFileNotFoundException ||
                (ex->GetHR() == FUSION_E_REF_DEF_MISMATCH) ||
                (ex->GetHR() == FUSION_E_INVALID_NAME))
            {
                result = TryResolveAssembly(pFailedSpec, FALSE /*fPreBind*/);

                if(result != NULL)
                {
                    fFailure = FALSE;
                    AddFileToCache(pSpec, result);
                    if(fNewSpecUsed)
                        AddFileToCache(&NewSpec, result);
                }
            }
            if(fFailure)
            {
                if (!bFileNotFoundException)
                {
                    fFailure = AddExceptionToCache(pFailedSpec, ex);
                } // else, fFailure stays TRUE
                // Effectively, fFailure == bFileNotFoundException || AddExceptionToCache(pFailedSpec, ex)

                // Only throw this exception if we are the first in the cache
                if (fFailure)
                {
                    //
                    // If the BindingFailure MDA is enabled, trigger one for this failure
                    // Note: TryResolveAssembly() can also throw if an AssemblyResolve event subscriber throws
                    //       and the MDA isn't sent in this case (or for transient failure cases)
                    //

                    // In the same cases as for the MDA, store the failure information for DAC to read
                    if (IsDebuggerAttached()) {
                        FailedAssembly *pFailed = new FailedAssembly();
                        pFailed->Initialize(pFailedSpec, ex);
                        IfFailThrow(m_failedAssemblies.Append(pFailed));
                    }

                    if (!bFileNotFoundException || fThrowOnFileNotFound)
                    {

                        //
                        //
                        //
                        
                        if (ex->GetHR() == INET_E_RESOURCE_NOT_FOUND)
                        {
                           EEFileLoadException::Throw(pFailedSpec, COR_E_FILENOTFOUND, ex);
                        }

						
						if (!fNewSpecUsed && EEFileLoadException::CheckType(ex))
                        {
                            EX_RETHROW; //preserve the information
                        }
                        else
							EEFileLoadException::Throw(pFailedSpec, ex->GetHR(), ex);
                    }

                }
            }
        }
    }
    EX_END_CATCH(RethrowTerminalExceptions);

    // Now, we need to re-fetch the result from the cache, as we may have been racing with another
    // thread to store our result.  Note that we may throw from here, if there is a cached exception.
    // This will release the refcount of the current result holder (if any), and will replace
    // it with a non-addref'ed result
    result = FindCachedFile(pSpec);
    result.SuppressRelease();
    if (result != NULL)
        result->AddRef();


    return result;
}

DomainAssembly *AppDomain::BindAssemblySpecForIntrospectionDependencies(AssemblySpec *pSpec)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_SO_INTOLERANT;
    PRECONDITION(CheckPointer(pSpec));
    PRECONDITION(pSpec->GetAppDomain() == this);
    PRECONDITION(pSpec->IsIntrospectionOnly());
    PRECONDITION(this==::GetAppDomain());
    PEAssemblyHolder result;

    EX_TRY
    {
        if (!IsCached(pSpec))
        {
            result = TryResolveAssembly(pSpec, TRUE /*fPreBind*/);
            if (result != NULL)
                AddFileToCache(pSpec, result);
        }
    }
    EX_CATCH
    {
        Exception *ex = GET_EXCEPTION();
        AssemblySpec NewSpec(this);
        BOOL fNewSpecUsed = FALSE;

        // Let transient exceptions propagate
        if (ex->IsTransient())
        {
            EX_RETHROW;
        }

        {
            // Fusion policy could have been applied,
            // so failed assembly could be not exactly what we ordered
            IAssemblyName *pRetName = pSpec->GetNameAfterPolicy();
            if(pRetName != NULL)
            {
                NewSpec.InitializeSpec(pRetName, NULL, pSpec->IsIntrospectionOnly());
                pSpec->ReleaseNameAfterPolicy();
                fNewSpecUsed = !NewSpec.CompareEx(pSpec);
            }
        }

        BOOL bFileNotFoundException = (EEFileLoadException::GetFileLoadKind(ex->GetHR()) == kFileNotFoundException);
        // Non-"file not found" exception also propagate
        BOOL fFailure = TRUE;
        AssemblySpec* pFailedSpec = fNewSpecUsed ? &NewSpec : pSpec;
        if (bFileNotFoundException ||
            (ex->GetHR() == FUSION_E_REF_DEF_MISMATCH) ||
            (ex->GetHR() == FUSION_E_INVALID_NAME))
        {
            result = TryResolveAssembly(pFailedSpec, FALSE /*fPreBind*/);

            if(result != NULL)
            {
                fFailure = FALSE;
                AddFileToCache(pSpec, result);
                if(fNewSpecUsed)
                    AddFileToCache(&NewSpec, result);
            }
        }
        if(fFailure)
        {
            if (AddExceptionToCache(pFailedSpec, ex))
            {
                if (!fNewSpecUsed && EEFileLoadException::CheckType(ex))
                {
                    EX_RETHROW; //preserve the information
                }
                else
                    EEFileLoadException::Throw(pFailedSpec, ex->GetHR(), ex);
            }
        }
    }
    EX_END_CATCH(RethrowTerminalExceptions);

    result = FindCachedFile(pSpec);
    result.SuppressRelease();


    if (result)
    {
        // It was either already in the spec cache or the prebind event returned a result.
        return LoadDomainAssembly(pSpec, result, FILE_LOADED);
    }


    // Otherwise, look in the list of assemblies already loaded for reflectiononly.
    IAssemblyName *ptmp = NULL;
    HRESULT hr = pSpec->CreateFusionName(&ptmp);
    if (FAILED(hr))
    {
        COMPlusThrowHR(hr);
    }
    SafeComHolder<IAssemblyName> pIAssemblyName(ptmp);

    AppDomain::AssemblyIterator i = IterateAssembliesEx( (AssemblyIterationFlags)(kIncludeLoaded | kIncludeIntrospection) );
    while (i.Next())
    {
        DomainAssembly *pCachedDomainAssembly = i.GetDomainAssembly();
        IAssemblyName *pCachedAssemblyName = pCachedDomainAssembly->GetAssembly()->GetFusionAssemblyName();
        if (S_OK == (pCachedAssemblyName->IsEqual(pIAssemblyName, ASM_CMPF_IL_ALL)))
        {
            return pCachedDomainAssembly;
        }
    }

    // If not found in that list, it is an ERROR. Yes, this is by design.
    EEFileLoadException::Throw(pSpec, IDS_EE_REFLECTIONONLY_LOADFAILURE);
}


PEAssembly *AppDomain::TryResolveAssembly(AssemblySpec *pSpec, BOOL fPreBind)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;

    PEAssembly *result = NULL;

    EX_TRY
    {
        result = pSpec->ResolveAssemblyFile(this, fPreBind);
    }
    EX_HOOK
    {
        Exception *pEx = GET_EXCEPTION();

        if (!pEx->IsTransient())
        {
            AddExceptionToCache(pSpec, pEx);
            if (!EEFileLoadException::CheckType(pEx))
                EEFileLoadException::Throw(pSpec, pEx->GetHR(), pEx);
        }
    }
    EX_END_HOOK;

    return result;
}

void AppDomain::GetFileFromFusion(IAssembly *pIAssembly, LPCWSTR wszModuleName,
                                   SString &path)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    SafeComHolder<IAssemblyModuleImport> pImport;
    IfFailThrow(pIAssembly->GetModuleByName(wszModuleName, &pImport));

    if (!pImport->IsAvailable()) {
        AssemblySink* pSink = AllocateAssemblySink(NULL);
        SafeComHolder<IAssemblyBindSink> sinkholder(pSink);
        SafeComHolder<IAssemblyModuleImport> pResult;

        IfFailThrow(FusionBind::RemoteLoadModule(GetFusionContext(),
                                                 pImport,
                                                 pSink,
                                                 &pResult));
        pResult->AddRef();
        pImport.Assign(pResult);
    }

    DWORD dwPath = 0;
    pImport->GetModulePath(NULL, &dwPath);

    LPWSTR buffer = path.OpenUnicodeBuffer(dwPath-1);
    IfFailThrow(pImport->GetModulePath(buffer, &dwPath));
    path.CloseBuffer();
}



ULONG AppDomain::AddRef()
{
    LEAF_CONTRACT;
    return InterlockedIncrement(&m_cRef);
}

ULONG AppDomain::Release()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(m_cRef > 0);
    }
    CONTRACTL_END;

    ULONG   cRef = InterlockedDecrement(&m_cRef);
    if (!cRef)
    {
        _ASSERTE (m_Stage == STAGE_CREATING || m_Stage == STAGE_CLOSED);
        ADID adid=GetId();
        delete this;
        TESTHOOKCALL(AppDomainDestroyed(adid.m_dwId));
    }
    return (cRef);
}

AssemblySink* AppDomain::AllocateAssemblySink(AssemblySpec* pSpec)
{
    CONTRACT(AssemblySink *)
    {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    AssemblySink* ret = (AssemblySink*) FastInterlockExchangePointer((PVOID*) &m_pAsyncPool, NULL);

    if(ret == NULL)
        ret = new AssemblySink(this);
    else
        ret->AddRef();
    ret->SetAssemblySpec(pSpec);
    RETURN ret;
}

void AppDomain::RaiseUnloadDomainEvent_Wrapper(LPVOID ptr)
{
    CONTRACTL
    {
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    AppDomain* pDomain = (AppDomain *) ptr;
    pDomain->RaiseUnloadDomainEvent();
}

AppDomain* AppDomain::s_pAppDomainToRaiseUnloadEvent;
BOOL AppDomain::s_fProcessUnloadDomainEvent = FALSE;

void AppDomain::ProcessUnloadDomainEventOnFinalizeThread()
{
    Thread *pThread = GetThread();
    _ASSERTE (pThread && IsFinalizerThread());

    // if we are not unloading domain now, do not process the event
    if (SystemDomain::AppDomainBeingUnloaded() == NULL)
    {
        s_pAppDomainToRaiseUnloadEvent->SetStage(STAGE_UNLOAD_REQUESTED);
        s_pAppDomainToRaiseUnloadEvent->EnableADUnloadWorker(
            s_pAppDomainToRaiseUnloadEvent->IsRudeUnload()?EEPolicy::ADU_Rude:EEPolicy::ADU_Safe);
        FastInterlockExchangePointer((PVOID *)&s_pAppDomainToRaiseUnloadEvent, NULL);
        return;
    }
    FastInterlockExchange((LONG*)&s_fProcessUnloadDomainEvent, TRUE);
    AppDomain::EnableADUnloadWorkerForFinalizer();
    pThread->SetThreadStateNC(Thread::TSNC_RaiseUnloadEvent);
    s_pAppDomainToRaiseUnloadEvent->RaiseUnloadDomainEvent();
    pThread->SetThreadStateNC(Thread::TSNC_RaiseUnloadEvent);
    s_pAppDomainToRaiseUnloadEvent->EnableADUnloadWorker(
        s_pAppDomainToRaiseUnloadEvent->IsRudeUnload()?EEPolicy::ADU_Rude:EEPolicy::ADU_Safe);
    FastInterlockExchangePointer((PVOID *)&s_pAppDomainToRaiseUnloadEvent, NULL);
    FastInterlockExchange((LONG*)&s_fProcessUnloadDomainEvent, FALSE);

    if (pThread->IsAbortRequested())
    {
        pThread->UnmarkThreadForAbort(Thread::TAR_Thread);
    }
}

void AppDomain::RaiseUnloadDomainEvent()
{
    CONTRACTL
    {
        NOTHROW;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    EX_TRY
    {
        Thread *pThread = GetThread();
        if (this != pThread->GetDomain())
        {
            pThread->DoADCallBack(this, AppDomain::RaiseUnloadDomainEvent_Wrapper, this,ADV_FINALIZER|ADV_COMPILATION);
        }
        else
        {
            struct _gc
            {
                APPDOMAINREF Domain;
                OBJECTREF    Delegate;
            } gc;
            ZeroMemory(&gc, sizeof(gc));

            GCPROTECT_BEGIN(gc);
            gc.Domain = (APPDOMAINREF) GetRawExposedObject();
            if (gc.Domain != NULL)
            {
                gc.Delegate = gc.Domain->m_pDomainUnloadEventHandler;
                if (gc.Delegate != NULL)
                    DistributeEventReliably(&gc.Delegate, (OBJECTREF *) &gc.Domain);
            }
            GCPROTECT_END();
        }
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);
}

void AppDomain::RaiseLoadingAssemblyEvent(DomainAssembly *pAssembly)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        PRECONDITION(this == GetAppDomain());
        MODE_ANY;
    }
    CONTRACTL_END;

    GCX_COOP();
    FAULT_NOT_FATAL();
    EX_TRY
    {
        struct _gc {
            APPDOMAINREF AppDomainRef;
            OBJECTREF    orThis;
        } gc;
        ZeroMemory(&gc, sizeof(gc));

        if ((gc.AppDomainRef = (APPDOMAINREF) GetRawExposedObject()) != NULL) {
            if (gc.AppDomainRef->m_pAssemblyEventHandler != NULL)
            {
                ARG_SLOT args[2];
                GCPROTECT_BEGIN(gc);

                gc.orThis = pAssembly->GetExposedAssemblyObject();

                MethodDescCallSite  onAssemblyLoad(METHOD__APP_DOMAIN__ON_ASSEMBLY_LOAD, &gc.orThis);

                // GetExposedAssemblyObject may cause a gc, so call this before filling args[0]
                args[1] = ObjToArgSlot(gc.orThis);
                args[0] = ObjToArgSlot(gc.AppDomainRef);

                onAssemblyLoad.Call(args);

                GCPROTECT_END();
            }
        }
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);
}


BOOL
AppDomain::OnUnhandledException(OBJECTREF *pThrowable, BOOL isTerminating/*=TRUE*/) {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;

    BOOL retVal= FALSE;

    GCX_COOP();

    //
    //
    //
    //
    //

    AppDomain *pAppDomain = GetAppDomain();
    OBJECTREF orSender = 0;

    GCPROTECT_BEGIN(orSender);

    orSender = pAppDomain->GetRawExposedObject();

    retVal = pAppDomain->RaiseUnhandledExceptionEventNoThrow(&orSender, pThrowable, isTerminating);
    if (pAppDomain != SystemDomain::System()->DefaultDomain())
        retVal |= SystemDomain::System()->DefaultDomain()->RaiseUnhandledExceptionEventNoThrow
                        (&orSender, pThrowable, isTerminating);

    GCPROTECT_END();

    return retVal;
}


extern BOOL g_fEEStarted;


// Move outside of the AppDomain iteration, to avoid issues with the GC Frames being outside
// the domain transition.  This is a chronic issue that causes us to report roots for an AppDomain
// after we have left it.  This causes problems with AppDomain unloading that we only find
// with stress coverage..
void AppDomain::RaiseOneExitProcessEvent()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    struct _gc
    {
        APPDOMAINREF Domain;
        OBJECTREF    Delegate;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    EX_TRY {

    GCPROTECT_BEGIN(gc);
    gc.Domain = (APPDOMAINREF) SystemDomain::GetCurrentDomain()->GetRawExposedObject();
    if (gc.Domain != NULL)
    {
        gc.Delegate = gc.Domain->m_pProcessExitEventHandler;
        if (gc.Delegate != NULL)
            DistributeEventReliably(&gc.Delegate, (OBJECTREF *) &gc.Domain);
    }
    GCPROTECT_END();

    } EX_CATCH {
    } EX_END_CATCH(SwallowAllExceptions);
}

// Local wrapper used in AppDomain::RaiseExitProcessEvent,
// introduced solely to avoid stack overflow because of _alloca in the loop.
// It's just factored out body of the loop, but it has to be a member method of AppDomain,
// because it calls private RaiseOneExitProcessEvent
/*static*/ void AppDomain::RaiseOneExitProcessEvent_Wrapper(AppDomainIterator* pi)
{

    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;

    CONTRACT_VIOLATION(ThrowsViolation); // Work around SCAN bug

    EX_TRY {
    ENTER_DOMAIN_PTR(pi->GetDomain(),ADV_ITERATOR)
            AppDomain::RaiseOneExitProcessEvent();
        END_DOMAIN_TRANSITION;
    } EX_CATCH {
    } EX_END_CATCH(SwallowAllExceptions);
    }

static LONG s_ProcessedExitProcessEventCount = 0;

LONG GetProcessedExitProcessEventCount()
{
    return s_ProcessedExitProcessEventCount;
}

void AppDomain::RaiseExitProcessEvent()
{
    if (!g_fEEStarted)
        return;

    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;

    // Only finalizer thread during shutdown can call this function.
    _ASSERTE ((g_fEEShutDown&ShutDown_Finalize1) && GetThread() == GCHeap::GetGCHeap()->GetFinalizerThread());

    _ASSERTE (GetThread()->PreemptiveGCDisabled());

    _ASSERTE (GetThread()->GetDomain()->IsDefaultDomain());

        AppDomainIterator i(TRUE);
        while (i.Next())
        {
            RaiseOneExitProcessEvent_Wrapper(&i);
            FastInterlockIncrement(&s_ProcessedExitProcessEventCount);
        }
}

void AppDomain::RaiseUnhandledExceptionEvent_Wrapper(LPVOID ptr)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_INTOLERANT;
    }
    CONTRACTL_END;
    AppDomain::RaiseUnhandled_Args *args = (AppDomain::RaiseUnhandled_Args *) ptr;

    struct _gc {
        OBJECTREF orThrowable;
        OBJECTREF orSender;
    } gc;

    ZeroMemory(&gc, sizeof(gc));

    _ASSERTE(args->pTargetDomain == GetAppDomain());

    GCPROTECT_BEGIN(gc);
    EX_TRY
    {
        SetObjectReference(&gc.orThrowable,
                           AppDomainHelper::CrossContextCopyFrom(args->pExceptionDomain,
                                                                 args->pThrowable),
                           args->pTargetDomain);

        SetObjectReference(&gc.orSender,
                           AppDomainHelper::CrossContextCopyFrom(args->pExceptionDomain,
                                                                 args->pSender),
                           args->pTargetDomain);
    }
    EX_CATCH
    {
        SetObjectReference(&gc.orThrowable, GETTHROWABLE(), args->pTargetDomain);
        SetObjectReference(&gc.orSender, GetAppDomain()->GetRawExposedObject(), args->pTargetDomain);
    }
    EX_END_CATCH(SwallowAllExceptions)
    *(args->pResult) = args->pTargetDomain->RaiseUnhandledExceptionEvent(&gc.orSender,
                                                                         &gc.orThrowable,
                                                                         args->isTerminating);
    GCPROTECT_END();

}

BOOL
AppDomain::RaiseUnhandledExceptionEventNoThrow(OBJECTREF *pSender, OBJECTREF *pThrowable, BOOL isTerminating)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    BOOL bRetVal=FALSE;

    EX_TRY
    {
        bRetVal = RaiseUnhandledExceptionEvent(pSender, pThrowable, isTerminating);
    }
    EX_CATCH
    {
        #if _DEBUG
        HRESULT hr;
        hr = Security::MapToHR(GETTHROWABLE());
        #endif // _DEBUG
    }
    EX_END_CATCH(SwallowAllExceptions)  // Swallow any errors.
    return bRetVal;

}
BOOL
AppDomain::HasUnhandledExceptionEventHandler()
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        GC_NOTRIGGER; //essential
        NOTHROW;
    }
    CONTRACTL_END;
    if (!CanThreadEnter(GetThread()))
        return FALSE;
    if (GetRawExposedObject()==NULL)
        return FALSE;
    return (((APPDOMAINREF)GetRawExposedObject())->m_pUnhandledExceptionEventHandler!=NULL);
}

BOOL
AppDomain::RaiseUnhandledExceptionEvent(OBJECTREF *pSender, OBJECTREF *pThrowable, BOOL isTerminating)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (!HasUnhandledExceptionEventHandler())
        return FALSE;

    BOOL result = FALSE;

    _ASSERTE(pThrowable != NULL && IsProtectedByGCFrame(pThrowable));
    _ASSERTE(pSender    != NULL && IsProtectedByGCFrame(pSender));

    Thread *pThread = GetThread();
    if (this != pThread->GetDomain())
    {
        RaiseUnhandled_Args args = {pThread->GetDomain(), this, pSender, pThrowable, isTerminating, &result};
        // call through DoCallBack with a domain transition
        pThread->DoADCallBack(this, AppDomain::RaiseUnhandledExceptionEvent_Wrapper, &args, ADV_DEFAULTAD);
        return result;
    }

    OBJECTREF orDelegate = NULL;

    GCPROTECT_BEGIN(orDelegate);

    APPDOMAINREF orAD = (APPDOMAINREF) GetAppDomain()->GetRawExposedObject();

    if (orAD != NULL)
    {
        orDelegate = orAD->m_pUnhandledExceptionEventHandler;
        if (orDelegate != NULL)
        {
            result = TRUE;
            DistributeUnhandledExceptionReliably(&orDelegate, pSender, pThrowable, isTerminating);
        }
    }
    GCPROTECT_END();
    return result;
}



// Create a domain based on a string name
AppDomain* AppDomain::CreateDomainContext(__in WCHAR* fileName)
{
    CONTRACTL
    {
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if(fileName == NULL) return NULL;

    AppDomain* pDomain = NULL;

    MethodDescCallSite valCreateDomain(METHOD__APP_DOMAIN__VAL_CREATE_DOMAIN);

    STRINGREF pFilePath = NULL;
    GCPROTECT_BEGIN(pFilePath);
    pFilePath = COMString::NewString(fileName);

    ARG_SLOT args[1] =
    {
        ObjToArgSlot(pFilePath),
    };

    APPDOMAINREF pDom = (APPDOMAINREF) valCreateDomain.Call_RetOBJECTREF(args);
    if(pDom != NULL)
    {
        Context* pContext = Context::GetExecutionContext(pDom);
        if(pContext)
        {
            pDomain = pContext->GetDomain();
    }
    }
    GCPROTECT_END();

    return pDomain;
}

// You must be in the correct context before calling this
// routine. Therefore, it is only good for initializing the
// default domain.
void AppDomain::InitializeDomainContext(BOOL allowRedirects,
                                        LPCWSTR pwszPath,
                                        LPCWSTR pwszConfig)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        THROWS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    struct _gc {
        STRINGREF pFilePath;
        STRINGREF pConfig;
        OBJECTREF ref;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);
    if(pwszPath)
    {
        gc.pFilePath = COMString::NewString(pwszPath);
    }

    if(pwszConfig)
    {
        gc.pConfig = COMString::NewString(pwszConfig);
    }

    if ((gc.ref = GetExposedObject()) != NULL)
    {
        MethodDescCallSite setupDomain(METHOD__APP_DOMAIN__SETUP_DOMAIN);

        ARG_SLOT args[4] =
        {
            ObjToArgSlot(gc.ref),
                allowRedirects,
                ObjToArgSlot(gc.pFilePath),
                ObjToArgSlot(gc.pConfig),
        };
        setupDomain.Call(args);
    }
    GCPROTECT_END();

    CacheStringsForDAC();
}


void AppDomain::SetupLoaderOptimization(DWORD optimization)
{
    CONTRACTL
    {
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if ((GetExposedObject()) != NULL)
    {
        MethodDescCallSite setupLoaderOptimization(METHOD__APP_DOMAIN__SETUP_LOADER_OPTIMIZATION);

        ARG_SLOT args[2] =
        {
            ObjToArgSlot(GetExposedObject()),
            optimization
        };
        setupLoaderOptimization.Call(args);
    }
}

// The fusion context should only be null when appdomain is being setup
// and there should be no reason to protect the creation.
IApplicationContext *AppDomain::CreateFusionContext()
{
    CONTRACT(IApplicationContext *)
    {
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    if(!m_pFusionContext) {

        MEMORY_REPORT_CONTEXT_SCOPE("FusionContext");
        ETWTraceStartup traceFusion(ETW_TYPE_STARTUP_FUSIONAPPCTX);

        SafeComHolder<IApplicationContext> pFusionContext;

        GCX_PREEMP();

        IfFailThrow(FusionBind::CreateFusionContext(NULL, &pFusionContext));


        pFusionContext.SuppressRelease();
        m_pFusionContext = pFusionContext;

        DWORD dwId = m_dwId.m_dwId;
        IfFailThrow(m_pFusionContext->Set(ACTAG_APP_DOMAIN_ID, &dwId, sizeof(DWORD), 0));
    }

    RETURN m_pFusionContext;
}

void AppDomain::TurnOnBindingRedirects()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    if ((GetExposedObject()) != NULL)
    {
        MethodDescCallSite turnOnBindingRedirects(METHOD__APP_DOMAIN__TURN_ON_BINDING_REDIRECTS);
        ARG_SLOT args[1] =
        {
            ObjToArgSlot(GetExposedObject()),
        };
        turnOnBindingRedirects.Call(args);
    }

    IfFailThrow(m_pFusionContext->Set(ACTAG_DISALLOW_APP_BINDING_REDIRECTS,
                                      NULL,
                                      0,
                                      0));
}

void AppDomain::SetupExecutableFusionContext(__in WCHAR *exePath)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(GetAppDomain() == this);
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    struct _gc {
        STRINGREF pFilePath;
        OBJECTREF ref;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);
    gc.pFilePath = COMString::NewString(exePath);

    if ((gc.ref = GetExposedObject()) != NULL)
    {
        MethodDescCallSite setDomainContext(METHOD__APP_DOMAIN__SET_DOMAIN_CONTEXT, &gc.ref);
        ARG_SLOT args[2] =
        {
            ObjToArgSlot(gc.ref),
            ObjToArgSlot(gc.pFilePath),
        };
        setDomainContext.Call(args);
    }

    GCPROTECT_END();

}

BOOL AppDomain::SetContextProperty(IApplicationContext* pFusionContext,
                                   LPCWSTR pProperty, OBJECTREF* obj)

{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if(obj && ((*obj) != NULL)) {
        MethodTable* pMT = (*obj)->GetMethodTable();
        DWORD lgth;

        if(g_Mscorlib.IsClass(pMT, CLASS__STRING)) {

            lgth = (ObjectToSTRINGREF(*(StringObject**)obj))->GetStringLength();
            CQuickBytes qb;
            LPWSTR wszValue = (LPWSTR) qb.AllocThrows((lgth+1)*sizeof(WCHAR));
            memcpy(wszValue, (ObjectToSTRINGREF(*(StringObject**)obj))->GetBuffer(), lgth*sizeof(WCHAR));
            if(lgth > 0 && wszValue[lgth-1] == '/')
                lgth--;
            wszValue[lgth] = L'\0';

            LOG((LF_LOADER,
                 LL_INFO10,
                 "Set: %S: *%S*.\n",
                 pProperty, wszValue));

            IfFailThrow(pFusionContext->Set(pProperty,
                                            wszValue,
                                            (lgth+1) * sizeof(WCHAR),
                                            0));
        }
        else {
            // Pin byte array for loading
            Wrapper<OBJECTHANDLE, DoNothing, DestroyPinningHandle> handle(
        GetAppDomain()->CreatePinningHandle(*obj));

            const BYTE *pbArray = ((U1ARRAYREF)(*obj))->GetDirectConstPointerToNonObjectElements();
            DWORD cbArray = (*obj)->GetNumComponents();

            IfFailThrow(pFusionContext->Set(pProperty,
                                            (LPVOID) pbArray,
                                            cbArray,
                                            0));
        }
    }

    return TRUE;
}


BOOL BaseDomain::IsShadowCopyOn()
{
    LEAF_CONTRACT;

    return m_fShadowCopy;
}

LPWSTR AppDomain::GetDynamicDir()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_pwDynamicDir == NULL) {

        BaseDomain::LockHolder lh(this);

        if(m_pwDynamicDir == NULL) {
            IApplicationContext* pFusionContext = GetFusionContext();
            _ASSERTE(pFusionContext);

            HRESULT hr = S_OK;
            DWORD dwSize = 0;
            hr = pFusionContext->GetDynamicDirectory(NULL, &dwSize);
            AllocMemHolder<WCHAR> tempDynamicDir;

            if(hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                tempDynamicDir = m_pLowFrequencyHeap->AllocMem(dwSize * sizeof(WCHAR));
                hr = pFusionContext->GetDynamicDirectory(tempDynamicDir, &dwSize);
            }
            if(hr==HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
                return NULL;
            IfFailThrow(hr);

            tempDynamicDir.SuppressRelease();
            m_pwDynamicDir = tempDynamicDir;
        }
        // lh out of scope here
    }

    return m_pwDynamicDir;;
}

#ifdef DEBUGGING_SUPPORTED
void AppDomain::SetDebuggerAttached(DWORD dwStatus)
{
    CONTRACTL
    {
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
    }
    CONTRACTL_END;

    // first, reset the debugger bits
    m_dwFlags &= ~DEBUGGER_STATUS_BITS_MASK;

    // then set the bits to the desired value
    m_dwFlags |= dwStatus;

    // Can't call GetFriendlyNameXXX b/c it's a contract violation
    LOG((LF_CORDB, LL_EVERYTHING, "AD::SDA AD:%#08x status:%#x flags:%#x\n",
        this, dwStatus, m_dwFlags));
}

DWORD AppDomain::GetDebuggerAttached(void)
{
    LEAF_CONTRACT;
    LOG((LF_CORDB, LL_EVERYTHING, "AD::GD this;0x%x val:0x%x\n", this,
        m_dwFlags & DEBUGGER_STATUS_BITS_MASK));

    return m_dwFlags & DEBUGGER_STATUS_BITS_MASK;
}

BOOL AppDomain::IsDebuggerAttached(void)
{
    LEAF_CONTRACT;
    LOG((LF_CORDB, LL_EVERYTHING, "AD::IDA this;0x%x flags:0x%x\n",
        this, m_dwFlags));

    // Of course, we can't have a debugger attached to this AD if there isn't a debugger attached to the whole
    // process...
    if (CORDebuggerAttached())
        return ((m_dwFlags & DEBUGGER_ATTACHED) == DEBUGGER_ATTACHED) ? TRUE : FALSE;
    else
        return FALSE;
}

// This is called from the debugger to request notification events from
// Assemblies, Modules, Types in this appdomain.
BOOL AppDomain::NotifyDebuggerLoad(int flags, BOOL attaching)
{
    WRAPPER_CONTRACT;
    BOOL result = FALSE;

    if (!attaching && !IsDebuggerAttached())
        return FALSE;

    AssemblyIterator i;

    // Attach to our assemblies
    LOG((LF_CORDB, LL_INFO100, "AD::NDA: Iterating assemblies\n"));
    i = IterateAssembliesEx((AssemblyIterationFlags)(kIncludeLoaded | kIncludeLoading | kIncludeExecution));
    while (i.Next())
    {
        result = i.GetDomainAssembly()->NotifyDebuggerLoad(flags, attaching) || result;
    }

    return result;
}

void AppDomain::NotifyDebuggerUnload()
{
    WRAPPER_CONTRACT;    
    if (!IsDebuggerAttached())
        return;

    LOG((LF_CORDB, LL_INFO10, "AD::NDD domain [%d] %#08x %ls\n",
         GetId().m_dwId, this, GetFriendlyNameForLogging()));

    LOG((LF_CORDB, LL_INFO100, "AD::NDD: Interating domain bound assemblies\n"));
    AssemblyIterator i = IterateAssembliesEx((AssemblyIterationFlags)(kIncludeLoaded |  kIncludeLoading  | kIncludeExecution));

    // Detach from our assemblies
    while (i.Next())
    {
        LOG((LF_CORDB, LL_INFO100, "AD::NDD: Iterating assemblies\n"));
        i.GetDomainAssembly()->NotifyDebuggerUnload();
    }
}
#endif // DEBUGGING_SUPPORTED

void AppDomain::SetSystemAssemblyLoadEventSent(BOOL fFlag)
{
    LEAF_CONTRACT;
    if (fFlag == TRUE)
        m_dwFlags |= LOAD_SYSTEM_ASSEMBLY_EVENT_SENT;
    else
        m_dwFlags &= ~LOAD_SYSTEM_ASSEMBLY_EVENT_SENT;
}

BOOL AppDomain::WasSystemAssemblyLoadEventSent(void)
{
    LEAF_CONTRACT;
    return ((m_dwFlags & LOAD_SYSTEM_ASSEMBLY_EVENT_SENT) == 0) ? FALSE : TRUE;
}

BOOL AppDomain::IsDomainBeingCreated(void)
{
    LEAF_CONTRACT;
    return ((m_dwFlags & APP_DOMAIN_BEING_CREATED) ? TRUE : FALSE);
}

void AppDomain::SetDomainBeingCreated(BOOL flag)
{
    LEAF_CONTRACT;

    if (flag == TRUE)
        m_dwFlags |= APP_DOMAIN_BEING_CREATED;
    else
        m_dwFlags &= ~APP_DOMAIN_BEING_CREATED;
}


BOOL AppDomain::CanThreadEnter(Thread *pThread)
{
    WRAPPER_CONTRACT;

    if (m_Stage < STAGE_EXITED)
        return TRUE;

    if (pThread == SystemDomain::System()->GetUnloadingThread())
        return m_Stage < STAGE_FINALIZING;
    if (pThread == GCHeap::GetGCHeap()->GetFinalizerThread())
        return m_Stage < STAGE_FINALIZED;

    return FALSE;
}

void AppDomain::AllowThreadEntrance(AppDomain * pApp)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pApp));
    }
    CONTRACTL_END;

    if (pApp->GetUnloadRequestThread() == NULL)
    {
        // This is asynchonous unload, either by a host, or by AppDomain.Unload from AD unload event.
        if (!pApp->IsUnloadingFromUnloadEvent())
        {
            pApp->SetStage(STAGE_UNLOAD_REQUESTED);
            pApp->EnableADUnloadWorker(
                 pApp->IsRudeUnload()?EEPolicy::ADU_Rude:EEPolicy::ADU_Safe);
            return;
        }
    }

    SystemDomain::LockHolder lh; // we don't want to reopen appdomain if other thread can be preparing to unload it


    pApp->SetStage(STAGE_OPEN);
}

void AppDomain::RestrictThreadEntrance(AppDomain * pApp)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
        DISABLED(FORBID_FAULT);
        PRECONDITION(CheckPointer(pApp));
    }
    CONTRACTL_END;


    SystemDomain::LockHolder lh; // we don't want to reopen appdomain if other thread can be preparing to unload it
    // Release our ID so remoting and thread pool won't enter
    pApp->SetStage(STAGE_EXITED);
};

void AppDomain::Exit(BOOL fRunFinalizers, BOOL fAsyncExit)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    LOG((LF_APPDOMAIN | LF_CORDB, LL_INFO10, "AppDomain::Exiting domain [%d] %#08x %ls\n",
         GetId().m_dwId, this, GetFriendlyNameForLogging()));

    RestrictEnterHolder RestrictEnter(this);

    {
    SystemDomain::LockHolder lh; // we don't want to close appdomain if other thread can be preparing to unload it
    SetStage(STAGE_EXITING);  // Note that we're trying to exit
    }

    // Raise the event indicating the domain is being unloaded.
    if (GetDefaultContext())
    {
        FastInterlockExchangePointer((PVOID *)&s_pAppDomainToRaiseUnloadEvent, this);

        DWORD timeout = GetEEPolicy()->GetTimeout(m_fRudeUnload?OPR_AppDomainRudeUnload : OPR_AppDomainUnload);
        //if (timeout == INFINITE)
        //{
        //    timeout = 20000; // 20 seconds
        //}
        DWORD timeoutForFinalizer = GetEEPolicy()->GetTimeout(OPR_FinalizerRun);
        ULONGLONG curTime = CLRGetTickCount64();
        ULONGLONG endTime = 0;
        if (timeout != INFINITE)
        {
            endTime = curTime + timeout;
            // We will try to kill AD unload event if it takes too long, and then we move on to the next registered caller.
            timeout /= 5;
        }

        while (s_pAppDomainToRaiseUnloadEvent != NULL)
        {
            GCHeap::GetGCHeap()->FinalizerThreadWait(s_fProcessUnloadDomainEvent?timeout:timeoutForFinalizer);
            if (endTime != 0 && s_pAppDomainToRaiseUnloadEvent != NULL)
            {
                if (CLRGetTickCount64() >= endTime)
                {
                    SString sThreadId;
                    sThreadId.Printf(L"%x", GCHeap::GetGCHeap()->GetFinalizerThread()->GetThreadId());
                    COMPlusThrow(kCannotUnloadAppDomainException,
                                 IDS_EE_ADUNLOAD_CANT_UNWIND_THREAD,
                                 sThreadId);
                }
            }
        }
    }


    //
    // Set up blocks so no threads can enter except for the finalizer and the thread
    // doing the unload.
    //

    RestrictThreadEntrance(this);





    // Cause existing threads to abort out of this domain.  This should ensure all
    // normal threads are outside the domain, and we've already ensured that no new threads
    // can enter.

    UnwindThreads();
    
    TESTHOOKCALL(UnwoundThreads(GetId().m_dwId)) ;    
    ProcessEventForHost(Event_DomainUnload, (PVOID)(UINT_PTR)GetId().m_dwId);

    RestrictEnter.SuppressRelease(); //after this point we don't guarantee appdomain consistency
#ifdef PROFILING_SUPPORTED
    // Signal profile if present.
    if (CORProfilerTrackAppDomainLoads())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->AppDomainShutdownStarted((ThreadID) GetThread(), (AppDomainID) this);
    }
#endif // PROFILING_SUPPORTED
    COUNTER_ONLY(GetPrivatePerfCounters().m_Loading.cAppDomains--);
    COUNTER_ONLY(GetPrivatePerfCounters().m_Loading.cAppDomainsUnloaded++);



    LOG((LF_APPDOMAIN | LF_CORDB, LL_INFO10, "AppDomain::Domain [%d] %#08x %ls is exited.\n",
         GetId().m_dwId, this, GetFriendlyNameForLogging()));



    //
    // Spin running finalizers until we flush them all.  We need to make multiple passes
    // in case the finalizers create more finalizable objects.  This is important to clear
    // the finalizable objects as roots, as well as to actually execute the finalizers. This
    // will only finalize instances instances of types that aren't potentially agile becuase we can't
    // risk finalizing agile objects. So we will be left with instances of potentially agile types
    // in handles or statics.
    //
    //

    SetStage(STAGE_FINALIZING);

    // Flush finalizers now.
    GCHeap::GetGCHeap()->UnloadAppDomain(this, fRunFinalizers);
    DWORD timeout = GetEEPolicy()->GetTimeout(m_fRudeUnload?OPR_AppDomainRudeUnload : OPR_AppDomainUnload);
    ULONGLONG curTime = CLRGetTickCount64();
    ULONGLONG endTime = 0;
    if (timeout != INFINITE)
    {
        endTime = curTime + timeout;
    }

    while (GCHeap::GetGCHeap()->GetUnloadingAppDomain() != NULL)
    {
        if (endTime != 0)
        {
            timeout = endTime - CLRGetTickCount64();
        }
        GCHeap::GetGCHeap()->FinalizerThreadWait(timeout); //will set stage to finalized
        if (endTime != 0 && GCHeap::GetGCHeap()->GetUnloadingAppDomain() != NULL)
        {
            if (CLRGetTickCount64() >= endTime)
            {
                SetRudeUnload();
                endTime = 0;
                timeout = INFINITE;
            }
        }
    }


    LOG((LF_APPDOMAIN | LF_CORDB, LL_INFO10, "AppDomain::Domain [%d] %#08x %ls is finalized.\n",
         GetId().m_dwId, this, GetFriendlyNameForLogging()));


    AppDomainRefHolder This(this);
    AddRef();           // Hold a reference so CloseDomain won't delete us yet
    CloseDomain();      // Remove ourself from the list of app domains

    // This needs to be done prior to destroying the handle tables below.
    ReleaseDomainBoundInfo();
    //
    // It should be impossible to run non-mscorlib code in this domain now.
    // Nuke all of our roots except the handles. We do this to allow as many
    // finalizers as possible to run correctly. If we delete the handles, they
    // can't run.
    //
    EX_TRY
    {
        ADID domainId = GetId();
        MethodDescCallSite  domainUnloaded(s_remoteServiceUnloadMD,
                                           METHOD__REMOTING_SERVICES__DOMAIN_UNLOADED);

        ARG_SLOT args[1];
        args[0] = domainId.m_dwId;
        domainUnloaded.Call(args);
    }
    EX_CATCH
    {
        //we don't care if it fails
    }
    EX_END_CATCH(SwallowAllExceptions);


    ClearGCRoots();
    ClearGCHandles ();

    LOG((LF_APPDOMAIN | LF_CORDB, LL_INFO10, "AppDomain::Domain [%d] %#08x %ls is cleared.\n",
         GetId().m_dwId, this, GetFriendlyNameForLogging()));

    if (fAsyncExit && fRunFinalizers)
   {
        m_AssemblyCache.Clear();
        ReleaseFiles();
   }
    AddMemoryPressure();
    SystemDomain::System()->AddToDelayedUnloadList(this, fAsyncExit);
    SystemDomain::SetUnloadDomainCleared();
    if (m_dwId.m_dwId!=0)
        SystemDomain::ReleaseAppDomainId(m_dwId);
#ifdef PROFILING_SUPPORTED
    // Always signal profile if present, even when failed.
    if (CORProfilerTrackAppDomainLoads())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->AppDomainShutdownFinished((ThreadID) GetThread(), (AppDomainID) this, S_OK);
    }
#endif // PROFILING_SUPPORTED

}

void AppDomain::Close()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        NOTHROW;
    }
    CONTRACTL_END;

    LOG((LF_APPDOMAIN | LF_CORDB, LL_INFO10, "AppDomain::Domain [%d] %#08x %ls is collected.\n",
         GetId().m_dwId, this, GetFriendlyNameForLogging()));


#if CHECK_APP_DOMAIN_LEAKS
    if (g_pConfig->AppDomainLeaks())
        // at this point shouldn't have any non-agile objects in the heap because we finalized all the non-agile ones.
        SyncBlockCache::GetSyncBlockCache()->CheckForUnloadedInstances(GetIndex());
#endif // CHECK_APP_DOMAIN_LEAKS
    RemoveMemoryPressure();
    _ASSERTE(m_cRef>0); //should be alive at this point otherwise iterator can revive us and crash
    {
        SystemDomain::LockHolder lh;    // Avoid races with AppDomainIterator
        SetStage(STAGE_CLOSED);
    }

}


void AppDomain::ResetUnloadRequestThread(ADID Id)
{
    CONTRACTL
    {
        NOTHROW;
        MODE_ANY;
        PRECONDITION(!IsADUnloadHelperThread());
    }
    CONTRACTL_END;

    GCX_COOP();
    AppDomainFromIDHolder ad(Id, TRUE);
    if(!ad.IsUnloaded() && ad->m_Stage < STAGE_UNLOAD_REQUESTED)
    {
        Thread *pThread = ad->GetUnloadRequestThread();
        if(pThread==::GetThread())
        {
            ad->m_dwThreadsStillInAppDomain=(ULONG)-1;

            if(pThread)
            {
                if (pThread->GetUnloadBoundaryFrame() && pThread->IsBeingAbortedForADUnload())
                {
                    pThread->UnmarkThreadForAbort(Thread::TAR_ADUnload);
                }
                ad->GetUnloadRequestThread()->ResetUnloadBoundaryFrame();
                pThread->ResetBeginAbortedForADUnload();
            }
            
            ad->SetUnloadRequestThread(NULL);
        }
    }
}


int g_fADUnloadWorkerOK = -1;

HRESULT AppDomain::UnloadById(ADID dwId, BOOL fSync,BOOL fExceptionsPassThrough)
{
    CONTRACTL
    {
        if(fExceptionsPassThrough) {THROWS;} else {NOTHROW;}
        MODE_ANY;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_TRIGGERS);}
        FORBID_FAULT;
    }
    CONTRACTL_END;

   if (dwId==(ADID)DefaultADID)
        return COR_E_CANNOTUNLOADAPPDOMAIN;

   Thread *pThread = GetThread();

   // Finalizer thread can not wait until AD unload is done,
   // because AD unload is going to wait for Finalizer Thread.
   if (fSync && pThread == GCHeap::GetGCHeap()->GetFinalizerThread() && 
       !pThread->HasThreadStateNC(Thread::TSNC_RaiseUnloadEvent))
       return COR_E_CANNOTUNLOADAPPDOMAIN;


    // AD unload helper thread should have been created.
    _ASSERTE (g_fADUnloadWorkerOK == 1);

    _ASSERTE (!IsADUnloadHelperThread());

    BOOL fIsRaisingUnloadEvent = (pThread != NULL && pThread->HasThreadStateNC(Thread::TSNC_RaiseUnloadEvent));

    SystemDomain::LockHolder ulh(!fIsRaisingUnloadEvent);

   AppDomainFromIDHolder pApp(dwId, TRUE, fIsRaisingUnloadEvent?
                                    AppDomainFromIDHolder::SyncType_GC:
                                    AppDomainFromIDHolder::SyncType_ADLock);

   if (pApp.IsUnloaded() || ! pApp->CanLoadCode() || pApp->GetId().m_dwId == 0)
        return COR_E_APPDOMAINUNLOADED;



#ifdef _DEBUG
    DWORD hostTestADUnload = g_pConfig->GetHostTestADUnload();
    if (hostTestADUnload == 2) {
        pApp->SetRudeUnload();
    }
#endif // _DEBUG

    if (fIsRaisingUnloadEvent)
    {
        //DebugBreak();
#ifdef _DEBUG
        pApp->EnableADUnloadWorker(hostTestADUnload != 2?EEPolicy::ADU_Safe:EEPolicy::ADU_Rude);
#else // !_DEBUG
        pApp->EnableADUnloadWorker(EEPolicy::ADU_Safe);
#endif // !_DEBUG
        return S_FALSE;
    }

    if (g_fADUnloadWorkerOK == 1) {
        if (!fSync)
        {
#ifdef _DEBUG
            pApp->EnableADUnloadWorker(hostTestADUnload != 2?EEPolicy::ADU_Safe:EEPolicy::ADU_Rude);
#else // !_DEBUG
            pApp->EnableADUnloadWorker(EEPolicy::ADU_Safe);
#endif // !_DEBUG
            pApp.Release();
            ulh.Release(); // we don't care anymore if the appdomain is gone

            return S_OK;
        }
        ADUnloadSinkHolder pSink(pApp->PrepareForWaitUnloadCompletion());

#ifdef _DEBUG
        pApp->EnableADUnloadWorker(hostTestADUnload != 2?EEPolicy::ADU_Safe:EEPolicy::ADU_Rude);
#else // !_DEBUG
        pApp->EnableADUnloadWorker(EEPolicy::ADU_Safe);
#endif // !_DEBUG
        pApp.Release();
        ulh.Release(); // we don't care anymore if the appdomain is gone

        CONTRACT_VIOLATION(FaultViolation);
            return UnloadWait(dwId,pSink);
    }
    else
    {
        _ASSERTE(FALSE);
        return E_UNEXPECTED;
    }    
}

HRESULT AppDomain::UnloadWait(ADID Id, ADUnloadSink * pSink)
{
    CONTRACTL
    {
        NOTHROW;
        MODE_ANY;
        if (GetThread()) {GC_TRIGGERS;} else {DISABLED(GC_TRIGGERS);}
    }
    CONTRACTL_END;
    HRESULT hr=S_OK;
    EX_TRY
    {
        // IF you ever try to change this to something not using events, please address the fact that
        // AppDomain::StopEEAndUnwindThreads relies on that events are used.

            pSink->WaitUnloadCompletion();
    }
    EX_CATCH_HRESULT(hr);

    if (SUCCEEDED(hr))
        hr=pSink->GetUnloadResult();

    if (FAILED(hr))
    {
        ResetUnloadRequestThread(Id);
    }
    return hr;
}


void AppDomain::Unload(BOOL fForceUnload)
{
    CONTRACTL
    {
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    Thread *pThread = GetThread();


    if (! fForceUnload && !g_pConfig->AppDomainUnload())
        return;

    EPolicyAction action;
    EClrOperation operation;
    if (!IsRudeUnload())
    {
        operation = OPR_AppDomainUnload;
    }
    else
    {
        operation = OPR_AppDomainRudeUnload;
    }
    action = GetEEPolicy()->GetDefaultAction(operation,NULL);
    GetEEPolicy()->NotifyHostOnDefaultAction(operation,action);

    switch (action)
    {
    case eUnloadAppDomain:
        break;
    case eRudeUnloadAppDomain:
        SetRudeUnload();
        break;
    case eExitProcess:
    case eFastExitProcess:
    case eRudeExitProcess:
    case eDisableRuntime:
        EEPolicy::HandleExitProcessFromEscalation(action, HOST_E_EXITPROCESS_ADUNLOAD);
        _ASSERTE (!"Should not get here");
        break;
    default:
        break;
    }

#if (defined(_DEBUG) || defined(BREAK_ON_UNLOAD) || defined(AD_LOG_MEMORY) || defined(AD_SNAPSHOT))
    static int unloadCount = 0;
#endif

#ifdef AD_LOG_MEMORY
    GCX_PREEMP();
    static int logMemory = g_pConfig->GetConfigDWORD(L"ADLogMemory", 0);
    typedef void (__cdecl *LogItFcn) ( int );
    static LogItFcn pLogIt = NULL;

    if (logMemory && ! pLogIt)
    {
        HMODULE hMod = CLRLoadLibrary(L"mpdh.dll");
        if (hMod)
        {
            pThread->EnablePreemptiveGC();
            pLogIt = (LogItFcn)GetProcAddress(hMod, "logIt");
            if (pLogIt)
            {
                pLogIt(9999);
                pLogIt(9999);
            }
            pThread->DisablePreemptiveGC();
        }
    }
#endif // AD_LOG_MEMORY

    if (IsDefaultDomain())
        COMPlusThrow(kCannotUnloadAppDomainException, IDS_EE_ADUNLOAD_DEFAULT);

    _ASSERTE(CanUnload());

    if (pThread == GCHeap::GetGCHeap()->GetFinalizerThread() || GetUnloadRequestThread() == GCHeap::GetGCHeap()->GetFinalizerThread())
        COMPlusThrow(kCannotUnloadAppDomainException, IDS_EE_ADUNLOAD_IN_FINALIZER);

    _ASSERTE(! SystemDomain::AppDomainBeingUnloaded());

    // should not be running in this AD because unload spawned thread in default domain
    _ASSERTE(!pThread->IsRunningIn(this, NULL));

#ifdef APPDOMAIN_STATE
    _ASSERTE_ALL_BUILDS(pThread->GetDomain()->IsDefaultDomain());
#endif

    LOG((LF_APPDOMAIN | LF_CORDB, LL_INFO10, "AppDomain::Unloading domain [%d] %#08x %ls\n", GetId().m_dwId, this, GetFriendlyName()));

    STRESS_LOG3 (LF_APPDOMAIN, LL_INFO100, "Unload domain [%d, %d] %p\n", GetId().m_dwId, GetIndex().m_dwIndex, this);

    UnloadHolder hold(this);

    SystemDomain::System()->SetUnloadRequestingThread(GetUnloadRequestThread());
    SystemDomain::System()->SetUnloadingThread(pThread);


#ifdef _DEBUG
    static int dumpSB = -1;

    if (dumpSB == -1)
        dumpSB = g_pConfig->GetConfigDWORD(L"ADDumpSB", 0);

    if (dumpSB > 1)
    {
        LogSpewAlways("Starting unload %3.3d\n", unloadCount);
        DumpSyncBlockCache();
    }
#endif // _DEBUG

    BOOL bForceGC=m_bForceGCOnUnload;

#ifdef AD_LOG_MEMORY
    if (pLogIt)
        bForceGC=TRUE;
#endif // AD_LOG_MEMORY

#ifdef AD_SNAPSHOT
    static int takeSnapShot = -1;

    if (takeSnapShot == -1)
        takeSnapShot = g_pConfig->GetConfigDWORD(L"ADTakeSnapShot", 0);

    if (takeSnapShot)
        bForceGC=TRUE;
#endif // AD_SNAPSHOT

#ifdef _DEBUG
    static int dbgAllocReport = -1;

    if (dbgAllocReport == -1)
        dbgAllocReport = g_pConfig->GetConfigDWORD(L"ADDbgAllocReport", 0);

    if (dbgAllocReport || dumpSB > 0)
        bForceGC=TRUE;
#endif // _DEBUG
    static int cfgForceGC = -1;

    if (cfgForceGC == -1)
        cfgForceGC =!g_pConfig->GetConfigDWORD(L"ADULazyMemoryRelease", 1);

    bForceGC=bForceGC||cfgForceGC;
    AppDomainRefHolder This(this);
    AddRef();
    // Do the actual unloading
    {
        // We do not want other threads to abort the current one.
        ThreadPreventAsyncHolder preventAsync(TRUE);
        Exit(TRUE, !bForceGC);
    }
    if(bForceGC)
    {
        GCHeap::GetGCHeap()->GarbageCollect();
        GCHeap::GetGCHeap()->FinalizerThreadWait();
        SetStage(STAGE_COLLECTED);
        Close(); //NOTHROW!
    }

#ifdef AD_LOG_MEMORY
    if (pLogIt)
    {
        pThread->EnablePreemptiveGC();
        pLogIt(unloadCount);
        pThread->DisablePreemptiveGC();
    }
#endif // AD_LOG_MEMORY

#ifdef AD_SNAPSHOT
    if (takeSnapShot)
    {
        char buffer[1024];
        sprintf(buffer, "vadump -p %d -o > vadump.%d", GetCurrentProcessId(), unloadCount);
        system(buffer);
        sprintf(buffer, "umdh -p:%d -d -i:1 -f:umdh.%d", GetCurrentProcessId(), unloadCount);
        system(buffer);
        int takeDHSnapShot = g_pConfig->GetConfigDWORD(L"ADTakeDHSnapShot", 0);
        if (takeDHSnapShot)
        {
            sprintf(buffer, "dh -p %d -s -g -h -b -f dh.%d", GetCurrentProcessId(), unloadCount);
            system(buffer);
        }
    }
#endif // AD_SNAPSHOT

#ifdef _DEBUG
    if (dbgAllocReport)
    {
        DbgAllocReport(NULL, FALSE, FALSE);
        ShutdownLogging();
        WCHAR buffer[128];
        swprintf_s(buffer, COUNTOF(buffer), L"DbgAlloc.%d", unloadCount);
        _ASSERTE(WszMoveFileEx(L"COMPLUS.LOG", buffer, MOVEFILE_REPLACE_EXISTING));
        // this will open a new file
        InitLogging();
    }

    if (dumpSB > 0)
    {
        // do extra finalizer wait to remove any leftover sb entries
        GCHeap::GetGCHeap()->FinalizerThreadWait();
        GCHeap::GetGCHeap()->GarbageCollect();
        GCHeap::GetGCHeap()->FinalizerThreadWait();
        LogSpewAlways("Done unload %3.3d\n", unloadCount);
        DumpSyncBlockCache();
        ShutdownLogging();
        WCHAR buffer[128];
        swprintf_s(buffer, NumItems(buffer), L"DumpSB.%d", unloadCount);
        _ASSERTE(WszMoveFileEx(L"COMPLUS.LOG", buffer, MOVEFILE_REPLACE_EXISTING));
        // this will open a new file
        InitLogging();
    }
#endif // _DEBUG
}

void AppDomain::ExceptionUnwind(Frame *pFrame)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);  // EEResourceException
        DISABLED(THROWS);   // EEResourceException
        MODE_ANY;
    }
    CONTRACTL_END;

    LOG((LF_APPDOMAIN, LL_INFO10, "AppDomain::ExceptionUnwind for %8.8x\n", pFrame));
#if _DEBUG_ADUNLOAD
    printf("%x AppDomain::ExceptionUnwind for %8.8p\n", GetThread()->GetThreadId(), pFrame);
#endif
    Thread *pThread = GetThread();
    _ASSERTE(pThread);

    if (! pThread->ShouldChangeAbortToUnload(pFrame))
    {
        LOG((LF_APPDOMAIN, LL_INFO10, "AppDomain::ExceptionUnwind: not first transition or abort\n"));
        return;
    }

    LOG((LF_APPDOMAIN, LL_INFO10, "AppDomain::ExceptionUnwind: changing to unload\n"));

    GCX_COOP();
    OBJECTREF throwable = NULL;
    EEResourceException e(kAppDomainUnloadedException, L"Remoting_AppDomainUnloaded_ThreadUnwound");
    throwable = e.GetThrowable();

    // reset the exception to an AppDomainUnloadedException
    if (throwable != NULL)
    {
        GetThread()->SafeSetThrowables(throwable);
    }
}

BOOL AppDomain::StopEEAndUnwindThreads(unsigned int retryCount, BOOL *pFMarkUnloadRequestThread)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    Thread *pThread = NULL;
    DWORD nThreadsNeedMoreWork=0;
    if (retryCount != (unsigned int)-1 && retryCount < g_pConfig->AppDomainUnloadRetryCount())
    {
        Thread *pCurThread = GetThread();
        if (pCurThread->CatchAtSafePoint())
            pCurThread->PulseGCMode();

        // We know which thread is not in the domain now.  We just need to
        // work on those threads.  We do not need to suspend the runtime.
        ThreadStoreLockHolder tsl(TRUE);
        while ((pThread = ThreadStore::GetThreadList(pThread)) != NULL)
        {
            if (pThread == pCurThread)
            {
                continue;
            }

            if (pThread == GCHeap::GetGCHeap()->GetFinalizerThread())
            {
                continue;
            }

            if (pThread->GetUnloadBoundaryFrame() == NULL)
            {
                continue;
            }

            // A thread may have UnloadBoundaryFrame set if
            // 1. Being unloaded by AD unload helper thread
            // 2. Escalation from OOM or SO triggers AD unload
            // Here we only need to work on threads that are in the domain.  If we work on other threads,
            // those threads may be stucked in a finally, and we will not be able to escalate for them,
            // therefore AD unload is blocked.
            if (pThread->IsBeingAbortedForADUnload() ||
                pThread == SystemDomain::System()->GetUnloadRequestingThread())
            {
                nThreadsNeedMoreWork++;
            }

            if (!(IsRudeUnload() ||
                  (pThread != SystemDomain::System()->GetUnloadRequestingThread() || OnlyOneThreadLeft())))
            {
                continue;
            }

            if ((pThread == SystemDomain::System()->GetUnloadRequestingThread()) && *pFMarkUnloadRequestThread)
            {
                // Mark thread for abortion only once; later on interrupt only
                *pFMarkUnloadRequestThread = FALSE;
                pThread->SetAbortRequest(m_fRudeUnload? EEPolicy::TA_Rude : EEPolicy::TA_V1Compatible);
            }
            else
            {
                if (pThread->m_State & Thread::TS_Interruptible)
                {
                    pThread->UserInterrupt(Thread::TI_Abort);
                }
            }

            if (pThread->PreemptiveGCDisabledOther())
            {
            }
        }

        tsl.Release();

        if (nThreadsNeedMoreWork && CLRTaskHosted())
        {
            // In case a thread is the domain is blocked due to its scheduler being
            // occupied by another thread.
            Thread::ThreadAbortWatchDog();
        }
        m_dwThreadsStillInAppDomain=nThreadsNeedMoreWork;
        return !nThreadsNeedMoreWork;
    }

    // For now piggyback on the GC's suspend EE mechanism
    GCHeap::GetGCHeap()->SuspendEE(GCHeap::GetGCHeap()->SUSPEND_FOR_APPDOMAIN_SHUTDOWN);
#ifdef _DEBUG
    _ASSERTE(ThreadStore::s_pThreadStore->DbgBackgroundThreadCount() > 0);
#endif // _DEBUG

    int totalADCount = 0;
    int finalizerADCount = 0;
    pThread = NULL;

    RuntimeExceptionKind reKind = kLastException;
    UINT resId = 0;
    SmallStackSString ssThreadId;

    while ((pThread = ThreadStore::GetThreadList(pThread)) != NULL)
    {
        // we already checked that we're not running in the unload domain
        if (pThread == GetThread())
        {
            continue;
        }

#ifdef _DEBUG
        void PrintStackTraceWithADToLog(Thread *pThread);
        if (LoggingOn(LF_APPDOMAIN, LL_INFO100)) {
            LOG((LF_APPDOMAIN, LL_INFO100, "\nStackTrace for %x\n", pThread->GetThreadId()));
            PrintStackTraceWithADToLog(pThread);
        }
#endif // _DEBUG
        int count = 0;
        Frame *pFrame = pThread->GetFirstTransitionInto(this, &count);
        if (! pFrame) {
            _ASSERTE(count == 0);
            if (pThread->IsBeingAbortedForADUnload())
            {
                pThread->ResetBeginAbortedForADUnload();
            }
            continue;
        }

        if (pThread != GCHeap::GetGCHeap()->GetFinalizerThread())
        {
            totalADCount += count;
            nThreadsNeedMoreWork++;
            pThread->SetUnloadBoundaryFrame(pFrame);
        }
        else
        {
            finalizerADCount = count;
        }

        // don't setup the exception info for the unloading thread unless it's the last one in
        if (retryCount != ((unsigned int) -1) && retryCount > g_pConfig->AppDomainUnloadRetryCount() && reKind == kLastException &&
            (pThread != SystemDomain::System()->GetUnloadRequestingThread() || OnlyOneThreadLeft()))
        {
#ifdef AD_BREAK_ON_CANNOT_UNLOAD
            static int breakOnCannotUnload = g_pConfig->GetConfigDWORD(L"ADBreakOnCannotUnload", 0);
            if (breakOnCannotUnload)
                _ASSERTE(!"Cannot unload AD");
#endif // AD_BREAK_ON_CANNOT_UNLOAD
            reKind = kCannotUnloadAppDomainException;
            resId = IDS_EE_ADUNLOAD_CANT_UNWIND_THREAD;
            ssThreadId.Printf(L"%x", pThread->GetThreadId());
            STRESS_LOG2(LF_APPDOMAIN, LL_INFO10, "AppDomain::UnwindThreads cannot stop thread %x with %d transitions\n", pThread->GetThreadId(), count);
            // don't break out of this early or the assert totalADCount == (int)m_dwThreadEnterCount below will fire
            // it's better to chew a little extra time here and make sure our counts are consistent
        }
        // only abort the thread requesting the unload if it's the last one in, that way it will get
        // notification that the unload failed for some other thread not being aborted. And don't abort
        // the finalizer thread - let it finish it's work as it's allowed to be in there. If it won't finish,
        // then we will eventually get a CannotUnloadException on it.

        if (pThread != GCHeap::GetGCHeap()->GetFinalizerThread() &&
            // If the domain is rudely unloaded, we will unwind the requesting thread out
            // Rude unload is going to succeed, or escalated to disable runtime or higher.
            (IsRudeUnload() ||
              (pThread != SystemDomain::System()->GetUnloadRequestingThread() || OnlyOneThreadLeft())
              )
             )
        {

            STRESS_LOG2(LF_APPDOMAIN, LL_INFO100, "AppDomain::UnwindThreads stopping %x with %d transitions\n", pThread->GetThreadId(), count);
            LOG((LF_APPDOMAIN, LL_INFO100, "AppDomain::UnwindThreads stopping %x with %d transitions\n", pThread->GetThreadId(), count));
#if _DEBUG_ADUNLOAD
            printf("AppDomain::UnwindThreads %x stopping %x with first frame %8.8p\n", GetThread()->GetThreadId(), pThread->GetThreadId(), pFrame);
#endif
                if (pThread == SystemDomain::System()->GetUnloadRequestingThread())
                {
                    // Mark thread for abortion only once; later on interrupt only
                    *pFMarkUnloadRequestThread = FALSE;
                }
                pThread->SetAbortRequest(m_fRudeUnload? EEPolicy::TA_Rude : EEPolicy::TA_V1Compatible);
        }
        TESTHOOKCALL(UnwindingThreads(GetId().m_dwId)) ;
    }
    _ASSERTE(totalADCount + finalizerADCount == (int)m_dwThreadEnterCount);

    if (totalADCount + finalizerADCount != (int)m_dwThreadEnterCount)
        FreeBuildDebugBreak();

    // if our count did get messed up, set it to whatever count we actually found in the domain to avoid looping
    // or other problems related to incorrect count. This is very much a bug if this happens - a thread should always
    // exit the domain gracefully.
    // m_dwThreadEnterCount = totalADCount;

    if (reKind != kLastException)
    {
        pThread = NULL;
        while ((pThread = ThreadStore::GetThreadList(pThread)) != NULL)
        {
            if (pThread->IsBeingAbortedForADUnload())
            {
                pThread->ResetBeginAbortedForADUnload();
            }
        }
    }

    // CommonTripThread will handle the abort for any threads that we've marked
    GCHeap::GetGCHeap()->RestartEE(FALSE, TRUE);
    if (reKind != kLastException)
        COMPlusThrow(reKind, resId, ssThreadId.GetUnicode());

    _ASSERTE((totalADCount==0 && nThreadsNeedMoreWork==0) ||(totalADCount!=0 && nThreadsNeedMoreWork!=0));
    
    m_dwThreadsStillInAppDomain=nThreadsNeedMoreWork;
    return (totalADCount == 0);
}

void AppDomain::UnwindThreads()
{
    // This function should guarantee appdomain
    // consistency even if it fails. Everything that is going
    // to make the appdomain impossible to reenter
    // should be factored out

    CONTRACTL
    {
        MODE_COOPERATIVE;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    int retryCount = -1;
    m_dwThreadsStillInAppDomain=(ULONG)-1;
    ULONGLONG startTime = CLRGetTickCount64();

    if (GetEEPolicy()->GetDefaultAction(OPR_AppDomainUnload, NULL) == eRudeUnloadAppDomain &&
        !IsRudeUnload())
    {
        GetEEPolicy()->NotifyHostOnDefaultAction(OPR_AppDomainUnload, eRudeUnloadAppDomain);
        SetRudeUnload();
    }

    // Force threads to go through slow path during AD unload.
    CounterHolder trapReturningThreads(&g_TrapReturningThreads);

    BOOL fCurrentUnloadMode = IsRudeUnload();
    BOOL fMarkUnloadRequestThread = TRUE;

    // now wait for all the threads running in our AD to get out
    do
    {
        DWORD timeout = GetEEPolicy()->GetTimeout(m_fRudeUnload?OPR_AppDomainRudeUnload : OPR_AppDomainUnload);
        EPolicyAction action = GetEEPolicy()->GetActionOnTimeout(m_fRudeUnload?OPR_AppDomainRudeUnload : OPR_AppDomainUnload, NULL);
        if (timeout != INFINITE && action > eUnloadAppDomain) {
            // Escalation policy specified.
            ULONGLONG curTime = CLRGetTickCount64();
            ULONGLONG elapseTime = curTime - startTime;
            if (elapseTime > timeout)
            {
                // Escalate
                switch (action)
                {
                case eRudeUnloadAppDomain:
                    GetEEPolicy()->NotifyHostOnTimeout(m_fRudeUnload?OPR_AppDomainRudeUnload : OPR_AppDomainUnload, action);
                    SetRudeUnload();
                    STRESS_LOG1(LF_APPDOMAIN, LL_INFO100,"Escalating to RADU, adid=%d",GetId().m_dwId);
                    break;
                case eExitProcess:
                case eFastExitProcess:
                case eRudeExitProcess:
                case eDisableRuntime:
                    GetEEPolicy()->NotifyHostOnTimeout(m_fRudeUnload?OPR_AppDomainRudeUnload : OPR_AppDomainUnload, action);
                    EEPolicy::HandleExitProcessFromEscalation(action, HOST_E_EXITPROCESS_TIMEOUT);
                    _ASSERTE (!"Should not reach here");
                    break;
                default:
                    break;
                }
            }
        }
#ifdef _DEBUG
        if (LoggingOn(LF_APPDOMAIN, LL_INFO100))
            DumpADThreadTrack();
#endif // _DEBUG
        BOOL fNextUnloadMode = IsRudeUnload();
        if (fCurrentUnloadMode != fNextUnloadMode)
        {
            // We have changed from normal unload to rude unload.  We need to mark the thread
            // with RudeAbort, but we can only do this safely if the runtime is suspended.
            fCurrentUnloadMode = fNextUnloadMode;
            retryCount = -1;
        }
        if (StopEEAndUnwindThreads(retryCount, &fMarkUnloadRequestThread))
            break;
        if (timeout != INFINITE || m_fRudeUnload)
        {
            // Turn off the timeout used by AD.
            retryCount = 1;
        }
        else
        {
#ifdef STRESS_HEAP
            // GCStress takes a long time to unwind, due to expensive creation of
            // a threadabort exception.
            if(g_pConfig->GetGCStressLevel() == 0)
#endif // STRESS_HEAP
                ++retryCount;
            LOG((LF_APPDOMAIN, LL_INFO10, "AppDomain::UnwindThreads iteration %d waiting on thread count %d\n", retryCount, m_dwThreadEnterCount));
#if _DEBUG_ADUNLOAD
            printf("AppDomain::UnwindThreads iteration %d waiting on thread count %d\n", retryCount, m_dwThreadEnterCount);
#endif
        }

        if (m_dwThreadEnterCount != 0)
        {
#ifdef _DEBUG
            GetThread()->UserSleep(20);
#else // !_DEBUG
            GetThread()->UserSleep(10);
#endif // !_DEBUG
        }
    }
    while (TRUE) ;
}

void AppDomain::ClearGCHandles()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        NOTHROW;
    }
    CONTRACTL_END;

    SetStage(STAGE_HANDLETABLE_NOACCESS);

    GCHeap::GetGCHeap ()->WaitUntilConcurrentGCComplete ();

    // Keep async pin handles alive by moving them to default domain
    HandleAsyncPinHandles();

    // Remove our handle table as a source of GC roots
    HandleTableBucket *pBucket = m_hHandleTableBucket;

#ifdef _DEBUG
    if (((HandleTable *)(pBucket->pTable[0]))->uADIndex != m_dwIndex)
        _ASSERTE (!"AD index mismatch");
#endif // _DEBUG

    Ref_RemoveHandleTableBucket(pBucket);
}

// When an AD is unloaded, we will release all objects in this AD.
// If a future asynchronous operation, like io completion port function,
// we need to keep the memory space fixed so that the gc heap is not corrupted.
void AppDomain::HandleAsyncPinHandles()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        NOTHROW;
    }
    CONTRACTL_END;

    HandleTableBucket *pBucket = m_hHandleTableBucket;
    // IO completion port picks IO job using FIFO.  Here is how we know which AsyncPinHandle can be freed.
    // 1. We mark all non-pending AsyncPinHandle with READYTOCLEAN.
    // 2. We queue a dump Overlapped to the IO completion as a marker.
    // 3. When the Overlapped is picked up by completion port, we wait until all previous IO jobs are processed.
    // 4. Then we can delete all AsyncPinHandle marked with READYTOCLEAN.
    HandleTableBucket *pBucketInDefault = SystemDomain::System()->DefaultDomain()->m_hHandleTableBucket;
    Ref_RelocateAsyncPinHandles(pBucket, pBucketInDefault);

    OverlappedDataObject::RequestCleanup();
}

void AppDomain::ClearGCRoots()
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        NOTHROW;
    }
    CONTRACTL_END;

    enum
    {
        FirstPass,
        Retry,
        LastPass
    }  
    state=FirstPass;

    // Release the DLS for this domain for each thread. Also, remove the TLS for this
    // domain for each thread.


    do
    {
    DLSLockHolder dlsHolder;
    Thread *pThread = NULL;
    GCHeap::GetGCHeap()->SuspendEE(GCHeap::SUSPEND_FOR_APPDOMAIN_SHUTDOWN);
        if(state==FirstPass)
        {
    // Tell the JIT managers to delete any entries in their structures. All the cooperative mode threads are stopped at
    // this point, so only need to synchronize the preemptive mode threads.
    ExecutionManager::Unload(this);
        }
        LocalDataStore* pStores[128] ; //the number does not really matter
        UINT                  nStores=NumItems(pStores);

        // Need to take this lock prior to suspending the EE because ReleaseDomainStores needs it. All
        // access to thread_m_pDLSHash is done with the LockDLSHash, so don't have to worry about cooperative
        // mode threads

        state=ReleaseDomainStores(pStores, &nStores)?LastPass:Retry;

        // Clear out the exceptions objects held by a thread.
        while ((pThread = ThreadStore::GetAllThreadList(pThread, 0, 0)) != NULL)
        {
            if (m_hHandleTableBucket->Contains(pThread->m_LastThrownObjectHandle))
            {
                // Never delete a handle to a preallocated exception object.
                if (!CLRException::IsPreallocatedExceptionHandle(pThread->m_LastThrownObjectHandle))
                {
                    DestroyHandle(pThread->m_LastThrownObjectHandle);
                }

                pThread->m_LastThrownObjectHandle = NULL;
            }

            pThread->GetExceptionState()->ClearThrowablesForUnload(m_hHandleTableBucket);

            // go through the thread local statics and clear out any whose methoddesc
        }

        if(state==LastPass)
        {
            //delete them while we still have the runtime suspended
            // This must be deleted before the loader heaps are deleted.
            if (m_pMarshalingData != NULL)
            {
                delete m_pMarshalingData;
                m_pMarshalingData = NULL;
            }

            if (m_pLargeHeapHandleTable != NULL)
            {
                delete m_pLargeHeapHandleTable;
                m_pLargeHeapHandleTable = NULL;
            }
           
        }

        GCHeap::GetGCHeap()->RestartEE(FALSE, TRUE);
        dlsHolder.Release();

        // Now remove these LocalDataStores from the managed LDS manager. This must be done outside the
        // suspend of the EE because RemoveDLSFromList calls managed code.
        if (pStores != NULL)
        {
            int i = nStores;
            while (i-- > 0) {
                if (pStores[i]) {
                    Thread::RemoveDLSFromListNoThrow(pStores[i]);
                    delete pStores[i];
                }
            }
        }        
    }
    while(state!=LastPass);


    
}


// (1) Remove the DLS for this domain from each thread
// (2) Also, remove the TLS (Thread local static) stores for this
// domain from each thread
BOOL AppDomain::ReleaseDomainStores(LocalDataStore **pStores, UINT *pNumStores)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    // Don't bother cleaning this up if we're detaching
    if (g_fProcessDetach)
    {
        *pNumStores=0;
        return TRUE;
    }

    Thread *pThread = NULL;
    ADID id = GetId();
    UINT i = 0;
    BOOL bNeedRetry=FALSE;

    while ((pThread = ThreadStore::GetAllThreadList(pThread, 0, 0)) != NULL)
    {
        if (i>=*pNumStores)
        {
            bNeedRetry=TRUE;
            break;
        }
        // Get a pointer to the Domain Local Store
        if((pStores[i] = pThread->RemoveDomainLocalStore(id))!=NULL)
            i++;

        // Delete the thread local static store
        pThread->DeleteThreadStaticData(this);

        pThread->ResetCultureForDomain(id);
    }

    *pNumStores = i;
    return !bNeedRetry;
}

#ifdef _DEBUG

void AppDomain::TrackADThreadEnter(Thread *pThread, Frame *pFrame)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        // REENTRANT
        PRECONDITION(CheckPointer(pThread));
        PRECONDITION(pFrame != (Frame*)(size_t) INVALID_POINTER_CD);
    }
    CONTRACTL_END;

    while (FastInterlockCompareExchange((LONG*)&m_TrackSpinLock, 1, 0) != 0)
        ;
    if (m_pThreadTrackInfoList == NULL)
        m_pThreadTrackInfoList = new (nothrow) ThreadTrackInfoList;
    // If we don't assert here, we will AV in the for loop below
    _ASSERTE(m_pThreadTrackInfoList);

    ThreadTrackInfoList *pTrackList= m_pThreadTrackInfoList;

    ThreadTrackInfo *pTrack = NULL;
    int i;
    for (i=0; i < pTrackList->Count(); i++) {
        if ((*(pTrackList->Get(i)))->pThread == pThread) {
        pTrack = *(pTrackList->Get(i));
        break;
        }
    }
    if (! pTrack) {
        pTrack = new (nothrow) ThreadTrackInfo;
        // If we don't assert here, we will AV in the for loop below.
        _ASSERTE(pTrack);
        pTrack->pThread = pThread;
        ThreadTrackInfo **pSlot = pTrackList->Append();
        *pSlot = pTrack;
    }

    InterlockedIncrement((LONG*)&m_dwThreadEnterCount);
    Frame **pSlot;
    if (pTrack)
    {
        pSlot = pTrack->frameStack.Insert(0);
        *pSlot = pFrame;
    }
    int totThreads = 0;
    for (i=0; i < pTrackList->Count(); i++)
        totThreads += (*(pTrackList->Get(i)))->frameStack.Count();
    _ASSERTE(totThreads == (int)m_dwThreadEnterCount);

    InterlockedExchange((LONG*)&m_TrackSpinLock, 0);
}


void AppDomain::TrackADThreadExit(Thread *pThread, Frame *pFrame)
{
    CONTRACTL
    {
        if (GetThread()) {MODE_COOPERATIVE;}
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    while (FastInterlockCompareExchange((LONG*)&m_TrackSpinLock, 1, 0) != 0)
        ;
    ThreadTrackInfoList *pTrackList= m_pThreadTrackInfoList;
    _ASSERTE(pTrackList);
    ThreadTrackInfo *pTrack = NULL;
    int i;
    for (i=0; i < pTrackList->Count(); i++)
    {
        if ((*(pTrackList->Get(i)))->pThread == pThread)
        {
            pTrack = *(pTrackList->Get(i));
            break;
        }
    }
    _ASSERTE(pTrack);
    _ASSERTE(*(pTrack->frameStack.Get(0)) == pFrame);
    pTrack->frameStack.Delete(0);
    InterlockedDecrement((LONG*)&m_dwThreadEnterCount);

    int totThreads = 0;
    for (i=0; i < pTrackList->Count(); i++)
        totThreads += (*(pTrackList->Get(i)))->frameStack.Count();
    _ASSERTE(totThreads == (int)m_dwThreadEnterCount);

    InterlockedExchange((LONG*)&m_TrackSpinLock, 0);
}

void AppDomain::DumpADThreadTrack()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    while (FastInterlockCompareExchange((LONG*)&m_TrackSpinLock, 1, 0) != 0)
        ;
    ThreadTrackInfoList *pTrackList= m_pThreadTrackInfoList;
    if (!pTrackList)
        goto end;

    {
    LOG((LF_APPDOMAIN, LL_INFO10000, "\nThread dump of %d threads for [%d] %#08x %S\n",
        m_dwThreadEnterCount, GetId().m_dwId, this, GetFriendlyNameForLogging()));
    int totThreads = 0;
    for (int i=0; i < pTrackList->Count(); i++)
    {
        ThreadTrackInfo *pTrack = *(pTrackList->Get(i));
        if(pTrack->frameStack.Count()==0)
            continue;
        LOG((LF_APPDOMAIN, LL_INFO100, "  ADEnterCount for %x is %d\n", pTrack->pThread->GetThreadId(), pTrack->frameStack.Count()));
        totThreads += pTrack->frameStack.Count();
        for (int j=0; j < pTrack->frameStack.Count(); j++)
            LOG((LF_APPDOMAIN, LL_INFO100, "      frame %8.8x\n", *(pTrack->frameStack.Get(j))));
    }
    _ASSERTE(totThreads == (int)m_dwThreadEnterCount);
    }
end:
    InterlockedExchange((LONG*)&m_TrackSpinLock, 0);
}
#endif // _DEBUG

OBJECTREF AppDomain::GetAppDomainProxy()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    OBJECTREF orProxy = CRemotingServices::CreateProxyForDomain(this);

    _ASSERTE(orProxy->GetMethodTable()->IsThunking());

    return orProxy;
}


void *SharedDomain::operator new(size_t size, void *pInPlace)
{
    LEAF_CONTRACT;
    return pInPlace;
}

void SharedDomain::operator delete(void *pMem)
{
    LEAF_CONTRACT;
    // Do nothing - new() was in-place
}


void SharedDomain::Attach()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Create the global SharedDomain and initialize it.
    m_pSharedDomain = new (&g_pSharedDomainMemory) SharedDomain();
    // This cannot fail since g_pSharedDomainMemory is a static array.
    CONSISTENCY_CHECK(CheckPointer(m_pSharedDomain));

    LOG((LF_CLASSLOADER,
         LL_INFO10,
         "Created shared domain at %x\n",
         m_pSharedDomain));

    // We need to initialize the memory pools etc. for the system domain.
    m_pSharedDomain->Init(); // Setup the memory heaps

    // allocate a Virtual Call Stub Manager for the shared domain
    m_pSharedDomain->InitVirtualCallStubManager();
}

void SharedDomain::Detach()
{
    if (m_pSharedDomain)
    {
        m_pSharedDomain->Terminate();
        delete m_pSharedDomain;
        m_pSharedDomain = NULL;
    }
}

#endif // !DACCESS_COMPILE

SharedDomain *SharedDomain::GetDomain()
{
    LEAF_CONTRACT;

    return m_pSharedDomain;
}

#ifndef DACCESS_COMPILE

#define INITIAL_ASSEMBLY_MAP_SIZE 17
void SharedDomain::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    BaseDomain::Init();

    m_FileCreateLock.Init("ShareDomainLock", CrstSharedAssemblyCreate, CRST_DEFAULT,TRUE);

    LockOwner lock = {&m_DomainCrst, IsOwnerOfCrst};
    m_assemblyMap.Init(INITIAL_ASSEMBLY_MAP_SIZE, CompareSharedAssembly, TRUE, &lock);

}

void SharedDomain::Terminate()
{
    // make sure we delete the StringLiteralMap before unloading
    // the asemblies since the string literal map entries can
    // point to metadata string literals.
    if (m_pStringLiteralMap != NULL)
    {
        delete m_pStringLiteralMap;
        m_pStringLiteralMap = NULL;
    }

    PtrHashMap::PtrIterator i = m_assemblyMap.begin();

    while (!i.end())
    {
        Assembly *pAssembly = (Assembly*) i.GetValue();
        delete pAssembly;
        ++i;
    }

    ListLockEntry* pElement;
    pElement = m_FileCreateLock.Pop(TRUE);
    while (pElement)
    {
#ifdef STRICT_CLSINITLOCK_ENTRY_LEAK_DETECTION
        _ASSERTE (dbg_fDrasticShutdown || g_fInControlC);
#endif
        delete(pElement);
        pElement = (FileLoadLock*) m_FileCreateLock.Pop(TRUE);
    }
    m_FileCreateLock.Destroy();

    BaseDomain::Terminate();
}


HRESULT CompareAssemblies(IUnknown *pUnk1, IUnknown *pUnk2, BOOL *pbIsEqual);


BOOL SharedDomain::CompareSharedAssembly(UPTR u1, UPTR u2)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_INTOLERANT;
    // This is the input to the lookup
    SharedAssemblyLocator *pLocator = (SharedAssemblyLocator *) (u1<<1);

    // This is the value stored in the table
    Assembly *pAssembly = (Assembly *) u2;
    BOOL bEqual;
    if (pLocator->GetType()==SharedAssemblyLocator::DOMAINASSEMBLY)
        return pAssembly->CanBeShared(pLocator->GetDomainAssembly());
    else
    if (pLocator->GetType()==SharedAssemblyLocator::PEASSEMBLY)
        return pAssembly->GetManifestFile()->Equals(pLocator->GetPEAssembly());
    else
    if (pLocator->GetType()==SharedAssemblyLocator::IASSEMBLY)
    {
        IfFailThrow(CompareAssemblies(pLocator->GetIAssembly(),pAssembly->GetManifestFile()->GetFusionAssembly(),&bEqual));
        return bEqual;
    }
    else
    if (pLocator->GetType()==SharedAssemblyLocator::IHOSTASSEMBLY)
    {
        IfFailThrow(CompareAssemblies(pLocator->GetIHostAssembly(),pAssembly->GetManifestFile()->GetIHostAssembly(),&bEqual));
        return bEqual;
    }
    else
        return FALSE;

}


//Assembly *SharedDomain::FindShareableAssembly(PEAssembly *pFile)
//{
//    LEAF_CONTRACT;
//    Assembly *match = (Assembly *) m_assemblyMap.LookupValue(pFile->HashIdentity(), NULL);
//    if (match != (Assembly *) INVALIDENTRY)
//        return match;
//    else
//        return NULL;
//}

DWORD SharedAssemblyLocator::Hash()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_type==DOMAINASSEMBLY)
        return GetDomainAssembly()->GetFile()->HashIdentity();
    if (m_type==IASSEMBLY)
    {
        StackSString path;
        FusionBind::GetAssemblyManifestModulePath(GetIAssembly(),path);
        return path.Hash();
    }
    if (m_type==PEASSEMBLY)
    {
        return GetPEAssembly()->HashIdentity();
    }
    if (m_type==IHOSTASSEMBLY)
    {
        UINT64 assemblyId;
        GetIHostAssembly()->GetAssemblyId(&assemblyId);
        return PEImage::HashStreamIds(assemblyId, 0);
    }
    _ASSERTE(!"NYI");
    return 0;
}

Assembly *SharedDomain::FindShareableAssembly(SharedAssemblyLocator *pLocator)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    Assembly *match= (Assembly *) m_assemblyMap.LookupValue(pLocator->Hash(), pLocator);
    if (match != (Assembly *) INVALIDENTRY)
        return match;
    else
        return NULL;
}


void SharedDomain::AddShareableAssembly(Assembly *pAssembly)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // We have a lock on the file. There should be no races to add the same assembly.

    {
        LockHolder holder(this);

        EX_TRY
        {
            pAssembly->SetIsTenured();
            m_assemblyMap.InsertValue(pAssembly->GetManifestFile()->HashIdentity(), pAssembly);
        }
        EX_HOOK
        {
            // There was an error adding the assembly to the assembly hash (probably an OOM),
            // so we need to unset the tenured bit so that correct cleanup can happen.
            pAssembly->UnsetIsTenured();
        }
        EX_END_HOOK
    }

    pAssembly->IncrementShareCount();

    LOG((LF_CODESHARING,
         LL_INFO100,
         "Successfully added shareable assembly \"%s\".\n",
         pAssembly->GetManifestFile()->GetSimpleName()));
}

#endif // !DACCESS_COMPILE

BOOL DomainLocalModule::GetClassFlags(MethodTable* pMT, DWORD iClassIndex /*=(DWORD)-1*/)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
        PRECONDITION(GetDomainFile()->GetModule() == pMT->GetModuleForStatics());
    } CONTRACTL_END;

    if (pMT->IsDynamicStatics())
    {
        _ASSERTE(!pMT->ContainsGenericVariables());
        DWORD dynamicClassID = pMT->GetModuleDynamicEntryID();
        if(m_aDynamicEntries <= dynamicClassID)
            return FALSE;
        return (m_pDynamicClassTable[dynamicClassID].m_dwFlags);
    }
    else
    {
        if (iClassIndex == (DWORD)-1)
            iClassIndex = pMT->GetClassIndex();
        return (PTR_BYTE(GetPrecomputedStaticsClassData()))[iClassIndex];
    }
}

#ifndef DACCESS_COMPILE

void DomainLocalModule::SetClassFlags(MethodTable* pMT, DWORD dwFlags)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(GetDomainFile()->GetModule() == pMT->GetModuleForStatics());
    } CONTRACTL_END;

    BaseDomain::DomainLocalBlockLockHolder lh(GetDomainFile()->GetAppDomain());
    if (pMT->IsDynamicStatics())
    {
        _ASSERTE(!pMT->ContainsGenericVariables());
        DWORD dwID = pMT->GetModuleDynamicEntryID();
        EnsureDynamicClassIndex(dwID);
        m_pDynamicClassTable[dwID].m_dwFlags |= dwFlags;
    }
    else
    {
        GetPrecomputedStaticsClassData()[pMT->GetClassIndex()] |= dwFlags;
    }
}

void    DomainLocalModule::EnsureDynamicClassIndex(DWORD dwID)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (dwID < m_aDynamicEntries)
    {
        _ASSERTE(m_pDynamicClassTable != NULL);
        return;
    }

    BaseDomain::DomainLocalBlockLockHolder lh(GetDomainFile()->GetAppDomain());

    // Did we lose the race?
    if (dwID < m_aDynamicEntries)
    {
        _ASSERTE(m_pDynamicClassTable != NULL);
        return;
    }

    SIZE_T aDynamicEntries = max(16, m_aDynamicEntries);
    while (aDynamicEntries <= dwID)
    {
        aDynamicEntries *= 2;
    }

    DynamicClassInfo* pNewDynamicClassTable;
    pNewDynamicClassTable = (DynamicClassInfo*)
        (void*)GetDomainFile()->GetAppDomain()->GetHighFrequencyHeap()->AllocMem(
            sizeof(DynamicClassInfo) * aDynamicEntries);

    memcpy(pNewDynamicClassTable, m_pDynamicClassTable, sizeof(DynamicClassInfo) * m_aDynamicEntries);

    // Note: Memory allocated on loader heap is zero filled
    // memset(pNewDynamicClassTable + m_aDynamicEntries, 0, (aDynamicEntries - m_aDynamicEntries) * sizeof(DynamicClassInfo));

    _ASSERTE(m_aDynamicEntries%2 == 0);

    // Commit new dynamic table. The lock-free helpers depend on the order.
    MemoryBarrier();
    m_pDynamicClassTable = pNewDynamicClassTable;
    MemoryBarrier();
    m_aDynamicEntries = aDynamicEntries;
}

void    DomainLocalModule::AllocateDynamicClass(MethodTable *pMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(!pMT->ContainsGenericVariables());
    _ASSERTE(GetDomainFile()->GetModule() == pMT->GetModuleForStatics());
    _ASSERTE(pMT->IsDynamicStatics());

    DWORD dwID = pMT->GetModuleDynamicEntryID();

    EnsureDynamicClassIndex(dwID);

    _ASSERTE(m_aDynamicEntries > dwID);

    EEClass *pClass = pMT->GetClass();
    g_IBCLogger.LogEEClassAndMethodTableAccess( pClass );
    DWORD dwStaticBytes = pClass->GetNonGCStaticFieldBytes();
    DWORD dwNumHandleStatics = pMT->GetNumHandleStatics();

    _ASSERTE(GetDomainFile()->GetAppDomain()->OwnDomainLocalBlockLock());

    _ASSERTE(!IsClassAllocated(pMT));
    _ASSERTE(!IsClassInitialized(pMT));
    _ASSERTE(!IsClassInitError(pMT));

    DynamicEntry *pDynamicStatics = m_pDynamicClassTable[dwID].m_pDynamicEntry;

    // We need this check because maybe a class had a cctor but no statics
    if (dwStaticBytes > 0 || dwNumHandleStatics > 0)
    {
        if (pDynamicStatics == NULL)
        {
            pDynamicStatics = (DynamicEntry*)
              (void*)GetDomainFile()->GetAppDomain()->GetHighFrequencyHeap()->AllocMem(sizeof(DynamicEntry)+dwStaticBytes);

            // Note: Memory allocated on loader heap is zero filled
            // pDynamicStatics->m_pGCStatics = 0;
            // memset(((BYTE*)pDynamicStatics) + sizeof(DynamicEntry), 0, dwStaticBytes);

            m_pDynamicClassTable[dwID].m_pDynamicEntry = pDynamicStatics;
        }

        if (dwNumHandleStatics > 0)
        {
            GetAppDomain()->AllocateStaticFieldObjRefPtrs(dwNumHandleStatics,
                                                          &pDynamicStatics->m_pGCStatics);
        }
    }
}


void DomainLocalModule::PopulateClass(MethodTable *pMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    _ASSERTE(!pMT->ContainsGenericVariables());

    DWORD iClassIndex = pMT->GetClassIndex();

    if (!IsClassAllocated(pMT, iClassIndex))
    {
        BaseDomain::DomainLocalBlockLockHolder lh(GetDomainFile()->GetAppDomain());

        if (!IsClassAllocated(pMT, iClassIndex))
        {
            // Allocate dynamic space if necessary
            if (pMT->IsDynamicStatics())
                AllocateDynamicClass(pMT);

            // If we have no class constructor and no boxed value types to allocate,
            // we're done.
            if (!pMT->HasClassConstructor() && pMT->GetNumBoxedStatics() == 0)
            {
                // Set both Initialized and Allocated bits at the same time to avoid races
                SetClassAllocatedAndInitialized(pMT);
            }
            else
            {
                SetClassAllocated(pMT);
            }
        }
    }

    return;
}




SIZE_T DomainLocalBlock::m_cModuleIndices = 0;               // Module entries that have been used

void DomainLocalBlock::EnsureModuleIndex(SIZE_T index)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_aModuleIndices > index)
    {
        _ASSERTE(m_pModuleSlots != NULL);
        return;
    }

    BaseDomain::DomainLocalBlockLockHolder lh(m_pDomain);

    // Did we lose a race?
    if (m_aModuleIndices > index)
    {
        _ASSERTE(m_pModuleSlots != NULL);
        return;
    }

    SIZE_T aModuleIndices = max(16, m_aModuleIndices);
    while (aModuleIndices <= index)
    {
        aModuleIndices *= 2;
    }

    PTR_DomainLocalModule* pNewModuleSlots = (PTR_DomainLocalModule*) (void*)m_pDomain->GetHighFrequencyHeap()->AllocMem(sizeof(PTR_DomainLocalModule) * aModuleIndices);

    memcpy(pNewModuleSlots, m_pModuleSlots, sizeof(SIZE_T)*m_aModuleIndices);

    // Note: Memory allocated on loader heap is zero filled
    // memset(pNewModuleSlots + m_aModuleIndices, 0 , (aModuleIndices - m_aModuleIndices)*sizeof(PTR_DomainLocalModule) );

    // Commit new table. The lock-free helpers depend on the order.
    MemoryBarrier();
    m_pModuleSlots = pNewModuleSlots;
    MemoryBarrier();
    m_aModuleIndices = aModuleIndices;

}

SIZE_T DomainLocalBlock::AllocateModuleID()
{
    return IndexToID(InterlockedIncrement((LONG*) &m_cModuleIndices) - 1);
}

void DomainLocalBlock::SetModuleSlot(SIZE_T ModuleID, PTR_DomainLocalModule pLocalModule)
{
    EnsureModuleIndex(ModuleID);

    // Need to synchronize with table growth in this domain
    BaseDomain::DomainLocalBlockLockHolder lh(m_pDomain);

    SIZE_T index = IDToIndex(ModuleID);
    _ASSERTE(index < m_aModuleIndices);


    m_pModuleSlots[index] = pLocalModule;
}

DomainAssembly* AppDomain::RaiseTypeResolveEventThrowing(LPCSTR szName)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS;
        THROWS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    DomainAssembly* pAssembly = NULL;
    _ASSERTE(strcmp(szName, g_AppDomainClassName));

    GCX_COOP();

    struct _gc {
        OBJECTREF AppDomainRef;
        STRINGREF str;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);
    if ((gc.AppDomainRef = GetRawExposedObject()) != NULL)
    {
        MethodDescCallSite onTypeResolve(METHOD__APP_DOMAIN__ON_TYPE_RESOLVE, &gc.AppDomainRef);

        gc.str = COMString::NewString(szName);
        ARG_SLOT args[2] =
        {
            ObjToArgSlot(gc.AppDomainRef),
                ObjToArgSlot(gc.str)
        };
        ASSEMBLYREF ResultingAssemblyRef = (ASSEMBLYREF) onTypeResolve.Call_RetOBJECTREF(args);
        if (ResultingAssemblyRef != NULL)
        {
            pAssembly = ResultingAssemblyRef->GetDomainAssembly();
    }
    }
    GCPROTECT_END();


    return pAssembly;
}


Assembly* AppDomain::RaiseResourceResolveEvent(LPCSTR szName)
{
    CONTRACT(Assembly*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    Assembly* pAssembly = NULL;

    GCX_COOP();

    struct _gc {
        OBJECTREF AppDomainRef;
        STRINGREF str;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);
    if ((gc.AppDomainRef = GetRawExposedObject()) != NULL)
    {
        MethodDescCallSite onResourceResolve(METHOD__APP_DOMAIN__ON_RESOURCE_RESOLVE, &gc.AppDomainRef);
        gc.str = COMString::NewString(szName);
        ARG_SLOT args[2] =
        {
            ObjToArgSlot(gc.AppDomainRef),
                ObjToArgSlot(gc.str)
        };
        ASSEMBLYREF ResultingAssemblyRef = (ASSEMBLYREF) onResourceResolve.Call_RetOBJECTREF(args);
        if (ResultingAssemblyRef != NULL)
        {
            pAssembly = ResultingAssemblyRef->GetAssembly();
    }
    }
    GCPROTECT_END();

    RETURN pAssembly;
}

Assembly* AppDomain::RaiseAssemblyResolveEvent(AssemblySpec *pSpec, BOOL fIntrospection, BOOL fPreBind)
{
    CONTRACT(Assembly*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    BinderMethodID methodId;
    StackSString ssName;
    pSpec->GetFileOrDisplayName(0, ssName);

    if ( (!fIntrospection) && (!fPreBind) )
    {
        methodId = METHOD__APP_DOMAIN__ON_ASSEMBLY_RESOLVE;  // post-bind execution event (the classic V1.0 event)
    }
    else if ((!fIntrospection) && fPreBind)
    {
        RETURN NULL; // There is currently no prebind execution resolve event
    }
    else if (fIntrospection && !fPreBind)
    {
        RETURN NULL; // There is currently no post-bind introspection resolve event
    }
    else
    {
        _ASSERTE( fIntrospection && fPreBind );
        methodId = METHOD__APP_DOMAIN__ON_REFLECTION_ONLY_ASSEMBLY_RESOLVE; // event for introspection assemblies
    }

    // Elevate threads allowed loading level.  This allows the host to load an assembly even in a restricted
    // condition.  Note, however, that this exposes us to possible recursion failures, if the host tries to
    // load the assemblies currently being loaded.  (Such cases would then throw an exception.)

    OVERRIDE_LOAD_LEVEL_LIMIT(FILE_ACTIVE);

    GCX_COOP();

    Assembly* pAssembly = NULL;

    struct _gc {
        OBJECTREF AppDomainRef;
        STRINGREF str;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);
    if ((gc.AppDomainRef = GetRawExposedObject()) != NULL)
    {
        MethodDescCallSite onAssemblyResolve(methodId, &gc.AppDomainRef);

        gc.str = COMString::NewString(ssName);
        ARG_SLOT args[2] = {
            ObjToArgSlot(gc.AppDomainRef),
                ObjToArgSlot(gc.str)
        };
        ASSEMBLYREF ResultingAssemblyRef = (ASSEMBLYREF) onAssemblyResolve.Call_RetOBJECTREF(args);
        if (ResultingAssemblyRef != NULL)
            pAssembly = ResultingAssemblyRef->GetAssembly();
    }
    GCPROTECT_END();

    if (pAssembly != NULL)
    {
        if  ((!(pAssembly->IsIntrospectionOnly())) != (!fIntrospection))
        {
            // Cannot return an introspection assembly from an execution callback or vice-versa
            COMPlusThrow(kFileLoadException, pAssembly->IsIntrospectionOnly() ? IDS_CLASSLOAD_ASSEMBLY_RESOLVE_RETURNED_INTROSPECTION : IDS_CLASSLOAD_ASSEMBLY_RESOLVE_RETURNED_EXECUTION);
        }

        // Check that the public key token matches the one specified in the spec
        // MatchPublicKeys throws as appropriate
        pSpec->MatchPublicKeys(pAssembly);
    }

    RETURN pAssembly;
}

#endif // !DACCESS_COMPILE

MethodTable* TheSByteClass()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    static MethodTable *g_pSByteClass = NULL;
    if (!g_pSByteClass)
        g_pSByteClass = g_Mscorlib.FetchClass(CLASS__SBYTE);

    RETURN g_pSByteClass;
#else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchClass(CLASS__SBYTE);
#endif // DACCESS_COMPILE
}


MethodTable* TheInt16Class()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    static MethodTable *g_pInt16Class = NULL;
    if (!g_pInt16Class)
        g_pInt16Class = g_Mscorlib.FetchClass(CLASS__INT16);

    RETURN g_pInt16Class;
#else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchClass(CLASS__INT16);
#endif // DACCESS_COMPILE
}


MethodTable* TheInt32Class()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    static MethodTable *g_pInt32Class = NULL;
    if (!g_pInt32Class)
        g_pInt32Class = g_Mscorlib.FetchClass(CLASS__INT32);

    RETURN g_pInt32Class;
#else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchClass(CLASS__INT32);
#endif // DACCESS_COMPILE
}


MethodTable* TheByteClass()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    static MethodTable *g_pByteClass = NULL;
    if (!g_pByteClass)
        g_pByteClass = g_Mscorlib.FetchClass(CLASS__BYTE);

    RETURN g_pByteClass;
#else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchClass(CLASS__BYTE);
#endif // DACCESS_COMPILE
}


MethodTable* TheByteArrayClass()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    if (g_pPredefinedArrayTypes[ELEMENT_TYPE_U1]  == NULL)
    {
        // Load the byte class and save it so that we can then make byte arrays
        //    g_Mscorlib.FetchElementType(ELEMENT_TYPE_U1);

        g_pPredefinedArrayTypes[ELEMENT_TYPE_U1] = g_Mscorlib.FetchType(TYPE__BYTE_ARRAY).AsArray();
    }

    RETURN g_pPredefinedArrayTypes[ELEMENT_TYPE_U1]->GetMethodTable();
 #else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchType(TYPE__BYTE_ARRAY).AsArray()->GetMethodTable();
#endif // DACCESS_COMPILE
 }

MethodTable* TheUInt16Class()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    static MethodTable *g_pUInt16Class = NULL;
    if (!g_pUInt16Class)
        g_pUInt16Class = g_Mscorlib.FetchClass(CLASS__UINT16);

    RETURN g_pUInt16Class;
#else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchClass(CLASS__UINT16);
#endif // DACCESS_COMPILE
}


MethodTable* TheUInt32Class()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    static MethodTable *g_pUInt32Class = NULL;
    if (!g_pUInt32Class)
        g_pUInt32Class = g_Mscorlib.FetchClass(CLASS__UINT32);

    RETURN g_pUInt32Class;
#else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchClass(CLASS__UINT32);
#endif // DACCESS_COMPILE
}

MethodTable* TheBooleanClass()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    static MethodTable *g_pBooleanClass = NULL;
    if (!g_pBooleanClass)
        g_pBooleanClass = g_Mscorlib.FetchClass(CLASS__BOOLEAN);

    RETURN g_pBooleanClass;
#else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchClass(CLASS__BOOLEAN);
#endif // DACCESS_COMPILE
}


MethodTable* TheSingleClass()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    static MethodTable *g_pSingleClass = NULL;
    if (!g_pSingleClass)
        g_pSingleClass = g_Mscorlib.FetchClass(CLASS__SINGLE);

    RETURN g_pSingleClass;
#else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchClass(CLASS__SINGLE);
#endif // DACCESS_COMPILE
}

MethodTable* TheDoubleClass()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    static MethodTable *g_pDoubleClass = NULL;
    if (!g_pDoubleClass)
        g_pDoubleClass = g_Mscorlib.FetchClass(CLASS__DOUBLE);

    RETURN g_pDoubleClass;
#else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchClass(CLASS__DOUBLE);
#endif // DACCESS_COMPILE
}

MethodTable* TheIntPtrClass()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    static MethodTable *g_pIntPtrClass = NULL;
    if (!g_pIntPtrClass)
        g_pIntPtrClass = g_Mscorlib.FetchClass(CLASS__INTPTR);

    RETURN g_pIntPtrClass;
#else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchClass(CLASS__INTPTR);
#endif // DACCESS_COMPILE
}


MethodTable* TheUIntPtrClass()
{
    CONTRACT (MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

#ifndef DACCESS_COMPILE
    static MethodTable *g_pUIntPtrClass = NULL;
    if (!g_pUIntPtrClass)
        g_pUIntPtrClass = g_Mscorlib.FetchClass(CLASS__UINTPTR);

    RETURN g_pUIntPtrClass;
#else // DACCESS_COMPILE
    RETURN (&g_Mscorlib)->FetchClass(CLASS__UINTPTR);
#endif // DACCESS_COMPILE
}

#ifndef DACCESS_COMPILE

void AppDomain::InitializeDefaultDomainManager ()
{
    CONTRACTL {
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        THROWS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    struct _gc {
        OBJECTREF   orThis;
        STRINGREF   AppFullName;
        PTRARRAYREF ManifestPathsArray;
        PTRARRAYREF ActivationDataArray;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCX_COOP();
    GCPROTECT_BEGIN(gc);

    LPCWSTR pwzAppFullName = (LPCWSTR) CorCommandLine::m_pwszAppFullName;
    DWORD dwManifestPaths = CorCommandLine::m_dwManifestPaths;
    LPCWSTR *ppwzManifestPaths = (LPCWSTR*) CorCommandLine::m_ppwszManifestPaths;
    DWORD dwActivationData = CorCommandLine::m_dwActivationData;
    LPCWSTR *ppwzActivationData = (LPCWSTR*) CorCommandLine::m_ppwszActivationData;

    gc.AppFullName = COMString::NewString(pwzAppFullName);

    OBJECTREF o;
    if (dwManifestPaths > 0 && ppwzManifestPaths) {
        gc.ManifestPathsArray = (PTRARRAYREF) AllocateObjectArray(dwManifestPaths, g_pStringClass);
        for (DWORD i=0; i<dwManifestPaths; i++) {
            o = COMString::NewString(*ppwzManifestPaths++);
            gc.ManifestPathsArray->SetAt(i, o);
        }
    }

    if (dwActivationData > 0 && ppwzActivationData) {
        gc.ActivationDataArray = (PTRARRAYREF) AllocateObjectArray(dwActivationData, g_pStringClass);
        for (DWORD i=0; i<dwActivationData; i++) {
            o = COMString::NewString(*ppwzActivationData++);
            gc.ActivationDataArray->SetAt(i, o);
        }
    }

    gc.orThis = SystemDomain::System()->DefaultDomain()->GetExposedObject();

    MethodDescCallSite setDefaultDomainManager(METHOD__APP_DOMAIN__SET_DEFAULT_DOMAIN_MANAGER);
    ARG_SLOT args[] = {
        ObjToArgSlot(gc.orThis),
        ObjToArgSlot(gc.AppFullName),
        ObjToArgSlot(gc.ManifestPathsArray),
        ObjToArgSlot(gc.ActivationDataArray),
    };
    setDefaultDomainManager.Call(args);

    GCPROTECT_END();
}

CLREvent * AppDomain::g_pUnloadStartEvent;

void AppDomain::CreateADUnloadWorker()
{
    CONTRACTL
    {
        THROWS;
        MODE_PREEMPTIVE;
        DISABLED(GC_TRIGGERS);
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

Retry:
    BOOL fCreator = FALSE;
    if (FastInterlockCompareExchange((LONG *)&g_fADUnloadWorkerOK,-2,-1)==-1)  //we're first
    {
        Thread *pThread = SetupUnstartedThread();

        if (pThread->CreateNewThread(0, ADUnloadThreadStart, pThread))
        {
            fCreator = TRUE;
            DWORD dwRet;
            dwRet = pThread->StartThread();
            _ASSERTE(dwRet == 1);
        }
        else
        {
            pThread->DecExternalCount(FALSE);
            FastInterlockExchange((LONG *)&g_fADUnloadWorkerOK, -1);
            ThrowOutOfMemory();
        }
    }

    while (g_fADUnloadWorkerOK == -2)
        __SwitchToThread(0);

    if (g_fADUnloadWorkerOK == -1) {
        if (fCreator)
        {
            ThrowOutOfMemory();
        }
        else
        {
            goto Retry;
        }
    }
}

MethodDesc *AppDomain::s_remoteServiceUnloadMD = NULL;
/*static*/ void AppDomain::ADUnloadWorkerHelper(AppDomain *pDomain)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;
    ADUnloadSink* pADUnloadSink=pDomain->GetADUnloadSinkForUnload();
    HRESULT hr=S_OK;

    BEGIN_EXCEPTION_GLUE(&hr,NULL)
    {
        pDomain->Unload(FALSE);
    }
    END_EXCEPTION_GLUE;

    if(pADUnloadSink)
    {
        SystemDomain::LockHolder lh;
        pADUnloadSink->ReportUnloadResult(hr,NULL);
        pADUnloadSink->Release();
    }
}

void AppDomain::DoADUnloadWork()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(s_remoteServiceUnloadMD));
    }
    CONTRACTL_END;

    DWORD i = 1;
    while (TRUE) {

        // Take the lock so that no domain can be added or removed from the system domain
        SystemDomain::LockHolder lh;
        DWORD numDomain = SystemDomain::GetCurrentAppDomainMaxIndex();
        AppDomain *pDomain = NULL;
        for (; i <= numDomain; i ++) {
            pDomain = SystemDomain::TestGetAppDomainAtIndex(ADIndex(i));
            if (pDomain && pDomain->IsUnloadRequested())
            {
                i ++;
                break;
            }
            else
            {
                pDomain = NULL;
            }
        }
        if (pDomain) {
            // We are the only thread that can unload domains so no one else can delete the appdomain
            lh.Release();
            ADUnloadWorkerHelper(pDomain);
        }
        else
            break;
    }


}

static void DoADUnloadWorkHelper()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    EX_TRY {
        AppDomain::DoADUnloadWork();
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);
}

ULONGLONG g_ObjFinalizeStartTime = 0;
volatile BOOL g_FinalizerIsRunning = FALSE;

ULONGLONG GetObjFinalizeStartTime()
{
    LEAF_CONTRACT;
    return g_ObjFinalizeStartTime;
}

void FinalizerThreadAbortOnTimeout()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_GC_TRIGGERS;

    {
        // If finalizer thread is blocked because scheduler is running another task,
        // or it is waiting for another thread, we first see if we get finalizer thread
        // running again.
        Thread::ThreadAbortWatchDog();
    }

    EX_TRY
    {
        Thread *pFinalizerThread = GCHeap::GetGCHeap()->GetFinalizerThread();
        EPolicyAction action = GetEEPolicy()->GetActionOnTimeout(OPR_FinalizerRun, pFinalizerThread);
        switch (action)
        {
        case eAbortThread:
            GetEEPolicy()->NotifyHostOnTimeout(OPR_FinalizerRun, action);
            pFinalizerThread->UserAbort(Thread::TAR_Thread,
                                        EEPolicy::TA_Safe,
                                        INFINITE,
                                        Thread::UAC_WatchDog);
            break;
        case eRudeAbortThread:
            GetEEPolicy()->NotifyHostOnTimeout(OPR_FinalizerRun, action);
            pFinalizerThread->UserAbort(Thread::TAR_Thread,
                                        EEPolicy::TA_Rude,
                                        INFINITE,
                                        Thread::UAC_WatchDog);
            break;
        case eUnloadAppDomain:
            {
                AppDomain *pDomain = pFinalizerThread->GetDomain();
                pFinalizerThread->UserAbort(Thread::TAR_Thread,
                                            EEPolicy::TA_Safe,
                                            INFINITE,
                                            Thread::UAC_WatchDog);
                if (!pDomain->IsDefaultDomain())
                {
                    GetEEPolicy()->NotifyHostOnTimeout(OPR_FinalizerRun, action);
                    pDomain->EnableADUnloadWorker(EEPolicy::ADU_Safe);
                }
            }
            break;
        case eRudeUnloadAppDomain:
            {
                AppDomain *pDomain = pFinalizerThread->GetDomain();
                pFinalizerThread->UserAbort(Thread::TAR_Thread,
                                            EEPolicy::TA_Rude,
                                            INFINITE,
                                            Thread::UAC_WatchDog);
                if (!pDomain->IsDefaultDomain())
                {
                    GetEEPolicy()->NotifyHostOnTimeout(OPR_FinalizerRun, action);
                    pDomain->EnableADUnloadWorker(EEPolicy::ADU_Rude);
                }
            }
            break;
        case eExitProcess:
        case eFastExitProcess:
        case eRudeExitProcess:
        case eDisableRuntime:
            GetEEPolicy()->NotifyHostOnTimeout(OPR_FinalizerRun, action);
            EEPolicy::HandleExitProcessFromEscalation(action, HOST_E_EXITPROCESS_TIMEOUT);
            _ASSERTE (!"Should not get here");
            break;
        default:
            break;
        }
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);
}

enum WorkType
{
    WT_UnloadDomain = 0x1,
    WT_ThreadAbort = 0x2,
    WT_FinalizerThread = 0x4,
    WT_ClearCollectedDomains=0x8
};

static volatile DWORD s_WorkType = 0;


DWORD WINAPI AppDomain::ADUnloadThreadStart(void *args)
{

    ClrFlsSetThreadType (ThreadType_ADUnloadHelper);

    CONTRACTL
    {
        NOTHROW;
        DISABLED(GC_TRIGGERS);

        ENTRY_POINT;
    }
    CONTRACTL_END;

    BEGIN_ENTRYPOINT_NOTHROW;

    Thread *pThread = (Thread*)args;
    BOOL fOK = pThread->HasStarted();

    if (fOK)
    {
        pThread->EnablePreemptiveGC();

        EX_TRY
        {
            if (CLRTaskHosted())
            {
                // ADUnload helper thread is critical.  We do not want it to share scheduler
                // with other tasks.
                pThread->LeaveRuntime(0);
            }
        }
        EX_CATCH
        {
            fOK = FALSE;
        }
        EX_END_CATCH(SwallowAllExceptions);
    }

    _ASSERTE (g_fADUnloadWorkerOK == -2);

    FastInterlockExchange((LONG *)&g_fADUnloadWorkerOK,fOK?1:-1);

    if (!fOK) {
        DestroyThread(pThread);
        goto Exit;
    }

    pThread->SetBackground(TRUE);

    pThread->SetThreadStateNC(Thread::TSNC_ADUnloadHelper);

    while (TRUE) {
        DWORD TAtimeout = INFINITE;
        ULONGLONG endTime = Thread::GetNextSelfAbortEndTime();
        ULONGLONG curTime = CLRGetTickCount64();
        if (endTime <= curTime) {
            TAtimeout = 5;
        }
        else
        {
            ULONGLONG diff = endTime - curTime;
            if (diff < MAXULONG)
            {
                TAtimeout = (DWORD)diff;
            }
        }
        ULONGLONG finalizeStartTime = GetObjFinalizeStartTime();
        DWORD finalizeTimeout = INFINITE;
        DWORD finalizeTimeoutSetting = GetEEPolicy()->GetTimeout(OPR_FinalizerRun);
        if (finalizeTimeoutSetting != INFINITE && g_FinalizerIsRunning)
        {
            if (finalizeStartTime == 0)
            {
                finalizeTimeout = finalizeTimeoutSetting;
            }
            else
            {
                endTime = finalizeStartTime + finalizeTimeoutSetting;
                if (endTime <= curTime) {
                    finalizeTimeout = 0;
                }
                else
                {
                    ULONGLONG diff = endTime - curTime;
                    if (diff < MAXULONG)
                    {
                        finalizeTimeout = (DWORD)diff;
                    }
                }
            }
        }

        if (AppDomain::HasWorkForFinalizerThread())
        {
            if (finalizeTimeout > finalizeTimeoutSetting)
            {
                finalizeTimeout = finalizeTimeoutSetting;
            }
        }

        DWORD timeout = INFINITE;
        if (finalizeTimeout <= TAtimeout)
        {
            timeout = finalizeTimeout;
        }
        else
        {
            timeout = TAtimeout;
        }

        if (timeout != 0)
        {
            LOG((LF_APPDOMAIN, LL_INFO10, "Waiting to start unload\n"));
            g_pUnloadStartEvent->Wait(timeout,FALSE);
        }

        if (finalizeTimeout != INFINITE || (s_WorkType & WT_FinalizerThread) != 0)
        {
            STRESS_LOG0(LF_ALWAYS, LL_ALWAYS, "ADUnloadThreadStart work for Finalizer thread\n");
            FastInterlockAnd(&s_WorkType, ~WT_FinalizerThread);
            if (GetObjFinalizeStartTime() == finalizeStartTime && finalizeStartTime != 0)
            {
                if (CLRGetTickCount64() >= finalizeStartTime+finalizeTimeoutSetting)
                {
                    GCX_COOP();
                    FinalizerThreadAbortOnTimeout();
                }
            }
            if (s_fProcessUnloadDomainEvent)
            {
                GCX_COOP();
                FinalizerThreadAbortOnTimeout();
            }
        }

        if (TAtimeout != INFINITE || (s_WorkType & WT_ThreadAbort) != 0)
        {
            STRESS_LOG0(LF_ALWAYS, LL_ALWAYS, "ADUnloadThreadStart work for thread abort\n");
            FastInterlockAnd(&s_WorkType, ~WT_ThreadAbort);
            GCX_COOP();
            Thread::ThreadAbortWatchDog();
        }

        if ((s_WorkType & WT_UnloadDomain) != 0 && !AppDomain::HasWorkForFinalizerThread())
        {
            STRESS_LOG0(LF_ALWAYS, LL_ALWAYS, "ADUnloadThreadStart work for AD unload\n");
            FastInterlockAnd(&s_WorkType, ~WT_UnloadDomain);
            GCX_COOP();
            DoADUnloadWorkHelper();
        }

        if ((s_WorkType & WT_ClearCollectedDomains) != 0)
        {
            STRESS_LOG0(LF_ALWAYS, LL_ALWAYS, "ADUnloadThreadStart work for AD cleanup\n");
            FastInterlockAnd(&s_WorkType, ~WT_ClearCollectedDomains);
            GCX_COOP();
            SystemDomain::System()->ClearCollectedDomains();
        }

    }

Exit:

#ifndef TOTALLY_DISBLE_STACK_GUARDS 
    // We may have destroyed Thread object.  It is not safe to use cached Thread object in 
    // DebugSOIntolerantTransitionHandler or SOIntolerantTransitionHandler
    __soIntolerantTransitionHandler.SetThread(NULL);
#endif

    END_ENTRYPOINT_NOTHROW;

    return 0;
}

void AppDomain::EnableADUnloadWorker(EEPolicy::AppDomainUnloadTypes type, BOOL fHasStack)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT; // Called during a SO
    }
    CONTRACTL_END;

    FastInterlockOr (&s_WorkType, WT_UnloadDomain);

    LONG stage = m_Stage;
    C_ASSERT(sizeof(m_Stage) == sizeof(int));

    _ASSERTE(!IsDefaultDomain());

    // Mark unload requested.
    if (type == EEPolicy::ADU_Rude) {
        SetRudeUnload();
    }
    while (stage < STAGE_UNLOAD_REQUESTED) {
        stage = FastInterlockCompareExchange((LONG*)&m_Stage,STAGE_UNLOAD_REQUESTED,stage);
    }

    if (!fHasStack)
    {
        // Can not call Set due to limited stack.
        return;
    }
    LOG((LF_APPDOMAIN, LL_INFO10, "Enabling unload worker\n"));
    g_pUnloadStartEvent->Set();
}

void AppDomain::EnableADUnloadWorkerForThreadAbort()
{
    LEAF_CONTRACT;
    STRESS_LOG0(LF_ALWAYS, LL_ALWAYS, "Enabling unload worker for thread abort\n");
    LOG((LF_APPDOMAIN, LL_INFO10, "Enabling unload worker for thread abort\n"));
    FastInterlockOr (&s_WorkType, WT_ThreadAbort);
    g_pUnloadStartEvent->Set();
}


void AppDomain::EnableADUnloadWorkerForFinalizer()
{
    LEAF_CONTRACT;
    if (GetEEPolicy()->GetTimeout(OPR_FinalizerRun) != INFINITE)
    {
        LOG((LF_APPDOMAIN, LL_INFO10, "Enabling unload worker for Finalizer Thread\n"));
        FastInterlockOr (&s_WorkType, WT_FinalizerThread);
        g_pUnloadStartEvent->Set();
    }
}

void AppDomain::EnableADUnloadWorkerForCollectedADCleanup()
{
    LEAF_CONTRACT;
    LOG((LF_APPDOMAIN, LL_INFO10, "Enabling unload worker for collected domains\n"));
    FastInterlockOr (&s_WorkType, WT_ClearCollectedDomains);
    g_pUnloadStartEvent->Set();
}


void ADUnloadSink::ReportUnloadResult (HRESULT hr, OBJECTREF* pException)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(m_UnloadCompleteEvent.IsValid());
    }
    CONTRACTL_END;

    //pException is unused;
    m_UnloadResult=hr;
    m_UnloadCompleteEvent.Set();
};

void ADUnloadSink::WaitUnloadCompletion()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(m_UnloadCompleteEvent.IsValid());
    }
    CONTRACTL_END;
    CONTRACT_VIOLATION(FaultViolation);
    m_UnloadCompleteEvent.WaitEx(INFINITE, (WaitMode)(WaitMode_Alertable | WaitMode_ADUnload));
};

ADUnloadSink* AppDomain::PrepareForWaitUnloadCompletion()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(SystemDomain::IsUnderDomainLock());
        FORBID_FAULT;
    }
    CONTRACTL_END;

    ADUnloadSink* pADSink=GetADUnloadSink();
    PREFIX_ASSUME(pADSink!=NULL);
    if (m_Stage < AppDomain::STAGE_UNLOAD_REQUESTED) //we're first
    {
        pADSink->Reset();
        SetUnloadRequestThread(GetThread());
    }
    return pADSink;
};

ADUnloadSink::ADUnloadSink()
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    m_cRef=1;
    m_UnloadCompleteEvent.CreateManualEvent(FALSE);
    m_UnloadResult=S_OK;
};

ADUnloadSink::~ADUnloadSink()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    m_UnloadCompleteEvent.CloseEvent();

};


ULONG ADUnloadSink::AddRef()
{
    LEAF_CONTRACT;
    return InterlockedIncrement(&m_cRef);
};

ULONG ADUnloadSink::Release()
{
    LEAF_CONTRACT;
    ULONG ulRef = InterlockedDecrement(&m_cRef);
    if (ulRef == 0)
    {
        delete this;
        return 0;
    }
    return ulRef;
};

void ADUnloadSink::Reset()
{
    LEAF_CONTRACT;
    m_UnloadResult=S_OK;
    m_UnloadCompleteEvent.Reset();
}

ADUnloadSink* AppDomain::GetADUnloadSink()
{
    LEAF_CONTRACT;
    _ASSERTE(SystemDomain::IsUnderDomainLock());
    if(m_ADUnloadSink)
        m_ADUnloadSink->AddRef();
    return m_ADUnloadSink;
};

ADUnloadSink* AppDomain::GetADUnloadSinkForUnload()
{
    // unload thread only. Doesn't need to have AD lock
    LEAF_CONTRACT;
    if(m_ADUnloadSink)
        m_ADUnloadSink->AddRef();
    return m_ADUnloadSink;
};


void AppDomain::EnumStaticGCRefs(GCEnumCallback  pCallback, LPVOID hCallBack)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    _ASSERTE(GCHeap::IsGCInProgress() &&
             GCHeap::IsServerHeap()   &&
             IsGCSpecialThread());

    AppDomain::AssemblyIterator asmIterator = IterateAssembliesEx((AssemblyIterationFlags)(kIncludeLoaded | kIncludeExecution));
    while (asmIterator.Next())
    {
        DomainAssembly* pDomainAssembly = asmIterator.GetDomainAssembly();
        pDomainAssembly->EnumStaticGCRefs(pCallback, hCallBack);
    }

    RETURN;
}



#endif // !DACCESS_COMPILE

//------------------------------------------------------------------------
UINT32 BaseDomain::GetTypeID(PTR_MethodTable pMT) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(pMT->GetDomain() == this);
    } CONTRACTL_END;

    return m_typeIDMap.GetTypeID(pMT);
}

//------------------------------------------------------------------------
// Returns the ID of the type if found. If not found, returns INVALID_TYPE_ID
UINT32 BaseDomain::LookupTypeID(PTR_MethodTable pMT)
{
    CONTRACTL {
        NOTHROW;
        SO_TOLERANT;
        WRAPPER(GC_TRIGGERS);
        PRECONDITION(pMT->GetDomain() == this);
    } CONTRACTL_END;

    return m_typeIDMap.LookupTypeID(pMT);
}

//------------------------------------------------------------------------
PTR_MethodTable BaseDomain::LookupType(UINT32 id) {
    CONTRACTL {
        NOTHROW;
        SO_TOLERANT;
        WRAPPER(GC_TRIGGERS);
        CONSISTENCY_CHECK(id != TYPE_ID_THIS_CLASS);
    } CONTRACTL_END;

    PTR_MethodTable pMT = m_typeIDMap.LookupType(id);
    if (pMT == NULL && !IsSharedDomain()) {
        pMT = SharedDomain::GetDomain()->LookupType(id);
    }

    CONSISTENCY_CHECK(CheckPointer(pMT));
    CONSISTENCY_CHECK(pMT->IsInterface());
    return pMT;
}


#ifndef DACCESS_COMPILE


BOOL GetCompatibilityFlag(CompatibilityFlag flag)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    } CONTRACTL_END;

    // No compatibility flags for FEATURE_PAL
    return FALSE;
}
#endif // !DACCESS_COMPILE

//------------------------------------------------------------------------
BOOL AppDomain::AssemblyIterator::Next()
{
    WRAPPER_CONTRACT;
    while (m_i.Next())
    {
        DomainAssembly *pDomainAssembly = GetDomainAssembly();
        if(!pDomainAssembly)
            continue;

        if (!pDomainAssembly->IsError())
        {
            if (pDomainAssembly->IsLoaded())
            {
                // A loaded assembly
                if (!(m_assemblyIterationFlags & kIncludeLoaded))
                {
                    continue; // reject
                }
            }
            else
            {
                // A loading assembly
                if (!(m_assemblyIterationFlags & kIncludeLoading))
                {
                    continue; // reject
                }
            }

            if (pDomainAssembly->IsIntrospectionOnly())
            {
                // introspection assembly
                if (!(m_assemblyIterationFlags & kIncludeIntrospection))
                {
                    continue; // reject
                }
            }
            else
            {
                // execution assembly
                if (!(m_assemblyIterationFlags & kIncludeExecution))
                {
                    continue; // reject
                }
            }

            return TRUE;
        }
        else
            if (m_assemblyIterationFlags & kIncludeFailedToLoad)
                return TRUE;
    }

    return FALSE;
}

//approximate size of loader data
//maintained for each assembly
#define APPROX_LOADER_DATA_PER_ASSEMBLY 8196

size_t AppDomain::EstimateSize()
{
    LEAF_CONTRACT;
    size_t retval=sizeof(AppDomain);
    if(m_pHighFrequencyHeap) 
        retval+=m_pHighFrequencyHeap->GetSize();
    if(m_pLowFrequencyHeap) 
        retval+=m_pLowFrequencyHeap->GetSize();  
     if(m_pStubHeap) 
        retval+=m_pStubHeap->GetSize();   
    if(m_pVirtualCallStubManager)
        retval+=m_pVirtualCallStubManager->GetSize();
    //very rough estimate
    retval+=GetAssemblyCount()*APPROX_LOADER_DATA_PER_ASSEMBLY;
    if(m_pStringLiteralMap)
        retval+=m_pStringLiteralMap->GetSize();
    return retval;
}

#ifdef DACCESS_COMPILE

void
DomainLocalModule::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    // XXX drewb - How big is this block?
    DAC_ENUM_DTHIS();

    if (m_pDomainFile.IsValid())
    {
        m_pDomainFile->EnumMemoryRegions(flags);
    }

    if (m_pDynamicClassTable.IsValid())
    {
        DacEnumMemoryRegion(PTR_TO_TADDR(m_pDynamicClassTable),
                            m_aDynamicEntries * sizeof(DynamicClassInfo));

        for (SIZE_T i = 0; i < m_aDynamicEntries; i++)
        {
            PTR_DynamicEntry entry = PTR_DynamicEntry(PTR_TO_TADDR(m_pDynamicClassTable[i].m_pDynamicEntry));
            if (entry.IsValid())
            {
                // XXX drewb - How big is the block?
                entry.EnumMem();
            }
        }
    }
}

void
DomainLocalBlock::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    // Block is contained in AppDomain, don't enum this.

    if (m_pModuleSlots.IsValid())
    {
        DacEnumMemoryRegion(PTR_TO_TADDR(m_pModuleSlots),
                            m_aModuleIndices * sizeof(TADDR));

        for (SIZE_T i = 0; i < m_aModuleIndices; i++)
        {
            PTR_DomainLocalModule domMod = m_pModuleSlots[i];
            if (domMod.IsValid())
            {
                domMod->EnumMemoryRegions(flags);
            }
        }
    }
}

void
BaseDomain::EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                              bool enumThis)
{
    if (enumThis)
    {
        DAC_ENUM_VTHIS();
    }

    EMEM_OUT(("MEM: %p BaseDomain\n", PTR_HOST_TO_TADDR(this)));

    if (m_pLowFrequencyHeap.IsValid())
    {
        m_pLowFrequencyHeap->EnumMemoryRegions(flags);
    }
    if (m_pHighFrequencyHeap.IsValid())
    {
        m_pHighFrequencyHeap->EnumMemoryRegions(flags);
    }
    if (m_pStubHeap.IsValid())
    {
        m_pStubHeap->EnumMemoryRegions(flags);
    }
}

void
AppDomain::EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                             bool enumThis)
{
    if (enumThis)
    {
        DAC_ENUM_VTHIS();
    }
    BaseDomain::EnumMemoryRegions(flags, false);

    m_friendlyName.EnumMemoryRegions();

    m_Assemblies.EnumMemoryRegions(flags);
    AssemblyIterator assem = IterateAssembliesEx((AssemblyIterationFlags)(kIncludeLoaded | kIncludeExecution | kIncludeIntrospection));
    while (assem.Next())
    {
        assem.GetDomainAssembly()->EnumMemoryRegions(flags);
    }

    m_sDomainLocalBlock.EnumMemoryRegions(flags);
}

void
SystemDomain::EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                                bool enumThis)
{
    if (enumThis)
    {
        DAC_ENUM_VTHIS();
    }
    BaseDomain::EnumMemoryRegions(flags, false);

    if (m_pSystemFile.IsValid())
    {
        m_pSystemFile->EnumMemoryRegions(flags);
    }
    if (m_pSystemAssembly.IsValid())
    {
        m_pSystemAssembly->EnumMemoryRegions(flags);
    }
    if (m_pChildren.IsValid())
    {
        m_pChildren->EnumMemoryRegions(flags, true);
    }
    if (m_pDefaultDomain.IsValid())
    {
        m_pDefaultDomain->EnumMemoryRegions(flags, true);
    }
    if (m_pPool.IsValid())
    {
        m_pPool->EnumMemoryRegions(flags, true);
    }
    if (m_pBaseComObjectClass.IsValid())
    {
        m_pBaseComObjectClass->EnumMemoryRegions(flags);
    }

    m_appDomainIndexList.EnumMem();
    (&m_appDomainIndexList)->EnumMemoryRegions(flags);
}

void
SharedDomain::EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                                bool enumThis)
{
    if (enumThis)
    {
        DAC_ENUM_VTHIS();
    }
    BaseDomain::EnumMemoryRegions(flags, false);

    m_assemblyMap.EnumMemoryRegions(flags);

    SharedAssemblyIterator assem;
    while (assem.Next())
    {
        assem.GetAssembly()->EnumMemoryRegions(flags);
    }
}



#endif // DACCESS_COMPILE




