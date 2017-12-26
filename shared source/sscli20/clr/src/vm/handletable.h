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
 * Generational GC handle manager.  Entrypoint Header.
 *
 * Implements generic support for external handles into a GC heap.
 *
 */

#ifndef _HANDLETABLE_H
#define _HANDLETABLE_H



/****************************************************************************
 *
 * FLAGS, CONSTANTS AND DATA TYPES
 *
 ****************************************************************************/

#ifdef _DEBUG
#define DEBUG_DestroyedHandleValue ((_UNCHECKED_OBJECTREF)0x7)
#endif

/*
 * handle flags used by HndCreateHandleTable
 */
#define HNDF_NORMAL         (0x00)
#define HNDF_EXTRAINFO      (0x01)

/*
 * handle to handle table
 */
DECLARE_HANDLE(HHANDLETABLE);

/*--------------------------------------------------------------------------*/



/****************************************************************************
 *
 * PUBLIC ROUTINES AND MACROS
 *
 ****************************************************************************/

/*
 * handle manager init and shutdown routines
 */
HHANDLETABLE    HndCreateHandleTable(UINT *pTypeFlags, UINT uTypeCount, ADIndex uADIndex);
void            HndDestroyHandleTable(HHANDLETABLE hTable);

/*
 * retrieve index stored in table at creation 
 */
void            HndSetHandleTableIndex(HHANDLETABLE hTable, UINT uTableIndex);
UINT            HndGetHandleTableIndex(HHANDLETABLE hTable);
ADIndex         HndGetHandleTableADIndex(HHANDLETABLE hTable);
ADIndex         HndGetHandleADIndex(OBJECTHANDLE handle);

/*
 * individual handle allocation and deallocation
 */
OBJECTHANDLE    HndCreateHandle(HHANDLETABLE hTable, UINT uType, OBJECTREF object, LPARAM lExtraInfo = 0);
void            HndDestroyHandle(HHANDLETABLE hTable, UINT uType, OBJECTHANDLE handle);

void            HndDestroyHandleOfUnknownType(HHANDLETABLE hTable, OBJECTHANDLE handle);

/*
 * bulk handle allocation and deallocation
 */
UINT            HndCreateHandles(HHANDLETABLE hTable, UINT uType, OBJECTHANDLE *pHandles, UINT uCount);
void            HndDestroyHandles(HHANDLETABLE hTable, UINT uType, const OBJECTHANDLE *pHandles, UINT uCount);

/*
 * owner data associated with handles
 */
void            HndSetHandleExtraInfo(OBJECTHANDLE handle, UINT uType, LPARAM lExtraInfo);
LPARAM          HndGetHandleExtraInfo(OBJECTHANDLE handle);

/*
 * get parent table of handle
 */
HHANDLETABLE    HndGetHandleTable(OBJECTHANDLE handle);

/*
 * write barrier
 */
void            HndWriteBarrier(OBJECTHANDLE handle, OBJECTREF value);

/*
 * NON-GC handle enumeration
 */
typedef void (CALLBACK *HNDENUMPROC)(OBJECTHANDLE handle, LPARAM lExtraInfo, LPARAM lCallerParam);

void HndEnumHandles(HHANDLETABLE hTable, const UINT *puType, UINT uTypeCount,
                    HNDENUMPROC pfnEnum, LPARAM lParam, BOOL fAsync);

/*
 * GC-time handle scanning
 */
#define HNDGCF_NORMAL       (0x00000000)    // normal scan
#define HNDGCF_AGE          (0x00000001)    // age handles while scanning
#define HNDGCF_ASYNC        (0x00000002)    // drop the table lock while scanning
#define HNDGCF_EXTRAINFO    (0x00000004)    // iterate per-handle data while scanning

typedef void (CALLBACK *HANDLESCANPROC)(_UNCHECKED_OBJECTREF *pref, LPARAM *pExtraInfo, LPARAM param1, LPARAM param2);

void            HndScanHandlesForGC(HHANDLETABLE hTable,
                                    HANDLESCANPROC scanProc,
                                    LPARAM param1,
                                    LPARAM param2,
                                    const UINT *types,
                                    UINT typeCount,
                                    UINT condemned,
                                    UINT maxgen,
                                    UINT flags);

