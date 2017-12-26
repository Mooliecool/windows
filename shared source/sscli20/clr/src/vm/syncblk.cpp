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
//
// SYNCBLK.CPP
//
// Definition of a SyncBlock and the SyncBlockCache which manages it
//

#include "common.h"

#include "vars.hpp"
#include "util.hpp"
#include "class.h"
#include "object.h"
#include "threads.h"
#include "excep.h"
#include "threads.h"
#include "syncblk.h"
#include "interoputil.h"
#include "encee.h"
#include "perfcounters.h"
#include "eventtrace.h"
#include "dllimportcallback.h"
#include "comcallablewrapper.h"
#include "eeconfig.h"
#include "corhost.h"
#include "comdelegate.h"
#include "comstring.h"


// Allocate 1 page worth. Typically enough
#define MAXSYNCBLOCK (PAGE_SIZE-sizeof(void*))/sizeof(SyncBlock)
#define SYNC_TABLE_INITIAL_SIZE 250

//#define DUMP_SB

class  SyncBlockArray
{
  public:
    SyncBlockArray *m_Next;
    BYTE            m_Blocks[MAXSYNCBLOCK * sizeof (SyncBlock)];
};

// For in-place constructor
BYTE g_SyncBlockCacheInstance[sizeof(SyncBlockCache)];

SPTR_IMPL (SyncBlockCache, SyncBlockCache, s_pSyncBlockCache);

#ifndef DACCESS_COMPILE



void SyncBlock::OnADUnload()
{
    LEAF_CONTRACT;
}

InteropSyncBlockInfo::~InteropSyncBlockInfo()
{
    CONTRACTL
    {
        NOTHROW;
        DESTRUCTOR_CHECK;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    FreeUMEntryThunk();
}

void InteropSyncBlockInfo::FreeUMEntryThunk()
{
    CONTRACTL
    {
        NOTHROW;
        DESTRUCTOR_CHECK;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    if (!g_fEEShutDown && m_pUMEntryThunk != NULL)
    {
        COMDelegate::RemoveEntryFromFPtrHash((UPTR)(LPVOID)m_pUMEntryThunk);
        UMEntryThunk::FreeUMEntryThunk((UMEntryThunk*)m_pUMEntryThunk);
    }

    m_pUMEntryThunk = NULL;
}

void UMEntryThunk::OnADUnload()
{
    LEAF_CONTRACT;
#ifndef DACCESS_COMPILE
    m_pObjectHandle = NULL;
#endif
}

SyncTableEntry*& SyncTableEntry::GetSyncTableEntry()
{
    LEAF_CONTRACT;

    return g_pSyncTable;
}

/* static */
SyncBlockCache*& SyncBlockCache::GetSyncBlockCache()
{
    LEAF_CONTRACT;

    return s_pSyncBlockCache;
}


//----------------------------------------------------------------------------
//
//   ThreadQueue Implementation
//
//----------------------------------------------------------------------------

// Given a link in the chain, get the Thread that it represents
/* static */
inline WaitEventLink *ThreadQueue::WaitEventLinkForLink(SLink *pLink)
{
    LEAF_CONTRACT;
    return (WaitEventLink *) (((BYTE *) pLink) - offsetof(WaitEventLink, m_LinkSB));
}


// Unlink the head of the Q.  We are always in the SyncBlock's critical
// section.
/* static */
inline WaitEventLink *ThreadQueue::DequeueThread(SyncBlock *psb)
{
    LEAF_CONTRACT;
    SyncBlockCache::LockHolder lh(SyncBlockCache::GetSyncBlockCache());

    WaitEventLink      *ret = NULL;
    SLink       *pLink = psb->m_Link.m_pNext;

    if (pLink)
    {
        psb->m_Link.m_pNext = pLink->m_pNext;
#ifdef _DEBUG
        pLink->m_pNext = (SLink *)POISONC;
#endif
        ret = WaitEventLinkForLink(pLink);
        _ASSERTE(ret->m_WaitSB == psb);
        COUNTER_ONLY(GetPrivatePerfCounters().m_LocksAndThreads.cQueueLength--);
    }
    return ret;
}

// Enqueue is the slow one.  We have to find the end of the Q since we don't
// want to burn storage for this in the SyncBlock.
/* static */
inline void ThreadQueue::EnqueueThread(WaitEventLink *pWaitEventLink, SyncBlock *psb)
{
    LEAF_CONTRACT;
    COUNTER_ONLY(GetPrivatePerfCounters().m_LocksAndThreads.cQueueLength++);

    _ASSERTE (pWaitEventLink->m_LinkSB.m_pNext == NULL);

    SyncBlockCache::LockHolder lh(SyncBlockCache::GetSyncBlockCache());

    SLink       *pPrior = &psb->m_Link;

    while (pPrior->m_pNext)
    {
        // We shouldn't already be in the waiting list!
        _ASSERTE(pPrior->m_pNext != &pWaitEventLink->m_LinkSB);

        pPrior = pPrior->m_pNext;
    }
    pPrior->m_pNext = &pWaitEventLink->m_LinkSB;
}


// Wade through the SyncBlock's list of waiting threads and remove the
// specified thread.
/* static */
BOOL ThreadQueue::RemoveThread (Thread *pThread, SyncBlock *psb)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    BOOL res = FALSE;

    SyncBlockCache::LockHolder lh(SyncBlockCache::GetSyncBlockCache());

    SLink       *pPrior = &psb->m_Link;
    SLink       *pLink;
    WaitEventLink *pWaitEventLink;

    while ((pLink = pPrior->m_pNext) != NULL)
    {
        pWaitEventLink = WaitEventLinkForLink(pLink);
        if (pWaitEventLink->m_Thread == pThread)
        {
            pPrior->m_pNext = pLink->m_pNext;
#ifdef _DEBUG
            pLink->m_pNext = (SLink *)POISONC;
#endif
            _ASSERTE(pWaitEventLink->m_WaitSB == psb);
            res = TRUE;
            break;
        }
        pPrior = pLink;
    }
    return res;
}

// ***************************************************************************
//
//              Ephemeral Bitmap Helper
//
// ***************************************************************************

#define card_size 32

#define card_word_width 32

size_t CardIndex (size_t card)
{
    LEAF_CONTRACT;
    return card_size * card;
}

size_t CardOf (size_t idx)
{
    LEAF_CONTRACT;
    return idx / card_size;
}

size_t CardWord (size_t card)
{
    LEAF_CONTRACT;
    return card / card_word_width;
}
inline
unsigned CardBit (size_t card)
{
    LEAF_CONTRACT;
    return (unsigned)(card % card_word_width);
}

inline
void SyncBlockCache::SetCard (size_t card)
{
    WRAPPER_CONTRACT;
    m_EphemeralBitmap [CardWord (card)] =
        (m_EphemeralBitmap [CardWord (card)] | (1 << CardBit (card)));
}

inline
void SyncBlockCache::ClearCard (size_t card)
{
    WRAPPER_CONTRACT;
    m_EphemeralBitmap [CardWord (card)] =
        (m_EphemeralBitmap [CardWord (card)] & ~(1 << CardBit (card)));
}

inline
BOOL  SyncBlockCache::CardSetP (size_t card)
{
    WRAPPER_CONTRACT;
    return ( m_EphemeralBitmap [ CardWord (card) ] & (1 << CardBit (card)));
}

inline
void SyncBlockCache::CardTableSetBit (size_t idx)
{
    WRAPPER_CONTRACT;
    SetCard (CardOf (idx));
}


size_t BitMapSize (size_t cacheSize)
{
    LEAF_CONTRACT;

    return (cacheSize + card_size * card_word_width - 1)/ (card_size * card_word_width);
}

// ***************************************************************************
//
//              SyncBlockCache class implementation
//
// ***************************************************************************

SyncBlockCache::SyncBlockCache()
    : m_pCleanupBlockList(NULL),
      m_FreeBlockList(NULL),
      m_CacheLock("SyncBlockCache", CrstSyncBlockCache, (CrstFlags) (CRST_UNSAFE_ANYMODE | CRST_DEBUGGER_THREAD)),
      m_FreeCount(0),
      m_ActiveCount(0),
      m_SyncBlocks(0),
      m_FreeSyncBlock(0),
      m_FreeSyncTableIndex(1),
      m_FreeSyncTableList(0),
      m_SyncTableSize(SYNC_TABLE_INITIAL_SIZE),
      m_OldSyncTables(0),
      m_bSyncBlockCleanupInProgress(FALSE),
      m_EphemeralBitmap(0)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;
}


SyncBlockCache::~SyncBlockCache()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Clear the list the fast way.
    m_FreeBlockList = NULL;
    m_pCleanupBlockList = NULL;

    // destruct all arrays
    while (m_SyncBlocks)
    {
        SyncBlockArray *next = m_SyncBlocks->m_Next;
        delete m_SyncBlocks;
        m_SyncBlocks = next;
    }

    // Also, now is a good time to clean up all the old tables which we discarded
    // when we overflowed them.
    SyncTableEntry* arr;
    while ((arr = m_OldSyncTables) != 0)
    {
        m_OldSyncTables = (SyncTableEntry*)arr[0].m_Object;
        delete arr;
    }
}

void SyncBlockCache::CleanupSyncBlocks()
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    _ASSERTE(GetThread() == GCHeap::GetGCHeap()->GetFinalizerThread());

    // Set the flag indicating sync block cleanup is in progress.
    // IMPORTANT: This must be set before the sync block cleanup bit is reset on the thread.
    m_bSyncBlockCleanupInProgress = TRUE;

    SyncBlock* psb = NULL;


    EE_TRY_FOR_FINALLY
    {
        // reset the flag
        GCHeap::GetGCHeap()->GetFinalizerThread()->ResetSyncBlockCleanup();

        // walk the cleanup list and cleanup 'em up
        while ((psb = GetNextCleanupSyncBlock()) != NULL)
        {

            // Delete the sync block.
            DeleteSyncBlock(psb);
            psb = NULL;

            // pulse GC mode to allow GC to perform its work
            if (GCHeap::GetGCHeap()->GetFinalizerThread()->CatchAtSafePoint())
            {
                GCHeap::GetGCHeap()->GetFinalizerThread()->PulseGCMode();
            }
        }
        
    }
    EE_FINALLY
    {
        // We are finished cleaning up the sync blocks.
        m_bSyncBlockCleanupInProgress = FALSE;


        if (psb)
            DeleteSyncBlock(psb);
    } EE_END_FINALLY;
}

VOID SyncBlockCache::CleanupSyncBlocksInAppDomain(AppDomain *pDomain)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

