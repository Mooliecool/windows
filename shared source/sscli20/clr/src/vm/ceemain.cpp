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
// ===========================================================================
// File: CEEMAIN.CPP
//
// ===========================================================================

#include "common.h"

// Declare global variables
#define DECLARE_DATA
#include "vars.hpp"
#include "veropcodes.hpp"
#undef DECLARE_DATA

#include "dbgalloc.h"
#include "log.h"
#include "ceemain.h"
#include "clsload.hpp"
#include "object.h"
#include "hash.h"
#include "ecall.h"
#include "ceemain.h"
#include "dllimport.h"
#include "syncblk.h"
#include "comstring.h"
#include "comsystem.h"
#include "eeconfig.h"
#include "stublink.h"
#include "handletable.h"
#include "method.hpp"
#include "codeman.h"
#include "gcscan.h"
#include "frames.h"
#include "threads.h"
#include "stackwalk.h"
#include "gc.h"
#include "interoputil.h"
#include "security.h"
#include "fieldmarshaler.h"
#include "dbginterface.h"
#include "eedbginterfaceimpl.h"
#include "debugdebugger.h"
#include "cordbpriv.h"
#include "remoting.h"
#include "comdelegate.h"
#include "appdomain.hpp"
#include "perfcounters.h"
#include "rwlock.h"
#include "ipcmanagerinterface.h"
#include "eventtrace.h"
#include "timeline.h"
#include "internaldebug.h"
#include "corhost.h"
#include "binder.h"
#include "olevariant.h"
#include "comcallablewrapper.h"
#include "apithreadstress.h"
#include "ipcfunccall.h"
#include "perflog.h"
#include "../dlls/mscorrc/resource.h"
#include "comnlsinfo.h"
#include "util.hpp"
#include "shimload.h"
#include "comthreadpool.h"
#include "stackprobe.h"
#include "posterror.h"
#include "timeline.h"
#include "memoryreport.h"
#include "virtualcallstub.h"
#include "strongname.h"
#include "syncclean.hpp"
#include "typeparse.h"
#include "debuginfostore.h"
#include "mdaassistants.h"
#include "eemessagebox.h"




#ifdef PROFILING_SUPPORTED
#include "proftoeeinterfaceimpl.h"
#endif // PROFILING_SUPPORTED

#include "mdaassistants.h"

HRESULT RunDllMain(MethodDesc *pMD, HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved);

static HRESULT InitializeIPCManager(void);
static void TerminateIPCManager(void);

static int GetThreadUICultureId();

static HRESULT GetThreadUICultureNames(__out_ecount(cchCultureName) LPWSTR wszCultureName, SIZE_T cchCultureName,
                                   __out_ecount(cchParentCultureName) LPWSTR wszParentCultureName, SIZE_T cchParentCultureName);

HRESULT EEStartup(COINITIEE fFlags);
extern "C" HRESULT STDMETHODCALLTYPE InitializeFusion();

HRESULT PrepareExecuteDLLForThunk(HINSTANCE hInst,
                                  DWORD dwReason,
                                  LPVOID lpReserved);
BOOL STDMETHODCALLTYPE ExecuteDLL(HINSTANCE hInst,
                                  DWORD dwReason,
                                  LPVOID lpReserved,
                                  BOOL fFromThunk);
BOOL STDMETHODCALLTYPE ExecuteEXE(HMODULE hMod);
BOOL STDMETHODCALLTYPE ExecuteEXE(__in LPWSTR pImageNameIn);

static void InitializeEventTracing();

static HRESULT InitializeGarbageCollector();

void InitFastInterlockOps(); // cgenxxx.cpp

#ifdef DEBUGGING_SUPPORTED
static void InitializeDebugger(void);
static void TerminateDebugger(void);
extern "C" HRESULT __cdecl CorDBGetInterface(DebugInterface** rcInterface);
static void GetDbgProfControlFlag();
#endif // DEBUGGING_SUPPORTED

HRESULT InitializeDebugStore();
void TerminateDebugStore();

#ifdef PROFILING_SUPPORTED
static HRESULT InitializeProfiling();
static void TerminateProfiling(BOOL fProcessDetach);
#endif // PROFILING_SUPPORTED

// Remember how the last startup of EE went.
HRESULT g_EEStartupStatus = S_OK;

// Flag indicating if the EE has been started.
BOOL    g_fEEStarted = FALSE;

// Flag indicating if the EE should be suspended on shutdown.
BOOL    g_fSuspendOnShutdown = FALSE;

// Event to synchronize EE shutdown.
static CLREvent * g_pEEShutDownEvent;

// ---------------------------------------------------------------------------
// %%Function: EnsureEEStarted()
//
// Description: Ensure the CLR is started.
// ---------------------------------------------------------------------------
HRESULT EnsureEEStarted(COINITIEE flags)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        ENTRY_POINT;
    }
    CONTRACTL_END;

    if (g_fEEShutDown)
        return E_FAIL;

    HRESULT hr = E_FAIL;

    // On non x86 platforms, when we load mscorlib.dll during EEStartup, we will
    // re-enter _CorDllMain with a DLL_PROCESS_ATTACH for mscorlib.dll. We are
    // far enough in startup that this is allowed, however we don't want to
    // re-start the startup code so we need to check to see if startup has
    // been initiated or completed before we call EEStartup. We do however want to
    // make sure other threads block until the EE is started.
    if (!g_fEEStarted)
    {
	BEGIN_ENTRYPOINT_NOTHROW;


        IHostTaskManager *pHostTaskManager = CorHost2::GetHostTaskManager();
        if (pHostTaskManager)
        {
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
            pHostTaskManager->BeginThreadAffinity();
            END_SO_TOLERANT_CODE_CALLING_HOST;
        }

        volatile static LONG fOnlyOne = 0;

        BOOL bStarted=FALSE;

        while (FastInterlockExchange(&fOnlyOne, 1) == 1)
        {
            __SwitchToThread(0);
        }

        // Now that we've acquired the lock, check again to make sure we aren't in
        // the process of starting the CLR or that it hasn't already been fully started.
        // At this point, if startup has been inited we don't have anything more to do.
        // And if EEStartup already failed before, we don't do it again.
        if (!g_fEEStarted && !g_fEEInit && SUCCEEDED (g_EEStartupStatus))
        {
            EEStartup(flags);
            bStarted=g_fEEStarted;
            hr = g_EEStartupStatus;
        }
        else
        {
            hr = g_EEStartupStatus;
            if (SUCCEEDED(g_EEStartupStatus))
            {
                hr = S_FALSE;
            }
        }

        fOnlyOne = 0;

        if (pHostTaskManager)
        {
            BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
            pHostTaskManager->EndThreadAffinity();
            END_SO_TOLERANT_CODE_CALLING_HOST;
        }
        END_ENTRYPOINT_NOTHROW;
    }
    else
    {
        hr = g_EEStartupStatus;
        if (SUCCEEDED(g_EEStartupStatus))
        {
            hr = S_FALSE;
        }
    }

    return hr;
}

// ---------------------------------------------------------------------------
// %%Function: ForceEEShutdown()
//
// Description: Force the EE to shutdown now.
// ---------------------------------------------------------------------------
void ForceEEShutdown()
{
    WRAPPER_CONTRACT;

    // Don't bother to take the lock for this case.

    STRESS_LOG0(LF_STARTUP, INFO3, "EEShutdown invoked from ForceEEShutdown");
    EEPolicy::HandleExitProcess();
}

// This is our Ctrl-C, Ctrl-Break, etc. handler.
static BOOL WINAPI DbgCtrlCHandler(DWORD dwCtrlType)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

#ifdef DEBUGGING_SUPPORTED
    if (CORDebuggerAttached() &&
        (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_BREAK_EVENT))
    {
        return g_pDebugInterface->SendCtrlCToDebugger(dwCtrlType);
    }
    else
#endif // DEBUGGING_SUPPORTED
    {
        g_fInControlC = true;     // only for weakening assertions in checked build.
        return FALSE;               // keep looking for a real handler.
    }
}

// A host can specify that it only wants one version of hosting interface to be used.
BOOL g_singleVersionHosting;

// ---------------------------------------------------------------------------
// %%Function: GetStartupInformation
//
// Get Configuration Information
// ---------------------------------------------------------------------------
typedef HRESULT (STDMETHODCALLTYPE* pGetHostConfigurationFile)(LPCWSTR, SIZE_T*);
HRESULT GetStartupInformation()
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(return E_OUTOFMEMORY);
    } CONTRACTL_END;
     GCHeap::InitializeHeapType(false);
    return S_OK;
}


//-----------------------------------------------------------------------------


void InitGSCookie()
{
    GSCookie * pGSCookiePtr = GetProcessGSCookiePtr();

    DWORD oldProtection;
    ClrVirtualProtect((LPVOID)pGSCookiePtr, sizeof(GSCookie), PAGE_EXECUTE_READWRITE, &oldProtection);

    GSCookie val = (GSCookie)GetTickCount();

#ifdef _DEBUG
    // In _DEBUG, always use the same value to make it easier to search for the cookie
    val = (GSCookie) WIN64_ONLY(0x9ABCDEF012345678) NOT_WIN64(0x12345678);
#endif

    // To test if it is initialized. Also for ICorMethodInfo::getGSCookie()
    if (val == 0)
        val ++;
    *pGSCookiePtr = val;

    ClrVirtualProtect((LPVOID)pGSCookiePtr, sizeof(GSCookie), oldProtection, &oldProtection);
}


// ---------------------------------------------------------------------------
// %%Function: EEStartupHelper
//
// Parameters:
//  fFlags                  - Initialization flags for the engine.  See the
//                              EEStartupFlags enumerator for valid values.
//
// Returns:
//  S_OK                    - On success
//
// Description:
//  Reserved to initialize the EE runtime engine explicitly.
// ---------------------------------------------------------------------------

