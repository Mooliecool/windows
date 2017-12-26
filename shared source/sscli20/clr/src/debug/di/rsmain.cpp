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
// File: RsMain.cpp
// Random RS utility stuff, plus root ICorCordbug implementation
//
//*****************************************************************************
#include "stdafx.h"
#include "primitives.h"
#include "safewrap.h"

#include "check.h"


#ifndef SM_REMOTESESSION
#define SM_REMOTESESSION 0x1000
#endif

#include "corpriv.h"
#include "../../dlls/mscorrc/resource.h"
#include <limits.h>

#include "dacprivate.h"





//-----------------------------------------------------------------------------
// For debugging ease, cache some global values.
// Include these in retail & free because that's where we need them the most!!
// Optimized builds may not let us view locals & parameters. So Having these
// cached as global values should let us inspect almost all of
// the interesting parts of the RS even in a Retail build!
//-----------------------------------------------------------------------------

RSDebuggingInfo g_RSDebuggingInfo_OutOfProc = {0 }; // set to NULL
RSDebuggingInfo * g_pRSDebuggingInfo = &g_RSDebuggingInfo_OutOfProc;


#ifdef _DEBUG
// For logs, we can print the string name for the debug codes.
const char * GetDebugCodeName(DWORD dwCode)
{
    if (dwCode < 1 || dwCode > 9)
    {
        return "!Invalid Debug Event Code!";
    }

    static char * szNames[] = {
        "(1) EXCEPTION_DEBUG_EVENT",
        "(2) CREATE_THREAD_DEBUG_EVENT",
        "(3) CREATE_PROCESS_DEBUG_EVENT",
        "(4) EXIT_THREAD_DEBUG_EVENT",
        "(5) EXIT_PROCESS_DEBUG_EVENT",
        "(6) LOAD_DLL_DEBUG_EVENT",
        "(7) UNLOAD_DLL_DEBUG_EVENT",
        "(8) OUTPUT_DEBUG_STRING_EVENT"
        "(9) RIP_EVENT",// <-- only on Win9X
    };

    return szNames[dwCode - 1];
}

#endif



//-----------------------------------------------------------------------------
// Simple holder for a copy of a string.
//-----------------------------------------------------------------------------

// Initialize to Null.
StringCopyHolder::StringCopyHolder()
{
    m_szData = NULL;
}

// Dtor to free memory.
StringCopyHolder::~StringCopyHolder()
{
    delete [] m_szData;
    m_szData = NULL;
}

// Allocate a copy of the incoming string and assign it to this holder.
// pStringSrc is a non-null pointer to a null-terminated string.
//
// Returns:
// true on success. That means it succeeded in allocating and copying pStringSrc.
//   if wcslen(pStringSrc) == 0, then the underlying string will be null (and
//   not a pointer to a 0-length null terminated string)
//
// false on failure. Only happens in an OOM scenario.
bool StringCopyHolder::AssignCopy(const WCHAR * pStringSrc)
{
    _ASSERTE(pStringSrc != NULL);
    _ASSERTE(m_szData == NULL); // only allocate once.

    SIZE_T len = wcslen(pStringSrc) + 1;

    // Ignore < 1 in case len overflowed when we did +1.
    if (len > 1)
    {
        m_szData = new (nothrow) WCHAR[len];
        if (m_szData == NULL)
        {
            _ASSERTE(!"Warning: Out-of-Memory in Right Side. This component is not robust in OOM sccenarios.");
            return false;
        }

        wcscpy_s(m_szData, len, pStringSrc);
    }
    return true;
}


//-----------------------------------------------------------------------------
// Per-thread state for Debug builds...
//-----------------------------------------------------------------------------
#ifdef RSCONTRACTS
DWORD DbgRSThread::s_TlsSlot = TLS_OUT_OF_INDEXES;
LONG DbgRSThread::s_Total = 0;

DbgRSThread::DbgRSThread()
{
    m_cInsideRS         = 0;
    m_fIsInCallback     = false;
    m_fIsUnrecoverableErrorCallback = false;

    m_cTotalLocks = 0;
    for(int i = 0; i < RSLock::LL_MAX; i++)
    {
        m_cLocks[i] = 0;
    }

    // Initialize Identity info
    m_Cookie = COOKIE_VALUE;
    m_tid = GetCurrentThreadId();
}

// NotifyTakeLock & NotifyReleaseLock are called by RSLock to update the per-thread locking context.
// This will assert if the operation is unsafe (ie, violates lock order).
void DbgRSThread::NotifyTakeLock(RSLock * pLock)
{
    if (pLock->HasLock())
    {
        return;
    }


    int iLevel = pLock->GetLevel();

    // Is it safe to take this lock?
    // Must take "bigger" locks first. We shouldn't hold any locks at our current level either.
    // If this lock is re-entrant and we're double-taking it, we would have returned already.
    // And the locking model on the RS forbids taking multiple locks at the same level.
    for(int i = iLevel; i >= 0; i --)
    {
        bool fHasLowerLock = m_cLocks[i] > 0;
        CONSISTENCY_CHECK_MSGF(!fHasLowerLock, (
            "RSLock violation. Trying to take lock '%s (%d)', but already have smaller lock at level %d'\n",
            pLock->Name(), iLevel,
            i));
    }

    // Update the counts
    _ASSERTE(m_cLocks[iLevel] == 0);
    m_cLocks[iLevel]++;
    m_cTotalLocks++;
}

void DbgRSThread::NotifyReleaseLock(RSLock * pLock)
{
    if (pLock->HasLock())
    {
        return;
    }

    int iLevel = pLock->GetLevel();
    m_cLocks[iLevel]--;
    _ASSERTE(m_cLocks[iLevel] == 0);

    m_cTotalLocks--;
    _ASSERTE(m_cTotalLocks >= 0);
}

void DbgRSThread::TakeVirtualLock(RSLock::ERSLockLevel level)
{
    m_cLocks[level]++;
}

void DbgRSThread::ReleaseVirtualLock(RSLock::ERSLockLevel level)
{
    m_cLocks[level]--;
    _ASSERTE(m_cLocks[level] >= 0);
}


// Get a DbgRSThread for the current OS thread id; lazily create if needed.
DbgRSThread * DbgRSThread::GetThread()
{
    _ASSERTE(DbgRSThread::s_TlsSlot != TLS_OUT_OF_INDEXES);

    void * p2 = TlsGetValue(DbgRSThread::s_TlsSlot);
    if (p2 == NULL)
    {
        // We lazily create for threads that haven't gone through DllMain
        // Since this is per-thread, we don't need to lock.
        p2 = DbgRSThread::Create();
    }
    DbgRSThread * p = reinterpret_cast<DbgRSThread*> (p2);

    _ASSERTE(p->m_Cookie == COOKIE_VALUE);

    return p;
}

// Only safe to Neuter if we (hold the Stop-Go lock)
bool DbgRSThread::IsSafeToNeuter()
{
    // This check is a little conservative b/c there's 1 SG lock per process.
    // But our processes never cross, so it will do.
    bool fHasSGLock = m_cLocks[RSLock::LL_STOP_GO_LOCK] > 0;

    bool fHasProcessListLock = m_cLocks[RSLock::LL_PROCESS_LIST_LOCK] > 0;
    return fHasSGLock ^ fHasProcessListLock;
}


