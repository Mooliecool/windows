/*=============================================================================
**
** Source: test1.c
**
** Purpose: Test to ensure ioctlsocket fails without a WSAStartup call first.
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
    u_long data;
    int err;
    DWORD dwSocketError;

    SOCKET sock = INVALID_SOCKET;


    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }
    
    /* attempt to set some bogus socket to non-blocking */
    data = 1;
    err = ioctlsocket( sock, FIONBIO, &data );

    if( err == 0 )
    {
        Fail( "ioctlsocket call succeeded without initializing winsock\n" );
    }

    /* check the socket-specific error code */    
    dwSocketError = GetLastError();
    if( dwSocketError != WSANOTINITIALISED )
    {
        Fail(   "ioctlsocket failed with error code %d, "
                "expected WSANOTINITIALISED\n",
                dwSocketError );
    }

    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}
