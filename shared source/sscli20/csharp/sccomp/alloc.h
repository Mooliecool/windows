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
// ===========================================================================
// File: alloc.h
//
// ===========================================================================

#ifndef __alloc_h__
#define __alloc_h__

#include "locks.h"
#include "iheapallocator.h"
class MEMHEAP;
class PAGEHEAP;

#ifdef DEBUG
#define DEBUGPROTECT
#endif // DEBUG

/*
    Memory Protection:

    In order to prevent corruption of long term data structures, and to catch
    errors when they happen, as opposed to when the symptoms appear, we have a
    policy of locking relevant allocated pages in read-only memory, and unlocking
    only around explicit writes.  Unallocated memory can be marked as NO_ACCESS
    which prevents reads as well as writes.

    To accomplish this we divide the memory we use into 4 Entities:
    Other
    Nametable memory
    Parse tree memory
    Unallocated memory

    We then have a policy for each memory-entity-kind which we can vary
    individually.

    All protection is ultimately accomplished by the static PAGEHEAP method
    called ToggleWrite which takes an enity kind describing which entity the
    address range to be operated upon belongs to.  We then perform a policy
    check which turns the ToggleWrite call into a NOP or into an actuall OS
    call based on what entities we decided to protect in the const declaration
    below.

    Callers to ToggleWrite have to always specify the correct entity kind, which
    only NRHEAPS do automatically by being marked with the entity kind at
    creation time.  A sideeffect of this is that any NRHEAP which participates
    in memory protection operations must belong to a specific entity different
    than Other.  We ensure the latter by well places asserts in externally
    exposed NRHEAP operations.

*/


namespace ProtectedEntityFlags{
    enum _Enum {
        Other = 0,
        Nametable = 1,
        ParseTree = 2,
        UnusedMemory = 4,
    };
};
DECLARE_FLAGS_TYPE(ProtectedEntityFlags);

ProtectedEntityFlagsEnum const whatIsProtected = 
#ifdef DEBUG
    ProtectedEntityFlags::Nametable | ProtectedEntityFlags::ParseTree | ProtectedEntityFlags::UnusedMemory
#else
            ProtectedEntityFlags::Other
#endif
;

#ifndef UNREACHABLE
// Bug PREFast_:510  v4.51 does not support __assume(0)
#if (defined(_MSC_VER) && !defined(_PREFAST_)) || defined(_PREFIX_)
#define UNREACHABLE() __assume(0)
#else
#define UNREACHABLE()  do { } while(true)
#endif
#endif

////////////////////////////////////////////////////////////////////////////////
// ALLOCHOST
//
// Each allocator must be created with a pointer to a "host".  A host is a sink
// for "out of memory events" and is also an access point for getting to other 
// allocators which this allocator should use as appropriate.
class ALLOCHOST
{
public:    
    _declspec(noreturn) virtual void NoMemory () = 0; // The method must exit by throwing an exception.
    virtual MEMHEAP     *GetStandardHeap () = 0;
    virtual PAGEHEAP    *GetPageHeap () = 0;
    
    // Helper for implementors of ALLOCMHOST so NoMemory throws a consistent exception.
    _declspec(noreturn) static void ThrowOutOfMemoryException() {
        RaiseException(STATUS_NO_MEMORY, 0, 0, 0);
        UNREACHABLE();
    }
};

////////////////////////////////////////////////////////////////////////////////
// Alloc, AllocZero are MACROS!
// In DEBUG, these add the file and line to the allocation for leak detection.

#ifdef DEBUG
#define Alloc(s) _Alloc (s, __FILE__, __LINE__)
#define AllocZero(s) _AllocZero (s, __FILE__, __LINE__)
#else
#define Alloc(s) _Alloc (s)
#define AllocZero(s) _AllocZero (s)
#endif

/* Standard memory heap; supports alloc/free/realloc.
 * Out-of-memory failures call ALLOCHOST::NoMemory which always exits via an exception.
 * Not synchronized.
 */
