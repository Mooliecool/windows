/*=============================================================================
**
** Source: wsarecv1_server.c (WSASend, WSARecv)
**
** Purpose: Test to ensure that WSARecv returns WSANOTINITIALISED when
**          WSA services are closed.
**
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Fail
**               WSAStartup
**               WSACleanup
**               socket
**               memset
**               closesocket
**               GetLastError
**               listen
**               accept
**
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
 * Global variables
 */


const int       listenBacklog   = 1;    /* backlog for listen */

/**
 * main
 * 
 * executable entry point
 */
INT __cdecl main(INT argc, CHAR **argv)
{
    int     i;
    int     err;
    int     addrlen = sizeof(struct sockaddr);
    struct  sockaddr_in mySockaddr;
    WSADATA wsaData;

    /* Sockets descriptor */
    const int numSockets = 2;    /* number of sockets used in this test */

    SOCKET testSockets[2];

    /* Variables needed for setsockopt */
    BOOL bReuseAddr = TRUE;

    /* Variables needed for WSARecv */
    DWORD         dwRecvFlags = 0;

    /* Variables needed for select */
    struct timeval waitTime;
    fd_set readFds;
    int    socketFds;

    /* Socket DLL version */
    const WORD wVersionRequested = MAKEWORD(2,2);

    /* Sockets initialization to INVALID_SOCKET */
    for( i = 0; i < numSockets; i++ )
    {
        testSockets[i] = INVALID_SOCKET;
    }

    /* PAL initialization */
    if( PAL_Initialize(argc, argv) != 0 )
    {
        return FAIL;
    }
   
    /* Initialize to use winsock2.dll */
    err = WSAStartup( wVersionRequested,
                      &wsaData);

    if(err != 0)
    {
        Fail( "ERROR: Unexpected failure: "
              "WSAStartup(%i) "
              "returned %d\n",
              wVersionRequested, 
              GetLastError() );
    }

    /* Confirm that the WinSock DLL supports 2.2.
       Note that if the DLL supports versions greater    
       than 2.2 in addition to 2.2, it will still return
       2.2 in wVersion since that is the version we      
       requested.                                        
    */
    if ( wsaData.wVersion != wVersionRequested ) 
    {
         
        Trace("ERROR: Unexpected failure "
              "to find a usable version of WinSock DLL\n");

        /* Do some cleanup */
        DoWSATestCleanup( 0, 0 );

        Fail("");
    }

    /* create an overlapped stream socket in AF_INET domain */

    testSockets[0] = WSASocketA( AF_INET, 
                                 SOCK_STREAM, 
                                 IPPROTO_TCP,
                                 NULL, 
                                 0, 
                                 WSA_FLAG_OVERLAPPED ); 


    if( testSockets[0] == INVALID_SOCKET )

    {
        Trace("ERROR: Unexpected failure: "
              "WSASocketA"
              "(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED)) "
              " returned %d\n",
              GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( 0, 0 );

        Fail("");
    }

    /* Allows the socket to be bound to an address that is already in use. */
    err = setsockopt( testSockets[0],
                      SOL_SOCKET,
                      SO_REUSEADDR,
                      (const char *)&bReuseAddr,
                      sizeof( BOOL ) );

    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "setsockopt(.., SOL_SOCKET,SO_REUSEADDR, ..) "
              "returned %d\n",
              GetLastError() );

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    /* prepare the sockaddr structure */

    mySockaddr.sin_family           = AF_INET;
    mySockaddr.sin_port             = getRotorTestPort();
    mySockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    memset( &(mySockaddr.sin_zero), 0, 8);

    /* bind local address to a socket */
    err = bind( testSockets[0],
                (struct sockaddr *)&mySockaddr,
                sizeof(struct sockaddr) );


    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "bind() socket with local address "
              "returned %d\n",
              GetLastError() );

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    /* listen to the socket */
    err = listen( testSockets[0], 
                  listenBacklog );

    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "listen() to sockets "
              "returned %d\n",
              GetLastError() );

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    /* set the server waiting time as 10 seconds */
    waitTime.tv_sec = 10L;
    waitTime.tv_usec = 0L;

    /* initialize the except socket set  */
    FD_ZERO(&readFds);

    /* add socket to readable socket set */
    FD_SET( testSockets[0], 
            &readFds);

    /* monitor the readable socket set   */
    socketFds = select( 0,
                        &readFds,
                        NULL,
                        NULL,
                        &waitTime);

    

    if( socketFds == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure "
              "with select\n");

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    if( socketFds == 0 )
    {
        Trace("ERROR: Unexpected select "
              "timed out\n");

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    /* accept connection */
    testSockets[1] = accept( testSockets[0],
                             (struct sockaddr *)&mySockaddr,
                             &addrlen );

    if( testSockets[1] == INVALID_SOCKET )
    {
        Trace("ERROR: Unexpected failure: "
              "accept() connection on socket "
              "returned %d\n",
              GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    /* no send data needed */

    DoWSATestCleanup( testSockets,
                      numSockets );


    PAL_Terminate();
    return PASS;
}

