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
#include "stdafx.h"                     // Precompiled header key.
#include "loaderheap.h"
#if defined( MAXALLOC )
#include "dbgalloc.h"
#endif // MAXALLOC
#include "perfcounters.h"
#include "ex.h"
#include "memoryreport.h"
#include "pedecoder.h"

// Disable the "initialization of static local vars is no thread safe" error
#ifdef _MSC_VER
#pragma warning(disable : 4640)
#endif

#ifndef DACCESS_COMPILE

INDEBUG(DWORD UnlockedLoaderHeap::s_dwNumInstancesOfLoaderHeaps = 0;)

//
// RangeLists are constructed so they can be searched from multiple
// threads without locking.  They do require locking in order to 
// be safely modified, though.
//

RangeList::RangeList()
{
    WRAPPER_CONTRACT;

    InitBlock(&m_starterBlock);
         
    m_firstEmptyBlock = &m_starterBlock;
    m_firstEmptyRange = 0;
}

RangeList::~RangeList()
{
    LEAF_CONTRACT;
    
    RangeListBlock *b = m_starterBlock.next;

    while (b != NULL)
    {
        RangeListBlock *bNext = b->next;
        delete b;
        b = bNext;
    }
}

void RangeList::InitBlock(RangeListBlock *b)
{
    LEAF_CONTRACT;

    Range *r = b->ranges;
    Range *rEnd = r + RANGE_COUNT; 
    while (r < rEnd)
        r++->id = NULL;

    b->next = NULL;
}

BOOL RangeList::AddRangeWorker(const BYTE *start, const BYTE *end, void *id)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return FALSE;);
    }
    CONTRACTL_END

    _ASSERTE(id != NULL);

    RangeListBlock *b = m_firstEmptyBlock;
    Range *r = b->ranges + m_firstEmptyRange;
    Range *rEnd = b->ranges + RANGE_COUNT;

    while (TRUE)
    {
        while (r < rEnd)
        {
            if (r->id == NULL)
            {
                r->start = (TADDR)start;
                r->end = (TADDR)end;
                r->id = (TADDR)id;
                
                r++;

                m_firstEmptyBlock = b;
                m_firstEmptyRange = r - b->ranges;

                return TRUE;
            }
            r++;
        }

        //
        // If there are no more blocks, allocate a 
        // new one.
        //

        if (b->next == NULL)
        {
            RangeListBlock *newBlock = new (nothrow) RangeListBlock;

            if (newBlock == NULL)
            {
                m_firstEmptyBlock = b;
                m_firstEmptyRange = r - b->ranges;
                return FALSE;
            }

            InitBlock(newBlock);

            newBlock->next = NULL;
            b->next = newBlock;
        }

        //
        // Next block
        //

        b = b->next;
        r = b->ranges;
        rEnd = r + RANGE_COUNT;
    }
}

void RangeList::RemoveRangesWorker(void *id, const BYTE* start, const BYTE* end)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    RangeListBlock *b = &m_starterBlock;
    Range *r = b->ranges;
    Range *rEnd = r + RANGE_COUNT;
    
    //
    // Find the first free element, & mark it.
    //

    while (TRUE)
    {
        //
        // Clear entries in this block.
        //

        while (r < rEnd)
        {
            if (r->id != NULL)
            {
                if (start != NULL)
                {
                    _ASSERTE(end != NULL);

                    if (r->start >= (TADDR)start && r->start < (TADDR)end)
                    {
                        CONSISTENCY_CHECK_MSGF(r->end >= (TADDR)start &&
                                               r->end <= (TADDR)end,
                                               ("r: %p start: %p end: %p", r, start, end));
                        r->id = NULL;
                    }
                }
                else if (r->id == (TADDR)id)
                {
                    r->id = NULL;
                }
            }

            r++;
        }

        //
        // If there are no more blocks, we're done.
        //

        if (b->next == NULL)
        {
            m_firstEmptyRange = 0;
            m_firstEmptyBlock = &m_starterBlock;

            return; 
        }

        // 
        // Next block.
        // 

        b = b->next;
        r = b->ranges;
        rEnd = r + RANGE_COUNT;
    }
}

#endif // #ifndef DACCESS_COMPILE

TADDR RangeList::FindIdWithinRangeWorker(TADDR start, TADDR end)
{
    CONTRACT(TADDR)
    {
        INSTANCE_CHECK;
        NOTHROW;
        FORBID_FAULT;
        GC_NOTRIGGER;
        SO_TOLERANT;
        POSTCONDITION(CheckPointer((void*)RETVAL));
    }
    CONTRACT_END

    RangeListBlock* b = &m_starterBlock;
    Range* r = b->ranges;
    Range* rEnd = r + RANGE_COUNT;
    TADDR idCandidate = 0; // want this to be as large as possible

    //
    // Look for a matching element
    //

    while (TRUE)
    {
        while (r < rEnd)
        {
            if (r->id != NULL &&
                r->id > idCandidate &&
                start <= r->start && 
                r->end < end)
                idCandidate = r->id;
            
            r++;
        }

        //
        // If there are no more blocks, we're done.
        //

        if (b->next == NULL)
            RETURN idCandidate;

        // 
        // Next block.
        // 

        b = b->next;
        r = b->ranges;
        rEnd = r + RANGE_COUNT;
    }
}


BOOL RangeList::IsInRangeWorker(TADDR address, TADDR *pID /* = NULL */)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        FORBID_FAULT;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END

    RangeListBlock* b = &m_starterBlock;
    Range* r = b->ranges;
    Range* rEnd = r + RANGE_COUNT;

    //
    // Look for a matching element
    //

    while (TRUE)
    {
        while (r < rEnd)
        {
            if (r->id != NULL &&
                address >= r->start 
                && address < r->end)
            {
                if (pID != NULL)
                {
                    *pID = r->id;
                }
                return TRUE;
            }
            r++;
        }
        
        //
        // If there are no more blocks, we're done.
        //

        if (b->next == NULL)
            return FALSE;

        // 
        // Next block.
        // 

        b = b->next;
        r = b->ranges;
        rEnd = r + RANGE_COUNT;
    }
}

#ifdef DACCESS_COMPILE

void
RangeList::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    // This class is almost always contained in something
    // else so there's no enumeration of 'this'.
    
    RangeListBlock* block = &m_starterBlock;
    block->EnumMemoryRegions(flags);

    while (block->next.IsValid())
    {
        block->next.EnumMem();
        block = block->next;
        
        block->EnumMemoryRegions(flags);
    }
}

void 
RangeList::RangeListBlock::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    Range*          range;
    TADDR           BADFOOD;
    unsigned int    size;
    int             i;

    WIN64_ONLY( BADFOOD = 0xbaadf00dbaadf00d; );
    NOT_WIN64(  BADFOOD = 0xbaadf00d;         );

    for (i=0; i<RANGE_COUNT; i++)
    {
        range = &(this->ranges[i]);
        if (range->id == NULL || range->start == NULL || range->end == NULL || 
            // just looking at the lower 4bytes is good enough on WIN64 
            range->start == BADFOOD || range->end == BADFOOD)
        {
            break;
        }

        size = (unsigned int) (range->end - range->start);
        _ASSERTE( (UINT64)(range->end - range->start) == (UINT64)size );    // ranges should be less than 4gig!

        DacEnumMemoryRegion(range->start, (ULONG32) size);
    }    
}

#endif // #ifdef DACCESS_COMPILE


//=====================================================================================
// In DEBUG builds only, we tag live blocks with the requested size and the type of
// allocation (AllocMem, AllocAlignedMem, AllocateOntoReservedMem). This is strictly
// to validate that those who call Backout* are passing in the right values.
//
// For simplicity, we'll use one LoaderHeapValidationTag structure for all types even
// though not all fields are applicable to all types.
//=====================================================================================
#ifdef _DEBUG
enum AllocationType
{
    kAllocMem = 1,
    kFreedMem = 4,
};

struct LoaderHeapValidationTag
{
    size_t         m_dwRequestedSize;      // What the caller requested (not what was actually allocated)
    AllocationType m_allocationType;       // Which api allocated this block.
    char *         m_szFile;               // Who allocated me
    int            m_lineNum;              // Who allocated me

};
#endif //_DEBUG





//=====================================================================================
// These classes do detailed loaderheap sniffing to help in debugging heap crashes
//=====================================================================================
#ifdef _DEBUG

// This structure logs the results of an Alloc or Free call. They are stored in reverse time order
// with UnlockedLoaderHeap::m_pEventList pointing to the most recent event.
struct LoaderHeapEvent
{
    LoaderHeapEvent *m_pNext;
    AllocationType   m_allocationType;      //Which api was called
    char            *m_szFile;              //Caller Id
    int              m_lineNum;             //Caller Id
    char            *m_szAllocFile;         //(BackoutEvents): Who allocated the block?
    int              m_allocLineNum;        //(BackoutEvents): Who allocated the block?
    void            *m_pMem;                //Starting address of block
    size_t           m_dwRequestedSize;     //Requested size of block
    size_t           m_dwSize;              //Actual size of block (including validation tags, padding, everything)


    void Describe(SString *pSString)
    {
        CONTRACTL
        {
            INSTANCE_CHECK;
            DISABLED(NOTHROW);
            GC_NOTRIGGER;
        }
        CONTRACTL_END

        pSString->AppendASCII("\n");

        {
            StackSString buf;
            if (m_allocationType == kFreedMem)
            {
                buf.Printf("    Freed at:         %s (line %d)\n", m_szFile, m_lineNum);
                buf.Printf("       (block originally allocated at %s (line %d)\n", m_szAllocFile, m_allocLineNum);
            }
            else
            {
                buf.Printf("    Allocated at:     %s (line %d)\n", m_szFile, m_lineNum);
            }
            pSString->Append(buf);
        }

        if (!QuietValidate())
        {
            pSString->AppendASCII("    *** THIS BLOCK HAS BEEN CORRUPTED ***\n");
        }



        {
            StackSString buf;
            buf.Printf("    Type:          ");
            switch (m_allocationType)
            {
                case kAllocMem:
                    buf.AppendASCII("AllocMem()\n");
                    break;
                case kFreedMem:
                    buf.AppendASCII("Free\n");
                    break;
                default:
                    break;
            }
            pSString->Append(buf);
        }


        {
            StackSString buf;
            buf.Printf("    Start of block:       0x%p\n", m_pMem);
            pSString->Append(buf);
        }

        {
            StackSString buf;
            buf.Printf("    End of block:         0x%p\n", ((BYTE*)m_pMem) + m_dwSize - 1);
            pSString->Append(buf);
        }

        {
            StackSString buf;
            buf.Printf("    Requested size:       %lu (0x%lx)\n", (ULONG)m_dwRequestedSize, (ULONG)m_dwRequestedSize);
            pSString->Append(buf);
        }

        {
            StackSString buf;
            buf.Printf("    Actual size:          %lu (0x%lx)\n", (ULONG)m_dwSize, (ULONG)m_dwSize);
            pSString->Append(buf);
        }

        pSString->AppendASCII("\n");
    }



