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

#ifndef _RUNTIMEHANDLES_H_
#define _RUNTIMEHANDLES_H_

#include "object.h"
#include "typehandle.h"
#include "fcall.h"
#include "field.h"
#include "typectxt.h"
#include "constrainedexecutionregion.h"

typedef void* EnregisteredTypeHandle;
struct SignatureNative;

// NOTE: These are defined in CallingConventions.cs.
typedef enum ReflectionCallConv {
    CALLCONV_Standard       = 0x0001,
    CALLCONV_VarArgs        = 0x0002,
    CALLCONV_Any            = CALLCONV_Standard | CALLCONV_VarArgs,
    CALLCONV_HasThis        = 0x0020,
    CALLCONV_ExplicitThis   = 0x0040,
    CALLCONV_ValueRetBufArg = 0x0100, // PRIVATE MEMBER -- Not exposed in CallingConventions.cs
    CALLCONV_HasMethodInst  = 0x0200, // PRIVATE MEMBER -- Not exposed in CallingConventions.cs
} ReflectionCallConv;


// Types used to expose method bodies via reflection.

class ExceptionHandlingClause;
class MethodBody;
class LocalVariableInfo;

#ifdef USE_CHECKED_OBJECTREFS
typedef REF<ExceptionHandlingClause> EXCEPTIONHANDLINGCLAUSEREF;
typedef REF<MethodBody> METHODBODYREF;
typedef REF<LocalVariableInfo> LOCALVARIABLEINFOREF;
#else
typedef DPTR(ExceptionHandlingClause) EXCEPTIONHANDLINGCLAUSEREF;
typedef DPTR(MethodBody) METHODBODYREF;
typedef DPTR(LocalVariableInfo) LOCALVARIABLEINFOREF;
#endif

class ExceptionHandlingClause : Object 
{
private:
    // Disallow creation and copy construction of these.
    ExceptionHandlingClause() { }
    ExceptionHandlingClause(ExceptionHandlingClause &r) { }    

public:
    METHODBODYREF m_methodBody;
    CorExceptionFlag m_flags;
    INT32 m_tryOffset;
    INT32 m_tryLength;
    INT32 m_handlerOffset;
    INT32 m_handlerLength;
    mdTypeDef m_catchToken;
    INT32 m_filterOffset;
};

class MethodBody : Object 
{     
private:
    // Disallow creation and copy construction of these.
    MethodBody() { }
    MethodBody(MethodBody &r) { }    

public:
    U1ARRAYREF m_IL;
    PTRARRAYREF m_exceptionClauses;
    PTRARRAYREF m_localVariables;
    OBJECTREF m_methodBase;

    INT32 m_localVarSigToken;
    INT32 m_maxStackSize;
    CLR_BOOL m_initLocals;    
};

class LocalVariableInfo : Object
{
private:
    // Disallow creation and copy construction of these.
    LocalVariableInfo() { }
    LocalVariableInfo(LocalVariableInfo &r) { }    

public:
    INT32 m_bIsPinned;
    INT32 m_localIndex;
    TypeHandle m_typeHandle;
};

class Utf8String {
public:
    static FCDECL3(FC_BOOL_RET, EqualsCaseInsensitive, LPCUTF8 szLhs, LPCUTF8 szRhs, INT32 stringNumBytes);
    static FCDECL3(FC_BOOL_RET, EqualsCaseSensitive, LPCUTF8 szLhs, LPCUTF8 szRhs, INT32 stringNumBytes);
};


class MethodDescChunkHandle {
public:
     static FCDECL1(MethodDescChunk*, GetNextMethodDescChunk, MethodDescChunk **ppChunk);
     static FCDECL2(MethodDesc*, GetMethodAt, MethodDescChunk **ppChunk, INT32 index);
     static FCDECL1(INT32, GetMethodCount, MethodDescChunk **ppChunk);
};


class FastArray {
public:
    static FCDECL3(void, SetValueAt, PtrArray* pPtrArray, INT32 index, Object* object);
//    static FCDECL2(void, CopyArray, PtrArray* pSrc, PtrArray* pDst);
};

class RuntimeTypeHandle {

public:

