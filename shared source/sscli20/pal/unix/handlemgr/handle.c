/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    handle.c

Abstract:

    Implementation of the handle manager for kernel objects.

--*/

#include "pal/palinternal.h"
#include "pal/critsect.h"
#include "pal/dbgmsg.h"
#include "pal/handle.h"
#include "pal/thread.h"

#include "pal/sync.h"
#include "pal/mutex.h"

#include <limits.h>
#include <errno.h>

SET_DEFAULT_DEBUG_CHANNEL(HANDLE);


/* macro-definitions */

#define BASIC_GROWTH_RATE 1024

/* HANDLEs are multiples of 4 and there is no handle 0 */
/* use these to convert between HANDLEs and indexes */
#define HANDLE_TO_INDEX(h) ((((INT_PTR)(h))>>2)-1)
#define INDEX_TO_HANDLE(i) ((HANDLE)(((i)+1)<<2))

/* max. handle is 0xFFFFFFFC (highest possible 32-bit multiple of 4) */
/* max. index is (max. handle)>>2 -1; */
/* we define it here to avoid signed/unsigned confusion */
/*                                                                      
                                                      */
#define MAX_INDEX 0x3FFFFFFE

/* type definitions */

/* structure to manage handle locks */
struct HANDLE_SLOT
{
    HOBJSTRUCT *object; /* object data associated to a handle, or if the
                           HANDLE_SLOT is unallocated, this is the index
                           of the next unallocated HANDLE_SLOT.  For
                           unallocated slots, the lock_count is -1. */
    short closing;      /* TRUE if CloseHandle has been called on a handle */
    short lock_count;   /* number of locks currently held on a handle */
};

/* static function prototypes */

static BOOL close_special_handle(HANDLE hObject);
static BOOL duplicate_special_handle(HANDLE hSourceHandle,
                                     LPHANDLE lpTargetHandle);
static BOOL HMGRIsSpecialHandle(HANDLE handle);
static BOOL HMGRValidateHandle(HANDLE handle);

/*static variables*/

/* critical section for handle table functions */
static CRITICAL_SECTION hmgr_critsec;

/* variables to manage the handle pool */
static INT pool_growth;
static INT free_handle_list=-1; /* Index of first free HANDLE, -1 if none free */
static INT free_handle_list_tail=-1; /* Implements FILO handle management, the last free HANDLE slot  */

/* variables to manage the handle table */
static INT table_growth;
static INT handle_table_size=0;
static struct HANDLE_SLOT *handle_table=NULL;

/* Constants */
/* Special handles */
/* Pseudo handles constant for current thread and process */
const HANDLE hPseudoCurrentProcess = (HANDLE) 0xFFFFFF01;
const HANDLE hPseudoCurrentThread  = (HANDLE) 0xFFFFFF03;
/* Pseudo handle constant for the global IO Completion port */
const HANDLE hPseudoGlobalIOCP  = (HANDLE) 0xFFFFFF05;



/* API Functions **************************************************************/

