/*=============================================================================
**
** Source: test3.c (WSAIoctl)
**
** Purpose: Test to verify that WSAIoctl returns WSAENOTSOCK 
**          if it's called with INVALID_SOCKET.
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
    int err;
    WORD wVersionRequested;
    WSADATA wsaData;

    /* Variables needed by WSAIoctl */

    WSAOVERLAPPED   wsaIoctlOverlapped;
    DWORD           dwIoctlBytesReturned;
    u_long          unInBuffer;
    DWORD           dwInBufferLen = sizeof(unInBuffer);
    char            *pOutBuffer[255];
    DWORD           dwOutBufferLen = 255;

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

    /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaIoctlOverlapped, 0, sizeof(WSAOVERLAPPED));

    /* attempt to set some bogus socket to non-blocking */
    unInBuffer = 1;

    err = WSAIoctl( INVALID_SOCKET, 
                    FIONBIO,
                    &unInBuffer,
                    dwInBufferLen,
                    pOutBuffer,
                    dwOutBufferLen,
                    &dwIoctlBytesReturned,
                    &wsaIoctlOverlapped,
                    0 );

    if( (err == 0) ||
        (GetLastError() != WSAENOTSOCK) )
    { 
        Trace( "ERROR: "
               "WSAIoctl call returned %d "
               "when it should have returned WSAENOTSOCK\n",
                GetLastError() );

        /* Do some cleanup */
        DoWSATestCleanup( NULL,
                          0 );

        Fail("");

    }
    
    /* Do some cleanup */
    DoWSATestCleanup( NULL,
                      0 );
    
    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}
