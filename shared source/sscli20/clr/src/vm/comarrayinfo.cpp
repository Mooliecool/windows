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
// This file contains the native methods that support the ArrayInfo class
////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "comarrayinfo.h"
#include "excep.h"
#include "field.h"
#include "remoting.h"
#include "security.h"
#include "invokeutil.h"

// Check we're allowed to create an array with the given element type.
void CheckElementType(TypeHandle elementType)
{
    // Check for simple types first.
    if (elementType.IsUnsharedMT())
    {
        MethodTable *pMT = elementType.AsMethodTable();

        // Check for TypedReference, ArgIterator and RuntimeArgument.
        if (pMT->ContainsStackPtr())
            COMPlusThrow(kNotSupportedException, L"NotSupported_ContainsStackPtr[]");

        // Check for open generic types.
        if (pMT->IsGenericTypeDefinition() || pMT->ContainsGenericVariables())
            COMPlusThrow(kNotSupportedException, L"NotSupported_OpenType");

        // Check for Void.
        if (elementType.GetSignatureCorElementType() == ELEMENT_TYPE_VOID)
            COMPlusThrow(kNotSupportedException, L"NotSupported_Void[]");

        // That's all the dangerous simple types we know, it must be OK.
        return;
    }

    // Checks apply recursively for arrays of arrays etc.
    if (elementType.IsArray())
    {
        CheckElementType(elementType.GetElementType());
        return;
    }

    // ByRefs and generic type variables are never allowed.
    if (elementType.IsByRef() || elementType.IsGenericVariable())
        COMPlusThrow(kNotSupportedException, L"NotSupported_ContainsStackPtr[]");

    // We can create pointers and function pointers, but it requires skip verification permission.
    CorElementType etType = elementType.GetSignatureCorElementType();
    if (etType == ELEMENT_TYPE_PTR || etType == ELEMENT_TYPE_FNPTR)
    {
        Security::SpecialDemand(SSWT_LATEBOUND_LINKDEMAND, SECURITY_SKIP_VER);
        return;
    }

    // We shouldn't get here (it means we've encountered a new type of typehandle if we do).
    _ASSERTE(!"Shouldn't get here, unknown type handle type");
    COMPlusThrow(kNotSupportedException, L"NotSupported_ContainsStackPtr[]");
}

FCIMPL4(Object*, COMArrayInfo::CreateInstance, void* elementTypeHandle, INT32 rank, INT32* pLengths, INT32* pLowerBounds)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);  // reenable once GC_TRIGGERS allowed in FCALLs
        PRECONDITION(rank > 0);
        PRECONDITION(CheckPointer(pLengths));
        PRECONDITION(CheckPointer(pLowerBounds, NULL_OK));
    } CONTRACTL_END;

    OBJECTREF pRet = NULL;
    TypeHandle elementType = TypeHandle::FromPtr(elementTypeHandle);

    _ASSERTE(!elementType.IsNull());

    // pLengths and pLowerBounds are pinned buffers. No need to protect them.
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);

    CheckElementType(elementType);

    CorElementType CorType = elementType.GetSignatureCorElementType();

    CorElementType kind = ELEMENT_TYPE_ARRAY;

    // Is it ELEMENT_TYPE_SZARRAY array?
    if (rank == 1 && (pLowerBounds == NULL || pLowerBounds[0] == 0))
    {
        // Shortcut for common cases
        if (CorTypeInfo::IsPrimitiveType(CorType))
        {
            pRet = AllocatePrimitiveArray(CorType,pLengths[0]);
            goto Done;
        }
        else
        if (CorTypeInfo::IsObjRef(CorType))
        {
            pRet = AllocateObjectArray(pLengths[0],elementType);
            goto Done;
        }

        kind = ELEMENT_TYPE_SZARRAY;
        pLowerBounds = NULL;
    }

    {
        // Find the Array class...
        TypeHandle typeHnd = ClassLoader::LoadArrayTypeThrowing(elementType, kind, rank);

        DWORD  boundsSize;
        INT32* bounds;
        if (pLowerBounds != NULL) {
            boundsSize = rank*2;
            bounds = (INT32*) _alloca(boundsSize * sizeof(INT32));

            for (int i=0;i<rank;i++) {
                bounds[2*i] = pLowerBounds[i];
                bounds[2*i+1] = pLengths[i];
            }
        }
        else {
            boundsSize = rank;
            bounds = (INT32*) _alloca(boundsSize * sizeof(INT32));

            // We need to create a private copy of pLengths to avoid holes caused
            // by caller mutating the array
            for (int i=0;i<rank;i++)
                bounds[i] = pLengths[i];
        }

        pRet = AllocateArrayEx(typeHnd, bounds, boundsSize);
    }

