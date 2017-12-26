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
** Header: AssemblySpec.cpp
**
** Purpose: Implements Assembly binding class
**
** Date:  May 5, 2000
**
===========================================================*/

#include "common.h"

#include <stdlib.h>

#include "assemblyspec.hpp"
#include "security.h"
#include "eeconfig.h"
#include "strongname.h"
#include "assemblysink.h"
#include "assemblyfilehash.h"
#include "mdaassistantsptr.h"
#include "eventtrace.h"

#ifdef _DEBUG
// This debug-only wrapper for LookupAssembly is solely for the use of postconditions and
// assertions. The problem is that the real LookupAssembly can throw an OOM
// simply because it can't allocate scratch space. For the sake of asserting,
// we can treat those as successful lookups.  
BOOL UnsafeVerifyLookupAssembly(AssemblySpecBindingCache *pCache, AssemblySpec *pSpec, DomainAssembly *pComparator)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FORBID_FAULT;

    BOOL result = FALSE;

    EX_TRY
    {
        SCAN_IGNORE_FAULT; // Won't go away: This wrapper exists precisely to turn an OOM here into something our postconditions can deal with.
        result = (pComparator == pCache->LookupAssembly(pSpec));
    }
    EX_CATCH
    {
        Exception *ex = GET_EXCEPTION();

        result = ex->IsTransient();
    }
    EX_END_CATCH(SwallowAllExceptions)

    return result;

}
#endif

#ifdef _DEBUG
// This debug-only wrapper for LookupFile is solely for the use of postconditions and
// assertions. The problem is that the real LookupFile can throw an OOM
// simply because it can't allocate scratch space. For the sake of asserting,
// we can treat those as successful lookups.  
BOOL UnsafeVerifyLookupFile(AssemblySpecBindingCache *pCache, AssemblySpec *pSpec, PEAssembly *pComparator)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FORBID_FAULT;

    BOOL result = FALSE;

    EX_TRY
    {
        SCAN_IGNORE_FAULT; // Won't go away: This wrapper exists precisely to turn an OOM here into something our postconditions can deal with.
        result = pCache->LookupFile(pSpec)->Equals(pComparator);
    }
    EX_CATCH
    {
        Exception *ex = GET_EXCEPTION();

        result = ex->IsTransient();
    }
    EX_END_CATCH(SwallowAllExceptions)

    return result;

}

#endif

#ifdef _DEBUG

// This debug-only wrapper for Contains is solely for the use of postconditions and
// assertions. The problem is that the real Contains can throw an OOM
// simply because it can't allocate scratch space. For the sake of asserting,
// we can treat those as successful lookups.  
BOOL UnsafeContains(AssemblySpecBindingCache *pCache, AssemblySpec *pSpec)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_FORBID_FAULT;

    BOOL result = FALSE;

    EX_TRY
    {
        SCAN_IGNORE_FAULT; // Won't go away: This wrapper exists precisely to turn an OOM here into something our postconditions can deal with.
        result = pCache->Contains(pSpec);
    }
    EX_CATCH
    {
        Exception *ex = GET_EXCEPTION();

        result = ex->IsTransient();
    }
    EX_END_CATCH(SwallowAllExceptions)

    return result;

}
#endif



AssemblySpecHash::~AssemblySpecHash()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    PtrHashMap::PtrIterator i = m_map.begin();
    while (!i.end())
    {
        AssemblySpec *s = (AssemblySpec*) i.GetValue();
        if (m_pHeap != NULL)
            s->~AssemblySpec();            
        else
            delete s;

        ++i;
    }
}

// Check assembly name for invalid characters
// Return value:
//      TRUE: If no invalid characters were found, or if the assembly name isn't set
//      FALSE: If invalid characters were found
// This is needed to prevent security loopholes with ':', '/' and '\' in the assembly name
BOOL AssemblySpec::IsValidAssemblyName()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    if (m_pAssemblyName)
    {
        SString ssAssemblyName(SString::Utf8, m_pAssemblyName);
        for (SString::Iterator i = ssAssemblyName.Begin(); i[0] != L'\0'; i++) {
            switch (i[0]) {
                case L':':
                case L'\\':
                case L'/':
                    return FALSE;

                default:
                    break;
            }
        }
    }
    return TRUE;
}

