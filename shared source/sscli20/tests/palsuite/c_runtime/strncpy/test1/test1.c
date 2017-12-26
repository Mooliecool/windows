/*============================================================================
**
** Source:  test1.c
**
** Purpose:
** Test to see that you can copy a portion of a string into a new buffer.
** Also check that the strncpy function doesn't overflow when it is used.
** Finally check that if the number of characters given is greater than the 
** amount to copy, that the destination buffer is padded with NULLs.
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
    char dest[80];
    char *result = "foobar";
    char *str = "foobar\0baz";
    char *ret;
    int i;
    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    for (i=0; i<80; i++)
    {
        dest[i] = 'x';
    }

    ret = strncpy(dest, str, 3);
    if (ret != dest)
    {
        Fail("Expected strncpy to return %p, got %p!\n", dest, ret);        
    }

    if (strncmp(dest, result, 3) != 0)
    {
        Fail("Expected strncpy to give \"%s\", got \"%s\"!\n", result, dest);
    }

    if (dest[3] != 'x')
    {
        Fail("strncpy overflowed!\n");
    }

    ret = strncpy(dest, str, 40);
    if (ret != dest)
    {
        Fail("Expected strncpy to return %p, got %p!\n", dest, ret);        
    }

    if (strcmp(dest, result) != 0)
    {
        Fail("Expected strncpy to give \"%s\", got \"%s\"!\n", result, dest);
    }

    for (i=strlen(str); i<40; i++)
    {
        if (dest[i] != 0)
        {
            Fail("strncpy failed to pad the destination with NULLs!\n");
        }
    }

    if (dest[40] != 'x')
    {
        Fail("strncpy overflowed!\n");
    }
    


    PAL_Terminate();

    return PASS;
}
