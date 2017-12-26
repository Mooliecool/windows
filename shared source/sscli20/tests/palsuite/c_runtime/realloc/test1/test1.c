/*============================================================================
**
** Source:  test1.c
**
** Purpose: Uses realloc to allocate and realloate memory, checking
**          that memory contents are copied when the memory is reallocated.
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
**==========================================================================*/

#include <palsuite.h>

int __cdecl main(int argc, char **argv)
{
    char *testA;
    const int len1 = 10;
    const char str1[] = "aaaaaaaaaa";

    const int len2 = 20;
    const char str2[] = "bbbbbbbbbbbbbbbbbbbb";

    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    /* this should work like malloc */
    testA = (char *)realloc(NULL, len1*sizeof(char));  
    memcpy(testA, str1, len1);
    if (testA == NULL)
    {
        Fail("We ran out of memory (unlikely), or realloc is broken.\n");
    }

    if (memcmp(testA, str1, len1) != 0)
    { 
        Fail("realloc doesn't properly allocate new memory.\n");
    }
  
    testA = (char *)realloc(testA, len2*sizeof(char));  
    if (memcmp(testA, str1, len1) != 0)
    { 
        Fail("realloc doesn't move the contents of the original memory "
             "block to the newly allocated block.\n");
    }

    memcpy(testA, str2, len2);
    if (memcmp(testA, str2, len2) != 0)
    {
        Fail("Couldn't write to memory allocated by realloc.\n");
    }

    /* free the buffer */
    testA = realloc(testA, 0);
    if (testA != NULL)
    {
        Fail("Realloc didn't return NULL when called with a length "
             "of zero.\n");
    }
    PAL_Terminate();
    return PASS;
}
