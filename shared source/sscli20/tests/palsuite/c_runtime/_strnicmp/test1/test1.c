/*============================================================================
**
** Source:  test1.c
**
** Purpose: Test #1 for the _strnicmp function
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
    char str1[] = "foo";
    char str2[] = "foox";
    char str3[] = "fOo";
    char str4[] = "ABCDE";
    char str5[] = "ABCD[";
    char str6[] = "abcde";
    char str7[] = "abcd^";

    /*
     *  Initialize the PAL and return FAIL if this fails
     */
    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    if (_strnicmp(str1, str2, strlen(str2)) >= 0)
    {
        Fail ("ERROR: _strnicmp(\"%s\", \"%s\", %d) returned >= 0\n",
                str1, str2, strlen(str2));
    }

    if (_strnicmp(str2, str1, strlen(str2)) <= 0)
    {
        Fail ("ERROR: _strnicmp(\"%s\", \"%s\", %d) returned <= 0\n",
                str2, str1, strlen(str2));
    }

    if (_strnicmp(str1, str2, strlen(str1)) != 0)
    {
        Fail ("ERROR: _strnicmp(\"%s\", \"%s\", %d) returned != 0\n",
                str1, str2, strlen(str1));
    }

    if (_strnicmp(str1, str3, strlen(str1)) != 0)
    {
        Fail ("ERROR: _strnicmp(\"%s\", \"%s\", %d) returned != 0\n",
                str1, str3, strlen(str3));
    }

    if (_strnicmp(str3, str1, strlen(str1)) != 0)
    {
        Fail ("ERROR: _strnicmp(\"%s\", \"%s\", %d) returned != 0\n",
                str3, str1, strlen(str1));
    }

    /* new testing */
    
    /* str4 should be greater than str5 */
    if (_strnicmp(str4, str5, strlen(str4)) <= 0)
    {
        Fail ("ERROR: _strnicmp(\"%s\", \"%s\", %d) returned >= 0\n",
                str4, str5, strlen(str4));
    }

    /* str6 should be greater than str7 */
    if (_strnicmp(str6, str7, strlen(str6)) <= 0)
    {
        Fail ("ERROR: _strnicmp(\"%s\", \"%s\", %d) returned <= 0\n",
                str6, str7, strlen(str6));
    }


    PAL_Terminate();

    return PASS;
}
