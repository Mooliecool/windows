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
// MarshalNative.h -
//
// FCall's for the Marshal class
//


#ifndef __MARSHALNATIVE_H__
#define __MARSHALNATIVE_H__

#include "fcall.h"

//!!! Must be kept in sync with ArrayWithOffset class layout.
struct ArrayWithOffsetData
{
    BASEARRAYREF    m_Array;
    INT32           m_cbOffset;
    INT32           m_cbCount;
};



class MarshalNative
{
public:
    //====================================================================
    // These methods convert between an HR and and a managed exception.
    //====================================================================
    static FCDECL2(void, ThrowExceptionForHR, INT32 errorCode, LPVOID errorInfo);
    static FCDECL2(Object *, GetExceptionForHR, INT32 errorCode, LPVOID errorInfo);
    static FCDECL1(int, GetHRForException, Object* eUNSAFE);

    static FCDECL4(void, CopyToNative, Object* psrcUNSAFE, INT32 startindex, LPVOID pdst, INT32 length);
    static FCDECL4(void, CopyToManaged, LPVOID psrc, Object* pdstUNSAFE, INT32 startindex, INT32 length);
    static FCDECL1(UINT32, SizeOfClass, ReflectClassBaseObject* refClass);

    static FCDECL1(UINT32, FCSizeOfObject, Object* pObjUNSAFE);
    static FCDECL2(LPVOID, FCUnsafeAddrOfPinnedArrayElement, ArrayBase *arr, INT32 index);

    static FCDECL1(UINT32, OffsetOfHelper, FieldDesc *pField);
    static FCDECL0(int, GetLastWin32Error);
    static FCDECL1(void, SetLastWin32Error, int error);
    static FCDECL1(INT32, CalculateCount, ArrayWithOffsetData* pRef);
    
    static FCDECL2(Object*, PtrToStringAnsi, LPVOID ptr, INT32 len);
    static FCDECL2(Object*, PtrToStringUni, LPVOID ptr, INT32 len);

    static FCDECL3(VOID, StructureToPtr, Object* pObjUNSAFE, LPVOID ptr, CLR_BOOL fDeleteOld);
    static FCDECL3(VOID, PtrToStructureHelper, LPVOID ptr, Object* pObjIn, CLR_BOOL allowValueClasses);
    static FCDECL2(VOID, DestroyStructure, LPVOID ptr, ReflectClassBaseObject* refClassUNSAFE);


    static FCDECL0(UINT32, GetSystemMaxDBCSCharSize);

    static FCDECL2(LPVOID, GCHandleInternalAlloc, Object *obj, int type);
    static FCDECL1(VOID, GCHandleInternalFree, OBJECTHANDLE handle);
    static FCDECL1(LPVOID, GCHandleInternalGet, OBJECTHANDLE handle);
    static FCDECL3(VOID, GCHandleInternalSet, OBJECTHANDLE handle, Object *obj, CLR_BOOL isPinned);
    static FCDECL4(Object*, GCHandleInternalCompareExchange, OBJECTHANDLE handle, Object *obj, Object* oldObj, CLR_BOOL isPinned);
    static FCDECL1(LPVOID, GCHandleInternalAddrOfPinnedObject, OBJECTHANDLE handle);
    static FCDECL1(VOID, GCHandleInternalCheckDomain, OBJECTHANDLE handle);

    static FCDECL2(Object*, GetDelegateForFunctionPointerInternal, LPVOID FPtr, ReflectClassBaseObject* refTypeUNSAFE);
    static FCDECL1(LPVOID, GetFunctionPointerForDelegateInternal, Object* refDelegateUNSAFE);


};

#endif
