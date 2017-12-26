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
// --------------------------------------------------------------------------------
// DomainFile.h
// --------------------------------------------------------------------------------

#ifndef _DOMAINFILE_H_
#define _DOMAINFILE_H_

// --------------------------------------------------------------------------------
// Required headers
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// Forward class declarations
// --------------------------------------------------------------------------------
#include "securitydescriptorassembly.h"
class AppDomain;
class DomainAssembly;
class DomainModule;
class Assembly;
class Module;
class DynamicMethodTable;

enum FileLoadLevel
{
    // These states are tracked by FileLoadLock

    // Note that semantics here are description is the LAST step done, not what is 
    // currently being done.

    FILE_LOAD_CREATE,
    FILE_LOAD_BEGIN,
    FILE_LOAD_FIND_NATIVE_IMAGE,
    FILE_LOAD_VERIFY_NATIVE_IMAGE_DEPENDENCIES,
    FILE_LOAD_ALLOCATE,
    FILE_LOAD_ADD_DEPENDENCIES,
    FILE_LOAD_PRE_LOADLIBRARY,
    FILE_LOAD_LOADLIBRARY,
    FILE_LOAD_POST_LOADLIBRARY,
    FILE_LOAD_EAGER_FIXUPS,
    FILE_LOAD_DELIVER_EVENTS,
    FILE_LOADED,                    // Loaded by not yet active
    FILE_LOAD_VERIFY_EXECUTION,
    FILE_ACTIVE                     // Fully active (constructors run & security checked)
};


enum NotificationStatus
{
	NOT_NOTIFIED=0,
	PROFILER_NOTIFIED=1,
	DEBUGGER_NEEDNOTIFICATION=2,
	DEBUGGER_NOTIFIED=4
};
// --------------------------------------------------------------------------------
// DomainFile represents a file loaded (or being loaded) into an app domain.  It
// is guranteed to be unique per file per app domain.
// --------------------------------------------------------------------------------

class DomainFile
{
    VPTR_BASE_VTABLE_CLASS(DomainFile);

  public:

    // ------------------------------------------------------------
    // Public API
    // ------------------------------------------------------------

#ifndef DACCESS_COMPILE
    virtual ~DomainFile();
    DomainFile() {LEAF_CONTRACT;};
#endif

    AppDomain *GetAppDomain()
    {
        LEAF_CONTRACT;
        return m_pDomain;
    }

    PEFile *GetFile()
    {
        LEAF_CONTRACT;
        return m_pFile;
    }

    IMDInternalImport *GetMDImport()
    {
        WRAPPER_CONTRACT;
        return m_pFile->GetPersistentMDImport();
    };

    OBJECTREF GetExposedModuleObjectAsModule();
    OBJECTREF GetExposedModuleObject();

    BOOL IsSystem()
    {
        WRAPPER_CONTRACT;
        return GetFile()->IsSystem();
    }

    LPCUTF8 GetSimpleName()
    {
        WRAPPER_CONTRACT;
        return GetFile()->GetSimpleName();
    }

#ifdef LOGGING
    LPCWSTR GetDebugName()
    {
        WRAPPER_CONTRACT;
        return GetFile()->GetDebugName();
    }
#endif


    void ReleaseFiles();

    virtual BOOL IsAssembly() = 0;

    DomainAssembly *GetDomainAssembly();

    // ------------------------------------------------------------
    // Loading state checks
    // ------------------------------------------------------------

    // Return the File's load level.  Note that this is the last level actually successfully completed.
    // Note that this is subtly different than the FileLoadLock's level, which is the last level
    // which was triggered (but potentially skipped if error or inappropriate.)
    FileLoadLevel GetLoadLevel() { LEAF_CONTRACT; return m_level; }

    // Error means that a permanent x-appdomain load error has occurred.
    BOOL IsError() { LEAF_CONTRACT; return m_pError != NULL; }