class MEMHEAP {
public:
    MEMHEAP(ALLOCHOST * host, bool bTrackMem = false);
    ~MEMHEAP();

    //void * operator new(size_t size) { return VSAlloc (size); }
    //void operator delete(void * p) { VSFree (p); }


#ifdef DEBUG
    void * _Alloc(size_t sz, PCSTR pszFile, UINT iLine);
    void * _AllocZero(size_t sz, PCSTR pszFile, UINT iLine);
#else
    void * _Alloc(size_t sz);
    void * _AllocZero(size_t sz);
#endif
    PWSTR AllocStr(PCWSTR str);
    void * Realloc(void * p, size_t sz);
    void * ReallocZero(void * p, size_t sz);
    void Free(void * p);
    void FreeHeap(bool checkLeaks = true);

    __forceinline unsigned GetCurrentSize() { return 0; }
    __forceinline unsigned GetMaxSize() { return 0; }
    __forceinline void SetMemTrack( bool bTrackMem) { m_bTrackMem = bTrackMem; }

private:
    HANDLE heap;
    ALLOCHOST * host;
    bool m_bTrackMem;
};


/* Page allocation heap. Allocates and frees pages
 * or groups of pages. Allocation must be a multiple
 * of the system page size. Memory is not zeroed.
 */

#define PAGES_PER_ARENA 1024     // 4M or 8M on typical systems.
#define BIGALLOC_SIZE   (256 * 1024) // more than this alloc is not done from an arena.

#define DWORD_BIT_SHIFT 5        // log2 of bits in a DWORD.
#define BITS_DWORD      (1 << DWORD_BIT_SHIFT)
#define DWORD_BIT_MASK  (BITS_DWORD - 1)

class ProtectionToggleLock 
{
public:
    ProtectionToggleLock();
    ~ProtectionToggleLock();
private:
    bool locked;
};

class PAGEHEAP {
public:
    // Store information about a memory arena we allocate pages from.
    struct PAGEARENA {
        PAGEARENA * nextArena;  // the arena.
        void * pages;           // the pages in the arena.
        size_t size;            // size of the arena.
        bool largeAlloc;        // if true, a "large allocation", and the bitmaps aren't used.
        DWORD used[PAGES_PER_ARENA / BITS_DWORD];        // bit map of in-use pages in this arena.
        DWORD committed[PAGES_PER_ARENA / BITS_DWORD];   // bit map of committed pages in this arena.
        bool OwnsPage(void * p) {
            return (p >= pages && p < (BYTE *)pages + size);
        }

        bool IsPageCommitted (unsigned iPage) const
        {
            return !!(committed[iPage >> DWORD_BIT_SHIFT] & (1 << (iPage & DWORD_BIT_MASK)));
        }
        bool IsPageUsed (unsigned iPage) const
        {
            return !!(used[iPage >> DWORD_BIT_SHIFT] & (1 << (iPage & DWORD_BIT_MASK)));
        }

    };

    PAGEHEAP(ALLOCHOST * host, bool bTrackMem = false);
    ~PAGEHEAP();

    static void StaticInit();

    //void * operator new(size_t size) { return VSAlloc (size); }
    //void operator delete(void * p) { VSFree (p); }

    void * AllocPages(size_t sz);
    void FreePages(ProtectedEntityFlagsEnum entity, void * p, size_t sz);
    void FreeAllPages(bool checkLeaks = true);
    void DecommitUnusedPages();

    static DWORD ForbidWrite(ProtectedEntityFlagsEnum entity, void * p, size_t sz) { return ToggleWrite(entity, p, sz, PAGE_READONLY); }
    static DWORD AllowWrite(ProtectedEntityFlagsEnum entity, void * p, size_t sz) { return ToggleWrite(entity, p, sz, PAGE_READWRITE); }
    static DWORD ForbidAccess(ProtectedEntityFlagsEnum entity, void *p, size_t sz) { return ToggleWrite(entity, p, sz, PAGE_NOACCESS); }