#endif // RSCONTRACTS






#ifdef _DEBUG
LONG CordbCommonBase::s_TotalObjectCount = 0;
LONG CordbCommonBase::s_CordbObjectUID = 0;


LONG CordbCommonBase::m_saDwInstance[enumMaxDerived];
LONG CordbCommonBase::m_saDwAlive[enumMaxDerived];
PVOID CordbCommonBase::m_sdThis[enumMaxDerived][enumMaxThis];

#endif

#ifdef _DEBUG_IMPL
// Mem tracking
LONG Cordb::s_DbgMemTotalOutstandingCordb            = 0;
LONG Cordb::s_DbgMemTotalOutstandingInternalRefs     = 0;
#endif

#ifdef TRACK_OUTSTANDING_OBJECTS
void *Cordb::s_DbgMemOutstandingObjects[MAX_TRACKED_OUTSTANDING_OBJECTS] = { NULL };
LONG Cordb::s_DbgMemOutstandingObjectMax = 0;
#endif

//-----------------------------------------------------------------------------
// Neuter tickets. Ctor just does validation
//-----------------------------------------------------------------------------
NeuterTicket::NeuterTicket(CordbProcess *pProc)
{
    _ASSERTE(pProc->GetStopGoLock()->HasLock());
    _ASSERTE(pProc->GetSynchronized() || !"Must be sync-ed to Neuter");
}



//-----------------------------------------------------------------------------
// NeuterLists
//-----------------------------------------------------------------------------

NeuterList::NeuterList()
{
    m_pHead = NULL;

#ifdef _DEBUG
    m_DbgCount = 0;
#endif
}

NeuterList::~NeuterList()
{
    // Our owner should have neutered us before deleting us.
    // Thus we should be empty.
    CONSISTENCY_CHECK_MSGF(m_pHead == NULL,
        ("NeuterList not empty on shutdown. this=0x%p. count=%d", this, m_DbgCount));

#ifdef _DEBUG
    _ASSERTE(m_DbgCount == 0);
#endif
}

// Add an object to be neutered.
// This will add it to the list and maintain an internal reference to it.
void NeuterList::Add(CordbBase * pObject)
{
#ifdef RSCONTRACTS
    _ASSERTE(DbgRSThread::GetThread()->IsSafeToNeuter());
#endif
    UnsafeAdd(pObject);
}

void NeuterList::UnsafeAdd(CordbBase * pObject)
{
    _ASSERTE(pObject != NULL);

    Node * pNode = new Node();
    pNode->m_pObject.Assign(pObject);
    pNode->m_pNext = m_pHead;

    m_pHead = pNode;

#ifdef _DEBUG
    m_DbgCount++;
#endif
}

// Neuter everything on the list.
// This will release all internal references and empty the list.
void NeuterList::NeuterAndClear(NeuterTicket ticket)
{
#ifdef RSCONTRACTS
    _ASSERTE(DbgRSThread::GetThread()->IsSafeToNeuter());
#endif

    Node * pCur = m_pHead;

    while (pCur != NULL)
    {
        Node * pTemp = pCur;
        pCur = pCur->m_pNext;

        pTemp->m_pObject->Neuter(ticket);
        delete pTemp; // will implicitly release

#ifdef _DEBUG
        _ASSERTE(m_DbgCount > 0);
        m_DbgCount--;
#endif
    }
    m_pHead = NULL;
    _ASSERTE(m_DbgCount == 0);
}

// Only neuter objects that are marked.
void NeuterList::SweepAllNeuterAtWillObjects(NeuterTicket ticket)
{
#ifdef RSCONTRACTS
    _ASSERTE(DbgRSThread::GetThread()->IsSafeToNeuter());
#endif

    Node ** ppLast = &m_pHead;
    Node * pCur = m_pHead;

#ifdef _DEBUG
    int dbgCountStart = m_DbgCount;
    int dbgCountDelete = 0;
    int dbgCountKeep = 0;
#endif

    while (pCur != NULL)
    {
        CordbBase * pObject = pCur->m_pObject;
        if (pObject->IsNeuterAtWill() || pObject->IsNeutered())
        {
            // Delete
#ifdef _DEBUG
            _ASSERTE(m_DbgCount > 0);
            m_DbgCount--;
            dbgCountDelete++;
#endif

            pObject->Neuter(ticket);

            Node * pNext = pCur->m_pNext;
            delete pCur; // dtor will implicitly release the internal ref to pObject
            pCur =  *ppLast = pNext;
        }
        else
        {
            // Move to next.
            ppLast = &pCur->m_pNext;
            pCur = pCur->m_pNext;

#ifdef _DEBUG
            dbgCountKeep++;
#endif
        }
    }

    _ASSERTE(m_DbgCount + dbgCountDelete == dbgCountStart);
    _ASSERTE(dbgCountKeep ==  m_DbgCount);

}



/* ------------------------------------------------------------------------- *
 * CordbBase class
 * ------------------------------------------------------------------------- */
void CordbCommonBase::NeuterAndClearHashtable(CordbHashTable * pCordbHashtable, NeuterTicket ticket)
{
    HASHFIND hfDT;
    CordbBase * pCordbBase;

    while ((pCordbBase = (pCordbHashtable->UnsafeFindFirst(&hfDT))) != 0)
    {
        _ASSERTE(pCordbBase != NULL);
        RSSmartPtr<CordbBase> pRef(pCordbBase);
        pCordbHashtable->UnsafeRemoveBase((ULONG_PTR)pCordbBase->m_id); // this will call release

        // Neuter after RemoveBase so that the object's neuter function could
        // check to make sure it's out of it's parent's hashtable.
        pCordbBase->Neuter(ticket);

        // Implicit Release from SmartPtr
    }

#if _DEBUG
    // The hash should be empty now.
    HASHFIND h2;
    _ASSERTE(pCordbHashtable->UnsafeFindFirst(&h2) == NULL);
#endif

}

// Do any initialization necessary for both CorPublish and CorDebug
// This includes enabling logging and adding the SEDebug priv.
void CordbCommonBase::InitializeCommon()
{
    static bool IsInitialized = false;
    if( IsInitialized )
    {
        return;
    }
    
#ifdef STRESS_LOG
    {
        bool fStressLog = false;

#ifdef _DEBUG
        // default for stress log is on debug build
        fStressLog = true;
#endif // DEBUG

        // StressLog will turn on stress logging for the entire runtime.
        // RSStressLog is only used here and only effects just the RS.
        fStressLog =
            (REGUTIL::GetConfigDWORD(L"StressLog", fStressLog) != 0) ||
            (REGUTIL::GetConfigDWORD(L"RSStressLog", 0) != 0);

        if (fStressLog == true)
        {
            unsigned facilities = REGUTIL::GetConfigDWORD(L"LogFacility", LF_ALL);
            unsigned level = REGUTIL::GetConfigDWORD(L"LogLevel", LL_INFO1000);
            unsigned bytesPerThread = REGUTIL::GetConfigDWORD(L"StressLogSize", STRESSLOG_CHUNK_SIZE * 2);
            unsigned totalBytes = REGUTIL::GetConfigDWORD(L"TotalStressLogSize", STRESSLOG_CHUNK_SIZE * 1024);
            StressLog::Initialize(facilities, level, bytesPerThread, totalBytes, GetModuleInst());
        }
    }

#endif // STRESS_LOG

#ifdef LOGGING
    InitializeLogging();
#endif

    // Add debug privilege. This will let us call OpenProcess() on anything, regardless of ACL.
    AddDebugPrivilege();
        
    IsInitialized = true;
}

