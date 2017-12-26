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



//-----------------------------------------------------------------------------
//
//
//     CODE FOR MAKING THE SECURITY STUB AT JIT-TIME
//
//
//-----------------------------------------------------------------------------


enum DeclSecMergeMethod
{
    DS_METHOD_OVERRIDE,
    DS_CLASS_OVERRIDE,
    DS_UNION,
    DS_INTERSECT,
    DS_APPLY_METHOD_THEN_CLASS, // not supported with stack modifier actions
    DS_APPLY_CLASS_THEN_METHOD, // not supported with stack modifier actions
    DS_NOT_APPLICABLE, // action not supported on both method and class
};

// (Note: The values that are DS_NOT_APPLICABLE are not hooked up to
// this table, so changing one of those values will have no effect)
const DeclSecMergeMethod g_DeclSecClassAndMethodMergeTable[] =
{
    DS_NOT_APPLICABLE, // dclActionNil = 0
    DS_NOT_APPLICABLE, // dclRequest = 1
    DS_UNION, // dclDemand = 2
    DS_METHOD_OVERRIDE, // dclAssert = 3
    DS_UNION, // dclDeny = 4
    DS_INTERSECT, // dclPermitOnly = 5
    DS_NOT_APPLICABLE, // dclLinktimeCheck = 6
    DS_NOT_APPLICABLE, // dclInheritanceCheck = 7
    DS_NOT_APPLICABLE, // dclRequestMinimum = 8
    DS_NOT_APPLICABLE, // dclRequestOptional = 9
    DS_NOT_APPLICABLE, // dclRequestRefuse = 10
    DS_NOT_APPLICABLE, // dclPrejitGrant = 11
    DS_NOT_APPLICABLE, // dclPrejitDenied = 12
    DS_UNION, // dclNonCasDemand = 13
    DS_NOT_APPLICABLE, // dclNonCasLinkDemand = 14
    DS_NOT_APPLICABLE, // dclNonCasInheritance = 15
};

// This table specifies the order in which runtime declarative actions will be performed
// (Note that for stack-modifying actions, this means the order in which they are applied to the
//  frame descriptor, not the order in which they are evaluated when a demand is performed.
//  That order is determined by the code in System.Security.FrameSecurityDescriptor.)
const CorDeclSecurity g_RuntimeDeclSecOrderTable[] =
{
    dclPermitOnly, // 5
    dclDeny, // 4
    dclAssert, // 3
    dclDemand, // 2
    dclNonCasDemand, // 13
};

#define DECLSEC_RUNTIME_ACTION_COUNT (sizeof(g_RuntimeDeclSecOrderTable) / sizeof(CorDeclSecurity))


TokenDeclActionInfo* TokenDeclActionInfo::Init(DWORD dwAction, DWORD dwSetIndex)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    AppDomain                   *pDomain        = GetAppDomain();

    TokenDeclActionInfo *pTemp;
	pTemp = static_cast<TokenDeclActionInfo*>((void*)pDomain->GetLowFrequencyHeap()
							->AllocMem_NoThrow(sizeof(TokenDeclActionInfo)));
    if (pTemp == NULL)
        ThrowOutOfMemory();

    pTemp->dwDeclAction = dwAction;
    pTemp->dwSetIndex = dwSetIndex;
    pTemp->pNext = NULL;

    return pTemp;
	
}

void TokenDeclActionInfo::LinkNewDeclAction(TokenDeclActionInfo** ppActionList, CorDeclSecurity action, DWORD dwSetIndex)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    TokenDeclActionInfo *temp = Init(DclToFlag(action), dwSetIndex);
    if (!(*ppActionList))
        *ppActionList = temp;
    else
    {
        temp->pNext = *ppActionList;
        *ppActionList = temp;
    }
}
UINT_PTR SecurityDeclarative::GetSecurityStubToken(MethodDesc* pMD, DWORD dwDeclFlags)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    return (UINT_PTR)DetectDeclActions(pMD, dwDeclFlags);
}


DeclActionInfo *DeclActionInfo::Init(MethodDesc *pMD, DWORD dwAction, DWORD dwSetIndex)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;
    DeclActionInfo *pTemp;
    pTemp = (DeclActionInfo *)(void*)pMD->GetDomain()->GetLowFrequencyHeap()->AllocMem_NoThrow(sizeof(DeclActionInfo));

    if (pTemp == NULL)
        ThrowOutOfMemory();

    pTemp->dwDeclAction = dwAction;
    pTemp->dwSetIndex = dwSetIndex;
    pTemp->pNext = NULL;

    return pTemp;
}

void LinkNewDeclAction(DeclActionInfo** ppActionList, CorDeclSecurity action, DWORD dwSetIndex, MethodDesc *pMeth)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    DeclActionInfo *temp = DeclActionInfo::Init(pMeth, DclToFlag(action), dwSetIndex);
    if (!(*ppActionList))
        *ppActionList = temp;
    else
    {
        // Add overrides to the end of the list, all others to the front
        if (IsDclActionAnyStackModifier(action))
        {
            DeclActionInfo *w = *ppActionList;
            while (w->pNext != NULL)
                w = w->pNext;
            (*ppActionList)->pNext = temp;
        }
        else
        {
            temp->pNext = *ppActionList;
            *ppActionList = temp;
        }
    }
}

