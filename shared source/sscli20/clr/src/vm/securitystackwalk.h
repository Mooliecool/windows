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
#ifndef __SECURITYSTACKWALK_H__
#define __SECURITYSTACKWALK_H__

#include "common.h"

#include "object.h"
#include "util.hpp"
#include "fcall.h"
#include "perfcounters.h"
#include "securitydescriptor.h"
#include "holder.h"
#include "appdomainhelper.h"
#include "securitydescriptorappdomain.h"

class Security;
class SecurityDeclarative;
class DomainCompressedStack;
class SecurityPolicy;
class ApplicationSecurityDescriptor;
class DemandStackWalk;
class NewCompressedStack;
class CountOverridesStackWalk;
class AssertStackWalk;
struct TokenDeclActionInfo;

enum SecurityStackWalkType
{
    SSWT_DECLARATIVE_DEMAND = 1,
    SSWT_IMPERATIVE_DEMAND = 2,
    SSWT_DEMAND_FROM_NATIVE = 3,
    SSWT_IMPERATIVE_ASSERT = 4,
    SSWT_DENY_OR_PERMITONLY = 5,
    SSWT_LATEBOUND_LINKDEMAND = 6,
    SSWT_COUNT_OVERRIDES = 7,
    SSWT_GET_ZONE_AND_URL = 8,
};


//-----------------------------------------------------------
// SecurityStackWalk implements all the native methods
// for the interpreted System/Security/SecurityEngine.
//-----------------------------------------------------------
class SecurityStackWalk
{
friend class Security;
friend class SecurityDeclarative;
friend class DomainCompressedStack;
friend class SecurityPolicy;
friend class ApplicationSecurityDescriptor;
friend class DemandStackWalk;
friend class NewCompressedStack;
friend class CountOverridesStackWalk;
friend class AssertStackWalk;
friend class SecurityRuntime;
friend struct TokenDeclActionInfo;
friend struct MethodSecurityDescriptor;
friend struct TypeSecurityDescriptor;
friend void __stdcall DoSpecialUnmanagedCodeDemand();

protected:

    //-----------------------------------------------------------
    // Cached class and method pointers.
    //-----------------------------------------------------------
    typedef struct _SEData
    {
        BOOL        fInitialized;
        MethodTable    *pSecurityEngine;
        MethodTable    *pSecurityRuntime;
        MethodTable    *pPermListSet;
        MethodTable    *pPermSet;
        MethodTable    *pIUnrestrictedPermission;
        MethodTable    *pFrameSecurityDescriptor;
        MethodDesc     *pMethPLSDemand;
        MethodDesc     *pMethPLSDemandSet;
        MethodDesc     *pMethEC_Run;
        MethodDesc     *pMethSC_Run;
        MethodDesc     *pMethCS_Run;
        MetaSig        *pSigPLSDemand;
        MetaSig        *pSigPLSDemandSet;
    } SEData;

    SecurityStackWalkType m_eStackWalkType;
    DWORD m_dwFlags;

public:
    MarshalCache m_objects;

    SecurityStackWalk(SecurityStackWalkType eType, DWORD flags)
    {
        m_eStackWalkType = eType;
        m_dwFlags = flags;
    }

    // ----------------------------------------------------
    // FCalls
    // ----------------------------------------------------

    // FCall wrapper for CheckInternal
    static FCDECL3(void, Check, Object* permOrPermSetUNSAFE, StackCrawlMark* stackMark, CLR_BOOL isPermSet);
    static FC_INNER_RET CheckFramed(Object* permOrPermSetUNSAFE, StackCrawlMark* stackMark, CLR_BOOL isPermSet);


    // FCALL wrapper for quickcheckforalldemands
    static FCDECL0(FC_BOOL_RET, FCallQuickCheckForAllDemands);
    static FCDECL0(FC_BOOL_RET, FCallAllDomainsHomogeneousWithNoStackModifiers);




    static FCDECL3(void, GetZoneAndOrigin, Object* pZoneListUNSAFE, Object* pOriginListUNSAFE, StackCrawlMark* stackMark);

    // Do an imperative assert.  (Check the for the permission and return the SecurityObject for the first frame)
    static FCDECL5(Object*, CheckNReturnSO, Object* permTokenUNSAFE, Object* permUNSAFE, StackCrawlMark* stackMark, INT32 unrestrictedOverride, INT32 create);

    // Reterns a reference to the grant set of the security descriptor that you pass in
    static FCDECL3(void, GetGrantedPermissionSet, void* pSecDesc, OBJECTREF* ppGranted, OBJECTREF* ppDenied);

    // Do a demand for a special permission type
    static FCDECL2(void, FcallSpecialDemand, DWORD whatPermission, StackCrawlMark* stackMark);




private:
    // ----------------------------------------------------
    // Checks
    // ----------------------------------------------------

    // Methods for checking grant and refused sets
    void CheckPermissionAgainstGrants(OBJECTREF refCS, OBJECTREF refGrants, OBJECTREF refRefused, AppDomain *pDomain, MethodDesc* pMethod, Assembly* pAssembly);
    void CheckSetAgainstGrants(OBJECTREF refCS, OBJECTREF refGrants, OBJECTREF refRefused, AppDomain *pDomain, MethodDesc* pMethod, Assembly* pAssembly);
    void GetZoneAndOriginGrants(OBJECTREF refCS, OBJECTREF refGrants, OBJECTREF refRefused, AppDomain *pDomain, MethodDesc* pMethod, Assembly* pAssembly);

    // Methods for checking stack modifiers
    BOOL CheckPermissionAgainstFrameData(OBJECTREF refFrameData, AppDomain* pDomain, MethodDesc* pMethod);
    BOOL CheckSetAgainstFrameData(OBJECTREF refFrameData, AppDomain* pDomain, MethodDesc* pMethod);

