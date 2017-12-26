/*=============================================================================
**
** Source: test1.c (WSAIoctl)
**
** Purpose: Test to ensure WSAIoctl fails without a WSAStartup call first.
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Fail
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
    DWORD dwSocketError;

    SOCKET socketID = INVALID_SOCKET;

    /* Variables needed by WSAIoctl */

    WSAOVERLAPPED   wsaIoctlOverlapped;
    DWORD           dwIoctlBytesReturned;
    DWORD           dwInBuffer;
    DWORD           dwInBufferLen = sizeof(dwInBuffer);
    char            *pOutBuffer[255];
    DWORD           dwOutBufferLen = 255;


    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }
    
    /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaIoctlOverlapped, 0, sizeof(WSAOVERLAPPED));

    /* attempt to set some bogus socket to non-blocking */
    dwInBuffer = 1;
    err = WSAIoctl( socketID, 
                    FIONBIO,
                    &dwInBuffer,
                    dwInBufferLen,
                    pOutBuffer,
                    dwOutBufferLen,
                    &dwIoctlBytesReturned,
                    &wsaIoctlOverlapped,
                    0 );

    if( err == 0 )
    {
        Fail( "WSAIoctl call succeeded without initializing winsock\n" );
    }

    /* check the socket-specific error code */    
    dwSocketError = GetLastError();
    if( dwSocketError != WSANOTINITIALISED )
    {
        Fail(   "WSAIoctl failed with error code %d, "
                "expected WSANOTINITIALISED\n",
                dwSocketError );
    }

    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}
