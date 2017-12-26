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
/*============================================================
**
** Header:  AppDomain.cpp
**
** Purpose: Implements AppDomain (loader domain) architecture
**
** Date:  Dec 1, 1998
**
===========================================================*/
#ifndef _APPDOMAIN_H
#define _APPDOMAIN_H

#include "assembly.hpp"
#include "clsload.hpp"
#include "eehash.h"
#include "fusion.h"
#include "arraylist.h"
#include "comreflectioncache.hpp"
#include "comreflectioncommon.h"
#include "priorityqueue.h"
#include "domainfile.h"
#include "objectlist.h"
#include "fptrstubs.h"
#include "testhookmgr.h"
#include "securitydescriptorappdomain.h"
class MetaSigCache;
class BaseDomain;
class SystemDomain;
class SharedDomain;
class AppDomain;
class CompilationDomain;
class AppDomainEnum;
class AssemblySink;
class EEMarshalingData;
class Context;
class GlobalStringLiteralMap;
class AppDomainStringLiteralMap;
struct SecurityContext;
class ArgBasedStubCache;
class MngStdInterfacesInfo;
class VirtualCallStubManager;
class DomainModule;
class DomainAssembly;
struct InteropMethodTableData;
class LoadLevelLimiter;


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4200) // Disable zero-sized array warning
#endif



// This enum is aligned to System.ExceptionCatcherType. 
enum ExceptionCatcher {
    ExceptionCatcher_ManagedCode = 0,
    ExceptionCatcher_AppDomainTransition = 1,
    ExceptionCatcher_COMInterop = 2,    
};

struct DomainLocalModule
{
    friend class ClrDataAccess;
    friend class CheckAsmOffsets;
    friend class ZapMonitor;
    struct DynamicEntry
    {
        OBJECTREF*      m_pGCStatics;
        BYTE            m_pDataBlob[0];

        static SIZE_T GetOffsetOfDataBlob() { return offsetof(DynamicEntry, m_pDataBlob); }
            
        inline TADDR GetNonGCStaticsBasePointer()
        {
            LEAF_CONTRACT
            return PTR_HOST_TO_TADDR(this);
        }

        inline TADDR  GetGCStaticsBasePointer()
        {
            LEAF_CONTRACT;
            return (TADDR)m_pGCStatics;
        }
    };
    typedef DPTR(DynamicEntry) PTR_DynamicEntry;

    struct DynamicClassInfo
    {
        PTR_VOLATILE PTR_DynamicEntry   m_pDynamicEntry;
        volatile DWORD                  m_dwFlags;
    };
    typedef DPTR(DynamicClassInfo) PTR_DynamicClassInfo;
    
    // We would like *ALLOCATECLASS_FLAG to AV (in order to catch errors), so don't change it
    enum
    {
        INITIALIZED_FLAG_BIT    = 0,
        INITIALIZED_FLAG        = 1<<INITIALIZED_FLAG_BIT,
        ERROR_FLAG_BIT          = 1,
        ERROR_FLAG              = 1<<ERROR_FLAG_BIT,
        ALLOCATECLASS_FLAG_BIT  = 2,                    // Bit to avoid racing for InstantiateStaticHandles
        ALLOCATECLASS_FLAG      = 1<<ALLOCATECLASS_FLAG_BIT,
    };

    inline volatile UMEntryThunk* GetADThunkTable()
    {
        LEAF_CONTRACT
        return m_pADThunkTable;
    }

    inline void SetADThunkTable(UMEntryThunk* pADThunkTable)
    {
        LEAF_CONTRACT
        InterlockedCompareExchangePointer((volatile PVOID*)&m_pADThunkTable,pADThunkTable,NULL);

    }

    void         Init()
    {
        LEAF_CONTRACT
        m_pADThunkTable = 0;
        m_pGCStatics    = 0;
    }

    // Note the difference between:
    // 
    //  GetPrecomputedNonGCStaticsBasePointer() and
    //  GetPrecomputedStaticsClassData()
    //
    //  GetPrecomputedNonGCStaticsBasePointer returns the pointer that should be added to field offsets to retrieve statics
    //  GetPrecomputedStaticsClassData returns a pointer to the first byte of the precomputed statics block
    inline TADDR GetPrecomputedNonGCStaticsBasePointer()
    {
        LEAF_CONTRACT
        return PTR_HOST_TO_TADDR(this);
    }

    inline BYTE* GetPrecomputedStaticsClassData()
    {
        LEAF_CONTRACT
        return (BYTE*)(PTR_HOST_TO_TADDR(this) +
                       offsetof(DomainLocalModule, m_pDataBlob));
    }

    static SIZE_T GetOffsetOfDataBlob() { return offsetof(DomainLocalModule, m_pDataBlob); }
    static SIZE_T GetOffsetOfGCStaticPointer() { return offsetof(DomainLocalModule, m_pGCStatics); }

    inline DomainFile* GetDomainFile()
    {
        LEAF_CONTRACT
        return m_pDomainFile;
    }

#ifndef DACCESS_COMPILE
    inline void        SetDomainFile(DomainFile* pDomainFile)
    {
        LEAF_CONTRACT
        m_pDomainFile = pDomainFile;
    }
#endif

    inline OBJECTREF* GetPrecomputedGCStaticsBasePointer()
    {
        LEAF_CONTRACT        
        return m_pGCStatics;
    }

    inline OBJECTREF** GetPrecomputedGCStaticsBasePointerAddress()
    {
        LEAF_CONTRACT        
        return &m_pGCStatics;
    }

    // Returns bytes so we can add offsets
    inline TADDR GetGCStaticsBasePointer(MethodTable* pMT)
    {
        WRAPPER_CONTRACT
        if (pMT->IsDynamicStatics())
        {
            if (pMT->ContainsGenericVariables())
            {
                return NULL;
            }

            _ASSERTE(GetDomainFile()->GetModule() == pMT->GetModuleForStatics());
            return GetDynamicEntryGCStaticsBasePointer(pMT->GetModuleDynamicEntryID());
        }
        else
        {
            return (TADDR)m_pGCStatics;
        }
    }

    inline TADDR GetNonGCStaticsBasePointer(MethodTable* pMT)
    {
        WRAPPER_CONTRACT

        if (pMT->IsDynamicStatics())
        {
            if (pMT->ContainsGenericVariables())
            {
                return NULL;
            }

            _ASSERTE(GetDomainFile()->GetModule() == pMT->GetModuleForStatics());
            return GetDynamicEntryNonGCStaticsBasePointer(pMT->GetModuleDynamicEntryID());
        }
        else
        {
            return PTR_HOST_TO_TADDR(this);
        }
    }

    inline DynamicEntry* GetDynamicEntry(DWORD n)
    {
        LEAF_CONTRACT
        _ASSERTE(m_pDynamicClassTable && m_aDynamicEntries > n);
        DynamicEntry* pEntry = PTR_DynamicEntry
            (PTR_TO_TADDR(m_pDynamicClassTable[n].m_pDynamicEntry));

        return pEntry;
    }

    // These helpers can now return null, as the debugger may do queries on a type
    // before the calls to PopulateClass happen
    inline TADDR GetDynamicEntryGCStaticsBasePointer(DWORD n)
    {
        WRAPPER_CONTRACT

        if (CORDebuggerAttached() && n >= m_aDynamicEntries)
        {
            return NULL;
        }
        
        DynamicEntry* pEntry = GetDynamicEntry(n);
        if (!pEntry)
        {
            return NULL;
        }

        return pEntry->GetGCStaticsBasePointer();
    }
   

    FORCEINLINE TADDR GetDynamicEntryGCStaticsBasePointerIfAllocatedAndInited(DWORD n)
    {
        WRAPPER_CONTRACT;

        // m_aDynamicEntries is set last, it needs to be checked first
        if (n >= m_aDynamicEntries)
        {
            return NULL;
        }

        PTR_DynamicClassInfo pDynamicClassTable = m_pDynamicClassTable;
        _ASSERTE(pDynamicClassTable);

        // INITIALIZED_FLAG is set last, it needs to be checked first
        if ((pDynamicClassTable[n].m_dwFlags & INITIALIZED_FLAG) == 0)
        {
            return NULL;
        }

        return PTR_DynamicEntry(PTR_TO_TADDR(pDynamicClassTable[n].m_pDynamicEntry))->GetGCStaticsBasePointer();
    }

    inline TADDR GetDynamicEntryNonGCStaticsBasePointer(DWORD n)
    {
        WRAPPER_CONTRACT
        
        if (CORDebuggerAttached() && n >= m_aDynamicEntries)
        {
            return NULL;
        }

        DynamicEntry* pEntry = GetDynamicEntry(n);
        if (!pEntry)
        {
            return NULL;
        }

        return pEntry->GetNonGCStaticsBasePointer();
    }

    FORCEINLINE TADDR GetDynamicEntryNonGCStaticsBasePointerIfAllocatedAndInited(DWORD n)
    {
        WRAPPER_CONTRACT;

        // m_aDynamicEntries is set last, it needs to be checked first
        if (n >= m_aDynamicEntries)
        {
            return NULL;
        }

        PTR_DynamicClassInfo pDynamicClassTable = m_pDynamicClassTable;
        _ASSERTE(pDynamicClassTable != NULL);

        // INITIALIZED_FLAG is set last, it needs to be checked first
        if ((pDynamicClassTable[n].m_dwFlags & INITIALIZED_FLAG) == 0)
        {
            return NULL;
        }

        return PTR_DynamicEntry(PTR_TO_TADDR(pDynamicClassTable[n].m_pDynamicEntry))->GetNonGCStaticsBasePointer();
    }

    // iClassIndex is slightly expensive to compute, so if we already know
    // it, we can use this helper
    inline BOOL IsClassInitialized(MethodTable* pMT, DWORD iClassIndex = (DWORD)-1)
    {
        WRAPPER_CONTRACT;
        return (GetClassFlags(pMT, iClassIndex) & INITIALIZED_FLAG) != 0;
    }

    inline BOOL IsDynamicClassInitialized(DWORD dynamicClassID)
    {
        WRAPPER_CONTRACT;
        
        if(m_aDynamicEntries <= dynamicClassID)
            return FALSE;
        return (m_pDynamicClassTable[dynamicClassID].m_dwFlags & INITIALIZED_FLAG) != 0;
    }

    inline BOOL IsPrecomputedClassInitialized(DWORD classID)
    {
        return (PTR_BYTE(GetPrecomputedStaticsClassData()))[classID] & INITIALIZED_FLAG;
    }
    
    inline BOOL IsClassAllocated(MethodTable* pMT, DWORD iClassIndex = (DWORD)-1)
    {
        WRAPPER_CONTRACT;
        return (GetClassFlags(pMT, iClassIndex) & ALLOCATECLASS_FLAG) != 0;
    }

    BOOL IsClassInitError(MethodTable* pMT, DWORD iClassIndex = (DWORD)-1)
    {
        WRAPPER_CONTRACT;
        return (GetClassFlags(pMT, iClassIndex) & ERROR_FLAG) != 0;
    }

    void SetClassInitialized(MethodTable* pMT)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(!IsClassInitialized(pMT));
        _ASSERTE(!IsClassInitError(pMT));

        SetClassFlags(pMT, INITIALIZED_FLAG);
    }

    void SetClassAllocatedAndInitialized(MethodTable* pMT)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(!IsClassInitialized(pMT));
        _ASSERTE(!IsClassInitError(pMT));

        SetClassFlags(pMT, ALLOCATECLASS_FLAG | INITIALIZED_FLAG);
    }

    void SetClassAllocated(MethodTable* pMT)
    {
        WRAPPER_CONTRACT;

        SetClassFlags(pMT, ALLOCATECLASS_FLAG);
    }

    void SetClassInitError(MethodTable* pMT)
    {
        WRAPPER_CONTRACT;

        SetClassFlags(pMT, ERROR_FLAG);
    }

    void    EnsureDynamicClassIndex(DWORD dwID);

    void    AllocateDynamicClass(MethodTable *pMT);

    void    PopulateClass(MethodTable *pMT);


#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    static DWORD OffsetOfDataBlob()
    {
        LEAF_CONTRACT;
        return offsetof(DomainLocalModule, m_pDataBlob);
    }

    
private:
    friend void EmitFastGetSharedStaticBase(CPUSTUBLINKER *psl, CodeLabel *init, bool bCCtorCheck);

    void SetClassFlags(MethodTable* pMT, DWORD dwFlags);
    BOOL GetClassFlags(MethodTable* pMT, DWORD iClassIndex);

    PTR_DomainFile           m_pDomainFile;
    PTR_VOLATILE PTR_DynamicClassInfo m_pDynamicClassTable;   // used for generics and reflection.emit in memory
    volatile SIZE_T          m_aDynamicEntries;      // number of entries in dynamic table
    volatile UMEntryThunk*   m_pADThunkTable;
    OBJECTREF*               m_pGCStatics;           // Handle to GC statics of the module
    BYTE                     m_pDataBlob[0];         // First byte of the statics blob

    // Layout of m_pDataBlob is:
    //              ClassInit bytes (hold flags for cctor run, cctor error, etc)
    //              Non GC Statics

};


typedef DPTR(class DomainLocalBlock) PTR_DomainLocalBlock;
class DomainLocalBlock
{
    friend class ClrDataAccess;
    friend class CheckAsmOffsets;

  private:
    PTR_AppDomain          m_pDomain;
    DPTR(PTR_DomainLocalModule) m_pModuleSlots;
    SIZE_T                 m_aModuleIndices;               // Module entries the shared block has allocated
    static SIZE_T          m_cModuleIndices;               // Module entries that have been used




  public: // used by code generators
    static SIZE_T GetOffsetOfModuleSlotsPointer() { return offsetof(DomainLocalBlock, m_pModuleSlots); }


  public:

#ifndef DACCESS_COMPILE
    DomainLocalBlock()
      : m_pDomain(NULL),  m_pModuleSlots(NULL), m_aModuleIndices(0) {}

    void    EnsureModuleIndex(SIZE_T index);

    void Init(AppDomain *pDomain) { LEAF_CONTRACT; m_pDomain = pDomain; }
#endif

    // ModuleID's are tagged with the low bit 1, so they can be stored in a unioned field
    // with DomainLocalModule pointers.
    static BOOL IsModuleID(SIZE_T ModuleID)
    {
        LEAF_CONTRACT
            
        return (ModuleID&1)==1;
    }

    static SIZE_T IndexToID(SIZE_T index)
    {
        LEAF_CONTRACT
            
        return (index<<1)|1;
    }

    static SIZE_T IDToIndex(SIZE_T ModuleID)
    {
        LEAF_CONTRACT
            
        _ASSERTE(IsModuleID(ModuleID));
        return ModuleID>>1;
    }

    static SIZE_T AllocateModuleID();

