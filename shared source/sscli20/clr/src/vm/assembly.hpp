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
** Header:  Assembly.hpp
**
** Purpose: Implements assembly (loader domain) architecture
**
** Date:  Oct 26, 1998
**
===========================================================*/
#ifndef _ASSEMBLY_H
#define _ASSEMBLY_H

#include "ceeload.h"
#include "exceptmacros.h"
#include "clsload.hpp"
#include "fusion.h"
#include "eehash.h"
#include "listlock.h"
#include "iceefilegen.h"
#include "fusionbind.h"
#include "cordbpriv.h"
#include "assemblyspec.hpp"

// A helper macro for the assembly's module hash (m_pAllowedFiles).
#define UTF8_TO_LOWER_CASE(str, qb)                                                             \
{                                                                                               \
    WRAPPER_CONTRACT;                                                                           \
    GCX_COOP();                                                                                 \
    INT32 allocBytes = InternalCasingHelper::InvariantToLower(NULL, 0, str);                    \
    qb.AllocThrows(allocBytes);                                                                 \
    InternalCasingHelper::InvariantToLower((LPUTF8) qb.Ptr(), allocBytes, str);                 \
}


class BaseDomain;
class AppDomain;
class DomainAssembly;
class DomainModule;
class SystemDomain;
class ClassLoader;
class ComDynamicWrite;
class AssemblySink;
class AssemblyNative;
class AssemblySpec;
class COMHash;
class SharedSecurityDescriptor;
class Pending;
class AllocMemTracker;

// Bits in m_dwDynamicAssemblyAccess (see System.Reflection.Emit.AssemblyBuilderAccess.cs)
#define ASSEMBLY_ACCESS_RUN     0x01
#define ASSEMBLY_ACCESS_SAVE    0x02
#define ASSEMBLY_ACCESS_REFLECTION_ONLY    0x04


struct AssemblySharingContext
{
    IAssemblyBindingClosure* pContext;
    volatile LONG                    shareCount;
};

struct CreateDynamicAssemblyArgsGC
{
    APPDOMAINREF    refThis;
    OBJECTREF       refusedPset;
    OBJECTREF       optionalPset;
    OBJECTREF       requiredPset;
    OBJECTREF       identity;
    ASSEMBLYNAMEREF assemblyName;
};

struct CreateDynamicAssemblyArgs : CreateDynamicAssemblyArgsGC
{
    INT32           access;
    StackCrawlMark* stackMark;
};


class Assembly
{
    friend class BaseDomain;
    friend class SystemDomain;
    friend class ClassLoader;
    friend class AssemblyNative;
    friend class AssemblySpec;
    friend class COMHash;
    friend class NDirect;
    friend class AssemblyNameNative;
    friend class ClrDataAccess;

public:
    Assembly(BaseDomain *pDomain, PEAssembly *pFile, DebuggerAssemblyControlFlags debuggerFlags);
    void Init(AllocMemTracker *pamTracker);

    void StartUnload();
    void Terminate( BOOL signalProfiler = TRUE );

    static Assembly *Create(BaseDomain *pDomain, PEAssembly *pFile, DebuggerAssemblyControlFlags debuggerFlags, AllocMemTracker *pamTracker);

    BOOL IsSystem() { WRAPPER_CONTRACT; return m_pManifestFile->IsSystem(); }

    static Assembly *CreateDynamic(BaseDomain *pDomain, CreateDynamicAssemblyArgs *args);
    ReflectionModule *CreateDynamicModule(LPCWSTR name);

    MethodDesc *GetEntryPoint();

    //****************************************************************************************
    //
    // Additional init tasks for Modules. This should probably be part of Module::Initialize()
    // but there's at least one call to ReflectionModule::Create that is *not* followed by a
    // PrepareModule call.
    void PrepareModuleForAssembly(Module* module, AllocMemTracker *pamTracker);

