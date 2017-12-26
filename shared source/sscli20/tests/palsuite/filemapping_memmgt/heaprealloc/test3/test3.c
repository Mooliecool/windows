/*=============================================================
**
** Source: test3.c
**
** Purpose: Allocate some memory. Then reallocate that memory into a
** bigger space on the heap.  Check that the first portion of the data is 
** unchanged.  Then set the new portion to a value, to ensure that it is
** properly writable memory.
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
    int i;

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
    
    /* Set the first 100 bytes to 'X' */
    memset(TheMemory, 'X', 100);
    
    /* Reallocate the memory to 200 bytes  */
    ReAllocMemory = HeapReAlloc(TheHeap, 0, TheMemory, 200);
    
    if(ReAllocMemory == NULL)
    {
        Fail("ERROR: HeapReAlloc failed to reallocate the 100 bytes of "
             "heap memory. GetLastError returns %d.",GetLastError());
    }
    
    /* Check that each of the first 100 bytes hasn't lost any data. */
    for(i=0; i<100; ++i)
    {
        
        if(ReAllocMemory[i] != 'X')
        {
            Fail("ERROR: Byte number %d of the reallocated memory block "
                 "is not set to 'X' as it should be.",i);
        }
    }    

    /* Beyond the first 100 bytes is valid free memory.  We'll set all this
       memory to a value -- though, even if HeapReAlloc didn't work, it might 
       still be possible to memset this memory without raising an exception.
    */
    memset(ReAllocMemory+100, 'Z', 100);
    
    PAL_Terminate();
    return PASS;
}
