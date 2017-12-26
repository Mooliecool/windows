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
// CorHost.h
//
// Class factories are used by the pluming in COM to activate new objects.
// This module contains the class factory code to instantiate the debugger
// objects described in <cordb.h>.
//
//*****************************************************************************
#ifndef __CorHost__h__
#define __CorHost__h__

#include "windows.h" // worth to include before mscoree.h so we are guaranteed to pick few definitions
#ifdef CreateSemaphore
#undef CreateSemaphore
#endif
#include "mscoree.h"
#include "ivehandler.h"
#include "ivalidator.h"
#include "fusion.h"
#include "threadpool.h"
#include "holder.h"

#define INVALID_STACK_BASE_MARKER_FOR_CHECK_STATE 2

#if (_WIN32_WINNT < 0x0500)
typedef VOID (__stdcall *WAITORTIMERCALLBACK)(PVOID, BOOL);
#endif

GVAL_DECL(ECustomDumpFlavor, g_ECustomDumpFlavor);

class AppDomain;

class CorThreadpool : public ICorThreadpool
{
public:
    HRESULT STDMETHODCALLTYPE  CorRegisterWaitForSingleObject(PHANDLE phNewWaitObject,
                                                              HANDLE hWaitObject,
                                                              WAITORTIMERCALLBACK Callback,
                                                              PVOID Context,
                                                              ULONG timeout,
                                                              BOOL  executeOnlyOnce,
                                                              BOOL* pResult);

    HRESULT STDMETHODCALLTYPE  CorBindIoCompletionCallback(HANDLE fileHandle, LPOVERLAPPED_COMPLETION_ROUTINE callback);

    HRESULT STDMETHODCALLTYPE  CorUnregisterWait(HANDLE hWaitObject,HANDLE CompletionEvent, BOOL* pResult);

    HRESULT STDMETHODCALLTYPE  CorQueueUserWorkItem(LPTHREAD_START_ROUTINE Function,PVOID Context,BOOL executeOnlyOnce, BOOL* pResult );

    HRESULT STDMETHODCALLTYPE  CorCallOrQueueUserWorkItem(LPTHREAD_START_ROUTINE Function,PVOID Context,BOOL* pResult );

    HRESULT STDMETHODCALLTYPE  CorCreateTimer(PHANDLE phNewTimer,
                                              WAITORTIMERCALLBACK Callback,
                                              PVOID Parameter,
                                              DWORD DueTime,
                                              DWORD Period,
                                              BOOL* pResult);

    HRESULT STDMETHODCALLTYPE  CorDeleteTimer(HANDLE Timer, HANDLE CompletionEvent, BOOL* pResult);

    HRESULT STDMETHODCALLTYPE  CorChangeTimer(HANDLE Timer,ULONG DueTime,ULONG Period, BOOL* pResult);

    HRESULT STDMETHODCALLTYPE CorSetMaxThreads(DWORD MaxWorkerThreads,
                                               DWORD MaxIOCompletionThreads);

    HRESULT STDMETHODCALLTYPE CorGetMaxThreads(DWORD *MaxWorkerThreads,
                                               DWORD *MaxIOCompletionThreads);

    HRESULT STDMETHODCALLTYPE CorGetAvailableThreads(DWORD *AvailableWorkerThreads,
                                                  DWORD *AvailableIOCompletionThreads);
};

class CorGCHost : public IGCHost
{
public:
    // IGCHost
    STDMETHODIMP STDMETHODCALLTYPE SetGCStartupLimits(
        DWORD SegmentSize,
        DWORD MaxGen0Size);

    STDMETHODIMP STDMETHODCALLTYPE Collect(
        LONG Generation);

    STDMETHODIMP STDMETHODCALLTYPE GetStats(
        COR_GC_STATS *pStats);

    STDMETHODIMP STDMETHODCALLTYPE GetThreadStats(
        DWORD *pFiberCookie,
        COR_GC_THREAD_STATS *pStats);

    STDMETHODIMP STDMETHODCALLTYPE SetVirtualMemLimit(
        SIZE_T sztMaxVirtualMemMB);
};