/*++
Function:
  DuplicateHandle

See MSDN doc.

PAL-specific behavior :
    -Source and Target process needs to be the current process.
    -lpTargetHandle must be non-NULL
    -dwDesiredAccess is ignored
    -bInheritHandle must be FALSE
    -dwOptions must be a combo of DUPLICATE_SAME_ACCESS and
               DUPLICATE_CLOSE_SOURCE

--*/
BOOL
PALAPI
DuplicateHandle(
        IN HANDLE hSourceProcessHandle,
        IN HANDLE hSourceHandle,
        IN HANDLE hTargetProcessHandle,
        OUT LPHANDLE lpTargetHandle,
        IN DWORD dwDesiredAccess,
        IN BOOL bInheritHandle,
        IN DWORD dwOptions)
{
    DWORD source_process_id;
    DWORD target_process_id;
    DWORD cur_process_id;

    HOBJSTRUCT *handle_data = NULL;
    int retval;
    BOOL bRet = FALSE;

    PERF_ENTRY(DuplicateHandle);
    ENTRY("DuplicateHandle( hSrcProcHandle=%p, hSrcHandle=%p, "
          "hTargetProcHandle=%p, lpTargetHandle=%p, dwAccess=%#x, "
          "bInheritHandle=%d, dwOptions=%#x) \n", hSourceProcessHandle,
          hSourceHandle, hTargetProcessHandle, lpTargetHandle,
          dwDesiredAccess, bInheritHandle, dwOptions);

    cur_process_id=GetCurrentProcessId();
    source_process_id = PROCGetProcessIDFromHandle(hSourceProcessHandle);
    target_process_id = PROCGetProcessIDFromHandle(hTargetProcessHandle);

    /* Check validity of process handles */
    if(0 == source_process_id || 0 == target_process_id)
    {
        ASSERT("Can't duplicate handle: invalid source or destination process");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto DuplicateExit;
    }

    /* At least source or target process should be the current process. */
    if(source_process_id!=cur_process_id &&
       target_process_id!=cur_process_id)
    {
        ASSERT("Can't duplicate handle : neither source or destination"
               "processes are from current process");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto DuplicateExit;
    }

    if(bInheritHandle!=FALSE)
    {
        ASSERT("Can't duplicate handle : bInheritHandle is not FALSE.\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto DuplicateExit;
    }

    if(dwOptions & ~(DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE))
    {
        ASSERT(
            "Can't duplicate handle : dwOptions is %#x which is not "
            "a subset of (DUPLICATE_SAME_ACCESS|DUPLICATE_CLOSE_SOURCE) "
            "(%#x).\n",
            dwOptions,
            DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto DuplicateExit;
    }
    if(!(dwOptions & DUPLICATE_SAME_ACCESS))
    {
        ASSERT(
            "Can't duplicate handle : dwOptions is %#x which does not "
            "include DUPLICATE_SAME_ACCESS (%#x).\n",
            dwOptions,
            DUPLICATE_SAME_ACCESS);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto DuplicateExit;
    }

    if(!lpTargetHandle)
    {
        ASSERT("Can't duplicate handle : lpTargetHandle is NULL.\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto DuplicateExit;
    }

    if(HMGRIsSpecialHandle(hSourceHandle))
    {
        TRACE("Duplicating a special handle(%p)\n", hSourceHandle);
        bRet = duplicate_special_handle(hSourceHandle, lpTargetHandle);

        /* duplicate_special_handle will set last error */

        goto DuplicateExit;
    }

    /* Since handles can be remoted to others processes using PAL_LocalHsndleToRemote
       and PAL_RemoteHandleToLocal, DuplicateHandle needs some special handling
       when this scenario occurs.

       if hSourceProcessHandle is from another process OR
       hTargetProcessHandle is from another process but both aren't
       ( handled above ) return hSourceHandle.
    */
    if(source_process_id!=cur_process_id || target_process_id!=cur_process_id)
    {
        *lpTargetHandle = hSourceHandle;
        bRet = TRUE;
        goto DuplicateExit;
    }

    /* keep the handle locked until the duplicate is created, to prevent the
       source handle from being closed and the object destroyed before the
       duplication adds a reference to it */
    handle_data=HMGRLockHandle(hSourceHandle);
    if(NULL == handle_data)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        goto DuplicateExit;
    }

    *lpTargetHandle=HMGRGetHandle(handle_data);
    if(NULL == *lpTargetHandle)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto DuplicateExit;
    }
    retval=handle_data->dup_handle(*lpTargetHandle, handle_data);

    switch(retval)
    {
    case 0:
        bRet = TRUE;
        goto DuplicateExit;
    case -1:
        ERROR("Object validation failed!\n", retval);
        SetLastError(ERROR_INVALID_HANDLE);
        break;
    case -2:
        ASSERT("Encountered Shared Memory corruption!!\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        break;
    default:
        ASSERT("Type-specific dup_handle returned unknown error code %d\n",
              retval);
        SetLastError(ERROR_INTERNAL_ERROR);
        break;
    }

    /* if we get here, duplication failed.*/
    HMGRFreeHandle(*lpTargetHandle);
    *lpTargetHandle=NULL;

DuplicateExit:

    /* close the handle while it's locked */
    if (dwOptions & DUPLICATE_CLOSE_SOURCE)
    {
        TRACE ("DuplicateHandle closing source handle %p\n", hSourceHandle);
        CloseHandle (hSourceHandle);
    }

    /* unlock the source handle if it was locked */
    if(NULL != handle_data)
    {
        HMGRUnlockHandle(hSourceHandle, handle_data);
    }

    LOGEXIT("DuplicateHandle returns BOOL %d\n", bRet);
    PERF_EXIT(DuplicateHandle);
    return bRet;
}


/*++
Function:
  CloseHandle

See MSDN doc.

Note : according to MSDN, FALSE is returned in case of error. But also
according to MSDN, closing an invalid handle raises an exception when running a
debugger [or, alternately, if a special registry key is set]. This behavior is
not required in the PAL, so we'll always return FALSE.
--*/
BOOL
PALAPI
CloseHandle(
        IN OUT HANDLE hObject)
{
    HOBJSTRUCT *handle_struct;
    HOBJTYPE handle_type;
    BOOL bRet = FALSE;
    int handle_index;
    PERF_ENTRY(CloseHandle);
    ENTRY("CloseHandle (hObject=%p) \n", hObject);

    if (HMGRIsSpecialHandle(hObject))
    {
        TRACE("CloseHandle called on a special handle (%p)\n", hObject);
        bRet = close_special_handle(hObject);
        goto CloseExit;
    }

    SYNCEnterCriticalSection(&hmgr_critsec, TRUE);

    /* Get the handle's information structure, do some validation */

    handle_struct=HMGRLockHandle(hObject);
    if(!handle_struct)
    {
        TRACE("CloseHandle called with invalid handle %p\n", hObject);
        SetLastError(ERROR_INVALID_HANDLE);
        SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
        goto CloseExit;
    }

    handle_index = HANDLE_TO_INDEX(hObject);
    /* indicate we want to close the handle. HMGRLockHandle calls will now fail,
       and HMGRUnlockHandle will close the handle when the lock count reaches
       zero [right now if we have the only lock] */
    handle_table[handle_index].closing = TRUE;
    handle_type = handle_struct->type;

    /* leave the critical section before unlocking to avoid potential deadlocks:
       Unlock may invoke a closehandle callback, which may need to enter a
       different critical section */
    SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);

    HMGRUnlockHandle(hObject,handle_struct);

    bRet = TRUE;
    TRACE("Closing handle %p of type %d.\n", hObject, handle_type);

CloseExit:
    LOGEXIT("CloseHandle returns BOOL %d\n", bRet);
    PERF_EXIT(CloseHandle);
    return bRet;
}

/* PAL Internal functions *****************************************************/

/*++
Function :
    HMGRInitHandleManager

    Initialize the handle manager.

    (no parameters)

Return value :
    TRUE on success, FALSE on failure
--*/
BOOL HMGRInitHandleManager(void)
{
    INT_PTR i;

    if(handle_table)
    {
        WARN("Trying to reinitialize the handle manager : not good.\n");
        return FALSE;
    }

    if (0 != SYNCInitializeCriticalSection(&hmgr_critsec))
    {
      WARN("Unable to initialize the handle manager critical section\n")
      return FALSE;
    }

    SYNCEnterCriticalSection(&hmgr_critsec, TRUE);

    pool_growth=table_growth=BASIC_GROWTH_RATE;

    /* initialize the handle table - the free list is stored in the 'object'
       field, with the head in the global 'free_handle_list'. */
    handle_table_size=table_growth;
    handle_table=malloc(handle_table_size * sizeof(struct HANDLE_SLOT));
    if(!handle_table)
    {
        ERROR("failed to allocate %d bytes! system resources at critical level!\n",
              handle_table_size * sizeof(LPVOID));
        SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
        DeleteCriticalSection(&hmgr_critsec);
        return FALSE;
    }

    for(i=0;i< handle_table_size; i++)
    {
        handle_table[i].object=(HOBJSTRUCT*)(i+1);
        handle_table[i].lock_count = -1;
        handle_table[i].closing = FALSE;
    }
    handle_table[handle_table_size-1].object = (HOBJSTRUCT*)-1;
    free_handle_list = 0;
    free_handle_list_tail = handle_table_size-1;

    TRACE("Handle Manager initialization complete.\n");

    SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
    return TRUE;
}

/*++
Function :
    HMGRStopHandleManager

    Free all resources used by the Handle manager; close all handles.

    (no parameters)

Return value :
    TRUE on success, FALSE on failure
--*/
BOOL HMGRStopHandleManager(void)
{
    INT i;
    HOBJSTRUCT *hs;
    BOOL entered_cs;
    if(!handle_table)
    {
        WARN("Trying to stop a stopped Handle Manager!\n");
        return FALSE;
    }

    /* we don't really want to enter the critical section : all other threads
       should have been terminated, so there's no need. (And if a thread was
       killed while holding the critical section, this might deadlock) */
    entered_cs = TryEnterCriticalSection(&hmgr_critsec);
    if(!entered_cs)
    {
        WARN("Handle manager critical section is held by another thread!\n");
    }

    /* Close the handles in reverse order, so the initial process handle
       is closed at the end */
    for(i=handle_table_size-1;i>=0;i--)
    {
        hs=handle_table[i].object;
        /* if the handle isn't already free, close it */
        if (handle_table[i].lock_count != -1 && hs && hs->close_handle)
        {
            hs->close_handle(hs);
        }
    }
    free(handle_table);
    handle_table=NULL;
    free_handle_list=-1;
    free_handle_list_tail = -1;

    if(entered_cs)
    {
        LeaveCriticalSection(&hmgr_critsec);
    }
    DeleteCriticalSection(&hmgr_critsec);

    TRACE("Handle Manager stopped.\n");

    return TRUE;
}


/*++
Function :
    HMGRGetHandle

    Allocate a handle from the Free Handle Pool

Parameters :
    HANDLESTRUCT handle_data : information about the handle.

Return Value :
    Value of allocated handle, or INVALID_HANDLE_VALUE (-1) on failure.
--*/
HANDLE HMGRGetHandle(HOBJSTRUCT *handle_data)
{
    HANDLE new_handle;
    INT_PTR index;

    if(!handle_table)
    {
        ASSERT("Handle Manager is not initialized!\n");
        return INVALID_HANDLE_VALUE;
    }

    if(!handle_data)
    {
        ASSERT( "handle_data is NULL.\n" );
        return INVALID_HANDLE_VALUE;
    }

    if(handle_data->type<=HOBJ_INVALID || handle_data->type>=HOBJ_LAST)
    {
        ASSERT( "The handle type %d is not a known handle type.\n",
                handle_data->type );
        return INVALID_HANDLE_VALUE;
    }

    SYNCEnterCriticalSection(&hmgr_critsec, TRUE);

    /* if no free handles are available, we need to grow the handle table and
       add new handles to the pool */
    if(free_handle_list == -1)
    {
        struct HANDLE_SLOT *temp_table;
        INT_PTR i;

        TRACE("Handle pool empty (%d handles allocated), growing handle table "
              "by %d entries.\n", handle_table_size, table_growth );

        /* make sure handle values don't overflow */
        if(handle_table_size+table_growth>=MAX_INDEX)
        {
            WARN("Unable to allocate handle : maximum (%d) reached!\n",
                 handle_table_size);
            SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
            return INVALID_HANDLE_VALUE;
        }

        /* grow handle table */
        temp_table=realloc(handle_table,
                 (handle_table_size + table_growth)*sizeof(struct HANDLE_SLOT));
        if(!temp_table)
        {
            WARN("not enough memory to grow handle table!\n");
            SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
            return INVALID_HANDLE_VALUE;
        }
        handle_table=temp_table;

        /* update handle table and handle pool */
        for(i=0; i<table_growth; i++)
        {
            /* new handles are initially invalid */
            /* the last "old" handle was handle_table_size-1, so the new
               handles range from handle_table_size to
               handle_table_size+table_growth-1 */
            handle_table[handle_table_size+i].object=(HOBJSTRUCT*)(handle_table_size+i+1);
            handle_table[handle_table_size+i].lock_count = -1;
            handle_table[handle_table_size+i].closing = FALSE;
        }
        handle_table[handle_table_size+table_growth-1].object = (HOBJSTRUCT*)-1;
        free_handle_list = handle_table_size;
        free_handle_list_tail = handle_table_size+table_growth-1;
        handle_table_size+=table_growth;
    }

    /* take the next free handle */
    index=free_handle_list;

    /* remove the handle from the pool */
    free_handle_list = (INT_PTR)handle_table[index].object;
    
    /* clear the tail record if this is the last handle slot available */
    if(free_handle_list == -1) 
    {
        free_handle_list_tail = -1;
    }

    /* save the data associated with the new handle */
    new_handle = INDEX_TO_HANDLE(index);
    handle_table[index].object=handle_data;
    handle_table[index].closing = FALSE;
    handle_table[index].lock_count = 0;

    SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
    TRACE("Allocating new handle %p (data is %p)\n",
          new_handle, handle_data);
    return new_handle;
}

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
BOOL HMGRFreeHandle(HANDLE handle)
{
    INT_PTR handle_index=HANDLE_TO_INDEX(handle);

    TRACE( "Entered HMGRFreeHandle.\n" );
    if(!handle_table)
    {
        ASSERT("Handle Manager is not initialized!\n");
        return FALSE;
    }

    SYNCEnterCriticalSection(&hmgr_critsec, TRUE);

    if(!HMGRValidateHandle(handle))
    {
        ASSERT("Trying to free invalid handle %p.\n", handle);
        SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
        return FALSE;
    }

    if(HMGRIsSpecialHandle(handle))
    {
        ASSERT("Trying to free Special Handle %p.\n", handle);
        SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
        return FALSE;
    }

    if(0<handle_table[handle_index].lock_count)
    {
        WARN("Freeing a handle with a positive lock count %d!\n",
             handle_table[handle_index].lock_count );
    }
    handle_table[handle_index].lock_count=-1;
    handle_table[handle_index].closing = FALSE;

    /* add handle to the free pool */
    if(free_handle_list_tail != -1)
    {
        handle_table[free_handle_list_tail].object = (HOBJSTRUCT*)handle_index;
    }
    else
    {
        free_handle_list = handle_index;
    }
    handle_table[handle_index].object = (HOBJSTRUCT*) -1;
    free_handle_list_tail = handle_index;

    SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
    TRACE("Handle %p returned to the handle pool\n",handle);
    return TRUE;
}

/*++
Function :
    HMGRValidateHandle

    Check if a handle is valid

Parameters :
    HANDLE handle : handle to check.

Return Value :
    TRUE if valid, FALSE if invalid.
--*/
static BOOL HMGRValidateHandle(HANDLE handle)
{
    int handle_int;
    HOBJSTRUCT *handle_data;

    if(!handle_table)
    {
        ASSERT("Handle Manager is not initialized!\n");
        return FALSE;
    }
    if(handle==INVALID_HANDLE_VALUE || handle == 0)
    {
        TRACE( "INVALID_HANDLE_VALUE or NULL value is not a valid handle.\n" );
        return FALSE;
    }

    if(HMGRIsSpecialHandle(handle))
    {
        TRACE( "Handle %p is a special handle, returning TRUE.\n", handle );
        return TRUE;
    }

    handle_int=HANDLE_TO_INDEX(handle);

    if(handle_int<0 || handle_int>=handle_table_size)
    {
        WARN( "The handle value(%p) is out of the bounds for the handle table.\n", handle );
        return FALSE;
    }

    handle_data=handle_table[handle_int].object;
    if(handle_table[handle_int].lock_count != -1)
    {
	    if (handle_data != NULL)
	    {
            if(handle_data->type<=HOBJ_INVALID || handle_data->type>=HOBJ_LAST)
            {
                ASSERT( "The handle %p is not a known handle type.\n", handle );
                handle_data=NULL;
            }
	    }
        else
	    {
	        TRACE( "The handle %p points to NULL data.\n", handle );
	    }
    }
    else
    {
        TRACE( "The handle %p points a free HANDLE.\n", handle );
	    handle_data = NULL;
    }

    return (handle_data!=NULL);
}


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
HOBJSTRUCT *HMGRLockHandle(HANDLE handle)
{
    HOBJSTRUCT *handle_data = NULL;
    int index;

    if(!handle_table)
    {
        ASSERT("Handle Manager is not initialized!\n");
        return NULL;
    }

    if(HMGRIsSpecialHandle(handle))
    {
        ERROR("Tried to lock a special handle!\n");
        return NULL;
    }

    SYNCEnterCriticalSection(&hmgr_critsec, TRUE);

    if(!HMGRValidateHandle(handle))
    {
        ERROR( "Tried to lock an invalid handle %p\n", handle );
        SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
        return NULL;
    }
    index = HANDLE_TO_INDEX(handle);
    if(handle_table[index].closing && 0 == handle_table[index].lock_count)
    {
        /* if lock count is zero and handle is flagged as "closing", the
           close_handle callback is already in progress. we should never get
           here, HMGRValidateHandle should have recognized the handle as being
           invalid */
        ASSERT( "Trying to lock a handle that is being closed\n" );
    }
    else if(SHRT_MAX == handle_table[index].lock_count) /* paranoid check */
    {
        ASSERT("Handle lock count is maxed out!!!\n");
    }
    else
    {
        if(handle_table[index].closing)
        {
            /* this means that although the handle has been flagged as closing,
               the PAL was still holding at least 1 lock on it. This can, for
               example, happen if one application thread closes the handle after
               another thread passed it to a PAL function which has locked it
               and is still using it.  Asking for more locks is acceptable; it
               can, for example, mean that the function holding the lock had to
               call a function that takes a handle instead of a handle object
               (and therefore needs to lock it too). however this should be
               rare, so let's output a warning so we know when this happens */
            WARN( "Locking a handle when there is already a close request\n" );
        }

        handle_table[index].lock_count++;
        handle_data=handle_table[index].object;
        TRACE("Handle %p locked; lock count is now %d\n",
              handle, handle_table[index].lock_count);
    }
    SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
    return handle_data;
}


HOBJSTRUCT *HMGRLockHandle2(HANDLE handle, HOBJTYPE otype)
{
    HOBJSTRUCT *handle_data = NULL;

    if(otype<=HOBJ_INVALID || otype>=HOBJ_LAST)
    {
        ASSERT("Illegal HOBJTYPE!\n");
        return NULL;
    }

    handle_data=HMGRLockHandle(handle);

    if(NULL!=handle_data)
    {
        if(handle_data->type!=otype)
        {
            HMGRUnlockHandle(handle, handle_data);
            return NULL;
        }
    }

    return handle_data;
}

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
void HMGRUnlockHandle(HANDLE handle, HOBJSTRUCT *handle_data)
{
    int index;
    INT retval;

    SYNCEnterCriticalSection(&hmgr_critsec, TRUE);
    if(!HMGRValidateHandle(handle))
    {
        ASSERT("trying to unlock an invalid handle!!!\n");
        goto done;
    }
    index = HANDLE_TO_INDEX(handle);
    if(handle_table[index].object != handle_data)
    {
        ASSERT("Mismatch between handle and handle data!\n");
        goto done;
    }
    if(handle_table[index].lock_count==0)
    {
        ASSERT("Handle isn't locked!\n");
        goto done;
    }
    handle_table[index].lock_count--;
    if(FALSE == handle_table[index].closing ||
       handle_table[index].lock_count != 0)
    {
        TRACE("Handle %p unlocked, lock count is now %d.\n",
              handle, handle_table[index].lock_count );
        goto done;
    }
    /* handle is closing, lock count is 0 : we can close */

    TRACE("Handle %p unlocked, closing handle\n", handle);

    /*Close the handle : free the handle, then call callback function */

    HMGRFreeHandle(handle);

    /* leave the critical section before calling the callback to avoid
       potential deadlocks */
    SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);

    retval=handle_data->close_handle(handle_data);

    switch(retval)
    {
    case 0:
        break;
    case -1:
        ASSERT("Object validation failed!, errno is %d (%s)\n", errno, strerror(errno));
        SetLastError(ERROR_INVALID_HANDLE);
        break;
    case -2:
        ASSERT("Encountered Shared Memory corruption!!\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        break;
    default:
        ASSERT("Type-specific close_handle returned unknown error code %d\n",
              retval);
        SetLastError(ERROR_INTERNAL_ERROR);
        break;
    }
    /* we already left the critical section */
    return;
done:
    SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
}

/*++
Function :
    HMGRIsSpecialHandle

    Check if a handle is one of the "special" handles

Parameters :
    HANDLE handle : handle to check.

Return Value :
    TRUE if it's a special handle, FALSE otherwise.
--*/
BOOL HMGRIsSpecialHandle(HANDLE handle)
{
    /* the pseudo handle for current thread, current process, and
       global IO completion port
       are considered as special handles */
    if ((handle == hPseudoCurrentThread) ||
        (handle == hPseudoCurrentProcess) ||
        (handle == hPseudoGlobalIOCP))
        return TRUE;

    return FALSE;
}

/* static functions ***********************************************************/

/*++
Function:
  duplicate_special_handle

  Duplicate a special handle. Special handles are fixed values for
  current process, current thread, etc.
  This function set the last error.

Parameters :
    IN hSourceHandle:   Special handle to duplicate.
    OUT lpTargetHandle: Output handle

Return Value :
    TRUE if source handle is duplicated sucessfully
    FALSE otherwise

--*/
static BOOL duplicate_special_handle(
    HANDLE hSourceHandle,
    LPHANDLE lpTargetHandle)
{
    if (hSourceHandle == hPseudoCurrentProcess)
    {
        HANDLE hCurrentProcess;

        hCurrentProcess = PROCGetRealCurrentProcess();

        return DuplicateHandle(GetCurrentProcess(), hCurrentProcess,
                               GetCurrentProcess(), lpTargetHandle,
                               0, FALSE, DUPLICATE_SAME_ACCESS);
    }
    else if (hSourceHandle == hPseudoCurrentThread)
    {
        HANDLE hCurrentThread;
        hCurrentThread = PROCGetRealCurrentThread();

        return DuplicateHandle(GetCurrentProcess(), hCurrentThread,
                               GetCurrentProcess(), lpTargetHandle,
                               0, FALSE, DUPLICATE_SAME_ACCESS);
    }

    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
}


/*++
Function:
  close_special_handle

  Close a special handle. Special handles are fixed values for
  current process, current thread, etc.
  This functions sets the last error.

Parameters :
    IN hSourceHandle:   Special handle to close.

Return Value :
    TRUE if source handle is closed sucessfully
    FALSE otherwise

--*/
static BOOL close_special_handle(
    HANDLE hObject)
{
    if ((hObject == hPseudoCurrentThread) ||
        (hObject == hPseudoCurrentProcess))
    {
        /* do nothing, just return true */
        return TRUE;
    }

    SetLastError(ERROR_INVALID_HANDLE);
    return FALSE;
}

PALIMPORT
RHANDLE
PALAPI
PAL_LocalHandleToRemote(IN HANDLE hLocal)
{
    RHANDLE hRemote = INVALID_HANDLE_VALUE;
    SHMPTR ShmRemoteObject = 0;
    HREMOTEOBJSTRUCT* remote_handle_data = NULL;
    HOBJSTRUCT *handle_data = NULL;

    PERF_ENTRY(PAL_LocalHandleToRemote);
    ENTRY("PAL_LocalHandleToRemote( hLocal=0x%lx )\n", hLocal);

    handle_data = HMGRLockHandle(hLocal);
    if (NULL == handle_data)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        goto Exit;
    }

    SHMLock();
    ShmRemoteObject = SHMalloc( sizeof(HREMOTEOBJSTRUCT) );
    if(ShmRemoteObject)
    {
        BOOL success=FALSE;
        remote_handle_data =
                    (HREMOTEOBJSTRUCT*)SHMPTR_TO_PTR( ShmRemoteObject );
        remote_handle_data->type = handle_data->type;
        switch(remote_handle_data->type)
        {
            case HOBJ_MUTEX:
               success=MutexLocalToRemote(handle_data, remote_handle_data);
               break;
            case HOBJ_PROCESS:
                success=ProcessLocalToRemote(handle_data, remote_handle_data);
                break;
            case HOBJ_EVENT:
                success=EventLocalToRemote(handle_data, remote_handle_data);
                break;
            default:
                ASSERT("LocalHandleToRemote cannot be performed on this type"
                      "of handle");
                SHMfree(ShmRemoteObject);
                break;
        }
        if(success)
        {
            hRemote = (RHANDLE) ShmRemoteObject;
        }
        else
        {
            ERROR("Invalid shared memory pointer\n");
            SHMfree(ShmRemoteObject);
        }

    }
    else
    {
        ERROR( "Unable to allocate any shared memory.\n" );
    }
    SHMRelease();
    HMGRUnlockHandle(hLocal, handle_data);

Exit:
    LOGEXIT("PAL_LocalHandleToRemote returns RHANDLE 0x%lx\n", hRemote);
    PERF_EXIT(PAL_LocalHandleToRemote);
    return hRemote;
}

