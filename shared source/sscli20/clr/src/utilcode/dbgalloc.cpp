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
// DbgAlloc.cpp
//
//  Routines layered on top of allocation primitives to provide debugging
//  support.
//

#include "stdafx.h"
#include "unsafe.h"
#include "utilcode.h"
#include "dbgalloc.h"
#define LOGGING
#include "log.h"


#ifdef _DEBUG


// We used a zero sized array, disable the non-standard extension warning.
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4200)
#endif

// Various patterns written into packet headers and bodies.
#define MAX_CLASSNAME_LENGTH    1024
#define CDA_ALLOC_PATTERN   0xaa        // Pattern to fill newly allocated packets with
#define CDA_DEALLOC_PATTERN 0xdd        // Pattern to fill deallocated packets with
#define CDA_GUARD_PATTERN   ((unsigned char) 0xff)  // Guard pattern written after user data
#define CDA_MAGIC_1         0x12345678  // Special tag value at start of alloc header
#define CDA_MAGIC_2         0x87654321  // Special tag value at end of alloc header
#define CDA_INV_PATTERN     0xeeeeeeee  // Used to overwrite tag values of deallocation


// Number of entries in a table of the top allocators (by allocation operations)
// that are shown when statistics are reported. This is not the same as the
// number of entries in the table itself (which is dynamically sized and
// unbounded).
#define CDA_TOP_ALLOCATORS  10


// An entry in the top allocators table.
struct DbgAllocTop {
    void           *m_EIP;              // Allocator's EIP
    unsigned        m_Count;            // Number of allocations made so far
    __int64         m_TotalBytes;       // Cumulative total of bytes allocated
};


// Allocation header prepended to allocated memory. This structure varies in
// size (due to allocator/deallocator call stack information that is sized at
// initialization time).
struct DbgAllocHeader {
    unsigned        m_Magic1;           // Tag value used to check for corruption
    unsigned        m_SN;               // Sequence number assigned at allocation time
    BOOL            m_IsArray;          // Allocated by new []
    unsigned        m_Length;           // Length of user data in packet
    DbgAllocHeader *m_Next;             // Next packet in chain of live allocations
    DbgAllocHeader *m_Prev;             // Previous packet in chain of live allocations
    HMODULE         m_hmod;             // hmod of allocator
    void           *m_AllocStack[1];    // Call stack of allocator
    void           *m_DeallocStack[1];  // Call stack of deallocator
    unsigned        m_Magic2;           // Tag value used to check for corruption
    char            m_Data[];           // Start of user data
};

// Number of guard bytes allocated after user data.
#define CDA_GUARD_BYTES     (offsetof(DbgAllocHeader,m_AllocStack)-offsetof(DbgAllocHeader,m_Length))
#define CDA_OPT_GUARD_BYTES (g_AllocGuard ? CDA_GUARD_BYTES : 0)


// Macros to aid in locating fields in/after the variably sized section.
#define CDA_ALLOC_STACK(_h, _n) ((_h)->m_AllocStack[_n])
#define CDA_DEALLOC_STACK(_h, _n) CDA_ALLOC_STACK((_h), g_CallStackDepth + (_n))
#define CDA_MAGIC2(_h) ((unsigned*)&CDA_DEALLOC_STACK(_h, g_CallStackDepth))
#define CDA_DATA(_h, _n)    (((unsigned char *)(CDA_MAGIC2(_h) + 1))[_n])
#define CDA_HEADER_TO_DATA(_h) (&CDA_DATA(_h, 0))
#define CDA_DATA_TO_HEADER(_d) ((DbgAllocHeader *)((unsigned char *)(_d) - CDA_HEADER_TO_DATA((DbgAllocHeader*)0)))
#define CDA_SIZEOF_HEADER() ((unsigned)(UINT_PTR)CDA_HEADER_TO_DATA((DbgAllocHeader*)0))


// Various global allocation statistics.
struct DbgAllocStats {
    __int64         m_Allocs;           // Number of calls to DbgAlloc
    __int64         m_AllocFailures;    // Number of above calls that failed
    __int64         m_ZeroAllocs;       // Number of above calls that asked for zero bytes
    __int64         m_Frees;            // Number of calls to DbgFree
    __int64         m_NullFrees;        // Number of above calls that passed a NULL pointer
    __int64         m_AllocBytes;       // Total number of bytes ever allocated
    __int64         m_FreeBytes;        // Total number of bytes ever freed
    __int64         m_MaxAlloc;         // Largest number of bytes ever allocated simultaneously
};