void CordbCommonBase::AddDebugPrivilege()
{
} 


namespace
{

    //
    // DefaultManagedCallback2
    //
    // In the event that the debugger is of an older version than the Right Side & Left Side, the Right Side may issue
    // new callbacks that the debugger is not expecting. In this case, we need to provide a default behavior for those
    // new callbacks, if for nothing else than to force the debugger to Continue().
    //
    class DefaultManagedCallback2 : public ICorDebugManagedCallback2
    {
    public:
        DefaultManagedCallback2(ICorDebug* pDebug);
        virtual ~DefaultManagedCallback2() { }
        virtual HRESULT __stdcall QueryInterface(REFIID iid, void** pInterface);
        virtual ULONG __stdcall AddRef();
        virtual ULONG __stdcall Release();
        COM_METHOD FunctionRemapOpportunity(ICorDebugAppDomain* pAppDomain,
                                                 ICorDebugThread* pThread,
                                                 ICorDebugFunction* pOldFunction,
                                                 ICorDebugFunction* pNewFunction,
                                                 ULONG32 oldILOffset);
        COM_METHOD FunctionRemapComplete(ICorDebugAppDomain *pAppDomain,
                                    ICorDebugThread *pThread,
                                    ICorDebugFunction *pFunction);

        COM_METHOD CreateConnection(ICorDebugProcess *pProcess,
                                    CONNID dwConnectionId,
                                    __in_z WCHAR* pConnectionName);
        COM_METHOD ChangeConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId);
        COM_METHOD DestroyConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId);

        COM_METHOD Exception(ICorDebugAppDomain *pAddDomain,
                             ICorDebugThread *pThread,
                             ICorDebugFrame *pFrame,
                             ULONG32 nOffset,
                             CorDebugExceptionCallbackType eventType,
                             DWORD dwFlags );

        COM_METHOD ExceptionUnwind(ICorDebugAppDomain *pAddDomain,
                                   ICorDebugThread *pThread,
                                   CorDebugExceptionUnwindCallbackType eventType,
                                   DWORD dwFlags );
        COM_METHOD MDANotification(
                            ICorDebugController * pController,
                            ICorDebugThread *pThread,
                            ICorDebugMDA * pMDA
        ) { return E_NOTIMPL; }

    private:
        // not implemented
        DefaultManagedCallback2(const DefaultManagedCallback2&);
        DefaultManagedCallback2& operator=(const DefaultManagedCallback2&);

        ICorDebug* m_pDebug;
        LONG m_refCount;
    };




    DefaultManagedCallback2::DefaultManagedCallback2(ICorDebug* pDebug) : m_pDebug(pDebug), m_refCount(0)
    {
    }

    HRESULT
    DefaultManagedCallback2::QueryInterface(REFIID iid, void** pInterface)
    {
        if (IID_ICorDebugManagedCallback2 == iid)
        {
            *pInterface = static_cast<ICorDebugManagedCallback2*>(this);
        }
        else if (IID_IUnknown == iid)
        {
            *pInterface = static_cast<IUnknown*>(this);
        }
        else
        {
            *pInterface = NULL;
            return E_NOINTERFACE;
        }

        this->AddRef();
        return S_OK;
    }

    ULONG
    DefaultManagedCallback2::AddRef()
    {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG
    DefaultManagedCallback2::Release()
    {
        ULONG ulRef = InterlockedDecrement(&m_refCount);
        if (0 == ulRef)
        {
            delete this;
        }

        return ulRef;
    }

    HRESULT
    DefaultManagedCallback2::FunctionRemapOpportunity(ICorDebugAppDomain* pAppDomain,
                                                      ICorDebugThread* pThread,
                                                      ICorDebugFunction* pOldFunction,
                                                      ICorDebugFunction* pNewFunction,
                                                      ULONG32 oldILOffset)
    {

        //
        // In theory, this function should never be reached. To get here, we'd have to have a debugger which doesn't
        // support edit and continue somehow turn on edit & continue features.
        //
        _ASSERTE(!"Edit & Continue callback reached when debugger doesn't support Edit And Continue");


        // If you ignore this assertion, or you're in a retail build, there are two options as far as how to proceed
        // from this point
        //  o We can do nothing, and let the debugee process hang, or
        //  o We can silently ignore the FunctionRemapOpportunity, and tell the debugee to Continue running.
        //
        // For now, we'll silently ignore the function remapping.
        pAppDomain->Continue(false);
        pAppDomain->Release();

        return S_OK;
    }


    HRESULT
    DefaultManagedCallback2::FunctionRemapComplete(ICorDebugAppDomain *pAppDomain,
                          ICorDebugThread *pThread,
                          ICorDebugFunction *pFunction)
    {
        //
        // In theory, this function should never be reached. To get here, we'd have to have a debugger which doesn't
        // support edit and continue somehow turn on edit & continue features.
        //
        _ASSERTE(!"Edit & Continue callback reached when debugger doesn't support Edit And Continue");
        return E_NOTIMPL;
    }

    //
    //
    //
    //
    HRESULT
    DefaultManagedCallback2::CreateConnection(ICorDebugProcess *pProcess,
                                              CONNID dwConnectionId,
                                              __in_z WCHAR* pConnectionName)
    {
        _ASSERTE(!"DefaultManagedCallback2::CreateConnection not implemented");
        return E_NOTIMPL;
    }

    HRESULT
    DefaultManagedCallback2::ChangeConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId)
    {
        _ASSERTE(!"DefaultManagedCallback2::ChangeConnection not implemented");
        return E_NOTIMPL;
    }

    HRESULT
    DefaultManagedCallback2::DestroyConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId)
    {
        _ASSERTE(!"DefaultManagedCallback2::DestroyConnection not implemented");
        return E_NOTIMPL;
    }

    HRESULT
    DefaultManagedCallback2::Exception(ICorDebugAppDomain *pAppDomain,
                                       ICorDebugThread *pThread,
                                       ICorDebugFrame *pFrame,
                                       ULONG32 nOffset,
                                       CorDebugExceptionCallbackType eventType,
                                       DWORD dwFlags )
    {
        //
        // Just ignore and continue the process.
        //
        pAppDomain->Continue(false);
        return S_OK;
    }

    HRESULT
    DefaultManagedCallback2::ExceptionUnwind(ICorDebugAppDomain *pAppDomain,
                                             ICorDebugThread *pThread,
                                             CorDebugExceptionUnwindCallbackType eventType,
                                             DWORD dwFlags )
    {
        //
        // Just ignore and continue the process.
        //
        pAppDomain->Continue(false);
        return S_OK;
    }
}

/* ------------------------------------------------------------------------- *
 * Cordb class
 * ------------------------------------------------------------------------- */


Cordb::Cordb(CorDebugInterfaceVersion iDebuggerVersion)
  : CordbBase(NULL, 0, enumCordb),
    m_pCorHost(NULL),
    m_processes(11),
    m_initialized(false),
    m_debuggerSpecifiedVersion(iDebuggerVersion)
{
    g_pRSDebuggingInfo->m_Cordb = this;

#ifdef _DEBUG_IMPL
    // Memory leak detection
    InterlockedIncrement(&s_DbgMemTotalOutstandingCordb);
#endif
}