#ifndef DACCESS_COMPILE
    _ASSERTE(IsFinalizerThread());
    
    ADIndex index = pDomain->GetIndex();


    // Make sure we dont race with anybody updating the table
    BOOL bNeedToRetry;
    do
    {
        DWORD maxIndex;

        {            
            SyncBlockCache::LockHolder lh(SyncBlockCache::GetSyncBlockCache());
            maxIndex = m_FreeSyncTableIndex;
        }
        BOOL bModifiedCleanupList=FALSE;
        bNeedToRetry=FALSE;
        STRESS_LOG1(LF_APPDOMAIN, LL_INFO100, "To cleanup - %d sync blocks", maxIndex);
        DWORD nb;
        for (nb = 1; nb < maxIndex; nb++)
        {
            // This is a check for syncblocks that were already cleaned up.
            if ((size_t)SyncTableEntry::GetSyncTableEntry()[nb].m_Object & 1)
            {
                continue;
            }

            // If the syncblock pointer is invalid, nothing more we can do.
            SyncBlock *pSyncBlock = SyncTableEntry::GetSyncTableEntry()[nb].m_SyncBlock;
            if (!pSyncBlock)
            {
                continue;
            }
            
            // If we happen to have a CCW living in the AppDomain being cleaned, then we need to neuter it.
            //  We do this check early because we have to neuter CCWs for agile objects as well.
            //  Neutering the object simply means we disconnect the object from the CCW so it can no longer
            //  be used.  When its ref-count falls to zero, it gets cleaned up.

            // NOTE: this will only notify the sync block if it is non-agile and living in the unloading domain.
            //  Agile objects that are still alive will not get notification!
            if (pSyncBlock->GetAppDomainIndex() == index)
            {
                pSyncBlock->OnADUnload();
            }
        }
        STRESS_LOG1(LF_APPDOMAIN, LL_INFO100, "AD cleanup - %d sync blocks done", nb);
        // Make sure nobody decreased m_FreeSyncTableIndex behind our back (we would read
        // off table limits)
        _ASSERTE(maxIndex <= m_FreeSyncTableIndex);

        if (bModifiedCleanupList)
            GetThread()->SetSyncBlockCleanup();
#ifdef _DEBUG
        else
            _ASSERTE(!bNeedToRetry);
#endif        

        while (GetThread()->RequireSyncBlockCleanup()) //we also might have something in the cleanup list
            CleanupSyncBlocks();
          
    }
    while(bNeedToRetry);
    
    
#endif
}


// create the sync block cache
/* static */
void SyncBlockCache::Attach()
{
    LEAF_CONTRACT;
}

// destroy the sync block cache
void SyncBlockCache::DoDetach()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    Object *pObj;
    ObjHeader  *pHeader;


    // Ensure that all the critical sections are released.  This is particularly
    // important in DEBUG, because all critical sections are threaded onto a global
    // list which would otherwise be corrupted.
    for (DWORD i=0; i<m_FreeSyncTableIndex; i++)
        if (((size_t)SyncTableEntry::GetSyncTableEntry()[i].m_Object & 1) == 0)
            if (SyncTableEntry::GetSyncTableEntry()[i].m_SyncBlock)
            {
                //
                //
                //
                //
                //

                // Make sure this gets updated because the finalizer thread & others
                // will continue to run for a short while more during our shutdown.
                pObj = SyncTableEntry::GetSyncTableEntry()[i].m_Object;
                pHeader = pObj->GetHeader();

                pHeader->EnterSpinLock();
                ADIndex appDomainIndex = pHeader->GetAppDomainIndex();
                if (! appDomainIndex.m_dwIndex)
                {
                    SyncBlock* syncBlock = pObj->PassiveGetSyncBlock();
                    if (syncBlock)
                        appDomainIndex = syncBlock->GetAppDomainIndex();
                }

                pHeader->ResetIndex();

                if (appDomainIndex.m_dwIndex)
                {
                    pHeader->SetIndex(appDomainIndex.m_dwIndex<<SBLK_APPDOMAIN_SHIFT);
                }
                pHeader->ReleaseSpinLock();

                SyncTableEntry::GetSyncTableEntry()[i].m_Object = (Object *)(m_FreeSyncTableList | 1);
                m_FreeSyncTableList = i << 1;

                DeleteSyncBlock(SyncTableEntry::GetSyncTableEntry()[i].m_SyncBlock);
            }
}

// destroy the sync block cache
/* static */
void SyncBlockCache::Detach()
{
    SyncBlockCache::GetSyncBlockCache()->DoDetach();
}


// create the sync block cache
/* static */
void SyncBlockCache::Start()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    DWORD* bm = new DWORD [BitMapSize(SYNC_TABLE_INITIAL_SIZE+1)];

    memset (bm, 0, BitMapSize (SYNC_TABLE_INITIAL_SIZE+1)*sizeof(DWORD));

    SyncTableEntry::GetSyncTableEntry() = new SyncTableEntry[SYNC_TABLE_INITIAL_SIZE+1];

    SyncTableEntry::GetSyncTableEntry()[0].m_SyncBlock = 0;
    SyncBlockCache::GetSyncBlockCache() = new (&g_SyncBlockCacheInstance) SyncBlockCache;

    SyncBlockCache::GetSyncBlockCache()->m_EphemeralBitmap = bm;
}


// destroy the sync block cache
/* static */
void SyncBlockCache::Stop()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    // cache must be destroyed first, since it can traverse the table to find all the
    // sync blocks which are live and thus must have their critical sections destroyed.
    if (SyncBlockCache::GetSyncBlockCache())
    {
        delete SyncBlockCache::GetSyncBlockCache();
        SyncBlockCache::GetSyncBlockCache() = 0;
    }

    if (SyncTableEntry::GetSyncTableEntry())
    {
        delete SyncTableEntry::GetSyncTableEntry();
        SyncTableEntry::GetSyncTableEntry() = 0;
    }
}


void    SyncBlockCache::InsertCleanupSyncBlock(SyncBlock* psb)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    // free up the threads that are waiting before we use the link
    // for other purposes
    if (psb->m_Link.m_pNext != NULL)
    {
        while (ThreadQueue::DequeueThread(psb) != NULL)
            continue;
    }


    // This method will be called only by the GC thread
    // we don't need to lock here
    //EnterCacheLock();

    psb->m_Link.m_pNext = m_pCleanupBlockList;
    m_pCleanupBlockList = &psb->m_Link;

    // we don't need a lock here
    //LeaveCacheLock();
}

SyncBlock* SyncBlockCache::GetNextCleanupSyncBlock()
{
    LEAF_CONTRACT;

    // we don't need a lock here,
    // as this is called only on the finalizer thread currently

    SyncBlock       *psb = NULL;
    if (m_pCleanupBlockList)
    {
        // get the actual sync block pointer
        psb = (SyncBlock *) (((BYTE *) m_pCleanupBlockList) - offsetof(SyncBlock, m_Link));
        m_pCleanupBlockList = m_pCleanupBlockList->m_pNext;
    }
    return psb;
}


// returns and removes the next free syncblock from the list
// the cache lock must be entered to call this
SyncBlock *SyncBlockCache::GetNextFreeSyncBlock()
{
    CONTRACTL
    {
        INJECT_FAULT(COMPlusThrowOM());
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

#ifdef _DEBUG  // Instrumentation for OOM fault injection testing
    delete new char;
#endif

    SyncBlock       *psb;
    SLink           *plst = m_FreeBlockList;

    COUNTER_ONLY(GetPrivatePerfCounters().m_GC.cSinkBlocks ++);
    m_ActiveCount++;

    if (plst)
    {
        m_FreeBlockList = m_FreeBlockList->m_pNext;

        // shouldn't be 0
        m_FreeCount--;

        // get the actual sync block pointer
        psb = (SyncBlock *) (((BYTE *) plst) - offsetof(SyncBlock, m_Link));

        return psb;
    }
    else
    {
        if ((m_SyncBlocks == NULL) || (m_FreeSyncBlock >= MAXSYNCBLOCK))
        {
#ifdef DUMP_SB
//            LogSpewAlways("Allocating new syncblock array\n");
//            DumpSyncBlockCache();
#endif
            SyncBlockArray* newsyncblocks = new(SyncBlockArray);
            if (!newsyncblocks)
                COMPlusThrowOM ();

            newsyncblocks->m_Next = m_SyncBlocks;
            m_SyncBlocks = newsyncblocks;
            m_FreeSyncBlock = 0;
        }
        return &(((SyncBlock*)m_SyncBlocks->m_Blocks)[m_FreeSyncBlock++]);
    }

}


DWORD SyncBlockCache::NewSyncBlockSlot(Object *obj)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        if (GetThread()) {MODE_COOPERATIVE;} else {MODE_ANY;}
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    DWORD indexNewEntry;
    if (m_FreeSyncTableList)
    {
        indexNewEntry = (DWORD)(m_FreeSyncTableList >> 1);
        _ASSERTE ((size_t)SyncTableEntry::GetSyncTableEntry()[indexNewEntry].m_Object & 1);
        m_FreeSyncTableList = (size_t)SyncTableEntry::GetSyncTableEntry()[indexNewEntry].m_Object & ~1;
    }
    else if ((indexNewEntry = (DWORD)(m_FreeSyncTableIndex)) >= m_SyncTableSize)
    {
        STRESS_LOG0(LF_SYNC, LL_INFO10000, "SyncBlockCache::NewSyncBlockSlot growing SyncBlockCache \n");
        
        NewArrayHolder<SyncTableEntry> newSyncTable (NULL);
        NewArrayHolder<DWORD>          newBitMap    (NULL);
        DWORD*         oldBitMap;

        // Compute the size of the new synctable. Normally, we double it - unless
        // doing so would create slots with indices too high to fit within the
        // mask. If so, we create a synctable up to the mask limit. If we're
        // already at the mask limit, then caller is out of luck.
        DWORD newSyncTableSize;
        if (m_SyncTableSize <= (MASK_SYNCBLOCKINDEX >> 1))
        {
            newSyncTableSize = m_SyncTableSize * 2;
        }
        else
        {
            newSyncTableSize = MASK_SYNCBLOCKINDEX;
        }

        if (!(newSyncTableSize > m_SyncTableSize)) // Make sure we actually found room to grow!
        {
            COMPlusThrowOM();
        }

        newSyncTable = new(SyncTableEntry[newSyncTableSize]);
        newBitMap = new(DWORD[BitMapSize (newSyncTableSize)]);


        {
            //! From here on, we assume that we will succeed and start doing global side-effects.
            //! Any operation that could fail must occur before this point.
            CANNOTTHROWCOMPLUSEXCEPTION();
            FAULT_FORBID();

            newSyncTable.SuppressRelease();
            newBitMap.SuppressRelease();


            // We chain old table because we can't delete
            // them before all the threads are stoppped
            // (next GC)
            SyncTableEntry::GetSyncTableEntry() [0].m_Object = (Object *)m_OldSyncTables;
            m_OldSyncTables = SyncTableEntry::GetSyncTableEntry();

            memset (newSyncTable, 0, newSyncTableSize*sizeof (SyncTableEntry));
            memset (newBitMap, 0, BitMapSize (newSyncTableSize)*sizeof (DWORD));
            CopyMemory (newSyncTable, SyncTableEntry::GetSyncTableEntry(),
                        m_SyncTableSize*sizeof (SyncTableEntry));

            CopyMemory (newBitMap, m_EphemeralBitmap,
                        BitMapSize (m_SyncTableSize)*sizeof (DWORD));

            oldBitMap = m_EphemeralBitmap;
            m_EphemeralBitmap = newBitMap;
            delete[] oldBitMap;

            _ASSERTE((m_SyncTableSize & MASK_SYNCBLOCKINDEX) == m_SyncTableSize);
            FastInterlockExchangePointer((void**)&SyncTableEntry::GetSyncTableEntry(),newSyncTable);

            m_FreeSyncTableIndex++;

            m_SyncTableSize = newSyncTableSize;

#ifdef _DEBUG
            static int dumpSBOnResize = -1;

            if (dumpSBOnResize == -1)
                dumpSBOnResize = g_pConfig->GetConfigDWORD(L"SBDumpOnResize", 0);

            if (dumpSBOnResize)
            {
                LogSpewAlways("SyncBlockCache resized\n");
                DumpSyncBlockCache();
            }
#endif
        }
    }
    else
    {
#ifdef _DEBUG
        static int dumpSBOnNewIndex = -1;

        if (dumpSBOnNewIndex == -1)
            dumpSBOnNewIndex = g_pConfig->GetConfigDWORD(L"SBDumpOnNewIndex", 0);

        if (dumpSBOnNewIndex)
        {
            LogSpewAlways("SyncBlockCache index incremented\n");
            DumpSyncBlockCache();
        }
#endif
        m_FreeSyncTableIndex ++;
    }


    CardTableSetBit (indexNewEntry);

    SyncTableEntry::GetSyncTableEntry() [indexNewEntry].m_Object = obj;
    SyncTableEntry::GetSyncTableEntry() [indexNewEntry].m_SyncBlock = NULL;

    _ASSERTE(indexNewEntry != 0);

    return indexNewEntry;
}