// Global debugging cells.
bool                g_HeapInitialized = false;
LONG                g_HeapInitializing = 0;
// We can leave this as OS CriticalSection.  It is very difficult to move it to be host aware.
// While having this lock, the thread is updating some tracking data without a possibility of causing
// fiber switching.
CRITICAL_SECTION    g_AllocMutex;
DbgAllocStats       g_AllocStats;
unsigned            g_NextSN;
DbgAllocHeader     *g_AllocListFirst;
DbgAllocHeader     *g_AllocListLast;
DbgAllocHeader    **g_AllocFreeQueue;       // Don't free memory right away, to allow poisoning to work
unsigned            g_FreeQueueSize;
unsigned            g_AllocFreeQueueCur;
HANDLE              g_HeapHandle;
unsigned            g_PageSize;
DbgAllocTop        *g_TopAllocators;
unsigned            g_TopAllocatorsSlots;
bool                g_DbgEnabled;
bool                g_ConstantRecheck;
bool                g_PoisonPackets;
bool                g_AllocGuard;
bool                g_LogDist;
bool                g_LogStats;
bool                g_DetectLeaks;
bool                g_AssertOnLeaks;
bool                g_BreakOnAlloc;
unsigned            g_BreakOnAllocNumber;
bool                g_UsePrivateHeap;
bool                g_ValidateHeap;
bool                g_PagePerAlloc;
bool                g_UsageByAllocator;
bool                g_DisplayLockInfo;
unsigned            g_CallStackDepth;

// Macros to manipulate stats (these are all called with a mutex held).
#define CDA_STATS_CLEAR() memset(&g_AllocStats, 0, sizeof(g_AllocStats))
#define CDA_STATS_INC(_stat) g_AllocStats.m_##_stat++
#define CDA_STATS_ADD(_stat, _n) g_AllocStats.m_##_stat += (_n)


// Mutex macros.
#define CDA_LOCK()   UnsafeEnterCriticalSection(&g_AllocMutex)
#define CDA_UNLOCK() UnsafeLeaveCriticalSection(&g_AllocMutex);


// Forward routines.
void DbgAllocInit();


// The number and size range of allocation size distribution buckets we keep.
#define CDA_DIST_BUCKETS        16
#define CDA_DIST_BUCKET_SIZE    16
#define CDA_MAX_DIST_SIZE       ((CDA_DIST_BUCKETS * CDA_DIST_BUCKET_SIZE) - 1)

// The buckets themselves (plus a variable to capture the number of allocations
// that wouldn't fit into the largest bucket).
unsigned g_AllocBuckets[CDA_DIST_BUCKETS];
unsigned g_LargeAllocs;


// Routine to check that an allocation header looks valid. Asserts on failure.
void DbgValidateHeader(DbgAllocHeader *h)
{
    STATIC_CONTRACT_LEAF;

    _ASSERTE((h->m_Magic1 == CDA_MAGIC_1) &&
             (*CDA_MAGIC2(h) == CDA_MAGIC_2) &&
             ((unsigned)(UINT_PTR)h->m_Next != CDA_INV_PATTERN) &&
             ((unsigned)(UINT_PTR)h->m_Prev != CDA_INV_PATTERN));
    if (g_AllocGuard)
        for (unsigned i = 0; i < CDA_GUARD_BYTES; i++)
            _ASSERTE(CDA_DATA(h, h->m_Length + i) == CDA_GUARD_PATTERN);
}


// Routine to check all active packets to see if they still look valid.
// Optionally, also check that the non-NULL address passed does not lie within
// any of the currently allocated packets.
void DbgValidateActivePackets(void *Start, void *End)
{
    STATIC_CONTRACT_LEAF;

    DbgAllocHeader *h = g_AllocListFirst;

    while (h) {
        DbgValidateHeader(h);
        if (Start) {
            void *head = (void *)h;
            void *tail = (void *)&CDA_DATA(h, h->m_Length + CDA_OPT_GUARD_BYTES);
            _ASSERTE((End <= head) || (Start >= tail));
        }
        h = h->m_Next;
    }

}



// Called to free resources allocated by DbgInitSymbols.
void DbgUnloadSymbols()
{
    STATIC_CONTRACT_NOTHROW;

}



