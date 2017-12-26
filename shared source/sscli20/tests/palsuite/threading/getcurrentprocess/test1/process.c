/*============================================================
**
** Source: GetCurrentProcess/test1/process.c
**
** Purpose:  Test for to see if the process GetCurrentProcess
** returns a handle to the current process or not.
**
** Dependencies: TerminateProcess 
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

    HANDLE hProcess; 
    
    if(0 != (PAL_Initialize(argc, argv)))
    {
        return ( FAIL );
    }
    
    hProcess = GetCurrentProcess();
    Trace ("Testing the handle returned by GetCurrentProcess\n");
    if ( 0 == ( TerminateProcess ( hProcess, PASS ) ) )
    {
	Fail ("Testing GetCurrentProcess, the TerminateProcess function "
		"failed.\n");
    }

    PAL_Terminate();
    return ( PASS );

}
