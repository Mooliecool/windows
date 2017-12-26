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
#ifndef _BINDERMODULE_H_
#define _BINDERMODULE_H_

class DataImage;
class Module;
class MethodTable;
class MethodDesc;
class FieldDesc;

//
// Use the Binder objects to avoid doing unnecessary name lookup 
// (esp. in the prejit case) 
//
// E.g. g_Mscorlib.GetClass(CLASS__APP_DOMAIN);
// 

// BinderClassIDs are of the form CLASS__XXX

enum BinderClassID
{
    CLASS__NIL = 0,

    CLASS__MSCORLIB_NIL = CLASS__NIL,

#define DEFINE_CLASS(i,n,s)         CLASS__ ## i,
#include "mscorlib.h"

    CLASS__MSCORLIB_COUNT,
};


// BinderMethdoIDs are of the form METHOD__XXX__YYY, 
// where X is the class and Y is the method

enum BinderMethodID
{
    METHOD__NIL = 0,

    METHOD__MSCORLIB_NIL = METHOD__NIL,

#define DEFINE_METHOD(c,i,s,g)      METHOD__ ## c ## __ ## i,
#include "mscorlib.h"

    METHOD__MSCORLIB_COUNT,
};

// BinderFieldIDs are of the form FIELD__XXX__YYY, 
// where X is the class and Y is the field

enum BinderFieldID
{
    FIELD__NIL = 0,

    // Mscorlib:
    FIELD__MSCORLIB_NIL = FIELD__NIL,
    
#define DEFINE_FIELD(c,i,s)                 FIELD__ ## c ## __ ## i,
#ifdef _DEBUG
#define DEFINE_FIELD_U(c,i,s,uc,uf)         FIELD__ ## c ## __ ## i,
#endif
#include "mscorlib.h"

    FIELD__MSCORLIB_COUNT,
};

enum BinderTypeID
{
    TYPE__NIL = 0,

    // Mscorlib:
    TYPE__MSCORLIB_NIL = TYPE__NIL,

    TYPE__BYTE_ARRAY,
    TYPE__OBJECT_ARRAY,
    TYPE__VARIANT_ARRAY,
    TYPE__VOID_PTR,

    TYPE__MSCORLIB_COUNT,
};

class Binder
{
  public:

    //
    // Retrieve tokens from ID
    // 

    mdTypeDef GetTypeDef(BinderClassID id);
    mdMethodDef GetMethodDef(BinderMethodID id);
    mdFieldDef GetFieldDef(BinderFieldID id);

    //
    // Normal calls retrieve structures from ID
    // and make sure proper class initialization
    // has occurred.
    //

    MethodTable *GetClass(BinderClassID id);
    MethodDesc *GetMethod(BinderMethodID id);
    FieldDesc *GetField(BinderFieldID id);
    TypeHandle GetType(BinderTypeID id);

    //
    // Retrieve structures from ID, but 
    // don't run the .cctor
    //

    MethodTable *FetchClass(BinderClassID id, BOOL fLoad = TRUE, ClassLoadLevel level = CLASS_LOADED);
    MethodDesc *FetchMethod(BinderMethodID id);
    FieldDesc *FetchField(BinderFieldID id);
    TypeHandle FetchType(BinderTypeID id);

    //
    // Retrieve structures from ID, but 
    // only if they have been loaded already.
    // This methods ensure that no gc will happen
    //
    MethodTable *GetExistingClass(BinderClassID id)
    {
        WRAPPER_CONTRACT;

        return RawGetClass(id);
    }

    MethodDesc *GetExistingMethod(BinderMethodID id)
    {
        WRAPPER_CONTRACT;

        return RawGetMethod(id);
    }

    FieldDesc *GetExistingField(BinderFieldID id)
    {
        WRAPPER_CONTRACT;

        return RawGetField(id);
    }

