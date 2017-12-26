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
// File: debugger.h
//
// Header file for Runtime Controller classes of the COM+ Debugging Services.
//
//*****************************************************************************

#ifndef DEBUGGER_H_
#define DEBUGGER_H_

#include <windows.h>

#include <utilcode.h>

#if defined(_DEBUG) && !defined(DACCESS_COMPILE)
#define LOGGING
#endif

#include <log.h>

#include "cor.h"
#include "corpriv.h"
#include "daccess.h"

#include "common.h"
#include "winwrap.h"
#include "threads.h"
#include "frames.h"

#include "appdomain.hpp"
#include "eedbginterface.h"
#include "dbginterface.h"
#include "corhost.h"


#include "corjit.h"
#include <dbgmeta.h>

#include "frameinfo.h"

#include "dllimportcallback.h"

#include "canary.h"

#undef ASSERT
#define CRASH(x)  _ASSERTE(!x)
#define ASSERT(x) _ASSERTE(x)


#ifndef TRACE_MEMORY
#define TRACE_MEMORY 0
#endif

#if TRACE_MEMORY
#define TRACE_ALLOC(p)  LOG((LF_CORDB, LL_INFO10000, \
                       "--- Allocated %x at %s:%d\n", p, __FILE__, __LINE__));
#define TRACE_FREE(p)   LOG((LF_CORDB, LL_INFO10000, \
                       "--- Freed %x at %s:%d\n", p, __FILE__, __LINE__));
#else
#define TRACE_ALLOC(p)
#define TRACE_FREE(p)
#endif

typedef CUnorderedArray<BYTE *,11> UnorderedBytePtrArray;



extern bool g_EnableSIS;


/* ------------------------------------------------------------------------ *
 * Forward class declarations
 * ------------------------------------------------------------------------ */

class DebuggerFrame;
class DebuggerModule;
class DebuggerModuleTable;
class Debugger;
class DebuggerBreakpoint;
class DebuggerPendingFuncEvalTable;
class DebuggerRCThread;
class DebuggerStepper;
class DebuggerMethodInfo;
class DebuggerJitInfo;
class DebuggerMethodInfoTable;
struct DebuggerControllerPatch;
class DebuggerEval;
class DebuggerControllerQueue;
class DebuggerController;
class Crst;

typedef CUnorderedArray<DebuggerControllerPatch *, 17> PATCH_UNORDERED_ARRAY;
template<class T> void DeleteInteropSafe(T *p);

typedef VPTR(class Debugger) PTR_Debugger;
typedef DPTR(struct DebuggerILToNativeMap) PTR_DebuggerILToNativeMap;
typedef DPTR(class DebuggerMethodInfo) PTR_DebuggerMethodInfo;
typedef VPTR(class DebuggerMethodInfoTable) PTR_DebuggerMethodInfoTable;
typedef DPTR(class DebuggerJitInfo) PTR_DebuggerJitInfo;

/* ------------------------------------------------------------------------ *
 * Global variables
 * ------------------------------------------------------------------------ */

GPTR_DECL(Debugger,         g_pDebugger);
GPTR_DECL(EEDebugInterface, g_pEEInterface);
extern DebuggerRCThread     *g_pRCThread;

// There are still some APIs that call new that we call from the helper thread.
// These are unsafe operations, so we wrap them here. Each of these is a potential hang.
inline DWORD UnsafeGetConfigDWORD(LPCWSTR name, DWORD defValue)
{
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
    return REGUTIL::GetConfigDWORD(name, defValue);
}


#define CORDBDebuggerSetUnrecoverableWin32Error(__d, __code, __w) \
    ((__d)->UnrecoverableError(HRESULT_FROM_WIN32(GetLastError()), \
                               (__code), __FILE__, __LINE__, (__w)), \
     HRESULT_FROM_GetLastError())

#define CORDBDebuggerSetUnrecoverableError(__d, __hr, __w) \
    (__d)->UnrecoverableError((__hr), \
                               (__hr), __FILE__, __LINE__, (__w))

#define CORDBUnrecoverableError(__d) ((__d)->m_unrecoverableError == TRUE)

/* ------------------------------------------------------------------------ *
 * Helpers used for contract preconditions.
 * ------------------------------------------------------------------------ */


bool ThisIsHelperThreadWorker(void);
bool ThisIsTempHelperThread();
bool ThisIsTempHelperThread(DWORD tid);

#ifdef _DEBUG

// Functions can be split up into 3 categories:
// 1.) Functions that must run on the helper thread.
//     Returns true if this is the helper thread (or the thread
//     doing helper-threadduty).

// 2.) Functions that can't run on the helper thread.
//     This is just !ThisIsHelperThread();

// 3.) Functions that may or may not run on the helper thread.
//     Note this is trivially true, but it's presences means that
//     we're not case #1 or #2, so it's still valuable.
inline bool ThisMaybeHelperThread() { return true; }

//
//

bool DebuggerIsInvolved();

// 2) functions that run even when a debugger is not involved
//    ex: some startup code, stuff to track jit info.
//    ex: notification for jit attach.
// Although this is trivially true, marking this is a contract
// means that we thought about the invovlement status for this function
// and that it shouldn't be case #1.
inline bool DebuggerMaybeInvolved() {LEAF_CONTRACT;  return true; }

// Note that we have no case 3 here (!DebuggerIsInvolved) because we're
// biased towards case #1.



// Some things are called whether the debugger is attached or not.
// For example, an unhandled exception will notify the debugger.


#endif


// These are methods for transferring information between a REGDISPLAY and
// a DebuggerREGDISPLAY.
extern void CopyREGDISPLAY(REGDISPLAY* pDst, REGDISPLAY* pSrc);
extern void SetDebuggerREGDISPLAYFromREGDISPLAY(DebuggerREGDISPLAY* pDRD, REGDISPLAY* pRD
                                                IA64_ARG(bool  fIsFirstFrame = false)
                                                IA64_ARG(UINT  relOffset     = 0));

//
inline LPVOID PushedRegAddr(REGDISPLAY* pRD, LPVOID pAddr)
{
    LEAF_CONTRACT;

    if ( ((UINT_PTR)(pAddr) >= (UINT_PTR)pRD->pContext) &&
         ((UINT_PTR)(pAddr) <= ((UINT_PTR)pRD->pContext + sizeof(CONTEXT))) )
        return NULL;
    else
        return pAddr;
}

bool HandleIPCEventWrapper(Debugger* pDebugger, DebuggerIPCEvent *e, IpcTarget iWhich);

HRESULT ValidateObject(Object *objPtr);

//-----------------------------------------------------------------------------
// Execution control needs several ways to get at the context of a thread
// stopped in mangaged code (stepping, setip, func-eval).
// We want to abstract away a few things:
// - active: this thread is stopped at a patch
// - inactive: this threads was managed suspended somewhere in jitted code
//             because of some other active thread.
//
// In general, execution control operations administered from the helper thread
// can occur on any managed thread (active or inactive).
// Intermediate triggers (eg, TriggerPatch) only occur on an active thread.
//
// Viewing the context in terms of Active vs. Inactive lets us abstract away
// filter context, redirected context, and interop hijacks.
//-----------------------------------------------------------------------------

// Get the context for a thread stopped (perhaps temporarily) in managed code.
// The process may be live or stopped.
// This thread could be 'active' (stopped at patch) or inactive.
// This context should always be in managed code and this context can be manipulated
// for execution control (setip, single-step, func-eval, etc)
// Returns NULL if not available.
CONTEXT * GetManagedStoppedCtx(Thread * pThread);

// Get the context for a thread live in or around managed code.
// Caller guarantees this is active.
// This ctx is just for a 'live' thread. This means that the ctx may include
// from a M2U hijack or from a Native patch (like .
// Never NULL.
CONTEXT * GetManagedLiveCtx(Thread * pThread);


#undef UtilMessageBoxCatastrophic
#undef UtilMessageBoxCatastrophicNonLocalized
#undef UtilMessageBoxCatastrophicVA
#undef UtilMessageBoxCatastrophicNonLocalizedVA
#undef UtilMessageBox
#undef UtilMessageBoxNonLocalized
#undef UtilMessageBoxVA
#undef UtilMessageBoxNonLocalizedVA
#undef WszMessageBox
#define UtilMessageBoxCatastrophic __error("Use g_pDebugger->MessageBox from inside the left side of the debugger")
#define UtilMessageBoxCatastrophicNonLocalized __error("Use g_pDebugger->MessageBox from inside the left side of the debugger")
#define UtilMessageBoxCatastrophicVA __error("Use g_pDebugger->MessageBox from inside the left side of the debugger")
#define UtilMessageBoxCatastrophicNonLocalizedVA __error("Use g_pDebugger->MessageBox from inside the left side of the debugger")
#define UtilMessageBox __error("Use g_pDebugger->MessageBox from inside the left side of the debugger")
#define UtilMessageBoxNonLocalized __error("Use g_pDebugger->MessageBox from inside the left side of the debugger")
#define UtilMessageBoxVA __error("Use g_pDebugger->MessageBox from inside the left side of the debugger")
#define UtilMessageBoxNonLocalizedVA __error("Use g_pDebugger->MessageBox from inside the left side of the debugger")
#define WszMessageBox __error("Use g_pDebugger->MessageBox from inside the left side of the debugger")


/* ------------------------------------------------------------------------ *
 * Thread classes
 * ------------------------------------------------------------------------ */

class DebuggerThread
{
public:
    static HRESULT TraceAndSendStack(Thread *thread,
                                     DebuggerRCThread* rcThread,
                                     IpcTarget iWhich);


  private:
    static StackWalkAction TraceAndSendStackCallback(FrameInfo *pInfo,
                                                     VOID* data);

    static StackWalkAction StackWalkCount(FrameInfo *pinfo,
                                          VOID* data);

    static inline CORDB_ADDRESS GetObjContext( CrawlFrame *pCf );
};


/* ------------------------------------------------------------------------ *
 * Module classes
 * ------------------------------------------------------------------------ */

// Once a module / appdomain is unloaded, all Right-side objects (such as breakpoints)
// in that appdomain will get neutered and will thus be prevented from accessing
// the unloaded appdomain. 
class DebuggerModule
{
  public:
    DebuggerModule(Module* pRuntimeModule, AppDomain *pAppDomain);

    // Do we have any optimized code in the module?
    // JMC-probes aren't emitted in optimized code,
    bool HasAnyOptimizedCode();

    // If the debugger updates things to allow/disallow optimized code, then we have to track that.
    void MarkAllowedOptimizedCode();
    void UnmarkAllowedOptimizedCode();


    BOOL ClassLoadCallbacksEnabled(void);
    void EnableClassLoadCallbacks(BOOL f);

    AppDomain* GetAppDomain();

    Module * GetRuntimeModule();


    //
    DebuggerModule * GetPrimaryModule();

    // Called by DebuggerModuleTable to set our primary module
    void SetPrimaryModule(DebuggerModule * pPrimary);


    // We can skip the JMC probes if we know that a module has no JMC stuff
    // inside. So keep a strict count of all functions inside us.
    bool HasAnyJMCFunctions();
    void IncJMCFuncCount();
    void DecJMCFuncCount();

    void SetJMCStatus(bool fStatus, ULONG32 cTokens, mdToken * pTokens);
    bool LookupMethodDefaultJMCStatus(DebuggerMethodInfo * dmi);

  private:
    BOOL            m_enableClassLoadCallbacks;

    // The default jmc status for methods in this module.
    // Set by SetJMCStatus(). Individual methods can be override.
    bool            m_fDefaultJMCStatus;

    // Strict count of # of DebuggerMethodInfos that have jmc-enabled.
    LONG           m_cTotalJMCFuncs;

    // First step in moving away from hiding shared modules.
    DebuggerModule* m_pPrimaryModule;

    Module*       m_pRuntimeModule;

    AppDomain*     m_pAppDomain;

    bool m_fHasOptimizedCode;

    void PickPrimaryModule();


};

/* ------------------------------------------------------------------------ *
 * Hash to hold pending func evals by thread id
 * ------------------------------------------------------------------------ */

struct DebuggerPendingFuncEval
{
    FREEHASHENTRY   entry;
    Thread         *pThread;
    DebuggerEval   *pDE;
};

/* ------------------------------------------------------------------------ *
 * DebuggerRCThread class -- the Runtime Controller thread.
 * ------------------------------------------------------------------------ */

#define DRCT_CONTROL_EVENT  0
#define DRCT_RSEA           1
#define DRCT_FAVORAVAIL     2
#define DRCT_COUNT_INITIAL  3

#define DRCT_DEBUGGER_EVENT 3
#define DRCT_COUNT_FINAL    4





// Canary is used as way to have a runtime failure for the SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE
// contract violation.
// Have a macro which checks the canary and then uses the Suppress macro.
// We need this check to be a macro in order to chain to the Suppress_allocation macro.
#define CHECK_IF_CAN_TAKE_HELPER_LOCKS_IN_THIS_SCOPE(pHR, pCanary) \
    { \
        HelperCanary * __pCanary = (pCanary);  \
        if (!__pCanary->AreLocksAvailable()) { \
            (*pHR) = CORDBG_E_HELPER_MAY_DEADLOCK;  \
        } else  { \
            (*pHR) = S_OK; \
        } \
    } \
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE \
    ; \


// Mechanics for cross-thread call to helper thread (called "Favor").
class HelperThreadFavor
{
    // Only let RCThread access these fields.
    friend class DebuggerRCThread;

    HelperThreadFavor();
    // No dtor because we intentionally leak all shutdown.
    void Init();
    
protected:
    // Stuff for having the helper thread do function calls for a thread
    // that blew its stack
    FAVORCALLBACK m_fpFavor;
    void                           *m_pFavorData;
    HANDLE                          m_FavorReadEvent;
    Crst                            m_FavorLock;

    HANDLE                          m_FavorAvailableEvent;
};


// The *LazyInit classes represents storage that the debugger doesn't need until after it has started up.
// This is effectively an extension to the debugger class; but for perf reasons, we only
// want to instantiate it if we're actually debugging.

// Fields that are a logical extension of RCThread
class RCThreadLazyInit
{
    // Only let RCThread access these fields.
    friend class DebuggerRCThread;

public:
    RCThreadLazyInit() { }
    ~RCThreadLazyInit() { }

    void Init() { }
protected:



    HelperCanary m_Canary;
};

// Fields that are a logical extension of Debugger
class DebuggerLazyInit
{
    friend class Debugger;
public:
    DebuggerLazyInit();
    ~DebuggerLazyInit();

protected:
    void Init();

    DebuggerPendingFuncEvalTable *m_pPendingEvals;