    static ProtectedEntityFlagsEnum StaticWhatIsProtected() { return ::whatIsProtected; };
    ProtectedEntityFlagsEnum InstanceWhatIsProtected() { return this->whatIsProtected; };
    static bool IsEntityProtected(ProtectedEntityFlagsEnum entity) { return !!(entity & StaticWhatIsProtected()); }

    static size_t pageSize;         // The system page size.
    static int pageShift;           // log2 of the page size
    static bool reliableCommit;     // Commit of memory protects it correctly even if already committed

    __forceinline unsigned GetCurrentUseSize() { return 0; }
    __forceinline unsigned GetMaxUseSize() { return 0; }
    __forceinline unsigned GetCurrentReserveSize() { return 0; }
    __forceinline unsigned GetMaxReserveSize() { return 0; }
    __forceinline void SetMemTrack( bool bTrackMem) { m_bTrackMem = bTrackMem; }

    PAGEARENA * FindArena(void * p);

private:
    CTinyLock lock;             // This is the lock mechanism for thread safety.  
                                
    PAGEARENA * CreateArena(bool isLarge, size_t sz);
    static DWORD ToggleWrite(ProtectedEntityFlagsEnum entity, void * p, size_t sz, DWORD level);

    void * LargeAlloc(size_t sz);
    void LargeFree(void * p, size_t sz);

    PAGEARENA * arenaList;          // List of memory arenas.
    PAGEARENA * arenaLast;          // Last memory arena in list.
    ALLOCHOST * host;               // Alloc host

    bool m_bTrackMem;

    ProtectedEntityFlagsEnum whatIsProtected;
    bool ShouldProtectFreePages() {
        return !!(InstanceWhatIsProtected() & ProtectedEntityFlags::UnusedMemory);
    }

};

extern int current;

class WriteToggler
{
public:
    template<typename T>
    WriteToggler(ProtectedEntityFlagsEnum entity, T & item) 
    {
        this->entity = entity;
        this->addr[0] = &item;
        this->size[0] = sizeof(item);
        onSeparatePages = false;
        this->wasReadOnly [0] = false;
#ifdef DEBUGPROTECT
        this->tsOfMRO[0] = -1;
        this->tsOfMRO[1] = -1;
        this->tsOfMW[0] = -1;
        this->tsOfMW[1] = -1;
#endif
        MakeWriteable();
    }
    template <typename T1, typename T2>
    WriteToggler(ProtectedEntityFlagsEnum entity, T1 & item1, T2 & item2) 
    {
        this->entity = entity;
        this->addr[0] = &item1;
        this->size[0] = sizeof(T1);
        this->addr[1] = &item2;
        this->size[1] = sizeof(T2);
#ifdef DEBUGPROTECT
        this->tsOfMRO[0] = -1;
        this->tsOfMRO[1] = -1;
        this->tsOfMW[0] = -1;
        this->tsOfMW[1] = -1;
#endif
        this->wasReadOnly[0] = this->wasReadOnly[1] = false;
        onSeparatePages = OnSeparatePages();
        MakeWriteable();
    }
    ~WriteToggler()
    {
        MakeReadOnly();
    }
private:
    void MakeReadOnly(int index) {
        if (wasReadOnly[index]) {
            PAGEHEAP::ForbidWrite(entity, addr[index], size[index]);
#ifdef DEBUGPROTECT
            tsOfMRO[index] = current;
#endif
        }
    }
    void MakeWriteable(int index) {
        DWORD oldFlags = PAGEHEAP::AllowWrite(entity, addr[index], size[index]);
#ifdef DEBUGPROTECT
        tsOfMW[index] = current;
#endif
        if (oldFlags == PAGE_NOACCESS) {
            ASSERT(!"about to write to a non-access page.");
            PAGEHEAP::ForbidAccess(entity, addr[index], size[index]);
        } else if (oldFlags == PAGE_READONLY) {
            wasReadOnly[index] = true;
        }
    }
    void MakeReadOnly() {
        MakeReadOnly(0);
        if (onSeparatePages) {
            MakeReadOnly(1);
        }
    }
    void MakeWriteable() {
        MakeWriteable(0);
        if (onSeparatePages) {
            MakeWriteable(1);
        }
    }
    bool OnSeparatePages() 
    {
        size_t page1Start, page2Start, page1End, page2End;
        ComputePagesFromAddress(addr[0], size[0], &page1Start, &page1End);
        ComputePagesFromAddress(addr[1], size[1], &page2Start, &page2End);
        return (page1Start != page2Start || page1End != page2End);
    }
    void ComputePagesFromAddress(void * addr, size_t size, size_t * pageFrom, size_t * pageTo) 
    {
        size_t lowerByFrom = ((size_t)addr) % PAGEHEAP::pageSize;
        size_t lowerByTo = (((size_t)addr) + size) % PAGEHEAP::pageSize;
        *pageFrom = (size_t) addr - lowerByFrom;
        *pageTo = ((size_t) addr + size) - lowerByTo;
    }
    ProtectionToggleLock toggleLock;
    bool wasReadOnly[2];
    bool onSeparatePages;
    void * addr[2];
    size_t  size[2];
    ProtectedEntityFlagsEnum entity;
#ifdef DEBUGPROTECT
    int tsOfMW[2];
    int tsOfMRO[2];
#endif
};