#ifndef IfFailGotoLog
#define IfFailGotoLog(EXPR, LABEL) \
do { \
    hr = (EXPR);\
    if(FAILED(hr)) { \
        STRESS_LOG2(LF_STARTUP, LL_ALWAYS, "%s failed with code %x", #EXPR, hr);\
        goto LABEL; \
    } \
    else \
       STRESS_LOG1(LF_STARTUP, LL_ALWAYS, "%s completed", #EXPR);\
} while (0)
#endif

#ifndef IfFailGoLog
#define IfFailGoLog(EXPR) IfFailGotoLog(EXPR, ErrExit)
#endif

void EEStartupHelper(COINITIEE fFlags)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    static ConfigDWORD breakOnEELoad;
    BOOL    initEEConfig = FALSE;
    EX_TRY
    {
#ifdef _DEBUG
    DisableGlobalAllocStore();
#endif //_DEBUG
    g_fEEInit = true;

    ::SetConsoleCtrlHandler(DbgCtrlCHandler, TRUE/*add*/);

    // Initialize event tracing early so we can trace CLR startup time events.
    InitializeEventTracing();

    ETWTraceStartup trace(ETW_TYPE_STARTUP_EESTARTUP);
    TIMELINE_AUTO(STARTUP, "EEStartup");

    // Give PerfMon a chance to hook up to us
    // Do this both *before* and *after* ipcman init so corperfmonext.dll
    // has a chance to release stale private blocks that IPCMan could collide with.
    // do this early to maximize window between perfmon refresh and ipc block creation.
    IPCFuncCallSource::DoThreadSafeCall();

    InitGSCookie();
    Frame::Init();


    // Initialize EEConfig
    if (!g_pConfig)
    {
        IfFailGo(EEConfig::Setup());
        initEEConfig = TRUE;
    }
    // Go get configuration information this is necessary
    // before the EE has started. This will also get any host-supplied
    // configuration file, which EEConfig can then use.
    GetStartupInformation();

    // get any configuration information from the registry
    if (initEEConfig)
        IfFailGo(g_pConfig->SetupConfiguration());


#ifdef STRESS_LOG
    if (REGUTIL::GetConfigDWORD(L"StressLog", g_pConfig->StressLog ()) != 0) {
        unsigned facilities = REGUTIL::GetConfigDWORD(L"LogFacility", LF_ALL);
        unsigned level = REGUTIL::GetConfigDWORD(L"LogLevel", LL_INFO1000);
        unsigned bytesPerThread = REGUTIL::GetConfigDWORD(L"StressLogSize", STRESSLOG_CHUNK_SIZE * 4);
        unsigned totalBytes = REGUTIL::GetConfigDWORD(L"TotalStressLogSize", STRESSLOG_CHUNK_SIZE * 1024);
        StressLog::Initialize(facilities, level, bytesPerThread, totalBytes, GetModuleInst());
        g_pStressLog = &StressLog::theLog;
    }
#endif

#ifdef LOGGING
    InitializeLogging();
#endif

#ifdef ENABLE_PERF_LOG
    PerfLog::PerfLogInitialize();
#endif //ENABLE_PERF_LOG

    STRESS_LOG0(LF_STARTUP, LL_ALWAYS, "===================EEStartup Starting===================");



    // SString
    SString::Startup();

    // Fusion
    {
        ETWTraceStartup traceFusion(ETW_TYPE_STARTUP_FUSIONINIT);
        IfFailGoLog(InitializeFusion());
    }


#if ENABLE_TIMELINE
    Timeline::Startup();
#endif

    InitEventStore();

    if (g_pConfig != NULL)
    {
        IfFailGoLog(g_pConfig->sync());        
    }

    if (breakOnEELoad.val(L"BreakOnEELoad", 0) == 1)
    {
#ifdef _DEBUG
        _ASSERTE(!"Start loading EE!");
#else
                DebugBreak();
#endif
        }




    // Initialize all our InterProcess Communications with COM+
    IfFailGoLog(InitializeIPCManager());

#ifdef ENABLE_PERF_COUNTERS
    hr = PerfCounters::Init();
    _ASSERTE(SUCCEEDED(hr));
    IfFailGo(hr);
#endif


    StubManager::InitializeStubManagers();

    IfFailGoLog(InitializeDebugStore());

    // Set up the cor handle map. This map is used to load assemblies in
    // memory instead of using the normal system load
    PEImage::Startup();


    EECodeInfo::Init();
    HardCodedMetaSig::Init();
    Stub::Init();
    StubLinkerCPU::Init();

  // weak_short, weak_long, strong; no pin
    Ref_Initialize();

    // Initialize remoting
    CRemotingServices::Initialize();

    // Initialize contexts
    Context::Initialize();

    InitThreadManager();
    STRESS_LOG0(LF_STARTUP, LL_ALWAYS, "Returned successfully from InitThreadManager");


    g_pEEShutDownEvent = new CLREvent();
    g_pEEShutDownEvent->CreateManualEvent(FALSE);

    // Initialize RWLocks
    CRWLock::ProcessInit();

    // Initialize debugger manager
    CCLRDebugManager::ProcessInit();


#ifdef DEBUGGING_SUPPORTED
    // Check the debugger/profiling control environment variable to
    // see if there's any work to be done.
    GetDbgProfControlFlag();
#endif // DEBUGGING_SUPPORTED

    if (!VirtualCallStubManager::InitStatic())
        IfFailGoLog(E_FAIL);

    GCInterface::m_MemoryPressureLock.Init("MemoryPressure", CrstGCMemoryPressure);

    // Setup the domains. Threads are started in a default domain.
    SystemDomain::Attach();

#ifdef DEBUGGING_SUPPORTED
    // Initialize the debugging services. This must be done before any
    // EE thread objects are created, and before any classes or
    // modules are loaded.
    InitializeDebugger(); // throws on error
#endif // DEBUGGING_SUPPORTED


#ifdef PROFILING_SUPPORTED
    // Initialize the profiling services.
    hr = InitializeProfiling();

    _ASSERTE(SUCCEEDED(hr));
    IfFailGo(hr);
#endif // PROFILING_SUPPORTED

    InitializeExceptionHandling();
    //
    // Install our global exception filter
    //
    InstallUnhandledExceptionFilter();

    // throws on error
    SetupThread();

    // Give PerfMon a chance to hook up to us
    // Do this both *before* and *after* ipcman init so corperfmonext.dll
    // has a chance to release stale private blocks that IPCMan could collide with.
    IPCFuncCallSource::DoThreadSafeCall();
    STRESS_LOG0(LF_STARTUP, LL_ALWAYS, "Returned successfully from second call to  IPCFuncCallSource::DoThreadSafeCall");

    InitPreStubManager();


    NDirect::Init();

    // Before setting up the execution manager initialize the first part
    // of the JIT helpers.
    InitJITHelpers1();
    InitJITHelpers2();

    SyncBlockCache::Attach();

    // Start up the EE intializing all the global variables
    ECall::Init();

    UMThunkInit();

    COMDelegate::Init();

    // Set up the sync block
    SyncBlockCache::Start();

    StackwalkCache::Init();

    ExecutionManager::Init();
    HostCodeHeap::Init();

    if (!COMNlsInfo::InitializeNLS())
        IfFailGo(E_FAIL);

    // Start up security
    Security::Start();

    AppDomain::CreateADUnloadStartEvent();

    CpuStoreBufferControl::Init();
    
    IfFailGoLog(InitializeGarbageCollector());

    InitializePinHandleTable();


    SystemDomain::System()->Init();

#ifdef PROFILING_SUPPORTED

    SystemDomain::NotifyProfilerStartup();
#endif // PROFILING_SUPPORTED

    if (CLRHosted()
#ifdef _DEBUG
        || ((fFlags & COINITEE_DLL) == 0 &&
            g_pConfig->GetHostTestADUnload())
#endif
        ) {
        // If we are hosted, a host may specify unloading AD when a managed allocation in
        // critical region fails.  We need to precreate a thread to unload AD.
        AppDomain::CreateADUnloadWorker();
    }

    g_fEEInit = false;

    SystemDomain::System()->DefaultDomain()->LoadSystemAssemblies();

    SystemDomain::System()->DefaultDomain()->SetupSharedStatics();

#ifdef DEBUGGING_SUPPORTED

    LOG((LF_CORDB | LF_SYNC | LF_STARTUP, LL_INFO1000, "EEStartup: adding default domain 0x%x\n",
        SystemDomain::System()->DefaultDomain()));

    // Make a call to publish the DefaultDomain for the debugger, etc
        SystemDomain::System()->PublishAppDomainAndInformDebugger(SystemDomain::System()->DefaultDomain());
#endif // DEBUGGING_SUPPORTED

#ifdef _DEBUG
    APIThreadStress::SetThreadStressCount(g_pConfig->GetAPIThreadStressCount());
#endif

    // Perform any once-only SafeHandle initialization.
    SafeHandle::Init();

    // Load mscorsn.dll if the app requested the legacy mode in its configuration file.
    if (g_pConfig->LegacyLoadMscorsnOnStartup())
        IfFailGo(LoadMscorsn());

    g_fEEStarted = TRUE;
    g_EEStartupStatus = S_OK;
    hr = S_OK;
    STRESS_LOG0(LF_STARTUP, LL_ALWAYS, "===================EEStartup Completed===================");

#ifdef _DEBUG
    // Perform mscorlib consistency check if requested
    Binder::CheckMscorlibExtended();
#endif

#if defined(STACK_GUARDS_DEBUG)
    // now we can start stack probing.
    InitStackProbes();
#elif defined(STACK_GUARDS_RELEASE)
    InitStackProbesRetail();
#endif

ErrExit: ;
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions)

    if (!g_fEEStarted) {
        if (g_fEEInit)
            g_fEEInit = false;

        if (!FAILED(hr))
            hr = E_FAIL;

        g_EEStartupStatus = hr;
    }

    if (breakOnEELoad.val(L"BreakOnEELoad", 0) == 2)
    {
#ifdef _DEBUG
        _ASSERTE(!"Done loading EE!");
#else
        DebugBreak();
#endif
    }

}

LONG FilterStartupException(PEXCEPTION_POINTERS p, PVOID pv)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(p));
        PRECONDITION(CheckPointer(pv));
    } CONTRACTL_END;

    g_EEStartupStatus = (HRESULT)p->ExceptionRecord->ExceptionInformation[0];

    // Make sure we got a failure code in this case
    if (!FAILED(g_EEStartupStatus))
        g_EEStartupStatus = E_FAIL;

    // Initializations has failed so reset the g_fEEInit flag.
    g_fEEInit = false;

    if (p->ExceptionRecord->ExceptionCode == BOOTUP_EXCEPTION_COMPLUS)
    {
        // Don't ever handle the exception in a checked build
#ifndef _DEBUG
        return EXCEPTION_EXECUTE_HANDLER;
#endif
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

HRESULT EEStartup(COINITIEE fFlags)
{
    // Cannot use normal contracts here because of the PAL_TRY.
    STATIC_CONTRACT_NOTHROW;

    _ASSERTE(!g_fEEStarted && !g_fEEInit && SUCCEEDED (g_EEStartupStatus));

    PAL_TRY
    {
        EEStartupHelper(fFlags);
    }
    PAL_EXCEPT_FILTER (FilterStartupException, NULL)
    {
        // The filter should have set g_EEStartupStatus to a failure HRESULT.
        _ASSERTE(FAILED(g_EEStartupStatus));
    }
    PAL_ENDTRY

    if(SUCCEEDED(g_EEStartupStatus) && (fFlags & COINITEE_MAIN) == 0)
        g_EEStartupStatus = SystemDomain::SetupDefaultDomainNoThrow();

    return g_EEStartupStatus;
}





STDAPI ClrCreateManagedInstance(LPCWSTR typeName,
                                REFIID riid,
                                LPVOID FAR *ppv)
{

    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(ppv, NULL_OK));
        PRECONDITION(CheckPointer(typeName, NULL_OK));
    } CONTRACTL_END;

    HRESULT hr = S_OK;
   
    if (ppv == NULL)
        return E_POINTER;

    if (typeName == NULL)
        return E_INVALIDARG;

    MAKE_UTF8PTR_FROMWIDE(pName, typeName);

    IfFailRet(CoInitializeEE(0));


    BOOL fCtorAlreadyCalled = FALSE;
    OBJECTREF       newobj = NULL;
    MethodTable *pMT = NULL;


    // Retrieve the current thread.
    BEGIN_EXTERNAL_ENTRYPOINT(&hr)

    pMT = TypeName::GetTypeUsingCASearchRules(pName, FALSE).GetMethodTable();

    if (!pMT)
        hr = CLASS_E_CLASSNOTAVAILABLE;
    else
    {
        pMT->EnsureInstanceActive();
        if (!pMT->HasPublicDefaultConstructor())
        {
            hr = COR_E_MEMBERACCESS;
        }
        else
        {
            // Call class init if necessary
            pMT->CheckRunClassInitThrowing();
        }
    }

    // If we failed return
    IfFailGo(hr);

    if (CRemotingServices::RequiresManagedActivation(TypeHandle(pMT)) != NoManagedActivation)
    {
        fCtorAlreadyCalled = TRUE;
        newobj = CRemotingServices::CreateProxyOrObject(pMT, TRUE);
    }
    else
    {
        newobj = pMT->Allocate();
    }

    GCPROTECT_BEGIN(newobj);

    // don't call any constructors if we already have called them
    if (!fCtorAlreadyCalled)
        CallDefaultConstructor(newobj);

    // return the tear-off
    SafeComHolder<IUnknown> punk(GetComIPFromObjectRef(&newobj));
    hr = punk->QueryInterface(riid, ppv);

    GCPROTECT_END();

    // If we failed return
    IfFailGo(hr);


