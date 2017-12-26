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
// codeman.cpp - a managment class for handling multiple code managers
//
#include "common.h"
#include "jitinterface.h"
#include "corjit.h"
#include "eetwain.h"
#include "eeconfig.h"
#include "excep.h"
#include "appdomain.hpp"
#include "codeman.h"
#include "codeman.inl"
#include "nibblemapmacros.h"
#include "generics.h"
#include "dynamicmethod.h"
#include "eemessagebox.h"


#include "fjit_eetwain.h"
#include "jitperf.h"
#include "shimload.h"
#include "memoryreport.h"
#include "debuginfostore.h"


#define MAX_M_ALLOCATED         (16 * 1024)

SPTR_IMPL(RangeSection, ExecutionManager, m_CodeRangeList);
SPTR_IMPL(RangeSection, ExecutionManager, m_DataRangeList);
SPTR_IMPL(IJitManager,  ExecutionManager, m_pJitList);
#ifndef FJITONLY
SPTR_IMPL(EECodeManager, ExecutionManager, m_pDefaultCodeMan);
#endif

#ifdef DACCESS_COMPILE
SVAL_IMPL(LONG, ExecutionManager, m_dwReaderCount);
SVAL_IMPL(LONG, ExecutionManager, m_dwWriterLock);
#endif

#ifndef DACCESS_COMPILE

CrstStatic ExecutionManager::m_ExecutionManagerCrst;
CrstStatic ExecutionManager::m_JumpStubCrst;
CrstStatic ExecutionManager::m_RangeCrst;
BYTE ExecutionManager::m_fFailedToLoad = 0x00;
volatile LONG ExecutionManager::m_dwReaderCount = 0;
volatile LONG ExecutionManager::m_dwWriterLock = 0;
JumpStubBlockHeader *  ExecutionManager::m_jumpStubBlock;
ExecutionManager::JumpStubTable*    ExecutionManager::m_jumpStubTable;


/*-----------------------------------------------------------------------------
 This is a listing of which methods uses which synchronization mechanism
 in the EEJitManager.
//-----------------------------------------------------------------------------

Setters of EEJitManager::m_CodeHeapCritSec
-----------------------------------------------
allocCode
allocEntryChunk
allocGCInfo
allocEHInfo
allocJumpStubBlock
ResolveEHClause
RemoveJitData
Unload
ReleaseReferenceToHeap
JitCodeToMethodInfo


Need EEJitManager::m_CodeHeapCritSec to be set
-----------------------------------------------
NewCodeHeap
allocCodeRaw
GetCodeHeapList
GetCodeHeap
RemoveCodeHeapFromDomainList
DeleteCodeHeap
AddRangeToJitHeapCache
DeleteJitHeapCache


HeapList::changeStart/changeEnd
-----------------------------------------------
allocCode
allocEntryChunk
allocJumpStubBlock
RemoveJitData
JitCodeToMethodInfo (read-only)

*/

void EEJitManager::IncrementReader(EEJitManager *pMgr)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    } CONTRACTL_END;

    _ASSERTE(pMgr != NULL);
    CounterHolder h(&pMgr->m_dwReaderCount);
    while (pMgr->m_dwWriterLock)
        __SwitchToThread(0);
    h.SuppressRelease();
}

void EEJitManager::DecrementReader(EEJitManager *pMgr)
{
    LEAF_CONTRACT;

    FastInterlockDecrement(&pMgr->m_dwReaderCount);
}

void EEJitManager::TakeWriter(EEJitManager *pMgr)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    } CONTRACTL_END;

    _ASSERTE(pMgr != NULL);
    _ASSERTE(pMgr->m_dwWriterLock == 0);

    // Signal to a debugger or hijacking profiler that it's not safe to do a stackwalk now
    IncCantStopCount(StateHolderParamValue);
    
    while (TRUE)
    {
        Thread::IncForbidSuspendThread(StateHolderParamValue);
        FastInterlockIncrement(&pMgr->m_dwWriterLock);
        if (pMgr->m_dwReaderCount == 0)
            break;
        FastInterlockDecrement(&pMgr->m_dwWriterLock);
        Thread::DecForbidSuspendThread(StateHolderParamValue);
        __SwitchToThread(0);
    }
}

void EEJitManager::ReleaseWriter(EEJitManager *pMgr)
{
    LEAF_CONTRACT;

    FastInterlockDecrement(&pMgr->m_dwWriterLock);
    
    // Signal to a debugger or hijacking profiler that it's again safe to do a stackwalk now    
    Thread::DecForbidSuspendThread(StateHolderParamValue);
    DecCantStopCount(StateHolderParamValue);
}

/*-----------------------------------------------------------------------------
 This is a listing of which methods uses which synchronization mechanism
 in the EEJitManager
//-----------------------------------------------------------------------------

==============================================================================
ExecutionManger::ReaderLockHolder and ExecutionManger::WriterLockHolder
Protects the callers of ExecutionManager::GetRangeSection from heap deletions
while walking RangeSections.  You need to take a reader lock before reading the
values: m_CodeRangeList or m_DataRangeList and hold it while walking the lists
The two xxxWrapper functions allows us to avoid taking a ReaderLock in some cases

Uses ReaderLockHolder (allows multiple reeaders with no writers)
-----------------------------------------
ExecutionManager::FindJitManNonZeroWrapper
ExecutionManager::GetMethodHeaderForAddressWrapper
ExecutionManager::GetRangeSectionForAddress
ExecutionManager::FindZapModule
ExecutionManager::FindZapModuleForNativeCode
ExecutionManager::AddRangeHelper
ExecutionManager::EnumMemoryRegions

Uses WriterLockHolder (allows single writer and no readers)
-----------------------------------------
ExecutionManager::DeleteRangeHelper


==============================================================================
EEJitManger::ReaderLockHolder and EEJitManager::WriterLockHolder
Protects the callers of EEJitManager::JitCodeToMethodInfo from heap deletions
while walking HeapLists.  You need to take a reader lock before reading the
value of pCodeHeap (using GetVolatile_pCodeHeap() and hold it while walking
the HeapList usinging GetVolatile_hpNext

Uses ReaderLockHolder (allows multiple reeaders with no writers)
-----------------------------------------
EEJitManager::JitCodeToMethodDescWrapper

Uses WriterLockHolder (allows single writer and no readers)
-----------------------------------------
EEJitManager::DeleteCodeHeap

ExecutionManager::m_ExecutionManagerCrst
-----------------------------------------
GetJitForType

*/

//-----------------------------------------------------------------------------

/* static */
void ExecutionManager::IncrementReader(ExecutionManager *)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    } CONTRACTL_END;

    FastInterlockIncrement(&m_dwReaderCount);
    while (m_dwWriterLock)
        __SwitchToThread(0);
    IncCantAllocCount();    
}

/* static */
void ExecutionManager::DecrementReader(ExecutionManager *)
{
    LEAF_CONTRACT;

    DecCantAllocCount();
    FastInterlockDecrement(&m_dwReaderCount);    
}

/* static */
void ExecutionManager::TakeWriter(ExecutionManager *)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(m_dwWriterLock == 0);
    while (TRUE)
    {
        FastInterlockIncrement(&m_dwWriterLock);
        if (m_dwReaderCount == 0)
            break;
        FastInterlockDecrement(&m_dwWriterLock);
        __SwitchToThread(0);
    }
    IncCantAllocCount();    
}

/* static */
void ExecutionManager::ReleaseWriter(ExecutionManager *)
{
    LEAF_CONTRACT;
    
    DecCantAllocCount();
    FastInterlockDecrement(&m_dwWriterLock);
}


//**********************************************************************************
//  IJitManager
//**********************************************************************************
IJitManager::IJitManager()
{
    LEAF_CONTRACT;

    m_IsDefaultCodeMan = FALSE;
    m_runtimeSupport   = NULL;
    m_JITCompiler      = NULL;
}

IJitManager::~IJitManager()
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    } CONTRACTL_END;

   // Unload the JIT DLL
   if ((m_runtimeSupport) && (!m_IsDefaultCodeMan))
        delete m_runtimeSupport;

   m_runtimeSupport = NULL;

   if (m_JITCompiler) {
        if(!g_fProcessDetach || !RunningOnWin95())
        {
            CLRFreeLibrary(m_JITCompiler);
        }
    }
    // Null all pointers to make sure checks fail!
    m_JITCompiler = NULL;
}

void  SetCpuInfo()
{
    //
    // NOTE: This function needs to be kept in sync with Zapper::CompileAssembly()
    //

#ifdef _X86_

    // NOTE: if you're adding any flags here, you probably should also be doing it
    // for ngen (zapper.cpp)
    CORINFO_CPU cpuInfo;
    GetSpecificCpuInfo(&cpuInfo);

    switch (CPU_X86_FAMILY(cpuInfo.dwCPUType))
    {
    case CPU_X86_PENTIUM:
        g_pConfig->SetCpuFlag(CORJIT_FLG_TARGET_PENTIUM);
        break;
    case CPU_X86_PENTIUM_PRO:
        // check for the mobile class
        if(CPU_X86_MODEL(cpuInfo.dwCPUType) == CPU_X86_MODEL_PENTIUM_PRO_BANIAS)
            g_pConfig->SetCpuFlag(CORJIT_FLG_TARGET_BANIAS);
        else
            g_pConfig->SetCpuFlag(CORJIT_FLG_TARGET_PPRO);
        break;
    case CPU_X86_PENTIUM_4:
        g_pConfig->SetCpuFlag(CORJIT_FLG_TARGET_P4);
        break;
    default:
        g_pConfig->SetCpuFlag(0);
        break;
    }

    if (CPU_X86_USE_CMOV(cpuInfo.dwFeatures))
    {
        g_pConfig->SetCpuCapabilities(g_pConfig->GetCpuCapabilities() |
                                      CORJIT_FLG_USE_CMOV |
                                      CORJIT_FLG_USE_FCOMI);
    }

#ifdef PLATFORM_UNIX
#else
    if (CPU_X86_USE_SSE2(cpuInfo.dwFeatures))
    {
        g_pConfig->SetCpuCapabilities(g_pConfig->GetCpuCapabilities() |
                                      CORJIT_FLG_USE_SSE2);
    }
#endif
    
#endif // _X86_
}

BOOL IJitManager::LoadJIT(LPCWSTR szJITdll)
{
    Thread  *thread = GetThread();
    BOOL     toggleGC = (thread && thread->PreemptiveGCDisabled());
    BOOL     res = TRUE;

    CONTRACTL {
        NOTHROW;
        if (toggleGC) GC_TRIGGERS; else GC_NOTRIGGER;
    } CONTRACTL_END;

    if (toggleGC)
        thread->EnablePreemptiveGC();

    m_JITCompiler = CLRLoadSystemLibrary(szJITdll);

    if (!m_JITCompiler)
    {
        res = FALSE;
        goto leav;
    }

    {
        typedef ICorJitCompiler* (__stdcall* pGetJitFn)();
        pGetJitFn getJitFn = (pGetJitFn) GetProcAddress(m_JITCompiler, "getJit");

        SetCpuInfo();

        EX_TRY
        {
            if (getJitFn)
                m_jit = (*getJitFn)();
        }
        EX_CATCH
        {
        }
        EX_END_CATCH(SwallowAllExceptions)

        if (!m_jit)
        {
            res = FALSE;
            goto leav;
        }
    }

leav:
    if (toggleGC)
        thread->DisablePreemptiveGC();

    return res;
}


#endif // #ifndef DACCESS_COMPILE

// When we unload an appdomain, we need to make sure that any threads that are crawling through
// our heap or rangelist are out. For cooperative-mode threads, we know that they will have
// been stopped when we suspend the EE so they won't be touching an element that is about to be deleted.
// However for pre-emptive mode threads, they could be stalled right on top of the element we want
// to delete, so we need to apply the reader lock to them and wait for them to drain.
IJitManager::ScanFlag IJitManager::GetScanFlags()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    } CONTRACTL_END;

#ifndef DACCESS_COMPILE
    Thread *pThread = GetThread();

    if (!pThread)
        return ScanNoReaderLock;

    // If this thread is hijacked by a profiler and crawling its own stack,
    // we do need to take the lock
    if (pThread->GetProfilerFilterContext() != NULL)
        return ScanReaderLock;
    
    if (pThread->PreemptiveGCDisabled() || pThread == GCHeap::GetGCHeap()->GetGCThread())
        return ScanNoReaderLock;

    return ScanReaderLock;
#else
    return ScanNoReaderLock;
#endif
}

#ifdef DACCESS_COMPILE

void IJitManager::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_VTHIS();
    if (m_runtimeSupport.IsValid())
    {
        m_runtimeSupport->EnumMemoryRegions(flags);
    }
}

#endif // #ifdef DACCESS_COMPILE


#ifndef DACCESS_COMPILE

//**********************************************************************************
//  EEJitManager
//**********************************************************************************

EEJitManager::EEJitManager()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    } CONTRACTL_END;

    m_next = NULL;
    m_pCodeHeap = NULL;
    m_jit = NULL;
    m_dwReaderCount = 0;
    m_dwWriterLock = 0;

    for (unsigned int i=0; i<HASH_BUCKETS; i++)
        m_JitCodeHashTable[i] = NULL;
    m_pJitHeapCacheUnlinkedList = NULL;

    // There is a situation that we have seen in the debugger (currently only on IA64) where we are doing
    // a right-side requested stackwalk of the threads and there is a thread that isn't in coop mode so
    // doesn't get suspended, when we get around to doing the stackwalk it is Jitting and we run into 
    // conflicts trying to read the nibble map using EEJitManager::JitCodeToMethodInfo which tries to
    // take this lock if it can't get a clean copy of the nibble map on its first time through.
    //
    // As a workaround we're adding the CRST_DEBUGGER_THREAD attribute to this Crst's init.
    m_CodeHeapCritSec.Init("JitMetaHeapCrst", 
                        CrstSingleUseLock, 
                        (CrstFlags)(CRST_UNSAFE_ANYMODE|CRST_DEBUGGER_THREAD));


}

