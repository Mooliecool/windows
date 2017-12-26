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
#include "security.h"

AssemblySecurityDescriptor::AssemblySecurityDescriptor(AppDomain *pDomain, DomainAssembly *pAssembly) :
    SecurityDescriptor(pDomain, pAssembly, pAssembly->GetFile()),
    m_dwNumPassedDemands(0),
    m_pSignature(NULL),
    m_pSharedSecDesc(NULL),
    m_fAdditionalEvidence(FALSE),
    m_fIsSignatureLoaded(FALSE),
    m_fAssemblyRequestsComputed(FALSE)
{
    CONTRACTL 
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
    } CONTRACTL_END;

    m_hRequiredPermissionSet = NULL;
    m_hOptionalPermissionSet = NULL;
    m_hDeniedPermissionSet = NULL;
}

OBJECTREF AssemblySecurityDescriptor::GetRequestedPermissionSet(OBJECTREF *pOptionalPermissionSet,
                                                                OBJECTREF *pDeniedPermissionSet)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(m_pAppDomain == GetAppDomain());
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    OBJECTREF req = NULL;

    if (!IsAssemblyRequestsComputed())
    {
        ReleaseHolder<IMDInternalImport> pImport (m_pAssem->GetFile()->GetMDImportWithRef());
        // Try to load permission requests from assembly first.
        SecurityAttributes::LoadPermissionRequestsFromAssembly(pImport,
                                                           &req,
                                                           pOptionalPermissionSet,
                                                               pDeniedPermissionSet);

            SetRequestedPermissionSet(req, *pOptionalPermissionSet, *pDeniedPermissionSet);
    }
    else
    {
        *pOptionalPermissionSet = ObjectFromLazyHandle(m_hOptionalPermissionSet);
        *pDeniedPermissionSet = ObjectFromLazyHandle(m_hDeniedPermissionSet);
        req = ObjectFromLazyHandle(m_hRequiredPermissionSet);
    }

    return req;
}

//
// This method will return TRUE if this assembly is allowed to skip verification.
//

BOOL AssemblySecurityDescriptor::CanSkipVerification()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(IsResolved());
    } CONTRACTL_END;

/*
    // Assemblies loaded into the verification domain never get to skip verification.
    if (m_pAppDomain->IsVerificationDomain() && m_pAssem->IsIntrospectionOnly())
        return FALSE;
*/

    return CheckSpecialFlag(1 << SECURITY_SKIP_VER);
}

//
// This method will return TRUE if this assembly has assertion permission.
//

BOOL AssemblySecurityDescriptor::CanAssert()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(IsResolved());
    } CONTRACTL_END;

    return CheckSpecialFlag(1 << SECURITY_ASSERT);
}

//
// This method will return TRUE if this assembly has unrestricted UI permissions.
//

BOOL AssemblySecurityDescriptor::HasUnrestrictedUIPermission()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(IsResolved());
    } CONTRACTL_END;

    return CheckSpecialFlag(1 << UI_PERMISSION);
}

OBJECTREF AssemblySecurityDescriptor::GetSerializedEvidence()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(m_pAppDomain == GetAppDomain());
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    return SecurityDescriptor::GetSerializedEvidence( m_pAssem->GetFile() );
}

