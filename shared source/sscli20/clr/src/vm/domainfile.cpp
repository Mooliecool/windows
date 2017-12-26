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
// DomainFile.cpp
// --------------------------------------------------------------------------------

#include <common.h>

// --------------------------------------------------------------------------------
// Headers
// --------------------------------------------------------------------------------

#include <timeline.h>
#include "security.h"
#include "invokeutil.h"
#include "securitydescriptor.h"
#include "eeconfig.h"
#include "dynamicmethod.h"
#include "field.h"
#include "dbginterface.h"
#include "eventtrace.h"


#include "umthunkhash.h"
#include "peimagelayout.inl"


#ifndef DACCESS_COMPILE

DomainFile::DomainFile(AppDomain *pDomain, PEFile *pFile)
  : m_pDomain(pDomain),
    m_pFile(pFile),
    m_pOriginalFile(NULL),
    m_pModule(NULL),
    m_level(FILE_LOAD_CREATE),
    m_pError(NULL),
    m_notifyflags(NOT_NOTIFIED),
    m_loading(TRUE),
    m_pDynamicMethodTable(NULL),
    m_pUMThunkHash(NULL),
    m_bDisableActivationCheck(FALSE)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;  // From CreateHandle
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    m_hExposedModuleObject = NULL;
    pFile->AddRef();
}

DomainFile::~DomainFile()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    m_pFile->Release();
    if(m_pOriginalFile)
        m_pOriginalFile->Release();
    if (m_pDynamicMethodTable) 
        m_pDynamicMethodTable->Destroy();
    delete m_pError;
}

void DomainFile::ReleaseFiles()
{
    WRAPPER_CONTRACT;
#ifndef  DACCESS_COMPILE
    Module* pModule=GetCurrentModule();
    if(pModule)
        pModule->StartUnload();
    if (m_pFile)
        m_pFile->ReleaseIL();
    if(m_pOriginalFile)
        m_pOriginalFile->ReleaseIL();
    if(pModule)
        pModule->ReleaseILData();
#endif

}

BOOL DomainFile::TryEnsureActive()
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    BOOL success = TRUE;

    EX_TRY
      {
          EnsureActive();
      }
    EX_CATCH
      {
          success = FALSE;
      }
    EX_END_CATCH(RethrowTransientExceptions);

    RETURN success;
}

#ifdef _MSC_VER 
#ifndef _DEBUG 
// Optimization intended for EnsureLoadLevel only
#pragma optimize("t", on)
#endif
#endif // _MSC_VER
void DomainFile::EnsureLoadLevel(FileLoadLevel targetLevel)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    TRIGGERSGC ();
    if (IsLoading())
    {
        GetAppDomain()->LoadDomainFile(this, targetLevel);

        // Enforce the loading requirement.  Note that we may have a deadlock in which case we 
        // may be off by one which is OK.  (At this point if we are short of targetLevel we know
        // we have done so because of reentrancy contraints.)

        RequireLoadLevel((FileLoadLevel)(targetLevel-1));
    }
    else
        ThrowIfError(targetLevel);

    RETURN;
}
#ifdef _MSC_VER 
#ifndef _DEBUG 
#pragma optimize("", on)
#endif
#endif // _MSC_VER

void DomainFile::AttemptLoadLevel(FileLoadLevel targetLevel)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    if (IsLoading())
        GetAppDomain()->LoadDomainFile(this, targetLevel);
    else
        ThrowIfError(targetLevel);

    RETURN;
}

CHECK DomainFile::CheckLoadLevel(FileLoadLevel requiredLevel, BOOL deadlockOK)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (deadlockOK)
    {
        CHECK(GetAppDomain()->CheckLoading(this, requiredLevel));
    }
    else
    {
        CHECK_MSG(m_level >= requiredLevel,
                  "File not sufficiently loaded");
    }

    CHECK_OK;
}

void DomainFile::RequireLoadLevel(FileLoadLevel targetLevel)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    if (GetLoadLevel() < targetLevel)
    {
        ThrowIfError(targetLevel);
        ThrowHR(MSEE_E_ASSEMBLYLOADINPROGRESS);
    }

    RETURN;
}


void DomainFile::SetError(Exception *ex)
{
    CONTRACT_VOID
    {
        PRECONDITION(!IsError());
        PRECONDITION(ex != NULL);
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        POSTCONDITION(IsError());
    }
    CONTRACT_END;

    m_pError = new ExInfo(ex->DomainBoundClone());

    if (!IsProfilerNotified())
    {
        SetProfilerNotified();

#ifdef PROFILING_SUPPORTED
        if (GetCurrentModule() != NULL
            && !GetCurrentModule()->GetAssembly()->IsDomainNeutral())
        {
            // Only send errors for non-shared assemblies; other assemblies might be successfully completed
            // in another app domain later.
            GetCurrentModule()->NotifyProfilerLoadFinished(ex->GetHR());
        }
#endif
    }

    RETURN;
}

void DomainFile::ThrowIfError(FileLoadLevel targetLevel)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        MODE_ANY;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    if (m_level < targetLevel)
    {
        if (m_pError)
            m_pError->Throw();
    }

    RETURN;
}

CHECK DomainFile::CheckNoError(FileLoadLevel targetLevel)
{
    CHECK(m_level >= targetLevel
          || !IsError());

    CHECK_OK;
}

CHECK DomainFile::CheckLoaded()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    CHECK_MSG(CheckNoError(FILE_LOADED), "DomainFile load resulted in an error");

    if (IsLoaded())
        CHECK_OK;

    // Mscorlib is allowed to run managed code much earlier than other
    // assemblies for bootstrapping purposes.  This is because it has no
    // dependencies, security checks, and doesn't rely on loader notifications.

    if (GetFile()->IsSystem())
        CHECK_OK;

    CHECK_MSG(GetFile()->CheckLoaded(), "PEFile has not been loaded");

    CHECK_OK;
}

CHECK DomainFile::CheckActivated()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    CHECK_MSG(CheckNoError(FILE_ACTIVE), "DomainFile load resulted in an error");

    if (IsActive())
        CHECK_OK;

    // Mscorlib is allowed to run managed code much earlier than other
    // assemblies for bootstrapping purposes.  This is because it has no
    // dependencies, security checks, and doesn't rely on loader notifications.

    if (GetFile()->IsSystem())
        CHECK_OK;

    CHECK_MSG(GetFile()->CheckLoaded(), "PEFile has not been loaded");
    CHECK_MSG(IsLoaded(), "DomainFile has not been fully loaded");
    CHECK_MSG(m_bDisableActivationCheck || CheckLoadLevel(FILE_ACTIVE), "File has not had execution verified");
    CHECK_MSG(!GetDomainAssembly()->HasSecurityError(), "DomainFile's Assembly did not pass security checks");

    CHECK_OK;
}

DomainAssembly *DomainFile::GetDomainAssembly()
{
    WRAPPER_CONTRACT;
    if (IsAssembly())
    {
        return (DomainAssembly *) this;
    }
    else
    {
        return ((DomainModule *) this)->GetDomainAssembly();
    }
}

#endif // #ifndef DACCESS_COMPILE

BOOL DomainFile::IsIntrospectionOnly()
{
    WRAPPER_CONTRACT;
    return GetFile()->IsIntrospectionOnly();
}



#ifndef DACCESS_COMPILE

void DomainFile::ExternalLog(DWORD level, const WCHAR *fmt, ...)
{
    WRAPPER_CONTRACT;

    va_list args;
    va_start(args, fmt);

    GetFile()->ExternalVLog(level, fmt, args);

    va_end(args);
}

