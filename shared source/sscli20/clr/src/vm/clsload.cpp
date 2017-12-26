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
//
// clsload.cpp
//

#include "common.h"
#include "winwrap.h"
#include "ceeload.h"
#include "siginfo.hpp"
#include "vars.hpp"
#include "clsload.hpp"
#include "class.h"
#include "method.hpp"
#include "ecall.h"
#include "stublink.h"
#include "object.h"
#include "excep.h"
#include "threads.h"
#include "comstring.h"
#include "comstringbuffer.h"
#include "comsystem.h"
#include "comsynchronizable.h"
#include "threads.h"
#include "threads.inl"
#include "classfac.h"
#include "dllimport.h"
#include "security.h"
#include "dbginterface.h"
#include "log.h"
#include "eeconfig.h"
#include "fieldmarshaler.h"
#include "jitinterface.h"
#include "comvariant.h"
#include "vars.hpp"
#include "assembly.hpp"
#include "perfcounters.h"
#include "eeprofinterfaces.h"
#include "eehash.h"
#include "typehash.h"
#include "comdelegate.h"
#include "array.h"
#include "comnlsinfo.h"
#include "stackprobe.h"
#include "posterror.h"
#include "wrappers.h"
#include "generics.h"
#include "typestring.h"
#include "typedesc.h"
#include "cgencpu.h"
#include "memoryreport.h"
#include "eventtrace.h"
#include "timeline.h"
#include "typekey.h"
#include "pendingload.h"
#include "proftoeeinterfaceimpl.h"
#include "mdaassistants.h"
#include "virtualcallstub.h"
#include "remoting.h"


#include "security.inl"

enum CorEntryPointType
{
    EntryManagedMain,                   // void main(String[])
    EntryCrtMain                        // unsigned main(void)
};

// forward decl
void ValidateMainMethod(MethodDesc * pFD, CorEntryPointType *pType);

#ifndef DACCESS_COMPILE

WCHAR* wszClass = L"Class";
WCHAR* wszFile =  L"File";

extern BOOL CompareCLSID(UPTR u1, UPTR u2);

void DECLSPEC_NORETURN ThrowMainMethodException(MethodDesc* pMD, UINT resID)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    DefineFullyQualifiedNameForClassW();
    LPCWSTR szClassName = GetFullyQualifiedNameForClassW(pMD->GetClass());
    LPCUTF8 szUTFMethodName = pMD->GetMDImport()->GetNameOfMethodDef(pMD->GetMemberDef());
    PREFIX_ASSUME(szUTFMethodName!=NULL);
    MAKE_WIDEPTR_FROMUTF8(szMethodName, szUTFMethodName);
    COMPlusThrowHR(COR_E_METHODACCESS, resID, szClassName, szMethodName);
}

#endif // #ifndef DACCESS_COMPILE

// This method determines the "loader module" for an instantiated type
// or method. The rule must ensure that any types involved in the
// instantiated type or method do not outlive the loader module itself
// with respect to app-domain unloading (e.g. MyList<MyType> can't be
// put in the module of MyList if MyList's assembly is
// app-domain-neutral but MyType's assembly is app-domain-specific).
// The rule we use is:
//
// * Pick the first type in the class instantiation, followed by
//   method instantiation, whose loader module is non-shared (app-domain-bound)
// * If no type is app-domain-bound, return the module containing the generic type itself
//
// Some useful effects of this rule (for ngen purposes) are:
// * G<object,...,object> lives in the module defining G
// * non-mscorlib instantiations of mscorlib-defined generic types live in the module of the instantiation (
// * if only one module is invloved in the instantiation)
//

/* static */
Module* ClassLoader::ComputeLoaderModule(Module *pDefinitionModule,  // the module that declares the generic type or method
                                         mdToken token,              // method or class token for this item
                                         TypeHandle *pClassArgs,     // the type arguments to the type (if any)
                                         DWORD nClassArgs,           // the number of type arguménts to the type
                                         TypeHandle *pMethodArgs,    // the type arguments to the method (if any)
                                         DWORD nMethodArgs)          // the number of type arguments to the method
{
    CONTRACT(Module*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
        PRECONDITION(CheckPointer(pDefinitionModule, NULL_OK));
        PRECONDITION(CheckPointer(pClassArgs, NULL_OK));
        PRECONDITION(CheckPointer(pMethodArgs, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL));
        SO_INTOLERANT;
    }
    CONTRACT_END

    if (nClassArgs == 0 && nMethodArgs == 0)
        RETURN pDefinitionModule;

    Module *pFirstNonSharedLoaderModule = NULL;
    Module *pFirstTypeVarLoaderModule = NULL;
    Module *pFirstNonSystemModule = NULL;
    Module *pLoaderModule = NULL;

    Module *retModule = NULL;
    
    if (pDefinitionModule) {
        if (!pDefinitionModule->GetAssembly()->IsDomainNeutral() && pFirstNonSharedLoaderModule == NULL)
            pFirstNonSharedLoaderModule = pDefinitionModule;
        if (!pDefinitionModule->IsSystem() && pFirstNonSystemModule == NULL)
            pFirstNonSystemModule = pDefinitionModule;
    }

    if (pClassArgs) {
        for (DWORD i = 0; i < nClassArgs; i++) {
            _ASSERTE(!pClassArgs[i].IsEncodedFixup());
            Module *pOpenModule = pClassArgs[i].GetDefiningModuleForOpenType();
            if (pOpenModule != NULL && pFirstTypeVarLoaderModule == NULL)
                pFirstTypeVarLoaderModule = pOpenModule;
            Module* pModule = pClassArgs[i].GetLoaderModule();
            if (!pModule->GetAssembly()->IsDomainNeutral() && pFirstNonSharedLoaderModule == NULL)
                pFirstNonSharedLoaderModule = pModule;
            if (!pModule->IsSystem() && pFirstNonSystemModule == NULL)
                pFirstNonSystemModule = pModule;
        }
    }

    // RULE: Prefer modules from type variables occurring in the class type arguments.
    // This ensures that open types referenced by definitions of generic types and methods
    // live in the module of the definition.
    // This in turn ensures that they can be hard bound.
    if (pFirstTypeVarLoaderModule != NULL) {
        // To be safe for app-domain-unloading, we cannot use the type variable module if it is in a
        // shared assembly and some other component is non-shared.
        // (This will not occur inside definitions of generic types and methods because shared assemblies
        // cannot reference non-shared assemblies).
        if (!pFirstTypeVarLoaderModule->GetAssembly()->IsDomainNeutral() || pFirstNonSharedLoaderModule == NULL)
        {
            retModule = pFirstTypeVarLoaderModule;
            goto Exit;
        }
    }

    if (pMethodArgs) {
        for (DWORD i = 0; i < nMethodArgs; i++) {
            _ASSERTE(!pMethodArgs[i].IsEncodedFixup());
            Module *pModule = pMethodArgs[i].GetLoaderModule();
            if (!pModule->GetAssembly()->IsDomainNeutral() && pFirstNonSharedLoaderModule == NULL)
                pFirstNonSharedLoaderModule = pModule;
            if (!pModule->IsSystem() && pFirstNonSystemModule == NULL)
                pFirstNonSystemModule = pModule;
        }
    }

    // RULE: Prefer modules in non-shared assemblies.
    // This ensures safety of app-domain unloading.
    if (pFirstNonSharedLoaderModule != NULL)
    {
        pLoaderModule = pFirstNonSharedLoaderModule;
    }
    // OK, so all the modules are shared, so any module amongst those involved 
    // will do w.r.t. lifetime properties.  Also we're not NGEN'ing, so it doesn't
    // really matter which one we pick, though it probably helps if we pick the
    // same one repeatedly to avoid writing to too many different instantiation
    // tables.  
    else if (pFirstNonSystemModule != NULL)
    {
        pLoaderModule = pFirstNonSystemModule;
    }
    else
    {
        CONSISTENCY_CHECK((&g_Mscorlib)->GetModule() && (&g_Mscorlib)->GetModule()->IsSystem());

        pLoaderModule = (&g_Mscorlib)->GetModule();
    }

    retModule = pLoaderModule;

#ifndef DACCESS_COMPILE
#endif // #ifndef DACCESS_COMPILE
Exit:
    ;
    
    RETURN (retModule);
}

/*static*/
Module *ClassLoader::ComputeLoaderModule(MethodTable *pMT, mdToken token, TypeHandle *genericMethodArgs, DWORD nGenericMethodArgs)
{
    WRAPPER_CONTRACT;
    return ComputeLoaderModule(pMT->GetModule(), 
                               token,
                               pMT->GetInstantiation(),
                               pMT->GetNumGenericArgs(),
                               genericMethodArgs,
                               nGenericMethodArgs);    
}
/*static*/
Module *ClassLoader::ComputeLoaderModule(TypeKey *typeKey)
{
    WRAPPER_CONTRACT;

    if (typeKey->GetKind() == ELEMENT_TYPE_CLASS)
        return ComputeLoaderModule(typeKey->GetModule(),
                                   typeKey->GetTypeToken(),
                                   typeKey->GetInstantiation(),
                                   typeKey->GetNumGenericArgs(), NULL, 0);
    else if (typeKey->GetKind() == ELEMENT_TYPE_FNPTR)
        return ComputeLoaderModule(NULL,
                                   0,
                                   typeKey->GetRetAndArgTypes(),
                                   typeKey->GetNumArgs()+1, NULL,0);
    else                                                    
    {
        TypeHandle paramType = TypeHandle(typeKey->GetElementType());
        return ComputeLoaderModule(NULL,
                                   0,
                                   &paramType,
                                   1, NULL,0);
    }
}

void PendingTypeLoadEntry::SetExceptionThrowing(Exception *pException)
{
    CONTRACTL
      {
          THROWS;
          GC_TRIGGERS;
          INJECT_FAULT(COMPlusThrowOM(););
          PRECONDITION(HasLock());
          PRECONDITION(m_pException == NULL);
          PRECONDITION(m_dwWaitCount > 0);
      }
    CONTRACTL_END;

    m_pException = pException->Clone();
}

/*static*/ 
BOOL ClassLoader::IsTypicalInstantiation(Module *pModule, mdToken token, DWORD numGenericArgs, TypeHandle *inst)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(TypeFromToken(token) == mdtTypeDef || TypeFromToken(token) == mdtMethodDef);
    }
    CONTRACTL_END

    CorElementType etype = TypeFromToken(token) == mdtTypeDef ? ELEMENT_TYPE_VAR : ELEMENT_TYPE_MVAR;
    for (DWORD i = 0; i < numGenericArgs; i++) {
        if (inst[i].GetSignatureCorElementType() == etype) {
            TypeVarTypeDesc* tyvar =
                PTR_TypeVarTypeDesc(PTR_HOST_TO_TADDR(inst[i].AsTypeDesc()));
            PREFIX_ASSUME(tyvar!=NULL);
            if (PTR_HOST_TO_TADDR(tyvar->GetModule()) !=
                PTR_HOST_TO_TADDR(pModule) ||
                tyvar->GetIndex() != i ||
                tyvar->GetTypeOrMethodDef() != token)
                return FALSE;
        }
        else return FALSE;
    }
    return TRUE;
}

// External class loader entry point: load a type by name
/*static*/
TypeHandle ClassLoader::LoadTypeByNameThrowing(Assembly *pAssembly,
                                               LPCUTF8 nameSpace,
                                               LPCUTF8 name,
                                               NotFoundAction fNotFound,
                                               ClassLoader::LoadTypesFlag fLoadTypes,
                                               ClassLoadLevel level)
{
    CONTRACT(TypeHandle)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
        PRECONDITION(CheckPointer(pAssembly));
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
        POSTCONDITION(CheckPointer(RETVAL, 
                     (fNotFound == ThrowIfNotFound && fLoadTypes == LoadTypes )? NULL_NOT_OK : NULL_OK));
        POSTCONDITION(RETVAL.IsNull() || RETVAL.CheckLoadLevel(level));
    }
    CONTRACT_END

    NameHandle nameHandle(nameSpace, name);
    if (fLoadTypes == DontLoadTypes)
        nameHandle.SetTokenNotToLoad(tdAllTypes);
    if (fNotFound == ThrowIfNotFound)
        RETURN pAssembly->GetLoader()->LoadTypeHandleThrowIfFailed(&nameHandle, level);
    else
        RETURN pAssembly->GetLoader()->LoadTypeHandleThrowing(&nameHandle, level);
}

#ifndef DACCESS_COMPILE

/*static*/
TypeHandle ClassLoader::LoadTypeByNameNoThrow(Assembly *pAssembly,
                                              LPCUTF8 nameSpace,
                                              LPCUTF8 name,
                                              OBJECTREF *pThrowable,
                                              ClassLoadLevel level)
{

    CONTRACTL
    {
        NOTHROW;
        MODE_ANY;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(return TypeHandle();); }
        PRECONDITION(CheckPointer(pAssembly));
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
    }
    CONTRACTL_END

    TypeHandle th;
    BEGIN_EXCEPTION_GLUE(NULL, pThrowable)
    {
        th = LoadTypeByNameThrowing(pAssembly, nameSpace, name, ClassLoader::ThrowIfNotFound, ClassLoader::LoadTypes, level);
    }
    END_EXCEPTION_GLUE
    return th;
}


#endif // #ifndef DACCESS_COMPILE

//
// Find a class given name, using the classloader's global list of known classes.
// If the type is found, it will be restored unless pName->GetTokenNotToLoad() prohibits that
// Returns NULL if class not found AND pName->OKToLoad returns false
TypeHandle ClassLoader::LoadTypeHandleThrowIfFailed(NameHandle* pName, ClassLoadLevel level,
                                                    Module* pLookInThisModuleOnly/*=NULL*/)
{
    CONTRACT(TypeHandle)
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
        PRECONDITION(CheckPointer(pName));
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
        POSTCONDITION(CheckPointer(RETVAL, pName->OKToLoad() ? NULL_NOT_OK : NULL_OK));
        POSTCONDITION(RETVAL.IsNull() || RETVAL.CheckLoadLevel(level));
    }
    CONTRACT_END;

#ifdef _DEBUG_IMPL
    pName->Validate();
#endif

    // Lookup in the classes that this class loader knows about
    TypeHandle typeHnd = LoadTypeHandleThrowing(pName, level, pLookInThisModuleOnly);

    if(typeHnd.IsNull()) {

        if ( pName->OKToLoad() ) {
#ifdef _DEBUG_IMPL
            {
                LPCUTF8 szName = pName->GetName();
                if (szName == NULL)
                    szName = "<UNKNOWN>";
                
                StackSString codeBase;
                GetAssembly()->GetCodeBase(codeBase);

                LOG((LF_CLASSLOADER, LL_INFO10, "Failed to find class \"%s\" in the manifest for assembly \"%ws\"\n", szName, (LPCWSTR)codeBase));
            }
#endif

#ifndef DACCESS_COMPILE
            COUNTER_ONLY(GetPrivatePerfCounters().m_Loading.cLoadFailures++);

            m_pAssembly->ThrowTypeLoadException(pName, IDS_CLASSLOAD_GENERAL);
#else
            DacNotImpl();
#endif
        }
    }

    RETURN(typeHnd);
}

#ifndef DACCESS_COMPILE

EEClassHashEntry_t* ClassLoader::InsertValue(EEClassHashTable *pClassHash, EEClassHashTable *pClassCaseInsHash, LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum Data, EEClassHashEntry_t *pEncloser, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    LPUTF8 pszLowerCaseNS = NULL;
    LPUTF8 pszLowerCaseName = NULL;
    EEClassHashEntry_t *pCaseInsEntry = NULL;

    pClassHash->RetuneHashTable(pamTracker);
    if (pClassCaseInsHash)
        pClassCaseInsHash->RetuneHashTable(pamTracker);

    EEClassHashEntry_t *pEntry = pClassHash->AllocNewEntry(pamTracker);
   
    if (pClassCaseInsHash) {
        CreateCanonicallyCasedKey(pszNamespace, pszClassName, &pszLowerCaseNS, &pszLowerCaseName);
        pCaseInsEntry = pClassCaseInsHash->AllocNewEntry(pamTracker);
    }


    {
        // ! We cannot fail after this point.
        CANNOTTHROWCOMPLUSEXCEPTION();
        FAULT_FORBID();


        pClassHash->InsertValueUsingPreallocatedEntry(pEntry, pszNamespace, pszClassName, Data, pEncloser);
    
        //If we're keeping a table for case-insensitive lookup, keep that up to date
        if (pClassCaseInsHash)
            pClassCaseInsHash->InsertValueUsingPreallocatedEntry(pCaseInsEntry, pszLowerCaseNS, pszLowerCaseName, pEntry, pEntry->pEncloser);
        
        return pEntry;
    }

}

#endif // #ifndef DACCESS_COMPILE

BOOL ClassLoader::CompareNestedEntryWithExportedType(IMDInternalImport *pImport,
                                                     mdExportedType mdCurrent,
                                                     EEClassHashTable *pClassHash,
                                                     EEClassHashEntry_t *pEntry)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    LPCUTF8 Key[2];

    do {
        pImport->GetExportedTypeProps(mdCurrent,
                                      &Key[0],
                                      &Key[1],
                                      &mdCurrent,
                                      NULL, //binding (type def)
                                      NULL); //flags

        if (pClassHash->CompareKeys(pEntry, Key)) {
            // Reached top level class for mdCurrent - return whether
            // or not pEntry is a top level class
            // (pEntry is a top level class if its pEncloser is NULL)
            if ((TypeFromToken(mdCurrent) != mdtExportedType) ||
                (mdCurrent == mdExportedTypeNil))
                return (!pEntry->pEncloser);
        }
        else
            return FALSE;
    }
    while ((pEntry = pEntry->pEncloser) != NULL);

    // Reached the top level class for pEntry, but mdCurrent is nested
    return FALSE;
}


BOOL ClassLoader::CompareNestedEntryWithTypeDef(IMDInternalImport *pImport,
                                                mdTypeDef mdCurrent,
                                                EEClassHashTable *pClassHash,
                                                EEClassHashEntry_t *pEntry)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    LPCUTF8 Key[2];

    do {
        pImport->GetNameOfTypeDef(mdCurrent, &Key[1], &Key[0]);

        if (pClassHash->CompareKeys(pEntry, Key)) {
            // Reached top level class for mdCurrent - return whether
            // or not pEntry is a top level class
            // (pEntry is a top level class if its pEncloser is NULL)
            if (FAILED(pImport->GetNestedClassProps(mdCurrent, &mdCurrent)))
                return (!pEntry->pEncloser);
        }
        else
            return FALSE;
    }
    while ((pEntry = pEntry->pEncloser) != NULL);

    // Reached the top level class for pEntry, but mdCurrent is nested
    return FALSE;
}


BOOL ClassLoader::CompareNestedEntryWithTypeRef(IMDInternalImport *pImport,
                                                mdTypeRef mdCurrent,
                                                EEClassHashTable *pClassHash,
                                                EEClassHashEntry_t *pEntry)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    LPCUTF8 Key[2];

    do {
        pImport->GetNameOfTypeRef(mdCurrent, &Key[0], &Key[1]);

        if (pClassHash->CompareKeys(pEntry, Key)) {
            mdCurrent = pImport->GetResolutionScopeOfTypeRef(mdCurrent);
            // Reached top level class for mdCurrent - return whether
            // or not pEntry is a top level class
            // (pEntry is a top level class if its pEncloser is NULL)
            if ((TypeFromToken(mdCurrent) != mdtTypeRef) ||
                (mdCurrent == mdTypeRefNil))
                return (!pEntry->pEncloser);
        }
        else
            return FALSE;
    }
    while ((pEntry = pEntry->pEncloser)!=NULL);

    // Reached the top level class for pEntry, but mdCurrent is nested
    return FALSE;
}


/*static*/
BOOL ClassLoader::IsNested(Module *pModule, mdToken token, mdToken *mdEncloser)
{
    CONTRACTL
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
    }
    CONTRACTL_END;

    switch(TypeFromToken(token)) {
        case mdtTypeDef:
            return (SUCCEEDED(pModule->GetMDImport()->GetNestedClassProps(token, mdEncloser)));

        case mdtTypeRef:
            *mdEncloser = pModule->GetMDImport()->GetResolutionScopeOfTypeRef(token);
            return ((TypeFromToken(*mdEncloser) == mdtTypeRef) &&
                    (*mdEncloser != mdTypeRefNil));

        case mdtExportedType:
            pModule->GetAssembly()->GetManifestImport()->GetExportedTypeProps(token,
                                                                              NULL, // namespace
                                                                              NULL, // name
                                                                              mdEncloser,
                                                                              NULL, //binding (type def)
                                                                              NULL); //flags
            return ((TypeFromToken(*mdEncloser) == mdtExportedType) &&
                    (*mdEncloser != mdExportedTypeNil));

        default:
            ThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN_TYPE); 
    }
}

BOOL ClassLoader::IsNested(NameHandle* pName, mdToken *mdEncloser)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
    }
    CONTRACTL_END;

    if (pName->GetTypeModule()) {
        if (TypeFromToken(pName->GetTypeToken()) == mdtBaseType)
        {
            if (pName->GetBucket())
                return TRUE;
            return FALSE;
        }
        else
            return IsNested(pName->GetTypeModule(), pName->GetTypeToken(), mdEncloser);
    }
    else
        return FALSE;
}

EEClassHashEntry_t *ClassLoader::GetClassValue(NameHandleTable nhTable,
                                               NameHandle *pName,
                                               HashDatum *pData,
                                               EEClassHashTable **ppTable,
                                               Module* pLookInThisModuleOnly)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        MODE_ANY;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        PRECONDITION(CheckPointer(pName));
    }
    CONTRACTL_END


    mdToken             mdEncloser;
    EEClassHashEntry_t  *pBucket = NULL;

#if _DEBUG
    if (pName->GetName()) {
        if (pName->GetNameSpace() == NULL)
            LOG((LF_CLASSLOADER, LL_INFO1000, "Looking up %s by name.\n",
                 pName->GetName()));
        else
            LOG((LF_CLASSLOADER, LL_INFO1000, "Looking up %s.%s by name.\n",
                 pName->GetNameSpace(), pName->GetName()));
    }
#endif

    if (IsNested(pName, &mdEncloser)) {
        Module *pModule = pName->GetTypeModule();
        PREFIX_ASSUME(pModule != NULL);
        Assembly* assembly=GetAssembly();
        PREFIX_ASSUME(assembly!=NULL);
        ModuleIterator i = assembly->IterateModules();
        Module *pClsModule = NULL;

        while (i.Next()) {
            pClsModule = i.GetModule();
            if (pClsModule->IsResource())
                continue;
            EEClassHashTable* pTable = NULL;
            if (nhTable == nhCaseSensitive)
            {
                *ppTable = pTable = pClsModule->GetAvailableClassHash();
                
            }
            else {
                // currently we expect only these two kinds
                _ASSERTE(nhTable == nhCaseInsensitive);
                *ppTable = pTable = pClsModule->GetAvailableClassCaseInsHash();

                if (pTable == NULL) {
                    // We have not built the table yet - the caller will handle
                    return NULL;
                }
            }

            _ASSERTE(pTable);

            if ((pBucket = pTable->GetValue(pName, pData, TRUE)) != NULL) {
                switch (TypeFromToken(pName->GetTypeToken())) {
                case mdtTypeDef:
                    while ((!CompareNestedEntryWithTypeDef(pModule->GetMDImport(),
                                                           mdEncloser,
                                                           pClsModule->GetAvailableClassHash(),
                                                           pBucket->pEncloser)) &&
                           (pBucket = pTable->FindNextNestedClass(pName, pData, pBucket)) != NULL);
                    break;
                case mdtTypeRef:
                    while ((!CompareNestedEntryWithTypeRef(pModule->GetMDImport(),
                                                           mdEncloser,
                                                           pClsModule->GetAvailableClassHash(),
                                                           pBucket->pEncloser)) &&
                           (pBucket = pTable->FindNextNestedClass(pName, pData, pBucket)) != NULL);
                    break;
                case mdtExportedType:
                    while ((!CompareNestedEntryWithExportedType(pModule->GetAssembly()->GetManifestImport(),
                                                                mdEncloser,
                                                                pClsModule->GetAvailableClassHash(),
                                                                pBucket->pEncloser)) &&
                           (pBucket = pTable->FindNextNestedClass(pName, pData, pBucket)) != NULL);
                    break;
                default:
                    while ((pBucket->pEncloser != pName->GetBucket())  &&
                           (pBucket = pTable->FindNextNestedClass(pName, pData, pBucket)) != NULL);
                }
            }
            if (pBucket) // break on the first success
                break;
        }
    }
    else {
        // Check if this non-nested class is in the table of available classes.
        ModuleIterator i = GetAssembly()->IterateModules();
        Module *pModule = NULL;

        while (i.Next()) {
            pModule = i.GetModule();
            // i.Next will not return TRUE unless i.GetModule will return non-NULL.
            PREFIX_ASSUME(pModule != NULL);
            if (pModule->IsResource())
                continue;
            if (pLookInThisModuleOnly && (pModule != pLookInThisModuleOnly))
                continue;
            
            PREFIX_ASSUME(pModule!=NULL);
            EEClassHashTable* pTable = NULL;
            if (nhTable == nhCaseSensitive)
                *ppTable = pTable = pModule->GetAvailableClassHash();
            else {
                // currently we support only these two types
                _ASSERTE(nhTable == nhCaseInsensitive);
                *ppTable = pTable = pModule->GetAvailableClassCaseInsHash();

                // We have not built the table yet - the caller will handle
                if (pTable == NULL)
                    return NULL;
            }

            _ASSERTE(pTable);
            pBucket = pTable->GetValue(pName, pData, FALSE);
            if (pBucket) // break on the first success
                break;
        }
    }

    return pBucket;
}

#ifndef DACCESS_COMPILE

VOID ClassLoader::PopulateAvailableClassHashTable(Module* pModule,
                                                  AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    mdTypeDef      td;
    HENUMInternal  hTypeDefEnum;
    IMDInternalImport *pImport = pModule->GetMDImport();

    IfFailThrow(pImport->EnumTypeDefInit(&hTypeDefEnum));

    // Now loop through all the classdefs adding the CVID and scope to the hash
    while(pImport->EnumTypeDefNext(&hTypeDefEnum, &td)) {
        
        AddAvailableClassHaveLock(pModule,
                                  td,
                                  pamTracker);
    }
    pImport->EnumTypeDefClose(&hTypeDefEnum);
}


void ClassLoader::LazyPopulateCaseInsensitiveHashTables()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;


    // Add any unhashed modules into our hash tables, and try again.

    ModuleIterator i = GetAssembly()->IterateModules();

    while (i.Next()) {
        Module *pModule = i.GetModule();
        PREFIX_ASSUME(pModule!=NULL);
        if (pModule->IsResource())
            continue;

        if (pModule->GetAvailableClassCaseInsHash() == NULL) {
            AllocMemTracker amTracker;
            EEClassHashTable *pNewClassCaseInsHash = pModule->GetAvailableClassHash()->MakeCaseInsensitiveTable(pModule, &amTracker);

            LOG((LF_CLASSLOADER, LL_INFO10, "%s's classes being added to case insensitive hash table\n",
                 pModule->GetSimpleName()));

            {
                CANNOTTHROWCOMPLUSEXCEPTION();
                FAULT_FORBID();
                
                amTracker.SuppressRelease();
                pModule->SetAvailableClassCaseInsHash(pNewClassCaseInsHash);
                FastInterlockDecrement((LONG*)&m_cUnhashedModules);
            }
        }
    }
}

/*static*/
void DECLSPEC_NORETURN ClassLoader::ThrowTypeLoadException(TypeKey *pKey,
                                                           UINT resIDWhy)
{
    STATIC_CONTRACT_THROWS;

    StackSString fullName;
    StackSString assemblyName;
    TypeString::AppendTypeKey(fullName, pKey);
    pKey->GetModule()->GetAssembly()->GetDisplayName(assemblyName);
    ::ThrowTypeLoadException(fullName, assemblyName, NULL, resIDWhy);        
}

#endif


TypeHandle ClassLoader::LoadConstructedTypeThrowing(TypeKey *pKey,
                                                    LoadTypesFlag fLoadTypes /*= LoadTypes*/,
                                                    ClassLoadLevel level /*=CLASS_LOADED*/)
{
    CONTRACT(TypeHandle)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        if (fLoadTypes == DontLoadTypes) SO_TOLERANT; else SO_INTOLERANT;
        PRECONDITION(CheckPointer(pKey));
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
        POSTCONDITION(CheckPointer(RETVAL, fLoadTypes==DontLoadTypes ? NULL_OK : NULL_NOT_OK));
        POSTCONDITION(RETVAL.IsNull() || RETVAL.GetLoadLevel() >= level);
        MODE_ANY;
    }
    CONTRACT_END

    // Lookup in the classes that this class loader knows about
    TypeHandle typeHnd = LookupTypeHandleForTypeKey(pKey);

    // If something has been published in the tables, and it's at the right level, just return it
    if (!typeHnd.IsNull() && typeHnd.GetLoadLevel() >= level)
    {
        RETURN typeHnd;
    }

    // If we're not loading any types at all, then we're not creating
    // instantiations either because we're in FORBIDGC_LOADER_USE mode, so
    // we should bail out here.
    if (fLoadTypes == DontLoadTypes)
       RETURN TypeHandle();

#ifndef DACCESS_COMPILE
    // If we got here, we now have to allocate a new parameterized type.
    // By definition, forbidgc-users aren't allowed to reach this point.
    CONSISTENCY_CHECK(!FORBIDGC_LOADER_USE_ENABLED());

    GCX_PREEMP();
    Module *pLoaderModule = ComputeLoaderModule(pKey);
    RETURN(pLoaderModule->GetClassLoader()->LoadTypeHandleForTypeKey(pKey, typeHnd, level));
#else
    DacNotImpl();
    RETURN(typeHnd);
#endif
}


/*static*/
void ClassLoader::EnsureLoaded(TypeHandle typeHnd, ClassLoadLevel level)
{
    CONTRACTL
    {
        PRECONDITION(CheckPointer(typeHnd));
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
    }
    CONTRACTL_END

#ifndef DACCESS_COMPILE // Nothing to do for the DAC case

    if (typeHnd.GetLoadLevel() < level)
    {
        INTERIOR_STACK_PROBE_CHECK_THREAD;

        if (level > CLASS_LOAD_UNRESTORED)
        {
            TypeKey typeKey = typeHnd.GetTypeKey();
            
            Module *pLoaderModule = ComputeLoaderModule(&typeKey);
            GCX_PREEMP();
            pLoaderModule->GetClassLoader()->LoadTypeHandleForTypeKey(&typeKey, typeHnd, level);
        }

        END_INTERIOR_STACK_PROBE;
    }

#endif // DACCESS_COMPILE
}

/*static*/
void ClassLoader::TryEnsureLoaded(TypeHandle typeHnd, ClassLoadLevel level)
{
    WRAPPER_CONTRACT;

#ifndef DACCESS_COMPILE // Nothing to do for the DAC case

    EX_TRY
    {
        ClassLoader::EnsureLoaded(typeHnd, level);
    }
    EX_CATCH
    {
        // Some type may not load successfully. For eg. generic instantiations
        // that do not satisfy the constraints of the type arguments.
    }
    EX_END_CATCH(RethrowTerminalExceptions);

#endif // DACCESS_COMPILE
}

