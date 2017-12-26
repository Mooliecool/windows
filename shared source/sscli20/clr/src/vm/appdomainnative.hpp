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
** Header:  AppDomainNative.hpp
**
** Purpose: Implements native methods for AppDomains
**
** Date:  May 20, 2000
**
===========================================================*/
#ifndef _APPDOMAINNATIVE_H
#define _APPDOMAINNATIVE_H

class AppDomainNative
{
public:
    static AppDomain *ValidateArg(APPDOMAINREF pThis);

    static FCDECL5(Object*, CreateDomain, StringObject* strFriendlyNameUNSAFE, Object* appdomainSetup, Object* providedEvidenceUNSAFE, Object* creatorsEvidenceUNSAFE, void* parentSecurityDescriptor);
    static FCDECL5(Object*, CreateInstance, StringObject* strFriendlyNameUNSAFE, Object* appdomainSetup, Object* providedEvidenceUNSAFE, Object* creatorsEvidenceUNSAFE, void* parentSecurityDescriptor);
    static FCDECL4(void, SetupDomainSecurity, AppDomainBaseObject* refThisUNSAFE, Object* appDomainEvidenceUNSAFE, void* parentSecurityDescriptor, CLR_BOOL fPublishAppDomain);
    static FCDECL2(void, SetupFriendlyName, AppDomainBaseObject* refThisUNSAFE, StringObject* strFriendlyNameUNSAFE);
    static FCDECL1(void*, GetSecurityDescriptor, AppDomainBaseObject* refThisUNSAFE);
    static FCDECL2(void, UpdateLoaderOptimization, AppDomainBaseObject* refThisUNSAFE, DWORD optimization);
    static FCDECL8(Object*, CreateDynamicAssembly, AppDomainBaseObject* refThisUNSAFE, AssemblyNameBaseObject* assemblyNameUNSAFE, Object* identityUNSAFE, StackCrawlMark* stackMark, Object* requiredPsetUNSAFE, Object* optionalPsetUNSAFE, Object* refusedPsetUNSAFE, INT32 access);
    static FCDECL0(StringObject*, GetDomainManagerAsm);
    static FCDECL0(StringObject*, GetDomainManagerType);
    static FCDECL1(void, RegisterWithHost, Object* refDomainManagerUNSAFE);
    static FCDECL1(void, SetHostSecurityManagerFlags, DWORD dwFlags);
    static FCDECL0(void, SetSecurityHomogeneousFlag);
    static FCDECL1(Object*, GetFriendlyName, AppDomainBaseObject* refThisUNSAFE);
    static FCDECL1(FC_BOOL_RET, IsDefaultAppDomainForSecurity, AppDomainBaseObject* refThisUNSAFE);
    static FCDECL2(Object*, GetAssemblies, AppDomainBaseObject* refThisUNSAFE, CLR_BOOL fForIntrospection); 
    static FCDECL2(Object*, GetOrInternString, AppDomainBaseObject* refThisUNSAFE, StringObject* pStringUNSAFE);
    static FCDECL3(INT32, ExecuteAssembly, AppDomainBaseObject* refThisUNSAFE, AssemblyBaseObject* assemblyNameUNSAFE, PTRArray* stringArgsUNSAFE);
    static FCDECL1(void, Unload, INT32 dwId);
    static FCDECL1(Object*, GetDynamicDir, AppDomainBaseObject* refThisUNSAFE);
    static FCDECL1(INT32, GetId, AppDomainBaseObject* refThisUNSAFE);
    static FCDECL1(INT32, GetIdForUnload, AppDomainBaseObject* refDomainUNSAFE);
    static FCDECL1(FC_BOOL_RET, IsDomainIdValid, INT32 dwId);
    static FCDECL1(FC_BOOL_RET, IsFinalizingForUnload, AppDomainBaseObject* refThisUNSAFE);
    static FCDECL1(void, ForceToSharedDomain, Object* pObjectUNSAFE);
    static FCDECL3(void, GetGrantSet, AppDomainBaseObject* refThisUNSAFE, OBJECTREF* ppGranted, OBJECTREF* ppDenied);
    static FCDECL1(void, ChangeSecurityPolicy, AppDomainBaseObject* refThisUNSAFE);
    static FCDECL0(Object*, GetDefaultDomain);
    static FCDECL1(LPVOID,  GetFusionContext, AppDomainBaseObject* refThis);
    static FCDECL2(Object*, IsStringInterned, AppDomainBaseObject* refThis, StringObject* pString);
    static FCDECL1(FC_BOOL_RET, IsUnloadingForcedFinalize, AppDomainBaseObject* refThis);
    static FCDECL3(void,    UpdateContextProperty, LPVOID fusionContext, StringObject* key, Object* value);
    static FCDECL2(StringObject*, nApplyPolicy, AppDomainBaseObject* refThisUNSAFE, AssemblyNameBaseObject* assemblyNameUNSAFE);
    static FCDECL1(UINT32,  GetAppDomainId, AppDomainBaseObject* refThisUNSAFE);

private:
    static INT32 ExecuteAssemblyHelper(Assembly* pAssembly,
                                       BOOL bCreatedConsole,
                                       PTRARRAYREF *pStringArgs);
    static HRESULT InitializeDomainManagerVariables ();
    static void CreateDomainHelper (STRINGREF* ppFriendlyName, OBJECTREF* ppAppdomainSetup, OBJECTREF* ppProvidedEvidence, OBJECTREF* ppCreatorsEvidence, void* parentSecurityDescriptor, OBJECTREF* pEntryPointProxy, OBJECTREF* pRetVal);
};

#endif
