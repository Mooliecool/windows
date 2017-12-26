/*=============================================================================
**
**
** Source: wsarecv9_client.c (WSASend, WSARecv)
**
** Purpose: This test sends and receives a large amount of buffer. At 400th 
**          WSASend, a shutdown is done on the client. Then the server should
**          immediately stop receiving data and close normally. A check is done
**          to verify that information has been sent correctly. Operations are 
**          done in TCP, Blocking and Overlapped.
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
**               CreateEvent
**               WaitForSingleObject
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

    int byteCounter;

    /* Sockets descriptor */
    const int numSockets = 1;    /* number of sockets used in this test */

    SOCKET testSockets[1];    

    
    /* Variables for WSASend */

    WSABUF wsaSendBuf;
    DWORD  dwNbrOfByteSent;
    DWORD  dwNbrOfBuf  = 1;
    DWORD  dwSendFlags = 0;
    
    unsigned char   sendBuffer[255];
    

    WSAOVERLAPPED wsaOverlapped;
    

    /* Socket DLL version */
    const WORD wVersionRequested = MAKEWORD(2,2);

    HANDLE  writeEvent;
    DWORD   waitResult;

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

    /* fill the sent buffer with value */
    for (i=0;i<255;i++)
    {
        sendBuffer[i]= i;
    }           

    /* create events */
    writeEvent = CreateEvent( NULL, /* no security */
                             FALSE, /* reset type */
                             FALSE, /* initial state */
                             NULL );  /* object name */
            
    if( writeEvent == NULL )
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
    
    
    /* Set the WSABUF structure */
    wsaSendBuf.len = 255;        
    wsaSendBuf.buf = sendBuffer;    
    
    byteCounter = 0;

    for(i=0;i<500;i++)
    {
        /* Initialize the WSAOVERLAPPED to 0 */
        memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED)); 

        wsaOverlapped.hEvent = writeEvent;

        ResetEvent(writeEvent);

        /* after 399 successfull send, shutdown the socket */
        if(i==400)
        {
            /* Disconnect the socket */
            err = shutdown( testSockets[0], 
                            SD_BOTH);
            if (err == SOCKET_ERROR)
            {
                Trace("ERROR: Unexpected failure: "
                      "shutdown() socket with local server "
                       "returned %d\n",
                       GetLastError());            

                /* Do some cleanup */
                DoWSATestCleanup( testSockets,
                                  numSockets );

                CloseHandle(writeEvent);

                Fail("");
            }
            break;
        }

        /* Send some data */
        err = WSASend( testSockets[0],
                       &wsaSendBuf,
                       dwNbrOfBuf,
                       &dwNbrOfByteSent,
                       dwSendFlags,
                       &wsaOverlapped,
                       0 );

        
        if(err == SOCKET_ERROR )
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

                CloseHandle(writeEvent);

                Fail("");
            }

            /* Wait 10 seconds for WriteEvent to be signaled 
               for pending operation
            */
            waitResult = WaitForSingleObject( writeEvent, 
                                              10000 );    
 
            if (waitResult!=WAIT_OBJECT_0)
            {   
                Trace("ERROR: Unexpected failure: "
                      "WaitForSingleObject has timed out \n");
        
                /* Do some cleanup */
                DoWSATestCleanup( testSockets,
                                  numSockets );

                CloseHandle(writeEvent);

                Fail("");
            }  

        }        
        
        

        /* keep track of the number of bytes sent */
        byteCounter += wsaOverlapped.InternalHigh;
 
        /* Verify that the number of bytes sent are the number of byte
           specified in the wsaBuf structure 
        */
        if(wsaSendBuf.len!=wsaOverlapped.InternalHigh)
        {            
            Trace("WSASend has not send the number of byte requested "
                  "wsaSendBuf.len = %d while "
                  "wsaOverlapped.InternalHigh = % d",
                  wsaSendBuf.len, wsaOverlapped.InternalHigh);

            CloseHandle(writeEvent);

            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                              numSockets );
            Fail("");
        }        
    }

         
    /* close the event */
    if( CloseHandle(writeEvent) == 0 )
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


    /* Do some cleanup */
    DoWSATestCleanup( testSockets,
                      numSockets );
 

    /* Expected number of bytes sent is 102000 */
    if(byteCounter!=102000)
    {
        Fail("Invalid number of byte sent to the server");
    }    

    PAL_Terminate();
    return PASS;
}
