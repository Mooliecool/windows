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
// File: TYPEDESC.H
// ===========================================================================

#ifndef TYPEDESC_H
#define TYPEDESC_H
#include <specstrings.h>

class TypeHandleList;
class ZapCodeMap;

/*************************************************************************/
/* TypeDesc is a discriminated union of all types that can not be directly
   represented by a simple MethodTable*.   The discrimintor of the union at the present
   time is the CorElementType numeration.  The subclass of TypeDesc are
   the possible variants of the union.  


   ParamTypeDescs only include byref, array and pointer types.  They do NOT
   include instantaitions of generic types, which are represented by MethodTables.
*/ 


typedef DPTR(class TypeDesc) PTR_TypeDesc;

class TypeDesc 
{
public:
#ifndef DACCESS_COMPILE
    TypeDesc(CorElementType type) { 
        LEAF_CONTRACT;

        m_typeAndFlags = type;
    }
#endif

    // This is the ELEMENT_TYPE* that would be used in the type sig for this type
    // For enums this is the uderlying type
    inline CorElementType GetInternalCorElementType() { 
        LEAF_CONTRACT;

        return (CorElementType) (m_typeAndFlags & 0xff);
    }

    // Get the exact parent (superclass) of this type  
    TypeHandle GetParent();

    // Returns the name of the array.  Note that it returns
    // the length of the returned string 
    static void ConstructName(CorElementType kind,
                              TypeHandle param,
                              int rank,
                              SString &ssBuff);

    void GetName(SString &ssBuf);

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

    BOOL CanCastTo(TypeHandle type);
    TypeHandle::CastResult CanCastToNoGC(TypeHandle type);

    // BYREF
    BOOL TypeDesc::IsByRef() {              // BYREFS are often treated specially 
        WRAPPER_CONTRACT;

        return(GetInternalCorElementType() == ELEMENT_TYPE_BYREF);
    }

    // PTR
    BOOL TypeDesc::IsPointer() {
        WRAPPER_CONTRACT;

        return(GetInternalCorElementType() == ELEMENT_TYPE_PTR);
    }

    // ARRAY, SZARRAY
    BOOL IsArray();

    // VAR, MVAR
    BOOL IsGenericVariable();

    // Is actually ParamTypeDesc (ARRAY, SZARRAY, BYREF, PTR)
    BOOL HasTypeParam();


    void DoRestoreTypeKey();
    void Restore();
    BOOL IsRestored();
    BOOL IsRestored_NoLogging();
    void SetIsRestored();

    inline BOOL HasUnrestoredTypeKey() const
    {
        LEAF_CONTRACT;

        return (m_typeAndFlags & TypeDesc::enum_flag_UnrestoredTypeKey) != 0;       
    }

    BOOL IsFullyLoaded() const
    {
        LEAF_CONTRACT;

        return (m_typeAndFlags & TypeDesc::enum_flag_IsNotFullyLoaded) == 0;       
    }


    VOID SetIsFullyLoaded()
    {
        LEAF_CONTRACT;
        FastInterlockAnd(&m_typeAndFlags, ~TypeDesc::enum_flag_IsNotFullyLoaded);
    }

    ClassLoadLevel GetLoadLevel();

    void DoFullyLoad(TypeHandleList *pVisited, ClassLoadLevel level, DFLPendingList *pPending, BOOL *pfBailed);

    // The module that defined the underlying type
    Module* GetModule();

    // The ngen'ed module where this type-desc lives
    Module* GetZapModule();

    // The module where this type lives for the purposes of loading and prejitting
    // See ComputeLoaderModule for more information
    Module* GetLoaderModule();
    
    // The assembly that defined this type (== GetModule()->GetAssembly())
    Assembly* GetAssembly();

    MethodTable*  GetMethodTable();         // only meaningful for ParamTypeDesc
    TypeHandle GetTypeParam();              // only meaningful for ParamTypeDesc
    TypeHandle* GetClassOrArrayInstantiation();      // only meaningful for ParamTypeDesc; see above

    // Note that if the TypeDesc, e.g. a function pointer type, involves parts that may
    // come from either a SharedDomain or an AppDomain then special rules apply to GetDomain.
    // It returns the SharedDomain if all the
    // constituent parts of the type are SharedDomain (i.e. domain-neutral), 
    // and returns an AppDomain if any of the parts are from an AppDomain, 
    // i.e. are domain-bound.  If any of the parts are domain-bound
    // then they will all belong to the same domain.
    BaseDomain *GetDomain();
    BOOL IsDomainNeutral();

