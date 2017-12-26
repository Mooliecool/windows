/*=====================================================================
**
** Source:  PAL_TRY_EXCEPT.c (test 1)
**
** Purpose: Tests the PAL implementation of the PAL_TRY and 
**          PAL_EXCEPT functions. An exception is forced to ensure
**          the exception block is hit.
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
    int* p = 0x00000000;   /* NULL pointer */
    BOOL bTry = FALSE;
    BOOL bExcept = FALSE;

    if (0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    /*
    ** test to make sure we get into the exception block
    */
    
    PAL_TRY 
    {
        if (bExcept)
        {
            Fail("PAL_TRY_EXCEPT: ERROR -> Something weird is going on."
                " PAL_EXCEPT was hit before PAL_TRY.\n");
        }
        bTry = TRUE;    /* indicate we hit the PAL_TRY block */
        *p = 13;        /* causes an access violation exception */
        Fail("PAL_TRY_EXCEPT: ERROR -> code was executed after the "
            "access violation.\n");
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        if (!bTry)
        {
            Fail("PAL_TRY_EXCEPT: ERROR -> Something weird is going on."
                " PAL_EXCEPT was hit without PAL_TRY being hit.\n");
        }
        bExcept = TRUE; /* indicate we hit the PAL_EXCEPT block */
    }
    PAL_ENDTRY;

    if (!bTry)
    {
        Trace("PAL_TRY_EXCEPT: ERROR -> It appears the code in the PAL_TRY"
            " block was not executed.\n");
    }

    if (!bExcept)
    {
        Trace("PAL_TRY_EXCEPT: ERROR -> It appears the code in the PAL_EXCEPT"
            " block was not executed.\n");
    }

    /* did we hit all the code blocks? */
    if(!bTry || !bExcept)
    {
        Fail("");
    }


    PAL_Terminate();  
    return PASS;

}
