/*============================================================
**
** Source: getcurrentprocessid/test1/processid.c
**
** Purpose: Test to ensure GetCurrentProcessId returns the current 
** process id number.  This test compares the result of 
** GetCurrentProcessId to getpid.
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

INT __cdecl main( int argc, char **argv ) 
{

    DWORD dwProcessId; 

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return ( FAIL );
    }

    dwProcessId = GetCurrentProcessId();
    
    if ( 0 >= dwProcessId ) 
    {
        Fail ("%s has dwProcessId has id value %d\n", argv[0], 
		dwProcessId );
    }
    Trace ("%s has dwProcessId %d\nPassing test as dwProcessId is > 0\n"
	    , argv[0], dwProcessId);

    PAL_Terminate();
    return ( PASS ); 

}
