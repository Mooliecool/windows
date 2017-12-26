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
#include "common.h"
#include "appdomain.hpp"
#include "appdomainnative.hpp"
#include "remoting.h"
#include "comstring.h"
#include "security.h"
#include "eeconfig.h"
#include "comsystem.h"
#include "appdomainhelper.h"
#include "appdomain.inl"

//************************************************************************
inline AppDomain *AppDomainNative::ValidateArg(APPDOMAINREF pThis)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    if (pThis == NULL)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    // Should not get here with a Transparent proxy for the this pointer -
    // should have always called through onto the real object
    _ASSERTE(! CRemotingServices::IsTransparentProxy(OBJECTREFToObject(pThis)));

    AppDomain* pDomain = (AppDomain*)pThis->GetDomain();

    if(!pDomain)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    // should not get here with an invalid appdomain. Once unload it, we won't let anyone else
    // in and any threads that are already in will be unwound.
    _ASSERTE(SystemDomain::GetAppDomainAtIndex(pDomain->GetIndex()) != NULL);
    return pDomain;
}

void ReleaseAppDomainDuringCreation(AppDomain* pDomain)
{
    CONTRACTL
    {
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
    }
    CONTRACTL_END;

    if (pDomain->NotReadyForManagedCode())
    {
        pDomain->Release();
    }
    else
    {
        STRESS_LOG2 (LF_APPDOMAIN, LL_INFO100, "Unload domain during creation [%d] %p\n", pDomain->GetId().m_dwId, pDomain);
        SystemDomain::MakeUnloadable(pDomain);
#ifdef _DEBUG
        DWORD hostTestADUnload = g_pConfig->GetHostTestADUnload();
        pDomain->EnableADUnloadWorker(hostTestADUnload != 2?EEPolicy::ADU_Safe:EEPolicy::ADU_Rude);
#else
        pDomain->EnableADUnloadWorker(EEPolicy::ADU_Safe);
#endif
    }
    pDomain->Release();
}

extern int g_fADUnloadWorkerOK;

//************************************************************************
FCIMPL5(Object*, AppDomainNative::CreateDomain, StringObject* strFriendlyNameUNSAFE, Object* appdomainSetupUNSAFE, Object* providedEvidenceUNSAFE, Object* creatorsEvidenceUNSAFE, void* parentSecurityDescriptor)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    struct _gc
    {
        OBJECTREF       retVal;
        STRINGREF       strFriendlyName;
        OBJECTREF       appdomainSetup;
        OBJECTREF       providedEvidence;
        OBJECTREF       creatorsEvidence;
        OBJECTREF       entryPointProxy;
    } gc;

    ZeroMemory(&gc, sizeof(gc));
    gc.strFriendlyName=(STRINGREF)strFriendlyNameUNSAFE;
    gc.appdomainSetup=(OBJECTREF)appdomainSetupUNSAFE;
    gc.providedEvidence=(OBJECTREF)providedEvidenceUNSAFE;
    gc.creatorsEvidence=(OBJECTREF)creatorsEvidenceUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    CreateDomainHelper(&gc.strFriendlyName, &gc.appdomainSetup, &gc.providedEvidence, &gc.creatorsEvidence, parentSecurityDescriptor, &gc.entryPointProxy, &gc.retVal);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(gc.retVal);
}
FCIMPLEND

FCIMPL5(Object*, AppDomainNative::CreateInstance, StringObject* strFriendlyNameUNSAFE, Object* appdomainSetupUNSAFE, Object* providedEvidenceUNSAFE, Object* creatorsEvidenceUNSAFE, void* parentSecurityDescriptor)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    struct _gc
    {
        OBJECTREF       retVal;
        STRINGREF       strFriendlyName;
        OBJECTREF       appdomainSetup;
        OBJECTREF       providedEvidence;
        OBJECTREF       creatorsEvidence;
        OBJECTREF       entryPointProxy;
    } gc;

    ZeroMemory(&gc, sizeof(gc));
    gc.strFriendlyName=(STRINGREF)strFriendlyNameUNSAFE;
    gc.appdomainSetup=(OBJECTREF)appdomainSetupUNSAFE;
    gc.providedEvidence=(OBJECTREF)providedEvidenceUNSAFE;
    gc.creatorsEvidence=(OBJECTREF)creatorsEvidenceUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    CreateDomainHelper(&gc.strFriendlyName, &gc.appdomainSetup, &gc.providedEvidence, &gc.creatorsEvidence, parentSecurityDescriptor, &gc.entryPointProxy, &gc.retVal);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(gc.entryPointProxy);
}
FCIMPLEND