// free a used sync block
void SyncBlockCache::DeleteSyncBlock(SyncBlock *psb)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;



    // Destruct the SyncBlock, but don't reclaim its memory.  (Overridden
    // operator delete).
    delete psb;

    //synchronizer with the consumers,
    {
        SyncBlockCache::LockHolder lh(this);

        DeleteSyncBlockMemory(psb);
    }
}


// returns the sync block memory to the free pool but does not destruct sync block (must own cache lock already)
void    SyncBlockCache::DeleteSyncBlockMemory(SyncBlock *psb)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    COUNTER_ONLY(GetPrivatePerfCounters().m_GC.cSinkBlocks --);

    m_ActiveCount--;
    m_FreeCount++;

    psb->m_Link.m_pNext = m_FreeBlockList;
    m_FreeBlockList = &psb->m_Link;

}

// free a used sync block
void SyncBlockCache::GCDeleteSyncBlock(SyncBlock *psb)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Destruct the SyncBlock, but don't reclaim its memory.  (Overridden
    // operator delete).
    delete psb;

    COUNTER_ONLY(GetPrivatePerfCounters().m_GC.cSinkBlocks --);


    m_ActiveCount--;
    m_FreeCount++;

    psb->m_Link.m_pNext = m_FreeBlockList;
    m_FreeBlockList = &psb->m_Link;
}

void SyncBlockCache::GCWeakPtrScan(HANDLESCANPROC scanProc, LPARAM lp1, LPARAM lp2)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;


    // First delete the obsolete arrays since we have exclusive access
    BOOL fSetSyncBlockCleanup = FALSE;

    SyncTableEntry* arr;
    while ((arr = m_OldSyncTables) != NULL)
    {
        m_OldSyncTables = (SyncTableEntry*)arr[0].m_Object;
        delete arr;
    }

#ifdef DUMP_SB
    LogSpewAlways("GCWeakPtrScan starting\n");
#endif

   if (g_pConfig->GetHeapVerifyLevel()& EEConfig::HEAPVERIFY_SYNCBLK)
       STRESS_LOG0 (LF_GC | LF_SYNC, LL_INFO100, "GCWeakPtrScan starting\n");

   if (GCHeap::GetGCHeap()->GetCondemnedGeneration() < GCHeap::GetGCHeap()->GetMaxGeneration())
   {
#ifdef VERIFY_HEAP
        DWORD freeSyncTalbeIndexCopy = m_FreeSyncTableIndex;
        SyncTableEntry * syncTableShadow = NULL;
        if ((g_pConfig->GetHeapVerifyLevel()& EEConfig::HEAPVERIFY_SYNCBLK) && !((ScanContext*)lp1)->promotion)
        {
            syncTableShadow = new(nothrow) SyncTableEntry [m_FreeSyncTableIndex];
            if (syncTableShadow)
            {
                memcpy (syncTableShadow, SyncTableEntry::GetSyncTableEntry(), m_FreeSyncTableIndex * sizeof (SyncTableEntry));                
            }
        }
#endif //VERIFY_HEAP

        //scan the bitmap
        size_t dw = 0;
        while (1)
        {
            while (dw < BitMapSize (m_SyncTableSize) && (m_EphemeralBitmap[dw]==0))
            {
                dw++;
            }
            if (dw < BitMapSize (m_SyncTableSize))
            {
                //found one
                for (int i = 0; i < card_word_width; i++)
                {
                    size_t card = i+dw*card_word_width;
                    if (CardSetP (card))
                    {
                        BOOL clear_card = TRUE;
                        for (int idx = 0; idx < card_size; idx++)
                        {
                            size_t nb = CardIndex (card) + idx;
                            if (( nb < m_FreeSyncTableIndex) && (nb > 0))
                            {
                                Object* o = SyncTableEntry::GetSyncTableEntry()[nb].m_Object;
                                if (o && !((size_t)o & 1))
                                {
                                    if (GCHeap::GetGCHeap()->IsEphemeral (o))
                                    {
                                        clear_card = FALSE;

                                        GCWeakPtrScanElement ((int)nb, scanProc,
                                                              lp1, lp2, fSetSyncBlockCleanup);
                                    }
                                }
                            }
                        }
                        if (clear_card)
                            ClearCard (card);
                    }
                }
                dw++;
            }
            else
                break;
        }
        
#ifdef VERIFY_HEAP
        if (g_pConfig->GetHeapVerifyLevel()& EEConfig::HEAPVERIFY_SYNCBLK)
        {
            if (syncTableShadow)
            {
                for (DWORD nb = 1; nb < m_FreeSyncTableIndex; nb++)
                {
                    Object **keyv = (Object **) &syncTableShadow[nb].m_Object;

                    if (((size_t) *keyv & 1) == 0)
                    {
                        (*scanProc) (keyv, NULL, lp1, lp2);
                        SyncBlock   *pSB = syncTableShadow[nb].m_SyncBlock;
                        if (*keyv != 0 && (!pSB || !pSB->IsIDisposable()))
                        {
                            if (syncTableShadow[nb].m_Object != SyncTableEntry::GetSyncTableEntry()[nb].m_Object)
                                DebugBreak ();
                        }
                    }
                } 
                delete []syncTableShadow;
                syncTableShadow = NULL;
            }
            if (freeSyncTalbeIndexCopy != m_FreeSyncTableIndex)
                DebugBreak ();
        }
#endif //VERIFY_HEAP

    }
    else
    {
        for (DWORD nb = 1; nb < m_FreeSyncTableIndex; nb++)
        {
            GCWeakPtrScanElement (nb, scanProc, lp1, lp2, fSetSyncBlockCleanup);
        }


    }

    if (fSetSyncBlockCleanup)
    {
        // mark the finalizer thread saying requires cleanup
        GCHeap::GetGCHeap()->GetFinalizerThread()->SetSyncBlockCleanup();
        GCHeap::GetGCHeap()->EnableFinalization();
    }

#if defined(VERIFY_HEAP)
    if (g_pConfig->GetHeapVerifyLevel() & EEConfig::HEAPVERIFY_GC)
    {
        if (((ScanContext*)lp1)->promotion)
        {

            for (int nb = 1; nb < (int)m_FreeSyncTableIndex; nb++)
            {
                Object* o = SyncTableEntry::GetSyncTableEntry()[nb].m_Object;
                if (((size_t)o & 1) == 0)
                {
                    o->Validate();
                }
            }
        }
    }
#endif // VERIFY_HEAP
}

/* Scan the weak pointers in the SyncBlockEntry and report them to the GC.  If the
   reference is dead, then return TRUE */

BOOL SyncBlockCache::GCWeakPtrScanElement (int nb, HANDLESCANPROC scanProc, LPARAM lp1, LPARAM lp2,
                                           BOOL& cleanup)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    Object **keyv = (Object **) &SyncTableEntry::GetSyncTableEntry()[nb].m_Object;

#ifdef DUMP_SB
    char *name;
    PAL_TRY {
        if (! *keyv)
            name = "null";
        else if ((size_t) *keyv & 1)
            name = "free";
        else {
            name = (*keyv)->GetClass()->GetDebugClassName();
            if (strlen(name) == 0)
                name = "<INVALID>";
        }
    } PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
        name = "<INVALID>";
    }
    PAL_ENDTRY
    LogSpewAlways("[%4.4d]: %8.8x, %s\n", nb, *keyv, name);