BOOL AssemblySpec::InitializeSpec(mdToken kAssemblyToken,
                                  IMDInternalImport *pImport,
                                  PEAssembly *pStaticParent, /*=NULL*/
                                  BOOL fIntrospectionOnly, /*=FALSE*/
                                  BOOL fThrow /*=TRUE*/)
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        if (fThrow) {
            GC_TRIGGERS;
            THROWS;
        }
        else {
            GC_NOTRIGGER;
            NOTHROW;
        }
        MODE_ANY;
        PRECONDITION(pImport->IsValidToken(kAssemblyToken));
        PRECONDITION(TypeFromToken(kAssemblyToken) == mdtAssembly
                     || TypeFromToken(kAssemblyToken) == mdtAssemblyRef);
        PRECONDITION(pStaticParent == NULL || !(pStaticParent->IsIntrospectionOnly() && !fIntrospectionOnly));
    }
    CONTRACT_END;

    // We also did this check as a precondition as we should have prevented this structurally - but just in case, make sure retail stops us from proceeding further.
    if (pStaticParent != NULL && pStaticParent->IsIntrospectionOnly() && !fIntrospectionOnly)
    {
        if (fThrow)
            COMPlusThrow(kTypeLoadException, IDS_EE_CODEEXECUTION_IN_INTROSPECTIVE_ASSEMBLY);
        RETURN FALSE;
    }

    // Normalize this boolean as it tends to be used for comparisons
    m_fIntrospectionOnly = !!fIntrospectionOnly;

    if (TypeFromToken(kAssemblyToken) == mdtAssembly) {

        pImport->GetAssemblyProps(kAssemblyToken,
                                  (const void **) &m_pbPublicKeyOrToken,
                                  &m_cbPublicKeyOrToken,
                                  NULL, 
                                  &m_pAssemblyName,
                                  &m_context,
                                  &m_dwFlags);
        
        if (m_cbPublicKeyOrToken != 0)
            m_dwFlags |= afPublicKey;
    }
    else
        pImport->GetAssemblyRefProps(kAssemblyToken,
                                     (const void**) &m_pbPublicKeyOrToken,
                                     &m_cbPublicKeyOrToken,
                                     &m_pAssemblyName,
                                     &m_context,
                                     NULL,
                                     NULL,
                                     &m_dwFlags);

    // For static binds, we cannot reference a strongly named assembly from a weakly named one.
    // (Note that this constraint doesn't apply to dynamic binds which is why this check is
    // not farther down the stack.)
    if (pStaticParent != NULL) {
        if (pStaticParent->IsStrongNamed() && !IsStrongNamed()) {
            if (fThrow)
                EEFileLoadException::Throw(this, FUSION_E_PRIVATE_ASM_DISALLOWED);
            RETURN FALSE;
        }

        {
            CONTRACT_VIOLATION(GCViolation);

            SetParentAssembly(pStaticParent);
        }
    }

    // data might go away if pImport is for a native image
    HRESULT hr = CloneFields(ALL_OWNED);
    if (FAILED(hr)) {
        if (fThrow)
            COMPlusThrowHR(hr);
        RETURN FALSE;
    }

    RETURN TRUE;
}

void AssemblySpec::SetParentAssembly(PEAssembly *pAssembly)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        GC_TRIGGERS;
        NOTHROW;
        MODE_ANY;
        PRECONDITION(CheckPointer(pAssembly));
    }
    CONTRACTL_END;

    GCX_PREEMP(); // fusion takes a Crst on AddRef
    m_CodeInfo.SetParentAssembly(pAssembly->GetFusionAssembly());
}


void AssemblySpec::InitializeSpec(IAssemblyName *pName,
                                  PEAssembly *pStaticParent /*=NULL*/,
                                  BOOL fIntrospectionOnly /*=FALSE*/)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        GC_TRIGGERS;
        THROWS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Normalize this boolean as it tends to be used for comparisons
    m_fIntrospectionOnly = !!fIntrospectionOnly;
    IfFailThrow(Init(pName));

    // For static binds, we cannot reference a strongly named assembly from a weakly named one.
    // (Note that this constraint doesn't apply to dynamic binds which is why this check is
    // not farther down the stack.)

    if (pStaticParent != NULL) {
        if (pStaticParent->IsStrongNamed() && !IsStrongNamed())
            EEFileLoadException::Throw(this, FUSION_E_PRIVATE_ASM_DISALLOWED);

        SetParentAssembly(pStaticParent);
    }
}


void AssemblySpec::InitializeSpec(PEAssembly *pFile)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pFile));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    ReleaseHolder<IMDInternalImport> pImport(pFile->GetMDImportWithRef());
    mdAssembly a;
    IfFailThrow(pImport->GetAssemblyFromScope(&a));
    InitializeSpec(a, pImport, NULL, pFile->IsIntrospectionOnly());
}