/* No-release allocator. Allocates very fast, cannot
 * be released in general, except in a LIFO fashion
 * to a particular mark.
 */

// Holds a page of memory being used by the NRHEAP.
struct NRPAGE {
    NRPAGE * next;      // next page in use.
    BYTE * firstAvail;  // first available byte for allocation.
    BYTE * limitAvail;  // limit for allocation.
};



class NRMARK {
private:
    friend class NRHEAP;

    NRPAGE * page;      // page.
    BYTE * nextFree;    // first free location within the page.
};

class NRHEAP {
#ifdef DEBUG
    inline size_t DebugSize(size_t sz)
    {
        return RoundUpAllocSize(sizeof(size_t)) + RoundUpAllocSize(sz + 1); // int at beginning for size, plus 1 (or more) bytes of sentinel.
    }
    static const BYTE DEBUGSENTINAL = 0xAE;  // put at end of block to detect overrun.
#endif //DEUBG
        
public:

    class AllowingWrite
    {
    private:
        NRHEAP * heap;
        bool alreadyInAllowingWrite;
    public:
        AllowingWrite(NRHEAP * heap)
        {
            this->heap = heap;
            if (heap) {
                alreadyInAllowingWrite = heap->inAllowingWrite;
                heap->inAllowingWrite = true;
            } else {
                alreadyInAllowingWrite = false;
            }
        }
        ~AllowingWrite()
        {
            if (!alreadyInAllowingWrite && heap) {
                heap->inAllowingWrite = false;
            }
        }
        bool OwnsHeapWriteability()
        {
            return !alreadyInAllowingWrite;
        }
    };

#if defined(__GNUC__) && (__GNUC__ < 3)
    friend class NRHEAP::AllowingWrite;
#endif

    NRHEAP(ALLOCHOST * pHost, bool bTrackMem = false, ProtectedEntityFlagsEnum entity = ProtectedEntityFlags::Other);
    ~NRHEAP();

    //void * operator new(size_t size) { return VSAlloc (size); }
    //void operator delete(void * p) { VSFree (p); }

    void ForbidWrite(void * p, size_t sz) { VerifyHeapEntity(); ForbidWriteInternal(p, sz); }
    void AllowWrite(void * p, size_t sz) { VerifyHeapEntity(); AllowWriteInternal(p, sz); }

