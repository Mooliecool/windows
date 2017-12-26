/*=============================================================================
**
** Source: test4.c
**
** Purpose: Positive test for OpenEventW.
**
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               CreateEvent
**               CloseHandle
**               WaitForSingleObject
**
** Purpose:
**
** Test to ensure proper operation of the OpenEventW()
** API by trying to open an event with a name that is
** already taken by a non-event object.
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
    /* local variables */
    BOOL                    bRet = PASS;
    DWORD                   dwLastError = 0;
    HANDLE                  hMutex = NULL;
    HANDLE                  hTestEvent = NULL;
    LPSECURITY_ATTRIBUTES   lpSecurityAttributes = NULL;
    BOOL                    bInitialState = TRUE;
    WCHAR                   wcName[] = {'I','m','A','M','u','t','e','x','\0'};
    LPWSTR                  lpName = wcName;


    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }

    /* create a mutex object */
    hMutex = CreateMutexW(  lpSecurityAttributes,
                             bInitialState,
                             lpName );

    if( hMutex == NULL )
    {
        /* ERROR */
        Fail( "ERROR:%lu:CreateMutexW() call failed\n", GetLastError() );
    }

    /* open a new handle to our event */
    hTestEvent = OpenEventW(EVENT_ALL_ACCESS,  /* we want all rights */
                            FALSE,             /* no inherit         */
                            lpName );

    if( hTestEvent != NULL )
    {
        /* ERROR */
        Trace( "ERROR:OpenEventW() call succeeded against a named "
                "mutex, should have returned NULL\n" );
        if( ! CloseHandle( hTestEvent ) )
        {
            Trace( "ERROR:%lu:CloseHandle() call failed \n", GetLastError() );
        }
        bRet = FAIL;
    }
    else
    {
        dwLastError = GetLastError();
        if( dwLastError != ERROR_INVALID_HANDLE )
        {
            /* ERROR */
            Trace( "ERROR:OpenEventW() call failed against a named "
                    "mutex, but returned an unexpected result: %lu\n",
                    dwLastError );
            bRet = FAIL;
        }
    }


    /* close the mutex handle */
    if( ! CloseHandle( hMutex ) )
    {
        Trace( "ERROR:%lu:CloseHandle() call failed \n", GetLastError() );
        bRet = FAIL;
    }


    /* fail here if we weren't successful */
    if( bRet == FAIL )
    {
        Fail( "" );
    }


    /* PAL termination */
    PAL_Terminate();

    /* return success or failure */
    return PASS;
}


