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
// File: CEELOAD.CPP
//

// CEELOAD reads in the PE file format using LoadLibrary
// ===========================================================================

#include "common.h"

#include "array.h"
#include "ceeload.h"
#include "hash.h"
#include "vars.hpp"
#include "reflectclasswriter.h"
#include "method.hpp"
#include "stublink.h"
#include "security.h"
#include "cgensys.h"
#include "excep.h"
#include "dbginterface.h"
#include "dllimport.h"
#include "eeprofinterfaces.h"
#include "perfcounters.h"
#include "encee.h"
#include "jitinterface.h"
#include "eeconfig.h"
#include "dllimportcallback.h"
#include "contractimpl.h"
#include "typehash.h"
#include "instmethhash.h"
#include "virtualcallstub.h"
#include "typestring.h"
#include "stringliteralmap.h"
#include "eventtrace.h"
#include "securitytransparentassembly.h"
#include "prettyprintsig.h"



#include "perflog.h"
#include "timeline.h"
#include "memoryreport.h"
#include "ecall.h"
#include "objectclone.h"
#include "constrainedexecutionregion.h"
#include "typekey.h"
#include "peimagelayout.inl"

#define COR_VTABLE_PTRSIZED     COR_VTABLE_32BIT
#define COR_VTABLE_NOT_PTRSIZED COR_VTABLE_64BIT

// Hash table parameter of available classes (name -> module/class) hash
#define AVAILABLE_CLASSES_HASH_BUCKETS 1024
#define PARAMTYPES_HASH_BUCKETS 23
#define PARAMMETHODS_HASH_BUCKETS 11

#define NGEN_STATICS_ALLCLASSES_WERE_LOADED -1

#ifdef _X86_ 
extern "C" VOID __stdcall IJWNOADThunkJumpTarget(void);
#endif




#ifndef DACCESS_COMPILE 

HRESULT STDMETHODCALLTYPE CreateICeeGen(REFIID riid, void **pCeeGen);


// {F5398690-98FE-11d2-9C56-00A0C9B7CC45}
const GUID IID_ICorReflectionModule =
{ 0xf5398690, 0x98fe, 0x11d2, { 0x9c, 0x56, 0x0, 0xa0, 0xc9, 0xb7, 0xcc, 0x45 } };





// ===========================================================================
// Module
// ===========================================================================

#ifdef _X86_
#define FixupVTableSlot FixupVTableSlotX86
#endif

void __cdecl FixupVTableSlot();

//---------------------------------------------------------------------------------------------------
// This wrapper just invokes the real initialization inside a try/hook.
//---------------------------------------------------------------------------------------------------
void Module::DoInit(AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("Module");

    if (CORProfilerTrackModuleLoads())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->ModuleLoadStarted((ThreadID) GetThread(), (ModuleID) this);
    }
    // Need TRY/HOOK instead of holder so we can get HR of exception thrown for profiler callback
    EX_TRY
    {
        Initialize(pamTracker);
    }
    EX_HOOK
    {
        if (CORProfilerTrackModuleLoads())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ModuleLoadFinished((ThreadID) GetThread(),
                                                                 (ModuleID) this, GET_EXCEPTION()->GetHR());
        }
    }
    EX_END_HOOK;
}

#if PROFILING_SUPPORTED 
void Module::NotifyProfilerLoadFinished(HRESULT hr)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END;

    // Note that in general we wil reuse shared modules.  So we need to make sure we only notify
    // the profiler once.

    if (FastInterlockCompareExchange(&m_profilerNotified, 1, 0) == 0)
    {
    
        if (CORProfilerTrackModuleLoads())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ModuleLoadFinished((ThreadID) GetThread(), (ModuleID) this, hr);

            if (SUCCEEDED(hr))
            {
                GetISymUnmanagedReader();
                g_profControlBlock.pProfInterface->ModuleAttachedToAssembly((ThreadID) GetThread(), (ModuleID) this,
                                                                            (AssemblyID) m_pAssembly);
            }
        }

        if (CORProfilerTrackAssemblyLoads() && IsManifest())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->AssemblyLoadFinished((ThreadID) GetThread(), (AssemblyID) m_pAssembly, hr);
        }
    }
}
#endif // PROFILING_SUPPORTED


// Module initialization occurs in two phases: the constructor phase and the Initialize phase.
//
// The constructor phase initializes just enough so that Destruct() can be safely called.
// It cannot throw or fail.
//
Module::Module(Assembly *pAssembly, mdFile moduleRef, PEFile *file)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        FORBID_FAULT;
    }
    CONTRACTL_END

    PREFIX_ASSUME(pAssembly != NULL);

    m_pAssembly = pAssembly;
    m_moduleRef = moduleRef;
    m_file      = file;
    m_dwTransientFlags = CLASSES_FREED;


    m_pISymUnmanagedReaderLock = NULL;
    m_dwDebuggerJMCProbeCount  = 0;


#ifdef PROFILING_SUPPORTED_DATA 
    m_profilerNotified         = 0;
#endif

    m_pCerPrepInfo              = NULL;
    m_pCerCrst                  = NULL;

    m_pRVAOverrides             = NULL;
    m_pRVAOverridesCrst         = NULL;

    m_pRemotingInterfaceThunks  = NULL;
    m_pRemotingInterfaceThunksCrst = NULL;
    if (!m_file->HasNativeImage())
    {
        // Init fields which are persisted in the native image
        m_dwCompilationRelaxationFlags = 0;
        m_cDynamicEntries           = 0;
        m_pStaticOffsets            = 0;
        m_maxDynamicEntries         = 0;
        m_dwMaxGCStaticHandles      = 0;
        m_dwStaticsBlockSize        = 0;
        m_dwPersistedFlags          = 0;
        m_pDllMain                  = NULL;
        m_pVASigCookieBlock         = NULL;
        m_pISymUnmanagedReader      = NULL;

        m_pIStreamSym               = NULL;

        // Set up tables
        ZeroMemory(&m_TypeDefToMethodTableMap, sizeof(LookupMap));
        m_dwTypeDefMapBlockSize = 0;
        ZeroMemory(&m_TypeRefToMethodTableMap, sizeof(LookupMap));
        m_dwTypeRefMapBlockSize = 0;
        ZeroMemory(&m_MethodDefToDescMap, sizeof(LookupMap));
        m_dwMethodDefMapBlockSize = 0;
        ZeroMemory(&m_FieldDefToDescMap, sizeof(LookupMap));
        m_dwFieldDefMapBlockSize = 0;
        ZeroMemory(&m_MemberRefToDescMap, sizeof(LookupMap));
        m_dwMemberRefMapBlockSize = 0;
        ZeroMemory(&m_FileReferencesMap, sizeof(LookupMap));
        m_dwFileReferencesMapBlockSize = 0;
        ZeroMemory(&m_ManifestModuleReferencesMap, sizeof(LookupMap));
        m_dwManifestModuleReferencesMapBlockSize = 0;

        m_pAvailableClasses         = NULL;
        m_pAvailableClassesCaseIns  = NULL;
        m_pAvailableParamTypes      = NULL;
        m_pInstMethodHashTable      = NULL;
        m_pDynamicStaticsInfo       = NULL;

        ZeroMemory(&m_ModuleCtorInfo, sizeof(m_ModuleCtorInfo));

        m_pBinder                   = NULL;





#ifdef _DEBUG 
        m_dwDebugArrayClassSize   = 0;
#endif

        // If we are being created fresh, the compilation type will match the loading type.
        // If we were ngenned, we will retain the compilation type across the load operation.

        if (pAssembly->IsDomainNeutral())
            m_dwPersistedFlags |= COMPILED_DOMAIN_NEUTRAL;

        m_pModuleSecurityDescriptor = NULL;
    }

    file->AddRef();
}



// Module initialization occurs in two phases: the constructor phase and the Initialize phase.
//
// The Initialize() phase completes the initialization after the constructor has run.
// It can throw exceptions but whether it throws or succeeds, it must leave the Module
// in a state where Destruct() can be safely called.
//
void Module::Initialize(AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    m_dwNumberOfActivations=0;
    m_Crst.Init("ModuleCrst", CrstModule, CRST_DEFAULT);
    m_LookupTableCrst.Init("ModuleLookupTableCrst", CrstModuleLookupTable, CrstFlags(CRST_UNSAFE_ANYMODE | CRST_DEBUGGER_THREAD));
    m_FixupCrst.Init("StringFixupCrst", CrstModuleFixup, (CrstFlags)(CRST_DEFAULT|CRST_HOST_BREAKABLE|CRST_REENTRANCY));
    m_InstMethodHashTableCrst.Init("InstMethodHashTableCrst", CrstInstMethodHashTable, CRST_REENTRANCY);

    if (!m_file->HasNativeImage())
    {
        m_rgDispatchTypeTable = NULL;
        AllocateMaps();

        _ASSERT(m_pModuleSecurityDescriptor == NULL);
        m_pModuleSecurityDescriptor = new ModuleSecurityDescriptor(this);

    }

    m_dwTransientFlags &= ~((DWORD)CLASSES_FREED);  // Set flag indicating LookupMaps are now in a consistent and destructable state
    
#ifdef PROFILING_SUPPORTED 
    // Profiler enabled, and re-jits requested?
    // if (CORProfilerAllowRejit())
    // {
    //    not supported in V2
    // }
#endif // PROFILING_SUPPORTED

#ifdef _DEBUG 
    m_fForceVerify = FALSE;
#endif

    if (m_pAvailableClasses == NULL && !IsResource())
    {
        m_pAvailableClasses = EEClassHashTable::Create(this,
                                                       AVAILABLE_CLASSES_HASH_BUCKETS,
                                                       FALSE /* bCaseInsensitive */, pamTracker);
    }

    if (m_pAvailableParamTypes == NULL && !IsResource())
    {
        m_pAvailableParamTypes = EETypeHashTable::Create(GetDomain(), this, PARAMTYPES_HASH_BUCKETS, pamTracker);
    }

    if (m_pInstMethodHashTable == NULL && !IsResource())
    {
        m_pInstMethodHashTable = InstMethodHashTable::Create(GetDomain(), this, PARAMMETHODS_HASH_BUCKETS, pamTracker);
    }

    if (GetAssembly()->IsDomainNeutral())
    {
        // Allocate the slots, record their values and get our shared module index
        // Note that we shift over one and tag it with the low bit
        m_pDomainLocalModule = (DomainLocalModule *) DomainLocalBlock::AllocateModuleID();
    }
    else
    {
        // this will be initialized a bit later.
        m_pDomainLocalModule = NULL;
    }

    // Prepare statics that are known at module load time
    AllocateStatics(pamTracker);


    LOG((LF_CLASSLOADER, LL_INFO10, "Loaded pModule: \"%ws\".\n", GetDebugName()));

}


#ifdef DEBUGGING_SUPPORTED 
void Module::SetDebuggerInfoBits(DebuggerAssemblyControlFlags newBits)
{
    LEAF_CONTRACT;

    _ASSERTE(((newBits << DEBUGGER_INFO_SHIFT_PRIV) &
              ~DEBUGGER_INFO_MASK_PRIV) == 0);

    m_dwTransientFlags &= ~DEBUGGER_INFO_MASK_PRIV;
    m_dwTransientFlags |= (newBits << DEBUGGER_INFO_SHIFT_PRIV);

    BOOL setEnC = ((newBits & DACF_ENC_ENABLED) != 0) && IsEditAndContinueCapable() && !GetAssembly()->IsDomainNeutral();

    // The only way can change Enc is through debugger override.
    if (setEnC)
    {
        EnableEditAndContinue();
    }
    else
    {
        DisableEditAndContinue();
    }
}
#endif // DEBUGGING_SUPPORTED

/* static */
Module *Module::Create(Assembly *pAssembly, mdFile moduleRef, PEFile *file, AllocMemTracker *pamTracker)
{
    CONTRACT(Module *)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pAssembly));
        PRECONDITION(CheckPointer(file));
        PRECONDITION(!IsNilToken(moduleRef) || file->IsAssembly());
        POSTCONDITION(CheckPointer(RETVAL));
        POSTCONDITION(RETVAL->GetFile() == file);
    }
    CONTRACT_END;

    // Hoist CONTRACT into separate routine because of EX incompatibility

    Module *pModule = NULL;

    // Create the module


    if (pModule == NULL)
    {
        {
            void* pMemory = pamTracker->Track(pAssembly->GetHighFrequencyHeap()->AllocMem(sizeof(Module)));
            pModule = new (pMemory) Module(pAssembly, moduleRef, file);
        }
    }

    PREFIX_ASSUME(pModule != NULL);
    ModuleHolder pModuleSafe(pModule);
    pModuleSafe->DoInit(pamTracker);

    pModuleSafe.SuppressRelease();

    RETURN pModuleSafe;
}



//
// Destructor for Module
//

void Module::Destruct()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    LOG((LF_EEMEM, INFO3, "Deleting module %x\n", this));
#ifdef PROFILING_SUPPORTED 
    if (!IsBeingUnloaded() && CORProfilerTrackModuleLoads())
    {
        // Profiler is causing some peripheral class loads. Probably this just needs
        // to be turned into a Fault_not_fatal and moved to a specific place inside the profiler.
        BEGIN_EXCEPTION_GLUE(/*hr=*/NULL, NULL)
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ModuleUnloadStarted((ThreadID) GetThread(), (ModuleID) this);
        }
        END_EXCEPTION_GLUE
    }
#endif // PROFILING_SUPPORTED


    DACNotify::DoModuleUnloadNotification(this);

    // Free classes in the class table
    FreeClassTables();

#ifdef DEBUGGING_SUPPORTED 
    {
        GCX_PREEMP();
        g_pDebugInterface->DestructModule(this);
    }
#endif // DEBUGGING_SUPPORTED

    ReleaseISymUnmanagedReader();

    if (m_pISymUnmanagedReaderLock)
    {
        delete m_pISymUnmanagedReaderLock;
        m_pISymUnmanagedReaderLock = NULL;
    }

   // Clean up sig cookies
    VASigCookieBlock    *pVASigCookieBlock = m_pVASigCookieBlock;
    while (pVASigCookieBlock)
    {
        VASigCookieBlock    *pNext = pVASigCookieBlock->m_Next;

        for (UINT i = 0; i < pVASigCookieBlock->m_numcookies; i++)
            pVASigCookieBlock->m_cookies[i].Destruct();

        delete pVASigCookieBlock;

        pVASigCookieBlock = pNext;
    }




#ifdef PROFILING_SUPPORTED 
    if (CORProfilerTrackModuleLoads())
    {
        // Profiler is causing some peripheral class loads. Probably this just needs
        // to be turned into a Fault_not_fatal and moved to a specific place inside the profiler.
        BEGIN_EXCEPTION_GLUE(/*hr=*/NULL, NULL)
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ModuleUnloadFinished((ThreadID) GetThread(),
                                                                    (ModuleID) this, S_OK);
        }
        END_EXCEPTION_GLUE
    }
#endif // PROFILING_SUPPORTED


    //
    // Warning - deleting the zap file will cause the module to be unmapped
    //
    IStream *pStream = GetInMemorySymbolStream();
    if(pStream != NULL)
    {
        pStream->Release();
        SetInMemorySymbolStream(NULL);
    }

    m_Crst.Destroy();
    m_FixupCrst.Destroy();
    m_LookupTableCrst.Destroy();
    m_InstMethodHashTableCrst.Destroy();

    if (m_pCerPrepInfo)
    {
        GCX_COOP();

        _ASSERTE(m_pCerCrst != NULL);
        CrstHolder sCrstHolder(m_pCerCrst);

        EEHashTableIteration sIter;
        m_pCerPrepInfo->IterateStart(&sIter);
        while (m_pCerPrepInfo->IterateNext(&sIter)) {
            CerPrepInfo *pPrepInfo = (CerPrepInfo*)m_pCerPrepInfo->IterateGetValue(&sIter);
            delete pPrepInfo;
        }

        delete m_pCerPrepInfo;
    }
    if (m_pCerCrst)
        delete m_pCerCrst;

    if (m_pRVAOverrides)
        delete m_pRVAOverrides;

    if (m_pRVAOverridesCrst)
        delete m_pRVAOverridesCrst;

    if (m_pRemotingInterfaceThunksCrst)
        delete m_pRemotingInterfaceThunksCrst;

    if (m_pRemotingInterfaceThunks)
        delete m_pRemotingInterfaceThunks;

    {
        m_file->Release();

        if (m_pModuleSecurityDescriptor)
            delete m_pModuleSecurityDescriptor;

    }
}


bool Module::NeedsGlobalMethodTable()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (!IsResource() && GetMDImport()->IsValidToken(COR_GLOBAL_PARENT_TOKEN))
    {
        HENUMInternalHolder funcEnum(GetMDImport());
        funcEnum.EnumGlobalFunctionsInit();

        HENUMInternalHolder fieldEnum(GetMDImport());
        fieldEnum.EnumGlobalFieldsInit();

        if (GetMDImport()->EnumGetCount(&funcEnum) != 0
            || GetMDImport()->EnumGetCount(&fieldEnum) != 0)
        {
            return true;
        }
    }

    // resource module or no global statics nor global functions
    return false;
}


MethodTable *Module::GetGlobalMethodTable()
{
    CONTRACT (MethodTable *)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(CONTRACT_RETURN NULL;);
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;


    if ((m_dwPersistedFlags & COMPUTED_GLOBAL_CLASS) == 0)
    {
        MethodTable *pMT = NULL;

        if (NeedsGlobalMethodTable())
        {
            pMT = ClassLoader::LoadTypeDefThrowing(this, COR_GLOBAL_PARENT_TOKEN,
                                                   ClassLoader::ThrowIfNotFound,
                                                   ClassLoader::FailIfUninstDefOrRef).AsMethodTable();
        }

        FastInterlockOr(&m_dwPersistedFlags, COMPUTED_GLOBAL_CLASS);
        RETURN pMT;
    }
    else
    {
        RETURN LookupTypeDef(COR_GLOBAL_PARENT_TOKEN).AsMethodTable();
    }
}


#endif // !DACCESS_COMPILE




BOOL Module::IsManifest()
{
    WRAPPER_CONTRACT;
    return PTR_HOST_TO_TADDR(GetAssembly()->GetManifestModule()) ==
        PTR_HOST_TO_TADDR(this);
}

