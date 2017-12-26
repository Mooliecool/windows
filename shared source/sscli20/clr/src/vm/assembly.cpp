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
/*============================================================
**
** Header: Assembly.cpp
**
** Purpose: Implements assembly (loader domain) architecture
**
** Date:  Dec 1, 1998
**
===========================================================*/

#include "common.h"

#include <stdlib.h>

#include "assembly.hpp"
#include "appdomain.hpp"
#include "security.h"
#include "comstring.h"
#include "comreflectioncommon.h"
#include "assemblysink.h"
#include "perfcounters.h"
#include "assemblyname.hpp"
#include "fusion.h"
#include "eeprofinterfaces.h"
#include "reflectclasswriter.h"
#include "comdynamic.h"
#include "comnlsinfo.h"
#include "internaldebug.h"


#include "eeconfig.h"
#include "strongname.h"

#include "ceefilegenwriter.h"
#include "assemblynative.hpp"
#include "timeline.h"


#include "appdomainnative.hpp"
#include "remoting.h"
#include "safegetfilesize.h"
#include "customattribute.h"
#include "winnls.h"

#include "appdomainhelper.h"
#include "memoryreport.h"
#include "constrainedexecutionregion.h"
#include "../md/compiler/custattr.h"
#include "peimagelayout.inl"


// Define these macro's to do strict validation for jit lock and class init entry leaks.
// This defines determine if the asserts that verify for these leaks are defined or not.
// These asserts can sometimes go off even if no entries have been leaked so this defines
// should be used with caution.
//
// If we are inside a .cctor when the application shut's down then the class init lock's
// head will be set and this will cause the assert to go off.,
//
// If we are jitting a method when the application shut's down then the jit lock's head
// will be set causing the assert to go off.

//#define STRICT_JITLOCK_ENTRY_LEAK_DETECTION
//#define STRICT_CLSINITLOCK_ENTRY_LEAK_DETECTION


#ifndef DACCESS_COMPILE

#define NO_FRIEND_ASSEMBLIES_MARKER ((IAssemblyName**)S_FALSE)

//----------------------------------------------------------------------------------------------
// The ctor's job is to initialize the Assembly enough so that the dtor can safely run.
// It cannot do any allocations or operations that might fail. Those operations should be done
// in Assembly::Init()
//----------------------------------------------------------------------------------------------
Assembly::Assembly(BaseDomain *pDomain, PEAssembly* pFile, DebuggerAssemblyControlFlags debuggerFlags) :
    m_FreeFlag(0),
    m_pAllowedFiles(NULL),
    m_pDomain(pDomain),
    m_pClassLoader(NULL),
    m_pEntryPoint(NULL),
    m_pManifest(NULL),
    m_pManifestFile(pFile),
    m_pOnDiskManifest(NULL),
    m_pFriendAssemblyNames(0),
    m_isDynamic(false),
    m_needsToHideManifestForEmit(FALSE),
    m_dwDynamicAssemblyAccess(ASSEMBLY_ACCESS_RUN),
    m_nextAvailableModuleIndex(1),
    m_pSharedSecurityDesc(NULL),
    m_fIsDomainNeutral(pDomain == SharedDomain::GetDomain()),
    m_pSharingProps(NULL),
    m_debuggerFlags(debuggerFlags),
    m_fTerminated(FALSE),
    m_fCheckedForNoStringInterning(FALSE),
    m_fRuntimeWrapExceptions(FALSE),
    m_fCheckedForRuntimeWrapExceptions(FALSE),
    m_HostAssemblyId(0)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    pFile->AddRef();
}

//----------------------------------------------------------------------------------------------
// Does most Assembly initialization tasks. It can assume the ctor has already run
// and the assembly is safely destructable. Whether this function throws or succeeds,
// it must leave the Assembly in a safely destructable state.
//----------------------------------------------------------------------------------------------
void Assembly::Init(AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    MEMORY_REPORT_CONTEXT_SCOPE("Assembly");

    m_pClassLoader = new ClassLoader(this);
    m_pClassLoader->Init(pamTracker);

    m_pSharedSecurityDesc = new SharedSecurityDescriptor(this);

    m_pAllowedFiles = new EEUtf8StringHashTable();

    COUNTER_ONLY(GetPrivatePerfCounters().m_Loading.cAssemblies++);

    if (GetManifestFile()->IsDynamic())
        m_pManifest = ReflectionModule::Create(this, GetManifestFile(), pamTracker);
    else
        m_pManifest = Module::Create(this, mdFileNil, GetManifestFile(), pamTracker);

    PrepareModuleForAssembly(m_pManifest, pamTracker);

    CacheManifestFiles();

    CacheManifestExportedTypes(pamTracker);

    // Cache assembly level reliability contract info.
    SetReliabilityContract(::GetReliabilityContract(GetManifestImport(), TokenFromRid(1, mdtAssembly)));

    // We'll load the friend assembly information lazily.  For the ngen case we should avoid
    //  loading it entirely.
    //CacheFriendAssemblyInfo();

    {
        CANNOTTHROWCOMPLUSEXCEPTION();
        FAULT_FORBID();
        //Cannot fail after this point.

        PublishModuleIntoAssembly(m_pManifest);

        return;  // Explicit return to let you know you are NOT welcome to add code after the CANNOTTHROW/FAULT_FORBID expires
    }
}

Assembly::~Assembly()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        DISABLED(FORBID_FAULT); //Must clean up some profiler stuff
    }
    CONTRACTL_END

    Terminate();

    // If there was a list of names of Friend Assemblies, loop through the list and free the names, then the list.
    if (m_pFriendAssemblyNames != NULL && m_pFriendAssemblyNames != NO_FRIEND_ASSEMBLIES_MARKER) {
        for (int i=0; m_pFriendAssemblyNames[i]; ++i)
            m_pFriendAssemblyNames[i]->Release();
        delete [] m_pFriendAssemblyNames;
    }

    if (m_pbStrongNameKeyPair && (m_FreeFlag & FREE_KEY_PAIR))
        delete[] m_pbStrongNameKeyPair;
    if (m_pwStrongNameKeyContainer && (m_FreeFlag & FREE_KEY_CONTAINER))
        delete[] m_pwStrongNameKeyContainer;

    if (m_pAllowedFiles)
        delete(m_pAllowedFiles);

    if (m_pSharingProps) 
    {
        if (m_pSharingProps->pContext)
            m_pSharingProps->pContext->Release();
        delete m_pSharingProps;            
    }

    if (IsDynamic()) {
        if (m_pOnDiskManifest)
            // clear the on disk manifest if it is not cleared yet.
            m_pOnDiskManifest = NULL;
    }

    if (m_pManifestFile)
        m_pManifestFile->Release();
}


void ProfilerCallAssemblyUnloadStarted(Assembly* assemblyUnloaded)
{
    WRAPPER_CONTRACT;
    PROFILER_CALL;
    g_profControlBlock.pProfInterface->AssemblyUnloadStarted((ThreadID) GetThread(), (AssemblyID)assemblyUnloaded);
}

void ProfilerCallAssemblyUnloadFinished(Assembly* assemblyUnloaded)
{
    WRAPPER_CONTRACT;
    PROFILER_CALL;
    g_profControlBlock.pProfInterface->AssemblyUnloadFinished((ThreadID) GetThread(), (AssemblyID) assemblyUnloaded, S_OK);
}

void Assembly::StartUnload()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FORBID_FAULT;
#ifdef PROFILING_SUPPORTED
    if (CORProfilerTrackAssemblyLoads()) {
        PAL_TRY
        {
            ProfilerCallAssemblyUnloadStarted(this);

        }
        PAL_EXCEPT_FILTER(DefaultCatchFilter, COMPLUS_EXCEPTION_EXECUTE_HANDLER)
        {
        }
        PAL_ENDTRY
    }
#endif

        // we need to release tlb files eagerly

}

void Assembly::Terminate( BOOL signalProfiler )
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FORBID_FAULT;

    if (this->m_fTerminated)
        return;


    delete m_pSharedSecurityDesc;
    m_pSharedSecurityDesc = NULL;

    if(m_pClassLoader != NULL) {
        delete m_pClassLoader;
        m_pClassLoader = NULL;
    }

    COUNTER_ONLY(GetPrivatePerfCounters().m_Loading.cAssemblies--);


#ifdef PROFILING_SUPPORTED
    if (CORProfilerTrackAssemblyLoads())
    {
        ProfilerCallAssemblyUnloadFinished(this);
    }    
#endif // PROFILING_SUPPORTED

    this->m_fTerminated = TRUE;
    return;  // makes the compiler happy when PROFILING_SUPPORTED isn't defined
}



Assembly *Assembly::Create(BaseDomain *pDomain, PEAssembly* pFile, DebuggerAssemblyControlFlags debuggerFlags, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END


    MEMORY_REPORT_ASSEMBLY_SCOPE(pFile);

    NewHolder<Assembly> pAssembly (new Assembly(pDomain, pFile, debuggerFlags));

    // If there are problems that arise from this call stack, we'll chew up a lot of stack
    // with the various EX_TRY/EX_HOOKs that we will encounter.
    INTERIOR_STACK_PROBE_FOR(GetThread(), DEFAULT_ENTRY_PROBE_SIZE); 

    if (CORProfilerTrackAssemblyLoads())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->AssemblyLoadStarted((ThreadID) GetThread(), (AssemblyID)(Assembly*) pAssembly);

    }

    // Need TRY/HOOK instead of holder so we can get HR of exception thrown for profiler callback
    EX_TRY
    {
        pAssembly->Init(pamTracker);
    }
    EX_HOOK
    {
        if (CORProfilerTrackAssemblyLoads())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->AssemblyLoadFinished((ThreadID) GetThread(),
                                                                    (AssemblyID)(Assembly*) pAssembly,
                                                                    GET_EXCEPTION()->GetHR());
        }
    }
    EX_END_HOOK;

    pAssembly.SuppressRelease();
    END_INTERIOR_STACK_PROBE;
    
    return pAssembly;
}


