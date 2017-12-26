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
// File: alloc.cpp
//
// Handles the memory allocation for the compiler
//
// There are three memory allocators:
//
// MEMHEAP - a pretty standard Alloc/Free/Realloc heap. Has optional
//           leak checking.
// PAGEHEAP - allocates pages from the operation system.
// NRHEAP - "no-release" heap; incrementally allocates from pages
//          allocated from the PAGEHEAP.
// ===========================================================================

#include "stdafx.h"

#define PROTECT

/*
 * We have special allocation rules in the compiler, where everything
 * should be allocated out of a heap associated with the compiler
 * instance we are in. Prevent people from using new/delete or
 * malloc/free.
 *       
 *
 * This is a great idea, but in fact the above rules are not 100% true.
 * There are some allocations which "outlive" compiler instances (the
 * compiler has been made multi-instance since the rules were made)
 * such as allocations from the name table, and the name table itself
 * (among other COM objects whose references are provided to parties
 * outside the compiler, such as source modules, source data objects,
 * etc.)
 *
 * These allocations are now made from the module's default heap, using
 * VSMEM's allocation tracking mechanism for standard leak detection,
 * etc.

PVOID __cdecl operator new(size_t size)
      {
        VSFAIL("Never use the global operator new in the compiler");
        return 0;
      }
PVOID __cdecl operator new(size_t size, PCSTR pszFile, UINT uLine)
      {
        VSFAIL("Never use the global operator new in the compiler");
        return 0;
      }
void  __cdecl operator delete(PVOID pv)
      {
        VSFAIL("Never use the global operator delete in the compiler");
      }
 */


VSDEFINE_SWITCH(gfNRHeapValidation, "C# CodeSense", "Enable NR Heap validation (slow)");

/*
 ************************** MEMHEAP methods ******************************
 */

/* The "MEMHEAP" is a simple heap like the Win32 heaps, but with leak
 * checking and fatal error on out of memory. Just a simple wrapper
 * on top of the VS heap routines.
 */

/* Create a new heap for allocation.
 */

MEMHEAP::MEMHEAP(ALLOCHOST *pHost, bool bTrackMem)
{
    ASSERT(pHost != 0);

    host = pHost;
    m_bTrackMem = bTrackMem;

    heap = GetProcessHeap();

}

/* The heap is being destroyed. Free everything if
 * not done already.
 */
MEMHEAP::~MEMHEAP()
{
    FreeHeap();
}

/*
 * Free everything in the heap. The heap can no longer be used.
 */
void MEMHEAP::FreeHeap(bool checkLeaks)
{

}

/*
 * Allocate from the heap. Memory will NOT be zeroed.
 */
void * MEMHEAP::_Alloc(size_t sz
#ifdef DEBUG
, PCSTR pszFile, UINT iLine
#endif
)
{
    ASSERT(heap != 0);

#ifdef DEBUG
    // Use the actual internal alloc function so we can pass in the file/line values
    void * p = VsDebugAllocInternal(heap, 0, sz, pszFile, iLine, INSTANCE_GLOBAL, NULL);
#else
    void * p = VSHeapAlloc(heap, sz);
#endif
    if (p == 0)
        host->NoMemory();


    return p;
}

/*
 * Allocate from the heap. Memory WILL be zeroed.
 */
void * MEMHEAP::_AllocZero(size_t sz
#ifdef DEBUG
, PCSTR pszFile, UINT iLine
#endif
)
{
    ASSERT(heap != 0);

#ifdef DEBUG
    // Use the actual internal alloc function so we can pass in the file/line values
    void * p = VsDebugAllocInternal(heap, HEAP_ZERO_MEMORY, sz, pszFile, iLine, INSTANCE_GLOBAL, NULL);
#else
    void * p = VSHeapAllocZero(heap, sz);
#endif
    if (p == 0)
        host->NoMemory();


    return p;
}

/*
 * Allocate (duplicate) a wide char string.
 */
PWSTR MEMHEAP::AllocStr(PCWSTR str)
{
    if (str == NULL)
        return NULL;

    size_t str_len = wcslen(str) + 1;
    PWSTR strNew = (PWSTR) Alloc(str_len * sizeof(WCHAR));
    HRESULT hr;
    hr = StringCchCopyW(strNew, str_len, str);
    ASSERT (SUCCEEDED (hr));
    return strNew;
}

/*
 * Realloc a block on the heap. New memory won't be zeroed.
 */
void * MEMHEAP::Realloc(void * p, size_t sz)
{
    ASSERT(heap != 0);
    ASSERT(p); // The release version of VSHeapRealloc doesn't like NULL - even though the debug version does! Grrrr!


    void * newp = VSHeapRealloc(heap, p, sz);
    if (newp == 0)
        host->NoMemory();


    return newp;
}

