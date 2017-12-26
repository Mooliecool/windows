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
// SYNCBLK.H
//
// Definition of a SyncBlock and the SyncBlockCache which manages it
//

#ifndef _SYNCBLK_H_
#define _SYNCBLK_H_

#include "util.hpp"
#include "list.h"
#include "crst.h"
#include "handletable.h"
#include "vars.hpp"

// Every Object is preceded by an ObjHeader (at a negative offset).  The
// ObjHeader has an index to a SyncBlock.  This index is 0 for the bulk of all
// instances, which indicates that the object shares a dummy SyncBlock with
// most other objects.
//
// The SyncBlock is primarily responsible for object synchronization.  However,
// it is also a "kitchen sink" of sparsely allocated instance data.  For instance,
// the default implementation of Hash() is based on the existence of a SyncTableEntry.
// And objects exposed to or from COM, or through context boundaries, can store sparse
// data here.
//
// SyncTableEntries and SyncBlocks are allocated in non-GC memory.  A weak pointer
// from the SyncTableEntry to the instance is used to ensure that the SyncBlock and
// SyncTableEntry are reclaimed (recycled) when the instance dies.
//
// The organization of the SyncBlocks isn't intuitive (at least to me).  Here's
// the explanation:
//
// Before each Object is an ObjHeader.  If the object has a SyncBlock, the
// ObjHeader contains a non-0 index to it.
//
// The index is looked up in the g_pSyncTable of SyncTableEntries.  This means
// the table is consecutive for all outstanding indices.  Whenever it needs to
// grow, it doubles in size and copies all the original entries.  The old table
// is kept until GC time, when it can be safely discarded.
//
// Each SyncTableEntry has a backpointer to the object and a forward pointer to
// the actual SyncBlock.  The SyncBlock is allocated out of a SyncBlockArray
// which is essentially just a block of SyncBlocks.
//
// The SyncBlockArrays are managed by a SyncBlockCache that handles the actual
// allocations and frees of the blocks.
//
// So...
//
// Each allocation and release has to handle free lists in the table of entries
// and the table of blocks.
//
// We burn an extra 4 bytes for the pointer from the SyncTableEntry to the
// SyncBlock.
//
// The reason for this is that many objects have a SyncTableEntry but no SyncBlock.
// That's because someone (e.g. HashTable) called Hash() on them.
//
// Incidentally, there's a better write-up of all this stuff in the archives.

#ifdef _X86_
#include <pshpack4.h>
#endif // _X86_

// forwards:
class SyncBlock;
class SyncBlockCache;
class SyncTableEntry;
class SyncBlockArray;
class AwareLock;
class Thread;
class AppDomain;


#include "eventstore.hpp"

#include "eventstore.hpp"

#include "synch.h"


// At a negative offset from each Object is an ObjHeader.  The 'size' of the
// object includes these bytes.  However, we rely on the previous object allocation
// to zero out the ObjHeader for the current allocation.  And the limits of the
// GC space are initialized to respect this "off by one" error.

// m_SyncBlockValue is carved up into an index and a set of bits.  Steal bits by
// reducing the mask.  We use the very high bit, in _DEBUG, to be sure we never forget
// to mask the Value to obtain the Index

    // These first three are only used on strings (If the first one is on, we know whether 
    // the string has high byte characters, and the second bit tells which way it is. 
    // Note that we are reusing the FINALIZER_RUN bit since strings don't have finalizers,
    // so the value of this bit does not matter for strings
#define BIT_SBLK_STRING_HAS_NO_HIGH_CHARS   0x80000000

// Used as workaround for infinite loop case.  Will set this bit in the sblk if we have already
// seen this sblk in our agile checking logic.  Problem is seen when object 1 has a ref to object 2
// and object 2 has a ref to object 1.  The agile checker will infinitely loop on these references.
#define BIT_SBLK_AGILE_IN_PROGRESS          0x80000000
#define BIT_SBLK_STRING_HIGH_CHARS_KNOWN    0x40000000
#define BIT_SBLK_STRING_HAS_SPECIAL_SORT    0xC0000000
#define BIT_SBLK_STRING_HIGH_CHAR_MASK      0xC0000000

#define BIT_SBLK_FINALIZER_RUN              0x40000000
#define BIT_SBLK_GC_RESERVE                 0x20000000