// This uses thread storage to allocate space. Please use Checkpoint and release it.
HRESULT AssemblySpec::InitializeSpec(StackingAllocator* alloc, ASSEMBLYNAMEREF* pName, 
                                  BOOL fParsed /*=FALSE*/, BOOL fIntrospectionOnly /*=FALSE*/)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(alloc));
        PRECONDITION(CheckPointer(pName));
        PRECONDITION(IsProtectedByGCFrame(pName));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Simple name
    if ((*pName)->GetSimpleName() != NULL) {
        WCHAR* pString;
        int    iString;
        RefInterpretGetStringValuesDangerousForGC((STRINGREF) (*pName)->GetSimpleName(), &pString, &iString);
        DWORD lgth = WszWideCharToMultiByte(CP_UTF8, 0, pString, iString, NULL, 0, NULL, NULL);
        if (lgth + 1 < lgth)
            ThrowHR(E_INVALIDARG);
        LPSTR lpName = (LPSTR) alloc->Alloc(lgth + 1);
        WszWideCharToMultiByte(CP_UTF8, 0, pString, iString,
                               lpName, lgth+1, NULL, NULL);
        lpName[lgth] = '\0';
        m_pAssemblyName = lpName;
    }

    if (fParsed) {
        HRESULT hr = ParseName();
        // Sometimes Fusion flags invalid characters in the name, sometimes it doesn't
        // depending on where the invalid characters are
        // We want to Raise the assembly resolve event on all invalid characters
        // but calling ParseName before checking for invalid characters gives Fusion a chance to
        // parse the rest of the name (to get a public key token, etc.)
        if ((hr == FUSION_E_INVALID_NAME) || (!IsValidAssemblyName())) {
            // This is the only case where we do not throw on an error
            // We don't want to throw so as to give the caller a chance to call RaiseAssemblyResolveEvent
            // The only caller that cares is System.Reflection.Assembly.InternalLoad which calls us through
            // AssemblyNameNative::Init
            return FUSION_E_INVALID_NAME;
        }
        else
            IfFailThrow(hr);
    }
    else {
        // Flags
        m_dwFlags = (*pName)->GetFlags();
    
        // Version
        VERSIONREF version = (VERSIONREF) (*pName)->GetVersion();
        if(version == NULL) {
            m_context.usMajorVersion = (USHORT)-1;
            m_context.usMinorVersion = (USHORT)-1;
            m_context.usBuildNumber = (USHORT)-1;
            m_context.usRevisionNumber = (USHORT)-1;
        }
        else {
            m_context.usMajorVersion = version->GetMajor();
            m_context.usMinorVersion = version->GetMinor();
            m_context.usBuildNumber = version->GetBuild();
            m_context.usRevisionNumber = version->GetRevision();
        }

        m_context.szLocale = 0;

        if ((*pName)->GetCultureInfo() != NULL) 
        {
            struct _gc {
                OBJECTREF   cultureinfo;
                STRINGREF   pString;
            } gc;

            gc.cultureinfo = (*pName)->GetCultureInfo();
            gc.pString = NULL;
            
            GCPROTECT_BEGIN(gc);

            MethodDescCallSite getName(METHOD__CULTURE_INFO__GET_NAME, &gc.cultureinfo);
            
            ARG_SLOT args[] = {
                ObjToArgSlot(gc.cultureinfo)
            };
            gc.pString = getName.Call_RetSTRINGREF(args);
            if (gc.pString != NULL) {
                WCHAR* pString;
                int    iString;
                RefInterpretGetStringValuesDangerousForGC(gc.pString, &pString, &iString);
                DWORD lgth = WszWideCharToMultiByte(CP_UTF8, 0, pString, iString, NULL, 0, NULL, NULL);
                LPSTR lpLocale = (LPSTR) alloc->Alloc(lgth + 1);
                WszWideCharToMultiByte(CP_UTF8, 0, pString, iString,
                                       lpLocale, lgth+1, NULL, NULL);
                lpLocale[lgth] = '\0';
                m_context.szLocale = lpLocale;
            }
            GCPROTECT_END();
        }

        // Strong name

        // Note that we prefer to take a public key token if present,
        // even if flags indicate a full public key
        if ((*pName)->GetPublicKeyToken() != NULL) {
            m_dwFlags &= ~afPublicKey;
            PBYTE  pArray = NULL;
            pArray = (*pName)->GetPublicKeyToken()->GetDirectPointerToNonObjectElements();
            m_cbPublicKeyOrToken = (*pName)->GetPublicKeyToken()->GetNumComponents();
            m_pbPublicKeyOrToken = (PBYTE) alloc->Alloc(m_cbPublicKeyOrToken);
            memcpy(m_pbPublicKeyOrToken, pArray, m_cbPublicKeyOrToken);
        }
        else if ((*pName)->GetPublicKey() != NULL) {
            m_dwFlags |= afPublicKey;
            PBYTE  pArray = NULL;
            pArray = (*pName)->GetPublicKey()->GetDirectPointerToNonObjectElements();
            m_cbPublicKeyOrToken = (*pName)->GetPublicKey()->GetNumComponents();
            m_pbPublicKeyOrToken = (PBYTE) alloc->Alloc(m_cbPublicKeyOrToken);
            memcpy(m_pbPublicKeyOrToken, pArray, m_cbPublicKeyOrToken);
        }
    }

    // Hash for control 
    if ((*pName)->GetHashForControl() != NULL)
        SetHashForControl((*pName)->GetHashForControl()->GetDataPtr(), 
                          (*pName)->GetHashForControl()->GetNumComponents(), 
                          (*pName)->GetHashAlgorithmForControl());

    // Normalize this boolean as it tends to be used for comparisons
    m_fIntrospectionOnly = !!fIntrospectionOnly;

    return S_OK;
}

// This uses thread storage to allocate space. Please use Checkpoint and release it.
void AssemblySpec::SetCodeBase(StackingAllocator* alloc, STRINGREF *pCodeBase)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pCodeBase));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Codebase

    if (pCodeBase != NULL && *pCodeBase != NULL) {
        WCHAR* pString;
        int    iString;
        RefInterpretGetStringValuesDangerousForGC(*pCodeBase, &pString, &iString);
        m_CodeInfo.m_dwCodeBase = (DWORD) iString+1;
        m_CodeInfo.m_pszCodeBase = (LPWSTR) alloc->Alloc(m_CodeInfo.m_dwCodeBase * sizeof(WCHAR));
        memcpy((void*)m_CodeInfo.m_pszCodeBase, pString, m_CodeInfo.m_dwCodeBase*sizeof(WCHAR));
    }
}

/* static */
void AssemblySpec::DemandFileIOPermission(PEAssembly *pFile)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pFile));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // should have already checked permission if the codebase is set
    if (!m_CodeInfo.m_pszCodeBase) { 

        if (pFile->IsBindingCodeBase()) {
            if (pFile->IsSourceDownloadCache()) {
                StackSString check;
                pFile->GetCodeBase(check);

                DemandFileIOPermission(check, FALSE, FILE_WEBPERM);
            }
            else
                DemandFileIOPermission(pFile->GetPath(), TRUE, FILE_READANDPATHDISC);
        }
    }
}


STDAPI RuntimeCheckLocationAccess(LPCWSTR wszLocation)
{

    if (GetThread()==NULL)
        return S_FALSE;

    CONTRACTL
    {
        NOTHROW;
        MODE_ANY;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(wszLocation));
    }
    CONTRACTL_END;
    OVERRIDE_LOAD_LEVEL_LIMIT(FILE_ACTIVE);
    HRESULT hr=S_OK;
    DWORD dwDemand = 0;

    if (SString::_wcsnicmp(wszLocation, L"file", 4))
        dwDemand = AssemblySpec::FILE_WEBPERM;
    else
        dwDemand = AssemblySpec::FILE_READANDPATHDISC;

    EX_TRY
    {
        AssemblySpec::DemandFileIOPermission(wszLocation,
                                             FALSE,
                                             dwDemand);
    }
    EX_CATCH_HRESULT(hr);
    return hr;

}
    