Cordb::~Cordb()
{
    LOG((LF_CORDB, LL_INFO10, "C::~C Terminating Cordb object.\n"));
    g_pRSDebuggingInfo->m_Cordb = NULL;
}

void Cordb::Neuter(NeuterTicket ticket)
{
    if (this->IsNeutered())
    {
        return;
    }
    RSSmartPtr<Cordb> pRef(this);

    {
        RSLockHolder ch(&m_processListMutex);
        m_pProcessEnumList.NeuterAndClear(ticket);
    }

    NeuterAndClearHashtable(&m_processes, ticket);
    CordbBase::Neuter(ticket);

    // Implicit release from smart ptr.
}

#ifdef _DEBUG_IMPL
void CheckMemLeaks()
{
    // Memory leak detection.
    long l = InterlockedDecrement(&Cordb::s_DbgMemTotalOutstandingCordb);
    if (l == 0)
    {
        // If we just released our final Cordb root object,  then we expect no internal references at all.
        // Note that there may still be external references (and thus not all objects may have been
        // deleted yet).
        bool fLeakedInternal = (Cordb::s_DbgMemTotalOutstandingInternalRefs > 0);

        // Some Cordb objects (such as CordbValues) may not be rooted, and thus we can't neuter
        // them and thus an external ref may keep them alive. Since these objects may have internal refs,
        // This means that external refs can keep internal refs.
        // Thus this assert must be tempered if unrooted objects are leaked. (But that means we can always
        // assert the tempered version; regardless of bugs in Cordbg).
        CONSISTENCY_CHECK_MSGF(!fLeakedInternal,
            ("'%d' Outstanding internal references at final Cordb::Terminate\n",
            Cordb::s_DbgMemTotalOutstandingInternalRefs));


        DWORD dLeakCheck = REGUTIL::GetConfigDWORD(L"DbgLeakCheck", 0);
        if (dLeakCheck > 0)
        {
            // We have 1 ref for this Cordb root object. All other refs should have been deleted.
            CONSISTENCY_CHECK_MSGF(Cordb::s_TotalObjectCount == 1, ("'%d' total cordbBase objects are leaked.\n",
                Cordb::s_TotalObjectCount-1));
        }
    }
}
#endif

// This shuts down ICorDebug.
// All CordbProcess objects owned by this Cordb object must have either:
// - returned for a Detach() call
// - returned from dispatching the ExitProcess() callback.
// In both cases, CordbProcess::NeuterChildren has been called, although the Process object itself
// may not yet be neutered.  This condition will ensure that the CordbProcess objects don't need
// any resources that we're about to release.
HRESULT Cordb::Terminate()
{
    LOG((LF_CORDB, LL_INFO10000, "[%x] Terminating Cordb\n", GetCurrentThreadId()));

    if (!m_initialized)
        return E_FAIL;

    FAIL_IF_NEUTERED(this);

    // We can't terminate the debugging services from within a callback. 
    // Caller is supposed to be out of all callbacks when they call this.
    // This also avoids a deadlock because we'll shutdown the RCET, which would block if we're
    // in the RCET.
    if (m_rcEventThread->IsRCEventThread())
    {
        STRESS_LOG0(LF_CORDB, LL_INFO10, "C::T: failed on RCET\n");
        _ASSERTE(!"Gross API Misuse: Debugger shouldn't call ICorDebug::Terminate from within a managed callback.");
        return CORDBG_E_CANT_CALL_ON_THIS_THREAD;
    }


    {
        RSLockHolder ch(&m_processListMutex);

        HASHFIND hfDT;
        CordbProcess * pProcess;

        for (pProcess=  (CordbProcess*) m_processes.FindFirst(&hfDT);
             pProcess != NULL;
             pProcess = (CordbProcess*) m_processes.FindNext(&hfDT))
        {
            _ASSERTE(pProcess->IsSafeToSendEvents() || pProcess->m_unrecoverableError);
            if (pProcess->IsSafeToSendEvents() && !pProcess->m_unrecoverableError)
            {
                CONSISTENCY_CHECK_MSGF(false, ("Gross API misuses. Callling terminate with live process:0x%p\n", pProcess));
                STRESS_LOG1(LF_CORDB, LL_INFO10, "Cordb::Terminate b/c of non-neutered process '%p'\n", pProcess);
                // This is very bad.
                // GROSS API MISUSES - Debugger is calling ICorDebug::Terminate while there
                // are still outstanding (non-neutered) ICorDebugProcess.
                // ICorDebug is now in an undefined state.
                // We will also leak memory b/c we're leaving the EventThreads up (which will in turn
                // keep a reference to this Cordb object).
                return ErrWrapper(CORDBG_E_ILLEGAL_SHUTDOWN_ORDER);
            }
        }
    }

    //
    if (m_rcEventThread != NULL)
    {
        // Stop may do significant work b/c if it drains the worker queue.
        m_rcEventThread->Stop();
        delete m_rcEventThread;
        m_rcEventThread = NULL;
    }


#ifdef _DEBUG
    m_processes.DebugSetRSLock(NULL);
#endif

    //
    // We expect the debugger to neuter all processes before calling Terminate(), so do not neuter them here.
    //

#ifdef _DEBUG
    {
        HASHFIND find;
        _ASSERTE(m_processes.FindFirst(&find) == NULL); // should be emptied by neuter
    }
#endif //_DEBUG

    // Officially mark us as neutered.

    NeuterTicket ticket(this);
    this->Neuter(ticket);

    m_processListMutex.Destroy();

    //
    // Release the metadata interfaces
    //
    m_pMetaDispenser.Clear();

    //
    // Release the callbacks
    //
    m_managedCallback.Clear();
    m_managedCallback2.Clear();
    m_unmanagedCallback.Clear();

    if (m_pCorHost)
    {
        m_pCorHost->Stop();
        m_pCorHost->Release();
        m_pCorHost = NULL;
    }


    // The Shell may still have outstanding references, so we don't want to shutdown logging yet.
    // But everything should be neutered anyways.

    m_initialized = FALSE;


    // After this, all outstanding Cordb objects should be neutered.
    LOG((LF_CORDB, LL_EVERYTHING, "Cordb finished terminating.\n"));

#if defined(_DEBUG)
    //
    // Assert that there are no outstanding object references within the debugging
    // API itself.
    //
    CheckMemLeaks();
#endif

    return S_OK;
}

HRESULT Cordb::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebug)
        *pInterface = static_cast<ICorDebug*>(this);
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebug*>(this));
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}


