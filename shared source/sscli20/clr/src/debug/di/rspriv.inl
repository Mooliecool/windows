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
// File: cordb.inl
//
// Inline functions for cordb.h
//
//*****************************************************************************

#ifndef RSPRIV_INL_
#define RSPRIV_INL_

#include "rspriv.h"


//-----------------------------------------------------------------------------
// CordbCode builders - make it obvious if the caller is building
// native or IL code.
//-----------------------------------------------------------------------------
inline
CordbCode * CordbCode::BuildILCode(
    CordbFunction *m, REMOTE_PTR startAddress,
    SIZE_T size, SIZE_T nVersion
)
{
    return new (nothrow) CordbCode(m, startAddress, size, nVersion);
}

inline
CordbCode * CordbCode::BuildNativeCode(
    CordbFunction *m, DebuggerIPCE_JITFuncData * pJitData
)
{
    CordbCode * pCode = new (nothrow) CordbCode(m, pJitData);

#if defined(_DEBUG)
    {
        CORDB_ADDRESS pStart = NULL;
        HRESULT hr2 = pCode->GetAddress(&pStart);
        SIMPLIFYING_ASSUMPTION(SUCCEEDED(hr2));

        CheckAgainstDAC(m, CORDB_ADDRESS_TO_PTR(pStart), m->m_token);
    }
#endif

    return pCode;
}


inline
CordbModule *CordbJITILFrame::GetModule(void)
{
    return m_ilCode->m_function->GetModule();
}

inline
CordbAppDomain *CordbJITILFrame::GetCurrentAppDomain(void)
{
    return (m_nativeFrame->GetCurrentAppDomain());
}



inline
CordbFunction *CordbJITILFrame::GetFunction(void)
{
    return m_nativeFrame->m_jitinfo->m_function;
}

//-----------------------------------------------------------------------------
// Helpers to assert threading semantics.
//-----------------------------------------------------------------------------
inline bool IsWin32EventThread(CordbProcess * p)
{
    _ASSERTE(p!= NULL);
    return p->IsWin32EventThread();
}

inline bool IsRCEventThread(Cordb* p)
{
    _ASSERTE(p!= NULL);
    return (p->m_rcEventThread != NULL) && p->m_rcEventThread->IsRCEventThread();
}

inline bool CordbProcess::IsWin32EventThread()
{
    return (m_win32EventThread != NULL) && m_win32EventThread->IsWin32EventThread();
}


//-----------------------------------------------------------------------------
// StopContinueHolder. Ensure that we're synced during a certain region.
//-----------------------------------------------------------------------------
inline HRESULT StopContinueHolder::Init(CordbProcess * p)
{
    _ASSERTE(p != NULL);
    LOG((LF_CORDB, LL_INFO100000, "Doing RS internal Stop\n"));
    HRESULT hr = p->StopInternal(INFINITE, LSPTR_APPDOMAIN::NullPtr());
    if ((hr == CORDBG_E_PROCESS_TERMINATED) || SUCCEEDED(hr))
    {
        // Better be synced after calling Stop!
        _ASSERTE(p->GetSynchronized());
        m_p = p;
    }

    return hr;
};

inline StopContinueHolder::~StopContinueHolder()
{
    // If Init() failed to call Stop, then don't call continue
    if (m_p == NULL)
        return;

    HRESULT hr;
    LOG((LF_CORDB, LL_INFO100000, "Doing RS internal Continue\n"));
    hr = m_p->ContinueInternal(false);
    SIMPLIFYING_ASSUMPTION(
        (hr == CORDBG_E_PROCESS_TERMINATED) ||
        (hr == CORDBG_E_PROCESS_DETACHED) ||
        (hr == CORDBG_E_OBJECT_NEUTERED) ||
        SUCCEEDED(hr));
}