Done: ;
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(pRet);
}
FCIMPLEND


FCIMPL4(void, COMArrayInfo::GetReference, ArrayBase* refThisUNSAFE, TypedByRef* elemRef, INT32 rank, INT32* pIndices)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);  // reenable once GC_TRIGGERS allowed in FCALLs
    } CONTRACTL_END;

    // FC_GC_POLL not necessary. We poll for GC in Array.Rank that's always called
    // right before this function
    FC_GC_POLL_NOT_NEEDED();

    BASEARRAYREF    refThis  = (BASEARRAYREF) refThisUNSAFE;

    _ASSERTE(rank == (INT32)refThis->GetRank());

    SIZE_T Offset               = 0;
    const INT32 *pBoundsPtr     = refThis->GetBoundsPtr();

    if (rank == 1)
    {
        Offset = pIndices[0] - refThis->GetLowerBoundsPtr()[0];

        // Bounds check each index
        // Casting to unsigned allows us to use one compare for [0..limit-1]
        if (((UINT32) Offset) >= ((UINT32) pBoundsPtr[0]))
            FCThrowVoid(kIndexOutOfRangeException);
    }
    else
    {
        // Avoid redundant computation in GetLowerBoundsPtr
        const INT32 *pLowerBoundsPtr = pBoundsPtr + rank;
        _ASSERTE(refThis->GetLowerBoundsPtr() == pLowerBoundsPtr);

        SIZE_T Multiplier = 1;

        for (int i = rank-1; i >= 0; i--) {
            INT32 curIndex = pIndices[i] - pLowerBoundsPtr[i];

            // Bounds check each index
            // Casting to unsigned allows us to use one compare for [0..limit-1]
            if (((UINT32) curIndex) >= ((UINT32) pBoundsPtr[i]))
                FCThrowVoid(kIndexOutOfRangeException);

            Offset += curIndex * Multiplier;
            Multiplier *= pBoundsPtr[i];
        }
    }

    TypeHandle arrayElementType = refThis->GetArrayElementTypeHandle();

    // Legacy behavior
    if (arrayElementType.IsTypeDesc())
    {
        CorElementType elemtype = arrayElementType.AsTypeDesc()->GetInternalCorElementType();
        if (elemtype == ELEMENT_TYPE_PTR || elemtype == ELEMENT_TYPE_FNPTR)
            FCThrowResVoid(kNotSupportedException, L"NotSupported_Type");
    }
#ifdef _DEBUG
    CorElementType elemtype = arrayElementType.GetInternalCorElementType();
    _ASSERTE(elemtype != ELEMENT_TYPE_PTR && elemtype != ELEMENT_TYPE_FNPTR);
#endif

    elemRef->data = refThis->GetDataPtr() + (Offset * refThis->GetComponentSize());
    elemRef->type = arrayElementType;
}
FCIMPLEND

FCIMPL2(void, COMArrayInfo::SetValue, TypedByRef * target, Object* objUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    OBJECTREF obj = ObjectToOBJECTREF(objUNSAFE);

    TypeHandle thTarget(target->type);

    MethodTable* pTargetMT = thTarget.GetMethodTable();
    PREFIX_ASSUME(NULL != pTargetMT);

    if (obj == NULL)
    {
        // Null is the universal zero...
        if (pTargetMT->IsValueType())
            InitValueClass(target->data,pTargetMT);
        else
            ClearObjectReference((OBJECTREF*)target->data);
    }
    else
    if (thTarget == TypeHandle(g_pObjectClass))
    {
        // Everything is compatible with Object
        SetObjectReference((OBJECTREF*)target->data,(OBJECTREF)obj,GetAppDomain());
    }
    else
    if (!pTargetMT->IsValueType())
    {
        if (ObjIsInstanceOfNoGC(OBJECTREFToObject(obj), thTarget) != TypeHandle::CanCast)
        {
            // target->data is protected by the caller
            HELPER_METHOD_FRAME_BEGIN_1(obj);

            if (!ObjIsInstanceOf(OBJECTREFToObject(obj), thTarget))
                COMPlusThrow(kInvalidCastException,L"InvalidCast_StoreArrayElement");

            HELPER_METHOD_FRAME_END();
        }

        SetObjectReference((OBJECTREF*)target->data,obj,GetAppDomain());
    }
    else
    {
        // value class or primitive type

        if (!pTargetMT->UnBoxInto(target->data, obj))
        {
            // target->data is protected by the caller
            HELPER_METHOD_FRAME_BEGIN_1(obj);

            ARG_SLOT value = 0;

            // Allow enum -> primitive conversion, disallow primitive -> enum conversion
            TypeHandle thSrc = obj->GetTypeHandle();
            CorElementType srcType = thSrc.GetVerifierCorElementType();
            CorElementType targetType = thTarget.GetSignatureCorElementType();

            if (!InvokeUtil::IsPrimitiveType(srcType) || !InvokeUtil::IsPrimitiveType(targetType))
                COMPlusThrow(kInvalidCastException, L"InvalidCast_StoreArrayElement");

            // Get a properly widened type
            InvokeUtil::CreatePrimitiveValue(targetType,srcType,obj,&value);

            UINT cbSize = CorTypeInfo::Size(targetType);
            memcpyNoGCRefs(target->data, ArgSlotEndianessFixup(&value, cbSize), cbSize);

            HELPER_METHOD_FRAME_END();
        }
    }
}
FCIMPLEND

