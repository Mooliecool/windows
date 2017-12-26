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

#ifndef _INL_SECURITY_
#define _INL_SECURITY_

// Init
inline void Security::Start() 
{
    WRAPPER_CONTRACT;
    SecurityPolicy::Start();
    InitializeCheckUseNoStubsApproach();
}

inline  void Security::Stop()
{
    WRAPPER_CONTRACT;
    SecurityPolicy::Stop();
}

inline void Security::SaveCache() 
{
    WRAPPER_CONTRACT;
    SecurityPolicy::SaveCache();
}

// ----------------------------------------
// SecurityPolicy
// ----------------------------------------
inline BOOL Security::IsSecurityOn()
{
    WRAPPER_CONTRACT;
    return SecurityPolicy::IsSecurityOn();
}

inline BOOL Security::IsSecurityOff()
{
    WRAPPER_CONTRACT;
    return SecurityPolicy::IsSecurityOff();
}

inline BOOL Security::DoesFullTrustMeanFullTrust() 
{

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;


    // 
    // 


    return !(g_pConfig->LegacyV1CASPolicy());
}

inline void Security::EarlyResolveThrowing(Assembly *pAssembly, AssemblySecurityDescriptor *pSecDesc) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return;
    SecurityPolicy::EarlyResolveThrowing(pAssembly, pSecDesc);
}

inline DWORD Security::QuickGetZone( const WCHAR* url ) 
{
    WRAPPER_CONTRACT;
    return SecurityPolicy::QuickGetZone(url);
}

inline BOOL Security::CanCallUnmanagedCode(Module *pModule) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return TRUE;
    return SecurityPolicy::CanCallUnmanagedCode(pModule); 
}

inline BOOL Security::CanAssert(Module *pModule) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return TRUE;
    SharedSecurityDescriptor *pSharedSecDesc = pModule->GetAssembly()->GetSharedSecurityDescriptor();
    if (pSharedSecDesc)
        return pSharedSecDesc->CanAssert();

    AssemblySecurityDescriptor *pSec = pModule->GetSecurityDescriptor();
    _ASSERTE(pSec);
    return pSec->CanAssert();
}

inline BOOL Security::IsExecutionPermissionCheckEnabled() 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return TRUE;
    return SecurityPolicy::IsExecutionPermissionCheckEnabled();
}

inline HRESULT Security::HasREQ_SOAttribute(MethodDesc* pMethod) 
{
    WRAPPER_CONTRACT;
    return SecurityPolicy::HasREQ_SOAttribute(pMethod);
}

inline void Security::CreateSecurityException(__in_z char *szDemandClass, DWORD dwFlags, OBJECTREF* pThrowable) 
{
    WRAPPER_CONTRACT;
    SecurityPolicy::CreateSecurityException(szDemandClass, dwFlags, pThrowable);
}

inline DECLSPEC_NORETURN void Security::ThrowSecurityException(__in_z char *szDemandClass, DWORD dwFlags) 
{
    WRAPPER_CONTRACT;
    SecurityPolicy::ThrowSecurityException(szDemandClass, dwFlags);
}

// ----------------------------------------
// SecurityAttributes
// ----------------------------------------

inline VOID Security::Init()
{
    WRAPPER_CONTRACT;
    return SecurityAttributes::Init();
}

inline VOID Security::Shutdown()
{
    WRAPPER_CONTRACT;
    return SecurityAttributes::Shutdown();
}

inline HRESULT Security::MapToHR(OBJECTREF ref)
{
    WRAPPER_CONTRACT;
    return SecurityAttributes::MapToHR(ref);
}

inline OBJECTREF Security::CreatePermissionSet(BOOL fTrusted) 
{
    WRAPPER_CONTRACT; 
    return SecurityAttributes::CreatePermissionSet(fTrusted); 
}

