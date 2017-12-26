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

#include "common.h"
#include "invokeutil.h"
#include "comstring.h"
#include "corpriv.h"
#include "method.hpp"
#include "threads.h"
#include "excep.h"
#include "gcscan.h"
#include "remoting.h"
#include "security.h"
#include "field.h"
#include "customattribute.h"
#include "eeconfig.h"
#include "generics.h"

// The Attributes Table
//  20 bits for built in types and 12 bits for Properties
//  The properties are followed by the widening mask.  All types widen to them selves.
const DWORD InvokeUtil::PrimitiveAttributes[PRIMITIVE_TABLE_SIZE] = {
    0x00,                     // ELEMENT_TYPE_END
    0x00,                     // ELEMENT_TYPE_VOID
    PT_Primitive | 0x0004,    // ELEMENT_TYPE_BOOLEAN
    PT_Primitive | 0x3F88,    // ELEMENT_TYPE_CHAR (W = U2, CHAR, I4, U4, I8, U8, R4, R8) (U2 == Char)
    PT_Primitive | 0x3550,    // ELEMENT_TYPE_I1   (W = I1, I2, I4, I8, R4, R8) 
    PT_Primitive | 0x3FE8,    // ELEMENT_TYPE_U1   (W = CHAR, U1, I2, U2, I4, U4, I8, U8, R4, R8)
    PT_Primitive | 0x3540,    // ELEMENT_TYPE_I2   (W = I2, I4, I8, R4, R8)
    PT_Primitive | 0x3F88,    // ELEMENT_TYPE_U2   (W = U2, CHAR, I4, U4, I8, U8, R4, R8)
    PT_Primitive | 0x3500,    // ELEMENT_TYPE_I4   (W = I4, I8, R4, R8)
    PT_Primitive | 0x3E00,    // ELEMENT_TYPE_U4   (W = U4, I8, R4, R8)
    PT_Primitive | 0x3400,    // ELEMENT_TYPE_I8   (W = I8, R4, R8)
    PT_Primitive | 0x3800,    // ELEMENT_TYPE_U8   (W = U8, R4, R8)
    PT_Primitive | 0x3000,    // ELEMENT_TYPE_R4   (W = R4, R8)
    PT_Primitive | 0x2000,    // ELEMENT_TYPE_R8   (W = R8) 
};

MethodDesc  *RefSecContext::s_pMethPrivateProcessMessage = NULL;
MethodTable *RefSecContext::s_pTypeRuntimeMethodInfo = NULL;
MethodTable *RefSecContext::s_pTypeMethodBase = NULL;
MethodTable *RefSecContext::s_pTypeRuntimeConstructorInfo = NULL;
MethodTable *RefSecContext::s_pTypeConstructorInfo = NULL;
MethodTable *RefSecContext::s_pTypeRuntimeType = NULL;
MethodTable *RefSecContext::s_pTypeRuntimeTypeHandle = NULL;
MethodTable *RefSecContext::s_pTypeType = NULL;
MethodTable *RefSecContext::s_pTypeRuntimeFieldInfo = NULL;
MethodTable *RefSecContext::s_pTypeFieldInfo = NULL;
MethodTable *RefSecContext::s_pTypeRtFieldInfo = NULL;
MethodTable *RefSecContext::s_pTypeRuntimeEventInfo = NULL;
MethodTable *RefSecContext::s_pTypeEventInfo = NULL;
MethodTable *RefSecContext::s_pTypeRuntimePropertyInfo = NULL;
MethodTable *RefSecContext::s_pTypePropertyInfo = NULL;
MethodTable *RefSecContext::s_pTypeActivator = NULL;
MethodTable *RefSecContext::s_pTypeAppDomain = NULL;
MethodTable *RefSecContext::s_pTypeAssembly = NULL;
MethodTable *RefSecContext::s_pTypeTypeDelegator = NULL;
MethodTable *RefSecContext::s_pTypeDelegate = NULL;
MethodTable *RefSecContext::s_pTypeMulticastDelegate = NULL;
MethodTable *RefSecContext::s_pTypeRuntimeMethodHandle = NULL;
MethodTable *RefSecContext::s_pTypeRuntimeFieldHandle = NULL;

TypeHandle InvokeUtil::GetPointerType(OBJECTREF* pObj) {
    CONTRACT(TypeHandle) {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pObj));
        PRECONDITION(IsProtectedByGCFrame (pObj));
        POSTCONDITION(!RETVAL.IsNull());

        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACT_END;

    FieldDesc *ptrType = g_Mscorlib.GetField(FIELD__POINTER__TYPE);
    REFLECTCLASSBASEREF o = (REFLECTCLASSBASEREF)ptrType->GetRefValue(*pObj);
    TypeHandle typeHandle = o->GetType();
    RETURN typeHandle;
}

void* InvokeUtil::GetPointerValue(OBJECTREF* pObj) {
    CONTRACT(void*) {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pObj));
        PRECONDITION(IsProtectedByGCFrame (pObj));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));

        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACT_END;

    FieldDesc *ptrValue = g_Mscorlib.GetField(FIELD__POINTER__VALUE);
    void *value = (void*)ptrValue->GetValuePtr(*pObj);
    RETURN value;
}

void *InvokeUtil::GetIntPtrValue(OBJECTREF* pObj) {
    CONTRACT(void*) {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pObj));
        PRECONDITION(IsProtectedByGCFrame (pObj));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));

        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACT_END;
    
    FieldDesc *intPtrValue = g_Mscorlib.GetField(FIELD__INTPTR__VALUE);
    void *value = (void*)intPtrValue->GetValuePtr(*pObj);
    RETURN value;
}

void *InvokeUtil::GetUIntPtrValue(OBJECTREF* pObj) {
    CONTRACT(void*) {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pObj));
        PRECONDITION(IsProtectedByGCFrame (pObj));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));

        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACT_END;
    
    FieldDesc *uintPtrValue = g_Mscorlib.GetField(FIELD__UINTPTR__VALUE);
    void *value = (void*)uintPtrValue->GetValuePtr(*pObj);
    RETURN value;
}

