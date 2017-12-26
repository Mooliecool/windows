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
 *                        
 */

#include "common.h"
#include "vars.hpp"
#include "object.h"
#include "object.inl"
#include "log.h"
#include "eeconfig.h"
#include "gc.h"
#include "fieldmarshaler.h"
#include "memoryreport.h"
#ifdef GC_PROFILING
#include "handletablepriv.h"
#endif // GC_PROFILING

#undef ARRAYSIZE
#define ARRAYSIZE(a)        (sizeof(a) / sizeof(a[0]))

#include "objecthandle.h"
#include "nativeoverlapped.h"

GVAL_IMPL(HandleTableMap, g_HandleTableMap);

#ifndef DACCESS_COMPILE

//----------------------------------------------------------------------------

/*
 * struct VARSCANINFO
 *
 * used when tracing variable-strength handles.
 */
struct VARSCANINFO
{
    LPARAM         lEnableMask; // mask of types to trace
    HANDLESCANPROC pfnTrace;    // tracing function to use
	LPARAM		   lp2;			// second parameter
};


//----------------------------------------------------------------------------

/*
 * Scan callback for tracing variable-strength handles.
 *
 * This callback is called to trace individual objects referred to by handles
 * in the variable-strength table.
 */
void CALLBACK VariableTraceDispatcher(_UNCHECKED_OBJECTREF *pObjRef, LPARAM *pExtraInfo, LPARAM lp1, LPARAM lp2)
{
    WRAPPER_CONTRACT;

    // lp2 is a pointer to our VARSCANINFO
    struct VARSCANINFO *pInfo = (struct VARSCANINFO *)lp2;

    // is the handle's dynamic type one we're currently scanning?
    if ((*pExtraInfo & pInfo->lEnableMask) != 0)
    {
        // yes - call the tracing function for this handle
        pInfo->pfnTrace(pObjRef, NULL, lp1, pInfo->lp2);
    }
}



/*
 * Scan callback for pinning handles.
 *
 * This callback is called to pin individual objects referred to by handles in
 * the pinning table.
 */
void CALLBACK PinObject(_UNCHECKED_OBJECTREF *pObjRef, LPARAM *pExtraInfo, LPARAM lp1, LPARAM lp2)
{
    WRAPPER_CONTRACT;

    // PINNING IS EVIL - DON'T DO IT IF YOU CAN AVOID IT
    LOG((LF_GC, LL_WARNING, LOG_HANDLE_OBJECT_CLASS("WARNING: ", pObjRef, "causes pinning of ", *pObjRef)));

    Object **pRef = (Object **)pObjRef;
    _ASSERTE(lp2);
    promote_func* callback = (promote_func*) lp2;
    callback(*pRef, (ScanContext *)lp1, GC_CALL_PINNED);

    Object * pPinnedObj = *pRef;

    if (!HndIsNullOrDestroyedHandle(pPinnedObj) && pPinnedObj->GetGCSafeMethodTable() == g_pOverlappedDataClass)
    {
        // reporting the pinned user objects
        OverlappedDataObject *pOverlapped = (OverlappedDataObject *)pPinnedObj;
        if (pOverlapped->m_userObject != NULL)
        {
            //callback(OBJECTREF_TO_UNCHECKED_OBJECTREF(pOverlapped->m_userObject), (ScanContext *)lp1, GC_CALL_PINNED);
            if (pOverlapped->m_isArray)
            {
                pOverlapped->m_userObjectInternal = static_cast<void*>(OBJECTREFToObject(pOverlapped->m_userObject));
                ArrayBase* pUserObject = (ArrayBase*)OBJECTREFToObject(pOverlapped->m_userObject);
                Object **pObj = (Object**)pUserObject->GetDataPtr(TRUE);
                DWORD num = pUserObject->GetNumComponents();
                DWORD i;
                for (i = 0; i < num; i ++)
                {
                    callback(pObj[i], (ScanContext *)lp1, GC_CALL_PINNED);
                }
            }
            else
            {
                callback(OBJECTREF_TO_UNCHECKED_OBJECTREF(pOverlapped->m_userObject), (ScanContext *)lp1, GC_CALL_PINNED);
            }
        }

        if (pOverlapped->GetAppDomainId() !=  DefaultADID && pOverlapped->GetAppDomainIndex().m_dwIndex == DefaultADID)
        {
            OverlappedDataObject::MarkCleanupNeededFromGC();
        }
    }
}


/*
 * Scan callback for tracing strong handles.
 *
 * This callback is called to trace individual objects referred to by handles
 * in the strong table.
 */
void CALLBACK PromoteObject(_UNCHECKED_OBJECTREF *pObjRef, LPARAM *pExtraInfo, LPARAM lp1, LPARAM lp2)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC, LL_INFO1000, LOG_HANDLE_OBJECT_CLASS("", pObjRef, "causes promotion of ", *pObjRef)));

    Object **pRef = (Object **)pObjRef;
	_ASSERTE(lp2);
	promote_func* callback = (promote_func*) lp2;
	callback(*pRef, (ScanContext *)lp1, 0);
}


/*
 * Scan callback for disconnecting dead handles.
 *
 * This callback is called to check promotion of individual objects referred to by
 * handles in the weak tables.
 */
