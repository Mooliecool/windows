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
// util.hpp
//
// Miscellaneous useful functions
//
#ifndef _H_UTIL
#define _H_UTIL

#define MAX_UINT32_HEX_CHAR_LEN 8 // max number of chars representing an unsigned int32, not including terminating null char.
#define MAX_INT32_DECIMAL_CHAR_LEN 11 // max number of chars representing an int32, including sign, not including terminating null char.

#include "utilcode.h"
#include "metadata.h"
#include "holderinst.h"
#include "clrdata.h"
#include "xclrdata.h"
#include "posterror.h"

                  
// Prevent the use of UtilMessageBox and WszMessageBox from inside the EE.
#undef UtilMessageBoxCatastrophic
#undef UtilMessageBoxCatastrophicNonLocalized
#undef UtilMessageBoxCatastrophic
#undef UtilMessageBoxCatastrophicNonLocalizedVA
#undef UtilMessageBox
#undef UtilMessageBoxNonLocalized
#undef UtilMessageBoxVA
#undef UtilMessageBoxNonLocalizedVA
#undef WszMessageBox
#define UtilMessageBoxCatastrophic __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")
#define UtilMessageBoxCatastrophicNonLocalized __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")
#define UtilMessageBoxCatastrophicVA __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")
#define UtilMessageBoxCatastrophicNonLocalizedVA __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")
#define UtilMessageBox __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")
#define UtilMessageBoxNonLocalized __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")
#define UtilMessageBoxVA __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")
#define UtilMessageBoxNonLocalizedVA __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")
#define WszMessageBox __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")


//========================================================================
// More convenient names for integer types of a guaranteed size.
//========================================================================

typedef __int8              I1;
typedef unsigned __int8     U1;
typedef __int16             I2;
typedef unsigned __int16    U2;
typedef __int32             I4;
typedef unsigned __int32    U4;
typedef __int64             I8;
typedef unsigned __int64    U8;
typedef float               R4;
typedef double              R8;


// Based on whether we are running on a Uniprocessor or Multiprocessor machine,
// set up a bunch of services that are correct / efficient.  These are initialized
// in InitFastInterlockOps().

typedef void   (__fastcall *BitFieldOps) (DWORD volatile *Target, const int Bits);
typedef LONG   (__fastcall *XchgOps)     (LONG volatile *Target, LONG Value);
typedef LONG    (__fastcall *CmpXchgOps)    (LONG volatile *Destination, LONG Exchange, LONG Comperand);
typedef INT64    (__fastcall *CmpXchgLongOps)    (INT64 volatile *Destination, INT64 Exchange, INT64 Comperand);
typedef LONG   (__fastcall *XchngAddOps) (LONG volatile *Target, LONG Value);
typedef LONG   (__fastcall *IncDecOps)   (LONG volatile *Target);
typedef UINT64  (__fastcall *IncDecLongOps) (UINT64 volatile *Target);
typedef INT64   (__fastcall *XchgLongOps)     (INT64 volatile *Target, INT64 Value);
typedef INT64   (__fastcall *XchgAddLongOps)     (INT64 volatile *Target, INT64 Value);

//
// these don't have corresponding compiler intrinsics
//
extern BitFieldOps FastInterlockOr;
extern BitFieldOps FastInterlockAnd;
extern IncDecLongOps FastInterlockIncrementLong;
extern IncDecLongOps FastInterlockDecrementLong;

//
// these DO have corresponding compiler intrinsics
//
#if !defined(_WIN64) && !defined(DACCESS_COMPILE)
extern XchgOps     FastInterlockExchange;
extern XchgLongOps     FastInterlockExchangeLong;
extern CmpXchgOps  FastInterlockCompareExchange;
extern CmpXchgLongOps  FastInterlockCompareExchangeLong;
extern XchngAddOps FastInterlockExchangeAdd;
extern XchgAddLongOps FastInterlockExchangeAddLong;

inline PVOID FastInterlockExchangePointer(PVOID volatile *Target, PVOID Value)
{
    LEAF_CONTRACT;

    return (PVOID)(size_t)FastInterlockExchange((LONG*)Target, (LONG)(size_t)Value);
}