class CorConfiguration : public ICorConfiguration
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetGCThreadControl(
        /* [in] */ IGCThreadControl __RPC_FAR *pGCThreadControl);

    virtual HRESULT STDMETHODCALLTYPE SetGCHostControl(
        /* [in] */ IGCHostControl __RPC_FAR *pGCHostControl);

    virtual HRESULT STDMETHODCALLTYPE SetDebuggerThreadControl(
        /* [in] */ IDebuggerThreadControl __RPC_FAR *pDebuggerThreadControl);

    virtual HRESULT STDMETHODCALLTYPE AddDebuggerSpecialThread(
        /* [in] */ DWORD dwSpecialThreadId);

    // This mechanism isn't thread-safe with respect to reference counting, because
    // the runtime will use the cached pointer without adding extra refcounts to protect
    // itself.  So if one thread calls GetGCThreadControl & another thread calls
    // ICorHost::SetGCThreadControl, we have a race.
    static IGCThreadControl *GetGCThreadControl()
    {
        LEAF_CONTRACT;

        return m_CachedGCThreadControl;
    }

    static IGCHostControl *GetGCHostControl()
    {
        LEAF_CONTRACT;

        return m_CachedGCHostControl;
    }

    static IDebuggerThreadControl *GetDebuggerThreadControl()
    {
        LEAF_CONTRACT;

        return m_CachedDebuggerThreadControl;
    }

    static DWORD GetDebuggerSpecialThreadCount()
    {
        LEAF_CONTRACT;

        return m_DSTCount;
    }

    static DWORD *GetDebuggerSpecialThreadArray()
    {
        LEAF_CONTRACT;

        return m_DSTArray;
    }

    // Helper function that returns true if the thread is in the debugger special thread list
    static BOOL IsDebuggerSpecialThread(DWORD dwThreadId);

    // Helper function to update the thread list in the debugger control block
    static HRESULT RefreshDebuggerSpecialThreadList();

    // Clean up debugger thread control object, called at shutdown
    static void CleanupDebuggerThreadControl();

private:
    // Cache the IGCThreadControl interface until the EE is started, at which point
    // we pass it through.
    static IGCThreadControl *m_CachedGCThreadControl;
    static IGCHostControl *m_CachedGCHostControl;
    static IDebuggerThreadControl *m_CachedDebuggerThreadControl;

    // Array of ID's of threads that should be considered "special" to
    // the debugging services.
    static DWORD *m_DSTArray;
    static DWORD  m_DSTArraySize;
    static DWORD  m_DSTCount;
};

class CorValidator : public IValidator
{
protected:
    CorValidator() {LEAF_CONTRACT;}

public:
    STDMETHODIMP STDMETHODCALLTYPE Validate(
            IVEHandler        *veh,
            IUnknown          *pAppDomain,
            unsigned long      ulFlags,
            unsigned long      ulMaxError,
            unsigned long      token,
            __in_z LPWSTR             fileName,
            BYTE               *pe,
            unsigned long      ulSize);

    STDMETHODIMP STDMETHODCALLTYPE FormatEventInfo(
            HRESULT            hVECode,
            VEContext          Context,
            __out_ecount(ulMaxLength) LPWSTR             msg,
            unsigned long      ulMaxLength,
            SAFEARRAY         *psa);
};

class CLRValidator : public ICLRValidator
{
protected:
    CLRValidator() {LEAF_CONTRACT;}

public:
    STDMETHODIMP STDMETHODCALLTYPE Validate(
            IVEHandler        *veh,
            unsigned long      ulAppDomainId,
            unsigned long      ulFlags,
            unsigned long      ulMaxError,
            unsigned long      token,
            __in_z LPWSTR             fileName,
            BYTE               *pe,
            unsigned long      ulSize);

    STDMETHODIMP STDMETHODCALLTYPE FormatEventInfo(
            HRESULT            hVECode,
            VEContext          Context,
            __out_ecount(ulMaxLength) LPWSTR             msg,
            unsigned long      ulMaxLength,
            SAFEARRAY         *psa);
};

class CorDebuggerInfo : public IDebuggerInfo
{
public:
    STDMETHODIMP IsDebuggerAttached(BOOL *pbAttached);
};

class CorRuntimeHostBase
{
protected:
    CorRuntimeHostBase()
    :m_Started(FALSE),
     m_cRef(0)
    {LEAF_CONTRACT;}

    STDMETHODIMP_(ULONG) AddRef(void);

    // Starts the runtime. This is equivalent to CoInitializeCor()
    STDMETHODIMP Start();

