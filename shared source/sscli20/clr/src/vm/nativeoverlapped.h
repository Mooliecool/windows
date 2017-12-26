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
** Header: COMNativeOverlapped.h
**       
**
** Purpose: Native methods for allocating and freeing NativeOverlapped
**
** Date:  January, 2000
** 
===========================================================*/

#ifndef _OVERLAPPED_H
#define _OVERLAPPED_H

// This should match the managed Overlapped object.
// If you make any change here, you need to change the managed part Overlapped.
class OverlappedDataObject : public Object
{
public:
    ASYNCRESULTREF m_asyncResult;
    OBJECTREF m_iocb;
    OBJECTREF m_iocbHelper;
    OBJECTREF m_overlapped;
    OBJECTREF m_userObject;
    OBJECTREF m_cacheLine;
    OBJECTHANDLE m_pinSelf;
    // OverlappedDataObject is very special.  An async pin handle keeps it alive.
    // During GC, we also make sure
    // 1. m_userObject itself does not move if m_userObject is not array
    // 2. Every object pointed by m_userObject does not move if m_userObject is array
    // We do not want to pin m_userObject if it is array.  But m_userObject may be updated
    // during relocation phase before OverlappedDataObject is doing relocation.
    // m_userObjectInternal is used to track the location of the m_userObject before it is updated.
    void *m_userObjectInternal;
    DWORD m_AppDomainId;
    short m_slot;
    unsigned char m_isArray;
    unsigned char m_toBeCleaned;

    ULONG_PTR  Internal;
    ULONG_PTR  InternalHigh;
    int     OffsetLow;
    int     OffsetHigh;
    ULONG_PTR  EventHandle;

    static OverlappedDataObject* GetOverlapped (LPOVERLAPPED nativeOverlapped)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        
        _ASSERTE (nativeOverlapped != NULL);
        _ASSERTE (GCHeap::GetGCHeap()->IsHeapPointer((BYTE *) nativeOverlapped));
        
        return (OverlappedDataObject*)((BYTE*)nativeOverlapped - offsetof(OverlappedDataObject, Internal));
    }

    DWORD GetAppDomainId()
    {
        return m_AppDomainId;
    }

    void HandleAsyncPinHandle();

    void FreeAsyncPinHandles();

    BOOL HasCompleted()
    {
        return FALSE;
    }

private:
    static LONG s_CleanupRequestCount;
    static BOOL s_CleanupInProgress;
    static BOOL s_GCDetectsCleanup;
    static BOOL s_CleanupFreeHandle;

public:
    static void RequestCleanup()
    {
        WRAPPER_CONTRACT;

        FastInterlockIncrement(&s_CleanupRequestCount);
        if (!s_CleanupInProgress)
        {
            StartCleanup();
        }
    }
    static void StartCleanup();

    static void FinishCleanup();

    static void MarkCleanupNeededFromGC()
    {
        LEAF_CONTRACT;
        s_GCDetectsCleanup = TRUE;
    }

    static BOOL CleanupNeededFromGC()
    {
        return s_GCDetectsCleanup;
    }

    static void RequestCleanupFromGC()
    {
        WRAPPER_CONTRACT;

        if (s_GCDetectsCleanup)
        {
            s_GCDetectsCleanup = FALSE;
            RequestCleanup();
        }
    }
};

#ifdef USE_CHECKED_OBJECTREFS

typedef REF<OverlappedDataObject> OVERLAPPEDDATAREF;
#define ObjectToOVERLAPPEDDATAREF(obj)     (OVERLAPPEDDATAREF(obj))
#define OVERLAPPEDDATAREFToObject(objref)  (OBJECTREFToObject (objref))

#else

typedef OverlappedDataObject* OVERLAPPEDDATAREF;
#define ObjectToOVERLAPPEDDATAREF(obj)    ((OverlappedDataObject*) (obj))
#define OVERLAPPEDDATAREFToObject(objref) ((OverlappedDataObject*) (objref))

#endif

FCDECL1(void*, AllocateNativeOverlapped, OverlappedDataObject* overlapped);
FCDECL1(void, FreeNativeOverlapped, LPOVERLAPPED lpOverlapped);
FCDECL1(OverlappedDataObject*, GetOverlappedFromNative, LPOVERLAPPED lpOverlapped);

#endif
