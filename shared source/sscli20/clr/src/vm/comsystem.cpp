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
/*============================================================
**
** Header: COMSystem.cpp
**
**                                             
**
** Purpose: Native methods on System.Environment & Array
**
** Date:  March 30, 1998
**
===========================================================*/
#include "common.h"

#include <object.h>

#include "ceeload.h"

#include "excep.h"
#include "frames.h"
#include "vars.hpp"
#include "classnames.h"
#include "comsystem.h"
#include "comstring.h"
#include "comvariant.h"    // for Element type to class lookup table.
#include "eeconfig.h"
#include "assemblynative.hpp"
#include "generics.h"
#include "invokeutil.h"
#include "array.h"
#include "eepolicy.h"

// The exit code for the process is communicated in one of two ways.  If the
// entrypoint returns an 'int' we take that.  Otherwise we take a latched
// process exit code.  This can be modified by the app via setting
// Environment's ExitCode property.
INT32 SystemNative::LatchedExitCode;

// Returns an enum saying whether you can copy an array of srcType into destType.
AssignArrayEnum SystemNative::CanAssignArrayTypeNoGC(const BASEARRAYREF pSrc, const BASEARRAYREF pDest)
{
     CONTRACTL
     {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(pSrc != NULL);
        PRECONDITION(pDest != NULL);
     }
     CONTRACTL_END;

    // The next 50 lines are a little tricky.  Change them with great care.

    // This first bit is a minor optimization: e.g. when copying byte[] to byte[]
    // we do not need to call GetArrayElementTypeHandle(), which touches the corresponding
    // ArrayClass.
    MethodTable *pSrcMT = pSrc->GetMethodTable();
    MethodTable *pDestMT = pDest->GetMethodTable();
    if (pSrcMT == pDestMT &&
        !pSrcMT->IsSharedByReferenceArrayTypes())
        return AssignWillWork;

    TypeHandle srcTH = pSrc->GetArrayElementTypeHandle();
    TypeHandle destTH = pDest->GetArrayElementTypeHandle();


    if (srcTH == destTH)
        return AssignWillWork;
    
    // Value class boxing
    if (srcTH.IsValueType() && !destTH.IsValueType())
    {
        switch (srcTH.CanCastToNoGC(destTH))
        {
        case TypeHandle::CanCast : return AssignBoxValueClassOrPrimitive;
        case TypeHandle::CannotCast : return AssignWrongType;
        default : return AssignDontKnow;
        }
    }

    // Value class unboxing.
    if (!srcTH.IsValueType() && destTH.IsValueType())
    {
        if (srcTH.CanCastToNoGC(destTH) == TypeHandle::CanCast)
            return AssignUnboxValueClassAndCast;
        else if (destTH.CanCastToNoGC(srcTH) == TypeHandle::CanCast)   // V extends IV. Copying from IV to V, or Object to V.
            return AssignUnboxValueClassAndCast;
        else
            return AssignDontKnow;
    }
    
    const CorElementType srcElType = srcTH.GetSignatureCorElementType();
    const CorElementType destElType = destTH.GetSignatureCorElementType();
    _ASSERTE(srcElType < ELEMENT_TYPE_MAX);
    _ASSERTE(destElType < ELEMENT_TYPE_MAX);

    // Copying primitives from one type to another
    if (CorTypeInfo::IsPrimitiveType(srcElType) && CorTypeInfo::IsPrimitiveType(destElType))
    {
        if (InvokeUtil::CanPrimitiveWiden(destElType, srcElType))
            return AssignPrimitiveWiden;
        else
            return AssignWrongType;
    }
    
    // dest Object extends src
    if (srcTH.CanCastToNoGC(destTH) == TypeHandle::CanCast)
        return AssignWillWork;
    
    // src Object extends dest
    if (destTH.CanCastToNoGC(srcTH) == TypeHandle::CanCast)
        return AssignMustCast;
    
    // class X extends/implements src and implements dest.
    if (destTH.IsInterface() && srcElType != ELEMENT_TYPE_VALUETYPE)
        return AssignMustCast;
    
    // class X implements src and extends/implements dest
    if (srcTH.IsInterface() && destElType != ELEMENT_TYPE_VALUETYPE)
        return AssignMustCast;

    // Enum is stored as a primitive of type dest.
    if (srcTH.IsEnum() && srcTH.GetInternalCorElementType() == destElType)
        return AssignWillWork;
    
    return AssignDontKnow;
}


// Returns an enum saying whether you can copy an array of srcType into destType.
AssignArrayEnum SystemNative::CanAssignArrayType(const BASEARRAYREF pSrc, const BASEARRAYREF pDest)
{
     CONTRACTL
     {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(pSrc != NULL);
        PRECONDITION(pDest != NULL);
     }
     CONTRACTL_END;

    // The next 50 lines are a little tricky.  Change them with great care.

    // This first bit is a minor optimization: e.g. when copying byte[] to byte[]
    // we do not need to call GetArrayElementTypeHandle(), which touches the corresponding
    // ArrayClass.
    MethodTable *pSrcMT = pSrc->GetMethodTable();
    MethodTable *pDestMT = pDest->GetMethodTable();
    if (pSrcMT == pDestMT &&
        !pSrcMT->IsSharedByReferenceArrayTypes())
        return AssignWillWork;

    TypeHandle srcTH = pSrc->GetArrayElementTypeHandle();
    TypeHandle destTH = pDest->GetArrayElementTypeHandle();


    if (srcTH == destTH)
        return AssignWillWork;
    
    // Value class boxing
    if (srcTH.IsValueType() && !destTH.IsValueType())
    {
        if (srcTH.CanCastTo(destTH))
            return AssignBoxValueClassOrPrimitive;
        else 
            return AssignWrongType;
    }

    // Value class unboxing.
    if (!srcTH.IsValueType() && destTH.IsValueType())
    {
        if (srcTH.CanCastTo(destTH))
            return AssignUnboxValueClassAndCast;
        else if (destTH.CanCastTo(srcTH))   // V extends IV. Copying from IV to V, or Object to V.
            return AssignUnboxValueClassAndCast;
        else
            return AssignWrongType;
    }
    
    const CorElementType srcElType = srcTH.GetSignatureCorElementType();
    const CorElementType destElType = destTH.GetSignatureCorElementType();
    _ASSERTE(srcElType < ELEMENT_TYPE_MAX);
    _ASSERTE(destElType < ELEMENT_TYPE_MAX);

    // Copying primitives from one type to another
    if (CorTypeInfo::IsPrimitiveType(srcElType) && CorTypeInfo::IsPrimitiveType(destElType))
    {
        if (InvokeUtil::CanPrimitiveWiden(destElType, srcElType))
            return AssignPrimitiveWiden;
        else
            return AssignWrongType;
    }
    
    // dest Object extends src
    if (srcTH.CanCastTo(destTH))
        return AssignWillWork;
    
    // src Object extends dest
    if (destTH.CanCastTo(srcTH))
        return AssignMustCast;
    
    // class X extends/implements src and implements dest.
    if (destTH.IsInterface() && srcElType != ELEMENT_TYPE_VALUETYPE)
        return AssignMustCast;
    
    // class X implements src and extends/implements dest
    if (srcTH.IsInterface() && destElType != ELEMENT_TYPE_VALUETYPE)
        return AssignMustCast;

    // Enum is stored as a primitive of type dest.
    if (srcTH.IsEnum() && srcTH.GetInternalCorElementType() == destElType)
        return AssignWillWork;
    
    return AssignWrongType;
}

