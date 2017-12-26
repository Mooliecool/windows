/*=============================================================================
**
** Source: test4.c
**
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               CreateEvent
**               CloseHandle
**               WaitForSingleObject
**               DuplicateHandle
**               GetCurrentProcess
**
** Purpose:
**
** Test to ensure proper operation of the SetEvent()
** API by calling it on an event handle that's the
** result of a DuplicateHandle() call on another event
** handle.
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
    DWORD                   dwRet = 0;
    HANDLE                  hEvent = NULL;
    HANDLE                  hDupEvent = NULL;
    LPSECURITY_ATTRIBUTES   lpEventAttributes = NULL;
    BOOL                    bManualReset = TRUE; 
    BOOL                    bInitialState = FALSE;
    LPCTSTR                 lpName = "OhYeah";


    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }


    /* create an event which we can use with SetEvent */
    hEvent = CreateEvent(   lpEventAttributes, 
                            bManualReset,
                            bInitialState,
                            lpName );

    if( hEvent == INVALID_HANDLE_VALUE )
    {
        /* ERROR */
        Fail( "ERROR:%lu:CreateEvent() call failed\n", GetLastError() );
    }
    
    /* verify that the event isn't signalled yet */
    dwRet = WaitForSingleObject( hEvent, 0 );
    if( dwRet != WAIT_TIMEOUT )
    {
        /* ERROR */
        Trace( "ERROR:WaitForSingleObject() call returned %lu, "
                "expected WAIT_TIMEOUT\n",
                dwRet );
        CloseHandle( hEvent );
        Fail( "Test failed\n" );
    }
    
    
    /* duplicate the event handle */
    if( ! (DuplicateHandle(
                            GetCurrentProcess(),
                            hEvent,
                            GetCurrentProcess(), 
                            &hDupEvent,
                            GENERIC_READ|GENERIC_WRITE, /* ignored in PAL */
                            FALSE, 
                            DUPLICATE_SAME_ACCESS ) ) )
    {
        Trace("ERROR:%u:DuplicateHandle() call failed\n",
             GetLastError() );
        CloseHandle( hEvent );
        Fail("Test failed\n");
    }

    /* verify that the event isn't signalled yet with the duplicate handle */
    dwRet = WaitForSingleObject( hDupEvent, 0 );
    if( dwRet != WAIT_TIMEOUT )
    {
        /* ERROR */
        Trace( "ERROR:WaitForSingleObject() call returned %lu, "
                "expected WAIT_TIMEOUT\n",
                dwRet );
        CloseHandle( hEvent );
        CloseHandle( hDupEvent );
        Fail( "Test failed\n" );
    }


    /* set the event using the duplicate handle */
    if( ! SetEvent( hDupEvent ) )
    {
        /* ERROR */
        Trace( "ERROR:%lu:SetEvent() call failed\n", GetLastError() );
        CloseHandle( hEvent );
        CloseHandle( hDupEvent );
        Fail( "Test failed\n" );
    }
    
    /* verify that the event is signalled using the duplicate handle*/
    dwRet = WaitForSingleObject( hDupEvent, 0 );
    if( dwRet != WAIT_OBJECT_0 )
    {
        /* ERROR */
        Trace( "ERROR:WaitForSingleObject() call returned %lu, "
                "expected WAIT_OBJECT_0\n",
                dwRet );
        CloseHandle( hEvent );
        Fail( "Test failed\n" );
    }
    
    /* verify that the event is signalled using the original event handle */
    dwRet = WaitForSingleObject( hEvent, 0 );
    if( dwRet != WAIT_OBJECT_0 )
    {
        /* ERROR */
        Trace( "ERROR:WaitForSingleObject() call returned %lu, "
                "expected WAIT_OBJECT_0\n",
                dwRet );
        CloseHandle( hEvent );
        Fail( "Test failed\n" );
    }
    

    /* close the duplicate event handle */
    if( ! CloseHandle( hDupEvent ) )
    {
        Fail( "ERROR:%lu:CloseHandle() call failed for duplicate handle\n",
                GetLastError() );
    }
    
    
    /* close the event handle */
    if( ! CloseHandle( hEvent ) )
    {
        Fail( "ERROR:%lu:CloseHandle() call failed for original handle\n",
                GetLastError() );
    }

    
    /* PAL termination */
    PAL_Terminate();
    
    /* return success */
    return PASS;
}