// This lock is only taken when we need to modify the index value in m_SyncBlockValue. 
// It should not be taken if the object already has a real syncblock index. 
#define BIT_SBLK_SPIN_LOCK                  0x10000000

#define BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX    0x08000000

// if BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX is clear, the rest of the header dword is layed out as follows:
// - lower ten bits (bits 0 thru 9) is thread id used for the thin locks
//   value is zero if no thread is holding the lock
// - following six bits (bits 10 thru 15) is recursion level used for the thin locks
//   value is zero if lock is not taken or only taken once by the same thread
// - following 11 bits (bits 16 thru 26) is app domain index
//   value is zero if no app domain index is set for the object
#define SBLK_MASK_LOCK_THREADID             0x000003FF   // special value of 0 + 1023 thread ids
#define SBLK_MASK_LOCK_RECLEVEL             0x0000FC00   // 64 recursion levels
#define SBLK_LOCK_RECLEVEL_INC              0x00000400   // each level is this much higher than the previous one
#define SBLK_APPDOMAIN_SHIFT                16           // shift right this much to get appdomain index
#define SBLK_RECLEVEL_SHIFT                 10           // shift right this much to get recursion level
#define SBLK_MASK_APPDOMAININDEX            0x000007FF   // 2048 appdomain indices

// add more bits here... (adjusting the following mask to make room)

// if BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX is set, 
// then if BIT_SBLK_IS_HASHCODE is also set, the rest of the dword is the hash code (bits 0 thru 25),
// otherwise the rest of the dword is the sync block index (bits 0 thru 25)
#define BIT_SBLK_IS_HASHCODE            0x04000000

#ifdef _DEBUG
// in the debug build, we use the upper hash bit to indicate that it's a frozen object
// this facilitates asserts on frozen-ness 
#define HASHCODE_BITS                   25
#define BIT_SBLK_IS_FROZEN            0x02000000
#else
#define HASHCODE_BITS                   26
#endif

#define MASK_HASHCODE                   ((1<<HASHCODE_BITS)-1)
#define SYNCBLOCKINDEX_BITS             26
#define MASK_SYNCBLOCKINDEX             ((1<<SYNCBLOCKINDEX_BITS)-1)

// Spin for about 1000 cycles before waiting longer.
#define     BIT_SBLK_SPIN_COUNT         1000

// The GC is highly dependent on SIZE_OF_OBJHEADER being exactly the sizeof(ObjHeader)
// We define this macro so that the preprocessor can calculate padding structures.
#define SIZEOF_OBJHEADER    4
 


// this is a 'GC-aware' Lock.  It is careful to enable preemptive GC before it
// attempts any operation that can block.  Once the operation is finished, it
// restores the original state of GC.

// AwareLocks can only be created inside SyncBlocks, since they depend on the
// enclosing SyncBlock for coordination.  This is enforced by the private ctor.

class AwareLock
{
    friend class CheckAsmOffsets;

    friend class SyncBlockCache;
    friend class SyncBlock;

public:
    volatile LONG   m_MonitorHeld;
    ULONG           m_Recursion;
    PTR_Thread      m_HoldingThread;
    
  private:
    LONG            m_TransientPrecious;


    // This is a backpointer from the syncblock to the synctable entry.  This allows
    // us to recover the object that holds the syncblock.
    DWORD           m_dwSyncIndex;

    CLREvent        m_SemEvent;

    // Only SyncBlocks can create AwareLocks.  Hence this private constructor.
    AwareLock(DWORD indx)
        : m_MonitorHeld(0),
          m_Recursion(0),
#ifndef DACCESS_COMPILE          
// PreFAST has trouble with intializing a NULL PTR_Thread.
          m_HoldingThread(NULL),
#endif // DACCESS_COMPILE          
          m_TransientPrecious(0),
          m_dwSyncIndex(indx)
    {
        LEAF_CONTRACT;
    }

    ~AwareLock()
    {
        LEAF_CONTRACT;
        // We deliberately allow this to remain incremented if an exception blows
        // through a lock attempt.  This simply prevents the GC from aggressively
        // reclaiming a particular syncblock until the associated object is garbage.
        // From a perf perspective, it's not worth using SEH to prevent this from
        // happening.
        //
        // _ASSERTE(m_TransientPrecious == 0);
    }

  public:

    enum LeaveHelperAction {
        LeaveHelperAction_None,
        LeaveHelperAction_Signal,
        LeaveHelperAction_Yield,
        LeaveHelperAction_Contention,
        LeaveHelperAction_Error,
    };