inline PVOID FastInterlockCompareExchangePointer(PVOID volatile *Destination, PVOID Exchange, PVOID Comperand)
{
    LEAF_CONTRACT;

    return (PVOID)(size_t)FastInterlockCompareExchange((LONG*)Destination, (LONG)(size_t)Exchange, (LONG)(size_t)Comperand);
}

// So that we can run on Win95 386, which lacks the xadd instruction, the following
// services return zero or a positive or negative number only.  Do not rely on
// values -- only on the sign of the return.
extern IncDecOps   FastInterlockIncrement;
extern IncDecOps   FastInterlockDecrement;
#else

#define FastInterlockExchange               InterlockedExchange
#define FastInterlockExchangeLong           InterlockedExchange64
#define FastInterlockExchangePointer        InterlockedExchangePointer
#define FastInterlockCompareExchange        InterlockedCompareExchange
#define FastInterlockCompareExchangeLong    InterlockedCompareExchange64
#define FastInterlockCompareExchangePointer InterlockedCompareExchangePointer
#define FastInterlockExchangeAdd                   InterlockedExchangeAdd
#define FastInterlockExchangeAddLong            InterlockedExchangeAdd64

// So that we can run on Win95 386, which lacks the xadd instruction, the following
// services return zero or a positive or negative number only.  Do not rely on
// values -- only on the sign of the return.
#define FastInterlockIncrement              InterlockedIncrement
#define FastInterlockDecrement              InterlockedDecrement

#endif

#define FastInterlockCompareExchangeAcquire InterlockedCompareExchange
#define FastInterlockCompareExchangeRelease InterlockedCompareExchange


// Function to parse apart a command line and return the
// arguments just like argv and argc
LPWSTR* CommandLineToArgvW(LPWSTR lpCmdLine, DWORD *pNumArgs);
#define ISWWHITE(x) ((x)==L' ' || (x)==L'\t' || (x)==L'\n' || (x)==L'\r' )



BOOL inline FitsInI1(__int64 val)
{
    LEAF_CONTRACT;

    return val == (__int64)(__int8)val;
}

BOOL inline FitsInI2(__int64 val)
{
    LEAF_CONTRACT;

    return val == (__int64)(__int16)val;
}

BOOL inline FitsInI4(__int64 val)
{
    LEAF_CONTRACT;

    return val == (__int64)(__int32)val;
}


BOOL inline FitsInU1(unsigned __int64 val)
{
    return val == (unsigned __int64)(unsigned __int8)val;
}

BOOL inline FitsInU2(unsigned __int64 val)
{
    return val == (unsigned __int64)(unsigned __int16)val;
}

BOOL inline FitsInU4(unsigned __int64 val)
{
    return val == (unsigned __int64)(unsigned __int32)val;
}


// returns FALSE if overflows 15 bits: otherwise, (*pa) is incremented by b
BOOL inline SafeAddUINT15(UINT16 *pa, ULONG b)
{
    LEAF_CONTRACT;

    UINT16 a = *pa;
    // first check if overflows 16 bits
    if ( ((UINT16)b) != b )
    {
        return FALSE;
    }
    // now make sure that doesn't overflow 15 bits
    if (((ULONG)a + b) > 0x00007FFF)
    {
        return FALSE;
    }
    (*pa) += (UINT16)b;
    return TRUE;
}


// returns FALSE if overflows 16 bits: otherwise, (*pa) is incremented by b
BOOL inline SafeAddUINT16(UINT16 *pa, ULONG b)
{
    UINT16 a = *pa;
    if ( ((UINT16)b) != b )
    {
        return FALSE;
    }
    // now make sure that doesn't overflow 16 bits
    if (((ULONG)a + b) > 0x0000FFFF)
    {
        return FALSE;
    }
    (*pa) += (UINT16)b;
    return TRUE;
}


// returns FALSE if overflow: otherwise, (*pa) is incremented by b
BOOL inline SafeAddUINT32(UINT32 *pa, UINT32 b)
{
    LEAF_CONTRACT;

    UINT32 a = *pa;
    if ( ((UINT32)(a + b)) < a)
    {
        return FALSE;
    }
    (*pa) += b;
    return TRUE;
}

