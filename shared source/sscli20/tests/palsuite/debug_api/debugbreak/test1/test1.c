/*=============================================================
**
** Source: test1.c
**
** Purpose: Tests that DebugBreak works in the grossest fashion.
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
    BOOL bTry = FALSE;

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
    
    PAL_TRY 
    {
        DebugBreak();
        if (!bTry)
        {
            Fail("DebugBreak: Continued in Try block.\n");
        }
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        bTry = TRUE;
    }
    PAL_ENDTRY;

    if (!bTry)
    {
         Fail("DebugBreak: Did not reach the exception block.\n");
    }


    PAL_Terminate();
    return PASS;
}