//-----------------------------------------------------------------------------
// Neutering on the base object
//-----------------------------------------------------------------------------
inline
void CordbCommonBase::Neuter(NeuterTicket ticket)
{
    LOG((LF_CORDB, LL_EVERYTHING, "Memory: CordbBase object neutered: this=%p, id=%p\n", this, m_id));
    m_fIsNeutered = true;

#ifdef RSCONTRACTS
    // The root Cordb object doesn't have process affinity.
    if (this->m_type != enumCordb)
    {
        // All non-Cordb neutering should either be under the Stop-Go-Lock;
        // or on the W32EventThread (which will be during an ExitProcess callback)
        // However, asserting this may be tough b/c we no longer have access to our parent Process.
        // if our parent neuter set our refs to NULL.
        DbgRSThread * pThread = DbgRSThread::GetThread();
        _ASSERTE(pThread->IsSafeToNeuter());
    }
#endif
}

// Unsafe neuter for an object that's already dead. Only use this if you know exactly what you're doing.
// The point here is that we can mark the object neutered even though we may not hold the stop-go lock.
inline
void CordbCommonBase::UnsafeNeuterDeadObject()
{
    LOG((LF_CORDB, LL_EVERYTHING, "Memory: CordbBase object neutered: this=%p, id=%p\n", this, m_id));
    m_fIsNeutered = true;
}


//-----------------------------------------------------------------------------
// Reference Counting
//-----------------------------------------------------------------------------
inline
void CordbCommonBase::InternalAddRef()
{
    CONSISTENCY_CHECK_MSGF((m_RefCount & CordbBase_InternalRefCountMask) != (CordbBase_InternalRefCountMax),
        ("Internal AddRef overlow, External Count = %d,\n'%s' @ 0x%p",
        (m_RefCount >> CordbBase_ExternalRefCountShift), this->DbgGetName(), this));

    // Since the internal ref-count is the lower bits, and we know we'll never overflow ;)
    // we can just do an interlocked increment on the whole 32 bits.
#ifdef TRACK_OUTSTANDING_OBJECTS
    MixedRefCountUnsigned Count =
#endif

    InterlockedIncrement64((MixedRefCountSigned*) &m_RefCount);


#ifdef _DEBUG_IMPL

    // For leak detection in debug builds, track all internal references.
    InterlockedIncrement(&Cordb::s_DbgMemTotalOutstandingInternalRefs);
#endif

#ifdef TRACK_OUTSTANDING_OBJECTS
    if ((Count & CordbBase_InternalRefCountMask) != 1)
    {
        return;
    }

    LONG i;

    for (i = 0; i < Cordb::s_DbgMemOutstandingObjectMax; i++)
    {
        if (Cordb::s_DbgMemOutstandingObjects[i] == NULL)
        {
            if (InterlockedCompareExchangePointer(&(Cordb::s_DbgMemOutstandingObjects[i]), this, NULL) == NULL)
            {
                return;
            }
        }
    }

    do
    {
        i = Cordb::s_DbgMemOutstandingObjectMax + 1;
    }
    while ((i < MAX_TRACKED_OUTSTANDING_OBJECTS) &&
           (InterlockedCompareExchange(&Cordb::s_DbgMemOutstandingObjectMax, i, i - 1) != (i - 1)));

    if (i < MAX_TRACKED_OUTSTANDING_OBJECTS)
    {
        Cordb::s_DbgMemOutstandingObjects[i] = this;
    }
#endif

}

// Derived versions of AddRef / Release will call these.
// External AddRef.
inline
ULONG CordbCommonBase::BaseAddRef()
{
    volatile MixedRefCountUnsigned ref;
    MixedRefCountUnsigned refNew;
    ExternalRefCount cExternalCount;

    // Compute what refNew ought to look like; and then If m_RefCount hasn't changed on us
    // (via another thread), then stash the new one in.
    do
    {
        ref = m_RefCount;

        cExternalCount = (ExternalRefCount) (ref >> CordbBase_ExternalRefCountShift);

        if (cExternalCount == CordbBase_InternalRefCountMax)
        {
            CONSISTENCY_CHECK_MSGF(false, ("Overflow in External AddRef. Internal Count =%d,\n'%s' @ 0x%p",
                (ref & CordbBase_InternalRefCountMask), this->DbgGetName(), this));

            return cExternalCount;
        }

        cExternalCount++;

        refNew = (((MixedRefCountUnsigned)cExternalCount) << CordbBase_ExternalRefCountShift) | (ref & CordbBase_InternalRefCountMask);
    }
    while ((MixedRefCountUnsigned)InterlockedCompareExchange64((MixedRefCountSigned*)&m_RefCount, refNew, ref) != ref);

    return cExternalCount;
}

