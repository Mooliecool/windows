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
////////////////////////////////////////////////////////////////////////////////
// This module defines a Utility Class used by reflection
//
// Date: March/April 1998
////////////////////////////////////////////////////////////////////////////////

#ifndef __INVOKEUTIL_H__
#define __INVOKEUTIL_H__

// The following class represents the value class
#include <pshpack1.h>

struct InterfaceMapData
{
    REFLECTCLASSBASEREF     m_targetType;
    REFLECTCLASSBASEREF     m_interfaceType;
    PTRARRAYREF             m_targetMethods;
    PTRARRAYREF             m_interfaceMethods;
};

// Calling Conventions
// NOTE: These are defined in CallingConventions.cs They must match up.
#define Standard_CC     0x0001
#define VarArgs_CC      0x0002
#define Any_CC          (Standard_CC | VarArgs_CC)

#define PRIMITIVE_TABLE_SIZE  ELEMENT_TYPE_STRING
#define PT_Primitive    0x01000000

// Define the copy back constants.
#define COPYBACK_PRIMITIVE      1
#define COPYBACK_OBJECTREF      2
#define COPYBACK_VALUECLASS     3

#include <poppack.h>

class ReflectMethodList;

#define REFSEC_CHECK_MEMBERACCESS   0x00000001
#define REFSEC_THROW_MEMBERACCESS   0x00000002
#define REFSEC_THROW_FIELDACCESS    0x00000004
#define REFSEC_THROW_SECURITY       0x00000008


// Structure used to track security access checks efficiently when applied
// across a range of methods, fields etc.
//
class RefSecContext
{
    static StackWalkAction TraceCallerCallback(CrawlFrame* pCf, VOID* data);

    void Init()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;
        
        ZeroMemory(this, sizeof(*this));
        if (s_pTypeMulticastDelegate == NULL)
        {
            s_pMethPrivateProcessMessage = (GVAL_ADDR(g_Mscorlib))->FetchMethod(METHOD__STACK_BUILDER_SINK__PRIVATE_PROCESS_MESSAGE);
            s_pTypeRuntimeMethodInfo = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__METHOD);
            s_pTypeMethodBase = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__METHOD_BASE);
            s_pTypeRuntimeConstructorInfo = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__CONSTRUCTOR);
            s_pTypeConstructorInfo = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__CONSTRUCTOR_INFO);
            s_pTypeRuntimeType = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__CLASS);
            s_pTypeRuntimeTypeHandle = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__TYPE_HANDLE);
            s_pTypeType = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__TYPE);
            s_pTypeRuntimeFieldInfo = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__FIELD);
            s_pTypeRtFieldInfo = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__RT_FIELD_INFO);
            s_pTypeFieldInfo = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__FIELD_INFO);
            s_pTypeRuntimeEventInfo = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__EVENT);
            s_pTypeEventInfo = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__EVENT_INFO);
            s_pTypeRuntimePropertyInfo = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__PROPERTY);
            s_pTypePropertyInfo = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__PROPERTY_INFO);
            s_pTypeActivator = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__ACTIVATOR);
            s_pTypeAppDomain = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__APP_DOMAIN);
            s_pTypeAssembly = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__ASSEMBLY);
            s_pTypeTypeDelegator = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__TYPE_DELEGATOR);
            s_pTypeRuntimeMethodHandle = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__METHOD_HANDLE);
            s_pTypeRuntimeFieldHandle = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__FIELD_HANDLE);
            s_pTypeDelegate = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__DELEGATE);
            s_pTypeMulticastDelegate = (GVAL_ADDR(g_Mscorlib))->FetchClass(CLASS__MULTICAST_DELEGATE);
        }        
    }

public:
    RefSecContext()
    {
        WRAPPER_CONTRACT;
        Init();
    }
    
    MethodTable* GetCallerMT();
    MethodDesc* GetCallerMethod();
    
    AppDomain* GetCallerDomain();
    bool DemandMemberAccess(DWORD dwFlags);