HeapList* LoaderCodeHeap::CreateCodeHeap(CodeHeapRequestInfo *pInfo, LoaderHeap *pJitMetaHeap)
{
    CONTRACT(HeapList *) {
        THROWS;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    size_t * pPrivatePCLBytes   = NULL;
    size_t   reserveSize        = pInfo->getReserveSize();
#ifdef _DEBUG
    size_t   initialRequestSize = pInfo->getRequestSize();
#endif
    size_t   cacheSize          = pInfo->getCacheSize();
    const BYTE *   loAddr       = pInfo->m_loAddr;
    const BYTE *   hiAddr       = pInfo->m_hiAddr;

    // Make sure that what we are reserving will fix inside a DWORD
    if (reserveSize != (DWORD) reserveSize)
    {
        _ASSERTE(!"reserveSize does not fit in a DWORD");
        EEPOLICY_HANDLE_FATAL_ERROR(COR_E_EXECUTIONENGINE);
    }

#ifdef PROFILING_SUPPORTED
    pPrivatePCLBytes   = &(GetPrivatePerfCounters().m_Loading.cbLoaderHeapSize);
#endif

    LOG((LF_JIT, LL_INFO100,
         "Request new LoaderCodeHeap::CreateCodeHeap(%08x, %08x, for domain" FMT_ADDR "in" FMT_ADDR ".." FMT_ADDR ")\n",
         (DWORD) reserveSize, (DWORD) initialRequestSize, DBG_ADDR(pInfo->m_pDomain), DBG_ADDR(loAddr), DBG_ADDR(hiAddr)
                                ));

    NewHolder<ExplicitControlLoaderHeap> pHeap(
                    new ExplicitControlLoaderHeap((DWORD) reserveSize,
                                                  (DWORD) PAGE_SIZE,
                                                  pPrivatePCLBytes,
                                                  0,                          // RangeList *pRangeList
                                                  TRUE,                       // BOOL fMakeExecutable
                                                  loAddr,
                                                  hiAddr));

    // this first allocation is critical as it sets up correctly the loader heap info
    HeapList *pHp = (HeapList*)pHeap->AllocMem(sizeof(HeapList));

    NewHolder<LoaderCodeHeap> pCodeHeap(new LoaderCodeHeap(pHeap));
    pHeap.SuppressRelease();
    pHp->pHeap = pCodeHeap;

    pHp->pCacheSpacePtr  = (PBYTE)pHeap->AllocMem(cacheSize);
    pHp->bCacheSpaceSize = (pHp->pCacheSpacePtr == NULL) ? 0 : cacheSize;

    size_t heapSize = pHeap->GetReservedBytesFree();
    size_t nibbleMapSize = HEAP2MAPSIZE(ROUND_UP_TO_PAGE(heapSize));

    pHp->startAddress    = (TADDR)pHp + sizeof(HeapList);

    pHp->startAddress   += pHp->bCacheSpaceSize;

    pHp->endAddress      = pHp->startAddress;
    pHp->maxCodeHeapSize = heapSize;

    _ASSERTE(heapSize >= initialRequestSize);

    // We do not need to memset this memory, since ClrVirtualAlloc() guarantees that the memory is zero.
    // Furthermore, if we avoid writing to it, these pages don't come into our working set

    pHp->changeStart     = NULL;
    pHp->changeEnd       = NULL;
    pHp->bFull           = FALSE;
    pHp->cBlocks         = 0;

    pHp->mapBase         = ROUND_DOWN_TO_PAGE(pHp->startAddress);  // round down to next lower page align
    pHp->pHdrMap         = (DWORD*)(void*)pJitMetaHeap->AllocMem(nibbleMapSize);

    LOG((LF_JIT, LL_INFO100,
         "Created new CodeHeap(" FMT_ADDR ".." FMT_ADDR ")\n",
         DBG_ADDR(pHp->startAddress), DBG_ADDR(pHp->startAddress+pHp->maxCodeHeapSize)
         ));


    pCodeHeap.SuppressRelease();
    RETURN pHp;
}

void CodeHeapRequestInfo::Init()
{ 
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION((m_hiAddr == 0) ||
                     ((m_loAddr < m_hiAddr) &&
                      ((m_loAddr + m_requestSize) < m_hiAddr)));
    } CONTRACTL_END;

    if (m_pDomain == NULL)
        m_pDomain = m_pMD->GetDomain();
    m_isDynamicDomain = m_pMD->IsLCGMethod();
}

HeapList* EEJitManager::NewCodeHeap(CodeHeapRequestInfo *pInfo, DomainCodeHeapList *pADHeapList)
{
    CONTRACT(HeapList *) {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(m_CodeHeapCritSec.OwnedByCurrentThread());
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("CodeHeap");

    size_t initialRequestSize = pInfo->getRequestSize();
    size_t reserveSize;
    size_t cacheSize = 0;
    size_t minReserveSize = VIRTUAL_ALLOC_RESERVE_GRANULARITY; //     ( 64 KB)           


    // SETUP_NEW_BLOCK reserves the first sizeof(LoaderHeapBlock) bytes for LoaderHeapBlock.
    // In other word, the first m_pAllocPtr starts at sizeof(LoaderHeapBlock) bytes 
    // after the allocated memory. Therefore, we need to take it into account.
    size_t requestAndHeadersSize = sizeof(LoaderHeapBlock) + sizeof(HeapList) + initialRequestSize;

    // This loop will typically execute only once, it will execute twice when 
    // we have an initialRequestSize that is close to a V_A_R_GRANULARITY
    do {
        reserveSize = requestAndHeadersSize + cacheSize;
        if (reserveSize < minReserveSize)
            reserveSize = minReserveSize;
        reserveSize = ALIGN_UP(reserveSize, VIRTUAL_ALLOC_RESERVE_GRANULARITY);

        cacheSize = GetCodeHeapCacheSize(reserveSize);

    } while (reserveSize - cacheSize < requestAndHeadersSize);

    pInfo->setReserveSize(reserveSize);
    pInfo->setCacheSize(cacheSize);

    HeapList *pHp = NULL;

    if (pInfo->IsDynamicDomain())
    {
        //
        // Initialize m_loAddr and m_hiAddr appropriately to the 
        // memory range that should be used for executable code
        //
        if ((pInfo->m_loAddr == 0) && (pInfo->m_hiAddr == 0))
        {
            InitArgsForAllocInExecutableRange(pInfo->m_loAddr,
                                              pInfo->m_hiAddr);
        }
        
        pHp = HostCodeHeap::CreateCodeHeap(pInfo, this);
    }
    else
    {
        LoaderHeap *pJitMetaHeap = pADHeapList->m_pDomain->GetLowFrequencyHeap();
        pHp = LoaderCodeHeap::CreateCodeHeap(pInfo, pJitMetaHeap);
    }

    _ASSERTE (pHp != NULL);
    _ASSERTE (pHp->maxCodeHeapSize >= initialRequestSize);

    pHp->SetVolatile_hpNext(GetVolatile_pCodeHeap());

    EX_TRY
    {        
        ExecutionManager::AddCodeRange((TADDR) pHp, (TADDR)&((BYTE*)pHp->startAddress)[pHp->maxCodeHeapSize],this, NULL);
    }
    EX_CATCH
    {
        // If we failed to alloc memory in ExecutionManager::AddCodeRange()
        // then we will delete the LoaderHeap that we allocated

        // pHp is allocated in pHeap, so only need to delete the LoaderHeap itself
        delete pHp->pHeap;

        pHp = NULL;
    }
    EX_END_CATCH(SwallowAllExceptions)

    if (pHp == NULL)
    {
        ThrowOutOfMemory();
    }

    // Since the virtualalloc'ed memory is MEM_RESERVED in the LoaderHeap at a page boundary the
    // following assertion shold be true. If you added some structs between the start of the LoaderHeap and the startAddress
    // then update this ASSERTEand the following if condition appropriately.
    _ASSERTE(((size_t)(pHp->startAddress -  (pHp->pHeap->GetReservedPrivateData() + pHp->bCacheSpaceSize)) & PAGE_MASK) == 0);

    // Insure that the LoaderHeap's start address is 64 KB aligned.
    // The MDTOKEN_CACHE only works if this is true.
    _ASSERTE(pHp->pHeap->GetHeapStartAddress() != NULL);
    _ASSERTE(((size_t)pHp->pHeap->GetHeapStartAddress() & CACHE_BLOCK_MASK) == 0);

    AddRangeToJitHeapCache (pHp->startAddress, pHp->startAddress+pHp->maxCodeHeapSize, pHp);

    SetVolatile_pCodeHeap(pHp);

#ifdef _DEBUG
    DebugCheckJitHeapCacheValidity ();
#endif // _DEBUG

    HeapList **ppHeapList = pADHeapList->m_CodeHeapList.AppendThrowing();
    *ppHeapList = pHp;

    RETURN(pHp);
}

void* EEJitManager::allocCodeRaw(CodeHeapRequestInfo *pInfo,
                                 size_t blockSize, unsigned align, 
                                 HeapList ** ppCodeHeap /* Writeback, Can be null */ )
{
    CONTRACT(void *) {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(m_CodeHeapCritSec.OwnedByCurrentThread());
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    // Initialize the writeback value to NULL if a non-NULL pointer was provided
    if (ppCodeHeap)
        *ppCodeHeap = NULL;

    void *      mem       = NULL;
    HeapList *  pCodeHeap = GetCodeHeap(pInfo);

    if (pCodeHeap != NULL)
    {
        mem = (pCodeHeap->pHeap)->AllocMemory_NoThrow(blockSize, align);
    }

    if (mem == NULL)
    {
        // The current heap couldn't handle our request.
        if (pCodeHeap != NULL)
        {
            pCodeHeap->SetHeapFull();  // Mark it as full
        }

        // Let us create a new heap.

        DomainCodeHeapList *pList = GetCodeHeapList(pInfo->m_pMD, pInfo->m_pDomain);
        _ASSERTE(pList);

        pInfo->setRequestSize(blockSize+align);
        pCodeHeap = NewCodeHeap(pInfo, pList);
        _ASSERTE(pCodeHeap);

        mem = (void *)(pCodeHeap->pHeap)->AllocMemory(blockSize, align);
        _ASSERTE(mem);
    }

    // Record the pCodeHeap value into ppCodeHeap, if a non-NULL pointer was provided
    if (ppCodeHeap)
        *ppCodeHeap = pCodeHeap;

    _ASSERTE((TADDR)mem >= pCodeHeap->startAddress);

    if (((TADDR) mem)+blockSize > (TADDR)pCodeHeap->endAddress)
    {
        // Update the CodeHeap endAddress
        pCodeHeap->endAddress = (TADDR)mem+blockSize;
    }

    RETURN(mem);
}

CodeHeader* EEJitManager::allocCode(MethodDesc* pMD, size_t blockSize, CorJitAllocMemFlag flag)
{
    CONTRACT(CodeHeader *) {
        THROWS;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    unsigned alignment = CODE_SIZE_ALIGN;
    unsigned alignmentFudge = sizeof(CodeHeader) % alignment;
    
    if ((flag & CORJIT_ALLOCMEM_FLG_16BYTE_ALIGN) != 0) alignment = max(alignment, 16);
    
     // when not optimizing for code size, 8-byte align the method entry point, so that the JIT
     // can in turn 8-byte align the loop entry headers.
    if (g_pConfig->GenOptimizeType() != OPT_SIZE) alignment = max(alignment, 8);

    // CodeHeader immediately proceeds the code, so it must be aligned for the 
    // code to be aligned properly.
    _ASSERTE(IS_ALIGNED(sizeof(CodeHeader) + alignmentFudge, alignment));

    CodeHeader * pCodeHdr = NULL;

    // Immediately before the native code we place a CodeHeader struct
    blockSize += sizeof(CodeHeader) + alignmentFudge;   // 8 is needed for 16 byte code alignment on win64

    // Ensure minimal size
    if (blockSize < BYTES_PER_BUCKET)
        blockSize = BYTES_PER_BUCKET;

    CodeHeapRequestInfo requestInfo(pMD);

    // Scope the lock
    {
        // This crst has the CRST_DEBUGGER_THREAD flag, which prevents
        // debuggers from halting this thread after if has incremented changeStart,
        // but before it has incremented changeEnd; otherwise it would loop forever
        // when it does a stack walk and tries to access the nibble map.
        // Hijacking profilers have a similar problem, which is fixed by the forbid
        // suspend thread holder below.  Profilers will refuse a stackwalk in
        // a forbid suspend thread region.
        CrstHolder ch(&m_CodeHeapCritSec);

        HeapList *pCodeHeap = NULL;

#ifdef USE_INDIRECT_CODEHEADER
        // if this is a LCG method then we will be allocating the RealCodeHeader
        // alongside the code (immediately preceeding the CodeHeader) so that the
        // code block can be removed easily by the LCG code heap.
        if (pMD->IsLCGMethod())
        {
            blockSize -= alignmentFudge;
            blockSize += sizeof(RealCodeHeader);
            alignmentFudge = (sizeof(RealCodeHeader)+sizeof(CodeHeader)) % alignment;
            blockSize += alignmentFudge;
        }
#endif  // USE_INDIRECT_CODEHEADER

        size_t mem = (size_t) allocCodeRaw(&requestInfo, blockSize, alignment, &pCodeHeap);

        _ASSERTE(pCodeHeap);

        // bump us up to get proper alignment
        mem += alignmentFudge;

        TADDR pCode;

        if (pMD->IsLCGMethod())
        {
#ifdef USE_INDIRECT_CODEHEADER
            mem += sizeof(RealCodeHeader);
#endif  // USE_INDIRECT_CODEHEADER

            pCode = mem + sizeof(CodeHeader);

            ((DynamicMethodDesc*)pMD)->GetLCGMethodResolver()->m_recordCodePointer = (void*) pCode;
        }
        else
        {
            pCode = mem + sizeof(CodeHeader);
        }

        _ASSERTE(IS_ALIGNED(pCode, alignment));

        JIT_PERF_UPDATE_X86_CODE_SIZE(blockSize);

        BEGIN_HOST_NOCALL_CODE;
        {
            // Prevents a thread suspension AND prevents a hijacking profiler from doing
            // a stackwalk.
            ForbidSuspendThreadHolder suspend;

            CantAllocHolder caHolder;
        

            pCodeHeap->changeStart++;   // mark that we are about to make changes

            _ASSERTE(pCode >= pCodeHeap->mapBase);

            size_t delta = pCode - pCodeHeap->mapBase;
            NibbleMapSet(pCodeHeap->pHdrMap, delta);

            pCodeHeap->cBlocks++;
            pCodeHeap->changeEnd++;

        }
        END_HOST_NOCALL_CODE;

        pCodeHdr = (CodeHeader *)(mem);    

#ifdef USE_INDIRECT_CODEHEADER
        if (pMD->IsLCGMethod())
        {
            pCodeHdr->SetRealCodeHeader((BYTE*)pCodeHdr - sizeof(RealCodeHeader));
        }
        else
        {
            //
            size_t cbExtra;
            BYTE* pRealHeader = (BYTE*)(void*)pMD->GetDomain()->GetLowFrequencyHeap()->AllocAlignedMem(sizeof(RealCodeHeader), sizeof(void*), &cbExtra);
            pCodeHdr->SetRealCodeHeader(pRealHeader);
        }
#endif
    }

    pCodeHdr->SetDebugInfo(NULL);
    pCodeHdr->SetEHInfo(NULL);
    pCodeHdr->SetGCInfo(NULL);
    pCodeHdr->SetMethodDesc(pMD);

    RETURN(pCodeHdr);
}

#endif // #ifndef DACCESS_COMPILE


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef DACCESS_COMPILE
EEJitManager::DomainCodeHeapList *EEJitManager::GetCodeHeapList(MethodDesc *pMD, BaseDomain *pDomain)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(m_CodeHeapCritSec.OwnedByCurrentThread());
    } CONTRACTL_END;

    DomainCodeHeapList *pList = NULL;
    DomainCodeHeapList **ppList = NULL;
    int count = 0;

    // get the appropriate list of heaps
    if (pMD == NULL || pMD->IsLCGMethod())
    {
        ppList = m_DynamicDomainCodeHeaps.Table();
        count = m_DynamicDomainCodeHeaps.Count();
    }
    else
    {
        ppList = m_DomainCodeHeaps.Table();
        count = m_DomainCodeHeaps.Count();
    }

    // look for a DomainCodeHeapList
    for (int i=0; i < count; i++)
    {
        if (ppList[i]->m_pDomain == pDomain ||
            ! ppList[i]->m_pDomain->CanUnload() && ! pDomain->CanUnload())
        {
            pList = ppList[i];
        }
    }
    return pList;
}

HeapList* EEJitManager::GetCodeHeap(CodeHeapRequestInfo *pInfo)
{
    CONTRACT(HeapList *) {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(m_CodeHeapCritSec.OwnedByCurrentThread());
    } CONTRACT_END;

    HeapList *pResult = NULL;

    _ASSERTE(pInfo->m_pDomain != NULL);

    // loop through the m_DomainCodeHeaps to find the AppDomain
    // if not found, then create it
    DomainCodeHeapList *pList = GetCodeHeapList(pInfo->m_pMD, pInfo->m_pDomain);
    if (pList)
    {
        _ASSERTE(pList->m_CodeHeapList.Count() > 0);

        // Set pResult to the largest non-full HeapList
        // that also satisfies the [loAddr..hiAddr] constraint
        for (int i=0; i < pList->m_CodeHeapList.Count(); i++)
        {
            HeapList *pCandidate = NULL;
            HeapList *pCurrent   = pList->m_CodeHeapList[i];
            
            if ((pInfo->m_loAddr == 0) && (pInfo->m_hiAddr == 0))
            {
                if (!pCurrent->IsHeapFull())
                {
                    // We have no constraint so this non empty heap will be able to satistfy our request
                    pCandidate = pCurrent;
                }
            }
            else
            {
                if (!pCurrent->IsHeapFull())
                {
                    // We also check to see if an allocation in this heap would satistfy
                    // the [loAddr..hiAddr] requirement
                    
                    // Calculate the byte range that can ever be returned by
                    // an allocation in this HeapList element
                    //
                    BYTE * firstAddr      = (BYTE *) pCurrent->startAddress;
                    BYTE * lastAddr       = (BYTE *) pCurrent->startAddress + pCurrent->maxCodeHeapSize;

                    _ASSERTE(pCurrent->startAddress <= pCurrent->endAddress);
                    _ASSERTE(firstAddr <= lastAddr);
                    
                    if (pInfo->IsDynamicDomain())
                    {
                        // We check to see if every allocation in this heap
                        // will satistfy the [loAddr..hiAddr] requirement.
                        //
                        // Dynmaic domains use a free list allocator, 
                        // thus we can recieve any address in the range
                        // when calling AllocMemory with a DynamicDomain
                    
                        // [firstaddr .. lastAddr] must be entirely within
                        // [pInfo->m_loAddr .. pInfo->m_hiAddr]
                        //
                        if ((pInfo->m_loAddr <= firstAddr)   &&
                            (lastAddr        <= pInfo->m_hiAddr))
                        {
                            // This heap will always satistfy our constraint
                            pCandidate = pCurrent;
                        }
                    }
                    else // non-DynamicDoman
                    {
                        // Calculate the byte range that would be allocated for the
                        // next allocation request into [loRequestAddr..hiRequestAddr]
                        //
                        BYTE * loRequestAddr  = (BYTE *) pCurrent->endAddress;
                        BYTE * hiRequestAddr  = loRequestAddr + pInfo->getRequestSize();
                        _ASSERTE(loRequestAddr <= hiRequestAddr);

                        // loRequestAddr and hiRequestAddr must be entirely within
                        // [pInfo->m_loAddr .. pInfo->m_hiAddr]
                        // additionally hiRequestAddr must also be less than
                        // or equal to lastAddr
                        //
                        if ((pInfo->m_loAddr <= loRequestAddr)   &&
                            (hiRequestAddr   <= pInfo->m_hiAddr) &&
                            (hiRequestAddr   <= lastAddr))
                        {
                            // This heap will be able to satistfy our constraint
                            pCandidate = pCurrent;
                        }
                    }
                }
            }

            if (pCandidate != NULL)
            {
                if (pResult == NULL)
                {
                    // pCandidate is the first (and possibly only) heap that would satistfy
                    pResult = pCandidate;
                }
                // We use the initial creation size as a discriminator (i.e largest heap)
                else if (pResult->maxCodeHeapSize < pCandidate->maxCodeHeapSize)
                {
                    pResult = pCandidate;
                }
            }
        }
    }
    else
    {
        // not found so need to create the first one
    NewHolder<DomainCodeHeapList> pNewList(new DomainCodeHeapList());
        pNewList->m_pDomain = pInfo->m_pDomain;

        pResult = NewCodeHeap(pInfo, pNewList);

    DomainCodeHeapList **ppList = NULL;
        if (pInfo->IsDynamicDomain())
        ppList = m_DynamicDomainCodeHeaps.AppendThrowing();
    else
        ppList = m_DomainCodeHeaps.AppendThrowing();
    *ppList = pNewList;

    pNewList.SuppressRelease();
}

    RETURN (pResult);
}

LoaderHeap *EEJitManager::GetJitMetaHeap(MethodDesc *pMD)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    BaseDomain *pDomain = pMD->GetDomain();
    _ASSERTE(pDomain);

    return pDomain->GetLowFrequencyHeap();
}