// Casts and assigns each element of src array to the dest array type.
void SystemNative::CastCheckEachElement(const BASEARRAYREF pSrcUnsafe, const unsigned int srcIndex, BASEARRAYREF pDestUnsafe, unsigned int destIndex, const unsigned int len)
{
     CONTRACTL
     {
         THROWS;
         GC_TRIGGERS;
         MODE_COOPERATIVE;
         PRECONDITION(pSrcUnsafe != NULL);
         PRECONDITION(srcIndex >= 0);
         PRECONDITION(pDestUnsafe != NULL);
         PRECONDITION(len >= 0);
     }
     CONTRACTL_END;

    // pSrc is either a PTRARRAYREF or a multidimensional array.
    TypeHandle destTH = pDestUnsafe->GetArrayElementTypeHandle();
    MethodTable * pDestMT = destTH.GetMethodTable();
    _ASSERTE(pDestMT);
    
    // Cache last cast test to speed up cast checks.
    MethodTable * pLastMT = NULL;

    const BOOL destIsArray = destTH.IsArray();

    struct _gc
    {
        OBJECTREF obj;
        BASEARRAYREF pDest;
        BASEARRAYREF pSrc;
    } prot;
    
    prot.obj = NULL;
    prot.pDest = pDestUnsafe;
    prot.pSrc = pSrcUnsafe;

    GCPROTECT_BEGIN(prot);
    
    for(unsigned int i=srcIndex; i<srcIndex + len; ++i)
    {
        MethodTable * pMT = NULL;
        prot.obj = ObjectToOBJECTREF(*((Object**) prot.pSrc->GetDataPtr() + i));

        // Now that we have grabbed obj, we are no longer subject to races from another
        // mutator thread.
        if (!prot.obj)
            goto assign;

        pMT = prot.obj->GetTrueMethodTable();
        if (pMT == pLastMT || pMT == pDestMT)
            goto assign;

        pLastMT = pMT;
        // Handle whether these are interfaces or not.
        if (pDestMT->IsInterface())
        {
            // Check for o implementing dest.
            if (pMT->CanCastToInterface(pDestMT))
                goto assign;


            if (pMT->IsArray())
            {
                if (ArraySupportsBizarreInterface(prot.obj, pDestMT))
                    goto assign;
            }

            COMPlusThrow(kInvalidCastException, L"InvalidCast_DownCastArrayElement");
        }
        else if (destIsArray)
        {
            TypeHandle srcTH = prot.obj->GetTypeHandle();
            if (!srcTH.CanCastTo(destTH))
                COMPlusThrow(kInvalidCastException, L"InvalidCast_DownCastArrayElement");
        } 
        else
        {
            if (!pMT->CanCastToClass(pDestMT))
                COMPlusThrow(kInvalidCastException, L"InvalidCast_DownCastArrayElement");
        }
assign:
        // It is safe to assign obj
        OBJECTREF * destData = (OBJECTREF*)(prot.pDest->GetDataPtr()) + i - srcIndex + destIndex;
        SetObjectReference(destData, prot.obj, prot.pDest->GetAppDomain());
    }

    GCPROTECT_END();

    return;
}


// Will box each element in an array of value classes or primitives into an array of Objects.
void __stdcall SystemNative::BoxEachElement(BASEARRAYREF pSrc, unsigned int srcIndex, BASEARRAYREF pDest, unsigned int destIndex, unsigned int length)
{
    CONTRACTL
    {
         THROWS;
         GC_TRIGGERS;
         MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    // pDest is either a PTRARRAYREF or a multidimensional array.
    _ASSERTE(pSrc!=NULL && srcIndex>=0 && pDest!=NULL && destIndex>=0 && length>=0);
    TypeHandle srcTH = pSrc->GetArrayElementTypeHandle();
#ifdef _DEBUG
    TypeHandle destTH = pDest->GetArrayElementTypeHandle();
#endif
    _ASSERTE(srcTH.GetSignatureCorElementType() == ELEMENT_TYPE_CLASS || srcTH.GetSignatureCorElementType() == ELEMENT_TYPE_VALUETYPE || CorTypeInfo::IsPrimitiveType(pSrc->GetArrayElementType()));
    _ASSERTE(!destTH.GetClass()->IsValueClass());

    // Get method table of type we're copying from - we need to allocate objects of that type.
    MethodTable * pSrcMT = srcTH.GetMethodTable();
    PREFIX_ASSUME(pSrcMT != NULL);
    
    if (!pSrcMT->IsClassInited())
    {
        BASEARRAYREF pSrcTmp = pSrc;
        BASEARRAYREF pDestTmp = pDest;
        GCPROTECT_BEGIN (pSrcTmp);
        GCPROTECT_BEGIN (pDestTmp);
        pSrcMT->CheckRunClassInitThrowing();
        pSrc = pSrcTmp;
        pDest = pDestTmp;
        GCPROTECT_END ();
        GCPROTECT_END ();
    }

    const unsigned int srcSize = pSrcMT->GetNumInstanceFieldBytes();
    unsigned int srcArrayOffset = srcIndex * srcSize;

    struct _gc
    {
        BASEARRAYREF src;
        BASEARRAYREF dest;
        OBJECTREF obj;
    }  prot;
    
    prot.src = pSrc;
    prot.dest = pDest;
    prot.obj = NULL;

    void* srcPtr = 0;
    GCPROTECT_BEGIN(prot);
    GCPROTECT_BEGININTERIOR(srcPtr);
    for (unsigned int i=destIndex; i < destIndex+length; i++, srcArrayOffset += srcSize)
    {
        srcPtr = (BYTE*)prot.src->GetDataPtr() + srcArrayOffset;
        prot.obj = pSrcMT->FastBox(&srcPtr);

        OBJECTREF * destData = (OBJECTREF*)((prot.dest)->GetDataPtr()) + i;
        SetObjectReference(destData, prot.obj, prot.dest->GetAppDomain());
    }
    GCPROTECT_END();
    GCPROTECT_END();
}


// Unboxes from an Object[] into a value class or primitive array.
void __stdcall SystemNative::UnBoxEachElement(BASEARRAYREF pSrc, unsigned int srcIndex, BASEARRAYREF pDest, unsigned int destIndex, unsigned int length, BOOL castEachElement)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(pSrc != NULL);
        PRECONDITION(srcIndex >= 0);
        PRECONDITION(pDest != NULL);
        PRECONDITION(destIndex >= 0);
        PRECONDITION(length >= 0);
    }
    CONTRACTL_END;