    // Static method on RuntimeTypeHandle
    static FCDECL1(Object*, Allocate, TypeHandle *pTypeHandle) ; //A.CI work	
    static FCDECL6(Object*, CreateInstance, ReflectClassBaseObject* refThisUNSAFE, CLR_BOOL publicOnly, CLR_BOOL securityOff, CLR_BOOL *bCanBeCached, MethodDesc** constructor, CLR_BOOL *bNeedSecurityCheck);
    static FCDECL2(Object*, CreateCaInstance, TypeHandle* pCaType, MethodDesc* pCtor);
    static FCDECL1(EnregisteredTypeHandle, MakeByRef, TypeHandle *pTypeHandle);
    static FCDECL1(EnregisteredTypeHandle, MakePointer, TypeHandle *pTypeHandle);
    static FCDECL1(EnregisteredTypeHandle, MakeSZArray, TypeHandle *pTypeHandle);
    static FCDECL2(EnregisteredTypeHandle, MakeArray, TypeHandle *pTypeHandle, INT32 rank);
    static FCDECL1(EnregisteredTypeHandle, GetTrueTypeHandle, Object* o);
    static FCDECL1(Object*, GetRuntimeType, void *th);
    static FCDECL1(LPVOID, TypeHandle_get, Object* o);
    static FCDECL1(void, PrepareMemberInfoCache, MethodTable *pMemberInfoCache);

    static FCDECL2(LPVOID, GetMethodFromToken, TypeHandle *pTypeHandle, INT32 methodDef);
    static FCDECL4(StringObject*, ConstructName, TypeHandle *pTypeHandle, CLR_BOOL fNamespace, CLR_BOOL fFullInst, CLR_BOOL fAssembly);
    static FCDECL2(EnregisteredTypeHandle, GetTypeByNameUsingCARules, StringObject* classNameUNSAFE, Module* pModule);
    static FCDECL6(EnregisteredTypeHandle, GetTypeByName, 
        StringObject* classNameUNSAFE, CLR_BOOL bThrowOnError, CLR_BOOL bIgnoreCase, CLR_BOOL bReflectionOnly, 
        StackCrawlMark* stackMark, CLR_BOOL bLoadTypeFromPartialNameHack);

    static FCDECL1(DomainAssembly*, GetAssemblyHandle, TypeHandle *pTypeHandle);
    static FCDECL1(EnregisteredTypeHandle, GetBaseTypeHandle, TypeHandle *pTypeHandle);
    static FCDECL1(Module*, GetModuleHandle, TypeHandle *pTypeHandle);
    static FCDECL1(INT32, GetAttributes, TypeHandle *pTypeHandle);
    static FCDECL1(INT32, GetToken, TypeHandle *pTypeHandle);
    static FCDECL1(LPCUTF8, GetUtf8Name, TypeHandle *pTypeHandle);
    static FCDECL1(INT32, GetArrayRank, TypeHandle *pTypeHandle);

    static FCDECL1(MethodDesc*, GetDeclaringMethod, TypeHandle *pTypeHandle);
    static FCDECL1(MethodDesc*, GetDefaultConstructor, TypeHandle *pTypeHandle);
    static FCDECL1(EnregisteredTypeHandle, GetDeclaringType, TypeHandle *pTypeHandle);
    static FCDECL1(FC_BOOL_RET, IsContextful, TypeHandle *pTypeHandle);
    static FCDECL1(FC_BOOL_RET, IsVisible, TypeHandle *pTypeHandle);
    static FCDECL2(FC_BOOL_RET, IsVisibleFromModule, TypeHandle *pTypeHandle, Module* pModule);
    static FCDECL1(FC_BOOL_RET, HasProxyAttribute, TypeHandle *pTypeHandle);
    static FCDECL2(FC_BOOL_RET, IsComObject, TypeHandle *pTypeHandle, CLR_BOOL isGenericCOM);
    static FCDECL2(FC_BOOL_RET, CanCastTo, TypeHandle *pTypeHandle, EnregisteredTypeHandle thTarget);
    static FCDECL2(FC_BOOL_RET, IsInstanceOfType, TypeHandle *pTypeHandle, Object *object);
    static FCDECL2(FC_BOOL_RET, SupportsInterface, TypeHandle *pTypeHandle, Object* objUNSAFE);

