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
// File: process.cpp
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
#include "corexcep.h"
#include "../../dlls/mscorrc/resource.h"
#include <limits.h>

#include "dacprivate.h"
#include <sstring.h>

// Keep this around for retail debugging. It's very very useful because
// it's global state that we can always find, regardless of how many locals the compiler
// optimizes away ;)
struct RSDebuggingInfo;
extern RSDebuggingInfo * g_pRSDebuggingInfo;


//-----------------------------------------------------------------------------
// Most Hresults to Unrecoverable error indicate an internal error
// in the Right-Side.
// However, a few are legal (eg, "could actually happen in a retail scenario and
// not indicate a bug in mscorbi"). Track that here.
//-----------------------------------------------------------------------------
bool IsLegalFatalError(HRESULT hr)
{
    return
        (hr == CORDBG_E_INCOMPATIBLE_PROTOCOL) ||
        (hr == CORDBG_E_CANNOT_DEBUG_FIBER_PROCESS) ||
        (hr == CORDBG_E_UNCOMPATIBLE_PLATFORMS) ||
        // This should only happen in the case of a security attack on us.
        (hr == E_ACCESSDENIED) ||
        (hr == E_FAIL);
}

//-----------------------------------------------------------------------------
// Safe wait. Use this anytime we're waiting on:
// - an event signaled by the helper thread.
// - something signaled by a thread that holds the process lock.
// Note that we must preserve GetLastError() semantics.
//-----------------------------------------------------------------------------
inline DWORD SafeWaitForSingleObject(CordbProcess * p, HANDLE h, DWORD dwTimeout)
{
    // Can't hold process lock while blocking
    _ASSERTE(!p->ThreadHoldsProcessLock());

    return ::WaitForSingleObject(h, dwTimeout);
}

#define CORDB_WAIT_TIMEOUT 360000 // milliseconds

static inline DWORD CordbGetWaitTimeout()
{
#ifdef _DEBUG
    // 0 = Wait forever
    // 1 = Wait for CORDB_WAIT_TIMEOUT
    // n = Wait for n milliseconds
    static ConfigDWORD cordbWaitTimeout;
    DWORD timeoutVal = cordbWaitTimeout.val(L"DbgWaitTimeout", 1);
    if (timeoutVal == 0)
        return DWORD(-1);
    else if (timeoutVal != 1)
        return timeoutVal;
    else
#endif
    {
        return CORDB_WAIT_TIMEOUT;
    }
}

/* ------------------------------------------------------------------------- *
 * Process class
 * ------------------------------------------------------------------------- */

CordbProcess::CordbProcess(Cordb* cordb, CordbWin32EventThread * pW32, DWORD processID, HANDLE handle)
  : CordbBase(NULL, processID, enumCordbProcess),
    m_fDoDelayedManagedAttached(false),
    m_cHijackedThreads(0),
    m_cordb(cordb), 
    m_handle(handle),
    m_attached(false), m_detached(false), m_uninitializedStop(false),
    m_creating(false),
    m_exiting(false),
    m_firstExceptionHandled(false),
    m_terminated(false),
    m_unrecoverableError(false), m_sendAttachIPCEvent(false),
    m_firstManagedEvent(false), m_specialDeferment(false),
    m_helperThreadDead(false),
    m_loaderBPReceived(false),
    m_cOutstandingEvals(0),
    m_stopCount(0),
    m_synchronized(false),
    m_syncCompleteReceived(false),
    m_win32EventThread(pW32),
    m_oddSync(false),
    m_userThreads(11),
    m_unmanagedThreads(11),
    m_appDomains(11),
    m_steppers(11),
    m_continueCounter(1),
    m_DCB(NULL),
    m_leftSideEventAvailable(NULL),
    m_leftSideEventRead(NULL),
    m_rightSideEventAvailable(NULL),
    m_rightSideEventRead(NULL),
    m_leftSideUnmanagedWaitEvent(NULL),
    m_syncThreadIsLockFree(NULL),
    m_SetupSyncEvent(NULL),
    m_initialized(false),
    m_queuedEventList(NULL),
    m_lastQueuedEvent(NULL),
    m_stopRequested(false),
    m_stopWaitEvent(NULL),
    m_lastIBStoppingEvent(NULL),
    m_dispatchingUnmanagedEvent(false),
    m_dispatchingOOBEvent(false),
    m_doRealContinueAfterOOBBlock(false),
    m_deferContinueDueToOwnershipWait(false),
    m_helperThreadId(0),
    m_state(0),
    m_pPatchTable(NULL),
    m_cPatch(0),
    m_rgData(NULL),
    m_rgNextPatch(NULL),
    m_rgUncommitedOpcode(NULL),
    m_minPatchAddr(MAX_ADDRESS),
    m_maxPatchAddr(MIN_ADDRESS),
    m_iFirstPatch(0),
    m_hHelperThread(NULL),
    m_dispatchedEvent(DB_IPCE_DEBUGGER_INVALID),
    m_pDefaultAppDomain(NULL)    

{
    _ASSERTE(pW32 != NULL);

    m_DbgSupport.m_DebugEventQueueIdx = 0;
    m_DbgSupport.m_TotalNativeEvents = 0;
    m_DbgSupport.m_TotalIB = 0;
    m_DbgSupport.m_TotalOOB = 0;
    m_DbgSupport.m_TotalCLR = 0;

    g_pRSDebuggingInfo->m_MRUprocess = this;

    m_pProcess.Assign(this);
}

/*
    A list of which resources owned by this object are accounted for.

    UNKNOWN
        Cordb*                      m_cordb;
        CordbHashTable              m_unmanagedThreads; // Released in CordbProcess but not removed from hash
        DebuggerIPCControlBlock     *m_DCB;
        DebuggerIPCEvent*           m_lastQueuedEvent;

        // CordbUnmannagedEvent is a struct which is not derrived from CordbBase.
        // It contains a CordbUnmannagedThread which may need to be released.
        CordbUnmanagedEvent         *m_unmanagedEventQueue;
        CordbUnmanagedEvent         *m_lastQueuedUnmanagedEvent;
        CordbUnmanagedEvent         *m_lastIBStoppingEvent;
        CordbUnmanagedEvent         *m_outOfBandEventQueue;
        CordbUnmanagedEvent         *m_lastQueuedOOBEvent;

        BYTE*                       m_pPatchTable;
        BYTE                        *m_rgData;
        void                        *m_pbRemoteBuf;

   RESOLVED
        // Nutered
        CordbHashTable        m_userThreads;
        CordbHashTable        m_appDomains;

        // Cleaned up in ExitProcess
        HANDLE                m_SetupSyncEvent;
        DebuggerIPCEvent*     m_queuedEventList;

        CordbHashTable        m_steppers; // Closed in ~CordbProcess

        // Closed in CloseIPCEventHandles called from ~CordbProcess
        HANDLE                m_leftSideEventAvailable;
        HANDLE                m_leftSideEventRead;

        // Closed in ~CordbProcess
        HANDLE                m_handle;
        HANDLE                m_rightSideEventAvailable;
        HANDLE                m_rightSideEventRead;
        HANDLE                m_leftSideUnmanagedWaitEvent;
        HANDLE                m_syncThreadIsLockFree;
        HANDLE                m_stopWaitEvent;

        // Deleted in ~CordbProcess
        CRITICAL_SECTION      m_processMutex;
*/


CordbProcess::~CordbProcess()
{
    LOG((LF_CORDB, LL_INFO1000, "CP::~CP: deleting process 0x%08x\n", this));

    DTOR_ENTRY(this);

    _ASSERTE(IsNeutered());

    _ASSERTE(m_cordb == NULL);



    m_processMutex.Destroy();
    m_StopGoLock.Destroy();

    // These handles were cleared in neuter
    _ASSERTE(m_handle == NULL); 
    _ASSERTE(m_rightSideEventAvailable == NULL);
    _ASSERTE(m_rightSideEventRead == NULL);
    _ASSERTE(m_leftSideUnmanagedWaitEvent == NULL);
    _ASSERTE(m_syncThreadIsLockFree == NULL);
    _ASSERTE(m_stopWaitEvent == NULL);

    
    // Set this to mark that we really did cleanup.
}

//-----------------------------------------------------------------------------
// Static build helper.
// This will create a process under the pCordb root, and add it to the list.
// We don't return the process - caller gets the pid and looks it up under
// the Cordb object.
//-----------------------------------------------------------------------------
HRESULT CordbProcess::CreateProcess(
      Cordb * pCordb,
      LPCWSTR programName,
      __in_z LPWSTR  programArgs,
      LPSECURITY_ATTRIBUTES lpProcessAttributes,
      LPSECURITY_ATTRIBUTES lpThreadAttributes,
      BOOL bInheritHandles,
      DWORD dwCreationFlags,
      PVOID lpEnvironment,
      LPCWSTR lpCurrentDirectory,
      LPSTARTUPINFOW lpStartupInfo,
      LPPROCESS_INFORMATION lpProcessInformation,
      CorDebugCreateProcessFlags corDebugFlags
)
{
    _ASSERTE(pCordb != NULL);

    HRESULT hr = S_OK;
    CordbWin32EventThread * pW32 = NULL;
    hr = CreateAndStartWin32ET(pCordb, &pW32);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = pW32->SendCreateProcessEvent(programName,
                                      programArgs,
                                      lpProcessAttributes,
                                      lpThreadAttributes,
                                      bInheritHandles,
                                      dwCreationFlags,
                                      lpEnvironment,
                                      lpCurrentDirectory,
                                      lpStartupInfo,
                                      lpProcessInformation,
                                      corDebugFlags);

    // If this succeeds, then process takes ownership of thread. Else we need to kill it.
    if (FAILED(hr))
    {
        pW32->Stop();
        delete pW32;
    }

    return hr;
}

//-----------------------------------------------------------------------------
// Static build helper for the attach case.
// On success, this will add the process to the pCordb list, and then
// callers can look it up there by pid.
//
// pCordb - root under which this all lives
// pid - OS process ID to attach to
// win32Attach - are we interop debugging?
//-----------------------------------------------------------------------------
HRESULT CordbProcess::DebugActiveProcess(
    Cordb * pCordb,
    DWORD pid,
    BOOL win32Attach
)
{
    _ASSERTE(pCordb != NULL);

    HRESULT hr = S_OK;
    CordbWin32EventThread * pW32 = NULL;
    hr = CreateAndStartWin32ET(pCordb, &pW32);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = pW32->SendDebugActiveProcessEvent(pid,
                                           win32Attach == TRUE,
                                           NULL);


    // If this succeeds, then process takes ownership of thread. Else we need to kill it.
    if (FAILED(hr))
    {
        pW32->Stop();
        delete pW32;
    }

    return hr;
}


// Neuter all of all children, but not the actual process object.
// Once we neuter ourself, we can no longer send IPC events. So this is useful
// on detach.
void CordbProcess::NeuterChildren(NeuterTicket ticket)
{
    _ASSERTE(m_StopGoLock.HasLock());

    m_EvalTable.Clear();    

    // Sweep neuter lists.
    m_ExitNeuterList.NeuterAndClear(ticket);
    m_ContinueNeuterList.NeuterAndClear(ticket);

    NeuterAndClearHashtable(&m_userThreads, ticket);
    NeuterAndClearHashtable(&m_appDomains, ticket);
    NeuterAndClearHashtable(&m_steppers, ticket);

    NeuterAndClearHashtable(&m_unmanagedThreads, ticket);

    // Explicitly keep the Win32EventThread alive so that we can use it in the window
    // between NeuterChildren + Neuter.
}

// Neutered when process dies.
void CordbProcess::Neuter(NeuterTicket ticket)
{
    _ASSERTE(!this->ThreadHoldsProcessLock());

    NeuterChildren(ticket);

    if (m_hHelperThread != NULL)
    {
        CloseHandle(m_hHelperThread);
        m_hHelperThread = NULL;
    }

    // Now that all of our children are neutered, it should be safe to kill the W32ET.
    if (m_win32EventThread != NULL)
    {
        // This will block waiting for the thread to exit gracefully.
        m_win32EventThread->Stop();

        delete m_win32EventThread;
        m_win32EventThread = NULL;
    }

    {
        // Nead process lock to clear the patch table
        RSLockHolder ch(&this->m_processMutex);

        ClearPatchTable();
    }

    FreeDac();


    CordbProcess::CloseIPCHandles();

    CordbBase::Neuter(ticket);
        
    m_cordb.Clear();

    // Need to release this reference to ourselves. Other leaf objects may still hold
    // strong references back to this CordbProcess object.
    _ASSERTE(m_pProcess == this);
    m_pProcess.Clear();    
}

void CordbProcess::CloseIPCHandles(void)
{
    INTERNAL_API_ENTRY(this);

    // Close off Right Side's handles.
    if (m_leftSideEventAvailable != NULL)
    {
        CloseHandle(m_leftSideEventAvailable);
        m_leftSideEventAvailable = NULL;
    }

    if (m_leftSideEventRead != NULL)
    {
        CloseHandle(m_leftSideEventRead);
        m_leftSideEventRead = NULL;
    }

    if (m_rightSideEventAvailable != NULL)
    {
        CloseHandle(m_rightSideEventAvailable);
        m_rightSideEventAvailable = NULL;
    }

    if (m_rightSideEventRead != NULL)
    {
        CloseHandle(m_rightSideEventRead);
        m_rightSideEventRead = NULL;
    }

    if (m_leftSideUnmanagedWaitEvent != NULL)
    {
        CloseHandle(m_leftSideUnmanagedWaitEvent);
        m_leftSideUnmanagedWaitEvent = NULL;
    }

    if (m_syncThreadIsLockFree != NULL)
    {
        CloseHandle(m_syncThreadIsLockFree);
        m_syncThreadIsLockFree = NULL;
    }

    if (m_stopWaitEvent != NULL)
    {
        CloseHandle(m_stopWaitEvent);
        m_stopWaitEvent = NULL;
    }

    if (m_handle != NULL)
    {
        CloseHandle(m_handle);
        m_handle = NULL;
    }    
}


//-----------------------------------------------------------------------------
// Create new OS Thread for the Win32 Event Thread (the thread used in interop-debugging to sniff
// native debug events). This is 1:1 w/ a CordbProcess object.
// This will then be used to actuall create the CordbProcess object.
// The process object will then take ownership of the thread.
//
// pCordb - the root object that the process lives under
// *ppw32 - outparameter for newly created thread object.
//-----------------------------------------------------------------------------
HRESULT CordbProcess::CreateAndStartWin32ET(Cordb * pCordb, CordbWin32EventThread ** ppw32)
{
    _ASSERTE(ppw32 != NULL);
    *ppw32 = NULL;

    //
    // Create the win32 event listening thread
    //
    CordbWin32EventThread * pW32 = new (nothrow) CordbWin32EventThread(pCordb);

    HRESULT hr = S_OK;

    if (pW32 != NULL)
    {
        hr = pW32->Init();

        if (SUCCEEDED(hr))
        {
            hr = pW32->Start();
        }

        if (FAILED(hr))
        {
            delete pW32;
            pW32 = NULL;
        }
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    *ppw32 = pW32;
    return ErrWrapper(hr);
}


// Load & Init DAC
// This needs to succeed, and should always succeed (baring a bad installation)
// so we assert on failure paths.
HRESULT CordbProcess::InitializeDac()
{

    return S_OK;
}

void CordbProcess::FreeDac()
{
}

//
// Init -- create any objects that the process object needs to operate.
// Currently, this is just a few events.
//
HRESULT CordbProcess::Init(bool win32Attached)
{
    INTERNAL_API_ENTRY(this);
    // This is being done on the win32 event thread.

    HRESULT hr = S_OK;
    BOOL succ = TRUE;
    WCHAR tmpName[256];

    _ASSERTE(!win32Attached);
    FAIL_IF_NEUTERED(this);


    // Add CreateProcess event to the queue
    m_queuedEventList = (DebuggerIPCEvent*) new (nothrow) BYTE[sizeof(DebuggerIPCEvent)];

    if (m_queuedEventList == NULL)
    {
        return E_OUTOFMEMORY;
    }

    ZeroMemory(m_queuedEventList, sizeof(DebuggerIPCEvent));
    m_queuedEventList->next = NULL;
    m_queuedEventList->type = DB_IPCE_CREATE_PROCESS;
    m_queuedEventList->processId = (DWORD) this->m_id;
    m_lastQueuedEvent = m_queuedEventList;


    IPCWriterInterface *pIPCManagerInterface = new (nothrow) IPCWriterInterface();

    if (pIPCManagerInterface == NULL)
        return (E_OUTOFMEMORY);

    hr = pIPCManagerInterface->Init();

    if (FAILED(hr))
        return (hr);

    m_processMutex.Init("Process Lock", RSLock::cLockReentrant, RSLock::LL_PROCESS_LOCK);
    m_StopGoLock.Init("Stop-Go Lock", RSLock::cLockReentrant, RSLock::LL_STOP_GO_LOCK);


    //
    // Setup events needed by the left side to send the right side an event.
    //
    // Grab the security attributes that we'll use to create kernel objects for the target process.
    SECURITY_ATTRIBUTES *pSA = NULL;


    GetPidDecoratedName(tmpName, NumItems(tmpName), CorDBIPCLSEventAvailName, GetPid());



    m_leftSideEventAvailable = WszCreateEvent(pSA, FALSE, FALSE, tmpName);

    if (m_leftSideEventAvailable == NULL)
    {
        hr = HRESULT_FROM_GetLastError();

        goto exit;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        hr = CORDBG_E_DEBUGGER_ALREADY_ATTACHED;
        goto exit;
    }

    GetPidDecoratedName(tmpName, NumItems(tmpName), CorDBIPCLSEventReadName, GetPid());



    m_leftSideEventRead = WszCreateEvent(pSA, FALSE, FALSE, tmpName);

    if (m_leftSideEventRead == NULL)
    {
        hr = HRESULT_FROM_GetLastError();
        goto exit;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        // Security Fix: it is important to fail out here so that we won't
        // be using a precreated event to endanger our managed app.
        //
        hr = CORDBG_E_DEBUGGER_ALREADY_ATTACHED;
        goto exit;
    }

    m_stopWaitEvent = WszCreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_stopWaitEvent == NULL)
    {
        hr = HRESULT_FROM_GetLastError();
        goto exit;
    }


    //
    // Duplicate our own copy of the process's handle because the handle
    // we've got right now is also passed back to the caller of CreateProcess
    // in the process info structure, and they're supposed to be able to
    // close that handle whenever they want to.

    HANDLE tmpHandle;

    succ = DuplicateHandle(GetCurrentProcess(),
                           m_handle,
                           GetCurrentProcess(),
                           &tmpHandle,
                           NULL,
                           FALSE,
                           DUPLICATE_SAME_ACCESS);

    if (!succ)
    {
        hr = HRESULT_FROM_GetLastError();
        goto exit;
    }
    m_handle = tmpHandle;

    GetPidDecoratedName(tmpName, NumItems(tmpName), CorDBIPCSetupSyncEventName, GetPid());


    LOG((LF_CORDB, LL_INFO10000,
         "CP::I: creating setup sync event with name [%S]\n", tmpName));

    m_SetupSyncEvent = WszCreateEvent(pSA, TRUE, FALSE, tmpName);

    if (m_SetupSyncEvent == NULL)
    {
        hr = HRESULT_FROM_GetLastError();
        goto exit;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {

        
        // If the event already exists, then the Left Side has already
        // setup the shared memory.
        LOG((LF_CORDB, LL_INFO10000, "CP::I: setup sync event already exists.\n"));

        // Wait for the Setup Sync event before continuing. This
        // ensures that the Left Side is finished setting up the
        // control block.
        // We can never 100% guarantee this event is from a well-behaved debuggee, so 
        // we wait w/ a timeout just in case.        
        DWORD dwTimeoutMS = 20 * 1000;
        DWORD ret = WaitForSingleObject(m_SetupSyncEvent, dwTimeoutMS);

        if (ret != WAIT_OBJECT_0)
        {
            hr = HRESULT_FROM_GetLastError();
            goto exit;
        }

        // We no longer need this event now.
        CloseHandle(m_SetupSyncEvent);
        m_SetupSyncEvent = NULL;

        hr = m_IPCReader.OpenPrivateBlockOnPid((DWORD)m_id);
        // If we're trying to debug across platforms, opening the IPC block would have
        // failed with hr= CORDBG_E_UNCOMPATIBLE_PLATFORMS

        if (!SUCCEEDED(hr))
        {
            goto exit;
        }

        m_DCB = m_IPCReader.GetDebugBlock();

        if (m_DCB == NULL)
        {
            hr = ERROR_FILE_NOT_FOUND;
            goto exit;
        }

        // Verify that the control block is valid.
        hr = VerifyControlBlock();

        if (FAILED(hr))
            goto exit;

        // Dup LSEA and LSER into the remote process.
        succ = m_DCB->m_leftSideEventAvailable.DuplicateToRemoteProcess(m_handle, m_leftSideEventAvailable);
        if (!succ)
        {
            hr = HRESULT_FROM_GetLastError();
            goto exit;
        }

        succ = m_DCB->m_leftSideEventRead.DuplicateToRemoteProcess(m_handle, m_leftSideEventRead);
        if (!succ)
        {
            hr = HRESULT_FROM_GetLastError();
            goto exit;
        }

        // Dup our own process handle into the remote process.
        succ = m_DCB->m_rightSideProcessHandle.DuplicateToRemoteProcess(m_handle, GetCurrentProcess());
        if (!succ)
        {
            hr = HRESULT_FROM_GetLastError();
            goto exit;
        }

        // Dup RSEA and RSER into this process.
        succ = m_DCB->m_rightSideEventAvailable.DuplicateToLocalProcess(m_handle, &m_rightSideEventAvailable);
        if (!succ)
        {
            hr = HRESULT_FROM_GetLastError();
            goto exit;
        }

        succ = m_DCB->m_rightSideEventRead.DuplicateToLocalProcess(m_handle, &m_rightSideEventRead);
        if (!succ)
        {
            hr = HRESULT_FROM_GetLastError();
            goto exit;
        }

        succ = m_DCB->m_leftSideUnmanagedWaitEvent.DuplicateToLocalProcess(m_handle, &m_leftSideUnmanagedWaitEvent);
        if (!succ)
        {
            hr = HRESULT_FROM_GetLastError();
            goto exit;
        }

        succ = m_DCB->m_syncThreadIsLockFree.DuplicateToLocalProcess(m_handle, &m_syncThreadIsLockFree);
        if (!succ)
        {
            hr = HRESULT_FROM_GetLastError();
            goto exit;
        }

        m_sendAttachIPCEvent = true;

        m_DCB->m_rightSideIsWin32Debugger = win32Attached;

        // At this point, the control block is complete and all four
        // events are available and valid for the remote process.
    }
    else
    {
        // If the event was created by us, then we need to signal
        // its state. The fields in the shared mem which need to be
        // filled out by us will be done upon receipt of the first
        // event from the LHS
        LOG((LF_CORDB, LL_INFO10000, "DRCT::I: setup sync event was created.\n"));

        // Set the Setup Sync event
        SetEvent(m_SetupSyncEvent);
    }

exit:

    if (pIPCManagerInterface != NULL)
    {
        pIPCManagerInterface->Terminate();
        delete pIPCManagerInterface;
    }

    if (FAILED(hr))
    {
        CleanupHalfBakedLeftSide();
    }

    return hr;
}


COM_METHOD CordbProcess::CanCommitChanges(ULONG cSnapshots,
                ICorDebugEditAndContinueSnapshot *pSnapshots[],
                ICorDebugErrorInfoEnum **pError)
{
    return E_NOTIMPL;
}

COM_METHOD CordbProcess::CommitChanges(ULONG cSnapshots,
    ICorDebugEditAndContinueSnapshot *pSnapshots[],
    ICorDebugErrorInfoEnum **pError)
{
    return E_NOTIMPL;
}


//
// Terminating -- places the process into the terminated state. This should
// also get any blocking process functions unblocked so they'll return
// a failure code.
//
void CordbProcess::Terminating(BOOL fDetach)
{
    INTERNAL_API_ENTRY(this);

    LOG((LF_CORDB, LL_INFO1000,"CP::T: Terminating process 0x%x detach=%d\n", m_id, fDetach));
    m_terminated = true;

    m_cordb->ProcessStateChanged();

    // Set events that may be blocking stuff.
    // But don't set RSER unless we actually read the event. We don't block on RSER
    // since that wait also checks the leftside's process handle.
    SetEvent(m_leftSideEventRead);
    SetEvent(m_leftSideEventAvailable);
    SetEvent(m_stopWaitEvent);

    if (fDetach)
    {
        // This averts a race condition wherein we'll detach, then reattach,
        // and find these events in the still-signalled state.
        ResetEvent(m_rightSideEventAvailable);
        ResetEvent(m_rightSideEventRead);
    }
}



//
// Send a managed attach. This is asynchronous and will return immediately.
//
HRESULT CordbProcess::QueueManagedAttach()
{
    INTERNAL_API_ENTRY(this);

    _ASSERTE(ThreadHoldsProcessLock());

    // We don't know what Queue it.
    SendAttachProcessWorkItem * pItem = new SendAttachProcessWorkItem(this);
    if (pItem == NULL)
    {
        return E_OUTOFMEMORY;
    }
    this->m_cordb->m_rcEventThread->QueueAsyncWorkItem(pItem);
    return S_OK;
}

void SendAttachProcessWorkItem::Do()
{
    HRESULT hr;

    // This is being processed on the RCET, where it's safe to take the Stop-Go lock.
    RSLockHolder ch(this->GetProcess()->GetStopGoLock());

    DebuggerIPCEvent *event = (DebuggerIPCEvent*) _alloca(CorDBIPC_BUFFER_SIZE);
    GetProcess()->InitAsyncIPCEvent(event, DB_IPCE_ATTACHING, LSPTR_APPDOMAIN::NullPtr());

    LOG((LF_CORDB, LL_INFO1000, "[%x] CP::S: sending attach.\n", GetCurrentThreadId()));

    hr = GetProcess()->SendIPCEvent(event, CorDBIPC_BUFFER_SIZE);

    LOG((LF_CORDB, LL_INFO1000, "[%x] CP::S: sent attach.\n", GetCurrentThreadId()));
}


//
// HandleManagedCreateThread processes a managed create thread debug event.
//
void CordbProcess::HandleManagedCreateThread(DWORD dwThreadId,
                                             HANDLE hThread)
{
    INTERNAL_API_ENTRY(this);

    LOG((LF_CORDB, LL_INFO10000, "[%x] CP::HMCT: Create Thread %#x\n",
         GetCurrentThreadId(),
         dwThreadId));

    Lock();

    CordbThread* t = new (nothrow) CordbThread(this, dwThreadId, hThread);

    if (t != NULL)
    {
        HRESULT hr = m_userThreads.AddBase(t);

        if (FAILED(hr))
        {
            delete t;

            LOG((LF_CORDB, LL_INFO10000,
                 "Failed adding thread to process!\n"));
            CORDBSetUnrecoverableError(this, hr, 0);
        }
    }
    else
    {
        LOG((LF_CORDB, LL_INFO10000, "New CordbThread failed!\n"));
        CORDBSetUnrecoverableError(this, E_OUTOFMEMORY, 0);
    }

    Unlock();
}


HRESULT CordbProcess::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugProcess)
    {
        *pInterface = static_cast<ICorDebugProcess*>(this);
    }
    else if (id == IID_ICorDebugController)
    {
        *pInterface = static_cast<ICorDebugController*>(static_cast<ICorDebugProcess*>(this));
    }
    else if ((id == IID_ICorDebugProcess2) &&
             (this->SupportsVersion(ver_ICorDebugProcess2)))

    {
        *pInterface = static_cast<ICorDebugProcess2*>(this);
    }
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown*>(static_cast<ICorDebugProcess*>(this));
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    ExternalAddRef();
    return S_OK;
}

// Detach the Debugger from the LS process.
// Once we're detached, the LS can resume running and exit.
// So it's possible to get an ExitProcess callback in the middle of the Detach phase. If that happens,
// we must return CORDBG_E_PROCESS_TERMINATED and pretend that the exit happened before we tried to detach.
// Else if we detach successfully, return S_OK.
HRESULT CordbProcess::Detach()
{
    PUBLIC_API_ENTRY(this);

    LOG((LF_CORDB, LL_INFO1000, "CP::Detach - beginning\n"));
    FAIL_IF_NEUTERED(this);

    ATT_REQUIRE_SYNCED_OR_NONINIT_MAY_FAIL(this);


    // Interop detach is not supported.
    if (m_state & PS_WIN32_ATTACHED)
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::Detach - can't detach if interop-attached\n"));
        return ErrWrapper(CORDBG_E_INTEROP_NOT_SUPPORTED);
    }

    // Since the detach may resume the LS and allow it to exit, which may invoke the EP callback
    // which may destroy this process object, be sure to protect us w/ an extra AddRef/Release
    RSSmartPtr<CordbProcess> pRef(this);

    HRESULT hr = S_OK;
    HASHFIND hf;

    if (m_SetupSyncEvent != NULL)
    {
        CloseHandle(m_SetupSyncEvent);
        m_SetupSyncEvent = NULL;
    }

    NeuterTicket ticket(this);

    // If we detach before the CLR is loaded into the debuggee, then we can no-op a lot of work.
    // We sure can't be sending IPC events to the LS before it exists.
    if (m_initialized)
    {
        //
        // Ensure the process is ready for a detach at this time.  If not, let the IDE know.
        //
        hr = IsReadyForDetach();

        if (FAILED(hr))
        {
            return ErrWrapper(hr);
        }

        // The managed event queue is not necessarily drained. Cordbg could call detach between any callback.

        // Detach from each AD before detaching from the entire process.
        // Note that we may have neutered appdomains in the list b/c we don't
        // remove appdomains until we get the exit thread callback.
        CordbAppDomain *cad = m_appDomains.FindFirst(&hf);

        while (cad != NULL)
        {
            hr = cad->DetachWorker();

            if (FAILED(hr) && (hr != CORDBG_E_OBJECT_NEUTERED))
                return hr;

            cad = m_appDomains.FindNext(&hf);
        }


        // While the process is still stopped, neuter all of our children.
        // This will make our Neuter() a nop and saves the W32ET from having to do dangerous work.
        this->NeuterChildren(ticket);

        // Go ahead and detach from the entire process now. This is like sending a "Continue".
        DebuggerIPCEvent *event = (DebuggerIPCEvent*) _alloca(CorDBIPC_BUFFER_SIZE);
        InitIPCEvent(event, DB_IPCE_DETACH_FROM_PROCESS, true, LSPTR_APPDOMAIN::NullPtr());

        hr = m_cordb->SendIPCEvent(this, event, CorDBIPC_BUFFER_SIZE);
        hr = WORST_HR(hr, event->hr);
    }
    else
    {
        // Shouldn't have any appdomains.
        _ASSERTE(m_appDomains.FindFirst(&hf) == NULL);
    }

    if (!FAILED(hr))
    {
        LOG((LF_CORDB, LL_INFO10000, "CP::Detach - got reply from LS\n"));

        // It's possible that the LS may exit after they reply to our detach_from_process, but
        // before we update our internal state that they're detached. So still have to check
        // failure codes here.
        hr = this->m_win32EventThread->SendDetachProcessEvent(this);


        // Since we're auto-continuing when we detach, we should set the stop count back to zero.
        // This (along w/ m_detached) prevents anyone from calling Continue on this process
        // after this call returns.
        m_stopCount = 0;

        if (hr != CORDBG_E_PROCESS_TERMINATED)
        {
            // Remember that we've detached from this process object. This will prevent any further operations on
            // this process, just in case... :)
            // If LS exited, then don't set this flag because it overrides m_terminated when reporting errors;
            // and we want to provide a consistent story about whether we detached or whether the LS exited.
            m_detached = true;
        }
    }

    // Now that all complicated cleanup is done, do a final neuter.
    // This will implicitly stop our Win32 event thread as well.
    this->Neuter(ticket);

    // Implicit release on pRef

    LOG((LF_CORDB, LL_INFO1000, "CP::Detach - returning w/ hr=0x%x\n", hr));
    return hr;
}

// Delete all events from the queue without dispatching. This is useful in shutdown.
// An event that is currently dispatching is not on the queue.
void CordbProcess::DeleteQueuedEvents()
{
    INTERNAL_API_ENTRY(this);

    LOG((LF_CORDB, LL_INFO1000, "CP::DeleteQueuedEvents: Begin deleting queued events\n"));

    // We must have the process lock to ensure that no one is trying to add an event
    _ASSERTE(ThreadHoldsProcessLock());

    // Free all the remaining events
    DebuggerIPCEvent *pCur = m_queuedEventList;
    while (pCur != NULL)
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::DeleteQueuedEvents: Deleting queued event: '%s'\n", IPCENames::GetName(pCur->type)));

        DebuggerIPCEvent *pDel = pCur;
        pCur = pCur->next;
        DeleteIPCEventHelper(pDel);
    }
    m_queuedEventList = NULL;

    LOG((LF_CORDB, LL_INFO1000, "CP::DeleteQueuedEvents: Finished deleting queued events\n"));

}

