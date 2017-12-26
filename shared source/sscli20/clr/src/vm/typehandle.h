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
// ===========================================================================
// File: TYPEHANDLE.H
//
// ===========================================================================

#ifndef TYPEHANDLE_H
#define TYPEHANDLE_H

#include "check.h"
#include "classloadlevel.h"

class TypeDesc;
class TypeHandle;
class ArrayTypeDesc;
class FnPtrTypeDesc;
class ParamTypeDesc;
class TypeVarTypeDesc;
class MethodTable;
class EEClass;
class Module;
class Assembly;
class BaseDomain;
class MethodDesc;
class ZapCodeMap;
class TypeKey;
class TypeHandleList;
struct CORINFO_CLASS_STRUCT_;

#ifndef DEFINE_OBJECTREF
#define DEFINE_OBJECTREF
#ifdef _DEBUG_IMPL
class OBJECTREF;
#else
typedef PTR_Object OBJECTREF;
#endif
#endif

typedef DPTR(class TypeVarTypeDesc) PTR_TypeVarTypeDesc;
typedef SPTR(class FnPtrTypeDesc) PTR_FnPtrTypeDesc;
typedef DPTR(class ParamTypeDesc) PTR_ParamTypeDesc;
typedef DPTR(class ArrayTypeDesc) PTR_ArrayTypeDesc;
typedef DPTR(class TypeDesc) PTR_TypeDesc;


typedef CUnorderedArray<TypeHandle, 40> DFLPendingList;


/*************************************************************************/
// A TypeHandle is the FUNDAMENTAL concept of type identity in the CLR.
// That is two types are equal if and only if their type handles
// are equal.  A TypeHandle, is a pointer sized struture that encodes 
// everything you need to know to figure out what kind of type you are
// actually dealing with.  

// At the present time a TypeHandle can point at two possible things
//
//      1) A MethodTable    (Intrinsics, Classes, Value Types and their instantiations)
//      2) A TypeDesc       (all other cases: arrays, byrefs, pointer types, function pointers, generic type variables)  
//
// or with IL stubs, a third thing:
//
//      3) A MethodTable for a native value type.
//
// MTs that satisfy IsSharedByReferenceArrayTypes are not 
// valid TypeHandles: for example no allocated object will
// ever return such a type handle from Object::GetTypeHandle(), and
// these type handles should not be passed across the JIT Interface
// as CORINFO_CLASS_HANDLEs.  However some code in the EE does create 
// temporary TypeHandles out of these MTs, so we can't yet assert 
// !IsSharedByReferenceArrayTypes() in the TypeHandle constructor.
//
// Wherever possible, you should be using TypeHandles or MethodTables.
// Code that is known to work over Class/ValueClass types (including their
// instantaitions) is currently written to use MethodTables.
//
// TypeDescs in turn break down into several variants and are
// for special cases around the edges
//    - array types whose method tables get share
//    - types for function pointers for verification and reflection
//    - types for generic parameters for verification and reflection
//
// Generic type instantiations (in C# syntax: C<ty_1,...,ty_n>) are represented by
// MethodTables, i.e. a new MethodTable gets allocated for each such instantiation.
// The entries in these tables (i.e. the code) are, however, often shared.
// Clients of TypeHandle don't need to know any of this detail; just use the
// GetInstantiation, GetNumGenericArgs, and HasInstantiation methods.

class TypeHandle 
{
public:
    TypeHandle() { 
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        m_asPtr = 0; 
    }

    static TypeHandle FromPtr(void *aPtr)
    { 
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        return TypeHandle(aPtr);
    }
    static TypeHandle FromData(SIZE_T data)
    { 
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        return TypeHandle(data);
    }

    // When you ask for a class in JitInterface when all you have
    // is a methodDesc of an array method...
    // Convert from a JitInterface handle to an internal EE TypeHandle
    explicit TypeHandle(struct CORINFO_CLASS_STRUCT_*aPtr)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        m_asPtr = aPtr;
        // NormalizeUnsharedArrayMT();
        INDEBUGIMPL(Verify());
    }

    TypeHandle(MethodTable* aMT) {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        m_asTAddr = PTR_HOST_TO_TADDR(aMT); 
        // NormalizeUnsharedArrayMT();
        INDEBUGIMPL(Verify());
    }

    explicit TypeHandle(TypeDesc *aType) {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        _ASSERTE(aType);

        m_asTAddr = (PTR_HOST_TO_TADDR(aType) | 2); 
        INDEBUGIMPL(Verify());
    }