BYTE* EEJitManager::allocGCInfo(CodeHeader* pCodeHeader, DWORD blockSize, size_t * pAllocationSize)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    MethodDesc* pMD = pCodeHeader->GetMethodDesc();
    // sadly for light code gen I need the check in here. We should change GetJitMetaHeap
    if (pMD->IsLCGMethod()) 
    {
        CrstHolder ch(&m_CodeHeapCritSec);
        pCodeHeader->SetGCInfo((BYTE*)(void*)pMD->GetDynamicMethodDesc()->GetResolver()->GetJitMetaHeap()->New(blockSize));
    }
    else
    {
        pCodeHeader->SetGCInfo((BYTE*) (void*)GetJitMetaHeap(pMD)->AllocMem(blockSize));
    }
    _ASSERTE(pCodeHeader->GetGCInfo()); // AllocMem throws if there's not enough memory
    JIT_PERF_UPDATE_X86_CODE_SIZE(blockSize);

    * pAllocationSize = blockSize;  // Store the allocation size so we can backout later.
    
    return(pCodeHeader->GetGCInfo());
}

void* EEJitManager::allocEHInfoRaw(CodeHeader* pCodeHeader, DWORD blockSize, size_t * pAllocationSize)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    MethodDesc* pMD = pCodeHeader->GetMethodDesc();
    void * mem = NULL;

    // sadly for light code gen I need the check in here. We should change GetJitMetaHeap
    if (pMD->IsLCGMethod()) 
    {
        CrstHolder ch(&m_CodeHeapCritSec);
        mem = (void*)pMD->GetDynamicMethodDesc()->GetResolver()->GetJitMetaHeap()->New(blockSize);
    }
    else 
    {
        mem = (void*)GetJitMetaHeap(pMD)->AllocMem(blockSize);
    }
    _ASSERTE(mem);   // AllocMem throws if there's not enough memory

    JIT_PERF_UPDATE_X86_CODE_SIZE(blockSize);

    * pAllocationSize = blockSize; // Store the allocation size so we can backout later.
    
    return(mem);
}


EE_ILEXCEPTION* EEJitManager::allocEHInfo(CodeHeader* pCodeHeader, unsigned numClauses, size_t * pAllocationSize)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    // Note - pCodeHeader->phdrJitEHInfo - sizeof(size_t) contains the number of EH clauses
    DWORD blockSize = EE_ILEXCEPTION::Size(numClauses) + sizeof(size_t);
    BYTE *EHInfo = (BYTE*)allocEHInfoRaw(pCodeHeader, blockSize, pAllocationSize);

    pCodeHeader->SetEHInfo((EE_ILEXCEPTION*) (EHInfo + sizeof(size_t)));
    pCodeHeader->GetEHInfo()->Init(numClauses);
    *((size_t *)EHInfo) = numClauses;
    return(pCodeHeader->GetEHInfo());
}

JumpStubBlockHeader *  EEJitManager::allocJumpStubBlock(MethodDesc* pMD, DWORD numJumps, 
                                                        BYTE * loAddr, BYTE * hiAddr,
                                                        BaseDomain *pDomain)
{
    CONTRACT(JumpStubBlockHeader *) {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(loAddr < hiAddr);
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    if (numJumps < 32)
        numJumps = 32;

    _ASSERTE((sizeof(JumpStubBlockHeader) % CODE_SIZE_ALIGN) == 0);
    _ASSERTE(numJumps < MAX_M_ALLOCATED);

    size_t blockSize = sizeof(JumpStubBlockHeader) + (size_t) numJumps * BACK_TO_BACK_JUMP_ALLOCATE_SIZE;

    size_t alignmentFudge = sizeof(CodeHeader) % CODE_SIZE_ALIGN;

    // Immediately before the native code we place a CodeHeader struct
    size_t extraSize = sizeof(CodeHeader) + alignmentFudge;
#ifdef USE_INDIRECT_CODEHEADER
    // alignmentFudge might change with CH+RCH
    extraSize -= alignmentFudge;
    extraSize += sizeof(RealCodeHeader);
    alignmentFudge = (sizeof(RealCodeHeader) + sizeof(CodeHeader)) % CODE_SIZE_ALIGN;
    extraSize += alignmentFudge;
#endif

    blockSize += extraSize;

    // Ensure minimal size
    if (blockSize < BYTES_PER_BUCKET)
        blockSize = BYTES_PER_BUCKET;

    if (pDomain == NULL)
        pDomain = pMD->GetDomain();

    HeapList *pCodeHeap = NULL;

   CodeHeapRequestInfo    requestInfo(pMD, pDomain, blockSize, loAddr, hiAddr);

    CodeHeader *           pCodeHdr;
    BYTE *                 mem;
    JumpStubBlockHeader *  pBlock;

    // Scope the lock
    {
        // This crst has the CRST_DEBUGGER_THREAD flag, which prevents
        // debuggers from halting this thread after if has incremented changeStart,
        // but before it has incremented changeEnd; otherwise it would loop forever
        // when it does a stack walk and tries to access the nibble map.
        // Hijacking profilers have a similar problem, which is fixed by the forbid
        // suspend thread holder below.  Profilers will refuse a stackwalk in
        // a forbid suspend thread region.
        CrstHolder ch(&m_CodeHeapCritSec);

        mem       = (BYTE *) allocCodeRaw(&requestInfo, blockSize, CODE_SIZE_ALIGN, &pCodeHeap);
#ifdef USE_INDIRECT_CODEHEADER
        // RealCodeHeader comes immediately before the CodeHeader
        pCodeHdr  = (CodeHeader *) (mem + sizeof(RealCodeHeader) + alignmentFudge);
        pCodeHdr->SetRealCodeHeader(mem);
#else   // USE_INDIRECT_CODEHEADER
        pCodeHdr  = (CodeHeader *) (mem + alignmentFudge);
#endif  // USE_INDIRECT_CODEHEADER
        // CodeHeader comes immediately before the code;
        pBlock    = (JumpStubBlockHeader *)(pCodeHdr + 1);

        _ASSERTE(IS_ALIGNED(pBlock, CODE_SIZE_ALIGN));
    
        JIT_PERF_UPDATE_X86_CODE_SIZE(blockSize);
    
        {
            // Prevents a thread suspension AND prevents a hijacking profiler from doing
            // a stackwalk.
            ForbidSuspendThreadHolder suspend;
            BEGIN_HOST_NOCALL_CODE;
            {
            CantAllocHolder caHolder;

            pCodeHeap->changeStart++;   // mark that we are about to make changes
        
            _ASSERTE((TADDR)pCodeHdr >= pCodeHeap->mapBase);
            size_t delta = (TADDR)pCodeHdr + sizeof(CodeHeader) - pCodeHeap->mapBase;
            NibbleMapSet(pCodeHeap->pHdrMap, delta);
    
            pCodeHeap->cBlocks++;
            pCodeHeap->changeEnd++;
            }
            END_HOST_NOCALL_CODE;
        }
    }

    pBlock->m_next      = NULL;
    pBlock->m_used      = 0;
    pBlock->m_allocated = numJumps;
    pBlock->m_domain    = pDomain;
    pBlock->m_zero      = 0;

    BYTE *  start = mem;
    BYTE *  end   = start + blockSize;

    JumpStubStubManager::g_pManager->GetRangeList()->AddRange(start, end, this);

    LOG((LF_JIT, LL_INFO1000, "Allocated new JumpStubBlockHeader for %d stubs at" FMT_ADDR " in domain" FMT_ADDR "\n",
         numJumps, DBG_ADDR(pBlock) , DBG_ADDR(pDomain) ));

    RETURN(pBlock);
}

#endif //!DACCESS_COMPILE

// creates an enumeration and returns the number of EH clauses
unsigned EEJitManager::InitializeEHEnumeration(METHODTOKEN MethodToken, EH_CLAUSE_ENUMERATOR* pEnumState)
{
    LEAF_CONTRACT;
    *pEnumState = 1;     // since the EH info is not compressed, the clause number is used to do the enumeration
    EE_ILEXCEPTION *EHInfo = PTR_CodeHeader((CodeHeader*)MethodToken)->GetEHInfo();
    if (!EHInfo)
        return 0;
    return *(PTR_unsigned(PTR_HOST_TO_TADDR(EHInfo) - sizeof(size_t)));
}

EE_ILEXCEPTION_CLAUSE*  EEJitManager::GetNextEHClause(METHODTOKEN MethodToken,
                              //unsigned clauseNumber,
                              EH_CLAUSE_ENUMERATOR* pEnumState,
                              EE_ILEXCEPTION_CLAUSE* pEHClauseOut)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CodeHeader *pHeader = PTR_CodeHeader((CodeHeader*)MethodToken);

    EE_ILEXCEPTION *pExceptions = pHeader->GetEHInfo();

    (*pEnumState)++;

    return pExceptions->EHClause((unsigned) *pEnumState - 2);
}