ErrExit: ;
    END_EXTERNAL_ENTRYPOINT;

    return hr;
}



//---------------------------------------------------------------------------
// %%Function: void STDMETHODCALLTYPE CorExitProcess(int exitCode)
//
// Parameters:
//  int exitCode :: process exit code
//
// Returns:
//  Nothing
//
// Description:
//  COM Objects shutdown stuff should be done here
// ---------------------------------------------------------------------------
extern "C" void STDMETHODCALLTYPE CorExitProcess(int exitCode)
{

    CONTRACT_VIOLATION(GCViolation | ModeViolation);
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        ENTRY_POINT;
    } CONTRACTL_END;

    if (g_fEEShutDown || !g_fEEStarted)
        return;

    HRESULT hr;
    BEGIN_EXTERNAL_ENTRYPOINT(&hr)

    // The exit code for the process is communicated in one of two ways.  If the
    // entrypoint returns an 'int' we take that.  Otherwise we take a latched
    // process exit code.  This can be modified by the app via System.SetExitCode().
    SetLatchedExitCode(exitCode);

    // Bump up the ref-count on the module
    for (int i =0; i<6; i++)
        CLRLoadLibrary(MSCOREE_SHIM_W);

    END_EXTERNAL_ENTRYPOINT;

    ForceEEShutdown();


}

#include "../ildasm/dynamicarray.h"
struct RVAFSE // RVA Field Start & End
{
    BYTE* pbStart;
    BYTE* pbEnd;
};
extern DynamicArray<RVAFSE> *g_drRVAField;

static bool WaitForEndOfShutdown_OneIteration()
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    } CONTRACTL_END;

    // We are shutting down.  GC triggers does not have any effect now.
    CONTRACT_VIOLATION(GCViolation);

    // If someone calls EEShutDown while holding OS loader lock, the thread we created for shutdown
    // won't start running.  This is a deadlock we can not fix.  Instead, we timeout and continue the 
    // current thread.
    DWORD timeout = GetEEPolicy()->GetTimeout(OPR_ProcessExit);
    timeout *= 2;
    ULONGLONG endTime = CLRGetTickCount64() + timeout;
    bool done = false;
    EX_TRY
    {
        ULONGLONG curTime = CLRGetTickCount64();
        if (curTime > endTime)
        {
            done = true;
        }
        else
        {
            timeout = endTime - curTime;
            DWORD status = g_pEEShutDownEvent->Wait(timeout,TRUE);
            if (status == WAIT_OBJECT_0 || status == WAIT_TIMEOUT)
            {
                done = true;
            }
            else
            {
                done = false;
            }
        }
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);
    return done;
}

void WaitForEndOfShutdown()
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    } CONTRACTL_END;

    // We are shutting down.  GC triggers does not have any effect now.
    CONTRACT_VIOLATION(GCViolation);

    while (!WaitForEndOfShutdown_OneIteration());
}

void STDMETHODCALLTYPE EEShutDownHelper(BOOL fIsDllUnloading)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    // Get the current thread.
    Thread * pThisThread = GetThread();

    // If the process is detaching then set the global state.
    // This is used to get around FreeLibrary problems.
    if(fIsDllUnloading)
        g_fProcessDetach = true;

    if (IsDbgHelperSpecialThread())
    {
        // Our debugger helper thread does not allow Thread object to be set up.
        // We should not run shutdown code on debugger helper thread.
        _ASSERTE(fIsDllUnloading);
        return;
    }

#ifdef _DEBUG
    // stop API thread stress
    APIThreadStress::SetThreadStressCount(0);
#endif

    STRESS_LOG1(LF_STARTUP, LL_INFO10, "EEShutDown entered unloading = %d", fIsDllUnloading);

#ifdef _DEBUG
    if (_DbgBreakCount)
        _ASSERTE(!"An assert was hit before EE Shutting down");

    if (g_pConfig->GetConfigDWORD(L"BreakOnEEShutdown", 0))
        _ASSERTE(!"Shutting down EE!");
#endif

#ifdef DEBUGGING_SUPPORTED
    // This is a nasty, terrible, horrible thing. If we're being
    // called from our DLL main, then the odds are good that our DLL
    // main has been called as the result of some person calling
    // ExitProcess. That rips the debugger helper thread away very
    // ungracefully. This check is an attempt to recognize that case
    // and avoid the impending hang when attempting to get the helper
    // thread to do things for us.
    if ((g_pDebugInterface != NULL) && g_fProcessDetach)
        g_pDebugInterface->EarlyHelperThreadDeath();
#endif // DEBUGGING_SUPPORTED

    BOOL fFinalizeOK = FALSE;

    EX_TRY
    {
        ClrFlsSetThreadType(ThreadType_Shutdown);

        if (!fIsDllUnloading)
        {
            ProcessEventForHost(Event_ClrDisabled, NULL);
        }
        else if (g_fEEShutDown)
        {
            // I'm in the final shutdown and the first part has already been run.
            goto part2;
        }
        // Indicate the EE is the shut down phase.
        g_fEEShutDown |= ShutDown_Start;

        fFinalizeOK = TRUE;




        // We perform the final GC only if the user has requested it through the GC class.
        // We should never do the final GC for a process detach
        if (!g_fProcessDetach && !g_fFastExitProcess)
        {
            g_fEEShutDown |= ShutDown_Finalize1;
            GCHeap::GetGCHeap()->EnableFinalization();
            fFinalizeOK = GCHeap::GetGCHeap()->FinalizerThreadWatchDog();
        }


        // Ok.  Let's stop the EE.
        if (!g_fProcessDetach)
        {
            if (g_IBCLogger.InstrEnabled())
            {
                // acquire the SystemDomain lock on the second call to shutdown,
                // because after this call to FinalizerThreadWatchDog
                // every thread can be blocked inside Thread::RareDisablePreemptiveGC with this lock acquired
                // making the main shutdown thread to deadlock at the call to WriteAllModuleProfileData below
                SystemDomain::LockHolder systemDomainLock;
                systemDomainLock.SuppressRelease();
            }

            // Convert key locks into "shutdown" mode. A lock in shutdown mode means:
            // - Only the finalizer/helper/shutdown threads will be able to take the the lock.
            // - Any other thread that tries takes it will just get redirected to an endless WaitForEndOfShutdown().
            //
            // The only managed code that should run after this point is the finalizers for shutdown.
            // We convert locks needed for running + debugging such finalizers. Since such locks may need to be
            // juggled between multiple threads (finalizer/helper/shutdown), no single thread can take the
            // lock and not give it up.
            //
            // Each lock needs its own shutdown flag (they can't all be converted at once).
            // To avoid deadlocks, we need to convert locks in order of crst level (biggest first).

            // Notify the debugger that we're going into shutdown to convert debugger-lock to shutdown.
            if (g_pDebugInterface != NULL)
            {
                g_pDebugInterface->LockDebuggerForShutdown();
            }

            // This call will convert the ThreadStoreLock into "shutdown" mode, just like the debugger lock above.
            g_fEEShutDown |= ShutDown_Finalize2;
            if (fFinalizeOK)
            {
                fFinalizeOK = GCHeap::GetGCHeap()->FinalizerThreadWatchDog();
            }
        }

        FastInterlockOr ((DWORD*)&g_fForbidEnterEE, 1);

        if (g_fProcessDetach || !fFinalizeOK)
        {
            ThreadStore::TrapReturningThreads(TRUE);
        }

        if (!g_fProcessDetach && !fFinalizeOK)
        {
            goto lDone;
        }

#ifdef PROFILING_SUPPORTED
        // If profiling is enabled, then notify of shutdown first so that the
        // profiler can make any last calls it needs to.  Do this only if we
        // are not detaching

        if (IsProfilerPresent())
        {
            // If EEShutdown is not being called due to a ProcessDetach event, so
            // the profiler should still be present
            if (!g_fProcessDetach)
            {
                LOG((LF_CORPROF, LL_INFO10, "**PROF: EEShutDown entered.\n"));
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->Shutdown((ThreadID) pThisThread);
            }

            g_fEEShutDown |= ShutDown_Profiler;

            // Free the interface objects.
            TerminateProfiling(g_fProcessDetach);

            // EEShutdown is being called due to a ProcessDetach event, so the
            // profiler has already been unloaded and we must set the profiler
            // status to profNone so we don't attempt to send any more events to
            // the profiler
            if (g_fProcessDetach)
                g_profStatus = profNone;
        }
#endif // PROFILING_SUPPORTED

        // CoEEShutDownCOM moved to
        // the Finalizer thread. See bug 87809
        if (!g_fProcessDetach && !g_fFastExitProcess)
        {
            g_fEEShutDown |= ShutDown_COM;
            if (fFinalizeOK)
                GCHeap::GetGCHeap()->FinalizerThreadWatchDog();
        }

#ifdef _DEBUG
        else
            g_fEEShutDown |= ShutDown_COM;
#endif

#ifdef _DEBUG
        g_fEEShutDown |= ShutDown_SyncBlock;

        // This releases any metadata interfaces that may be held by leaked
        // ISymUnmanagedReaders or Writers. We do this in phase two now that
        // we know any such readers or writers can no longer be in use.
        //
        // Note: we only do this in a debug build to support our wacky leak
        // detection.
        if (g_fProcessDetach)
            Module::ReleaseMemoryForTracking();
#endif

        // Save the security policy cache as necessary.
        Security::SaveCache();



        // This is the end of Part 1.
part2:


        // On the new plan, we only do the tear-down under the protection of the loader
        // lock -- after the OS has stopped all other threads.
        if (fIsDllUnloading && (g_fEEShutDown & ShutDown_Phase2) == 0)
        {
            g_fEEShutDown |= ShutDown_Phase2;


            // Shutdown finalizer before we suspend all background threads. Otherwise we
            // never get to finalize anything. Obviously.

#ifdef _DEBUG
            if (_DbgBreakCount)
                _ASSERTE(!"An assert was hit After Finalizer run");
#endif

            // No longer process exceptions
            g_fNoExceptions = true;

            //
            // Remove our global exception filter. If it was NULL before, we want it to be null now.
            //
            UninstallUnhandledExceptionFilter();

            //
            if (!g_fFastExitProcess)
            {
                SystemDomain::DetachBegin();
            }


#ifdef DEBUGGING_SUPPORTED
            // Terminate the debugging services.
            TerminateDebugger();
#endif // DEBUGGING_SUPPORTED

            TerminateDebugStore();

            StubManager::TerminateStubManagers();


#ifdef ENABLE_PERF_COUNTERS
            // Terminate Perf Counters as late as we can (to get the most data)
            PerfCounters::Terminate();
#endif // ENABLE_PERF_COUNTERS

            VirtualCallStubManager::UninitStatic();

            // Terminate the InterProcess Communications with COM+
            TerminateIPCManager();

#ifdef ENABLE_PERF_LOG
            PerfLog::PerfLogDone();
#endif //ENABLE_PERF_LOG

            // Give PerfMon a chance to hook up to us
            // Have perfmon resync list *after* we close IPC so that it will remove
            // this process
            IPCFuncCallSource::DoThreadSafeCall();

            if (!g_fFastExitProcess)
            {
                SystemDomain::DetachEnd();
            }

            TerminateStackProbes();

#ifdef ENABLE_TIMELINE
            Timeline::Shutdown();
#endif
#ifdef _DEBUG
            if (_DbgBreakCount)
                _ASSERTE(!"EE Shutting down after an assert");
#endif


#ifdef LOGGING
            extern unsigned FcallTimeHist[];
#endif
            LOG((LF_STUBS, LL_INFO10, "FcallHist %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
                FcallTimeHist[0], FcallTimeHist[1], FcallTimeHist[2], FcallTimeHist[3],
                FcallTimeHist[4], FcallTimeHist[5], FcallTimeHist[6], FcallTimeHist[7],
                FcallTimeHist[8], FcallTimeHist[9], FcallTimeHist[10]));

            WriteJitHelperCountToSTRESSLOG();

            STRESS_LOG0(LF_STARTUP, LL_INFO10, "EEShutdown shutting down logging");


        if (g_pConfig != NULL)
            g_pConfig->Cleanup();

#ifdef LOGGING
            ShutdownLogging();
#endif
        }

    lDone: ;
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

    ClrFlsClearThreadType(ThreadType_Shutdown);
    if (!g_fProcessDetach)
    {
        g_pEEShutDownEvent->Set();
    }
}