#endif

    if (((size_t) *keyv & 1) == 0)
    {
        if (g_pConfig->GetHeapVerifyLevel () & EEConfig::HEAPVERIFY_SYNCBLK)
        {
            STRESS_LOG3 (LF_GC | LF_SYNC, LL_INFO100000, "scanning syncblk[%d, %p, %p]\n", nb, (size_t)SyncTableEntry::GetSyncTableEntry()[nb].m_SyncBlock, (size_t)*keyv);

        }

        (*scanProc) (keyv, NULL, lp1, lp2);
        SyncBlock   *pSB = SyncTableEntry::GetSyncTableEntry()[nb].m_SyncBlock;
        if ((*keyv == 0 ) || (pSB && pSB->IsIDisposable()))
        {
            if (g_pConfig->GetHeapVerifyLevel () & EEConfig::HEAPVERIFY_SYNCBLK)
            {
                STRESS_LOG3 (LF_GC | LF_SYNC, LL_INFO100000, "freeing syncblk[%d, %p, %p]\n", nb, (size_t)pSB, (size_t)*keyv);

            }
            if (*keyv)
            {
                _ASSERTE (pSB);
                GCDeleteSyncBlock(pSB);
                //clean the object syncblock header
                ((Object*)(*keyv))->GetHeader()->GCResetIndex();
            }
            else if (pSB)
            {

                cleanup = TRUE;
                // insert block into cleanup list
                InsertCleanupSyncBlock (SyncTableEntry::GetSyncTableEntry()[nb].m_SyncBlock);
#ifdef DUMP_SB
                LogSpewAlways("       Cleaning up block at %4.4d\n", nb);
#endif
            }

            // delete the entry
#ifdef DUMP_SB
            LogSpewAlways("       Deleting block at %4.4d\n", nb);
#endif
            SyncTableEntry::GetSyncTableEntry()[nb].m_Object = (Object *)(m_FreeSyncTableList | 1);
            m_FreeSyncTableList = nb << 1;
            return TRUE;
        }
        else
        {
#ifdef DUMP_SB
            LogSpewAlways("       Keeping block at %4.4d with oref %8.8x\n", nb, *keyv);
#endif
        }
    }
    return FALSE;
}

void SyncBlockCache::GCDone(BOOL demoting, int max_gen)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (demoting && 
        (GCHeap::GetGCHeap()->GetCondemnedGeneration() == 
         GCHeap::GetGCHeap()->GetMaxGeneration()))
    {
        //scan the bitmap
        size_t dw = 0;
        while (1)
        {
            while (dw < BitMapSize (m_SyncTableSize) && 
                   (m_EphemeralBitmap[dw]==(DWORD)~0))
            {
                dw++;
            }
            if (dw < BitMapSize (m_SyncTableSize))
            {
                //found one
                for (int i = 0; i < card_word_width; i++)
                {
                    size_t card = i+dw*card_word_width;
                    if (!CardSetP (card))
                    {
                        for (int idx = 0; idx < card_size; idx++)
                        {
                            size_t nb = CardIndex (card) + idx;
                            if (( nb < m_FreeSyncTableIndex) && (nb > 0))
                            {
                                Object* o = SyncTableEntry::GetSyncTableEntry()[nb].m_Object;
                                if (o && !((size_t)o & 1))
                                {
                                    if (GCHeap::GetGCHeap()->WhichGeneration (o) < (unsigned int)max_gen)
                                    {
                                        SetCard (card);
                                        break;

                                    }
                                }
                            }
                        }
                    }
                }
                dw++;
            }
            else
                break;
        }
    }
}


#if defined (VERIFY_HEAP)

#ifndef _DEBUG
#ifdef _ASSERTE
#undef _ASSERTE
#endif
#define _ASSERTE(c) if (!(c)) DebugBreak()
#endif

void SyncBlockCache::VerifySyncTableEntry()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    for (DWORD nb = 1; nb < m_FreeSyncTableIndex; nb++)
    {
        Object* o = SyncTableEntry::GetSyncTableEntry()[nb].m_Object;
        // if the slot was just allocated, the object may still be null
        if (o && (((size_t)o & 1) == 0)) 
        {
            //there is no need to verify next object's header because this is called
            //from verify_heap, which will verify every object anyway
            o->Validate(TRUE, FALSE);

            //
            // This loop is just a heuristic to try to catch errors, but it is not 100%.
            // To prevent false positives, we weaken our assert below to exclude the case
            // where the index is still NULL, but we've reached the end of our loop.
            //
            static const DWORD max_iterations = 100;
            DWORD loop = 0;
            
            for (; loop < max_iterations; loop++)
            {
                // The syncblock index may be updating by another thread.
                if (o->GetHeader()->GetHeaderSyncBlockIndex() != 0)
                {
                    break;
                }
                __SwitchToThread(0);
            }
            
            DWORD idx = o->GetHeader()->GetHeaderSyncBlockIndex();
            _ASSERTE(idx == nb || ((0 == idx) && (loop == max_iterations)));
            _ASSERTE(!GCHeap::GetGCHeap()->IsEphemeral(o) || CardSetP(CardOf(nb)));
        }
    }
}

#ifndef _DEBUG
#undef _ASSERTE
#define _ASSERTE(expr) ((void)0)
#endif   // _DEBUG

#endif // VERIFY_HEAP

#if CHECK_APP_DOMAIN_LEAKS
void SyncBlockCache::CheckForUnloadedInstances(ADIndex unloadingIndex)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Can only do in leak mode because agile objects will be in the domain with
    // their index set to their creating domain and check will fail.
    if (! g_pConfig->AppDomainLeaks())
        return;

    for (DWORD nb = 1; nb < m_FreeSyncTableIndex; nb++)
    {
        SyncTableEntry *pEntry = &SyncTableEntry::GetSyncTableEntry()[nb];
        Object *oref = (Object *) pEntry->m_Object;
        if (((size_t) oref & 1) != 0)
            continue;

        ADIndex idx;
        if (oref)
            idx = pEntry->m_Object->GetHeader()->GetRawAppDomainIndex();
        if (! idx.m_dwIndex && pEntry->m_SyncBlock)
            idx = pEntry->m_SyncBlock->GetAppDomainIndex();
        // if the following assert fires, someobody is holding a reference to an object in an unloaded appdomain
        if (idx == unloadingIndex)
        {
            // object must be agile to have survived the unload. If can't make it agile, that's a bug            
            if (!oref->TrySetAppDomainAgile(TRUE))
                _ASSERTE(!"Detected instance of unloaded appdomain that survived GC\n");
        }
    }
}
#endif

#ifdef _DEBUG

void DumpSyncBlockCache()
{
    STATIC_CONTRACT_NOTHROW;

    SyncBlockCache *pCache = SyncBlockCache::GetSyncBlockCache();

    LogSpewAlways("Dumping SyncBlockCache size %d\n", pCache->m_FreeSyncTableIndex);

    static int dumpSBStyle = -1;
    if (dumpSBStyle == -1)
        dumpSBStyle = g_pConfig->GetConfigDWORD(L"SBDumpStyle", 0);
    if (dumpSBStyle == 0)
        return;

    BOOL isString = FALSE;
    DWORD objectCount = 0;
    DWORD slotCount = 0;

    for (DWORD nb = 1; nb < pCache->m_FreeSyncTableIndex; nb++)
    {
        isString = FALSE;
        char buffer[1024], buffer2[1024];
        LPCUTF8 descrip = "null";
        SyncTableEntry *pEntry = &SyncTableEntry::GetSyncTableEntry()[nb];
        Object *oref = (Object *) pEntry->m_Object;
        if (((size_t) oref & 1) != 0)
        {
            descrip = "free";
            oref = 0;
        }
        else
        {
            ++slotCount;
            if (oref)
            {
                ++objectCount;
                PAL_TRY
                {
                    descrip = oref->GetMethodTable()->GetDebugClassName();
                    if (strlen(descrip) == 0)
                        descrip = "<INVALID>";
                    else if (oref->GetMethodTable() == g_pStringClass)
                    {
                        sprintf_s(buffer2, COUNTOF(buffer2), "%s (%S)", descrip, ObjectToSTRINGREF((StringObject*)oref)->GetBuffer());
                        descrip = buffer2;
                        isString = TRUE;
                    }
                }
                PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
                    descrip = "<INVALID>";
                }
                PAL_ENDTRY
            }
            ADIndex idx;
            if (oref)
                idx = pEntry->m_Object->GetHeader()->GetRawAppDomainIndex();
            if (! idx.m_dwIndex && pEntry->m_SyncBlock)
                idx = pEntry->m_SyncBlock->GetAppDomainIndex();
            if (idx.m_dwIndex && ! SystemDomain::System()->TestGetAppDomainAtIndex(idx))
            {
                sprintf_s(buffer, COUNTOF(buffer), "** unloaded (%3.3x) %s", idx.m_dwIndex, descrip);
                descrip = buffer;
            }
            else
            {
                sprintf_s(buffer, COUNTOF(buffer), "(AD %3.3x) %s", idx.m_dwIndex, descrip);
                descrip = buffer;
            }
        }
        if (dumpSBStyle < 2)
            LogSpewAlways("[%4.4d]: %8.8x %s\n", nb, oref, descrip);
        else if (dumpSBStyle == 2 && ! isString)
            LogSpewAlways("[%4.4d]: %s\n", nb, descrip);
    }
    LogSpewAlways("Done dumping SyncBlockCache used slots: %d, objects: %d\n", slotCount, objectCount);
}
#endif

// ***************************************************************************
//
//              ObjHeader class implementation
//
// ***************************************************************************

// this enters the monitor of an object
void ObjHeader::EnterObjMonitor()
{
    WRAPPER_CONTRACT;
    GetSyncBlock()->EnterMonitor();
}

// Non-blocking version of above
BOOL ObjHeader::TryEnterObjMonitor(INT32 timeOut)
{
    WRAPPER_CONTRACT;
    return GetSyncBlock()->TryEnterMonitor(timeOut);
}

BOOL ObjHeader::LeaveObjMonitor()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    //this function switch to preemp mode so we need to protect the object in some path
    OBJECTREF thisObj = ObjectToOBJECTREF (GetBaseObject ());
    for (;;)
    {
        AwareLock::LeaveHelperAction action = thisObj->GetHeader ()->LeaveObjMonitorHelper(GetThread());

        switch(action)
        {
        case AwareLock::LeaveHelperAction_None:
            // We are done
            return TRUE;
        case AwareLock::LeaveHelperAction_Signal:
            {
                // Signal the event
                SyncBlock *psb = thisObj->GetHeader ()->PassiveGetSyncBlock();
                if (psb != NULL)
                    psb->QuickGetMonitor()->Signal();
            }
            return TRUE;
        case AwareLock::LeaveHelperAction_Yield:
            YieldProcessor();
            continue;
        case AwareLock::LeaveHelperAction_Contention:
            // Some thread is updating the syncblock value.
            {
                //protect the object before switching mode
                GCPROTECT_BEGIN (thisObj);
                GCX_PREEMP();
                __SwitchToThread(0);
                GCPROTECT_END ();
            }
            continue;
        default:
            // Must be an error otherwise - ignore it
            _ASSERTE(action == AwareLock::LeaveHelperAction_Error);
            return FALSE;
        }
    }
}