    // Loading means that the load is still being tracked by a FileLoadLock.
    BOOL IsLoading() { LEAF_CONTRACT; return m_loading; }

    // Loaded means that the file can be used passively.  This includes loading types, reflection, and
    // jitting.
    BOOL IsLoaded() { LEAF_CONTRACT; return m_level >= FILE_LOAD_DELIVER_EVENTS; }

    // Active means that the file can be used actively in the current app domain.  Note that a shared file
    // may conditionally not be able to be made active on a per app domain basis.
    BOOL IsActive() { LEAF_CONTRACT; return m_level >= FILE_ACTIVE; }

    // CheckLoaded is appropriate for asserts that the assembly can be passively used.  
    CHECK CheckLoaded();

    // CheckActivated is appropriate for asserts that the assembly can be actively used.  Note that
    // it is slightly different from IsActive in that it deals with reentrancy cases properly.
    CHECK CheckActivated();

    // Ensure that an assembly has reached at least the IsLoaded state.  Throw if not.
    void EnsureLoaded() 
    { 
        WRAPPER_CONTRACT;
        return EnsureLoadLevel(FILE_LOADED);
    }

    // Ensure that an assembly has reached at least the IsActive state.  Throw if not.
    void EnsureActive()
    { 
        WRAPPER_CONTRACT;
        return EnsureLoadLevel(FILE_ACTIVE);
    }

    // Ensure that an assembly has reached at least the Allocated state.  Throw if not.
    void EnsureAllocated()
    { 
        WRAPPER_CONTRACT;
        return EnsureLoadLevel(FILE_LOAD_ALLOCATE);
    }


    void EnsureLibraryLoaded()
    {
        WRAPPER_CONTRACT;
        return EnsureLoadLevel(FILE_LOAD_LOADLIBRARY);
    }

    // This wraps EnsureActive, suppressing non-transient exceptions
    BOOL TryEnsureActive();

    // EnsureLoadLevel is a generic routine used to ensure that the file is not in a delay loaded
    // state (unless it needs to be.)  This should be used when a particular level of loading
    // is required for an operation.  Note that deadlocks are tolerated so the level may be one
    void EnsureLoadLevel(FileLoadLevel targetLevel) DAC_EMPTY();

    // AttemptLoadLevel is a generic routine used to try to further load the file to a given level.
    // No guarantee is made about the load level resulting however.
    void AttemptLoadLevel(FileLoadLevel targetLevel) DAC_EMPTY();

    // CheckLoadLevel is an assert predicate used to verify the load level of an assembly.
    // deadlockOK indicates that the level is allowed to be one short if we are restricted
    // by loader reentrancy.
    CHECK CheckLoadLevel(FileLoadLevel requiredLevel, BOOL deadlockOK = TRUE) DAC_EMPTY_RET(CHECK::OK());

    // RequireLoadLevel throws an exception if the domain file isn't loaded enough.  Note
    // that this is intolerant of deadlock related failures so is only really appropriate for 
    // checks inside the main loading loop.
    void RequireLoadLevel(FileLoadLevel targetLevel) DAC_EMPTY();

    // Throws if a load error has occurred
    void ThrowIfError(FileLoadLevel targetLevel) DAC_EMPTY();

    // Checks that a load error has not occured before the given level
    CHECK CheckNoError(FileLoadLevel targetLevel) DAC_EMPTY_RET(CHECK::OK());

    // IsNotified means that the profiler API notification has been delivered
    BOOL IsProfilerNotified() { LEAF_CONTRACT; return m_notifyflags & PROFILER_NOTIFIED; }
    BOOL IsDebuggerNotified() { LEAF_CONTRACT; return m_notifyflags & DEBUGGER_NOTIFIED; }	
    BOOL ShouldNotifyDebugger() { LEAF_CONTRACT; return m_notifyflags & DEBUGGER_NEEDNOTIFICATION; }	
	

