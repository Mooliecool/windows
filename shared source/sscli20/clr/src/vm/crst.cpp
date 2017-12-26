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
/*  CRST.CPP
 *
 */

#include "common.h"

#include "crst.h"
#include "log.h"
#include "corhost.h"

// We need to know if we're on the helper thread.  We need this header for g_pDebugInterface.
#include "dbginterface.h"

//------------------------------------------------------------------------
// This generates global variable declaration for each of our CrstLevels, e.g.
//
//  extern UINT CrstFusionLogRanking;
//  extern UINT CrstFusionTreeRanking;
//  extern UINT CrstFusionForestRanking;
//  extern UINT CrstFusionInvasionOfTheTreePeopleRanking;
//   ...
//
// The actual initializations are done manually at the bottom of this file.
//
// We use global variables rather than enums here because we also want to be
// able to tweak these rankings using the registry.
//------------------------------------------------------------------------
#ifdef _DEBUG
#define DEFINE_CRST_LEVEL(name) extern UINT name##Ranking;
#include "crstlevels.h"
#undef DEFINE_CRST_LEVEL

UINT GetRankOfCrstLevel(CrstLevel level);

#define CRSTUNORDERED ((UINT)(-1))

#endif


//-----------------------------------------------------------------
// Initialize critical section
//-----------------------------------------------------------------
VOID CrstBase::InitWorker(INDEBUG_COMMA(LPCSTR szTag) INDEBUG_COMMA(CrstLevel crstlevel) CrstFlags flags)
{
    CONTRACTL {
        THROWS;
        WRAPPER(GC_TRIGGERS);
    } CONTRACTL_END;    

    // Disallow creation of Crst before EE starts.  But only complain if we end up
    // being hosted, since such Crsts have escaped the hosting net and will cause
    // AVs on next use.
#ifdef _DEBUG
    static bool fEarlyInit; // = false

    if (!g_fEEStarted)
    {
        if (!CLRSyncHosted())
            fEarlyInit = true;
    }

    // If we are now hosted, we better not have *ever* created some Crsts that are
    // not known to our host.
    _ASSERTE(!fEarlyInit || !CLRSyncHosted());

#endif

    _ASSERTE((flags & CRST_INITIALIZED) == 0);
    
    IHostSyncManager *pSyncManager = CorHost2::GetHostSyncManager();
    if (pSyncManager) {
        ResetOSCritSec ();
    }
    else {
        SetOSCritSec ();
    }
            

    if (IsOSCritSec()) {
        UnsafeInitializeCriticalSection(&m_criticalsection);
    }
    else {
        IHostCrst *pHostCrst;
        PREFIX_ASSUME(pSyncManager != NULL);
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pSyncManager->CreateCrst(&pHostCrst);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        if (hr != S_OK) {
            _ASSERTE(hr == E_OUTOFMEMORY);
            ThrowOutOfMemory();
        }
        m_pHostCrst = pHostCrst;
    }

    SetFlags(flags);
    SetCrstInitialized();

#ifdef _DEBUG
    DebugInit(szTag, crstlevel, flags);
#endif
}

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
Crst::Crst(LPCSTR szTag, CrstLevel crstlevel, CrstFlags flags /* = CRST_DEFAULT*/, AcquireImmediatelyFlag acquireImmediately /* = CRST_NOACQUIRE_IMMEDIATELY*/
#ifdef _DEBUG
    , NoLevelCheckFlag noLevelCheckOnInitialAcquire/* = CRST_LEVEL_CHECK*/
#endif
    )
{
    WRAPPER_CONTRACT;

    InitWorker(INDEBUG_COMMA(szTag) INDEBUG_COMMA(crstlevel) flags);

    if (acquireImmediately == CRST_ACQUIRE_IMMEDIATELY)
    {
        class CleanupInException
        {
        private:
            BOOL m_fCleanup;
            Crst *m_pCrst;
        public:
            CleanupInException(Crst* pcrst)
            :m_fCleanup(TRUE), m_pCrst(pcrst)
            {}
            ~CleanupInException()
            {
                if (m_fCleanup)
                {
                    m_pCrst->Destroy();
                }
            }
            void SuppressRelease()
            {
                m_fCleanup = FALSE;
            }
        };
        CleanupInException cleanup(this);
        Enter(INDEBUG(noLevelCheckOnInitialAcquire));
        cleanup.SuppressRelease();
    }
}



#endif