void CordbProcess::StartEventDispatch(DebuggerIPCEventType event)
{
    LEAF_CONTRACT;

    _ASSERTE(m_dispatchedEvent == DB_IPCE_DEBUGGER_INVALID);
    _ASSERTE(event != DB_IPCE_DEBUGGER_INVALID);
    m_dispatchedEvent = event;
}

void CordbProcess::FinishEventDispatch()
{
    LEAF_CONTRACT;

    _ASSERTE(m_dispatchedEvent != DB_IPCE_DEBUGGER_INVALID);
    m_dispatchedEvent = DB_IPCE_DEBUGGER_INVALID;
}

bool CordbProcess::AreDispatchingEvent()
{
    LEAF_CONTRACT;

    return m_dispatchedEvent != DB_IPCE_DEBUGGER_INVALID;
}

DebuggerIPCEventType CordbProcess::GetDispatchedEvent()
{
    LEAF_CONTRACT;

    return m_dispatchedEvent;
}

//
// Helper to clean up IPCEvent before deleting it.
//
void DeleteIPCEventHelper(DebuggerIPCEvent *pDel)
{
    if (pDel == NULL)
    {
        return;
    }
    switch (pDel->type & DB_IPCE_TYPE_MASK)
    {
        // so far only this event need to cleanup.
        case DB_IPCE_UPDATE_MODULE_SYMS:
            pDel->UpdateModuleSymsData.dataBuffer.CleanUp();
            break;
        case DB_IPCE_MDA_NOTIFICATION:
            pDel->MDANotification.szName.CleanUp();
            pDel->MDANotification.szDescription.CleanUp();
            pDel->MDANotification.szXml.CleanUp();
            break;

        case DB_IPCE_FIRST_LOG_MESSAGE:
            pDel->FirstLogMessage.szContent.CleanUp();
            break;

        default:
            break;
    }
    delete [] (BYTE *)pDel;
}

// Terminate the app. We'll still dispatch an ExitProcess callback, so the app
// must wait for that before calling Cordb::Terminate.
// If this fails, the client can always call the OS's TerminateProcess command
// to rudely kill the debuggee.
HRESULT CordbProcess::Terminate(unsigned int exitCode)
{
    PUBLIC_API_ENTRY(this);

    LOG((LF_CORDB, LL_INFO1000, "CP::Terminate: with exitcode %u\n", exitCode));
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_SYNCED_OR_NONINIT_MAY_FAIL(this);


    Lock();

    // In case we're continuing from the loader bp, we don't want to try and kick off an attach. :)
    m_fDoDelayedManagedAttached = false;
    m_exiting = true;

    // Free all the remaining events
    DeleteQueuedEvents();

    RSSmartPtr<CordbProcess> pRef(this);

    Unlock();


    // Right now, we simply pass through to the Win32 terminate...
    // At any point after this call, the w32 ET may run the ExitProcess code which will race w/ the continue call.
    // This call only posts a request that the process terminate and does not guarantee the process actually
    // terminates. In particular, the process can not exit until any outstanding IO requests are done (on cancelled).
    // It also can not exit if we have an outstanding not-continued native-debug event.
    // Fortunately, the interesting work in terminate is done in ExitProcessWorkItem::Do, which can take the Stop-Go lock.
    // Since we're currently holding the stop-go lock, that means we at least get some serialization.
    TerminateProcess(m_handle, exitCode);


    ContinueInternal(FALSE);


    // Implicit release on pRef here (since it's going out of scope)...
    // After this release, this object may be destroyed. So don't use any member functions
    // (including Locks) after here.


    return S_OK;
}

// This can be called at any time, even if we're in an unrecoverable error state.
HRESULT CordbProcess::GetID(DWORD *pdwProcessId)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    OK_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pdwProcessId, DWORD *);

    *pdwProcessId = GetPid();

    return S_OK;
}

// Helper to get PID internally. We know we'll always succeed.
// This is more convient for internal callers since they can just use it as an expression
// without having to check HRESULTS.
DWORD CordbProcess::GetPid()
{
    return (DWORD) m_id;
}


HRESULT CordbProcess::GetHandle(HANDLE *phProcessHandle)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this); // Once we neuter the process, we close our OS handle to it.
    VALIDATE_POINTER_TO_OBJECT(phProcessHandle, HANDLE *);
    *phProcessHandle = m_handle;

    return S_OK;
}

HRESULT CordbProcess::IsRunning(BOOL *pbRunning)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pbRunning, BOOL*);

    *pbRunning = !GetSynchronized();

    return S_OK;
}

HRESULT CordbProcess::EnableSynchronization(BOOL bEnableSynchronization)
{
    /* !!! */
    PUBLIC_API_ENTRY(this);
    return E_NOTIMPL;
}

HRESULT CordbProcess::Stop(DWORD dwTimeout)
{
    PUBLIC_API_ENTRY(this);
    CORDBRequireProcessStateOK(this);

    HRESULT hr = StopInternal(dwTimeout, LSPTR_APPDOMAIN::NullPtr());

    return ErrWrapper(hr);
}

HRESULT CordbProcess::StopInternal(DWORD dwTimeout, LSPTR_APPDOMAIN pAppDomainToken)
{
    LOG((LF_CORDB, LL_INFO1000, "CP::S: stopping process 0x%x(%d) with timeout %d\n", m_id, m_id,  dwTimeout));

    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    CORDBFailIfOnWin32EventThread(this);

    DebuggerIPCEvent* event;
    HRESULT hr = S_OK;



    CORDBRequireProcessStateOK(this);

    STRESS_LOG2(LF_CORDB, LL_INFO1000, "CP::SI, timeout=%d, this=%p\n", dwTimeout, this);

    dwTimeout = INFINITE;


    // Stop + Continue are executed under the Stop-Go lock. This makes them atomic.
    // We'll toggle the process-lock (b/c we communicate w/ the W32et, so just the process-lock is
    // not sufficient to make this atomic).
    RSLockHolder ch(&m_StopGoLock);

    Lock();

    ASSERT_SINGLE_THREAD_ONLY(HoldsLock(&m_StopGoLock));

    // Don't need to stop if the process hasn't even executed any managed code yet.
    if (!m_initialized)
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::S: process isn't initialized yet.\n"));

        // Mark the process as synchronized so no events will be dispatched until the thing is continued.
        SetSynchronized(true);

        // Remember uninitialized stop...
        m_uninitializedStop = true;


        // Get the RC Event Thread to stop listening to the process.
        m_cordb->ProcessStateChanged();

        hr = S_OK;
        goto Exit;
    }

    if (GetSynchronized() || GetSyncCompleteRecv())
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::S: process was already synchronized. m_syncCompleteReceived=%d\n", GetSyncCompleteRecv()));

        if (GetSyncCompleteRecv())
        {
            // We must be in that window alluded to above (while the RCET is sweeping). Re-ping the RCET.
            SetSynchronized(true);
            m_cordb->ProcessStateChanged();
        }
        hr = S_OK;
        goto Exit;
    }

    LOG((LF_CORDB, LL_INFO1000, "CP::S: process not sync'd, requesting stop.\n"));

    m_stopRequested = true;

    // We don't want to dispatch any Win32 debug events while we're trying to stop.
    // Setting m_specialDeferment=true means that any debug event we get will be queued and not dispatched.
    // We do this to avoid a nested call to Continue.
    // These defered events will get dispatched when somebody calls continue (and since they're calling
    // stop now, they must call continue eventually).
    // Note that if we got a Win32 debug event between when we took the Stop-Go lock above and now,
    // that even may have been dispatched. We're ok because SSFW32Stop will hijack that event and continue it,
    // and then all future events will be queued.
    m_specialDeferment = true;
    Unlock();

    BOOL asyncBreakSent;

    // We need to ensure that the helper thread is alive.
    hr = this->StartSyncFromWin32Stop(&asyncBreakSent);
    if (FAILED(hr))
    {
        return hr;
    }


    if (asyncBreakSent)
    {
        hr = S_OK;
        Lock();

        m_stopRequested = false;

        goto Exit;
    }

    // Send the async break event to the RC.
    event = (DebuggerIPCEvent*) _alloca(CorDBIPC_BUFFER_SIZE);
    InitIPCEvent(event, DB_IPCE_ASYNC_BREAK, false, pAppDomainToken);

    LOG((LF_CORDB, LL_INFO1000, "CP::S: sending async stop to appd 0x%x.\n", LsPtrToCookie(pAppDomainToken)));

    hr = m_cordb->SendIPCEvent(this, event, CorDBIPC_BUFFER_SIZE);
    hr = WORST_HR(hr, event->hr);
    if (FAILED(hr))
    {
        // We don't hold the lock so just return immediately. Don't adjust stop-count.
        _ASSERTE(!ThreadHoldsProcessLock());
        return hr;
    }

    LOG((LF_CORDB, LL_INFO1000, "CP::S: sent async stop to appd 0x%x.\n", LsPtrToCookie(pAppDomainToken)));

    // Wait for the sync complete message to come in. Note: when the sync complete message arrives to the RCEventThread,
    // it will mark the process as synchronized and _not_ dispatch any events. Instead, it will set m_stopWaitEvent
    // which will let this function return. If the user wants to process any queued events, they will need to call
    // Continue.
    LOG((LF_CORDB, LL_INFO1000, "CP::S: waiting for event.\n"));

    DWORD ret;
    ret = SafeWaitForSingleObject(this, m_stopWaitEvent, dwTimeout);

    LOG((LF_CORDB, LL_INFO1000, "CP::S: got event, %d.\n", ret));

    if (m_terminated)
    {
        return CORDBG_E_PROCESS_TERMINATED;
    }

    if (ret == WAIT_OBJECT_0)
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::S: process stopped.\n"));

        m_stopRequested = false;
        m_cordb->ProcessStateChanged();

        hr = S_OK;
        Lock();
        goto Exit;
    }
    else if (ret == WAIT_TIMEOUT)
    {
        hr = ErrWrapper(CORDBG_E_TIMEOUT);
    }
    else
        hr = HRESULT_FROM_GetLastError();

    // We came out of the wait, but we weren't signaled because a sync complete event came in. Re-check the process and
    // remove the stop requested flag.
    Lock();
    m_stopRequested = false;

    if (GetSynchronized())
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::S: process stopped.\n"));

        m_cordb->ProcessStateChanged();

        hr = S_OK;
    }

Exit:
    _ASSERTE(ThreadHoldsProcessLock());

    // Stop queuing any Win32 Debug events. We should be synchronized now.
    m_specialDeferment = false;

    if (SUCCEEDED(hr))
    {
        IncStopCount();
    }

    LOG((LF_CORDB, LL_INFO1000, "CP::S: returning from Stop, hr=0x%08x, m_stopCount=%d.\n", hr, GetStopCount()));

    Unlock();

    return hr;
}


void CordbProcess::MarkAllThreadsDirty(void)
{
    INTERNAL_API_ENTRY(this);
    _ASSERTE(this->ThreadHoldsProcessLock());

    CordbThread* t;
    HASHFIND find;

    Lock();

    for (t =  m_userThreads.FindFirst(&find);
         t != NULL;
         t =  m_userThreads.FindNext(&find))
    {
        _ASSERTE(t != NULL);

        t->MarkStackFramesDirty();
    }

    ClearPatchTable();

    Unlock();
}

HRESULT CordbProcess::Continue(BOOL fIsOutOfBand)
{
    PUBLIC_API_ENTRY(this);

    HRESULT hr;

    if (fIsOutOfBand)
    {
        hr = E_INVALIDARG;
    }
    else
    {
        hr = ContinueInternal(fIsOutOfBand);
    }

    return hr;
}


HRESULT CordbProcess::ContinueInternal(BOOL fIsOutOfBand)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    // Continue has an ATT similar to ATT_REQUIRE_STOPPED_MAY_FAIL, but w/ some subtle differences.
    // - if we're stopped at a native DE, but not synchronized, we don't want to sync.
    // - We may get Debug events (especially native ones) at weird times, and thus we have to continue
    // at weird times.

    // External APIs should not have the process lock.
    _ASSERTE(!ThreadHoldsProcessLock());

    // OutOfBand should use ContinueOOB
    _ASSERTE(!fIsOutOfBand);

    // Since Continue is process-wide, just use a null appdomain pointer.
    LSPTR_APPDOMAIN pAppDomainToken = LSPTR_APPDOMAIN::NullPtr();

    DebuggerIPCEvent* event;
    HRESULT hr = S_OK;

    if (m_unrecoverableError)
        return CORDBHRFromProcessState(this, NULL);


    // We can't call ContinueInternal for an inband event on the win32 event thread.
    // This is a bug in the CLR (or an API design decision, depending on your perspective).
    // Continue() may send an IPC event and we can't do that on the win32 event thread.
    CORDBFailIfOnWin32EventThread(this);

    STRESS_LOG1(LF_CORDB, LL_INFO1000, "CP::CI: continuing IB,  this=0x%X\n", this);

    // Stop + Continue are executed under the Stop-Go lock. This makes them atomic.
    // We'll toggle the process-lock (b/c we communicate w/ the W32et, so that's not sufficient).
    RSLockHolder ch(&m_StopGoLock);

    // Check for other failures (do these after we have the SG lock).
    if (m_terminated)
    {
        return CORDBG_E_PROCESS_TERMINATED;
    }
    if (m_detached)
    {
        return CORDBG_E_PROCESS_DETACHED;
    }

    Lock();

    ASSERT_SINGLE_THREAD_ONLY(HoldsLock(&m_StopGoLock));
    _ASSERTE(fIsOutOfBand == FALSE);

    // If we've got multiple Stop calls, we need a Continue for each one. So, if the stop count > 1, just go ahead and
    // return without doing anything. Note: this is only for in-band or managed events. OOB events are still handled as
    // normal above.
    _ASSERTE(GetStopCount() > 0);

    if (GetStopCount() == 0)
    {
        Unlock();
        _ASSERTE(!"Superflous Continue. ICorDebugProcess.Continue() called too many times");
        return CORDBG_E_SUPERFLOUS_CONTINUE;
    }

    DecStopCount();

    // We give managed events priority over unmanaged events. That way, the entire queued managed state can drain before
    // we let any other unmanaged events through.

    // Every stop or event must be matched by a corresponding Continue. m_stopCount counts outstanding stopping events
    // along with calls to Stop. If the count is high at this point, we simply return. This ensures that even if someone
    // calls Stop just as they're receiving an event that they can call Continue for that Stop and for that event
    // without problems.
    if (GetStopCount() > 0)
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::CI: m_stopCount=%d, Continue just returning S_OK...\n", GetStopCount()));

        Unlock();
        return S_OK;
    }



    // We're no longer stopped, so reset the m_stopWaitEvent.
    ResetEvent(m_stopWaitEvent);

    // If we're continuing from an uninitialized stop, then we don't need to do much at all. No event need be sent to
    // the Left Side (duh, it isn't even there yet.) We just need to get the RC Event Thread to start listening to the
    // process again, and resume any unmanaged threads if necessary.
    if (m_uninitializedStop)
    {
        STRESS_LOG0(LF_CORDB, LL_INFO1000, "CP::CI: continuing from uninitialized stop.\n");

        // No longer synchronized (it was a half-assed sync in the
        // first place.)
        SetSynchronized(false);
        MarkAllThreadsDirty();

        // No longer in an uninitialized stop.
        m_uninitializedStop = false;

        // Notify the RC Event Thread.
        m_cordb->ProcessStateChanged();

        Unlock();



        return S_OK;
    }

    // If there are more managed events, get them dispatched now.
    if ((m_queuedEventList != NULL) && GetSynchronized())
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::CI: managed event queued.\n"));

        // Mark that we're not synchronized anymore.
        SetSynchronized(false);
        MarkAllThreadsDirty();

        // If we're in the middle of dispatching a managed event, then simply return. This indicates to HandleRCEvent
        // that the user called Continue and HandleRCEvent will dispatch the next queued event. But if Continue was
        // called outside the managed callback, all we have to do is tell the RC event thread that something about the
        // process has changed and it will dispatch the next managed event.
        if (! AreDispatchingEvent())
        {
            LOG((LF_CORDB, LL_INFO1000, "CP::CI: continuing while not dispatching managed event.\n"));

            m_cordb->ProcessStateChanged();
        }

        Unlock();
        return S_OK;
    }

    // Neuter if we have an outstanding object.
    // Only do this if we're really continuining the debuggee. So don't do this if our stop-count is high b/c we
    // shouldn't neuter until we're done w/ the current event. And don't do this until we drain the current callback queue.
    // Note that we can't hold the process lock while we do this b/c Neutering may send IPC events.
    // However, we're still under the StopGo lock b/c that may help us serialize things.

    // Sweep neuter list. This will catch anything that's marked as 'safe to neuter'. This includes
    // all objects added to the 'neuter-on-Continue'.
    // Only do this if we're synced- we don't want to do this if we're continuing from a Native Debug event.
    if (GetSynchronized())
    {
        NeuterTicket ticket(this);

        // Can't hold process lock b/c neutering may send events.
        Unlock();

        m_ContinueNeuterList.NeuterAndClear(ticket);

        //
        m_ExitNeuterList.SweepAllNeuterAtWillObjects(ticket);

        HASHFIND        find;
        CordbAppDomain * pAppDomain;

        for (pAppDomain = m_appDomains.FindFirst(&find);
             pAppDomain != NULL;
             pAppDomain = m_appDomains.FindNext(&find))
        {
            // In v2.0, clients should always attach to the appdomain. If we find any ADs that they haven't
            // attached to, then that's a horrendous API misuse. In v1.1, we'd just AV. We'll try to be
            // a little more graceful here.
            if (!pAppDomain->m_fAttached)
            {
                CONSISTENCY_CHECK_MSGF(false, 
                ("API misuse: failed to call ICorDebugAppDomain::Attach() appdomain:0x%p\n", pAppDomain));

                // Provide some level of runtime checking. We explicitly choose an UnrecoverableError 
                // over a graceful failure because our API is actually in a very inconsistent state right now.
                CORDBSetUnrecoverableError(this, CORDBG_E_APPDOMAIN_MISMATCH, 0);
            }
            pAppDomain->GetSweepableExitNeuterList()->SweepAllNeuterAtWillObjects(ticket);
        }

        Lock();
    }


    // At this point, if the managed event queue is empty, m_synchronized may still be true if we had previously
    // synchronized.


    // Both the managed and unmanaged event queues are now empty. Go
    // ahead and continue the process for real.
    LOG((LF_CORDB, LL_INFO1000, "CP::CI: headed for true continue.\n"));

    // We need to check these while under the lock, but action must be
    // taked outside of the lock.
    bool isExiting = m_exiting;
    bool wasSynchronized = GetSynchronized();

    // Mark that we're no longer synchronized.
    if (wasSynchronized)
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::CI: process was synchronized.\n"));

        SetSynchronized(false);
        SetSyncCompleteRecv(false);
        MarkAllThreadsDirty();

        // Tell the RC event thread that something about this process has changed.
        m_cordb->ProcessStateChanged();
    }

    m_continueCounter++;

    // If m_oddSync is set, then out last synchronization was due to us syncing the process because we were Win32
    // stopped. Therefore, while we do need to do most of the work to continue the process below, we don't actually have
    // to send the managed continue event. Setting wasSynchronized to false here helps us do that.
    if (m_oddSync)
    {
        wasSynchronized = false;
        m_oddSync = false;
    }



    Unlock();

    // Although we've released the Process-lock, we still have the Stop-Go lock.
    _ASSERTE(m_StopGoLock.HasLock());

    // If we're processing an ExitProcess managed event, then we don't want to really continue the process, so just fall
    // thru.  Note: we did let the unmanaged continue go through above for this case.
    if (isExiting)
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::CI: continuing from exit case.\n"));
    }
    else if (wasSynchronized)
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::CI: Sending continue to AppD:0x%x.\n", LsPtrToCookie(pAppDomainToken)));

        STRESS_LOG2(LF_CORDB, LL_INFO1000, "Continue flags:special=%d, dowin32=%d\n", m_specialDeferment, doWin32Continue);

        // Send to the RC to continue the process.
        event = (DebuggerIPCEvent*) _alloca(CorDBIPC_BUFFER_SIZE);
        InitIPCEvent(event, DB_IPCE_CONTINUE, false, pAppDomainToken);

        hr = m_cordb->SendIPCEvent(this, event, CorDBIPC_BUFFER_SIZE);

        if (hr == CORDBG_E_PROCESS_TERMINATED)
        {
            hr = S_FALSE;
        }
        _ASSERTE(SUCCEEDED(event->hr));

        LOG((LF_CORDB, LL_INFO1000, "CP::CI: Continue sent to AppD:0x%x.\n", LsPtrToCookie(pAppDomainToken)));
    }


    LOG((LF_CORDB, LL_INFO1000, "CP::CI: continue done, returning.\n"));

    return hr;
}

HRESULT CordbProcess::HasQueuedCallbacks(ICorDebugThread *pThread,
                                         BOOL *pbQueued)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pThread,ICorDebugThread *);
    VALIDATE_POINTER_TO_OBJECT(pbQueued,BOOL *);

    ATT_EVERETT_HACK_REQUIRE_STOPPED_ALLOW_NONINIT(this);

    Lock();

    if (pThread == NULL)
        *pbQueued = (m_queuedEventList != NULL);
    else
    {
        *pbQueued = FALSE;

        for (DebuggerIPCEvent *event = m_queuedEventList;
             event != NULL;
             event = event->next)
        {
            CordbThread *t =
                m_userThreads.GetBase(event->threadId);

            if (t == static_cast<CordbThread*> (pThread))
            {
                *pbQueued = TRUE;
                break;
            }
        }
    }

    Unlock();

    return S_OK;
}

//
// A small helper function to convert a CordbBreakpoint to an ICorDebugBreakpoint based on its type.
//
static ICorDebugBreakpoint *CordbBreakpointToInterface(CordbBreakpoint *bp)
{
    _ASSERTE(bp != NULL);

    //
    switch(bp->GetBPType())
    {
    case CBT_FUNCTION:
        return ((ICorDebugFunctionBreakpoint*)(CordbFunctionBreakpoint*)bp);
        break;

    case CBT_MODULE:
        return ((ICorDebugModuleBreakpoint*)(CordbModuleBreakpoint*)bp);
        break;

    case CBT_VALUE:
        return ((ICorDebugValueBreakpoint*)(CordbValueBreakpoint*)bp);
        break;

    default:
        _ASSERTE(!"Invalid breakpoint type!");
    }

    return NULL;
}


