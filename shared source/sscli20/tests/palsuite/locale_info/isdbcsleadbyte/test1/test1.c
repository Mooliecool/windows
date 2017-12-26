/*============================================================================
**
** Source: test1.c
**
** Purpose: Tests that IsDBCSLeadByte does not find any lead-bytes in the
**          current ansi code page 
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
** TODO: Test for positive, i.e., if it is potentially isdbcsleadbyte
**==========================================================================*/


#include <palsuite.h>

void DoTest()
{
    int value;
    int ret;
    int i;


    for (i=0; i<256; i++)
    {
        value = IsDBCSLeadByte(i);

        ret = GetLastError();
        if (ret == ERROR_INVALID_PARAMETER)
        {
            Fail("IsDBCSLeadByte unexpectedly errored with ERROR_INVALID_PARAMETER for %d!\n", i);
        }
        else if (ret != 0)
        {
            Fail("IsDBCSLeadByte had an unexpected error [%d] for %d!\n", ret, i);
        }
        else if (value)
        {
            Fail("IsDBCSLeadByte incorrectly found a lead byte in value [%d] for"
                " %d\n", value, i);
        }

    }
}

int __cdecl main(int argc, char *argv[])
{

    if (0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    DoTest();

    PAL_Terminate();

//    setlocale( "japan", );

    return PASS;
}