DomainAssembly* Module::GetDomainAssembly(AppDomain *pDomain)
{
    CONTRACT(DomainAssembly *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pDomain, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACT_END;

    if (IsManifest())
        RETURN (DomainAssembly *) GetDomainFile(pDomain);
    else
        RETURN (DomainAssembly *) m_pAssembly->GetDomainAssembly(pDomain);
}

DomainFile *Module::GetDomainFile(AppDomain *pDomain)
{
    CONTRACT(DomainFile *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pDomain, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL));
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
    }
    CONTRACT_END;

    // Null means current domain
    if (pDomain == NULL)
        pDomain = GetAppDomain();

    if (m_pAssembly->IsDomainNeutral())
    {
        DomainLocalBlock *pLocalBlock = pDomain->GetDomainLocalBlock();
        DomainFile *pDomainFile =  pLocalBlock->TryGetDomainFile(GetModuleID());

#ifndef DACCESS_COMPILE 
        if (pDomainFile == NULL)
            pDomainFile = pDomain->LoadDomainNeutralModuleDependency(this, FILE_LOADED);
#endif // !DACCESS_COMPILE

        RETURN (PTR_DomainFile) pDomainFile;
    }
    else
    {

        CONSISTENCY_CHECK(PTR_HOST_TO_TADDR(pDomain) == PTR_HOST_TO_TADDR(GetDomain()));
        RETURN PTR_DomainFile(m_pDomainLocalModule->GetDomainFile());
    }
}

DomainAssembly* Module::FindDomainAssembly(AppDomain *pDomain)
{
    CONTRACT(DomainAssembly *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pDomain));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    if (IsManifest())
        RETURN (DomainAssembly *) FindDomainFile(pDomain);
    else
        RETURN (DomainAssembly *) m_pAssembly->FindDomainAssembly(pDomain);
}

DomainModule *Module::GetDomainModule(AppDomain *pDomain)
{
    CONTRACT(DomainModule *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pDomain, NULL_OK));
        PRECONDITION(!IsManifest());
        POSTCONDITION(CheckPointer(RETVAL));

        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACT_END;

    RETURN (DomainModule *) GetDomainFile(pDomain);
}

DomainFile *Module::FindDomainFile(AppDomain *pDomain)
{
    CONTRACT(DomainFile *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pDomain));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACT_END;

    if (m_pAssembly->IsDomainNeutral())
    {
        DomainLocalBlock *pLocalBlock = pDomain->GetDomainLocalBlock();
        RETURN pLocalBlock->TryGetDomainFile(GetModuleID());
    }
    else
    {
        if (PTR_HOST_TO_TADDR(pDomain) == PTR_HOST_TO_TADDR(GetDomain()))
            RETURN PTR_DomainFile(m_pDomainLocalModule->GetDomainFile());
        else
            RETURN NULL;
    }
}

DomainModule *Module::FindDomainModule(AppDomain *pDomain)
{
    CONTRACT(DomainModule *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pDomain));
        PRECONDITION(!IsManifest());
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        GC_NOTRIGGER;
        NOTHROW;
        MODE_ANY;
    }
    CONTRACT_END;

    RETURN (DomainModule *) FindDomainFile(pDomain);
    }

#ifndef DACCESS_COMPILE 


CorElementType  Module::ParseMetadataForStaticsIsTypeDefEnum(mdToken tk, const SigTypeContext* pContext)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    IMDInternalImport *pImport = GetMDImport();

    // The only condition we will be checking is that the parent of the type is System.Enum
    // Rest of the checks will be handed by class loader, which will fail to load if it's malformed
    // hence, no need to do all the checks here.
    mdToken tkParent = 0;
    DWORD dwParentAttr = 0;

    pImport->GetTypeDefProps(tk, &dwParentAttr, &tkParent);

    if(RidFromToken(tkParent)==0)
    {
        return ELEMENT_TYPE_END;
    }

    LPCSTR szName = NULL, szNamespace = NULL;
    switch(TypeFromToken(tkParent))
    {
        case mdtTypeDef:
            pImport->GetNameOfTypeDef(tkParent, &szName, &szNamespace);
            break;

        case mdtTypeRef:
            pImport->GetNameOfTypeRef(tkParent, &szNamespace, &szName);
            break;

        default:
            return ELEMENT_TYPE_END;
    }

    if (szName == NULL || szNamespace == NULL)
    {
        return ELEMENT_TYPE_END;
    }    
    
    // If it doesn't inherit from System.Enum, then it must be a value type
    // Note that loader will not load malformed types so this check is enough
    if(strcmp(szName,"Enum") != 0 || strcmp(szNamespace,"System") != 0)
    {
        return ELEMENT_TYPE_VALUETYPE;
    }

    // OK, it's an enum; find its instance field and get its type
    HENUMInternalHolderNoRef   hEnum(pImport);
    mdToken tkField;
    pImport->EnumInit(mdtFieldDef,tk,&hEnum);
    while(pImport->EnumNext(&hEnum,&tkField))
    {
        PCCOR_SIGNATURE pMemberSignature;
        DWORD           cMemberSignature;

        // Get the type of the static field.
        DWORD dwMemberAttribs = pImport->GetFieldDefProps(tkField);

        if(!IsFdStatic(dwMemberAttribs))
        {
            pMemberSignature = pImport->GetSigOfFieldDef(tkField, &cMemberSignature);
            HRESULT hr = validateTokenSig(tkField,pMemberSignature,cMemberSignature,dwMemberAttribs,pImport);

            if (FAILED(hr))
            {
                COMPlusThrowHR(hr);
            }

            SigTypeContext typeContext;
            MetaSig fsig(pMemberSignature, cMemberSignature, this, &typeContext, FALSE, MetaSig::sigField);
            CorElementType ElementType = fsig.NextArg();
            return ElementType;
        }
    }

    // no instance field found -- error!
    return ELEMENT_TYPE_END;
}

CorElementType  Module::ParseMetadataForStaticsIsValueTypeEnum(mdToken tk, const SigTypeContext* pContext)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if(TypeFromToken(tk) == mdtTypeDef)
    {
        return ParseMetadataForStaticsIsTypeDefEnum(tk, pContext);
    }
    else
    {
        // At this point, we would have to load other assemblies. The only one we have guaranteed
        // to be there is mscorlib. Would have to investigate if its worth it.
        return ELEMENT_TYPE_END;
    }
}


// Parses metadata and returns size needed for the different static blocks.
// Also allocates the offsets needed for each class
void Module::ParseMetadataForStatics(DWORD* pdwNumTypes, DWORD* pdwNonGCStaticBytes, DWORD* pdwGCStaticHandles)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Trade off here. We want a slot for each type. That way we can get to 2 bits per class and
    // index directly and not need a mapping from ClassID to MethodTable (we will use the RID
    // as the mapping)
    IMDInternalImport *pImport = GetMDImport();
    DWORD dwTypes   = pImport->GetCountWithTokenKind(mdtTypeDef) + 1; // +1 for module type

    // Allocate table for offsets. We need offsets for GC and non GC areas. We add +1 to use as
    // a sentinel (we don't care about not needing it release because we dont have this table
    // in correctly ngened images
    m_pStaticOffsets =
    (PTR_DWORD)(void*)
       GetDomain()->GetHighFrequencyHeap()->AllocMem((sizeof(DWORD)*(dwTypes+1))*2);

    HENUMInternal hTypeEnum;
    IfFailThrow(pImport->EnumAllInit(mdtTypeDef, &hTypeEnum));

    DWORD      dwGCHandles = 0;
    DWORD      dwNonGCBytes = 0;

    mdTypeDef type;
    // Parse each type of the class
    while (pImport->EnumNext(&hTypeEnum, &type))
    {
        HENUMInternal hFieldEnum;
        mdFieldDef field;

        // Set offset for this type
        DWORD dwIndex = RidFromToken(type) - 1;
        DWORD dwAlignment =  DATA_ALIGNMENT;
        DWORD dwClassNonGCBytes = 0;
        DWORD dwClassGCHandles  = 0;

        IfFailThrow(pImport->EnumInit(mdtFieldDef, type, &hFieldEnum));

        // need to check if the type is generic and if so exclude it from iteration as we don't know the size
        HENUMInternal hGenericEnum;
        IfFailThrow(pImport->EnumInit(mdtGenericParam, type, &hGenericEnum));
        ULONG cGenericParams = pImport->EnumGetCount(&hGenericEnum);
        pImport->EnumClose(&hGenericEnum);

        if (cGenericParams > 0)
        {
        }
        else
        {
            // Parse each field of the type
            while (pImport->EnumNext(&hFieldEnum, &field))
            {
                if (!GetMDImport()->IsValidToken(field))
                {
                    ThrowHR(COR_E_BADIMAGEFORMAT, BFA_METADATA_CORRUPT);
                }

                DWORD dwFieldAttrs = GetMDImport()->GetFieldDefProps(field);
                if (!IsFdLiteral(dwFieldAttrs) && IsFdStatic(dwFieldAttrs))
                {
                    PCCOR_SIGNATURE pMemberSignature;
                    DWORD       cMemberSignature;
                    DWORD       dwMemberAttribs;

                    // Get the type of the static field.
                    dwMemberAttribs = pImport->GetFieldDefProps(field);
                    pMemberSignature = pImport->GetSigOfFieldDef(field, &cMemberSignature);
                    HRESULT hr = validateTokenSig(field,pMemberSignature,cMemberSignature,dwMemberAttribs,pImport);
                    if (FAILED(hr))
                    {
                        COMPlusThrowHR(hr);
                    }
                    SigTypeContext typeContext;
                    MetaSig fsig(pMemberSignature, cMemberSignature, this, &typeContext, FALSE, MetaSig::sigField);
                    CorElementType ElementType = fsig.NextArg();

                ADD_SPACE:
                    switch (ElementType)
                    {
                        case ELEMENT_TYPE_I1:
                        case ELEMENT_TYPE_U1:
                        case ELEMENT_TYPE_BOOLEAN:
                            dwClassNonGCBytes += 4;  // We don't pack statics
                            break;
                        case ELEMENT_TYPE_I2:
                        case ELEMENT_TYPE_U2:
                        case ELEMENT_TYPE_CHAR:
                            dwClassNonGCBytes += 4; // We don't pack statics
                            break;
                        case ELEMENT_TYPE_I4:
                        case ELEMENT_TYPE_U4:
                        case ELEMENT_TYPE_R4:
                            dwClassNonGCBytes += 4;
                            break;
                        case ELEMENT_TYPE_FNPTR:
                        case ELEMENT_TYPE_PTR:
                        case ELEMENT_TYPE_I:
                        case ELEMENT_TYPE_U:
#if defined(ALIGN_ACCESS) 
                            dwClassNonGCBytes = ALIGN_UP(dwClassNonGCBytes, (1 << LOG2_PTRSIZE));
#endif
                            dwClassNonGCBytes += (1 << LOG2_PTRSIZE);
                            break;

                        case ELEMENT_TYPE_I8:
                        case ELEMENT_TYPE_U8:
                        case ELEMENT_TYPE_R8:
                            dwAlignment =  max(8, DATA_ALIGNMENT);
#if defined(ALIGN_ACCESS) 
                            dwClassNonGCBytes = ALIGN_UP(dwClassNonGCBytes, 8);
#endif
                            dwClassNonGCBytes += 8;
                            break;
                        case ELEMENT_TYPE_VAR:
                        case ELEMENT_TYPE_MVAR:
                        case ELEMENT_TYPE_STRING:
                        case ELEMENT_TYPE_SZARRAY:
                        case ELEMENT_TYPE_ARRAY:
                        case ELEMENT_TYPE_CLASS:
                        case ELEMENT_TYPE_OBJECT:
                            dwClassGCHandles  += 1;
                            break;
                        case ELEMENT_TYPE_VALUETYPE:
                        {
                            // See if we can figure out what the value type is
                            mdToken tk = fsig.GetArgProps().PeekValueTypeTokenClosed(&typeContext);
                            ElementType = ParseMetadataForStaticsIsValueTypeEnum(tk, &typeContext);

                            switch (ElementType)
                            {
                                case ELEMENT_TYPE_I1:
                                case ELEMENT_TYPE_U1:
                                case ELEMENT_TYPE_BOOLEAN:
                                case ELEMENT_TYPE_I2:
                                case ELEMENT_TYPE_U2:
                                case ELEMENT_TYPE_CHAR:
                                case ELEMENT_TYPE_I4:
                                case ELEMENT_TYPE_U4:
                                case ELEMENT_TYPE_R4:
                                case ELEMENT_TYPE_I:
                                case ELEMENT_TYPE_U:
                                case ELEMENT_TYPE_I8:
                                case ELEMENT_TYPE_U8:
                                case ELEMENT_TYPE_R8:
                                    goto ADD_SPACE;
                                case ELEMENT_TYPE_VALUETYPE:
                                {
                                    dwClassGCHandles  += 1;
                                    break;
                                }
                                case ELEMENT_TYPE_END:
                                default:
                                {
                                    // We'll have to be pessimistic here
#if defined(ALIGN_ACCESS) 
                                    dwClassNonGCBytes = ALIGN_UP(dwClassNonGCBytes, (1 << MAX_LOG2_PRIMITIVE_FIELD_SIZE));
#endif
                                    dwClassNonGCBytes += (1 << MAX_LOG2_PRIMITIVE_FIELD_SIZE);
                                    dwClassGCHandles  += 1;
                                    break;
                                }
                            }

                            break;
                        }

                        default:
                            // field has an unexpected type
                            ThrowHR(VER_E_FIELD_SIG);
                            break;
                    }
                }
            }
        }

        // We will require alignment if the type has non gc statics
        if (dwClassNonGCBytes)
        {
            dwNonGCBytes = (DWORD) ALIGN_UP(dwNonGCBytes, dwAlignment);
        }

        // Save current offsets
        m_pStaticOffsets[dwIndex*2]     = dwGCHandles*sizeof(sizeof(OBJECTREF*));
        m_pStaticOffsets[dwIndex*2 + 1] = dwNonGCBytes;

        // Increment for next class
        dwGCHandles  += dwClassGCHandles;
        dwNonGCBytes += dwClassNonGCBytes;


    }

    m_pStaticOffsets[dwTypes*2]     = dwGCHandles*sizeof(sizeof(OBJECTREF*));
    m_pStaticOffsets[dwTypes*2 + 1] = dwNonGCBytes;

    if (pdwNonGCStaticBytes) { *pdwNonGCStaticBytes = dwNonGCBytes; }
    if (pdwGCStaticHandles)  { *pdwGCStaticHandles  = dwGCHandles; }
    if (pdwNumTypes)         { *pdwNumTypes = dwTypes; }
}


void  Module::GetOffsetsForStaticData(
                    mdToken cl,
                    BOOL bDynamic, DWORD dwGCStaticHandles,
                    DWORD dwNonGCStaticBytes,
                    DWORD * pOutStaticHandleOffset,
                    DWORD * pOutNonGCStaticOffset)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    *pOutStaticHandleOffset = 0;
    *pOutNonGCStaticOffset  = 0;

    if (!dwGCStaticHandles && !dwNonGCStaticBytes)
    {
        return;
    }

    // Statics for instantiated types are allocated dynamically per-instantiation
    if (bDynamic)
    {
        // Non GC statics are embedded in the Dynamic Entry.
        *pOutNonGCStaticOffset  = DomainLocalModule::DynamicEntry::GetOffsetOfDataBlob();
        return;
    }

    {
        // We allocate in the big blob.

        _ASSERTE(m_pStaticOffsets);
        DWORD index = RidFromToken(cl) - 1;

        *pOutStaticHandleOffset = m_pStaticOffsets[index*2];

        *pOutNonGCStaticOffset  = m_pStaticOffsets[index*2 + 1];

        // Check we didnt go out of what we predicted we would need for the class
        if (*pOutStaticHandleOffset + sizeof(OBJECTREF*)*dwGCStaticHandles >
                    m_pStaticOffsets[(index+1)*2] ||
            *pOutNonGCStaticOffset + dwNonGCStaticBytes >
                    m_pStaticOffsets[(index+1)*2 + 1])
        {
            // If there is a corner case bug - mismatch between the loader code and code that estimates
            // the statics size for the module, we fail instead of opening a security hole.
            _ASSERTE(false);
            COMPlusThrowNonLocalized(kExecutionEngineException, L"Incorrect estimate of the statics size");
        }
    }
}

#endif // !DACCESS_COMPILE

#define IS_ENCODED_TYPEID(id) (id & 0x2)
#define ENCODE_TYPEID(id) ((id << 2) | 0x2)
#define DECODE_TYPEID(id) (id >> 2)

UINT32 Module::MapZapTypeID(UINT32 typeID)
{
    CONTRACTL {
        INSTANCE_CHECK;
        MODE_ANY;
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(m_rgDispatchTypeTable));
        PRECONDITION(typeID != TYPE_ID_THIS_CLASS);
    } CONTRACTL_END;

    if (typeID == TYPE_ID_THIS_CLASS) {
        return typeID;
    }

    SIZE_T typeTableValue = *((volatile SIZE_T *)(m_rgDispatchTypeTable + typeID));

#ifndef DACCESS_COMPILE 
    if (CORCOMPILE_IS_TOKEN_TAGGED(typeTableValue)) {
    }
#else // DACCESS_COMPILE
    DacError(E_FAIL);
#endif // DACCESS_COMPILE

    if (!IS_ENCODED_TYPEID(typeTableValue)) {
#ifndef DACCESS_COMPILE 
        // This is a MethodTable that was hard bound or restored just above
        MethodTable *pMT = (MethodTable *)typeTableValue;

        // Normally, an interface is restored by the time there is a call to
        // an interface method. However, SZArrays implicitly implement IList<T>,
        // and the IList<T> is not guaranteed to be restored in such cases.
        INDEBUG(BOOL wasAlreadyRestored = pMT->IsRestored());
        pMT->CheckRestore();
        _ASSERTE(wasAlreadyRestored || IsImplicitInterfaceOfSZArray(pMT));

        UINT32 newTypeID = pMT->GetTypeID();
        typeTableValue = ENCODE_TYPEID(newTypeID);
#else // DACCESS_COMPILE
        DacError(E_FAIL);
#endif // DACCESS_COMPILE
    }

    // Write the final result back into the array.
    m_rgDispatchTypeTable[typeID] = typeTableValue;

    CONSISTENCY_CHECK(!CORCOMPILE_IS_TOKEN_TAGGED(m_rgDispatchTypeTable[typeID]));
    CONSISTENCY_CHECK(IS_ENCODED_TYPEID(m_rgDispatchTypeTable[typeID]));
    INDEBUG(MethodTable *pActualMT = NULL; pActualMT = GetDomain()->LookupType(DECODE_TYPEID(m_rgDispatchTypeTable[typeID]));)
    return DECODE_TYPEID(m_rgDispatchTypeTable[typeID]);
}

#undef DECODE_TYPEID
#undef ENCODE_TYPEID
#undef IS_ENCODED_TYPEID



#ifndef DACCESS_COMPILE 


BOOL Module::IsNoStringInterning()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END

    if (!HasNativeImage())
    {
        return GetAssembly()->IsNoStringInterning();
    }