void AppDomainNative::CreateDomainHelper (STRINGREF* ppFriendlyName, OBJECTREF* ppAppdomainSetup, OBJECTREF* ppProvidedEvidence, OBJECTREF* ppCreatorsEvidence, void* parentSecurityDescriptor, OBJECTREF* pEntryPointProxy, OBJECTREF* pRetVal)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        THROWS;
        PRECONDITION(IsProtectedByGCFrame(ppFriendlyName));
        PRECONDITION(IsProtectedByGCFrame(ppAppdomainSetup));
        PRECONDITION(IsProtectedByGCFrame(ppProvidedEvidence));
        PRECONDITION(IsProtectedByGCFrame(ppCreatorsEvidence));
        PRECONDITION(IsProtectedByGCFrame(pEntryPointProxy));
        PRECONDITION(IsProtectedByGCFrame(pRetVal));
    }
    CONTRACTL_END;
    
    if (g_fADUnloadWorkerOK<0)
    {
        GCX_PREEMP();
        AppDomain::CreateADUnloadWorker();
    }

    //@todo: B#25921
    // We addref Appdomain object here and notify a profiler that appdomain 
    // creation has started, then return to managed code which will  call 
    // the function that releases the appdomain and notifies a profiler that we finished
    // creating the appdomain. If an exception is raised while we're in that managed code
    // we will leak memory and the profiler will not be notified about the failure

    // Create the domain adding the appropriate arguments

    Wrapper<AppDomain*,DoNothing<AppDomain*>,ReleaseAppDomainDuringCreation,NULL> pDomain(new AppDomain());

    // During creation of the domain, another thread may unload the domain.
    // We need to keep AppDomain memory around until the end of the function.
    pDomain->AddRef();

#ifdef PROFILING_SUPPORTED
    // Signal profile if present.
    if (CORProfilerTrackAppDomainLoads())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->AppDomainCreationStarted((ThreadID) GetThread(), (AppDomainID) (AppDomain*) pDomain);
    }
    EX_TRY
#endif // PROFILING_SUPPORTED
    {
        {
            SystemDomain::LockHolder lh;
            pDomain->Init(); 
            // allocate a Virtual Call Stub Manager for this domain
            pDomain->InitVirtualCallStubManager();
        }

        pDomain->SetCanUnload();    // by default can unload any domain
        
        STRESS_LOG2 (LF_APPDOMAIN, LL_INFO100, "Create domain [%d] %p\n", pDomain->GetId().m_dwId, (AppDomain*)pDomain);
        pDomain->LoadSystemAssemblies();
        pDomain->SetupSharedStatics();

        MEMORY_REPORT_APP_DOMAIN_SCOPE(pDomain);

        #ifdef DEBUGGING_SUPPORTED    
        // Notify the debugger here, before the thread transitions into the 
        // AD to finish the setup.  If we don't, stepping won't work right                              
        SystemDomain::PublishAppDomainAndInformDebugger(pDomain);
        #endif // DEBUGGING_SUPPORTED

        pDomain->SetStage(AppDomain::STAGE_ACTIVE);
        
        *pRetVal = pDomain->GetAppDomainProxy();

        MethodDescCallSite remotelySetupRemoteDomain(METHOD__APP_DOMAIN__REMOTELY_SETUP_REMOTE_DOMAIN);

        ARG_SLOT args[9];
        args[0]=ObjToArgSlot(*pRetVal);
        args[1]=ObjToArgSlot(*ppFriendlyName);
        args[2]=ObjToArgSlot(*ppAppdomainSetup);
        args[3]=ObjToArgSlot(*ppProvidedEvidence);
        args[4]=ObjToArgSlot(*ppCreatorsEvidence);
        args[5]=PtrToArgSlot(parentSecurityDescriptor);

        *pEntryPointProxy = remotelySetupRemoteDomain.Call_RetOBJECTREF(args);

        pDomain->CacheStringsForDAC();

        pDomain.SuppressRelease();
        pDomain->Release();
   }
#ifdef PROFILING_SUPPORTED
    EX_HOOK
    {
        // Need the first assembly loaded in to get any data on an app domain.
        if (CORProfilerTrackAppDomainLoads())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->AppDomainCreationFinished((ThreadID) GetThread(), (AppDomainID)(AppDomain*) pDomain, GET_EXCEPTION()->GetHR());
        }
    }
    EX_END_HOOK;

    // Need the first assembly loaded in to get any data on an app domain.
    if (CORProfilerTrackAppDomainLoads())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->AppDomainCreationFinished((ThreadID) GetThread(), (AppDomainID)(AppDomain*) pDomain, S_OK);
    }        
