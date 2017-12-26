/*=============================================================
**
** Source: heapalloc.c
**
** Purpose: Positive test the HeapAlloc API.
**          Call HeapAlloc by pssing zero as control flag 
**          Call HeapAlloc by passing HEAP_ZERO_MEMORY as control flag
**          Call HeapAlloc to allocate one byte heap memory
**          Call HeapAlloc to allocate maximum available heap memory
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

#define HEAPSIZE 64

int __cdecl main(int argc, char *argv[])
{
    int err;
    HANDLE ProcessHeapHandle;
    LPVOID lpHeap;

    /* Initialize the PAL environment */
    err = PAL_Initialize(argc, argv);
    if(0 != 0)
    {
        ExitProcess(FAIL);
    }

    /* Retrive the calling process heap handle */
    ProcessHeapHandle = GetProcessHeap();
     
    if(!ProcessHeapHandle)
    {
        Fail("\nFailed to call GetProcessHeap API!\n");
    }
    
    /* allocate a heap memory in specified size */
    lpHeap = HeapAlloc(ProcessHeapHandle,   /* HeapHandle            */
                        0,                  /* control flag          */
                        HEAPSIZE);          /* /specify the heap size */
    if(NULL == lpHeap)
    {
        Fail("Failed to call HeapAlloc API!\n");
    }

    /* free the heap memory */
    err = HeapFree(ProcessHeapHandle,
                    0,
                    lpHeap);
    if(0 == err)
    {
        Fail("Failed to call HeapFree API!\n");
    }


    /* allocate a heap memory in 1 byte size */
    lpHeap = HeapAlloc(ProcessHeapHandle,   /* HeapHandle    */
                        0,                  /* control flag  */
                        1);          /* specify the heap size*/
    if(NULL == lpHeap)
    {
        Fail("Failed to call HeapAlloc API to allocate one byte heap memory!\n");
    }

    /* free the heap memory */
    err = HeapFree(ProcessHeapHandle,
                    0,
                    lpHeap);
    if(0 == err)
    {
        Fail("Failed to call HeapFree API!\n");
    }

    /* allocate a heap memory and initialize it to zero */
    lpHeap = HeapAlloc(ProcessHeapHandle,/* HeapHandle            */
                        HEAP_ZERO_MEMORY,/* control flag          */
                        HEAPSIZE);       /* specify the heap size */
    if(NULL == lpHeap)
    {
        Fail("Failed to call HeapAlloc API with HEAP_ZERO_MEMORY control flag!\n");
    }

    /* free the heap memory */
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
