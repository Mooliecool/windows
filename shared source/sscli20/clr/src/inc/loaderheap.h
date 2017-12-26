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
//*****************************************************************************
// LoaderHeap.h
//
// Utility functions for managing memory allocations that typically do not
// need releasing.
//
//*****************************************************************************

#ifndef __LoaderHeap_h__
#define __LoaderHeap_h__

#include "utilcode.h"

//==============================================================================
// This structure packages up all the data needed to back out an AllocMem.
// It's mainly a short term parking place to get the data from the AllocMem
// to the AllocMemHolder while preserving the illusion that AllocMem() still
// returns just a pointer as it did in V1.
//==============================================================================
struct TaggedMemAllocPtr
{
    // Note: For AllocAlignedMem blocks, m_pMem and m_dwRequestedSize are the actual values to pass
    // to BackoutMem. Do not add "m_dwExtra"
    void        *m_pMem;                //Pointer to AllocMem'd block (needed to pass back to BackoutMem)
    size_t       m_dwRequestedSize;     //Requested allocation size (needed to pass back to BackoutMem)

    class LoaderHeap *m_pHeap;          //The heap that alloc'd the block (needed to know who to call BackoutMem on)


    //For AllocMem'd blocks, this is always 0.
    //For AllocAlignedMem blocks, you have to add m_dwExtra to m_pMem to arrive
    //   at the actual aligned pointer.
    size_t       m_dwExtra;

#ifdef _DEBUG
    char        *m_szFile;              //File that called AllocMem
    int          m_lineNum;             //Line # of AllocMem callsite
#endif

//! Note: this structure is copied around using bitwise copy ("=").
//! Don't get too fancy putting stuff in here. It's really just a temporary
//! holding place to get stuff from RealAllocMem() to the MemAllocHolder.


  public:

    //
    // This makes "void *ptr = pLoaderHeap->AllocMem()" work as in V1.
    //
    operator void*() const
    {
        LEAF_CONTRACT;
        return (void*)(m_dwExtra + (BYTE*)m_pMem);
    }

    template < typename T >
    T cast() const
    {
        LEAF_CONTRACT;
        return reinterpret_cast< T >( operator void *() );
    }
};



//==============================================================================
// AllocMemHolder : Allocated memory from LoaderHeap
//
// Old:
//
//   Foo* pFoo = (Foo*)pLoaderHeap->AllocMem(size);
//   pFoo->BackoutMem(pFoo, size)
//
//
// New:
//
//  {
//      AllocMemHolder<Foo> pfoo = pLoaderHeap->AllocMem();
//  } // BackoutMem on out of scope
//
//==============================================================================
template <typename TYPE>
class AllocMemHolder
{
    private:
        TaggedMemAllocPtr m_value;
        BOOL              m_fAcquired;


    //--------------------------------------------------------------------
    // All allowed (and disallowed) ctors here.
    //--------------------------------------------------------------------
    public:
        // Allow the construction "Holder h;"
        AllocMemHolder()
        {
            LEAF_CONTRACT;
            m_value.m_pMem = NULL;
            m_fAcquired    = FALSE;
        }

    public:
        // Allow the construction "Holder h = pHeap->AllocMem()"
        AllocMemHolder(const TaggedMemAllocPtr value)
        {
            LEAF_CONTRACT;
            m_value     = value;
            m_fAcquired = TRUE;
        }

    private:
        // Disallow "Holder holder1 = holder2"
        AllocMemHolder(const AllocMemHolder<TYPE> &);


    private:
        // Disallow "Holder holder1 = void*"
        AllocMemHolder(const LPVOID &);

    //--------------------------------------------------------------------
    // Destructor (and the whole point of AllocMemHolder)
    //--------------------------------------------------------------------
    public:
        ~AllocMemHolder()
        {
            WRAPPER_CONTRACT;
            if (m_fAcquired && m_value.m_pMem)
            {
                m_value.m_pHeap->RealBackoutMem(m_value.m_pMem,
                                                m_value.m_dwRequestedSize
#ifdef _DEBUG
                                               ,__FILE__
                                               ,__LINE__
                                               ,m_value.m_szFile
                                               ,m_value.m_lineNum
#endif
                                               );
            }
        }


    //--------------------------------------------------------------------
    // All allowed (and disallowed) assignment operators here.
    //--------------------------------------------------------------------
    public:
        // Reluctantly allow "AllocMemHolder h; ... h = pheap->AllocMem()"
        void operator=(const TaggedMemAllocPtr & value)
        {
            WRAPPER_CONTRACT;
            // However, prevent repeated assignments as that would leak.
            _ASSERTE(m_value.m_pMem == NULL && !m_fAcquired);
            m_value = value;
            m_fAcquired = TRUE;
        }

    private:
        // Disallow "holder == holder2"
        const AllocMemHolder<TYPE> & operator=(const AllocMemHolder<TYPE> &);

    private:
        // Disallow "holder = void*"
        const AllocMemHolder<TYPE> & operator=(const LPVOID &);