    void SetModuleSlot(SIZE_T ModuleID, PTR_DomainLocalModule pLocalModule);

    FORCEINLINE PTR_DomainLocalModule GetModuleSlot(SIZE_T ModuleID)
    {
        WRAPPER_CONTRACT;
        SIZE_T index = IDToIndex(ModuleID);
        _ASSERTE(index < m_aModuleIndices);
        return m_pModuleSlots[index];
    }

    inline PTR_DomainLocalModule GetModuleSlot(MethodTable* pMT)
    {
        WRAPPER_CONTRACT;
        return GetModuleSlot(pMT->GetModuleForStatics()->GetModuleID());
    }

    DomainFile* TryGetDomainFile(SIZE_T ModuleID)
    {
        WRAPPER_CONTRACT;
        SIZE_T index = IDToIndex(ModuleID);
        if (index < m_aModuleIndices &&
            m_pModuleSlots[index])
        {
            return m_pModuleSlots[index]->GetDomainFile();
        }

        return NULL;
    }

    DomainFile* GetDomainFile(SIZE_T ModuleID)
    {
        WRAPPER_CONTRACT;
        SIZE_T index = IDToIndex(ModuleID);
        _ASSERTE(index < m_aModuleIndices);
        return m_pModuleSlots[index]->GetDomainFile();
    }

#ifndef DACCESS_COMPILE
    void SetDomainFile(SIZE_T ModuleID, DomainFile* pDomainFile)
    {
        WRAPPER_CONTRACT;
        SIZE_T index = IDToIndex(ModuleID);
        _ASSERTE(index < m_aModuleIndices);
        m_pModuleSlots[index]->SetDomainFile(pDomainFile);
    }
#endif

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif


private:

    //
    // Low level routines to get & set class entries
    //

};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

// The large heap handle bucket class is used to contain handles allocated
// from an array contained in the large heap.
class LargeHeapHandleBucket
{
public:
    // Constructor and desctructor.
    LargeHeapHandleBucket(LargeHeapHandleBucket *pNext, DWORD Size, BaseDomain *pDomain);
    ~LargeHeapHandleBucket();

    // This returns the next bucket.
    LargeHeapHandleBucket *GetNext()
    {
        LEAF_CONTRACT;

        return m_pNext;
    }

    // This returns the number of remaining handle slots.
    DWORD GetNumRemainingHandles()
    {
        LEAF_CONTRACT;

        return m_ArraySize - m_CurrentPos;
    }

    void ConsumeRemaining()
    {
        LEAF_CONTRACT;
        
        m_CurrentPos = m_ArraySize;
    }

    OBJECTREF *TryAllocateEmbeddedFreeHandle();       

    // Allocate handles from the bucket.
    OBJECTREF* AllocateHandles(DWORD nRequested);
    OBJECTREF* CurrentPos()
    {
      LEAF_CONTRACT;
      return m_pArrayDataPtr + m_CurrentPos;
    }

private:
    LargeHeapHandleBucket *m_pNext;
    int m_ArraySize;
    int m_CurrentPos;
    int m_CurrentEmbeddedFreePos;
    OBJECTHANDLE m_hndHandleArray;
    OBJECTREF *m_pArrayDataPtr;
};

// The large heap handle table is used to allocate handles that are pointers
// to objects stored in an array in the large object heap.
class LargeHeapHandleTable
{
public:
    // Constructor and desctructor.
    LargeHeapHandleTable(BaseDomain *pDomain, DWORD BucketSize);
    ~LargeHeapHandleTable();

    // Allocate handles from the large heap handle table.
    OBJECTREF* AllocateHandles(DWORD nRequested);

    // Release object handles allocated using AllocateHandles().
    void ReleaseHandles(OBJECTREF *pObjRef, DWORD nReleased);    

private:
    // The buckets of object handles.
    LargeHeapHandleBucket *m_pHead;

    // We need to know the containing domain so we know where to allocate handles
    BaseDomain *m_pDomain;

    // The size of the LargeHeapHandleBuckets.
    DWORD m_BucketSize;

    // for finding and re-using embedded free items in the list
    LargeHeapHandleBucket *m_pFreeSearchHint;
    DWORD m_cEmbeddedFree;   

#ifdef _DEBUG

    // these functions are present to enforce that there is a locking mechanism in place
    // for each LargeHeapHandleTable even though the code itself does not do the locking
    // you must tell the table which lock you intend to use and it will verify that it has
    // in fact been taken before performing any operations

public:
    void RegisterCrstDebug(CrstBase *pCrst)
    {
        LEAF_CONTRACT;

        // this function must be called exactly once
        _ASSERTE(pCrst != NULL);
        _ASSERTE(m_pCrstDebug == NULL);
        m_pCrstDebug = pCrst;
    }

private:
    // we will assert that this Crst is held before using the object
    CrstBase *m_pCrstDebug;

#endif
    
};

class LargeHeapHandleBlockHolder;
void LargeHeapHandleBlockHolder__StaticFree(LargeHeapHandleBlockHolder*);


class LargeHeapHandleBlockHolder:public Holder<LargeHeapHandleBlockHolder*,DoNothing,LargeHeapHandleBlockHolder__StaticFree>

{
    LargeHeapHandleTable* m_pTable;
    DWORD m_Count;
    OBJECTREF* m_Data;
public:
    FORCEINLINE LargeHeapHandleBlockHolder(LargeHeapHandleTable* pOwner, DWORD nCount)
    {
        WRAPPER_CONTRACT;
        m_Data = pOwner->AllocateHandles(nCount);
        m_Count=nCount;
        m_pTable=pOwner;
    };

    FORCEINLINE void FreeData()
    {
        WRAPPER_CONTRACT;
        for (DWORD i=0;i< m_Count;i++)
            ClearObjectReference(m_Data+i);
        m_pTable->ReleaseHandles(m_Data, m_Count);
    };
    FORCEINLINE OBJECTREF* operator[] (DWORD idx)
    {
        LEAF_CONTRACT;
        _ASSERTE(idx<m_Count);
        return &(m_Data[idx]);
    }
};

FORCEINLINE  void LargeHeapHandleBlockHolder__StaticFree(LargeHeapHandleBlockHolder* pHolder)
{
    WRAPPER_CONTRACT;
    pHolder->FreeData();
};

//--------------------------------------------------------------------------------------
// Base class for domains. It provides an abstract way of finding the first assembly and
// for creating assemblies in the the domain. The system domain only has one assembly, it
// contains the classes that are logically shared between domains. All other domains can
// have multiple assemblies. Iteration is done be getting the first assembly and then
// calling the Next() method on the assembly.
//
// The system domain should be as small as possible, it includes object, exceptions, etc.
// which are the basic classes required to load other assemblies. All other classes
// should be loaded into the domain. Of coarse there is a trade off between loading the
// same classes multiple times, requiring all domains to load certain assemblies (working
// set) and being able to specify specific versions.
//

#define LOW_FREQUENCY_HEAP_RESERVE_SIZE        (2 * PAGE_SIZE)
#define LOW_FREQUENCY_HEAP_COMMIT_SIZE         (1 * PAGE_SIZE)

#define HIGH_FREQUENCY_HEAP_RESERVE_SIZE       (8 * PAGE_SIZE)
#define HIGH_FREQUENCY_HEAP_COMMIT_SIZE        (1 * PAGE_SIZE)

#define STUB_HEAP_RESERVE_SIZE                 (2 * PAGE_SIZE)
#define STUB_HEAP_COMMIT_SIZE                  (1 * PAGE_SIZE)


// --------------------------------------------------------------------------------
// PE File List lock - for creating list locks on PE files
// --------------------------------------------------------------------------------

class PEFileListLock : public ListLock
{
  public:
#ifndef DACCESS_COMPILE
    ListLockEntry *FindFileLock(PEFile *pFile)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_NOTRIGGER;
        STATIC_CONTRACT_FORBID_FAULT;

        PRECONDITION(HasLock());

        ListLockEntry *pEntry;

        for (pEntry = m_pHead;
             pEntry != NULL;
             pEntry = pEntry->m_pNext)
        {
            if (((PEFile *)pEntry->m_pData)->Equals(pFile))
            {
                return pEntry;
            }
        }

        return NULL;
    }
#endif // DACCESS_COMPILE

    static void HolderEnter(PEFileListLock *pThis)
    {
        WRAPPER_CONTRACT;

        pThis->Enter();
    }

    static void HolderLeave(PEFileListLock *pThis)
    {
        WRAPPER_CONTRACT;

        pThis->Leave();
    }

    typedef Wrapper<PEFileListLock*, PEFileListLock::HolderEnter, PEFileListLock::HolderLeave> Holder;
};

typedef PEFileListLock::Holder PEFileListLockHolder;

// Loading infrastructure:
//
// a DomainFile is a file being loaded.  Files are loaded in layers to enable loading in the
// presence of dependency loops.
//
// FileLoadLevel describes the various levels available.  These are implemented slightly
// differently for assemblies and modules, but the basic structure is the same.
//
// LoadLock and FileLoadLock form the ListLock data structures for files. The FileLoadLock
// is specialized in that it allows taking a lock at a particular level.  Basicall any
// thread may obtain the lock at a level at which the file has previously been loaded to, but
// only one thread may obtain the lock at its current level.
//
// The PendingLoadQueue is a per thread data structure which serves two purposes.  First, it
// holds a "load limit" which automatically restricts the level of recursive loads to be
// one less than the current load which is preceding.  This, together with the AppDomain
// LoadLock level behavior, will prevent any deadlocks from occuring due to circular
// dependencies.  (Note that it is important that the loading logic understands this restriction,
// and any given level of loading must deal with the fact that any recursive loads will be partially
// unfulfilled in a specific way.)
//
// The second function is to queue up any unfulfilled load requests for the thread.  These
// are then delivered immediately after the current load request is dealt with.

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(FileLoadLock*);
template BOOL CompareDefault(FileLoadLock*,FileLoadLock*);
template void DoTheRelease(FileLoadLock*);
typedef Wrapper<FileLoadLock *, DoNothing, DoTheRelease<FileLoadLock>,NULL> HACKFileLoadLockRefHolder;
#endif

class FileLoadLock : public ListLockEntry
{
  private:
    FileLoadLevel           m_level;
    DomainFile              *m_pDomainFile;
    HRESULT                 m_cachedHR;
    ADID                    m_AppDomainId;

  public:
    static FileLoadLock *Create(PEFileListLock *pLock, PEFile *pFile, DomainFile *pDomainFile);

    ~FileLoadLock();
    DomainFile *GetDomainFile();
    ADID GetAppDomainId();
    FileLoadLevel GetLoadLevel();

    // CanAcquire will return FALSE if Acquire will definitely not take the lock due
    // to levels or deadlock.
    // (Note that there is a race exiting from the function, where Acquire may end
    // up not taking the lock anyway if another thread did work in the meantime.)
    BOOL CanAcquire(FileLoadLevel targetLevel);

    // Acquire will return FALSE and not take the lock if the file
    // has already been loaded to the target level.  Otherwise,
    // it will return TRUE and take the lock.
    //
    // Note that the taker must release the lock via IncrementLoadLevel.
    BOOL Acquire(FileLoadLevel targetLevel);

    // CompleteLoadLevel can be called after Acquire returns true
    // returns TRUE if it updated load level, FALSE if the level was set already
    BOOL CompleteLoadLevel(FileLoadLevel level, BOOL success);

    void SetError(Exception *ex);

    void AddRef();
    UINT32 Release() DAC_EMPTY_RET(0);

  private:

    FileLoadLock(PEFileListLock *pLock, PEFile *pFile, DomainFile *pDomainFile);

    static void HolderLeave(FileLoadLock *pThis);

  public:
    typedef Wrapper<FileLoadLock *, DoNothing, FileLoadLock::HolderLeave> Holder;
    
};

typedef FileLoadLock::Holder FileLoadLockHolder;
#ifndef DACCESS_COMPILE
typedef Wrapper<FileLoadLock *, DoNothing, DoTheRelease<FileLoadLock>,NULL> FileLoadLockRefHolder;
#endif // DACCESS_COMPILE


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning (disable: 4324) //sometimes 64bit compilers complain about alignment
#endif
class LoadLevelLimiter
{
    FileLoadLevel                   m_currentLevel;
    LoadLevelLimiter* m_previousLimit;
    BOOL m_bActive;

  public:

    LoadLevelLimiter()
      : m_currentLevel(FILE_ACTIVE),
      m_previousLimit(NULL),
      m_bActive(FALSE)
    {
        LEAF_CONTRACT;
    }

    void Activate()
    {
        WRAPPER_CONTRACT;
        m_previousLimit=GetThread()->GetLoadLevelLimiter();
        if(m_previousLimit)
            m_currentLevel=m_previousLimit->GetLoadLevel();
        GetThread()->SetLoadLevelLimiter(this);       
        m_bActive=TRUE;
    }

    void Deactivate()
    {
        WRAPPER_CONTRACT;
        if (m_bActive)
        {
            GetThread()->SetLoadLevelLimiter(m_previousLimit);
            m_bActive=FALSE;
        }
    }

    ~LoadLevelLimiter()
    {
        WRAPPER_CONTRACT;

        // PendingLoadQueues are allocated on the stack during a load, and
        // shared with all nested loads on the same thread.

        // Make sure the thread pointer gets reset after the
        // top level queue goes out of scope.
        if(m_bActive)
            Deactivate();        

    }

    FileLoadLevel GetLoadLevel()
    {
        LEAF_CONTRACT;
        return m_currentLevel;
    }

    void SetLoadLevel(FileLoadLevel level)
    {
        LEAF_CONTRACT;
        m_currentLevel = level;
    }
};
#ifdef _MSC_VER
#pragma warning (pop) //4324
#endif

#define OVERRIDE_LOAD_LEVEL_LIMIT(newLimit)                    \
    LoadLevelLimiter __newLimit;                                                    \
    __newLimit.Activate();                                                              \
    __newLimit.SetLoadLevel(newLimit);

class BaseDomain
{
    friend class Assembly;
    friend class AssemblySpec;
    friend class AppDomain;
    friend class AppDomainNative;

    VPTR_BASE_CONCRETE_VTABLE_CLASS(BaseDomain)
    VPTR_UNIQUE(VPTR_UNIQUE_BaseDomain)

public:

    class AssemblyIterator;
    friend class AssemblyIterator;

    //****************************************************************************************
    //
    // Initialization/shutdown routines for every instance of an BaseDomain.
    BaseDomain();
    void Init();
    void Stop();
    void Terminate();
    void LazyInitStringLiteralMap();
    
    virtual BOOL IsAppDomain()    { LEAF_CONTRACT; return FALSE; }
    virtual BOOL IsSharedDomain() { LEAF_CONTRACT; return FALSE; }

