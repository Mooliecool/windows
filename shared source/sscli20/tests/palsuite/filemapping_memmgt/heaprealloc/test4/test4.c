/*=============================================================
**
** Source: test4.c
**
** Purpose: Call HeapReAlloc with a NULL pointer to memory.  It should
** return NULL.
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

/* Note: When attempted with a NULL Handle, this test crashes under win32.
   As such, the behaviour isn't tested here.
*/

#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{
    
    HANDLE TheHeap;
    LPVOID TheMemory = NULL;

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
    if(HeapAlloc(TheHeap, 0, 100) == NULL)
    {
        Fail("ERROR: HeapAlloc returned NULL when it was called.  "
             "GetLastError() returned %d.",GetLastError());
    }

    /* Call HeapReAlloc with a NULL memory pointer.  It should fail */
    
    if(HeapReAlloc(TheHeap, 0, TheMemory, 100) != NULL)
    {
        Fail("ERROR: HeapReAlloc was passed an invalid memory pointer.  "
             "It should have failed and returned NULL upon failure.");
    }

    if(GetLastError() != 0)
    {
        Fail("ERROR: GetLastError should be zero after passing a NULL "
             "memory pointer to HeapReAlloc.\n");
    }

    /* Call HeapReAlloc with a size of 0 bytes on a NULL memory pointer.
       It should still fail.
    */
    
    if(HeapReAlloc(TheHeap, 0, TheMemory, 0) != NULL)
    {
        Fail("ERROR: HeapReAlloc was passed an invalid memory pointer and "
             "the amount of memory to reallocate was 0.  "
             "It should have failed and returned NULL upon failure.");
    }
    
    if(GetLastError() != 0)
    {
        Fail("ERROR: GetLastError should be zero after passing a NULL "
             "memory pointer to HeapReAlloc.\n");
    }

    PAL_Terminate();
    return PASS;
}