// The only difference between LeaveObjMonitor and LeaveObjMonitorAtException is switch 
// to preemptive mode around __SwitchToThread
BOOL ObjHeader::LeaveObjMonitorAtException()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    for (;;)
    {
        AwareLock::LeaveHelperAction action = LeaveObjMonitorHelper(GetThread());

        switch(action)
        {
        case AwareLock::LeaveHelperAction_None:
            // We are done
            return TRUE;
        case AwareLock::LeaveHelperAction_Signal:
            {
                // Signal the event
                SyncBlock *psb = PassiveGetSyncBlock();
                if (psb != NULL)
                    psb->QuickGetMonitor()->Signal();
            }
            return TRUE;
        case AwareLock::LeaveHelperAction_Yield:
            YieldProcessor();
            continue;
        case AwareLock::LeaveHelperAction_Contention:
            // Some thread is updating the syncblock value.
            //
            // We never toggle GC mode while holding the spinlock (BeginNoTriggerGC/EndNoTriggerGC 
            // in EnterSpinLock/ReleaseSpinLock ensures it). Thus we do not need to switch to preemptive
            // while waiting on the spinlock.
            //
            {
                __SwitchToThread(0);
            }
            continue;
        default:
            // Must be an error otherwise - ignore it
            _ASSERTE(action == AwareLock::LeaveHelperAction_Error);
            return FALSE;
        }
    }
}

BOOL ObjHeader::IsObjMonitorOwnedByThread(Thread* pExpectedThread)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        if (!IsGCSpecialThread ()) {MODE_COOPERATIVE;} else {MODE_ANY;}
    }
    CONTRACTL_END;

    while (TRUE)
    {
        SyncBlock *psb = PassiveGetSyncBlock();
        if (psb != NULL)
        {
            return (pExpectedThread == psb->GetMonitor()->m_HoldingThread);
        }

        DWORD bits = GetBits();
        if (bits & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX)
        {
            if (bits & BIT_SBLK_IS_HASHCODE)
            {
                // This thread does not own the lock.
                return FALSE;
            }

            // Some thread may have created SyncBlock
            continue;
        }
        else
        {
            DWORD lockThreadId, recursionLevel;
            lockThreadId = bits & SBLK_MASK_LOCK_THREADID;
            recursionLevel = (bits & SBLK_MASK_LOCK_RECLEVEL) >> SBLK_RECLEVEL_SHIFT;
            //if thread ID is 0, recursionLevel got to be zero
            //but thread ID doesn't have to be valid because the lock could be orphanend
            _ASSERTE (lockThreadId != 0 || recursionLevel == 0 );

            return (pExpectedThread->GetThreadId() == lockThreadId);
        }
    }
}

void ObjHeader::EnterSpinLock()
{
    LEAF_CONTRACT;
#ifdef _DEBUG
    int i = 0;
#endif

    while (TRUE)
    {
#ifdef _DEBUG
        if (i++ > 10000)
            _ASSERTE(!"ObjHeader::EnterLock timed out");
#endif
        // get the value so that it doesn't get changed under us.
        // Must cast through volatile to ensure the lock is refetched from memory.
        LONG curValue = *(volatile LONG*)&m_SyncBlockValue;

        // check if lock taken
        if (! (curValue & BIT_SBLK_SPIN_LOCK))
        {
            // try to take the lock
            LONG newValue = curValue | BIT_SBLK_SPIN_LOCK;
            LONG result = FastInterlockCompareExchange((LONG*)&m_SyncBlockValue, newValue, curValue);
            if (result == curValue)
                break;
        }
        __SwitchToThread(0);
    }

    INCONTRACT(Thread* pThread = GetThread());
    INCONTRACT(if (pThread != NULL) pThread->BeginNoTriggerGC(__FILE__, __LINE__));
}

void ObjHeader::ReleaseSpinLock()
{
    LEAF_CONTRACT;

    INCONTRACT(Thread* pThread = GetThread());
    INCONTRACT(if (pThread != NULL) pThread->EndNoTriggerGC());

    FastInterlockAnd(&m_SyncBlockValue, ~BIT_SBLK_SPIN_LOCK);
}

ADIndex ObjHeader::GetRawAppDomainIndex()
{
    LEAF_CONTRACT;

    // pull the value out before checking it to avoid race condition
    DWORD value = m_SyncBlockValue;
    if ((value & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX) == 0)
        return ADIndex((value >> SBLK_APPDOMAIN_SHIFT) & SBLK_MASK_APPDOMAININDEX);
    return ADIndex(0);
}

ADIndex ObjHeader::GetAppDomainIndex()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    ADIndex indx = GetRawAppDomainIndex();
    if (indx.m_dwIndex)
        return indx;
    SyncBlock* syncBlock = GetBaseObject()->PassiveGetSyncBlock();
    if (! syncBlock)
        return ADIndex(0);

    return syncBlock->GetAppDomainIndex();
}

void ObjHeader::SetAppDomainIndex(ADIndex indx)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    //
    // This should only be called during the header initialization,
    // so don't worry about races.
    //

    BOOL done = FALSE;

#ifdef _DEBUG
    static int forceSB = -1;

    if (forceSB == -1)
        forceSB = g_pConfig->GetConfigDWORD(L"ADForceSB", 0);

    if (forceSB)
        // force a synblock so we get one for every object.
        GetSyncBlock();
#endif

    if (GetHeaderSyncBlockIndex() == 0 && indx.m_dwIndex < SBLK_MASK_APPDOMAININDEX)
    {
        EnterSpinLock();
        //Try one more time
        if (GetHeaderSyncBlockIndex() == 0)
        {
            _ASSERTE(GetRawAppDomainIndex().m_dwIndex == 0);
            // can store it in the object header
            FastInterlockOr(&m_SyncBlockValue, indx.m_dwIndex << SBLK_APPDOMAIN_SHIFT);
            done = TRUE;
        }
        ReleaseSpinLock();
    }

    if (!done)
    {
        // must create a syncblock entry and store the appdomain indx there
        SyncBlock *psb = GetSyncBlock();
        _ASSERTE(psb);
        psb->SetAppDomainIndex(indx);
    }
}

void ObjHeader::ResetAppDomainIndex(ADIndex indx)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    //
    // This should only be called during the header initialization,
    // so don't worry about races.
    //

    BOOL done = FALSE;

    if (GetHeaderSyncBlockIndex() == 0 && indx.m_dwIndex < SBLK_MASK_APPDOMAININDEX)
    {
        EnterSpinLock();
        //Try one more time
        if (GetHeaderSyncBlockIndex() == 0)
        {
            // can store it in the object header
            while (TRUE)
            {
                DWORD oldValue = m_SyncBlockValue;
                DWORD newValue = (oldValue & (~(SBLK_MASK_APPDOMAININDEX << SBLK_APPDOMAIN_SHIFT))) |
                    (indx.m_dwIndex << SBLK_APPDOMAIN_SHIFT);
                if (FastInterlockCompareExchange((LONG*)&m_SyncBlockValue,
                                                 newValue,
                                                 oldValue) == (LONG)oldValue)
                {
                    break;
                }
            }
            done = TRUE;
        }
        ReleaseSpinLock();
    }

    if (!done)
    {
        // must create a syncblock entry and store the appdomain indx there
        SyncBlock *psb = GetSyncBlock();
        _ASSERTE(psb);
        psb->SetAppDomainIndex(indx);
    }
}

void ObjHeader::ResetAppDomainIndexNoFailure(ADIndex indx)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(indx.m_dwIndex < SBLK_MASK_APPDOMAININDEX);
    }
    CONTRACTL_END;

    EnterSpinLock();
    if (GetHeaderSyncBlockIndex() == 0)
    {
        // can store it in the object header
        while (TRUE)
        {
            DWORD oldValue = m_SyncBlockValue;
            DWORD newValue = (oldValue & (~(SBLK_MASK_APPDOMAININDEX << SBLK_APPDOMAIN_SHIFT))) |
                (indx.m_dwIndex << SBLK_APPDOMAIN_SHIFT);
            if (FastInterlockCompareExchange((LONG*)&m_SyncBlockValue,
                                             newValue,
                                             oldValue) == (LONG)oldValue)
            {
                break;
            }
        }
    }
    else
    {
        SyncBlock *psb = PassiveGetSyncBlock();
        _ASSERTE(psb);
        psb->SetAppDomainIndex(indx);
    }
    ReleaseSpinLock();
}

DWORD ObjHeader::GetSyncBlockIndex()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    DWORD   indx;

    if ((indx = GetHeaderSyncBlockIndex()) == 0)
    {
        if (GetAppDomainIndex().m_dwIndex)
        {
            // if have an appdomain set then must create a sync block to store it
            GetSyncBlock();
        }
        else
        {
            //Need to get it from the cache
            SyncBlockCache::LockHolder lh(SyncBlockCache::GetSyncBlockCache());

            //Try one more time
            if (GetHeaderSyncBlockIndex() == 0)
            {
                EnterSpinLock();
                // Now the header will be stable - check whether hashcode, appdomain index or lock information is stored in it.
                DWORD bits = GetBits();
                if (((bits & (BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX | BIT_SBLK_IS_HASHCODE)) == (BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX | BIT_SBLK_IS_HASHCODE)) ||
                    ((bits & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX) == 0 &&
                     (bits & ((SBLK_MASK_APPDOMAININDEX<<SBLK_APPDOMAIN_SHIFT)|SBLK_MASK_LOCK_RECLEVEL|SBLK_MASK_LOCK_THREADID)) != 0))
                {
                    // Need a sync block to store this info
                    ReleaseSpinLock();
                    lh.Release();
                    GetSyncBlock();
                }
                else
                {
                    SetIndex(BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX | SyncBlockCache::GetSyncBlockCache()->NewSyncBlockSlot(GetBaseObject()));
                    ReleaseSpinLock();
                }
            }
            // SyncBlockCache::LockHolder goes out of scope here
        }
        if ((indx = GetHeaderSyncBlockIndex()) == 0)
            COMPlusThrowOM();
    }

    return indx;
}


#if defined (VERIFY_HEAP)

#define ASSERT_AND_CHECK(x) {_ASSERTE(x); if (!(x)) return FALSE;}

