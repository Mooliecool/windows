/*=============================================================
**
** Source: pal_registerlibraryw_unregisterlibraryw_neg.c
**
** Purpose: Negative test the PAL_RegisterLibrary API.
**          Call PAL_RegisterLibrary to map a non-existant module
**          into the calling process address space. 
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
    char ModuleName[64];
    WCHAR *wpModuleName = NULL;
    int err;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    memset(ModuleName, 0, 64);
    sprintf(ModuleName, "%s", "not_exist_module_name");

    /*convert a normal string to a wide one*/
    wpModuleName = convert(ModuleName);

    /*load a not exist module*/
    ModuleHandle = PAL_RegisterLibrary(wpModuleName);

    /*free the memory*/
    free(wpModuleName);

    if(NULL != ModuleHandle)
    {
        Trace("ERROR: PAL_RegisterLibrary successfully mapped "
              "a module that does not exist into the calling process\n");

        /*decrement the reference count of the loaded DLL*/
        err = PAL_UnregisterLibrary(ModuleHandle);
        if(0 == err)
        {
            Trace("\nFailed to call PAL_UnregisterLibrary API to decrement the "
                "count of the loaded DLL module!\n");
        }
        Fail("");

    }

    PAL_Terminate();
    return PASS;
}
