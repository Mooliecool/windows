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
//----------------------------------------------------------------------------
//  spinlock.h , defines the spin lock class and a profiler class
//      
//----------------------------------------------------------------------------


//#ifndef _H_UTIL
//#error I am a part of util.hpp Please don't include me alone !
//#endif


#ifndef _H_SPINLOCK_
#define _H_SPINLOCK_

#include <stddef.h>

class SpinLock;

// Lock Types, used in profiling
//
enum LOCK_TYPE
{
    LOCK_PLUSWRAPPER_CACHE = 1,  // change
    LOCK_FCALL = 2,              // leave, but rank to tip
    LOCK_COMCTXENTRYCACHE = 3,   // creates events, allocs memory, SEH, etc.
    LOCK_REFLECTCACHE = 5,
    LOCK_CORMAP = 7,
    LOCK_TYPE_DEFAULT  = 8
};

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(SpinLock*);
template BOOL CompareDefault(SpinLock*,SpinLock*);
typedef Holder<SpinLock*, DoNothing, DoNothing> HACKSpinLockHolder;
#endif

//----------------------------------------------------------------------------
// class: Spinlock 
//
// PURPOSE:
//   spinlock class that contains constructor and out of line spinloop.
//
//----------------------------------------------------------------------------
class SpinLock
{

private:
    union {
        // m_lock has to be the fist data member in the class
        volatile LONG       m_lock;     // LONG used in interlocked exchange
        IHostCrst          *m_hostLock;
    };

    enum SpinLockState
    {
        UnInitialized,
        BeingInitialized,
        Initialized
    };

    volatile SpinLockState       m_Initialized; // To verify initialized
                                        // And initialize once

#ifdef _DEBUG
    LOCK_TYPE           m_LockType;     // lock type to track statistics
    
    // Check for dead lock situation.
    bool                m_requireCoopGCMode;
    EEThreadId          m_holdingThreadId;
#endif

public:
    SpinLock ();
    ~SpinLock ();

    //Init method, initialize lock and _DEBUG flags
    void Init(LOCK_TYPE type, bool RequireCoopGC = FALSE);

    //-----------------------------------------------------------------
    // Is the current thread the owner?
    //-----------------------------------------------------------------
#ifdef _DEBUG
    BOOL OwnedByCurrentThread();
#endif

private:
    void SpinToAcquire (); // out of line call spins 
    
#ifdef _DEBUG
    void dbg_PreEnterLock();
    void dbg_EnterLock();
    void dbg_LeaveLock();
#endif

    // The following 5 APIs must remain private.  We want all entry/exit code to
    // occur via holders, so that exceptions will be sure to release the lock.
private:
    void GetLock ();        // Acquire lock, blocks if unsuccessful
    BOOL GetLockNoWait();   // Acquire lock, fail-fast 
    void FreeLock ();       // Release lock
    
    static void AcquireLock(SpinLock *s);
    static void ReleaseLock(SpinLock *s);

public:
    typedef Holder<SpinLock *, SpinLock::AcquireLock, SpinLock::ReleaseLock> Holder;
};


typedef SpinLock::Holder SpinLockHolder;
#define TAKE_SPINLOCK_IN_COOP_AND_FORBID_GC(lock) \
                        GCX_COOP();\
                        SpinLockHolder __spinLockHolder(lock);\
                        GCX_FORBID ();

__inline BOOL IsOwnerOfSpinLock (LPVOID lock)
{
#ifdef _DEBUG
    return ((SpinLock*)lock)->OwnedByCurrentThread();
#else
    // This function should not be called on free build.
    DebugBreak();
    return TRUE;
#endif
}

#ifdef _DEBUG
//----------------------------------------------------------------------------
// class SpinLockProfiler 
//  to track contention, useful for profiling
//
//----------------------------------------------------------------------------
class SpinLockProfiler
{
    // Pointer to spinlock names.
    //
    static ULONG    s_ulBackOffs;
    static ULONG    s_ulCollisons [LOCK_TYPE_DEFAULT + 1];
    static ULONG    s_ulSpins [LOCK_TYPE_DEFAULT + 1];

public:

    static void InitStatics ();

    static void IncrementSpins (LOCK_TYPE type, ULONG value);

    static void IncrementCollisions (LOCK_TYPE type);

    static void IncrementBackoffs (ULONG value);

    static void DumpStatics();

};

#endif  // ifdef _DEBUG
#endif //  ifndef _H_SPINLOCK_