/*
 * Realloc a block on the heap. New memory WILL be zeroed.
 */
void * MEMHEAP::ReallocZero(void * p, size_t sz)
{
    ASSERT(heap != 0);


    void * newp = VSHeapReallocZero(heap, p, sz);
    if (newp == 0)
        host->NoMemory();


    return newp;
}

/*
 * Free memory from the heap
 */
void MEMHEAP::Free(void * p)
{
    ASSERT(heap != 0);


    VSHeapFree(heap, p);
}


/*
 ************************** PAGEHEAP methods ******************************
 */

/* The biggest trickiness with the page heap is that it is inefficient
 * to allocate single pages from the operating system - NT will allocate
 * only on 64K boundaries, so allocating a 4k page is needlessly inefficient.
 * We use the ability to reserve then commit pages to reserve moderately
 * large chunks of memory (a PAGEARENA), then commit pages in the arena.
 * This also allows us to track pages allocated and detect leaks.
 */

/*
 * static data members
 */
size_t PAGEHEAP::pageSize;         // The system page size.
int PAGEHEAP::pageShift;           // log2 of the page size
bool PAGEHEAP::reliableCommit;     // MEM_COMMIT reliable?

void PAGEHEAP::StaticInit()
{
    // First time through -- get system page size.
    if (!PAGEHEAP::pageSize) {
        // Get the system page size.
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        PAGEHEAP::pageSize = sysinfo.dwPageSize;

        // Determine the page shift.
        int shift = 0;
        size_t size = PAGEHEAP::pageSize;
        while (size != 1) {
            shift += 1;
            size >>= 1;
        }
        PAGEHEAP::pageShift = shift;

        ASSERT((size_t)(1 << PAGEHEAP::pageShift) == PAGEHEAP::pageSize);

        OSVERSIONINFO osvi;
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        BOOL ok = GetVersionEx(&osvi);
        ASSERT(ok);
        reliableCommit = ok && osvi.dwMajorVersion >= 5;

    }

}


/*
 * Constructor.
 */
PAGEHEAP::PAGEHEAP (ALLOCHOST *pHost, bool bTrackMem)
{
    CTinyGate   gate (&lock); // Acquire the lock

    m_bTrackMem = bTrackMem;

    StaticInit();

    arenaList = arenaLast = NULL;

    // Remember the host
    host = pHost;
    this->whatIsProtected = PAGEHEAP::StaticWhatIsProtected();
}

/*
 * Destructor. Free everything.
 */
PAGEHEAP::~PAGEHEAP()
{
    CTinyGate   gate (&lock); // Acquire the lock
    FreeAllPages();
}


#ifdef DEBUGPROTECT
struct ITEM {

    void * a;
    size_t sz;
    void * np;
    size_t nsz;
    int ts;
    DWORD level;
    DWORD origOldLevel;
    DWORD oldLevel;
};

static ITEM items[1000];
int current = 0;
int large = 0;

void dumpItem(void * a, size_t sz, void * np, size_t nsz, DWORD level, DWORD origOld, DWORD oldLevel)
{
    items[current].a = a;
    items[current].sz = sz;
    items[current].np = np;
    items[current].nsz = nsz;
    items[current].level = level;
    items[current].origOldLevel = origOld;
    items[current].oldLevel = oldLevel;
    items[current].ts = large;
    current ++;
    large ++;
    if (current == 1000) current = 0;
}

#endif // DEBUGPROTECT

CTinyLock memoryProtectionLock;

ProtectionToggleLock::ProtectionToggleLock()
{
    locked = memoryProtectionLock.Acquire();
}

ProtectionToggleLock::~ProtectionToggleLock()
{
    if (locked) {
        memoryProtectionLock.Release();
    }
}



DWORD PAGEHEAP::ToggleWrite(ProtectedEntityFlagsEnum entity, void * p, size_t sz, DWORD level)
{
    if (!PAGEHEAP::IsEntityProtected(entity)) return PAGE_READWRITE;

    ProtectionToggleLock toggleLock;

    size_t lowerBy = ((size_t)p) % pageSize;
    size_t nsz = sz + lowerBy;
    nsz = nsz - (nsz % pageSize) + pageSize * ((nsz % pageSize) == 0 ? 0 : 1);
    DWORD oldFlags, origOldFlags;
    void * np = (void*)(((size_t)p) - lowerBy);
#ifdef PROTECT

    BOOL succeeded = VirtualProtect(np, nsz, level, &oldFlags);
    ASSERT(succeeded);

#ifdef DEBUGPROTECT
    if (level == PAGE_READWRITE) {
        BYTE oldValue = *(BYTE*)p;
        *(BYTE*)p = 0;
        *(BYTE*)p = oldValue;
    }
#endif // DEBUGPROTECT

#else // PROTECT
    oldFlags = PAGE_READWRITE;
#endif // !PROTECT
    origOldFlags = oldFlags;
    if (oldFlags & (PAGE_NOACCESS | PAGE_EXECUTE)) {
        oldFlags = PAGE_NOACCESS;
    } else if (oldFlags & (PAGE_READONLY | PAGE_EXECUTE_READ)) {
        oldFlags = PAGE_READONLY;
    }  else {
        ASSERT(oldFlags & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY));
        oldFlags = PAGE_READWRITE;
    }