 protected:
    // See methodtable.h for details of the flags with the same name there
    enum
    {
        enum_flag_NeedsRestore           = 0x00000100, // Only used during ngen
        enum_flag_PreRestored            = 0x00000200, // Only used during ngen
        enum_flag_Unrestored             = 0x00000400, 
        enum_flag_UnrestoredTypeKey      = 0x00000800,
        enum_flag_IsNotFullyLoaded       = 0x00001000,
        enum_flag_DependenciesLoaded     = 0x00002000,
    };
    //
    // Low-order 8 bits of this flag are used to store the CorElementType, which
    // discriminates what kind of TypeDesc we are
    //
    // The remaining bits are available for flags
    //
    DWORD m_typeAndFlags;
};


/*************************************************************************/
// This variant is used for parameterized types that have exactly one argument
// type.  This includes arrays, byrefs, pointers.  

typedef DPTR(class ParamTypeDesc) PTR_ParamTypeDesc;

class ParamTypeDesc : public TypeDesc {
    friend class TypeDesc;
    friend class JIT_TrialAlloc;
    friend class CheckAsmOffsets;


public:
#ifndef DACCESS_COMPILE
    ParamTypeDesc(CorElementType type, MethodTable* pMT, TypeHandle arg) 
        : TypeDesc(type), m_TemplateMT(pMT), m_Arg(arg), m_ExposedClassObject(NULL) {

        LEAF_CONTRACT;

        // ParamTypeDescs start out life not fully loaded
        m_typeAndFlags |= TypeDesc::enum_flag_IsNotFullyLoaded;

        INDEBUGIMPL(Verify());
    }
#endif 

    INDEBUGIMPL(BOOL Verify();)

    OBJECTREF GetManagedClassObject();
    OBJECTREF GetManagedClassObjectIfExists() {
        LEAF_CONTRACT;
        if (m_ExposedClassObject)
            return *m_ExposedClassObject;
        return NULL;
    }

    TypeHandle GetModifiedType()
    {
        LEAF_CONTRACT;

        return m_Arg;
    }


    BOOL OwnsTemplateMethodTable();

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
    
    friend class StubLinkerCPU;

protected:
    // the m_typeAndFlags field in TypeDesc tell what kind of parameterized type we have
    PTR_MethodTable m_TemplateMT;       // The shared method table, some variants do not use this field (it is null)
    TypeHandle      m_Arg;              // The type that is being modified
    OBJECTREF       *m_ExposedClassObject;  // pointer back to the internal reflection Type object
};

/*************************************************************************/
/* An ArrayTypeDesc represents a Array of some pointer type. */

class ArrayTypeDesc : public ParamTypeDesc
{
public:
#ifndef DACCESS_COMPILE
    ArrayTypeDesc(MethodTable* arrayMT, TypeHandle elementType) :
        ParamTypeDesc(arrayMT->IsMultiDimArray() ? ELEMENT_TYPE_ARRAY : ELEMENT_TYPE_SZARRAY, arrayMT, elementType) {
        WRAPPER_CONTRACT;
        INDEBUG(Verify());
        }

    // placement new operator
    void* operator new(size_t size, void* spot) {   return (spot); }

#endif

    TypeHandle GetArrayElementTypeHandle() {
        WRAPPER_CONTRACT;
        return GetTypeParam();
    }

    unsigned GetRank() {
        WRAPPER_CONTRACT;
        return(GetMethodTable()->GetRank());
    }

    MethodDesc* GetArrayElementCtor() {
        WRAPPER_CONTRACT;
        return(GetArrayClass()->GetArrayElementCtor());
    }

    MethodTable* GetParent()
    {
        WRAPPER_CONTRACT;
        _ASSERTE(m_TemplateMT != NULL);
        _ASSERTE(m_TemplateMT->IsArray());

        // Once we insert Array<T> into array hierarchy, this assert will no longer be valid.
        _ASSERTE(m_TemplateMT->ParentEquals(g_pArrayClass));
        return g_pArrayClass;
    }


    INDEBUG(BOOL Verify();)

private:

    friend class TypeHandle;

    ArrayClass* GetArrayClass() {
        WRAPPER_CONTRACT;
        ArrayClass* ret =
            PTR_ArrayClass(PTR_HOST_TO_TADDR(m_TemplateMT->GetClass()));
        _ASSERTE(ret->IsArrayClass());
        return ret;
    }

};

/*************************************************************************/
// These are for verification of generic code and reflection over generic code.
// Each TypeVarTypeDesc represents a class or method type variable, as specified by a GenericParam entry.
// The type variables are tied back to the class or method that *defines* them.
// This is done through typedef or methoddef tokens.

class TypeVarTypeDesc : public TypeDesc 
{
public:

#ifndef DACCESS_COMPILE