private:
    // These constructors have been made private.  You must use the explicit static functions
    // TypeHandle::FromPtr and TypeHandle::FromData instead of these constructors.  
    // Allowing a public constructor that takes a "void*" or a "SIZE_T" is error-prone.
    explicit TypeHandle(void* aPtr)
    { 
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        m_asPtr = aPtr;
        // NormalizeUnsharedArrayMT();
        INDEBUGIMPL(Verify());
    }
    
    explicit TypeHandle(SIZE_T data)
    { 
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        m_asTAddr = data;
        // NormalizeUnsharedArrayMT();
        INDEBUGIMPL(Verify());
    }

    
public:
    FORCEINLINE int operator==(const TypeHandle& typeHnd) const {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        return(m_asPtr == typeHnd.m_asPtr);
    }

    FORCEINLINE int operator!=(const TypeHandle& typeHnd) const {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        return(m_asPtr != typeHnd.m_asPtr);
    }

        // Methods for probing exactly what kind of a type handle we have
    FORCEINLINE BOOL IsNull() const { 
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return(m_asPtr == 0); 
    }

    FORCEINLINE BOOL IsUnsharedMT() const {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return((m_asTAddr & 2) == 0);
    }

    FORCEINLINE BOOL IsTypeDesc() const  {
        WRAPPER_CONTRACT;
        return(!IsUnsharedMT());
    }

    BOOL IsEnum() const;

    BOOL IsFnPtrType() const;
                             
    inline MethodTable* AsMethodTable() const;

    inline TypeDesc* AsTypeDesc() const;

    // To the extent possible, you should try to use methods like the ones
    // below that treat all types uniformly.

    // Gets the size that this type would take up embedded in another object
    // thus objects all return sizeof(void*).  
    unsigned GetSize() const;

    // Returns the type name, including the generic instantiation if possible.
    // See the TypeString class for better control over name formatting.
    void GetName(SString &result) const;

    // Returns the ELEMENT_TYPE_* that you would use in a signature
    // The only normalization that happens is that for type handles
    // for instantiated types (e.g. class List<String> or
    // value type Pair<int,int>)) this returns either ELEMENT_TYPE_CLASS
    // or ELEMENT_TYPE_VALUE, _not_ ELEMENT_TYPE_WITH.
    CorElementType GetSignatureCorElementType() const;
         
    // This helper:
    // - Will return enums underlying type
    // - Will return underlying primitive for System.Int32 etc...
    // - Will return underlying primitive as will be used in the calling convention
    //      For example
    //              struct t
    //              {
    //                  public int i;
    //              }
    //      will return ELEMENT_TYPE_I4 in x86 instead of ELEMENT_TYPE_VALUETYPE. We
    //      call this type of value type a primitive value type
    //
    // Internal representation is used among another things for the calling convention
    // (jit benefits of primitive value types) or optimizing marshalling.
    //
    // This will NOT convert E_T_ARRAY, E_T_SZARRAY etc. to E_T_CLASS (though it probably
    // should).  Use CorTypeInfo::IsObjRef for that.
    CorElementType GetInternalCorElementType() const; 

    // This helper will return the same as GetSignatureCorElementType except:
    // - Will return enums underlying type
    CorElementType GetVerifierCorElementType() const;

    //-------------------------------------------------------------------
    // CASTING
    // 
    // There are two variants of the "CanCastTo" method:
    //
    // CanCastTo
    // - restore encoded pointers on demand
    // - might throw, might trigger GC
    // - return type is boolean (FALSE = cannot cast, TRUE = can cast)
    //
    // CanCastToNoGC
    // - do not restore encoded pointers on demand
    // - does not throw, does not trigger GC
    // - return type is three-valued (CanCast, CannotCast, MaybeCast)
    // - MaybeCast indicates that the test tripped on an encoded pointer
    //   so the caller should now call CanCastTo if it cares
    //
    // Note that if the TypeHandle is a valuetype, the caller is responsible
    // for checking that the valuetype is in its boxed form before calling
    // CanCastTo. Otherwise, the caller should be using IsBoxedAndCanCastTo()
    typedef enum { CannotCast, CanCast, MaybeCast } CastResult;

    BOOL CanCastTo(TypeHandle type) const;
    BOOL IsBoxedAndCanCastTo(TypeHandle type) const;
    CastResult CanCastToNoGC(TypeHandle type) const;
    
    // get the parent (superclass) of this type
    TypeHandle GetPossiblyEncodedParent() const; 

    // Get the parent, known to be decoded
    TypeHandle GetParent() const;

    // Obtain element type for an array or pointer, returning NULL otherwise
    TypeHandle GetTypeParam() const;

    // Obtain instantiation from an instantiated type
    // NULL if not instantiated
    TypeHandle* GetInstantiation() const;

    // Does this type satisfy its class constraints, recursively up the hierarchy
    BOOL SatisfiesClassConstraints() const;

    TypeHandle Instantiate(TypeHandle* inst, INT32 cInst) const;
    TypeHandle MakePointer() const;
    TypeHandle MakeByRef() const;
    TypeHandle MakeSZArray() const;
    TypeHandle MakeArray(int rank) const;

    // Obtain instantiation from an instantiated type *or* a pointer to the element type for an array 
    TypeHandle* GetClassOrArrayInstantiation() const;

    // Is this type instantiated?
    BOOL HasInstantiation() const;

    // Is this a generic type whose type arguments are its formal type parameters?
    BOOL IsGenericTypeDefinition() const;

    // Is this either a non-generic type (e.g. a non-genric class type or an array type or a pointer type etc.)
    // or a generic type whose type arguments are its formal type parameters?
    //Equivalent to (!HasInstantiation() || IsGenericTypeDefinition());
    inline BOOL IsTypicalTypeDefinition() const;     

    // This enum describes the sharability of a generic type argument or complete instantiation
    typedef enum { NonShared, CanonicalShared, NonCanonicalShared } GenericSharingInfo;

    // Given the EEConfig settings for generic code sharing, determine
    // if the specified type representation induces a sharable 
    // set of compatible instantiations when used as a type parameter to 
    // a generic type or method.
    //
    // For example, when sharing at reference types "object" and "Struct<object>"
    // both induce sets of compatible instantiations, e.g. when used to build types
    // "List<object>" and "List<Struct<object>>" respectively.
    GenericSharingInfo GetTypeArgumentSharingInfo() const;

    // Similar to GetTypeArgumentSharingInfo, but applied to a vector.
    static GenericSharingInfo GetInstantiationSharingInfo(DWORD ntypars, TypeHandle *inst);

    // The base case of GetTypeArgumentSharingInfo.
    GenericSharingInfo GetTypeArgumentSharingInfoBaseCase() const;

    static BOOL IsSharableInstantiation(DWORD ntypars, TypeHandle *inst) 
    {
        return GetInstantiationSharingInfo(ntypars, inst) != NonShared;
    }

    // Recursively search the type arguments and if
    // one of the type arguments is Canon then return TRUE
    //
    // A<__Canon>    is the canonical TypeHandle (aka "representative" generic MT)
    // A<B<__Canon>> is a subtype that contains a Canonical type
    //
    BOOL IsCanonicalSubtype() const;

    // For an uninstantiated generic type, return the number of type parameters required for instantiation
    // For an instantiated type, return the number of type parameters in the instantiation
    // Otherwise return 0
    DWORD GetNumGenericArgs() const;

    // For an generic type instance return the representative within the class of
    // all type handles that share code.  For example, 
    //    <int> --> <int>,
    //    <object> --> <__Canon>,
    //    <string> --> <__Canon>,
    //    <List<string>> --> <__Canon>,
    //    <Struct<string>> --> <Struct<__Canon>>
    //
    // If the code for the type handle is not shared then return 
    // the type handle itself.
    TypeHandle LoadCanonicalGenericArg() const;

    BOOL IsValueType() const;
    BOOL IsInterface() const;
    BOOL IsAbstract() const;
    WORD GetNumVirtuals() const;

    inline DWORD IsObjectType() const
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return GetMethodTable() == g_pObjectClass;
    }

    DWORD IsThunking() const;

    // Retrieve the key corresponding to this handle
    TypeKey GetTypeKey() const;

    // To what level has this type been loaded?
    ClassLoadLevel GetLoadLevel();

    // Equivalent to GetLoadLevel() == CLASS_LOADED
    BOOL IsFullyLoaded();

    void DoFullyLoad(TypeHandleList *pVisited, ClassLoadLevel level, DFLPendingList *pPending, BOOL *pfBailed);

    inline void SetIsFullyLoaded();

 