BOOL g_fWeOwnProcess = FALSE;

static LONG s_ActiveShutdownThreadCount = 0;

DWORD WINAPI EEShutDownProcForSTAThread(LPVOID lpParameter)
{
    STATIC_CONTRACT_SO_INTOLERANT;;


    ClrFlsSetThreadType(ThreadType_ShutdownHelper);

    EEShutDownHelper(FALSE);
    for (int i = 0; i < 10; i ++)
    {
        if (s_ActiveShutdownThreadCount)
        {
            return 0;
        }
        __SwitchToThread(20);
    }

    EPolicyAction action = GetEEPolicy()->GetDefaultAction(OPR_ProcessExit, NULL);
    if (action < eRudeExitProcess)
    {
        action = eRudeExitProcess;
    }
    UINT exitCode;
    if (g_fWeOwnProcess)
    {
        exitCode = GetLatchedExitCode();
    }
    else
    {
        exitCode = HOST_E_EXITPROCESS_TIMEOUT;
    }
    EEPolicy::HandleExitProcessFromEscalation(action, exitCode);

    return 0;
}

// ---------------------------------------------------------------------------
// %%Function: EEShutDown(BOOL fIsDllUnloading)
//
// Parameters:
//  BOOL fIsDllUnloading :: is it safe point for full cleanup
//
// Returns:
//  Nothing
//
// Description:
//  All ee shutdown stuff should be done here
// ---------------------------------------------------------------------------
void STDMETHODCALLTYPE EEShutDown(BOOL fIsDllUnloading)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT; // we don't need to cleanup 'cus we're shutting down
        PRECONDITION(g_fEEStarted);
    } CONTRACTL_END;

    // If we have not started runtime successfully, it is not safe to call EEShutDown.
    if (!g_fEEStarted || g_fFastExitProcess == 2)
    {
        return;
    }

    // Stop stack probing and asserts right away.  Once we're shutting down, we can do no more.
    // And we don't want to SO-protect anything at this point anyway. This really only has impact
    // on a debug build.
    TerminateStackProbes();

    // We only do the first part of the shutdown once.
    static LONG OnlyOne = -1;

    if (!fIsDllUnloading)
    {
        if (FastInterlockIncrement(&OnlyOne) != 0)
        {
            // I'm in a regular shutdown -- but another thread got here first.
            // It's a race if I return from here -- I'll call ExitProcess next, and
            // rip things down while the first thread is half-way through a
            // nice cleanup.  Rather than do that, I should just wait until the
            // first thread calls ExitProcess().  I'll die a nice death when that
            // happens.
            Thread *pThisThread = GetThread();
            if (pThisThread && pThisThread->PreemptiveGCDisabled())
            {
                pThisThread->EnablePreemptiveGC();
            }
            WaitForEndOfShutdown();
            return;
        }
    }

    if (GetThread())
    {
        GCX_COOP();
        EEShutDownHelper(fIsDllUnloading);
        if (!fIsDllUnloading)
        {
            FastInterlockIncrement(&s_ActiveShutdownThreadCount);
        }
    }
    else
    {
        EEShutDownHelper(fIsDllUnloading);
        if (!fIsDllUnloading)
        {
            FastInterlockIncrement(&s_ActiveShutdownThreadCount);
        }
    }
}

// ---------------------------------------------------------------------------
// %%Function: CanRunManagedCode()
//
// Parameters:
//  none
//
// Returns:
//  true or false
//
// Description: Indicates if one is currently allowed to run managed code.
// ---------------------------------------------------------------------------

BOOL CanRunManagedCode(HINSTANCE hInst)
{
    return CanRunManagedCode(TRUE, hInst);
}

BOOL CanRunManagedCode(BOOL fCannotRunIsUserError, HINSTANCE hInst)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;        // because of the CustomerDebugProbe
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    // If we are shutting down the runtime, then we cannot run code.
    if (g_fForbidEnterEE == TRUE)
        return FALSE;

    // If we are finaling live objects or processing ExitProcess event,
    // we can not allow managed method to run unless the current thread
    // is the finalizer thread
    if ((g_fEEShutDown & ShutDown_Finalize2) && !GCHeap::GetGCHeap()->IsCurrentThreadFinalizer())
        return FALSE;

    // If pre-loaded objects are not present, then no way.
    if (g_pPreallocatedOutOfMemoryException == NULL)
        return FALSE;


    return TRUE;
}

// ---------------------------------------------------------------------------
// %%Function: CoInitializeEE(DWORD fFlags)
//
// Parameters:
//  fFlags                  - Initialization flags for the engine.  See the
//                              COINITIEE enumerator for valid values.
//
// Returns:
//  Nothing
//
// Description:
//  Initializes the EE if it hasn't already been initialized. This function
//  no longer maintains a ref count since the EE doesn't support being
//  unloaded and re-loaded. It simply ensures the EE has been started.
// ---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CoInitializeEE(DWORD fFlags)
{
    WRAPPER_CONTRACT;
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = EnsureEEStarted((COINITIEE)fFlags);
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

// ---------------------------------------------------------------------------
// %%Function: CoUninitializeEE
//
// Parameters:
//  BOOL fIsDllUnloading :: is it safe point for full cleanup
//
// Returns:
//  Nothing
//
// Description:
//  Must be called by client on shut down in order to free up the system.
// ---------------------------------------------------------------------------
void STDMETHODCALLTYPE CoUninitializeEE(BOOL fIsDllUnloading)
{
    LEAF_CONTRACT;


}

// ---------------------------------------------------------------------------
//// Win9x: During PROCESS_DETACH as a result of process termination,
//// anything on the stack allocated before PROCESS_DETACH notification
//// is not reliable.
//// Any global and thread variables that track information
//// on the stack should be reset here.


//*****************************************************************************
// This entry point is called from the native DllMain of the loaded image.
// This gives the COM+ loader the chance to dispatch the loader event.  The
// first call will cause the loader to look for the entry point in the user
// image.  Subsequent calls will dispatch to either the user's DllMain or
// their Module derived class.
//*****************************************************************************
BOOL STDMETHODCALLTYPE _CorDllMain(     // TRUE on success, FALSE on error.
    HINSTANCE   hInst,                  // Instance handle of the loaded module.
    DWORD       dwReason,               // Reason for loading.
    LPVOID      lpReserved              // Unused.
    )
{
    STATIC_CONTRACT_NOTHROW;
    //STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_ENTRY_POINT;
    BOOL retval = FALSE;

    //BEGIN_ENTRYPOINT_NOTHROW;

    // Can't use PAL_TRY/EX_TRY here as they access the ClrDebugState which gets blown away as part of the
    // PROCESS_DETACH path. Must use special PAL_TRY_FOR_DLLMAIN, passing the reason were in the DllMain.
    PAL_TRY_FOR_DLLMAIN(dwReason)
    {
#ifdef _DEBUG
    if (CLRTaskHosted() &&
        ((dwReason == DLL_PROCESS_ATTACH && lpReserved == NULL) ||  // LoadLibrary of a managed dll
         (dwReason == DLL_PROCESS_DETACH && lpReserved == NULL)     // FreeLibrary of a managed dll
         )) {
        // OS loader lock is being held by the current thread.  We can not allow the fiber
        // to be rescheduled here while processing DllMain for managed dll.
        IHostTask *pTask = GetCurrentHostTask();
        if (pTask) {
            Thread *pThread = GetThread();
            _ASSERTE (pThread);
            _ASSERTE (pThread->HasThreadAffinity());
        }
    }
#endif

        // Since we're in _CorDllMain, we know that we were not called because of a
        // bootstrap thunk, since they will call CorDllMainForThunk. Because of this,
        // we can pass FALSE for the fFromThunk parameter.
        retval = ExecuteDLL(hInst,dwReason,lpReserved, FALSE);
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
    }
    PAL_ENDTRY;

    //END_ENTRYPOINT_NOTHROW;

    return retval;
}

//*****************************************************************************
void STDMETHODCALLTYPE CorDllMainForThunk(HINSTANCE hInst, HINSTANCE hInstShim)
{

    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;


    CONSISTENCY_CHECK(g_hInstShim == NULL || hInstShim == g_hInstShim);
    g_hInstShim = hInstShim;

    {
        // If no managed thread exists, then we need to call the prepare method
        // to try and startup the runtime and/or create a managed thread object
        // so that installing an unwind and continue handler below is possible.
        // If we fail to startup or create a thread, we'll raise the basic
        // EXCEPTION_COMPLUS exception.
        if (GetThread() == NULL)
        {
            HRESULT hr;
            // Since this method is only called if a bootstrap thunk is invoked, we
            // know that passing TRUE for fFromThunk is the correct value.
            if (FAILED(hr = PrepareExecuteDLLForThunk(hInst, 0, NULL)))
            {
                // We failed to start the runtime or to create a runtime thread.
                RaiseException(EXCEPTION_COMPLUS, 0, 0, NULL);
            }
        }

    }

    INSTALL_UNWIND_AND_CONTINUE_HANDLER(GetThread());

    // We're actually going to run some managed code and we're inside the loader lock.
    // There may be a customer debug probe enabled that prevents this.
    CanRunManagedCode(hInst);

    // Since this method is only called if a bootstrap thunk is invoked, we
    // know that passing TRUE for fFromThunk is the correct value.
    ExecuteDLL(hInst, 0, NULL, TRUE);

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

}


static BOOL CacheCommandLine(__in LPWSTR pCmdLine, __in_opt LPWSTR* ArgvW)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pCmdLine));
        PRECONDITION(CheckPointer(ArgvW));
    } CONTRACTL_END;

    if (pCmdLine) {
        size_t len = wcslen(pCmdLine);

        _ASSERT(g_pCachedCommandLine== NULL);
        g_pCachedCommandLine = new WCHAR[len+1];
        wcscpy_s(g_pCachedCommandLine, len+1, pCmdLine);
    }

    if (ArgvW != NULL && ArgvW[0] != NULL) {
        WCHAR wszModuleName[MAX_PATH];
        WCHAR wszCurDir[MAX_PATH];
        if (!WszGetCurrentDirectory(MAX_PATH, wszCurDir))
            return FALSE;
        if (PathCombine(wszModuleName, wszCurDir, ArgvW[0]) == NULL)
            return FALSE;

        size_t len = wcslen(wszModuleName);
        _ASSERT(g_pCachedModuleFileName== NULL);
        g_pCachedModuleFileName = new WCHAR[len+1];
        wcscpy_s(g_pCachedModuleFileName, len+1, wszModuleName);
    }

    return TRUE;
}