inline void Security::CopyByteArrayToEncoding(IN U1ARRAYREF* pArray, OUT PBYTE* pbData, OUT DWORD* cbData) 
{
    WRAPPER_CONTRACT; 
    SecurityAttributes::CopyByteArrayToEncoding(pArray, pbData, cbData); 
}

inline void Security::CopyEncodingToByteArray(IN PBYTE   pbData, IN DWORD   cbData, IN OBJECTREF* pArray) 
{
    WRAPPER_CONTRACT; 
    SecurityAttributes::CopyEncodingToByteArray(pbData, cbData, pArray); 
}

// ----------------------------------------
// SecurityDeclarative
// ----------------------------------------

inline HRESULT Security::GetDeclarationFlags(IMDInternalImport *pInternalImport, mdToken token, DWORD* pdwFlags, DWORD* pdwNullFlags, BOOL* fHasSuppressUnmanagedCodeAccessAttr) 
{
    WRAPPER_CONTRACT;
    return SecurityDeclarative::GetDeclarationFlags(pInternalImport, token, pdwFlags, pdwNullFlags, fHasSuppressUnmanagedCodeAccessAttr); 
}

inline BOOL Security::MethodRequiresStub(UINT_PTR uSecurityStubToken) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return FALSE;
    return SecurityDeclarative::MethodRequiresStub(uSecurityStubToken); 
}

inline void Security::RetrieveLinktimeDemands(MethodDesc* pMD, OBJECTREF* pClassCas, OBJECTREF* pClassNonCas, OBJECTREF* pMethodCas, OBJECTREF* pMethodNonCas) 
{
    WRAPPER_CONTRACT; 
    SecurityDeclarative::RetrieveLinktimeDemands(pMD, pClassCas, pClassNonCas, pMethodCas, pMethodNonCas); 
}

inline void Security::CheckLinkDemandAgainstAppDomain(MethodDesc *pMD) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return;
    SecurityDeclarative::CheckLinkDemandAgainstAppDomain(pMD); 
}

inline BOOL Security::LinktimeCheckMethod(Assembly *pCaller, MethodDesc *pCallee, OBJECTREF *pThrowable, BOOL fIsFullStackWalk ) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return TRUE;
    return SecurityDeclarative::LinktimeCheckMethod(pCaller, pCallee, pThrowable); 
}

inline void Security::ClassInheritanceCheck(EEClass *pClass, EEClass *pParent) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return;
    SecurityDeclarative::ClassInheritanceCheck(pClass, pParent); 
}

inline void Security::MethodInheritanceCheck(MethodDesc *pMethod, MethodDesc *pParent) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return;
    SecurityDeclarative::MethodInheritanceCheck(pMethod, pParent); 
}

inline UINT_PTR Security::GetSecurityStubToken(MethodDesc* pMD, DWORD dwDeclFlags) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return 0;
    return SecurityDeclarative::GetSecurityStubToken(pMD, dwDeclFlags); 
}

inline Stub* Security::CreateStub(SecurityStubLinker *pstublinker, MethodDesc* pMD, DWORD dwDeclFlags, UINT_PTR uSecurityStubToken, Stub* pRealStub, LPVOID pRealAddr) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return NULL;
    return SecurityDeclarative::CreateStub(pstublinker, pMD, dwDeclFlags, uSecurityStubToken, pRealStub, pRealAddr); 
}

inline void Security::GetPermissionInstance(OBJECTREF *perm, int index) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return;
    SecurityDeclarative::GetPermissionInstance(perm, index); 
}

inline void Security::DoDeclarativeActions(MethodDesc *pMD, DeclActionInfo *pActions, LPVOID pSecObj, MethodSecurityDescriptor *pMSD) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return;
    SecurityDeclarative::DoDeclarativeActions(pMD, pActions, pSecObj, pMSD); 
}

#ifndef DACCESS_COMPILE
inline void Security::CheckNonCasDemand(OBJECTREF *prefDemand) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return ;
    SecurityDeclarative::CheckNonCasDemand(prefDemand); 
}
#endif // #ifndef DACCESS_COMPILE

