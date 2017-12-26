/*=============================================================================
**
** Source: test1.c
**
** Purpose: Test to ensure WSACleanup works properly. The test calls WSAStartup
**          5 times. It then calls (4 times) the trio wsacleanup, WSASocket, 
**          closesocket. On the fifth call to wsacleanup, wsasocket is 
**          expected to fail.
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
    int i,j;
    WORD wVersionRequested;
    WSADATA wsaData;
    DWORD dwSocketError;

    SOCKET sock = INVALID_SOCKET;


    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }

    /* try to cleanup the winsock library -- should fail */
    /* since we haven't initialized it yet */
    err = WSACleanup();
    if( err == 0 )
    {
        Fail(   "WSACleanup call succeeded before call to WSAStartup\n" );
    }
    
    /* check the socket-specific error code */    
    dwSocketError = GetLastError();
    if( dwSocketError != WSANOTINITIALISED )
    {
        Fail(   "WSACleanup call does not return WSANOTINITIALISED "
                "before successful call to WSAStartup\n" );
    }


    /* initialize winsock version 2.2 */
    wVersionRequested = MAKEWORD( 2, 2 );
    
    for(i=0;i<5;i++)
    {
        err = WSAStartup( wVersionRequested, &wsaData );
        if( err != 0 )
        {            
            for(j=0;j<i;j++)
            {
                err = WSACleanup();
                if( err != 0 )
                {
                    /* check the socket-specific error code */    
                    dwSocketError = GetLastError();
                    Trace(   "WSACleanup call fails with error code %d\n",
                            dwSocketError ); 
                }
            }
            Fail( "Unexpected WSAStartup call failed with error code %d\n", 
                err ); 
        }

        /* Confirm that the WinSock DLL supports the specified version. */
        if( LOBYTE( wsaData.wVersion ) != 2 ||
            HIBYTE( wsaData.wVersion ) != 2 )
        {
            /* Tell the user that we could not find a usable winsock DLL. */
            Trace( "Requested winsock version unsupported, "
                    "returned version %d.%d\n",
                    LOBYTE( wsaData.wVersion ),
                    HIBYTE( wsaData.wVersion ) ); 

            for(j=0;j<=i;j++)
            {
                err = WSACleanup();
                if( err != 0 )
                {
                    /* check the socket-specific error code */    
                    dwSocketError = GetLastError();
                    Trace(   "WSACleanup call fails with error code %d\n",
                            dwSocketError ); 
                }
            }
            
            return( FAIL );
        }
    }

    for(i=0;i<4;i++)
    {
        /* try to cleanup the winsock library -- should succeed now */
        err = WSACleanup();
        if( err != 0 )
        {
            /* check the socket-specific error code */    
            dwSocketError = GetLastError();
            Fail(   "WSACleanup call fails with error code %d\n",
                    dwSocketError ); 
        }

        /* make a socket call before calling WSAStartup -- it should fail */                        
        sock = WSASocketA(  AF_INET,
                            SOCK_STREAM,
                            IPPROTO_TCP,
                            NULL,
                            0,
                            WSA_FLAG_OVERLAPPED
                        );

        if( sock == INVALID_SOCKET )
        {
            Trace("Unexpected failure: WSASocketA returned error\n");
            for(j=i;j<5;j++)
            {
                /* try to cleanup the winsock library -- should succeed now */
                err = WSACleanup();
                if( err != 0 )
                {
                    /* check the socket-specific error code */    
                    dwSocketError = GetLastError();
                    Trace(   "WSACleanup call fails with error code %d\n",
                            dwSocketError ); 
                }
            }
            Fail("");
        }
        if(closesocket( sock )==SOCKET_ERROR)
        {
            Trace("Unexpected failure: CloseSocket returned error\n");            
            for(j=i;j<5;j++)
            {
                /* try to cleanup the winsock library -- should succeed now */
                err = WSACleanup();
                if( err != 0 )
                {
                    /* check the socket-specific error code */    
                    dwSocketError = GetLastError();
                    Trace(   "WSACleanup call fails with error code %d\n",
                            dwSocketError ); 
                }
            }
            Fail("");
        }


    }

    /* try to cleanup the winsock library -- should succeed now */
    err = WSACleanup(); /* handle the last wsastartup */
    if( err != 0 )
    {
        /* check the socket-specific error code */    
        dwSocketError = GetLastError();
        Fail(   "WSACleanup call fails with error code %d\n",
                dwSocketError ); 
    }

    /* try a socket call to make sure the library's no longer initialized */
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
        closesocket( sock );
        Fail(   "Arbitrary socket call works after a successful "
                "call to WSACleanup\n" ); 
    }
    else
    {
        /* check the socket-specific error code */    
        dwSocketError = GetLastError();
        if( dwSocketError != WSANOTINITIALISED )
        {
            Fail(   "Arbitrary socket call does not return WSANOTINITIALISED "
                    "after successful call to WSACleanup\n" ); 
        }
    }
    

    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}
