/*=============================================================
**
** Source: loadlibrarya.c
**
** Purpose: Negative test the LoadLibrary API with NULL module
**          name.
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

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*load a module by passing a NULL module name*/
    ModuleHandle = LoadLibraryA(NULL);
    if(NULL != ModuleHandle)
    {
        Fail("\nFailed to call loadlibrarya API for a negative test, "
            "call loadibrarya with NULL moudle name, a NULL module "
            "handle is expected, but no NULL module handle is returned, "
            "error code =%u\n", GetLastError());   
    }
  
    PAL_Terminate();
    return PASS;
}