//
// DispatchRCEvent -- dispatches a previously queued IPC event received
// from the runtime controller. This represents the last amount of processing
// the DI gets to do on an event before giving it to the user.
//
void CordbProcess::DispatchRCEvent(void)
{
    INTERNAL_API_ENTRY(this);

    //
    // Note: the current thread should have the process locked when it
    // enters this method.
    //
    _ASSERTE(ThreadHoldsProcessLock());

    // Create/Launch paths already ensured that we had a callback.
    _ASSERTE(m_cordb != NULL);
    _ASSERTE(m_cordb->m_managedCallback != NULL);
    _ASSERTE(m_cordb->m_managedCallback2 != NULL);

    //
    // Snag the first event off the queue.
    //
    DebuggerIPCEvent* event = m_queuedEventList;

    _ASSERTE(event != NULL);

    #ifdef _DEBUG
    // On a debug build, keep track of the last IPC event we dispatched.
    m_pDBGLastIPCEventType = event->type;
    #endif

    m_queuedEventList = event->next;

    if (m_queuedEventList == NULL)
        m_lastQueuedEvent = NULL;

    // Bump up the stop count. Either we'll dispatch a managed event,
    // or the logic below will decide not to dispatch one and call
    // Continue itself. Either way, the stop count needs to go up by
    // one...
    _ASSERTE(this->GetSyncCompleteRecv());
    SetSynchronized(true);
    IncStopCount();

    //
    // Call StartEventDispatch to true to guard against calls to Continue()
    // from within the user's callback. We need Continue() to behave a little
    // bit differently in such a case.
    //
    // Also note that Win32EventThread::ExitProcess will take the lock and free all
    // events in the queue. (the current event is already off the queue, so
    // it will be ok). But we can't do the EP callback in the middle of this dispatch
    // so if this flag is set, EP will wait on the miscWaitEvent (which will
    // get set in FlushQueuedEvents when we return from here) and let us finish here.
    //
    StartEventDispatch(event->type);

    // Keep strong references to these objects in case a callback deletes them from underneath us.
    RSSmartPtr<CordbAppDomain> pAppDomain;
    CordbThread* thread;

    // The thread may have moved the appdomain it occupies since the last time we saw it, so update it.
    thread = m_userThreads.GetBase(event->threadId);
    pAppDomain.Assign(m_appDomains.GetBase(LsPtrToCookie(event->appDomainToken)));

    //
    // Update the app domain that this thread lives in.
    //

    // Sanity-check when we expect the appDomain to be empty / unknown
    //   - If this event isn't tied to a thread, then it's also not tied to an AppDomain
    //   - If we're creating a new AppDomain, then the supplied token won't be in our table
    _ASSERTE( pAppDomain != NULL ||
        thread == NULL  || 
        (event->type&DB_IPCE_TYPE_MASK) == DB_IPCE_CREATE_APP_DOMAIN );
    
    if ((thread != NULL) && (pAppDomain != NULL))
    {
        // It shouldn't be possible for us to see an exited AppDomain here
        _ASSERTE( !pAppDomain->IsNeutered() );
        
         thread->m_pAppDomain = pAppDomain;
    }

    Unlock();


    // We want to stay synced until after the callbacks return. This is b/c we're on the RCET,
    // and we may deadlock if we send IPC events on the RCET if we're not synced (see SendIPCEvent for details).
    // So here, stopcount=1. The StopContinueHolder bumps it up to 2.
    // - If Cordbg calls continue in the callback, that bumps it back down to 1, but doesn't actually continue.
    //   The holder dtor then bumps it down to 0, doing the real continue.
    // - If Cordbg doesn't call continue in the callback, then stopcount stays at 2, holder dtor drops it down to 1,
    //   and then the holder was just a nop.
    // This gives us delayed continues w/ no extra state flags.


    // The debugger may call Detach() immediately after it returns from the callback, but before this thread returns 
    // from this function.  Thus after we execute the callbacks, it's possible the CordbProcess object has been neutered.
    
    // Since we're already sycned, the Stop from the holder here is practically a nop that just bumps up a count.
    // Create an extra scope for the StopContinueHolder.
    {
    StopContinueHolder h;
    HRESULT hr = h.Init(this);
    if (FAILED(hr))
    {
        CORDBSetUnrecoverableError(this, hr, 0);
    }

    STRESS_LOG1(LF_CORDB, LL_EVERYTHING, "Pre-Dispatch IPC event: %s\n", IPCENames::GetName(event->type));

    switch (event->type & DB_IPCE_TYPE_MASK)
    {
    case DB_IPCE_CREATE_PROCESS:
        {
            PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
            m_cordb->m_managedCallback->CreateProcess(static_cast<ICorDebugProcess*> (this));


            // We need to emulate backwards compat. V2.0 allows debugging optimized code by default.
            // V1.1 was basically always non-optimized in the launch case.
            if (!this->SupportsVersion(CorDebugVersion_2_0))
            {
                // We don't care if this fails. It will always fail in the attach case, and that's ok.
                DWORD flags = CORDEBUG_JIT_DEFAULT | CORDEBUG_JIT_DISABLE_OPTIMIZATION;
                this->SetDesiredNGENCompilerFlags(flags);
            }
        }
        break;

    case DB_IPCE_BREAKPOINT:
        {
            RSSmartPtr<CordbBreakpoint> pRef; // need a ref while we dispatch the callback..
            Lock(); // lock while we access the Breakpoints hash.

            _ASSERTE(thread != NULL);
            _ASSERTE (pAppDomain != NULL);

            // Find the breakpoint object on this side.            
            CordbBreakpoint *bp = NULL;

            if (thread == NULL)
            {            
                // We've found cases out in the wild where we get this event on a thread we don't recognize.
                // We're not sure how this happens. Add a runtime check to protect ourselves to avoid the 
                // an AV. We still assert because this should not be happening.
                // It likely means theres some bug where we failed to send a CreateThread notification.
                STRESS_LOG1(LF_CORDB, LL_INFO1000, "Breakpoint on unrecognized thread. %p\n", bp);                

                _ASSERTE(!"Missing thread on breakpoint event");
            }   
            else
            {
                bp = pAppDomain->m_breakpoints.GetBase(
                  LsPtrToCookie(event->BreakpointData.breakpointToken));
            }


            ICorDebugBreakpoint *ibp = NULL;
            if (bp != NULL)
            {
                pRef.Assign(bp); // maintain ref across callback.
                ibp = CordbBreakpointToInterface(bp);
                _ASSERTE(ibp != NULL);
            }

            Unlock();

            if (bp != NULL)
            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE2(this, event, "thread=0x%p, bp=0x%p", thread, bp);
                m_cordb->m_managedCallback->Breakpoint(static_cast<ICorDebugAppDomain*> (pAppDomain),
                                                       static_cast<ICorDebugThread*> (thread),
                                                       ibp);
            }
            else
            {
                // If we didn't find a breakpoint object on this side,
                // the we have an extra BP event for a breakpoint that
                // has been removed and released on this side. Just
                // ignore the event.
                ContinueInternal(FALSE);
            }

            // implicit Release() on bp by pRef
        }
        break;

    case DB_IPCE_USER_BREAKPOINT:
        {
            STRESS_LOG1(LF_CORDB, LL_INFO1000, "[%x] RCET::DRCE: user breakpoint.\n",
                 GetCurrentThreadId());

            Lock();

            _ASSERTE(thread != NULL);
            _ASSERTE (pAppDomain != NULL);

            Unlock();

            _ASSERTE(thread->m_pAppDomain != NULL);

            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                m_cordb->m_managedCallback->Break(static_cast<ICorDebugAppDomain*> (thread->m_pAppDomain),
                                                  static_cast<ICorDebugThread*> (thread));
            }

        }
        break;

    case DB_IPCE_STEP_COMPLETE:
        {
            STRESS_LOG1(LF_CORDB, LL_INFO1000, "[%x] RCET::DRCE: step complete.\n",
                 GetCurrentThreadId());

            Lock();
            PREFIX_ASSUME(thread != NULL);

            CordbStepper *stepper =
              m_steppers.GetBase(LsPtrToCookie(
                                    event->StepData.stepperToken));

            // It's possible the stepper is NULL if:
            // - event X & step-complete are both in the queue
            // - during dispatch for event X, Cordbg cancels the stepper (thus removing it from m_steppers)
            // - the Step-Complete still stays in the queue, and so we're here, but out stepper's been removed.
            if (stepper != NULL)
            {
                RSSmartPtr<CordbStepper> pRef(stepper);
                stepper->m_active = false;
                m_steppers.RemoveBase((ULONG_PTR)stepper->m_id);

                Unlock();

                if (m_cordb->m_managedCallback)
                {
                    _ASSERTE(thread->m_pAppDomain != NULL);
                    PUBLIC_CALLBACK_IN_THIS_SCOPE2(this, event, "thrad=0x%p, stepper=0x%p", thread, stepper);
                    m_cordb->m_managedCallback->StepComplete(
                                                       (ICorDebugAppDomain*) thread->m_pAppDomain,
                                                       (ICorDebugThread*) thread,
                                                       (ICorDebugStepper*) stepper,
                                                       event->StepData.reason);
                }

                // implicit Release on pRef
            }
            else
            {
                Unlock();

                // Don't bother w/ a null step-complete, so just continue automatically
                ContinueInternal(false);
            }

        }
        break;

    case DB_IPCE_EXCEPTION:
        {
            STRESS_LOG1(LF_CORDB, LL_INFO1000, "[%x] RCET::DRCE: exception.\n",
                 GetCurrentThreadId());

            _ASSERTE(pAppDomain != NULL);

            if( thread == NULL )
            {
                // We've got an exception on a thread we don't know about.  This could be a thread that 
                // has never run any managed code, and due to an ICorDebug design flaw we have no
                // way of reporting this.  We'll ignore the exception, but generate a log message to give
                // the user some idea of what happened.
                _ASSERTE( !event->Exception.firstChance );  // expect only for unhandled exceptions
                {
                    // Get the log messages from the resource table.  Note that these are always in
                    // unicode, so the GetUnicode() operations below cannot fail.
                    SString logCat;
                    logCat.LoadResource( IDS_DBI_ERRLOG_CATAGORY ); 
                    SString logMsg;
                    logMsg.LoadResource( IDS_DBI_EXCEPTION_ON_UNKNOWN_THREAD_MSG ); 

                    // We don't have an ICorDebugThread, so we'll have to fudge any random one.
                    // Some managed code must have run in order to throw an exception
                    HASHFIND hf;
                    CordbThread* anyThread = m_userThreads.FindFirst(&hf);
                    _ASSERTE( anyThread != NULL );  
                    
                    PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                    m_cordb->m_managedCallback->LogMessage(
                                           static_cast<ICorDebugAppDomain*> (pAppDomain),
                                           anyThread,
                                           LPanicLevel,
                                           const_cast<WCHAR*>( logCat.GetUnicode() ),
                                           const_cast<WCHAR*>( logMsg.GetUnicode() ) );
                }
                
                break;
            }
            
            thread->SetExInfo(event->Exception.exceptionHandle);

            _ASSERTE (thread->m_pAppDomain != NULL);

            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                m_cordb->m_managedCallback->Exception((ICorDebugAppDomain*) thread->m_pAppDomain,
                                                      (ICorDebugThread*) thread,
                                                      !event->Exception.firstChance);
            }

        }
        break;

    case DB_IPCE_SYNC_COMPLETE:
        _ASSERTE(!"Should have never queued a sync complete event.");
        break;

    case DB_IPCE_THREAD_ATTACH:
        {
            STRESS_LOG2(LF_CORDB, LL_INFO100, "[%x] RCET::DRCE: thread attach : ID=%x.\n",
                 GetCurrentThreadId(), event->threadId);

            Lock();

#ifdef _DEBUG
            _ASSERTE(thread == NULL);
#endif

            HANDLE threadHandle;
            BOOL succ = TRUE;
            HRESULT hrError = S_OK;

#ifndef PLATFORM_UNIX
            if (event->ThreadAttachData.threadHandle != SWITCHOUT_HANDLE_VALUE)
            {
                  _ASSERTE(event->ThreadAttachData.threadHandle != INVALID_HANDLE_VALUE);

                // Dup the runtime thread's handle into our process.
                succ = DuplicateHandle(this->m_handle,
                                            event->ThreadAttachData.threadHandle,
                                            GetCurrentProcess(),
                                            &threadHandle,
                                            NULL, FALSE, DUPLICATE_SAME_ACCESS);

            }
            else
            {
                // Just copy the handle value
                threadHandle = event->ThreadAttachData.threadHandle;
            }
#else
            threadHandle = INVALID_HANDLE_VALUE;
#endif // !PLATFORM_UNIX

            if (succ)
            {
                HandleManagedCreateThread(event->threadId, threadHandle);

                thread =
                    m_userThreads.GetBase(event->threadId);

                _ASSERTE(thread != NULL);
                PREFIX_ASSUME(thread != NULL);

                thread->m_debuggerThreadToken =
                    event->ThreadAttachData.debuggerThreadToken;
                thread->m_firstExceptionHandler =
                    event->ThreadAttachData.firstExceptionHandler;

                _ASSERTE(thread->m_firstExceptionHandler != NULL);

                RSSmartPtr<CordbThread> pRef(thread);

                _ASSERTE (pAppDomain != NULL);

                thread->m_pAppDomain = pAppDomain;
                pAppDomain->m_fHasAtLeastOneThreadInsideIt = true;

                Unlock();

                PUBLIC_CALLBACK_IN_THIS_SCOPE1(this, event, "thread=0x%p", thread);
                m_cordb->m_managedCallback->CreateThread((ICorDebugAppDomain*) pAppDomain,
                                                         (ICorDebugThread*) thread);

                // implicit release on pRef
            }
            else
            {
                hrError = HRESULT_FROM_GetLastError(); // get err from DuplicateHandle() above.

            // If we failed b/c the LS exited, then just ignore this event
            // and make way for the ExitProcess() callback.
                if (CheckIfLSExited())
                {
                    Unlock();
                    ContinueInternal(FALSE);
                    break;
                }

                Unlock();
                CORDBSetUnrecoverableError(this, hrError, 0);
            }
        }
        break;

    case DB_IPCE_THREAD_DETACH:
        {
            STRESS_LOG2(LF_CORDB, LL_INFO100, "[%x] RCET::HRCE: thread detach : ID=%x \n",
                 GetCurrentThreadId(), event->threadId);



            // If the runtime thread never entered managed code, there
            // won't be a CordbThread, and CreateThread was never
            // called, so don't bother calling ExitThread.
            if (thread != NULL)
            {
                {
                    RSLockHolder ch(GetStopGoLock());
                    AddToNeuterOnContinueList(thread);
                }

                Lock();
                RSSmartPtr<CordbThread>    pRefThread(thread);

                _ASSERTE(pAppDomain != NULL);
                _ASSERTE(thread->m_detached);

                // Remove the thread from the hash. If we've removed it from the hash, we really should
                // neuter it ... but that causes test failures.
                // We'll neuter it in continue.
                m_userThreads.RemoveBase((ULONG_PTR)event->threadId);

                Unlock();

                LOG((LF_CORDB, LL_INFO1000, "[%x] RCET::HRCE: sending "
                     "thread detach.\n",
                     GetCurrentThreadId()));

                {
                    PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                    m_cordb->m_managedCallback->ExitThread(
                                       (ICorDebugAppDomain*) pAppDomain,
                                       (ICorDebugThread*) thread);
                }

                // Implicit release on thread & pAppDomain
            }
            else
            {
                ContinueInternal(FALSE);
            }
        }
        break;

    case DB_IPCE_LOAD_MODULE:
        {
            RSLockHolder ch(GetStopGoLock());

            LOG((LF_CORDB, LL_INFO100,
                 "RCET::HRCE: load module on thread %#x Mod:0x%08x Asm:0x%08x AD:0x%08x Metadata:0x%08x/%d IsDynamic:%d\n",
                 event->threadId,
                 LsPtrToCookie(event->LoadModuleData.debuggerModuleToken),
                 LsPtrToCookie(event->LoadModuleData.debuggerAssemblyToken),
                 LsPtrToCookie(event->appDomainToken),
                 event->LoadModuleData.pMetadataStart,
                 event->LoadModuleData.nMetadataSize,
                 event->LoadModuleData.fIsDynamic));

            _ASSERTE (pAppDomain != NULL);

            CordbModule *moduleDup = (CordbModule*) pAppDomain->LookupModule (
                            event->LoadModuleData.debuggerModuleToken);
            if (moduleDup != NULL)
            {
                LOG((LF_CORDB, LL_INFO100, "Already loaded Module - continue()ing!" ));
                ContinueInternal(FALSE);
                break;
            }
            _ASSERTE(moduleDup == NULL);

            CordbAssembly *pAssembly =
                pAppDomain->m_assemblies.GetBase (
                    LsPtrToCookie(event->LoadModuleData.debuggerAssemblyToken));

            // It is possible to get a load module event before the corresponding
            // assembly has been loaded. Therefore, just ignore the event and continue.
            // A load module event for this module will be sent by the left side
            // after it has loaded the assembly.
            if (pAssembly == NULL)
            {
                LOG((LF_CORDB, LL_INFO100, "Haven't loaded Assembly "
                    "yet - continue()ing!" ));
                ContinueInternal(FALSE);
            }
            else
            {
                hr = S_OK;
                CordbModule* module = new (nothrow) CordbModule(
                                        this,
                                        pAssembly,
                                        event->LoadModuleData.debuggerModuleToken,
                                        event->LoadModuleData.pPEBaseAddress,
                                        event->LoadModuleData.nPESize,
                                        event->LoadModuleData.fIsDynamic,
                                        event->LoadModuleData.fInMemory,
                                        event->LoadModuleData.rcName.GetString(),
                                        event->LoadModuleData.rcFullNgenName.GetString(),
                                        pAppDomain);

                if (module != NULL)
                {
                    hr = module->Init(
                        event->LoadModuleData.pMetadataStart,
                        event->LoadModuleData.nMetadataSize );

                    if (SUCCEEDED(hr))
                    {
                        hr = pAppDomain->m_modules.AddBase(module);

                        if (SUCCEEDED(hr))
                        {
                            ch.Release();

                            PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                            m_cordb->m_managedCallback->LoadModule(
                                             (ICorDebugAppDomain*) pAppDomain,
                                             (ICorDebugModule*) module);
                        }
                        else
                            CORDBSetUnrecoverableError(this, hr, 0);
                    }
                    else
                        CORDBSetUnrecoverableError(this, hr, 0);
                }
                else
                    CORDBSetUnrecoverableError(this, E_OUTOFMEMORY, 0);
            }
        }
        break;

    case DB_IPCE_CREATE_CONNECTION:
        {
            STRESS_LOG1(LF_CORDB, LL_INFO100,
                "RCET::HRCE: Connection change %d \n",
                event->CreateConnection.connectionId);

            // pass back the connection id and the connection name.
            PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
            m_cordb->m_managedCallback2->CreateConnection((ICorDebugProcess *)this,
                                                          event->CreateConnection.connectionId,
                                                          const_cast<WCHAR*> (event->CreateConnection.wzConnectionName.GetString()));
        }
        break;

    case DB_IPCE_DESTROY_CONNECTION:
        {
            STRESS_LOG1(LF_CORDB, LL_INFO100,
                 "RCET::HRCE: Connection destroyed %d \n",
                 event->ConnectionChange.connectionId);
            PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
            m_cordb->m_managedCallback2->DestroyConnection((ICorDebugProcess *)this,
                                                           event->ConnectionChange.connectionId);
        }
        break;

    case DB_IPCE_CHANGE_CONNECTION:
        {
            STRESS_LOG1(LF_CORDB, LL_INFO100,
                 "RCET::HRCE: Connection changed %d \n",
                 event->ConnectionChange.connectionId);

            PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
            m_cordb->m_managedCallback2->ChangeConnection((ICorDebugProcess *)this,
                                                          event->ConnectionChange.connectionId);
        }
        break;

    case DB_IPCE_UNLOAD_MODULE:
        {
            STRESS_LOG3(LF_CORDB, LL_INFO100, "RCET::HRCE: unload module on thread %#x Mod:0x%x AD:0x%08x\n",
                 event->threadId,
                 LsPtrToCookie(event->UnloadModuleData.debuggerModuleToken),
                 LsPtrToCookie(event->appDomainToken));

            PREFIX_ASSUME (pAppDomain != NULL);

            CordbModule *module = (CordbModule*) pAppDomain->LookupModule (
                            event->UnloadModuleData.debuggerModuleToken);
            if (module == NULL)
            {
                LOG((LF_CORDB, LL_INFO100, "Already unloaded Module - continue()ing!" ));
                ContinueInternal(FALSE);
                break;
            }
            _ASSERTE(module != NULL);

            // The appdomain we're unloading in must be the appdomain we were loaded in. Otherwise, we've got mismatched
            // module and appdomain pointers. Bugs 65943 & 81728.
            _ASSERTE(pAppDomain == module->GetAppDomain());

            // Ensure the module gets neutered once we call continue.
            {
                RSLockHolder ch(GetStopGoLock());
                AddToNeuterOnContinueList(module);
            }

            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                m_cordb->m_managedCallback->UnloadModule((ICorDebugAppDomain*) pAppDomain,
                                                         (ICorDebugModule*) module);
            }


            pAppDomain->m_modules.RemoveBase(
                         LsPtrToCookie(event->UnloadModuleData.debuggerModuleToken));
        }
        break;

    case DB_IPCE_LOAD_CLASS:
        {
            CordbClass *pClass = NULL;
            {
                RSLockHolder ch(this->GetStopGoLock());

                void * remotePtr = NULL;

                LOG((LF_CORDB, LL_INFO10000,
                     "RCET::HRCE: load class on thread %#x Tok:0x%08x Mod:0x%08x Asm:0x%08x AD:0x%08x\n",
                     event->threadId,
                     event->LoadClass.classMetadataToken,
                     LsPtrToCookie(event->LoadClass.classDebuggerModuleToken),
                     LsPtrToCookie(event->LoadClass.classDebuggerAssemblyToken),
                     LsPtrToCookie(event->appDomainToken)));

                _ASSERTE (pAppDomain != NULL);

                CordbModule* module =
                    (CordbModule*) pAppDomain->LookupModule(event->LoadClass.classDebuggerModuleToken);
                if (module == NULL)
                {
                    LOG((LF_CORDB, LL_INFO100, "Load Class on not-loaded Module - continue()ing!" ));
                    ContinueInternal(FALSE);
                    break;
                }
                _ASSERTE(module != NULL);

                BOOL dynamic = module->IsDynamic();

                // If this is a class load in a dynamic module, then we'll have
                // to grab an up-to-date copy of the metadata from the left side,
                // then send the "release buffer" message to free the memory.
                if (dynamic && !FAILED(hr))
                {
                    // Get it
                    remotePtr = event->LoadClass.pNewMetaData;
                    if (remotePtr != NULL)
                    {
                        hr = module->UpdateMetadataFromRemote(
                            remotePtr,
                            event->LoadClass.cbNewMetaData );

                        if( FAILED(hr) )
                        {
                            LOG((LF_CORDB, LL_INFO1000, "RCET::HRCE: Failed to copy MD! hr=%x\n", hr));
                            ContinueInternal(FALSE);
                            break; // out of the switch
                        }
                    }

                }

                hr = module->LookupOrCreateClass(event->LoadClass.classMetadataToken,&pClass);

                if (!SUCCEEDED(hr))
                    pClass = NULL;

                if (pClass->m_loadEventSent)
                {
                    // Dynamic modules are dynamic at the module level -
                    // you can't add a new version of a class once the module
                    // is baked.
                    // EnC adds completely new classes.
                    // There shouldn't be any other way to send multiple
                    // ClassLoad events.
                    // Except that there are race conditions between loading
                    // an appdomain, and loading a class, so if we get the extra
                    // class load, we should ignore it.

                    ContinueInternal(FALSE);
                    break; //out of the switch statement
                }

                pClass->m_loadEventSent = TRUE;

                if (dynamic && remotePtr != NULL)
                {
                    // Free it on the left side
                    // Now free the left-side memory
                    DebuggerIPCEvent eventReleaseBuffer;

                    InitIPCEvent(&eventReleaseBuffer,
                                 DB_IPCE_RELEASE_BUFFER,
                                 true,
                                 LSPTR_APPDOMAIN::NullPtr());

                    // Indicate the buffer to release
                    eventReleaseBuffer.ReleaseBuffer.pBuffer = remotePtr;

                    // Make the request, which is synchronous
                    hr = SendIPCEvent(&eventReleaseBuffer, sizeof(eventReleaseBuffer));
#ifdef _DEBUG
                    if (FAILED(hr))
                    {
                        LOG((LF_CORDB, LL_INFO1000, "RCET::HRCE: Failed to send msg!\n"));
                    }
#endif
                }

            } // implicit release StopGo Lock.

            if (pClass != NULL)
            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                m_cordb->m_managedCallback->LoadClass((ICorDebugAppDomain*) pAppDomain,
                                                      (ICorDebugClass*) pClass);
            }
        }
        break;

    case DB_IPCE_UNLOAD_CLASS:
        {
            LOG((LF_CORDB, LL_INFO10000,
                 "RCET::HRCE: unload class on thread %#x Tok:0x%08x Mod:0x%08x AD:0x%08x\n",
                 event->threadId,
                 event->UnloadClass.classMetadataToken,
                 LsPtrToCookie(event->UnloadClass.classDebuggerModuleToken),
                 LsPtrToCookie(event->appDomainToken)));

            // get the appdomain object
            _ASSERTE (pAppDomain != NULL);

            CordbModule *module = (CordbModule*) pAppDomain->LookupModule (
                            event->UnloadClass.classDebuggerModuleToken);
            if (module == NULL)
            {
                LOG((LF_CORDB, LL_INFO100, "Unload Class on not-loaded Module - continue()ing!" ));
                ContinueInternal(FALSE);
                break;
            }
            _ASSERTE(module != NULL);

            CordbClass *pClass = module->LookupClass(event->UnloadClass.classMetadataToken);

            if (pClass != NULL && !pClass->m_hasBeenUnloaded)
            {
                pClass->m_hasBeenUnloaded = true;

                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                m_cordb->m_managedCallback->UnloadClass((ICorDebugAppDomain*) pAppDomain,
                                                        (ICorDebugClass*) pClass);

            }
            else
            {
                LOG((LF_CORDB, LL_INFO10000,
                     "Unload already unloaded class 0x%08x.\n",
                     event->UnloadClass.classMetadataToken));

                ContinueInternal(FALSE);
            }
        }
        break;

    case DB_IPCE_FIRST_LOG_MESSAGE:
        {
            _ASSERTE(thread != NULL);
            _ASSERTE(pAppDomain != NULL);

            const WCHAR * pszContent = event->FirstLogMessage.szContent.GetString();
            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                m_cordb->m_managedCallback->LogMessage(
                                           static_cast<ICorDebugAppDomain*> (pAppDomain),
                                           static_cast<ICorDebugThread*> (thread),
                                           event->FirstLogMessage.iLevel,
                                           const_cast<WCHAR*> (event->FirstLogMessage.szCategory.GetString()),
                                           const_cast<WCHAR*> (pszContent));
            }
        }
        break;

    case DB_IPCE_LOGSWITCH_SET_MESSAGE:
        {

            LOG((LF_CORDB, LL_INFO10000,
                "[%x] RCET::DRCE: Log Switch Setting Message.\n",
                 GetCurrentThreadId()));

            Lock();

            _ASSERTE(thread != NULL);
            Unlock();

            const WCHAR *pstrLogSwitchName = event->LogSwitchSettingMessage.szSwitchName.GetString();
            const WCHAR *pstrParentName = event->LogSwitchSettingMessage.szParentSwitchName.GetString();

            // from the thread object get the appdomain object
            _ASSERTE(pAppDomain == thread->m_pAppDomain);
            _ASSERTE (pAppDomain != NULL);

            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                m_cordb->m_managedCallback->LogSwitch(static_cast<ICorDebugAppDomain*> (pAppDomain),
                                                      static_cast<ICorDebugThread*> (thread),
                                                      event->LogSwitchSettingMessage.iLevel,
                                                      event->LogSwitchSettingMessage.iReason,
                                                      const_cast<WCHAR*> (pstrLogSwitchName),
                                                      const_cast<WCHAR*> (pstrParentName));

            }
        }

        break;

    case DB_IPCE_CREATE_APP_DOMAIN:
        {
            LOG((LF_CORDB, LL_INFO100,
                 "RCET::HRCE: create appdomain on thread %#x AD:0x%08x \n",
                 event->threadId,
                 LsPtrToCookie(event->appDomainToken)));

            // We must not have seen this AppDomain already.  We remove AppDomains from the
            // hash as soon as they are exited.
            _ASSERTE( !m_appDomains.GetBase( LsPtrToCookie(event->appDomainToken)) );

            pAppDomain.Assign(new (nothrow) CordbAppDomain(
                                    this,
                                    event->appDomainToken,
                                    event->AppDomainData.id
                                    ));



            if (pAppDomain != NULL)
            {
                if( event->AppDomainData.fIsDefaultDomain )
                {
                    _ASSERTE( m_pDefaultAppDomain == NULL );
                    m_pDefaultAppDomain = pAppDomain;
                }
                
                // The CordbAppDomain ctor will have AddRef this CordbProcess object.
                hr = m_appDomains.AddBase(pAppDomain);

                if (SUCCEEDED(hr))
                {
                    PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);

                    hr = m_cordb->m_managedCallback->CreateAppDomain(static_cast<ICorDebugProcess*> (this),
                                                                     static_cast<ICorDebugAppDomain*> (pAppDomain));

                    // If they don't implement this callback, then just attach and continue.
                    if (hr == E_NOTIMPL)
                    {
                        pAppDomain->Attach();
                        ContinueInternal(FALSE);
                    }

                }
                else
                    CORDBSetUnrecoverableError(this, hr, 0);
            }
            else
                CORDBSetUnrecoverableError(this, E_OUTOFMEMORY, 0);
        }


        break;

    case DB_IPCE_EXIT_APP_DOMAIN:
        {
            LOG((LF_CORDB, LL_INFO100, "RCET::HRCE: exit appdomain on thread %#x AD:0x%08x \n",
                 event->threadId,
                 LsPtrToCookie(event->appDomainToken)));

            // If we get an ExitAD message for which we have no AppDomain, then ignore it.
            // This can happen if an AD gets torn down very early (before the LS AD is to the
            // point that it can be published).
            // This case is effectively a bug in the LS.
            if (pAppDomain == NULL)
            {
                ContinueInternal(FALSE);
                break;
            }
            _ASSERTE (pAppDomain != NULL);

            // The default AppDomain is never exited
            _ASSERTE (pAppDomain != m_pDefaultAppDomain );
            
            // Update any threads which were last seen in this AppDomain.  We don't 
            // get any notification when a thread leaves an AppDomain, so our idea
            // of what AppDomain the thread is in may be out of date.
            UpdateThreadsForAdUnload( pAppDomain );
            
            // This will still maintain weak references so we could call Continue.
            {
                RSLockHolder ch(GetStopGoLock());
                AddToNeuterOnContinueList(pAppDomain);
            }

            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                hr = m_cordb->m_managedCallback->ExitAppDomain((ICorDebugProcess*) this,
                                                                       (ICorDebugAppDomain*) pAppDomain);
            }

            // Even after ExitAppDomain, the outside world will want to continue calling
            // Continue (and thus they may need to call CordbAppDomain::GetProcess(), which Neutering
            // would clear). Thus we can't neuter yet.

            // Just continue if they didn't implement the callback.
            if (hr == E_NOTIMPL)
            {
                ContinueInternal(FALSE);
            }

            // Remove this app domain. This means any attempt to lookup the AppDomain
            // will fail (which we do at the top of this method).  Since any threads (incorrectly) referring
            // to this AppDomain have been moved to the default AppDomain, no one should be 
            // interested in looking this AppDomain up anymore.
            m_appDomains.RemoveBase(LsPtrToCookie(event->appDomainToken));

        }

        break;

    case DB_IPCE_LOAD_ASSEMBLY:
        {
            hr = S_OK;

            LOG((LF_CORDB, LL_INFO100,
                 "RCET::HRCE: load assembly on thread %#x Asm:0x%08x AD:0x%08x \n",
                 event->threadId,
                 LsPtrToCookie(event->AssemblyData.debuggerAssemblyToken),
                 LsPtrToCookie(event->appDomainToken)));

            _ASSERTE (pAppDomain != NULL);

            // If the debugger detached from, then reattached to an AppDomain,
            //  this side may get LoadAssembly messages for previously loaded
            //  Assemblies.

            // Determine if this Assembly is cached.
            CordbAssembly* assembly =
                 pAppDomain->m_assemblies.GetBase(
                         LsPtrToCookie(event->AssemblyData.debuggerAssemblyToken));

            if (assembly != NULL)
            {
                // We may receive multiple LOAD_ASSEMBLY events in the case of domain neutral assemblies
                // (since the EE doesn't quite produce them in a reliable way.)  So if we see
                // a duplicate here, just ignore it.

                // If the Assembly is cached, assert that the properties are unchanged.
                _ASSERTE(wcscmp(assembly->GetName(), event->AssemblyData.rcName.GetString()) == 0);
                _ASSERTE(assembly->IsSystemAssembly() == event->AssemblyData.fIsSystemAssembly);

                ContinueInternal(FALSE);
            }
            else
            {
                //currently, event->AssemblyData.fIsSystemAssembly is never true
                assembly = new (nothrow) CordbAssembly(
                                pAppDomain,
                                event->AssemblyData.debuggerAssemblyToken,
                                event->AssemblyData.rcName.GetString(),
                                event->AssemblyData.fIsSystemAssembly);

                if (assembly != NULL)
                {
                    hr = pAppDomain->m_assemblies.AddBase(assembly);
                }
                else
                {
                    hr = E_OUTOFMEMORY;
                }

                // If created, or have, an Assembly, notify callback.
                if (SUCCEEDED(hr))
                {
                    {
                        PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                        hr = m_cordb->m_managedCallback->LoadAssembly((ICorDebugAppDomain*) pAppDomain,
                                                                      (ICorDebugAssembly*) assembly);
                    }

                    // Just continue if they didn't implement the callback.
                    if (hr == E_NOTIMPL)
                    {
                        ContinueInternal(FALSE);
                    }
                }
                else
                    CORDBSetUnrecoverableError(this, hr, 0);
            }
        }

        break;

    case DB_IPCE_UNLOAD_ASSEMBLY:
        {
            LOG((LF_CORDB, LL_INFO100, "RCET::DRCE: unload assembly on thread %#x Asm:0x%x AD:0x%x\n",
                 event->threadId,
                 LsPtrToCookie(event->AssemblyData.debuggerAssemblyToken),
                 LsPtrToCookie(event->appDomainToken)));

            _ASSERTE (pAppDomain != NULL);

            CordbAssembly* assembly =
                pAppDomain->m_assemblies.GetBase(
                         LsPtrToCookie(event->AssemblyData.debuggerAssemblyToken));
            if (assembly == NULL)
            {
                LOG((LF_CORDB, LL_INFO100, "Assembly not loaded - continue()ing!" ));
                ContinueInternal(FALSE);
                break;
            }
            _ASSERTE(assembly != NULL);

            // Ensure the assembly gets neutered when we call continue.
            {
                RSLockHolder ch(GetStopGoLock());
                AddToNeuterOnContinueList(assembly);
            }

            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                hr = m_cordb->m_managedCallback->UnloadAssembly((ICorDebugAppDomain*) pAppDomain,
                                                                        (ICorDebugAssembly*) assembly);
            }

            // Just continue if they didn't implement this callback.
            if (hr == E_NOTIMPL)
            {
                ContinueInternal(FALSE);
            }

            pAppDomain->m_assemblies.RemoveBase(
                         LsPtrToCookie(event->AssemblyData.debuggerAssemblyToken));
        }

        break;

    case DB_IPCE_FUNC_EVAL_COMPLETE:
        {
            LOG((LF_CORDB, LL_INFO1000, "RCET::DRCE: func eval complete.\n"));

            CordbEval *pEval = NULL;
            {
                RSLockHolder ch(this->GetStopGoLock());
                pEval = event->FuncEvalComplete.funcEvalKey.UnWrapAndRemove(this);
                if (pEval  == NULL)
                {
                    _ASSERTE(!"Bogus FuncEval handle in IPC block.");
                    // Bogus handle in IPC block.
                    ContinueInternal(FALSE);
                    break;
                }
            }
            _ASSERTE(pEval != NULL);

            Lock();

            _ASSERTE(thread != NULL);
            _ASSERTE(pAppDomain != NULL);

            CONSISTENCY_CHECK_MSGF(pEval->m_DbgAppDomainStarted == pAppDomain,
                ("AppDomain changed from Func-Eval. Eval=%p, Started=%p, Now=%p\n",
                pEval, pEval->m_DbgAppDomainStarted, (void*) pAppDomain));

            // Hold the data about the result in the CordbEval for later.
            pEval->m_complete = true;
            pEval->m_successful = event->FuncEvalComplete.successful;
            pEval->m_aborted = event->FuncEvalComplete.aborted;
            pEval->m_resultAddr = event->FuncEvalComplete.resultAddr;
            pEval->m_objectHandle = event->FuncEvalComplete.objectHandle;
            pEval->m_resultType = event->FuncEvalComplete.resultType;
            pEval->m_resultAppDomainToken = event->FuncEvalComplete.resultAppDomainToken;

            CordbAppDomain *pResultAppDomain = m_appDomains.GetBase(LsPtrToCookie(event->FuncEvalComplete.resultAppDomainToken));

            _ASSERTE(OutstandingEvalCount() > 0);
            DecrementOutstandingEvalCount();

            CONSISTENCY_CHECK_MSGF(pEval->m_DbgAppDomainStarted == pAppDomain,
                ("AppDomain changed from Func-Eval. Eval=%p, Started=%p, Now=%p\n",
                pEval, pEval->m_DbgAppDomainStarted, (void*) pAppDomain));

            // If we did this func eval with this thread stopped at an excpetion, then we need to pretend as if we
            // really didn't continue from the exception, since, of course, we really didn't on the Left Side.
            if (pEval->IsEvalDuringException())
            {
                thread->SetExInfo(pEval->m_ThreadOldExceptionHandle);
            }

            Unlock();

            bool evalCompleted = pEval->m_successful || pEval->m_aborted;

            // If a CallFunction() is aborted, the LHS may not complete the abort
            // immediately and hence we cant do a SendCleanup() at that point. Also,
            // the debugger may (incorrectly) release the CordbEval before this
            // DB_IPCE_FUNC_EVAL_COMPLETE event is received. Hence, we maintain an
            // extra ref-count to determine when this can be done.
            // Note that this can cause a two-way DB_IPCE_FUNC_EVAL_CLEANUP event
            // to be sent. Hence, it has to be done before the Continue (see bug 102745).


            // Note that if the debugger has already (incorrectly) released the CordbEval,
            // pEval will be pointing to garbage and should not be used by the debugger.
            if (evalCompleted)
            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE2(this, event, "thread=0x%p, eval=0x%p. (Complete)", thread, pEval);
                m_cordb->m_managedCallback->EvalComplete(
                                      (ICorDebugAppDomain*)pResultAppDomain,
                                      (ICorDebugThread*)thread,
                                      (ICorDebugEval*)pEval);
            }
            else
            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE2(this, event, "thread=0x%p, eval=0x%p. (Exception)", thread, pEval);
                m_cordb->m_managedCallback->EvalException(
                                      (ICorDebugAppDomain*)pResultAppDomain,
                                      (ICorDebugThread*)thread,
                                      (ICorDebugEval*)pEval);
            }

            pEval->Release();
        }
        break;


    case DB_IPCE_NAME_CHANGE:
        {
            LOG((LF_CORDB, LL_INFO1000, "RCET::HRCE: Name Change %d  0x%p 0x%08x\n",
                 event->threadId,
                 LsPtrToCookie(event->NameChange.debuggerAppDomainToken),
                 event->NameChange.debuggerThreadIdToken));

            thread = NULL;
            pAppDomain.Clear();
            if (event->NameChange.eventType == THREAD_NAME_CHANGE)
            {
                // Lookup the CordbThread that matches this runtime thread.
                thread = m_userThreads.GetBase(
                                        event->NameChange.debuggerThreadIdToken);
            }
            else
            {
                _ASSERTE (event->NameChange.eventType == APP_DOMAIN_NAME_CHANGE);
                pAppDomain.Assign(m_appDomains.GetBase(LsPtrToCookie(event->appDomainToken)));
                if (pAppDomain)
                {
                    pAppDomain->InvalidateName();
                }
            }

            if (thread || pAppDomain)
            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                m_cordb->m_managedCallback->NameChange(static_cast<ICorDebugAppDomain*> (pAppDomain),
                                                       static_cast<ICorDebugThread*> (thread));
            }
            else
            {
                ContinueInternal(FALSE);
            }
        }

        break;

    case DB_IPCE_UPDATE_MODULE_SYMS:
        {
            IStream *pStream = NULL;
            CordbModule *module = NULL;
            {
                RSLockHolder ch(this->GetStopGoLock());


                // Find the app domain the module lives in.
                _ASSERTE (pAppDomain != NULL);

                // Find the Right Side module for this module.
                module = (CordbModule*) pAppDomain->LookupModule (
                                event->UpdateModuleSymsData.debuggerModuleToken);
                _ASSERTE(module != NULL);

                if (!event->UpdateModuleSymsData.dataBuffer.GetRSPointer())
                {
                    CORDBSetUnrecoverableError(this, E_OUTOFMEMORY, 0);
                    break;
                }

                // Create a stream from the memory.
                hr = CInMemoryStream::CreateStreamOnMemoryCopy(
                                             event->UpdateModuleSymsData.dataBuffer.GetRSPointer(),
                                             event->UpdateModuleSymsData.dataBuffer.GetSize(),
                                             &pStream);
                _ASSERTE(SUCCEEDED(hr) && (pStream != NULL));
            } // implicit release.
            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                m_cordb->m_managedCallback->UpdateModuleSymbols(static_cast<ICorDebugAppDomain*> (pAppDomain),
                                                                static_cast<ICorDebugModule*> (module),
                                                                pStream);
            }

            pStream->Release();
            event->UpdateModuleSymsData.dataBuffer.CleanUp();
        }
        break;

    case DB_IPCE_MDA_NOTIFICATION:
        {
            CordbMDA * pMDA = new (nothrow) CordbMDA(this, &event->MDANotification);


            bool fContinue = false;

            if (pMDA != NULL)
            {
                {
                    // Ctor leaves both internal + ext Ref at 0, adding to neuter list bumps int-ref up to 1.
                    // Neutering will dump it back down to zero.
                    // Must do this while under the stop-go lock.
                    RSLockHolder ch(GetStopGoLock());
                    this->AddToNeuterOnExitList(pMDA);
                }


                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);

                // We bump up and down the external ref so that even if the callback doensn't touch the refs,
                // our Ext-Release here will still cause a 1->0 ext-ref transition, which will get it
                // swept on the neuter list.
                pMDA->AddRef();
                hr = m_cordb->m_managedCallback2->MDANotification(
                    static_cast<ICorDebugController*> (this),
                    static_cast<CordbThread*> (thread), // may be null
                    static_cast<ICorDebugMDA*> (pMDA)
                );

                // This release is very significant because it may be the one that does the 1->0 ext ref transition,
                // which may mean cause the "NeuterAtWill" bit to get flipped on this CordbMDA object.
                pMDA->Release();

                if (hr == E_NOTIMPL)
                {
                    fContinue = true;
                }
            }
            else
            {
                fContinue = true;
            }

            // If the callback is not implemented, then we do the continue on behalf of the client.
            if (fContinue)
            {
                ContinueInternal(FALSE);
            }


            break;
        }

    case DB_IPCE_CONTROL_C_EVENT:
        {
            hr = S_FALSE;

            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                hr = m_cordb->m_managedCallback->ControlCTrap((ICorDebugProcess*) this);
            }

            {
                RSLockHolder ch(this->GetStopGoLock());

                DebuggerIPCEvent eventControlCResult;

                InitIPCEvent(&eventControlCResult,
                             DB_IPCE_CONTROL_C_EVENT_RESULT,
                             false,
                             LSPTR_APPDOMAIN::NullPtr());

                // Indicate the buffer to release.
                eventControlCResult.hr = hr;

                // Send the event
                SendIPCEvent(&eventControlCResult, sizeof(eventControlCResult));
            } // release SG lock

        }
        break;

        case DB_IPCE_ENC_REMAP:
        {
            LOG((LF_CORDB, LL_INFO1000, "[%x] RCET::DRCE: EnC Remap!.\n",
                 GetCurrentThreadId()));

            _ASSERTE(NULL != pAppDomain);

            CordbModule* module = (CordbModule *)pAppDomain->LookupModule(
                                        event->EnCRemap.debuggerModuleToken);
            PREFIX_ASSUME(module != NULL);

            CordbFunction *pCurFunction=NULL, *pResumeFunction=NULL;

                // lookup the version of the function that we are mapping from
                // this is the one that is currently running
                hr = module->LookupOrCreateFunction(
                        event->EnCRemap.funcMetadataToken, 0, event->EnCRemap.currentVersionNumber, &pCurFunction);
                // function must exist
                _ASSERTE(SUCCEEDED(hr));

                // lookup the version of the function that we are mapping to
                // it will always be the most recent
                hr = module->LookupOrCreateFunction(
                        event->EnCRemap.funcMetadataToken, 0, event->EnCRemap.resumeVersionNumber, &pResumeFunction);

                // function must exist
                _ASSERTE(SUCCEEDED(hr));
                _ASSERTE(pCurFunction->m_dwEnCVersionNumber < pResumeFunction->m_dwEnCVersionNumber);

            RSSmartPtr<CordbFunction> pRefCurFunction(pCurFunction);
            RSSmartPtr<CordbFunction> pRefResumeFunction(pResumeFunction);

            thread->m_EnCRemapFunctionIP = event->EnCRemap.resumeILOffset;

            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                m_cordb->m_managedCallback2->FunctionRemapOpportunity(
                                      (ICorDebugAppDomain*)pAppDomain,
                                      (ICorDebugThread*) thread,
                                      (ICorDebugFunction *)pCurFunction,
                                      (ICorDebugFunction *)pResumeFunction,
                                      (ULONG32)event->EnCRemap.currentILOffset);
            }

            // Implicit release on pCurFunction and pResumeFunction.
        }
        break;

        case DB_IPCE_ENC_REMAP_COMPLETE:
        {
            LOG((LF_CORDB, LL_INFO1000, "[%x] RCET::DRCE: EnC Remap Complete!.\n",
                 GetCurrentThreadId()));

            _ASSERTE(NULL != pAppDomain);

            CordbModule* module = (CordbModule *)pAppDomain->LookupModule(event->EnCRemap.debuggerModuleToken);
            PREFIX_ASSUME(module != NULL);

            CordbFunction *pRemapFunction=
                module->LookupFunction(event->EnCRemapComplete.funcMetadataToken);

            // function must exist
            PREFIX_ASSUME(pRemapFunction != NULL);

            RSSmartPtr<CordbFunction> pRef(pRemapFunction);
            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE(this, event);
                m_cordb->m_managedCallback2->FunctionRemapComplete(
                                    (ICorDebugAppDomain*)pAppDomain,
                                    (ICorDebugThread*) thread,
                                    (ICorDebugFunction *)pRemapFunction);
            }
            // Implicit release on pRemapFunction via holder
        }
        break;

        case DB_IPCE_BREAKPOINT_SET_ERROR:
        {
            LOG((LF_CORDB, LL_INFO1000, "RCET::DRCE: breakpoint set error.\n"));

            RSSmartPtr<CordbBreakpoint> pRef;
            Lock();

            _ASSERTE(thread != NULL);
            _ASSERTE(pAppDomain != NULL);

            // Find the breakpoint object on this side.
            CordbBreakpoint *bp = NULL;


            if (thread == NULL)
            {
                // We've found cases out in the wild where we get this event on a thread we don't recognize.
                // We're not sure how this happens. Add a runtime check to protect ourselves to avoid the 
                // an AV. We still assert because this should not be happening.
                // It likely means theres some bug where we failed to send a CreateThread notification.
                STRESS_LOG1(LF_CORDB, LL_INFO1000, "BreakpointSetError on unrecognized thread. %p\n", bp);                

                _ASSERTE(!"Missing thread on bp set error");
            }   
            else
            {
                bp = pAppDomain->m_breakpoints.GetBase(
                                                   LsPtrToCookie(event->BreakpointSetErrorData.breakpointToken));
            }
            

            ICorDebugBreakpoint *ibp = NULL;
            if (bp != NULL)
            {
                pRef.Assign(bp); // take ref while under the lock so that another thread can't remove it on us.

                ibp = CordbBreakpointToInterface(bp);
                _ASSERTE(ibp != NULL);
            }

            Unlock();

            if (bp != NULL)
            {
                _ASSERTE(ibp != NULL);

                PUBLIC_CALLBACK_IN_THIS_SCOPE2(this, event, "thread=0x%p, bp=0x%p", thread, bp);
                m_cordb->m_managedCallback->BreakpointSetError(static_cast<ICorDebugAppDomain*> (pAppDomain),
                                                               static_cast<ICorDebugThread*> (thread),
                                                               ibp,
                                                               0);
            }
            else
            {
                // If we didn't find a breakpoint object on this side,
                // the we have an extra BP event for a breakpoint that
                // has been removed and released on this side. Just
                // ignore the event.
                ContinueInternal(FALSE);
            }
            // Implicit release on pRef.
        }
        break;


    case DB_IPCE_EXCEPTION_CALLBACK2:
        {
            LOG((LF_CORDB, LL_INFO100,
                "RCET::DRCE: Exception2 0x%p 0x%X 0x%X 0x%X\n",
                 event->ExceptionCallback2.framePointer.GetSPValue(),
                 event->ExceptionCallback2.nOffset,
                 event->ExceptionCallback2.eventType,
                 event->ExceptionCallback2.dwFlags
                 ));

            if( thread == NULL )
            {
                // We've got an exception on a thread we don't know about.  This could be a thread that 
                // has never run any managed code, so let's just ignore the exception.  We should have 
                // already sent a log message about this situation for the EXCEPTION callback above.
                _ASSERTE( event->ExceptionCallback2.eventType == DEBUG_EXCEPTION_UNHANDLED );
                ContinueInternal(FALSE);
                break;
            }

            thread->SetExInfo(event->ExceptionCallback2.exceptionHandle);

            //
            // Send all the information back to the debugger.
            //
            CordbFrame* pFrame = NULL;

            if (event->ExceptionCallback2.framePointer == LEAF_MOST_FRAME)
            {
                hr = S_OK;
                pFrame = NULL;
            }
            else
            {
                hr = thread->FindFrame(&pFrame, event->ExceptionCallback2.framePointer);
            }

            if (FAILED(hr))
            {
                pFrame = NULL;
                hr = S_OK;
            }

            if (SUCCEEDED(hr))
            {
                CorDebugExceptionCallbackType type = event->ExceptionCallback2.eventType;

                PUBLIC_CALLBACK_IN_THIS_SCOPE3(this, event, "thread=0x%p, frame=%p, type=%d", thread, pFrame, type);
                hr = m_cordb->m_managedCallback2->Exception(static_cast<ICorDebugAppDomain*> (thread->m_pAppDomain),
                                                            static_cast<ICorDebugThread*> (thread),
                                                            static_cast<ICorDebugFrame*> (pFrame),
                                                            (ULONG32)(event->ExceptionCallback2.nOffset),
                                                            type,
                                                            event->ExceptionCallback2.dwFlags
                                                           );
            }

            if (hr == E_NOTIMPL)
            {
                ContinueInternal(false);
                hr = S_OK;
            }
        }
        break;

    case DB_IPCE_EXCEPTION_UNWIND:
        {
            LOG((LF_CORDB, LL_INFO100,
                "RCET::DRCE: Exception Unwind 0x%X 0x%X\n",
                 event->ExceptionCallback2.eventType,
                 event->ExceptionCallback2.dwFlags
                 ));

            if( thread == NULL )
            {
                // We've got an exception on a thread we don't know about.  This probably should never
                // happen (if it's unwinding, then we expect a managed frame on the stack, and so we should
                // know about the thread), but if it does fall back to ignoring the exception.
                _ASSERTE( !"Got unwind event for unknown exception" );
                ContinueInternal(FALSE);
                break;
            }

            //
            // Send all the information back to the debugger.
            //
            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE1(this, event, "thread=0x%p", thread);
                hr = m_cordb->m_managedCallback2->ExceptionUnwind((ICorDebugAppDomain*) thread->m_pAppDomain,
                                                                  (ICorDebugThread*) thread,
                                                                  event->ExceptionUnwind.eventType,
                                                                  event->ExceptionUnwind.dwFlags
                                                                 );
            }
            if (hr == E_NOTIMPL)
            {
                ContinueInternal(false);
                hr = S_OK;
            }
        }
        break;


    case DB_IPCE_INTERCEPT_EXCEPTION_COMPLETE:
        {
            LOG((LF_CORDB, LL_INFO100, "RCET::DRCE: Exception Interception Complete.\n"));

            if( thread == NULL )
            {
                // We've got an exception on a thread we don't know about.  This probably should never
                // happen (if it's unwinding, then we expect a managed frame on the stack, and so we should
                // know about the thread), but if it does fall back to ignoring the exception.
                _ASSERTE( !"Got complete event for unknown exception" );
                ContinueInternal(FALSE);
                break;
            }

            //
            // Send all the information back to the debugger.
            //
            {
                PUBLIC_CALLBACK_IN_THIS_SCOPE1(this, event, "thread=0x%p", thread);
                hr = m_cordb->m_managedCallback2->ExceptionUnwind((ICorDebugAppDomain*) thread->m_pAppDomain,
                                                                  (ICorDebugThread*) thread,
                                                                  DEBUG_EXCEPTION_INTERCEPTED,
                                                                  0
                                                                 );
            }

            if (hr == E_NOTIMPL)
            {
                ContinueInternal(false);
                hr = S_OK;
            }
        }
        break;



    default:
        _ASSERTE(!"Unknown event");
        LOG((LF_CORDB, LL_INFO1000,
             "[%x] RCET::HRCE: Unknown event: 0x%08x\n",
             GetCurrentThreadId(), event->type));
    }

    // It's possible a ICorDebugProcess::Detach() may have occured by now. 

    } // forces Continue to be called

    Lock();  

    //
    // Set for Continue().
    //
    FinishEventDispatch();

    DeleteIPCEventHelper(event);
}