// This is separated out to avoid the overhead of C++ exception handling in the non-locking case.
/* static */
TypeHandle ClassLoader::LookupTypeKeyUnderLock(TypeKey *pKey,
                                               EETypeHashTable *pTable,
                                               CrstBase *pLock)
{
    WRAPPER_CONTRACT;

    CrstHolder ch(pLock);
    return pTable->GetValue(pKey);
}

/* static */
TypeHandle ClassLoader::LookupTypeKey(TypeKey *pKey,
                                      EETypeHashTable *pTable,
                                      CrstBase *pLock,
                                      BOOL fCheckUnderLock)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER; 
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pKey));
        PRECONDITION(pKey->IsConstructed());
        PRECONDITION(CheckPointer(pTable));
        PRECONDITION(!fCheckUnderLock || CheckPointer(pLock));
        MODE_ANY;
    } CONTRACTL_END;

    TypeHandle th;

    // If this is the GC thread, and we're hosted, we're in a sticky situation with
    // SQL where we may have suspended another thread while doing a SysSuspendForGC.
    // In this case, we have the issue that a thread holding this lock could be
    // suspended, perhaps implicitly because the active thread on the SQL scheduler
    // has been suspended by the GC thread. In such a case, we need to skip taking
    // the lock. We can be sure that there will be no races in such a condition because
    // we will only be looking for types that are already loaded, or for a type that
    // is not loaded, but we will never cause the type to get loaded, and so the result
    // of the lookup will not change.
    if (fCheckUnderLock && !(IsGCThread() && CLRTaskHosted()))
    {
        th = LookupTypeKeyUnderLock(pKey, pTable, pLock);
    }
    else
    {
        th = pTable->GetValue(pKey);
    }

    return th;
}



/* static */
TypeHandle ClassLoader::LookupInLoaderModule(TypeKey *pKey, BOOL fCheckUnderLock)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER; 
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pKey));
        PRECONDITION(pKey->IsConstructed());
        MODE_ANY;
    } CONTRACTL_END;


    TypeHandle th;

    Module *pLoaderModule = ComputeLoaderModule(pKey);
    PREFIX_ASSUME(pLoaderModule!=NULL);
    
    TypeHandle thLoaderModule = LookupTypeKey(pKey,
                                              pLoaderModule->GetAvailableParamTypes(),
                                              &pLoaderModule->GetClassLoader()->m_AvailableParamTypesLock,
                                              fCheckUnderLock);

    if (!thLoaderModule.IsNull() && !thLoaderModule.IsZapped())
        th = thLoaderModule;

    return th;
}





/* static */
TypeHandle ClassLoader::LookupTypeHandleForTypeKey(TypeKey *pKey)
{
    WRAPPER_CONTRACT;

    // Make an initial lookup without taking any locks.
    TypeHandle th = LookupTypeHandleForTypeKeyInner(pKey, FALSE);

    // A non-null TypeHandle for the above lookup indicates success
    // A null TypeHandle only indicates "well, it might have been there,
    // try again with a lock".  This kind of negative result will
    // only happen while accessing the underlying EETypeHashTable 
    // during a resize, i.e. very rarely. In such a case, we just
    // perform the lookup again, but indicate that appropriate locks
    // should be taken.

    if (th.IsNull())
    {
        th = LookupTypeHandleForTypeKeyInner(pKey, TRUE);
    }

    return th;
}
/* static */
TypeHandle ClassLoader::LookupTypeHandleForTypeKeyInner(TypeKey *pKey, BOOL fCheckUnderLock)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER; 
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pKey));
        MODE_ANY;
    }
    CONTRACTL_END

    // Check if it's the typical instantiation.  In this case it's not stored in the same
    // way as other constructed types.
    if (!pKey->IsConstructed() || 
        pKey->GetKind() == ELEMENT_TYPE_CLASS && ClassLoader::IsTypicalInstantiation(pKey->GetModule(), 
                                                                                     pKey->GetTypeToken(),
                                                                                     pKey->GetNumGenericArgs(),
                                                                                     pKey->GetInstantiation()))
    {
        return TypeHandle(pKey->GetModule()->LookupTypeDef(pKey->GetTypeToken(), CLASS_LOAD_UNRESTOREDTYPEKEY));
    }


    // Next look in the loader module.  This is where the item is guaranteed to live if
    // it is not latched from an NGEN image, i.e. if it is JIT loaded. 
    // If the thing is not NGEN'd then this may
    // be different to pPreferredZapModule.  If they are the same then 
    // we can reuse the results of the lookup above.
    TypeHandle thLM = LookupInLoaderModule(pKey, fCheckUnderLock);
    if (!thLM.IsNull())
    {
        return thLM;
    }


    return TypeHandle();
}


//---------------------------------------------------------------------------
// ClassLoader::TryFindDynLinkZapType
//
// This is a major routine in the process of finding and using
// zapped generic instantiations (excluding those which were zapped into
// their PreferredZapModule).
//
// DynLinkZapItems are generic instantiations that may have been NGEN'd
// into more than one NGEN image (e.g. the code and TypeHandle for 
// List<int> may in principle be zapped into several client images - it is theoretically
// an NGEN policy decision about how often this done, though for now we
// have hard-baked a strategy).  
// 
// There are lots of potential problems with this kind of duplication 
// and the way we get around nearly all of these is to make sure that
// we only use one at most one "unique" copy of each item 
// at runtime. Thus we keep tables in the SharedDomain and the AppDomain indicating
// which unique items have been chosen.  If an item is "loaded" by this technique
// then it will not be loaded by any other technique.
//
// Note generic instantiations may have the good fortune to be zapped 
// into the "PreferredZapModule".  If so we can hardbind to them and
// they will not be considered to be DynLinkZapItems.  We always
// look in the PreferredZapModule first, and we do not add an entry to the
// DynLinkZapItems table for this case.
//
// Zap references to DynLinkZapItems are always via encoded fixups, except 
// for a few intra-module references when one DynLinkZapItem is "TightlyBound"
// to another, e.g. an canonical DynLinkZap MethodTable may directly refer to 
// its EEClass - this is because we know that if one is used at runtime then the
// other will also be.  These items should be thought of as together constituting
// one DynLinkedZapItem.
//
// This function section searches for a copy of the instantiation in various NGEN images.  This is effectively
// like doing a load since we are choosing which copy of the instantiation
// to use from among a number of potential candidates.  We have to have the loading lock
// for this item before we can do this to make sure no other threads choose a
// different copy of the instantiation, and that no other threads are JIT-loading
// the instantiation.



#ifndef DACCESS_COMPILE
#endif // !DACCESS_COMPILE

// FindClassModuleThrowing discovers which module the type you're looking for is in and loads the Module if necessary.
// Basically, it iterates through all of the assembly's modules until a name match is found in a module's
// AvailableClassHashTable.
//
// The possible outcomes are:
//
//    - Function returns TRUE   - class exists and we successfully found/created the containing Module. See below
//                                for how to deconstruct the results.
//    - Function returns FALSE  - class affirmatively NOT found (that means it doesn't exist as a regular type although
//                                  it could also be a parameterized type)
//    - Function throws         - OOM or some other reason we couldn't do the job (if it's a case-sensitive search
//                                  and you're looking for already loaded type or you've set the TokenNotToLoad.
//                                  we are guaranteed not to find a reason to throw.)
//
//
// If it succeeds (returns TRUE), one of the following will occur. Check (*pType)->IsNull() to discriminate.
//
//     1. *pType: set to the null TypeHandle()
//        *ppModule: set to the owning Module
//        *pmdClassToken: set to the typedef
//        *pmdFoundExportedType: if this name bound to an ExportedType, this contains the mdtExportedType token (otherwise,
//                               it's set to mdTokenNil.) You need this because in this case, *pmdClassToken is just
//                               a best guess and you need to verify it. (The division of labor between this
//                               and LoadTypeHandle could definitely be better!)
//
//     2. *pType: set to non-null TypeHandle()
//        This means someone else had already done this same lookup before you and caused the actual
//        TypeHandle to be cached. Since we know that's what you *really* wanted, we'll just forget the
//        Module/typedef stuff and give you the actual TypeHandle.
//
//
BOOL ClassLoader::FindClassModuleThrowing(NameHandle* pName,
                                          TypeHandle* pType,
                                          mdToken* pmdClassToken,
                                          Module** ppModule,
                                          mdToken *pmdFoundExportedType,
                                          EEClassHashEntry_t** ppEntry,
                                          Module* pLookInThisModuleOnly,
                                          Loader::LoadFlag loadFlag)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        PRECONDITION(CheckPointer(pName));
        MODE_ANY;
    }
    CONTRACTL_END

    HashDatum   Data;
    EEClassHashEntry_t *pBucket;
    PTR_EEClassHashEntry *ppBucket = &(pName->m_pBucket);
    NameHandleTable nhTable = nhCaseSensitive; // just to initialize this ...
    NameHandle  tmpName = *pName;
    pName = &tmpName;

    switch (pName->GetTable()) 
    {

      case nhCaseInsensitive :
      {
#ifndef DACCESS_COMPILE
        // GC-type users should only be loading types through tokens.
#ifdef _DEBUG_IMPL
        _ASSERTE(!FORBIDGC_LOADER_USE_ENABLED());
#endif

        // Use the case insensitive table
        nhTable = nhCaseInsensitive;

        // Create a low case version of the namespace and name
        LPUTF8 pszLowerNameSpace = NULL;
        LPUTF8 pszLowerClassName = "";
        int allocLen;

        if(pName->GetNameSpace()) {
            allocLen = InternalCasingHelper::InvariantToLower(NULL, 0, pName->GetNameSpace());
            if (!allocLen)
                return FALSE;

            pszLowerNameSpace = (LPUTF8)_alloca(allocLen);
            if (allocLen == 1)
                *pszLowerNameSpace = '\0';
            else if (!InternalCasingHelper::InvariantToLower(pszLowerNameSpace, allocLen, pName->GetNameSpace()))
                return FALSE;
        }

        _ASSERTE(pName->GetName());
        allocLen = InternalCasingHelper::InvariantToLower(NULL, 0, pName->GetName());
        if (!allocLen)
            return FALSE;

        pszLowerClassName = (LPUTF8)_alloca(allocLen);
        if (!InternalCasingHelper::InvariantToLower(pszLowerClassName, allocLen, pName->GetName()))
            return FALSE;

        // Substitute the lower case version of the name.
        // The field are will be released when we leave this scope
        pName->SetName(pszLowerNameSpace, pszLowerClassName);
        break;
#else
        DacNotImpl();
        break;
#endif // #ifndef DACCESS_COMPILE
      }
      case nhCaseSensitive :
        nhTable = nhCaseSensitive;
        break;
    }

    // Remember if there are any unhashed modules.  We must do this before
    // the actual look to avoid a race condition with other threads doing lookups.
#ifdef LOGGING
    BOOL incomplete = (m_cUnhashedModules > 0);
#endif

    EEClassHashTable *pTable = NULL;
    pBucket = GetClassValue(nhTable, pName, &Data, &pTable, pLookInThisModuleOnly);

    if (pBucket == NULL) {
        if (nhTable == nhCaseInsensitive) {

#ifndef DACCESS_COMPILE
            // Do this outside the lock
            InternalCasingHelper::InitTable();
#endif

            AvailableClasses_LockHolder lh(this);

            // Try again with the lock.  This will protect against another thread reallocating
            // the hash table underneath us
            pBucket = GetClassValue(nhTable, pName, &Data, &pTable, pLookInThisModuleOnly);

            {
#ifndef DACCESS_COMPILE
                if (pBucket == NULL && m_cUnhashedModules > 0) {
                    LazyPopulateCaseInsensitiveHashTables();

                    // Try yet again with the new classes added
                    pBucket = GetClassValue(nhTable, pName, &Data, &pTable, pLookInThisModuleOnly);
                }
#endif
            }
        }
    }

    if (!pBucket) {
#ifdef _DEBUG_IMPL
        LPCUTF8 szName = pName->GetName();
        if (szName == NULL)
            szName = "<UNKNOWN>";
        LOG((LF_CLASSLOADER, LL_INFO10, "Failed to find type \"%s\", assembly \"%ws\" in hash table. Incomplete = %d\n",
            szName, GetAssembly()->GetDebugName(), incomplete));
#endif
        return FALSE;
    }

    if(pName->GetTable() == nhCaseInsensitive) {
        _ASSERTE(Data);
        pBucket = (EEClassHashEntry_t*) Data;
        Data = pBucket->Data;
    }

    if (pName->GetTypeToken() == mdtBaseType)
        *ppBucket = PTR_EEClassHashEntry(PTR_HOST_TO_TADDR(pBucket));

    // Lower bit is a discriminator.  If the lower bit is NOT SET, it means we have
    // a TypeHandle. Otherwise, we have a Module/CL.
    if ((((ULONG_PTR) Data) & EECLASSHASH_TYPEHANDLE_DISCR) == 0) {
        TypeHandle t = TypeHandle::FromPtr(Data);
        _ASSERTE(!t.IsNull());

        *pType = t;
        if(ppEntry) *ppEntry = pBucket;
        return TRUE;
    }

    // We have a Module/CL
    pTable->UncompressModuleAndClassDef(Data, loadFlag,
                                        ppModule, pmdClassToken,
                                        pmdFoundExportedType);


    *pType = TypeHandle();
    if(ppEntry) *ppEntry = pBucket;
    return TRUE;
}

// Does not throw an exception if the type was not found.  Use LoadTypeHandleThrowIfFailed()
// instead if you need that.
//
TypeHandle ClassLoader::LoadTypeHandleThrowing(NameHandle* pName, ClassLoadLevel level,
                                               Module* pLookInThisModuleOnly/*=NULL*/)
{
    CONTRACT(TypeHandle) {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
        PRECONDITION(CheckPointer(pName));
        POSTCONDITION(RETVAL.IsNull() || RETVAL.GetLoadLevel() >= level);
        MODE_ANY;
    } CONTRACT_END

    TypeHandle  typeHnd;
    INTERIOR_STACK_PROBE_NOTHROW_CHECK_THREAD(RETURN_FROM_INTERIOR_PROBE(TypeHandle()));

    Module*     pFoundModule = NULL;
    mdToken     FoundCl;
    EEClassHashEntry_t* pEntry = NULL;
    mdExportedType FoundExportedType = mdTokenNil;


    {
        // Look outside the lock (though we're actually still a long way from the
        // lock at this point...).  This may discover that the type is actually
        // defined in another module...

        BOOL foundSomething = FindClassModuleThrowing(pName,
                                                      &typeHnd,
                                                      &FoundCl,
                                                      &pFoundModule,
                                                      &FoundExportedType,
                                                      &pEntry,
                                                      pLookInThisModuleOnly,
                                                      pName->OKToLoad()?Loader::Load:Loader::DontLoad);
        
        if (foundSomething && !typeHnd.IsNull() && typeHnd.GetLoadLevel() >= level)  // Found the cached value, or a constructedtype
            RETURN_FROM_INTERIOR_PROBE(typeHnd);
        
        if (!typeHnd.IsNull()) {
            typeHnd = LoadTypeDefThrowing(typeHnd.GetModule(), typeHnd.GetCl(),
                                          ClassLoader::ReturnNullIfNotFound, 
                                          ClassLoader::PermitUninstDefOrRef, // when loading by name we always permit naked type defs/refs
                                          pName->GetTokenNotToLoad(), 
                                          level);
            RETURN_FROM_INTERIOR_PROBE(typeHnd);
        }
        
        if (foundSomething) { 
            // Found a cl, pModule pair            

            MEMORY_REPORT_ASSEMBLY_SCOPE(pFoundModule);

            if (pFoundModule->GetClassLoader() == this) {
                BOOL fTrustTD = TRUE;
#ifdef DACCESS_COMPILE
                BOOL fVerifyTD = FALSE;
#else
                BOOL fVerifyTD = ((FoundExportedType != mdTokenNil) &&
                    !(m_pAssembly->IsStrongNamed() ||
                    Security::IsSigned(m_pAssembly->GetSecurityDescriptor())));

                // If this is an exported type with a mdTokenNil class token, then then
                // exported type did not give a typedefID hint. We won't be able to trust the typedef
                // here.
                if (FoundExportedType != mdTokenNil && FoundCl == mdTokenNil) {
                    fVerifyTD = TRUE;
                    fTrustTD = FALSE;
                }
                // verify that FoundCl is a valid token for pFoundModule, because
                // it may be just the hint saved in an ExportedType in another scope
                else if (fVerifyTD) {
                    fTrustTD = pFoundModule->GetMDImport()->IsValidToken(FoundCl);
                }                    
#endif // #ifndef DACCESS_COMPILE
                    
                if (fTrustTD) {
                    typeHnd = LoadTypeDefThrowing(pFoundModule, FoundCl, 
                                                  ClassLoader::ReturnNullIfNotFound, 
                                                  ClassLoader::PermitUninstDefOrRef, // when loading by name we always permit naked type defs/refs
                                                  pName->GetTokenNotToLoad(), 
                                                  level);
                }                
                // If we used a TypeDef saved in a ExportedType, if we didn't verify
                // the hash for this internal module, don't trust the TD value.
                if (fVerifyTD) {
                    BOOL fNoMatch = TRUE;
                    if (!typeHnd.IsNull()) {
                        // Catch transient exceptions (OOM) and just indicate no match in that case.
#ifndef DACCESS_COMPILE
                        EX_TRY {
#endif // #ifndef DACCESS_COMPILE
                            StackSString ssBuiltName;
                            ns::MakePath(ssBuiltName,
                                         StackSString(SString::Utf8, pName->GetNameSpace()),
                                         StackSString(SString::Utf8, pName->GetName()));
                            StackSString ssName;
                            typeHnd.GetName(ssName);
                            fNoMatch = !ssName.Equals(ssBuiltName);
#ifndef DACCESS_COMPILE
                        }
                        EX_CATCH {
                            // Technically, the above operations should never result in a non-OOM
                            // exception, but we'll put the rethrow line in there just in case.
                            CONSISTENCY_CHECK(!GET_EXCEPTION()->IsTerminal());
                            RethrowTerminalExceptions;
                        }
                        EX_END_CATCH(SwallowAllExceptions);
#endif // #ifndef DACCESS_COMPILE
                    }
                    
                    if (fNoMatch) {
                        if (SUCCEEDED(FindTypeDefByExportedType(m_pAssembly->GetManifestImport(),
                                                                FoundExportedType,
                                                                pFoundModule->GetMDImport(),
                                                                &FoundCl))) 
                        {
                            typeHnd = LoadTypeDefThrowing(pFoundModule, FoundCl, 
                                                          ClassLoader::ReturnNullIfNotFound, 
                                                          ClassLoader::PermitUninstDefOrRef, 
                                                          pName->GetTokenNotToLoad(), 
                                                          level);
                        }
                        else
                        {
                            typeHnd = TypeHandle();
                            RETURN_FROM_INTERIOR_PROBE(typeHnd);
                        }
                    }
                }
            }
            else {
                EEClassHashEntry_t *pBucket = pName->GetBucket();
                if (pBucket) { // reset pName's bucket entry
                    if (pBucket->pEncloser)
                        pName->SetBucket(pBucket->pEncloser);
                    else
                        pName->SetBucket(NULL);
                }

                typeHnd = pFoundModule->GetClassLoader()->LoadTypeHandleThrowing(pName, level);

                if ((FoundExportedType != mdTokenNil) &&
                    (!typeHnd.IsNull()) &&
                    typeHnd.IsGenericTypeDefinition()) {
                    DWORD dwFlags;
                    m_pAssembly->GetManifestImport()->GetExportedTypeProps(FoundExportedType,
                                                                           NULL,  // namespace
                                                                           NULL,  // name
                                                                           NULL, // impl token
                                                                           NULL,  // TypeDef
                                                                           &dwFlags);
                    if (IsTdForwarder(dwFlags)) {
                        ThrowHR(COR_E_BADIMAGEFORMAT, BFA_NO_FWD_GEN_TYPES);
                    }
                }
            }

            // Replace AvailableClasses Module entry with found TypeHandle
            if (!typeHnd.IsNull() && typeHnd.IsRestored() && pEntry != NULL && pEntry->Data != typeHnd.AsPtr())
                pEntry->Data = typeHnd.AsPtr();
        }
    }

    
    if (!typeHnd.IsNull() && typeHnd.IsRestored()) {
        // Move any system interfaces defined for this type to the current domain.
        if(typeHnd.IsUnsharedMT()) {
        }
    }

    END_INTERIOR_STACK_PROBE;
    RETURN typeHnd;
}

/* static */
TypeHandle ClassLoader::LoadPointerOrByrefTypeThrowing(CorElementType typ, 
                                                       TypeHandle baseType,
                                                       LoadTypesFlag fLoadTypes/*=LoadTypes*/, 
                                                       ClassLoadLevel level/*=CLASS_LOADED*/)
{
    CONTRACT(TypeHandle)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
        PRECONDITION(CheckPointer(baseType));
        PRECONDITION(typ == ELEMENT_TYPE_BYREF || typ == ELEMENT_TYPE_PTR);
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
        POSTCONDITION(CheckPointer(RETVAL, ((fLoadTypes == LoadTypes) ? NULL_NOT_OK : NULL_OK)));
    }
    CONTRACT_END

    TypeKey key(typ, baseType);
    RETURN(LoadConstructedTypeThrowing(&key, fLoadTypes, level));
}

/* static */
TypeHandle ClassLoader::LoadFnptrTypeThrowing(BYTE callConv,
                                              DWORD ntypars,
                                              TypeHandle* inst, 
                                              LoadTypesFlag fLoadTypes/*=LoadTypes*/,
                                              ClassLoadLevel level/*=CLASS_LOADED*/)
{
    CONTRACT(TypeHandle)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
        POSTCONDITION(CheckPointer(RETVAL, ((fLoadTypes == LoadTypes) ? NULL_NOT_OK : NULL_OK)));
        MODE_ANY;
    }
    CONTRACT_END

    TypeKey key(callConv, ntypars, inst);
    RETURN(LoadConstructedTypeThrowing(&key, fLoadTypes, level));
}

#ifndef DACCESS_COMPILE

/* static */
TypeHandle ClassLoader::LoadGenericInstantiationNoThrow(Module *pModule,
                                                        mdTypeDef typeDef,
                                                        DWORD nGenericClassArgs,
                                                        TypeHandle* pGenericClassArgs,
                                                        OBJECTREF *pThrowable,
                                                        LoadTypesFlag fLoadTypes/*=LoadTypes*/,
                                                        ClassLoadLevel level/*=CLASS_LOADED*/)
{
    CONTRACTL
    {
        NOTHROW;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        PRECONDITION((pThrowable != NULL ||  FORBIDGC_LOADER_USE_ENABLED()) && "You may not ignore exceptions from the loader!");
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
    }
    CONTRACTL_END

    TypeHandle th;
    BEGIN_EXCEPTION_GLUE(NULL, pThrowable)
    {
        th = LoadGenericInstantiationThrowing(pModule, typeDef, nGenericClassArgs, pGenericClassArgs, 
                                              fLoadTypes, level);
    }
    END_EXCEPTION_GLUE
    return th;
}

#endif // #ifndef DACCESS_COMPILE

// Find an instantiation of a generic type if it has already been created.
// If genericTy is not a generic type or is already instantiated then throw an exception.
// If its arity does not match ntypars then throw an exception.
// Value will be non-null if we're loading types.
/* static */
TypeHandle ClassLoader::LoadGenericInstantiationThrowing(Module *pModule,
                                                         mdTypeDef typeDef,
                                                         DWORD nGenericClassArgs,
                                                         TypeHandle* pGenericClassArgs,
                                                         LoadTypesFlag fLoadTypes/*=LoadTypes*/,
                                                         ClassLoadLevel level/*=CLASS_LOADED*/)
{
    // This can be called in FORBIDGC_LOADER_USE mode by the debugger to find
    // a particular generic type instance that is already loaded.
    CONTRACT(TypeHandle)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        PRECONDITION(CheckPointer(pModule));
        MODE_ANY;
        PRECONDITION(nGenericClassArgs == 0 || CheckPointer(pGenericClassArgs));
        PRECONDITION(nGenericClassArgs == 0 || !pGenericClassArgs[0].IsNull()); // check at least the first type handle is ok
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
        POSTCONDITION(CheckPointer(RETVAL, ((fLoadTypes == LoadTypes) ? NULL_NOT_OK : NULL_OK)));
    }
    CONTRACT_END

    DECLARE_INTERIOR_STACK_PROBE;

    IMDInternalImport *pInternalImport = pModule->GetMDImport();

    // Count the formal type parameters
    HENUMInternal   hEnumGenericPars;
    HRESULT hr = pInternalImport->EnumInit(mdtGenericParam, typeDef, &hEnumGenericPars);
#ifndef DACCESS_COMPILE
    Assembly* pAssembly = pModule->GetAssembly();
    if (FAILED(hr))
        pAssembly->ThrowTypeLoadException(pInternalImport, typeDef, IDS_CLASSLOAD_BADFORMAT);
#else
    if (FAILED(hr))
    {
        DacNotImpl();
        return TypeHandle();
    }
#endif
   

    DWORD nGenericClassParams = pInternalImport->EnumGetCount(&hEnumGenericPars);
    pInternalImport->EnumClose(&hEnumGenericPars);


    // Essentially all checks to determine if a generic instantiation of a type
    // is well-formed go in this method, i.e. this is the 
    // "choke" point through which all attempts
    // to create an instantiation flow.  There is a similar choke point for generic
    // methods in genmeth.cpp.

    // If the class isn't generic and there are no type parameters then everything's OK
    if (nGenericClassParams == 0 && pGenericClassArgs == NULL && nGenericClassArgs == 0)
    {
        TypeHandle th = LoadTypeDefThrowing(pModule, typeDef, 
                                            ThrowIfNotFound,
                                            FailIfUninstDefOrRef, 
                                            fLoadTypes == DontLoadTypes ? tdAllTypes : tdNoTypes, 
                                            level);
        RETURN_FROM_INTERIOR_PROBE(th);
    }

    // We interpret pGenericClassArgs=NULL as <__Canon,...,__Canon> instantiation
    if (pGenericClassArgs == NULL)
    {
        SIZE_T cbAllocaSize = 0;

        if (!ClrSafeInt<SIZE_T>::multiply(sizeof(TypeHandle), nGenericClassArgs, cbAllocaSize))
#ifdef DACCESS_COMPILE
            ThrowHR(E_INVALIDARG);
#else
            COMPlusThrowHR(COR_E_OVERFLOW);
#endif

        if (ShouldProbeOnThisThread() && (cbAllocaSize/PAGE_SIZE+1) >= 2)
        {
            DO_INTERIOR_STACK_PROBE_FOR_NOTHROW(GetThread(),cbAllocaSize/PAGE_SIZE+1, NO_FORBIDGC_LOADER_USE_ThrowSO(););
        }

        pGenericClassArgs = (TypeHandle*) _alloca(cbAllocaSize);
        for (DWORD i = 0; i < nGenericClassArgs; i++)
            pGenericClassArgs[i] = TypeHandle(g_pHiddenMethodTableClass);
    }

    if (nGenericClassParams != nGenericClassArgs)
    {
#ifndef DACCESS_COMPILE
        pAssembly->ThrowTypeLoadException(pInternalImport, typeDef, IDS_CLASSLOAD_TYPEWRONGNUMGENERICARGS);
#else
        DacNotImpl();
        return TypeHandle();
#endif
    }

    // Check for typical instantiation.  
    if (ClassLoader::IsTypicalInstantiation(pModule,
                                            typeDef,
                                            nGenericClassArgs, 
                                            pGenericClassArgs))
    {
        TypeHandle th = LoadTypeDefThrowing(pModule, typeDef, 
                                            ThrowIfNotFound, 
                                            PermitUninstDefOrRef,
                                            fLoadTypes == DontLoadTypes ? tdAllTypes : tdNoTypes, 
                                            level);
        RETURN_FROM_INTERIOR_PROBE(th);
    }

    if (!g_pConfig->DontLoadOpenTypes())
    {
        ClassLoader::LoadTypeDefThrowing(pModule, typeDef, 
                                         ThrowIfNotFound,
                                         PermitUninstDefOrRef,
                                         fLoadTypes == DontLoadTypes ? tdAllTypes : tdNoTypes, 
                                         level);
    }

#ifndef DACCESS_COMPILE
    if (!Generics::CheckInstantiation(nGenericClassArgs, pGenericClassArgs))
        pAssembly->ThrowTypeLoadException(pInternalImport, typeDef, IDS_CLASSLOAD_INVALIDINSTANTIATION);

    if (!Generics::CheckInstantiationForRecursion(nGenericClassArgs, pGenericClassArgs))
        pAssembly->ThrowTypeLoadException(pInternalImport, typeDef, IDS_CLASSLOAD_INSTANTIATION_TOO_DEEP);
#endif

    TypeKey key(pModule, typeDef, nGenericClassArgs, pGenericClassArgs);
    TypeHandle th = LoadConstructedTypeThrowing(&key, fLoadTypes, level);
    RETURN_FROM_INTERIOR_PROBE(th);

    END_INTERIOR_STACK_PROBE;
}

//   For non-nested classes, gets the ExportedType name and finds the corresponding
// TypeDef.
//   For nested classes, gets the name of the ExportedType and its encloser.
// Recursively gets and keeps the name for each encloser until we have the top
// level one.  Gets the TypeDef token for that.  Then, returns from the
// recursion, using the last found TypeDef token in order to find the
// next nested level down TypeDef token.  Finally, returns the TypeDef
// token for the type we care about.
/*static*/
HRESULT ClassLoader::FindTypeDefByExportedType(IMDInternalImport *pCTImport, mdExportedType mdCurrent,
                                               IMDInternalImport *pTDImport, mdTypeDef *mtd)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END

    mdToken mdImpl;
    LPCSTR szcNameSpace;
    LPCSTR szcName;
    HRESULT hr;

    pCTImport->GetExportedTypeProps(mdCurrent,
                                    &szcNameSpace,
                                    &szcName,
                                    &mdImpl,
                                    NULL, //binding
                                    NULL); //flags
    if ((TypeFromToken(mdImpl) == mdtExportedType) &&
        (mdImpl != mdExportedTypeNil)) {
        // mdCurrent is a nested ExportedType
        if (FAILED(hr = FindTypeDefByExportedType(pCTImport, mdImpl, pTDImport, mtd)))
            return hr;

        // Get TypeDef token for this nested type
        return pTDImport->FindTypeDef(szcNameSpace, szcName, *mtd, mtd);
    }

    // Get TypeDef token for this top-level type
    return pTDImport->FindTypeDef(szcNameSpace, szcName, mdTokenNil, mtd);
}

#ifndef DACCESS_COMPILE