#ifdef _DEBUG 
    static ConfigDWORD g_NoStringInterning;
    DWORD dwOverride = g_NoStringInterning.val(L"NoStringInterning", 1);

    if (dwOverride == 0)
    {
        // Disabled
        return FALSE;
    }
    else if (dwOverride == 2)
    {
        // Always true (testing)
        return TRUE;
    }
#endif // _DEBUG

    return (m_dwCompilationRelaxationFlags & CompilationRelaxations_NoStringInterning)?TRUE:FALSE;
}

// Makes sure all flags are set
void Module::GenerateAllCompilationRelaxationFlags()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END

    if (GetAssembly()->IsNoStringInterning())
    {
        m_dwCompilationRelaxationFlags |=  CompilationRelaxations_NoStringInterning;
    }
}

MethodTable ** ModuleCtorInfo::GetGCStaticMTs(DWORD index)
{
    LEAF_CONTRACT;

    if (index < numHotGCStaticsMTs)
    {
        _ASSERTE(ppHotGCStaticsMTs != NULL);

        return ppHotGCStaticsMTs + index;
    }
    else
    {
        _ASSERTE(ppColdGCStaticsMTs != NULL);

        // shift the start of the cold table because all cold offsets are also shifted
        return ppColdGCStaticsMTs + (index - numHotGCStaticsMTs);
    }
}

DWORD Module::AllocateDynamicEntry(MethodTable *pMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(pMT->GetModuleForStatics() == this);
        PRECONDITION(pMT->IsDynamicStatics());
        PRECONDITION(!pMT->ContainsGenericVariables());
    }
    CONTRACTL_END;

    DWORD newId = FastInterlockExchangeAdd((LONG*)&m_cDynamicEntries, 1);

    if (newId >= m_maxDynamicEntries)
    {
        CrstHolder ch(&m_Crst);

        if (newId >= m_maxDynamicEntries)
        {
            SIZE_T maxDynamicEntries = max(16, m_maxDynamicEntries);
            while (maxDynamicEntries <= newId)
            {
                maxDynamicEntries *= 2;
            }

            DynamicStaticsInfo* pNewDynamicStaticsInfo = (DynamicStaticsInfo*)
                (void*)GetDomain()->GetHighFrequencyHeap()->AllocMem(sizeof(DynamicStaticsInfo) * maxDynamicEntries);

            if (m_pDynamicStaticsInfo)
                memcpy(pNewDynamicStaticsInfo, m_pDynamicStaticsInfo, sizeof(DynamicStaticsInfo) * m_maxDynamicEntries);

            m_pDynamicStaticsInfo = pNewDynamicStaticsInfo;
            m_maxDynamicEntries = maxDynamicEntries;
        }
    }

    m_pDynamicStaticsInfo[newId].pEnclosingMT = pMT;

    LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: Assigned dynamic ID %d to %s\n", newId, pMT->GetDebugClassName()));

    return newId;
}

void Module::BuildStaticsOffsets()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: Building statics offsets for %s\n", GetSimpleName()));

    // Parse metadata to obtain a good guess of how many bytes we need
    DWORD dwNumTypes, dwNonGCStaticBytes;

    ParseMetadataForStatics(&dwNumTypes, &dwNonGCStaticBytes, &m_dwMaxGCStaticHandles);


    // Calculate offsets of each section.

    // Organization in memory of the static block
    //
    //
    //                                       |  GC Statics             |
    //                                                  |
    //                                                  |
    //    | Class Data (one byte per class)  |   pointer to gc statics | primitive type statics |
    //
    //
    DWORD dwOffsetClassSlots;
    DWORD dwOffsetNonGCStaticBytes;

    dwOffsetClassSlots        = (DWORD) DomainLocalModule::OffsetOfDataBlob();

    dwOffsetNonGCStaticBytes  = (DWORD) ALIGN_UP((size_t)(dwOffsetClassSlots + sizeof(BYTE)*dwNumTypes), (1 << MAX_LOG2_PRIMITIVE_FIELD_SIZE));

    // Calculate final size of the precomputed statics block
    m_dwStaticsBlockSize = (dwOffsetNonGCStaticBytes + dwNonGCStaticBytes - dwOffsetClassSlots);

    DWORD i;
    for (i = 0 ; i <= dwNumTypes ; i++) // <= for sentinel
    {
        m_pStaticOffsets[i*2]     += 0;
        m_pStaticOffsets[i*2 + 1] += dwOffsetNonGCStaticBytes;
    }

    // Check we're sane
    _ASSERTE(m_pStaticOffsets[0               ] == 0);
    _ASSERTE(m_pStaticOffsets[dwNumTypes*2    ] == m_dwMaxGCStaticHandles*sizeof(OBJECTREF*));

    _ASSERTE(m_pStaticOffsets[1               ] == dwOffsetNonGCStaticBytes);
    _ASSERTE(m_pStaticOffsets[dwNumTypes*2 + 1] == dwOffsetClassSlots + m_dwStaticsBlockSize);
}



void Module::AllocateStaticHandles(AppDomain* pDomain)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    // Allocate the handles we will need. Note that AllocateStaticFieldObjRefPtrs will only
    // allocate if pModuleData->GetGCStaticsBasePointerAddress(pMT) != 0, avoiding creating
    // handles more than once for a given MT or module

    DomainLocalModule *pModuleData = GetDomainLocalModule(pDomain);

    _ASSERTE(pModuleData->GetPrecomputedGCStaticsBasePointerAddress() != NULL);
    if (this->m_dwMaxGCStaticHandles > 0)
    {
        pDomain->AllocateStaticFieldObjRefPtrs(this->m_dwMaxGCStaticHandles,
                                               pModuleData->GetPrecomputedGCStaticsBasePointerAddress());

        // We should throw if we fail to allocate and never hit this assert
        _ASSERTE(pModuleData->GetPrecomputedGCStaticsBasePointer() != NULL);
    }

    RETURN;
}

void Module::AllocateStatics(AllocMemTracker *pamTracker)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    if (IsResource())
    {
        // If it has no code, we don't have to allocate anything
        LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: Resource module %s. No statics neeeded\n", GetSimpleName()));
        RETURN;
    }

    if (m_pStaticOffsets == (PTR_DWORD) NGEN_STATICS_ALLCLASSES_WERE_LOADED)
    {
        _ASSERTE(HasNativeImage());

        // This is an ngen image and all the classes were loaded at ngen time, so we're done.
        LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: 'Complete' Native image found, no statics parsing needed for module %s.\n", GetSimpleName()));
        RETURN;
    }

    LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: Allocating statics for module %s\n", GetSimpleName()));

    // Build the offset table, which will tell us what the offsets for the statics of each class are (one offset for gc handles, one offset
    // for non gc types)
    BuildStaticsOffsets();

    RETURN;
}

// This method will report GC static refs of the module. It doesn't have to be complete (ie, it's 
// currently used to opportunistically get more concurrency in the marking of statics), so it currently
// ignores any statics that are not preallocated (ie: won't report statics from IsDynamicStatics() MT)
// The reason this function is in Module and not in DomainFile (together with DomainLocalModule is because
// for shared modules we need a very fast way of getting to the DomainLocalModule. For that we use
// a table in DomainLocalBlock that's indexed with a module ID
void Module::EnumStaticGCRefs(AppDomain* pAppDomain, GCEnumCallback  pCallback, LPVOID hCallBack)
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


    DomainLocalModule *pModuleData = GetDomainLocalModule(pAppDomain);
    DWORD dwHandles                = m_dwMaxGCStaticHandles;

    if (IsResource())
    {
        RETURN;
    }

    LOG((LF_GC, LL_INFO100, "Scanning statics for module %s\n", GetSimpleName()));

    OBJECTREF* ppObjectRefs       = pModuleData->GetPrecomputedGCStaticsBasePointer();
    for (DWORD i = 0 ; i < dwHandles ; i++)
    {
        // Handles are allocated in SetDomainFile (except for bootstrapped mscorlib). In any
        // case, we shouldnt get called if the module hasn't had it's handles allocated (as we
        // only get here if IsActive() is true, which only happens after SetDomainFile(), which
        // is were we allocate handles.
        _ASSERTE(ppObjectRefs);
        pCallback(hCallBack, ppObjectRefs+i, 0);
    }        

    LOG((LF_GC, LL_INFO100, "Done scanning statics for module %s\n", GetSimpleName()));

    RETURN;
}

void Module::SetDomainFile(DomainFile *pDomainFile)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pDomainFile));
        PRECONDITION(IsManifest() == pDomainFile->IsAssembly());
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    DomainLocalModule* pModuleData = 0;

    // Do we need to allocate memory for the non GC statics?
    if (GetAssembly()->IsDomainNeutral() || m_pDomainLocalModule == NULL)
    {
        LOG((LF_CLASSLOADER, LL_INFO10, "STATICS: Allocating %i bytes for precomputed statics in module %s in Appdomain %08x\n",
        this->GetStaticsAndClassInitBlockSize(), this->GetDebugName(), pDomainFile->GetAppDomain()));

        MEMORY_REPORT_CONTEXT_SCOPE("ModuleStatics");

        // Allocate memory for the module statics. For ngened non shared case, we dont
        // need to do so, as we have the data in the image
        pModuleData =    (DomainLocalModule*)(void*)
                         pDomainFile->GetAppDomain()->GetHighFrequencyHeap()->AllocMem(sizeof(DomainLocalModule) +
                         this->GetStaticsAndClassInitBlockSize());

        // Note: Memory allocated on loader heap is zero filled
        // memset(pModuleData->GetPrecomputedStaticsAndClassInitBlock(), 0, this->GetStaticsAndClassInitBlockSize());

        // Init members
        pModuleData->Init();
    }
    else
    {
        pModuleData = this->m_pDomainLocalModule;
        LOG((LF_CLASSLOADER, LL_INFO10, "STATICS: Allocation not needed for ngened non shared module %s in Appdomain %08x\n"));
    }

    if (GetAssembly()->IsDomainNeutral())
    {
        DomainLocalBlock *pLocalBlock;
        {
            pLocalBlock = pDomainFile->GetAppDomain()->GetDomainLocalBlock();
            pLocalBlock->SetModuleSlot(GetModuleID(), pModuleData);
        }

        pLocalBlock->SetDomainFile(GetModuleID(), pDomainFile);
    }
    else
    {
        // Non shared case, module points directly to the statics. In ngen case
        // m_pDomainModule is already set for the non shared case
        if (m_pDomainLocalModule == NULL)
        {
            m_pDomainLocalModule = pModuleData;
        }

        m_pDomainLocalModule->SetDomainFile(pDomainFile);
    }

    // Allocate static handles now.
    // NOTE: Bootstrapping issue with mscorlib - we will manually allocate later
    if (g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT] != NULL)
        AllocateStaticHandles(pDomainFile->GetAppDomain());

}

OBJECTREF Module::GetExposedObject()
{
    CONTRACT(OBJECTREF)
    {
        INSTANCE_CHECK;
        POSTCONDITION(RETVAL != NULL);
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACT_END;

    RETURN GetDomainFile()->GetExposedModuleObject();
}

//
// AllocateMap allocates the RID maps based on the size of the current
// metadata (if any)
//

void Module::AllocateMaps()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    enum
    {
        TYPEDEF_MAP_INITIAL_SIZE = 5,
        TYPEREF_MAP_INITIAL_SIZE = 5,
        MEMBERREF_MAP_INITIAL_SIZE = 10,
        MEMBERDEF_MAP_INITIAL_SIZE = 10,
        FILEREFERENCES_MAP_INITIAL_SIZE = 5,
        ASSEMBLYREFERENCES_MAP_INITIAL_SIZE = 5,

        TYPEDEF_MAP_BLOCK_SIZE = 50,
        TYPEREF_MAP_BLOCK_SIZE = 50,
        MEMBERREF_MAP_BLOCK_SIZE = 50,
        MEMBERDEF_MAP_BLOCK_SIZE = 50,
        FILEREFERENCES_MAP_BLOCK_SIZE = 50,
        ASSEMBLYREFERENCES_MAP_BLOCK_SIZE = 50,
    };

    DWORD           dwTableAllocElements;
    DWORD           dwReserveSize;
    TADDR           *pTable = NULL;

    if (IsResource())
        return;

    MEMORY_REPORT_CONTEXT_SCOPE("RidMap");

    if (IsReflection())
    {
        m_TypeDefToMethodTableMap.dwMaxIndex = TYPEDEF_MAP_INITIAL_SIZE;

        // The above is essential.  The following ones are precautionary.
        m_TypeRefToMethodTableMap.dwMaxIndex = TYPEREF_MAP_INITIAL_SIZE;
        m_MethodDefToDescMap.dwMaxIndex = MEMBERDEF_MAP_INITIAL_SIZE;
        m_FieldDefToDescMap.dwMaxIndex = MEMBERDEF_MAP_INITIAL_SIZE;
        m_MemberRefToDescMap.dwMaxIndex = MEMBERREF_MAP_BLOCK_SIZE;
        m_FileReferencesMap.dwMaxIndex = FILEREFERENCES_MAP_INITIAL_SIZE;
        m_ManifestModuleReferencesMap.dwMaxIndex = ASSEMBLYREFERENCES_MAP_INITIAL_SIZE;
    }
    else
    {
        HENUMInternal   hTypeDefEnum;

        IfFailThrow(GetMDImport()->EnumTypeDefInit(&hTypeDefEnum));
        m_TypeDefToMethodTableMap.dwMaxIndex = GetMDImport()->EnumTypeDefGetCount(&hTypeDefEnum);
        GetMDImport()->EnumTypeDefClose(&hTypeDefEnum);

        if (m_TypeDefToMethodTableMap.dwMaxIndex >= MAX_CLASSES_PER_MODULE)
            ThrowHR(COR_E_TYPELOAD);

        // Metadata count is inclusive
        m_TypeDefToMethodTableMap.dwMaxIndex++;

        // Get # TypeRefs
        m_TypeRefToMethodTableMap.dwMaxIndex = GetMDImport()->GetCountWithTokenKind(mdtTypeRef)+1;

        // Get # MethodDefs
        m_MethodDefToDescMap.dwMaxIndex = GetMDImport()->GetCountWithTokenKind(mdtMethodDef)+1;

        // Get # FieldDefs
        m_FieldDefToDescMap.dwMaxIndex = GetMDImport()->GetCountWithTokenKind(mdtFieldDef)+1;

        // Get # MemberRefs
        m_MemberRefToDescMap.dwMaxIndex = GetMDImport()->GetCountWithTokenKind(mdtMemberRef)+1;

        // Get the number of AssemblyReferences and FileReferences in the map
        m_FileReferencesMap.dwMaxIndex = GetMDImport()->GetCountWithTokenKind(mdtFile)+1;
        m_ManifestModuleReferencesMap.dwMaxIndex = GetMDImport()->GetCountWithTokenKind(mdtAssemblyRef)+1;
    }

    ClrSafeInt<DWORD> acc;
    acc += m_TypeDefToMethodTableMap.dwMaxIndex;
    acc += m_TypeRefToMethodTableMap.dwMaxIndex;
    acc += m_MemberRefToDescMap.dwMaxIndex;
    acc += m_MethodDefToDescMap.dwMaxIndex;
    acc += m_FieldDefToDescMap.dwMaxIndex;
    acc += m_ManifestModuleReferencesMap.dwMaxIndex;
    acc += m_FileReferencesMap.dwMaxIndex;
    if (acc.IsOverflow())
        COMPlusThrowOM();
    dwTableAllocElements = acc.Value();

    acc *= (sizeof(void*));
    if (acc.IsOverflow())
        COMPlusThrowOM();
    dwReserveSize = acc.Value();


    if (dwTableAllocElements > 0)
    {
        _ASSERTE (m_pAssembly && m_pAssembly->GetLowFrequencyHeap());
        pTable = (TADDR *)(void*) m_pAssembly->GetLowFrequencyHeap()->AllocMem(dwTableAllocElements * sizeof(void*));

        // Note: Memory allocated on loader heap is zero filled
        // memset(pTable, 0, dwTableAllocElements * sizeof(void*));
    }

    m_dwTypeDefMapBlockSize = TYPEDEF_MAP_BLOCK_SIZE;
    m_TypeDefToMethodTableMap.pdwBlockSize = &m_dwTypeDefMapBlockSize;
    m_TypeDefToMethodTableMap.pNext  = NULL;
    m_TypeDefToMethodTableMap.pTable = pTable;

    m_dwTypeRefMapBlockSize = TYPEREF_MAP_BLOCK_SIZE;
    m_TypeRefToMethodTableMap.pdwBlockSize = &m_dwTypeRefMapBlockSize;
    m_TypeRefToMethodTableMap.pNext  = NULL;
    m_TypeRefToMethodTableMap.pTable = &pTable[m_TypeDefToMethodTableMap.dwMaxIndex];

    m_dwMethodDefMapBlockSize = MEMBERDEF_MAP_BLOCK_SIZE;
    m_MethodDefToDescMap.pdwBlockSize = &m_dwMethodDefMapBlockSize;
    m_MethodDefToDescMap.pNext  = NULL;
    m_MethodDefToDescMap.pTable = &m_TypeRefToMethodTableMap.pTable[m_TypeRefToMethodTableMap.dwMaxIndex];

    m_dwFieldDefMapBlockSize = MEMBERDEF_MAP_BLOCK_SIZE;
    m_FieldDefToDescMap.pdwBlockSize = &m_dwFieldDefMapBlockSize;
    m_FieldDefToDescMap.pNext  = NULL;
    m_FieldDefToDescMap.pTable = &m_MethodDefToDescMap.pTable[m_MethodDefToDescMap.dwMaxIndex];

    m_dwMemberRefMapBlockSize = MEMBERREF_MAP_BLOCK_SIZE;
    m_MemberRefToDescMap.pdwBlockSize = &m_dwMemberRefMapBlockSize;
    m_MemberRefToDescMap.pNext  = NULL;
    m_MemberRefToDescMap.pTable = &m_FieldDefToDescMap.pTable[m_FieldDefToDescMap.dwMaxIndex];

    m_dwFileReferencesMapBlockSize = FILEREFERENCES_MAP_BLOCK_SIZE;
    m_FileReferencesMap.pdwBlockSize = &m_dwFileReferencesMapBlockSize;
    m_FileReferencesMap.pNext  = NULL;
    m_FileReferencesMap.pTable = &m_MemberRefToDescMap.pTable[m_MemberRefToDescMap.dwMaxIndex];

    m_dwManifestModuleReferencesMapBlockSize = ASSEMBLYREFERENCES_MAP_BLOCK_SIZE;
    m_ManifestModuleReferencesMap.pdwBlockSize = &m_dwManifestModuleReferencesMapBlockSize;
    m_ManifestModuleReferencesMap.pNext  = NULL;
    m_ManifestModuleReferencesMap.pTable = &m_FileReferencesMap.pTable[m_FileReferencesMap.dwMaxIndex];
}

