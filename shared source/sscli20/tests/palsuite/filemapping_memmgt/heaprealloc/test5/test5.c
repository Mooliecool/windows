/*=============================================================
**
** Source: test5.c
**
** Purpose: Allocate some memory. Then call HeapRealloc with 0 as the
** amount of memory to reallocate.  This should work, essentially freeing
** the memory (though we can't verfiy this)
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
    
    HANDLE TheHeap;
    char* TheMemory;
    char* ReAllocMemory;
    char* ReAllocMemory2;

    if(PAL_Initialize(argc, argv) != 0)
    {
        return FAIL;
    }

    TheHeap = GetProcessHeap();

    if(TheHeap == NULL)
    {
        Fail("ERROR: GetProcessHeap() returned NULL when it was called. "
             "GetLastError() returned %d.",GetLastError());
    }

    /* Allocate 100 bytes on the heap */
    if((TheMemory = HeapAlloc(TheHeap, 0, 100)) == NULL)
    {
        Fail("ERROR: HeapAlloc returned NULL when it was called.  "
             "GetLastError() returned %d.",GetLastError());
    }

    /* Set each byte of that memory block to 'x' */
    memset(TheMemory, 'X', 100);

    /* Reallocate the memory into 0 bytes */
    ReAllocMemory = HeapReAlloc(TheHeap, 0, TheMemory, 0);

    if(ReAllocMemory == NULL)
    {
        Fail("ERROR: HeapReAlloc failed to reallocate the 100 bytes of "
             "heap memory. GetLastError returns %d.",GetLastError());
    }

    /* Reallocate the memory we just put into 0 bytes, into 100 bytes. */
    ReAllocMemory2 = HeapReAlloc(TheHeap, 0, ReAllocMemory, 100);

    if(ReAllocMemory2 == NULL)
    {
        Fail("ERROR: HeapReAlloc failed to reallocate the 0 bytes of "
             "heap memory into 100. GetLastError returns %d.",GetLastError());
    }

    PAL_Terminate();
    return PASS;
}
