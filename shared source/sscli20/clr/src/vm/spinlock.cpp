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
// spinlock.cpp
//

#include "common.h"

#include "list.h"
#include "spinlock.h"
#include "threads.h"
#include "corhost.h"

enum
{
	BACKOFF_LIMIT = 1000		// used in spin to acquire
};

#ifdef _DEBUG

	// profile information
ULONG	SpinLockProfiler::s_ulBackOffs = 0;
ULONG	SpinLockProfiler::s_ulCollisons [LOCK_TYPE_DEFAULT + 1] = { 0 };
ULONG	SpinLockProfiler::s_ulSpins [LOCK_TYPE_DEFAULT + 1] = { 0 };

#endif

SpinLock::SpinLock()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    m_hostLock = NULL;
    m_Initialized = UnInitialized;
}


SpinLock::~SpinLock()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (CLRSyncHosted() && m_hostLock) {
        m_hostLock->Release();
        m_hostLock = NULL;
    }
}

void SpinLock::Init(LOCK_TYPE type, bool RequireCoopGC)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;

        // Disallow creation of Crst before EE initialization has begun.
        PRECONDITION(g_fEEInit || g_fEEStarted);
    }
    CONTRACTL_END;

    if (m_Initialized == Initialized)
    {
        _ASSERTE (type == m_LockType);
        _ASSERTE (RequireCoopGC == m_requireCoopGCMode);

        // We have initialized this spinlock.
        return;
    }

    while (TRUE)
    {
        LONG curValue = FastInterlockCompareExchange((LONG*)&m_Initialized, BeingInitialized, UnInitialized);
        if (curValue == Initialized)
        {
            return;
        }
        else if (curValue == UnInitialized)
        {
            // We are the first to initialize the lock
            break;
        }
        else
        {
            __SwitchToThread(10);
        }
    }

    IHostSyncManager *pManager = CorHost2::GetHostSyncManager();
    _ASSERTE((pManager == NULL && m_lock == 0) ||
             (pManager && m_hostLock == NULL));

    if (pManager == 0) {
		m_lock = 0;		
    }
    else {
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pManager->CreateCrst(&m_hostLock);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (hr != S_OK) {
            _ASSERTE (hr == E_OUTOFMEMORY);
            _ASSERTE (m_Initialized == BeingInitialized);
            m_Initialized = UnInitialized;
            ThrowOutOfMemory();
        }
    }

#ifdef _DEBUG
    m_LockType = type;
    m_requireCoopGCMode = RequireCoopGC;
#endif

    _ASSERTE (m_Initialized == BeingInitialized);
    m_Initialized = Initialized;
}

#ifdef _DEBUG
BOOL SpinLock::OwnedByCurrentThread()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return m_holdingThreadId.IsSameThread();
}
#endif

void SpinLock::AcquireLock(SpinLock *s)
{
    CONTRACTL
    {
        DISABLED(THROWS);  // Need to rewrite spin locks.
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    s->GetLock(); 
}

void SpinLock::ReleaseLock(SpinLock *s) 
{ 
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    s->FreeLock(); 
}


void SpinLock::GetLock()
{
    CONTRACTL
    {
        DISABLED(THROWS);  // need to rewrite spin locks to no-throw.
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(m_Initialized == Initialized);

#ifdef _DEBUG
    dbg_PreEnterLock();
#endif

    if (!CLRSyncHosted()) {
        if (!GetLockNoWait()) {
            SpinToAcquire();
        }
    }
    else {
        DWORD option = WAIT_NOTINDEADLOCK;
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = m_hostLock->Enter(option);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        _ASSERTE(hr == S_OK);
    }
    INCTHREADLOCKCOUNT();
#ifdef _DEBUG
    m_holdingThreadId.SetThreadId();
    dbg_EnterLock();
#endif
}

//----------------------------------------------------------------------------
// SpinLock::GetLockNoWait   
// used interlocked exchange and fast lock acquire

BOOL SpinLock::GetLockNoWait()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (!CLRSyncHosted())
    {
        if (m_lock == 0 && FastInterlockExchange (&m_lock, 1) == 0)
        {
            return 1;
        }
        return 0;
    }
    else {
        BOOL result;
        HRESULT hr;
        hr = m_hostLock->TryEnter(WAIT_NOTINDEADLOCK, &result);
        _ASSERTE(hr == S_OK);
        return result;
    }
}

//----------------------------------------------------------------------------
// SpinLock::FreeLock   
//  Release the spinlock
//
void
SpinLock::FreeLock ()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

	_ASSERTE(m_Initialized == Initialized);

#ifdef _DEBUG
    _ASSERTE(OwnedByCurrentThread());
    m_holdingThreadId.ResetThreadId();
    dbg_LeaveLock();
#endif

    if (!CLRSyncHosted()) {
    	// else uses interlocked exchange.
    	//
    	FastInterlockExchange (&m_lock, 0);
    }
    else {
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = m_hostLock->Leave();
        END_SO_TOLERANT_CODE_CALLING_HOST;
        _ASSERTE (hr == S_OK);
    }
    DECTHREADLOCKCOUNT();

} // SpinLock::FreeLock ()