void DomainFile::ExternalLog(DWORD level, const char *msg)
{
    WRAPPER_CONTRACT;

    GetFile()->ExternalLog(level, msg);
}

OBJECTREF DomainFile::GetExposedModuleObjectAsModule()
{
    CONTRACT(OBJECTREF)
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        POSTCONDITION(RETVAL != NULL);
    }
    CONTRACT_END;

    if (m_hExposedModuleObject == NULL)
        m_hExposedModuleObject = m_pDomain->CreateHandle(NULL);
        
    OBJECTREF ref = ObjectFromHandle(m_hExposedModuleObject);
    if (ref == NULL)
    {
        REFLECTMODULEBASEREF  refClass = NULL;

        refClass = (REFLECTMODULEBASEREF) AllocateObject(g_Mscorlib.FetchClass(CLASS__MODULE));
        refClass->SetData(m_pModule);

        ref = (OBJECTREF)refClass;
        StoreFirstObjectInHandle(m_hExposedModuleObject, ref);
    }

    RETURN ref;
}

OBJECTREF DomainFile::GetExposedModuleObject()
{
    CONTRACT(OBJECTREF)
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        POSTCONDITION(RETVAL != NULL);
    }
    CONTRACT_END;

    if (m_hExposedModuleObject == NULL)
        m_hExposedModuleObject = m_pDomain->CreateHandle(NULL);
        
    OBJECTREF ref = ObjectFromHandle(m_hExposedModuleObject);
    if (ref == NULL)
    {
        REFLECTMODULEBASEREF  refClass = NULL;

        if (GetFile()->IsDynamic())
            refClass = (REFLECTMODULEBASEREF) AllocateObject(g_Mscorlib.FetchClass(CLASS__MODULE_BUILDER));
        else
            refClass = (REFLECTMODULEBASEREF) AllocateObject(g_Mscorlib.FetchClass(CLASS__MODULE));
        refClass->SetData(m_pModule);

        ref = (OBJECTREF)refClass;
        StoreFirstObjectInHandle(m_hExposedModuleObject, ref);
    }

    RETURN ref;
}

BOOL DomainFile::DoIncrementalLoad(FileLoadLevel level)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (IsError())
        return FALSE;

    Thread *pThread;
    pThread = GetThread();
    _ASSERTE(pThread);
    INTERIOR_STACK_PROBE_FOR(pThread, 8);

    switch (level)
    {
    case FILE_LOAD_BEGIN:
        Begin();
        break;

    case FILE_LOAD_FIND_NATIVE_IMAGE:
        break;

    case FILE_LOAD_VERIFY_NATIVE_IMAGE_DEPENDENCIES:
        break;

    case FILE_LOAD_ALLOCATE:
        Allocate();
        break;

    case FILE_LOAD_ADD_DEPENDENCIES:
        AddDependencies();
        break;

    case FILE_LOAD_PRE_LOADLIBRARY:
        PreLoadLibrary();
        break;

    case FILE_LOAD_LOADLIBRARY:
        LoadLibrary();
        break;

    case FILE_LOAD_POST_LOADLIBRARY:
        PostLoadLibrary();
        break;

    case FILE_LOAD_EAGER_FIXUPS:
        EagerFixups();
        break;

    case FILE_LOAD_DELIVER_EVENTS:
        DeliverSyncEvents();
        break;

    case FILE_LOADED:
        FinishLoad();
        break;

    case FILE_LOAD_VERIFY_EXECUTION:
        VerifyExecution();
        break;

    case FILE_ACTIVE:
        Activate();
        break;

    default:
        UNREACHABLE();
    }

    END_INTERIOR_STACK_PROBE;
    
    return TRUE;
}



void DomainFile::PreLoadLibrary()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Check skip verification for loading if required
    if (!GetFile()->CanLoadLibrary())
    {
        DomainAssembly* pDomainAssembly = GetDomainAssembly();
        if (Security::IsResolved(pDomainAssembly->GetSecurityDescriptor()))
        {
            if (Security::CanSkipVerification(pDomainAssembly))
                GetFile()->SetSkipVerification();
        }
        else
        {
            AppDomain *pAppDomain = GetAppDomain();
            PEFile *pFile = GetFile();
            _ASSERTE(pFile != NULL);
            PEImage *pImage = pFile->GetILimage();
            _ASSERTE(pImage != NULL);
            _ASSERTE(!pImage->IsFile());
            if (pImage->HasV1Metadata())
            {
                // In V1 case, try to derive SkipVerification status from parents
                do {
                    PEAssembly *pAssembly = pFile->GetAssembly();
                    if (!pAssembly)
                        break;
                    pFile = pAssembly->GetCreator();
                    if (pFile != NULL)
                    {
                        pAssembly = pFile->GetAssembly();
                        // Find matching DomainAssembly for the given PEAsssembly
                        // Perf: This does not scale
                        AssemblyIterationFlags flags =
                            (AssemblyIterationFlags) (kIncludeLoaded | kIncludeLoading | kIncludeExecution);
                        AppDomain::AssemblyIterator i = pAppDomain->IterateAssembliesEx(flags);
                        DomainAssembly *pDomainAssembly = NULL;
                        while (i.Next())
                        {
                            DomainAssembly *pIterDomainAssembly = i.GetDomainAssembly();
                            if (pIterDomainAssembly && pIterDomainAssembly->GetFile() == pAssembly)
                            {
                                pDomainAssembly = pIterDomainAssembly;
                                break;
                            }
                        }
                        if (pDomainAssembly != NULL)
                        {
                            if (Security::IsResolved(pDomainAssembly->GetSecurityDescriptor()))
                            {
                                if (Security::CanSkipVerification(pDomainAssembly))
                                {
                                    GetFile()->SetSkipVerification();
                                    break;
                                }
                            }
                        }
                        else
                        {
                            // Potential Bug: Unable to find DomainAssembly for given PEAssembly
                            // In retail build gracefully exit loop
                            _ASSERTE(pDomainAssembly != NULL);
                            break;
                        }
                    }
                } while (pFile != NULL);
            }
        }
    }
}

// Note that this is the sole loading function which must be called OUTSIDE THE LOCK, since
// it will potentially involve the OS loader lock.
void DomainFile::LoadLibrary()
    {
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    Thread::LoadingFileHolder holder(GetThread());
    GetThread()->SetLoadingFile(this);
    GetFile()->LoadLibrary();
}

void DomainFile::PostLoadLibrary()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        // Note that GetFile()->LoadLibrary must be called before this OUTSIDE OF THE LOCKS
        PRECONDITION(GetFile()->CheckLoaded());
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

#ifdef PROFILING_SUPPORTED
        // After this point, it is possible to load types.
        // We need to call this now because the profiler may need to inject methods into
        // the module, and to do so reliable, it must have the chance to do so before
        // any types are loaded from the module.
        GetCurrentModule()->NotifyProfilerLoadFinished(S_OK);
#endif


}

void DomainFile::AddDependencies()
{
    
}


void DomainFile::EagerFixups()
{
    WRAPPER_CONTRACT;
}

void DomainFile::FinishLoad()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    // Flush any log messages
    GetFile()->FlushExternalLog();

    // Must set this a bit prematurely for the DAC stuff to work
    m_level = FILE_LOADED;

    // Now the DAC can find this module by enumerating assemblies in a domain.
    DACNotify::DoModuleLoadNotification(m_pModule);
}

