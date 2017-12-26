/*============================================================
**
** Source: test.c
**
** Purpose: Test for wsprintfW() function
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
**=========================================================*/

#define UNICODE
#include <palsuite.h>


char * ErrorMessage = NULL; 
WCHAR * BadResult = NULL; 

/* memcmp is used to verify the results, so this test is dependent on it. */
/* ditto with strlen */


BOOL test1()
{
    char checkstr[] = "hello world";

    WCHAR buf[256];

    /* Test a simple string */
    wsprintf(buf, convert("hello world"));

    /* Error message */
    ErrorMessage = "ERROR: (Test 1) Failed on 'hello world' test. "
        "The correct string is 'hello world' and the result returned was ";
    BadResult = buf;

    return (memcmp(convert(checkstr), buf, wcslen(buf)*2+2) != 0);
}

int __cdecl main(int argc, char *argv[]) {

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    if(test1()) {
        Fail("%s '%s'\n",ErrorMessage,convertC(BadResult));

    }
  
    PAL_Terminate();  
    return PASS;

}