PALIMPORT
HANDLE
PALAPI
PAL_RemoteHandleToLocal(IN RHANDLE hRemote)
{
    HANDLE hLocal = INVALID_HANDLE_VALUE;
    HREMOTEOBJSTRUCT* remote_handle_data;

    PERF_ENTRY(PAL_RemoteHandleToLocal);
    ENTRY("PAL_RemoteHandleToLocal( hRemote=0x%lx )\n", hRemote);

    if (hRemote == NULL || hRemote == INVALID_HANDLE_VALUE)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        goto Exit;
    }

    SHMLock();
    remote_handle_data =
                    (HREMOTEOBJSTRUCT*) SHMPTR_TO_PTR( (SHMPTR) hRemote );

    if( NULL == remote_handle_data )
    {
        ASSERT("Invalid remote handle hRemote=%#x", hRemote);
        SetLastError(ERROR_INVALID_HANDLE);
        SHMRelease();
        goto Exit;
    }

    switch(remote_handle_data->type)
    {
        case HOBJ_MUTEX:
            hLocal = MutexRemoteToLocal( remote_handle_data );
            break;
        case HOBJ_PROCESS:
            hLocal = ProcessRemoteToLocal( remote_handle_data );
            break;
        case HOBJ_EVENT:
            hLocal = EventRemoteToLocal( remote_handle_data );
            break;
        default:
            ERROR("LocalHandleToRemote cannot be performed on this type"
                  "of handle");
            break;
    }
    SHMfree( (SHMPTR) hRemote );
    SHMRelease();