// CopyArg
// To make the code endianess aware, the destination is specified twice: 
// once as argslot, second time as a raw buffer. The appropriate one will
// be used based on the type:
//  pSlot - the argslot for primitive types and objectrefs
//  pDst - the buffer for value types and typedrefs
void InvokeUtil::CopyArg(TypeHandle th, OBJECTREF *pObjUNSAFE, ARG_SLOT* pSlot, void *pDst) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(!th.IsNull());
        PRECONDITION(CheckPointer(pObjUNSAFE));
        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;
    
    OBJECTREF rObj = *pObjUNSAFE;
    MethodTable* pMT;
    CorElementType oType;
    CorElementType type;

    {
        // raw interior pointer is used in this funciton, 
        // which means GC can't happen in this segment
        GCX_FORBID ();
        if (rObj != 0) {
            pMT = rObj->GetMethodTable();
            oType = pMT->GetInternalCorElementType();
        }
        else {
            pMT = 0;
            oType = ELEMENT_TYPE_OBJECT;
        }
        type = th.GetVerifierCorElementType();
    }
    
    GCPROTECT_BEGIN (rObj);
    // This basically maps the Signature type our type and calls the CreatePrimitiveValue
    //  method.  We can nuke this if we get alignment on these types.
    switch (type) {
    case ELEMENT_TYPE_BOOLEAN:
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
    case ELEMENT_TYPE_R4:
    case ELEMENT_TYPE_R8:
    {
        // If we got the univeral zero...Then assign it and exit.
        if (rObj == 0)
            *pSlot = 0;
        else
            CreatePrimitiveValue(type, oType, rObj, pSlot);
        break;
    }

    case ELEMENT_TYPE_VALUETYPE:
    {
        // If we got the univeral zero...Then assign it and exit.
        if (rObj == 0) {
            int size = th.GetClass()->GetNumInstanceFieldBytes();
            memset(pDst, 0, size);
         }
        else {
            TypeHandle srcTH = (rObj)->GetTypeHandle();
            CreateValueTypeValue(th, pDst, oType, srcTH, rObj);
        }
        break;
    }

    case ELEMENT_TYPE_SZARRAY:          // Single Dim
    case ELEMENT_TYPE_ARRAY:            // General Array
    case ELEMENT_TYPE_CLASS:            // Class
    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_STRING:           // System.String
    case ELEMENT_TYPE_VAR:
    {
        if (rObj == 0) 
            *pSlot = 0;
        else
            *pSlot = ObjToArgSlot(rObj);
        break;
    }

    case ELEMENT_TYPE_BYREF:
    {
       // 
       //     (obj is the parameter passed to MethodInfo.Invoke, by the caller)
       //     if argument is a primitive
       //     {
       //         if incoming argument, obj, is null
       //             Allocate a boxed object and place ref to it in 'obj'
       //         Unbox 'obj' and pass it to callee
       //     }
       //     if argument is a value class
       //     {
       //         if incoming argument, obj, is null
       //             Allocate an object of that valueclass, and place ref to it in 'obj'
       //         Unbox 'obj' and pass it to callee
       //     }
       //     if argument is an objectref
       //     {
       //         pass obj to callee
       //     }
       //
        TypeHandle thBaseType;
        TypeHandle srcTH;
        {
            // this case uses raw interior pointer, which assumes GC can't happen
            GCX_FORBID ();
            thBaseType = th.AsTypeDesc()->GetTypeParam();

                // We should never get here for nullable types.  Instead invoke
                // heads these off and morphs the type handle to not be byref anymore
            _ASSERTE(!Nullable::IsNullableType(thBaseType));

            srcTH = TypeHandle();
            if (rObj == 0) 
                oType = thBaseType.GetSignatureCorElementType();
            else
                srcTH = rObj->GetTypeHandle();
        }
        //CreateByRef only triggers GC in throw path, so it's OK to use the raw unsafe pointer
        *pSlot = PtrToArgSlot(CreateByRef(thBaseType, oType, srcTH, rObj, pObjUNSAFE));
        break;
    }

    case ELEMENT_TYPE_TYPEDBYREF:
    {        
        TypedByRef* ptr = (TypedByRef*) pDst;
        TypeHandle srcTH;
        BOOL bIsZero = FALSE;

        {
            // this case uses raw interior pointer, which assumes GC can't happen
            GCX_FORBID ();
            
            // If we got the univeral zero...Then assign it and exit.
            if (rObj== 0) {
                bIsZero = TRUE;
                ptr->data = 0;
                ptr->type = TypeHandle();                
            }
            else {
                bIsZero = FALSE;
                srcTH = rObj->GetTypeHandle();
                ptr->type = rObj->GetTypeHandle();                
            }
        }

        if (!bIsZero)
        {
            //CreateByRef only triggers GC in throw path
            ptr->data = CreateByRef(srcTH, oType, srcTH, rObj, pObjUNSAFE);
        }
        
        break;
    }

    case ELEMENT_TYPE_PTR: 
    case ELEMENT_TYPE_FNPTR:
    {
        // If we got the univeral zero...Then assign it and exit.
        if (rObj == 0) {
            *pSlot = 0;
        }
        else {
            if (g_Mscorlib.IsClass(rObj->GetMethodTable(), CLASS__POINTER) && type == ELEMENT_TYPE_PTR) 
                // because we are here only if obj is a System.Reflection.Pointer GetPointerValue()
                // should not cause a gc (no transparent proxy). If otherwise we got a nasty bug here
                *pSlot = PtrToArgSlot(GetPointerValue(&rObj));
            else if (rObj->GetTypeHandle().AsMethodTable() == g_Mscorlib.GetExistingClass(CLASS__INTPTR)) 
                CreatePrimitiveValue(oType, oType, rObj, pSlot);
            else
                COMPlusThrow(kArgumentException,L"Arg_ObjObj");
        }
        break;
    }

    case ELEMENT_TYPE_VOID:
    default:
        _ASSERTE(!"Unknown Type");
        COMPlusThrow(kNotSupportedException);
    }

    GCPROTECT_END ();
}

// CreatePrimitiveValue
// This routine will validate the object and then place the value into 
//  the destination
//  dstType -- The type of the destination
//  srcType -- The type of the source
//  srcObj -- The Object containing the primitive value.
//  pDst -- pointer to the destination
void InvokeUtil::CreatePrimitiveValue(CorElementType dstType, 
                                      CorElementType srcType,
                                      OBJECTREF srcObj,
                                      ARG_SLOT *pDst) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(srcObj != NULL);
        PRECONDITION(CheckPointer(pDst));
        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;
    CreatePrimitiveValue(dstType, srcType, srcObj->UnBox(), srcObj->GetMethodTable(), pDst);
}

