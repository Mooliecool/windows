/*=============================================================================
**
** Source: wsarecvfrom9.c (WSASend, WSARecvFrom)
**
** Purpose: This test tries the receive operation with a buffer smaller than
**          the one specified at by the send operation. Operations are done with 
**          an with a TCP socket,Blocking and ovelapped. A buffer of 255 is sent
**          and the receive operation should be divided in 2 parts, one 150 bytes
**          and a second 105 bytes.
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


const int       listenBacklog   = 1;    /* backlog for listen */
const int       THREAD_FAIL     = 0; /* server port number */
const int       THREAD_SUCCESS  = 1;    /* backlog for listen */
const int       THREAD_UNDEFINED = 2;    /* backlog for listen */

/* Thread that is used as a client */
void PALAPI Thread_Client(LPVOID lpParam);

/* utility cleanup function */
static int CloseEventHandle(HANDLE Event);
static int CloseThreadHandle(HANDLE thread);
static int WaitForClientThreadToFinish(HANDLE thread);
 
/* We use a global variable to keep track of the 
   client thread return value because GetExitCodeThread
   is not supported on PAL
*/
int threadExitCode;

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
    HANDLE  hReadEvent;
    DWORD   waitResult;

    /* Thread variable */
    HANDLE hThreadClient;
    DWORD dwThreadClient;     
    DWORD dwClientParam[2]; 

    HANDLE hThreadEvent; 

    /* Sockets descriptor */
    const int numSockets = 2;    /* number of sockets used in this test */

    SOCKET testSockets[2];

     /* Variables needed for setsockopt */
    BOOL bReuseAddr = TRUE;    

    /* Variables needed for select */
    struct timeval waitTime;
    fd_set readFds;
    int    socketFds;

    /* Variables needed for WSARecv */
    WSABUF        wsaBuf;
    DWORD         dwNbrOfBuf  = 1;
    DWORD         dwNbrOfByteSent;
    DWORD         dwRecvFlags = 0;
    WSAOVERLAPPED wsaRecvOverlapped;
    
    /* Variable used to store transmitted data */
    unsigned char myBuffer[300];    
    
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
        Fail( "Server error: Unexpected failure: "
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
        Trace("Server error: Unexpected failure "
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
        Trace("Server error: Unexpected failure: "
              "WSASocketA"
              "(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED)) "
              " returned %d\n",
              GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( 0, 0);

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
        Trace("Server error: Unexpected failure: "
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
    
    /* Create a Event with initial owner. */
    hThreadEvent = CreateEvent( NULL, /* no security   */
                             FALSE,   /* reset type    */
                             FALSE,   /* initial state */
                             "EventClientServer" );  /* object name   */

    if (hThreadEvent == NULL) 
    {        
        /* Check for error. */
        Trace( "Server Error: Unexpected failure: "
              "CreateEvent() "
              "returned NULL\n");

          /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }    
    
    /* create a client thread */

    hThreadClient = 
        CreateThread( 
                NULL,                        /* no security attributes */
                0,                           /* use default stack size */
                (LPTHREAD_START_ROUTINE)Thread_Client,/* thread function    */
                (LPVOID)&dwClientParam,      /* argument to thread function */
                0,                           /* use default creation flags  */
                &dwThreadClient);            /* returns the thread identifier*/

    if(hThreadClient==NULL)
    {        
        Trace( "Server Error: Unexpected failure: "
              "CreateThread() "
              "returned NULL\n");

        CloseEventHandle(hThreadEvent);        

          /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    
    if(SetEvent(hThreadEvent)==0)
    {
        Trace("Server error: Unexpected failure: "
            "SetEvent has not set hThreadEvent as expected"
            "GetLastError returned = %d.\n",GetLastError());

        WaitForClientThreadToFinish(hThreadClient);

        CloseThreadHandle(hThreadClient);

        CloseEventHandle(hThreadEvent);        

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                        numSockets );

        Fail("");
    }

        /* set the server waiting time as 10 seconds */
    waitTime.tv_sec = 10L;
    waitTime.tv_usec = 0L;

    /* initialize the except socket set  */
    FD_ZERO( &readFds );

    /* add socket to readable socket set */
    FD_SET( testSockets[0], 
            &readFds );


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

        WaitForClientThreadToFinish(hThreadClient);

        CloseThreadHandle(hThreadClient);

        CloseEventHandle(hThreadEvent);        

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                        numSockets );

        Fail("");
    }

    if( socketFds == 0 )
    {
        Trace("ERROR: Unexpected select "
              "timed out\n");

        WaitForClientThreadToFinish(hThreadClient);

        CloseThreadHandle(hThreadClient);

        CloseEventHandle(hThreadEvent);        

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                        numSockets );

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

        WaitForClientThreadToFinish(hThreadClient);

        CloseEventHandle(hThreadEvent);

        CloseThreadHandle(hThreadClient);        

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
        
    }



    /* Initialize the WSABUF structure */
    memset(myBuffer, 0, 300);
    wsaBuf.buf = myBuffer;
    wsaBuf.len = 150;
    
    
    for(i=0;i<2;i++)
    {
        /* Initialize the WSAOVERLAPPED to 0 */
        memset(&wsaRecvOverlapped, 0, sizeof(WSAOVERLAPPED));

        /* Specify which event to signal when data is arrived*/
        wsaRecvOverlapped.hEvent = hReadEvent;

        /* Prepare to receive data */
        err = WSARecvFrom( testSockets[1],
                    &wsaBuf,
                    dwNbrOfBuf,
                    &dwNbrOfByteSent,
                    &dwRecvFlags,                    
                    (struct sockaddr FAR *)NULL,
                    (int)NULL,
                    &wsaRecvOverlapped,
                    0 );

        if( err != SOCKET_ERROR )
        {            
            /* If no error occur on WSARecvFrom, it is expected that
               dwNbrOfByteSent hold 150 bytes(first iteration) or
               105(second iteration).
            */
            if(dwNbrOfByteSent!=150&&dwNbrOfByteSent!=105)
            {   
                Trace("Server error: WSARecvFrom() "
                    "returned %d, expected SOCKET_ERROR.\n",
                    err);
                
                WaitForClientThreadToFinish(hThreadClient);

                CloseEventHandle(hThreadEvent);

                CloseThreadHandle(hThreadClient);

                CloseEventHandle(hReadEvent);                

                /* Do some cleanup */
                DoWSATestCleanup( testSockets,
                                numSockets );

                Fail("");
            }
        }
        else if(GetLastError()!=WSA_IO_PENDING)
        {
            Trace("Server error: WSARecv() "
                    "returned %d, expected WSA_IO_PENDING\n",
                    err );
                
            WaitForClientThreadToFinish(hThreadClient);

            CloseEventHandle(hThreadEvent);
            
            CloseThreadHandle(hThreadClient);

            CloseEventHandle(hReadEvent);
            
            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                            numSockets );
    
            Fail("");
        }
        else
        {
            waitResult = WaitForSingleObject( hReadEvent,
                                            10000 );           
            
            if (waitResult!=WAIT_OBJECT_0)
            {                   
                Trace("Server error: Unexpected failure: "
                    "WaitForSingleObject has timed out \n");

                WaitForClientThreadToFinish(hThreadClient);

                CloseEventHandle(hThreadEvent);

                CloseThreadHandle(hThreadClient);

                CloseEventHandle(hReadEvent);                
        
                /* Do some cleanup */
                DoWSATestCleanup( testSockets,
                                numSockets );

                Fail("");
            }        
        }    

        /* if it is the first receive, we would expect MSG_PARTIAL
           because the buffer sent is 255 long while the receive
           buffer is 150.                      
        */
        if(i==0)
        {
             /* 
               MSDN: The lpFlags parameter is both an input and an output 
               parameter, allowing applications to sense the output state 
               of the MSG_PARTIAL flag bit. However, the MSG_PARTIAL flag bit 
               is not supported by all protocols. 

               According to win32 result, MSG_PARTIAL is not set with TCP.                              

               this test is no longer valid:
               if(wsaRecvOverlapped.Internal==MSG_PARTIAL))

               Here we test if the first WSARecv has filled the 150 bytes 
               buffer completely

            */

            if(wsaRecvOverlapped.InternalHigh!=150)
            {
                Trace("Server error: "
                    "First WSARecv(...) "
                    "returned wsaRecvOverlapped.InternalHigh = %d" 
                    ", expected maximum buffer size of 150."
                    "\n", wsaRecvOverlapped.InternalHigh);

                WaitForClientThreadToFinish(hThreadClient);

                CloseEventHandle(hThreadEvent);

                CloseThreadHandle(hThreadClient);

                CloseEventHandle(hReadEvent);

                 /* Do some cleanup */
                DoWSATestCleanup( testSockets,
                                numSockets );

                Fail("");
           }
            /*set the recv buffer to the second half of myBuffer */
            wsaBuf.buf = myBuffer+150;
            wsaBuf.len = 150;
        }
        else /* i == 1 */
        {
           /* on the second wsarecv, it should have read the remaining data to 
              be read on the overlapped result.
           */            
            if(wsaRecvOverlapped.InternalHigh!=105)
            {
                Trace("Server error: "
                    "Second WSARecv(...) "
                    "returned wsaRecvOverlapped.InternalHigh = %d" 
                    ", expected maximum buffer size of 105."
                    "\n", wsaRecvOverlapped.InternalHigh);

                WaitForClientThreadToFinish(hThreadClient);

                CloseEventHandle(hThreadEvent);

                CloseThreadHandle(hThreadClient);

                CloseEventHandle(hReadEvent);

                 /* Do some cleanup */
                DoWSATestCleanup( testSockets,
                                numSockets );

                Fail("");
           }           
        }       
    } /* end of the for loop */

    if(!WaitForClientThreadToFinish(hThreadClient))
    {
        CloseEventHandle(hThreadEvent);

        CloseThreadHandle(hThreadClient);

        CloseEventHandle(hReadEvent);

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    if(!CloseEventHandle(hThreadEvent)||
       !CloseThreadHandle(hThreadClient)||
       !CloseEventHandle(hReadEvent))
    {
        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );
        Fail("");
    }

    /* test the data received if it valid */
    for(i=0;i<255;i++)
    {
        if(myBuffer[i]!=i)
        {
           Trace("Server error: "
                "WSARecv(...) "
                "returned wsaRecvOverlapped.InternalHigh = %d" 
                ", expected maximum buffer size of 105."
                "\n", wsaRecvOverlapped.InternalHigh);

                /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                            numSockets );

            Fail("");           
         }
    }
 
    /* Do some cleanup */
    DoWSATestCleanup( testSockets,
                      numSockets );


    PAL_Terminate();
    return PASS;
}

