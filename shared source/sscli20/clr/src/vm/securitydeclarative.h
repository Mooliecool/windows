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

#ifndef __SECURITYDECLARATIVE_H__
#define __SECURITYDECLARATIVE_H__

struct bmtInternalInfo;
struct bmtMetaDataInfo;
class SecurityStackWalk;
struct MethodSecurityDescriptor;
struct TokenSecurityDescriptor;
struct TokenDeclActionInfo;
struct TypeSecurityDescriptor;

class SecurityDeclarative
{
friend class Security;
friend class SecurityDescriptor;
friend class SecurityStackWalk;
friend class SecurityRuntime;
friend struct MethodSecurityDescriptor;
friend struct TokenSecurityDescriptor;
friend struct TokenDeclActionInfo;
friend struct TypeSecurityDescriptor;

public:
friend VOID __stdcall DoDeclarativeSecurity(MethodDesc *pMeth, DeclActionInfo *pActions, InterceptorFrame* frame);

// Delayed Declarative Security processing
#ifndef DACCESS_COMPILE
static inline void DoDeclarativeSecurityAtStackWalk(MethodDesc* pFunc, AppDomain* pAppDomain, OBJECTREF* pFrameObjectSlot)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;
    

    BOOL hasDeclarativeStackModifier = (pFunc->IsInterceptedForDeclSecurity() && !pFunc->IsInterceptedForDeclSecurityCASDemandsOnly());
    if (hasDeclarativeStackModifier)
    {

        _ASSERTE(pFrameObjectSlot != NULL);
        if (*pFrameObjectSlot == NULL || !( ((FRAMESECDESCREF)(*pFrameObjectSlot))->IsDeclSecComputed()) )
        {
            // Populate the FSD with declarative assert/deny/PO
            SecurityDeclarative::DoDeclarativeStackModifiers(pFunc, pAppDomain, pFrameObjectSlot);
        }
    }
}
#endif
private:
	// Perform the declarative actions
	//   Callers:
	//     DoDeclarativeSecurity
    static void DoDeclarativeActions(MethodDesc *pMD, DeclActionInfo *pActions, LPVOID pSecObj, MethodSecurityDescriptor *pMSD = NULL);
    static void DoDeclarativeStackModifiers(MethodDesc *pMeth, AppDomain* pAppDomain, LPVOID pSecObj);
    static void DoDeclarativeStackModifiersInternal(MethodDesc *pMeth, LPVOID pSecObj);
    static void EnsureAssertAllowed(MethodDesc *pMeth, MethodSecurityDescriptor* pMSD); // throws exception if assert is not allowed for MethodDesc
	// Determine which declarative SecurityActions are used on this type and return a
	// DWORD of flags to represent the results
	//   Callers:
	//     MethodTableBuilder::CreateClass
	//     MethodTableBuilder::EnumerateClassMembers
	//     MethodDesc::GetSecurityFlags
    static HRESULT GetDeclarationFlags(IMDInternalImport *pInternalImport, mdToken token, DWORD* pdwFlags, DWORD* pdwNullFlags, BOOL* fHasSuppressUnmanagedCodeAccessAttr = NULL);

	// Checks if the flags returned by GetDeclarationFlags are zero or not
	//   Callers:
	//     MethodDesc::DoPrestub
    static bool MethodRequiresStub(UINT_PTR uSecurityStubToken);

	// Query the metadata to get all LinkDemands on this method (and it's class)
	//   Callers:
	//     CanAccess (ReflectionInvocation)
	//     ReflectionInvocation::GetSpecialSecurityFlags
	//     RuntimeMethodHandle::InvokeMethod_Internal
	//     Security::CheckLinkDemandAgainstAppDomain
    static void RetrieveLinktimeDemands(MethodDesc* pMD,
                                        OBJECTREF* pClassCas,
                                        OBJECTREF* pClassNonCas,
                                        OBJECTREF* pMethodCas,
                                        OBJECTREF* pMethodNonCas);

	// Used by interop to simulate the effect of link demands when the caller is
	// in fact script constrained by an appdomain setup by IE.
	//   Callers:
	//     DispatchInfo::InvokeMember
	//     COMToCLRWorkerBody (COMToCLRCall)
    static void CheckLinkDemandAgainstAppDomain(MethodDesc *pMD);

	// Perform a LinkDemand
	//   Callers:
	//     COMCustomAttribute::CreateCAObject
	//     CheckMethodAccess
	//     InvokeUtil::CheckLinktimeDemand
	//     CEEInfo::findMethod
	//     RuntimeMethodHandle::InvokeMethod_Internal
    static BOOL LinktimeCheckMethod(Assembly *pCaller, MethodDesc *pCallee, OBJECTREF *pThrowable); 

	// Do InheritanceDemands on the type
	//   Called by:
	//     MethodTableBuilder::VerifyInheritanceSecurity
    static void ClassInheritanceCheck(EEClass *pClass, EEClass *pParent);

	// Do InheritanceDemands on the Method
	//   Callers:
	//     MethodTableBuilder::VerifyInheritanceSecurity
    static void MethodInheritanceCheck(MethodDesc *pMethod, MethodDesc *pParent);

	// Returns a liked list of all the (security action, set index) pairs for this method
	//   Callers:
    //     InsertUnmanagedCodeCheckSecurityStub
    //     MethodDesc::DoPrestub
    //     COMDelegate::ConvertToDelegate
    static UINT_PTR GetSecurityStubToken(MethodDesc* pMD, DWORD dwDeclFlags);
   

	// Emits a security interceptor stub
	//   Callers:
	//     MakeSecurityWorker (PreStub)
    static Stub* CreateStub(SecurityStubLinker *pstublinker, 
                            MethodDesc* pMD, 
                            DWORD dwDeclFlags,
                            UINT_PTR uSecurityStubToken,
                            Stub* pRealStub, 
                            LPVOID pRealAddr);

	// Returns a managed instance of a well-known PermissionSet
	//   Callers:
	//     COMCodeAccessSecurityEngine::SpecialDemand
	//     ReflectionSerialization::GetSafeUninitializedObject
	//     CompilationDomain::OnLinktimeCanCallUnmanagedCheck
	//     CompilationDomain::OnLinktimeCanSkipVerificationCheck
    static void GetPermissionInstance(OBJECTREF *perm, int index)
    { 
        WRAPPER_CONTRACT;
        _GetSharedPermissionInstance(perm, index);
    }

    static inline BOOL FullTrustCheckForLinkOrInheritanceDemand(Assembly *pAssembly);

	// Returns TRUE if an APTCA check is necessary
	//   Callers:
	//     CanAccess
    static BOOL IsUntrustedCallerCheckNeeded(MethodDesc *pCalleeMD, Assembly *pCallerAssem = NULL);

	// Perform the APTCA check
	//   Callers:
	//     CanAccess
	//     Security::CheckLinkDemandAgainstAppDomain
    static BOOL DoUntrustedCallerChecks(
        Assembly *pCaller, MethodDesc *pCalee, OBJECTREF *pThrowable, 
        BOOL fFullStackWalk);