    // This is the final step of publishing a Module into an Assembly. This step cannot fail.
    void PublishModuleIntoAssembly(Module *module);

#ifndef DACCESS_COMPILE
    void SetIsTenured()
    {
        WRAPPER_CONTRACT;
        m_pManifest->SetIsTenured();
    }

    // CAUTION: This should only be used as backout code if an assembly is unsuccessfully
    //          added to the shared domain assembly map.
    void UnsetIsTenured()
    {
        WRAPPER_CONTRACT;
        m_pManifest->UnsetIsTenured();
    }
#endif // DACCESS_COMPILE

    //****************************************************************************************
    //
    // Returns the class loader associated with the assembly.
    ClassLoader* GetLoader()
    {
        LEAF_CONTRACT;
        return m_pClassLoader;
    }

    // ------------------------------------------------------------
    // Modules
    // ------------------------------------------------------------

    class ModuleIterator
    {
        Module* m_pManifest;
        DWORD m_i;

      public:
#ifdef DACCESS_COMPILE
        ModuleIterator()
        {
            // This ctor is only useful for creating placeholder
            // iterator that are later copied over.
            m_pManifest = NULL;
        }
#endif
        ModuleIterator(Assembly *pAssembly)
          : m_pManifest(pAssembly->GetManifestModule()),
            m_i((DWORD) -1)
        {
            WRAPPER_CONTRACT;
        }

        BOOL Next()
        {
            WRAPPER_CONTRACT;
            while (++m_i <= m_pManifest->GetFileMax())
            {
                if (GetModule() != NULL)
                    return TRUE;
            }
            return FALSE;
        }
        Module *GetModule()
        {
            WRAPPER_CONTRACT;
            return m_pManifest->LookupFile(TokenFromRid(m_i, mdtFile));
        }
    };

    ModuleIterator IterateModules()
    {
        WRAPPER_CONTRACT;
        return ModuleIterator(this);
    }

    //****************************************************************************************
    //
    // Find the module 
    Module* FindModule(PEFile *pFile, BOOL includeLoading = FALSE);

    // Finds loading modules as well
    DomainFile* FindIJWDomainFile(HMODULE hMod, const SString &path);
    //****************************************************************************************
    //
    // Get the domain the assembly lives in.
    BaseDomain* Parent()
    {
        LEAF_CONTRACT;
        return m_pDomain;
    }

    // Sets the assemblies domain.
    void SetParent(BaseDomain* pParent);

    //-----------------------------------------------------------------------------------------
    // If true, this assembly is loaded only for introspection. We can load modules, types, etc,
    // but no code execution or object instantiation is permitted.
    //-----------------------------------------------------------------------------------------
    BOOL IsIntrospectionOnly();


    //-----------------------------------------------------------------------------------------
    // If true,  strings only need to be interned at a per module basis, instead of at a
    // per appdomain basis, which is the default. Use the module accessor so you don't need
    // to touch the metadata in the ngen case
    //-----------------------------------------------------------------------------------------
    BOOL IsNoStringInterning();

    //-----------------------------------------------------------------------------------------
    // If true,  any non-CLSCompliant exceptions (i.e. ones which derive from something other
    // than System.Exception) are wrapped in a RuntimeWrappedException instance.  In other
    // words, they become compliant
    //-----------------------------------------------------------------------------------------
    BOOL IsRuntimeWrapExceptions();

    //-----------------------------------------------------------------------------------------
    // EnsureActive ensures that the assembly is properly prepped in the current app domain
    // for active uses like code execution, static field access, and instance allocation
    //-----------------------------------------------------------------------------------------
    VOID EnsureActive();

    //-----------------------------------------------------------------------------------------
    // CheckActivated is a check predicate which should be used in active use paths like code
    // execution, static field access, and instance allocation
    //-----------------------------------------------------------------------------------------
    CHECK CheckActivated();

    // Returns the parent domain if it is not the system area. Returns NULL if it is the
    // system domain
    BaseDomain* GetDomain();