//
// FreeClassTables frees the classes in the module
//

void Module::FreeClassTables()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (m_dwTransientFlags & CLASSES_FREED)
        return;

    FastInterlockOr(&m_dwTransientFlags, CLASSES_FREED);

    // disable ibc here because it can cause errors during the destruction of classes
    IBCLoggingDisabler disableIbc;
            
#if _DEBUG 
    DebugLogRidMapOccupancy();
#endif

    // Free the types filled out in the TypeDefToEEClass map
    LookupMap *pMap;
    DWORD       dwMinIndex = 0;
    MethodTable *pMT;

    // Go through each linked block
    for (pMap = &m_TypeDefToMethodTableMap; pMap != NULL && pMap->pTable; pMap = pMap->pNext)
    {
        DWORD i;
        DWORD dwIterCount = pMap->dwMaxIndex - dwMinIndex;
        TADDR *pRealTableStart = &pMap->pTable[dwMinIndex];

        for (i = 0; i < dwIterCount; i++)
        {
            TADDR result = pRealTableStart[i];
            if (result != NULL && (result & (TADDR)LookupMap::IS_MODULE_BACKPOINTER) == 0)
            {
                pMT = (MethodTable *) result;

                if (pMT != NULL)
                {
                    pMT->GetClass()->Destruct();
                    pRealTableStart[i] = NULL;
                }
            }
        }

        dwMinIndex = pMap->dwMaxIndex;
    }

    // Now do the same for constructed types (arrays and instantiated generic types)
    if (IsTenured())  // If we're destructing because of an error during the module's creation, we'll play it safe and not touch this table as its memory is freed by a
    {                 // separate AllocMemTracker. Though you're supposed to destruct everything else before destructing the AllocMemTracker, this is an easy invariant to break so
                      // we'll play extra safe on this end.
        if (m_pAvailableParamTypes != NULL)
        {
            EETypeHashTable::Iterator it(m_pAvailableParamTypes);
            EETypeHashEntry *pEntry;
            while (m_pAvailableParamTypes->FindNext(&it, &pEntry))
            {
                // We need to call destruct on instances of EEClass whose "canonical" dependent lives in this table
                if (pEntry->data.IsRestored() && pEntry->data.OwnsEEClass())
                {
                    EEClass *pClass = pEntry->data.GetClass();
                    CONSISTENCY_CHECK(pClass != NULL);
                    if (!pClass->IsDestroyed())
                    {
                        pClass->Destruct();
                    }
                    pEntry->data = TypeHandle();
                }
            }
        }
    }
}




#endif // !DACCESS_COMPILE

ClassLoader *Module::GetClassLoader()
{
    WRAPPER_CONTRACT;
    _ASSERTE(m_pAssembly != NULL);
    return m_pAssembly->GetLoader();
}

BaseDomain *Module::GetDomain()
{
    WRAPPER_CONTRACT;
    _ASSERTE(m_pAssembly != NULL);
    return m_pAssembly->GetDomain();
}

#ifndef DACCESS_COMPILE 

AssemblySecurityDescriptor *Module::GetSecurityDescriptor()
{
    WRAPPER_CONTRACT;
    _ASSERTE(m_pAssembly != NULL);
    return m_pAssembly->GetSecurityDescriptor();
}

//
// We'll use this struct and global to keep a list of all
// ISymUnmanagedReaders and ISymUnmanagedWriters (or any IUnknown) so
// we can relelease them at the end.
//
struct IUnknownList
{
    IUnknownList   *next;
    HelpForInterfaceCleanup *cleanup;
    IUnknown       *pUnk;
};

static IUnknownList *g_IUnknownList = NULL;

/* static */
HRESULT Module::TrackIUnknownForDelete(IUnknown *pUnk,
                                 IUnknown ***pppUnk,
                                 HelpForInterfaceCleanup *pCleanHelp)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    IUnknownList *pNew = new (nothrow) IUnknownList;
    if (!pNew)
        return E_OUTOFMEMORY;

    pNew->pUnk = pUnk; // Ref count is 1
    pNew->cleanup = pCleanHelp;
    pNew->next = g_IUnknownList;
    g_IUnknownList = pNew;

    // Return the address of where we're keeping the IUnknown*, if
    // needed.
    if (pppUnk)
        *pppUnk = &(pNew->pUnk);

    return S_OK;
}

/* static */
void Module::ReleaseAllIUnknowns(void)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    IUnknownList **ppElement = &g_IUnknownList;

    while (*ppElement)
    {
        IUnknownList *pTmp = *ppElement;

        // Release the IUnknown
        if (pTmp->pUnk != NULL)
            pTmp->pUnk->Release();

        if (pTmp->cleanup != NULL)
            delete pTmp->cleanup;

        *ppElement = pTmp->next;
        delete pTmp;
    }
}

/* static */
void Module::ReleaseIUnknown(IUnknown *pUnk)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    IUnknownList **ppElement = &g_IUnknownList;

    while (*ppElement)
    {
        IUnknownList *pTmp = *ppElement;

        // Release the IUnknown
        if (pTmp->pUnk == pUnk)
        {

            //
            FAULT_NOT_FATAL();
            pTmp->pUnk->Release();
            if (pTmp->cleanup != NULL)
                delete pTmp->cleanup;
            *ppElement = pTmp->next;
            delete pTmp;
            break;
        }
        ppElement = &pTmp->next;
    }
    _ASSERTE(ppElement);    // if have a reader, should have found it in list
}

/* static */
void Module::ReleaseIUnknown(IUnknown **ppUnk)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    IUnknownList **ppElement = &g_IUnknownList;

    while (*ppElement)
    {
        IUnknownList *pTmp = *ppElement;

        // Release the IUnknown
        if (&(pTmp->pUnk) == ppUnk)
        {
            //
            FAULT_NOT_FATAL();
            if (pTmp->pUnk)
                pTmp->pUnk->Release();
            if (pTmp->cleanup != NULL)
                delete pTmp->cleanup;
            *ppElement = pTmp->next;
            delete pTmp;
            break;
        }
        ppElement = &pTmp->next;
    }
    _ASSERTE(ppElement);    // if have a reader, should have found it in list
}

void Module::ReleaseISymUnmanagedReader(void)
{
    WRAPPER_CONTRACT;
    // This doesn't have to take the m_pISymUnmanagedReaderLock since
    // a module is destroyed only by one thread.
    if (m_pISymUnmanagedReader == NULL)
        return;
    Module::ReleaseIUnknown(m_pISymUnmanagedReader);
    m_pISymUnmanagedReader = NULL;
}

void Module::StartUnload()
{
    WRAPPER_CONTRACT;
#ifdef PROFILING_SUPPORTED 
    if (!IsBeingUnloaded() && CORProfilerTrackModuleLoads())
    {
        // Profiler is causing some peripheral class loads. Probably this just needs
        // to be turned into a Fault_not_fatal and moved to a specific place inside the profiler.
        BEGIN_EXCEPTION_GLUE(/*hr=*/NULL, NULL)
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->ModuleUnloadStarted((ThreadID) GetThread(), (ModuleID) this);

        }
        END_EXCEPTION_GLUE
    }
#endif // PROFILING_SUPPORTED
    SetBeingUnloaded();
}

void Module::ReleaseILData(void)
{
    WRAPPER_CONTRACT;

    ReleaseISymUnmanagedReader();
}


/* static */
void Module::ReleaseMemoryForTracking()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    IUnknownList **ppElement = &g_IUnknownList;

    while (*ppElement)
    {
        IUnknownList *pTmp = *ppElement;

        *ppElement = pTmp->next;

        if (pTmp->cleanup != NULL)
        {
            (*(pTmp->cleanup->pFunction))(pTmp->cleanup->pData);

            delete pTmp->cleanup;
        }

        delete pTmp;
    }
}// ReleaseMemoryForTracking


//
// Module::FusionCopyPDBs asks Fusion to copy PDBs for a given
// assembly if they need to be copied. This is for the case where a PE
// file is shadow copied to the Fusion cache. Fusion needs to be told
// to take the time to copy the PDB, too.
//

STDAPI CopyPDBs(IAssembly *pAsm); // private fusion API

void Module::FusionCopyPDBs(LPCWSTR moduleName)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    Assembly *pAssembly = GetAssembly();

    // Just return if we've already done this for this Module's
    // Assembly.
    if ((pAssembly->GetDebuggerInfoBits() & DACF_PDBS_COPIED) ||
        (pAssembly->GetFusionAssembly() == NULL))
    {
        LOG((LF_CORDB, LL_INFO10,
             "Don't need to copy PDB's for module %S\n",
             moduleName));

        return;
    }

    LOG((LF_CORDB, LL_INFO10,
         "Attempting to copy PDB's for module %S\n", moduleName));

    HRESULT hr;
    hr = CopyPDBs(pAssembly->GetFusionAssembly());
    LOG((LF_CORDB, LL_INFO10,
            "Fusion.dll!CopyPDBs returned hr=0x%08x for module 0x%08x\n",
            hr, this));

    // Remember that we've copied the PDBs for this assembly.
    pAssembly->SetCopiedPDBs();
}

// This function will free the metadata interface if we are not
// able to free the ISymUnmanagedReader
static void ReleaseImporterFromISymUnmanagedReader(void * pData)
{
    WRAPPER_CONTRACT;
    IMetaDataImport *md = (IMetaDataImport*)pData;

    // We need to release it twice
    md->Release();
    md->Release();

}// ReleaseImporterFromISymUnmanagedReader

// This function will return PDB stream if exist.
// It is the caller responsibility to call release on *ppStream after a successful
// result.
// We will first check to see if we have a cached pdb stream available. If not,
// we will ask fusion which in terms to ask host vis HostProvideAssembly. Host may
// decide to provide one or not.
//
HRESULT Module::GetHostPdbStream(IStream **ppStream)
{
    CONTRACTL
    {
        NOTHROW;
        if(GetThread()) {GC_TRIGGERS;} else {GC_NOTRIGGER;}
    }
    CONTRACTL_END

    HRESULT hr = NOERROR;

    _ASSERTE(ppStream);

    *ppStream = NULL;

    if (m_file->IsIStream() == false)
    {
        // not a host stream
        return E_FAIL;
    }
    *ppStream = m_file->GetPdbStream();

    if (*ppStream != NULL)
    {
        // We are done!
        // This is the case where host specified the pdb in the IStream format
        // in LoadPdb host API call. This is the case where module was loaded
        // without pdb before attach.
        //
        return S_OK;
    }

    // Maybe fusion can ask our host. This will give us back a PDB stream if
    // host decides to provide one.
    //
    if (m_file->IsAssembly())
    {
        GCX_PREEMP();
        hr = ((PEAssembly*)m_file)->GetIHostAssembly()->GetAssemblyDebugStream(ppStream);
    }
    else
    {
        _ASSERTE(m_file->IsModule());
        IHostAssemblyModuleImport *pIHAMI;
        MAKE_WIDEPTR_FROMUTF8_NOTHROW(pName, m_file->GetSimpleName());
        if (pName == NULL)
            return E_OUTOFMEMORY;
        IfFailRet(m_file->GetAssembly()->GetIHostAssembly()->GetModuleByName(pName, &pIHAMI));
        hr = pIHAMI->GetModuleDebugStream(ppStream);
    }
    return hr;
}

// Clear the cached PDB stream associated with this module if we have one.
// Note that SQL will provide PDB stream in HostProvideAssembly call. However, only
// when Loader triggers this call, we will cache it. If it is debugger who triggers this call,
// we will not cache it.
//
HRESULT Module::ClearHostPdbStream()
{
    // clear our cache if we have one!
    m_file->ClearPdbStream();

    return S_OK;
}



ISymUnmanagedReader *Module::GetISymUnmanagedReaderNoThrow(void)
{
    CONTRACT(ISymUnmanagedReader *)
    {
        INSTANCE_CHECK;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
    }
    CONTRACT_END;

    ISymUnmanagedReader *ret = NULL;

    EX_TRY
    {
        ret = GetISymUnmanagedReader();
    }
    EX_CATCH
    {
        // We swallow any exception and say that we simply couldn't get a reader by returning NULL.
        ret = NULL;
    }
    EX_END_CATCH(SwallowAllExceptions);

    RETURN (ret);
}

ISymUnmanagedReader *Module::GetISymUnmanagedReader(void)
{
    CONTRACT(ISymUnmanagedReader *)
    {
        INSTANCE_CHECK;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        THROWS;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
    }
    CONTRACT_END;

    // No symbols for resource modules
    if (IsResource())
        RETURN NULL;

    // ReleaseAllIUnknowns() called during EEShutDown() will destroy
    // m_pISymUnmanagedReader. We cannot use it for stack-traces or anything
    if (g_fEEShutDown)
        RETURN NULL;

    // Verify that symbol reading is permitted for this module.
    // If we know we've already created a symbol reader, don't bother checking.  There is
    // no advantage to allowing symbol reading to be turned off if we've already created the reader.
    // Note that we can't just put this code in the creation block below because we might have to
    // call managed code to resolve security policy, and we can't do that while holding a lock.
    // There is no disadvantage other than a minor perf cost to calling this unnecessarily, so the
    // race on m_pISymUnmanagedReader here is OK.  The perf cost is minor because the only real
    // work is done by the security system which caches the result.
    if( m_pISymUnmanagedReader == NULL && !IsSymbolReadingEnabled() )
        RETURN NULL;
    
    // If we haven't created the lock yet, do so lazily here
    if (m_pISymUnmanagedReaderLock == NULL)
    {

        Crst* pCritSec = new Crst("ISymUnmangedReader",
                                            (CrstLevel) CrstISymUnmanagedReader, CRST_DEBUGGER_THREAD);

        // Swap the pointers in a thread safe manner.
        if (InterlockedCompareExchangePointer((PVOID *)&m_pISymUnmanagedReaderLock,
                                              (PVOID)pCritSec, NULL) != NULL)
            delete pCritSec;
    }

    // Take the lock for the m_pISymUnmanagedReader
    CrstPreempHolder holder(m_pISymUnmanagedReaderLock);

    UINT lastErrorMode = 0;

    // Check to see if this variable has already been set
    if (m_pISymUnmanagedReader == NULL)
    {
        // There are 4 main cases here:
        //  1. Assembly is on disk and we'll get the symbols from a file next to the assembly
        //  2. Assembly is provided by the host and we'll get the symbols from the host
        //  3. Assembly was loaded in-memory (by byte array or ref-emit), and symbols were
        //      provided along with it.
        //  4. Assembly was loaded in-memory but no symbols were provided.

        // Determine whether we should be looking in memory for the symbols (cases 2 & 3)        
        bool fInMemorySymbols = ( m_file->IsIStream() || GetInMemorySymbolStream() );
        if( !fInMemorySymbols && m_file->GetPath().IsEmpty() )
        {
            // Case 4.  We don't have a module path, an IStream or an in memory symbol stream, 
            // so there is no-where to try and get symbols from.
            RETURN (NULL);
        }

        // Create a binder to find the reader.
        //
        HRESULT hr = S_OK;

        SafeComHolder<ISymUnmanagedBinder> pBinder;

        IfFailThrow(FakeCoCreateInstance(CLSID_CorSymBinder_SxS,
                                  IID_ISymUnmanagedBinder,
                                  (void**)&pBinder));

        LOG((LF_CORDB, LL_INFO10, "M::GISUR: Created binder\n"));

        // Note: we change the error mode here so we don't get any popups as the PDB symbol reader attempts to search the
        // hard disk for files.
        lastErrorMode = SetErrorMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);

        SafeComHolder<ISymUnmanagedReader> pReader;

        if (fInMemorySymbols) 
        {
            SafeComHolder<IStream> pIStream( NULL );

            // If debug stream is already specified, don't bother to go through fusion
            if (GetInMemorySymbolStream() )
            {
                // Case 3 - byte-array or ref-emit.

                // If this is reflection-emit, we need to flush the writer
                if( IsReflection() )
                {
                    ISymUnmanagedWriter* pWriter = GetReflectionModule()->GetISymUnmanagedWriter();
                    if( pWriter != NULL )
                    {
                        SafeComHolder<ISymUnmanagedWriter3> pWriter3;
                        HRESULT thr = pWriter->QueryInterface(IID_ISymUnmanagedWriter3, (void**)&pWriter3);
                        _ASSERTE( SUCCEEDED(thr) );
                        if( SUCCEEDED(thr) )
                        {
                            thr = pWriter3->Commit();
                            _ASSERTE( SUCCEEDED(thr) || thr == E_OUTOFMEMORY );
                        }
                    }
                }

                pIStream = GetInMemorySymbolStream();
                pIStream->AddRef();
            }
            else
            {
                if (CLRHosted())
                {
                    pIStream = m_file->GetPdbStream();
                }

                if (pIStream == NULL)
                {
                    // For hosting scenario where pdb is loaded alone with module,
                    // it will be stored at fusion. The following code will retrieve
                    // the pdb stream from fusion.
                    //
                    if (m_file->IsAssembly())
                    {
                        GCX_PREEMP();
                        hr = ((PEAssembly*)m_file)->GetIHostAssembly()->GetAssemblyDebugStream(&pIStream);
                    }
                    else {
                        _ASSERTE(m_file->IsModule());
                        IHostAssemblyModuleImport *pIHAMI;
                        MAKE_WIDEPTR_FROMUTF8(pName, m_file->GetSimpleName());
                        IfFailThrow(m_file->GetAssembly()->GetIHostAssembly()->GetModuleByName(pName, &pIHAMI));
                        hr = pIHAMI->GetModuleDebugStream(&pIStream);
                    }
                }
            }

            if (SUCCEEDED(hr))
            {
                hr = pBinder->GetReaderFromStream(GetRWImporter(), pIStream, &pReader);
            }
        }
        else
        {
            // The assembly is on disk, so try and load symbols based on the path to the assembly (case 1)
            const SString &path = m_file->GetPath();

            // Call Fusion to ensure that any PDB's are shadow copied before
            // trying to get a symbol reader. This has to be done once per
            // Assembly.
            FusionCopyPDBs(path);

            hr = pBinder->GetReaderForFile(GetRWImporter(), path, NULL, &pReader);
        }

        SetErrorMode(lastErrorMode);

        if (SUCCEEDED(hr))
        {
            NewHolder<HelpForInterfaceCleanup> hlp(new HelpForInterfaceCleanup);

            hlp->pData = GetRWImporter();
            hlp->pFunction = ReleaseImporterFromISymUnmanagedReader;

            IfFailThrow(Module::TrackIUnknownForDelete((IUnknown*)pReader,
                                            NULL,
                                            hlp));

            hlp.SuppressRelease();
            m_pISymUnmanagedReader = pReader.Extract();

            LOG((LF_CORDB, LL_INFO10, "M::GISUR: Loaded symbols for module %S\n", GetDebugName()));
        }
    }

    if (m_pISymUnmanagedReader == NULL)
    {
        LOG((LF_CORDB, LL_INFO10, "M::GISUR: Failed to load symbols for module %S\n", GetDebugName()));
        m_pISymUnmanagedReader = (ISymUnmanagedReader*)0x01; // Failed to load.
    }

    // Make checks that don't have to be done under lock
    if (m_pISymUnmanagedReader == (ISymUnmanagedReader *)0x01)
        RETURN (NULL);
    else
        RETURN (m_pISymUnmanagedReader);
}