#ifdef DEBUGPROTECT
    dumpItem(p, sz, np, nsz, level, origOldFlags, oldFlags);
#endif
    return oldFlags;
}

/*
 * Allocate a set of pages from the page heap. Memory is not
 * zero-filled.
 */
void * PAGEHEAP::AllocPages(size_t sz)
{
    CTinyGate   gate (&lock); // Acquire the lock
    
    ASSERT(sz % pageSize == 0 && sz != 0);     // must be page size multiple.


    // Handle very large allocations differently.
    if (sz > BIGALLOC_SIZE) {
        return LargeAlloc(sz);
    }

    // How many pages are being allocated?
    size_t cPages = (sz >> pageShift);
    void * p;
    PAGEARENA * arena;

    // Check each arena in turn for enough contiguous pages.
    for (arena = arenaList; arena != NULL; arena = arena->nextArena)
    {
        if (arena->largeAlloc)
            continue;           // Large allocation arenas are not interesting.

        // Search this area for free pages. First, find a dword that isn't all used.
        // This loop quickly skips (32 at a time) the used pages that will tend to
        // be at the beginning of an arena.
        int dwIndex;
        for (dwIndex = 0; dwIndex < PAGES_PER_ARENA / BITS_DWORD; ++dwIndex) {
            if (arena->used[dwIndex] != 0xFFFFFFFF)
                break;      // not all used.
        }
        if (dwIndex >= PAGES_PER_ARENA / BITS_DWORD)
            continue;       // No free pages in this arena, go to next one.

        // Now scan for pages starting in this dword.
        size_t iPage = dwIndex * BITS_DWORD;

        while (iPage < PAGES_PER_ARENA) {
            size_t c;
            bool allCommitted;

            // Scan to see if cPages pages starting at iPage are not in use.
            // While scanning, remember if they are all committed or not.
            c = cPages;
            allCommitted = true;
            while (c) {
                if (arena->used[iPage >> DWORD_BIT_SHIFT] & (1 << (iPage & DWORD_BIT_MASK)))
                    break;
                if (! (arena->committed[iPage >> DWORD_BIT_SHIFT] & (1 << (iPage & DWORD_BIT_MASK))))
                    allCommitted = false;

                ++iPage;
                --c;
            }

            if (c) {
                // Enough free pages not found. Start looking again at next page.
                ++iPage;
                continue;
            }

            // Success, we found contiguous free pages.

            iPage -= cPages;    // Back up to first page again.
            p = (BYTE *)arena->pages + (iPage << pageShift);    // Calculate address of allocation.

            //  Commit the pages from the OS if needed.
            if (!allCommitted)
            {
                if (VirtualAlloc(p, sz, MEM_COMMIT, PAGE_READWRITE) != p)
                    host->NoMemory ();
            }
            if (!reliableCommit || allCommitted) {
                // On Win9X the above call to VirtualAlloc does not leave the memory writeable
                AllowWrite(ProtectedEntityFlags::UnusedMemory, p, sz);
            }

            // Mark them as in use and committed.
            c = cPages;
            while (c--) {
                ASSERT(! (arena->used[iPage >> DWORD_BIT_SHIFT] & (1 << (iPage & DWORD_BIT_MASK))));

                arena->used     [iPage >> DWORD_BIT_SHIFT] |= (1 << (iPage & DWORD_BIT_MASK));
                arena->committed[iPage >> DWORD_BIT_SHIFT] |= (1 << (iPage & DWORD_BIT_MASK));

                ++iPage;
            }

#ifdef DEBUG
            // Make sure they aren't zero filled.
            memset(p, 0xCC, sz);
#endif //DEBUG

            // Return the address of the allocated pages.
            return p;
        }
    }

    // No arenas have enough free space. Create a new arena and allocate
    // at the beginning of that arena.
    arena = CreateArena(false, PAGES_PER_ARENA * pageSize);
    if (arena == NULL)
        return NULL;

    p = arena->pages;


    // Commit the memory we need.
    if (VirtualAlloc(p, sz, MEM_COMMIT, PAGE_READWRITE) != p)
        host->NoMemory ();

    // Mark the pages as in use and committed.
    size_t c = cPages;
    for (size_t iPage = 0; c > 0; --c, ++iPage) {
        ASSERT(! (arena->used[iPage >> DWORD_BIT_SHIFT] & (1 << (iPage & DWORD_BIT_MASK))));

        arena->used     [iPage >> DWORD_BIT_SHIFT] |= (1 << (iPage & DWORD_BIT_MASK));
        arena->committed[iPage >> DWORD_BIT_SHIFT] |= (1 << (iPage & DWORD_BIT_MASK));
    }

#ifdef DEBUG
    // Make sure they aren't zero filled.
    memset(p, 0xCC, sz);
#endif //DEBUG

    return p;
}