inline BOOL Security::MethodIsVisibleOutsideItsAssembly(DWORD dwMethodAttr, DWORD dwClassAttr) 
{
    WRAPPER_CONTRACT; 
    return SecurityDeclarative::MethodIsVisibleOutsideItsAssembly(dwMethodAttr, dwClassAttr); 
}

inline void Security::CheckBeforeAllocConsole(AppDomain* pDomain, Assembly* pAssembly) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return;
    SecurityRuntime::CheckBeforeAllocConsole(pDomain, pAssembly); 
}

// ----------------------------------------
// SecurityStackWalk
// ----------------------------------------

// other CAS Actions
inline void Security::Demand(SecurityStackWalkType eType, OBJECTREF demand) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return;
    SecurityStackWalk::Demand(eType, demand); 
}

inline void Security::DemandSet(SecurityStackWalkType eType, OBJECTREF demand) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return ;
    SecurityStackWalk::DemandSet(eType, demand); 
}

inline void Security::DemandSet(SecurityStackWalkType eType, DWORD dwSetIndex, DWORD dwAction) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return;
    SecurityStackWalk::DemandSet(eType, dwSetIndex, dwAction); 
}

inline void Security::SpecialDemand(SecurityStackWalkType eType, DWORD whatPermission) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return;
    SecurityStackWalk::SpecialDemand(eType, whatPermission); 
}

// Compressed Stack

inline COMPRESSEDSTACKREF Security::GetCSFromContextTransitionFrame(Frame *pFrame) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return NULL;
    return SecurityStackWalk::GetCSFromContextTransitionFrame(pFrame); 
}

inline BOOL Security::IsContextTransitionFrameWithCS(Frame *pFrame) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return FALSE;
    return SecurityStackWalk::IsContextTransitionFrameWithCS(pFrame); 
}


FORCEINLINE  VOID Security::IncrementSecurityPerfCounter() 
{
    WRAPPER_CONTRACT; 
    SecurityStackWalk::IncrementSecurityPerfCounter(); 
}

inline BOOL Security::IsSpecialRunFrame(MethodDesc *pMeth) 
{
    WRAPPER_CONTRACT;
    return SecurityStackWalk::IsSpecialRunFrame(pMeth);
}

inline BOOL Security::SkipAndFindFunctionInfo(INT32 i, MethodDesc** ppMD, OBJECTREF** ppOR, AppDomain **ppAppDomain ) 
{
    WRAPPER_CONTRACT; 
    return SecurityStackWalk::SkipAndFindFunctionInfo(i, ppMD, ppOR, ppAppDomain); 
}

inline BOOL Security::SkipAndFindFunctionInfo(StackCrawlMark* pSCM, MethodDesc** ppMD, OBJECTREF** ppOR, AppDomain **ppAppDomain ) 
{
    WRAPPER_CONTRACT; 
    return SecurityStackWalk::SkipAndFindFunctionInfo(pSCM, ppMD, ppOR, ppAppDomain); 
}

// SecurityDescriptor
inline DWORD Security::GetSpecialFlags(SecurityDescriptor* pSecDesc)
{
    WRAPPER_CONTRACT;
    return pSecDesc->GetSpecialFlags();
}

// ApplicationSecurityDescriptor
#ifndef DACCESS_COMPILE
inline void Security::SetEvidence(ApplicationSecurityDescriptor* pSecDesc, OBJECTREF evidence)
{
    WRAPPER_CONTRACT;
    pSecDesc->SetEvidence(evidence);
}
#endif

inline void Security::SetHostSecurityManagerFlags(ApplicationSecurityDescriptor* pASD, DWORD dwFlags)
    {WRAPPER_CONTRACT; pASD->SetHostSecurityManagerFlags(dwFlags);}