Assembly *Assembly::CreateDynamic(BaseDomain *pDomain, CreateDynamicAssemblyArgs *args)
{
    // WARNING: not backout clean
    CONTRACT(Assembly *)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(args));
    }
    CONTRACT_END;

    AllocMemTracker amTracker;
    AllocMemTracker *pamTracker = &amTracker;

    Assembly *pRetVal = NULL;

    // First, we set up a pseudo-manifest file for the assembly.

    // Set up the assembly name

    STRINGREF strRefName = (STRINGREF) args->assemblyName->GetSimpleName();

    if (strRefName == NULL)
        COMPlusThrow(kArgumentException, L"ArgumentNull_AssemblyNameName");

    StackSString name;
    strRefName->GetSString(name);

    if (name.GetCount() == 0)
        COMPlusThrow(kArgumentException, L"ArgumentNull_AssemblyNameName");

    SString::Iterator i = name.Begin();
    if (COMCharacter::nativeIsWhiteSpace(*i)
        || name.Find(i, '\\')
        || name.Find(i, ':')
        || name.Find(i, '/'))
        COMPlusThrow(kArgumentException, L"Argument_InvalidAssemblyName");

    // Set up the assembly manifest metadata
    // When we create dynamic assembly, we always use a working copy of IMetaDataAssemblyEmit
    // to store temporary runtime assembly information. This is to preserve the invariant that
    // an assembly must have a PEFile with proper metadata.
    // This working copy of IMetaDataAssemblyEmit will store every AssemblyRef as a simple name
    // reference as we must have an instance of Assembly(can be dynamic assembly) before we can
    // add such a reference. Also because the referenced assembly if dynamic strong name, it may
    // not be ready to be hashed!

    SafeComHolder<IMetaDataAssemblyEmit> pAssemblyEmit;
    PEFile::DefineEmitScope(IID_IMetaDataAssemblyEmit, &pAssemblyEmit);

    // remember the hash algorithm
    ULONG ulHashAlgId = args->assemblyName->GetAssemblyHashAlgorithm();
    if (ulHashAlgId == 0)
        ulHashAlgId = CALG_SHA1;

    ASSEMBLYMETADATA assemData;
    memset(&assemData, 0, sizeof(assemData));

    // get the version info (default to 0.0.0.0 if none)
    VERSIONREF versionRef = (VERSIONREF) args->assemblyName->GetVersion();
    if (versionRef != NULL) {
        assemData.usMajorVersion = versionRef->GetMajor();
        assemData.usMinorVersion = versionRef->GetMinor();
        assemData.usBuildNumber = versionRef->GetBuild();
        assemData.usRevisionNumber = versionRef->GetRevision();
    }

    struct _gc {
        OBJECTREF granted;
        OBJECTREF denied;
        OBJECTREF cultureinfo;
        STRINGREF pString;
        OBJECTREF orArrayOrContainer;
        OBJECTREF throwable;
        OBJECTREF strongNameKeyPair;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);

    StackSString culture;

    gc.cultureinfo = args->assemblyName->GetCultureInfo();
    if (gc.cultureinfo != NULL) {

        MethodDescCallSite getName(METHOD__CULTURE_INFO__GET_NAME, &gc.cultureinfo);

        ARG_SLOT args2[] = 
        {
            ObjToArgSlot(gc.cultureinfo)
        };

        // convert culture info into a managed string form
        gc.pString = getName.Call_RetSTRINGREF(args2);
        gc.pString->GetSString(culture);

        assemData.szLocale = (LPWSTR) (LPCWSTR) culture;
    }

    SBuffer publicKey;
    if (args->assemblyName->GetPublicKey() != NULL) {

        publicKey.Set(args->assemblyName->GetPublicKey()->GetDataPtr(),
                      args->assemblyName->GetPublicKey()->GetNumComponents());

    }

    // get flags
    DWORD dwFlags = args->assemblyName->GetFlags();

    mdAssembly ma;
    IfFailThrow(pAssemblyEmit->DefineAssembly(publicKey, publicKey.GetSize(), ulHashAlgId,
                                               name, &assemData, dwFlags,
                                               &ma));

    AppDomain *pCallersDomain;
    Assembly *pCaller = SystemDomain::GetCallersAssembly(args->stackMark, &pCallersDomain);

    // Now create a dynamic PE file out of the name & metadata

    PEAssemblyHolder pFile(PEAssembly::Create(pCaller->GetManifestFile(), pAssemblyEmit, args->access & ASSEMBLY_ACCESS_REFLECTION_ONLY));

    // Create a domain assembly
    NewHolder<DomainAssembly> pDomainAssembly(new DomainAssembly(GetAppDomain(), pFile, NULL, NULL));

    // Start loading process

    // Get the security descriptor for the assembly.
    AssemblySecurityDescriptor *pSecDesc = pDomainAssembly->GetSecurityDescriptor();

    // Propagate identity and permission request information into the assembly's
    // security descriptor. Then when policy is resolved we'll end up with the
    // correct grant set.
    // If identity has not been provided then the caller's assembly will be
    // calculated instead and we'll just copy the granted permissions from the
    // caller to the new assembly and mark policy as resolved (done
    // automatically by SetGrantedPermissionSet).
    Security::SetRequestedPermissionSet(pSecDesc, 
                                        args->requiredPset,
                                        args->optionalPset,
                                        args->refusedPset);

    // Don't bother with setting up permissions if this isn't allowed to run
    if ((args->identity != NULL) &&
        (args->access & ASSEMBLY_ACCESS_RUN))
    {
        Security::SetEvidence(pSecDesc, args->identity);
    }
    else
    {
        AssemblySecurityDescriptor *pCallerSecDesc = NULL;
        if (pCaller) // can be null if caller is interop
        {
            pCallerSecDesc = pCaller->GetSecurityDescriptor(pCallersDomain);
            gc.granted = Security::GetGrantedPermissionSet(pCallerSecDesc, &(gc.denied));
            // Caller may be in another appdomain context, in which case we'll
            // need to marshal/unmarshal the grant and deny sets across.
            if (pCallersDomain != GetAppDomain())
            {
                gc.granted = AppDomainHelper::CrossContextCopyFrom(pCallersDomain->GetId(), &(gc.granted));
                if (gc.denied != NULL)
                {
                    gc.denied = AppDomainHelper::CrossContextCopyFrom(pCallersDomain->GetId(), &(gc.denied));
                }
            }
            Security::SetGrantedPermissionSet(pSecDesc, gc.granted, gc.denied, Security::GetSpecialFlags(pCallerSecDesc));
        }
        else
        {
            Security::SetGrantedPermissionSet(pSecDesc, gc.granted, gc.denied, 0xFFFFFFFF); // interop gets full trust
        }
    }

    {
        // Create a concrete assembly
        // (!Do not remove scoping brace: order is important here: the Assembly holder must destruct before the AllocMemTracker!)
        NewHolder<Assembly> pAssem(Assembly::Create(pDomain, pFile, pDomainAssembly->GetDebuggerInfoBits(), pamTracker));
        pAssem->m_isDynamic = true;
        pAssem->m_dwDynamicAssemblyAccess = args->access;

        // Set the additional strong name information

        pAssem->SetStrongNameLevel(Assembly::SN_NONE);

        if (publicKey.GetSize() > 0) {

            pAssem->SetStrongNameLevel(Assembly::SN_PUBLIC_KEY);
            gc.strongNameKeyPair = args->assemblyName->GetStrongNameKeyPair();
            // If there's a public key, there might be a strong name key pair.
            if (gc.strongNameKeyPair != NULL) 
            {
                MethodDescCallSite getKeyPair(METHOD__STRONG_NAME_KEY_PAIR__GET_KEY_PAIR, &gc.strongNameKeyPair);

                ARG_SLOT arglist[] = 
                {
                    ObjToArgSlot(gc.strongNameKeyPair),
                    PtrToArgSlot(&gc.orArrayOrContainer)
                };

                BOOL bKeyInArray;
                bKeyInArray = (BOOL)getKeyPair.Call_RetBool(arglist);

                if (bKeyInArray) {
                    U1ARRAYREF orArray = (U1ARRAYREF)gc.orArrayOrContainer;
                    pAssem->m_cbStrongNameKeyPair = orArray->GetNumComponents();
                    pAssem->m_pbStrongNameKeyPair = new BYTE[pAssem->m_cbStrongNameKeyPair];

                    pAssem->m_FreeFlag |= pAssem->FREE_KEY_PAIR;
                    memcpy(pAssem->m_pbStrongNameKeyPair, orArray->GetDataPtr(), pAssem->m_cbStrongNameKeyPair);
                    pAssem->SetStrongNameLevel(Assembly::SN_FULL_KEYPAIR_IN_ARRAY);
                }
                else {
                    STRINGREF orContainer = (STRINGREF)gc.orArrayOrContainer;
                    DWORD cchContainer = orContainer->GetStringLength();
                    pAssem->m_pwStrongNameKeyContainer = new WCHAR[cchContainer + 1];

                    pAssem->m_FreeFlag |= pAssem->FREE_KEY_CONTAINER;
                    memcpy(pAssem->m_pwStrongNameKeyContainer, orContainer->GetBuffer(), cchContainer * sizeof(WCHAR));
                    pAssem->m_pwStrongNameKeyContainer[cchContainer] = L'\0';

                    pAssem->SetStrongNameLevel(Assembly::SN_FULL_KEYPAIR_IN_CONTAINER);
                }
            }
        }

        //we need to suppress release for pAssem to avoid double release
        pAssem.SuppressRelease ();
        pDomainAssembly->SetAssembly(pAssem);

        // Finish loading process
        pDomainAssembly->Begin();
        pDomainAssembly->m_level = FILE_LOAD_ALLOCATE;
        pDomainAssembly->DeliverSyncEvents();
        pDomainAssembly->DeliverAsyncEvents();
        pDomainAssembly->FinishLoad();
        pDomainAssembly->ClearLoading();
        pDomainAssembly->m_level = FILE_ACTIVE;

        {
            CANNOTTHROWCOMPLUSEXCEPTION();
            FAULT_FORBID();

            //Cannot fail after this point

            pDomainAssembly.SuppressRelease(); // This also effectively suppresses the release of the pAssem 
            pamTracker->SuppressRelease();
            pAssem->SetIsTenured();
            pRetVal = pAssem;
        }
    }
    GCPROTECT_END();

    RETURN pRetVal;
}

ReflectionModule *Assembly::CreateDynamicModule(LPCWSTR name)
{
    CONTRACT(ReflectionModule *)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    AllocMemTracker amTracker;

    // Add a manifest entry for the module
    mdFile token;
    IMetaDataAssemblyEmit *pAssemblyEmit = GetManifestFile()->GetAssemblyEmitter();
    IfFailThrow(pAssemblyEmit->DefineFile(name, NULL, 0, 0, &token));

    // Define initial metadata for the module
    SafeComHolder<IMetaDataEmit> pEmit;
    PEFile::DefineEmitScope(IID_IMetaDataEmit, (void **) &pEmit);

    pEmit->SetModuleProps(name);

    // Create the PEFile for the module
    PEModuleHolder pFile(PEModule::Create(GetManifestFile(), token, pEmit));

    // Create the DomainModule
    NewHolder<DomainModule> pDomainModule(new DomainModule(GetAppDomain(), GetDomainAssembly(), pFile));

    // Create the module itself
    ReflectionModuleHolder pWrite(ReflectionModule::Create(this, pFile, &amTracker));

    amTracker.SuppressRelease();
    pWrite->SetIsTenured();

    // Modules take the DebuggerAssemblyControlFlags down from its parent Assembly initially.
    // By default, this turns on JIT optimization.
    
    pWrite->SetDebuggerInfoBits(GetDebuggerInfoBits());

    // Associate the two
    pDomainModule->SetModule(pWrite);
    m_pManifest->StoreFileThrowing(token, pWrite);

    // Simulate loading process
    pDomainModule->Begin();
    pDomainModule->DeliverSyncEvents();
    pDomainModule->DeliverAsyncEvents();
    pDomainModule->FinishLoad();
    pDomainModule->ClearLoading();
    pDomainModule->m_level = FILE_ACTIVE;

    pDomainModule.SuppressRelease();
    RETURN pWrite.Extract();
}