    static FCDECL4(FC_BOOL_RET, SatisfiesConstraints, TypeHandle *pGenericParameter, ArrayBase* arrayTypeContext, ArrayBase* arrayMethodContext, EnregisteredTypeHandle ethGenericArgument);
    static FCDECL1(FC_BOOL_RET, HasInstantiation, TypeHandle *pTypeHandle);
    static FCDECL1(FC_BOOL_RET, IsGenericTypeDefinition, TypeHandle *pTypeHandle);
    static FCDECL1(FC_BOOL_RET, IsGenericVariable, TypeHandle *pTypeHandle);
    static FCDECL1(INT32, GetGenericVariableIndex, TypeHandle *pTypeHandle);
    static FCDECL1(FC_BOOL_RET, ContainsGenericVariables, TypeHandle *pTypeHandle);
    static FCDECL1(FC_BOOL_RET, IsGenericType, TypeHandle *pTypeHandle);
    static FCDECL1(ArrayBase*, GetInstantiation, TypeHandle *pTypeHandle);
    static FCDECL2(EnregisteredTypeHandle, Instantiate, TypeHandle *pTypeHandle, ArrayBase *instArray);
    static FCDECL1(EnregisteredTypeHandle, GetGenericTypeDefinition, TypeHandle *pTypeHandle);

    static FCDECL1(void*, GetCanonicalHandle, TypeHandle *pTypeHandle);

    static FCDECL1(ArrayBase*, GetInterfaces, TypeHandle *pTypeHandle);
    static FCDECL1(ArrayBase*, GetConstraints, TypeHandle *pTypeHandle);
    static FCDECL2(LPVOID, GetGCHandle, TypeHandle *pTypeHandle, int handleType);
    static FCDECL2(void, FreeGCHandle, TypeHandle *pTypeHandle, OBJECTHANDLE handle);
    static FCDECL1(INT32, GetCorElementType, TypeHandle *pTypeHandle);
    static FCDECL1(EnregisteredTypeHandle, GetElementType, TypeHandle *pTypeHandle);

    static FCDECL2(void*, GetMethodAt, TypeHandle *pTypeHandle, INT32 slot);
    static FCDECL1(INT32, GetNumVtableSlots, TypeHandle *pTypeHandle);
    static FCDECL1(INT32, GetInterfaceMethodSlots, TypeHandle *pTypeHandle);
    static FCDECL2(INT32, GetFirstSlotForInterface, TypeHandle *pTypeHandle, EnregisteredTypeHandle iface);
    static FCDECL3(INT32, GetInterfaceMethodImplementationSlot, TypeHandle *pTypeHandle, void *th, void *md);

    static FCDECL3(FC_BOOL_RET, GetFields, TypeHandle *pTypeHandle, INT32 **result, INT32 *pCount);
    static FCDECL1(void*, GetFieldHandleIterator, TypeHandle *pTypeHandle);
    static FCDECL1(void*, GetMethodDescChunk, TypeHandle *pTypeHandle);
    static FCDECL2(Object*, CreateInstanceForGenericType, TypeHandle* pThisTypeHandle, ReflectClassBaseObject* parameterType );	
};


class RuntimeMethodHandle {
    
public:  
    static FCDECL1(MethodDesc*, GetCurrentMethod, StackCrawlMark* stackMark);

    static FCDECL12(Object*, InvokeMethod, MethodDesc **ppMethod, Object *target, INT32 attrs, Object *binder, PTRArray *objs, Object *locale,
                                           CLR_BOOL isBinderDefault, DomainAssembly *caller, void *reflectedTypeHandle, void *declaringTypeHandle,
                                           SignatureNative* pSig, CLR_BOOL verifyAccess);
    static FCDECL6(Object*, InvokeMethodFast, MethodDesc **ppMethod, Object *target, PTRArray *objs, SignatureNative* pSig, DWORD attr, EnregisteredTypeHandle enregOwnerType);
    static FCDECL4(Object*, InvokeConstructor, MethodDesc **ppMethod, PTRArray *objsUNSAFE, SignatureNative* pSig, 
                                                EnregisteredTypeHandle enregDeclaringType);
	
    struct StreamingContextData {
        Object * additionalContext;  // additionalContex was changed from OBJECTREF to Object to avoid having a
        INT32 contextStates;         // constructor in this struct. GCC doesn't allow structs with constructors to be
    };
    