    // Helper encapsulating the core logic for leaving monitor. Returns what kind of 
    // follow up action is necessary
    FORCEINLINE AwareLock::LeaveHelperAction LeaveHelper(Thread* pCurThread);

    void    Enter();
    BOOL    TryEnter(INT32 timeOut = 0);
    BOOL    EnterEpilog(Thread *pCurThread, INT32 timeOut = INFINITE);
    BOOL    Leave();

    void    Signal()
    {
        WRAPPER_CONTRACT;
        
        // CLREvent::SetMonitorEvent works even if the event has not been intialized yet
        m_SemEvent.SetMonitorEvent();
    }

    bool    Contention(INT32 timeOut = INFINITE);
    void    AllocLockSemEvent();
    LONG    LeaveCompletely();
    BOOL    OwnedByCurrentThread();

    void    IncrementTransientPrecious()
    {
        LEAF_CONTRACT;
        FastInterlockIncrement(&m_TransientPrecious);
        _ASSERTE(m_TransientPrecious > 0);
    }

    void    DecrementTransientPrecious()
    {
        LEAF_CONTRACT;
        _ASSERTE(m_TransientPrecious > 0);
        FastInterlockDecrement(&m_TransientPrecious);
    }

    void SetPrecious();

    // Provide access to the object associated with this awarelock, so client can
    // protect it.
    inline OBJECTREF GetOwningObject();

    // Provide access to the Thread object that owns this awarelock.  This is used
    // to provide a host to find out owner of a lock.
    inline Thread* GetOwningThread()
    {
        LEAF_CONTRACT;
        return m_HoldingThread;
    }
};


class InteropSyncBlockInfo
{
    friend class RCWHolder;
    
public:
    InteropSyncBlockInfo()
    {
        LEAF_CONTRACT;
        memset(this, 0, sizeof(InteropSyncBlockInfo));
    }
#ifndef DACCESS_COMPILE
    ~InteropSyncBlockInfo();
#endif


#ifndef DACCESS_COMPILE
    // set m_pUMEntryThunk if not already set - return true if not already set
    bool SetUMEntryThunk(void* pUMEntryThunk)
    {
        WRAPPER_CONTRACT;
        return (FastInterlockCompareExchangePointer( (void*volatile*)&m_pUMEntryThunk,
                                                                pUMEntryThunk,
                                                                NULL) == NULL);
    }

    void FreeUMEntryThunk();

    void OnADUnload();

#endif // DACCESS_COMPILE

    void* GetUMEntryThunk()
    {
        LEAF_CONTRACT;
        return m_pUMEntryThunk;
    }
    
private:
    // If this is a delegate marshalled out to unmanaged code, this points
    // to the thunk generated for unmanaged code to call back on.
    void*               m_pUMEntryThunk;


};

typedef DPTR(InteropSyncBlockInfo) PTR_InteropSyncBlockInfo;

// this is a lazily created additional block for an object which contains
// synchronzation information and other "kitchen sink" data
typedef DPTR(SyncBlock) PTR_SyncBlock;

class SyncBlock
{
    // ObjHeader creates our Mutex and Event
    friend class ObjHeader;
    friend class SyncBlockCache;
    friend struct ThreadQueue;
#ifdef DACCESS_COMPILE
    friend class ClrDataAccess;
#endif
    friend class CheckAsmOffsets;

  protected:
    AwareLock  m_Monitor;                    // the actual monitor

  public:
    // If this object is exposed to unmanaged code, we keep some extra info here.
    PTR_InteropSyncBlockInfo    m_pInteropInfo;

  protected:

    // We thread two different lists through this link.  When the SyncBlock is
    // active, we create a list of waiting threads here.  When the SyncBlock is
    // released (we recycle them), the SyncBlockCache maintains a free list of
    // SyncBlocks here.
    //
    // We can't afford to use an SList<> here because we only want to burn
    // space for the minimum, which is the pointer within an SLink.
    SLink       m_Link;

    // This is the index for the appdomain to which the object belongs. If we
    // can't set it in the object header, then we set it here. Note that an
    // object doesn't always have this filled in. Only for COM interop, 
    // finalizers and objects in handles
    ADIndex m_dwAppDomainIndex;

    // This is the hash code for the object. It can either have been transfered
    // from the header dword, in which case it will be limited to 26 bits, or
    // have been generated right into this member variable here, when it will
    // be a full 32 bits.

