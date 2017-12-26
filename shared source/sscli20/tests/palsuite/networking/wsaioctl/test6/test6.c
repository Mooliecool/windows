/*=============================================================================
**
** Source: test6.c (WSAIoctl)
**
** Purpose: Tests WSAIoctl checking the amount of data 
**          that can be read automatically from a socket.
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
#include "wsacommon.h"

/**
 * main
 * 
 * executable entry point
 */
int __cdecl main( int argc, char **argv ) 
{
    /* local variables */
    int i;
    int err;
    WORD wVersionRequested;
    WSADATA wsaData;

    /* Sockets descriptor */
    const int numSockets = 1;    /* number of sockets used in this test */
    SOCKET    testSockets[1];    /* test's socket list */

    /* Variables needed by WSAIoctl */

    WSAOVERLAPPED   wsaIoctlOverlapped;
    DWORD           dwIoctlBytesReturned;
    u_long          unOutBuffer;
    DWORD           dwOutBufferLen = sizeof(unOutBuffer);

    /* Sockets initialization to INVALID_SOCKET */
    for( i = 0; i < numSockets; i++ )
    {
        testSockets[i] = INVALID_SOCKET;
    }

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
    if( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 )
    {
        /* Tell the user that we could not find a usable winsock DLL. */
        Trace( "ERROR: Unexpected failure: "
               "Requested winsock version unsupported, "
               "returned version %d.%d\n", 
               LOBYTE( wsaData.wVersion ),
               HIBYTE( wsaData.wVersion ) );

        /* Do some cleanup */
        DoWSATestCleanup( NULL,
                          0 );

        Fail("");

    }


    /* create a stream socket in Internet domain */
    testSockets[0] = WSASocketA(  AF_INET,
                                  SOCK_STREAM,
                                  IPPROTO_IP,
                                  NULL,
                                  0,
                                  WSA_FLAG_OVERLAPPED
                               );


    if( testSockets[0] == INVALID_SOCKET )
    {
        /* check the socket-specific error code */    
        Trace( "ERROR: Unexpected failure: "
               "WSASocket call failed with error code %d\n",
                GetLastError() );
        
        /* Do some cleanup */
        DoWSATestCleanup( NULL,
                          0 );

        Fail("");
    }

    /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaIoctlOverlapped, 0, sizeof(WSAOVERLAPPED));

    err = WSAIoctl( testSockets[0], 
                    FIONREAD,
                    NULL,
                    0,
                    &unOutBuffer,
                    dwOutBufferLen,
                    &dwIoctlBytesReturned,
                    &wsaIoctlOverlapped,
                    0 );

    /* Check for an error */
    if( err == SOCKET_ERROR )
    { 
        Trace( "ERROR: "
               "WSAIoctl call failed with error code %d "
               "when it should have returned no error\n",
                GetLastError() );

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");

    }

    /* Make sure the return value is OK */
    if( unOutBuffer != 0 )
    {
        Trace( "ERROR: "
               "WSAIoctl returned %d "
               "on an FIONREAD call "
               "when it should have returned 0\n",
                unOutBuffer );

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }


    
    /* Do some cleanup */
    DoWSATestCleanup( testSockets,
                      numSockets );
    
    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}