    // Creates a domain in the runtime. The identity array is
    // a pointer to an array TYPE containing IIdentity objects defining
    // the security identity.
    STDMETHODIMP CreateDomain(LPCWSTR pwzFriendlyName,   // Optional
                              IUnknown* pIdentityArray, // Optional
                              IUnknown ** pAppDomain);

    // Returns the default domain.
    STDMETHODIMP GetDefaultDomain(IUnknown ** pAppDomain);


    // Enumerate currently existing domains.
    STDMETHODIMP EnumDomains(HDOMAINENUM *hEnum);

    // Returns S_FALSE when there are no more domains. A domain
    // is passed out only when S_OK is returned.
    STDMETHODIMP NextDomain(HDOMAINENUM hEnum,
                            IUnknown** pAppDomain);

    // Close the enumeration releasing resources
    STDMETHODIMP CloseEnum(HDOMAINENUM hEnum);

    STDMETHODIMP CreateDomainEx(LPCWSTR pwzFriendlyName,
                                IUnknown* pSetup, // Optional
                                IUnknown* pEvidence, // Optional
                                IUnknown ** pAppDomain);

    // Create appdomain setup object that can be passed into CreateDomainEx
    STDMETHODIMP CreateDomainSetup(IUnknown** pAppDomainSetup);

    // Create Evidence object that can be passed into CreateDomainEx
    STDMETHODIMP CreateEvidence(IUnknown** pEvidence);

    // Unload a domain, releasing the reference will only release the
    // the wrapper to the domain not unload the domain.
    STDMETHODIMP UnloadDomain(IUnknown* pAppDomain);

    // Returns the threads domain if there is one.
    STDMETHODIMP CurrentDomain(IUnknown ** pAppDomain);

    STDMETHODIMP MapFile(                       // Return code.
        HANDLE     hFile,                       // [in]  Handle for file
        HMODULE   *hMapAddress                  // [out] HINSTANCE for mapped file
        );

    STDMETHODIMP LocksHeldByLogicalThread(      // Return code.
        DWORD *pCount                           // [out] Number of locks that the current thread holds.
        );

protected:
    BOOL        m_Started;              // Has START been called?

    LONG        m_cRef;                 // Ref count.


    static ULONG       m_Version;              // Version of ICorRuntimeHost.
                                        // Some functions are only available in ICLRRuntimeHost.
                                        // Some functions are no-op in ICLRRuntimeHost.

    STDMETHODIMP UnloadAppDomain(DWORD dwDomainId, BOOL fWaitUntilDone);

public:
    static ULONG GetHostVersion()
    {
        LEAF_CONTRACT;
        _ASSERTE (m_Version != 0);
        return m_Version;
    }

};