//-----------------------------------------------------------------
// Clean up critical section
//-----------------------------------------------------------------
void CrstBase::Destroy()
{
    WRAPPER_CONTRACT;

    // nothing to do if not initialized
    if (!IsCrstInitialized())
        return;

    // If this assert fired, a crst got deleted while some thread
    // still owned it.  This can happen if the process detaches from
    // our DLL.
#ifdef _DEBUG
    EEThreadId holderthreadid = m_holderthreadid;
    _ASSERTE(holderthreadid.IsUnknown() || g_fProcessDetach || g_fEEShutDown);
#endif

    // If a lock is host breakable, a host is required to block the release call until
    // deadlock detection is finished.
    GCPreemp __gcHolder(m_dwFlags & CRST_HOST_BREAKABLE);

    if (IsOSCritSec()) {
        UnsafeDeleteCriticalSection(&m_criticalsection);
    }
    else {
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        m_pHostCrst->Release();
        END_SO_TOLERANT_CODE_CALLING_HOST;
    }

    LOG((LF_SYNC, INFO3, "Deleting 0x%x\n", this));
#ifdef _DEBUG
    DebugDestroy();
#endif

    ResetFlags();
}

extern void WaitForEndOfShutdown();

//-----------------------------------------------------------------
// If we're in shutdown (as determined by caller since each lock needs its
// own shutdown flag) and this is a non-special thread (not helper/finalizer/shutdown),
// then release the crst and block forever.
// See the prototype for more details.
//-----------------------------------------------------------------
void CrstBase::ReleaseAndBlockForShutdownIfNotSpecialThread()
{
    CONTRACTL {
        NOTHROW;

        // We're almost always MODE_PREEMPTIVE, but if it's a thread suspending for GC, 
        // then we might be MODE_COOPERATIVE. Fortunately in that case, we don't block on shutdown.
        // We assert this below.
        MODE_ANY;
        GC_NOTRIGGER;
        
        PRECONDITION(this->OwnedByCurrentThread());
    }
    CONTRACTL_END;

    if (
        (((size_t)ClrFlsGetValue (TlsIdx_ThreadType)) & (ThreadType_Finalizer|ThreadType_DbgHelper|ThreadType_Shutdown)) == 0)
    {
#ifdef _DEBUG
        Thread * pCurThread = ::GetThread();
#endif // _DEBUG
        
        // The process is shutting down. Release the lock and just block forever.
        this->Leave();

        // If we're actually going to block, then we'd better be in preemptive mode.
        _ASSERTE (pCurThread == NULL || !pCurThread->PreemptiveGCDisabled());


        WaitForEndOfShutdown();
        __SwitchToThread(INFINITE);
        _ASSERTE (!"Can not reach here");
    }
}


//-----------------------------------------------------------------
// Acquire the lock.
//-----------------------------------------------------------------
void CrstBase::Enter(INDEBUG(NoLevelCheckFlag noLevelCheckFlag/* = CRST_LEVEL_CHECK*/))
{
    // Enter() has different behaviors depending on hostbreakable flags, etc.
    SCAN_IGNORE_THROW;
    SCAN_IGNORE_FAULT;
    SCAN_IGNORE_TRIGGER;

    _ASSERTE(IsCrstInitialized());

    Thread *pThread = GetThread();
    BOOL fToggle = (pThread &&  
                    (pThread->PreemptiveGCDisabled() && 
                     (m_dwFlags & (CRST_UNSAFE_ANYMODE | CRST_UNSAFE_COOPGC)) == 0));
    if (fToggle) {
        pThread->EnablePreemptiveGC();
    }
#ifdef THREAD_DELAY
    if (g_pConfig !=NULL && g_pConfig->m_pThreadDelay != NULL) 
    {
        if (g_pConfig->m_pThreadDelay->SpikeOn ())
            g_pConfig->m_pThreadDelay->SpikeDelay ();
        if (g_pConfig->m_pThreadDelay->DelayOn ())
            g_pConfig->m_pThreadDelay->ShortDelay ();
    }
#endif //THREAD_DELAY

#ifdef _DEBUG
    PreEnter ();
#endif

    _ASSERTE(noLevelCheckFlag == CRST_NO_LEVEL_CHECK || IsSafeToTake() || g_fEEShutDown);

    // If this is a debugger lock, bump up the "Can't-Stop" count.
    // We'll bump it down when we release the lock.
    bool fIsDebuggerLock = (m_dwFlags & CRST_DEBUGGER_THREAD) != 0;
    if (fIsDebuggerLock)
    {
        IncCantStopCount(StateHolderParamValue);
    }

    // Similarly, for locks that could be taken by threads that suspend
    // other threads (CRST_SUSPEND_THREAD), we want to increment
    // ForbidSuspendThread on current thread.  This prevents
    // current thread (A) from being suspended by another thread (B),
    // otherwise B would be blocked by getting this same lock.
    if (m_dwFlags & CRST_SUSPEND_THREAD)
    {
        Thread::IncForbidSuspendThread(StateHolderParamValue);
    }
    
    if (IsOSCritSec()) {
    UnsafeEnterCriticalSection(&m_criticalsection);
    }
    else {
        DWORD option;
        if (m_dwFlags & CRST_HOST_BREAKABLE)
        {
            option = 0;
        }
        else
        {
            option = WAIT_NOTINDEADLOCK;
        }
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = m_pHostCrst->Enter(option);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        
        PREFIX_ASSUME (hr == S_OK || ((m_dwFlags & CRST_HOST_BREAKABLE) && hr == HOST_E_DEADLOCK));
        if (hr == HOST_E_DEADLOCK)
        {
            RaiseDeadLockException();
        }
        INCTHREADLOCKCOUNT();
    }

#ifdef _DEBUG
    PostEnter ();
#endif
    if (fToggle) {
        pThread->DisablePreemptiveGC();
    }
}