/* static */
void AssemblySpec::DemandFileIOPermission(LPCWSTR wszCodeBase,
                                          BOOL fHavePath,
                                          DWORD dwDemandFlag)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(wszCodeBase));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    GCX_COOP();
        
    MethodDescCallSite demandPermission(METHOD__ASSEMBLY__DEMAND_PERMISSION);

    STRINGREF codeBase = NULL;
    GCPROTECT_BEGIN(codeBase);
            
    codeBase = COMString::NewString(wszCodeBase);
    ARG_SLOT args[3] = 
    {
        ObjToArgSlot(codeBase),
        fHavePath,
        dwDemandFlag
    };
    demandPermission.Call(args);
    GCPROTECT_END();
}

BOOL AssemblySpec::FindAssemblyFile(AppDomain* pAppDomain, BOOL fThrowOnFileNotFound,
                                    IAssembly** ppIAssembly, IHostAssembly **ppIHostAssembly, IAssembly** ppNativeFusionAssembly,
                                    IFusionBindLog** ppFusionLog, StackCrawlMark *pCallerStackMark /* = NULL */)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pAppDomain));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    GCX_PREEMP();

    MEMORY_REPORT_ASSEMBLY_SCOPE(this);

    IApplicationContext *pFusionContext = pAppDomain->GetFusionContext();

    AssemblySink* pSink = pAppDomain->AllocateAssemblySink(this);
    SafeComHolder<IAssemblyBindSink> sinkholder(pSink);

    if (!GetCodeBase()->m_pszCodeBase)
        pSink->RequireCodebaseSecurityCheck();

    BOOL fIsWellKnown = FALSE;
    HRESULT hr = S_OK;

    IfFailGo(FusionBind::LoadAssembly(pFusionContext,
                                    pSink,
                                    ppIAssembly,
                                    ppIHostAssembly,
                                    ppNativeFusionAssembly,
                                    IsIntrospectionOnly()));

    // Host should have already done appropriate permission demand
    if (!(*ppIHostAssembly)) {
        DWORD dwLocation;
        IfFailGo((*ppIAssembly)->GetAssemblyLocation(&dwLocation));

        fIsWellKnown = (dwLocation == ASMLOC_UNKNOWN);

        // check if it was cached, where a codebase had originally loaded it
        if (pSink->DoCodebaseSecurityCheck() &&
            (dwLocation & ASMLOC_CODEBASE_HINT)) {
            if ((dwLocation & ASMLOC_LOCATION_MASK) == ASMLOC_DOWNLOAD_CACHE) {
                StackSString codeBase;
                SafeComHolder<IAssemblyName> pNameDef;
                
                IfFailGo((*ppIAssembly)->GetAssemblyNameDef(&pNameDef));

                GetAssemblyNameStringProperty(pNameDef, ASM_NAME_CODEBASE_URL, codeBase);

                DemandFileIOPermission(codeBase, FALSE, FILE_WEBPERM);
            }
            else if ((dwLocation & ASMLOC_LOCATION_MASK) != ASMLOC_GAC) {
                StackSString path;
                GetAssemblyManifestModulePath((*ppIAssembly), path);
                
                DemandFileIOPermission(path, TRUE, FILE_READANDPATHDISC);
            }
        }

        // Verify control hash
        if (m_HashForControl.GetSize() > 0) {
            StackSString path;
            
            GetAssemblyManifestModulePath((*ppIAssembly), path);
            
            AssemblyFileHash fileHash(AssemblyFileHash::HASH_CONTENT_FULL);
            IfFailGo(fileHash.SetFileName(path));
            IfFailGo(fileHash.CalculateHash(m_dwHashAlg));
            
            if (!m_HashForControl.Equals(fileHash.GetHash(), fileHash.GetHashSize()))
                IfFailGo(FUSION_E_REF_DEF_MISMATCH);
        }
    }


    *ppFusionLog = pSink->m_pFusionLog;
    if (*ppFusionLog)
        (*ppFusionLog)->AddRef();
    return fIsWellKnown;

 ErrExit:
    {
        GCX_COOP();

         if (fThrowOnFileNotFound || (!Assembly::FileNotFound(hr)))
            EEFileLoadException::Throw(this, pSink->m_pFusionLog, hr);
    }

    return FALSE;
}

void AssemblySpec::MatchRetargetedPublicKeys(Assembly *pAssembly)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pAssembly));
    }
    CONTRACTL_END;

    if (IsAfRetargetable(m_dwFlags)) {
        // Manually apply fusion policy to obtain retargeted public key
        SafeComHolder<IAssemblyName> pRequestedAssemblyName(NULL);
        SafeComHolder<IAssemblyName> pPostPolicyAssemblyName(NULL);
        IfFailThrow(CreateFusionName(&pRequestedAssemblyName));
        HRESULT hr = PreBindAssembly(GetAppDomain()->GetFusionContext(),
                                     pRequestedAssemblyName,
                                     NULL, // pAsmParent
                                     &pPostPolicyAssemblyName,
                                     NULL  // pvReserved
                                     );
        if (SUCCEEDED(hr)
            || (FAILED(hr) && (hr == FUSION_E_REF_DEF_MISMATCH))) {
            IAssemblyName *pResultAssemblyName = pAssembly->GetFusionAssemblyName();
            if (pResultAssemblyName
                && pPostPolicyAssemblyName
                && pResultAssemblyName->IsEqual(pPostPolicyAssemblyName, ASM_CMPF_PUBLIC_KEY_TOKEN) == S_OK)
                return;
        }
    }
    ThrowHR(FUSION_E_REF_DEF_MISMATCH);
}