    // ------------------------------------------------------------
    // Other public APIs
    // ------------------------------------------------------------

    BOOL IsIntrospectionOnly();

#ifndef DACCESS_COMPILE
    BOOL Equals(DomainFile *pFile) { WRAPPER_CONTRACT; return GetFile()->Equals(pFile->GetFile()); }
    BOOL Equals(PEFile *pFile) { WRAPPER_CONTRACT; return GetFile()->Equals(pFile); }
#endif // DACCESS_COMPILE

    Module* GetCurrentModule();
    Module* GetLoadedModule();
    Module* GetModule();

    // The format string is intentionally unicode to avoid globalization bugs
    void ExternalLog(DWORD level, const WCHAR *fmt, ...);
    void ExternalLog(DWORD level, const char *msg);

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

#ifndef DACCESS_COMPILE
    // light code gen. Keep the list of MethodTables needed for creating dynamic methods
    DynamicMethodTable* GetDynamicMethodTable();
#endif

 protected:
    // ------------------------------------------------------------
    // Loader API
    // ------------------------------------------------------------

    friend class AppDomain;
    friend class Assembly;
    friend class Module;
    friend class FileLoadLock;

    DomainFile(AppDomain *pDomain, PEFile *pFile);

    BOOL DoIncrementalLoad(FileLoadLevel targetLevel);
    void ClearLoading() { LEAF_CONTRACT; m_loading = FALSE; }
    void SetLoadLevel(FileLoadLevel level) { LEAF_CONTRACT; m_level = level; }

#ifndef DACCESS_COMPILE
    virtual void Begin() = 0;
    virtual void Allocate() = 0;
    void AddDependencies();
    void EagerFixups();
    void PreLoadLibrary();
    void LoadLibrary();
    void PostLoadLibrary();
    virtual void DeliverSyncEvents() = 0;
    virtual void DeliverAsyncEvents() = 0;    
    void FinishLoad();
    void VerifyExecution();
    void Activate();
#endif

    // This is called when a new active dependency is added.
    static BOOL PropagateNewActivation(Module *pModuleFrom, Module *pModuleTo);
    static BOOL PropagateActivationInAppDomain(Module *pModuleFrom, Module *pModuleTo, AppDomain* pDomain);
    // This can be used to verify that no propagation is needed
    static CHECK CheckUnactivatedInAllDomains(Module *pModule);

    // This should be used to permanently set the load to fail. Do not use with transient conditions
    void SetError(Exception *ex);


    void SetProfilerNotified() { LEAF_CONTRACT; m_notifyflags|= PROFILER_NOTIFIED; }
    void SetDebuggerNotified() { LEAF_CONTRACT; m_notifyflags|=DEBUGGER_NOTIFIED; }
    void SetShouldNotifyDebugger() { LEAF_CONTRACT; m_notifyflags|=DEBUGGER_NEEDNOTIFICATION; }
    void UpdatePEFile(PTR_PEFile pFile);
    // ------------------------------------------------------------
    // Instance data
    // ------------------------------------------------------------

    PTR_AppDomain               m_pDomain;
    PTR_PEFile                  m_pFile;
    PTR_PEFile                  m_pOriginalFile;  // keep file alive just in case someone is sitill using it. If this is not NULL then m_pFile contains reused file from the shared assembly   
    PTR_Module                  m_pModule;
    FileLoadLevel               m_level;
    OBJECTHANDLE                m_hExposedModuleObject;
    class ExInfo
    {
        enum
        {
           ExType_ClrEx,
           ExType_HR
        }
        m_type;
        union
        {
            Exception                   *m_pEx;
            HRESULT                     m_hr;
        };
 