#endif // PROFILING_SUPPORTED
}

FCIMPL4(void, AppDomainNative::SetupDomainSecurity, AppDomainBaseObject* refThisUNSAFE, Object* appDomainEvidenceUNSAFE, void* parentSecurityDescriptor, CLR_BOOL fPublishAppDomain)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    struct _gc
    {
        APPDOMAINREF    refThis;
        OBJECTREF       evidence;
    } gc;

    gc.refThis  = (APPDOMAINREF) refThisUNSAFE;
    gc.evidence = (OBJECTREF)    appDomainEvidenceUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_PROTECT(gc)

    // Set up the default AppDomain property.
    AppDomainRefHolder pDomain(ValidateArg(gc.refThis));
    pDomain->AddRef();
    ApplicationSecurityDescriptor *pSecDesc = pDomain->GetSecurityDescriptor();

    if (!Security::IsHomogeneous(pSecDesc)) {
        if (pDomain->IsDefaultDomain()) {
            Security::SetDefaultAppDomainProperty(pSecDesc);
        }
        // Set up the evidence property in the VM side.
        else {
            ApplicationSecurityDescriptor *pCreatorSecDesc = (ApplicationSecurityDescriptor*) parentSecurityDescriptor;

            // If the AppDomain that created this one is a default appdomain and
            // no evidence is provided, then this new AppDomain is also a default appdomain.
            // If there is no provided evidence but the creator is not a default appdomain,
            // then this new appdomain just gets the same evidence as the creator.
            // If evidence is provided, the new appdomain is not a default appdomain and
            // we simply use the provided evidence.

            if (gc.evidence == NULL && Security::IsDefaultAppDomain(pCreatorSecDesc))
                Security::SetDefaultAppDomainProperty(pDomain->GetSecurityDescriptor());
        }
    }

    if (gc.evidence != NULL)
        Security::SetEvidence(pSecDesc, gc.evidence);

    // We need to downgrade sharing level if the AppDomain is homogeneous or 
    // an AppDomainManager provides a custom host policy.
    if (Security::IsHomogeneous(pSecDesc) || Security::CallHostSecurityManager(pSecDesc)) {
        if (pDomain->GetSharePolicy() == AppDomain::SHARE_POLICY_ALWAYS) {
            // We may not be able to reduce sharing policy at this point, if we have already loaded
            // some non-GAC assemblies as domain neutral.  For this case we must regrettably fail
            // the whole operation.
            if (!pDomain->ReduceSharePolicyFromAlways())
                ThrowHR(COR_E_CANNOT_SET_POLICY);
        }
    }

    // Now finish the initialization.
    Security::FinishInitialization(pSecDesc);

    // once domain is loaded it is publically available so if you have anything 
    // that a list interrogator might need access to if it gets a hold of the
    // appdomain, then do it above the LoadDomain.
    if (fPublishAppDomain)
        SystemDomain::LoadDomain(pDomain);

#ifdef _DEBUG
    LOG((LF_APPDOMAIN, LL_INFO100, "AppDomainNative::CreateDomain domain [%d] %p %S\n", pDomain->GetIndex().m_dwIndex, (AppDomain*)pDomain, pDomain->GetFriendlyName()));
#endif

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL2(void, AppDomainNative::SetupFriendlyName, AppDomainBaseObject* refThisUNSAFE, StringObject* strFriendlyNameUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    struct _gc
    {
        APPDOMAINREF    refThis;
        STRINGREF       strFriendlyName;
    } gc;

    gc.refThis          = (APPDOMAINREF) refThisUNSAFE;
    gc.strFriendlyName  = (STRINGREF)    strFriendlyNameUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_PROTECT(gc)

    AppDomainRefHolder pDomain(ValidateArg(gc.refThis));
    pDomain->AddRef();

    // If the user created this domain, need to know this so the debugger doesn't
    // go and reset the friendly name that was provided.
    pDomain->SetIsUserCreatedDomain();

    WCHAR* pFriendlyName = NULL;
    Thread *pThread = GetThread();

    CheckPointHolder cph(pThread->m_MarshalAlloc.GetCheckpoint()); //hold checkpoint for autorelease
    if (gc.strFriendlyName != NULL) {
        WCHAR* pString = NULL;
        int    iString;
        RefInterpretGetStringValuesDangerousForGC(gc.strFriendlyName, &pString, &iString);
        pFriendlyName = (WCHAR*) pThread->m_MarshalAlloc.Alloc((++iString) * sizeof(WCHAR));

        // Check for a valid string allocation
        if (pFriendlyName == (WCHAR*)-1)
           pFriendlyName = NULL;
        else
           memcpy(pFriendlyName, pString, iString*sizeof(WCHAR));
    }

    pDomain->SetFriendlyName(pFriendlyName);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(LPVOID, AppDomainNative::GetFusionContext, AppDomainBaseObject* refThis)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    LPVOID rv = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_1(rv);

    
    AppDomain* pApp = ValidateArg((APPDOMAINREF)refThis);

    rv = pApp->CreateFusionContext();

    HELPER_METHOD_FRAME_END();
    return rv;
}
FCIMPLEND