    BOOL GetModuleZapFile(LPCWSTR name, SString &path);

    BOOL AllowUntrustedCaller();
    
#ifdef LOGGING
    LPCWSTR GetDebugName()
    {
        WRAPPER_CONTRACT;
        return GetManifestFile()->GetDebugName();
    }
#endif

    LPCUTF8 GetSimpleName()
    {
        WRAPPER_CONTRACT;
        return GetManifestFile()->GetSimpleName();
    }

    BOOL IsStrongNamed()
    {
        WRAPPER_CONTRACT;
        return GetManifestFile()->IsStrongNamed();
    }

    const void *GetPublicKey(DWORD *pcbPK)
    {
        WRAPPER_CONTRACT;
        return GetManifestFile()->GetPublicKey(pcbPK);
    }

    ULONG GetHashAlgId()
    {
        WRAPPER_CONTRACT;
        return GetManifestFile()->GetHashAlgId();
    }

    void GetVersion(USHORT *pMajor, USHORT *pMinor, USHORT *pBuild, USHORT *pRevision)
    {
        WRAPPER_CONTRACT;
        return GetManifestFile()->GetVersion(pMajor, pMinor, pBuild, pRevision);
    }

    LPCUTF8 GetLocale()
    {
        WRAPPER_CONTRACT;
        return GetManifestFile()->GetLocale();
    }

    DWORD GetFlags()
    {
        WRAPPER_CONTRACT;
        return GetManifestFile()->GetFlags();
    }


    // Level of strong name support (dynamic assemblies only).
    enum StrongNameLevel {
        SN_NONE = 0,
        SN_PUBLIC_KEY = 1,
        SN_FULL_KEYPAIR_IN_ARRAY = 2,
        SN_FULL_KEYPAIR_IN_CONTAINER = 3
    };

    StrongNameLevel GetStrongNameLevel()
    {
        LEAF_CONTRACT;
        return m_eStrongNameLevel;
    }

    void SetStrongNameLevel(StrongNameLevel eLevel)
    {
        LEAF_CONTRACT;
        m_eStrongNameLevel = eLevel;
    }

    // returns whether CAS policy needs to be resolved for this assembly
    // or whether it's safe to skip that step.
    BOOL ShouldSkipPolicyResolution ()
    {
        WRAPPER_CONTRACT;
        return IsSystem() || IsIntrospectionOnly() || (m_isDynamic && !(m_dwDynamicAssemblyAccess & ASSEMBLY_ACCESS_RUN));
    }

    // Cached assembly level reliability contract info, see ConstrainedExecutionRegion.cpp for details.
    inline DWORD GetReliabilityContract()
    {
        LEAF_CONTRACT;
        return m_dwReliabilityContract;
    }

    inline void SetReliabilityContract(DWORD dwValue)
    {
        LEAF_CONTRACT;
        m_dwReliabilityContract = dwValue;
    }

    LoaderHeap* GetLowFrequencyHeap();
    LoaderHeap* GetHighFrequencyHeap();
    LoaderHeap* GetStubHeap();

    Module* GetManifestModule()
    {
        LEAF_CONTRACT;
        return m_pManifest;
    }

    ReflectionModule* GetOnDiskManifestModule()
    {
        LEAF_CONTRACT;
        return m_pOnDiskManifest;
    }

    BOOL NeedsToHideManifestForEmit()
    {
        return m_needsToHideManifestForEmit;
    }

    PEAssembly* GetManifestFile()
    {
        LEAF_CONTRACT;
        return m_pManifestFile;
    }

    IMDInternalImport* GetManifestImport()
    {
        WRAPPER_CONTRACT;
        return m_pManifestFile->GetPersistentMDImport();
    }

#ifndef DACCESS_COMPILE
    IMetaDataImport* GetManifestImporter()
    {
        WRAPPER_CONTRACT;
        return m_pManifestFile->GetRWImporter();
    }

