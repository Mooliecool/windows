/*=============================================================
**
** Source: LocalAlloc.c
**
** Purpose: Positive test the LocalAlloc API.
**          Call LocalAlloc with zero as the  allocation attribute 
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
    HLOCAL LocalHeap;
    HLOCAL FreeHeap;
    int err;
    const SIZE_T heapSize = 64;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }    

    /*Allocate the specified number of bytes from the heap*/
    /*with allocation attribute: zero which is required by PAL Doc*/
    LocalHeap = LocalAlloc(0, heapSize);
    if(!LocalHeap)
    {
        Fail("\nFailed to call LocalAlloc API, "
            "error code=%u\n", GetLastError());
    }
    
    /*Free the allocated local heap memory*/
    FreeHeap = LocalFree(LocalHeap);
    if(FreeHeap)
    {
        Fail("Failed to call LocalFree API, "
            "error code=%u\n", GetLastError());
    }

    PAL_Terminate();
    return PASS;
}
