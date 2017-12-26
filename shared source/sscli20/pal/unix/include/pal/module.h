/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/module.h

Abstract:
    Header file for modle management utilities.

--*/

#ifndef _PAL_MODULE_H_
#define _PAL_MODULE_H_

#define PAL_SHLIB_PREFIX "lib"

#if __APPLE__
#define PAL_SHLIB_SUFFIX ".dylib"
#else
#define PAL_SHLIB_SUFFIX ".so"
#endif

typedef BOOL (__stdcall *PDLLMAIN)(HINSTANCE,DWORD,LPVOID); /* entry point of module */

typedef struct _MODSTRUCT
{
    HMODULE self;         /* circular reference to this module */
    void *dl_handle;      /* handle returned by dlopen() */
    LPWSTR lib_name;      /* full path of module */
    INT refcount;         /* reference count */
                          /* -1 means infinite reference count - module is never released */
    BOOL ThreadLibCalls;  /* TRUE for DLL_THREAD_ATTACH/DETACH notifications 
                              enabled, FALSE if they are disabled */

#if RETURNS_NEW_HANDLES_ON_REPEAT_DLOPEN
    ino_t inode;
    dev_t device;
#endif

    PDLLMAIN pDllMain; /* entry point of module */

    /* reference to next and previous modules in list (in load order) */
    struct _MODSTRUCT *next;
    struct _MODSTRUCT *prev;
} MODSTRUCT;

extern MODSTRUCT pal_module;


/*++
Function :
    LoadInitializeModules

    Initialize the process-wide list of modules (2 initial modules : 1 for
    the executable and 1 for the PAL)

Parameters :
    LPWSTR exe_name : full path to executable

Return value :
    TRUE on success, FALSE on failure

Notes :
    the module manager takes ownership of the string
--*/
BOOL LOADInitializeModules(LPWSTR exe_name);

/*++
Function :
    LOADFreeModules

    Release all resources held by the module manager (including dlopen handles)

Parameters:
    BOOL bTerminateUnconditionally: If TRUE, this will avoid calling any DllMains

    (no return value)
--*/
void LOADFreeModules(BOOL bTerminateUnconditionally);

/*++
Function :
    LOADCallDllMain

    Call DllMain for all modules (that have one) with the given "fwReason"

Parameters :
    DWORD dwReason : parameter to pass down to DllMain, one of DLL_PROCESS_ATTACH, DLL_PROCESS_DETACH, 
        DLL_THREAD_ATTACH, DLL_THREAD_DETACH

    LPVOID lpReserved : parameter to pass down to DllMain
        If dwReason is DLL_PROCESS_ATTACH, lpvReserved is NULL for dynamic loads and non-NULL for static loads.
        If dwReason is DLL_PROCESS_DETACH, lpvReserved is NULL if DllMain has been called by using FreeLibrary 
            and non-NULL if DllMain has been called during process termination. 

(no return value)

Notes :
    This is used to send DLL_THREAD_*TACH messages to modules
--*/
void LOADCallDllMain(DWORD dwReason, LPVOID lpReserved);

/*++
Function:
  LockModuleList

Abstract
  Enter the critical section associated to the module list

Parameter
  void

Return
  void
--*/
void LockModuleList();

/*++
Function:
  UnlockModuleList

Abstract
  Leave the critical section associated to the module list

Parameter
  void

Return
  void
--*/
void UnlockModuleList();


/*++
    LOADGetLibRotorPalSoFileName

    Retrieve the full path of the librotor_pal.so being used.

Parameters:
    OUT pwzBuf - WCHAR buffer of MAX_PATH length to receive file name

Return value:
    0 if successful
    -1 if failure, with last error set.
--*/
int LOADGetLibRotorPalSoFileName(LPSTR pszBuf);

#endif /* _PAL_MODULE_H_ */

