/*============================================================
**
** Source: exitprocess/test1/exitprocess.c
**
** Purpose: Test to ensure ExitProcess returns the argument given
**          to it. 
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

int __cdecl main( int argc, char **argv ) 

{
    
    if(0 != (PAL_Initialize(argc, argv)))
    {
	return ( FAIL );
    }
 
    ExitProcess(PASS);

    Fail ("ExitProcess(0) failed to exit.\n  Test Failed.\n");

    return ( FAIL);

}