BOOL ObjHeader::Validate (BOOL bVerifySyncBlkIndex)
{
    WRAPPER_CONTRACT;
    
    DWORD bits = GetBits ();
    Object * obj = GetBaseObject ();
    BOOL bVerifyMore = g_pConfig->GetHeapVerifyLevel() & EEConfig::HEAPVERIFY_SYNCBLK;
    //the highest 2 bits have reloaded meaning
    //for string objects:
    //         BIT_SBLK_STRING_HAS_NO_HIGH_CHARS   0x80000000
    //         BIT_SBLK_STRING_HIGH_CHARS_KNOWN    0x40000000
    //         BIT_SBLK_STRING_HAS_SPECIAL_SORT    0xC0000000
    //for other objects:
    //         BIT_SBLK_AGILE_IN_PROGRESS          0x80000000
    //         BIT_SBLK_FINALIZER_RUN              0x40000000
    if (bits & BIT_SBLK_STRING_HIGH_CHAR_MASK)
    {
        if (obj->GetGCSafeMethodTable () == g_pStringClass)
        {
            if (bVerifyMore)
            {
                ASSERT_AND_CHECK (COMString::ValidateHighChars ((StringObject *)obj));
            }
        }
        else
        {
#if CHECK_APP_DOMAIN_LEAKS
            if (bVerifyMore)
            {  
                if (bits & BIT_SBLK_AGILE_IN_PROGRESS)
                {
                    BOOL fResult;
                    ASSERT_AND_CHECK (
                        //BIT_SBLK_AGILE_IN_PROGRESS is set only if the object needs to check appdomain agile
                        obj->ShouldCheckAppDomainAgile(FALSE, &fResult)
                        //before BIT_SBLK_AGILE_IN_PROGRESS is cleared, the object might already be marked as agile 
                        ||(obj->PassiveGetSyncBlock () && obj->PassiveGetSyncBlock ()->IsAppDomainAgile ())
                        ||(obj->PassiveGetSyncBlock () && obj->PassiveGetSyncBlock ()->IsCheckedForAppDomainAgile ())
                    );
                }
            }
#else //CHECK_APP_DOMAIN_LEAKS
            //BIT_SBLK_AGILE_IN_PROGRESS is set only in debug build
            ASSERT_AND_CHECK (!(bits & BIT_SBLK_AGILE_IN_PROGRESS));
#endif  //CHECK_APP_DOMAIN_LEAKS
            if (bits & BIT_SBLK_FINALIZER_RUN)
            {
                ASSERT_AND_CHECK (obj->GetGCSafeMethodTable ()->HasFinalizer ());
            }
        }
    }

    //BIT_SBLK_GC_RESERVE (0x20000000) is only set during GC. But for frozen object, we don't clean the bit
    if (bits & BIT_SBLK_GC_RESERVE)
    {
        ASSERT_AND_CHECK (GCHeap::GetGCHeap()->IsGCInProgress () 
            || GCHeap::GetGCHeap()->IsConcurrentGCInProgress ()
            //this check could be slow in concurrent GC case
            || GCHeap::GetGCHeap()->IsInFrozenSegment(obj));
    }

    //Don't know how to verify BIT_SBLK_SPIN_LOCK (0x10000000)
    
    //BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX (0x08000000)
    if (bits & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX)
    {
        //if BIT_SBLK_IS_HASHCODE (0x04000000) is not set, 
        //rest of the DWORD is SyncBlk Index
        if (!(bits & BIT_SBLK_IS_HASHCODE))
        {
            if (bVerifySyncBlkIndex  && CNameSpace::GetGcRuntimeStructuresValid ())
            {
                DWORD sbIndex = bits & MASK_SYNCBLOCKINDEX;
                ASSERT_AND_CHECK(SyncTableEntry::GetSyncTableEntry()[sbIndex].m_Object == obj);             
            }
        }

        else
        {
#ifdef _DEBUG
            //in debug build, BIT_SBLK_IS_FROZEN (0x02000000) is used for frozen objects
            if (bits & BIT_SBLK_IS_FROZEN)
            {
                //this check could be slow, espeically in concurrent GC
                if (bVerifyMore)
                {
                    ASSERT_AND_CHECK (GCHeap::GetGCHeap ()->IsInFrozenSegment (obj));
                }
            }
#endif //_DEBUG
        //  rest of the DWORD is a hash code and we don't have much to validate it
        }
    }
    else
    {
        //if BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX is clear, rest of DWORD is thin lock thread ID, 
        //thin lock recursion level and appdomain index
        DWORD lockThreadId = bits & SBLK_MASK_LOCK_THREADID;
        DWORD recursionLevel = (bits & SBLK_MASK_LOCK_RECLEVEL) >> SBLK_RECLEVEL_SHIFT;
        //if thread ID is 0, recursionLeve got to be zero
        //but thread ID doesn't have to be valid because the lock could be orphanend
        ASSERT_AND_CHECK (lockThreadId != 0 || recursionLevel == 0 );     

        DWORD adIndex  = (bits >> SBLK_APPDOMAIN_SHIFT) & SBLK_MASK_APPDOMAININDEX;
        if (adIndex!= 0)
        {
#ifndef _DEBUG            
            //in non debug build, only objects of domain neutral type have appdomain index in header
            ASSERT_AND_CHECK (obj->GetGCSafeMethodTable()->IsDomainNeutral());
#endif
        }
    }
    
    return TRUE;
}

#endif //VERIFY_HEAP

// This holder takes care of the SyncBlock memory cleanup if an OOM occurs inside a call to NewSyncBlockSlot.
//
// Warning: Assumes you already own the cache lock.
//          Assumes nothing allocated inside the SyncBlock (only releases the memory, does not destruct.)
//
// This holder really just meets GetSyncBlock()'s special needs. It's not a general purpose holder.


FORCEINLINE VOID VoidDeleteSyncBlockMemory(SyncBlock* psb)
{
    SyncBlockCache::GetSyncBlockCache()->DeleteSyncBlockMemory(psb);
}

typedef Wrapper<SyncBlock*, DoNothing<SyncBlock*>, VoidDeleteSyncBlockMemory, NULL> SyncBlockMemoryHolder;


// get the sync block for an existing object
SyncBlock *ObjHeader::GetSyncBlock()
{
    CONTRACT(SyncBlock *)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    SyncBlock* syncBlock = GetBaseObject()->PassiveGetSyncBlock();
    DWORD      indx = 0;
    BOOL indexHeld = FALSE;

    if (syncBlock)
    {
        // Has our backpointer been correctly updated through every GC?
        _ASSERTE(SyncTableEntry::GetSyncTableEntry()[GetHeaderSyncBlockIndex()].m_Object == GetBaseObject());
        RETURN syncBlock;
    }

    //Need to get it from the cache
    {
        SyncBlockCache::LockHolder lh(SyncBlockCache::GetSyncBlockCache());

        //Try one more time
        syncBlock = GetBaseObject()->PassiveGetSyncBlock();
        if (syncBlock)
            RETURN syncBlock;


        SyncBlockMemoryHolder syncBlockMemoryHolder(SyncBlockCache::GetSyncBlockCache()->GetNextFreeSyncBlock());
        syncBlock = syncBlockMemoryHolder;

        if ((indx = GetHeaderSyncBlockIndex()) == 0)
        {
            indx = SyncBlockCache::GetSyncBlockCache()->NewSyncBlockSlot(GetBaseObject());
        }
        else
        {
            //We already have an index, we need to hold the syncblock
            indexHeld = TRUE;
        }

        {
            //! NewSyncBlockSlot has side-effects that we don't have backout for - thus, that must be the last
            //! failable operation called.
            CANNOTTHROWCOMPLUSEXCEPTION();
            FAULT_FORBID();


            syncBlockMemoryHolder.SuppressRelease();

            new (syncBlock) SyncBlock(indx);

            // after this point, nobody can update the index in the header to give an AD index
            EnterSpinLock();

            {
                // If there's an appdomain index stored in the header, transfer it to the syncblock

                ADIndex dwAppDomainIndex = GetAppDomainIndex();
                if (dwAppDomainIndex.m_dwIndex)
                    syncBlock->SetAppDomainIndex(dwAppDomainIndex);

                // If the thin lock in the header is in use, transfer the information to the syncblock
                DWORD bits = GetBits();
                if ((bits & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX) == 0)
                {
                    DWORD lockThreadId = bits & SBLK_MASK_LOCK_THREADID;
                    DWORD recursionLevel = (bits & SBLK_MASK_LOCK_RECLEVEL) >> SBLK_RECLEVEL_SHIFT;
                    if (lockThreadId != 0 || recursionLevel != 0)
                    {
                        // recursionLevel can't be non-zero if thread id is 0
                        _ASSERTE(lockThreadId != 0);

                        Thread *pThread = g_pThinLockThreadIdDispenser->IdToThreadWithValidation(lockThreadId);

                        if (pThread == NULL)
                        {
                            // The lock is orphaned.
                            pThread = (Thread*) -1;
                        }
                        syncBlock->InitState();
                        syncBlock->SetAwareLock(pThread, recursionLevel + 1);
                    }
                }
                else if ((bits & BIT_SBLK_IS_HASHCODE) != 0)
                {
                    DWORD hashCode = bits & MASK_HASHCODE;

                    syncBlock->SetHashCode(hashCode);
                }
            }

            SyncTableEntry::GetSyncTableEntry() [indx].m_SyncBlock = syncBlock;

            // in order to avoid a race where some thread tries to get the AD index and we've already nuked it,
            // make sure the syncblock etc is all setup with the AD index prior to replacing the index
            // in the header
            if (GetHeaderSyncBlockIndex() == 0)
            {
                // We have transferred the AppDomain into the syncblock above.
                SetIndex(BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX | indx);
            }

            //If we had already an index, hold the syncblock
            //for the lifetime of the object.
            if (indexHeld)
                syncBlock->SetPrecious();

            ReleaseSpinLock();

            // SyncBlockCache::LockHolder goes out of scope here
        }
    }

    RETURN syncBlock;
}


// COM Interop has special access to sync blocks
// for now check if we already have a sync block
// other wise create one,
// returns NULL in case of exceptions
SyncBlock* ObjHeader::GetSyncBlockSpecial()
{
    STATIC_CONTRACT_NOTHROW;

    SyncBlock* syncBlock = GetBaseObject()->PassiveGetSyncBlock();
    if (syncBlock == NULL)
    {
        EX_TRY
        {
            syncBlock = GetSyncBlock();
        }
        EX_CATCH
        {
            syncBlock = NULL;
        }
        EX_END_CATCH(SwallowAllExceptions)
    }
    return syncBlock;
}

BOOL ObjHeader::Wait(INT32 timeOut, BOOL exitContext)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    //  The following code may cause GC, so we must fetch the sync block from
    //  the object now in case it moves.
    SyncBlock *pSB = GetBaseObject()->GetSyncBlock();

    // GetSyncBlock throws on failure
    _ASSERTE(pSB != NULL);

    // make sure we own the crst
    if (!pSB->DoesCurrentThreadOwnMonitor())
        COMPlusThrow(kSynchronizationLockException);