//
// Initialize -- setup the ICorDebug object by creating any objects
// that the object needs to operate and starting the two needed IPC
// threads.
//
HRESULT Cordb::Initialize(void)
{
    HRESULT hr = S_OK;

    FAIL_IF_NEUTERED(this);

    if (!m_initialized)
    {
        CordbCommonBase::InitializeCommon();
        
        // Since logging wasn't active when we called CordbBase, do it now.
        LOG((LF_CORDB, LL_EVERYTHING, "Memory: CordbBase object allocated: this=%p, count=%d, RootObject\n", this, s_TotalObjectCount));
        LOG((LF_CORDB, LL_INFO10, "Initializing ICorDebug...\n"));

        _ASSERTE(sizeof(DebuggerIPCEvent) <= CorDBIPC_BUFFER_SIZE);

        //
        // Init things that the Cordb will need to operate
        //
        m_processListMutex.Init("Process-List Lock", RSLock::cLockReentrant, RSLock::LL_PROCESS_LIST_LOCK);

#ifdef _DEBUG
        m_processes.DebugSetRSLock(&m_processListMutex);
#endif

        //
        // Create the runtime controller event listening thread
        //
        m_rcEventThread = new (nothrow) CordbRCEventThread(this);

        if (m_rcEventThread == NULL)
        {
            hr = E_OUTOFMEMORY;
        }
        else
        {
            // This stuff only creates events & starts the thread
            hr = m_rcEventThread->Init();

            if (SUCCEEDED(hr))
                hr = m_rcEventThread->Start();

            if (FAILED(hr))
            {
                delete m_rcEventThread;
                m_rcEventThread = NULL;
            }
        }

        if (FAILED(hr))
            goto exit;

        hr = CoCreateInstance(CLSID_CorMetaDataDispenser, NULL,
                              CLSCTX_INPROC_SERVER,
                              IID_IMetaDataDispenser,
                              (void**)&m_pMetaDispenser);

        if (FAILED(hr))
        {
            goto exit;
        }

        _ASSERTE(m_pMetaDispenser != NULL);

        m_initialized = TRUE;
    }

exit:
    return hr;
}

//
// Do we allow another process?
// This is highly dependent on the wait sets in the Win32 & RCET threads.
//
bool Cordb::AllowAnotherProcess()
{
    bool fAllow;

    LockProcessList();

    // Cordb, Win32, and RCET all have process sets, but Cordb's is the
    // best count of total debuggees. The RCET set is volatile (processes
    // are added / removed when they become synchronized), and Win32's set
    // doesn't include all processes.
    int cCurProcess = GetProcessList()->GetCount();

    // In order to accept another debuggee, we must have a free slot in all
    // wait sets. Currently, we don't expose the size of those sets, but
    // we know they're MAXIMUM_WAIT_OBJECTS. Note that we lose one slot
    // to the control event.
    if (cCurProcess >= MAXIMUM_WAIT_OBJECTS - 1)
    {
        fAllow = false;
    } else {
        fAllow = true;
    }

    UnlockProcessList();

    return fAllow;
}

//
// AddProcess -- add a process object to this ICorDebug's hash of processes.
// This also tells this ICorDebug's runtime controller thread that the
// process set has changed so it can update its list of wait events.
//
HRESULT Cordb::AddProcess(CordbProcess* process)
{
    // At this point, we should have already checked that we
    // can have another debuggee.
    _ASSERTE(AllowAnotherProcess());
    STRESS_LOG1(LF_CORDB, LL_INFO10, "Cordb::AddProcess %08x...\n", process);

    FAIL_IF_NEUTERED(this);

    if ((m_managedCallback == NULL) || (m_managedCallback2 == NULL))
    {
        return E_FAIL;
    }

    LockProcessList();

    // Once we add another process, all outstanding process-enumerators become invalid.
    NeuterTicket ticket(this);
    m_pProcessEnumList.NeuterAndClear(ticket);


    HRESULT hr = GetProcessList()->AddBase(process);

    if (SUCCEEDED(hr))
    {
        m_rcEventThread->ProcessStateChanged();
    }

    UnlockProcessList();

    return hr;
}

//
// RemoveProcess -- remove a process object from this ICorDebug's hash of
// processes. This also tells this ICorDebug's runtime controller thread
// that the process set has changed so it can update its list of wait events.
//
void Cordb::RemoveProcess(CordbProcess* process)
{
    STRESS_LOG1(LF_CORDB, LL_INFO10, "Cordb::RemoveProcess %08x...\n", process);

    LockProcessList();
    GetProcessList()->RemoveBase((ULONG_PTR)process->m_id);

    m_rcEventThread->ProcessStateChanged();

    UnlockProcessList();
}

//
// LockProcessList -- Lock the process list.
//
void Cordb::LockProcessList(void)
{
    m_processListMutex.Lock();
}

//
// UnlockProcessList -- Unlock the process list.
//
void Cordb::UnlockProcessList(void)
{
    m_processListMutex.Unlock();
}

#ifdef _DEBUG
// Return true iff this thread owns the ProcessList lock
bool Cordb::ThreadHasProcessListLock()
{
    return m_processListMutex.HasLock();
}
#endif


// Get the hash that has the process.
CordbSafeHashTable<CordbProcess> *Cordb::GetProcessList()
{
    // If we're accessing the hash, we'd better be locked.
    _ASSERTE(ThreadHasProcessListLock());

    return &m_processes;
}


HRESULT Cordb::SendIPCEvent(CordbProcess* process,
                            DebuggerIPCEvent* event,
                            SIZE_T eventSize)
{
    LOG((LF_CORDB, LL_EVERYTHING, "SendIPCEvent in Cordb called\n"));
    return m_rcEventThread->SendIPCEvent(process, event, eventSize);
}


void Cordb::ProcessStateChanged(void)
{
    m_rcEventThread->ProcessStateChanged();
}


HRESULT Cordb::WaitForIPCEventFromProcess(CordbProcess* process,
                                          CordbAppDomain *pAppDomain,
                                          DebuggerIPCEvent* event)
{
    return m_rcEventThread->WaitForIPCEventFromProcess(process,
                                                       pAppDomain,
                                                       event);
}

HRESULT Cordb::GetFirstContinuationEvent(CordbProcess *process,
                                         DebuggerIPCEvent *event)
{
    return m_rcEventThread->ReadRCEvent(process,
                                        event);
}

HRESULT Cordb::GetNextContinuationEvent(CordbProcess *process,
                                        DebuggerIPCEvent *event)
{
    _ASSERTE( event->next != NULL );
    if ( event->next == NULL)
        return E_FAIL;

    m_rcEventThread->CopyRCEvent((BYTE*)event->next, (BYTE*)event);

    return S_OK;
}

HRESULT Cordb::GetCorRuntimeHost(ICorRuntimeHost **ppHost)
{
    // If its already created, pass it out with an extra reference.
    if (m_pCorHost != NULL)
    {
        m_pCorHost->AddRef();
        *ppHost = m_pCorHost;
        return S_OK;
    }

    HRESULT hr = CoCreateInstance(CLSID_CorRuntimeHost,
                                  NULL,
                                  CLSCTX_INPROC_SERVER,
                                  IID_ICorRuntimeHost,
                                  (void**)&m_pCorHost);

    if (SUCCEEDED(hr))
    {
        // Start it up.
        hr = m_pCorHost->Start();

        if (SUCCEEDED(hr))
        {
            *ppHost = m_pCorHost;

            // Keep a ref for ourselves.
            m_pCorHost->AddRef();
        }
        else
        {
            m_pCorHost->Release();
            m_pCorHost = NULL;
        }
    }

    return hr;
}