// Do an AddRef against the External count. This is a semantics issue.
// We use this when an internal component Addrefs out-parameters (which Cordbg will call Release on).
inline
void CordbCommonBase::ExternalAddRef()
{
    // Call on BaseAddRef() to avoid any asserts that prevent stuff from inside the RS from bumping
    // up the external ref count.
    BaseAddRef();
}

inline
void CordbCommonBase::InternalRelease()
{
    CONSISTENCY_CHECK_MSGF((m_RefCount & CordbBase_InternalRefCountMask) != 0,
        ("Internal Release underflow, External Count = %d,\n'%s' @ 0x%p",
        (m_RefCount >> CordbBase_ExternalRefCountShift), this->DbgGetName(), this));

#ifdef _DEBUG_IMPL
    // For leak detection in debug builds, track all internal references.
    InterlockedDecrement(&Cordb::s_DbgMemTotalOutstandingInternalRefs);
#endif



    // The internal count is in the low 16 bits, and we know that we'll never underflow the internal
    // release. ;)
    // Furthermore we know that ExternalRelease  will prevent us from underflowing the external release count.
    // Thus we can just do an simple decrement here, and compare against 0x00000000 (which is the value
    // when both the Internal + External counts are at 0)
    MixedRefCountSigned cRefCount = InterlockedDecrement64((MixedRefCountSigned*) &m_RefCount);

#ifdef TRACK_OUTSTANDING_OBJECTS
    if ((cRefCount & CordbBase_InternalRefCountMask) == 0)
    {
        for (LONG i = 0; i < Cordb::s_DbgMemOutstandingObjectMax; i++)
        {
            if (Cordb::s_DbgMemOutstandingObjects[i] == this)
            {
                Cordb::s_DbgMemOutstandingObjects[i] = NULL;
                break;
            }
        }
    }
#endif


    if (cRefCount == 0x00000000)
    {
        delete this;
    }
}

// Do an external release.
inline
ULONG CordbCommonBase::BaseRelease()
{
    volatile MixedRefCountUnsigned ref;
    MixedRefCountUnsigned refNew;
    ExternalRefCount cExternalCount;

    // Compute what refNew ought to look like; and then If m_RefCount hasn't changed on us
    // (via another thread), then stash the new one in.
    do
    {
        ref = m_RefCount;

        cExternalCount = (ExternalRefCount) (ref >> CordbBase_ExternalRefCountShift);

        if (cExternalCount == 0)
        {
            CONSISTENCY_CHECK_MSGF(false, ("Underflow in External Release. Internal Count = %d\n'%s' @ 0x%p",
                (ref & CordbBase_InternalRefCountMask), this->DbgGetName(), this));

            return 0;
        }

        cExternalCount--;

        refNew = (((MixedRefCountUnsigned) cExternalCount) << CordbBase_ExternalRefCountShift) | (ref & CordbBase_InternalRefCountMask);
    }
    while ((MixedRefCountUnsigned)InterlockedCompareExchange64((MixedRefCountSigned*)&m_RefCount, refNew, ref) != ref);

    // If the external count just dropped to 0, then this object can be neutered.
    if (cExternalCount == 0)
    {
        m_fNeuterAtWill = true;
    }

    if (refNew == 0)
    {
        delete this;
        return 0;
    }
    return cExternalCount;

}


inline ULONG CordbCommonBase::BaseAddRefEnforceExternal()
{
    // External refs shouldn't be called while in the RS
#ifdef RSCONTRACTS   
    DbgRSThread * pThread = DbgRSThread::GetThread();
    CONSISTENCY_CHECK_MSGF(!pThread->IsInRS(), 
        ("External addref for pThis=0x%p, name='%s' called from within RS", 
            this, this->DbgGetName()
        ));
#endif
    return (BaseAddRef());

}