void InvokeUtil::CreatePrimitiveValue(CorElementType dstType,CorElementType srcType,
    void *pSrc, MethodTable *pSrcMT, ARG_SLOT* pDst)
{

    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pDst));
        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;

    if (!IsPrimitiveType(srcType) || !CanPrimitiveWiden(dstType, srcType))
        COMPlusThrow(kArgumentException, L"Arg_PrimWiden");

    ARG_SLOT data = 0;
 
    switch (srcType) {
    case ELEMENT_TYPE_I1:
        data = *(INT8*)pSrc;
        break;
    case ELEMENT_TYPE_I2:
        data = *(INT16*)pSrc;
        break;
    IN_WIN32(case ELEMENT_TYPE_I:)
    case ELEMENT_TYPE_I4:
        data = *(INT32 *)pSrc;
        break;
    IN_WIN64(case ELEMENT_TYPE_I:)
    case ELEMENT_TYPE_I8:
        data = *(INT64 *)pSrc;
        break;
    default:
        switch (pSrcMT->GetNumInstanceFieldBytes())
        {
        case 1:
            data = *(UINT8 *)pSrc;
            break;
        case 2:
            data = *(UINT16 *)pSrc;
            break;
        case 4:
            data = *(UINT32 *)pSrc;
            break;
        case 8:
            data = *(UINT64 *)pSrc;
            break;
        default:
            memcpy(&data,pSrc,pSrcMT->GetNumInstanceFieldBytes());
            break;
        }
    }

    if (srcType == dstType) {
        // shortcut
        *pDst = data;
        return;
    }

    // Copy the data and return
    switch (dstType) {
    case ELEMENT_TYPE_BOOLEAN:
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_CHAR:
    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
        switch (srcType) {
        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_CHAR:
        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
        case ELEMENT_TYPE_I:
        case ELEMENT_TYPE_U:
        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
            *pDst = data;
            break;
        case ELEMENT_TYPE_R4:
            *pDst = (I8)(*(R4*)pSrc);
            break;
        case ELEMENT_TYPE_R8:
            *pDst = (I8)(*(R8*)pSrc);
            break;
        default:
            _ASSERTE(!"Unknown conversion");
            // this is really an impossible condition
            COMPlusThrow(kNotSupportedException);
        }
        break;
    case ELEMENT_TYPE_R4:
    case ELEMENT_TYPE_R8:
        {
        R8 r8 = 0;
        switch (srcType) {
        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_I4:
        IN_WIN32(case ELEMENT_TYPE_I:)
            r8 = (R8)((INT32)data);
            break;
        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_CHAR:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_U4:
        IN_WIN32(case ELEMENT_TYPE_U:)
            r8 = (R8)((UINT32)data);
            break;
        case ELEMENT_TYPE_U8:
        IN_WIN64(case ELEMENT_TYPE_U:)
            r8 = (R8)((UINT64)data);
            break;
        case ELEMENT_TYPE_I8:
        IN_WIN64(case ELEMENT_TYPE_I:)
            r8 = (R8)((INT64)data);
            break;
        case ELEMENT_TYPE_R4:
            r8 = *(R4*)pSrc;
            break;
        case ELEMENT_TYPE_R8:
            r8 = *(R8*)pSrc;
            break;
        default:
            _ASSERTE(!"Unknown R4 or R8 conversion");
            // this is really an impossible condition
            COMPlusThrow(kNotSupportedException);
        }

        if (dstType == ELEMENT_TYPE_R4) {
            R4 r4 = (R4)r8;
            *pDst = (UINT32&)r4;
        }
        else {
            *pDst = (UINT64&)r8;
        }

        }
        break;
    default:
        _ASSERTE(!"Unknown conversion");
    }
}

void* InvokeUtil::CreateByRef(TypeHandle dstTh,
                              CorElementType srcType, 
                              TypeHandle srcTH,
                              OBJECTREF srcObj, 
                              OBJECTREF *pIncomingObj) {
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(!dstTh.IsNull());
        PRECONDITION(CheckPointer(pIncomingObj));

        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;

    CorElementType dstType = dstTh.GetSignatureCorElementType();
    if (IsPrimitiveType(srcType) && IsPrimitiveType(dstType)) {
        if (dstType != srcType)
        {
            CONTRACT_VIOLATION (GCViolation);
            COMPlusThrow(kArgumentException,L"Arg_PrimWiden");
        }

        return srcObj->UnBox();
    }

    if (srcTH.IsNull()) {
        return pIncomingObj;
    }

    _ASSERTE(srcObj != NULL);

    if (dstType == ELEMENT_TYPE_VALUETYPE) {
        return srcObj->UnBox();
    }
    else
        return pIncomingObj;
}

void InvokeUtil::CreateValueTypeValue(TypeHandle dstTH,
                                      void* pDst,
                                      CorElementType srcType,
                                      TypeHandle srcTH,
                                      OBJECTREF srcObj) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(!dstTH.IsNull());
        PRECONDITION(CheckPointer(pDst));
        PRECONDITION(!srcTH.IsNull());
        PRECONDITION(srcObj != NULL);
        
        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;

    if (!dstTH.GetMethodTable()->UnBoxInto(pDst, srcObj))
        COMPlusThrow(kArgumentException, L"Arg_ObjObj");
}

// GetBoxedObject
// Given an address of a primitve type, this will box that data...
OBJECTREF InvokeUtil::GetBoxedObject(TypeHandle th, void* pData) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(!th.IsNull());
        PRECONDITION(CheckPointer(pData));
        
        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;

    MethodTable *pMethTable = th.GetMethodTable();
    PREFIX_ASSUME(pMethTable != NULL);
    // Save off the data.  We are going to create and object
    //  which may cause GC to occur.
    int size = pMethTable->GetClass()->GetNumInstanceFieldBytes();
    void *p = _alloca(size);
    memcpy(p, pData, size);
    OBJECTREF retO = pMethTable->Box(p);
    return retO;
}

//ValidField
// This method checks that the object can be widened to the proper type
void InvokeUtil::ValidField(TypeHandle th, OBJECTREF* value, RefSecContext *pSCtx) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(!th.IsNull());
        PRECONDITION(CheckPointer(value));
        PRECONDITION(IsProtectedByGCFrame (value));
        PRECONDITION(CheckPointer(pSCtx, NULL_OK));
        
        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;

    if ((*value) == 0)
        return;

    MethodTable* pMT;
    CorElementType oType;
    CorElementType type = th.GetSignatureCorElementType();
    pMT = (*value)->GetMethodTable();
    oType = TypeHandle(pMT).GetSignatureCorElementType();

    // handle pointers
    if (type == ELEMENT_TYPE_PTR || type == ELEMENT_TYPE_FNPTR) {
        if ((*value)->GetTypeHandle() == TypeHandle(g_Mscorlib.GetClass(CLASS__POINTER)) && type == ELEMENT_TYPE_PTR) {
            TypeHandle srcTH = GetPointerType(value);

            if (th != TypeHandle(g_Mscorlib.GetType(TYPE__VOID_PTR))) {
                if (!srcTH.CanCastTo(th))
                    COMPlusThrow(kArgumentException,L"Arg_ObjObj");
            }
            //if (!Security::CanSkipVerification(pSCtx->GetCallerMethod()->GetModule()))
                //Security::ThrowSecurityException(g_SecurityPermissionClassName, SPFLAGSSKIPVERIFICATION);
            Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_SKIP_VER);
            return;
        }
        else if ((*value)->GetTypeHandle().AsMethodTable() == g_Mscorlib.FetchClass(CLASS__INTPTR)) {
            //if (!Security::CanSkipVerification(pSCtx->GetCallerMethod()->GetModule()))
                //Security::ThrowSecurityException(g_SecurityPermissionClassName, SPFLAGSSKIPVERIFICATION);
            Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_SKIP_VER);
            return;
        }

        COMPlusThrow(kArgumentException,L"Arg_ObjObj");
    }

   // Need to handle Object special
    if (type == ELEMENT_TYPE_CLASS  || type == ELEMENT_TYPE_VALUETYPE ||
            type == ELEMENT_TYPE_OBJECT || type == ELEMENT_TYPE_STRING ||
            type == ELEMENT_TYPE_ARRAY  || type == ELEMENT_TYPE_SZARRAY) 
    {

        if (th.GetMethodTable() == g_pObjectClass)
            return;
        if (IsPrimitiveType(oType)) {
            if (type != ELEMENT_TYPE_VALUETYPE)
                COMPlusThrow(kArgumentException,L"Arg_ObjObj");

            // Legacy behavior: The following if disallows assigning primitives to enums.
            if (th.IsEnum())
                COMPlusThrow(kArgumentException,L"Arg_ObjObj");

            type = th.GetVerifierCorElementType();
            if (IsPrimitiveType(type)) 
                if (CanPrimitiveWiden(type, oType)) 
                    return;
                else 
                    COMPlusThrow(kArgumentException,L"Arg_ObjObj");
        }

        //Get the type handle.  For arrays we need to
        //  get it from the object itself.
        TypeHandle h;
        MethodTable* pSrcMT = (*value)->GetMethodTable();
        if (pSrcMT->IsArray())
            h = ((BASEARRAYREF)(*value))->GetTypeHandle();
        else
            h = TypeHandle(pSrcMT);

        if(h.GetMethodTable()->IsThunking()) {
            // Extract the true class that the thunking class represents
            h = (*value)->GetTrueTypeHandle();
        }

        if (!h.CanCastTo(th) && !Nullable::IsNullableForType(th, h.GetMethodTable())) {
            BOOL fCastOK = FALSE;
            // Give thunking classes a second chance to check the cast
            if ((*value)->GetMethodTable()->IsTransparentProxyType()) {
                fCastOK = CRemotingServices::CheckCast(*value, th);
            }
            else {
                // If the object is a COM object then we need to check to see
                // if it implements the interface.
                MethodTable *pSrcComMT = h.GetMethodTable();
                MethodTable *pDstComMT = th.GetMethodTable();
                if (pDstComMT->IsInterface() && pSrcComMT->IsComObjectType() && Object::SupportsInterface(*value, pDstComMT))
                    fCastOK = TRUE;
            }

            if(!fCastOK)
                COMPlusThrow(kArgumentException,L"Arg_ObjObj");
        }
        return;
    }


    if (!IsPrimitiveType(oType))
        COMPlusThrow(kArgumentException,L"Arg_ObjObj");
    // Now make sure we can widen into the proper type -- CanWiden may run GC...
    if (!CanPrimitiveWiden(type,oType))
        COMPlusThrow(kArgumentException,L"Arg_ObjObj");
}