void Assembly::SetDomainAssembly(DomainAssembly *pDomainAssembly)
{
    CONTRACT_VOID
    {
        PRECONDITION(CheckPointer(pDomainAssembly, NULL_OK));
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    GetManifestModule()->SetDomainFile(pDomainAssembly);

    AssemblySecurityDescriptor *pSec = pDomainAssembly->GetSecurityDescriptor();

    Security::AddToSharedDescriptor(pSec, GetSharedSecurityDescriptor());

    TIMELINE_START(LOADER, ("EarlyResolve"));

    if (!pDomainAssembly->ShouldDelayPolicyResolution())
    {
        if (g_pConfig->SecurityNeutralCode())
        {
            // If a security failure occurs, we will note it on the DomainAssembly.  However
            // we let the load proceed as passive uses of the assembly are still valid.
            EX_TRY
            {
                Security::EarlyResolveThrowing(this, pSec);
            }
            EX_CATCH
            {
                if (!GET_EXCEPTION()->IsTransient())
                    pDomainAssembly->SetSecurityError(GET_EXCEPTION());
            }
            EX_END_CATCH(RethrowTransientExceptions);
        }
        else
            Security::EarlyResolveThrowing(this, pSec);
    }

    TIMELINE_END(LOADER, ("EarlyResolve"));

    RETURN;
}

#endif // #ifndef DACCESS_COMPILE

DomainAssembly *Assembly::GetDomainAssembly(AppDomain *pDomain)
{
    CONTRACT(DomainAssembly *)
    {
        PRECONDITION(CheckPointer(pDomain, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    RETURN GetManifestModule()->GetDomainAssembly(pDomain);
}

DomainAssembly *Assembly::FindDomainAssembly(AppDomain *pDomain)
{
    CONTRACT(DomainAssembly *)
    {
        PRECONDITION(CheckPointer(pDomain));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACT_END;

    PREFIX_ASSUME (GetManifestModule() !=NULL); 
    RETURN GetManifestModule()->FindDomainAssembly(pDomain);
}

BOOL Assembly::IsIntrospectionOnly()
{
    WRAPPER_CONTRACT;
    return m_pManifestFile->IsIntrospectionOnly();
}


#ifndef DACCESS_COMPILE

ListLock*  Assembly::GetClassInitLock()
{
    WRAPPER_CONTRACT;

    return m_pDomain->GetClassInitLock();
}

LoaderHeap* Assembly::GetLowFrequencyHeap()
{
    WRAPPER_CONTRACT;

    return m_pDomain->GetLowFrequencyHeap();
}

LoaderHeap* Assembly::GetHighFrequencyHeap()
{
    WRAPPER_CONTRACT;

    return m_pDomain->GetHighFrequencyHeap();
}

LoaderHeap* Assembly::GetStubHeap()
{
    WRAPPER_CONTRACT;

    return m_pDomain->GetStubHeap();
}

#endif // #ifndef DACCESS_COMPILE

BaseDomain* Assembly::GetDomain()
{
    LEAF_CONTRACT;

    _ASSERTE(m_pDomain);
    return static_cast<BaseDomain*>(m_pDomain);
}
AssemblySecurityDescriptor *Assembly::GetSecurityDescriptor(AppDomain *pDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END

    AssemblySecurityDescriptor* pSecDesc;

    if (pDomain == NULL)
        pDomain = GetAppDomain();

    PREFIX_ASSUME(FindDomainAssembly(pDomain) != NULL);
    pSecDesc = FindDomainAssembly(pDomain)->GetSecurityDescriptor();

    CONSISTENCY_CHECK(pSecDesc != NULL);

    return pSecDesc;
}

#ifndef DACCESS_COMPILE

void Assembly::SetParent(BaseDomain* pParent)
{
    LEAF_CONTRACT;

    m_pDomain = pParent;
}


mdFile Assembly::GetManifestFileToken(LPCSTR name)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Don't need to call InitTable() because it was called when
    // m_pAllowedFiles was created.

    HashDatum datum;
    // Note: We're doing a case sensitive lookup
    // This is OK because the lookup string and the string we insert into the hashtable
    // are obtained from the same place.
    if (m_pAllowedFiles->GetValue(name, &datum)) {

        if (datum) // internal module
            return (mdFile)(size_t)datum;
        else // manifest file
            return mdFileNil;
    }
    else
        return mdTokenNil; // not found
}

mdFile Assembly::GetManifestFileToken(IMDInternalImport *pImport, mdFile kFile)
{
    WRAPPER_CONTRACT;

    LPCSTR name;
    if (! ((TypeFromToken(kFile) == mdtFile) &&
           pImport->IsValidToken(kFile)) ) {

        BAD_FORMAT_NOTHROW_ASSERT(!"Invalid File token");
        return mdTokenNil;
    }

    pImport->GetFileProps(kFile, &name, NULL, NULL, NULL);

    return GetManifestFileToken(name);
}



Module *Assembly::FindModuleByExportedType(mdExportedType mdType,
                                           Loader::LoadFlag loadFlag,
                                           mdTypeDef mdNested,
                                           mdTypeDef* pCL)
{
    CONTRACT(Module *)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, loadFlag==Loader::Load ? NULL_NOT_OK : NULL_OK));
    }
    CONTRACT_END

    mdToken mdLinkRef;
    mdToken mdBinding;

    IMDInternalImport *pManifestImport = GetManifestImport();

    pManifestImport->GetExportedTypeProps(mdType,
                                          NULL,
                                          NULL,
                                          &mdLinkRef, //impl
                                          &mdBinding, // Hint
                                          NULL); // dwflags

    // Don't trust the returned tokens.
    if (!pManifestImport->IsValidToken(mdLinkRef))
    {
        if ( loadFlag!=Loader::Load)
        {
            RETURN NULL;
        }
        else
        {
            ThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN);
        }
    }


    switch(TypeFromToken(mdLinkRef)) {
    case mdtAssemblyRef:
        {
            *pCL = mdTypeDefNil;  // We don't trust the mdBinding token

            Assembly *pAssembly = NULL;
            switch(loadFlag) 
            {
                case Loader::Load:
                {
                    // LoadAssembly never returns NULL
                    DomainAssembly *pDomainAssembly = GetManifestModule()->LoadAssembly(GetAppDomain(), mdLinkRef);
                    PREFIX_ASSUME(pDomainAssembly != NULL);

                    RETURN pDomainAssembly->GetCurrentModule();
                };
                case Loader::DontLoad: 
                    pAssembly=GetManifestModule()->GetAssemblyIfLoaded(mdLinkRef);
                    break;
                case Loader::SafeLookup:
                    pAssembly=GetManifestModule()->LookupAssemblyRef(mdLinkRef);
                    break;
                default:
                    _ASSERTE(FALSE);
            }  
            
            if (pAssembly)
                RETURN pAssembly->GetManifestModule();
            else
                RETURN NULL;

        }

    case mdtFile:
        {
            // We may not want to trust this TypeDef token, since it
            // was saved in a scope other than the one it was defined in
            if (mdNested == mdTypeDefNil)
                *pCL = mdBinding;
            else
                *pCL = mdNested;

            // Note that we don't want to attempt a LoadModule if a GetModuleIfLoaded will
            // succeed, because it has a stronger contract.
            Module *pModule = GetManifestModule()->GetModuleIfLoaded(mdLinkRef, FALSE);
            if (pModule != NULL)
                RETURN pModule;

            if(loadFlag==Loader::SafeLookup)
                return NULL;

            // We should never get here in the GC case - the above should have succeeded.
            CONSISTENCY_CHECK(!FORBIDGC_LOADER_USE_ENABLED());

            DomainFile *pDomainModule = GetManifestModule()->LoadModule(GetAppDomain(), mdLinkRef, FALSE, loadFlag!=Loader::Load);

            if (pDomainModule == NULL)
                RETURN NULL;
            else
            {
                pModule = pDomainModule->GetCurrentModule();
                if (pModule == NULL)
                {
                    _ASSERTE(loadFlag!=Loader::Load);
                }

                RETURN pModule;
            }
        }

    case mdtExportedType:
        // Only override the nested type token if it hasn't been set yet.
        if (mdNested != mdTypeDefNil)
            mdBinding = mdNested;

        RETURN FindModuleByExportedType(mdLinkRef, loadFlag, mdBinding, pCL);

    default:
        ThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN_TYPE);
    }
}

#endif // #ifndef DACCESS_COMPILE

// The returned Module is non-NULL unless you prevented the load by setting dontLoad=TRUE.
/* static */
Module *Assembly::FindModuleByTypeRef(Module *pModule, mdTypeRef tkType,
                                     Loader::LoadFlag loadFlag, BOOL *pfNoResolutionScope)
{
    CONTRACT(Module*)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM();); }

        MODE_ANY;

        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(TypeFromToken(tkType) == mdtTypeRef);
        PRECONDITION(CheckPointer(pfNoResolutionScope));
        POSTCONDITION( CheckPointer(RETVAL, loadFlag==Loader::Load ? NULL_NOT_OK : NULL_OK) );
    }
    CONTRACT_END

    IMDInternalImport *pImport;
    BOOL fFoundBadToken = FALSE;

    {
        // Find the top level encloser
        GCX_NOTRIGGER();
        CANNOTTHROWCOMPLUSEXCEPTION();

        pImport = pModule->GetMDImport();
        if (TypeFromToken(tkType) != mdtTypeRef)
            ThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN_TYPE);

        // If nested, get top level encloser's impl
        do {
            if(!pImport->IsValidToken(tkType))
            {
                fFoundBadToken = TRUE;
                break;
            }

            tkType = pImport->GetResolutionScopeOfTypeRef(tkType);

            // nil-scope TR okay if there's an ExportedType
            // Return manifest file
            if (IsNilToken(tkType)) {
                *pfNoResolutionScope = TRUE;
                RETURN( pModule );
            }

        } while (TypeFromToken(tkType) == mdtTypeRef);
    }

    *pfNoResolutionScope = FALSE;

#ifndef DACCESS_COMPILE
    if (fFoundBadToken)
    {
        THROW_BAD_FORMAT(BFA_BAD_TYPEREF_TOKEN, pModule);
    }
#endif // #ifndef DACCESS_COMPILE

    switch (TypeFromToken(tkType)) {
        case mdtModule:
        {
            // Type is in the referencing module.
            GCX_NOTRIGGER();
            CANNOTTHROWCOMPLUSEXCEPTION();
            RETURN( pModule );
        }

        case mdtModuleRef:
        {
#ifndef DACCESS_COMPILE
            if (loadFlag!=Loader::Load || IsGCThread())
            {
                RETURN(pModule->LookupModule(tkType,FALSE));
            }
            
            DomainFile *pActualDomainFile = pModule->LoadModule(GetAppDomain(), tkType, FALSE, loadFlag!=Loader::Load);
            if (pActualDomainFile == NULL)
                RETURN NULL;
            else
                RETURN(  pActualDomainFile->GetModule() );

#else
            DacNotImpl();
            RETURN NULL;
#endif // #ifndef DACCESS_COMPILE
        }
        break;

    case mdtAssemblyRef:
        {
            // Do this first because it has a strong contract
            Assembly *pAssembly = NULL;
            if (loadFlag==Loader::SafeLookup)
                pAssembly=pModule->LookupAssemblyRef(tkType);
            else
                pAssembly=pModule->GetAssemblyIfLoaded(tkType);
            if (pAssembly)
                RETURN pAssembly->m_pManifest;
#ifndef DACCESS_COMPILE
            if (loadFlag!=Loader::Load)
#endif // #ifndef DACCESS_COMPILE
                RETURN NULL;

#ifndef DACCESS_COMPILE
            DomainAssembly *pDomainAssembly = pModule->LoadAssembly(GetAppDomain(), tkType);
            if (pDomainAssembly == NULL)
                RETURN NULL;
            
            pAssembly = pDomainAssembly->GetCurrentAssembly();
            if (pAssembly == NULL)
                RETURN NULL;
            else
                RETURN pAssembly->m_pManifest;
#endif // #ifndef DACCESS_COMPILE
        }

    default:
        ThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN_TYPE);
    }

}

#ifndef DACCESS_COMPILE

Module *Assembly::FindModuleByName(LPCSTR pszModuleName)
{
    CONTRACT(Module *)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    CQuickBytes qbLC;

    // Need to perform case insensitive hashing.
    UTF8_TO_LOWER_CASE(pszModuleName, qbLC);
    pszModuleName = (LPUTF8) qbLC.Ptr();

    mdFile kFile = GetManifestFileToken(pszModuleName);
    if (kFile == mdTokenNil)
        ThrowHR(COR_E_UNAUTHORIZEDACCESS);

    if (this == SystemDomain::SystemAssembly())
        RETURN m_pManifest->GetModuleIfLoaded(kFile);
    else
        RETURN m_pManifest->LoadModule(::GetAppDomain(), kFile)->GetModule();
}

void Assembly::CacheManifestExportedTypes(AllocMemTracker *pamTracker)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    // Prejitted assemblies have their table prebuilt.
    if (GetManifestFile()->HasNativeImage())
        RETURN;

    mdToken mdExportedType;

    HENUMInternalHolder phEnum(GetManifestImport());
    phEnum.EnumInit(mdtExportedType,
                    mdTokenNil);

    ClassLoader::AvailableClasses_LockHolder lh(m_pClassLoader);

    for(int i = 0; GetManifestImport()->EnumNext(&phEnum, &mdExportedType); i++)
        m_pClassLoader->AddExportedTypeHaveLock(GetManifestModule(),
                                                mdExportedType,
                                                pamTracker);

    RETURN;
}