BOOL AssemblySecurityDescriptor::QuickIsFullyTrusted()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    if (IsSystem())
        return TRUE;

    if (!Security::DoesFullTrustMeanFullTrust())
        return FALSE;

    // quickly detect if we've got a request refused or a request optional.
    ReleaseHolder<IMDInternalImport> pImport(m_pAssem->GetFile()->GetMDImportWithRef());
    if (SecurityAttributes::RestrictiveRequestsInAssembly(pImport))
        return FALSE;

    // If the assembly is in the GAC then it gets FullTrust.
    if (m_pAssem->GetFile()->IsSourceGAC())
        return TRUE;

    ApplicationSecurityDescriptor* pAppSecDesc = m_pAppDomain->GetSecurityDescriptor();
    // Check if we need to call the HostSecurityManager.
    if (pAppSecDesc->CallHostSecurityManager())
        return FALSE;

    // If the AppDomain is homogeneous, we currently simply detect the FT case.
    if (pAppSecDesc->IsHomogeneous())
        return m_pAppDomain->GetSecurityDescriptor()->IsFullyTrusted();

    // Find out the quick cache value.
    static const SecurityConfig::QuickCacheEntryType fullTrustTable[] = {
        SecurityConfig::FullTrustZoneMyComputer,
        SecurityConfig::FullTrustZoneIntranet,
        SecurityConfig::FullTrustZoneInternet,
        SecurityConfig::FullTrustZoneTrusted,
        SecurityConfig::FullTrustZoneUntrusted
    };
    if (CheckQuickCache(SecurityConfig::FullTrustAll, fullTrustTable))
        return TRUE;

    // See if we've already determined that the assembly is FT
    // in another AppDomain, in case this is a shared assembly.
    SharedSecurityDescriptor* pSharedSecDesc = GetSharedSecDesc();
    if (pSharedSecDesc && pSharedSecDesc->IsResolved() && pSharedSecDesc->IsFullyTrusted())
        return TRUE;

    return FALSE;
}

// Gather all raw materials to construct evidence and punt them up to the managed
// assembly, which constructs the actual Evidence object and returns it (as well
// as caching it).
OBJECTREF AssemblySecurityDescriptor::GetEvidence()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(m_pAppDomain == GetAppDomain());
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    struct _gc
    {
        OBJECTREF objAsm;
        OBJECTREF objAdditionalEvidence;
        OBJECTREF objEvidence;
    } gc;
    ZeroMemory( &gc, sizeof( gc ) );

    gc.objAsm = m_pAssem->GetExposedAssemblyObject();
    gc.objAdditionalEvidence = ObjectFromLazyHandle( m_hAdditionalEvidence );
   
    GCPROTECT_BEGIN( gc );

    gc.objEvidence = SecurityDescriptor::GetEvidenceForPEFile( m_pPEFile, gc.objAsm, gc.objAdditionalEvidence );

    GCPROTECT_END();

    return gc.objEvidence;
}

void AssemblySecurityDescriptor::Resolve()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(m_pAssem != NULL);
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    // Always resolve the assembly security descriptor in the new AppDomain
    if (!IsResolved())
        ResolveWorker();

    // Update the info in the shared security descriptor
    SharedSecurityDescriptor* pSharedSecDesc = GetSharedSecDesc();
    if (pSharedSecDesc)
        pSharedSecDesc->Resolve(this);
}

void AssemblySecurityDescriptor::ResolveWorker()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    if (ShouldSkipPolicyResolution()) {
        SetGrantedPermissionSet(NULL, NULL, 0xFFFFFFFF);
        return;
    }

    struct _gc {
        OBJECTREF reqdPset;         // Required Requested Permissions
        OBJECTREF optPset;          // Optional Requested Permissions
        OBJECTREF denyPset;         // Denied Permissions
        OBJECTREF evidence;         // Object containing evidence
        OBJECTREF granted;          // Policy based Granted Permission
        OBJECTREF grantdenied;      // Policy based explicitly Denied Permissions
    } gc;
    ZeroMemory(&gc, sizeof(gc));
   
    
    ENTER_DOMAIN_PTR_PREDICATED(m_pAppDomain,ADV_RUNNINGIN,!IsSystem())
    {
        GCPROTECT_BEGIN(gc);

        //
        // GAC assemblies with no RequestRefuse get FullTrust
        // Also AppDomains with an AppTrust that are fully trusted are
        // homogeneous and so every assembly that does not have a RequestRefuse
        // will also get FullTrust.
        //

        int dwSpecialFlags;
        if (QuickIsFullyTrusted()) {
            Security::GetPermissionInstance(&gc.granted, SECURITY_FULL_TRUST);
            dwSpecialFlags = 0xFFFFFFFF;
        }
        else {
            gc.reqdPset = GetRequestedPermissionSet(&gc.optPset, &gc.denyPset);
            // We need to gather the evidence and call managed code.
            if (IsEvidenceComputed())
                gc.evidence = ObjectFromLazyHandle(m_hAdditionalEvidence);
            else
                gc.evidence = GetEvidence();
            gc.granted = SecurityPolicy::ResolvePolicy(gc.evidence, gc.reqdPset, gc.optPset, gc.denyPset, &gc.grantdenied, &dwSpecialFlags, TRUE);
        }

        SetGrantedPermissionSet(gc.granted, gc.denyPset, dwSpecialFlags);
        // Only fully trusted assemblies are allowed to be loaded when 
        // the AppDomain is in the initialization phase.
        if (m_pAppDomain->GetSecurityDescriptor()->IsInitializationInProgress() && !IsFullyTrusted())
            COMPlusThrow(kApplicationException, L"Policy_CannotLoadSemiTrustAssembliesDuringInit");

        GCPROTECT_END();
    }
    END_DOMAIN_TRANSITION;
}

