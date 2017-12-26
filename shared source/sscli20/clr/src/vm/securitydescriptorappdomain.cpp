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
#include "crossdomaincalls.h"


BOOL ApplicationSecurityDescriptor::QuickIsFullyTrusted()
{
    WRAPPER_CONTRACT;

    if (IsDefaultAppDomain())
        return TRUE;

    if (!Security::DoesFullTrustMeanFullTrust())
        return FALSE;

    // Check if we need to call the HostSecurityManager.
    if (CallHostSecurityManager())
        return FALSE;

    // If this is a homogeneous case, get the PermissionSet from managed code.
    if (IsHomogeneous())
        return FALSE;

    static const SecurityConfig::QuickCacheEntryType fullTrustTable[] = {
        SecurityConfig::FullTrustZoneMyComputer,
        SecurityConfig::FullTrustZoneIntranet,
        SecurityConfig::FullTrustZoneInternet,
        SecurityConfig::FullTrustZoneTrusted,
        SecurityConfig::FullTrustZoneUntrusted
    };
    return CheckQuickCache(SecurityConfig::FullTrustAll, fullTrustTable);
}

OBJECTREF ApplicationSecurityDescriptor::GetEvidence()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(m_pAppDomain == GetAppDomain());
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    OBJECTREF retval = ObjectFromLazyHandle(m_hAdditionalEvidence);
    return retval;
}

void ApplicationSecurityDescriptor::Resolve()
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

void ApplicationSecurityDescriptor::ResolveWorker()
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

DWORD ApplicationSecurityDescriptor::GetZone()
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

    DWORD dwZone = (DWORD) NoZone; 
    if (m_pAppDomain->m_pRootAssembly != NULL && m_pAppDomain->IsDefaultDomain())
    {
        LPCWSTR wszAsmPath = m_pAppDomain->m_pRootAssembly->GetManifestFile()->GetPath();

        if (wszAsmPath)
        {
            StackSString ssPath( L"file://" );
            ssPath.Append( wszAsmPath );

            dwZone = SecurityPolicy::QuickGetZone( ssPath.GetUnicode() );

        }
    }

    SetZone(dwZone);
    return dwZone;
}

//
// PLS (PermissionListSet) optimization Implementation
//   The idea of the PLS optimization is to maintain the intersection
//   of the grant sets of all assemblies loaded into the AppDomain (plus
//   the grant set of the AppDomain itself) and the union of all denied
//   sets. When a demand is evaluated, we first check the permission
//   that is being demanded against the combined grant and denied set
//   and if that check succeeds, then we know the demand is satisfied
//   in the AppDomain without having to perform an entire stack walk.
//

// Creates the PermissionListSet which holds the AppDomain level intersection of
// granted and denied permission sets of all assemblies in the domain and updates
// the granted and denied set with those of the AppDomain.
void ApplicationSecurityDescriptor::InitializePLS()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsResolved());
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    struct _gc {
        OBJECTREF refGrantedSet;
        OBJECTREF refDeniedSet;
        OBJECTREF refPermListSet;
        OBJECTREF refRetVal;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    if (!IsFullyTrusted()) {
        GCPROTECT_BEGIN(gc);

        gc.refPermListSet = ObjectFromLazyHandle(m_hDomainPermissionListSet);
        gc.refGrantedSet = GetGrantedPermissionSet(&gc.refDeniedSet);

        MethodDescCallSite updateAppDomainPLS(METHOD__SECURITY_ENGINE__UPDATE_APPDOMAIN_PLS);
        ARG_SLOT args[] = {
            ObjToArgSlot(gc.refPermListSet),
            ObjToArgSlot(gc.refGrantedSet),
            ObjToArgSlot(gc.refDeniedSet),
        };
        gc.refRetVal = updateAppDomainPLS.Call_RetOBJECTREF(args);

        GCPROTECT_END();
    }

    StoreObjectInLazyHandle(m_hDomainPermissionListSet, gc.refRetVal, m_pAppDomain);
    m_dwDomainWideSpecialFlags = m_dwSpecialFlags;
}