    // The "debugger data lock" is a very small leaf lock used to protect debugger internal data structures (such 
    // as DJIs, DMIs, module table). It is a GC-unsafe-anymode lock and so it can't trigger a GC while being held.
    // It also can't issue any callbacks into the EE or anycode that it does not directly control. 
    // This is a separate lock from the the larger Debugger-lock / Controller lock, which allows regions under those
    // locks to access debugger datastructures w/o blocking each other. 
    Crst                  m_DebuggerDataLock;
    HANDLE                m_CtrlCMutex;
    HANDLE                m_exAttachEvent;
    HANDLE                m_exUnmanagedAttachEvent;
    HANDLE                m_exAttachAbortEvent;

    BOOL                  m_DebuggerHandlingCtrlC;

    SIZE_T_UNORDERED_ARRAY m_BPMappingDuplicates; // Used by
        // MapAndBindFunctionBreakpoints.  Note that this is
        // thread-safe only b/c we access it from within
        // the DebuggerController::Lock

    UnorderedBytePtrArray  m_pMemBlobs;

    // Hang RCThread fields off DebuggerLazyInit to avoid an extra pointer.
    RCThreadLazyInit m_RCThread;
};


class DebuggerRCThread
{
public:
    DebuggerRCThread(Debugger* debugger);
    virtual ~DebuggerRCThread();
    void CloseIPCHandles(IpcTarget iWhich);

    //
    // You create a new instance of this class, call Init() to set it up,
    // then call Start() start processing events. Stop() terminates the
    // thread and deleting the instance cleans all the handles and such
    // up.
    //
    HRESULT Init(void);
    HRESULT Start(void);
    HRESULT AsyncStop(void);

    //
    // These are used by this thread to send IPC events to the Debugger
    // Interface side.
    //
    DebuggerIPCEvent* GetIPCEventSendBuffer(IpcTarget iTarget)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        CONTRACTL_END;

#ifdef LOGGING
        if(IsRCThreadReady()) {
            LOG((LF_CORDB, LL_EVERYTHING, "RCThread is ready\n"));
        }
#endif

        _ASSERTE(m_rgDCB != NULL);
        _ASSERTE(m_rgDCB[iTarget] != NULL);
        // In case this turns into a continuation event
        ((DebuggerIPCEvent*) (m_rgDCB[iTarget])->m_sendBuffer)->next = NULL;
        LOG((LF_CORDB,LL_EVERYTHING, "GIPCESBuffer: got event 0x%x\n",
            (m_rgDCB[iTarget])->m_sendBuffer));

        return (DebuggerIPCEvent*) (m_rgDCB[iTarget])->m_sendBuffer;
    }

    DebuggerIPCEvent *GetIPCEventSendBufferContinuation(
        DebuggerIPCEvent *eventCur)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(eventCur != NULL);
            PRECONDITION(eventCur->next == NULL);
        }
        CONTRACTL_END;

        DebuggerIPCEvent *dipce = (DebuggerIPCEvent *) new (nothrow) BYTE [CorDBIPC_BUFFER_SIZE];
        dipce->next = NULL;

        LOG((LF_CORDB,LL_INFO1000000, "About to GIPCESBC 0x%x\n",dipce));

        if (dipce != NULL)
        {
            eventCur->next = dipce;
        }
#ifdef _DEBUG
        else
        {
            _ASSERTE( !"GetIPCEventSendBufferContinuation failed to allocate mem!" );
        }
#endif //_DEBUG

        return dipce;
    }

    // Send an IPCEvent once we're ready for sending. This should be done inbetween
    // SENDIPCEVENT_BEGIN & SENDIPCEVENT_END. See definition of SENDIPCEVENT_BEGIN
    // for usage pattern
    HRESULT SendIPCEvent(IpcTarget iTarget);

    HRESULT EnsureRuntimeOffsetsInit(IpcTarget i); // helper function for SendIPCEvent
    void NeedRuntimeOffsetsReInit(IpcTarget i);

    DebuggerIPCEvent* GetIPCEventReceiveBuffer(IpcTarget iTarget)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        CONTRACTL_END;
        _ASSERTE(m_rgDCB != NULL);
        _ASSERTE(m_rgDCB[iTarget] != NULL);

        return (DebuggerIPCEvent*) (m_rgDCB[iTarget])->m_receiveBuffer;
    }

    HRESULT SendIPCReply(IpcTarget iTarget);

    //
    // Handle Favors - get the Helper thread to do a function call for us
    // because our thread can't (eg, we don't have the stack space)
    // DoFavor will call (*fp)(pData) and block until fp returns.
    // pData can store parameters, return value, and a this ptr (if we
    // need to call a member function)
    //
    void DoFavor(FAVORCALLBACK fp, void * pData);

    //
    // Convience routines
    //
    DebuggerIPCControlBlock *GetDCB(IpcTarget iTarget)
    {
        LEAF_CONTRACT;
        // This may be called before we init or after we shutdown.
        if (m_rgDCB == NULL)
        {
            return NULL;
        }

        if (iTarget >= IPC_TARGET_COUNT)
        {
            iTarget = IPC_TARGET_OUTOFPROC;
        }

        return m_rgDCB[iTarget];
    }

    void WatchForStragglers(void);

    HRESULT SetupRuntimeOffsets(DebuggerIPCControlBlock *pDCB);

    bool HandleRSEA();
    void MainLoop();
    void TemporaryHelperThreadMainLoop();

    HANDLE GetHelperThreadCanGoEvent(void) {LEAF_CONTRACT;  return m_helperThreadCanGoEvent; }

    void EarlyHelperThreadDeath(void);

    DWORD CreateSetupSyncEvent(void);

    void RightSideDetach(void);

    //
    //
    void ThreadProc(void);
    static DWORD WINAPI ThreadProcStatic(LPVOID parameter);
    static DWORD WINAPI ThreadProcRemote(LPVOID parameter);

    DWORD GetRCThreadId()
    {
        LEAF_CONTRACT;

        return m_rgDCB[IPC_TARGET_OUTOFPROC]->m_helperThreadId;
    }

    // Return true if the Helper Thread up & initialized.
    bool IsRCThreadReady();

    HRESULT ReDaclEvents(PSECURITY_DESCRIPTOR securityDescriptor);
private:

    FAVORCALLBACK GetFavorFnPtr()           { return m_favorData.m_fpFavor; }
    void * GetFavorData()                   { return m_favorData.m_pFavorData; }

    void SetFavorFnPtr(FAVORCALLBACK fp, void * pData)    
    { 
        m_favorData.m_fpFavor = fp; 
        m_favorData.m_pFavorData = pData;
    }
    Crst * GetFavorLock()                   { return &m_favorData.m_FavorLock; }

    HANDLE GetFavorReadEvent()              { return m_favorData.m_FavorReadEvent; }
    HANDLE GetFavorAvailableEvent()         { return m_favorData.m_FavorAvailableEvent; }

    HelperThreadFavor m_favorData;  
    

    HelperCanary * GetCanary()              { return &GetLazyData()->m_Canary; }


    friend class Debugger;
    HRESULT VerifySecurityOnRSCreatedEvents(HANDLE sse, HANDLE lsea, HANDLE lser);
    Debugger*                       m_debugger;

    // IPC_TARGET_* define default targets - if we ever want to do
    // multiple right sides, we'll have to switch to a OUTOFPROC + iTargetProcess scheme
    DebuggerIPCControlBlock       **m_rgDCB;

    HANDLE                          m_thread;
    bool                            m_run;

    HANDLE                          m_threadControlEvent;
    HANDLE                          m_helperThreadCanGoEvent;
    bool                            m_rgfInitRuntimeOffsets[IPC_TARGET_COUNT];
    bool                            m_fDetachRightSide;

    RCThreadLazyInit *              GetLazyData();
#ifdef _DEBUG
    // Tracking to ensure that the helper thread only calls New() on the interop-safe heap.
    // We need a very light-weight way to track the helper b/c we need to check everytime somebody
    // calls operator new, which may occur during shutdown paths.
    static EEThreadId               s_DbgHelperThreadId;

    friend void AssertAllocationAllowed();

public:
    // The OS ThreadId of the helper as determined from the CreateThread call.
    DWORD                           m_DbgHelperThreadOSTid;
private:
#endif

public:
    HANDLE                          m_SetupSyncEvent;
};


/* ------------------------------------------------------------------------ *
 * Debugger Method Info struct and hash table
 * ------------------------------------------------------------------------ */

// class DebuggerMethodInfo: Struct to hold all the information
// necessary for a given function.
//
// m_module, m_token:   Method that this DMI applies to
//
// COR_IL_MAP m_rgInstrumentedILMap:   The profiler may instrument the
//      code. This is done by modifying the IL code that gets handed to the
//      JIT.  This array will map offsets within the original ("old") IL code,
//      to offsets within the instrumented ("new") IL code that is actually
//      being instrumented.  Note that this map will actually be folded into
//      the IL-Native map, so that we don't have to go through this
//      except in special cases.  We have to keep this around for corner
//      cases like a rejiting a pitched method.
//
// SIZE_T m_cInstrumentedILMap:  A count of elements in
//      m_rgInstrumentedILMap
//
const bool bOriginalToInstrumented = true;
const bool bInstrumentedToOriginal = false;

class DebuggerMethodInfo
{
    // This is the most recent version of the function based on the latest update and is
    // set in UpdateFunction. When a function is jitted, the version is copied from here
    // and stored in the corresponding DebuggerJitInfo structure so can always know the
    // version of a particular jitted function.
    SIZE_T          m_currentEnCVersion;

public:
    PTR_Module          m_module;
    mdMethodDef         m_token;

    PTR_DebuggerMethodInfo m_prevMethodInfo;
    PTR_DebuggerMethodInfo m_nextMethodInfo;


    // Enumerate DJIs
    // Expected usage:
    // DMI.InitDJIIterator(&it);
    // while(!it.IsAtEnd()) {
    //    f(it.Current()); it.Next();
    // }
    class DJIIterator
    {
        friend class DebuggerMethodInfo;

        DebuggerJitInfo* m_pCurrent;
    public:
        DJIIterator();

        bool IsAtEnd();
        DebuggerJitInfo * Current();
        void Next();

    };

    // Ensure the DJI cache is completely up to date. (This is heavy weight).
    void CreateDJIsForNativeBlobs(AppDomain * pAppDomain);

    // Get an iterator for all native blobs (accounts for Generics, Enc, + Prejiiting).
    // Must be stopped when we do this. This could be heavy weight.
    // This will call CreateDJIsForNativeBlobs() to ensure we have all DJIs available.
    void IterateAllDJIs(AppDomain * pAppDomain, DJIIterator * pEnum);


private:
    // The linked list of JIT's of this version of the method.   This will ALWAYS
    // contain one element except for code in generic classes or generic methods,
    // which may get JITted more than once under different type instantiations.
    //
    // We find the appropriate JitInfo by searching the list (nearly always this
    // will return the first element of course).
    //
    // The JitInfos contain back pointers to this MethodInfo.  They should never be associated
    // with any other MethodInfo.
    //
    // USE ACCESSOR FUNCTION GetLatestJitInfo(), as it does lazy init of this field.
    //
    PTR_DebuggerJitInfo m_latestJitInfo;

public:

    PTR_DebuggerJitInfo GetLatestJitInfo(MethodDesc *fd);

#ifdef LOGGING
    DebuggerJitInfo * Logging_GetLatestJitInfo();
#endif


    // Creating the Jit-infos.
    DebuggerJitInfo *FindOrCreateInitAndAddJitInfo(MethodDesc* fd);
    DebuggerJitInfo *CreateInitAndAddJitInfo(MethodDesc* fd, TADDR startAddr, SIZE_T sizeBytes);


    void DeleteJitInfo(DebuggerJitInfo *dji);
    void DeleteJitInfoList(void);

    // Return true iff this has been jitted.
    // Since we can create DMIs freely, a DMI's existence doesn't mean that the method was jitted.
    bool HasJitInfos();

    // Return true iff this has been EnCed since the last time the function was jitted.
    bool HasMoreRecentEnCVersion();


    // Return true iif this is a JMC function, else false.
    bool IsJMCFunction();
    void SetJMCStatus(bool fStatus);


    DebuggerMethodInfo(Module *module, mdMethodDef token);
    ~DebuggerMethodInfo();

    // A profiler can remap the IL. We track the "instrumented" IL map here.
    void SetInstrumentedILMap(COR_IL_MAP * pMap, SIZE_T cEntries);
    bool HasInstrumentedILMap() { return m_cInstrumentedILMap != 0; }

    // TranslateToInstIL will take offOrig, and translate it to the
    //      correct IL offset if this code happens to be instrumented (i.e.,
    //      if m_rgInstrumentedILMap != NULL && m_cInstrumentedILMap > 0)
    SIZE_T TranslateToInstIL(SIZE_T offOrig, bool fOrigToInst);


    // We don't always have a debugger module. (Ex: we're tracking debug info,
    // but no debugger's attached). So this may return NULL alot.
    // If we can, we should use the RuntimeModule when ever possible.
    DebuggerModule* GetPrimaryModule();

    // We always have a runtime module.
    Module * GetRuntimeModule();


    void SetCurrentEnCVersion(SIZE_T currentEnCVersion)
    {
        LEAF_CONTRACT;

        _ASSERTE(currentEnCVersion >= CorDB_DEFAULT_ENC_FUNCTION_VERSION);
        m_currentEnCVersion = currentEnCVersion;
    }

    SIZE_T GetCurrentEnCVersion()
    {
        LEAF_CONTRACT;

        return m_currentEnCVersion;
    }

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

protected:
    // JMC info. Each method can have its own JMC setting.
    bool m_fJMCStatus;

    // "Instrumented" IL map set by the profiler.
    SIZE_T                   m_cInstrumentedILMap;
    COR_IL_MAP               *m_rgInstrumentedILMap;

};


// ------------------------------------------------------------------------ *
// DebuggerHeap class
// For interop debugging, we need a heap that:
// - does not take any outside looks
// - returns memory which could be executed.
// ------------------------------------------------------------------------ */


class DebuggerHeap
{
public:
    DebuggerHeap();
    ~DebuggerHeap();

    bool IsInit();
    void Destroy();
    HRESULT Init();

    void *Alloc(DWORD size);
    void *Realloc(void *pMem, DWORD newSize, DWORD oldSize);
    void  Free(void *pMem);


protected:
};

class DebuggerJitInfo;


class CodeRegionInfo
{
public:
    CodeRegionInfo() :
        m_addrOfHotCode(NULL),
        m_addrOfColdCode(NULL),
        m_sizeOfHotCode(0),
        m_sizeOfColdCode(0)
    {
        WRAPPER_CONTRACT;
    }

    static CodeRegionInfo GetCodeRegionInfo(DebuggerJitInfo *dji, MethodDesc *md = NULL, CORDB_ADDRESS_TYPE *addr = NULL);

