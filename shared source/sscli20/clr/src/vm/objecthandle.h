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
/*
 * Wraps handle table to implement various handle types (Strong, Weak, etc.)
 *       
 */

#ifndef _OBJECTHANDLE_H
#define _OBJECTHANDLE_H

#include "holder.h"

/*
 * include handle manager declarations
 */
#include "handletable.h"

/*
 * Convenience macros for accessing handles.  StoreFirstObjectInHandle is like
 * StoreObjectInHandle, except it only succeeds if transitioning from NULL to
 * non-NULL.  In other words, if this handle is being initialized for the first
 * time.
 */
#define ObjectFromHandle(handle)                   HndFetchHandle(handle)
#define StoreObjectInHandle(handle, object)        HndAssignHandle(handle, object)
#define InterlockedCompareExchangeObjectInHandle(handle, object, oldObj)        HndInterlockedCompareExchangeHandle(handle, object, oldObj)
#define StoreFirstObjectInHandle(handle, object)   HndFirstAssignHandle(handle, object)
#define ObjectHandleIsNull(handle)                 HndIsNull(handle)
#define IsHandleNullUnchecked(handle)              HndCheckForNullUnchecked(handle)


/*
 * HANDLES
 *
 * The default type of handle is a strong handle.
 *
 */
#define HNDTYPE_DEFAULT                         HNDTYPE_STRONG


/*
 * WEAK HANDLES
 *
 * Weak handles are handles that track an object as long as it is alive,
 * but do not keep the object alive if there are no strong references to it.
 *
 * The default type of weak handle is 'long-lived' weak handle.
 *
 */
#define HNDTYPE_WEAK_DEFAULT                    HNDTYPE_WEAK_LONG


/*
 * SHORT-LIVED WEAK HANDLES
 *
 * Short-lived weak handles are weak handles that track an object until the
 * first time it is detected to be unreachable.  At this point, the handle is
 * severed, even if the object will be visible from a pending finalization
 * graph.  This further implies that short weak handles do not track
 * across object resurrections.
 *
 */
#define HNDTYPE_WEAK_SHORT                      (0)


/*
 * LONG-LIVED WEAK HANDLES
 *
 * Long-lived weak handles are weak handles that track an object until the
 * object is actually reclaimed.  Unlike short weak handles, long weak handles
 * continue to track their referents through finalization and across any
 * resurrections that may occur.
 *
 */
#define HNDTYPE_WEAK_LONG                       (1)


/*
 * STRONG HANDLES
 *
 * Strong handles are handles which function like a normal object reference.
 * The existence of a strong handle for an object will cause the object to
 * be promoted (remain alive) through a garbage collection cycle.
 *
 */
#define HNDTYPE_STRONG                          (2)


/*
 * PINNED HANDLES
 *
 * Pinned handles are strong handles which have the added property that they
 * prevent an object from moving during a garbage collection cycle.  This is
 * useful when passing a pointer to object innards out of the runtime while GC
 * may be enabled.
 *
 * NOTE:  PINNING AN OBJECT IS EXPENSIVE AS IT PREVENTS THE GC FROM ACHIEVING
 *        OPTIMAL PACKING OF OBJECTS DURING EPHEMERAL COLLECTIONS.  THIS TYPE
 *        OF HANDLE SHOULD BE USED SPARINGLY!
 */
#define HNDTYPE_PINNED                          (3)


/*
 * VARIABLE HANDLES
 *
 * Variable handles are handles whose type can be changed dynamically.  They
 * are larger than other types of handles, and are scanned a little more often,
 * but are useful when the handle owner needs an efficient way to change the
 * strength of a handle on the fly.
 * 
 */
#define HNDTYPE_VARIABLE                        (4)




/*
 * PINNED HANDLES for asynchronous operation
 *
 * Pinned handles are strong handles which have the added property that they
 * prevent an object from moving during a garbage collection cycle.  This is
 * useful when passing a pointer to object innards out of the runtime while GC
 * may be enabled.
 *
 * NOTE:  PINNING AN OBJECT IS EXPENSIVE AS IT PREVENTS THE GC FROM ACHIEVING
 *        OPTIMAL PACKING OF OBJECTS DURING EPHEMERAL COLLECTIONS.  THIS TYPE
 *        OF HANDLE SHOULD BE USED SPARINGLY!
 */