    IMetaDataAssemblyImport* GetManifestAssemblyImporter()
    {
        WRAPPER_CONTRACT;
        return m_pManifestFile->GetAssemblyImporter();
    }
#endif // DACCESS_COMPILE

    mdAssembly GetManifestToken()
    {
        LEAF_CONTRACT;

        return TokenFromRid(1, mdtAssembly);
    }

#ifndef DACCESS_COMPILE
    void GetDisplayName(SString &result, DWORD flags = 0)
    {
        WRAPPER_CONTRACT;

        return m_pManifestFile->GetDisplayName(result, flags);
    }
#endif // DACCESS_COMPILE

    void GetCodeBase(SString &result)
    {
        WRAPPER_CONTRACT;

        return m_pManifestFile->GetCodeBase(result);
    }

    // Initialize an AssemblySpec from an Assembly.
    void GetAssemblySpec(AssemblySpec *pSpec);

    OBJECTREF GetExposedObject();

    DebuggerAssemblyControlFlags GetDebuggerInfoBits(void)
    {
        LEAF_CONTRACT;

        return m_debuggerFlags;
    }

    void SetDebuggerInfoBits(DebuggerAssemblyControlFlags flags)
    {
        LEAF_CONTRACT;

        m_debuggerFlags = flags;
    }

    void SetCopiedPDBs()
    {
        LEAF_CONTRACT;

        m_debuggerFlags = (DebuggerAssemblyControlFlags) (m_debuggerFlags | DACF_PDBS_COPIED);
    }

    //****************************************************************************************
    //
    // Uses the given token to load a module or another assembly. Returns the module in
    // which the implementation resides.

    mdFile GetManifestFileToken(IMDInternalImport *pImport, mdFile kFile);
    mdFile GetManifestFileToken(LPCSTR name);

    Module *FindModuleByExportedType(mdExportedType mdType,
                                     Loader::LoadFlag loadFlag,
                                     mdTypeDef mdNested,
                                     mdTypeDef *pCL);

    static Module *FindModuleByTypeRef(Module *pModule, 
                                       mdTypeRef typeRef, 
                                       Loader::LoadFlag loadFlag,
                                       BOOL *pfNoResolutionScope);

    Module *FindModuleByName(LPCSTR moduleName);

    //****************************************************************************************
    //
    INT32 ExecuteMainMethod(PTRARRAYREF *stringArgs);

    //****************************************************************************************

    Assembly();
    ~Assembly();

    BOOL GetResource(LPCSTR szName, DWORD *cbResource,
                     PBYTE *pbInMemoryResource, Assembly **pAssemblyRef,
                     LPCSTR *szFileName, DWORD *dwLocation,
                     StackCrawlMark *pStackMark = NULL, BOOL fSkipSecurityCheck = FALSE,
                     BOOL fSkipRaiseResolveEvent = FALSE);

    //****************************************************************************************
#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    FORCEINLINE BOOL IsDynamic() { LEAF_CONTRACT; return m_isDynamic; }
    FORCEINLINE BOOL HasRunAccess() {LEAF_CONTRACT; return m_dwDynamicAssemblyAccess & ASSEMBLY_ACCESS_RUN; }

    DWORD GetNextModuleIndex() { LEAF_CONTRACT; return m_nextAvailableModuleIndex++; }

    void AddType(Module* pModule,
                 mdTypeDef cl);

