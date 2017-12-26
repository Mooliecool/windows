/*============================================================
**
** Source: test.c
**
** Purpose: Test for wsprintfA() function
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

#include <palsuite.h>

/* memcmp is used to verify the results, so this test is dependent on it. */
/* ditto with strlen */

char * ErrorMessage; 
char buf[256];

BOOL test1()
{
    int pos = 0x1234ab;

    /* Test 1 */
    wsprintf(buf, "foo %x", pos);
    if (memcmp(buf, "foo 1234ab", strlen(buf) + 1) != 0)
    {
        ErrorMessage = "ERROR: (Test 1) Failed. The correct "
            "string is 'foo 1234ab' and the result returned was ";
        return FAIL;
    }

    /* Test 2 */
    wsprintf(buf, "foo %lx", pos);
    if (memcmp(buf, "foo 1234ab", strlen(buf) + 1) != 0)
    {
        ErrorMessage = "ERROR: (Test 2) Failed. The correct "
            "string is 'foo 1234ab' and the result returned was ";
        return FAIL;
    }


    /* Test 3 */
    wsprintf(buf, "foo %7x", pos);
    if (memcmp(buf, "foo  1234ab", strlen(buf) + 1) != 0)
    {
        ErrorMessage = "ERROR: (Test 3) Failed. The correct string is "
            "'foo  1234ab' and the result returned was ";
        return FAIL;
    }

    /* Test 4 */
    wsprintf(buf, "foo %-7x", pos);
    if (memcmp(buf, "foo 1234ab ", strlen(buf) + 1) != 0)
    {
        ErrorMessage = "ERROR: (Test 4) Failed. The correct string is "
            "'foo 1234ab' and the result returned was ";
        return FAIL;
    }

    /* Test 5 */
    wsprintf(buf, "foo %.1x", pos);
    if (memcmp(buf, "foo 1234ab", strlen(buf) + 1) != 0)
    {
        ErrorMessage = "ERROR: (Test 5) Failed. The correct string is "
            "'foo 1234ab' and the result returned was ";
        return FAIL;
    }

    /* Test 6 */
    wsprintf(buf, "foo %.7x", pos);
    if (memcmp(buf, "foo 01234ab", strlen(buf) + 1) != 0)
    {
        ErrorMessage = "ERROR: (Test 6) Failed. The correct string is "
            "'foo 01234ab' and the result returned was ";
        return FAIL;
    }

    /*  Test 7 */
    wsprintf(buf, "foo %07x", pos);
    if (memcmp(buf, "foo 01234ab", strlen(buf) + 1) != 0)
    {
        ErrorMessage = "ERROR: (Test 7) Failed. The correct string is "
            "'foo 01234ab' and the result returned was ";
        return FAIL;
    }

    /* Test 8 */
    wsprintf(buf, "foo %#x", pos);
    if (memcmp(buf, "foo 0x1234ab", strlen(buf) + 1) != 0)
    {
        ErrorMessage = "ERROR: (Test 8) Failed. The correct string is "
            "'foo 0x1234ab' and the result returned was ";
        return FAIL;
    }

    return PASS;
}

int __cdecl main(int argc, char *argv[])
{

    if(0 != (PAL_Initialize(argc, argv)))
    {
       return FAIL;
    }

    if(test1())
    {
       Fail("%s '%s'\n",ErrorMessage,buf);
    }
  
  PAL_Terminate();  
  return PASS;
}