void SecurityDeclarative::AddDeclAction(CorDeclSecurity action, DWORD dwClassSetIndex, DWORD dwMethodSetIndex, DeclActionInfo** ppActionList, MethodDesc *pMeth)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    if(dwClassSetIndex == INVALID_SET_INDEX)
    {
        if(dwMethodSetIndex == INVALID_SET_INDEX)
            return;
        LinkNewDeclAction(ppActionList, action, dwMethodSetIndex, pMeth);
        return;
    }
    else if(dwMethodSetIndex == INVALID_SET_INDEX)
    {
        LinkNewDeclAction(ppActionList, action, dwClassSetIndex, pMeth);
        return;
    }

    // Merge class and method declarations
    switch(g_DeclSecClassAndMethodMergeTable[action])
    {
    case DS_METHOD_OVERRIDE:
        LinkNewDeclAction(ppActionList, action, dwMethodSetIndex, pMeth);
        break;

    case DS_CLASS_OVERRIDE:
        LinkNewDeclAction(ppActionList, action, dwClassSetIndex, pMeth);
        break;

    case DS_UNION:
        LinkNewDeclAction(ppActionList, action, SecurityAttributes::MergePermissionSets(dwClassSetIndex, dwMethodSetIndex, false, action), pMeth);
        break;

    case DS_INTERSECT:
        LinkNewDeclAction(ppActionList, action, SecurityAttributes::MergePermissionSets(dwClassSetIndex, dwMethodSetIndex, true, action), pMeth);
        break;

    case DS_APPLY_METHOD_THEN_CLASS:
        LinkNewDeclAction(ppActionList, action, dwClassSetIndex, pMeth); // note: order reversed because LinkNewDeclAction inserts at beginning of list
        LinkNewDeclAction(ppActionList, action, dwMethodSetIndex, pMeth);
        break;

    case DS_APPLY_CLASS_THEN_METHOD:
        LinkNewDeclAction(ppActionList, action, dwMethodSetIndex, pMeth); // note: order reversed because LinkNewDeclAction inserts at beginning of list
        LinkNewDeclAction(ppActionList, action, dwClassSetIndex, pMeth);
        break;

    case DS_NOT_APPLICABLE:
        _ASSERTE(!"not a runtime action");
        break;

    default:
        _ASSERTE(!"unexpected merge type");
        break;
    }
}


// Here we see what declarative actions are needed everytime a method is called,
// and create a list of these actions, which will be emitted as an argument to
// DoDeclarativeSecurity
DeclActionInfo* SecurityDeclarative::DetectDeclActions(MethodDesc *pMeth, DWORD dwDeclFlags)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    DeclActionInfo              *pDeclActions = NULL;

    EEClass *pCl = pMeth -> GetClass () ;
    _ASSERTE(pCl && "Should be a EEClass pointer here") ;

    Module *pModule = pMeth -> GetModule () ;
    _ASSERTE(pModule && "Should be a Module pointer here") ;
    PREFIX_ASSUME_MSG(pModule != NULL, "Should be a Module pointer here");

#ifdef _DEBUG
    SecurityProperties* psp = pCl -> GetSecurityProperties () ;
    _ASSERTE(psp && "Should be a PSecurityProperties here") ;
#endif

    IMDInternalImport *pInternalImport = pModule->GetMDImport();

    // Lets check the Ndirect/Interop cases first
    if (dwDeclFlags & DECLSEC_UNMNGD_ACCESS_DEMAND)
    {
        HRESULT hr = S_FALSE;
        if (pMeth->HasSuppressUnmanagedCodeAccessAttr())
        {
            dwDeclFlags &= ~DECLSEC_UNMNGD_ACCESS_DEMAND;
        }
        else
        {
            if (pMeth->GetClass())
            {
                // If speculatively true then check the CA
                if (pMeth->GetClass()->HasSuppressUnmanagedCodeAccessAttr())
                {
                    hr = pInternalImport->GetCustomAttributeByName(pMeth->GetMethodTable()->GetCl(),
                                                                   COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                                   NULL,
                                                                   NULL);
                    if (hr != S_OK)
                        pMeth->GetClass()->SetDoesNotHaveSuppressUnmanagedCodeAccessAttr();
                }
                _ASSERTE(SUCCEEDED(hr));
                if (hr == S_OK)
                    dwDeclFlags &= ~DECLSEC_UNMNGD_ACCESS_DEMAND;
            }
        }
        // Check if now there are no actions left
        if (dwDeclFlags == 0)
            return NULL;

        if (dwDeclFlags & DECLSEC_UNMNGD_ACCESS_DEMAND)
        {
            // A NDirect/Interop demand is required.
            DeclActionInfo *temp = DeclActionInfo::Init(pMeth, DECLSEC_UNMNGD_ACCESS_DEMAND, NULL);
            if (!pDeclActions)
                pDeclActions = temp;
            else
            {
                temp->pNext = pDeclActions;
                pDeclActions = temp;
            }
        }
    } // if DECLSEC_UNMNGD_ACCESS_DEMAND

    // Find class declarations
    DWORD classSetIndexes[dclMaximumValue + 1];
/*    DWORD* classSetIndexes = pCl->GetDeclarativeSecuritySets();
    if (!classSetIndexes)
    {
        classSetIndexes = new DWORD[dclMaximumValue + 1];
        pCl->SetDeclarativeSecuritySets(classSetIndexes);*/
        DetectDeclActionsOnToken(pCl->GetCl(), dwDeclFlags, classSetIndexes, pInternalImport);
//    }

    // Find method declarations
    DWORD methodSetIndexes[dclMaximumValue + 1];
    DetectDeclActionsOnToken(pMeth->GetMemberDef(), dwDeclFlags, methodSetIndexes, pInternalImport);

    // Make sure the g_DeclSecClassAndMethodMergeTable is okay
    _ASSERTE(sizeof(g_DeclSecClassAndMethodMergeTable) == sizeof(DeclSecMergeMethod) * (dclMaximumValue + 1) &&
            "g_DeclSecClassAndMethodMergeTable wrong size!");

    // Merge class and method runtime declarations into a single linked list of set indexes
    int i;
    for(i = DECLSEC_RUNTIME_ACTION_COUNT - 1; i >= 0; i--) // note: the loop uses reverse order because AddDeclAction inserts at beginning of the list
    {
        CorDeclSecurity action = g_RuntimeDeclSecOrderTable[i];
        _ASSERTE(action > dclActionNil && action <= dclMaximumValue && "action out of range");
        AddDeclAction(action, classSetIndexes[action], methodSetIndexes[action], &pDeclActions, pMeth);
    }

    return pDeclActions;
}