    // Fills in the CodeRegoinInfo fields from the start address.
    void InitializeFromStartAddress(CORDB_ADDRESS_TYPE *addr)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        CONTRACTL_END;

        m_addrOfHotCode = (BYTE *) addr;
        g_pEEInterface->GetMethodRegionInfo((BYTE *) addr,
            &m_addrOfColdCode,
            (size_t *) &m_sizeOfHotCode,
            (size_t *) &m_sizeOfColdCode);
    }

    // Converts an offset within a method to a code address
    BYTE *OffsetToAddress(SIZE_T offset)
    {
        LEAF_CONTRACT;

        if (m_addrOfHotCode)
        {
            if (offset < m_sizeOfHotCode)
            {
                return m_addrOfHotCode + offset;
            }
            else
            {
                _ASSERTE(m_addrOfColdCode);
                _ASSERTE(offset <= m_sizeOfHotCode + m_sizeOfColdCode);

                return m_addrOfColdCode + (offset - m_sizeOfHotCode);
            }
        }
        else
        {
            return NULL;
        }
    }

    // Converts a code address to an offset within the method
    SIZE_T AddressToOffset(const BYTE *address)
    {
        LEAF_CONTRACT;

        if ((address >= m_addrOfHotCode) &&
            (address <  m_addrOfHotCode + m_sizeOfHotCode))
        {
            return address - m_addrOfHotCode;
        }
        else if ((address >= m_addrOfColdCode) &&
                 (address <  m_addrOfColdCode + m_sizeOfColdCode))
        {
            return address - m_addrOfColdCode + m_sizeOfHotCode;
        }
        else
        {
            _ASSERTE(!"addressToOffset called with invalid address");
            return NULL;
        }
    }

    // Determines whether the address lies within the method
    bool IsMethodAddress(const BYTE *address)
    {
        LEAF_CONTRACT;

        return (((address >= m_addrOfHotCode) &&
                 (address <  m_addrOfHotCode + m_sizeOfHotCode)) ||
                ((address >= m_addrOfColdCode) &&
                 (address <  m_addrOfColdCode + m_sizeOfColdCode)));
    }

    // Determines whether the offset is in the hot section
    bool IsOffsetHot(SIZE_T offset)
    {
        LEAF_CONTRACT;

        return (offset < m_sizeOfHotCode);
    }

    BYTE  *getAddrOfHotCode()  {return m_addrOfHotCode;}
    BYTE  *getAddrOfColdCode() {return m_addrOfColdCode;}
    SIZE_T getSizeOfHotCode()  {return m_sizeOfHotCode;}
    SIZE_T getSizeOfColdCode() {return m_sizeOfColdCode;}
    SIZE_T getSizeOfTotalCode(){return m_sizeOfHotCode + m_sizeOfColdCode; }

private:

    BYTE *               m_addrOfHotCode;
    BYTE *               m_addrOfColdCode;
    SIZE_T               m_sizeOfHotCode;
    SIZE_T               m_sizeOfColdCode;
};

// Helper to copy Hot-Cold info from a code-region to a JITFuncData
void InitJITFuncDataFromCodeRegion(DebuggerIPCE_JITFuncData * pData, CodeRegionInfo * pInfo);

/* ------------------------------------------------------------------------ *
 * Debugger JIT Info struct
 * ------------------------------------------------------------------------ */

// class DebuggerJitInfo:   Struct to hold all the JIT information
// necessary for a given function.
// - DJIs are 1:1 w/ native codeblobs. They're almost 1:1 w/ Native Method Descs.
//    except that a MethodDesc only refers to the most recent EnC version of a method.
// - If 2 DJIs are different, they refer to different code-blobs.
// - DJIs are lazily created, and so you can't safely enumerate them b/c
// you can't rely on whether they're created or not.


//
// MethodDesc* m_fd:   MethodDesc of the method that this DJI applies to
//
// CORDB_ADDRESS m_addrOfCode:   Address of the code.  This will be read by
//      the right side (via ReadProcessMemory) to grab the actual  native start
//      address of the jitted method.
//
// SIZE_T m_sizeOfCode:   Pseudo-private variable: use the GetSkzeOfCode
//      method to get this value.
//
// bool m_codePitched:   Set to true if the code is, in fact,
//      no longer there, but the DJI will be valid once
//      the method is reJITted.
//
// bool m_jitComplete:   Set to true once JITComplete has been called.
//
// DebuggerILToNativeMap* m_sequenceMap:   This is the sequence map, which
//      is actually a collection of IL-Native pairs, where each IL corresponds
//      to a line of source code.  Each pair is refered to as a sequence map point.
//
// SIZE_T m_lastIL:   last nonEPILOG instruction
//
// unsigned int m_sequenceMapCount:   Count of the DebuggerILToNativeMaps
//      in m_sequenceMap.
//
// bool m_sequenceMapSorted:   Set to true once m_sequenceMapSorted is sorted
//      into ascending IL order (Debugger::setBoundaries, SortMap).
//
// bool m_encBreakpointsApplied:   Set to true once UpdateFunction has
//      plaster all the sequence points with DebuggerEnCBreakpoints
//

class DebuggerJitInfo
{
public:

    PTR_MethodDesc           m_fd;
    bool                     m_codePitched;
    bool                     m_jitComplete;


    // If the variable layout of this method changes from this version
    // to the next, then it's illegal to move from this version to the next.
    // The caller is responsible for ensuring that local variable layout
    // only changes when there are no frames in any stack that are executing
    // this method.
    // In a debug build, we'll assert if we try to make this EnC transition,
    // in a free/retail build we'll silently fail to make the transition.
    BOOL                     m_illegalToTransitionFrom;

    PTR_DebuggerMethodInfo   m_methodInfo;

    CORDB_ADDRESS            m_addrOfCode;
    SIZE_T                   m_sizeOfCode;

    CodeRegionInfo           m_codeRegionInfo;

    PTR_DebuggerJitInfo      m_prevJitInfo;
    PTR_DebuggerJitInfo      m_nextJitInfo;

protected:
    // The jit maps are lazy-initialized.
    // They are always sorted.
    SIZE_T                   m_lastIL;
    PTR_DebuggerILToNativeMap m_sequenceMap;
    unsigned int             m_sequenceMapCount;
    bool                     m_sequenceMapSorted;

    PTR_NativeVarInfo        m_varNativeInfo;
    unsigned int             m_varNativeInfoCount;

    bool                     m_fAttemptInit;

#ifndef DACCESS_COMPILE
    void LazyInitBounds();
#else
    void LazyInitBounds() { }
#endif

public:
    unsigned int GetSequenceMapCount()
    {
        LazyInitBounds();
        return m_sequenceMapCount;
    }

    PTR_DebuggerILToNativeMap GetSequenceMap()
    {
        LazyInitBounds();
        return m_sequenceMap;
    }

    PTR_NativeVarInfo GetVarNativeInfo()
    {
        LazyInitBounds();
        return m_varNativeInfo;
    }

    unsigned int GetVarNativeInfoCount()
    {
        LazyInitBounds();
        return m_varNativeInfoCount;
    }


    // The version number of this jitted code
    SIZE_T                   m_encVersion;


#ifndef DACCESS_COMPILE

    DebuggerJitInfo(DebuggerMethodInfo *minfo, MethodDesc *fd);
    ~DebuggerJitInfo();

#endif // #ifdef DACCESS_COMPILE

    class ILToNativeOffsetIterator;

    // Usage of ILToNativeOffsetIterator:
    //
    // ILToNativeOffsetIterator it;
    // dji->InitILToNativeOffsetIterator(&it, ilOffset);
    // while (!it.IsAtEnd())
    // {
    //     nativeOffset = it.Current(&fExact);
    //     it.Next();
    // }
    struct ILOffset
    {
        friend class DebuggerJitInfo;
        friend class DebuggerJitInfo::ILToNativeOffsetIterator;

    private:
        SIZE_T m_ilOffset;
        WIN64_ONLY(int m_funcletIndex);
    };

    struct NativeOffset
    {
        friend class DebuggerJitInfo;
        friend class DebuggerJitInfo::ILToNativeOffsetIterator;

    private:
        SIZE_T m_nativeOffset;
        BOOL   m_fExact;
    };

    class ILToNativeOffsetIterator
    {
        friend class DebuggerJitInfo;

    public:
        ILToNativeOffsetIterator();

        bool   IsAtEnd();
        SIZE_T Current(BOOL* pfExact);
        SIZE_T CurrentAssertOnlyOne(BOOL* pfExact);
        void   Next();

    private:
        void   Init(DebuggerJitInfo* dji, SIZE_T ilOffset);

        DebuggerJitInfo* m_dji;
        ILOffset     m_currentILOffset;
        NativeOffset m_currentNativeOffset;
    };

    void InitILToNativeOffsetIterator(ILToNativeOffsetIterator &it, SIZE_T ilOffset);

    DebuggerILToNativeMap *MapILOffsetToMapEntry(SIZE_T ilOffset, BOOL *exact=NULL
                                                 WIN64_ARG(BOOL fWantFirst = TRUE));
    void MapILRangeToMapEntryRange(SIZE_T ilStartOffset, SIZE_T ilEndOffset,
                                   DebuggerILToNativeMap **start,
                                   DebuggerILToNativeMap **end);
    NativeOffset MapILOffsetToNative(ILOffset ilOffset);

    // MapSpecialToNative maps a CordDebugMappingResult to a native
    //      offset so that we can get the address of the prolog & epilog. which
    //      determines which epilog or prolog, if there's more than one.
    SIZE_T MapSpecialToNative(CorDebugMappingResult mapping,
                              SIZE_T which,
                              BOOL *pfAccurate);

    // MapNativeOffsetToIL Takes a given nativeOffset, and maps it back
    //      to the corresponding IL offset, which it returns.  If mapping indicates
    //      that a the native offset corresponds to a special region of code (for
    //      example, the epilog), then the return value will be specified by
    //      ICorDebugILFrame::GetIP (see cordebug.idl)
    DWORD MapNativeOffsetToIL(DWORD nativeOffset,
                              CorDebugMappingResult *mapping,
                              DWORD *which);

    // If a method has multiple copies of code (because of EnC or code-pitching),
    // this returns the DJI corresponding to 'pbAddr'
    DebuggerJitInfo *GetJitInfoByAddress(const BYTE *pbAddr );

    void Init(TADDR newAddress, SIZE_T sizeOfCode);


    void SetVars(ULONG32 cVars, ICorDebugInfo::NativeVarInfo *pVars);
    void SetBoundaries(ULONG32 cMap, ICorDebugInfo::OffsetMapping *pMap);

    ICorDebugInfo::SourceTypes GetSrcTypeFromILOffset(SIZE_T ilOffset);

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    // Debug support
    CHECK Check() const;
    CHECK Invariant() const;
};

/*
 * class MapSortIL:  A template class that will sort an array of DebuggerILToNativeMap.
 * This class is intended to be instantiated on the stack / in temporary storage, and used to reorder the sequence map.
 */
class MapSortIL : public CQuickSort<DebuggerILToNativeMap>
{
  public:
    //Constructor
    MapSortIL(DebuggerILToNativeMap *map,
              int count)
      : CQuickSort<DebuggerILToNativeMap>(map, count) {}

    inline int CompareInternal(DebuggerILToNativeMap *first,
                               DebuggerILToNativeMap *second)
    {
        LEAF_CONTRACT;

        if (first->nativeStartOffset == second->nativeStartOffset)
            return 0;
        else if (first->nativeStartOffset < second->nativeStartOffset)
            return -1;
        else
            return 1;
    }

    //Comparison operator
    int Compare(DebuggerILToNativeMap *first,
                DebuggerILToNativeMap *second)
    {
        LEAF_CONTRACT;

        //PROLOGs go first
        if (first->ilOffset == (ULONG) ICorDebugInfo::PROLOG
            && second->ilOffset == (ULONG) ICorDebugInfo::PROLOG)
        {
            return CompareInternal(first, second);
        } else if (first->ilOffset == (ULONG) ICorDebugInfo::PROLOG)
        {
            return -1;
        } else if (second->ilOffset == (ULONG) ICorDebugInfo::PROLOG)
        {
            return 1;
        }
        //NO_MAPPING go last
        else if (first->ilOffset == (ULONG) ICorDebugInfo::NO_MAPPING
            && second->ilOffset == (ULONG) ICorDebugInfo::NO_MAPPING)
        {
            return CompareInternal(first, second);
        } else if (first->ilOffset == (ULONG) ICorDebugInfo::NO_MAPPING)
        {
            return 1;
        } else if (second->ilOffset == (ULONG) ICorDebugInfo::NO_MAPPING)
        {
            return -1;
        }
        //EPILOGs go next-to-last
        else if (first->ilOffset == (ULONG) ICorDebugInfo::EPILOG
            && second->ilOffset == (ULONG) ICorDebugInfo::EPILOG)
        {
            return CompareInternal(first, second);
        } else if (first->ilOffset == (ULONG) ICorDebugInfo::EPILOG)
        {
            return 1;
        } else if (second->ilOffset == (ULONG) ICorDebugInfo::EPILOG)
        {
            return -1;
        }
        //normal offsets compared otherwise
        else if (first->ilOffset < second->ilOffset)
            return -1;
        else if (first->ilOffset == second->ilOffset)
            return CompareInternal(first, second);
        else
            return 1;
    }
};

/*
 * class MapSortNative:  A template class that will sort an array of DebuggerILToNativeMap by the nativeStartOffset field.
 * This class is intended to be instantiated on the stack / in temporary storage, and used to reorder the sequence map.
 */
class MapSortNative : public CQuickSort<DebuggerILToNativeMap>
{
  public:
    //Constructor
    MapSortNative(DebuggerILToNativeMap *map,
                  int count)
      : CQuickSort<DebuggerILToNativeMap>(map, count)
    {
        WRAPPER_CONTRACT;
    }


    //Returns -1,0,or 1 if first's nativeStartOffset is less than, equal to, or greater than second's
    int Compare(DebuggerILToNativeMap *first,
                DebuggerILToNativeMap *second)
    {
        LEAF_CONTRACT;

        if (first->nativeStartOffset < second->nativeStartOffset)
            return -1;
        else if (first->nativeStartOffset == second->nativeStartOffset)
            return 0;
        else
            return 1;
    }
};

/* ------------------------------------------------------------------------ *
 * Import flares from assembly file
 * We rely on flares having unique addresses, and so we need to keeps them
 * from getting folded by the linker (Since they are identical code).
 * ------------------------------------------------------------------------ */

extern "C" void __stdcall ExceptionForRuntimeFlare(void);
extern "C" void __stdcall ExceptionForRuntimeHandoffStartFlare(void);
extern "C" void __stdcall ExceptionForRuntimeHandoffCompleteFlare(void);
extern "C" void __stdcall ExceptionNotForRuntimeFlare(void);
extern "C" void __stdcall NotifyRightSideOfSyncCompleteFlare(void);
extern "C" void __stdcall NotifySecondChanceReadyForDataFlare(void);