void DomainFile::VerifyExecution()
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(IsLoaded());
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACT_END;

    if (GetModule()->IsIntrospectionOnly())
    {
        // Throw an exception
        COMPlusThrow(kInvalidOperationException, IDS_EE_CODEEXECUTION_IN_INTROSPECTIVE_ASSEMBLY);
    }

    // Throw a security exception if relevant
    GetDomainAssembly()->ThrowIfSecurityError();

    if(GetFile()->PassiveDomainOnly())
    {
    // Remove path - location must be hidden for security purposes
        LPCWSTR path=GetFile()->GetPath();
        LPCWSTR pStart = wcsrchr(path, '\\');
        if (pStart != NULL)
            pStart++;
        else
            pStart = path;    
        COMPlusThrow(kInvalidOperationException, IDS_EE_CODEEXECUTION_ASSEMBLY_FOR_PASSIVE_DOMAIN_ONLY,pStart);
    }
    
    RETURN;
}

void DomainFile::Activate()
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(IsLoaded());
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACT_END;

    // If we are a module, ensure we've activated the assembly first.

    if (!IsAssembly())
    {
        GetDomainAssembly()->EnsureActive();
    }
    else
    {
        // We cannot execute any code in this assembly until we know what exception plan it is on.
        // At the point of an exception's stack-crawl it is too late because we cannot tolerate a GC.
        // See PossiblyUnwrapThrowable and its callers.
        GetDomainAssembly()->GetLoadedAssembly()->IsRuntimeWrapExceptions();
    }
    
    // Now activate any dependencies.
    // This will typically cause reentrancy of course.

    // increment the counter (see the comment in Module::AddActiveDependency)
    GetModule()->IncrementNumberOfActivations();

    AppDomain *pDomain = GetAppDomain();
    
    Module::DependencyIterator i = GetCurrentModule()->IterateActiveDependencies();
    STRESS_LOG2(LF_CLASSLOADER, LL_INFO100,"Activating module %08x in AD %i",GetCurrentModule(),pDomain->GetId().m_dwId);

    while (i.Next())
    {
        Module *pModule = i.GetDependency();
        DomainFile *pDomainFile = pModule->FindDomainFile(pDomain);
        if (pDomainFile == NULL)
            pDomainFile = pDomain->LoadDomainNeutralModuleDependency(pModule, FILE_LOADED);

        STRESS_LOG3(LF_CLASSLOADER, LL_INFO100,"Acitvating dependency %08x -> %08x, unconditional=%i",GetCurrentModule(),pModule,i.IsUnconditional());

        if (i.IsUnconditional())
        {
            // Let any failures propagate
            pDomainFile->EnsureActive();
        }
        else
        {
            // Enable triggers if we fail here
            if (!pDomainFile->TryEnsureActive())
                GetCurrentModule()->EnableModuleFailureTriggers(pModule, GetAppDomain());
        }
        STRESS_LOG3(LF_CLASSLOADER, LL_INFO100,"Acitvated dependency %08x -> %08x, unconditional=%i",GetCurrentModule(),pModule,i.IsUnconditional());        
    }

    if (m_pModule->CanExecuteCode())
    {
        //
        // Now call the module constructor.  Note that this might cause reentrancy;
        // this is fine and will be handled by the class cctor mechanism.
        //

        MethodTable *pMT = m_pModule->GetGlobalMethodTable();
        if (pMT != NULL)
        {
            pMT->CheckRestore();
            m_bDisableActivationCheck=TRUE;
            pMT->CheckRunClassInitThrowing();
        }
    }
    else
    {
        // This exception does not need to be localized as it can only happen in
        // NGen and PEVerify, and we are not localizing those tools.
        _ASSERTE(GetAppDomain()->IsPassiveDomain());

        LPCWSTR message = L"You may be trying to evaluate a permission from an assembly "
                          L"without FullTrust, or which cannot execute code for other reasons.";
        COMPlusThrowNonLocalized(kFileLoadException, message);
    }
    
    RETURN;
}


BOOL DomainFile::PropagateActivationInAppDomain(Module *pModuleFrom, Module *pModuleTo, AppDomain* pDomain)
{
    CONTRACTL
    {
        PRECONDITION(CheckPointer(pModuleFrom));
        PRECONDITION(CheckPointer(pModuleTo));
        THROWS; // should only throw transient failures
        DISABLED(GC_TRIGGERS);
        MODE_ANY;
    }
    CONTRACTL_END;

    BOOL completed=true;
    EX_TRY
    {
        ENTER_DOMAIN_PTR(pDomain,ADV_ITERATOR); //iterator
        DomainFile *pDomainFileFrom = pModuleFrom->FindDomainFile(pDomain);
        if (pDomainFileFrom != NULL && pDomain->IsLoading(pDomainFileFrom, FILE_ACTIVE))
        {
            DomainFile *pDomainFileTo = pModuleTo->FindDomainFile(pDomain);
            if (pDomainFileTo == NULL)
                pDomainFileTo = pDomain->LoadDomainNeutralModuleDependency(pModuleTo, FILE_LOADED);

            if (!pDomainFileTo->TryEnsureActive())
                pModuleFrom->EnableModuleFailureTriggers(pModuleTo, pDomain);
            else if (!pDomainFileTo->IsActive())
            {
                // We are in a reentrant case
                completed = FALSE;
            }
        }
        END_DOMAIN_TRANSITION;
    }
    EX_CATCH 
    {
          if (!IsExceptionOfType(kAppDomainUnloadedException, GET_EXCEPTION())) 
            EX_RETHROW;
    }
    EX_END_CATCH(SwallowAllExceptions)
    return completed;
}