    // *******************************************************************************************
    // Keep these in sync with the version in bcl\system\runtime\serialization\streamingcontext.cs
    // *******************************************************************************************
    enum StreamingContextStates
    {
        CONTEXTSTATE_CrossProcess   = 0x01,
        CONTEXTSTATE_CrossMachine   = 0x02,
        CONTEXTSTATE_File           = 0x04,
        CONTEXTSTATE_Persistence    = 0x08,
        CONTEXTSTATE_Remoting       = 0x10,
        CONTEXTSTATE_Other          = 0x20,
        CONTEXTSTATE_Clone          = 0x40,
        CONTEXTSTATE_CrossAppDomain = 0x80,
        CONTEXTSTATE_All            = 0xFF
    }; 

    // passed by value
    // STATIC IMPLEMENTATION
    static OBJECTREF InvokeMethod_Internal(
        MethodDesc *pMethod, OBJECTREF targetUNSAFE, INT32 attrs, OBJECTREF binderUNSAFE, PTRARRAYREF objsUNSAFE, OBJECTREF localeUNSAFE,
        BOOL isBinderDefault, Assembly *caller, Assembly *reflectedClassAssembly, TypeHandle declaringType, SignatureNative* pSig, BOOL verifyAccess);


    static FCDECL1(FC_BOOL_RET, IsILStub, MethodDesc **ppMethod);
    static FCDECL2(FC_BOOL_RET, IsVisibleFromModule, MethodDesc **ppMethod, Module* pModule);
    static FCDECL2(FC_BOOL_RET, IsVisibleFromType, MethodDesc **ppMethod, EnregisteredTypeHandle enregTypeHandle);
    static FCDECL3(void, CheckLinktimeDemands, MethodDesc **ppMethod, Module* pModule, INT32 tkToken);
    static FCDECL5(void, SerializationInvoke, MethodDesc **ppMethod, Object* targetUNSAFE, SignatureNative* pSig, 
        Object* serializationInfoUNSAFE, struct StreamingContextData context);
    static FCDECL1(StringObject*, ConstructInstantiation, MethodDesc** ppMethod);
    static FCDECL1(void*, GetFunctionPointer, MethodDesc **ppMethod);
    static FCDECL1(INT32, GetAttributes, MethodDesc **ppMethod);
    static FCDECL1(INT32, GetImplAttributes, MethodDesc **ppMethod);
    static FCDECL1(EnregisteredTypeHandle, GetDeclaringType, MethodDesc **ppMethod);
    static FCDECL1(INT32, GetSlot, MethodDesc **ppMethod);
    static FCDECL1(INT32, GetMethodDef, MethodDesc **ppMethod);
    static FCDECL1(StringObject*, GetName, MethodDesc **ppMethod);
    static FCDECL1(LPCUTF8, GetUtf8Name, MethodDesc **ppMethod);
    static FCDECL1(ArrayBase*, GetMethodInstantiation, MethodDesc **ppMethod);
    static FCDECL1(FC_BOOL_RET, HasMethodInstantiation, MethodDesc **ppMethod);
    static FCDECL1(FC_BOOL_RET, IsGenericMethodDefinition, MethodDesc **ppMethod);
    static FCDECL1(MethodDesc*, GetTypicalMethodDefinition, MethodDesc **ppMethod);
    static FCDECL1(MethodDesc*, StripMethodInstantiation, MethodDesc **ppMethod);
    // see comment in the cpp file
    static FCDECL3(MethodDesc*, GetInstantiatingStub, MethodDesc **ppMethod, EnregisteredTypeHandle enregTypeHandle, ArrayBase* instArray);
    static FCDECL2(MethodDesc*, GetMethodFromCanonical, MethodDesc **ppMethod, EnregisteredTypeHandle enregTypeHandle);
    static FCDECL1(FC_BOOL_RET, IsDynamicMethod, MethodDesc **ppMethod);
    static FCDECL1(Object*, GetResolver, MethodDesc **ppMethod);
    static FCDECL1(void, Destroy, MethodDesc **ppMethod);
    static FCDECL2(MethodBody*, GetMethodBody, MethodDesc **ppMethod, EnregisteredTypeHandle enregDeclaringTypeHandle);

    static FCDECL1(FC_BOOL_RET, IsConstructor, MethodDesc **ppMethod);
};