/* ------------------------------------------------------------------------ *
 * Debugger class
 * ------------------------------------------------------------------------ */


enum DebuggerAttachState
{
    SYNC_STATE_0,   // Debugger is attached
    SYNC_STATE_1,   // Debugger is attaching: Send CREATE_APP_DOMAIN_EVENTS
    SYNC_STATE_2,   // Debugger is attaching: Send LOAD_ASSEMBLY and LOAD_MODULE events
    SYNC_STATE_3,   // Debugger is attaching: Send LOAD_CLASS and THREAD_ATTACH events
    SYNC_STATE_10,  // Attaching to appdomain during create: send LOAD_ASSEMBLY and LOAD_MODULE events. (Much like SYNC_STATE_2)
    SYNC_STATE_11,  // Attaching to appdomain during create: send LOAD_CLASS events. (Much like SYNC_STATE_3, but no THREAD_ATTACH events)
    SYNC_STATE_20,  // Debugger is attached; We've accumulated EnC remap info to send on next continue
};

// Forward declare some parameter marshalling structs
struct ShouldAttachDebuggerParams;
struct EnsureDebuggerAttachedParams;
struct SendMDANotificationParams;

// class Debugger:  This class implements DebugInterface to provide
// the hooks to the Runtime directly.
//

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(Debugger *);
template BOOL CompareDefault(Debugger *,Debugger *);
typedef DacHolder<Debugger *, DoNothing, DoNothing> HACKDebuggerLockHolder;
#endif

// SEH handlers need symbols that are not name-mangled, so that we can specify them
// in the .safeseh directives of handlers.asm.
extern "C" EXCEPTION_DISPOSITION __cdecl
Debugger_FirstChanceHijackFilter(EXCEPTION_RECORD *pExceptionRecord,
                             EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
                             CONTEXT *pContext,
                             void *DispatcherContext);
extern "C" EXCEPTION_DISPOSITION __cdecl
Debugger_M2UHandoffHijackFilter(EXCEPTION_RECORD *pExceptionRecord,
                             EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
                             CONTEXT *pContext,
                             void *DispatcherContext);

class Debugger : public DebugInterface
{
    VPTR_VTABLE_CLASS(Debugger, DebugInterface);
public:

#ifndef DACCESS_COMPILE
    Debugger();
    ~Debugger();
#endif

    friend class DebuggerLazyInit;

    // Checks if the JitInfos table has been allocated, and if not does so.
    HRESULT inline CheckInitMethodInfoTable();
    HRESULT inline CheckInitModuleTable();
    HRESULT CheckInitPendingFuncEvalTable();

#ifndef DACCESS_COMPILE
    DWORD GetRCThreadId()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        CONTRACTL_END;

        if (m_pRCThread)
            return m_pRCThread->GetRCThreadId();
        else
            return 0;
    }
#endif

    //
    // Methods exported from the Runtime Controller to the Runtime.
    // (These are the methods specified by DebugInterface.)
    //
    HRESULT Startup(void);
    void LazyInit(); // will throw
    HRESULT LazyInitWrapper(); // calls LazyInit and converts to HR.

    // Message box API for the left side of the debugger. This API handles calls from the
    // debugger helper thread as well as from normal EE threads. It is the only one that
    // should be used from inside the debugger left side.
    int MessageBox(
                UINT uText,       // Resource Identifier for Text message
                UINT uCaption,    // Resource Identifier for Caption
                UINT uType,       // Style of MessageBox
                BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                ...);             // Additional Arguments

    void SetEEInterface(EEDebugInterface* i);
    void StopDebugger(void);
    BOOL IsStopped(void)
    {
        LEAF_CONTRACT;
        // implements DebugInterface but also is called internally
        return m_stopped;
    }

    bool IsSISEnabled() { return g_EnableSIS; }

    void ThreadCreated(Thread* pRuntimeThread);
    void ThreadStarted(Thread* pRuntimeThread, BOOL fAttaching);
    void DetachThread(Thread *pRuntimeThread);

    BOOL SuspendComplete();

    void LoadModule(Module* pRuntimeModule,
                    LPCWSTR pszModuleName,
                    DWORD dwModuleName,
                    Assembly *pAssembly,
                    AppDomain *pAppDomain,
                    BOOL fAttaching);
    DebuggerModule* AddDebuggerModule(Module* pRuntimeModule,
                              AppDomain *pAppDomain);


    void UnloadModule(Module* pRuntimeModule,
                      AppDomain *pAppDomain);
    void DestructModule(Module *pModule);

    void RemoveModuleReferences(Module * pModule);

    HRESULT SendPdbStream(Module* pRuntimeModule,
                                AppDomain *pAppDomain,
                                BOOL fAttaching);

    void UpdateModuleSyms(Module *pRuntimeModule,
                          AppDomain *pAppDomain,
                          BOOL fAttaching);

    HRESULT ModuleMetaDataToMemory(Module *pMod, BYTE **prgb, DWORD *pcb);

    BOOL LoadClass(TypeHandle th,
                   mdTypeDef classMetadataToken,
                   Module* classModule,
                   AppDomain *pAD,
                   BOOL fAllAppDomains,
                   BOOL fAttaching);
    void UnloadClass(mdTypeDef classMetadataToken,
                     Module* classModule,
                     AppDomain *pAD,
                     BOOL fAllAppDomains);

    void SendClassLoadUnloadEvent (mdTypeDef classMetadataToken,
                                   DebuggerModule *classModule,
                                   Assembly *pAssembly,
                                   AppDomain *pAppDomain,
                                   BOOL fIsLoadEvent);
    BOOL SendSystemClassLoadUnloadEvent (mdTypeDef classMetadataToken,
                                         Module *classModule,
                                         BOOL fIsLoadEvent);

    void SendCatchHandlerFound(Thread *pThread,
                               FramePointer fp,
                               SIZE_T nOffset,
                               DWORD  dwFlags);

    LONG NotifyOfCHFFilter(EXCEPTION_POINTERS* pExceptionPointers, PVOID pCatchStackAddr);


    bool FirstChanceNativeException(EXCEPTION_RECORD *exception,
                               CONTEXT *context,
                               DWORD code,
                               Thread *thread);

    bool IsJMCMethod(Module* pModule, mdMethodDef tkMethod);

    int GetMethodEncNumber(MethodDesc * pMethod);


    bool FirstChanceManagedException(Thread *pThread, SIZE_T currentIP, SIZE_T currentSP
                                     IA64_ARG(SIZE_T currentBSP));

    void FirstChanceManagedExceptionCatcherFound(Thread *pThread,
                                                 MethodDesc *pMD, TADDR pMethodAddr,
                                                 BYTE *currentSP
                                                 IA64_ARG(BYTE *currentBSP),
                                                 EE_ILEXCEPTION_CLAUSE *pEHClause);

    LONG LastChanceManagedException(EXCEPTION_RECORD *pExceptionRecord,
                             CONTEXT *pContext,
                             Thread *pThread,
                             BOOL jitAttachRequested,
                             BOOL useManagedDebugger);

    void ManagedExceptionUnwindBegin(Thread *pThread);

    void DeleteInterceptContext(void *pContext);

    void ExceptionFilter(MethodDesc *fd, TADDR pMethodAddr, SIZE_T offset, BYTE *pStack
                         IA64_ARG(BYTE *pBStore));
    void ExceptionHandle(MethodDesc *fd, TADDR pMethodAddr, SIZE_T offset, BYTE *pStack
                         IA64_ARG(BYTE *pBStore));

    int NotifyUserOfFault(bool userBreakpoint, DebuggerLaunchSetting dls);

    SIZE_T GetArgCount(MethodDesc* md, BOOL *fVarArg = NULL);

    void FuncEvalComplete(Thread *pThread, DebuggerEval *pDE);

    DebuggerMethodInfo *CreateMethodInfo(Module *module, mdMethodDef md);
    void JITBeginning(MethodDesc* fd, bool trackJITInfo);
    void JITComplete(MethodDesc* fd, BYTE* newAddress, SIZE_T sizeOfCode,
                     JITCodeInfo jitCodeInfo);

    HRESULT RequestFavor(FAVORCALLBACK fp, void * pData);


    void GetVarInfo(MethodDesc *       fd,         // [IN] method of interest
                    void *DebuggerVersionToken,    // [IN] which edit version
                    SIZE_T *           cVars,      // [OUT] size of 'vars'
                    const NativeVarInfo **vars     // [OUT] map telling where local vars are stored
                    );

    void * __stdcall allocateArray(ULONG cBytes);
    void __stdcall freeArray(void *array);

    void __stdcall getBoundariesHelper(CORINFO_METHOD_HANDLE ftn,
                                       unsigned int *cILOffsets, DWORD **pILOffsets);
    void __stdcall getBoundaries(CORINFO_METHOD_HANDLE ftn,
                                 unsigned int *cILOffsets, DWORD **pILOffsets,
                                 ICorDebugInfo::BoundaryTypes* implictBoundaries);
    void __stdcall setBoundaries(CORINFO_METHOD_HANDLE ftn,
                                 ULONG32 cMap, OffsetMapping *pMap);

    void __stdcall getVars(CORINFO_METHOD_HANDLE ftn,
                           ULONG32 *cVars, ILVarInfo **vars,
                           bool *extendOthers);
    void __stdcall setVars(CORINFO_METHOD_HANDLE ftn,
                           ULONG32 cVars, NativeVarInfo *vars);

    DebuggerMethodInfo *GetOrCreateMethodInfo(Module *pModule, mdMethodDef token);

    DebuggerMethodInfoTable * GetMethodInfoTable() { return m_pMethodInfos; }

    // Gets the DJI for 'fd'
    // If 'pbAddr' is non-NULL and if the method has multiple copies of code
    // (because of EnC or code-pitching), this returns the DJI corresponding
    // to 'pbAddr'
    DebuggerJitInfo *GetJitInfo(MethodDesc *fd, const BYTE *pbAddr, DebuggerMethodInfo **pMethInfo = NULL);

    // Several ways of getting a DJI. DJIs are 1:1 w/ Native Code blobs.
    // Caller must guarantee good parameters.
    // DJIs can be lazily created; so the only way these will fail is in an OOM case.
    DebuggerJitInfo *GetJitInfoFromAddr(TADDR addr);

    // EnC trashes the methoddesc to point to the latest version. Thus given a method-desc,
    // we can get the most recent DJI.
    DebuggerJitInfo *GetLatestJitInfoFromMethodDesc(MethodDesc * pMethodDesc);


    HRESULT GetILToNativeMapping(MethodDesc *pMD, ULONG32 cMap, ULONG32 *pcMap,
                                 COR_DEBUG_IL_TO_NATIVE_MAP map[]);

    PRD_TYPE GetPatchedOpcode(CORDB_ADDRESS_TYPE *ip);
    BOOL CheckGetPatchedOpcode(CORDB_ADDRESS_TYPE *address, /*OUT*/ PRD_TYPE *pOpcode);

    void TraceCall(const BYTE *address);
    void PossibleTraceCall(UMEntryThunk *pUMEntryThunk, Frame *pFrame);

    bool ThreadsAtUnsafePlaces(void);

    void PitchCode( MethodDesc *fd, const BYTE *pbAddr );

    void MovedCode( MethodDesc *fd, CORDB_ADDRESS_TYPE *pbOldAddress,
                    CORDB_ADDRESS_TYPE *pbNewAddress);


    void PollWaitingForHelper();

    void IncThreadsAtUnsafePlaces(void)
    {
        LEAF_CONTRACT;
        InterlockedIncrement(&m_threadsAtUnsafePlaces);
    }

    void DecThreadsAtUnsafePlaces(void)
    {
        LEAF_CONTRACT;
        InterlockedDecrement(&m_threadsAtUnsafePlaces);
    }

    static StackWalkAction AtSafePlaceStackWalkCallback(CrawlFrame *pCF,
                                                        VOID* data);
    bool IsThreadAtSafePlaceWorker(Thread *thread);
    bool IsThreadAtSafePlace(Thread *thread);

    CorDebugUserState GetFullUserState(Thread *pThread);


    void Terminate();
    void Continue();

    bool HandleIPCEvent(DebuggerIPCEvent* event, IpcTarget iWhich);

    DebuggerModule* LookupModule(Module* pModule, AppDomain *pAppDomain);

    HRESULT GetAndSendSyncBlockFieldInfo(Object *pObject,
                                         DebuggerIPCE_BasicTypeData *objectType,
                                         SIZE_T offsetToVars,
                                         mdFieldDef fldToken,
                                         DebuggerRCThread* rcThread,
                                         IpcTarget iWhich);

    HRESULT GetAndSendFunctionData(DebuggerRCThread* rcThread,
                                   mdMethodDef methodToken,
                                   DebuggerModule* functionModuleToken,
                                   SIZE_T nVersion,
                                   IpcTarget iWhich);

    HRESULT GetAndSendInterceptCommand(DebuggerIPCEvent *event, IpcTarget iWhich);

    //HRESULT GetAndSendJITFunctionData(DebuggerRCThread* rcThread,
    //                               mdMethodDef methodToken,
    //                               void* functionModuleToken,
    //                               IpcTarget iWhich);
    HRESULT GetFuncData(mdMethodDef funcMetadataToken,
                        DebuggerModule* pDebuggerModule,
                        SIZE_T nVersion,
                        DebuggerIPCE_FuncData *data);


    HRESULT GetAndSendObjectInfo(DebuggerRCThread* rcThread,
                                 AppDomain *pAppDomain,
                                 void* objectRefAddress,
                                 OBJECTHANDLE objectInHandle,
                                 bool objectRefIsValue,
                                 CorElementType objectType,
                                 IpcTarget iWhich);

    HRESULT GetAndSendClassInfo(DebuggerRCThread* rcThread,
                                TypeHandle th,
                                TypeHandle thApprox,
                                BOOL fInstantiatedType, // false when getting fields for a constructed type
                                AppDomain *pAppDomain,
                                mdFieldDef fldToken, // for special use by GASSBFI, above
                                FieldDesc **pFD, //OUT
                                IpcTarget iWhich);

    HRESULT GetAndSendTypeHandleParams(DebuggerRCThread* rcThread,
                                       AppDomain *pAppDomain,
                                       TypeHandle pTypeHandle,
                                       IpcTarget iWhich);

    HRESULT GetAndSendExpandedTypeInfo(DebuggerRCThread* rcThread,
                                     AppDomain *pAppDomain,
                                     TypeHandle pTypeHandle,
                                     IpcTarget iWhich);

    HRESULT GetAndSendTypeHandle(DebuggerRCThread* rcThread,
                                 AppDomain *pAppDomain,
                                 DebuggerIPCE_ExpandedTypeData* typeData,
                                 unsigned int genericArgsCount,
                                 DebuggerIPCE_BasicTypeData *genericArgData,
                                 IpcTarget iWhich);

    HRESULT GetAndSendApproxTypeHandle(DebuggerRCThread* rcThread,
                                 AppDomain *pAppDomain,
                                 unsigned int nTypeData,
                                 DebuggerIPCE_TypeArgData *pTypeData,
                                 IpcTarget iWhich);

    // The following four functions convert between type handles and the data that is
    // shipped for types to and from the right-side.
    //
    // I'm heading toward getting rid of the first two - they are almost never used.
    static HRESULT ExpandedTypeInfoToTypeHandle(DebuggerIPCE_ExpandedTypeData *data,
                                                unsigned int genericArgsCount,
                                                DebuggerIPCE_BasicTypeData *genericArgs,
                                                TypeHandle *pRes);
    static HRESULT BasicTypeInfoToTypeHandle(DebuggerIPCE_BasicTypeData *data,
                                             TypeHandle *pRes);
    void TypeHandleToBasicTypeInfo(AppDomain *pAppDomain,
                                   TypeHandle th,
                                   DebuggerIPCE_BasicTypeData *res,
                                   IpcTarget iWhich) ;

    // TypeHandleToExpandedTypeInfo returns different DebuggerIPCE_ExpandedTypeData objects
    // depending on whether the object value that the TypeData corresponds to is
    // boxed or not.  Different parts of the API transfer objects in slightly different ways.
    // AllBoxed:
    //    For GetAndSendObjectData all values are boxed,
    //
    // StructsBoxed:
    //     When returning results from FuncEval only "true" structs
    //     get boxed, i.e. primitives are unboxed.
    //
    // NoSpecialBoxing:
    //     TypeHandleToExpandedTypeInfo is also used to report type parameters,
    //      and in this case none of the types are considered boxed (
    enum AreValueTypesBoxed { NoValueTypeBoxing, OnlyPrimitivesUnboxed, AllBoxed };

    void TypeHandleToExpandedTypeInfo(AreValueTypesBoxed boxed,
                                      AppDomain *pAppDomain,
                                      TypeHandle th,
                                      DebuggerIPCE_ExpandedTypeData *res, IpcTarget iWhich);

    class TypeDataWalk
    {
        DebuggerIPCE_TypeArgData *m_curdata;
        unsigned int m_remaining;

    public:
        TypeDataWalk(DebuggerIPCE_TypeArgData *pData, unsigned int nData)
        {
            m_curdata = pData;
            m_remaining = nData;
        }


        // These are for type arguments in the funceval case.
        // They throw COMPLUS exceptions if they fail, so can only be used during funceval.
        void ReadTypeHandles(unsigned int nTypeArgs, TypeHandle *pRes);
        TypeHandle ReadInstantiation(Module *pModule, mdTypeDef tok, unsigned int nTypeArgs);
        TypeHandle ReadTypeHandle();

        // These are like the above but return null if the item could not be found.
        // They also optionally find the canonical form for the specified type
        // (used if generic code sharing is enabled) even if the exact form has not
        // yet been loaded for some reason
        TypeHandle ReadLoadedTypeArg(BOOL fCanon);
        BOOL ReadLoadedTypeHandles(BOOL fCanon, unsigned int nTypeArgs, TypeHandle *ppResults);
        TypeHandle ReadLoadedInstantiation(BOOL fCanon, Module *pModule, mdTypeDef tok, unsigned int nTypeArgs);
        TypeHandle ReadLoadedTypeHandle(BOOL fCanonTypeArgs);
        void Skip();
        BOOL Finished() { LEAF_CONTRACT; return m_remaining == 0; }
        DebuggerIPCE_TypeArgData *ReadOne() { LEAF_CONTRACT; if (m_remaining) { m_remaining--; return m_curdata++; } else return NULL; }

    };



    HRESULT GetAndSendMethodDescParams(DebuggerRCThread* rcThread,
                                       AppDomain *pAppDomain,
                                       MethodDesc *pFD,
                                       void *exactGenericArgsToken,
                                       IpcTarget iWhich);

    HRESULT GetAndSendSpecialStaticInfo(DebuggerRCThread *rcThread,
                                        FieldDesc *pField,
                                        Thread *pRuntimeThread,
                                        IpcTarget iWhich);

    HRESULT GetAndSendJITInfo(DebuggerRCThread* rcThread,
                              DebuggerJitInfo *pJITInfo,
                              AppDomain *pAppDomain,
                              IpcTarget iWhich);

    HRESULT GetMethodDescData(MethodDesc *pFD,
                              DebuggerJitInfo *pJITInfo,
                              DebuggerIPCE_JITFuncData *data);

    void GetAndSendTransitionStubInfo(CORDB_ADDRESS_TYPE *stubAddress,
                                      IpcTarget iWhich);

    void SendBreakpoint(Thread *thread, CONTEXT *context,
                        DebuggerBreakpoint *breakpoint);

    void SendStep(Thread *thread, CONTEXT *context,
                  DebuggerStepper *stepper,
                  CorDebugStepReason reason);

    void LockAndSendEnCRemapEvent(DebuggerJitInfo * dji, SIZE_T currentIP, SIZE_T *resumeIP);
    void LockAndSendEnCRemapCompleteEvent(MethodDesc *pFD);
    void SendEnCUpdateEvent(DebuggerIPCEventType eventType, Module *pModule, mdToken memberToken, mdTypeDef classToken, SIZE_T enCVersion);
    void LockAndSendBreakpointSetError(PATCH_UNORDERED_ARRAY * listUnbindablePatches);

    HRESULT SendException(Thread *pThread,
                          bool firstChance,
                          SIZE_T currentIP,
                          SIZE_T currentSP
                          IA64_ARG(SIZE_T currentBSP),
                          bool continuable,
                          bool fAttaching,
                          bool fForceNonInterceptable,
                          BOOL useManagedDebugger
                         );

    void SendUserBreakpoint(Thread *thread, __in_z __in_opt WCHAR* szMessage = NULL);
    void SendRawUserBreakpoint(Thread *thread);
    void SendInterceptExceptionComplete(Thread *thread);

    HRESULT AttachDebuggerForBreakpoint(Thread *thread,
                                        __in_opt __in_z WCHAR *wszLaunchReason);


    void ThreadIsSafe(Thread *thread);

    void UnrecoverableError(HRESULT errorHR,
                            unsigned int errorCode,
                            const char *errorFile,
                            unsigned int errorLine,
                            bool exitThread);

    BOOL IsSynchronizing(void)
    {
        LEAF_CONTRACT;

        return m_trappingRuntimeThreads;
    }

    //
    // The debugger mutex is used to protect any "global" Left Side
    // data structures. The RCThread takes it when handling a Right
    // Side event, and Runtime threads take it when processing
    // debugger events.
    //
