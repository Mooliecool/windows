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

#ifndef __security_h__
#define __security_h__

#include "securitypolicy.h"
#include "securityattributes.h"
#include "securitydeclarative.h"
#include "securityimperative.h"
#include "securitystackwalk.h"
#include "securitydescriptor.h"
#include "securitydescriptorappdomain.h"
#include "securitydescriptorassembly.h"
#include "securitymeta.h"
#include "securitytransparentassembly.h"

// Ultimately this will become the only interface through
// which the VM will access security code.
class Security
{
public:

    // ----------------------------------------
    // SecurityPolicy
    // ----------------------------------------


    // Init
    static void Start();
    static void Stop();
    static void SaveCache();

    // Policy
    static BOOL IsSecurityOff();
    static BOOL IsSecurityOn(); 
    static BOOL DoesFullTrustMeanFullTrust() ;
    static BOOL FullTrustOrBust();
    static void EarlyResolveThrowing(Assembly *pAssembly, AssemblySecurityDescriptor *pSecDesc) ;
    static DWORD QuickGetZone( const WCHAR* url ) ;
    static BOOL CanCallUnmanagedCode(Module *pModule);
    static BOOL CanAssert(Module *pModule);
    static inline BOOL IsExecutionPermissionCheckEnabled();
    static HRESULT HasREQ_SOAttribute(MethodDesc* pMethod);
    static void CreateSecurityException(__in_z char *szDemandClass, DWORD dwFlags, OBJECTREF* pThrowable);
    static DECLSPEC_NORETURN void ThrowSecurityException(__in_z char *szDemandClass, DWORD dwFlags);

    static inline BOOL CanTailCall(MethodDesc* pMD)
    {
        WRAPPER_CONTRACT; 
        return Security::CanSkipVerification(pMD);
    }

    static BOOL CanHaveRVA(FieldDesc* pFD, Assembly* pAssembly);

    static inline BOOL CanAccessNonVerifiableExplicitField(MethodDesc* pMD)
    {
        WRAPPER_CONTRACT
        // just check if the method can have unverifiable code
        return Security::CanSkipVerification(pMD);
    }

    // CanSkipVerification
    static BOOL CanSkipVerification(MethodDesc * pMethod, BOOL fCommit = TRUE);

    static BOOL CanSkipVerification(DomainAssembly * pAssembly, BOOL fCommit = TRUE)
    {
        WRAPPER_CONTRACT;
        if (IsSecurityOff())
            return TRUE;
        return SecurityPolicy::CanSkipVerification(pAssembly, fCommit);
    }

    static inline CorInfoCanSkipVerificationResult JITCanSkipVerification(DomainAssembly * pAssembly, BOOL fQuickCheckOnly) 
    {
        WRAPPER_CONTRACT;
        return SecurityTransparent::JITCanSkipVerification( pAssembly, fQuickCheckOnly);
    }

    static inline CorInfoCanSkipVerificationResult JITCanSkipVerification(MethodDesc * pMD, BOOL fQuickCheckOnly) 
    {
        WRAPPER_CONTRACT;
        return SecurityTransparent::JITCanSkipVerification( pMD, fQuickCheckOnly);
    }

    static inline BOOL JITCanAlwaySkipVerification(DomainAssembly * pAssembly) 
    {
        WRAPPER_CONTRACT; 
        return pAssembly->IsSystem();
    }

    static inline BOOL JITCanAlwaySkipVerification(MethodDesc * pMD) 
    {
        WRAPPER_CONTRACT; 
        return !SecurityTransparent::IsMethodTransparent(pMD) &&
                JITCanAlwaySkipVerification(pMD->GetAssembly()->GetDomainAssembly());
    }

    // ----------------------------------------
    // SecurityAttributes
    // ----------------------------------------

    static VOID Init();
    static VOID Shutdown();
    static HRESULT MapToHR(OBJECTREF ref);
    static OBJECTREF CreatePermissionSet(BOOL fTrusted);
    static void CopyByteArrayToEncoding(IN U1ARRAYREF* pArray, OUT PBYTE* pbData, OUT DWORD* cbData);
    static void CopyEncodingToByteArray(IN PBYTE   pbData, IN DWORD   cbData, IN OBJECTREF* pArray) ;



    // ----------------------------------------
    // SecurityDeclarative
    // ----------------------------------------
    static HRESULT GetDeclarationFlags(IMDInternalImport *pInternalImport, mdToken token, DWORD* pdwFlags, DWORD* pdwNullFlags, BOOL* fHasSuppressUnmanagedCodeAccessAttr = NULL);
    static BOOL MethodRequiresStub(UINT_PTR uSecurityStubToken) ;
    static void RetrieveLinktimeDemands(MethodDesc* pMD, OBJECTREF* pClassCas, OBJECTREF* pClassNonCas, OBJECTREF* pMethodCas, OBJECTREF* pMethodNonCas);
    static void CheckLinkDemandAgainstAppDomain(MethodDesc *pMD) ;

