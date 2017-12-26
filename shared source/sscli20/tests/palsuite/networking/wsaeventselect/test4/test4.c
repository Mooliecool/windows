/*=============================================================================
**
** Source: test4.c
**
** Purpose: Tests WSAEventSelect
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Fail
**               WSAStartup
**               WSACleanup
**               memset
**               socket
**               closesocket
**               connect
**               WSACreateEvent
**               GetLastError
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


/* helper function for cleaning up the winsock library */
static void CleanupWinsock( void )
{
    int err;
    DWORD dwError;

    err = WSACleanup();
    if( err == SOCKET_ERROR )
    {
        dwError = GetLastError();
        Trace(   "WSACleanup() call failed with error code %lu\n",
                dwError );
    }
}

/* helper function for closing a socket */
static void CloseSocket( SOCKET sock )
{
    int err;
    DWORD dwError;

    err = closesocket( sock );
    if( err == SOCKET_ERROR )
    {
        dwError = GetLastError();
        Trace(  "closesocket() call failed with error code %lu\n",
                dwError );
    }
}


/* helper function for closing a handle */
static void MyCloseHandle( HANDLE h )
{
    DWORD dwError;

    if( ! CloseHandle( h ) )
    {
        dwError = GetLastError();
        Trace(  "CloseHandle() call failed with error code %lu\n",
                dwError );
    }
}



/**
 * main
 * 
 * executable entry point
 */
int __cdecl main( int argc, char **argv ) 
{
    /* local variables */
    int err;
    WORD wVersionRequested;
    WSADATA wsaData;
    DWORD dwError;
    
    HANDLE hEvent;

    SOCKET sock = INVALID_SOCKET;



    /* PAL initialization */
    if ( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }


    /* call WSAEventSelect -- should fail because winsock not initialized */
    err = WSAEventSelect(   sock,
                            NULL,
                            FD_CONNECT | FD_READ | FD_WRITE );


    if( err != SOCKET_ERROR )
    {
        Trace( "WSAEventSelect call succeeded before winsock initialized\n" );
        CleanupWinsock();
        Fail( "test failed\n" );
    }

    dwError = GetLastError();
    if( dwError != WSANOTINITIALISED )
    {
        Fail(  "WSAEventSelect() call failed with error code %lu, "
                "expected WSANOTINITIALISED\n",
                dwError );
    }


    /* initialize winsock version 2.2 */
    wVersionRequested = MAKEWORD( 2, 2 );

    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        Fail( "Unexpected WSAStartup call failed with error code %d\n", 
              err ); 
    }

    /* Confirm that the WinSock DLL supports the specified version. */
    if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 )
    {
        /* Tell the user that we could not find a usable winsock DLL. */
        CleanupWinsock();
        Fail(   "Requested winsock version unsupported, "
                "returned version %d.%d\n", 
                LOBYTE( wsaData.wVersion ),
                HIBYTE( wsaData.wVersion ) ); 
    }


    
    /* create an event for socket notifications */    
    hEvent = CreateEvent( NULL, FALSE, FALSE, "foo" );
    if( hEvent == NULL )
    {
        dwError = GetLastError();
        Trace(  "CreateEvent() call failed with error code %lu\n",
                dwError );
        CleanupWinsock();
        Fail( "test failed\n" );
    }

    

    /* call WSAEventSelect -- should fail because socket is no good */
    err = WSAEventSelect(   sock,
                            hEvent,
                            FD_CONNECT | FD_READ | FD_WRITE );


    if( err != SOCKET_ERROR )
    {
        Trace( "WSAEventSelect call succeeded on a non-socket\n" );
        MyCloseHandle( hEvent );
        CleanupWinsock();
        Fail( "test failed\n" );
    }

    dwError = GetLastError();
    if( dwError != WSAENOTSOCK )
    {
        Trace(  "WSAEventSelect() call failed with error code %lu, "
                "expected WSAENOTSOCK\n",
                dwError );
        MyCloseHandle( hEvent );
        CleanupWinsock();
        Fail( "test failed\n" );
    }


    /* close the handle */
    if( ! CloseHandle( hEvent ) )
    {
        dwError = GetLastError();
        Trace(  "CloseHandle() call failed with error code %lu\n",
                dwError );
        CleanupWinsock();
        Fail( "test failed\n" );
    }
    

    /* create a stream socket in Internet domain */
    sock = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
    if ( sock == INVALID_SOCKET )
    {
        /* check the socket-specific error code */    
        dwError = GetLastError();
        Trace( "socket() call failed with error code %lu\n",
                dwError );
        CleanupWinsock();
        Fail( "test failed\n" );
    }

    /* call WSAEventSelect -- should fail because the event is no good */
    err = WSAEventSelect(   sock,
                            NULL,
                            FD_CONNECT | FD_READ | FD_WRITE );


    if( err != SOCKET_ERROR )
    {
        Trace( "WSAEventSelect call succeeded with a NULL event\n" );
        CleanupWinsock();
        CloseSocket( sock );
        Fail( "test failed\n" );
    }

    dwError = GetLastError();
    if( dwError != WSAEINVAL )
    {
        Trace(  "WSAEventSelect() call failed with error code %lu, "
                "expected WSAEINVAL\n",
                dwError );
        CloseSocket( sock );
        CleanupWinsock();
        Fail( "test failed\n" );
    }
    
    
    /* close the socket */
    err = closesocket( sock );
    if ( err != 0 )
    {
        Trace( "closesocket() call failed for accept socket\n" );
        CleanupWinsock();
        Fail( "test failed\n" );
    }





    /* cleanup the winsock library */
    err = WSACleanup();
    if ( err != 0 )
    {
        /* check the socket-specific error code */    
        dwError = GetLastError();
        Fail(   "WSACleanup call failed with error code %d\n",
                dwError ); 
    }
    
    
    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}