    // A 0 in this variable means no hash code has been set yet - this saves having
    // another flag to express this state, and it enables us to use a 32-bit interlocked
    // operation to set the hash code, on the other hand it means that hash codes
    // can never be 0. ObjectNative::GetHashCode in COMObject.cpp makes sure to enforce this.
    DWORD m_dwHashCode;

#if CHECK_APP_DOMAIN_LEAKS 
    DWORD m_dwFlags;

    enum {
        IsObjectAppDomainAgile = 1,
        IsObjectCheckedForAppDomainAgile = 2,
        //Thread::m_ThreadStatics is an object array, which by definition is not agile.
        //But due to the special way we handle it, it's considered to be agile and could refer other non-agile objects
        //in different domains
        IsObjectThreadStaticsArray = 4   
    };
#endif

  public:
    SyncBlock(DWORD indx)
        : m_Monitor(indx)
        , m_dwHashCode(0)
#if CHECK_APP_DOMAIN_LEAKS 
        , m_dwFlags(0)
#endif
    {
        LEAF_CONTRACT;

        m_pInteropInfo = NULL;
        
        // The monitor must be 32-bit aligned for atomicity to be guaranteed.
        _ASSERTE((((size_t) &m_Monitor) & 3) == 0);
    }

   // As soon as a syncblock acquires some state that cannot be recreated, we latch
   // a bit.
   void SetPrecious()
   {
       WRAPPER_CONTRACT;
       m_Monitor.SetPrecious();
   }

   BOOL IsPrecious()
   {
       LEAF_CONTRACT;
       return (m_Monitor.m_dwSyncIndex & SyncBlockPrecious) != 0;
   }

   void OnADUnload();

    // True is the syncblock and its index are disposable. 
    // If new members are added to the syncblock, this 
    // method needs to be modified accordingly
    BOOL IsIDisposable()
    {
        WRAPPER_CONTRACT;
#ifndef GC_SMP
        return (!IsPrecious() &&
                m_Monitor.m_MonitorHeld == 0 &&
                m_Monitor.m_TransientPrecious == 0);
#else
        // During the copy phase, returning true from here
        // causes deallocation of sync blocks associated with
        // relocated objects
        return false;
#endif
    }

    // Gets the InteropInfo block, creates a new one if none is present.
    InteropSyncBlockInfo* GetInteropInfo()
    {
        CONTRACT (InteropSyncBlockInfo*)
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            POSTCONDITION(CheckPointer(RETVAL));
        }
        CONTRACT_END;
        
        LEAF_CONTRACT;

        if (!m_pInteropInfo)
        {
            NewHolder<InteropSyncBlockInfo> pInteropInfo(new InteropSyncBlockInfo());
            if (SetInteropInfo(pInteropInfo))
                pInteropInfo.SuppressRelease();
        }
        
