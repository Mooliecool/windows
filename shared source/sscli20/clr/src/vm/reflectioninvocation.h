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
#ifndef _REFLECTIONINVOCATION_H_
#define _REFLECTIONINVOCATION_H_

#include "object.h"
#include "fcall.h"
#include "field.h"
#include "stackwalktypes.h"
#include "runtimehandles.h"
#include "invokeutil.h"

// NOTE: The following constants are defined in BindingFlags.cs
#define BINDER_IgnoreCase           0x01
#define BINDER_DeclaredOnly         0x02
#define BINDER_Instance             0x04
#define BINDER_Static               0x08
#define BINDER_Public               0x10
#define BINDER_NonPublic            0x20
#define BINDER_FlattenHierarchy     0x40

#define BINDER_InvokeMethod         0x00100
#define BINDER_CreateInstance       0x00200
#define BINDER_GetField             0x00400
#define BINDER_SetField             0x00800
#define BINDER_GetProperty          0x01000
#define BINDER_SetProperty          0x02000
#define BINDER_PutDispProperty      0x04000
#define BINDER_PutRefDispProperty   0x08000

#define BINDER_ExactBinding         0x010000
#define BINDER_SuppressChangeType   0x020000
#define BINDER_OptionalParamBinding 0x040000

#define BINDER_IgnoreReturn         0x1000000

#define BINDER_DefaultLookup        (BINDER_Instance | BINDER_Static | BINDER_Public)
#define BINDER_AllLookup            (BINDER_Instance | BINDER_Static | BINDER_Public | BINDER_Instance)

void CanAccess(MethodDesc* pMeth, MethodTable* pParentMT, MethodTable* pInstanceMT, RefSecContext *pSCtx,
                bool verifyAccess = true, 
                bool checkSkipVer = false, 
                bool thisIsImposedSecurity = false,
                bool knowForSureImposedSecurityState = false);

class ReflectionInvocation {

public:
    static FCDECL1(void, RunClassConstructor, LPVOID handle);
    static FCDECL1(void, RunModuleConstructor, LPVOID handle);
    static FCDECL2(void, PrepareMethod, LPVOID handle, PTRArray* instantiationUNSAFE);
    static FCDECL1(void, PrepareDelegate, Object* delegateUNSAFE);
    static FCDECL0(void, ProbeForSufficientStack);
    static FCDECL3(void, ExecuteCodeWithGuaranteedCleanup, Object* pCodeDelegateUNSAFE, Object* pBackoutDelegateUNSAFE, Object* pUserDataUNSAFE);

    // TypedReference functions, should go somewhere else
    static FCDECL4(void, MakeTypedReference, TypedByRef * value, Object* targetUNSAFE, ArrayBase* fldsUNSAFE, EnregisteredTypeHandle enregFieldType);
    static FCDECL2(void, SetTypedReference, TypedByRef * target, Object* objUNSAFE);
    static FCDECL1(Object*, TypedReferenceToObject, TypedByRef * value);
    static FCDECL2(void, GetGUID, ReflectClassBaseObject* refThisUNSAFE, GUID * result);
    static FCDECL2_IV(Object*, CreateEnum, void *typeHandle, INT64 value);

    // helper fcalls for invocation
    static FCDECL1(DWORD, GetSpecialSecurityFlags, MethodDesc *pMethod);
    static FCDECL2(FC_BOOL_RET, CanValueSpecialCast, void *valueType, void *targetType);
    static FCDECL2(Object*, AllocateObjectForByRef, void *targetType, Object *valueUNSAFE);

    static FCDECL4(void, PerformSecurityCheck, Object *target, MethodDesc *pMeth, EnregisteredTypeHandle enregParent, DWORD dwFlags);
    static FCDECL2(void, CheckArgs, PTRArray *objs, SignatureNative sig);

    static FCDECL5(void, PerformVisibilityCheckOnField, FieldDesc *fieldDesc, Object *target, void *declaringTH, DWORD attr, DWORD invocationFlags);

    static void PrepareDelegateHelper(OBJECTREF* pDelegate);
    static void CanCacheTargetAndCrackedSig(MethodDesc* pMD);
};

class ReflectionSerialization {
public:
    static FCDECL1(Object*, GetUninitializedObject, ReflectClassBaseObject* objTypeUNSAFE);
    static FCDECL1(Object*, GetSafeUninitializedObject, ReflectClassBaseObject* objTypeUNSAFE);

};

class ReflectionEnum {
public:
    static FCDECL1(Object *, InternalGetEnumUnderlyingType, ReflectClassBaseObject *target);
    static FCDECL1(Object *, InternalGetEnumValue, Object *pRefThis);
    static FCDECL3(void, InternalGetEnumValues, ReflectClassBaseObject *target, Object **pReturnValues, Object **pReturnNames);
    static FCDECL2_IV(Object*, InternalBoxEnum, ReflectClassBaseObject* pEnumType, INT64 value);
    static FCDECL2(FC_BOOL_RET, ReflectionEnum::InternalEquals, Object *pRefThis, Object* pRefTarget);
    static FCDECL2(int, ReflectionEnum::InternalCompareTo, Object *pRefThis, Object* pRefTarget);
};

class ReflectionBinder {
public:
    static FCDECL2(FC_BOOL_RET, DBCanConvertPrimitive, ReflectClassBaseObject* vSource, ReflectClassBaseObject* vTarget);
    static FCDECL2(FC_BOOL_RET, DBCanConvertObjectPrimitive, Object* vSourceObj, ReflectClassBaseObject* vTarget);


};

#endif // _REFLECTIONINVOCATION_H_