    static BOOL LinktimeCheckMethod(Assembly *pCaller, MethodDesc *pCallee, OBJECTREF *pThrowable, BOOL fIsFullStackWalk = FALSE) ;
    static void ClassInheritanceCheck(EEClass *pClass, EEClass *pParent);
    static void MethodInheritanceCheck(MethodDesc *pMethod, MethodDesc *pParent) ;
    static UINT_PTR GetSecurityStubToken(MethodDesc* pMD, DWORD dwDeclFlags) ;
    static Stub* CreateStub(SecurityStubLinker *pstublinker, MethodDesc* pMD, DWORD dwDeclFlags, UINT_PTR uSecurityStubToken, Stub* pRealStub, LPVOID pRealAddr) ;
    static void GetPermissionInstance(OBJECTREF *perm, int index);    
    static void DoDeclarativeActions(MethodDesc *pMD, DeclActionInfo *pActions, LPVOID pSecObj, MethodSecurityDescriptor *pMSD = NULL) ;
#ifndef DACCESS_COMPILE
    static void CheckNonCasDemand(OBJECTREF *prefDemand) ;
#endif // #ifndef DACCESS_COMPILE
    static inline BOOL MethodIsVisibleOutsideItsAssembly(DWORD dwMethodAttr, DWORD dwClassAttr) ;
    static void CheckBeforeAllocConsole(AppDomain* pDomain, Assembly* pAssembly);


    // ----------------------------------------
    // SecurityStackWalk
    // ----------------------------------------

    // other CAS Actions
    static void Demand(SecurityStackWalkType eType, OBJECTREF demand) ;
    static void DemandSet(SecurityStackWalkType eType, OBJECTREF demand) ;
    static void DemandSet(SecurityStackWalkType eType, DWORD dwSetIndex, DWORD dwAction) ;
    static void SpecialDemand(SecurityStackWalkType eType, DWORD whatPermission) ;


    // Compressed Stack

    static COMPRESSEDSTACKREF GetCSFromContextTransitionFrame(Frame *pFrame) ;
    static BOOL IsContextTransitionFrameWithCS(Frame *pFrame) ;



    FORCEINLINE static VOID IncrementSecurityPerfCounter() ;
    static BOOL IsSpecialRunFrame(MethodDesc *pMeth) ;
    static BOOL SkipAndFindFunctionInfo(INT32 i, MethodDesc** ppMD, OBJECTREF** ppOR, AppDomain **ppAppDomain = NULL);
    static BOOL SkipAndFindFunctionInfo(StackCrawlMark* pSCM, MethodDesc** ppMD, OBJECTREF** ppOR, AppDomain **ppAppDomain = NULL);

    // ----------------------------------------
    // SecurityDescriptor
    // ----------------------------------------

    // SecurityDescriptor
    static DWORD GetSpecialFlags(SecurityDescriptor* pSecDesc);

    // ApplicationSecurityDescriptor
#ifndef DACCESS_COMPILE
    static void SetEvidence(ApplicationSecurityDescriptor* pSecDesc, OBJECTREF evidence);
#endif

    static BOOL AllDomainsOnStackFullyTrusted() 
    { 
        WRAPPER_CONTRACT; 
        return (SecurityStackWalk::HasFlagsOrFullyTrusted(0));
    }

    static BOOL CanCallUnmanagedCode(ApplicationSecurityDescriptor* pASD);

    static void SetHostSecurityManagerFlags(ApplicationSecurityDescriptor* pASD, DWORD dwFlags);
    static void SetPolicyLevelFlag(ApplicationSecurityDescriptor* pASD);
    static void SetHomogeneousFlag(ApplicationSecurityDescriptor* pASD);
    static BOOL IsHomogeneous(ApplicationSecurityDescriptor* pASD);
    static BOOL CallHostSecurityManager(ApplicationSecurityDescriptor* pASD);
    static void SetDefaultAppDomainProperty(ApplicationSecurityDescriptor* pASD);
    static BOOL IsDefaultAppDomain(ApplicationSecurityDescriptor* pASD);
    static void FinishInitialization(ApplicationSecurityDescriptor* pASD);
    static OBJECTREF GetEvidence(ApplicationSecurityDescriptor* pASD);
    static void Resolve(ApplicationSecurityDescriptor* pASD);

    // AssemblySecurityDescriptor
#ifndef DACCESS_COMPILE
    static void SetRequestedPermissionSet(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF RequiredPermissionSet, OBJECTREF OptionalPermissionSet, OBJECTREF DeniedPermissionSet);
    static OBJECTREF GetGrantedPermissionSet(ApplicationSecurityDescriptor* pAsmSecDesc);
    static OBJECTREF GetGrantedPermissionSet(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF* RefusedPermissions);
    static void Resolve(AssemblySecurityDescriptor* pAsmSecDesc);
    static void SetGrantedPermissionSet(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF GrantedPermissionSet, OBJECTREF DeniedPermissionSet, DWORD dwSpecialFlags);
    static void SetAdditionalEvidence(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF evidence);
    static BOOL HasAdditionalEvidence(AssemblySecurityDescriptor* pAsmSecDesc);
    static OBJECTREF GetAdditionalEvidence(AssemblySecurityDescriptor* pAsmSecDesc) ;
    static void SetEvidence(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF evidence) ;
    static void AddToSharedDescriptor(AssemblySecurityDescriptor* pAsmSecDesc, SharedSecurityDescriptor *pSharedDesc);
#endif // #ifndef DACCESS_COMPILE