    void DisallowReadOnlyDirectives() { VerifyHeapEntity(); DisallowReadOnlyDirectivesInternal(); }
    void AllowReadOnlyDirectives() { VerifyHeapEntity(); AllowReadOnlyDirectivesInternal(); }
    bool AllowingReadOnlyDirectives() { VerifyHeapEntity(); return AllowingReadOnlyDirectivesInternal(); }
    void MakeCurrentPageReadOnly() { VerifyHeapEntity(); MakeCurrentPageReadOnlyInternal(); }// Calling Alloc makes it writeable until the next call to MakeReadOnly
    void MakeCurrentPageWriteable() { VerifyHeapEntity(); MakeCurrentPageWriteableInternal(); }// Calling Alloc makes it writeable until the next call to MakeReadOnly
    void MakeAllHeapReadOnly() { VerifyHeapEntity(); MakeAllHeapReadOnlyInternal(); }
    void MakeAllHeapWriteable() { VerifyHeapEntity(); MakeAllHeapWriteableInternal(); }

    bool IsAddressInCurrentPage(void * addr);
    bool IsAddressInHeap(void * addr)
    {
        for (NRPAGE * page = pageList; page; page = page->next) {
            if (addr >= page && addr < page->limitAvail) return true;
        }
        return false;
    }

#ifdef DEBUG
    void * DebugFill(void * p, size_t sz);
    void * _Alloc(size_t sz, PCSTR pszFile, UINT iLine);
    void * _AllocZero(size_t sz, PCSTR pszFile, UINT iLine);
#else
    void * _Alloc(size_t sz);
    void * _AllocZero(size_t sz);
#endif
    void ValidateHeap();
    PWSTR AllocStr(PCWSTR str);
    void Mark(NRMARK * mark);
    void Free(NRMARK * mark);
    void FreeHeap();
    size_t CalcCommittedSize ();

private:
    void VerifyHeapEntity() {
        ASSERT(entity && L"Heap entity must be set before manipulating heap writeability");
    }
    void ForbidWriteInternal(void * p, size_t sz);
    void AllowWriteInternal(void * p, size_t sz);

    void DisallowReadOnlyDirectivesInternal() { allowReadOnlyDirectives = false; }
    void AllowReadOnlyDirectivesInternal() { allowReadOnlyDirectives = true; }
    bool AllowingReadOnlyDirectivesInternal() {  return allowReadOnlyDirectives; }
    void MakeCurrentPageReadOnlyInternal(); // Calling Alloc makes it writeable until the next call to MakeReadOnly
    void MakeCurrentPageWriteableInternal(); // Calling Alloc makes it writeable until the next call to MakeReadOnly
    void MakeAllHeapReadOnlyInternal();
    void MakeAllHeapWriteableInternal();

    void SetPageWriteStatus(NRPAGE * page, bool writeable);
    void SetPageRegionWriteStatus(NRPAGE * first, NRPAGE * last, bool writeable);
    void SetAllPagesWriteStatus(bool writeable);

    BYTE * nextFree;                // location of free area
    BYTE * limitFree;               // just beyond end of free area in this page.

    NRPAGE * pageList;              // list of pages used by this allocator.
    NRPAGE * pageLast;              // last page in the list.

    ALLOCHOST   * host;             // Host

    void * AllocMore(size_t sz);
    NRPAGE * NewPage(size_t sz);

    ProtectedEntityFlagsEnum entity;
    bool m_bTrackMem;
    bool allowReadOnlyDirectives;
    bool anyPageMarkedReadOnly;
    bool inAllowingWrite;
};

class NRHeapMarker
{
public:
    NRHeapMarker(NRHEAP * h) {
        heap = h;
        heap->Mark(&mark);
    }
    ~NRHeapMarker() {
        heap->Free(&mark);
    }
private:
    NRHEAP * heap;
    NRMARK mark;
};

class NRHeapWriteMaker
{
public:
    NRHeapWriteMaker(NRHEAP * heap) : allowingWrite(heap) 
    {
        this->heap = heap;
        if (heap) {
            heap->MakeAllHeapWriteable();
        }
    }
    ~NRHeapWriteMaker()
    {
        if (heap && allowingWrite.OwnsHeapWriteability()) {
            heap->AllowReadOnlyDirectives();
            heap->MakeAllHeapReadOnly();
        }
    }
private:
    NRHEAP * heap;
    NRHEAP::AllowingWrite allowingWrite;
};

