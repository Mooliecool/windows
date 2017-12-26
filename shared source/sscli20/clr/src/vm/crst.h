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
// CRST.H
//
// Debug-instrumented hierarchical critical sections.
//
//
// The hierarchy:
// --------------
//    The EE divides critical sections into numbered groups or "levels."
//    Crsts that guard the lowest level data structures that don't
//    use other services are grouped into the lowest-numbered levels.
//    The higher-numbered levels are reserved for high-level crsts
//    that guard broad swatches of code. Multiple groups can share the
//    same number to indicate that they're disjoint (their locks will never
//    nest.)
//
//    The fundamental rule of the hierarchy that threads can only request
//    a crst whose level is lower than any crst currently held by the thread.
//    E.g. if a thread current holds a level-3 crst, he can try to enter
//    a level-2 crst, but not a level-4 crst, nor a different level-3
//    crst. This prevents the cyclic dependencies that lead to deadlock.
//
//
//
// To create a crst:
//
//    Crst *pcrst = new Crst("tag", level);
//
//      where the "tag" is a short string describing the purpose of the crst
//      (to aid debugging) and the level of the crst (a member of the CrstLevel
//      enum.)
//
//      By default, crsts don't support nested enters by the same thread. If
//      you need reentrancy, use the alternate form:
//
//    Crst *pcrst = new Crst("tag", level, TRUE);
//
//      Since reentrancies never block the caller, they're allowed to
//     "violate" the level ordering rule.
//
//
// To enter/leave a crst:
// ----------------------
//
//
//    pcrst->Enter();
//    pcrst->Leave();
//
// An assertion will fire on Enter() if a thread attempts to take locks
// in the wrong order.
//
// Finally, a few DEBUG-only methods:
//
// To assert taking a crst won't violate level order:
// --------------------------------------------------
//
//    _ASSERTE(pcrst->IsSafeToTake());
//
//    This is a good line to put at the start of any function that
//    enters a crst in some circumstances but not others. If it
//    always enters the crst, it's not necessary to call IsSafeToTake()
//    since Enter() does this for you.
//
// To assert that the current thread owns a crst:
// --------------------------------------------------
//
//   _ASSERTE(pcrst->OwnedByCurrentThread());


#ifndef __crst_h__
#define __crst_h__

#include "util.hpp"
#include "log.h"

#define ShutDown_Start                          0x00000001
#define ShutDown_Finalize1                      0x00000002
#define ShutDown_Finalize2                      0x00000004
#define ShutDown_Profiler                       0x00000008
#define ShutDown_COM                            0x00000010
#define ShutDown_SyncBlock                      0x00000020
#define ShutDown_IUnknown                       0x00000040
#define ShutDown_Phase2                         0x00000080

#ifndef DACCESS_COMPILE
extern bool g_fProcessDetach;
#endif
extern DWORD g_fEEShutDown;
extern BOOL g_fForbidEnterEE;
extern bool g_fFinalizerRunOnShutDown;

// The CRST.
class CrstBase
{
// The following classes and methods violate the requirement that Crst usage be
// exception-safe, or they satisfy that requirement using techniques other than
// Holder objects:
friend class ThreadStore;
friend class ListLock;
friend class ListLockEntry;
//friend class CExecutionEngine;
friend struct SavedExceptionInfo;
friend void EEEnterCriticalSection(CRITSEC_COOKIE cookie);
friend void EELeaveCriticalSection(CRITSEC_COOKIE cookie);

friend class Debugger;
friend class Crst;

public:
#ifdef _DEBUG
    enum NoLevelCheckFlag
    {
        CRST_NO_LEVEL_CHECK = 1,
        CRST_LEVEL_CHECK = 0,
    };
#endif

private:
    //
    // 
    // 
    void ReleaseAndBlockForShutdownIfNotSpecialThread();

    // Enter & Leave are deliberately private to force callers to use the
    // Holder class.  If you bypass the Holder class and access these members
    // directly, your lock is not exception-safe.
    //
    // noLevelCheckFlag parameter lets you disable the crst level checking. This is
    // very dangerous so it is only used when the constructor is the one performing
    // the Enter (that attempt cannot possibly block since the current thread is
    // the only one with a pointer to the crst.)
    //
    // For obvious reasons, this parameter must never be made public.
    void Enter(INDEBUG(NoLevelCheckFlag noLevelCheckFlag = CRST_LEVEL_CHECK));
    void Leave();

#ifndef DACCESS_COMPILE
    static void AcquireLock(CrstBase *c) {
        WRAPPER_CONTRACT;
        c->Enter(); 
    }

    static void ReleaseLock(CrstBase *c) { 
        WRAPPER_CONTRACT;
        c->Leave(); 
    }
#else // DACCESS_COMPILE
    static void AcquireLock(CrstBase *c);
    static void ReleaseLock(CrstBase *c);
#endif // DACCESS_COMPILE