#ifdef _DEBUG
    TypeHandle srcTH = pSrc->GetArrayElementTypeHandle();
#endif
    TypeHandle destTH = pDest->GetArrayElementTypeHandle();
    _ASSERTE(destTH.GetSignatureCorElementType() == ELEMENT_TYPE_CLASS || destTH.GetSignatureCorElementType() == ELEMENT_TYPE_VALUETYPE || CorTypeInfo::IsPrimitiveType(pDest->GetArrayElementType()));
    _ASSERTE(!srcTH.GetClass()->IsValueClass());

    MethodTable * pDestMT = destTH.GetMethodTable();
    PREFIX_ASSUME(pDestMT != NULL);

    const unsigned int destSize = pDestMT->GetNumInstanceFieldBytes();
    BYTE* srcData = (BYTE*) pSrc->GetDataPtr() + srcIndex * sizeof(OBJECTREF);
    BYTE* data = (BYTE*) pDest->GetDataPtr() + destIndex * destSize;

    for(; length>0; length--, srcData += sizeof(OBJECTREF), data += destSize)
    {
        OBJECTREF obj = ObjectToOBJECTREF(*(Object**)srcData);
        
        // Now that we have retrieved the element, we are no longer subject to race
        // conditions from another array mutator.

        if (!pDestMT->UnBoxInto(data, obj)) 
            goto fail;
    }
    return;

fail:
    COMPlusThrow(kInvalidCastException, L"InvalidCast_DownCastArrayElement");
}


// Widen primitive types to another primitive type.
void __stdcall SystemNative::PrimitiveWiden(BASEARRAYREF pSrc, unsigned int srcIndex, BASEARRAYREF pDest, unsigned int destIndex, unsigned int length)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(pSrc != NULL);
        PRECONDITION(pDest != NULL);
    }
    CONTRACTL_END;

    // Get appropriate sizes, which requires method tables.
    TypeHandle srcTH = pSrc->GetArrayElementTypeHandle();
    TypeHandle destTH = pDest->GetArrayElementTypeHandle();

    const CorElementType srcElType = srcTH.GetSignatureCorElementType();
    const CorElementType destElType = destTH.GetSignatureCorElementType();
    const unsigned int srcSize = GetSizeForCorElementType(srcElType);
    const unsigned int destSize = GetSizeForCorElementType(destElType);

    BYTE* srcData = (BYTE*) pSrc->GetDataPtr() + srcIndex * srcSize;
    BYTE* data = (BYTE*) pDest->GetDataPtr() + destIndex * destSize;

    _ASSERTE(srcElType != destElType);  // We shouldn't be here if these are the same type.
    _ASSERTE(CorTypeInfo::IsPrimitiveType(srcElType) && CorTypeInfo::IsPrimitiveType(destElType));

    for(; length>0; length--, srcData += srcSize, data += destSize)
    {
        // We pretty much have to do some fancy datatype mangling every time here, for
        // converting w/ sign extension and floating point conversions.
        switch (srcElType)
        {
            case ELEMENT_TYPE_U1:
                if (destElType==ELEMENT_TYPE_R4)
                    *(float*)data = *(UINT8*)srcData;
                else if (destElType==ELEMENT_TYPE_R8)
                    *(double*)data = *(UINT8*)srcData;
                else
                {
                    *(UINT8*)data = *(UINT8*)srcData;
                    memset(data+1, 0, destSize - 1);
                }
                break;


            case ELEMENT_TYPE_I1:
                switch (destElType)
                {
                    case ELEMENT_TYPE_I2:
                        *(INT16*)data = *(INT8*)srcData;
                        break;

                    case ELEMENT_TYPE_I4:
                        *(INT32*)data = *(INT8*)srcData;
                        break;

                    case ELEMENT_TYPE_I8:
                        *(INT64*)data = *(INT8*)srcData;
                        break;

                    case ELEMENT_TYPE_R4:
                        *(float*)data = *(INT8*)srcData;
                        break;

                    case ELEMENT_TYPE_R8:
                        *(double*)data = *(INT8*)srcData;
                        break;

                    default:
                        _ASSERTE(!"Array.Copy from I1 to another type hit unsupported widening conversion");
                }
                break;          


            case ELEMENT_TYPE_U2:
            case ELEMENT_TYPE_CHAR:
                if (destElType==ELEMENT_TYPE_R4)
                    *(float*)data = *(UINT16*)srcData;
                else if (destElType==ELEMENT_TYPE_R8)
                    *(double*)data = *(UINT16*)srcData;
                else
                {
                    *(UINT16*)data = *(UINT16*)srcData;
                    memset(data+2, 0, destSize - 2);
                }
                break;


            case ELEMENT_TYPE_I2:
                switch (destElType)
                {
                    case ELEMENT_TYPE_I4:
                        *(INT32*)data = *(INT16*)srcData;
                        break;

                    case ELEMENT_TYPE_I8:
                        *(INT64*)data = *(INT16*)srcData;
                        break;

                    case ELEMENT_TYPE_R4:
                        *(float*)data = *(INT16*)srcData;
                        break;

                    case ELEMENT_TYPE_R8:
                        *(double*)data = *(INT16*)srcData;
                        break;

                    default:
                        _ASSERTE(!"Array.Copy from I2 to another type hit unsupported widening conversion");
                }
                break;


            case ELEMENT_TYPE_I4:
                switch (destElType)
                {
                    case ELEMENT_TYPE_I8:
                        *(INT64*)data = *(INT32*)srcData;
                        break;

                    case ELEMENT_TYPE_R4:
                        *(float*)data = (float)*(INT32*)srcData;
                        break;

                    case ELEMENT_TYPE_R8:
                        *(double*)data = *(INT32*)srcData;
                        break;

                    default:
                        _ASSERTE(!"Array.Copy from I4 to another type hit unsupported widening conversion");
                }
                break;
        

            case ELEMENT_TYPE_U4:
                switch (destElType)
                {
                    case ELEMENT_TYPE_I8:
                    case ELEMENT_TYPE_U8:
                        *(INT64*)data = *(UINT32*)srcData;
                        break;

                    case ELEMENT_TYPE_R4:
                        *(float*)data = (float)*(UINT32*)srcData;
                        break;

                    case ELEMENT_TYPE_R8:
                        *(double*)data = *(UINT32*)srcData;
                        break;

                    default:
                        _ASSERTE(!"Array.Copy from U4 to another type hit unsupported widening conversion");
                }
                break;


            case ELEMENT_TYPE_I8:
                if (destElType == ELEMENT_TYPE_R4)
                {
                    *(float*) data = (float) *(INT64*)srcData;
                }
                else
                {
                    _ASSERTE(destElType==ELEMENT_TYPE_R8);
                    *(double*) data = (double) *(INT64*)srcData;
                }
                break;
            

            case ELEMENT_TYPE_U8:
                if (destElType == ELEMENT_TYPE_R4)
                {
                    //*(float*) data = (float) *(UINT64*)srcData;
                    INT64 srcVal = *(INT64*)srcData;
                    float f = (float) srcVal;
                    if (srcVal < 0)
                        f += 4294967296.0f * 4294967296.0f; // This is 2^64
                        
                    *(float*) data = f;
                }
                else
                {
                    _ASSERTE(destElType==ELEMENT_TYPE_R8);
                    //*(double*) data = (double) *(UINT64*)srcData;
                    INT64 srcVal = *(INT64*)srcData;
                    double d = (double) srcVal;
                    if (srcVal < 0)
                        d += 4294967296.0 * 4294967296.0;   // This is 2^64
                        
                    *(double*) data = d;
                }
                break;


            case ELEMENT_TYPE_R4:
                *(double*) data = *(float*)srcData;
                break;
            
            default:
                _ASSERTE(!"Fell through outer switch in PrimitiveWiden!  Unknown primitive type for source array!");
        }
    }
}