#ifdef _DEBUG
    // Check that this type matches the key given 
    // i.e. that all aspects (element type, module/token, rank for arrays, instantiation for generic types) match up
    CHECK CheckMatchesKey(TypeKey *pKey) const;

    // Check that this type is loaded up to the level indicated
    // Also check that it is non-null
    CHECK CheckLoadLevel(ClassLoadLevel level);

    // Equivalent to CheckLoadLevel(CLASS_LOADED)
    CHECK CheckFullyLoaded();
#endif

#ifndef DACCESS_COMPILE

    BOOL IsBlittable() const;
    BOOL HasLayout() const;
    

#endif 

    // Unlike AsMethodTable, GetMethodTable will get the method table
    // of the type, regardless of whether it is an array etc. Note, however
    // this method table may be shared, and some types (like TypeByRef), have
    // no method table (and this function returns NULL for them)
    inline MethodTable* GetMethodTable() const;

    // Returns the method table which should be used for visibility checking.
    // Like GetMethodTable except for TypeDescs returns the root ElementType.
    // So for Foo[] instead of returning Array returns Foo. 
    inline MethodTable* GetMethodTableOfElementType() const;

    // Returns the MethodTable for the SZARRAY or ARRAY type
    inline MethodTable * GetPossiblySharedArrayMethodTable() const;

    // As above but returns a TypeHandle (so it will return a non-null result
    // for generic type variables, for instance).
    inline TypeHandle GetElementType() const;

    // Return the canonical representative MT amongst the set of MT's that share
    // code with the MT for the given TypeHandle because of generics.
    MethodTable* GetCanonicalMethodTable() const;

    // The module that defined the underlying type
    // (First strip off array/ptr qualifiers and generic type arguments)
    Module* GetModule() const;

    // The ngen'ed module where this type lives
    Module* GetZapModule() const;

    // Does this immediate item live in an NGEN module?
    BOOL IsZapped() const;

    // The module where this type lives for the purposes of loading and prejitting
    // See ComputeLoaderModule for more information
    Module* GetLoaderModule() const;

    // Is this type the principal owner of its EEClass?
    // This is true for 
    // - ordinary classes
    // - canonical instantiated types (e.g. List<__Canon> but not List<string>)
    // - array types that are the principal owner of their template method table (e.g. object[] and int[] but not string[])
    // Otherwise false
    BOOL OwnsEEClass() const;

    // Is the type fully visible from pContainingModule?
    // "foo" may not be visible to "List<foo>"->GetLoaderModule()
    BOOL IsScopedByModule(Module * pContainingModule) const;

    // The assembly that defined this type (== GetModule()->GetAssembly())
    Assembly* GetAssembly() const;

    // GetDomain on an instantiated type, e.g. C<ty1,ty2> returns the SharedDomain if all the
    // constituent parts of the type are SharedDomain (i.e. domain-neutral), 
    // and returns an AppDomain if any of the parts are from an AppDomain, 
    // i.e. are domain-bound.  If any of the parts are domain-bound
    // then they will all belong to the same domain.
    BaseDomain *GetDomain() const;

    BOOL IsDomainNeutral() const;

    // BEWARE using this on instantiated types whose EEClass pointer may be shared
    // between compatible instantiations as described above
    EEClass* GetClass() const;

    // Get the class token, assuming the type handle represents a named type,
    // i.e. a class, a value type, a generic instantiation etc.
    inline mdTypeDef GetCl() const;

    // Shortcuts

    // ARRAY or SZARRAY TypeDesc (arrays with a shared MethodTable)
    // If this is TRUE, it is OK to call AsArray()
    // Also see IsArrayType()
    BOOL IsArray() const;

    // See comment of IsArrayType() for the explanation of this method

    //
    //
    BOOL IsArrayType() const;

    // VAR or MVAR
    BOOL IsGenericVariable() const;

    // BYREF
    BOOL IsByRef() const;

    // PTR
    BOOL IsPointer() const;

    // True if this type *is* a formal generic type parameter or any component of it is a formal generic type parameter
    BOOL ContainsGenericVariables(BOOL methodOnly=FALSE) const;

    Module* GetDefiningModuleForOpenType() const;    

    // Is actually ParamTypeDesc (ARRAY, SZARRAY, BYREF, PTR)
    BOOL HasTypeParam() const;

    BOOL IsRestored() const;

    // Does this type have zap-encoded components (generic arguments, etc)?
    BOOL HasUnrestoredTypeKey() const;

    // True if this type handle is a zap-encoded fixup
    BOOL IsEncodedFixup() const;

    // Only used at NGEN-time
    BOOL ComputeNeedsRestore(TypeHandleList *pVisited) const;

    void DoRestoreTypeKey();    

    void CheckRestore() const;
    BOOL IsExternallyVisible() const;

    // Is this type part of an assembly loaded for introspection?
    BOOL IsIntrospectionOnly() const;

    // Not clear we should have this.  
    inline ArrayTypeDesc* AsArray() const;
    