void SecurityDeclarative::DetectDeclActionsOnToken(mdToken tk, DWORD dwDeclFlags, DWORD* pSetIndexes, IMDInternalImport *pInternalImport)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;
    
    // Make sure the DCL to Flag table is okay
    _ASSERTE(DclToFlag(dclDemand) == DECLSEC_DEMANDS &&
             sizeof(DCL_FLAG_MAP) == sizeof(DWORD) * (dclMaximumValue + 1) &&
             "DCL_FLAG_MAP out of sync with CorDeclSecurity!");

    // Initialize the array
    int i;
    for(i = 0; i < dclMaximumValue + 1; i++)
        pSetIndexes[i] = INVALID_SET_INDEX;

    // Look up declarations on the token for each SecurityAction
    DWORD dwAction;
    for (dwAction = 0; dwAction <= dclMaximumValue; dwAction++)
    {
        // don't bother with actions that are not in the requested mask
        CorDeclSecurity action = (CorDeclSecurity)dwAction;
        DWORD dwActionFlag = DclToFlag(action);
        if ((dwDeclFlags & dwActionFlag) == 0)
            continue;

        // Load the PermissionSet or PermissionSetCollection from the security action table in the metadata
        DWORD dwSetIndex;
        HRESULT hr = SecurityAttributes::GetDeclaredPermissions(pInternalImport, tk, action, NULL, &dwSetIndex);
        if (hr != S_OK) // returns S_FALSE if it didn't find anything in the metadata
            continue;

        pSetIndexes[dwAction] = dwSetIndex;
    }
}

// Returns TRUE if there is a possibility that a token has declarations of the type specified by 'action'
// Returns FALSE if it can determine that the token definately does not.
BOOL SecurityDeclarative::TokenMightHaveDeclarations(IMDInternalImport *pInternalImport, mdToken token, CorDeclSecurity action)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    HENUMInternal hEnumDcl;
    DWORD cDcl;

    // Check if the token has declarations for
    // the action specified.
    hr = pInternalImport->EnumPermissionSetsInit(
        token,
        action,
        &hEnumDcl);

    if (FAILED(hr) || hr == S_FALSE)
    {
        // PermissionSets for non-CAS actions are special cases because they may be mixed with
        // the set for the corresponding CAS action in a serialized CORSEC_PSET
        if(action == dclNonCasDemand || action == dclNonCasLinkDemand || action == dclNonCasInheritance)
        {
            // See if the corresponding CAS action has permissions
            BOOL fDoCheck = FALSE;
            if(action == dclNonCasDemand)
                    fDoCheck = TokenMightHaveDeclarations(pInternalImport, token, dclDemand);
            else if(action == dclNonCasLinkDemand)
                    fDoCheck = TokenMightHaveDeclarations(pInternalImport, token, dclLinktimeCheck);
            else if(action == dclNonCasInheritance)
                    fDoCheck = TokenMightHaveDeclarations(pInternalImport, token, dclInheritanceCheck);
            if(fDoCheck)
            {
                // We can't tell for sure if there are declarations unless we deserializing something
                // (which is too expensive), so we'll just return TRUE
                return TRUE;
            /*
                OBJECTREF refPermSet = NULL;
                DWORD dwIndex = ~0;
                hr = SecurityAttributes::GetDeclaredPermissionsWithCache(pInternalImport, token, action, &refPermSet, &dwIndex);
                if(refPermSet != NULL)
                {
                    _ASSERTE(dwIndex != (~0));
                    return TRUE;
                }
            */
            }
        }
        pInternalImport->EnumClose(&hEnumDcl);
        return FALSE;
    }

    cDcl = pInternalImport->EnumGetCount(&hEnumDcl);
    pInternalImport->EnumClose(&hEnumDcl);

    return (cDcl > 0);
}


bool SecurityDeclarative::BlobMightContainNonCasPermission(PBYTE pbAttrSet, ULONG cbAttrSet, DWORD dwAction)
{
    CONTRACTL {
        THROWS;
    } CONTRACTL_END;

    // Deserialize the CORSEC_ATTRSET
    CORSEC_ATTRSET attrSet;
    HRESULT hr = BlobToAttributeSet(pbAttrSet, &attrSet, dwAction);
    if(FAILED(hr))
        COMPlusThrowHR(hr);

    // this works because SecurityAttributes::CanUnrestrictedOverride only returns
    // true if the attribute set contains only well-known non-CAS permissions
    return !SecurityAttributes::ContainsBuiltinCASPermsOnly(&attrSet);
}

// Accumulate status of declarative security.
HRESULT SecurityDeclarative::GetDeclarationFlags(IMDInternalImport *pInternalImport, mdToken token, DWORD* pdwFlags, DWORD* pdwNullFlags, BOOL* pfHasSuppressUnmanagedCodeAccessAttr /*[IN:TRUE if Pinvoke/Cominterop][OUT:FALSE if doesn't have attr]*/)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    HENUMInternal   hEnumDcl;
    HRESULT         hr;
    DWORD           dwFlags = 0;
    DWORD           dwNullFlags = 0;

    _ASSERTE(pdwFlags);
    *pdwFlags = 0;

    if (pdwNullFlags)
        *pdwNullFlags = 0;

    hr = pInternalImport->EnumPermissionSetsInit(token, dclActionNil, &hEnumDcl);
    if (FAILED(hr))
        goto Exit;

    if (hr == S_OK)
    {
        //Look through the security action table in the metadata for declared permission sets
        mdPermission    perms;
        DWORD           dwAction;
        DWORD           dwDclFlags;
        ULONG           cbPerm;
        PBYTE           pbPerm;
        while (pInternalImport->EnumNext(&hEnumDcl, &perms))
        {
           pInternalImport->GetPermissionSetProps(
                perms,
                &dwAction,
                (const void**)&pbPerm,
                &cbPerm);

            dwDclFlags = DclToFlag(dwAction);

            dwFlags |= dwDclFlags;

            if(pbPerm[0] == LAZY_DECL_SEC_FLAG) // indicates a serialized CORSEC_PSET
            {
                if(BlobMightContainNonCasPermission(pbPerm, cbPerm, dwAction))
                {
                    switch(dwAction)
                    {
                        case dclDemand:
                            dwFlags |= DclToFlag(dclNonCasDemand);
                            break;
                        case dclLinktimeCheck:
                            dwFlags |= DclToFlag(dclNonCasLinkDemand);
                            break;
                        case dclInheritanceCheck:
                            dwFlags |= DclToFlag(dclNonCasInheritance);
                            break;
                    }
                }
            }
        }
    }
    pInternalImport->EnumClose(&hEnumDcl);

    // Disable any runtime checking of UnmanagedCode permission if the correct
    // custom attribute is present.
    // By default, check except when told not to by the passed in BOOL*

    BOOL hasSuppressUnmanagedCodeAccessAttr;
    if (pfHasSuppressUnmanagedCodeAccessAttr == NULL)
    {
        hasSuppressUnmanagedCodeAccessAttr = 
          (pInternalImport->GetCustomAttributeByName(token,
                                                     COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                     NULL,
                                                     NULL) == S_OK);
    }
    else
        hasSuppressUnmanagedCodeAccessAttr = *pfHasSuppressUnmanagedCodeAccessAttr;
        

    if (hasSuppressUnmanagedCodeAccessAttr)
    {
        dwFlags |= DECLSEC_UNMNGD_ACCESS_DEMAND;
        dwNullFlags |= DECLSEC_UNMNGD_ACCESS_DEMAND;
    }

    *pdwFlags = dwFlags;
    if (pdwNullFlags)
        *pdwNullFlags = dwNullFlags;