// returns FALSE if overflow: otherwise, (*pa) is incremented by b
BOOL inline SafeAddULONG(ULONG *pa, ULONG b)
{
    LEAF_CONTRACT;

    ULONG a = *pa;
    if ( ((ULONG)(a + b)) < a)
    {
        return FALSE;
    }
    (*pa) += b;
    return TRUE;
}

// returns FALSE if overflow: otherwise, (*pa) is multiplied by b
BOOL inline SafeMulSIZE_T(SIZE_T *pa, SIZE_T b)
{
    LEAF_CONTRACT;

#ifdef _DEBUG_IMPL
    {
        //Make sure SIZE_T is unsigned
        SIZE_T m = ((SIZE_T)(-1));
        SIZE_T z = 0;
        _ASSERTE(m > z);
    }
#endif


    SIZE_T a = *pa;
    const SIZE_T m = ((SIZE_T)(-1));
    if ( (m / b) < a )
    {
        return FALSE;
    }
    (*pa) *= b;
    return TRUE;
}



//************************************************************************
// CQuickHeap
//
// A fast non-multithread-safe heap for short term use.
// Destroying the heap frees all blocks allocated from the heap.
// Blocks cannot be freed individually.
//
// The heap uses COM+ exceptions to report errors.
//
// The heap does not use any internal synchronization so it is not
// multithreadsafe.
//************************************************************************
class CQuickHeap
{
    public:
        CQuickHeap();
        ~CQuickHeap();

        //---------------------------------------------------------------
        // Allocates a block of "sz" bytes. If there's not enough
        // memory, throws an OutOfMemoryError.
        //---------------------------------------------------------------
        LPVOID Alloc(UINT sz);


    private:
        enum {
#ifdef _DEBUG
            kBlockSize = 24
#else
            kBlockSize = 1024
#endif
        };

        struct QuickBlock
        {
            QuickBlock  *m_next;
            BYTE         m_bytes[1];
        };


        // Linked list of QuickBlock's.
        QuickBlock      *m_pFirstQuickBlock;

        // Offset to next available byte in m_pFirstQuickBlock.
        LPBYTE           m_pNextFree;

        // Linked list of big QuickBlock's
        QuickBlock      *m_pFirstBigQuickBlock;

};

//======================================================================
// String Helpers
//
//
//
ULONG StringHashValueW(LPWSTR wzString);
ULONG StringHashValueA(LPCSTR szString);


LPCVOID ReserveAlignedMemory(DWORD dwAlign, DWORD dwSize);

void PrintToStdOutA(const char *pszString);
void PrintToStdOutW(const WCHAR *pwzString);
void PrintToStdErrA(const char *pszString);
void PrintToStdErrW(const WCHAR *pwzString);
void NPrintToStdOutA(const char *pszString, size_t nbytes);
void NPrintToStdOutW(const WCHAR *pwzString, size_t nchars);
void NPrintToStdErrA(const char *pszString, size_t nbytes);
void NPrintToStdErrW(const WCHAR *pwzString, size_t nchars);


//=====================================================================
// Function for formatted text output to the debugger
//
//
void __cdecl VMDebugOutputA(LPSTR format, ...);
void __cdecl VMDebugOutputW(LPWSTR format, ...);

//=====================================================================
// VM-safe wrapper for PostError.
//
HRESULT VMPostError(                    // Returned error.
    HRESULT     hrRpt,                  // Reported error.
    ...);                               // Error arguments.
    
//=====================================================================
// Displays the messaage box or logs the message, corresponding to the last COM+ error occured
void VMDumpCOMErrors(HRESULT hrErr);
HRESULT LoadMscorsn();

#include "nativevaraccessors.h"