    inline BOOL IsDefaultDomain();  // defined later in this file

    virtual AppDomain *AsAppDomain()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        _ASSERTE(!"Not an AppDomain");
        return NULL;
    }

    
     // If one domain is the SharedDomain and one is an AppDomain then
     // return the AppDomain, i.e. return the domain with the shorter lifetime
     // of the two given domains.
     static BaseDomain* ComputeBaseDomain(BaseDomain *pGenericDefinitionDomain,   // the domain that owns the generic type or method
                                           DWORD numGenericClassArgs,              // the number of type arguménts to the type
                                           TypeHandle *pGenericClassArgs,          // the type arguments to the type (if any)
                                           DWORD numGenericMethodArgs = 0,         // the number of type arguments to the method
                                           TypeHandle *pGenericMethodArgs = NULL); // the type arguments to the method (if any)

     static BaseDomain* ComputeBaseDomain(TypeKey *pTypeKey);


    //****************************************************************************************
    // For in-place new()
    LoaderHeap* GetLowFrequencyHeap()
    {
        LEAF_CONTRACT;
        return m_pLowFrequencyHeap;
    }

    LoaderHeap* GetHighFrequencyHeap()
    {
        LEAF_CONTRACT;
        return m_pHighFrequencyHeap;
    }

    LoaderHeap* GetStubHeap()
    {
        LEAF_CONTRACT;
        return m_pStubHeap;
    }

    FuncPtrStubs * GetFuncPtrStubs()
    {
        LEAF_CONTRACT;
        return m_pFuncPtrStubs;
    }

    // We will use the LowFrequencyHeap for reflection purpose.  
    LoaderHeap* GetReflectionHeap()
    {
        LEAF_CONTRACT;
        return m_pLowFrequencyHeap;
    }



    //****************************************************************************************
    // This method returns marshaling data that the EE uses that is stored on a per app domain
    // basis.
    EEMarshalingData *GetMarshalingData();

   //****************************************************************************************
    // Methods to retrieve a pointer to the COM+ string STRINGREF for a string constant.
    // If the string is not currently in the hash table it will be added and if the
    // copy string flag is set then the string will be copied before it is inserted.
    STRINGREF *GetStringObjRefPtrFromUnicodeString(EEStringData *pStringData);

#ifdef _DEBUG
    BOOL OwnDomainLocalBlockLock()
    {
        WRAPPER_CONTRACT;

        return m_DomainLocalBlockCrst.OwnedByCurrentThread();
    }
#endif

    //****************************************************************************************
    //
    //  Set the shadow copy option for the domain.
    BOOL IsShadowCopyOn();

    //****************************************************************************************
    //

    //****************************************************************************************
    //
#ifndef DACCESS_COMPILE
    void SetExecutable(BOOL value) {LEAF_CONTRACT;  m_fExecutable = value != 0; }
    BOOL IsExecutable() {LEAF_CONTRACT;  return m_fExecutable; }
#endif // DACCESS_COMPILE

    virtual ApplicationSecurityDescriptor* GetSecurityDescriptor() { LEAF_CONTRACT; return NULL; }


    //****************************************************************************************
    // Get the class init lock. The method is limited to friends because inappropriate use
    // will cause deadlocks in the system
    ListLock*  GetClassInitLock()
    {
        LEAF_CONTRACT;

        return &m_ClassInitLock;
    }

    ListLock* GetJitLock()
    {
        LEAF_CONTRACT;
        return &m_JITLock;
    }


    STRINGREF *IsStringInterned(STRINGREF *pString);
    STRINGREF *GetOrInternString(STRINGREF *pString);

    virtual BOOL CanUnload()   { LEAF_CONTRACT; return FALSE; }    // can never unload BaseDomain

    // Returns an array of OBJECTREF* that can be used to store domain specific data.
    // Statics and reflection info (Types, MemberInfo,..) are stored this way
    // If ppLazyAllocate != 0, allocation will only take place if *ppLazyAllocate != 0 (and the allocation
    // will be properly serialized)
    OBJECTREF *AllocateObjRefPtrsInLargeTable(int nRequested, OBJECTREF** ppLazyAllocate = NULL);


    //****************************************************************************************
    // Handles

#ifndef DACCESS_COMPILE // needs GetCurrentThreadHomeHeapNumber
    OBJECTHANDLE CreateTypedHandle(OBJECTREF object, int type)
    {
        WRAPPER_CONTRACT;
        return ::CreateTypedHandle(m_hHandleTableBucket->pTable[GetCurrentThreadHomeHeapNumber()], object, type);
    }

    OBJECTHANDLE CreateHandle(OBJECTREF object)
    {
        WRAPPER_CONTRACT;
        return ::CreateHandle(m_hHandleTableBucket->pTable[GetCurrentThreadHomeHeapNumber()], object);
    }

    OBJECTHANDLE CreateWeakHandle(OBJECTREF object)
    {
        WRAPPER_CONTRACT;
        return ::CreateWeakHandle(m_hHandleTableBucket->pTable[GetCurrentThreadHomeHeapNumber()], object);
    }

    OBJECTHANDLE CreateShortWeakHandle(OBJECTREF object)
    {
        WRAPPER_CONTRACT;
        return ::CreateShortWeakHandle(m_hHandleTableBucket->pTable[GetCurrentThreadHomeHeapNumber()], object);
    }

    OBJECTHANDLE CreateLongWeakHandle(OBJECTREF object)
    {
        WRAPPER_CONTRACT;
        return ::CreateLongWeakHandle(m_hHandleTableBucket->pTable[GetCurrentThreadHomeHeapNumber()], object);
    }

    OBJECTHANDLE CreateStrongHandle(OBJECTREF object)
    {
        WRAPPER_CONTRACT;
        return ::CreateStrongHandle(m_hHandleTableBucket->pTable[GetCurrentThreadHomeHeapNumber()], object);
    }

    OBJECTHANDLE CreatePinningHandle(OBJECTREF object)
    {
        WRAPPER_CONTRACT;
#if CHECK_APP_DOMAIN_LEAKS     
        if(IsAppDomain())
            object->TryAssignAppDomain((AppDomain*)this,TRUE);
#endif
        return ::CreatePinningHandle(m_hHandleTableBucket->pTable[GetCurrentThreadHomeHeapNumber()], object);
    }


    OBJECTHANDLE CreateVariableHandle(OBJECTREF object, UINT type)
    {
        WRAPPER_CONTRACT;
        return ::CreateVariableHandle(m_hHandleTableBucket->pTable[GetCurrentThreadHomeHeapNumber()], object, type);
    }

#endif // DACCESS_COMPILE

    BOOL ContainsOBJECTHANDLE(OBJECTHANDLE handle);

    IApplicationContext *GetFusionContext() {LEAF_CONTRACT;  return m_pFusionContext; }



protected:

    //****************************************************************************************
    // Helper method to initialize the large heap handle table.
    void InitLargeHeapHandleTable();

    //****************************************************************************************
    //
    // Adds an assembly to the domain.
    void AddAssemblyNoLock(Assembly* assem);

    //****************************************************************************************
    //
    // Hash table that maps a MethodTable to COM Interop compatibility data.
    PtrHashMap          m_interopDataHash;

    //****************************************************************************************
    //
    // Heaps for allocating data that persists for the life of the AppDomain
    // Objects that are allocated frequently should be allocated into the HighFreq heap for
    // better page management
    BYTE *              m_InitialReservedMemForLoaderHeaps;
    BYTE                m_LowFreqHeapInstance[sizeof(LoaderHeap)];
    BYTE                m_HighFreqHeapInstance[sizeof(LoaderHeap)];
    BYTE                m_StubHeapInstance[sizeof(LoaderHeap)];
    PTR_LoaderHeap      m_pLowFrequencyHeap;
    PTR_LoaderHeap      m_pHighFrequencyHeap;
    PTR_LoaderHeap      m_pStubHeap; // stubs for PInvoke, remoting, etc
    FuncPtrStubs *      m_pFuncPtrStubs; // for GetMultiCallableAddrOfCode()



    // Critical sections & locks
    PEFileListLock      m_FileLoadLock;     // Protects the list of assemblies in the domain
    CrstExplicitInit    m_DomainCrst;          // General Protection for the Domain
    CrstExplicitInit    m_DomainCacheCrst;     // Protects the Assembly and Unmanaged caches
    CrstExplicitInit    m_DomainLocalBlockCrst;
    CrstExplicitInit    m_InteropDataCrst;     // Used for COM Interop compatiblilty
    ListLock            m_ClassInitLock;
    ListLock            m_JITLock;

    // Fusion context, used for adding assemblies to the is domain. It defines
    // fusion properties for finding assemblyies such as SharedBinPath,
    // PrivateBinPath, Application Directory, etc.
    IApplicationContext* m_pFusionContext; // Binding context for the domain

    HandleTableBucket *m_hHandleTableBucket;

    // The AppDomain specific string literal map.
    AppDomainStringLiteralMap   *m_pStringLiteralMap;

    // The large heap handle table.
    LargeHeapHandleTable        *m_pLargeHeapHandleTable;

    // The large heap handle table critical section.
    CrstExplicitInit    m_LargeHeapHandleTableCrst;

    EEMarshalingData            *m_pMarshalingData;



#ifndef DACCESS_COMPILE
    static BOOL         m_fShadowCopy;
    static BOOL         m_fExecutable;
#endif // DACCESS_COMPILE

    // Number of allocated slots for thread local statics of this domain
    DWORD m_dwThreadStatics;

    // Number of allocated slots for thread local statics of this domain
    DWORD m_dwContextStatics;

    // Protects allocation of slot IDs for thread and context statics
    static CrstStatic   m_SpecialStaticsCrst;

public:
    // Lazily allocate offset for thread of context static
    DWORD AllocateThreadOrContextStaticsOffset(DWORD* pOffsetSlot, BOOL fContextStatics);

public:
    // Only call this routine when you can guarantee there are no
    // loads in progress.
    void ClearFusionContext();

public:

    //****************************************************************************************
    // Synchronization holders.

    class LockHolder : public CrstPreempHolder
    {
    public:
        LockHolder(BaseDomain *pD, BOOL Take = TRUE)
            : CrstPreempHolder(&pD->m_DomainCrst, Take)
        {
            LEAF_CONTRACT;
        }
    };
    friend class LockHolder;

    class CacheLockHolder : public CrstPreempHolder
    {
    public:
        CacheLockHolder(BaseDomain *pD, BOOL Take = TRUE)
            : CrstPreempHolder(&pD->m_DomainCacheCrst, Take)
        {
            LEAF_CONTRACT;
        }
    };
    friend class CacheLockHolder;

    class DomainLocalBlockLockHolder : public CrstPreempHolder
    {
    public:
        DomainLocalBlockLockHolder(BaseDomain *pD, BOOL Take = TRUE)
            : CrstPreempHolder(&pD->m_DomainLocalBlockCrst, Take)
        {
            LEAF_CONTRACT;
        }
    };
    friend class DomainLocalBlockLockHolder;

    class LoadLockHolder : private GCCoop, public PEFileListLockHolder
    {
    public:
        LoadLockHolder(BaseDomain *pD, BOOL Take = TRUE)
          : PEFileListLockHolder(&pD->m_FileLoadLock, Take)
        {
            LEAF_CONTRACT;
        }
    };
    friend class LoadLockHolder;

// Code for VirtualCallStubManager
private:
    VirtualCallStubManager *m_pVirtualCallStubManager;

    inline void SetVirtualCallStubManager(VirtualCallStubManager *pMgr)
        {LEAF_CONTRACT;  m_pVirtualCallStubManager = pMgr; }

public:
    inline VirtualCallStubManager *GetVirtualCallStubManager()
        {LEAF_CONTRACT;  return m_pVirtualCallStubManager; }

    inline void ClearVirtualCallStubManager()
        {LEAF_CONTRACT; SetVirtualCallStubManager(NULL); }

    void InitVirtualCallStubManager();

private:
    TypeIDMap m_typeIDMap;

public:
    UINT32 GetTypeID(PTR_MethodTable pMT);
    UINT32 LookupTypeID(PTR_MethodTable pMT);
    PTR_MethodTable LookupType(UINT32 id);

#ifdef DACCESS_COMPILE
public:
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                                   bool enumThis);
#endif
};

enum
{
        ATTACH_ASSEMBLY_LOAD = 0x1,
        ATTACH_MODULE_LOAD = 0x2,
        ATTACH_CLASS_LOAD = 0x4,

        ATTACH_ALL = 0x7
};

class ADUnloadSink
{
    
protected:
    ~ADUnloadSink();
    CLREvent m_UnloadCompleteEvent;
    HRESULT   m_UnloadResult;
    volatile LONG m_cRef;
public:
    ADUnloadSink();
    void ReportUnloadResult (HRESULT hr, OBJECTREF* pException);
    void WaitUnloadCompletion();
    HRESULT GetUnloadResult() {LEAF_CONTRACT; return m_UnloadResult;};
    void Reset();
    ULONG AddRef();
    ULONG Release();
};


FORCEINLINE void ADUnloadSink__Release(ADUnloadSink* pADSink)
{
    WRAPPER_CONTRACT;

    if (pADSink)
        pADSink->Release();
}

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(ADUnloadSink*);
template BOOL CompareDefault(ADUnloadSink*,ADUnloadSink*);
#endif
typedef Wrapper <ADUnloadSink*,DoNothing,ADUnloadSink__Release,NULL> ADUnloadSinkHolder;

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(AppDomain*);
template BOOL CompareDefault(AppDomain*,AppDomain*);
template void DoTheRelease(AppDomain*);
typedef Wrapper<AppDomain*,DoNothing<AppDomain*>,DoTheRelease<AppDomain>,NULL> HACKAppDomainHolder;
#endif


// This filters the output of IterateAssemblies. This ought to be declared more locally
// but it would result in really verbose callsites.
//
// Assemblies can be categorized as loaded or loading.
// Indepedently, they can also be categorized as execution or introspection.
//
// An assembly will be included in the results of IterateAssemblies only if
// the appropriate bit is set for *both* characterizations.
//
// The flags can be combined so if you want all loaded assemblies, you must specify:
//
///     kIncludeLoaded|kIncludeExecution|kIncludeIntrospection

enum AssemblyIterationFlags
{
    kIncludeLoaded        = 0x00000001,       // include assemblies that are already loaded
    kIncludeLoading       = 0x00000002,       // include assemblies that are still in the process of loading

    kIncludeExecution     = 0x00000004,       // include assemblies that are loaded for execution only
    kIncludeIntrospection = 0x00000008,       // include assemblies that are loaded for introspection only
    
    kIncludeFailedToLoad  = 0x00000010,       // include assemblies that failed to load 

};

