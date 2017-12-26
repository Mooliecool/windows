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
// clsload.hpp
//
#ifndef _H_CLSLOAD
#define _H_CLSLOAD

#include "crst.h"
#include "eehash.h"
#include "vars.hpp"
#include "stubmgr.h"
#include "typehandle.h"
#include "object.h" // only needed for def. of PTRARRAYREF
#include "classloadlevel.h"
#include "specstrings.h"
#include "simplerwlock.hpp"

// SystemDomain is a friend of ClassLoader.
class SystemDomain;
class Assembly;
class ClassLoader;
class TypeKey;
struct EnCInfo;
interface IEnCErrorCallback;
class PendingTypeLoadEntry;
class PendingTypeLoadTable;
class PendingTypeLoadEntry_LockHolder;
class EEClass;
class Thread;
class EETypeHashTable;

// Hash table parameter for unresolved class hash
#define UNRESOLVED_CLASS_HASH_BUCKETS 8

// Used by Module Index / CL hash
#define MAX_CLASSES_PER_MODULE (1 << 21)

// This is information required to look up a type in the loader. Besides the
// basic name there is the meta data information for the type, whether the
// the name is case sensitive, and tokens not to load. This last item allows
// the loader to prevent a type from being recursively loaded.
typedef enum NameHandleTable
{
    nhCaseSensitive = 0,
    nhCaseInsensitive = 1
} NameHandleTable;


    // This token can be used on a pThrowable, it is just a token that says
    // don't return the throwable, just throw it instead

#define RETURN_ON_ERROR (NULL)                   // We do this all the time right now.
inline bool  pThrowableAvailable(OBJECTREF* pThrowable) {LEAF_CONTRACT;  return pThrowable != NULL; }

    // used in a catch handler, updates pThrowable GETHROWABLE, if available
#define UpdateThrowable(_pT)        \
    if (_pT != RETURN_ON_ERROR)     \
        (*_pT) = GET_THROWABLE();


class NameHandle
{
    friend class ClassLoader;

    LPCUTF8 m_nameSpace;
    LPCUTF8 m_name;

    PTR_Module m_pTypeScope;
    mdToken m_mdType;
    mdToken m_mdTokenNotToLoad;
    NameHandleTable m_WhichTable;
    PTR_EEClassHashEntry m_pBucket;

public:

    NameHandle()
    {
        LEAF_CONTRACT;
        memset((void*) this, NULL, sizeof(*this));
    }

    NameHandle(LPCUTF8 name) :
        m_nameSpace(NULL),
        m_name(name),
        m_pTypeScope(PTR_NULL),
        m_mdType(mdTokenNil),
        m_mdTokenNotToLoad(tdNoTypes),
        m_WhichTable(nhCaseSensitive),
        m_pBucket(PTR_NULL)
    {
        LEAF_CONTRACT;
    }

    NameHandle(LPCUTF8 nameSpace, LPCUTF8 name) :
        m_nameSpace(nameSpace),
        m_name(name),
        m_pTypeScope(PTR_NULL),
        m_mdType(mdTokenNil),
        m_mdTokenNotToLoad(tdNoTypes),
        m_WhichTable(nhCaseSensitive),
        m_pBucket(PTR_NULL)
    {
        LEAF_CONTRACT;
    }

    NameHandle(Module* pModule, mdToken token) :
        m_nameSpace(NULL),
        m_name(NULL),
        m_pTypeScope(pModule),
        m_mdType(token),
        m_mdTokenNotToLoad(tdNoTypes),
        m_WhichTable(nhCaseSensitive),
        m_pBucket(PTR_NULL)
    {
        LEAF_CONTRACT;
    }

    NameHandle(NameHandle& p)
    {
        LEAF_CONTRACT;
        m_nameSpace = p.m_nameSpace;
        m_name = p.m_name;
        m_pTypeScope = p.m_pTypeScope;
        m_mdType = p.m_mdType;
        m_mdTokenNotToLoad = p.m_mdTokenNotToLoad;
        m_WhichTable = p.m_WhichTable;
        m_pBucket = p.m_pBucket;
    }

    void SetName(LPCUTF8 pName)
    {
        LEAF_CONTRACT;
        m_name = pName;
    }

    void SetName(LPCUTF8 pNameSpace, LPCUTF8 pName)
    {
        LEAF_CONTRACT;
        m_nameSpace = pNameSpace;
        m_name = pName;
    }

    LPCUTF8 GetName() const
    {
        LEAF_CONTRACT;
        return m_name;
    }

    LPCUTF8 GetNameSpace() const
    {
        LEAF_CONTRACT;
        return m_nameSpace;
    }

    void SetTypeToken(Module* pModule, mdToken mdToken)
    {
        LEAF_CONTRACT;
        m_pTypeScope = PTR_Module(PTR_HOST_TO_TADDR(pModule));
        m_mdType = mdToken;
    }

    Module* GetTypeModule() const
    {
        LEAF_CONTRACT;
        return m_pTypeScope;
    }

    mdToken GetTypeToken() const
    {
        LEAF_CONTRACT;
        return m_mdType;
    }

    void SetTokenNotToLoad(mdToken mdtok)
    {
        LEAF_CONTRACT;
        m_mdTokenNotToLoad = mdtok;
    }

    mdToken GetTokenNotToLoad() const
    {
        LEAF_CONTRACT;
        return m_mdTokenNotToLoad;
    }

    void SetCaseInsensitive()
    {
        LEAF_CONTRACT;
        m_WhichTable = nhCaseInsensitive;
    }

    NameHandleTable GetTable() const
    {
        LEAF_CONTRACT;
        return m_WhichTable;
    }

    void SetBucket(EEClassHashEntry_t * pBucket)
    {
        LEAF_CONTRACT;
        m_pBucket = PTR_EEClassHashEntry(PTR_HOST_TO_TADDR(pBucket));
    }


    EEClassHashEntry_t * GetBucket()
    {
        LEAF_CONTRACT;
        return m_pBucket;
    }


#ifdef _DEBUG
    void Validate()
    {
        WRAPPER_CONTRACT;
        if(g_pConfig->fAssertOnBadImageFormat())
        {
            _ASSERTE(GetName());
            _ASSERTE(ns::IsValidName(GetName()));
        }
    }
#endif