#ifndef DACCESS_COMPILE
	// Calls PermissionSet.Demand
	//   Callers:
	//     CanAccess (ReflectionInvocation)
	//     Security::CheckLinkDemandAgainstAppDomain
    static void CheckNonCasDemand(OBJECTREF *prefDemand);
#endif // #ifndef DACCESS_COMPILE

	// Returns TRUE if the method is visible outside its assembly
	//   Callers:
	//     MethodTableBuilder::SetSecurityFlagsOnMethod
    static inline BOOL MethodIsVisibleOutsideItsAssembly(
                DWORD dwMethodAttr, DWORD dwClassAttr)
    {
        WRAPPER_CONTRACT;
        return (MethodIsVisibleOutsideItsAssembly(dwMethodAttr) &&
                ClassIsVisibleOutsideItsAssembly(dwClassAttr));
    }

    static BOOL TokenMightHaveDeclarations(IMDInternalImport *pInternalImport, mdToken token, CorDeclSecurity action);
    static DeclActionInfo *DetectDeclActions(MethodDesc *pMeth, DWORD dwDeclFlags);
    static void DetectDeclActionsOnToken(mdToken tk, DWORD dwDeclFlags, DWORD* pSetIndexes, IMDInternalImport *pInternalImport);
    static void InvokeLinktimeChecks(Assembly *pCaller,
                                     Module *pModule,
                                     mdToken token,
                                     BOOL *pfResult, 
                                     OBJECTREF *pThrowable);

    static inline BOOL MethodIsVisibleOutsideItsAssembly(DWORD dwMethodAttr)
    {
        WRAPPER_CONTRACT;
        return ( IsMdPublic(dwMethodAttr)    || 
                 IsMdFamORAssem(dwMethodAttr)||
                 IsMdFamily(dwMethodAttr) );
    }

    static inline BOOL ClassIsVisibleOutsideItsAssembly(DWORD dwClassAttr)
    {
        LEAF_CONTRACT;
        return ( IsTdPublic(dwClassAttr)      || 
                 IsTdNestedPublic(dwClassAttr)||
                 IsTdNestedFamily(dwClassAttr)||
                 IsTdNestedFamORAssem(dwClassAttr) );
    }

	// Returns an instance of a well-known permission.  (It caches them, so each permission is created only once.)
    static void _GetSharedPermissionInstance(OBJECTREF *perm, int index);

	// Returns an instance of a SecurityException with the message "This method doesn't allow partially trusted callers"
	//   Callers:
	//     DoUntrustedCallerChecks
    static void GetAPTCAException(OBJECTREF *pThrowable, Assembly *pCaller, MethodDesc *pCallee);
    static void GetHPException(OBJECTREF *pThrowable, EApiCategories protectedCategories, EApiCategories demandedCategories);

    // Add a declarative action and PermissionSet index to the linked list
    static void AddDeclAction(CorDeclSecurity action, DWORD dwClassSetIndex, DWORD dwMethodSetIndex, DeclActionInfo** ppActionList, MethodDesc *pMeth);

    // Helper for DoDeclarativeActions
    static void InvokeDeclarativeActions(MethodDesc *pMeth, DeclActionInfo *pActions, MethodSecurityDescriptor *pMSD);
    static void InvokeDeclarativeStackModifiers (MethodDesc *pMeth, DeclActionInfo *pActions, OBJECTREF * pSecObj);

    static bool BlobMightContainNonCasPermission(PBYTE pbPerm, ULONG cbPerm, DWORD dwAction);	
};




void __stdcall DoDeclarativeSecurity(MethodDesc *pMD, DeclActionInfo *pActions, InterceptorFrame* frame);
void __stdcall DoSpecialUnmanagedCodeDemand();

struct DeclActionInfo
{
    DWORD           dwDeclAction;   // This'll tell InvokeDeclarativeSecurity whats the action needed
    DWORD           dwSetIndex;     // The index of the cached permissionset on which to demand/assert/deny/blah
    DeclActionInfo *pNext;              // Next declarative action needed on this method, if any.

    static DeclActionInfo *Init(MethodDesc *pMD, DWORD dwAction, DWORD dwSetIndex);
};


#endif // __SECURITYDECLARATIVE_H__