    TypeVarTypeDesc(Module *pModule, mdToken typeOrMethodDef, unsigned int index, mdGenericParam token) :
        TypeDesc(TypeFromToken(typeOrMethodDef) == mdtTypeDef ? ELEMENT_TYPE_VAR : ELEMENT_TYPE_MVAR)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(CheckPointer(pModule));
            PRECONDITION(TypeFromToken(typeOrMethodDef) == mdtTypeDef || TypeFromToken(typeOrMethodDef) == mdtMethodDef);
            PRECONDITION(index >= 0);
            PRECONDITION(TypeFromToken(token) == mdtGenericParam);
        }
        CONTRACTL_END;

        m_pModule = pModule;
        m_typeOrMethodDef = typeOrMethodDef;
        m_token = token;
        m_index = index;
        m_ExposedClassObject = NULL;
        m_constraints = NULL;
        m_numConstraints = (DWORD)-1;
    }
#endif // #ifndef DACCESS_COMPILE

    // placement new operator
    void* operator new(size_t size, void* spot) { LEAF_CONTRACT;  return (spot); }

    Module* GetModule()
    {
        LEAF_CONTRACT;
        return m_pModule;
    }

    unsigned int GetIndex() 
    { 
        LEAF_CONTRACT;
        return m_index; 
    }

    mdGenericParam GetToken() 
    { 
        LEAF_CONTRACT;
        return m_token; 
    }

    mdToken GetTypeOrMethodDef() 
    { 
        LEAF_CONTRACT;
        return m_typeOrMethodDef; 
    }

    OBJECTREF GetManagedClassObject();
    OBJECTREF GetManagedClassObjectIfExists() 
    {
        LEAF_CONTRACT;
        if (m_ExposedClassObject)
            return *m_ExposedClassObject;
        return NULL;
    }

    // Return NULL if no constraints are specified 
    // Return an array of type handles if constraints are specified,
    // with the number of constraints returned in pNumConstraints
    // The base type constraint, if any, will be the first TypeHandle in the array
    BOOL ConstraintsLoaded() { LEAF_CONTRACT; return m_numConstraints != (DWORD)-1; }
    TypeHandle* GetCachedConstraints(DWORD *pNumConstraints);
    TypeHandle* GetConstraints(DWORD *pNumConstraints, ClassLoadLevel level = CLASS_LOADED);

    // Load the constraints if not already loaded
    void LoadConstraints(ClassLoadLevel level = CLASS_LOADED);

    // Check the constraints on this type parameter hold in the supplied context for the supplied type
    BOOL SatisfiesConstraints(SigTypeContext *pTypeContext, TypeHandle thArg);
    BOOL ConstrainedAsObjRef();

    
#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
    
protected:
    // Module containing the generic definition, also the loader module for this type desc
    PTR_Module m_pModule;

    // Declaring type or method
    mdToken m_typeOrMethodDef;

    // Constraints, determined on first call to GetConstraints
    DWORD m_numConstraints;    // -1 until number has been determined
    PTR_TypeHandle m_constraints;

    // pointer back to the internal reflection Type object
    OBJECTREF    *m_ExposedClassObject;    

    // token for GenericParam entry
    mdGenericParam    m_token; 

    // index within declaring type or method, numbered from zero
    unsigned int m_index;
};

/*************************************************************************/
/* represents a function type.  */

typedef SPTR(class FnPtrTypeDesc) PTR_FnPtrTypeDesc;

class FnPtrTypeDesc : public TypeDesc {
public:
#ifndef DACCESS_COMPILE
    FnPtrTypeDesc(BYTE callConv, DWORD numArgs, TypeHandle* retAndArgTypes) 
        : TypeDesc(ELEMENT_TYPE_FNPTR), m_NumArgs(numArgs), m_CallConv(callConv) {
        LEAF_CONTRACT;
        for (DWORD i = 0; i <= numArgs; i++) 
            m_RetAndArgTypes[i] = retAndArgTypes[i];
    }
#endif 

    DWORD GetNumArgs() 
    { 
        LEAF_CONTRACT;
        return m_NumArgs;
    }

    BYTE GetCallConv() 
    { 
        LEAF_CONTRACT;
        return m_CallConv;
    }

    // Return a pointer to the types of the signature, return type followed by argument types
    // The type handles are guaranteed to be fixed up
    TypeHandle* GetRetAndArgTypes();

    // As above, but the type handles might be zap-encodings that need fixing up explicitly
    TypeHandle* GetRetAndArgTypesPointer()
    {
        LEAF_CONTRACT;

        return m_RetAndArgTypes;
    }


#ifdef DACCESS_COMPILE
    static ULONG32 DacSize(TADDR addr)
    {
        DWORD numArgs = *PTR_DWORD(addr + offsetof(FnPtrTypeDesc, m_NumArgs));
        return offsetof(FnPtrTypeDesc, m_RetAndArgTypes) +
            numArgs * sizeof(TypeHandle);
    }

    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
    
protected:
    // Number of arguments
    DWORD m_NumArgs;

    // Calling convention (actually just a single byte)
    DWORD m_CallConv;

    // Return type first, then argument types
    TypeHandle m_RetAndArgTypes[1];
};


#endif // TYPEDESC_H