    static BOOL OKToLoad(mdToken token, mdToken tokenNotToLoad)
    {
        LEAF_CONTRACT;

        return (token == 0 || token != tokenNotToLoad) && tokenNotToLoad != tdAllTypes;
    }

    BOOL OKToLoad()
    {
        WRAPPER_CONTRACT;
        return OKToLoad(m_mdType, m_mdTokenNotToLoad);
    }

};

class ClassLoader
{
    friend class MethodTableBuilder;
    friend class SystemDomain;
    friend class AppDomain;
    friend class Assembly;
    friend class Module;

    // the following three classes are friends because they will call LoadTypeHandleForTypeKey by token directly
    friend class COMDynamicWrite;
    friend class COMModule;
    friend class TypeLibExporter;
    friend class MemberLoader;
    friend class PendingTypeLoadEntry;

protected:
    static HRESULT RunMain(MethodDesc *pFD,
                           short numSkipArgs,
                           INT32 *piRetVal,
                           PTRARRAYREF *stringArgs = NULL);

private:
    // Classes for which load is in progress
    PendingTypeLoadTable  * m_pUnresolvedClassHash;
    CrstExplicitInit        m_UnresolvedClassLock;

    // Protects addition of elements to module's m_pAvailableClasses.
    // (indeed thus protects addition of elements to any m_pAvailableClasses in any
    // of the modules managed by this loader)
    CrstExplicitInit        m_AvailableClassLock;

    CrstExplicitInit        m_AvailableParamTypesLock;

    // Do we have any modules which need to have their classes added to
    // the available list?
    volatile LONG       m_cUnhashedModules;

    // Back reference to the assembly
    PTR_Assembly        m_pAssembly;

public:

    // Next classloader in global list
    PTR_ClassLoader     m_pNext;

#ifdef _DEBUG
    DWORD               m_dwDebugMethods;
    DWORD               m_dwDebugFieldDescs; // Doesn't include anything we don't allocate a FieldDesc for
    DWORD               m_dwDebugClasses;
    DWORD               m_dwDebugDuplicateInterfaceSlots;
    DWORD               m_dwDebugArrayClassRefs;
    DWORD               m_dwDebugConvertedSigSize;
    DWORD               m_dwGCSize;
    DWORD               m_dwInterfaceMapSize;
    DWORD               m_dwMethodTableSize;
    DWORD               m_dwVtableData;
    DWORD               m_dwStaticFieldData;
    DWORD               m_dwFieldDescData;
    DWORD               m_dwMethodDescData;
    size_t              m_dwEEClassData;
#endif

public:
    ClassLoader(Assembly *pAssembly);
    ~ClassLoader();

private:

    VOID PopulateAvailableClassHashTable(Module *pModule,
                                         AllocMemTracker *pamTracker);

    void LazyPopulateCaseInsensitiveHashTables();

    // Lookup the hash table entry from the hash table
    EEClassHashEntry_t *GetClassValue(NameHandleTable nhTable,
                                      NameHandle *pName,
                                      HashDatum *pData,
                                      EEClassHashTable **ppTable,
                                      Module* pLookInThisModuleOnly);


public:
    // This determines in which module an item gets placed.  For everything
    // except paramaterized types and methods the choice
    // is easy.
    //
    // If NGEN'ing we may choose to place the item into the current module.
    //
    // If running code then the rule for determining the loader
    // module must ensure that a type or method never outlives
    // its loader module with respect to app-domain unloading
    static Module* ComputeLoaderModule(Module *pDefinitionModule,      // the module that declares the generic type or method
                                       mdToken token,
                                       TypeHandle *pClassArgs,         // the type arguments to the type (if any)
                                       DWORD numClassArgs,             // the number of type arguménts to the type
                                       TypeHandle *pMethodArgs, // the type arguments to the method (if any)
                                       DWORD numMethodArgs);       // the number of type arguments to the method

    static Module* ComputeLoaderModule(MethodTable *pMT,
                                       mdToken token,           // the token of the method
                                       TypeHandle *pMethodArgs, // the type arguments to the method (if any)
                                       DWORD numMethodArgs);       // the number of type arguments to the method

    static Module *ComputeLoaderModule(TypeKey *typeKey);

private:
    BOOL FindClassModuleThrowing(NameHandle* pName,
                                 TypeHandle* pType,
                                 mdToken* pmdClassToken,
                                 Module** ppModule,
                                 mdToken *pmdFoundExportedType,
                                 EEClassHashEntry_t** ppEntry,
                                 Module* pLookInThisModuleOnly,
                                 Loader::LoadFlag loadFlag);

public:
    void Init(AllocMemTracker *pamTracker);

    static BOOL InitializeEE();

    Assembly* GetAssembly();
    DomainAssembly* GetDomainAssembly(AppDomain *pDomain = NULL);

    void    FreeModules();

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    //==================================================================================
    // Main entry points to class loader
    // Organized as follows:
    //   by token:
    //       TypeDef
    //       TypeDefOrRef
    //       TypeDefOrRefOrSpec
    //   by constructed type:
    //       ArrayType
    //       PointerOrByrefType
    //       FnPtrType
    //       GenericInstantiation
    //   by name:
    //       ByName
    // Each takes a parameter comes, with the following semantics:
    //       fLoadTypes=DontLoadTypes:  if type isn't already in the loader's table, return NULL
    //       fLoadTypes=LoadTypes: if type isn't already in the loader's table, then create it
    // Each comes in two variants, LoadXThrowing and LoadXNoThrow, the latter being just
    // a exception-handling wrapper around the former.
    //
    // Each also allows types to be loaded only up to a particular level (see classloadlevel.h).
    // The class loader itself makes use of these levels to "break" recursion across
    // generic instantiations. External clients should leave the parameter at its default
    // value (CLASS_LOADED).
    //==================================================================================

public:

    // We use enums for these flags so that we can easily search the codebase to
    // determine where the flags are set to their non-default values.
    //
    // This enum tells us what to do if the load fails.  If ThrowIfNotFound is used
    // with a HRESULT-returning NOTHROW function then it actually indicates that
    // an error-HRESULT will be returned.
    typedef enum { ThrowIfNotFound, ReturnNullIfNotFound } NotFoundAction;