Exit:
    return hr;
}

bool SecurityDeclarative::MethodRequiresStub(UINT_PTR uSecurityStubToken)
{
    LEAF_CONTRACT;
    return (NULL != uSecurityStubToken);
}

Stub* SecurityDeclarative::CreateStub(SecurityStubLinker* pstublinker,
                           MethodDesc* pMD,
                           DWORD dwDeclFlags,
                           UINT_PTR uSecurityStubToken,
                           Stub* pRealStub,
                           LPVOID pRealAddr)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    DeclActionInfo *actionsNeeded = (DeclActionInfo*) uSecurityStubToken;
    if (actionsNeeded == NULL)
        return NULL;       // Nothing to do

    // Wrapper needs to know if it needs to setup the MD on exit
    SecurityStubLinker *psl = (SecurityStubLinker*)pstublinker;

    BOOL fWrappedTargetRequiresMD = NULL != pRealStub && pRealStub->RequiresMethodDescCallingConvention();

    if(dwDeclFlags & DECLSEC_FRAME_ACTIONS)
    {
        if (pMD->IsVarArg())
            COMPlusThrow(kNotSupportedException, L"NotSupported_DeclSecVarArg");

        pRealAddr = psl->EmitSecurityWrapperStub(pMD->SizeOfActualFixedArgStack(), pMD, fWrappedTargetRequiresMD, pRealAddr, actionsNeeded);
    }
    else
    {
        pRealAddr = psl->EmitSecurityInterceptorStub(pMD, fWrappedTargetRequiresMD, pRealAddr, actionsNeeded);
    }

    Stub* result = psl->LinkInterceptor(pMD->GetDomain()->GetStubHeap(), pRealStub, pRealAddr);

    result->SetRequiresMethodDescCallingConvention();

    return result;
}

















//-----------------------------------------------------------------------------
//
//
//     CODE FOR PERFORMING JIT-TIME CHECKS
//
//
//-----------------------------------------------------------------------------

void SecurityDeclarative::_GetSharedPermissionInstance(OBJECTREF *perm, int index)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    _ASSERTE(index < (int) NUM_PERM_OBJECTS);

    AppDomain *pDomain = GetAppDomain();
    SharedPermissionObjects *pShared = &pDomain->m_pSecContext->m_rPermObjects[index];

    if (pShared->hPermissionObject == NULL) {
        pShared->hPermissionObject = pDomain->CreateHandle(NULL);
        *perm = NULL;
    }
    else
        *perm = ObjectFromHandle(pShared->hPermissionObject);

    if (*perm == NULL)
    {
        MethodTable *pMT = NULL;
        OBJECTREF p = NULL;

        GCPROTECT_BEGIN(p);

        pMT = g_Mscorlib.GetClass(pShared->idClass);
        MethodDescCallSite  ctor(pShared->idConstructor);

        p = AllocateObject(pMT);

        ARG_SLOT argInit[2] =
        {
            ObjToArgSlot(p),
            (ARG_SLOT) pShared->dwPermissionFlag
        };

        ctor.Call(argInit);

        StoreObjectInHandle(pShared->hPermissionObject, p);
        *perm = p;

        GCPROTECT_END();
    }
}

void SecurityDeclarative::GetAPTCAException(OBJECTREF *pThrowable, Assembly *pCaller, MethodDesc *pCallee)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    EX_TRY
    {
        MethodDescCallSite throwSecurityException(METHOD__SECURITY_ENGINE__THROW_SECURITY_EXCEPTION);

        OBJECTREF callerObj = NULL;
        if (pCaller->GetDomain() == GetAppDomain())
            callerObj = pCaller->GetExposedObject();

        ARG_SLOT args[7];
        args[0] = ObjToArgSlot(callerObj);
        args[1] = ObjToArgSlot(NULL);
        args[2] = ObjToArgSlot(NULL);
        args[3] = PtrToArgSlot(pCallee);
        args[4] = (ARG_SLOT)dclLinktimeCheck;
        args[5] = ObjToArgSlot(NULL);
        args[6] = ObjToArgSlot(NULL);
        throwSecurityException.Call(args);
    }
    EX_CATCH
    {
        UpdateThrowable(pThrowable);
    }
    EX_END_CATCH(RethrowTerminalExceptions);
}

void SecurityDeclarative::GetHPException(OBJECTREF *pThrowable, EApiCategories protectedCategories, EApiCategories demandedCategories)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    OBJECTREF hpException = NULL;
    GCPROTECT_BEGIN(hpException);

    static MethodTable* pMT = NULL;
    if (pMT == NULL)
        pMT = g_Mscorlib.GetClass(CLASS__HOST_PROTECTION_EXCEPTION);
    hpException = (OBJECTREF) AllocateObject(pMT);


    MethodDescCallSite ctor(METHOD__HOST_PROTECTION_EXCEPTION__CTOR);

    ARG_SLOT arg[3] = { 
        ObjToArgSlot(hpException),
        protectedCategories,
        demandedCategories
    };
    ctor.Call(arg);
    
    GCPROTECT_END();
    *pThrowable = hpException;
    return;

}

BOOL SecurityDeclarative::IsUntrustedCallerCheckNeeded(MethodDesc *pCalleeMD, Assembly *pCallerAssem)
{
    if (!SecurityPolicy::IsSecurityOn() ||
        !MethodIsVisibleOutsideItsAssembly(pCalleeMD->GetAttrs()) ||
        !ClassIsVisibleOutsideItsAssembly(pCalleeMD->GetClass()->GetAttrClass())||
        pCalleeMD->GetAssembly()->AllowUntrustedCaller() ||
        (pCallerAssem && pCallerAssem == pCalleeMD->GetAssembly()))
        return FALSE;

    return TRUE;
}