/*
 * Free pages page to the page heap. The size must be the
 * same as when allocated.
 */
void PAGEHEAP::FreePages(ProtectedEntityFlagsEnum entity, void * p, size_t sz)
{
    CTinyGate   gate (&lock); // Acquire the lock
    
    ASSERT(sz % pageSize == 0 && sz != 0);     // must be page size multiple.

    
    // Handle very large allocations differently.
    if (sz > BIGALLOC_SIZE) {
        LargeFree(p, sz);
        return;
    }

    // Find the arena this page is in.
    PAGEARENA * arena = FindArena(p);
    ASSERT(arena);

    // Get page index within this arena, and page count.
    size_t iPage = ((BYTE *)p - (BYTE *)arena->pages) >> pageShift;
    size_t cPages = sz >> pageShift;

    // Pages must be in-use and committed. Set the pages to not-in-use. We could
    // decommit the pages here, but it is more efficient to keep them around
    // committed because we'll probably want them again. To actually decommit
    // them, call PAGEHEAP::DecommitUnusedPages().
    while (cPages--) {
        ASSERT(arena->used[iPage >> DWORD_BIT_SHIFT]      & (1 << (iPage & DWORD_BIT_MASK)));
        ASSERT(arena->committed[iPage >> DWORD_BIT_SHIFT] & (1 << (iPage & DWORD_BIT_MASK)));

        arena->used[iPage >> DWORD_BIT_SHIFT] &= ~(1 << (iPage & DWORD_BIT_MASK));

        ++iPage;
    }

#ifdef DEBUG
    AllowWrite(entity, p, sz);

    // Fill pages with junk to indicated unused.
    memset(p, 0xEE, sz);
#endif //DEBUG

    if (PAGEHEAP::IsEntityProtected(ProtectedEntityFlags::UnusedMemory)) {
        ForbidAccess(ProtectedEntityFlags::UnusedMemory, p, sz);
    } else {
        AllowWrite(entity, p, sz);
    }
}

/*
 * Allocate a very large allocation. An entire arena is allocated for the allocation.
 */
void * PAGEHEAP::LargeAlloc(size_t sz)
{
    // Create an arena for this large allocation.
    PAGEARENA * newArena = CreateArena(true, sz);
    if (newArena == NULL)
        return NULL;

#ifdef DEBUG
    // Make sure they aren't zero filled.
    memset(newArena->pages, 0xCC, sz);
#endif //DEBUG


    return newArena->pages;
}

/*
 * Free a large allocation made via LargeAlloc.
 */
void PAGEHEAP::LargeFree(void * p, size_t sz)
{
    // Find the arena corresponding to this large allocation.
    PAGEARENA * arena = FindArena(p);
    ASSERT(arena && arena->largeAlloc && arena->pages == p && arena->size == sz);


    // Free the pages.
    BOOL b;
    b = VirtualFree(p, 0, MEM_RELEASE); ASSERT(b);

    // Remove the arena from the arena list.
    if (arenaList == arena) {
        arenaList = arena->nextArena;
        if (arenaLast == arena)
            arenaLast = NULL;
    }
    else {
        PAGEARENA * arenaPrev;

        // Find arena just before the one we want to remove
        for (arenaPrev = arenaList; arenaPrev->nextArena != arena; arenaPrev = arenaPrev->nextArena)
            ;

        ASSERT(arenaPrev->nextArena == arena);
        arenaPrev->nextArena = arena->nextArena;
        if (arenaLast == arena)
            arenaLast = arenaPrev;
   }

    // Free the arena structure.
    host->GetStandardHeap()->Free(arena);
}

/*
 * Free everything allocated by the page heap; optionally checking for
 * leak (memory that hasn't been freed via FreePages).
 */