#ifdef _DEBUG
    int m_mutexCount;
#endif

    // Helper function
    HRESULT AttachDebuggerForBreakpointOnHelperThread(Thread *pThread);

    // helper function to send Exception IPC event and Exception_CallBack2 event
    HRESULT SendExceptionCallBackHelper(
        Thread      *pThread,
        OBJECTHANDLE exceptionHandle,
        bool        continuable,
        FramePointer framePointer,
        SIZE_T      nOffset,
        CorDebugExceptionCallbackType eventType,
        DWORD       dwFlags);

    // helper function to call SendExceptionCallBackHelper on helper thread
    HRESULT SendExceptionOnHelperThread(
        Thread      *pThread,
        OBJECTHANDLE exceptionHandle,
        bool        continuable,
        FramePointer framePointer,
        SIZE_T      nOffset,
        CorDebugExceptionCallbackType eventType,
        DWORD       dwFlags);

    // Helper function to call send LogMessage and finish attach logic on helper thread
    HRESULT SendRawLogMessageOnHelperThread(
        Thread                                     *pThread,
        AppDomain                                  *pAppDomain,
        int                                         iLevel,
        __in_z __in_ecount(iCategoryLen + 1) WCHAR *pCategory,
        int                                         iCategoryLen,
        __in_ecount(iMessageLen + 1) WCHAR         *pMessage,
        int                                         iMessageLen);

    // Helper function to send out LogMessage only. Can be either on helper thread or manager thread.
    void Debugger::SendRawLogMessage(
        Thread                                    *pThread,
        AppDomain                                 *pAppDomain,
        int                                        iLevel,
        __in_z __in_ecount(iCategoryLen+1) WCHAR  *pCategory,
        int                                        iCategoryLen,
        __in_ecount(iMessageLen+1) WCHAR          *pMessage,
        int                                        iMessageLen);


    // Helper function to send MDA notification
    void SendRawMDANotification(SendMDANotificationParams * params);
    static void SendMDANotificationOnHelperThreadProxy(SendMDANotificationParams * params);

private:
    void DoNotCallDirectlyPrivateLock(void);
    void DoNotCallDirectlyPrivateUnlock(void);

#ifndef DACCESS_COMPILE
    static void AcquireDebuggerLock(Debugger *c)
    {
        WRAPPER_CONTRACT;
        c->DoNotCallDirectlyPrivateLock();
    }

    static void ReleaseDebuggerLock(Debugger *c)
    {
        WRAPPER_CONTRACT;
        c->DoNotCallDirectlyPrivateUnlock();
    }
#else // DACCESS_COMPILE
    static void AcquireDebuggerLock(Debugger *c);
    static void ReleaseDebuggerLock(Debugger *c);
#endif // DACCESS_COMPILE


public:
    // define type for DebuggerLockHolder
    typedef DacHolder<Debugger *, Debugger::AcquireDebuggerLock, Debugger::ReleaseDebuggerLock> DebuggerLockHolder;

    void LockForEventSending(DebuggerLockHolder *dbgLockHolder, BOOL fNoRetry = FALSE);
    void UnlockFromEventSending(DebuggerLockHolder *dbgLockHolder);
    void SyncAllThreads(DebuggerLockHolder *dbgLockHolder);
    void SendSyncCompleteIPCEvent();

    // Helper for sending a single pre-baked IPC event and blocking on the continue.
    // See definition of SENDIPCEVENT_BEGIN for usage pattern.
    void SendSimpleIPCEventAndBlock(DebuggerLockHolder *pdbgLockHolder, IpcTarget iTarget = IPC_TARGET_OUTOFPROC);

    void IncrementClassLoadCallbackCount(void)
    {
        LEAF_CONTRACT;
        InterlockedIncrement(&m_dClassLoadCallbackCount);
    }

    void DecrementClassLoadCallbackCount(void)
    {
        LEAF_CONTRACT;
        _ASSERTE(m_dClassLoadCallbackCount > 0);
        InterlockedDecrement(&m_dClassLoadCallbackCount);
    }


#ifdef _DEBUG_IMPL
    bool ThreadHoldsLock(void)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        CONTRACTL_END;

        if (g_fProcessDetach)
            return true;
        if (g_pEEInterface->GetThread())
        {
            return (GetThreadIdHelper(g_pEEInterface->GetThread()) == m_mutexOwner);
        }
        else
        {
            return (GetCurrentThreadId() == m_mutexOwner);
        }
    }