private:    
    bool            m_fCheckedCaller;
    bool            m_fCheckedPerm;
    bool            m_fCallerHasPerm;
    bool            m_fSkippingRemoting;
    
    MethodDesc*     m_pCaller;
    AppDomain*      m_pCallerDomain;

    static MethodDesc*  s_pMethPrivateProcessMessage;
    static MethodTable* s_pTypeRuntimeMethodInfo;
    static MethodTable* s_pTypeMethodBase;
    static MethodTable* s_pTypeRuntimeConstructorInfo;
    static MethodTable* s_pTypeConstructorInfo;
    static MethodTable* s_pTypeRuntimeType;
    static MethodTable* s_pTypeRuntimeTypeHandle;
    static MethodTable* s_pTypeType;
    static MethodTable* s_pTypeRuntimeFieldInfo;
    static MethodTable* s_pTypeRtFieldInfo;
    static MethodTable* s_pTypeFieldInfo;
    static MethodTable* s_pTypeRuntimeEventInfo;
    static MethodTable* s_pTypeEventInfo;
    static MethodTable* s_pTypeRuntimePropertyInfo;
    static MethodTable* s_pTypePropertyInfo;
    static MethodTable* s_pTypeActivator;
    static MethodTable* s_pTypeAppDomain;
    static MethodTable* s_pTypeAssembly;
    static MethodTable* s_pTypeTypeDelegator;
    static MethodTable* s_pTypeDelegate;
    static MethodTable* s_pTypeMulticastDelegate;
    static MethodTable* s_pTypeRuntimeMethodHandle;
    static MethodTable* s_pTypeRuntimeFieldHandle;

};

// This class abstracts the functionality which creats the
//  parameters on the call stack and deals with the return type
//  inside reflection.
//
class InvokeUtil
{

public:
    struct _ObjectToTypedReferenceArgs
    {
        TypeHandle      th;
        OBJECTREF       obj;
        TypedByRef      typedReference;
    };

    static void CheckArg(TypeHandle th, OBJECTREF* obj, RefSecContext *pSCtx);

    // CopyArg
    // To make the code endianess aware, the destination is specified twice: 
    // once as argslot, second time as a raw buffer. The appropriate one will
    // be used based on the type:
    //  pSlot - the argslot for primitive types and objectrefs
    //  pDst - the buffer for value types and typedrefs
    static void CopyArg(TypeHandle th, OBJECTREF *obj, ARG_SLOT* pSlot, void *pDst);
   
    // Given a type, this routine will convert an ARG_SLOT representing that
    //  type into an ObjectReference.  If the type is a primitive, the 
    //  value is wrapped in one of the Value classes.
    static OBJECTREF CreateObject(TypeHandle th,ARG_SLOT value);

    // This is a special purpose Exception creation function.  It
    //  creates the TargetInvocationExeption placing the passed
    //  exception into it.
    static OBJECTREF CreateTargetExcept(OBJECTREF* except);

    // This is a special purpose Exception creation function.  It
    //  creates the ReflectionClassLoadException placing the passed
    //  classes array and exception array into it.
    static OBJECTREF CreateClassLoadExcept(OBJECTREF* classes,OBJECTREF* except);

    // Validate that the field can be widened for Set
    static void ValidField(TypeHandle th, OBJECTREF* value, RefSecContext *pSCtx);

    static void CheckSecurity();

    // ChangeType
    // This method will invoke the Binder change type method on the object
    //  binder -- The Binder object
    //  srcObj -- The source object to be changed
    //  th -- The TypeHandel of the target type
    //  locale -- The locale passed to the class.
    static OBJECTREF ChangeType(OBJECTREF binder,OBJECTREF srcObj,TypeHandle th,OBJECTREF locale);

    // CreatePrimitiveValue
    // This routine will validate the object and then place the value into 
    //  the destination
    //  dstType -- The type of the destination
    //  srcType -- The type of the source
    //  srcObj -- The Object containing the primitive value.
    //  pDst -- poiner to the destination
    static void CreatePrimitiveValue(CorElementType dstType, CorElementType srcType, OBJECTREF srcObj, ARG_SLOT* pDst);

    // CreatePrimitiveValue
    // This routine will validate the object and then place the value into 
    //  the destination
    //  dstType -- The type of the destination
    //  srcType -- The type of the source
    //  pSrc -- pointer to source data.
    //  pSrcMT - MethodTable of source type
    //  pDst -- poiner to the destination
    static void CreatePrimitiveValue(CorElementType dstType,CorElementType srcType,
        void *pSrc, MethodTable *pSrcMT, ARG_SLOT* pDst);