    //--------------------------------------------------------------------
    // Operations on the holder itself
    //--------------------------------------------------------------------
    public:
        // Call this when you're ready to take ownership away from the holder.
        void SuppressRelease()
        {
            LEAF_CONTRACT;
            m_fAcquired = FALSE;
        }



    //--------------------------------------------------------------------
    // ... And the smart-pointer stuff so we can drop holders on top
    // of former pointer variables (mostly)
    //--------------------------------------------------------------------
    public:
        // Allow holder to be treated as the underlying pointer type
        operator TYPE* ()
        {
            LEAF_CONTRACT;
            return (TYPE*)(void*)m_value;
        }

    public:
        // Allow holder to be treated as the underlying pointer type
        TYPE* operator->()
        {
            LEAF_CONTRACT;
            return (TYPE*)(void*)m_value;
        }
    public:
        int operator==(TYPE* value)
        {
            LEAF_CONTRACT;
            return ((void*)m_value) == ((void*)value);
        }

    public:
        int operator!=(TYPE* value)
        {
            LEAF_CONTRACT;
            return ((void*)m_value) != ((void*)value);
        }

    public:
        int operator!() const
        {
            LEAF_CONTRACT;
            return m_value.m_pMem == NULL;
        }


};



// # bytes to leave between allocations in debug mode
// Set to a > 0 boundary to debug problems - I've made this zero, otherwise a 1 byte allocation becomes
// a (1 + LOADER_HEAP_DEBUG_BOUNDARY) allocation
#define LOADER_HEAP_DEBUG_BOUNDARY  0

#define VIRTUAL_ALLOC_RESERVE_GRANULARITY (64*1024)    // 0x10000  (64 KB)

typedef DPTR(struct LoaderHeapBlock) PTR_LoaderHeapBlock;

struct LoaderHeapBlock
{
    PTR_LoaderHeapBlock     pNext;
    void *                  pVirtualAddress;
    size_t                  dwVirtualSize;
    BOOL                    m_fReleaseMemory;

    // pVirtualMemory  - the start address of the virtual memory
    // cbVirtualMemory - the length in bytes of the virtual memory
    // fReleaseMemory  - should LoaderHeap be responsible for releasing this memory
    void Init(void   *pVirtualMemory,
              size_t  cbVirtualMemory,
              BOOL    fReleaseMemory)
    {
        LEAF_CONTRACT;
        this->pNext = NULL;
        this->pVirtualAddress = pVirtualMemory;
        this->dwVirtualSize = cbVirtualMemory;
        this->m_fReleaseMemory = fReleaseMemory;
    }

    // Just calls LoaderHeapBlock::Init
    LoaderHeapBlock(void   *pVirtualMemory,
                    size_t  cbVirtualMemory,
                    BOOL    fReleaseMemory)
    {
        WRAPPER_CONTRACT;
        Init(pVirtualMemory, cbVirtualMemory, fReleaseMemory);
    }

    LoaderHeapBlock()
    {
        WRAPPER_CONTRACT;
        Init(NULL, 0, FALSE);
    }
};

// If we call UnlockedCanAllocMem, we'll actually try and allocate
// the memory, put it into the list, and then not use it till later.
// But we need to record the following information so that we can
// properly fix up the list when we do actually use it.  So we'll
// stick this info into the block we just allocated, but ONLY
// if the allocation was for a CanAllocMem.  Otherwise it's a waste
// of CPU time, and so we won't do it.
struct LoaderHeapBlockUnused : LoaderHeapBlock
{
    size_t                   cbCommitted;
    size_t                   cbReserved;
};


struct LoaderHeapFreeBlock;

// Collection of methods for helping in debugging heap corruptions
#ifdef _DEBUG
class  LoaderHeapSniffer;
struct LoaderHeapEvent;
#endif








//
//
//
//
class UnlockedLoaderHeap
{
#ifdef _DEBUG
    friend class LoaderHeapSniffer;
#endif

#ifdef DACCESS_COMPILE
    friend class ClrDataAccess;
#endif


private:
    // Linked list of ClrVirtualAlloc'd pages
    PTR_LoaderHeapBlock m_pFirstBlock;

    // Allocation pointer in current block
    BYTE *              m_pAllocPtr;

    // Points to the end of the committed region in the current block
    BYTE *              m_pPtrToEndOfCommittedRegion;
    BYTE *              m_pEndReservedRegion;

    PTR_LoaderHeapBlock m_pCurBlock;

    // When we need to ClrVirtualAlloc() MEM_RESERVE a new set of pages, number of bytes to reserve
    DWORD               m_dwReserveBlockSize;

    // When we need to commit pages from our reserved list, number of bytes to commit at a time
    DWORD               m_dwCommitBlockSize;

    // Created by in-place new?
    BOOL                m_fInPlace;

    // Range list to record memory ranges in
    RangeList *         m_pRangeList;

    size_t              m_dwTotalAlloc;

    size_t *             m_pPrivatePerfCounter_LoaderBytes;
    size_t *             m_pGlobalPerfCounter_LoaderBytes;