// Check if the supplied assembly's public key matches up with the one in the Spec, if any
// Throws an appropriate exception in case of a mismatch
void AssemblySpec::MatchPublicKeys(Assembly *pAssembly)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Check that the public keys are the same as in the AR.
    if (m_cbPublicKeyOrToken) {

        const void *pbPublicKey;
        DWORD cbPublicKey;
        pbPublicKey = pAssembly->GetPublicKey(&cbPublicKey);
        if (cbPublicKey == 0)
            ThrowHR(FUSION_E_PRIVATE_ASM_DISALLOWED);

        if (m_dwFlags & afPublicKey) {
            if ((m_cbPublicKeyOrToken != cbPublicKey) ||
                memcmp(m_pbPublicKeyOrToken, pbPublicKey, m_cbPublicKeyOrToken))
                return MatchRetargetedPublicKeys(pAssembly);
        }

        // Ref has a token
        else {
            BYTE *pbStrongNameToken;
            DWORD cbStrongNameToken;

            if (!StrongNameTokenFromPublicKey((BYTE*) pbPublicKey,
                                              cbPublicKey,
                                              &pbStrongNameToken,
                                              &cbStrongNameToken))
                ThrowHR(StrongNameErrorInfo());
                
            if ((m_cbPublicKeyOrToken != cbStrongNameToken) ||
                memcmp(m_pbPublicKeyOrToken,
                       pbStrongNameToken,
                       cbStrongNameToken)) {
                StrongNameFreeBuffer(pbStrongNameToken);
                return MatchRetargetedPublicKeys(pAssembly);
            }

            StrongNameFreeBuffer(pbStrongNameToken);
        }
    }
}
    
PEAssembly *AssemblySpec::ResolveAssemblyFile(AppDomain *pDomain, BOOL fPreBind)
{
    CONTRACT(PEAssembly *)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    // No assembly resolve on codebase binds
    if (m_pAssemblyName == NULL)
        RETURN NULL;

    Assembly *pAssembly = pDomain->RaiseAssemblyResolveEvent(this, IsIntrospectionOnly(), fPreBind);

    if (pAssembly != NULL) {
        PEAssembly *pFile = pAssembly->GetManifestFile();
        pFile->AddRef();

        RETURN pFile;
    }

    RETURN NULL;
}


Assembly *AssemblySpec::LoadAssembly(FileLoadLevel targetLevel, OBJECTREF* pEvidence, OBJECTREF* pExtraEvidence, BOOL fDelayPolicyResolution, BOOL fThrowOnFileNotFound, BOOL fRaisePrebindEvents, StackCrawlMark *pCallerStackMark)
{
    WRAPPER_CONTRACT;
 
    DomainAssembly * pDomainAssembly = LoadDomainAssembly(targetLevel, pEvidence, pExtraEvidence, fDelayPolicyResolution, fThrowOnFileNotFound, fRaisePrebindEvents, pCallerStackMark);
    if (pDomainAssembly == NULL) {
        _ASSERTE(!fThrowOnFileNotFound);
        return NULL;
    }
    return pDomainAssembly->GetAssembly();
}

DomainAssembly *LoadDomainAssemblyHelper(AssemblySpec *pSpec, AppDomain *pDomain,
                                         PEAssembly *pFile, FileLoadLevel targetLevel, OBJECTREF* pEvidence,
                                         OBJECTREF *pExtraEvidence, BOOL fDelayPolicyResolution)
{
    STATIC_CONTRACT_THROWS;

    DomainAssembly* pRetVal = NULL;
    EX_TRY
    {
        pRetVal = pDomain->LoadDomainAssembly(pSpec, pFile, targetLevel, pEvidence, pExtraEvidence, fDelayPolicyResolution);
    }
    EX_HOOK
    {
        Exception* pEx=GET_EXCEPTION();
        if (!pEx->IsTransient())
        {
            if (!EEFileLoadException::CheckType(pEx))
            {
                StackSString name;
                pSpec->GetFileOrDisplayName(0, name);
                pEx=new EEFileLoadException(name, pEx->GetHR(), NULL, pEx);
                pDomain->AddExceptionToCache(pSpec, pEx);
                PAL_CPP_THROW(Exception *, pEx);
            }
            else
                pDomain->AddExceptionToCache(pSpec, pEx);
        }
    }
    EX_END_HOOK;

    return pRetVal;
}

DomainAssembly *AssemblySpec::LoadDomainAssembly(FileLoadLevel targetLevel,
                                                 OBJECTREF* pEvidence,
                                                 OBJECTREF* pExtraEvidence,
                                                 BOOL fDelayPolicyResolution,
                                                 BOOL fThrowOnFileNotFound,
                                                 BOOL fRaisePrebindEvents,
                                                 StackCrawlMark *pCallerStackMark)
{
    CONTRACT(DomainAssembly *)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION((!fThrowOnFileNotFound && CheckPointer(RETVAL, NULL_OK))
                      || CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    MEMORY_REPORT_ASSEMBLY_SCOPE(this);

    ETWTraceStartup trace(ETW_TYPE_STARTUP_LOADERCATCHALL);

    AppDomain* pDomain = GetAppDomain();

    DomainAssembly *pAssembly = pDomain->FindCachedAssembly(this);
    if (pAssembly) {
        pDomain->LoadDomainFile(pAssembly, targetLevel);
        RETURN pAssembly;
    }

    if (IsIntrospectionOnly() && GetCodeBase()->m_pszCodeBase == NULL)
    {
        IAssemblyName *ptmp = NULL;
        HRESULT hr = CreateFusionName(&ptmp);
        if (FAILED(hr))
        {
            COMPlusThrowHR(hr);
        }
        SafeComHolder<IAssemblyName> pIAssemblyName(ptmp);

        AppDomain::AssemblyIterator i = pDomain->IterateAssembliesEx( (AssemblyIterationFlags)(kIncludeLoaded | kIncludeIntrospection) );
        while (i.Next())
        {
            DomainAssembly *pCachedDomainAssembly = i.GetDomainAssembly();
            IAssemblyName *pCachedAssemblyName = pCachedDomainAssembly->GetAssembly()->GetFusionAssemblyName(); 
            if (S_OK == (pCachedAssemblyName->IsEqual(pIAssemblyName, ASM_CMPF_IL_ALL)))
            {
                RETURN pCachedDomainAssembly;
                break;
            }
        }
    }

    PEAssemblyHolder pFile(pDomain->BindAssemblySpec(this, fThrowOnFileNotFound, fRaisePrebindEvents, pCallerStackMark));
    if (pFile == NULL)
        RETURN NULL;
    pAssembly = LoadDomainAssemblyHelper(this, pDomain, pFile, targetLevel, pEvidence, pExtraEvidence, fDelayPolicyResolution);


    RETURN pAssembly;
}

/* static */
Assembly *AssemblySpec::LoadAssembly(LPCSTR pSimpleName, 
                                     AssemblyMetaDataInternal* pContext,
                                     PBYTE pbPublicKeyOrToken,
                                     DWORD cbPublicKeyOrToken,
                                     DWORD dwFlags)
{
    CONTRACT(Assembly *)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pSimpleName));
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    AssemblySpec spec;
    IfFailThrow(spec.Init(pSimpleName, pContext,
                          pbPublicKeyOrToken, cbPublicKeyOrToken, dwFlags));
    
    RETURN spec.LoadAssembly(FILE_LOADED);
}