/* DWORD  PALAPI Thread_Client(LPVOID lpParam)  
   This is a client thread started by the main process.
   It simulate a client connecting to a remote server.   
*/
void  PALAPI Thread_Client(LPVOID lpParam)
{     
    int     i;
    int     err;
    struct  sockaddr_in mySockaddr;
    WSADATA wsaData;        

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

    HANDLE  hWriteEvent;
    DWORD   waitResult;
    
    /* Event handle */
    HANDLE hThreadEvent;

    threadExitCode=THREAD_UNDEFINED;

    /* Create a Event with no initial owner. EventClientServer is
       the Event owned by the server.
    */
    hThreadEvent = CreateEvent( NULL, /* no security   */
                             FALSE,   /* reset type    */
                             FALSE,   /* initial state */
                             "EventClientServer" );  /* object name   */

    /* Check for error. */
    if (hThreadEvent == NULL) 
    {        
        Trace( "Client Error: Unexpected failure: "
              "CreateEvent() "
              "returned NULL\n");

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }    
    
    /* Wait 10 seconds for EventClientServer to be signaled 
       from the server. It will mean that the server is ready
       to receive data or connection.
    */
    waitResult = WaitForSingleObject( hThreadEvent,
                                      10000 );
            
    if (waitResult!=WAIT_OBJECT_0)
    {
        Trace("Client error: Unexpected failure: "
              "WaitForSingleObject has timed out while "
              "waiting for EventClientServer.\n");

        CloseEventHandle(hThreadEvent);        
    
        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }

    /* close the thread handle, don't need anymore */
    if(!CloseEventHandle(hThreadEvent))
    {
        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }
    
    /* Sockets initialization to INVALID_SOCKET */
    for( i = 0; i < numSockets; i++ )
    {
        testSockets[i] = INVALID_SOCKET;
    }

    /* Initialize to use winsock2.dll */
    err = WSAStartup( wVersionRequested,
                      &wsaData);

    if(err != 0)
    {
        Trace( "Client error: Unexpected failure: "
              "WSAStartup(%i) "
              "returned %d\n",
              wVersionRequested, 
              GetLastError() );

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }

    /* Confirm that the WinSock DLL supports 2.2.
       Note that if the DLL supports versions greater    
       than 2.2 in addition to 2.2, it will still return
       2.2 in wVersion since that is the version we      
       requested.                                        
    */
    if ( wsaData.wVersion != wVersionRequested ) 
    {
        Trace("Client error: Unexpected failure "
              "to find a usable version of WinSock DLL\n");

        /* Do some cleanup */
        DoWSATestCleanup( 0, 0);

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
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
        Trace("Client error: Unexpected failure: "
              "WSASocketA"
              "(AF_INET,SOCK_DGRAM,IPPROTO_UDP,NULL,0,WSA_FLAG_OVERLAPPED) "
              " returned %d\n",
              GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( 0, 0);

        threadExitCode=THREAD_FAIL;
        ExitThread(0);
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
        Trace("Client error: Unexpected failure: "
              "connect() socket with local server "
              "returned %d\n",
              GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }


    /* fill the sent buffer with value */
    for (i=0;i<255;i++)
    {
        sendBuffer[i]= i;
    }

   
    /* create events */
    hWriteEvent = CreateEvent( NULL, /* no security   */
                             FALSE,    /* reset type    */
                             FALSE,    /* initial state */
                             NULL );   /* object name   */
            
    if( hWriteEvent == NULL )
    {
        Trace("Client error: Unexpected failure: "
              "CreateEvent() "
              "returned %d\n",
              GetLastError());
           
        CloseEventHandle(hWriteEvent);

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }

     /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED));  
    wsaOverlapped.hEvent = hWriteEvent;
    
    /* Set the WSABUF structure */
    wsaSendBuf.len = 255;        
    wsaSendBuf.buf = sendBuffer;

   
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
            Trace("Client error: Unexpected failure: "
                  "WSASend() "
                  "returned %d\n",
                  GetLastError());
                 
            CloseEventHandle(hWriteEvent);

            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                              numSockets );

            threadExitCode=THREAD_FAIL;

            ExitThread(0);
        }

        /* Wait 10 seconds for hWriteEvent to be signaled 
           for pending operation
        */
        waitResult = WaitForSingleObject( hWriteEvent, 
                                          10000 );            

        if (waitResult!=WAIT_OBJECT_0)
        {  
            Trace("Client Error: Unexpected failure: "
                  "WaitForSingleObject has timed out \n");

             CloseEventHandle(hWriteEvent);
    
            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                              numSockets );        

            threadExitCode=THREAD_FAIL;
            ExitThread(0);
        }        
    }

    /* close the handle to hWriteEvent */
    if(!CloseEventHandle(hWriteEvent))
    {
        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                        numSockets );

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }

    /* Verify that the number of bytes sent are the number of byte
       specified in the wsaBuf structure 
    */
    if(wsaSendBuf.len!=wsaOverlapped.InternalHigh)
    {   
        Trace("Client WSASend has not send the number of byte requested "
              "wsaSendBuf.len = %d while "
              "wsaOverlapped.InternalHigh = % d",
              wsaSendBuf.len, wsaOverlapped.InternalHigh);        

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }

    /* Do some cleanup */
    DoWSATestCleanup( testSockets,
                      numSockets );

    threadExitCode=THREAD_SUCCESS;

    ExitThread(0);    
} 