inline void Security::SetPolicyLevelFlag(ApplicationSecurityDescriptor* pASD)
    {WRAPPER_CONTRACT; pASD->SetPolicyLevelFlag();}
inline void Security::SetHomogeneousFlag(ApplicationSecurityDescriptor* pASD)
    {WRAPPER_CONTRACT; pASD->SetHomogeneousFlag();}
inline BOOL Security::IsHomogeneous(ApplicationSecurityDescriptor* pASD)
    {WRAPPER_CONTRACT; return pASD->IsHomogeneous();}
inline BOOL Security::CallHostSecurityManager(ApplicationSecurityDescriptor* pASD)
    {WRAPPER_CONTRACT; return pASD->CallHostSecurityManager();}
inline void Security::SetDefaultAppDomainProperty(ApplicationSecurityDescriptor* pASD)
    {WRAPPER_CONTRACT; pASD->SetDefaultAppDomain();}
inline BOOL Security::IsDefaultAppDomain(ApplicationSecurityDescriptor* pASD)
    {WRAPPER_CONTRACT; return pASD->IsDefaultAppDomain();}
inline OBJECTREF Security::GetEvidence(ApplicationSecurityDescriptor* pASD)
    {WRAPPER_CONTRACT; return pASD->GetEvidence();}

inline void Security::Resolve(ApplicationSecurityDescriptor* pASD) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return;
    pASD->Resolve();
}

inline void Security::FinishInitialization(ApplicationSecurityDescriptor* pASD)
{
    WRAPPER_CONTRACT;
    // Resolve the AppDomain security descriptor.
    if (IsSecurityOn())
        pASD->Resolve();

    // Reset the initialization in-progress flag.
    pASD->ResetInitializationInProgress();

    // Initialize the PLS with the grant set of the AppDomain
    if (IsSecurityOn())
        pASD->InitializePLS();
}

inline BOOL Security::CanCallUnmanagedCode(ApplicationSecurityDescriptor* pASD) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return TRUE;
    return pASD->CanCallUnmanagedCode();
}

// AssemblySecurityDescriptor
#ifndef DACCESS_COMPILE
inline void Security::SetRequestedPermissionSet(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF RequiredPermissionSet, OBJECTREF OptionalPermissionSet, OBJECTREF DeniedPermissionSet) 
{
    WRAPPER_CONTRACT;
    pAsmSecDesc->SetRequestedPermissionSet(RequiredPermissionSet, OptionalPermissionSet, DeniedPermissionSet); 
}

inline OBJECTREF Security::GetGrantedPermissionSet(ApplicationSecurityDescriptor* pAsmSecDesc) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return NULL;
    return pAsmSecDesc->GetGrantedPermissionSet(NULL); 
}

inline OBJECTREF Security::GetGrantedPermissionSet(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF* RefusedPermissions) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return NULL;
    return pAsmSecDesc->GetGrantedPermissionSet(RefusedPermissions);
}

inline void Security::Resolve(AssemblySecurityDescriptor* pAsmSecDesc) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return;
    pAsmSecDesc->Resolve();
}

inline void Security::SetGrantedPermissionSet(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF GrantedPermissionSet, OBJECTREF DeniedPermissionSet, DWORD dwSpecialFlags) 
{
    WRAPPER_CONTRACT;
    pAsmSecDesc->SetGrantedPermissionSet(GrantedPermissionSet, DeniedPermissionSet, dwSpecialFlags);
}

inline void Security::SetAdditionalEvidence(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF evidence) 
{
    WRAPPER_CONTRACT; 
    pAsmSecDesc->SetAdditionalEvidence(evidence); 
}

inline BOOL Security::HasAdditionalEvidence(AssemblySecurityDescriptor* pAsmSecDesc) 
{
    WRAPPER_CONTRACT;
    return pAsmSecDesc->HasAdditionalEvidence(); 
}

inline OBJECTREF Security::GetAdditionalEvidence(AssemblySecurityDescriptor* pAsmSecDesc) 
{
    WRAPPER_CONTRACT;
    return pAsmSecDesc->GetAdditionalEvidence(); 
}