//----------------------------------------------------------------------------
// SpinLock::SpinToAcquire   , non-inline function, called from inline Acquire
//  
//  Spin waiting for a spinlock to become free.
//
//  
void
SpinLock::SpinToAcquire ()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE (!CLRSyncHosted());

	ULONG				ulBackoffs = 0;
	ULONG				ulSpins = 0;

	while (true)
	{
		for (unsigned i = ulSpins+10000;
			 ulSpins < i;
			 ulSpins++)
		{
			// Note: Must cast through volatile to ensure the lock is
			// refetched from memory.
			//
			if (*((volatile LONG*)&m_lock) == 0)
			{
				break;
			}
			YieldProcessor();			// indicate to the processor that we are spining 
		}

		// Try the inline atomic test again.
		//
		if (GetLockNoWait ())
		{
			break;
		}

        //backoff
        ulBackoffs++;

		if ((ulBackoffs % BACKOFF_LIMIT) == 0)
		{	
			__SwitchToThread (500);
		}
		else
        {
			__SwitchToThread (0);
        }
	}

#ifdef _DEBUG
		//profile info
	SpinLockProfiler::IncrementCollisions (m_LockType);
	SpinLockProfiler::IncrementSpins (m_LockType, ulSpins);
	SpinLockProfiler::IncrementBackoffs (ulBackoffs);
#endif

} // SpinLock::SpinToAcquire ()

#ifdef _DEBUG
// If a GC is not allowed when we enter the lock, we'd better not do anything inside
// the lock that could provoke a GC.  Otherwise other threads attempting to block
// (which are presumably in the same GC mode as this one) will block.  This will cause
// a deadlock if we do attempt a GC because we can't suspend blocking threads and we
// can't release the spin lock.
void SpinLock::dbg_PreEnterLock()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    Thread* pThread = GetThread();

    // SpinLock can only be used by managed thread
    _ASSERTE (pThread);
    // SpinLock can not be nested.
    _ASSERTE ((pThread->m_StateNC & Thread::TSNC_OwnsSpinLock) == 0);

    pThread->SetThreadStateNC(Thread::TSNC_OwnsSpinLock);

    if (!pThread->PreemptiveGCDisabled())
        _ASSERTE(!m_requireCoopGCMode);
}

void SpinLock::dbg_EnterLock()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    INCONTRACT(Thread  *pThread = GetThread());
    INCONTRACT(pThread->BeginNoTriggerGC(__FILE__, __LINE__));
}

void SpinLock::dbg_LeaveLock()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    Thread  *pThread = GetThread();
    pThread->ResetThreadStateNC(Thread::TSNC_OwnsSpinLock);

    INCONTRACT(pThread->EndNoTriggerGC());
}


void SpinLockProfiler::InitStatics ()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    s_ulBackOffs = 0;
    memset (s_ulCollisons, 0, sizeof (s_ulCollisons));
    memset (s_ulSpins, 0, sizeof (s_ulSpins));
}

void SpinLockProfiler::IncrementSpins (LOCK_TYPE type, ULONG value)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(type <= LOCK_TYPE_DEFAULT);
    s_ulSpins [type] += value;
}

void SpinLockProfiler::IncrementCollisions (LOCK_TYPE type)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    ++s_ulCollisons [type];
}

void SpinLockProfiler::IncrementBackoffs (ULONG value)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    s_ulBackOffs += value;
}

void SpinLockProfiler::DumpStatics()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

}

#endif

// End of file: spinlock.cpp