DWORD AssemblySecurityDescriptor::GetZone()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    if (IsZoneComputed())
        return m_dwZone;

    StackSString    codebase;
    DWORD       dwZone = (DWORD) NoZone;
    BYTE        rbUniqueID[MAX_SIZE_SECURITY_ID];
    DWORD       cbUniqueID = sizeof(rbUniqueID);

    m_pAssem->GetSecurityIdentity(codebase, &dwZone, rbUniqueID, &cbUniqueID);
    SetZone(dwZone);

    return dwZone;
}

Assembly* AssemblySecurityDescriptor::GetAssembly()
{
    return m_pAssem->GetAssembly();
}

BOOL AssemblySecurityDescriptor::ShouldSkipPolicyResolution()
{
    WRAPPER_CONTRACT;
    Assembly* pAssembly = GetAssembly();
    return pAssembly && pAssembly->ShouldSkipPolicyResolution();
}

HRESULT AssemblySecurityDescriptor::LoadSignature(COR_TRUST **ppSignature)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    if (IsSignatureLoaded()) {
        if (ppSignature)
            *ppSignature = m_pSignature;
        return S_OK;
    }

    m_pSignature = m_pAssem->GetFile()->GetSecuritySignature();

    SetSignatureLoaded();

    if (ppSignature)
        *ppSignature = m_pSignature;

    return S_OK;
}

void AssemblySecurityDescriptor::AddToSharedDescriptor(SharedSecurityDescriptor *pSharedSecDesc)
{
    LEAF_CONTRACT;
    m_pSharedSecDesc = pSharedSecDesc;
}

SharedSecurityDescriptor::SharedSecurityDescriptor(Assembly *pAssembly) :
    m_pAssembly(pAssembly),
    m_fResolved(FALSE),
    m_fFullyTrusted(FALSE),
    m_fCanCallUnmanagedCode(FALSE),
    m_fCanAssert(FALSE)
{
    LEAF_CONTRACT;
}

void SharedSecurityDescriptor::Resolve(AssemblySecurityDescriptor *pSecDesc)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(pSecDesc->IsResolved());
    } CONTRACTL_END;

    if (m_fResolved) {
        _ASSERTE(!!m_fFullyTrusted == !!pSecDesc->IsFullyTrusted());
        _ASSERTE(!!m_fCanCallUnmanagedCode == !!pSecDesc->CanCallUnmanagedCode());
        _ASSERTE(!!m_fCanAssert == !!pSecDesc->CanAssert());
        return;
    }

    m_fFullyTrusted = pSecDesc->IsFullyTrusted();
    m_fCanCallUnmanagedCode = pSecDesc->CanCallUnmanagedCode();
    m_fCanAssert = pSecDesc->CanAssert();
    m_fResolved = TRUE;
}

BOOL SharedSecurityDescriptor::IsFullyTrusted()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(IsResolved());
    } CONTRACTL_END;

    return m_fFullyTrusted;
}

BOOL SharedSecurityDescriptor::CanCallUnmanagedCode()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(IsResolved());
    } CONTRACTL_END;

    return m_fCanCallUnmanagedCode;
}

BOOL SharedSecurityDescriptor::CanAssert()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(IsResolved());
    } CONTRACTL_END;

    return m_fCanAssert;
}