    void PrepareSavingManifest(ReflectionModule *pAssemblyModule);
    mdFile AddFileList(LPWSTR wszFileName);
    void SetHashValue(mdFile tkFile, LPWSTR wszFullFileName);
    mdAssemblyRef AddAssemblyRef(Assembly *refedAssembly, IMetaDataAssemblyEmit *pAssemEmitter = NULL, BOOL fUsePublicKeyToken = TRUE);
    mdExportedType AddExportedType(LPWSTR wszExportedType, mdToken tkImpl, mdToken tkTypeDef, CorTypeAttr flags);
    void AddStandAloneResource(LPCWSTR wszName, LPWSTR wszDescription, LPWSTR wszMimeType, LPCWSTR wszFileName , LPCWSTR wszFullFileName, int iAttribute);
    void SaveManifestToDisk(LPWSTR wszFileName, int entrypoint, int fileKind, DWORD corhFlags, DWORD peFlags);
    void AddFileToInMemoryFileList(LPWSTR wszFileName, Module *pModule);
    void SavePermissionRequests(U1ARRAYREF orRequired, U1ARRAYREF orOptional, U1ARRAYREF orRefused);
    IMetaDataAssemblyEmit *GetOnDiskMDAssemblyEmitter();

    //****************************************************************************************

    DomainAssembly *GetDomainAssembly(AppDomain *pDomain = NULL);
    void SetDomainAssembly(DomainAssembly *pAssembly);

    // FindDomainAssembly will return NULL if the assembly is not in the given domain
    DomainAssembly *FindDomainAssembly(AppDomain *pDomain);

    AssemblySecurityDescriptor *GetSecurityDescriptor(AppDomain *pDomain = NULL);
    SharedSecurityDescriptor *GetSharedSecurityDescriptor() { LEAF_CONTRACT; return m_pSharedSecurityDesc; }

    // BOOL CanSkipVerification();
//    BOOL QuickCanSkipVerificationIfEasyCommit();


    // pDependencies should be allocated with new []
    void SetSharingProperties(IAssemblyBindingClosure* pContext);
    BOOL GetSharingProps(IAssemblyBindingClosure** pContext);    
    BOOL CanBeShared(DomainAssembly *pAsAssembly);
    void SetDomainNeutral() { LEAF_CONTRACT; m_fIsDomainNeutral = TRUE; }
    BOOL IsDomainNeutral() { LEAF_CONTRACT; return m_fIsDomainNeutral; }
    BOOL IsInstrumented();
    BOOL IsInstrumentedHelper();
    void IncrementShareCount();
    void DecrementShareCount();

    HRESULT AllocateStrongNameSignature(ICeeFileGen  *pCeeFileGen,
                                        HCEEFILE      ceeFile);
    HRESULT SignWithStrongName(LPWSTR wszFileName);

    IAssembly* GetFusionAssembly()
    {
        WRAPPER_CONTRACT;
        return m_pManifestFile->GetFusionAssembly();
    }

    IAssemblyName* GetFusionAssemblyName()
    {
        WRAPPER_CONTRACT;
        return m_pManifestFile->GetFusionAssemblyName();
    }

    IHostAssembly* GetIHostAssembly()
    {
        WRAPPER_CONTRACT;
        return m_pManifestFile->GetIHostAssembly();
    }



    //****************************************************************************************
    // Get the class init lock. The method is limited to friends because inappropriate use
    // will cause deadlocks in the system
    ListLock* GetClassInitLock();

#ifndef DACCESS_COMPILE

    void DECLSPEC_NORETURN ThrowTypeLoadException(LPCUTF8 pszFullName, UINT resIDWhy);

    void DECLSPEC_NORETURN ThrowTypeLoadException(LPCUTF8 pszNameSpace, LPCUTF8 pTypeName,
                                                  UINT resIDWhy);

    void DECLSPEC_NORETURN ThrowTypeLoadException(NameHandle *pName, UINT resIDWhy);

    void DECLSPEC_NORETURN ThrowTypeLoadException(IMDInternalImport *pInternalImport,
                                                  mdToken token,
                                                  UINT resIDWhy);

    void DECLSPEC_NORETURN ThrowTypeLoadException(IMDInternalImport *pInternalImport,
                                                  mdToken token,
                                                  LPCUTF8 pszFieldOrMethodName,
                                                  UINT resIDWhy);

    void DECLSPEC_NORETURN ThrowTypeLoadException(LPCUTF8 pszNameSpace,
                                                  LPCUTF8 pszTypeName,
                                                  LPCUTF8 pszMethodName,
                                                  UINT resIDWhy);