inline void Security::SetEvidence(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF evidence) 
{
    WRAPPER_CONTRACT;
    pAsmSecDesc->SetEvidence(evidence);
}

inline void Security::AddToSharedDescriptor(AssemblySecurityDescriptor* pAsmSecDesc, SharedSecurityDescriptor *pSharedDesc) 
{
    WRAPPER_CONTRACT;  
    pAsmSecDesc->AddToSharedDescriptor(pSharedDesc);
}

#endif // #ifndef DACCESS_COMPILE

inline AppDomain* Security::GetDomain(AssemblySecurityDescriptor* pAsmSecDesc) 
{
    WRAPPER_CONTRACT; 
    return pAsmSecDesc->GetDomain();
}

inline BOOL Security::CanCallUnmanagedCode(AssemblySecurityDescriptor* pAsmSecDesc) 
{
    WRAPPER_CONTRACT; 
    if (IsSecurityOff()) return TRUE;
    return pAsmSecDesc->CanCallUnmanagedCode();
}

inline BOOL Security::CanSkipVerification(AssemblySecurityDescriptor* pSecDesc) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return TRUE;
    return pSecDesc->CanSkipVerification();
}

inline BOOL Security::CanAssert(AssemblySecurityDescriptor* pSecDesc) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return TRUE;
    return pSecDesc->CanAssert();
}

inline BOOL Security::HasUnrestrictedUIPermission(AssemblySecurityDescriptor* pSecDesc) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return TRUE;
    return pSecDesc->HasUnrestrictedUIPermission();
}

inline SharedSecurityDescriptor *Security::GetSharedSecDesc(AssemblySecurityDescriptor* pAsmSecDesc) 
{
    WRAPPER_CONTRACT;
    return pAsmSecDesc->GetSharedSecDesc();
}

inline BOOL Security::IsSigned(AssemblySecurityDescriptor* pAsmSecDesc)
{
    WRAPPER_CONTRACT;
    return pAsmSecDesc->IsSigned();
}

inline BOOL Security::IsSystem(AssemblySecurityDescriptor* pAsmSecDesc)
{
    WRAPPER_CONTRACT;
    return pAsmSecDesc->IsSystem();
}

inline BOOL Security::IsResolved(AssemblySecurityDescriptor* pAsmSecDesc)
{
    WRAPPER_CONTRACT;
    return pAsmSecDesc->IsResolved();
}

inline HRESULT Security::LoadSignature(AssemblySecurityDescriptor* pAsmSecDesc, COR_TRUST **ppSignature ) 
{
    WRAPPER_CONTRACT;
    return pAsmSecDesc->LoadSignature(ppSignature); 
}

inline OBJECTREF Security::GetRequestedPermissionSet(AssemblySecurityDescriptor* pAsmSecDesc, OBJECTREF *pOptionalPermissionSet, OBJECTREF *pDeniedPermissionSet) 
{
    WRAPPER_CONTRACT;
    return pAsmSecDesc->GetRequestedPermissionSet(pOptionalPermissionSet, pDeniedPermissionSet); 
}

inline BOOL Security::IsEvidenceComputed(AssemblySecurityDescriptor* pAsmSecDesc) 
{
    WRAPPER_CONTRACT;
    return pAsmSecDesc->IsEvidenceComputed();
}

inline OBJECTREF Security::GetEvidence(AssemblySecurityDescriptor* pAsmSecDesc) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return NULL;
    return pAsmSecDesc->GetEvidence();
}

// SharedSecurityDescriptor

inline void Security::Resolve(SharedSecurityDescriptor* pSSD, AssemblySecurityDescriptor *pSecDesc ) 
{
    WRAPPER_CONTRACT;
    pSSD->Resolve(pSecDesc); 
}