    DWORD                m_flProtect;

    LoaderHeapFreeBlock *m_pFirstFreeBlock;

    // This is used to hold on to a block of reserved memory provided to the
    // constructor. We do this instead of adding it as the first block because
    // that requires comitting the first page of the reserved block, and for
    // startup working set reasons we want to delay that as long as possible.
    LoaderHeapBlock      m_reservedBlock;

protected:
    // If the user is only willing to accept memory addresses above a certain point, then
    // this will be non-NULL.  Note that this involves iteratively testing memory
    // regions, etc, and should be assumed to be slow, a lot.
    const BYTE *        m_pMinAddr;

    // don't allocate anything that overlaps/is greater than this point in memory.
    const BYTE *        m_pMaxAddr;
public:

#ifdef _DEBUG
    enum
    {
        kCallTracing    = 0x00000001,   // Keep a permanent log of all callers

        kEncounteredOOM = 0x80000000,   // One time flag to record that an OOM interrupted call tracing
    }
    LoaderHeapDebugFlags;

    DWORD               m_dwDebugFlags;

    LoaderHeapEvent    *m_pEventList;   // Linked list of events (in reverse time order)
#endif



#ifdef _DEBUG
    size_t              m_dwDebugWastedBytes;
    static DWORD        s_dwNumInstancesOfLoaderHeaps;
#endif

#ifdef _DEBUG
    size_t DebugGetWastedBytes()
    {
        WRAPPER_CONTRACT;
        return m_dwDebugWastedBytes + GetBytesAvailCommittedRegion();
    }
#endif

#ifdef _DEBUG
    // Stubs allocated from a LoaderHeap will have unwind info registered with NT.
    // The info must be unregistered when the heap is destroyed.
    BOOL                m_fPermitStubsWithUnwindInfo;
    BOOL                m_fStubUnwindInfoUnregistered;
#endif

public:
    BOOL                m_fExplicitControl;  // Am I a LoaderHeap or an ExplicitControlLoaderHeap?

public:

    // Regular new
    void *operator new(size_t size)
    {
        WRAPPER_CONTRACT;
        void *pResult = new BYTE[size];

        if (pResult != NULL)
            ((UnlockedLoaderHeap *) pResult)->m_fInPlace = FALSE;

        return pResult;
    }

    // In-place new
    void *operator new(size_t size, void *pInPlace)
    {
        LEAF_CONTRACT;
        ((UnlockedLoaderHeap *) pInPlace)->m_fInPlace = TRUE;
        return pInPlace;
    }

    void operator delete(void *p)
    {
        LEAF_CONTRACT;
        if (p != NULL)
        {
            if (((UnlockedLoaderHeap *) p)->m_fInPlace == FALSE)
                ::delete[] ((BYTE*)p);
        }
    }

#ifdef DACCESS_COMPILE
public:
    void EnumMemoryRegions(enum CLRDataEnumMemoryFlags flags);
#endif

public:
    typedef void EnumPageRegionsCallback (PVOID pvAllocationBase, SIZE_T cbReserved);
    void EnumPageRegions (EnumPageRegionsCallback *pCallback);

protected:
    // Copies all the arguments, but DOESN'T actually allocate any memory,
    // yet.
    UnlockedLoaderHeap(DWORD dwReserveBlockSize,
                       DWORD dwCommitBlockSize,
                       size_t *pPrivatePerfCounter_LoaderBytes = NULL,
                       RangeList *pRangeList = NULL,
                       BOOL fMakeExecutable = FALSE,
                       const BYTE *pMinAddr = NULL,
                       const BYTE *pMaxAddr = NULL);

protected:
    // Use this version if dwReservedRegionAddress already points to a
    // blob of reserved memory.  This will set up internal data structures,
    // using the provided, reserved memory.
    UnlockedLoaderHeap(DWORD dwReserveBlockSize,
                       DWORD dwCommitBlockSize,
                       const BYTE* dwReservedRegionAddress,
                       DWORD dwReservedRegionSize,
                       size_t *pPrivatePerfCounter_LoaderBytes = NULL,
                       RangeList *pRangeList = NULL,
                       BOOL fMakeExecutable = FALSE);

protected:
    ~UnlockedLoaderHeap();

private:
    size_t GetBytesAvailCommittedRegion();
    size_t GetBytesAvailReservedRegion();

protected:
    BYTE *UnlockedGetAllocPtr()
    {
        LEAF_CONTRACT;
        return m_pAllocPtr;
    }

protected:
    // number of bytes available in region
    size_t UnlockedGetReservedBytesFree()
    {
        LEAF_CONTRACT;
        return m_pEndReservedRegion - m_pAllocPtr;
    }

protected:
    void* UnlockedGetFirstBlockVirtualAddress()
    {
        LEAF_CONTRACT;
        if (m_pFirstBlock != NULL)
            return m_pFirstBlock->pVirtualAddress;
        else
            return NULL;
    }

private:
    // Get some more committed pages - either commit some more in the current reserved region, or, if it
    // has run out, reserve another set of pages
    BOOL GetMoreCommittedPages(size_t dwMinSize,
                               BOOL bGrowHeap,
                               const BYTE *pMinAddr,
                               const BYTE *pMaxAddr,
                               BOOL fCanAlloc);

private:
    // Did a previous call to CanAllocMem(WithinRange) allocate space that we can use now?
    BOOL PreviouslyAllocated(BYTE *pMinAddr,
                             BYTE *pMaxAddr,
                             size_t dwMinSize,
                             BOOL fCanAlloc);


protected:
    // Reserve some pages either at any address or assumes the given address to have
    // already been reserved, and commits the given number of bytes.
    BOOL UnlockedReservePages(size_t dwCommitBlockSize,
                              const BYTE* dwReservedRegionAddress,
                              size_t dwReservedRegionSize,
                              const BYTE* pMinAddr,
                              const BYTE* pMaxAddr,
                              BOOL fCanAlloc);

protected:
    // In debug mode, allocate an extra LOADER_HEAP_DEBUG_BOUNDARY bytes and fill it with invalid data.  The reason we
    // do this is that when we're allocating vtables out of the heap, it is very easy for code to
    // get careless, and end up reading from memory that it doesn't own - but since it will be
    // reading some other allocation's vtable, no crash will occur.  By keeping a gap between
    // allocations, it is more likely that these errors will be encountered.
    void *UnlockedAllocMem(size_t dwSize
                          ,BOOL bGrowHeap
#ifdef _DEBUG
                          ,__in __in_z char *szFile
                          ,int  lineNum
#endif
                          );
    void *UnlockedAllocMem_NoThrow(size_t dwSize
                                   ,BOOL bGrowHeap
#ifdef _DEBUG
                                   ,__in __in_z char *szFile
                                   ,int  lineNum
#endif
                                   );





protected:
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
    void *UnlockedAllocAlignedMem(size_t  dwRequestedSize
                                 ,size_t  dwAlignment
                                 ,size_t *pdwExtra
                                 ,BOOL    bGrowHeap
#ifdef _DEBUG
                                 ,__in __in_z char *szFile
                                 ,int  lineNum
#endif
                                 );