    static void AcquirePreempLock(CrstBase *c);

public:
    //-----------------------------------------------------------------
    // Clean up critical section
    // Safe to call multiple times or on non-initialized critical section
    //-----------------------------------------------------------------
    void Destroy();

    
#ifdef _DEBUG
    //-----------------------------------------------------------------
    // Check if attempting to take the lock would violate level order.
    //-----------------------------------------------------------------
    BOOL IsSafeToTake();
    void SetCantLeave(BOOL bSet)
    {
        LEAF_CONTRACT;
        if (bSet)
            FastInterlockIncrement(&m_cannotLeave);
        else
        {
            _ASSERTE(m_cannotLeave);
            FastInterlockDecrement(&m_cannotLeave);
        }
    };    
    //-----------------------------------------------------------------
    // Is the current thread the owner?
    //-----------------------------------------------------------------
    BOOL OwnedByCurrentThread()
    {
        WRAPPER_CONTRACT;
        return m_holderthreadid.IsSameThread();
    }
    
    //-----------------------------------------------------------------
    // For clients who want to assert whether they are in or out of the
    // region.
    //-----------------------------------------------------------------
    UINT GetEnterCount()
    {
        LEAF_CONTRACT;
        return m_entercount;
    }

    CrstBase *GetThreadsOwnedCrsts();
    void SetThreadsOwnedCrsts(CrstBase *pCrst);
#endif //_DEBUG
    
protected:    

    VOID InitWorker(INDEBUG_COMMA(LPCSTR szTag) INDEBUG_COMMA(CrstLevel crstlevel) CrstFlags flags);

#ifdef _DEBUG
    void DebugInit(LPCSTR szTag, CrstLevel crstlevel, CrstFlags flags);
    void DebugDestroy();
#endif


    union {
        CRITICAL_SECTION    m_criticalsection;
        IHostCrst          *m_pHostCrst;
    };

    typedef enum
    {
        // Mask to indicate reserved flags
        CRST_RESERVED_FLAGS_MASK = 0xC0000000,
        // private flag to indicate initialized Crsts
        CRST_INITIALIZED = 0x80000000,
        // private flag to indicate Crst is OS Critical Section
        CRST_OS_CRIT_SEC = 0x40000000,

        // rest of the flags are CrstFlags
    } CrstReservedFlags;
    DWORD               m_dwFlags;            // Re-entrancy and same level
#ifdef _DEBUG
    char                m_tag[30];          // descriptive string 
    unsigned int        m_crstlevel;        // what level is the crst in?
    EEThreadId          m_holderthreadid;   // current holder (or NULL)
    UINT                m_entercount;       // # of unmatched Enters
    CrstBase           *m_next;             // link for global linked list
    CrstBase           *m_prev;             // link for global linked list
    volatile LONG       m_cannotLeave; 

    // Check for dead lock situation.
    ULONG               m_countNoTriggerGC;
    
    void                PostEnter ();
    void                PreEnter ();
    void                PreLeave  ();
#endif //_DEBUG
    


private:

    void SetOSCritSec ()
    {
        m_dwFlags |= CRST_OS_CRIT_SEC;
    }
    void ResetOSCritSec ()
    {
        m_dwFlags &= ~CRST_OS_CRIT_SEC;
    }
    BOOL IsOSCritSec ()
    {
        return m_dwFlags & CRST_OS_CRIT_SEC;
    }
    void SetCrstInitialized()
    {
        m_dwFlags |= CRST_INITIALIZED;
    }

    BOOL IsCrstInitialized()
    {
        return m_dwFlags & CRST_INITIALIZED;
    }

    void SetFlags(CrstFlags f)
    {
        _ASSERTE(((CrstFlags)(f & ~CRST_RESERVED_FLAGS_MASK)) == f);
        m_dwFlags = (f & ~CRST_RESERVED_FLAGS_MASK) | (m_dwFlags & CRST_RESERVED_FLAGS_MASK);
    }

    void ResetFlags() // resets the reserved and the CrstFlags
    {
        m_dwFlags = 0;
    }
    // ------------------------------- Holders ------------------------------
 public:

    // Note that the holders for CRSTs are used in extremely low stack conditions. Because of this, they 
    // aren't allowed to use more than HOLDER_CODE_MINIMUM_STACK_LIMIT pages of stack.
    typedef DacHolder<CrstBase *, CrstBase::AcquireLock, CrstBase::ReleaseLock, 0, CompareDefault, HSV_ValidateMinimumStackReq> CrstHolder;