// Transform an address into a string of the form '(symbol + offset)' if
// possible. Note that the string returned is statically allocated, so don't
// make a second call to this routine until you've finsihed with the results of
// this call.
char *DbgSymbolize(void *Address)
{
    STATIC_CONTRACT_NOTHROW;

    return "";
}


DWORD DbgAllocReadRegistry(__in_z char *Name)
{
    STATIC_CONTRACT_WRAPPER;

    // REGUTIL::GetConfigDWORD does not allocate memory for FEATURE_PAL
    WCHAR wName[50];
    _snwprintf(wName, 50, L"%S", Name);
    return REGUTIL::GetConfigDWORD(wName, 0);
}

// Called to initialise the allocation subsystem (the first time it's used).
void DbgAllocInit()
{
    STATIC_CONTRACT_NOTHROW;

 retry:

    // Try to get the exclusive right to initialize.
    if (InterlockedExchange(&g_HeapInitializing, 1) == 0) {

        // We're now in a critical section. Check whether the subsystem was
        // initialized in the meantime.
        if (g_HeapInitialized) {
            g_HeapInitializing = 0;
            return;
        }

        // Nobody beat us to it. Initialize the subsystem now (other potential
        // initializors are spinning on g_HeapInitializing).

        // Create the mutex used to synchronize all heap debugging operations.
        UnsafeInitializeCriticalSection(&g_AllocMutex);

        // Reset statistics.
        CDA_STATS_CLEAR();

        // Reset allocation size distribution buckets.
        memset (&g_AllocBuckets, 0, sizeof(g_AllocBuckets));
        g_LargeAllocs = 0;

        // Initialize the global serial number count. This is stamped into newly
        // allocated packet headers and then incremented as a way of uniquely
        // identifying allocations.
        g_NextSN = 1;

        // Initialize the pointers to the first and last packets in a chain of
        // live allocations (used to track all leaked packets at the end of a
        // run).
        g_AllocListFirst = NULL;
        g_AllocListLast = NULL;


        // See if we should be logging locking stuff
        g_DisplayLockInfo = DbgAllocReadRegistry("DisplayLockInfo") != 0;

        // Get setup from registry.
        g_DbgEnabled = DbgAllocReadRegistry("AllocDebug") != 0;
        if (g_DbgEnabled) {
            g_ConstantRecheck = DbgAllocReadRegistry("AllocRecheck") != 0;
            g_AllocGuard = DbgAllocReadRegistry("AllocGuard") != 0;
            g_PoisonPackets = DbgAllocReadRegistry("AllocPoison") != 0;
            g_FreeQueueSize = DbgAllocReadRegistry("AllocFreeQueueSize") != 0;
            g_LogDist = DbgAllocReadRegistry("AllocDist") != 0;
            g_LogStats = DbgAllocReadRegistry("AllocStats") != 0;
            g_DetectLeaks = DbgAllocReadRegistry("AllocLeakDetect") != 0;
            g_AssertOnLeaks = 0;
            g_BreakOnAlloc = DbgAllocReadRegistry("AllocBreakOnAllocEnable") != 0;
            g_BreakOnAllocNumber = DbgAllocReadRegistry("AllocBreakOnAllocNumber");
            g_UsePrivateHeap = DbgAllocReadRegistry("AllocUsePrivateHeap") != 0;
            g_ValidateHeap = DbgAllocReadRegistry("AllocValidateHeap") != 0;
            g_PagePerAlloc = DbgAllocReadRegistry("AllocPagePerAlloc") != 0;
            g_UsageByAllocator = DbgAllocReadRegistry("UsageByAllocator") != 0;

        }


        // Page per alloc mode isn't compatible with some heap functions and
        // guard bytes don't make any sense.
        if (g_PagePerAlloc) {
            g_UsePrivateHeap = false;
            g_ValidateHeap = false;
            g_AllocGuard = false;
        }

            g_HeapHandle = ClrGetProcessHeap();

        // Get the system page size.
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        g_PageSize = sysinfo.dwPageSize;

        // If we have been asked to record the top allocators, initialize the
        // table to its empty state.
        if (g_UsageByAllocator) {
            g_TopAllocators = NULL;
            g_TopAllocatorsSlots = 0;
        }

        if (g_PoisonPackets) {
            if (g_FreeQueueSize == 0)
                g_FreeQueueSize = 8192;     // keep the last 8K free packets around
            g_AllocFreeQueueCur = 0;

            _ASSERTE( !"This is broken because ClrHeapAlloc allocates and this function is not reentrant" );
            g_AllocFreeQueue = (DbgAllocHeader ** )
                ClrHeapAlloc(g_HeapHandle, HEAP_ZERO_MEMORY, sizeof(DbgAllocHeader*)*g_FreeQueueSize);
            _ASSERTE(g_AllocFreeQueue);
        }

        // Initialization complete. Once we reset g_HeapInitializing to 0, any
        // other potential initializors can get in and see they have no work to
        // do.
        g_HeapInitialized = true;
        g_HeapInitializing = 0;
    } else {
        // Someone else is initializing, wait until they finish.
        ClrSleepEx(0, FALSE);
        goto retry;
    }
}