VOID ClassLoader::CreateCanonicallyCasedKey(LPCUTF8 pszNameSpace, LPCUTF8 pszName, LPUTF8 *ppszOutNameSpace, LPUTF8 *ppszOutName)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        SO_TOLERANT;
        MODE_ANY;
    }
    CONTRACTL_END

    // We can use the NoThrow versions here because we only call this routine if we're maintaining
    // a case-insensitive hash table, and the creation of that table initialized the
    // CasingHelper system.
    INT32 iNSLength = InternalCasingHelper::InvariantToLowerNoThrow(NULL, 0, pszNameSpace);
    if (!iNSLength)
        COMPlusThrowOM();

    INT32 iNameLength = InternalCasingHelper::InvariantToLowerNoThrow(NULL, 0, pszName);
    if (!iNameLength)
        COMPlusThrowOM();

    {
        //Calc & allocate path length
        //Includes terminating null
        INT32 iAllocSize = 0;
        if (!ClrSafeInt<INT32>::addition(iNSLength, iNameLength, iAllocSize))
            ThrowHR(COR_E_OVERFLOW);
        AllocMemHolder<char> pszOutNameSpace (GetAssembly()->GetHighFrequencyHeap()->AllocMem(iAllocSize));
        *ppszOutNameSpace = pszOutNameSpace;
    
        if (iNSLength == 1)
            **ppszOutNameSpace = '\0';
        else {
            if (!InternalCasingHelper::InvariantToLowerNoThrow(*ppszOutNameSpace, iNSLength, pszNameSpace))
                COMPlusThrowOM();
        }

        *ppszOutName = *ppszOutNameSpace + iNSLength;
    
        if (!InternalCasingHelper::InvariantToLowerNoThrow(*ppszOutName, iNameLength, pszName))
            COMPlusThrowOM();

        pszOutNameSpace.SuppressRelease();
    }
}

#endif // #ifndef DACCESS_COMPILE


//
// Return a class that is already loaded
// Only for type refs and type defs (not type specs)
//
/*static*/
TypeHandle ClassLoader::LookupTypeDefOrRefInModule(Module *pModule, mdToken cl)
{
    CONTRACT(TypeHandle)
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
        PRECONDITION(CheckPointer(pModule));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END

    BAD_FORMAT_NOTHROW_ASSERT((TypeFromToken(cl) == mdtTypeRef ||
                       TypeFromToken(cl) == mdtTypeDef ||
                       TypeFromToken(cl) == mdtTypeSpec));

    if (TypeFromToken(cl) == mdtTypeDef)
        RETURN(pModule->LookupTypeDef(cl, CLASS_LOAD_UNRESTOREDTYPEKEY));
    else if (TypeFromToken(cl) == mdtTypeRef)
        RETURN(pModule->LookupTypeRef(cl, CLASS_LOAD_UNRESTOREDTYPEKEY));

    TypeHandle typeHandle;
    RETURN(typeHandle);
}

DomainAssembly *ClassLoader::GetDomainAssembly(AppDomain *pDomain/*=NULL*/)
{
    WRAPPER_CONTRACT;
    return GetAssembly()->GetDomainAssembly(pDomain);
}

#ifndef DACCESS_COMPILE

//
// Free all modules associated with this loader
//
void ClassLoader::FreeModules()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        DISABLED(FORBID_FAULT);  //Lots of crud to clean up to make this work
    }
    CONTRACTL_END;

    Module *pManifest = NULL;
    if (GetAssembly() && (NULL != (pManifest = GetAssembly()->GetManifestModule()))) {
        // Unload the manifest last, since it contains the module list in its rid map
        ModuleIterator i = GetAssembly()->IterateModules();
        while (i.Next()) {
            // Have the module free its various tables and some of the EEClass links
            if (i.GetModule() != pManifest)
                i.GetModule()->Destruct();
        }
        
        // Now do the manifest module.
        pManifest->Destruct();
    }

}

ClassLoader::~ClassLoader()
{
    CONTRACTL
    {
        NOTHROW;
        DESTRUCTOR_CHECK;
        GC_TRIGGERS;
        MODE_ANY;
        DISABLED(FORBID_FAULT);  //Lots of crud to clean up to make this work
    }
    CONTRACTL_END

#ifdef _DEBUG
    // Do not walk m_pUnresolvedClassHash at destruct time as it is loaderheap allocated memory
    // and may already have been deallocated via an AllocMemTracker.
    m_pUnresolvedClassHash = (PendingTypeLoadTable*)(UINT_PTR)0xcccccccc;
#endif

#ifdef _DEBUG
//     LOG((
//         LF_CLASSLOADER,
//         INFO3,
//         "Deleting classloader %x\n"
//         "  >EEClass data:     %10d bytes\n"
//         "  >Classname hash:   %10d bytes\n"
//         "  >FieldDesc data:   %10d bytes\n"
//         "  >MethodDesc data:  %10d bytes\n"
//         "  >Converted sigs:   %10d bytes\n"
//         "  >GCInfo:           %10d bytes\n"
//         "  >Interface maps:   %10d bytes\n"
//         "  >MethodTables:     %10d bytes\n"
//         "  >Vtables:          %10d bytes\n"
//         "  >Static fields:    %10d bytes\n"
//         "# methods:           %10d\n"
//         "# field descs:       %10d\n"
//         "# classes:           %10d\n"
//         "# dup intf slots:    %10d\n"
//         "# array classrefs:   %10d\n"
//         "Array class overhead:%10d bytes\n",
//         this,
//             m_dwEEClassData,
//             m_pAvailableClasses->m_dwDebugMemory,
//             m_dwFieldDescData,
//             m_dwMethodDescData,
//             m_dwDebugConvertedSigSize,
//             m_dwGCSize,
//             m_dwInterfaceMapSize,
//             m_dwMethodTableSize,
//             m_dwVtableData,
//             m_dwStaticFieldData,
//         m_dwDebugMethods,
//         m_dwDebugFieldDescs,
//         m_dwDebugClasses,
//         m_dwDebugDuplicateInterfaceSlots,
//         m_dwDebugArrayClassRefs,
//         m_dwDebugArrayClassSize
//     ));
#endif

    FreeModules();

    m_UnresolvedClassLock.Destroy();
    m_AvailableClassLock.Destroy();
    m_AvailableParamTypesLock.Destroy();
}


//----------------------------------------------------------------------------
// The constructor should only initialize enough to ensure that the destructor doesn't
// crash. It cannot allocate or do anything that might fail as that would leave
// the ClassLoader undestructable. Any such tasks should be done in ClassLoader::Init().
//----------------------------------------------------------------------------
ClassLoader::ClassLoader(Assembly *pAssembly)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END

    m_pAssembly = pAssembly;

    m_pUnresolvedClassHash          = NULL;
    m_cUnhashedModules              = 0;

    m_pNext                 = NULL;

#ifdef _DEBUG
    m_dwDebugMethods        = 0;
    m_dwDebugFieldDescs     = 0;
    m_dwDebugClasses        = 0;
    m_dwDebugArrayClassRefs = 0;
    m_dwDebugDuplicateInterfaceSlots = 0;
    m_dwDebugConvertedSigSize = 0;
    m_dwGCSize              = 0;
    m_dwInterfaceMapSize    = 0;
    m_dwMethodTableSize     = 0;
    m_dwVtableData          = 0;
    m_dwStaticFieldData     = 0;
    m_dwFieldDescData       = 0;
    m_dwMethodDescData      = 0;
    m_dwEEClassData         = 0;
#endif
}


//----------------------------------------------------------------------------
// This function completes the initialization of the ClassLoader. It can
// assume the constructor is run and that the function is entered with
// ClassLoader in a safely destructable state. This function can throw
// but whether it throws or succeeds, it must leave the ClassLoader in a safely
// destructable state.
//----------------------------------------------------------------------------
VOID ClassLoader::Init(AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END


    m_pUnresolvedClassHash = PendingTypeLoadTable::Create(GetAssembly()->GetLowFrequencyHeap(), 
                                                          UNRESOLVED_CLASS_HASH_BUCKETS, 
                                                          pamTracker);

    m_UnresolvedClassLock.Init("UnresolvedClassLock", CrstUnresolvedClassLock, CrstFlags(0));

    // This lock is taken within the classloader whenever we have to enter a
    // type in one of the modules governed by the loader.
    // The process of creating these types may be reentrant.  The ordering has
    // not yet been sorted out, and when we sort it out we should also modify the
    // ordering for m_AvailableParamTypesLock in BaseDomain.
    m_AvailableClassLock.Init("AvailableClassLock",
                                      CrstAvailableClass,
                                      CRST_REENTRANCY);

    // This lock is taken within the classloader whenever we have to insert a new param. type into the table
    // This lock also needs to be taken for a read operation in a GC_NOTRIGGER scope, thus the ANYMODE flag.
    m_AvailableParamTypesLock.Init("AvailableParamTypesLock",
                                   CrstAvailableParamTypes,
                                   (CrstFlags)(CRST_UNSAFE_ANYMODE | CRST_DEBUGGER_THREAD));

#ifdef _DEBUG
    CorTypeInfo::CheckConsistancy();
#endif


}


/*static*/
HRESULT ClassLoader::LoadTypeDefOrRefOrSpecNoThrow(Module *pModule,
                                                   mdToken typeDefOrRefOrSpec,
                                                   const SigTypeContext *pTypeContext,
                                                   TypeHandle *pthOut,
                                                   OBJECTREF *pThrowable,
                                                   NotFoundAction fNotFoundAction /* = ThrowIfNotFound */ ,
                                                   PermitUninstantiatedFlag fUninstantiated /* = FailIfUninstDefOrRef */ ,
                                                   LoadTypesFlag fLoadTypes/*=LoadTypes*/,
                                                   ClassLoadLevel level /*=CLASS_LOADED*/)
{
    STATIC_CONTRACT_NOTHROW;

    _ASSERTE(IsProtectedByGCFrame(pThrowable));

    _ASSERT(pThrowable == NULL || GetThread() != NULL);

    if(pthOut)
        *pthOut = TypeHandle();

    TypeHandle th;
    HRESULT hr = S_OK;
    BEGIN_EXCEPTION_GLUE(&hr, pThrowable)
    {
        th = LoadTypeDefOrRefOrSpecThrowing(pModule, typeDefOrRefOrSpec, pTypeContext, fNotFoundAction, fUninstantiated, fLoadTypes, level);
    }
    END_EXCEPTION_GLUE

    if(pthOut)
        *pthOut = th;
    return hr;
}

/*static*/
HRESULT ClassLoader::LoadTypeDefOrRefNoThrow(Module *pModule,
                                             mdToken typeDefOrRef,
                                           TypeHandle *pthOut,
                                           OBJECTREF *pThrowable,
                                           NotFoundAction fNotFoundAction /* = ThrowIfNotFound */ ,
                                           PermitUninstantiatedFlag fUninstantiated /* = FailIfUninstDefOrRef */ )
{
    STATIC_CONTRACT_NOTHROW;

    _ASSERTE(IsProtectedByGCFrame(pThrowable));

    _ASSERT(pThrowable == NULL || GetThread() != NULL);

    if(pthOut)
        *pthOut = TypeHandle();

    TypeHandle th;
    HRESULT hr = S_OK;
    BEGIN_EXCEPTION_GLUE(&hr, pThrowable)
    {
       th = LoadTypeDefOrRefThrowing(pModule, typeDefOrRef, fNotFoundAction, fUninstantiated);
    }
    END_EXCEPTION_GLUE

    if(pthOut)
        *pthOut = th;
    return hr;
}

#endif // #ifndef DACCESS_COMPILE

/*static*/
TypeHandle ClassLoader::LoadTypeDefOrRefOrSpecThrowing(Module *pModule,
                                                       mdToken typeDefOrRefOrSpec,
                                                       const SigTypeContext *pTypeContext,
                                                       NotFoundAction fNotFoundAction /* = ThrowIfNotFound */ ,
                                                       PermitUninstantiatedFlag fUninstantiated /* = FailIfUninstDefOrRef */,
                                                       LoadTypesFlag fLoadTypes/*=LoadTypes*/ ,
                                                       ClassLoadLevel level /* = CLASS_LOADED */,
                                                       BOOL dropGenericArgumentLevel /* = FALSE */,
                                                       const Substitution *pSubst)
{
    CONTRACT(TypeHandle)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        MODE_ANY;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
        PRECONDITION(FORBIDGC_LOADER_USE_ENABLED() || GetAppDomain()->CheckCanLoadTypes(pModule->GetAssembly()));
        POSTCONDITION(CheckPointer(RETVAL, (fNotFoundAction == ThrowIfNotFound)? NULL_NOT_OK : NULL_OK));
    }
    CONTRACT_END

    if (TypeFromToken(typeDefOrRefOrSpec) == mdtTypeSpec) 
    {
        ULONG cSig;
        PCCOR_SIGNATURE pSig;

        IMDInternalImport *pInternalImport = pModule->GetMDImport();
        pInternalImport->GetTypeSpecFromToken(typeDefOrRefOrSpec, &pSig, &cSig);
        SigPointer sigptr(pSig);
        TypeHandle typeHnd = sigptr.GetTypeHandleThrowing(pModule, pTypeContext, fLoadTypes, 
                                                          level, dropGenericArgumentLevel, pSubst);
#ifndef DACCESS_COMPILE
        if ((fNotFoundAction == ThrowIfNotFound) && typeHnd.IsNull())
            pModule->GetAssembly()->ThrowTypeLoadException(pInternalImport, typeDefOrRefOrSpec,
                                                           IDS_CLASSLOAD_GENERAL);
#endif
        RETURN (typeHnd);
    }
    else
    {
        RETURN (LoadTypeDefOrRefThrowing(pModule, typeDefOrRefOrSpec, 
                                         fNotFoundAction, 
                                         fUninstantiated,
                                         ((fLoadTypes == LoadTypes) ? tdNoTypes : tdAllTypes), 
                                         level));
    }
}

// Given a token specifying a typeDef, and a module in which to
// interpret that token, find or load the corresponding type handle.
//
//
/*static*/
TypeHandle ClassLoader::LoadTypeDefThrowing(Module *pModule,
                                            mdToken typeDef,
                                            NotFoundAction fNotFoundAction /* = ThrowIfNotFound */ ,
                                            PermitUninstantiatedFlag fUninstantiated /* = FailIfUninstDefOrRef */,
                                            mdToken tokenNotToLoad,
                                            ClassLoadLevel level)
{

    CONTRACT(TypeHandle)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        MODE_ANY;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
        PRECONDITION(FORBIDGC_LOADER_USE_ENABLED()
                     || GetAppDomain()->CheckCanLoadTypes(pModule->GetAssembly()));

        POSTCONDITION(CheckPointer(RETVAL, NameHandle::OKToLoad(typeDef, tokenNotToLoad) && (fNotFoundAction == ThrowIfNotFound) ? NULL_NOT_OK : NULL_OK));
        POSTCONDITION(RETVAL.IsNull() || RETVAL.GetModule() == pModule);
        POSTCONDITION(RETVAL.IsNull() || RETVAL.GetCl() == typeDef);
    }
    CONTRACT_END;

    TypeHandle typeHnd = TypeHandle();

    IMDInternalImport *pInternalImport = NULL;
    // We don't want to probe on any threads except for those with a managed thread.  This function
    // can be called from the GC thread etc. so need to control how we probe.
    INTERIOR_STACK_PROBE_NOTHROW_CHECK_THREAD(goto Exit;);

    {
        GCX_NOTRIGGER();
        CANNOTTHROWCOMPLUSEXCEPTION();

        // First, attempt to find the class if it is already loaded
        typeHnd = pModule->LookupTypeDef(typeDef, level);
        if (!typeHnd.IsNull())
        {
            goto Exit;
        }
    }

    pInternalImport = pModule->GetMDImport();

    if (IsNilToken(typeDef) || TypeFromToken(typeDef) != mdtTypeDef || !pInternalImport->IsValidToken(typeDef) ) {
#ifdef _DEBUG
        LOG((LF_CLASSLOADER, LL_INFO10, "Bogus class token to load: 0x%08x\n", typeDef));
#endif

        typeHnd = TypeHandle();
    }

    else 
    {
        // *****************************************************************************
        //
        //             Important invariant:
        //
        // The rule here is that we never go to LoadTypeHandleForTypeKey if a Find should succeed.
        // This is vital, because otherwise a stack crawl will open up opportunities for
        // GC.  Since operations like setting up a GCFrame will trigger a crawl in stress
        // mode, a GC at that point would be disastrous.  We can't assert this, because
        // of race conditions.  (In other words, the type could suddently be find-able
        // because another thread loaded it while we were in this method.

        // Not found - try to load it unless we are told not to

#ifndef DACCESS_COMPILE
        if (typeHnd.IsNull())
        {
            if ( !NameHandle::OKToLoad(typeDef, tokenNotToLoad) )
            {
                typeHnd = TypeHandle();
            }
            else 
            {
                // Anybody who puts himself in a FORBIDGC_LOADER state has promised
                // to use us only for resolving, not loading. We are now transitioning into
                // loading.
#ifdef _DEBUG_IMPL
                _ASSERTE(!FORBIDGC_LOADER_USE_ENABLED());
#endif
                TRIGGERSGC();
                
                if (pModule->IsReflection()) 
                {
                    //if (!(pModule->IsIntrospectionOnly()))
                    {
                        // Don't try to load types that are not in available table, when this
                        // is an in-memory module.  Raise the type-resolve event instead.
                        typeHnd = TypeHandle();
                        
                        // Avoid infinite recursion
                        if (tokenNotToLoad != tdAllAssemblies) 
                        {
                            AppDomain* pDomain = SystemDomain::GetCurrentDomain();
                            
                            LPUTF8 pszFullName;
                            LPCUTF8 className;
                            LPCUTF8 nameSpace;
                            pInternalImport->GetNameOfTypeDef(typeDef, &className, &nameSpace);
                            MAKE_FULL_PATH_ON_STACK_UTF8(pszFullName,
                                                         nameSpace,
                                                         className);
                            
                            DomainAssembly *pDomainAssembly = pDomain->RaiseTypeResolveEventThrowing(pszFullName);
                            if (pDomainAssembly) 
                            {
                                Assembly *pAssembly = pDomainAssembly->GetAssembly();

                                NameHandle name(nameSpace, className);
                                name.SetTypeToken(pModule, typeDef);
                                name.SetTokenNotToLoad(tdAllAssemblies);
                                typeHnd = pAssembly->GetLoader()->LoadTypeHandleThrowing(&name, level);
                            }
                        }
                    }
                }
                else 
                {
                    GCX_PREEMP();
                    TypeKey typeKey(pModule, typeDef);
                    typeHnd = pModule->GetClassLoader()->LoadTypeHandleForTypeKey(&typeKey, 
                                                                                  typeHnd,
                                                                                  level);
                }
            }
        }
#endif // !DACCESS_COMPILE
    }

Exit:
#ifndef DACCESS_COMPILE
    if ((fUninstantiated == FailIfUninstDefOrRef) && !typeHnd.IsNull() && typeHnd.IsGenericTypeDefinition())
    {
        typeHnd = TypeHandle();
    }

    if ((fNotFoundAction == ThrowIfNotFound) && typeHnd.IsNull() && (tokenNotToLoad != tdAllTypes))
        pModule->GetAssembly()->ThrowTypeLoadException(pModule->GetMDImport(), 
                                                       typeDef,
                                                       IDS_CLASSLOAD_GENERAL);
#endif
    ;
    END_INTERIOR_STACK_PROBE;
    
    RETURN(typeHnd);
}


// Given a token specifying a typeDef or typeRef, and a module in
// which to interpret that token, find or load the corresponding type
// handle.
//
/*static*/
TypeHandle ClassLoader::LoadTypeDefOrRefThrowing(Module *pModule,
                                                 mdToken typeDefOrRef,
                                                 NotFoundAction fNotFoundAction /* = ThrowIfNotFound */ ,
                                                 PermitUninstantiatedFlag fUninstantiated /* = FailIfUninstDefOrRef */,
                                                 mdToken tokenNotToLoad,
                                                 ClassLoadLevel level)
{

    CONTRACT(TypeHandle)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        MODE_ANY;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(level > CLASS_LOAD_BEGIN && level <= CLASS_LOADED);
        PRECONDITION(FORBIDGC_LOADER_USE_ENABLED()
                     || GetAppDomain()->CheckCanLoadTypes(pModule->GetAssembly()));

        POSTCONDITION(CheckPointer(RETVAL, NameHandle::OKToLoad(typeDefOrRef, tokenNotToLoad) && (fNotFoundAction == ThrowIfNotFound) ? NULL_NOT_OK : NULL_OK));
    }
    CONTRACT_END;

    // First, attempt to find the class if it is already loaded
    TypeHandle typeHnd = LookupTypeDefOrRefInModule(pModule, typeDefOrRef);
    if (!typeHnd.IsNull())
    {
        if (typeHnd.GetLoadLevel() < level)
        {
            pModule = typeHnd.GetModule();
            typeDefOrRef = typeHnd.GetCl();
        }
    }

    if (!typeHnd.IsNull() && typeHnd.GetLoadLevel() >= level)
    {
        // perform the check that it's not an uninstantiated TypeDef/TypeRef
        // being used inappropriately.
        if (!((fUninstantiated == FailIfUninstDefOrRef) && !typeHnd.IsNull() && typeHnd.IsGenericTypeDefinition()))
        {
            RETURN(typeHnd);
        }
    }
    else
    {
        // otherwise try to resolve the TypeRef and/or load the corresponding TypeDef
        IMDInternalImport *pInternalImport = pModule->GetMDImport();
        mdToken tokType = TypeFromToken(typeDefOrRef);
        
        if (IsNilToken(typeDefOrRef) || ((tokType != mdtTypeDef)&&(tokType != mdtTypeRef))
            || !pInternalImport->IsValidToken(typeDefOrRef) ) 
        {
#ifdef _DEBUG
            LOG((LF_CLASSLOADER, LL_INFO10, "Bogus class token to load: 0x%08x\n", typeDefOrRef));
#endif
            
            typeHnd = TypeHandle();
        }
        
        else if (tokType == mdtTypeRef) 
        {
            BOOL fNoResolutionScope;
            Module *pFoundModule = Assembly::FindModuleByTypeRef(pModule, typeDefOrRef,
                                                                 tokenNotToLoad==tdAllTypes?Loader::DontLoad:Loader::Load,
                                                                 &fNoResolutionScope);

            if (pFoundModule) 
            {
                
                // Not in my module, have to look it up by name.  This is the primary path
                // taken by the TypeRef case, i.e. we've resolve a TypeRef to a TypeDef/Module
                // pair.
                LPCUTF8 pszNameSpace;
                LPCUTF8 pszClassName;
                pInternalImport->GetNameOfTypeRef(typeDefOrRef,
                                                  &pszNameSpace,
                                                  &pszClassName);
                if(fNoResolutionScope)
                {
                    typeHnd = ClassLoader::LoadTypeByNameThrowing(pFoundModule->GetAssembly(),
                                                                  pszNameSpace,
                                                                  pszClassName,
                                                                  ClassLoader::ReturnNullIfNotFound,
                                                                  tokenNotToLoad==tdAllTypes ? ClassLoader::DontLoadTypes : ClassLoader::LoadTypes,
                                                                  level);
                    if(typeHnd.IsNull())
                    {
                        fNotFoundAction = ReturnNullIfNotFound;
                        RETURN(typeHnd);
                    }
                }
                else
                {
                    NameHandle nameHandle(pModule, typeDefOrRef);
                    nameHandle.SetName(pszNameSpace, pszClassName);
                    nameHandle.SetTokenNotToLoad(tokenNotToLoad);
                    typeHnd = pFoundModule->GetClassLoader()->
                        LoadTypeHandleThrowIfFailed(&nameHandle, level,
                                                    pFoundModule->IsReflection() ? NULL : pFoundModule);
                }

#ifndef DACCESS_COMPILE
                if (!(typeHnd.IsNull()))
                    pModule->StoreTypeRef(typeDefOrRef, typeHnd);
#endif
            }
        }
        else 
            // This is the mdtTypeDef case...
            typeHnd = LoadTypeDefThrowing(pModule, typeDefOrRef, 
                                          fNotFoundAction, 
                                          fUninstantiated, 
                                          tokenNotToLoad, 
                                          level);
    }
    TypeHandle thRes = typeHnd;

    // reject the load if it's an uninstantiated TypeDef/TypeRef
    // being used inappropriately.  
    if ((fUninstantiated == FailIfUninstDefOrRef) && !typeHnd.IsNull() && typeHnd.IsGenericTypeDefinition())
        thRes = TypeHandle();
    
    // perform the check to throw when the thing is not found
    if ((fNotFoundAction == ThrowIfNotFound) && thRes.IsNull() && (tokenNotToLoad != tdAllTypes))
    {
#ifndef DACCESS_COMPILE
        pModule->GetAssembly()->ThrowTypeLoadException(pModule->GetMDImport(),
                                                       typeDefOrRef,
                                                       IDS_CLASSLOAD_GENERAL);
#else
        DacNotImpl();
#endif
    }
    RETURN(thRes);
}

/*static*/
BOOL ClassLoader::ResolveTokenToTypeDefThrowing(Module *pTypeRefModule,
                                                mdTypeRef typeRefToken,
                                                Module **ppTypeDefModule,
                                                mdTypeDef *pTypeDefToken,
                                                Loader::LoadFlag loadFlag)
{
    CONTRACT(BOOL)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        MODE_ANY;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        PRECONDITION(CheckPointer(pTypeRefModule));
    }
    CONTRACT_END;

    // It's a TypeDef already
    if (TypeFromToken(typeRefToken) == mdtTypeDef)
    {
        if (ppTypeDefModule)
            *ppTypeDefModule = pTypeRefModule;
        if (pTypeDefToken)
            *pTypeDefToken = typeRefToken;
        RETURN TRUE;
    }

    TypeHandle typeHnd = pTypeRefModule->LookupTypeRef(typeRefToken, CLASS_LOAD_UNRESTOREDTYPEKEY);

    // Type is already (partially) loaded and cached in the module's TypeRef table
    if (!typeHnd.IsNull())
    {
        if (ppTypeDefModule)
            *ppTypeDefModule = typeHnd.GetModule();
        if (pTypeDefToken)
            *pTypeDefToken = typeHnd.GetCl();
        RETURN TRUE;
    }

    BOOL fNoResolutionScope; //not used
    Module *pFoundRefModule = Assembly::FindModuleByTypeRef(pTypeRefModule, 
                                                            typeRefToken, 
                                                            loadFlag,
                                                            &fNoResolutionScope);

    if (pFoundRefModule) 
    {
        // Not in my module, have to look it up by name
        LPCUTF8 pszNameSpace;
        LPCUTF8 pszClassName;
        pTypeRefModule->GetMDImport()->GetNameOfTypeRef(typeRefToken, &pszNameSpace, &pszClassName);
        NameHandle nameHandle(pTypeRefModule, typeRefToken);
        nameHandle.SetName(pszNameSpace, pszClassName);
        if(loadFlag!=Loader::Load)
            nameHandle.SetTokenNotToLoad(tdAllTypes);

        mdToken     foundTypeDef = mdTokenNil;
        Module*     pFoundModule = NULL;
        mdExportedType foundExportedType = mdTokenNil;
        if (!pFoundRefModule->GetClassLoader()->FindClassModuleThrowing(&nameHandle,
                                                                        &typeHnd,
                                                                        &foundTypeDef,
                                                                        &pFoundModule,
                                                                        &foundExportedType,
                                                                        NULL,
                                                                        pFoundRefModule->IsReflection() ? NULL : pFoundRefModule,
                                                                        loadFlag))
            RETURN FALSE;

        // Type is already loaded and cached in the loader's by-name table
        if (!typeHnd.IsNull())
        {
            if (ppTypeDefModule)
                *ppTypeDefModule = typeHnd.GetModule();
            if (pTypeDefToken)
                *pTypeDefToken = typeHnd.GetCl();
            RETURN TRUE;
        }

        // The type is not loaded but it has been resolved to a token/Module pair
        // All that remains is to check validity
        MEMORY_REPORT_ASSEMBLY_SCOPE(pFoundModule);

        Assembly *pAssembly = pTypeRefModule->GetAssembly();
        // It's in the same assembly as the TypeRef
        if(pFoundModule->GetAssembly() == pAssembly) 
        {
            // Do we need to verify the token?
#ifndef DACCESS_COMPILE
            BOOL fVerifyTD = foundExportedType != mdTokenNil &&
                !(pAssembly->IsStrongNamed() || Security::IsSigned(pAssembly->GetSecurityDescriptor()));
                
            // Verify that foundTypeDef is a valid token for pFoundModule, because
            // It may be just the hint saved in an ExportedType in another scope
            if (fVerifyTD) 
            {
                // First check that the RID is valid
                // Why can't we use IsValidToken here?
                HENUMInternal phTDEnum;
                DWORD dwElements = 0;
                BOOL fTrustTD = TRUE;
                if (pFoundModule->GetMDImport()->EnumTypeDefInit(&phTDEnum) == S_OK) 
                {
                    dwElements = pFoundModule->GetMDImport()->EnumGetCount(&phTDEnum);
                    pFoundModule->GetMDImport()->EnumTypeDefClose(&phTDEnum);
                    // assumes max rid is incremented by one for globals (0x02000001)
                    if (RidFromToken(foundTypeDef) > dwElements+1)
                        fTrustTD = FALSE;
                }
                
                // Now check that the names match
                if (fTrustTD)
                {
                    LPCUTF8 foundClassName;
                    LPCUTF8 foundNameSpace;
                    pFoundModule->GetMDImport()->GetNameOfTypeDef(foundTypeDef, &foundClassName, &foundNameSpace);
                    fTrustTD = strcmp(foundClassName, pszClassName) == 0 && strcmp(foundNameSpace, pszNameSpace) == 0;
                }
        
                // If we can't trust it, look it up by exported type
                if (!fTrustTD)
                {
                    if (!SUCCEEDED(FindTypeDefByExportedType(pAssembly->GetManifestImport(),
                                                             foundExportedType,
                                                             pFoundModule->GetMDImport(),
                                                             &foundTypeDef))) 
                        RETURN FALSE;
                }
            }
#endif // #ifndef DACCESS_COMPILE
        }
        else 
        // It's not in the same assembly as the TypeRef
        { 
            /*
            //                                                                    
            if ((FoundExportedType != mdTokenNil) &&
                (!typeHnd.IsNull()) &&
                typeHnd.IsGenericTypeDefinition()) {
                DWORD dwFlags;
                m_pAssembly->GetManifestImport()->GetExportedTypeProps(fFoundExportedType,
                                                                       NULL,  // namespace
                                                                       NULL,  // name
                                                                       NULL, // impl token
                                                                       NULL,  // TypeDef
                                                                       &dwFlags);
                if (IsTdForwarder(dwFlags)) {
                    THROW_BAD_FORMAT_MAYBE(!"Forwarding of generic types not supported", BFA_NO_FWD_GEN_TYPES, pTypeRefModule); 
                    RETURN FALSE;
                }
            }
            */
        }
        
        if (pTypeDefToken)
            *pTypeDefToken = foundTypeDef;
        if (ppTypeDefModule)
            *ppTypeDefModule = pFoundModule;
        RETURN TRUE;
    }

    // We didn't find it anywhere
    RETURN FALSE;
}    

#ifndef DACCESS_COMPILE