        RETURN m_pInteropInfo;
    }

    InteropSyncBlockInfo* GetInteropInfoNoCreate()
    {
        CONTRACT (InteropSyncBlockInfo*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        }
        CONTRACT_END;

        RETURN m_pInteropInfo;
    }

    // Returns false if the InteropInfo block was already set - does not overwrite the previous value.
    // True if the InteropInfo block was successfully set with the passed in value.
    bool SetInteropInfo(InteropSyncBlockInfo* pInteropInfo);
        

    ADIndex GetAppDomainIndex()
    {
        LEAF_CONTRACT;
        return m_dwAppDomainIndex;
    }

    void SetAppDomainIndex(ADIndex dwAppDomainIndex)
    {
        WRAPPER_CONTRACT;
        SetPrecious();
        m_dwAppDomainIndex = dwAppDomainIndex;
    }

    void SetAwareLock(Thread *holdingThread, DWORD recursionLevel)
    {
        LEAF_CONTRACT;
        // <NOTE>
        // DO NOT SET m_MonitorHeld HERE!  THIS IS NOT PROTECTED BY ANY LOCK!!
        // </NOTE>
        m_Monitor.m_HoldingThread = PTR_Thread(holdingThread);
        m_Monitor.m_Recursion = recursionLevel;
    }

    DWORD GetHashCode()
    {
        LEAF_CONTRACT;

        return m_dwHashCode;
    }

    DWORD SetHashCode(DWORD hashCode)
    {
        WRAPPER_CONTRACT;
        DWORD result = FastInterlockCompareExchange((LONG*)&m_dwHashCode, hashCode, 0);
        if (result == 0)
        {
            // the sync block now holds a hash code, which we can't afford to lose.
            SetPrecious();
            return hashCode;
        }
        else
            return result;
    }

    void *operator new (size_t sz, void* p)
    {
        LEAF_CONTRACT;
        return p ;
    }
    void operator delete(void *p)
    {
        LEAF_CONTRACT;
        // We've already destructed.  But retain the memory.
    }

    void EnterMonitor()
    {
        WRAPPER_CONTRACT;
        m_Monitor.Enter();
    }

    BOOL TryEnterMonitor(INT32 timeOut = 0)
    {
        WRAPPER_CONTRACT;
        return m_Monitor.TryEnter(timeOut);
    }

    // leave the monitor
    BOOL LeaveMonitor()
    {
        WRAPPER_CONTRACT;
        return m_Monitor.Leave();
    }

    AwareLock* GetMonitor()
    {
        WRAPPER_CONTRACT;
        //hold the syncblock 
        SetPrecious();
        return &m_Monitor;
    }

    AwareLock* QuickGetMonitor()
    {
        LEAF_CONTRACT;
    // Note that the syncblock isn't marked precious, so use caution when
    // calling this method.
        return &m_Monitor;
    }

    BOOL DoesCurrentThreadOwnMonitor()
    {
        WRAPPER_CONTRACT;
        return m_Monitor.OwnedByCurrentThread();
    }

    LONG LeaveMonitorCompletely()
    {
        WRAPPER_CONTRACT;
        return m_Monitor.LeaveCompletely();
    }

    BOOL Wait(INT32 timeOut, BOOL exitContext);
    void Pulse();
    void PulseAll();

    enum
    {
        // This bit indicates that the syncblock is valuable and can neither be discarded
        // nor re-created.
        SyncBlockPrecious   = 0x80000000,
    };


#if CHECK_APP_DOMAIN_LEAKS 
    BOOL IsAppDomainAgile() 
    {
        LEAF_CONTRACT;
        return m_dwFlags & IsObjectAppDomainAgile;
    }
    void SetIsAppDomainAgile() 
    {
        LEAF_CONTRACT;
        m_dwFlags |= IsObjectAppDomainAgile;
    }
    void UnsetIsAppDomainAgile()
    {
        LEAF_CONTRACT;
        m_dwFlags = m_dwFlags & ~IsObjectAppDomainAgile;
    }
    BOOL IsCheckedForAppDomainAgile() 
    {
        LEAF_CONTRACT;
        return m_dwFlags & IsObjectCheckedForAppDomainAgile;
    }
    void SetIsCheckedForAppDomainAgile() 
    {
        LEAF_CONTRACT;
        m_dwFlags |= IsObjectCheckedForAppDomainAgile;
    }

    //Special case: thread statics array is considered to be agile and could refer other non-agile objects in different domains
    BOOL IsThreadStaticsArray() 
    {
        LEAF_CONTRACT;
        return m_dwFlags & IsObjectThreadStaticsArray;
    }
    void SetIsThreadStaticsArray() 
    {
        LEAF_CONTRACT;
        m_dwFlags |= IsObjectThreadStaticsArray;
    }
#endif //CHECK_APP_DOMAIN_LEAKS
  protected:
    // <NOTE>
    // This should ONLY be called when initializing a SyncBlock (i.e. ONLY from
    // ObjHeader::GetSyncBlock()), otherwise we'll have a race condition.
    // </NOTE>
    void InitState()
    {
        LEAF_CONTRACT;
        m_Monitor.m_MonitorHeld = 1;
    }
};

class SyncTableEntry
{
  public:
    PTR_SyncBlock    m_SyncBlock;
    Object   *m_Object;
    static SyncTableEntry*& GetSyncTableEntry();
};


// this class stores free sync blocks after they're allocated and
// unused

typedef DPTR(SyncBlockCache) PTR_SyncBlockCache;

class SyncBlockCache
{
#ifdef DACCESS_COMPILE
    friend class ClrDataAccess;
#endif