// Returns TRUE if activation is completed for all app domains
// static
BOOL DomainFile::PropagateNewActivation(Module *pModuleFrom, Module *pModuleTo)
{
    CONTRACTL
    {
        PRECONDITION(CheckPointer(pModuleFrom));
        PRECONDITION(CheckPointer(pModuleTo));
        THROWS; // should only throw transient failures
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    BOOL completed = TRUE;
    if (pModuleFrom->GetAssembly()->IsDomainNeutral())
    {
        AppDomainIterator ai(TRUE);
        Thread *pThread = GetThread();

        while (ai.Next())
        {
            // This is to minimize the chances of trying to run code in an appdomain that's shutting down.
            if (ai.GetDomain()->CanThreadEnter(pThread))
            {
                completed &= PropagateActivationInAppDomain(pModuleFrom,pModuleTo,ai.GetDomain());
            }
        }
    }
    else
    {
        AppDomain *pDomain = pModuleFrom->GetDomain()->AsAppDomain();
        DomainFile *pDomainFileFrom = pModuleFrom->GetDomainFile(pDomain);
        if (pDomain->IsLoading(pDomainFileFrom, FILE_ACTIVE))
        {
            // The dependency should already be loaded
            DomainFile *pDomainFileTo = pModuleTo->GetDomainFile(pDomain);
            if (!pDomainFileTo->TryEnsureActive())
                pModuleFrom->EnableModuleFailureTriggers(pModuleTo, pDomain);
            else if (!pDomainFileTo->IsActive())
            {
                // Reentrant case
                completed = FALSE;
            }
        }
    }

    return completed;
}

// Checks that module has not been activated in any domain
CHECK DomainFile::CheckUnactivatedInAllDomains(Module *pModule)
{
    CONTRACTL
    {
        PRECONDITION(CheckPointer(pModule));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (pModule->GetAssembly()->IsDomainNeutral())
    {
        AppDomainIterator ai(TRUE);

        while (ai.Next())
        {
            AppDomain *pDomain = ai.GetDomain();
            DomainFile *pDomainFile = pModule->FindDomainFile(pDomain);
            if (pDomainFile != NULL)
                CHECK(!pDomainFile->IsActive());
        }
    }
    else
    {
        DomainFile *pDomainFile = pModule->FindDomainFile(pModule->GetDomain()->AsAppDomain());
        if (pDomainFile != NULL)
            CHECK(!pDomainFile->IsActive());
    }
    
    CHECK_OK;
}

//--------------------------------------------------------------------------------
// DomainAssembly
//--------------------------------------------------------------------------------

DomainAssembly::DomainAssembly(AppDomain *pDomain, PEFile *pFile, 
                               OBJECTREF* pEvidence, OBJECTREF *pExtraEvidence,
                               BOOL fDelayPolicyResolution)
  : DomainFile(pDomain, pFile),
    m_pSecurityError(NULL),
    m_pAssembly(NULL),
    m_debuggerFlags(DACF_NONE),
    m_pAssemblyBindingClosure(NULL),
    m_fDelayPolicyResolution(fDelayPolicyResolution)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // !!! backout

    m_hExposedAssemblyObject = NULL;

    NewHolder<AssemblySecurityDescriptor> pSecurityDescriptorHolder(new AssemblySecurityDescriptor(pDomain, this));

    if ((pEvidence != NULL && (*pEvidence) != NULL) 
        || (pExtraEvidence != NULL && (*pExtraEvidence != NULL)))
    {
        if (GetFile()->IsSourceGAC())
        {
            // Assemblies in the GAC are not allowed to 
            // specify additional evidence.  They must always follow default machine policy rules.

            // So, we just ignore the evidence. (Ideally we would throw an error, but it would introduce app 
            // compat issues.)
        }
        else
        {
            // We do not support sharing behavior of ALWAYS when using evidence to load assemblies
            if (pDomain->GetSharePolicy() == AppDomain::SHARE_POLICY_ALWAYS
                && ShouldLoadDomainNeutral())
            {
                // We may not be able to reduce sharing policy at this point, if we have already loaded
                // some non-GAC assemblies as domain neutral.  For this case we must regrettably fail
                // the whole operation.
                if (!pDomain->ReduceSharePolicyFromAlways())
                    ThrowHR(COR_E_CANNOT_SPECIFY_EVIDENCE);
            }

    if (pExtraEvidence != NULL)
    {
        GCX_COOP();
        if(*pExtraEvidence != NULL)
                    Security::SetAdditionalEvidence(pSecurityDescriptorHolder, *pExtraEvidence);
    }
    else if (pEvidence != NULL)
    {
        GCX_COOP();
        if (*pEvidence != NULL)
                    Security::SetEvidence(pSecurityDescriptorHolder, *pEvidence);
            }
        }
    }

    SetupDebuggingConfig();

    // Add a Module iterator entry for this assembly.
    IfFailThrow(m_Modules.Append(this));
    
    m_pSecurityDescriptor = pSecurityDescriptorHolder.Extract();
}



DomainAssembly::~DomainAssembly()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if(m_pAssemblyBindingClosure)
        m_pAssemblyBindingClosure->Release();

    ModuleIterator i = IterateModules(TRUE);
    while (i.Next())
    {
        if (i.GetDomainFile() != this)
            delete i.GetDomainFile();
    }

    if (m_pAssembly != NULL)
    {
        if (m_pAssembly->IsDomainNeutral())
            m_pAssembly->DecrementShareCount();
        else
            delete m_pAssembly;
    }

    delete m_pSecurityDescriptor;

    delete m_pSecurityError;
}

void DomainAssembly::ReleaseFiles()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if(m_pAssembly)
        m_pAssembly->StartUnload();

    if(m_pAssemblyBindingClosure)
    {
        m_pAssemblyBindingClosure->Release();
        m_pAssemblyBindingClosure=NULL;
    }
    
    ModuleIterator i = IterateModules(TRUE);
    while (i.Next())
    {
        if (i.GetDomainFile() != this)
             i.GetDomainFile()->ReleaseFiles();
    }
    DomainFile::ReleaseFiles();   
}

void DomainAssembly::SetAssembly(Assembly* pAssembly)
{
    WRAPPER_CONTRACT;

    UpdatePEFile(pAssembly->GetManifestFile());
    _ASSERTE(pAssembly->GetManifestModule()->GetFile()==m_pFile);
    m_pAssembly = pAssembly;
    m_pModule = pAssembly->GetManifestModule();

    pAssembly->SetDomainAssembly(this);
}

void DomainAssembly::AddModule(DomainModule *pModule)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    DWORD index = RidFromToken(pModule->GetToken());

    while (index >= m_Modules.GetCount())
        IfFailThrow(m_Modules.Append(NULL));

    m_Modules.Set(index, pModule);
}

OBJECTREF DomainAssembly::GetExposedAssemblyObject()
{
    CONTRACT(OBJECTREF)
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        POSTCONDITION(RETVAL != NULL);
    }
    CONTRACT_END;

    if (m_hExposedAssemblyObject == NULL)
        m_hExposedAssemblyObject = m_pDomain->CreateHandle(NULL);
        
    OBJECTREF ref = ObjectFromHandle(m_hExposedAssemblyObject);
    if (ref == NULL)
    {
        MethodTable *pMT;
        if (GetFile()->IsDynamic())
            pMT = g_Mscorlib.GetClass(CLASS__ASSEMBLY_BUILDER);
        else
            pMT = g_Mscorlib.GetClass(CLASS__ASSEMBLY);

        // Create the assembly object
        ASSEMBLYREF obj = (ASSEMBLYREF) AllocateObject(pMT);
        obj->SetAssembly(this);

        ref = (OBJECTREF)obj;
        StoreFirstObjectInHandle(m_hExposedAssemblyObject, ref);
    }

    RETURN ref;
}

void DomainAssembly::SetSecurityError(Exception *ex)
{
    CONTRACT_VOID
    {
        PRECONDITION(CheckPointer(ex));
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        POSTCONDITION(HasSecurityError());
    }
    CONTRACT_END;

    m_pSecurityError = ex->DomainBoundClone();

    RETURN;
}