// --------------------------------------------------------------------------------
// GCX macros
//
// These are the normal way to change or assert the GC mode of a thread.  They handle
// the required stack discipline in mode switches with an autodestructor which
// automatically triggers on leaving the current scope.
//
// Usage:
// GCX_COOP();              Switch to cooperative mode, assume thread is setup
// GCX_PREEMP();            Switch to preemptive mode, NOP if no thread setup
// GCX_COOP_THREAD_EXISTS(Thread*);    Fast switch to cooperative mode, must pass non-null Thread
// GCX_PREEMP_THREAD_EXISTS(Thread*);  Fast switch to preemptive mode, must pass non-null Thread
//
// (There is an intentional asymmetry between GCX_COOP and GCX_PREEMP. GCX_COOP
// asserts if you call it without having a Thread setup. GCX_PREEMP becomes a NOP.
// This is because all unmanaged threads are effectively preemp.)
//
// (There is actually one more case here - an "EE worker thread" such as the debugger
// thread or GC thread, which we don't want to call SetupThread() on, but which is
// effectively in cooperative mode due to explicit cooperation with the collector.
// This case is not handled by these macros; the current working assumption is that
// such threads never use them. But at some point we may have to consider
// this case if there is utility code which is called from those threads.)
//
// GCX_MAYBE_*(BOOL);       Same as above, but only do the switch if BOOL is TRUE.
//
// GCX_POP();               Early out of the GCX in the current scope.
//
// GCX_ASSERT_*();          Same as above, but assert mode rather than switch to mode.
//                          Note that assert is applied during backout as well.
//                          No overhead in a free build.
//
// GCX_FORBID();            Add "ForbidGC" semantics to a cooperative mode situation.
//                          Asserts that the thread will not trigger a GC or
//                          reach a GC-safe point, or call anything that might
//                          do one of these things.
//
// GCX_NOTRIGGER();         "ForbidGC" without the automatic assertion for coop mode.
//
// -------------------------------------------------------------------------------- </STRIP>

template <BOOL COOPERATIVE, BOOL THREAD_EXISTS, BOOL HACK_NO_THREAD>
class GCHolder;

typedef GCHolder<TRUE, FALSE, FALSE>    GCCoop;
typedef GCHolder<FALSE, FALSE, FALSE>   GCPreemp;
typedef GCHolder<TRUE, TRUE, FALSE>     GCCoopThreadExists;
typedef GCHolder<FALSE, TRUE, FALSE>    GCPreempThreadExists;


template<BOOL COOPERATIVE>
class GCAssert;

typedef GCAssert<TRUE>                  GCAssertCoop;
typedef GCAssert<FALSE>                 GCAssertPreemp;

#define GCX_COOP()                      GCCoop __gcHolder


#define GCX_PREEMP()                        GCPreemp __gcHolder

#define GCX_PREEMP_IF(fSwitch)              GCPreemp __gcHolder(fSwitch)

#define GCX_COOP_THREAD_EXISTS(curThread)   GCCoopThreadExists __gcHolder((curThread))

#define GCX_PREEMP_THREAD_EXISTS(curThread) GCPreempThreadExists __gcHolder((curThread))

#define GCX_MAYBE_COOP(_cond)                             GCCoop __gcHolder(_cond)


#define GCX_MAYBE_PREEMP(_cond)                           GCPreemp __gcHolder(_cond)

#define GCX_MAYBE_COOP_THREAD_EXISTS(curThread, _cond)    GCCoopThreadExists __gcHolder((curThread), (_cond))

#define GCX_MAYBE_PREEMP_THREAD_EXISTS(curThread, _cond)  GCPreempThreadExists __gcHolder((curThread), (_cond))

#ifdef _MSC_VER

#define GCX_COOP_NO_THREAD_BROKEN()                 GCHolder<TRUE, FALSE, TRUE> __gcHolder

#else

// This macro is used inside template without GCHolder type being fully resolved. 
// Non-Microsoft compilers need a slightly different syntax for this case.
#define GCX_COOP_NO_THREAD_BROKEN()                 GCHolder<TRUE, FALSE, TRUE> __gcHolder()

#endif


#define GCX_MAYBE_COOP_NO_THREAD_BROKEN(_cond)      GCHolder<TRUE, FALSE, TRUE> __gcHolder(_cond)

#define GCX_POP()                           __gcHolder.Pop()

