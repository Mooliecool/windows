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
// File: RCThread.cpp
//
// Runtime Controller Thread
//
//*****************************************************************************

#include "stdafx.h"



#include "ipcmanagerinterface.h"
#include "eemessagebox.h"
#include "genericstackprobe.h"

#ifndef SM_REMOTESESSION
#define SM_REMOTESESSION 0x1000
#endif

#include <limits.h>

#ifdef _DEBUG
// Declare statics
EEThreadId DebuggerRCThread::s_DbgHelperThreadId;
#endif

//
// Constructor
//
DebuggerRCThread::DebuggerRCThread(Debugger* debugger)
    : m_debugger(debugger), m_rgDCB(NULL), m_thread(NULL), m_run(true),
      m_threadControlEvent(NULL),
      m_helperThreadCanGoEvent(NULL),
      m_fDetachRightSide(false),
      m_SetupSyncEvent(NULL)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        WRAPPER(THROWS);
        GC_NOTRIGGER;
        CONSTRUCTOR_CHECK;
    }
    CONTRACTL_END;

    _ASSERTE(debugger != NULL);

    for( int i = 0; i < IPC_TARGET_COUNT;i++)
        m_rgfInitRuntimeOffsets[i] = true;

    // Initialize this here because we Destroy it in the DTOR.
    // Note that this function can't fail.
}


//
// Destructor. Cleans up all of the open handles the RC thread uses.
// This expects that the RC thread has been stopped and has terminated
// before being called.
//
DebuggerRCThread::~DebuggerRCThread()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        DESTRUCTOR_CHECK;
    }
    CONTRACTL_END;

    LOG((LF_CORDB,LL_INFO1000, "DebuggerRCThread::~DebuggerRCThread\n"));

    // We explicitly leak the debugger object on shutdown. See Debugger::StopDebugger for details.
    _ASSERTE(!"RCThread dtor should not be called.");   
}

void DebuggerRCThread::CloseIPCHandles(IpcTarget iWhich)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    int i = (int)iWhich;

    if( m_rgDCB != NULL && m_rgDCB[i] != NULL)
    {
        m_rgDCB[i]->m_leftSideEventAvailable.Close();
        m_rgDCB[i]->m_leftSideEventRead.Close();
        m_rgDCB[i]->m_rightSideProcessHandle.Close();
    }
}

//-----------------------------------------------------------------------------
// Helper to get the proper decorated name
// Caller ensures that pBufSize is large enough. We'll assert just to check,
// but no runtime failure.
// pBuf - the output buffer to write the decorated name in
// cBufSizeInChars - the size of the buffer in characters, including the null.
// pPrefx - The undecorated name of the event.
//-----------------------------------------------------------------------------
void GetPidDecoratedName(__out_z __in_ecount(cBufSizeInChars) WCHAR * pBuf,
                         int cBufSizeInChars,
                         const WCHAR * pPrefix)
{
    LEAF_CONTRACT;

    DWORD pid = GetCurrentProcessId();

    GetPidDecoratedName(pBuf, cBufSizeInChars, pPrefix, pid);
}





//-----------------------------------------------------------------------------
// Create the SetupSyncEvent, which is shared with the out-of-process piece.
// Returns the status of the event. Our caller uses that to determine if
// we created the event or if the RS created it.
//-----------------------------------------------------------------------------
DWORD DebuggerRCThread::CreateSetupSyncEvent(void)
{
    CONTRACT(DWORD)
    {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(ThisMaybeHelperThread());
        PRECONDITION(m_SetupSyncEvent == NULL);
        POSTCONDITION(
            // (1) we failed to open/create the event
            (m_SetupSyncEvent == NULL) ||

            // (2) RS created it for us, we opened it.
            (m_SetupSyncEvent != NULL && RETVAL == ERROR_ALREADY_EXISTS) ||

            // (3) we created it first.
            (m_SetupSyncEvent != NULL && RETVAL == ERROR_SUCCESS)
        );
    }
    CONTRACT_END;

    WCHAR tmpName[256];

    // Attempt to create the Setup Sync event.
    GetPidDecoratedName(tmpName, NumItems(tmpName), CorDBIPCSetupSyncEventName);


    LOG((LF_CORDB, LL_INFO10000,
         "DRCT::I: creating setup sync event with name [%S]\n", tmpName));

    IPCHostSecurityAttributeHolder x(GetCurrentProcessId());
    // Both debugger +  debuggee race to create these event.
    // If event already exists, we requests the EVENT_ALL_ACCESS access right (and the intitial state + security attribute are ignored).
    m_SetupSyncEvent = WszCreateEvent(x.GetHostSA(), TRUE, FALSE, tmpName);
    DWORD dwStatus = GetLastError();


    if (dwStatus == ERROR_ACCESS_DENIED)
    {
        // try to use WszOpenEvent to see if we already have the privilege
        _ASSERTE(m_SetupSyncEvent  == NULL);

        // Security concern : isn't this a possible attack? Only our mscordbi will
        // create this event and the DACL should match, right? Unless it is a narrowing.
        //
        m_SetupSyncEvent = WszOpenEvent(EVENT_ALL_ACCESS, FALSE, tmpName);
        if (m_SetupSyncEvent)
        {
            // Set dwStatus to this value so that we will take the code path of
            // launch.
            //
            dwStatus = ERROR_ALREADY_EXISTS;
        }
    }

    // Do not fail because we cannot create the setup sync event.
    // This may fail to open if the event already exists, but this process has
    // insufficent privileges to open it.

    RETURN dwStatus;
}

HANDLE CreateWin32EventOrThrow(
    LPSECURITY_ATTRIBUTES lpEventAttributes,
    EEventResetType eType,
    BOOL bInitialState
)
{
    CONTRACT(HANDLE)
    {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(lpEventAttributes, NULL_OK));
        POSTCONDITION(RETVAL != NULL);
    }
    CONTRACT_END;

    HANDLE h = NULL;
    h = WszCreateEvent(lpEventAttributes, (BOOL) eType, bInitialState, NULL);

    if (h == NULL)
        ThrowLastError();

    RETURN h;
}

//-----------------------------------------------------------------------------
// Open an event. Another helper for DebuggerRCThread::Init
//-----------------------------------------------------------------------------
HANDLE OpenWin32EventOrThrow(
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    LPCWSTR lpName
)
{
    CONTRACT(HANDLE)
    {
        THROWS;
        GC_NOTRIGGER;
        POSTCONDITION(RETVAL != NULL);
    }
    CONTRACT_END;

    HANDLE h = WszOpenEvent(
        dwDesiredAccess,
        bInheritHandle,
        lpName
    );
    if (h == NULL)
        ThrowLastError();

    RETURN h;
}

//-----------------------------------------------------------------------------
// Holder for IPC SecurityAttribute
//-----------------------------------------------------------------------------
IPCHostSecurityAttributeHolder::IPCHostSecurityAttributeHolder(DWORD pid)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    m_pSA = NULL;

}

SECURITY_ATTRIBUTES * IPCHostSecurityAttributeHolder::GetHostSA()
{
    LEAF_CONTRACT;
    return m_pSA;
}


IPCHostSecurityAttributeHolder::~IPCHostSecurityAttributeHolder()
{
    LEAF_CONTRACT;

}


// NOTE The Init method works since there are no virtual functions - don't add any virtual functions without
// changing this!
// Initialize the IPC block.
// We assume ownership of the handles as soon as we're called; regardless of our success.
// On failure, we throw.
HRESULT DebuggerIPCControlBlock::Init(
    HANDLE rsea,
    HANDLE rser,
    HANDLE lsea,
    HANDLE lser,
    HANDLE lsuwe,
    HANDLE stilf
)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;

        PRECONDITION(DebuggerMaybeInvolved());
    }
    CONTRACTL_END;

    // NOTE this works since there are no virtual functions - don't add any without changing this!
    memset( this, 0, sizeof( DebuggerIPCControlBlock) );

    m_DCBSize = sizeof(DebuggerIPCControlBlock);

    // Setup version checking info.
    m_verMajor = VER_PRODUCTBUILD;
    m_verMinor = VER_PRODUCTBUILD_QFE;

#ifdef _DEBUG
    m_checkedBuild = true;
#else
    m_checkedBuild = false;
#endif
    m_bHostingInFiber = false;

    if (g_CORDebuggerControlFlags & DBCF_FIBERMODE)
    {
        m_bHostingInFiber = true;
    }

    // Copy RSEA and RSER into the control block.
    if (!m_rightSideEventAvailable.SetLocal(rsea))
        ThrowLastError();

    if (!m_rightSideEventRead.SetLocal(rser))
        ThrowLastError();

    if (!m_leftSideUnmanagedWaitEvent.SetLocal(lsuwe))
        ThrowLastError();

    if (!m_syncThreadIsLockFree.SetLocal(stilf))
        ThrowLastError();

    // Mark the debugger special thread list as not dirty, empty and null.
    m_specialThreadListDirty = false;
    m_specialThreadListLength = 0;
    m_specialThreadList = NULL;

    m_shutdownBegun = false;
    return S_OK;
}