FCIMPL1(void*, AppDomainNative::GetSecurityDescriptor, AppDomainBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    void*        pvRetVal = NULL;    
    APPDOMAINREF refThis = (APPDOMAINREF) refThisUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(refThis);

    
    pvRetVal = ValidateArg(refThis)->GetSecurityDescriptor();

    HELPER_METHOD_FRAME_END();
    return pvRetVal;
}
FCIMPLEND


FCIMPL2(void, AppDomainNative::UpdateLoaderOptimization, AppDomainBaseObject* refThisUNSAFE, DWORD optimization)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    APPDOMAINREF refThis = (APPDOMAINREF) refThisUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_1(refThis);

    ValidateArg(refThis)->SetSharePolicy((AppDomain::SharePolicy) (optimization & AppDomain::SHARE_POLICY_MASK));

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL3(void, AppDomainNative::UpdateContextProperty, LPVOID fusionContext, StringObject* keyUNSAFE, Object* valueUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    struct _gc
    {
        STRINGREF key;
        OBJECTREF value;
    } gc;

    gc.key   = ObjectToSTRINGREF(keyUNSAFE);
    gc.value = ObjectToOBJECTREF(valueUNSAFE);
    _ASSERTE(gc.key != NULL);

    HELPER_METHOD_FRAME_BEGIN_PROTECT(gc);

    IApplicationContext* pContext = (IApplicationContext*) fusionContext;

    DWORD lgth = gc.key->GetStringLength();
    CQuickBytes qb;
    LPWSTR key = (LPWSTR) qb.AllocThrows((lgth+1)*sizeof(WCHAR));
    memcpy(key, gc.key->GetBuffer(), lgth*sizeof(WCHAR));
    key[lgth] = L'\0';
        
    AppDomain::SetContextProperty(pContext, key, &gc.value);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

HRESULT AppDomainNative::InitializeDomainManagerVariables ()
{
    CONTRACTL {
        MODE_ANY;
        GC_TRIGGERS;
        THROWS;
    }
    CONTRACTL_END;

    
    NewArrayHolder<WCHAR> wszAppDomainManagerAsm(NULL);
    NewArrayHolder<WCHAR> wszAppDomainManagerType(NULL);

    HRESULT hr = S_OK;
    IfFailRet(g_pConfig->GetConfigString(L"APPDOMAIN_MANAGER_ASM", &wszAppDomainManagerAsm, FALSE));
    IfFailRet(g_pConfig->GetConfigString(L"APPDOMAIN_MANAGER_TYPE", &wszAppDomainManagerType, FALSE));

    PVOID pv = NULL;
    if (wszAppDomainManagerAsm) {
        pv = InterlockedCompareExchangePointer((PVOID *) &g_wszAppDomainManagerAsm, (PVOID) wszAppDomainManagerAsm, NULL);
        if (pv == NULL)
            wszAppDomainManagerAsm.SuppressRelease();
    }
    if (wszAppDomainManagerType) {
        pv = InterlockedCompareExchangePointer((PVOID *) &g_wszAppDomainManagerType, (PVOID) wszAppDomainManagerType, NULL);
        if (pv == NULL)
            wszAppDomainManagerType.SuppressRelease();
    }

    g_fDomainManagerInitialized = true;

    // reset the environment variables so child processes
    // do not inherit the same AppDomainManager.
    WszSetEnvironmentVariable(L"APPDOMAIN_MANAGER_ASM", NULL);
    WszSetEnvironmentVariable(L"APPDOMAIN_MANAGER_TYPE", NULL);

    return S_OK;
}

/* static */
INT32 AppDomainNative::ExecuteAssemblyHelper(Assembly* pAssembly,
                                             BOOL bCreatedConsole,
                                             PTRARRAYREF *pStringArgs)
{
    STATIC_CONTRACT_THROWS;

    INT32 iRetVal = 0;



        iRetVal = pAssembly->ExecuteMainMethod(pStringArgs);



    return iRetVal;
}

FCIMPL3(INT32, AppDomainNative::ExecuteAssembly, AppDomainBaseObject* refThisUNSAFE,
    AssemblyBaseObject* assemblyNameUNSAFE, PTRArray* stringArgsUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    INT32 iRetVal = 0;

    struct _gc
    {
        APPDOMAINREF    refThis;
        ASSEMBLYREF     assemblyName;
        PTRARRAYREF     stringArgs;
    } gc;

    gc.refThis      = (APPDOMAINREF) refThisUNSAFE;
    gc.assemblyName = (ASSEMBLYREF)  assemblyNameUNSAFE;
    gc.stringArgs   = (PTRARRAYREF)  stringArgsUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_PROTECT(gc);

    AppDomain* pDomain = ValidateArg(gc.refThis);

    if (gc.assemblyName == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");

    if((BaseDomain*) pDomain == SystemDomain::System()) 
        COMPlusThrow(kUnauthorizedAccessException, L"UnauthorizedAccess_SystemDomain");

    Assembly* pAssembly = (Assembly*) gc.assemblyName->GetAssembly();

    if (!pDomain->m_pRootAssembly)
        pDomain->m_pRootAssembly = pAssembly;

    BOOL bCreatedConsole = FALSE;


    // This helper will call FreeConsole()
    iRetVal = ExecuteAssemblyHelper(pAssembly, bCreatedConsole, &gc.stringArgs);

    HELPER_METHOD_FRAME_END();

    return iRetVal;
}
FCIMPLEND

FCIMPL8(Object*, AppDomainNative::CreateDynamicAssembly, AppDomainBaseObject* refThisUNSAFE, AssemblyNameBaseObject* assemblyNameUNSAFE, Object* identityUNSAFE, StackCrawlMark* stackMark, Object* requiredPsetUNSAFE, Object* optionalPsetUNSAFE, Object* refusedPsetUNSAFE, INT32 access)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    ASSEMBLYREF refRetVal = NULL;

    CreateDynamicAssemblyArgs   args;

    args.refThis        = (APPDOMAINREF)    refThisUNSAFE;
    args.assemblyName   = (ASSEMBLYNAMEREF) assemblyNameUNSAFE;
    args.identity       = (OBJECTREF)       identityUNSAFE;
    args.requiredPset   = (OBJECTREF)       requiredPsetUNSAFE;
    args.optionalPset   = (OBJECTREF)       optionalPsetUNSAFE;
    args.refusedPset    = (OBJECTREF)       refusedPsetUNSAFE;

    args.access         = access;
    args.stackMark      = stackMark;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, (CreateDynamicAssemblyArgsGC&)args);

    AppDomain* pAppDomain = ValidateArg(args.refThis);

    Assembly *pAssembly = Assembly::CreateDynamic(pAppDomain, &args);

    refRetVal = (ASSEMBLYREF) pAssembly->GetExposedObject();

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

// Gets AppDomainManager assembly name. Returns NULL if the host did not specify a custom implementation.
FCIMPL0(StringObject*, AppDomainNative::GetDomainManagerAsm)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    STRINGREF retString = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, retString);

    LPCWSTR wszAppDomainManagerAsm = CorHost2::GetAppDomainManagerAsm();
    if (wszAppDomainManagerAsm) {
        retString = COMString::NewString(wszAppDomainManagerAsm);
    } else {
        // initialize global variables
        if (!g_fDomainManagerInitialized) {
            HRESULT hr = InitializeDomainManagerVariables();
            if (FAILED(hr))
                COMPlusThrowHR(hr);
        }
        if (g_wszAppDomainManagerAsm)
            retString = COMString::NewString(g_wszAppDomainManagerAsm);
    }

    HELPER_METHOD_FRAME_END();
    return (StringObject*) OBJECTREFToObject(retString);
}
FCIMPLEND