BOOL Module::IsSymbolReadingEnabled()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // This can be set by the SymbolReadingPolicy config switch or by a host via
    // ICLRDebugManager.AllowFileLineInfo.
    
    ESymbolReadingPolicy policy = CCLRDebugManager::GetSymbolReadingPolicy();
    if( policy == eSymbolReadingFullTrustOnly )
    {
        // We're only supposed to read symbols corresponding to full-trust assemblies.
        // Note that there is no strong (cryptographic) connection between a symbol file and it's assembly.
        // The intent here is just to ensure that the common high-risk scenarios (IEExec, AppLaunch, etc)
        // will never be able to load untrusted PDB files.
        if( Security::CanCallUnmanagedCode( this ) )
        {
            return TRUE;
        }
    } 
    else if( policy == eSymbolReadingAlways )
    {
        return TRUE;
    }
    else
    {        
        _ASSERTE( policy == eSymbolReadingNever );
    }

    return FALSE;
}

// At this point, this is only called when we're creating an appdomain
// out of an array of bytes, so we'll keep the IStream that we create
// around in case the debugger attaches later (including detach & re-attach!)
HRESULT Module::SetSymbolBytes(BYTE *pbSyms, DWORD cbSyms)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END


    HRESULT hr = S_OK;

    // Create a IStream from the memory for the syms.
    CGrowableStream *pStream = new (nothrow) CGrowableStream();
    if (pStream == NULL)
        return E_OUTOFMEMORY;

    // Do not need to AddRef the CGrowableStream because the constructor set it to 1
    // ref count already. The Module will keep a copy for its own use.

    // Make sure to set the symbol stream on the module before
    // attempting to send UpdateModuleSyms messages up for it.
    SetInMemorySymbolStream(pStream);

#ifdef LOGGING 
    LPCWSTR pName = NULL;
    pName = GetDebugName();
#endif // LOGGING

    ULONG cbWritten;
    IfFailRet(HRESULT_FROM_WIN32(pStream->Write((const void *)pbSyms,
                                                (ULONG)cbSyms,
                                                &cbWritten)));

    // clear the old one if exist
    if (m_pISymUnmanagedReader)
    {
        m_pISymUnmanagedReader->Release();
        m_pISymUnmanagedReader = NULL; // We'll try again next time.
    }

    // Don't eager load the diasymreader
    BEGIN_EXCEPTION_GLUE(&hr, NULL)
    {

#ifdef DEBUGGING_SUPPORTED 
        // Tell the debugger that symbols have been loaded for this
        // module.  We iterate through all domains which contain this
        // module's assembly, and send a debugger notify for each one.
        if (CORDebuggerAttached())
        {
            AppDomainIterator i(FALSE);

            while (i.Next())
            {
                AppDomain *pDomain = i.GetDomain();

                if (pDomain->IsDebuggerAttached() && (GetDomain() == SystemDomain::System() ||
                                                      pDomain->ContainsAssembly(m_pAssembly)))
                    g_pDebugInterface->UpdateModuleSyms(this, pDomain, FALSE);
            }
        }
#endif // DEBUGGING_SUPPORTED
    }
    END_EXCEPTION_GLUE
    return hr;
}


// Distinguish between the fake class associated with the module (global fields &
// functions) from normal classes.
void Module::AddClass(mdTypeDef classdef)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(!IsResource());
    }
    CONTRACTL_END;

    if (RidFromToken(classdef) == 0)
    {
        BuildClassForModule();
    }
    else
    {
        AllocMemTracker amTracker;
        GetClassLoader()->AddAvailableClassDontHaveLock(this,
                                                        classdef,
                                                        &amTracker);
        amTracker.SuppressRelease();
    }
}

//---------------------------------------------------------------------------
// For the global class this builds the table of MethodDescs an adds the rids
// to the MethodDef map.
//---------------------------------------------------------------------------
void Module::BuildClassForModule()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    HENUMInternal   hEnum;
    DWORD           cFunctions, cFields;

    // Obtain count of global functions
    IfFailThrow(GetMDImport()->EnumGlobalFunctionsInit(&hEnum));
    cFunctions = GetMDImport()->EnumGetCount(&hEnum);
    GetMDImport()->EnumClose(&hEnum);

    // Obtain count of global fields
    IfFailThrow(GetMDImport()->EnumGlobalFieldsInit(&hEnum));
    cFields = GetMDImport()->EnumGetCount(&hEnum);
    GetMDImport()->EnumClose(&hEnum);

    // If we have any work to do...
    if (cFunctions > 0 || cFields > 0)
    {
        COUNTER_ONLY(size_t _HeapSize = 0);

        TypeKey typeKey(this, COR_GLOBAL_PARENT_TOKEN);
        GCX_PREEMP();
        TypeHandle typeHnd = GetClassLoader()->LoadTypeHandleForTypeKeyNoLock(&typeKey);

#ifdef ENABLE_PERF_COUNTERS 

        _HeapSize = typeHnd.GetDomain()->GetHighFrequencyHeap()->GetSize();

        GetPrivatePerfCounters().m_Loading.cbLoaderHeapSize = _HeapSize;
#endif // ENABLE_PERF_COUNTERS

    }
}

#endif // !DACCESS_COMPILE

const void *Module::GetIL(DWORD target)
{
    WRAPPER_CONTRACT;

    return m_file->GetIL(target);
}

#ifndef DACCESS_COMPILE 

CHECK Module::CheckIL(RVA il, COUNT_T size)
{
    WRAPPER_CONTRACT;
    // Don't check if we're a reflection module or
    // if we have methods with phony rvas.
    if (!IsReflection()
        && ((m_dwTransientFlags & HAS_PHONY_IL_RVAS) == 0))
        CHECK(m_file->CheckIL(il, size));
    CHECK_OK;
}

CHECK Module::CheckIL(RVA il)
{
    WRAPPER_CONTRACT;
    // Don't check if we're a reflection module or
    // if we have methods with phony rvas.
    if (!IsReflection()
        && ((m_dwTransientFlags & HAS_PHONY_IL_RVAS) == 0))
        CHECK(m_file->CheckIL(il));
    CHECK_OK;
}

BYTE *Module::GetPhonyILBase()
{
    WRAPPER_CONTRACT;

    return m_file->GetPhonyILBase();
}

RVA Module::GetPhonyILRva(BYTE *il)
{
    WRAPPER_CONTRACT;

    // Set a flag on the module that we should be on the lookout
    // for bogus RVAs.  These are only used in full trust scenarios
    // (EnC & profiling) so disabling checks is OK. (If we wanted
    // to be really fancy we could remember the specific phony RVAs
    // but it's probably not worth it.)
    FastInterlockOr(&m_dwTransientFlags, HAS_PHONY_IL_RVAS);

    return m_file->GetPhonyILRva(il);
}

#endif // !DACCESS_COMPILE

void *Module::GetRvaField(DWORD rva)
{
    WRAPPER_CONTRACT;


    return m_file->GetRvaField(rva);
}

#ifndef DACCESS_COMPILE 

CHECK Module::CheckRvaField(RVA field)
{
    WRAPPER_CONTRACT;
    if (!IsReflection())
        CHECK(m_file->CheckRvaField(field));
    CHECK_OK;
}

CHECK Module::CheckRvaField(RVA field, COUNT_T size)
{
    WRAPPER_CONTRACT;
    if (!IsReflection())
        CHECK(m_file->CheckRvaField(field, size));
    CHECK_OK;
}

#endif // !DACCESS_COMPILE

BOOL Module::HasTls()
{
    WRAPPER_CONTRACT;

    return m_file->HasTls();
}

BOOL Module::IsRvaFieldTls(DWORD rva)
{
    WRAPPER_CONTRACT;

    return m_file->IsRvaFieldTls(rva);
}

UINT32 Module::GetFieldTlsOffset(DWORD rva)
{
    WRAPPER_CONTRACT;

    return m_file->GetFieldTlsOffset(rva);
}

UINT32 Module::GetTlsIndex()
{
    WRAPPER_CONTRACT;

    return m_file->GetTlsIndex();
}

PCCOR_SIGNATURE Module::GetSignature(RVA signature)
{
    WRAPPER_CONTRACT;

    return m_file->GetSignature(signature);
}

RVA Module::GetSignatureRva(PCCOR_SIGNATURE signature)
{
    WRAPPER_CONTRACT;

    return m_file->GetSignatureRva(signature);
}

#ifndef DACCESS_COMPILE 

CHECK Module::CheckSignatureRva(RVA signature)
{
    WRAPPER_CONTRACT;
    CHECK(m_file->CheckSignatureRva(signature));
    CHECK_OK;
}

CHECK Module::CheckSignature(PCCOR_SIGNATURE signature)
{
    WRAPPER_CONTRACT;
    CHECK(m_file->CheckSignature(signature));
    CHECK_OK;
}

void Module::InitializeStringData(DWORD token, EEStringData *pstrData, CQuickBytes *pqb)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(TypeFromToken(token) == mdtString);
    }
    CONTRACTL_END;

    if(!GetMDImport()->IsValidToken(token))
    {
        THROW_BAD_FORMAT(BFA_BAD_STRING_TOKEN, this);
    }

    BOOL fIs80Plus;
    DWORD dwCharCount;
    LPCWSTR pString = GetMDImport()->GetUserString(token, &dwCharCount, &fIs80Plus);

    if (!pString)
    {
        THROW_BAD_FORMAT(BFA_BAD_STRING_TOKEN_RANGE, this);
    }

#if !BIGENDIAN 
    pstrData->SetStringBuffer(pString);
#else // !!BIGENDIAN
    _ASSERTE(pqb != NULL);

    LPWSTR pSwapped;

    pSwapped = (LPWSTR) pqb->AllocThrows(dwCharCount * sizeof(WCHAR));
    memcpy((void*)pSwapped, (void*)pString, dwCharCount*sizeof(WCHAR));
    SwapStringLength(pSwapped, dwCharCount);

    pstrData->SetStringBuffer(pSwapped);
#endif // !!BIGENDIAN

        // MD and String look at this bit in opposite ways.  Here's where we'll do the conversion.
        // MD sets the bit to true if the string contains characters greater than 80.
        // String sets the bit to true if the string doesn't contain characters greater than 80.

    pstrData->SetCharCount(dwCharCount);
    pstrData->SetIsOnlyLowChars(!fIs80Plus);
}


OBJECTHANDLE Module::ResolveStringRef(DWORD token, BaseDomain *pDomain, bool bNeedToSyncWithFixups)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(TypeFromToken(token) == mdtString);
    }
    CONTRACTL_END;

    EEStringData strData;
    OBJECTHANDLE string = NULL;

#if !BIGENDIAN 
    InitializeStringData(token, &strData, NULL);
#else // !!BIGENDIAN
    CQuickBytes qb;
    InitializeStringData(token, &strData, &qb);
#endif // !!BIGENDIAN

    GCX_COOP();

    if (HasNativeImage() && IsNoStringInterning())
    {
        /* Unfortunately, this assert won't work in some cases of generics, consider the following scenario:

            1) Generic type in mscorlib.
            2) Instantiation of generic (1) (via valuetype) in another module
            3) other module now holds a copy of the code of the generic for that particular instantiation
               however, it is resolving the string literals against mscorlib, which breaks the invariant
               this assert was based on (no string fixups against other modules). In fact, with NoStringInterning,
               our behavior is not very intuitive.
        */
        /*
        _ASSERTE(pDomain == GetAssembly()->GetDomain() && "If your are doing ldstr for a string"
        "in another module, either the JIT is very smart or you have a bug, check INLINE_NO_CALLEE_LDSTR");

        */

        // The caller is going to update an ngen fixup entry. The fixup entry
        // is used to reference the string and to ensure that the string is
        // allocated only once. Hence, this operation needs to be done under a lock.
        _ASSERTE(GetFixupCrst()->OwnedByCurrentThread());

        // Allocate handle
        OBJECTREF* pRef = pDomain->AllocateObjRefPtrsInLargeTable(1);

        STRINGREF str = AllocateStringObject(&strData);
        SetObjectReference(pRef, str, NULL);

        #ifdef LOGGING 
        int length = strData.GetCharCount();
        length = min(length, 100);
        WCHAR *szString = (WCHAR *)_alloca((length + 1) * sizeof(WCHAR));
        memcpyNoGCRefs((void*)szString, (void*)strData.GetStringBuffer(), length * sizeof(WCHAR));
        szString[length] = '\0';
        LOG((LF_APPDOMAIN, LL_INFO10000, "String literal \"%S\" won't be interned due to NoInterningAttribute\n", szString));
        #endif // LOGGING

        return (OBJECTHANDLE) pRef;
    }

    // Retrieve the string from the AppDomain.
    string = (OBJECTHANDLE)pDomain->GetStringObjRefPtrFromUnicodeString(&strData);

    return string;
}

//
// Used by the verifier.  Returns whether this stringref is valid.
//
CHECK Module::CheckStringRef(DWORD token)
{
    WRAPPER_CONTRACT;
    CHECK(TypeFromToken(token)==mdtString);
    CHECK(!IsNilToken(token));
    CHECK(GetMDImport()->IsValidToken(token));
    CHECK_OK;
}

mdToken Module::GetEntryPointToken()
{
    WRAPPER_CONTRACT;

    return m_file->GetEntryPointToken();
}

BYTE *Module::GetProfilerBase()
{
    CONTRACT(BYTE*)
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    if (m_file == NULL)  // I'd rather assert this is not the case...
    {
        RETURN NULL;
    }
    else if (HasNativeImage())
    {
        RETURN (BYTE*)(GetNativeImage()->GetBase());
    }
    else if (m_file->IsLoaded())
    {
        RETURN  (BYTE*)(m_file->GetLoadedIL()->GetBase());
    }
    else
    {
        RETURN NULL;
    }
}

BOOL Module::AddActiveDependency(Module *pModule, BOOL unconditional)
{
    CONTRACT(BOOL)
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(pModule != this);
        PRECONDITION(!IsSystem());
        PRECONDITION(!GetAssembly()->IsDomainNeutral() || pModule->GetAssembly()->IsDomainNeutral());
        POSTCONDITION(HasActiveDependency(pModule));
        POSTCONDITION(!unconditional || HasUnconditionalActiveDependency(pModule));
        // Postcondition about activation
    }
    CONTRACT_END;

    CONSISTENCY_CHECK_MSG(!GetAssembly()->IsDomainNeutral() || pModule->GetAssembly()->IsDomainNeutral(),
                          "Active dependency from domain neutral to domain bound is illegal");

    // We must track this dependency for multiple domains' use
    STRESS_LOG2(LF_CLASSLOADER, LL_INFO100," %08x -> %08x",this,pModule);
    _ASSERTE(!unconditional || pModule->HasNativeImage()); 
    _ASSERTE(!unconditional || HasNativeImage()); 
    
    COUNT_T index;

    // this function can run in parallel with DomainFile::Activate and sychronizes via GetNumberOfActivations()
    // because we expose dependency only in the end Domain::Activate might miss it, but it will increment a counter module
    // so we can realize we have to additionally propagate a dependency into that appdomain.
    // currently we do it just by rescanning al appdomains.
    // needless to say, updating the counter and checking counter+adding dependency to the list should be atomic


    BOOL propagate = FALSE;
    ULONG startCounter=0;
    ULONG endCounter=0;    
    do
    {
        // First, add the dependency to the physical dependency list
        {
            CrstPreempHolder lock(&m_Crst);
            startCounter=GetNumberOfActivations();

            index = m_activeDependencies.FindElement(0, pModule);
            if (index == (COUNT_T) ArrayList::NOT_FOUND)
            {
                propagate = TRUE;
                STRESS_LOG3(LF_CLASSLOADER, LL_INFO100,"Adding new module dependency %08x -> %08x, unconditional=%i",this,pModule,unconditional);
            }

            if (unconditional)
            {
#ifdef _DEBUG 
                lock.Release();
                CHECK check=DomainFile::CheckUnactivatedInAllDomains(this);
                lock.Acquire();

                if (propagate)
                CONSISTENCY_CHECK_MSG(check,
                                      "Unconditional dependency cannot be added after module has already been activated");
#endif // _DEBUG
                index = m_activeDependencies.GetCount();
                m_activeDependencies.Append(pModule);
                m_unconditionalDependencies.SetBit(index);
                STRESS_LOG2(LF_CLASSLOADER, LL_INFO100," Unconditional module dependency propagated %08x -> %08x",this,pModule);
                // Now other threads can skip this dependency without propagating.
                RETURN propagate;
            }

        }

        // Now we have to propagate any module activations in the loader

        if (propagate)
        {

            _ASSERTE(!unconditional);
            DomainFile::PropagateNewActivation(this, pModule);

            CrstPreempHolder lock(&m_Crst);
            STRESS_LOG2(LF_CLASSLOADER, LL_INFO100," Conditional module dependency propagated %08x -> %08x",this,pModule);
            // Now other threads can skip this dependency without propagating.
            endCounter=GetNumberOfActivations();
            if(startCounter==endCounter)
                m_activeDependencies.Append(pModule);
        }
        
    }while(propagate && startCounter!=endCounter); //need to retry if someone was activated in parallel
    RETURN propagate;
}

BOOL Module::HasActiveDependency(Module *pModule)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END;

    if (pModule == this)
        return TRUE;

    DependencyIterator i = IterateActiveDependencies();
    while (i.Next())
    {
        if (i.GetDependency() == pModule)
            return TRUE;
    }

    return FALSE;
}

BOOL Module::HasUnconditionalActiveDependency(Module *pModule)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END;

    if (pModule == this)
        return TRUE;

    DependencyIterator i = IterateActiveDependencies();
    while (i.Next())
    {
        if (i.GetDependency() == pModule
            && i.IsUnconditional())
            return TRUE;
    }

    return FALSE;
}