enum DbgHelperThreadCreateOption
{
    DBG_HELPER_THREAD_CREATE_EAGERLY_ON_LAUNCH = 0,
    DBG_HELPER_THREAD_CREATE_EAGERLY_ALWAYS = 1,
    DBG_HELPER_THREAD_CREATE_EAGERLY_NEVER = 2
};

bool ShouldStartHelperThreadNow()
{

    DWORD dwDbgThreadCreateOption = DBG_HELPER_THREAD_CREATE_EAGERLY_ALWAYS;

    // defer creation as DbgThreadCreateOption specifies
    dwDbgThreadCreateOption = REGUTIL::GetConfigDWORD(L"DbgThreadCreateOption", dwDbgThreadCreateOption);

    switch (dwDbgThreadCreateOption)
    {
    case DBG_HELPER_THREAD_CREATE_EAGERLY_ON_LAUNCH:
        // early start only if under debugger
        return CORLaunchedByDebugger();
        break;
    case DBG_HELPER_THREAD_CREATE_EAGERLY_ALWAYS:
        // option 1, start always
        return true;
        break;
    case DBG_HELPER_THREAD_CREATE_EAGERLY_NEVER:
        // option 2, start never
        return false;
        break;
    default:
        _ASSERTE(!"BadThreadCreateOption");
    }

    // We can only get here if we have a bogus registry key setting, in which
    // case we should go with the safe bet and create the helper thread now
    return true;
}


void DebuggerRCThread::WatchForStragglers(void)
{
    WRAPPER_CONTRACT;

    _ASSERTE(m_threadControlEvent != NULL);
    LOG((LF_CORDB,LL_INFO100000, "DRCT::WFS:setting event to watch "
        "for stragglers\n"));

    SetEvent(m_threadControlEvent);
}


//
// Init sets up all the objects that the RC thread will need to run.
//
HRESULT DebuggerRCThread::Init(void)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(!ThisIsHelperThreadWorker()); // initialized by main thread
    }
    CONTRACTL_END;


    LOG((LF_CORDB, LL_EVERYTHING, "DebuggerRCThreadInit called\n"));
    HRESULT hr = S_OK;
    DWORD dwStatus;
    WCHAR tmpName[256];

    if (m_debugger == NULL)
        ThrowHR(E_INVALIDARG);

    // Init should only be called once.
    if (g_pRCThread != NULL)
        ThrowHR(E_FAIL);

    g_pRCThread = this;

    m_rgDCB = new DebuggerIPCControlBlock *[IPC_TARGET_COUNT];

    PREFIX_ASSUME(m_rgDCB != NULL); // throw on OOM

    memset( m_rgDCB, 0, sizeof(DebuggerIPCControlBlock *)*IPC_TARGET_COUNT);

    m_favorData.Init(); // throws


    // Create the thread control event.
    m_threadControlEvent = CreateWin32EventOrThrow(NULL, kAutoResetEvent, FALSE);

    // Create the helper thread can go event.
    m_helperThreadCanGoEvent = CreateWin32EventOrThrow(NULL, kManualResetEvent, TRUE);


    // We need to setup the shared memory and control block.
    // Get shared memory block from the IPCManager.
    if (g_pIPCManagerInterface == NULL)
    {
        LOG((LF_CORDB, LL_INFO10000, "DRCT::I: g_pIPCManagerInterface == NULL, can't create IPC Block\n"));
        ThrowHR(E_FAIL);
    }

    IPCHostSecurityAttributeHolder sa(GetCurrentProcessId());

    // Create the events that the thread will need to receive events
    // from the out of process piece on the right side.
    // We will not fail out if CreateEvent fails for RSEA or RSER. Because
    // the worst case is that debugger cannot attach to debuggee.
    //
    HandleHolder rightSideEventAvailable(WszCreateEvent(sa.GetHostSA(), (BOOL) kAutoResetEvent, FALSE, NULL));

    // Security fix:
    // We need to check the last error to see if the event was precreated or not
    // If so, we need to release the handle right now.
    //
    dwStatus = GetLastError();
    if (dwStatus == ERROR_ALREADY_EXISTS)
    {
        // clean up the handle now
        rightSideEventAvailable.Clear();
    }

    HandleHolder rightSideEventRead(WszCreateEvent(sa.GetHostSA(), (BOOL) kAutoResetEvent, FALSE, NULL));

    // Security fix:
    // We need to check the last error to see if the event was precreated or not
    // If so, we need to release the handle right now.
    //
    dwStatus = GetLastError();
    if (dwStatus == ERROR_ALREADY_EXISTS)
    {
        // clean up the handle now
        rightSideEventRead.Clear();
    }


    HandleHolder leftSideUnmanagedWaitEvent(CreateWin32EventOrThrow(NULL, kManualResetEvent, FALSE));
    HandleHolder syncThreadIsLockFree(CreateWin32EventOrThrow(NULL, kManualResetEvent, FALSE));

    m_rgDCB[IPC_TARGET_OUTOFPROC] = g_pIPCManagerInterface->GetDebugBlock();

    // Don't fail out because the SHM failed to create
#if _DEBUG
    if (m_rgDCB[IPC_TARGET_OUTOFPROC] == NULL)
    {
       LOG((LF_CORDB, LL_INFO10000,
             "DRCT::I: Failed to get Debug IPC block from IPCManager.\n"));
    }
#endif // _DEBUG

    // Copy RSEA and RSER into the control block only if SHM is created without error.
    if (m_rgDCB[IPC_TARGET_OUTOFPROC])
    {
        // Since Init() gets ownership of handles as soon as it's called, we can
        // release our ownership now.
        rightSideEventAvailable.SuppressRelease();
        rightSideEventRead.SuppressRelease();
        leftSideUnmanagedWaitEvent.SuppressRelease();
        syncThreadIsLockFree.SuppressRelease();

        hr = m_rgDCB[IPC_TARGET_OUTOFPROC]->Init(rightSideEventAvailable,
                                            rightSideEventRead,
                                            NULL,
                                            NULL,
                                            leftSideUnmanagedWaitEvent,
                                            syncThreadIsLockFree);
        _ASSERTE(SUCCEEDED(hr)); // throws on error.

        // We have to ensure that most of the runtime offsets for the out-of-proc DCB are initialized right away. This is
        // needed to support certian races during an interop attach. Since we can't know whether an interop attach will ever
        // happen or not, we are forced to do this now. Note: this is really too early, as some data structures haven't been
        // initialized yet!
        hr = EnsureRuntimeOffsetsInit(IPC_TARGET_OUTOFPROC);
        _ASSERTE(SUCCEEDED(hr)); // throw on error

        // Note: we have to mark that we need the runtime offsets re-initialized for the out-of-proc DCB. This is because
        // things like the patch table aren't initialized yet. Calling NeedRuntimeOffsetsReInit() ensures that this happens
        // before we really need the patch table.
        NeedRuntimeOffsetsReInit(IPC_TARGET_OUTOFPROC);

        m_rgDCB[IPC_TARGET_OUTOFPROC]->m_helperThreadStartAddr =
            (void *) DebuggerRCThread::ThreadProcStatic;

        m_rgDCB[IPC_TARGET_OUTOFPROC]->m_helperRemoteStartAddr =
            (void *) DebuggerRCThread::ThreadProcRemote;

        m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideProtocolCurrent = CorDB_LeftSideProtocolCurrent;
        m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideProtocolMinSupported = CorDB_LeftSideProtocolMinSupported;

        LOG((LF_CORDB, LL_INFO10,
             "DRCT::I: version info: %d.%d.%d current protocol=%d, min protocol=%d\n",
             m_rgDCB[IPC_TARGET_OUTOFPROC]->m_verMajor,
             m_rgDCB[IPC_TARGET_OUTOFPROC]->m_verMinor,
             m_rgDCB[IPC_TARGET_OUTOFPROC]->m_checkedBuild,
             m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideProtocolCurrent,
             m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideProtocolMinSupported));

        m_rgDCB[IPC_TARGET_OUTOFPROC]->m_rightSideShouldCreateHelperThread =
            !ShouldStartHelperThreadNow();
    }

    // Next we'll create the setup sync event for the right side - this
    // solves a race condition of "who gets to the setup code first?"
    // Since there's no guarantee that the thread executing managed
    // code will be executed after us.

    // this will throw on error.
    dwStatus = CreateSetupSyncEvent();


    if (dwStatus == ERROR_ALREADY_EXISTS)
    {
        // the event already exists. Degenerate into a launch case.
        LOG((LF_CORDB, LL_INFO10000, "DRCT::I: setup sync event already exists.\n"));

        g_pDebugger->LazyInit();

        // Need to do some delayed initialization of the debugger services.
        DebuggerController::Initialize();

        // Open LSEA and LSER (which would have been created by Right side)
        if (m_rgDCB[IPC_TARGET_OUTOFPROC])
        {
            GetPidDecoratedName(tmpName, NumItems(tmpName), CorDBIPCLSEventAvailName);
            if (!m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventAvailable.SetLocal(
                    OpenWin32EventOrThrow(EVENT_ALL_ACCESS, true, tmpName)))
                ThrowLastError();

            GetPidDecoratedName(tmpName, NumItems(tmpName), CorDBIPCLSEventReadName);
            if (!m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventRead.SetLocal(
                    OpenWin32EventOrThrow(EVENT_ALL_ACCESS, true, tmpName)))
                ThrowLastError();

            if (FAILED(VerifySecurityOnRSCreatedEvents(m_SetupSyncEvent, m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventAvailable, m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventRead)))
            {               
                m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventAvailable.Close();
                m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventRead.Close();
            }
        }



        // Need to validate the SSE *before* we wait on it.
        // Wait for the Setup Sync event before continuing.
        // We can never 100% guarantee this event is from a well-behaved debugger, so it
        // may not get signaled. Thus don't wait infinite else we'd get DOS of execution.
        // If we timeout, still execute the managed code, but we'll just have DOS of debugging.
        // The RS should create + signal the event immediately.        
        DWORD dwTimeoutMS = 5 * 1000;
        DWORD ret = WaitForSingleObject(m_SetupSyncEvent, dwTimeoutMS);

        if (ret == WAIT_TIMEOUT)
        {
            // Assume we're not debugging. We don't actually have to do anything else.
            // We'll just continue and execute.
        }
        else if (ret != WAIT_OBJECT_0)
        {
            ThrowLastError();
        }


        // We no longer need this event now.
        CloseHandle(m_SetupSyncEvent);
        m_SetupSyncEvent = NULL;


        LOG((LF_CORDB, LL_INFO10000,"DRCT::I: calling PAL_InitializeDebug.\n"));
        // Tell the PAL that we're trying to debug
        PAL_InitializeDebug();

        // At this point, the control block is complete and all four
        // events are available and valid for this process.

        // Since the sync event was created by the Right Side,
        // we'll need to mark the debugger as "attached."
        m_debugger->MarkDebuggerAttachedInternal();
    }
    else
    {
        LOG((LF_CORDB, LL_INFO10000, "DRCT::I: setup sync event was created.\n"));

        // At this point, only RSEA and RSER are in the control
        // block. LSEA and LSER will remain invalid until the first
        // receipt of an event from the Right Side.

        // Set the Setup Sync event to let the Right Side know that
        // we've finished setting up the control block.
        if (m_SetupSyncEvent)
        {
            SetEvent(m_SetupSyncEvent);
        }
    }

    return S_OK;
}

