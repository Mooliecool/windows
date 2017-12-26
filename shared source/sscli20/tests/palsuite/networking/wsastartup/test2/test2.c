/*=============================================================================
**
** Source: test2.c
**
** Purpose: Test to ensure WSAStartup works properly.
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Fail
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

    SOCKET sock = INVALID_SOCKET;


    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }

    /* make a socket call before calling WSAStartup -- it should fail */
    sock = WSASocketA(  AF_INET,
                        SOCK_STREAM,
                        IPPROTO_TCP,
                        NULL,
                        0,
                        WSA_FLAG_OVERLAPPED
                    );

    if( sock != INVALID_SOCKET )
    {
        Fail(   "Arbitrary socket call works before successful "
                "call to WSAStartup\n" ); 
    }

    /* check the socket-specific error code */    
    dwSocketError = GetLastError();
    if( dwSocketError != WSANOTINITIALISED )
    {
        Fail(   "Arbitrary socket call does not return WSANOTINITIALISED "
                "before successful call to WSAStartup\n" ); 
    }
    
    

    /* request winsock version 2.2 */
    wVersionRequested = MAKEWORD( 2, 2 );

    err = WSAStartup( wVersionRequested, &wsaData );
    if( err != 0 )
    {
        Fail( "Unexpected WSAStartup call failed with error code %d\n", 
              err ); 
    }

    /* Confirm that the WinSock DLL supports the specified version. */
    if( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 )
    {
        /* Tell the user that we could not find a usable winsock DLL. */
        WSACleanup();
        Fail(   "Requested winsock version unsupported, "
                "returned version %d.%d\n",
                LOBYTE( wsaData.wVersion ),
                HIBYTE( wsaData.wVersion ) ); 
    }


    /* try again to perform our socket call */
    sock = WSASocketA(  AF_INET,
                        SOCK_STREAM,
                        IPPROTO_TCP,
                        NULL,
                        0,
                        WSA_FLAG_OVERLAPPED
                    );

    if( sock == INVALID_SOCKET )
    {
        /* check the socket-specific error code */    
        dwSocketError = GetLastError();
        if( dwSocketError == WSANOTINITIALISED )
        {
            Fail(   "Arbitrary socket call fails, reports WSANOTINITIALISED "
                    "after a successful call to WSAStartup\n" ); 
        }
        
        /* we let this call potentially fail for other reasons */
    }
    else
    {
        closesocket( sock );
    }



    /* cleanup the winsock library */
    if( WSACleanup() != 0 )
    {
        dwSocketError = GetLastError();
        Fail(   "WSACleanup failed with error code %lu\n",
                dwSocketError );
    }
    

    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}