BOOL Module::AddClassDependency(Module *pModule, BitMask *mask)
{
    CONTRACT(BOOL)
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(pModule != this);
    }
    CONTRACT_END;

    if (pModule->IsSystem() ||
        HasUnconditionalActiveDependency(pModule))
        RETURN FALSE;

    COUNT_T index;

    // Add the dependency to the physical dependency list

    {
        CrstPreempHolder lock(&m_Crst);

        //

        index = m_classDependencies.FindElement(0, pModule);
        if (index == (COUNT_T) ArrayList::NOT_FOUND)
        {
            index = m_classDependencies.GetCount();
            m_classDependencies.Append(pModule);
        }
    }

    mask->SetBit(index);

    RETURN TRUE;
}


void Module::EnableModuleFailureTriggers(Module *pModuleTo, AppDomain *pDomain)
{
    // At this point we need to enable failure triggers we have placed in the code for this module.  However,
    // the failure trigger codegen logic is NYI.  To keep correctness, we just allow the exception to propagate
    // here.  Note that in general this will enforce the failure invariants, but will also result in some rude
    // behavior as these failures will be propagated too widely rather than constrained to the appropriate
    // assemblies/app domains.
    //
    // This should throw.
    DomainFile *pDomainFileTo = pModuleTo->GetDomainFile(pDomain);
    pDomainFileTo->EnsureActive();

    UNREACHABLE_MSG("Module failure triggers NYI");
}

#endif // !DACCESS_COMPILE

Assembly *Module::GetAssemblyIfLoaded(mdAssemblyRef kAssemblyRef)
{
    CONTRACT(Assembly *)
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    Assembly *pAssembly = LookupAssemblyRef(kAssemblyRef);
    if (!pAssembly) {

#ifndef DACCESS_COMPILE 

        AssemblySpec spec;
        if (!spec.InitializeSpec(kAssemblyRef, GetMDImport(), GetAssembly()->GetManifestFile(),
                                 IsIntrospectionOnly(), FALSE /*fThrow*/))
            RETURN NULL;

        DomainAssembly *pDomainAssembly = GetAppDomain()->FindCachedAssembly(&spec, FALSE /*fThrow*/);
        if (pDomainAssembly && pDomainAssembly->IsLoaded())
            pAssembly = pDomainAssembly->GetCurrentAssembly();

        if (pAssembly)
            StoreAssemblyRef(kAssemblyRef, pAssembly);

#else // DACCESS_COMPILE
        DacNotImpl();
#endif // DACCESS_COMPILE
    }

    RETURN pAssembly;
}

#ifndef DACCESS_COMPILE 




DomainAssembly *Module::LoadAssembly(AppDomain *pDomain, mdAssemblyRef kAssemblyRef)
{
    CONTRACT(DomainAssembly *)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pDomain));
        POSTCONDITION(CheckPointer(RETVAL, NULL_NOT_OK));
    }
    CONTRACT_END;

    ETWTraceStartup trace(ETW_TYPE_STARTUP_LOADERCATCHALL);

    Assembly *pAssembly = LookupAssemblyRef(kAssemblyRef);

    DomainAssembly *pDomainAssembly;
    if (pAssembly != NULL)
    {
        pDomainAssembly = pAssembly->FindDomainAssembly(pDomain);

        if (pDomainAssembly == NULL)
            pDomainAssembly=pAssembly->GetDomainAssembly(pDomain);
        pDomain->LoadDomainFile(pDomainAssembly, FILE_LOADED);
    }
    else if (IsIntrospectionOnly())
    {
        AssemblySpec spec;
        spec.InitializeSpec(kAssemblyRef, GetMDImport(), GetDomainFile(GetAppDomain())->GetFile()->GetAssembly(), IsIntrospectionOnly());
        pDomainAssembly = GetAppDomain()->BindAssemblySpecForIntrospectionDependencies(&spec);
    }
    else
    {
        PEAssemblyHolder pFile(GetDomainFile(GetAppDomain())->GetFile()->LoadAssembly(kAssemblyRef));
        AssemblySpec spec;
        spec.InitializeSpec(kAssemblyRef, GetMDImport(), GetDomainFile(GetAppDomain())->GetFile()->GetAssembly(), IsIntrospectionOnly());
        pDomainAssembly = GetAppDomain()->LoadDomainAssembly(&spec, pFile, FILE_LOADED, NULL, NULL);
    }

    if (pDomainAssembly != NULL)
    {
        if (pAssembly == NULL)
        {
            if (pDomainAssembly->GetCurrentAssembly() != NULL)
                StoreAssemblyRef(kAssemblyRef, pDomainAssembly->GetCurrentAssembly());
        }
        else
        {
            CONSISTENCY_CHECK(pAssembly == pDomainAssembly->GetAssembly());
        }
    }

    RETURN pDomainAssembly;
}

Module *Module::GetModuleIfLoaded(mdFile kFile, BOOL permitResources/*=TRUE*/)
{
    CONTRACT(Module *)
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(TypeFromToken(kFile) == mdtFile
                     || TypeFromToken(kFile) == mdtModuleRef);
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        FORBID_FAULT;
    }
    CONTRACT_END;

    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    // Handle the module ref case
    if (TypeFromToken(kFile) == mdtModuleRef)
    {
        LPCSTR moduleName;
        GetMDImport()->GetModuleRefProps(kFile, &moduleName);

        // This is required only because of some lower casing on the name
        kFile = GetAssembly()->GetManifestFileToken(moduleName);
        if (kFile == mdTokenNil)
            RETURN NULL;

        RETURN GetAssembly()->GetManifestModule()->GetModuleIfLoaded(kFile, permitResources);
    }

    Module *pModule = LookupFile(kFile);
    if (pModule == NULL)
    {
        if (IsManifest())
        {
            if (kFile == mdFileNil)
                pModule = GetAssembly()->GetManifestModule();
        }
        else
        {
            // If we didn't find it there, look at the "master rid map" in the manifest file
            Assembly *pAssembly = GetAssembly();
            mdFile kMatch;

            // This is required only because of some lower casing on the name
            kMatch = pAssembly->GetManifestFileToken(GetMDImport(), kFile);
            if (IsNilToken(kMatch)) {
                if (kMatch == mdFileNil)
                    pModule = pAssembly->GetManifestModule();
                else
                    RETURN NULL;
            }
            else
            pModule = pAssembly->GetManifestModule()->LookupFile(kMatch);
        }

        if (pModule)
            StoreFileNoThrow(kFile, pModule);
    }

    // We may not want to return a resource module
    if (!permitResources && pModule && pModule->IsResource())
        pModule = NULL;

    RETURN pModule;
}

DomainFile *Module::LoadModule(AppDomain *pDomain, mdFile kFile,
                               BOOL permitResources/*=TRUE*/, BOOL bindOnly/*=FALSE*/)
{
    CONTRACT(DomainFile *)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(TypeFromToken(kFile) == mdtFile
                     || TypeFromToken(kFile) == mdtModuleRef);
        POSTCONDITION(CheckPointer(RETVAL, !permitResources || bindOnly ? NULL_OK : NULL_NOT_OK));
    }
    CONTRACT_END;


    // Handle the module ref case
    if (TypeFromToken(kFile) == mdtModuleRef)
    {
        LPCSTR moduleName;
        GetMDImport()->GetModuleRefProps(kFile, &moduleName);
        mdFile kFileLocal = GetAssembly()->GetManifestFileToken(moduleName);

        if (kFileLocal == mdTokenNil)
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT);

        RETURN GetAssembly()->GetManifestModule()->LoadModule(pDomain, kFileLocal, permitResources, bindOnly);
    }

    // First, make sure the assembly is loaded in our domain

    DomainAssembly *pDomainAssembly = GetAssembly()->FindDomainAssembly(pDomain);
    if (!bindOnly)
    {
        if (pDomainAssembly == NULL)
            pDomainAssembly = GetAssembly()->GetDomainAssembly(pDomain);
        pDomain->LoadDomainFile(pDomainAssembly, FILE_LOADED);
    }

    if (kFile == mdFileNil)
        RETURN pDomainAssembly;

    if (pDomainAssembly == NULL)
        RETURN NULL;

    // Now look for the module in the rid maps

    Module *pModule = LookupFile(kFile);
    if (pModule == NULL && !IsManifest())
    {
        // If we didn't find it there, look at the "master rid map" in the manifest file
        Assembly *pAssembly = GetAssembly();
        mdFile kMatch = pAssembly->GetManifestFileToken(GetMDImport(), kFile);
        if (IsNilToken(kMatch)) {
            if (kMatch == mdFileNil)
                pModule = pAssembly->GetManifestModule();
            else
                COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
        }
        else
            pModule = pAssembly->GetManifestModule()->LookupFile(kMatch);
    }

    // Get a DomainModule for our domain

    DomainModule *pDomainModule = NULL;
    if (pModule)
    {
        pDomainModule = pModule->FindDomainModule(pDomain);

        if (!bindOnly && (permitResources || !pModule->IsResource()))
        {
            if (pDomainModule == NULL)
                pDomainModule = pDomain->LoadDomainModule(pDomainAssembly, (PEModule*) pModule->GetFile(), FILE_LOADED);
            else
                pDomain->LoadDomainFile(pDomainModule, FILE_LOADED);
        }
    }
    else if (!bindOnly)
    {
        PEModuleHolder pFile(GetAssembly()->LoadModule(kFile, permitResources));
        if (pFile)
            pDomainModule = pDomain->LoadDomainModule(pDomainAssembly, pFile, FILE_LOADED);
    }

    // Cache the result in the rid map
    if (pDomainModule != NULL && pDomainModule->GetCurrentModule() != NULL)
        StoreFileThrowing(kFile, pDomainModule->GetCurrentModule());

    // Make sure we didn't load a different module than what was in the rid map
    CONSISTENCY_CHECK(pDomainModule == NULL || pModule == NULL || pDomainModule->GetModule() == pModule);

    // We may not want to return a resource module
    if (!permitResources && pDomainModule != NULL && pDomainModule->GetFile()->IsResource())
        pDomainModule = NULL;

    RETURN pDomainModule;
}


Module* Module::LookupModule(mdToken kFile,BOOL permitResources/*=TRUE*/)
{
    CONTRACT(Module *)
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM();); }
        MODE_ANY;
        PRECONDITION(TypeFromToken(kFile) == mdtFile
                     || TypeFromToken(kFile) == mdtModuleRef);
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    if (TypeFromToken(kFile) == mdtModuleRef)
    {
        LPCSTR moduleName;
        IfFailThrow(GetMDImport()->GetModuleRefProps(kFile, &moduleName));
        mdFile kFileLocal = GetAssembly()->GetManifestFileToken(moduleName);

        if (kFileLocal == mdTokenNil)
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT);

        RETURN GetAssembly()->GetManifestModule()->LookupModule(kFileLocal, permitResources);
    }

    Module *pModule = LookupFile(kFile);
    if (pModule == NULL && !IsManifest())
    {
        // If we didn't find it there, look at the "master rid map" in the manifest file
        Assembly *pAssembly = GetAssembly();
        mdFile kMatch = pAssembly->GetManifestFileToken(GetMDImport(), kFile);
        if (IsNilToken(kMatch)) {
            if (kMatch == mdFileNil)
                pModule = pAssembly->GetManifestModule();
            else
                COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
        }
        else
            pModule = pAssembly->GetManifestModule()->LookupFile(kMatch);
    }
    RETURN pModule;
}
#endif // !DACCESS_COMPILE


TypeHandle Module::LookupTypeRef(mdTypeRef token, ClassLoadLevel level)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    _ASSERTE(TypeFromToken(token) == mdtTypeRef);

    g_IBCLogger.LogRidMapAccess( MakePair( this, token ) );

    TypeHandle entry = TypeHandle::FromData(GetFromRidMap(&m_TypeRefToMethodTableMap,
                                                  RidFromToken(token)));

    if (entry.IsNull() || entry.GetLoadLevel() < level)
        return TypeHandle();

    // Cannot do this in a NOTHROW function.
    // Note that this could be called while doing GC from the prestub of
    // a method to resolve typerefs in a signature. We cannot THROW
    // during GC.

    // @PERF: Enable this so that we do not need to touch metadata
    // to resolve typerefs


    return entry;
}




#ifndef DACCESS_COMPILE 

// Increase the size of one of the maps, such that it can handle a RID of at least "rid".
//
// This function must also check that another thread didn't already add a LookupMap capable
// of containing the same RID.
//
LookupMap *Module::IncMapSize(LookupMap *pMap, DWORD rid)
{
    CONTRACT(LookupMap *)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(ThrowOutOfMemory(););
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    LookupMap  *pPrev = NULL;
    DWORD       dwPrevMaxIndex = 0;
    LookupMap  *pNewMap;

    {
        CrstHolder ch(&m_LookupTableCrst);
        // Check whether we can already handle this RID index
        do
        {
            if (rid < pMap->dwMaxIndex)
            {
                // Already there - some other thread must have added it
                RETURN pMap;
            }

            dwPrevMaxIndex = pMap->dwMaxIndex;
            pPrev = pMap;
            pMap = pMap->pNext;
        } while (pMap != NULL);

        _ASSERTE(pPrev != NULL); // should never happen, because there's always at least one map

        DWORD dwMinNeeded = rid - dwPrevMaxIndex + 1; // Min # elements required for this chunk
        DWORD dwBlockSize = *pPrev->pdwBlockSize;   // Min # elements required by block size
        DWORD dwSizeToAllocate;                     // Actual number of elements we will allocate

        if (dwMinNeeded > dwBlockSize)
        {
            dwSizeToAllocate = dwMinNeeded;
        }
        else
        {
            dwSizeToAllocate = dwBlockSize;
            dwBlockSize <<= 1;                      // Increase block size
            *pPrev->pdwBlockSize = dwBlockSize;
        }

        MEMORY_REPORT_CONTEXT_SCOPE("RidMap");

        _ASSERTE (m_pAssembly && m_pAssembly->GetLowFrequencyHeap());

        pNewMap = (LookupMap *) (void*)m_pAssembly->GetLowFrequencyHeap()->AllocMem(sizeof(LookupMap) + dwSizeToAllocate*sizeof(void*));

        // Note: Memory allocated on loader heap is zero filled
        // memset(pNewMap, 0, sizeof(LookupMap) + dwSizeToAllocate*sizeof(void*));

        pNewMap->pNext          = NULL;
        pNewMap->dwMaxIndex     = dwPrevMaxIndex + dwSizeToAllocate;
        pNewMap->pdwBlockSize   = pPrev->pdwBlockSize;

        // pTable is not a pointer to the beginning of the table.  Rather, anyone who uses Table can
        // simply index off their RID (as long as their RID is < dwMaxIndex, and they are not serviced
        // by a previous table for lower RIDs).
        pNewMap->pTable         = ((TADDR *) (pNewMap + 1)) - dwPrevMaxIndex;

        // Link ourselves in

        pPrev->pNext            = pNewMap;
    }

    RETURN pNewMap;
}


// Stores an association in a ridmap that has been previously grown to
// the required size. Will never throw or fail.
void Module::SetInRidMap(LookupMap *pMap, DWORD rid, SIZE_T pDatum)
{
    WRAPPER_CONTRACT;

    for(;;)
    {
        _ASSERTE(pMap != NULL && "The onus is on my caller to make sure the ridmap was already grown to the right size");

        if (rid < pMap->dwMaxIndex)
        {
            pMap->pTable[rid] = pDatum;
            return;
        }

        pMap = pMap->pNext;
    }
}

BOOL Module::TryAddToRidMap(LookupMap *pMap, DWORD rid, TADDR pDatum)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pMap));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    do
    {
        if (rid < pMap->dwMaxIndex)
        {
            pMap->pTable[rid] = pDatum;
            return TRUE;
        }

        pMap = pMap->pNext;
    } while (pMap != NULL);

    return FALSE;
}

void Module::AddToRidMap(LookupMap *pMap, DWORD rid, TADDR pDatum)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pMap));
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(ThrowOutOfMemory(););
    }
    CONTRACTL_END;


    LookupMap *pMapStart = pMap;

    do
    {
        if (rid < pMap->dwMaxIndex)
        {
            pMap->pTable[rid] = pDatum;
            return;
        }

        pMap = pMap->pNext;
    } while (pMap != NULL);

    pMap = IncMapSize(pMapStart, rid);
    pMap->pTable[rid] = pDatum;
}

#endif // !DACCESS_COMPILE

TADDR* Module::RidToDefHandle(LookupMap *pMap, DWORD rid)
{
    CONTRACT(TADDR*)
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMap));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END

    do
    {
        if (rid < pMap->dwMaxIndex)
        {
            TADDR* result = &(pMap->pTable[rid]);
            if (*result == NULL && !HasNativeImage())
            {
                *result = (TADDR)this | (TADDR)LookupMap::IS_MODULE_BACKPOINTER;
            }
            RETURN result;
        }

        pMap = pMap->pNext;
    } while (pMap != NULL);

    RETURN(NULL);
}

TypeDefHandle Module::TypeDefToTypeDefHandle(mdTypeDef typeDef)
{
    CONTRACT(TypeDefHandle)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INSTANCE_CHECK;
        PRECONDITION(TypeFromToken(typeDef) == mdtTypeDef);
        PRECONDITION(GetMDImport()->IsValidToken(typeDef));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END

    RETURN (TypeDefHandle) RidToDefHandle(&m_TypeDefToMethodTableMap, RidFromToken(typeDef));
}

MethodDefHandle Module::MethodDefToMethodDefHandle(mdMethodDef methodDef)
{
    CONTRACT(MethodDefHandle)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INSTANCE_CHECK;
        PRECONDITION(TypeFromToken(methodDef) == mdtMethodDef);
        PRECONDITION(GetMDImport()->IsValidToken(methodDef));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END

    RETURN (MethodDefHandle) RidToDefHandle(&m_MethodDefToDescMap, RidFromToken(methodDef));
}

/*static*/
Module* Module::GetModuleForTypeDefHandle(TypeDefHandle handle)
{
    CONTRACT(Module*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(handle));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END

    TADDR entry = *((TADDR*) handle);
    if (entry == NULL)
    {
        RETURN (ExecutionManager::FindZapModule((TADDR) handle));
    }
    else if (entry & LookupMap::IS_MODULE_BACKPOINTER)
    {
        RETURN ((Module*) (entry & ~(TADDR)LookupMap::IS_MODULE_BACKPOINTER));
    }
    else
    {
        MethodTable* pMT = (MethodTable*) entry;
        RETURN pMT->GetModule();
    }
}


/*static*/
Module* Module::GetModuleForMethodDefHandle(MethodDefHandle handle)
{
    CONTRACT(Module*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(handle));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END

    TADDR entry = *((TADDR*) handle);
    if (entry == NULL)
    {
        RETURN (ExecutionManager::FindZapModule((TADDR) handle));
    }
    else if (entry & LookupMap::IS_MODULE_BACKPOINTER)
    {
        RETURN ((Module*) (entry & ~(TADDR)LookupMap::IS_MODULE_BACKPOINTER));
    }
    else
    {
        MethodDesc* pMD = (MethodDesc*) entry;
        RETURN pMD->GetModule();
    }
}