void InvokeUtil::CheckSecurity() {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        
        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;

    // Call the check
    Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, REFLECTION_TYPE_INFO);
}

// InternalCreateObject
// This routine will create the specified object from the ARG_SLOT value
OBJECTREF InvokeUtil::CreateObject(TypeHandle th, ARG_SLOT value) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(!th.IsNull());
        
        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;

    CorElementType type = th.GetSignatureCorElementType();
    MethodTable *pMT = NULL;
    OBJECTREF obj = NULL;

    // Handle the non-table types
    switch (type) {
    case ELEMENT_TYPE_VOID:
        break;

    case ELEMENT_TYPE_PTR:
    {
        struct _gc 
        {
            OBJECTREF refObj;
            OBJECTREF refType;
        } gc;
        gc.refObj = AllocateObject(g_Mscorlib.GetClass(CLASS__POINTER));
        gc.refType = NULL;
        GCPROTECT_BEGIN(gc);
        gc.refType = th.GetManagedClassObject();
        FieldDesc *pField = g_Mscorlib.GetField(FIELD__POINTER__TYPE);
        pField->SetRefValue(gc.refObj, gc.refType);
        pField = g_Mscorlib.GetField(FIELD__POINTER__VALUE);
        pField->SetValuePtr(gc.refObj, ArgSlotToPtr(value));
        GCPROTECT_END();
        obj = gc.refObj;
        break;
    }

    case ELEMENT_TYPE_FNPTR:
        pMT = g_Mscorlib.GetExistingClass(CLASS__INTPTR);
        goto PrimitiveType;

    case ELEMENT_TYPE_VALUETYPE:
    {
        _ASSERTE(th.IsUnsharedMT());
        pMT = th.AsMethodTable();
        obj = pMT->Box(ArgSlotEndianessFixup(&value, pMT->GetClass()->GetNumInstanceFieldBytes()));
        break;
    }

    case ELEMENT_TYPE_CLASS:        // Class
    case ELEMENT_TYPE_SZARRAY:      // Single Dim, Zero
    case ELEMENT_TYPE_ARRAY:        // General Array
    case ELEMENT_TYPE_STRING:
    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_VAR:
        obj = ArgSlotToObj(value);
        break;
    
    case ELEMENT_TYPE_BOOLEAN:      // boolean
    case ELEMENT_TYPE_I1:           // byte
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_I2:           // short
    case ELEMENT_TYPE_U2:           
    case ELEMENT_TYPE_CHAR:         // char
    case ELEMENT_TYPE_I4:           // int
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_I8:           // long
    case ELEMENT_TYPE_U8:       
    case ELEMENT_TYPE_R4:           // float
    case ELEMENT_TYPE_R8:           // double
    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
        _ASSERTE(th.IsUnsharedMT());
        pMT = th.AsMethodTable();
    PrimitiveType:
        // Don't use MethodTable::Box here for perf reasons
        PREFIX_ASSUME(pMT != NULL);
        obj = FastAllocateObject(pMT);
        memcpyNoGCRefs(obj->UnBox(), ArgSlotEndianessFixup(&value,pMT->GetClass()->GetNumInstanceFieldBytes()), pMT->GetClass()->GetNumInstanceFieldBytes());
        break;
    
    case ELEMENT_TYPE_BYREF:
        COMPlusThrow(kNotSupportedException, L"NotSupported_ByRefReturn");
    case ELEMENT_TYPE_END:
    default:
        _ASSERTE(!"Unknown Type");
        COMPlusThrow(kNotSupportedException);
    }
    
    return obj;    
}

// This is a special purpose Exception creation function.  It
//  creates the ReflectionTypeLoadException placing the passed
//  classes array and exception array into it.
OBJECTREF InvokeUtil::CreateClassLoadExcept(OBJECTREF* classes, OBJECTREF* except) {
    CONTRACT(OBJECTREF) {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(classes));
        PRECONDITION(CheckPointer(except));
        PRECONDITION(IsProtectedByGCFrame (classes));
        PRECONDITION(IsProtectedByGCFrame (except));
        
        POSTCONDITION(RETVAL != NULL);

        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACT_END;

    OBJECTREF oRet = 0;

    struct {
        OBJECTREF o;
        STRINGREF str;
    } gc;
    ZeroMemory(&gc, sizeof(gc));

    MethodTable *pVMClassLoadExcept = g_Mscorlib.GetException(kReflectionTypeLoadException);
    gc.o = AllocateObject(pVMClassLoadExcept);
    GCPROTECT_BEGIN(gc);
    ARG_SLOT args[4];

    // Retrieve the resource string.
    ResMgrGetString(L"ReflectionTypeLoad_LoadFailed", &gc.str);

    MethodDesc* pMD = gc.o->GetTrueMethodTable()->GetClass()->FindMethod(
                            COR_CTOR_METHOD_NAME, &gsig_IM_ArrType_ArrException_Str_RetVoid);

    if (!pMD)
    {
        MAKE_WIDEPTR_FROMUTF8(wzMethodName, COR_CTOR_METHOD_NAME);
        COMPlusThrowNonLocalized(kMissingMethodException, wzMethodName);
    }

    MethodDescCallSite ctor(pMD);

    // Call the constructor
    args[0]  = ObjToArgSlot(gc.o);
    args[1]  = ObjToArgSlot(*classes);
    args[2]  = ObjToArgSlot(*except);
    args[3]  = ObjToArgSlot((OBJECTREF)gc.str);

    ctor.Call_RetArgSlot(args);

    oRet = gc.o;

    GCPROTECT_END();
    RETURN oRet;
}