void CALLBACK CheckPromoted(_UNCHECKED_OBJECTREF *pObjRef, LPARAM *pExtraInfo, LPARAM lp1, LPARAM lp2)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC, LL_INFO100000, LOG_HANDLE_OBJECT_CLASS("Checking referent of Weak-", pObjRef, "to ", *pObjRef)));

    Object **pRef = (Object **)pObjRef;
    if (!GCHeap::GetGCHeap()->IsPromoted(*pRef, (ScanContext *)lp1))
    {
        LOG((LF_GC, LL_INFO100, LOG_HANDLE_OBJECT_CLASS("Severing Weak-", pObjRef, "to unreachable ", *pObjRef)));

        *pRef = NULL;
    }
    else
    {
        LOG((LF_GC, LL_INFO1000000, "reachable " LOG_OBJECT_CLASS(*pObjRef)));
    }
}


/*
 * Scan callback for updating pointers.
 *
 * This callback is called to update pointers for individual objects referred to by
 * handles in the weak and strong tables.
 */
void CALLBACK UpdatePointer(_UNCHECKED_OBJECTREF *pObjRef, LPARAM *pExtraInfo, LPARAM lp1, LPARAM lp2)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC, LL_INFO100000, LOG_HANDLE_OBJECT_CLASS("Querying for new location of ", pObjRef, "to ", *pObjRef)));

    Object **pRef = (Object **)pObjRef;

#ifdef _DEBUG
    Object *pOldLocation = *pRef;
#endif

	_ASSERTE(lp2);
	promote_func* callback = (promote_func*) lp2;
	callback(*pRef, (ScanContext *)lp1, 0);

#ifdef _DEBUG
    if (pOldLocation != *pObjRef)
        LOG((LF_GC, LL_INFO10000,  "Updating " FMT_HANDLE "from" FMT_ADDR "to " FMT_OBJECT "\n", 
             DBG_ADDR(pObjRef), DBG_ADDR(pOldLocation), DBG_ADDR(*pObjRef)));
    else
        LOG((LF_GC, LL_INFO100000, "Updating " FMT_HANDLE "- " FMT_OBJECT "did not move\n", 
             DBG_ADDR(pObjRef), DBG_ADDR(*pObjRef)));
#endif
}


#ifdef GC_PROFILING
/*
 * Scan callback for updating pointers.
 *
 * This callback is called to update pointers for individual objects referred to by
 * handles in the weak and strong tables.
 */
void CALLBACK ScanPointerForProfiler(_UNCHECKED_OBJECTREF *pObjRef, LPARAM *pExtraInfo, LPARAM lp1, LPARAM lp2)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC | LF_CORPROF, LL_INFO100000, LOG_HANDLE_OBJECT_CLASS("Notifying profiler of ", pObjRef, "to ", *pObjRef)));

    // Get the baseobject (which can subsequently be cast into an OBJECTREF == ObjectID
    Object **pRef = (Object **)pObjRef;

    // Get a hold of the heap ID that's tacked onto the end of the scancontext struct.
    ProfilingScanContext *pSC = (ProfilingScanContext *)lp1;

    DWORD rootFlags = 0;

    OBJECTHANDLE handle = (OBJECTHANDLE)(pRef);
    switch (HandleFetchType(handle))
    {
    case    HNDTYPE_WEAK_SHORT:
    case    HNDTYPE_WEAK_LONG:
        rootFlags |= COR_PRF_GC_ROOT_WEAKREF;
        break;

    case    HNDTYPE_STRONG:
        break;

    case    HNDTYPE_PINNED:
    case    HNDTYPE_ASYNCPINNED:
        rootFlags |= COR_PRF_GC_ROOT_PINNING;
        break;

    case    HNDTYPE_VARIABLE:
        _ASSERTE(!"Variable handle encountered");
        break;

    }

    // Give the profiler the objectref.
    PROFILER_CALL;
    g_profControlBlock.pProfInterface->RootReference2((BYTE *)*pRef, COR_PRF_GC_ROOT_HANDLE, (COR_PRF_GC_ROOT_FLAGS)rootFlags, pRef, &pSC->pHeapId);
}
#endif // GC_PROFILING


/*
 * Scan callback for updating pointers.
 *
 * This callback is called to update pointers for individual objects referred to by
 * handles in the pinned table.
 */
void CALLBACK UpdatePointerPinned(_UNCHECKED_OBJECTREF *pObjRef, LPARAM *pExtraInfo, LPARAM lp1, LPARAM lp2)
{
    WRAPPER_CONTRACT;

    Object **pRef = (Object **)pObjRef;

    _ASSERTE(lp2);
    promote_func* callback = (promote_func*) lp2;
    callback(*pRef, (ScanContext *)lp1, GC_CALL_PINNED);

    Object * pPinnedObj = *pRef;

    if (!HndIsNullOrDestroyedHandle(pPinnedObj) && pPinnedObj->GetGCSafeMethodTable() == g_pOverlappedDataClass)
    {
        // reporting the pinned user objects
        OverlappedDataObject *pOverlapped = (OverlappedDataObject *)pPinnedObj;
        if (pOverlapped->m_userObject != NULL)
        {
            if (pOverlapped->m_isArray != NULL)
            {
              ArrayBase* pUserObject = (ArrayBase*)(static_cast<Object*>(pOverlapped->m_userObjectInternal));
              Object **pObj = (Object**)pUserObject->GetDataPtr(TRUE);
                DWORD num = pUserObject->GetNumComponents();
                DWORD i;
                for (i = 0; i < num; i ++)
                {
                    callback(pObj[i], (ScanContext *)lp1, GC_CALL_PINNED);
                }
            }
            else
            {
                callback(OBJECTREF_TO_UNCHECKED_OBJECTREF(pOverlapped->m_userObject), (ScanContext *)lp1, GC_CALL_PINNED);
            }
        }
    }
    
    LOG((LF_GC, LL_INFO100000, LOG_HANDLE_OBJECT_CLASS("Updating ", pObjRef, "to pinned ", *pObjRef)));
}