#if defined(_X86_)
//This is a replacement for the memmove intrinsic.
//It performs better than the CRT one and the inline version.
// On WIN64 the CRT implementation of memmove is actually faster than the CLR implementation of m_memmove().
void m_memmove(BYTE* dmem, BYTE* smem, int size)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(dmem));
        PRECONDITION(CheckPointer(smem));
        PRECONDITION(size >= 0);
        SO_TOLERANT;
    }
    CONTRACTL_END;

#if defined(_WIN64) || defined(ALIGN_ACCESS)
    // Bail out and use the slow version if the destination and the source don't have the same alignment.
    if ( ( ((SIZE_T)dmem) & (sizeof(SIZE_T) - 1) ) != 
         ( ((SIZE_T)smem) & (sizeof(SIZE_T) - 1) ) )
    {
        memmove(dmem, smem, size);
    }
    else
#endif // _WIN64 || ALIGN_ACCESS
    if (dmem <= smem)
    {
        // make sure the destination is pointer-aligned
        while (( ((SIZE_T)dmem) & (sizeof(SIZE_T) - 1) ) != 0 && size >= (int)(sizeof(SIZE_T) - 1))
        {
            *dmem++ = *smem++;
            size -= 1;
        }

        // copy 16 bytes at a time
        if (size >= 16)
        {
            size -= 16;
            do
            {
                ((DWORD *)dmem)[0] = ((DWORD *)smem)[0];
                ((DWORD *)dmem)[1] = ((DWORD *)smem)[1];
                ((DWORD *)dmem)[2] = ((DWORD *)smem)[2];
                ((DWORD *)dmem)[3] = ((DWORD *)smem)[3];
                dmem += 16;
                smem += 16;
            }
            while ((size -= 16) >= 0);
        }

#if defined(_WIN64) || defined(ALIGN_ACCESS)
        if (!IS_ALIGNED((SIZE_T)dmem, sizeof(SIZE_T)))
        {
            while (size > 0)
            {
                *dmem++ = *smem++;
                size -= 1;
            }
        }
        else
#endif // _WIN64 || ALIGN_ACCESS
        {
            // still 8 bytes or more left to copy?
            if (size & 8)
            {
                ((DWORD *)dmem)[0] = ((DWORD *)smem)[0];
                ((DWORD *)dmem)[1] = ((DWORD *)smem)[1];
                dmem += 8;
                smem += 8;
            }

            // still 4 bytes or more left to copy?
            if (size & 4)
            {
                ((DWORD *)dmem)[0] = ((DWORD *)smem)[0];
                dmem += 4;
                smem += 4;
            }

            // still 2 bytes or more left to copy?
            if (size & 2)
            {
                ((WORD *)dmem)[0] = ((WORD *)smem)[0];
                dmem += 2;
                smem += 2;
            }

            // still 1 byte left to copy?
            if (size & 1)
            {
                dmem[0] = smem[0];
                dmem += 1;
                smem += 1;
            }
        }
    }
    else
    {
        smem += size;
        dmem += size;

        // make sure the destination is pointer-aligned
        while (( ((SIZE_T)dmem) & (sizeof(SIZE_T) - 1) ) != 0 && size >= (int)(sizeof(SIZE_T) - 1))
        {
            *--dmem = *--smem;
            size -= 1;
        }

        // copy 16 bytes at a time
        if (size >= 16)
        {
            size -= 16;
            do
            {
                dmem -= 16;
                smem -= 16;
                ((DWORD *)dmem)[3] = ((DWORD *)smem)[3];
                ((DWORD *)dmem)[2] = ((DWORD *)smem)[2];
                ((DWORD *)dmem)[1] = ((DWORD *)smem)[1];
                ((DWORD *)dmem)[0] = ((DWORD *)smem)[0];
            }
            while ((size -= 16) >= 0);
        }

#if defined(_WIN64) || defined(ALIGN_ACCESS)
        if (!IS_ALIGNED((SIZE_T)dmem, sizeof(SIZE_T)))
        {
            while (size > 0)
            {
                *--dmem = *--smem;
                size -= 1;
            }
        }
        else
#endif // _WIN64 || ALIGN_ACCESS
        {
            // still 8 bytes or more left to copy?
            if (size & 8)
            {
                dmem -= 8;
                smem -= 8;
                ((DWORD *)dmem)[1] = ((DWORD *)smem)[1];
                ((DWORD *)dmem)[0] = ((DWORD *)smem)[0];
            }

            // still 4 bytes or more left to copy?
            if (size & 4)
            {
                dmem -= 4;
                smem -= 4;
                ((DWORD *)dmem)[0] = ((DWORD *)smem)[0];
            }

            // still 2 bytes or more left to copy?
            if (size & 2)
            {
                dmem -= 2;
                smem -= 2;
                ((WORD *)dmem)[0] = ((WORD *)smem)[0];
            }

            // still 1 byte left to copy?
            if (size & 1)
            {
                dmem -= 1;
                smem -= 1;
                dmem[0] = smem[0];
            }
        }
    }
}
#else
#define m_memmove(a, b, c) memmove((a), (b), (c))
#endif // _X86_


