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
#ifndef _SimpleRWLock_hpp_
#define _SimpleRWLock_hpp_

#include "threads.h"

class SimpleRWLock;

enum GC_MODE {
    COOPERATIVE,
    PREEMPTIVE,
    COOPERATIVE_OR_PREEMPTIVE} ;

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(SimpleRWLock*);
template BOOL CompareDefault(SimpleRWLock*,SimpleRWLock*);
typedef Holder<SimpleRWLock*, DoNothing, DoNothing> HACKSimpleRWLockHolder;
#endif

class SimpleRWLock
{
private:
    BOOL IsWriterWaiting()
    {
        LEAF_CONTRACT;
        return m_WriterWaiting != 0;
    }

    void SetWriterWaiting()
    {
        LEAF_CONTRACT;
        m_WriterWaiting = 1;
    }

    void ResetWriterWaiting()
    {
        LEAF_CONTRACT;
        m_WriterWaiting = 0;
    }

    // lock used for R/W synchronization
    volatile LONG                m_RWLock;     

    // Does this lock require to be taken in PreemptiveGC mode?
    const GC_MODE          m_gcMode;

    // spin count for a reader waiting for a writer to release the lock
    LONG                m_spinCount;

    // used to prevent writers from being starved by readers
    // we currently do not prevent writers from starving readers since writers 
    // are supposed to be rare.
    BOOL                m_WriterWaiting;

#ifndef DACCESS_COMPILE
    static void AcquireReadLock(SimpleRWLock *s) { LEAF_CONTRACT; s->EnterRead(); }
    static void ReleaseReadLock(SimpleRWLock *s) { LEAF_CONTRACT; s->LeaveRead(); }

    static void AcquireWriteLock(SimpleRWLock *s) { LEAF_CONTRACT; s->EnterWrite(); }
    static void ReleaseWriteLock(SimpleRWLock *s) { LEAF_CONTRACT; s->LeaveWrite(); }
#else // DACCESS_COMPILE
    static void AcquireReadLock(SimpleRWLock *s);
    static void ReleaseReadLock(SimpleRWLock *s);

    static void AcquireWriteLock(SimpleRWLock *s);
    static void ReleaseWriteLock(SimpleRWLock *s);
#endif // DACCESS_COMPILE

public:
    SimpleRWLock (GC_MODE gcMode, LOCK_TYPE locktype)
        : m_gcMode (gcMode)
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
        } CONTRACTL_END;

        m_RWLock = 0;
        m_spinCount = (GetCurrentProcessCpuCount() == 1) ? 0 : 4000;
        m_WriterWaiting = FALSE;
    }
#ifdef DACCESS_COMPILE
    // Special empty CTOR for DAC. We still need to assign to const fields, but they won't actually be used.
    SimpleRWLock()  
        : m_gcMode(COOPERATIVE_OR_PREEMPTIVE)
    {
        LEAF_CONTRACT;
    }
#endif
    
#ifndef DACCESS_COMPILE
    // Acquire the reader lock.
    BOOL TryEnterRead();
    void EnterRead();

    // Acquire the writer lock.
    BOOL TryEnterWrite();
    void EnterWrite();

    // Leave the reader lock.
    void LeaveRead()
    {
        LEAF_CONTRACT;
        LONG RWLock = InterlockedDecrement(&m_RWLock);
        _ASSERTE (RWLock >= 0);
        DECTHREADLOCKCOUNT();
    }

    // Leave the writer lock.
    void LeaveWrite()
    {
        LEAF_CONTRACT;        
        LONG RWLock = InterlockedExchange (&m_RWLock, 0);
        _ASSERTE(RWLock == -1);
        DECTHREADLOCKCOUNT();
    }
#endif // DACCESS_COMPILE

    typedef DacHolder<SimpleRWLock *, SimpleRWLock::AcquireReadLock, SimpleRWLock::ReleaseReadLock> SimpleReadLockHolder;
    typedef DacHolder<SimpleRWLock *, SimpleRWLock::AcquireWriteLock, SimpleRWLock::ReleaseWriteLock> SimpleWriteLockHolder;

#ifdef _DEBUG
    BOOL LockTaken ()
    {
        LEAF_CONTRACT;
        return m_RWLock != 0;
    }

    BOOL IsReaderLock ()
    {
        LEAF_CONTRACT;
        return m_RWLock > 0;
    }

    BOOL IsWriterLock ()
    {
        LEAF_CONTRACT;
        return m_RWLock < 0;
    }
    
#endif    
};

typedef SimpleRWLock::SimpleReadLockHolder SimpleReadLockHolder;
typedef SimpleRWLock::SimpleWriteLockHolder SimpleWriteLockHolder;

#endif