    void *UnlockedAllocAlignedMem_NoThrow(size_t  dwRequestedSize
                                         ,size_t  dwAlignment
                                         ,size_t *pdwExtra
                                         ,BOOL    bGrowHeap
#ifdef _DEBUG
                                         ,__in __in_z char *szFile
                                         ,int  lineNum
#endif
                                 );



protected:
    // Don't actually increment the next free pointer, just tell us if we can.
    BOOL UnlockedCanAllocMem(size_t dwSize, BOOL bGrowHeap = TRUE);

protected:
    // This frees memory allocated by UnlockAllocMem. It's given this horrible name to emphasize
    // that it's purpose is for error path leak prevention purposes. You shouldn't
    // use LoaderHeap's as general-purpose alloc-free heaps.
    void UnlockedBackoutMem(void *pMem
                          , size_t dwSize
#ifdef _DEBUG
                          , __in __in_z char *szFile
                          , int lineNum
                          , __in __in_z char *szAllocFile
                          , int AllocLineNum
#endif
                          );



protected:
    // Don't actually increment the next free pointer, just tell us if we can get
    // memory within a certain range..
    BOOL UnlockedCanAllocMemWithinRange(size_t dwSize, BYTE *pStart, BYTE *pEnd, BOOL bGrowHeap);

public:
    // Perf Counter reports the size of the heap
    virtual size_t GetSize ()
    {
        LEAF_CONTRACT;
        return m_dwTotalAlloc;
    }

    BOOL IsExecutable()
    {
        return (PAGE_EXECUTE_READWRITE == m_flProtect);
    }


public:
#ifdef _DEBUG
    void DumpFreeList();
#endif

public:
// Extra CallTracing support
#ifdef _DEBUG
    void UnlockedClearEvents();     //Discard saved events
    void UnlockedCompactEvents();   //Discard matching alloc/free events
    void UnlockedPrintEvents();     //Print event list
#endif

protected:
    void *UnlockedAllocAlignedmem(size_t dwSize, DWORD alignment, BOOL bGrowHeap = TRUE);
    void *UnlockedAllocAlignedmem_NoThrow(size_t dwSize, DWORD alignment, BOOL bGrowHeap = TRUE);
};

//===============================================================================
// Create the LoaderHeap lock. It's the same lock for several different Heaps.
//===============================================================================
inline CRITSEC_COOKIE CreateLoaderHeapLock()
{
    return ClrCreateCriticalSection("LoaderHeap",CrstLoaderHeap,CrstFlags(CRST_UNSAFE_ANYMODE | CRST_DEBUGGER_THREAD));
}