//*****************************************************************************
// This entry point is called from the native entry piont of the loaded
// executable image.  The command line arguments and other entry point data
// will be gathered here.  The entry point for the user image will be found
// and handled accordingly.
//*****************************************************************************
__int32 STDMETHODCALLTYPE _CorExeMain2( // Executable exit code.
    PBYTE   pUnmappedPE,                // -> memory mapped code
    DWORD   cUnmappedPE,                // Size of memory mapped code
    __in LPWSTR  pImageNameIn,          // -> Executable Name
    __in LPWSTR  pLoadersFileName,      // -> Loaders Name
    __in LPWSTR  pCmdLine)              // -> Command Line
{

    // This entry point is used by clix
    BOOL bRetVal = 0;

    //BEGIN_ENTRYPOINT_VOIDRET;

    // Before we initialize the EE, make sure we've snooped for all EE-specific
    // command line arguments that might guide our startup.
    HRESULT result = CorCommandLine::SetArgvW(pCmdLine);

    if (!CacheCommandLine(pCmdLine, CorCommandLine::GetArgvW(NULL))) {
        LOG((LF_STARTUP, LL_INFO10, "Program exiting - CacheCommandLine failed\n"));
        bRetVal = -1;
        goto exit;
    }

    if (SUCCEEDED(result))
        result = CoInitializeEE(COINITEE_DEFAULT | COINITEE_MAIN);

    if (FAILED(result)) {
        VMDumpCOMErrors(result);
        SetLatchedExitCode (-1);
        goto exit;
    }

    // This is here to get the ZAPMONITOR working correctly
    INSTALL_UNWIND_AND_CONTINUE_HANDLER;


    // Load the executable
    bRetVal = ExecuteEXE(pImageNameIn);

    if (!bRetVal) {
        // The only reason I've seen this type of error in the wild is bad
        // metadata file format versions and inadequate error handling for
        // partially signed assemblies.  While this may happen during
        // development, our customers should not get here.  This is a back-stop
        // to catch CLR bugs. If you see this, please try to find a better way
        // to handle your error, like throwing an unhandled exception.
        EEMessageBoxCatastrophic(IDS_EE_COREXEMAIN2_FAILED_TEXT, IDS_EE_COREXEMAIN2_FAILED_TITLE);
        SetLatchedExitCode (-1);
    }

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

exit:
    STRESS_LOG1(LF_STARTUP, LL_ALWAYS, "Program exiting: return code = %d", GetLatchedExitCode());

    STRESS_LOG0(LF_STARTUP, LL_INFO10, "EEShutDown invoked from _CorExeMain2");

    EEPolicy::HandleExitProcess();
    
    //END_ENTRYPOINT_VOIDRET;

    return bRetVal;
}

//*****************************************************************************
// This is the call point to wire up an EXE.  In this case we have the HMODULE
// and just need to make sure we do to correct self referantial things.
//*****************************************************************************

BOOL STDMETHODCALLTYPE ExecuteEXE(HMODULE hMod)
{
    STATIC_CONTRACT_GC_TRIGGERS;

    _ASSERTE(hMod);
    if (!hMod)
        return FALSE;

    ETWTraceStartup::TraceEvent(ETW_TYPE_STARTUP_EXEC_EXE);
    TIMELINE_START(STARTUP, ("ExecuteExe"));

    EX_TRY_NOCATCH
    {
        // Executables are part of the system domain
        SystemDomain::ExecuteMainMethod(hMod);
    }
    EX_END_NOCATCH;

    ETWTraceStartup::TraceEvent(ETW_TYPE_STARTUP_EXEC_EXE+1);
    TIMELINE_END(STARTUP, ("ExecuteExe"));

    return TRUE;
}

BOOL STDMETHODCALLTYPE ExecuteEXE(__in LPWSTR pImageNameIn)
{
    STATIC_CONTRACT_GC_TRIGGERS;

    WCHAR               wzPath[MAX_PATH];
    DWORD               dwPathLength = 0;

    EX_TRY_NOCATCH
    {
        // get the path of executable
        dwPathLength = WszGetFullPathName(pImageNameIn, MAX_PATH, wzPath, NULL);

        if (!dwPathLength || dwPathLength > MAX_PATH)
        {
            ThrowWin32( !dwPathLength ? GetLastError() : ERROR_FILENAME_EXCED_RANGE);
        }

        SystemDomain::ExecuteMainMethod( NULL, (WCHAR *)wzPath );
    }
    EX_END_NOCATCH;

    return TRUE;
}

//*****************************************************************************
// fFromThunk indicates that a dependency is calling through the Import Export table,
// and calling indirect through the IJW vtfixup slot.
//
// fFromThunk=FALSE means that we are running DllMain during LoadLibrary while
// holding the loader lock.
//



//*****************************************************************************
BOOL ExecuteDLL_ReturnOrThrow(HRESULT hr, BOOL fFromThunk)
{
    CONTRACTL {
        if (fFromThunk) THROWS; else NOTHROW;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
    SO_TOLERANT;
    } CONTRACTL_END;

    // If we have a failure result, and we're called from a thunk,
    // then we need to throw an exception to communicate the error.
    if (FAILED(hr) && fFromThunk)
    {
        COMPlusThrowHR(hr);
    }
    return SUCCEEDED(hr);
}

#ifdef _DEBUG
//*****************************************************************************
// Factor some common debug code.
//*****************************************************************************
static void EnsureManagedThreadExistsForHostedThread()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    if (CLRTaskHosted()) {
        // If CLR is hosted, and this is on a thread that a host controls,
        // we must have created Thread object.
        IHostTask *pHostTask = GetCurrentHostTask();
        if (pHostTask)
        {
            CONSISTENCY_CHECK(CheckPointer(GetThread()));
        }
    }
}
#endif

//*****************************************************************************
// This ensure that the runtime is started and an EEThread object is created
// for the current thread. This functionality is duplicated in ExecuteDLL,
// except that this code will not throw.
//*****************************************************************************
HRESULT PrepareExecuteDLLForThunk(HINSTANCE hInst,
                                                 DWORD dwReason,
                                                 LPVOID lpReserved)
{
    CONTRACTL {
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        PRECONDITION(CheckPointer(lpReserved, NULL_OK));
        PRECONDITION(CheckPointer(hInst));
    } CONTRACTL_END;


    HRESULT hr = S_OK;
    Thread *pThread = GetThread();

    INDEBUG(EnsureManagedThreadExistsForHostedThread();)

    if (pThread == NULL)
    {
        // If necessary, start the runtime and create a managed thread object.
        hr = EnsureEEStarted(COINITEE_DLL);
        if (FAILED(hr))
        {
            return hr;
        }
        if ((pThread = SetupThreadNoThrow(&hr)) == NULL)
        {
            return hr;
        }
    }

    CONSISTENCY_CHECK(CheckPointer(pThread));

    return S_OK;
}

//*****************************************************************************
// This is the call point to make a DLL that is already loaded into our address
// space run. There will be other code to actually have us load a DLL due to a
// class referance.
//*****************************************************************************
BOOL STDMETHODCALLTYPE ExecuteDLL(HINSTANCE hInst,
                                  DWORD dwReason,
                                  LPVOID lpReserved,
                                  BOOL fFromThunk)
{

    CONTRACTL{
        THROWS;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(lpReserved, NULL_OK));
        PRECONDITION(CheckPointer(hInst));
        PRECONDITION(GetThread() != NULL || !fFromThunk);
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    BOOL fRetValue = FALSE;
    BEGIN_ENTRYPOINT_THROWS;

    Thread *pThread = GetThread();

    if (!hInst)
    {
        fRetValue = ExecuteDLL_ReturnOrThrow(E_FAIL, fFromThunk);
        goto Exit;
    }

    // Note that we always check fFromThunk before checking the dwReason value.
    // This is because the dwReason value is undefined in the case that we're
    // being invoked due to a bootstrap (because that is by definition outside
    // of the loader lock and there is no appropriate dwReason value).
    if (fFromThunk ||
        dwReason == DLL_PROCESS_ATTACH ||
        dwReason == DLL_THREAD_ATTACH)
    {
        INDEBUG(EnsureManagedThreadExistsForHostedThread();)


        // If necessary, start the runtime and create a managed thread object.
        if (fFromThunk || dwReason == DLL_PROCESS_ATTACH)
        {
            hr = EnsureEEStarted(COINITEE_DLL);

            if (SUCCEEDED(hr) && pThread == NULL)
            {
                pThread = SetupThreadNoThrow(&hr);
            }

            if(FAILED(hr))
            {
                fRetValue = ExecuteDLL_ReturnOrThrow(hr, fFromThunk);
                goto Exit;
            }
        }

        // IJW assemblies cause the thread doing the process attach to
        // re-enter ExecuteDLL and do a thread attach. This happens when
        // CoInitializeEE() above executed
        else if (!(pThread &&
                   pThread->GetDomain() &&
                   CanRunManagedCode(FALSE/*=fCannotRunIsUserError*/)))
        {
            fRetValue = ExecuteDLL_ReturnOrThrow(S_OK, fFromThunk);
            goto Exit;
        }

        // we now have a thread setup - either the 1st if set it up, or
        // the else if ran if we didn't have a thread setup.

    }
    else
    {
        PEDecoder pe(hInst);
        if (pe.HasManagedEntryPoint())
        {
            // If the EE is still intact, then run user entry points.  Otherwise
            // detach was handled when the app domain was stopped.
            //
            // Checks for the loader lock will occur within RunDllMain, if that's
            FAULT_NOT_FATAL();
            if (CanRunManagedCode(FALSE/*=fCannotRunIsUserError*/))
            {
                hr = SystemDomain::RunDllMain(hInst, dwReason, lpReserved);
            }
        }
        // This does need to match the attach. We will only unload dll's
        // at the end and CoUninitialize will just bounce at 0. WHEN and IF we
        // get around to unloading IL DLL's during execution prior to
        // shutdown we will need to bump the reference one to compensate
        // for this call.
        if (dwReason == DLL_PROCESS_DETACH && !g_fForbidEnterEE)
        {
        }
    }

    fRetValue = ExecuteDLL_ReturnOrThrow(hr, fFromThunk);

Exit:
    
    END_ENTRYPOINT_THROWS;
    return fRetValue;
}