//-----------------------------------------------------------------
// Release the lock.
//-----------------------------------------------------------------
void CrstBase::Leave()
{
    WRAPPER_CONTRACT;

    _ASSERTE(IsCrstInitialized());

#ifdef _DEBUG
    PreLeave ();
#endif //_DEBUG

    if (IsOSCritSec()) {
        UnsafeLeaveCriticalSection(&m_criticalsection);
    }
    else {
        HRESULT hr;
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = m_pHostCrst->Leave();
        END_SO_TOLERANT_CODE_CALLING_HOST;
        _ASSERTE (hr == S_OK);
        DECTHREADLOCKCOUNT ();
    }


    // If this is a debugger lock, restore the "Can't-Stop" count.
    // We bumped it up when we Entered the lock.
    bool fIsDebuggerLock = (m_dwFlags & CRST_DEBUGGER_THREAD) != 0;
    if (fIsDebuggerLock)
    {
        DecCantStopCount(StateHolderParamValue);
    }

    // Similarly, for locks that could be taken by threads that suspend
    // other threads (CRST_SUSPEND_THREAD), we want to restore
    // the ForbidSuspendThread count on current thread.
    // We bumped it up when we Entered the lock.
    if (m_dwFlags & CRST_SUSPEND_THREAD)
    {
        Thread::DecForbidSuspendThread(StateHolderParamValue);
    }
    
#ifdef _DEBUG
    //_ASSERTE(m_cannotLeave==0 || OwnedByCurrentThread());
#endif
}




// Helper for acquiring the lock with a GC toggle to preemptive
void CrstBase::AcquirePreempLock(CrstBase *c)
{
    WRAPPER_CONTRACT;

    GCX_PREEMP();
    c->Enter();
}




#ifdef _DEBUG
void CrstBase::PreEnter()
{
    WRAPPER_CONTRACT;

    Thread* pThread = GetThread();

    if (pThread)
    {
        // If the thread has SpinLock, it can not take Crst.
        _ASSERTE ((pThread->m_StateNC & Thread::TSNC_OwnsSpinLock) == 0);
    }

    // If we're on the debugger helper thread, we can only take helper thread locks.
    bool fIsHelperThread = (g_pDebugInterface == NULL) ? false : g_pDebugInterface->ThisIsHelperThread();
    bool fIsDebuggerLock = (m_dwFlags & CRST_DEBUGGER_THREAD) != 0;

    if (fIsHelperThread && !fIsDebuggerLock && !g_fProcessDetach)
    {
        CONSISTENCY_CHECK_MSGF(false, ("Helper thread taking non-helper lock:'%s'", this->m_tag));
    }

    // If a thread suspends another thread, it can only acquire locks marked with CRST_SUSPEND_THREAD.
    if (pThread != NULL
        && pThread->GetSuspendeeCount() != 0
        && (m_dwFlags & CRST_SUSPEND_THREAD) == 0)
    {
        CONSISTENCY_CHECK_MSGF(false, ("Suspender thread taking non-suspender lock:'%s'", this->m_tag));
    }

    if (ThreadStore::s_pThreadStore->IsCrstForThreadStore(this))
        return;
    
    if (m_dwFlags & CRST_UNSAFE_COOPGC)
        CONSISTENCY_CHECK (IsGCThread ()
                          || (pThread != NULL && pThread->PreemptiveGCDisabled()));

}