//===============================================================================
// The LoaderHeap is the black-box heap and has a Backout() method but none
// of the advanced features that let you control address ranges.
//===============================================================================
typedef DPTR(class LoaderHeap) PTR_LoaderHeap;
class LoaderHeap : public UnlockedLoaderHeap
{
private:
    CRITSEC_COOKIE    m_CriticalSection;

public:
    LoaderHeap(DWORD dwReserveBlockSize,
               DWORD dwCommitBlockSize,
               size_t *pPrivatePerfCounter_LoaderBytes = NULL,
               RangeList *pRangeList = NULL,
               BOOL fMakeExecutable = FALSE,
               const BYTE *pMinAddr = NULL,
               const BYTE *pMaxAddr = NULL
               )
      : UnlockedLoaderHeap(dwReserveBlockSize,
                           dwCommitBlockSize,
                           pPrivatePerfCounter_LoaderBytes,
                           pRangeList,
                           fMakeExecutable,
                           pMinAddr,
                           pMaxAddr)
    {
        WRAPPER_CONTRACT;
        m_CriticalSection = NULL;
        m_CriticalSection = CreateLoaderHeapLock();
        m_fExplicitControl = FALSE;
    }

public:
    LoaderHeap(DWORD dwReserveBlockSize,
               DWORD dwCommitBlockSize,
               const BYTE* dwReservedRegionAddress,
               DWORD dwReservedRegionSize,
               size_t *pPrivatePerfCounter_LoaderBytes = NULL,
               RangeList *pRangeList = NULL,
               BOOL fMakeExecutable = FALSE
               )
      : UnlockedLoaderHeap(dwReserveBlockSize,
                           dwCommitBlockSize,
                           dwReservedRegionAddress,
                           dwReservedRegionSize,
                           pPrivatePerfCounter_LoaderBytes,
                           pRangeList,
                           fMakeExecutable)
    {
        WRAPPER_CONTRACT;
        m_CriticalSection = NULL;
        m_CriticalSection = CreateLoaderHeapLock();
        m_fExplicitControl = FALSE;
    }

public:
    ~LoaderHeap()
    {
        WRAPPER_CONTRACT;

        if (m_CriticalSection != NULL)
        {
            ClrDeleteCriticalSection(m_CriticalSection);
        }
    }



#ifdef _DEBUG
#define AllocMem(dwSize)                  RealAllocMem( (dwSize), TRUE, __FILE__, __LINE__ )
#define AllocMem_NoThrow(dwSize)          RealAllocMem_NoThrow( (dwSize), TRUE, __FILE__, __LINE__ )
#define AllocMemNoGrow(dwSize)            RealAllocMem( (dwSize), FALSE, __FILE__, __LINE__ )
#define AllocMemNoGrow_NoThrow(dwSize)    RealAllocMem_NoThrow( (dwSize), FALSE, __FILE__, __LINE__ )
#else
#define AllocMem(dwSize)                  RealAllocMem( (dwSize), TRUE )
#define AllocMem_NoThrow(dwSize)          RealAllocMem_NoThrow( (dwSize), TRUE )
#define AllocMemNoGrow(dwSize)            RealAllocMem( (dwSize), FALSE )
#define AllocMemNoGrow_NoThrow(dwSize)    RealAllocMem_NoThrow( (dwSize), FALSE )
#endif
public:
    TaggedMemAllocPtr RealAllocMem(size_t dwSize
                                  ,BOOL bGrowHeap
#ifdef _DEBUG
                                  ,__in __in_z char *szFile
                                  ,int  lineNum
#endif
                  )
    {
        WRAPPER_CONTRACT;

        void *pResult;
        TaggedMemAllocPtr tmap;

        CRITSEC_Holder csh(m_CriticalSection);
        pResult = UnlockedAllocMem(dwSize
                                 , bGrowHeap
#ifdef _DEBUG
                                 , szFile
                                 , lineNum
#endif
                                 );
        tmap.m_pMem             = pResult;
        tmap.m_dwRequestedSize  = dwSize;
        tmap.m_pHeap            = this;
        tmap.m_dwExtra          = 0;
#ifdef _DEBUG
        tmap.m_szFile           = szFile;
        tmap.m_lineNum          = lineNum;
#endif
        return tmap;
    }