//----------------------------------------------------------------------------

// flags describing the handle types
static UINT s_rgTypeFlags[] =
{
    HNDF_NORMAL,    // HNDTYPE_WEAK_SHORT
    HNDF_NORMAL,    // HNDTYPE_WEAK_LONG
    HNDF_NORMAL,    // HNDTYPE_STRONG
    HNDF_NORMAL,    // HNDTYPE_PINNED
    HNDF_EXTRAINFO, // HNDTYPE_VARIABLE
    HNDF_NORMAL,    // HNDTYPE_REFCOUNTED
    HNDF_EXTRAINFO, // HNDTYPE_DEPENDENT
    HNDF_NORMAL,    // HNDTYPE_ASYNCPINNED
};

int getNumberOfSlots()
{
    WRAPPER_CONTRACT;

    
    return (GCHeap::IsServerHeap() ? g_SystemInfo.dwNumberOfProcessors : 1);
}

void Ref_Initialize()
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("GCHandle");

    // sanity
    _ASSERTE(g_HandleTableMap.pBuckets == NULL);

    // Create an array of INITIAL_HANDLE_TABLE_ARRAY_SIZE HandleTableBuckets to hold the handle table sets
    HandleTableBucket  **pBuckets = new HandleTableBucket * [ INITIAL_HANDLE_TABLE_ARRAY_SIZE ];

    ZeroMemory(pBuckets,
         INITIAL_HANDLE_TABLE_ARRAY_SIZE * sizeof (HandleTableBucket *));

    // create the handle table set for the first bucket
    int n_slots = getNumberOfSlots();
    HHANDLETABLE *pTable = new HHANDLETABLE [ n_slots ];

    ZeroMemory(pTable,
         n_slots * sizeof (HHANDLETABLE));
    for (int uCPUindex=0; uCPUindex < n_slots; uCPUindex++)
    {
        pTable[uCPUindex] = HndCreateHandleTable(s_rgTypeFlags, ARRAYSIZE(s_rgTypeFlags), ADIndex(1));
    
        HndSetHandleTableIndex(pTable[uCPUindex], 0);
    }	
    HandleTableBucket *pBucket = new HandleTableBucket;

    pBuckets[0] = pBucket;
    pBucket->pTable = pTable;
    pBucket->HandleTableIndex = 0;

    g_HandleTableMap.pBuckets = pBuckets;
    g_HandleTableMap.dwMaxIndex = INITIAL_HANDLE_TABLE_ARRAY_SIZE;
    g_HandleTableMap.pNext = NULL;
}


class HandleTableBucketHolder
{
private:
    HandleTableBucket* m_bucket;
    int m_slots;
    BOOL m_SuppressRelease;
public:
    HandleTableBucketHolder(HandleTableBucket* bucket, int slots);
    ~HandleTableBucketHolder();

    void SuppressRelease()
    {
        m_SuppressRelease = TRUE;
    }
};

HandleTableBucketHolder::HandleTableBucketHolder(HandleTableBucket* bucket, int slots)
    :m_bucket(bucket), m_slots(slots), m_SuppressRelease(FALSE)
{
}

HandleTableBucketHolder::~HandleTableBucketHolder()
{
    if (m_SuppressRelease)
    {
        return;
    }
    if (m_bucket->pTable)
    {
        for (int n = 0; n < m_slots; n ++)
        {
            if (m_bucket->pTable[n])
            {
                HndDestroyHandleTable(m_bucket->pTable[n]);
            }
        }
        delete [] m_bucket->pTable;
    }
    delete m_bucket;
}

// ATTENTION: interface changed
// Note: this function called only from AppDomain::Init()
HandleTableBucket *Ref_CreateHandleTableBucket(ADIndex uADIndex)
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("GCHandle");

    HandleTableBucket *result = NULL;
    HandleTableMap *walk;
    
    walk = &g_HandleTableMap;
    HandleTableMap *last = NULL;
    UINT offset = 0;

    result = new HandleTableBucket;
    result->pTable = NULL;

    // create handle table set for the bucket
    int n_slots = getNumberOfSlots();

    HandleTableBucketHolder bucketHolder(result, n_slots);

    result->pTable = new HHANDLETABLE [ n_slots ];
    ZeroMemory(result->pTable, n_slots * sizeof (HHANDLETABLE));

    for (int uCPUindex=0; uCPUindex < n_slots; uCPUindex++) {
        result->pTable[uCPUindex] = HndCreateHandleTable(s_rgTypeFlags, ARRAYSIZE(s_rgTypeFlags), uADIndex);
    }	

    for (;;) {
        // Do we have free slot
        while (walk) {
            for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++) {
                if (walk->pBuckets[i] == 0) {
		        for (int uCPUindex=0; uCPUindex < n_slots; uCPUindex++)
			        HndSetHandleTableIndex(result->pTable[uCPUindex], i+offset);

                    result->HandleTableIndex = i+offset;
                    if (FastInterlockCompareExchangePointer((void**)&walk->pBuckets[i], (void*)result, NULL) == 0) {
                        // Get a free slot.
                        bucketHolder.SuppressRelease();
                        return result;
                    }
                }
            }
            last = walk;
            offset = walk->dwMaxIndex;
            walk = walk->pNext;
        }

        // No free slot.
        // Let's create a new node
        NewHolder<HandleTableMap> newMap;
        newMap = new HandleTableMap;

        newMap->pBuckets = new HandleTableBucket * [ INITIAL_HANDLE_TABLE_ARRAY_SIZE ];
        newMap.SuppressRelease();

        newMap->dwMaxIndex = last->dwMaxIndex + INITIAL_HANDLE_TABLE_ARRAY_SIZE;
        newMap->pNext = NULL;
        ZeroMemory(newMap->pBuckets,
                INITIAL_HANDLE_TABLE_ARRAY_SIZE * sizeof (HandleTableBucket *));

        if (FastInterlockCompareExchangePointer((void**)&last->pNext,newMap,NULL) != NULL) 
        {
            // This thread loses.
            delete [] newMap->pBuckets;
            delete newMap;
        }
        walk = last->pNext;
        offset = last->dwMaxIndex;
    }
}