void CrstBase::PostEnter()
{
    WRAPPER_CONTRACT;

    m_holderthreadid.SetThreadId();
    m_entercount++;

    if (m_entercount == 1)
    {
        _ASSERTE((m_next == NULL) && (m_prev == NULL));
        
        // Link this Crst into the Thread's chain of OwnedCrsts
        CrstBase *pcrst = GetThreadsOwnedCrsts();
        if (pcrst == NULL)
        {
            SetThreadsOwnedCrsts (this);
        }
        else
        {
            while (pcrst->m_next != NULL)
                pcrst = pcrst->m_next;
            pcrst->m_next = this;
            m_prev = pcrst;
        }
    }

    Thread* pThread = GetThread();
    if ((m_dwFlags & CRST_HOST_BREAKABLE) == 0)
    {
        if (pThread)
        {
            pThread->IncUnbreakableLockCount();
        }
    }

    if (ThreadStore::s_pThreadStore->IsCrstForThreadStore(this))
        return;

    if (m_dwFlags & (CRST_UNSAFE_ANYMODE | CRST_UNSAFE_COOPGC))
    {
        if (pThread == NULL)
        {
            // Cannot set NoTrigger.  This could conceivably turn into 
            // A GC hole if the thread is created and then a GC rendezvous happens
            // while the lock is still held.
        }
        else
        {
            // Keep a count, since the thread may change from NULL to non-NULL and
            // we don't want to have unbalanced NoTrigger calls
            m_countNoTriggerGC++;
            INCONTRACT(pThread->BeginNoTriggerGC(__FILE__, __LINE__));
        }
    }

}

void CrstBase::PreLeave()
{
    WRAPPER_CONTRACT;

    _ASSERTE(OwnedByCurrentThread());
    _ASSERTE(m_entercount > 0);
    m_entercount--;
    if (!m_entercount) {
        m_holderthreadid.ResetThreadId();

        // Delink it from the Thread's chain of OwnedChain
        if (m_prev)
            m_prev->m_next = m_next;
        else
            SetThreadsOwnedCrsts(m_next);
        
        if (m_next)
            m_next->m_prev = m_prev;

        m_next = NULL;
        m_prev = NULL;
    }

    Thread *pThread = GetThread();

    if ((m_dwFlags & CRST_HOST_BREAKABLE) == 0)
    {
        if (pThread)
        {
            pThread->DecUnbreakableLockCount();
        }
    }

    if (ThreadStore::s_pThreadStore->IsCrstForThreadStore(this))
        return;       

    if (m_countNoTriggerGC > 0)
    {
        m_countNoTriggerGC--;
        if (pThread != NULL)
        {
            INCONTRACT(pThread->EndNoTriggerGC());
        }
    }

}

struct CrstDebugInfo
{
    CrstBase *pAddress;
    CHAR      Tag[24];
};
const int crstDebugInfoCount = 4000;
CrstDebugInfo crstDebugInfo[crstDebugInfoCount];

CrstBase *CrstBase::GetThreadsOwnedCrsts()
{
    return (CrstBase*)ClrFlsGetValue(TlsIdx_OwnedCrstsChain);
}
void CrstBase::SetThreadsOwnedCrsts(CrstBase *pCrst)
{
    ClrFlsSetValue(TlsIdx_OwnedCrstsChain, (LPVOID) (pCrst));
}

void CrstBase::DebugInit(LPCSTR szTag, CrstLevel crstlevel, CrstFlags flags)
{
    WRAPPER_CONTRACT;

    if(szTag) {
        int lgth = (int)strlen(szTag) + 1;
        lgth = (lgth > (int)(sizeof(m_tag)/sizeof(m_tag[0]))) ? (sizeof(m_tag) / sizeof(m_tag[0])) : (lgth);
        memcpy(m_tag, szTag, lgth);

        // Null terminate the string in case it got truncated
        m_tag[lgth-1] = 0;
    }
    
    m_crstlevel = GetRankOfCrstLevel(crstlevel);
    m_holderthreadid.ResetThreadId();
    m_entercount       = 0;
    m_next = NULL;
    m_prev = NULL;
    m_cannotLeave=0;
    
    _ASSERTE((m_dwFlags & ~(CRST_REENTRANCY |
                          CRST_UNSAFE_SAMELEVEL |
                          CRST_UNSAFE_COOPGC |
                          CRST_UNSAFE_ANYMODE | 
                          CRST_DEBUGGER_THREAD |
                          CRST_HOST_BREAKABLE |
                          CRST_SUSPEND_THREAD |
                          CRST_OS_CRIT_SEC |
                          CRST_INITIALIZED)) == 0);

    
    LOG((LF_SYNC, INFO3, "ConstructCrst with this:0x%x\n", this));

    for (int i = 0; i < crstDebugInfoCount; i++)
    {
        if (crstDebugInfo[i].pAddress == NULL)
        {
            crstDebugInfo[i].pAddress = this;
            strncpy_s (crstDebugInfo[i].Tag, COUNTOF(crstDebugInfo[i].Tag), m_tag, 23);
            crstDebugInfo[i].Tag[23] = 0; // Null terminate
            break;
        }
    }

    m_countNoTriggerGC = 0;
}