/*static*/
VOID ClassLoader::GetEnclosingClassThrowing(IMDInternalImport *pInternalImport, Module *pModule, mdTypeDef cl, mdTypeDef *tdEnclosing)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE(tdEnclosing);
    *tdEnclosing = mdTypeDefNil;

    HRESULT hr = pInternalImport->GetNestedClassProps(cl, tdEnclosing);

    if (FAILED(hr)) {
        if (hr != CLDB_E_RECORD_NOTFOUND)
            COMPlusThrowHR(hr);
        return;
    }

    if (TypeFromToken(*tdEnclosing) != mdtTypeDef)
        pModule->GetAssembly()->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_ENCLOSING);
}


// Load a parent type or implemented interface type.
//
// If this is an instantiated type represented by a type spec, then instead of attempting to load the
// exact type, load an approximate instantiation in which all reference types are replaced by Object.
// The exact instantiated types will be loaded later by LoadInstantiatedInfo.
// We do this to avoid cycles early in class loading caused by definitions such as
//   struct M : ICloneable<M>                     // load ICloneable<object>
//   class C<T> : D<C<T>,int> for any T           // load D<object,int>
/*static*/
TypeHandle ClassLoader::LoadApproxTypeThrowing(Module *pModule, 
                                               mdToken tok,
                                               PCCOR_SIGNATURE *sigInst, 
                                               const SigTypeContext *pClassTypeContext)
{

    CONTRACT(TypeHandle)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
        PRECONDITION(CheckPointer(sigInst, NULL_OK));
        PRECONDITION(CheckPointer(pModule));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    IMDInternalImport *pInternalImport = pModule->GetMDImport();

    if (TypeFromToken(tok) == mdtTypeSpec) {
        ULONG cSig;
        PCCOR_SIGNATURE pSig;
        pInternalImport->GetTypeSpecFromToken(tok, &pSig, &cSig);
        SigPointer sigptr = SigPointer(pSig, cSig);
        CorElementType type;
        IfFailThrowBF(sigptr.GetElemType(&type), BFA_BAD_SIGNATURE, pModule);

        // The only kind of type specs that we recognise are instantiated types
        if (type != ELEMENT_TYPE_GENERICINST)
            pModule->GetAssembly()->ThrowTypeLoadException(pInternalImport, tok, IDS_CLASSLOAD_GENERAL);

        // Of these, we outlaw instantiated value classes (they can't be interfaces and can't be subclassed)
        IfFailThrowBF(sigptr.GetElemType(&type), BFA_BAD_SIGNATURE, pModule);
     
        if (type != ELEMENT_TYPE_CLASS)
            pModule->GetAssembly()->ThrowTypeLoadException(pInternalImport, tok, IDS_CLASSLOAD_GENERAL);

        mdToken genericTok;
        IfFailThrowBF(sigptr.GetToken(&genericTok), BFA_BAD_SIGNATURE, pModule);
        IfFailThrowBF(sigptr.GetData(NULL), BFA_BAD_SIGNATURE, pModule);

        if (sigInst)
            *sigInst = sigptr.GetPtr();

        // Try to load the generic type itself
        THROW_BAD_FORMAT_MAYBE(TypeFromToken(genericTok) == mdtTypeRef || TypeFromToken(genericTok) == mdtTypeDef, BFA_UNEXPECTED_GENERIC_TOKENTYPE, pModule);
        TypeHandle genericTypeTH = LoadTypeDefOrRefThrowing(pModule, genericTok, 
                                                            ClassLoader::ThrowIfNotFound,
                                                            ClassLoader::PermitUninstDefOrRef,
                                                            tdNoTypes,
                                                            CLASS_LOAD_APPROXPARENTS);

        // We load interfaces at very approximate types - the generic
        // interface itself.  We fix this up in LoadInstantiatedInfo.
        // This allows us to load recursive interfaces on structs such
        // as "struct VC : I<VC>".  The details of the interface
        // are not currently needed during the first phase
        // of setting up the method table.
        if (genericTypeTH.IsInterface())
        {
            RETURN genericTypeTH;
        }
        else            
        {
            // approxTypes, i.e. approximate reference types by Object, i.e. load the canonical type
            RETURN (SigPointer(pSig).GetTypeHandleThrowing(pModule, pClassTypeContext, ClassLoader::LoadTypes, CLASS_LOAD_APPROXPARENTS, TRUE /*dropGenericArgumentLevel*/));
        }
    }
    else 
    {
        if (sigInst)
            *sigInst = NULL;
        RETURN LoadTypeDefOrRefThrowing(pModule, tok, 
                                        ClassLoader::ThrowIfNotFound,
                                        ClassLoader::FailIfUninstDefOrRef,
                                        tdNoTypes,
                                        CLASS_LOAD_APPROXPARENTS);
    }
}


/*static*/
MethodTable* ClassLoader::LoadApproxParentThrowing(Module *pModule,
                                                   mdToken cl,
                                                   PCCOR_SIGNATURE *pParentInst,
                                                   const SigTypeContext *pClassTypeContext)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END;

    mdTypeRef   crExtends;
    MethodTable *pParentMethodTable = NULL;
    TypeHandle  parentType;
    DWORD       dwAttrClass;
    Assembly *pAssembly = pModule->GetAssembly();
    IMDInternalImport* pInternalImport = pModule->GetMDImport();

    // Initialize the return value;
    *pParentInst = NULL;

    // Now load all dependencies of this class
    pInternalImport->GetTypeDefProps(
        cl,
        &dwAttrClass, // AttrClass
        &crExtends
    );

    if (RidFromToken(crExtends) == mdTokenNil) {
//          if(cl == COR_GLOBAL_PARENT_TOKEN)
//              pParentMethodTable = g_pObjectClass;
    }
    else {
        // Do an "approximate" load of the parent, replacing reference types in the instantiation by Object
        // This is to avoid cycles in the loader e.g. on class C : D<C> or class C<T> : D<C<T>>
        // We fix up the exact parent later in LoadInstantiatedInfo
        parentType = LoadApproxTypeThrowing(pModule, crExtends, pParentInst, pClassTypeContext);

        pParentMethodTable = parentType.GetMethodTable();

        if (pParentMethodTable == NULL)
            pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_PARENTNULL);

        // cannot inherit from an interface
        if (pParentMethodTable->IsInterface())
            pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_PARENTINTERFACE);

        if (IsTdInterface(dwAttrClass)) {
            // Interfaces must extend from Object
            if (! pParentMethodTable->IsObjectClass())
                pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_INTERFACEOBJECT);
        }
    }

    return pParentMethodTable;
}

#ifndef DACCESS_COMPILE
// Perform a single phase of class loading
// It is the caller's responsibility to lock
/*static*/
TypeHandle ClassLoader::DoIncrementalLoad(TypeKey *pTypeKey, TypeHandle typeHnd, ClassLoadLevel currentLevel)
{
    CONTRACTL
    {        
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pTypeKey));
        PRECONDITION(currentLevel >= CLASS_LOAD_BEGIN && currentLevel < CLASS_LOADED);
        MODE_ANY;
    }
    CONTRACTL_END;

    INTERIOR_STACK_PROBE(GetThread());

#ifdef _DEBUG
    if (LoggingOn(LF_CLASSLOADER, LL_INFO10000))
    {
        SString name;
        TypeString::AppendTypeKeyDebug(name, pTypeKey);
        LOG((LF_CLASSLOADER, LL_INFO10000, "PHASEDLOAD: About to do incremental load of type %S (%p) from level %s\n", name.GetUnicode(), typeHnd.AsPtr(), classLoadLevelName[currentLevel]));
    }
#endif

    // Level is BEGIN if and only if type handle is null
    CONSISTENCY_CHECK((currentLevel == CLASS_LOAD_BEGIN) == typeHnd.IsNull());

    switch (currentLevel)
    {
        // Attain at least level CLASS_LOAD_UNRESTORED (if just locating type in ngen image)
        // or at least level CLASS_LOAD_APPROXPARENTS (if creating type for the first time)
        case CLASS_LOAD_BEGIN :
            {
                AllocMemTracker amTracker;
                typeHnd = CreateTypeHandleForTypeKey(pTypeKey, &amTracker);
                CONSISTENCY_CHECK(!typeHnd.IsNull());
                PublishType(pTypeKey, typeHnd);
                amTracker.SuppressRelease();
            }
            break;

        case CLASS_LOAD_UNRESTOREDTYPEKEY :
            break;

        // Attain level CLASS_LOAD_APPROXPARENTS, starting with unrestored class
        case CLASS_LOAD_UNRESTORED :
            break;

        // Attain level CLASS_LOAD_EXACTPARENTS
        case CLASS_LOAD_APPROXPARENTS :
            if (!typeHnd.IsTypeDesc())
            {
                LoadExactParents(typeHnd.AsMethodTable());
            }
            break;

        case CLASS_LOAD_EXACTPARENTS :
        case CLASS_DEPENDENCIES_LOADED :
        case CLASS_LOADED :
            break;

    }

    if (typeHnd.GetLoadLevel() >= CLASS_LOAD_EXACTPARENTS)
        Notify(typeHnd);

    END_INTERIOR_STACK_PROBE;
    
    return typeHnd;
}

/*static*/
// For non-canonical instantiations of generic types, create a fresh type by replicating the canonical instantiation
// For canonical instantiations of generic types, create a brand new method table
// For other constructed types, create a type desc and template method table if necessary
// For all other types, create a method table
TypeHandle ClassLoader::CreateTypeHandleForTypeKey(TypeKey* pKey, AllocMemTracker* pamTracker)
{
    CONTRACT(TypeHandle)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pKey));
      
        POSTCONDITION(RETVAL.CheckMatchesKey(pKey));
        MODE_ANY;
    }
    CONTRACT_END

    TypeHandle typeHnd = TypeHandle();
    
    if (!pKey->IsConstructed())
    {
        typeHnd = CreateTypeHandleForTypeDefThrowing(pKey->GetModule(),
                                                     pKey->GetTypeToken(),
                                                     pKey->GetInstantiation(),
                                                     pamTracker);
    }
    else if (pKey->HasInstantiation())
    {
        {
            if (TypeHandle::GetInstantiationSharingInfo(pKey->GetNumGenericArgs(), pKey->GetInstantiation()) != TypeHandle::NonCanonicalShared)
            {
                typeHnd = CreateTypeHandleForTypeDefThrowing(pKey->GetModule(),
                                                                pKey->GetTypeToken(),
                                                                pKey->GetInstantiation(),
                                                                pamTracker);
            }
            else 
            {
                typeHnd = ClassLoader::CreateTypeHandleForNonCanonicalGenericInstantiation(pKey,
                                                                                            pamTracker);
            }
#ifdef _DEBUG
            if (Nullable::IsNullableType(typeHnd)) 
                Nullable::CheckFieldOffsets(typeHnd);
#endif
        }
    }
    else if (pKey->GetKind() == ELEMENT_TYPE_FNPTR) 
    {
        Module *pLoaderModule = ComputeLoaderModule(pKey);
        PREFIX_ASSUME(pLoaderModule!=NULL);
        DWORD numArgs = pKey->GetNumArgs();
        BYTE* mem = (BYTE*) pamTracker->Track(pLoaderModule->GetAssembly()->GetLowFrequencyHeap()->AllocMem(sizeof(FnPtrTypeDesc) + sizeof(TypeHandle) * numArgs));
            
        typeHnd = TypeHandle(new(mem)  FnPtrTypeDesc(pKey->GetCallConv(), numArgs, pKey->GetRetAndArgTypes()));
    }
    else 
    {            
        Module *pLoaderModule = ComputeLoaderModule(pKey);
        PREFIX_ASSUME(pLoaderModule!=NULL);

        CorElementType kind = pKey->GetKind();
        TypeHandle paramType = pKey->GetElementType();
        MethodTable *templateMT;
        
        // Create a new type descriptor and insert into constructed type table
        if (CorTypeInfo::IsArray(kind)) 
        {                
            DWORD rank = pKey->GetRank();                
            THROW_BAD_FORMAT_MAYBE((kind != ELEMENT_TYPE_ARRAY) || rank > 0, BFA_MDARRAY_BADRANK, pLoaderModule);
            THROW_BAD_FORMAT_MAYBE((kind != ELEMENT_TYPE_SZARRAY) || rank == 1, BFA_SDARRAY_BADRANK, pLoaderModule);
            
            // Arrays of BYREFS not allowed
            if (paramType.GetInternalCorElementType() == ELEMENT_TYPE_BYREF || 
                paramType.GetInternalCorElementType() == ELEMENT_TYPE_TYPEDBYREF)
            {
                ThrowTypeLoadException(pKey, IDS_CLASSLOAD_CANTCREATEARRAYCLASS);
            }
                
            // We really don't need this check anymore.
            if (rank > MAX_RANK)
            {
                ThrowTypeLoadException(pKey, IDS_CLASSLOAD_RANK_TOOLARGE);
            }
                
            templateMT = pLoaderModule->CreateArrayMethodTable(paramType, kind, rank, pamTracker);
        }
        else 
        {            
            // no parameterized type allowed on a reference
            if (paramType.GetInternalCorElementType() == ELEMENT_TYPE_BYREF || 
                paramType.GetInternalCorElementType() == ELEMENT_TYPE_TYPEDBYREF)
            {
                ThrowTypeLoadException(pKey, IDS_CLASSLOAD_GENERAL);
            }
                
            // let <Type>* type have a method table
            // System.UIntPtr's method table is used for types like int*, void *, string * etc.
            if (kind == ELEMENT_TYPE_PTR)
                templateMT = TheUIntPtrClass();
            else 
                templateMT = NULL;
        }
            
        BYTE* mem = (BYTE*) pamTracker->Track(pLoaderModule->GetAssembly()->GetLowFrequencyHeap()->AllocMem(sizeof(ParamTypeDesc)));
            
        typeHnd = TypeHandle(new(mem)  ParamTypeDesc(kind, templateMT, paramType));
    }

    RETURN typeHnd;
}
#endif // #ifndef DACCESS_COMPILE

// Publish a type (and possibly member information) in the loader's
// tables Types are published before they are fully loaded. In
// particular, exact parent info (base class and interfaces) is loaded
// in a later phase
/*static*/
void ClassLoader::PublishType(TypeKey *pTypeKey, TypeHandle typeHnd)
{
    CONTRACTL
    {        
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
        PRECONDITION(CheckPointer(typeHnd));
        PRECONDITION(CheckPointer(pTypeKey));

        // Key must match that of the handle
        PRECONDITION(typeHnd.CheckMatchesKey(pTypeKey));

        // Don't publish array template method tables; these are accessed only through type descs
        PRECONDITION(typeHnd.IsTypeDesc() || !typeHnd.AsMethodTable()->IsArray());
    }
    CONTRACTL_END;


    if (pTypeKey->IsConstructed())
    {
        {
            Module *pLoaderModule = ComputeLoaderModule(pTypeKey);
            EETypeHashTable *pTable = pLoaderModule->GetAvailableParamTypes();

            // This lock was changed to be have the CRST_UNSAFE_ANYMODE bit set, which does not
            // interact well with CrstPreempHolder. But we still want to go into preemp mode
            // when taking the lock at this location; the bit was set for taking the lock without
            // toggling in another method (see LookupTypeKeyUnderLock). So, call GCX_PREEMP to make
            // sure we're as close to the original behaviour as possible.
            GCX_PREEMP();

            CrstHolder ch(&pLoaderModule->GetClassLoader()->m_AvailableParamTypesLock);
            pTable->InsertValue(typeHnd);
        }

        // For arrays, perhaps also publish in global table of primitive array types
        if (pTypeKey->GetKind() == ELEMENT_TYPE_SZARRAY) 
        {
            TypeHandle paramType = pTypeKey->GetElementType();
            CorElementType type = paramType.GetSignatureCorElementType();
            if (type <= ELEMENT_TYPE_R8) 
            {
                _ASSERTE(g_pPredefinedArrayTypes[type] == 0 || g_pPredefinedArrayTypes[type] == typeHnd.AsArray());
                g_pPredefinedArrayTypes[type] = typeHnd.AsArray();
            }
            else if (paramType.GetMethodTable() == g_pObjectClass) 
            {
                _ASSERTE(g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT] == 0 || g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT] == typeHnd.AsArray());
                g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT] = typeHnd.AsArray();
            }
            else if (paramType.GetMethodTable() == g_pStringClass) 
            {
                _ASSERTE(g_pPredefinedArrayTypes[ELEMENT_TYPE_STRING] == 0 || g_pPredefinedArrayTypes[ELEMENT_TYPE_STRING] == typeHnd.AsArray());
                g_pPredefinedArrayTypes[ELEMENT_TYPE_STRING] = typeHnd.AsArray();
            }
        }                        
    }
    else
    {       
        // ! We cannot fail after this point.
        CANNOTTHROWCOMPLUSEXCEPTION();
        FAULT_FORBID();

        Module *pModule = pTypeKey->GetModule();
        mdTypeDef typeDef = pTypeKey->GetTypeToken();

        CONSISTENCY_CHECK_MSG(pModule->LookupTypeDef(typeDef).IsNull(),
                              "Module's TypeDef map already has this type\n");
        pModule->EnsuredStoreTypeDef(typeDef, typeHnd);
        MethodTable *pMT = typeHnd.AsMethodTable();

        /* It would be cleaner to use the MethodIterator interface here but it allocates, unfortunately */
        MethodDescChunk* pChunk = pMT->GetClass()->GetChunks();
        while (pChunk != NULL)
        {
            DWORD n = pChunk->GetCount();
            for (DWORD i = 0; i < n; i++)
            {
                MethodDesc *pMD = pChunk->GetMethodDescAt(i);
                CONSISTENCY_CHECK(pMD != NULL && pMD->GetMethodTable() == pMT);
                if (!pMD->IsUnboxingStub())
                {
                    pModule->EnsuredStoreMethodDef(pMD->GetMemberDef(), pMD);
                }
            }
            pChunk = pChunk->GetNextChunk();            
        }

        ApproxFieldDescIterator fdIterator(pMT, ApproxFieldDescIterator::ALL_FIELDS);
        FieldDesc* pFD;
    
        while ((pFD = fdIterator.Next()) != NULL)
        {
            if (pFD->GetEnclosingMethodTable() == pMT)
            {
                pModule->EnsuredStoreFieldDef(pFD->GetMemberDef(), pFD);
            }
        }
    }
}

// Notify profiler and debugger that a type load has completed
// Also adjust perf counters
/*static*/
void ClassLoader::Notify(TypeHandle typeHnd)
{
    CONTRACTL
    {        
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
        PRECONDITION(CheckPointer(typeHnd));
    }
    CONTRACTL_END;


    LOG((LF_CLASSLOADER, LL_INFO1000, "Notify: %p %s\n", typeHnd.AsPtr(), typeHnd.IsTypeDesc() ? "typedesc" : typeHnd.GetMethodTable()->GetDebugClassName()));

#ifdef PROFILING_SUPPORTED
    if (!typeHnd.IsTypeDesc() && !typeHnd.ContainsGenericVariables()  && CORProfilerTrackClasses()) 
    {
        LOG((LF_CLASSLOADER, LL_INFO1000, "Notifying profiler of Started1 %p %s\n", typeHnd.AsPtr(), typeHnd.GetMethodTable()->GetDebugClassName()));
        // Record successful load of the class for the profiler
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->ClassLoadStarted((ThreadID) GetThread(),
                                                            TypeHandleToTypeID(typeHnd));

        //
        // Profiler can turn off TrackClasses during the Started() callback.  Need to
        // retest the flag here.
        //
        if (CORProfilerTrackClasses()) 
        {
        LOG((LF_CLASSLOADER, LL_INFO1000, "Notifying profiler of Finished1 %p %s\n", typeHnd.AsPtr(), typeHnd.GetMethodTable()->GetDebugClassName()));
            g_profControlBlock.pProfInterface->ClassLoadFinished((ThreadID) GetThread(),
                                                                 TypeHandleToTypeID(typeHnd),
                                                                 S_OK);
        }
    }
#endif //PROFILING_SUPPORTED
    g_IBCLogger.LogTypeMethodTableAccess(&typeHnd);

    if (!typeHnd.IsTypeDesc() && typeHnd.GetMethodTable()->IsTypicalTypeDefinition())
    {
        LOG((LF_CLASSLOADER, LL_INFO100, "Successfully loaded class %s\n", typeHnd.GetMethodTable()->GetDebugClassName()));

#ifdef DEBUGGING_SUPPORTED
        if (CORDebuggerAttached()) 
        {
            LOG((LF_CORDB, LL_EVERYTHING, "NotifyDebuggerLoad clsload 2239 class %s\n", typeHnd.GetMethodTable()->GetDebugClassName()));
            TypeHandle(typeHnd.GetMethodTable()).NotifyDebuggerLoad(NULL, FALSE);
        }
#endif // DEBUGGING_SUPPORTED

#if defined(ENABLE_PERF_COUNTERS)
        GetPrivatePerfCounters().m_Loading.cClassesLoaded ++;
#endif

    }
}


//-----------------------------------------------------------------------------
// Common helper for LoadTypeHandleForTypeKey and LoadTypeHandleForTypeKeyNoLock.
// Makes the root level call to kick off the transitive closure walk for
// the final level pushes.
//-----------------------------------------------------------------------------
static void PushFinalLevels(TypeHandle typeHnd, ClassLoadLevel targetLevel)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END


    // This phase brings the type and all its transitive dependencies to their
    // final state, sans the IsFullyLoaded bit.
    if (targetLevel >= CLASS_DEPENDENCIES_LOADED)
    {
        BOOL fBailed = FALSE;
        typeHnd.DoFullyLoad(NULL, CLASS_DEPENDENCIES_LOADED, NULL, &fBailed);
    }

    // This phase does access/constraint and other type-safety checks on the type
    // and on its transitive dependencies.
    if (targetLevel == CLASS_LOADED)
    {
        DFLPendingList pendingList;
        BOOL           fBailed = FALSE;
    
        typeHnd.DoFullyLoad(NULL, CLASS_LOADED, &pendingList, &fBailed);


        // In the case of a circular dependency, one or more types will have
        // had their promotions deferred.
        //
        // If we got to this point, all checks have successfully passed on
        // the transitive closure (otherwise, DoFullyLoad would have thrown.)
        //
        // So we can go ahead and mark everyone as fully loaded.
        //
        UINT numTH = pendingList.Count();
        TypeHandle *pTHPending = pendingList.Table();
        for (UINT i = 0; i < numTH; i++)
        {
            // NOTE: It is possible for duplicates to appear in this list so
            // don't do any operation that isn't idempodent.

            pTHPending[i].SetIsFullyLoaded();
        }
    }
}


// 
TypeHandle ClassLoader::LoadTypeHandleForTypeKey(TypeKey *pTypeKey,
                                                 TypeHandle typeHnd,
                                                 ClassLoadLevel targetLevel/*=CLASS_LOADED*/)
{

    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACTL_END

    // Type loading can be recursive.  Probe for sufficient stack.
    //
    // Execution of the FINALLY in LoadTypeHandleForTypeKey_Body can eat 
    // a lot of stack because LoadTypeHandleForTypeKey_Inner can rethrow 
    // any non-SO exceptions that it takes, ensure that we have plenty 
    // of stack before getting into it (>24 pages on AMD64, remember 
    // that num pages probed is 2*N on AMD64).
    INTERIOR_STACK_PROBE_FOR(GetThread(),20);

#ifdef _DEBUG
    if (LoggingOn(LF_CLASSLOADER, LL_INFO1000))
    {
        SString name;
        TypeString::AppendTypeKeyDebug(name, pTypeKey);
        LOG((LF_CLASSLOADER, LL_INFO10000, "PHASEDLOAD: LoadTypeHandleForTypeKey for type %S to level %s\n", name.GetUnicode(), classLoadLevelName[targetLevel]));
        CrstHolder unresolvedClassLockHolder(&m_UnresolvedClassLock, FALSE);
        m_pUnresolvedClassHash->Dump();
    }
#endif

    // When using domain neutral assemblies (and not eagerly propagating dependency loads), 
    // it's possible to get here without having injected the module into the current app domain.
    // GetDomainFile will accomplish that.
    
    if (!pTypeKey->IsConstructed())
    {
        pTypeKey->GetModule()->GetDomainFile();
    }

    
    ClassLoadLevel currentLevel = typeHnd.IsNull() ? CLASS_LOAD_BEGIN : typeHnd.GetLoadLevel();
    ClassLoadLevel targetLevelUnderLock = targetLevel < CLASS_DEPENDENCIES_LOADED ? targetLevel : (ClassLoadLevel) (CLASS_DEPENDENCIES_LOADED-1);
    while (currentLevel < targetLevelUnderLock)
    {
        // Track both of the following locks, but don't take either one yet.
        PendingTypeLoadEntry_LockHolder loadingEntryLockHolder;
        CrstHolder                 unresolvedClassLockHolder(&m_UnresolvedClassLock, FALSE);

        typeHnd = LoadTypeHandleForTypeKey_Body(pTypeKey, 
                                                typeHnd,
                                                currentLevel, 
                                                &loadingEntryLockHolder,
                                                &unresolvedClassLockHolder);

        if (!typeHnd.IsNull())
            currentLevel = typeHnd.GetLoadLevel();
    };

    PushFinalLevels(typeHnd, targetLevel);

    END_INTERIOR_STACK_PROBE;

    return typeHnd;
}

// 
TypeHandle ClassLoader::LoadTypeHandleForTypeKeyNoLock(TypeKey *pTypeKey,
                                                       ClassLoadLevel targetLevel/*=CLASS_LOADED*/)
{

    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
        PRECONDITION(CheckPointer(pTypeKey));
        PRECONDITION(targetLevel >= 0 && targetLevel <= CLASS_LOADED);
    }
    CONTRACTL_END

    TypeHandle typeHnd = TypeHandle();

    // Type loading can be recursive.  Probe for sufficient stack.
    INTERIOR_STACK_PROBE_FOR(GetThread(),8);

    ClassLoadLevel currentLevel = CLASS_LOAD_BEGIN;
    ClassLoadLevel targetLevelUnderLock = targetLevel < CLASS_DEPENDENCIES_LOADED ? targetLevel : (ClassLoadLevel) (CLASS_DEPENDENCIES_LOADED-1);
    while (currentLevel < targetLevelUnderLock)
    {
        typeHnd = DoIncrementalLoad(pTypeKey, typeHnd, currentLevel);
        CONSISTENCY_CHECK(typeHnd.GetLoadLevel() > currentLevel);
        currentLevel = typeHnd.GetLoadLevel();
    };

    PushFinalLevels(typeHnd, targetLevel);

    END_INTERIOR_STACK_PROBE;

    return typeHnd;
}

TypeHandle ClassLoader::LoadTypeHandleForTypeKey_Inner(TypeKey *pTypeKey,
                                                       TypeHandle typeHnd,
                                                       ClassLoadLevel currentLevel,
                                                       PendingTypeLoadEntry_LockHolder *pLoadingEntryLockHolder,
                                                       CrstHolder *pUnresolvedClassLockHolder,
                                                       PendingTypeLoadEntry*& pPendingLoadingEntry
                                                       )
{
    CONTRACT(TypeHandle)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_PREEMPTIVE;
        PRECONDITION(CheckPointer(pTypeKey));
        POSTCONDITION(typeHnd.IsNull() || typeHnd.GetLoadLevel() > currentLevel);
    }
    CONTRACT_END;

    PendingTypeLoadEntry* pLoadingEntry;

    // Add LoadingEntry to hash table of unresolved classes
    _ASSERTE(m_UnresolvedClassLock.OwnedByCurrentThread());

    if (typeHnd.IsNull())
    {
        // The class was not being loaded.  However, it may have already been loaded after our
        // first LoadTypeHandleThrowIfFailed() and before taking the lock.
        typeHnd = LookupTypeHandleForTypeKey(pTypeKey);
    }

    // Found it.
    if (!typeHnd.IsNull() && typeHnd.GetLoadLevel() > currentLevel)
        RETURN typeHnd;

    EX_TRY
    {
        // It was not loaded, and it is not being loaded, so we must load it.  Create a new LoadingEntry
        // and acquire it immediately so that other threads will block. 
        pLoadingEntry = new PendingTypeLoadEntry(*pTypeKey, typeHnd);  // this atomically creates a crst and acquires it
        pLoadingEntryLockHolder->AssignPreaquiredLock(pLoadingEntry);

        if (!(m_pUnresolvedClassHash->InsertValue(pLoadingEntry)))
        {
            pLoadingEntryLockHolder->Clear();  // releases the lock and breaks the link between the lock and the holder
            delete pLoadingEntry;
            COMPlusThrowOM();
        }
        pPendingLoadingEntry = pLoadingEntry;

        TRIGGERS_TYPELOAD();

        // Leave the global lock, so that other threads may now start waiting on our class's lock
        pUnresolvedClassLockHolder->Release();
        typeHnd = DoIncrementalLoad(pTypeKey, typeHnd, currentLevel);
        if (!typeHnd.IsNull())
        {
            pLoadingEntry->SetResult(typeHnd);        
        }
    }
    EX_HOOK
    {

        LOG((LF_CLASSLOADER, LL_INFO10, "Caught an exception loading: %x, %0x (Module)\n", pTypeKey->GetTypeToken(), pTypeKey->GetModule()));

        Thread* pThread = GetThread();
        if (pThread)
            pThread->DisablePreemptiveGC();

        Exception *pException = GET_EXCEPTION();

        // Release the global lock, if held
        pUnresolvedClassLockHolder->Release();

        // Fix up the loading entry.
        if (pPendingLoadingEntry) 
        {
            pPendingLoadingEntry->SetException(pException);
        }
    }
    EX_END_HOOK;

    RETURN typeHnd;
}