FCIMPL6(void, SystemNative::ArrayCopy, ArrayBase* m_pSrc, INT32 m_iSrcIndex, ArrayBase* m_pDst, INT32 m_iDstIndex, INT32 m_iLength, CLR_BOOL reliable)
{
    BYTE *src;
    BYTE *dst;
    int  size;
    
    struct _gc
    {
        BASEARRAYREF pSrc;
        BASEARRAYREF pDst;
    } gc;

    gc.pSrc = (BASEARRAYREF)m_pSrc;
    gc.pDst = (BASEARRAYREF)m_pDst;

    //
    // creating a HelperMethodFrame is quite expensive, 
    // so we want to delay this for the most common case which doesn't trigger a GC.
    // FCThrow is needed to throw an exception without a HelperMethodFrame
    //
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    // cannot pass null for source or destination
    if (gc.pSrc == NULL || gc.pDst == NULL) {
        FCThrowArgumentNullVoid(gc.pSrc==NULL ? L"source" : L"dest");
    }

    // source and destination must be arrays
    _ASSERTE(gc.pSrc->GetMethodTable()->IsArray());
    _ASSERTE(gc.pDst->GetMethodTable()->IsArray());

    g_IBCLogger.LogEEClassAndMethodTableAccess(gc.pSrc->GetArrayClass());

    // Equal method tables should imply equal rank
    _ASSERTE(!(gc.pSrc->GetMethodTable() == gc.pDst->GetMethodTable() && gc.pSrc->GetRank() != gc.pDst->GetRank()));

    // Which enables us to avoid touching the EEClass in simple cases
    if (gc.pSrc->GetMethodTable() != gc.pDst->GetMethodTable() && gc.pSrc->GetRank() != gc.pDst->GetRank()) {
        FCThrowResVoid(kRankException, L"Rank_MustMatch");
    }

    // Variant is dead.
    _ASSERTE(gc.pSrc->GetMethodTable() != COMVariant::s_pVariantClass);
    _ASSERTE(gc.pDst->GetMethodTable() != COMVariant::s_pVariantClass);


    int srcLB = gc.pSrc->GetLowerBoundsPtr()[0];
    int destLB = gc.pDst->GetLowerBoundsPtr()[0];
    // array bounds checking
    const unsigned int srcLen = gc.pSrc->GetNumComponents();
    const unsigned int destLen = gc.pDst->GetNumComponents();
    if (m_iLength < 0)
        FCThrowArgumentOutOfRangeVoid(L"length", L"ArgumentOutOfRange_NeedNonNegNum");

    if (m_iSrcIndex < srcLB || (m_iSrcIndex - srcLB < 0))
        FCThrowArgumentOutOfRangeVoid(L"srcIndex", L"ArgumentOutOfRange_ArrayLB");
        
    if (m_iDstIndex < destLB || (m_iDstIndex - destLB < 0))
        FCThrowArgumentOutOfRangeVoid(L"dstIndex", L"ArgumentOutOfRange_ArrayLB");

    if ((DWORD)(m_iSrcIndex - srcLB + m_iLength) > srcLen)
        FCThrowResVoid(kArgumentException, L"Arg_LongerThanSrcArray");
        
    if ((DWORD)(m_iDstIndex - destLB + m_iLength) > destLen)
        FCThrowResVoid(kArgumentException, L"Arg_LongerThanDestArray");

    int r = 0;
    
    // Small perf optimization - we copy from one portion of an array back to
    // itself a lot when resizing collections, etc.  The cost of doing the type
    // checking is significant for copying small numbers of bytes (~half of the time
    // for copying 1 byte within one array from element 0 to element 1).
    if (gc.pSrc == gc.pDst)
        r = AssignWillWork;
    else
        r = CanAssignArrayTypeNoGC(gc.pSrc, gc.pDst);

    if (r == AssignWrongType) {
        FCThrowResVoid(kArrayTypeMismatchException, L"ArrayTypeMismatch_CantAssignType");
    }

    if (r == AssignWillWork) {
        src = (BYTE*)gc.pSrc->GetDataPtr();
        dst = (BYTE*)gc.pDst->GetDataPtr();
        size = gc.pSrc->GetMethodTable()->GetComponentSize();
        g_IBCLogger.LogMethodTableAccess(gc.pSrc->GetMethodTable());
        m_memmove(dst + ((m_iDstIndex - destLB) * size), src + ((m_iSrcIndex - srcLB) * size), m_iLength * size);
        if (gc.pDst->GetMethodTable()->ContainsPointers())
        {
            GCHeap::GetGCHeap()->SetCardsAfterBulkCopy( (Object**) (dst + (m_iDstIndex * size)), m_iLength * size);
        }        
        FC_GC_POLL();
        return;
    }
    else if (reliable) {
        FCThrowResVoid(kArrayTypeMismatchException, L"ArrayTypeMismatch_ConstrainedCopy");
    }
    
    BOOL castEachElement = false;
    BOOL boxEachElement = false;
    BOOL unboxEachElement = false;
    BOOL primitiveWiden = false;

    HELPER_METHOD_FRAME_BEGIN_PROTECT(gc);
    if (r == AssignDontKnow)
    {
        r = CanAssignArrayType(gc.pSrc, gc.pDst);
    }
    CONSISTENCY_CHECK(r != AssignDontKnow);

    switch (r)
    {
        case AssignWrongType:
            COMPlusThrow(kArrayTypeMismatchException, L"ArrayTypeMismatch_CantAssignType");
            break;
            
        case AssignMustCast:
            castEachElement = true;
            break;
            
        case AssignWillWork:
            break;
            
        case AssignBoxValueClassOrPrimitive:
            boxEachElement = true;
            break;
            
        case AssignUnboxValueClassAndCast:
            castEachElement = true;
            unboxEachElement = true;
            break;
            
        case AssignPrimitiveWiden:
            primitiveWiden = true;
            break;

        default:
            _ASSERTE(!"Fell through switch in Array.Copy!");
    }
    // If we were called from Array.ConstrainedCopy, ensure that the array copy
    // is guaranteed to succeed.
    _ASSERTE(!reliable || r == AssignWillWork);

    if (m_iLength > 0)
    {
        // Casting and boxing are mutually exclusive.  But casting and unboxing may
        // coincide -- they are handled in the UnboxEachElement service.
        _ASSERTE(!boxEachElement || !castEachElement);

        if (r == AssignWillWork)
        {
            src = (BYTE*)gc.pSrc->GetDataPtr();
            dst = (BYTE*)gc.pDst->GetDataPtr();
            size = gc.pSrc->GetMethodTable()->GetComponentSize();
            g_IBCLogger.LogMethodTableAccess(gc.pSrc->GetMethodTable());
            m_memmove(dst + ((m_iDstIndex - destLB) * size), src + ((m_iSrcIndex - srcLB) * size), m_iLength * size);
            if (gc.pDst->GetMethodTable()->ContainsPointers())
            {
                GCHeap::GetGCHeap()->SetCardsAfterBulkCopy( (Object**) (dst + (m_iDstIndex * size)), m_iLength * size);
            }        
        }
        else if (unboxEachElement)
        {
            UnBoxEachElement(gc.pSrc, m_iSrcIndex - srcLB, gc.pDst, m_iDstIndex - destLB, m_iLength, castEachElement);
        }
        else if (boxEachElement)
        {
            BoxEachElement(gc.pSrc, m_iSrcIndex - srcLB, gc.pDst, m_iDstIndex - destLB, m_iLength);
        }
        else if (castEachElement)
        {
            _ASSERTE(!unboxEachElement);   // handled above
            CastCheckEachElement(gc.pSrc, m_iSrcIndex - srcLB, gc.pDst, m_iDstIndex - destLB, m_iLength);
        }
        else if (primitiveWiden)
        {
            PrimitiveWiden(gc.pSrc, m_iSrcIndex - srcLB, gc.pDst, m_iDstIndex - destLB, m_iLength);
        }
    }

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL3(void, SystemNative::ArrayClear, ArrayBase* pArrayUNSAFE, INT32 iIndex, INT32 iLength)
{
    BASEARRAYREF pArray = (BASEARRAYREF)pArrayUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_1(pArray);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // cannot pass null for array
    if (pArray == NULL)
        COMPlusThrowArgumentNull(L"array", L"ArgumentNull_Array");

    // array must be an array
    _ASSERTE(pArray->GetMethodTable()->IsArray());

    // array bounds checking
    int lb = pArray->GetLowerBoundsPtr()[0];
    if (iIndex < lb || (iIndex - lb) < 0 || iLength < 0)
        COMPlusThrow(kIndexOutOfRangeException);

    if ((iIndex - lb) > (int)pArray->GetNumComponents() - iLength)
        COMPlusThrow(kIndexOutOfRangeException);

    if (iLength > 0)
    {
        char* array = (char*)pArray->GetDataPtr();

        int size = pArray->GetMethodTable()->GetComponentSize();
        _ASSERTE(size >= 1);

        ZeroMemory(array + (iIndex - lb) * size, iLength * size);
    }

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL0(INT64, SystemNative::__GetSystemTimeAsFileTime)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    INT64 timestamp;

    ::GetSystemTimeAsFileTime((FILETIME*)&timestamp);

#if BIGENDIAN
    timestamp = (INT64)(((UINT64)timestamp >> 32) | ((UINT64)timestamp << 32));
#endif

    return timestamp;
}
FCIMPLEND;



FCIMPL0(UINT32, SystemNative::GetTickCount)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    return ::GetTickCount();
}
FCIMPLEND;