void Ref_RemoveHandleTableBucket(HandleTableBucket *pBucket)
{
    LEAF_CONTRACT;

    size_t          index   = pBucket->HandleTableIndex;
    HandleTableMap* walk    = &g_HandleTableMap;
    size_t          offset  = 0;

    while (walk) 
    {
        if ((index < walk->dwMaxIndex) && (index >= offset)) 
        {
            // During AppDomain unloading, we first remove a handle table and then destroy
            // the table.  As soon as the table is removed, the slot can be reused.
            if (walk->pBuckets[index - offset] == pBucket)
            {
                walk->pBuckets[index - offset] = NULL;
                return;
            }
        }
        offset = walk->dwMaxIndex;
        walk   = walk->pNext;
    }

    // Didn't find it.  This will happen typically from Ref_DestroyHandleTableBucket if 
    // we explicitly call Ref_RemoveHandleTableBucket first.
}


void Ref_DestroyHandleTableBucket(HandleTableBucket *pBucket)
{
    WRAPPER_CONTRACT;

    // this check is because here we might be called from AppDomain::Terminate after AppDomain::ClearGCRoots,
    // which calls Ref_RemoveHandleTableBucket itself

    Ref_RemoveHandleTableBucket(pBucket);
    for (int uCPUindex=0; uCPUindex < getNumberOfSlots(); uCPUindex++)
    {
        HndDestroyHandleTable(pBucket->pTable[uCPUindex]);
    }
    delete [] pBucket->pTable;
    delete pBucket;
}

int getSlotNumber(ScanContext* sc)
{
    WRAPPER_CONTRACT;

    return (GCHeap::IsServerHeap() ? sc->thread_number : 0);
}

void Ref_EndSynchronousGC(UINT condemned, UINT maxgen)
{
    LEAF_CONTRACT;

// NOT used, must be modified for MTHTS (scalable HandleTable scan) if planned to use:
// need to pass ScanContext info to split HT bucket by threads, or to be performed under t_join::join
/*
    // tell the table we finished a GC
    HandleTableMap *walk = &g_HandleTableMap;
    while (walk) {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++) {
            HHANDLETABLE hTable = walk->pTable[i];
            if (hTable)
                HndNotifyGcCycleComplete(hTable, condemned, maxgen);
        }
        walk = walk->pNext;
    }
*/    
}



//----------------------------------------------------------------------------

/*
 * CreateVariableHandle.
 *
 * Creates a variable-strength handle.
 *
 * N.B. This routine is not a macro since we do validation in RETAIL.
 * We always validate the type here because it can come from external callers.
 */
OBJECTHANDLE CreateVariableHandle(HHANDLETABLE hTable, OBJECTREF object, UINT type)
{
    WRAPPER_CONTRACT;

    // verify that we are being asked to create a valid type
    if (!IS_VALID_VHT_VALUE(type))
    {
        // bogus value passed in
        _ASSERTE(FALSE);
        return NULL;
    }

    // create the handle
    return HndCreateHandle(hTable, HNDTYPE_VARIABLE, object, (LPARAM)type);
}


/*
 * UpdateVariableHandleType.
 *
 * Changes the dynamic type of a variable-strength handle.
 *
 * N.B. This routine is not a macro since we do validation in RETAIL.
 * We always validate the type here because it can come from external callers.
 */
void UpdateVariableHandleType(OBJECTHANDLE handle, UINT type)
{
    WRAPPER_CONTRACT;

    // verify that we are being asked to set a valid type
    if (!IS_VALID_VHT_VALUE(type))
    {
        // bogus value passed in
        _ASSERTE(FALSE);
        return;
    }

    //
    // If/when concurrent GC is implemented, we need to make sure variable handles
    // DON'T change type during an asynchronous scan, OR that we properly recover
    // from the change.  Some changes are benign, but for example changing to or
    // from a pinning handle in the middle of a scan would not be fun.
    //

    // store the type in the handle's extra info
    HndSetHandleExtraInfo(handle, HNDTYPE_VARIABLE, (LPARAM)type);
}


/*
 * TraceVariableHandles.
 *
 * Convenience function for tracing variable-strength handles.
 * Wraps HndScanHandlesForGC.
 */
void TraceVariableHandles(HANDLESCANPROC pfnTrace, LPARAM lp1, LPARAM lp2, UINT uEnableMask, UINT condemned, UINT maxgen, UINT flags)
{
    WRAPPER_CONTRACT;

    // set up to scan variable handles with the specified mask and trace function
    UINT               type = HNDTYPE_VARIABLE;
    struct VARSCANINFO info = { (LPARAM)uEnableMask, pfnTrace, lp2 };

    HandleTableMap *walk = &g_HandleTableMap;
    while (walk) {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
        	if (walk->pBuckets[i] != NULL)
		{
	           HHANDLETABLE hTable = walk->pBuckets[i]->pTable[getSlotNumber((ScanContext*) lp1)];
       	     if (hTable)
	                HndScanHandlesForGC(hTable, VariableTraceDispatcher,
       	                             lp1, (LPARAM)&info, &type, 1, condemned, maxgen, HNDGCF_EXTRAINFO | flags);
	        }
        walk = walk->pNext;
    }
}