OBJECTREF InvokeUtil::CreateTargetExcept(OBJECTREF* except) {
    CONTRACT(OBJECTREF) {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(except));
        PRECONDITION(IsProtectedByGCFrame (except));
        
        POSTCONDITION(RETVAL != NULL);

        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACT_END;

    OBJECTREF o;
    OBJECTREF oRet = 0;

    MethodTable *pVMTargetExcept = g_Mscorlib.GetException(kTargetInvocationException);
    o = AllocateObject(pVMTargetExcept);
    GCPROTECT_BEGIN(o);
    ARG_SLOT args[2];

    MethodDesc* pMD = o->GetTrueMethodTable()->GetClass()->FindMethod(
                            COR_CTOR_METHOD_NAME, &gsig_IM_Exception_RetVoid);
    
    if (!pMD)
    {
        MAKE_WIDEPTR_FROMUTF8(wzMethodName, COR_CTOR_METHOD_NAME);
        COMPlusThrowNonLocalized(kMissingMethodException, wzMethodName);
    }

    MethodDescCallSite ctor(pMD);

    // Call the constructor
    args[0]  = ObjToArgSlot(o);
    // for security, don't allow a non-exception object to be spoofed as an exception object. We cast later and
    // don't check and this could cause us grief.
    _ASSERTE(!except || IsException((*except)->GetMethodTable()));  // how do we get non-exceptions?
    if (except && IsException((*except)->GetMethodTable()))
    {
        args[1]  = ObjToArgSlot(*except);
    }
    else
    {
        args[1] = NULL;
    }

    ctor.Call_RetArgSlot(args);

    oRet = o;

    GCPROTECT_END();
    RETURN oRet;
}

// ChangeType
// This method will invoke the Binder change type method on the object
//  binder -- The Binder object
//  srcObj -- The source object to be changed
//  th -- The TypeHandel of the target type
//  locale -- The locale passed to the class.
OBJECTREF InvokeUtil::ChangeType(OBJECTREF binder, OBJECTREF srcObj, TypeHandle th, OBJECTREF locale) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(binder != NULL);
        PRECONDITION(srcObj != NULL);
        
        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;
    
    OBJECTREF typeClass = NULL;
    OBJECTREF o;

    struct _gc {
        OBJECTREF binder;
        OBJECTREF srcObj;
        OBJECTREF locale;
        OBJECTREF typeClass;
    } gc; 

    gc.binder = binder;
    gc.srcObj = srcObj;
    gc.locale = locale;
    gc.typeClass = NULL;

    GCPROTECT_BEGIN(gc);

    MethodDescCallSite changeType(METHOD__BINDER__CHANGE_TYPE, &gc.binder);

    // Now call this method on this object.
    typeClass = th.GetManagedClassObject();

    ARG_SLOT pNewArgs[] = {
            ObjToArgSlot(gc.binder),
            ObjToArgSlot(gc.srcObj),
            ObjToArgSlot(gc.typeClass),
            ObjToArgSlot(gc.locale),
    };

    o = changeType.Call_RetOBJECTREF(pNewArgs);

    GCPROTECT_END();

    return o;
}

// Ensure that the field is declared on the type or subtype of the type to which the typed reference refers.
// Note that a typed reference is a reference to an object and is not a field on that object (as in C# ref).
// Ensure that if the field is an instance field that the typed reference is not null.
void InvokeUtil::ValidateObjectTarget(FieldDesc *pField, TypeHandle enclosingType, OBJECTREF *target) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pField));
        PRECONDITION(!enclosingType.IsNull() || pField->IsStatic());
        PRECONDITION(CheckPointer(target));
    
        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;
    
    if (pField->IsStatic() && (enclosingType.IsNull() || !*target))
        return;

    if (!pField->IsStatic() && !*target)
        COMPlusThrow(kTargetException,L"RFLCT.Targ_StatFldReqTarg");
        
    // Verify that the object is of the proper type...
    TypeHandle ty = (*target)->GetTrueTypeHandle();
    while (!ty.IsNull() && ty != enclosingType)
        ty = ty.GetParent();

    // Give a second chance to thunking classes to do the 
    // correct cast
    if (ty.IsNull()) {

        BOOL fCastOK = FALSE;
        if ((*target)->GetMethodTable()->IsThunking()) {
            fCastOK = CRemotingServices::CheckCast(*target, enclosingType);
        }
        if(!fCastOK) {
            COMPlusThrow(kArgumentException,L"Arg_ObjObj");
        }
    }
}

// SetValidField
// Given an target object, a value object and a field this method will set the field
//  on the target object.  The field must be validate before calling this.
void InvokeUtil::SetValidField(CorElementType fldType,
                               TypeHandle fldTH,
                               FieldDesc *pField,
                               OBJECTREF *target,
                               OBJECTREF *valueObj,
                               TypeHandle declaringType,
                               CLR_BOOL *pDomainInitialized) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(!fldTH.IsNull());
        PRECONDITION(CheckPointer(pField));
        PRECONDITION(CheckPointer(target));
        PRECONDITION(CheckPointer(valueObj));
        PRECONDITION(IsProtectedByGCFrame (target));
        PRECONDITION(IsProtectedByGCFrame (valueObj));        
        PRECONDITION(declaringType.IsNull () || declaringType.IsUnsharedMT());
    
        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;

    // We don't allow setting the field of nullable<T> (hasValue and value)
    // Because you can't independantly set them for this type.  
    if (!declaringType.IsNull() && Nullable::IsNullableType(declaringType.GetMethodTable())) 
        COMPlusThrow(kNotSupportedException);

    // call the <cinit> 
    OBJECTREF Throwable = NULL;
    HRESULT hr=S_OK;
    if (*pDomainInitialized == FALSE)
    {
        BEGIN_EXCEPTION_GLUE(&hr,&Throwable);
        if (declaringType.IsNull())
        {
            pField->GetModule()->GetGlobalMethodTable()->EnsureInstanceActive();
            pField->GetModule()->GetGlobalMethodTable()->CheckRunClassInitThrowing();
        }
        else
        {
            declaringType.GetMethodTable()->EnsureInstanceActive();
            declaringType.GetMethodTable()->CheckRunClassInitThrowing();   

            if (declaringType.IsDomainNeutral() == FALSE)
                *pDomainInitialized = TRUE;
        }
        END_EXCEPTION_GLUE;
    }
#ifdef _DEBUG
    else if (*pDomainInitialized == TRUE && !declaringType.IsNull())
       CONSISTENCY_CHECK(declaringType.GetMethodTable()->CheckActivated());