// This function is used to verify the security descriptor on an event
// matches our expectation to prevent attack. This should be called when
// we opened an event by name and assumed that the RS creates the event.
// That means the event's dacl should match our default policy - current user
// and admin. It can be narrower. By default, the DACL looks like the debugger
// process user, debuggee user, and admin.
//
HRESULT DebuggerRCThread::VerifySecurityOnRSCreatedEvents(
    HANDLE  sse,
    HANDLE  lsea,
    HANDLE  lser)
{
    return S_OK;
}

//
// Setup the Runtime Offsets struct.
//
HRESULT DebuggerRCThread::SetupRuntimeOffsets(DebuggerIPCControlBlock *pDCB)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(ThisMaybeHelperThread());
        PRECONDITION(DebuggerMaybeInvolved());
    }
    CONTRACTL_END;

    // Allocate the struct if needed. We just fill in any existing one.
    DebuggerIPCRuntimeOffsets *pRO = pDCB->m_runtimeOffsets;

    if (pRO == NULL)
    {
        // Perhaps we should preallocate this. This is the only allocation
        // that would force SendIPCEvent to throw an exception. It'd be very
        // nice to have
        CONTRACT_VIOLATION(ThrowsViolation);
        pRO = new DebuggerIPCRuntimeOffsets();
        _ASSERTE(pRO != NULL); // throws on oom
    }

    // Fill out the struct.

    pRO->m_pPatches = DebuggerController::GetPatchTable();
    pRO->m_pPatchTableValid = DebuggerController::GetPatchTableValidAddr();
    pRO->m_offRgData = DebuggerPatchTable::GetOffsetOfEntries();
    pRO->m_offCData = DebuggerPatchTable::GetOffsetOfCount();
    pRO->m_cbPatch = sizeof(DebuggerControllerPatch);
    pRO->m_offAddr = offsetof(DebuggerControllerPatch, address);
    pRO->m_offOpcode = offsetof(DebuggerControllerPatch, opcode);
    pRO->m_cbOpcode = sizeof(PRD_TYPE);
    pRO->m_offTraceType = offsetof(DebuggerControllerPatch, trace.type);
    pRO->m_traceTypeUnmanaged = TRACE_UNMANAGED;

    g_pEEInterface->GetRuntimeOffsets(&pRO->m_TLSIndex,
                                      &pRO->m_TLSIsSpecialIndex,
                                      &pRO->m_TLSCantStopIndex,
                                      &pRO->m_TLSIndexOfPredefs,
                                      &pRO->m_EEThreadStateOffset,
                                      &pRO->m_EEThreadStateNCOffset,
                                      &pRO->m_EEThreadPGCDisabledOffset,
                                      &pRO->m_EEThreadPGCDisabledValue,
                                      &pRO->m_EEThreadDebuggerWordOffset,
                                      &pRO->m_EEThreadFrameOffset,
                                      &pRO->m_EEThreadMaxNeededSize,
                                      &pRO->m_EEThreadSteppingStateMask,
                                      &pRO->m_EEMaxFrameValue,
                                      &pRO->m_EEThreadDebuggerFilterContextOffset,
                                      &pRO->m_EEThreadCantStopOffset,
                                      &pRO->m_EEFrameNextOffset,
                                      &pRO->m_EEIsManagedExceptionStateMask);

    _ASSERTE((pRO->m_TLSIndexOfPredefs != 0) || !"CExecutionEngine::TlsIndex is not initialized yet");

    // Remember the struct in the control block.
    pDCB->m_runtimeOffsets = pRO;

    return S_OK;
}

// Filter called when we throw an exception while Handling events.
static LONG _debugFilter(LPEXCEPTION_POINTERS ep, PVOID pv)
{
    LOG((LF_CORDB, LL_INFO10,
         "Unhandled exception in Debugger::HandleIPCEvent\n"));

    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;

    DebuggerIPCEvent *event = (DebuggerIPCEvent *)pv;

    DWORD pid = GetCurrentProcessId();
    DWORD tid = GetCurrentThreadId();

    DebuggerIPCEventType type = (DebuggerIPCEventType) (event->type & DB_IPCE_TYPE_MASK);

    // We should never AV here. In a debug build, throw up an assert w/ lots of useful (private) info.
#ifdef _DEBUG
    {
        // We can't really use SStrings on the helper thread; though if we're at this point, we've already died.
        // So go ahead and risk it and use them anyways.
        SString sStack;
        StackScratchBuffer buffer;
        GetStackTraceAtContext(sStack, ep->ContextRecord);
        const CHAR *string = NULL;

        EX_TRY
        {
            string = sStack.GetANSI(buffer);
        }
        EX_CATCH
        {
            string = "*Could not retrieve stack*";
        }
        EX_END_CATCH(RethrowTerminalExceptions);

        CONSISTENCY_CHECK_MSGF(false,
            ("Unhandled exception on the helper thread.\nEvent=%s(0x%x)\nCode=0x%0x, Ip=0x%p, .cxr=%p, .exr=%p.\n pid=0x%x (%d), tid=0x%x (%d).\n-----\nStack of exception:\n%s\n----\n",
            IPCENames::GetName(type), type,
            ep->ExceptionRecord->ExceptionCode, GetIP(ep->ContextRecord), ep->ContextRecord, ep->ExceptionRecord,
            pid, pid, tid, tid,
            string));
    }
#endif
    static int breakOnAV = -1;

    if (breakOnAV == -1)
        breakOnAV = UnsafeGetConfigDWORD(L"BreakOnAV", 0);

    if (breakOnAV != 0 && ep->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION)
#ifdef _DEBUG
        _ASSERTE(!"AV occured");
#else
    if (UnsafeGetConfigDWORD(L"EHGolden", 0))
            DebugBreak();
#endif

    // We took an AV on the helper thread. This is a catastrophic situation so we can
    // simply call the EE's catastrophic message box to display the error.
    EEMessageBoxCatastrophic(
        IDS_DEBUG_UNHANDLEDEXCEPTION_IPC, IDS_DEBUG_SERVICE_CAPTION,
        type,
        ep->ExceptionRecord->ExceptionCode,
        GetIP(ep->ContextRecord),
        pid, pid, tid, tid);

    // For debugging, we can change the behavior by manually setting eax.
    // EXCEPTION_EXECUTE_HANDLER=1, EXCEPTION_CONTINUE_SEARCH=0, EXCEPTION_CONTINUE_EXECUTION=-1
    return EXCEPTION_CONTINUE_SEARCH;
}