#ifndef DACCESS_COMPILE
TypeHandle EEJitManager::ResolveEHClause(METHODTOKEN MethodToken,
                                         //unsigned clauseNumber,
                                         EH_CLAUSE_ENUMERATOR* pEnumState,
                                         EE_ILEXCEPTION_CLAUSE* pEHClauseOut,
                                         CrawlFrame *pCf)
{
    // We don't want to use a runtime contract here since this codepath is used during
    // the processing of a hard SO. Contracts use a significant amount of stack
    // which we can't afford for those cases.
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    EE_ILEXCEPTION* pExceptions;
    TypeHandle      typeHnd = TypeHandle();
    CodeHeader *    pCHdr   = (CodeHeader*) MethodToken;
    MethodDesc *    pMD     = JitTokenToMethodDesc(MethodToken);
    Module     *    pModule = pMD->GetModule();

    PREFIX_ASSUME(pModule != NULL);

    pExceptions = pCHdr->GetEHInfo();
    // use -2 because need to go back to previous one, as enum will have already been updated
    _ASSERTE(*pEnumState >= 2);
    EE_ILEXCEPTION_CLAUSE *pClause = pExceptions->EHClause((unsigned) *pEnumState - 2);
    _ASSERTE(IsTypedHandler(pClause));

    {
        mdToken typeTok = mdTokenNil;

        {
            CrstHolder chRead(&m_CodeHeapCritSec);
            if (HasCachedTypeHandle(pClause))
            {
                typeHnd = TypeHandle::FromPtr(pClause->TypeHandle);
            }
            else
            {
                typeTok = pClause->ClassToken;
            }
        }

        if (!typeHnd.IsNull())
        {
            return typeHnd;
        }

        SigTypeContext typeContext(pMD);
        VarKind k = hasNoVars;
        if (TypeFromToken(typeTok) == mdtTypeSpec)
        {
            PCCOR_SIGNATURE pSig;
            ULONG cSig;
            pModule->GetMDImport()->GetTypeSpecFromToken(typeTok, &pSig, &cSig);
            SigPointer psig(pSig);
            k = psig.IsPolyType(&typeContext);
            //
            // Grab the active class and method instantiation.  This exact instantiation is only
            // needed in the corner case of "generic" exception catching in shared
            // generic code.  We don't need the exact instantiation if the token
            // doesn't contain E_T_VAR or E_T_MVAR.
            if ((k & hasSharableVarsMask) != 0)
            {
                TypeHandle *classInst = NULL;
                TypeHandle *methodInst = NULL;
                pCf->GetExactGenericInstantiations(&classInst, &methodInst);
                SigTypeContext::InitTypeContext(pMD,classInst, methodInst,&typeContext);
            }
        }

        typeHnd = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(pModule, typeTok, &typeContext, 
                                                              ClassLoader::ReturnNullIfNotFound);

        // If the type (pModule,typeTok) was not loaded or not
        // restored then the exception object won't have this type, because an
        // object of this type has been allocated.
        if (typeHnd.IsNull())
            return typeHnd;

        CrstHolder chWrite(&m_CodeHeapCritSec);

        // check first as if has already been resolved then token will have been replaced with TypeHandle
        if (!HasCachedTypeHandle(pClause))
        {
            //
            // We can cache any exception specification except:
            //   - If the type contains type variables in generic code,
            //     e.g. catch E<T> where T is a type variable.
            //   - If the type handle load returned NULL
            // We CANNOT cache E<T> in non-shared instantiations of generic code because
            // there is only one EHClause cache for the IL, shared across all instantiations.
            if (((k & hasAnyVarsMask) == 0) && !typeHnd.IsNull())
            {
                pClause->TypeHandle = typeHnd.AsPtr();
                SetHasCachedTypeHandle(pClause);
            }
        }
        else
        {
            // If we raced in here with aother thread and got held up on the lock, then we just need to return the
            // type handle that the other thread put into the clause.
            typeHnd = TypeHandle::FromPtr(pClause->TypeHandle);
        }
    }
    return typeHnd;
}

void EEJitManager::RemoveJitData (METHODTOKEN token, size_t GCinfo_len, size_t EHinfo_len)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CodeHeader* pCHdr = (CodeHeader*) token;

    HeapList *pHp = GetVolatile_pCodeHeap();

    {
        // This crst has the CRST_DEBUGGER_THREAD flag, which prevents
        // debuggers from halting this thread after if has incremented changeStart,
        // but before it has incremented changeEnd; otherwise it would loop forever
        // when it does a stack walk and tries to access the nibble map.
        // Hijacking profilers have a similar problem, which is fixed by the forbid
        // suspend thread holder below.  Profilers will refuse a stackwalk in
        // a forbid suspend thread region.
        CrstHolder ch(&m_CodeHeapCritSec);

        while (pHp && ((pHp->startAddress > (TADDR)pCHdr) ||
                        (pHp->endAddress < (TADDR)pCHdr + sizeof(CodeHeader))))
        {
            pHp = pHp->GetVolatile_hpNext();
        }

        _ASSERTE(pHp && pHp->pHdrMap);
        _ASSERTE(pHp && pHp->cBlocks);

        // Better to just return than AV?
        if (pHp ==  NULL)
            return;

        // After we suspend a thread for GC or abort, we may call EEJitManager::JitCodeToMethodInfo which
        // will loop until changeStart == changeEnd.  If a thread is supended in between, we hang.
        // Also prevents a stackwalk by a hijacking profiler which would see a similar hang when it
        // accesses the EEJitManager.
        ForbidSuspendThreadHolder suspend;
        BEGIN_HOST_NOCALL_CODE;
        {
        CantAllocHolder caHolder;
        
        pHp->changeStart++;

        size_t delta = (TADDR)pCHdr + sizeof(CodeHeader) - pHp->mapBase;
        NibbleMapSet(pHp->pHdrMap, delta, FALSE);

        pHp->cBlocks--;

        pHp->changeEnd++;
        }
        END_HOST_NOCALL_CODE;
        // leave lock
    }

    MethodDesc* pMD = pCHdr->GetMethodDesc();

    // Backout the GCInfo  
    if (GCinfo_len > 0) {
        if (pMD->IsDynamicMethod()) {

             // !!DO NOTHING!!, See comment above.            
        }
        else {
            GetJitMetaHeap(pMD)->BackoutMem(pCHdr->GetGCInfo(), GCinfo_len);
        }
    }
    
    // Backout the EHInfo  
    BYTE *EHInfo = (BYTE *)pCHdr->GetEHInfo();
    if (EHInfo) {
        EHInfo -= sizeof(size_t);

        _ASSERTE(EHinfo_len>0);

        if (pMD->IsDynamicMethod()) {

            // !!DO NOTHING!!, See comment above. 
        }
        else {
            GetJitMetaHeap(pMD)->BackoutMem(EHInfo, EHinfo_len);
        }
    }  

    // 
    //
    //
    return;
}

// appdomain is being unloaded, so delete any data associated with it. We have to do this in two stages.
// On the first stage, we remove the elements from the list. On the second stage, which occurs after a GC
// we know that only threads who were in preemptive mode prior to the GC could possibly still be looking
// at an element that is about to be deleted. All such threads are guarded with a reader count, so if the
// count is 0, we can safely delete, otherwise we must add to the cleanup list to be deleted later. We know
// there can only be one unload at a time, so we can use a single var to hold the unlinked, but not deleted,
// elements.
void EEJitManager::Unload(AppDomain *pDomain)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
     } CONTRACTL_END;

    CrstHolder ch(&m_CodeHeapCritSec);

    DomainCodeHeapList **ppList = m_DomainCodeHeaps.Table();
    int count = m_DomainCodeHeaps.Count();
    for (int i=0; i < count; i++) {
        if (ppList[i]->m_pDomain == pDomain) {
            DomainCodeHeapList *pList = ppList[i];
            m_DomainCodeHeaps.DeleteByIndex(i);

            // pHeapList is allocated in pHeap, so only need to delete the LoaderHeap itself
            count = pList->m_CodeHeapList.Count();
            for (i=0; i < count; i++) {
                HeapList *pHeapList = pList->m_CodeHeapList[i];
                DeleteCodeHeap(pHeapList);
            }

            // this is ok to do delete as anyone accessing the DomainCodeHeapList structure holds the critical section.
            delete pList;

            break;
        }
    }

    ppList = m_DynamicDomainCodeHeaps.Table();
    count = m_DynamicDomainCodeHeaps.Count();
    for (int i=0; i < count; i++) {
        if (ppList[i]->m_pDomain == pDomain) {
            DomainCodeHeapList *pList = ppList[i];
            m_DynamicDomainCodeHeaps.DeleteByIndex(i);

            // pHeapList is allocated in pHeap, so only need to delete the CodeHeap itself
            count = pList->m_CodeHeapList.Count();
            for (i=0; i < count; i++) {
                HeapList *pHeapList = pList->m_CodeHeapList[i];
                DeleteCodeHeap(pHeapList);
            }

            // this is ok to do delete as anyone accessing the DomainCodeHeapList structure holds the critical section.
            delete pList;

            break;
        }
    }

    ResetNextCodeStartAllocHint();
}

EEJitManager::DomainCodeHeapList::DomainCodeHeapList()
{
    LEAF_CONTRACT;
    m_pDomain = NULL;
}

EEJitManager::DomainCodeHeapList::~DomainCodeHeapList()
{
    LEAF_CONTRACT;
}

//static BOOL bJitHeapShutdown = FALSE;

void EEJitManager::RemoveCodeHeapFromDomainList(CodeHeap *pHeap, BaseDomain *pDomain)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(m_CodeHeapCritSec.OwnedByCurrentThread());
    } CONTRACTL_END;

    // get the proper AppDomain list of heaps
    DomainCodeHeapList *pList = GetCodeHeapList(NULL, pDomain);
    // go through the heaps and find and remove pHeap
    int count = pList->m_CodeHeapList.Count();
    for (int i = 0; i < count; i++) {
        HeapList *pHeapList = pList->m_CodeHeapList[i];
        if (pHeapList->pHeap == pHeap) {
            // found the heap to remove. If thsi is the only heap we remove the whole DomainCodeHeapList
            // otherwise we just remove this heap
            if (count == 1) {
                m_DynamicDomainCodeHeaps.Delete(pList);
                delete pList;
            }
            else
                pList->m_CodeHeapList.Delete(i);
        }
    }

}

void ExecutionManager::ReleaseReferenceToHeap(EEJitManager *pJitManager, CodeHeap *pCodeHeap, MethodDesc *pMethod)
{
    WRAPPER_CONTRACT;
    CrstHolder ch(&m_JumpStubCrst);
    pJitManager->ReleaseReferenceToHeap(pCodeHeap, pMethod);
}

void EEJitManager::ReleaseReferenceToHeap(CodeHeap *pCodeHeap, MethodDesc *pMD)
{
    WRAPPER_CONTRACT;
    CrstHolder ch(&m_CodeHeapCritSec);
    pCodeHeap->ReleaseReferenceToHeap(pMD);
}

void ExecutionManager::ReleaseHeap(EEJitManager *pJitManager, CodeHeap *pCodeHeap)
{
    WRAPPER_CONTRACT;
    CrstHolder ch(&m_JumpStubCrst);
    pJitManager->ReleaseHeap(pCodeHeap);
}

void EEJitManager::ReleaseHeap(CodeHeap *pCodeHeap)
{
    WRAPPER_CONTRACT;
    CrstHolder ch(&m_CodeHeapCritSec);
    pCodeHeap->DestroyHeap();
}

void EEJitManager::DeleteCodeHeap(HeapList *pHeapList)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(m_CodeHeapCritSec.OwnedByCurrentThread());
    } CONTRACTL_END;

#ifdef _DEBUG
    DebugCheckJitHeapCacheValidity ();
#endif // _DEBUG

    HeapList *pHp = GetVolatile_pCodeHeap();
    if (pHp == pHeapList)
        SetVolatile_pCodeHeap(pHp->GetVolatile_hpNext());
    else
    {
        HeapList *pHpNext = pHp->GetVolatile_hpNext();
        
        while (pHpNext != pHeapList)
        {
            pHp = pHpNext;
            _ASSERTE(pHp != NULL);  // should always find the HeapList
            pHpNext = pHp->GetVolatile_hpNext();
        }
        pHp->SetVolatile_hpNext(pHeapList->GetVolatile_hpNext());
    }

    ExecutionManager::DeleteCodeRange((TADDR)pHeapList);

    DeleteJitHeapCache (pHeapList);
    m_pJitHeapCacheUnlinkedList = 0; // automatically deleted when the LoaderHeap is deleted.
#ifdef _DEBUG
    DebugCheckJitHeapCacheValidity ();
#endif // _DEBUG

    LOG((LF_JIT, LL_INFO100, "DeleteCodeHeap start" FMT_ADDR "end" FMT_ADDR "\n",
                              (const BYTE*)pHeapList->startAddress, 
                              (const BYTE*)pHeapList->endAddress     ));





    // Take the writer lock to ensure all readers have finished with the pHp pointer,
    // then release it so we delete the memory outside the lock to avoid deadlocks
    // with the heap lock.
    
    WriterLockHolder wlh(this);
    wlh.Release();
    
    // pHeapList is allocated in pHeap, so only need to delete the CodeHeap itself
    delete pHeapList->pHeap;
}

#endif // #ifndef DACCESS_COMPILE

MethodDesc* EEJitManager::JitCodeToMethodDescWrapper(SLOT currentPC, METHODTOKEN* pMethodToken, DWORD* pPCOffset)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    ReaderLockHolder rlh(this);
    MethodDesc* pMethodDesc = NULL;
    JitCodeToMethodInfo(currentPC, 
                        &pMethodDesc, 
                        pMethodToken, 
                        pPCOffset, 
                        ScanNoReaderLock);
    return pMethodDesc;
}


// Given an CodeHeader for an EEJitManager, get the Debugging Information associated with it.
JitDebugInfo * GetDebugInfoSlotForHeader(CodeHeader * pHdr)
{
    LEAF_CONTRACT;
    
    JitDebugInfo *  p = pHdr->GetDebugInfo();
    return p;
}

