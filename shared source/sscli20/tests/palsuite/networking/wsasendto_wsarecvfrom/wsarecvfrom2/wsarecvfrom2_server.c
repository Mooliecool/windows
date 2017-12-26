/*=============================================================================
**
** Source: wsarecvfrom2_server.c (WSARecvFrom)
**
** Purpose: Test to ensure that WSARecvFrom returns the WSAEINVAL
**          when no bind is done.
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Fail
**               Trace
**               WSAStartup
**               WSACleanup
**               WSASocketA
**               memset
**               GetLastError
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
 * Defines
 */

#define RECV_BUF_COUNT 1        /* number of recv buffer */
#define RECV_BUF_SIZE  255     /* buffer size max tcp:4328 */

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
    struct  sockaddr_in mySockaddr;
    WSADATA wsaData;    

    /* Variables needed for WSARecv */
    WSABUF        wsaBuf[RECV_BUF_COUNT];
    DWORD         dwNbrOfBuf  = RECV_BUF_COUNT;
    DWORD         dwNbrOfByteSent;
    WSAOVERLAPPED wsaOverlapped;

    char myBuffer[RECV_BUF_SIZE] = "violets are blue, roses are red"; 
    
    int addrlen = sizeof(struct sockaddr);

    /* Sockets descriptor */
    const int numSockets = 2;    /* number of sockets used in this test */

    SOCKET testSockets[2];

    /* Variables needed for WSARecv */
    DWORD         dwRecvFlags = 0;

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
    err = WSAStartup(wVersionRequested,
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
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    /* create an overlapped UDP socket in AF_INET domain */

    testSockets[0] = WSASocketA( AF_INET, 
                           SOCK_DGRAM, 
                           IPPROTO_UDP,
                           NULL, 
                           0, 
                           WSA_FLAG_OVERLAPPED ); 


    if( testSockets[0] == INVALID_SOCKET )

    {
         Trace("ERROR: Unexpected failure: "
              "WSASocketA"
              "(AF_INET,SOCK_DGRAM,IPPROTO_UDP,NULL,0,WSA_FLAG_OVERLAPPED)) "
              " returned %d\n",
              GetLastError());

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

    /* No bind of a local address to a socket */ 

    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "WSASocketA"
              "(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED)) "
              " returned %d\n",
              GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }    
    

    /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED));

    /* Initialize the WSABUF structure */
    wsaBuf[0].buf = myBuffer;
    wsaBuf[0].len = RECV_BUF_SIZE;

    /* Prepare to receive data */
    err = WSARecvFrom( testSockets[0],
                       wsaBuf,
                       dwNbrOfBuf,
                       &dwNbrOfByteSent,
                       &dwRecvFlags,
                       (struct sockaddr *)&mySockaddr,
                       &addrlen,
                       &wsaOverlapped,
                       0 );
    
    if( err != SOCKET_ERROR)      
    {
        
        Trace("ERROR: "
              "WSARecvFrom(...) "
              "returned %d, expected SOCKET_ERROR\n",
              err );  

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );
        
        Fail("");
    }      
    else if( GetLastError()!=WSAEINVAL )
    {
         Trace("ERROR: "
              "WSARecvFrom() returned %d, expected WSAEINVAL.\n",
              GetLastError() );

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    } 

    /* Do some cleanup */
    DoWSATestCleanup( testSockets,
                      numSockets );


    PAL_Terminate();
    return PASS;
};