void PAGEHEAP::FreeAllPages(bool checkLeaks)
{
    CTinyGate   gate (&lock); // Acquire the lock
    
    PAGEARENA * arena, *nextArena;

    for (arena = arenaList; arena != NULL; arena = nextArena) {
        nextArena = arena->nextArena;

        // Check arena for leaks, if desired.
        if (checkLeaks) {
            ASSERT(! arena->largeAlloc);        // Large allocation should have been freed by now.

            for (int dwIndex = 0; dwIndex < PAGES_PER_ARENA / BITS_DWORD; ++dwIndex) {
                ASSERT(arena->used[dwIndex] == 0);  // All pages in this arena should be free.
            }
        }


        // Free the pages in the arena.
        BOOL b;
        b = VirtualFree(arena->pages, 0, MEM_RELEASE);            ASSERT(b);

        // Free the arena structure.
        host->GetStandardHeap()->Free(arena);
    }



    arenaList = arenaLast = NULL;
}

/*
 * Decommit any pages that aren't in use. Decommits memory that
 * can be profitably be other parts of the system. 
 */
void PAGEHEAP::DecommitUnusedPages()
{
    CTinyGate   gate (&lock); // Acquire the lock
    
    PAGEARENA * arena;
    BOOL b;
    
    for (arena = arenaList; arena != NULL; arena = arena->nextArena) {
        if (arena->largeAlloc)
            continue;

        for (int dwIndex = 0; dwIndex < PAGES_PER_ARENA / BITS_DWORD; ++dwIndex) {
            // Can we decommit 32 pages at once with one OS call?
            if (arena->used[dwIndex] == 0 && arena->committed[dwIndex] != 0) {
#if _MSC_VER > 1310
#pragma warning (disable: 6250)
#endif
                b = VirtualFree((BYTE *)arena->pages + ((dwIndex * BITS_DWORD) << pageShift),
                                BITS_DWORD << pageShift,
                                MEM_DECOMMIT);
#if _MSC_VER > 1310
#pragma warning (default: 6250)
#endif
                ASSERT(b);
                if (b)
                    arena->committed[dwIndex] = 0;
            }
            else if (arena->used[dwIndex] != arena->committed[dwIndex]) {
                // Some pages in this group should be decommitted. Check each one individually.
                for (int iPage = dwIndex * BITS_DWORD; iPage < (dwIndex + 1) * BITS_DWORD; ++iPage) {
                    if ( ! (arena->used[iPage >> DWORD_BIT_SHIFT] & (1 << (iPage & DWORD_BIT_MASK))) &&
                           (arena->committed[iPage >> DWORD_BIT_SHIFT] & (1 << (iPage & DWORD_BIT_MASK))))
                    {
#if _MSC_VER > 1310
#pragma warning (disable: 6250)
#endif
                        b = VirtualFree((BYTE *)arena->pages + (iPage << pageShift), pageSize, MEM_DECOMMIT);
#if _MSC_VER > 1310
#pragma warning (default: 6250)
#endif
                        ASSERT(b);
                        if (b)
                            arena->committed[iPage >> DWORD_BIT_SHIFT] &= ~(1 << (iPage & DWORD_BIT_MASK));
                    }
                }
            }
        }
        
#ifdef DEBUG        
        // At this point, the only committed pages in this arena should be in use.
        for (int dwIndex = 0; dwIndex < PAGES_PER_ARENA / BITS_DWORD; ++dwIndex) {
            ASSERT(arena->used[dwIndex] == arena->committed[dwIndex]);
        }
#endif //DEBUG        
    }
}

/*
 * Create a new memory arena of a size and reserve or commit pages for it.
 * If isLarge is true, set this as a "large allocation" arena and commit
 * the memory. If not just reserve the memory.
 */
PAGEHEAP::PAGEARENA * PAGEHEAP::CreateArena(bool isLarge, size_t sz)
{
    PAGEARENA * newArena;

    // Allocate an arena and reserve pages for it.
    newArena = (PAGEARENA *) host->GetStandardHeap()->AllocZero(sizeof(PAGEARENA));
    newArena->pages = VirtualAlloc(0, sz, isLarge ? MEM_COMMIT : MEM_RESERVE, PAGE_READWRITE);
    if (!newArena->pages)
        host->NoMemory ();

    newArena->size = sz;
    newArena->largeAlloc = isLarge;

    // Add to arena list. For efficiency, large allocation arenas are placed
    // at the end, but regular arenas at the beginning. This ensures that
    // regular allocation are almost always satisfied by the first arena in the list.
    if (!arenaList) {
        arenaList = arenaLast = newArena;
    }
    else if (isLarge) {
        // Add to end of arena list.
        arenaLast->nextArena = newArena;
        arenaLast = newArena;
    }
    else {
        // Add to front of arena list
        newArena->nextArena = arenaList;
        arenaList = newArena;
    }

    return newArena;
}

/*
 * Find an arena that contains a particular pointer.
 */
PAGEHEAP::PAGEARENA * PAGEHEAP::FindArena(void * p)
{
    PAGEARENA * arena;

    for (arena = arenaList; arena != NULL; arena = arena->nextArena) {
        if (arena->OwnsPage(p))
            return arena;
    }

    ASSERT(0);      // Should find the arena.
    return NULL;
}