    BOOL QuietValidate();

};


class LoaderHeapSniffer
{
    public:
        static DWORD InitDebugFlags()
        {
            WRAPPER_CONTRACT;

            DWORD dwDebugFlags = 0;
            if (REGUTIL::GetConfigDWORD(L"LoaderHeapCallTracing", 0))
            {
                dwDebugFlags |= UnlockedLoaderHeap::kCallTracing;
            }
            return dwDebugFlags;
        }


        static VOID RecordEvent(UnlockedLoaderHeap *pHeap,
                                AllocationType allocationType,
                                __in char     *szFile,
                                int            lineNum,
                                __in char     *szAllocFile,
                                int            allocLineNum,
                                void          *pMem,
                                size_t         dwRequestedSize,
                                size_t         dwSize
                                );

        static VOID ClearEvents(UnlockedLoaderHeap *pHeap)
        {
            STATIC_CONTRACT_NOTHROW;
            STATIC_CONTRACT_FORBID_FAULT;

            LoaderHeapEvent *pEvent = pHeap->m_pEventList;
            while (pEvent)
            {
                LoaderHeapEvent *pNext = pEvent->m_pNext;
                delete pEvent;
                pEvent = pNext;
            }
            pHeap->m_pEventList = NULL;
        }


        static VOID CompactEvents(UnlockedLoaderHeap *pHeap)
        {
            STATIC_CONTRACT_NOTHROW;
            STATIC_CONTRACT_FORBID_FAULT;

            LoaderHeapEvent **ppEvent = &(pHeap->m_pEventList);
            while (*ppEvent)
            {
                LoaderHeapEvent *pEvent = *ppEvent;
                if (pEvent->m_allocationType != kFreedMem)
                {
                    ppEvent = &(pEvent->m_pNext);
                }
                else
                {
                    LoaderHeapEvent **ppWalk = &(pEvent->m_pNext);
                    BOOL fMatchFound = FALSE;
                    while (*ppWalk && !fMatchFound)
                    {
                        LoaderHeapEvent *pWalk = *ppWalk;
                        if (pWalk->m_allocationType  != kFreedMem &&
                            pWalk->m_pMem            == pEvent->m_pMem &&
                            pWalk->m_dwRequestedSize == pEvent->m_dwRequestedSize)
                        {
                            // Delete matched pairs

                            // Order is important here - updating *ppWalk may change pEvent->m_pNext, and we want
                            // to get the updated value when we unlink pEvent.
                            *ppWalk = pWalk->m_pNext;
                            *ppEvent = pEvent->m_pNext;

                            delete pEvent;
                            delete pWalk;
                            fMatchFound = TRUE;
                        }
                        else
                        {
                            ppWalk = &(pWalk->m_pNext);
                        }
                    }

                    if (!fMatchFound)
                    {
                        ppEvent = &(pEvent->m_pNext);
                    }
                }
            }
        }
        static VOID PrintEvents(UnlockedLoaderHeap *pHeap)
        {
            STATIC_CONTRACT_NOTHROW;
            STATIC_CONTRACT_FORBID_FAULT;

            printf("\n------------- LoaderHeapEvents (in reverse time order!) --------------------");

            LoaderHeapEvent *pEvent = pHeap->m_pEventList;
            while (pEvent)
            {
                printf("\n");
                switch (pEvent->m_allocationType)
                {
                    case kAllocMem:         printf("AllocMem        "); break;
                    case kFreedMem:         printf("BackoutMem      "); break;

                }
                printf(" ptr = 0x%-8p", pEvent->m_pMem);
                printf(" rqsize = 0x%-8x", pEvent->m_dwRequestedSize);
                printf(" actsize = 0x%-8x", pEvent->m_dwSize);
                printf(" (at %s@%d)", pEvent->m_szFile, pEvent->m_lineNum);
                if (pEvent->m_allocationType == kFreedMem)
                {
                    printf(" (original allocation at %s@%d)", pEvent->m_szAllocFile, pEvent->m_allocLineNum);
                }

                pEvent = pEvent->m_pNext;

            }
            printf("\n------------- End of LoaderHeapEvents --------------------------------------");
            printf("\n");

        }


        static VOID PitchSniffer(SString *pSString)
        {
            WRAPPER_CONTRACT;
            pSString->AppendASCII("\n"
                             "\nBecause call-tracing wasn't turned on, we couldn't provide details about who last owned the affected memory block. To get more precise diagnostics,"
                             "\nset the following registry DWORD value:"
                             "\n"
                             "\n    HKLM\\Software\\Microsoft\\.NETFramework\\LoaderHeapCallTracing = 1"
                             "\n"
                             "\nand rerun the scenario that crashed."
                             "\n"
                             "\n");
        }

        static LoaderHeapEvent *FindEvent(UnlockedLoaderHeap *pHeap, void *pAddr)
        {
            LEAF_CONTRACT;

            LoaderHeapEvent *pEvent = pHeap->m_pEventList;
            while (pEvent)
            {
                if (pAddr >= pEvent->m_pMem && pAddr <= ( ((BYTE*)pEvent->m_pMem) + pEvent->m_dwSize - 1))
                {
                    return pEvent;
                }
                pEvent = pEvent->m_pNext;
            }
            return NULL;

        }


        static void ValidateFreeList(UnlockedLoaderHeap *pHeap);

        static void WeGotAFaultNowWhat(UnlockedLoaderHeap *pHeap)
        {
            WRAPPER_CONTRACT;
            ValidateFreeList(pHeap);

            //If none of the above popped up an assert, pop up a generic one.
            _ASSERTE(!("Unexpected AV inside LoaderHeap. The usual reason is that someone overwrote the end of a block or wrote into a freed block.\n"));
                       
        }

};


#endif


#ifdef _DEBUG
#define LOADER_HEAP_BEGIN_TRAP_FAULT BOOL __faulted = FALSE; EX_TRY {
#define LOADER_HEAP_END_TRAP_FAULT   } EX_CATCH {__faulted = TRUE; } EX_END_CATCH(SwallowAllExceptions) if (__faulted) LoaderHeapSniffer::WeGotAFaultNowWhat(pHeap);
#else
#define LOADER_HEAP_BEGIN_TRAP_FAULT 
#define LOADER_HEAP_END_TRAP_FAULT   
#endif


size_t AllocMem_TotalSize(size_t dwRequestedSize, UnlockedLoaderHeap *pHeap);

//=====================================================================================
// This freelist implementation is a first cut and probably needs to be tuned.
// It should be tuned with the following assumptions:
//
//    - Freeing LoaderHeap memory is done primarily for OOM backout. LoaderHeaps
//      weren't designed to be general purpose heaps and shouldn't be used that way.
//
//    - And hence, when memory is freed, expect it to be freed in large clumps and in a
//      LIFO order. Since the LoaderHeap normally hands out memory with sequentially
//      increasing addresses, blocks will typically be freed with sequentially decreasing
//      addresses.
//
// The first cut of the freelist is a single-linked list of free blocks using first-fit.
// Assuming the above alloc-free pattern holds, the list will end up mostly sorted
// in increasing address order. When a block is freed, we'll attempt to coalesce it
// with the first block in the list. We could also choose to be more aggressive about
// sorting and coalescing but this should probably catch most cases in practice.
//=====================================================================================

// When a block is freed, we place this structure on the first bytes of the freed block (Allocations
// are bumped in size if necessary to make sure there's room.)
struct LoaderHeapFreeBlock
{
    public:
        LoaderHeapFreeBlock   *m_pNext;    // Pointer to next block on free list
        size_t                 m_dwSize;   // Total size of this block (including this header)
//! Try not to grow the size of this structure. It places a minimum size on LoaderHeap allocations.

        static void InsertFreeBlock(LoaderHeapFreeBlock **ppHead, void *pMem, size_t dwTotalSize, UnlockedLoaderHeap *pHeap)
        {
            STATIC_CONTRACT_NOTHROW;

            LOADER_HEAP_BEGIN_TRAP_FAULT

            // It's illegal to insert a free block that's smaller than the minimum sized allocation -
            // it may stay stranded on the freelist forever.
#ifdef _DEBUG
            if (!(dwTotalSize >= AllocMem_TotalSize(1, pHeap)))
            {
                LoaderHeapSniffer::ValidateFreeList(pHeap);
                _ASSERTE(dwTotalSize >= AllocMem_TotalSize(1, pHeap));
            }

            if (!(0 == (dwTotalSize & ALLOC_ALIGN_CONSTANT)))
            {
                LoaderHeapSniffer::ValidateFreeList(pHeap);
                _ASSERTE(0 == (dwTotalSize & ALLOC_ALIGN_CONSTANT));
            }
#endif

            INDEBUG(memset(pMem, 0xcc, dwTotalSize);)
            LoaderHeapFreeBlock *pNewBlock = (LoaderHeapFreeBlock*)pMem;
            pNewBlock->m_pNext  = *ppHead;
            pNewBlock->m_dwSize = dwTotalSize;
            *ppHead = pNewBlock;

            MergeBlock(pNewBlock, pHeap);

            MEMORY_REPORT_FREE(pNewBlock, MemoryReport::VIRTUAL_ALLOCATED_COMMIT_INUSE);

            LOADER_HEAP_END_TRAP_FAULT
        }