class SharedAssemblyLocator
{
    public:
    enum
    {
        DOMAINASSEMBLY,
        IASSEMBLY,
        IHOSTASSEMBLY,
        PEASSEMBLY
    };
    DWORD GetType() {LEAF_CONTRACT; return m_type;};
    DomainAssembly* GetDomainAssembly() {LEAF_CONTRACT; _ASSERTE(m_type==DOMAINASSEMBLY); return (DomainAssembly*)m_value;};
    PEAssembly* GetPEAssembly() {LEAF_CONTRACT; _ASSERTE(m_type==PEASSEMBLY); return (PEAssembly*)m_value;};
    IAssembly* GetIAssembly() {LEAF_CONTRACT; _ASSERTE(m_type==IASSEMBLY); return (IAssembly*)m_value;};
    IHostAssembly* GetIHostAssembly() {LEAF_CONTRACT; _ASSERTE(m_type==IHOSTASSEMBLY); return (IHostAssembly*)m_value;};  
    SharedAssemblyLocator(DomainAssembly* pAssembly) {LEAF_CONTRACT; m_type=DOMAINASSEMBLY;m_value=pAssembly;};  
    SharedAssemblyLocator(PEAssembly* pAssembly) {LEAF_CONTRACT; m_type=PEASSEMBLY;m_value=pAssembly;if (pAssembly) pAssembly->AddRef();};  
    SharedAssemblyLocator(IAssembly* pAssembly) {LEAF_CONTRACT; m_type=IASSEMBLY;m_value=pAssembly;if (pAssembly) pAssembly->AddRef();};  
    SharedAssemblyLocator(IHostAssembly* pAssembly) {LEAF_CONTRACT; m_type=IHOSTASSEMBLY;m_value=pAssembly;if (pAssembly) pAssembly->AddRef();};  
    ~SharedAssemblyLocator()
    {
        LEAF_CONTRACT;
        if (m_value==NULL)
            return;
        switch(GetType())
        {
            case DOMAINASSEMBLY:break;
            case PEASSEMBLY:GetPEAssembly()->Release();break;
            case IASSEMBLY:GetIAssembly()->Release();break;
            case IHOSTASSEMBLY:GetIHostAssembly()->Release();break;
            default:_ASSERTE(!"NYI");
        }
    }
    DWORD Hash();
    protected:
    DWORD m_type;
    LPVOID m_value;
};

//
// Stores binding information about failed assembly loads for DAC
//
struct FailedAssembly {
    SString displayName;
    SString location;
    LOADCTX_TYPE context;
    HRESULT error;

    void Initialize(AssemblySpec *pSpec, Exception *ex)
    {
        CodeBaseInfo *codeBase = pSpec->GetCodeBase();
        
        displayName.SetASCII(pSpec->GetName());
        location.Set(codeBase ? codeBase->m_pszCodeBase : NULL);
        error = ex->GetHR();

        // 
        // Determine the binding context assembly would have been in.
        // If the parent has been set, use its binding context.
        // If the parent hasn't been set but the code base has, use LoadFrom.
        // Otherwise, use the default.
        //
        if (codeBase) {
            context = codeBase->GetParentAssembly() ? codeBase->GetParentAssembly()->GetFusionLoadContext() : LOADCTX_TYPE_LOADFROM;
        } else {
            context = LOADCTX_TYPE_DEFAULT;
        }
    }
};

class AppDomainIterator;

const DWORD DefaultADID = 1;

class AppDomain : public BaseDomain
{
    friend class ADUnloadSink;
    friend class SystemDomain;
    friend class AssemblySink;
    friend class ApplicationSecurityDescriptor;
    friend class AppDomainNative;
    friend class AssemblyNative;
    friend class AssemblySpec;
    friend class ClassLoader;
    friend class ThreadNative;
    friend class RCWCache;
    friend class ClrDataAccess;
    friend class CheckAsmOffsets;
    friend class AppDomainFromIDHolder;

    VPTR_VTABLE_CLASS(AppDomain, BaseDomain)

public:
#ifndef DACCESS_COMPILE
    AppDomain();
    virtual ~AppDomain();
#endif
    static void DoADUnloadWork();
    DomainAssembly* FindDomainAssembly(Assembly*);
    void EnterContext(Thread* pThread, Context* pCtx,ContextTransitionFrame *pFrame);
    //****************************************************************************************
    //
    // Initializes an AppDomain. (this functions is not called from the SystemDomain)
    void Init();

    //****************************************************************************************
    //
    // Stop deletes all the assemblies but does not remove other resources like
    // the critical sections
    void Stop();

    // Gets rid of resources
    void Terminate();


    // final assembly cleanup
    void ShutdownAssemblies();
    
    void ReleaseDomainBoundInfo();
    void ReleaseFiles();
    

    // Remove the Appdomain for the system and cleans up. This call should not be
    // called from shut down code.
    void CloseDomain();

    BOOL IsAppDomain() { LEAF_CONTRACT; return TRUE; }
    AppDomain *AsAppDomain() { LEAF_CONTRACT; return this; }

    OBJECTREF GetExposedObject();
    OBJECTREF GetRawExposedObject() {
        WRAPPER_CONTRACT;
        if (m_ExposedObject) {
            return ObjectFromHandle(m_ExposedObject);
        }
        else {
            return NULL;
        }
    }

    HRESULT GetComIPForExposedObject(IUnknown **pComIP);

    //****************************************************************************************

    ArrayList           m_Assemblies;

    class AssemblyIterator
    {
        ArrayList::Iterator m_i;
        AssemblyIterationFlags m_assemblyIterationFlags;

      public:
        BOOL Next();

        inline Assembly *GetAssembly()
        {
            CONTRACTL {
                if(m_assemblyIterationFlags & kIncludeLoading) {THROWS;} else {NOTHROW;};
                if(m_assemblyIterationFlags & kIncludeLoading) {GC_TRIGGERS;} else {GC_NOTRIGGER;};
                if(m_assemblyIterationFlags & kIncludeLoading) {INJECT_FAULT(COMPlusThrowOM());} else {FORBID_FAULT;};                
                MODE_ANY;
            } CONTRACTL_END;
            return (m_assemblyIterationFlags & kIncludeLoading)
                       ? GetDomainAssembly()->GetAssembly()
                       : GetDomainAssembly()->GetLoadedAssembly();
        }

        inline DomainAssembly *GetDomainAssembly()
        {
            WRAPPER_CONTRACT;
            return PTR_DomainAssembly((TADDR) m_i.GetElement());
        }

        inline SIZE_T GetIndex()
        {
            WRAPPER_CONTRACT;
            return m_i.GetIndex();
        }

      private:
        friend class AppDomain;
        // Cannot have constructor so this iterator can be used inside a union
        static AssemblyIterator Create(AppDomain *pDomain, AssemblyIterationFlags assemblyIterationFlags)
        {
            WRAPPER_CONTRACT;
            AssemblyIterator i;

            i.m_i = pDomain->m_Assemblies.Iterate();
            i.m_assemblyIterationFlags = assemblyIterationFlags;
            return i;
        }
    };

    AssemblyIterator IterateAssembliesEx(AssemblyIterationFlags assemblyIterationFlags)
    {
        WRAPPER_CONTRACT;
        return AssemblyIterator::Create(this, assemblyIterationFlags);
    }

    SIZE_T GetAssemblyCount()
    {
        WRAPPER_CONTRACT;
        return m_Assemblies.GetCount();
    }

    CHECK CheckCanLoadTypes(Assembly *pAssembly);
    CHECK CheckCanExecuteManagedCode(MethodDesc* pMD);
    CHECK CheckLoading(DomainFile *pFile, FileLoadLevel level);

    FileLoadLevel GetDomainFileLoadLevel(DomainFile *pFile);
    BOOL IsLoading(DomainFile *pFile, FileLoadLevel level);
    static FileLoadLevel GetThreadFileLoadLevel();

    void LoadDomainFile(DomainFile *pFile,
                        FileLoadLevel targetLevel);

    Module* FindModule(PEFile *pFile, BOOL includeLoading = FALSE) DAC_EMPTY_RET(NULL);
    DomainAssembly* FindAssembly(PEAssembly *pFile, BOOL includeLoading = FALSE) DAC_EMPTY_RET(NULL);

    // Finds only loaded modules, elevates level if needed
    Module* GetIJWModule(HMODULE hMod) DAC_EMPTY_RET(NULL);
    // Finds loading modules
    DomainFile* FindIJWDomainFile(HMODULE hMod, const SString &path) DAC_EMPTY_RET(NULL);

    Assembly *LoadAssembly(AssemblySpec* pIdentity,
                           PEAssembly *pFile,
                           FileLoadLevel targetLevel,
                           OBJECTREF* pEvidence = NULL,
                           OBJECTREF* pExtraEvidence = NULL,
                           BOOL fDelayPolicyResolution = FALSE);

    DomainAssembly *LoadDomainAssembly(AssemblySpec* pIdentity,
                                       PEAssembly *pFile,
                                       FileLoadLevel targetLevel,
                                       OBJECTREF* pEvidence = NULL,
                                       OBJECTREF* pExtraEvidence = NULL,
                                       BOOL fDelayPolicyResolution = FALSE);

    DomainModule *LoadDomainModule(DomainAssembly *pAssembly,
                                   PEModule *pFile,
                                   FileLoadLevel targetLevel);

    CHECK CheckValidModule(Module *pModule);
    DomainFile *LoadDomainNeutralModuleDependency(Module *pModule, FileLoadLevel targetLevel);


    void GetFileFromFusion(IAssembly *pIAssembly, LPCWSTR wszModuleName,
                           SString &path);

    // private:
    void LoadSystemAssemblies();

    DomainFile *LoadDomainFile(FileLoadLock *pLock,
                               FileLoadLevel targetLevel);

    void TryIncrementalLoad(DomainFile *pFile, FileLoadLevel workLevel, FileLoadLockHolder &lockHolder);

    Assembly *LoadAssemblyHelper(LPCWSTR wszAssembly,
                                 LPCWSTR wszCodeBase);

#ifndef DACCESS_COMPILE // needs AssemblySpec
    //****************************************************************************************
    // Returns and Inserts assemblies into a lookup cache based on the binding information
    // in the AssemblySpec. There can be many AssemblySpecs to a single assembly.
    DomainAssembly* FindCachedAssembly(AssemblySpec* pSpec, BOOL fThrow=TRUE)
    {
        WRAPPER_CONTRACT;
        return m_AssemblyCache.LookupAssembly(pSpec, fThrow);
    }

    PEAssembly* FindCachedFile(AssemblySpec* pSpec);
    BOOL IsCached(AssemblySpec *pSpec);
#endif // DACCESS_COMPILE
    void CacheStringsForDAC();

    BOOL AddFileToCache(AssemblySpec* pSpec, PEAssembly *pFile);
    BOOL AddAssemblyToCache(AssemblySpec* pSpec, DomainAssembly *pAssembly);
    BOOL AddExceptionToCache(AssemblySpec* pSpec, Exception *ex);

    void AddUnmanagedImageToCache(LPCWSTR libraryName, HMODULE hMod);
    HMODULE FindUnmanagedImageInCache(LPCWSTR libraryName);

    //****************************************************************************************
    //
    // Adds an assembly to the domain.
    void AddAssembly(DomainAssembly* assem);

    BOOL ContainsAssembly(Assembly *assem);

    enum SharePolicy
    {
        // Attributes to control when to use domain neutral assemblies
        SHARE_POLICY_UNSPECIFIED,   // Use the current default policy
        SHARE_POLICY_NEVER,         // Do not share anything, except the system assembly
        SHARE_POLICY_ALWAYS,        // Share everything possible
        SHARE_POLICY_GAC,           // Share only GAC-bound assemblies

        SHARE_POLICY_COUNT,
        SHARE_POLICY_MASK = 0x3,

        // NOTE that previously defined was a bit 0x40 which might be set on this value
        // in custom attributes.
        SHARE_POLICY_DEFAULT = SHARE_POLICY_NEVER,
    };

    void SetSharePolicy(SharePolicy policy);
    SharePolicy GetSharePolicy();
    BOOL ReduceSharePolicyFromAlways();

    //****************************************************************************************
    // Determines if the image is to be loaded into the shared assembly or an individual
    // appdomains.
    BOOL ApplySharePolicy(DomainAssembly *pFile);
    BOOL ApplySharePolicyFlag(DomainAssembly *pFile);

    BOOL HasSetSecurityPolicy();

    ApplicationSecurityDescriptor* GetSecurityDescriptor()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return static_cast<ApplicationSecurityDescriptor*>(m_pSecDesc);
    }

    void CreateSecurityDescriptor();

    //****************************************************************************************
    //
    // Reference count. When an appdomain is first created the reference is bump
    // to one when it is added to the list of domains (see SystemDomain). An explicit
    // Removal from the list is necessary before it will be deleted.
    ULONG AddRef(void);
    ULONG Release(void) DAC_EMPTY_RET(0);

    //****************************************************************************************
    LPCWSTR GetFriendlyName(BOOL fDebuggerCares = TRUE);
    LPCWSTR GetFriendlyNameForDebugger();
    LPCWSTR GetFriendlyNameForLogging();
#ifdef DACCESS_COMPILE
    PVOID GetFriendlyNameNoSet(bool* isUtf8);
#endif
    void SetFriendlyName(LPCWSTR pwzFriendlyName, BOOL fDebuggerCares = TRUE);
    void ResetFriendlyName(BOOL fDebuggerCares = TRUE);

    //****************************************************************************************

    // This can be used to override the binding behavior of the appdomain.   It
    // is overridden in the compilation domain.  It is important that all
    // static binding goes through this path.
    virtual PEAssembly *BindAssemblySpec(AssemblySpec *pSpec, BOOL fThrowOnFileNotFound , BOOL fRaisePrebindEvents, StackCrawlMark *pCallerStackMark = NULL) DAC_EMPTY_RET(NULL);
    virtual DomainAssembly *BindAssemblySpecForIntrospectionDependencies(AssemblySpec *pSpec) DAC_EMPTY_RET(NULL);


    PEAssembly *TryResolveAssembly(AssemblySpec *pSpec, BOOL fPreBind);


    // This is overridden by a compilation domain to record the linktime checks made
    // when compiling an assembly
    virtual void OnLinktimeCheck(Assembly *pAssembly,
                                 OBJECTREF refCasDemands,
                                 OBJECTREF refNonCasDemands) {LEAF_CONTRACT;      STATIC_CONTRACT_SO_TOLERANT;;return; }
    virtual void OnLinktimeCanCallUnmanagedCheck(Assembly *pAssembly) { LEAF_CONTRACT; return; }
    virtual void OnLinktimeCanSkipVerificationCheck(Assembly * pAssembly) { LEAF_CONTRACT; return; }
    virtual void OnLinktimeFullTrustCheck(Assembly *pAssembly) { LEAF_CONTRACT; return; }


    // Store a successful binding into the cache.  This will keep the file from
    // being physically unmapped, as well as shortcutting future attempts to bind
    // the same spec throught the Cached entry point.
    //
    // Right now we only cache assembly binds for "probing" type
    // binding situations, basically when loading domain neutral assemblies or
    // zap files.
    //
    //
    // Returns TRUE if stored
    //         FALSE if it's a duplicate (caller should clean up args)
    BOOL StoreBindAssemblySpecResult(AssemblySpec *pSpec,
                                     PEAssembly *pFile,
                                     BOOL clone = TRUE);

    BOOL StoreBindAssemblySpecError(AssemblySpec *pSpec,
                                    HRESULT hr,
                                    OBJECTREF *pThrowable,
                                    BOOL clone = TRUE);

    //****************************************************************************************
    //
    static BOOL SetContextProperty(IApplicationContext* pFusionContext,
                                   LPCWSTR pProperty,
                                   OBJECTREF* obj);

    //****************************************************************************************
    //
    // Uses the first assembly to add an application base to the Context. This is done
    // in a lazy fashion so executables do not take the perf hit unless the load other
    // assemblies
    LPWSTR GetDynamicDir();