void Assembly::CacheManifestFiles()
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    mdToken tkFile;
    LPCSTR pszFileName;
    CQuickBytes qbLC;

    HENUMInternalHolder phEnum(GetManifestImport());
    phEnum.EnumInit(mdtFile,
                    mdTokenNil);

    
    DWORD dwCount = GetManifestImport()->EnumGetCount(&phEnum);
    if (!m_pAllowedFiles->Init(dwCount+1, NULL))
        ThrowOutOfMemory();

    m_nextAvailableModuleIndex = dwCount+1;

    while (GetManifestImport()->EnumNext(&phEnum, &tkFile)) {
        if(TypeFromToken(tkFile) == mdtFile) {
            GetManifestImport()->GetFileProps(tkFile,
                                              &pszFileName,
                                              NULL,  // hash
                                              NULL,  // hash len
                                              NULL); // flags

            // Add to hash table
            m_pAllowedFiles->InsertValue(pszFileName, (HashDatum)(size_t)tkFile, TRUE);
            
            // Need to perform case insensitive hashing as well.
            {
                UTF8_TO_LOWER_CASE(pszFileName, qbLC);
                pszFileName = (LPUTF8) qbLC.Ptr();
            }

            // Add each internal module
            m_pAllowedFiles->InsertValue(pszFileName, (HashDatum)(size_t)tkFile, TRUE);
        }
    }

    HENUMInternalHolder phEnumModules(GetManifestImport());
    phEnumModules.EnumInit(mdtModuleRef, mdTokenNil);
    mdToken tkModuleRef;
    
    while (GetManifestImport()->EnumNext(&phEnumModules, &tkModuleRef)) {
        LPCSTR pszModuleRefName, pszModuleRefNameLower;
        
        if (TypeFromToken(tkModuleRef) == mdtModuleRef) {
            GetManifestImport()->GetModuleRefProps(tkModuleRef, &pszModuleRefName);

            // Convert to lower case and lookup
            {
                UTF8_TO_LOWER_CASE(pszModuleRefName, qbLC);
                pszModuleRefNameLower = (LPUTF8) qbLC.Ptr();
            }
            
            HashDatum datum;
            if (m_pAllowedFiles->GetValue(pszModuleRefNameLower, &datum)) {
                mdFile tkFileForModuleRef = (mdFile)(size_t)datum;
                m_pAllowedFiles->InsertValue(pszModuleRefName, (HashDatum)(size_t)tkFileForModuleRef);
            }
        }
    }
    
    // Add the manifest file
    if (GetManifestImport()->IsValidToken(GetManifestImport()->GetModuleFromScope())) {
        GetManifestImport()->GetScopeProps(&pszFileName, NULL);

        // Add to hash table
        m_pAllowedFiles->InsertValue(pszFileName, NULL, TRUE);
        
        // Need to perform case insensitive hashing as well.
        {
            UTF8_TO_LOWER_CASE(pszFileName, qbLC);
            pszFileName = (LPUTF8) qbLC.Ptr();
        }

        m_pAllowedFiles->InsertValue(pszFileName, NULL, TRUE);
    }
    else
        ThrowHR(COR_E_BADIMAGEFORMAT);

    RETURN;
}


void Assembly::PrepareModuleForAssembly(Module* module, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(module));
    }
    CONTRACTL_END;
    
    if (!module->IsPersistedObject(module->m_pAvailableClasses)) {
        if (!(module->IsResource()))
           // ! We intentionally do not take the AvailableClass lock here. It creates problems at
           // startup and we haven't yet published the module yet so nobody should be searching it.
            m_pClassLoader->PopulateAvailableClassHashTable(module,
                                                            pamTracker);
    }


#ifdef DEBUGGING_SUPPORTED
    // Modules take the DebuggerAssemblyControlFlags down from its
    // parent Assembly initially.
    module->SetDebuggerInfoBits(GetDebuggerInfoBits());

    LOG((LF_CORDB, LL_INFO10, "Module %s: bits=0x%x\n",
         module->GetFile()->GetSimpleName(),
         module->GetDebuggerInfoBits()));
#endif // DEBUGGING_SUPPORTED

    m_pManifest->EnsureFileCanBeStored(module->GetModuleRef());
}

// This is the final step of publishing a Module into an Assembly. This step cannot fail.
void Assembly::PublishModuleIntoAssembly(Module *module)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        FORBID_FAULT;
    }
    CONTRACTL_END

    GetManifestModule()->EnsuredStoreFile(module->GetModuleRef(), module);
    FastInterlockIncrement((LONG*)&m_pClassLoader->m_cUnhashedModules);
}



Module* Assembly::FindModule(PEFile *pFile, BOOL includeLoading)
{
    CONTRACT(Module *)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    DomainFile *pModule = GetDomainAssembly()->FindModule(pFile, includeLoading);

    if (pModule == NULL)
        RETURN NULL;
    else
        RETURN pModule->GetModule();
}


DomainFile* Assembly::FindIJWDomainFile(HMODULE hMod, const SString &path)
{
    CONTRACT(DomainFile *)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    RETURN GetDomainAssembly()->FindIJWModule(hMod, path);
}

//*****************************************************************************
// Set up the list of names of any friend assemblies
void Assembly::CacheFriendAssemblyInfo()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    HRESULT     hr;
    IMDInternalImport *pImport = NULL;  // Metadata for this assembly.
    mdCustomAttribute tkAttribute;      // A custom attribute on this assembly.
    const BYTE  *pbAttr;                // Custom attribute data as a BYTE*.
    ULONG       cbAttr;                 // Size of custom attribute data.
    ULONG       cbString;               // Size of string from custom attribute.
    ULONG       cb;                     // Size of an uncompressed length from custom attribute.
    ULONG       cFriendNames;           // Count of friends.
    ULONG       ix;                     // Loop counter for friends.

    // See if this assembly declares any friend assemblies.
    pImport = this->GetManifestImport();
    MDEnumHolder hEnum(pImport);        // Enumerator for custom attributes
    hr = pImport->EnumCustomAttributeByNameInit(GetManifestToken(), FRIEND_ASSEMBLY_TYPE, &hEnum);

    if (hr == S_FALSE) // there aren't any friends.
    {
        m_pFriendAssemblyNames = NO_FRIEND_ASSEMBLIES_MARKER;
        return;
    }

    // Was there an error enumerating?
    IfFailThrow(hr);

    // Allocate a working array to hold the friend names.
    cFriendNames = pImport->EnumGetCount(&hEnum);
    typedef NonVMComHolder<IAssemblyName> IAssemblyNameHolder;
    NewArrayHolder< IAssemblyNameHolder > names ( new IAssemblyNameHolder [cFriendNames] );

    for (ix=0; ix < cFriendNames; ++ix)
        names[ix] = 0;

    // Enumerate over the declared friends, and create an IAssemblyName for each.
    ix = 0;
    while (pImport->EnumNext(&hEnum, &tkAttribute))
    {   // Get raw custom attribute.
        pImport->GetCustomAttributeAsBlob(tkAttribute, (const void**)&pbAttr, &cbAttr);

        // Check the header.
        if (cbAttr < 2 || pbAttr[0] != 0x01 || pbAttr[1] != 0x00)
        {
            THROW_BAD_FORMAT(BFA_BAD_CA_HEADER, GetManifestModule());
        }
        cbAttr -= 2;
        pbAttr += 2;

        // Get the string length and check it.
        cb = CorSigUncompressData(pbAttr, &cbString);
        if (cb > cbAttr || cbString > (cbAttr-cb))
        {
            THROW_BAD_FORMAT(BFA_BAD_CA_STRING, GetManifestModule());
        }
        cbAttr -= cb;
        pbAttr += cb;

        // Convert the string to Unicode.
        StackSString displayName(SString::Utf8, (const UTF8 *)pbAttr, cbString);

        // Create an AssemblyNameObject from the string.
        hr = CreateAssemblyNameObject(&(names[ix]), displayName.GetUnicode(), CANOF_PARSE_FRIEND_DISPLAY_NAME, NULL);
        if (FAILED(hr))
            THROW_HR_ERROR_WITH_INFO(hr, GetManifestModule());

        // If this assembly has a strong name, then its friends declarations need to have strong names too
        if (IsStrongNamed())
        {
            DWORD dwSize=0;

            if (SUCCEEDED(hr = names[ix]->GetProperty(ASM_NAME_PUBLIC_KEY, NULL, &dwSize))) 
                // If this call succeeds with an empty buffer, then the supplied name doesn't have a public key.
                THROW_HR_ERROR_WITH_INFO(META_E_CA_FRIENDS_SN_REQUIRED, GetManifestModule());
            else if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
                IfFailThrow(hr);            
        }

        // On to next.
        ++ix;
    }
    _ASSERTE(ix == cFriendNames);

    // Now allocate an array on the Assembly to hold the results.
    IAssemblyName **pNames = new IAssemblyName*[cFriendNames+1];
    for (ix=0; ix < cFriendNames; ++ix)
    {
        pNames[ix] = names[ix].GetValue();
        names[ix].SuppressRelease();
    }
    pNames[cFriendNames] = 0;

    // Update the member, if no other thread already has.
    if (InterlockedCompareExchangePointer((PVOID*)&m_pFriendAssemblyNames, pNames, NULL) != NULL)
    {   // Another thread got there first, so release this thread's allocations.
        for (int i=0; pNames[i]; ++i)
            pNames[i]->Release();
        delete [] pNames;
    }

} // void Assembly::CacheFriendAssemblyInfo()

//*****************************************************************************
// Is the given assembly a friend of this assembly?
BOOL Assembly::GrantsFriendAccessTo(    // TRUE if this assembly grants friend access to the other assembly.
    Assembly    *pAssembly)             // The other Assembly
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    BOOL        bIsFriend = FALSE;      // Assume no friendship.
    ULONG       ix;                     // To Iterate over assembly names.
    HRESULT     hr;

    // If the cache has not been inited, do that now.
    if (m_pFriendAssemblyNames == NULL)
        CacheFriendAssemblyInfo();
    
    // If the assembly has NO friends, the one in question is not.
    if (m_pFriendAssemblyNames == NO_FRIEND_ASSEMBLIES_MARKER)
        return FALSE;

    // Iterate over the names, and see if any of them match.
    for (ix=0; m_pFriendAssemblyNames[ix]; ++ix)
    {
        // Check whether the target assembly matches the declared friend.
        hr = m_pFriendAssemblyNames[ix]->IsEqual(pAssembly->GetFusionAssemblyName(), ASM_CMPF_DEFAULT);
        IfFailThrow(hr);
        
        if (hr == S_OK)
        {   // Yes, it matches.
            bIsFriend = TRUE;
            break;
        }
    }
    
    return bIsFriend;
} // Assembly::GrantsFriendAccessTo();



static void RunMainPre()
{
    LEAF_CONTRACT;

    _ASSERTE(GetThread() != 0);
    g_fWeControlLifetime = TRUE;
}

static void RunMainPost()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(GetThread()));
    }
    CONTRACTL_END

    GCX_PREEMP();
    ThreadStore::s_pThreadStore->WaitForOtherThreads();

#ifdef _DEBUG
    // Turn on memory dump checking in debug mode.
    _DbgRecord();
#endif
}


INT32 Assembly::ExecuteMainMethod(PTRARRAYREF *stringArgs)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    INT32   iRetVal = 0;

    BEGIN_ENTRYPOINT_THROWS;

    Thread *pThread = GetThread();
    MethodDesc *pMeth;
    {
        // This thread looks like it wandered in -- but actually we rely on it to keep the process alive.
        pThread->SetBackground(FALSE);
    
        GCX_COOP();

        pMeth = GetEntryPoint();
        if (pMeth) {
            RunMainPre();
            hr = ClassLoader::RunMain(pMeth, 1, &iRetVal, stringArgs);
        }
    }

    //RunMainPost is supposed to be called on the main thread of an EXE,
    //after that thread has finished doing useful work.  It contains logic
    //to decide when the process should get torn down.  So, don't call it from
    // AppDomain.ExecuteAssembly()
    if (pMeth) {
        if (stringArgs == NULL)
            RunMainPost();
    }
    else {
        StackSString displayName;
        GetDisplayName(displayName);
        COMPlusThrowHR(COR_E_MISSINGMETHOD, IDS_EE_FAILED_TO_FIND_MAIN, displayName);
    }

    if (FAILED(hr))
        ThrowHR(hr);
    END_ENTRYPOINT_THROWS;
    return iRetVal;
}