    // We have some situations where we're already holding a lock, and we need to release and reacquire the lock across a window.
    // This is a dangerous construct because the backout code can block.
    // Generally, it's better to use a regular CrstHolder, and then use the Release() / Acquire() methods on it.
    // This just exists to convert legacy OS Critical Section patterns over to holders.
    typedef DacHolder<CrstBase *, CrstBase::ReleaseLock, CrstBase::AcquireLock, 0, CompareDefault, HSV_ValidateMinimumStackReq> UnsafeCrstInverseHolder;
    

    // A popular request is for the Acquire to toggle into Preemptive mode around the call.
    typedef DacHolder<CrstBase *, CrstBase::AcquirePreempLock, CrstBase::ReleaseLock, 0, CompareDefault, HSV_ValidateMinimumStackReq> CrstPreempHolder;
};

typedef CrstBase::CrstHolder CrstHolder;
typedef CrstBase::CrstPreempHolder CrstPreempHolder;


// The CRST.
class Crst : public CrstBase
{
public:
    void *operator new(size_t size)
    {
        WRAPPER_CONTRACT;
        return new BYTE[size];
    }

private:
    void *operator new(size_t size, void *pInPlace);

public:

    enum AcquireImmediatelyFlag
    {
        CRST_ACQUIRE_IMMEDIATELY = 1,
        CRST_NOACQUIRE_IMMEDIATELY = 0,
    };


#ifndef DACCESS_COMPILE

    //-----------------------------------------------------------------
    // Constructor.
    //
    // You can also request that the thread start out holding the lock
    // by passing CRST_ACQUIRE_IMMEDIATELY. Since this particular acquire
    // is guaranteed not to block, you can also optionally disable the crst level
    // check for this initial acquire by passing CRST_NO_LEVEL_CHECK. The
    // PendingLoadEntryLock in particular uses this as it has a legitimate
    // need to violate the level order wrt UnresolvedClassLock when being created.
    //-----------------------------------------------------------------
    Crst(LPCSTR szTag, CrstLevel crstlevel, CrstFlags flags = CRST_DEFAULT, AcquireImmediatelyFlag acquireImmediately = CRST_NOACQUIRE_IMMEDIATELY
#ifdef _DEBUG
        , NoLevelCheckFlag noLevelCheckOnInitialAcquire = CRST_LEVEL_CHECK
#endif
        );

    //-----------------------------------------------------------------
    // Destructor.
    //-----------------------------------------------------------------
    ~Crst()
    {
        WRAPPER_CONTRACT;

        Destroy();
    };

#else

    Crst(LPCSTR szTag, CrstLevel crstlevel, CrstFlags flags = CRST_DEFAULT, AcquireImmediatelyFlag acquireImmediately = CRST_NOACQUIRE_IMMEDIATELY
#ifdef _DEBUG
        , NoLevelCheckFlag noLevelCheckOnInitialAcquire = CRST_LEVEL_CHECK
#endif
        ) {
        LEAF_CONTRACT;
    };

    Crst() {
        LEAF_CONTRACT;
    }




#endif
};


/* to be used as static variable - no constructor/destructor, assumes zero 
   initialized memory */
class CrstStatic : public CrstBase
{
public:
    VOID Init(LPCSTR szTag, CrstLevel crstlevel, CrstFlags flags = CRST_DEFAULT)
    {
        WRAPPER_CONTRACT;

        _ASSERTE((flags & CRST_INITIALIZED) == 0);

        // throw away the debug-only parameters in retail
        InitWorker(INDEBUG_COMMA(szTag) INDEBUG_COMMA(crstlevel) flags);
    }
};

/* to be used as regular variable when a explicit call to Init method is needed */
class CrstExplicitInit : public CrstStatic
{
public:
    CrstExplicitInit() {
        m_dwFlags = 0;
    }
     ~CrstExplicitInit() {
#ifndef DACCESS_COMPILE
        Destroy();
#endif
    }   
};

__inline BOOL IsOwnerOfCrst(LPVOID lock)
{
    WRAPPER_CONTRACT;

#ifdef _DEBUG
    return ((Crst*)lock)->OwnedByCurrentThread();
#else
    // This function should not be called on free build.
    DebugBreak();
    return TRUE;
#endif
}

__inline BOOL IsOwnerOfOSCrst(LPVOID lock)
{
    WRAPPER_CONTRACT;

#ifdef _DEBUG
    volatile static int bOnW95; // = 0
    if (bOnW95==0)
        bOnW95=RunningOnWin95() ? 1 : -1;

    if (bOnW95==1) {
        // We can not determine if the current thread owns CRITICAL_SECTION on Win9x
        return TRUE;
    }
    else {
        CRITICAL_SECTION *pCrit = (CRITICAL_SECTION*)lock;
        return (size_t)pCrit->OwningThread == (size_t) GetCurrentThreadId();
    }
#else
    // This function should not be called on free build.
    DebugBreak();
    return TRUE;
#endif
}

#endif // __crst_h__