#ifndef DACCESS_COMPILE
    void OnAssemblyLoad(Assembly *assem);
    void OnAssemblyLoadUnlocked(Assembly *assem);
    static BOOL OnUnhandledException(OBJECTREF *pThrowable, BOOL isTerminating = TRUE);
    
#endif

#ifdef DEBUGGING_SUPPORTED
    void SetDebuggerAttached (DWORD dwStatus);
    DWORD GetDebuggerAttached (void);
    BOOL IsDebuggerAttached (void);
    BOOL NotifyDebuggerLoad(int flags, BOOL attaching);
    void NotifyDebuggerUnload();
#endif // DEBUGGING_SUPPORTED

    void SetSystemAssemblyLoadEventSent (BOOL fFlag);
    BOOL WasSystemAssemblyLoadEventSent (void);
    BOOL IsDomainBeingCreated (void);
    void SetDomainBeingCreated (BOOL flag);

#ifndef DACCESS_COMPILE
    OBJECTREF* AllocateStaticFieldObjRefPtrs(int nRequested, OBJECTREF** ppLazyAllocate = NULL)
    {
        WRAPPER_CONTRACT;

        return AllocateObjRefPtrsInLargeTable(nRequested, ppLazyAllocate);
    }
#endif // DACCESS_COMPILE

    void              EnumStaticGCRefs(GCEnumCallback  pCallback, LPVOID hCallBack);

    DomainLocalBlock *GetDomainLocalBlock()
    {
        LEAF_CONTRACT;

        return &m_sDomainLocalBlock;
    }

    static SIZE_T GetOffsetOfModuleSlotsPointer()
    {
        WRAPPER_CONTRACT;

        return offsetof(AppDomain,m_sDomainLocalBlock) + DomainLocalBlock::GetOffsetOfModuleSlotsPointer();
    }

    void SetupSharedStatics();

    ADUnloadSink* PrepareForWaitUnloadCompletion();

    //****************************************************************************************
    //
    // Create a quick lookup for classes loaded into this domain based on their GUID.
    //
    void InsertClassForCLSID(MethodTable* pMT, BOOL fForceInsert = FALSE);
    void InsertClassForCLSID(MethodTable* pMT, GUID *pGuid);



#ifndef DACCESS_COMPILE
    MethodTable* LookupClass(REFIID iid)
    {
        WRAPPER_CONTRACT;

        MethodTable *pMT = (MethodTable*) m_clsidHash.LookupValue((UPTR) GetKeyFromGUID(&iid), (LPVOID)&iid);
        return (pMT == (MethodTable*) INVALIDENTRY
            ? NULL
            : pMT);
    }
#endif // DACCESS_COMPILE

    ULONG GetKeyFromGUID(const GUID *pguid)
    {
        LEAF_CONTRACT;

        ULONG key = *(ULONG *) pguid;

        if (key <= DELETED)
            key = DELETED+1;

        return key;
    }


    //****************************************************************************************
    // Get the proxy for this app domain
    OBJECTREF GetAppDomainProxy();

    ADIndex GetIndex()
    {
        LEAF_CONTRACT;

        return m_dwIndex;
    }

    IApplicationContext *CreateFusionContext();

    void InitializeDomainContext(BOOL allowRedirects, LPCWSTR pwszPath, LPCWSTR pwszConfig);
    void SetupLoaderOptimization(DWORD optimization);


    //****************************************************************************************
    // Create a domain context rooted at the fileName. The directory containing the file name
    // is the application base and the configuration file is the fileName appended with
    // .config. If no name is passed in then no domain is created.
    static AppDomain* CreateDomainContext(WCHAR* fileName);

    // Sets up the current domain's fusion context based on the given exe file name
    // (app base & config file)
    void SetupExecutableFusionContext(WCHAR *exePath);

    //****************************************************************************************
    // Manage a pool of asyncrhonous objects used to fetch assemblies.  When a sink is released
    // it places itself back on the pool list.  Only one object is kept in the pool.
    AssemblySink* AllocateAssemblySink(AssemblySpec* pSpec);

    void SetIsUserCreatedDomain()
    {
        LEAF_CONTRACT;

        m_dwFlags |= USER_CREATED_DOMAIN;
    }

    BOOL IsUserCreatedDomain()
    {
        LEAF_CONTRACT;

        return (m_dwFlags & USER_CREATED_DOMAIN);
    }

    void SetPassiveDomain()
    {
        LEAF_CONTRACT;

        m_dwFlags |= PASSIVE_DOMAIN;
    }

    BOOL IsPassiveDomain()
    {
        LEAF_CONTRACT;

        return (m_dwFlags & PASSIVE_DOMAIN);
    }

    void SetVerificationDomain()
    {
        LEAF_CONTRACT;

        m_dwFlags |= VERIFICATION_DOMAIN;
    }

    BOOL IsVerificationDomain()
    {
        LEAF_CONTRACT;

        return (m_dwFlags & VERIFICATION_DOMAIN);
    }

    void SetIllegalVerificationDomain()
    {
        LEAF_CONTRACT;

        m_dwFlags |= ILLEGAL_VERIFICATION_DOMAIN;
    }

    BOOL IsIllegalVerificationDomain()
    {
        LEAF_CONTRACT;

        return (m_dwFlags & ILLEGAL_VERIFICATION_DOMAIN);
    }

    void SetCompilationDomain()
    {
        LEAF_CONTRACT;

        m_dwFlags |= (PASSIVE_DOMAIN|COMPILATION_DOMAIN);
    }

    BOOL IsCompilationDomain();

    CompilationDomain * ToCompilationDomain()
    {
        LEAF_CONTRACT;

        _ASSERTE(IsCompilationDomain());
        return PTR_CompilationDomain(PTR_HOST_TO_TADDR(this));
    }

    void SetCanUnload()
    {
        LEAF_CONTRACT;

        m_dwFlags |= APP_DOMAIN_CAN_BE_UNLOADED;
    }

    BOOL CanUnload()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_dwFlags & APP_DOMAIN_CAN_BE_UNLOADED;
    }

    void SetRemotingConfigured()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        FastInterlockOr((ULONG*)&m_dwFlags, REMOTING_CONFIGURED_FOR_DOMAIN);
    }

    BOOL IsRemotingConfigured()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_dwFlags & REMOTING_CONFIGURED_FOR_DOMAIN;
    }

    void SetOrphanedLocks()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        FastInterlockOr((ULONG*)&m_dwFlags, ORPHANED_LOCKS);
    }

    BOOL HasOrphanedLocks()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_dwFlags & ORPHANED_LOCKS;
    }

    // This function is used to relax asserts in the lock accounting.
    // It returns true if we are fine with hosed lock accounting in this domain.
    BOOL OkToIgnoreOrphanedLocks()
    {
        WRAPPER_CONTRACT;
        return HasOrphanedLocks() && m_Stage >= STAGE_UNLOAD_REQUESTED;
    }

    static void ExceptionUnwind(Frame *pFrame);

#ifdef _DEBUG
    void TrackADThreadEnter(Thread *pThread, Frame *pFrame);
    void TrackADThreadExit(Thread *pThread, Frame *pFrame);
    void DumpADThreadTrack();
#endif

#ifndef DACCESS_COMPILE
    void ThreadEnter(Thread *pThread, Frame *pFrame)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_NOTRIGGER;

#ifdef _DEBUG
        if (LoggingOn(LF_APPDOMAIN, LL_INFO100))
            TrackADThreadEnter(pThread, pFrame);
        else
#endif
        {
            InterlockedIncrement((LONG*)&m_dwThreadEnterCount);
            LOG((LF_APPDOMAIN, LL_INFO1000, "AppDomain::ThreadEnter  %x to [%d] (%8.8x) %S count %d\n", 
                    pThread->GetThreadId(),GetId().m_dwId, this,
                    GetFriendlyNameForLogging(),GetThreadEnterCount()));
#if _DEBUG_AD_UNLOAD
            printf("AppDomain::ThreadEnter %x to [%d] (%8.8x) %S count %d\n",
                    pThread->GetThreadId(), GetId().m_dwId, this,
                    GetFriendlyNameForLogging(), GetThreadEnterCount());
#endif
        }
    }

    void ThreadExit(Thread *pThread, Frame *pFrame)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_NOTRIGGER;

#ifdef _DEBUG
        if (LoggingOn(LF_APPDOMAIN, LL_INFO100)) {
            TrackADThreadExit(pThread, pFrame);
        }
        else
#endif
        {
            LONG result;
            result = InterlockedDecrement((LONG*)&m_dwThreadEnterCount);
            _ASSERTE(result >= 0);
            LOG((LF_APPDOMAIN, LL_INFO1000, "AppDomain::ThreadExit from [%d] (%8.8x) %S count %d\n",
                    this, GetId().m_dwId,
                    GetFriendlyNameForLogging(), GetThreadEnterCount()));
#if _DEBUG_ADUNLOAD
            printf("AppDomain::ThreadExit %x from [%d] (%8.8x) %S count %d\n",
                    pThread->GetThreadId(), this, GetId().m_dwId,
                    GetFriendlyNameForLogging(), GetThreadEnterCount());
#endif
        }
    }
#endif // DACCESS_COMPILE

    ULONG GetThreadEnterCount()
    {
        LEAF_CONTRACT;

        return m_dwThreadEnterCount;
    }

    BOOL OnlyOneThreadLeft()
    {
        LEAF_CONTRACT;
        return m_dwThreadEnterCount==1 || m_dwThreadsStillInAppDomain ==1;
    }

    Context *GetDefaultContext()
    {
        LEAF_CONTRACT;

        return m_pDefaultContext;
    }

    BOOL CanLoadCode()
    {
        LEAF_CONTRACT;
        return m_Stage >= STAGE_READYFORMANAGEDCODE && m_Stage < STAGE_CLOSED;        
    }

    BOOL HasUnloadStarted()
    {
        LEAF_CONTRACT;
        return m_Stage>=STAGE_EXITED;
    }
    static void RefTakerAcquire(AppDomain* pDomain)
    {
        WRAPPER_CONTRACT;
        if(!pDomain)
            return;
        pDomain->AddRef();
#ifdef _DEBUG
        FastInterlockIncrement(&pDomain->m_dwRefTakers);
#endif
    }

    static void RefTakerRelease(AppDomain* pDomain)
    {
        WRAPPER_CONTRACT;
        if(!pDomain)
            return;
#ifdef _DEBUG
        _ASSERTE(pDomain->m_dwRefTakers);
        FastInterlockDecrement(&pDomain->m_dwRefTakers);
#endif
        pDomain->Release();
    }


#ifdef _DEBUG 

    BOOL IsHeldByIterator()
    {
        LEAF_CONTRACT;
        return m_dwIterHolders>0;
    }

    BOOL IsHeldByRefTaker()
    {
        LEAF_CONTRACT;
        return m_dwRefTakers>0;
    }

    void IteratorRelease()
    {
        LEAF_CONTRACT;
        _ASSERTE(m_dwIterHolders);
        FastInterlockDecrement(&m_dwIterHolders);
    }

      
    void IteratorAcquire()
    {
        LEAF_CONTRACT;
        FastInterlockIncrement(&m_dwIterHolders);
    }


      
    
#endif    
    BOOL IsActive()
    {
        LEAF_CONTRACT;

        return m_Stage >= STAGE_ACTIVE && m_Stage < STAGE_CLOSED;
    }
    BOOL IsValid()
    {
        LEAF_CONTRACT;

#ifdef DACCESS_COMPILE
        // We want to see all appdomains in SOS, even the about to be destructed ones.
        // There is no risk of races under DAC, so we will pretend to be unconditionally valid.
        return TRUE;
#else
        return m_Stage > STAGE_CREATING && m_Stage < STAGE_CLOSED;
#endif
    }

#ifdef _DEBUG
    BOOL IsBeingCreated()
    {
        LEAF_CONTRACT;

        return m_Stage < STAGE_OPEN;
    }
#endif
    BOOL IsRunningIn(Thread* pThread);

    
    BOOL IsUnloading()
    {
        LEAF_CONTRACT;

        return m_Stage > STAGE_UNLOAD_REQUESTED;
    }

    BOOL NotReadyForManagedCode()
    {
        LEAF_CONTRACT;

        return m_Stage < STAGE_READYFORMANAGEDCODE;
    }



    void SetFinalized()
    {
        LEAF_CONTRACT;
        SetStage(STAGE_FINALIZED);
    }

    BOOL IsFinalizing()
    {
        LEAF_CONTRACT;

        return m_Stage >= STAGE_FINALIZING;
    }

    BOOL IsFinalized()
    {
        LEAF_CONTRACT;

        return m_Stage >= STAGE_FINALIZED;
    }

    BOOL NoAccessToHandleTable()
    {
        LEAF_CONTRACT;

        return m_Stage >= STAGE_HANDLETABLE_NOACCESS;
    }

    // Checks whether the given thread can enter the app domain
    BOOL CanThreadEnter(Thread *pThread);

    // Following two are needed for the Holder
    static void SetUnloadInProgress(AppDomain *pThis);
    static void SetUnloadComplete(AppDomain *pThis);
    // Predicates for GC asserts
    BOOL ShouldHaveFinalization()
    {
        LEAF_CONTRACT;

        return ((DWORD) m_Stage) < STAGE_COLLECTED;
    }
    BOOL ShouldHaveCode()
    {
        LEAF_CONTRACT;

        return ((DWORD) m_Stage) < STAGE_COLLECTED;
    }
    BOOL ShouldHaveRoots()
    {
        LEAF_CONTRACT;

        return ((DWORD) m_Stage) < STAGE_CLEARED;
    }
    BOOL ShouldHaveInstances()
    {
        LEAF_CONTRACT;

        return ((DWORD) m_Stage) < STAGE_COLLECTED;
    }


    static void RaiseExitProcessEvent();
    Assembly* RaiseResourceResolveEvent(LPCSTR szName);
    DomainAssembly* RaiseTypeResolveEventThrowing(LPCSTR szName);
    Assembly* RaiseAssemblyResolveEvent(AssemblySpec *pSpec, BOOL fIntrospection, BOOL fPreBind);