// Do a fulltrust check on the caller if the callee is fully trusted and
// callee did not enable AllowUntrustedCallerChecks
/*static*/
BOOL SecurityDeclarative::DoUntrustedCallerChecks(
        Assembly *pCaller, MethodDesc *pCallee, OBJECTREF *pThrowable,
        BOOL fFullStackWalk)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    BOOL fRet = TRUE;

#ifdef _DEBUG
    if (!g_pConfig->Do_AllowUntrustedCaller_Checks())
        return TRUE;
#endif

    if (!IsUntrustedCallerCheckNeeded(pCallee, pCaller))
        return TRUE;

    // Expensive calls after this point, this could end up resolving policy

    OBJECTREF permSet;

    if (fFullStackWalk)
    {
        // It is possible that wrappers like VBHelper libraries that are
        // fully trusted, make calls to public methods that do not have
        // safe for Untrusted caller custom attribute set.
        // Like all other link demand that gets transformed to a full stack
        // walk for reflection, calls to public methods also gets
        // converted to full stack walk

        permSet = NULL;
        GCPROTECT_BEGIN(permSet);

        GetPermissionInstance(&permSet, SECURITY_FULL_TRUST);
        EX_TRY
        {
            SecurityStackWalk::DemandSet(SSWT_LATEBOUND_LINKDEMAND, permSet);
        }
        EX_CATCH
        {
            fRet = FALSE;
        }
        EX_END_CATCH(RethrowTerminalExceptions);

        GCPROTECT_END();
    }
    else
    {
        _ASSERTE(pCaller);

        // Link Demand only, no full stack walk here
        if (!pCaller->GetSecurityDescriptor()->IsFullyTrusted())
            fRet = FALSE;
        else
        {
            // Add fulltrust permission Set to the prejit case.
            GetAppDomain()->OnLinktimeFullTrustCheck(pCaller);
        }
    }

    if (!fRet && pThrowable != RETURN_ON_ERROR)
        GetAPTCAException(pThrowable, pCaller, pCallee);

    return fRet;
}

void SecurityDeclarative::ClassInheritanceCheck(EEClass *pClass, EEClass *pParent)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pClass));
        PRECONDITION(CheckPointer(pParent));
        PRECONDITION(!pClass->IsInterface());
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    // Fast path check
    if (FullTrustCheckForLinkOrInheritanceDemand(pClass->GetAssembly()))
        return;

    // Regular check since Fast path check didn't succeed
    TypeSecurityDescriptor typeSecDesc(pParent);
    typeSecDesc.InvokeInheritanceChecks(pClass);

}

void SecurityDeclarative::MethodInheritanceCheck(MethodDesc *pMethod, MethodDesc *pParent)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pParent));
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;
    
    _ASSERTE (pParent->RequiresInheritanceCheck());
    _ASSERTE(GetThread() != NULL);
    
    // Fast path check    
    if (FullTrustCheckForLinkOrInheritanceDemand(pMethod->GetAssembly()))
        return;
    
    // Regular check since Fast path check didn't succeed    
    MethodSecurityDescriptor MDSecDesc(pParent); 
    MDSecDesc.InvokeInheritanceChecks(pMethod);


    
}


// Retrieve all linktime demands sets for a method. This includes both CAS and
// non-CAS sets for LDs at the class and the method level, so we could get up to
// four sets.
void SecurityDeclarative::RetrieveLinktimeDemands(MethodDesc  *pMD,
                                       OBJECTREF   *pClassCas,
                                       OBJECTREF   *pClassNonCas,
                                       OBJECTREF   *pMethodCas,
                                       OBJECTREF   *pMethodNonCas)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    EEClass *pClass = pMD->GetClass();

    // Class level first.
    if (pClass->RequiresLinktimeCheck())
        *pClassCas = TypeSecurityDescriptor::GetLinktimePermissions(pClass, pClassNonCas);

    // Then the method level.
    if (IsMdHasSecurity(pMD->GetAttrs()))
        *pMethodCas = MethodSecurityDescriptor::GetLinktimePermissions(pMD,  pMethodNonCas);
}