/* static */
Assembly *AssemblySpec::LoadAssembly(LPCWSTR pFilePath)
{
    CONTRACT(Assembly *)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pFilePath));
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    AssemblySpec spec;
    spec.SetCodeBase(pFilePath, (DWORD) wcslen(pFilePath)+1);
    RETURN spec.LoadAssembly(FILE_LOADED);
}


#define ENGLISH_LOCALE                                                      \
          (MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL), SORT_DEFAULT))


BOOL AssemblySpec::IsMscorlib()
{
    CONTRACTL
    {
        THROWS;
        INSTANCE_CHECK;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_pAssemblyName == NULL) 
    {
        LPCWSTR file = FusionBind::GetCodeBase()->m_pszCodeBase;
        if (file) 
        {
            StackSString path(file);
            PEAssembly::UrlToPath(path);
            return SystemDomain::System()->IsBaseLibrary(path);
        }
        return FALSE;
    }

    _ASSERTE(strlen(g_psBaseLibraryName) == 8);

    size_t iNameLen = strlen(m_pAssemblyName);
    return ( (iNameLen >= 8) &&
             ( (!stricmpUTF8(m_pAssemblyName, g_psBaseLibrary)) ||
             ( (!SString::_strnicmp(m_pAssemblyName, g_psBaseLibraryName, 8)) &&
               ( (iNameLen == 8) || (m_pAssemblyName[8] == ',') ) ) ) );
}

// A satellite assembly for mscorlib is named "mscorlib.resources" and uses
// the same public key as mscorlib.  It does not necessarily have the same
// version, and the Culture will always be set to something like "jp-JP".
BOOL AssemblySpec::IsMscorlibSatellite()
{
    CONTRACTL
    {
        THROWS;
        INSTANCE_CHECK;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_pAssemblyName == NULL) 
    {
        LPCWSTR file = FusionBind::GetCodeBase()->m_pszCodeBase;
        if (file) 
        {
            StackSString path(file);
            PEAssembly::UrlToPath(path);
            return SystemDomain::System()->IsBaseLibrarySatellite(path);
        }
        return FALSE;
    }

    _ASSERTE(strlen(g_psBaseLibrarySatelliteAssemblyName) == 18);
    _ASSERTE(strlen(ECMA_PUBLICKEY_FULL_STR) == 32);

    size_t iNameLen = strlen(m_pAssemblyName);
    BOOL r = ( (m_cbPublicKeyOrToken == 16) && 
             (iNameLen >= 18) &&
             (!SString::_strnicmp(m_pAssemblyName, g_psBaseLibrarySatelliteAssemblyName, 18)) &&
             ( (iNameLen == 18) || (m_pAssemblyName[18] == ',') ) );

    // Now compare this public key vs. the ECMA public key
    for(int i=0; i<16 && r; i++) {
        char ch1 = ECMA_PUBLICKEY_FULL_STR[2*i];
        char ch2 = ECMA_PUBLICKEY_FULL_STR[2*i+1];
        unsigned int digit1 = (ch1 <= '9') ? ch1 - '0' : 10 + ((ch1 & 0x20) - 'a');
        unsigned int digit2 = (ch2 <= '9') ? ch2 - '0' : 10 + ((ch2 & 0x20) - 'a');
        BYTE b = (digit1 << 4) | digit2;
        r = (b == m_pbPublicKeyOrToken[i]);
    }
    return r;
}



AssemblySpecBindingCache::AssemblySpecBindingCache()
{
    LEAF_CONTRACT;
}

AssemblySpecBindingCache::~AssemblySpecBindingCache()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    Clear();
}

void AssemblySpecBindingCache::Clear()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    PtrHashMap::PtrIterator i = m_map.begin();
    while (!i.end())
    {
        AssemblyBinding *b = (AssemblyBinding*) i.GetValue();
        if (m_pHeap == NULL)
            delete b;
        else
            b->~AssemblyBinding();
    
        ++i;
    }
        
    m_map.Clear();
}

void AssemblySpecBindingCache::OnAppDomainUnload()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    PtrHashMap::PtrIterator i = m_map.begin();
    while (!i.end())
    {
        AssemblyBinding *b = (AssemblyBinding*) i.GetValue();
        b->OnAppDomainUnload();

        ++i;
    }
}