MethodDesc* Assembly::GetEntryPoint()
{
    CONTRACT(MethodDesc*)
    {
        THROWS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_COOPERATIVE;

        // Can return NULL if no entry point.
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    if (m_pEntryPoint)
        RETURN m_pEntryPoint;

    mdToken mdEntry = m_pManifestFile->GetEntryPointToken();
    if (IsNilToken(mdEntry))
        RETURN NULL;

    Module *pModule = NULL;
    switch(TypeFromToken(mdEntry)) {
    case mdtFile:
        pModule = m_pManifest->LoadModule(GetAppDomain(), mdEntry, FALSE)->GetModule();
        
        mdEntry = pModule->GetEntryPointToken();
        if ( (TypeFromToken(mdEntry) != mdtMethodDef) ||
             (!pModule->GetMDImport()->IsValidToken(mdEntry)) )
            pModule = NULL;
        break;
        
    case mdtMethodDef:
        if (m_pManifestFile->GetPersistentMDImport()->IsValidToken(mdEntry))
            pModule = m_pManifest;
        break;
    }

    // May be unmanaged entrypoint
    if (!pModule)
        RETURN NULL;

    // We need to get its properties and the class token for this MethodDef token.
    mdToken mdParent;
    if (FAILED(pModule->GetMDImport()->GetParentToken(mdEntry, &mdParent))) {
        StackSString displayName;
        GetDisplayName(displayName);
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, IDS_EE_ILLEGAL_TOKEN_FOR_MAIN, displayName);
    }

    if (mdParent != COR_GLOBAL_PARENT_TOKEN) {
        // This code needs a class init frame, because without it, the
        // debugger will assume any code that results from searching for a
        // type handle (ie, loading an assembly) is the first line of a program.
        FrameWithCookie<DebuggerClassInitMarkFrame> __dcimf;
            
        EEClass* pInitialClass = ClassLoader::LoadTypeDefOrRefThrowing(pModule, mdParent, 
                                                                       ClassLoader::ThrowIfNotFound,
                                                                       ClassLoader::FailIfUninstDefOrRef).GetClass();

        m_pEntryPoint = pInitialClass->FindMethod(mdEntry);
            
        __dcimf.Pop();
    }
    else
        m_pEntryPoint = pModule->FindMethod(mdEntry);
    
    RETURN m_pEntryPoint;
}


PEModule *Assembly::LoadModule(mdFile kFile, BOOL fLoadResource)
{
    CONTRACT(PEModule *) 
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, fLoadResource ? NULL_NOT_OK : NULL_OK));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END

    if (! ((TypeFromToken(kFile) == mdtFile) &&
           GetManifestImport()->IsValidToken(kFile)) ) {
        ThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_FILE_TOKEN);
    }

    LPCSTR psModuleName;
    DWORD dwFlags;
    GetManifestImport()->GetFileProps(kFile,
                                      &psModuleName,
                                      NULL,
                                      NULL,
                                      &dwFlags);

    if (! (IsFfContainsMetaData(dwFlags) || fLoadResource) ) 
        RETURN NULL;

    SString name(SString::Utf8, psModuleName);
    PEModule* pModule = NULL;

    if (FusionBind::VerifyBindingStringW((LPCWSTR)name)) {

        EX_TRY
        {
            if (GetFusionAssembly()) {
                StackSString path;
                GetAppDomain()->GetFileFromFusion(GetFusionAssembly(),
                                                  (LPCWSTR)name, path);
                pModule = PEModule::Open(m_pManifestFile, kFile, path);
                goto lDone;
            }
            
            if (GetIHostAssembly()) {
                pModule = PEModule::Open(m_pManifestFile, kFile, name);
                goto lDone;
            }

            if (!m_pManifestFile->GetPath().IsEmpty()) {
                StackSString path = m_pManifestFile->GetPath();
                
                SString::Iterator i = path.End()-1;
            
                if (PEAssembly::FindLastPathSeparator(path, i)) {
                    path.Truncate(++i);
                    path.Insert(i, name);
                }
                pModule = PEModule::Open(m_pManifestFile, kFile, path);
            }

        lDone: ;
        }
        EX_CATCH
        {
            Exception *ex = GET_EXCEPTION();
            if (FileNotFound(ex->GetHR()) ||
                (ex->GetHR() == FUSION_E_INVALID_NAME))
                pModule = RaiseModuleResolveEvent(psModuleName, kFile);

            if (!pModule) {
                EEFileLoadException::Throw(name, ex->GetHR(), ex);
            }
        }
        EX_END_CATCH(SwallowAllExceptions)
    }

    if (!pModule) {
        pModule = RaiseModuleResolveEvent(psModuleName, kFile);
        if (!pModule) {
            EEFileLoadException::Throw(name, HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND));
        }
    }

    RETURN pModule;    
}


OBJECTREF Assembly::GetExposedObject()
{
    CONTRACT(OBJECTREF)
    {
        GC_TRIGGERS;
        THROWS;
        INJECT_FAULT(COMPlusThrowOM(););
        POSTCONDITION(RETVAL != NULL);
        MODE_COOPERATIVE;
        }
    CONTRACT_END;

    RETURN GetDomainAssembly()->GetExposedAssemblyObject();
}

/* static */
BOOL Assembly::FileNotFound(HRESULT hr)
{
    LEAF_CONTRACT;

    switch (hr) {
    case HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND):
    case HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
    case HRESULT_FROM_WIN32(ERROR_INVALID_NAME):
    case CTL_E_FILENOTFOUND:
    case HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND):
    case HRESULT_FROM_WIN32(ERROR_BAD_NET_NAME):
    case HRESULT_FROM_WIN32(ERROR_BAD_NETPATH):
    case HRESULT_FROM_WIN32(ERROR_NOT_READY):
    case HRESULT_FROM_WIN32(ERROR_WRONG_TARGET_NAME):
    case HRESULT_FROM_WIN32(ERROR_DLL_NOT_FOUND):

    case COR_E_DLLNOTFOUND:
    case INET_E_UNKNOWN_PROTOCOL:
    case INET_E_CONNECTION_TIMEOUT:
    case INET_E_CANNOT_CONNECT:
    case INET_E_RESOURCE_NOT_FOUND:
    case INET_E_OBJECT_NOT_FOUND:
    case INET_E_DATA_NOT_AVAILABLE:
        return TRUE;
    default:
        return FALSE;
    }
}


PEModule* Assembly::RaiseModuleResolveEvent(LPCSTR szName, mdFile kFile)
{
    CONTRACT(PEModule *)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
        
    Module* pModule = NULL;

    GCX_COOP();

    struct _gc {
        OBJECTREF AssemblyRef;
        STRINGREF str;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    
    GCPROTECT_BEGIN(gc);
    if ((gc.AssemblyRef = GetExposedObject()) != NULL) 
    {
        MethodDescCallSite onModuleResolve(METHOD__ASSEMBLY__ON_MODULE_RESOLVE, &gc.AssemblyRef);
        gc.str = COMString::NewString(szName);
        ARG_SLOT args[2] = {
            ObjToArgSlot(gc.AssemblyRef),
            ObjToArgSlot(gc.str)
        };
        
        REFLECTMODULEBASEREF ResultingModuleRef = 
            (REFLECTMODULEBASEREF) onModuleResolve.Call_RetOBJECTREF(args);
        
        if (ResultingModuleRef != NULL)
        {
            pModule = (Module*) ResultingModuleRef->GetData();
        }
    }
    GCPROTECT_END();

    if (pModule && ( (!(pModule->IsIntrospectionOnly())) != !(IsIntrospectionOnly()) ))
    {
        COMPlusThrow(kFileLoadException, IDS_CLASSLOAD_MODULE_RESOLVE_INTROSPECTION_MISMATCH);
    }

    if (pModule &&
        (pModule == m_pManifest->LookupFile(kFile)))
    {
        RETURN (PEModule*) pModule->GetFile();
    }

    RETURN NULL;
}



BOOL Assembly::GetResource(LPCSTR szName, DWORD *cbResource,
                              PBYTE *pbInMemoryResource, Assembly** pAssemblyRef,
                              LPCSTR *szFileName, DWORD *dwLocation,
                              StackCrawlMark *pStackMark, BOOL fSkipSecurityCheck,
                              BOOL fSkipRaiseResolveEvent)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    
    DomainAssembly *pAssembly = NULL;
    BOOL result = GetDomainAssembly()->GetResource(szName, cbResource,
                                                   pbInMemoryResource, &pAssembly,
                                                   szFileName, dwLocation, pStackMark, fSkipSecurityCheck,
                                                   fSkipRaiseResolveEvent);
    if (result && pAssemblyRef != NULL && pAssembly!=NULL)
        *pAssemblyRef = pAssembly->GetAssembly();

    return result;
}

BOOL Assembly::IsInstrumented()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FAULT;


    BOOL isInstrumented = false;

    EX_TRY
    {
        FAULT_NOT_FATAL();

        isInstrumented = IsInstrumentedHelper();
    }
    EX_CATCH
    {
        isInstrumented = false;
    }
    EX_END_CATCH(RethrowTerminalExceptions);

    return isInstrumented;
}

BOOL Assembly::IsInstrumentedHelper()
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FAULT;

    if (IsDynamic())
        return false;

    // not only we're not really interested but 
    // IL image might be gone already
    if (!GetManifestFile()->HasNativeImage())
        return false;
    

    LPCUTF8 szZapBBInstr = g_pConfig->GetZapBBInstr();
    LPCUTF8 szAssemblyName = GetSimpleName();

    if (!szZapBBInstr || !szAssemblyName ||
        (*szZapBBInstr == '\0') || (*szAssemblyName == '\0'))
        return false;

    // Convert to unicode so that we can do a case insensitive comparison

    SString instrumentedAssemblyNamesList(SString::Utf8, szZapBBInstr);
    SString assemblyName(SString::Utf8, szAssemblyName);

    const WCHAR *wszInstrumentedAssemblyNamesList = instrumentedAssemblyNamesList.GetUnicode();
    const WCHAR *wszAssemblyName                  = assemblyName.GetUnicode();


    // wszInstrumentedAssemblyNamesList is a space separated list of assembly names. 
    // We need to determine if wszAssemblyName is in this list.
    // If there is a "*" in the list, then all assemblies match.

    const WCHAR * pCur = wszInstrumentedAssemblyNamesList;

    do
    {
        _ASSERTE(pCur[0] != L'\0');
        const WCHAR * pNextSpace = wcschr(pCur, L' ');
        _ASSERTE(pNextSpace == NULL || pNextSpace[0] == L' ');
        
        if (pCur != pNextSpace)
        {
            // pCur is not pointing to a space
            _ASSERTE(pCur[0] != L' ');
            
            if (pCur[0] == L'*' && (pCur[1] == L' ' || pCur[1] == L'\0'))
                return true;

            if (pNextSpace == NULL)
            {
                // We have reached the last name in the list. There are no more spaces.
                return (SString::_wcsicmp(wszAssemblyName, pCur) == 0);
            }
            else
            {
                if (SString::_wcsnicmp(wszAssemblyName, pCur, pNextSpace - pCur) == 0)
                    return true;
            }
        }

        pCur = pNextSpace + 1;
    }
    while (pCur[0] != L'\0');

    return false;    
}


//***********************************************************
// Add a typedef to the runtime TypeDef table of this assembly
//***********************************************************
void Assembly::AddType(
    Module          *pModule,
    mdTypeDef       cl)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    AllocMemTracker amTracker;

    if (pModule->GetAssembly() != this)
    {
        // you cannot add a typedef outside of the assembly to the typedef table
        _ASSERTE(!"Bad usage!");
    }
    m_pClassLoader->AddAvailableClassDontHaveLock(pModule,
                                                  cl,
                                                  &amTracker);
    amTracker.SuppressRelease();
}



//***********************************************************
//
// get the IMetaDataAssemblyEmit for the on disk manifest.
// Note that the pointer returned is AddRefed. It is the caller's
// responsibility to release the reference.
//
//***********************************************************
IMetaDataAssemblyEmit *Assembly::GetOnDiskMDAssemblyEmitter()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    IMetaDataAssemblyEmit *pAssemEmitter = NULL;
    IMetaDataEmit   *pEmitter;
    HRESULT         hr;
    RefClassWriter  *pRCW;

    _ASSERTE(m_pOnDiskManifest);

    pRCW = m_pOnDiskManifest->GetClassWriter();
    _ASSERTE(pRCW);

    // If the RefClassWriter has a on disk emitter, then use it rather than the in-memory emitter.
    pEmitter = pRCW->GetOnDiskEmitter();

    if (pEmitter == NULL)
        pEmitter = m_pOnDiskManifest->GetEmitter();

    _ASSERTE(pEmitter);

    hr = pEmitter->QueryInterface(IID_IMetaDataAssemblyEmit, (void**) &pAssemEmitter);
    if (FAILED(hr))
    {
        COMPlusThrowHR(hr);
    }
    if (pAssemEmitter == NULL)
    {
        // the manifest is not writable
        _ASSERTE(!"Bad usage!");
    }
    return pAssemEmitter;
}