    TaggedMemAllocPtr RealAllocMem_NoThrow(size_t dwSize
                                           ,BOOL bGrowHeap
#ifdef _DEBUG
                                           ,__in __in_z char *szFile
                                           ,int  lineNum
#endif
                  )
    {
        WRAPPER_CONTRACT;

        void *pResult;
        TaggedMemAllocPtr tmap;

        CRITSEC_Holder csh(m_CriticalSection);

        pResult = UnlockedAllocMem_NoThrow(dwSize
                                           , bGrowHeap
#ifdef _DEBUG
                                           , szFile
                                           , lineNum
#endif
                                           );

        tmap.m_pMem             = pResult;
        tmap.m_dwRequestedSize  = dwSize;
        tmap.m_pHeap            = this;
        tmap.m_dwExtra          = 0;
#ifdef _DEBUG
        tmap.m_szFile           = szFile;
        tmap.m_lineNum          = lineNum;
#endif

        return tmap;
    }



#ifdef _DEBUG
#define AllocAlignedMem(dwSize, dwAlign, pdwExtra)                  RealAllocAlignedMem( (dwSize), (dwAlign), (pdwExtra), TRUE, __FILE__, __LINE__)
#define AllocAlignedMem_NoThrow(dwSize, dwAlign, pdwExtra)          RealAllocAlignedMem_NoThrow( (dwSize), (dwAlign), (pdwExtra), TRUE, __FILE__, __LINE__)
#define AllocAlignedMemNoGrow(dwSize, dwAlign, pdwExtra)            RealAllocAlignedMem( (dwSize), (dwAlign), (pdwExtra), FALSE, __FILE__, __LINE__)
#define AllocAlignedMemNoGrow_NoThrow(dwSize, dwAlign, pdwExtra)    RealAllocAlignedMem_NoThrow( (dwSize), (dwAlign), (pdwExtra), FALSE, __FILE__, __LINE__)
#else
#define AllocAlignedMem(dwSize, dwAlign, pdwExtra)                  RealAllocAlignedMem( (dwSize), (dwAlign), (pdwExtra), TRUE )
#define AllocAlignedMem_NoThrow(dwSize, dwAlign, pdwExtra)          RealAllocAlignedMem_NoThrow( (dwSize), (dwAlign), (pdwExtra), TRUE )
#define AllocAlignedMemNoGrow(dwSize, dwAlign, pdwExtra)            RealAllocAlignedMem( (dwSize), (dwAlign), (pdwExtra), FALSE )
#define AllocAlignedMemNoGrow_NoThrow(dwSize, dwAlign, pdwExtra)    RealAllocAlignedMem_NoThrow( (dwSize), (dwAlign), (pdwExtra), FALSE )
#endif

public:
    TaggedMemAllocPtr RealAllocAlignedMem(size_t  dwRequestedSize
                                         ,size_t  dwAlignment
                                         ,size_t *pdwExtra
                                         ,BOOL    bGrowHeap
#ifdef _DEBUG
                                         ,__in __in_z char *szFile
                                         ,int  lineNum
#endif
                                         )
    {
        WRAPPER_CONTRACT;

        CRITSEC_Holder csh(m_CriticalSection);


        TaggedMemAllocPtr tmap;
        void *pResult;
        size_t dwExtra;

        pResult = UnlockedAllocAlignedMem(dwRequestedSize
                                         ,dwAlignment
                                         ,&dwExtra
                                         ,bGrowHeap
#ifdef _DEBUG
                                         ,szFile
                                         ,lineNum
#endif
                                     );

        if (pdwExtra)
        {
            *pdwExtra = dwExtra;
        }

        tmap.m_pMem             = (void*)(((BYTE*)pResult) - dwExtra);
        tmap.m_dwRequestedSize  = dwRequestedSize + dwExtra;
        tmap.m_pHeap            = this;
        tmap.m_dwExtra          = dwExtra;
#ifdef _DEBUG
        tmap.m_szFile           = szFile;
        tmap.m_lineNum          = lineNum;
#endif

        return tmap;
    }


    TaggedMemAllocPtr RealAllocAlignedMem_NoThrow(size_t  dwRequestedSize
                                                 ,size_t  dwAlignment
                                                 ,size_t *pdwExtra
                                                 ,BOOL    bGrowHeap
#ifdef _DEBUG
                                                 ,__in __in_z char *szFile
                                                 ,int  lineNum
#endif
                                                 )
    {
        WRAPPER_CONTRACT;

        CRITSEC_Holder csh(m_CriticalSection);


        TaggedMemAllocPtr tmap;
        void *pResult;
        size_t dwExtra;

        pResult = UnlockedAllocAlignedMem_NoThrow(dwRequestedSize
                                                 ,dwAlignment
                                                 ,&dwExtra
                                                 ,bGrowHeap
#ifdef _DEBUG
                                                 ,szFile
                                                 ,lineNum
#endif
                                            );

        _ASSERTE(!(pResult == NULL && dwExtra != 0));

        if (pdwExtra)
        {
            *pdwExtra = dwExtra;
        }

        tmap.m_pMem             = (void*)(((BYTE*)pResult) - dwExtra);
        tmap.m_dwRequestedSize  = dwRequestedSize + dwExtra;
        tmap.m_pHeap            = this;
        tmap.m_dwExtra          = dwExtra;
#ifdef _DEBUG
        tmap.m_szFile           = szFile;
        tmap.m_lineNum          = lineNum;
#endif

        return tmap;
    }




#ifdef _DEBUG
#define BackoutMem(pMem, dwSize)  RealBackoutMem( (pMem), (dwSize), __FILE__, __LINE__, "UNKNOWN", -1 )
#else
#define BackoutMem(pMem, dwSize)  RealBackoutMem( (pMem), (dwSize) )
#endif


public:
    // This frees memory allocated by AllocMem. It's given this horrible name to emphasize
    // that it's purpose is for error path leak prevention purposes. You shouldn't
    // use LoaderHeap's as general-purpose alloc-free heaps.
    void RealBackoutMem(void *pMem
                        , size_t dwSize
#ifdef _DEBUG
                        , __in __in_z char *szFile
                        , int lineNum
                        , __in __in_z char *szAllocFile
                        , int allocLineNum
#endif
                        )
    {
        WRAPPER_CONTRACT;
        CRITSEC_Holder csh(m_CriticalSection);
        UnlockedBackoutMem(pMem
                           , dwSize
#ifdef _DEBUG
                           , szFile
                           , lineNum
                           , szAllocFile
                           , allocLineNum
#endif
                           );
    }

public:
// Extra CallTracing support
#ifdef _DEBUG
    void ClearEvents()
    {
        WRAPPER_CONTRACT;
        CRITSEC_Holder csh(m_CriticalSection);
        UnlockedClearEvents();
    }

