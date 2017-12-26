/*=============================================================================
**
** Source: wsasendrecv1_client.c (WSASend, WSARecv)
**
** Purpose: Simple client application that sends and receives data to/from
**          the server. To be successful, the string sent has to be
**          the same as the string received.
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Trace
**               Fail
**               WSAStartup
**               WSACleanup
**               WSASocketA
**               connect
**               memset
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
 * Global variables
 */


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

    /* Sockets descriptor */
    const int numSockets = 1;    /* number of sockets used in this test */

    SOCKET testSockets[1];

    /* Event */
    DWORD  waitResult;
    HANDLE readEvent;

    /* Variables for WSASend */

    WSABUF wsaSendBuf[1];
    DWORD  dwNbrOfByteSent;
    DWORD  dwNbrOfBuf  = 1;
    DWORD  dwSendFlags = 0;
    char   sendBuffer[255] = "violets are blue, roses are red";

    WSAOVERLAPPED wsaOverlapped;

    /* Variables for WSARecv */
    WSABUF        wsaRecvBuf[1];
    DWORD         dwRecvFlags = 0;
    WSAOVERLAPPED wsaRecvOverlapped;
    char          recvBuffer[255] = "";   
    

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
        DoWSATestCleanup( 0, 0);

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
        DoWSATestCleanup( 0, 0);

        Fail("");
    }
    
    /* prepare the sockaddr_in structure */
    mySockaddr.sin_family           = AF_INET;
    mySockaddr.sin_port             = getRotorTestPort();
    mySockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    memset( &(mySockaddr.sin_zero), 0, 8);

    /* connect to a server */
    err = connect( testSockets[0], 
                   (struct sockaddr *)&mySockaddr,
                   sizeof(struct sockaddr));

    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "connect() socket with local server "
              "returned %d\n",
              GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    /* create an event */
    readEvent = CreateEvent( NULL, /* no security */
                             FALSE, /* reset type */
                             FALSE, /* initial state */
                             NULL );  /* object name */
                
    if( readEvent == NULL )
    {
        Trace("ERROR: Unexpected failure: "
              "CreateEvent() "
              "returned %d\n",
              GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED));


    /* Set the WSABUF structure */
    wsaSendBuf[0].len = 255;
    wsaSendBuf[0].buf = sendBuffer;

    /* Set the WSAOVERLAPPED to zero */
    memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED));
    wsaOverlapped.hEvent = readEvent;
    
    /* Send some data */
    err = WSASend( testSockets[0],
                   wsaSendBuf,
                   dwNbrOfBuf,
                   &dwNbrOfByteSent,
                   dwSendFlags,
                   &wsaOverlapped,
                   0 );

    if(err != SOCKET_ERROR )
    {
        /* set back the event to null */
        ResetEvent(readEvent);
    }
    else
    {
        if(GetLastError()!=WSA_IO_PENDING)
        {
            Trace("ERROR: Unexpected failure: "
                "WSASend() "
                "returned %d\n",
                GetLastError());

            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                            numSockets );

            Fail("");
        }

        /* Wait 10 seconds for ReadEvent to be signaled */
        waitResult = WaitForSingleObject( readEvent,  /* handle to object */
                                          10000 );/* 10 sec time-out interval*/

        if(waitResult!=WAIT_OBJECT_0)
        {
            Trace("ERROR: WaitForSingleObject() "
              "returned %d "
              "when it should have returned WAIT_OBJECT_0.\n"
              " wsaOverlapped.InternalHigh=%d.\n",
              waitResult,wsaOverlapped.InternalHigh);

            /* close the event */
            if( CloseHandle(readEvent) == 0 )
            {
                Trace("ERROR: Unexpected failure: "
                    "CloseHandle() "
                    "returned %d\n",
                    GetLastError());
            }

            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                              numSockets );

            Fail("");
        }
    }   

    /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaRecvOverlapped, 0, sizeof(WSAOVERLAPPED));

    /* Initialize the recvBuffer to empty 0 */
    memset(recvBuffer,0, 255);

    /* Specify which event to signal when data is arrived*/
    wsaRecvOverlapped.hEvent = readEvent;

    /* Set the WSABUF structure */
    wsaRecvBuf[0].len = 255;
    wsaRecvBuf[0].buf = recvBuffer;    

    /* Prepare to receive data */
    err = WSARecv( testSockets[0],
                   wsaRecvBuf,
                   dwNbrOfBuf,
                   &dwNbrOfByteSent,
                   &dwRecvFlags,
                   &wsaRecvOverlapped,
                   0 ); 

    if( err != SOCKET_ERROR)      
    {
        if(dwNbrOfByteSent!=255)
        {
            Trace("ERROR: "
                "WSARecv(...) "
                "returned %d, and number of byte send is not 255.\n",
                err ); 

            /* close the event */
            if( CloseHandle(readEvent) == 0 )
            {
                Trace("ERROR: Unexpected failure: "
                    "CloseHandle() "
                    "returned %d\n",
                    GetLastError());
            }

                
            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                            numSockets );
            
            Fail("");
        }

        /* set the event to non-signaled state */
        ResetEvent(readEvent);
    }      
    else if( GetLastError()!= WSA_IO_PENDING )
    {
        Trace("ERROR, WSARecv returned %d "
             "when it should have returned WSA_IO_PENDING\n", 
             GetLastError());

        /* close the event */
        if( CloseHandle(readEvent) == 0 )
        {
            Trace("ERROR: Unexpected failure: "
                  "CloseHandle() "
                  "returned %d\n",
                  GetLastError());
        }

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );
        
        Fail("");
    }
    else  /*WSA_IO_PENDING */
    {
        /* Wait 10 seconds for ReadEvent to be signaled */
        waitResult = WaitForSingleObject( readEvent,  /* handle to object */
                                        10000 );    /* 10 sec time-out interval */

        if (waitResult!=WAIT_OBJECT_0)
        {   
            Trace("ERROR: Unexpected failure: "
                "WaitForSingleObject has timed out \n");

            /* close the event */
            if( CloseHandle(readEvent) == 0 )
            {
                Trace("ERROR: Unexpected failure: "
                    "CloseHandle() "
                    "returned %d\n",
                    GetLastError());
            }

            
            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                            numSockets );

            Fail("");
        }
    }

    /* close the event */
    if( CloseHandle(readEvent) == 0 )
    {
        Trace("ERROR: Unexpected failure: "
              "CloseHandle() "
              "returned %d\n",
              GetLastError());
        
        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }   

    
    /* Verify that the received buffer is the same as the one we sent */
    if( strcmp( recvBuffer, sendBuffer ) != 0 )
    {
        Trace("Error: The client received %s "
              "when it should have received %s \n",
              recvBuffer,
              sendBuffer);

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }


    /* Disconnect the socket */
    err = shutdown( testSockets[0], 
                    SD_BOTH);

    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "shutdown() socket "
              "returned %d\n",
              GetLastError());

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
}