inline BOOL Security::IsSystem(SharedSecurityDescriptor* pSSD) 
{
    WRAPPER_CONTRACT;
    return pSSD->IsSystem(); 
}

inline BOOL Security::IsResolved(SharedSecurityDescriptor* pSSD) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return TRUE;
    return pSSD->IsResolved();
}

inline Assembly* Security::GetAssembly(SharedSecurityDescriptor* pSSD) 
{
    WRAPPER_CONTRACT; 
    return pSSD->GetAssembly();
}

// PEFileSecurityDescriptor
inline BOOL Security::AllowBindingRedirects(PEFileSecurityDescriptor* pSecDesc) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return TRUE;
    return pSecDesc->AllowBindingRedirects();
}

inline void Security::Resolve(PEFileSecurityDescriptor* pSecDesc) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return;
    return pSecDesc->Resolve();
}

// SecurityDescriptor
inline BOOL Security::IsFullyTrusted(SecurityDescriptor* pSecDesc) 
{
    if (IsSecurityOff()) return TRUE;
    BOOL isFullyTrusted = FALSE;

    #ifndef DACCESS_COMPILE
    isFullyTrusted = pSecDesc->IsFullyTrusted(); 
    #endif

    return isFullyTrusted;
}

inline BOOL Security::ZapIsFullyTrusted(SecurityDescriptor* pSecDesc)
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return TRUE;
    return pSecDesc->QuickIsFullyTrusted();
}

// Transparency checks
inline CorInfoIsCallAllowedResult Security::RequiresTransparentAssemblyChecks(MethodDesc* pCaller, MethodDesc* pCallee) 
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return CORINFO_CALL_ALLOWED;
    return SecurityTransparent::RequiresTransparentAssemblyChecks(pCaller, pCallee);
}

inline VOID Security::EnforceTransparentAssemblyChecks( MethodDesc* pCallee, MethodDesc* pCaller)
{
    WRAPPER_CONTRACT;
    if (IsSecurityOff()) return;
    SecurityTransparent::EnforceTransparentAssemblyChecks( pCallee, pCaller);
}

inline BOOL Security::CanHaveRVA(FieldDesc* pFD, Assembly* pAssembly)
{
    WRAPPER_CONTRACT; 
    BOOL fCanSkipVerification = Security::CanSkipVerification(pAssembly->GetDomainAssembly(), TRUE); 
    /*if (fCanSkipVerification)
    {
        // check for transparency
        if (SecurityTransparent::IsFieldTransparent(pFD))
            return FALSE;
    }*/
   return fCanSkipVerification;
}

inline BOOL Security::CanSkipVerification(MethodDesc * pMD, BOOL fCommit) 
{
    WRAPPER_CONTRACT;
    BOOL fCanSkipVerification = Security::CanSkipVerification(pMD->GetAssembly()->GetDomainAssembly(), fCommit); 
    if (fCanSkipVerification)
    {
        // check for transparency
        if (SecurityTransparent::IsMethodTransparent(pMD))
            return FALSE;
    }
   return fCanSkipVerification;
}

// SECURITY INTERNAL FUNCTIONS

// This class should be used only from within the security infrastructure,
// rule of thumb, if your code is not a file whose name starts with security*.*, 
// then don't call any functions here, call only fuctions in the above security class

/*class SecurityTransparent;
class SecurityInternal 
{
    friend class SecurityDeclarative;
    friend class SecurityAttributes;
    friend class SecurityTransparent;

    private:
        // security attributes
        static void EncodePermissionSet(IN OBJECTREF* pRef, OUT PBYTE* ppbData, OUT DWORD* pcbData);     
        // declarative security
        static HRESULT GetDeclaredPermissions(IN IMDInternalImport *pInternalImport, IN mdToken token, IN CorDeclSecurity action, OUT OBJECTREF *pDeclaredPermissions, OUT DWORD* pdwSetIndex = NULL, OUT SpecialPermissionSetFlag* pSpecialFlags = NULL, BOOL fCreate = TRUE) ;

        // security enforcement
        static BOOL CanUnrestrictedOverride(DWORD dwSetIndex);
        static BOOL CanUnrestrictedOverride(CORSEC_ATTRSET* pAttrSet);
        static BOOL IsUntrustedCallerCheckNeeded(MethodDesc *pCalleeMD, Assembly *pCallerAssem = NULL) ;
        static BOOL DoUntrustedCallerChecks(Assembly *pCaller, MethodDesc *pCalee, OBJECTREF *pThrowable, BOOL fFullStackWalk) ;
};*/