class CorHost :
    public CorRuntimeHostBase, public ICorRuntimeHost, public CorThreadpool
    , public CorGCHost, public CorConfiguration
    , public CorValidator, public CorDebuggerInfo
{
public:
    CorHost() {WRAPPER_CONTRACT;}

    // *** IUnknown methods ***
    STDMETHODIMP    QueryInterface(REFIID riid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef(void)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::AddRef();
    }
    STDMETHODIMP_(ULONG) Release(void);


    // *** ICorRuntimeHost methods ***
    // Returns an object for configuring the runtime prior to
    // it starting. If the runtime has been initialized this
    // routine returns an error. See ICorConfiguration.
    STDMETHODIMP GetConfiguration(ICorConfiguration** pConfiguration);


    // Starts the runtime. This is equivalent to CoInitializeCor();
    STDMETHODIMP Start(void);

    STDMETHODIMP Stop();

    // Creates a domain in the runtime. The identity array is
    // a pointer to an array TYPE containing IIdentity objects defining
    // the security identity.
    STDMETHODIMP CreateDomain(LPCWSTR pwzFriendlyName,   // Optional
                              IUnknown* pIdentityArray, // Optional
                              IUnknown ** pAppDomain)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::CreateDomain(pwzFriendlyName,pIdentityArray,pAppDomain);
    }

    // Returns the default domain.
    STDMETHODIMP GetDefaultDomain(IUnknown ** pAppDomain)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::GetDefaultDomain(pAppDomain);
    }

    // Enumerate currently existing domains.
    STDMETHODIMP EnumDomains(HDOMAINENUM *hEnum)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::EnumDomains(hEnum);
    }

    // Returns S_FALSE when there are no more domains. A domain
    // is passed out only when S_OK is returned.
    STDMETHODIMP NextDomain(HDOMAINENUM hEnum,
                            IUnknown** pAppDomain)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::NextDomain(hEnum,pAppDomain);
    }

    // Close the enumeration releasing resources
    STDMETHODIMP CloseEnum(HDOMAINENUM hEnum)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::CloseEnum(hEnum);
    }

    STDMETHODIMP CreateDomainEx(LPCWSTR pwzFriendlyName,
                                IUnknown* pSetup, // Optional
                                IUnknown* pEvidence, // Optional
                                IUnknown ** pAppDomain)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::CreateDomainEx(pwzFriendlyName,pSetup,pEvidence,pAppDomain);
    }

    // Create appdomain setup object that can be passed into CreateDomainEx
    STDMETHODIMP CreateDomainSetup(IUnknown** pAppDomainSetup)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::CreateDomainSetup(pAppDomainSetup);
    }

    // Create Evidence object that can be passed into CreateDomainEx
    STDMETHODIMP CreateEvidence(IUnknown** pEvidence)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::CreateEvidence(pEvidence);
    }

    // Unload a domain, releasing the reference will only release the
    // the wrapper to the domain not unload the domain.
    STDMETHODIMP UnloadDomain(IUnknown* pAppDomain)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::UnloadDomain(pAppDomain);
    }

    // Returns the threads domain if there is one.
    STDMETHODIMP CurrentDomain(IUnknown ** pAppDomain)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::CurrentDomain(pAppDomain);
    }

    STDMETHODIMP CreateLogicalThreadState();    // Return code.
    STDMETHODIMP DeleteLogicalThreadState();    // Return code.
    STDMETHODIMP SwitchInLogicalThreadState(    // Return code.
        DWORD *pFiberCookie                     // [in] Cookie that indicates the fiber to use.
        );

    STDMETHODIMP SwitchOutLogicalThreadState(   // Return code.
        DWORD **pFiberCookie                    // [out] Cookie that indicates the fiber being switched out.
        );

    STDMETHODIMP LocksHeldByLogicalThread(      // Return code.
        DWORD *pCount                           // [out] Number of locks that the current thread holds.
        )
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::LocksHeldByLogicalThread(pCount);
    }

    // Class factory hook-up.
    static HRESULT CreateObject(REFIID riid, void **ppUnk);

    STDMETHODIMP MapFile(                       // Return code.
        HANDLE     hFile,                       // [in]  Handle for file
        HMODULE   *hMapAddress                  // [out] HINSTANCE for mapped file
        )
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::MapFile(hFile,hMapAddress);
    }
};

class ConnectionNameTable;
class CrstStatic;

// Defines the precedence (in increading oder) of the two symbol reading knobs
enum ESymbolReadingSetBy
{
    eSymbolReadingSetByDefault,
    eSymbolReadingSetByConfig,  // EEConfig - config file, env var, etc.
    eSymbolReadingSetByHost,    // Hosting API - highest precedence
    eSymbolReadingSetBy_COUNT
};

class CCLRDebugManager : public ICLRDebugManager
{
public:
    CCLRDebugManager() {LEAF_CONTRACT;};

    STDMETHODIMP    QueryInterface(REFIID riid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef(void)
    {
        LEAF_CONTRACT;
        return 1;
    }
    STDMETHODIMP_(ULONG) Release(void);

    // ICLRTDebugManager's interface
    STDMETHODIMP BeginConnection(
        CONNID  dwConnectionId,
        __in_z wchar_t *szConnectionName);
    STDMETHODIMP SetConnectionTasks(
        DWORD id,
        DWORD dwCount,
        ICLRTask **ppCLRTask);
    STDMETHODIMP EndConnection(
        CONNID  dwConnectionId);

    // Set ACL on shared section, events, and process
    STDMETHODIMP SetDacl(PACL pacl);

    // Returning the current ACL that CLR is using
    STDMETHODIMP GetDacl(PACL *pacl);

    STDMETHODIMP IsDebuggerAttached(BOOL *pbAttached);

    // symbol reading policy - include file line info when getting a call stack etc.
    STDMETHODIMP SetSymbolReadingPolicy(ESymbolReadingPolicy policy);

    // function to iterate connection upon debugger attach
    static HRESULT IterateConnectionForAttach(BOOL fAttaching);
    static void ProcessInit();
    static void ProcessCleanup();

    // Get the current symbol reading policy setting
    static ESymbolReadingPolicy GetSymbolReadingPolicy()
    {
        return m_symbolReadingPolicy;
    }

    // Set the symbol reading policy if the setter has higher precendence than the current setting
    static void SetSymbolReadingPolicy( ESymbolReadingPolicy policy, ESymbolReadingSetBy setBy );
    
private:
    static CrstStatic m_lockConnectionNameTable;
    static ConnectionNameTable *m_pConnectionNameHash;    
    static ESymbolReadingPolicy m_symbolReadingPolicy;
    static ESymbolReadingSetBy m_symbolReadingSetBy;
};



class CCLRErrorReportingManager : public ICLRErrorReportingManager
{
public:
    CCLRErrorReportingManager() {LEAF_CONTRACT;};