        static void *AllocFromFreeList(LoaderHeapFreeBlock **ppHead, size_t dwSize, BOOL fRemoveFromFreeList, UnlockedLoaderHeap *pHeap)
        {
            STATIC_CONTRACT_NOTHROW;

            INCONTRACT(_ASSERTE_IMPL(!ARE_FAULTS_FORBIDDEN()));

            void *pResult = NULL;
            LOADER_HEAP_BEGIN_TRAP_FAULT

            LoaderHeapFreeBlock **ppWalk = ppHead;
            while (*ppWalk)
            {
                LoaderHeapFreeBlock *pCur = *ppWalk;
                size_t dwCurSize = pCur->m_dwSize;
                if (dwCurSize == dwSize)
                {
                    pResult = pCur;
                    // Exact match. Hooray!
                    if (fRemoveFromFreeList)
                    {
                        *ppWalk = pCur->m_pNext;
                    }
                    break;
                }
                else if (dwCurSize > dwSize && (dwCurSize - dwSize) >= AllocMem_TotalSize(1, pHeap))
                {
                    // Partial match. Ok...
                    pResult = pCur;
                    if (fRemoveFromFreeList)
                    {
                        *ppWalk = pCur->m_pNext;
                        InsertFreeBlock(ppWalk, ((BYTE*)pCur) + dwSize, dwCurSize - dwSize, pHeap );
                    }
                    break;
                }

                // Either block is too small or splitting the block would leave a remainder that's smaller than
                // the minimum block size. Onto next one.

                ppWalk = &( pCur->m_pNext );
            }

            if (pResult && fRemoveFromFreeList)
            {
                // Callers of loaderheap assume allocated memory is zero-inited so we must preserve this invariant!
                memset(pResult, 0, dwSize);
            }
            LOADER_HEAP_END_TRAP_FAULT
            return pResult;



        }


    private:
        // Try to merge pFreeBlock with its immediate successor. Return TRUE if a merge happened. FALSE if no merge happened. 
        static BOOL MergeBlock(LoaderHeapFreeBlock *pFreeBlock, UnlockedLoaderHeap *pHeap)
        {
            STATIC_CONTRACT_NOTHROW;

            BOOL result = FALSE;

            LOADER_HEAP_BEGIN_TRAP_FAULT

            LoaderHeapFreeBlock *pNextBlock = pFreeBlock->m_pNext;
            size_t               dwSize     = pFreeBlock->m_dwSize;

            if (pNextBlock == NULL || ((BYTE*)pNextBlock) != (((BYTE*)pFreeBlock) + dwSize))
            {
                result = FALSE;
            }
            else
            {
                size_t dwCombinedSize = dwSize + pNextBlock->m_dwSize;
                LoaderHeapFreeBlock *pNextNextBlock = pNextBlock->m_pNext;
                INDEBUG(memset(pFreeBlock, 0xcc, dwCombinedSize);)
                pFreeBlock->m_pNext  = pNextNextBlock;
                pFreeBlock->m_dwSize = dwCombinedSize;

                result = TRUE;
            }

            LOADER_HEAP_END_TRAP_FAULT
            return result;

        }

};




//=====================================================================================
// These helpers encapsulate the actual layout of a block allocated by AllocMem
// and UnlockedAllocMem():
//
// ==> Starting address is always pointer-aligned.
//
//   - x  bytes of user bytes        (where "x" is the actual dwSize passed into AllocMem)
//
//   - y  bytes of "EE" (DEBUG-ONLY) (where "y" == LOADER_HEAP_DEBUG_BOUNDARY (normally 0))
//   - z  bytes of pad  (DEBUG-ONLY) (where "z" is just enough to pointer-align the following byte)
//   - a  bytes of tag  (DEBUG-ONLY) (where "a" is sizeof(LoaderHeapValidationTag)
//
//   - b  bytes of pad               (if total size after all this < sizeof(LoaderHeapFreeBlock), pad enough to make it the size of LoaderHeapFreeBlock)
//   - c  bytes of pad               (where "c" is just enough to pointer-align the following byte)
//
// ==> Following address is always pointer-aligned
//=====================================================================================

// Convert the requested size into the total # of bytes we'll actually allocate (including padding)
inline size_t AllocMem_TotalSize(size_t dwRequestedSize, UnlockedLoaderHeap *pHeap)
{
    LEAF_CONTRACT;

    size_t dwSize = dwRequestedSize;
#ifdef _DEBUG
    dwSize += LOADER_HEAP_DEBUG_BOUNDARY;
    dwSize = ((dwSize + ALLOC_ALIGN_CONSTANT) & (~ALLOC_ALIGN_CONSTANT));

    if (!pHeap->m_fExplicitControl)
    {
        dwSize += sizeof(LoaderHeapValidationTag);
    }
#endif
    if (!pHeap->m_fExplicitControl)
    {
        if (dwSize < sizeof(LoaderHeapFreeBlock))
        {
            dwSize = sizeof(LoaderHeapFreeBlock);
        } 
    }
    dwSize = ((dwSize + ALLOC_ALIGN_CONSTANT) & (~ALLOC_ALIGN_CONSTANT));

    return dwSize;
}


#ifdef _DEBUG
LoaderHeapValidationTag *AllocMem_GetTag(LPVOID pBlock, size_t dwRequestedSize)
{
    LEAF_CONTRACT;

    size_t dwSize = dwRequestedSize;
    dwSize += LOADER_HEAP_DEBUG_BOUNDARY;
    dwSize = ((dwSize + ALLOC_ALIGN_CONSTANT) & (~ALLOC_ALIGN_CONSTANT));
    return (LoaderHeapValidationTag *)( ((BYTE*)pBlock) + dwSize );
}
#endif





//=====================================================================================
// UnlockedLoaderHeap methods
//=====================================================================================

#ifndef DACCESS_COMPILE

UnlockedLoaderHeap::UnlockedLoaderHeap(DWORD dwReserveBlockSize, 
                                       DWORD dwCommitBlockSize, 
                                       size_t *pPrivatePerfCounter_LoaderBytes,
                                       RangeList *pRangeList,
                                       BOOL fMakeExecutable,
                                       const BYTE *pMinAddr,
                                       const BYTE *pMaxAddr)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    m_pCurBlock                  = NULL;
    m_pFirstBlock                = NULL;

    m_dwCommitBlockSize          = max(dwCommitBlockSize, sizeof(LoaderHeapBlockUnused));  // Internal overhead for an allocation 
    m_dwReserveBlockSize         = max(dwReserveBlockSize,  m_dwCommitBlockSize);

    m_pPtrToEndOfCommittedRegion = NULL;
    m_pEndReservedRegion         = NULL;
    m_pAllocPtr                  = NULL;

    m_pRangeList                 = pRangeList;

    if (fMakeExecutable && (pMinAddr == 0) && (pMaxAddr == 0))
    {
        //
        // Initialize m_pMinAddr and m_pMaxAddr appropriately to the 
        // memory range that should be used for executable code
        //
        InitArgsForAllocInExecutableRange(m_pMinAddr, m_pMaxAddr);
    }
    else
    {
        // MinAddr can't be lower than BOT_MEMORY
        // 0 for pMaxAddr means TOP_MEMORY
        m_pMinAddr                   = max((PBYTE)BOT_MEMORY, pMinAddr);
        m_pMaxAddr                   = (pMaxAddr == 0) ? (PBYTE)TOP_MEMORY : pMaxAddr;
    }

    // Round reserve size to VIRTUAL_ALLOC_RESERVE_GRANULARITY size
    m_dwReserveBlockSize         = (DWORD)ALIGN_UP(m_dwReserveBlockSize, VIRTUAL_ALLOC_RESERVE_GRANULARITY);
    // Round commit size to PAGE_SIZE size
    m_dwCommitBlockSize          = (DWORD)ALIGN_UP(m_dwCommitBlockSize, PAGE_SIZE);

    m_dwTotalAlloc               = 0;

#ifdef _DEBUG
    m_dwDebugWastedBytes         = 0;
    s_dwNumInstancesOfLoaderHeaps++;
    m_pEventList                 = NULL;
    m_dwDebugFlags               = LoaderHeapSniffer::InitDebugFlags();
    m_fPermitStubsWithUnwindInfo = FALSE;
    m_fStubUnwindInfoUnregistered= FALSE;
#endif

    m_pPrivatePerfCounter_LoaderBytes = pPrivatePerfCounter_LoaderBytes;

    // Make it executable - there are code fragments in next to all loader heaps
    m_flProtect = PAGE_EXECUTE_READWRITE;

    m_pFirstFreeBlock            = NULL;
}

UnlockedLoaderHeap::UnlockedLoaderHeap(DWORD dwReserveBlockSize,
                                       DWORD dwCommitBlockSize, 
                                       const BYTE* dwReservedRegionAddress,
                                       DWORD dwReservedRegionSize, 
                                       size_t *pPrivatePerfCounter_LoaderBytes,
                                       RangeList *pRangeList,
                                       BOOL fMakeExecutable)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END;
    
    m_pCurBlock                  = NULL;
    m_pFirstBlock                = NULL;

    m_dwReserveBlockSize         = dwReserveBlockSize + sizeof(LoaderHeapBlock); // Internal overhead for an allocation 
    m_dwCommitBlockSize          = dwCommitBlockSize + sizeof(LoaderHeapBlock);  // Internal overhead for an allocation 

    m_pPtrToEndOfCommittedRegion = NULL;
    m_pEndReservedRegion         = NULL;
    m_pAllocPtr                  = NULL;

    m_pRangeList                 = pRangeList;

    //
    // Initialize m_pMinAddr and m_pMaxAddr appropriately to the 
    // memory range that should be used for executable code
    //
    InitArgsForAllocInExecutableRange(m_pMinAddr, m_pMaxAddr);

    // Round to VIRTUAL_ALLOC_RESERVE_GRANULARITY
    m_dwReserveBlockSize         = (DWORD)ALIGN_UP(m_dwReserveBlockSize, VIRTUAL_ALLOC_RESERVE_GRANULARITY);
    m_dwTotalAlloc               = 0;

#ifdef _DEBUG
    m_dwDebugWastedBytes         = 0;
    s_dwNumInstancesOfLoaderHeaps++;
    m_pEventList                 = NULL;
    m_dwDebugFlags               = LoaderHeapSniffer::InitDebugFlags();
    m_fPermitStubsWithUnwindInfo = FALSE;
    m_fStubUnwindInfoUnregistered= FALSE;
#endif

    m_pPrivatePerfCounter_LoaderBytes = pPrivatePerfCounter_LoaderBytes;

    // Make it executable - there are code fragments in next to all loader heaps
    m_flProtect = PAGE_EXECUTE_READWRITE;

    m_pFirstFreeBlock            = NULL;

    if (dwReservedRegionAddress != NULL && dwReservedRegionSize > 0)
    {
        m_reservedBlock.Init((void *)dwReservedRegionAddress, dwReservedRegionSize, FALSE);
    }
}

