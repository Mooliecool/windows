/*=============================================================================
**
** Source: test3.c
**
** Purpose: Tests ioctlsocket by passing invalid argument data.
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Fail
**               WSAStartup
**               WSACleanup
**               WSASocketA
**               closesocket
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


/**
 * main
 * 
 * executable entry point
 */
INT __cdecl main( INT argc, CHAR **argv ) 
{
    /* local variables */
    int err;
    WORD wVersionRequested;
    WSADATA wsaData;
    DWORD dwSocketError;
    u_long pending;

    SOCKET sock = INVALID_SOCKET;


    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }


    /* initialize winsock version 2.2 */
    wVersionRequested = MAKEWORD( 2, 2 );

    err = WSAStartup( wVersionRequested, &wsaData );
    if( err != 0 ) {
        Fail( "Unexpected WSAStartup call failed with error code %d\n", 
              err ); 
    }

    /* Confirm that the WinSock DLL supports the specified version. */
    if( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 ) {
        /* Tell the user that we could not find a usable winsock DLL. */
        WSACleanup();
        Fail(   "Requested winsock version unsupported, "
                "returned version %d.%d\n", 
                LOBYTE( wsaData.wVersion ),
                HIBYTE( wsaData.wVersion ) ); 
    }


    /* create a socket */
    sock = WSASocketA(  AF_INET,
                        SOCK_STREAM,
                        IPPROTO_IP,
                        NULL,
                        0,
                        WSA_FLAG_OVERLAPPED
                    );


    if( sock == INVALID_SOCKET )
    {
        /* check the socket-specific error code */    
        dwSocketError = GetLastError();
        WSACleanup();
        Fail(   "WSASocket call failed with error code %d\n",
                dwSocketError ); 
    }
    
    /* try to check the number of bytes available for reading */
    pending = 1234;
    err = ioctlsocket( sock, FIONREAD , NULL );
    if( err == 0 )
    {
        /* close the socket */
        err = closesocket( sock );
        if( err != 0 )
        {
            /* check the socket-specific error code */    
            dwSocketError = GetLastError();
            Trace( "closesocket call failed with error code %d\n",
                    dwSocketError ); 
        }
        WSACleanup();        
        Fail(   "ioctlsocket call succeeded with null data argument\n" ); 
    }

    dwSocketError = GetLastError();
    if( dwSocketError != WSAEFAULT )
    {
        /* close the socket */
        err = closesocket( sock );
        if( err != 0 )
        {
            /* check the socket-specific error code */    
            Trace(   "closesocket call failed with error code %d\n",
                    GetLastError() ); 
        }
        WSACleanup();
        Fail(   "ioctlsocket call failed with error code %d, "
                "expected WSAEFAULT\n",
                dwSocketError ); 
    }

    /* try again, properly this time */
    err = ioctlsocket( sock, FIONREAD , &pending );
    if( err == SOCKET_ERROR )
    {        
        dwSocketError = GetLastError();
        /* close the socket */
        err = closesocket( sock );
        if( err != 0 )
        {
            /* check the socket-specific error code */    
            Trace(   "closesocket call failed with error code %d\n",
                    GetLastError() ); 
        }
        WSACleanup();
        Fail(   "ioctlsocket call failed with error code %d\n",
                dwSocketError ); 
    }
    
    /* check for the expected result */
    if( pending != 0 )
    {
        /* close the socket */
        err = closesocket( sock );
        if( err != 0 )
        {
            /* check the socket-specific error code */    
            dwSocketError = GetLastError();
            Trace(   "closesocket call failed with error code %d\n",
                    dwSocketError ); 
        }
        WSACleanup();
        Fail(   "ioctlsocket call returned %ul bytes to be read, "
                "expected 0\n",
                pending ); 
    }

    /* try to check the number of bytes available for reading */   
    pending = 1;
    err = ioctlsocket( INVALID_SOCKET, FIONBIO , &pending );
    if( err != SOCKET_ERROR || GetLastError()!= WSAENOTSOCK)
    {
        
         /* close the socket */
        err = closesocket( sock );
        if( err != 0 )
        {
            /* check the socket-specific error code */    
            dwSocketError = GetLastError();
            Trace(   "closesocket call failed with error code %d\n",
                    dwSocketError ); 
        }
        WSACleanup();
        Fail(   "ioctlsocket call succeeded with null data argument\n" ); 
    }    

    /* try again, with an invalid command */
    /* Disabled test: Invalid command
       The win32 application crash whan calling this
       err = ioctlsocket( sock, -1 , &pending );       
    */
    

    /* close the socket */
    err = closesocket( sock );
    if( err != 0 )
    {
        /* check the socket-specific error code */    
        dwSocketError = GetLastError();
        Fail(   "closesocket call failed with error code %d\n",
                dwSocketError ); 
    }
    
    

    /* cleanup the winsock library */
    err = WSACleanup();
    if( err != 0 )
    {
        /* check the socket-specific error code */    
        dwSocketError = GetLastError();
        Fail(   "WSACleanup call failed with error code %d\n",
                dwSocketError ); 
    }
    
    
    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}