#ifdef _DEBUG_IMPL

#define GCX_ASSERT_PREEMP()                 ::GCAssertPreemp __gcHolder
#define GCX_ASSERT_COOP()                   ::GCAssertCoop __gcHolder

#else

#define GCX_ASSERT_PREEMP()
#define GCX_ASSERT_COOP()

#endif



#define GCX_FORBID()
#define GCX_NOTRIGGER()

#define GCX_MAYBE_FORBID(fConditional)
#define GCX_MAYBE_NOTRIGGER(fConditional)


typedef BOOL (*FnLockOwner)(LPVOID);
struct LockOwner
{
    LPVOID lock;
    FnLockOwner lockOwnerFunc;
};

// this is the standard lockowner for things that require a lock owner but which really don't 
// need any validation due to their simple/safe semantics
// the classic example of this is a hash table that is initialized and then never grows
extern LockOwner g_lockTrustMeIAmThreadSafe;

// The OS ThreadId is not a stable ID for a thread we a host uses fiber instead of Thread.
// For each managed Thread, we have a stable and unique id in Thread object.  For other threads,
// e.g. Server GC or Concurrent GC thread, debugger helper thread, we do not have a Thread object,
// and we use OS ThreadId to identify them since they are not managed by a host.
class EEThreadId
{
private:
    void *m_FiberPtrId;
public:
#ifdef _DEBUG
    EEThreadId()
    : m_FiberPtrId(NULL)
    {
        LEAF_CONTRACT;
    }
#endif

    void SetThreadId()
    {
        WRAPPER_CONTRACT;

        m_FiberPtrId = ClrTeb::GetFiberPtrId();
    }

    BOOL IsSameThread() const
    {
        WRAPPER_CONTRACT;

        return (m_FiberPtrId == ClrTeb::GetFiberPtrId());
    }

    
#ifdef _DEBUG
    BOOL IsUnknown() const
    {
        LEAF_CONTRACT;
        return m_FiberPtrId == NULL;
    }
#endif
    void ResetThreadId()
    {
        LEAF_CONTRACT;
        m_FiberPtrId = NULL;
    }
};

#define CLRMEMORYHOSTED                             0x1
#define CLRTASKHOSTED                               0x2
#define CLRSYNCHOSTED                               0x4
#define CLRTHREADPOOLHOSTED                         0x8
#define CLRIOCOMPLETIONHOSTED                       0x10
#define CLRASSEMBLYHOSTED                           0x20
#define CLRGCHOSTED                                 0x40
#define CLRSECURITYHOSTED                           0x80
#define CLRHOSTED           0x80000000

GVAL_DECL(DWORD, g_fHostConfig);


inline BOOL CLRRobustToSO()
{
    LEAF_CONTRACT;

    return g_fHostConfig;
}


inline BOOL CLRHosted()
{
    LEAF_CONTRACT;

    return g_fHostConfig;
}

inline BOOL CLRMemoryHosted()
{
    LEAF_CONTRACT;

    return g_fHostConfig&CLRMEMORYHOSTED;
}

inline BOOL CLRTaskHosted()
{
    // !!! Can not use contract here.
    // !!! This function is called by Thread::DetachThread after we free TLS memory.
    // !!! Contract will recreate TLS memory.
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    return g_fHostConfig&CLRTASKHOSTED;
}

inline BOOL CLRSyncHosted()
{
    LEAF_CONTRACT;

    return g_fHostConfig&CLRSYNCHOSTED;
}
inline BOOL CLRThreadpoolHosted()
{
    LEAF_CONTRACT;

    return g_fHostConfig&CLRTHREADPOOLHOSTED;
}
inline BOOL CLRIoCompletionHosted()
{
    LEAF_CONTRACT;

    return g_fHostConfig&CLRIOCOMPLETIONHOSTED;
}
inline BOOL CLRAssemblyHosted()
{
    return g_fHostConfig&CLRASSEMBLYHOSTED;
}

inline BOOL CLRGCHosted()
{
    LEAF_CONTRACT;

    return g_fHostConfig&CLRGCHOSTED;
}