#define HNDTYPE_ASYNCPINNED                          (7)


typedef DPTR(struct HandleTableMap) PTR_HandleTableMap;
typedef DPTR(struct HandleTableBucket) PTR_HandleTableBucket;
typedef DPTR(PTR_HandleTableBucket) PTR_PTR_HandleTableBucket;

struct HandleTableMap
{
    PTR_PTR_HandleTableBucket   pBuckets;
    PTR_HandleTableMap          pNext;
    DWORD                       dwMaxIndex;
};

GVAL_DECL(HandleTableMap, g_HandleTableMap);

#define INITIAL_HANDLE_TABLE_ARRAY_SIZE 10

// struct containing g_SystemInfo.dwNumberOfProcessors HHANDLETABLEs and current table index
// instead of just single HHANDLETABLE for on-fly balancing while adding handles on multiproc machines

struct HandleTableBucket
{
    HHANDLETABLE*   pTable;
    UINT            HandleTableIndex;

    bool Contains(OBJECTHANDLE handle);
};


/*
 * Type mask definitions for HNDTYPE_VARIABLE handles.
 */
#define VHT_WEAK_SHORT              (0x00000100)  // avoid using low byte so we don't overlap normal types
#define VHT_WEAK_LONG               (0x00000200)  // avoid using low byte so we don't overlap normal types
#define VHT_STRONG                  (0x00000400)  // avoid using low byte so we don't overlap normal types
#define VHT_PINNED                  (0x00000800)  // avoid using low byte so we don't overlap normal types

#define IS_VALID_VHT_VALUE(flag)   ((flag == VHT_WEAK_SHORT) || \
                                    (flag == VHT_WEAK_LONG)  || \
                                    (flag == VHT_STRONG)     || \
                                    (flag == VHT_PINNED))

/*
 * Convenience macros and prototypes for the various handle types we define
 */

inline OBJECTHANDLE CreateTypedHandle(HHANDLETABLE table, OBJECTREF object, int type)
{ 
    WRAPPER_CONTRACT;

    return HndCreateHandle(table, type, object); 
}

inline void DestroyTypedHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandleOfUnknownType(HndGetHandleTable(handle), handle);
}

inline OBJECTHANDLE CreateHandle(HHANDLETABLE table, OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

    return HndCreateHandle(table, HNDTYPE_DEFAULT, object); 
}

inline void DestroyHandle(OBJECTHANDLE handle)
{ 
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_DEFAULT, handle);
}

inline OBJECTHANDLE CreateDuplicateHandle(OBJECTHANDLE handle) {
    WRAPPER_CONTRACT;

    // Create a new STRONG handle in the same table as an existing handle.  
    return HndCreateHandle(HndGetHandleTable(handle), HNDTYPE_DEFAULT, ObjectFromHandle(handle));
}


inline OBJECTHANDLE CreateWeakHandle(HHANDLETABLE table, OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

    return HndCreateHandle(table, HNDTYPE_WEAK_DEFAULT, object); 
}

inline void DestroyWeakHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_WEAK_DEFAULT, handle);
}

inline OBJECTHANDLE CreateShortWeakHandle(HHANDLETABLE table, OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

    return HndCreateHandle(table, HNDTYPE_WEAK_SHORT, object); 
}

inline void DestroyShortWeakHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_WEAK_SHORT, handle);
}


inline OBJECTHANDLE CreateLongWeakHandle(HHANDLETABLE table, OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

    return HndCreateHandle(table, HNDTYPE_WEAK_LONG, object); 
}

inline void DestroyLongWeakHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_WEAK_LONG, handle);
}

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(OBJECTHANDLE);
#endif

typedef Holder<OBJECTHANDLE,DoNothing<OBJECTHANDLE>,DestroyLongWeakHandle> LongWeakHandleHolder;

inline OBJECTHANDLE CreateStrongHandle(HHANDLETABLE table, OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

    return HndCreateHandle(table, HNDTYPE_STRONG, object); 
}

inline void DestroyStrongHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_STRONG, handle);
}

inline OBJECTHANDLE CreatePinningHandle(HHANDLETABLE table, OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

    return HndCreateHandle(table, HNDTYPE_PINNED, object); 
}