#endif // _DEBUG_IMPL


    //
    // InsertToMethodInfoList puts the given DMI onto the DMI list.
    //
    HRESULT InsertToMethodInfoList(DebuggerMethodInfo *dmi);


    // MapBreakpoints will map any and all breakpoints (except EnC
    //      patches) from previous versions of the method into the current version.
    HRESULT MapAndBindFunctionPatches( DebuggerJitInfo *pJiNew,
        MethodDesc * fd,
        CORDB_ADDRESS_TYPE * addrOfCode);

    // MPTDJI takes the given patch (and djiFrom, if you've got it), and
    // does the IL mapping forwards to djiTo.  Returns
    // CORDBG_E_CODE_NOT_AVAILABLE if there isn't a mapping, which means that
    // no patch was placed.
    HRESULT MapPatchToDJI(DebuggerControllerPatch *dcp, DebuggerJitInfo *djiTo);

    HRESULT  MapThroughVersions(SIZE_T fromIL,
                                DebuggerMethodInfo *dmiFrom,
                                SIZE_T *toIL,
                                DebuggerMethodInfo *dmiTo,
                                BOOL fMappingForwards,
                                BOOL *fAccurate);

    HRESULT LaunchDebuggerForUser (void);

    void SendLogMessage (int iLevel,
                         __in_z __in_ecount(iCategoryLen + 1) WCHAR *pCategory,
                         int iCategoryLen,
                         __in_ecount(iMessageLen + 1) WCHAR *pMessage,
                         int iMessageLen);

    void SendLogSwitchSetting (int iLevel,
                               int iReason,
                               __in_z WCHAR *pLogSwitchName,
                               __in_z WCHAR *pParentSwitchName);

    bool IsLoggingEnabled (void)
    {
        LEAF_CONTRACT;

        if (m_LoggingEnabled)
            return true;
        return false;
    }

    // Send an MDA notification. This ultimately translates to an ICorDebugMDA object on the Right-Side.
    void SendMDANotification(
        Thread * pThread, // may be NULL. Lets us send on behalf of other threads.
        SString * szName,
        SString * szDescription,
        SString * szXML,
        CorDebugMDAFlags flags,
        BOOL bAttach
    );


    void EnableLogMessages (bool fOnOff) {LEAF_CONTRACT;  m_LoggingEnabled = fOnOff;}
    bool GetILOffsetFromNative (MethodDesc *PFD, const BYTE *pbAddr,
                                DWORD nativeOffset, DWORD *ilOffset);

    DWORD GetHelperThreadID(void );


    HRESULT SetIP( bool fCanSetIPOnly,
                   Thread *thread,
                   Module *module,
                   mdMethodDef mdMeth,
                   DebuggerJitInfo* dji,
                   SIZE_T offsetILTo,
                   BOOL fIsIL,
                   void *firstExceptionHandler);

    // Helper routines used by Debugger::SetIP
    HRESULT ShuffleVariablesGet(DebuggerJitInfo  *dji,
                                SIZE_T            offsetFrom,
                                CONTEXT          *pCtx,
                                SIZE_T          **prgVal1,
                                SIZE_T          **prgVal2,
                                BYTE           ***prgpVCs);

    HRESULT ShuffleVariablesSet(DebuggerJitInfo  *dji,
                             SIZE_T            offsetTo,
                             CONTEXT          *pCtx,
                             SIZE_T          **prgVal1,
                             SIZE_T          **prgVal2,
                             BYTE            **rgpVCs);

    HRESULT GetVariablesFromOffset(MethodDesc                 *pMD,
                                   UINT                        varNativeInfoCount,
                                   ICorJitInfo::NativeVarInfo *varNativeInfo,
                                   SIZE_T                      offsetFrom,
                                   CONTEXT                    *pCtx,
                                   SIZE_T                     *rgVal1,
                                   SIZE_T                     *rgVal2,
                                   UINT                       uRgValSize, // number of element of the preallocated rgVal1 and rgVal2
                                   BYTE                     ***rgpVCs);

    HRESULT SetVariablesAtOffset(MethodDesc                 *pMD,
                                 UINT                        varNativeInfoCount,
                                 ICorJitInfo::NativeVarInfo *varNativeInfo,
                                 SIZE_T                      offsetTo,
                                 CONTEXT                    *pCtx,
                                 SIZE_T                     *rgVal1,
                                 SIZE_T                     *rgVal2,
                                 BYTE                      **rgpVCs);

    BOOL IsThreadContextInvalid(Thread *pThread);

    // notification for SQL fiber debugging support
    void CreateConnection(CONNID dwConnectionId, __in_z WCHAR *wzName, BOOL fAttaching);
    void DestroyConnection(CONNID dwConnectionId);
    void ChangeConnection(CONNID dwConnectionId, BOOL fAttaching);

    //
    // This function is used to identify the helper thread.
    //
    bool ThisIsHelperThread(void);

    HRESULT ReDaclEvents(PSECURITY_DESCRIPTOR securityDescriptor);

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    BOOL ShouldAutoAttach();
    BOOL FallbackJITAttachPrompt();
    HRESULT SetFiberMode(bool isFiberMode);
    BOOL IsDebuggerAttached();

    HRESULT AddAppDomainToIPC (AppDomain *pAppDomain);
    HRESULT RemoveAppDomainFromIPC (AppDomain *pAppDomain);
    HRESULT UpdateAppDomainEntryInIPC (AppDomain *pAppDomain);
    HRESULT IterateAppDomainsForAttach(AttachAppDomainEventsEnum EventsToSend, BOOL *fEventSent, BOOL fAttaching);
    HRESULT AttachDebuggerToAppDomain(ULONG id);
    HRESULT MarkAttachingAppDomainsAsAttachedToDebugger(void);
    HRESULT DetachDebuggerFromAppDomain(ULONG id, AppDomain **ppAppDomain);

    void SendCreateAppDomainEvent (AppDomain *pAppDomain,
                                   BOOL fAttaching);
    void SendExitAppDomainEvent (AppDomain *pAppDomain);

    void LoadAssembly(AppDomain* pRuntimeAppDomain,
                      Assembly *pAssembly,
                      BOOL fSystem,
                      BOOL fAttaching);
    void UnloadAssembly(AppDomain *pAppDomain,
                        Assembly* pAssembly);

    HRESULT FuncEvalSetup(DebuggerIPCE_FuncEvalInfo *pEvalInfo, BYTE **argDataArea, DebuggerEval **debuggerEvalKey);
    HRESULT FuncEvalSetupReAbort(Thread *pThread, Thread::ThreadAbortRequester requester);
    HRESULT FuncEvalAbort(DebuggerEval *debuggerEvalKey);
    HRESULT FuncEvalRudeAbort(DebuggerEval *debuggerEvalKey);
    HRESULT FuncEvalCleanup(DebuggerEval *debuggerEvalKey);

    HRESULT SetReference(void *objectRefAddress, LSPTR_OBJECTHANDLE objectHandle, void *newReference);
    HRESULT SetValueClass(void *oldData, void *newData, DebuggerIPCE_BasicTypeData *type);

    HRESULT SetILInstrumentedCodeMap(MethodDesc *fd,
                                     BOOL fStartJit,
                                     ULONG32 cILMapEntries,
                                     COR_IL_MAP rgILMapEntries[]);

    void EarlyHelperThreadDeath(void);

    void ShutdownBegun(void);

    void LockDebuggerForShutdown(void);

    void DisableDebugger(void);

    // Pid of the left side process that this Debugger instance is in.
    DWORD GetPid(void) { return m_processId; }

    // Virtual RPC to Virtual Left side, called by the in-proc Cordb
    // Note that this means that all calls from the Left side are synchronous
    // with respect to the thread that's making the calls.
    // See also: CordbgRCEvent::VrpcToVrs
    HRESULT VrpcToVls(DebuggerIPCEvent *event);

    HRESULT NameChangeEvent(AppDomain *pAppDomain, Thread *pThread);

    // This acquires a lock on the jit patch table, iterates through the table,
    // and eliminates all the patches / controllers that are specific to
    // the given domain.  Used as part of the AppDomain detach logic.
    void ClearAppDomainPatches(AppDomain *pAppDomain);

    BOOL SendCtrlCToDebugger(DWORD dwCtrlType);

    // Allows the debugger to keep an up to date list of special threads
    HRESULT UpdateSpecialThreadList(DWORD cThreadArrayLength, DWORD *rgdwThreadIDArray);

    // Updates the pointer for the debugger services
    void SetIDbgThreadControl(IDebuggerThreadControl *pIDbgThreadControl);

#ifndef DACCESS_COMPILE
    static void AcquireDebuggerDataLock(Debugger *pDebugger)
    {
        WRAPPER_CONTRACT;

        if (!g_fProcessDetach)
        {
            pDebugger->GetDebuggerDataLock()->Enter();
        }
    }

    static void ReleaseDebuggerDataLock(Debugger *pDebugger)
    {
        WRAPPER_CONTRACT;

        if (!g_fProcessDetach)
        {
            pDebugger->GetDebuggerDataLock()->Leave();
        }
    }
#else // DACCESS_COMPILE
    static void AcquireDebuggerDataLock(Debugger *pDebugger);
    static void ReleaseDebuggerDataLock(Debugger *pDebugger);
#endif // DACCESS_COMPILE

    // define type for DebuggerDataLockHolder
    typedef DacHolder<Debugger *, Debugger::AcquireDebuggerDataLock, Debugger::ReleaseDebuggerDataLock> DebuggerDataLockHolder;

#ifdef _DEBUG
    // Use for asserts
    bool HasDebuggerDataLock()
    {
        // If no lazy data yet, then can't possibly have the debugger-data lock.
        if (!g_pDebugger->HasLazyData())
        {
            return false;
        }
        return (g_pDebugger->GetDebuggerDataLock()->OwnedByCurrentThread()) != 0;
    }
#endif


    AppDomainEnumerationIPCBlock *GetAppDomainEnumIPCBlock() {LEAF_CONTRACT;  return m_pAppDomainCB; }

    // For Just-My-Code (aka Just-User-Code).
    // The jit injects probes in debuggable managed methods that look like:
    // if (*pFlag != 0) call JIT_DbgIsJustMyCode.
    // pFlag is unique per-method constant determined by GetJMCFlagAddr.
    // JIT_DbgIsJustMyCode will get the ip & fp and call OnMethodEnter.

    // pIP is an ip within the method, right after the prolog.
#ifndef DACCESS_COMPILE
    virtual void OnMethodEnter(void * pIP);
    virtual DWORD* GetJMCFlagAddr(Module * pModule);
#endif

    // GetJMCFlagAddr provides a unique flag for each module. UpdateModuleJMCFlag
    // will go through all modules with user-code and set their flag to fStatus.
    void UpdateAllModuleJMCFlag(bool fStatus);
    void UpdateModuleJMCFlag(DebuggerModule * pPrimary, bool fStatus);


    friend bool DebuggerIsInvolved();
#ifndef DACCESS_COMPILE
    static DWORD GetThreadIdHelper(Thread *pThread);
#endif // DACCESS_COMPILE

private:
    DebuggerJitInfo *GetJitInfoWorker(MethodDesc *fd, const BYTE *pbAddr, DebuggerMethodInfo **pMethInfo);
    

    void InitDebugEventCounting();
    void    DoHelperThreadDuty();

    typedef enum
    {
        ATTACH_YES,
        ATTACH_NO,
        ATTACH_TERMINATE
    } ATTACH_ACTION;

    // Returns true if the debugger is not attached and DbgJITDebugLaunchSetting
    // is set to either ATTACH_DEBUGGER or ASK_USER and the user request attaching.
    ATTACH_ACTION ShouldAttachDebugger(bool fIsUserBreakpoint, UnhandledExceptionLocation location);
    ATTACH_ACTION ShouldAttachDebuggerProxy(bool fIsUserBreakpoint, UnhandledExceptionLocation location);
    friend void ShouldAttachDebuggerStub(ShouldAttachDebuggerParams * p);
    friend struct ShouldAttachDebuggerParams;

    void TrapAllRuntimeThreads(DebuggerLockHolder *dbgLockHolder, AppDomain *pAppDomain);
    void ReleaseAllRuntimeThreads(AppDomain *pAppDomain);

#ifndef DACCESS_COMPILE
    // Do not call this function unless you are getting ThreadId from RS
    void InitIPCEventWithThreadId(DebuggerIPCEvent *ipce,
                      DebuggerIPCEventType type,
                      DWORD threadId,
                      LSPTR_APPDOMAIN pAppDomainToken)
    {
        LEAF_CONTRACT;

        _ASSERTE(ipce != NULL);
        ipce->type = type;
        ipce->hr = S_OK;
        ipce->processId = m_processId;
        ipce->appDomainToken = pAppDomainToken;
        ipce->threadId = threadId;
    }

    void InitIPCEvent(DebuggerIPCEvent *ipce,
                      DebuggerIPCEventType type,
                      Thread *pThread,
                      AppDomain* pAppDomain)
    {
        WRAPPER_CONTRACT;

        InitIPCEvent(ipce, type, pThread, LSPTR_APPDOMAIN::MakePtr(pAppDomain));
    }

    // Let this function to figure out the unique Id that we will use for Thread.
    void InitIPCEvent(DebuggerIPCEvent *ipce,
                      DebuggerIPCEventType type,
                      Thread *pThread,
                      LSPTR_APPDOMAIN pAppDomainToken)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
        }
        CONTRACTL_END;

        _ASSERTE(ipce != NULL);
        ipce->type = type;
        ipce->hr = S_OK;
        ipce->processId = m_processId;
        ipce->appDomainToken = pAppDomainToken;
        if (pThread)
        {
            ipce->threadId = GetThreadIdHelper(pThread);
        }
        else
        {
            ipce->threadId = 0;
        }
    }

    void InitIPCEvent(DebuggerIPCEvent *ipce,
                      DebuggerIPCEventType type)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(type == DB_IPCE_SYNC_COMPLETE ||
                 type == DB_IPCE_GET_SYNC_BLOCK_FIELD_RESULT);

        Thread *pThread = g_pEEInterface->GetThread();
        AppDomain *pAppDomain = NULL;

        if (pThread)
        {
            pAppDomain = pThread->GetDomain();
        }

        InitIPCEvent(ipce,
                     type,
                     pThread,
                     LSPTR_APPDOMAIN::MakePtr(pAppDomain));
    }
#endif // DACCESS_COMPILE

    HRESULT GetFunctionInfo(Module *pModule,
                            mdToken functionToken,
                            //MethodDesc **ppFD,
                            ULONG *pRVA,
                            BYTE **pCodeStart,
                            unsigned int *pCodeSize,
                            mdToken *pLocalSigToken);
    HRESULT GetAndSendBuffer(DebuggerRCThread* rcThread, ULONG bufSize);

    HRESULT SendReleaseBuffer(DebuggerRCThread* rcThread, BYTE *pBuffer);

public:
    HRESULT ReleaseRemoteBuffer(BYTE *pBuffer, bool removeFromBlobList);
private:


    void ComputeFieldData( FieldDesc* fd,
                                     BYTE * pGCStaticsBase,
                                     BYTE * pNonGCStaticsBase,
                                     DebuggerIPCE_FieldData * currentFieldData );

    WCHAR *GetDebuggerLaunchStringTemplate(BOOL useManagedDebugger);
    bool GetCompleteDebuggerLaunchString(AppDomain* pAppDomain, LPCWSTR wszAttachReason, BOOL bUseManagedDebugger, SString * pStrArgsBuf);

    HRESULT EnsureDebuggerAttached(AppDomain *pAppDomain,
                                   LPCWSTR exceptionName,
                                   BOOL useManagedDebugger);
    HRESULT EDAHelper(AppDomain *pAppDomain, LPCWSTR wszAttachReason, BOOL useManagedDebugger);

    HRESULT EDAHelperProxy(AppDomain *pAppDomain, LPCWSTR exceptionName, BOOL useManagedDebugger);
    friend void EDAHelperStub(EnsureDebuggerAttachedParams * p);

    HRESULT FinishEnsureDebuggerAttached();

    DebuggerLaunchSetting GetDbgJITDebugLaunchSetting(void);


public:
    HRESULT InitAppDomainIPC(void);
    HRESULT TerminateAppDomainIPC(void);

    ULONG IsDebuggerAttachedToAppDomain(Thread *pThread);

    bool ResumeThreads(AppDomain* pAppDomain);

    static DWORD WaitForSingleObjectHelper(HANDLE handle, DWORD dwMilliseconds);

    void ProcessAnyPendingEvals(Thread *pThread);

    bool HasLazyData();
    RCThreadLazyInit * GetRCThreadLazyData();

    DebuggerModuleTable          * GetModuleTable();
    DebuggerHeap                 *GetInteropSafeHeap();
    DebuggerHeap                 *GetInteropSafeHeap_NoThrow();
    DebuggerLazyInit             *GetLazyData();
    HelperCanary * GetCanary();
    void MarkDebuggerAttachedInternal();
    void MarkDebuggerUnattachedInternal();
    bool IsAttachInProgress();

private:
    DebuggerPendingFuncEvalTable *GetPendingEvals() { return GetLazyData()->m_pPendingEvals; }
    SIZE_T_UNORDERED_ARRAY * GetBPMappingDuplicates() { return &GetLazyData()->m_BPMappingDuplicates; }
    HANDLE                GetAttachAbortEvent()     { return GetLazyData()->m_exAttachAbortEvent; }
    HANDLE                GetAttachEvent()          { return  GetLazyData()->m_exAttachEvent; }
    HANDLE                GetUnmanagedAttachEvent() { return  GetLazyData()->m_exUnmanagedAttachEvent; }
    BOOL                  GetDebuggerHandlingCtrlC() { return GetLazyData()->m_DebuggerHandlingCtrlC; }
    void                  SetDebuggerHandlingCtrlC(BOOL f) { GetLazyData()->m_DebuggerHandlingCtrlC = f; }
    HANDLE                GetCtrlCMutex()           { return GetLazyData()->m_CtrlCMutex; }
    UnorderedBytePtrArray* GetMemBlobs()            { return &GetLazyData()->m_pMemBlobs; }


    DebuggerRCThread*     m_pRCThread;
    DWORD                 m_processId; // our pid
    BOOL                  m_trappingRuntimeThreads;
    BOOL                  m_stopped;
    BOOL                  m_unrecoverableError;
    BOOL                  m_ignoreThreadDetach;
    PTR_DebuggerMethodInfoTable   m_pMethodInfos;


    // This is the main debugger lock. It is a large lock and used to synchronize complex operations
    // such as sending IPC events, debugger sycnhronization, and attach / detach. 
    // The debugger effectively can't make any radical state changes without holding this lock.
    // 
    Crst                  m_mutex; // The main debugger lock.

    // Flag to track if the debugger Crst needs to go into "Shutdown for Finalizer" mode.
    // This means that only special shutdown threads (helper / finalizer / shutdown) can
    // take the lock, and all others will just block forever if they take it.
    bool                  m_fShutdownMode;

    //
    // Flag to track if the VM has told the debugger that it should block all threads
    // as soon as possible as it goes thru the debugger.  As of this writing, this is
    // done via the debugger Crst, anyone attempting to take the lock will block forever.
    //
    bool                  m_fDisabled;