//---------------------------------------------------------
// Invoke linktime checks on the caller if demands exist
// for the callee.
//
// TRUE  = check pass
// FALSE = check failed
//---------------------------------------------------------
/*static*/
BOOL SecurityDeclarative::LinktimeCheckMethod(Assembly *pCaller, MethodDesc *pCallee, OBJECTREF *pThrowable)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    // Do a fulltrust check on the caller if the callee is fully trusted         
    if (FullTrustCheckForLinkOrInheritanceDemand(pCaller))
    {
        OBJECTREF permSet = NULL;
        GCPROTECT_BEGIN(permSet);
        GetPermissionInstance(&permSet, SECURITY_FULL_TRUST);
        GetAppDomain()->OnLinktimeCheck(pCaller, permSet, NULL);
        GCPROTECT_END();   	  		
        return TRUE;
    }

    // If it's a delegate BeginInvoke, we need to perform a HostProtection check for synchronization
    EEClass *pTargetClass = pCallee->GetClass();
    if(pTargetClass->IsAnyDelegateClass())
    {
        DelegateEEClass* pDelegateClass = (DelegateEEClass*)pTargetClass;
        if(pCallee == pDelegateClass->m_pBeginInvokeMethod)
        {
            EApiCategories eProtectedCategories = GetHostProtectionManager()->GetProtectedCategories();
            if((eProtectedCategories & eSynchronization) == eSynchronization)
            {
                if(!pCaller->GetSecurityDescriptor()->IsFullyTrusted())
                {
                    GetHPException(pThrowable, eProtectedCategories, eSynchronization);
                    return FALSE;
                }
            }
        }
    }

    // the rest of the LinkDemand checks
    {
        BOOL        fResult = TRUE;

        // Track perfmon counters. Linktime security checkes.
        COUNTER_ONLY(GetPrivatePerfCounters().m_Security.cLinkChecks++);

        // APTCA check
        if (!SecurityDeclarative::DoUntrustedCallerChecks(pCaller, pCallee, pThrowable, FALSE))
        {
              return FALSE;
        }

        // If the class has its own linktime checks, do them first...
        if (pTargetClass->RequiresLinktimeCheck())
        {
            TypeSecurityDescriptor::InvokeLinktimeChecks(pTargetClass, pCaller, &fResult, pThrowable);
        }

        // If the previous check passed, check the method for
        // method-specific linktime checks...
        if (fResult && IsMdHasSecurity(pCallee->GetAttrs()) &&
            (TokenMightHaveDeclarations(pTargetClass->GetMDImport(),
                                  pCallee->GetMemberDef(),
                                  dclLinktimeCheck) ||
             TokenMightHaveDeclarations(pTargetClass->GetMDImport(),
                                  pCallee->GetMemberDef(),
                                  dclNonCasLinkDemand) ))
        {
            MethodSecurityDescriptor::InvokeLinktimeChecks(pCallee, pCaller, &fResult, pThrowable);
        }

        // We perform automatic linktime checks for UnmanagedCode in three cases:
        //   o  P/Invoke calls
        //   o  Calls through an interface that have a suppress runtime check
        //      attribute on them (these are almost certainly interop calls).
        //   o  Interop calls made through method impls.
        if (pCallee->IsNDirect() ||
            (pTargetClass->IsInterface() &&
             (pTargetClass->GetMDImport()->GetCustomAttributeByName(pTargetClass->GetCl(),
                                                                   COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                                   NULL,
                                                                   NULL) == S_OK ||
              pTargetClass->GetMDImport()->GetCustomAttributeByName(pCallee->GetMemberDef(),
                                                                   COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                                   NULL,
                                                                   NULL) == S_OK) ) ||
            (pCallee->IsComPlusCall() && !pCallee->IsInterface()))
        {
            if (pCaller->GetSecurityDescriptor()->CanCallUnmanagedCode())
            {
                GetAppDomain()->OnLinktimeCanCallUnmanagedCheck(pCaller);
            }
            else {
                fResult = FALSE;
                Security::CreateSecurityException(g_SecurityPermissionClassName, SPFLAGSUNMANAGEDCODE, pThrowable);
            }
        }

        return fResult;
    }
}
// Used by interop to simulate the effect of link demands when the caller is
// in fact script constrained by an appdomain setup by IE.
void SecurityDeclarative::CheckLinkDemandAgainstAppDomain(MethodDesc *pMD)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    if (!pMD->RequiresLinktimeCheck())
        return;

    // Find the outermost (closest to caller) appdomain. This
    // represents the domain in which the unmanaged caller is
    // considered to "live" (or, at least, be constrained by).
    AppDomain *pDomain = GetThread()->GetInitialDomain();

    // The link check is only performed if this app domain has
    // security permissions associated with it, which will be
    // the case for all IE scripting callers that have got this
    // far because we automatically reported our managed classes
    // as "safe for scripting".
    ApplicationSecurityDescriptor *pSecDesc = pDomain->GetSecurityDescriptor();
    if (pSecDesc == NULL || pSecDesc->IsDefaultAppDomain())
        return;

    struct _gc
    {
        OBJECTREF refThrowable;
        OBJECTREF refGrant;
        OBJECTREF refDenied;
        OBJECTREF refClassNonCasDemands;
        OBJECTREF refClassCasDemands;
        OBJECTREF refMethodNonCasDemands;
        OBJECTREF refMethodCasDemands;
        OBJECTREF refAssembly;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    GCPROTECT_BEGIN(gc);

    // Do a fulltrust check on the caller if the callee did not enable
    // AllowUntrustedCallerChecks. Pass a NULL caller assembly:
    // DoUntrustedCallerChecks needs to be able to cope with this.
    if (!SecurityDeclarative::DoUntrustedCallerChecks(NULL, pMD, &gc.refThrowable, TRUE))
        COMPlusThrow(gc.refThrowable);

    // Fetch link demand sets from all the places in metadata where we might
    // find them (class and method). These might be split into CAS and non-CAS
    // sets as well.
    SecurityDeclarative::RetrieveLinktimeDemands(pMD,
                                      &gc.refClassCasDemands,
                                      &gc.refClassNonCasDemands,
                                      &gc.refMethodCasDemands,
                                      &gc.refMethodNonCasDemands);

    // Check CAS link demands.
    bool fGotGrantSet = false;
    if (gc.refClassCasDemands != NULL || gc.refMethodCasDemands != NULL)
    {
        // Get grant (and possibly denied) sets from the app
        // domain.
        gc.refGrant = pSecDesc->GetGrantedPermissionSet(&gc.refDenied);
        fGotGrantSet = true;
        gc.refAssembly = pMD->GetClass()->GetAssembly()->GetExposedObject();

        if (gc.refClassCasDemands != NULL)
            SecurityStackWalk::CheckSetHelper(&gc.refClassCasDemands,
                                                        &gc.refGrant,
                                                        &gc.refDenied,
                                                        pDomain,
                                                        pMD,
                                                        &gc.refAssembly,
                                                        dclLinktimeCheck);

        if (gc.refMethodCasDemands != NULL)
            SecurityStackWalk::CheckSetHelper(&gc.refMethodCasDemands,
                                                        &gc.refGrant,
                                                        &gc.refDenied,
                                                        pDomain,
                                                        pMD,
                                                        &gc.refAssembly,
                                                        dclLinktimeCheck);

    }

    // Non-CAS demands are not applied against a grant
    // set, they're standalone.
    if (gc.refClassNonCasDemands != NULL)
        CheckNonCasDemand(&gc.refClassNonCasDemands);

    if (gc.refMethodNonCasDemands != NULL)
        CheckNonCasDemand(&gc.refMethodNonCasDemands);


    // We perform automatic linktime checks for UnmanagedCode in three cases:
    //   o  P/Invoke calls (shouldn't get these here, but let's be paranoid).
    //   o  Calls through an interface that have a suppress runtime check
    //      attribute on them (these are almost certainly interop calls).
    //   o  Interop calls made through method impls.
    // Just walk the stack in these cases, they'll be extremely rare and the
    // perf delta isn't that huge.
    if (pMD->IsNDirect() ||
        (pMD->IsInterface() &&
         (pMD->GetMDImport()->GetCustomAttributeByName(pMD->GetMethodTable()->GetCl(),
                                                      COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                      NULL,
                                                      NULL) == S_OK ||
          pMD->GetMDImport()->GetCustomAttributeByName(pMD->GetMemberDef(),
                                                      COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                      NULL,
                                                      NULL) == S_OK) ) ||
        (pMD->IsComPlusCall() && !pMD->IsInterface()))
        SecurityStackWalk::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_UNMANAGED_CODE);

    GCPROTECT_END();
}

