HRESULT Cordb::SetManagedHandler(ICorDebugManagedCallback *pCallback)
{
    if (!m_initialized)
        return E_FAIL;

    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pCallback, ICorDebugManagedCallback*);

    m_managedCallback.Clear();
    m_managedCallback2.Clear();

    // For SxS, V2.0 debuggers must implement ManagedCallback2 to handle v2.0 debug events.
    // For Single-CLR, A v1.0 debugger may actually geta V2.0 debuggee.
    pCallback->QueryInterface(IID_ICorDebugManagedCallback2, (void **)&m_managedCallback2);
    if (m_managedCallback2 == NULL)
    {
        if (GetDebuggerVersion() >= CorDebugVersion_2_0)
        {
            // This will leave our internal callbacks null, which future operations (Create/Attach) will
            // use to know that we're not sufficiently initialized.
            return E_NOINTERFACE;
        }
        else
        {
            // This should only be used in a single-CLR shimming scenario.
            m_managedCallback2.Assign(new (nothrow) DefaultManagedCallback2(this));

            if (m_managedCallback2 == NULL)
            {
                return E_OUTOFMEMORY;
            }
        }
    }

    m_managedCallback.Assign(pCallback);


    return S_OK;
}

HRESULT Cordb::SetUnmanagedHandler(ICorDebugUnmanagedCallback *pCallback)
{
    if (!m_initialized)
        return E_FAIL;

    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pCallback, ICorDebugUnmanagedCallback*);

    m_unmanagedCallback.Assign(pCallback);

    return S_OK;
}



// Determine if interop-debugging is supported on the given platform.
inline bool DoesPlatformSupportInteropDebugging()
{
    // Interop debugging is only supported on x86, and only on non-Win9X OS's.
    return false;
}

// Given everything we know about our configuration, can we support interop-debugging
bool Cordb::IsInteropDebuggingSupported()
{
    // We explicitly refrain from checking the unmanaged callback. See comment in
    // ICorDebug::SetUnmanagedHandler for details.
    return DoesPlatformSupportInteropDebugging();
}


HRESULT Cordb::CreateProcess(LPCWSTR lpApplicationName,
                             __in_z LPWSTR lpCommandLine,
                             LPSECURITY_ATTRIBUTES lpProcessAttributes,
                             LPSECURITY_ATTRIBUTES lpThreadAttributes,
                             BOOL bInheritHandles,
                             DWORD dwCreationFlags,
                             PVOID lpEnvironment,
                             LPCWSTR lpCurrentDirectory,
                             LPSTARTUPINFOW lpStartupInfo,
                             LPPROCESS_INFORMATION lpProcessInformation,
                             CorDebugCreateProcessFlags debuggingFlags,
                             ICorDebugProcess **ppProcess)
{
    //_ASSERTE(!IsWin32EventThread(this));

    // If you hit this assert, it means that you are attempting to create a process without specifying the version
    // number.
    _ASSERTE(CorDebugInvalidVersion != m_debuggerSpecifiedVersion);

    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppProcess, ICorDebugProcess**);

    if (!m_initialized)
        return E_FAIL;



    // Must have a managed-callback by now.
    if ((m_managedCallback == NULL) || (m_managedCallback2 == NULL))
    {
        return E_FAIL;
    }


    // Check that we can even accept another debuggee before trying anything.
    if (!AllowAnotherProcess())
    {
        return CORDBG_E_TOO_MANY_PROCESSES;
    }

    HRESULT hr = S_OK;

    hr = CordbProcess::CreateProcess(this,
                                                    lpApplicationName,
                                                    lpCommandLine,
                                                    lpProcessAttributes,
                                                    lpThreadAttributes,
                                                    bInheritHandles,
                                                    dwCreationFlags,
                                                    lpEnvironment,
                                                    lpCurrentDirectory,
                                                    lpStartupInfo,
                                                    lpProcessInformation,
                                                    debuggingFlags
                                                    );
    LOG((LF_CORDB, LL_EVERYTHING, "Handle in Cordb::CreateProcess is: %.I64x\n", lpProcessInformation->hProcess));

    if (SUCCEEDED(hr))
    {
        LockProcessList();
        CordbProcess *process = GetProcessList()->GetBase(lpProcessInformation->dwProcessId);
        UnlockProcessList();

        PREFIX_ASSUME(process != NULL);

        process->ExternalAddRef();
        *ppProcess = (ICorDebugProcess*) process;

        // also indicate that this process was started under the debugger
        // as opposed to attaching later.
        process->m_attached = false;
    }

    return hr;
}

HRESULT Cordb::DebugActiveProcess(DWORD processId,
                                  BOOL win32Attach,
                                  ICorDebugProcess **ppProcess)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppProcess, ICorDebugProcess **);

    if (!m_initialized)
    {
        STRESS_LOG0(LF_CORDB, LL_INFO10, "Cordb::DebugActiveProcess failed with not initialized\n");

        return E_FAIL;
    }

    // Must have a managed-callback by now.
    if ((m_managedCallback == NULL) || (m_managedCallback2 == NULL))
    {
        return E_FAIL;
    }

    //_ASSERTE(!IsWin32EventThread(this));

    // See the comment in Cordb::CreateProcess
    _ASSERTE(CorDebugInvalidVersion != m_debuggerSpecifiedVersion);

    // Check that we can even accept another debuggee before trying anything.
    if (!AllowAnotherProcess())
    {
        STRESS_LOG0(LF_CORDB, LL_INFO10, "Cordb::DebugActiveProcess failed too many process\n");
        return CORDBG_E_TOO_MANY_PROCESSES;
    }


    // Check if we're allowed to do interop.
    bool fAllowInterop = IsInteropDebuggingSupported();



    if (!fAllowInterop && win32Attach)
    {
        return CORDBG_E_INTEROP_NOT_SUPPORTED;
    }


    HRESULT hr = CordbProcess::DebugActiveProcess(this,
                                                        processId,
                                                        win32Attach == TRUE
                                                        );

    // If that worked, then there will be a process object...
    if (SUCCEEDED(hr))
    {
        LockProcessList();
        CordbProcess* process =
            GetProcessList()->GetBase(processId);
        if (process != NULL)
        {
            // Add a reference now so process won't go away
            process->ExternalAddRef();
        }
        UnlockProcessList();

        if (process == NULL)
        {
            // This can happen if we add the process into process hash in
            // SendDebugActiveProcessEvent and then process exit
            // before we attemp to retrieve it again from GetBase.
            //
            *ppProcess = NULL;
            return S_FALSE;
        }
        PREFIX_ASSUME(process != NULL);

        // If we're doing an interop attach, then we don't start the managed attach
        // until _after_ Cordbg continues from the load-complete breakpoint.
        if (process->m_sendAttachIPCEvent)
        {
            // LS has already loaded & initialed the runtime.
            // We will need to initiate a managed attach.
            if (!win32Attach)
            {
                // If we're managed-only, we can do the Managed-attach now.
                process->Lock();
                hr = process->QueueManagedAttach();
                process->Unlock();

            }
            else
            {
                // If we're interop, then do the managed-attach at the loader-bp.
                process->m_fDoDelayedManagedAttached = true;
            }

        }
        else
        {
            // We created the SSE. We attached before the LS became managed.
            // We don't ever need to do a managed attach b/c if the LS ever becomes
            // managed, then it will attach to us! (ie, we've degenerated into
            // the Create case)
            // (It doesn't matter if we're managed-only or interop).
        }

        *ppProcess = (ICorDebugProcess*) process;

        // also indicate that this process was attached to, as
        // opposed to being started under the debugger.
        process->m_attached = true;
    }

    return hr;
}

