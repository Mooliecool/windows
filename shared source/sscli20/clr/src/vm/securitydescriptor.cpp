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
#include "eventtrace.h"
#include "timeline.h"

///////////////////////////////////////////////////////////////////////////////
//
//  [SecurityDescriptor]
//  |
//  |
//  +----[PEFileSecurityDescriptor]
//
///////////////////////////////////////////////////////////////////////////////

BOOL SecurityDescriptor::CanCallUnmanagedCode ()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(IsResolved() || m_pAppDomain->GetSecurityDescriptor()->IsInitializationInProgress());
    } CONTRACTL_END;

    return CheckSpecialFlag(1 << SECURITY_UNMANAGED_CODE);
}

OBJECTREF SecurityDescriptor::GetGrantedPermissionSet(OBJECTREF* pRefusedPermissions)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsResolved() || m_pAppDomain->GetSecurityDescriptor()->IsInitializationInProgress());
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    if (pRefusedPermissions)
        *pRefusedPermissions = ObjectFromHandle(m_hGrantDeniedPermissionSet);
    return ObjectFromLazyHandle(m_hGrantedPermissionSet);
}

//
// Returns TRUE if the given zone has the given special permission.
//

BOOL SecurityDescriptor::CheckQuickCache(SecurityConfig::QuickCacheEntryType all, const SecurityConfig::QuickCacheEntryType* zoneTable)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    // If an additional evidence was provided, then perform the normal
    // policy resolution. This is true for all AppDomains and also for
    // assemblies loaded with a specific additional evidence. Note that
    // for the default AppDomain, the policy resolution code paths short
    // circuits the parsing of the security XML files by granting FullTrust
    // to the default AppDomain.

    if (m_hAdditionalEvidence != NULL)
        return FALSE;

    BOOL fMachine = SecurityConfig::GetQuickCacheEntry(SecurityConfig::MachinePolicyLevel, all);
    BOOL fUser = SecurityConfig::GetQuickCacheEntry(SecurityConfig::UserPolicyLevel, all);
    BOOL fEnterprise = SecurityConfig::GetQuickCacheEntry(SecurityConfig::EnterprisePolicyLevel, all);

    if (fMachine && fUser && fEnterprise)
        return TRUE;

    // If we can't match for all, try for our zone.
    DWORD dwZone = GetZone();
    if (dwZone == 0xFFFFFFFF)
        return FALSE;

    fMachine = SecurityConfig::GetQuickCacheEntry(SecurityConfig::MachinePolicyLevel, zoneTable[dwZone]);
    fUser = SecurityConfig::GetQuickCacheEntry(SecurityConfig::UserPolicyLevel, zoneTable[dwZone]);
    fEnterprise = SecurityConfig::GetQuickCacheEntry(SecurityConfig::EnterprisePolicyLevel, zoneTable[dwZone]);

    return (fMachine && fUser && fEnterprise);
}

//
// This method will return TRUE if this object is fully trusted.
//

BOOL SecurityDescriptor::IsFullyTrusted ()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(IsResolved() || m_pAppDomain->GetSecurityDescriptor()->IsInitializationInProgress());
    } CONTRACTL_END;

    return CheckSpecialFlag(1 << SECURITY_FULL_TRUST);
}

// Checks if the granted permission set has a security permission
// using stored Permission Object instances.
BOOL SecurityDescriptor::CheckSecurityPermission(int index)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(m_pAppDomain == GetAppDomain());
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    BOOL fRet = FALSE;

    // Check for a special permission (used for well known SecurityPermission
    // and ReflectionPermission flags commonly used inside of the VM).
    struct _gc {
        OBJECTREF granted;
        OBJECTREF denied;
        OBJECTREF perm;
    } gc;
    ZeroMemory(&gc, sizeof(_gc));

    GCPROTECT_BEGIN(gc);

    SecurityDeclarative::_GetSharedPermissionInstance(&gc.perm, index);
    gc.granted = GetGrantedPermissionSet(&gc.denied);

    // Denied permission set should not refuse the special demand
    if (gc.denied != NULL)
    {
        MethodDescCallSite permSetContains(METHOD__PERMISSION_SET__CONTAINS);
        ARG_SLOT arg[] = {
            ObjToArgSlot(gc.denied),
            ObjToArgSlot(gc.perm),
        };
        if (permSetContains.Call_RetBool(arg))
            goto Exit;
    }

    // Granted permission set should contain the special demand
    if (gc.granted != NULL)
    {
        MethodDescCallSite permSetContains(METHOD__PERMISSION_SET__CONTAINS);
        ARG_SLOT arg[] = {
            ObjToArgSlot(gc.granted),
            ObjToArgSlot(gc.perm),
        };
        if (permSetContains.Call_RetBool(arg))
            fRet = TRUE;
    }