// Gets AppDomainManager type name. Returns NULL if the host did not specify a custom implementation.
FCIMPL0(StringObject*, AppDomainNative::GetDomainManagerType)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    STRINGREF retString = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, retString);

    LPCWSTR wszAppDomainManagerType = CorHost2::GetAppDomainManagerType();
    if (wszAppDomainManagerType) {
        retString = COMString::NewString(wszAppDomainManagerType);
    } else {
        // initialize global variables
        if (!g_fDomainManagerInitialized) {
            HRESULT hr = InitializeDomainManagerVariables();
            if (FAILED(hr))
                COMPlusThrowHR(hr);
        }
        if (g_wszAppDomainManagerType)
            retString = COMString::NewString(g_wszAppDomainManagerType);
    }

    HELPER_METHOD_FRAME_END();
    return (StringObject*) OBJECTREFToObject(retString);
}
FCIMPLEND

// registers the current domain's AppDomainManager with the host.
FCIMPL1(void, AppDomainNative::RegisterWithHost, Object* refDomainManagerUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    OBJECTREF refDomainManagerSAFE = (OBJECTREF) refDomainManagerUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(refDomainManagerSAFE);

    HRESULT hr = S_OK;
    IHostControl *pHostControl = CorHost2::GetHostControl();
    if (pHostControl) {
        EnsureComStarted();
        IUnknown* pAppDomainMgr = GetComIPFromObjectRef(&refDomainManagerSAFE);
        ADID dwDomainId = SystemDomain::GetCurrentDomain()->GetId();
        GCX_PREEMP();
        // call the host API to register the AppDomainManager
        BEGIN_SO_TOLERANT_CODE_CALLING_HOST(GetThread());
        hr = pHostControl->SetAppDomainManager(dwDomainId.m_dwId, pAppDomainMgr);
        END_SO_TOLERANT_CODE_CALLING_HOST;
        pAppDomainMgr->Release();
    }
    if (FAILED(hr))
        ThrowHR(hr);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(void, AppDomainNative::SetHostSecurityManagerFlags, DWORD dwFlags);
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    Security::SetHostSecurityManagerFlags(GetThread()->GetDomain()->GetSecurityDescriptor(), dwFlags);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL0(void, AppDomainNative::SetSecurityHomogeneousFlag);
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    ApplicationSecurityDescriptor* pAppSecDesc = GetThread()->GetDomain()->GetSecurityDescriptor();
    Security::SetHomogeneousFlag(pAppSecDesc);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(Object*, AppDomainNative::GetFriendlyName, AppDomainBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    STRINGREF    str     = NULL;
    APPDOMAINREF refThis = (APPDOMAINREF) refThisUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refThis);

    AppDomain* pApp = ValidateArg(refThis);

    LPCWSTR wstr = pApp->GetFriendlyName();
    if (wstr)
        str = COMString::NewString(wstr);   

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(str);
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, AppDomainNative::IsDefaultAppDomainForSecurity, AppDomainBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    AppDomain* pApp = ValidateArg((APPDOMAINREF) refThisUNSAFE);

    FC_RETURN_BOOL(Security::IsDefaultAppDomain(pApp->GetSecurityDescriptor()));
}
FCIMPLEND