    friend class SyncBlock;

    
  private:
    PTR_SLink   m_pCleanupBlockList;    // list of sync blocks that need cleanup
    SLink*      m_FreeBlockList;        // list of free sync blocks
    Crst        m_CacheLock;            // cache lock
    DWORD       m_FreeCount;            // count of active sync blocks
    DWORD       m_ActiveCount;          // number active
    SyncBlockArray *m_SyncBlocks;       // Array of new SyncBlocks.
    DWORD       m_FreeSyncBlock;        // Next Free Syncblock in the array
    DWORD       m_FreeSyncTableIndex;   // free index in the SyncBlocktable
    size_t      m_FreeSyncTableList;    // index of the free list of SyncBlock
                                        // Table entry
    DWORD       m_SyncTableSize;
    SyncTableEntry *m_OldSyncTables;    // Next old SyncTable
    BOOL        m_bSyncBlockCleanupInProgress;  // A flag indicating if sync block cleanup is in progress.    
    DWORD*      m_EphemeralBitmap;      // card table for ephemeral scanning

    BOOL        GCWeakPtrScanElement(int elindex, HANDLESCANPROC scanProc, LPARAM lp1, LPARAM lp2, BOOL& cleanup);

    void SetCard (size_t card);
    void ClearCard (size_t card);
    BOOL CardSetP (size_t card);
    void CardTableSetBit (size_t idx);


  public:
    SPTR_DECL(SyncBlockCache, s_pSyncBlockCache);
    static SyncBlockCache*& GetSyncBlockCache();

    void *operator new(size_t size, void *pInPlace)
    {
        LEAF_CONTRACT;
        return pInPlace;
    }

    void operator delete(void *p)
    {
        LEAF_CONTRACT;
    }

    SyncBlockCache();
    ~SyncBlockCache();

    static void Attach();
    static void Detach();
    void DoDetach();

    static void Start();
    static void Stop();

    // returns and removes next from free list
    SyncBlock* GetNextFreeSyncBlock();
    // returns and removes the next from cleanup list
    SyncBlock* GetNextCleanupSyncBlock();
    // inserts a syncblock into the cleanup list
    void    InsertCleanupSyncBlock(SyncBlock* psb);

    // Obtain a new syncblock slot in the SyncBlock table. Used as a hash code
    DWORD   NewSyncBlockSlot(Object *obj);

    // return sync block to cache or delete
    void    DeleteSyncBlock(SyncBlock *sb);

    // returns the sync block memory to the free pool but does not destruct sync block (must own cache lock already)
    void    DeleteSyncBlockMemory(SyncBlock *sb);

    // return sync block to cache or delete, called from GC
    void    GCDeleteSyncBlock(SyncBlock *sb);

    void    GCWeakPtrScan(HANDLESCANPROC scanProc, LPARAM lp1, LPARAM lp2);

    void    GCDone(BOOL demoting, int max_gen);

    void    CleanupSyncBlocks();

    void    CleanupSyncBlocksInAppDomain(AppDomain *pDomain);


    // Determines if a sync block cleanup is in progress.
    BOOL    IsSyncBlockCleanupInProgress()
    {
        LEAF_CONTRACT;
        return m_bSyncBlockCleanupInProgress;
    }

    // Encapsulate a CrstHolder, so that clients of our lock don't have to know
    // the details of our implementation.
    class LockHolder : public CrstHolder
    {
    public:
        LockHolder(SyncBlockCache *pCache, BOOL Take = TRUE)
            : CrstHolder(&pCache->m_CacheLock, Take)
        {
            LEAF_CONTRACT;
        }
    };
    friend class LockHolder;


#if CHECK_APP_DOMAIN_LEAKS 
    void CheckForUnloadedInstances(ADIndex unloadingIndex);
#endif
#ifdef _DEBUG
    friend void DumpSyncBlockCache();
#endif

#ifdef VERIFY_HEAP
    void    VerifySyncTableEntry();
#endif
};

class ObjHeader
{
    friend class CheckAsmOffsets;

#if !defined(_X86_) && !defined(_AMD64_)
    friend FCDECL_MONHELPER(JIT_MonEnterWorker_Portable, Object* obj);
    friend FCDECL2(FC_BOOL_RET, JIT_MonTryEnter_Portable, Object* obj, INT32 timeOut);
#endif

    friend FCDECL2(void, JIT_MonReliableEnter, Object* pThisUNSAFE, CLR_BOOL *tookLock);

  private:
    // !!! Notice: m_SyncBlockValue *MUST* be the last field in ObjHeader.
    DWORD  m_SyncBlockValue;      // the Index and the Bits


  public:

    // Access to the Sync Block Index, by masking the Value.
    DWORD GetHeaderSyncBlockIndex()
    {
        LEAF_CONTRACT;

        // pull the value out before checking it to avoid race condition
        DWORD value = m_SyncBlockValue;
        if ((value & (BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX | BIT_SBLK_IS_HASHCODE)) != BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX)
            return 0;
        return value & MASK_SYNCBLOCKINDEX;
    }
    // Ditto for setting the index, which is careful not to disturb the underlying
    // bit field -- even in the presence of threaded access.
    // 
    // This service can only be used to transition from a 0 index to a non-0 index.
    void SetIndex(DWORD indx)
    {
        CONTRACTL
        {
            INSTANCE_CHECK;
            NOTHROW;
            GC_NOTRIGGER;
            FORBID_FAULT;
            MODE_ANY;
            PRECONDITION(GetHeaderSyncBlockIndex() == 0);
            PRECONDITION(m_SyncBlockValue & BIT_SBLK_SPIN_LOCK);
        }
        CONTRACTL_END


#ifdef _DEBUG
        // if we have an index here, make sure we already transferred it to the syncblock
        // before we clear it out
        ADIndex adIndex = GetRawAppDomainIndex();
        if (adIndex.m_dwIndex)
        {
            SyncBlock *pSyncBlock = SyncTableEntry::GetSyncTableEntry() [indx & ~BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX].m_SyncBlock;
            _ASSERTE(pSyncBlock && pSyncBlock->GetAppDomainIndex() == adIndex);
        }
#endif

        LONG newValue;
        LONG oldValue;
        while (TRUE) {
            oldValue = *(volatile LONG*)&m_SyncBlockValue;
            _ASSERTE(GetHeaderSyncBlockIndex() == 0);
            // or in the old value except any index that is there - 
            // note that indx could be carrying the BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX bit that we need to preserve
            newValue = (indx | 
                (oldValue & ~(BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX | BIT_SBLK_IS_HASHCODE | MASK_SYNCBLOCKINDEX)));
            if (FastInterlockCompareExchange((LONG*)&m_SyncBlockValue, 
                                             newValue, 
                                             oldValue)
                == oldValue)
            {
                return;
            }
        }
    }

    // Used only during shutdown
    void ResetIndex()
    {
        LEAF_CONTRACT;

        _ASSERTE(m_SyncBlockValue & BIT_SBLK_SPIN_LOCK);
        FastInterlockAnd(&m_SyncBlockValue, ~(BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX | BIT_SBLK_IS_HASHCODE | MASK_SYNCBLOCKINDEX));
    }

    // Used only GC
    void GCResetIndex()
    {
        LEAF_CONTRACT;

        m_SyncBlockValue &=~(BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX | BIT_SBLK_IS_HASHCODE | MASK_SYNCBLOCKINDEX);
    }

    void SetAppDomainIndex(ADIndex);
    void ResetAppDomainIndex(ADIndex);
    void ResetAppDomainIndexNoFailure(ADIndex);
    ADIndex GetRawAppDomainIndex();
    ADIndex GetAppDomainIndex();


    // For now, use interlocked operations to twiddle bits in the bitfield portion.
    // If we ever have high-performance requirements where we can guarantee that no
    // other threads are accessing the ObjHeader, this can be reconsidered for those
    // particular bits.
    void SetBit(DWORD bit)
    {
        LEAF_CONTRACT;

        _ASSERTE((bit & MASK_SYNCBLOCKINDEX) == 0);
        FastInterlockOr(&m_SyncBlockValue, bit);
    }
    void ClrBit(DWORD bit)
    {
        LEAF_CONTRACT;

        _ASSERTE((bit & MASK_SYNCBLOCKINDEX) == 0);
        FastInterlockAnd(&m_SyncBlockValue, ~bit);
    }
    //GC accesses this bit when all threads are stopped. 
    void SetGCBit()
    {
        LEAF_CONTRACT;

        m_SyncBlockValue |= BIT_SBLK_GC_RESERVE;
    }
    void ClrGCBit()
    {
        LEAF_CONTRACT;

        m_SyncBlockValue &= ~BIT_SBLK_GC_RESERVE;
    }


    DWORD GetBits()
    {
        LEAF_CONTRACT;


        return m_SyncBlockValue;
    }