    STDMETHODIMP    QueryInterface(REFIID riid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // Get Watson bucket parameters for "current" exception (on calling thread).
    STDMETHODIMP GetBucketParametersForCurrentException(BucketParameters *pParams);
    STDMETHODIMP BeginCustomDump(   ECustomDumpFlavor dwFlavor,
                                        DWORD dwNumItems,
                                        CustomDumpItem items[],
                                        DWORD dwReserved);
    STDMETHODIMP EndCustomDump();
    

};



class CorHost2 :
    public CorRuntimeHostBase, public ICLRRuntimeHost, public CorThreadpool
    , public CorGCHost, public CorConfiguration
    , public CLRValidator, public CorDebuggerInfo
{
public:
    CorHost2() {LEAF_CONTRACT;}

    // *** IUnknown methods ***
    STDMETHODIMP    QueryInterface(REFIID riid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef(void)
    {
        WRAPPER_CONTRACT;
        return CorRuntimeHostBase::AddRef();
    }
    STDMETHODIMP_(ULONG) Release(void);


    // *** ICorRuntimeHost methods ***
    // Returns an object for configuring the runtime prior to
    // it starting. If the runtime has been initialized this
    // routine returns an error. See ICorConfiguration.
    STDMETHODIMP GetConfiguration(ICorConfiguration** pConfiguration);

    // Starts the runtime. This is equivalent to CoInitializeCor().
    STDMETHODIMP Start();
    STDMETHODIMP Stop();

    STDMETHODIMP ExecuteInAppDomain(DWORD dwAppDomainId,
                                    FExecuteInAppDomainCallback pCallback,
                                    void * cookie);

    STDMETHODIMP LocksHeldByLogicalThread(      // Return code.
        DWORD *pCount                           // [out] Number of locks that the current thread holds.
        )
    {
        WRAPPER_CONTRACT;
        return CorRuntimeHostBase::LocksHeldByLogicalThread(pCount);
    }

    // Class factory hook-up.
    static HRESULT CreateObject(REFIID riid, void **ppUnk);

    STDMETHODIMP MapFile(                       // Return code.
        HANDLE     hFile,                       // [in]  Handle for file
        HMODULE   *hMapAddress                  // [out] HINSTANCE for mapped file
        )
    {
        WRAPPER_CONTRACT;
        return CorRuntimeHostBase::MapFile(hFile,hMapAddress);
    }

    STDMETHODIMP STDMETHODCALLTYPE SetHostControl(
        IHostControl* pHostControl);

    STDMETHODIMP STDMETHODCALLTYPE GetCLRControl(
        ICLRControl** pCLRControl);

    STDMETHODIMP UnloadAppDomain(DWORD dwDomainId, BOOL fWaitUntilDone)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return CorRuntimeHostBase::UnloadAppDomain(dwDomainId, fWaitUntilDone);
    }

    STDMETHODIMP GetCurrentAppDomainId(DWORD *pdwAppDomainId);

    STDMETHODIMP ExecuteApplication(LPCWSTR  pwzAppFullName,
                                    DWORD    dwManifestPaths,
                                    LPCWSTR  *ppwzManifestPaths,
                                    DWORD    dwActivationData,
                                    LPCWSTR  *ppwzActivationData,
                                    int      *pReturnValue);

    STDMETHODIMP ExecuteInDefaultAppDomain(LPCWSTR pwzAssemblyPath,
                                           LPCWSTR pwzTypeName,
                                           LPCWSTR pwzMethodName,
                                           LPCWSTR pwzArgument,
                                           DWORD   *pReturnValue);

    static IHostControl *GetHostControl ()
    {
        LEAF_CONTRACT;

        return m_HostControl;
    }

    static IHostMemoryManager *GetHostMemoryManager ()
    {
        LEAF_CONTRACT;

        return m_HostMemoryManager;
    }

    static IHostMalloc *GetHostMalloc ()
    {
        LEAF_CONTRACT;

        return m_HostMalloc;
    }

    static IHostTaskManager *GetHostTaskManager ()
    {
        LEAF_CONTRACT;

        return m_HostTaskManager;
    }

    static IHostThreadpoolManager *GetHostThreadpoolManager ()
    {
        LEAF_CONTRACT;

        return m_HostThreadpoolManager;
    }

    static IHostIoCompletionManager *GetHostIoCompletionManager ()
    {
        LEAF_CONTRACT;

        return m_HostIoCompletionManager;
    }

    static IHostSyncManager *GetHostSyncManager ()
    {
        LEAF_CONTRACT;

        return m_HostSyncManager;
    }

    static IHostAssemblyManager *GetHostAssemblyManager()
    {
        LEAF_CONTRACT;

        return m_HostAssemblyManager;
    }

    static IHostGCManager *GetHostGCManager()
    {
        LEAF_CONTRACT;

        return m_HostGCManager;
    }

    static IHostSecurityManager *GetHostSecurityManager()
    {
        LEAF_CONTRACT;

        return m_HostSecurityManager;
    }

    static IHostPolicyManager *GetHostPolicyManager ()
    {
        LEAF_CONTRACT;

        return m_HostPolicyManager;
    }

    static int GetHostOverlappedExtensionSize()
    {
        LEAF_CONTRACT;

        _ASSERTE (m_HostOverlappedExtensionSize != -1);
        return m_HostOverlappedExtensionSize;
    }

    static ICLRAssemblyReferenceList *GetHostDomainNeutralAsms()
    {
        LEAF_CONTRACT;
        return m_pHostDomainNeutralAsms;
    }

    static LPCWSTR GetAppDomainManagerAsm();

    static LPCWSTR GetAppDomainManagerType();

    static BOOL HasStarted()
    {
        return m_RefCount != 0;
    }
    
    static BOOL IsLoadFromBlocked(); // LoadFrom, LoadFile and Load(byte[]) are blocked in certain hosting scenarios

private:
    static IHostControl *m_HostControl;
    static LONG  m_RefCount;

protected:
    static IHostMemoryManager *m_HostMemoryManager;
    static IHostMalloc *m_HostMalloc;
    static IHostTaskManager  *m_HostTaskManager;
    static IHostThreadpoolManager *m_HostThreadpoolManager;
    static IHostIoCompletionManager *m_HostIoCompletionManager;
    static IHostSyncManager *m_HostSyncManager;
    static IHostAssemblyManager *m_HostAssemblyManager;
    static IHostGCManager *m_HostGCManager;
    static IHostSecurityManager *m_HostSecurityManager;
    static IHostPolicyManager *m_HostPolicyManager;

    static int m_HostOverlappedExtensionSize;
    static ICLRAssemblyReferenceList *m_pHostDomainNeutralAsms;
};


class CorHostProtectionManager : public ICLRHostProtectionManager
{
private:
    EApiCategories m_eProtectedCategories;
    bool m_fEagerSerializeGrantSet;
    bool m_fFrozen;

public:
    CorHostProtectionManager();

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID id,
        void **pInterface);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // Interface methods
    virtual HRESULT STDMETHODCALLTYPE SetProtectedCategories(/* [in] */ EApiCategories eFullTrustOnlyResources);
    virtual HRESULT STDMETHODCALLTYPE SetEagerSerializeGrantSets();

    // Getters
    EApiCategories GetProtectedCategories();
    bool GetEagerSerializeGrantSets() const;

    void Freeze();
};


extern DWORD Host_SegmentSize;
extern DWORD Host_MaxGen0Size;
extern BOOL  Host_fSegmentSizeSet;
extern BOOL  Host_fMaxGen0SizeSet;

#endif // __CorHost__h__