FCIMPL2(Object*, AppDomainNative::GetAssemblies, AppDomainBaseObject* refThisUNSAFE, CLR_BOOL forIntrospection);
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    struct _gc
    {
        PTRARRAYREF     AsmArray;
        APPDOMAINREF    refThis;
    } gc;

    gc.AsmArray = NULL;
    gc.refThis  = (APPDOMAINREF) refThisUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    MethodTable *pAssemblyClass = g_Mscorlib.GetClass(CLASS__ASSEMBLY);

    AppDomain* pApp = ValidateArg(gc.refThis);

    // Allocate an array with as many elements as there are assemblies in this
    //  appdomain.  This will usually be correct, but there may be assemblies
    //  that are still loading, and those won't be included in the array of
    //  loaded assemblies.  When that happens, the array will have some trailing
    //  NULL entries; those entries will need to be trimmed.
    size_t nArrayElems = pApp->m_Assemblies.GetCount();
    gc.AsmArray = (PTRARRAYREF) AllocateObjectArray((DWORD) nArrayElems,
                                                 pAssemblyClass);

    // Iterate over the loaded assemblies in the appdomain, and add each one to
    //  to the array.  Quit when the array is full, in case assemblies have been
    //  loaded into this appdomain, on another thread.
    size_t numAssemblies = 0;
    AppDomain::AssemblyIterator i = pApp->IterateAssembliesEx( (AssemblyIterationFlags)(kIncludeLoaded | (forIntrospection ? kIncludeIntrospection : kIncludeExecution)) );
    while (i.Next() && (numAssemblies < nArrayElems))
    {
        // Do not change this code.  This is done this way to
        //  prevent a GC hole in the SetObjectReference() call.  The compiler
        //  is free to pick the order of evaluation.
        OBJECTREF o = (OBJECTREF) i.GetDomainAssembly()->GetExposedAssemblyObject();
        gc.AsmArray->SetAt(numAssemblies++, o);
    }

    // If we didn't fill the array, allocate a new array that is exactly the
    //  right size, and copy the data to it.
    if (numAssemblies < nArrayElems)
    {
        PTRARRAYREF     AsmArray2;
        AsmArray2 = (PTRARRAYREF) AllocateObjectArray((DWORD) numAssemblies,
                                                 pAssemblyClass);

        for (size_t ix=0; ix<numAssemblies; ++ix)
            AsmArray2->SetAt(ix, gc.AsmArray->GetAt(ix));

        gc.AsmArray = AsmArray2;
    }

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(gc.AsmArray);
}
FCIMPLEND