Exit:
    LOGEXIT("PAL_RemoteHandleToLocal returns HANDLE 0x%lx\n", hLocal);
    PERF_EXIT(PAL_RemoteHandleToLocal);
    return hLocal;
}

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

BOOL HMGRReplaceHandleData(HANDLE handle, HOBJSTRUCT *handle_data)
{
    int index;

    if(!handle_table)
    {
        ASSERT("Handle Manager is not initialized!\n");
        return FALSE;
    }

    SYNCEnterCriticalSection(&hmgr_critsec, TRUE);

    if(!HMGRValidateHandle(handle))
    {
        ERROR( "Tried to lock an invalid handle %p\n", handle );
        SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);
        return FALSE;
    }
    index = HANDLE_TO_INDEX(handle);
    handle_table[index].object = handle_data;

    SYNCLeaveCriticalSection(&hmgr_critsec, TRUE);

    return TRUE;
}

#ifdef _DEBUG
/*++
Function :
    HMGRListThreadInfo

    Iterate through the handle table to print out the
    information of each thread handle.

    This is a handy tool to debug threading issues inside gdb.

--*/  
VOID HMGRListThreadInfo()
{
    HOBJSTRUCT *handle_data;
    THREAD *lpThread = NULL;
    int i;

    if (handle_table == NULL)
        return;

    for (i = 0; i < handle_table_size; i++)
    {
        handle_data=handle_table[i].object;
        if (handle_table[i].lock_count != -1)
        {
            if (handle_data != NULL && handle_data->type == HOBJ_THREAD)
            {
                lpThread = (THREAD*) handle_data;
                printf("HANDLE=%d, dwThreadId=%d, thread_state=%d, blockingPipe=%d, lpStartAddress=%p, lpStartParameter=%p, threadPriority=%d\n",
                    (int)lpThread->hThread,
                    lpThread->dwThreadId,
                    lpThread->thread_state,
                    lpThread->blockingPipe,
                    lpThread->lpStartAddress,
                    lpThread->lpStartParameter,
                    lpThread->threadPriority);
            }
        }  
    }
}
#endif
