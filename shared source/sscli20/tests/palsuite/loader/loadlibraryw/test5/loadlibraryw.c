/*=============================================================
**
** Source: loadlibraryw.c
**
** Purpose: Negative test the LoadLibraryW API.
**          Call LoadLibraryW by passing a module name 
**          without extension but with a trailing dot.
**
** 
**  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
** 
**  The use and distribution terms for this software are contained in the file
**  named license.txt, which can be found in the root of this distribution.
**  By using this software in any fashion, you are agreeing to be bound by the
**  terms of this license.
** 
**  You must not remove this notice, or any other, from this software.
** 
**
**============================================================*/
#define UNICODE
#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{
    HMODULE ModuleHandle;
    int err;
    WCHAR *lpModuleName;
    char ModuleName[_MAX_FNAME];

    /* Initialize the PAL environment */
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    memset(ModuleName, 0, _MAX_FNAME);

    /*Module name without extension but with a trailing dot*/
#if WIN32
    sprintf(ModuleName,"%s","rotor_pal.");
#else
    sprintf(ModuleName,"%s","librotor_pal.");
#endif

    /* convert a normal string to a wide one */
    lpModuleName = convert(ModuleName);

    /* load a module */
    ModuleHandle = LoadLibraryW(lpModuleName);

    /* free the memory */
    free(lpModuleName);

    if(NULL != ModuleHandle)
    {
        Trace("Failed to call LoadLibraryW API for a negative test "
            "call LoadLibraryW with module name which does not have "
            "extension except a trailing dot, a NULL module handle is"
            "expected, but no NULL module handle is returned, "
            "error code = %u\n", GetLastError());    

        /* decrement the reference count of the loaded dll */
        err = FreeLibrary(ModuleHandle);
        if(0 == err)
        {
            Trace("\nFailed to call FreeLibrary API, "
                "error code = %u\n", GetLastError());
        }

        Fail("");
    }

    PAL_Terminate();
    return PASS;
}