inline BOOL CLRSecurityHosted()
{
    LEAF_CONTRACT;

    return g_fHostConfig&CLRSECURITYHOSTED;
}


HMODULE CLRLoadLibrary(LPCWSTR lpLibFileName);


// Loads the given system library from the internal system folder
HMODULE CLRLoadSystemLibrary(LPCWSTR lpLibFileName);

BOOL CLRFreeLibrary(HMODULE hModule);
VOID CLRFreeLibraryAndExitThread(HMODULE hModule, DWORD dwExitCode);

LPVOID
CLRMapViewOfFileEx(
    IN HANDLE hFileMappingObject,
    IN DWORD dwDesiredAccess,
    IN DWORD dwFileOffsetHigh,
    IN DWORD dwFileOffsetLow,
    IN SIZE_T dwNumberOfBytesToMap,
    IN LPVOID lpBaseAddress
    );

LPVOID
CLRMapViewOfFile(
    IN HANDLE hFileMappingObject,
    IN DWORD dwDesiredAccess,
    IN DWORD dwFileOffsetHigh,
    IN DWORD dwFileOffsetLow,
    IN SIZE_T dwNumberOfBytesToMap
    );


BOOL
CLRUnmapViewOfFile(
    IN LPVOID lpBaseAddress
    );

BOOL CompareFiles(HANDLE hFile1,HANDLE hFile2);


#ifndef DACCESS_COMPILE
FORCEINLINE void VoidCLRUnmapViewOfFile(void *ptr) { CLRUnmapViewOfFile(ptr); }
typedef Wrapper<void *, DoNothing, VoidCLRUnmapViewOfFile> CLRMapViewHolder;
#else
typedef Wrapper<void *, DoNothing, DoNothing> CLRMapViewHolder;
#endif


void ProcessEventForHost(EClrEvent event, void *data);
void ProcessSOEventForHost(EXCEPTION_POINTERS *pExceptionInfo, BOOL fInSoTolerant);
BOOL IsHostRegisteredForEvent(EClrEvent event);

#define InternalSetupForComCall(CannotEnterRetVal, OOMRetVal, SORetVal)     \
if (!CanRunManagedCode())                                                   \
    return CannotEnterRetVal;                                               \
Thread *__pThread = GetThread();                                            \
if (__pThread == NULL)                                                      \
{                                                                           \
    __pThread = SetupThreadNoThrow();                                       \
    if (__pThread == NULL)                                                  \
        return OOMRetVal;                                                   \
}                                                                           \
SO_INTOLERANT_CODE_RETVAL(__pThread, SORetVal) \

#define SetupForComCallHRNoHostNotif() InternalSetupForComCall(COR_E_CANNOTENTEREE, E_OUTOFMEMORY, COR_E_STACKOVERFLOW)
#define SetupForComCallDWORDNoHostNotif() InternalSetupForComCall(-1, -1, -1)

#define SetupForComCallHR()                                                 \
InternalSetupForComCall(HOST_E_CLRNOTAVAILABLE, E_OUTOFMEMORY, COR_E_STACKOVERFLOW)              \
ReverseEnterRuntimeHolder REHolder(FALSE);                                  \
if (CLRTaskHosted())                                                        \
{                                                                           \
    BOOL fRet = FALSE;                                                      \
    HRESULT hr = S_OK;                                                      \
                                                                            \
    Exception* __pException = NULL;                                         \
    EX_TRY {                                                                \
        REHolder.Acquire();                                                 \
    }                                                                       \
    EX_CATCH {                                                              \
        __pException = GET_EXCEPTION();                                     \
    }                                                                       \
    EX_END_CATCH_UNSAFE_UNSCOPED(SwallowAllExceptions);                     \
                                                                            \
        if (__pException != NULL)                                           \
        {                                                                   \
            fRet = TRUE;                                                    \
            hr = __pException->GetHR();                                     \
        }                                                                   \
    }                                                                       \
                                                                            \
    if (fRet)                                                               \
    {                                                                       \
        return hr;                                                          \
    }                                                                       \
}