void            HndResetAgeMap(HHANDLETABLE hTable, const UINT *types, UINT typeCount, UINT condemned, UINT maxgen, UINT flags);
void            HndVerifyTable(HHANDLETABLE hTable, const UINT *types, UINT typeCount, UINT condemned, UINT maxgen, UINT flags);

void            HndNotifyGcCycleComplete(HHANDLETABLE hTable, UINT condemned, UINT maxgen);

/*--------------------------------------------------------------------------*/


#if defined(USE_CHECKED_OBJECTREFS) && !defined(_NOVM)
#define OBJECTREF_TO_UNCHECKED_OBJECTREF(objref)    (*((_UNCHECKED_OBJECTREF*)&(objref)))
#define UNCHECKED_OBJECTREF_TO_OBJECTREF(obj)       (OBJECTREF(obj))
#else
#define OBJECTREF_TO_UNCHECKED_OBJECTREF(objref)    (objref)
#define UNCHECKED_OBJECTREF_TO_OBJECTREF(obj)       (obj)
#endif

#ifdef _DEBUG_IMPL
void ValidateAssignObjrefForHandle(OBJECTREF, ADIndex appDomainIndex);
void ValidateFetchObjrefForHandle(OBJECTREF, ADIndex appDomainIndex);
void ValidateAppDomainForHandle(OBJECTHANDLE handle);
#endif

/*
 * handle assignment
 */
void HndAssignHandle(OBJECTHANDLE handle, OBJECTREF objref);

/*
 * interlocked-exchange assignment
 */
void* HndInterlockedCompareExchangeHandle(OBJECTHANDLE handle, OBJECTREF objref, OBJECTREF oldObjref);

/*
 * Note that HndFirstAssignHandle is similar to HndAssignHandle, except that it only
 * succeeds if transitioning from NULL to non-NULL.  In other words, if this handle
 * is being initialized for the first time.
 */
BOOL HndFirstAssignHandle(OBJECTHANDLE handle, OBJECTREF objref);

/*
 * inline handle dereferencing
 */

FORCEINLINE OBJECTREF HndFetchHandle(OBJECTHANDLE handle)
{
    WRAPPER_CONTRACT;

    // sanity
    _ASSERTE(handle);

#ifdef _DEBUG_IMPL
    _ASSERTE("Attempt to access destroyed handle." && *(_UNCHECKED_OBJECTREF *)handle != DEBUG_DestroyedHandleValue);

    // Make sure the objref for handle is valid
    ValidateFetchObjrefForHandle(ObjectToOBJECTREF(*(Object **)handle), 
                            HndGetHandleTableADIndex(HndGetHandleTable(handle)));
#endif

    // wrap the raw objectref and return it
    return UNCHECKED_OBJECTREF_TO_OBJECTREF(*PTR_UNCHECKED_OBJECTREF(handle));
}


/*
 * inline null testing (needed in certain cases where we're in the wrong GC mod)
 */
FORCEINLINE BOOL HndIsNull(OBJECTHANDLE handle)
{
    LEAF_CONTRACT;

    // sanity
    _ASSERTE(handle);

    return NULL == *(Object **)handle;
}



/*
 * inline handle checking
 */
FORCEINLINE BOOL HndCheckForNullUnchecked(OBJECTHANDLE handle)
{
    LEAF_CONTRACT;

    return (handle == NULL || (*(_UNCHECKED_OBJECTREF *)handle) == NULL);
}


/*
 *
 * Checks handle value for null or special value used for free handles in cache.
 *
 */
FORCEINLINE BOOL HndIsNullOrDestroyedHandle(_UNCHECKED_OBJECTREF value)
{
    LEAF_CONTRACT;

#ifdef DEBUG_DestroyedHandleValue
    if (value == DEBUG_DestroyedHandleValue)
         return TRUE;
#endif

    return (value == NULL);
}

/*--------------------------------------------------------------------------*/

#endif //_HANDLETABLE_H