// ~LoaderHeap is not synchronised (obviously)
UnlockedLoaderHeap::~UnlockedLoaderHeap()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    _ASSERTE(!m_fPermitStubsWithUnwindInfo || m_fStubUnwindInfoUnregistered);

    if (m_pRangeList != NULL)
        m_pRangeList->RemoveRanges((void *) this);

    LoaderHeapBlock *pSearch, *pNext;

    for (pSearch = m_pFirstBlock; pSearch; pSearch = pNext)
    {
        BOOL    fSuccess;
        void *  pVirtualAddress;
        BOOL    fReleaseMemory;

        pVirtualAddress = pSearch->pVirtualAddress;
        fReleaseMemory = pSearch->m_fReleaseMemory;
        pNext = pSearch->pNext;
                
        fSuccess = ClrVirtualFree(pVirtualAddress, pSearch->dwVirtualSize, MEM_DECOMMIT);
        _ASSERTE(fSuccess);

        if (fReleaseMemory)
        {    
            fSuccess = ClrVirtualFree(pVirtualAddress, 0, MEM_RELEASE);
            _ASSERTE(fSuccess);
        }
    }

    if (m_pPrivatePerfCounter_LoaderBytes)
        *m_pPrivatePerfCounter_LoaderBytes = *m_pPrivatePerfCounter_LoaderBytes - (DWORD) m_dwTotalAlloc;

    INDEBUG(s_dwNumInstancesOfLoaderHeaps --;)
}

#endif // #ifndef DACCESS_COMPILE


size_t UnlockedLoaderHeap::GetBytesAvailCommittedRegion()
{
    LEAF_CONTRACT;

    if (m_pAllocPtr < m_pPtrToEndOfCommittedRegion)
        return (size_t)(m_pPtrToEndOfCommittedRegion - m_pAllocPtr);
    else
        return 0;
}

size_t UnlockedLoaderHeap::GetBytesAvailReservedRegion()
{
    LEAF_CONTRACT;

    if (m_pAllocPtr < m_pEndReservedRegion)
        return (size_t)(m_pEndReservedRegion- m_pAllocPtr);
    else
        return 0;
}

#define SETUP_NEW_BLOCK(pData, dwSizeToCommit, dwSizeToReserve)                     \
        m_pPtrToEndOfCommittedRegion = (BYTE *) (pData) + (dwSizeToCommit);         \
        m_pAllocPtr                  = (BYTE *) (pData) + sizeof(LoaderHeapBlock);  \
        m_pEndReservedRegion         = (BYTE *) (pData) + (dwSizeToReserve);


#ifndef DACCESS_COMPILE

BOOL UnlockedLoaderHeap::UnlockedReservePages(size_t dwSizeToCommit, 
                                      const BYTE* dwReservedRegionAddress,
                                      size_t dwReservedRegionSize,
                                      const BYTE* pMinAddr,
                                      const BYTE* pMaxAddr,
                                      BOOL fCanAlloc)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        INJECT_FAULT(return FALSE;);
    }
    CONTRACTL_END;
    
    size_t dwSizeToReserve;

    // Add sizeof(LoaderHeapBlock)
    dwSizeToCommit += sizeof(LoaderHeapBlockUnused);

    // Round to page size again
    dwSizeToCommit = ALIGN_UP(dwSizeToCommit, PAGE_SIZE);

    void *pData = NULL;
    BOOL fReleaseMemory = TRUE;

    if (dwReservedRegionAddress != NULL)
    {
        // We should never come in with pre-reserved memory that does not satisfy the commit requirement.
        CONSISTENCY_CHECK(dwReservedRegionSize >= dwSizeToCommit);
        pData = (void *)dwReservedRegionAddress;
        dwSizeToReserve = dwReservedRegionSize;
        fReleaseMemory = FALSE;
    }
    // We were provided with a reserved memory block at instance creation time, so use it if it's big enough.
    else if (m_reservedBlock.pVirtualAddress != NULL &&
             m_reservedBlock.dwVirtualSize >= dwSizeToCommit)
    {
        // Get the info out of the block.
        pData = m_reservedBlock.pVirtualAddress;
        dwSizeToReserve = m_reservedBlock.dwVirtualSize;
        fReleaseMemory = m_reservedBlock.m_fReleaseMemory;

        // Zero the block so this memory doesn't get used again.
        m_reservedBlock.Init(NULL, 0, FALSE);
    }
    // The caller is asking us to allocate the memory
    else
    {
        // Figure out how much to reserve
        dwSizeToReserve = max(dwSizeToCommit, m_dwReserveBlockSize);

        // The caller can pass in a NULL address but still request a minimum reserve size.
        dwSizeToReserve = max(dwSizeToReserve, dwReservedRegionSize);

        // Round to VIRTUAL_ALLOC_RESERVE_GRANULARITY 
        dwSizeToReserve = ALIGN_UP(dwSizeToReserve, VIRTUAL_ALLOC_RESERVE_GRANULARITY);

        _ASSERTE(dwSizeToCommit <= dwSizeToReserve);    

        //
        // Reserve pages
        //

        const BYTE *pStart = NULL;
        HRESULT hr = ClrVirtualAllocWithinRange(pStart, 
                                                pMinAddr, pMaxAddr,
                                                dwSizeToReserve, 
                                                MEM_RESERVE, PAGE_NOACCESS);
        if (FAILED(hr))
        {
            return FALSE;
        }
        pData = (void *)pStart;
    }

    // When the user passes in the reserved memory, the commit size is 0 and is adjusted to be the sizeof(LoaderHeap). 
    // If for some reason this is not true then we just catch this via an assertion and the dev who changed code
    // would have to add logic here to handle the case when committed mem is more than the reserved mem. One option 
    // could be to leak the users memory and reserve+commit a new block, Another option would be to fail the alloc mem
    // and notify the user to provide more reserved mem.
    _ASSERTE((dwSizeToCommit <= dwSizeToReserve) && "Loaderheap tried to commit more memory than reserved by user");

    if (pData == NULL)
    {
        //_ASSERTE(!"Unable to ClrVirtualAlloc reserve in a loaderheap");
        return FALSE;
    }

    // Don't report this range in the memory report since we will track individual allocations
    MEMORY_REPORT_CLEAR();

    // Commit first set of pages, since it will contain the LoaderHeapBlock
    void *pTemp = ClrVirtualAlloc(pData, dwSizeToCommit, MEM_COMMIT, m_flProtect);
    if (pTemp == NULL)
    {
        //_ASSERTE(!"Unable to ClrVirtualAlloc commit in a loaderheap");

        // Unable to commit - release pages
        if (fReleaseMemory)
            ClrVirtualFree(pData, 0, MEM_RELEASE);

        return FALSE;
    }

    if (m_pPrivatePerfCounter_LoaderBytes)
        *m_pPrivatePerfCounter_LoaderBytes = *m_pPrivatePerfCounter_LoaderBytes + (DWORD) dwSizeToCommit;

    // Record reserved range in range list, if one is specified
    // Do this AFTER the commit - otherwise we'll have bogus ranges included.
    if (m_pRangeList != NULL)
    {
        if (!m_pRangeList->AddRange((const BYTE *) pData, 
                                    ((const BYTE *) pData) + dwSizeToReserve, 
                                    (void *) this))
        {

            if (fReleaseMemory)
                ClrVirtualFree(pData, 0, MEM_RELEASE);

            return FALSE;
        }
    }

    m_dwTotalAlloc += dwSizeToCommit;

    LoaderHeapBlock *pNewBlock;

    pNewBlock = (LoaderHeapBlock *) pData;

    pNewBlock->dwVirtualSize    = dwSizeToReserve;
    pNewBlock->pVirtualAddress  = pData;
    pNewBlock->pNext            = NULL;
    pNewBlock->m_fReleaseMemory = fReleaseMemory;

    LoaderHeapBlock *pCurBlock = m_pCurBlock;

    // Add to linked list
    while (pCurBlock != NULL &&
           pCurBlock->pNext != NULL)
        pCurBlock = pCurBlock->pNext;

    if (pCurBlock != NULL)        
        m_pCurBlock->pNext = pNewBlock;
    else
        m_pFirstBlock = pNewBlock;

    if (!fCanAlloc)
    {
        // If we want to use the memory immediately...
        m_pCurBlock = pNewBlock;

        SETUP_NEW_BLOCK(pData, dwSizeToCommit, dwSizeToReserve);
    }
    else
    {
        // The caller is just interested if we can, actually get the memory.
        // So stash it into the next item in the list & we'll go looking for
        // it later.
        LoaderHeapBlockUnused *pCanAllocBlock = (LoaderHeapBlockUnused *)pNewBlock;
        pCanAllocBlock->cbCommitted = dwSizeToCommit;
        pCanAllocBlock->cbReserved = dwSizeToReserve;
    }
    return TRUE;
}

// Get some more committed pages - either commit some more in the current reserved region, or, if it
// has run out, reserve another set of pages. 
// Returns: FALSE if we can't get any more memory (we can't commit any more, and
//              if bGrowHeap is TRUE, we can't reserve any more)
// TRUE: We can/did get some more memory - check to see if it's sufficient for
//       the caller's needs (see UnlockedAllocMem for example of use)
BOOL UnlockedLoaderHeap::GetMoreCommittedPages(size_t dwMinSize, 
                                               BOOL bGrowHeap,
                                               const BYTE *pMinAddr,
                                               const BYTE *pMaxAddr,
                                               BOOL fCanAlloc)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        INJECT_FAULT(return FALSE;);
    }
    CONTRACTL_END;
    
    // If we have memory we can use, what are you doing here!  
    DWORD_PTR memAvailable = m_pPtrToEndOfCommittedRegion - m_pAllocPtr;

    // The current region may be out of range, if there is a current region.
    BOOL fOutOfRange = (pMaxAddr < m_pAllocPtr || pMinAddr >= m_pEndReservedRegion) &&
                        m_pAllocPtr != NULL && m_pEndReservedRegion != NULL;

    _ASSERTE(dwMinSize > memAvailable || fOutOfRange);
    
    size_t dwSizeToCommit = m_dwCommitBlockSize;
    if (dwMinSize > memAvailable)
    {
        dwSizeToCommit = max(dwMinSize, dwSizeToCommit) - memAvailable;
    }

    // If we do not have space in the current block of heap to commit
    if (m_pAllocPtr + dwMinSize > m_pEndReservedRegion) {
        dwSizeToCommit = dwMinSize;
    }

    // Round to page size
    dwSizeToCommit = ALIGN_UP(dwSizeToCommit, PAGE_SIZE);

    // Does this fit in the reserved region?
    if (!fOutOfRange &&
         m_pPtrToEndOfCommittedRegion + dwSizeToCommit <= m_pEndReservedRegion)
    {
        // Don't report this memory in the memory report (we will report it later as consumed)
        MEMORY_REPORT_CLEAR();

        // Yes, so commit the desired number of reserved pages
        void *pData = ClrVirtualAlloc(m_pPtrToEndOfCommittedRegion, dwSizeToCommit, MEM_COMMIT, m_flProtect);
        if (pData == NULL)
            return FALSE;

        if (m_pPrivatePerfCounter_LoaderBytes)
            *m_pPrivatePerfCounter_LoaderBytes = *m_pPrivatePerfCounter_LoaderBytes + (DWORD) dwSizeToCommit;

        // If fCanAlloc is true, then we'll end up doing this work before
        // the actual alloc, but it won't change anything else.

        m_dwTotalAlloc += dwSizeToCommit;

        m_pPtrToEndOfCommittedRegion += dwSizeToCommit;
        return TRUE;
    }

    if (PreviouslyAllocated((BYTE*)pMinAddr, (BYTE*)pMaxAddr, dwMinSize, fCanAlloc))
        return TRUE;

    if (bGrowHeap)
    {
        // Need to allocate a new set of reserved pages
        INDEBUG(m_dwDebugWastedBytes += (size_t)(m_pPtrToEndOfCommittedRegion - m_pAllocPtr);)
    
        // Note, there are unused reserved pages at end of current region -can't do much about that
        // Provide dwMinSize here since UnlockedReservePages will round up the commit size again
        // after adding in the size of the LoaderHeapBlock header.
        return UnlockedReservePages(dwMinSize, 0, 0, pMinAddr, pMaxAddr, fCanAlloc);
    }
    return FALSE;
}

