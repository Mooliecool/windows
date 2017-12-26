/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/handle.h

Abstract:

    Generic handle management : obtain, validate and release handle numbers;
    other functions that don't need any knowledge of the handle type

--*/

#ifndef _PAL_HANDLE_H_
#define _PAL_HANDLE_H_

#include "pal/palinternal.h"
#include "pal/shmemory.h"

/* Pseudo handles constant for current thread and process */
extern const HANDLE hPseudoCurrentProcess;
extern const HANDLE hPseudoCurrentThread;
/* Pseudo handle constant for the global IO Completion port */
extern const HANDLE hPseudoGlobalIOCP;

typedef struct _HOBJSTRUCT HOBJSTRUCT;
/* Remote Object Struct */
typedef struct _HREMOTEOBJSTRUCT HREMOTEOBJSTRUCT; 

/* definition of callback functions for type-specific handle management */

/*++
Callback :
    CLOSEHANDLEFUNC

    Performs type-specific work when closing a handle (decrement refcount,
    free memory, etc.). Called from CLoseHandle

Parameters :
    HANDLE handle : handle to close
    HANDLESTRUCT * structure : information structure of handle to close

Return Value :
    0 on success
    -1 if the object validation fails
    -2 if a problem is encountered while accessing shared memory

Notes :
    This function is reponsible for freeing the memory of the HANDLESTRUCT,
    since it will have been allocated in type-specific code.
    This function MUST NOT call HMGRFreeHandle, CloseHandle does that.
--*/
typedef int (*CLOSEHANDLEFUNC)(HOBJSTRUCT *);

/*++
Callback :
    DUPHANDLEFUNC

    Performs type-specific work when duplicating a handle, eg. increment
    reference count, etc. Called after a new handle has been allocated.

Parameters :
    HANDLE handle : the new handle
    HANDLESTRUCT * structure : handle data associated with original handle

Return Value :
    0 on success
    -1 if the object validation fails
    -2 if a problem is encountered while accessing shared memory

Notes :
    This function is responsible for ensuring that multiple handles to the
    same object are properly accounted for.
    If this function fails (non-zero error code), the handle is NOT duplicated.
    This means the reference count must NOT be incremented in case of failure.
--*/
typedef int (*DUPHANDLEFUNC)(HANDLE, HOBJSTRUCT *);

typedef enum
{
    HOBJ_INVALID,
    HOBJ_PROCESS,
    HOBJ_THREAD,
    HOBJ_FILE,
    HOBJ_MAP,
    HOBJ_SEMAPHORE,
    HOBJ_EVENT,
    HOBJ_MUTEX,
    HOBJ_LAST,        /* The last of the real handle types */
} HOBJTYPE;

struct _HOBJSTRUCT
{
    HOBJTYPE type;
    /* callback functions for type-specific work */
    DUPHANDLEFUNC   dup_handle;
    CLOSEHANDLEFUNC close_handle;
};

/* PAL_LocalHandleToRemote and PAL_RemoteHandleToLocal both uses a temporary 
   shared memory object to reference the share memory object structure */
struct _HREMOTEOBJSTRUCT
{ 
    HOBJTYPE type;
    SHMPTR ShmKernelObject;
};

/* function declarations */

/*++
Handle management functions :
    These functions are ONLY for obtaining handle values! No management is
    done on the actual objects associated with the handles.
--*/

/*++
Function :
    HMGRInitHandleManager

    Initialize the handle manager. Call this only once!

    (no parameters)

Return value :
    TRUE on success, FALSE on failure
--*/
BOOL HMGRInitHandleManager(void);

/*++
Function :
    HMGRStopHandleManager

    Free all resources used by the Handle manager; close all handles.

    (no parameters)

Return value :
    TRUE on success, FALSE on failure
--*/
BOOL HMGRStopHandleManager(void);

/*++
Function :
    HMGRGetHandle

    Allocate a handle from the Free Handle Pool

Parameters :
    HOBJSTRUCT *handle_data : information to associate with the handle.
                Function fails if this is NULL.

Return Value :
    Value of allocated handle, or INVALID_HANDLE_VALUE (-1) on failure.

Notes :
    The handle manager may check the object type in handle_data->type to
    validate the handle, but will not modify it in any way. Basically, it only
    keeps it so that HMGRGetData can return it.
--*/
HANDLE HMGRGetHandle(HOBJSTRUCT *handle_data);

/*++
Function :
    HMGRFreeHandle

    Place a handle back in the free handle pool

Parameters :
    HANDLE handle : handle to free.

Return Value :
    TRUE if handle was freed, FALSE if it wasn't.

Notes :
    A FALSE value can mean either that the handle was invalid or that it is a
    special handle that can't be freed.
--*/
BOOL HMGRFreeHandle(HANDLE handle);

/*++
Function :
    HMGRLockHandle

    Get the information associated with a handle, protect the handle from
    premature closing

Parameters :
    HANDLE handle : handle to lock.

Return Value :
    The pointer originally passed to HMGRGetHandle, or NULL on failure.
--*/
HOBJSTRUCT *HMGRLockHandle(HANDLE handle);

/*++
Function :
    HMGRLockHandle2

    Get the information associated with a handle, protect the handle from
    premature closing

Parameters :
    HANDLE handle : handle to lock.
    HOBJTYPE otype : type of the handle

Return Value :
    The pointer originally passed to HMGRGetHandle, or NULL on failure.
--*/
HOBJSTRUCT *HMGRLockHandle2(HANDLE handle, HOBJTYPE otype);

/*++
Function :
    HMGRUnlockHandle

    Release a handle lock acquired with HMGRLockHandle

Parameters :
    HANDLE handle : handle to unlock.
    HOBJSTRUCT *handle_data : handle object returned by HMGRLockHandle 
                              (for validation)

(no return value)
--*/
void HMGRUnlockHandle(HANDLE handle, HOBJSTRUCT *handle_data);


/*++
Function :
    HMGRReplaceHandleData

    Replace the handle data with a new _file object

Parameters :
    HANDLE handle : handle to be replaced.
    HOBJSTRUCT *handle_data : _file handle object

Return value:
    TRUE if successful, otherwise False
--*/            
BOOL HMGRReplaceHandleData(HANDLE handle, HOBJSTRUCT *handle_data);
#endif /* _PAL_HANDLE_H_ */