void CrstBase::DebugDestroy()
{
    WRAPPER_CONTRACT;

    // Ideally, when we destroy the crst, it wouldn't be held.
    // This is violated if a thread holds a lock and is asynchronously killed 
    // (such as what happens on ExitProcess).
    // Delink it from the Thread's chain of OwnedChain
    if (g_fProcessDetach)
    {
        // In shutdown scenario, crst may or may not be held.
        if (m_prev == NULL)
        {
            if (!m_holderthreadid.IsUnknown()) // Crst taken!
            {
                if (m_next)
                    m_next->m_prev = NULL;
                SetThreadsOwnedCrsts(NULL);
            }
        }
        else
        {
            m_prev->m_next = m_next;
            if (m_next)
                m_next->m_prev = m_prev;
        }
    }
    else
    {
        // Crst is destroyed while being held.
        CONSISTENCY_CHECK_MSGF(
            ((m_prev == NULL) && (m_next == NULL) && m_holderthreadid.IsUnknown()),
            ("CRST '%s' is destroyed while being held in non-shutdown scenario.\n"
            "this=0x%p, m_prev=0x%p. m_next=0x%p", this->m_tag, this, this->m_prev, this->m_next));
    }
    
    FillMemory(&m_criticalsection, sizeof(m_criticalsection), 0xcc);
    m_holderthreadid.ResetThreadId();
    m_entercount     = 0xcccccccc;

    m_next = (CrstBase*)POISONC;
    m_prev = (CrstBase*)POISONC;

    for (int i = 0; i < crstDebugInfoCount; i++)
    {
        if (crstDebugInfo[i].pAddress == this)
        {
            crstDebugInfo[i].pAddress = NULL;
            crstDebugInfo[i].Tag[0] = '\0';
            break;
        }
    }
}

//-----------------------------------------------------------------
// Check if attempting to take the lock would violate level order.
//-----------------------------------------------------------------
BOOL CrstBase::IsSafeToTake()
{
    CONTRACTL {
        DEBUG_ONLY;
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
    } CONTRACTL_END;    

    // If mscoree.dll is being detached
    if (g_fProcessDetach)
        return TRUE;

    // Cannot take a Crst in cooperative mode unless CRST_UNSAFE_COOPGC is set, in
    // which case it must always be taken in this mode.
    // If there is no thread object, we ignore the check since this thread isn't
    // coordinated with the GC.
    Thread *pThread = GetThread();

    _ASSERTE(pThread == NULL ||
             (pThread->PreemptiveGCDisabled() == ((m_dwFlags & CRST_UNSAFE_COOPGC) != 0)) ||
             ((m_dwFlags & CRST_UNSAFE_ANYMODE) != 0) ||
             (GCHeap::IsGCInProgress() && pThread == GCHeap::GetGCHeap()->GetGCThread()));

    if (m_holderthreadid.IsSameThread())
    {
        // If we already hold it, we can't violate level order.
        // Check if client wanted to allow reentrancy.
        if ((m_dwFlags & CRST_REENTRANCY) == 0)
        {
            LOG((LF_SYNC, INFO3, "Crst Reentrancy violation on %s\n", m_tag));
            // So that we can debug here.
            _ASSERTE (g_fEEShutDown || !"Crst Reentrancy violation");
        }
        return ((m_dwFlags & CRST_REENTRANCY) != 0);
    }

    // Is the current Crst exempt from the Crst ranking enforcement?
    if (m_crstlevel == CRSTUNORDERED 
        // when the thread is doing a stressing GC, some Crst violations could be ignored
        // also, we want to keep an explicit list of Crst's that we may take during GC stress
        || (GetThread () && GetThread()->GetGCStressing () 
            && (m_crstlevel == CrstThreadStoreRanking || m_crstlevel == CrstHandleTableRanking 
                || m_crstlevel == CrstSyncBlockCacheRanking || m_crstlevel == CrstIbcProfileRanking 
                || m_crstlevel == CrstAvailableParamTypesRanking)
           )
        || (GetThread () && GetThread ()->GetUniqueStacking ())
    )
    {
        return TRUE;
    }

    // See if the current thread already owns a lower or sibling lock.
    BOOL fSafe = TRUE;
    for (CrstBase *pcrst = GetThreadsOwnedCrsts(); pcrst != NULL; pcrst = pcrst->m_next)
    {
        fSafe = 
            !pcrst->m_holderthreadid.IsSameThread()
            || (pcrst->m_crstlevel == CRSTUNORDERED)
            || (pcrst->m_crstlevel > m_crstlevel)
            || (pcrst->m_crstlevel == m_crstlevel && (m_dwFlags & CRST_UNSAFE_SAMELEVEL) != 0);
        if (!fSafe)
        {
            LOG((LF_SYNC, INFO3, "Crst Level violation: Can't take level %lu lock %s because you already holding level %lu lock %s\n",
                (ULONG)m_crstlevel, m_tag, (ULONG)(pcrst->m_crstlevel), pcrst->m_tag));
            // So that we can debug here.
            if (!g_fEEShutDown)
            {
                CONSISTENCY_CHECK_MSGF(false, ("Crst Level violation: Can't take level %lu lock %s because you already holding level %lu lock %s\n",
                                               (ULONG)m_crstlevel,
                                               m_tag,
                                               (ULONG)(pcrst->m_crstlevel),
                                               pcrst->m_tag));
            }
            break;
        }
    }
    return fSafe;
}