inline void DestroyPinningHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_PINNED, handle);
}

typedef Wrapper<OBJECTHANDLE, DoNothing<OBJECTHANDLE>, DestroyPinningHandle, NULL> PinningHandleHolder;

inline OBJECTHANDLE CreateAsyncPinningHandle(HHANDLETABLE table, OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

    return HndCreateHandle(table, HNDTYPE_ASYNCPINNED, object); 
}

inline void DestroyAsyncPinningHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_ASYNCPINNED, handle);
}

typedef Wrapper<OBJECTHANDLE, DoNothing<OBJECTHANDLE>, DestroyAsyncPinningHandle, NULL> AsyncPinningHandleHolder;



OBJECTHANDLE CreateVariableHandle(HHANDLETABLE hTable, OBJECTREF object, UINT type);
void         UpdateVariableHandleType(OBJECTHANDLE handle, UINT type);

inline void  DestroyVariableHandle(OBJECTHANDLE handle)
{
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_VARIABLE, handle);
}

void GCHandleValidatePinnedObject(OBJECTREF obj);

/*
 * Holder for OBJECTHANDLE
 */
 
FORCEINLINE void VoidDestroyHandle(OBJECTHANDLE oh) 
{ 
    WRAPPER_CONTRACT;

    (DestroyHandle)(oh); 
}
typedef Wrapper<OBJECTHANDLE, DoNothing<OBJECTHANDLE>, VoidDestroyHandle > OHWrapper;

class OBJECTHANDLEHolder : public OHWrapper
{
public:
    FORCEINLINE OBJECTHANDLEHolder(OBJECTHANDLE p = NULL) : OHWrapper(p)
    {
        LEAF_CONTRACT;
    }
    FORCEINLINE void operator=(OBJECTHANDLE p)
    {
        WRAPPER_CONTRACT;

        OHWrapper::operator=(p);
    }
};

/*
 * Convenience prototypes for using the global handles
 */

int GetCurrentThreadHomeHeapNumber();

inline OBJECTHANDLE CreateGlobalTypedHandle(OBJECTREF object, int type)
{ 
    WRAPPER_CONTRACT;
#ifndef DACCESS_COMPILE
    return HndCreateHandle(g_HandleTableMap.pBuckets[0]->pTable[GetCurrentThreadHomeHeapNumber()], type, object); 
#else
    DacNotImpl();
    return NULL;
#endif    
}

inline void DestroyGlobalTypedHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandleOfUnknownType(HndGetHandleTable(handle), handle);
}

inline OBJECTHANDLE CreateGlobalHandle(OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

#ifndef DACCESS_COMPILE
    return HndCreateHandle(g_HandleTableMap.pBuckets[0]->pTable[GetCurrentThreadHomeHeapNumber()], HNDTYPE_DEFAULT, object); 
#else
    DacNotImpl();
    return NULL;
#endif
}

inline void DestroyGlobalHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_DEFAULT, handle);
}

inline OBJECTHANDLE CreateGlobalWeakHandle(OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

#ifndef DACCESS_COMPILE
    return HndCreateHandle(g_HandleTableMap.pBuckets[0]->pTable[GetCurrentThreadHomeHeapNumber()], HNDTYPE_WEAK_DEFAULT, object); 
#else
    DacNotImpl();
    return NULL;
#endif

}

inline void DestroyGlobalWeakHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_WEAK_DEFAULT, handle);
}

inline OBJECTHANDLE CreateGlobalShortWeakHandle(OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

#ifndef DACCESS_COMPILE
    return HndCreateHandle(g_HandleTableMap.pBuckets[0]->pTable[GetCurrentThreadHomeHeapNumber()], HNDTYPE_WEAK_SHORT, object); 
#else
    DacNotImpl();
    return NULL;
#endif
    
}

inline void DestroyGlobalShortWeakHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_WEAK_SHORT, handle);
}

typedef Holder<OBJECTHANDLE,DoNothing<OBJECTHANDLE>,DestroyGlobalShortWeakHandle> GlobalShortWeakHandleHolder;

inline OBJECTHANDLE CreateGlobalLongWeakHandle(OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

#ifndef DACCESS_COMPILE
    return HndCreateHandle(g_HandleTableMap.pBuckets[0]->pTable[GetCurrentThreadHomeHeapNumber()], HNDTYPE_WEAK_LONG, object); 
#else
    DacNotImpl();
    return NULL;
#endif

}

inline void DestroyGlobalLongWeakHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_WEAK_LONG, handle);
}

inline OBJECTHANDLE CreateGlobalStrongHandle(OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

#ifndef DACCESS_COMPILE
    return HndCreateHandle(g_HandleTableMap.pBuckets[0]->pTable[GetCurrentThreadHomeHeapNumber()], HNDTYPE_STRONG, object); 
#else
    DacNotImpl();
    return NULL;
#endif
    
}

inline void DestroyGlobalStrongHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_STRONG, handle);
}

typedef Holder<OBJECTHANDLE,DoNothing<OBJECTHANDLE>,DestroyGlobalStrongHandle> GlobalStrongHandleHolder;

inline OBJECTHANDLE CreateGlobalPinningHandle(OBJECTREF object)
{ 
    WRAPPER_CONTRACT;

#ifndef DACCESS_COMPILE
    return HndCreateHandle(g_HandleTableMap.pBuckets[0]->pTable[GetCurrentThreadHomeHeapNumber()], HNDTYPE_PINNED, object); 
#else
    DacNotImpl();
    return NULL;
#endif
    
}

inline void DestroyGlobalPinningHandle(OBJECTHANDLE handle)
{ 
    WRAPPER_CONTRACT;

    HndDestroyHandle(HndGetHandleTable(handle), HNDTYPE_PINNED, handle);
}

inline void ResetOBJECTHANDLE(OBJECTHANDLE handle)
{
    WRAPPER_CONTRACT;

    StoreObjectInHandle(handle, NULL);
}

typedef Holder<OBJECTHANDLE,DoNothing<OBJECTHANDLE>,ResetOBJECTHANDLE> ObjectInHandleHolder;

/*
 * Table maintenance routines
 */
void Ref_Initialize();
void Ref_Shutdown();
HandleTableBucket *Ref_CreateHandleTableBucket(ADIndex uADIndex);
BOOL Ref_HandleAsyncPinHandles();
void Ref_RelocateAsyncPinHandles(HandleTableBucket *pSource, HandleTableBucket *pTarget);
void Ref_RemoveHandleTableBucket(HandleTableBucket *pBucket);
void Ref_DestroyHandleTableBucket(HandleTableBucket *pBucket);

BOOL Ref_ContainHandle(HandleTableBucket *pBucket, OBJECTHANDLE handle);

/*
 * GC-time scanning entrypoints
 */
void Ref_BeginSynchronousGC   (UINT uCondemnedGeneration, UINT uMaxGeneration);
void Ref_EndSynchronousGC     (UINT uCondemnedGeneration, UINT uMaxGeneration);

typedef void Ref_promote_func(class Object*&, ScanContext*, DWORD);

void Ref_TracePinningRoots(UINT condemned, UINT maxgen, ScanContext* sc, Ref_promote_func* fn);
void Ref_TraceNormalRoots(UINT condemned, UINT maxgen, ScanContext* sc, Ref_promote_func* fn);
void Ref_UpdatePointers(UINT condemned, UINT maxgen, ScanContext* sc, Ref_promote_func* fn);
void Ref_UpdatePinnedPointers(UINT condemned, UINT maxgen, ScanContext* sc, Ref_promote_func* fn);

void Ref_CheckReachable       (UINT uCondemnedGeneration, UINT uMaxGeneration, LPARAM lp1);
void Ref_CheckAlive           (UINT uCondemnedGeneration, UINT uMaxGeneration, LPARAM lp1);
void Ref_ScanPointersForProfiler(UINT uMaxGeneration, LPARAM lp1);
void Ref_AgeHandles           (UINT uCondemnedGeneration, UINT uMaxGeneration, LPARAM lp1);
void Ref_RejuvenateHandles(UINT uCondemnedGeneration, UINT uMaxGeneration, LPARAM lp1);

void Ref_VerifyHandleTable(UINT condemned, UINT maxgen);

#ifndef DACCESS_COMPILE
void InitializePinHandleTable();
void AddMTForPinHandle(OBJECTREF obj);
void BashMTForPinnedObject(OBJECTREF obj);
#endif

#endif //_OBJECTHANDLE_H
