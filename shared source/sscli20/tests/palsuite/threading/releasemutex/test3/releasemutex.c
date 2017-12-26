/*============================================================
**
** Source: ReleaseMutex/test3/ReleaseMutex.c
**
** Purpose: Test failure code for ReleaseMutex. 
**
** Dependencies: CreateMutex
**               ReleaseMutex
**               CreateThread
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

DWORD dwTestResult;  /* global for test result */

DWORD dwThreadId;  /* consumer thread identifier */

HANDLE hMutex;  /* handle to mutex */

HANDLE hThread;  /* handle to thread */

/* 
 * Thread function. 
 */
DWORD
PALAPI 
ThreadFunction( LPVOID lpNoArg )
{

    dwTestResult = ReleaseMutex(hMutex);

    return 0;
}

int __cdecl main (int argc, char **argv) 
{

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return (FAIL);
    }

    /*
     * set dwTestResult so test fails even if ReleaseMutex is not called
     */
    dwTestResult = 1;

    /*
     * Create mutex
     */
    hMutex = CreateMutexW (
	NULL,
	TRUE,
	NULL);

    if ( NULL == hMutex ) 
    {
        Fail ( "hMutex = CreateMutex () - returned NULL\n"
		 "Failing Test.\nGetLastError returned %d\n", GetLastError());
    }

    /* 
     * Create ThreadFunction
     */
    hThread = CreateThread(
	NULL, 
	0,    
	ThreadFunction,
	NULL,          
	0,             
	&dwThreadId);  

    if ( NULL == hThread ) 
    {

	Fail ( "CreateThread() returned NULL.  Failing test.\n"
		 "GetLastError returned %d\n", GetLastError());
    }
    
    /*
     * Wait for ThreadFunction to complete
     */
    WaitForSingleObject (hThread, INFINITE);
    
    if (dwTestResult)
    {
	Fail ("ReleaseMutex() test was expected to return 0.\n" 
		"It returned %d.  Failing test.\n", dwTestResult );
    }

    Trace ("ReleaseMutex() test returned 0.\nTest passed.\n");

    PAL_Terminate();
    return ( PASS );

}