HRESULT CordbProcess::EnumerateThreads(ICorDebugThreadEnum **ppThreads)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_EVERETT_HACK_REQUIRE_STOPPED_ALLOW_NONINIT(this);
    VALIDATE_POINTER_TO_OBJECT(ppThreads,ICorDebugThreadEnum **);

    CordbHashTableEnum *e = new (nothrow) CordbHashTableEnum(this, this->GetContinueNeuterList(), &m_userThreads,
                                                   IID_ICorDebugThreadEnum);
    if (e == NULL)
        return E_OUTOFMEMORY;

    *ppThreads = static_cast<ICorDebugThreadEnum*> (e);
    e->ExternalAddRef();

    return S_OK;
}

HRESULT CordbProcess::GetThread(DWORD dwThreadId, ICorDebugThread **ppThread)
{
    PUBLIC_API_ENTRY(this);
    VALIDATE_POINTER_TO_OBJECT(ppThread, ICorDebugThread **);

    // No good pre-existing ATT_* contract for this.
    // Because for legacy, we have to allow this on the win32 event thread.

    HRESULT hr = S_OK;

    CordbThread *t = m_userThreads.GetBase(dwThreadId);

    if (t == NULL)
    {
        hr = E_INVALIDARG;
        goto LExit;
    }

    *ppThread = static_cast<ICorDebugThread*> (t);
    t->ExternalAddRef();

LExit:
    return hr;
}

HRESULT CordbProcess::ThreadForFiberCookie(DWORD fiberCookie,
                                           ICorDebugThread **ppThread)
{
    return E_NOTIMPL;
}

HRESULT CordbProcess::GetHelperThreadID(DWORD *pThreadID)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    if (pThreadID == NULL)
        return (E_INVALIDARG);

    // Return the ID of the current helper thread. There may be no thread in the process, or there may be a true helper
    // thread.
    if ((m_helperThreadId != 0) && !m_helperThreadDead)
        *pThreadID = m_helperThreadId;
    else if ((m_DCB != NULL) && (m_DCB->m_helperThreadId != 0))
        *pThreadID = m_DCB->m_helperThreadId;
    else
        *pThreadID = 0;

    return S_OK;
}

HRESULT CordbProcess::SetAllThreadsDebugState(CorDebugThreadState state,
                                              ICorDebugThread *pExceptThread)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pExceptThread,ICorDebugThread *);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this);

    CordbThread *et = static_cast<CordbThread*> (pExceptThread);

    LOG((LF_CORDB, LL_INFO1000, "CP::SATDS: except thread=0x%08x 0x%x\n", pExceptThread, et != NULL ? et->m_id : 0));

    // Send one event to the Left Side to twiddle each thread's state.
    DebuggerIPCEvent event;
    InitIPCEvent(&event, DB_IPCE_SET_ALL_DEBUG_STATE, true, LSPTR_APPDOMAIN::NullPtr());
    event.SetAllDebugState.debuggerExceptThreadToken = (et != NULL) ? et->m_debuggerThreadToken : LSPTR_THREAD::NullPtr();
    event.SetAllDebugState.debugState = state;

    HRESULT hr = SendIPCEvent(&event, sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event.hr);

    // If that worked, then loop over all the threads on this side and set their states.
    if (SUCCEEDED(hr))
    {
        HASHFIND        find;
        CordbThread    *thread;

        for (thread = m_userThreads.FindFirst(&find);
             thread != NULL;
             thread = m_userThreads.FindNext(&find))
        {
            if (thread != et)
            {
                thread->m_debugState = state;
            }
        }
    }

    return hr;
}


HRESULT CordbProcess::EnumerateObjects(ICorDebugObjectEnum **ppObjects)
{
    /* !!! */
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppObjects, ICorDebugObjectEnum **);

    return E_NOTIMPL;
}

HRESULT CordbProcess::IsTransitionStub(CORDB_ADDRESS address, BOOL *pbTransitionStub)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pbTransitionStub, BOOL *);

    // Default to FALSE
    *pbTransitionStub = FALSE;

    if (this->m_helperThreadDead)
    {
        return S_OK;
    }

    // If we're not initialized, then it can't be a stub...
    if (!m_initialized)
    {
        return S_OK;
    }


    ATT_REQUIRE_STOPPED_MAY_FAIL(this);

    LOG((LF_CORDB, LL_INFO1000, "CP::ITS: addr=0x%08x\n", address));

    DebuggerIPCEvent *event =
      (DebuggerIPCEvent *) _alloca(CorDBIPC_BUFFER_SIZE);

    InitIPCEvent(event, DB_IPCE_IS_TRANSITION_STUB, true, LSPTR_APPDOMAIN::NullPtr());

    event->IsTransitionStub.address = CORDB_ADDRESS_TO_PTR(address);

    HRESULT hr = SendIPCEvent(event, CorDBIPC_BUFFER_SIZE);
    hr = WORST_HR(hr, event->hr);

    if (FAILED(hr))
    {
        return hr;
    }

    _ASSERTE(event->type == DB_IPCE_IS_TRANSITION_STUB_RESULT);

    *pbTransitionStub = event->IsTransitionStubResult.isStub;


    return S_OK;
}


HRESULT CordbProcess::SetStopState(DWORD threadID, CorDebugThreadState state)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    return E_NOTIMPL;
}

HRESULT CordbProcess::IsOSSuspended(DWORD threadID, BOOL *pbSuspended)
{
    PUBLIC_API_ENTRY(this);
    // Gotta have a place for the result!
    if (!pbSuspended)
        return E_INVALIDARG;

    FAIL_IF_NEUTERED(this);

    // Have we seen this thread?
    CordbUnmanagedThread *ut = GetUnmanagedThread(threadID);

    // If we have, and if we've suspended it, then say so.
    if (ut && ut->IsSuspended())
        *pbSuspended = TRUE;
    else
        *pbSuspended = FALSE;

    return S_OK;
}

//
// This routine reads a thread context from the process being debugged, taking into account the fact that the context
// record may be a different size than the one we compiled with. On systems < NT5, then OS doesn't usually allocate
// space for the extended registers. However, the CONTEXT struct that we compile with does have this space.
//
HRESULT CordbProcess::SafeReadThreadContext(LSPTR_CONTEXT pContext, CONTEXT *pCtx)
{
    HRESULT hr = S_OK;
    SIZE_T nRead = 0;

    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    void *pRemoteContext = pContext.UnsafeGet();


    // At a minimum we have room for a whole context up to the extended registers.
#if defined(CONTEXT_EXTENDED_REGISTERS)
    SIZE_T minContextSize = offsetof(CONTEXT, ExtendedRegisters);
    // The extended registers are optional...
    SIZE_T extRegSize = sizeof(CONTEXT) - minContextSize;

    // Start of the extended registers, in the remote process and in the current process
    void *pRmtExtReg = (void*)((UINT_PTR)pRemoteContext + minContextSize);
    void *pCurExtReg = (void*)((UINT_PTR)pCtx + minContextSize);
#else
    SIZE_T minContextSize = sizeof(CONTEXT);
#endif

    // Read the minimum part.
    BOOL succ = ReadProcessMemoryI(m_handle, pRemoteContext, pCtx, minContextSize, &nRead);

    if (!succ || (nRead != minContextSize))
    {
        hr = HRESULT_FROM_GetLastError();
    }

#if defined(CONTEXT_EXTENDED_REGISTERS)
    // Now, read the extended registers if the context contains them. If the context does not have extended registers,
    // just set them to zero.
    if (SUCCEEDED(hr) && (pCtx->ContextFlags & CONTEXT_EXTENDED_REGISTERS) == CONTEXT_EXTENDED_REGISTERS)
    {
        succ = ReadProcessMemoryI(m_handle, pRmtExtReg, pCurExtReg, extRegSize, &nRead);

        if (!succ || (nRead != extRegSize))
        {
            hr = HRESULT_FROM_GetLastError();
        }
    }
    else
    {
        memset(pCurExtReg, 0, extRegSize);
    }
#endif

    return hr;
}

//
// This routine writes a thread context to the process being debugged, taking into account the fact that the context
// record may be a different size than the one we compiled with. On systems < NT5, then OS doesn't usually allocate
// space for the extended registers. However, the CONTEXT struct that we compile with does have this space.
//
HRESULT CordbProcess::SafeWriteThreadContext(LSPTR_CONTEXT pContext, CONTEXT *pCtx)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    HRESULT hr = S_OK;
    SIZE_T nWritten = 0;
    DWORD sizeToWrite = sizeof(CONTEXT);

    void *pRemoteContext = pContext.UnsafeGet();


#if defined(CONTEXT_EXTENDED_REGISTERS)
    // If our context has extended registers, then write the whole thing. Otherwise, just write the minimum part.
    if ((pCtx->ContextFlags & CONTEXT_EXTENDED_REGISTERS) != CONTEXT_EXTENDED_REGISTERS)
        sizeToWrite = offsetof(CONTEXT, ExtendedRegisters);
#endif

    // Write the context.
    BOOL succ = WriteProcessMemory(m_handle, pRemoteContext, pCtx, sizeToWrite, &nWritten);

    if (!succ || (nWritten != sizeToWrite))
    {
        hr = HRESULT_FROM_GetLastError();
    }
    return hr;
}


HRESULT CordbProcess::GetThreadContext(DWORD threadID, ULONG32 contextSize, BYTE context[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    FAIL_IF_MANAGED_ONLY(this);

    CONTEXT * pContext;

    if (contextSize != sizeof(CONTEXT))
    {
        LOG((LF_CORDB, LL_INFO10000, "CP::GTC: thread=0x%x, context size is invalid.\n", threadID));
        return E_INVALIDARG;
    }

    pContext = reinterpret_cast<CONTEXT*> (context);

    VALIDATE_POINTER_TO_OBJECT_ARRAY(context, BYTE, contextSize, true, true);

    return E_NOTIMPL;
}

HRESULT CordbProcess::SetThreadContext(DWORD threadID, ULONG32 contextSize, BYTE context[])
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_MANAGED_ONLY(this);

    return E_NOTIMPL;
}


HRESULT CordbProcess::ReadMemory(CORDB_ADDRESS address,
                                 DWORD size,
                                 BYTE buffer[],
                                 SIZE_T *read)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    // A read of 0 bytes is okay.
    if (size == 0)
        return S_OK;

    VALIDATE_POINTER_TO_OBJECT_ARRAY(buffer, BYTE, size, true, true);
    VALIDATE_POINTER_TO_OBJECT(buffer, SIZE_T *);

    if (address == NULL)
        return E_INVALIDARG;

    // If no read parameter is supplied, we ignore it. This matches the semantics of kernel32!ReadProcessMemory.
    SIZE_T dummyRead;
    if (read == NULL)
    {
        read = &dummyRead;
    }
    *read = 0;

    HRESULT hr = S_OK;
    HRESULT hrSaved = hr; // this will hold the 'real' hresult in case of a partially completed operation.
    HRESULT hrPartialCopy = HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY);


    CORDBRequireProcessStateOK(this);

    //grab the memory we want to read
    if (ReadProcessMemoryI(m_handle, (LPCVOID) CORDB_ADDRESS_TO_PTR(address), buffer, size, read) == 0)
    {
        hr = HRESULT_FROM_GetLastError();

        if (hr != hrPartialCopy)
            goto LExit;
        else
            hrSaved = hr;
    }

    // There seem to be strange cases where ReadProcessMemory will return a seemingly negative number into *read, which
    // is an unsigned value. So we check the sanity of *read by ensuring that its no bigger than the size we tried to
    // read.
    if ((*read > 0) && (*read <= size))
    {
        LOG((LF_CORDB, LL_INFO100000, "CP::RM: read %d bytes from 0x%08x, first byte is 0x%x\n",
             *read, (DWORD)address, buffer[0]));

        if (m_initialized)
        {
            RSLockHolder ch(&this->m_processMutex);

            // If m_pPatchTable is NULL, then it's been cleaned out b/c of a Continue for the left side.  Get the table
            // again. Only do this, of course, if the managed state of the process is initialized.
            if (m_pPatchTable == NULL)
            {
                hr = RefreshPatchTable(address, *read, buffer);
            }
            else
            {
                // The previously fetched table is still good, so run through it & see if any patches are applicable
                hr = AdjustBuffer(address, *read, buffer, NULL, AB_READ);
            }
        }
    }

LExit:
    if (FAILED(hr))
    {
        RSLockHolder ch(&this->m_processMutex);
        ClearPatchTable();
    }
    else if (FAILED(hrSaved))
    {
        hr = hrSaved;
    }

    return hr;
}

//
// This function checks if a patch falls (fully or partially) in the requested range of memory.
//
inline bool IsPatchInRequestedRange(CORDB_ADDRESS requestedAddr,
                                    SIZE_T requestedSize,
                                    CORDB_ADDRESS patchAddr
                                    IA64_ARG(PRD_TYPE *pPRD))
{
    if (requestedAddr == 0)
        return false;

    // Note that patchEnd points to the byte immediately AFTER the patch, so patchEnd is NOT
    // part of the patch.
    CORDB_ADDRESS patchEnd = GetPatchEndAddr(patchAddr  IA64_ARG(pPRD));

    // We have three cases:
    // 1) the entire patch is in the requested range
    // 2) the beginning of the requested range is covered by the patch
    // 3) the end of the requested range is covered by the patch
    //
    // Note that on x86, since the break instruction only takes up one byte, the following condition
    // degenerates to case 1 only.
    return ((requestedAddr <= patchAddr && patchEnd <= (requestedAddr + requestedSize)) ||
            (patchAddr <= requestedAddr && requestedAddr <= patchEnd) ||
            (patchAddr <= (requestedAddr + requestedSize) && (requestedAddr + requestedSize) <= patchEnd));
}

// Update patches & buffer to make the left-side's usage of patches transparent
// to our client.  Behavior depends on AB_MODE:
// AB_READ:
// - use the RS patch table structure to replace patch opcodes in buffer.
// AB_WRITE:
// - update the RS patch table structure w/ new replace-opcode values
// if we've written over them. And put the int3 back in for write-memory.
//
// Note: If we're writing memory over top of a patch, then it must be JITted or stub code.
// Writing over JITed or Stub code can be dangerous since the CLR may not expect it 
// (eg. JIT data structures about the code layout may be incorrect), but in certain
// narrow cases it may be safe (eg. replacing a constant).  VS says they wouldn't expect 
// this to work, but we'll keep the support in for legacy reasons.
//
// address, size - describe buffer in LS memory
// buffer - local copy of buffer that will be read/written from/to LS.
// bufferCopy - for writeprocessmemory, copy of original buffer (w/o injected patches)
// pbUpdatePatchTable - flag if patchtable got dirty and needs to be updated.
HRESULT CordbProcess::AdjustBuffer( CORDB_ADDRESS address,
                                    SIZE_T size,
                                    BYTE buffer[],
                                    BYTE **bufferCopy,
                                    AB_MODE mode,
                                    BOOL *pbUpdatePatchTable)
{
    INTERNAL_API_ENTRY(this);

    _ASSERTE(m_initialized);
    _ASSERTE(this->ThreadHoldsProcessLock());

    if (    address == NULL
         || size == NULL
         || buffer == NULL
         || (mode != AB_READ && mode != AB_WRITE) )
        return E_INVALIDARG;

    if (pbUpdatePatchTable != NULL )
        *pbUpdatePatchTable = FALSE;

    // If we don't have a patch table loaded, then return S_OK since there are no patches to adjust
    if (m_pPatchTable == NULL)
        return S_OK;

    //is the requested memory completely out-of-range?
    if ((m_minPatchAddr > (address + (size - 1))) ||
        (m_maxPatchAddr < address))
    {
        return S_OK;
    }

    LOG((LF_CORDB,LL_INFO10000, "CordbProcess::AdjustBuffer at addr 0x%p\n", address));

    if (mode == AB_WRITE)
    {
        // We don't want to mess up the original copy of the buffer, so
        // for right now, just copy it wholesale.
        (*bufferCopy) = new (nothrow) BYTE[size];
        if (NULL == (*bufferCopy))
            return E_OUTOFMEMORY;

        memmove((*bufferCopy), buffer, size);
    }

    ULONG iNextFree = m_iFirstPatch;
    while( iNextFree != DPT_TERMINATING_INDEX )
    {
        BYTE *DebuggerControllerPatch = m_pPatchTable + m_runtimeOffsets.m_cbPatch*iNextFree;
        PRD_TYPE opcode = *(PRD_TYPE *)(DebuggerControllerPatch + m_runtimeOffsets.m_offOpcode);
        CORDB_ADDRESS patchAddress = PTR_TO_CORDB_ADDRESS(*(BYTE**)(DebuggerControllerPatch + m_runtimeOffsets.m_offAddr));

        if (IsPatchInRequestedRange(address, size, patchAddress
                                    IA64_ARG(&opcode)))
        {
            if (mode == AB_READ)
            {
                CORDbgSetInstructionEx(buffer, address, patchAddress, opcode, size);
            }
            else if (mode == AB_WRITE)
            {
                _ASSERTE( pbUpdatePatchTable != NULL );
                _ASSERTE( bufferCopy != NULL );

                //There can be multiple patches at the same address: we don't want 2nd+ patches to get the 
                // break opcode, so we read from the unmodified copy.
                m_rgUncommitedOpcode[iNextFree] =
                    CORDbgGetInstructionEx(*bufferCopy, address, patchAddress, opcode, size);

                //put the breakpoint into the memory itself
                CORDbgInsertBreakpointEx(buffer, address, patchAddress, opcode, size);

                *pbUpdatePatchTable = TRUE;
            }
            else
                _ASSERTE( !"CordbProcess::AdjustBuffergiven non(Read|Write) mode!" );
        }

        iNextFree = m_rgNextPatch[iNextFree];
    }

    // If we created a copy of the buffer but didn't modify it, then free it now.
    if( ( mode == AB_WRITE ) && ( !*pbUpdatePatchTable ) )
    {
        delete [] *bufferCopy;
        *bufferCopy = NULL;
    }
    
    return S_OK;
}


void CordbProcess::CommitBufferAdjustments( CORDB_ADDRESS start,
                                            CORDB_ADDRESS end )
{
    INTERNAL_API_ENTRY(this);

    _ASSERTE(m_initialized);
    _ASSERTE(this->ThreadHoldsProcessLock());

    ULONG iPatch = m_iFirstPatch;
    while( iPatch != DPT_TERMINATING_INDEX )
    {
        BYTE *DebuggerControllerPatch = m_pPatchTable +
            m_runtimeOffsets.m_cbPatch*iPatch;

        BYTE *patchAddress = *(BYTE**)(DebuggerControllerPatch + m_runtimeOffsets.m_offAddr);
        IA64_ONLY(PRD_TYPE opcode    = *(PRD_TYPE *)(DebuggerControllerPatch + m_runtimeOffsets.m_offOpcode);)

        if (IsPatchInRequestedRange(start, (SIZE_T)(end - start), PTR_TO_CORDB_ADDRESS(patchAddress)
                                    IA64_ARG(&opcode)) &&
            !PRDIsBreakInst(&(m_rgUncommitedOpcode[iPatch])))
        {
            //copy this back to the copy of the patch table
            *(PRD_TYPE *)(DebuggerControllerPatch + m_runtimeOffsets.m_offOpcode) =
                m_rgUncommitedOpcode[iPatch];
        }

        iPatch = m_rgNextPatch[iPatch];
    }
}

void CordbProcess::ClearBufferAdjustments( )
{
    INTERNAL_API_ENTRY(this);
    _ASSERTE(this->ThreadHoldsProcessLock());

    ULONG iPatch = m_iFirstPatch;
    while( iPatch != DPT_TERMINATING_INDEX )
    {
        InitializePRDToBreakInst(&(m_rgUncommitedOpcode[iPatch]));
        iPatch = m_rgNextPatch[iPatch];
    }
}

void CordbProcess::ClearPatchTable(void )
{
    INTERNAL_API_ENTRY(this);
    _ASSERTE(this->ThreadHoldsProcessLock());

    if (m_pPatchTable != NULL )
    {
        delete [] m_pPatchTable;
        m_pPatchTable = NULL;

        delete [] m_rgNextPatch;
        m_rgNextPatch = NULL;

        delete [] m_rgUncommitedOpcode;
        m_rgUncommitedOpcode = NULL;

        m_iFirstPatch = DPT_TERMINATING_INDEX;
        m_minPatchAddr = MAX_ADDRESS;
        m_maxPatchAddr = MIN_ADDRESS;
        m_rgData = NULL;
        m_cPatch = 0;
    }
}