FCIMPL1(void, AppDomainNative::Unload, INT32 dwId)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    IfFailThrow(AppDomain::UnloadById(ADID(dwId),TRUE));

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, AppDomainNative::IsDomainIdValid, INT32 dwId)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    BOOL retVal = FALSE;
    HELPER_METHOD_FRAME_BEGIN_RET_0()

    AppDomainFromIDHolder ad((ADID)dwId, TRUE);
    retVal=!ad.IsUnloaded();
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND

FCIMPL0(Object*, AppDomainNative::GetDefaultDomain)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    APPDOMAINREF rv = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, rv);

    if (GetThread()->GetDomain()->IsDefaultDomain())
        rv = (APPDOMAINREF) SystemDomain::System()->DefaultDomain()->GetExposedObject();
    else
        rv = (APPDOMAINREF) SystemDomain::System()->DefaultDomain()->GetAppDomainProxy();

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(rv);
}
FCIMPLEND

FCIMPL1(INT32, AppDomainNative::GetId, AppDomainBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    INT32        iRetVal = 0;
    APPDOMAINREF refThis = (APPDOMAINREF) refThisUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_1(refThis);

    AppDomain* pApp = ValidateArg(refThis);
    // can only be accessed from within current domain
    _ASSERTE(GetThread()->GetDomain() == pApp);

    iRetVal = pApp->GetId().m_dwId;

    HELPER_METHOD_FRAME_END();
    return iRetVal;
}
FCIMPLEND

FCIMPL1(void, AppDomainNative::ChangeSecurityPolicy, AppDomainBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    APPDOMAINREF refThis = (APPDOMAINREF) refThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(refThis);

    AppDomain* pApp = ValidateArg(refThis);

    // We do not support sharing behavior of ALWAYS when using app-domain local security config
    if (pApp->GetSharePolicy() == AppDomain::SHARE_POLICY_ALWAYS)
    {
        // We may not be able to reduce sharing policy at this point, if we have already loaded
        // some non-GAC assemblies as domain neutral.  For this case we must regrettably fail
        // the whole operation.
        if (!pApp->ReduceSharePolicyFromAlways())
            ThrowHR(COR_E_CANNOT_SET_POLICY);
    }

    Security::SetPolicyLevelFlag(pApp->GetSecurityDescriptor());

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL2(Object*, AppDomainNative::IsStringInterned, AppDomainBaseObject* refThisUNSAFE, StringObject* pStringUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    APPDOMAINREF    refThis     = (APPDOMAINREF)ObjectToOBJECTREF(refThisUNSAFE);
    STRINGREF       refString   = ObjectToSTRINGREF(pStringUNSAFE);
    STRINGREF*      prefRetVal  = NULL;

    ValidateArg(refThis);
    
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, refThis, refString);
    
    if (refString == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_String");

    prefRetVal = refThis->GetDomain()->IsStringInterned(&refString);

    HELPER_METHOD_FRAME_END();

    if (prefRetVal == NULL)
        return NULL;

    return OBJECTREFToObject(*prefRetVal);
}
FCIMPLEND

FCIMPL2(Object*, AppDomainNative::GetOrInternString, AppDomainBaseObject* refThisUNSAFE, StringObject* pStringUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    STRINGREF    refRetVal  = NULL;
    APPDOMAINREF refThis    = (APPDOMAINREF) refThisUNSAFE;
    STRINGREF    pString    = (STRINGREF)    pStringUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, refThis, pString);

    ValidateArg(refThis);

    if (pString == NULL)
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_String");

    STRINGREF* stringVal = refThis->GetDomain()->GetOrInternString(&pString);
    if (stringVal != NULL)
    {
        refRetVal = *stringVal;
    }

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND


FCIMPL1(Object*, AppDomainNative::GetDynamicDir, AppDomainBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    STRINGREF    str        = NULL;
    APPDOMAINREF refThis    = (APPDOMAINREF) refThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refThis);
    
    AppDomain *pDomain = ValidateArg(refThis);
    str = COMString::NewString(pDomain->GetDynamicDir());
    HELPER_METHOD_FRAME_END();
    
    return OBJECTREFToObject(str);
}
FCIMPLEND