    void DECLSPEC_NORETURN ThrowBadImageException(LPCUTF8 pszNameSpace,
                                                  LPCUTF8 pszTypeName,
                                                  UINT resIDWhy);

    UINT64 GetHostAssemblyId() const
    {
        LEAF_CONTRACT;
        return m_HostAssemblyId;
    }

#endif // #ifndef DACCESS_COMPILE

    // Memory report support:

    static LPCSTR NameFromAssembly(AssemblySpec *pSpec) { return pSpec->GetName(); }
    static LPCSTR NameFromAssembly(PEAssembly *pFile) { return pFile->GetSimpleName(); }
    static LPCSTR NameFromAssembly(Assembly *pAssembly) { return pAssembly->GetSimpleName(); }
    static LPCSTR NameFromAssembly(Module *pModule) { return pModule->GetAssembly()->GetSimpleName(); }

#define MEMORY_REPORT_ASSEMBLY_SCOPE(d) \
    MEMORY_REPORT_ATTRIBUTION_SCOPE(2, "Assembly %s", Assembly::NameFromAssembly(d))

    //****************************************************************************************
    //

    PEModule *LoadModule(mdFile kFile, BOOL fLoadResource = TRUE);

    static BOOL FileNotFound(HRESULT hr);
    PEModule* RaiseModuleResolveEvent(LPCSTR szName, mdFile kFile);

    //****************************************************************************************
    // Is the given assembly a friend of this assembly?
    BOOL GrantsFriendAccessTo(Assembly *pAccessingAssembly);

protected:

    enum {
        FREE_KEY_PAIR = 4,
        FREE_KEY_CONTAINER = 8,
    };

    // Keep track of the vars that need to be freed.
    short int m_FreeFlag;

    // Hash of files in manifest by name to File token
    EEUtf8StringHashTable *m_pAllowedFiles;

private:

    //****************************************************************************************

    void CacheManifestExportedTypes(AllocMemTracker *pamTracker);
    void CacheManifestFiles();

    void CacheFriendAssemblyInfo();
   
    PTR_BaseDomain        m_pDomain;        // Parent Domain
    PTR_ClassLoader       m_pClassLoader;   // Single Loader



    PTR_MethodDesc        m_pEntryPoint;    // Method containing the entry point
    PTR_Module            m_pManifest;
    PTR_PEAssembly        m_pManifestFile;
    ReflectionModule*     m_pOnDiskManifest;  // This is the module containing the on disk manifest.
    BOOL                  m_fEmbeddedManifest;

    IAssemblyName         **m_pFriendAssemblyNames;

    // Strong name key info for reflection emit
    PBYTE                 m_pbStrongNameKeyPair;
    DWORD                 m_cbStrongNameKeyPair;
    LPWSTR                m_pwStrongNameKeyContainer;
    StrongNameLevel       m_eStrongNameLevel;

    BOOL                  m_isDynamic;
    BOOL                  m_needsToHideManifestForEmit;
    DWORD                 m_dwDynamicAssemblyAccess;
    DWORD                 m_nextAvailableModuleIndex;


    SharedSecurityDescriptor* m_pSharedSecurityDesc;    // Security descriptor (permission requests, signature etc)

    BOOL                   m_fIsDomainNeutral;
    AssemblySharingContext *m_pSharingProps;

    DebuggerAssemblyControlFlags m_debuggerFlags;

    BOOL                  m_fTerminated;

    BOOL                  m_fNoStringInterning;
    BOOL                  m_fCheckedForNoStringInterning;

    BOOL                  m_fRuntimeWrapExceptions;
    BOOL                  m_fCheckedForRuntimeWrapExceptions;

    UINT64                m_HostAssemblyId;

    DWORD                 m_dwReliabilityContract;
};

typedef Assembly::ModuleIterator ModuleIterator;

#endif