CodeHeader * EEJitDebugInfoStore::GetCodeHeaderFromJitRequest(const DebugInfoRequest & request)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    
    TADDR address = (TADDR) request.GetStartAddress();
    _ASSERTE(address != NULL);
    
    CodeHeader * pHeader = EEJitManager::GetCodeHeaderFromAddr(address);
    _ASSERTE(pHeader != NULL);    

    _ASSERTE(!pHeader->GetMethodDesc()->IsPreImplemented());
    
    return pHeader;
}

//-----------------------------------------------------------------------------
// Get vars from Jit Store
//-----------------------------------------------------------------------------
HRESULT EEJitDebugInfoStore::GetVars(
        const DebugInfoRequest & request,
        IN FP_IDS_NEW fpNew, IN void * pNewData,
        OUT ULONG32 * pcVars, 
        OUT ICorDebugInfo::NativeVarInfo **  ppVars)
{
    CONTRACTL {
        THROWS;       // on OOM.
        GC_NOTRIGGER; // getting vars shouldn't trigger
    } CONTRACTL_END;
    
    _ASSERTE(ppVars != NULL);
    _ASSERTE(pcVars != NULL);
    _ASSERTE(fpNew != NULL);
    
    *pcVars = 0;
    *ppVars = NULL;

    CodeHeader * pHdr = GetCodeHeaderFromJitRequest(request);
    _ASSERTE(pHdr != NULL);

    JitDebugInfo * pSlot = GetDebugInfoSlotForHeader(pHdr);
    
    // No header created, which means no jit information is available.
    if (pSlot == NULL)
    {
        return E_FAIL;
    }
    
    
    CompressDebugInfo::Vars info;    
    info = pSlot->m_vars;

    // Uncompress. This allocates memory and may throw.
    CompressDebugInfo::RestoreVars(
        fpNew, pNewData, // allocators
        &info,           // input
        pcVars, ppVars); // output

    return S_OK;
}

//-----------------------------------------------------------------------------
// Get boundaries from JIT store.
//-----------------------------------------------------------------------------
HRESULT EEJitDebugInfoStore::GetBoundaries(
        const DebugInfoRequest & request,
        IN FP_IDS_NEW fpNew, IN void * pNewData,
        OUT ULONG32 * pcMap, 
        OUT ICorDebugInfo::OffsetMapping ** ppMap)
{
    CONTRACTL {
        THROWS;       // on OOM.
        GC_NOTRIGGER; // getting vars shouldn't trigger
    } CONTRACTL_END;
    
    _ASSERTE(ppMap != NULL);
    _ASSERTE(pcMap != NULL);
    _ASSERTE(fpNew != NULL);

    *pcMap = 0;
    *ppMap = NULL;
    
    //MethodDesc * md = request.GetMD();
    CodeHeader * pHdr = GetCodeHeaderFromJitRequest(request);
    _ASSERTE(pHdr != NULL);

    JitDebugInfo * pSlot = GetDebugInfoSlotForHeader(pHdr);    
    
    // No header created, which means no jit information is available.
    if (pSlot == NULL)
    {
        return E_FAIL;
    }
    
    CompressDebugInfo::Boundaries info;    

    info = pSlot->m_bounds;


    // Uncompress. This allocates memory and may throw.
    CompressDebugInfo::RestoreBoundaries(
        fpNew, pNewData, // allocators
        &info,           // input
        pcMap, ppMap);   // output

    return S_OK;
}

//-----------------------------------------------------------------------------
// Get a IDebugInfoStore which can read the debugging information.
//-----------------------------------------------------------------------------
void EEJitManager::GetDebugInfoStore(IDebugInfoStore ** ppStore)
{
    CONTRACT_VOID {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACT_END;
    
    _ASSERTE(ppStore != NULL);

    TADDR addrStore = PTR_HOST_MEMBER_TADDR(EEJitManager, this, m_DebugInfoStore);
    IDebugInfoStore * pStore = PTR_IDebugInfoStore(addrStore);
    *ppStore = pStore;
    
    RETURN;
}

#ifndef DACCESS_COMPILE   
// Allocators for Jit's implementation of IDebugInfoStore
BYTE * LoaderHeapNewMD(MethodDesc * pMD, size_t cBytes)
{
    CONTRACTL {
        THROWS;
        WRAPPER(GC_TRIGGERS);
    } CONTRACTL_END;
    
    LoaderHeap * pHeap = pMD->GetDomain()->GetLowFrequencyHeap();
    // Note that void*() casting operator is overloaded and very special here.
    TaggedMemAllocPtr p = pHeap->AllocMem(cBytes);
    void* mem = (void*) p;
    
    return (BYTE*) mem;
}

// Read-write operation which will get the JitDebugInfo from the header,
// and create it lazily if needed.
JitDebugInfo * GetOrCreateInfoSlotForHeader(CodeHeader* pHeader)
{
    CONTRACTL {
        THROWS;
        WRAPPER(GC_TRIGGERS);
    } CONTRACTL_END;
   
    _ASSERTE(pHeader != NULL);
    
    JitDebugInfo * pInfo = pHeader->GetDebugInfo();    
    if (pInfo == NULL)
    {
        // DAC is read-only here and will return NULL if not initialized.
        pInfo = reinterpret_cast<JitDebugInfo*> (LoaderHeapNewMD(pHeader->GetMethodDesc(), sizeof(JitDebugInfo)));
        pHeader->SetDebugInfo(pInfo);
        pInfo->Init();
    }
    return pInfo;    
}

// Used to populate the info store.
void EEJitManager::setVars(CodeHeader * pHeader, CORINFO_METHOD_HANDLE ftn, ULONG32 cVars, ICorDebugInfo::NativeVarInfo *vars)
{
    CONTRACTL {
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
    } CONTRACTL_END;

    _ASSERTE(pHeader != NULL);
    
    if (!ShouldTrackJitInfo(pHeader))
    {
        return;
    }

    if (cVars > 0)
    {
        EX_TRY
        {        
            _ASSERTE((cVars == 0) == (vars == NULL));
            
            StackSBuffer bufferTemp;
            CompressDebugInfo::CompressVars(cVars, vars, &bufferTemp);
            ULONG32 cbSize = bufferTemp.GetSize();

            BYTE* pBuffer = LoaderHeapNewMD(pHeader->GetMethodDesc(), cbSize); // throws
            memcpy(pBuffer, (const void*) bufferTemp, cbSize);

            JitDebugInfo * pSlot = GetOrCreateInfoSlotForHeader(pHeader);        

            CompressDebugInfo::Vars info;

            info.Deserialize((TADDR) pBuffer, cbSize);
            pSlot->m_vars = info;
        }
        EX_CATCH
        {
            // Just ignore exceptions here. The debugger's structures will still be in a consistent state.
        }
        EX_END_CATCH(SwallowAllExceptions)
    }        
}

void EEJitManager::setBoundaries(CodeHeader * pHeader, CORINFO_METHOD_HANDLE ftn, ULONG32 cMap, ICorDebugInfo::OffsetMapping *pMap)
{
    CONTRACTL {
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
    } CONTRACTL_END;

    _ASSERTE(pHeader != NULL);
    
    if (!ShouldTrackJitInfo(pHeader))
    {
        return;
    }

    if (cMap > 0)
    {
        EX_TRY
        {        
            StackSBuffer bufferTemp;
            CompressDebugInfo::CompressBoundaries(cMap, pMap, &bufferTemp);
            ULONG32 cbSize = bufferTemp.GetSize();
            

            BYTE* pBuffer = LoaderHeapNewMD(pHeader->GetMethodDesc(), cbSize); // throws
            memcpy(pBuffer, (const void *) bufferTemp, cbSize);


            CompressDebugInfo::Boundaries info;

            JitDebugInfo * pSlot = GetOrCreateInfoSlotForHeader(pHeader);
            info.Deserialize((TADDR) pBuffer, cbSize);
            pSlot->m_bounds = info;           
        }
        EX_CATCH
        {
            // Just ignore exceptions here. The debugger's structures will still be in a consistent state.
        }
        EX_END_CATCH(SwallowAllExceptions)
    }        
}
#endif

#ifdef DACCESS_COMPILE
void JitDebugInfo::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;    

    DAC_ENUM_DTHIS();

    DacEnumMemoryRegion(PTR_HOST_TO_TADDR(this), sizeof(JitDebugInfo));

    this->m_bounds.EnumMemoryRegions(flags);
    this->m_vars.EnumMemoryRegions(flags);
}


void CodeHeader::EnumMemoryRegions(CLRDataEnumMemoryFlags flags, IJitManager* pJitMan)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;    

    DAC_ENUM_DTHIS();
    
#ifdef USE_INDIRECT_CODEHEADER
    DacEnumMemoryRegion(PTR_HOST_TO_TADDR(this), sizeof(CodeHeader));
    DacEnumMemoryRegion(PTR_HOST_TO_TADDR(this->pRealCodeHeader), sizeof(RealCodeHeader));
#else // USE_INDIRECT_CODEHEADER
    DacEnumMemoryRegion(PTR_HOST_TO_TADDR(this), sizeof(CodeHeader));
#endif // USE_INDIRECT_CODEHEADER

    if (this->GetDebugInfo() != NULL)
    {
        this->GetDebugInfo()->EnumMemoryRegions(flags);
    }

}

//-----------------------------------------------------------------------------
// Enumerate for minidumps.
//-----------------------------------------------------------------------------
void EEJitDebugInfoStore::EnumMemoryRegionsForMethodDebugInfo(CLRDataEnumMemoryFlags flags, MethodDesc * pMD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;    

    DebugInfoRequest request;
    TADDR addrCode = (TADDR) pMD->GetFunctionAddress();
    request.InitFromStartingAddr(pMD, addrCode);

    CodeHeader* pHeader = EEJitDebugInfoStore::GetCodeHeaderFromJitRequest(request);

    pHeader->EnumMemoryRegions(flags, NULL);
}
#endif

void EEJitManager::JitCodeToMethodInfo(
        SLOT currentPC,
        MethodDesc** ppMethodDesc,
        METHODTOKEN* pMethodToken,
        DWORD* pPCOffset,
        ScanFlag scanFlag)
{
    CONTRACT_VOID {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
        POSTCONDITION(( ppMethodDesc == NULL) || 
                      (*ppMethodDesc == NULL) ||
                      (*ppMethodDesc != (MethodDesc*)POISONC && (*ppMethodDesc)->SanityCheck()));
        POSTCONDITION(pMethodToken == NULL || *pMethodToken != (METHODTOKEN)POISONC);
    } CONTRACT_END;

#ifdef _DEBUG
    if (ppMethodDesc)
    {
        *ppMethodDesc = (MethodDesc*)POISONC;
    }
    if (pMethodToken)
    {
        *pMethodToken = (METHODTOKEN)POISONC;
    }
#endif // _DEBUG

    if (scanFlag != IJitManager::ScanNoReaderLock && IJitManager::GetScanFlags() != IJitManager::ScanNoReaderLock)
    {
        MethodDesc * pMethodDesc = JitCodeToMethodDescWrapper(currentPC, 
                                                              pMethodToken, 
                                                              pPCOffset
                                                              );
        if (ppMethodDesc)
        {
            *ppMethodDesc = pMethodDesc;
        }
        RETURN;        
    }

    HeapList *pHp = NULL;
    CodeHeader *pCHdr;
    PBYTE tick;

#ifdef _DEBUG_IMPL
    HeapList *pDebugHp = GetVolatile_pCodeHeap();
    while (pDebugHp != NULL)
    {
        if ( (pDebugHp->startAddress < (TADDR)currentPC) &&
             (pDebugHp->endAddress >= (TADDR)currentPC) )
        {
            break;
        }
        pDebugHp = pDebugHp->GetVolatile_hpNext();
    }
#endif // _DEBUG_IMPL


    //-------------------------------------------------------------------------
    // Search the cache for the HeapList containing "currentPC"

    HashEntry* hashEntry = NULL;

    size_t currHashKey = COMPUTE_HASH_KEY((size_t)currentPC);
    DWORD  index       = GET_CACHE_INDEX(currHashKey);

    LOG((LF_SYNC, LL_INFO10000, "JitCodeToMethodDesc_CacheCompare: %0x\t%0x\n", index, currHashKey));

    hashEntry = m_JitCodeHashTable[index];
    while (hashEntry != NULL)
    {
        if (hashEntry->hashKey == currHashKey)
        {
            pHp = hashEntry->pHp;

            // The first page of the heap has header. The header can contain executable code 
            // that is not jitted code (e.g. exception handling rebounder jump). We need to check 
            // actual boundaries to confirm the cache hit.
            if ( (pHp->startAddress < (TADDR)currentPC) &&
                (pHp->endAddress >= (TADDR)currentPC) )
            {
                LOG((LF_SYNC, LL_INFO1000, "JitCodeToMethodDesc_CacheHit: %0x\t%0x\n", COMPUTE_HASH_KEY((size_t)currentPC), pHp));
                goto foundHeader;
            }
        }
        hashEntry = hashEntry->pNext;
    }
    

    // Reached here imples that we didn't find the range in the cache or the cache is off
    pHp = GetVolatile_pCodeHeap();
    while (pHp != NULL)
    {
        if ( (pHp->startAddress < (TADDR)currentPC) &&
             (pHp->endAddress >= (TADDR)currentPC) )
        {
            break;
        }
        pHp = pHp->GetVolatile_hpNext();
    }

foundHeader:

    // Whatever method we use to get to the heap node, the following should be true
#ifdef _DEBUG_IMPL
    _ASSERTE ((pHp == pDebugHp) && "JitCodeToMethodDesc cache incorrect");
#endif

    if ((pHp == NULL) || ((TADDR)currentPC < pHp->startAddress) ||
        ((TADDR)currentPC > pHp->endAddress))
    {
        if (pMethodToken)
        {
            *pMethodToken = NULL;
        }
        if (ppMethodDesc)
        {
            *ppMethodDesc = NULL;
        }
        RETURN;
    }

    // we now access the nibble-map and are prone to race conditions.
    // Since we are strictly a "reader", we just use a simple counter
    // scheme to detect if something changed while we were accessing
    // the map. In that case we simply try it again.
    while (1)
    {

        tick = pHp->changeEnd;

        size_t codeOffset = FindMethodCode(pHp->pHdrMap, (TADDR)currentPC - pHp->mapBase);
        if (codeOffset == UINT_MAX)
        {
            if (pMethodToken)
            {
                *pMethodToken = NULL;
            }
            if (ppMethodDesc)
            {
                *ppMethodDesc = NULL;
            }
            RETURN;
        }
        pCHdr = PTR_CodeHeader(pHp->mapBase + (codeOffset - sizeof(CodeHeader)));

        // now check if something has changed while we were accessing
        // the map
        if (tick == pHp->changeStart)
        {
            // no changes, we are done
            _ASSERTE((TADDR)currentPC > PTR_HOST_TO_TADDR(pCHdr));
            if (pMethodToken)
            {
                *pMethodToken = (METHODTOKEN) pCHdr;
            }

            if (pPCOffset)
            {
                *pPCOffset = (DWORD)(currentPC - pCHdr->GetCodeStartAddress());
            }

            if (ppMethodDesc)
            {
                *ppMethodDesc = pCHdr->GetMethodDesc();
            }
            RETURN;
        }
        else
        {
            // A writer modified heap while we were reading it.
            // Give the writer a chance to finish what it's doing and
            // then retry. This is potentially bad as we could keep on
            // looping if writers keep on making changes, but since we
            // typically will not have many writers it should be fine.
            //
            // <WARNING!!!>
            // DO NOT replace this __SwitchToThread() with grabbing
            // the m_CodeHeapCritSec to get in sync with the writers.
            // On 64bit platforms we can frequently end up in JitCodeToMethodInfo
            // under the Loader lock and have numerous potential
            // deadlocks with that implementation.
            // </WARNING!!!>
#ifndef DACCESS_COMPILE
            {
            // When hosted, call SwitchToThread through the OS instead of the host
                __DangerousSwitchToThread(0, TRUE);
            }
#else
            // Nothing should be updating the nibble map under DAC!
            UNREACHABLE();
#endif
        }
    }

    _ASSERTE(!"Unreachable");
    RETURN ;
}