        public:
        void Throw()
        {
            if (m_type==ExType_ClrEx)
                PAL_CPP_THROW(Exception *, m_pEx->DomainBoundClone());
            if (m_type==ExType_HR)
                ThrowHR(m_hr);
            _ASSERTE(!"Bad exception type");
            ThrowHR(E_UNEXPECTED);        
        };
        ExInfo(Exception* pEx)
        {
            LEAF_CONTRACT;
            m_type=ExType_ClrEx;
            m_pEx=pEx;
        };
        
        void ConvertToHResult()
        {
            LEAF_CONTRACT;
            if(m_type==ExType_HR)
                return;
            _ASSERTE(m_type==ExType_ClrEx);
            HRESULT hr=m_pEx->GetHR();
            delete m_pEx;
            m_hr=hr;
            m_type=ExType_HR;
        };
        ~ExInfo()
        {
            LEAF_CONTRACT;
            if (m_type==ExType_ClrEx)
                delete m_pEx;
        }
    }* m_pError;
    
    void ReleaseManagedData()
    {
        if (m_pError)
            m_pError->ConvertToHResult();
    };    



    DWORD                    m_notifyflags;
    BOOL                        m_loading;
    // m_pDynamicMethodTable is used by the light code generation to allow method
    // generation on the fly. They are lazily created when/if a dynamic method is requested
    // for this specific module
    DynamicMethodTable          *m_pDynamicMethodTable;
    class UMThunkHash *m_pUMThunkHash;
    BOOL m_bDisableActivationCheck;
};

// --------------------------------------------------------------------------------
// DomainAssembly is a subclass of DomainFile which specifically represents a assembly.
// --------------------------------------------------------------------------------

class DomainAssembly : public DomainFile
{
    VPTR_VTABLE_CLASS(DomainAssembly, DomainFile);

public:
    // ------------------------------------------------------------
    // Public API
    // ------------------------------------------------------------

    PEAssembly *GetFile()
    {
        WRAPPER_CONTRACT;
        return PTR_PEAssembly(m_pFile);
    }

   IAssemblyBindingClosure* GetAssemblyBindingClosure(BOOL bTakeRef);
   BOOL IsClosedInGAC();

    // Returns security information for the assembly based on the codebase
    void GetSecurityIdentity(SString &codebase, DWORD *pdwZone, BYTE *pbUniqueID, DWORD *pcbUniqueID);

    AssemblySecurityDescriptor* GetSecurityDescriptor()
    {
        LEAF_CONTRACT;
        return static_cast<AssemblySecurityDescriptor*>(m_pSecurityDescriptor);
    }

private:
    enum CMDI_Result
    {
        CMDI_End,
        CMDI_AssemblyResolveSucceeded,
        CMDI_AssemblyResolveFailed
    };

    CMDI_Result CheckMissingDependencyInner(IAssemblyBindingClosure* pClosure, DWORD idx);

public:
    BOOL CheckMissingDependencies();

#ifndef DACCESS_COMPILE
    DomainFile *FindModule(PEFile *pFile, BOOL includeLoading = FALSE);
    DomainModule *FindModule(PEModule *pFile, BOOL includeLoading = FALSE)
    {
        WRAPPER_CONTRACT;
        return (DomainModule *) FindModule((PEFile *) pFile, includeLoading);
    }
    void ReleaseFiles();
#endif // DACCESS_COMPILE

    // Finds only loaded hmods, nothrow
    DomainFile *FindIJWModule(HMODULE hMod);
    // Finds loading hmods, may throw
    DomainFile *FindIJWModule(HMODULE hMod, const SString &path);

    void SetAssembly(Assembly* pAssembly);

    BOOL IsAssembly()
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    OBJECTREF GetExposedAssemblyObject();

    Assembly* GetCurrentAssembly();
    Assembly* GetLoadedAssembly();
    Assembly* GetAssembly();

    // ------------------------------------------------------------
    // Security
    // ------------------------------------------------------------