#ifdef _DEBUG


void AssertAllocationAllowed()
{
}
#endif


//
// Primary function of the Runtime Controller thread. First, we let
// the Debugger Interface know that we're up and running. Then, we run
// the main loop.
//
void DebuggerRCThread::ThreadProc(void)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_TRIGGERS;        // Debugger::SuspendComplete can trigger GC

        // Although we're the helper thread, we haven't set it yet.
        DISABLED(PRECONDITION(ThisIsHelperThreadWorker()));

        PRECONDITION(DebuggerMaybeInvolved());
        INSTANCE_CHECK;
    }
    CONTRACTL_END;

    STRESS_LOG_RESERVE_MEM (0);
    // This message actually serves a purpose (which is why it is always run)
    // The Stress log is run during hijacking, when other threads can be suspended
    // at arbitrary locations (including when holding a lock that NT uses to serialize
    // all memory allocations).  By sending a message now, we insure that the stress
    // log will not allocate memory at these critical times an avoid deadlock.
    {
        SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
        STRESS_LOG0(LF_CORDB|LF_ALWAYS, LL_ALWAYS, "Debugger Thread spinning up\n");

        // Call this to force creation of the TLS slots on helper-thread.
        IsDbgHelperSpecialThread();
    }    

#ifdef _DEBUG
    // Track the helper thread.
    s_DbgHelperThreadId.SetThreadId();
#endif
    CantAllocHolder caHolder;


#ifdef _DEBUG
    // Cause wait in the helper thread startup. This lets us test against certain races.
    // 1 = 6 sec. (shorter than Poll)
    // 2 = 12 sec (longer than Poll).
    // 3 = infinite - never comes up.
    static int fDelayHelper = -1;

    if (fDelayHelper == -1)
        fDelayHelper = UnsafeGetConfigDWORD(L"DbgDelayHelper", 0);

    if (fDelayHelper)
    {
        DWORD sleep = 6000;

        switch(fDelayHelper)
        {
            case 1: sleep =  6000; break;
            case 2: sleep = 12000; break;
            case 3: sleep = INFINITE; break;
        }

        ClrSleepEx(sleep, FALSE);
    }
#endif

    LOG((LF_CORDB, LL_INFO1000, "DRCT::TP: helper thread spinning up...\n"));

    // In case the SHM is not initialized properly, it will be noop
    if (m_rgDCB[IPC_TARGET_OUTOFPROC] == NULL)
        return;

    // Lock the debugger before spinning up.
    Debugger::DebuggerLockHolder debugLockHolder(m_debugger);

    if (m_rgDCB[IPC_TARGET_OUTOFPROC]->m_helperThreadId != 0)
    {
        // someone else has created a helper thread, we're outta here
        // the most likely scenario here is that there was some kind of
        // race between remotethread creation and localthread creation

        LOG((LF_CORDB, LL_EVERYTHING, "Second debug helper thread creation detected, thread will safely suicide\n"));
        // dbgLockHolder goes out of scope - implicit Release
        return;
    }

    // this thread took the lock and there is no existing m_helperThreadID therefore
    // this *IS* the helper thread and nobody else can be the helper thread

    // the handle was created by the Start method
    _ASSERTE(m_thread != NULL);

#ifdef _DEBUG
    // Make sure that we have the proper permissions.
    {
        int ret = WaitForSingleObject(m_thread, 0);
        _ASSERTE(ret == WAIT_TIMEOUT);
    }
#endif

    // Mark that we're the true helper thread. Now that we've marked
    // this, no other threads will ever become the temporary helper
    // thread.
    m_rgDCB[IPC_TARGET_OUTOFPROC]->m_helperThreadId = GetCurrentThreadId();

    LOG((LF_CORDB, LL_INFO1000, "DRCT::TP: helper thread id is 0x%x helperThreadId\n",
         m_rgDCB[IPC_TARGET_OUTOFPROC]->m_helperThreadId));

    // If there is a temporary helper thread, then we need to wait for
    // it to finish being the helper thread before we can become the
    // helper thread.
    if (m_rgDCB[IPC_TARGET_OUTOFPROC]->m_temporaryHelperThreadId != 0)
    {
        LOG((LF_CORDB, LL_INFO1000,
             "DRCT::TP: temporary helper thread 0x%x is in the way, "
             "waiting...\n",
             m_rgDCB[IPC_TARGET_OUTOFPROC]->m_temporaryHelperThreadId));

        debugLockHolder.Release();

        // Wait for the temporary helper thread to finish up.
        DWORD ret;
        ret = WaitForSingleObject(m_helperThreadCanGoEvent, INFINITE);

        LOG((LF_CORDB, LL_INFO1000,
             "DRCT::TP: done waiting for temp help to finish up.\n"));

        _ASSERTE(ret == WAIT_OBJECT_0);
        _ASSERTE(m_rgDCB[IPC_TARGET_OUTOFPROC]->m_temporaryHelperThreadId==0);
    }
    else
    {
        LOG((LF_CORDB, LL_INFO1000,
             "DRCT::TP: no temp help in the way...\n"));

        debugLockHolder.Release();
    }

    // Run the main loop as the true helper thread.
    MainLoop();
}

void DebuggerRCThread::RightSideDetach(void)
{
    _ASSERTE( m_fDetachRightSide == false );
    m_fDetachRightSide = true;
    CloseIPCHandles(IPC_TARGET_OUTOFPROC);
}

//
// These defines control how many times we spin while waiting for threads to sync and how often. Note its higher in
// debug builds to allow extra time for threads to sync.
//
#define CorDB_SYNC_WAIT_TIMEOUT  20   // 20ms

#ifdef _DEBUG
#define CorDB_MAX_SYNC_SPIN_COUNT (10000 / CorDB_SYNC_WAIT_TIMEOUT)  // (10 seconds)
#else
#define CorDB_MAX_SYNC_SPIN_COUNT (3000 / CorDB_SYNC_WAIT_TIMEOUT)   // (3 seconds)
#endif

bool HandleIPCEventWrapper(Debugger* pDebugger, DebuggerIPCEvent *e, IpcTarget iWhich)
{
    bool wasContinue = false;

    PAL_TRY
    {
        wasContinue = pDebugger->HandleIPCEvent(e, iWhich);
    }
    PAL_EXCEPT_FILTER(_debugFilter, e)
    {
        LOG((LF_CORDB, LL_INFO10, "Unhandled exception caught in Debugger::HandleIPCEvent\n"));
    }
    PAL_ENDTRY

    return wasContinue;
}

bool DebuggerRCThread::HandleRSEA()
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        if (g_pEEInterface->GetThread() != NULL) { GC_TRIGGERS; } else { GC_NOTRIGGER; }
        PRECONDITION(ThisIsHelperThreadWorker());
    }
    CONTRACTL_END;

    SIZE_T iWhich = IPC_TARGET_OUTOFPROC;

    LOG((LF_CORDB,LL_INFO10000, "RSEA from out of process (right side)\n"));

    // If this is our first event, we may need to init some stuff.
    {
        if (m_rgDCB[iWhich]->m_DebuggerVersion < CorDebugVersion_2_0)
        {
            g_EnableSIS = false;
        }
    }

    // Make room for any Right Side event on the stack.
    BYTE buffer[CorDBIPC_BUFFER_SIZE];
    DebuggerIPCEvent * e = (DebuggerIPCEvent *) buffer;

    // If the RSEA is signaled, then handle the event from the Right Side.
    memcpy(e, GetIPCEventReceiveBuffer((IpcTarget)iWhich), CorDBIPC_BUFFER_SIZE);

    // If no reply is required, then let the Right Side go since we've got a copy of the event now.
    _ASSERTE(!e->asyncSend || !e->replyRequired);

    if (!e->replyRequired && !e->asyncSend)
    {
        LOG((LF_CORDB, LL_INFO1000, "DRCT::ML: no reply required, letting Right Side go.\n"));

        BOOL succ = SetEvent(m_rgDCB[iWhich]->m_rightSideEventRead);

        if (!succ)
            CORDBDebuggerSetUnrecoverableWin32Error(m_debugger, 0, true);
    }
#ifdef LOGGING
    else if (e->asyncSend)
        LOG((LF_CORDB, LL_INFO1000, "DRCT::ML: async send.\n"));
    else
        LOG((LF_CORDB, LL_INFO1000, "DRCT::ML: reply required, holding Right Side...\n"));
#endif

    // Pass the event to the debugger for handling. Returns true if the event was a Continue event and we can
    // stop looking for stragglers.  We wrap this whole thing in an exception handler to help us debug faults.
    bool wasContinue = false;

    wasContinue = HandleIPCEventWrapper(m_debugger, e, (IpcTarget)iWhich);

    return wasContinue;
}