    void CompactEvents()
    {
        WRAPPER_CONTRACT;
        CRITSEC_Holder csh(m_CriticalSection);
        UnlockedCompactEvents();
    }

    void PrintEvents()
    {
        WRAPPER_CONTRACT;
        CRITSEC_Holder csh(m_CriticalSection);
        UnlockedPrintEvents();
    }
#endif

};





//===============================================================================
// The ExplicitControlLoaderHeap exposes all the advanced features but
// has no Backout() feature. (If someone wants a backout feature, they need
// to design an appropriate one into this class.)
//===============================================================================
typedef DPTR(class ExplicitControlLoaderHeap) PTR_ExplicitControlLoaderHeap;
class ExplicitControlLoaderHeap : public UnlockedLoaderHeap
{
private:
    CRITSEC_COOKIE    m_CriticalSection;

public:
    ExplicitControlLoaderHeap(DWORD dwReserveBlockSize,
                              DWORD dwCommitBlockSize,
                              size_t *pPrivatePerfCounter_LoaderBytes = NULL,
                              RangeList *pRangeList = NULL,
                              BOOL fMakeExecutable = FALSE,
                              const BYTE *pMinAddr = NULL,
                              const BYTE *pMaxAddr = NULL)
      : UnlockedLoaderHeap(dwReserveBlockSize,
                           dwCommitBlockSize,
                           pPrivatePerfCounter_LoaderBytes,
                           pRangeList,
                           fMakeExecutable,
                           pMinAddr,
                           pMaxAddr)
    {
        WRAPPER_CONTRACT;
        m_CriticalSection = NULL;
        m_CriticalSection = CreateLoaderHeapLock();
        m_fExplicitControl = TRUE;
    }

public:
    ExplicitControlLoaderHeap(DWORD dwReserveBlockSize,
                              DWORD dwCommitBlockSize,
                              const BYTE* dwReservedRegionAddress,
                              DWORD dwReservedRegionSize,
                              size_t *pPrivatePerfCounter_LoaderBytes = NULL,
                              RangeList *pRangeList = NULL,
                              BOOL fMakeExecutable = FALSE
               )
      : UnlockedLoaderHeap(dwReserveBlockSize,
                           dwCommitBlockSize,
                           dwReservedRegionAddress,
                           dwReservedRegionSize,
                           pPrivatePerfCounter_LoaderBytes,
                           pRangeList,
                           fMakeExecutable)
    {
        WRAPPER_CONTRACT;
        m_CriticalSection = NULL;
        m_CriticalSection = CreateLoaderHeapLock();
        m_fExplicitControl = TRUE;
    }

public:
    ~ExplicitControlLoaderHeap()
    {
        WRAPPER_CONTRACT;

        if (m_CriticalSection != NULL)
        {
            ClrDeleteCriticalSection(m_CriticalSection);
        }
    }



public:
    BYTE *GetNextAllocAddress()
    {
        WRAPPER_CONTRACT;
        BYTE *ptr;

        CRITSEC_Holder csh(m_CriticalSection);
        ptr = UnlockedGetAllocPtr();

        return ptr;
    }

    BYTE *GetAllocPtr()
    {
        WRAPPER_CONTRACT;
        return UnlockedGetAllocPtr();
    }

public:
    BOOL AllocateOntoReservedMem(const BYTE* dwReservedRegionAddress, DWORD dwReservedRegionSize)
    {
        WRAPPER_CONTRACT;
        BOOL result;

        CRITSEC_Holder csh(m_CriticalSection);
        result = ReservePages(0, dwReservedRegionAddress, dwReservedRegionSize, (PBYTE)BOT_MEMORY, (PBYTE)TOP_MEMORY, FALSE);

        return result;
    }


public:
    // This is only used for EnC.
    // If anyone else uses it, please change the above comment.
    BOOL CanAllocMem(size_t dwSize, BOOL bGrowHeap = TRUE)
    {
        WRAPPER_CONTRACT;
        BOOL bResult;

        CRITSEC_Holder csh(m_CriticalSection);

        bResult = UnlockedCanAllocMem(dwSize, bGrowHeap);

        return bResult;
    }


public:
    BOOL CanAllocMemWithinRange(size_t dwSize, BYTE *pStart, BYTE *pEnd, BOOL bGrowHeap)
    {
        WRAPPER_CONTRACT;
        BOOL bResult;

        CRITSEC_Holder csh(m_CriticalSection);

        bResult = UnlockedCanAllocMemWithinRange(dwSize, pStart, pEnd, bGrowHeap);

        return bResult;
    }

public:
    void *RealAllocMem(size_t dwSize
                       ,BOOL bGrowHeap
#ifdef _DEBUG
                       ,__in __in_z char *szFile
                       ,int  lineNum
#endif
                       )
    {
        WRAPPER_CONTRACT;

        void *pResult;

        CRITSEC_Holder csh(m_CriticalSection);
        pResult = UnlockedAllocMem(dwSize
                                   , bGrowHeap
#ifdef _DEBUG
                                   , szFile
                                   , lineNum
#endif
                                   );
        return pResult;
    }