HRESULT Cordb::GetProcess(DWORD dwProcessId, ICorDebugProcess **ppProcess)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppProcess, ICorDebugProcess**);

    if (!m_initialized)
    {
        return E_FAIL;
    }

    LockProcessList();
    CordbProcess *p = GetProcessList()->GetBase(dwProcessId);
    UnlockProcessList();

    if (p == NULL)
        return E_INVALIDARG;

    p->ExternalAddRef();
    *ppProcess = static_cast<ICorDebugProcess*> (p);

    return S_OK;
}

HRESULT Cordb::EnumerateProcesses(ICorDebugProcessEnum **ppProcesses)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppProcesses, ICorDebugProcessEnum **);

    if (!m_initialized)
        return E_FAIL;

    // Locking here just means that the enumerator gets initialized against a consistent
    // process-list. If we add/remove processes w/ an outstanding enumerator, things
    // could still get out of sync.
    LockProcessList();
    CordbHashTableEnum *e = new (nothrow) CordbHashTableEnum(this, &m_pProcessEnumList, GetProcessList(),
                                                   IID_ICorDebugProcessEnum);
    UnlockProcessList();

    if (e == NULL)
        return E_OUTOFMEMORY;

    e->ExternalAddRef();
    *ppProcesses = static_cast<ICorDebugProcessEnum*> (e);

    return S_OK;
}



HRESULT Cordb::CanLaunchOrAttach(DWORD dwProcessId, BOOL win32DebuggingEnabled)
{
    if (!m_initialized)
        return E_FAIL;

    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    if (!AllowAnotherProcess())
        return CORDBG_E_TOO_MANY_PROCESSES;


    return S_OK;
}

HRESULT Cordb::CreateObjectV1(REFIID id, void **object)
{
    return CreateObject(CorDebugVersion_1_0, id, object);
}

// Static
// Used to create an instance for a ClassFactory (thus an external ref).
HRESULT Cordb::CreateObject(CorDebugInterfaceVersion iDebuggerVersion, REFIID id, void **object)
{
    if (id != IID_IUnknown && id != IID_ICorDebug)
        return (E_NOINTERFACE);

    Cordb *db = new (nothrow) Cordb(iDebuggerVersion);

    if (db == NULL)
        return (E_OUTOFMEMORY);

    *object = static_cast<ICorDebug*> (db);
    db->ExternalAddRef();

    return (S_OK);
}


// This is the version of the ICorDebug APIs that the debugger believes it's consuming.
// If this is a different version than that of the debuggee, we have the option of shimming
// behavior.
CorDebugInterfaceVersion
Cordb::GetDebuggerVersion() const
{
    return m_debuggerSpecifiedVersion;
}

//***********************************************************************
//              ICorDebugTMEnum (Thread and Module enumerator)
//***********************************************************************
CordbEnumFilter::CordbEnumFilter(CordbBase * pOwnerObj, NeuterList * pOwnerList)
    : CordbBase (pOwnerObj->GetProcess(), 0),
    m_pOwnerObj(pOwnerObj),
    m_pOwnerNeuterList(pOwnerList),
    m_pFirst (NULL),
    m_pCurrent (NULL),
    m_iCount (0)
{
    _ASSERTE(m_pOwnerNeuterList != NULL);
    m_pOwnerNeuterList->Add(this);

}

CordbEnumFilter::CordbEnumFilter(CordbEnumFilter *src)
    : CordbBase (src->GetProcess(), 0),
    m_pOwnerObj(src->m_pOwnerObj),
    m_pOwnerNeuterList(src->m_pOwnerNeuterList),
    m_pFirst (NULL),
    m_pCurrent (NULL)
{
    _ASSERTE(m_pOwnerNeuterList != NULL);
    m_pOwnerNeuterList->Add(this);


    int iCountSanityCheck = 0;
    EnumElement *pElementCur = NULL;
    EnumElement *pElementNew = NULL;
    EnumElement *pElementNewPrev = NULL;

    m_iCount = src->m_iCount;

    pElementCur = src->m_pFirst;

    while (pElementCur != NULL)
    {
        pElementNew = new (nothrow) EnumElement;
        if (pElementNew == NULL)
        {
            // Out of memory. Clean up and bail out.
            goto Error;
        }

        if (pElementNewPrev == NULL)
        {
            m_pFirst = pElementNew;
        }
        else
        {
            pElementNewPrev->SetNext(pElementNew);
        }

        pElementNewPrev = pElementNew;

        // Copy the element, including the AddRef part
        pElementNew->SetData(pElementCur->GetData());
        IUnknown *iu = (IUnknown *)pElementCur->GetData();
        iu->AddRef();

        if (pElementCur == src->m_pCurrent)
            m_pCurrent = pElementNew;

        pElementCur = pElementCur->GetNext();
        iCountSanityCheck++;
    }

    _ASSERTE(iCountSanityCheck == m_iCount);

    return;
Error:
    // release all the allocated memory before returning
    pElementCur = m_pFirst;

    while (pElementCur != NULL)
    {
        pElementNewPrev = pElementCur;
        pElementCur = pElementCur->GetNext();

        ((ICorDebugModule *)pElementNewPrev->GetData())->Release();
        delete pElementNewPrev;
    }
}

CordbEnumFilter::~CordbEnumFilter()
{
    _ASSERTE(this->IsNeutered());

    _ASSERTE(m_pFirst == NULL);
}

void CordbEnumFilter::Neuter(NeuterTicket ticket)
{
    EnumElement *pElement = m_pFirst;
    EnumElement *pPrevious = NULL;

    while (pElement != NULL)
    {
        pPrevious = pElement;
        pElement = pElement->GetNext();
        delete pPrevious;
    }

    // Null out the head in case we get neutered again.
    m_pFirst = NULL;
    m_pCurrent = NULL;

    CordbBase::Neuter(ticket);
}



HRESULT CordbEnumFilter::QueryInterface(REFIID id, void **ppInterface)
{
    if (id == IID_ICorDebugModuleEnum)
        *ppInterface = (ICorDebugModuleEnum*)this;
    else if (id == IID_ICorDebugThreadEnum)
        *ppInterface = (ICorDebugThreadEnum*)this;
    else if (id == IID_IUnknown)
        *ppInterface = this;
    else
    {
        *ppInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

HRESULT CordbEnumFilter::Skip(ULONG celt)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    while (celt-- > 0 && m_pCurrent != NULL)
        m_pCurrent = m_pCurrent->GetNext();

    return S_OK;
}

HRESULT CordbEnumFilter::Reset()
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    m_pCurrent = m_pFirst;

    return S_OK;
}

HRESULT CordbEnumFilter::Clone(ICorDebugEnum **ppEnum)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(ppEnum, ICorDebugEnum **);

    CordbEnumFilter *clone = new (nothrow) CordbEnumFilter(this);

    if (NULL == clone)
        return E_OUTOFMEMORY;

    clone->ExternalAddRef();
    (*ppEnum) = static_cast<ICorDebugThreadEnum *> (clone);

    return S_OK;
}