#endif


#ifdef _DEBUG


struct CrstLevelMapping
{
    CrstLevel m_szName;
    UINT     *m_pranking;
};


//------------------------------------------------------------------------
// This simply sets up our lookup array for resolving CrstLevel names to
// rankings:
//
// CrstLevelMapping gCrstLevelMapping[] = {
//    {"CrstLevelFusionLog",        &CrstLevelFusionLogRanking},
//    {"CrstLevelFusionTree",       &CrstLevelFusionTreeRanking},
//    ...
// };
//------------------------------------------------------------------------
CrstLevelMapping gCrstLevelMapping[] = {
#define DEFINE_CRST_LEVEL(name) {#name, &(name##Ranking)},
#include "crstlevels.h"
#undef DEFINE_CRST_LEVEL
};


//------------------------------------------------------------------------
// This converts a textual CrstLevel to the actual ranking. Normally,
// this is just a straightforward lookup from gCrstLevelMapping. However,
// you can also override individual rankings using the registry.
//------------------------------------------------------------------------
UINT GetRankOfCrstLevel(CrstLevel level)
{
    LEAF_CONTRACT;

    // This allows retail components to call debug mscorwks.dll. 
    if (level == NULL)
    {
        return CRSTUNORDERED;
    }

#ifdef _X86_
#endif


    for (UINT i = 0; i < sizeof(gCrstLevelMapping)/sizeof(*gCrstLevelMapping); i++)
    {
        if (0 == strcmp(level, gCrstLevelMapping[i].m_szName))
        {
            return *(gCrstLevelMapping[i].m_pranking);
        }
    }
    _ASSERTE(!"Invalid CrstLevel.");
    return 0;
}


//================================================================================================
// These are CrstLevel rankings. To add a ranking, you must update both this file and
// src\inc\crstlevels.h. See comments there for full details.
//================================================================================================

UINT Crst_TEMP_HACK_UnorderedRanking    = CRSTUNORDERED;

// These should not be used in any checked in sources. They exist simply
// so that if you need to add a new crstlevel, you can use one of these temporarily
// until it's convenient for you to hit crstlevels.h and do a total source rebuild.
UINT CrstSpare1Ranking                  = CRSTUNORDERED;
UINT CrstSpare2Ranking                  = CRSTUNORDERED;
UINT CrstSpare3Ranking                  = CRSTUNORDERED;
UINT CrstSpare4Ranking                  = CRSTUNORDERED;
UINT CrstSpare5Ranking                  = CRSTUNORDERED;
UINT CrstSpare6Ranking                  = CRSTUNORDERED;
UINT CrstSpare7Ranking                  = CRSTUNORDERED;
UINT CrstSpare8Ranking                  = CRSTUNORDERED;
UINT CrstSpare9Ranking                  = CRSTUNORDERED;



UINT CrstMetadataTrackerRanking         = CRSTUNORDERED;
UINT CrstJitPerfRanking                 = CRSTUNORDERED;
UINT CrstCCompRCRanking                 = CRSTUNORDERED;
UINT CrstMemoryReportRanking            = CRSTUNORDERED;

//dealock aware, cannot be ranked
UINT CrstListLockRanking                = CRSTUNORDERED; 


// Fusion critical sections.
//
// These critical sections are ordered correctly with respect to each other
// but have not yet been ordered with respect to the places the CLR and
// Fusion interact (reading metadata00; strong name signature validation).
// When these code paths have their crsts levelled these levels will
// probably change.
//

UINT CrstModIntPairListRanking          = 2450; // must be less than other Fusion CRSTs
UINT CrstFusionListRanking              = 2500;
UINT CrstFusionLogRanking               = 2500;
UINT CrstFusionSingleUseRanking         = 2500;
UINT CrstFusionAsmImprtRanking          = 2600;
UINT CrstFusionHistoryRanking           = 2600;
UINT CrstFusionModImprtRanking          = 2600;
UINT CrstFusionNgenIndexRanking         = 2700;

UINT CrstFusionClbRanking               = 2800;
UINT CrstFusionPcyCacheRanking          = 2900;
UINT CrstFusionBindResultRanking        = 2900;
    
