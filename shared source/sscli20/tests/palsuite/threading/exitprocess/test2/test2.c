/*=============================================================================
**
** Source: test2.c
**
** Purpose: Positive test for ExitProcess.
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
**===========================================================================*/
#include <palsuite.h>



int __cdecl main( int argc, char **argv ) 

{
    /* call ExitProcess() -- should work without PAL_Initialize() */
    ExitProcess(PASS);

    
    /* return failure if we reach here -- note no attempt at       */
    /* meaningful output because we never called PAL_Initialize(). */
    return FAIL; 
}