// This method will initialize an array from a TypeHandle to a field.

FCIMPL2(void, COMArrayInfo::InitializeArray, ArrayBase* pArrayRef, HANDLE handle)

    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);  // reenable once GC_TRIGGERS allowed in FCALLs
    } CONTRACTL_END;

    BASEARRAYREF arr = BASEARRAYREF(pArrayRef);
    HELPER_METHOD_FRAME_BEGIN_1(arr);

    if (arr == 0)
        COMPlusThrow(kArgumentNullException);

    FieldDesc* pField = (FieldDesc*) handle;

    if (pField == NULL)
        COMPlusThrow(kArgumentNullException);

    if (pField->IsRVA() == FALSE)
        COMPlusThrow(kArgumentException);

    {  
        // We want to identify hot RVA blobs. To do that we need a place to put an IBC probe.
        // But array initialization is done by a jit intrinsic; there is no code in the EE that gets 
        // executed. So there is no nice place to place a probe. So whats done is, in an instrumented 
        // ngen image, this intrinsic is turned off (see CEEInfo::getArrayInitializationData). Thus we 
        // force COMArrayInfo::InitializeArray to be called during an IBC run, and this gives us a 
        // place to probe. Now the problem is, this function calls lot of EE code which
        // causes lot of IBC probes to execute. As a result, the EEClass/MethodTable/FieldDesc of 
        // the RVA field get reported as "hot", while it is only the blob that is really hot. So we turn
        // off IBC logging in this function, and before returning, turn logging back on and report
        // just the rva blob address to the logger.
        IBCLoggingDisabler ibcld;
        
        // Note that we do not check that the field is actually in the PE file that is initializing
        // the array. Basically the data being published is can be accessed by anyone with the proper
        // permissions (C# marks these as assembly visibility, and thus are protected from outside
        // snooping)

        CorElementType type = arr->GetArrayElementType();
        if (!CorTypeInfo::IsPrimitiveType(type))
            COMPlusThrow(kArgumentException);

        DWORD dwCompSize = arr->GetComponentSize();
        DWORD dwElemCnt = arr->GetNumComponents();
        DWORD dwTotalSize = dwCompSize * dwElemCnt;

        DWORD size = 0;


        // Restore the declaring class in case it's instantiated
        pField->GetApproxEnclosingMethodTable()->CheckRestore();  // <BUG> this probably should be Approx - see bug 184355 </BUG>
        size = pField->LoadSize();

        // make certain you don't go off the end of the rva static
        if (dwTotalSize > size)
            COMPlusThrow(kArgumentException);

        void *src = pField->GetStaticAddressHandle(NULL);
        void *dest = arr->GetDataPtr();

#if BIGENDIAN
        DWORD i;
        switch (dwCompSize) {
        case 1:
            memcpyNoGCRefs(dest, src, dwElemCnt);
            break;
        case 2:
            for (i = 0; i < dwElemCnt; i++)
                *((UINT16*)dest + i) = GET_UNALIGNED_VAL16((UINT16*)src + i);
            break;
        case 4:
            for (i = 0; i < dwElemCnt; i++)
                *((UINT32*)dest + i) = GET_UNALIGNED_VAL32((UINT32*)src + i);
            break;
        case 8:
            for (i = 0; i < dwElemCnt; i++)
                *((UINT64*)dest + i) = GET_UNALIGNED_VAL64((UINT64*)src + i);
            break;
        default:
            // should not reach here.
            UNREACHABLE_MSG("Incorrect primitive type size!");
            break;
        }
#else
        memcpyNoGCRefs(dest, src, dwTotalSize);
#endif
    }
    // Report the RVA field to the logger.
    g_IBCLogger.LogRVADataAccess(pField);

    HELPER_METHOD_FRAME_END();

FCIMPLEND