    DWORD SetBits(DWORD newBits, DWORD oldBits)
    {
        LEAF_CONTRACT;

        _ASSERTE((oldBits & BIT_SBLK_SPIN_LOCK) == 0);
        DWORD result = FastInterlockCompareExchange((LONG*)&m_SyncBlockValue, newBits, oldBits);
        return result;
    }

#ifdef _DEBUG
    BOOL HasEmptySyncBlockInfo()
    {
        WRAPPER_CONTRACT;
        return m_SyncBlockValue == 0;
    }
#endif

    // TRUE if the header has a real SyncBlockIndex (i.e. it has an entry in the
    // SyncTable, though it doesn't necessarily have an entry in the SyncBlockCache)
    BOOL HasSyncBlockIndex()
    {
        WRAPPER_CONTRACT;
        return (GetHeaderSyncBlockIndex() != 0);
    }

    // retrieve or allocate a sync block for this object
    SyncBlock *GetSyncBlock();

    // retrieve sync block but don't allocate
    SyncBlock *PassiveGetSyncBlock()
    {
#ifndef DACCESS_COMPILE
        LEAF_CONTRACT;

        return g_pSyncTable [GetHeaderSyncBlockIndex()].m_SyncBlock;
#else
        DacNotImpl();
        return NULL;
#endif // !DACCESS_COMPILE
    }


    // COM Interop has special access to sync blocks
    // check .cpp file for more info
    SyncBlock* GetSyncBlockSpecial();

    DWORD GetSyncBlockIndex();

    // this enters the monitor of an object
    void EnterObjMonitor();

    // non-blocking version of above
    BOOL TryEnterObjMonitor(INT32 timeOut = 0);

    // leaves the monitor of an object
    BOOL LeaveObjMonitor();

    // should be called only from unwind code
    BOOL LeaveObjMonitorAtException();

    // Helper encapsulating the core logic for releasing monitor. Returns what kind of
    // follow up action is necessary
    FORCEINLINE AwareLock::LeaveHelperAction LeaveObjMonitorHelper(Thread* pCurThread);

    BOOL IsObjMonitorOwnedByThread(Thread* pExpectedThread);

    Object *GetBaseObject()
    {
        LEAF_CONTRACT;
        return (Object *) (this + 1);
    }

    BOOL Wait(INT32 timeOut, BOOL exitContext);
    void Pulse();
    void PulseAll();

    void EnterSpinLock();
    void ReleaseSpinLock();

    BOOL Validate (BOOL bVerifySyncBlkIndex = TRUE);
};

// A SyncBlock contains an m_Link field that is used for two purposes.  One
// is to manage a FIFO queue of threads that are waiting on this synchronization
// object.  The other is to thread free SyncBlocks into a list for recycling.
// We don't want to burn anything else on the SyncBlock instance, so we can't
// use an SList or similar data structure.  So here's the encapsulation for the
// queue of waiting threads.
//
// Note that Enqueue is slower than it needs to be, because we don't want to
// burn extra space in the SyncBlock to remember the head and the tail of the Q.
// An alternate approach would be to treat the list as a LIFO stack, which is not
// a fair policy because it permits to starvation.

struct ThreadQueue
{
    // Given a link in the chain, get the Thread that it represents
    static WaitEventLink *WaitEventLinkForLink(SLink *pLink);

    // Unlink the head of the Q.  We are always in the SyncBlock's critical
    // section.
    static WaitEventLink *DequeueThread(SyncBlock *psb);

    // Enqueue is the slow one.  We have to find the end of the Q since we don't
    // want to burn storage for this in the SyncBlock.
    static void         EnqueueThread(WaitEventLink *pWaitEventLink, SyncBlock *psb);
    
    // Wade through the SyncBlock's list of waiting threads and remove the
    // specified thread.
    static BOOL         RemoveThread (Thread *pThread, SyncBlock *psb);
};


// The true size of an object is whatever C++ thinks, plus the ObjHeader we
// allocate before it.

#define ObjSizeOf(c)    (sizeof(c) + sizeof(ObjHeader))

// non-zero return value if this function causes the OS to switch to another thread
BOOL __SwitchToThread (DWORD dwSleepMSec);
BOOL __DangerousSwitchToThread (DWORD dwSleepMSec, BOOL goThroughOS);


inline void AwareLock::SetPrecious()
{
    LEAF_CONTRACT;

    m_dwSyncIndex |= SyncBlock::SyncBlockPrecious;
}

#ifdef _X86_
#include <poppack.h>
#endif // _X86_

#endif // _SYNCBLK_H_