HRESULT CordbProcess::RefreshPatchTable(CORDB_ADDRESS address, SIZE_T size, BYTE buffer[])
{
    INTERNAL_API_ENTRY(this);
    _ASSERTE(m_initialized);
    _ASSERTE(this->ThreadHoldsProcessLock());

    HRESULT hr = S_OK;
    BYTE *rgb = NULL;
    SIZE_T nRead = 0;
    BOOL fOk = false;

    _ASSERTE( m_runtimeOffsets.m_cbOpcode == sizeof(PRD_TYPE) );

    CORDBRequireProcessStateOK(this);

    if (m_pPatchTable == NULL )
    {
        // First, check to be sure the patch table is valid on the Left Side. If its not, then we won't read it.
        BOOL fPatchTableValid = FALSE;

        fOk = ReadProcessMemoryI(m_handle, m_runtimeOffsets.m_pPatchTableValid,
                                 &fPatchTableValid, sizeof(fPatchTableValid), &nRead);

        if (!fOk || (nRead != sizeof(fPatchTableValid)) || !fPatchTableValid)
        {
            LOG((LF_CORDB, LL_INFO10000, "Wont refresh patch table because its not valid now.\n"));
            return S_OK;
        }

        SIZE_T offStart = 0;
        SIZE_T offEnd = 0;
        UINT cbTableSlice = 0;

        // Grab the patch table info
        offStart = min(m_runtimeOffsets.m_offRgData, m_runtimeOffsets.m_offCData);
        offEnd   = max(m_runtimeOffsets.m_offRgData, m_runtimeOffsets.m_offCData) + sizeof(SIZE_T);
        cbTableSlice = (UINT)(offEnd - offStart);

        if (cbTableSlice == 0)
        {
            LOG((LF_CORDB, LL_INFO10000, "Wont refresh patch table because its not valid now.\n"));
            return S_OK;
        }

        rgb = new (nothrow) BYTE[cbTableSlice];

        if (rgb == NULL )
        {
            hr = E_OUTOFMEMORY;
            goto LExit;
        }

        fOk = ReadProcessMemoryI(m_handle, (BYTE*)m_runtimeOffsets.m_pPatches + offStart,
                                 rgb, cbTableSlice, &nRead);

        if ( !fOk || (nRead != cbTableSlice ) )
        {
            hr = HRESULT_FROM_GetLastErrorNA();
            goto LExit;
        }

        // Note that rgData is a pointer in the left side address space
        m_rgData = *(BYTE**)(rgb + m_runtimeOffsets.m_offRgData - offStart);
        m_cPatch = *(ULONG*)(rgb + m_runtimeOffsets.m_offCData - offStart);

        // Grab the patch table
        UINT cbPatchTable = (UINT)(m_cPatch * m_runtimeOffsets.m_cbPatch);

        if (cbPatchTable == 0)
        {
            LOG((LF_CORDB, LL_INFO10000, "Wont refresh patch table because its not valid now.\n"));
            return S_OK;
        }

        m_pPatchTable = new (nothrow) BYTE[ cbPatchTable ];
        m_rgNextPatch = new (nothrow) ULONG[m_cPatch];
        m_rgUncommitedOpcode = new (nothrow) PRD_TYPE[m_cPatch];

        if (m_pPatchTable == NULL || m_rgNextPatch == NULL || m_rgUncommitedOpcode == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto LExit;
        }

        fOk = ReadProcessMemoryI(m_handle, m_rgData, m_pPatchTable, cbPatchTable, &nRead);

        if ( !fOk || (nRead != cbPatchTable ) )
        {
            hr = HRESULT_FROM_GetLastErrorNA();
            goto LExit;
        }

        //As we go through the patch table we do three things:
        //
        // 1. collect min,max address seen for quick fail check
        //
        // 2. Link all valid entries into a linked list, the first entry of which is m_iFirstPatch
        //
        // 3. Initialize m_rgUncommitedOpcode, so that we can undo local patch table changes if WriteMemory can't write
        // atomically.
        //
        // 4. If the patch is in the memory we grabbed, unapply it.

        ULONG iDebuggerControllerPatchPrev = DPT_TERMINATING_INDEX;

        m_minPatchAddr = MAX_ADDRESS;
        m_maxPatchAddr = MIN_ADDRESS;
        m_iFirstPatch = DPT_TERMINATING_INDEX;

        for (ULONG iPatch = 0; iPatch < m_cPatch;iPatch++)
        {
            BYTE *DebuggerControllerPatch = m_pPatchTable + m_runtimeOffsets.m_cbPatch * iPatch;
            PRD_TYPE opcode = *(PRD_TYPE*)(DebuggerControllerPatch + m_runtimeOffsets.m_offOpcode);
            CORDB_ADDRESS patchAddress = PTR_TO_CORDB_ADDRESS(*(BYTE**)(DebuggerControllerPatch + m_runtimeOffsets.m_offAddr));

            // A non-zero opcode indicates to us that this patch is valid.
            if (!PRDIsEmpty(opcode))
            {
                _ASSERTE( patchAddress != 0 );

                // (1), above
                // Note that GetPatchEndAddr() returns the address immediately AFTER the patch,
                // so we have to subtract 1 from it below.
                if (m_minPatchAddr > patchAddress )
                    m_minPatchAddr = patchAddress;
                if (m_maxPatchAddr < patchAddress )
                    m_maxPatchAddr = GetPatchEndAddr(patchAddress  IA64_ARG(&opcode)) - 1;

                // (2), above
                if ( m_iFirstPatch == DPT_TERMINATING_INDEX)
                {
                    m_iFirstPatch = iPatch;
                    _ASSERTE( iPatch != DPT_TERMINATING_INDEX);
                }

                if (iDebuggerControllerPatchPrev != DPT_TERMINATING_INDEX)
                {
                    m_rgNextPatch[iDebuggerControllerPatchPrev] = iPatch;
                }

                iDebuggerControllerPatchPrev = iPatch;

                // (3), above
                InitializePRDToBreakInst(&(m_rgUncommitedOpcode[iPatch]));

                // (4), above
                if (IsPatchInRequestedRange(address, size, patchAddress
                                            IA64_ARG(&opcode)))
                {
                    _ASSERTE( buffer != NULL );
                    _ASSERTE( size != NULL );


                    //unapply the patch here.
                    CORDbgSetInstructionEx(buffer, address, patchAddress, opcode, size);
                }

            }
        }

        if (iDebuggerControllerPatchPrev != DPT_TERMINATING_INDEX)
            m_rgNextPatch[iDebuggerControllerPatchPrev] = DPT_TERMINATING_INDEX;

    }

 LExit:
    if (rgb != NULL )
    {
        delete [] rgb;
    }

    if (FAILED( hr ) )
    {
        ClearPatchTable();
    }

    return hr;
}

//
// Given an address, see if there is a patch in the patch table that matches it and return if its an unmanaged patch or
// not.
//
// Note: this method is pretty in-efficient. It refreshes the patch table, then scans it. Refreshing the patch table
// involves a scan, too, so this method could be folded with that.
//
HRESULT CordbProcess::FindPatchByAddress(CORDB_ADDRESS address, bool *patchFound, bool *patchIsUnmanaged)
{
    INTERNAL_API_ENTRY(this);
    _ASSERTE(ThreadHoldsProcessLock());

    _ASSERTE(patchFound != NULL && patchIsUnmanaged != NULL);
    FAIL_IF_NEUTERED(this);

    *patchFound = false;
    *patchIsUnmanaged = false;

    // First things first. If the process isn't initialized, then there can be no patch table, so we know the breakpoint
    // doesn't belong to the Runtime.
    if (!m_initialized)
        return S_OK;

    // This method is called from the main loop of the win32 event thread in response to a first chance breakpoint event
    // that we know is not a flare. The process has been runnning, and it may have invalidated the patch table, so we'll
    // flush it here before refreshing it to make sure we've got the right thing.
    //
    // Note: we really should have the Left Side mark the patch table dirty to help optimize this.
    ClearPatchTable();

    // Refresh the patch table.
    HRESULT hr = RefreshPatchTable();

    if (FAILED(hr))
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::FPBA: failed to refresh the patch table\n"));
        return hr;
    }

    // If there is no patch table yet, then we know there is no patch at the given address, so return S_OK with
    // *patchFound = false.
    if (m_pPatchTable == NULL)
    {
        LOG((LF_CORDB, LL_INFO1000, "CP::FPBA: no patch table\n"));
        return S_OK;
    }

    // Scan the patch table for a matching patch.
    for (ULONG iNextPatch = m_iFirstPatch; iNextPatch != DPT_TERMINATING_INDEX; iNextPatch = m_rgNextPatch[iNextPatch])
    {
        BYTE *patch = m_pPatchTable + (m_runtimeOffsets.m_cbPatch * iNextPatch);
        BYTE *patchAddress = *(BYTE**)(patch + m_runtimeOffsets.m_offAddr);
        DWORD traceType = *(DWORD*)(patch + m_runtimeOffsets.m_offTraceType);

        if (address == PTR_TO_CORDB_ADDRESS(patchAddress))
        {
            *patchFound = true;

            if (traceType == m_runtimeOffsets.m_traceTypeUnmanaged)
            {
                *patchIsUnmanaged = true;

            }

            break;
        }
    }

    // If we didn't find a patch, its actually still possible that this breakpoint exception belongs to us. There are
    // races with very large numbers of threads entering the Runtime through the same managed function. We will have
    // multiple threads adding and removing ref counts to an int 3 in the code stream. Sometimes, this count will go to
    // zero and the int 3 will be removed, then it will come back up and the int 3 will be replaced. The in-process
    // logic takes pains to ensure that such cases are handled properly, therefore we need to perform the same check
    // here to make the correct decision. Basically, the check is to see if there is indeed an int 3 at the exception
    // address. If there is _not_ an int 3 there, then we've hit this race. We will lie and say a managed patch was
    // found to cover this case. This is tracking the logic in DebuggerController::ScanForTriggers, where we call
    // IsPatched.
    if (*patchFound == false)
    {
        // Read one instruction from the faulting address...
        BYTE TrapCheck = 0;

        BOOL succ = ReadProcessMemoryI(m_handle, CORDB_ADDRESS_TO_PTR(address), (LPVOID)&TrapCheck, sizeof(TrapCheck), NULL);

        if (succ && (TrapCheck != CORDbg_BREAK_INSTRUCTION))
        {
            LOG((LF_CORDB, LL_INFO1000, "CP::FPBA: patchFound=true based on odd missing int 3 case.\n"));

            *patchFound = true;
        }
    }

    LOG((LF_CORDB, LL_INFO1000, "CP::FPBA: patchFound=%d, patchIsUnmanaged=%d\n", *patchFound, *patchIsUnmanaged));

    return S_OK;
}

HRESULT CordbProcess::WriteMemory(CORDB_ADDRESS address, DWORD size,
                                  BYTE buffer[], SIZE_T *written)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    CORDBRequireProcessStateOK(this);

    if (size == 0 || address == NULL)
        return E_INVALIDARG;

    VALIDATE_POINTER_TO_OBJECT_ARRAY(buffer, BYTE, size, true, true);
    VALIDATE_POINTER_TO_OBJECT(written, SIZE_T *);


#ifdef _DEBUG
    // Shouldn't be using this to write int3. Use UM BP API instead.
    // This is technically legal (what if the '0xcc' is data or something), so we can't fail in retail.
    // But we can add this debug-only check to help VS migrate to the new API.
    static ConfigDWORD configCheckInt3;
    DWORD fCheckInt3 = configCheckInt3.val(L"DbgCheckInt3", 0);
    if (fCheckInt3)
    {
#if defined(_X86_) || defined(_AMD64_)
        if (size == 1 && buffer[0] == 0xCC)
        {
            CONSISTENCY_CHECK_MSGF(false,
                ("You're using ICorDebugProcess::WriteMemory() to write an 'int3' (1 byte 0xCC) at address 0x%p.\n"
                "If you're trying to set a breakpoint, you should be using ICorDebugProcess::SetUnmanagedBreakpoint() instead.\n"
                "(This assert is only enabled under the COM+ knob DbgCheckInt3.)\n",
                CORDB_ADDRESS_TO_PTR(address)));
        }
#endif // _X86_

        // check if we're replaced an opcode.
        if (size == 1)
        {
            RSLockHolder ch(&this->m_processMutex);
            
            NativePatch * p = GetNativePatch(CORDB_ADDRESS_TO_PTR(address));
            if (p != NULL)
            {
            CONSISTENCY_CHECK_MSGF(false,
                ("You're using ICorDebugProcess::WriteMemory() to write an 'opcode (0x%x)' at address 0x%p.\n"
                "There's already a native patch at that address from ICorDebugProcess::SetUnmanagedBreakpoint().\n"
                "If you're trying to remove the breakpoint, use ICDProcess::ClearUnmanagedBreakpoint() instead.\n"
                "(This assert is only enabled under the COM+ knob DbgCheckInt3.)\n",
                (DWORD) (buffer[0]), CORDB_ADDRESS_TO_PTR(address)));
            }
        }
    }
#endif


    *written = 0;

    HRESULT hr = S_OK;
    HRESULT hrSaved = hr; // this will hold the 'real' hresult in case of a
                          // partially completed operation
    HRESULT hrPartialCopy = HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY);

    SIZE_T nWritten = 0;
    BOOL bUpdateOriginalPatchTable = FALSE;
    BYTE *bufferCopy = NULL;


    // Only update the patch table if the managed state of the process
    // is initialized.
    if (m_initialized)
    {
        RSLockHolder ch(&this->m_processMutex);
        
        if (m_pPatchTable == NULL )
        {
            if (!SUCCEEDED( hr = RefreshPatchTable() ) )
            {
                goto LExit;
            }
        }

        if ( !SUCCEEDED( hr = AdjustBuffer( address,
                                            size,
                                            buffer,
                                            &bufferCopy,
                                            AB_WRITE,
                                            &bUpdateOriginalPatchTable)))
        {
            goto LExit;
        }
    }

    //conveniently enough, WPM will fail if it can't complete the entire
    //operation
    if ( WriteProcessMemory( m_handle,
                             CORDB_ADDRESS_TO_PTR(address),
                             buffer,
                             size,
                             written) == 0)
    {
        hr = HRESULT_FROM_GetLastError();
        if(hr != hrPartialCopy)
            goto LExit;
        else
            hrSaved = hr;
    }

    LOG((LF_CORDB, LL_INFO100000, "CP::WM: wrote %d bytes at 0x%08x, first byte is 0x%x\n",
         *written, (DWORD)address, buffer[0]));

    if (bUpdateOriginalPatchTable == TRUE )
    {
        {
            RSLockHolder ch(&this->m_processMutex);

            //don't tweak patch table for stuff that isn't written to LeftSide
            CommitBufferAdjustments(address, address + *written);
        }

        // The only way this should be able to fail is if
        //someone else fiddles with the memory protections on the
        //left side while it's frozen
        WriteProcessMemory( m_handle,
                            (LPVOID)m_rgData,
                            m_pPatchTable,
                            m_cPatch*m_runtimeOffsets.m_cbPatch,
                            &nWritten);
        _ASSERTE( nWritten ==m_cPatch*m_runtimeOffsets.m_cbPatch);
    }

    // Since we may have
    // overwritten anything (objects, code, etc), we should mark
    // everything as needing to be re-cached.
    m_continueCounter++;

 LExit:
    if (m_initialized)
    {
        RSLockHolder ch(&this->m_processMutex);
        ClearBufferAdjustments( );
    }

    //we messed up our local copy, so get a clean copy the next time
    //we need it
    if (bUpdateOriginalPatchTable==TRUE)
    {
        if (bufferCopy != NULL)
        {
            memmove(buffer, bufferCopy, size);
            delete bufferCopy;
        }
    }

    if (FAILED( hr ))
    {
        //we messed up our local copy, so get a clean copy the next time
        //we need it
        if (bUpdateOriginalPatchTable==TRUE)
        {
            RSLockHolder ch(&this->m_processMutex);
            ClearPatchTable();
        }
    }
    else if( FAILED(hrSaved) )
    {
        hr = hrSaved;
    }

    return hr;
}

HRESULT CordbProcess::ClearCurrentException(DWORD threadID)
{
    return E_INVALIDARG;
}

CordbUnmanagedThread *CordbProcess::HandleUnmanagedCreateThread(DWORD dwThreadId, HANDLE hThread, void *lpThreadLocalBase)
{
    INTERNAL_API_ENTRY(this);
    CordbUnmanagedThread *ut = new (nothrow) CordbUnmanagedThread(this, dwThreadId, hThread, lpThreadLocalBase);

    if (ut != NULL)
    {
        HRESULT hr = m_unmanagedThreads.AddBase(ut); // InternalAddRef, release on EXIT_THREAD events.

        if (!SUCCEEDED(hr))
        {
            delete ut;

            LOG((LF_CORDB, LL_INFO10000, "Failed adding unmanaged thread to process!\n"));
            CORDBSetUnrecoverableError(this, hr, 0);
        }
    }
    else
    {
        LOG((LF_CORDB, LL_INFO10000, "New CordbThread failed!\n"));
        CORDBSetUnrecoverableError(this, E_OUTOFMEMORY, 0);
    }

    return ut;
}

//
// Verify that the version info in the control block matches what we expect. The minimum supported protocol from the
// Left Side must be greater or equal to the minimum required protocol of the Right Side. Note: its the Left Side's job
// to conform to whatever protocol the Right Side requires, so long as minimum is supported.
//
HRESULT CordbProcess::VerifyControlBlock(void)
{
    INTERNAL_API_ENTRY(this);

    // Fill in the protocol numbers for the Right Side.
    m_DCB->m_rightSideProtocolCurrent = CorDB_RightSideProtocolCurrent;
    m_DCB->m_rightSideProtocolMinSupported = CorDB_RightSideProtocolMinSupported;

    bool fSkipVerCheck = false;
#if _DEBUG
    // In debug builds, allow us to disable the version check to help with applying hotfixes.
    // The hotfix may be built against a compatible IPC protocol, but have a slightly different build number.
    fSkipVerCheck = REGUTIL::GetConfigDWORD(L"DbgSkipVerCheck", 0) != 0;
#endif

    if (!fSkipVerCheck)
    {
        //
        // These asserts double check that the version of the Right Side matches the version of the left side.
        //
        // If you hit these asserts, it is probably because you rebuilt mscordbi without rebuilding mscorwks, or rebuilt
        // mscorwks without rebuilding mscordbi. You might be able to ignore these asserts, but proceed at your own risk.
        //
        CONSISTENCY_CHECK_MSGF(VER_PRODUCTBUILD == m_DCB->m_verMajor,
            ("version of mscorwks.dll (%d) in the debuggee does not match version of mscordbi.dll (%d) in the debugger.\n"
             "This means your setup is wrong. You can ignore this but procede at your own risk.\n", m_DCB->m_verMajor, VER_PRODUCTBUILD));
        CONSISTENCY_CHECK_MSGF(VER_PRODUCTBUILD_QFE == m_DCB->m_verMinor,
            ("QFE version of mscorwks.dll (%d) in the debuggee does not match QFE version of mscordbi.dll (%d) in the debugger.\n"
             "Both dlls have build # (%d).\n"
             "This means your setup is wrong. You can ignore this but procede at your own risk.\n", m_DCB->m_verMinor, VER_PRODUCTBUILD_QFE, VER_PRODUCTBUILD));
    }

    // These assertions verify that the debug manager is behaving correctly.
    // An assertion failure here means that the runtime version of the debuggee is different from the runtime version of
    // the debugger is capable of debugging.

    // The Debug Manager should properly match LS & RS, and thus guarantee that this assert should never fire.
    // But just in case the installation is corrupted, we'll check it.
    if (m_DCB->m_DCBSize != sizeof(DebuggerIPCControlBlock))
    {
        CONSISTENCY_CHECK_MSGF(false, ("DCB in LS is %d bytes, in RS is %d bytes. Version mismatch!!\n", m_DCB->m_DCBSize, sizeof(DebuggerIPCControlBlock)));
        return CORDBG_E_INCOMPATIBLE_PROTOCOL;
    }

    // The Left Side has to support at least our minimum required protocol.
    if (m_DCB->m_leftSideProtocolCurrent < m_DCB->m_rightSideProtocolMinSupported)
    {
        _ASSERTE(m_DCB->m_leftSideProtocolCurrent >= m_DCB->m_rightSideProtocolMinSupported);
        return CORDBG_E_INCOMPATIBLE_PROTOCOL;
    }

    // The Left Side has to be able to emulate at least our minimum required protocol.
    if (m_DCB->m_leftSideProtocolMinSupported > m_DCB->m_rightSideProtocolCurrent)
    {
        _ASSERTE(m_DCB->m_leftSideProtocolMinSupported <= m_DCB->m_rightSideProtocolCurrent);
        return CORDBG_E_INCOMPATIBLE_PROTOCOL;
    }

#ifdef _DEBUG
    char buf[MAX_PATH];
    DWORD len = GetEnvironmentVariableA("CORDBG_NotCompatibleTest", buf, sizeof(buf));
    _ASSERTE(len < sizeof(buf));

    if (len > 0)
        return CORDBG_E_INCOMPATIBLE_PROTOCOL;
#endif

    if (m_DCB->m_bHostingInFiber == true)
    {
        return CORDBG_E_CANNOT_DEBUG_FIBER_PROCESS;
    }

    //
    // Because we have a DebugManager which will be responsible for matching up the correct version of the Right Side
    // (mscordbi.dll) with the left side, we can just use constants found in this particular mscordbi.dll to determine
    // the version of the left side.
    //
    m_leftSideVersion.dwMajor = 2;
    m_leftSideVersion.dwMinor = 0;
    m_leftSideVersion.dwBuild = m_DCB->m_verMajor;
    m_leftSideVersion.dwSubBuild = m_DCB->m_verMinor;

    m_DCB->m_DebuggerVersion = m_cordb->GetDebuggerVersion();

    if (m_DCB->m_rightSideShouldCreateHelperThread)
    {
        _ASSERTE(!"Helper thread has not been started!");
        return E_FAIL;
    }


    return S_OK;
}

//-----------------------------------------------------------------------------
// This is the CordbProcess objects chance to inspect the DCB and intialize stuff
//-----------------------------------------------------------------------------
HRESULT CordbProcess::GetRuntimeOffsets(void)
{
    INTERNAL_API_ENTRY(this);


    BOOL succ;
    succ = ReadProcessMemoryI(m_handle,
                             m_DCB->m_runtimeOffsets,
                             &m_runtimeOffsets,
                             sizeof(DebuggerIPCRuntimeOffsets),
                             NULL);

    if (!succ)
    {
        return HRESULT_FROM_GetLastError();
    }

    LOG((LF_CORDB, LL_INFO10000, "CP::GRO: got runtime offsets: \n"));

    LOG((LF_CORDB, LL_INFO10000, "    m_TLSIndex=                       0x%08x\n",
         m_runtimeOffsets.m_TLSIndex));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEThreadStateOffset=            0x%08x\n",
         m_runtimeOffsets.m_EEThreadStateOffset));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEThreadStateNCOffset=          0x%08x\n",
         m_runtimeOffsets.m_EEThreadStateNCOffset));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEThreadPGCDisabledOffset=      0x%08x\n",
         m_runtimeOffsets.m_EEThreadPGCDisabledOffset));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEThreadPGCDisabledValue=       0x%08x\n",
         m_runtimeOffsets.m_EEThreadPGCDisabledValue));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEThreadDebuggerWordOffset=     0x%08x\n",
         m_runtimeOffsets.m_EEThreadDebuggerWordOffset));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEThreadFrameOffset=            0x%08x\n",
         m_runtimeOffsets.m_EEThreadFrameOffset));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEThreadMaxNeededSize=          0x%08x\n",
         m_runtimeOffsets.m_EEThreadMaxNeededSize));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEThreadSteppingStateMask=      0x%08x\n",
         m_runtimeOffsets.m_EEThreadSteppingStateMask));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEMaxFrameValue=                0x%08x\n",
         m_runtimeOffsets.m_EEMaxFrameValue));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEThreadDebuggerFilterContextOffset= 0x%08x\n",
         m_runtimeOffsets.m_EEThreadDebuggerFilterContextOffset));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEThreadCantStopOffset=         0x%08x\n",
         m_runtimeOffsets.m_EEThreadCantStopOffset));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEFrameNextOffset=              0x%08x\n",
         m_runtimeOffsets.m_EEFrameNextOffset));
    LOG((LF_CORDB, LL_INFO10000, "    m_EEIsManagedExceptionStateMask=  0x%08x\n",
         m_runtimeOffsets.m_EEIsManagedExceptionStateMask));
    LOG((LF_CORDB, LL_INFO10000, "    m_pPatches=                       0x%08x\n",
         m_runtimeOffsets.m_pPatches));
    LOG((LF_CORDB, LL_INFO10000, "    m_offRgData=                      0x%08x\n",
         m_runtimeOffsets.m_offRgData));
    LOG((LF_CORDB, LL_INFO10000, "    m_offCData=                       0x%08x\n",
         m_runtimeOffsets.m_offCData));
    LOG((LF_CORDB, LL_INFO10000, "    m_cbPatch=                        0x%08x\n",
         m_runtimeOffsets.m_cbPatch));
    LOG((LF_CORDB, LL_INFO10000, "    m_offAddr=                        0x%08x\n",
         m_runtimeOffsets.m_offAddr));
    LOG((LF_CORDB, LL_INFO10000, "    m_offOpcode=                      0x%08x\n",
         m_runtimeOffsets.m_offOpcode));
    LOG((LF_CORDB, LL_INFO10000, "    m_cbOpcode=                       0x%08x\n",
         m_runtimeOffsets.m_cbOpcode));
    LOG((LF_CORDB, LL_INFO10000, "    m_offTraceType=                   0x%08x\n",
         m_runtimeOffsets.m_offTraceType));
    LOG((LF_CORDB, LL_INFO10000, "    m_traceTypeUnmanaged=             0x%08x\n",
         m_runtimeOffsets.m_traceTypeUnmanaged));


    return S_OK;
}


// Get the process from a Fozen state or a Live state to a Synchronized State.
// Note that Process Exit is considered to be synchronized.
// This is a nop if we're not Interop Debugging.
// If this function succeeds, we're in a synchronized state.
HRESULT CordbProcess::StartSyncFromWin32Stop(BOOL *asyncBreakSent)
{
    INTERNAL_API_ENTRY(this);

    HRESULT hr = S_OK;

    // Caller should have taken the stop-go lock. This prevents us from racing w/ a continue.
    _ASSERTE(m_StopGoLock.HasLock());

    // Process should be init before we try to sync it.
    _ASSERTE(this->m_initialized);

    // If nobody's listening for an AsyncBreak, and we're not stopped, then our caller
    // doesn't know if we're sending an AsyncBreak or not; and thus we may not continue.
    // Failing this assert means that we're stopping but we don't think we're going to get a continue
    // down the road, and thus we're headed for a deadlock.
    _ASSERTE((asyncBreakSent != NULL) || (m_stopCount > 0));

    if (asyncBreakSent)
    {
        *asyncBreakSent = FALSE;
    }


    return hr;
}

// Check if the left side has exited. If so, get the right-side
// into shutdown mode. Only use this to avert us from going into
// an unrecoverable error.
bool CordbProcess::CheckIfLSExited()
{
// Check by waiting on the handle with no timeout.
    if (WaitForSingleObject(m_handle, 0) == WAIT_OBJECT_0)
    {
        Lock();
        m_terminated = true;
        m_exiting = true;
        Unlock();
    }

    LOG((LF_CORDB, LL_INFO10, "CP::IsLSExited() returning '%s'\n",
        m_exiting ? "true" : "false"));

    return m_exiting;
}

// Call this if something really bad happened and we can't do
// anything meaningful with the CordbProcess.
void CordbProcess::UnrecoverableError(HRESULT errorHR,
                                      unsigned int errorCode,
                                      const char *errorFile,
                                      unsigned int errorLine)
{
    LOG((LF_CORDB, LL_INFO10, "[%x] CP::UE: unrecoverable error 0x%08x "
         "(%d) %s:%d\n",
         GetCurrentThreadId(),
         errorHR, errorCode, errorFile, errorLine));

    // We definitely want to know about any of these.
    STRESS_LOG3(LF_CORDB, LL_EVERYTHING, "Unrecoverable Error:0x%08x, File=%s, line=%d\n", errorHR, errorFile, errorLine);

#ifdef _DEBUG
    // Ping our error trapping logic
    HRESULT hrDummy;
    hrDummy = ErrWrapper(errorHR);
#endif

    CONSISTENCY_CHECK_MSGF(IsLegalFatalError(errorHR), ("Unrecoverable internal error: hr=0x%08x!", errorHR));

    if (!IsLegalFatalError(errorHR) || (errorHR != CORDBG_E_CANNOT_DEBUG_FIBER_PROCESS))
    {
        // This will throw everything into a Zombie state. The ATT_ macros will check this and fail immediately.
        m_unrecoverableError = true;

        //
        // Mark the process as no longer synchronized.
        //
        Lock();
        SetSynchronized(false);
        IncStopCount();
        Unlock();
    }

    // Set the error flags in the process so that if parts of it are
    // still alive, it will realize that its in this mode and do the
    // right thing.
    if (m_DCB != NULL)
    {
        m_DCB->m_errorHR = errorHR;
        m_DCB->m_errorCode = errorCode;
    }

    //
    // Let the user know that we've hit an unrecoverable error.
    //
    if (m_cordb->m_managedCallback)
    {
        // We are about to send DebuggerError call back. The state of RS is undefined.
        // So we use the special Public Callback. We may be holding locks and stuff.
        // We may also be deeply nested within the RS.
        PUBLIC_CALLBACK_IN_THIS_SCOPE_DEBUGGERERROR(this);
        m_cordb->m_managedCallback->DebuggerError((ICorDebugProcess*) this,
                                                  errorHR,
                                                  errorCode);
    }
}


HRESULT CordbProcess::CheckForUnrecoverableError(void)
{
    HRESULT hr = S_OK;

    if ((m_DCB != NULL) && (m_DCB->m_errorHR != S_OK))
    {
        UnrecoverableError(m_DCB->m_errorHR,
                           m_DCB->m_errorCode,
                           __FILE__, __LINE__);

        hr = m_DCB->m_errorHR;
    }

    return hr;
}


/*
 * EnableLogMessages enables/disables sending of log messages to the
 * debugger for logging.
 */
HRESULT CordbProcess::EnableLogMessages(BOOL fOnOff)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this);
    HRESULT hr = S_OK;

    DebuggerIPCEvent *event = (DebuggerIPCEvent*) _alloca(CorDBIPC_BUFFER_SIZE);
    InitIPCEvent(event, DB_IPCE_ENABLE_LOG_MESSAGES, false, LSPTR_APPDOMAIN::NullPtr());
    event->LogSwitchSettingMessage.iLevel = (int)fOnOff;

    hr = m_cordb->SendIPCEvent(this, event, CorDBIPC_BUFFER_SIZE);
    hr = WORST_HR(hr, event->hr);

    LOG((LF_CORDB, LL_INFO10000, "[%x] CP::EnableLogMessages: EnableLogMessages=%d sent.\n",
         GetCurrentThreadId(), fOnOff));

    return hr;
}

/*
 * ModifyLogSwitch modifies the specified switch's severity level.
 */
COM_METHOD CordbProcess::ModifyLogSwitch(__in_z WCHAR *pLogSwitchName, LONG lLevel)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this);

    HRESULT hr = S_OK;

    _ASSERTE (pLogSwitchName != NULL);

    DebuggerIPCEvent *event = (DebuggerIPCEvent*) _alloca(CorDBIPC_BUFFER_SIZE);
    InitIPCEvent(event, DB_IPCE_MODIFY_LOGSWITCH, false, LSPTR_APPDOMAIN::NullPtr());
    event->LogSwitchSettingMessage.iLevel = lLevel;
    event->LogSwitchSettingMessage.szSwitchName.SetStringTruncate(pLogSwitchName);

    hr = m_cordb->SendIPCEvent(this, event, CorDBIPC_BUFFER_SIZE);
    hr = WORST_HR(hr, event->hr);

    LOG((LF_CORDB, LL_INFO10000, "[%x] CP::ModifyLogSwitch: ModifyLogSwitch sent.\n",
         GetCurrentThreadId()));

    return hr;
}


/*
 * EnumerateAppDomains enumerates all app domains in the process.
 */
HRESULT CordbProcess::EnumerateAppDomains(ICorDebugAppDomainEnum **ppAppDomains)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_EVERETT_HACK_REQUIRE_STOPPED_ALLOW_NONINIT(this);
    VALIDATE_POINTER_TO_OBJECT(ppAppDomains, ICorDebugAppDomainEnum **);

    CordbHashTableEnum *e = new (nothrow) CordbHashTableEnum(this, GetContinueNeuterList(), &m_appDomains,
                                                   IID_ICorDebugAppDomainEnum);
    if (e == NULL)
        return E_OUTOFMEMORY;

    *ppAppDomains = static_cast<ICorDebugAppDomainEnum*> (e);
    e->ExternalAddRef();

    return S_OK;
}

/*
 * GetObject returns the runtime process object.
 * Note: This method is not yet implemented.
 */
HRESULT CordbProcess::GetObject(ICorDebugValue **ppObject)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppObject, ICorDebugObjectValue **);

    return E_NOTIMPL;
}


/*
 * Given a taskid, finding the corresponding thread
 * The function can fail if we do not find any thread with the given taskid
 *
 */
HRESULT CordbProcess::GetThreadForTaskID(TASKID taskid, ICorDebugThread2 **ppThread)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    HRESULT hr = S_OK;
    DebuggerIPCEvent event;

    if (ppThread == NULL)
        return E_INVALIDARG;

    // two way event.
    InitIPCEvent(&event,
                 DB_IPCE_GET_THREAD_FOR_TASKID,
                 true,
                 LSPTR_APPDOMAIN::NullPtr());
    event.GetThreadForTaskId.taskid = taskid;

    hr = m_cordb->SendIPCEvent(this, &event, sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event.hr);

    // Stop now if we can't even send the event.
    if (!SUCCEEDED(hr))
        goto exit;

    _ASSERTE(event.type == DB_IPCE_GET_THREAD_FOR_TASKID_RESULT);

    // now find the ICorDebugThread corresponding to it
    CordbThread *t;
    HASHFIND    find;

    {
        LSPTR_THREAD debuggerThreadToken = event.GetThreadForTaskIdResult.debuggerThreadToken;
        for (t  = m_userThreads.FindFirst(&find);
             t != NULL;
             t  = m_userThreads.FindNext(&find))
        {
            // The fiber cookie is really a ptr to the EE's Thread object,
            // which is exactly what out m_debuggerThreadToken is.
            if (t->m_debuggerThreadToken == debuggerThreadToken)
                break;
        }
    }

    if (t == NULL)
    {
        *ppThread = NULL;
        return S_FALSE;
    }
    else
    {
        *ppThread = static_cast<ICorDebugThread2*> (t);
        t->ExternalAddRef();
    }