BOOL UnlockedLoaderHeap::PreviouslyAllocated(BYTE *pMinAddr, BYTE *pMaxAddr, size_t dwMinSize, BOOL fCanAlloc)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END;
    
    // We may have already allocated the memory when someone called "UnlockedCanAllocMem"
    if (m_pFirstBlock != NULL)
    {
        LoaderHeapBlockUnused* unused = NULL;
        
        // If we've already got a current block, then check to see if the 'next' one has
        // already been allocated.
        if (m_pCurBlock != NULL &&
            m_pCurBlock->pNext != NULL)
            unused = (LoaderHeapBlockUnused*)m_pCurBlock->pNext;

        // Alternately, the first thing the caller may have done is call "UCAM", in which
        // case the m_pFirstBlock will be set, but m_pCurBlock won't be (yet).
        else if (m_pFirstBlock != NULL &&
                 m_pCurBlock == NULL)
            unused = (LoaderHeapBlockUnused*)m_pFirstBlock;

        while(unused != NULL)
        {
            if (fCanAlloc)
            {
                BYTE *pBlockSpaceStart = (BYTE *)unused + sizeof(LoaderHeapBlock);

                // If there's space available, and it's located where we need it, use it.
                if (unused->cbReserved - sizeof(LoaderHeapBlock) >= dwMinSize &&
                    pBlockSpaceStart >= pMinAddr &&
                    pBlockSpaceStart + dwMinSize < pMaxAddr)
                    return TRUE;
                else
                // otherwise check the next one, if there is one.
                    unused = (LoaderHeapBlockUnused*)unused->pNext;
            }
            else
            {
                SETUP_NEW_BLOCK( ((void *)unused), unused->cbCommitted, unused->cbReserved);
                m_pCurBlock = unused;

                // Zero out the fields that we borrowed...
                unused->cbCommitted = 0;
                unused->cbReserved = 0;
                return TRUE; 
                // Note that we haven't actually checked to make sure that
                // this has enough space / is in the right place: UnlockedAllocMem 
                // will loop around, thus checking to make sure that 
                // this block is actually ok to use.
            }
        }
    }

    return FALSE;
}



void *UnlockedLoaderHeap::UnlockedAllocMem(size_t dwSize,
                                           BOOL bGrowHeap
                                           COMMA_INDEBUG(__in char *szFile)
                                           COMMA_INDEBUG(int  lineNum))
{
    CONTRACT(void*)
    {
        INSTANCE_CHECK;
        THROWS;
        INJECT_FAULT(ThrowOutOfMemory(););
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    void *pResult = UnlockedAllocMem_NoThrow(
        dwSize, bGrowHeap COMMA_INDEBUG(szFile) COMMA_INDEBUG(lineNum));

    if (pResult == NULL)
        ThrowOutOfMemory();
    
    RETURN pResult;
}

#ifdef _DEBUG
static DWORD ShouldInjectFault()
{
    static DWORD fInjectFault = 99;

    if (fInjectFault == 99)
        fInjectFault = (REGUTIL::GetConfigDWORD(L"InjectFault", 0) != 0);
    return fInjectFault;
}

#define SHOULD_INJECT_FAULT(return_statement)   \
    do {                                        \
        if (ShouldInjectFault() & 0x1)          \
        {                                       \
            char *a = new (nothrow) char;       \
            if (a == NULL)                      \
            {                                   \
                return_statement;               \
            }                                   \
            delete a;                           \
        }                                       \
    } while (FALSE)

#else

#define SHOULD_INJECT_FAULT(return_statement) do { ((void *)0); } while (FALSE)
        
#endif

void *UnlockedLoaderHeap::UnlockedAllocMem_NoThrow(size_t dwSize,
                                                   BOOL bGrowHeap
                                                   COMMA_INDEBUG(__in char *szFile)
                                                   COMMA_INDEBUG(int lineNum))
{
    CONTRACT(void*)
    {
        INSTANCE_CHECK;
        NOTHROW;
        INJECT_FAULT(CONTRACT_RETURN NULL;);
        PRECONDITION(dwSize != 0);
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    SHOULD_INJECT_FAULT(RETURN NULL);

    INDEBUG(size_t dwRequestedSize = dwSize;)

#ifndef SELF_NO_HOST
    INCONTRACT(_ASSERTE(IsInIEEInit() || !ARE_FAULTS_FORBIDDEN()));
#endif

#ifdef MAXALLOC

    CHECK_LOCAL_STATIC_VAR(static AllocRequestManager allocManager(L"AllocMaxLoaderHeap"));

    if (! allocManager.CheckRequest(dwSize))
        RETURN NULL;
#endif

    dwSize = AllocMem_TotalSize(dwSize, this);

again:

    {
        // Any memory available on the free list?
        void *pData = LoaderHeapFreeBlock::AllocFromFreeList(&m_pFirstFreeBlock, dwSize, TRUE /*fRemoveFromFreeList*/, this);
        if (!pData)
        {
            // Enough bytes available in committed region?
            if (dwSize <= GetBytesAvailCommittedRegion())
            {
                pData = m_pAllocPtr;
                m_pAllocPtr += dwSize;
            }
        }

        if (pData)
        {
#ifdef _DEBUG
    
#if LOADER_HEAP_DEBUG_BOUNDARY > 0
            // Don't fill the memory we allocated - it is assumed to be zeroed - fill the memory after it
            memset((BYTE *) pData + dwRequestedSize, 0xEE, LOADER_HEAP_DEBUG_BOUNDARY);
#endif

            if (!m_fExplicitControl)
            {
                LoaderHeapValidationTag *pTag = AllocMem_GetTag(pData, dwRequestedSize);
                pTag->m_allocationType  = kAllocMem;
                pTag->m_dwRequestedSize = dwRequestedSize;
                pTag->m_szFile          = szFile;
                pTag->m_lineNum         = lineNum;
            }

            if (m_dwDebugFlags & kCallTracing)
            {
                LoaderHeapSniffer::RecordEvent(this,
                                               kAllocMem,
                                               szFile,
                                               lineNum,
                                               szFile,
                                               lineNum,
                                               pData,
                                               dwRequestedSize,
                                               dwSize
                                               );
            }
    
#endif

            MEMORY_REPORT_ALLOCATE(pData, dwSize, MemoryReport::VIRTUAL_ALLOCATED_COMMIT_INUSE, 2 /* callers */);
    
            RETURN pData;
        }
    }

tryCommit:
    // Need to commit some more pages in reserved region.
    // If we run out of pages in the reserved region, ClrVirtualAlloc some more pages
    // if bGrowHeap is true
    if (GetMoreCommittedPages(dwSize,bGrowHeap, m_pMinAddr, m_pMaxAddr, FALSE))
        goto again;

    if (IsStandardExecutableRange(m_pMinAddr, m_pMaxAddr))
    {
        // If our standard executable range area is unavailable (or all used up)
        // we will accept any memory 
        m_pMinAddr = (BYTE *) BOT_MEMORY;
        m_pMaxAddr = (BYTE *) TOP_MEMORY;
        goto tryCommit;
    }
    
    // We could not satisfy this allocation request
    RETURN NULL;
}

// Can we allocate memory within the heap?
BOOL UnlockedLoaderHeap::UnlockedCanAllocMem(size_t dwSize, BOOL bGrowHeap)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        FORBID_FAULT;  // Since this api is used to ask politely, we'll assume the caller intends to handle the OOM himself
    }
    CONTRACTL_END;
    
    SHOULD_INJECT_FAULT(return FALSE);

    dwSize = AllocMem_TotalSize(dwSize, this);

again:
    {
        FAULT_NOT_FATAL();
        if (LoaderHeapFreeBlock::AllocFromFreeList(&m_pFirstFreeBlock, dwSize, FALSE /*fRemoveFromFreeList*/, this))
        {
            return TRUE;
        }
    }

    if (dwSize == 0 ||
        dwSize <= GetBytesAvailReservedRegion() ||
        PreviouslyAllocated((BYTE*)m_pMinAddr, 
                            (BYTE*)m_pMaxAddr, 
                            dwSize,
                            TRUE))
    {
        // We should only be handing out memory within the ranges the heap operates in.
        _ASSERTE(dwSize == 0 ||
                 PreviouslyAllocated((BYTE*)m_pMinAddr, 
                                     (BYTE*)m_pMaxAddr, 
                                     dwSize, 
                                     TRUE) ||
                 m_pAllocPtr >= m_pMinAddr && m_pAllocPtr +dwSize < m_pMaxAddr);
        return TRUE;
    }
    else
    {
        FAULT_NOT_FATAL();
        if (GetMoreCommittedPages(dwSize, bGrowHeap, m_pMinAddr, m_pMaxAddr, TRUE) == FALSE)
        {
            return FALSE;
        }
    }

    goto again;
}


