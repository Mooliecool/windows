/*============================================================
**
** Source: childprocess.c
**
** Purpose: Test to ensure DuplicateHandle works properly.
**
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               CreateMutexW
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
#include "myexitcode.h"


int __cdecl main( int argc, char **argv )
{
    HANDLE hMutex;
    WCHAR wszMutexName[] = { 'T','E','S','T','1','1','\0' };
    DWORD dwRet;
    int i;

    /* initialize the PAL */
    if( PAL_Initialize(argc, argv) != 0 )
    {
	    return( FAIL );
    }

    /* open a mutex to synchronize with the parent process */
    hMutex = CreateMutexW( NULL, FALSE, wszMutexName );
    if( hMutex == NULL )
    {
        Fail( "ERROR:%lu:CreateMutex() call failed\r\n", GetLastError() );
    }

    /* acquire the mutex lock */
    dwRet = WaitForSingleObject( hMutex, 10000 );
    if( dwRet != WAIT_OBJECT_0 )
    {
        Trace( "ERROR:WaitForSingleObject() returned %lu, "
                "expected WAIT_OBJECT_0",
                dwRet );
        if( ! CloseHandle( hMutex ) )
        {
            Trace( "ERROR:%lu:CloseHandle() call failed\n", GetLastError() );
        }
        Fail( "test failed\n" );
    }


    /* simulate some activity  */
    for( i=0; i<50000; i++ )
        ;

    /* close our mutex handle */
    if( ! CloseHandle( hMutex ) )
    {
        Fail( "ERROR:%lu:CloseHandle() call failed\n", GetLastError() );
    }

    /* terminate the PAL */
    PAL_Terminate();

    /* return the predefined exit code */
    return TEST_EXIT_CODE;
}