#if !defined(DACCESS_COMPILE)


void  EEJitManager::ResumeAtJitEH(CrawlFrame* pCf, EE_ILEXCEPTION_CLAUSE *EHClausePtr, DWORD nestingLevel, Thread *pThread, BOOL unwindStack)
{
    // no explicit contract here because this function doesn't return and destructors wouldn't be executed

    WRAPPER_CONTRACT;

    BYTE* startAddress = JitTokenToStartAddress(pCf->GetMethodToken());
    ::ResumeAtJitEH(pCf,startAddress,EHClausePtr,nestingLevel,pThread, unwindStack);
}

int  EEJitManager::CallJitEHFilter(CrawlFrame* pCf, EE_ILEXCEPTION_CLAUSE *EHClausePtr, DWORD nestingLevel, OBJECTREF thrownObj)
{
    // Exact contract depends on called code, assume the worst.

    // Note: you can't have a dynamic contract in here. We've pushed a nested exception handler before calling
    // this, and that handler has to be the top FS:0 handler when calling managed code, which is what's going to
    // happen very shortly.
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    BYTE* startAddress = JitTokenToStartAddress(pCf->GetMethodToken());
    return ::CallJitEHFilter(pCf,startAddress,EHClausePtr,nestingLevel,thrownObj);
}

void   EEJitManager::CallJitEHFinally(CrawlFrame* pCf, EE_ILEXCEPTION_CLAUSE *EHClausePtr, DWORD nestingLevel)
{
    // Exact contract depends on called code, assume the worst.

    // Note: you can't have a dynamic contract in here. We've pushed a nested exception handler before calling
    // this, and that handler has to be the top FS:0 handler when calling managed code, which is what's going to
    // happen very shortly.
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    BYTE* startAddress = JitTokenToStartAddress(pCf->GetMethodToken());
    ::CallJitEHFinally(pCf,startAddress,EHClausePtr,nestingLevel);
}

void EEJitManager::NibbleMapSet(DWORD * pMap, size_t delta, BOOL bSet /* = TRUE */) 
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        HOST_NOCALLS;
    } CONTRACTL_END;

    size_t pos  = ADDR2POS(delta); 
    DWORD value = bSet?ADDR2OFFS(delta):0;

    DWORD index = (DWORD) (pos >> LOG2_NIBBLES_PER_DWORD);
    DWORD mask  = ~((DWORD) HIGHEST_NIBBLE_MASK >> ((pos & NIBBLES_PER_DWORD_MASK) << LOG2_NIBBLE_SIZE));

//  printf("[Set: pos=%5x, val=%d]\n", pos, value);

    value = value << POS2SHIFTCOUNT(pos);

    // assert that we don't overwrite an existing offset
    // (it's a reset or it is empty)
    _ASSERTE(!value || !((*(pMap+index))& ~mask));

    *(pMap+index) = ((*(pMap+index))&mask)|value;
}

#endif // !DACCESS_COMPILE



// Finds the header corresponding to the code at offset "delta".
// Note that there are different types of headers like CodeHeader, CORCOMPILE_METHOD_HEADER, etc.
// Returns UINT_MAX if there is no header for the given "delta"

size_t EEJitManager::FindMethodCode(PTR_DWORD pMap, TADDR delta)
{
    LEAF_CONTRACT;

    PTR_DWORD pMapStart = pMap;

    DWORD tmp;

    size_t startPos = ADDR2POS(delta);  // align to 32byte buckets
                                        // ( == index into the array of nibbles)
    DWORD  offset   = ADDR2OFFS(delta); // this is the offset inside the bucket + 1

    _ASSERTE(offset == (offset & NIBBLE_MASK));

    pMap += (startPos >> LOG2_NIBBLES_PER_DWORD); // points to the proper DWORD of the map

    // get DWORD and shift down our nibble

    PREFIX_ASSUME(pMap != NULL);
    tmp = (*pMap) >> POS2SHIFTCOUNT(startPos);

    if ((tmp & NIBBLE_MASK) && ((tmp & NIBBLE_MASK) <= offset) )
    {
        return POSOFF2ADDR(startPos, tmp & NIBBLE_MASK);
    }

    // Is there a header in the remainder of the DWORD ?
    tmp = tmp >> NIBBLE_SIZE;

    if (tmp)
    {
        startPos--;
        while (!(tmp & NIBBLE_MASK))
        {
            tmp = tmp >> NIBBLE_SIZE;
            startPos--;
        }
        return POSOFF2ADDR(startPos, tmp & NIBBLE_MASK);
    }

    // We skipped the remainder of the DWORD,
    // so we must set startPos to the highest position of
    // previous DWORD, unless we are already on the first DWORD

    if (startPos < NIBBLES_PER_DWORD)
        return UINT_MAX;

    startPos = ((startPos >> LOG2_NIBBLES_PER_DWORD) << LOG2_NIBBLES_PER_DWORD) - 1;

    // Skip "headerless" DWORDS

    while (pMapStart < pMap && 0 == (tmp = *(--pMap)))
    {
        startPos -= NIBBLES_PER_DWORD;
    }

    // This helps to catch degenerate error cases. This relies on the fact that
    // startPos cannot ever be bigger than MAX_UINT
    if (((INT_PTR)startPos) < 0)
        return UINT_MAX;

    // Find the nibble with the header in the DWORD

    while (startPos && !(tmp & NIBBLE_MASK))
    {
        tmp = tmp >> NIBBLE_SIZE;
        startPos--;
    }

    if (startPos == 0 && tmp == 0)
        return UINT_MAX;

    return POSOFF2ADDR(startPos, tmp & NIBBLE_MASK);
}

#ifdef DACCESS_COMPILE

void EEJitManager::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    IJitManager::EnumMemoryRegions(flags);

    //
    // Save all of the code heaps.
    //

    HeapList* heap;

    for (heap = GetVolatile_pCodeHeap(); heap; heap = heap->GetVolatile_hpNext())
    {
        DacEnumHostDPtrMem(heap);

        if (heap->pHeap.IsValid())
        {
            heap->pHeap->EnumMemoryRegions(flags);
        }

        DacEnumMemoryRegion(heap->startAddress, (ULONG32)
                            (heap->endAddress - heap->startAddress));

        if (heap->pHdrMap.IsValid())
        {
            ULONG32 nibbleMapSize = (ULONG32)
                HEAP2MAPSIZE(ROUND_UP_TO_PAGE(heap->maxCodeHeapSize));
            DacEnumMemoryRegion(PTR_TO_TADDR(heap->pHdrMap), nibbleMapSize);
        }
    }


    //
    // Save the hash table entries.
    //
    for (ULONG i = 0; i < NumItems(m_JitCodeHashTable); i++)
    {
        HashEntry* hashEntry;

        for (hashEntry = m_JitCodeHashTable[i];
             hashEntry;
             hashEntry = hashEntry->pNext)
        {
            DacEnumHostDPtrMem(hashEntry);
        }
    }

}

#endif // #ifdef DACCESS_COMPILE

#ifndef DACCESS_COMPILE

//*******************************************************
// Execution Manager
//*******************************************************

// Init statics
void ExecutionManager::Init()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;


    m_ExecutionManagerCrst.Init("ExecuteManCrst", CrstExecuteManLock);

    m_JumpStubCrst.Init("JumpStubCrst", CrstJumpStubCache, CRST_UNSAFE_ANYMODE);

    m_RangeCrst.Init("ExecuteManRangeCrst", CrstExecuteManRangeLock, CRST_UNSAFE_ANYMODE);


#ifndef FJITONLY
    m_pDefaultCodeMan = new EECodeManager();
#endif

}

#endif // #ifndef DACCESS_COMPILE

IJitManager* ExecutionManager::FindJitManNonZeroWrapper(SLOT currentPC)
{
    WRAPPER_CONTRACT;

    ReaderLockHolder rlh;    

    return FindJitManNonZero(currentPC, IJitManager::ScanNoReaderLock);
}

//**************************************************************************
// Find a jit manager from the current locations of the IP
//
IJitManager*
ExecutionManager::FindJitManNonZero(SLOT currentPC,
                                    IJitManager::ScanFlag scanFlag)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    } CONTRACTL_END;

    if (scanFlag != IJitManager::ScanNoReaderLock && IJitManager::GetScanFlags() != IJitManager::ScanNoReaderLock)
        return FindJitManNonZeroWrapper(currentPC);

    RangeSection *pRS = GetRangeSection(m_CodeRangeList, (TADDR) currentPC);

    if (pRS == NULL)
    {
        return NULL;
    }
    else
    {

        return pRS->pjit;
    }
}

#ifndef DACCESS_COMPILE

//**************************************************************************
// Find a code manager for a particular type of code
// ie. IL, Managed Native or OPT_IL
//
IJitManager* ExecutionManager::FindJitForType(DWORD Flags)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    if (!m_pJitList)
        return NULL;

    IJitManager *walker = m_pJitList;

    while (walker)
    {
        if (walker->IsJitForType(Flags))
            return walker;
        else
            walker = walker->m_next;
    }
    return walker;
}

//**************************************************************************
// Clear the caches for all JITs loaded.
//
void ExecutionManager::ClearCaches( void )
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    if (!m_pJitList)
        return;

    IJitManager *walker = m_pJitList;

    while (walker)
    {
        walker->ClearCache();
        walker = walker->m_next;
    }
    
    return;
}

//**************************************************************************
// Check if caches for any JITs loaded need to be cleaned
//
BOOL ExecutionManager::IsCacheCleanupRequired( void )
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    if (!m_pJitList)
        return FALSE;

    IJitManager *walker = m_pJitList;

    while (walker)
    {
        if( walker->IsCacheCleanupRequired() )
        {
            return TRUE;
        }
        
        walker = walker->m_next;
    }
    
    return FALSE;
}

/*********************************************************************/
// This static method returns the name of the jit dll
//
LPWSTR ExecutionManager::GetJitName()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        SO_INTOLERANT;
    } CONTRACTL_END;

    LPWSTR  pwzJitName;

    // Try to obtain a name for the jit library from the env. variable
    IfFailThrow(g_pConfig->GetConfigString(L"JitName", &pwzJitName));
    
    if (NULL == pwzJitName)
    {
        pwzJitName = MAKEDLLNAME_W(L"mscorjit");
    }
    return pwzJitName;
}

/*********************************************************************/
// This static method on ExecutionManager returns the IJitManager for
//  the appropriate code type based on the Flags parameter
//
// When creating the method entry points we don't need to force the jit dll to be loaded
// But typically asking for a managaed IL IJitCodeManager forces the jit dll to be loaded
// So w have added a bool argument to GetJitForType to tell it that it doesn't need to
// force load the jit dll
// If we call GetJitForType later on a different codepath where we need to load the JIT
// it will load the jit dll at that later time
//
IJitManager* ExecutionManager::GetJitForType(DWORD Flags, bool dontLoadJit /*= false*/)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        SO_INTOLERANT;  // allocates memory
    } CONTRACTL_END;

    // If we have instantiated jit managers then we need to walk the and see if one of them handles this codetype.
    IJitManager* jitMgr = FindJitForType(Flags);
    if (!jitMgr)
    {
        // Take the code manager lock
        CrstPreempHolder ch(&m_ExecutionManagerCrst);

        jitMgr = FindJitForType(Flags);
        if (!jitMgr)
        {
            NewHolder<ICodeManager> codeMgrHolder;
            ICodeManager* codeMgr = NULL;
            BOOL bIsDefault = FALSE;

            switch (Flags)
            {
            case (miManaged | miIL):
                break;


#ifdef FJIT
            // This is the type that is set if we have used the Econo-Jit!!!
            case miManaged_IL_EJIT:
                codeMgrHolder = new Fjit_EETwain();
                codeMgr = codeMgrHolder;
                break;
#endif

            default:
                _ASSERTE(false);
                break;
            }

            // Use the default JIT manager if we do not have one by this point
            if (jitMgr == NULL)
                jitMgr = new EEJitManager();
            jitMgr->SetCodeType(Flags);

#ifndef FJITONLY
            // Use the default code manager if we do not have one by this point
            if (codeMgr == NULL)
            {
                codeMgr = m_pDefaultCodeMan;
                bIsDefault = TRUE;
            }
#endif
            jitMgr->SetCodeManager(codeMgr, bIsDefault);
            codeMgrHolder.SuppressRelease();

            AddJitManager(jitMgr);
        }
    }


    if (!dontLoadJit && !jitMgr->IsJitLoaded())
    {
        // Take the code manager lock
        CrstPreempHolder ch(&m_ExecutionManagerCrst);

        if (!jitMgr->IsJitLoaded())
        {
            LPWSTR  pwzJitName;
            
            switch (Flags)
            {

#ifndef FJITONLY
            case (miManaged | miIL):
                pwzJitName = ExecutionManager::GetJitName();

                if (!jitMgr->LoadJIT(pwzJitName))
                {
                    if (!(m_fFailedToLoad & FAILED_JIT))
                    {
                        EEMessageBoxCatastrophic(IDS_EE_CANNOT_LOAD_JIT, IDS_FATAL_ERROR, pwzJitName);
                        m_fFailedToLoad = m_fFailedToLoad | FAILED_JIT;
                    }
                }
                break;
#endif // FJITONLY

#ifdef FJIT
            // This is the type that is set if we have used the Econo-Jit!!!
            case miManaged_IL_EJIT:
                // Try to obtain a name for the jit library from the env. variable
                IfFailThrow(g_pConfig->GetConfigString(L"JitName", &pwzJitName));

                if (NULL == pwzJitName)
                {
                    pwzJitName = MAKEDLLNAME_W(L"mscorejt");
                }

                if (!(jitMgr->LoadJIT(pwzJitName)))
                {
                    if (!(m_fFailedToLoad & FAILED_EJIT))
                    {
                        m_fFailedToLoad = m_fFailedToLoad | FAILED_EJIT;
                    }
                }
                break;
#endif  // FJIT

            default :
                // If we got here then something odd happened
                // and we need to free the codeMgr and exit
                _ASSERTE(0 && "Unknown impl type");
                break;
            }

        }
    }

    _ASSERTE(jitMgr != NULL);

    return jitMgr;
}