/*
  loop scan version of TraceVariableHandles for single-thread-managed Ref_* functions
  should be kept in sync with the code above
*/
void TraceVariableHandlesBySingleThread(HANDLESCANPROC pfnTrace, LPARAM lp1, LPARAM lp2, UINT uEnableMask, UINT condemned, UINT maxgen, UINT flags)
{
    WRAPPER_CONTRACT;

    // set up to scan variable handles with the specified mask and trace function
    UINT type = HNDTYPE_VARIABLE;
    struct VARSCANINFO info = { (LPARAM)uEnableMask, pfnTrace, lp2 };

    HandleTableMap *walk = &g_HandleTableMap;
    while (walk) {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
        	if (walk->pBuckets[i] != NULL)
              // this is the one of Ref_* function performed by single thread in MULTI_HEAPS case, so we need to loop through all HT of the bucket
        	for (int uCPUindex=0; uCPUindex < getNumberOfSlots(); uCPUindex++)
		{
	           HHANDLETABLE hTable = walk->pBuckets[i]->pTable[uCPUindex];
            if (hTable)
                HndScanHandlesForGC(hTable, VariableTraceDispatcher,
                                    lp1, (LPARAM)&info, &type, 1, condemned, maxgen, HNDGCF_EXTRAINFO | flags);
        }
        walk = walk->pNext;
    }
}


//----------------------------------------------------------------------------

void Ref_TracePinningRoots(UINT condemned, UINT maxgen, ScanContext* sc, Ref_promote_func* fn)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC, LL_INFO10000, "Pinning referents of pinned handles in generation %u\n", condemned));

    // pin objects pointed to by pinning handles
    UINT types[2] = {HNDTYPE_PINNED, HNDTYPE_ASYNCPINNED};
    UINT flags = sc->concurrent ? HNDGCF_ASYNC : HNDGCF_NORMAL;

    HandleTableMap *walk = &g_HandleTableMap;
    while (walk) {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
            if (walk->pBuckets[i] != NULL)
            {
                HHANDLETABLE hTable = walk->pBuckets[i]->pTable[getSlotNumber((ScanContext*) sc)];
                if (hTable)
                    HndScanHandlesForGC(hTable, PinObject, LPARAM(sc), LPARAM(fn), types, ARRAYSIZE(types), condemned, maxgen, flags);
            }
        walk = walk->pNext;
    }

    // pin objects pointed to by variable handles whose dynamic type is VHT_PINNED
    TraceVariableHandles(PinObject, LPARAM(sc), LPARAM(fn), VHT_PINNED, condemned, maxgen, flags);
}


void Ref_TraceNormalRoots(UINT condemned, UINT maxgen, ScanContext* sc, Ref_promote_func* fn)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC, LL_INFO10000, "Promoting referents of strong handles in generation %u\n", condemned));

    // promote objects pointed to by strong handles
    UINT type = HNDTYPE_STRONG;
    UINT flags = (sc->concurrent) ? HNDGCF_ASYNC : HNDGCF_NORMAL;

    HandleTableMap *walk = &g_HandleTableMap;
    while (walk) {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
        	if (walk->pBuckets[i] != NULL)
	       {
	            HHANDLETABLE hTable = walk->pBuckets[i]->pTable[getSlotNumber(sc)];
            if (hTable)
                HndScanHandlesForGC(hTable, PromoteObject, LPARAM(sc), LPARAM(fn), &type, 1, condemned, maxgen, flags);
        }
        walk = walk->pNext;
    }

    // promote objects pointed to by variable handles whose dynamic type is VHT_STRONG
    TraceVariableHandles(PromoteObject, LPARAM(sc), LPARAM(fn), VHT_STRONG, condemned, maxgen, flags);

}

void Ref_CheckReachable(UINT condemned, UINT maxgen, LPARAM lp1)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC, LL_INFO10000, "Checking reachability of referents of long-weak handles in generation %u\n", condemned));

    // these are the handle types that need to be checked
    UINT types[] =
    {
        HNDTYPE_WEAK_LONG,
    };

    // check objects pointed to by short weak handles
    UINT flags = (((ScanContext*) lp1)->concurrent) ? HNDGCF_ASYNC : HNDGCF_NORMAL;
    int uCPUindex = getSlotNumber((ScanContext*) lp1);

    HandleTableMap *walk = &g_HandleTableMap;
    while (walk) {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
        {
        	if (walk->pBuckets[i] != NULL)
	       {
	            HHANDLETABLE hTable = walk->pBuckets[i]->pTable[uCPUindex];
            if (hTable)
                HndScanHandlesForGC(hTable, CheckPromoted, lp1, 0, types, ARRAYSIZE(types), condemned, maxgen, flags);
        }
        }
        walk = walk->pNext;
    }

    // check objects pointed to by variable handles whose dynamic type is VHT_WEAK_LONG
    TraceVariableHandles(CheckPromoted, lp1, 0, VHT_WEAK_LONG, condemned, maxgen, flags);
}