mdTypeDef Module::GetTypeDefForTypeDefHandle(TypeDefHandle handle)
{
    CONTRACT(mdTypeDef)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(handle));
        POSTCONDITION(!IsNilToken(RETVAL));
        POSTCONDITION(TypeFromToken(RETVAL) == mdtTypeDef);
        POSTCONDITION(GetMDImport()->IsValidToken(RETVAL));
    }
    CONTRACT_END

    TADDR entry = *((TADDR*) handle);
    if (entry == NULL || (entry & LookupMap::IS_MODULE_BACKPOINTER) != 0)
    {
        DWORD rid = GetRidFromMapAddress(&m_TypeDefToMethodTableMap, (TADDR*) handle);
        RETURN (TokenFromRid(rid, mdtTypeDef));
    }
    else
    {
        MethodTable* pMT = (MethodTable*) entry;
        mdTypeDef typeDef = pMT->GetCl();
        _ASSERTE(GetRidFromMapAddress(&m_TypeDefToMethodTableMap, (TADDR*) handle) == RidFromToken(typeDef));
        RETURN typeDef;
    }
}

mdMethodDef Module::GetMethodDefForMethodDefHandle(MethodDefHandle handle)
{
    CONTRACT(mdMethodDef)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(handle));
        POSTCONDITION(!IsNilToken(RETVAL));
        POSTCONDITION(TypeFromToken(RETVAL) == mdtMethodDef);
        POSTCONDITION(GetMDImport()->IsValidToken(RETVAL));
    }
    CONTRACT_END

    TADDR entry = *((TADDR*) handle);
    if (entry == NULL || (entry & LookupMap::IS_MODULE_BACKPOINTER) != 0)
    {
        DWORD rid = GetRidFromMapAddress(&m_MethodDefToDescMap, (TADDR*) handle);
        RETURN TokenFromRid(rid, mdtMethodDef);
    }
    else
    {
        MethodDesc* pMD = (MethodDesc*) entry;
        mdMethodDef methodDef = pMD->GetMemberDef();
        _ASSERTE(GetRidFromMapAddress(&m_MethodDefToDescMap, (TADDR*) handle) == RidFromToken(methodDef));
        RETURN methodDef;
    }
}


DWORD Module::GetRidFromMapAddress(LookupMap *pMap, TADDR* addr)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMap));
    }
    CONTRACTL_END

    DWORD index = 0;
    do
    {
        DWORD rid = addr - pMap->pTable;
        if (rid >= index && rid < pMap->dwMaxIndex)
        {
            return rid;
        }
        index = pMap->dwMaxIndex;
        pMap = pMap->pNext;
    } while (pMap != NULL);

    return 0;
}



TADDR Module::GetFromRidMap(LookupMap *pMap, DWORD rid)
{
    CONTRACT(TADDR)
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pMap));
        POSTCONDITION(CheckPointer((void*)RETVAL, NULL_OK));
    }
    CONTRACT_END


    TADDR result = NULL;
    do
    {
        if (rid < pMap->dwMaxIndex)
        {
            result = pMap->pTable[rid];
            break;
        }

        pMap = pMap->pNext;
    } while (pMap != NULL);

    if (result & (TADDR)LookupMap::IS_MODULE_BACKPOINTER)
        result = NULL;


    RETURN(result);
}

#ifndef DACCESS_COMPILE 

#ifdef _DEBUG 
void Module::DebugGetRidMapOccupancy(LookupMap *pMap, DWORD *pdwOccupied, DWORD *pdwSize)
{
    LEAF_CONTRACT;

    DWORD       dwMinIndex = 0;

    *pdwOccupied = 0;
    *pdwSize     = 0;

    if(pMap == NULL) return;

    // Go through each linked block
    for (; pMap != NULL; pMap = pMap->pNext)
    {
        DWORD i;
        DWORD dwIterCount = pMap->dwMaxIndex - dwMinIndex;
        TADDR *pRealTableStart = &pMap->pTable[dwMinIndex];

        for (i = 0; i < dwIterCount; i++)
        {
            if (pRealTableStart[i] != NULL && (pRealTableStart[i] & (TADDR)LookupMap::IS_MODULE_BACKPOINTER) == 0)
                (*pdwOccupied)++;
        }

        (*pdwSize) += dwIterCount;

        dwMinIndex = pMap->dwMaxIndex;
    }
}

void Module::DebugLogRidMapOccupancy()
{
    WRAPPER_CONTRACT;

    DWORD dwOccupied1, dwSize1, dwPercent1;
    DWORD dwOccupied2, dwSize2, dwPercent2;
    DWORD dwOccupied3, dwSize3, dwPercent3;
    DWORD dwOccupied4, dwSize4, dwPercent4;
    DWORD dwOccupied5, dwSize5, dwPercent5;
    DWORD dwOccupied6, dwSize6, dwPercent6;
    DWORD dwOccupied7, dwSize7, dwPercent7;

    DebugGetRidMapOccupancy(&m_TypeDefToMethodTableMap, &dwOccupied1, &dwSize1);
    DebugGetRidMapOccupancy(&m_TypeRefToMethodTableMap, &dwOccupied2, &dwSize2);
    DebugGetRidMapOccupancy(&m_MethodDefToDescMap, &dwOccupied3, &dwSize3);
    DebugGetRidMapOccupancy(&m_FieldDefToDescMap, &dwOccupied4, &dwSize4);
    DebugGetRidMapOccupancy(&m_MemberRefToDescMap, &dwOccupied5, &dwSize5);
    DebugGetRidMapOccupancy(&m_FileReferencesMap, &dwOccupied6, &dwSize6);
    DebugGetRidMapOccupancy(&m_ManifestModuleReferencesMap, &dwOccupied7, &dwSize7);

    dwPercent1 = dwOccupied1 ? ((dwOccupied1 * 100) / dwSize1) : 0;
    dwPercent2 = dwOccupied2 ? ((dwOccupied2 * 100) / dwSize2) : 0;
    dwPercent3 = dwOccupied3 ? ((dwOccupied3 * 100) / dwSize3) : 0;
    dwPercent4 = dwOccupied4 ? ((dwOccupied4 * 100) / dwSize4) : 0;
    dwPercent5 = dwOccupied5 ? ((dwOccupied5 * 100) / dwSize5) : 0;
    dwPercent6 = dwOccupied6 ? ((dwOccupied6 * 100) / dwSize6) : 0;
    dwPercent7 = dwOccupied7 ? ((dwOccupied7 * 100) / dwSize7) : 0;

    LOG((
        LF_EEMEM,
        INFO3,
        "   Map occupancy:\n"
        "      TypeDefToEEClass map: %4d/%4d (%2d %%)\n"
        "      TypeRefToEEClass map: %4d/%4d (%2d %%)\n"
        "      MethodDefToDesc map:  %4d/%4d (%2d %%)\n"
        "      FieldDefToDesc map:  %4d/%4d (%2d %%)\n"
        "      MemberRefToDesc map:  %4d/%4d (%2d %%)\n"
        "      FileReferences map:  %4d/%4d (%2d %%)\n"
        "      AssemblyReferences map:  %4d/%4d (%2d %%)\n"
        ,
        dwOccupied1, dwSize1, dwPercent1,
        dwOccupied2, dwSize2, dwPercent2,
        dwOccupied3, dwSize3, dwPercent3,
        dwOccupied4, dwSize4, dwPercent4,
        dwOccupied5, dwSize5, dwPercent5,
        dwOccupied6, dwSize6, dwPercent6,
        dwOccupied7, dwSize7, dwPercent7

    ));
}
#endif // _DEBUG

BOOL Module::CanExecuteCode()
{
    WRAPPER_CONTRACT;

    // In a passive domain, we lock down which assemblies can run code
    if (!GetAppDomain()->IsPassiveDomain())
        return TRUE;

    Assembly * pAssembly = GetAssembly();
    PEAssembly * pPEAssembly = pAssembly->GetManifestFile();

    // ExecuteDLLForAttach does not run the managed entry point in 
    // a passive domain to avoid loader-lock deadlocks.
    // Hence, it is not safe to execute any code from this assembly.
    if (pPEAssembly->GetEntryPointToken(INDEBUG(TRUE)) != mdTokenNil)
        return FALSE;

    // EXEs loaded using LoadAssembly() may not be loaded at their
    // preferred base address. If they have any relocs, these may
    // not have been fixed up.
    if (!pPEAssembly->IsDll() && !pPEAssembly->IsILOnly())
        return FALSE;

    // If the assembly does not have FullTrust, we should not execute its code.
    if (!Security::IsFullyTrusted(pAssembly->GetSecurityDescriptor()))
        return FALSE;

    return TRUE;
}



//
// FindMethod finds a MethodDesc for a global function methoddef or ref
//

MethodDesc *Module::FindMethodThrowing(mdToken pMethod)
{
    CONTRACT (MethodDesc *)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END

    SigTypeContext typeContext;  /* empty type context: methods will not be generic */
    RETURN MemberLoader::GetMethodDescFromMemberDefOrRefOrSpecThrowing(this, pMethod,
                                                                       &typeContext,
                                                                       TRUE, /* strictMetadataChecks */
                                                                       FALSE /* dont get code shared between generic instantiations */);
}

//
// FindMethod finds a MethodDesc for a global function methoddef or ref
//

MethodDesc *Module::FindMethod(mdToken pMethod)
{
    CONTRACT (MethodDesc *) {
        INSTANCE_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    } CONTRACT_END;

    MethodDesc *pMDRet = NULL;

    EX_TRY
    {
        pMDRet = FindMethodThrowing(pMethod);
    }
    EX_CATCH
    {
#ifdef _DEBUG 
        GCX_COOP();
        OBJECTREF throwable = NULL;
        GCPROTECT_BEGIN(throwable);
        throwable = GETTHROWABLE();
        CONTRACT_VIOLATION(ThrowsViolation);
        char szMethodName [MAX_CLASSNAME_LENGTH];
        CEEInfo::findNameOfToken(this, pMethod, szMethodName, COUNTOF (szMethodName));
        LOG((LF_IJW, LL_INFO10, "Failed to find Method: %s for Vtable Fixup\n", szMethodName));
        GCPROTECT_END();
#endif // _DEBUG
    }
    EX_END_CATCH(SwallowAllExceptions)

    RETURN pMDRet;
}

#ifdef DEBUGGING_SUPPORTED 

BOOL Module::NotifyDebuggerLoad(AppDomain *pDomain, int flags, BOOL attaching)
{
    WRAPPER_CONTRACT;

    if (IsIntrospectionOnly())
        return FALSE;

    if (!attaching && !pDomain->IsDebuggerAttached())
        return FALSE;

    // We don't notify the debugger about modules that don't contain any code.
    if (IsResource())
        return FALSE;

    BOOL result = FALSE;

    if (flags & ATTACH_MODULE_LOAD)
    {
        g_pDebugInterface->LoadModule(this,
                                      m_file->GetPath(),
                                      m_file->GetPath().GetCount(),
                                      GetAssembly(), pDomain,
                                      attaching);

        result = TRUE;
    }

    if (flags & ATTACH_CLASS_LOAD)
    {
        LookupMap *pMap;
        DWORD       dwMinIndex = 0;

        // Go through each linked block
        for (pMap = &m_TypeDefToMethodTableMap; pMap != NULL; pMap = pMap->pNext)
        {
            DWORD i;
            DWORD dwIterCount = pMap->dwMaxIndex - dwMinIndex;
            TADDR *pRealTableStart = &pMap->pTable[dwMinIndex];

            for (i = 0; i < dwIterCount; i++)
            {
                TADDR addr = pRealTableStart[i];
                if (addr != NULL && (addr & (TADDR)LookupMap::IS_MODULE_BACKPOINTER) == 0)
                {
                    MethodTable *pMT = (MethodTable *) addr;

                    if (pMT != NULL && pMT->IsRestored())
                    {
                        result = TypeHandle(pMT).NotifyDebuggerLoad(pDomain, attaching) || result;
                    }
                }
            }

            dwMinIndex = pMap->dwMaxIndex;
        }
    }

    return result;
}

void Module::NotifyDebuggerUnload(AppDomain *pDomain)
{
    WRAPPER_CONTRACT;

    if (IsIntrospectionOnly())
        return;

    if (!pDomain->IsDebuggerAttached())
        return;

    // We don't notify the debugger about modules that don't contain any code.
    if (IsResource())
        return;

    LookupMap  *pMap;
    DWORD       dwMinIndex = 0;

    // Go through each linked block
    for (pMap = &m_TypeDefToMethodTableMap; pMap != NULL;
         pMap = pMap->pNext)
    {
        DWORD i;
        DWORD dwIterCount = pMap->dwMaxIndex - dwMinIndex;
        TADDR *pRealTableStart = &pMap->pTable[dwMinIndex];

        for (i = 0; i < dwIterCount; i++)
        {
            TADDR result = pRealTableStart[i];
            if (result != NULL && (result & (TADDR)LookupMap::IS_MODULE_BACKPOINTER) == 0)
            {
                MethodTable *pMT = (MethodTable *) result;

                if (pMT != NULL && pMT->IsRestored())
                {
                    TypeHandle(pMT).NotifyDebuggerUnload(pDomain);
                }
            }
        }

        dwMinIndex = pMap->dwMaxIndex;
    }

    g_pDebugInterface->UnloadModule(this, pDomain);
}
#endif // DEBUGGING_SUPPORTED

#endif // !DACCESS_COMPILE


BOOL Module::IsPersistedObject(void *address)
{
    LEAF_CONTRACT;
    return FALSE;
}



#ifndef DACCESS_COMPILE 


#endif // !DACCESS_COMPILE

#ifndef DACCESS_COMPILE
void Module::SetBeingUnloaded()
{
    LEAF_CONTRACT;
    FastInterlockOr((ULONG*)&m_dwTransientFlags, IS_BEING_UNLOADED);
}
#endif



void Module::LogInstantiatedType(TypeHandle typeHnd, ULONG flagNum)
{
#ifndef DACCESS_COMPILE 

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(g_IBCLogger.InstrEnabled());
        PRECONDITION(!typeHnd.HasUnrestoredTypeKey());
        // We want to report the type only in its own loader module as a type's
        // MethodTable can only live in its own loader module.
        // We can relax this if we allow a (duplicate) MethodTable to live
        // in any module (which might be needed for ngen of generics)
        PRECONDITION(this == GetPreferredZapModuleForTypeHandle(typeHnd));
    }
    CONTRACTL_END;

    FAULT_NOT_FATAL();

    EX_TRY
    {
    }

    EX_CATCH
    {
        _ASSERTE(!"Exception in LogInstantiatedType");
    }
    EX_END_CATCH(SwallowAllExceptions);

#endif // !DACCESS_COMPILE

}


void Module::LogInstantiatedMethod( MethodDesc * md )
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION( md != NULL );
    }
    CONTRACT_END;

}


#ifndef DACCESS_COMPILE

// ===========================================================================
// ReflectionModule
// ===========================================================================

/* static */
ReflectionModule *ReflectionModule::Create(Assembly *pAssembly, PEFile *pFile, AllocMemTracker *pamTracker)
{
    CONTRACT(ReflectionModule *)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pAssembly));
        PRECONDITION(CheckPointer(pFile));
        PRECONDITION(pFile->IsDynamic());
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // Hoist CONTRACT into separate routine because of EX incompatibility

    mdFile token;
    if (pFile->IsAssembly())
        token = mdFileNil;
    else
        token = ((PEModule *)pFile)->GetToken();

    // Initial memory block for Modules must be zero-initialized (to make it harder
    // to introduce Destruct crashes arising from OOM's during initialization.)

    void* pMemory = pamTracker->Track(pAssembly->GetHighFrequencyHeap()->AllocMem(sizeof(ReflectionModule)));
    ReflectionModuleHolder pModule(new (pMemory) ReflectionModule(pAssembly, token, pFile));

    pModule->DoInit(pamTracker);

    pModule.SuppressRelease();
    
    RETURN pModule;
}


// Module initialization occurs in two phases: the constructor phase and the Initialize phase.
//
// The constructor phase initializes just enough so that Destruct() can be safely called.
// It cannot throw or fail.
//
ReflectionModule::ReflectionModule(Assembly *pAssembly, mdFile token, PEFile *pFile)
  : Module(pAssembly, token, pFile)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        FORBID_FAULT;
    }
    CONTRACTL_END

    m_pInMemoryWriter = NULL;
    m_sdataSection = NULL;
    m_ppISymUnmanagedWriter = NULL;
    m_pCreatingAssembly = NULL;
    m_pCeeFileGen = NULL;

}



// Module initialization occurs in two phases: the constructor phase and the Initialize phase.
//
// The Initialize() phase completes the initialization after the constructor has run.
// It can throw exceptions but whether it throws or succeeds, it must leave the Module
// in a state where Destruct() can be safely called.
//
void ReflectionModule::Initialize(AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END;

    Module::Initialize(pamTracker);

    IfFailThrow(CreateICeeGen(IID_ICeeGen, (void **)&m_pCeeFileGen));

    m_pInMemoryWriter = new RefClassWriter();

    IfFailThrow(m_pInMemoryWriter->Init(GetCeeGen(), GetEmitter()));

}

void ReflectionModule::Destruct()
{
    WRAPPER_CONTRACT;

    delete m_pInMemoryWriter;

    if (m_ppISymUnmanagedWriter)
    {
        Module::ReleaseIUnknown((IUnknown**)m_ppISymUnmanagedWriter);
        m_ppISymUnmanagedWriter = NULL;
    }

    if (m_pCeeFileGen)
        m_pCeeFileGen->Release();

    Module::Destruct();
}

void ReflectionModule::ReleaseILData()
{
    WRAPPER_CONTRACT;

    if (m_ppISymUnmanagedWriter)
    {
        Module::ReleaseIUnknown((IUnknown**)m_ppISymUnmanagedWriter);
        m_ppISymUnmanagedWriter = NULL;
    }

    Module::ReleaseILData();
}


#endif // !DACCESS_COMPILE

const void *ReflectionModule::GetIL(RVA il) // virtual
{
#ifndef DACCESS_COMPILE 
    WRAPPER_CONTRACT;
    BYTE* pByte = NULL;
    m_pCeeFileGen->GetMethodBuffer(il, &pByte);
    return pByte;
#else // DACCESS_COMPILE
    DacNotImpl();
    return NULL;
#endif // DACCESS_COMPILE
}

void *ReflectionModule::GetRvaField(RVA field) // virtual
{
#ifndef DACCESS_COMPILE 
    WRAPPER_CONTRACT;
    // This function should be call only if the target is a field or a field with RVA.
    BYTE* pByte = NULL;
    m_pCeeFileGen->ComputePointer(m_sdataSection, field, &pByte);
    return pByte;
#else // DACCESS_COMPILE
    DacNotImpl();
    return NULL;
#endif // DACCESS_COMPILE
}