#ifdef _DEBUG
    Thread *pThread = GetThread();
    DWORD curLockCount = pThread->m_dwLockCount;
#endif

    BOOL result = pSB->Wait(timeOut,exitContext);

    _ASSERTE (curLockCount == pThread->m_dwLockCount);

    return result;
}

void ObjHeader::Pulse()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    //  The following code may cause GC, so we must fetch the sync block from
    //  the object now in case it moves.
    SyncBlock *pSB = GetBaseObject()->GetSyncBlock();

    // GetSyncBlock throws on failure
    _ASSERTE(pSB != NULL);

    // make sure we own the crst
    if (!pSB->DoesCurrentThreadOwnMonitor())
        COMPlusThrow(kSynchronizationLockException);

    pSB->Pulse();
}

void ObjHeader::PulseAll()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    //  The following code may cause GC, so we must fetch the sync block from
    //  the object now in case it moves.
    SyncBlock *pSB = GetBaseObject()->GetSyncBlock();

    // GetSyncBlock throws on failure
    _ASSERTE(pSB != NULL);

    // make sure we own the crst
    if (!pSB->DoesCurrentThreadOwnMonitor())
        COMPlusThrow(kSynchronizationLockException);

    pSB->PulseAll();
}


// ***************************************************************************
//
//              AwareLock class implementation (GC-aware locking)
//
// ***************************************************************************

void AwareLock::AllocLockSemEvent()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Before we switch from cooperative, ensure that this syncblock won't disappear
    // under us.  For something as expensive as an event, do it permanently rather
    // than transiently.
    SetPrecious();

    GCX_PREEMP();

    // No need to take a lock - CLREvent::CreateMonitorEvent is thread safe
    m_SemEvent.CreateMonitorEvent((SIZE_T)this);
}

void AwareLock::Enter()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    Thread  *pCurThread = GetThread();

    for (;;) 
    {
        // Read existing lock state.
        volatile LONG state = m_MonitorHeld;

        if (state == 0) 
        {
            // Common case: lock not held, no waiters. Attempt to acquire lock by
            // switching lock bit.
            if (FastInterlockCompareExchange((LONG*)&m_MonitorHeld, 1, 0) == 0)
            {
                break;
            }
        } 
        else 
        {
            // It's possible to get here with waiters but no lock held, but in this
            // case a signal is about to be fired which will wake up a waiter. So
            // for fairness sake we should wait too.
            // Check first for recursive lock attempts on the same thread.
            if (m_HoldingThread == pCurThread)
            {    
                goto Recursion;
            }

            // Attempt to increment this count of waiters then goto contention
            // handling code.
            if (FastInterlockCompareExchange((LONG*)&m_MonitorHeld, (state + 2), state) == state)
            {
                goto MustWait;
            }
        }

    }

    // We get here if we successfully acquired the mutex.
    m_HoldingThread = pCurThread;
    m_Recursion = 1;
    pCurThread->IncLockCount();

#if defined(_DEBUG) && defined(TRACK_SYNC)
    {
        // The best place to grab this is from the ECall frame
        Frame   *pFrame = pCurThread->GetFrame();
        int      caller = (pFrame && pFrame != FRAME_TOP
                            ? (int) pFrame->GetReturnAddress()
                            : -1);
        pCurThread->m_pTrackSync->EnterSync(caller, this);
    }
#endif

    return;

MustWait:
    // Didn't manage to get the mutex, must wait.
    EnterEpilog(pCurThread);
    return;

Recursion:
    // Got the mutex via recursive locking on the same thread.
    _ASSERTE(m_Recursion >= 1);
    m_Recursion++;
#if defined(_DEBUG) && defined(TRACK_SYNC)
    // The best place to grab this is from the ECall frame
    Frame   *pFrame = pCurThread->GetFrame();
    int      caller = (pFrame && pFrame != FRAME_TOP ? (int) pFrame->GetReturnAddress() : -1);
    pCurThread->m_pTrackSync->EnterSync(caller, this);
#endif
}

BOOL AwareLock::TryEnter(INT32 timeOut)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (timeOut != 0)
    {
        LARGE_INTEGER qpFrequency, qpcStart, qpcEnd;
        BOOL canUseHighRes = QueryPerformanceCounter(&qpcStart);

        // try some more busy waiting
        if (Contention(timeOut))
            return TRUE;

        DWORD elapsed = 0;
        if (canUseHighRes && QueryPerformanceCounter(&qpcEnd) && QueryPerformanceFrequency(&qpFrequency))
            elapsed = (DWORD)((qpcEnd.QuadPart-qpcStart.QuadPart)/(qpFrequency.QuadPart/1000));

        if (elapsed >= (DWORD)timeOut)
            return FALSE;

        if (timeOut != (INT32)INFINITE)
            timeOut -= elapsed;
    }

    Thread  *pCurThread = GetThread();
    TESTHOOKCALL(AppDomainCanBeUnloaded(pCurThread->GetDomain()->GetId().m_dwId,FALSE));    

    if (pCurThread->IsAbortRequested()) 
    {
        pCurThread->HandleThreadAbort();
    }

retry:
    for (;;) {

        // Read existing lock state.
        LONG state = m_MonitorHeld;

        if (state == 0) 
        {
            // Common case: lock not held, no waiters. Attempt to acquire lock by
            // switching lock bit.
            if (FastInterlockCompareExchange((LONG*)&m_MonitorHeld, 1, 0) == 0)
            {
                break;
            }

        } 
        else 
        {
            // It's possible to get here with waiters but no lock held, but in this
            // case a signal is about to be fired which will wake up a waiter. So
            // for fairness sake we should wait too.
            // Check first for recursive lock attempts on the same thread.
            if (m_HoldingThread == pCurThread)
            {
                goto Recursion;
            }
            else
            {
                goto WouldBlock;
            }
        }

    }

    // We get here if we successfully acquired the mutex.
    m_HoldingThread = pCurThread;
    m_Recursion = 1;
    pCurThread->IncLockCount();

#if defined(_DEBUG) && defined(TRACK_SYNC)
    {
        // The best place to grab this is from the ECall frame
        Frame   *pFrame = pCurThread->GetFrame();
        int      caller = (pFrame && pFrame != FRAME_TOP ? (int) pFrame->GetReturnAddress() : -1);
        pCurThread->m_pTrackSync->EnterSync(caller, this);
    }
#endif

    return true;

WouldBlock:
    // Didn't manage to get the mutex, return failure if no timeout, else wait
    // for at most timeout milliseconds for the mutex.
    if (!timeOut)
    {
        return false;
    }

    // The precondition for EnterEpilog is that the count of waiters be bumped
    // to account for this thread
    for (;;)
    {
        // Read existing lock state.
        volatile LONG state = m_MonitorHeld;
        if (state == 0)
        {
            goto retry;
        }
        if (FastInterlockCompareExchange((LONG*)&m_MonitorHeld, (state + 2), state) == state)
        {
            break;
        }
    }
    return EnterEpilog(pCurThread, timeOut);

Recursion:
    // Got the mutex via recursive locking on the same thread.
    _ASSERTE(m_Recursion >= 1);
    m_Recursion++;
#if defined(_DEBUG) && defined(TRACK_SYNC)
    // The best place to grab this is from the ECall frame
    Frame   *pFrame = pCurThread->GetFrame();
    int      caller = (pFrame && pFrame != FRAME_TOP ? (int) pFrame->GetReturnAddress() : -1);
    pCurThread->m_pTrackSync->EnterSync(caller, this);
#endif

    return true;
}


BOOL AwareLock::EnterEpilog(Thread* pCurThread, INT32 timeOut)
{
    STATIC_CONTRACT_THROWS;

    DWORD ret = 0;
    BOOL finished = false;
    ULONGLONG start, end, duration;

    // Require all callers to be in cooperative mode.  If they have switched to preemptive
    // mode temporarily before calling here, then they are responsible for protecting
    // the object associated with this lock.
    _ASSERTE(pCurThread->PreemptiveGCDisabled());

    OBJECTREF    obj = GetOwningObject();

    // We cannot allow the AwareLock to be cleaned up underneath us by the GC.
    IncrementTransientPrecious();

    GCPROTECT_BEGIN(obj);
    {
        if (!m_SemEvent.IsMonitorEventAllocated())
        {
            AllocLockSemEvent();
        }
        _ASSERTE(m_SemEvent.IsMonitorEventAllocated());

        pCurThread->EnablePreemptiveGC();

        for (;;)
        {
            // We might be interrupted during the wait (Thread.Interrupt), so we need an
            // exception handler round the call.
            EE_TRY_FOR_FINALLY
            {
                // Measure the time we wait so that, in the case where we wake up
                // and fail to acquire the mutex, we can adjust remaining timeout
                // accordingly.
                start = CLRGetTickCount64();
                ret = m_SemEvent.Wait(timeOut, TRUE);
                _ASSERTE((ret == WAIT_OBJECT_0) || (ret == WAIT_TIMEOUT));
                if (timeOut != (INT32) INFINITE)
                {
                    end = CLRGetTickCount64();
                    if (end == start)
                    {
                        duration = 1;
                    }
                    else
                    {
                        duration = end - start;
                    }
                    duration = min(duration, (DWORD)timeOut);
                    timeOut -= (INT32)duration;
                }
            }
            EE_FINALLY
            {
                if (GOT_EXCEPTION())
                {
                    // We must decrement the waiter count.
                    for (;;)
                    {
                        volatile LONG state = m_MonitorHeld;
                        _ASSERTE((state >> 1) != 0);
                        if (FastInterlockCompareExchange((LONG*)&m_MonitorHeld, state - 2, state) == state)
                        {
                            break;
                        }
                    }
                    // And signal the next waiter, else they'll wait forever.
                    m_SemEvent.Set();
                }
            } EE_END_FINALLY;

            if (ret == WAIT_OBJECT_0)
            {
                // Attempt to acquire lock (this also involves decrementing the waiter count).
                for (;;) 
                {
                    volatile LONG state = m_MonitorHeld;
                    _ASSERTE(((size_t)state >> 1) != 0);
                    if ((size_t)state & 1)
                    {
                        break;
                    }
                    if (FastInterlockCompareExchange((LONG*)&m_MonitorHeld, ((state - 2) | 1), state) == state)
                    {
                        finished = true;
                        break;
                    }
                }
            }
            else
            {
                // We timed out, decrement waiter count.
                for (;;) 
                {
                    volatile LONG state = m_MonitorHeld;
                    _ASSERTE((state >> 1) != 0);
                    if (FastInterlockCompareExchange((LONG*)&m_MonitorHeld, state - 2, state) == state)
                    {
                        finished = true;
                        break;
                    }
                }
            }

            if (finished)
            {
                break;
            }
        }

        pCurThread->DisablePreemptiveGC();
    }
    GCPROTECT_END();
    DecrementTransientPrecious();

    if (ret == WAIT_TIMEOUT)
    {
        return FALSE;
    }

    m_HoldingThread = pCurThread;
    m_Recursion = 1;
    pCurThread->IncLockCount();

#if defined(_DEBUG) && defined(TRACK_SYNC)
    // The best place to grab this is from the ECall frame
    Frame   *pFrame = pCurThread->GetFrame();
    int      caller = (pFrame && pFrame != FRAME_TOP ? (int) pFrame->GetReturnAddress() : -1);
    pCurThread->m_pTrackSync->EnterSync(caller, this);
#endif

    return (ret != WAIT_TIMEOUT);
}