void Ref_CheckAlive(UINT condemned, UINT maxgen, LPARAM lp1)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC, LL_INFO10000, "Checking liveness of referents of short-weak handles in generation %u\n", condemned));

    // perform a multi-type scan that checks for unreachable objects
    UINT type = HNDTYPE_WEAK_SHORT;
    UINT flags = (((ScanContext*) lp1)->concurrent) ? HNDGCF_ASYNC : HNDGCF_NORMAL;

    int uCPUindex = getSlotNumber((ScanContext*) lp1);
    HandleTableMap *walk = &g_HandleTableMap;
    while (walk)
    {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
        {
        	if (walk->pBuckets[i] != NULL)
	       {
	            HHANDLETABLE hTable = walk->pBuckets[i]->pTable[uCPUindex];
            if (hTable)
                HndScanHandlesForGC(hTable, CheckPromoted, lp1, 0, &type, 1, condemned, maxgen, flags);
        }
        }
        walk = walk->pNext;
    }
    // check objects pointed to by variable handles whose dynamic type is VHT_WEAK_SHORT
    TraceVariableHandles(CheckPromoted, lp1, 0, VHT_WEAK_SHORT, condemned, maxgen, flags);
}


// NTOE: Please: if you update this function, update the very similar profiling function immediately below!!!
void Ref_UpdatePointers(UINT condemned, UINT maxgen, ScanContext* sc, Ref_promote_func* fn)
{
    WRAPPER_CONTRACT;

    // For now, treat the syncblock as if it were short weak handles.                  
    BOOL bDo = TRUE;

    if (GCHeap::IsServerHeap()) 
    {
        static LONG volatile uCount = 0;
        bDo = (FastInterlockIncrement(&uCount) == 1);
        FastInterlockCompareExchange (&uCount, 0, GCHeap::GetGCHeap()->GetNumberOfHeaps());        
        _ASSERTE (uCount <= GCHeap::GetGCHeap()->GetNumberOfHeaps());
    }

    if (bDo)   
    	SyncBlockCache::GetSyncBlockCache()->GCWeakPtrScan(&UpdatePointer, LPARAM(sc), LPARAM(fn));

    LOG((LF_GC, LL_INFO10000, "Updating pointers to referents of non-pinning handles in generation %u\n", condemned));

    // these are the handle types that need their pointers updated
    UINT types[] =
    {
        HNDTYPE_WEAK_SHORT,
        HNDTYPE_WEAK_LONG,
        HNDTYPE_STRONG,
    };

    // perform a multi-type scan that updates pointers
    UINT flags = (sc->concurrent) ? HNDGCF_ASYNC : HNDGCF_NORMAL;

    HandleTableMap *walk = &g_HandleTableMap;
    while (walk) {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
        	if (walk->pBuckets[i] != NULL)
	       {
	            HHANDLETABLE hTable = walk->pBuckets[i]->pTable[getSlotNumber(sc)];
            if (hTable)
                HndScanHandlesForGC(hTable, UpdatePointer, LPARAM(sc), LPARAM(fn), types, ARRAYSIZE(types), condemned, maxgen, flags);
        }
        walk = walk->pNext;
    }

    // update pointers in variable handles whose dynamic type is VHT_WEAK_SHORT, VHT_WEAK_LONG or VHT_STRONG
    TraceVariableHandles(UpdatePointer, LPARAM(sc), LPARAM(fn), VHT_WEAK_SHORT | VHT_WEAK_LONG | VHT_STRONG, condemned, maxgen, flags);
}

#ifdef PROFILING_SUPPORTED
// Please update this if you change the Ref_UpdatePointers function above.
void Ref_ScanPointersForProfiler(UINT maxgen, LPARAM lp1)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC | LF_CORPROF, LL_INFO10000, "Scanning all roots for profiler.\n"));

    // Don't scan the sync block because they should not be reported. They are weak handles only

    UINT types[] =
    {
        HNDTYPE_WEAK_SHORT,
        HNDTYPE_WEAK_LONG,
        HNDTYPE_STRONG,
        HNDTYPE_PINNED,
//        HNDTYPE_VARIABLE,
        HNDTYPE_ASYNCPINNED,
    };

    UINT flags = HNDGCF_NORMAL;

    // perform a multi-type scan that updates pointers
    HandleTableMap *walk = &g_HandleTableMap;
    while (walk) {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
        	if (walk->pBuckets[i] != NULL)
              // this is the one of Ref_* function performed by single thread in MULTI_HEAPS case, so we need to loop through all HT of the bucket
        	for (int uCPUindex=0; uCPUindex < getNumberOfSlots(); uCPUindex++)
	       {
	            HHANDLETABLE hTable = walk->pBuckets[i]->pTable[uCPUindex];
            if (hTable)
                HndScanHandlesForGC(hTable, &ScanPointerForProfiler, lp1, 0, types, ARRAYSIZE(types), maxgen, maxgen, flags);
        }
        walk = walk->pNext;
    }

    // update pointers in variable handles whose dynamic type is VHT_WEAK_SHORT, VHT_WEAK_LONG or VHT_STRONG
    TraceVariableHandlesBySingleThread(&ScanPointerForProfiler, lp1, 0, VHT_WEAK_SHORT | VHT_WEAK_LONG | VHT_STRONG, maxgen, maxgen, flags);
}
#endif // PROFILING_SUPPORTED

void Ref_UpdatePinnedPointers(UINT condemned, UINT maxgen, ScanContext* sc, Ref_promote_func* fn)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC, LL_INFO10000, "Updating pointers to referents of pinning handles in generation %u\n", condemned));

    // these are the handle types that need their pointers updated
    UINT types[2] = {HNDTYPE_PINNED, HNDTYPE_ASYNCPINNED};
    UINT flags = (sc->concurrent) ? HNDGCF_ASYNC : HNDGCF_NORMAL;

    HandleTableMap *walk = &g_HandleTableMap;
    while (walk) {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
            if (walk->pBuckets[i] != NULL)
            {
                HHANDLETABLE hTable = walk->pBuckets[i]->pTable[getSlotNumber(sc)];
                if (hTable)
                    HndScanHandlesForGC(hTable, UpdatePointerPinned, LPARAM(sc), LPARAM(fn), types, ARRAYSIZE(types), condemned, maxgen, flags); 
            }
        walk = walk->pNext;
    }

    // update pointers in variable handles whose dynamic type is VHT_PINNED
    TraceVariableHandles(UpdatePointerPinned, LPARAM(sc), LPARAM(fn), VHT_PINNED, condemned, maxgen, flags);
}


