/*=============================================================
**
** Source:  getmodulefilenamew.c
**
** Purpose: Positive test the GetModuleFileName API.
**          Call GetModuleFileName to retrive current process 
**          full path and file name by passing a NULL module handle
**          in UNICODE
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

#define MODULENAMEBUFFERSIZE 1024


int __cdecl main(int argc, char *argv[])
{

    DWORD ModuleNameLength;
    WCHAR *ModuleFileNameBuf = malloc(MODULENAMEBUFFERSIZE*sizeof(WCHAR));
    int err;


    //Initialize the PAL environment
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        ExitProcess(FAIL);
    }

    //retrive the current process full path and file name
    //by passing a NULL module handle
    ModuleNameLength = GetModuleFileName(
                NULL,             //a NULL handle
                ModuleFileNameBuf,//buffer for module file name
                MODULENAMEBUFFERSIZE);

    //free the memory
    free(ModuleFileNameBuf);

    if(0 == ModuleNameLength)
    {
        Fail("\nFailed to all GetModuleFileName API!\n");
    }


    PAL_Terminate();
    return PASS;
}