    TypeHandle GetExistingType(BinderTypeID id)
    {
        WRAPPER_CONTRACT;

        return RawGetType(id);
    }

    //
    // Info about stuff
    //
    
    LPCUTF8 GetClassName(BinderClassID id)
    {
        LEAF_CONTRACT;

        _ASSERTE(id != CLASS__NIL);
        _ASSERTE(id <= m_cClassRIDs);
        return m_classDescriptions[id-1].name;
    }

    LPCUTF8 GetClassNameSpace(BinderClassID id)
    {
        LEAF_CONTRACT;

        _ASSERTE(id != CLASS__NIL);
        _ASSERTE(id <= m_cClassRIDs);
        return m_classDescriptions[id-1].namesp;
    }

    BinderClassID GetMethodClass(BinderMethodID id)
    { 
        LEAF_CONTRACT;

        _ASSERTE(id != METHOD__NIL);
        _ASSERTE(id <= m_cMethodMDs);
        return m_methodDescriptions[id-1].classID;
    }

    LPCUTF8 GetMethodName(BinderMethodID id)
    { 
        LEAF_CONTRACT;

        _ASSERTE(id != METHOD__NIL);
        _ASSERTE(id <= m_cMethodMDs);
        return m_methodDescriptions[id-1].name;
    }

    LPHARDCODEDMETASIG GetMethodSig(BinderMethodID id)
    { 
        LEAF_CONTRACT;

        _ASSERTE(id != METHOD__NIL);
        _ASSERTE(id <= m_cMethodMDs);
        return m_methodDescriptions[id-1].sig;
    }

    void GetMethodBinarySigAndSize(BinderMethodID id,
                                   PCCOR_SIGNATURE *ppSig,
                                   DWORD *pcbSigSize)
    { 
        WRAPPER_CONTRACT;

        _ASSERTE(id != METHOD__NIL);
        _ASSERTE(id <= m_cMethodMDs);
        _ASSERTE(ppSig != NULL);
        _ASSERTE(pcbSigSize != NULL);

        m_methodDescriptions[id-1].sig->GetBinarySig(ppSig, pcbSigSize);
    }

    BinderClassID GetFieldClass(BinderFieldID id)
    { 
        LEAF_CONTRACT;

        _ASSERTE(id != FIELD__NIL);
        _ASSERTE(id <= m_cFieldRIDs);
        return m_fieldDescriptions[id-1].classID;
    }

    LPCUTF8 GetFieldName(BinderFieldID id)
    { 
        LEAF_CONTRACT;

        _ASSERTE(id != FIELD__NIL);
        _ASSERTE(id <= m_cFieldRIDs);
        return m_fieldDescriptions[id-1].name;
    }

    //
    // Identity test - doesn't do unnecessary
    // class loading or initialization.
    //

    BOOL IsClass(MethodTable *pMT, BinderClassID id);
    BOOL IsType(TypeHandle th, BinderTypeID id);

    //
    // Method address - these could conceivably be implemented
    // more efficiently than accessing the Desc info.
    // 

    const BYTE *GetMethodAddress(BinderMethodID id);

    //
    // Offsets - these could conceivably be implemented
    // more efficiently than accessing the Desc info.
    // 

    DWORD GetFieldOffset(BinderFieldID id);

    //
    // Utilities for exceptions
    //

    BOOL IsException(MethodTable *pMT, RuntimeExceptionKind kind);
    MethodTable *GetException(RuntimeExceptionKind kind);
    MethodTable *FetchException(RuntimeExceptionKind kind);
    LPCUTF8 GetExceptionName(RuntimeExceptionKind kind);

    //
    // Utilities for signature element types
    //

    BOOL IsElementType(MethodTable *pMT, CorElementType type);
    MethodTable *GetElementType(CorElementType type);
    MethodTable *FetchElementType(CorElementType type, BOOL fLoad = TRUE, ClassLoadLevel level = CLASS_LOADED);
    MethodTable *LookupElementType(CorElementType type, ClassLoadLevel level = CLASS_LOADED);
    LPCUTF8 GetElementTypeName(CorElementType type);