private:
    CrstExplicitInit    m_ReflectionCrst;
    CrstExplicitInit    m_RefClassFactCrst;

    EEClassFactoryInfoHashTable *m_pRefClassFactHash;   // Hash table that maps a class factory info to a COM comp.

public:

    CrstBase *GetRefClassFactCrst()
    {
        LEAF_CONTRACT;

        return &m_RefClassFactCrst;
    }

#ifndef DACCESS_COMPILE
    EEClassFactoryInfoHashTable* GetClassFactHash()
    {
        STATIC_CONTRACT_THROWS;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FAULT;

        if (m_pRefClassFactHash != NULL) {
            return m_pRefClassFactHash;
        }

        return SetupClassFactHash();
    }
#endif // DACCESS_COMPILE


    // Memory reporting support:

    // This overloaded function helps produce overloaded macros
    static ADID IdFromAppDomain(ADID id) { return id; }
    static ADID IdFromAppDomain(AppDomain *pDomain) { return pDomain->GetId(); }

#define MEMORY_REPORT_SET_APP_DOMAIN(d) \
    MEMORY_REPORT_SET_ATTRIBUTION(1, "AppDomain %d", AppDomain::IdFromAppDomain(d).m_dwId)

#define MEMORY_REPORT_APP_DOMAIN_SCOPE(d) \
    MEMORY_REPORT_ATTRIBUTION_SCOPE(1, "AppDomain %d", AppDomain::IdFromAppDomain(d).m_dwId)

private:
    static void RaiseOneExitProcessEvent_Wrapper(AppDomainIterator* pi);
    static void RaiseOneExitProcessEvent();
    size_t EstimateSize();
    EEClassFactoryInfoHashTable* SetupClassFactHash();

    void InitializeDefaultDomainManager ();

protected:

    LPWSTR m_pwDynamicDir;

private:
    void RaiseLoadingAssemblyEvent(DomainAssembly* pAssembly);

    friend class DomainAssembly;

public:
    static void ProcessUnloadDomainEventOnFinalizeThread();
    static BOOL HasWorkForFinalizerThread()
    {
        LEAF_CONTRACT;
        return s_pAppDomainToRaiseUnloadEvent != NULL;
    }

private:
    static AppDomain* s_pAppDomainToRaiseUnloadEvent;
    static BOOL s_fProcessUnloadDomainEvent;

    void RaiseUnloadDomainEvent();
    static void RaiseUnloadDomainEvent_Wrapper(LPVOID /* AppDomain * */);

    BOOL RaiseUnhandledExceptionEvent(OBJECTREF *pSender, OBJECTREF *pThrowable, BOOL isTerminating);
    BOOL HasUnhandledExceptionEventHandler();
    BOOL RaiseUnhandledExceptionEventNoThrow(OBJECTREF *pSender, OBJECTREF *pThrowable, BOOL isTerminating);
    
    struct RaiseUnhandled_Args
    {
        AppDomain *pExceptionDomain;
        AppDomain *pTargetDomain;
        OBJECTREF *pSender;
        OBJECTREF *pThrowable;
        BOOL isTerminating;
        BOOL *pResult;
    };
    static void RaiseUnhandledExceptionEvent_Wrapper(LPVOID /* RaiseUnhandled_Args * */);


    static void AllowThreadEntrance(AppDomain *pApp);
    static void RestrictThreadEntrance(AppDomain *pApp);

    typedef Holder<AppDomain*,DoNothing<AppDomain*>,AppDomain::AllowThreadEntrance,NULL> RestrictEnterHolder;
    
    enum Stage {
        STAGE_CREATING,
        STAGE_READYFORMANAGEDCODE,
        STAGE_ACTIVE,
        STAGE_OPEN,
        STAGE_UNLOAD_REQUESTED,
        STAGE_EXITING,
        STAGE_EXITED,
        STAGE_FINALIZING,
        STAGE_FINALIZED,
        STAGE_HANDLETABLE_NOACCESS,
        STAGE_CLEARED,
        STAGE_COLLECTED,
        STAGE_CLOSED
    };
    void SetStage(Stage stage)
    {
        STRESS_LOG2(LF_APPDOMAIN, LL_INFO100,"Updating AD stage, ADID=%d, stage=%d",GetId().m_dwId,stage);
        TESTHOOKCALL(AppDomainStageChanged(GetId().m_dwId,m_Stage,stage));
        Stage lastStage=m_Stage;
        while (lastStage !=stage) 
            lastStage = (Stage)FastInterlockCompareExchange((LONG*)&m_Stage,stage,lastStage);
    };
    void Exit(BOOL fRunFinalizers, BOOL fAsyncExit);
    void Close();
    void ClearGCRoots();
    void ClearGCHandles();
    void HandleAsyncPinHandles();
    void UnwindThreads();
    // Return TRUE if EE is stopped
    // Return FALSE if more work is needed
    BOOL StopEEAndUnwindThreads(unsigned int retryCount, BOOL *pFMarkUnloadRequestThread);
    BOOL ReleaseDomainStores(LocalDataStore **pStores, UINT *numStores);

    // Use Rude Abort to unload the domain.
    BOOL m_fRudeUnload;

    Thread *m_pUnloadRequestThread;
    ADUnloadSink*   m_ADUnloadSink;
    BOOL  m_bForceGCOnUnload;
    BOOL  m_bUnloadingFromUnloadEvent;
public:
    AppDomain* m_pNextInDelayedUnloadList;
    void SetForceGCOnUnload(BOOL bSet)
    {
        m_bForceGCOnUnload=bSet;
    }

    void SetUnloadingFromUnloadEvent()
    {
        m_bUnloadingFromUnloadEvent=TRUE;
    }

    BOOL IsUnloadingFromUnloadEvent()
    {
        return m_bUnloadingFromUnloadEvent;
    }
    
    void SetRudeUnload()
    {
        LEAF_CONTRACT;

        m_fRudeUnload = TRUE;
    }

    BOOL IsRudeUnload()
    {
        LEAF_CONTRACT;

        return m_fRudeUnload;
    }

    ADUnloadSink* GetADUnloadSink();
    ADUnloadSink* GetADUnloadSinkForUnload();
    void SetUnloadRequestThread(Thread *pThread)
    {
        LEAF_CONTRACT;

        m_pUnloadRequestThread = pThread;
    }

    Thread *GetUnloadRequestThread()
    {
        LEAF_CONTRACT;

        return m_pUnloadRequestThread;
    }

public:
    // ID to uniquely identify this AppDomain - used by the AppDomain publishing
    // service (to publish the list of all appdomains present in the process),
    // which in turn is used by, for eg., the debugger (to decide which App-
    // Domain(s) to attach to).
    // This is also used by Remoting for routing cross-appDomain calls.
    ADID GetId (void)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_dwId;
    }
    void SetGCRefPoint ( int gccounter)
    {
        LEAF_CONTRACT;
        m_nGCCount=gccounter;
    }
    int  GetGCRefPoint (void)
    {
        LEAF_CONTRACT;
        return m_nGCCount;
    }

    static USHORT GetOffsetOfId()
    {
        LEAF_CONTRACT;
        size_t ofs = offsetof(class AppDomain, m_dwId);
        _ASSERTE(FitsInI2(ofs));
        return (USHORT)ofs;
    }

    
    void AddMemoryPressure();
    void RemoveMemoryPressure();
    void Unload(BOOL fForceUnload);
    static HRESULT UnloadById(ADID Id, BOOL fSync, BOOL fExceptionsPassThrough=FALSE);
    static HRESULT UnloadWait(ADID Id, ADUnloadSink* pSink);
    static void ResetUnloadRequestThread(ADID Id);

    void UnlinkClass(MethodTable *pMT);

    typedef Holder<AppDomain *, AppDomain::SetUnloadInProgress, AppDomain::SetUnloadComplete> UnloadHolder;
    Assembly *GetRootAssembly()
    {
        LEAF_CONTRACT;
        return m_pRootAssembly;
    }

private:
    SString         m_friendlyName;
    Assembly        *m_pRootAssembly;

    // General purpose flags.
    DWORD           m_dwFlags;

    // When an application domain is created the ref count is artifically incremented
    // by one. For it to hit zero an explicit close must have happened.
    LONG        m_cRef;                    // Ref count.

    PTR_ApplicationSecurityDescriptor m_pSecDesc;  // Application Security Descriptor

    OBJECTHANDLE    m_ExposedObject;

    // Indicates where assemblies will be loaded for
    // this domain. By default all assemblies are loaded into the domain.
    // There are two additional settings, all
    // assemblies can be loaded into the shared domain or assemblies
    // that are strong named are loaded into the shared area.
    SharePolicy m_SharePolicy;

    IUnknown        *m_pComIPForExposedObject;

    // Hash table that maps a clsid to a type
    PtrHashMap          m_clsidHash;


    AssemblySink*      m_pAsyncPool;  // asynchronous retrival object pool (only one is kept)

    // The index of this app domain among existing app domains (starting from 1)
    ADIndex m_dwIndex;

    // The creation sequence number of this app domain (starting from 1)
    ADID m_dwId;

    int m_nGCCount;

#ifdef _DEBUG
    struct ThreadTrackInfo;
    typedef CDynArray<ThreadTrackInfo *> ThreadTrackInfoList;
    ThreadTrackInfoList *m_pThreadTrackInfoList;
    DWORD m_TrackSpinLock;
#endif

    DomainLocalBlock    m_sDomainLocalBlock;

    // The number of  times we have entered this AD
    ULONG m_dwThreadEnterCount;
    // The number of threads that have entered this AD, for ADU only
    ULONG m_dwThreadsStillInAppDomain;

    volatile Stage m_Stage;

    // The default context for this domain
    Context *m_pDefaultContext;

    SString         m_applicationBase;
    SString         m_privateBinPaths;
    SString         m_configFile;

    ArrayList        m_failedAssemblies;

#ifdef _DEBUG
    volatile LONG m_dwIterHolders;
    volatile LONG m_dwRefTakers;
#endif

    //
    // DAC iterator for failed assembly loads
    //
    class FailedAssemblyIterator
    {
        ArrayList::Iterator m_i;
        
      public:
        BOOL Next()
        {
            WRAPPER_CONTRACT;
            return m_i.Next();
        }
        FailedAssembly *GetFailedAssembly()
        {
            WRAPPER_CONTRACT;
            return PTR_FailedAssembly((TADDR) m_i.GetElement());
        }
        SIZE_T GetIndex()
        {
            WRAPPER_CONTRACT;
            return m_i.GetIndex();
        }

      private:
        friend class AppDomain;
        // Cannot have constructor so this iterator can be used inside a union
        static FailedAssemblyIterator Create(AppDomain *pDomain)
        {
            WRAPPER_CONTRACT;
            FailedAssemblyIterator i;

            i.m_i = pDomain->m_failedAssemblies.Iterate();
            return i;
        }
    };
    friend class FailedAssemblyIterator;

    FailedAssemblyIterator IterateFailedAssembliesEx()
    {
        WRAPPER_CONTRACT;
        return FailedAssemblyIterator::Create(this);
    }

    //---------------------------------------------------------
    // Stub caches for Method stubs
    //---------------------------------------------------------


    void TurnOnBindingRedirects();

public:


    enum {
        CONTEXT_INITIALIZED =               0x0001,
        USER_CREATED_DOMAIN =               0x0002, // created by call to AppDomain.CreateDomain
        ALLOCATEDCOM =                      0x0008,
        DEBUGGER_NOT_ATTACHED =             0x0010,
        DEBUGGER_ATTACHING =                0x0020, // assemblies & modules
        DEBUGGER_ATTACHED =                 0x0030,
        DEBUGGER_STATUS_BITS_MASK =         0x0230,
        LOAD_SYSTEM_ASSEMBLY_EVENT_SENT =   0x0040,
        APP_DOMAIN_BEING_CREATED =          0x0080,
        REMOTING_CONFIGURED_FOR_DOMAIN =    0x0100,
        DEBUGGER_ATTACHING_THREAD =         0x0200,
        COMPILATION_DOMAIN =                0x0400, // Are we ngenning?
        APP_DOMAIN_CAN_BE_UNLOADED =        0x0800, // if need extra bits, can derive this at runtime
        ORPHANED_LOCKS =                    0x1000, // Orphaned locks exist in this appdomain.
        PASSIVE_DOMAIN =                    0x2000, // Can we execute code in this AppDomain
        VERIFICATION_DOMAIN =               0x4000,  // This is a verification domain
        ILLEGAL_VERIFICATION_DOMAIN =       0x8000  // This can't be a verification domain
    };

    SecurityContext *m_pSecContext;

    AssemblySpecBindingCache  m_AssemblyCache;
    DomainAssemblyCache       m_UnmanagedCache;
    size_t                                m_MemoryPressure;

    // Section to support AD unload due to escalation
public:
    MetaSigCache *m_pMetaSigCache;   
    LONG m_metaSigCacheState;
    static void CreateADUnloadWorker();

    static void CreateADUnloadStartEvent();

    static DWORD WINAPI ADUnloadThreadStart(void *args);

    // If called to handle stack overflow, we can not set event, since the thread has limit stack.
    void EnableADUnloadWorker(EEPolicy::AppDomainUnloadTypes type, BOOL fHasStack = TRUE);

    static void EnableADUnloadWorkerForThreadAbort();
    static void EnableADUnloadWorkerForFinalizer();
    static void EnableADUnloadWorkerForCollectedADCleanup();

    BOOL IsUnloadRequested()
    {
        LEAF_CONTRACT;

        return (m_Stage == STAGE_UNLOAD_REQUESTED);
    }

    // Remoting unload method.
    static MethodDesc *s_remoteServiceUnloadMD;

private:
    static void ADUnloadWorkerHelper(AppDomain *pDomain);
    static CLREvent * g_pUnloadStartEvent;

#ifdef DACCESS_COMPILE
public:
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                                   bool enumThis);
#endif

};

// This holder is to be used to take a reference to make sure AppDomain* is still valid
// Please do not use if you are aleady ADU-safe
typedef Wrapper<AppDomain*,AppDomain::RefTakerAcquire,AppDomain::RefTakerRelease,NULL> AppDomainRefTaker;