//
// Main loop of the Runtime Controller thread. It waits for IPC events
// and dishes them out to the Debugger object for processing.
//
// Some of this logic is copied in Debugger::VrpcToVls
//
void DebuggerRCThread::MainLoop()
{
    // This function can only be called on native Debugger helper thread.
    //

    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;

        PRECONDITION(m_thread != NULL);
        PRECONDITION(ThisIsHelperThreadWorker());
        PRECONDITION(DebuggerMaybeInvolved());
        PRECONDITION(IsDbgHelperSpecialThread());   // Can only be called on native debugger helper thread
        PRECONDITION((!ThreadStore::HoldingThreadStore()) || g_fProcessDetach);
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000, "DRCT::ML:: running main loop\n"));

    // Anbody doing helper duty is in a can't-stop range, period.
    // Our helper thread is already in a can't-stop range, so this is particularly useful for
    // threads doing helper duty.
    CantStopHolder hCantStop;

    HANDLE waitSet[DRCT_COUNT_FINAL];

#ifdef _DEBUG
    DWORD dbg_syncSpinCount = 0;
#endif

    // We start out just listening on RSEA and the thread control event...
    unsigned int waitCount = DRCT_COUNT_INITIAL;
    DWORD waitTimeout = INFINITE;
    waitSet[DRCT_CONTROL_EVENT] = m_threadControlEvent;
    waitSet[DRCT_RSEA] = m_rgDCB[IPC_TARGET_OUTOFPROC]->m_rightSideEventAvailable;
    waitSet[DRCT_FAVORAVAIL] = GetFavorAvailableEvent();

    CONTRACT_VIOLATION(ThrowsViolation);// HndCreateHandle throws, and this loop is not backstopped by any EH

    // Lock holder. Don't take it yet. We take lock on this when we succeeded suspended runtime.
    // We will release the lock later when continue happens and runtime resumes
    Debugger::DebuggerLockHolder debugLockHolderSuspended(m_debugger, false);

    while (m_run)
    {
        LOG((LF_CORDB, LL_INFO1000, "DRCT::ML: waiting for event.\n"));

        // If there is a debugger attached, wait on its handle, too...
        if (waitCount == DRCT_COUNT_INITIAL
            && m_rgDCB[IPC_TARGET_OUTOFPROC]->m_rightSideProcessHandle.ImportToLocalProcess() != NULL)
        {
            _ASSERTE((waitCount + 1) == DRCT_COUNT_FINAL);
            waitSet[DRCT_DEBUGGER_EVENT] = m_rgDCB[IPC_TARGET_OUTOFPROC]->m_rightSideProcessHandle;
            waitCount = DRCT_COUNT_FINAL;
        }

        if (m_fDetachRightSide)
        {
            m_fDetachRightSide = false;

            _ASSERTE(waitCount == DRCT_COUNT_FINAL);
            _ASSERTE((waitCount-1) == DRCT_COUNT_INITIAL);

            waitSet[DRCT_DEBUGGER_EVENT] = NULL;
            waitCount = DRCT_COUNT_INITIAL;
        }

        // Wait for an event from the Right Side.
        DWORD ret = WaitForMultipleObjects(waitCount, waitSet, FALSE, waitTimeout);

        if (!m_run)
            continue;

        if (ret == WAIT_OBJECT_0 + DRCT_DEBUGGER_EVENT)
        {
            // If the handle of the right side process is signaled, then we've lost our controlling debugger. We
            // terminate this process immediatley in such a case.
            LOG((LF_CORDB, LL_INFO1000, "DRCT::ML: terminating this process. Right Side has exited.\n"));
            SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
            EEPOLICY_HANDLE_FATAL_ERROR(0);
            _ASSERTE(!"Should never reach this point.");
        }

        else if (ret == WAIT_OBJECT_0 + DRCT_FAVORAVAIL)
        {
            // execute the callback set by DoFavor()
            (*GetFavorFnPtr())(GetFavorData());

            SetEvent(GetFavorReadEvent());
        }

        else if (ret == WAIT_OBJECT_0 + DRCT_RSEA)
        {
            bool wasContinue = HandleRSEA();

            if (wasContinue)
            {

                // If they called continue, then we must have released the TSL.
                _ASSERTE(!ThreadStore::HoldingThreadStore() || g_fProcessDetach);

                // Let's release the lock here since runtime is resumed.
                debugLockHolderSuspended.Release();

                // This debugger thread shoud not be holding debugger locks anymore
                _ASSERTE(!g_pDebugger->ThreadHoldsLock());
#ifdef _DEBUG
                // Always reset the syncSpinCount to 0 on a continue so that we have the maximum number of possible
                // spins the next time we need to sync.
                dbg_syncSpinCount = 0;
#endif

                if (waitTimeout != INFINITE)
                {
                    LOG((LF_CORDB, LL_INFO1000, "DRCT::ML:: don't check for stragglers due to continue.\n"));

                    waitTimeout = INFINITE;
                }

            }
        }
        else if (ret == WAIT_OBJECT_0 + DRCT_CONTROL_EVENT)
        {
            LOG((LF_CORDB, LL_INFO1000, "DRCT::ML:: straggler event set.\n"));

            Debugger::DebuggerLockHolder debugLockHolder(m_debugger);
            // Make sure that we're still synchronizing...
            if (m_debugger->IsSynchronizing())
            {
                LOG((LF_CORDB, LL_INFO1000, "DRCT::ML:: dropping the timeout.\n"));

                waitTimeout = CorDB_SYNC_WAIT_TIMEOUT;

                //
                // Skip waiting the first time and just give it a go.  Note: Implicit
                // release of the lock, because we are leaving its scope.
                //
                goto LabelWaitTimedOut;
            }
#ifdef LOGGING
            else
                LOG((LF_CORDB, LL_INFO1000, "DRCT::ML:: told to wait, but not syncing anymore.\n"));
#endif
            // dbgLockHolder goes out of scope - implicit Release
         }
        else if (ret == WAIT_TIMEOUT)
        {

LabelWaitTimedOut:

            LOG((LF_CORDB, LL_INFO1000, "DRCT::ML:: wait timed out.\n"));

            // Debugger::DebuggerLockHolder debugLockHolder(m_debugger);
            // Explicitly get the lock here since we try to check to see if
            // have suspended.  We will release the lock if we are not suspended yet.
            //
            debugLockHolderSuspended.Acquire();

            // We should still be synchronizing, otherwise we would not have timed out.
            _ASSERTE(m_debugger->IsSynchronizing());

            LOG((LF_CORDB, LL_INFO1000, "DRCT::ML:: sweeping the thread list.\n"));

#ifdef _DEBUG
            // If we fail to suspend the CLR, don't bother waiting for a BVT to timeout,
            // fire up an assert up now.
            // Threads::m_DebugWillSyncCount+1 is the number of outstanding threads.
            // We're trying to suspend any thread w/ TS_DebugWillSync set.
            if (dbg_syncSpinCount++ > CorDB_MAX_SYNC_SPIN_COUNT)
            {
                _ASSERTE(false || !"Timeout trying to suspend CLR for debugging. Possibly a deadlock. "
                "You can ignore this assert to continue waiting\n");
                dbg_syncSpinCount = 0;
            }
#endif

            // Don't call Sweep if we're doing helper thread duty.
            // If we're doing helper thread duty, then we already Suspended the CLR, and we already hold the TSL.
            bool fSuspended;
            {
                // SweepThreadsForDebug() may call new!!! ARGG!!!
                SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
                fSuspended = g_pEEInterface->SweepThreadsForDebug(false);
            }

            if (fSuspended)
            {
                STRESS_LOG0(LF_CORDB, LL_INFO1000, "DRCT::ML:: wait set empty after sweep.\n");

                // There are no more threads to wait for, so go ahead and send the sync complete event.
                m_debugger->SuspendComplete();
                waitTimeout = INFINITE;

                // Note: we hold the thread store lock now and debugger lock...

                // We also hold debugger lock the whole time that Runtime is stopped. We will release the debugger lock
                // when we receive the Continue event that resumes the runtime.

                _ASSERTE(ThreadStore::HoldingThreadStore() || g_fProcessDetach);
            }
            else
            {
                // If we're doing helper thread duty, then we expect to have been suspended already.
                // And so the sweep should always succeed.
                STRESS_LOG0(LF_CORDB, LL_INFO1000, "DRCT::ML:: threads still syncing after sweep.\n");
                debugLockHolderSuspended.Release();
            }
            // debugLockHolderSuspended does not go out of scope. It has to be either released explicitly on the line above or
            // we intend to hold the lock till we hit continue event.

        }
    }

    STRESS_LOG0(LF_CORDB, LL_INFO1000, "DRCT::ML:: Exiting.\n");
}




