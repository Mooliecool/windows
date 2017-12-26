/*============================================================
**
** Source: test.c
**
** Purpose: Test for CharNextExA, ensures it returns an LPTSTR
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

/* Depends on strcmp() */

#include <palsuite.h>

void testString(LPSTR input, LPSTR expected)
{

    LPTSTR pReturned = NULL;

    pReturned = CharNextExA(0,input,0);

    /* Compare the Returned String to what it should be */
    if(strcmp(expected,pReturned) != 0) 
    {
        Fail("ERROR: CharNextExA Failed: [%s] and [%s] are not equal, "
            "they should be after calling CharNextExA.\n",
            pReturned,expected);
    }


}

int __cdecl main(int argc, char *argv[]) 
{

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    /* test several Strings */
    testString("this is the string", "his is the string");  
    testString("t", "");  
    testString("", ""); 
    testString("a\t", "\t"); 
    testString("a\a", "\a");
    testString("a\b", "\b");
    testString("a\"", "\"");
    testString("a\\", "\\");
    testString("\\", "");
    testString("\f", "");
    testString("\bx", "x");
    
    PAL_Terminate();
    return PASS;
}