// Just a ref holder
typedef Wrapper<AppDomain*,DoNothing<AppDomain*>,DoTheRelease<AppDomain>,NULL> AppDomainRefHolder;


// This class provides a way to access AppDomain by ID
// without risking the appdomain getting invalid in the process
class AppDomainFromIDHolder
{
public:
    enum SyncType  
    {
        SyncType_GC,     // Prevents AD from being unloaded by forbidding GC for the lifetime of the object
        SyncType_ADLock  // Prevents AD from being unloaded by requiring ownership of DomainLock for the lifetime of the object
    };
protected:    
    AppDomain* m_pDomain;
#ifdef _DEBUG    
    BOOL       m_bAcquired;
    BOOL       m_bChecked;
    SyncType   m_type;
#endif
public:
    AppDomainFromIDHolder(ADID adId, BOOL bUnsafePoint, SyncType synctype=SyncType_GC);
    AppDomainFromIDHolder(SyncType synctype=SyncType_GC);    
    ~AppDomainFromIDHolder();    
    void Assign(ADID adId, BOOL bUnsafePoint);
    void ThrowIfUnloaded();
    void Release();
    BOOL IsUnloaded() 
    {
        LEAF_CONTRACT;
#ifdef _DEBUG
        m_bChecked=TRUE; 
        if (m_pDomain==NULL)
        {
            // no need to enforce anything
            Release(); 
        }
#endif
        return m_pDomain==NULL;
    };
    AppDomain* operator->();
};



typedef VPTR(class SystemDomain) PTR_SystemDomain;

class SystemDomain : public BaseDomain
{
    friend class AppDomainNative;
    friend class AppDomainIterator;
    friend class ClrDataAccess;
    friend class AppDomainFromIDHolder;
    friend Frame *Thread::IsRunningIn(AppDomain* pDomain, int *count);

    VPTR_VTABLE_CLASS(SystemDomain, BaseDomain)
    VPTR_UNIQUE(VPTR_UNIQUE_SystemDomain)
    static AppDomain *GetAppDomainAtId(ADID indx);

public:  
    
    static AppDomain* GetAppDomainFromId(ADID indx,DWORD ADValidityKind)
    {
        WRAPPER_CONTRACT;
        AppDomain* pRetVal;
        if (indx.m_dwId==DefaultADID)
            pRetVal= SystemDomain::System()->DefaultDomain();
        else
            pRetVal= GetAppDomainAtId(indx);
#ifdef _DEBUG
        // Only call CheckADValidity in DEBUG builds for non-NULL return values
        if (pRetVal != NULL)
            CheckADValidity(pRetVal, ADValidityKind);
#endif        
        return pRetVal;
    }
    //****************************************************************************************
    //
    // To be run during the initial start up of the EE. This must be
    // performed prior to any class operations.
    static void Attach();

    //****************************************************************************************
    //
    // To be run during shutdown. This must be done after all operations
    // that require the use of system classes (i.e., exceptions).
    // DetachBegin stops all domains, while DetachEnd deallocates domain resources.
    static void DetachBegin();

    //****************************************************************************************
    //
    // To be run during shutdown. This must be done after all operations
    // that require the use of system classes (i.e., exceptions).
    // DetachBegin stops release resources held by systemdomain and the default domain.
    static void DetachEnd();

    //****************************************************************************************
    //
    // Initializes and shutdowns the single instance of the SystemDomain
    // in the EE
#ifndef DACCESS_COMPILE
    void *operator new(size_t size, void *pInPlace);
    void operator delete(void *pMem);
#endif
    void Init();
    void Stop();
    void Terminate();
    static void LazyInitGlobalStringLiteralMap();

    //****************************************************************************************
    //
    // Load the base system classes, these classes are required before
    // any other classes are loaded
    void LoadBaseSystemClasses();

    AppDomain* DefaultDomain()
    {
        LEAF_CONTRACT;

        return m_pDefaultDomain;
    }

    // Notification when an assembly is loaded into the system domain
    void OnAssemblyLoad(Assembly *assem);

    //****************************************************************************************
    //
    // Global Static to get the one and only system domain
    static SystemDomain* System()
    {
        LEAF_CONTRACT;

        return m_pSystemDomain;
    }

    //****************************************************************************************
    //
    // Global static to get the loader of the one and only system assembly and loader
    static ClassLoader* Loader()
    {
        WRAPPER_CONTRACT;

        // Since system only has one assembly we
        // can provide quick access
        _ASSERTE(m_pSystemDomain);
        return System()->m_pSystemAssembly->GetLoader();
    }

    static PEAssembly* SystemFile()
    {
        WRAPPER_CONTRACT;

        _ASSERTE(m_pSystemDomain);
        return System()->m_pSystemFile;
    }

    static Assembly* SystemAssembly()
    {
        WRAPPER_CONTRACT;

        _ASSERTE(m_pSystemDomain);
        return System()->m_pSystemAssembly;
    }

    static Module* SystemModule()
    {
        WRAPPER_CONTRACT;

        Assembly *pAssembly = SystemAssembly();
        if (pAssembly == NULL)
            return NULL;
        else
            return pAssembly->GetManifestModule();
    }

    static BOOL IsSystemLoaded()
    {
        WRAPPER_CONTRACT;

        return System()->m_pSystemAssembly != NULL;
    }

#ifndef DACCESS_COMPILE
    static GlobalStringLiteralMap *GetGlobalStringLiteralMap()
    {
        WRAPPER_CONTRACT;

        if (m_pGlobalStringLiteralMap == NULL)
        {
            SystemDomain::LazyInitGlobalStringLiteralMap();
        }
        _ASSERTE(m_pGlobalStringLiteralMap);
        return m_pGlobalStringLiteralMap;
    }
    static GlobalStringLiteralMap *GetGlobalStringLiteralMapNoCreate()
    {
        LEAF_CONTRACT;

        _ASSERTE(m_pGlobalStringLiteralMap);
        return m_pGlobalStringLiteralMap;
    }
#endif // DACCESS_COMPILE

    static void ExecuteMainMethod(HMODULE hMod, LPWSTR path = NULL);
    static void ActivateApplication(int *pReturnValue);

    static void InitializeDefaultDomain(BOOL allowRedirects);
    static void SetupDefaultDomain();
    static HRESULT SetupDefaultDomainNoThrow();

    static void SetDefaultDomainAttributes(IMDInternalImport* pScope, mdMethodDef mdMethod);

    static HRESULT RunDllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved);

    //****************************************************************************************
    //
    // Use an already exising & inited Application Domain (e.g. a subclass).
    static void LoadDomain(AppDomain     *pDomain);

#ifndef DACCESS_COMPILE
    static void MakeUnloadable(AppDomain* pApp)
    {
        System()->AddDomain(pApp);
        pApp->SetCanUnload();
    }
#endif // DACCESS_COMPILE


    //****************************************************************************************
    // Method used to get the callers module and hence assembly and app domain.

    // these are obsolete and should not be used. Use the stackMark version instead
    static Module* GetCallersModule(int skip);
    static Assembly* GetCallersAssembly(int skip);
     static MethodDesc* GetCallersMethod(StackCrawlMark* stackMark,
                                       AppDomain **ppAppDomain/*=NULL*/);
    static MethodTable* GetCallersType(StackCrawlMark* stackMark, AppDomain **ppAppDomain = NULL);
    static Module* GetCallersModule(StackCrawlMark* stackMark, AppDomain **ppAppDomain = NULL);
    static Assembly* GetCallersAssembly(StackCrawlMark* stackMark, AppDomain **ppAppDomain = NULL);

    //****************************************************************************************
    // Returns the domain associated with the current context. (this can only be a child domain)
    static inline AppDomain* GetCurrentDomain()
    {
        WRAPPER_CONTRACT;

        return GetAppDomain();
    }


#ifdef DEBUGGING_SUPPORTED
    //****************************************************************************************
    // Debugger/Publisher helper function to indicate creation of new app domain to debugger
    // and publishing it in the IPC block
    static void PublishAppDomainAndInformDebugger (AppDomain *pDomain);
#endif // DEBUGGING_SUPPORTED

    //****************************************************************************************
    // Helper function to remove a domain from the system
    BOOL RemoveDomain(AppDomain* pDomain); // Does not decrement the reference

#ifdef PROFILING_SUPPORTED
    //****************************************************************************************
    // Tell profiler about system created domains which are created before the profiler is
    // actually activated.
    static void NotifyProfilerStartup();

    //****************************************************************************************
    // Tell profiler at shutdown that system created domains are going away.  They are not
    // torn down using the normal sequence.
    static HRESULT NotifyProfilerShutdown();
#endif // PROFILING_SUPPORTED

    ApplicationSecurityDescriptor* GetSecurityDescriptor()
    {
        LEAF_CONTRACT;

        return NULL;
    }

    //****************************************************************************************
    // return the dev path
    void GetDevpathW(LPWSTR* pPath, DWORD* pSize);

#ifndef DACCESS_COMPILE
    void IncrementNumAppDomains ()
    {
        LEAF_CONTRACT;

        s_dNumAppDomains++;
    }

    void DecrementNumAppDomains ()
    {
        LEAF_CONTRACT;

        s_dNumAppDomains--;
    }

    ULONG GetNumAppDomains ()
    {
        LEAF_CONTRACT;

        return s_dNumAppDomains;
    }
#endif // DACCESS_COMPILE

    //
    // AppDomains currently have both an index and an ID.  The
    // index is "densely" assigned; indices are reused as domains
    // are unloaded.  The Id's on the other hand, are not reclaimed
    // so may be sparse.
    //
    // Another important difference - it's OK to call GetAppDomainAtId for
    // an unloaded domain (it will return NULL), while GetAppDomainAtIndex
    // will assert if the domain is unloaded.

    static ADIndex GetNewAppDomainIndex(AppDomain *pAppDomain);
    static void ReleaseAppDomainIndex(ADIndex indx);
    static AppDomain *GetAppDomainAtIndex(ADIndex indx);
    static AppDomain *TestGetAppDomainAtIndex(ADIndex indx);
    static DWORD GetCurrentAppDomainMaxIndex()
    {
        WRAPPER_CONTRACT;

        ArrayListStatic* list = (ArrayListStatic *)&m_appDomainIndexList;
        PREFIX_ASSUME(list!=NULL);
        return list->GetCount();
    }

    static ADID GetNewAppDomainId(AppDomain *pAppDomain);
    static void ReleaseAppDomainId(ADID indx);
    
#ifndef DACCESS_COMPILE
    static ADID GetCurrentAppDomainMaxId() { ADID id; id.m_dwId=m_appDomainIdList.GetCount(); return id;}
#endif // DACCESS_COMPILE


#ifndef DACCESS_COMPILE
    DWORD RequireAppDomainCleanup()
    {
        return m_pDelayedUnloadList != 0;
    }

    void AddToDelayedUnloadList(AppDomain* pDomain, BOOL bAsync)
    {
       CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;
        m_UnloadIsAsync = bAsync;
        
        CrstHolder lh(&m_DelayedUnloadCrst);
        pDomain->m_pNextInDelayedUnloadList=m_pDelayedUnloadList;
        m_pDelayedUnloadList=pDomain;
        if (m_UnloadIsAsync)
        {
            pDomain->AddRef();
            int iGCRefPoint=GCHeap::GetGCHeap()->CollectionCount(GCHeap::GetGCHeap()->GetMaxGeneration());
            if (GCHeap::GetGCHeap()->IsGCInProgress())
                iGCRefPoint++;
            pDomain->SetGCRefPoint(iGCRefPoint);
        }
    }


    void ClearCollectedDomains()
    {
        CONTRACTL
        {
            GC_TRIGGERS;
            NOTHROW;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;
        
        AppDomain* pDomainsToClear=NULL;
        {
            CrstHolder lh(&m_DelayedUnloadCrst); 
            for (AppDomain** ppDomain=&m_pDelayedUnloadList;(*ppDomain)!=NULL; )
            {
                if ((*ppDomain)->m_Stage==AppDomain::STAGE_COLLECTED)
                {
                    AppDomain* pAppDomain=*ppDomain;
                    *ppDomain=(*ppDomain)->m_pNextInDelayedUnloadList;
                    pAppDomain->m_pNextInDelayedUnloadList=pDomainsToClear;
                    pDomainsToClear=pAppDomain;
                }
                else
                    ppDomain=&((*ppDomain)->m_pNextInDelayedUnloadList);
            }
        }
        
        for (AppDomain* pDomain=pDomainsToClear;pDomain!=NULL;)
        {
            AppDomain* pNext=pDomain->m_pNextInDelayedUnloadList;
            pDomain->Close(); //NOTHROW!
            pDomain->Release();
            pDomain=pNext;
        }
    }
 
    void ProcessClearingDomains()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;           
        }
        CONTRACTL_END;
        CrstHolder lh(&m_DelayedUnloadCrst); 

        for (AppDomain** ppDomain=&m_pDelayedUnloadList;(*ppDomain)!=NULL; )
        {
            if ((*ppDomain)->m_Stage==AppDomain::STAGE_HANDLETABLE_NOACCESS)
            {
                AppDomain* pAppDomain=*ppDomain;
                pAppDomain->SetStage(AppDomain::STAGE_CLEARED);
            }
            ppDomain=&((*ppDomain)->m_pNextInDelayedUnloadList);
        }
        
        if (!m_UnloadIsAsync)
        {
            // For synchronous mode, we are now done with the list.
            m_pDelayedUnloadList = NULL;    
        }
    }

    void ProcessDelayedUnloadDomains()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_TRIGGERS;
            MODE_COOPERATIVE;
        }
        CONTRACTL_END;    
        int iGCRefPoint=GCHeap::GetGCHeap()->CollectionCount(GCHeap::GetGCHeap()->GetMaxGeneration());
        if (GCHeap::GetGCHeap()->IsConcurrentGCInProgress())
            iGCRefPoint--;
        BOOL bChanged=FALSE;
        CrstHolder lh(&m_DelayedUnloadCrst); 
        for (AppDomain* pDomain=m_pDelayedUnloadList; pDomain!=NULL ;pDomain=pDomain->m_pNextInDelayedUnloadList)
        {
            if (pDomain->m_Stage==AppDomain::STAGE_CLEARED)
            {
                if (pDomain->GetGCRefPoint()<iGCRefPoint)
                {
                    bChanged=TRUE;
                    pDomain->SetStage(AppDomain::STAGE_COLLECTED);
                }
            }
        }
        if (bChanged)
             AppDomain::EnableADUnloadWorkerForCollectedADCleanup();

    }
    
    static void SetUnloadInProgress(AppDomain *pDomain)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(m_pAppDomainBeingUnloaded == NULL);
        m_pAppDomainBeingUnloaded = pDomain;
        m_dwIndexOfAppDomainBeingUnloaded = pDomain->GetIndex();
    }

    static void SetUnloadDomainCleared()
    {
        LEAF_CONTRACT;

        // about to delete, so clear this pointer so nobody uses it
        m_pAppDomainBeingUnloaded = NULL;
    }
    static void SetUnloadComplete()
    {
        LEAF_CONTRACT;

        // should have already cleared the AppDomain* prior to delete
        // either we succesfully unloaded and cleared or we failed and restored the ID
        _ASSERTE(m_pAppDomainBeingUnloaded == NULL && m_dwIndexOfAppDomainBeingUnloaded.m_dwIndex != 0
            || m_pAppDomainBeingUnloaded && SystemDomain::GetAppDomainAtId(m_pAppDomainBeingUnloaded->GetId()) != NULL);
        m_pAppDomainBeingUnloaded = NULL;
        m_pAppDomainUnloadingThread = NULL;
    }

    static AppDomain *AppDomainBeingUnloaded()
    {
        LEAF_CONTRACT;
        return m_pAppDomainBeingUnloaded;
    }

    static ADIndex IndexOfAppDomainBeingUnloaded()
    {
        LEAF_CONTRACT;
        return m_dwIndexOfAppDomainBeingUnloaded;
    }

    static void SetUnloadRequestingThread(Thread *pRequestingThread)
    {
        LEAF_CONTRACT;
        m_pAppDomainUnloadRequestingThread = pRequestingThread;
    }

    static Thread *GetUnloadRequestingThread()
    {
        LEAF_CONTRACT;
        return m_pAppDomainUnloadRequestingThread;
    }

    static void SetUnloadingThread(Thread *pUnloadingThread)
    {
        LEAF_CONTRACT;
        m_pAppDomainUnloadingThread = pUnloadingThread;
    }

    static Thread *GetUnloadingThread()
    {
        LEAF_CONTRACT;
        return m_pAppDomainUnloadingThread;
    }

    static void EnumAllStaticGCRefs(GCEnumCallback  pCallback, LPVOID hCallBack);
