/*=============================================================================
**
** Source: test2.c (WSAIoctl)
**
** Purpose: Tests WSAIoctl by setting a socket to non-blocking. wsarecvfrom is
**          is then called to test if are in non-blocking mode.
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
    struct  sockaddr_in mySockaddr;

    /* Sockets descriptor */
    const int numSockets = 1;    /* number of sockets used in this test */
    SOCKET    testSockets[1];    /* test's socket list */
    
    /* Variables needed by WSAIoctl */

    WSAOVERLAPPED   wsaIoctlOverlapped;
    DWORD           dwIoctlBytesReturned;
    u_long          unInBuffer;
    DWORD           dwInBufferLen = sizeof(unInBuffer);
    char            *pOutBuffer[255];
    DWORD           dwOutBufferLen = 255;

    /* recvfrom operation varaibles*/
    HANDLE hReadEvent;    
    char myBuffer[255];    
    DWORD         dwRecvFlags = 0;
    WSAOVERLAPPED wsaRecvOverlapped;
    int sizeOverlapped;    

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
                                  SOCK_DGRAM, 
                                  IPPROTO_UDP, 
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

    /* attempt to set some bogus socket to non-blocking */
    unInBuffer = 1;

    err = WSAIoctl( testSockets[0], 
                    FIONBIO,
                    &unInBuffer,
                    dwInBufferLen,
                    pOutBuffer,
                    dwOutBufferLen,
                    &dwIoctlBytesReturned,
                    &wsaIoctlOverlapped,
                    0 );

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

    /* prepare the sockaddr_in structure */
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



    /* create an event */
    hReadEvent = CreateEvent( NULL, /* no security   */
                             FALSE,   /* reset type    */
                             FALSE,   /* initial state */
                             NULL );  /* object name   */
            
    if( hReadEvent == NULL )
    {            
        Trace("Server error: Unexpected failure: "
              "CreateEvent() "
              "returned %d\n",
              GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
        
    }

    /* Initialize the WSABUF structure */
    memset(myBuffer, 0, 255);    

    /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaRecvOverlapped, 0, sizeof(WSAOVERLAPPED));

    /* Specify which event to signal when data is arrived*/
    wsaRecvOverlapped.hEvent = hReadEvent;

    sizeOverlapped = (int) sizeof(mySockaddr);

    /* Prepare to receive data */
    err = recvfrom( testSockets[0],
                myBuffer,
                255,
                dwRecvFlags,
                (struct sockaddr*)&mySockaddr,
                &sizeOverlapped);

    if( err != SOCKET_ERROR )
    {
        Trace("Server error: WSARecv() "
                    "returned  %d, SOCKET_ERROR\n",
                    err ); 

        if (CloseHandle(hReadEvent)==0)
        {
            Trace("Server error: CloseHandle Failed ");                    
        }

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                        numSockets );

        Fail("");
    
    }
    else
    {
        err = GetLastError();
        /* Only WSA_IO_PENDING is expected */
        if(err==WSA_IO_PENDING)
        {
            /* Wait 10 seconds for ReadEvent to be signaled 
            from the pending operation
            */
            err = WaitForSingleObject( hReadEvent, 1000 );    
            
            if (err==WAIT_FAILED)
            {           
                Trace("Server error: Unexpected failure: "
                    "WaitForSingleObject failed \n");

                if (CloseHandle(hReadEvent)==0)
                {
                    Trace("Server error: CloseHandle Failed ");                    
                }

                Fail("");
            }
            err = wsaRecvOverlapped.Internal;
        }


        /* Only WSAEWOULDBLOCK is expected */
        if(err!=WSAEWOULDBLOCK)
        {
            Trace("Server error: WSARecvFrom() "
                    "returned %d, expected WSAEWOULDBLOCK\n",
                    err );

            if (CloseHandle(hReadEvent)==0)
            {
                Trace("Server error: CloseHandle Failed ");                    
            }
            
            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                            numSockets );
    
            Fail("");
        }     
    }       
    

    /* Do some cleanup */
    DoWSATestCleanup( testSockets,
                      numSockets );

    if (CloseHandle(hReadEvent)==0)
    {
        Trace("Server error: CloseHandle Failed "); 
        Fail("");
    }
    
    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}