/* int CloseThreadHandle(HANDLE thread)
   This function close a thread handle 
   and return an appropriate message in case of error
*/
int CloseThreadHandle(HANDLE thread)
{   
    if(thread!=NULL)
    {
        if( CloseHandle(thread) == 0 )
        {
            Trace("Server error: Unexpected failure: "
                  "CloseHandle(thread) "
                  "returned %d\n",
                GetLastError());
            return 0;
        }
    }
    return 1;
}

/* int CloseEventHandle(HANDLE Event)
   This function close an Event handle 
   and return an appropriate message in case of error
*/
static int CloseEventHandle(HANDLE Event)
{
    if( CloseHandle(Event) == 0 )
    {
        Trace("Server error: Unexpected failure: "
              "CloseHandle() "
              "returned %d\n",
            GetLastError());
        return 0;
    }    
    return 1;
}

/* int WaitForThreadToFinish(HANDLE thread)
   This function return 0 if the client thread exit code 
   is clientHasFailed and 1 other is the execution was successfull
*/
int WaitForClientThreadToFinish(HANDLE thread)
{
    DWORD   waitResult;   
    
    if(thread==NULL)
    {
        return 0;
    }

    /* Wait for the thread to finish execution */
    waitResult = WaitForSingleObject( thread, 10000 );
            
    if (waitResult!=WAIT_OBJECT_0)
    {   
        Trace("Server error: Unexpected failure: "
              "WaitForSingleObject has timed out waiting for" 
              " the client thread to finish.\n");
        return 0;
    }

    if(threadExitCode==THREAD_SUCCESS)
    {
        return 1;
    }    
    else if(threadExitCode==THREAD_FAIL)
    {
        /* return error, error msg has been handled by the client */
        return 0;
    }
    else
    {
        Trace("Client error: Unexpected failure: "
              "No known error code defined on the client thread.");
        return 0;
    }
}