#ifndef DACCESS_COMPILE 


VASigCookie *Module::GetVASigCookie(PCCOR_SIGNATURE pVASig)
{
    CONTRACT(VASigCookie*)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACT_END;

    VASigCookieBlock *pBlock;
    VASigCookie      *pCookie;

    pCookie = NULL;

    // First, see if we already enregistered this sig.
    // Note that we're outside the lock here, so be a bit careful with our logic
    for (pBlock = m_pVASigCookieBlock; pBlock != NULL; pBlock = pBlock->m_Next)
    {
        for (UINT i = 0; i < pBlock->m_numcookies; i++)
        {
            if (pVASig == pBlock->m_cookies[i].mdVASig)
            {
                pCookie = &(pBlock->m_cookies[i]);
                break;
            }
        }
    }

    if (!pCookie)
    {
        // If not, time to make a new one.

        // Compute the size of args first, outside of the lock.

        DWORD sizeOfArgs;

        SigTypeContext typeContext;

        sizeOfArgs = MetaSig::SizeOfActualFixedArgStack(this, 
                                                        pVASig,
                                                        SigParser::LengthOfSig(pVASig),
                                                        (*pVASig & IMAGE_CEE_CS_CALLCONV_HASTHIS)==0, 
                                                        &typeContext);

        // enable gc before taking lock
        {
            CrstPreempHolder ch(&m_Crst);

            // Note that we were possibly racing to create the cookie, and another thread
            // may have already created it.  We could put another check
            // here, but it's probably not worth the effort, so we'll just take an
            // occasional duplicate cookie instead.

            // Is the first block in the list full?
            if (m_pVASigCookieBlock && m_pVASigCookieBlock->m_numcookies
                < VASigCookieBlock::kVASigCookieBlockSize)
            {
                // Nope, reserve a new slot in the existing block.
                pCookie = &(m_pVASigCookieBlock->m_cookies[m_pVASigCookieBlock->m_numcookies]);
            }
            else
            {
                // Yes, create a new block.
                VASigCookieBlock *pNewBlock = new VASigCookieBlock();
                if (pNewBlock)
                {
                    pNewBlock->m_Next = m_pVASigCookieBlock;
                    pNewBlock->m_numcookies = 0;
                    m_pVASigCookieBlock = pNewBlock;
                    pCookie = &(pNewBlock->m_cookies[0]);
                }
            }

            // Now, fill in the new cookie (assuming we had enough memory to create one.)
            if (pCookie)
            {
                pCookie->mdVASig = pVASig;
                pCookie->pModule = this;
                pCookie->pNDirectMLStub = NULL;
                pCookie->sizeOfArgs = sizeOfArgs;
            }

            // Finally, now that it's safe for ansynchronous readers to see it,
            // update the count.
            m_pVASigCookieBlock->m_numcookies++;
        }
    }

    RETURN pCookie;
}


VOID VASigCookie::Destruct()
{
    WRAPPER_CONTRACT;
    if (pNDirectMLStub)
        pNDirectMLStub->DecRef();
}

// ===========================================================================
// LookupMap
// ===========================================================================

DWORD LookupMap::Find(TADDR pointer)
{
    LEAF_CONTRACT;

    LookupMap *map = this;
    DWORD index = 0;
    while (map != NULL)
    {
#ifndef DACCESS_COMPILE 
        TADDR *p = map->pTable + index;
        TADDR *pEnd = map->pTable + map->dwMaxIndex;
        while (p < pEnd)
        {
            if (*p == pointer)
                return (DWORD)(p - map->pTable);
            p++;
        }
#else // DACCESS_COMPILE
        while (index < map->dwMaxIndex)
        {
            if (map->pTable[index] == pointer)
                return index;
            index++;
        }
#endif // DACCESS_COMPILE
        index = map->dwMaxIndex;
        map = map->pNext;
    }

    return 0;
}

#endif // !DACCESS_COMPILE

#ifdef DACCESS_COMPILE 

void
LookupMap::EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                             DWORD minIndex,
                             bool enumThis)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    if (enumThis)
    {
        DacEnumHostDPtrMem(this);
    }
    if ((minIndex < dwMaxIndex) && pTable.IsValid())
    {
        DacEnumMemoryRegion(PTR_TO_TADDR(pTable),
                            (dwMaxIndex - minIndex) * sizeof(TADDR));
    }
}


/* static */
void
LookupMap::ListEnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                                 LookupMap* headMap,
                                 bool enumHead)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    DWORD minIndex = 0;
    while (headMap)
    {
        headMap->EnumMemoryRegions(flags, minIndex, enumHead);

        if (!headMap->pNext.IsValid())
        {
            break;
        }

        minIndex = headMap->dwMaxIndex;
        headMap = headMap->pNext;
        enumHead = false;
    }
}

#endif // DACCESS_COMPILE

// -------------------------------------------------------
// Stub manager for thunks.
//
// Note, the only reason we have this stub manager is so that we can recgonize UMEntryThunks for IsTransitionStub. If it
// turns out that having a full-blown stub manager for these things causes problems else where, then we can just attach
// a range list to the thunk heap and have IsTransitionStub check that after checking with the main stub manager.
// -------------------------------------------------------

SPTR_IMPL(ThunkHeapStubManager, ThunkHeapStubManager, g_pManager);

#ifndef DACCESS_COMPILE 

/* static */
void ThunkHeapStubManager::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    g_pManager = new ThunkHeapStubManager();
    StubManager::AddStubManager(g_pManager);
}

#endif // !DACCESS_COMPILE

BOOL ThunkHeapStubManager::CheckIsStub_Internal(TADDR stubStartAddress)
{
    WRAPPER_CONTRACT;
    // Its a stub if its in our heaps range.
    return GetRangeList()->IsInRange((const BYTE *) stubStartAddress);
}

BOOL ThunkHeapStubManager::DoTraceStub(const BYTE *stubStartAddress,
                                       TraceDestination *trace)
{
    LEAF_CONTRACT;
    // We never trace through these stubs when stepping through managed code. The only reason we have this stub manager
    // is so that IsTransitionStub can recgonize UMEntryThunks.
    return FALSE;
}



#ifdef _MSC_VER 
#ifndef _DEBUG 
// Optimization intended for Module::IsIntrospectionOnly and Module::EnsureActive only
#pragma optimize("t", on)
#endif
#endif

BOOL Module::IsIntrospectionOnly()
{
    WRAPPER_CONTRACT;
    return GetAssembly()->IsIntrospectionOnly();
}

VOID Module::EnsureActive()
{
    WRAPPER_CONTRACT;

    GetDomainFile()->EnsureActive();
}

#ifdef _MSC_VER 
#ifndef _DEBUG 
#pragma optimize("", on)
#endif
#endif

VOID Module::EnsureAllocated()
{
    WRAPPER_CONTRACT;

    GetDomainFile()->EnsureAllocated();
}

VOID Module::EnsureLibraryLoaded()
{
    WRAPPER_CONTRACT;

    GetDomainFile()->EnsureLibraryLoaded();
}



CHECK Module::CheckActivated()
{
    WRAPPER_CONTRACT;
    DomainFile *pDomainFile = FindDomainFile(GetAppDomain());
    CHECK(pDomainFile != NULL);
#ifndef DACCESS_COMPILE 
    PREFIX_ASSUME(pDomainFile != NULL);
    CHECK(pDomainFile->CheckActivated());
#endif
    CHECK_OK;
}

#ifdef DACCESS_COMPILE 

void
ModuleCtorInfo::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    // This class is contained so do not enumerate 'this'.
    DacEnumMemoryRegion(PTR_TO_TADDR(ppMT), numElements *
                        sizeof(TADDR));
    DacEnumMemoryRegion(PTR_TO_TADDR(cctorInfoHot), numElementsHot *
                        sizeof(ClassCtorInfoEntry));
    DacEnumMemoryRegion(PTR_TO_TADDR(cctorInfoCold),
                        (numElements - numElementsHot) *
                        sizeof(ClassCtorInfoEntry));
    DacEnumMemoryRegion(PTR_TO_TADDR(hotHashOffsets), numHotHashes *
                        sizeof(DWORD));
    DacEnumMemoryRegion(PTR_TO_TADDR(coldHashOffsets), numColdHashes *
                        sizeof(DWORD));
}

void Module::EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                               bool enumThis)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    if (enumThis)
    {
        DAC_ENUM_VTHIS();
        EMEM_OUT(("MEM: %p Module\n", PTR_HOST_TO_TADDR(this)));
    }

    if (m_file.IsValid())
    {
        m_file->EnumMemoryRegions(flags);
    }
    if (m_pAssembly.IsValid())
    {
        m_pAssembly->EnumMemoryRegions(flags);
    }
    if (m_pAvailableClasses.IsValid())
    {
        m_pAvailableClasses->EnumMemoryRegions(flags);
    }
    if (m_pAvailableParamTypes.IsValid())
    {
        m_pAvailableParamTypes->EnumMemoryRegions(flags);
    }
    if (m_pInstMethodHashTable.IsValid())
    {
        m_pInstMethodHashTable->EnumMemoryRegions(flags);
    }
    if (m_pAvailableClassesCaseIns.IsValid())
    {
        m_pAvailableClassesCaseIns->EnumMemoryRegions(flags);
    }
    if (m_pBinder.IsValid())
    {
        m_pBinder->EnumMemoryRegions(flags);
    }
    m_ModuleCtorInfo.EnumMemoryRegions(flags);

    // Save the LookupMap structures.
    LookupMap::ListEnumMemoryRegions(flags, &m_TypeDefToMethodTableMap, false);
    LookupMap::ListEnumMemoryRegions(flags, &m_TypeRefToMethodTableMap, false);
    LookupMap::ListEnumMemoryRegions(flags, &m_MethodDefToDescMap, false);
    LookupMap::ListEnumMemoryRegions(flags, &m_FieldDefToDescMap, false);
    LookupMap::ListEnumMemoryRegions(flags, &m_MemberRefToDescMap, false);
    LookupMap::ListEnumMemoryRegions(flags, &m_FileReferencesMap, false);
    LookupMap::ListEnumMemoryRegions(flags, &m_ManifestModuleReferencesMap, false);

    LookupMap::Iterator typeDefIter(&m_TypeDefToMethodTableMap);
    while (typeDefIter.Next())
    {
        if (typeDefIter.GetElement())
        {
            (PTR_MethodTable(typeDefIter.GetElement()))->
                EnumMemoryRegions(flags);
        }
    }

    LookupMap::Iterator typeRefIter(&m_TypeRefToMethodTableMap);
    while (typeRefIter.Next())
    {
        if (typeRefIter.GetElement())
        {
            (PTR_MethodTable(typeRefIter.GetElement()))->
                EnumMemoryRegions(flags);
        }
    }

    LookupMap::Iterator methodDefIter(&m_MethodDefToDescMap);
    while (methodDefIter.Next())
    {
        if (methodDefIter.GetElement())
        {
            (PTR_MethodDesc(methodDefIter.GetElement()))->
                EnumMemoryRegions(flags);
        }
    }

    LookupMap::Iterator fieldDefIter(&m_FieldDefToDescMap);
    while (fieldDefIter.Next())
    {
        if (fieldDefIter.GetElement())
        {
            (PTR_FieldDesc(fieldDefIter.GetElement()))->
                EnumMemoryRegions(flags);
        }
    }

    LookupMap::Iterator memberRefIter(&m_MemberRefToDescMap);
    while (memberRefIter.Next())
    {
        if (memberRefIter.GetElement())
        {
            TADDR memb = memberRefIter.GetElement();
            if (memb & IS_FIELD_MEMBER_REF)
            {
                (PTR_FieldDesc(memb & ~(TADDR)IS_FIELD_MEMBER_REF))->
                    EnumMemoryRegions(flags);
            }
            else
            {
                // Extra casts are to satisfy prefast.
                (PTR_MethodDesc((TADDR)(void*)memb))->
                    EnumMemoryRegions(flags);
            }
        }
    }

    LookupMap::Iterator fileRefIter(&m_FileReferencesMap);
    while (fileRefIter.Next())
    {
        if (fileRefIter.GetElement())
        {
            (PTR_Module(fileRefIter.GetElement()))->
                EnumMemoryRegions(flags, true);
        }
    }

    LookupMap::Iterator asmRefIter(&m_ManifestModuleReferencesMap);
    while (asmRefIter.Next())
    {
        if (asmRefIter.GetElement())
        {
            (PTR_Module(asmRefIter.GetElement())->GetAssembly())->
                EnumMemoryRegions(flags);
        }
    }

}

FieldDesc *Module::LookupFieldDef(mdFieldDef token)
{
    WRAPPER_CONTRACT;
    _ASSERTE(TypeFromToken(token) == mdtFieldDef);
    g_IBCLogger.LogRidMapAccess( MakePair( this, token ) );
    return PTR_FieldDesc(GetFromRidMap(&m_FieldDefToDescMap,
                                       RidFromToken(token)));
}

#endif // DACCESS_COMPILE


#ifndef DACCESS_COMPILE 

// Access to CerPrepInfo, the structure used to track CERs prepared at runtime (as opposed to ngen time). GetCerPrepInfo will
// return the structure associated with the given method desc if it exists or NULL otherwise. CreateCerPrepInfo will get the
// structure if it exists or allocate and return a new struct otherwise. Creation of CerPrepInfo structures is automatically
// synchronized by the CerCrst (lazily allocated as needed).
CerPrepInfo *Module::GetCerPrepInfo(MethodDesc *pMD)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMD));
    }
    CONTRACTL_END;

    if (m_pCerPrepInfo == NULL)
        return NULL;

    // Don't need a crst for read only access to the hash table.
    HashDatum sDatum;
    if (m_pCerPrepInfo->GetValue(pMD, &sDatum))
        return (CerPrepInfo*)sDatum;
    else
        return NULL;
}

CerPrepInfo *Module::CreateCerPrepInfo(MethodDesc *pMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pMD));
    }
    CONTRACTL_END;

    // Lazily allocate a Crst to serialize update access to the info structure.
    // Carefully synchronize to ensure we don't leak a Crst in race conditions.
    if (m_pCerCrst == NULL)
    {
        Crst *pCrst = new Crst("ConstrainedExecutionRegionHashTable", CrstCer, CRST_UNSAFE_COOPGC);
        if (InterlockedCompareExchangePointer((void**)&m_pCerCrst, pCrst, NULL) != NULL)
            delete pCrst;
    }

    CrstHolder sCrstHolder(m_pCerCrst);

    // Lazily allocate the info structure.
    if (m_pCerPrepInfo == NULL)
    {
        LockOwner sLock = {m_pCerCrst, IsOwnerOfCrst};
        NewHolder <EEPtrHashTable> tempCerPrepInfo (new EEPtrHashTable());
        if (!tempCerPrepInfo->Init(CER_DEFAULT_HASH_SIZE, &sLock))
            COMPlusThrowOM();
        m_pCerPrepInfo = tempCerPrepInfo.Extract ();
    }
    else
    {
        // Try getting an existing value first.
        HashDatum sDatum;
        if (m_pCerPrepInfo->GetValue(pMD, &sDatum))
            return (CerPrepInfo*)sDatum;
    }

    // We get here if there was no info structure or no existing method desc entry. Either way we now have an info structure and
    // need to create a new method desc entry.
    NewHolder<CerPrepInfo> pInfo(new CerPrepInfo());
    m_pCerPrepInfo->InsertValue(pMD, (HashDatum)pInfo);

    return pInfo.Extract();
}


BOOL Module::GetRVAOverrideForMethod(MethodDesc* pMD, DWORD* pdwOverride)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    if (!m_pRVAOverrides)
        return FALSE;

    // Don't need a crst for read only access to the hash table.
    HashDatum sDatum;
    if (!m_pRVAOverrides->GetValue(pMD, &sDatum))
        return FALSE;
    
    *pdwOverride = (DWORD)(SIZE_T)sDatum;
    return TRUE;
}

void  Module::SetRVAOverrideForMethod(MethodDesc* pMD, DWORD dwOverride)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    // Lazily allocate a Crst to serialize update access to the info structure.
    // Carefully synchronize to ensure we don't leak a Crst in race conditions.
    if (m_pRVAOverridesCrst == NULL)
    {
        Crst *pCrst = new Crst("RVAOverridesHashTable", CrstRVAOverrides, CrstFlags(CRST_UNSAFE_ANYMODE | CRST_DEBUGGER_THREAD));
        if (InterlockedCompareExchangePointer((void**)&m_pRVAOverridesCrst, pCrst, NULL) != NULL)
            delete pCrst;
    }

    CrstHolder sCrstHolder(m_pRVAOverridesCrst);

    // Lazily allocate the info structure.
    if (m_pRVAOverrides == NULL)
    {
        LockOwner sLock = {m_pRVAOverridesCrst, IsOwnerOfCrst};
        NewHolder <EEPtrHashTable> tempRVAOverrides(new EEPtrHashTable());
        if (!tempRVAOverrides->Init(59, &sLock))
            COMPlusThrowOM();
        MemoryBarrier();
        m_pRVAOverrides = tempRVAOverrides.Extract ();
    }
    else
    {
        if (m_pRVAOverrides->ReplaceValue(pMD, (HashDatum)(SIZE_T)dwOverride))
            return;
    }

    m_pRVAOverrides->InsertValue(pMD, (HashDatum)(SIZE_T)dwOverride);
}

#endif // !DACCESS_COMPILE



//-------------------------------------------------------------------------------
// Make best-case effort to obtain an image name for use in an error message.
//
// This routine must expect to be called before the this object is fully loaded.
// It can return an empty if the name isn't available or the object isn't initialized
// enough to get a name, but it mustn't crash.
//-------------------------------------------------------------------------------
void Module::GetPathForErrorMessages(SString & result)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
    }
    CONTRACTL_END

    PEFile *pFile = GetFile();
    if (pFile)
    {
        pFile->GetPathForErrorMessages(result);
    }
    else
    {
        result = L"";
    }
}

//-------------------------------------------------------------------------------

// Verify consistency of asmconstants.h

// Wrap all C_ASSERT's in asmconstants.h with a class definition.  Many of the
// fields referenced below are private, and this class is a friend of the
// enclosing type.  (A C_ASSERT isn't a compiler intrinsic, just a magic
// typedef that produces a compiler error when the condition is false.)

class CheckAsmOffsets
{
#define ASMCONSTANTS_C_ASSERT(cond) \
        typedef char UNIQUE_LABEL(__C_ASSERT__)[(cond) ? 1 : -1];
#include "asmconstants.h"
};

//-------------------------------------------------------------------------------