void UnlockedLoaderHeap::UnlockedBackoutMem(void *pMem,
                                            size_t dwRequestedSize
                                            COMMA_INDEBUG(__in char *szFile)
                                            COMMA_INDEBUG(int  lineNum)
                                            COMMA_INDEBUG(__in char *szAllocFile)
                                            COMMA_INDEBUG(int  allocLineNum))
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        FORBID_FAULT;  
    }
    CONTRACTL_END;
    
    // Because the primary use of this function is backout, we'll be nice and
    // define Backout(NULL) be a legal NOP.
    if (pMem == NULL)
    {
        return;
    }

#ifdef _DEBUG
    {
        LoaderHeapValidationTag *pTag = AllocMem_GetTag(pMem, dwRequestedSize);

        if (pTag->m_dwRequestedSize != dwRequestedSize || pTag->m_allocationType != kAllocMem)
        {
            CONTRACT_VIOLATION(ThrowsViolation|FaultViolation); // We're reporting a heap corruption - who cares about violations

            StackSString message;
            message.Printf("HEAP VIOLATION: Invalid BackoutMem() call made at:\n"
                           "\n"
                           "     File: %s\n"
                           "     Line: %d\n"
                           "\n"
                           "Attempting to free block originally allocated at:\n"
                           "\n"
                           "     File: %s\n"
                           "     Line: %d\n"
                           "\n"
                           "The arguments to BackoutMem() were:\n"
                           "\n"
                           "     Pointer: 0x%p\n"
                           "     Size:    %lu (0x%lx)\n"
                           "\n"
                           ,szFile
                           ,lineNum
                           ,szAllocFile
                           ,allocLineNum
                           ,pMem
                           ,(ULONG)dwRequestedSize
                           ,(ULONG)dwRequestedSize
                          );


            if (m_dwDebugFlags & kCallTracing)
            {
                message.AppendASCII("*** CALLTRACING ENABLED ***\n");
                LoaderHeapEvent *pEvent = LoaderHeapSniffer::FindEvent(this, pMem);
                if (!pEvent)
                {
                    message.AppendASCII("This pointer doesn't appear to have come from this LoaderHeap.\n");
                }
                else
                {
                    message.AppendASCII(pMem == pEvent->m_pMem ? "We have the following data about this pointer:" : "This pointer points to the middle of the following block:");
                    pEvent->Describe(&message);
                }
            }

            if (pTag->m_dwRequestedSize != dwRequestedSize)
            {
                StackSString buf;
                buf.Printf(
                        "Possible causes:\n"
                        "\n"
                        "   - This pointer wasn't allocated from this loaderheap.\n"
                        "   - This pointer was allocated by AllocAlignedMem and you didn't adjust for the \"extra.\"\n" 
                        "   - This pointer has already been freed.\n"
                        "   - You passed in the wrong size. You must pass the exact same size you passed to AllocMem().\n"
                        "   - Someone wrote past the end of this block making it appear as if one of the above were true.\n"
                        );
                message.Append(buf);

            }
            else 
            {
                message.AppendASCII("This memory block is completely unrecognizable.\n");
            }


            if (!(m_dwDebugFlags & kCallTracing))
            {
                LoaderHeapSniffer::PitchSniffer(&message);
            }

            StackScratchBuffer scratch;
            DbgAssertDialog(szFile, lineNum, (char*) message.GetANSI(scratch));

        }
    }
#endif

    size_t dwSize = AllocMem_TotalSize(dwRequestedSize, this);

#ifdef _DEBUG
    if (m_dwDebugFlags & kCallTracing)
    {
        LoaderHeapValidationTag *pTag = m_fExplicitControl ? NULL : AllocMem_GetTag(pMem, dwRequestedSize);
        

        LoaderHeapSniffer::RecordEvent(this,
                                       kFreedMem,
                                       szFile,
                                       lineNum,
                                       (pTag && (allocLineNum < 0)) ? pTag->m_szFile  : szAllocFile,
                                       (pTag && (allocLineNum < 0)) ? pTag->m_lineNum : allocLineNum,
                                       pMem,
                                       dwRequestedSize,
                                       dwSize
                                       );
    }
#endif

    if (m_pAllocPtr == ( ((BYTE*)pMem) + dwSize ))
    {
        memset(pMem, 0, dwSize);  // Must zero init this memory as AllocMem expect it
        m_pAllocPtr = (BYTE*)pMem;
    }
    else
    {
        LoaderHeapFreeBlock::InsertFreeBlock(&m_pFirstFreeBlock, pMem, dwSize, this);
    }

}


// Can we allocate memory within the heap, but within different ranges than
// the heap's range?
BOOL UnlockedLoaderHeap::UnlockedCanAllocMemWithinRange(size_t dwSize, BYTE *pStart, BYTE *pEnd, BOOL bGrowHeap)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        FORBID_FAULT;  // Since this api is used to ask politely, we'll assume the caller intends to handle the OOM himself
    }
    CONTRACTL_END;
    
    // This function does not actually allocate any memory
    // It just checks that a memory request can be satisfied
    // So don't call ShouldInjectFault here

again:
    BYTE *pMemIsOkStart = UnlockedGetAllocPtr();
    BYTE *pMemIsOkEnd = pMemIsOkStart + UnlockedGetReservedBytesFree();

    // If the next available memory completely within the given range, use it
    if (dwSize == 0 ||
        (dwSize <= GetBytesAvailReservedRegion() &&
         pMemIsOkStart >= pStart &&
         pMemIsOkEnd < pEnd) ||
        PreviouslyAllocated((BYTE*)m_pMinAddr, 
                            (BYTE*)m_pMaxAddr, 
                            dwSize,
                            TRUE))
    {
        return TRUE;
    }
    else
    {
        FAULT_NOT_FATAL();
        if (GetMoreCommittedPages(dwSize, bGrowHeap, pStart, pEnd, TRUE))
          goto again;
    }
        
    return FALSE;
}



// Allocates memory aligned on power-of-2 boundary.
//
// The return value is a pointer that's guaranteed to be aligned.
//
// FREEING THIS BLOCK: Underneath, the actual block allocated may
// be larger and start at an address prior to the one you got back.
// It is this adjusted size and pointer that you pass to BackoutMem.
// The required adjustment is passed back thru the pdwExtra pointer.
//
// Here is how to properly backout the memory:
//
//   size_t dwExtra;
//   void *pMem = UnlockedAllocAlignedMem(dwRequestedSize, alignment, bgrowHeap, &dwExtra);
//   _ASSERTE( 0 == (pMem & (alignment - 1)) );
//   UnlockedBackoutMem( ((BYTE*)pMem) - dExtra, dwRequestedSize + dwExtra );
//
// If you use the AllocMemHolder or AllocMemTracker, all this is taken care of
// behind the scenes. 
//
// 
void *UnlockedLoaderHeap::UnlockedAllocAlignedMem_NoThrow(size_t  dwRequestedSize,
                                                          size_t  alignment,
                                                          size_t *pdwExtra,
                                                          BOOL    bGrowHeap
                                                          COMMA_INDEBUG(__in char *szFile)
                                                          COMMA_INDEBUG(int  lineNum))
{
    CONTRACT(void*)
    {
        NOTHROW;

        // Macro syntax can't handle this INJECT_FAULT expression - we'll use a precondition instead
        //INJECT_FAULT( do{ if (*pdwExtra) {*pdwExtra = 0} RETURN NULL; } while(0) );

        PRECONDITION( alignment != 0 );
        PRECONDITION(0 == (alignment & (alignment - 1))); // require power of 2
        POSTCONDITION( (RETVAL) ?
                       (0 == ( ((UINT_PTR)(RETVAL)) & (alignment - 1))) : // If non-null, pointer must be aligned
                       (pdwExtra == NULL || 0 == *pdwExtra)    //   or else *pdwExtra must be set to 0
                     );
    }
    CONTRACT_END

    STATIC_CONTRACT_FAULT;

    // Set default value
            if (pdwExtra)
            {
                *pdwExtra = 0;
            }

    SHOULD_INJECT_FAULT(RETURN NULL);

    void *pResult;
    
#ifndef SELF_NO_HOST
    INCONTRACT(_ASSERTE(IsInIEEInit() || !ARE_FAULTS_FORBIDDEN()));
#endif



    // We don't know how much "extra" we need to satisfy the alignment until we know
    // which address will be handed out which in turn we don't know because we don't
    // know whether the allocation will fit within the current reserved range.
    //
    // Thus, we'll request as much heap growth as is needed for the worst case (extra == alignment)
    size_t dwRoomSize = AllocMem_TotalSize(dwRequestedSize + alignment, this);
    if (dwRoomSize > GetBytesAvailCommittedRegion())
    {
again:
        if (!GetMoreCommittedPages(dwRoomSize, bGrowHeap, m_pMinAddr, m_pMaxAddr, FALSE))
        {
            if (IsStandardExecutableRange(m_pMinAddr, m_pMaxAddr))
            {
                // If our standard executable range area is unavailable (or all used up)
                // we will accept any memory 
                m_pMinAddr = (BYTE *) BOT_MEMORY;
                m_pMaxAddr = (BYTE *) TOP_MEMORY;
                goto again;
            }

            RETURN NULL;
        }
    }

    pResult = m_pAllocPtr;

    size_t extra = alignment - ((size_t)pResult & ((size_t)alignment - 1));

// On DEBUG, we force a non-zero extra so people don't forget to adjust for it on backout
#ifndef _DEBUG
    if (extra == alignment)
    {
        extra = 0;
    }
#endif

    S_SIZE_T cbAllocSize = S_SIZE_T( dwRequestedSize ) + S_SIZE_T( extra );
    if( cbAllocSize.IsOverflow() )
    {
        RETURN NULL;
    }

#ifdef MAXALLOC
    CHECK_LOCAL_STATIC_VAR(static AllocRequestManager allocManager(L"AllocMaxLoaderHeap"));

    if (! allocManager.CheckRequest( cbAllocSize.Value() ))
    {
        RETURN NULL;
    }
#endif


    size_t dwSize = AllocMem_TotalSize( cbAllocSize.Value(), this);
    m_pAllocPtr += dwSize;

    
    ((BYTE*&)pResult) += extra;
#ifdef _DEBUG
        // Don't fill the entire memory - we assume it is all zeroed -just the memory after our alloc
#if LOADER_HEAP_DEBUG_BOUNDARY > 0
    memset( ((BYTE*)pResult) + dwRequestedSize, 0xee, LOADER_HEAP_DEBUG_BOUNDARY );
#endif

    if (m_dwDebugFlags & kCallTracing)
    {
        LoaderHeapSniffer::RecordEvent(this,
                                       kAllocMem,
                                       szFile,
                                       lineNum,
                                       szFile,
                                       lineNum,
                                       ((BYTE*)pResult) - extra,
                                       dwRequestedSize + extra,
                                       dwSize
                                       );
    }


    if (!m_fExplicitControl)
    {
        LoaderHeapValidationTag *pTag = AllocMem_GetTag(((BYTE*)pResult) - extra, dwRequestedSize + extra);
        pTag->m_allocationType  = kAllocMem;
        pTag->m_dwRequestedSize = dwRequestedSize + extra;
        pTag->m_szFile          = szFile;
        pTag->m_lineNum         = lineNum;
    }
#endif

    if (pdwExtra)
    {
        *pdwExtra = extra;
    }

    RETURN pResult;

}