#endif // DACCESS_COMPILE


    //****************************************************************************************
    // Routines to deal with the base library (currently mscorlib.dll)
    LPCWSTR BaseLibrary()
    {
        WRAPPER_CONTRACT;

        return m_BaseLibrary;
    }

#ifndef DACCESS_COMPILE
    BOOL IsBaseLibrary(SString &path)
    {
        WRAPPER_CONTRACT;

        // See if it is the installation path to mscorlib
        if (path.EqualsCaseInsensitive(m_BaseLibrary, PEImage::GetFileSystemLocale()))
            return TRUE;

        // Or, it might be the GAC location of mscorlib
        if (System()->SystemAssembly() != NULL
            && path.EqualsCaseInsensitive(System()->SystemAssembly()->GetManifestFile()->GetPath(),
                                          PEImage::GetFileSystemLocale()))
            return TRUE;

        return FALSE;
    }

    BOOL IsBaseLibrarySatellite(SString &path)
    {
        WRAPPER_CONTRACT;

        // See if it is the installation path to mscorlib.resources
        if (path.EqualsCaseInsensitive(SString(SString::Ascii,g_psBaseLibrarySatelliteAssemblyName), PEImage::GetFileSystemLocale()))
            return TRUE;


        /*
        // Or, it might be the GAC location of mscorlib.resources
        if (System()->SystemAssembly() != NULL
            && path.EqualsCaseInsensitive(System()->SystemAssembly()->GetManifestFile()->GetPath(),
                                          PEImage::GetFileSystemLocale()))
            return TRUE;
        */

        return FALSE;
    }
#endif // DACCESS_COMPILE

    // Return the system directory
    LPCWSTR SystemDirectory()
    {
        WRAPPER_CONTRACT;

        return m_SystemDirectory;
    }

private:

    //****************************************************************************************
    // Helper function to create the single COM domain
    void CreateDefaultDomain();

    //****************************************************************************************
    // Helper function to add a domain to the global list
    void AddDomain(AppDomain* pDomain);

    void CreatePreallocatedExceptions();

    void PreallocateSpecialObjects();

    //****************************************************************************************
    //
    static StackWalkAction CallersMethodCallback(CrawlFrame* pCrawlFrame, VOID* pClientData);
    static StackWalkAction CallersMethodCallbackWithStackMark(CrawlFrame* pCrawlFrame, VOID* pClientData);

#ifndef DACCESS_COMPILE
    // This class is not to be created through normal allocation.
    SystemDomain()
    {
        WRAPPER_CONTRACT;

        m_pChildren = NULL;
        m_pDefaultDomain = NULL;
        m_pPool = NULL;
        m_pBaseComObjectClass = NULL;
        m_dwZapLogDirectory = 0;
        m_pDelayedUnloadList=NULL;
        m_UnloadIsAsync = FALSE;
    }
#endif

    PTR_PEAssembly  m_pSystemFile;      // Single assembly (here for quicker reference);
    PTR_Assembly    m_pSystemAssembly;  // Single assembly (here for quicker reference);
    PTR_AppDomain   m_pChildren;        // Children domain
    PTR_AppDomain   m_pDefaultDomain;   // Default domain for COM+ classes exposed through IClassFactory.
    PTR_AppDomain   m_pPool;            // Created and pooled objects
    PTR_MethodTable m_pBaseComObjectClass; // The default wrapper class for COM

    InlineSString<100>  m_BaseLibrary;
    InlineSString<100>  m_SystemDirectory;

    WCHAR       m_pZapLogDirectory[MAX_PATH];
    DWORD       m_dwZapLogDirectory;

    LPWSTR      m_pwDevpath;
    DWORD       m_dwDevpath;
    BOOL        m_fDevpath;  // have we searched the environment


    // Global domain that every one uses
    SPTR_DECL(SystemDomain, m_pSystemDomain);

    AppDomain* m_pDelayedUnloadList;
    BOOL m_UnloadIsAsync;

    SVAL_DECL(ArrayListStatic, m_appDomainIndexList);
#ifndef DACCESS_COMPILE
    static CrstStatic m_DelayedUnloadCrst;
    static CrstStatic       m_SystemDomainCrst;

    static ArrayListStatic  m_appDomainIdList;

    // only one ad can be unloaded at a time
    static AppDomain*   m_pAppDomainBeingUnloaded;
    // need this so can determine AD being unloaded after it has been deleted
    static ADIndex      m_dwIndexOfAppDomainBeingUnloaded;

    // if had to spin off a separate thread to do the unload, this is the original thread.
    // allows us to delay aborting it until it's the last one so that it can receive
    // notification of an unload failure
    static Thread *m_pAppDomainUnloadRequestingThread;

    // this is the thread doing the actual unload. He's allowed to enter the domain
    // even if have started unloading.
    static Thread *m_pAppDomainUnloadingThread;

    static GlobalStringLiteralMap *m_pGlobalStringLiteralMap;

    static ULONG       s_dNumAppDomains;  // Maintain a count of children app domains.

    static DWORD        m_dwLowestFreeIndex;
#endif // DACCESS_COMPILE


protected:

    // These flags let the correct native image of mscorlib to be loaded.
    // This is important for hardbinding to it

    static BOOL     s_fForceDebug;
    static BOOL     s_fForceProfiling;
    static BOOL     s_fForceInstrument;

public:
    static void     SetCompilationOverrides(BOOL fForceDebug,
                                            BOOL fForceProfiling,
                                            BOOL fForceInstrument);

    static void     GetCompilationOverrides(BOOL * fForceDebug,
                                            BOOL * fForceProfiling,
                                            BOOL * fForceInstrument);
public:
    //****************************************************************************************
    //

#ifndef DACCESS_COMPILE
#ifdef _DEBUG
inline static BOOL IsUnderDomainLock() { LEAF_CONTRACT; return m_SystemDomainCrst.OwnedByCurrentThread();};
#endif
#endif // DACCESS_COMPILE

    // This lock controls adding and removing domains from the system domain
    class LockHolder : public CrstPreempHolder
    {
    public:
        LockHolder(BOOL Take = TRUE)
#ifndef DACCESS_COMPILE
            : CrstPreempHolder(&m_SystemDomainCrst, Take)
#endif // DACCESS_COMPILE
        {
            LEAF_CONTRACT;
        }
    };

#ifdef DACCESS_COMPILE
public:
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                                   bool enumThis);
#endif

};



//
// an AppDomainIterator is used to iterate over all existing domains.
//
// The iteration is guaranteed to include all domains that exist at the
// start & end of the iteration.  Any domains added or deleted during
// iteration may or may not be included.  The iterator also guarantees
// that the current iterated appdomain (GetDomain()) will not be deleted.
//

class AppDomainIterator
{
    friend class SystemDomain;

  public:
    AppDomainIterator(BOOL bOnlyActive)
    {
        WRAPPER_CONTRACT;
        Init(bOnlyActive);
    }

    ~AppDomainIterator()
    {
        WRAPPER_CONTRACT;

#ifndef DACCESS_COMPILE
        if (m_pCurrent != NULL)
        {
#ifdef _DEBUG            
            m_pCurrent->IteratorRelease();
#endif            
            m_pCurrent->Release();
        }
#endif
    }

    void Init(BOOL bOnlyActive)
    {
        WRAPPER_CONTRACT;

        SystemDomain* sysDomain = SystemDomain::System();
        if (sysDomain)
        {
            ArrayListStatic* list = &sysDomain->m_appDomainIndexList;
            PREFIX_ASSUME(list != NULL);
            m_i = list->Iterate();
        }
        else
        {
            m_i.SetEmpty();
        }
        
        m_bOnlyActive = bOnlyActive;
        m_pCurrent = NULL;
    }

    BOOL Next()
    {
        WRAPPER_CONTRACT;

#ifndef DACCESS_COMPILE
        if (m_pCurrent != NULL)
        {
#ifdef _DEBUG            
            m_pCurrent->IteratorRelease();
#endif            
            m_pCurrent->Release();
        }

        SystemDomain::LockHolder lh;
#endif

        while (m_i.Next())
        {
            m_pCurrent = PTR_AppDomain((TADDR)m_i.GetElement());
            if (m_pCurrent != NULL &&
                (m_bOnlyActive ?
                 m_pCurrent->IsActive() : m_pCurrent->IsValid()))
            {
#ifndef DACCESS_COMPILE
                m_pCurrent->AddRef();
#ifdef _DEBUG            
                m_pCurrent->IteratorAcquire();
#endif            
#endif
                return TRUE;
            }
        }

        m_pCurrent = NULL;
        return FALSE;
    }

    AppDomain *GetDomain()
    {
        LEAF_CONTRACT;

        return m_pCurrent;
    }

  private:

    ArrayList::Iterator m_i;
    AppDomain *         m_pCurrent;
    BOOL                m_bOnlyActive;
};

typedef VPTR(class SharedDomain) PTR_SharedDomain;

class SharedDomain : public BaseDomain
{

    VPTR_VTABLE_CLASS(SharedDomain, BaseDomain)

  public:

    static void Attach();
    static void Detach();

    virtual BOOL IsSharedDomain() { LEAF_CONTRACT; return TRUE; }

    AppDomain *AsAppDomain()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        _ASSERTE(!"Not an AppDomain");
        return NULL;
    }

    static SharedDomain *GetDomain();

    void Init();
    void Terminate();

    // This will also set the tenured bit if and only if the add was successful,
    // and will make sure that the bit appears atomically set to all readers that
    // might be accessing the hash on another thread.
    void AddShareableAssembly(Assembly *pAssembly);

    MethodTable *FindIndexClass(SIZE_T index);

    class SharedAssemblyIterator
    {
        PtrHashMap::PtrIterator i;
        Assembly *m_pAssembly;

      public:
        SharedAssemblyIterator() :
          i(GetDomain() ? GetDomain()->m_assemblyMap.firstBucket() : NULL)
        { LEAF_CONTRACT; }

        BOOL Next()
        {
            WRAPPER_CONTRACT;

            if (i.end())
                return FALSE;

            m_pAssembly = PTR_Assembly((TADDR)i.GetValue());
            ++i;
            return TRUE;
        }

        Assembly *GetAssembly()
        {
            LEAF_CONTRACT;

            return m_pAssembly;
        }

      private:
        friend class SharedDomain;
    };

    Assembly *FindShareableAssembly(SharedAssemblyLocator *pLocator);

  private:
    friend class SharedAssemblyIterator;
    friend class SharedFileLockHolder;
    friend class ClrDataAccess;

#ifndef DACCESS_COMPILE
    void *operator new(size_t size, void *pInPlace);
    void operator delete(void *pMem);
#endif

    SPTR_DECL(SharedDomain, m_pSharedDomain);

    PEFileListLock          m_FileCreateLock;

    SIZE_T                  m_nextClassIndex;
    PtrHashMap              m_assemblyMap;


public:
#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                                   bool enumThis);
#endif
    // Hash map comparison function`
    static BOOL CompareSharedAssembly(UPTR u1, UPTR u2);
};

class SharedFileLockHolderBase : protected HolderBase<PEFile *>
{
  protected:
    PEFileListLock      *m_pLock;
    ListLockEntry   *m_pLockElement;

    SharedFileLockHolderBase(PEFile *value)
      : HolderBase<PEFile *>(value)
    {
        LEAF_CONTRACT;

        m_pLock = NULL;
        m_pLockElement = NULL;
    }

#ifndef DACCESS_COMPILE
    void DoAcquire()
    {
        STATIC_CONTRACT_THROWS;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FAULT;

        PEFileListLockHolder lockHolder(m_pLock);

        m_pLockElement = m_pLock->FindFileLock(m_value);
        if (m_pLockElement == NULL)
        {
            m_pLockElement = new ListLockEntry(m_pLock, m_value);
            m_pLock->AddElement(m_pLockElement);
        }
        else
            m_pLockElement->AddRef();

        lockHolder.Release();

        m_pLockElement->Enter();
    }

    void DoRelease()
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FORBID_FAULT;

        m_pLockElement->Leave();
        m_pLockElement->Release();
        m_pLockElement = NULL;
    }
#endif // DACCESS_COMPILE
};

class SharedFileLockHolder : public BaseHolder<PEFile *, SharedFileLockHolderBase>
{
  public:
    SharedFileLockHolder(SharedDomain *pDomain, PEFile *pFile, BOOL Take = TRUE)
      : BaseHolder<PEFile *, SharedFileLockHolderBase>(pFile, FALSE)
    {
        STATIC_CONTRACT_THROWS;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FAULT;

        m_pLock = &pDomain->m_FileCreateLock;
        if (Take)
            Acquire();
    }
};

inline BOOL BaseDomain::IsDefaultDomain()
{ 
    LEAF_CONTRACT; 
    return (SystemDomain::System()->DefaultDomain() == this);
}

#endif