inline ULONG CordbCommonBase::BaseReleaseEnforceExternal()
{
#ifdef RSCONTRACTS   
    DbgRSThread * pThread = DbgRSThread::GetThread();
    
    CONSISTENCY_CHECK_MSGF(!pThread->IsInRS(), 
        ("External release for pThis=0x%p, name='%s' called from within RS", 
            this, this->DbgGetName()
        ));
#endif

    return (BaseRelease());
}



//-----------------------------------------------------------------------------
// Locks
//-----------------------------------------------------------------------------

// Base class
#ifdef _DEBUG
inline bool RSLock::HasLock()
{
    CONSISTENCY_CHECK_MSGF(IsInit(), ("RSLock '%s' not inited", m_szTag));
    return m_tidOwner == ::GetCurrentThreadId();
}
#endif

#ifdef _DEBUG
// Ctor+  Dtor are only used for asserts.
inline RSLock::RSLock()
{
    m_eAttr = cLockUninit;
    m_tidOwner = (DWORD)-1;
};

inline RSLock::~RSLock()
{
    // If this lock is still ininitialized, then no body ever deleted the critical section
    // for it and we're leaking.
    CONSISTENCY_CHECK_MSGF(!IsInit(), ("Leaking Critical section for RS Lock '%s'", m_szTag));
}
#endif


// Initialize a lock.
inline void RSLock::Init(const char * szTag, ELockAttr eAttr, ERSLockLevel level)
{
    CONSISTENCY_CHECK_MSGF(!IsInit(), ("RSLock '%s' already inited", szTag));
#ifdef _DEBUG
    m_szTag = szTag;
    m_eAttr = eAttr;
    m_count = 0;
    m_level = level;

    // Must be either re-entrant xor flat. (not neither; not both)
    _ASSERTE(IsReentrant() ^ ((m_eAttr & cLockFlat) == cLockFlat));
#endif
    _ASSERTE((level >= 0) && (level <= RSLock::LL_MAX));

    _ASSERTE(IsInit());

    InitializeCriticalSection(&m_lock);
}

// Cleanup a lock.
inline void RSLock::Destroy()
{
    CONSISTENCY_CHECK_MSGF(IsInit(), ("RSLock '%s' not inited", m_szTag));
    DeleteCriticalSection(&m_lock);

#ifdef _DEBUG
    m_eAttr = cLockUninit; // No longer initialized.
    _ASSERTE(!IsInit());
#endif
}

inline void RSLock::Lock()
{
    CONSISTENCY_CHECK_MSGF(IsInit(), ("RSLock '%s' not inited", m_szTag));

#ifdef RSCONTRACTS
    DbgRSThread * pThread = DbgRSThread::GetThread();
    pThread->NotifyTakeLock(this);
#endif

    EnterCriticalSection(&m_lock);
#ifdef _DEBUG
    m_tidOwner = ::GetCurrentThreadId();
    m_count++;

    // Either count == 1 or we're re-entrant.
    _ASSERTE((m_count == 1) || (m_eAttr == cLockReentrant));
#endif
}

inline void RSLock::Unlock()
{
    CONSISTENCY_CHECK_MSGF(IsInit(), ("RSLock '%s' not inited", m_szTag));

#ifdef _DEBUG
    _ASSERTE(HasLock());
    m_count--;
    _ASSERTE(m_count >= 0);
    if (m_count == 0)
    {
        m_tidOwner = (DWORD)-1;
    }
#endif

#ifdef RSCONTRACTS
    // NotifyReleaseLock needs to be called before we release the lock.
    // Note that HasLock()==false at this point. NotifyReleaseLock relies on that.
    DbgRSThread * pThread = DbgRSThread::GetThread();
    pThread->NotifyReleaseLock(this);
#endif

    LeaveCriticalSection(&m_lock);
}

template <class T>
inline T* CordbSafeHashTable<T>::GetBase(ULONG_PTR id, BOOL fFab)
{
    return static_cast<T*>(UnsafeGetBase(id, fFab));
}

#endif  // RSPRIV_INL_