// Called just before process exit to report stats and check for memory
// leakages etc.
void __stdcall DbgAllocReport(__in_z __in_opt char * pString,
                              BOOL fDone,
                              BOOL fDoPrintf,
                              unsigned snapShot)
{
    STATIC_CONTRACT_NOTHROW;

    if (!g_HeapInitialized)
        return;

    CDA_LOCK();

    if (g_LogStats || g_LogDist || g_DetectLeaks || g_UsageByAllocator)
        LOG((LF_DBGALLOC, LL_ALWAYS, "------ Allocation Stats ------\n"));

    // Print out basic statistics.
    if (g_LogStats) {
        LOG((LF_DBGALLOC, LL_ALWAYS, "\n"));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Alloc calls    : %u\n", (int)g_AllocStats.m_Allocs));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Alloc failures : %u\n", (int)g_AllocStats.m_AllocFailures));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Alloc 0s       : %u\n", (int)g_AllocStats.m_ZeroAllocs));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Alloc bytes    : %u\n", (int)g_AllocStats.m_AllocBytes));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Free calls     : %u\n", (int)g_AllocStats.m_Frees));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Free NULLs     : %u\n", (int)g_AllocStats.m_NullFrees));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Free bytes     : %u\n", (int)g_AllocStats.m_FreeBytes));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Leaked allocs  : %u\n", (int)(g_AllocStats.m_Allocs - g_AllocStats.m_AllocFailures) -
             (g_AllocStats.m_Frees - g_AllocStats.m_NullFrees)));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Leaked bytes   : %u\n", (int)g_AllocStats.m_AllocBytes - g_AllocStats.m_FreeBytes));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Max allocation : %u\n", (int)g_AllocStats.m_MaxAlloc));
    }

    // Print out allocation size distribution statistics.
    if (g_LogDist) {
        LOG((LF_DBGALLOC, LL_ALWAYS, "\n"));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Alloc distrib  :\n"));
        for (unsigned i = 0; i < CDA_DIST_BUCKETS; i++)
            LOG((LF_DBGALLOC, LL_ALWAYS, "  [%3u,%3u] : %u\n", i * CDA_DIST_BUCKET_SIZE,
                 (i * CDA_DIST_BUCKET_SIZE) + (CDA_DIST_BUCKET_SIZE - 1),
                 (int)g_AllocBuckets[i]));
        LOG((LF_DBGALLOC, LL_ALWAYS, "  [%3u,---] : %u\n", CDA_MAX_DIST_SIZE + 1, (int)g_LargeAllocs));
    }

    // Print out the table of top allocators. Table is pre-sorted, the first
    // NULL entry indicates the end of the valid list.
    if (g_UsageByAllocator && g_TopAllocators) {
        LOG((LF_DBGALLOC, LL_ALWAYS, "\n"));
        LOG((LF_DBGALLOC, LL_ALWAYS, "Top allocators :\n"));
        for (unsigned i = 0; i < min(CDA_TOP_ALLOCATORS, g_TopAllocatorsSlots); i++) {
            if (g_TopAllocators[i].m_EIP == NULL)
                break;
            LOG((LF_DBGALLOC, LL_ALWAYS, "  %2u: %08X %s\n",
                 i + 1,
                 g_TopAllocators[i].m_EIP,
                 DbgSymbolize(g_TopAllocators[i].m_EIP)));
            LOG((LF_DBGALLOC, LL_ALWAYS, "       %u allocations, %u bytes total, %u bytes average size\n",
                 g_TopAllocators[i].m_Count,
                 (unsigned)g_TopAllocators[i].m_TotalBytes,
                 (unsigned)(g_TopAllocators[i].m_TotalBytes / g_TopAllocators[i].m_Count)));
        }
    }

    // Print out info for all leaked packets.
    if (g_DetectLeaks) {

        DbgAllocHeader *h = g_AllocListFirst;

        // Find first leak after snapshot point
        while (h)
        {
            if (h->m_SN > snapShot)
            {
                break;
            }
            else
            {
                h = h->m_Next;
            }
        }

        int fHaveLeaks = (h!=NULL);

        if (h) {

            // Tell the Log we had memory leaks
            LOG((LF_DBGALLOC, LL_ALWAYS, "\n"));
            LOG((LF_DBGALLOC, LL_ALWAYS, "Detected memory leaks!\n"));
            LOG((LF_DBGALLOC, LL_ALWAYS, "Leaked packets :\n"));

            // Tell the console we had memory leaks
            if (fDoPrintf)
            {
                printf("Detected memory leaks!\n");
                if (pString != NULL)
                    printf("%s\n", pString);

                printf("Leaked packets :\n");
            }
        }

        while (h) {
            if (h->m_SN > snapShot) {
                char buffer1[132];
                char buffer2[32];
                sprintf(buffer1, "#%u %08p:%u ", h->m_SN, CDA_HEADER_TO_DATA(h), (unsigned int) h->m_Length);
                unsigned i;
                for (i = 0; i < 16; i++) {
                    if (i < h->m_Length)
                        sprintf(buffer2, "%02X", (BYTE)CDA_DATA(h, i));
                    else
                        strcpy(buffer2, "  ");
                    if ((i % 4) == 3)
                        strcat_s(buffer2, _countof(buffer2), " ");
                    strcat_s(buffer1, _countof(buffer1), buffer2);
                }
                for (i = 0; i < min(16, h->m_Length); i++) {
                    sprintf(buffer2, "%c", (CDA_DATA(h, i) < 32) || (CDA_DATA(h, i) > 127) ? '.' : CDA_DATA(h, i));
                    strcat_s(buffer1, _countof(buffer1), buffer2);
                }
                LOG((LF_DBGALLOC, LL_ALWAYS, "%s\n", buffer1));
                if (fDoPrintf)
                    printf("%s\n", buffer1);

                if (g_CallStackDepth == 1) {
                    LOG((LF_DBGALLOC, LL_ALWAYS, " Allocated at %08X %s\n",
                         CDA_ALLOC_STACK(h, 0), DbgSymbolize(CDA_ALLOC_STACK(h, 0))));

                if (fDoPrintf)
                    printf(" Allocated at %p %s\n",
                         CDA_ALLOC_STACK(h, 0), DbgSymbolize(CDA_ALLOC_STACK(h, 0)));
                } else {
                    LOG((LF_DBGALLOC, LL_ALWAYS, " Allocation call stack:\n"));
                    if (fDoPrintf)
                        printf(" Allocation call stack:\n");
                    for (unsigned i = 0; i < g_CallStackDepth; i++) {
                        if (CDA_ALLOC_STACK(h, i) == NULL)
                            break;
                        LOG((LF_DBGALLOC, LL_ALWAYS, "  %08X %s\n",
                             CDA_ALLOC_STACK(h, i), DbgSymbolize(CDA_ALLOC_STACK(h, i))));
                        if (fDoPrintf)
                            printf("  %p %s\n",
                                 CDA_ALLOC_STACK(h, i), DbgSymbolize(CDA_ALLOC_STACK(h, i)));
                    }
                }
                wchar_t buf[256];
                GetModuleFileNameW(h->m_hmod, buf, 256);
                LOG((LF_DBGALLOC, LL_ALWAYS, " Base, name: %08X %S\n\n", h->m_hmod, buf));
                if (fDoPrintf)
                    printf(" Base, name: %p %S\n\n", h->m_hmod, buf);
            }

            h = h->m_Next;
        }

        if (fDoPrintf)
            fflush(stdout);

        if (fHaveLeaks && g_AssertOnLeaks)
            _ASSERTE(!"Detected memory leaks!");

    }

    if (g_LogStats || g_LogDist || g_DetectLeaks || g_UsageByAllocator) {
        LOG((LF_DBGALLOC, LL_ALWAYS, "\n"));
        LOG((LF_DBGALLOC, LL_ALWAYS, "------------------------------\n"));
    }

    CDA_UNLOCK();

    if (fDone)
    {
        DbgUnloadSymbols();
        UnsafeDeleteCriticalSection(&g_AllocMutex);
        // We won't be doing any more of our debug allocation stuff
        g_DbgEnabled=0;
    }
}