#endif

    if(FAILED(hr))
    {
        GCPROTECT_BEGIN(Throwable);
        OBJECTREF except = CreateTargetExcept(&Throwable);
        COMPlusThrow(except);
        GCPROTECT_END();
    }

    // Set the field
    ARG_SLOT value;

    void* valueptr;
    switch (fldType) {
    case ELEMENT_TYPE_VOID:
        _ASSERTE(!"Void used as Field Type!");
        COMPlusThrow(kNotSupportedException);

    case ELEMENT_TYPE_BOOLEAN:  // boolean
    case ELEMENT_TYPE_I1:       // byte
    case ELEMENT_TYPE_U1:       // unsigned byte
        value = 0;
        if (*valueObj != 0) {
            MethodTable *p = (*valueObj)->GetMethodTable();
            CorElementType oType = p->GetInternalCorElementType();
            CreatePrimitiveValue(fldType, oType, *valueObj, &value);
        }

        if (pField->IsStatic())
            pField->SetStaticValue8((unsigned char)value);
        else 
            pField->SetValue8(*target,(unsigned char)value);
        break;

    case ELEMENT_TYPE_I2:       // short
    case ELEMENT_TYPE_U2:       // unsigned short
    case ELEMENT_TYPE_CHAR:     // char
        value = 0;
        if (*valueObj != 0) {
            MethodTable *p = (*valueObj)->GetMethodTable();
            CorElementType oType = p->GetInternalCorElementType();
            CreatePrimitiveValue(fldType, oType, *valueObj, &value);
        }

        if (pField->IsStatic())
            pField->SetStaticValue16((short)value);
        else 
            pField->SetValue16(*target, (short)value);
        break;

    case ELEMENT_TYPE_I:
        valueptr = *valueObj != 0 ? GetIntPtrValue(valueObj) : NULL;
        if (pField->IsStatic()) 
            pField->SetStaticValuePtr(valueptr);
        else 
            pField->SetValuePtr(*target,valueptr);
        break;

    case ELEMENT_TYPE_U:
        valueptr = *valueObj != 0 ? GetUIntPtrValue(valueObj) : NULL;
        if (pField->IsStatic()) 
            pField->SetStaticValuePtr(valueptr);
        else 
            pField->SetValuePtr(*target,valueptr);
        break;
    
    case ELEMENT_TYPE_PTR:      // pointers
        if (*valueObj != 0 && (*valueObj)->GetTypeHandle() == TypeHandle(g_Mscorlib.GetClass(CLASS__POINTER))) {
            valueptr = GetPointerValue(valueObj);
            if (pField->IsStatic()) 
                pField->SetStaticValuePtr(valueptr);
            else 
                pField->SetValuePtr(*target,valueptr);
            break;
        }
        // drop through
    case ELEMENT_TYPE_FNPTR:
        valueptr = *valueObj != 0 ? GetIntPtrValue(valueObj) : NULL;
        if (pField->IsStatic()) 
            pField->SetStaticValuePtr(valueptr);
        else 
            pField->SetValuePtr(*target,valueptr);
        break;

    case ELEMENT_TYPE_I4:       // int
    case ELEMENT_TYPE_U4:       // unsigned int
    case ELEMENT_TYPE_R4:       // float
        value = 0;
        if (*valueObj != 0) {
            MethodTable *p = (*valueObj)->GetMethodTable();
            CorElementType oType = p->GetInternalCorElementType();
            CreatePrimitiveValue(fldType, oType, *valueObj, &value);
        }

        if (pField->IsStatic()) 
            pField->SetStaticValue32((int)value);
        else 
            pField->SetValue32(*target, (int)value);
        break;

    case ELEMENT_TYPE_I8:       // long
    case ELEMENT_TYPE_U8:       // unsigned long
    case ELEMENT_TYPE_R8:       // double
        value = 0;
        if (*valueObj != 0) {
            MethodTable *p = (*valueObj)->GetMethodTable();
            CorElementType oType = p->GetInternalCorElementType();
            CreatePrimitiveValue(fldType, oType, *valueObj, &value);
        }

        if (pField->IsStatic())
            pField->SetStaticValue64(value);
        else 
            pField->SetValue64(*target,value);
        break;

    case ELEMENT_TYPE_SZARRAY:          // Single Dim, Zero
    case ELEMENT_TYPE_ARRAY:            // General Array
    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_VAR:
        if (pField->IsStatic())
            pField->SetStaticOBJECTREF(*valueObj);
        else
            pField->SetRefValue(*target, *valueObj);
        break;

    case ELEMENT_TYPE_VALUETYPE:
    {
        _ASSERTE(fldTH.IsUnsharedMT());
        MethodTable *pMT = fldTH.AsMethodTable();
        if((*target) != NULL && (*target)->IsThunking()) {
            OBJECTREF val = *valueObj;        
            GCPROTECT_BEGIN(val)

            void* valueData;
            if (Nullable::IsNullableType(fldTH)) {
                // Special case for Nullable<T>, we need a true nullable that is gc protected.  The easiest
                // way to make one is to allocate an object on the heap 
                OBJECTREF trueNullable = fldTH.AsMethodTable()->Allocate();
                BOOL typesChecked = Nullable::UnBox(trueNullable->GetData(), val, fldTH.AsMethodTable());
                _ASSERTE(typesChecked);
                val = trueNullable;
                valueData = val->GetData();
            }
            else if (val == NULL) {
                // Null is the universal null object.  (Is this a good idea?)
                int size = pMT->GetClass()->GetNumInstanceFieldBytes();
                valueData = _alloca(size);
                memset(valueData, 0, size);
            }
            else 
                valueData = val->GetData();

            Object *puo = CRemotingServices::AlwaysUnwrap(OBJECTREFToObject(*target));
            OBJECTREF unwrapped = ObjectToOBJECTREF(puo);
            CRemotingServices::FieldAccessor(pField, unwrapped, valueData, FALSE);
            GCPROTECT_END();
        }
        else {
            void* pFieldData;
            if (pField->IsStatic()) 
                pFieldData = pField->GetPrimitiveOrValueClassStaticAddress();
            else 
                pFieldData = (*((BYTE**)target)) + pField->GetOffset() + sizeof(Object);

            if (*valueObj == NULL) 
                memset(pFieldData, 0, pMT->GetClass()->GetNumInstanceFieldBytes());
            else  
                pMT->UnBoxIntoUnchecked(pFieldData, *valueObj);
        }
    }
    break;

    default:
        _ASSERTE(!"Unknown Type");
        // this is really an impossible condition
        COMPlusThrow(kNotSupportedException);
    }
}

// GetFieldValue
// This method will return an ARG_SLOT containing the value of the field.
// GetFieldValue
// This method will return an ARG_SLOT containing the value of the field.
OBJECTREF InvokeUtil::GetFieldValue(FieldDesc* pField, TypeHandle fieldType, OBJECTREF* target, TypeHandle declaringType, CLR_BOOL *pDomainInitialized) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pField));
        PRECONDITION(!fieldType.IsNull());
        PRECONDITION(CheckPointer(target));
        PRECONDITION(declaringType.IsNull () || declaringType.IsUnsharedMT());

        INJECT_FAULT(COMPlusThrowOM()); 
    }
    CONTRACTL_END;

    OBJECTREF obj = NULL;

    // call the .cctor 
    OBJECTREF Throwable = NULL;
    HRESULT hr=S_OK;
    if (*pDomainInitialized == FALSE)
    {
        BEGIN_EXCEPTION_GLUE(&hr,&Throwable);
        if (declaringType.IsNull())
        {
            pField->GetModule()->GetGlobalMethodTable()->EnsureInstanceActive();
            pField->GetModule()->GetGlobalMethodTable()->CheckRunClassInitThrowing();
        }
        else
        {
            declaringType.GetMethodTable()->EnsureInstanceActive();
            declaringType.GetMethodTable()->CheckRunClassInitThrowing();   

            if (!declaringType.IsDomainNeutral())
                *pDomainInitialized = TRUE;
        }
        END_EXCEPTION_GLUE;
    }