FCIMPL0(INT64, SystemNative::GetWorkingSet)
{
    DWORD memUsage = 0;
        
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    memUsage = WszGetWorkingSet();
    HELPER_METHOD_FRAME_END();
    
    return memUsage;
}
FCIMPLEND;

FCIMPL1(VOID,SystemNative::Exit,INT32 exitcode)
{
    // The exit code for the process is communicated in one of two ways.  If the
    // entrypoint returns an 'int' we take that.  Otherwise we take a latched
    // process exit code.  This can be modified by the app via setting
    // Environment's ExitCode property.
    SetLatchedExitCode(exitcode);

    HELPER_METHOD_FRAME_BEGIN_0();

    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    ForceEEShutdown();
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(VOID,SystemNative::SetExitCode,INT32 exitcode)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    // The exit code for the process is communicated in one of two ways.  If the
    // entrypoint returns an 'int' we take that.  Otherwise we take a latched
    // process exit code.  This can be modified by the app via setting
    // Environment's ExitCode property.
    SetLatchedExitCode(exitcode);
}
FCIMPLEND

FCIMPL0(INT32, SystemNative::GetExitCode)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;    
    
    // Return whatever has been latched so far.  This is uninitialized to 0.
    return GetLatchedExitCode();
}
FCIMPLEND