/*
 ************************** NRHEAP methods ******************************
 */

/* The "no-release" heap works by incrementally allocating from a page
 * of memory that was gotten from the page allocator. An allocation operation
 * is just an increment of the current pointer, and a range check to make
 * sure that we haven't gone beyond the end of the page. If we do go beyond
 * the end of the page, we just allocate a new page. All the pages
 * are linked together.
 */

/*
 * Constructor.
 */
NRHEAP::NRHEAP(ALLOCHOST * pHost, bool bTrackMem, ProtectedEntityFlagsEnum entity)
{
    host = pHost;
    nextFree = limitFree = NULL;
    pageList = pageLast = NULL;
    m_bTrackMem = bTrackMem;
    allowReadOnlyDirectives = false;
    anyPageMarkedReadOnly = false;
    inAllowingWrite = false;
    this->entity = entity;
}

/*
 * Destructor
 */
NRHEAP::~NRHEAP()
{
    FreeHeap();
}

/*
 * Allocate zeroed memory.
 */
void * NRHEAP::_AllocZero(size_t sz
#ifdef DEBUG
, PCSTR pszFile, UINT iLine    // Again, these aren't used
#endif
)
{
    void * p = _Alloc(sz
#ifdef DEBUG
                      , pszFile, iLine
#endif
    );
    memset(p, 0, sz);  // zero the memory.
    return p;
}

/*
 * Allocate (duplicate) a wide char string.
 */
PWSTR NRHEAP::AllocStr(PCWSTR str)
{
    if (str == NULL)
        return NULL;

    size_t str_len = wcslen(str) + 1;
    PWSTR strNew = (PWSTR) Alloc(str_len * sizeof(WCHAR));
    HRESULT hr;
    hr =StringCchCopyW (strNew, str_len, str);
    ASSERT (SUCCEEDED (hr));
    return strNew;
}


void NRHEAP::ForbidWriteInternal(void * p, size_t sz)
{
    ASSERT (IsAddressInHeap (p));
    PAGEHEAP::ForbidWrite (entity, p, sz);
}

void NRHEAP::AllowWriteInternal(void * p, size_t sz)
{
    ASSERT (IsAddressInHeap (p));
    PAGEHEAP::AllowWrite (entity, p, sz);
}

void NRHEAP::SetPageWriteStatus(NRPAGE * page, bool writeable)
{
    SetPageRegionWriteStatus(page, page, writeable);
}

void NRHEAP::SetPageRegionWriteStatus(NRPAGE * first, NRPAGE * last, bool writeable)
{
    size_t end = last->limitAvail - ((BYTE*)first);
    if (writeable) {
        AllowWriteInternal(first, end);
    } else {
        anyPageMarkedReadOnly = true;
        ForbidWriteInternal(first, end);
    }
}

bool NRHEAP::IsAddressInCurrentPage(void * addr)
{
    if (!pageList) return false;
    return (addr >= pageLast && addr < pageLast->limitAvail);
}