void ExecutionManager::Unload(AppDomain *pDomain)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    // a size of 0 is a signal to Nirvana to flush the entire cache 
    FlushInstructionCache(GetCurrentProcess(),0,0);
    CrstHolder ch(&m_JumpStubCrst);

    /* StackwalkCacheEntry::EIP is an address into code. Since we are
    unloading the code, we need to invalidate the cache. Otherwise,
    its possible that another appdomain might generate code at the very
    same address, and we might incorrectly think that the old
    StackwalkCacheEntry corresponds to it. So flush the cache.
    */
    StackwalkCache::Invalidate(pDomain);
    
    IJitManager *pMan = m_pJitList;
    while (pMan)
    {
        pMan->Unload(pDomain);
        pMan = pMan->m_next;
    }
}

BOOL ExecutionManager::PatchEntryPoint(BYTE* pBuffer, BYTE epSize, BYTE* target, MethodDesc * methodDesc)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    UNREACHABLE();
}

void ExecutionManager::AddJitManager(IJitManager * newjitmgr)
{
    LEAF_CONTRACT;

    // This is the first code manager added.
    if (!m_pJitList)
        m_pJitList = newjitmgr;
    // else walk the list.
    else
    {
        IJitManager *walker = m_pJitList;
        while (walker->m_next)
            walker = walker->m_next;
        walker->m_next = newjitmgr;
    }
}

#endif // #ifndef DACCESS_COMPILE


RangeSection* ExecutionManager::GetRangeSection(RangeSection *pHead, TADDR addr)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    RangeSection *pCurr = pHead;

#ifndef DACCESS_COMPILE
    RangeSection *pLastUsedRS = (pCurr != NULL) ? pCurr->pLastUsed : NULL;

    if ((pLastUsedRS != NULL)              &&
        (addr >= pLastUsedRS->LowAddress)  &&
        (addr <  pLastUsedRS->HighAddress)   )
    {
        return pLastUsedRS;
    }
#endif

    while (pCurr != NULL)
    {
        // See if addr is in [pCurr->LowAddress .. pCurr->HighAddress)
        if (pCurr->LowAddress <= addr)
        {
            // Since we are sorted, once pCurr->HighAddress is less than addr
            // then all subsequence ones will also be lower, so we are done.
            if (addr >= pCurr->HighAddress)
                break;
        
            // addr must be in [pCurr->LowAddress .. pCurr->HighAddress)
            _ASSERTE((pCurr->LowAddress <= addr) && (addr < pCurr->HighAddress));
        
            // Found the matching RangeSection
            
#ifndef DACCESS_COMPILE
            // Cache pCurr as pLastUsed in the head node
            // Unless we are on an MP system with many cpus
            // where this sort of caching actually diminishes scaling during server GC
            // due to many processors writing to a common location
            if (g_SystemInfo.dwNumberOfProcessors < 4 || !GCHeap::IsServerHeap() || !GCHeap::IsGCInProgress())
                pHead->pLastUsed = pCurr;
#endif
            
            return pCurr;
        }
        pCurr = pCurr->pnext;
    }

    return NULL;
}

RangeSection* ExecutionManager::GetRangeSectionAndPrev(RangeSection *pHead, TADDR addr, RangeSection** ppPrev)
{
    WRAPPER_CONTRACT;

    RangeSection *pCurr;
    RangeSection *pPrev;
    RangeSection *result = NULL;

    for (pPrev = NULL,  pCurr = pHead; 
         pCurr != NULL; 
         pPrev = pCurr, pCurr = pCurr->pnext)
    {
        // See if addr is in [pCurr->LowAddress .. pCurr->HighAddress)
        if (pCurr->LowAddress > addr)
            continue;

        if (addr >= pCurr->HighAddress)
            break;
        
        // addr must be in [pCurr->LowAddress .. pCurr->HighAddress)
        _ASSERTE((pCurr->LowAddress <= addr) && (addr < pCurr->HighAddress));
    
        // Found the matching RangeSection
        result = pCurr;

        // Write back pPrev to ppPrev if it is non-null
        if (ppPrev != NULL)
            *ppPrev = pPrev;

        break;
    }
    
    // If we failed to find a match write NULL to ppPrev if it is non-null
    if ((ppPrev != NULL) && (result == NULL))
    {
        *ppPrev = NULL;
    }

    return result;
}

RangeSection* ExecutionManager::GetRangeSectionForAddress(TADDR startAddress)
{
    WRAPPER_CONTRACT;

    ReaderLockHolder rlh;

    return GetRangeSection(m_CodeRangeList, startAddress);
}

/* static */
Module* ExecutionManager::FindZapModule(TADDR currentData)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    ReaderLockHolder rlh;

    RangeSection *pRS = GetRangeSection(m_DataRangeList, currentData);
    
    return (pRS == NULL) ? (Module*) NULL : pRS->pModule;
}

/* static */
Module* ExecutionManager::FindZapModuleForNativeCode(TADDR currentCode)
{
    WRAPPER_CONTRACT;

    ReaderLockHolder rlh;

    RangeSection *pRS = GetRangeSection(m_CodeRangeList, currentCode);

    return (pRS == NULL) ? (Module*) NULL : pRS->pModule;
}

#ifndef DACCESS_COMPILE

RangeSection * ExecutionManager::AddCodeRange(TADDR          pStartRange,
                                              TADDR          pEndRange,
                                              IJitManager *  pJit,
                                              TADDR          pTable,
                                              DWORD          flags,
                                              RangeSection * pColdRangeSection,
                                              Module*        pModule)
{
    CONTRACT(RangeSection *) {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer((LPVOID)pStartRange));
        PRECONDITION(CheckPointer((LPVOID)pEndRange));
        PRECONDITION(CheckPointer(pJit));
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    if (!(pStartRange && pEndRange && pJit))
        COMPlusThrowArgumentNull(L"AddCodeRange argument");

    RangeSection * result = AddRangeHelper(&m_CodeRangeList,
                                           pStartRange,
                                           pEndRange,
                                           pJit,
                                           pTable,
                                           flags,
                                           pColdRangeSection,
                                           pModule);


    RETURN (result);
}


RangeSection * ExecutionManager::AddDataRange(TADDR          pStartRange,
                                              TADDR          pEndRange,
                                              Module*        pModule)
{
    CONTRACT(RangeSection *) {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer((LPVOID)pStartRange));
        PRECONDITION(CheckPointer((LPVOID)pEndRange));
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    if (!(pStartRange && pEndRange))
       COMPlusThrowArgumentNull(L"AddDataRange argument");

    RETURN(AddRangeHelper(&m_DataRangeList,
                          pStartRange,
                          pEndRange,
                          NULL,
                          NULL,
                          NULL,
                          NULL,
                          pModule));
}

RangeSection * ExecutionManager::AddRangeHelper(RangeSection** ppRangeList,
                                                TADDR          pStartRange,
                                                TADDR          pEndRange,
                                                IJitManager *  pJit,
                                                TADDR          pTable,
                                                DWORD          flags,
                                                RangeSection * pColdRangeSection,
                                                Module*        pModule)
{
    CONTRACT(RangeSection *) {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(ppRangeList));
        PRECONDITION(CheckPointer((LPVOID)pStartRange));
        PRECONDITION(CheckPointer((LPVOID)pEndRange));
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    ReaderLockHolder rlh;

    RangeSection *pnewrange = new RangeSection;

    _ASSERTE(pEndRange > pStartRange);

    pnewrange->LowAddress  = pStartRange;
    pnewrange->HighAddress = pEndRange;
    pnewrange->pjit        = pJit;
    pnewrange->ptable      = pTable;
    pnewrange->pnext       = NULL;
    pnewrange->flags       = flags;
    pnewrange->pcold       = pColdRangeSection;
    pnewrange->pModule     = pModule;
    pnewrange->pLastUsed   = NULL;

    {
        CrstHolder ch(&m_RangeCrst); // Acquire the Crst before linking in a new RangeList

        if (*ppRangeList != NULL)
        {
            RangeSection * current  = *ppRangeList;
            RangeSection * previous = NULL;
            
            while (true)
            {
                // Sort addresses top down so that more recently created ranges
                // will populate the top of the list
                if (pnewrange->LowAddress > current->LowAddress)
                {
                    if (previous == NULL) // insert new head
                    {
                        pnewrange->pnext = *ppRangeList;
                        *ppRangeList = pnewrange;
                    }
                    else // insert in the middle
                    {
                        pnewrange->pnext = current;
                        previous->pnext  = pnewrange;
                    }
                    // Asserts if ranges are overlapping
                    _ASSERTE(pnewrange->LowAddress > pnewrange->pnext->HighAddress);
                    break;
                }
                else if (current->pnext == NULL) // insert at end of list
                {
                    current->pnext = pnewrange;
                    break;
                }
                else  // Continue walking the RangeSection list
                {
                    previous = current;
                    current  = current->pnext;
                }
            }
        }
        else
        {
            *ppRangeList = pnewrange;
        }
    }

    RETURN(pnewrange);
}


// Deletes a single range starting at pStartRange
void ExecutionManager::DeleteCodeRange(TADDR pStartRange)
{
    WRAPPER_CONTRACT;
    DeleteRangeHelper(&m_CodeRangeList, pStartRange);
}

// Deletes a single range starting at pStartRange
void ExecutionManager::DeleteDataRange(TADDR pStartRange)
{
    WRAPPER_CONTRACT;
    DeleteRangeHelper(&m_DataRangeList, pStartRange);
}

// Deletes a single range starting at pStartRange
void ExecutionManager::DeleteRangeHelper(RangeSection** ppRangeList,
                                         TADDR pStartRange)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    WriterLockHolder wlh;        // Acquire the WriteLock and prevent any readers from walking the RangeList
    CrstHolder ch(&m_RangeCrst); // Acquire the Crst before unlinking a RangeList
    
    RangeSection *pCurr = NULL;
    RangeSection *pPrev = NULL;

    pCurr = GetRangeSectionAndPrev(*ppRangeList, pStartRange, &pPrev);
    
    // pCurr points at the Range that needs to be unlinked from the RangeList
    if (pCurr != NULL)
    {

        // If pPrev is NULL the the head of this list is to be deleted
        if (pPrev == NULL)
        {
            *ppRangeList = pCurr->pnext;          
        }
        else
        {
            _ASSERT(pPrev->pnext == pCurr);
        
            pPrev->pnext = pCurr->pnext;
        }

        // Clear the cache pLastUsed in the head node (if any)
        if (*ppRangeList)
        {
            (*ppRangeList)->pLastUsed = NULL;
        }

        delete pCurr;
    }
}

#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void ExecutionManager::EnumRangeList(RangeSection* list,
                                     CLRDataEnumMemoryFlags flags)
{
    while (list != NULL) 
    {
        DacEnumMemoryRegion(PTR_HOST_TO_TADDR(list), sizeof(*list));

        if (list->pjit.IsValid())
        {
            list->pjit->EnumMemoryRegions(flags);
        }
        if (list->pModule.IsValid())
        {
            list->pModule->EnumMemoryRegions(flags, true);
        }

        list = list->pnext;
    }
}

void ExecutionManager::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    ReaderLockHolder rlh;

    //
    // Report the global data portions.
    //

    m_CodeRangeList.EnumMem();
    m_DataRangeList.EnumMem();
#ifndef FJITONLY
    m_pDefaultCodeMan.EnumMem();
#endif

    //
    // Walk structures and report.
    //

    if (m_CodeRangeList.IsValid())
    {
        EnumRangeList(m_CodeRangeList, flags);
    }
    if (m_DataRangeList.IsValid())
    {
        EnumRangeList(m_DataRangeList, flags);
    }
}
#endif // #ifdef DACCESS_COMPILE

#ifndef DACCESS_COMPILE
void ExecutionManager::preallocatedJumpStubBlock(BYTE * mem, DWORD blockSize, BaseDomain *pDomain)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(((size_t) mem % CODE_SIZE_ALIGN) == 0);
    _ASSERTE((sizeof(JumpStubBlockHeader) % CODE_SIZE_ALIGN) == 0);

    DWORD                  numJumps = (blockSize - sizeof(JumpStubBlockHeader)) / BACK_TO_BACK_JUMP_ALLOCATE_SIZE;
    JumpStubBlockHeader *  pBlock   = (JumpStubBlockHeader *)(mem);

    _ASSERTE(numJumps < MAX_M_ALLOCATED);

    {
        CrstHolder ch(&m_JumpStubCrst);

        pBlock->m_next      = m_jumpStubBlock;
        pBlock->m_used      = 0;
        pBlock->m_allocated = numJumps;
        pBlock->m_domain    = pDomain;
        pBlock->m_zero      = 0;

        m_jumpStubBlock     = pBlock;
        
        // We don't really use the ID field of the range list, so just give it anything that's not NULL.
        // (In the other case we give it a pointer to the EEJitManager.)
        JumpStubStubManager::g_pManager->GetRangeList()->AddRange(mem, mem + blockSize, (LPVOID)mem);
    }

    LOG((LF_ZAP, LL_INFO100, "preallocated JumpStubBlockHeader for %d stubs at" FMT_ADDR "\n",
         numJumps, DBG_ADDR(pBlock) ));
}

