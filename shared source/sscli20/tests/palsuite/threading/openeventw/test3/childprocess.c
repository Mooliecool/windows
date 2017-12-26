/*============================================================
**
** Source: childprocess.c
**
** Purpose: Test to ensure that OpenEventW() works when
** opening an event created by another process. The test
** program launches this program as a child, which creates
** a named, initially-unset event. The child waits up to
** 10 seconds for the parent process to open that event
** and set it, and returns PASS if the event was set or FAIL
** otherwise. The parent process checks the return value
** from the child to verify that the opened event was
** properly used across processes.
**
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               CreateEventW
**               WaitForSingleObject
**               CloseHandle
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
    /* local variables */
    DWORD                   dwRet = 0;
    HANDLE                  hEvent = NULL;
    LPSECURITY_ATTRIBUTES   lpEventAttributes = NULL;
    WCHAR                   wcName[] = {'P','A','L','R','o','c','k','s','\0'};
    LPWSTR                  lpName = wcName;


    /* initialize the PAL */
    if( PAL_Initialize(argc, argv) != 0 )
    {
	    return( FAIL );
    }

    /* create an event which we can use with SetEvent */
    hEvent = CreateEventW(  lpEventAttributes,
                            TRUE,               /* manual reset */
                            FALSE,              /* unsignalled  */
                            lpName );

    if( hEvent == NULL )
    {
        /* ERROR */
        Fail(   "ERROR:%lu:CreateEventW() call failed in child\n",
                GetLastError());
    }

    /* verify that the event is signalled by the parent process */
    dwRet = WaitForSingleObject( hEvent, 10000 );
    if( dwRet != WAIT_OBJECT_0 )
    {
        /* ERROR */
        Trace( "ERROR:WaitForSingleObject() call returned %lu, "
                "expected WAIT_OBJECT_0\n",
                dwRet );
        if( !CloseHandle( hEvent ) )
        {
            Trace(   "ERROR:%lu:CloseHandle() call failed in child\n",
                     GetLastError());
        }
        Fail( "test failed\n" );
    }


    /* close the event handle */
    if( ! CloseHandle( hEvent ) )
    {
        /* ERROR */
        Fail(   "ERROR:%lu:CloseHandle() call failed in child\n",
                GetLastError());
    }

    /* terminate the PAL */
    PAL_Terminate();

    /* return success or failure */
    return PASS;
}