    static AppDomain* GetDomain(AssemblySecurityDescriptor* pAsmSecDesc);
    static BOOL CanCallUnmanagedCode(AssemblySecurityDescriptor* pAsmSecDesc);
    static BOOL CanSkipVerification(AssemblySecurityDescriptor* pSecDesc);
    static BOOL CanAssert(AssemblySecurityDescriptor* pSecDesc);
    static BOOL HasUnrestrictedUIPermission(AssemblySecurityDescriptor* pSecDesc);
    static SharedSecurityDescriptor *GetSharedSecDesc(AssemblySecurityDescriptor* pAsmSecDesc);
    static BOOL IsSigned(AssemblySecurityDescriptor* pAsmSecDesc);
    static BOOL IsSystem(AssemblySecurityDescriptor* pAsmSecDesc);
    static BOOL IsResolved(AssemblySecurityDescriptor* pAsmSecDesc);
    static HRESULT LoadSignature(AssemblySecurityDescriptor* pAsmSecDesc, COR_TRUST **ppSignature = NULL);
    static OBJECTREF GetRequestedPermissionSet(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF *pOptionalPermissionSet, OBJECTREF *pDeniedPermissionSet);
    static BOOL IsEvidenceComputed(AssemblySecurityDescriptor* pAsmSecDesc);
    static OBJECTREF GetEvidence(AssemblySecurityDescriptor* pAsmSecDesc);

    // SharedSecurityDescriptor
    static void Resolve(SharedSecurityDescriptor* pSSD, AssemblySecurityDescriptor *pSecDesc = NULL);
    static BOOL IsSystem(SharedSecurityDescriptor* pSSD);
    static BOOL IsResolved(SharedSecurityDescriptor* pSSD);
    static Assembly* GetAssembly(SharedSecurityDescriptor* pSSD);

    // PEFileSecurityDescriptor
    static BOOL AllowBindingRedirects(PEFileSecurityDescriptor* pSecDesc);
    static void Resolve(PEFileSecurityDescriptor* pSecDesc);

    // SecurityDescriptor
    static BOOL IsFullyTrusted(SecurityDescriptor* pSecDesc);
    static BOOL ZapIsFullyTrusted(SecurityDescriptor* pSecDesc);

    // Transparency checks
    static BOOL IsTransparentMethod(MethodDesc * pMD) 
    {
        WRAPPER_CONTRACT;
        return SecurityTransparent::IsMethodTransparent(pMD);
    }
    static CorInfoIsCallAllowedResult RequiresTransparentAssemblyChecks(MethodDesc* pCaller, MethodDesc* pCallee) ;
    static VOID EnforceTransparentAssemblyChecks(MethodDesc* pCallee, MethodDesc* pCaller);

    static inline BOOL CheckNonPublicCriticalAccess(MethodDesc* pCurrentMD,
        MethodDesc* pOptionalTargetMethod = NULL,
        FieldDesc* pOptionalTargetField = NULL,
        MethodTable * pOptionalTargetType = NULL)
    {
        return SecurityTransparent::CheckNonPublicCriticalAccess(pCurrentMD,
                    pOptionalTargetMethod, 
                    pOptionalTargetField, 
                    pOptionalTargetType);
    }

   static BOOL UseNoStubsApproach();
   static VOID InitializeCheckUseNoStubsApproach();

    // thjese methods should go into a seperate SecurityInternal class
    // security attributes
    static void EncodePermissionSet(IN OBJECTREF* pRef, OUT PBYTE* ppbData, OUT DWORD* pcbData);

    // declarative security
    static HRESULT GetDeclaredPermissions(IN IMDInternalImport *pInternalImport, IN mdToken token, IN CorDeclSecurity action, OUT OBJECTREF *pDeclaredPermissions, OUT DWORD* pdwSetIndex = NULL) ;

    // security enforcement
    static BOOL ContainsBuiltinCASPermsOnly(DWORD dwSetIndex, DWORD dwAction);
    static BOOL ContainsBuiltinCASPermsOnly(CORSEC_ATTRSET* pAttrSet);
    static BOOL IsUntrustedCallerCheckNeeded(MethodDesc *pCalleeMD, Assembly *pCallerAssem = NULL) ;
    static BOOL DoUntrustedCallerChecks(Assembly *pCaller, MethodDesc *pCalee, OBJECTREF *pThrowable, BOOL fFullStackWalk) ;
};

#include "security.inl"

#endif
