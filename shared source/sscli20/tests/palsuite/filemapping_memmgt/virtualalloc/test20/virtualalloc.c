/*=============================================================
**
** Source:  virtualalloc.c
**
** Purpose: Positive test the VirtualAlloc API.
**          Ensure that memory re-committed through VirtualAlloc
**          is not changed.
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
    int err;
    int *ptr;

    //Initialize the PAL environment
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        ExitProcess(FAIL);
    }
    
    ptr = (int *) VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE,
                               PAGE_READWRITE);
    if (ptr == NULL)
    {
        Fail("First VirtualAlloc failed!\n");
    }
    
    *ptr = 123;

    ptr = (int *) VirtualAlloc(ptr, 4096, MEM_COMMIT, PAGE_READWRITE);
    if (ptr == NULL)
    {
        Fail("Second VirtualAlloc failed!\n");
    }
    if (*ptr != 123)
    {
        Fail("VirtualAlloc modified (probably zeroed) re-committed memory!\n");
    }

    PAL_Terminate();
    return PASS;
}