void Ref_AgeHandles(UINT condemned, UINT maxgen, LPARAM lp1)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC, LL_INFO10000, "Aging handles in generation %u\n", condemned));

    // these are the handle types that need their ages updated
    UINT types[] =
    {
        HNDTYPE_WEAK_SHORT,
        HNDTYPE_WEAK_LONG,

        HNDTYPE_STRONG,

        HNDTYPE_PINNED,
        HNDTYPE_VARIABLE,
        HNDTYPE_ASYNCPINNED,
    };

    int uCPUindex = getSlotNumber((ScanContext*) lp1);
    // perform a multi-type scan that ages the handles
    HandleTableMap *walk = &g_HandleTableMap;
    while (walk) {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
        	if (walk->pBuckets[i] != NULL)
	       {
	            HHANDLETABLE hTable = walk->pBuckets[i]->pTable[uCPUindex];
            if (hTable)
                HndScanHandlesForGC(hTable, NULL, 0, 0, types, ARRAYSIZE(types), condemned, maxgen, HNDGCF_AGE);
        }
        walk = walk->pNext;
    }
}


void Ref_RejuvenateHandles(UINT condemned, UINT maxgen, LPARAM lp1)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC, LL_INFO10000, "Rejuvenating handles.\n"));

    // these are the handle types that need their ages updated
    UINT types[] =
    {
        HNDTYPE_WEAK_SHORT,
        HNDTYPE_WEAK_LONG,


        HNDTYPE_STRONG,

        HNDTYPE_PINNED,
        HNDTYPE_VARIABLE,
        HNDTYPE_ASYNCPINNED,
    };

    int uCPUindex = getSlotNumber((ScanContext*) lp1);
    // reset the ages of these handles
    HandleTableMap *walk = &g_HandleTableMap;
    while (walk) {
        for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
        	if (walk->pBuckets[i] != NULL)
	       {
	            HHANDLETABLE hTable = walk->pBuckets[i]->pTable[uCPUindex];
            if (hTable)
                HndResetAgeMap(hTable, types, ARRAYSIZE(types), condemned, maxgen, HNDGCF_NORMAL);
        }
        walk = walk->pNext;
    }
}

void Ref_VerifyHandleTable(UINT condemned, UINT maxgen)
{
    WRAPPER_CONTRACT;

    LOG((LF_GC, LL_INFO10000, "Verifying handles.\n"));

    // these are the handle types that need to be verified
    UINT types[] =
    {
        HNDTYPE_WEAK_SHORT,
        HNDTYPE_WEAK_LONG,


        HNDTYPE_STRONG,

        HNDTYPE_PINNED,
        HNDTYPE_VARIABLE,
        HNDTYPE_ASYNCPINNED,
    };

    int n_slots = getNumberOfSlots();
    for (int uCPUindex = 0; uCPUindex < n_slots; uCPUindex++)
    {
        // verify these handles
        HandleTableMap *walk = &g_HandleTableMap;
        while (walk)
        {
            for (UINT i = 0; i < INITIAL_HANDLE_TABLE_ARRAY_SIZE; i ++)
            {
                if (walk->pBuckets[i] != NULL)
	            {
	                HHANDLETABLE hTable = walk->pBuckets[i]->pTable[uCPUindex];
                    if (hTable)
                        HndVerifyTable(hTable, types, ARRAYSIZE(types), condemned, maxgen, HNDGCF_NORMAL);
                }
            }
            walk = walk->pNext;
        }
    }
}

int GetCurrentThreadHomeHeapNumber()
{
    WRAPPER_CONTRACT;

	if (!GCHeap::IsGCHeapInitialized())
		return 0;
    return GCHeap::GetGCHeap()->GetHomeHeapNumber();
}

bool HandleTableBucket::Contains(OBJECTHANDLE handle)
{
    LEAF_CONTRACT;

    if (NULL == handle)
    {
        return FALSE;
    }
    
    HHANDLETABLE hTable = HndGetHandleTable(handle);
    for (int uCPUindex=0; uCPUindex < GCHeap::GetGCHeap()->GetNumberOfHeaps(); uCPUindex++)
    {
        if (hTable == this->pTable[uCPUindex]) 
        {
            return TRUE;
        }
    }
    return FALSE;
}

// A hash table to track size of objects that may be moved to default domain
typedef EEHashTable<size_t, EEPtrHashTableHelper<size_t>, FALSE> EEHashTableOfMT;
EEHashTableOfMT *s_pPinHandleTable;

CrstStatic s_PinHandleTableCrst;

void InitializePinHandleTable()
{
    WRAPPER_CONTRACT;

    s_PinHandleTableCrst.Init("Pin Handle Table", CrstPinHandle, CRST_DEFAULT);
    LockOwner lock = {&s_PinHandleTableCrst, IsOwnerOfCrst};
    s_pPinHandleTable = new EEHashTableOfMT();
    s_pPinHandleTable->Init(10, &lock);
}