inline BOOL Security::ContainsBuiltinCASPermsOnly(DWORD dwSetIndex, DWORD dwAction) 
{ 
    WRAPPER_CONTRACT; 
    if (Security::IsSecurityOff()) return TRUE;
    return SecurityAttributes::ContainsBuiltinCASPermsOnly(dwSetIndex, dwAction); 
}

inline    BOOL Security::ContainsBuiltinCASPermsOnly(CORSEC_ATTRSET* pAttrSet) 
{ 
    WRAPPER_CONTRACT; 
    if (Security::IsSecurityOff()) return TRUE;
    return SecurityAttributes::ContainsBuiltinCASPermsOnly(pAttrSet); 
}

inline BOOL Security::IsUntrustedCallerCheckNeeded(MethodDesc *pCalleeMD, Assembly *pCallerAssem)
{ 
    WRAPPER_CONTRACT; 
    if (Security::IsSecurityOff()) return FALSE;
    return SecurityDeclarative::IsUntrustedCallerCheckNeeded(pCalleeMD, pCallerAssem); 
}
 
inline BOOL Security::DoUntrustedCallerChecks(Assembly *pCaller, MethodDesc *pCalee, OBJECTREF *pThrowable, BOOL fFullStackWalk) 
{
    WRAPPER_CONTRACT;
    if (Security::IsSecurityOff()) return TRUE;
    return SecurityDeclarative::DoUntrustedCallerChecks(pCaller, pCalee, pThrowable, fFullStackWalk); 
}

inline void Security::EncodePermissionSet(IN OBJECTREF* pRef, OUT PBYTE* ppbData, OUT DWORD* pcbData) 
{
    WRAPPER_CONTRACT;
    SecurityAttributes::EncodePermissionSet(pRef, ppbData, pcbData); 
}