exit:
    return hr;
}   // CordbProcess::GetThreadForTaskid

HRESULT
CordbProcess::GetVersion(COR_VERSION* pVersion)
{
    if (NULL == pVersion)
    {
        return E_INVALIDARG;
    }

    *pVersion = m_leftSideVersion;
    return S_OK;
}

//-----------------------------------------------------------------------------
// Search for a native patch given the address. Return null if not found.
// Since we return an address, this is only valid until the table is disturbed.
//-----------------------------------------------------------------------------
NativePatch * CordbProcess::GetNativePatch(const void * pAddress)
{
    _ASSERTE(ThreadHoldsProcessLock());
    
    int cTotal = m_NativePatchList.Count();
    NativePatch * pTable = m_NativePatchList.Table();
    if (pTable == NULL)
    {
        return NULL;
    }

    for(int i = 0; i  < cTotal; i++)
    {
        if (pTable[i].pAddress == pAddress)
        {
            return &pTable[i];
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------
// Is there an break-opcode (int3 on x86) at the address in the debuggee?
//-----------------------------------------------------------------------------
bool CordbProcess::IsBreakOpcodeAtAddress(const void * address)
{
    BOOL succ;
    // There should have been an int3 there already. Since we already put it in there,
    // we should be able to safely read it out.
    BYTE opcodeTest = 0;
    SIZE_T cbRead;
    succ = ReadProcessMemoryI(m_handle, CORDB_ADDRESS_TO_PTR(address), &opcodeTest, 1, &cbRead);
    
    _ASSERTE(succ);
    _ASSERTE(cbRead == 1);

    return (opcodeTest == CORDbg_BREAK_INSTRUCTION);
}


//-----------------------------------------------------------------------------
// CordbProcess::SetUnmanagedBreakpoint
// Called by a native debugger to add breakpoints during Interop.
// address - remote address into the debuggee
// bufsize, buffer[] - initial size & buffer for the opcode that we're replacing.
// buflen - size of the buffer that we write to.
//-----------------------------------------------------------------------------
HRESULT
CordbProcess::SetUnmanagedBreakpoint(CORDB_ADDRESS address, ULONG32 bufsize, BYTE buffer[], ULONG32 * bufLen)
{
    LOG((LF_CORDB, LL_INFO100, "CP::SetUnBP: pProcess=%x, address=%p.\n", this, CORDB_ADDRESS_TO_PTR(address)));

    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    FAIL_IF_MANAGED_ONLY(this);
    _ASSERTE(!ThreadHoldsProcessLock());

    HRESULT hr = S_OK;

    Lock();

    NativePatch * p = NULL;
#if defined(_X86_)
    const BYTE patch = CORDbg_BREAK_INSTRUCTION;
    BYTE opcode;

    // Make sure args are good
    if ((buffer == NULL) || (bufsize < sizeof(patch)) || (bufLen == NULL))
    {
        hr = E_INVALIDARG;
        goto ErrExit;
    }

    // Fail if there's already a patch at this address.
    if (GetNativePatch(CORDB_ADDRESS_TO_PTR(address)) != NULL)
    {
        hr = CORDBG_E_NATIVE_PATCH_ALREADY_AT_ADDR;
        goto ErrExit;
    }

    // Preallocate this now so that if are oom, we can fail before we get half-way through.
    p = m_NativePatchList.Append();
    if (p == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto ErrExit;
    }


    // Read out opcode. 1 byte on x86

    hr = ApplyRemotePatch(m_handle, CORDB_ADDRESS_TO_PTR(address), &p->opcode);
    if (FAILED(hr))
        goto ErrExit;

    // It's all successful, so now update our out-params & internal bookkeaping.
    opcode = (BYTE) p->opcode;
    buffer[0] = opcode;
    *bufLen = sizeof(opcode);

    p->pAddress = CORDB_ADDRESS_TO_PTR(address);
    p->opcode = opcode;

    _ASSERTE(SUCCEEDED(hr));
#else
    hr =  E_NOTIMPL;
    goto ErrExit;
#endif // _X86_


ErrExit:
    // If we failed, then free the patch
    if (FAILED(hr) && (p != NULL))
    {
        m_NativePatchList.Delete(*p);
    }

    Unlock();
    return hr;
}


//-----------------------------------------------------------------------------
// CordbProcess::ClearUnmanagedBreakpoint
// Called by a native debugger to remove breakpoints during Interop.
// The patch is deleted even if the function fails.
//-----------------------------------------------------------------------------
HRESULT
CordbProcess::ClearUnmanagedBreakpoint(CORDB_ADDRESS address)
{
    LOG((LF_CORDB, LL_INFO100, "CP::ClearUnBP: pProcess=%x, address=%p.\n", this, CORDB_ADDRESS_TO_PTR(address)));

    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    FAIL_IF_MANAGED_ONLY(this);
    
    _ASSERTE(!ThreadHoldsProcessLock());

    HRESULT hr = S_OK;
    PRD_TYPE opcode;

    Lock();

    // Make sure this is a valid patch.
    int cTotal = m_NativePatchList.Count();
    NativePatch * pTable = m_NativePatchList.Table();
    if (pTable == NULL)
    {
        hr = CORDBG_E_NO_NATIVE_PATCH_AT_ADDR;
        goto ErrExit;
    }

    int i;
    for(i = 0; i  < cTotal; i++)
    {
        if (pTable[i].pAddress == CORDB_ADDRESS_TO_PTR(address))
            break;
    }

    if (i >= cTotal)
    {
        hr = CORDBG_E_NO_NATIVE_PATCH_AT_ADDR;
        goto ErrExit;
    }

    // Found it! Remove it from our table. Note that this may shuffle table contents
    // around, so don't keep pointers into the table.
    opcode = pTable[i].opcode;

    m_NativePatchList.Delete(pTable[i]);
    _ASSERTE(m_NativePatchList.Count() == cTotal - 1);

    // Now remove the patch.



    // Just call through to Write ProcessMemory
    hr = RemoveRemotePatch(m_handle, CORDB_ADDRESS_TO_PTR(address), opcode);
    if (FAILED(hr))
        goto ErrExit;


    // Our internal bookeaping was already updated to remove the patch, so now we're done.
    // If we had a failure, we should have already bailed.
    _ASSERTE(SUCCEEDED(hr));

ErrExit:
    Unlock();
    return hr;
}


HRESULT CordbProcess::Attach(ULONG AppDomainId)
{
    INTERNAL_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    HRESULT hr = S_OK;

    DebuggerIPCEvent *event = (DebuggerIPCEvent*) _alloca(CorDBIPC_BUFFER_SIZE);
    InitIPCEvent(event, DB_IPCE_ATTACH_TO_APP_DOMAIN, false, LSPTR_APPDOMAIN::NullPtr());
    event->AppDomainData.id = AppDomainId;
    event->appDomainToken.Set(NULL);

    hr = m_cordb->SendIPCEvent(this, event, CorDBIPC_BUFFER_SIZE);
    hr = WORST_HR(hr, event->hr);

    LOG((LF_CORDB, LL_INFO100, "[%x] CP::Attach: pProcess=%x sent.\n",
         GetCurrentThreadId(), this));

    return hr;
}

//------------------------------------------------------------------------------------
// StopCount, Sync, SyncReceived form our stop-status. This status is super-critical
// to most hangs, so we stress log it.
//------------------------------------------------------------------------------------
void CordbProcess::SetSynchronized(bool fSynch)
{
    _ASSERTE(ThreadHoldsProcessLock() || !"Must have process lock to toggle SyncStatus");
    STRESS_LOG1(LF_CORDB, LL_INFO1000, "CP:: set sync=%d\n", fSynch);
    m_synchronized = fSynch;
}

bool CordbProcess::GetSynchronized(void)
{
    // This can be accessed whether we're Locked or not. This means that the result
    // may change underneath us.
    return m_synchronized;
}

void CordbProcess::IncStopCount()
{
    _ASSERTE(ThreadHoldsProcessLock());
    m_stopCount++;
    STRESS_LOG1(LF_CORDB, LL_INFO1000, "CP:: Inc StopCount=%d\n", m_stopCount);
}
void CordbProcess::DecStopCount()
{
    // We can inc w/ just the process lock (b/c we can dispatch events from the W32ET)
    // But decrementing (eg, Continue), requires the stop-go lock.
    // This if an operation takes the SG lock, it ensures we don't continue from underneath it.
    ASSERT_SINGLE_THREAD_ONLY(HoldsLock(&m_StopGoLock));
    _ASSERTE(ThreadHoldsProcessLock());

    m_stopCount--;
    STRESS_LOG1(LF_CORDB, LL_INFO1000, "CP:: Dec StopCount=%d\n", m_stopCount);
}

// Just gets whether we're stopped or not (m_stopped > 0).
// You only need the StopGo lock for this.
bool CordbProcess::IsStopped()
{
    // We don't require the process-lock, just the SG-lock.
    // Holding the SG lock prevents another thread from continuing underneath you.
    // (see DecStopCount()).
    // But you could still be running free, and have another thread stop-underneath you.
    // Thus IsStopped() leans towards returning false.
    ASSERT_SINGLE_THREAD_ONLY(HoldsLock(&m_StopGoLock));

    return (m_stopCount > 0);
}

int CordbProcess::GetStopCount()
{
    _ASSERTE(ThreadHoldsProcessLock());
    return m_stopCount;
}

bool CordbProcess::GetSyncCompleteRecv()
{
    _ASSERTE(ThreadHoldsProcessLock());
    return m_syncCompleteReceived;
}

void CordbProcess::SetSyncCompleteRecv(bool fSyncRecv)
{
    _ASSERTE(ThreadHoldsProcessLock());
    STRESS_LOG1(LF_CORDB, LL_INFO1000, "CP:: set syncRecv=%d\n", fSyncRecv);
    m_syncCompleteReceived = fSyncRecv;
}

// This can be used if we ever need the RS to emulate old behavior of previous versions.
// This can not be used in QIs to deny queries for new interfaces.
// QIs must be consistent across the lifetime of an object. Say CordbThread used this in a QI 
// do deny returning a ICorDebugThread2 interface when emulating v1.1. Once that Thread is neutered,
// it no longer has a pointer to the process, and it no longer knows if it should be denying
// the v2.0 query. An object's QI can't start returning new interfaces onces its neutered.
bool CordbProcess::SupportsVersion(CorDebugInterfaceVersion featureVersion)
{
    _ASSERTE(featureVersion == CorDebugVersion_2_0);
    return true;
}


// This list will get actively swept (looking for objects w/ external ref = 0) between continues.
void CordbProcess::AddToNeuterOnExitList(CordbBase *pObject)
{
    INTERNAL_API_ENTRY(this);
    _ASSERTE(pObject != NULL);
    this->m_ExitNeuterList.Add(pObject);
}

// Mark that this object should be neutered the next time we Continue the process.
void CordbProcess::AddToNeuterOnContinueList(CordbBase *pObject)
{
    INTERNAL_API_ENTRY(this);
    _ASSERTE(pObject != NULL);

    m_ContinueNeuterList.Add(pObject);
}


/* ------------------------------------------------------------------------- *
 * Runtime Controller Event Thread class
 * ------------------------------------------------------------------------- */

//
// Constructor
//
CordbRCEventThread::CordbRCEventThread(Cordb* cordb)
{
    _ASSERTE(cordb != NULL);

    m_cordb.Assign(cordb);
    m_thread = NULL;
    m_threadId = 0;
    m_run = TRUE;
    m_threadControlEvent = NULL;
    m_processStateChanged = FALSE;

    g_pRSDebuggingInfo->m_RCET = this;
}


//
// Destructor. Cleans up all of the open handles and such.
// This expects that the thread has been stopped and has terminated
// before being called.
//
CordbRCEventThread::~CordbRCEventThread()
{
    if (m_threadControlEvent != NULL)
        CloseHandle(m_threadControlEvent);

    if (m_thread != NULL)
        CloseHandle(m_thread);

    g_pRSDebuggingInfo->m_RCET = NULL;
}

//
// Init sets up all the objects that the thread will need to run.
//
HRESULT CordbRCEventThread::Init(void)
{
    if (m_cordb == NULL)
        return E_INVALIDARG;

    m_threadControlEvent = WszCreateEvent(NULL, FALSE, FALSE, NULL);

    if (m_threadControlEvent == NULL)
        return HRESULT_FROM_GetLastError();

    return S_OK;
}


//
// HandleFirstRCEvent -- called to handle the very first IPC event from
// the runtime controller. The first event is special because at that point
// we don't know the handles for RSEA and RSER if we launched the process.
//
HRESULT CordbRCEventThread::HandleFirstRCEvent(CordbProcess* process)
{
    HRESULT hr = S_OK;

    BOOL succ;

    LOG((LF_CORDB, LL_INFO1000, "[%x] RCET::HFRCE: first event...\n", GetCurrentThreadId()));

    if (!(process->m_IPCReader.IsPrivateBlockOpen()))
    {
        // Open the shared memory segment which contains the control block.

        hr = process->m_IPCReader.OpenPrivateBlockOnPid((DWORD)process->m_id);
        // If we're trying to debug across platforms, opening the IPC block would have
        // failed with hr= CORDBG_E_UNCOMPATIBLE_PLATFORMS

        if (hr == CORDBG_E_UNCOMPATIBLE_PLATFORMS)
        {
            // Send up the DebuggerError event
            process->UnrecoverableError(hr, 0, NULL, 0);

            // Kill the process...
            TerminateProcess(process->m_handle, hr);

            return hr;
        }


        if (!SUCCEEDED(hr))
        {
            STRESS_LOG1(LF_CORDB, LL_INFO1000, "HFCR: 1 hr=0x%08x\n", hr);
            goto exit;
        }
#ifdef _DEBUG
        if(!process->m_IPCReader.IsValid())
        {
            LOG((LF_CORDB, LL_EVERYTHING, "IPCReader not valid!\n"));
        }
#endif
        process->m_DCB = process->m_IPCReader.GetDebugBlock();

        if (process->m_DCB == NULL)
        {
            hr = ERROR_FILE_NOT_FOUND;
            STRESS_LOG1(LF_CORDB, LL_INFO1000, "HFCR: 2 hr=0x%08x\n", hr);
            goto exit;
        }
    }
    if ((process->m_DCB != NULL) && (process->m_DCB->m_rightSideProtocolCurrent == 0))
    {
        // Verify that the control block is valid. This is the create case. If we fail validation, then we need to send
        // up a DebuggerError with an hr of CORDBG_E_INCOMPATIBLE_PROTOCOL and kill the process.
        // Or if process is hosted in fiber mode, we should not allow the launch to succeed
        //
        hr = process->VerifyControlBlock();

        if (FAILED(hr))
        {
            _ASSERTE(hr == CORDBG_E_INCOMPATIBLE_PROTOCOL || hr == CORDBG_E_CANNOT_DEBUG_FIBER_PROCESS);

            // Send up the DebuggerError event
            process->UnrecoverableError(hr, 0, NULL, 0);

            // Kill the process...
            TerminateProcess(process->m_handle, hr);

            return hr;
        }
    }

    // Dup our own process handle into the remote process.
    LOG((LF_CORDB, LL_EVERYTHING, "Size of CdbP is %d\n", sizeof(CordbProcess)));

    succ = process->m_DCB->m_rightSideProcessHandle.DuplicateToRemoteProcess(process->m_handle, GetCurrentProcess());
    if (!succ)
    {
        LOG((LF_CORDB, LL_EVERYTHING, "DuplicateHandle Failed: %d\n", GetLastError()));
        STRESS_LOG0(LF_CORDB, LL_INFO1000, "HFCR: 3 DuplicateHandle\n");
        goto exit;
    }
    LOG((LF_CORDB, LL_EVERYTHING, "process lives at: %p\n", process));

    // Dup RSEA and RSER into this process if we don't already have them.
    // On Launch, we don't have them yet, but on attach we do.
    if (process->m_rightSideEventAvailable == NULL)
    {
        succ = process->m_DCB->m_rightSideEventAvailable.DuplicateToLocalProcess(
            process->m_handle, &process->m_rightSideEventAvailable);
        if (!succ)
        {
            STRESS_LOG0(LF_CORDB, LL_INFO1000, "HFCR: 4 RSEA\n");
            goto exit;
        }
    }

    if (process->m_rightSideEventRead == NULL)
    {
        succ = process->m_DCB->m_rightSideEventRead.DuplicateToLocalProcess(
            process->m_handle, &process->m_rightSideEventRead);
        if (!succ)
        {
            STRESS_LOG0(LF_CORDB, LL_INFO1000, "HFCR: 5 RSER\n");
            goto exit;
        }
    }

    if (process->m_leftSideUnmanagedWaitEvent == NULL)
    {
        succ = process->m_DCB->m_leftSideUnmanagedWaitEvent.DuplicateToLocalProcess(
            process->m_handle, &process->m_leftSideUnmanagedWaitEvent);
        if (!succ)
        {
            STRESS_LOG0(LF_CORDB, LL_INFO1000, "HFCR: 6 LSUWE\n");
            goto exit;
        }
    }

    if (process->m_syncThreadIsLockFree == NULL)
    {
        succ = process->m_DCB->m_syncThreadIsLockFree.DuplicateToLocalProcess(
            process->m_handle, &process->m_syncThreadIsLockFree);
        if (!succ)
        {
            STRESS_LOG0(LF_CORDB, LL_INFO1000, "HFCR: 7 TILF\n");
            goto exit;
        }
    }

    // Read the Runtime Offsets struct out of the debuggee.
    hr = process->GetRuntimeOffsets();
    if (SUCCEEDED(hr))
    {
        process->m_initialized = true;

        process->m_DCB->m_rightSideIsWin32Debugger =
            (process->m_state & process->PS_WIN32_ATTACHED) ? true : false;

        LOG((LF_CORDB, LL_INFO1000, "[%x] RCET::HFRCE: ...went fine\n", GetCurrentThreadId()));

        return hr;
    }

    // Fall-through case, if we passed, we should have failed by now, so we've got an error.
    STRESS_LOG0(LF_CORDB, LL_INFO1000, "HFCR: 8 \n");

exit:
    // We only land here on failure cases.
    // We must have jumped to this label. Maybe we didn't set HR, so check now.
    if (SUCCEEDED(hr))
    {
        if (GetLastError() == 0)
        {
            STRESS_LOG0(LF_CORDB, LL_INFO1000, "HFCR: 8b \n");
            hr = E_FAIL;
        } else {
            hr = HRESULT_FROM_GetLastError();
        }
    }
    STRESS_LOG1(LF_CORDB, LL_INFO1000, "HFCR: 9 hr=0x%08x\n", hr);
    _ASSERTE(!SUCCEEDED(hr));

    if (process->m_rightSideEventAvailable != NULL)
    {
        CloseHandle(process->m_rightSideEventAvailable);
        process->m_rightSideEventAvailable = NULL;
    }

    if (process->m_rightSideEventRead != NULL)
    {
        CloseHandle(process->m_rightSideEventRead);
        process->m_rightSideEventRead = NULL;

    }

    if (process->m_leftSideUnmanagedWaitEvent != NULL)
    {
        CloseHandle(process->m_leftSideUnmanagedWaitEvent);
        process->m_leftSideUnmanagedWaitEvent = NULL;
    }

    if (process->m_syncThreadIsLockFree != NULL)
    {
        CloseHandle(process->m_syncThreadIsLockFree);
        process->m_syncThreadIsLockFree = NULL;
    }

    process->m_DCB = NULL;
    if (process->m_IPCReader.IsPrivateBlockOpen())
    {
        process->m_IPCReader.ClosePrivateBlock();
    }
    LOG((LF_CORDB, LL_INFO1000, "[%x] RCET::HFRCE: ...failed\n",
             GetCurrentThreadId()));

    CORDBSetUnrecoverableError(process, hr, 0);
    return hr;
}


//
// ReadRCEvent -- read an IPC event sent by the runtime controller from the
// remote process.
//
HRESULT CordbRCEventThread::ReadRCEvent(CordbProcess* process,
                                        DebuggerIPCEvent* event)
{
    _ASSERTE(event != NULL);

    CopyRCEvent((BYTE*)process->m_DCB->m_sendBuffer, (BYTE*)event);

    // Do a pre-processing on the event
    switch (event->type & DB_IPCE_TYPE_MASK)
    {
        case DB_IPCE_UPDATE_MODULE_SYMS:
        {
            HRESULT hr= S_OK;
            hr = event->UpdateModuleSymsData.dataBuffer.CopyLSDataToRS(process->m_handle);

            _ASSERTE(SUCCEEDED(hr));
            if (FAILED(hr))
            {
                STRESS_LOG0(LF_CORDB, LL_INFO1000,
                     "[%x] RCET::RRCE: UpdateModuleSyms preprocessing failed\n");

                // we don't need to fail out from here. The DispatchEvent's will check
                // on m_pbRS and fail out
            }
            break;
        }

        case DB_IPCE_MDA_NOTIFICATION:
        {
            event->MDANotification.szName.CopyLSDataToRS(process->m_handle);
            event->MDANotification.szDescription.CopyLSDataToRS(process->m_handle);
            event->MDANotification.szXml.CopyLSDataToRS(process->m_handle);
            break;
        }

        case DB_IPCE_FIRST_LOG_MESSAGE:
        {
            event->FirstLogMessage.szContent.CopyLSDataToRS(process->m_handle);
            break;
        }

        default:
            break;
    }

    return event->hr;
}

void inline CordbRCEventThread::CopyRCEvent(BYTE *src,
                                            BYTE *dst)
{
    memcpy(dst, src, CorDBIPC_BUFFER_SIZE);
}

// Return true if this is the RCEvent thread, else false.
bool CordbRCEventThread::IsRCEventThread(void)
{
    return (m_threadId == GetCurrentThreadId());
}

//
// SendIPCEvent -- send an IPC event to the runtime controller. All this
// really does is copy the event into the process's send buffer and sets
// the RSEA then waits on RSER.
//
// Note: when sending a two-way event (replyRequired = true), the
// eventSize must be large enough for both the event sent and the
// result event.
//
// Returns whether the event was sent successfully. This is different than event->eventHr.
//
HRESULT CordbRCEventThread::SendIPCEvent(CordbProcess* process,
                                         DebuggerIPCEvent* event,
                                         SIZE_T eventSize)
{

    _ASSERTE(process != NULL);
    _ASSERTE(event != NULL);
#ifdef _DEBUG
        // We need to be synchronized whenever we're sending an IPC Event.
        // This may require our callers' using a Stop-Continue holder.
        // Attach + AsyncBreak are the only (obvious) exceptions.
        // For continue, we set Sync-Status to false before sending, so we exclude that too.
        // Everybody else should only be sending events when synced. We should never ever ever
        // send an event from a CorbXYZ dtor (b/c that would be called at any random time). Instead,
        // use a NeuterList.
        switch (event->type)
        {
        case DB_IPCE_ATTACHING:
        case DB_IPCE_ASYNC_BREAK:
        case DB_IPCE_CONTINUE:
            break;

        default:
            CONSISTENCY_CHECK_MSGF(process->GetSynchronized(), ("Must by synced while sending IPC event: %s (0x%x)",
                IPCENames::GetName(event->type), event->type));
        }
#endif


    LOG((LF_CORDB, LL_EVERYTHING, "SendIPCEvent in CordbRCEventThread called\n"));

    // For simplicity sake, we have the following conservative invariants when sending IPC events:
    // - Always hold the Stop-Go lock.
    // - never on the W32ET.
    // - Never hold the Process-lock (this allows the w32et to take that lock to pump)

    // Must have the stop-go lock to send an IPC event.
    CONSISTENCY_CHECK_MSGF(process->GetStopGoLock()->HasLock(), ("Must have stop-go lock to send event. proc=%p, event=%s",
        process, IPCENames::GetName(event->type)));

    // The w32 ET will need to take the process lock. So if we're holding it here, then we'll
    // deadlock (since W32 ET is blocked on lock, which we would hold; and we're blocked on W32 ET
    // to keep pumping.
    _ASSERTE(!process->ThreadHoldsProcessLock() || !"Can't hold P-lock while sending blocking IPC event");


    // Can't be on the w32 ET, or we can't be pumping.
    // Although we can trickle in here from public APIs, our caller should have validated
    // that we weren't on the w32et, so the assert here is justified. But just in case there's something we missed,
    // we have a runtime check (as a final backstop against a deadlock).
    _ASSERTE(!process->IsWin32EventThread());
    CORDBFailIfOnWin32EventThread(process);


    // If this is an async event, then we expect it to be sent while the process is locked.
    if (event->asyncSend)
    {
        // This may be on the w32et, so we can't hold the stop-go lock.
        _ASSERTE(event->type == DB_IPCE_ATTACHING); // only async event should be attaching.
    }


    // This will catch us if we've detached or exited.
    // Note if we exited, then we should have been neutered and so shouldn't even be sending an IPC event,
    // but just in case, we'll check.
    CORDBRequireProcessStateOK(process);


#ifdef _DEBUG
    // We should never send an Async Break on the RCET. This will deadlock.
    // - if we're on the RCET, we should be stopped, and thus Stop() should just bump up a stop count,
    //   and not actually send an AsyncBreak.
    // - Delayed-Continues help enforce this.
    // This is a special case of the deadlock check below.
    if (IsRCEventThread())
    {
        _ASSERTE(event->type != DB_IPCE_ASYNC_BREAK);
    }
#endif

#ifdef _DEBUG
    // This assert protects us against a deadlock.
    // 1) (RCET) blocked on (This function): If we're on the RCET, then the RCET is blocked until we return (duh).
    // 2) (LS) blocked on (RCET): If the LS is not synchronized, then it may be sending an event to the RCET, and thus blocked on the RCET.
    // 3) (Helper thread) blocked on (LS): That LS thread may be holding a lock that the helper thread needs, thus blocking the helper thread.
    // 4) (This function) blocked on (Helper Thread): We block until the helper thread can process our IPC event.
    //     #4 is not true for async events.
    //
    // If we hit this assert, it means we may get the deadlock above and we're calling SendIPCEvent at a time we shouldn't.
    // Note this race is as old as dirt.
    if (IsRCEventThread() && !event->asyncSend)
    {
        // Note that w/ Continue & Attach, GetSynchronized() has a different meaning and the race above won't happen.
        BOOL fPossibleDeadlock = process->GetSynchronized() || (event->type == DB_IPCE_CONTINUE) || (event->type == DB_IPCE_ATTACHING);
        CONSISTENCY_CHECK_MSGF(fPossibleDeadlock, ("Possible deadlock while sending: '%s'\n", IPCENames::GetName(event->type)));
    }
#endif



    // Cache this process into the MRU so that we can find it if we're debugging in retail.
    g_pRSDebuggingInfo->m_MRUprocess = process;

    HRESULT hr = S_OK;
    HRESULT hrEvent = S_OK;
    BOOL succ = TRUE;

    _ASSERTE(event != NULL);

    // NOTE: the eventSize parameter is only so you can specify an event size that is SMALLER than the process send
    // buffer size!!
    if (eventSize > CorDBIPC_BUFFER_SIZE)
        return E_INVALIDARG;

    STRESS_LOG4(LF_CORDB, LL_INFO1000, "CRCET::SIPCE: sending %s to AD 0x%x, proc 0x%x(%d)\n",
         IPCENames::GetName(event->type), LsPtrToCookie(event->appDomainToken), process->m_id, process->m_id);

    // For 2-way events, this check is unnecessary (since we already check for LS exit)
    // But for async events, we need this.
    // So just check it up here and make everyone's life easier.
    if (process->m_terminated)
    {
        STRESS_LOG0(LF_CORDB, LL_INFO10000, "CRCET::SIPCE: LS already terminated, shortcut exiting\n");
        hr = CORDBG_E_PROCESS_TERMINATED;
        goto exit;
    }

    // If the helper thread has died, we can't send an IPC event (and it's never coming back either). 
    // Although we do wait on the thread's handle, there are strange windows where the thread's handle
    // is not yet signaled even though we've continued from the exit-thread event for the helper.
    if (process->m_helperThreadDead)
    {
        STRESS_LOG0(LF_CORDB, LL_INFO10000, "CRCET::SIPCE: Helper-thread dead, shortcut exiting\n");
        hr = CORDBG_E_PROCESS_TERMINATED;
        goto exit;
    }




    // Copy the event into the shared memory segment.
    memcpy(process->m_DCB->m_receiveBuffer, event, eventSize);

    // Do some safety-checks for sending an Async-Event.
    #ifdef _DEBUG
    {
        // We can only send 1 event from RS-->LS at a time.
        // For non-async events, this is obviously enforced. (since the events are blocking & serialized)
        // If this is an AsyncSend, then our caller was responsible for making sure it
        // was safe to send.
        // There should be no other IPC event in the pipeline. This, both RSEA & RSER
        // should be non-signaled. check that now.
        // It's ok if these fail - we detect that below.
        int res2 = ::WaitForSingleObject(process->m_rightSideEventAvailable, 0);
        CONSISTENCY_CHECK_MSGF(res2 != WAIT_OBJECT_0, ("RSEA:%d", res2));

        int res3 = ::WaitForSingleObject(process->m_rightSideEventRead, 0);
        CONSISTENCY_CHECK_MSGF(res3 != WAIT_OBJECT_0, ("RSER:%d", res3));
    }

    #endif


    // Tell the runtime controller there is an event ready.
    STRESS_LOG0(LF_CORDB,LL_INFO1000, "Set RSEA\n");
    succ = SetEvent(process->m_rightSideEventAvailable);

    // If we're sending a Continue() event, then after this, the LS may run free.
    // If this is the last managed event before the LS exits, (which is the case
    // if we're responding to either an Exit-Thread or if we respond to a Detach)
    // the LS may exit at anytime from here on, so we need to be careful.


    if (succ)
    {
        STRESS_LOG0(LF_CORDB, LL_INFO1000, "CRCET::SIPCE: sent...\n");

        // If this is an async send, then don't wait for the left side to acknowledge that its read the event.
        _ASSERTE(!event->asyncSend || !event->replyRequired);

        if (!event->asyncSend)
        {
            DWORD ret;

            STRESS_LOG0(LF_CORDB, LL_INFO1000,"CRCET::SIPCE: waiting for left side to read event. (on RSER)\n");

            // Wait for either a reply (common case) or the left side to go away.
            // We can't detach while waiting for a reply (because detach needs to send events).
            // All of the outcomes from this wait are completely disjoint.
            // It's possible for the LS to reply and then exit normally (Thread_Detach, Process_Detach)
            // and so ExitProcess may have been called, but it doesn't matter.

            enum {
                ID_RSER = WAIT_OBJECT_0,
                ID_LSPROCESS,
                ID_HELPERTHREAD,
            };

            HANDLE hLSProcess = NULL;
            hr = process->GetHandle(&hLSProcess);

            // We take locks to ensure that the CordbProcess object is still alive,
            // even if the OS process exited.
            _ASSERTE(SUCCEEDED(hr));

            // Only wait on the helper thread for cases where the process is stopped (and thus we don't expect it do exit on us).
            // If the process is running and we lose our helper thread, it ought to be during shutdown and we ough to
            // follow up with an exit.
            // This includes when we've dispatch Native events, and it includes the AsyncBreak sent to get us from a
            // win32 frozen state to a synchronized state).
            HANDLE hHelperThread = NULL;
            if (process->IsStopped())
            {
                hHelperThread = process->GetHelperThreadHandle();
            }


            // Note that in case of a tie (multiple handles signaled), WaitForMultipleObjects gives
            // priority to the handle earlier in the array.
            HANDLE waitSet[] = { process->m_rightSideEventRead, hLSProcess, hHelperThread};
            DWORD cWaitSet = NumItems(waitSet);
            if (hHelperThread == NULL)
            {
                cWaitSet--;
            }

            do
            {
                ret = WaitForMultipleObjects(cWaitSet, waitSet, FALSE, CordbGetWaitTimeout());
                // If we timeout because we're waiting for an uncontinued OOB event, we need to just keep waiting.
            } while ((ret == WAIT_TIMEOUT) && process->IsWaitingForOOBEvent());

            switch(ret)
            {
            case ID_RSER:
                // Normal reply from LS.
                // This is set iff the LS replied to our event. The LS may have exited since it replied
                // but we don't care. We still have the reply and we'll pass it on.
                STRESS_LOG0(LF_CORDB, LL_INFO1000, "CRCET::SIPCE: left side read the event.\n");

                // If this was a two-way event, then the result is already ready for us. Simply copy the result back
                // over the original event that was sent. Otherwise, the left side has simply read the event and is
                // processing it...
                if (event->replyRequired)
                {
                    memcpy(event, process->m_DCB->m_receiveBuffer, eventSize);
                    hrEvent = event->hr;
                }
                break;

            case ID_LSPROCESS:
                // Left side exited on us.
                // ExitProcess may or may not have been called here (since it's on a different thread).
                STRESS_LOG0(LF_CORDB, LL_INFO1000, "CRCET::SIPCE: left side exiting while RS was waiting for reply.\n");
                hr = CORDBG_E_PROCESS_TERMINATED;
                break;

            case ID_HELPERTHREAD:
                // We can only send most IPC events while the LS is synchronized. We shouldn't lose our helper thread
                // when synced under any sort of normal conditions.
                // This won't fire if the process already exited, because LSPROCESS gets higher priority in the wait
                // (since it was placed earlier).
                // Thus the only "legitimate" window where this could happen would be in a shutdown scenario after
                // the helper is dead but before the process has died. We shouldn't be synced in that scenario,
                // so we shouldn't be sending IPC events during it.
                STRESS_LOG0(LF_CORDB, LL_INFO1000, "CRCET::SIPCE: lost helper thread.\n");


                // Assert because we want to know if we ever actually hit this in any detectable scenario.
                // However, shutdown can occur in preemptive mode. Thus if the RS does an AsyncBreak late
                // enough, then the LS will appear to be stopped but may still shutdown.
                // Since the debuggee can exit asynchronously at any time (eg, suppose somebody forcefully
                // kills it with taskman), this doesn't introduce a new case.
                // That aside, it would be great to be able to assert this:
                //_ASSERTE(!"Potential deadlock - Randomly Lost helper thread");

                // We'll piggy back this on the terminated case.
                hr = CORDBG_E_PROCESS_TERMINATED;
                break;

            default:
                {
                    // If we timed out/failed, check the left side to see if it is in the unrecoverable error mode. If it is,
                    // return the HR from the left side that caused the error.  Otherwise, return that we timed out and that
                    // we don't really know why.
                    HRESULT realHR = (ret == WAIT_FAILED) ? HRESULT_FROM_GetLastError() : ErrWrapper(CORDBG_E_TIMEOUT);

                    hr = process->CheckForUnrecoverableError();

                    if (hr == S_OK)
                    {
                        CORDBSetUnrecoverableError(process, realHR, 0);
                        hr = realHR;
                    }

                    STRESS_LOG1(LF_CORDB, LL_INFO1000, "CRCET::SIPCE: left side timeout/fail while RS waiting for reply. hr = 0x%08x\n", hr);
                }
                break;
            }

            // If the LS picked up RSEA, it will be reset (since it's an auto event).
            // But in the case that the wait failed or  that the LS exited, we need to explicitly reset RSEA
            if (hr != S_OK)
            {
                ResetEvent(process->m_rightSideEventAvailable);
            }

            // Done waiting for reply.
        }
    }
    else
    {
        hr = HRESULT_FROM_GetLastError();
        CORDBSetUnrecoverableError(process, hr, 0);
    }

exit:
    // The hr and hrEvent are 2 very different things.
    // hr tells us whether the event was sent successfully.
    // hrEvent tells us how the LS responded to it.
    // if FAILED(hr), then hrEvent is useless b/c the LS never got it.
    // But if SUCCEEDED(hr), then hrEvent may still have failed and that could be
    // valuable information.

    return hr;
}


//
// FlushQueuedEvents flushes a process's event queue.
//
void CordbRCEventThread::FlushQueuedEvents(CordbProcess* process)
{
    STRESS_LOG0(LF_CORDB,LL_INFO10000, "CRCET::FQE: Beginning to flush queue\n");

    // We should only call this is we already have queued events
    _ASSERTE(process->m_queuedEventList != NULL);

    //
    // Dispatch queued events so long as they keep calling Continue()
    // before returning from their callback. If they call Continue(),
    // process->m_synchronized will be false again and we know to
    // loop around and dispatch the next event.
    //
    _ASSERTE(process->ThreadHoldsProcessLock());

    {

        // Main dispatch loop here. DispatchRCEvent will take events out of the
        // queue and invoke callbacks
        do
        {
            // DispatchRCEvent will mark the process as stopped before dispatching.
            process->DispatchRCEvent();

            LOG((LF_CORDB,LL_INFO10000, "CRCET::FQE: Finished w/ "
                 "DispatchRCEvent\n"));
        }
        while ((process->GetSynchronized() == false) &&
               (process->m_queuedEventList != NULL) &&
               (process->m_unrecoverableError == false));
    }

    //
    // If they returned from a callback without calling Continue() then
    // the process is still synchronized, so let the rc event thread
    // know that it need to update its process list and remove the
    // process's event.
    //
    if (process->GetSynchronized())
    {
        ProcessStateChanged();
    }

    LOG((LF_CORDB,LL_INFO10000, "CRCET::FQE: finished\n"));
}


//
// HandleRCEvent -- handle an IPC event received from the runtime controller.
// This really just queues events where necessary and performs various
// DI-releated housekeeping for each event received. The events are
// dispatched via DispatchRCEvent.
//
void CordbRCEventThread::HandleRCEvent(CordbProcess* process,
                                       DebuggerIPCEvent* event)
{
    _ASSERTE(process->ThreadHoldsProcessLock());



    switch (event->type & DB_IPCE_TYPE_MASK)
    {
    case DB_IPCE_PING_ON_LAUNCH:
        // Nothing to do on the first ping for launch.
        // This was just to let the RS have a chance to fully update the IPC block.
        break;

    case DB_IPCE_SYNC_COMPLETE:
        //
        // The RC has synchronized the process. Flush any queued events.
        //
        STRESS_LOG1(LF_CORDB, LL_INFO1000, "[%x] RCET::HRCE: sync complete.\n",
             GetCurrentThreadId());

        DeleteIPCEventHelper(event);

        // when interop-debugging, we never send sync-complete via IPC events.
        // Always send via a flare.
        _ASSERTE((process->m_state & CordbProcess::PS_WIN32_ATTACHED) == 0);

        process->SetSynchronized(true);
        process->SetSyncCompleteRecv(true);

        if (!process->m_stopRequested)
        {
            // Note: we set the m_stopWaitEvent all the time and leave it high while we're stopped. Also note that we
            // can only do this after we've checked process->m_stopRequested!
            SetEvent(process->m_stopWaitEvent);

            _ASSERTE( (process->m_queuedEventList != NULL) ||
                    !"Sync complete received with no queued managed events!");

            FlushQueuedEvents(process);
        }
        else
        {
            STRESS_LOG1(LF_CORDB, LL_INFO1000,
                 "[%x] RCET::HRCE: stop requested, setting event.\n",
                 GetCurrentThreadId());

            SetEvent(process->m_stopWaitEvent);

            STRESS_LOG1(LF_CORDB, LL_INFO1000, "[%x] RCET::HRCE: set stop event.\n",
                 GetCurrentThreadId());
        }

        break;

    case DB_IPCE_THREAD_DETACH:
        {
            // Remember that we know the current thread has detached
            // so we won't try to work with it anymore. This prevents
            // people from making mistakes before draining an
            // ExitThread event.
            CordbThread *pThread =
                process->m_userThreads.GetBase(event->threadId);

            if (pThread != NULL)
                pThread->m_detached = true;
            // Fall through...
            goto QueueEvent;
        }

    case DB_IPCE_UPDATE_MODULE_SYMS:
    case DB_IPCE_MDA_NOTIFICATION:
    case DB_IPCE_BREAKPOINT:
    case DB_IPCE_USER_BREAKPOINT:
    case DB_IPCE_EXCEPTION:
    case DB_IPCE_STEP_COMPLETE:
    case DB_IPCE_THREAD_ATTACH:
    case DB_IPCE_LOAD_MODULE:
    case DB_IPCE_UNLOAD_MODULE:
    case DB_IPCE_LOAD_CLASS:
    case DB_IPCE_UNLOAD_CLASS:
    case DB_IPCE_FIRST_LOG_MESSAGE:
    case DB_IPCE_LOGSWITCH_SET_MESSAGE:
    case DB_IPCE_CREATE_APP_DOMAIN:
    case DB_IPCE_EXIT_APP_DOMAIN:
    case DB_IPCE_LOAD_ASSEMBLY:
    case DB_IPCE_UNLOAD_ASSEMBLY:
    case DB_IPCE_FUNC_EVAL_COMPLETE:
    case DB_IPCE_NAME_CHANGE:
    case DB_IPCE_CONTROL_C_EVENT:
    case DB_IPCE_ENC_REMAP:
    case DB_IPCE_ENC_REMAP_COMPLETE:
    case DB_IPCE_BREAKPOINT_SET_ERROR:
    case DB_IPCE_CREATE_CONNECTION:
    case DB_IPCE_DESTROY_CONNECTION:
    case DB_IPCE_CHANGE_CONNECTION:
    case DB_IPCE_EXCEPTION_CALLBACK2:
    case DB_IPCE_EXCEPTION_UNWIND:
    case DB_IPCE_INTERCEPT_EXCEPTION_COMPLETE:

QueueEvent:
        //
        // Queue all of these events.
        //

        STRESS_LOG2(LF_CORDB, LL_INFO1000, "[%x] RCET::HRCE: Queue event: %s\n",
             GetCurrentThreadId(), IPCENames::GetName(event->type));

        event->next = NULL;

        if (process->m_queuedEventList == NULL)
            process->m_queuedEventList = event;
        else
            process->m_lastQueuedEvent->next = event;

        process->m_lastQueuedEvent = event;

        break;

    default:
        STRESS_LOG2(LF_CORDB, LL_INFO1000,
             "[%x] RCET::HRCE: Unknown event: 0x%08x\n",
             GetCurrentThreadId(), event->type);
    }
}


//
// ProcessStateChanged -- tell the rc event thread that the ICorDebug's
// process list has changed by setting its flag and thread control event.
// This will cause the rc event thread to update its set of handles to wait
// on.
//
void CordbRCEventThread::ProcessStateChanged(void)
{
    m_cordb->LockProcessList();
    STRESS_LOG0(LF_CORDB, LL_INFO100000, "CRCET::ProcessStateChanged\n");
    m_processStateChanged = TRUE;
    SetEvent(m_threadControlEvent);
    m_cordb->UnlockProcessList();
}


//
// Primary loop of the Runtime Controller event thread.
//
// Some of this code is copied in CordbRCEventThead::VrpcToVrs
void CordbRCEventThread::ThreadProc(void)
{
    HANDLE        waitSet[MAXIMUM_WAIT_OBJECTS];
    CordbProcess* processSet[MAXIMUM_WAIT_OBJECTS];
    unsigned int  waitCount;

#ifdef _DEBUG
    memset(&processSet, NULL, MAXIMUM_WAIT_OBJECTS * sizeof(CordbProcess *));
    memset(&waitSet, NULL, MAXIMUM_WAIT_OBJECTS * sizeof(HANDLE));
#endif


    // First event to wait on is always the thread control event.
    waitSet[0] = m_threadControlEvent;
    processSet[0] = NULL;
    waitCount = 1;

    while (m_run)
    {
        DWORD ret = WaitForMultipleObjects(waitCount, waitSet, FALSE, 2000);

        if (ret == WAIT_FAILED)
        {
            STRESS_LOG1(LF_CORDB, LL_INFO10000, "CordbRCEventThread::ThreadProc WaitFor"
                "MultipleObjects failed: 0x%x\n", GetLastError());
        }
        else if ( (ret >= WAIT_OBJECT_0) && (ret < WAIT_OBJECT_0 + waitCount) && m_run)
        {
            // Got an event. Figure out which process it came from.
            unsigned int wn = ret - WAIT_OBJECT_0;

            if (wn != 0)
            {
                _ASSERTE(wn < MAXIMUM_WAIT_OBJECTS);
                CordbProcess* process = processSet[wn];
                _ASSERTE(process != NULL);

                // Check the process for any unrecoverable errors. It is possible for a process
                // to enter an unrecoverable error state and still send us a valid event.
                // Checking for that here ensures that the debugger will be notified as soon
                // as possible. If we don't check here, the unrecoverable error will most probably
                // go un-noticed until the process finally hangs and a request to the process
                // eventually times out.
                process->CheckForUnrecoverableError();

                HRESULT hr = S_OK;

                // Handle the first event from this process differently then all other events. The first event from the
                // process is special because it signals the first time that we know we can send events to the left side
                // if we actually launched this process.
                process->Lock();

                // Note: we also include a check of m_firstManagedEvent because m_initialized can go back to being false
                // during shutdown, even though we'll still be receiving some managed events (like module unloads, etc.)
                if (!process->m_firstManagedEvent && !process->m_initialized && process->IsSafeToSendEvents())
                {

                    STRESS_LOG3(LF_CORDB, LL_INFO1000, "RCET::TP: first event, pid 0x%x(%d)\n", wn, process->m_id, process->m_id);

                    // This can fail with the incompatable version HR. The process has already been terminated if this
                    // is the case.
                    hr = HandleFirstRCEvent(process);

                    _ASSERTE(SUCCEEDED(hr) || IsLegalFatalError(hr));

                    if (SUCCEEDED(hr))
                    {
                        // Remember that we're processing the first managed event... this will be used in HandleRCEvent
                        // below once the new event gets queued.
                        process->m_firstManagedEvent = true;
                    }
                }

                if (process->IsSafeToSendEvents() && SUCCEEDED(hr) && !process->m_exiting)
                {
                    STRESS_LOG2(LF_CORDB, LL_INFO1000, "RCET::TP: other event, pid 0x%x(%d)\n", process->m_id, process->m_id);

                    // Got a real IPC event.
                    DebuggerIPCEvent* event;

                    event = (DebuggerIPCEvent*) new (nothrow) BYTE[CorDBIPC_BUFFER_SIZE];

                    if (event == NULL)
                        CORDBSetUnrecoverableError(process, E_OUTOFMEMORY, 0);
                    else
                    {
                        hr = ReadRCEvent(process, event);
                        SetEvent(process->m_leftSideEventRead);

                        STRESS_LOG4(LF_CORDB, LL_INFO1000, "RCET::TP: Got %s for AD 0x%x, proc 0x%x(%d)\n",
                            IPCENames::GetName(event->type), LsPtrToCookie(event->appDomainToken), process->m_id, process->m_id);

                        if (SUCCEEDED(hr))
                        {
                            HandleRCEvent(process, event);
                        }
                        else
                        {
                            DeleteIPCEventHelper(event);
                            CORDBSetUnrecoverableError(process, hr, 0);
                        }
                    }
                }

                process->Unlock();
            }
        }

        // Empty any queued work items.
        DrainWorkerQueue();

        // Check a flag to see if we need to update our list of processes to wait on.
        if (m_processStateChanged)
        {
            STRESS_LOG0(LF_CORDB, LL_INFO1000, "RCET::TP: refreshing process list.\n");

            unsigned int i;

            //
            // free the old wait list
            //
            for (i = 1; i < waitCount; i++)
            {
                processSet[i]->InternalRelease();
            }

            // Pass 1: iterate the hash of all processes and collect the unsynchronized ones into the wait list.
            // Note that Stop / Continue can still be called on a different thread while we're doing this.
            m_cordb->LockProcessList();
            m_processStateChanged = FALSE;

            waitCount = 1;

            CordbSafeHashTable<CordbProcess>* ph = m_cordb->GetProcessList();
            CordbProcess * entry;
            HASHFIND find;

            for (entry =  ph->FindFirst(&find); entry != NULL; entry =  ph->FindNext(&find))
            {
                _ASSERTE(waitCount < MAXIMUM_WAIT_OBJECTS);
                if( waitCount >= MAXIMUM_WAIT_OBJECTS )
                {
                    break;
                }

                CordbProcess* p = entry;

                // Only listen to unsynchronized processes. Processes that are synchronized will not send events without
                // being asked by us first, so there is no need to async listen to them.
                //
                // Note: if a process is not synchronized then there is no way for it to transition to the syncrhonized
                // state without this thread receiving an event and taking action. So there is no need to lock the
                // per-process mutex when checking the process's synchronized flag here.
                if (!p->GetSynchronized() && p->IsSafeToSendEvents())
                {
                    STRESS_LOG2(LF_CORDB, LL_INFO1000, "RCET::TP: listening to process 0x%x(%d)\n", p->m_id, p->m_id);

                    waitSet[waitCount] = p->m_leftSideEventAvailable;
                    processSet[waitCount] = p;
                    processSet[waitCount]->InternalAddRef();

                    waitCount++;
                }
            }

            m_cordb->UnlockProcessList();

            // Pass 2: for each process that we placed in the wait list, determine if there are any existing queued
            // events that need to be flushed.

            // Start i at 1 to skip the control event...
            i = 1;
            while(i < waitCount)
            {
                CordbProcess *p = processSet[i];

                // Take the process lock so we can check the queue safely
                p->Lock();

                // Now that we've just locked the processes, we can safely inspect it and dispatch events.
                // The process may have changed since when we first added it to the process list in Pass 1,
                // so we can't make any assumptions about whether it's sync, live, or exiting.

                // Flush the queue if necessary. Note, we only do this if we've actually received a SyncComplete message
                // from this process. If we haven't received a SyncComplete yet, then we don't attempt to drain any
                // queued events yet. They'll be drained when the SyncComplete event is actually received.
                if (p->GetSyncCompleteRecv() &&
                    (p->m_queuedEventList != NULL) &&
                    !p->GetSynchronized())
                {
                    FlushQueuedEvents(p);
                }

                // Flushing could have left the process synchronized...
                // Common case is if the callback didn't call Continue().
                if (p->GetSynchronized())
                {
                    // remove the process from the wait list by sucking all the other processes down one.
                    if ((i + 1) < waitCount)
                    {
                        memcpy(&processSet[i], &processSet[i+1], sizeof(processSet[0]) * (waitCount - i - 1));
                        memcpy(&waitSet[i], &waitSet[i+1], sizeof(waitSet[0]) * (waitCount - i - 1));
                    }

                    // drop the count of processes to wait on
                    waitCount--;

                    p->Unlock();

                    // make sure to release the reference we added when the process was added to the wait list.
                    p->InternalRelease();

                    // We don't have to increment i because we've copied the next element into
                    // the current value at i.
                }
                else
                {
                    // Even after flushing, its still not syncd, so leave it in the wait list.
                    p->Unlock();

                    // Increment i normally.
                    i++;
                }
            }
        } // end ProcessStateChanged
    }
}


//
// This is the thread's real thread proc. It simply calls to the
// thread proc on the given object.
//
/*static*/ DWORD WINAPI CordbRCEventThread::ThreadProc(LPVOID parameter)
{
    CordbRCEventThread* t = (CordbRCEventThread*) parameter;

    INTERNAL_THREAD_ENTRY(t);
    t->ThreadProc();
    return 0;
}

template<typename T>
InterlockedStack<T>::InterlockedStack()
{
    m_pHead = NULL;
}

template<typename T>
InterlockedStack<T>::~InterlockedStack()
{
    // This is an arbitrary choice. We expect the stacks be drained.
    _ASSERTE(m_pHead == NULL);
}

// Thread safe pushes + pops.
// Many threads can push simultaneously.
// Only 1 thread can pop.
template<typename T>
void InterlockedStack<T>::Push(T * pItem)
{
    // InterlockedCompareExchangePointer(&dest, ex, comp).
    // Really behaves like:
    //     val = *dest;
    //     if (*dest == comp) { *dest = ex; }
    //     return val;
    //
    // We can do a thread-safe assign { comp = dest; dest = ex } via:
    //     do { comp = dest } while (ICExPtr(&dest, ex, comp) != comp));


    do
    {
        pItem->m_next = m_pHead;
    }
    while(InterlockedCompareExchangePointer((LPVOID volatile *) &m_pHead, pItem, pItem->m_next) != pItem->m_next);
}

// Returns NULL on empty,
// else returns the head of the list.
template<typename T>
T * InterlockedStack<T>::Pop()
{
    if (m_pHead == NULL)
    {
        return NULL;
    }

    // This allows 1 thread to Pop() and race against N threads doing a Push().
    T * pItem = NULL;
    do
    {
        pItem = m_pHead;
    } while(InterlockedCompareExchangePointer((LPVOID volatile *) &m_pHead, pItem->m_next, pItem) != pItem);

    return pItem;
}


// RCET will take ownership of this item and delete it.
// This can be done w/o taking any locks (thus it can be called from any lock context)
// This may race w/ the RCET draining the queue.
void CordbRCEventThread::QueueAsyncWorkItem(RCETWorkItem * pItem)
{

    _ASSERTE(pItem != NULL);

    m_WorkerStack.Push(pItem);

    // Ping the RCET so that it drains the queue.
    SetEvent(m_threadControlEvent);
}

// Execute & delete all workitems in the queue.
// This can be done w/o taking any locks. (though individual items may take locks).
void CordbRCEventThread::DrainWorkerQueue()
{
    _ASSERTE(IsRCEventThread());

    while(true)
    {
        RCETWorkItem* pCur = m_WorkerStack.Pop();
        if (pCur == NULL)
        {
            break;
        }

        pCur->Do();
        delete pCur;
    }
}


//
// WaitForIPCEventFromProcess waits for an event from just the specified
// process. This should only be called when the process is in a synchronized
// state, which ensures that the RCEventThread isn't listening to the
// process's event, too, which would get confusing.
//
HRESULT CordbRCEventThread::WaitForIPCEventFromProcess(
                                                   CordbProcess* process,
                                                   CordbAppDomain *pAppDomain,
                                                   DebuggerIPCEvent* event)
{
    CORDBRequireProcessStateOKAndSync(process, pAppDomain);

    DWORD ret;

    do
    {
        ret = SafeWaitForSingleObject(process, process->m_leftSideEventAvailable,
                                   CordbGetWaitTimeout());

        if (process->m_terminated)
        {
            return CORDBG_E_PROCESS_TERMINATED;
        }
        // If we timeout because we're waiting for an uncontinued OOB event, we need to just keep waiting.
    } while ((ret == WAIT_TIMEOUT) && process->IsWaitingForOOBEvent());




    if (ret == WAIT_OBJECT_0)
    {
        HRESULT hr = ReadRCEvent(process, event);
        STRESS_LOG4(LF_CORDB, LL_INFO1000, "CRCET::SIPCE: Got %s for AD 0x%x, proc 0x%x(%d)\n",
             IPCENames::GetName(event->type), LsPtrToCookie(event->appDomainToken), process->m_id, process->m_id);
        SetEvent(process->m_leftSideEventRead);

        return hr;
    }
    else if (ret == WAIT_TIMEOUT)
    {
        //
        // If we timed out, check the left side to see if it is in the
        // unrecoverable error mode. If it is, return the HR from the
        // left side that caused the error. Otherwise, return that we timed
        // out and that we don't really know why.
        //
        HRESULT realHR = ErrWrapper(CORDBG_E_TIMEOUT);

        HRESULT hr = process->CheckForUnrecoverableError();

        if (hr == S_OK)
        {
            CORDBSetUnrecoverableError(process, realHR, 0);
            return realHR;
        }
        else
            return hr;
    }
    else
    {
        _ASSERTE(ret == WAIT_FAILED);
        HRESULT hr = HRESULT_FROM_GetLastError();
        CORDBSetUnrecoverableError(process, hr, 0);
        return hr;
    }
}


//
// Start actually creates and starts the thread.
//
HRESULT CordbRCEventThread::Start(void)
{
    if (m_threadControlEvent == NULL)
        return E_INVALIDARG;

    m_thread = CreateThread(NULL, 0, &CordbRCEventThread::ThreadProc,
                            (LPVOID) this, 0, &m_threadId);

    if (m_thread == NULL)
        return HRESULT_FROM_GetLastError();

    return S_OK;
}


//
// Stop causes the thread to stop receiving events and exit. It
// waits for it to exit before returning.
//
HRESULT CordbRCEventThread::Stop(void)
{
    if (m_thread != NULL)
    {
        LOG((LF_CORDB, LL_INFO100000, "CRCET::Stop\n"));
        m_run = FALSE;
        SetEvent(m_threadControlEvent);

        DWORD ret = WaitForSingleObject(m_thread, INFINITE);

        if (ret != WAIT_OBJECT_0)
            return HRESULT_FROM_GetLastError();
    }

    m_cordb.Clear();

    return S_OK;
}


/* ------------------------------------------------------------------------- *
 * Win32 Event Thread class
 * ------------------------------------------------------------------------- */

enum
{
    W32ETA_NONE              = 0,
    W32ETA_CREATE_PROCESS    = 1,
    W32ETA_ATTACH_PROCESS    = 2,
    W32ETA_CONTINUE          = 3,
    W32ETA_DETACH            = 4
};



//
// Constructor
//
CordbWin32EventThread::CordbWin32EventThread(Cordb* cordb) :
    m_thread(NULL), m_threadControlEvent(NULL),
    m_actionTakenEvent(NULL), m_run(TRUE),
    m_action(W32ETA_NONE)
{
    m_cordb.Assign(cordb);

    _ASSERTE(cordb != NULL);
}


//
// Destructor. Cleans up all of the open handles and such.
// This expects that the thread has been stopped and has terminated
// before being called.
//
CordbWin32EventThread::~CordbWin32EventThread()
{
    if (m_thread != NULL)
        CloseHandle(m_thread);

    if (m_threadControlEvent != NULL)
        CloseHandle(m_threadControlEvent);

    if (m_actionTakenEvent != NULL)
        CloseHandle(m_actionTakenEvent);

    m_sendToWin32EventThreadMutex.Destroy();
}


//
// Init sets up all the objects that the thread will need to run.
//
HRESULT CordbWin32EventThread::Init(void)
{
    if (m_cordb == NULL)
        return E_INVALIDARG;


    m_sendToWin32EventThreadMutex.Init("Win32-Send lock", RSLock::cLockFlat, RSLock::LL_WIN32_SEND_LOCK);

    m_threadControlEvent = WszCreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_threadControlEvent == NULL)
        return HRESULT_FROM_GetLastError();

    m_actionTakenEvent = WszCreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_actionTakenEvent == NULL)
        return HRESULT_FROM_GetLastError();

    return S_OK;
}



//
// Main function of the Win32 Event Thread
//
void CordbWin32EventThread::ThreadProc(void)
{
#if defined(RSCONTRACTS)
    DbgRSThread::GetThread()->SetThreadType(DbgRSThread::cW32ET);

    // The win32 ET conceptually holds a lock (all threads do).
    DbgRSThread::GetThread()->TakeVirtualLock(RSLock::LL_WIN32_EVENT_THREAD);
#endif

    // Run the top-level event loop.
    Win32EventLoop();

#if defined(RSCONTRACTS)
    // The win32 ET conceptually holds a lock (all threads do).
    DbgRSThread::GetThread()->ReleaseVirtualLock(RSLock::LL_WIN32_EVENT_THREAD);
#endif
}

//
// Primary loop of the Win32 debug event thread.
//
void CordbWin32EventThread::Win32EventLoop(void)
{
    // This must be called from the win32 event thread.
    _ASSERTE(IsWin32EventThread());

    LOG((LF_CORDB, LL_INFO1000, "W32ET::W32EL: entered win32 event loop\n"));



    while (m_run)
    {
        BOOL eventAvailable = FALSE;

        // We should not have any locks right now.


        // Have to wait on 2 sources:
        // WaitForMultipleObjects - ping for messages (create, attach, Continue, detach) and also
        //    process exits in the managed-only case.
        // Native Debug Events - This is a huge perf hit so we want to avoid it whenever we can.
        //    Only wait on these if we're interop debugging and if the process is not frozen.
        //    A frozen process can't send any debug events, so don't bother looking for them.


        unsigned int waitCount = 1;
        HANDLE waitSet[2];
        waitSet[0] = m_threadControlEvent;
        DWORD waitTimeout = INFINITE;

        // In non-interop scenarios, we'll never get any native debug events, let alone an ExitProcess native event.
        bool fDidNotJustGetExitProcessEvent = true;


        // The m_pProcess handle will get nulled out after we process the ExitProcess event, and
        // that will ensure that we only wait for an Exit event once.
        if ((m_pProcess != NULL) && fDidNotJustGetExitProcessEvent)
        {
            waitSet[1] = m_pProcess->m_handle;
            waitCount = 2;
        }

        // See if any process that we aren't attached to as the Win32 debugger have exited. (Note: this is a
        // polling action if we are also waiting for Win32 debugger events. We're also looking at the thread
        // control event here, too, to see if we're supposed to do something, like attach.
        DWORD ret = WaitForMultipleObjects(waitCount, waitSet, FALSE, waitTimeout);

        _ASSERTE(ret == WAIT_TIMEOUT || ret < waitCount);

        if (!m_run)
        {
            _ASSERTE(m_action == W32ETA_NONE);
            break;
        }

        LOG((LF_CORDB, LL_INFO100000, "W32ET::W32EL - got event , ret=%d, has w32 dbg event=%d\n", ret, eventAvailable));

        // If we haven't timed out, or if it wasn't the thread control event
        // that was set, then a process has
        // exited...
        if ((ret != WAIT_TIMEOUT) && (ret != WAIT_OBJECT_0))
        {
            // Grab the process that exited.
            _ASSERTE((ret - WAIT_OBJECT_0) == 1);
            ExitProcess(false); // not detach
        }

        // Should we create a process?
        else if (m_action == W32ETA_CREATE_PROCESS)
            CreateProcess();

        // Should we attach to a process?
        else if (m_action == W32ETA_ATTACH_PROCESS)
            AttachProcess();

        // Should we detach from a process?
        else if (m_action == W32ETA_DETACH)
        {
            ExitProcess(true); // detach case
        }


        // We don't need to sweep the FCH threads since we never hijack a thread in cooperative mode.


        // Only process an event if one is available.
        if (!eventAvailable)
        {
            continue;
        }

    } // loop

    LOG((LF_CORDB, LL_INFO1000, "W32ET::W32EL: exiting event loop\n"));

    return;
}


HRESULT CordbProcess::GetAndWriteRemoteBuffer(CordbAppDomain *pDomain, unsigned int bufferSize, void *bufferFrom, void **ppRes)
{
    _ASSERTE(ppRes != NULL);
    *ppRes = NULL;

    HRESULT hr;
    void *buffer;
    IfFailRet(GetRemoteBuffer(pDomain, bufferSize, &buffer));

    // Write the new data into the buffer.
    BOOL succ = WriteProcessMemory(this->m_handle, buffer, bufferFrom, bufferSize, NULL);

    if (!succ)
    {
        hr = HRESULT_FROM_GetLastError();
        return hr;
    }
    *ppRes = buffer;
    return S_OK;
}

//
bool CordbProcess::IsSpecialStackOverflowCase(CordbUnmanagedThread *pUThread, DEBUG_EVENT *pEvent)
{
    return false;
}


//
// This is the thread's real thread proc. It simply calls to the
// thread proc on the given object.
//
/*static*/ DWORD WINAPI CordbWin32EventThread::ThreadProc(LPVOID parameter)
{
    CordbWin32EventThread* t = (CordbWin32EventThread*) parameter;
    INTERNAL_THREAD_ENTRY(t);
    t->ThreadProc();
    return 0;
}


//
// Send a CreateProcess event to the Win32 thread to have it create us
// a new process.
//
HRESULT CordbWin32EventThread::SendCreateProcessEvent(
                                  LPCWSTR programName,
                                  __in_z LPWSTR  programArgs,
                                  LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                  LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                  BOOL bInheritHandles,
                                  DWORD dwCreationFlags,
                                  PVOID lpEnvironment,
                                  LPCWSTR lpCurrentDirectory,
                                  LPSTARTUPINFOW lpStartupInfo,
                                  LPPROCESS_INFORMATION lpProcessInformation,
                                  CorDebugCreateProcessFlags corDebugFlags)
{
    HRESULT hr = S_OK;

    LockSendToWin32EventThreadMutex();
    LOG((LF_CORDB, LL_EVERYTHING, "CordbWin32EventThread::SCPE Called\n"));
    m_actionData.createData.programName = programName;
    m_actionData.createData.programArgs = programArgs;
    m_actionData.createData.lpProcessAttributes = lpProcessAttributes;
    m_actionData.createData.lpThreadAttributes = lpThreadAttributes;
    m_actionData.createData.bInheritHandles = bInheritHandles;
    m_actionData.createData.dwCreationFlags = dwCreationFlags;
    m_actionData.createData.lpEnvironment = lpEnvironment;
    m_actionData.createData.lpCurrentDirectory = lpCurrentDirectory;
    m_actionData.createData.lpStartupInfo = lpStartupInfo;
    m_actionData.createData.lpProcessInformation = lpProcessInformation;
    m_actionData.createData.corDebugFlags = corDebugFlags;

    // m_action is set last so that the win32 event thread can inspect
    // it and take action without actually having to take any
    // locks. The lock around this here is simply to prevent multiple
    // threads from making requests at the same time.
    m_action = W32ETA_CREATE_PROCESS;

    BOOL succ = SetEvent(m_threadControlEvent);

    if (succ)
    {
      LOG((LF_CORDB, LL_EVERYTHING, "Process Handle is: %x, m_threadControlEvent is %x\n", (UINT_PTR)m_actionData.createData.lpProcessInformation->hProcess, (UINT_PTR)m_threadControlEvent));
      DWORD ret = WaitForSingleObject(m_actionTakenEvent, INFINITE);

        if (ret == WAIT_OBJECT_0)
            hr = m_actionResult;
        else
            hr = HRESULT_FROM_GetLastError();
    }
    else
        hr = HRESULT_FROM_GetLastError();

    UnlockSendToWin32EventThreadMutex();

    return hr;
}


//
// Create a new process. This is called in the context of the Win32
// event thread to ensure that if we're Win32 debugging the process
// that the same thread that waits for debugging events will be the
// thread that creates the process.
//
void CordbWin32EventThread::CreateProcess(void)
{
    m_action = W32ETA_NONE;
    HRESULT hr = S_OK;

    // Process information is passed in the action struct
    PROCESS_INFORMATION *pi =
        m_actionData.createData.lpProcessInformation;

    DWORD dwCreationFlags = m_actionData.createData.dwCreationFlags;

    const bool fInterop =
            false; // FEATURE_PAL doesn't support interop-debugging.

    // the environment string that we pass to CreateProcess is either unicode or 
    // ansi (depending on the creation flags).
    // Rotor doesn't support unicode environments.
    const bool fUnicode =
            false;

    // Ensure that any environment block actually contains CORDBG_ENABLE.
    const BYTE *lpEnvironment = (BYTE*) m_actionData.createData.lpEnvironment;

    const BYTE * lpMemoryToFree = NULL;

    if (lpEnvironment == NULL)
    {
        // Memory from GetEnvironmentStrings() needs to be freed later with 
        // FreeEnviromentStrings. We also can not modify this memory.
        if (fUnicode)
        {
            lpMemoryToFree = (const BYTE*) WszGetEnvironmentStrings();
        }
        else
        {
            lpMemoryToFree = (const BYTE*) GetEnvironmentStringsA(); // Ansi version
        }
        lpEnvironment = lpMemoryToFree;
    }
    
    bool needToFreeEnvBlock = false;

    // We should have already verified that we can have another debuggee
    _ASSERTE(m_cordb->AllowAnotherProcess());

    _ASSERTE (lpEnvironment != NULL);
    
    if (fUnicode)
    {
        needToFreeEnvBlock = EnsureCorDbgEnvVarSet(
                            (const WCHAR**)&lpEnvironment,
                            (const WCHAR*) CorDB_CONTROL_ENV_VAR_NAMEL L"=",
                            true,
                            (DWORD)DBCF_GENERATE_DEBUG_CODE);
    }
    else
    {
        needToFreeEnvBlock = EnsureCorDbgEnvVarSet(
                            (const CHAR**)&lpEnvironment,
                            (const CHAR*)CorDB_CONTROL_ENV_VAR_NAME "=",
                            false,
                            (DWORD)DBCF_GENERATE_DEBUG_CODE);
    }


    // If the creation flags has DEBUG_PROCESS in them, then we're
    // Win32 debugging this process. Otherwise, we have to create
    // suspended to give us time to setup up our side of the IPC
    // channel.
    BOOL clientWantsSuspend;
    clientWantsSuspend = (dwCreationFlags & CREATE_SUSPENDED);

    if (!fInterop)
    {
        dwCreationFlags |= CREATE_SUSPENDED;
    }

    // Have Win32 create the process...
    BOOL ret;

    if (needToFreeEnvBlock)
    {
        ret = ::WszCreateProcess(
                          m_actionData.createData.programName,
                          m_actionData.createData.programArgs,
                          m_actionData.createData.lpProcessAttributes,
                          m_actionData.createData.lpThreadAttributes,
                          m_actionData.createData.bInheritHandles,
                          dwCreationFlags,
                          (void*) lpEnvironment,
                          m_actionData.createData.lpCurrentDirectory,
                          m_actionData.createData.lpStartupInfo,
                          m_actionData.createData.lpProcessInformation);


        if (!ret)
        {
            hr = HRESULT_FROM_GetLastError();
            // Don't bail yet since we still need to remove the env var.
        }
    }
    else
    {
        // Failed to allocate new env block.
        ret =FALSE;
        hr = E_OUTOFMEMORY;
    }
    
    // Free memory from GetEnviromentStrings()
    if (lpMemoryToFree != NULL)
    {
        if (fUnicode)
        {
            WszFreeEnvironmentStrings( (LPWSTR) (lpMemoryToFree));
        }
        else
        {
            FreeEnvironmentStringsA( (LPSTR) (lpMemoryToFree));
        }
        lpMemoryToFree = NULL;
    }


    if (ret)
    {
        // Create a process object to represent this process.
        CordbProcess* process = new (nothrow) CordbProcess(m_cordb, this,
                                                 pi->dwProcessId,
                                                 pi->hProcess);
        if (process != NULL)
        {
            RSSmartPtr<CordbProcess> ptr(process);

            // Initialize the process. This will setup our half of the
            // IPC channel, too.
            hr = process->Init(fInterop);

            // Shouldn't happen on a create, only an attach
            _ASSERTE(hr != CORDBG_E_DEBUGGER_ALREADY_ATTACHED);

            // Remember the process in the global list of processes.
            if (SUCCEEDED(hr))
            {
                hr = m_cordb->AddProcess(process); // will take ref if it succeeds
            }

            // ptr dtor will release. If SUCCEEDED(hr), then Cordb has another ref. Else, this Release could delete us.
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }

        // If we're Win32 attached to this process, then increment the
        // proper count, otherwise add this process to the wait set
        // and resume the process's main thread.
        if (SUCCEEDED(hr))
        {
            _ASSERTE(m_pProcess == NULL);
            m_pProcess.Assign(process);


            if (!fInterop)
            {
                // Also, pretend that we've already received the loader breakpoint so that managed events will get
                // dispatched.
                process->m_loaderBPReceived = true;


                // Resume the process's main thread now that
                // everything is set up. But only resume if the user
                // didn't specify that they wanted the process created
                // suspended!
                if (!clientWantsSuspend)
                {
                    ResumeThread(pi->hThread);
            }
            } // if managed-only
        }
    }



    // If we created this environment block, then free it.
    if (needToFreeEnvBlock)
    {
        delete [] lpEnvironment;
    }

    //
    // Signal the hr to the caller.
    //
    m_actionResult = hr;
    SetEvent(m_actionTakenEvent);
}


//
// Send a DebugActiveProcess event to the Win32 thread to have it attach to
// a new process.
//
HRESULT CordbWin32EventThread::SendDebugActiveProcessEvent(
                                                  DWORD pid,
                                                  bool fWin32Attach,
                                                  CordbProcess *pProcess)
{
    HRESULT hr = S_OK;

    LockSendToWin32EventThreadMutex();

    m_actionData.attachData.processId = pid;
    m_actionData.attachData.pProcess = pProcess;

    // m_action is set last so that the win32 event thread can inspect
    // it and take action without actually having to take any
    // locks. The lock around this here is simply to prevent multiple
    // threads from making requests at the same time.
    m_action = W32ETA_ATTACH_PROCESS;

    BOOL succ = SetEvent(m_threadControlEvent);

    if (succ)
    {
        DWORD ret = WaitForSingleObject(m_actionTakenEvent, INFINITE);

        if (ret == WAIT_OBJECT_0)
            hr = m_actionResult;
        else
            hr = HRESULT_FROM_GetLastError();
    }
    else
        hr = HRESULT_FROM_GetLastError();

    UnlockSendToWin32EventThreadMutex();

    return hr;
}

//-----------------------------------------------------------------------------
// Is the given thread id a helper thread (real or worker?)
//-----------------------------------------------------------------------------
bool CordbProcess::IsHelperThreadWorked(DWORD tid)
{
    // Check against the id gained by sniffing Thread-Create events.
    if (tid == this->m_helperThreadId)
    {
        return true;
    }

    // Now check for potential datate in the IPC block. If not there,
    // then we know it can't be the helper.
    DebuggerIPCControlBlock * pDCB = this->m_DCB;

    if (pDCB == NULL)
    {
        return false;
    }

    return
        (tid == pDCB->m_realHelperThreadId) ||
        (tid == pDCB->m_temporaryHelperThreadId);

}

//
// Cleans up the Left Side's DCB after a failed attach attempt.
//
void CordbProcess::CleanupHalfBakedLeftSide(void)
{
    _ASSERTE(IsWin32EventThread());

    if (m_DCB != NULL)
    {
        m_DCB->m_leftSideEventAvailable.CloseInRemoteProcess(m_handle);
        m_DCB->m_leftSideEventRead.CloseInRemoteProcess(m_handle);
        m_DCB->m_rightSideProcessHandle.CloseInRemoteProcess(m_handle);

        m_DCB->m_rightSideIsWin32Debugger = false;
    }

    // We need to close the setup sync event if we still have it, since a) we shouldn't leak the handle and b) if the
    // debuggee doesn't have a CLR loaded into it, then it shouldn't have a setup sync event created! This was the cause
    // of bug 98348.
    if (m_SetupSyncEvent != NULL)
    {
        CloseHandle(m_SetupSyncEvent);
        m_SetupSyncEvent = NULL;
    }

    // Close the various handles and events.
    CloseIPCHandles();

    m_cordb.Clear();
    
    // This process object is Dead-On-Arrival, so it doesn't really have anything to neuter.
    // But for safekeeping, we'll mark it as neutered.
    UnsafeNeuterDeadObject(); 
}


//
// Attach to a process. This is called in the context of the Win32
// event thread to ensure that if we're Win32 debugging the process
// that the same thread that waits for debugging events will be the
// thread that attaches the process.
//
void CordbWin32EventThread::AttachProcess()
{
    _ASSERTE(IsWin32EventThread());

    CordbProcess* process = NULL;
    m_action = W32ETA_NONE;
    HRESULT hr = S_OK;


    // We should have already verified that we can have another debuggee
    _ASSERTE(m_cordb->AllowAnotherProcess());

    // We need to get a handle to the process.
    HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE        |
                                  PROCESS_QUERY_INFORMATION |
                                  PROCESS_TERMINATE         |
                                  PROCESS_VM_OPERATION      |
                                  PROCESS_VM_READ           |
                                  PROCESS_VM_WRITE          |
                                  SYNCHRONIZE,
                                  FALSE,
                                  m_actionData.attachData.processId);
    if (hProcess == NULL)
    {
        hr = HRESULT_FROM_GetLastError();
    }

    LOG((LF_CORDB, LL_INFO10000, "[%x] W32ET::TP: process handle 0x%08x\n", GetCurrentThreadId(), hProcess));

    if( hProcess != NULL ) 
    {
        // Create a process object to represent this process.
        process = new (nothrow) CordbProcess(m_cordb, this, m_actionData.attachData.processId, hProcess);

        if( process == NULL )
        {
            hr = E_OUTOFMEMORY;
            CloseHandle(hProcess);
        }
        else
        {
            RSSmartPtr<CordbProcess> ptr(process);

            // Initialize the process. This will setup our half of the IPC channel, too.
            hr = process->Init(
                               FALSE
                   );

            // Remember the process in the global list of processes.
            if (SUCCEEDED(hr))
            {
                hr = m_cordb->AddProcess(process); // will take ref if it succeeds

                if (!SUCCEEDED(hr))
                {
                    process->CleanupHalfBakedLeftSide();
                }
            }
            // ptr dtor will always release. On success, Cordb already took a ref. On failure,
            // this release might be our final one.
        }

        // If we're Win32 attaching to this process, then increment
        // the proper count, otherwise add this process to the wait
        // set and resume the process's main thread.
        if (SUCCEEDED(hr))
        {
            _ASSERTE(m_pProcess == NULL);
            m_pProcess.Assign(process);
            process = NULL;     // ownership transfered to m_pProcess

            {
                // Also, pretend that we've already received the loader breakpoint so that managed events will get
                // dispatched.
                m_pProcess->m_loaderBPReceived = true;
            }
        }
    }

    //
    // Signal the hr to the caller.
    //
    m_actionResult = hr;
    SetEvent(m_actionTakenEvent);
}

