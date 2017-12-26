/*============================================================================
**
** Source:  test1.c
**
** Purpose: 
** Search for a number of tokens within strings.  Check that the return values
** are what is expect, and also that the strings match up with our expected
** results.
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


int __cdecl main(int argc, char *argv[])
{
    char str[] = "foo bar baz";
    char *result1= "foo \0ar baz";
    char *result2= "foo \0a\0 baz";
    int len = strlen(str) + 1;
    char *ptr;

    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    ptr = strtok(str, "bz");
    if (ptr != str)
    {
        Fail("Expected strtok() to return %p, got %p!\n", str, ptr);
    }
    if (memcmp(str, result1, len) != 0)
    {
        Fail("strtok altered the string in an unexpeced way!\n");
    }

    ptr = strtok(NULL, "r ");
    if (ptr != str + 5)
    {
        Fail("Expected strtok() to return %p, got %p!\n", str+5, ptr);
    }
    if (memcmp(str, result2, len) != 0)
    {
        Fail("strtok altered the string in an unexpeced way!\n");
    }


    ptr = strtok(NULL, "X");
    if (ptr != str + 7)
    {
        Fail("Expected strtok() to return %p, got %p!\n", str + 7, ptr);
    }
    if (memcmp(str, result2, len) != 0)
    {
        Fail("strtok altered the string in an unexpeced way!\n");
    }

    ptr = strtok(NULL, "X");
    if (ptr != NULL)
    {
        Fail("Expected strtok() to return %p, got %p!\n", NULL, ptr);
    }
    if (memcmp(str, result2, len) != 0)
    {
        Fail("strtok altered the string in an unexpeced way!\n");
    }

    PAL_Terminate();
    return PASS;
}