UINT CrstFusionLoadContextRanking       = 2900; // must be less than CrstThreadStore
UINT CrstFusionConfigSettingsRanking    = 2900;

UINT CrstFusionNgenIndexPoolRanking     = 2900;
UINT CrstFusionGACAsmPoolRanking        = 2900;
UINT CrstFusionNIAsmPoolRanking         = 2900;

UINT CrstFusionDownloadRanking          = 3800;  
UINT CrstFusionAssemblyDownloadRanking  = 3900; // must be more than CrstThreadStore

UINT CrstFusionAppCtxRanking            = 4000; 

UINT CrstFusionClosureRanking           = 4100;


UINT CrstDummyRanking                   = 0;           // For internal use only. Not a true level.
UINT CrstGCMemoryPressureRanking        = 10;          // used by the AddMemoryPressure/RemoveMemoryPressure apis

UINT CrstStressLogRanking               = 10;
UINT CrstSecurityStackwalkCacheRanking  = 10;          // For security stackwalk cache
UINT CrstLockedRangeListRanking         = 30;          // Must be lower than CrstLoaderHeap
UINT CrstLoaderHeapRanking              = 50;          // Must be lower than CrstUniqueStackRanking (!)
UINT CrstUniqueStackRanking             = 70;          // For FastGCStress00; which should be lower than CrstLeaf
                                                       //   (! If it's supposed to be lower than leaf, why is it using the LoaderHeap?)
UINT CrstSystemDomainDelayedUnloadListRanking  = 90;

UINT CrstIbcProfileRanking              = 95;          // For the module profiler--should not enter any other critical section while in here 
UINT CrstAvailableClassRanking          = 100;
UINT CrstStubDispatchCacheRanking       = 100;
UINT CrstPEImageViewRanking             = 100;
UINT CrstInteropDataRanking             = 100;
UINT CrstWatsonRanking                  = 100;
UINT CrstDynamicMTRanking               = 100;
UINT CrstCodeHeapListRanking            = 100;
UINT CrstPinHandleRanking               = 100;
UINT CrstSpecialStaticsRanking          = 100;
UINT CrstIJWHashRanking                 = 100;
UINT CrstExternalRelocsRanking          = 100;
UINT CrstStubUnwindInfoHeapSegmentsRanking = 100;
UINT CrstDeadlockDetectionRanking       = 100;

UINT CrstXMLParserRanking               = 100;

UINT CrstPublisherCertificateRanking    = 100;         // Publisher certificate table.
UINT CrstStrongNameRanking              = 100;         // Some crypto APIs are non thread safe. This Crst is used to serialize these operations.
UINT CrstSaveModuleProfileDataRanking   = 100;
UINT CrstDebuggerFavorLockRanking       = 100;
UINT CrstDebuggerHeapLockRanking        = 100;
UINT CrstReDaclRanking                  = 200;         // Must be more than CrstDebuggerHeapLockRanking
UINT CrstDebuggerJitInfoRanking         = 200;         // Debugger Jit Info lock
UINT CrstIJWFixupDataRanking            = 200;
UINT CrstPEImageRanking                 = 200;   
UINT CrstObjectListRanking              = 300;         // For the object list
UINT CrstThreadIdDispenserRanking       = 300;         // For the thin lock thread ids - needs to be less than CrstThreadStore
UINT CrstMdaRanking                     = 500;         // For inilization of mda after dynamic activation
UINT CrstCerRanking                     = 600;         // For manipulating per-module hash tables used by Critical Execution Regions
UINT CrstOnEventManagerRanking          = 700;         // For CCLROnEventManager
UINT CrstInterfaceVTableMapRanking      = 1000;        // synchronize access to InterfaceVTableMap
UINT CrstStubInterceptorRanking         = 1000;        // stub tracker (debug)
UINT CrstStubTrackerRanking             = 1000;        // stub tracker (debug)
UINT CrstSyncBlockCacheRanking          = 1300;        // allocate a SyncBlock to an object -- taken inside CrstHandleTable
UINT CrstHandleTableRanking             = 1500;        // allocate / release a handle (called inside CrstSingleUseLock)
UINT CrstCompressedStackRefRanking      = 1500;        // Reference counting lock for the compressed stack
UINT CrstExecuteManRangeLockRanking     = 1900;
UINT CrstSyncHashLockRanking            = 2000;        // used for synchronized access to a hash table
UINT CrstSingleUseLockRanking           = 2000;        // one time initialization of data00; locks use this level
UINT CrstModuleRanking                  = 2000;
UINT CrstModuleLookupTableRanking       = 2000;
UINT CrstArgBasedStubCacheRanking       = 2000;
UINT CrstThreadRanking                  = 2000;        // used during e.g. thread suspend
UINT CrstMLCacheRanking                 = 2000;
UINT CrstUMThunkHashRanking             = 2000;
UINT CrstMUThunkHashRanking             = 2000;
UINT CrstReflectionRanking              = 2000;        // Reflection memory setup
UINT CrstADCompressedStackRegistryRanking = 2000;     // lock for protecting access to the AD domain compressed stack list
UINT CrstCompressedStackTransitionRanking = 2000;      // State transition lock for the compressed stack
UINT CrstSecurityPolicyCacheRanking     = 2000;        // For Security policy cache
UINT CrstRVAOverridesRanking            = 2000;        // Overrides of method RVAs
UINT CrstRCWCacheRanking                = 2200;        // For RCWCache
UINT CrstSigConvertRanking              = 2500;        // convert a gsig_ from text to binary
UINT CrstCompressedStackListCleanupRanking = 2500;     // List cleanup lock for the compressed stack
UINT CrstJumpStubCacheRanking           = 2500;
UINT CrstPatchEntryPointRanking         = 2500;