// return current id
unsigned __stdcall DbgAllocSnapshot()
{
    return g_NextSN;
}

// Allocate a block of memory at least n bytes big.
void * __stdcall DbgAlloc(size_t n, void **ppvCallstack, BOOL isArray)
{
    STATIC_CONTRACT_NOTHROW;

    // Initialize if necessary (DbgAllocInit takes care of the synchronization).
    if (!g_HeapInitialized)
        DbgAllocInit();

    if (!g_DbgEnabled)
        return ClrAllocInProcessHeap(0, n);

    CDA_LOCK();

    // Count calls to this routine and the number that specify 0 bytes of
    // allocation. This needs to be done under the lock since the counters
    // themselves aren't synchronized.
    CDA_STATS_INC(Allocs);
    if (n == 0)
        CDA_STATS_INC(ZeroAllocs);

    CDA_UNLOCK();

    // Allocate enough memory for the caller, our debugging header and possibly
    // some guard bytes.
    unsigned        length = CDA_SIZEOF_HEADER() + (unsigned)n + CDA_OPT_GUARD_BYTES;
    DbgAllocHeader *h;

    if (g_PagePerAlloc) {
        // In page per alloc mode we allocate a number of whole pages. The
        // actual packet is placed at the end of the second to last page and the
        // last page is reserved but never commited (so will cause an access
        // violation if touched). This will catch heap crawl real quick.
        unsigned pages = ((length + (g_PageSize - 1)) / g_PageSize) + 1;
        h = (DbgAllocHeader *)ClrVirtualAlloc(NULL, pages * g_PageSize, MEM_RESERVE, PAGE_NOACCESS);
        if (h) {
            ClrVirtualAlloc(h, (pages - 1) * g_PageSize, MEM_COMMIT, PAGE_READWRITE);
            h = (DbgAllocHeader *)((BYTE *)h + (g_PageSize - (length % g_PageSize)));
        }
    } else
        h = (DbgAllocHeader *)ClrHeapAlloc(g_HeapHandle, 0, length);

    CDA_LOCK();
    if (h == NULL) {

        // Whoops, allocation failure. Record it.
        CDA_STATS_INC(AllocFailures);
        LOG((LF_DBGALLOC, LL_ALWAYS, "DbgAlloc: alloc fail for %u bytes\n", n));

    } else {

        // Check all active packets still look OK.
        if (g_ConstantRecheck)
            DbgValidateActivePackets(h, &CDA_DATA(h, n + CDA_OPT_GUARD_BYTES));

        // Count the total number of bytes we've allocated so far.
        CDA_STATS_ADD(AllocBytes, n);

        // Record the largest amount of concurrent allocations we ever see
        // during the life of the process.
        if((g_AllocStats.m_AllocBytes - g_AllocStats.m_FreeBytes) > g_AllocStats.m_MaxAlloc)
            g_AllocStats.m_MaxAlloc = g_AllocStats.m_AllocBytes - g_AllocStats.m_FreeBytes;

        // Fill in the packet debugging header.
        for (unsigned i = 0; i < g_CallStackDepth; i++) {
            CDA_ALLOC_STACK(h, i) = ppvCallstack[i];
            CDA_DEALLOC_STACK(h, i) = NULL;
        }
        h->m_hmod = NULL;
        h->m_SN = g_NextSN++;
        h->m_Length = (unsigned)n;
        h->m_IsArray = isArray;
        h->m_Prev = g_AllocListLast;
        h->m_Next = NULL;
        h->m_Magic1 = CDA_MAGIC_1;
        *CDA_MAGIC2(h) = CDA_MAGIC_2;

        // If the user wants to breakpoint on the allocation of a specific
        // packet, do it now.
        if (g_BreakOnAlloc && (h->m_SN == g_BreakOnAllocNumber))
            _ASSERTE(!"Hit memory allocation # for breakpoint");

        // Link the packet into the queue of live packets.
        if (g_AllocListLast != NULL) {
            g_AllocListLast->m_Next = h;
            g_AllocListLast = h;
        }
        if (g_AllocListFirst == NULL) {
            _ASSERTE(g_AllocListLast == NULL);
            g_AllocListFirst = h;
            g_AllocListLast = h;
        }

        if (g_PoisonPackets)
            memset(CDA_HEADER_TO_DATA(h), CDA_ALLOC_PATTERN, n);

        // Write a guard pattern after the user data to trap overwrites.
        if (g_AllocGuard)
            memset(&CDA_DATA(h, n), CDA_GUARD_PATTERN, CDA_GUARD_BYTES);

        // See if our allocator makes the list of most frequent allocators.
        if (g_UsageByAllocator) {
            // Look for an existing entry in the table for our EIP, or for the
            // first empty slot (the table is kept in sorted order, so the first
            // empty slot marks the end of the table).
            unsigned i;
            for (i = 0; i < g_TopAllocatorsSlots; i++) {

                if (g_TopAllocators[i].m_EIP == ppvCallstack[0]) {
                    // We already have an entry for this allocator. Incrementing
                    // the count may allow us to move the allocator up the
                    // table.
                    g_TopAllocators[i].m_Count++;
                    g_TopAllocators[i].m_TotalBytes += n;
                    if ((i > 0) &&
                        (g_TopAllocators[i].m_Count > g_TopAllocators[i - 1].m_Count)) {
                        DbgAllocTop tmp = g_TopAllocators[i - 1];
                        g_TopAllocators[i - 1] = g_TopAllocators[i];
                        g_TopAllocators[i] = tmp;
                    }
                    break;
                }

                if (g_TopAllocators[i].m_EIP == NULL) {
                    // We've found an empty slot, we weren't in the table. This
                    // is the right place to put the entry though, since we've
                    // only done a single allocation.
                    g_TopAllocators[i].m_EIP = ppvCallstack[0];
                    g_TopAllocators[i].m_Count = 1;
                    g_TopAllocators[i].m_TotalBytes = n;
                    break;
                }

            }

            if (i == g_TopAllocatorsSlots) {
                // Ran out of space in the table, need to expand it.
                unsigned slots = g_TopAllocatorsSlots ?
                    g_TopAllocatorsSlots * 2 :
                    CDA_TOP_ALLOCATORS;
                DbgAllocTop *newtab = (DbgAllocTop*)ClrAllocInProcessHeap(0, slots*sizeof(DbgAllocTop));
                if (newtab) {

                    // Copy old contents over.
                    if (g_TopAllocatorsSlots) {
                        memcpy(newtab, g_TopAllocators, sizeof(DbgAllocTop) * g_TopAllocatorsSlots);
                        delete [] g_TopAllocators;
                    }

                    // Install new table.
                    g_TopAllocators = newtab;
                    g_TopAllocatorsSlots = slots;

                    // Add new entry to tail.
                    g_TopAllocators[i].m_EIP = ppvCallstack[0];
                    g_TopAllocators[i].m_Count = 1;
                    g_TopAllocators[i].m_TotalBytes = n;

                    // And initialize the rest of the entries to empty.
                    memset(&g_TopAllocators[i + 1],
                           0,
                           sizeof(DbgAllocTop) * (slots - (i + 1)));

                }
            }
        }

        // Count how many allocations of each size range we get. Allocations
        // above a certain size are all dumped into one bucket.
        if (g_LogDist) {
            if (n > CDA_MAX_DIST_SIZE)
                g_LargeAllocs++;
            else {
                for (unsigned i = CDA_DIST_BUCKET_SIZE - 1; i <= CDA_MAX_DIST_SIZE; i += CDA_DIST_BUCKET_SIZE)
                    if (n <= i) {
                        g_AllocBuckets[i/CDA_DIST_BUCKET_SIZE]++;
                        break;
                    }
            }
        }

    }
    CDA_UNLOCK();

    return h ? CDA_HEADER_TO_DATA(h) : NULL;
}


