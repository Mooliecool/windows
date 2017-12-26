/*=============================================================================
**
** Source: wsarecvfrom3_server.c (WSARecvFrom)
**
** Purpose: Test to ensure that WSARecvFrom returns WSAEOPNOTSUPP
**          error when specifying MSG_OOB flags.
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Fail
**               WSAStartup
**               WSACleanup
**               WSASocketA
**               memset
**               bind
**               GetLastError
**               
**               
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
const DWORD     waitTimeout     = 5000; /* five seconds should be enough */

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
    BOOL          testFailed = TRUE; 

    /* Sockets descriptor */
    const int numSockets = 2;    /* number of sockets used in this test */

    SOCKET testSockets[2];

    /* Variables needed for setsockopt */
    BOOL bReuseAddr = TRUE;

    /* Variables needed for WSARecv */
    DWORD         dwRecvFlags = MSG_OOB;

    char myBuffer[RECV_BUF_SIZE] = "violets are blue, roses are red"; 
    
    int addrlen = sizeof(struct sockaddr);

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
              "(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED)) "
              " returned %d\n",
              GetLastError());

        DoWSATestCleanup( testSockets,
                          numSockets );
    
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

    /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED));

    /* Specify which event to signal when data is arrived*/
    wsaOverlapped.hEvent = CreateEvent ( 
                                 NULL, 
                                 FALSE, 
                                 FALSE, 
                                 NULL );
                                 
    if (NULL == wsaOverlapped.hEvent)
    {
        Trace("Unable to create event object (LastError=%d)\n", GetLastError());
        DoWSATestCleanup( testSockets,
                          numSockets );
    
        Fail("");
    }
    
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
        
        goto cleanup;
    }      
    else
    {
        DWORD dwLastError;

        dwLastError = GetLastError();
        
        /* pending overlapped operation.
           we need to retrieve the last error from the internal
           field of WSAOVERLAPPED.
        */
        if ( dwLastError == WSA_IO_PENDING)
        {
            DWORD dwWaitResult;
        
            dwWaitResult = WaitForSingleObject(
                                wsaOverlapped.hEvent, 
                                waitTimeout);
                                
            switch(dwWaitResult)
            {
            case WAIT_OBJECT_0:
                /* last error is set in Internal field */
                dwLastError = wsaOverlapped.Internal;
                break;
            default:
                Trace("overlapped operation was not signaled in time."
                      "WFSO returned %d\n", dwWaitResult);
                goto cleanup;
            }
        }
        else
        {
            /* operation got carried over right away. LastError value
               correspond to the result of the requested operaton.
               Nothing to do. */
        }
       
        if( dwLastError != WSAEOPNOTSUPP)
        {
        
            Trace("ERROR: "
                  "WSARecvFrom(...) sets LastError to %d expected WSAEOPNOTSUPP\n",
                  dwLastError );

            goto cleanup;
        }

    }
    testFailed = FALSE;

cleanup:
    if (FALSE == CloseHandle(wsaOverlapped.hEvent))
    {
        Trace("ERROR: "
              "Could not close handle %x. LastError is %d\n",
              wsaOverlapped.hEvent,
              GetLastError());
    }
    
    wsaOverlapped.hEvent = NULL;
    DoWSATestCleanup( testSockets,
                      numSockets );

    if (testFailed == TRUE)
    {
        Fail("");
    }

    PAL_Terminate();
    return PASS;
}
        

