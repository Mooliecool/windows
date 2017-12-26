/*=============================================================================
**
** Source: test3.c
**
** Purpose: Test to ensure WSASocketA fails when called
**          before WSAStartup.
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
int __cdecl main( int argc, char **argv ) 
{
    /* local variables */
    DWORD dwSocketError;

    SOCKET sock = INVALID_SOCKET;


    /* PAL initialization */
    if ( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }


    /* try a socket call */
    sock = WSASocketA(  AF_INET,
                        SOCK_STREAM,
                        IPPROTO_IP,
                        NULL,
                        0,
                        WSA_FLAG_OVERLAPPED
                    );


    if ( sock != INVALID_SOCKET )
    {
        /* check the socket-specific error code */    
        closesocket( sock );
        Fail(   "WSASocketA call succeeded before call to WSAStartup\n" ); 
    }

    dwSocketError = GetLastError();
    if ( dwSocketError != WSANOTINITIALISED )
    {
        Fail(   "WSASocketA call failed with error code %d "
                "instead of WSANOTINITIALISED\n",
                dwSocketError ); 
    }
    
    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}