//
// Initialize the Garbage Collector
//

HRESULT InitializeGarbageCollector()
{
    CONTRACTL{
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(return E_OUTOFMEMORY);
    } CONTRACTL_END;

    HRESULT hr;

    // Build the special Free Object used by the Generational GC
    _ASSERT(g_pFreeObjectMethodTable == NULL);
    g_pFreeObjectMethodTable = (MethodTable *) new (nothrow) BYTE[sizeof(MethodTable)];
    if (g_pFreeObjectMethodTable == NULL)
        return (E_OUTOFMEMORY);
    ZeroMemory(g_pFreeObjectMethodTable, sizeof(MethodTable));

    // As the flags in the method table indicate there are no pointers
    // in the object, there is no gc descriptor, and thus no need to adjust
    // the pointer to skip the gc descriptor.

    g_pFreeObjectMethodTable->SetBaseSize(ObjSizeOf (ArrayBase));
    g_pFreeObjectMethodTable->SetClass (NULL);
    g_pFreeObjectMethodTable->InitializeFlags(MethodTable::enum_flag_Array | 1 /* ComponentSize */);

   {
        GCHeap *pGCHeap = GCHeap::CreateGCHeap();
        GCHeap::SetGCHeap(pGCHeap);

        if (!pGCHeap)
            return (E_OUTOFMEMORY);

        {
            CONTRACT_VIOLATION(ThrowsViolation);
            hr = pGCHeap->Initialize();
        }
    }

    return(hr);
}

/*****************************************************************************/
/* This is here only so that if we get an exception we stop before we catch it */
LONG DllMainFilter(PEXCEPTION_POINTERS p, PVOID pv)
{
        _ASSERTE(!"Exception happened in mscorwks!DllMain!");
        return EXCEPTION_EXECUTE_HANDLER;
}

//*****************************************************************************
// This is the part of the old-style DllMain that initializes the
// stuff that the EE team works on. It's called from the real DllMain
// up in MSCOREE land. Separating the DllMain tasks is simply for
// convenience due to the dual build trees.
//*****************************************************************************
BOOL STDMETHODCALLTYPE EEDllMain( // TRUE on success, FALSE on error.
    HINSTANCE   hInst,             // Instance handle of the loaded module.
    DWORD       dwReason,          // Reason for loading.
    LPVOID      lpReserved)        // Unused.
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;

    // this runs at the top of a thread, SO is not a concern here...
    ENTER_SO_NOT_MAINLINE_CODE;

    // HRESULT hr;
    // BEGIN_EXTERNAL_ENTRYPOINT(&hr);
    // EE isn't spun up enough to use this macro

    // Can't use PAL_TRY/EX_TRY here as they access the ClrDebugState which gets blown away as part of the
    // PROCESS_DETACH path. Must use special PAL_TRY_FOR_DLLMAIN, passing the reason were in the DllMain.
    PAL_TRY_FOR_DLLMAIN(dwReason)
    {

    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            // We cache the SystemInfo for anyone to use throughout the
            // life of the DLL.
            GetSystemInfo(&g_SystemInfo);

            // init sync operations
            InitFastInterlockOps();
            // Remember module instance
            g_pMSCorEE = hInst;


            // Set callbacks so that LoadStringRC knows which language our
            // threads are in so that it can return the proper localized string.
            SetResourceCultureCallbacks(GetThreadUICultureNames,
                                        GetThreadUICultureId);

            InitEEPolicy();
            InitHostProtectionManager();

            break;
        }

        case DLL_PROCESS_DETACH:
        {
            // lpReserved is NULL if we're here because someone called FreeLibrary
            // and non-null if we're here because the process is exiting.
            // Since nobody should ever be calling FreeLibrary on mscorwks.dll, lpReserved
            // should always be non NULL.
            _ASSERTE(lpReserved || !g_fEEStarted);
                g_fProcessDetach = TRUE;

            if (g_fEEStarted)
            {
                // GetThread() may be set to NULL for Win9x during shutdown.
                Thread *pThread = GetThread();
                if (GCHeap::IsGCInProgress() &&
                    ( (pThread && (pThread != GCHeap::GetGCHeap()->GetGCThread() ))
                        || !g_fSuspendOnShutdown))
                {
                    g_fEEShutDown |= ShutDown_Phase2;
                    break;
                }

                LOG((LF_STARTUP, INFO3, "EEShutDown invoked from EEDllMain"));
                EEShutDown(TRUE); // shut down EE if it was started up
                }
            break;
        }

        case DLL_THREAD_DETACH:
        {
            // Don't destroy threads here if we're in shutdown (shutdown will
            // clean up for us instead).

            // Don't use GetThread because perhaps we didn't initialize yet, or we
            // have already shutdown the EE.  Note that there is a race here.  We
            // might ask for TLS from a slot we just released.  We are assuming that
            // nobody re-allocates that same slot while we are doing this.  It just
            // isn't worth locking for such an obscure case.
            DWORD   tlsVal = GetThreadTLSIndex();

            if (tlsVal != (DWORD)-1)
            {
                Thread  *thread = (Thread *) UnsafeTlsGetValue(tlsVal);

                if (thread)
                {
                    // For case where thread calls ExitThread directly, we need to reset the
                    // frame pointer. Otherwise stackwalk would AV. We need to do it in cooperative mode.
                    // We need to set m_GCOnTransitionsOK so this thread won't trigger GC when toggle GC mode
                    if (thread->m_pFrame != FRAME_TOP)
                    {
#ifdef _DEBUG
                        thread->m_GCOnTransitionsOK = FALSE;
#endif
                        thread->DisablePreemptiveGC ();
                        thread->m_pFrame = FRAME_TOP;
                        thread->EnablePreemptiveGC ();
                    }
                    thread->DetachThread(TRUE);
                }
            }
        }
    }

    }
    PAL_EXCEPT_FILTER(DllMainFilter, NULL)
    {
    }
    PAL_ENDTRY;

    LEAVE_SO_NOT_MAINLINE_CODE;

    if (dwReason == DLL_THREAD_DETACH)
    {
        if (CLRMemoryHosted())
        {
            // A host may not support memory operation inside OS loader lock.
            // We will free these memory on finalizer thread.
            CExecutionEngine::DetachTlsInfo(CExecutionEngine::CheckThreadStateNoCreate(0));
        }
        else
        {
            CExecutionEngine::ThreadDetaching(CExecutionEngine::CheckThreadStateNoCreate(0));
        }
    }
    return TRUE;
}


#ifdef DEBUGGING_SUPPORTED
//*****************************************************************************
// This gets the environment var control flag for Debugging and Profiling
//*****************************************************************************


static void GetDbgProfControlFlag()
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    // Check the debugger/profiling control environment variable to
    // see if there's any work to be done.
    g_CORDebuggerControlFlags = DBCF_NORMAL_OPERATION;

    char buf[32];
    DWORD len = GetEnvironmentVariableA(CorDB_CONTROL_ENV_VAR_NAME,
                                        buf, sizeof(buf));
    _ASSERTE(len < sizeof(buf));

    char *szBad;
    int  iBase;
    if (len > 0 && len < sizeof(buf))
    {
        iBase = (*buf == '0' && (*(buf + 1) == 'x' || *(buf + 1) == 'X')) ? 16 : 10;
        ULONG dbg = strtoul(buf, &szBad, iBase) & DBCF_USER_MASK;

        if (dbg == 1)
            g_CORDebuggerControlFlags |= DBCF_GENERATE_DEBUG_CODE;
    }
}
#endif // DEBUGGING_SUPPORTED

#ifdef PROFILING_SUPPORTED

//*****************************************************************************
// This is used to get the proc address by name of a proc in MSCORDBC.DLL
// It will perform a LoadLibrary if necessary.
//*****************************************************************************
static HRESULT GetDBCProc(__in const char *szProcName, FARPROC *pProcAddr)
{
    CONTRACTL{
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        PRECONDITION(CheckPointer(szProcName));
        PRECONDITION(CheckPointer(pProcAddr));
    } CONTRACTL_END;

    HRESULT  hr = S_OK;

    GCX_PREEMP();

    // If the library hasn't already been loaded, do so
    if (g_pDebuggerDll == NULL)
    {
        DWORD lgth = _MAX_PATH + 1;
        WCHAR wszFile[_MAX_PATH + 1];
        hr = GetInternalSystemDirectory(wszFile, &lgth);
        if(FAILED(hr)) goto leav;

        wcscat_s(wszFile, COUNTOF(wszFile), MAKEDLLNAME_W(L"mscordbc"));
        g_pDebuggerDll = CLRLoadLibrary(wszFile);

        if (g_pDebuggerDll == NULL)
        {
            LOG((LF_CORPROF | LF_CORDB, LL_INFO10,
                 "MSCORDBC.DLL not found.\n"));
            hr = HRESULT_FROM_GetLastError();
            goto leav;
        }
    }
    _ASSERTE(g_pDebuggerDll != NULL);

    // Get the pointer to the requested function
    *pProcAddr = GetProcAddress(g_pDebuggerDll, szProcName);

    // If the proc address was not found, return error
    if (*pProcAddr == NULL)
    {
        LOG((LF_CORPROF | LF_CORDB, LL_INFO10,
             "'%s' not found in MSCORDBC.DLL\n"));
        hr = HRESULT_FROM_GetLastError();
        goto leav;
    }

leav:

    return hr;
}

void LogProfFailure(PCSTR szMsg)
{
    CONTRACTL{
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(szMsg));
    } CONTRACTL_END;

    SString msg(SString::Ascii, szMsg);

    EEMessageBox(IDS_EE_ERRORMESSAGETEXTTEMPLATE, IDS_EE_PROFILING_FAILURE, MB_OK | MB_ICONEXCLAMATION, msg.GetUnicode());
}

#define LOGPROFFAILURE(msg) LogProfFailure(msg)

/*
 * This will initialize the profiling services, if profiling is enabled.
 */