#define SetupForComCallDWORD()                                              \
InternalSetupForComCall(-1, -1, -1)                                         \
ReverseEnterRuntimeHolder REHolder(FALSE);                                  \
if (CLRTaskHosted())                                                        \
{                                                                           \
    BOOL fRet = FALSE;                                                      \
    EX_TRY {                                                                \
        REHolder.Acquire();                                                 \
    }                                                                       \
    EX_CATCH {                                                              \
        fRet = TRUE;                                                        \
    }                                                                       \
    EX_END_CATCH(SwallowAllExceptions);                                     \
    if (fRet)                                                               \
    {                                                                       \
        return -1;                                                          \
    }                                                                       \
}

#include "unsafe.h"

inline void UnsafeTlsFreeForHolder(DWORD* addr)
{
    WRAPPER_CONTRACT;

    if (addr && *addr != TLS_OUT_OF_INDEXES) {
        UnsafeTlsFree(*addr);
        *addr = TLS_OUT_OF_INDEXES;
    }
}

// A holder to make sure tls slot is released and memory for allocated one is set to TLS_OUT_OF_INDEXES
typedef Holder<DWORD*, DoNothing<DWORD*>, UnsafeTlsFreeForHolder> TlsHolder;

// A holder for HMODULE.
FORCEINLINE void VoidFreeLibrary(HMODULE h) { WRAPPER_CONTRACT; CLRFreeLibrary(h); }

typedef Wrapper<HMODULE, DoNothing<HMODULE>, VoidFreeLibrary, NULL> ModuleHandleHolder;

// For debugging, we can track arbitrary Can't-Stop regions.
// In V1.0, this was on the Thread objet, but we need to track this for threads w/o a Thread object.
inline void IncCantStopCount(StateHolderParam)
{
    size_t count = (size_t) ClrFlsGetValue(TlsIdx_CantStopCount);
    ClrFlsSetValue(TlsIdx_CantStopCount, (LPVOID) (count+1));
}
inline void DecCantStopCount(StateHolderParam)
{
    size_t count = (size_t) ClrFlsGetValue(TlsIdx_CantStopCount);
    _ASSERTE(count > 0);
    ClrFlsSetValue(TlsIdx_CantStopCount, (LPVOID) (count-1));
}

typedef StateHolder<IncCantStopCount, DecCantStopCount> CantStopHolder;

// This can be used w/ a holder to ensure that we're keeping our CS count balanced.
// This is now enabled for free/retail also because it's now used for the profiler
inline int GetCantStopCount()
{
    return (int) (size_t) ClrFlsGetValue(TlsIdx_CantStopCount);
}

// At places where we know we're calling out to native code, we can assert that we're NOT in a CS region.
// This is now enabled for free/retail also because it's now used for the profiler
inline bool IsInCantStopRegion()
{    
    return (GetCantStopCount() > 0);
}

class ThreadLocaleHolder
{
public:
    ThreadLocaleHolder()
    {
        m_locale = GetThreadLocale();
    }

    ~ThreadLocaleHolder();

private:
    LCID m_locale;
};

BOOL IsValidMethodCodeNotification(USHORT Notification);
    
typedef DPTR(struct JITNotification) PTR_JITNotification;
struct JITNotification
{
    USHORT state; // values from CLRDataMethodCodeNotification
    TADDR clrModule;
    mdToken methodToken;
    
    JITNotification() { SetFree(); } 
    BOOL IsFree() { return state == CLRDATA_METHNOTIFY_NONE; }
    void SetFree() { state = CLRDATA_METHNOTIFY_NONE; clrModule = NULL; methodToken = 0; }
    void SetState(TADDR moduleIn, mdToken tokenIn, USHORT NType) 
    { 
        _ASSERTE(IsValidMethodCodeNotification(NType)); 
        clrModule = moduleIn; 
        methodToken = tokenIn; 
        state = NType; 
    }
};

GPTR_DECL(JITNotification,g_pNotificationTable);
GVAL_DECL(ULONG32, g_dacNotificationFlags);

class JITNotifications
{
public:
    JITNotifications(JITNotification *jitTable);    
    BOOL SetNotification(TADDR clrModule, mdToken token, USHORT NType);
    USHORT Requested(TADDR clrModule, mdToken token); 