// Free a packet allocated with DbgAlloc.
void __stdcall DbgFree(void *b, void **ppvCallstack, BOOL isArray)
{
    SCAN_IGNORE_FAULT;  // tell the static contract analysis tool to ignore FAULTS due to calls of 'new' in code called by this function
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_DEBUG_ONLY;

    if (!g_DbgEnabled) {
        if (b) // check for null pointer Win98 doesn't like being
                // called to free null pointers.
            ClrFreeInProcessHeap(0, b);
        return;
    }

    // Technically it's possible to get here without having gone through
    // DbgAlloc (since it's legal to deallocate a NULL pointer), so we
    // better check for initializtion to be on the safe side.
    if (!g_HeapInitialized)
        DbgAllocInit();

    CDA_LOCK();

    // Check all active packets still look OK.
    if (g_ConstantRecheck)
        DbgValidateActivePackets(NULL, NULL);

    // Count this call to DbgFree.
    CDA_STATS_INC(Frees);

    if (b == NULL) {
        CDA_STATS_INC(NullFrees);
        CDA_UNLOCK();
        return;
    }

    // Locate the packet header in front of the data packet.
    DbgAllocHeader *h = CDA_DATA_TO_HEADER(b);

    // Verify not calling delete [] on new, and vice versa
    //_ASSERTE (h->m_IsArray == isArray);

    // Check that the header looks OK.
    DbgValidateHeader(h);

    // Count the total number of bytes we've freed so far.
    CDA_STATS_ADD(FreeBytes, h->m_Length);

    // Unlink the packet from the live packet queue.
    if (h->m_Prev)
        h->m_Prev->m_Next = h->m_Next;
    else
        g_AllocListFirst = h->m_Next;
    if (h->m_Next)
        h->m_Next->m_Prev = h->m_Prev;
    else
        g_AllocListLast = h->m_Prev;

    // Zap our link pointers so we'll spot corruption sooner.
    h->m_Next = (DbgAllocHeader *)(UINT_PTR)CDA_INV_PATTERN;
    h->m_Prev = (DbgAllocHeader *)(UINT_PTR)CDA_INV_PATTERN;

    // Zap the tag fields in the header so we'll spot double deallocations
    // straight away.
    h->m_Magic1 = CDA_INV_PATTERN;
    *CDA_MAGIC2(h) = CDA_INV_PATTERN;

    // Poison the user's data area so that continued access to it after the
    // deallocation will likely cause an assertion that much sooner.
    if (g_PoisonPackets)
        memset(b, CDA_DEALLOC_PATTERN, h->m_Length);

    // Record the callstack of the deallocator (handy for debugging double
    // deallocation problems).
    for (unsigned i = 0; i < g_CallStackDepth; i++)
        CDA_DEALLOC_STACK(h, i) = ppvCallstack[i];

    // put the pack on the free list for a while.  Delete the one that it replaces.
    if (g_PoisonPackets) {
        DbgAllocHeader* tmp = g_AllocFreeQueue[g_AllocFreeQueueCur];
        g_AllocFreeQueue[g_AllocFreeQueueCur] = h;
        h = tmp;

        g_AllocFreeQueueCur++;
        if (g_AllocFreeQueueCur >= g_FreeQueueSize)
            g_AllocFreeQueueCur = 0;
    }

    CDA_UNLOCK();

    if (h) {
        if (g_PagePerAlloc) {
            // In page per alloc mode we decommit the pages allocated, but leave
            // them reserved so that we never reuse the same virtual addresses.
            ClrVirtualFree(h, h->m_Length + CDA_SIZEOF_HEADER() + CDA_OPT_GUARD_BYTES, MEM_DECOMMIT);
        } else
            ClrHeapFree(g_HeapHandle, 0, h);
    }
}