Exit: ;
    GCPROTECT_END();

    return fRet;
}

OBJECTREF SecurityDescriptor::GetEvidenceForPEFile( PEFile* peFile, OBJECTREF& objAsm, OBJECTREF& objAdditionalEvidence )
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    StackSString ssCodebase;
    OBJECTREF   evidence = NULL;
    DWORD       dwZone = (DWORD) NoZone;
    BYTE        rbUniqueID[MAX_SIZE_SECURITY_ID];
    DWORD       cbUniqueID = sizeof(rbUniqueID);
    const void *pbSNPublicKey;
    DWORD       cbSNPublicKey;
    USHORT      major = 0, minor = 0, build = 0, revision = 0;
    COR_TRUST  *pSignature;

    struct _gc {
        OBJECTREF objAsm;
        STRINGREF strUrl;
        OBJECTREF objAuthCert;
        OBJECTREF objSNPublicKey;
        STRINGREF strSNName;
        OBJECTREF objSerializedEvidence;
        OBJECTREF objAdditionalEvidence;
    } gc;
    ZeroMemory(&gc, sizeof(_gc));

    gc.objAdditionalEvidence = objAdditionalEvidence;
    gc.objAsm = objAsm;

    GCPROTECT_BEGIN(gc);

#if CHECK_APP_DOMAIN_LEAKS
    if (g_pConfig->AppDomainLeaks()) {
        _ASSERTE( gc.objAsm == NULL || GetAppDomain() == gc.objAsm->GetAppDomain() );
        _ASSERTE( gc.objAdditionalEvidence == NULL || GetAppDomain() == gc.objAdditionalEvidence->GetAppDomain() );
    }
#endif

    peFile->GetSecurityIdentity( ssCodebase, &dwZone, rbUniqueID, &cbUniqueID );

    pSignature = peFile->GetSecuritySignature();

    pbSNPublicKey = peFile->GetPublicKey( &cbSNPublicKey );


    if (!ssCodebase.IsEmpty())
        gc.strUrl = COMString::NewString( ssCodebase );

    if (pSignature && pSignature->pbSigner && pSignature->cbSigner)
        SecurityAttributes::CopyEncodingToByteArray( pSignature->pbSigner,
                                                 pSignature->cbSigner,
                                                 &gc.objAuthCert );

    if (pbSNPublicKey != NULL && cbSNPublicKey != 0)
    {
        SecurityAttributes::CopyEncodingToByteArray( (BYTE*)pbSNPublicKey,
                                                 cbSNPublicKey,
                                                 &gc.objSNPublicKey );

        gc.strSNName = COMString::NewString( peFile->GetSimpleName() );
        peFile->GetVersion( &major, &minor, &build, &revision );
    }

    gc.objSerializedEvidence = SecurityDescriptor::GetSerializedEvidence( peFile );

    MethodDescCallSite  createSecurityIdentity(METHOD__ASSEMBLY__CREATE_SECURITY_IDENTITY);

    ARG_SLOT args[] = {
        ObjToArgSlot(gc.objAsm),
        ObjToArgSlot(gc.strUrl),
        (ARG_SLOT)dwZone,
        ObjToArgSlot(gc.objAuthCert),
        ObjToArgSlot(gc.objSNPublicKey),
        ObjToArgSlot(gc.strSNName),
        (ARG_SLOT)major,
        (ARG_SLOT)minor,
        (ARG_SLOT)build,
        (ARG_SLOT)revision,
        ObjToArgSlot(gc.objSerializedEvidence),
        ObjToArgSlot(gc.objAdditionalEvidence),
    };

    evidence = createSecurityIdentity.Call_RetOBJECTREF(args);

#if CHECK_APP_DOMAIN_LEAKS
    if (g_pConfig->AppDomainLeaks()) {
        _ASSERTE(evidence == NULL || GetAppDomain() == evidence->GetAppDomain());
    }
#endif

    GCPROTECT_END();

    return evidence;
}