#ifdef _DEBUG
    else if (*pDomainInitialized == TRUE && !declaringType.IsNull())
       CONSISTENCY_CHECK(declaringType.GetMethodTable()->CheckActivated());
#endif


    if(FAILED(hr))
    {
        GCPROTECT_BEGIN(Throwable);
        OBJECTREF except = CreateTargetExcept(&Throwable);
        COMPlusThrow(except);
        GCPROTECT_END();
    }

    // We don't allow getting the field just so we don't have more specical
    // cases than we need to.  The we need at least the throw check to insure
    // we don't allow data corruption, but 
    if (!declaringType.IsNull() && Nullable::IsNullableType(declaringType.GetMethodTable())) 
        COMPlusThrow(kNotSupportedException);

    switch (pField->GetFieldType()) {
    
    case ELEMENT_TYPE_BOOLEAN:  // boolean
    case ELEMENT_TYPE_I1:       // byte
    case ELEMENT_TYPE_U1:       // unsigned byte
    case ELEMENT_TYPE_I2:       // short
    case ELEMENT_TYPE_U2:       // unsigned short
    case ELEMENT_TYPE_CHAR:     // char
    case ELEMENT_TYPE_I4:       // int
    case ELEMENT_TYPE_U4:       // unsigned int
    case ELEMENT_TYPE_R4:       // float
    case ELEMENT_TYPE_I8:       // long
    case ELEMENT_TYPE_U8:       // unsigned long
    case ELEMENT_TYPE_R8:       // double
    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
    {
        // create the object and copy
        fieldType.AsMethodTable()->EnsureActive();
        obj = AllocateObject(fieldType.AsMethodTable());
        GCPROTECT_BEGIN(obj);
        if (pField->IsStatic()) 
            CopyValueClass(obj->UnBox(), 
                           pField->GetPrimitiveOrValueClassStaticAddress(), 
                           fieldType.AsMethodTable(), 
                           obj->GetAppDomain());
        else
            pField->GetInstanceField(*target, obj->UnBox());
        GCPROTECT_END();
        break;
    }

    case ELEMENT_TYPE_OBJECT:
    case ELEMENT_TYPE_CLASS:
    case ELEMENT_TYPE_SZARRAY:          // Single Dim, Zero
    case ELEMENT_TYPE_ARRAY:            // general array
    case ELEMENT_TYPE_VAR:
        if (pField->IsStatic()) 
            obj = pField->GetStaticOBJECTREF();
        else 
            obj = pField->GetRefValue(*target);
        break;

    case ELEMENT_TYPE_VALUETYPE:
    {
        // Value classes require createing a boxed version of the field and then
        //  copying from the source...
        // Allocate an object to return...
        _ASSERTE(fieldType.IsUnsharedMT());
        
        void *p = NULL;
        fieldType.AsMethodTable()->EnsureActive();
        obj = fieldType.AsMethodTable()->Allocate();
        GCPROTECT_BEGIN(obj);
        // calculate the offset to the field...
        if (pField->IsStatic())
            p = pField->GetPrimitiveOrValueClassStaticAddress();
        else {
            Object *o = OBJECTREFToObject(*target);
            if(o->IsThunking()) {
                Object *puo = (Object*)CRemotingServices::AlwaysUnwrap((Object*)o);
                OBJECTREF unwrapped = ObjectToOBJECTREF(puo);
                CRemotingServices::FieldAccessor(pField, unwrapped, (void*)obj->GetData(), TRUE);
            }
            else
                p = (*((BYTE**)target)) + pField->GetOffset() + sizeof(Object);
        }
        GCPROTECT_END();

        // copy the field to the unboxed object.
        // note: this will be done only for the non-remoting case
        if (p) {
            CopyValueClass(obj->GetData(), p, fieldType.AsMethodTable(), obj->GetAppDomain());
        }

        obj = Nullable::NormalizeBox(obj);
        break;
    }

    case ELEMENT_TYPE_FNPTR:
    {
        MethodTable *pIntPtrMT = g_Mscorlib.GetClass(CLASS__INTPTR);
        obj = AllocateObject(pIntPtrMT);
        GCPROTECT_BEGIN(obj);
        void *value = NULL;
        if (pField->IsStatic()) 
            value = pField->GetStaticValuePtr();
        else 
            value = pField->GetValuePtr(*target);
        CopyValueClass(obj->UnBox(), &value, pIntPtrMT, obj->GetAppDomain());
        GCPROTECT_END();
        break;
    }

    case ELEMENT_TYPE_PTR:
    {
        void *value = NULL;
        if (pField->IsStatic()) 
            value = pField->GetStaticValuePtr();
        else 
            value = pField->GetValuePtr(*target);
        obj = AllocateObject(g_Mscorlib.GetClass(CLASS__POINTER));
        GCPROTECT_BEGIN(obj);
        // Ignore null return
        FieldDesc * pFDPionterType = g_Mscorlib.GetField(FIELD__POINTER__TYPE);
        pFDPionterType->SetRefValue (obj, fieldType.GetManagedClassObject());
        FieldDesc * pFDPionterVal = g_Mscorlib.GetField(FIELD__POINTER__VALUE);
        pFDPionterVal->SetValuePtr (obj, value);        
        GCPROTECT_END();
        break;
    }

    default:
        _ASSERTE(!"Unknown Type");
        // this is really an impossible condition
        COMPlusThrow(kNotSupportedException);
    }

    return obj;
}

StackWalkAction RefSecContext::TraceCallerCallback(CrawlFrame* pCf, VOID* data) 
{
    CONTRACTL 
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        
        PRECONDITION(CheckPointer(pCf));
        PRECONDITION(CheckPointer(data));
    }
    CONTRACTL_END;
    
    RefSecContext *pCtx = (RefSecContext*)data;

    MethodDesc *pMeth = pCf->GetFunction();
    _ASSERTE(pMeth);        

    MethodTable* pCaller = pMeth->GetMethodTable();

    // If we see the top of a remoting chain (a call to StackBuilderSink.PrivateProcessMessage), we skip all the frames until
    // we see the bottom (a transparent proxy).
        
    if (pMeth == s_pMethPrivateProcessMessage) {
        _ASSERTE(!pCtx->m_fSkippingRemoting);
        pCtx->m_fSkippingRemoting = true;
        return SWA_CONTINUE;
    }
    if (!pCf->IsFrameless() && pCf->GetFrame()->GetFrameType() == Frame::TYPE_TP_METHOD_FRAME){
        pCtx->m_fSkippingRemoting = false;
        return SWA_CONTINUE;
    }
    if (pCtx->m_fSkippingRemoting)
        return SWA_CONTINUE;
        
    // If we are calling this from a reflection class we need to continue
    // up the chain (RuntimeMethodInfo, RuntimeConstructorInfo, MethodInfo,
    // ConstructorInfo).
    if (pCaller == s_pTypeRuntimeMethodInfo ||
        pCaller == s_pTypeMethodBase ||
        pCaller == s_pTypeRuntimeConstructorInfo ||
        pCaller == s_pTypeConstructorInfo ||
        pCaller == s_pTypeRuntimeType ||
        pCaller == s_pTypeRuntimeTypeHandle ||
        pCaller == s_pTypeRuntimeMethodHandle ||
        pCaller == s_pTypeRuntimeFieldHandle ||
        pCaller == s_pTypeType ||
        pCaller == s_pTypeRuntimeFieldInfo ||
        pCaller == s_pTypeRtFieldInfo ||
        pCaller == s_pTypeFieldInfo ||
        pCaller == s_pTypeRuntimeEventInfo ||
        pCaller == s_pTypeEventInfo ||
        pCaller == s_pTypeRuntimePropertyInfo ||
        pCaller == s_pTypePropertyInfo ||
        pCaller == s_pTypeActivator ||
        pCaller == s_pTypeAppDomain ||
        pCaller == s_pTypeAssembly ||
        pCaller == s_pTypeTypeDelegator ||
        pCaller == s_pTypeDelegate ||
        pCaller == s_pTypeMulticastDelegate)
    {
        return SWA_CONTINUE;
    }
        
    // Return the calling MethodDesc and AppDomain.
    pCtx->m_pCaller = pMeth;
    pCtx->m_pCallerDomain = pCf->GetAppDomain();
        
    return SWA_ABORT;
}