void NRHEAP::SetAllPagesWriteStatus(bool writeable)
{
    if (!PAGEHEAP::IsEntityProtected(this->entity)) {
        // if the entity type for this heap is not protected,
        // short-circuit before iterating pages
        return;
    }

    for (NRPAGE * first = pageList; first; first = first->next) {
        NRPAGE * end = first;
        PAGEHEAP::PAGEARENA * arena = NULL;
        for (NRPAGE * last = end->next; last; last = last->next)
        {
            if (end->limitAvail == (BYTE*)last) {
                if (arena == NULL) {
                    arena = host->GetPageHeap()->FindArena(first);
                } 
                if (arena->OwnsPage(last)) {
                    end = last;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        SetPageRegionWriteStatus(first, end, writeable);
        first = end;
    }
}

void NRHEAP::MakeCurrentPageReadOnlyInternal()
{
    if (pageLast && allowReadOnlyDirectives) {
        SetPageWriteStatus(pageLast, false);
    }
}


void NRHEAP::MakeCurrentPageWriteableInternal()
{
    if (anyPageMarkedReadOnly && pageLast) {
        SetPageWriteStatus(pageLast, true);
    }
}


void NRHEAP::MakeAllHeapWriteableInternal()
{
    SetAllPagesWriteStatus(true);
    anyPageMarkedReadOnly = false;
}


void NRHEAP::MakeAllHeapReadOnlyInternal()
{
    if (!allowReadOnlyDirectives) return;
    SetAllPagesWriteStatus(false);
}

/*
 * An allocation request has overflowed the current page.
 * allocate a new page and try again.
 */
void * NRHEAP::AllocMore(size_t sz)
{
    sz = RoundUpAllocSize(sz);   // round to appropriate byte boundary.
    NRPAGE * newPage = NewPage(sz);
    if (!newPage)
        host->NoMemory();

    ASSERT(newPage->next == NULL);

    // Set all the memory in this page as available for alloc.
    nextFree = newPage->firstAvail;
    limitFree = newPage->limitAvail;

    // Link the page in to the list.
    if (pageLast) {
        MakeCurrentPageWriteableInternal();
        pageLast->next = newPage;
        MakeCurrentPageReadOnlyInternal();
        pageLast = newPage;
    }
    else {
        // First page.
        pageList = pageLast = newPage;
    }

    // We must now be able to allocate the memory.
    return Alloc(sz);
}

/*
 * Allocate a new page of memory, with enough size
 * to handle a block of size sz.
 */
NRPAGE * NRHEAP::NewPage(size_t sz)
{
    size_t pageSize = PAGEHEAP::pageSize; // Page size.
    size_t allocSize;
    NRPAGE * newPage;

    allocSize = RoundUp(sz + sizeof(NRPAGE), pageSize);

    // Allocate the new page.
    newPage = (NRPAGE *) host->GetPageHeap()->AllocPages(allocSize);

    if (newPage) {
        // Initialize the new page.
        newPage->next = NULL;
        newPage->firstAvail = (BYTE *)newPage + RoundUpAllocSize(sizeof(NRPAGE));
        newPage->limitAvail = ((BYTE *)newPage) + allocSize;

        // We should have enough room in the new page!
        ASSERT(newPage->limitAvail > newPage->firstAvail);
        ASSERT((unsigned)(newPage->limitAvail - newPage->firstAvail) >= sz);
    }

    return newPage;
}

/*
 * Return a mark of the current allocation state, so you
 * can free all memory allocated subsequent.
 */
void NRHEAP::Mark(NRMARK * mark)
{
    // Record current page and location.
    mark->page = pageLast;
    mark->nextFree = nextFree;
}

size_t NRHEAP::CalcCommittedSize ()
{
    size_t    iCommit = 0;
    for (NRPAGE *p = pageList; p; p = p->next)
        iCommit += p->limitAvail - (BYTE *)p;

    return iCommit;
}

/*
 * Free all memory allocated after the mark.
 */
void NRHEAP::Free(NRMARK * mark)
{
    if (mark->page == NULL) {
        // Mark was before anything was allocated.
        FreeHeap();
    }
    else {
        NRPAGE * page, *nextPage;

#ifdef DEBUG
        page = pageList;
        while (page != pageLast) {
            if (page == mark->page)
                break;
            page = page->next;
        }
        ASSERT(mark->page == page); // we should have found the page in the list
#endif

        ValidateHeap();
    
        // Free all pages after the new last one.
        for (page = mark->page->next; page != NULL; page = nextPage)
        {
            nextPage = page->next;
            host->GetPageHeap()->FreePages(entity, page, (BYTE *)page->limitAvail - (BYTE *)page);
        }

        // Reset the last page and location.
        pageLast = mark->page;
        if (anyPageMarkedReadOnly) {
            SetPageWriteStatus(pageLast, true);
        }
        pageLast->next = NULL;
        nextFree = mark->nextFree;
        limitFree = pageLast->limitAvail;

#ifdef DEBUG
        // Free rest of page with junk
        memset(nextFree, 0xEE, limitFree - nextFree);
#endif //DEBUG
        MakeCurrentPageReadOnlyInternal();
    }
}

/*
 * Free the entire heap.
 */
void NRHEAP::FreeHeap()
{
    NRPAGE * page, *nextPage;
 
    ValidateHeap();
    
    // Free all the pages.
    for (page = pageList; page != NULL; page = nextPage) {
        nextPage = page->next;
        host->GetPageHeap()->FreePages(entity, page, (BYTE *)page->limitAvail - (BYTE *)page);
    }

    // Reset the allocator.
    nextFree = limitFree = NULL;
    pageList = pageLast = NULL;
}

#ifdef DEBUG
/*
 * For DEBUG allocations, add information to an allocation to validate 
 * no buffer overruns.
 */
void * NRHEAP::DebugFill(void * p, size_t sz)
{
    size_t sizeUser = sz;                   // user space
    
    if (p == NULL)
        return NULL;

    * (size_t *) p = sizeUser;  // record user size at beginning of block for traversal.
    p = (BYTE*)p + RoundUpAllocSize(sizeof(size_t));
    
    // Put sentinal bytes in.
    for (size_t i = sizeUser; i < RoundUpAllocSize(sizeUser + 1); ++i)
        * ((BYTE *)p + i) = DEBUGSENTINAL;
        
    return p;
}    
#endif //DEBUG


/* 
 * Validate that all the sentinal bytes in the heap 
 * are still intact.
 */
void NRHEAP::ValidateHeap()
{
#ifdef DEBUG
    if (!VSFSWITCH(gfNRHeapValidation))
        return;

    NRPAGE * page;
    
    for (page = pageList; page != NULL; page = page->next) {
        // Validate this page.
        BYTE * p = page->firstAvail;
        for (;;) {
            if (p >= page->limitAvail)
                break;
            size_t sizeUser = *(size_t *)p;
            if (sizeUser == (size_t)I64(0xCCCCCCCCCCCCCCCC) || sizeUser == (size_t)I64(0xEEEEEEEEEEEEEEEE))
                break;   // at the end of this page.

            p = (BYTE*)p + RoundUpAllocSize(sizeof(size_t));
            for (size_t i = sizeUser; i < RoundUpAllocSize(sizeUser + 1); ++i) {
                ASSERT(p[i] == DEBUGSENTINAL);
            }
            p += RoundUpAllocSize(sizeUser + 1);
        }
    }
#endif //DEBUG
}

bool StringBldr::Realloc(int cchNeed)
{
    DebugOnly(AssertValid());
    ASSERT(cchNeed > 0 && cchNeed + m_cch >= m_cchAlloc);

    if (m_fError)
        return false;

    // Leave room for future expansion and add an extra char for NULL termination.
    cchNeed += cchNeed + 1;

    int cchNew;

    if (m_prgch) {
        cchNew = m_cch + cchNeed;

        // Better not overflow!
        ASSERT(cchNew > m_cchAlloc);

        if (cchNew < m_cchAlloc * 2)
            cchNew = m_cchAlloc * 2;
    }
    else {
        cchNew = cchNeed;
    }
    if (cchNew < 128)
        cchNew = 128;

    PWCH prgch = ReallocBuffer(m_prgch, m_cch, m_cchAlloc, cchNew);
    if (!prgch) {
        m_fError = true;
        return false;
    }

    m_prgch = prgch;
    m_cchAlloc = cchNew;

    return true;
}

PWCH StringBldrNrHeap::ReallocBuffer(__in_ecount_opt(cchCopy) PWCH prgchOld, int cchCopy, int cchOld, int cchNew)
{
    PWCH prgch = (PWCH)m_heap->Alloc(SizeMul(cchNew,sizeof(WCHAR)));
    if (cchCopy > 0)
        memcpy(prgch, prgchOld, cchCopy * sizeof(WCHAR));
    return prgch;
}

PWCH StringBldrMemHeap::ReallocBuffer(__in_ecount_opt(cchCopy) PWCH prgchOld, int cchCopy, int cchOld, int cchNew)
{
    if (!prgchOld) {
        ASSERT(!cchOld && !cchCopy);
        return (PWCH)m_heap->Alloc(SizeMul(cchNew, sizeof(WCHAR)));
    }
    return (PWCH)m_heap->Realloc(prgchOld, cchNew * sizeof(WCHAR));
}

bool BlobBldr::Realloc(int cbNeed)
{
    DebugOnly(AssertValid());
    ASSERT(cbNeed > 0 && cbNeed + m_cb > m_cbAlloc);

    if (m_fError)
        return false;

    // Leave room for future expansion.
    cbNeed += cbNeed;

    int cbNew;

    if (m_prgb) {
        cbNew = m_cb + cbNeed;

        // Better not overflow!
        ASSERT(cbNew > m_cbAlloc);

        // At least double the size.
        if (cbNew < m_cbAlloc * 2)
            cbNew = m_cbAlloc * 2;
    }
    else {
        cbNew = cbNeed;
    }
    if (cbNew < 256)
        cbNew = 256;

    byte * prgb = ReallocBuffer(m_prgb, m_cb, m_cbAlloc, cbNew);
    if (!prgb) {
        m_fError = true;
        return false;
    }

    m_prgb = prgb;
    m_cbAlloc = cbNew;

    return true;
}

byte * BlobBldrNrHeap::ReallocBuffer(__in_ecount_opt(cbCopy) byte * prgbOld, int cbCopy, int cbOld, int cbNew)
{
    byte * prgb = (byte *)m_heap->Alloc(cbNew);
    if (cbCopy > 0)
        memcpy(prgb, prgbOld, cbCopy);
    return prgb;
}

byte * BlobBldrMemHeap::ReallocBuffer(__in_ecount_opt(cbCopy) byte * prgbOld, int cbCopy, int cbOld, int cbNew)
{
    if (!prgbOld) {
        ASSERT(!cbOld && !cbCopy);
        return (byte *)m_heap->Alloc(cbNew);
    }
    return (byte *)m_heap->Realloc(prgbOld, cbNew);
}