    BOOL HasSecurityError()
    {
        LEAF_CONTRACT;
        return m_pSecurityError != NULL;
    } 
    void ThrowIfSecurityError()
    {
        WRAPPER_CONTRACT;
        if (HasSecurityError())
        {
            PAL_CPP_THROW(Exception *, m_pSecurityError->DomainBoundClone());
        }
    }

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    // ------------------------------------------------------------
    // Modules
    // ------------------------------------------------------------

    class ModuleIterator
    {
        ArrayList::Iterator m_i;
        BOOL m_includeLoading;

      public:
        BOOL Next()
        {
            WRAPPER_CONTRACT;
            while (m_i.Next())
                if (m_i.GetElement() != NULL
                    && !GetDomainFile()->IsError()
                    && (m_includeLoading || GetDomainFile()->IsLoaded()))
                    return TRUE;
            return FALSE;
        }
        Module *GetModule()
        {
            WRAPPER_CONTRACT;
            return GetDomainFile()->GetModule();
        }
        Module  *GetLoadedModule()
        {
            WRAPPER_CONTRACT;
            return GetDomainFile()->GetLoadedModule();
        }
        DomainFile *GetDomainFile()
        {
            WRAPPER_CONTRACT;
            return PTR_DomainFile((TADDR)m_i.GetElement());
        }
        SIZE_T GetIndex()
        {
            WRAPPER_CONTRACT;
            return m_i.GetIndex();
        }

      private:
        friend class DomainAssembly;
        // Cannot have constructor so this iterator can be used inside a union
        static ModuleIterator Create(DomainAssembly *pDomainAssembly, BOOL includeLoading)
        {
            WRAPPER_CONTRACT;
            ModuleIterator i;

            i.m_i = pDomainAssembly->m_Modules.Iterate();
            i.m_includeLoading = includeLoading;

            return i;
        }
    };
    friend class ModuleIterator;

    ModuleIterator IterateModules(BOOL includeLoading)
    {
        WRAPPER_CONTRACT;
        return ModuleIterator::Create(this, includeLoading);
    }

    DomainFile *LookupDomainFile(DWORD index)
    {
        WRAPPER_CONTRACT;
        if (index >= m_Modules.GetCount())
            return NULL;
        else
            return PTR_DomainFile((TADDR)m_Modules.Get(index));
    }

    Module *LookupModule(DWORD index)
    {
        WRAPPER_CONTRACT;
        DomainFile *pModule = LookupDomainFile(index);
        if (pModule == NULL)
            return NULL;
        else
            return pModule->GetModule();
    }

    void AddModule(DomainModule *pModule);

    // ------------------------------------------------------------
    // Resource access
    // ------------------------------------------------------------

    BOOL GetResource(LPCSTR szName, DWORD *cbResource,
                     PBYTE *pbInMemoryResource, DomainAssembly** pAssemblyRef,
                     LPCSTR *szFileName, DWORD *dwLocation,
                     StackCrawlMark *pStackMark, BOOL fSkipSecurityCheck,
                     BOOL fSkipRaiseResolveEvent);

    BOOL GetModuleResource(mdFile mdResFile, LPCSTR szResName,
                           DWORD *cbResource, PBYTE *pbInMemoryResource,
                           LPCSTR *szFileName, DWORD *dwLocation,
                           BOOL fIsPublic, StackCrawlMark *pStackMark,
                           BOOL fSkipSecurityCheck);


    // ------------------------------------------------------------
    // Debugger control API
    // ------------------------------------------------------------

    DebuggerAssemblyControlFlags GetDebuggerInfoBits(void)
    {
        LEAF_CONTRACT;
        return m_debuggerFlags;
    }

    void SetDebuggerInfoBits(DebuggerAssemblyControlFlags newBits)
    {
        LEAF_CONTRACT;
        m_debuggerFlags = newBits;
    }

    void SetupDebuggingConfig(void);
    DWORD ComputeDebuggingConfig(void);

    bool GetDebuggingOverrides(DWORD *pdwFlags);