//***********************************************************
//
// prepare saving manifest to disk.
//
//***********************************************************
void Assembly::PrepareSavingManifest(ReflectionModule *pAssemblyModule)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    if (pAssemblyModule)
    {
        // embedded assembly
        m_pOnDiskManifest = pAssemblyModule;
        m_fEmbeddedManifest = true;
    }
    else
    {
        m_fEmbeddedManifest = false;

        StackSString name(SString::Utf8, GetSimpleName());

        // Create the module
        m_pOnDiskManifest = CreateDynamicModule(name);
        // store the fact this on disk manifest is temporary and can be hidden from the user
        m_needsToHideManifestForEmit = TRUE;
    }

    NonVMComHolder<IMetaDataAssemblyEmit> pAssemblyEmit(GetOnDiskMDAssemblyEmitter());

    // Copy assembly metadata to emit scope

    IMetaDataAssemblyImport *pAssemblyImport = GetManifestFile()->GetAssemblyImporter();

    const void          *pbPublicKey;
    ULONG               cbPublicKey;
    ULONG               ulHashAlgId;
    LPWSTR              szName;
    ULONG               chName;
    ASSEMBLYMETADATA    MetaData;
    DWORD               dwAssemblyFlags;

    MetaData.cbLocale = 0;
    MetaData.ulProcessor = 0;
    MetaData.ulOS = 0;
    IfFailThrow(pAssemblyImport->GetAssemblyProps(TokenFromRid(1, mdtAssembly),
                                                  NULL, NULL, NULL,
                                                  NULL, 0, &chName,
                                                  &MetaData, NULL));
    StackSString name;
    szName = name.OpenUnicodeBuffer(chName);

    SString locale;
    MetaData.szLocale = locale.OpenUnicodeBuffer(MetaData.cbLocale);

    SBuffer proc;
    MetaData.rProcessor = (DWORD *) proc.OpenRawBuffer(MetaData.ulProcessor*sizeof(*MetaData.rProcessor));

    SBuffer os;
    MetaData.rOS = (OSINFO *) os.OpenRawBuffer(MetaData.ulOS*sizeof(*MetaData.rOS));

    IfFailThrow(pAssemblyImport->GetAssemblyProps(TokenFromRid(1, mdtAssembly),
                                                  &pbPublicKey, &cbPublicKey, &ulHashAlgId,
                                                  szName, chName, &chName,
                                                  &MetaData, &dwAssemblyFlags));

    mdAssembly ad;
    IfFailThrow(pAssemblyEmit->DefineAssembly(pbPublicKey, cbPublicKey, ulHashAlgId,
                                              szName, &MetaData, dwAssemblyFlags, &ad));

    SafeComHolder<IMetaDataImport> pImport;
    IfFailThrow(pAssemblyEmit->QueryInterface(IID_IMetaDataImport, (void**)&pImport));
    ULONG cExistingName = 0;
    if (FAILED(pImport->GetScopeProps(NULL, 0, &cExistingName, NULL)) || cExistingName == 0)
    {
        SafeComHolder<IMetaDataEmit> pEmit;
        IfFailThrow(pAssemblyEmit->QueryInterface(IID_IMetaDataEmit, (void**)&pEmit));
        IfFailThrow(pEmit->SetModuleProps(szName));
    }

    name.CloseBuffer();
    locale.CloseBuffer();
    proc.CloseRawBuffer();
    os.CloseRawBuffer();

    RETURN;
}   // Assembly::PrepareSavingManifest


//***********************************************************
//
// add a file name to the file list of this assembly. On disk only.
//
//***********************************************************
mdFile Assembly::AddFileList(__in LPWSTR wszFileName)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    IMetaDataAssemblyEmit *pAssemEmitter = GetOnDiskMDAssemblyEmitter();
    HRESULT         hr = NOERROR;
    mdFile          fl;

    // File records are now emitted at Module create time. Do a check here
    // to make sure it works out OK.



    // Define File.
    IfFailGo( pAssemEmitter->DefineFile(
        wszFileName,                // [IN] Name of the file.
        0,                          // [IN] Hash Blob.
        0,                          // [IN] Count of bytes in the Hash Blob.
        0,                          // [IN] Flags.
        &fl) );                     // [OUT] Returned File token.

ErrExit:
    if (pAssemEmitter)
        pAssemEmitter->Release();

    if (FAILED(hr))
        COMPlusThrowHR(hr);

    return fl;
}   // Assembly::AddFileList


//***********************************************************
//
// Set the hash value on a file table entry.
//
//***********************************************************
void Assembly::SetHashValue(mdFile tkFile, __in LPWSTR wszFullFileName)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    SafeComHolder<IMetaDataAssemblyEmit> pAssemEmitter(GetOnDiskMDAssemblyEmitter());

    // Get the hash value.
    SBuffer buffer;
    PEImageHolder map(PEImage::OpenImage(StackSString(wszFullFileName)));
    map->ComputeHash(GetHashAlgId(), buffer);

    // Set the hash blob.
    IfFailThrow( pAssemEmitter->SetFileProps(
        tkFile,                 // [IN] File Token.
        buffer,                 // [IN] Hash Blob.
        buffer.GetSize(),       // [IN] Count of bytes in the Hash Blob.
        (DWORD) -1));           // [IN] Flags.

}   // Assembly::SetHashValue


//***********************************************************
// Add an assembly to the assemblyref list. pAssemEmitter specifies where
// the AssemblyRef is emitted to.
//***********************************************************
mdAssemblyRef Assembly::AddAssemblyRef(Assembly *refedAssembly, IMetaDataAssemblyEmit *pAssemEmitter, BOOL fUsePublicKeyToken)
{
    CONTRACT(mdAssemblyRef)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(refedAssembly));
        PRECONDITION(CheckPointer(pAssemEmitter, NULL_OK));
        POSTCONDITION(!IsNilToken(RETVAL));
        POSTCONDITION(TypeFromToken(RETVAL) == mdtAssemblyRef);
    }
    CONTRACT_END;

    SafeComHolder<IMetaDataAssemblyEmit> emitHolder;
    if (pAssemEmitter == NULL)
    {
        pAssemEmitter = GetOnDiskMDAssemblyEmitter();
        emitHolder.Assign(pAssemEmitter);
    }

    AssemblySpec spec;
    refedAssembly->GetAssemblySpec(&spec);

    mdAssemblyRef ar;
    IfFailThrow(spec.EmitToken(pAssemEmitter, &ar, fUsePublicKeyToken));

    RETURN ar;
}   // Assembly::AddAssemblyRef


//***********************************************************
// Initialize an AssemblySpec from the Assembly data.
//***********************************************************
void Assembly::GetAssemblySpec(AssemblySpec *pSpec)
{
    CONTRACT_VOID
{
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    pSpec->InitializeSpec(GetManifestFile());

    RETURN;
} // Assembly::GetAssemblySpec()

//***********************************************************
// add a Type name to COMType table. On disk only.
//***********************************************************
mdExportedType Assembly::AddExportedType(__in LPWSTR wszExportedType, mdToken tkImpl, mdToken tkTypeDef, CorTypeAttr flags)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END


    IMetaDataAssemblyEmit *pAssemEmitter = GetOnDiskMDAssemblyEmitter();
    HRESULT         hr = NOERROR;
    mdExportedType       ct;
    mdTypeDef       tkType = tkTypeDef;

    if (RidFromToken(tkTypeDef) == 0)
        tkType = mdTypeDefNil;

    IfFailGo( pAssemEmitter->DefineExportedType(
        wszExportedType,            // [IN] Name of the COMType.
        tkImpl,                     // [IN] mdFile or mdAssemblyRef that provides the ExportedType.
        tkType,                     // [IN] TypeDef token within the file.
        flags,                      // [IN] Flags.
        &ct) );                     // [OUT] Returned ExportedType token.

ErrExit:
    if (pAssemEmitter)
        pAssemEmitter->Release();

    if (FAILED(hr))
    {
        COMPlusThrowHR(hr);
    }
    return ct;
}   // Assembly::AddExportedType



//***********************************************************
// add an entry to ManifestResource table for a stand alone managed resource. On disk only.
//***********************************************************
void Assembly::AddStandAloneResource(LPCWSTR wszName, __in LPWSTR wszDescription, __in LPWSTR wszMimeType, LPCWSTR wszFileName, LPCWSTR wszFullFileName, int iAttribute)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    SafeComHolder<IMetaDataAssemblyEmit> pAssemEmitter(GetOnDiskMDAssemblyEmitter());
    mdFile          tkFile;
    mdManifestResource mr;
    SBuffer         hash;

    // Get the hash value;
    if (GetHashAlgId())
    {
        PEImageHolder pImage(PEImage::OpenImage(StackSString(wszFullFileName)));
        pImage->ComputeHash(GetHashAlgId(), hash);
    }

    IfFailThrow( pAssemEmitter->DefineFile(
        wszFileName,            // [IN] Name of the file.
        hash,                   // [IN] Hash Blob.
        hash.GetSize(),         // [IN] Count of bytes in the Hash Blob.
        ffContainsNoMetaData,   // [IN] Flags.
        &tkFile) );             // [OUT] Returned File token.


    IfFailThrow( pAssemEmitter->DefineManifestResource(
        wszName,                // [IN] Name of the resource.
        tkFile,                 // [IN] mdFile or mdAssemblyRef that provides the resource.
        0,                      // [IN] Offset to the beginning of the resource within the file.
        iAttribute,             // [IN] Flags.
        &mr) );                 // [OUT] Returned ManifestResource token.

    RETURN;
}   // Assembly::AddStandAloneResource


//***********************************************************
// Save security permission requests.
//***********************************************************
void Assembly::SavePermissionRequests(U1ARRAYREF orRequired,
                                      U1ARRAYREF orOptional,
                                      U1ARRAYREF orRefused)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END


    HRESULT              hr = S_OK;
    mdAssembly tkAssembly = 0x20000001;
    IMetaDataEmitHelper *pEmitHelper = NULL;
    IMetaDataAssemblyEmit *pAssemEmitter = GetOnDiskMDAssemblyEmitter();

    _ASSERTE( pAssemEmitter );
    IfFailGo( pAssemEmitter->QueryInterface(IID_IMetaDataEmitHelper, (void**)&pEmitHelper) );


    if (orRequired != NULL)
        IfFailGo(pEmitHelper->AddDeclarativeSecurityHelper(tkAssembly,
                                                           dclRequestMinimum,
                                                           orRequired->GetDataPtr(),
                                                           orRequired->GetNumComponents(),
                                                           NULL));

    if (orOptional != NULL)
        IfFailGo(pEmitHelper->AddDeclarativeSecurityHelper(tkAssembly,
                                                           dclRequestOptional,
                                                           orOptional->GetDataPtr(),
                                                           orOptional->GetNumComponents(),
                                                           NULL));

    if (orRefused != NULL)
        IfFailGo(pEmitHelper->AddDeclarativeSecurityHelper(tkAssembly,
                                                           dclRequestRefuse,
                                                           orRefused->GetDataPtr(),
                                                           orRefused->GetNumComponents(),
                                                           NULL));

 ErrExit:
    if (pEmitHelper)
        pEmitHelper->Release();
    if (pAssemEmitter)
        pAssemEmitter->Release();
    if (FAILED(hr))
    {
        COMPlusThrowHR(hr);
    }
}