    // This flag indicates whether we should accept an uninstantiatednaked TypeDef or TypeRef
    // for a generic type definition, where "uninstantiated" means "not used as part of
    // a TypeSpec"
    typedef enum { FailIfUninstDefOrRef, PermitUninstDefOrRef } PermitUninstantiatedFlag;

    // This flag indicates whether we want to "load" the type if it isn't already in the
    // loader's tables and has reached the load level desired.
    typedef enum { LoadTypes, DontLoadTypes } LoadTypesFlag;


    // Load types by token (Def, Ref and Spec)
    static TypeHandle LoadTypeDefThrowing(Module *pModule,
                                          mdToken typeDef,
                                          NotFoundAction fNotFound = ThrowIfNotFound,
                                          PermitUninstantiatedFlag fUninstantiated = FailIfUninstDefOrRef,
                                          mdToken tokenNotToLoad = tdNoTypes,
                                          ClassLoadLevel level = CLASS_LOADED);

    static TypeHandle LoadTypeDefOrRefThrowing(Module *pModule,
                                               mdToken typeRefOrDef,
                                               NotFoundAction fNotFound = ThrowIfNotFound,
                                               PermitUninstantiatedFlag fUninstantiated = FailIfUninstDefOrRef,
                                               mdToken tokenNotToLoad = tdNoTypes,
                                               ClassLoadLevel level = CLASS_LOADED);

    static TypeHandle LoadTypeDefOrRefOrSpecThrowing(Module *pModule,
                                                     mdToken typeRefOrDefOrSpec,
                                                     const SigTypeContext *pTypeContext,
                                                     NotFoundAction fNotFound = ThrowIfNotFound,
                                                     PermitUninstantiatedFlag fUninstantiated = FailIfUninstDefOrRef,
                                                     LoadTypesFlag fLoadTypes = LoadTypes,
                                                     ClassLoadLevel level = CLASS_LOADED,
                                                     BOOL dropGenericArgumentLevel = FALSE,
                                                     const Substitution *pSubst = NULL /* substitution to apply if the token is a type spec with generic variables */ );

    static HRESULT LoadTypeDefOrRefNoThrow(Module *pModule,
                                           mdToken typeRefOrDef,
                                           TypeHandle *pthOut,
                                           OBJECTREF *pThrowable,
                                           NotFoundAction fNotFound = ThrowIfNotFound,
                                           PermitUninstantiatedFlag fUninstantiated = FailIfUninstDefOrRef);


    static HRESULT LoadTypeDefOrRefOrSpecNoThrow(Module *pModule,
                                                 mdToken typeRefOrDefOrSpec,
                                                 const SigTypeContext *pTypeContext,
                                                 TypeHandle *pthOut,
                                                 OBJECTREF *pThrowable,
                                                 NotFoundAction fNotFound = ThrowIfNotFound,
                                                 PermitUninstantiatedFlag fUninstantiated = FailIfUninstDefOrRef,
                                                 LoadTypesFlag fLoadTypes = LoadTypes,
                                                 ClassLoadLevel level = CLASS_LOADED);

    // Load constructed types by providing their constituents
    static TypeHandle LoadPointerOrByrefTypeThrowing(CorElementType typ,
                                                     TypeHandle baseType,
                                                     LoadTypesFlag fLoadTypes = LoadTypes,
                                                     ClassLoadLevel level = CLASS_LOADED);

    static TypeHandle LoadArrayTypeThrowing(TypeHandle baseType,
                                            CorElementType typ = ELEMENT_TYPE_SZARRAY,
                                            unsigned rank = 0,
                                            LoadTypesFlag fLoadTypes = LoadTypes,
                                            ClassLoadLevel level = CLASS_LOADED);

    static TypeHandle LoadFnptrTypeThrowing(BYTE callConv,
                                            DWORD numArgs,
                                            TypeHandle* retAndArgTypes,
                                            LoadTypesFlag fLoadTypes = LoadTypes,
                                            ClassLoadLevel level = CLASS_LOADED);

    // Load types by name
    static TypeHandle LoadTypeByNameThrowing(Assembly *pAssembly,
                                             LPCUTF8 nameSpace,
                                             LPCUTF8 name,
                                             NotFoundAction fNotFound = ThrowIfNotFound,
                                             LoadTypesFlag fLoadTypes = LoadTypes,
                                             ClassLoadLevel level = CLASS_LOADED);

    static TypeHandle LoadTypeByNameNoThrow(Assembly *pAssembly,
                                            LPCUTF8 nameSpace,
                                            LPCUTF8 name,
                                            OBJECTREF *pThrowable,
                                            ClassLoadLevel level = CLASS_LOADED);

    // Resolve a TypeRef to a TypeDef
    // (Just a no-op on TypeDefs)
    // Return FALSE if operation failed (e.g. type does not exist)
    static BOOL ResolveTokenToTypeDefThrowing(Module *pTypeRefModule,
                                              mdTypeRef typeRefToken,
                                              Module **ppTypeDefModule,
                                              mdTypeDef *pTypeDefToken,
                                              Loader::LoadFlag loadFlag=Loader::Load);

    static void EnsureLoaded(TypeHandle typeHnd, ClassLoadLevel level = CLASS_LOADED);
    static void TryEnsureLoaded(TypeHandle typeHnd, ClassLoadLevel level = CLASS_LOADED);

public:
    // Look up a class by name
    //
    // Guaranteed to only return NULL if pName->OKToLoad() returns FALSE.
    // Thus when type loads are enabled this will return non-null.
    TypeHandle LoadTypeHandleThrowIfFailed(NameHandle* pName, ClassLoadLevel level = CLASS_LOADED,
                                           Module* pLookInThisModuleOnly=NULL);

public:
    // Looks up class in the local module table, if it is there it succeeds,
    // Otherwise it fails, This is meant only for optimizations etc
    static TypeHandle LookupTypeDefOrRefInModule(Module *pModule, mdToken cl);

private:

    VOID AddAvailableClassDontHaveLock(Module *pModule,
                                       mdTypeDef classdef,
                                       AllocMemTracker *pamTracker);

    VOID AddAvailableClassHaveLock(Module *pModule,
                                   mdTypeDef classdef,
                                   AllocMemTracker *pamTracker);

    VOID AddExportedTypeHaveLock(Module *pManifestModule,
                                 mdExportedType cl,
                                 AllocMemTracker *pamTracker);

public:

    static TypeHandle LoadCanonicalGenericArg(TypeHandle genericArg,
                                              LoadTypesFlag fLoadTypes = LoadTypes,
                                              ClassLoadLevel = CLASS_LOADED);

    // Create a generic instantiation.
    // If typeDef is not a generic type then throw an exception
    // If its arity does not match nGenericClassArgCount then throw an exception
    // The pointer to the instantiation is not persisted e.g. the type parameters can be stack-allocated.
    // If inst=NULL then <__Canon,...,__Canon> is assumed
    // If fLoadTypes=DontLoadTypes then the type handle is not created if it is not
    // already present in the tables.
    static TypeHandle LoadGenericInstantiationThrowing(Module *pModule,
                                                       mdTypeDef typeDef,
                                                       DWORD nGenericClassArgCount,
                                                       TypeHandle* pGenericClassArgs,
                                                       LoadTypesFlag fLoadTypes = LoadTypes,
                                                       ClassLoadLevel level = CLASS_LOADED);
    static TypeHandle LoadGenericInstantiationNoThrow(Module *pModule,
                                                      mdTypeDef typeDef,
                                                      DWORD nGenericClassArgCount,
                                                      TypeHandle* pGenericClassArgs,
                                                      OBJECTREF *pThrowable,
                                                      LoadTypesFlag fLoadTypes = LoadTypes,
                                                      ClassLoadLevel level = CLASS_LOADED);

    // Return TRUE if inst is the typical instantiation for the type or method specified by pModule/token
    static BOOL IsTypicalInstantiation(Module *pModule, mdToken token, DWORD numGenericArgs, TypeHandle *inst);

    // Can the objectref be cast to the type represented by pMT?
    // pMT cannot represent an array class, pRef can be an array
    // pMT can represent an instantiated type
    static BOOL CanCastToClassOrInterface(OBJECTREF pRef, MethodTable *pMT);

    static BOOL CanAccessMethod(MethodDesc *pCurrentMethod, MethodTable *pParentMT, MethodDesc *pMD);
    static BOOL CanAccessField(MethodDesc *pCurrentMethod, MethodTable *pParentMT, FieldDesc *pFD);
    static BOOL CanAccessMethodInstantiation(MethodDesc* pCallerMD, MethodTable *pCurrentClass, Assembly *pCurrentAssembly, MethodDesc *pOptionalTargetMethod);
    static BOOL CanAccessClass(MethodDesc* pCallerMD, MethodTable *pCurrentClass, Assembly *pCurrentAssembly,
                               MethodTable *pTargetClass, Assembly *pTargetAssembly);
    static BOOL CanAccess(MethodDesc* pCurrentMD, MethodTable *pCurrentClass, Assembly *pCurrentAssembly,
                          MethodTable *pTargetClass, Assembly *pTargetAssembly,
                          DWORD dwMemberAttrs, MethodDesc *pOptionalTargetMethod, FieldDesc* pOptionalTargetField = NULL);
    static BOOL CanAccess(MethodDesc* pCurrentMD, MethodTable *pCurrentClass, Assembly *pCurrentAssembly,
                          MethodTable *pTargetClass, Assembly *pTargetAssembly,
                          MethodTable *pInstanceClass, DWORD dwMemberAccess, MethodDesc *pOptionalTargetMethod, FieldDesc* pOptionalTargetField = NULL);
    static BOOL CanAccessMemberForExtraChecks(
                        MethodDesc*  pCurrentMD,
                        MethodTable *pCurrentMT,
                        Assembly    *pCurrentAssembly,
                        MethodTable *pTargetExactMT,
                        MethodDesc  *pOptionalTargetMethod,
                        FieldDesc   *pOptionalTargetField);
    static BOOL CanAccessSigForExtraChecks(
                        MethodDesc*  pCurrentMD,
                        MethodTable *pCurrentMT,
                        Assembly    *pCurrentAssembly,
                        MethodDesc  *pTargetMethodSig,
                        MethodTable *pTargetExactMT);
    static BOOL CanAccessSigForExtraChecks(
                        MethodDesc*  pCurrentMD,
                        PCCOR_SIGNATURE sig);
    static BOOL CanAccessClassForExtraChecks(
                        MethodDesc * pCurrentMD,
                        MethodTable *pCurrentClass,
                        Assembly *   pCurrentAssembly,
                        MethodTable *pTargetClass,
                        Assembly    *pTargetAssembly);
    static BOOL CanAccessFamily(MethodTable *pCurrentClass, MethodTable *pTargetClass,
                                MethodTable *pInstanceClass);
    static BOOL CheckAccess(MethodDesc* pCurrentMD, MethodTable *pCurrentClass, Assembly *pCurrentAssembly,
                            MethodTable *pTargetClass, Assembly *pTargetAssembly,
                            DWORD dwMemberAttrs,
                            MethodDesc *pOptionalTargetMethod, FieldDesc* pOptionalTargetField = NULL);
    static BOOL CheckAccess(MethodDesc* pCurrentMD, MethodTable *pCurrentClass, Assembly *pCurrentAssembly,
                            MethodTable *pTargetClass, Assembly *pTargetAssembly,
                            MethodTable *pInstanceClass, 
                            DWORD dwMemberAttrs,
                            MethodDesc *pOptionalTargetMethod, FieldDesc* pOptionalTargetField = NULL);

    //Creates a key with both the namespace and name converted to lowercase and
    //made into a proper namespace-path.
    VOID CreateCanonicallyCasedKey(LPCUTF8 pszNameSpace, LPCUTF8 pszName,
                                      LPUTF8 *ppszOutNameSpace, LPUTF8 *ppszOutName);

    static HRESULT FindTypeDefByExportedType(IMDInternalImport *pCTImport,
                                             mdExportedType mdCurrent,
                                             IMDInternalImport *pTDImport,
                                             mdTypeDef *mtd);

    class AvailableClasses_LockHolder : public CrstPreempHolder
    {
    public:
        AvailableClasses_LockHolder(ClassLoader *classLoader, BOOL Take = TRUE)
            : CrstPreempHolder(&classLoader->m_AvailableClassLock, Take)
        {
            WRAPPER_CONTRACT;
        }
    };
    friend class AvailableClasses_LockHolder;

private:

    static TypeHandle LoadConstructedTypeThrowing(TypeKey *pKey,
                                                  LoadTypesFlag fLoadTypes = LoadTypes,
                                                  ClassLoadLevel level = CLASS_LOADED);

    static TypeHandle LookupTypeKeyUnderLock(TypeKey *pKey,
                                             EETypeHashTable *pTable,
                                             CrstBase *pLock);

    static TypeHandle LookupTypeKey(TypeKey *pKey,
                                    EETypeHashTable *pTable,
                                    CrstBase *pLock,
                                    BOOL fCheckUnderLock);

    static TypeHandle LookupInLoaderModule(TypeKey* pKey, BOOL fCheckUnderLock);

    // Lookup a handle in the appropriate table 
    // (declaring module for TypeDef or loader-module for constructed types) 
    static TypeHandle LookupTypeHandleForTypeKey(TypeKey *pTypeKey);
    static TypeHandle LookupTypeHandleForTypeKeyInner(TypeKey *pTypeKey, BOOL fCheckUnderLock);

    static void DECLSPEC_NORETURN  ThrowTypeLoadException(TypeKey *pKey, UINT resIDWhy);


    BOOL IsNested(NameHandle* pName, mdToken *mdEncloser);
    static BOOL IsNested(Module *pModude, mdToken typeDefOrRef, mdToken *mdEncloser);

    // Helpers for FindClassModule()
    BOOL CompareNestedEntryWithTypeDef(IMDInternalImport *pImport,
                                       mdTypeDef mdCurrent,
                                       EEClassHashTable *pClassHash,
                                       EEClassHashEntry_t *pEntry);
    BOOL CompareNestedEntryWithTypeRef(IMDInternalImport *pImport,
                                       mdTypeRef mdCurrent,
                                       EEClassHashTable *pClassHash,
                                       EEClassHashEntry_t *pEntry);
    BOOL CompareNestedEntryWithExportedType(IMDInternalImport *pImport,
                                            mdExportedType mdCurrent,
                                            EEClassHashTable *pClassHash,
                                            EEClassHashEntry_t *pEntry);

public:
    //Attempts to find/load/create a type handle but does not throw
    // if used in "find" mode.
    TypeHandle LoadTypeHandleThrowing(NameHandle* pName, ClassLoadLevel level = CLASS_LOADED,
                                      Module* pLookInThisModuleOnly=NULL);

private:

#ifndef DACCESS_COMPILE
    // Perform a single phase of class loading
    // If no type handle has yet been created, typeHnd is null.
    static TypeHandle DoIncrementalLoad(TypeKey *pTypeKey, 
                                        TypeHandle typeHnd, 
                                        ClassLoadLevel workLevel);

    // Phase CLASS_LOAD_CREATE of class loading
    static TypeHandle CreateTypeHandleForTypeKey(TypeKey *pTypeKey,
                                                 AllocMemTracker *pamTracker);

    // Publish the type in the loader's tables
    static void PublishType(TypeKey *pTypeKey, TypeHandle typeHnd);

    // Notify profiler and debugger that a type load has completed
    // Also update perf counters
    static void Notify(TypeHandle typeHnd);

    // Phase CLASS_LOAD_EXACTPARENTS of class loading
    // Load exact parents and interfaces and dependent structures (generics dictionary, vtable fixes)
    static void LoadExactParents(MethodTable *pMT);

    static BOOL LoadExactParentAndInterfacesTransitively(MethodTable *pMT);


    static TypeHandle TryFindDynLinkZapType(TypeKey* pKey);

    // Create a non-canonical instantiation of a generic type based off the canonical instantiation
    // (For example, MethodTable for List<string> is based on the MethodTable for List<__Canon>)
    static TypeHandle CreateTypeHandleForNonCanonicalGenericInstantiation(TypeKey *pTypeKey,
                                                                          AllocMemTracker *pamTracker);

    // Loads a class. This is the inner call from the multi-threaded load. This load must
    // be protected in some manner.
    // If we're attempting to load a fresh instantiated type then genericArgs should be filled in

    static TypeHandle CreateTypeHandleForTypeDefThrowing(Module *pModule,
                                                         mdTypeDef cl,
                                                         TypeHandle* genericArgs,
                                                         AllocMemTracker *pamTracker);

    TypeHandle LoadTypeHandleForTypeKey(TypeKey *pTypeKey,
                                        TypeHandle typeHnd,
                                        ClassLoadLevel level = CLASS_LOADED);

    TypeHandle LoadTypeHandleForTypeKeyNoLock(TypeKey *pTypeKey,
                                              ClassLoadLevel level = CLASS_LOADED);

    // Used for initial loading of parent class and implemented interfaces
    // When tok represents an instantiated type return an *approximate* instantiated
    // type (where reference type arguments are replaced by Object)
    static TypeHandle LoadApproxTypeThrowing(Module *pModule,
                                             mdToken tok,
                                             PCCOR_SIGNATURE *formalInst,
                                             const SigTypeContext *pClassTypeContext);

    // Returns the parent of a token. The token must be a typedef.
    // If the parent is a shared constructed type (e.g. class C : List<string>) then
    // only the canonical instantiation is loaded at this point.
    // This is to avoid cycles in the loader e.g. on class C : D<C> or class C<T> : D<C<T>>
    // We fix up the exact parent later in LoadInstantiatedInfo.
    static MethodTable* LoadApproxParentThrowing(Module *pModule,
                                                 mdToken cl,
                                                 PCCOR_SIGNATURE* pParentInst,
                                                 const SigTypeContext *pClassTypeContext);

    // Locates the enclosing class of a token if any. The token must be a typedef.
    static VOID GetEnclosingClassThrowing(IMDInternalImport *pInternalImport,
                                          Module *pModule,
                                          mdTypeDef cl,
                                          mdTypeDef *tdEnclosing);
    
    // Insert the class in the classes hash table and if needed in the case insensitive one
    EEClassHashEntry_t *InsertValue(EEClassHashTable *pClassHash,
                                    EEClassHashTable *pClassCaseInsHash,
                                    LPCUTF8 pszNamespace,
                                    LPCUTF8 pszClassName,
                                    HashDatum Data,
                                    EEClassHashEntry_t *pEncloser,
                                    AllocMemTracker *pamTracker);