// Note that the actual 'DetachProcess' method is really ExitProcess with CW32ET_UNKNOWN_PROCESS_SLOT ==
// processSlot
HRESULT CordbWin32EventThread::SendDetachProcessEvent(CordbProcess *pProcess)
{
    LOG((LF_CORDB, LL_INFO1000, "W32ET::SDPE\n"));
    HRESULT hr = S_OK;

    LockSendToWin32EventThreadMutex();

    m_actionData.detachData.pProcess = pProcess;

    // m_action is set last so that the win32 event thread can inspect it and take action without actually
    // having to take any locks. The lock around this here is simply to prevent multiple threads from making
    // requests at the same time.
    m_action = W32ETA_DETACH;

    BOOL succ = SetEvent(m_threadControlEvent);

    if (succ)
    {
        DWORD ret = WaitForSingleObject(m_actionTakenEvent, INFINITE);

        if (ret == WAIT_OBJECT_0)
            hr = m_actionResult;
        else
            hr = HRESULT_FROM_GetLastError();
    }
    else
        hr = HRESULT_FROM_GetLastError();

    UnlockSendToWin32EventThreadMutex();

    return hr;
}


void ExitProcessWorkItem::Do()
{
    STRESS_LOG1(LF_CORDB, LL_INFO1000, "ExitProcessWorkItem proc=%p\n", GetProcess());

    // This is being called on the RCET.
    // That's the thread that dispatches managed events. Since it's calling us now, we know
    // it can't be dispatching a managed event, and so we don't need to both waiting for it

    Cordb * pCordb = GetProcess()->m_cordb;

    {
        // Get the SG lock here to coordinate against any other continues.
        RSLockHolder ch(GetProcess()->GetStopGoLock());
        RSLockHolder ch2(&(GetProcess()->m_processMutex));

        LOG((LF_CORDB, LL_INFO1000,"W32ET::EP: ExitProcess callback\n"));

        // We're synchronized now, so mark the process as such.
        GetProcess()->SetSynchronized(true);
        GetProcess()->IncStopCount();

        // By the time we release the SG + Process locks here, the process object has been
        // marked as exiting + terminated (by the w32et which queued us). Future attemps to
        // continue should fail, and thus we should remain synchronized.
    }


    //  Just to be safe, neuter any children before the exit process callback.
    {
        RSLockHolder ch(GetProcess()->GetStopGoLock());

        // Release the process.
        NeuterTicket ticket(GetProcess());
        GetProcess()->NeuterChildren(ticket);
    }


    // Invoke the ExitProcess callback. This is very important since the a shell
    // may rely on it for proper shutdown and may hang if they don't get it.
    // We don't expect Cordbg to continue from this (we're certainly not going to wait for it).
    if (pCordb->m_managedCallback)
    {
        PUBLIC_CALLBACK_IN_THIS_SCOPE0(GetProcess());
        pCordb->m_managedCallback->ExitProcess((ICorDebugProcess*)GetProcess());
    }
    // This CordbProcess object now has no reservations against a client calling ICorDebug::Terminate.
    // That call may race against the CordbProcess::Neuter below, but since we already neutered the children,
    // that neuter call will not do anything interesting that will conflict with Terminate.
    
    
    LOG((LF_CORDB, LL_INFO1000,"W32ET::EP: returned from ExitProcess callback\n"));


    {
        RSLockHolder ch(GetProcess()->GetStopGoLock());

        // Release the process.
        NeuterTicket ticket(GetProcess());
        GetProcess()->Neuter(ticket);
    }

    // Our dtor will release the Process object.
    // This may be the final release on the process.
}