#ifdef _DEBUG
    // Ownership tracking for debugging.
    DWORD                 m_mutexOwner;

    // Tid that last called LockForEventSending.
    DWORD                 m_tidLockedForEventSending;
#endif
    DebuggerAttachState   m_syncingForAttach;
    LONG                  m_threadsAtUnsafePlaces;
    BOOL                  m_attachingForException;
    LONG                  m_exLock;

    BOOL                  m_LoggingEnabled;
    AppDomainEnumerationIPCBlock    *m_pAppDomainCB;

    LONG                  m_dClassLoadCallbackCount;


    DebuggerModuleTable          *m_pModules;
    BOOL                          m_debuggerAttached;

public:
    IDebuggerThreadControl       *m_pIDbgThreadControl;


    BOOL                          m_forceNonInterceptable;


private:
    Crst                  *GetDebuggerDataLock()        { return &GetLazyData()-> m_DebuggerDataLock; }

    // This is lazily inititalized. It's just a wrapper around a handle so we embed it here.
    DebuggerHeap                 m_heap;

    DebuggerLazyInit             *m_pLazyData;
};



extern "C" {
void __stdcall FuncEvalHijack(void);
void * __stdcall FuncEvalHijackWorker(DebuggerEval *pDE);
};


// CNewZeroData is the allocator used by the all the hash tables that the helper thread could possibly alter. It uses
// the interop safe allocator.
class CNewZeroData
{
public:
#ifndef DACCESS_COMPILE
    static BYTE *Alloc(int iSize, int iMaxSize)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(g_pDebugger != NULL);
        }
        CONTRACTL_END;

        DebuggerHeap * pHeap = g_pDebugger->GetInteropSafeHeap_NoThrow();
        if (pHeap == NULL)
        {
            return NULL;
        }

        BYTE *pb = (BYTE *) pHeap->Alloc(iSize);
        if (pb == NULL)
        {
            return NULL;
        }

        memset(pb, 0, iSize);
        return pb;
    }
    static void Free(BYTE *pPtr, int iSize)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(g_pDebugger != NULL);
        }
        CONTRACTL_END;


        DebuggerHeap * pHeap = g_pDebugger->GetInteropSafeHeap_NoThrow();
        _ASSERTE(pHeap != NULL); // should already exist

        pHeap->Free(pPtr);
    }
    static BYTE *Grow(BYTE *&pPtr, int iCurSize)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(g_pDebugger != NULL);
        }
        CONTRACTL_END;

        void *p;

        DebuggerHeap* pHeap = g_pDebugger->GetInteropSafeHeap_NoThrow();
        _ASSERTE(pHeap != NULL); // should already exist 

        PREFIX_ASSUME( iCurSize >= 0 );
        S_UINT32 iNewSize = S_UINT32( iCurSize ) + S_UINT32( GrowSize(iCurSize) );
        if( iNewSize.IsOverflow() )
        {
            return NULL;
        }
        p = pHeap->Realloc(pPtr, iNewSize.Value(), iCurSize);
        if (p == NULL)
        {
            return NULL;
        }

        memset((BYTE*)p+iCurSize, 0, GrowSize(iCurSize));
        return (pPtr = (BYTE *)p);
    }

    // A hashtable may recycle memory. We need to zero it out again.
    static void Clean(BYTE * pData, int iSize)
    {
        LEAF_CONTRACT;

        memset(pData, 0, iSize);
    }
#endif // DACCESS_COMPILE

    static int RoundSize(int iSize)
    {
        LEAF_CONTRACT;

        return (iSize);
    }
    static int GrowSize(int iCurSize)
    {
        LEAF_CONTRACT;
        int newSize = (3 * iCurSize) / 2;
        return (newSize < 256) ? 256 : newSize;
    }
};

class DebuggerPendingFuncEvalTable : private CHashTableAndData<CNewZeroData>
{
  private:

    BOOL Cmp(const BYTE *pc1, const HASHENTRY *pc2)
    {
        LEAF_CONTRACT;
        return ((Thread*)pc1) != ((DebuggerPendingFuncEval*)pc2)->pThread;
    }

    ULONG HASH(Thread* pThread)
    { 
        LEAF_CONTRACT;
        return (ULONG)((SIZE_T)pThread);   // only use low 32-bits if 64-bit
    }


    BYTE *KEY(Thread* pthread)
    {
        LEAF_CONTRACT;
        return (BYTE *) pthread;
    }

  public:

#ifndef DACCESS_COMPILE
    DebuggerPendingFuncEvalTable() : CHashTableAndData<CNewZeroData>(11)
    {
        WRAPPER_CONTRACT;

        SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
        NewInit(11, sizeof(DebuggerPendingFuncEval), 11);
    }

    void AddPendingEval(Thread *pThread, DebuggerEval *pDE)
    {
        WRAPPER_CONTRACT;

        _ASSERTE((pThread != NULL) && (pDE != NULL));

        DebuggerPendingFuncEval *pfe = (DebuggerPendingFuncEval*)Add(HASH(pThread));
        pfe->pThread = pThread;
        pfe->pDE = pDE;
    }

    void RemovePendingEval(Thread* pThread)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(pThread != NULL);

        DebuggerPendingFuncEval *entry = (DebuggerPendingFuncEval*)Find(HASH(pThread), KEY(pThread));
        Delete(HASH(pThread), (HASHENTRY*)entry);
   }

#endif // #ifndef DACCESS_COMPILE

    DebuggerPendingFuncEval *GetPendingEval(Thread* pThread)
    {
        WRAPPER_CONTRACT;

        DebuggerPendingFuncEval *entry = (DebuggerPendingFuncEval*)Find(HASH(pThread), KEY(pThread));
        return entry;
    }
};

struct DebuggerModuleEntry
{
    FREEHASHENTRY   entry;
    DebuggerModule* module;
};

class DebuggerModuleTable : private CHashTableAndData<CNewZeroData>
{
  private:

    BOOL Cmp(const BYTE *pc1, const HASHENTRY *pc2)
    {
        LEAF_CONTRACT;
        return ((Module*)pc1) != ((DebuggerModuleEntry*)pc2)->module->GetRuntimeModule();
    }

    ULONG HASH(Module* module)
    {
        LEAF_CONTRACT;
        return (ULONG)((SIZE_T)module);   // only use low 32-bits if 64-bit
    }

    BYTE *KEY(Module* module)
    {
        LEAF_CONTRACT;
        return (BYTE *) module;
    }

#ifdef _DEBUG
    bool ThreadHoldsLock();
#endif

public:

#ifndef DACCESS_COMPILE

    DebuggerModuleTable();
    ~DebuggerModuleTable();

    void AddModule(DebuggerModule *module);

    void RemoveModule(Module* module, AppDomain *pAppDomain);


    void Clear();

    //
    // RemoveModules removes any module loaded into the given appdomain from the hash.  This is used when we send an
    // ExitAppdomain event to ensure that there are no leftover modules in the hash. This can happen when we have shared
    // modules that aren't properly accounted for in the CLR. We miss sending UnloadModule events for those modules, so
    // we clean them up with this method.
    //
    void RemoveModules(AppDomain *pAppDomain);
#endif // #ifndef DACCESS_COMPILE

    DebuggerModule *GetModule(Module* module);

    // We should never look for a NULL Module *
    DebuggerModule *GetModule(Module* module, AppDomain* pAppDomain);
    DebuggerModule *GetFirstModule(HASHFIND *info);
    DebuggerModule *GetNextModule(HASHFIND *info);
};

// struct DebuggerMethodInfoKey:   Key for each of the method info hash table entries.
// Module * m_pModule:  This and m_token make up the key
// mdMethodDef m_token:  This and m_pModule make up the key
typedef DPTR(struct DebuggerMethodInfoKey) PTR_DebuggerMethodInfoKey;
struct DebuggerMethodInfoKey
{
    PTR_Module          pModule;
    mdMethodDef         token;
} ;

// struct DebuggerMethodInfoEntry:  Entry for the JIT info hash table.
// FREEHASHENTRY entry:   Needed for use by the hash table
// DebuggerMethodInfo * ji:   The actual DebuggerMethodInfo to
//          hash.  Note that DMI's will be hashed by MethodDesc.
typedef DPTR(struct DebuggerMethodInfoEntry) PTR_DebuggerMethodInfoEntry;
struct DebuggerMethodInfoEntry
{
    FREEHASHENTRY          entry;
    DebuggerMethodInfoKey  key;
    SIZE_T                 nVersion;
    SIZE_T                 nVersionLastRemapped;
    PTR_DebuggerMethodInfo   mi;

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
};

// class DebuggerMethodInfoTable:   Hash table to hold all the non-JIT related
// info for each method we see.  The JIT infos live in a seperate table
// keyed by MethodDescs - there may be multiple
// JITted realizations of each MethodDef, e.g. under different generic
// assumptions.  Hangs off of the Debugger object.
// INVARIANT: There is only one DebuggerMethodInfo per method
// in the table. Note that DMI's will be hashed by MethodDesc.
//
class DebuggerMethodInfoTable : private CHashTableAndData<CNewZeroData>
{
    VPTR_BASE_CONCRETE_VTABLE_CLASS(DebuggerMethodInfoTable);

  private:

    BOOL Cmp(const BYTE *pc1, const HASHENTRY *pc2)
    {
        LEAF_CONTRACT;

        DebuggerMethodInfoKey *pDjik =
            (DebuggerMethodInfoKey*)(pc1);
        DebuggerMethodInfoEntry *pDjie =
            PTR_DebuggerMethodInfoEntry(PTR_HOST_TO_TADDR((void*)pc2));

        return pDjik->pModule != pDjie->key.pModule ||
               pDjik->token != pDjie->key.token;
    }

    ULONG HASH(DebuggerMethodInfoKey* pDjik)
    {
        LEAF_CONTRACT;
        return HashPtr( pDjik->token, pDjik->pModule );
    }

    BYTE *KEY(DebuggerMethodInfoKey* djik)
    {
        LEAF_CONTRACT;

        return (BYTE *) djik;
    }

//#define _DEBUG_DMI_TABLE

#ifdef _DEBUG_DMI_TABLE
public:
    ULONG CheckDmiTable();

#define CHECK_DMI_TABLE (CheckDmiTable())
#define CHECK_DMI_TABLE_DEBUGGER (m_pMethodInfos->CheckDmiTable())

#else

#define CHECK_DMI_TABLE
#define CHECK_DMI_TABLE_DEBUGGER

#endif // _DEBUG_DMI_TABLE

  public:

#ifndef DACCESS_COMPILE

    DebuggerMethodInfoTable();

    HRESULT AddMethodInfo(Module *pModule,
                       mdMethodDef token,
                       DebuggerMethodInfo *mi);
                       
    HRESULT OverwriteMethodInfo(Module *pModule,
                             mdMethodDef token,
                             DebuggerMethodInfo *mi,
                             BOOL fOnlyIfNull);

    // pModule is being unloaded - remove any entries that belong to it.  Why?
    // (a) Correctness: the module can be reloaded at the same address,
    //      which will cause accidental matches with our hashtable (indexed by
    //      {Module*,mdMethodDef}
    // (b) Perf: don't waste the memory!
    void ClearMethodsOfModule(Module *pModule);
    void RemoveMethodInfo(Module *pModule, mdMethodDef token);
    void DeleteEntryDMI(DebuggerMethodInfoEntry *entry);
    
#endif // #ifndef DACCESS_COMPILE

    DebuggerMethodInfo *GetMethodInfo(Module *pModule, mdMethodDef token);
    DebuggerMethodInfo *GetFirstMethodInfo(HASHFIND *info);
    DebuggerMethodInfo *GetNextMethodInfo(HASHFIND *info);

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
};

/* ------------------------------------------------------------------------ *
 * DebuggerEval class
 *
 * Note that arguments get passsed in a block allocated when
 * the func-eval is set up.  The setup phase passes the total count of arguments.
 *
 * In some situations type arguments must also be passed, e.g.
 * when performing a "newarr" operation or calling a generic function with a
 * "funceval". In the setup phase we pass a count of the number of
 * nodes in the "flattened" type expressions for the type arguments, if any.
 * e.g. for calls to non-generic code this is 0.
 *    - for "newobj List<int>" this is 1: there is one type argument "int".
 *    - for "newobj Dict<string,int>" this is 2: there are two
 *      type arguments "string" and "int".
 *    - for "newobj Dict<string,List<int>>" this is 3: there are two
        type arguments but the second contains two nodes (one for List and one for int).
 * The type argument will get placed in the allocated argument block,
 * the order being determined by the order they occur in the tree, i.e.
 * left-to-right, top-to-bottom in the type expressions tree, e.g. for
 * type arguments <string,List<int>> you get string followed by List followed by int.
 * ------------------------------------------------------------------------ */

class DebuggerEval
{
public:

    //
    // Used as a bit field.
    //
    enum FUNC_EVAL_ABORT_TYPE
    {
        FE_ABORT_NONE = 0,
        FE_ABORT_NORMAL = 1,
        FE_ABORT_RUDE = 2
    };

