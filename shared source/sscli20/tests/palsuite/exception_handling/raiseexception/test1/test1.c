/*=============================================================
**
** Source: test1.c
**
** Purpose: Tests that RaiseException throws a catchable exception
**          and Tests the behaviour of RaiseException with
**          PAL_FINALLY
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
    BOOL bExcept  = FALSE;
    BOOL bTry     = FALSE;
    BOOL bFinally = FALSE;

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    /*********************************************************
     * Tests that RaiseException throws a catchable exception
     */
    PAL_TRY
    {
        bTry = TRUE;
        RaiseException(0,0,0,0);

        Fail("RaiseException: ERROR -> code was executed after the "
             "exception was raised.\n");
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        bExcept = TRUE;
    }
    PAL_ENDTRY;

    if (!bTry)
    {
        Trace("RaiseException: ERROR -> It appears the code in the "
              "PAL_TRY block was not executed.\n");
    }

    if (!bExcept)
    {
        Trace("RaiseException: ERROR -> It appears the code in the "
              "PAL_EXCEPT_FILTER_EX block was not executed.\n");
    }

    /* did we hit all the code blocks? */
    if(!bTry || !bExcept)
    {
        Fail("");
    }

    /* Reinit flags */
    bTry = bExcept = FALSE;


    /*********************************************************
     * Tests the behaviour of RaiseException with
     * PAL_FINALLY
     * (bFinally should be set before bExcept)
     */
    PAL_TRY
    {
        PAL_TRY
        {
            bTry = TRUE;
            RaiseException(0,0,0,0);

            Fail("RaiseException: ERROR -> code was executed after the "
                 "exception was raised.\n");
        }
        PAL_FINALLY
        {
            bFinally = TRUE;
        }
        PAL_ENDTRY;
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        if( bFinally == FALSE )
        {
            Fail("RaiseException: ERROR -> It appears the code in the "
                 "PAL_EXCEPT executed before the code in PAL_FINALLY.\n");
        }

        bExcept = TRUE;
    }
    
    PAL_ENDTRY;

    if (!bTry)
    {
        Trace("RaiseException: ERROR -> It appears the code in the "
              "PAL_TRY block was not executed.\n");
    }

    if (!bExcept)
    {
        Trace("RaiseException: ERROR -> It appears the code in the "
              "PAL_EXCEPT block was not executed.\n");
    }

    if (!bFinally)
    {
        Trace("RaiseException: ERROR -> It appears the code in the "
              "PAL_FINALLY block was not executed.\n");
    }

    /* did we hit all the code blocks? */
    if(!bTry || !bExcept || !bFinally)
    {
        Fail("");
    }

    PAL_Terminate();
    return PASS;
}
