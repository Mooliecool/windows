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

#include "appdomainstack.h"
#include "appdomainstack.inl"
#include "security.h"
#include "securitydescriptor.h"
#include "securitypolicy.h"
#include "appdomain.inl"
#include "crossdomaincalls.h"

#ifdef _DEBUG
void AppDomainStack::CheckOverridesAssertCounts()
{
    LEAF_CONTRACT;
    DWORD   dwAppDomainIndex = 0;
    DWORD dwOverrides = 0;
    DWORD dwAsserts = 0;
    AppDomainStackEntry *pEntry = NULL;
    for(dwAppDomainIndex=0;dwAppDomainIndex<m_numEntries;dwAppDomainIndex++)
    {
        pEntry = __GetEntryPtr(dwAppDomainIndex);
        dwOverrides += pEntry->m_dwOverridesCount;
        dwAsserts += pEntry->m_dwAsserts;
    }
    _ASSERTE(dwOverrides == m_dwOverridesCount);
    _ASSERTE(dwAsserts == m_dwAsserts);    
}
#endif

BOOL AppDomainStackEntry::IsFullyTrustedWithNoStackModifiers(void)
{
    WRAPPER_CONTRACT;   
    if (m_domainID.m_dwId == INVALID_APPDOMAIN_ID || m_dwOverridesCount != 0 || m_dwAsserts != 0)
        return FALSE;

    AppDomainFromIDHolder pDomain(m_domainID, FALSE);
    if (pDomain.IsUnloaded())
        return FALSE;
    ApplicationSecurityDescriptor *currAppSecDesc = pDomain->GetSecurityDescriptor();
    if (currAppSecDesc == NULL)
        return FALSE;
    return currAppSecDesc->CheckDomainWideSpecialFlag(1 << SECURITY_FULL_TRUST);
}
BOOL AppDomainStackEntry::IsHomogeneousWithNoStackModifiers(void)
{
    WRAPPER_CONTRACT
    if (m_domainID.m_dwId == INVALID_APPDOMAIN_ID || m_dwOverridesCount != 0 || m_dwAsserts != 0)
        return FALSE;

    AppDomainFromIDHolder pDomain(m_domainID, FALSE);
    if (pDomain.IsUnloaded())
        return FALSE;
    ApplicationSecurityDescriptor *currAppSecDesc = pDomain->GetSecurityDescriptor();
    if (currAppSecDesc == NULL)
        return FALSE;
    return (currAppSecDesc->IsHomogeneous() && !currAppSecDesc->ContainsAnyRefusedPermissions());
}

BOOL AppDomainStackEntry::HasFlagsOrFullyTrustedWithNoStackModifiers(DWORD flags)
{
    WRAPPER_CONTRACT;
    if (m_domainID.m_dwId == INVALID_APPDOMAIN_ID || m_dwOverridesCount != 0 || m_dwAsserts != 0)
        return FALSE;

    AppDomainFromIDHolder pDomain(m_domainID, FALSE);
    if (pDomain.IsUnloaded())
        return FALSE;
    ApplicationSecurityDescriptor *currAppSecDesc = pDomain->GetSecurityDescriptor();
    if (currAppSecDesc == NULL)
        return FALSE;
    
    // either the desired flag (often 0) or fully trusted will do
    flags |= (1<<SECURITY_FULL_TRUST);
    return currAppSecDesc->CheckDomainWideSpecialFlag(flags);
}


void AppDomainStackEntry::UpdateHomogeneousPLS(OBJECTREF* homogeneousPLS)
{
    
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    AppDomain* pDomain = SystemDomain::GetAppDomainFromId(m_domainID,ADV_RUNNINGIN);
    
    ApplicationSecurityDescriptor *thisAppSecDesc = pDomain->GetSecurityDescriptor();
    
    if (thisAppSecDesc->IsHomogeneous())
    {
        // update the intersection with the current grant set
        
        NewArrayHolder<BYTE> pbtmpSerializedObject(NULL);
        DWORD cbtmpSerializedObject = 0;
        
        struct gc
        {
            OBJECTREF refGrantSet;
        } gc;
        ZeroMemory( &gc, sizeof( gc ) );
        AppDomain* pCurrentDomain = GetAppDomain();
            
        GCPROTECT_BEGIN( gc );
        if (pCurrentDomain->GetId() != m_domainID)
        {
            // Unlikely scenario where we have another homogeneous AD on the callstack that's different from
            // the current one. If there's another AD on the callstack, it's likely to be FT.
            ENTER_DOMAIN_ID(m_domainID)
            {
                gc.refGrantSet = thisAppSecDesc->GetGrantedPermissionSet(NULL); 
                AppDomainHelper::MarshalObject(GetAppDomain(), &gc.refGrantSet, &pbtmpSerializedObject, &cbtmpSerializedObject);
                if (pbtmpSerializedObject == NULL)
            	{
                    // this is an error: possibly an OOM prevented the blob from getting created.
                    // We could return null and let the managed code use a fully restricted object or throw here.
                    // Let's throw here...
                    COMPlusThrow(kSecurityException);
                }
                gc.refGrantSet = NULL;
            }
            END_DOMAIN_TRANSITION
            AppDomainHelper::UnmarshalObject(pCurrentDomain,pbtmpSerializedObject, cbtmpSerializedObject, &gc.refGrantSet);
        }
        else
        {
            gc.refGrantSet = thisAppSecDesc->GetGrantedPermissionSet(NULL); 
        }

        // At this point gc.refGrantSet has the grantSet of pDomain (thisAppSecDesc) in the current domain.
        // We don't care about refused perms since we established there were 
        // none earlier for this call stack.
        // Let's intersect with what we've already got.

        PREPARE_NONVIRTUAL_CALLSITE(METHOD__PERMISSION_LIST_SET__UPDATE);
        DECLARE_ARGHOLDER_ARRAY(args, 2);
        args[ARGNUM_0]  = OBJECTREF_TO_ARGHOLDER(*homogeneousPLS);    // arg 0
        args[ARGNUM_1]  = OBJECTREF_TO_ARGHOLDER(gc.refGrantSet);       // arg 1 
        CALL_MANAGED_METHOD_NORET(args);

        GCPROTECT_END();
    }
}


BOOL AppDomainStack::AllDomainsHomogeneousWithNoStackModifiers()
{
    WRAPPER_CONTRACT;

    if (!Security::DoesFullTrustMeanFullTrust())
        return FALSE; // Legacy mode - cannot use homogeneous PLS
    
    // Used primarily by CompressedStack code to decide if a CS has to be constructed 

    DWORD   dwAppDomainIndex = 0;


    InitDomainIteration(&dwAppDomainIndex);
    while (dwAppDomainIndex != 0)
    {
        AppDomainStackEntry* pEntry = GetNextDomainEntryOnStack(&dwAppDomainIndex);
        _ASSERTE(pEntry != NULL);
        
        if (!pEntry->IsHomogeneousWithNoStackModifiers() && !pEntry->IsFullyTrustedWithNoStackModifiers())
            return FALSE;
    }

    return TRUE;
}