class NRHeapWriteAllower
{
public:
    NRHeapWriteAllower(NRHEAP * heap) : allowingWrite(heap) 
    {
        this->heap = heap;
        if (heap) {
            heap->DisallowReadOnlyDirectives();
        }
    }
    ~NRHeapWriteAllower()
    {
        if (heap && allowingWrite.OwnsHeapWriteability()) {
            heap->AllowReadOnlyDirectives();
            heap->MakeAllHeapReadOnly();
        }
    }
private:
    NRHEAP * heap;
    NRHEAP::AllowingWrite allowingWrite;
};

/*
 * Allocate a new block of memory. This should be VERY FAST
 * and VERY SMALL. The complex case where a new page must
 * be allocated is put into an out of line method.
 */
__forceinline void * NRHEAP::_Alloc(size_t sz
#ifdef DEBUG
, PCSTR pszFile, UINT iLine        // NOTE:  These aren't used here, since NRHEAP's aren't leak-checked (obviously)...
#endif
)
{
    void * p = nextFree;
    if (sz == 0 && p == NULL) sz = 1;

#ifdef DEBUG
    if ((nextFree += DebugSize(sz)) > limitFree)
        return AllocMore(DebugSize(sz));
    else {
        MakeCurrentPageWriteableInternal();
        // fill in with non-zero values so user doesn't expect zeroed memory.
        return DebugFill(p, sz);
    }
#else
    if ((nextFree += RoundUpAllocSize(sz)) > limitFree)
        return AllocMore(sz);
    else {
        MakeCurrentPageWriteableInternal();
        return p;
    }
#endif //DEBUG
}


class NRHEAPWRAPPER : 
        public IHeapAllocator
{
    NRHEAP *heap;

public:    
    NRHEAPWRAPPER() :
      heap(NULL)
    {
    }
    NRHEAPWRAPPER(NRHEAP *h) :
      heap(h)
    {
    }

    ~NRHEAPWRAPPER() 
    {        
    }

    NRHEAP * Init(NRHEAP *h) { heap = h; return h; }
    void Delete() { if (heap) delete heap; }
    NRHEAP *operator->() const
    {
        return heap;
    }
    operator NRHEAP*() const
    {
        return heap;
    }

    void * Allocate(size_t sz) 
    {
        return heap->Alloc(sz);
    }
};


// Abstract string builder base class.
class StringBldr
{
private:
    PWCH m_prgch;
    int m_cch;
    int m_cchAlloc;
    bool m_fError;

    // Don't allow copy constructor or assignment.
    StringBldr(StringBldr & src) { }
    StringBldr & operator=(StringBldr & src);

protected:
#ifdef DEBUG
    void AssertValid()
    {
        if (!m_prgch) {
            ASSERT(!m_cchAlloc && !m_cch);
        }
        else {
            // There should always be room for the null termination.
            ASSERT(0 <= m_cch && m_cch < m_cchAlloc);
        }
    }
#endif

    StringBldr(__out_ecount_opt(cch) PWCH prgch = NULL, int cch = 0)
    {
        ASSERT(!cch == !prgch && cch >= 0);
        m_prgch = prgch;
        m_cch = 0;
        m_cchAlloc = cch;
        m_fError = false;
        DebugOnly(AssertValid());
    }

    // Does all the work of realloc except actually allocating memory and copying data
    // from the old buffer to the new buffer.
    bool Realloc(int cchNeed);

    // Derived classes must implement this.
    virtual PWCH ReallocBuffer(__in_ecount_opt(cchCopy) PWCH prgchOld, int cchCopy, int cchOld, int cchNew) = 0;

    void NullTerminate()
    {
        DebugOnly(AssertValid());
        if (m_prgch)
            m_prgch[m_cch] = 0;
    }

public:
    void Add(WCHAR ch)
    {
        DebugOnly(AssertValid());
        // Always leave room for null termination.
        if (m_cch + 1 < m_cchAlloc || Realloc(1))
            m_prgch[m_cch++] = ch;
        DebugOnly(AssertValid());
    }