//-----------------------------------------------------------------------------
//
//
//     CODE FOR PERFORMING RUN-TIME CHECKS
//
//
//-----------------------------------------------------------------------------

extern LPVOID GetSecurityObjectForFrameInternal(StackCrawlMark *stackMark, INT32 create, OBJECTREF *pRefSecDesc);

inline void UpdateFrameSecurityObj(DWORD dwAction, OBJECTREF *refPermSet, OBJECTREF * pSecObj)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    GetSecurityObjectForFrameInternal(NULL, true, pSecObj);

    FRAMESECDESCREF fsdRef = (FRAMESECDESCREF)*pSecObj;
    switch (dwAction)
    {
    // currently we require declarative security to store the data in both the fields in the FSD
        case dclAssert:
            fsdRef->SetDeclarativeAssertions(*refPermSet);  
            {
                PERMISSIONSETREF psRef = (PERMISSIONSETREF)*refPermSet;
                if (psRef != NULL && psRef->IsUnrestricted())
                    fsdRef->SetAssertFT(TRUE);
            }
            break;

        case dclDeny:        
            fsdRef->SetDeclarativeDenials(*refPermSet);
            break;

        case dclPermitOnly:			
            fsdRef->SetDeclarativeRestrictions(*refPermSet);
            break;

        default:
            _ASSERTE(0 && "Unreached, add code to handle if reached here...");
            break;
    }
}

void SecurityDeclarative::EnsureAssertAllowed(MethodDesc *pMeth, MethodSecurityDescriptor *pMSD)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pMeth));
    } CONTRACTL_END;

    // Check if this Assembly has permission to assert 
    if (pMSD == NULL || !pMSD->CanAssert()) // early out if we have an MSD and we already have checked this permission
    {
        Module* pModule = pMeth->GetModule();
        PREFIX_ASSUME_MSG(pModule != NULL, "Should be a Module pointer here");

        if (!Security::CanAssert(pModule))
            SecurityPolicy::ThrowSecurityException(g_SecurityPermissionClassName, SPFLAGSASSERTION);
    }

    // Check if the Method is allowed to assert based on transparent/critical classification
    if (pMSD != NULL)
    {
        if (pMSD->IsCritical())
           return;
        else
            COMPlusThrow(kInvalidOperationException,L"InvalidOperation_AssertTransparentCode");
    }
    else if (SecurityTransparent::IsMethodTransparent(pMeth))
    {
        // if assembly is transparent fail the ASSERT operations
        COMPlusThrow(kInvalidOperationException,L"InvalidOperation_AssertTransparentCode");
    }
    return;

}

void SecurityDeclarative::InvokeDeclarativeActions (MethodDesc *pMeth, DeclActionInfo *pActions, MethodSecurityDescriptor *pMSD)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    OBJECTREF       refPermSet = NULL;
    ARG_SLOT           arg = 0;

    // If we get a real PermissionSet, then invoke the action.
    switch (pActions->dwDeclAction)
    {
    case DECLSEC_DEMANDS:
        SecurityStackWalk::DemandSet(SSWT_DECLARATIVE_DEMAND, pActions->dwSetIndex, dclDemand);
        break;

    case DECLSEC_ASSERTIONS:
        EnsureAssertAllowed(pMeth, pMSD);
        GetThread()->IncrementAssertCount();
                break;

    case DECLSEC_DENIALS:
    case DECLSEC_PERMITONLY:
        GetThread()->IncrementOverridesCount();
        break;

    case DECLSEC_NONCAS_DEMANDS:
        refPermSet = SecurityAttributes::GetPermissionSet(pActions->dwSetIndex, dclNonCasDemand);
        if (refPermSet == NULL)
            break;
        if(!((PERMISSIONSETREF)refPermSet)->CheckedForNonCas() ||((PERMISSIONSETREF)refPermSet)->ContainsNonCas())
        {
            GCPROTECT_BEGIN(refPermSet);
            MethodDescCallSite demand(METHOD__PERMISSION_SET__DEMAND_NON_CAS, &refPermSet);

            arg = ObjToArgSlot(refPermSet);
            demand.Call(&arg);
            GCPROTECT_END();
        }
        break;

    default:
        _ASSERTE(!"Unknown action requested in InvokeDeclarativeActions");
        break;

    } // switch
}

void SecurityDeclarative::InvokeDeclarativeStackModifiers(MethodDesc * pMeth, DeclActionInfo * pActions, OBJECTREF * pSecObj)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    OBJECTREF       refPermSet = NULL;

    // If we get a real PermissionSet, then invoke the action.
    switch (pActions->dwDeclAction)
    {
    case DECLSEC_DEMANDS:
    case DECLSEC_NONCAS_DEMANDS:
        // Nothing to do for demands
        break;

    case DECLSEC_ASSERTIONS:
        refPermSet = SecurityAttributes::GetPermissionSet(pActions->dwSetIndex,dclAssert);
        if (refPermSet == NULL)
            break;
        GCPROTECT_BEGIN(refPermSet);
        // Now update the frame security object
        UpdateFrameSecurityObj(dclAssert, &refPermSet, pSecObj);
        GCPROTECT_END();
        break;

    case DECLSEC_DENIALS:
        // Update the frame security object
        refPermSet = SecurityAttributes::GetPermissionSet(pActions->dwSetIndex,dclDeny);
        if (refPermSet == NULL)
            break;
        GCPROTECT_BEGIN(refPermSet);
        UpdateFrameSecurityObj(dclDeny, &refPermSet, pSecObj);
        GCPROTECT_END();
        break;

    case DECLSEC_PERMITONLY:
        // Update the frame security object
        refPermSet = SecurityAttributes::GetPermissionSet(pActions->dwSetIndex,dclPermitOnly);
        if (refPermSet == NULL)
            break;
        GCPROTECT_BEGIN(refPermSet);
        UpdateFrameSecurityObj(dclPermitOnly, &refPermSet, pSecObj);
        GCPROTECT_END();
        break;


    default:
        _ASSERTE(!"Unknown action requested in InvokeDeclarativeStackModifiers");
        break;

    } // switch
}

