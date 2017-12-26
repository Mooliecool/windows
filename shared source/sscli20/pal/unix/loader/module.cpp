/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    module.c

Abstract:

    Implementation of module related functions in the Win32 API

--*/

extern "C" {

#include "config.h"
#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/module.h"
#include "pal/critsect.h"
#include "pal/thread.h"
#include "pal/file.h"
#include "pal/utils.h"
#include "pal/init.h"
#include "pal/modulename.h"
#include "pal/misc.h"

#include <sys/param.h>
#include <errno.h>
#include <string.h>
#if HAVE_DYLIBS
#include "dlcompat.h"
#else   // HAVE_DYLIBS
#include <dlfcn.h>
#endif  // HAVE_DYLIBS
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif  // HAVE_ALLOCA_H

}

SET_DEFAULT_DEBUG_CHANNEL(LOADER);


/* macro definitions **********************************************************/

/* get the full name of a module if available, and the short name otherwise*/
#define MODNAME(x) ((x)->lib_name)

/* Which path should FindLibrary search? */
#if defined(__APPLE__)
#define LIBSEARCHPATH "DYLD_LIBRARY_PATH"
#else
#define LIBSEARCHPATH "LD_LIBRARY_PATH"
#endif

/* static variables ***********************************************************/

/* critical section that regulates access to the module list */
CRITICAL_SECTION module_critsec;

MODSTRUCT exe_module; /* always the first, in the in-load-order list */
MODSTRUCT pal_module; /* always the second, in the in-load-order list */

/* static function declarations ***********************************************/

static BOOL LOADValidateModule(MODSTRUCT *module);
static LPWSTR LOADGetModuleFileName(MODSTRUCT *module);
static HMODULE LOADLoadLibrary(LPSTR ShortAsciiName, BOOL fDynamic);
static void LOAD_SEH_CallDllMain(MODSTRUCT *module, DWORD dwReason, LPVOID lpReserved);
static MODSTRUCT *LOADAllocModule(void *dl_handle, char *name);
static INT FindLibrary(CHAR* pszRelName, CHAR** ppszFullName);

/* API function definitions ***************************************************/

