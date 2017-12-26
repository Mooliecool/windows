/*=============================================================================
**
** Source: test3.c
**
** Purpose: Positive test for ExitProcess.
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
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
**===========================================================================*/
#include <palsuite.h>



int __cdecl main( int argc, char **argv ) 

{
    /* initialize the PAL */
    if( PAL_Initialize(argc, argv) != 0 )
    {
	    return( FAIL );
    }

    /* terminate the PAL */
    PAL_Terminate();

    /* call ExitProcess() -- should work after PAL_Terminate() */
    ExitProcess( PASS );

    
    /* return failure if we reach here -- note no attempt at   */
    /* meaningful output because we've called PAL_Terminte().  */
    return FAIL; 
}