FCIMPL0(StringObject*, SystemNative::_GetCommandLine)
{
    STRINGREF   refRetVal   = NULL;
    LPWSTR commandLine;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;                
    }
    CONTRACTL_END;

    if (g_pCachedCommandLine != NULL)
    {
        // Use the cached command line if available
        commandLine = g_pCachedCommandLine;
    }
    else
    {
        commandLine = WszGetCommandLine();
        if (commandLine==NULL)
            COMPlusThrowOM();
    }
    
    refRetVal = COMString::NewString(commandLine);
    
    HELPER_METHOD_FRAME_END();

    return (StringObject*)OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL0(Object*, SystemNative::GetCommandLineArgs)
{
    PTRARRAYREF strArray = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, strArray);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;                
    }
    CONTRACTL_END;    

    LPWSTR commandLine;

    if (g_pCachedCommandLine != NULL)
    {
        // Use the cached command line if available
        commandLine = g_pCachedCommandLine;
    }
    else
    {
        commandLine = WszGetCommandLine();
        if (commandLine==NULL)
            COMPlusThrowOM();
    }

    DWORD numArgs = 0;
    LPWSTR* argv = SegmentCommandLine(commandLine, &numArgs);
    if (!argv)
        COMPlusThrowOM();

    _ASSERTE(numArgs > 0);
    
    strArray = (PTRARRAYREF) AllocateObjectArray(numArgs, g_pStringClass);
    // Copy each argument into new Strings.
    for(unsigned int i=0; i<numArgs; i++) 
    {
        STRINGREF str = COMString::NewString(argv[i]);
        STRINGREF * destData = ((STRINGREF*)(strArray->GetDataPtr())) + i;
        SetObjectReference((OBJECTREF*)destData, (OBJECTREF)str, strArray->GetAppDomain());
    }
    delete [] argv;

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(strArray); 
}
FCIMPLEND


FCIMPL1(FC_BOOL_RET, SystemNative::_GetCompatibilityFlag, int flag)
{
    WRAPPER_CONTRACT;

    FC_RETURN_BOOL(GetCompatibilityFlag((CompatibilityFlag)flag));
}
FCIMPLEND

// Note: Arguments checked in IL.
FCIMPL1(Object*, SystemNative::_GetEnvironmentVariable, StringObject* strVarUNSAFE)
{
    STRINGREF refRetVal;
    STRINGREF strVar;

    refRetVal   = NULL;
    strVar      = ObjectToSTRINGREF(strVarUNSAFE);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, refRetVal, strVar);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;                
    }
    CONTRACTL_END;

    // We loop round getting the length of the env var and then trying to copy
    // the value into a managed string. Usually we'll go through this loop
    // precisely once, but the caution is ncessary in case the variable mutates
    // beneath us.
    int len, newLen;

    // Get the length of the environment variable.
    WCHAR dummy;    // prefix complains if pass a null ptr in, so rely on the final length parm instead
    len = WszGetEnvironmentVariable(strVar->GetBuffer(), &dummy, 0);

    while (len != 0)
    {
        // Allocate the string.
        refRetVal = COMString::NewString(len);

        // Get the value.
        newLen = WszGetEnvironmentVariable(strVar->GetBuffer(), refRetVal->GetBuffer(), len);
        if (newLen != (len - 1))
        {
            // The envvar changed, need to do this again. Let GC collect the
            // string we just allocated.
            refRetVal = NULL;

            // Go back and try again.
            len = newLen;
        }
        else
            break;
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL0(Object*, SystemNative::GetEnvironmentCharArray)
{
    CHARARRAYREF chars = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, chars);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;                
    }
    CONTRACTL_END;

    WCHAR * strings = WszGetEnvironmentStrings();
    // Format for GetEnvironmentStrings is:
    // [=HiddenVar=value\0]* [Variable=value\0]* \0
    // See the description of Environment Blocks in MSDN's
    // CreateProcess page (null-terminated array of null-terminated strings).

    if (strings != NULL)
    {
        // Search for terminating \0\0 (two unicode \0's).
        WCHAR* ptr=strings;
        while (!(*ptr==0 && *(ptr+1)==0))
            ptr++;

        int len = (int)(ptr - strings + 1);

        chars = (CHARARRAYREF)AllocatePrimitiveArray(ELEMENT_TYPE_CHAR,len);
        WCHAR * buf = (WCHAR*) chars->GetDataPtr();
        memcpyNoGCRefs(buf, strings, len*sizeof(WCHAR));
        WszFreeEnvironmentStrings(strings);
    }

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(chars);
}
FCIMPLEND


// CaptureStackTraceMethod
// Return a method info for the method were the exception was thrown
FCIMPL1(MethodDesc*, SystemNative::CaptureStackTraceMethod, ArrayBase* pStackTraceUNSAFE)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    }
    CONTRACTL_END;
    
    I1ARRAYREF pArray(static_cast<I1Array *>(pStackTraceUNSAFE));
    StackTraceArray stackArray(pArray);

    if (!stackArray.Size())
        return NULL;

    // The managed stacktrace classes always returns typical method definition, so we don't need to bother providing exact instantiation.
    // Generics::GetExactInstantiationsFromCallInformation(pElements[0].pFunc, pElements[0].pExactGenericArgsToken, pTypeHandle, &pMD);

    MethodDesc* pFunc = stackArray[0].pFunc;

    // Strip the instantiation to make sure that the reflection never gets an evil method desc back.
    if (pFunc->HasMethodInstantiation())
        pFunc = pFunc->StripMethodInstantiation();
    _ASSERTE(pFunc->IsRuntimeMethodHandle());

    return pFunc;
}
FCIMPLEND

FCIMPL0(StringObject*, SystemNative::_GetModuleFileName)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;                
    }
    CONTRACTL_END;
    

    WCHAR wszFile[MAX_PATH];
    STRINGREF   refRetVal   = NULL;
    LPCWSTR pFileName = NULL;
    DWORD lgth = 0;

    if (g_pCachedModuleFileName)
    {
        pFileName = g_pCachedModuleFileName;
        lgth = (DWORD)wcslen(pFileName);
    }
    else
    {
        HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);
        lgth = WszGetModuleFileName(NULL, wszFile, MAX_PATH);
        if (!lgth)
        {
            COMPlusThrowWin32();
        }
        HELPER_METHOD_FRAME_END();
        pFileName = wszFile;
    }

    if(lgth) 
    {
        HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);       
        refRetVal = COMString::NewString(pFileName, lgth);
        HELPER_METHOD_FRAME_END();
    }
    return (StringObject*)OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL0(StringObject*, SystemNative::GetDeveloperPath)
{
    STRINGREF   refDevPath  = NULL;
    LPWSTR pPath = NULL;
    DWORD lgth = 0;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refDevPath);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;                
    }
    CONTRACTL_END;
        
        
    SystemDomain::System()->GetDevpathW(&pPath, &lgth);
    if(lgth) 
        refDevPath = COMString::NewString(pPath, lgth);
    
    HELPER_METHOD_FRAME_END();
    return (StringObject*)OBJECTREFToObject(refDevPath);
}
FCIMPLEND