    // don't call this directly.
    TypeHandle LoadTypeHandleForTypeKey_Body(TypeKey *pTypeKey,
                                             TypeHandle typeHnd,
                                             ClassLoadLevel workLevel,
                                             PendingTypeLoadEntry_LockHolder *pLoadingEntryLockHolder,
                                             CrstHolder *pUnresolvedClassLockHolder);

    // don't call this directly.
    TypeHandle LoadTypeHandleForTypeKey_Inner(TypeKey *pTypeKey,
                                              TypeHandle typeHnd,
                                              ClassLoadLevel workLevel,
                                              PendingTypeLoadEntry_LockHolder *pLoadingEntryLockHolder,
                                              CrstHolder *pUnresolvedClassLockHolder,
                                              PendingTypeLoadEntry*& pPendingLoadingEntry);

#endif // #ifndef DACCESS_COMPILE

};

// Class to encapsulate Cor Command line processing
class CorCommandLine
{
public:

//********** TYPES

    // Note: We don't bother with interlocked operations as we manipulate these bits,
    // because we don't anticipate free-threaded access.  (Most of this is used only
    // during startup / shutdown).

//********** DATA

    // Hold the current (possibly parsed) command line here
    static DWORD            m_NumArgs;
    static LPWSTR          *m_ArgvW;

    static LPWSTR          m_pwszAppFullName;
    static DWORD           m_dwManifestPaths;
    static LPWSTR         *m_ppwszManifestPaths;
    static DWORD           m_dwActivationData;
    static LPWSTR         *m_ppwszActivationData;

//********** METHODS

    // parse the command line
    static HRESULT         SetArgvW(LPCWSTR lpCommandLine);

    // Retrieve the parsed command line
    static LPWSTR          *GetArgvW(DWORD *pNumArgs);

private:
    static HRESULT ReadClickOnceEnvVariables();
    static HRESULT ParseCor();
};

// -------------------------------------------------------
// This just wraps the RangeList methods in a read or
// write lock depending on the operation.
// -------------------------------------------------------

class LockedRangeList : public RangeList
{
  public:
    VPTR_VTABLE_CLASS(LockedRangeList, RangeList)
    
    LockedRangeList() : RangeList(), m_RangeListRWLock(COOPERATIVE_OR_PREEMPTIVE, LOCK_TYPE_DEFAULT)
    {
        WRAPPER_CONTRACT;
    }

    ~LockedRangeList()
    {
        LEAF_CONTRACT;
    }

  protected:

    virtual BOOL AddRangeWorker(const BYTE *start, const BYTE *end, void *id)
    {
        WRAPPER_CONTRACT;
        SimpleWriteLockHolder lh(&m_RangeListRWLock);
        return RangeList::AddRangeWorker(start,end,id);
    }

    virtual void RemoveRangesWorker(void *id, const BYTE *start = NULL, const BYTE *end = NULL)
    {
        WRAPPER_CONTRACT;
        SimpleWriteLockHolder lh(&m_RangeListRWLock);
        RangeList::RemoveRangesWorker(id,start,end);
    }

    virtual BOOL IsInRangeWorker(TADDR address, TADDR *pID = NULL)
    {
        WRAPPER_CONTRACT;
        SimpleReadLockHolder lh(&m_RangeListRWLock);
        return RangeList::IsInRangeWorker(address, pID);
    }

    virtual TADDR FindIdWithinRangeWorker(TADDR start, TADDR end)
    {
        WRAPPER_CONTRACT;
        SimpleReadLockHolder lh(&m_RangeListRWLock);
        return RangeList::FindIdWithinRangeWorker(start, end);
    }
  
    SimpleRWLock m_RangeListRWLock;
};

// -------------------------------------------------------
// Stub manager classes for method desc prestubs & normal
// frame-pushing, StubLinker created stubs
// -------------------------------------------------------

typedef VPTR(class MethodDescPrestubManager) PTR_MethodDescPrestubManager;

class MethodDescPrestubManager : public StubManager
{
    VPTR_VTABLE_CLASS(MethodDescPrestubManager, StubManager)

  public:

    SPTR_DECL(MethodDescPrestubManager, g_pManager);

#ifdef _DEBUG
        // Debug helper to help identify stub-managers.
        virtual const char * DbgGetName() { return "MethodDescPrestubManager"; }
#endif


    static void Init();

#ifndef DACCESS_COMPILE
    MethodDescPrestubManager() : StubManager(), m_rangeList() {WRAPPER_CONTRACT;}
    ~MethodDescPrestubManager() {LEAF_CONTRACT;}
#endif

  protected:
    LockedRangeList m_rangeList;
  public:
    // Get dac-ized pointer to rangelist.
    PTR_RangeList GetRangeList() 
    {
        TADDR addr = PTR_HOST_MEMBER_TADDR(MethodDescPrestubManager, this, m_rangeList);
        return PTR_RangeList(addr);
    }


    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress);

  private:

    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace);
#ifndef DACCESS_COMPILE
    virtual BOOL TraceManager(Thread *thread,
                              TraceDestination *trace,
                              CONTEXT *pContext,
                              BYTE **pRetAddr);
#endif

    virtual MethodDesc *Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT);

#ifdef DACCESS_COMPILE
    virtual void DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags);

  protected:
    virtual LPCWSTR GetStubManagerName(TADDR addr)
        { LEAF_CONTRACT; return L"MethodDescPrestub"; }
#endif
};

// Note that this stub was written by a debugger guy, and thus when he refers to 'multicast'
// stub, he really means multi or single cast stub.  This was done b/c the same stub
// services both types of stub.
// Note from the debugger guy: the way to understand what this manager does is to
// first grok EmitMulticastInvoke for the platform you're working on (right now, just x86).
// Then return here, and understand that (for x86) the only way we know which method
// we're going to invoke next is by inspecting EDI when we've got the debuggee stopped
// in the stub, and so our trace frame will either (FRAME_PUSH) put a breakpoint
// in the stub, or (if we hit the BP) examine EDI, etc, & figure out where we're going next.

typedef VPTR(class StubLinkStubManager) PTR_StubLinkStubManager;

class StubLinkStubManager : public StubManager
{
    VPTR_VTABLE_CLASS(StubLinkStubManager, StubManager)

  public:

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "StubLinkStubManager"; }
#endif    


    SPTR_DECL(StubLinkStubManager, g_pManager);

    static void Init();