FCIMPL3(void, AppDomainNative::GetGrantSet, AppDomainBaseObject* refThisUNSAFE, OBJECTREF* ppGranted, OBJECTREF* ppDenied)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    APPDOMAINREF refThis = (APPDOMAINREF) refThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(refThis);
    
    AppDomain* pAppDomain = ValidateArg(refThis);    
    ApplicationSecurityDescriptor *pSecDesc = pAppDomain->GetSecurityDescriptor();
    Security::Resolve(pSecDesc);
    OBJECTREF granted = Security::GetGrantedPermissionSet(pSecDesc);
    *ppGranted = granted;

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL1(FC_BOOL_RET, AppDomainNative::IsUnloadingForcedFinalize, AppDomainBaseObject* refThis)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    AppDomain* pApp = ValidateArg((APPDOMAINREF)refThis);

    FC_RETURN_BOOL(pApp->IsFinalized());
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, AppDomainNative::IsFinalizingForUnload, AppDomainBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    BOOL            retVal = FALSE;
    APPDOMAINREF    refThis = (APPDOMAINREF) refThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(refThis);

    AppDomain* pApp = ValidateArg(refThis);
    retVal = pApp->IsFinalizing();

    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND

STDAPI PreBindAssembly(IApplicationContext *pAppCtx, IAssemblyName *pName,
                       IAssembly *pAsmParent, IAssemblyName **ppNamePostPolicy,
                       LPVOID pvReserved);


FCIMPL2(StringObject*, AppDomainNative::nApplyPolicy, AppDomainBaseObject* refThisUNSAFE, AssemblyNameBaseObject* refAssemblyNameUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    struct _gc
    {
        APPDOMAINREF    refThis;
        ASSEMBLYNAMEREF assemblyName;
        STRINGREF       rv;
    } gc;

    gc.refThis      = (APPDOMAINREF)refThisUNSAFE;
    gc.assemblyName = (ASSEMBLYNAMEREF) refAssemblyNameUNSAFE;
    gc.rv           = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_RETURNOBJ, gc);

    AppDomain* pDomain = ValidateArg(gc.refThis);

    if (gc.assemblyName == NULL)
    {
        COMPlusThrow(kArgumentNullException, L"ArgumentNull_AssemblyName");
    }
    if( (gc.assemblyName->GetSimpleName() == NULL) )
    {
        COMPlusThrow(kArgumentException, L"Format_StringZeroLength");
    }
    Thread *pThread = GetThread();
    CheckPointHolder cph(pThread->m_MarshalAlloc.GetCheckpoint()); //hold checkpoint for autorelease

    // Initialize spec
    AssemblySpec spec;
    spec.InitializeSpec(&(pThread->m_MarshalAlloc), 
                        &gc.assemblyName,
                        FALSE, /*fIsStringized*/ 
                        FALSE /*fForIntrospection*/
                       );

    SafeComHolder<IAssemblyName> pAssemblyName(NULL);
    SafeComHolder<IAssemblyName> pBoundName(NULL);
    IfFailThrow(spec.CreateFusionName(&pAssemblyName));
    HRESULT hr = PreBindAssembly(pDomain->GetFusionContext(),
                                pAssemblyName,
                                NULL, // pAsmParent (only needed to see if parent is loadfrom - in this case, we always want it to load in the normal ctx)
                                &pBoundName,
                                NULL  // pvReserved
                                );
    if (FAILED(hr) && hr != FUSION_E_REF_DEF_MISMATCH)
    {
        ThrowHR(hr);
    }

    StackSString sDisplayName;
    FusionBind::GetAssemblyNameDisplayName(pBoundName, /*modifies*/sDisplayName, 0 /*flags*/);
    gc.rv = COMString::NewString(sDisplayName);

    HELPER_METHOD_FRAME_END();
    return (StringObject*)OBJECTREFToObject(gc.rv);
}
FCIMPLEND

FCIMPL1(UINT32, AppDomainNative::GetAppDomainId, AppDomainBaseObject* refThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    APPDOMAINREF domainRef = (APPDOMAINREF) refThisUNSAFE;
    AppDomain* pDomain = ValidateArg(domainRef);

    return pDomain->GetId().m_dwId;
}
FCIMPLEND