void *UnlockedLoaderHeap::UnlockedAllocAlignedMem(size_t  dwRequestedSize,
                                                  size_t  dwAlignment,
                                                  size_t *pdwExtra,
                                                  BOOL    bGrowHeap
                                                  COMMA_INDEBUG(__in char *szFile)
                                                  COMMA_INDEBUG(int  lineNum))
{
    CONTRACTL
    {
        THROWS;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END

    void *pResult = UnlockedAllocAlignedMem_NoThrow(dwRequestedSize,
                                                    dwAlignment,
                                                    pdwExtra,
                                                    bGrowHeap
                                                    COMMA_INDEBUG(szFile)
                                                    COMMA_INDEBUG(lineNum));

    if (!pResult)
    {
        ThrowOutOfMemory();
    }

    return pResult;
    

}






void *UnlockedLoaderHeap::UnlockedAllocAlignedmem(size_t dwRequestedSize, DWORD alignment, BOOL bGrowHeap)
{
    CONTRACT(void*)
    {
        INSTANCE_CHECK;
        THROWS;
        INJECT_FAULT(ThrowOutOfMemory());
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    void *pResult = UnlockedAllocAlignedmem_NoThrow(dwRequestedSize, alignment, bGrowHeap);

    if (pResult == NULL)
        ThrowOutOfMemory();

    RETURN pResult;
}

void *UnlockedLoaderHeap::UnlockedAllocAlignedmem_NoThrow(size_t dwRequestedSize, DWORD alignment, BOOL bGrowHeap)
{
    CONTRACT(void*)
    {
        INSTANCE_CHECK;
        NOTHROW;
        INJECT_FAULT(CONTRACT_RETURN NULL;);
        PRECONDITION(0 == (alignment & (alignment - 1))); // require power of 2
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    SHOULD_INJECT_FAULT(RETURN NULL);

    void *pResult;

#ifndef SELF_NO_HOST
    INCONTRACT(_ASSERTE(IsInIEEInit() || !ARE_FAULTS_FORBIDDEN()));
#endif

    // We don't know how much "extra" we need to satisfy the alignment until we know
    // which address will be handed out which in turn we don't know because we don't
    // know whether the allocation will fit within the current reserved range.
    //
    // Thus, we'll request as much heap growth as is needed for the worst case (extra == alignment)
    size_t dwRoomSize = AllocMem_TotalSize(dwRequestedSize + alignment, this);
    if (dwRoomSize > GetBytesAvailCommittedRegion())
    {
        if (GetMoreCommittedPages(dwRoomSize, bGrowHeap, m_pMinAddr, m_pMaxAddr, FALSE) == FALSE)
        {
            RETURN NULL;
        }
    }

    pResult = m_pAllocPtr;

    DWORD extra = alignment - (DWORD)((size_t)pResult & ((size_t)alignment - 1));
    if (extra == alignment)
    {
        extra = 0;
    }

    S_SIZE_T cbAllocSize = S_SIZE_T( dwRequestedSize ) + S_SIZE_T( extra );
    if( cbAllocSize.IsOverflow() )
    {
        RETURN NULL;
    }

#ifdef MAXALLOC
    CHECK_LOCAL_STATIC_VAR(static AllocRequestManager allocManager(L"AllocMaxLoaderHeap"));

    if (! allocManager.CheckRequest( cbAllocSize.Value() ))
    {
        RETURN NULL;
    }
#endif


    size_t dwSize = AllocMem_TotalSize(cbAllocSize.Value(), this);
    m_pAllocPtr += dwSize;


    ((BYTE*&)pResult) += extra;
#ifdef _DEBUG
        // Don't fill the entire memory - we assume it is all zeroed -just the memory after our alloc
#if LOADER_HEAP_DEBUG_BOUNDARY > 0
    memset( ((BYTE*)pResult) + dwRequestedSize, 0xee, LOADER_HEAP_DEBUG_BOUNDARY );
#endif

    _ASSERTE(m_fExplicitControl);

#endif

    RETURN pResult;
}


#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void UnlockedLoaderHeap::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    DAC_ENUM_DTHIS();

    PTR_LoaderHeapBlock block = m_pFirstBlock;
    while (block.IsValid())
    {
        TADDR addr = (TADDR)block->pVirtualAddress;
        size_t size = block->dwVirtualSize;

        while (size)
        {
            ULONG32 enumSize;

            if (size > 0x80000000)
            {
                enumSize = 0x80000000;
            }
            else
            {
                enumSize = (ULONG32)size;
            }
            
            DacEnumMemoryRegion(addr, enumSize);

            addr += enumSize;
            size -= enumSize;
        }
        
        block = block->pNext;
    }
}

#endif // #ifdef DACCESS_COMPILE


void UnlockedLoaderHeap::EnumPageRegions (EnumPageRegionsCallback *pCallback)
{
    WRAPPER_CONTRACT;

    PTR_LoaderHeapBlock block = m_pFirstBlock;
    while (block)
    {
        (*pCallback)(block->pVirtualAddress, block->dwVirtualSize);
        
        block = block->pNext;
    }
}


#ifdef _DEBUG

void UnlockedLoaderHeap::DumpFreeList()
{
    LEAF_CONTRACT;
    if (m_pFirstFreeBlock == NULL)
    {
        printf("FREEDUMP: FreeList is empty\n");
    }
    else
    {
        LoaderHeapFreeBlock *pBlock = m_pFirstFreeBlock;
        while (pBlock != NULL)
        {
            size_t dwsize = pBlock->m_dwSize;
            BOOL ccbad = FALSE;
            BOOL sizeunaligned = FALSE;
            BOOL sizesmall = FALSE;

            if ( 0 != (dwsize & ALLOC_ALIGN_CONSTANT) )
            {
                sizeunaligned = TRUE;
            }
            if ( dwsize < sizeof(LoaderHeapBlock))
            {
                sizesmall = TRUE;
            }

            for (size_t i = sizeof(LoaderHeapFreeBlock); i < dwsize; i++)
            {
                if ( ((BYTE*)pBlock)[i] != 0xcc )
                {
                    ccbad = TRUE;
                    break;
                }
            }

            printf("Addr = %pxh, Size = %lxh", pBlock, ((ULONG)dwsize));
            if (ccbad) printf(" *** ERROR: NOT CC'd ***");
            if (sizeunaligned) printf(" *** ERROR: size not a multiple of ALLOC_ALIGN_CONSTANT ***");
            if (sizesmall) printf(" *** ERROR: size smaller than sizeof(LoaderHeapFreeBlock) ***");
            printf("\n");

            pBlock = pBlock->m_pNext;
        }
    }
}


void UnlockedLoaderHeap::UnlockedClearEvents()
{
    WRAPPER_CONTRACT;
    LoaderHeapSniffer::ClearEvents(this);
}

void UnlockedLoaderHeap::UnlockedCompactEvents()
{
    WRAPPER_CONTRACT;
    LoaderHeapSniffer::CompactEvents(this);
}

void UnlockedLoaderHeap::UnlockedPrintEvents()
{
    WRAPPER_CONTRACT;
    LoaderHeapSniffer::PrintEvents(this);
}


#endif

#ifdef MAXALLOC
AllocRequestManager::AllocRequestManager(LPCTSTR key)
{
    WRAPPER_CONTRACT;
    m_newRequestCount = 0;
    m_maxRequestCount = UINT_MAX;    // to allow allocation during GetLong
    OnUnicodeSystem();
    m_maxRequestCount = REGUTIL::GetConfigDWORD(key, m_maxRequestCount);
}

BOOL AllocRequestManager::CheckRequest(size_t size)
{
    LEAF_CONTRACT;

    if (m_maxRequestCount == UINT_MAX)
        return TRUE;

    if (m_newRequestCount >= m_maxRequestCount)
        return FALSE;
    ++m_newRequestCount;
    return TRUE;
}

void AllocRequestManager::UndoRequest()
{
    LEAF_CONTRACT;
    if (m_maxRequestCount == UINT_MAX)
        return;

    _ASSERTE(m_newRequestCount > 0);
    --m_newRequestCount;
}

void * AllocMaxNew(size_t n, void **ppvCallstack, BOOL isArray)
{
    CANNOT_HAVE_CONTRACT;
    STATIC_CONTRACT_DEBUG_ONLY;

    // Can't have a contract here because AllocMaxNew is called well before the system is initialized enough to
    // support contracts.

    CHECK_LOCAL_STATIC_VAR(static AllocRequestManager allocManager(L"AllocMaxNew"));

    if (n == 0) n++;        // allocation size always > 0, makes Boundschecker happy

    if (! allocManager.CheckRequest(n))
        return NULL;
    return DbgAlloc(n, ppvCallstack, isArray);
}
#endif // MAXALLOC


//************************************************************************************
// LOADERHEAP SNIFFER METHODS
//************************************************************************************
#ifdef _DEBUG

/*static*/ VOID LoaderHeapSniffer::RecordEvent(UnlockedLoaderHeap *pHeap,
                                               AllocationType allocationType,
                                               __in char     *szFile,
                                               int            lineNum,
                                               __in char     *szAllocFile,
                                               int            allocLineNum,
                                               void          *pMem,
                                               size_t         dwRequestedSize,
                                               size_t         dwSize
                                              )
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;  //If we OOM in here, we just throw the event away.
    }
    CONTRACTL_END

    LoaderHeapEvent *pNewEvent;
    {
        {
            FAULT_NOT_FATAL();
            pNewEvent = new (nothrow) LoaderHeapEvent;
        }
        if (!pNewEvent)
        {
            if (!(pHeap->m_dwDebugFlags & pHeap->kEncounteredOOM))
            {
                pHeap->m_dwDebugFlags |= pHeap->kEncounteredOOM;
                _ASSERTE(!"LOADERHEAPSNIFFER: Failed allocation of LoaderHeapEvent. Call tracing information will be incomplete.");
            }
        }
        else
        {
            pNewEvent->m_allocationType     = allocationType;
            pNewEvent->m_szFile             = szFile;
            pNewEvent->m_lineNum            = lineNum;
            pNewEvent->m_szAllocFile        = szAllocFile;
            pNewEvent->m_allocLineNum       = allocLineNum;
            pNewEvent->m_pMem               = pMem;
            pNewEvent->m_dwRequestedSize    = dwRequestedSize;
            pNewEvent->m_dwSize             = dwSize;

            pNewEvent->m_pNext              = pHeap->m_pEventList;
            pHeap->m_pEventList             = pNewEvent;
        }
    }
}



