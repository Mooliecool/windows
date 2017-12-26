/*=============================================================
**
** Source:  loadlibraryw.c
**
** Purpose: Positive test the LoadLibrary API.
**          Call LoadLibrary to map a module into the calling
**          process address space(DLL file)
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
/* SHLEXT is defined only for Unix variants */

#if defined(SHLEXT)
#define ModuleName    "librotor_pal"SHLEXT
#else
#define ModuleName    "rotor_pal.dll"
#endif

int __cdecl main(int argc, char *argv[])
{
    HMODULE ModuleHandle;
    int err;
    WCHAR *lpModuleName;

    /* Initialize the PAL environment */
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        ExitProcess(FAIL);
    }

    /* convert a normal string to a wide one */
    lpModuleName = convert(ModuleName);

    /* load a module */
    ModuleHandle = LoadLibrary(lpModuleName);

    /* free the memory */
    free(lpModuleName);

    if(!ModuleHandle)
    {
        Fail("Failed to call LoadLibrary API!\n");
    }


    /* decrement the reference count of the loaded dll */
    err = FreeLibrary(ModuleHandle);
    if(0 == err)
    {
        Fail("\nFailed to all FreeLibrary API!\n");
    }

    PAL_Terminate();
    return PASS;
}
