/*=====================================================================
**
** Source:  test7.c (exception_handling\pal_except\test7)
**
** Purpose: Tests the PAL implementation of the PAL_TRY and 
**          PAL_EXCEPT functions. Tests that the EXCEPTION block
**          is missed if no exceptions happen
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
**===================================================================*/



#include <palsuite.h>


int __cdecl main(int argc, char *argv[])
{
    BOOL bTry = FALSE;
    BOOL bExcept = FALSE;

    if (0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    /*
    ** Test to make sure we skip the exception block.
    */

    PAL_TRY 
    {
        bTry = TRUE;    /* indicate we hit the PAL_TRY block */
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        bExcept = TRUE; /* indicate we hit the PAL_EXCEPT block */
    }
    PAL_ENDTRY;

    if (!bTry)
    {
        Trace("PAL_TRY_EXCEPT: ERROR -> It appears the code in the PAL_TRY"
            " block was not executed.\n");
    }

    if (bExcept)
    {
        Trace("PAL_TRY_EXCEPT: ERROR -> It appears the code in the PAL_EXCEPT"
            " block was executed even though no exception was supposed to"
            " happen.\n");
    }

    /* did we hit the correct code blocks? */
    if(!bTry || bExcept)
    {
        Fail("");
    }

    PAL_Terminate();  
    return PASS;

}
