/*=====================================================================
**
** Source:  pal_try_leave_finally.c
**
** Purpose: Tests the PAL implementation of the PAL_TRY, PAL_LEAVE  
**          and PAL_FINALLY functions.
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
    BOOL bFinally = FALSE;
    BOOL bLeave = TRUE;

    if (0 != PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    PAL_TRY 
    {
        bTry = TRUE;    /* indicate we hit the PAL_TRY block */

        goto Done;      /* let's blow this popsicle stand... */

        bLeave = FALSE; /* indicate we stuck around */
    Done: ;
    }
    PAL_FINALLY
    {
        bFinally = TRUE;    /* indicate we hit the PAL_FINALLY block */
    }
    PAL_ENDTRY;

    /* did we go where we were meant to go */
    if (!bTry)
    {
        Trace("PAL_TRY_FINALLY: ERROR -> It appears the code in the PAL_TRY"
            " block was not executed.\n");
    }

    if (!bLeave)
    {
        Trace("PAL_TRY_FINALLY: ERROR -> It appears code was executed after "
            "PAL_LEAVE was called. It should have jumped directly to the "
            "PAL_FINALLY block.\n");
    }

    if (!bFinally)
    {
        Trace("PAL_TRY_FINALLY: ERROR -> It appears the code in the PAL_FINALLY"
            " block was not executed.\n");
    }

    /* did we hit all the code blocks? */
    if(!bTry || !bLeave || !bFinally)
    {
        Fail("");
    }


    PAL_Terminate();  
    return PASS;

}