HRESULT CordbEnumFilter::GetCount(ULONG *pcelt)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    VALIDATE_POINTER_TO_OBJECT(pcelt, ULONG *);

    *pcelt = (ULONG)m_iCount;
    return S_OK;
}

HRESULT CordbEnumFilter::Next(ULONG celt,
                ICorDebugModule *objects[],
                ULONG *pceltFetched)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());


    //
    VALIDATE_POINTER_TO_OBJECT_ARRAY(objects, ICorDebugModule *,
        celt, true, true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pceltFetched, ULONG *);

    if ((pceltFetched == NULL) && (celt != 1))
    {
        return E_INVALIDARG;
    }

    if (celt == 0)
    {
        if (pceltFetched != NULL)
        {
            *pceltFetched = 0;
        }
        return S_OK;
    }

    HRESULT hr = S_OK;

    ULONG count = 0;

    while ((m_pCurrent != NULL) && (count < celt))
    {
        objects[count] = (ICorDebugModule *)m_pCurrent->GetData();
        m_pCurrent = m_pCurrent->GetNext();
        count++;
    }

    if (pceltFetched != NULL)
    {
        *pceltFetched = count;
    }

    //
    // If we reached the end of the enumeration, but not the end
    // of the number of requested items, we return S_FALSE.
    //
    if (count < celt)
    {
        return S_FALSE;
    }

    return hr;
}


HRESULT CordbEnumFilter::Next(ULONG celt,
                ICorDebugThread *objects[],
                ULONG *pceltFetched)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());


    VALIDATE_POINTER_TO_OBJECT_ARRAY(objects, ICorDebugThread *,
        celt, true, true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pceltFetched, ULONG *);

    if ((pceltFetched == NULL) && (celt != 1))
    {
        return E_INVALIDARG;
    }

    if (celt == 0)
    {
        if (pceltFetched != NULL)
        {
            *pceltFetched = 0;
        }
        return S_OK;
    }

    HRESULT hr = S_OK;

    ULONG count = 0;

    while ((m_pCurrent != NULL) && (count < celt))
    {
        objects[count] = (ICorDebugThread *)m_pCurrent->GetData();
        m_pCurrent = m_pCurrent->GetNext();
        count++;
    }

    if (pceltFetched != NULL)
    {
        *pceltFetched = count;
    }

    //
    // If we reached the end of the enumeration, but not the end
    // of the number of requested items, we return S_FALSE.
    //
    if (count < celt)
    {
        return S_FALSE;
    }

    return hr;
}



HRESULT CordbEnumFilter::Init (ICorDebugModuleEnum *pModEnum, CordbAssembly *pAssembly)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess());

    ICorDebugModule *pCorModule = NULL;
    CordbModule *pModule = NULL;
    ULONG ulDummy = 0;

    HRESULT hr = pModEnum->Next(1, &pCorModule, &ulDummy);

    //
    // Next returns E_FAIL if there is no next item, along with
    // the count being 0.  Convert that to just being S_OK.
    //
    if ((hr == E_FAIL) && (ulDummy == 0))
    {
        hr = S_OK;
    }

    if (FAILED (hr))
        return hr;

    EnumElement *pPrevious = NULL;
    EnumElement *pElement = NULL;

    while (ulDummy != 0)
    {
        pModule = (CordbModule *)(ICorDebugModule *)pCorModule;
        // Is this module part of the assembly for which we're enumerating?
        if (pModule->m_pAssembly == pAssembly)
        {
            pElement = new (nothrow) EnumElement;
            if (pElement == NULL)
            {
                // Out of memory. Clean up and bail out.
                hr = E_OUTOFMEMORY;
                goto Error;
            }

            pElement->SetData ((void *)pCorModule);
            m_iCount++;

            if (m_pFirst == NULL)
            {
                m_pFirst = pElement;
            }
            else
            {
                PREFIX_ASSUME(pPrevious != NULL);
                pPrevious->SetNext (pElement);
            }
            pPrevious = pElement;
        }
        else
            ((ICorDebugModule *)pModule)->Release();

        hr = pModEnum->Next(1, &pCorModule, &ulDummy);

        //
        // Next returns E_FAIL if there is no next item, along with
        // the count being 0.  Convert that to just being S_OK.
        //
        if ((hr == E_FAIL) && (ulDummy == 0))
        {
            hr = S_OK;
        }

        if (FAILED (hr))
            goto Error;
    }

    m_pCurrent = m_pFirst;

    return S_OK;

Error:
    // release all the allocated memory before returning
    pElement = m_pFirst;

    while (pElement != NULL)
    {
        pPrevious = pElement;
        pElement = pElement->GetNext();

        ((ICorDebugModule *)pPrevious->GetData())->Release();
        delete pPrevious;
    }

    return hr;
}

HRESULT CordbEnumFilter::Init (ICorDebugThreadEnum *pThreadEnum, CordbAppDomain *pAppDomain)
{
    INTERNAL_SYNC_API_ENTRY(GetProcess());

    ICorDebugThread *pCorThread = NULL;
    CordbThread *pThread = NULL;
    ULONG ulDummy = 0;

    HRESULT hr = pThreadEnum->Next(1, &pCorThread, &ulDummy);

    if ((hr == E_FAIL) && (ulDummy == 0))
    {
        hr = S_OK;
    }

    if (FAILED(hr))
    {
        return hr;
    }

    EnumElement *pPrevious = NULL;
    EnumElement *pElement = NULL;

    while (ulDummy > 0)
    {
        pThread = (CordbThread *)(ICorDebugThread *) pCorThread;

        // Is this module part of the appdomain for which we're enumerating?
        // Note that this is rather inefficient (we call into the left side for every AppDomain),
        // but the whole idea of enumerating the threads of an AppDomain is pretty bad,
        // and we don't expect this to be used much if at all.
        CordbAppDomain* pThreadDomain;
        hr = pThread->GetCurrentAppDomain( &pThreadDomain );
        if( FAILED(hr) )
        {
            goto Error;
        }
       
        if (pThreadDomain == pAppDomain)
        {
            pElement = new (nothrow) EnumElement;
            if (pElement == NULL)
            {
                // Out of memory. Clean up and bail out.
                hr = E_OUTOFMEMORY;
                goto Error;
            }

            pElement->SetData ((void *)pCorThread);
            m_iCount++;

            if (m_pFirst == NULL)
            {
                m_pFirst = pElement;
            }
            else
            {
                PREFIX_ASSUME(pPrevious != NULL);
                pPrevious->SetNext (pElement);
            }

            pPrevious = pElement;
        }
        else
        {
            ((ICorDebugThread *)pThread)->Release();
        }

        //  get the next thread in the thread list
        hr = pThreadEnum->Next(1, &pCorThread, &ulDummy);

        //
        // Next returns E_FAIL if there is no next item, along with
        // the count being 0.  Convert that to just being S_OK.
        //
        if ((hr == E_FAIL) && (ulDummy == 0))
        {
            hr = S_OK;
        }

        if (FAILED (hr))
            goto Error;
    }

    m_pCurrent = m_pFirst;

    return S_OK;

Error:
    // release all the allocated memory before returning
    pElement = m_pFirst;

    while (pElement != NULL)
    {
        pPrevious = pElement;
        pElement = pElement->GetNext();

        ((ICorDebugThread *)pPrevious->GetData())->Release();
        delete pPrevious;
    }

    return hr;
}