    void Add(PCWSTR psz)
    {
        DebugOnly(AssertValid());
        if (psz)
            Add(psz, (int)wcslen(psz));
    }

    void Add(__in_ecount(cch) PCWCH prgch, int cch)
    {
        DebugOnly(AssertValid());
        ASSERT(cch >= 0);
        // Always leave room for null termination.
        if (cch > 0 && (SizeAdd(m_cch, cch) < (size_t)m_cchAlloc || Realloc(cch))) {
            memcpy(m_prgch + m_cch, prgch, cch * sizeof(WCHAR));
            m_cch += cch;
        }
        DebugOnly(AssertValid());
    }

    void AddNum(uint u)
    {
        DebugOnly(AssertValid());
        uint uPow;
        for (uPow = 1; u / uPow > 9; uPow *= 10)
            ;
        ASSERT(u / uPow < 10 && (u / uPow > 0 || u == 0 && uPow == 1));
        do {
            ASSERT(u / uPow < 10);
            Add((WCHAR)(L'0' + u / uPow));
            u %= uPow;
            uPow /= 10;
        } while (uPow);
    }

    void AddNum(int n)
    {
        DebugOnly(AssertValid());
        if (n < 0) {
            Add(L'-');
            n = -n;
        }
        AddNum((uint)n);
    }

    PWSTR Str()
    {
        DebugOnly(AssertValid());
        if (!m_prgch)
            return L"";
        // Null terminate on demand.
        m_prgch[m_cch] = 0;
        return m_prgch;
    }

    int Length()
    {
        return m_cch;
    }

    // Transfers ownership to the caller.
    PWSTR Detach()
    {
        DebugOnly(AssertValid());
        PWSTR psz = m_prgch;
        if (psz)
            psz[m_cch] = 0; // Null terminate.
        m_prgch = NULL;
        m_cch = 0;
        m_cchAlloc = 0;
        m_fError = false;
        DebugOnly(AssertValid());
        return psz;
    }

    void Reset()
    {
        DebugOnly(AssertValid());
        m_cch = 0;
        m_fError = false;
    }

    bool Error()
    {
        return m_fError;
    }
};

class StringBldrNrHeap : public StringBldr
{
protected:
    NRHEAP * m_heap;
    virtual PWCH ReallocBuffer(__in_ecount_opt(cchCopy) PWCH prgchOld, int cchCopy, int cchOld, int cchNew);

public:
    StringBldrNrHeap(NRHEAP * heap, __out_ecount_opt(cch) PWCH prgch = NULL, int cch = 0) : StringBldr(prgch, cch)
        { m_heap = heap; }
};

class StringBldrMemHeap : public StringBldr
{
protected:
    MEMHEAP * m_heap;
    virtual PWCH ReallocBuffer(__in_ecount_opt(cchCopy) PWCH prgchOld, int cchCopy, int cchOld, int cchNew);

public:
    StringBldrMemHeap(MEMHEAP * heap)
        { m_heap = heap; }
    ~StringBldrMemHeap()
    {
        PWSTR psz = Detach();
        if (psz)
            m_heap->Free(psz);
    }
};

class StringBldrFixed : public StringBldr
{
protected:
    virtual PWCH ReallocBuffer(__in_ecount_opt(cchCopy) PWCH prgchOld, int cchCopy, int cchOld, int cchNew)
        { return NULL; }

public:
    StringBldrFixed(__out_ecount_opt(cch) PWCH prgch, int cch) : StringBldr(prgch, cch) { }
    ~StringBldrFixed()
        { NullTerminate(); }
};


// Abstract blob builder base class.
class BlobBldr
{
private:
    byte * m_prgb;
    int m_cb;
    int m_cbAlloc;
    bool m_fError;