// This inner method exists to avoid EX_TRY calling _alloca repeatedly in the for loop below.
DomainAssembly::CMDI_Result DomainAssembly::CheckMissingDependencyInner(IAssemblyBindingClosure* pClosure, DWORD idx)
{
    CONTRACTL {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    SafeComHolder<IAssemblyName>  pAssemblyName;
    HRESULT hrBindFailure = S_OK;
    HRESULT hr = pClosure->GetNextFailureAssembly(idx, &pAssemblyName, &hrBindFailure);
    if (hr == HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS))
    {
        return CMDI_End;
    }

    IfFailThrow(hr);

    CMDI_Result ret = CMDI_AssemblyResolveFailed;
    AssemblySpec spec;
    PEAssemblyHolder result;

    EX_TRY
    {
        spec.InitializeSpec(pAssemblyName, GetFile(), FALSE);
        result = GetAppDomain()->TryResolveAssembly(&spec,FALSE);

        if (result)
        {
            GetAppDomain()->AddFileToCache(&spec, result);
            ret = CMDI_AssemblyResolveSucceeded;
        }
        else
        {
            _ASSERTE(FAILED(hrBindFailure));

            StackSString name;
            spec.GetFileOrDisplayName(0, name);            
            NewHolder<EEFileLoadException> pEx(new EEFileLoadException(name, hrBindFailure));
            GetAppDomain()->AddExceptionToCache(&spec, pEx);
        }
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(RethrowTransientExceptions);

    return ret;
}

// CheckMissingDependencies returns FALSE if any missing dependency would
// successfully bind with an AssemblyResolve event. When this is the case, we
// want to avoid sharing this assembly, since AssemblyResolve events are not
// under our control, and therefore not predictable.
BOOL DomainAssembly::CheckMissingDependencies()
{
    CONTRACTL {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    if (GetAppDomain()->IsCompilationDomain())
    {
        // Compilation domains will never have resolve events.  Plus, this path
        // will sidestep the compilation domain's bind override, which will make
        // us skip over some dependencies.
        return TRUE;
    }

    if (IsSystem())
    {
        return TRUE;
    }

    GCX_PREEMP();
    IAssemblyBindingClosure* pClosure = GetAssemblyBindingClosure(FALSE);
    if(pClosure == NULL)
    {
        // If the closure is empty, no need to iterate them.
        return TRUE;
    }

    for (DWORD idx = 0;;idx++)
    {
        switch (CheckMissingDependencyInner(pClosure, idx))
        {
          case CMDI_AssemblyResolveSucceeded:
          {
            STRESS_LOG1(LF_CLASSLOADER,LL_INFO100,"Missing dependencies check FAILED, DomainAssembly=%08X",this);
            return FALSE;
            break;
          }

          case CMDI_End:
          {
            STRESS_LOG1(LF_CLASSLOADER,LL_INFO100,"Missing dependencies check SUCCESSFUL, DomainAssembly=%08X",this);
            return TRUE;
            break;
          }

          case CMDI_AssemblyResolveFailed:
          {
            // Don't take any action, just continue the loop.
            break;
          }
        }
    }
}

DomainFile* DomainAssembly::FindModule(PEFile *pFile, BOOL includeLoading)
{
    CONTRACT (DomainFile*)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    ModuleIterator i = IterateModules(includeLoading);
    while (i.Next())
    {
        if (i.GetDomainFile()->Equals(pFile))
            RETURN i.GetDomainFile();
    }
    RETURN NULL;
}

DomainFile* DomainAssembly::FindIJWModule(HMODULE hMod)
{
    CONTRACT (DomainFile*)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    ModuleIterator i = IterateModules(FALSE);
    while (i.Next())
    {
        PEFile *pFile = i.GetDomainFile()->GetFile();

        if (   !pFile->IsResource()
            && !pFile->IsDynamic()
            && !pFile->IsILOnly()
            && pFile->GetIJWBase() == hMod)
        {
            RETURN i.GetDomainFile();
        }
    }
    RETURN NULL;
}

DomainFile* DomainAssembly::FindIJWModule(HMODULE hMod, 
                                          const SString &path)
{
    CONTRACT (DomainFile*)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    ModuleIterator i = IterateModules(FALSE);
    while (i.Next())
    {
        PEFile *pFile = i.GetDomainFile()->GetFile();

        if (   !pFile->IsResource()
            && !pFile->IsDynamic()
            && !pFile->IsILOnly())
        {
            if (pFile->GetIJWBase() == hMod
                || PEImage::PathEquals(pFile->GetPath(), path))
                RETURN i.GetDomainFile();
        }
    }
    RETURN NULL;
}

void DomainAssembly::Begin()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_INTOLERANT;       
    AppDomain::LoadLockHolder lock(m_pDomain);
    m_pDomain->AddAssembly(this);

}


BOOL DomainAssembly::ShouldLoadDomainNeutral()
{
    // Only use domain neutral code for normal assembly loads
    if (GetFile()->GetFusionAssembly() == NULL)
        return FALSE;
    
    // Introspection only does not use domain neutral code
    if (IsIntrospectionOnly())
        return FALSE;
    
    // Don't use domain neutral code for LoadFrom context, as the 
    // requird eager binding interferes with LoadFrom binding semantics
    if (GetFile()->IsContextLoadFrom())
        return FALSE;

    // Check app domain policy...
    if (GetAppDomain()->ApplySharePolicy(this))
    {
        // Can be domain neutral only if we aren't binding any missing dependencies with 
        // the assembly resolve event
        return CheckMissingDependencies();
    }

    return FALSE;
}

BOOL DomainAssembly::ShouldDelayPolicyResolution()
{
    LEAF_CONTRACT;
    return m_fDelayPolicyResolution;
}

void DomainAssembly::Allocate()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_INTOLERANT;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // We can now rely on the fact that our MDImport will not change so we can stop refcounting it.
    GetFile()->MakeMDImportPersistent();

    AllocMemTracker amTracker;
    AllocMemTracker *pamTracker = &amTracker;

    Assembly *pAssembly=NULL;

    if (m_pAssembly==NULL)
    {
        //! If you decide to remove "if" do not remove this brace: order is important here - in the case of an exception, 
        //! the Assembly holder must destruct before the AllocMemTracker declared above.

        NewHolder<Assembly> assemblyHolder(NULL);
    
        // Determine whether we are supposed to load the assembly as a shared
        // assembly or into the app domain.

        if (ShouldLoadDomainNeutral())
        {
            // Try to find an existing shared version of the assembly which
            // is compatible with our domain.

            SharedDomain *pSharedDomain = SharedDomain::GetDomain();
            SharedFileLockHolder pFileLock(pSharedDomain, GetFile(), FALSE);
    
            TIMELINE_START(LOADER, ("FindShareableAssembly %s", GetSimpleName()));
            SharedAssemblyLocator locator(this);
            pAssembly = pSharedDomain->FindShareableAssembly(&locator);
    
            TIMELINE_END(LOADER, ("FindShareableAssembly %s", GetSimpleName()));
    
            if (pAssembly == NULL)
            {
                pFileLock.Acquire();
                pAssembly = pSharedDomain->FindShareableAssembly(&locator);
            }
    
            if (pAssembly == NULL)
            {
                // Go ahead and create new shared version of the assembly if possible
                assemblyHolder = pAssembly = Assembly::Create(pSharedDomain, GetFile(), GetDebuggerInfoBits(), pamTracker);
                // Compute the closure assembly dependencies
                // of the code & layout of given assembly.
                //
                // An assembly has direct dependencies listed in its manifest.
                //
                // We do not in general also have all of those dependencies' dependencies in the manifest.
                // After all, we may be only using a small portion of the assembly.
                //
                // However, since all dependent assemblies must also be shared (so that
                // the shared data in this assembly can refer to it), we are in
                // effect forced to behave as though we do have all of their dependencies.
                // This is because the resulting shared assembly that we will depend on
                // DOES have those dependencies, but we won't be able to validly share that
                // assembly unless we match all of ITS dependencies, too.
                if (GetAppDomain()->GetFusionContext()!=NULL && !IsSystem())
                {
                    pAssembly->SetSharingProperties(GetAssemblyBindingClosure(FALSE));
                }
                else
                {
                    _ASSERTE(IsSystem());
                    pAssembly->SetSharingProperties(NULL);
                }

                // Sets the tenured bit atomically with the hash insert.
                pSharedDomain->AddShareableAssembly(pAssembly);
            }
        }
        else
        {
            assemblyHolder = pAssembly = Assembly::Create(m_pDomain, GetFile(), GetDebuggerInfoBits(), pamTracker);
            assemblyHolder->SetIsTenured();
        }


        pamTracker->SuppressRelease();
        assemblyHolder.SuppressRelease();
    }
    else
        pAssembly = m_pAssembly;

    SetAssembly(pAssembly);
}