TypeHandle ClassLoader::LoadTypeHandleForTypeKey_Body(TypeKey *pTypeKey,
                                                      TypeHandle typeHnd,
                                                      ClassLoadLevel currentLevel,
                                                      PendingTypeLoadEntry_LockHolder *pLoadingEntryLockHolder,
                                                      CrstHolder *pUnresolvedClassLockHolder)
{
    STATIC_CONTRACT_THROWS;

    if (!pTypeKey->IsConstructed())
    {
        Module *pModule = pTypeKey->GetModule();
        IMDInternalImport* pInternalImport = pModule->GetMDImport();
        mdTypeDef cl = pTypeKey->GetTypeToken();
        DWORD rid = RidFromToken(cl);
        if(!((TypeFromToken(cl)==mdtTypeDef) && rid && pInternalImport->IsValidToken(cl))) {
            LOG((LF_CLASSLOADER, LL_INFO10, "Invalid type token to load: 0x%08x\n", cl));
            m_pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_BADFORMAT);
       }

#ifdef _DEBUG
        {
            LPCUTF8 className;
            LPCUTF8 nameSpace;
            pInternalImport->GetNameOfTypeDef(cl, &className, &nameSpace);
            if (g_pConfig->ShouldBreakOnClassLoad(className))
                _ASSERTE(!"BreakOnClassLoad");
        }
#endif // _DEBUG
        
        
        STRESS_LOG2(LF_CLASSLOADER,  LL_INFO1000, "LoadTypeHandle: Loading Class from Module %p token %x)\n", pModule, cl);
    }

    PendingTypeLoadEntry  *pLoadingEntry;
    TypeHandle  thRet;


    /*** NEW STATE VARIABLES FOR PAL_FINALLY */
    PendingTypeLoadEntry *pPendingLoadingEntry = NULL;
    PendingTypeLoadEntry *pLoadingEntryToDelete = NULL;
    /*** END NEW STATE VARIABLES FOR PAL_FINALLY */

    EX_TRY_FOR_FINALLY
    {

    retry:
        pUnresolvedClassLockHolder->Acquire();

        // Is it in the hash of classes currently being loaded?
        pLoadingEntry = m_pUnresolvedClassHash->GetValue(pTypeKey);

        if (pLoadingEntry)
        {
            pLoadingEntry->m_dwWaitCount++;

            // It is in the hash, which means that another thread is waiting for it (or that we are
            // already loading this class on this thread, which should never happen, since that implies
            // a recursive dependency).
            pUnresolvedClassLockHolder->Release();

            // Wait for class to be loaded by another thread.  This is where we start tracking the
            // entry, so there is an implicit Acquire in our use of Assign here.
            pLoadingEntryLockHolder->Assign(pLoadingEntry);
            _ASSERTE(pLoadingEntry->HasLock());
            pLoadingEntryLockHolder->Release();

            // Result of other thread loading the class
            HRESULT hr = pLoadingEntry->m_hrResult;

            // Get a pointer to the EEClass being loaded
            thRet = pLoadingEntry->m_typeHandle;

            _ASSERTE(hr != (HRESULT) 0xCDCDCDCD);

            // Enter the global lock
            pUnresolvedClassLockHolder->Acquire();

            // If we were the last thread waiting for this class, delete the LoadingEntry
            if (--pLoadingEntry->m_dwWaitCount == 0)
                pLoadingEntryToDelete = pLoadingEntry;    // Schedule for deletion in PAL_FINALLY


            if (FAILED(hr)) {
                if (hr == E_ABORT) {
                    LOG((LF_CLASSLOADER, LL_INFO10, "need to retry LoadTypeHandle: %x\n", hr));
                    goto retry;
                }

                LOG((LF_CLASSLOADER, LL_INFO10, "Failed to load in other entry: %x\n", hr));

                if (hr == E_OUTOFMEMORY) {
                    COMPlusThrowOM();
                }

                Thread* pThread = GetThread();
                pThread->DisablePreemptiveGC();
                pLoadingEntry->ThrowException();
            }
            pUnresolvedClassLockHolder->Release();
            
        }
        else {
            thRet = LoadTypeHandleForTypeKey_Inner(
                    pTypeKey, 
                    typeHnd,
                    currentLevel,
                    pLoadingEntryLockHolder,
                    pUnresolvedClassLockHolder,
                    pPendingLoadingEntry);  // passed by ref, used in finally
        }
    }
    EX_FINALLY
    {
        if (GetThread() && GetThread()->PreemptiveGCDisabled())
            GetThread()->EnablePreemptiveGC();

        pUnresolvedClassLockHolder->Release();

        if (pPendingLoadingEntry) 
        {
            _ASSERTE(pPendingLoadingEntry->HasLock());

            pUnresolvedClassLockHolder->Acquire();

            _ASSERTE(pPendingLoadingEntry->m_dwWaitCount > 0);

            // Unlink this class from the unresolved class list
            m_pUnresolvedClassHash->DeleteValue(pTypeKey);

            if (--pPendingLoadingEntry->m_dwWaitCount == 0) {
                pLoadingEntryLockHolder->Clear();
                _ASSERTE(!pPendingLoadingEntry->HasLock());
                delete(pPendingLoadingEntry);
            }
            else {
                // At least one other thread is waiting for this class.
                // m_pClass, m_hrResult and throwable already set in mainline code above.
                // Unblock other threads so that they can see the result code
                pLoadingEntryLockHolder->Release();
            }

            pUnresolvedClassLockHolder->Release();
        }

        // Order is important here: We must make sure we let go of the crst before the
        // "delete" deallocates it!
        pLoadingEntryLockHolder->Clear();

        if (pLoadingEntryToDelete)
            delete pLoadingEntryToDelete;

        pUnresolvedClassLockHolder->Release();


    }
    EX_END_FINALLY

    return thRet;
}


// Gather information about a generic type
// - number of parameters
// - variance annotations
// - dictionaries
// - sharability
/*static*/
void MethodTableBuilder::GatherGenericsInfo(Module *pModule, mdTypeDef cl, TypeHandle *genericArgs, 
                                            bmtGenericsInfo *bmtGenericsInfo)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
        PRECONDITION(GetThread() != NULL);
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(bmtGenericsInfo));
    }
    CONTRACTL_END;

    IMDInternalImport* pInternalImport = pModule->GetMDImport();

    // Enumerate the formal type parameters
    HENUMInternal   hEnumGenericPars;
    HRESULT hr = pInternalImport->EnumInit(mdtGenericParam, cl, &hEnumGenericPars);
    if (FAILED(hr))
        pModule->GetAssembly()->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_BADFORMAT);

    DWORD numGenericArgs = pInternalImport->EnumGetCount(&hEnumGenericPars);

    // Work out what kind of EEClass we're creating w.r.t. generics.  If there
    // are no generics involved this will be a VMFLAG_NONGENERIC.
    BOOL fHasVariance = FALSE;
    if (numGenericArgs > 0)
    {
        // Generic type verification
        {
            DWORD   dwAttr;
            mdToken tkParent;
            pInternalImport->GetTypeDefProps(cl, &dwAttr, &tkParent);
            // A generic with explicit layout is not allowed.
            if (IsTdExplicitLayout(dwAttr))
            {
                pModule->GetAssembly()->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_EXPLICIT_GENERIC);
            }
        }

        bmtGenericsInfo->numDicts = 1;

        mdGenericParam    tkTyPar;
        bmtGenericsInfo->pVarianceInfo = (BYTE*) GetThread()->m_MarshalAlloc.Alloc(numGenericArgs);

        // If it has generic arguments but none have been specified, then load the instantiation at the formals
        if (genericArgs == NULL)
        {
            bmtGenericsInfo->genericsKind = EEClass::VMFLAG_GENERIC_TYPICALINST;
            SIZE_T cbAllocSize = 0;
            if (!ClrSafeInt<SIZE_T>::multiply(numGenericArgs, sizeof(TypeHandle), cbAllocSize))
                COMPlusThrowHR(COR_E_OVERFLOW);
            genericArgs = (TypeHandle *) GetThread()->m_MarshalAlloc.Alloc(cbAllocSize);
        }
        else
        {
            BOOL sharable = TypeHandle::IsSharableInstantiation(numGenericArgs, genericArgs);
            bmtGenericsInfo->genericsKind = sharable ? EEClass::VMFLAG_GENERIC_SHAREDINST : EEClass::VMFLAG_GENERIC_UNSHAREDINST;
        }
        
        for(unsigned int i = 0; i < numGenericArgs; i++)
        {
            pModule->GetMDImport()->EnumNext(&hEnumGenericPars, &tkTyPar);
            DWORD flags;
            pModule->GetMDImport()->GetGenericParamProps(tkTyPar, NULL, &flags, NULL, NULL, NULL);

            if (bmtGenericsInfo->genericsKind == EEClass::VMFLAG_GENERIC_TYPICALINST)
            {
                void *mem = (void*)pModule->GetDomain()->GetLowFrequencyHeap()->AllocMem(sizeof(TypeVarTypeDesc));
                TypeVarTypeDesc *pTypeVarTypeDesc = new (mem) TypeVarTypeDesc(pModule, cl, i, tkTyPar);
                genericArgs[i] = TypeHandle(pTypeVarTypeDesc);
            }

            DWORD varianceAnnotation = flags & gpVarianceMask;
            bmtGenericsInfo->pVarianceInfo[i] = varianceAnnotation;
            if (varianceAnnotation != gpNonVariant)               
            {
                if (varianceAnnotation != gpContravariant && varianceAnnotation != gpCovariant)
                {
                    pModule->GetAssembly()->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_BADVARIANCE);
                }
                else
                    fHasVariance = TRUE;
            }
        }

        if (!fHasVariance)
            bmtGenericsInfo->pVarianceInfo = NULL;
    }
    else
    {
        bmtGenericsInfo->genericsKind = EEClass::VMFLAG_NONGENERIC;
        bmtGenericsInfo->numDicts = 0;
    }

    bmtGenericsInfo->fContainsGenericVariables = MethodTable::ComputeContainsGenericVariables(numGenericArgs, genericArgs);

    SigTypeContext typeContext(numGenericArgs, genericArgs, 0, NULL);
    bmtGenericsInfo->typeContext = typeContext;
}


// This service is called for normal classes -- and for the pseudo class we invent to
// hold the module's public members.
/*static*/
TypeHandle ClassLoader::CreateTypeHandleForTypeDefThrowing(Module *pModule,
                                                           mdTypeDef cl,
                                                           TypeHandle *genericArgs,
                                                           AllocMemTracker *pamTracker)
{
    CONTRACT(TypeHandle)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
        PRECONDITION(GetThread() != NULL);
        PRECONDITION(CheckPointer(pModule));
        POSTCONDITION(!RETVAL.IsNull());
        POSTCONDITION(CheckPointer(RETVAL.GetMethodTable()));
    }
    CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("ClassLoad");

    EEClass *pClass = NULL;
    size_t   dwAllocRequestSize = 0;
    MethodTable *pParentMethodTable = NULL;
    PCCOR_SIGNATURE parentInst;
    mdTypeDef tdEnclosing = mdTypeDefNil;
    DWORD       cInterfaces;
    BuildingInterfaceInfo_t *pInterfaceBuildInfo = NULL;
    IMDInternalImport* pInternalImport = NULL;
    LayoutRawFieldInfo *pLayoutRawFieldInfos = NULL;
    MethodTableBuilder::bmtGenericsInfo genericsInfo;

    Assembly *pAssembly = pModule->GetAssembly();
    pInternalImport = pModule->GetMDImport();

    DWORD rid = RidFromToken(cl);
    if ((rid==0) || (rid==0x00FFFFFF) || (rid > pInternalImport->GetCountWithTokenKind(mdtTypeDef) + 1))
        pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_BADFORMAT);

    Thread *pThread = GetThread();
    BEGIN_SO_INTOLERANT_CODE(pThread);

    // GetCheckpoint for the thread-based allocator
    // This checkpoint provides a scope for all transient allocations of data structures
    // used during class loading.
    // <NICE> Ideally a debug/checked build should pass around tokens indicating the Checkpoint
    // being used and check these dynamically </NICE>
    CheckPointHolder cph(pThread->m_MarshalAlloc.GetCheckpoint()); //hold checkpoint for autorelease
    
    // Gather up generics info
    MethodTableBuilder::GatherGenericsInfo(pModule, cl, genericArgs, &genericsInfo);

    Module *pLoaderModule = pModule;
    if (genericArgs != NULL)
        pLoaderModule = ClassLoader::ComputeLoaderModule(pModule, cl, genericArgs, genericsInfo.GetNumGenericArgs(),NULL,0);

    // Work out the domain in which allocate the EEClass and all
    // associated structures. Normally this is just the domain of
    // the module but for generic instantiations we have to take
    // into account the instantiation.
    BaseDomain* pDomain = pModule->GetDomain();
    if (genericArgs != NULL)
        pDomain = BaseDomain::ComputeBaseDomain(pDomain, genericsInfo.GetNumGenericArgs(), genericArgs);

    pParentMethodTable = LoadApproxParentThrowing(pModule, cl, &parentInst, &genericsInfo.typeContext);

    if (pParentMethodTable) {
        // Since methods on System.Array assume the layout of arrays, we can not allow
        // subclassing of arrays, it is sealed from the users point of view.
        if (pParentMethodTable->IsSealed() || pParentMethodTable == g_pArrayClass)
            pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_SEALEDPARENT);

        genericsInfo.numDicts += pParentMethodTable->GetNumDicts();
    }

    GetEnclosingClassThrowing(pInternalImport, pModule, cl, &tdEnclosing);

    BYTE nstructPackingSize = 0, nstructNLT = 0;
    BOOL fExplicitOffsets = FALSE;
    // NOTE: HasLayoutMetadata does not load classes
    BOOL fHasLayout = !genericsInfo.fContainsGenericVariables && 
        HasLayoutMetadata(pModule->GetAssembly(), pInternalImport, cl,
                          pParentMethodTable, 
                          &nstructPackingSize, &nstructNLT, &fExplicitOffsets);

    BOOL fIsEnum = (g_pEnumClass != NULL && pParentMethodTable == g_pEnumClass);

    // enums may not have layout because they derive from g_pEnumClass and that has no layout
    // this is enforced by HasLayoutMetadata above
    _ASSERTE(!(fIsEnum && fHasLayout));

    // This is a delegate class if it derives from either a singledelegate or multicast delegate
    BOOL        fIsAnyDelegateClass = pParentMethodTable && pParentMethodTable->IsAnyDelegateExact();

    // The only caveat to the above rule is that MulticastDelegate class itself derives from the Delegate class.
    // So we check for this case and mark the class as a regular class.
    if (fIsAnyDelegateClass) {
        LPCUTF8 className;
        LPCUTF8 nameSpace;
        pInternalImport->GetNameOfTypeDef(cl, &className, &nameSpace);
        if(strcmp(className, "MulticastDelegate") == 0)
            fIsAnyDelegateClass = FALSE;
    }

    EX_TRY
    {
        // Create a EEClass entry for it, filling out a few fields, such as the parent class token
        // (and the generic type should we be creating an instantiation)
        MethodTableBuilder::CreateClass(pDomain,
                                        pModule,
                                        cl,
                                        fHasLayout,
                                        fIsAnyDelegateClass,
                                        fIsEnum,
                                        &genericsInfo,
                                        &pClass,
                                        &dwAllocRequestSize,
                                        pamTracker);
        
        if (pParentMethodTable) {
            if (pParentMethodTable->IsMultiDelegateExact())
                pClass->SetIsMultiDelegate();
            else if (pParentMethodTable->IsSingleDelegateExact()) {
                    // We don't want MultiCastDelegate class itself to return true for IsSingleCastDelegate
                    // rather than do a name match, we look for the fact that it is not sealed
                if (pModule->GetAssembly() != SystemDomain::SystemAssembly()) {
                    pAssembly->ThrowTypeLoadException(pInternalImport, cl, BFA_CANNOT_INHERIT_FROM_DELEGATE);
                }
    #ifdef _DEBUG
                else {
                    // Only MultiCastDelegate should inherit from Delegate
                    LPCUTF8 className;
                    LPCUTF8 nameSpace;
                    pInternalImport->GetNameOfTypeDef(cl, &className, &nameSpace);
                    THROW_BAD_FORMAT_MAYBE(strcmp(className, "MulticastDelegate") == 0, 0, pClass);
                }
    #endif

                // Note we do not allow single cast delegates anymore
            }

            if (pClass->IsAnyDelegateClass() && !pClass->IsSealed()) {
                pAssembly->ThrowTypeLoadException(pInternalImport, cl, BFA_DELEGATE_CLASS_NOTSEALED);
            }
        }


        if (tdEnclosing != mdTypeDefNil) {
            pClass->SetIsNested();
            THROW_BAD_FORMAT_MAYBE(IsTdNested(pClass->GetProtection()), VLDTR_E_TD_ENCLNOTNESTED, pClass);
        }
        else if(IsTdNested(pClass->GetProtection()))
            pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_BADFORMAT);

        // We only permit generic interfaces and delegates to have variant type parameters
        if (genericsInfo.pVarianceInfo != NULL && !pClass->IsInterface() && !fIsAnyDelegateClass)
        {
            pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_VARIANCE_CLASS);
        }

        // Now load all the interfaces
        HENUMInternalHolder   hEnumInterfaceImpl(pInternalImport);
        hEnumInterfaceImpl.EnumInit(mdtInterfaceImpl, cl);

        cInterfaces = pInternalImport->EnumGetCount(&hEnumInterfaceImpl);

        if (cInterfaces != 0) {
            DWORD i;

            // Allocate the BuildingInterfaceList table
            pInterfaceBuildInfo = (BuildingInterfaceInfo_t *) GetThread()->m_MarshalAlloc.Alloc(cInterfaces * sizeof(BuildingInterfaceInfo_t));

            mdInterfaceImpl ii;
            for (i = 0; pInternalImport->EnumNext(&hEnumInterfaceImpl, &ii); i++) {

                // Get properties on this interface
                mdTypeRef crInterface = pInternalImport->GetTypeOfInterfaceImpl(ii);
                // validate the token
                mdToken crIntType = RidFromToken(crInterface)&&pInternalImport->IsValidToken(crInterface) ?
                    TypeFromToken(crInterface) : 0;
                switch(crIntType)
                {
                    case mdtTypeDef:
                    case mdtTypeRef:
                    case mdtTypeSpec:
                        break;
                    default:
                        pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_INTERFACENULL);
                }

                TypeHandle intType = LoadApproxTypeThrowing(pModule, crInterface, NULL, &genericsInfo.typeContext);

                pInterfaceBuildInfo[i].m_pMethodTable = intType.AsMethodTable();
                if (pInterfaceBuildInfo[i].m_pMethodTable == NULL)
                    pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_INTERFACENULL);

                // Ensure this is an interface
                if (pInterfaceBuildInfo[i].m_pMethodTable->IsInterface() == FALSE)
                     pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_NOTINTERFACE);

                // Check interface for use of variant type parameters
                if (genericsInfo.pVarianceInfo != NULL && TypeFromToken(crInterface) == mdtTypeSpec)
                {
                    ULONG cSig;
                    PCCOR_SIGNATURE pSig;
                    pInternalImport->GetTypeSpecFromToken(crInterface, &pSig, &cSig);
                    // Interfaces behave covariantly
                    if (!pClass->CheckVarianceInSig(genericsInfo.GetNumGenericArgs(),
                                                    genericsInfo.pVarianceInfo, 
                                                    SigPointer(pSig), 
                                                    gpCovariant))
                    {
                        pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_VARIANCE_IN_INTERFACE);
                    }
                }
            }
            _ASSERTE(i == cInterfaces);
        }

        if (fHasLayout ||
            /* Variant delegates should not have any instance fields of the variant.
               type parameter. For now, we just completely disallow all fields even
               if they are non-variant or static, as it is not a useful scenario.
               @TODO: A more logical place for this check would be in 
               MethodTableBuilder::EnumerateClassMembers() */
            (fIsAnyDelegateClass && genericsInfo.pVarianceInfo)) {
            
            // check for fields and variance
            ULONG               cFields;
            HENUMInternalHolder hEnumField(pInternalImport);
            hEnumField.EnumInit(mdtFieldDef, cl);

            cFields = pInternalImport->EnumGetCount(&hEnumField);

            if (cFields && fIsAnyDelegateClass && genericsInfo.pVarianceInfo)
            {
                pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_VARIANCE_IN_DELEGATE);
            }

            if (fHasLayout)
            {
                // Though we fail on this condition, we should never run into it.
                CONSISTENCY_CHECK(nstructPackingSize != 0);
                // MD Val check: PackingSize
                if((nstructPackingSize == 0)  ||
                   (nstructPackingSize > 128) ||
                   (nstructPackingSize & (nstructPackingSize-1))) {
                    THROW_BAD_FORMAT_MAYBE(!"ClassLayout:Invalid PackingSize", BFA_BAD_PACKING_SIZE, pClass);
                    pAssembly->ThrowTypeLoadException(pInternalImport, cl, IDS_CLASSLOAD_BADFORMAT);
                }

                pLayoutRawFieldInfos = (LayoutRawFieldInfo*)GetThread()->m_MarshalAlloc.Alloc((1+cFields) * sizeof(LayoutRawFieldInfo));
                // Warning: this can load classes
                EEClassLayoutInfo::CollectLayoutFieldMetadataThrowing(pDomain,
                                                   cl,
                                                   nstructPackingSize,
                                                   nstructNLT,
                                                   fExplicitOffsets,
                                                   pParentMethodTable,
                                                   cFields,
                                                   &hEnumField,
                                                   pModule,
                                                   &genericsInfo.typeContext,
                                                   &(((LayoutEEClass *) pClass)->m_LayoutInfo),
                                                   pLayoutRawFieldInfos,
                                                   pamTracker);
            }
        }


        // Resolve this class, given that we know now that all of its dependencies are loaded and resolved.
        // !!! This must be the last thing in this TRY block: if MethodTableBuilder succeeds, it has published the class
        // and there is no going back.
        MethodTableBuilder builder(pClass);

        builder.BuildMethodTableThrowing(pDomain,
                                         pLoaderModule,
                                         pModule,
                                         cl,
                                         pInterfaceBuildInfo,
                                         pLayoutRawFieldInfos,
                                         pParentMethodTable,
                                         &genericsInfo,
                                         parentInst,
                                         (WORD) cInterfaces,
                                         pamTracker);
    }
    EX_HOOK
    {
        if (pClass)
            pClass->Destruct();
    }
    EX_END_HOOK

    END_SO_INTOLERANT_CODE;
    RETURN(TypeHandle(pClass->GetMethodTable()));
}

#endif // #ifndef DACCESS_COMPILE

/* static */
TypeHandle ClassLoader::LoadArrayTypeThrowing(TypeHandle elemType, 
                                              CorElementType arrayKind, 
                                              unsigned rank/*=0*/, 
                                              LoadTypesFlag fLoadTypes/*=LoadTypes*/, 
                                              ClassLoadLevel level)
{
    CONTRACT(TypeHandle)
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        if (fLoadTypes == DontLoadTypes) SO_TOLERANT; else SO_INTOLERANT;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL, ((fLoadTypes == LoadTypes) ? NULL_NOT_OK : NULL_OK)));
    }
    CONTRACT_END

    // Try finding it in our cache of primitive SD arrays
    if (arrayKind == ELEMENT_TYPE_SZARRAY) {
        CorElementType type = elemType.GetSignatureCorElementType();
        if (type <= ELEMENT_TYPE_R8) {
            ArrayTypeDesc* typeDesc = g_pPredefinedArrayTypes[type];
            if (typeDesc != 0)
                RETURN(TypeHandle(typeDesc));
        }
        // This call to AsPtr is somewhat bogus and only used
        // as an optimization.  If the TypeHandle is really a TypeDesc
        // then the equality checks for the optimizations below will 
        // fail.  Thus ArrayMT should not be used elsewhere in this function
        else if (elemType.AsPtr() == g_pObjectClass) {
            // Code duplicated because Object[]'s SigCorElementType is E_T_CLASS, not OBJECT
            ArrayTypeDesc* typeDesc = g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT];
            if (typeDesc != 0)
                RETURN(TypeHandle(typeDesc));
        }
        else if (elemType.AsPtr() == g_pStringClass) {
            // Code duplicated because String[]'s SigCorElementType is E_T_CLASS, not STRING
            ArrayTypeDesc* typeDesc = g_pPredefinedArrayTypes[ELEMENT_TYPE_STRING];
            if (typeDesc != 0)
                RETURN(TypeHandle(typeDesc));
        }
        rank = 1;
    }
    TypeKey key(arrayKind, elemType, FALSE, rank);
    RETURN(LoadConstructedTypeThrowing(&key, fLoadTypes, level));
}

#ifndef DACCESS_COMPILE

VOID ClassLoader::AddAvailableClassDontHaveLock(Module *pModule,
                                                mdTypeDef classdef,
                                                AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    CrstPreempHolder ch(&m_AvailableClassLock);
    AddAvailableClassHaveLock(pModule, classdef, pamTracker);
}

#endif // #ifndef DACCESS_COMPILE



//  Low bit is discriminator between unresolved and resolved.
//     Low bit == 0:  Resolved:   data == TypeHandle
//     Low bit == 1:  Unresolved: data encodes either a typeDef or exportTypeDef. Use bit 31 as discriminator.
//
//  If not resolved, bit 31 (IA64: yes, it's bit31, not the high bit!) is discriminator between regular typeDef and exportedType
//
//     Bit31   == 0:  mdTypeDef:      000t tttt tttt tttt tttt tttt tttt ttt1
//     Bit31   == 1:  mdExportedType: 100e eeee eeee eeee eeee eeee eeee eee1
//
//

/* static */
HashDatum EEClassHashTable::CompressClassDef(mdToken cl)
{

    LEAF_CONTRACT;

    _ASSERTE(TypeFromToken(cl) == mdtTypeDef || TypeFromToken(cl) == mdtExportedType);

    switch (TypeFromToken(cl))
    {
        case mdtTypeDef:      return (HashDatum)(                         0 | (((ULONG_PTR)cl & 0x00ffffff) << 1) | EECLASSHASH_TYPEHANDLE_DISCR);
        case mdtExportedType: return (HashDatum)(EECLASSHASH_MDEXPORT_DISCR | (((ULONG_PTR)cl & 0x00ffffff) << 1) | EECLASSHASH_TYPEHANDLE_DISCR);
        default:
            _ASSERTE(!"Can't get here.");
            return 0;
    }

}

VOID EEClassHashTable::UncompressModuleAndNonExportClassDef(HashDatum Data, Module **ppModule, mdTypeDef *pCL)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;

    DWORD dwData = (DWORD)(DWORD_PTR)(Data);
    _ASSERTE((dwData & EECLASSHASH_TYPEHANDLE_DISCR) == EECLASSHASH_TYPEHANDLE_DISCR);
    _ASSERTE(!(dwData & EECLASSHASH_MDEXPORT_DISCR));

    *pCL = ((dwData >> 1) & 0x00ffffff) | mdtTypeDef;
    *ppModule = m_pModule;
}

VOID EEClassHashTable::UncompressModuleAndClassDef(HashDatum Data, Loader::LoadFlag loadFlag,
                                                   Module **ppModule, mdTypeDef *pCL,
                                                   mdExportedType *pmdFoundExportedType)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM();); }
        MODE_ANY;

        PRECONDITION(CheckPointer(pCL));
        PRECONDITION(CheckPointer(ppModule));
    }
    CONTRACTL_END

    DWORD dwData = (DWORD)(DWORD_PTR)(Data);
    _ASSERTE((dwData & EECLASSHASH_TYPEHANDLE_DISCR) == EECLASSHASH_TYPEHANDLE_DISCR);
    if(dwData & EECLASSHASH_MDEXPORT_DISCR) {
        *pmdFoundExportedType = ((dwData >> 1) & 0x00ffffff) | mdtExportedType;
#ifndef DACCESS_COMPILE
        *ppModule = m_pModule->GetAssembly()->
            FindModuleByExportedType(*pmdFoundExportedType, loadFlag, mdTypeDefNil, pCL);
#else
        DacNotImpl();
        *ppModule = NULL;
#endif
    }
    else {
        UncompressModuleAndNonExportClassDef(Data, ppModule, pCL);
        *pmdFoundExportedType = mdTokenNil;
    }
}

/* static */
mdToken EEClassHashTable::UncompressModuleAndClassDef(HashDatum Data)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END

    DWORD dwData = (DWORD)(ULONG_PTR) Data; // 64Bit: Pointer truncation is OK here - it's not actually a pointer
    _ASSERTE((dwData & EECLASSHASH_TYPEHANDLE_DISCR) == EECLASSHASH_TYPEHANDLE_DISCR);

    if(dwData & EECLASSHASH_MDEXPORT_DISCR)
        return ((dwData >> 1) & 0x00ffffff) | mdtExportedType;
    else
        return ((dwData >> 1) & 0x00ffffff) | mdtTypeDef;
}

#ifndef DACCESS_COMPILE

// This routine must be single threaded!  The reason is that there are situations which allow
// the same class name to have two different mdTypeDef tokens (for example, we load two different DLLs
// simultaneously, and they have some common class files, or we convert the same class file
// simultaneously on two threads).  The problem is that we do not want to overwrite the old
// <classname> -> pModule mapping with the new one, because this may cause identity problems.
//
// This routine assumes you already have the lock.  Use AddAvailableClassDontHaveLock() if you
// don't have it.
VOID ClassLoader::AddAvailableClassHaveLock(Module *pModule,
                                            mdTypeDef classdef,
                                            AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    EEClassHashTable *pClassHash = pModule->GetAvailableClassHash();
    EEClassHashTable *pClassCaseInsHash = pModule->GetAvailableClassCaseInsHash();
    pClassHash->RetuneHashTable(pamTracker);
    if (pClassCaseInsHash)
        pClassCaseInsHash->RetuneHashTable(pamTracker);

    LPCUTF8        pszName;
    LPCUTF8        pszNameSpace;
    HashDatum      ThrowawayData;
    IMDInternalImport *pMDImport = pModule->GetMDImport();
    pMDImport->GetNameOfTypeDef(classdef, &pszName, &pszNameSpace);

    EEClassHashEntry_t *pBucket;
    mdTypeDef      enclosing;
    if (SUCCEEDED(pMDImport->GetNestedClassProps(classdef, &enclosing))) {
        // nested type

        LPCUTF8 pszEnclosingName;
        LPCUTF8 pszEnclosingNameSpace;
        mdTypeDef enclEnclosing;

        // Find this type's encloser's entry in the available table.
        // We'll save a pointer to it in the new hash entry for this type.
        BOOL fNestedEncl = SUCCEEDED(pMDImport->GetNestedClassProps(enclosing, &enclEnclosing));

        pMDImport->GetNameOfTypeDef(enclosing, &pszEnclosingName, &pszEnclosingNameSpace);
        if ((pBucket = pClassHash->GetValue(pszEnclosingNameSpace,
                                            pszEnclosingName,
                                            &ThrowawayData,
                                            fNestedEncl)) != NULL) {
            if (fNestedEncl) {
                // Find entry for enclosing class - NOTE, this assumes that the
                // enclosing class's TypeDef or ExportedType was inserted previously,
                // which assumes that, when enuming TD's, we get the enclosing class first
                while ((!CompareNestedEntryWithTypeDef(pMDImport,
                                                       enclEnclosing,
                                                       pClassHash,
                                                       pBucket->pEncloser)) &&
                       (pBucket = pClassHash->FindNextNestedClass(pszEnclosingNameSpace,
                                                                  pszEnclosingName,
                                                                  &ThrowawayData,
                                                                  pBucket)) != NULL);
            }

            if (!pBucket) // Enclosing type not found in hash table
                pModule->GetAssembly()->ThrowBadImageException(pszNameSpace, pszName, BFA_ENCLOSING_TYPE_NOT_FOUND);

            // In this hash table, if the lower bit is set, it means a Module, otherwise it means EEClass*
            ThrowawayData = EEClassHashTable::CompressClassDef(classdef);
            InsertValue(pClassHash, pClassCaseInsHash, pszNameSpace, pszName, ThrowawayData, pBucket, pamTracker);
        }
    }
    else {
        // Don't add duplicate top-level classes.  Top-level classes are
        // added to the beginning of the bucket, while nested classes are
        // added to the end.  So, a duplicate top-level class could hide
        // the previous type's EEClass* entry in the hash table.
        EEClassHashEntry_t *pCaseInsEntry = NULL;
        LPUTF8 pszLowerCaseNS = NULL;
        LPUTF8 pszLowerCaseName = NULL;

        if (pClassCaseInsHash) {
            CreateCanonicallyCasedKey(pszNameSpace, pszName, &pszLowerCaseNS, &pszLowerCaseName);
            pCaseInsEntry = pClassCaseInsHash->AllocNewEntry(pamTracker);
        }

        EEClassHashEntry_t *pEntry = pClassHash->FindItem(pszNameSpace, pszName, FALSE);
        if (pEntry) {
            HashDatum Data = pEntry->Data;

            if (((size_t)Data & EECLASSHASH_TYPEHANDLE_DISCR) &&
                ((size_t)Data & EECLASSHASH_MDEXPORT_DISCR)) {

                // it's an ExportedType - check the 'already seen' bit and if on, report a class loading exception
                // otherwise, set it
                if ((size_t)Data & EECLASSHASH_ALREADYSEEN)
                    pModule->GetAssembly()->ThrowBadImageException(pszNameSpace, pszName, BFA_MULT_TYPE_SAME_NAME);
                else {
                    Data = (HashDatum)((size_t)Data | EECLASSHASH_ALREADYSEEN);
                    pEntry->Data = Data;
                }
            }
            else {
                // We want to throw an exception for a duplicate typedef.
                // However, this used to be allowed in 1.0/1.1, and some third-party DLLs have
                // been obfuscated so that they have duplicate private typedefs.
                // We must allow this for old assemblies for app compat reasons
                LPCSTR pszVersion = NULL;
                pModule->GetMDImport()->GetVersionString(&pszVersion);
                
                SString ssVersion(SString::Utf8, pszVersion);
                SString ssV1(SString::Literal, "v1.");

                AdjustImageRuntimeVersion(&ssVersion);

                // If not "v1.*", throw an exception
                if (!ssVersion.BeginsWith(ssV1))
                    pModule->GetAssembly()->ThrowBadImageException(pszNameSpace, pszName, BFA_MULT_TYPE_SAME_NAME);
            }
        }
        else {
            pEntry = pClassHash->AllocNewEntry(pamTracker);

            CANNOTTHROWCOMPLUSEXCEPTION();
            FAULT_FORBID();

            pClassHash->InsertValueUsingPreallocatedEntry(pEntry, pszNameSpace, pszName, EEClassHashTable::CompressClassDef(classdef), NULL);

            if (pClassCaseInsHash)
                pClassCaseInsHash->InsertValueUsingPreallocatedEntry(pCaseInsEntry, pszLowerCaseNS, pszLowerCaseName, pEntry, pEntry->pEncloser);
        }
    }

}