// Whenever a new assembly is added to the domain, we need to update the PermissionListSet
void ApplicationSecurityDescriptor::AddNewSecDescToPLS(AssemblySecurityDescriptor *pNewSecDescriptor)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(pNewSecDescriptor->IsResolved());
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    //
    // If the assembly is fully trusted, this should be a no-op as the PLS is unaffected.
    // Note it's Ok to call this method before the AppDomain is fully initialized (and so
    // before the PLS is created for the AppDomain) because we enforce that all assemblies
    // loaded during that phase are fully trusted.
    //

    if (!pNewSecDescriptor->IsFullyTrusted()) {
        struct _gc {
            OBJECTREF refGrantedSet;
            OBJECTREF refDeniedSet;
            OBJECTREF refPermListSet;
            OBJECTREF refRetVal;
        } gc;
        ZeroMemory(&gc, sizeof(gc));

        GCPROTECT_BEGIN(gc);

        gc.refGrantedSet = pNewSecDescriptor->GetGrantedPermissionSet(&gc.refDeniedSet);
        if (gc.refDeniedSet != NULL)
            m_fContainsAnyRefusedPermissions = TRUE;

        // we have to synchronize the update to the PLS across concurring threads.
        // we don't care if another thread is checking the existing PLS while this
        // update is in progress as the loaded assembly won't be on the stack for such
        // a demand and so will not affect the result of the existing PLS optimization.
        do {
            gc.refPermListSet = ObjectFromLazyHandle(m_hDomainPermissionListSet);

            MethodDescCallSite updateAppDomainPLS(METHOD__SECURITY_ENGINE__UPDATE_APPDOMAIN_PLS);
            ARG_SLOT args[] = {
                ObjToArgSlot(gc.refPermListSet),
                ObjToArgSlot(gc.refGrantedSet),
                ObjToArgSlot(gc.refDeniedSet),
            };
            // This returns a new copy of the PermissionListSet
            gc.refRetVal = updateAppDomainPLS.Call_RetOBJECTREF(args);
        }
        // If some other thread beat us to the PLS object handle, just try updating the PLS again
        // This race should be rare enough that recomputing the PLS is acceptable.
        while ((PVOID) InterlockedCompareExchangeObjectInHandle(m_hDomainPermissionListSet, gc.refRetVal, gc.refPermListSet) != (PVOID) OBJECTREF_TO_UNCHECKED_OBJECTREF(gc.refPermListSet));

        GCPROTECT_END();

        LONG dwNewDomainWideSpecialFlags = 0;
        LONG dwOldDomainWideSpecialFlags = 0;
        do {
            dwOldDomainWideSpecialFlags = m_dwDomainWideSpecialFlags;
            dwNewDomainWideSpecialFlags = (dwOldDomainWideSpecialFlags & pNewSecDescriptor->m_dwSpecialFlags);
        }
        while (InterlockedCompareExchange((LONG volatile *)&m_dwDomainWideSpecialFlags, dwNewDomainWideSpecialFlags, dwOldDomainWideSpecialFlags) != dwOldDomainWideSpecialFlags);
    }
}

BOOL ApplicationSecurityDescriptor::CheckPLS (OBJECTREF* orDemand, DWORD dwDemandSpecialFlags, BOOL fDemandSet)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    // Check if all assemblies so far have full trust.
    if (CheckDomainWideSpecialFlag(1 << SECURITY_FULL_TRUST))
        return TRUE;

    // Check if this is a one of the well-known permissions tracked in the special flags.
    if (CheckDomainWideSpecialFlag(dwDemandSpecialFlags))
        return TRUE;

    BOOL fResult = FALSE;
    //
    // only evaluate the PLS if we don't need to marshal the demand across AppDomains
    // This means we would perform a stack walk when there are multiple semi-trust
    // AppDomains on the stack, which is acceptable.
    // In homogeneous cases, the stack walk could potentially detect the situation
    // and avoid the expensive walk of the assemblies if the permission demand is a 
    // subset of the homogeneous grant set applied to the AppDomain.
    //
    if (m_pAppDomain == GetThread()->GetDomain()) {
        OBJECTREF refDomainPLS = NULL;

        GCPROTECT_BEGIN(refDomainPLS);
        refDomainPLS = ObjectFromLazyHandle(m_hDomainPermissionListSet);

        EX_TRY
        {
            if (fDemandSet) {
                PREPARE_NONVIRTUAL_CALLSITE(METHOD__PERMISSION_LIST_SET__CHECK_SET_DEMAND_NO_THROW);
                DECLARE_ARGHOLDER_ARRAY(args, 2);
                args[ARGNUM_0]  = OBJECTREF_TO_ARGHOLDER(refDomainPLS);    // arg 0
                args[ARGNUM_1]  = OBJECTREF_TO_ARGHOLDER(*orDemand);       // arg 1 
                CALL_MANAGED_METHOD(fResult, Bool, BOOL, args);
            }
            else {
                PREPARE_NONVIRTUAL_CALLSITE(METHOD__PERMISSION_LIST_SET__CHECK_DEMAND_NO_THROW);
                DECLARE_ARGHOLDER_ARRAY(args, 2);
                args[ARGNUM_0]  = OBJECTREF_TO_ARGHOLDER(refDomainPLS);    // arg 0
                args[ARGNUM_1]  = OBJECTREF_TO_ARGHOLDER(*orDemand);       // arg 1 
                CALL_MANAGED_METHOD(fResult, Bool, BOOL, args);
            }
        }
        EX_SWALLOW_NONTRANSIENT;

        GCPROTECT_END();
    }

    return fResult;
}