    HRESULT GetDebuggingCustomAttributes(DWORD *pdwFlags);

#ifdef DEBUGGING_SUPPORTED
    BOOL NotifyDebuggerLoad(int flags, BOOL attaching);
    void NotifyDebuggerUnload();
#endif // DEBUGGING_SUPPORTED


    // 
    //  GC API
    // 
    void EnumStaticGCRefs(GCEnumCallback  pCallback, LPVOID hCallBack);


 private:

    // ------------------------------------------------------------
    // Loader API
    // ------------------------------------------------------------

    friend class AppDomain;
    friend class Assembly;
    friend class AssemblyNameNative;

#ifndef DACCESS_COMPILE
public:
    ~DomainAssembly();
private:
    DomainAssembly(AppDomain *pDomain, PEFile *pFile, OBJECTREF* pEvidence, OBJECTREF *pExtraEvidence, BOOL fDelayPolicyResolution = FALSE);
#endif

    // ------------------------------------------------------------
    // Internal routines
    // ------------------------------------------------------------

    void SetSecurityError(Exception *ex);

#ifndef DACCESS_COMPILE
    void Begin();
    void Allocate();
    void LoadSharers();
    void DeliverSyncEvents();
    void DeliverAsyncEvents();    
#endif


    BOOL IsInstrumented();


    BOOL ShouldLoadDomainNeutral();
    BOOL ShouldDelayPolicyResolution();

    // ------------------------------------------------------------
    // Instance data
    // ------------------------------------------------------------

  private:
    OBJECTHANDLE                m_hExposedAssemblyObject;
    PTR_AssemblySecurityDescriptor  m_pSecurityDescriptor;
    Exception                   *m_pSecurityError;
    PTR_Assembly                 m_pAssembly;
    DebuggerAssemblyControlFlags m_debuggerFlags;
    IAssemblyBindingClosure* m_pAssemblyBindingClosure;

    ArrayList                   m_Modules;
    BOOL                        m_fDelayPolicyResolution;
};

typedef DomainAssembly::ModuleIterator DomainModuleIterator;

// --------------------------------------------------------------------------------
// DomainModule is a subclass of DomainFile which specifically represents a module.
// --------------------------------------------------------------------------------

class DomainModule : public DomainFile
{
    VPTR_VTABLE_CLASS(DomainModule, DomainFile);

  private:
    PTR_DomainAssembly m_pDomainAssembly;

  public:

    // ------------------------------------------------------------
    // Public API
    // ------------------------------------------------------------

    DomainAssembly *GetDomainAssembly()
    {
        LEAF_CONTRACT;
        return m_pDomainAssembly;
    }

    Module *GetModule()
    {
        LEAF_CONTRACT;
        return m_pModule;
    }

    LPCSTR GetName()
    {
        WRAPPER_CONTRACT;
        return GetFile()->GetSimpleName();
    }

    mdFile GetToken()
    {
        WRAPPER_CONTRACT;
        return GetFile()->GetToken();
    }

    PEModule *GetFile()
    {
        WRAPPER_CONTRACT;
        return PTR_PEModule(m_pFile);
    }

    BOOL IsAssembly()
    {
        LEAF_CONTRACT;
        return FALSE;
    }

    void SetModule(Module *pModule);

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    // ------------------------------------------------------------
    // Loader API
    // ------------------------------------------------------------

    friend class COMModule;

#ifndef DACCESS_COMPILE
    DomainModule(AppDomain *pDomain, DomainAssembly *pAssembly, PEFile *pFile);
    ~DomainModule();
#endif

    // ------------------------------------------------------------
    // Internal routines
    // ------------------------------------------------------------

#ifndef DACCESS_COMPILE
    void Begin();
    void Allocate();
    void LoadSharers();
    void DeliverSyncEvents();
    void DeliverAsyncEvents();    
#endif

};

#endif  // _DOMAINFILE_H_