/*++
Function:
  LoadLibraryA

See MSDN doc.
--*/
HMODULE
PALAPI
LoadLibraryA(
         IN LPCSTR lpLibFileName)
{
    LPSTR lpstr = NULL;
    HMODULE hModule = NULL;

    PERF_ENTRY(LoadLibraryA);
    ENTRY("LoadLibraryA (lpLibFileName=%p (%s)) \n",
          (lpLibFileName)?lpLibFileName:"NULL",
          (lpLibFileName)?lpLibFileName:"NULL");

    if(NULL == lpLibFileName)
    {
        ERROR("lpLibFileName is NULL;Exit.\n");
        SetLastError(ERROR_MOD_NOT_FOUND);
        goto Done;
    }

    if(lpLibFileName[0]=='\0')
    {
        ERROR("can't load library with NULL file name...\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Done;
    }

    /* do the Dos/Unix conversion on our own copy of the name */
    lpstr = strdup(lpLibFileName);
    if(!lpstr)
    {
        ERROR("memory allocation failure!\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Done;
    }
    FILEDosToUnixPathA(lpstr);

    hModule = LOADLoadLibrary(lpstr, TRUE);

    /* let LOADLoadLibrary call SetLastError */
 Done:
    if (lpstr != NULL)
        free(lpstr);

    LOGEXIT("LoadLibraryA returns HMODULE %p\n", hModule);
    PERF_EXIT(LoadLibraryA);
    return hModule;
}


/*++
Function:
  LoadLibraryW

See MSDN doc.
--*/
HMODULE
PALAPI
LoadLibraryW(
         IN LPCWSTR lpLibFileName)
{
    CHAR lpstr[MAX_PATH];
    INT name_length;
    HMODULE hModule = NULL;

    PERF_ENTRY(LoadLibraryW);
    ENTRY("LoadLibraryW (lpLibFileName=%p (%S)) \n",
          lpLibFileName?lpLibFileName:W16_NULLSTRING,
          lpLibFileName?lpLibFileName:W16_NULLSTRING);

    if(NULL == lpLibFileName)
    {
        ERROR("lpLibFileName is NULL;Exit.\n");
        SetLastError(ERROR_MOD_NOT_FOUND);
        goto done;
    }

    if(lpLibFileName[0]==0)
    {
        ERROR("Can't load library with NULL file name...\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    /* do the Dos/Unix conversion on our own copy of the name */

    name_length = WideCharToMultiByte(CP_ACP, 0, lpLibFileName, -1, lpstr,
                                      MAX_PATH, NULL, NULL);
    if( name_length == 0 )
    {
        DWORD dwLastError = GetLastError();
        if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
        {
            ERROR("lpLibFileName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    FILEDosToUnixPathA(lpstr);

    /* let LOADLoadLibrary call SetLastError in case of failure */
    hModule = LOADLoadLibrary(lpstr, TRUE);

done:
    LOGEXIT("LoadLibraryW returns HMODULE %p\n", hModule);
    PERF_EXIT(LoadLibraryW);
    return hModule;
}


/*++
Function:
  GetProcAddress

See MSDN doc.
--*/
FARPROC
PALAPI
GetProcAddress(
           IN HMODULE hModule,
           IN LPCSTR lpProcName)
{
    MODSTRUCT *module;
    FARPROC ProcAddress = NULL;
    LPCSTR symbolName = lpProcName;

    PERF_ENTRY(GetProcAddress);
    ENTRY("GetProcAddress (hModule=%p, lpProcName=%p (%s))\n",
          hModule, lpProcName?lpProcName:"NULL", lpProcName?lpProcName:"NULL");

    LockModuleList();

    module = (MODSTRUCT *) hModule;

    /* parameter validation */

    if( (lpProcName == NULL) || (*lpProcName == '\0') )
    {
        TRACE("No function name given\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    if( !LOADValidateModule( module ) )
    {
        TRACE("Invalid module handle %p\n", hModule);
        SetLastError(ERROR_INVALID_HANDLE);
        goto done;
    }
    
    /* try to assert on attempt to locate symbol by ordinal */
    /* this can't be an exact test for HIWORD((DWORD)lpProcName) == 0
       because of the address range reserved for ordinals contain can
       be a valid string address on non-Windows systems
    */
    if( (DWORD_PTR)lpProcName < VIRTUAL_PAGE_SIZE )
    {
        ASSERT("Attempt to locate symbol by ordinal?!\n");
    }

    // Get the symbol's address.
    
    // If we're looking for a symbol inside the PAL, we try the PAL_ variant
    // first because otherwise we run the risk of having the non-PAL_
    // variant preferred over the PAL's implementation.
    if (module->dl_handle == pal_module.dl_handle)
    {
        LPSTR lpPALProcName = (LPSTR) alloca(4 + strlen(lpProcName) + 1);
        strcpy(lpPALProcName, "PAL_");
        strcat(lpPALProcName, lpProcName);
        ProcAddress = (FARPROC) dlsym(module->dl_handle, lpPALProcName);
        symbolName = lpPALProcName;
    }

    // If we aren't looking inside the PAL or we didn't find a PAL_ variant
    // inside the PAL, fall back to a normal search.
    if (ProcAddress == NULL)
    {
        ProcAddress = (FARPROC) dlsym(module->dl_handle, lpProcName);
    }

    if (ProcAddress)
    {
        TRACE("Symbol %s found at address %p in module %p (named %S)\n",
              lpProcName, ProcAddress, module, MODNAME(module));

        /* if we don't know the module's full name yet, this is our chance to
           obtain it */
        if(!module->lib_name)
        {
            const char* libName = PAL_dladdr((LPVOID)ProcAddress);
            if (libName)
            {
                module->lib_name = UTIL_MBToWC_Alloc(libName, -1);
                if(NULL == module->lib_name)
                {
                    ERROR("MBToWC failure; can't save module's full name\n");
                }
                else
                {
                    TRACE("Saving full path of module %p as %s\n",
                          module, libName);
                }
            }
        }
    }
    else
    {
        TRACE("Symbol %s not found in module %p (named %S)\n",
              lpProcName, module, MODNAME(module), dlerror());
        SetLastError(ERROR_PROC_NOT_FOUND);
    }
done:
    UnlockModuleList();
    LOGEXIT("GetProcAddress returns FARPROC %p\n", ProcAddress);
    PERF_EXIT(GetProcAddress);
    return ProcAddress;
}


/*++
Function:
  FreeLibrary

See MSDN doc.
--*/
BOOL
PALAPI
FreeLibrary(
        IN OUT HMODULE hLibModule)
{
    MODSTRUCT *module;
    BOOL retval = FALSE;

    PERF_ENTRY(FreeLibrary);
    ENTRY("FreeLibrary (hLibModule=%p)\n", hLibModule);

    LockModuleList();

    module = (MODSTRUCT *) hLibModule;

    if (terminator)
    {
        /* PAL shutdown is in progress - ignore FreeLibrary calls */
        retval = TRUE;
        goto done;
    }

    if( !LOADValidateModule( module ) )
    {
        TRACE("Can't free invalid module handle %p\n", hLibModule);
        SetLastError(ERROR_INVALID_HANDLE);
        goto done;
    }

    if( module->refcount == -1 )
    {
        /* special module - never released */
        retval=TRUE;
        goto done;
    }

    module->refcount--;
    TRACE("Reference count for module %p (named %S) decreases to %d\n",
            module, MODNAME(module), module->refcount);

    if( module->refcount != 0 )
    {
        retval=TRUE;
        goto done;
    }

    /* Releasing the last reference : call dlclose(), remove module from the
       process-wide module list */

    TRACE("Reference count for module %p (named %S) now 0; destroying "
            "module structure.\n", module, MODNAME(module));

    /* unlink the module structure from the list */
    module->prev->next = module->next;
    module->next->prev = module->prev;

    /* remove the circular reference so that LOADValidateModule will fail */
    module->self=NULL;

    /* Call DllMain if the module contains one */
    if(module->pDllMain)
    {
        TRACE("Calling DllMain (%p) for module %S\n",
                module->pDllMain, 
                module->lib_name ? module->lib_name : W16_NULLSTRING);

/* reset ENTRY nesting level back to zero while inside the callback... */
#if !_NO_DEBUG_MESSAGES_
    {
        int old_level;
        old_level = DBG_change_entrylevel(0);
#endif /* !_NO_DEBUG_MESSAGES_ */
    
        module->pDllMain((HMODULE)module, DLL_PROCESS_DETACH, NULL);

/* ...and set nesting level back to what it was */
#if !_NO_DEBUG_MESSAGES_
        DBG_change_entrylevel(old_level);
    }
#endif /* !_NO_DEBUG_MESSAGES_ */
    }

    if(0!=dlclose(module->dl_handle))
    {
        /* report dlclose() failure, but proceed anyway. */
        WARN("dlclose() call failed! error message is \"%s\"\n", dlerror());
    }

    /* release all memory */
    free(module->lib_name);
    free(module);

    retval=TRUE;

done:
    UnlockModuleList();
    LOGEXIT("FreeLibrary returns BOOL %d\n", retval);
    PERF_EXIT(FreeLibrary);
    return retval;
}


/*++
Function:
  FreeLibraryAndExitThread

See MSDN doc.

--*/
PALIMPORT
VOID
PALAPI
FreeLibraryAndExitThread(
             IN HMODULE hLibModule,
             IN DWORD dwExitCode)
{
    PERF_ENTRY(FreeLibraryAndExitThread);
    ENTRY("FreeLibraryAndExitThread()\n"); 
    FreeLibrary(hLibModule);
    ExitThread(dwExitCode);
    LOGEXIT("FreeLibraryAndExitThread\n");
    PERF_EXIT(FreeLibraryAndExitThread);
}


/*++
Function:
  GetModuleFileNameA

See MSDN doc.

Notes :
    because of limitations in the dlopen() mechanism, this will only return the
    full path name if a relative or absolute path was given to LoadLibrary, or
    if the module was used in a GetProcAddress call. otherwise, this will return
    the short name as given to LoadLibrary. The exception is if hModule is
    NULL : in this case, the full path of the executable is always returned.
--*/
DWORD
PALAPI
GetModuleFileNameA(
           IN HMODULE hModule,
           OUT LPSTR lpFileName,
           IN DWORD nSize)
{
    INT name_length;
    DWORD retval=0;
    LPWSTR wide_name = NULL;

    PERF_ENTRY(GetModuleFileNameA);
    ENTRY("GetModuleFileNameA (hModule=%p, lpFileName=%p, nSize=%u)\n",
          hModule, lpFileName, nSize);

    LockModuleList();
    if(hModule && !LOADValidateModule((MODSTRUCT *)hModule))
    {
        TRACE("Can't find name for invalid module handle %p\n", hModule);
        SetLastError(ERROR_INVALID_HANDLE);
        goto done;
    }
    wide_name=LOADGetModuleFileName((MODSTRUCT *)hModule);

    if(!wide_name)
    {
        ASSERT("Can't find name for valid module handle %p\n", hModule);
        SetLastError(ERROR_INTERNAL_ERROR);
        goto done;
    }

    /* Convert module name to Ascii, place it in the supplied buffer */

    name_length = WideCharToMultiByte(CP_ACP, 0, wide_name, -1, lpFileName,
                                      nSize, NULL, NULL);
    if( name_length==0 )
    {
        TRACE("Buffer too small to copy module's file name.\n");
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        goto done;
    }

    TRACE("File name of module %p is %s\n", hModule, lpFileName);
    retval=name_length;
done:
    UnlockModuleList();
    LOGEXIT("GetModuleFileNameA returns DWORD %d\n", retval);
    PERF_EXIT(GetModuleFileNameA);
    return retval;
}


/*++
Function:
  GetModuleFileNameW

See MSDN doc.

Notes :
    because of limitations in the dlopen() mechanism, this will only return the
    full path name if a relative or absolute path was given to LoadLibrary, or
    if the module was used in a GetProcAddress call. otherwise, this will return
    the short name as given to LoadLibrary. The exception is if hModule is
    NULL : in this case, the full path of the executable is always returned.
--*/
DWORD
PALAPI
GetModuleFileNameW(
           IN HMODULE hModule,
           OUT LPWSTR lpFileName,
           IN DWORD nSize)
{
    INT name_length;
    DWORD retval=0;
    LPWSTR wide_name = NULL;

    PERF_ENTRY(GetModuleFileNameW);
    ENTRY("GetModuleFileNameW (hModule=%p, lpFileName=%p, nSize=%u)\n",
          hModule, lpFileName, nSize);

    LockModuleList();

    if(hModule && !LOADValidateModule((MODSTRUCT *)hModule))
    {
        TRACE("Can't find name for invalid module handle %p\n", hModule);
        SetLastError(ERROR_INVALID_HANDLE);
        goto done;
    }
    wide_name=LOADGetModuleFileName((MODSTRUCT *)hModule);

    if(!wide_name)
    {
        ASSERT("Can't find name for valid module handle %p\n", hModule);
        SetLastError(ERROR_INTERNAL_ERROR);
        goto done;
    }

    /* Copy module name into supplied buffer */

    name_length = lstrlenW(wide_name);
    if(name_length>=(INT)nSize)
    {
        TRACE("Buffer too small to copy module's file name.\n");
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        goto done;
    }
    lstrcpyW(lpFileName,wide_name);

    TRACE("file name of module %p is %S\n", hModule, lpFileName);
    retval=name_length;
done:
    UnlockModuleList();
    LOGEXIT("GetModuleFileNameW returns DWORD %u\n", retval);
    PERF_EXIT(GetModuleFileNameW);
    return retval;
}

/*++
Function:
  PAL_RegisterLibraryW

  Same as LoadLibraryW, but with only the base name of
  the library instead of a full filename.
--*/

HMODULE
PALAPI
PAL_RegisterLibraryW(
         IN LPCWSTR lpLibFileName)
{
    HMODULE hModule = NULL;
    CHAR    lpstr[MAX_PATH];
    INT     cbMultiByteShortName = 0;

    static const char LIB_PREFIX[] = PAL_SHLIB_PREFIX;
    static const char LIB_SUFFIX[] = PAL_SHLIB_SUFFIX;
    static const int LIB_PREFIX_LENGTH = sizeof(LIB_PREFIX) - 1;
    static const int LIB_SUFFIX_LENGTH = sizeof(LIB_SUFFIX) - 1;

    PERF_ENTRY(PAL_RegisterLibraryW);
    ENTRY("PAL_RegisterLibraryW (lpLibFileName=%p (%S)) \n",
          lpLibFileName?lpLibFileName:W16_NULLSTRING,
          lpLibFileName?lpLibFileName:W16_NULLSTRING);

    // First, copy the prefix into the buffer
    strcpy(lpstr, LIB_PREFIX);

    // Second, copy the file name, converting to multibyte along the way
    cbMultiByteShortName = WideCharToMultiByte(CP_ACP, 0, lpLibFileName, -1, 
                                               lpstr + LIB_PREFIX_LENGTH, 
                                               MAX_PATH - (LIB_PREFIX_LENGTH + LIB_SUFFIX_LENGTH),
                                               NULL, NULL);

    if (cbMultiByteShortName == 0)
    {
        DWORD dwLastError = GetLastError();
        if (dwLastError == ERROR_INSUFFICIENT_BUFFER)
        {
            if (lstrlenW(lpLibFileName) + LIB_PREFIX_LENGTH + LIB_SUFFIX_LENGTH < MAX_PATH)
            {
                ASSERT("Insufficient buffer error returned incorrectly from WideCharToMultiByte!\n");
            }
            ERROR("lpLibFileName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Done;
    }

    // Last, add the suffix
    strcat(lpstr, LIB_SUFFIX);

    FILEDosToUnixPathA(lpstr);

    hModule = LOADLoadLibrary(lpstr, FALSE);

Done:
    LOGEXIT("PAL_RegisterLibraryW returns HMODULE %p\n", hModule);
    PERF_EXIT(PAL_RegisterLibraryW);
    return hModule;
}


/*++
Function:
  PAL_UnregisterLibraryW

  Same as FreeLibrary.
--*/
BOOL
PALAPI
PAL_UnregisterLibraryW(
        IN OUT HMODULE hLibModule)
{
    BOOL retval;

    PERF_ENTRY(PAL_UnregisterLibraryW);
    ENTRY("PAL_UnregisterLibraryW (hLibModule=%p)\n", hLibModule);

    retval = FreeLibrary(hLibModule);

    LOGEXIT("PAL_UnregisterLibraryW returns BOOL %d\n", retval);
    PERF_EXIT(PAL_UnregisterLibraryW);
    return retval;
}

/* Internal PAL functions *****************************************************/

/*++
    LOADGetLibRotorPalSoFileName

    Search LD_LIBRARY_PATH (or DYLD_LIBRARY_PATH) for LibRotorPal.  This 
    defines the working directory for PAL.

Parameters:
    OUT LPSTR pszBuf - WCHAR buffer of MAX_PATH length to receive file name

Return value:
    0 if successful
    -1 if failure, with last error set.
--*/
extern "C"
int LOADGetLibRotorPalSoFileName(LPSTR pszBuf)
{
    INT     iRetVal = -1;
    CHAR*   pszFileName = NULL;

    if (!pszBuf)
    {
        ASSERT("LOADGetLibRotorPalSoFileName requires non-NULL pszBuf\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        goto Done;
    }
    iRetVal = FindLibrary(MAKEDLLNAME_A("rotor_pal"), &pszFileName);
    if (pszFileName)
    {
        UINT cchFileName = strlen(pszFileName);
        if (cchFileName + 1  > MAX_PATH)
        {
            ASSERT("Filename returned by FindLibrary was longer than"
                "MAX_PATH!\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            goto Done;
        }
        // If the path is relative, get current working directory and prepend 
        // it (Note that this function is called only on PAL startup, so 
        // current working directory should still be correct)
        if (pszFileName[0] != '/')
        {
            CHAR    szCurDir[MAX_PATH];
            CHAR*   pszRetVal = NULL;
            if ((pszRetVal = getcwd(szCurDir, MAX_PATH)) == NULL)
            {
                SetLastError(ERROR_INTERNAL_ERROR);
                goto Done;
            }
            // If the strings would overflow (note that if the sum of the 
            // lengths == MAX_PATH, the string would overflow b/c of the null
            // terminator -- the 1 is added to account for the /)
            if ((strlen(szCurDir) + strlen(pszFileName) + 1) >= MAX_PATH)
            {
                SetLastError(ERROR_INTERNAL_ERROR);
                goto Done;
            }
            strcat(pszBuf, szCurDir);
            strcat(pszBuf, "/");
            strcat(pszBuf, pszFileName);
        }
        else
        {
            strcpy(pszBuf, pszFileName);
        }
        iRetVal = 0;
    }
Done:
    return iRetVal;
}

/*++
Function :
    LOADInitializeModules

    Initialize the process-wide list of modules (2 initial modules : 1 for
    the executable and 1 for the PAL)

Parameters :
    LPWSTR exe_name : full path to executable

Return value:
    TRUE  if initialization succeedded
    FALSE otherwise

Notes :
    the module manager takes ownership of the exe_name string
--*/
extern "C"
BOOL LOADInitializeModules(LPWSTR exe_name)
{
    CHAR    librotor_fname[MAX_PATH];
    INT     iRetVal = -1;
    LPWSTR  lpwstr = NULL;
#if RETURNS_NEW_HANDLES_ON_REPEAT_DLOPEN
    LPSTR   pszExeName = NULL;
#endif
    BOOL    fRetCode = FALSE;

    if(exe_module.prev)
    {
        ERROR("Module manager already initialized!\n");
        fRetCode = FALSE;
        goto Done;
    }

    if (0 != SYNCInitializeCriticalSection(&module_critsec))
    {
        ERROR("Module manager critical section initialization failed!\n")
	fRetCode = FALSE;
        goto Done;
    }

    /* initialize module for main executable */
    TRACE("Initializing module for main executable\n");
    exe_module.self=(HMODULE)&exe_module;
    exe_module.dl_handle=dlopen(NULL, RTLD_LAZY);
    if(!exe_module.dl_handle)
    {
        ASSERT("Main executable module will be broken : dlopen(NULL) failed. "
             "dlerror message is \"%s\" \n", dlerror());
    }
    exe_module.lib_name = exe_name;
    exe_module.refcount=-1;
    exe_module.next=&pal_module;
    exe_module.prev=&pal_module;
    exe_module.pDllMain = NULL;
    exe_module.ThreadLibCalls = TRUE;
    
    TRACE("Initializing module for PAL library\n");
    pal_module.self=(HANDLE)&pal_module;

    /* Get the real name of the PAL library */
    iRetVal = PALGetLibRotorPalName(librotor_fname, MAX_PATH);
    if (iRetVal < 0)
    {
        ASSERT("PAL module will be broken : LOADGetLibRotorPalSoFileName() "
             "failed. dlerror message is \"%s\" \n", dlerror());
        pal_module.lib_name=NULL;
        pal_module.dl_handle=NULL;
    } else
    {
        TRACE("PAL library is %s\n", librotor_fname);
        lpwstr = UTIL_MBToWC_Alloc(librotor_fname, -1);
        if(NULL == lpwstr)
        {
            ERROR("MBToWC failure, unable to save full name of PAL module\n");
	    fRetCode = FALSE;
            goto Done;
	}
        pal_module.lib_name=lpwstr;
        pal_module.dl_handle=dlopen(librotor_fname, RTLD_LAZY);
        
        if(!pal_module.dl_handle)
        {
            ASSERT("PAL module will be broken : dlopen(%s) failed. dlerror "
                 "message is \"%s\"\n ", librotor_fname, dlerror());
        }
    }
    pal_module.refcount=-1;
    pal_module.next=&exe_module;
    pal_module.prev=&exe_module;
    pal_module.pDllMain = NULL;
    pal_module.ThreadLibCalls = TRUE;

    // For platforms where we can't trust the handle to be constant, we need to 
    // store the inode/device pairs for the modules we just initialized.
#if RETURNS_NEW_HANDLES_ON_REPEAT_DLOPEN
    {
        struct stat stat_buf;
        pszExeName = UTIL_WCToMB_Alloc(exe_name, -1);
        if (NULL == pszExeName)
        {
            ERROR("WCToMB failure, unable to get full name of exe\n");
            fRetCode = FALSE;
            goto Done;
        }
        if ( -1 == stat(pszExeName, &stat_buf))
        {
            SetLastError(ERROR_MOD_NOT_FOUND);
            fRetCode = FALSE;
            goto Done;
        }

        TRACE("Executable has inode %d and device %d\n", 
            stat_buf.st_ino, stat_buf.st_dev);

        exe_module.inode = stat_buf.st_ino; 
        exe_module.device = stat_buf.st_dev;
        if ( -1 == stat(librotor_fname, &stat_buf))
        {
            SetLastError(ERROR_MOD_NOT_FOUND);
            fRetCode = FALSE;
            goto Done;
        }

        TRACE("PAL Library has inode %d and device %d\n", 
            stat_buf.st_ino, stat_buf.st_dev);

        pal_module.inode = stat_buf.st_ino; 
        pal_module.device = stat_buf.st_dev;
    }
#endif

    // If we got here, init succeeded.
    fRetCode = TRUE;
 Done:
    TRACE("Module manager initialization complete.\n");

#if RETURNS_NEW_HANDLES_ON_REPEAT_DLOPEN
    if (pszExeName)
      free(pszExeName);
#endif
    TRACE("Module manager initialization returning %d.\n", fRetCode);
    return fRetCode;
}

/*++
Function :
    LOADFreeModules

    Release all resources held by the module manager (including dlopen handles)

Parameters:
    BOOL bTerminateUnconditionally: If TRUE, this will avoid calling any DllMains

    (no return value)
--*/
extern "C"
void LOADFreeModules(BOOL bTerminateUnconditionally)
{
    MODSTRUCT *module;

    if(!exe_module.prev)
    {
        ERROR("Module manager not initialized!\n");
        return;
    }

    LockModuleList();

    /* Go through the list of modules, release any references we still hold.
       The list is traversed from newest module to oldest */
    do
    {
        module = exe_module.prev;

        // Call DllMain if the module contains one and if we're supposed
        // to call DllMains.
        if( !bTerminateUnconditionally && module->pDllMain )
        {
           /* Exception-safe call to DllMain */
           LOAD_SEH_CallDllMain( module, DLL_PROCESS_DETACH, (LPVOID)-1 );
        }

        /* Remove the current MODSTRUCT from the list, then free its memory */
        module->prev->next = module->next;
        module->next->prev = module->prev;
        module->self = NULL;

        dlclose( module->dl_handle );

        free( module->lib_name );
        module->lib_name = NULL;
        if (module != &exe_module && module != &pal_module)
        {
            free( module );
        }
    }
    while( module != &exe_module );

    /* Flag the module manager as uninitialized */
    exe_module.prev = NULL;

    TRACE("Module manager stopped.\n");

    UnlockModuleList();
    DeleteCriticalSection(&module_critsec);
}

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
extern "C"
void LOADCallDllMain(DWORD dwReason, LPVOID lpReserved)
{
    MODSTRUCT *module;
    BOOL InLoadOrder = TRUE; /* true if in load order, false for reverse */
    THREAD *thread;
    
    thread = (THREAD*) PROCGetCurrentThreadObject();
    if (thread != NULL && thread->isInternal)
    {
        return;
    }

    /* Validate dwReason */
    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH: 
        ASSERT("got called with DLL_PROCESS_ATTACH parameter! Why?\n");
        break;
    case DLL_PROCESS_DETACH:
        ASSERT("got called with DLL_PROCESS_DETACH parameter! Why?\n");
        InLoadOrder = FALSE;
        break;
    case DLL_THREAD_ATTACH:
        TRACE("Calling DllMain(DLL_THREAD_ATTACH) on all known modules.\n");
        break;
    case DLL_THREAD_DETACH:
        TRACE("Calling DllMain(DLL_THREAD_DETACH) on all known modules.\n");
        InLoadOrder = FALSE;
        break;
    default:
        ASSERT("LOADCallDllMain called with unknown parameter %d!\n", dwReason);
        return;
    }

    LockModuleList();

    module = &exe_module;
    do {
        if (!InLoadOrder)
            module = module->prev;

        if (module->ThreadLibCalls)
        {
            if(module->pDllMain)
            {
#if !_NO_DEBUG_MESSAGES_
                /* reset ENTRY nesting level back to zero while inside the callback... */
                int old_level;
                old_level = DBG_change_entrylevel(0);
#endif /* !_NO_DEBUG_MESSAGES_ */

                module->pDllMain((HMODULE) module, dwReason, lpReserved);

#if !_NO_DEBUG_MESSAGES_
                /* ...and set nesting level back to what it was */
                DBG_change_entrylevel(old_level);
#endif /* !_NO_DEBUG_MESSAGES_ */
            }
        }

        if (InLoadOrder)
            module = module->next;
    } while (module != &exe_module);

    UnlockModuleList();
}


/*++
Function:
    DisableThreadLibraryCalls

See MSDN doc.
--*/
BOOL
PALAPI
DisableThreadLibraryCalls(
    IN HMODULE hLibModule)
{
    BOOL ret = FALSE;
    MODSTRUCT *module;
    PERF_ENTRY(DisableThreadLibraryCalls);
    ENTRY("DisableThreadLibraryCalls(hLibModule=%p)\n", hLibModule);

    if (terminator)
    {
        /* PAL shutdown in progress - ignore DisableThreadLibraryCalls */
        ret = TRUE;
        goto done_nolock;
    }

    LockModuleList();
    module = (MODSTRUCT *) hLibModule;

    if(!module || !LOADValidateModule(module))
    {
        // DisableThreadLibraryCalls() does nothing when given
        // an invalid module handle. This matches the Windows
        // behavior, though it is counter to MSDN.
        WARN("Invalid module handle %p\n", hLibModule);
        ret = TRUE;
        goto done;
    }

    module->ThreadLibCalls = FALSE;
    ret = TRUE;

done:
    UnlockModuleList();
done_nolock:
    LOGEXIT("DisableThreadLibraryCalls returns BOOL %d\n", ret);
    PERF_EXIT(DisableThreadLibraryCalls);
    return ret;
}


/* Static function definitions ************************************************/

/*++
Function :
    LOADValidateModule

    Check whether the given MODSTRUCT pointer is valid

Parameters :
    MODSTRUCT *module : module to check

Return value :
    TRUE if module is valid, FALSE otherwise

--*/
static BOOL LOADValidateModule(MODSTRUCT *module)
{
    MODSTRUCT *modlist_enum;

    LockModuleList();

    modlist_enum=&exe_module;

    /* enumerate through the list of modules to make sure the given handle is
       really a module (HMODULEs are actually MODSTRUCT pointers) */
    do 
    {
        if(module==modlist_enum)
        {
            /* found it; check its integrity to be on the safe side */
            if(module->self!=module)
            {
                ERROR("Found corrupt module %p!\n",module);
                UnlockModuleList();
                return FALSE;
            }
            UnlockModuleList();
            TRACE("Module %p is valid (name : %S)\n", module,
                  MODNAME(module));
            return TRUE;
        }
        modlist_enum = modlist_enum->next;
    }
    while (modlist_enum != &exe_module);

    TRACE("Module %p is NOT valid.\n", module);
    UnlockModuleList();
    return FALSE;
}

/*++
Function :
    LOADGetModuleFileName [internal]

    Retrieve the module's full path if it is known, the short name given to
    LoadLibrary otherwise.

Parameters :
    MODSTRUCT *module : module to check

Return value :
    pointer to internal buffer with name of module (Unicode)

Notes :
    this function assumes that the module critical section is held, and that
    the module has already been validated.
--*/
static LPWSTR LOADGetModuleFileName(MODSTRUCT *module)
{
    LPWSTR module_name;
    /* special case : if module is NULL, we want the name of the executable */
    if(!module)
    {
        module_name = exe_module.lib_name;
        TRACE("Returning name of main executable\n");
        return module_name;
    }

    /* return "real" name of module if it is known. we have this if LoadLibrary
       was given an absolute or relative path; we can also determine it at the
       first GetProcAdress call. */
    TRACE("Returning full path name of module\n");
    return module->lib_name;
}

/*++
Function :
    LOADAllocModule

    Allocate and initialize a new MODSTRUCT structure

Parameters :
    void *dl_handle :   handle returned by dl_open, goes in MODSTRUCT::dl_handle
    
    char *name :        name of new module. after conversion to widechar, 
                        goes in MODSTRUCT::lib_name
                        
Return value:
    a pointer to a new, initialized MODSTRUCT strucutre, or NULL on failure.
    
Notes :
    'name' is used to initialize MODSTRUCT::lib_name. The other member is set to NULL
    In case of failure (in malloc or MBToWC), this function sets LastError.
--*/
static MODSTRUCT *LOADAllocModule(void *dl_handle, char *name)
{   
    MODSTRUCT *module;
    LPWSTR wide_name;

    /* no match found : try to create a new module structure */
    module=(MODSTRUCT *) malloc(sizeof(MODSTRUCT));
    if(!module)
    {
        ERROR("malloc() failed! errno is %d (%s)\n", errno, strerror(errno));
        return NULL;
    }

    wide_name = UTIL_MBToWC_Alloc(name, -1);
    if(NULL == wide_name)
    {
        ERROR("couldn't convert name to a wide-character string\n");
        free(module);
        return NULL;
    }

    module->dl_handle = dl_handle;
#if HAVE_DYLIBS
    if (isdylib(module))
    {
        module->refcount = -1;
    }
    else
    {
        module->refcount = 1;
    }
#else   // HAVE_DYLIBS
    module->refcount = 1;
#endif  // HAVE_DYLIBS
    module->self = module;
    module->ThreadLibCalls = TRUE;
    module->next = NULL;
    module->prev = NULL;

    module->lib_name = wide_name;

    return module;
}

/*++
Function :
    LOADLoadLibrary [internal]

    implementation of LoadLibrary (for use by the A/W variants)

Parameters :
    LPSTR ShortAsciiName : name of module as specified to LoadLibrary

    BOOL fDynamic : TRUE if dynamic load through LoadLibrary, FALSE if static load through RegisterLibrary

Return value :
    handle to loaded module

--*/
static HMODULE LOADLoadLibrary(LPSTR ShortAsciiName, BOOL fDynamic)
{
    MODSTRUCT *module = NULL;
    CHAR LongAsciiName[MAXPATHLEN];
    void *dl_handle;
    DWORD dwerror;
    CHAR* pszFullName = NULL;
    BOOL fFullNameNeedsFreeing = FALSE;
    struct stat stat_buf;

    LockModuleList();

    // If we didn't have path information in the name we were called with, do
    // a manual search for the library.  We don't let dlopen do the search -- 
    // that could make the deployment story different on different platforms
    // and, since dlopen searches paths specified when we build, could lead to
    // weird bugs where we load random libraries that happen to be in the
    // search path we used for the build.
    // Instead, we just look to LD_LIBRARY_PATH (or DYLD_LIBRARY_PATH on MacOS)
    // for the library and pass the full name, if found, to dlopen.
    if(!strchr(ShortAsciiName, '/'))
    {
        CHAR szLibInAppDir[MAXPATHLEN];
        LPWSTR ExeDir;

        // first, try to load library from same directory as executable.  This
        // mimics Windows 
        // Note - Can't put this search in FindLibrary because pAppDir isn't 
        // guaranteed to be defined when FindLibrary gets called.  It is 
        // defined when the module manager is loaded, so it's safe for 
        // LoadLibrary.
        TRACE("Looking for module in application directory\n");

        ExeDir = pAppDir;
        if(0 == WideCharToMultiByte(CP_ACP, 0, ExeDir, -1, szLibInAppDir,
                                    MAXPATHLEN-strlen(ShortAsciiName)-1,
                                    NULL, NULL))
        {
            DWORD dwLastError = GetLastError();
            if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
            {
                ERROR("Can't fit application path and module name in "
                      "MAXPATHLEN (%d)!\n", MAXPATHLEN);
            }
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
            SetLastError(ERROR_INTERNAL_ERROR);
            goto done;
        }

        /* PROCGetAppPath returns a path without trailing /, so we add one. */

        strcat(szLibInAppDir, "/");
        strcat(szLibInAppDir, ShortAsciiName);

        // Let LOADLoadLibrary try to load it. No risk of infinite recursion, 
        // since it isn't a short name anymore.
        module = (MODSTRUCT*)LOADLoadLibrary(szLibInAppDir, fDynamic);
        if(module)
        {
            /* Found it :we're done. */
            TRACE("Module %s found \n", szLibInAppDir);
            goto done;
        }

        // If we didn't find it in the application directory, search 
        // the library path
        if (0 != FindLibrary(ShortAsciiName, &pszFullName))
        {
            goto done;
        }

        // FindLibrary returns pszFullLibName == NULL if not found
        if (pszFullName == NULL)
        {
            SetLastError(ERROR_MOD_NOT_FOUND);
            goto done;
        }
        fFullNameNeedsFreeing = TRUE;
        // Protect against infinite recursion -- make sure we now have a full
        // pathname
        if (!(strchr(pszFullName, '/')))
        {
            ASSERT("FindLibrary returned a path that didn't have a / in it?!");
            SetLastError(ERROR_INTERNAL_ERROR);
            goto done;
        }

        // Again, avoid code duplication - let LOADLoadLibrary try to load it.
        // No risk of infinite recursion, since it isn't a short name anymore.
        module = (MODSTRUCT*)LOADLoadLibrary(pszFullName, fDynamic);
        if(!module)
        {
            SetLastError(ERROR_MOD_NOT_FOUND);
            goto done;
        }

        /* Found it :we're done. */
        TRACE("Module %s found \n", pszFullName);
        goto done;        
    }

    TRACE("Looking for module with specific path %s\n", ShortAsciiName);

    if ( -1 == stat( ShortAsciiName, &stat_buf ) )
    {
        TRACE("File %s doesn't exist.\n", ShortAsciiName);
        SetLastError(ERROR_MOD_NOT_FOUND);
        goto done;
    }

    if ( !UTIL_IsExecuteBitsSet( &stat_buf ) )
    {
        TRACE("File %s isn't executable.\n", ShortAsciiName);
        SetLastError(ERROR_MOD_NOT_FOUND);
        goto done;
    }

#if RETURNS_NEW_HANDLES_ON_REPEAT_DLOPEN
    // Since we can't trust handle comparison to see if we've opened this 
    // library before, we need to compare inodes.  To do that, we need 
    // to find the full path.  Note that we could find another file with the
    // same name that isn't an executable library.  Not sure what dlopen
    // does in that situation, but we'll treat that as the library wrt inode, 
    // etc.
    TRACE("Attempt to load library named %s with inode %d and device %d\n", 
            LongAsciiName, stat_buf.st_ino, stat_buf.st_dev);

    /* search module list for a match. */
    module = &exe_module;
    do
    {
        if (stat_buf.st_ino == module->inode && 
            stat_buf.st_dev == module->device)
        {
            /* found the handle. increment the refcount and return the 
               existing module structure */
            TRACE("Found matching module %p for module name %s\n",
                 module, ShortAsciiName);
            if (module->refcount != -1)
                module->refcount++;
            goto done;
        }
        module = module->next;
    } while (module != &exe_module);
#endif

    /* get canonical name of file */
    if(!realpath(ShortAsciiName, LongAsciiName))
    {
        ASSERT("realpath() failed! problem path is %s\n", LongAsciiName);
        SetLastError(ERROR_INTERNAL_ERROR);
        goto done;
    }

    TRACE("Found module; real path is %s\n", LongAsciiName);

    /* see if file can be dlopen()ed; this should work even if it's already
        loaded */

    dl_handle=dlopen(LongAsciiName, RTLD_LAZY);
    if(!dl_handle)
    {
        WARN("dlopen() failed; dlerror says '%s'\n", dlerror()); 
        SetLastError(ERROR_MOD_NOT_FOUND);
        goto done;
    }
    TRACE("dlopen() found module %s\n", LongAsciiName);


#if !RETURNS_NEW_HANDLES_ON_REPEAT_DLOPEN
    /* search module list for a match. */
    module = &exe_module;
    do
    {
        if (dl_handle == module->dl_handle)
        {   
            /* found the handle. increment the refcount and return the 
               existing module structure */
            TRACE("Found matching module %p for module name %s\n",
                 module, ShortAsciiName);
            if (module->refcount != -1)
                module->refcount++;
            dlclose(dl_handle);
            goto done;
        }
        module = module->next;
    } while (module != &exe_module);
#endif

    TRACE("Module doesn't exist : creating %s.\n", LongAsciiName);
    module = LOADAllocModule(dl_handle, LongAsciiName);

    if(NULL == module)
    {
        ERROR("couldn't create new module\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        dlclose(dl_handle);
        goto done;
    }

    /* Add the new module on to the end of the list */
    module->prev = exe_module.prev;
    module->next = &exe_module;
    exe_module.prev->next = module;
    exe_module.prev = module;

#if RETURNS_NEW_HANDLES_ON_REPEAT_DLOPEN
    module->inode = stat_buf.st_ino; 
    module->device = stat_buf.st_dev;
#endif

    /* If we get here, then we have created a new module structure. We can now
       get the address of DllMain if the module contains one. We save
       the last error and restore it afterward, because our caller doesn't
       care about GetProcAddress failures. */
    dwerror = GetLastError();

    module->pDllMain = (PDLLMAIN)GetProcAddress((HMODULE)module, "DllMain");

    SetLastError(dwerror);

    /* If it did contain a DllMain, call it. */
    if(module->pDllMain)
    {
        DWORD dllmain_retval;

        TRACE("Calling DllMain (%p) for module %S\n", 
              module->pDllMain, 
              module->lib_name ? module->lib_name : W16_NULLSTRING);

        {
#if !_NO_DEBUG_MESSAGES_
            /* reset ENTRY nesting level back to zero while inside the callback... */
            int old_level;
            old_level = DBG_change_entrylevel(0);
#endif /* !_NO_DEBUG_MESSAGES_ */

            dllmain_retval = module->pDllMain((HINSTANCE) module,
                DLL_PROCESS_ATTACH, fDynamic ? NULL : (LPVOID)-1);

#if !_NO_DEBUG_MESSAGES_
            /* ...and set nesting level back to what it was */
            DBG_change_entrylevel(old_level);
#endif /* !_NO_DEBUG_MESSAGES_ */
        }

        /* If DlMain(DLL_PROCESS_ATTACH) returns FALSE, we must immediately
           unload the module.*/
        if(FALSE == dllmain_retval)
        {
            TRACE("DllMain returned FALSE; unloading module.\n");
            module->pDllMain = NULL;
            FreeLibrary((HMODULE) module);
            ERROR("DllMain failed and returned NULL. \n");
            SetLastError(ERROR_DLL_INIT_FAILED);
            module = NULL;
        }
    }
    else
    {
        TRACE("Module does not caontian a DllMain function.\n");
    }

done:
    UnlockModuleList();
    if (fFullNameNeedsFreeing)
        free(pszFullName);
    return (HMODULE)module;
}

/*++
Function :
    LOAD_SEH_CallDllMain

    Exception-safe call to DllMain.

Parameters :
    MODSTRUCT *module : module whose DllMain must be called

    DWORD dwReason : parameter to pass down to DllMain, one of DLL_PROCESS_ATTACH, DLL_PROCESS_DETACH, 
        DLL_THREAD_ATTACH, DLL_THREAD_DETACH

    LPVOID lpvReserved : parameter to pass down to DllMain,
        If dwReason is DLL_PROCESS_ATTACH, lpvReserved is NULL for dynamic loads and non-NULL for static loads. 
        If dwReason is DLL_PROCESS_DETACH, lpvReserved is NULL if DllMain has been called by using FreeLibrary 
            and non-NULL if DllMain has been called during process termination. 

(no return value)

Notes :
This function is called from LOADFreeModules. Since we get there from
PAL_Terminate, we can't let exceptions in DllMain go unhandled :
TerminateProcess would be called, and would have to abort uncleanly because
termination was already started. So we catch the exception and ignore it;
we're terminating anyway.
*/
static void LOAD_SEH_CallDllMain(MODSTRUCT *module, DWORD dwReason, LPVOID lpReserved)
{
#if !_NO_DEBUG_MESSAGES_
    /* reset ENTRY nesting level back to zero while inside the callback... */
    int old_level = DBG_change_entrylevel(0);
#endif /* !_NO_DEBUG_MESSAGES_ */
    
    PAL_TRY
    {
        TRACE("Calling DllMain (%p) for module %S\n",
              module->pDllMain, 
              module->lib_name ? module->lib_name : W16_NULLSTRING);
        
        module->pDllMain((HMODULE)module, dwReason, lpReserved);
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        WARN("Call to DllMain (%p) got an unhandled exception; "
              "ignoring.\n", module->pDllMain);
    }
    PAL_ENDTRY

#if !_NO_DEBUG_MESSAGES_
    /* ...and set nesting level back to what it was */
    DBG_change_entrylevel(old_level);
#endif /* !_NO_DEBUG_MESSAGES_ */
}

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
extern "C"
void LockModuleList()
{
    SYNCEnterCriticalSection(&module_critsec, TRUE);
}

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
extern "C"
void UnlockModuleList()
{
    SYNCLeaveCriticalSection(&module_critsec, TRUE);
}

/*++
Function:
  FindLibrary

Abstract
    Search LD_LIBRARY_PATH/DYLD_LIBRARY_PATH for a file named pszRelName

Parameter
    pszRelName: The relative name of the file sought
    ppszFullName: A pointer that will be filled in with the full filename if
        we find it

Return
    0 if completed successfully, even if library not found
    -1 on error
--*/
INT FindLibrary(CHAR* pszRelName, CHAR** ppszFullName)
{
    CHAR*   pszLibPath = NULL;
    CHAR*   pszNext = NULL;
    CHAR**  rgpLibDirSeparators = NULL;
    UINT    cSeparators = 0;
    UINT    iSeparator = 0;
    INT     iRetVal = 0;
    CHAR*   pszSearchPath = NULL;
    BOOL    fSearchPathNeedsFreeing = FALSE;

    if (!ppszFullName)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        iRetVal = -1;
        goto Done;
    }
    *ppszFullName = NULL;

    // First, get the LD_LIBRARY_PATH to figure out where to look
    // Note that pszLibPath points to system memory -- don't free.
    pszLibPath = MiscGetenv(LIBSEARCHPATH);
    if (!pszLibPath)
    {
        SetLastError(ERROR_INTERNAL_ERROR);
        iRetVal = -1;
        goto Done;
    }
    TRACE("FindLibrary: " LIBSEARCHPATH " is %s\n", pszLibPath);

    // We want to make sure that we always search the current directory,
    // regardless of whether LD_LIBRARY_PATH includes it (this mimics
    // Windows behavior)
    if (!(strstr(pszLibPath, ":.:")))
    {
        // 4 is hardcoded here for :.: and null
        pszSearchPath = (char*) malloc (strlen(pszLibPath) + 4);
        if (!pszSearchPath)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            iRetVal = -1;
            goto Done;
        }
        fSearchPathNeedsFreeing = TRUE;
        strcpy(pszSearchPath, pszLibPath);
        strcat(pszSearchPath, ":.:");
    }
    // If LD_LIBRARY_PATH already includes a reference to the current
    // directory, we'll search it in the right order.
    else
    {
        pszSearchPath = pszLibPath;
    }
      
    // Allocate an array for pointers to separators -- there can't be more than
    // the length of LD_LIBRARY_PATH separators...
    rgpLibDirSeparators = (char **) malloc(strlen(pszSearchPath));
    if (!rgpLibDirSeparators)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        iRetVal = -1;
        goto Done;
    }

    // Now, find the separators in LD_LIBRARY_PATH.  Set a pointer to each :
    pszNext = pszSearchPath;
    // There's an implicit separator at the start...
    rgpLibDirSeparators[cSeparators] = pszNext - 1;
    cSeparators++;
    while (*pszNext != '\0')
    {
        if (*pszNext == ':')
        {
            rgpLibDirSeparators[cSeparators] = pszNext;
            cSeparators++;
        }
        pszNext++;
    }
    // And there's an implicit separator at the end.
    rgpLibDirSeparators[cSeparators] = pszNext;
    cSeparators++;

    // Now, check each path for the File
    // Note that cSeparators is always >= 2, so the < -1 check is safe
    for (iSeparator = 0; iSeparator < (cSeparators-1); iSeparator++)
    {
        CHAR        szFileName[MAX_PATH + 1];
        CHAR        szDirName[MAX_PATH + 1];
        struct stat stat_buf;
        UINT        cchDirName = 0;

        // length of DirName is number of chars between the first char after 
        // the colon and the next colon
        cchDirName = rgpLibDirSeparators[iSeparator + 1] - 
                        (rgpLibDirSeparators[iSeparator] + 1);
        memcpy(szDirName, rgpLibDirSeparators[iSeparator] + 1, cchDirName);
        szDirName[cchDirName] = '\0';
        snprintf(szFileName, MAX_PATH, "%s/%s", szDirName, pszRelName);
        if (0 == stat(szFileName, &stat_buf))
        {
	    // First, make sure we've got the canonical path
	    CHAR   szRealPath[MAX_PATH + 1];

	    if(!realpath(szFileName, szRealPath))
	    {
	        ASSERT("realpath() failed! problem path is %s\n", szFileName);
		SetLastError(ERROR_INTERNAL_ERROR);
		goto Done;
	    }
	    // We've found it.  Rejoice!
            TRACE("FindLibrary: found file: %s\n", szRealPath);
            *ppszFullName = strdup(szRealPath);
            if (!*ppszFullName)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                iRetVal = -1;
            }
            goto Done;
        }
    }

Done:
    if (rgpLibDirSeparators)
        free(rgpLibDirSeparators);
    if (fSearchPathNeedsFreeing)
        free(pszSearchPath);
    // Don't treat it as an error if the library's not found -- just set
    // *ppszFullName to NULL.
    return iRetVal;
}

