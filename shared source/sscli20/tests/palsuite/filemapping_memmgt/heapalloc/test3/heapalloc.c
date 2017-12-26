/*=============================================================
**
** Source: heapalloc.c
**
** Purpose: Positive test the HeapAlloc API.
**          Call HeapAlloc asking for zero bytes 
**          with HEAP_ZERO_MEMORY control flag 
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

#define HEAPSIZE 0

int __cdecl main(int argc, char *argv[])
{
    int err;
    HANDLE ProcessHeapHandle;
    LPVOID lpHeap;


    //Initialize the PAL environment
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        ExitProcess(FAIL);
    }

    //Retrive the calling process heap handle 
    ProcessHeapHandle = GetProcessHeap();
     
    if(!ProcessHeapHandle)
    {
        Fail("\nFailed to call GetProcessHeap API!\n");
    }
    
    lpHeap = HeapAlloc(ProcessHeapHandle,//HeapHandle
                        HEAP_ZERO_MEMORY,//control flag
                        HEAPSIZE);       //specify the heap size

    //lpHeap should be non-NULL pointer
    if(NULL == lpHeap)
    {
        Fail("Failed to call HeapAlloc API, when number of bytes to be allocated is zero!\n");
    }

    //free the heap memory
    err = HeapFree(ProcessHeapHandle,
                    0,
                    lpHeap);
    if(0 == err)
    {
        Fail("Failed to call HeapFree API!\n");
    }

    PAL_Terminate();
    return PASS;
}