VOID ClassLoader::AddExportedTypeHaveLock(Module *pManifestModule,
                                          mdExportedType cl,
                                          AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END


    mdToken mdImpl;
    LPCSTR pszName;
    LPCSTR pszNameSpace;
    IMDInternalImport* pAsmImport = pManifestModule->GetMDImport();
    pAsmImport->GetExportedTypeProps(cl,
                                     &pszNameSpace,
                                     &pszName,
                                     &mdImpl,
                                     NULL,   // type def
                                     NULL);  // flags

    HashDatum ThrowawayData;

    if (TypeFromToken(mdImpl) == mdtExportedType) {
        // nested class
        LPCUTF8 pszEnclosingNameSpace;
        LPCUTF8 pszEnclosingName;
        mdToken nextImpl;
        pAsmImport->GetExportedTypeProps(mdImpl,
                                         &pszEnclosingNameSpace,
                                         &pszEnclosingName,
                                         &nextImpl,
                                         NULL,  // type def
                                         NULL); // flags

        // Find entry for enclosing class - NOTE, this assumes that the
        // enclosing class's ExportedType was inserted previously, which assumes that,
        // when enuming ExportedTypes, we get the enclosing class first
        EEClassHashEntry_t *pBucket;
        if ((pBucket = pManifestModule->GetAvailableClassHash()->GetValue(pszEnclosingNameSpace,
                                                pszEnclosingName,
                                                &ThrowawayData,
                                                TypeFromToken(nextImpl) == mdtExportedType)) != NULL) {
            do {
                // check to see if this is the correct class
                if (EEClassHashTable::UncompressModuleAndClassDef(ThrowawayData) == mdImpl) {
                    ThrowawayData = EEClassHashTable::CompressClassDef(cl);

                    // we explicitly don't check for the case insensitive hash table because we know it can't have been created yet
                    pManifestModule->GetAvailableClassHash()->InsertValue(pszNameSpace, pszName, ThrowawayData, pBucket, pamTracker);
                }
                pBucket = pManifestModule->GetAvailableClassHash()->FindNextNestedClass(pszEnclosingNameSpace, pszEnclosingName, &ThrowawayData, pBucket);
            } while (pBucket);
        }

        // If the encloser is not in the hash table, this nested class
        // was defined in the manifest module, so it doesn't need to be added
        return;
    }
    else {
        // Defined in the manifest module - add to the hash table by TypeDef instead
        if (mdImpl == mdFileNil)
            return;

        // Don't add duplicate top-level classes
        // In this hash table, if the lower bit is set, it means a Module, otherwise it means EEClass*
        ThrowawayData = EEClassHashTable::CompressClassDef(cl);
        // ThrowawayData is an IN OUT param. Going in its the pointer to the new value if the entry needs
        // to be inserted. The OUT param points to the value stored in the hash table.
        BOOL bFound;
        pManifestModule->GetAvailableClassHash()->InsertValueIfNotFound(pszNameSpace, pszName, &ThrowawayData, NULL, FALSE, &bFound, pamTracker);
        if (bFound) {

            // Check for duplicate ExportedTypes
            // Let it slide if it's pointing to the same type
            mdToken foundTypeImpl;
            if ((size_t)ThrowawayData & EECLASSHASH_MDEXPORT_DISCR) {
                mdExportedType foundExportedType = EEClassHashTable::UncompressModuleAndClassDef(ThrowawayData);
                pAsmImport->GetExportedTypeProps(foundExportedType,
                                                 NULL,  // namespace
                                                 NULL,  // name
                                                 &foundTypeImpl,
                                                 NULL,  // TypeDef
                                                 NULL); // flags
            }
            else
                foundTypeImpl = mdFileNil;

            if (mdImpl != foundTypeImpl)
                pManifestModule->GetAssembly()->ThrowBadImageException(pszNameSpace, pszName, BFA_MULT_TYPE_SAME_NAME);
        }
    }
}

//
// Returns whether we can cast the provided objectref to the provided template.
//
// pMT CANNOT be an array class (types with shared MethodTables).  However, pRef can be.
//
// However, pRef can be an array class, and the appropriate thing will happen.
//
// If an interface, does a dynamic interface check on pRef.
//
// This does not handle proxies, COMObjects, etc. JITutil_IsInstanceOfBizarre is
// the definitive casting logic to handle all those cases

/* static */
BOOL ClassLoader::CanCastToClassOrInterface(OBJECTREF pRef, MethodTable *pTargetMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    // @nice: This should really be caught by the caller. This low-level routine lacks the context to give a meaningful error.
    THROW_BAD_FORMAT_MAYBE(pTargetMT->IsArray() == FALSE, BFA_UNEXPECTED_ARRAY_TYPE, pTargetMT);

    // Try to make this as fast as possible in the non-context (typical) case.  In
    // effect, we just hoist the test out of GetTrueMethodTable() and do it here.
    MethodTable *pMT = pRef->GetTrueMethodTable();
    PREFIX_ASSUME(pMT!=NULL);

    if (pTargetMT->IsInterface())
        return Object::SupportsInterface(pRef, pTargetMT);
    else
    {
        // The template is a regular class.
        return pMT->CanCastToClass(pTargetMT);
    }
}


static MethodTable* GetEnclosingMethodTable(MethodTable *pMT)
{
    CONTRACT(MethodTable*)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
        POSTCONDITION(RETVAL == NULL || RETVAL->IsTypicalTypeDefinition());
    }
    CONTRACT_END

    WRAPPER_CONTRACT;

    RETURN pMT->LoadEnclosingMethodTable();
}

//******************************************************************************
// This function determines whether a Type is accessible from
//  outside of the assembly is lives in.
static BOOL IsTypeVisibleOutsideAssembly(MethodTable* pMT)
{
    DWORD dwProtection;
        // check all types in nesting chain, while inner types are public
    while (IsTdPublic(dwProtection = pMT->GetClass()->GetProtection()) ||
           IsTdNestedPublic(dwProtection))
    {
        // if type is nested, check outer type, too
        if (IsTdNested(dwProtection))
        {
            pMT = GetEnclosingMethodTable(pMT);
        }
        // otherwise, type is visible outside of the assembly
        else
        {
            return TRUE;
        }
    }
    return FALSE;
} // static BOOL IsTypeVisibleOutsideAssembly(MethodTable* pMT)

//******************************************************************************
// This function determines whether a method [if transparent]
//  can access a specified target (e.g. Type, Method, Field)
static BOOL CheckTransparentAccessToCriticalCode(
    MethodDesc* pCurrentMD,
    DWORD dwMemberAccess,
    MethodTable* pTargetMT,
    MethodDesc* pOptionalTargetMethod,
    FieldDesc* pOptionalTargetField)
{

    // if the caller [Method] is transparent, do special security checks
    if ((NULL != pCurrentMD) && Security::IsTransparentMethod(pCurrentMD))
    {
        // check if the Target member is in-accessible outside the assembly
        if (!(IsMdPublic(dwMemberAccess) && IsTypeVisibleOutsideAssembly(pTargetMT)))
        {                
            // check if security disallows access to target member
            if (!Security::CheckNonPublicCriticalAccess(pCurrentMD, pOptionalTargetMethod, pOptionalTargetField))
            {
                return FALSE;
            }
        }
    }
    return TRUE;
} // static BOOL CheckTransparentAccessToCriticalCode


//******************************************************************************
// This function determines whether a target method is accessible from
//  some given method.
/* static */
BOOL ClassLoader::CanAccessMethod(          // True if access is legal, false otherwise.
    MethodDesc  *pCurrentMethod,            // The method that wants access.
    MethodTable *pParentMT,                 // Class containing the target method.
    MethodDesc  *pMD)                       //  Desired target method within that class.
{
    WRAPPER_CONTRACT;

    // Defer to helper.
    return CanAccess(pCurrentMethod, pCurrentMethod->GetMethodTable(),
                     pCurrentMethod->GetAssembly(),
                     pParentMT,
                     pParentMT->GetAssembly(),
                     pMD->GetAttrs(),
                     pMD, NULL);
} // BOOL ClassLoader::CanAccessMethod()

//******************************************************************************
// This function determines whether a target field is accessible from
//  some given method.
/* static */
BOOL ClassLoader::CanAccessField(           // True if access is legal, false otherwise.  
    MethodDesc  *pCurrentMethod,            // The method that wants access.              
    MethodTable *pParentMT,                 // Class containing the target field.        
    FieldDesc   *pFD)                       //  Desired target field method within that class.           
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
    }
    CONTRACTL_END

    // Make sure that the accessibility flags are the same between methods and fields.
    //  (If they're ever not, this code would have to translate.)
    _ASSERTE(fdPublic == (CorFieldAttr) mdPublic);
    _ASSERTE(fdPrivate == (CorFieldAttr) mdPrivate);
    _ASSERTE(fdFamily == (CorFieldAttr) mdFamily);
    _ASSERTE(fdAssembly == (CorFieldAttr) mdAssem);
    _ASSERTE(fdFamANDAssem == (CorFieldAttr) mdFamANDAssem);
    _ASSERTE(fdFamORAssem == (CorFieldAttr) mdFamORAssem);
    _ASSERTE(fdPrivateScope == (CorFieldAttr) mdPrivateScope);

    // Defer to helper.
    return CanAccess(pCurrentMethod, pCurrentMethod->GetMethodTable(),
                     pCurrentMethod->GetAssembly(),
                     pParentMT,
                     pParentMT->GetAssembly(),
                     pFD->GetFieldProtection(),
                     NULL,
                     pFD);
} // BOOL ClassLoader::CanAccessField()


//******************************************************************************
// This function determines whether a target class is accessible from 
//  some given class.
/* static */
BOOL ClassLoader::CanAccessMethodInstantiation(// True if access is legal, false otherwise. 
    MethodDesc *pCurrentMD,          // the method from which access is desired; NULL if global
    MethodTable *pCurrentClass,                // The class that wants access; NULL if global.
    Assembly    *pCurrentAssembly,             // Assembly containing that class.
    MethodDesc *pOptionalTargetMethod  // The desired method; if NULL, return TRUE (or)
    )      
{                                           
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
    }
    CONTRACTL_END

    // If there is no target method just allow access.
    // NB: the caller may just be checking access to a field or class, so we allow for NULL.
    if (!pOptionalTargetMethod)
        return TRUE;

    // Is the desired target an instantiated generic method?
    if (pOptionalTargetMethod->HasMethodInstantiation())
    {   // check that the current class has access
        // to all of the instantiating classes.
        DWORD numGenericArgs = pOptionalTargetMethod->GetNumGenericMethodArgs();
        TypeHandle * inst = pOptionalTargetMethod->GetMethodInstantiation();
        PREFIX_ASSUME(inst!=NULL);
        for (DWORD i = 0; i < numGenericArgs; i++)
        {   
            TypeHandle th = inst[i];
            if (!CanAccessClass(pCurrentMD, pCurrentClass, pCurrentAssembly, th.GetMethodTableOfElementType(), th.GetAssembly()))
               return FALSE;
        }
        //  If we are here, the current class has access to all of the target's instantiating args,
    }
    return TRUE;
}

//******************************************************************************
// This function determines whether a target class is accessible from 
//  some given class.
/* static */
BOOL ClassLoader::CanAccessClass(           // True if access is legal, false otherwise. 
    MethodDesc* pCurrentMD,                 // the method that wants access; NULL if global
    MethodTable *pCurrentClass,             // The class that wants access; NULL if global.
    Assembly    *pCurrentAssembly,          //  Assembly containing that class.
    MethodTable *pTargetClass,              // The desired target class.
    Assembly    *pTargetAssembly)       //  Assembly containing that class.    
{                                           
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
    }
    CONTRACTL_END

    if (!pTargetClass)
        return TRUE;

    // perform transparency check on the type, if the caller is transparent
    if ((NULL != pCurrentMD) && Security::IsTransparentMethod(pCurrentMD))
    {
        // check if type is visible outside the assembly
        if (!IsTypeVisibleOutsideAssembly(pTargetClass))
        {
            // check transparent/critical on type
            if (!Security::CheckNonPublicCriticalAccess(pCurrentMD, NULL, NULL, pTargetClass)) 
                return FALSE;
        }
    }

    // Is the desired target a generic instantiation?
    if (pTargetClass->HasInstantiation())
    {   // Yes, so before going any further, check that the current class has access
        //  to all of the instantiating classes.
        DWORD numGenericArgs = pTargetClass->GetNumGenericArgs();
        TypeHandle * inst = pTargetClass->GetInstantiation();
        PREFIX_ASSUME(inst!=NULL);
        for (DWORD i = 0; i < numGenericArgs; i++)
        {   
            TypeHandle th = inst[i];
            if (!CanAccessClass(pCurrentMD, pCurrentClass, pCurrentAssembly, th.GetMethodTableOfElementType(), th.GetAssembly()))
               return FALSE;
        }
        // If we are here, the current class has access to all of the desired target's instantiating args.
        //  Now, check whether the current class has access to the desired target itself.
    }

    if (! pTargetClass->GetClass()->IsNested()) 
    {   // a non-nested class can be either all public or accessible only from its own assembly (and friends).
        if (IsTdPublic(pTargetClass->GetClass()->GetProtection()))
        {
            return TRUE;
        }
        else
        {
            return (pTargetAssembly == pCurrentAssembly) || pTargetAssembly->GrantsFriendAccessTo(pCurrentAssembly);
        }
    }

    // If we are here, the desired target class is nested.  Translate the type flags 
    //  to corresponding method access flags.
    DWORD dwProtection = mdPublic;

    switch(pTargetClass->GetClass()->GetProtection()) {
        case tdNestedPublic:
            dwProtection = mdPublic;
            break;
        case tdNestedFamily:
            dwProtection = mdFamily;
            break;
        case tdNestedPrivate:
            dwProtection = mdPrivate;
            break;
        case tdNestedFamORAssem:
            dwProtection = mdFamORAssem;
            break;
        case tdNestedFamANDAssem:
            dwProtection = mdFamANDAssem;
            break;
        case tdNestedAssembly:
            dwProtection = mdAssem;
            break;
        default:
            THROW_BAD_FORMAT_MAYBE(!"Unexpected class visibility flag value", BFA_BAD_VISIBILITY, pTargetClass); 
    }

    // The desired target class is nested, so translate the class access request into
    //  a member access request.  That is, if the current class is trying to access A::B, 
    //  check if it can access things in A with the visibility of B.
    // So, pass A as the desired target class and visibility of B within A as the member access
    return CanAccess(pCurrentMD, pCurrentClass,
                     pCurrentAssembly,
                     GetEnclosingMethodTable(pTargetClass),
                     pTargetAssembly,
                     dwProtection,
                     NULL, NULL);
} // BOOL ClassLoader::CanAccessClass()


//******************************************************************************
// This is a front-end to CheckAccess that handles the nested class scope. If can't access
// from the current point and are a nested class, then try from the enclosing class.
/* static */
BOOL ClassLoader::CanAccess(                // TRUE if access is allowed, FALSE otherwise.
    MethodDesc*  pCurrentMD,                // The method desiring access; can be NULL if you only care about access from pCurrentAssembly
    MethodTable *pCurrentMT,                // The class desiring access; NULL if global or if you only care about access from pCurrentAssembly
    Assembly    *pCurrentAssembly,          //  Assembly containing that class.
    MethodTable *pTargetMT,                 // The class containing the desired target member.
    Assembly    *pTargetAssembly,           //  Assembly containing that class.
    DWORD       dwMemberAccess,             // Member access flags of the desired target member (as method bits).
    MethodDesc *pOptionalTargetMethod,      // The target method; NULL if the target is a not a method or
                                            // there is no need to check the method's instantiation.
   FieldDesc     *pOptionalTargetField )    // or The desired field; if NULL, return TRUE;  
{
    CONTRACT(BOOL)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pCurrentAssembly));
        MODE_ANY;
    }
    CONTRACT_END;

    // Recursive: CanAccess->CheckAccess->CanAccessClass->CanAccess
    INTERIOR_STACK_PROBE(GetThread());

    // Check for access from current class to desired target.  If OK, then done.
    if (!CheckAccess(pCurrentMD, pCurrentMT,
                    pCurrentAssembly,
                    pTargetMT,
                    pTargetAssembly,
                    dwMemberAccess,
                    pOptionalTargetMethod, pOptionalTargetField))
    {
        // If we're here, CheckAccess didn't allow access.

        // If the current class ISN'T nested, there's no enclosing class that might have access.
        //  (And if the pCurrentMT == NULL, the current class is global, and so there is no enclosing class.)
        if (! pCurrentMT || ! pCurrentMT->GetClass()->IsNested())
        {
            RETURN_FROM_INTERIOR_PROBE(FALSE);
        }

        // A nested class also has access to anything that the enclosing class does, so 
        //  recursively check whether the enclosing class can access the desired target member.
        BOOL fRes = CanAccess(pCurrentMD, GetEnclosingMethodTable(pCurrentMT),
                              pCurrentAssembly,
                              pTargetMT,
                              pTargetAssembly,
                              dwMemberAccess,
                              pOptionalTargetMethod, pOptionalTargetField);
        if (!fRes)
            RETURN_FROM_INTERIOR_PROBE(FALSE);
    }

    // For member access, we do additional checks to ensure that the specific member can
    // be accessed

    if (!CanAccessMemberForExtraChecks(
                pCurrentMD,
                pCurrentMT,
                pCurrentAssembly,
                pTargetMT,
                pOptionalTargetMethod,
                pOptionalTargetField))
    {
        RETURN_FROM_INTERIOR_PROBE(FALSE);
    }

    RETURN_FROM_INTERIOR_PROBE(TRUE);

    END_INTERIOR_STACK_PROBE;
} // BOOL ClassLoader::CanAccess()

//******************************************************************************
#ifdef _DEBUG
static ConfigDWORD s_doFullAccessChecks;
#endif

//******************************************************************************
// Performs additional checks for member access

BOOL ClassLoader::CanAccessMemberForExtraChecks(
        MethodDesc*  pCurrentMD,
        MethodTable *pCurrentMT,
        Assembly    *pCurrentAssembly,
        MethodTable *pTargetExactMT,
        MethodDesc  *pOptionalTargetMethod,
        FieldDesc   *pOptionalTargetField )
{
    WRAPPER_CONTRACT;

    // Critical callers do not need the extra checks
    // This early-out saves the cost of all the subsequent work
    if (INDEBUG((s_doFullAccessChecks.val(L"DoFullAccessChecks", 0) == 0) &&)
        (pCurrentMD == NULL || !Security::IsTransparentMethod(pCurrentMD)))
    {
        return TRUE;
    }

    if (pOptionalTargetMethod == NULL && pOptionalTargetField == NULL)
        return TRUE;

    _ASSERTE((pOptionalTargetMethod == NULL) != (pOptionalTargetField == NULL));

    if (pOptionalTargetMethod)
    {
        // A method is accessible only if all the types in the signature
        // are also accessible.
        if (!CanAccessSigForExtraChecks(pCurrentMD,
                                        pCurrentMT,
                                        pCurrentAssembly,
                                        pOptionalTargetMethod,
                                        pTargetExactMT))
        {
            return FALSE;
        }
    }
    else
    {
        // A field is accessible only if the field type is also accessible

        TypeHandle fieldType = pOptionalTargetField->GetExactFieldType(TypeHandle(pTargetExactMT));
        CorElementType fieldCorType = fieldType.GetSignatureCorElementType();
        
        // TypeVarTypeDesc do not have a MethodTable, and they are always accessible
        if (fieldCorType != ELEMENT_TYPE_VAR && fieldCorType != ELEMENT_TYPE_MVAR)
        {
            MethodTable * pFieldTypeMT = fieldType.GetMethodTable();
            if (!CanAccessClassForExtraChecks(pCurrentMD,
                                              pCurrentMT,
                                              pCurrentAssembly,
                                              pFieldTypeMT,
                                              pFieldTypeMT->GetAssembly()))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

//******************************************************************************
// Can all the types in the signature of the pTargetMethodSig be accessed?
//
// "ForExtraChecks" means that we only do extra checks (security and transparency)
// instead of the usual loader visibility checks. Post V2, we can enable all checks.

BOOL ClassLoader::CanAccessSigForExtraChecks( // TRUE if access is allowed, FALSE otherwise.
    MethodDesc*  pCurrentMD,
    MethodTable *pCurrentMT,        // The class desiring access; NULL if global.
    Assembly    *pCurrentAssembly,  //  Assembly containing that class.
    MethodDesc  *pTargetMethodSig,  // The target method. If this is a shared method, pTargetExactMT gives 
                                    // additional information about the exact type
    MethodTable *pTargetExactMT)   // or The desired field; if NULL, return TRUE;  
{
    WRAPPER_CONTRACT;

    // Critical callers do not need the extra checks
    // This early-out saves the cost of all the subsequent work
    if (INDEBUG((s_doFullAccessChecks.val(L"DoFullAccessChecks", 0) == 0) &&)
        (pCurrentMD == NULL || !Security::IsTransparentMethod(pCurrentMD)))
    {
        return TRUE;
    }

    MetaSig sig(pTargetMethodSig, TypeHandle(pTargetExactMT));

    // First, check the return type

    TypeHandle retType = sig.GetRetTypeHandleThrowing();
    if (retType.IsByRef())
        retType = retType.GetTypeParam();

    // TypeVarTypeDesc do not have a MethodTable, and they are always accessible
    if (!retType.IsGenericVariable())
    {
        if (!CanAccessClassForExtraChecks(pCurrentMD,
                                          pCurrentMT,
                                          pCurrentAssembly,
                                          retType.GetMethodTable(),
                                          retType.GetAssembly()))
        {
            return FALSE;
        }
    }

    //
    // Now walk all the arguments in the signature
    //

    for (CorElementType argType = sig.NextArg(); argType != ELEMENT_TYPE_END; argType = sig.NextArg())
    {
        TypeHandle thArg = sig.GetLastTypeHandleThrowing();
        MethodTable * pArgMT = thArg.GetMethodTable();

        if (pArgMT == NULL)
        {
            if (thArg.IsByRef())
                thArg = thArg.GetTypeParam();

            // TypeVarTypeDesc do not have a MethodTable, and they are always accessible
            if (thArg.IsGenericVariable())
                continue;

            pArgMT = thArg.GetMethodTable();
        }

        BOOL canAcesssElement = CanAccessClassForExtraChecks(
                                        pCurrentMD,
                                        pCurrentMT,
                                        pCurrentAssembly,
                                        pArgMT,
                                        thArg.GetAssembly());
        if (!canAcesssElement)
            return FALSE;
    }

    return TRUE;
}

//******************************************************************************
// Can all the types in the signature be accessed.
//
// "ForExtraChecks" means that we only do extra checks (security and transparency)
// instead of the usual loader visibility checks. Post V2, we can enable all checks.

// static 
BOOL ClassLoader::CanAccessSigForExtraChecks(
                    MethodDesc*  pCurrentMD,
                    PCCOR_SIGNATURE sig)
{
    LEAF_CONTRACT;
    
    return TRUE;
}

//******************************************************************************
// Can the type be accessed?
//
// "ForExtraChecks" means that we only do extra checks (security and transparency)
// instead of the usual loader visibility checks. Post V2, we can enable all checks.

BOOL ClassLoader::CanAccessClassForExtraChecks( // True if access is legal, false otherwise.
    MethodDesc * pCurrentMD,        // the method that wants access; NULL if global
    MethodTable *pCurrentClass,     // The class that wants access; NULL if global.
    Assembly    *pCurrentAssembly,  //  Assembly containing that class.
    MethodTable *pTargetClass,      // The desired target class.
    Assembly    *pTargetAssembly)   //  Assembly containing that class.
{
    WRAPPER_CONTRACT;

    // Critical callers do not need the extra checks
    if (INDEBUG((s_doFullAccessChecks.val(L"DoFullAccessChecks", 0) == 0) &&)
        (pCurrentMD == NULL || !Security::IsTransparentMethod(pCurrentMD)))
    {
        return TRUE;
    }

    return CanAccessClass(pCurrentMD,
                          pCurrentClass,
                          pCurrentAssembly,
                          pTargetClass,
                          pTargetAssembly);
}

//******************************************************************************
//* This is the worker function that checks whether a class has access to a 
//   member of some given class.
// pCurrentMD and pCurrentMT can be NULL in the case of a global function
// pCurrentMT is the point from which we're trying to access something
// pTargetMT is the class containing the member we are trying to access
// dwMemberAccess is the member access within pTargetMT of the member we are trying to access
// pOptionalTargetMethod is the target method or NULL if the target member is not an (instantiated generic) method;
//   it is required in order to check the accessibility of the method instantiation.

/* static */
BOOL ClassLoader::CheckAccess(              // TRUE if access is allowed, false otherwise.
    MethodDesc*  pCurrentMD,                // The method desiring access; can be NULL if you only care about access from pCurrentAssembly
    MethodTable *pCurrentMT,                // The class desiring access; NULL if global or if you only care about access from pCurrentAssembly
    Assembly    *pCurrentAssembly,          //  Assembly containing that class.
    MethodTable *pTargetMT,                 // The class containing the desired target member.
    Assembly    *pTargetAssembly,           //  Assembly containing that class.
    DWORD       dwMemberAccess,             // Member access flags of the desired target member (as method bits).
    MethodDesc *pOptionalTargetMethod,      // The target method; NULL if the target is a not a method or
                                            //      there is no need to check the method's instantiation.
    FieldDesc* pOptionalTargetField)        // or The desired field; if NULL, return TRUE;  
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pCurrentAssembly));
        MODE_ANY;
    }
    CONTRACTL_END

    // To have access to a member of some class, one first needs access to the class itself.
    if (!CanAccessClass(pCurrentMD, pCurrentMT,
                         pCurrentAssembly,
                         pTargetMT,
                         pTargetAssembly))
        return FALSE;

    // if we are trying to access a generic method, we have to ensure its instantiation is accessible.
    if (!CanAccessMethodInstantiation(pCurrentMD, pCurrentMT, pCurrentAssembly, pOptionalTargetMethod))
        return FALSE;

        // if caller is transparent, and target is non-public and critical, then fail access check
    if (!CheckTransparentAccessToCriticalCode(pCurrentMD, dwMemberAccess, pTargetMT, pOptionalTargetMethod, pOptionalTargetField))
        return FALSE;

    // If the member is public, the current class has access.    
    if (IsMdPublic(dwMemberAccess))
         return TRUE;

    // This is module-scope checking, to support C++ file & function statics.
    if (IsMdPrivateScope(dwMemberAccess)) 
    {
        if (pCurrentMT == NULL)
            return FALSE;

        // PrivateScope is accessible to module only, so current and desired target
        //  must be in the same module.
        return (pCurrentMT->GetModule() == pTargetMT->GetModule());
    }

#ifdef _DEBUG
    if (pTargetMT == NULL &&
        (IsMdFamORAssem(dwMemberAccess) ||
         IsMdFamANDAssem(dwMemberAccess) ||
         IsMdFamily(dwMemberAccess))) {
        THROW_BAD_FORMAT_MAYBE(!"Family flag is not allowed on global functions", BFA_FAMILY_ON_GLOBAL, pTargetMT); 
    }
#endif

    if(pTargetMT == NULL || IsMdAssem(dwMemberAccess))
    {
        return (pCurrentAssembly == pTargetAssembly || 
                pTargetAssembly->GrantsFriendAccessTo(pCurrentAssembly));
    }

    // Nested classes can access all members of the parent class.
    // Check for access of the current class to a member of the desired target class.
    //  Based on member access flags, there may be requirements on the the assemblies being
    //  the same (or friends), or on the types having a subclass/superclass relationship.
    // We may be able to say definitely yes or no
    do {
        // If the current class is the same class as the desired target class, grant
        //  access (ie, classes have access to all of their own members).
        if (pCurrentMT && pCurrentMT->HasSameTypeDefAs(pTargetMT))
            return TRUE;

        // Check the case for Family Or Assembly.
        if (IsMdFamORAssem(dwMemberAccess)) 
        {
            // If the current assembly is same as the desired target, or if it grants friend access, allow access.
            if (pCurrentAssembly == pTargetAssembly || pTargetAssembly->GrantsFriendAccessTo(pCurrentAssembly))
                return TRUE;

            // Remember that pCurrentMT can be NULL on entry to this function
            if (!pCurrentMT)
                return FALSE;

            // If we are here, the member has Family Or Assembly accessibility, but
            //  access has not been granted based on assembly.  So, walk up the current
            //  class's parent (superclass) chain to see if the current class is a 
            //  subclass of the desired target method's class.
            MethodTable *pMT = pCurrentMT->GetParentMethodTable();
            while (pMT) 
            {
                if (pMT == pTargetMT)
                    return TRUE;

                // On to the next higher parent, if any.
                pMT = pMT->GetParentMethodTable();
            }
        }

        if (!pCurrentMT)
            return FALSE;

        if (IsMdPrivate(dwMemberAccess)) 
        {
            // If we are here, the desired target has private accessibility.  The current 
            //  class is not the desired target class.  So, if the current class is not
            //  nested, there is no enclosing class to through which to get access.
            // This next statement is not really needed, because if the class isn't nested,
            //  the call to GetEnclosingMethodTable() would return NULL, and we'd return 
            //  FALSE right away anyway.
            if (!pCurrentMT->GetClass()->IsNested())
                return FALSE;
        }
        else
        if (IsMdFamANDAssem(dwMemberAccess) &&
                 (pCurrentAssembly != pTargetAssembly) &&
                 !pTargetAssembly->GrantsFriendAccessTo(pCurrentAssembly))
        {
            // If we are here, the desired target has Family AND Assembly accessibility,
            //  and the current assembly is not the desired target assembly
            //  nor does the target grant friend access to the current assembly.
            // So, even if the current class is a subclass of the desired target class,
            //  it is in a different (and non-friend) assembly, so access is not granted.
            return FALSE;
        }
        else
        {  


            MethodTable *pMT = pCurrentMT->GetParentMethodTable();
            while (pMT) 
            {   // If that parent class is the same as the desired target class, we're in
                //  the family, so grant access.
                if (pMT->HasSameTypeDefAs(pTargetMT))
                    return TRUE;

                // On to the next higher parent, if any.
                pMT = pMT->GetParentMethodTable();
            }
        }

        // If we are here, the current class did not get access to the desired target class.
        //  But if the current class is nested, its enclosing class might have the desired access
        //  so walk up the enclosing chain, and keep trying.
        pCurrentMT = GetEnclosingMethodTable(pCurrentMT);
    } while (pCurrentMT);

    return FALSE;
} // BOOL ClassLoader::CheckAccess()