    // IsPrimitiveType
    // This method will verify the passed in type is a primitive or not
    //	type -- the CorElementType to check for
    inline static DWORD IsPrimitiveType(const CorElementType type)
    {
        LEAF_CONTRACT;
        
        if (type >= PRIMITIVE_TABLE_SIZE)
        {
            if (ELEMENT_TYPE_I==type || ELEMENT_TYPE_U==type)
            {
                return TRUE;
            }
            return 0;
        }

        return (PT_Primitive & PrimitiveAttributes[type]);
    }

    // CanPrimitiveWiden
    // This method determines if the srcType and be widdened without loss to the destType
    //  destType -- The target type
    //  srcType -- The source type.
    inline static DWORD CanPrimitiveWiden(const CorElementType destType, const CorElementType srcType)
    {
        LEAF_CONTRACT;
        
        if (destType >= PRIMITIVE_TABLE_SIZE || srcType >= PRIMITIVE_TABLE_SIZE)
        {
            if ((ELEMENT_TYPE_I==destType && ELEMENT_TYPE_I==srcType) ||
                (ELEMENT_TYPE_U==destType && ELEMENT_TYPE_U==srcType))
            {
                return TRUE;
            }
            return 0;
        }
        return ((1 << destType) & PrimitiveAttributes[srcType]);
    }

    // Field Stuff.  The following stuff deals with fields making it possible
    //  to set/get field values on objects

    // SetValidField
    // Given an target object, a value object and a field this method will set the field
    //  on the target object.  The field must be validate before calling this.
    static void SetValidField(CorElementType fldType, TypeHandle fldTH, FieldDesc* pField, OBJECTREF* target, OBJECTREF* value, TypeHandle declaringType, CLR_BOOL *pDomainInitialized);

    static OBJECTREF GetFieldValue(FieldDesc* pField, TypeHandle fieldType, OBJECTREF* target, TypeHandle declaringType, CLR_BOOL *pDomainInitialized);

    // ValidateObjectTarget
    // This method will validate the Object/Target relationship
    //  is correct.  It throws an exception if this is not the case.
    static void ValidateObjectTarget(FieldDesc* pField,TypeHandle fldType,OBJECTREF *target);

    static TypeHandle GetPointerType(OBJECTREF* pObj);
    static void* GetPointerValue(OBJECTREF* pObj);
    static void* GetIntPtrValue(OBJECTREF* pObj);
    static void* GetUIntPtrValue(OBJECTREF* pObj);

    // Check accessability of a field or method.
    // pOptionalTargetMethod should be NULL for a field, and may be NULL for a method.
    // If checking a generic method with a method instantiation, 
    // the method should be in as pOptionalTargetMethod so
    // that the accessibilty of its type arguments is checked too.
    static bool CheckAccess(RefSecContext *pCtx, DWORD dwAttributes, MethodTable *pTargetMT, MethodTable* pInstanceMT, MethodDesc* pTargetMD, DWORD dwFlags, FieldDesc* pOptionTargetField = NULL);

    // Check accessability of a type or nested type.
    static bool CheckAccessType(RefSecContext *pCtx, MethodTable *pClass, DWORD dwFlags);

    // If a method has a linktime demand attached, perform it.
    static bool CheckLinktimeDemand(RefSecContext *pCtx, MethodDesc *pMeth, bool fThrowOnError);

private:
    static void CreateValueTypeValue(TypeHandle dstTH,void* pDst,CorElementType srcType,TypeHandle srcTH,OBJECTREF srcObj);

    static void* CreateByRef(TypeHandle dstTh,CorElementType srcType, TypeHandle srcTH,OBJECTREF srcObj, OBJECTREF *pIncomingObj);

    // GetBoxedObject
    // Given an address of a primitve type, this will box that data...
    static OBJECTREF GetBoxedObject(TypeHandle th,void* pData);

private:
    // The Attributes Table
    // This constructs a table of legal widening operations
    //  for the primitive types.
    static DWORD const PrimitiveAttributes[PRIMITIVE_TABLE_SIZE];
};


#endif // __INVOKEUTIL_H__
