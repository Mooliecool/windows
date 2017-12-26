/*=============================================================
**
** Source: test2.c
**
** Purpose: Allocate some memory. Then reallocate that memory into less
** space than the original amount.  Ensure the
** return values are correct, and also that data placed in the allocated
** memory carries over to the reallocated block.
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

    /* Allocate 200 bytes on the heap */
    if((TheMemory = HeapAlloc(TheHeap, 0, 200)) == NULL)
    {
        Fail("ERROR: HeapAlloc returned NULL when it was called.  "
             "GetLastError() returned %d.",GetLastError());
    }
    
    /* Set the first 100 bytes to 'X' */
    memset(TheMemory, 'X', 100);

    /* Set the second 100 bytes to 'Z' */
    memset(TheMemory+100, 'Z', 100);
    
    /* Reallocate the memory to 100 bytes  */
    ReAllocMemory = HeapReAlloc(TheHeap, 0, TheMemory, 100);
    
    if(ReAllocMemory == NULL)
    {
        Fail("ERROR: HeapReAlloc failed to reallocate the 100 bytes of "
             "heap memory. GetLastError returns %d.",GetLastError());
    }
    
    /* Check that each of the first 100 bytes hasn't lost any data.  
       Anything beyond the first 100 might still be valid, but we can't
       gaurentee it.
    */
    
    for(i=0; i<100; ++i)
    {
        /* Note: Cast to char* so the function knows the size is 1 */
        if(ReAllocMemory[i] != 'X')
        {
            Fail("ERROR: Byte number %d of the reallocated memory block "
                 "is not set to 'X' as it should be.",i);
        }
    }    

    PAL_Terminate();
    return PASS;
}