    void *RealAllocMem_NoThrow(size_t dwSize
                               ,BOOL bGrowHeap
#ifdef _DEBUG
                               ,__in __in_z char *szFile
                               ,int  lineNum
#endif
                               )
    {
        WRAPPER_CONTRACT;

        void *pResult;

        CRITSEC_Holder csh(m_CriticalSection);

        pResult = UnlockedAllocMem_NoThrow(dwSize
                                           , bGrowHeap
#ifdef _DEBUG
                                           , szFile
                                           , lineNum
#endif
                                           );
        return pResult;
    }


public:
    void *AllocAlignedmem(size_t dwSize, DWORD alignment, BOOL bGrowHeap = TRUE)
    {
        WRAPPER_CONTRACT;
        CRITSEC_Holder csh(m_CriticalSection);
        return UnlockedAllocAlignedmem(dwSize, alignment, bGrowHeap);
    }

    void *AllocAlignedmem_NoThrow(size_t dwSize, DWORD alignment, BOOL bGrowHeap = TRUE)
    {
        WRAPPER_CONTRACT;
        CRITSEC_Holder csh(m_CriticalSection);
        return UnlockedAllocAlignedmem_NoThrow(dwSize, alignment, bGrowHeap);
    }

public:
    // number of bytes available in region
    size_t GetReservedBytesFree()
    {
        WRAPPER_CONTRACT;
        return UnlockedGetReservedBytesFree();
    }

public:
    void* GetFirstBlockVirtualAddress()
    {
        WRAPPER_CONTRACT;
        return UnlockedGetFirstBlockVirtualAddress();
    }

public:


    BOOL ReservePages(size_t dwCommitBlockSize,
                      const BYTE* dwReservedRegionAddress,
                      size_t dwReservedRegionSize,
                      const BYTE* pMinAddr,
                      const BYTE* pMaxAddr,
                      BOOL fCanAlloc)
    {
        WRAPPER_CONTRACT;
        return UnlockedReservePages(dwCommitBlockSize, dwReservedRegionAddress, dwReservedRegionSize, pMinAddr, pMaxAddr, fCanAlloc);
    }

public:
// Extra CallTracing support
#ifdef _DEBUG
    void ClearEvents()
    {
        WRAPPER_CONTRACT;
        CRITSEC_Holder csh(m_CriticalSection);
        UnlockedClearEvents();
    }

    void CompactEvents()
    {
        WRAPPER_CONTRACT;
        CRITSEC_Holder csh(m_CriticalSection);
        UnlockedCompactEvents();
    }

    void PrintEvents()
    {
        WRAPPER_CONTRACT;
        CRITSEC_Holder csh(m_CriticalSection);
        UnlockedPrintEvents();
    }
#endif



};



// This utility helps track loaderheap allocations. Its main purpose
// is to backout allocations in case of an exception.
class AllocMemTracker
{
    public:
        AllocMemTracker();
        ~AllocMemTracker();

        // Tells tracker to store an allocated loaderheap block.
        //
        // Returns the pointer address of block for convenience.
        //
        // Ok to call on failed loaderheap allocation (will just do nothing and propagate the OOM as apropos).
        //
        // If Track fails due to an OOM allocating node space, it will backout the loaderheap block before returning.
        void *Track(TaggedMemAllocPtr tmap);
        void *Track_NoThrow(TaggedMemAllocPtr tmap);

        void SuppressRelease();

    private:
        struct AllocMemTrackerNode
        {
            LoaderHeap      *m_pHeap;
            void            *m_pMem;
            size_t           m_dwRequestedSize;
#ifdef _DEBUG
            char            *m_szAllocFile;
            int              m_allocLineNum;
#endif
        };

        enum
        {
            kAllocMemTrackerBlockSize =
#ifdef _DEBUG
                                        3
#else
                                       20
#endif
        };

        struct AllocMemTrackerBlock
        {
            AllocMemTrackerBlock    *m_pNext;
            int                      m_nextFree;
            AllocMemTrackerNode      m_Node[kAllocMemTrackerBlockSize];
        };


        AllocMemTrackerBlock        *m_pFirstBlock;
        AllocMemTrackerBlock        m_FirstBlock; // Stack-allocate the first block - "new" the rest.

        BOOL                        m_fReleased;

};

#endif // __LoaderHeap_h__

