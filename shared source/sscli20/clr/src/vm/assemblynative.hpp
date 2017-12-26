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
** Header:  AssemblyNative.hpp
**
** Purpose: Implements FCalls for managed Assembly class
**
** Date:  Oct 26, 1998
**
===========================================================*/
#ifndef _ASSEMBLYNATIVE_H
#define _ASSEMBLYNATIVE_H


class AssemblyNative
{
    friend class Assembly;
    friend class BaseDomain;
    friend class DomainAssembly;

private:
    static PTRARRAYREF GetReferencedAssembliesInternal(Assembly *pAssembly);
    static Assembly* GetPostPolicyAssembly(PEAssembly *pFile,
                                           OBJECTREF *pSecurity,
                                           BOOL fForIntrospection,
                                           BOOL fSetAsExtraEvidence,
                                           BOOL *pfDelayPolicyResolution);

protected:

public:
    static Object* GetTypeInner(DomainAssembly *pAssem,
                                STRINGREF *refClassName, 
                                BOOL bThrowOnError, 
                                BOOL bIgnoreCase);
   


    // static FCALLs
    static FCDECL0(Object*,         GetEntryAssembly);
    static FCDECL2(Object*,         LoadFile,                   StringObject* pathUNSAFE,
                                                                Object* securityUNSAFE);
    static FCDECL5(Object*,         LoadImage,                  U1Array* PEByteArrayUNSAFE, U1Array* SymByteArrayUNSAFE, Object* securityUNSAFE, StackCrawlMark* stackMark, CLR_BOOL fForIntrospection);
    static FCDECL1(Object*,         GetExecutingAssembly,       StackCrawlMark* stackMark);
    static FCDECL2(Object*,         CreateQualifiedName,        StringObject* strAssemblyNameUNSAFE, StringObject* strTypeNameUNSAFE);
    static FCDECL7(Object*,         Load,                       AssemblyNameBaseObject* assemblyNameUNSAFE, 
                                                                StringObject* codeBaseUNSAFE, 
                                                                Object* securityUNSAFE, 
                                                                AssemblyBaseObject* locationHintUNSAFE,
                                                                StackCrawlMark* stackMark,
                                                                CLR_BOOL fThrowOnFileNotFound,
                                                                CLR_BOOL fForIntrospection);


    //
    // instance FCALLs
    //
    static FCDECL1(Object*,         GetLocale,                  Object* refThis);
    static FCDECL1(INT32,           GetHashAlgorithm,           Object* refThis);
    static FCDECL1(Object*,         GetSimpleName,              Object* refThis);
    static FCDECL1(Object*,         GetPublicKey,               Object* refThis);
    static FCDECL1(INT32,           GetFlags,                   Object* refThis);
    static FCDECL1(Object*,         GetStringizedName,          Object* refThis);
    static FCDECL1(Object*,         GetLocation,                Object* refThis);
    static FCDECL1(FC_BOOL_RET,     Reflection,                 Object* refThis);
    static FCDECL2(Object*,         GetCodeBase,                Object* refThis, CLR_BOOL fCopiedName);
    static FCDECL5(BYTE*,           GetResource,                Object* refThis, StringObject* name, UINT64* length, StackCrawlMark* stackMark, CLR_BOOL skipSecurityCheck);
    static FCDECL1(FC_BOOL_RET,     IsDynamic,                  Object* refThis);
    static FCDECL5(void,            GetVersion,                 Object* refThis, INT32* pMajorVersion, INT32* pMinorVersion, INT32*pBuildNumber, INT32* pRevisionNumber);
    static FCDECL5(Object*,         LoadModuleImage,            Object* refThisUNSAFE, StringObject* moduleNameUNSAFE, U1Array* PEByteArrayUNSAFE, U1Array* SymByteArrayUNSAFE, Object* securityUNSAFE);
    static FCDECL4(Object*,         GetType,                    Object* refThisUNSAFE, StringObject* nameUNSAFE, CLR_BOOL bThrowOnError, CLR_BOOL bIgnoreCase);
    static FCDECL5(INT32,           GetManifestResourceInfo,    Object* refThisUNSAFE, StringObject* nameUNSAFE, OBJECTREF* pAssemblyRef, STRINGREF* pFileName, StackCrawlMark* stackMark);
    static FCDECL3(Object*,         GetModules,                 Object* refThisUNSAFE, CLR_BOOL fLoadIfNotFound, CLR_BOOL fGetResourceModules);
    static FCDECL2(Object*,         GetModule,                  Object* refThisUNSAFE, StringObject* strFileNameUNSAFE);
    static FCDECL1(Object*,         GetExportedTypes,           Object* refThisUNSAFE);
    static FCDECL1(Object*,         GetResourceNames,           Object* refThisUNSAFE);
    static FCDECL1(Object*,         GetReferencedAssemblies,    Object* refThisUNSAFE);
    static FCDECL1(MethodDesc*,     GetEntryPoint,              Object* refThisUNSAFE);
    static FCDECL1(Object*,         GetOnDiskAssemblyModule,    Object* refThisUNSAFE);
    static FCDECL1(Object*,         GetInMemoryAssemblyModule,  Object* refThisUNSAFE);
    static FCDECL1(FC_BOOL_RET,     GlobalAssemblyCache,        Object* refThisUNSAFE);
    static FCDECL2(void,            PrepareSavingManifest,      Object* refThisUNSAFE, ReflectModuleBaseObject* moduleUNSAFE);
    static FCDECL2(mdFile,          AddFileList,                Object* refThisUNSAFE, StringObject* strFileNameUNSAFE);
    static FCDECL3(void,            SetHashValue,               Object* refThisUNSAFE, INT32 tkFile, StringObject* strFullFileNameUNSAFE);
    static FCDECL5(mdExportedType,  AddExportedType,            Object* refThisUNSAFE, StringObject* strCOMTypeNameUNSAFE, INT32 ar, INT32 tkTypeDef, INT32 flags);    
    static FCDECL5(void,            AddStandAloneResource,      Object* refThisUNSAFE, StringObject* strNameUNSAFE, StringObject* strFileNameUNSAFE, StringObject* strFullFileNameUNSAFE, INT32 iAttribute);
    static FCDECL4(void,            SavePermissionRequests,     Object* refThisUNSAFE, U1Array* requiredUNSAFE, U1Array* optionalUNSAFE, U1Array* refusedUNSAFE);
    static FCDECL6(void,            SaveManifestToDisk,         Object* refThisUNSAFE, StringObject* strManifestFileNameUNSAFE, INT32 entrypoint, INT32 fileKind, INT32 portableExecutableKind, INT32 imageFileMachine);
    static FCDECL3(void,            AddFileToInMemoryFileList,  Object* refThisUNSAFE, StringObject* strModuleFileNameUNSAFE, Object* refModuleUNSAFE);
    static FCDECL3(void,            GetGrantSet,                Object* refThisUNSAFE, OBJECTREF* ppGranted, OBJECTREF* ppDenied);
    static FCDECL1(Object*,         GetImageRuntimeVersion,     Object* refThisUNSAFE);
    static FCDECL1(INT64,           GetHostContext,             Object* refThisUNSAFE);
};


#endif