//******************************************************************************
// This function is like the other CanAccess, except that it also takes
//  an instance parameter.  The instance is the type through which the current
//  class is trying to access the member, which is not necessarily the same as
//  the type which actually contains the member.
// pCurrentMT       : The point from which access needs to be checked.
//                    NULL for global functions, or if you only care about access from pCurrentAssembly
// pCurrentAssembly : The assembly that is doing the access
// pTargetMT        : The class containing the member being accessed.
//                    NULL for global functions
// pInstanceMT      : The class containing the member being accessed.
//                     Could be same as pCurrentMT when accessing
//                     a static method, or if the object instance is NULL.
//                    Instance Class is required to verify family access.
//                    NULL for global functions
// dwMemberAccess   : The member access within pTargetClass of the
//                    member being accessed
// pOptionalTargetMethod: The target method or NULL if the member is not an (instantiated generic) method;
//                        required to check the accessibility of the method instantiation.
// pOptionalTargetField

/* static */
BOOL ClassLoader::CanAccess(                // TRUE if access is allowed, false otherwise.                        
    MethodDesc*  pCurrentMD,                // The method desiring access; can be NULL if you only care about access from pCurrentAssembly
    MethodTable *pCurrentMT,                // The class desiring access; NULL if global or if you only care about access from pCurrentAssembly
    Assembly    *pCurrentAssembly,          //  Assembly containing that class.                                   
    MethodTable *pTargetMT,                 // The class containing the desired target member.                    
    Assembly    *pTargetAssembly,           //  Assembly containing that class.                                   
    MethodTable *pInstanceMT,               // The instance through which access is desired.
    DWORD       dwMemberAccess,             // Member access flags of the desired target member (as method bits). 
    MethodDesc *pOptionalTargetMethod,      // The target method; NULL if the target is a not a method or
                                            // there is no need to check the method's instantiation.
    FieldDesc* pOptionalTargetField         // target field, NULL if there is no Target field
    )             
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pCurrentAssembly));
        MODE_ANY;
    }
    CONTRACTL_END

    BOOL canAccess = CheckAccess(pCurrentMD,
                                 pCurrentMT,
                                 pCurrentAssembly,
                                 pTargetMT,
                                 pTargetAssembly,
                                 pInstanceMT,
                                 dwMemberAccess,
                                 pOptionalTargetMethod,
                                 pOptionalTargetField);

    if (!canAccess)
        return FALSE;

    // For member access, we do additional checks to ensure that the specific member can
    // be accessed

    canAccess = CanAccessMemberForExtraChecks(pCurrentMD,
                                              pCurrentMT,
                                              pCurrentAssembly,
                                              pTargetMT,
                                              pOptionalTargetMethod,
                                              pOptionalTargetField);

    return canAccess;
}

//******************************************************************************
// This is the helper function for the corresponding CanAccess()

/* static */
BOOL ClassLoader::CheckAccess(                // TRUE if access is allowed, false otherwise.
    MethodDesc*  pCurrentMD,                // The method desiring access; can be NULL if you only care about access from pCurrentAssembly
    MethodTable *pCurrentMT,                // The class desiring access; NULL if global or if you only care about access from pCurrentAssembly
    Assembly    *pCurrentAssembly,          //  Assembly containing that class.                                   
    MethodTable *pTargetMT,                 // The class containing the desired target member.
    Assembly    *pTargetAssembly,           //  Assembly containing that class.                                   
    MethodTable *pInstanceMT,               // The instance through which access is desired.
    DWORD       dwMemberAccess,             // Member access flags of the desired target member (as method bits). 
    MethodDesc *pOptionalTargetMethod,       // The target method; NULL if the target is a not a method or
                                            // there is no need to check the method's instantiation.
    FieldDesc* pOptionalTargetField   // target field, NULL if there is no Target field
    )             
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pCurrentAssembly));
        MODE_ANY;
    }
    CONTRACTL_END

    // we're trying to access a member that is contained in the class pTargetClass, so need to
    // check if have access to pTargetClass itself from the current point before worry about
    // having access to the member within the class
    if (!CanAccessClass(pCurrentMD, pCurrentMT,
                        pCurrentAssembly,
                        pTargetMT,
                        pTargetAssembly))
        return FALSE;
    
    // if we are trying to access a generic method, we have to ensure its instantiation is accessible.
    if (!CanAccessMethodInstantiation(pCurrentMD, pCurrentMT, pCurrentAssembly, pOptionalTargetMethod))
        return FALSE;


        // if caller is transparent, and target is non-public and critical, then fail access check
    if (!CheckTransparentAccessToCriticalCode(pCurrentMD, dwMemberAccess, pTargetMT, pOptionalTargetMethod, pOptionalTargetField))
        return FALSE;

    if (IsMdPublic(dwMemberAccess))
        return TRUE;


    if (IsMdPrivateScope(dwMemberAccess))
        return (pCurrentMT && (pCurrentMT->GetModule() == pTargetMT->GetModule()));

    if (pTargetMT == NULL || IsMdAssem(dwMemberAccess))
        return (pTargetAssembly == pCurrentAssembly || pTargetAssembly->GrantsFriendAccessTo(pCurrentAssembly));

    // Nested classes can access all members of the parent class.
    while (pCurrentMT) {

#ifdef _DEBUG
        if (pTargetMT == NULL &&
            (IsMdFamORAssem(dwMemberAccess) ||
             IsMdFamANDAssem(dwMemberAccess) ||
             IsMdFamily(dwMemberAccess)))
            THROW_BAD_FORMAT_MAYBE(!"Family flag is not allowed on global functions", BFA_FAMILY_ON_GLOBAL, pTargetMT); 
#endif

    //@GENERICSVER:
        if (pTargetMT->HasSameTypeDefAs(pCurrentMT))
            return TRUE;

        if (IsMdPrivate(dwMemberAccess)) {
            if (!pCurrentMT->GetClass()->IsNested())
                return FALSE;
        }

        else if (IsMdFamORAssem(dwMemberAccess)) {
            if (pCurrentAssembly == pTargetAssembly ||
                pTargetAssembly->GrantsFriendAccessTo(pCurrentAssembly))
                return TRUE;

            if (CanAccessFamily(pCurrentMT,
                                   pTargetMT,
                                   pInstanceMT))
                return TRUE;
        }

        else if (IsMdFamANDAssem(dwMemberAccess) &&
                 (pCurrentAssembly != pTargetAssembly) &&
                 !pTargetAssembly->GrantsFriendAccessTo(pCurrentAssembly))
            return FALSE;

        // family, famANDAssem
        else if (CanAccessFamily(pCurrentMT,
                                pTargetMT,
                                pInstanceMT))
            return TRUE;

        pCurrentMT = GetEnclosingMethodTable(pCurrentMT);
    }

    return FALSE;
}

// Allowed only if
// Target >= Current >= Instance
// where '>=' is 'parent of or equal to' relation
//
// Current is the function / method where an attempt is made to access a member
// of Target, which is marked with family access, on an object of type Instance
//
// Eg.
//
// class X
//   member x : family access
//
// class Y
//   member y : family access
//
// class A, extends X
//   member a : family access
//
// class B, extends X
//   member b : family access
//
// class C, extends A
//   member c : family access
//
//  (X > A)
//  (X > B)
//  (A > C)
//
//   Y is unrelated to X, A or C
//
//
//  CanAccessFamily of  will pass only for :
//
//  --------------------------
//  Target | Cur | Instance
//  --------------------------
//   x.X   |  X  |  X, A, B, C
//   x.X   |  A  |  A, C
//   x.X   |  B  |  B
//   x.X   |  C  |  C
//   a.A   |  A  |  A, C
//   a.A   |  C  |  C
//   b.B   |  B  |  B
//   c.C   |  C  |  C
//   y.Y   |  Y  |  Y
//
//


/* static */
BOOL ClassLoader::CanAccessFamily(MethodTable *pCurrentClass,
                                  MethodTable *pTargetClass,
                                  MethodTable *pInstanceClass)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;

        PRECONDITION(CheckPointer(pTargetClass));
    }
    CONTRACTL_END

    if (pCurrentClass == NULL)
        return FALSE;


    while (pInstanceClass) {
        MethodTable *pCurInstance = pInstanceClass;

        while (pCurInstance) {
            if (pCurInstance->HasSameTypeDefAs(pCurrentClass)) {
                // check if Current is child or equal to Target
                while (pCurrentClass) {
                    if (pCurrentClass->HasSameTypeDefAs(pTargetClass))
                        return TRUE;
                    pCurrentClass = pCurrentClass->GetParentMethodTable();
                }

                return FALSE;
            }

            pCurInstance = pCurInstance->GetParentMethodTable();
        }

        pInstanceClass = GetEnclosingMethodTable(pInstanceClass);
    }

    return FALSE;
}


#ifdef STRESS_THREAD
struct Stress_Thread_Param
{
    MethodDesc *pFD;
    OBJECTHANDLE argHandle;
    short numSkipArgs;
    CorEntryPointType EntryType;
    Thread* pThread;
};

struct Stress_Thread_Worker_Param
{
    Stress_Thread_Param *lpParameter;
    ULONG retVal;
};

static void Stress_Thread_Proc_Worker (LPVOID ptr)
{
    STATIC_CONTRACT_THROWS;

    Stress_Thread_Worker_Param *args = (Stress_Thread_Worker_Param *) ptr;
    INT32 RetVal = E_FAIL;

    Stress_Thread_Param *lpParam = (Stress_Thread_Param *)args->lpParameter;

    EX_TRY_NOCATCH
    {
        ARG_SLOT stackVar = 0;

        MethodDescCallSite threadStart(lpParam->pFD);

        // Build the parameter array and invoke the method.
        if (lpParam->EntryType == EntryManagedMain) 
        {
            PTRARRAYREF StrArgArray = (PTRARRAYREF)ObjectFromHandle(lpParam->argHandle);
            stackVar = ObjToArgSlot(StrArgArray);
        }

        if (lpParam->pFD->IsVoid())
        {
            threadStart.Call(&stackVar);
            RetVal = GetLatchedExitCode();
        }
        else
        {
            RetVal = threadStart.Call_RetArgSlot(&stackVar);
        }

        fflush(stdout);
        fflush(stderr);

    }
    EX_END_NOCATCH

    args->retVal = RetVal;
}

static DWORD WINAPI __stdcall Stress_Thread_Proc (LPVOID lpParameter)
{
    STATIC_CONTRACT_THROWS;

    Stress_Thread_Worker_Param args = {(Stress_Thread_Param*)lpParameter,0};
    Stress_Thread_Param *lpParam = (Stress_Thread_Param *)lpParameter;
    Thread *pThread = lpParam->pThread;
    if (!pThread->HasStarted())
        return 0;

    _ASSERTE(GetAppDomain() != NULL);
    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return E_FAIL);
    EX_TRY
    {

        ADID KickOffDomain = pThread->GetKickOffDomainId();

        // should always have a kickoff domain - a thread should never start in a domain that is unloaded
        // because otherwise it would have been collected because nobody can hold a reference to thread object
        // in a domain that has been unloaded. But it is possible that we started the unload, in which
        // case this thread wouldn't be allowed in or would be punted anyway.
        if (KickOffDomain != lpParam->pThread->GetDomain()->GetId())
            pThread->DoADCallBack(KickOffDomain, Stress_Thread_Proc_Worker, &args);
        else
            Stress_Thread_Proc_Worker(&args);
       
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

    delete (Stress_Thread_Param *) lpParameter;
    // Enable preemptive GC so a GC thread can suspend me.
    pThread->EnablePreemptiveGC();
    DestroyThread(pThread);

#ifndef TOTALLY_DISBLE_STACK_GUARDS 
    // We may have destroyed Thread object.  It is not safe to use cached Thread object in 
    // DebugSOIntolerantTransitionHandler or SOIntolerantTransitionHandler
    __soIntolerantTransitionHandler.SetThread(NULL);
#endif

    END_SO_INTOLERANT_CODE;  
    return args.retVal;
}


LONG StressThreadLock = 0;
static void Stress_Thread_Start (LPVOID lpParameter)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_ANY;
    }
    CONTRACT_END;

    Thread *pCurThread = GetThread();
    if (pCurThread->m_stressThreadCount == -1) {
        pCurThread->m_stressThreadCount = g_pConfig->GetStressThreadCount();
    }
    DWORD dwThreads = pCurThread->m_stressThreadCount;
    if (dwThreads <= 1)
        RETURN;

    Thread ** threads = new Thread* [dwThreads-1];

    while (FastInterlockCompareExchange(&StressThreadLock,1, 0) != 0)
        __SwitchToThread (1);


    FastInterlockExchange(&StressThreadLock, 0);

    DWORD n;
    for (n = 0; n < dwThreads-1; n ++)
    {
        threads[n] = SetupUnstartedThread();
        if (threads[n] == NULL)
            COMPlusThrowOM();

        threads[n]->m_stressThreadCount = dwThreads/2;
        Stress_Thread_Param *param = new Stress_Thread_Param;

        param->pFD = ((Stress_Thread_Param*)lpParameter)->pFD;
        param->argHandle = ((Stress_Thread_Param*)lpParameter)->argHandle;
        param->numSkipArgs = ((Stress_Thread_Param*)lpParameter)->numSkipArgs;
        param->EntryType = ((Stress_Thread_Param*)lpParameter)->EntryType;
        param->pThread = threads[n];
        if (!threads[n]->CreateNewThread(0, Stress_Thread_Proc, param))
        {
            delete param;
            threads[n]->DecExternalCount(FALSE);
            ThrowOutOfMemory();
        }
        threads[n]->SetThreadPriority (THREAD_PRIORITY_NORMAL);
    }

    for (n = 0; n < dwThreads-1; n ++)
    {
        threads[n]->StartThread();
    }
    __SwitchToThread (0);

    RETURN;
}

#endif

/* static */
HRESULT ClassLoader::RunMain(MethodDesc *pFD ,
                             short numSkipArgs,
                             INT32 *piRetVal,
                             PTRARRAYREF *stringArgs /*=NULL*/)
{
    STATIC_CONTRACT_THROWS;
    _ASSERTE(piRetVal);

    DWORD       cCommandArgs = 0;  // count of args on command line
    DWORD       arg = 0;
    LPWSTR      *wzArgs = NULL; // command line args
    HRESULT     hr = S_OK;

    *piRetVal = -1;

    // The exit code for the process is communicated in one of two ways.  If the
    // entrypoint returns an 'int' we take that.  Otherwise we take a latched
    // process exit code.  This can be modified by the app via setting
    // Environment's ExitCode property.
    if (stringArgs == NULL)
        SetLatchedExitCode(0);

    if (!pFD) {
        _ASSERTE(!"Must have a function to call!");
        return E_FAIL;
    }

    CorEntryPointType EntryType = EntryManagedMain;
    ValidateMainMethod(pFD, &EntryType);

    if ((EntryType == EntryManagedMain) &&
        (stringArgs == NULL)) {
        // If you look at the DIFF on this code then you will see a major change which is that we
        // no longer accept all the different types of data arguments to main.  We now only accept
        // an array of strings.

        wzArgs = CorCommandLine::GetArgvW(&cCommandArgs);
        // In the WindowsCE case where the app has additional args the count will come back zero.
        if (cCommandArgs > 0) {
            if (!wzArgs)
                return E_INVALIDARG;
        }
    }


    ETWTraceStartup::TraceEvent(ETW_TYPE_STARTUP_MAIN);
    TIMELINE_START(STARTUP, ("RunMain"));

    EX_TRY_NOCATCH
    {
        MethodDescCallSite  threadStart(pFD);
        
        PTRARRAYREF StrArgArray = NULL;
        GCPROTECT_BEGIN(StrArgArray);

        // Build the parameter array and invoke the method.
        if (EntryType == EntryManagedMain) {
            if (stringArgs == NULL) {
                // Allocate a COM Array object with enough slots for cCommandArgs - 1
                StrArgArray = (PTRARRAYREF) AllocateObjectArray((cCommandArgs - numSkipArgs), g_pStringClass);

                // Create Stringrefs for each of the args
                for( arg = numSkipArgs; arg < cCommandArgs; arg++) {
                    STRINGREF sref = COMString::NewString(wzArgs[arg]);
                    StrArgArray->SetAt(arg-numSkipArgs, (OBJECTREF) sref);
                }
            }
            else
                StrArgArray = *stringArgs;
        }

#ifdef STRESS_THREAD
        OBJECTHANDLE argHandle = (StrArgArray != NULL) ? CreateGlobalStrongHandle (StrArgArray) : NULL;
        Stress_Thread_Param Param = {pFD, argHandle, numSkipArgs, EntryType, 0};
        Stress_Thread_Start (&Param);
#endif

        ARG_SLOT stackVar = ObjToArgSlot(StrArgArray);

        if (pFD->IsVoid()) 
        {
            // Set the return value to 0 instead of returning random junk
            *piRetVal = 0;
            threadStart.Call(&stackVar);
        }
        else 
        {
            *piRetVal = (INT32)threadStart.Call_RetArgSlot(&stackVar);
            if (stringArgs == NULL) 
            {
                SetLatchedExitCode(*piRetVal);
            }
        }

        GCPROTECT_END();

        fflush(stdout);
        fflush(stderr);
    }
    EX_END_NOCATCH

    ETWTraceStartup::TraceEvent(ETW_TYPE_STARTUP_MAIN+1);
    TIMELINE_END(STARTUP, ("RunMain"));

    return hr;
}

LONG RunDllMainFilter(EXCEPTION_POINTERS* ep, LPVOID pv)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    BOOL useLastThrownObject = UpdateCurrentThrowable(ep->ExceptionRecord);
    DefaultCatchHandler(ep, NULL, useLastThrownObject, FALSE);

    return DefaultCatchFilter(ep, COMPLUS_EXCEPTION_EXECUTE_HANDLER);
}

HRESULT RunDllMain(MethodDesc *pMD, HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved)
{
    STATIC_CONTRACT_NOTHROW;

    _ASSERTE(!GetAppDomain()->IsPassiveDomain());

    HRESULT hr = S_OK;

    if (!pMD) {
        _ASSERTE(!"Must have a valid function to call!");
        return E_INVALIDARG;
    }

    if (pMD->IsIntrospectionOnly())
        return S_OK;

    PAL_TRY
    {
        EX_TRY_NOCATCH
        {
            // This call is inherently unverifiable entry point.
            if (!Security::CanSkipVerification(pMD)) {
                hr = SECURITY_E_UNVERIFIABLE;
                goto Done;
            }

            {
            SigPointer sig(pMD->GetSig());

            ULONG data;
            CorElementType eType;
            CorElementType eType2 = ELEMENT_TYPE_END;
            
            
            IfFailGoto(sig.GetData(&data), Done);
            if (data != IMAGE_CEE_CS_CALLCONV_DEFAULT) {
                hr = COR_E_METHODACCESS;
                goto Done;
            }

            IfFailGoto(sig.GetData(&data), Done);
            if (data != 3) {
                hr = COR_E_METHODACCESS;
                goto Done;
            }

            IfFailGoto(sig.GetElemType(&eType), Done);
            if (eType != ELEMENT_TYPE_I4) {                                     // return type = int32
                hr = COR_E_METHODACCESS;
                goto Done;
            }

            IfFailGoto(sig.GetElemType(&eType), Done);
            if (eType == ELEMENT_TYPE_PTR)
                IfFailGoto(sig.GetElemType(&eType2), Done);
                
            if (eType!= ELEMENT_TYPE_PTR || eType2 != ELEMENT_TYPE_VOID) {  // arg1 = void*
                hr = COR_E_METHODACCESS;
                goto Done;
            }

            IfFailGoto(sig.GetElemType(&eType), Done);
            if (eType != ELEMENT_TYPE_U4) {                                             // arg2 = uint32
                hr = COR_E_METHODACCESS;
                goto Done;
            }

            IfFailGoto(sig.GetElemType(&eType), Done);
            if (eType == ELEMENT_TYPE_PTR)
                IfFailGoto(sig.GetElemType(&eType2), Done);

            if (eType != ELEMENT_TYPE_PTR || eType2 != ELEMENT_TYPE_VOID) {  // arg3 = void*
                hr = COR_E_METHODACCESS;
                goto Done;
            }
            }

            {
                MethodDescCallSite  dllMain(pMD);

                // Set up a callstack with the values from the OS in the argument array
                ARG_SLOT stackVar[3];
                stackVar[0] = PtrToArgSlot(hInst);
                stackVar[1] = (ARG_SLOT) dwReason;
                stackVar[2] = PtrToArgSlot(lpReserved);

                // Call the method in question with the arguments.
                if((dllMain.Call_RetI4(&stackVar[0]) == 0)
                    &&(dwReason==DLL_PROCESS_ATTACH) 
                    && (EEConfig::GetConfigDWORD(L"IgnoreDllMainReturn",0,
                          (DWORD)REGUTIL::COR_CONFIG_ALL,TRUE,EEConfig::CONFIG_APPLICATION) != 1))
                {
                    hr = COR_E_INVALIDPROGRAM;
                }
            }
Done: ;
        }
        EX_END_NOCATCH
    }
    PAL_EXCEPT_FILTER(RunDllMainFilter, NULL)
    {
        Thread *pThread = GetThread();
        if (! pThread->PreemptiveGCDisabled())
            pThread->DisablePreemptiveGC();
        // don't do anything - just want to catch it
    }
    PAL_ENDTRY

    return hr;
}


// Returns true if this is a valid main method?
void ValidateMainMethod(MethodDesc * pFD, CorEntryPointType *pType)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());

        PRECONDITION(CheckPointer(pType));
    }
    CONTRACTL_END;

    // Must be static, but we don't care about accessibility
    if ((pFD->GetAttrs() & mdStatic) == 0)
        ThrowMainMethodException(pFD, IDS_EE_MAIN_METHOD_MUST_BE_STATIC);

    if (pFD->GetNumGenericClassArgs() != 0 || pFD->GetNumGenericMethodArgs() != 0)
        ThrowMainMethodException(pFD, IDS_EE_LOAD_BAD_MAIN_SIG);

        // Check for types
    PCCOR_SIGNATURE pCurMethodSig;
    DWORD       cCurMethodSig;

    pFD->GetSig(&pCurMethodSig, &cCurMethodSig);
    SigPointer sig(pCurMethodSig);

    ULONG nCallConv;
    if (FAILED(sig.GetData(&nCallConv)))
        ThrowMainMethodException(pFD, BFA_BAD_SIGNATURE);
    
    if (nCallConv != IMAGE_CEE_CS_CALLCONV_DEFAULT)
        ThrowMainMethodException(pFD, IDS_EE_LOAD_BAD_MAIN_SIG);

    ULONG nParamCount;
    if (FAILED(sig.GetData(&nParamCount)))
        ThrowMainMethodException(pFD, BFA_BAD_SIGNATURE);
    

    CorElementType nReturnType;
    if (FAILED(sig.GetElemType(&nReturnType)))
        ThrowMainMethodException(pFD, BFA_BAD_SIGNATURE);
    
    if ((nReturnType != ELEMENT_TYPE_VOID) && (nReturnType != ELEMENT_TYPE_I4) && (nReturnType != ELEMENT_TYPE_U4))
         ThrowMainMethodException(pFD, IDS_EE_MAIN_METHOD_HAS_INVALID_RTN);

    if (nParamCount == 0)
        *pType = EntryCrtMain;
    else {
        *pType = EntryManagedMain;

        if (nParamCount != 1)
            ThrowMainMethodException(pFD, IDS_EE_TO_MANY_ARGUMENTS_IN_MAIN);

        CorElementType argType;
        CorElementType argType2 = ELEMENT_TYPE_END;

        if (FAILED(sig.GetElemType(&argType)))
            ThrowMainMethodException(pFD, BFA_BAD_SIGNATURE);

        if (argType == ELEMENT_TYPE_SZARRAY)
            if (FAILED(sig.GetElemType(&argType2)))
                ThrowMainMethodException(pFD, BFA_BAD_SIGNATURE);
            
        if (argType != ELEMENT_TYPE_SZARRAY || argType2 != ELEMENT_TYPE_STRING)
            ThrowMainMethodException(pFD, IDS_EE_LOAD_BAD_MAIN_SIG);
    }
}

typedef struct _EEHandle {
    DWORD Status[1];
} EEHandle, *PEEHandle;

//-------------------------------------------------------------------------
// CorCommandLine state and methods
//-------------------------------------------------------------------------
// Class to encapsulate Cor Command line processing

// Statics for the CorCommandLine class
DWORD                CorCommandLine::m_NumArgs     = 0;
LPWSTR              *CorCommandLine::m_ArgvW       = 0;

LPWSTR               CorCommandLine::m_pwszAppFullName = NULL;
DWORD                CorCommandLine::m_dwManifestPaths = 0;
LPWSTR              *CorCommandLine::m_ppwszManifestPaths = NULL;
DWORD                CorCommandLine::m_dwActivationData = 0;
LPWSTR              *CorCommandLine::m_ppwszActivationData = NULL;

#ifdef _DEBUG
LPCWSTR  g_CommandLine;
#endif

// Set argvw from command line
/* static */
HRESULT CorCommandLine::SetArgvW(LPCWSTR lpCommandLine)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(return E_OUTOFMEMORY;);

        PRECONDITION(CheckPointer(lpCommandLine));
    }
    CONTRACTL_END

    HRESULT hr = S_OK;
    if(!m_ArgvW) {
        INDEBUG(g_CommandLine = lpCommandLine);

        InitializeLogging();        // This is so early, we may not be initialized
        LOG((LF_ALWAYS, LL_INFO10, "Executing program with command line '%S'\n", lpCommandLine));

        m_ArgvW = SegmentCommandLine(lpCommandLine, &m_NumArgs);

        // Now that we have everything in a convenient form, do all the COR-specific parsing.
        if (m_ArgvW)
            hr = ParseCor();
        else
            hr = E_OUTOFMEMORY;
    }

    return hr;
}

// Retrieve the command line
/* static */
LPWSTR* CorCommandLine::GetArgvW(DWORD *pNumArgs)
{
    LEAF_CONTRACT;

    if (pNumArgs != 0)
        *pNumArgs = m_NumArgs;

    return m_ArgvW;
}

HRESULT CorCommandLine::ReadClickOnceEnvVariables()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    HRESULT hr = S_OK;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);

    END_SO_INTOLERANT_CODE;

    return hr;
}

// Parse the command line ()
HRESULT CorCommandLine::ParseCor()
{
    LEAF_CONTRACT;

    HRESULT hr = ReadClickOnceEnvVariables();

    //
    // Old 1.x /cor behavior is preserved.
    //

    if (m_NumArgs >= 3)  // e.g. -COR "xxxx xxx" or /cor "xx"
        if ((m_ArgvW[1][0] == '/' || m_ArgvW[1][0] == '-') && (SString::_wcsicmp(m_ArgvW[1]+1, L"cor") == 0))
        {
            LPWSTR  pCorCmdLine = m_ArgvW[2];

            // The application doesn't see any of the COR arguments.  We don't have to
            // worry about releasing anything, because it's all allocated in a single
            // block -- which is how we release it in CorCommandLine::Shutdown().
            m_NumArgs -= 2;
            for (DWORD i=1; i<m_NumArgs; i++)
                m_ArgvW[i] = m_ArgvW[i+2];

            // Now whip through pCorCmdLine and set all the COR specific switches.
            // Assert if anything is in an invalid format and then ignore the whole
            // thing.
            WCHAR   *pWC1 = pCorCmdLine;

            if (*pWC1 == '"')
                pWC1++;

            while (*pWC1)
            {
                if (*pWC1 == ' ')
                {
                    pWC1++;
                    continue;
                }
                // Anything else is either the end, or a surprise
                break;
            }
        }

    return hr;
}

// -------------------------------------------------------
// Class loader stub manager functions & globals
// -------------------------------------------------------

#endif // #ifndef DACCESS_COMPILE

SPTR_IMPL(MethodDescPrestubManager, MethodDescPrestubManager, g_pManager);

#ifndef DACCESS_COMPILE

/* static */
void MethodDescPrestubManager::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    g_pManager = new MethodDescPrestubManager();        
    StubManager::AddStubManager(g_pManager);
}

#endif // #ifndef DACCESS_COMPILE