//***********************************************************
// Allocate space for a strong name signature in the manifest
//***********************************************************
HRESULT Assembly::AllocateStrongNameSignature(ICeeFileGen  *pCeeFileGen,
                                              HCEEFILE      ceeFile)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT     hr;
    HCEESECTION TData;
    DWORD       dwDataOffset;
    DWORD       dwDataLength;
    DWORD       dwDataRVA;
    VOID       *pvBuffer;
    const void *pbPublicKey;
    ULONG       cbPublicKey;

    // Calling strong name routines for the first time can cause a load library,
    // potentially leaving us with a deadlock if we're in cooperative GC mode.
    // Switch to pre-emptive mode and touch a harmless strong name routine to
    // get any load library calls out of the way without having to switch modes
    // continuously through this routine (and the two support routines that
    // follow).
    if (GetThread()->PreemptiveGCDisabled()) {
        GCX_PREEMP();
        StrongNameErrorInfo();
    }

    // Determine size of signature blob.

    GetManifestImport()->GetAssemblyProps(TokenFromRid(1, mdtAssembly),
                                          &pbPublicKey, &cbPublicKey, NULL,
                                          NULL, NULL, NULL);

    if (!StrongNameSignatureSize((BYTE *) pbPublicKey, cbPublicKey, &dwDataLength)) {
        hr = StrongNameErrorInfo();
        return hr;
    }

    // Allocate space for the signature in the text section and update the COM+
    // header to point to the space.
    IfFailRet(pCeeFileGen->GetIlSection(ceeFile, &TData));
    IfFailRet(pCeeFileGen->GetSectionDataLen(TData, &dwDataOffset));
    IfFailRet(pCeeFileGen->GetSectionBlock(TData, dwDataLength, 4, &pvBuffer));
    IfFailRet(pCeeFileGen->GetMethodRVA(ceeFile, dwDataOffset, &dwDataRVA));
    IfFailRet(pCeeFileGen->SetStrongNameEntry(ceeFile, dwDataLength, dwDataRVA));

    return S_OK;
}


//***********************************************************
// Strong name sign a manifest already persisted to disk
//***********************************************************
HRESULT Assembly::SignWithStrongName(__in LPWSTR wszFileName)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT hr = S_OK;

    // If we're going to do a full signing we have a key pair either
    // in a key container or provided directly in a byte array.

    switch (m_eStrongNameLevel) {
    case SN_FULL_KEYPAIR_IN_ARRAY:
        if (!StrongNameSignatureGeneration(wszFileName, NULL, m_pbStrongNameKeyPair, m_cbStrongNameKeyPair, NULL, NULL))
            hr = StrongNameErrorInfo();
        break;

    case SN_FULL_KEYPAIR_IN_CONTAINER:
        if (!StrongNameSignatureGeneration(wszFileName, m_pwStrongNameKeyContainer, NULL, 0, NULL, NULL))
            hr = StrongNameErrorInfo();
        break;

    default:
        break;
    }

    return hr;
}


//***********************************************************
// save the manifest to disk!
//***********************************************************
void Assembly::SaveManifestToDisk(__in LPWSTR wszFileName, int entrypoint, int fileKind, DWORD corhFlags, DWORD peFlags)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END


    HRESULT         hr = NOERROR;
    HCEEFILE        ceeFile = NULL;
    ICeeFileGen     *pCeeFileGen = NULL;
    RefClassWriter  *pRCW;
    IMetaDataEmit   *pEmitter;

    _ASSERTE( m_fEmbeddedManifest == false );

    pRCW = m_pOnDiskManifest->GetClassWriter();
    _ASSERTE(pRCW);

    IfFailGo( pRCW->EnsureCeeFileGenCreated(corhFlags, peFlags) );

    pCeeFileGen = pRCW->GetCeeFileGen();
    ceeFile = pRCW->GetHCEEFILE();
    _ASSERTE(ceeFile && pCeeFileGen);

    //Emit the MetaData
    pEmitter = m_pOnDiskManifest->GetClassWriter()->GetEmitter();
    IfFailGo( pCeeFileGen->EmitMetaDataEx(ceeFile, pEmitter) );

    // Allocate space for a strong name signature if a public key was supplied
    // (this doesn't strong name the assembly, but it makes it possible to do so
    // as a post processing step).
    if (IsStrongNamed())
        IfFailGo(AllocateStrongNameSignature(pCeeFileGen, ceeFile));

    IfFailGo( pCeeFileGen->SetOutputFileName(ceeFile, wszFileName) );

    // the entryPoint for an assembly is a tkFile token if exist.
    if (RidFromToken(entrypoint) != mdTokenNil)
        IfFailGo( pCeeFileGen->SetEntryPoint(ceeFile, entrypoint) );
    if (fileKind == Dll)
    {
        pCeeFileGen->SetDllSwitch(ceeFile, true);
    }
    else
    {
        // should have a valid entry point for applications
        if (fileKind == WindowApplication)
        {
            IfFailGo( pCeeFileGen->SetSubsystem(ceeFile, IMAGE_SUBSYSTEM_WINDOWS_GUI, CEE_IMAGE_SUBSYSTEM_MAJOR_VERSION, CEE_IMAGE_SUBSYSTEM_MINOR_VERSION) );
        }
        else
        {
            _ASSERTE(fileKind == ConsoleApplication);
            IfFailGo( pCeeFileGen->SetSubsystem(ceeFile, IMAGE_SUBSYSTEM_WINDOWS_CUI, CEE_IMAGE_SUBSYSTEM_MAJOR_VERSION, CEE_IMAGE_SUBSYSTEM_MINOR_VERSION) );
        }

    }

    //Generate the CeeFile
    IfFailGo(pCeeFileGen->GenerateCeeFile(ceeFile) );

    // Strong name sign the resulting assembly if required.
    if (IsStrongNamed())
        IfFailGo(SignWithStrongName(wszFileName));

    // now release the m_pOnDiskManifest
ErrExit:
    pRCW->DestroyCeeFileGen();

    // we keep the on disk manifest so that the GetModules code can skip over this ad-hoc module when modules are enumerated.
    // Need to see if we can remove the creation of this module alltogether
    //m_pOnDiskManifest = NULL;

    if (FAILED(hr))
    {
        if (HRESULT_FACILITY(hr) == FACILITY_WIN32)
        {
            if (IsWin32IOError(HRESULT_CODE(hr)))
            {
                COMPlusThrowHR(COR_E_IO);
            }
            else
            {
                COMPlusThrowHR(hr);
            }
        }
        if (hr == CEE_E_CVTRES_NOT_FOUND)
            COMPlusThrow(kIOException, L"Argument_cvtres_NotFound");
        COMPlusThrowHR(hr);
    }
}   // Assembly::SaveManifestToDisk

//***********************************************************
// Adding a module with file name wszFileName into the file list
//***********************************************************
void Assembly::AddFileToInMemoryFileList(__in LPWSTR wszFileName, Module *pModule)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END


    IMetaDataAssemblyEmit   *pAssemEmitter = NULL;
    IMetaDataEmit           *pEmitter;
    mdFile                  tkFile;
    LPCSTR                  szFileName;
    HRESULT                 hr;
    CQuickBytes             qbLC;

    pEmitter = m_pManifest->GetEmitter();
    _ASSERTE(pEmitter);

    IfFailGo( pEmitter->QueryInterface(IID_IMetaDataAssemblyEmit, (void**) &pAssemEmitter) );
    if (pAssemEmitter == NULL)
    {
        // the manifest is not writable
        goto ErrExit;
    }

    // Define an entry in the in-memory file list for this module
    IfFailGo( pAssemEmitter->DefineFile(
        wszFileName,                // [IN] Name of the file.
        NULL,                       // [IN] Hash Blob.
        0,                          // [IN] Count of bytes in the Hash Blob.
        ffContainsMetaData,         // [IN] Flags.
        &tkFile) );                 // [OUT] Returned File token.

    m_pManifest->GetMDImport()->GetFileProps(tkFile, &szFileName, NULL, NULL, NULL);

    // Add to  table
    m_pAllowedFiles->InsertValue(szFileName, (HashDatum)(size_t)tkFile, TRUE);
    
    // Need to perform case insensitive hashing as well.
    {
        UTF8_TO_LOWER_CASE(szFileName, qbLC);
        szFileName = (LPUTF8) qbLC.Ptr();
    }

    // insert the value into manifest's look up table.
    m_pAllowedFiles->InsertValue(szFileName, (HashDatum)(size_t)tkFile, TRUE);

    // Now make file token associate with the loaded module
    m_pManifest->StoreFileThrowing(tkFile, pModule);

ErrExit:
    if (pAssemEmitter)
        pAssemEmitter->Release();

    if (FAILED(hr))
        COMPlusThrowHR(hr);
}   // AddFileToInMemoryFileList

HRESULT STDMETHODCALLTYPE
GetAssembliesByName(LPCWSTR  szAppBase,
                    LPCWSTR  szPrivateBin,
                    LPCWSTR  szAssemblyName,
                    IUnknown *ppIUnk[],
                    ULONG    cMax,
                    ULONG    *pcAssemblies)
{
    CONTRACTL
    {
        NOTHROW;
        MODE_PREEMPTIVE;
        GC_TRIGGERS;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT hr = S_OK;

    if (g_fEEInit) {
        // Cannot call this during EE startup
        return MSEE_E_ASSEMBLYLOADINPROGRESS;
    }

    if (!(szAssemblyName && ppIUnk && pcAssemblies))
        return E_POINTER;

    AppDomain *pDomain = NULL;

    BEGIN_EXTERNAL_ENTRYPOINT(&hr)
    {
        if(szAppBase || szPrivateBin) {
            MethodDescCallSite createDomainEx(METHOD__APP_DOMAIN__CREATE_DOMAINEX);
            struct _gc {
                STRINGREF pFriendlyName;
                STRINGREF pAppBase;
                STRINGREF pPrivateBin;
            } gc;
            ZeroMemory(&gc, sizeof(gc));

            GCPROTECT_BEGIN(gc);
            gc.pFriendlyName = COMString::NewString(L"GetAssembliesByName");

            if(szAppBase)
            {
                gc.pAppBase = COMString::NewString(szAppBase);
            }
            
            if(szPrivateBin)
            {
                gc.pPrivateBin = COMString::NewString(szPrivateBin);
            }

            ARG_SLOT args[5] = 
            {
                ObjToArgSlot(gc.pFriendlyName),
                NULL,
                ObjToArgSlot(gc.pAppBase),
                ObjToArgSlot(gc.pPrivateBin),
                0,
            };
            APPDOMAINREF pDom = (APPDOMAINREF) createDomainEx.Call_RetOBJECTREF(args);
            if (pDom == NULL)
            {
                hr = E_FAIL;
            }
            else 
            {
                Context *pContext = CRemotingServices::GetServerContextForProxy((OBJECTREF) pDom);
                _ASSERTE(pContext);
                pDomain = pContext->GetDomain();
            }

            GCPROTECT_END();
        }
        else
            pDomain = SystemDomain::System()->DefaultDomain();
    }

    Assembly *pFoundAssembly;
    if (SUCCEEDED(hr)) {
        pFoundAssembly = pDomain->LoadAssemblyHelper(szAssemblyName,
                                                     NULL);
        if (SUCCEEDED(hr)) {
            if (cMax < 1)
                hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            else {
                ppIUnk[0] = (IUnknown *)pFoundAssembly->GetManifestAssemblyImporter();
                ppIUnk[0]->AddRef();
            }
            *pcAssemblies = 1;
        }
    }

    END_EXTERNAL_ENTRYPOINT;

    return hr;
}// Used by the IMetadata API's to access an assemblies metadata.



void Assembly::SetSharingProperties(IAssemblyBindingClosure* pContext)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    _ASSERTE(m_pSharingProps == NULL);

    // Now allocate and initialize the sharing properties structure
    NewHolder<AssemblySharingContext> p(new AssemblySharingContext);

    p->shareCount = 0;
    p->pContext=pContext;
    if(pContext)
        pContext->AddRef();

    m_pSharingProps = p.Extract();

    RETURN;
}



BOOL Assembly::GetSharingProps(IAssemblyBindingClosure** ppContext)
{
    CONTRACT(BOOL)
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACT_END;

    if (m_pSharingProps == NULL )
    {
        *ppContext = NULL;
        RETURN FALSE;
    }
    else
    {
        *ppContext = m_pSharingProps->pContext;
        if (*ppContext)
            (*ppContext)->AddRef();
        RETURN TRUE;
    }
}