void AssemblySpecBindingCache::Init(CrstBase *pCrst, LoaderHeap *pHeap)
{
    WRAPPER_CONTRACT;

    LockOwner lock = {pCrst, IsOwnerOfCrst};
    m_map.Init(INITIAL_ASM_SPEC_HASH_SIZE, CompareSpecs, TRUE, &lock);
    m_pHeap = pHeap;
}

BOOL AssemblySpecBindingCache::Contains(AssemblySpec *pSpec)
{
    WRAPPER_CONTRACT;

    DWORD key = pSpec->Hash();

    AssemblyBinding *entry = (AssemblyBinding *) m_map.LookupValue(key, pSpec);

    return (entry != (AssemblyBinding *) INVALIDENTRY);
}

DomainAssembly *AssemblySpecBindingCache::LookupAssembly(AssemblySpec *pSpec,
                                                         BOOL fThrow /*=TRUE*/)
{
    CONTRACT(DomainAssembly *)
    {
        INSTANCE_CHECK;
        if (fThrow) {
            GC_TRIGGERS;
            THROWS;
        }
        else {
            GC_NOTRIGGER;
            NOTHROW;
        }
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    DWORD key = pSpec->Hash();

    AssemblyBinding *entry = (AssemblyBinding *) m_map.LookupValue(key, pSpec);

    if (entry == (AssemblyBinding *) INVALIDENTRY)
        RETURN NULL;
    else
    {
        if ((entry->GetAssembly() == NULL) && fThrow)
        {
            // May be either unloaded, or an exception occurred.
            entry->ThrowIfError();
        }

        RETURN entry->GetAssembly();
    }
}

PEAssembly *AssemblySpecBindingCache::LookupFile(AssemblySpec *pSpec)
{
    CONTRACT(PEAssembly *)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    DWORD key = pSpec->Hash();

    AssemblyBinding *entry = (AssemblyBinding *) m_map.LookupValue(key, pSpec);

    if (entry == (AssemblyBinding *) INVALIDENTRY)
        RETURN NULL;
    else
    {
        if (entry->GetFile() == NULL)
        {
            CONSISTENCY_CHECK(entry->IsError());
            entry->ThrowIfError();
        }

        RETURN entry->GetFile();
    }
}


class AssemblyBindingHolder
{
    public:
        AssemblyBindingHolder()
        {
            LEAF_CONTRACT;
            m_entry = NULL;
            m_pHeap = NULL;
        }

        AssemblySpecBindingCache::AssemblyBinding *CreateAssemblyBinding(LoaderHeap *pHeap)
        {
            CONTRACTL
            {
                THROWS;
                GC_TRIGGERS;
                INJECT_FAULT(COMPlusThrowOM(););
            }
            CONTRACTL_END

            m_pHeap = pHeap;
            if (pHeap)
            {
                m_entry = new (m_amTracker.Track(pHeap->AllocMem(sizeof(AssemblySpecBindingCache::AssemblyBinding)))) AssemblySpecBindingCache::AssemblyBinding;
            }
            else
            {
                m_entry = new AssemblySpecBindingCache::AssemblyBinding;
            }
            return m_entry;
        }

        ~AssemblyBindingHolder()
        {
            CONTRACTL
            {
                NOTHROW;
                GC_TRIGGERS;
                FORBID_FAULT;
            }
            CONTRACTL_END

            if (m_entry)
            {
                if (m_pHeap)
                {
                    // just call destructor - m_amTracker will delete the memory for m_entry itself.
                    m_entry->~AssemblyBinding();
                }
                else
                {
                    delete m_entry;
                }
            }
        }

        void SuppressRelease()
        {
            LEAF_CONTRACT;
            m_entry = NULL;
            m_pHeap = NULL;
            m_amTracker.SuppressRelease();
        }

        AllocMemTracker *GetPamTracker()
        {
            LEAF_CONTRACT;
            return &m_amTracker;
        }



    private:
        AssemblySpecBindingCache::AssemblyBinding *m_entry;
        LoaderHeap                                *m_pHeap;
        AllocMemTracker                            m_amTracker;
};

// NOTE ABOUT STATE OF CACHE ENTRIES:
// 
// A cache entry can be in one of 4 states:
// 1. Empty (no entry)
// 2. File (a PEAssembly has been bound, but not yet an Assembly)
// 3. Assembly (Both a PEAssembly & Assembly are available.)
// 4. Error (an error has occurred)
//
// The legal state transitions are:
// 1 -> any
// 2 -> 3
// 2 -> 4


BOOL AssemblySpecBindingCache::StoreAssembly(AssemblySpec *pSpec, DomainAssembly *pAssembly, BOOL clone)
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(UnsafeContains(this, pSpec));
        POSTCONDITION(UnsafeVerifyLookupAssembly(this, pSpec, pAssembly));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    DWORD key = pSpec->Hash();

    AssemblyBinding *entry = (AssemblyBinding *) m_map.LookupValue(key, pSpec);

    if (entry == (AssemblyBinding *) INVALIDENTRY)
    {
        AssemblyBindingHolder abHolder;
        entry = abHolder.CreateAssemblyBinding(m_pHeap);

        if (clone)
            entry->Init(pSpec,pAssembly->GetFile(),pAssembly,NULL,m_pHeap, abHolder.GetPamTracker());
        else
            entry->ShallowInit(pSpec,pAssembly->GetFile(),pAssembly,NULL);

        m_map.InsertValue(key, entry);

        abHolder.SuppressRelease();

        RETURN TRUE;
    }
    else
    {
        if (!entry->IsError())
        {
            if (entry->GetAssembly() != NULL)
            {
                // OK if this is a duplicate
                if (entry->GetAssembly() == pAssembly)
                    RETURN TRUE;
            }
            else
            {
                // OK if we have have a matching PEAssembly
                if (entry->GetFile() != NULL
                    && pAssembly->GetFile()->Equals(entry->GetFile()))
                {
                    entry->SetAssembly(pAssembly);
                    RETURN TRUE;
                }
            }
        }

        // Invalid cache transition (see above note about state transitions)
        RETURN FALSE;
    }
}