    // Note: this first field must be big enough to hold a breakpoint
    // instruction, and it MUST be the first field. (This
    // is asserted in debugger.cpp)
    BYTE                           m_breakpointInstruction[CORDbg_BREAK_INSTRUCTION_SIZE];
    CONTEXT                        m_context;
    Thread                        *m_thread;
    DebuggerIPCE_FuncEvalType      m_evalType;
    mdMethodDef                    m_methodToken;
    mdTypeDef                      m_classToken;
    ADID                           m_appDomainId;       // Safe even if AD unloaded
    DebuggerModule                *m_debuggerModule;     // Only valid if AD is still around
    RSPTR_CORDBEVAL                m_funcEvalKey;
    bool                           m_successful;        // Did the eval complete successfully
    Debugger::AreValueTypesBoxed   m_retValueBoxing;        // Is the return value boxed?
    SIZE_T                         m_argCount;
    SIZE_T                         m_genericArgsCount;
    SIZE_T                         m_genericArgsNodeCount;
    SIZE_T                         m_stringSize;
    BYTE                          *m_argData;
    MethodDesc                    *m_md;
    const BYTE                    *m_targetCodeAddr;
    INT64                          m_result;
    TypeHandle                     m_resultType;
    SIZE_T                         m_arrayRank;
    FUNC_EVAL_ABORT_TYPE           m_aborting;          // Has an abort been requested, and what type.
    bool                           m_aborted;           // Was this eval aborted
    bool                           m_completed;          // Is the eval complete - successfully or by aborting
    bool                           m_evalDuringException;
    bool                           m_rethrowAbortException;
    Thread::ThreadAbortRequester   m_requester;         // For aborts, what kind?
    LSPTR_OBJECTHANDLE             m_objectHandle;
    TypeHandle                     m_ownerTypeHandle;

    DebuggerEval(CONTEXT *context, DebuggerIPCE_FuncEvalInfo *pEvalInfo, bool fInException)
    {
        WRAPPER_CONTRACT;

        // This must be non-zero so that the saved opcode is non-zero, and on IA64 we want it to be 0x16
        // so that we can have a breakpoint instruction in any slot in the bundle.
        m_breakpointInstruction[0] = 0x16;
        m_thread = pEvalInfo->funcDebuggerThreadToken.UnWrap();
        m_evalType = pEvalInfo->funcEvalType;
        m_methodToken = pEvalInfo->funcMetadataToken;
        m_classToken = pEvalInfo->funcClassMetadataToken;

        // Note: we can't rely on just the DebuggerModule* or AppDomain* because the AppDomain
        // could get unloaded between now and when the funceval actually starts.  So we stash an 
        // AppDomain ID which is safe to use after the AD is unloaded.  It's only safe to 
        // use the DebuggerModule* after we've verified the ADID is still valid (i.e. by entering that domain).
        m_debuggerModule = pEvalInfo->funcDebuggerModuleToken.UnWrap();
        if( m_debuggerModule == NULL )
        {
            // We have no associated code.
            _ASSERTE( (m_evalType == DB_IPCE_FET_NEW_STRING) || (m_evalType == DB_IPCE_FET_NEW_ARRAY) );
            
            // We'll just do the creation in whatever domain the thread is already in.
            // It's conceivable that we might want to allow the caller to specify a specific domain, but
            // ICorDebug provides the debugger with no was to specify the domain.
            m_appDomainId = m_thread->GetDomain()->GetId();
        }
        else
        {
            m_appDomainId = m_debuggerModule->GetAppDomain()->GetId();
        }

        m_funcEvalKey = pEvalInfo->funcEvalKey;
        m_argCount = pEvalInfo->argCount;
        m_targetCodeAddr = NULL;
        m_stringSize = pEvalInfo->stringSize;
        m_arrayRank = pEvalInfo->arrayRank;
        m_genericArgsCount = pEvalInfo->genericArgsCount;
        m_genericArgsNodeCount = pEvalInfo->genericArgsNodeCount;
        m_successful = false;
        m_argData = NULL;
        m_result = 0;
        m_md = NULL;
        m_resultType = TypeHandle();
        m_aborting = FE_ABORT_NONE;
        m_aborted = false;
        m_completed = false;
        m_evalDuringException = fInException;
        m_rethrowAbortException = false;
        m_retValueBoxing = Debugger::NoValueTypeBoxing;
        m_requester = (Thread::ThreadAbortRequester)0;
        m_objectHandle = LSPTR_OBJECTHANDLE::NullPtr();

        // Copy the thread's context.
        if (context == NULL)
            memset(&m_context, 0, sizeof(m_context));
        else
            memcpy(&m_context, context, sizeof(m_context));
    }

    // This constructor is only used when setting up an eval to re-abort a thread.
    DebuggerEval(CONTEXT *context, Thread *pThread, Thread::ThreadAbortRequester requester)
    {
        WRAPPER_CONTRACT;

        // This must be non-zero so that the saved opcode is non-zero, and on IA64 we want it to be 0x16
        // so that we can have a breakpoint instruction in any slot in the bundle.
        m_breakpointInstruction[0] = 0x16;
        m_thread = pThread;
        m_evalType = DB_IPCE_FET_RE_ABORT;
        m_methodToken = mdMethodDefNil;
        m_classToken = mdTypeDefNil;
        m_debuggerModule = NULL;
        m_funcEvalKey = RSPTR_CORDBEVAL::NullPtr();
        m_argCount = 0;
        m_stringSize = 0;
        m_arrayRank = 0;
        m_genericArgsCount = 0;
        m_genericArgsNodeCount = 0;
        m_successful = false;
        m_argData = NULL;
        m_targetCodeAddr = NULL;
        m_result = 0;
        m_md = NULL;
        m_resultType = TypeHandle();
        m_aborting = FE_ABORT_NONE;
        m_aborted = false;
        m_completed = false;
        m_evalDuringException = false;
        m_rethrowAbortException = false;
        m_retValueBoxing = Debugger::NoValueTypeBoxing;
        m_requester = requester;
        // Copy the thread's context.
        memcpy(&m_context, context, sizeof(m_context));
        if (context == NULL)
            memset(&m_context, 0, sizeof(m_context));
        else
            memcpy(&m_context, context, sizeof(m_context));
    }

    bool Init()
    {
        _ASSERTE(DbgIsExecutable(&m_breakpointInstruction, sizeof(m_breakpointInstruction)));
        return true;
    }


    // The m_argData buffer holds both the type arg data (for generics) and the main argument data.
    //
    // For DB_IPCE_FET_NEW_STRING it holds the data specifying the string to create.
    DebuggerIPCE_TypeArgData *GetTypeArgData()
    {
        LEAF_CONTRACT;
        return (DebuggerIPCE_TypeArgData *) (m_argData);
    }

    DebuggerIPCE_FuncEvalArgData *GetArgData()
    {
        LEAF_CONTRACT;
        return (DebuggerIPCE_FuncEvalArgData*) (m_argData + m_genericArgsNodeCount * sizeof(DebuggerIPCE_TypeArgData));
    }

    WCHAR *GetNewStringArgData()
    {
        LEAF_CONTRACT;
        _ASSERTE(m_evalType == DB_IPCE_FET_NEW_STRING);
        return (WCHAR*)m_argData;
    }

    ~DebuggerEval()
    {
        WRAPPER_CONTRACT;

        // Clean up any temporary buffers used to send the argument type information.  These were allocated
        // in respnse to a GET_BUFFER message
        DebuggerIPCE_FuncEvalArgData *argData = GetArgData();
        for (unsigned int i = 0; i < m_argCount; i++)
        {
            if (argData[i].fullArgType != NULL)
            {
                _ASSERTE(g_pDebugger != NULL);
                g_pDebugger->ReleaseRemoteBuffer((BYTE*)argData[i].fullArgType, true);
            }
        }

        // Clean up the array of argument information.  This was allocated as part of Func Eval setup.
        if (m_argData)
        {
            DeleteInteropSafe(m_argData);
        }

#ifdef _DEBUG
        // Set flags to strategic values in case we access deleted memory.
        m_completed = false;
        m_rethrowAbortException = true;
#endif        
    }
};

/* ------------------------------------------------------------------------ *
 * New/delete overrides to use the debugger's private heap
 * ------------------------------------------------------------------------ */

class InteropSafe {};
#define interopsafe (*(InteropSafe*)NULL)

#ifndef DACCESS_COMPILE
static inline void * __cdecl operator new(size_t n, const InteropSafe&)
{
    CONTRACTL
    {
        THROWS; // throw on OOM
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(g_pDebugger != NULL);
    void *result = g_pDebugger->GetInteropSafeHeap()->Alloc((DWORD)n);
    if (result == NULL) {
        ThrowOutOfMemory();
    }
    return result;
}

static inline void * __cdecl operator new[](size_t n, const InteropSafe&)
{
    CONTRACTL
    {
        THROWS; // throw on OOM
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    _ASSERTE(g_pDebugger != NULL);
    void *result = g_pDebugger->GetInteropSafeHeap()->Alloc((DWORD)n);
    if (result == NULL) {
        ThrowOutOfMemory();
    }
    return result;
}

static inline void * __cdecl operator new(size_t n, const InteropSafe&, const NoThrow&)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(g_pDebugger != NULL);
    DebuggerHeap * pHeap = g_pDebugger->GetInteropSafeHeap_NoThrow();
    if (pHeap == NULL)
    {
        return NULL;
    }
    void *result = pHeap->Alloc((DWORD)n);
    return result;
}

static inline void * __cdecl operator new[](size_t n, const InteropSafe&, const NoThrow&)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(g_pDebugger != NULL);
    DebuggerHeap * pHeap = g_pDebugger->GetInteropSafeHeap_NoThrow();
    if (pHeap == NULL)
    {
        return NULL;
    }
    void *result = pHeap->Alloc((DWORD)n);
    return result;
}

// Note: there is no C++ syntax for manually invoking this, but if a constructor throws an exception I understand that
// this delete operator will be invoked automatically to destroy the object.
static inline void __cdecl operator delete(void *p, const InteropSafe&)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (p != NULL)
    {
        _ASSERTE(g_pDebugger != NULL);
        DebuggerHeap * pHeap = g_pDebugger->GetInteropSafeHeap_NoThrow();
        _ASSERTE(pHeap != NULL); // should have had heap around if we're deleting
        pHeap->Free(p);
    }
}

// Note: there is no C++ syntax for manually invoking this, but if a constructor throws an exception I understand that
// this delete operator will be invoked automatically to destroy the object.
static inline void __cdecl operator delete[](void *p, const InteropSafe&)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (p != NULL)
    {
        _ASSERTE(g_pDebugger != NULL);
        DebuggerHeap * pHeap = g_pDebugger->GetInteropSafeHeap_NoThrow();
        _ASSERTE(pHeap != NULL); // should have had heap around if we're deleting

        pHeap->Free(p);
    }
}

//
// Interop safe delete to match the interop safe new's above. There is no C++ syntax for actually invoking those interop
// safe delete operators above, so we use this method to accomplish the same thing.
//
template<class T> void DeleteInteropSafe(T *p)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Don't stop a thread that may hold the Interop-safe heap lock.
    // It may be in preemptive, but it's still "inside" the CLR and so inside the "Can't-Stop-Region"
    CantStopHolder hHolder;

    if (p != NULL)
    {
        p->T::~T();

        _ASSERTE(g_pDebugger != NULL);
        DebuggerHeap * pHeap = g_pDebugger->GetInteropSafeHeap_NoThrow();
        _ASSERTE(pHeap != NULL); // should have had heap around if we're deleting

        pHeap->Free(p);
    }
}
#endif // DACCESS_COMPILE


#if _DEBUG
#define DBG_RUNTIME_MAX ((DB_IPCE_RUNTIME_LAST&0xff)+1)
#define DBG_DEBUGGER_MAX ((DB_IPCE_DEBUGGER_LAST&0xff)+1)

#define DbgLog(event) DbgLogHelper(event)
void DbgLogHelper(DebuggerIPCEventType event);
#else
#define DbgLog(event)
#endif // _DEBUG

//-----------------------------------------------------------------------------
// Helpers for cleanup
// These are various utility functions, mainly where we factor out code.
//-----------------------------------------------------------------------------
void GetPidDecoratedName(__out_z __in_ecount(cBufSizeInChars) WCHAR * pBuf,
                         int cBufSizeInChars,
                         const WCHAR * pPrefix);

// Specify type of Win32 event
enum EEventResetType {
    kManualResetEvent = TRUE,
    kAutoResetEvent = FALSE
};

HANDLE CreateWin32EventOrThrow(
    LPSECURITY_ATTRIBUTES lpEventAttributes,
    EEventResetType eType,
    BOOL bInitialState
);

HANDLE OpenWin32EventOrThrow(
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    LPCWSTR lpName
);

//
class IPCHostSecurityAttributeHolder
{
public:
    IPCHostSecurityAttributeHolder(DWORD pid);
    ~IPCHostSecurityAttributeHolder();

    SECURITY_ATTRIBUTES * GetHostSA();

protected:
    SECURITY_ATTRIBUTES *m_pSA; // the resource we're protecting.
};

// Common data used to send an IPC Event.
class SendEventData
{
public:
    bool fPreGCDisabled;
    Debugger::DebuggerLockHolder * pDbgLockHolder;
};
void PreSendEvent_Internal(SendEventData & data);
void PostSendEvent_Internal(SendEventData & data);

#define SENDIPCEVENT_BEGIN(__pDbgLockHolder) \
{ \
    SendEventData __event_data; \
    __event_data.pDbgLockHolder = __pDbgLockHolder; \
    PreSendEvent_Internal(__event_data); \

#define SENDIPCEVENT_END \
    PostSendEvent_Internal(__event_data); \
}

//
//
#define SENDEVENT_CONTRACT_ITEMS \
    GC_NOTRIGGER; \
    MODE_PREEMPTIVE; \
    PRECONDITION(g_pDebugger->ThreadHoldsLock()); \
    PRECONDITION(!g_pDebugger->IsStopped()); \



//
//
//

//
//



#include "debugger.inl"


//
//
//
//  The below contract defines should only be used (A) if they apply, and (B) they are the LEAST
// definitive for the function you are contracting.  The below defines represent the baseline contract
// for each case.
//
// e.g. If a function FOO() throws, always, you should use THROWS, not any of the below.
//
//
//
#if _DEBUG

#define MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT \
      if (!m_pRCThread->IsRCThreadReady()) { THROWS; } else { NOTHROW; }

#define MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT \
      if (!m_pRCThread->IsRCThreadReady() || (GetThread() != NULL)) { GC_TRIGGERS; } else { GC_NOTRIGGER; }

#define GC_TRIGGERS_FROM_GETJITINFO if (GetThread() != NULL) { GC_TRIGGERS; } else { GC_NOTRIGGER; }

#define CALLED_IN_DEBUGGERDATALOCK_HOLDER_SCOPE_MAY_GC_TRIGGERS_CONTRACT WRAPPER(GC_TRIGGERS)

#else

#define MAY_DO_HELPER_THREAD_DUTY_THROWS_CONTRACT
#define MAY_DO_HELPER_THREAD_DUTY_GC_TRIGGERS_CONTRACT
#define CALLED_IN_DEBUGGERDATALOCK_HOLDER_SCOPE_MAY_GC_TRIGGERS_CONTRACT

#define GC_TRIGGERS_FROM_GETJITINFO

#endif

bool DbgIsSpecialILOffset(DWORD offset);

#endif /* DEBUGGER_H_ */