#ifndef DACCESS_COMPILE
    inline FnPtrTypeDesc* AsFnPtrType() const
    { 
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
  
        _ASSERTE(IsFnPtrType());

        FnPtrTypeDesc* result = PTR_FnPtrTypeDesc(m_asTAddr & ~2);
        PREFIX_ASSUME(result != NULL);
        return result;
    }
#else
    FnPtrTypeDesc* AsFnPtrType();
#endif
    
#ifndef DACCESS_COMPILE
    inline TypeVarTypeDesc* AsGenericVariable() const
    { 
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
    
        _ASSERTE(IsGenericVariable() && IsTypeDesc());

        TypeVarTypeDesc* result = PTR_TypeVarTypeDesc(m_asTAddr & ~2);
        PREFIX_ASSUME(result != NULL);
        return result;
    }
#else
    TypeVarTypeDesc* AsGenericVariable() const;
#endif
    
    TypeHandle* GetInstantiationOfParentClass(MethodTable *pWhichParent) const;

    void* AsPtr() const {                     // Please don't use this if you can avoid it
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        return((void *) m_asPtr); 
    }

    TADDR AsTAddr() const {       
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        return m_asTAddr;
    }

    INDEBUGIMPL(BOOL Verify();)             // DEBUGGING Make certain this is a valid type handle 