#define ENV_PROFILER L"COR_PROFILER"
#define ENV_PROFILER_A "COR_PROFILER"
#define ENV_PROFILER_DLL L"COR_PROFILER_DLL"
#define ENV_PROFILER_DLL_A "COR_PROFILER_DLL"
static HRESULT InitializeProfiling()
{
    CONTRACTL{
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(return E_OUTOFMEMORY);
    } CONTRACTL_END;

    HRESULT hr;

    // This has to be called to initialize the WinWrap stuff so that WszXXX
    // may be called.
    OnUnicodeSystem();

    {
        PROFILER_CALL;
        g_profControlBlock.Init();
    }
    // Find out if profiling is enabled
    DWORD fProfEnabled = 0;
     
    //
    // Profiling is only enabled on non-Win9x platforms, so only check the registry on these OSes.
    //
    if (!RunningOnWin95()) 
    {
        fProfEnabled = g_pConfig->GetConfigDWORD(CorDB_CONTROL_ProfilingL, 0, REGUTIL::COR_CONFIG_ALL, FALSE);
    }

    // If profiling is not enabled, return.
    if (fProfEnabled == 0)
    {
        LOG((LF_CORPROF, LL_INFO10, "**PROF: Profiling not enabled.\n"));
        return (S_OK);
    }

    LOG((LF_CORPROF, LL_INFO10, "**PROF: Initializing Profiling Services.\n"));

    // Get the CLSID of the profiler to CoCreate
    NewArrayHolder<WCHAR> wszCLSID(NULL);
    NewArrayHolder<WCHAR> wszProfilerDLL(NULL);

    IfFailRet(g_pConfig->GetConfigString(ENV_PROFILER, &wszCLSID, FALSE));
    IfFailRet(g_pConfig->GetConfigString(ENV_PROFILER_DLL, &wszProfilerDLL, FALSE));

    // If the environment variable doesn't exist, profiling is not enabled.
    if (wszCLSID == NULL)
    {
        LOG((LF_CORPROF, LL_INFO10, "**PROF: Profiling flag set, but required "
             "environment variable does not exist.\n"));

        LOGPROFFAILURE("Profiling flag set, but required environment ("
                       ENV_PROFILER_A ") was not set.");

        return (S_FALSE);
    }
    // If the environment variable doesn't exist, profiling is not enabled.
    if (wszProfilerDLL == NULL)
    {
        LOG((LF_CORPROF, LL_INFO10, "**PROF: Profiling flag set, but required "
             "environment variable does not exist.\n"));

        LOGPROFFAILURE("Profiling flag set, but required environment ("
                       ENV_PROFILER_DLL_A ") was not set.");

        return (S_FALSE);
    }

    //*************************************************************************
    // Create the EE interface to provide to the profiling services
    NewHolder<ProfToEEInterface> pProfEE(
        (ProfToEEInterface *) new ProfToEEInterfaceImpl());

    // Initialize the interface
    hr = pProfEE->Init();

    if (FAILED(hr))
    {
        LOG((LF_CORPROF, LL_INFO10, "**PROF: ProfToEEInterface::Init failed.\n"));

        LOGPROFFAILURE("Internal profiling services initialization failure.");
        return (S_FALSE);
    }

    //*************************************************************************
    // Provide the EE interface to the Profiling services
    SETPROFTOEEINTERFACE *pSetProfToEEInterface = NULL;
    hr = GetDBCProc("SetProfToEEInterface", (FARPROC *)&pSetProfToEEInterface);

    if (FAILED(hr))
    {
        LOGPROFFAILURE("Internal profiling services initialization failure.");
        return (S_FALSE);
    }

    _ASSERTE(pSetProfToEEInterface != NULL);

    // Provide the newly created and inited interface
    pSetProfToEEInterface(pProfEE);

    //*************************************************************************
    // Get the Profiling services interface
    GETEETOPROFINTERFACE *pGetEEToProfInterface;
    hr = GetDBCProc("GetEEToProfInterface", (FARPROC *)&pGetEEToProfInterface);
    _ASSERTE(pGetEEToProfInterface != NULL);

    pGetEEToProfInterface(&g_profControlBlock.pProfInterface);
    _ASSERTE(g_profControlBlock.pProfInterface != NULL);

    // Check if we successfully got an interface to
    if (g_profControlBlock.pProfInterface == NULL)
    {
        LOG((LF_CORPROF, LL_INFO10, "**PROF: GetEEToProfInterface failed.\n"));

        LOGPROFFAILURE("Internal profiling services initialization failure.");

        pSetProfToEEInterface(NULL);
        return (S_FALSE);
    }

    pProfEE.SuppressRelease();


    //*************************************************************************
    // Now ask the profiling services to CoCreate the profiler

    // Indicate that the profiler is in initialization phase
    g_profStatus = profInInit;

    // This will CoCreate the profiler
    {
        PROFILER_CALL;
        hr = g_profControlBlock.pProfInterface->CreateProfiler(wszCLSID, wszProfilerDLL);
    }
    
    if (FAILED(hr))
    {
        LOG((LF_CORPROF, LL_INFO10, "**PROF: No profiler registered, or "
             "CoCreate failed.  Shutting down profiling.\n"));

        if (hr == E_NOINTERFACE)
        {
            LOGPROFFAILURE("CLR 2.0 does not support profilers written for CLR 1.x");
        }
        else
        {
            LOGPROFFAILURE("Failed to CoCreate profiler.");
        }

        // Notify the profiling services that the EE is shutting down
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->Terminate(FALSE);
        }
        g_profControlBlock.pProfInterface = NULL;
        g_profStatus = profNone;

        return (S_FALSE);
    }

    g_profControlBlock.pReservedMem = (BYTE*)ClrVirtualAlloc((BYTE *)TOP_MEMORY - 4*1024*1024, 4*1024*1024,
                                                        MEM_RESERVE, PAGE_READWRITE);

    LOG((LF_CORPROF, LL_INFO10, "**PROF: Profiler created and enabled.\n"));


    // If the profiler is interested in tracking GC events, then we must
    // disable concurrent GC since concurrent GC can allocate and kill
    // objects without relocating and thus not doing a heap walk.
    if (CORProfilerTrackGC())
        g_pConfig->SetGCconcurrent(0);

    // Indicate that profiling is properly initialized.
    g_profStatus = profInit;
    return (hr);
}

/*
 * This will terminate the profiling services, if profiling is enabled.
 */
static void TerminateProfiling(BOOL fProcessDetach)
{
    WRAPPER_CONTRACT;

    _ASSERTE(g_profStatus != profNone);

    // If we have a profiler interface active, then terminate it.
    if (g_profControlBlock.pProfInterface)
    {
        // Notify the profiling services that the EE is shutting down
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->Terminate(fProcessDetach);

        g_profControlBlock.pProfInterface = NULL;
    }

    g_profStatus = profNone;
}
#endif // PROFILING_SUPPORTED



HRESULT InitializeDebugStore()
{
    _ASSERTE(g_pDebugInfoStore == NULL);

    // This will throw on OOM.
    g_pDebugInfoStore = new DebugInfoManager();

    return S_OK;
}

void TerminateDebugStore()
{
    if (g_pDebugInfoStore != NULL)
    {
        delete g_pDebugInfoStore;
        g_pDebugInfoStore = NULL;
    }
}

#ifdef DEBUGGING_SUPPORTED
//
// InitializeDebugger initialized the Runtime-side COM+ Debugging Services
//
static void InitializeDebugger(void)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Ensure that if we throw, we'll call TerminateDebugger to cleanup.
    // This makes our Init more atomic by avoiding partially-init states.
    class EnsureCleanup {
        BOOL    fNeedCleanup;
    public:
        EnsureCleanup()
        {
            fNeedCleanup = TRUE;
        }

        void SuppressCleanup()
        {
            fNeedCleanup  = FALSE;
        }

        ~EnsureCleanup()
        {
             STATIC_CONTRACT_NOTHROW;
             STATIC_CONTRACT_GC_NOTRIGGER;
             STATIC_CONTRACT_MODE_ANY;
            if (fNeedCleanup) { TerminateDebugger(); };
        }
    } hCleanup;

    HRESULT hr = S_OK;

    LOG((LF_CORDB, LL_INFO10, "Initializing left-side debugging services.\n"));

    FARPROC gi = (FARPROC) &CorDBGetInterface;

    // Init the interface the EE provides to the debugger,
    // ask the debugger for its interface, and if all goes
    // well call Startup on the debugger.
    EEDbgInterfaceImpl::Init();
    _ASSERTE(g_pEEDbgInterfaceImpl != NULL); // throws on OOM

    // This allocates the Debugger object.
    typedef HRESULT __cdecl CORDBGETINTERFACE(DebugInterface**);
    hr = ((CORDBGETINTERFACE*)gi)(&g_pDebugInterface);
    IfFailThrow(hr);

    g_pDebugInterface->SetEEInterface(g_pEEDbgInterfaceImpl);

    {
        hr = g_pDebugInterface->Startup(); // throw on error
        _ASSERTE(SUCCEEDED(hr));
    }

    // If there's a DebuggerThreadControl interface, then we
    // need to update the DebuggerSpecialThread list.
    if (CorHost::GetDebuggerThreadControl())
    {
        hr = CorHost::RefreshDebuggerSpecialThreadList();
        _ASSERTE((SUCCEEDED(hr)) && (hr != S_FALSE));

        // So we don't think this will ever fail, but just in case...
        IfFailThrow(hr);
    }

    LOG((LF_CORDB, LL_INFO10, "Left-side debugging services setup.\n"));


    // If there is a DebuggerThreadControl interface, then it was set before the debugger
    // was initialized and we need to provide this interface now.  If debugging is already
    // initialized then the IDTC pointer is passed in when it is set through CorHost
    IDebuggerThreadControl *pDTC = CorHost::GetDebuggerThreadControl();

    if (pDTC != NULL)
    {
        g_pDebugInterface->SetIDbgThreadControl(pDTC);
    }

    hCleanup.SuppressCleanup();
    return;
}


//
// TerminateDebugger shuts down the Runtime-side COM+ Debugging Services
// InitializeDebugger will call this if it fails.
// This may be called even if the debugger is partially initialized.
// This can be called multiple times.
//
static void TerminateDebugger(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10, "Shutting down left-side debugger services.\n"));

    // If initialized failed really early, then we didn't even get the Debugger object.
    if (g_pDebugInterface != NULL)
    {
        // Notify the out-of-process debugger that shutdown of the in-process debugging support has begun. This is only
        // really used in interop debugging scenarios.
        g_pDebugInterface->ShutdownBegun();

        // This will kill the helper thread, delete the Debugger object, and free all resources.
        g_pDebugInterface->StopDebugger();
        g_pDebugInterface = NULL;
    }

    // Delete this after Debugger, since Debugger may use this.
    EEDbgInterfaceImpl::Terminate();
    _ASSERTE(g_pEEDbgInterfaceImpl == NULL); // Terminate nulls this out for us.

    g_CORDebuggerControlFlags = DBCF_NORMAL_OPERATION;

    CorHost::CleanupDebuggerThreadControl();
}


// ---------------------------------------------------------------------------
// Initialize InterProcess Communications for COM+
// 1. Allocate an IPCManager Implementation and hook it up to our interface *
// 2. Call proper init functions to activate relevant portions of IPC block
// ---------------------------------------------------------------------------
static HRESULT InitializeIPCManager(void)
{
    CONTRACTL{
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("InitializeIPCManager");

    HRESULT hr = S_OK;
    HINSTANCE hInstIPCBlockOwner = 0;

    DWORD pid = 0;
    // Allocate the Implementation. Everyone else will work through the interface
    g_pIPCManagerInterface = new (nothrow) IPCWriterInterface();

    if (g_pIPCManagerInterface == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto errExit;
    }

    pid = GetCurrentProcessId();


    // Do general init
    hr = g_pIPCManagerInterface->Init();

    if (!SUCCEEDED(hr))
    {
        goto errExit;
    }

    // Generate private IPCBlock for our PID. Note that for the other side of the debugger,
    // they'll hook up to the debuggee's pid (and not their own). So we still
    // have to pass the PID in.
    EX_TRY
    {
        hr = g_pIPCManagerInterface->CreatePrivateBlockOnPid(pid, FALSE, &hInstIPCBlockOwner);
    }
    EX_CATCH_HRESULT(hr);


    // Generate public IPCBlock for our PID.
    EX_TRY
    {
        hr = g_pIPCManagerInterface->CreatePublicBlockOnPid(pid);
    }
    EX_CATCH_HRESULT(hr);


errExit:
    // If any failure, shut everything down.
    if (!SUCCEEDED(hr))
        TerminateIPCManager();

    return hr;
}

#endif // DEBUGGING_SUPPORTED

// ---------------------------------------------------------------------------
// Terminate all InterProcess operations
// ---------------------------------------------------------------------------
static void TerminateIPCManager(void)
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    if (g_pIPCManagerInterface != NULL)
    {
        g_pIPCManagerInterface->Terminate();

        delete g_pIPCManagerInterface;
        g_pIPCManagerInterface = NULL;
    }
}