// Note that this routine may be called outside a lock, so may be racing with another thread. 
// Returns TRUE if add was successful - if FALSE is returned, caller should honor current
// cached value to ensure consistency.

BOOL AssemblySpecBindingCache::StoreFile(AssemblySpec *pSpec, PEAssembly *pFile, BOOL clone)
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION((!RETVAL) || (UnsafeContains(this, pSpec) && UnsafeVerifyLookupFile(this, pSpec, pFile)));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    DWORD key = pSpec->Hash();

    AssemblyBinding *entry = (AssemblyBinding *) m_map.LookupValue(key, pSpec);

    if (entry == (AssemblyBinding *) INVALIDENTRY)
    {
        AssemblyBindingHolder abHolder;
        entry = abHolder.CreateAssemblyBinding(m_pHeap);

        if (clone)
            entry->Init(pSpec,pFile,NULL,NULL,m_pHeap, abHolder.GetPamTracker());
        else
            entry->ShallowInit(pSpec,pFile,NULL,NULL);

        m_map.InsertValue(key, entry);
        abHolder.SuppressRelease();

        RETURN TRUE;
    }
    else
    {
        if (!entry->IsError())
        {
            // OK if this is a duplicate
            if (entry->GetFile() != NULL
                && pFile->Equals(entry->GetFile()))
                RETURN TRUE;
        }
        STRESS_LOG2(LF_CLASSLOADER,LL_INFO10,"Incompatible cached entry found (%08x) when adding PEFile %08x",entry,pFile);
        // Invalid cache transition (see above note about state transitions)
        RETURN FALSE;
    }
}

BOOL AssemblySpecBindingCache::StoreException(AssemblySpec *pSpec, Exception* pEx, BOOL clone)
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        DISABLED(POSTCONDITION(UnsafeContains(this, pSpec)));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    DWORD key = pSpec->Hash();

    AssemblyBinding *entry = (AssemblyBinding *) m_map.LookupValue(key, pSpec);

    if (entry == (AssemblyBinding *) INVALIDENTRY) {
        AssemblyBindingHolder abHolder;
        entry = abHolder.CreateAssemblyBinding(m_pHeap);

        if (clone)
            entry->Init(pSpec,NULL,NULL,pEx,m_pHeap, abHolder.GetPamTracker());
        else
            entry->ShallowInit(pSpec,NULL,NULL,pEx);


        m_map.InsertValue(key, entry);
        abHolder.SuppressRelease();

        RETURN TRUE;
    }
    else
    {
        // OK if this is a duplicate
        if (entry->IsError())
        {
            if (entry->GetHR() == pEx->GetHR())
                RETURN TRUE;
        }
        else
        {
            // OK to transition to error if we don't have an Assembly yet
            if (entry->GetAssembly() == NULL)
            {
                entry->InitException(pEx);
                RETURN TRUE;
            }
        }

        // Invalid cache transition (see above note about state transitions)
        RETURN FALSE;
    }
}
        
        

/* static */
BOOL DomainAssemblyCache::CompareBindingSpec(UPTR spec1, UPTR spec2)
{
    WRAPPER_CONTRACT;

    AssemblySpec* pSpec1 = (AssemblySpec*) (spec1 << 1);
    AssemblyEntry* pEntry2 = (AssemblyEntry*) spec2;

    if ((!pSpec1->CompareEx(&pEntry2->spec)) ||
        (pSpec1->IsIntrospectionOnly() != pEntry2->spec.IsIntrospectionOnly()))
        return FALSE;

    return TRUE;
}


DomainAssemblyCache::AssemblyEntry* DomainAssemblyCache::LookupEntry(AssemblySpec* pSpec)
{
    CONTRACT (DomainAssemblyCache::AssemblyEntry*)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END

    DWORD hashValue = pSpec->Hash();

    LPVOID pResult = m_Table.LookupValue(hashValue, pSpec);
    if(pResult == (LPVOID) INVALIDENTRY)
        RETURN NULL;
    else
        RETURN (AssemblyEntry*) pResult;
}

VOID DomainAssemblyCache::InsertEntry(AssemblySpec* pSpec, LPVOID pData1, LPVOID pData2/*=NULL*/)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    LPVOID ptr = LookupEntry(pSpec);
    if(ptr == NULL) {

        MEMORY_REPORT_CONTEXT_SCOPE("AppDomainBindingCache");
        
        BaseDomain::CacheLockHolder lh(m_pDomain);

        ptr = LookupEntry(pSpec);
        if(ptr == NULL) {
            AllocMemTracker amTracker;
            AllocMemTracker *pamTracker = &amTracker;

            AssemblyEntry* pEntry = (AssemblyEntry*) pamTracker->Track( m_pDomain->GetLowFrequencyHeap()->AllocMem(sizeof(AssemblyEntry)) );
            new (&pEntry->spec) AssemblySpec ();

            IfFailThrow(pEntry->spec.Init(pSpec, FALSE));
            pEntry->spec.CloneFieldsToLoaderHeap(AssemblySpec::ALL_OWNED, m_pDomain->GetLowFrequencyHeap(), pamTracker);
            pEntry->pData[0] = pData1;
            pEntry->pData[1] = pData2;
            DWORD hashValue = pEntry->Hash();
            m_Table.InsertValue(hashValue, pEntry);

            pamTracker->SuppressRelease();
        }
        // lh goes out of scope here
    }
#ifdef _DEBUG
    else {
        _ASSERTE(pData1 == ((AssemblyEntry*) ptr)->pData[0]);
        _ASSERTE(pData2 == ((AssemblyEntry*) ptr)->pData[1]);
    }
#endif

}