//
// Main loop of the temporary Helper thread. It waits for IPC events
// and dishes them out to the Debugger object for processing.
//
// When we enter here, we are holding debugger lock and thread store lock.
// The debugger lock was SuppressRelease in DoHelperThreadDuty. The continue event
// that we are waiting for will trigger the corresponding release.
//
// IMPORTANT!!! READ ME!!!!
// This MainLoop is similiar to MainLoop function above but simplified to deal with only
// some scenario. So if you change here, you should look at MainLoop to see if same change is
// required.
//
void DebuggerRCThread::TemporaryHelperThreadMainLoop()
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;

        PRECONDITION(DebuggerMaybeInvolved());

        // If we come in here, this managed thread is trying to do helper thread duty.
        // It should be holding the debugger lock!!!
        //
        PRECONDITION(m_debugger->ThreadHoldsLock());
        PRECONDITION((ThreadStore::HoldingThreadStore()) || g_fProcessDetach);
        PRECONDITION(ThisIsTempHelperThread());
    }
    CONTRACTL_END;

    STRESS_LOG0(LF_CORDB, LL_INFO1000, "DRCT::THTML:: Doing helper thread duty, running main loop.\n");
    // Anbody doing helper duty is in a can't-stop range, period.
    // Our helper thread is already in a can't-stop range, so this is particularly useful for
    // threads doing helper duty.
    CantStopHolder hCantStop;

    HANDLE waitSet[DRCT_COUNT_FINAL];

#ifdef _DEBUG
    DWORD dbg_syncSpinCount = 0;
#endif

    // We start out just listening on RSEA and the thread control event...
    unsigned int waitCount = DRCT_COUNT_INITIAL;
    DWORD waitTimeout = INFINITE;
    waitSet[DRCT_CONTROL_EVENT] = m_threadControlEvent;
    waitSet[DRCT_RSEA] = m_rgDCB[IPC_TARGET_OUTOFPROC]->m_rightSideEventAvailable;
    waitSet[DRCT_FAVORAVAIL] = GetFavorAvailableEvent();

    CONTRACT_VIOLATION(ThrowsViolation);// HndCreateHandle throws, and this loop is not backstopped by any EH

    while (m_run)
    {
        LOG((LF_CORDB, LL_INFO1000, "DRCT::ML: waiting for event.\n"));

        // Wait for an event from the Right Side.
        DWORD ret = WaitForMultipleObjects(waitCount, waitSet, FALSE, waitTimeout);

        if (!m_run)
            continue;

        if (ret == WAIT_OBJECT_0 + DRCT_DEBUGGER_EVENT)
        {
            // If the handle of the right side process is signaled, then we've lost our controlling debugger. We
            // terminate this process immediatley in such a case.
            LOG((LF_CORDB, LL_INFO1000, "DRCT::THTML: terminating this process. Right Side has exited.\n"));

            TerminateProcess(GetCurrentProcess(), 0);
            _ASSERTE(!"Should never reach this point.");
        }

        else if (ret == WAIT_OBJECT_0 + DRCT_FAVORAVAIL)
        {
            // execute the callback set by DoFavor()
            (*GetFavorFnPtr())(GetFavorData());

            SetEvent(GetFavorReadEvent());
        }

        else if (ret == WAIT_OBJECT_0 + DRCT_RSEA)
        {
            bool wasContinue = HandleRSEA();

            if (wasContinue)
            {
                // If they called continue, then we must have released the TSL.
                _ASSERTE(!ThreadStore::HoldingThreadStore() || g_fProcessDetach);

#ifdef _DEBUG
                // Always reset the syncSpinCount to 0 on a continue so that we have the maximum number of possible
                // spins the next time we need to sync.
                dbg_syncSpinCount = 0;
#endif

                // HelperThread duty is finished. We have got a Continue message
                goto Exit;
            }
        }
        else if (ret == WAIT_OBJECT_0 + DRCT_CONTROL_EVENT)
        {
            LOG((LF_CORDB, LL_INFO1000, "DRCT::THTML:: straggler event set.\n"));

            // Make sure that we're still synchronizing...
            _ASSERTE(m_debugger->IsSynchronizing());
            LOG((LF_CORDB, LL_INFO1000, "DRCT::THTML:: dropping the timeout.\n"));

            waitTimeout = CorDB_SYNC_WAIT_TIMEOUT;

            //
            // Skip waiting the first time and just give it a go.  Note: Implicit
            // release of the lock, because we are leaving its scope.
            //
            goto LabelWaitTimedOut;
         }
        else if (ret == WAIT_TIMEOUT)
        {

LabelWaitTimedOut:

            LOG((LF_CORDB, LL_INFO1000, "DRCT::THTML:: wait timed out.\n"));

            // We should still be synchronizing, otherwise we would not have timed out.
            _ASSERTE(m_debugger->IsSynchronizing());

            LOG((LF_CORDB, LL_INFO1000, "DRCT::THTML:: sweeping the thread list.\n"));

#ifdef _DEBUG
            // If we fail to suspend the CLR, don't bother waiting for a BVT to timeout,
            // fire up an assert up now.
            // Threads::m_DebugWillSyncCount+1 is the number of outstanding threads.
            // We're trying to suspend any thread w/ TS_DebugWillSync set.
            if (dbg_syncSpinCount++ > CorDB_MAX_SYNC_SPIN_COUNT)
            {
                _ASSERTE(false || !"Timeout trying to suspend CLR for debugging. Possibly a deadlock. "
                "You can ignore this assert to continue waiting\n");
                dbg_syncSpinCount = 0;
            }
#endif

            STRESS_LOG0(LF_CORDB, LL_INFO1000, "DRCT::THTML:: wait set empty after sweep.\n");

            // We are holding Debugger lock (Look at the SuppressRelease on the DoHelperThreadDuty)
            // The debugger lock will be released on the Continue event which we will then
            // exit the loop.

            // There are no more threads to wait for, so go ahead and send the sync complete event.
            m_debugger->SuspendComplete();
            waitTimeout = INFINITE;

            // Note: we hold the thread store lock now and debugger lock...
            _ASSERTE(ThreadStore::HoldingThreadStore() || g_fProcessDetach);

        }
    }

Exit:

    STRESS_LOG0(LF_CORDB, LL_INFO1000, "DRCT::THTML:: Exiting.\n");
}



//
// This is the thread's real thread proc. It simply calls to the
// thread proc on the RCThread object.
//
/*static*/ DWORD WINAPI DebuggerRCThread::ThreadProcRemote(LPVOID)
{
    // We just wrap create a local thread and we're outta here
    WRAPPER_CONTRACT;

    ClrFlsSetThreadType(ThreadType_DbgHelper);

    LOG((LF_CORDB, LL_EVERYTHING, "ThreadProcRemote called\n"));
#ifdef _DEBUG
    dbgOnly_IdentifySpecialEEThread();
#endif

    // this method can be called both by a local createthread or a remote create thread
    // so we must use the g_RCThread global to find the (unique!) this pointer
    // we cannot count on the parameter.

    DebuggerRCThread* t = (DebuggerRCThread*)g_pRCThread;

    // This remote thread is created by the debugger process
    // and so its ACLs will reflect permissions for the user running
    // the debugger. If this process is running in the context of a
    // different user then this (the now running) process will not be
    // able to do operations on that (remote) thread.
    //
    // To avoid this problem, if we are the remote thread, then
    // we simply launch a new, local, thread right here and let
    // the remote thread die.  This new thread is created the same
    // way as always, and since it is created by this process
    // this process will be able to synchronize with it and so forth

    t->Start();  // this thread is remote, we must start a new thread

    return 0;
}

//
// This is the thread's real thread proc. It simply calls to the
// thread proc on the RCThread object.
//
/*static*/ DWORD WINAPI DebuggerRCThread::ThreadProcStatic(LPVOID)
{
    // We just wrap the instance method DebuggerRCThread::ThreadProc
    WRAPPER_CONTRACT;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD_FORCE_SO();

    ClrFlsSetThreadType(ThreadType_DbgHelper);

    LOG((LF_CORDB, LL_EVERYTHING, "ThreadProcStatic called\n"));

#ifdef _DEBUG
    dbgOnly_IdentifySpecialEEThread();
#endif


    DebuggerRCThread* t = (DebuggerRCThread*)g_pRCThread;

    t->ThreadProc(); // this thread is local, go and become the helper
    
    END_SO_INTOLERANT_CODE;

    return 0;
}

RCThreadLazyInit * DebuggerRCThread::GetLazyData()
{
    return g_pDebugger->GetRCThreadLazyData();
}