#if CHECK_APP_DOMAIN_LEAKS
    BOOL IsAppDomainAgile() const;
    BOOL IsCheckAppDomainAgile() const;

    BOOL IsArrayOfElementsAppDomainAgile() const;
    BOOL IsArrayOfElementsCheckAppDomainAgile() const;
#endif

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
    
    OBJECTREF GetManagedClassObject() const;
    OBJECTREF GetManagedClassObjectIfExists() const;
    
    static TypeHandle MergeArrayTypeHandlesToCommonParent(
        TypeHandle ta, TypeHandle tb);

    static TypeHandle MergeTypeHandlesToCommonParent(
        TypeHandle ta, TypeHandle tb);


#ifdef DEBUGGING_SUPPORTED
    BOOL NotifyDebuggerLoad(AppDomain *domain, BOOL attaching) const;
    void NotifyDebuggerUnload(AppDomain *domain) const;
#endif // DEBUGGING_SUPPORTED

private:
    static TypeHandle MergeClassWithInterface(
        TypeHandle tClass, TypeHandle tInterface);

    union 
    {
        TADDR             m_asTAddr;      // we look at the low order bits
        void*             m_asPtr;
#ifndef DACCESS_COMPILE
        PTR_MethodTable   m_asMT;
        PTR_TypeDesc      m_asTypeDesc;
        PTR_ArrayTypeDesc      m_asArrayTypeDesc;
        PTR_ParamTypeDesc      m_asParamTypeDesc;
        PTR_TypeVarTypeDesc      m_asTypeVarTypeDesc;
        PTR_FnPtrTypeDesc m_asFnPtrTypeDesc;
#endif
    };
};

class TypeHandleList
{
    TypeHandle m_typeHandle;
    TypeHandleList* m_pNext;
 public:
    TypeHandleList(TypeHandle t, TypeHandleList* pNext) : m_typeHandle(t),m_pNext(pNext) { };
    static BOOL Exists(TypeHandleList* pList, TypeHandle t)
    {
        LEAF_CONTRACT;
        while (pList != NULL) { if (pList->m_typeHandle == t) return TRUE; pList = pList->m_pNext; }
        return FALSE;
    }
};

#if CHECK_INVARIANTS
inline CHECK CheckPointer(TypeHandle th, IsNullOK ok = NULL_NOT_OK)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    if (th.IsNull())
    {
        CHECK_MSG(ok, "Illegal null TypeHandle");
    }
    else
    {
        __if_exists(TypeHandle::Check)
        {
            CHECK(th.Check());
        }
    }

    CHECK_OK;
}

#endif  // CHECK_INVARIANTS


#endif // TYPEHANDLE_H