// Determine whether an address is part of a live packet, or a live packet
// header. Intended for interactive use in the debugger, outputs to debugger
// console.
DbgAllocHeader *DbgCheckAddress(unsigned ptr)
{
    STATIC_CONTRACT_NOTHROW;

    DbgAllocHeader *h = g_AllocListFirst;
    WCHAR           output[1024];
    void           *p = (void *)(UINT_PTR)ptr;

    while (h) {
        void *head = (void *)h;
        void *start = (void *)CDA_HEADER_TO_DATA(h);
        void *end = (void *)&CDA_DATA(h, h->m_Length);
        void *tail = (void *)&CDA_DATA(h, h->m_Length + CDA_OPT_GUARD_BYTES);
        if ((p >= head) && (p < start)) {
            wsprintfW(output, L"0x%08X is in packet header at 0x%08X\n", p, h);
            WszOutputDebugString(output);
            return h;
        } else if ((p >= start) && (p < end)) {
            wsprintfW(output, L"0x%08X is in data portion of packet at 0x%08X\n", p, h);
            WszOutputDebugString(output);
            return h;
        } else if ((p >= end) && (p < tail)) {
            wsprintfW(output, L"0x%08X is in guard portion of packet at 0x%08X\n", p, h);
            WszOutputDebugString(output);
            return h;
        }
        h = h->m_Next;
    }

    wsprintfW(output, L"%08X not located in any live packet\n", p);
    WszOutputDebugString(output);

    return NULL;
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif  // defined(_MSC_VER)

#endif