OBJECTREF SecurityDescriptor::GetSerializedEvidence( PEFile* pPEFile )
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    DWORD cbResource;
    OBJECTREF serializedEvidence;
    PBYTE pbInMemoryResource = NULL;

    // Get the resource, and associated file handle, from the assembly.
    if (!pPEFile->GetResource(s_strSecurityEvidence, 
                                     &cbResource, &pbInMemoryResource,
                               NULL, NULL, NULL, NULL, FALSE, TRUE, NULL, NULL))
        return NULL;

    SecurityAttributes::CopyEncodingToByteArray( pbInMemoryResource,
                                             cbResource,
                                             &serializedEvidence );

    // Successfully read all data, set the allocated array as the return value.
    return serializedEvidence;
}

BOOL PEFileSecurityDescriptor::QuickIsFullyTrusted()
{
    WRAPPER_CONTRACT;

    static const SecurityConfig::QuickCacheEntryType fullTrustTable[] = {
        SecurityConfig::FullTrustZoneMyComputer,
        SecurityConfig::FullTrustZoneIntranet,
        SecurityConfig::FullTrustZoneInternet,
        SecurityConfig::FullTrustZoneTrusted,
        SecurityConfig::FullTrustZoneUntrusted
    };
    return (Security::DoesFullTrustMeanFullTrust() && CheckQuickCache(SecurityConfig::FullTrustAll, fullTrustTable));
}

OBJECTREF PEFileSecurityDescriptor::GetEvidence()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(m_pAppDomain == GetAppDomain());
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    struct _gc
    {
        OBJECTREF objAsm;
        OBJECTREF objAdditionalEvidence;
        OBJECTREF objEvidence;
    } gc;
    ZeroMemory( &gc, sizeof( gc ) );

    gc.objAdditionalEvidence = ObjectFromLazyHandle( m_hAdditionalEvidence );

    GCPROTECT_BEGIN( gc );
    BEGIN_SO_INTOLERANT_CODE(GetThread());

    gc.objEvidence = SecurityDescriptor::GetEvidenceForPEFile( m_pPEFile, gc.objAsm, gc.objAdditionalEvidence );

    END_SO_INTOLERANT_CODE;

    GCPROTECT_END();

    return gc.objEvidence;
}

DWORD PEFileSecurityDescriptor::GetZone()
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

    DWORD       dwZone = (DWORD) NoZone;
    BEGIN_SO_INTOLERANT_CODE(GetThread());

    StackSString    codebase;
    BYTE        rbUniqueID[MAX_SIZE_SECURITY_ID];
    DWORD       cbUniqueID = sizeof(rbUniqueID);

    m_pPEFile->GetSecurityIdentity(codebase, &dwZone, rbUniqueID, &cbUniqueID);
    SetZone(dwZone);
    END_SO_INTOLERANT_CODE;
    return dwZone;
}

void PEFileSecurityDescriptor::Resolve()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
    } CONTRACTL_END;

    if (IsResolved())
        return;
    ResolveWorker();
}

void PEFileSecurityDescriptor::ResolveWorker()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    struct _gc {
        OBJECTREF evidence;         // Object containing evidence
        OBJECTREF granted;          // Policy based Granted Permission
        OBJECTREF grantdenied;      // Policy based explicitly Denied Permissions
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);

    int dwSpecialFlags;
    if (QuickIsFullyTrusted()) {
        Security::GetPermissionInstance(&gc.granted, SECURITY_FULL_TRUST);
        dwSpecialFlags = 0xFFFFFFFF;
    } else {
        if (IsEvidenceComputed())
            gc.evidence = ObjectFromLazyHandle(m_hAdditionalEvidence);
        else
            gc.evidence = GetEvidence();
        gc.granted = SecurityPolicy::ResolvePolicy(gc.evidence, NULL, NULL, NULL, &gc.grantdenied, &dwSpecialFlags, FALSE);
    }

    SetGrantedPermissionSet(gc.granted, NULL, dwSpecialFlags);

    GCPROTECT_END();
}

BOOL PEFileSecurityDescriptor::AllowBindingRedirects()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsResolved());
    } CONTRACTL_END;

    ETWTraceStartup trace(ETW_TYPE_STARTUP_ALLOWBINDINGREDIRS);
    TIMELINE_AUTO(STARTUP, "AllowBindingRedirects");

    return CheckSpecialFlag(1 << SECURITY_BINDING_REDIRECTS);
}