/*static*/
void LoaderHeapSniffer::ValidateFreeList(UnlockedLoaderHeap *pHeap)
{
    CANNOT_HAVE_CONTRACT;

    // No contract. This routine is only called if we've AV'd inside the
    // loaderheap. The system is already toast. We're trying to be a hero
    // and produce the best diagnostic info we can. Last thing we need here
    // is a secondary assert inside the contract stuff.
    //
    // This contract violation is permanent.
    CONTRACT_VIOLATION(ThrowsViolation|FaultViolation|GCViolation|ModeViolation);  // This violation won't be removed

    LoaderHeapFreeBlock *pFree     = pHeap->m_pFirstFreeBlock;
    LoaderHeapFreeBlock *pPrev     = NULL;

   
    void                *pBadAddr = NULL;
    LoaderHeapFreeBlock *pProbeThis = NULL;
    char                *pExpected = NULL;

    while (pFree != NULL)
    {
        if ( 0 != ( ((ULONG_PTR)pFree) & ALLOC_ALIGN_CONSTANT ))
        {
            // Not aligned - can't be a valid freeblock. Most likely we followed a bad pointer from the previous block.
            pProbeThis = pPrev;
            pBadAddr = pPrev ? &(pPrev->m_pNext) : &(pHeap->m_pFirstFreeBlock);
            pExpected = "a pointer to a valid LoaderHeapFreeBlock";
            break;
        }

        size_t dwSize = pFree->m_dwSize;
        if (dwSize < AllocMem_TotalSize(1, pHeap) ||
            0 != (dwSize & ALLOC_ALIGN_CONSTANT))
        {
            // Size is not a valid value (out of range or unaligned.)
            pProbeThis = pFree;
            pBadAddr = &(pFree->m_dwSize);
            pExpected = "a valid block size (multiple of pointer size)";
            break;
        }

        size_t i;
        for (i = sizeof(LoaderHeapFreeBlock); i < dwSize; i++)
        {
            if ( ((BYTE*)pFree)[i] != 0xcc )
            {
                pProbeThis = pFree;
                pBadAddr = i + ((BYTE*)pFree);
                pExpected = "0xcc (our fill value for free blocks)";
                break;
            }
        }
        if (i != dwSize)
        {
            break;
        }
         


        pPrev = pFree;
        pFree = pFree->m_pNext;
    }

    if (pFree == NULL)
    {
        return; // No problems found
    }

    {
        StackSString message;

        message.Printf("A loaderheap freelist has been corrupted. The bytes at or near address 0x%p appears to have been overwritten. We expected to see %s here.\n"
                       "\n"
                       "    LoaderHeap:                 0x%p\n"
                       "    Suspect address at:         0x%p\n"
                       "    Start of suspect freeblock: 0x%p\n"
                       "\n"    
                       , pBadAddr
                       , pExpected
                       , pHeap
                       , pBadAddr
                       , pProbeThis
                       );

        if (!(pHeap->m_dwDebugFlags & pHeap->kCallTracing))
        {
            message.AppendASCII("\nThe usual reason is that someone wrote past the end of a block or wrote into a block after freeing it."
                           "\nOf course, the culprit is long gone so it's probably too late to debug this now. Try turning on call-tracing"
                           "\nand reproing. We can attempt to find out who last owned the surrounding pieces of memory."
                           "\n"
                           "\nTo turn on call-tracing, set the following registry DWORD value:"
                           "\n"
                           "\n    HKLM\\Software\\Microsoft\\.NETFramework\\LoaderHeapCallTracing = 1"
                           "\n"
                           );

        }
        else
        {
            LoaderHeapEvent *pBadAddrEvent = FindEvent(pHeap, pBadAddr);

            message.AppendASCII("*** CALL TRACING ENABLED ***\n\n");

            if (pBadAddrEvent)
            {
                message.AppendASCII("\nThe last known owner of the corrupted address was:\n");
                pBadAddrEvent->Describe(&message);
            }
            else
            {
                message.AppendASCII("\nNo known owner of last corrupted address.\n");
            }

            LoaderHeapEvent *pPrevEvent = FindEvent(pHeap, ((BYTE*)pProbeThis) - 1);

            int count = 3;
            while (count-- && 
                   pPrevEvent != NULL &&
                   ( ((UINT_PTR)pProbeThis) - ((UINT_PTR)(pPrevEvent->m_pMem)) + pPrevEvent->m_dwSize ) < 1024)
            {
                message.AppendASCII("\nThis block is located close to the corruption point. ");
                if (pPrevEvent->QuietValidate())
                {
                    message.AppendASCII("If it was overrun, it might have caused this.");
                }
                else
                {
                    message.AppendASCII("*** CORRUPTION DETECTED IN THIS BLOCK ***");
                }
                pPrevEvent->Describe(&message);
                pPrevEvent = FindEvent(pHeap, ((BYTE*)(pPrevEvent->m_pMem)) - 1);
            }


        }

        StackScratchBuffer scratch;
        DbgAssertDialog(__FILE__, __LINE__, (char*) message.GetANSI(scratch));

    }

    
    
}


BOOL LoaderHeapEvent::QuietValidate()
{
    WRAPPER_CONTRACT;

    if (m_allocationType == kAllocMem)
    {
        LoaderHeapValidationTag *pTag = AllocMem_GetTag(m_pMem, m_dwRequestedSize);
        return (pTag->m_allocationType == m_allocationType && pTag->m_dwRequestedSize == m_dwRequestedSize);
    }
    else
    {
        // We can't easily validate freed blocks, dammit.
        return TRUE;
    }
}


#endif

#ifndef DACCESS_COMPILE

AllocMemTracker::AllocMemTracker()
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    m_FirstBlock.m_pNext    = NULL;
    m_FirstBlock.m_nextFree = 0;
    m_pFirstBlock = &m_FirstBlock;

    m_fReleased   = FALSE;
}

AllocMemTracker::~AllocMemTracker()
{
    CONTRACTL
    {
        NOTHROW;
        FORBID_FAULT;
    }
    CONTRACTL_END

    if (!m_fReleased)
    {
        AllocMemTrackerBlock *pBlock = m_pFirstBlock;
        while (pBlock)
        {
            // Do the loop in reverse - loaderheaps work best if
            // we allocate and backout in LIFO order.
            for (int i = pBlock->m_nextFree - 1; i >= 0; i--)
            {
                AllocMemTrackerNode *pNode = &(pBlock->m_Node[i]);
                pNode->m_pHeap->RealBackoutMem(pNode->m_pMem
                                               ,pNode->m_dwRequestedSize
#ifdef _DEBUG
                                               ,__FILE__
                                               ,__LINE__
                                               ,pNode->m_szAllocFile
                                               ,pNode->m_allocLineNum
#endif
                                              );
    
            }
    
            pBlock = pBlock->m_pNext;
        }
    }


    AllocMemTrackerBlock *pBlock = m_pFirstBlock;
    while (pBlock != &m_FirstBlock)
    {
        AllocMemTrackerBlock *pNext = pBlock->m_pNext;
        delete pBlock;
        pBlock = pNext;
    }

    INDEBUG(memset(this, 0xcc, sizeof(*this));)

}

void *AllocMemTracker::Track(TaggedMemAllocPtr tmap)
{
    CONTRACTL
    {
        THROWS;
        INJECT_FAULT(ThrowOutOfMemory(););
    }
    CONTRACTL_END

    _ASSERTE(this); 

    void *pv = Track_NoThrow(tmap);
    if (!pv)
    {
        ThrowOutOfMemory();
    }
    return pv;
}

void *AllocMemTracker::Track_NoThrow(TaggedMemAllocPtr tmap)
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return NULL;);
    }
    CONTRACTL_END

    _ASSERTE(this); 

    // Calling Track() after calling SuppressRelease() is almost certainly a bug. You're supposed to call SuppressRelease() only after you're
    // sure no subsequent failure will force you to backout the memory.
    _ASSERTE( (!m_fReleased) && "You've already called SuppressRelease on this AllocMemTracker which implies you've passed your point of no failure. Why are you still doing allocations?");


    if (tmap.m_pMem != NULL)
    {
        AllocMemHolder<void*> holder(tmap);
        if (m_fReleased)
        {
            holder.SuppressRelease();
        }
        AllocMemTrackerBlock *pBlock = m_pFirstBlock;
        if (pBlock->m_nextFree == kAllocMemTrackerBlockSize)
        {
            AllocMemTrackerBlock *pNewBlock = new (nothrow) AllocMemTrackerBlock;
            if (!pNewBlock)
            {
                return NULL;
            }

            pNewBlock->m_pNext = m_pFirstBlock;
            pNewBlock->m_nextFree = 0;

            m_pFirstBlock = pNewBlock;

            pBlock = pNewBlock;
        }

        // From here on, we can't fail
        pBlock->m_Node[pBlock->m_nextFree].m_pHeap           = tmap.m_pHeap;
        pBlock->m_Node[pBlock->m_nextFree].m_pMem            = tmap.m_pMem;
        pBlock->m_Node[pBlock->m_nextFree].m_dwRequestedSize = tmap.m_dwRequestedSize;
#ifdef _DEBUG
        pBlock->m_Node[pBlock->m_nextFree].m_szAllocFile     = tmap.m_szFile;
        pBlock->m_Node[pBlock->m_nextFree].m_allocLineNum    = tmap.m_lineNum;
#endif

        pBlock->m_nextFree++;

        holder.SuppressRelease();


    }
    return (void *)tmap;
    


}


void AllocMemTracker::SuppressRelease()
{
    LEAF_CONTRACT;

    _ASSERTE(this); 

    m_fReleased = TRUE;
}

#endif //#ifndef DACCESS_COMPILE

#ifdef _MSC_VER
#pragma warning(default : 4640)
#endif