#ifndef DACCESS_COMPILE
    StubLinkStubManager() : StubManager(), m_rangeList() {WRAPPER_CONTRACT;}
    ~StubLinkStubManager() {LEAF_CONTRACT;}
#endif
  
  protected:
    LockedRangeList m_rangeList;
  public:
    // Get dac-ized pointer to rangelist.
    PTR_RangeList GetRangeList() 
    {
        TADDR addr = PTR_HOST_MEMBER_TADDR(StubLinkStubManager, this, m_rangeList);
        return PTR_RangeList(addr);
    }


    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress);

  private:
    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace);
#ifndef DACCESS_COMPILE
    virtual BOOL TraceManager(Thread *thread,
                              TraceDestination *trace,
                              CONTEXT *pContext,
                              BYTE **pRetAddr);
#endif

    virtual MethodDesc *Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT)  {LEAF_CONTRACT; return NULL;}

#ifdef DACCESS_COMPILE
    virtual void DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags);

  protected:
    virtual LPCWSTR GetStubManagerName(TADDR addr)
        { LEAF_CONTRACT; return L"StubLinkStub"; }
#endif
} ;

// Stub manager for thunks.

typedef VPTR(class ThunkHeapStubManager) PTR_ThunkHeapStubManager;

class ThunkHeapStubManager : public StubManager
{
    VPTR_VTABLE_CLASS(ThunkHeapStubManager, StubManager)

  public:

    SPTR_DECL(ThunkHeapStubManager, g_pManager);

    static void Init();

#ifndef DACCESS_COMPILE
    ThunkHeapStubManager() : StubManager(), m_rangeList() {WRAPPER_CONTRACT;}
    ~ThunkHeapStubManager() {LEAF_CONTRACT;}
#endif

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "ThunkHeapStubManager"; }
#endif

  protected:
    LockedRangeList m_rangeList;
  public:
    // Get dac-ized pointer to rangelist.
    PTR_RangeList GetRangeList() 
    {
        TADDR addr = PTR_HOST_MEMBER_TADDR(ThunkHeapStubManager, this, m_rangeList);
        return PTR_RangeList(addr);
    }
    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress);

  private:
    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace);
    virtual MethodDesc *Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT)  {LEAF_CONTRACT; return NULL;}

#ifdef DACCESS_COMPILE
    virtual void DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags);

  protected:
    virtual LPCWSTR GetStubManagerName(TADDR addr)
        { LEAF_CONTRACT; return L"ThunkHeapStub"; }
#endif
};

//
// Stub manager for jump stubs created by ExecutionManager::jumpStub()
// These are currently used only on the 64-bit targets IA64 and AMD64
//
typedef VPTR(class JumpStubStubManager) PTR_JumpStubStubManager;

class JumpStubStubManager : public StubManager
{
    VPTR_VTABLE_CLASS(JumpStubStubManager, StubManager)

  public:

    SPTR_DECL(JumpStubStubManager, g_pManager);

    static void Init();

#ifndef DACCESS_COMPILE
    JumpStubStubManager() : StubManager(), m_rangeList() {WRAPPER_CONTRACT;}
    ~JumpStubStubManager() {LEAF_CONTRACT;}
#endif
  
  protected:
    LockedRangeList m_rangeList;
  public:
    // Get dac-ized pointer to rangelist.
    PTR_RangeList GetRangeList() 
    {
        TADDR addr = PTR_HOST_MEMBER_TADDR(JumpStubStubManager, this, m_rangeList);
        return PTR_RangeList(addr);
    }


#ifdef _DEBUG
    virtual const char * DbgGetName() { return "JumpStubStubManager"; }
#endif

    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress);

  private:

    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace);

    virtual MethodDesc *Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT);

#ifdef DACCESS_COMPILE
    virtual void DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags);

  protected:
    virtual LPCWSTR GetStubManagerName(TADDR addr)
        { LEAF_CONTRACT; return L"JumpStub"; }
#endif
};

//
// Stub manager for method entry points
// These are currently used only by the IA64 target
//
typedef VPTR(class EntryPointStubManager) PTR_EntryPointStubManager;

class EntryPointStubManager : public StubManager
{
    VPTR_VTABLE_CLASS(EntryPointStubManager, StubManager)

  public:

    SPTR_DECL(EntryPointStubManager, g_pManager);

    static void Init();

#ifndef DACCESS_COMPILE
    EntryPointStubManager() : StubManager(), m_rangeList() {WRAPPER_CONTRACT;}
    ~EntryPointStubManager() {LEAF_CONTRACT;}
#endif

   protected:
    LockedRangeList m_rangeList;
   public:
    // Get dac-ized pointer to rangelist.
    PTR_RangeList GetRangeList() 
    {
        TADDR addr = PTR_HOST_MEMBER_TADDR(EntryPointStubManager, this, m_rangeList);
        return PTR_RangeList(addr);
    }


#ifdef _DEBUG
    virtual const char * DbgGetName() { return "EntryPointStubManager"; }
#endif

    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress);

    static BOOL CheckIsStub_Static(const BYTE *stubStartAddress, const BYTE **stubTargetAddress);

  private:

    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace);

    virtual MethodDesc *Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT);

#ifdef DACCESS_COMPILE
    virtual void DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags);

  protected:
    virtual LPCWSTR GetStubManagerName(TADDR addr)
        { LEAF_CONTRACT; return L"EntryPointStub"; }
#endif
};

//
// This is the stub manager for IL stubs, which are only used if
// STUBS_AS_IL is defined.
//
typedef VPTR(class ILStubManager) PTR_ILStubManager;

class ILStubManager : public StubManager
{
    VPTR_VTABLE_CLASS(ILStubManager, StubManager)

  public:

    SPTR_DECL(ILStubManager, g_pManager);

    static void Init();

#ifndef DACCESS_COMPILE
    ILStubManager() : StubManager(), m_rangeList() {WRAPPER_CONTRACT;}
    ~ILStubManager() {LEAF_CONTRACT;}
#endif