//
// Start actually creates and starts the RC thread. It waits for the thread
// to come up and perform initial synchronization with the Debugger
// Interface before returning.
//
HRESULT DebuggerRCThread::Start(void)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    LOG((LF_CORDB, LL_EVERYTHING, "DebuggerRCThread::Start called...\n"));

    DWORD helperThreadId;

    if (m_thread != NULL)
    {
       LOG((LF_CORDB, LL_EVERYTHING, "DebuggerRCThread::Start declined to start another helper thread...\n"));
       return S_OK;
    }

    Debugger::DebuggerLockHolder debugLockHolder(m_debugger);

    if (m_thread == NULL)
    {
        // Create suspended so that we can sniff the tid before the thread actually runs.
        // This may not be before the native thread-create event, but should be before everything else.
        // Note: strange as it may seem, the Right Side depends on us
        // using CreateThread to create the helper thread here. If you
        // ever change this to some other thread creation routine, you
        // need to update the logic in process.cpp where we discover the
        // helper thread on CREATE_THREAD_DEBUG_EVENTs...
        m_thread = CreateThread(NULL, 0, DebuggerRCThread::ThreadProcStatic,
                                NULL, CREATE_SUSPENDED, &helperThreadId );

        if (m_thread == NULL)
        {
            LOG((LF_CORDB, LL_EVERYTHING, "DebuggerRCThread failed, err=%d\n", GetLastError()));
            hr = HRESULT_FROM_GetLastError();

        }
        else
        {
            LOG((LF_CORDB, LL_EVERYTHING, "DebuggerRCThread start was successful, id=%d\n", helperThreadId));
        }

        // This gets published immediately.
        DebuggerIPCControlBlock* dcb = GetDCB(IPC_TARGET_OUTOFPROC);
        PREFIX_ASSUME(dcb != NULL);
        dcb->m_realHelperThreadId = helperThreadId;

#ifdef _DEBUG
        // Record the OS Thread ID for debugging purposes.
        m_DbgHelperThreadOSTid = helperThreadId ;
#endif

        if (m_thread != NULL)
        {
            ResumeThread(m_thread);
        }

    }

    // unlock debugger lock is implied.

    return hr;
}


//
// Stop causes the RC thread to stop receiving events and exit. 
// It does not wait for it to exit before returning (hence "AsyncStop" instead of "Stop").
//
HRESULT DebuggerRCThread::AsyncStop(void)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;

        // Win9x, a random thread is picked to do shutdown. So the helper thread may
        // have to shutdown itself, and so we may be on any thread then.
#ifdef _X86_
        PRECONDITION(!ThisIsHelperThreadWorker() || (RunningOnWin95() && g_fProcessDetach));
#else
        PRECONDITION(!ThisIsHelperThreadWorker());
#endif
        PRECONDITION(DebuggerMaybeInvolved());
    }
    CONTRACTL_END;


#ifdef _X86_
    // If we're on Win95, then it's possible that we're on the helper thread. In that case
    // don't bother signalling the helper thread and waiting for it to exit (b/c we'll be
    // waiting on ourselves, so we'll just timeout). Just exit now.
    if (RunningOnWin95() && IsDbgHelperSpecialThread())
    {
        return S_OK;
    }
#endif

    HRESULT hr = S_OK;


    m_run = FALSE;

    // We need to get the helper thread out of its wait loop. So ping the thread-control event.
    // (Don't ping RSEA since that event should be used only for IPC communication).
    // Don't bother waiting for it to exit.
    SetEvent(this->m_threadControlEvent);

    return hr;
}

HRESULT inline DebuggerRCThread::EnsureRuntimeOffsetsInit(IpcTarget i)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(DebuggerMaybeInvolved());
        PRECONDITION(ThisMaybeHelperThread());
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    if (m_rgfInitRuntimeOffsets[i] == true)
    {
        hr = SetupRuntimeOffsets(m_rgDCB[i]);
        _ASSERTE(SUCCEEDED(hr)); // throws on failure

        // RuntimeOffsets structure is setup.
        m_rgfInitRuntimeOffsets[i] = false;
    }

    return hr;
}

//
// Call this function to tell the rc thread that we need the runtime offsets re-initialized at the next avaliable time.
//
void DebuggerRCThread::NeedRuntimeOffsetsReInit(IpcTarget i)
{
    LEAF_CONTRACT;

    m_rgfInitRuntimeOffsets[i] = true;
}


//
// SendIPCEvent is used by the Debugger object to send IPC events to
// the Debugger Interface. It waits for acknowledgement from the DI
// before returning.
//
// NOTE: this assumes that the event send buffer has been properly
// filled in. All it does it wake up the DI and let it know that its
// safe to copy the event out of this process.
//
// This function may block indefinitely if the controlling debugger
// suddenly went away.
//
HRESULT DebuggerRCThread::SendIPCEvent(IpcTarget iTarget)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER; // duh, we're in preemptive..

        if (ThisIsHelperThreadWorker())
        {
            // When we're stopped, the helper could actually be contracted as either mode-cooperative
            // or mode-preemptive!
            // If we're the helper thread, we're only sending events while we're stopped.
            // Our callers will be mode-cooperative, so call this mode_cooperative to avoid a bunch
            // of unncessary contract violations.
            MODE_COOPERATIVE;
        }
        else
        {
            // Managed threads sending debug events should always be in preemptive mode.
            MODE_PREEMPTIVE;
        }

        // Since we're blocking the runtime, we need to be in preemptive mode.
        PRECONDITION(iTarget == IPC_TARGET_OUTOFPROC);


        PRECONDITION(ThisMaybeHelperThread());
    }
    CONTRACTL_END;


    // one right side
    LOG((LF_CORDB, LL_EVERYTHING, "D::SendIPCEvent called...\n"));
    _ASSERTE(IPC_TARGET_OUTOFPROC + 1 == IPC_TARGET_COUNT );
    _ASSERTE(m_debugger->ThreadHoldsLock());

    HRESULT hr = S_OK;

    hr = g_pDebugger->LazyInitWrapper();

    if (FAILED(hr))
    {
        return hr;
    }


    DWORD ret = 0;
    BOOL succ;
    int i;
    int n;
    DebuggerIPCEvent* event = NULL;

    // check if we need to init the RuntimeOffsets structure in the
    // IPC buffer.
    if (iTarget > IPC_TARGET_COUNT)
    {
        i = 0;
        n = IPC_TARGET_COUNT;
    }
    else
    {
        i = iTarget;
        n = iTarget+1;
    }

    // Setup the Runtime Offsets struct.
    for(; i < n; i++)
    {
        // If the sending is to Any Attached debugger (for a given appdomain)
        // then we should skip those that aren't attached.

        // Tell the Debugger Interface there is an event for it to read.
        switch(i)
        {
            case IPC_TARGET_OUTOFPROC:
            {
                // This is a little strange, since we can send events to the
                // OOP _before_ we've attached to it.
                if (m_debugger->IsDebuggerAttached()
                    || iTarget == IPC_TARGET_OUTOFPROC)
                {
                    LOG((LF_CORDB, LL_EVERYTHING,"SendIPCEvent2 IPC_TARGET_OUTOFPROC\n"));
                    event = GetIPCEventSendBuffer(IPC_TARGET_OUTOFPROC);

                    STRESS_LOG4(LF_CORDB, LL_INFO1000, "D::SendIPCEvent %s (tid:0x%x(%d)) to outofproc appD 0x%x,\n",
                            IPCENames::GetName(event->type),
                            event->threadId,
                            event->threadId,
                            LsPtrToCookie(event->appDomainToken));

                    STRESS_LOG4(LF_CORDB, LL_INFO1000, "     --- pid 0x%x(%d) tid:0x%x(%d)\n",
                        GetCurrentProcessId(), GetCurrentProcessId(),
                        GetCurrentThreadId(), GetCurrentThreadId());

                    // increase the debug counter
                    DbgLog((DebuggerIPCEventType)(event->type & DB_IPCE_TYPE_MASK));

                    hr = EnsureRuntimeOffsetsInit(IPC_TARGET_OUTOFPROC);
                    if (FAILED(hr))
                    {
                        goto LError;
                    }

                    if (m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventRead.ImportToLocalProcess() == NULL ||
                        m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventAvailable.ImportToLocalProcess() == NULL)
                    {
                        hr = E_FAIL;
                        STRESS_LOG0(LF_CORDB, LL_INFO1000, "D::Failed to import events\n");
                        m_debugger->UnrecoverableError(hr,
                                                       0,
                                                       __FILE__,
                                                       __LINE__,
                                                       false);
                        goto LError;
                    }

#ifdef _DEBUG
                    // Before we set LSEA, make sure that LSEA + LSER aren't already set.
                    {
                       DWORD ret2 = WaitForSingleObject(
                                m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventRead,
                                0);

                       _ASSERTE(ret2 != WAIT_OBJECT_0);

                        DWORD ret3 = WaitForSingleObject(
                                m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventAvailable,
                                0);

                       _ASSERTE(ret3 != WAIT_OBJECT_0);

                    }
#endif

                    STRESS_LOG0(LF_CORDB,LL_INFO1000, "D::SendIPCEvent Set lsea\n");
                    succ = SetEvent(
                        m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventAvailable);


                    if (!succ)
                    {
                        hr = HRESULT_FROM_GetLastError();
                        _ASSERTE(!"Failed to set LSEA");
                        
                        STRESS_LOG1(LF_CORDB, LL_INFO1000, "D::SendIPCEvent Error on SetEvent lser with 0x%x\n", hr);
                        m_debugger->UnrecoverableError(hr,
                                                       0,
                                                       __FILE__,
                                                       __LINE__,
                                                       false);
                        goto LError;
                    }

                    // Wait for the Debugger Interface to tell us that its read our event.
                    STRESS_LOG0(LF_CORDB, LL_INFO1000, "D::SendIPCEvent Waiting on lser\n");
                    ret = WaitForSingleObject(
                            m_rgDCB[IPC_TARGET_OUTOFPROC]->m_leftSideEventRead,
                            INFINITE);

                    if (ret != WAIT_OBJECT_0)
                    {
                        hr = HRESULT_FROM_GetLastError();
                        m_debugger->UnrecoverableError(hr,
                                                       0,
                                                       __FILE__,
                                                       __LINE__,
                                                       false);
                        goto LError;
                    }

                    LOG((LF_CORDB,LL_INFO1000,"D::SendIPCEvent SendIPCEvent %s to outofproc finished\n", IPCENames::GetName(event->type)));
                }
                break;
            }
        }
LError:
        ; //try the next debugger
    }

    return hr;
}

