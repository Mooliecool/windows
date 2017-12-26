/*=============================================================================
**
** Source: test8.c
**
** Purpose: tests bad AF, Type, and Protocol -1 parameter,
**          
**          
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
int __cdecl main( int argc, char **argv ) 
{
    /* local variables */
    int err;
    WORD wVersionRequested;
    WSADATA wsaData;
    DWORD dwSocketError;

    SOCKET sock = INVALID_SOCKET;


    /* PAL initialization */
    if ( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
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
        WSACleanup();
        Fail(   "Requested winsock version unsupported, "
                "returned version %d.%d\n", 
                LOBYTE( wsaData.wVersion ),
                HIBYTE( wsaData.wVersion ) ); 
    }

    /* try the WSASocketA call */
    sock = WSASocketA(  -1,
                        SOCK_RDM,
                        IPPROTO_IP,
                        NULL,
                        0,
                        WSA_FLAG_OVERLAPPED
                    );


    if ( sock != INVALID_SOCKET )
    {
        closesocket( sock );
        WSACleanup();
        Fail(   "WSASocketA call succeeded using an invalid combination of ",
                "address family (AF_INET) and protocol (IPPROTO_TCP) with "
                "a socket type of SOCK_RDM.\n" ); 
    }

    dwSocketError = GetLastError();
     /* should return WSAESOCKTNOSUPPORT or WSAEPROTONOSUPPORT */
    if ( (dwSocketError != WSAESOCKTNOSUPPORT) && 
         (dwSocketError != WSAEPROTONOSUPPORT))
    {
        WSACleanup();
        Fail(   "WSASocketA call failed with error code %u, "
                "expected WSAESOCKTNOSUPPORT\n",
                dwSocketError ); 
    }

    /* try the WSASocketA call */
    sock = WSASocketA(  AF_INET,
                        -1,
                        IPPROTO_IP,
                        NULL,
                        0,
                        WSA_FLAG_OVERLAPPED
                    );


    if ( sock != INVALID_SOCKET )
    {
        closesocket( sock );
        WSACleanup();
        Fail(   "WSASocketA call succeeded using an invalid combination of ",
                "address family (AF_INET) and protocol (IPPROTO_TCP) with "
                "a socket type of SOCK_RDM.\n" ); 
    }

    dwSocketError = GetLastError();
     /* should return WSAESOCKTNOSUPPORT or WSAEPROTONOSUPPORT */
    if ( (dwSocketError != WSAESOCKTNOSUPPORT) && 
         (dwSocketError != WSAEPROTONOSUPPORT))
    {
        WSACleanup();
        Fail(   "WSASocketA call failed with error code %u, "
                "expected WSAESOCKTNOSUPPORT\n",
                dwSocketError ); 
    }

    /* try the WSASocketA call */
    sock = WSASocketA(  AF_INET,
                        SOCK_RDM,
                        -1,
                        NULL,
                        0,
                        WSA_FLAG_OVERLAPPED
                    );


    if ( sock != INVALID_SOCKET )
    {
        closesocket( sock );
        WSACleanup();
        Fail(   "WSASocketA call succeeded using an invalid combination of ",
                "address family (AF_INET) and protocol (IPPROTO_TCP) with "
                "a socket type of SOCK_RDM.\n" ); 
    }

    dwSocketError = GetLastError();
     /* should return WSAESOCKTNOSUPPORT or WSAEPROTONOSUPPORT */
    if ( (dwSocketError != WSAESOCKTNOSUPPORT) && 
         (dwSocketError != WSAEPROTONOSUPPORT))
    {
        WSACleanup();
        Fail(   "WSASocketA call failed with error code %u, "
                "expected WSAESOCKTNOSUPPORT\n",
                dwSocketError ); 
    }

    /* cleanup the winsock library */
    err = WSACleanup();
    if ( err != 0 )
    {
        /* check the socket-specific error code */    
        dwSocketError = GetLastError();
        Fail(   "WSACleanup call failed with error code %u\n",
                dwSocketError ); 
    }
    
    
    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}