   protected:
    LockedRangeList m_rangeList;
   public:
    // Get dac-ized pointer to rangelist.
    PTR_RangeList GetRangeList() 
    {
        TADDR addr = PTR_HOST_MEMBER_TADDR(ILStubManager, this, m_rangeList);
        return PTR_RangeList(addr);
    }


#ifdef _DEBUG
    virtual const char * DbgGetName() { return "ILStubManager"; }
#endif

    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress);

    virtual BOOL AddStub(const BYTE* pILStub, MethodDesc* pMD);

  private:

    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace);

    virtual MethodDesc *Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT) {LEAF_CONTRACT; return NULL;}

#ifdef DACCESS_COMPILE
    virtual void DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags);

  protected:
    virtual LPCWSTR GetStubManagerName(TADDR addr)
        { LEAF_CONTRACT; return L"ILStub"; }
#endif
};

// This is only used to recognize NDirectEntryPointStub() on WIN64.
typedef VPTR(class NDirectDispatchStubManager) PTR_NDirectDispatchStubManager;

class NDirectDispatchStubManager : public StubManager
{
    VPTR_VTABLE_CLASS(NDirectDispatchStubManager, StubManager)

  public:

    SPTR_DECL(NDirectDispatchStubManager, g_pManager);

    static void Init();

#ifndef DACCESS_COMPILE
    NDirectDispatchStubManager() : StubManager() {LEAF_CONTRACT;}
    ~NDirectDispatchStubManager() {LEAF_CONTRACT;}
#endif

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "NDirectDispatchStubManager"; }
#endif

    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress);

  private:

    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace);

    virtual MethodDesc *Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT)  {LEAF_CONTRACT; return NULL;}

#ifdef DACCESS_COMPILE
    virtual void DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags);

  protected:
    virtual LPCWSTR GetStubManagerName(TADDR addr)
        { LEAF_CONTRACT; return L"NDirectEntryPointStub"; }
#endif
};

// This is only used to recognize NDirectEntryPointStub() on WIN64.
typedef VPTR(class ComPlusDispatchStubManager) PTR_ComPlusDispatchStubManager;

class ComPlusDispatchStubManager : public StubManager
{
    VPTR_VTABLE_CLASS(ComPlusDispatchStubManager, StubManager)

  public:

    SPTR_DECL(ComPlusDispatchStubManager, g_pManager);

    static void Init();

#ifndef DACCESS_COMPILE
    ComPlusDispatchStubManager() : StubManager() {LEAF_CONTRACT;}
    ~ComPlusDispatchStubManager() {LEAF_CONTRACT;}
#endif

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "ComPlusDispatchStubManager"; }
#endif

    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress);

  private:

    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace);

    virtual MethodDesc *Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT)  {LEAF_CONTRACT; return NULL;}

#ifdef DACCESS_COMPILE
    virtual void DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags);

  protected:
    virtual LPCWSTR GetStubManagerName(TADDR addr)
        { LEAF_CONTRACT; return L"GenericComPlusCallStub"; }
#endif
};

// This is only used to recognize UMThunkStub() on WIN64.
typedef VPTR(class ReverseInteropStubManager) PTR_ReverseInteropStubManager;

class ReverseInteropStubManager : public StubManager
{
    VPTR_VTABLE_CLASS(ReverseInteropStubManager, StubManager)

  public:

    SPTR_DECL(ReverseInteropStubManager, g_pManager);

    static void Init();

#ifndef DACCESS_COMPILE
    ReverseInteropStubManager() : StubManager() {LEAF_CONTRACT;}
    ~ReverseInteropStubManager() {LEAF_CONTRACT;}
#endif

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "ReverseInteropILStub"; }
#endif

    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress);

  private:

    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace);

    virtual MethodDesc *Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT)  {LEAF_CONTRACT; return NULL;}

#if !defined(DACCESS_COMPILE)
    virtual BOOL TraceManager(Thread *thread, TraceDestination *trace, CONTEXT *pContext, BYTE **pRetAddr);
    virtual void GetVirtualTraceCallTarget(TADDR* pTargets, DWORD* pdwNumTargets);
#endif // DACCESS_COMPILE

#ifdef DACCESS_COMPILE
    virtual void DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags);

  protected:
    virtual LPCWSTR GetStubManagerName(TADDR addr)
        { LEAF_CONTRACT; return L"UMThunkStub"; }
#endif
};

//
// Since we don't generate delegate invoke stubs at runtime on WIN64, we
// can't use the StubLinkStubManager for these stubs.  Instead, we create
// an additional DelegateInvokeStubManager instead.
//
typedef VPTR(class DelegateInvokeStubManager) PTR_DelegateInvokeStubManager;

class DelegateInvokeStubManager : public StubManager
{
    VPTR_VTABLE_CLASS(DelegateInvokeStubManager, StubManager)

  public:

    SPTR_DECL(DelegateInvokeStubManager, g_pManager);

    static void Init();

#if !defined(DACCESS_COMPILE)
    DelegateInvokeStubManager() : StubManager(), m_rangeList() {WRAPPER_CONTRACT;}
    ~DelegateInvokeStubManager() {LEAF_CONTRACT;}
#endif // DACCESS_COMPILE

    BOOL AddStub(Stub* pStub);
    void RemoveStub(Stub* pStub);

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "DelegateInvokeStubManager"; }
#endif


    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress);

#if !defined(DACCESS_COMPILE)
    virtual BOOL TraceManager(Thread *thread, TraceDestination *trace, CONTEXT *pContext, BYTE **pRetAddr);
    static BOOL TraceDelegateObject(BYTE *orDel, TraceDestination *trace);
#endif // DACCESS_COMPILE

  private:

    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace);

    virtual MethodDesc *Entry2MethodDesc(const BYTE *StubStartAddress, MethodTable *pMT)  {LEAF_CONTRACT; return NULL;}

   protected:
    LockedRangeList m_rangeList;
   public:
    // Get dac-ized pointer to rangelist.
    PTR_RangeList GetRangeList() 
    {
        TADDR addr = PTR_HOST_MEMBER_TADDR(DelegateInvokeStubManager, this, m_rangeList);
        return PTR_RangeList(addr);
    }


#ifdef DACCESS_COMPILE
    virtual void DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags);

  protected:
    virtual LPCWSTR GetStubManagerName(TADDR addr)
        { LEAF_CONTRACT; return L"DelegateInvokeStub"; }
#endif
};

#endif /* _H_CLSLOAD */