/* static */
BOOL MethodDescPrestubManager::CheckIsStub_Internal(TADDR stubStartAddress)
{
    CONTRACTL
    {
        THROWS; // address may be bad, so we may AV.
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    //
    // First, check if it looks like a stub.
    //
    PREFIX_ASSUME(stubStartAddress!=NULL);

#if defined(_X86_) || defined(_AMD64_)

#ifdef HAS_COMPACT_ENTRYPOINTS
    if (MethodDescChunk::IsCompactEntryPointAtAddress((TADDR)stubStartAddress))
    {
        if (*PTR_BYTE(stubStartAddress) != X86_INSTR_MOV_AL)
        {
            return FALSE;
        }
    }
    else
#endif // HAS_COMPACT_ENTRYPOINTS
    if (IS_ALIGNED(stubStartAddress, PRECODE_ALIGNMENT))
    {
        if (
            *PTR_BYTE(stubStartAddress) != X86_INSTR_MOV_EAX_IMM32
#ifdef HAS_FIXUP_PRECODE
            && *PTR_BYTE(stubStartAddress) != X86_INSTR_CALL_REL32 // unpatched fixup precode
            && *PTR_BYTE(stubStartAddress) != X86_INSTR_JMP_REL32 // patched fixup precode
#endif
#ifdef HAS_THISPTR_RETBUF_PRECODE
            && *PTR_DWORD(stubStartAddress) != 0xD189C889 // mov eax,ecx; mov ecx,edx
#endif
           )
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
#else
    PORTABILITY_ASSERT("MethodDescPrestubManager::CheckIsStub_Internal");
#endif

    if (!GetRangeList()->IsInRange((const BYTE *) stubStartAddress))
        return FALSE;


    return TRUE;

}

BOOL MethodDescPrestubManager::DoTraceStub(const BYTE *stubStartAddress,
                                           TraceDestination *trace)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END

    LOG((LF_CORDB, LL_EVERYTHING, "MethodDescPrestubManager::DoTraceStub called\n"));

    MethodDesc* pMD = NULL;
    TADDR target = NULL;

#ifdef HAS_COMPACT_ENTRYPOINTS
    if (MethodDescChunk::IsCompactEntryPointAtAddress((TADDR)stubStartAddress))
    {
        pMD = MethodDescChunk::GetMethodDescFromCompactEntryPoint((TADDR)stubStartAddress);
        target = (TADDR) ThePreStub()->GetEntryPoint();
    }
    else
#endif // HAS_COMPACT_ENTRYPOINTS
    {
        Precode* pPrecode = Precode::GetPrecodeFromEntryPoint((TADDR)stubStartAddress);
        PREFIX_ASSUME(pPrecode != NULL);

        switch (pPrecode->GetType())
        {
        case PRECODE_STUB:
            break;

#ifdef HAS_NDIRECT_IMPORT_PRECODE
        case PRECODE_NDIRECT_IMPORT:
#ifndef DACCESS_COMPILE
            trace->InitForUnmanaged((TADDR)GetEEFuncEntryPoint(NDirectImportThunk));
#else
            trace->InitForOther(NULL);
#endif
            LOG_TRACE_DESTINATION(trace, stubStartAddress, "MethodDescPrestubManager::DoTraceStub - NDirect import");
            return TRUE;
#endif // HAS_NDIRECT_IMPORT_PRECODE

#ifdef HAS_REMOTING_PRECODE
        case PRECODE_REMOTING:
#ifndef DACCESS_COMPILE
            trace->InitForManagerPush((TADDR)GetEEFuncEntryPoint(PrecodeRemotingThunk), this);
#else
            trace->InitForOther(NULL);
#endif
            LOG_TRACE_DESTINATION(trace, stubStartAddress, "MethodDescPrestubManager::DoTraceStub - remoting");
            return TRUE;
#endif // HAS_REMOTING_PRECODE

#ifdef HAS_FIXUP_PRECODE
        case PRECODE_FIXUP:
            break;
#endif // HAS_FIXUP_PRECODE

#ifdef HAS_THISPTR_RETBUF_PRECODE
        case PRECODE_THISPTR_RETBUF:
            break;
#endif // HAS_THISPTR_RETBUF_PRECODE

        default:
            _ASSERTE_IMPL(!"Unexpected precode type");
            break;
        }

        target = pPrecode->GetTarget();

        // check if the method has been jitted
        if (!pPrecode->IsPointingToPrestub(target))
        {
            trace->InitForStub(target);
            LOG_TRACE_DESTINATION(trace, stubStartAddress, "MethodDescPrestubManager::DoTraceStub - code");
            return TRUE;
        }

        pMD = pPrecode->GetMethodDesc();
    }

    PREFIX_ASSUME(pMD != NULL);

    // If the method is not IL, then we patch the prestub because no one will ever change the call here at the
    // MethodDesc. If, however, this is an IL method, then we are at risk to have another thread backpatch the call
    // here, so we'd miss if we patched the prestub. Therefore, we go right to the IL method and patch IL offset 0
    // by using TRACE_UNJITTED_METHOD.
    if (!pMD->IsIL())
    {
        trace->InitForStub(target);
    }
    else
    {
        trace->InitForUnjittedMethod(pMD);
    }

    LOG_TRACE_DESTINATION(trace, stubStartAddress, "MethodDescPrestubManager::DoTraceStub - prestub");
    return TRUE;
}

#ifndef DACCESS_COMPILE
BOOL MethodDescPrestubManager::TraceManager(Thread *thread,
                            TraceDestination *trace,
                            CONTEXT *pContext,
                            BYTE **pRetAddr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(thread, NULL_OK));
        PRECONDITION(CheckPointer(trace));
        PRECONDITION(CheckPointer(pContext));
        PRECONDITION(CheckPointer(pRetAddr));
    }
    CONTRACTL_END;

    BOOL bRet = FALSE;

#ifdef HAS_REMOTING_PRECODE
    LPVOID ip = GetIP(pContext);

    if (ip == GetEEFuncEntryPoint(PrecodeRemotingThunk))
    {
        BYTE** pStack = (BYTE**)GetSP(pContext);

        // Aligning down will handle differences in layout of virtual and nonvirtual remoting precodes
        Precode* pPrecode = (Precode*)ALIGN_DOWN(pStack[0] - sizeof(INT32) 
                - offsetof(RemotingPrecode,m_callRel32),
            PRECODE_ALIGNMENT);

        _ASSERTE(pPrecode->GetType() == PRECODE_REMOTING);

        // We need to tell the debugger where we're returning to just in case 
        // the debugger can't continue on.
        *pRetAddr = pStack[1];

        Object* pThis = (Object*)(size_t)pContext->Ecx;

        if ((pThis == NULL) || !CTPMethodTable::IsTPMethodTable(pThis->GetMethodTable()))
        {
            // No proxy in the way. Follow the target.
            trace->InitForStub(pPrecode->GetTarget());
        }
        else
        {
            // We have proxy in the way.
            trace->InitForFramePush(CTPMethodTable::GetTPStub()->GetPatchAddress());
        }

        bRet = TRUE;
    }
    else
#endif // HAS_REMOTING_PRECODE
    {
        _ASSERTE(!"Unexpected call to MethodDescPrestubManager::TraceManager");
    }

    return bRet;
}
#endif

MethodDesc * MethodDescPrestubManager::Entry2MethodDesc(const BYTE *stubStartAddress, MethodTable *pMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;


    if (!CheckIsStub_Worker((TADDR) stubStartAddress))
    {
        return NULL;
    }

#ifdef HAS_COMPACT_ENTRYPOINTS
    if (MethodDescChunk::IsCompactEntryPointAtAddress((TADDR)stubStartAddress))
    {
        return MethodDescChunk::GetMethodDescFromCompactEntryPoint((TADDR)stubStartAddress);
    }
    else
#endif // HAS_COMPACT_ENTRYPOINTS
    {
        Precode* pPrecode = Precode::GetPrecodeFromEntryPoint((TADDR)stubStartAddress);
        PREFIX_ASSUME(pPrecode != NULL);

        return pPrecode->GetMethodDesc();
    }

}

// -------------------------------------------------------
// StubLinkStubManager
// -------------------------------------------------------

SPTR_IMPL(StubLinkStubManager, StubLinkStubManager, g_pManager);

#ifndef DACCESS_COMPILE

/* static */
void StubLinkStubManager::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    g_pManager = new StubLinkStubManager();
    StubManager::AddStubManager(g_pManager);
}

#endif // #ifndef DACCESS_COMPILE

BOOL StubLinkStubManager::CheckIsStub_Internal(TADDR stubStartAddress)
{
    WRAPPER_CONTRACT;

    return GetRangeList()->IsInRange(stubStartAddress);
}

BOOL StubLinkStubManager::DoTraceStub(const BYTE *stubStartAddress,
                                      TraceDestination *trace)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(return FALSE;);
    }
    CONTRACTL_END

    LOG((LF_CORDB, LL_INFO10000,
         "StubLinkStubManager::DoTraceStub: stubStartAddress=0x%08x\n",
         stubStartAddress));

    Stub *stub = Stub::RecoverStub((TADDR)stubStartAddress);

    LOG((LF_CORDB, LL_INFO10000,
         "StubLinkStubManager::DoTraceStub: stub=0x%08x\n", stub));

    //
    // If this is an intercept stub, we may be able to step
    // into the intercepted stub.
    //
    //
    TADDR pRealAddr = 0;
    if (stub->IsIntercept()) 
    {
        InterceptStub *is = PTR_InterceptStub(PTR_HOST_TO_TADDR(stub));

        if (*is->GetInterceptedStub() == NULL) 
        {
            pRealAddr = *is->GetRealAddr();
            LOG((LF_CORDB, LL_INFO10000, "StubLinkStubManager::DoTraceStub"
                " Intercept stub, no following stub, real addr:0x%x\n",
                pRealAddr));
        }
        else 
        {
            stub = *is->GetInterceptedStub();

            pRealAddr = (TADDR)stub->GetEntryPoint();

            LOG((LF_CORDB, LL_INFO10000,
                 "StubLinkStubManager::DoTraceStub: intercepted "
                 "stub=0x%08x, ep=0x%08x\n",
                 stub, stub->GetEntryPoint()));
        }
        _ASSERTE( pRealAddr );

        // !!! will push a frame???
        return TraceStub(pRealAddr, trace);
    }
    else if (stub->IsMulticastDelegate()) 
    {
        LOG((LF_CORDB, LL_INFO10000,
             "StubLinkStubManager(MCDel)::DoTraceStub: stubStartAddress=0x%08x\n",
             stubStartAddress));

        LOG((LF_CORDB, LL_INFO10000,
             "StubLinkStubManager(MCDel)::DoTraceStub: stub=0x%08x MGR_PUSH to entrypoint:0x%x\n", stub,
             (BYTE*)stub->GetEntryPoint()));

        // If it's a MC delegate, then we want to set a BP & do a context-ful
        // manager push, so that we can figure out if this call will be to a
        // single multicast delegate or a multi multicast delegate
        trace->InitForManagerPush((TADDR)stubStartAddress, this);

        return TRUE;
    }
    else if (stub->GetPatchOffset() == 0) 
    {
        LOG((LF_CORDB, LL_INFO10000, "StubLinkStubManager::DoTraceStub: patch offset is 0!\n"));

        return FALSE;
    }
    else 
    {
        trace->InitForFramePush(stub->GetPatchAddress());

        LOG_TRACE_DESTINATION(trace, stubStartAddress, "StubLinkStubManager::DoTraceStub");

        return TRUE;
    }
}

#ifndef DACCESS_COMPILE

BOOL StubLinkStubManager::TraceManager(Thread *thread,
                                       TraceDestination *trace,
                                       CONTEXT *pContext,
                                       BYTE **pRetAddr)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(return FALSE;);
    }
    CONTRACTL_END

    // NOTE that we're assuming that this will be called if and ONLY if
    // we're examing a multicast delegate stub.  Otherwise, we'll have to figure out
    // what we're looking iat

    LPVOID pc;
    BYTE *pbDel;

    pc = GetIP(pContext);

#if defined(_X86_) // references to pContext->Ecx are x86 specific

    (*pRetAddr) = *(BYTE **)(size_t)(pContext->Esp);

    pbDel = (BYTE *)(size_t)pContext->Ecx;

#elif defined(_PPC_)
    // We need to find the delegate, which means we need to know which register has "this" (r3 or r4)
    // We can't decode the first instruction, since it has been replaced by the debugger with a trap
    // The third instruction is always a lwz r, thisReg, imm
    // where thisReg is what we want
    // so we decode the third instruction
    DWORD instr = *(DWORD*)((BYTE*)pc + 2*sizeof(DWORD));

    _ASSERTE((instr & 0xfc000000) == 0x80000000); // instr is a lwz
    int regThis = (instr & 0x001f0000) >> 16;
    _ASSERTE((regThis == 3) || (regThis == 4));

    *pRetAddr = (BYTE*)(size_t)pContext->Lr;
    pbDel = (BYTE*)(size_t)((regThis == 3) ? (pContext->Gpr3) : (pContext->Gpr4));

#else
    *pRetAddr = NULL;
    pbDel = NULL;
    PORTABILITY_ASSERT("StubLinkStubManager::TraceManager (clsload.cpp)");
#endif // _X86_

    LOG((LF_CORDB,LL_INFO10000, "SLSM:TM at 0x%x, retAddr is 0x%x\n", pc, (*pRetAddr)));

    return DelegateInvokeStubManager::TraceDelegateObject(pbDel, trace);
}

#endif // #ifndef DACCESS_COMPILE

// -------------------------------------------------------
// JumpStub stubs
//
// Stub manager for jump stubs created by ExecutionManager::jumpStub()
// These are currently used only on the 64-bit targets IA64 and AMD64
//
// -------------------------------------------------------

SPTR_IMPL(JumpStubStubManager, JumpStubStubManager, g_pManager);

#ifndef DACCESS_COMPILE
/* static */
void JumpStubStubManager::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    // These are currently used only on the 64-bit targets IA64 and AMD64
    g_pManager = new JumpStubStubManager();
    StubManager::AddStubManager(g_pManager);
}
#endif // #ifndef DACCESS_COMPILE

BOOL JumpStubStubManager::CheckIsStub_Internal(TADDR stubStartAddress)
{
    WRAPPER_CONTRACT;

    return GetRangeList()->IsInRange((const BYTE *) stubStartAddress);
}

BOOL JumpStubStubManager::DoTraceStub(const BYTE *stubStartAddress,
                                     TraceDestination *trace)
{
    LEAF_CONTRACT;

#ifndef DACCESS_COMPILE
    BYTE * pBuffer    = (BYTE *) stubStartAddress;
    BYTE * jumpTarget = decodeBackToBackJump(pBuffer);
    
    trace->InitForStub((TADDR) jumpTarget);
#else
    DacNotImpl();
#endif

    LOG_TRACE_DESTINATION(trace, stubStartAddress, "LdFtnStubManager::DoTraceStub");

    return TRUE;
}

MethodDesc * JumpStubStubManager::Entry2MethodDesc(const BYTE *stubStartAddress, MethodTable *pMT)
{
    WRAPPER_CONTRACT;

    if (CheckIsStub_Worker((TADDR) stubStartAddress))
    {
#ifndef DACCESS_COMPILE
        BYTE * pBuffer    = (BYTE *) stubStartAddress;
        BYTE * jumpTarget = decodeBackToBackJump(pBuffer);

        MethodDesc *pMD = IP2MethodDesc(jumpTarget);
        return pMD;
#else
        DacNotImpl();
#endif
    }

    return NULL;
}


//
// Stub manager for method entry points
// These are currently used only by the IA64 target
//
SPTR_IMPL(EntryPointStubManager, EntryPointStubManager, g_pManager);

#ifndef DACCESS_COMPILE

/* static */
void EntryPointStubManager::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    // These are currently used only by the IA64 target
}

#endif // #ifndef DACCESS_COMPILE

/* static */
BOOL EntryPointStubManager::CheckIsStub_Internal(TADDR stubStartAddress)
{
    WRAPPER_CONTRACT;

    return GetRangeList()->IsInRange((const BYTE *) stubStartAddress);
}

BOOL EntryPointStubManager::CheckIsStub_Static(const BYTE *stubStartAddress, const BYTE **stubTargetAddress)
{
    WRAPPER_CONTRACT;

    if (!g_pManager || !((StubManager*)g_pManager)->CheckIsStub_Worker((TADDR) stubStartAddress))
        return FALSE;

    TraceDestination trace;

    if (stubTargetAddress)
    {
        if (g_pManager->DoTraceStub(stubStartAddress, &trace))
        {
            *stubTargetAddress = (const BYTE*) trace.GetAddress();
        }
    }

    return TRUE;
}

BOOL EntryPointStubManager::DoTraceStub(const BYTE *stubStartAddress,
                                        TraceDestination *trace)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END

    LOG((LF_CORDB, LL_EVERYTHING, "EntryPointStubManager::DoTraceStub called\n"));
    BOOL result = FALSE;

#ifndef DACCESS_COMPILE
#else  // DACCESS_COMPILE
    DacNotImpl();
#endif // DACCESS_COMPILE


    LOG_TRACE_DESTINATION(trace, stubStartAddress, "EntryPointStubManager::DoTraceStub");


    return result;
}

MethodDesc* EntryPointStubManager::Entry2MethodDesc(const BYTE* stubStartAddress, MethodTable *pMT)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END

    MethodDesc* pMD = NULL;

#ifndef DACCESS_COMPILE
#else  // DACCESS_COMPILE
    DacNotImpl();
#endif // DACCESS_COMPILE

    return pMD;
}


//
// This is the stub manager for IL stubs, which are only used if
// STUBS_AS_IL is defined.
//
SPTR_IMPL(ILStubManager, ILStubManager, g_pManager);

#ifndef DACCESS_COMPILE

/* static */
void ILStubManager::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    g_pManager = new ILStubManager();
    StubManager::AddStubManager(g_pManager);
}

#endif // #ifndef DACCESS_COMPILE

BOOL ILStubManager::CheckIsStub_Internal(TADDR stubStartAddress)
{
    WRAPPER_CONTRACT;

    return GetRangeList()->IsInRange((const BYTE *) stubStartAddress);
}

BOOL ILStubManager::DoTraceStub(const BYTE *stubStartAddress, 
                                TraceDestination *trace)
{
    LEAF_CONTRACT;

    LOG((LF_CORDB, LL_EVERYTHING, "ILStubManager::DoTraceStub called\n"));

    MethodDesc* pILStubMD = NULL;

    INDEBUG(BOOL fIsInRange = ) GetRangeList()->IsInRange(stubStartAddress, (LPVOID*)&pILStubMD);

    _ASSERTE(fIsInRange);

    _ASSERTE(!"ILStubManager not used on this platform.");
    trace->InitForOther(NULL);
    return FALSE;

}

BOOL ILStubManager::AddStub(const BYTE* pILStub, MethodDesc* pMD)
{
    WRAPPER_CONTRACT;

    return FALSE;
}


// This is only used to recognize NDirectEntryPointStub() on WIN64.
SPTR_IMPL(NDirectDispatchStubManager, NDirectDispatchStubManager, g_pManager);

#ifndef DACCESS_COMPILE

/* static */
void NDirectDispatchStubManager::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    g_pManager = new NDirectDispatchStubManager();
    StubManager::AddStubManager(g_pManager);
}

#endif // #ifndef DACCESS_COMPILE

BOOL NDirectDispatchStubManager::CheckIsStub_Internal(TADDR stubStartAddress)
{
    WRAPPER_CONTRACT;

    {
        return false;
    }
}

BOOL NDirectDispatchStubManager::DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace)
{
    WRAPPER_CONTRACT;

    LOG((LF_CORDB, LL_EVERYTHING, "NDirectDispatchStubManager::DoTraceStub called\n"));

    _ASSERTE(!"NDirectDispatchStubManager not used on this platform.");
    trace->InitForOther(NULL);
    return FALSE;

}


// This is only used to recognize GenericComPlusCallStub[Worker]() on WIN64.
SPTR_IMPL(ComPlusDispatchStubManager, ComPlusDispatchStubManager, g_pManager);

#ifndef DACCESS_COMPILE

/* static */
void ComPlusDispatchStubManager::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    g_pManager = new ComPlusDispatchStubManager();
    StubManager::AddStubManager(g_pManager);
}

#endif // #ifndef DACCESS_COMPILE

BOOL ComPlusDispatchStubManager::CheckIsStub_Internal(TADDR stubStartAddress)
{
    WRAPPER_CONTRACT;

    {
        return false;
    }
}

BOOL ComPlusDispatchStubManager::DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace)
{
    WRAPPER_CONTRACT;

    LOG((LF_CORDB, LL_EVERYTHING, "ComPlusDispatchStubManager::DoTraceStub called\n"));

    _ASSERTE(!"ComPlusDispatchStubManager not used on this platform.");
    trace->InitForOther(NULL);
    return FALSE;

}



// This is used to put a patch when we enter managed code from unmanaged.
SPTR_IMPL(ReverseInteropStubManager, ReverseInteropStubManager, g_pManager);

#ifndef DACCESS_COMPILE

/* static */
void ReverseInteropStubManager::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    g_pManager = new ReverseInteropStubManager();
    StubManager::AddStubManager(g_pManager);
}

#endif // #ifndef DACCESS_COMPILE

BOOL ReverseInteropStubManager::CheckIsStub_Internal(TADDR stubStartAddress)
{
    WRAPPER_CONTRACT;

    {
        return false;
    }
}

BOOL ReverseInteropStubManager::DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace)
{
    WRAPPER_CONTRACT;

    LOG((LF_CORDB, LL_EVERYTHING, "ReverseInteropStubManager::DoTraceStub called\n"));

    _ASSERTE(!"ReverseInteropStubManager not used on this platform.");
    trace->InitForOther(NULL);
    return FALSE;

}

#if !defined(DACCESS_COMPILE)
BOOL ReverseInteropStubManager::TraceManager(Thread *thread,
                                      TraceDestination *trace,
                                      CONTEXT *pContext,
                                      BYTE **pRetAddr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;


    return FALSE;
}

// If pTargets is NULL, then the stub managers just need to increment the counter.
// Otherwise fill in the array using the counter as the index.
void ReverseInteropStubManager::GetVirtualTraceCallTarget(TADDR* pTargets, DWORD* pdwNumTargets)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION( CheckPointer(pdwNumTargets) );
    }
    CONTRACTL_END;


    return;
}
#endif // DACCESS_COMPILE


//
// Since we don't generate delegate invoke stubs at runtime on IA64, we
// can't use the StubLinkStubManager for these stubs.  Instead, we create
// an additional DelegateInvokeStubManager instead.
//
SPTR_IMPL(DelegateInvokeStubManager, DelegateInvokeStubManager, g_pManager);

#ifndef DACCESS_COMPILE

// static
void DelegateInvokeStubManager::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    g_pManager = new DelegateInvokeStubManager();
    StubManager::AddStubManager(g_pManager);
}

#endif // DACCESS_COMPILE

BOOL DelegateInvokeStubManager::AddStub(Stub* pStub)
{
    const BYTE* start = pStub->GetEntryPoint();

    // We don't really care about the size here.  We only stop in these stubs at the first instruction, 
    // so we'll never be asked to claim an address in the middle of a stub.
    return GetRangeList()->AddRange(start, start + 1, (LPVOID)start);

}

void DelegateInvokeStubManager::RemoveStub(Stub* pStub)
{
    const BYTE* start = pStub->GetEntryPoint();

    // We don't really care about the size here.  We only stop in these stubs at the first instruction, 
    // so we'll never be asked to claim an address in the middle of a stub.
    GetRangeList()->RemoveRanges((LPVOID)start);
}

BOOL DelegateInvokeStubManager::CheckIsStub_Internal(TADDR stubStartAddress)
{
    LEAF_CONTRACT;

    bool fIsStub = false;

#ifndef DACCESS_COMPILE
#endif // DACCESS_COMPILE

    fIsStub = fIsStub || GetRangeList()->IsInRange((const BYTE*)stubStartAddress);

    return fIsStub;
}

BOOL DelegateInvokeStubManager::DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace)
{
    LEAF_CONTRACT;

    LOG((LF_CORDB, LL_EVERYTHING, "DelegateInvokeStubManager::DoTraceStub called\n"));

    _ASSERTE(CheckIsStub_Internal((TADDR)(UINT_PTR)stubStartAddress));


    // If it's a MC delegate, then we want to set a BP & do a context-ful
    // manager push, so that we can figure out if this call will be to a
    // single multicast delegate or a multi multicast delegate
    trace->InitForManagerPush((TADDR) stubStartAddress, this);

    LOG_TRACE_DESTINATION(trace, stubStartAddress, "DelegateInvokeStubManager::DoTraceStub");

    return TRUE;
}


#if !defined(DACCESS_COMPILE)
BOOL DelegateInvokeStubManager::TraceManager(Thread *thread, TraceDestination *trace, 
                                             CONTEXT *pContext, BYTE **pRetAddr)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    LPVOID destAddr;
    LPVOID pc;
    pc = ::GetIP(pContext);

    BYTE* pThis;
    pThis = NULL;

    // Retrieve the this pointer from the context.
#if defined(_X86_)
    (*pRetAddr) = *(BYTE **)(size_t)(pContext->Esp);

    pThis = (BYTE*)(size_t)(pContext->Ecx);

    destAddr = *(LPVOID*)(pThis + DelegateObject::GetOffsetOfMethodPtrAux());

#elif defined(_PPC_)
    // We need to find the delegate, which means we need to know which register has "this" (r3 or r4)
    // We can't decode the first instruction, since it has been replaced by the debugger with a trap
    // The third instruction is always a lwz r, thisReg, imm
    // where thisReg is what we want
    // so we decode the third instruction
    DWORD instr = *(DWORD*)((BYTE*)pc + 2*sizeof(DWORD));

    _ASSERTE((instr & 0xfc000000) == 0x80000000); // instr is a lwz
    int regThis = (instr & 0x001f0000) >> 16;
    _ASSERTE((regThis == 3) || (regThis == 4));

    *pRetAddr = (BYTE*)(size_t)pContext->Lr;
    pThis     = (BYTE*)(size_t)((regThis == 3) ? (pContext->Gpr3) : (pContext->Gpr4));

    destAddr = *(LPVOID*)(pThis + DelegateObject::GetOffsetOfMethodPtrAux());

#endif // _X86_ || _PPC_ || _AMD64_

    LOG((LF_CORDB,LL_INFO10000, "DISM::TM: ppbDest: 0x%p\n", destAddr));
    
    BOOL res = StubManager::TraceStub((TADDR)destAddr, trace);
    LOG((LF_CORDB,LL_INFO10000, "DISM::TM: res: %d, result type: %d\n", res, trace->GetTraceType()));

    return res;
}

// static 
BOOL DelegateInvokeStubManager::TraceDelegateObject(BYTE* pbDel, TraceDestination *trace)
{
    BYTE **ppbDest = NULL;
    // If we got here, then we're here b/c we're at the start of a delegate stub 
    // need to figure out the kind of delegates we are dealing with

    BYTE *pbDelInvocationList = *(BYTE **)(pbDel + DelegateObject::GetOffsetOfInvocationList());

    LOG((LF_CORDB,LL_INFO10000, "DISM::TMI: invocationList: 0x%x\n", pbDelInvocationList));

    if (pbDelInvocationList == NULL)
    {

        ppbDest = (BYTE **)(pbDel + DelegateObject::GetOffsetOfMethodPtrAux());

        if (*ppbDest == NULL)
        {
            ppbDest = (BYTE **)(pbDel + DelegateObject::GetOffsetOfMethodPtr());

            if (*ppbDest == NULL)
            {
                // it's not looking good, bail out
                LOG((LF_CORDB,LL_INFO10000, "DISM(DelegateStub)::TM: can't trace into it\n"));
                return FALSE;
            }

        }

        LOG((LF_CORDB,LL_INFO10000, "DISM(DelegateStub)::TM: ppbDest: 0x%x *ppbDest:0x%x\n", ppbDest, *ppbDest));

        BOOL res = StubManager::TraceStub((TADDR) (*ppbDest), trace);

        LOG((LF_CORDB,LL_INFO10000, "DISM(MCDel)::TM: res: %d, result type: %d\n", res, trace->GetTraceType()));

        return res;
    }
    
    // invocationList is not null, so it can be one of the following:
    // Multicast, Static closed (special sig), Secure
    
    // rule out the static with special sig
    BYTE *pbCount = *(BYTE **)(pbDel + DelegateObject::GetOffsetOfInvocationCount());

    if (!pbCount)
    {
        // it's a static closed, the target lives in _methodAuxPtr
        ppbDest = (BYTE **)(pbDel + DelegateObject::GetOffsetOfMethodPtrAux());
        
        if (*ppbDest == NULL)
        {
            // it's not looking good, bail out
            LOG((LF_CORDB,LL_INFO10000, "DISM(DelegateStub)::TM: can't trace into it\n"));
            return FALSE;
        }
        
        LOG((LF_CORDB,LL_INFO10000, "DISM(DelegateStub)::TM: ppbDest: 0x%x *ppbDest:0x%x\n", ppbDest, *ppbDest));

        BOOL res = StubManager::TraceStub((TADDR) (*ppbDest), trace);

        LOG((LF_CORDB,LL_INFO10000, "DISM(MCDel)::TM: res: %d, result type: %d\n", res, trace->GetTraceType()));

        return res;
    }

    MethodTable *pType = *(MethodTable**)pbDelInvocationList;
    EEClass *cl = pType->GetClass();
    if (cl->IsAnyDelegateClass())
    {
        // this is a secure deelgate. The target is hidden inside this field, so recurse in and pray...
        return TraceDelegateObject(pbDelInvocationList, trace);
    }
    
    // Otherwise, we're going for the first invoke of the multi case.
    // In order to go to the correct spot, we have just have to fish out
    // slot 0 of the invocation list, and figure out where that's going to,
    // then put a breakpoint there...
    pbDel = *(BYTE**)(((ArrayBase *)pbDelInvocationList)->GetDataPtr());
    return TraceDelegateObject(pbDel, trace);
}

#endif // DACCESS_COMPILE


#ifdef DACCESS_COMPILE

void
ClassLoader::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_DTHIS();

    EMEM_OUT(("MEM: %p ClassLoader\n", PTR_HOST_TO_TADDR(this)));

    if (m_pAssembly.IsValid())
    {
        ModuleIterator modIter = GetAssembly()->IterateModules();

        while (modIter.Next())
        {
            modIter.GetModule()->EnumMemoryRegions(flags, true);
        }
    }
}

void
MethodDescPrestubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p MethodDescPrestubManager\n", PTR_HOST_TO_TADDR(this)));
    GetRangeList()->EnumMemoryRegions(flags);
}

void
StubLinkStubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p StubLinkStubManager\n", PTR_HOST_TO_TADDR(this)));
    GetRangeList()->EnumMemoryRegions(flags);
}

void
ThunkHeapStubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p ThunkHeapStubManager\n", PTR_HOST_TO_TADDR(this)));
    GetRangeList()->EnumMemoryRegions(flags);
}

void
JumpStubStubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p JumpStubStubManager\n", PTR_HOST_TO_TADDR(this)));
    GetRangeList()->EnumMemoryRegions(flags);
}

void
EntryPointStubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p EntryPointStubManager\n", PTR_HOST_TO_TADDR(this)));
    GetRangeList()->EnumMemoryRegions(flags);
}

void
ILStubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p ILStubManager\n", PTR_HOST_TO_TADDR(this)));
    GetRangeList()->EnumMemoryRegions(flags);
}

void
NDirectDispatchStubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p NDirectDispatchStubManager\n", PTR_HOST_TO_TADDR(this)));
}

void
ComPlusDispatchStubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p ComPlusDispatchStubManager\n", PTR_HOST_TO_TADDR(this)));
}

void
ReverseInteropStubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p ReverseInteropStubManager\n", PTR_HOST_TO_TADDR(this)));
}

void
DelegateInvokeStubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p DelegateInvokeStubManager\n", PTR_HOST_TO_TADDR(this)));
    GetRangeList()->EnumMemoryRegions(flags);
}

void
VirtualCallStubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p VirtualCallStubManager\n", PTR_HOST_TO_TADDR(this)));
    GetIndcellRangeList()->EnumMemoryRegions(flags);
    GetLookupRangeList()->EnumMemoryRegions(flags);
    GetResolveRangeList()->EnumMemoryRegions(flags);
    GetDispatchRangeList()->EnumMemoryRegions(flags);
    GetCacheEntryRangeList()->EnumMemoryRegions(flags);
}

#endif // #ifdef DACCESS_COMPILE