void DomainAssembly::DeliverAsyncEvents()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    OVERRIDE_LOAD_LEVEL_LIMIT(FILE_ACTIVE);
    m_pDomain->RaiseLoadingAssemblyEvent(this);
    
}


void DomainAssembly::DeliverSyncEvents()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_INTOLERANT;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // We may be notified from inside the loader lock if we are delivering IJW events, so keep track.
#ifdef PROFILING_SUPPORTED
    if (!IsProfilerNotified())
    {
        SetProfilerNotified();
        GetCurrentModule()->NotifyProfilerLoadFinished(S_OK);
    }
         
#endif
#ifdef DEBUGGING_SUPPORTED
    GCX_COOP();    
    if(!IsDebuggerNotified())
    {
        SetShouldNotifyDebugger();
      
        if (m_pDomain->IsDebuggerAttached())
        {
            // If this is the first assembly in the AppDomain, it may be possible to get a better name than the
            // default.
            if (m_pDomain->m_Assemblies.Get(0) == this && !m_pDomain->IsUserCreatedDomain())
                m_pDomain->ResetFriendlyName();
            NotifyDebuggerLoad(ATTACH_ASSEMBLY_LOAD, FALSE);
        }
    }
#endif

}

/*
  // The enum for dwLocation from managed code:
    public enum ResourceLocation
    {
        Embedded = 1,
        ContainedInAnotherAssembly = 2,
        ContainedInManifestFile = 4
    }
*/

BOOL DomainAssembly::GetResource(LPCSTR szName, DWORD *cbResource,
                                 PBYTE *pbInMemoryResource, DomainAssembly** pAssemblyRef,
                                 LPCSTR *szFileName, DWORD *dwLocation,
                                 StackCrawlMark *pStackMark, BOOL fSkipSecurityCheck,
                                 BOOL fSkipRaiseResolveEvent)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    return GetFile()->GetResource( szName,
                                   cbResource,
                                   pbInMemoryResource,
                                   pAssemblyRef,
                                   szFileName,
                                   dwLocation,
                                   pStackMark,
                                   fSkipSecurityCheck,
                                   fSkipRaiseResolveEvent,
                                   this,
                                   this->m_pDomain );
}


BOOL DomainAssembly::GetModuleResource(mdFile mdResFile, LPCSTR szResName,
                                       DWORD *cbResource, PBYTE *pbInMemoryResource,
                                       LPCSTR *szFileName, DWORD *dwLocation,
                                       BOOL fIsPublic, StackCrawlMark *pStackMark,
                                       BOOL fSkipSecurityCheck)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    const char     *szName;
    DWORD           dwFlags;
    DomainFile     *pModule = NULL;
    DWORD           dwOffset = 0;

    if (! ((TypeFromToken(mdResFile) == mdtFile) &&
           GetMDImport()->IsValidToken(mdResFile)) ) {
        ThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_FILE_TOKEN);
    }

    GetMDImport()->GetFileProps(mdResFile,
                                &szName,
                                NULL,
                                NULL,
                                &dwFlags);

    if (IsFfContainsMetaData(dwFlags)) {
        // The resource is embedded in a manifest-containing file.
        mdManifestResource mdResource;
        mdToken mdLinkRef;
        DWORD dwResourceFlags;

        Module *pContainerModule = GetCurrentModule();
        // Use the real assembly with a rid map if possible
        if (pContainerModule != NULL)
            pModule = pContainerModule->LoadModule(m_pDomain, mdResFile, FALSE);
        else
        {
            PEModuleHolder pFile(GetAssembly()->LoadModule(mdResFile, FALSE));
            pModule = m_pDomain->LoadDomainModule(this, pFile, FILE_LOADED);
        }

        if (FAILED(pModule->GetMDImport()->FindManifestResourceByName(szResName,
                                                                      &mdResource)))
            return FALSE;

        pModule->GetMDImport()->GetManifestResourceProps(mdResource,
                                                         NULL, //&szName,
                                                         &mdLinkRef,
                                                         &dwOffset,
                                                         &dwResourceFlags);
        _ASSERTE(mdLinkRef == mdFileNil);
        if (mdLinkRef != mdFileNil) {
            ThrowHR(COR_E_BADIMAGEFORMAT, BFA_CANT_GET_LINKREF);
        }
        fIsPublic = IsMrPublic(dwResourceFlags);
    }

    if (!fIsPublic && pStackMark && !fSkipSecurityCheck) {
        Assembly *pCallersAssembly = SystemDomain::GetCallersAssembly(pStackMark);
        if (pCallersAssembly && // full trust for interop
            (!pCallersAssembly->GetManifestFile()->Equals(GetFile()))) {

            RefSecContext sCtx;
            if (!sCtx.DemandMemberAccess(REFSEC_CHECK_MEMBERACCESS))
                return FALSE;
        }
    }

    if (IsFfContainsMetaData(dwFlags)) {
        if (dwLocation) {
            *dwLocation = *dwLocation | 1; // ResourceLocation.embedded
            *szFileName = szName;
            return TRUE;
        }

        pModule->GetFile()->GetEmbeddedResource(dwOffset, cbResource,
                                                pbInMemoryResource);

        return TRUE;
    }

    // The resource is linked (it's in its own file)
    if (szFileName) {
        *szFileName = szName;
        return TRUE;
    }

    Module *pContainerModule = GetCurrentModule();

    // Use the real assembly with a rid map if possible
    if (pContainerModule != NULL)
        pModule = pContainerModule->LoadModule(m_pDomain, mdResFile);
    else
    {
        PEModuleHolder pFile(GetAssembly()->LoadModule(mdResFile));
        pModule = m_pDomain->LoadDomainModule(this, pFile, FILE_LOADED);
    }

    COUNT_T size;
    const void *contents = pModule->GetFile()->GetManagedFileContents(&size);

    *pbInMemoryResource = (BYTE *) contents;
    *cbResource = size;

    return TRUE;
}



// Returns security information for the assembly based on the codebase
void DomainAssembly::GetSecurityIdentity(SString &codebase, DWORD *pdwZone, BYTE *pbUniqueID, DWORD *pcbUniqueID)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pdwZone));
        PRECONDITION(CheckPointer(pbUniqueID));
        PRECONDITION(CheckPointer(pcbUniqueID));
    }
    CONTRACTL_END;

    GetFile()->GetSecurityIdentity(codebase, pdwZone, pbUniqueID, pcbUniqueID);
}

IAssemblyBindingClosure* DomainAssembly::GetAssemblyBindingClosure(BOOL bTakeRef)
{
    CONTRACT(IAssemblyBindingClosure *)
    {
        INSTANCE_CHECK;
        POSTCONDITION(CheckPointer(RETVAL,NULL_OK));
        //we could  return NULL instead of asserting but hitting code paths that call this for mscorlib is just wasting of cycles anyhow
        PRECONDITION(!IsSystem());  
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    if (m_pAssemblyBindingClosure == NULL)
    {
        SafeComHolder<IAssemblyBindingClosure> pClosure;
        if(this->GetAppDomain()->GetFusionContext()==NULL)
        {
            _ASSERTE(IsSystem());
            RETURN NULL;
        }

        IUnknown *pUnk;
        if (GetFile()->IsIStream())
            pUnk = GetFile()->GetIHostAssembly();
        else
            pUnk = GetFile()->GetFusionAssembly();
        IfFailThrow(this->GetAppDomain()->GetFusionContext()->GetAssemblyBindingClosure(pUnk, &pClosure));

        if (FastInterlockCompareExchangePointer((volatile PVOID*)&m_pAssemblyBindingClosure, (IAssemblyBindingClosure*)pClosure, NULL)==NULL)
            pClosure.SuppressRelease();
    }
    if(bTakeRef)
        m_pAssemblyBindingClosure->AddRef();
    RETURN m_pAssemblyBindingClosure;
}

BOOL DomainAssembly::IsClosedInGAC()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if(IsSystem())
        return TRUE;

    if (!GetFile()->IsSourceGAC())
        return FALSE;

    IAssemblyBindingClosure* pClosure = GetAssemblyBindingClosure(FALSE);
    if (pClosure==NULL)
        return IsSystem();
    HRESULT hr=pClosure->IsAllAssembliesInGAC();
    IfFailThrow(hr);

    return (hr==S_OK);
}