FCIMPL0(StringObject*, SystemNative::GetRuntimeDirectory)
{
    wchar_t wszFile[MAX_PATH+1];
    STRINGREF   refRetVal   = NULL;
    DWORD dwFile = 0;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;


    HRESULT hr = ::GetCORSystemDirectory(wszFile, NumItems(wszFile), &dwFile);

    if(FAILED(hr))
        COMPlusThrowHR(hr);

    dwFile--; // remove the trailing NULL

    if(dwFile)
        refRetVal = COMString::NewString(wszFile, dwFile);

    HELPER_METHOD_FRAME_END();
    return (StringObject*)OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL0(StringObject*, SystemNative::GetHostBindingFile);
{
    STRINGREF refRetVal = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
        
        
    LPCWSTR wszFile = g_pConfig->GetProcessBindingFile();
    if(wszFile) 
        refRetVal = COMString::NewString(wszFile);

    HELPER_METHOD_FRAME_END();
    return (StringObject*)OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL1(FC_BOOL_RET, SystemNative::FromGlobalAccessCache, Object* refAssemblyUNSAFE)
{
    if (refAssemblyUNSAFE == NULL) 
        FCThrowRes(kNullReferenceException, L"NullReference_This");
    
    BOOL rv = FALSE;
    HELPER_METHOD_FRAME_BEGIN_RET_0();

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
        
        
    DomainAssembly*   pAssembly   = ((ASSEMBLYREF)ObjectToOBJECTREF(refAssemblyUNSAFE))->GetDomainAssembly();
    rv = pAssembly->GetFile()->IsSourceGAC();

    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(rv);
}
FCIMPLEND

FCIMPL0(FC_BOOL_RET, SystemNative::HasShutdownStarted)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    // Return true if the EE has started to shutdown and is now going to 
    // aggressively finalize objects referred to by static variables OR
    // if someone is unloading the current AppDomain AND we have started
    // finalizing objects referred to by static variables.
    FC_RETURN_BOOL((g_fEEShutDown & ShutDown_Finalize2) || GetAppDomain()->IsFinalizing());
}
FCIMPLEND

// Static message buffer used by SystemNative::FailFast to avoid reliance on a
// managed string object buffer. This buffer is not always used, see comments in
// the method below.
WCHAR g_szFailFastBuffer[256];
#define FAIL_FAST_STATIC_BUFFER_LENGTH (sizeof(g_szFailFastBuffer) / sizeof(WCHAR))

FCIMPL1(VOID, SystemNative::FailFast, StringObject* refMessageUNSAFE)
{   
    STRINGREF refMessage = (STRINGREF)refMessageUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_1(refMessage);

    CONTRACTL
    {
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    }
    CONTRACTL_END;

    // Managed code injected FailFast maps onto the unmanaged version
    // (EEPolicy::HandleFatalError) in the following manner: the exit code is
    // always set to COR_E_FAILFAST and the address passed (usually a failing
    // EIP) is in fact the address of a unicode message buffer (explaining the
    // reason for the fault).
    // The message string comes from a managed string object so we can't rely on
    // the buffer remaining in place below our feet. But equally we don't want
    // to inject failure points (by, for example, allocating a heap buffer or a
    // pinning handle) when we have a much higher chance than usual of actually
    // tripping those failure points and eradicating useful debugging info.
    // We employ various strategies to deal with this:
    //   o  If the message is small enough we copy it into a static buffer
    //      (g_szFailFastBuffer).
    //   o  Otherwise we try to allocate a buffer of the required size on the
    //      heap. This buffer will be leaked.
    //   o  If the allocation above fails we return to the static buffer and
    //      truncate the message.
    //
    // Another option would seem to be to implement a new frame type that
    // protects object references as pinned, but that seems like overkill for
    // just this problem.
    WCHAR  *pszMessage = NULL;
    DWORD   cchMessage = (refMessage == NULL) ? 0 : refMessage->GetStringLength();

    if (cchMessage < FAIL_FAST_STATIC_BUFFER_LENGTH)
    {
        pszMessage = g_szFailFastBuffer;
    }
    else
    {
        // We can fail here, but we can handle the fault.
        CONTRACT_VIOLATION(FaultViolation);
        pszMessage = new (nothrow) WCHAR[cchMessage + 1];
        if (pszMessage == NULL)
        {
            // Truncate the message to what will fit in the static buffer.
            cchMessage = FAIL_FAST_STATIC_BUFFER_LENGTH - 1;
            pszMessage = g_szFailFastBuffer;
        }
    }
    
    if (cchMessage > 0)
        memcpyNoGCRefs(pszMessage, refMessage->GetBuffer(), cchMessage * sizeof(WCHAR));
    pszMessage[cchMessage] = L'\0';

    if (cchMessage == 0) {
        WszOutputDebugString(L"CLR: Managed code called FailFast without specifying a reason.\r\n");
    }
    else {
        WszOutputDebugString(L"CLR: Managed code called FailFast, saying \"");
        WszOutputDebugString(pszMessage);
        WszOutputDebugString(L"\"\r\n");
    }

    // The HelperMethodFrame knows how to get the return address.
    UINT_PTR retaddr = HELPER_METHOD_FRAME_GET_RETURN_ADDRESS();
    EEPolicy::HandleFatalError(COR_E_FAILFAST, retaddr, pszMessage);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL0(FC_BOOL_RET, SystemNative::IsWin9x)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    FCUnique(0xa9);
    FC_RETURN_BOOL(FALSE);
}
FCIMPLEND

FCIMPL0(FC_BOOL_RET, SystemNative::IsServerHeap)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    FC_RETURN_BOOL(GCHeap::IsServerHeap());
}
FCIMPLEND

