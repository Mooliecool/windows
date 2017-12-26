/*============================================================
**
** Source: childprocess.c
**
** Purpose: Test to ensure ExitThread returns the right 
** value when shutting down the last thread of a process.
** All this program does is call ExitThread() with a predefined
** value.
**
** Dependencies: none
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
#include "myexitcode.h"

int __cdecl main( int argc, char **argv ) 
{
    /* initialize the PAL */
    if( PAL_Initialize(argc, argv) != 0 )
    {
	    return( FAIL );
    }
    
    /* exit the current thread with a magic test value -- it should */
    /* terminate the process and return that test value from this   */
    /* program.                                                     */
    ExitThread( TEST_EXIT_CODE );

    /* technically we should never get here */
    PAL_Terminate();
    
    /* return failure */
    return FAIL;
}