BOOL AwareLock::Leave()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    Thread* pThread = GetThread();

    AwareLock::LeaveHelperAction action = LeaveHelper(pThread);

    switch(action)
    {
    case AwareLock::LeaveHelperAction_None:
        // We are done
        return TRUE;
    case AwareLock::LeaveHelperAction_Signal:
        // Signal the event
        Signal();
        return TRUE;
    default:
        // Must be an error otherwise
        _ASSERTE(action == AwareLock::LeaveHelperAction_Error);
        return FALSE;
    }
}

#ifdef _DEBUG
#define _LOGCONTENTION
#endif // _DEBUG

#ifdef  _LOGCONTENTION
inline void LogContention()
{
    WRAPPER_CONTRACT;
#ifdef LOGGING
    if (LoggingOn(LF_SYNC, LL_INFO100))
    {
        LogSpewAlways("Contention: Stack Trace Begin\n");
        void LogStackTrace();
        LogStackTrace();
        LogSpewAlways("Contention: Stack Trace End\n");
    }
#endif
}
#else
#define LogContention()
#endif

bool AwareLock::Contention(INT32 timeOut)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    DWORD startTime = 0;
    if (timeOut != (INT32)INFINITE)
        startTime = GetTickCount();

    COUNTER_ONLY(GetPrivatePerfCounters().m_LocksAndThreads.cContention++);

    if ( ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) ) 
    {
        g_pEtwTracer->EtwTraceEvent(&MonitorGuid, ETW_TYPE_MON_CONTENTION, NULL, 0);
    }

    LogContention();
    Thread      *pCurThread = GetThread();
    OBJECTREF    obj = GetOwningObject();
    bool    bEntered = false;

    // We cannot allow the AwareLock to be cleaned up underneath us by the GC.
    IncrementTransientPrecious();

    GCPROTECT_BEGIN(obj);
    {
        GCX_PREEMP();

        // Try spinning and yielding before eventually blocking.
        // The limit of 10 is largely arbitrary - feel free to tune if you have evidence
        // you're making things better                        
        for (int iter = 0; iter < 10; iter++)
        {
            DWORD i = 50;
            do
            {
                if (TryEnter())
                {
                    pCurThread->DisablePreemptiveGC();
                    bEntered = true;
                    goto entered;
                }

                if (g_SystemInfo.dwNumberOfProcessors <= 1)
                {
                    break;
                }

                if (timeOut != (INT32)INFINITE && GetTickCount() - startTime >= (DWORD)timeOut)
                    break;

                // Delay by approximately 2*i clock cycles (Pentium III).
                // This is brittle code - future processors may of course execute this
                // faster or slower, and future code generators may eliminate the loop altogether.
                // The precise value of the delay is not critical, however, and I can't think
                // of a better way that isn't machine-dependent.                       
                int sum = 0;
                for (int delayCount = i; --delayCount; )
                {
                    sum += delayCount;
                    YieldProcessor();           // indicate to the processor that we are spining
                }
                if (sum == 0)
                {
                    // never executed, just to fool the compiler into thinking sum is live here,
                    // so that it won't optimize away the loop.
                    static char dummy;
                    dummy++;
                }

                // exponential backoff: wait 3 times as long in the next iteration
                i = i*3;
            }
            while (i < 20000*g_SystemInfo.dwNumberOfProcessors);

            {
                GCX_COOP();
                pCurThread->HandleThreadAbort();
            }

            __SwitchToThread(0);
        }
    }
entered: ;
    GCPROTECT_END();
    // we are in co-operative mode so no need to keep this set
    DecrementTransientPrecious();
    if (!bEntered && timeOut == (INT32)INFINITE)
    {

        // We've tried hard to enter - we need to eventually block to avoid wasting too much cpu
        // time.
        Enter();
        bEntered = TRUE;
    }
    return bEntered;
}


LONG AwareLock::LeaveCompletely()
{
    WRAPPER_CONTRACT;

    LONG count = 0;
    while (Leave()) {
        count++;
    }
    _ASSERTE(count > 0);            // otherwise we were never in the lock

    return count;
}


BOOL AwareLock::OwnedByCurrentThread()
{
    WRAPPER_CONTRACT;
    return (GetThread() == m_HoldingThread);
}


// ***************************************************************************
//
//              SyncBlock class implementation
//
// ***************************************************************************

// We maintain two queues for SyncBlock::Wait.
// 1. Inside SyncBlock we queue all threads that are waiting on the SyncBlock.
//    When we pulse, we pick the thread from this queue using FIFO.
// 2. We queue all SyncBlocks that a thread is waiting for in Thread::m_WaitEventLink.
//    When we pulse a thread, we find the event from this queue to set, and we also
//    or in a 1 bit in the syncblock value saved in the queue, so that we can return
//    immediately from SyncBlock::Wait if the syncblock has been pulsed.
BOOL SyncBlock::Wait(INT32 timeOut, BOOL exitContext)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    Thread  *pCurThread = GetThread();
    BOOL     isTimedOut = FALSE;
    BOOL     isEnqueued = FALSE;
    WaitEventLink waitEventLink;
    WaitEventLink *pWaitEventLink;

    // As soon as we flip the switch, we are in a race with the GC, which could clean
    // up the SyncBlock underneath us -- unless we report the object.
    _ASSERTE(pCurThread->PreemptiveGCDisabled());

    // Does this thread already wait for this SyncBlock?
    WaitEventLink *walk = pCurThread->WaitEventLinkForSyncBlock(this);
    if (walk->m_Next) {
        if (walk->m_Next->m_WaitSB == this) {
            // Wait on the same lock again.
            walk->m_Next->m_RefCount ++;
            pWaitEventLink = walk->m_Next;
        }
        else if ((SyncBlock*)(((DWORD_PTR)walk->m_Next->m_WaitSB) & ~1)== this) {
            // This thread has been pulsed.  No need to wait.
            return TRUE;
        }
    }
    else {
        // First time this thread is going to wait for this SyncBlock.
        CLREvent* hEvent;
        if (pCurThread->m_WaitEventLink.m_Next == NULL) {
            hEvent = &(pCurThread->m_EventWait);
        }
        else {
            hEvent = GetEventFromEventStore();
        }
        waitEventLink.m_WaitSB = this;
        waitEventLink.m_EventWait = hEvent;
        waitEventLink.m_Thread = pCurThread;
        waitEventLink.m_Next = NULL;
        waitEventLink.m_LinkSB.m_pNext = NULL;
        waitEventLink.m_RefCount = 1;
        pWaitEventLink = &waitEventLink;
        walk->m_Next = pWaitEventLink;

        // Before we enqueue it (and, thus, before it can be dequeued), reset the event
        // that will awaken us.
        hEvent->Reset();

        // This thread is now waiting on this sync block
        ThreadQueue::EnqueueThread(pWaitEventLink, this);

        isEnqueued = TRUE;
    }

    _ASSERTE ((SyncBlock*)((DWORD_PTR)walk->m_Next->m_WaitSB & ~1)== this);

    PendingSync   syncState(walk);

    OBJECTREF     obj = m_Monitor.GetOwningObject();

    m_Monitor.IncrementTransientPrecious();

    GCPROTECT_BEGIN(obj);
    {
        GCX_PREEMP();

        // remember how many times we synchronized
        syncState.m_EnterCount = LeaveMonitorCompletely();
        _ASSERTE(syncState.m_EnterCount > 0);

        Context* targetContext = pCurThread->GetContext();
        _ASSERTE(targetContext);
        Context* defaultContext = pCurThread->GetDomain()->GetDefaultContext();
        _ASSERTE(defaultContext);

        if (exitContext &&
            targetContext != defaultContext)
        {
            Context::MonitorWaitArgs waitArgs = {timeOut, &syncState, &isTimedOut};
            Context::CallBackInfo callBackInfo = {Context::MonitorWait_callback, (void*) &waitArgs};
            Context::RequestCallBack(CURRENT_APPDOMAIN_ID, defaultContext, &callBackInfo);
        }
        else
        {
            isTimedOut = pCurThread->Block(timeOut, &syncState);
        }
    }
    GCPROTECT_END();
    m_Monitor.DecrementTransientPrecious();

    return !isTimedOut;
}

void SyncBlock::Pulse()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    WaitEventLink  *pWaitEventLink;

    if ((pWaitEventLink = ThreadQueue::DequeueThread(this)) != NULL)
        pWaitEventLink->m_EventWait->Set();
}

void SyncBlock::PulseAll()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    WaitEventLink  *pWaitEventLink;

    while ((pWaitEventLink = ThreadQueue::DequeueThread(this)) != NULL)
        pWaitEventLink->m_EventWait->Set();
}

bool SyncBlock::SetInteropInfo(InteropSyncBlockInfo* pInteropInfo)
{
    WRAPPER_CONTRACT;
    SetPrecious();

    // We could be agile, but not have noticed yet.  We can't assert here
    //  that we live in any given domain, nor is this an appropriate place
    //  to re-parent the syncblock.
/*    _ASSERTE (m_dwAppDomainIndex.m_dwIndex == 0 || 
              m_dwAppDomainIndex == SystemDomain::System()->DefaultDomain()->GetIndex() || 
              m_dwAppDomainIndex == GetAppDomain()->GetIndex());
    m_dwAppDomainIndex = GetAppDomain()->GetIndex();
*/
    return (FastInterlockCompareExchangePointer( (void*volatile*)&m_pInteropInfo,
                                                            pInteropInfo,
                                                            NULL) == NULL);
}




#endif // !DACCESS_COMPILE