UINT CrstAvailableParamTypesRanking     = 2950;        // must be more than FusionLoadContextRanking
UINT CrstISymUnmanagedReaderRanking     = 2950;        // This one need to be taken larger than fusion locks

UINT CrstUnresolvedClassLockRanking     = 2960;        // must be more than FusionLoadContextRanking

UINT CrstDebuggerControllerMutexRanking = 2970;        // Debugger controller lock. Must be bigger than CrstUnresolvedClassLock, smaller than TSL and Debugger lock

UINT CrstThreadStoreRanking             = 3000;        // used to e.g. iterate over threads in system

    // must be CrstThreadStore unless we rewrite our Debugger::HandleIPCEvent code.
UINT CrstIsJMCMethodRanking             = 3100;         // Must be more than ThreadStoreRanking
UINT CrstDebuggerMutexRanking           = 3100;         // Must be more to ThreadStoreRanking


UINT CrstTPMethodTableRanking           = 3200;
UINT CrstThreadpoolWorkerRanking        = 3300;        // For threadpool worker
UINT CrstThreadpoolWaitThreadsRanking   = 3300;        // For threadpool wait thread
UINT CrstThreadpoolTimerQueueRanking    = 3300;        // For threadpool timer queue
UINT CrstThreadpoolEventCacheRanking    = 3300;        // For threadpool event cache
UINT CrstAppDomainCacheRanking          = 3500;
UINT CrstMethodJitLockRanking           = 3500;
UINT CrstExecuteManLockRanking          = 3500;
UINT CrstSystemDomainRanking            = 4000;
UINT CrstAppDomainHandleTableRanking    = 4500;        // A lock to protect the large heap handle table at the app domain level
UINT CrstGlobalStrLiteralMapRanking     = 4500;        // A lock to protect the global string literal map.
UINT CrstAppDomainStrLiteralMapRanking  = 5000;        // A lock to protect the app domain specific string literal map.
UINT CrstCompilationDomainRanking       = 5000;

UINT CrstDomainLocalBlockRanking        = 5000;
UINT CrstModuleFixupRanking             = 5100;

UINT CrstInstMethodHashTableRanking     = 5100;
 
UINT CrstClassInitRanking               = 5500;        // Class initializers

UINT CrstThreadDomainLocalStoreRanking  = 5600;        // used to update the thread's domain local store list
UINT CrstEventStoreRanking              = 5700;        // A lock to protect the store for events used for Object::Wait
UINT CrstSharedAssemblyCreateRanking    = 5900;        // Creating shared assemblies should be just below assembly loading
UINT CrstAssemblyLoaderRanking          = 6000;        // DO NOT place another crst at this level
UINT CrstSharedBaseDomainRanking        = 6300;      
UINT CrstSystemBaseDomainRanking        = 6400;      
UINT CrstBaseDomainRanking              = 6500;      
UINT CrstCtxVTableRanking               = 7000;        // increase the size of context proxy vtable
UINT CrstClassHashRanking               = 7500;
UINT CrstClassloaderRequestQueueRanking = 8000;
UINT CrstWrapperTemplateRanking         = 8400;        // Create a wrapper template for a class
UINT CrstCOMWrapperCacheRanking         = 8500;
UINT CrstILStubGenRanking               = 8600;
UINT CrstPendingTypeLoadEntryRanking    = 8700;
UINT CrstRemotingRanking                = 9000;        // Remoting infrastructure
UINT CrstInteropRanking                 = 9000;
UINT CrstClassFactInfoHashRanking       = 9500;        // Class factory hash lookup
UINT CrstStartupRanking                 = 10000;       // Initializes and uninitializes the EE

UINT CrstXMLParserManagedRanking        = MAXSHORT;

UINT CrstSynchronizedRanking            = MAXSHORT; // an object is Synchronized



#endif