    //
    // Store the binding arrays to a prejit image
    // so we don't have to do name lookup at runtime
    //

    //
    // These are called by initialization code:
    //

    static void StartupMscorlib(Module *pModule);

#ifdef _DEBUG
    static void CheckMscorlib();
    static void CheckMscorlibExtended();
#endif

    Module* GetModule() { LEAF_CONTRACT; return m_pModule; }

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif


private:

    struct ClassDescription
    {
        PTR_CSTR namesp;
        PTR_CSTR name;
    };

    struct MethodDescription
    {
        BinderClassID classID;
        PTR_CUTF8 name;
        PTR_HARDCODEDMETASIG sig;
    };

    struct FieldDescription
    {
        BinderClassID classID;
        PTR_CUTF8 name;
    };

    struct TypeDescription
    {
        BinderClassID   classID;
        CorElementType  type;
        int             rank;
        PTR_CSTR        name;
    };

    // NOTE: No constructors/destructors - we have global instances!

    void Init(Module *pModule, 
              const ClassDescription *pClassDescriptions,
              DWORD cClassDescriptions,
              const MethodDescription *pMethodDescriptions,
              DWORD cMethodDescriptions,
              const FieldDescription *pFieldDescriptions,
              DWORD cFieldDescriptions,
              const TypeDescription *pTypeDescriptions,
              DWORD cTypeDescriptions);
    void Destroy();

    static void CheckInit(MethodTable *pMT);
    static void InitClass(MethodTable *pMT);
    
    MethodTable *RawGetClass(BinderClassID id, ClassLoadLevel level = CLASS_LOADED);
    MethodDesc *RawGetMethod(BinderMethodID id);
public: // use by EnCSyncBlockInfo::ResolveField
    FieldDesc *RawGetField(BinderFieldID id);
private:    
    TypeHandle RawGetType(BinderTypeID id);

    MethodTable *LookupClass(BinderClassID id, BOOL fLoad = TRUE, ClassLoadLevel level = CLASS_LOADED);
    MethodDesc *LookupMethod(BinderMethodID id);
    FieldDesc *LookupField(BinderFieldID id);
    TypeHandle LookupType(BinderTypeID id, BOOL fLoad = TRUE);

    DPTR(const ClassDescription) m_classDescriptions;
    DPTR(const MethodDescription) m_methodDescriptions;
    DPTR(const FieldDescription) m_fieldDescriptions;
    DPTR(const TypeDescription) m_typeDescriptions;

    PTR_Module m_pModule;

    USHORT m_cClassRIDs;
    PTR_USHORT m_pClassRIDs;

    USHORT m_cFieldRIDs;
    PTR_USHORT m_pFieldRIDs;

    USHORT m_cMethodMDs;
    DPTR(PTR_MethodDesc) m_pMethodMDs;

    USHORT m_cTypeHandles;
    PTR_TypeHandle m_pTypeHandles;

    static const ClassDescription MscorlibClassDescriptions[];
    static const MethodDescription MscorlibMethodDescriptions[];
    static const FieldDescription MscorlibFieldDescriptions[];
    static const TypeDescription MscorlibTypeDescriptions[];

#ifdef _DEBUG
  
    struct FieldOffsetCheck
    {
        BinderFieldID fieldID;
        USHORT expectedOffset;
        USHORT expectedSize;
    };

    struct ClassSizeCheck
    {
        BinderClassID classID;
        USHORT expectedSize;
    };

    static const FieldOffsetCheck MscorlibFieldOffsets[];
    static const ClassSizeCheck MscorlibClassSizes[];

#endif

};

//
// Global bound modules:
// 

GVAL_DECL(Binder, g_Mscorlib);

#endif // _BINDERMODULE_H_