    // if clrModule is NULL, all active notifications are changed to NType
    BOOL SetAllNotifications(TADDR clrModule,USHORT NType,BOOL *changedOut);
    inline BOOL IsActive() { return m_jitTable!=NULL; }    

    UINT GetTableSize();    
#ifdef DACCESS_COMPILE
    static JITNotification *InitializeNotificationTable(UINT TableSize);
    // Updates target table from host copy
    BOOL UpdateOutOfProcTable();
#endif

private:
    UINT GetLength();
    void IncrementLength();
    void DecrementLength();

    BOOL FindItem(TADDR clrModule, mdToken token, UINT *indexOut);
    
    JITNotification *m_jitTable;
};

class MethodDesc;
class Module;

class DACNotify
{
public:
    // types
    enum {
        MODULE_LOAD_NOTIFICATION=1,
        MODULE_UNLOAD_NOTIFICATION=2,
        JIT_NOTIFICATION=3,
        JIT_DISCARD_NOTIFICATION=4,
        EXCEPTION_NOTIFICATION=5,
    };
    
    // called from the runtime
    static void DoJITNotification(MethodDesc *MethodDescPtr);
    static void DoJITDiscardNotification(MethodDesc *MethodDescPtr);    
    static void DoModuleLoadNotification(Module *Module);
    static void DoModuleUnloadNotification(Module *Module);
    static void DoExceptionNotification(class Thread* ThreadPtr);

    // called from the DAC
    static int GetType(TADDR Args[]);
    static BOOL ParseJITNotification(TADDR Args[], TADDR& MethodDescPtr);
    static BOOL ParseJITDiscardNotification(TADDR Args[], TADDR& MethodDescPtr);
    static BOOL ParseModuleLoadNotification(TADDR Args[], TADDR& ModulePtr);
    static BOOL ParseModuleUnloadNotification(TADDR Args[], TADDR& ModulePtr);
    static BOOL ParseExceptionNotification(TADDR Args[], TADDR& ThreadPtr);
};

void DACNotifyCompilationFinished(MethodDesc *pMethodDesc);
    
#ifdef _DEBUG
#endif //_DEBUG


// These wrap the SString:L:CompareCaseInsenstive function in a way that makes it
// easy to fix code that uses _stricmp. _stricmp should be avoided as it uses the current
// C-runtime locale rather than the invariance culture.
//
// Note that unlike the real _stricmp, these functions unavoidably have a throws/gc_triggers/inject_fault
// contract. So if need a case-insensitive comparison in a place where you can't tolerate this contract,
// you've got a problem.
int __cdecl stricmpUTF8(const char* szStr1, const char* szStr2);

#ifdef _DEBUG
class DisableDelayLoadCheckForOleaut32
{
public:
    DisableDelayLoadCheckForOleaut32();
    ~DisableDelayLoadCheckForOleaut32();
};
#endif

extern LONG g_OLEAUT32_Loaded;

#define ENSURE_OLEAUT32_LOADED()

BOOL DbgIsExecutableVM(LPVOID lpMem, SIZE_T length);


#ifndef DACCESS_COMPILE

//*************************************************************************************
// The purpose of this class to flush the store buffers on all the CPUs 
//  that the current process is running on
// Wherever supported, it uses the OS API FlushWriteBuffers
// VirtualProtect will obtain the desired side effect on all OS releases where
//  the API is not supported; it doesn't need to actually change the page protection
//  as calling VirtualProtect with the current page protection will still ensure the 
//  buffer flush
// On a single-proc system, there is no need to flush store buffers and so this is a nop

class CpuStoreBufferControl
{
public:
    static void Init();
    static void FlushStoreBuffers();

private:    
    typedef void (*PFN_FlushWriteBuffers)();
    static PFN_FlushWriteBuffers s_pFlushWriteBuffersFn;
    static volatile BYTE s_UseVirtualProtect;

#ifdef _DEBUG
    static bool s_Initialized;
#endif    
};

#endif // #ifndef DACCESS_COMPILE

#endif /* _H_UTIL */