void InitializeEventTracing()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#define ENABLE_ETW
#ifdef ENABLE_ETW
    if (g_pEtwTracer == NULL)
    {
        NewHolder <CEtwTracer> tempEtwTracer (new (nothrow) CEtwTracer());
        if (tempEtwTracer != NULL && tempEtwTracer->Register () == ERROR_SUCCESS)
            g_pEtwTracer = tempEtwTracer.Extract ();
    }
#endif  //ENABLE_ETW
}


// For ConvertLangIDToCultureName, found in culture.dll
typedef BOOL (STDMETHODCALLTYPE *PConvertLangIDToCultureName)(LANGID langID, LPWSTR pwzCultureName, SIZE_T* pcchCultureName, LPWSTR pwzParentName, SIZE_T *pcchParentName);

static BOOL callConvertLangIDToCultureName(LANGID langID, __out_opt LPWSTR pwzCultureName, SIZE_T* pcchCultureName, __out_opt LPWSTR pwzParentName, SIZE_T *pcchParentName)
{
    BOOL res = FALSE;

    HModuleHolder hCulture(NULL);
    if (SUCCEEDED(LoadLibraryShim(MAKEDLLNAME_W(L"culture"), NULL, 0, &hCulture)))
    {
        PConvertLangIDToCultureName pfnConvertLangIDToCultureName = NULL;

        pfnConvertLangIDToCultureName = (PConvertLangIDToCultureName) GetProcAddress(hCulture, "ConvertLangIdToCultureName");
        _ASSERTE(pfnConvertLangIDToCultureName != NULL);

        if (pfnConvertLangIDToCultureName != NULL)
        {
            res = pfnConvertLangIDToCultureName(langID, pwzCultureName, pcchCultureName, pwzParentName, pcchParentName);
        }
    }

    return res;
}// callConvertLangIDToCultureName

// ---------------------------------------------------------------------------
// Impl for UtilLoadStringRC Callback: In VM, we let the thread decide culture
// copy culture name into szBuffer and return length
// ---------------------------------------------------------------------------
static HRESULT GetThreadUICultureNames(__out_ecount(cchCultureName) LPWSTR wszCultureName, SIZE_T cchCultureName,
                                   __out_ecount(cchParentCultureName) LPWSTR wszParentCultureName, SIZE_T cchParentCultureName)
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(wszCultureName));
        PRECONDITION(CheckPointer(wszParentCultureName));
        SO_INTOLERANT;
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    SIZE_T cchCultureNameResult = 0;
    SIZE_T cchParentCultureNameResult = 0;

    _ASSERT(cchCultureName > 0);
    _ASSERT(cchParentCultureName > 0);
    
    wszCultureName[0] = 0;
    wszParentCultureName[0] = 0;

    Thread * pThread = GetThread();

    if (pThread != NULL) {

        // Switch to cooperative mode, since we'll be looking at managed objects
        // and we don't want them moving on us.
        GCX_COOP();

        THREADBASEREF pThreadBase = (THREADBASEREF)pThread->GetExposedObjectRaw();

        if (pThreadBase != NULL)
        {
            CULTUREINFOBASEREF pCurrentCulture = pThreadBase->GetCurrentUICulture();

            if (pCurrentCulture != NULL)
            {
                STRINGREF cultureName = pCurrentCulture->GetName();

                if (cultureName != NULL)
                {
                    cchCultureNameResult = cultureName->GetStringLength();
                    if (cchCultureNameResult < cchCultureName)
                    {
                        memcpy(wszCultureName, cultureName->GetBuffer(), cchCultureNameResult*sizeof(WCHAR));
                        wszCultureName[cchCultureNameResult]='\0';
                    }
                }

                CULTUREINFOBASEREF pParentCulture = pCurrentCulture->GetParent();

                if (pParentCulture != NULL)
                {
                    STRINGREF parentCultureName = pParentCulture->GetName();

                    if (parentCultureName != NULL)
                    {
                        cchParentCultureNameResult = parentCultureName->GetStringLength();
                        if (cchParentCultureNameResult < cchParentCultureName)
                        {
                            memcpy(wszParentCultureName, parentCultureName->GetBuffer(), cchParentCultureNameResult*sizeof(WCHAR));
                            wszParentCultureName[cchParentCultureNameResult]='\0';
                        }
                    }

                }
            }
        }
    }

    // If the lazily-initialized cultureinfo structures aren't initialized yet, we'll
    // need to do the lookup the hard way.
    if (cchCultureNameResult == 0 || cchParentCultureNameResult==0)
    {
        INT32 id = GetThreadUICultureId();
        _ASSERTE(id !=0 && id != (int)UICULTUREID_DONTCARE);

        if (!callConvertLangIDToCultureName(id, wszCultureName, &cchCultureName, wszParentCultureName, &cchParentCultureName))    
        {
            hr = E_FAIL;
        }                
    }

    return hr;
}

// ---------------------------------------------------------------------------
// Impl for UtilLoadStringRC Callback: In VM, we let the thread decide culture
// Return an int uniquely describing which language this thread is using for ui.
// ---------------------------------------------------------------------------
static int GetThreadUICultureId()
{
    CONTRACTL{
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_INTOLERANT;;
    } CONTRACTL_END;


    int Result = UICULTUREID_DONTCARE;

    Thread * pThread = GetThread();

    if (pThread != NULL) {

        // Switch to cooperative mode, since we'll be looking at managed objects
        // and we don't want them moving on us.
        GCX_COOP();

        THREADBASEREF pThreadBase = (THREADBASEREF)pThread->GetExposedObjectRaw();

        if (pThreadBase != NULL)
        {
            CULTUREINFOBASEREF pCurrentCulture = pThreadBase->GetCurrentUICulture();

            if (pCurrentCulture != NULL)
                Result = pCurrentCulture->GetCultureID();
        }
    }

    if (Result == (int)UICULTUREID_DONTCARE)
    {
        // This thread isn't set up to use a non-default culture. Let's grab the default
        // one and return that.


        if (Result == 0 || Result == (int)UICULTUREID_DONTCARE)
            Result = GetUserDefaultLangID();

        _ASSERTE(Result != 0);
        if (Result == 0)
        {
            Result = (int)UICULTUREID_DONTCARE;
        }
    }

    return Result;
}

// ---------------------------------------------------------------------------
// Export shared logging code for JIT, et.al.
// ---------------------------------------------------------------------------
#ifdef _DEBUG

extern VOID LogAssert( LPCSTR szFile, int iLine, LPCSTR expr);
extern "C"
//__declspec(dllexport)
VOID STDMETHODCALLTYPE LogHelp_LogAssert( LPCSTR szFile, int iLine, LPCSTR expr)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        ENTRY_POINT;            
        PRECONDITION(CheckPointer(szFile));
        PRECONDITION(CheckPointer(expr));
    }  CONTRACTL_END;

    BEGIN_ENTRYPOINT_VOIDRET;
    LogAssert(szFile, iLine, expr);
    END_ENTRYPOINT_VOIDRET;

}

extern BOOL NoGuiOnAssert();
extern "C"
//__declspec(dllexport)
BOOL STDMETHODCALLTYPE LogHelp_NoGuiOnAssert()
{
    WRAPPER_CONTRACT;
    BOOL fRet = FALSE;
    BEGIN_ENTRYPOINT_VOIDRET;
    fRet = NoGuiOnAssert();
    END_ENTRYPOINT_VOIDRET;
    return fRet;
}

extern VOID TerminateOnAssert();
extern "C"
//__declspec(dllexport)
VOID STDMETHODCALLTYPE LogHelp_TerminateOnAssert()
{
    WRAPPER_CONTRACT;
    BEGIN_ENTRYPOINT_VOIDRET;
//  __asm int 3;
    TerminateOnAssert();
    END_ENTRYPOINT_VOIDRET;

}

#else // !_DEBUG

extern "C"
//__declspec(dllexport)
VOID STDMETHODCALLTYPE LogHelp_LogAssert( LPCSTR szFile, int iLine, LPCSTR expr) {
    LEAF_CONTRACT;

    //BEGIN_ENTRYPOINT_VOIDRET;
    //END_ENTRYPOINT_VOIDRET;
}

extern "C"
//__declspec(dllexport)
BOOL STDMETHODCALLTYPE LogHelp_NoGuiOnAssert() {
    LEAF_CONTRACT;

    //BEGIN_ENTRYPOINT_VOIDRET;
    //END_ENTRYPOINT_VOIDRET;

    return FALSE;
}

extern "C"
//__declspec(dllexport)
VOID STDMETHODCALLTYPE LogHelp_TerminateOnAssert() {
    LEAF_CONTRACT;

    //BEGIN_ENTRYPOINT_VOIDRET;
    //END_ENTRYPOINT_VOIDRET;

}

#endif // _DEBUG


#ifndef ENABLE_PERF_COUNTERS
//
// perf counter stubs for builds which don't have perf counter support
// These are needed because we export these functions in our DLL


Perf_Contexts* STDMETHODCALLTYPE GetPrivateContextsPerfCounters()
{
    LEAF_CONTRACT;

    //BEGIN_ENTRYPOINT_VOIDRET;
    //END_ENTRYPOINT_VOIDRET;

    return NULL;
}


#endif

#ifdef PLATFORM_UNIX
//
// This routine is not directly called from within the CLR.  It is called
// by the developer debugging the CLR, from within GDB.  It's best to put
// this in your .gdbinit file:
//  define sos
//  call (void)SOS("$arg0")
//  echo \n
//  end
//
// Then you can use SOS from the (gdb) prompt like this:
//  sos Help
//  sos DumpStack\ -EE
//               ^
//               | note that space characters must be escaped!
//
extern "C" void __cdecl SOS(__in char *Command)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
        PRECONDITION(CheckPointer(Command));
    } CONTRACTL_END;

    static HMODULE hSOS;

    if (!Command) {
        fprintf(stderr, "SOS:  you must specify a command to run\n");
        return;
    }

    if (!hSOS) {
        hSOS = CLRLoadLibrary(MAKEDLLNAME_W(L"sos"));
        if (!hSOS) {
            fprintf(stderr, "SOS:  Failed to load " MAKEDLLNAME_A("sos") ".  LastErr=%d\n",
               GetLastError());
       return;
    }
    }

    typedef void (*SOSAPI)(HMODULE, char *);
    static SOSAPI pfnSOSAPI;

    if (!pfnSOSAPI) {
    pfnSOSAPI = (SOSAPI)GetProcAddress(hSOS, "GDBSOS");
    if (!pfnSOSAPI) {
        fprintf(stderr, "SOS:  Couldn't find GDBSOS() in libsos.so");
            return;
        }
    }

        (*pfnSOSAPI)(hSOS, Command);

    // It is a bad idea to load and free libsos back and forth since it is
    //  just increasing the chance of crashing in the debugged process
    // FreeLibrary(hSOS);
}
#endif //PLATFORM_UNIX
