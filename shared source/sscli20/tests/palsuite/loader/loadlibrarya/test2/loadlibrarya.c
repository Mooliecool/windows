/*=============================================================
**
** Source: loadlibrarya.c
**
** Purpose: Negative test the LoadLibraryA API.
**          Call LoadLibraryA with a not exist module Name 
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
#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{
    HMODULE ModuleHandle;
    int err;
    const char *pModuleName = "Not-exist-module-name";

    /* Initialize the PAL environment */
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }
    
    /*try to load a not exist module */
    ModuleHandle = LoadLibraryA(pModuleName);
    if(NULL != ModuleHandle)
    {
        Trace("Failed to call LoadLibraryA with a not exist mudule name, "
            "a NULL module handle is expected, but no NULL module handle "
            "is returned, error code=%u\n", GetLastError());

        /* decrement the reference count of the loaded module */
        err = FreeLibrary(ModuleHandle);
        if(0 == err)
        {
            Trace("\nFailed to all FreeLibrary API to decrement "
                    "the reference count of the loaded module, "
                    "error code = %u\n", GetLastError());

        }

        Fail("");
    }

    PAL_Terminate();
    return PASS;
}