void SecurityDeclarative::DoDeclarativeActions(MethodDesc *pMeth, DeclActionInfo *pActions, LPVOID pSecObj, MethodSecurityDescriptor *pMSD)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    // --------------------------------------------------------------------------- //
    //          D E C L A R A T I V E   S E C U R I T Y   D E M A N D S            //
    // --------------------------------------------------------------------------- //
    // The frame is now fully formed, arguments have been copied into place,
    // and synchronization monitors have been entered if necessary.  At this
    // point, we are prepared for something to throw an exception, so we may
    // check for declarative security demands and execute them.  We need a
    // well-formed frame and synchronization domain to accept security excep-
    // tions thrown by the SecurityManager.  We MAY need argument values in
    // the frame so that the arguments may be finalized if security throws an
    // exception across them (unknown).                          
    if (pActions->dwDeclAction == DECLSEC_UNMNGD_ACCESS_DEMAND &&
        pActions->pNext == NULL)
    {
        /* We special-case the security check on single pinvoke/interop calls
           so we can avoid setting up the GCFrame */

        SecurityStackWalk::SpecialDemand(SSWT_DECLARATIVE_DEMAND, SECURITY_UNMANAGED_CODE);
        return;
    }
    else
    {

        for (/**/; pActions; pActions = pActions->pNext)
        {
            if (pActions->dwDeclAction == DECLSEC_UNMNGD_ACCESS_DEMAND)
            {
                SecurityStackWalk::SpecialDemand(SSWT_DECLARATIVE_DEMAND, SECURITY_UNMANAGED_CODE);
            }
            else
            {
                InvokeDeclarativeActions(pMeth, pActions, pMSD);
            }
        }

    }
}
void SecurityDeclarative::DoDeclarativeStackModifiers(MethodDesc *pMeth, AppDomain* pAppDomain, LPVOID pSecObj)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;
    
    AppDomain* pCurrentDomain = GetAppDomain();
    
    if (pCurrentDomain != pAppDomain)
    {
        ENTER_DOMAIN_PTR(pAppDomain, ADV_RUNNINGIN)
        {
            DoDeclarativeStackModifiersInternal(pMeth, pSecObj);
        }
        END_DOMAIN_TRANSITION;
    }
    else
    {
        DoDeclarativeStackModifiersInternal(pMeth, pSecObj);
            }
        }

void SecurityDeclarative::DoDeclarativeStackModifiersInternal(MethodDesc *pMeth, LPVOID pSecObj)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    Object** ppSecObject = (Object**) pSecObj;
    _ASSERTE(pMeth->IsInterceptedForDeclSecurity() && !pMeth->IsInterceptedForDeclSecurityCASDemandsOnly());

    MethodSecurityDescriptor MDSecDesc(pMeth);
    MethodSecurityDescriptor::LookupOrCreateMethodSecurityDescriptor(&MDSecDesc);
    DeclActionInfo* pActions = MDSecDesc.GetRuntimeDeclActionInfo();

    OBJECTREF fsdRef = ObjectToOBJECTREF(*ppSecObject);
    GCPROTECT_BEGIN(fsdRef);

    for (/**/; pActions; pActions = pActions->pNext)
    {
        InvokeDeclarativeStackModifiers(pMeth, pActions, &fsdRef);
    }
    // If we had just NON-CAS demands, we'd come here but not create an FSD.
    if (fsdRef != NULL)
    {
        ((FRAMESECDESCREF)(fsdRef))->SetDeclSecComputed(TRUE);

        if (*ppSecObject == NULL)
        {
            // we came in with a NULL FSD and the FSD got created here...so we need to copy it back
            // If we had come in with a non-NULL FSD, that would have been updated and this (shallow/pointer) copy
            // would not be necessary
            *ppSecObject = OBJECTREFToObject(fsdRef);
    }
}

    GCPROTECT_END();
}

// This functions is logically part of the security stub
VOID __stdcall DoSpecialUnmanagedCodeDemand()
{

    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    // Initialize the security object slot here, as
    // INSTALL_UNWIND_AND_CONTINUE_HANDLER can trigger a GC
    ETWTraceStartup trace(ETW_TYPE_STARTUP_SECURITYCATCHALL);

    // Do I know the result from last time ?  (this path is no throw no gc and actually inlines to just some bit tests
    // so it's safe before the, safe before the INSTALL_UNWIND_AND_CONTINUE_HANDLER and msut stay that way

    if (SecurityStackWalk::HasFlagsOrFullyTrustedIgnoreMode(1<<SECURITY_UNMANAGED_CODE))
    {
        // Track perfmon counters. Runtime security checks.
        SecurityStackWalk::IncrementSecurityPerfCounter();
    } 
    else
    {
        // This method is called from stubs which are called by managed code, so
        // we must have our unwind and continue handler here wrapping anything 
        // that might throw an exception.
        INSTALL_UNWIND_AND_CONTINUE_HANDLER;

        SecurityStackWalk::SpecialDemand(SSWT_DECLARATIVE_DEMAND, SECURITY_UNMANAGED_CODE);

        UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
    }
}


// This functions is logically part of the security stub
VOID __stdcall DoDeclarativeSecurity(MethodDesc *pMeth, DeclActionInfo *pActions, InterceptorFrame* frame)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    // Initialize the security object slot here, as
    // INSTALL_UNWIND_AND_CONTINUE_HANDLER can trigger a GC
    ETWTraceStartup trace(ETW_TYPE_STARTUP_SECURITYCATCHALL);

    
    LPVOID pSecObj = frame->GetAddrOfSecurityDesc();
    *((Object**) pSecObj) = NULL;

    // This method is called from stubs which are called by managed code, so
    // we must have our unwind and continue handler here wrapping anything 
    // that might throw an exception.
    INSTALL_UNWIND_AND_CONTINUE_HANDLER;

    SecurityDeclarative::DoDeclarativeActions(pMeth, pActions, pSecObj);

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
}


#ifndef DACCESS_COMPILE
void SecurityDeclarative::CheckNonCasDemand(OBJECTREF *prefDemand)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsProtectedByGCFrame (prefDemand));
    } CONTRACTL_END;

    if(((PERMISSIONSETREF)*prefDemand)->CheckedForNonCas())
    {
        if(!((PERMISSIONSETREF)*prefDemand)->ContainsNonCas())
            return;
    }
    MethodDescCallSite demand(METHOD__PERMISSION_SET__DEMAND_NON_CAS, prefDemand);
    ARG_SLOT arg = ObjToArgSlot(*prefDemand);
    demand.Call(&arg);
}
#endif // #ifndef DACCESS_COMPILE





    