class RuntimeFieldHandle {
    
public:
    static FCDECL5(Object*, GetValue, FieldDesc **ppFieldDesc, Object *instanceUNSAFE, EnregisteredTypeHandle fieldTH, EnregisteredTypeHandle declaringTH, CLR_BOOL *pDomainInitialized);
    static FCDECL7(void, SetValue, FieldDesc **ppFieldDesc, Object *targetUNSAFE, Object *valueUNSAFE, EnregisteredTypeHandle fieldTH, DWORD attr, EnregisteredTypeHandle declaringTH, CLR_BOOL *pDomainInitialized);
    static FCDECL4(Object*, GetValueDirect, FieldDesc **ppFieldDesc, EnregisteredTypeHandle fieldTH, TypedByRef target, EnregisteredTypeHandle enregDeclaringType);
    static FCDECL5(void, SetValueDirect, FieldDesc **ppFieldDesc, EnregisteredTypeHandle fieldTH, TypedByRef target, Object *valueUNSAFE, EnregisteredTypeHandle contextTH);
    static FCDECL1(StringObject*, GetName, FieldDesc **ppField);
    static FCDECL1(LPCUTF8, GetUtf8Name, FieldDesc **ppField);
    static FCDECL1(INT32, GetAttributes, FieldDesc **ppField);
    static FCDECL1(void*, GetApproxDeclaringType, FieldDesc **ppField);
    static FCDECL2(void*, GetFieldType, FieldDesc **ppField, void *pHandle);
    static FCDECL1(INT32, GetToken, FieldDesc **ppField);
    static FCDECL2(FieldDesc*, GetStaticFieldForGenericType, FieldDesc **ppField, EnregisteredTypeHandle declaringTH);
    static FCDECL1(FC_BOOL_RET, AcquiresContextFromThis, FieldDesc **ppField);
};

class ModuleHandle {
    
public:
    static FCDECL4(void*, GetDynamicMethod, Module *pModule, StringObject *name, U1Array *sig, Object *resolver);
    static FCDECL1(INT32, GetToken, Module **ppModule);
    static FCDECL1(EnregisteredTypeHandle, GetModuleTypeHandle, Module **ppModule);
    static FCDECL1(IMDInternalImport*, GetMetadataImport, Module **ppModule);
    static FCDECL6(EnregisteredTypeHandle, ResolveType, Module **ppModule, INT32 tkType, TypeHandle *typeArgs, int typeArgsCount, TypeHandle *methodArgs, int methodArgsCount);
    static FCDECL6(MethodDesc*, ResolveMethod, Module **ppModule, INT32 tkMemberRef, TypeHandle *typeArgs, int typeArgsCount, TypeHandle *methodArgs, int methodArgsCount);
    static FCDECL6(FieldDesc*, ResolveField, Module **ppModule, INT32 tkMemberRef, TypeHandle *typeArgs, int typeArgsCount, TypeHandle *methodArgs, int methodArgsCount);
    static FCDECL1(Object*, GetModule, Module **ppModule);   
    static FCDECL1(EnregisteredTypeHandle, GetCallerType, StackCrawlMark* stackMark);   
    static FCDECL3(void, GetPEKind, Module **ppModule, DWORD* pdwPEKind, DWORD* pdwMachine);   
    static FCDECL1(INT32, GetMDStreamVersion, Module **ppModule);
};

class AssemblyHandle {

public:
    static FCDECL1(void*, GetManifestModule, DomainAssembly **ppAssembly);
    static FCDECL1(Object*, GetAssembly, DomainAssembly **ppAssembly);   
    static FCDECL1(INT32, GetToken, DomainAssembly **ppAssembly);   
    static FCDECL2(FC_BOOL_RET, AptcaCheck, DomainAssembly **ppTargetAssembly, DomainAssembly *pSourceAssembly);   
};

struct SignatureNative
{
    friend class RuntimeMethodHandle;
    
public:
    static FCDECL6(void, GetSignature,        
        SignatureNative* pSignatureNative, 
        PCCOR_SIGNATURE pCorSig, DWORD cCorSig, 
        FieldDesc *pFieldDesc, MethodDesc *pMethod, 
        EnregisteredTypeHandle thDeclaringType);   
    static FCDECL4(void, GetCustomModifiers, SignatureNative* pSig, INT32 parameter, ArrayBase** SAFERequired, ArrayBase** SAFEOptional);
    static FCDECL2(FC_BOOL_RET, CompareSig, SignatureNative* pLhs, SignatureNative* pRhs);

