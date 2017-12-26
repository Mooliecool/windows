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

inline void HndAssignHandle(OBJECTHANDLE handle, OBJECTREF objref)
{
    WRAPPER_CONTRACT;

    // sanity
    _ASSERTE(handle);

#ifdef _DEBUG_IMPL
    // handle should not be in unloaded domain
    ValidateAppDomainForHandle(handle);

    // Make sure the objref is valid before it is assigned to a handle
    ValidateAssignObjrefForHandle(objref, HndGetHandleTableADIndex(HndGetHandleTable(handle)));
#endif
    // unwrap the objectref we were given
    _UNCHECKED_OBJECTREF value = OBJECTREF_TO_UNCHECKED_OBJECTREF(objref);

    // if we are doing a non-NULL pointer store then invoke the write-barrier
    if (value)
        HndWriteBarrier(handle, objref);

    // store the pointer
    *(_UNCHECKED_OBJECTREF *)handle = value;
}

inline void* HndInterlockedCompareExchangeHandle(OBJECTHANDLE handle, OBJECTREF objref, OBJECTREF oldObjref)
{
    WRAPPER_CONTRACT;

    // sanity
    _ASSERTE(handle);

#ifdef _DEBUG_IMPL
    // handle should not be in unloaded domain
    ValidateAppDomainForHandle(handle);

    // Make sure the objref is valid before it is assigned to a handle
    ValidateAssignObjrefForHandle(objref, HndGetHandleTableADIndex(HndGetHandleTable(handle)));
#endif
    // unwrap the objectref we were given
    _UNCHECKED_OBJECTREF value = OBJECTREF_TO_UNCHECKED_OBJECTREF(objref);
    _UNCHECKED_OBJECTREF oldValue = OBJECTREF_TO_UNCHECKED_OBJECTREF(oldObjref);

    // if we are doing a non-NULL pointer store then invoke the write-barrier
    if (value)
        HndWriteBarrier(handle, objref);

    // store the pointer
    
    return InterlockedCompareExchangePointer((PVOID *)handle, (PVOID)value, (PVOID)oldValue);
}

inline BOOL HndFirstAssignHandle(OBJECTHANDLE handle, OBJECTREF objref)
{
    WRAPPER_CONTRACT;

    // sanity
    _ASSERTE(handle);

#ifdef _DEBUG_IMPL
    // handle should not be in unloaded domain
    ValidateAppDomainForHandle(handle);

    // Make sure the objref is valid before it is assigned to a handle
    ValidateAssignObjrefForHandle(objref, HndGetHandleTableADIndex(HndGetHandleTable(handle)));
#endif
    // unwrap the objectref we were given
    _UNCHECKED_OBJECTREF value = OBJECTREF_TO_UNCHECKED_OBJECTREF(objref);

    // store the pointer if we are the first ones here
    BOOL success = (NULL == FastInterlockCompareExchangePointer((void **)handle,
                                                                *(void **)&value,
                                                                NULL));

    // if we successfully did a non-NULL pointer store then invoke the write-barrier
    if (value && success)
        HndWriteBarrier(handle, objref);

    // return our result
    return success;
}