inline HRESULT Security::GetDeclaredPermissions(IN IMDInternalImport *pInternalImport, IN mdToken token, IN CorDeclSecurity action, OUT OBJECTREF *pDeclaredPermissions, OUT DWORD* pdwSetIndex ) 
{
    WRAPPER_CONTRACT;
    return SecurityAttributes::GetDeclaredPermissions(pInternalImport, token, action, pDeclaredPermissions, pdwSetIndex); 
}

  extern BOOL g_fUseNoStubsChecked;
  extern BOOL g_fUseNoStubs;
  inline  BOOL Security::UseNoStubsApproach()
  {
    return FALSE; // Hard-code FJIT/Rotor to use stubs
  }

  inline VOID Security::InitializeCheckUseNoStubsApproach()
  {
  	if (!g_fUseNoStubsChecked)
  	{
        // Default is to use no stubs
        if (REGUTIL::GetConfigDWORD(L"UseStubsForSecurity", 0) != 0)
            g_fUseNoStubs = FALSE;
        g_fUseNoStubsChecked = TRUE;
  	}
  }

  inline BOOL SecurityDeclarative::FullTrustCheckForLinkOrInheritanceDemand(Assembly *pAssembly)
  {
      WRAPPER_CONTRACT;
#ifndef DACCESS_COMPILE      
      AssemblySecurityDescriptor* pSecDesc = pAssembly->GetSecurityDescriptor();
      if (pSecDesc->IsSystem())
          return TRUE;
  
      if (Security::DoesFullTrustMeanFullTrust() && pSecDesc->IsFullyTrusted())
          return TRUE;
#endif  
      return FALSE;
      
  }

  
  // Returns true if everyone is fully trusted or has the indicated flags
  FORCEINLINE BOOL SecurityStackWalk::HasFlagsOrFullyTrustedIgnoreMode (DWORD flags) {
      CONTRACTL
      {
          NOTHROW;
          GC_NOTRIGGER;
          MODE_ANY;
          SO_TOLERANT;
      }
      CONTRACTL_END;
  
      // either the desired flag (often 0) or fully trusted will do
      flags |= (1<<SECURITY_FULL_TRUST);
  
      // in order for us to use the threadwide state it has to be the case that there have been no
      // overrides since the evaluation (e.g. no denies)  We keep the state up-to-date by updating 
      // it whenever a new AppDomainStackEntry is pushed on the AppDomainStack attached to the thread.
      // When we evaluate the demand, we always intersect the current thread state with the AppDomain
      // wide flags, which are updated anytime a new Assembly is loaded into that domain.
      //
      // note if the flag is clear we still might be able to satisfy the demand if we do the full 
      // stackwalk.
      //
      // this code is very perf sensitive, do not make changes here without running 
      // a lot of interop and declarative security benchmarks
      //
      // it's important that we be able to do these checks without having to touch objects
      // other than the thread itself -- that's where a big part of the speed comes from
      // L1 cache misses are at a premium on this code path -- never mind L2...
      // main memory is right out :)
  
      Thread* pThread = GetThread();
      return ((pThread->GetOverridesCount() == 0) &&
               pThread->CheckThreadWideSpecialFlag(flags) &&
               pThread->GetDomain()->GetSecurityDescriptor()->CheckDomainWideSpecialFlag(flags));
  }
  
  // Returns true if everyone is fully trusted or has the indicated flags AND we're not in legacy CAS mode
  FORCEINLINE BOOL SecurityStackWalk::HasFlagsOrFullyTrusted (DWORD flags) {
      CONTRACTL
      {
          NOTHROW;
          GC_NOTRIGGER;
          MODE_ANY;
          SO_TOLERANT;
      }
      CONTRACTL_END;
      return (Security::DoesFullTrustMeanFullTrust() && HasFlagsOrFullyTrustedIgnoreMode(flags));
  
  }
  
  FORCEINLINE BOOL SecurityStackWalk::QuickCheckForAllDemands(DWORD flags)
  {
      CONTRACTL {
          NOTHROW;
          GC_NOTRIGGER;
          MODE_ANY;
          SO_TOLERANT;
      } CONTRACTL_END;
  
      if (SecurityPolicy::IsSecurityOn()) 
      {
          return (SecurityStackWalk::HasFlagsOrFullyTrusted(flags));
      }
      return (TRUE);
  
  }
#ifndef DACCESS_COMPILE  
  inline void StoreObjectInLazyHandle(OBJECTHANDLE& handle, OBJECTREF ref, AppDomain* app_dom)
  {
      WRAPPER_CONTRACT;
      if (handle == NULL) {
          // Atomically create a handle and store it
          OBJECTHANDLE tmpHandle = app_dom->CreateHandle(NULL);
          if (FastInterlockCompareExchangePointer((LPVOID *)&handle, tmpHandle, NULL) != NULL) {
              // Another thread snuck in and created the handle - destroy the one we created here
              DestroyHandle(tmpHandle);
          }
      }
      StoreObjectInHandle(handle, ref);
  }
#endif
    FORCEINLINE BOOL AssemblySecurityDescriptor::IsSystem()
    {
        WRAPPER_CONTRACT;
        return m_pAssem->GetFile()->IsSystem();
    }
    FORCEINLINE BOOL SharedSecurityDescriptor::IsSystem()
    { 
        LEAF_CONTRACT;
        return m_pAssembly->IsSystem();
    }

#endif