//
// ExitProcess is called when a process exits or detaches.
// This does our final cleanup and removes the process from our wait sets.
// We're either here because we're detaching (fDetach == TRUE), or because the process has really exited,
// and we're doing shutdown logic.
//
void CordbWin32EventThread::ExitProcess(bool fDetach)
{
    INTERNAL_API_ENTRY(this);


    LOG((LF_CORDB, LL_INFO1000,"W32ET::EP: begin ExitProcess, detach=%d\n", fDetach));

    // We don't really care if we're on the Win32 thread or not here. We just want to be sure that
    // the LS Exit case and the Detach case both occur on the same thread. This makes it much easier
    // to assert that if we exit while detaching, EP is only called once.
    // If we ever decide to make the RCET listen on the LS process handle for EP(exit), then we should also
    // make the EP(detach) handled on the RCET (via DoFavor() ).
    _ASSERTE(IsWin32EventThread());

    // So either the Exit case or Detach case must happen first.
    // 1) If Detach first, then LS process is removed from wait set and so EP(Exit) will never happen
    //    because we check wait set after returning from EP(Detach).
    // 2) If Exit is first, m_pProcess gets set=NULL. EP(detach) will still get called, so explicitly check that.
    if (fDetach &&
        ((m_pProcess == NULL) || m_pProcess->m_terminated))
    {
        // m_terminated is only set after the LS exits.
        // So the only way (fDetach && m_terminated) is true is if the LS exited while detaching. In that case
        // we already called EP(exit) and we don't want to call it again for EP(detach). So return here.
        LOG((LF_CORDB, LL_INFO1000,"W32ET::EP: In EP(detach), but EP(exit) already called. Early failure\n"));

        m_actionResult = CORDBG_E_PROCESS_TERMINATED;
        SetEvent(m_actionTakenEvent);

        return;
    }

    // We null m_pProcess at the end here, so
    // Only way we could get here w/ null process is if we're called twice. We can only be called
    // by detach or exit. Can't detach twice, can't exit twice, so must have been one of each.
    // If exit is first, we got removed from the wait set, so 2nd call must be detach and we'd catch
    // that above. If detach is first, we'd get removed from the wait set and so exit would never happen.
    _ASSERTE(m_pProcess != NULL);
    _ASSERTE(!m_pProcess->ThreadHoldsProcessLock());

    // Interop-detach is not supported.
    _ASSERTE(!fDetach || !(m_pProcess->m_state & m_pProcess->PS_WIN32_ATTACHED));


    // Mark the process teminated. After this, the RCET will never call FlushQueuedEvents. It will
    // ignore all events it receives (including a SyncComplete) and the RCET also does not listen
    // to terminated processes (so ProcessStateChange() won't cause a FQE either).
    m_pProcess->Terminating(fDetach);

    // Take care of the race where the process exits right after the user calls Continue() from the last
    // managed event but before the handler has actually returned.
    //
    // Also, To get through this lock means that either:
    // 1. FlushQueuedEvents is not currently executing and no one will call FQE.
    // 2. FQE is exiting but is in the middle of a callback (so AreDispatchingEvent = true)
    //
    m_pProcess->Lock();

    m_pProcess->m_exiting = true;

    if (fDetach)
    {
        m_pProcess->SetSynchronized(false);
    }

    // Close off the handle to the setup sync event now, since we know that the pid could be reused at this
    // point (depending on how the exit occured.)
    if (m_pProcess->m_SetupSyncEvent != NULL)
    {
        LOG((LF_CORDB, LL_INFO1000,"W32ET::EP: Shutting down setupsync event\n"));

        CloseHandle(m_pProcess->m_SetupSyncEvent);
        m_pProcess->m_SetupSyncEvent = NULL;

        m_pProcess->m_DCB = NULL;

        if (m_pProcess->m_IPCReader.IsPrivateBlockOpen())
        {
            m_pProcess->m_IPCReader.ClosePrivateBlock();
            LOG((LF_CORDB, LL_INFO1000,"W32ET::EP: Closing private block\n"));
        }

    }

    // If we are exiting, we *must* dispatch the ExitProcess callback, but we will delete all the events
    // in the queue and not bother dispatching anything else. If (and only if) we are currently dispatching
    // an event, then we will wait while that event is finished before invoking ExitProcess.
    // (Note that a dispatched event has already been removed from the queue)


    // Delete all queued events while under the lock
    m_pProcess->DeleteQueuedEvents();

    // Remove the process from the global list of processes.
    m_cordb->RemoveProcess(m_pProcess);

    if (fDetach)
    {
        // Signal the hr to the caller.
        LOG((LF_CORDB, LL_INFO1000,"W32ET::EP: Detach: send result back!\n"));

        m_actionResult = S_OK;
        SetEvent(m_actionTakenEvent);
    }

    m_pProcess->Unlock();

    // If we're detaching, then the Detach already neutered everybody, so nothing here.
    // If we're exiting, then we still need to neuter things, but we can't do that on this thread,
    // so we queue it. We also need to dispatch an exit process callback. We'll queue that onto the RCET
    // and dispatch it inband w/the other callbacks.
    if (!fDetach)
    {
        ExitProcessWorkItem * pItem = new ExitProcessWorkItem(m_pProcess);
        if (pItem != NULL)
        {
            m_cordb->m_rcEventThread->QueueAsyncWorkItem(pItem);
        }
    }

    // This will remove the process from our wait lists (so that we don't send multiple ExitProcess events).
    m_pProcess.Clear();
}


//
// Start actually creates and starts the thread.
//
HRESULT CordbWin32EventThread::Start(void)
{
    HRESULT hr = S_OK;
    if (m_threadControlEvent == NULL)
        return E_INVALIDARG;

    // Create the thread suspended to make sure that m_threadId is set
    // before CordbWin32EventThread::ThreadProc runs
    m_thread = CreateThread(NULL, 0, &CordbWin32EventThread::ThreadProc,
                            (LPVOID) this, CREATE_SUSPENDED, &m_threadId);

    if (m_thread == NULL)
        return HRESULT_FROM_GetLastError();

    DWORD succ = ResumeThread(m_thread);
    if (succ == (DWORD)-1)
        return HRESULT_FROM_GetLastError();

    return hr;
}


//
// Stop causes the thread to stop receiving events and exit. It
// waits for it to exit before returning.
//
HRESULT CordbWin32EventThread::Stop(void)
{
    HRESULT hr = S_OK;

    // m_pProcess may be NULL from CordbWin32EventThread::ExitProcess

    // Can't block on W32ET while holding the process-lock since the W32ET may need that to exit.
    // But since m_pProcess may be null, we can't enforce that.

    if (m_thread != NULL)
    {
        LockSendToWin32EventThreadMutex();
        m_action = W32ETA_NONE;
        m_run = FALSE;

        SetEvent(m_threadControlEvent);
        UnlockSendToWin32EventThreadMutex();

        DWORD ret = WaitForSingleObject(m_thread, INFINITE);

        if (ret != WAIT_OBJECT_0)
            hr = HRESULT_FROM_GetLastError();
    }

    m_pProcess.Clear();
    m_cordb.Clear();

    return hr;
}








/*
 * This will request a buffer of size cbBuffer to be allocated
 * on the left side.
 *
 * If successful, returns S_OK.  If unsuccessful, returns E_OUTOFMEMORY.
 */
HRESULT CordbProcess::GetRemoteBuffer(CordbAppDomain *pDomain, ULONG cbBuffer, void **ppBuffer)
{
    INTERNAL_SYNC_API_ENTRY(this); //
    // Initialize variable to null in case request fails
    *ppBuffer = NULL;

    // Create and initialize the event as synchronous
    DebuggerIPCEvent event;
    InitIPCEvent(&event,
                 DB_IPCE_GET_BUFFER,
                 true,
                 pDomain ? pDomain->GetADToken() : LSPTR_APPDOMAIN::NullPtr());

    // Indicate the buffer size wanted
    event.GetBuffer.bufSize = cbBuffer;

    // Make the request, which is synchronous
    HRESULT hr = SendIPCEvent(&event, sizeof(event));
    TESTANDRETURNHR(hr);
    _ASSERTE(event.type == DB_IPCE_GET_BUFFER_RESULT);

    // Save the result
    *ppBuffer = event.GetBufferResult.pBuffer;

    // Indicate success
    return event.GetBufferResult.hr;
}

/*
 * This will release a previously allocated left side buffer.
 */
HRESULT CordbProcess::ReleaseRemoteBuffer(void **ppBuffer)
{
    INTERNAL_SYNC_API_ENTRY(this); //
    // Create and initialize the event as synchronous
    DebuggerIPCEvent event;
    InitIPCEvent(&event,
                 DB_IPCE_RELEASE_BUFFER,
                 true,
                 LSPTR_APPDOMAIN::NullPtr());

    // Indicate the buffer to release
    event.ReleaseBuffer.pBuffer = (*ppBuffer);

    // Make the request, which is synchronous
    HRESULT hr = SendIPCEvent(&event, sizeof(event));
    TESTANDRETURNHR(hr);

    (*ppBuffer) = NULL;

    // Indicate success
    return event.ReleaseBufferResult.hr;
}

HRESULT CordbProcess::SetDesiredNGENCompilerFlags(DWORD dwFlags)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);

    if (dwFlags != CORDEBUG_JIT_DEFAULT && dwFlags != CORDEBUG_JIT_DISABLE_OPTIMIZATION)

    {
        return E_INVALIDARG;
    }

    CordbProcess *pProcess = GetProcess();

    if (pProcess->GetDispatchedEvent() != DB_IPCE_CREATE_PROCESS)
    {
        return CORDBG_E_MUST_BE_IN_CREATE_PROCESS;
    }

    if (pProcess->m_attached)
    {
        return CORDBG_E_CANNOT_BE_ON_ATTACH;
    }

    ATT_REQUIRE_STOPPED_MAY_FAIL(pProcess);

    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event,
                           DB_IPCE_SET_NGEN_COMPILER_FLAGS,
                           true,
                           LSPTR_APPDOMAIN::NullPtr());

    event.JitDebugInfo.fAllowJitOpts =
        ((dwFlags & CORDEBUG_JIT_DISABLE_OPTIMIZATION) != CORDEBUG_JIT_DISABLE_OPTIMIZATION);

    // Note: two-way event here...
    HRESULT hr = pProcess->m_cordb->SendIPCEvent(pProcess,
                                                 &event,
                                                 sizeof(DebuggerIPCEvent));

    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_SET_NGEN_COMPILER_FLAGS_RESULT);

    return event.hr;
}

HRESULT CordbProcess::GetDesiredNGENCompilerFlags(DWORD *pdwFlags )
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pdwFlags, DWORD*);
    *pdwFlags = 0;

    CordbProcess *pProcess = GetProcess();


    ATT_REQUIRE_STOPPED_MAY_FAIL(pProcess);

    DebuggerIPCEvent event;
    pProcess->InitIPCEvent(&event,
                           DB_IPCE_GET_NGEN_COMPILER_FLAGS,
                           true,
                           LSPTR_APPDOMAIN::NullPtr());

    // Note: two-way event here...
    HRESULT hr = pProcess->m_cordb->SendIPCEvent(pProcess,
                                                 &event,
                                                 sizeof(DebuggerIPCEvent));

    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_GET_NGEN_COMPILER_FLAGS_RESULT);

    DWORD dwFlags = CORDEBUG_JIT_DEFAULT;
    if (! event.JitDebugInfo.fAllowJitOpts)
    {
        dwFlags = CORDEBUG_JIT_DISABLE_OPTIMIZATION;
    }

    *pdwFlags = dwFlags;
    return event.hr;
}

// Wrapper to get a typesafe ObjectHandle ptr from an untyped ptr.
// This does not validate the handle.
LSPTR_OBJECTHANDLE UnsafeCastToGCHandle(UINT_PTR gcHandle)
{
    LSPTR_OBJECTHANDLE x;
    x.Set((void*) gcHandle);
    return x;
}

//-----------------------------------------------------------------------------
// Get an ICorDebugReference Value for the GC handle.
// handle - raw bits for the GC handle.
// pOutHandle
//-----------------------------------------------------------------------------
HRESULT CordbProcess::GetReferenceValueFromGCHandle(
    UINT_PTR gcHandle,
    ICorDebugReferenceValue **pOutValue)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(this);
    VALIDATE_POINTER_TO_OBJECT(pOutValue, ICorDebugReferenceValue*);

    *pOutValue = NULL;
    HRESULT hr = S_OK;


    if (gcHandle == NULL)
    {
        return CORDBG_E_BAD_REFERENCE_VALUE;
    }

    LSPTR_OBJECTHANDLE oh = UnsafeCastToGCHandle(gcHandle);

    // All Values live in some AD, so get the Appdomain for this handle
    DebuggerIPCEvent event;

    this->InitIPCEvent(&event,
                            DB_IPCE_GET_GCHANDLE_INFO,
                            true,     // Note: two-way event here...
                            LSPTR_APPDOMAIN::NullPtr());
    event.GetGCHandleInfo.GCHandle = oh;

    hr = this->SendIPCEvent(&event, sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event.hr);

    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    _ASSERTE(event.type == DB_IPCE_GET_GCHANDLE_INFO_RESULT);


    if (!event.GetGCHandleInfoResult.fValid)
    {
        return CORDBG_E_BAD_REFERENCE_VALUE;
    }

    CordbAppDomain * pAppDomain = this->m_appDomains.GetBase(
            LsPtrToCookie(event.GetGCHandleInfoResult.pLSAppDomain));
    _ASSERTE(pAppDomain != NULL);

    // Now that we finally have the AppDomain, we can go ahead and get a ReferenceValue
    // from the ObjectHandle.
    hr = CordbReferenceValue::BuildFromGCHandle(pAppDomain, oh, pOutValue);
    _ASSERTE(SUCCEEDED(hr) == (*pOutValue != NULL));

    return hr;
}

/*
 * IsReadyForDetach
 *
 * This method encapsulates all logic for deciding if it is ok for a debugger to
 * detach from the process at this time.
 *
 * Parameters: None.
 *
 * Returns: S_OK if it is ok to detach, else a specific HRESULT describing why it
 *   is not ok to detach.
 *
 */
HRESULT CordbProcess::IsReadyForDetach(void)
{
    INTERNAL_SYNC_API_ENTRY(this);

    //
    // If there are any outstanding func-evals then fail the detach.
    //
    if (OutstandingEvalCount() != 0)
    {
        return CORDBG_E_DETACH_FAILED_OUTSTANDING_EVALS;
    }

    //
    // If there are any outstanding steppers then fail the detach.
    //
    if (m_steppers.IsInitialized() && (m_steppers.GetCount() > 0))
    {
        return CORDBG_E_DETACH_FAILED_OUTSTANDING_STEPPERS;
    }

    //
    // If there are any outstanding breakpoints then fail the detach.
    //
    HASHFIND foundAppDomain;
    CordbAppDomain *pAppDomain = m_appDomains.FindFirst(&foundAppDomain);

    while (pAppDomain != NULL)
    {
        if (pAppDomain->m_breakpoints.IsInitialized() && (pAppDomain->m_breakpoints.GetCount() > 0))
        {
            return CORDBG_E_DETACH_FAILED_OUTSTANDING_BREAKPOINTS;
        }

        // Check for any outstanding EnC modules.
        HASHFIND foundModule;
        CordbModule * pModule = pAppDomain->m_modules.FindFirst(&foundModule);
        while (pModule != NULL)
        {
            if (pModule->m_EnCCount > 0)
            {
                return CORDBG_E_DETACH_FAILED_ON_ENC;
            }
            pModule = pAppDomain->m_modules.FindNext(&foundModule);
        }


        pAppDomain = m_appDomains.FindNext(&foundAppDomain);
    }

    return S_OK;
}


/*
 * Look for any thread which was last seen in the specified AppDomain.  
 * The CordbAppDomain object is about to be neutered due to an AD Unload
 * So the thread must no longer be considered to be in that domain.
 * Note that this is a hack due to the existance of the (possibly incorrect) 
 * cached AppDomain value.  Ideally we would remove the cached value entirely
 * and there would be no need for this.
 */
void CordbProcess::UpdateThreadsForAdUnload( CordbAppDomain* pAppDomain )
{
    INTERNAL_API_ENTRY(this);

    // If we're doing an AD unload then we should have already seen the ATTACH 
    // notification for the default domain.
    _ASSERTE( m_pDefaultAppDomain != NULL );
    
    RSLockHolder ch(&this->m_processMutex);
    
    CordbThread* t;
    HASHFIND find;

    for (t =  m_userThreads.FindFirst(&find);
         t != NULL;
         t =  m_userThreads.FindNext(&find))
    {
        if( t->GetAppDomain() == pAppDomain )
        {
            // This thread cannot actually be in this AppDomain anymore (since it's being 
            // unloaded).  Reset it to point to the default AppDomain
            t->m_pAppDomain = m_pDefaultAppDomain;
        }
    }
}

/*
 *  Look for a specific module in all of our AppDomains.  
 *  Prefer using CordbAppDomain::LookupModule.
 */
CordbModule *CordbProcess::LookupModule( LSPTR_DMODULE debuggerModuleToken )
{
    INTERNAL_API_ENTRY(this);
    
    HASHFIND findappdomain;
    CordbAppDomain* ad;
    CordbModule *pModule = NULL;

    for (ad =  m_appDomains.FindFirst(&findappdomain);
         ad != NULL;
         ad =  m_appDomains.FindNext(&findappdomain))
    {
        // Can't call CordbAppDomain::LookupModule because it does sanity checks to ensure the
        // module is infact in that domain.  Instead we do the lookup directly.
        pModule = ad->m_modules.GetBase(LsPtrToCookie(debuggerModuleToken));

        if (pModule)
            break;
    }

    return pModule;    
}