#define DE_CUSTOM_VALUE_NAMESPACE        "System.Diagnostics"
#define DE_DEBUGGABLE_ATTRIBUTE_NAME     "DebuggableAttribute"

#define DE_INI_FILE_SECTION_NAME          L".NET Framework Debugging Control"
#define DE_INI_FILE_KEY_TRACK_INFO        L"GenerateTrackingInfo"
#define DE_INI_FILE_KEY_ALLOW_JIT_OPTS    L"AllowOptimize"

DWORD DomainAssembly::ComputeDebuggingConfig()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

#ifdef DEBUGGING_SUPPORTED
    DWORD dacfFlags = DACF_ALLOW_JIT_OPTS;

    if (GetDebuggingOverrides(&dacfFlags))
    {
        dacfFlags |= DACF_USER_OVERRIDE;
    }
    else
    {
        IfFailThrow(GetDebuggingCustomAttributes(&dacfFlags));
    }

    return dacfFlags;
#else // !DEBUGGING_SUPPORTED
    return 0;
#endif // DEBUGGING_SUPPORTED
}

void DomainAssembly::SetupDebuggingConfig(void)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

#ifdef DEBUGGING_SUPPORTED
    DWORD dacfFlags = ComputeDebuggingConfig();

    SetDebuggerInfoBits((DebuggerAssemblyControlFlags)dacfFlags);

    LOG((LF_CORDB, LL_INFO10, "Assembly %S: bits=0x%x\n", GetDebugName(), GetDebuggerInfoBits()));
#endif // DEBUGGING_SUPPORTED
}

// The format for the (temporary) .INI file is:

// [.NET Framework Debugging Control]
// GenerateTrackingInfo=<n> where n is 0 or 1
// AllowOptimize=<n> where n is 0 or 1

// Where neither x nor y equal INVALID_INI_INT:
#define INVALID_INI_INT (0xFFFF)

bool DomainAssembly::GetDebuggingOverrides(DWORD *pdwFlags)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

#ifdef DEBUGGING_SUPPORTED
    
    _ASSERTE(pdwFlags);

    WCHAR ConfigString[16];
    BOOL b;

    
    b = PAL_FetchConfigurationStringW(TRUE,
                          L"GenerateTrackingInfo",
                      ConfigString,
                      sizeof(ConfigString)/sizeof(WCHAR));
    if (b && _wtoi(ConfigString))
    {
        *pdwFlags |= DACF_OBSOLETE_TRACK_JIT_INFO;
    }
    else
    {
        *pdwFlags &= (~DACF_OBSOLETE_TRACK_JIT_INFO);
    }

    b = PAL_FetchConfigurationStringW(TRUE,
                                      L"AllowOptimize",
                                      ConfigString,
                                      sizeof(ConfigString)/sizeof(WCHAR));

    if (b) 
    {
        if (_wtoi(ConfigString)) 
        {
            *pdwFlags |= DACF_ALLOW_JIT_OPTS;
        }
        else
        {
            *pdwFlags &= (~DACF_ALLOW_JIT_OPTS);
        }

        return true;
    }

    return (false);


#else  // !DEBUGGING_SUPPORTED
    return false;
#endif // !DEBUGGING_SUPPORTED
}


// For right now, we only check to see if the DebuggableAttribute is present - later may add fields/properties to the
// attributes.
HRESULT DomainAssembly::GetDebuggingCustomAttributes(DWORD *pdwFlags)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pdwFlags));
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;

    {
        ULONG size;
        BYTE *blob;
        mdModule mdMod;
        ReleaseHolder<IMDInternalImport> mdImport(GetFile()->GetMDImportWithRef());
        mdMod = mdImport->GetModuleFromScope();
        mdAssembly asTK = TokenFromRid(mdtAssembly, 1);

        hr = mdImport->GetCustomAttributeByName(asTK,
                                                                DE_CUSTOM_VALUE_NAMESPACE
                                                                NAMESPACE_SEPARATOR_STR
                                                                DE_DEBUGGABLE_ATTRIBUTE_NAME,
                                                                (const void**)&blob,
                                                                &size);

        // If there is no custom value, then there is no entrypoint defined.
        if (!(FAILED(hr) || hr == S_FALSE))
        {
            // We're expecting a 6 or 8 byte blob:
            //
            // 1, 0, enable tracking, disable opts, 0, 0
            if ((size == 6) || (size == 8))
            {
                if (!((blob[0] == 1) && (blob[1] == 0)))
                {
                    BAD_FORMAT_NOTHROW_ASSERT(!"Invalid blob format for custom attribute");
                    return COR_E_BADIMAGEFORMAT;
                }

                if (blob[2] & 0x1)
                {
                    *pdwFlags |= DACF_OBSOLETE_TRACK_JIT_INFO;
                }
                else
                {
                    *pdwFlags &= (~DACF_OBSOLETE_TRACK_JIT_INFO);
                }

                if (blob[2] & 0x2)
                {
                    *pdwFlags |= DACF_IGNORE_PDBS;
                }
                else
                {
                    *pdwFlags &= (~DACF_IGNORE_PDBS);
                }


                // For compatibility, we enable optimizations if the tracking byte is zero,
                // even if disable opts is nonzero
                if (((blob[2] & 0x1) == 0) || (blob[3] == 0))
                {
                    *pdwFlags |= DACF_ALLOW_JIT_OPTS;
                }
                else
                {
                    *pdwFlags &= (~DACF_ALLOW_JIT_OPTS);
                }

                LOG((LF_CORDB, LL_INFO10, "Assembly %S: has %s=%d,%d bits = 0x%x\n", GetDebugName(),
                     DE_DEBUGGABLE_ATTRIBUTE_NAME,
                     blob[2], blob[3], *pdwFlags));
            }
        }
    }

    return hr;
}

#ifdef DEBUGGING_SUPPORTED
BOOL DomainAssembly::NotifyDebuggerLoad(int flags, BOOL attaching)
{
    WRAPPER_CONTRACT;

    BOOL result = FALSE;

    if (IsIntrospectionOnly())
        return FALSE;

    if (!attaching && !GetAppDomain()->IsDebuggerAttached())
        return FALSE;

    if (flags & ATTACH_ASSEMBLY_LOAD)
    {
        if (ShouldNotifyDebugger())
            g_pDebugInterface->LoadAssembly(GetAppDomain(), GetAssembly(), FALSE, attaching);
        result = TRUE;
    }

    DomainModuleIterator i = IterateModules(TRUE);
    while (i.Next())
    {
        if(i.GetDomainFile()->ShouldNotifyDebugger())
            result = i.GetDomainFile()->GetModule()->NotifyDebuggerLoad(GetAppDomain(), flags, attaching) || result;
    }
    if( ShouldNotifyDebugger())
    {
           result|=m_pModule->NotifyDebuggerLoad(m_pDomain, ATTACH_MODULE_LOAD, attaching);
           SetDebuggerNotified();          
    }



    return result;
}