// The shared module list is effectively an extension of the shared domain assembly hash table.
// It is the canonical list and aribiter of modules loaded from this assembly by any app domain.
// Modules are stored here immediately on creating (to prevent duplicate creation), as opposed to
// in the rid map, where they are only placed upon load completion.

void Assembly::IncrementShareCount()
{
    WRAPPER_CONTRACT;

    FastInterlockIncrement(&m_pSharingProps->shareCount);
}

void Assembly::DecrementShareCount()
{
    WRAPPER_CONTRACT;

    FastInterlockDecrement(&m_pSharingProps->shareCount);
}

BOOL Assembly::CanBeShared(DomainAssembly *pDomainAssembly)
{
    CONTRACTL
    {
        PRECONDITION(CheckPointer(pDomainAssembly, NULL_OK));
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    LOG((LF_CODESHARING,
         LL_INFO100,
         "Checking if we can share: \"%S\" in domain 0x%x.\n",
         GetDebugName(), pDomainAssembly == NULL ? 0 : pDomainAssembly->GetAppDomain()));

    STRESS_LOG2(LF_CODESHARING, LL_INFO1000,"Checking whether DomainAssembly %08x is compatible with Assembly %08x",
        pDomainAssembly,this);

    // We must always share the same system assemblies
    if (IsSystem())
    {
        STRESS_LOG0(LF_CODESHARING, LL_INFO1000,"System assembly - sharing");
        return TRUE;
    }

    //
    // If we weren't able to compute sharing properties, err on the side of caution
    // & just fail.
    //

    if (m_pSharingProps == NULL)
    {
        STRESS_LOG0(LF_CODESHARING, LL_INFO1000,"Assembly has no info recoded - not sharing");
        return FALSE;
    }

    if (pDomainAssembly == NULL)
    {
        return FALSE;
    }

    if ((pDomainAssembly->GetDebuggerInfoBits()&~(DACF_PDBS_COPIED|DACF_IGNORE_PDBS))
        != (m_debuggerFlags&~(DACF_PDBS_COPIED|DACF_IGNORE_PDBS)))
    {
        LOG((LF_CODESHARING,
                LL_INFO100,
                "We can't share it, desired debugging flags %x are different than %x\n",
                pDomainAssembly->GetDebuggerInfoBits(), (m_debuggerFlags&~(DACF_PDBS_COPIED|DACF_IGNORE_PDBS))));
        STRESS_LOG2(LF_CODESHARING, LL_INFO100,"Flags diff= %08x [%08x/%08x]",pDomainAssembly->GetDebuggerInfoBits(),
                m_debuggerFlags);
        return FALSE;
    }

    PEAssembly * pDomainAssemblyFile = pDomainAssembly->GetFile();
    if(pDomainAssemblyFile == NULL)
    {
        return FALSE;
    }

    IAssemblyBindingClosure* pContext;
    if (!GetSharingProps(&pContext)||pContext==NULL)
    {
        STRESS_LOG1(LF_CODESHARING, LL_INFO1000,"No context 1 - status=%d",pDomainAssemblyFile->IsSystem());        
        return pDomainAssemblyFile->IsSystem();
    }
    IAssemblyBindingClosure* pCurrentContext=pDomainAssembly->GetAssemblyBindingClosure(FALSE);
    if(pCurrentContext==NULL)
    {
        STRESS_LOG1(LF_CODESHARING, LL_INFO1000,"No context 2 - status=%d",pDomainAssemblyFile->IsSystem());        
        return pDomainAssemblyFile->IsSystem();
    }

    HRESULT hr=pContext->IsEqual(pCurrentContext);
    IfFailThrow(hr);
    if (hr!=S_OK)
    {
        STRESS_LOG1(LF_CODESHARING, LL_INFO1000,"Closure comparison returned %08x - not sharing",hr);        
        return FALSE;
    }

    LOG((LF_CODESHARING, LL_INFO100, "We can share it : \"%S\"\n", GetDebugName()));
    STRESS_LOG0(LF_CODESHARING, LL_INFO1000,"Everything is fine - sharing");                
    return TRUE;
}

BOOL Assembly::AllowUntrustedCaller()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    return ModuleSecurityDescriptor::GetModuleSecurityDescriptor(this)->IsAPTCA();
}



BOOL Assembly::IsNoStringInterning()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END
    

    if (!m_fCheckedForNoStringInterning)
    {
        // Default is string interning
        m_fNoStringInterning= FALSE;

        Module* pModule; pModule = this->GetManifestModule();
        _ASSERTE(pModule);

        IMDInternalImport *mdImport = this->GetManifestImport();        
        _ASSERTE(mdImport);
  
        mdToken token;
        mdImport->GetAssemblyFromScope(&token);

        const BYTE *        pVal;                 
        ULONG               cbVal;                 
        
        HRESULT hr = mdImport->GetCustomAttributeByName(token,
                        COMPILATIONRELAXATIONS_TYPE,
                        (const void**)&pVal, &cbVal);

        // Parse the attribute
        if (hr == S_OK)
        {
            // Skip the CA type prefix.        
            pVal += 2;

            // Get Flags
            DWORD flags = *((UNALIGNED DWORD*)(pVal));
            
            if (flags & CompilationRelaxations_NoStringInterning)
            {                    
                m_fNoStringInterning = TRUE;   
            }                
        }                                                             

        m_fCheckedForNoStringInterning = TRUE;
    }
                
    return m_fNoStringInterning;
}

BOOL Assembly::IsRuntimeWrapExceptions()
{
    CONTRACTL
    {
        THROWS;
        if (m_fCheckedForRuntimeWrapExceptions) GC_NOTRIGGER; else GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    if (!m_fCheckedForRuntimeWrapExceptions)
    {
        Module* pModule = this->GetManifestModule();
        _ASSERTE(pModule);

        IMDInternalImport *mdImport = this->GetManifestImport();        
        _ASSERTE(mdImport);
  
        mdToken token;
        mdImport->GetAssemblyFromScope(&token);

        const BYTE *        pVal;                 
        ULONG               cbVal;                 
        
        HRESULT hr = mdImport->GetCustomAttributeByName(token,
                        RUNTIMECOMPATIBILITY_TYPE,
                        (const void**)&pVal, &cbVal);

        // Parse the attribute
        if (hr == S_OK)
        {
            CustomAttributeParser ca(pVal, cbVal);

            // First, the void constructor:
            IfFailGo(ParseKnownCaArgs(ca, NULL, 0));

            // Then, find the named argument

            CaNamedArg namedArgs[1];
            namedArgs[0].InitBoolField("WrapNonExceptionThrows");

            IfFailGo(ParseKnownCaNamedArgs(ca, namedArgs, lengthof(namedArgs)));

            if (namedArgs[0].val.boolean)
                m_fRuntimeWrapExceptions = TRUE;   

ErrExit:    ;       // just one shot to override the default.
        }                                                             

        m_fCheckedForRuntimeWrapExceptions = TRUE;
    }
                
    return m_fRuntimeWrapExceptions;
}

void DECLSPEC_NORETURN Assembly::ThrowTypeLoadException(LPCUTF8 pszFullName, UINT resIDWhy)
{
    WRAPPER_CONTRACT;
    ThrowTypeLoadException(NULL, pszFullName, NULL,
                           resIDWhy);
}

void DECLSPEC_NORETURN Assembly::ThrowTypeLoadException(LPCUTF8 pszNameSpace, LPCUTF8 pszTypeName,
                                                        UINT resIDWhy)
{
    WRAPPER_CONTRACT;
    ThrowTypeLoadException(pszNameSpace, pszTypeName, NULL,
                           resIDWhy);

}

void DECLSPEC_NORETURN Assembly::ThrowTypeLoadException(NameHandle *pName, UINT resIDWhy)
{
    STATIC_CONTRACT_THROWS;

    if (pName->GetName()) {
        ThrowTypeLoadException(pName->GetNameSpace(),
                               pName->GetName(), 
                               NULL, 
                               resIDWhy);
    }
    else
        ThrowTypeLoadException(pName->GetTypeModule()->GetMDImport(),
                               pName->GetTypeToken(),
                               resIDWhy);

}

void DECLSPEC_NORETURN Assembly::ThrowTypeLoadException(IMDInternalImport *pInternalImport,
                                                        mdToken token,
                                                        UINT resIDWhy)
{
    WRAPPER_CONTRACT;
    ThrowTypeLoadException(pInternalImport, token, NULL, resIDWhy);
}

void DECLSPEC_NORETURN Assembly::ThrowTypeLoadException(IMDInternalImport *pInternalImport,
                                                        mdToken token,
                                                        LPCUTF8 pszFieldOrMethodName,
                                                        UINT resIDWhy)
{
    STATIC_CONTRACT_THROWS;
    char pszBuff[32];
    LPCUTF8 pszClassName = (LPCUTF8)pszBuff;
    LPCUTF8 pszNameSpace = "Invalid_Token";

    if(pInternalImport->IsValidToken(token))
    {
        switch (TypeFromToken(token)) {
        case mdtTypeRef:
            pInternalImport->GetNameOfTypeRef(token, &pszNameSpace, &pszClassName);
            break;
        case mdtTypeDef:
            pInternalImport->GetNameOfTypeDef(token, &pszClassName, &pszNameSpace);
            break;
        case mdtTypeSpec:

            // If you see this assert, you need to make sure the message for
            // this resID is appropriate for TypeSpecs
            _ASSERTE((resIDWhy == IDS_CLASSLOAD_GENERAL) ||
                     (resIDWhy == IDS_CLASSLOAD_TYPESPEC));

            resIDWhy = IDS_CLASSLOAD_TYPESPEC;
        }
    }
    else
        sprintf_s(pszBuff, sizeof(pszBuff), "0x%8.8X", token);

    ThrowTypeLoadException(pszNameSpace, pszClassName,
                           pszFieldOrMethodName, resIDWhy);
    }



void DECLSPEC_NORETURN Assembly::ThrowTypeLoadException(LPCUTF8 pszNameSpace,
                                                        LPCUTF8 pszTypeName,
                                                        LPCUTF8 pszMethodName,
                                                        UINT resIDWhy)
{
    STATIC_CONTRACT_THROWS;

    StackSString displayName;
    GetDisplayName(displayName);

    ::ThrowTypeLoadException(pszNameSpace, pszTypeName, displayName,
                             pszMethodName, resIDWhy);
}

void DECLSPEC_NORETURN Assembly::ThrowBadImageException(LPCUTF8 pszNameSpace,
                                                        LPCUTF8 pszTypeName,
                                                        UINT resIDWhy)
{
    STATIC_CONTRACT_THROWS;

    StackSString displayName;
    GetDisplayName(displayName);

    StackSString fullName;
    fullName.MakeFullNamespacePath(SString(SString::Utf8, pszNameSpace),
                                   SString(SString::Utf8, pszTypeName));

    COMPlusThrowHR(COR_E_BADIMAGEFORMAT, resIDWhy, fullName, displayName);
}



#endif // #ifndef DACCESS_COMPILE




void Assembly::EnsureActive()
{
#ifndef DACCESS_COMPILE
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    GetDomainAssembly()->EnsureActive();
#endif
}

CHECK Assembly::CheckActivated()
{
#ifndef DACCESS_COMPILE
    WRAPPER_CONTRACT;

    CHECK(GetDomainAssembly()->CheckActivated());
#endif
    CHECK_OK;
}



#ifdef DACCESS_COMPILE

void
Assembly::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    DAC_ENUM_DTHIS();
    EMEM_OUT(("MEM: %p Assembly\n", PTR_HOST_TO_TADDR(this)));

    if (m_pDomain.IsValid())
    {
        m_pDomain->EnumMemoryRegions(flags, true);
    }
    if (m_pClassLoader.IsValid())
    {
        m_pClassLoader->EnumMemoryRegions(flags);
    }
    if (m_pManifest.IsValid())
    {
        m_pManifest->EnumMemoryRegions(flags, true);
    }
    if (m_pManifestFile.IsValid())
    {
        m_pManifestFile->EnumMemoryRegions(flags);
    }
}

#endif

// -- eof --