//
// Return true if the helper thread is up & running
//
bool DebuggerRCThread::IsRCThreadReady()
{
    LEAF_CONTRACT;

    if (GetDCB(IPC_TARGET_OUTOFPROC) == NULL)
    {
        return false;
    }

    int idHelper = GetDCB(IPC_TARGET_OUTOFPROC)->m_helperThreadId;

    // The simplest check. If the threadid isn't set, we're not ready.
    if (idHelper == 0)
    {
        LOG((LF_CORDB, LL_EVERYTHING, "DRCT::IsReady - Helper not ready since DCB says id = 0.\n"));
        return false;
    }

    // a more subtle check. It's possible the thread was up, but then
    // an evil call to ExitProcess suddenly terminated the helper thread,
    // leaving the threadid still non-0. So check the actual thread object
    // and make sure it's still around.
    int ret = WaitForSingleObject(m_thread, 0);
    LOG((LF_CORDB, LL_EVERYTHING, "DRCT::IsReady - wait(0x%x)=%d, GetLastError() = %d\n", m_thread, ret, GetLastError()));

    if (ret != WAIT_TIMEOUT)
    {
        return false;
    }

    return true;
}


HRESULT DebuggerRCThread::ReDaclEvents(PSECURITY_DESCRIPTOR pSecurityDescriptor)
{
    LEAF_CONTRACT;


    return S_OK;
}


//
// A normal thread may hit a stack overflow and so we want to do
// any stack-intensive work on the Helper thread so that we don't
// blow the grace memory.
// Note that DoFavor will block until the fp is executed
//
void DebuggerRCThread::DoFavor(FAVORCALLBACK fp, void * pData)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(!ThisIsHelperThreadWorker());

        PRECONDITION(CheckPointer(fp));
        PRECONDITION(CheckPointer(pData, NULL_OK));
    }
    CONTRACTL_END;

    // We are being called on managed thread only.
    //

    // We'll have problems if another thread comes in and
    // deletes the RCThread object on us while we're in this call.
    if (IsRCThreadReady())
    {
        // If the helper thread calls this, we deadlock.
        // (Since we wait on an event that only the helper thread sets)
        _ASSERTE(GetRCThreadId() != GetCurrentThreadId());

        // Only lock if we're waiting on the helper thread.
        // This should be the only place the FavorLock is used.
        // Note this is never called on the helper thread.
        CrstHolder  ch(GetFavorLock());

        SetFavorFnPtr(fp, pData);

        // Our main message loop operating on the Helper thread will
        // pickup that event, call the fp, and set the Read event
        SetEvent(GetFavorAvailableEvent());

        LOG((LF_CORDB, LL_INFO10000, "DRCT::DF - Waiting on FavorReadEvent for favor 0x%08x\n", fp));

        // Wait for either the FavorEventRead to be set (which means that the favor
        // was executed by the helper thread) or the helper thread's handle (which means
        // that the helper thread exited without doing the favor, so we should do it)
        //
        // Note we are assuming that there's only 2 ways the helper thread can exit:
        // 1) Someone calls ::ExitProcess, killing all threads. That will kill us too, so we're "ok".
        // 2) Someone calls Stop(), causing the helper to exit gracefully. That's ok too. The helper
        // didn't execute the Favor (else the FREvent would have been set first) and so we can.
        //
        // Beware of problems:
        // 1) If the helper can block, we may deadlock.
        // 2) If the helper can exit magically (or if we change the Wait to include a timeout) ,
        // the helper thread may have not executed the favor, partially executed the favor,
        // or totally executed the favor but not yet signaled the FavorReadEvent. We don't
        // know what it did, so we don't know what we can do; so we're in an unstable state.

        const HANDLE waitset [] = { GetFavorReadEvent(), m_thread };

        DWORD ret = WaitForMultipleObjects(
            NumItems(waitset),
            waitset,
            FALSE,
            INFINITE
        );

        DWORD wn = (ret - WAIT_OBJECT_0);
        if (wn == 0) // m_FavorEventRead
        {
            // Favor was executed, nothing to do here.
            LOG((LF_CORDB, LL_INFO10000, "DRCT::DF - favor 0x%08x finished, ret = %d\n", fp, ret));
        }
        else
        {
            LOG((LF_CORDB, LL_INFO10000, "DRCT::DF - lost helper thread during wait, "
                "doing favor 0x%08x on current thread\n", fp));

            // Since we have no timeout, we shouldn't be able to get an error on the wait,
            // but just in case ...
            _ASSERTE(ret != WAIT_FAILED);
            _ASSERTE((wn == 1) && !"DoFavor - unexpected return from WFMO");

            // Thread exited without doing favor, so execute it on our thread.
            // If we're here because of a stack overflow, this may push us over the edge,
            // but there's nothing else we can really do
            (*fp)(pData);

            ResetEvent(GetFavorAvailableEvent());
        }

        // m_fpFavor & m_pFavorData are meaningless now. We could set them
        // to NULL, but we may as well leave them as is to leave a trail.

    }
    else
    {
        LOG((LF_CORDB, LL_INFO10000, "DRCT::DF - helper thread not ready, "
            "doing favor 0x%08x on current thread\n", fp));
        // If helper isn't ready yet, go ahead and execute the favor
        // on the callee's space
        (*fp)(pData);
    }

    // Drop a log message so that we know if we survived a stack overflow or not
    LOG((LF_CORDB, LL_INFO10000, "DRCT::DF - Favor 0x%08x completed successfully\n", fp));
}


//
// SendIPCReply simply indicates to the Right Side that a reply to a
// two-way event is ready to be read and that the last event sent from
// the Right Side has been fully processed.
//
// NOTE: this assumes that the event receive buffer has been properly
// filled in. All it does it wake up the DI and let it know that its
// safe to copy the event out of this process.
//
HRESULT DebuggerRCThread::SendIPCReply(IpcTarget iTarget)
{
    HRESULT hr = S_OK;

#ifdef LOGGING
    DebuggerIPCEvent* event = GetIPCEventReceiveBuffer(iTarget);

    LOG((LF_CORDB, LL_INFO10000, "D::SIPCR: replying with %s.\n",
         IPCENames::GetName(event->type)));
#endif

    if (iTarget == IPC_TARGET_OUTOFPROC)
    {
        BOOL succ = SetEvent(m_rgDCB[iTarget]->m_rightSideEventRead);

        if (!succ)
            hr = CORDBDebuggerSetUnrecoverableWin32Error(m_debugger, 0, false);
    }

    return hr;
}

//
// EarlyHelperThreadDeath handles the case where the helper
// thread has been ripped out from underneath of us by
// ExitProcess or TerminateProcess. These calls are pure evil, wacking
// all threads except the caller in the process. This can happen, for
// instance, when an app calls ExitProcess. All threads are wacked,
// the main thread calls all DLL main's, and the EE starts shutting
// down in its DLL main with the helper thread nuked.
//
void DebuggerRCThread::EarlyHelperThreadDeath(void)
{
    LOG((LF_CORDB, LL_INFO10000, "DRCT::EHTD\n"));

    // If we ever spun up a thread...
    if (m_thread != NULL && m_rgDCB[IPC_TARGET_OUTOFPROC])
    {
        Debugger::DebuggerLockHolder debugLockHolder(m_debugger);

        m_rgDCB[IPC_TARGET_OUTOFPROC]->m_helperThreadId = 0;

        LOG((LF_CORDB, LL_INFO10000, "DRCT::EHTD helperThreadId\n"));
        // dbgLockHolder goes out of scope - implicit Release
    }
}