    // Don't allow copy constructor or assignment.
    BlobBldr(BlobBldr & src) { }
    BlobBldr & operator=(BlobBldr & src) { return src; }

protected:
#ifdef DEBUG
    void AssertValid()
    {
        if (!m_prgb) {
            ASSERT(!m_cbAlloc && !m_cb);
        }
        else {
            ASSERT(0 <= m_cb && m_cb <= m_cbAlloc);
        }
    }
#endif

    BlobBldr(byte * prgb = NULL, int cb = 0)
    {
        ASSERT(!cb == !prgb && cb >= 0);
        m_prgb = prgb;
        m_cb = 0;
        m_cbAlloc = cb;
        m_fError = false;
        DebugOnly(AssertValid());
    }

    // Does all the work of realloc except actually allocating memory and copying data
    // from the old buffer to the new buffer.
    bool Realloc(int cbNeed);

    // Derived classes must implement this.
    virtual byte * ReallocBuffer(__in_ecount_opt(cbCopy) byte * prgbOld, int cbCopy, int cbOld, int cbNew) = 0;

public:
    void Add(byte b)
    {
        DebugOnly(AssertValid());
        // Always leave room for null termination.
        if (m_cb < m_cbAlloc || Realloc(1))
            m_prgb[m_cb++] = b;
        DebugOnly(AssertValid());
    }

    void Add(const void * prgb, int cb)
    {
        DebugOnly(AssertValid());
        ASSERT(cb >= 0);
        if (cb > 0 && (SizeAdd(m_cb, cb) <= (size_t)m_cbAlloc || Realloc(cb))) {
            memcpy(m_prgb + m_cb, prgb, cb);
            m_cb += cb;
        }
        DebugOnly(AssertValid());
    }

    byte * AddBuf(int cb)
    {
        DebugOnly(AssertValid());
        ASSERT(cb >= 0);
        if (cb > 0 && (SizeAdd(m_cb, cb) <= (size_t)m_cbAlloc || Realloc(cb))) {
            byte * prgb = m_prgb + m_cb;
            m_cb += cb;
            DebugOnly(AssertValid());
            return prgb;
        }
        DebugOnly(AssertValid());
        return NULL;
    }

    byte * Buffer()
    {
        return m_prgb;
    }

    int Length()
    {
        return m_cb;
    }

    // Transfers ownership to the caller.
    byte * Detach()
    {
        DebugOnly(AssertValid());
        byte * prgb = m_prgb;
        m_prgb = NULL;
        m_cb = 0;
        m_cbAlloc = 0;
        m_fError = false;
        DebugOnly(AssertValid());
        return prgb;
    }

    void Reset()
    {
        DebugOnly(AssertValid());
        m_cb = 0;
        m_fError = false;
    }

    bool Error()
    {
        return m_fError;
    }
};

class BlobBldrNrHeap : public BlobBldr
{
protected:
    NRHEAP * m_heap;
    virtual byte * ReallocBuffer(__in_ecount_opt(cbCopy) byte * prgbOld, int cbCopy, int cbOld, int cbNew);

public:
    BlobBldrNrHeap(NRHEAP * heap, byte * prgb = NULL, int cb = 0) : BlobBldr(prgb, cb)
        { m_heap = heap; }
};

class BlobBldrMemHeap : public BlobBldr
{
protected:
    MEMHEAP * m_heap;
    virtual byte * ReallocBuffer(__in_ecount_opt(cbCopy) byte * prgbOld, int cbCopy, int cbOld, int cbNew);

public:
    BlobBldrMemHeap(MEMHEAP * heap)
        { m_heap = heap; }
    ~BlobBldrMemHeap()
    {
        byte * prgb = Detach();
        if (prgb)
            m_heap->Free(prgb);
    }
};

class BlobBldrFixed : public BlobBldr
{
protected:
    virtual byte * ReallocBuffer(__in_ecount_opt(cbCopy) byte * prgbOld, int cbCopy, int cbOld, int cbNew)
        { return NULL; }

public:
    BlobBldrFixed(byte * prgb, int cb) : BlobBldr(prgb, cb) { }
};

#endif // __alloc_h__