// We can not fail due to OOM when we move an object to default domain during AD unload.
// If we may need a dummy MethodTable later, we allocate the MethodTable here.
void AddMTForPinHandle(OBJECTREF obj)
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
    }
    CONTRACTL_END;

    if (obj == NULL)
    {
        return;
    }

    _ASSERTE (g_pOverlappedDataClass != NULL);

    int size = 0;
    MethodTable *pMT = obj->GetMethodTable();

    if (pMT == g_pStringClass)
    {
        return;
    }
    else if (pMT == g_pOverlappedDataClass)
    {
        return;
    }
    else if (pMT->IsArray())
    {
        BASEARRAYREF asArray = (BASEARRAYREF) obj;
        if (CorTypeInfo::IsPrimitiveType(asArray->GetArrayElementType())) 
            return;

        TypeHandle th = asArray->GetArrayElementTypeHandle();
        _ASSERTE (th.IsUnsharedMT());
        MethodTable *pElemMT = th.AsMethodTable();
        _ASSERTE (pElemMT->IsValueClass() && pElemMT->GetClass()->IsBlittable());
        if (pElemMT->GetModule()->IsSystem())
        {
            return;
        }

        // Create an ArrayMethodTable that has the same element size
        size = - pMT->GetComponentSize();
    }
    else if (pMT->GetModule()->IsSystem())
    {
        return;
    }
    else
    {
        size = pMT->GetBaseSize();
    }

    HashDatum data;
    if (s_pPinHandleTable->GetValue(size, &data) == FALSE)
    {
        CrstHolder csh(&s_PinHandleTableCrst);
        if (s_pPinHandleTable->GetValue(size, &data) == FALSE)
        {
            // We do not need to include GCDescr here, since this
            // methodtable does not contain pointers.
            BYTE *buffer = new BYTE[sizeof(MethodTable)];
            memset (buffer, 0, sizeof(MethodTable));
            MethodTable *pNewMT = (MethodTable *)buffer;
            NewArrayHolder<BYTE> pMTHolder(buffer);
            pNewMT->SetClass(NULL);
            pNewMT->InitializeFlags2(MethodTable::enum_flag2_IsAsyncPin);
            if (size > 0)
            {
                pNewMT->SetBaseSize(size);
            }
            else
            {
                pNewMT->SetBaseSize(ObjSizeOf (ArrayBase));
                pNewMT->InitializeFlags(MethodTable::enum_flag_Array | (-size) /* ComponentSize */);
            }
            s_pPinHandleTable->InsertValue(size, (HashDatum)pNewMT);
            pMTHolder.SuppressRelease();
        }
    }
}

// We need to ensure that the MethodTable of an object is valid in default domain when the object
// is move to default domain duing AD unload.
void BashMTForPinnedObject(OBJECTREF obj)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;

    ADIndex adIndx = obj->GetAppDomainIndex();
    ADIndex defaultAdIndx = SystemDomain::System()->DefaultDomain()->GetIndex();
    if (adIndx.m_dwIndex != 0 && adIndx != defaultAdIndx)
    {
        obj->GetHeader()->ResetAppDomainIndexNoFailure(defaultAdIndx);
    }
    int size = 0;
    MethodTable *pMT = obj->GetMethodTable();
    if (pMT == g_pStringClass)
    {
        // String class has no reference to other objects
        return;
    }
    else if (pMT == g_pOverlappedDataClass)
    {
        // Managed Overlapped
        OVERLAPPEDDATAREF overlapped = (OVERLAPPEDDATAREF)(obj);
        overlapped->m_asyncResult = NULL;
        overlapped->m_iocb = NULL;
        overlapped->m_iocbHelper = NULL;
        overlapped->m_overlapped = NULL;
        overlapped->m_cacheLine = NULL;

        if (overlapped->m_userObject != NULL)
        {
            if (overlapped->m_isArray == 1)
            {
                BASEARRAYREF asArray = (BASEARRAYREF) (overlapped->m_userObject);
                OBJECTREF *pObj = (OBJECTREF*)asArray->GetDataPtr (TRUE);
                DWORD num = asArray->GetNumComponents();
                DWORD i;
                for (i = 0; i < num; i ++)
                {
                    BashMTForPinnedObject(pObj[i]);
                }
            }
            else
            {
                BashMTForPinnedObject(overlapped->m_userObject);
            }
        }
        STRESS_LOG1 (LF_APPDOMAIN | LF_GC, LL_INFO100, "OverlappedData %p:MT is bashed\n", OBJECTREFToObject (overlapped));
        return;
    }
    else if (pMT->IsArray())
    {
        BASEARRAYREF asArray = (BASEARRAYREF) obj;
        if (CorTypeInfo::IsPrimitiveType(asArray->GetArrayElementType())) 
            return;

        TypeHandle th = asArray->GetArrayElementTypeHandle();
        _ASSERTE (th.IsUnsharedMT());
        MethodTable *pElemMT = th.AsMethodTable();
        _ASSERTE (pElemMT->IsValueClass() && pElemMT->GetClass()->IsBlittable());
        if (pElemMT->GetModule()->IsSystem())
        {
            return;
        }

        // Create an ArrayMethodTable that has the same element size
        size = - pMT->GetComponentSize();
    }
    else if (pMT->GetModule()->IsSystem())
    {
        _ASSERTE (pMT->GetClass()->IsBlittable());
        return;
    }
    else
    {
        size = pMT->GetBaseSize();
    }
    
    HashDatum data = NULL;
    BOOL fRet;
    fRet = s_pPinHandleTable->GetValue(size, &data);
    _ASSERTE(fRet);
    PREFIX_ASSUME(data != NULL);
    obj->SetMethodTable((MethodTable*)data);
}
#endif // !DACCESS_COMPILE