void DomainAssembly::NotifyDebuggerUnload()
{
    WRAPPER_CONTRACT;

    if (IsIntrospectionOnly())
        return;

    if (!GetAppDomain()->IsDebuggerAttached())
        return;

    DomainModuleIterator i = IterateModules(TRUE);
    while (i.Next())
    {
        if(i.GetDomainFile()->IsDebuggerNotified() )
            i.GetDomainFile()->GetModule()->NotifyDebuggerUnload(GetAppDomain());
    }

    g_pDebugInterface->UnloadAssembly(GetAppDomain(), GetAssembly());

}
#endif // DEBUGGING_SUPPORTED

void DomainAssembly::EnumStaticGCRefs(GCEnumCallback  pCallback, LPVOID hCallBack)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    _ASSERTE(GCHeap::IsGCInProgress() &&    
         GCHeap::IsServerHeap()   &&      
         IsGCSpecialThread());     

    DomainModuleIterator i = IterateModules(FALSE);
    while (i.Next())
    {
        DomainFile* pDomainFile = i.GetDomainFile();

        if (pDomainFile->IsActive())
        {   
            // We guarantee that at this point the module has it's DomainLocalModule set up
            // , as we create it while we load the module
            _ASSERTE(pDomainFile->GetLoadedModule()->GetDomainLocalModule(this->GetAppDomain()));
            pDomainFile->GetLoadedModule()->EnumStaticGCRefs(this->GetAppDomain(), pCallback, hCallBack);
        }
        
    }

    RETURN;
}




//--------------------------------------------------------------------------------
// DomainModule
//--------------------------------------------------------------------------------

DomainModule::DomainModule(AppDomain *pDomain, DomainAssembly *pAssembly, PEFile *pFile)
  : DomainFile(pDomain, pFile),
    m_pDomainAssembly(pAssembly)
{
    WRAPPER_CONTRACT;
}

DomainModule::~DomainModule()
{
    WRAPPER_CONTRACT;
}

void DomainModule::SetModule(Module* pModule)
{
    WRAPPER_CONTRACT;
    UpdatePEFile(pModule->GetFile());
    m_pModule = pModule;
    pModule->SetDomainFile(this);
}

void DomainModule::Begin()
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_INTOLERANT;       
    m_pDomainAssembly->AddModule(this);
}



void DomainModule::Allocate()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_INTOLERANT;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // We can now rely on the fact that our MDImport will not change so we can stop refcounting it.
    GetFile()->MakeMDImportPersistent();

    AllocMemTracker amTracker;
    AllocMemTracker *pamTracker = &amTracker;

    Assembly *pAssembly = m_pDomainAssembly->GetCurrentAssembly();
    Module *pModule = NULL;

    if (pAssembly->IsDomainNeutral())
    {
        // For shared assemblies, the module may be already in the assembly list, even
        // though we haven't loaded it here yet.

        pModule = pAssembly->GetManifestModule()->GetModuleIfLoaded(GetToken());
        if (pModule != NULL)
        {
            SetModule(pModule);
            return;
        }
        else
        {
            SharedDomain *pSharedDomain = SharedDomain::GetDomain();

            SharedFileLockHolder pFileLock(pSharedDomain, GetFile());

            pModule = pAssembly->GetManifestModule()->GetModuleIfLoaded(GetToken());
            if (pModule != NULL)
            {
                SetModule(pModule);
                return;
            }
            else
            {
                pModule = Module::Create(pAssembly, GetToken(), m_pFile, pamTracker);

                EX_TRY
                {
                    pAssembly->PrepareModuleForAssembly(pModule, pamTracker);
                    SetModule(pModule);

                }
                EX_HOOK
                {
                    //! It's critical we destruct the manifest Module prior to the AllocMemTracker used to initialize it.
                    //! Otherwise, we will leave dangling pointers inside the Module that Module::Destruct will attempt
                    //! to dereference.
                    pModule->Destruct();
#ifdef _DEBUG
                    pModule = (Module*)(UINT_PTR)0xcccccccc;
#endif
                }
                EX_END_HOOK


                {
                    CANNOTTHROWCOMPLUSEXCEPTION();
                    FAULT_FORBID();
            
                    //Cannot fail after this point.
                    pamTracker->SuppressRelease();
                    pModule->SetIsTenured();

                    pAssembly->PublishModuleIntoAssembly(pModule);



                    return;  // Explicit return to let you know you are NOT welcome to add code after the CANNOTTHROW/FAULT_FORBID expires
                }
            

            }
        }

    }
    else
    {
        pModule = Module::Create(pAssembly, GetToken(), m_pFile, pamTracker);
        EX_TRY
        {
            pAssembly->PrepareModuleForAssembly(pModule, pamTracker);
            SetModule(pModule);
        }
        EX_HOOK
        {
            //! It's critical we destruct the manifest Module prior to the AllocMemTracker used to initialize it.
            //! Otherwise, we will leave dangling pointers inside the Module that Module::Destruct will attempt
            //! to dereference.
            pModule->Destruct();
#ifdef _DEBUG
            pModule = (Module*)(UINT_PTR)0xcccccccc;
#endif
        }
        EX_END_HOOK


        {
            CANNOTTHROWCOMPLUSEXCEPTION();
            FAULT_FORBID();
    
            //Cannot fail after this point.
            pamTracker->SuppressRelease();
            pModule->SetIsTenured();
            pAssembly->PublishModuleIntoAssembly(pModule);


            return;  // Explicit return to let you know you are NOT welcome to add code after the CANNOTTHROW/FAULT_FORBID expires
        }
    
    }


}



void DomainModule::DeliverAsyncEvents()
{
    LEAF_CONTRACT;
    return;
}

void DomainModule::DeliverSyncEvents()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_INTOLERANT;
    }
    CONTRACTL_END;

#ifdef PROFILING_SUPPORTED
    if (!IsProfilerNotified())
    {
        SetProfilerNotified();
        GetCurrentModule()->NotifyProfilerLoadFinished(S_OK);
    }
#endif

#ifdef DEBUGGING_SUPPORTED
    GCX_COOP();    
    if(!IsDebuggerNotified())
    {
        SetShouldNotifyDebugger();
        if (m_pDomain->IsDebuggerAttached())
        {
            m_pModule->NotifyDebuggerLoad(m_pDomain, ATTACH_MODULE_LOAD, FALSE);
            SetDebuggerNotified();
        }
    }
#endif
}

#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void
DomainFile::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_VTHIS();
    if (m_pDomain.IsValid())
    {
        m_pDomain->EnumMemoryRegions(flags, true);
    }
    if (m_pFile.IsValid())
    {
        m_pFile->EnumMemoryRegions(flags);
    }
    if (m_pModule.IsValid())
    {
        m_pModule->EnumMemoryRegions(flags, true);
    }
}

void
DomainAssembly::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DomainFile::EnumMemoryRegions(flags);
    if (m_pAssembly.IsValid())
    {
        m_pAssembly->EnumMemoryRegions(flags);
    }
    m_Modules.EnumMemoryRegions(flags);
}

void
DomainModule::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DomainFile::EnumMemoryRegions(flags);
    if (m_pDomainAssembly.IsValid())
    {
        m_pDomainAssembly->EnumMemoryRegions(flags);
    }
}


#endif // #ifdef DACCESS_COMPILE