    FORCEINLINE static BOOL QuickCheckForAllDemands(DWORD flags);





    
    // ----------------------------------------------------
    // CAS Actions
    // ----------------------------------------------------

    // Native version of CodeAccessPermission.Demand()
    //   Callers:
    //     <Currently unused>
    static void Demand(SecurityStackWalkType eType, OBJECTREF demand);

    // Native version of PermissionSet.Demand()
    //   Callers:
    //     CanAccess (ReflectionInvocation)
    //     ReflectionSerialization::GetSafeUninitializedObject
    //     SecurityDeclarative::DoUntrustedCallerChecks
    static void DemandSet(SecurityStackWalkType eType, OBJECTREF demand);

    // Native version of PermissionSet.Demand() that delays instantiating the PermissionSet object
    //   Callers:
    //     InvokeDeclarativeActions
    static void DemandSet(SecurityStackWalkType eType, DWORD dwSetIndex, DWORD dwAction);

    // Optimized demand for a well-known permission
    //   Callers:
    //     SecurityDeclarative::DoDeclarativeActions
    //     Security::CheckLinkDemandAgainstAppDomain
    //     TryDemand (ReflectionInvocation)
    //     CanAccess (ReflectionInvocation)
    //     ReflectionInvocation::CanValueSpecialCast
    //     RuntimeTypeHandle::CreateInstance
    //     RuntimeMethodHandle::InvokeMethod_Internal
    //     InvokeArrayConstructor (ReflectionInvocation)
    //     ReflectionInvocation::InvokeDispMethod
    //     COMArrayInfo::CreateInstance
    //     COMArrayInfo::CreateInstanceEx
    //     COMDelegate::BindToMethodName
    //     InvokeUtil::CheckArg
    //     InvokeUtil::ValidField
    //     RefSecContext::CallerHasPerm
    //     MngStdItfBase::ForwardCallToManagedView
    //     ObjectClone::Clone
    static void SpecialDemand(SecurityStackWalkType eType, DWORD whatPermission, StackCrawlMark* stackMark = NULL);







    // ----------------------------------------------------
    // Compressed Stack
    // ----------------------------------------------------
public:
    static FCDECL1(Object*, EcallGetDelayedCompressedStack, StackCrawlMark* stackMark);
    static FCDECL1(VOID, FcallDestroyDelayedCompressedStack, void *compressedStack);
    static COMPRESSEDSTACKREF GetCSFromContextTransitionFrame(Frame *pFrame);
    static BOOL IsContextTransitionFrameWithCS(Frame *pFrame)
    {
        return (GetCSFromContextTransitionFrame(pFrame) != NULL);
    }

    FORCEINLINE static BOOL HasFlagsOrFullyTrustedIgnoreMode (DWORD flags);   
    FORCEINLINE static BOOL HasFlagsOrFullyTrusted (DWORD flags);     
private:


    FORCEINLINE static VOID IncrementSecurityPerfCounter()
    {
        CONTRACTL {
            MODE_ANY;
            GC_NOTRIGGER;
            NOTHROW;
            SO_TOLERANT;
        } CONTRACTL_END;
        COUNTER_ONLY(GetPrivatePerfCounters().m_Security.cTotalRTChecks++);
    }

    static bool IsSpecialRunFrame(MethodDesc *pMeth);

    static BOOL SkipAndFindFunctionInfo(INT32, MethodDesc**, OBJECTREF**, AppDomain **ppAppDomain = NULL);
    static BOOL SkipAndFindFunctionInfo(StackCrawlMark*, MethodDesc**, OBJECTREF**, AppDomain **ppAppDomain = NULL);


    // check for Link/Inheritance CAS permissions
    static void LinkOrInheritanceCheck(AssemblySecurityDescriptor *pSecDesc, OBJECTREF refDemands, Assembly* pAssembly, CorDeclSecurity action);

    // Tries to avoid unnecessary demands
    static BOOL PreCheck(OBJECTREF* orDemand, BOOL fDemandSet = FALSE);
    static DWORD GetPermissionSpecialFlags (OBJECTREF* orDemand, BOOL *pfExactFlag, BOOL* pUnrestrictedOverride);


    // Does a demand for a CodeAccessPermission : First does PreCheck. If PreCheck fails then calls Check_StackWalk 
    static void Check_PLS_SW(BOOL isPermSet, SecurityStackWalkType eType, OBJECTREF* permOrPermSet, StackCrawlMark* stackMark);

    // Calls into Check_PLS_SW after GC protecting "perm "
    static void Check_PLS_SW_GC(BOOL isPermSet, SecurityStackWalkType eType, OBJECTREF permOrPermSet, StackCrawlMark* stackMark);

    // Walks the stack for a CodeAccessPermission demand (assumes PreCheck was already called)
    static void Check_StackWalk(SecurityStackWalkType eType, OBJECTREF* pPerm, StackCrawlMark* stackMark, BOOL isPermSet);

    // Walk the stack and count all the frame descriptors with an Assert, Deny, or PermitOnly
    static VOID UpdateOverridesCount();

    // Do a quick check to see whether or not all the permissions in the PermissionSet implement IUnrestricted
    static BOOL ContainsIdentityPermissions(OBJECTREF refPermSet);

    // Check the provided demand set against the provided grant/refused set
    static void CheckSetHelper(OBJECTREF *prefDemand,
                               OBJECTREF *prefGrant,
                               OBJECTREF *prefDenied,
                               AppDomain *pGrantDomain,
                               MethodDesc *pMethod,
                               OBJECTREF *pAssembly,
                               CorDeclSecurity action);
};


#endif /* __SECURITYSTACKWALK_H__ */