    BOOL IsVarArg() { LEAF_CONTRACT; return (m_managedCallingConvention & CALLCONV_VarArgs); }
    INT32 NumFixedArgs() { WRAPPER_CONTRACT; return m_arguments->GetNumComponents(); }
    TypeHandle GetReturnTypeHandle() { LEAF_CONTRACT; return m_returnType; }

    PCCOR_SIGNATURE GetCorSig() { LEAF_CONTRACT; return m_sig; }
    DWORD GetCorSigSize() { LEAF_CONTRACT; return m_cSig; }
    Module* GetModule() { WRAPPER_CONTRACT; return m_declaringType.GetModule(); }
    void* GetCallTarget() { LEAF_CONTRACT; return m_pCallTarget; }
    void CacheCallTarget(void* pCallTarget) { LEAF_CONTRACT; m_pCallTarget = pCallTarget; }
    
    void Reset(void** ppEnum) { LEAF_CONTRACT; *((INT32*)ppEnum) = 0; }
    TypeHandle NextArgExpanded(void** ppEnum) 
    { 
        INT32 i = *((INT32*)ppEnum); 
        *((INT32*)ppEnum) = i + 1;
        return GetArgumentAt(i);
    }

    BOOL IsRetBuffArg() { LEAF_CONTRACT; return (m_managedCallingConvention & CALLCONV_ValueRetBufArg); } 
    TypeHandle GetDeclaringType() { return m_declaringType; }
    MethodDesc* GetMethod() { return m_pMethod; }
    INT32 NumVirtualFixedArgs(BOOL isStatic)
    {
        LEAF_CONTRACT; 
        return m_numVirtualFixedArgs;
    }

    const SigTypeContext * GetTypeContext(SigTypeContext *pTypeContext) 
    { 
        WRAPPER_CONTRACT; 

       _ASSERTE(m_pMethod || !m_declaringType.IsNull());
        if (m_pMethod) 
            return SigTypeContext::GetOptionalTypeContext(m_pMethod, m_declaringType, pTypeContext);
        else
            return SigTypeContext::GetOptionalTypeContext(m_declaringType, pTypeContext);
    }

private:
    TypeHandle GetArgumentAt(INT32 position) 
    { 
        WRAPPER_CONTRACT;        
        TypeHandle *pElem = (TypeHandle*)m_arguments->GetDataPtr();
        return pElem[position];
    }
    
    void SetReturnType(TypeHandle typeHandle)
    {
        LEAF_CONTRACT; 
        m_returnType = typeHandle;        
    }
    
    void SetArgument(INT32 argument, TypeHandle typeHandle)
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(typeHandle.AsPtr()));
        }
        CONTRACTL_END;
        
        TypeHandle *pElem = (TypeHandle*)m_arguments->GetDataPtr();
        pElem[argument] = typeHandle;
    }

    void SetHasRetBuffArg()
    {
        LEAF_CONTRACT; 
        m_managedCallingConvention |= CALLCONV_ValueRetBufArg;
    }
    
    void SetCallingConvention(INT32 mdCallingConvention)
    {
        LEAF_CONTRACT; 
        
        if ((mdCallingConvention & IMAGE_CEE_CS_CALLCONV_MASK) == IMAGE_CEE_CS_CALLCONV_VARARG)
            m_managedCallingConvention = CALLCONV_VarArgs;
        else
            m_managedCallingConvention = CALLCONV_Standard;
        
        if ((mdCallingConvention & IMAGE_CEE_CS_CALLCONV_HASTHIS) != 0)
            m_managedCallingConvention |= CALLCONV_HasThis;
        
        if ((mdCallingConvention & IMAGE_CEE_CS_CALLCONV_EXPLICITTHIS) != 0)
            m_managedCallingConvention |= CALLCONV_ExplicitThis;
    }

    // Mirrored in the managed world
    //
    // this is the layout the classloader chooses by default for the managed struct.
    //
    BASEARRAYREF m_arguments;
    PCCOR_SIGNATURE m_sig; 
    PVOID m_pCallTarget; 
    INT32 m_managedCallingConvention;
    DWORD m_cSig;
    UINT m_numVirtualFixedArgs;
    UINT m_64bitpad;
    MethodDesc* m_pMethod;
    TypeHandle m_declaringType;
    TypeHandle m_returnType;
};

#endif