void ExecutionManager::unlinkPreallocatedJumpStubBlock(BYTE * mem, DWORD blockSize, BaseDomain *pDomain)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(((size_t) mem % CODE_SIZE_ALIGN) == 0);
    _ASSERTE((sizeof(JumpStubBlockHeader) % CODE_SIZE_ALIGN) == 0);

    JumpStubBlockHeader *  pBlockToUnlink;
    pBlockToUnlink = (JumpStubBlockHeader *)(mem);
    _ASSERTE(pBlockToUnlink->m_domain == pDomain);

    {
        CrstHolder ch(&m_JumpStubCrst);

        TADDR startAddr = (TADDR) mem;
        TADDR endAddr   = (TADDR) (mem + blockSize);

        deleteJumpStubBlockHelper(startAddr, endAddr);
    }

    LOG((LF_ZAP, LL_INFO100, "unlink the preallocated JumpStubBlockHeader at" FMT_ADDR "\n",
         DBG_ADDR(pBlockToUnlink) ));
}

void ExecutionManager::deleteJumpStubBlock (HeapList *pHp)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(m_JumpStubCrst.OwnedByCurrentThread());
    } CONTRACTL_END;

    TADDR startAddr = pHp->startAddress;
    TADDR endAddr   = pHp->endAddress;

    ExecutionManager::deleteJumpStubBlockHelper(startAddr, endAddr);
}

void ExecutionManager::deleteJumpStubBlockHelper(TADDR startAddr, TADDR endAddr)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(m_JumpStubCrst.OwnedByCurrentThread());
    } CONTRACTL_END;

    JumpStubBlockHeader *prevHeader = NULL;
    JumpStubBlockHeader *currHeader = m_jumpStubBlock;

    while (currHeader != NULL)
    {
        if (((TADDR)currHeader >= startAddr) && ((TADDR)currHeader < endAddr))
        {
            if (prevHeader == NULL)
            {
                // Remove currHeader from the head of this list
                m_jumpStubBlock = currHeader->m_next;
            }
            else
            {
                // Make preHeader skip over the currHeader
                prevHeader->m_next = currHeader->m_next;
            }

            //
            // Now we need to remove the matching entries
            //  in the JumpStubTable hashtable
            //
            for (unsigned i=0; i < currHeader->m_used; i++)
            {
                BYTE *  jumpStub  = (BYTE *) currHeader + sizeof(JumpStubBlockHeader) + ((size_t) i * BACK_TO_BACK_JUMP_ALLOCATE_SIZE);
                BYTE *  target    = decodeBackToBackJump(jumpStub);

                for (JumpStubTable::KeyIterator j = m_jumpStubTable->Begin(target), 
                    end = m_jumpStubTable->End(target); j != end; j++)
                {
                    if (j->m_jumpStub == jumpStub)
                    {
                        m_jumpStubTable->Remove(j);
                        break;
                    }
                }
            }
        }
        else
        {
            // update prevHeader
            prevHeader = currHeader;
        }
        currHeader = currHeader->m_next;
    }

    JumpStubStubManager::g_pManager->GetRangeList()->RemoveRanges(NULL,
                                                              (const BYTE*)startAddr, 
                                                              (const BYTE*)endAddr);
}

JumpStubBlockHeader * ExecutionManager::recoverJumpStubBlockHeader(BYTE * jumpStub)
{
    JumpStubBlockHeader * result = NULL;
    _ASSERTE(!"Unsupported target");

    // Sanity Check the fields of the JumpStubBlockHeader
    _ASSERTE(result->m_zero   == 0);
    _ASSERTE(result->m_domain != NULL);
    _ASSERTE(result->m_allocated < MAX_M_ALLOCATED);
    _ASSERTE(result->m_used <= result->m_allocated);

    return result;
}

BYTE * ExecutionManager::jumpStub(MethodDesc* pMD, BYTE * target,
                                  BYTE * loAddr,   BYTE * hiAddr,
                                  BaseDomain *pDomain)
{
    CONTRACT(BYTE *) {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(pDomain || NULL != pMD->GetDomain());
        PRECONDITION(loAddr < hiAddr);
        // This can't be checked - if a domain is provided, pMD may not be fully functional.
        //PRECONDITION(!pDomain || pDomain == pMD->GetDomain());
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    BYTE *         jumpStub = NULL;
    IJitManager *  pJitMan  = ExecutionManager::GetJitForType(miManaged|miIL);

    if (pDomain == NULL)
        pDomain = pMD->GetDomain();

    {
        CrstHolder ch(&m_JumpStubCrst);

        for (JumpStubTable::KeyIterator i = m_jumpStubTable->Begin(target), 
            end = m_jumpStubTable->End(target); i != end; i++)
        {
            jumpStub = i->m_jumpStub;

                // Is the matching entry with the requested range?
                if ((jumpStub != NULL) && (loAddr <= jumpStub) && (jumpStub <= hiAddr))
                {
                    JumpStubBlockHeader * header = recoverJumpStubBlockHeader(jumpStub);

                    PREFIX_ASSUME(header != NULL);
                    
                    if (header->m_domain == pDomain)
                        goto DONE_RELEASE_LOCK;
                }
        }

        // If we get here we need to create a new jump stub
        // add or change the jump stub table to point at the new one
        jumpStub = getNextJumpStub(pMD, pJitMan, target, loAddr, hiAddr, pDomain);    // this statement can throw

        _ASSERTE((loAddr <= jumpStub) && (jumpStub <= hiAddr));

        JumpStubEntry entry;

        entry.m_target = target;
        entry.m_jumpStub = jumpStub;

        m_jumpStubTable->Add(entry);

        LOG((LF_JIT, LL_INFO10000, "Add JumpStub to" FMT_ADDR "at" FMT_ADDR "\n",
             DBG_ADDR(target), DBG_ADDR(jumpStub) ));

DONE_RELEASE_LOCK:;
    }
    RETURN(jumpStub);
}

BYTE * ExecutionManager::getNextJumpStub(MethodDesc* pMD, IJitManager* pJitMan, BYTE * target,
                                         BYTE * loAddr, BYTE * hiAddr, BaseDomain *pDomain)
{
    CONTRACT(BYTE *) {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(m_JumpStubCrst.OwnedByCurrentThread());
        PRECONDITION(CheckPointer(pJitMan));
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    BYTE *                 jumpStub = NULL;
    JumpStubBlockHeader *  curBlock = m_jumpStubBlock;

    if (pDomain == NULL)
        pDomain = pMD->GetDomain();

    while (curBlock)
    {
        if ((curBlock->m_used < curBlock->m_allocated) && (pDomain == curBlock->m_domain))
        {
            jumpStub = (BYTE *) curBlock + sizeof(JumpStubBlockHeader) + ((size_t) curBlock->m_used * BACK_TO_BACK_JUMP_ALLOCATE_SIZE);

            if ((loAddr <= jumpStub) && (jumpStub <= hiAddr))
            {
                goto DONE;
            }
        }

        curBlock = curBlock->m_next;
    }

    // If we get here then we need to allocate a new JumpStubBlock

    // this can throw an OM exception
    curBlock = pJitMan->allocJumpStubBlock(pMD, 32, loAddr, hiAddr, pDomain);

    jumpStub = (BYTE *) curBlock + sizeof(JumpStubBlockHeader) + ((size_t) curBlock->m_used * BACK_TO_BACK_JUMP_ALLOCATE_SIZE);

    if ((loAddr > jumpStub) || (jumpStub > hiAddr))
    {
        _ASSERTE(!"allocJumpStubBlock block was not in expected range");        
        EEPOLICY_HANDLE_FATAL_ERROR(COR_E_EXECUTIONENGINE);
    }

    curBlock->m_next = m_jumpStubBlock;
    m_jumpStubBlock = curBlock;

DONE:

    _ASSERTE((curBlock->m_used < curBlock->m_allocated));

    curBlock->m_used++;

    emitBackToBackJump(jumpStub, (void*) target);

    RETURN(jumpStub);
}
#endif // #ifndef DACCESS_COMPILE


#ifndef DACCESS_COMPILE

//**************************************************
// Helpers
//**************************************************

inline DWORD MIN (DWORD a, DWORD b)
{
    LEAF_CONTRACT;

    if (a < b)
        return a;
    else
        return b;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

size_t EEJitManager::GetCodeHeapCacheSize (size_t bAllocationRequest)
{
    LEAF_CONTRACT;

    return ((bAllocationRequest/CACHE_BLOCK_SIZE)+1) * sizeof (HashEntry);
}

void EEJitManager::AddRangeToJitHeapCache (TADDR startAddr, TADDR endAddr, HeapList *pHp)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(m_CodeHeapCritSec.OwnedByCurrentThread());
        PRECONDITION (((size_t)(pHp->startAddress -  (pHp->pHeap->GetReservedPrivateData() + pHp->bCacheSpaceSize)) & CACHE_BLOCK_MASK) == 0);
        PRECONDITION (pHp->pHeap->GetHeapStartAddress() && (((size_t)pHp->pHeap->GetHeapStartAddress() & CACHE_BLOCK_MASK) == 0));
    } CONTRACTL_END;

    HashEntry* hashEntry = NULL;

    size_t currHashKey = COMPUTE_HASH_KEY((size_t)startAddr);
    size_t cacheSpaceSizeLeft = pHp->bCacheSpaceSize;
    PBYTE cacheSpacePtr = pHp->pCacheSpacePtr;
    while ((currHashKey < (size_t)endAddr) && (cacheSpaceSizeLeft > 0))
    {
        _ASSERTE ((cacheSpaceSizeLeft % sizeof (HashEntry)) == 0);
        _ASSERTE (cacheSpacePtr && "Cache ptr and size out of sync");

        DWORD index = GET_CACHE_INDEX(currHashKey);

        hashEntry = new (cacheSpacePtr) HashEntry;
        hashEntry->hashKey = currHashKey;
        hashEntry->pHp = pHp;
        hashEntry->pNext = m_JitCodeHashTable[index];
        m_JitCodeHashTable[index] = hashEntry;
        currHashKey += CACHE_BLOCK_SIZE;
        LOG((LF_SYNC, LL_INFO1000, "AddRangeToJitHeapCache: %0x\t%0x\t%0x\n", index, currHashKey, pHp));

        cacheSpacePtr += sizeof (HashEntry);
        cacheSpaceSizeLeft -= sizeof (HashEntry);
    }

    pHp->bCacheSpaceSize = cacheSpaceSizeLeft;
    pHp->pCacheSpacePtr  = cacheSpacePtr;

}

void EEJitManager::DeleteJitHeapCache (HeapList *pHp)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(m_CodeHeapCritSec.OwnedByCurrentThread());
    } CONTRACTL_END;

    // If the following condition is not true then this heap node was not inserted in the cache
    _ASSERTE (pHp->pHeap->GetHeapStartAddress());
    if (((size_t)pHp->pHeap->GetHeapStartAddress() & CACHE_BLOCK_MASK) != 0)
        return;

    TADDR startAddr = pHp->startAddress;
    TADDR endAddr = pHp->startAddress+pHp->maxCodeHeapSize;

    size_t currHashKey = COMPUTE_HASH_KEY((size_t)startAddr);
    while (currHashKey < (size_t)endAddr)
    {
        DWORD index = GET_CACHE_INDEX(currHashKey);

        HashEntry *hashEntry = m_JitCodeHashTable[index];
        _ASSERTE (hashEntry && "JitHeapCache entry not found");
        if (hashEntry && (hashEntry->hashKey == currHashKey))
        {
            m_JitCodeHashTable[index] = hashEntry->pNext;
            hashEntry->pNext = m_pJitHeapCacheUnlinkedList;
            m_pJitHeapCacheUnlinkedList = hashEntry;
        }
        else
        {
            // We are guaranteed to find all the sub heaps in the collision lists.
            _ASSERTE (hashEntry && hashEntry->pNext && "JitHeapCache entry not found");
            while (hashEntry && hashEntry->pNext && (hashEntry->pNext->hashKey != currHashKey))
            {
                hashEntry = hashEntry->pNext;
                _ASSERTE (hashEntry && hashEntry->pNext && "JitHeapCache entry not found");
            }
            if (hashEntry && hashEntry->pNext && (hashEntry->pNext->hashKey == currHashKey))
            {
                HashEntry *ptmpEntry = hashEntry->pNext;
                hashEntry->pNext = hashEntry->pNext->pNext;
                ptmpEntry->pNext = m_pJitHeapCacheUnlinkedList;
                m_pJitHeapCacheUnlinkedList = ptmpEntry;
            }
        }
        currHashKey += CACHE_BLOCK_SIZE;
        LOG((LF_SYNC, LL_INFO1000, "UnlinkJitHeapCache: %0x\t%0x\t%0x\n", index, currHashKey, hashEntry));
    }
}

#ifdef _DEBUG

BOOL EEJitManager::DebugContainedInHeapList (HeapList *pHashEntryHp)
{
    LEAF_CONTRACT;
    HeapList *pHp = GetVolatile_pCodeHeap();
    while (pHp)
    {
        if (pHp == pHashEntryHp)
            return TRUE;
        pHp = pHp->GetVolatile_hpNext();
    }
    return FALSE;
}

void EEJitManager::DebugCheckJitHeapCacheValidity ()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    HeapList *pHp = GetVolatile_pCodeHeap();
    while (pHp)
    {

        TADDR startAddr = pHp->startAddress;
        TADDR endAddr = pHp->startAddress+pHp->maxCodeHeapSize;

        size_t currHashKey = COMPUTE_HASH_KEY((size_t)startAddr);
        while (currHashKey < (size_t)endAddr)
        {
            DWORD index = GET_CACHE_INDEX(currHashKey);

            HashEntry *hashEntry = m_JitCodeHashTable[index];
            _ASSERTE (hashEntry && "JitHeapCache entry not found");
            if (hashEntry && (hashEntry->hashKey == currHashKey))
            {
                // found the entry
            }
            else
            {
                // We are guaranteed to find all the sub heaps in the collision lists.
                _ASSERTE (hashEntry && hashEntry->pNext && "JitHeapCache entry not found");
                while (hashEntry && (hashEntry->hashKey != currHashKey))
                {
                    hashEntry = hashEntry->pNext;
                }
                _ASSERTE (hashEntry && "JitHeapCache entry not found");
            }
            currHashKey += CACHE_BLOCK_SIZE;
        }
        pHp = pHp->GetVolatile_hpNext();
    }

    for (unsigned int i=0; i<HASH_BUCKETS; i++)
    {
        HashEntry *hashEntry = m_JitCodeHashTable[i];
        while (hashEntry)
        {
            if (!DebugContainedInHeapList (hashEntry->pHp))
            {
                _ASSERTE (!"Inconsistent JitHeapCache found");
            }
            hashEntry = hashEntry->pNext;
        }
    }

}
#endif // _DEBUG

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#endif // #ifndef DACCESS_COMPILE