MethodDesc *RefSecContext::GetCallerMethod() {
    CONTRACTL {


        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (!m_fCheckedCaller) {

        GetThread()->StackWalkFrames(TraceCallerCallback, this, FUNCTIONSONLY | LIGHTUNWIND);
        // If we didn't find a caller, we were called through interop. In this
        // case we know we're going to get full permissions.
        if (m_pCaller == NULL && !m_fCheckedPerm) {
            m_fCallerHasPerm = true;
            m_fCheckedPerm = true;
        }
        m_fCheckedCaller = true;
    }

    return m_pCaller;
}

AppDomain *RefSecContext::GetCallerDomain() {
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    if (!m_fCheckedCaller) {
        GetThread()->StackWalkFrames(TraceCallerCallback, this, FUNCTIONSONLY | LIGHTUNWIND);
        // If we didn't find a caller, we were called through interop. In this
        // case we know we're going to get full permissions.
        if (m_pCaller == NULL && !m_fCheckedPerm) {
            m_fCallerHasPerm = true;

            m_fCheckedPerm = true;
        }
        m_fCheckedCaller = true;
    }

    return m_pCallerDomain;
}

MethodTable *RefSecContext::GetCallerMT() {
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    MethodDesc *pCaller = GetCallerMethod();
    return pCaller ? pCaller->GetMethodTable() : NULL;
}

bool RefSecContext::DemandMemberAccess(DWORD dwFlags) 
{
    CONTRACTL 
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (!m_fCheckedPerm) 
    {
        DWORD dwPermType = dwFlags & REFSEC_CHECK_MEMBERACCESS ? REFLECTION_MEMBER_ACCESS : REFLECTION_TYPE_INFO;
        
        EX_TRY 
        {
            Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, dwPermType);
            
            m_fCallerHasPerm = true;
        } 
        EX_CATCH 
        {
            m_fCallerHasPerm = false;
            
            if (dwFlags & REFSEC_THROW_MEMBERACCESS) 
            {
                COMPlusThrow(kMethodAccessException, L"Arg_MethodAccessException");
            }
            else if (dwFlags & REFSEC_THROW_FIELDACCESS) 
            {
                COMPlusThrow(kFieldAccessException, L"Arg_FieldAccessException");
            }
            else if (dwFlags & REFSEC_THROW_SECURITY) 
            {
                EX_RETHROW;
            }
        } 
        EX_END_CATCH(SwallowAllExceptions)

        m_fCheckedPerm = true;
    }

    return m_fCallerHasPerm;
}

// Check accessability of a field or method.
bool InvokeUtil::CheckAccess(RefSecContext *pCtx, DWORD dwAttr, MethodTable *pTargetMT, MethodTable* pInstanceMT, MethodDesc* pTargetMD, DWORD dwFlags, FieldDesc* pOptionalTargetField) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

   if (IsTdPublic(pTargetMT->GetClass()->GetProtection())    // If parent class is public (and not nested because it's not IsTdNestedPublic)
        && !pTargetMT->GetClass()->HasInstantiation()       // and has no instantiation
        && (IsMdPublic(dwAttr)                              // and the method itself is public but has no instantiation
            && (pTargetMD!=NULL) 
            && !pTargetMD->HasMethodInstantiation())  
      )
        return TRUE;                                        // then the caller has access to the method
        
    MethodTable *pCallerMT = pCtx->GetCallerMT();

    // Always allow interop callers full access.
    if (!pCallerMT)
        return true;

    // Global methods are not visible outside the module they are defined in
    if ((!pTargetMT->IsGlobalClass()) ||
        (pTargetMT->GetModule() == pCallerMT->GetModule()))
    {
        BOOL canAccess;
        if (pInstanceMT) 
            canAccess = ClassLoader::CanAccess(pCtx->GetCallerMethod(), pCallerMT,
                                               pCallerMT->GetAssembly(),
                                               pTargetMT,
                                               pTargetMT->GetAssembly(),
                                               pInstanceMT,
                                               dwAttr,
                                               pTargetMD, pOptionalTargetField);
        else
            canAccess = ClassLoader::CanAccess(pCtx->GetCallerMethod(), pCallerMT,
                                               pCallerMT->GetAssembly(),
                                               pTargetMT,
                                               pTargetMT->GetAssembly(),
                                               dwAttr,
                                               pTargetMD, pOptionalTargetField);
        if (canAccess) 
            return true;
    }

    return pCtx->DemandMemberAccess(dwFlags);
}

// Check accessability of a type or nested type.
bool InvokeUtil::CheckAccessType(RefSecContext *pCtx, MethodTable *pClassMT, DWORD dwFlags) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    MethodTable *pCallerMT = pCtx->GetCallerMT();

    // Always allow interop callers full access.
    if (!pCallerMT)
        return true;

    if (ClassLoader::CanAccessClass(pCtx->GetCallerMethod(), pCallerMT,
                                    pCallerMT->GetAssembly(),
                                    pClassMT,
                                    pClassMT->GetAssembly()))
        return true;

    return pCtx->DemandMemberAccess(dwFlags);
}

// If a method has a linktime demand attached, perform it.
bool InvokeUtil::CheckLinktimeDemand(RefSecContext *pCtx, MethodDesc *pCalleeMD, bool fThrowOnError) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END    
        
    if (pCalleeMD->RequiresLinktimeCheck() && pCtx->GetCallerMethod()) 
   {
           MethodDesc* pCallerMD = pCtx->GetCallerMethod();
        OBJECTREF refThrowable = NULL;
        bool fOK = true;
        GCPROTECT_BEGIN(refThrowable);
        if (!Security::LinktimeCheckMethod(pCallerMD->GetAssembly(), pCalleeMD, &refThrowable))
            fOK = false;
        if (!fOK && fThrowOnError)
            COMPlusThrow(refThrowable);
        GCPROTECT_END();
    
      // perform transparency checks as well
     if (Security::RequiresTransparentAssemblyChecks(pCallerMD, pCalleeMD))
     {
         Security::EnforceTransparentAssemblyChecks(pCallerMD, pCalleeMD);
     }
        return fOK;
    }
    return true;
}


