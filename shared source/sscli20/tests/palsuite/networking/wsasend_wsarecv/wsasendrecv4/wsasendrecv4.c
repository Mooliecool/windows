/*=============================================================================
**
** Source: wsasendrecv4.c (WSASend, WSARecv)
**
** Purpose: This test simulates multiple access to a server using multiple 
**          client threads. The main function thread serves for accepting 
**          connection from client threads. For every client that connects, the 
**          main thread creates a server thread; it will handle data exchange
**          with that client. Client threads are created on the fly for every
**          iteration of the main for loop. Every client will wait
**          few seconds to get a chance to connect to the server. If a failure
**          occurs with a client, the test continues and an error will be 
**          triggered by a call to Fail() at the end of the main thread. This 
**          procedure insures that all allocated handles and ressources are 
**          unallocated. The amount of data transfered are known by the server
**          and the client. As soon as they have the expected number of bytes
**          received, they will stop receiving data and start replying on the 
**          the same socket.
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

#define NUM_BYTE_TO_SEND        35000
#define NUM_CONNECTION          50
#define NUM_DATA_EXCHANGE       200

/**
 * Global variables
 */

const int       listenBacklog   = 1;    /* backlog for listen */
const int       THREAD_FAIL     = 0; /* server port number */
const int       THREAD_SUCCESS  = 1;    /* backlog for listen */
const int       THREAD_UNDEFINED = 2;    /* backlog for listen */
const int       THREAD_CLOSED    = 3;

/* Thread that is used as a client */
void PALAPI Thread_Client(LPVOID lpParam);
void PALAPI Thread_Server(LPVOID lpParam);

/* utility cleanup function */
static int CloseEventHandle(HANDLE Event);
static int CloseThreadHandle(HANDLE thread);
static int WaitForThreadToFinish(HANDLE thread, 
                                 int id, 
                                 int *pReturnErrorArray);
static int CloseSocket(SOCKET sockList[], int numSockets);
 
/* We use a global variable to keep track of the 
   client thread return value because GetExitCodeThread
   is not supported on PAL
*/
int threadClientExitCode[NUM_CONNECTION];
int threadServerExitCode[NUM_CONNECTION];

/* Sockets descriptor */
/* number of sockets used in this test */
const int server_numSockets = NUM_CONNECTION;
SOCKET server_testSockets[NUM_CONNECTION];
char      dataToSend[NUM_BYTE_TO_SEND];

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

    int testHasFailed=0;

    /* Thread client variable */
    HANDLE hThreadClient[NUM_CONNECTION];
    DWORD dwThreadClient[NUM_CONNECTION];     
    DWORD dwClientParam; 

    /* Server client variable */
    HANDLE hThreadServer[NUM_CONNECTION];
    DWORD dwThreadServer[NUM_CONNECTION];     
    DWORD dwServerParam;     

    /* variable for iocltsocket */
    u_long argp;

     /* Variables needed for setsockopt */
    BOOL bReuseAddr = TRUE;    

    /* Variables needed for select */
    struct timeval waitTime;
    fd_set readFds;
    int    socketFds;    
    
    /* Socket DLL version */
    const WORD wVersionRequested = MAKEWORD(2,2);

    /* Sockets initialization to INVALID_SOCKET */
    for( i = 0; i < server_numSockets; i++ )
    {
        server_testSockets[i] = INVALID_SOCKET;
    }

    for( i = 0; i < NUM_CONNECTION; i++ )
    {
        threadClientExitCode[i]=THREAD_CLOSED;
        threadServerExitCode[i]=THREAD_CLOSED;
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
        
        DoWSATestCleanup( 0, 0);
        Fail("");
    }
    
    for(i=0;i<NUM_BYTE_TO_SEND;i++)
    {
        dataToSend[i]=i;
    }

    /* create an overlapped stream socket in AF_INET domain */

    server_testSockets[0] = WSASocketA( AF_INET, 
                                 SOCK_STREAM, 
                                 IPPROTO_TCP,
                                 NULL, 
                                 0, 
                                 WSA_FLAG_OVERLAPPED ); 


    if( server_testSockets[0] == INVALID_SOCKET )
    {   
        Trace("Server error: Unexpected failure: "
              "WSASocketA"
              "(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED)) "
              " returned %d\n",
              GetLastError());
        
        DoWSATestCleanup( 0, 0);
        Fail("");
    }

    /* Allows the socket to be bound to an address that is already in use. */
    err = setsockopt( server_testSockets[0],
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
        
        DoWSATestCleanup( server_testSockets, server_numSockets );
        Fail("");
    }

    /* enable non blocking socket */
    argp=1;
    err = ioctlsocket(server_testSockets[0], FIONBIO, (u_long FAR *)&argp);

    if (err==SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "ioctlsocket(.., FIONBIO, ..) "
              "returned %d\n",
              GetLastError() );
        
        DoWSATestCleanup( server_testSockets,server_numSockets );        
        Fail("");
    }

    /* prepare the sockaddr structure */
    mySockaddr.sin_family           = AF_INET;
    mySockaddr.sin_port             = getRotorTestPort();
    mySockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    memset( &(mySockaddr.sin_zero), 0, 8);

    /* bind local address to a socket */
    err = bind( server_testSockets[0],
                (struct sockaddr *)&mySockaddr,
                sizeof(struct sockaddr) );

    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "bind() socket with local address "
              "returned %d\n",
              GetLastError() );

        DoWSATestCleanup( server_testSockets,server_numSockets );
        Fail("");
    }

    for(i=1;i<NUM_CONNECTION;i++)
    {
        /* listen to the socket */
        err = listen( server_testSockets[0], listenBacklog );

        if( err == SOCKET_ERROR )
        {
            Trace("Main error(%d), Unexpected failure: "
                "listen() to sockets "
                "returned %d\n",i,
                GetLastError() );            

            testHasFailed=1;
            break;
        }

        
        /* set index for thread exit code array. */
        dwClientParam = (DWORD)i;
        
        /* Create a client thread */
        hThreadClient[i] = 
            CreateThread( 
                    NULL,                   /* no security attributes */
                    0,                      /* use default stack size */
                    (LPTHREAD_START_ROUTINE)Thread_Client,/* thread function*/
                    (LPVOID)dwClientParam,  /* argument to thread function  */
                    0,                      /* use default creation flags   */
                    &dwThreadClient[i]);    /* returns the thread identifier*/

        if(hThreadClient[i]==NULL)
        {        
            Trace( "Main error(%d), Unexpected failure: "
                "CreateThread() "
                "returned NULL\n",i);           
            
            testHasFailed=1;
            break;
        }        
   
        /* set the accept waiting time as 10 seconds */
        waitTime.tv_sec = 10L;
        waitTime.tv_usec = 0L;

        /* initialize the readable socket set  */
        FD_ZERO( &readFds );

        /* add socket to readable socket set */
        FD_SET( server_testSockets[0], &readFds );

        /* monitor the readable socket set 
        to determine when a connection is ready to be accepted
        */
        socketFds = select( 0,
                            &readFds,
                            NULL,
                            NULL,
                            &waitTime);
        

        if( socketFds == SOCKET_ERROR )
        {
            Trace("Main error(%d), Unexpected failure "
                "with select\n",i);

            testHasFailed=1;
            break;
        }

        if( socketFds == 0 )
        {
            Trace("Main error(%d), Unexpected select "
                "timed out\n",i);

            testHasFailed=1;
            break;
        }
 
        /* accept connection */
        server_testSockets[i] = accept( server_testSockets[0],
                                (struct sockaddr *)&mySockaddr,
                                &addrlen );

        if( server_testSockets[i] == INVALID_SOCKET )
        {
            Trace("Main error(%d), Unexpected failure: "
                "accept() connection on socket "
                "returned %d\n",i,
                GetLastError());

            testHasFailed=1;
            break;
        }

        /* enable non blocking socket */
        argp=1;
        err = ioctlsocket(server_testSockets[i], FIONBIO, (u_long FAR *)&argp);

        if (err==SOCKET_ERROR )
        {
            Trace("Main error(%d), Unexpected failure: "
                "ioctlsocket(.., FIONBIO, ..) "
                "returned %d\n",i,
                GetLastError() );

            testHasFailed=1;
            break;
        }

        /* set index for thread exit code and socket array */
        dwServerParam = (DWORD)i;

        /* create a server thread to handle client data exchange */
        hThreadServer[i] = 
            CreateThread( 
                    NULL,                   /* no security attributes */
                    0,                      /* use default stack size */
                    (LPTHREAD_START_ROUTINE)Thread_Server,/* thread function */
                    (LPVOID)dwServerParam,  /* argument to thread function */
                    0,                      /* use default creation flags  */
                    &dwThreadServer[i]);    /* returns the thread identifier*/

        if(hThreadServer[i]==NULL)
        {        
            Trace( "Main error(%d), Unexpected failure: "
                "CreateThread() "
                "returned NULL\n",i);

            testHasFailed=1;
            break;
        }
    }

    /* Wait for every connection to complete. */
    for(i=1;i<NUM_CONNECTION;i++)
    {        
        if(threadClientExitCode[i]==THREAD_UNDEFINED)
        {
            if(!
               WaitForThreadToFinish(hThreadClient[i],i, threadClientExitCode))
            {
                testHasFailed=1;
            }
        }

        if(hThreadClient[i]!=NULL)
        {
            if(!CloseThreadHandle(hThreadClient[i]))
            {
                testHasFailed=1;
            }
        }
        
        if(threadServerExitCode[i]==THREAD_UNDEFINED)
        {
            if(!
               WaitForThreadToFinish(hThreadServer[i],i, threadServerExitCode))
            {
                testHasFailed=1;
            }
        }
        
        if(hThreadServer[i]!=NULL)
        {   
            if(!CloseThreadHandle(hThreadServer[i]))
            {
                testHasFailed=1;
            }
        }
    }    

    DoWSATestCleanup( server_testSockets, server_numSockets );
    
    if(testHasFailed)
    {
        Fail("Test Failed.\n");
    }

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

    /* Sockets descriptor */
    const int numSockets = 1;    /* number of sockets used in this test */

    SOCKET testSockets[1];        
    
    /* Variables for WSASend */
    WSABUF wsaSendBuf;
    DWORD  dwNbrOfByteSent;
    DWORD  dwNbrOfBuf  = 1;
    DWORD  dwSendFlags = 0;
    WSAOVERLAPPED wsaOverlapped;

    /* Variables for WSARecv */
    WSABUF wsaRecvBuf;
    DWORD  dwRecvFlags = 0;
    WSAOVERLAPPED wsaRecvOverlapped;
    
    unsigned char   dataBuffer[NUM_BYTE_TO_SEND];        
    int bufferCount;    

    int clientId=(int)lpParam;

    /* variable for iocltsocket */
    u_long argp;

    /* Variables needed for select */
    struct timeval waitTime;
    fd_set writeFds;    
    int    socketFds;        

    HANDLE  hWriteEvent;
    DWORD   waitResult;

    threadClientExitCode[clientId]=THREAD_UNDEFINED;  
    
    /* Sockets initialization to INVALID_SOCKET */
    for( i = 0; i < numSockets; i++ )
    {
        testSockets[i] = INVALID_SOCKET;
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
        Trace("Client error(%d): Unexpected failure: "
              "WSASocketA"
              "(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED) "
              " returned %d\n",clientId,
              GetLastError());        

        threadClientExitCode[clientId]=THREAD_FAIL;
        ExitThread(0);
    }

    /* enable non blocking socket */
    argp=1;
    err = ioctlsocket(testSockets[0], FIONBIO, (u_long FAR *)&argp);

    if (err==SOCKET_ERROR )
    {
        Trace("Client error(%d): Unexpected failure: "
              "ioctlsocket(.., FIONBIO, ..) "
              "returned %d\n",clientId,
              GetLastError() );
        
        CloseSocket( testSockets, numSockets );       
        threadClientExitCode[clientId]=THREAD_FAIL;
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
        err = GetLastError();
        if ( err != WSAEWOULDBLOCK )
        {
            Trace("Client error(%d): Unexpected failure: "
              "connect() socket with local server "
              "returned %d, expected WSAEWOULDBLOCK\n",clientId,
              GetLastError());
           
            CloseSocket( testSockets, numSockets );
            threadClientExitCode[clientId]=THREAD_FAIL;
            ExitThread(0);
        }
    }

    /* set the connect waiting time as 10 seconds */
    waitTime.tv_sec = 10L;
    waitTime.tv_usec = 0L;

    /* initialize the writable socket set  */
    FD_ZERO( &writeFds );

    /* add socket to writable socket set */
    FD_SET( testSockets[0], &writeFds );

    /* monitor the writable socket set to determine the
       completion of the connection request.
    */
    socketFds = select( 0,                        
                        NULL,
                        &writeFds,
                        NULL,
                        &waitTime);

    if( socketFds == SOCKET_ERROR )
    {
        Trace("Client Error(%d): Unexpected failure "
              "with select\n",clientId);

        CloseSocket( testSockets, numSockets );
        threadClientExitCode[clientId]=THREAD_FAIL;
        ExitThread(0);
    }

    if( socketFds == 0 )
    {
        Trace("Client Error(%d): Unexpected select "
              "timed out\n",clientId);

        CloseSocket( testSockets, numSockets );
        threadClientExitCode[clientId]=THREAD_FAIL;
        ExitThread(0);
    }

    /* create events */
    hWriteEvent = CreateEvent( NULL, /* no security   */
                             FALSE,    /* reset type    */
                             FALSE,    /* initial state */
                             NULL );   /* object name   */
            
    if( hWriteEvent == NULL )
    {
        Trace("Client error(%d): Unexpected failure: "
              "CreateEvent() "
              "returned %d\n",clientId,
              GetLastError());
        
        CloseSocket( testSockets, numSockets );
        threadClientExitCode[clientId]=THREAD_FAIL;
        ExitThread(0);
    }

    /* This sleep allow the main thread to start more than one
       client / server at the same time.
    */
    Sleep(10000);

    /*  This loop recv data and send it back to server     */
    for(i=0;i<NUM_DATA_EXCHANGE;i++)
    {        
        /* Set the WSABUF structure */
        memset(dataBuffer, 0, NUM_BYTE_TO_SEND);        

        for(bufferCount=0;bufferCount<NUM_BYTE_TO_SEND;)
        {
            wsaRecvBuf.len = NUM_BYTE_TO_SEND-bufferCount;
            wsaRecvBuf.buf = dataBuffer+bufferCount;

            /* Initialize the WSAOVERLAPPED to 0 */
            memset(&wsaRecvOverlapped, 0, sizeof(WSAOVERLAPPED));

            /* Specify which event to signal when data is arrived*/
            wsaRecvOverlapped.hEvent = hWriteEvent;

            /* Prepare to receive data */
            err = WSARecv( testSockets[0],
                        &wsaRecvBuf,
                        dwNbrOfBuf,
                        &dwNbrOfByteSent,
                        &dwRecvFlags,
                        &wsaRecvOverlapped,
                        0 );

            if( err != SOCKET_ERROR )
            {            
                if(dwNbrOfByteSent==0)
                {
                    Trace("Client error(%d): Unexpected failure: "
                            "WSARecv() "
                            "returned %d\n",clientId,
                            GetLastError());
                            
                    CloseEventHandle(hWriteEvent);                    
                    CloseSocket( testSockets, numSockets );
                    threadClientExitCode[clientId]=THREAD_FAIL;
                    ExitThread(0);
                }
                 /* Reset the event */
                ResetEvent(hWriteEvent);
            }
            else if(GetLastError()!=WSA_IO_PENDING)
            {
                Trace("Client error(%d): WSARecv() "
                        "returned %d,%d, expected WSA_IO_PENDING\n",clientId,
                        err,GetLastError() );

                CloseEventHandle(hWriteEvent);
                CloseSocket( testSockets, numSockets );
                threadClientExitCode[clientId]=THREAD_FAIL;
                ExitThread(0);
            }
            else
            {    
                waitResult = WaitForSingleObject( hWriteEvent,
                                                30000 );
    
                if (waitResult!=WAIT_OBJECT_0)
                {                   
                    Trace("Client error(%d): Unexpected failure: "
                        "WaitForSingleObject has timed out \n",clientId);            
                    CloseEventHandle(hWriteEvent);
                    CloseSocket( testSockets, numSockets );
                    threadClientExitCode[clientId]=THREAD_FAIL;
                    ExitThread(0);
                }  

            }

            bufferCount+=wsaRecvOverlapped.InternalHigh;
        }
        
        /* compare data receveid with expected data */
        if(memcmp(dataBuffer,dataToSend,NUM_BYTE_TO_SEND)!=0)
        {
            Trace("Client error(%d), WSARecv(): "
                  "data received was not the expected sent data"
                  "...wsaRecvOverlapped.InternalHigh=%d, "
                  "wsaRecvOverlapped.Internal=%d\n",clientId
                  ,wsaRecvOverlapped.InternalHigh,wsaRecvOverlapped.Internal);

            CloseEventHandle(hWriteEvent);
            CloseSocket( testSockets, numSockets );
            threadClientExitCode[clientId]=THREAD_FAIL;
            ExitThread(0);
        }

        /* Prepare the receive operation : Initialize the WSAOVERLAPPED to 0 */
        memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED));  
        wsaOverlapped.hEvent = hWriteEvent;
        
        /* Set the WSABUF structure */
        wsaSendBuf.len = NUM_BYTE_TO_SEND;        
        wsaSendBuf.buf = dataBuffer;

        while (wsaSendBuf.len > 0)
        {
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
                  Trace("Client error(%d), Unexpected failure: "
                        "WSASend() "
                        "returned %d\n",clientId,
                        GetLastError());
                     
                  CloseEventHandle(hWriteEvent);
                  CloseSocket( testSockets, numSockets );
                  threadClientExitCode[clientId]=THREAD_FAIL;
                  ExitThread(0);
              }

              /* Wait 10 seconds for hWriteEvent to be signaled 
                 for pending operation
              */
              waitResult = WaitForSingleObject( hWriteEvent, 
                                                30000 );            

              if (waitResult!=WAIT_OBJECT_0)
              {  
                  Trace("Client Error(%d): Unexpected failure: "
                        "WaitForSingleObject has timed out \n",clientId);

                  CloseEventHandle(hWriteEvent);
                  CloseSocket( testSockets, numSockets );
                  threadClientExitCode[clientId]=THREAD_FAIL;
                  ExitThread(0);
              }

              dwNbrOfByteSent = wsaOverlapped.InternalHigh;
          }
          else
          {
              /* reset the event */
              ResetEvent(hWriteEvent);
          }

          wsaSendBuf.len -= dwNbrOfByteSent;
          wsaSendBuf.buf += dwNbrOfByteSent;
	}
    }

    /* close the handle to hWriteEvent */
    if(!CloseEventHandle(hWriteEvent))
    {        
        CloseSocket( testSockets, numSockets );
        threadClientExitCode[clientId]=THREAD_FAIL;
        ExitThread(0);
    }

    CloseSocket( testSockets, numSockets );

    threadClientExitCode[clientId]=THREAD_SUCCESS;
    ExitThread(0);    
} 

/* DWORD  PALAPI Thread_Server(LPVOID lpParam)
   This is a server thread started by the main process.
   It simulate a server handling a remote client.
*/
void  PALAPI Thread_Server(LPVOID lpParam)
{     
    int     i;
    int     err;    
    int     serverId;    
    
    /* Variables for WSASend */

    WSABUF wsaSendBuf;
    DWORD  dwNbrOfByteSent;
    DWORD  dwNbrOfBuf  = 1;
    DWORD  dwSendFlags = 0;
    WSAOVERLAPPED wsaOverlapped;

    /* Variables for WSARecv */
    WSABUF wsaRecvBuf;
    DWORD  dwRecvFlags = 0;
    WSAOVERLAPPED wsaRecvOverlapped;
    
    unsigned char   dataBuffer[NUM_BYTE_TO_SEND];

    int bufferCount;
    
    HANDLE  hWriteEvent;
    DWORD   waitResult;
    
    serverId = (int)lpParam;

    threadServerExitCode[serverId]=THREAD_UNDEFINED;

    /* create events */
    hWriteEvent = CreateEvent( NULL, /* no security   */
                             FALSE,    /* reset type    */
                             FALSE,    /* initial state */
                             NULL );   /* object name   */
            
    if( hWriteEvent == NULL )
    {
        Trace("Server error(%d): Unexpected failure: "
              "CreateEvent() "
              "returned %d\n",serverId,
              GetLastError());
        
        threadServerExitCode[serverId]=THREAD_FAIL;
        ExitThread(0);
    }

    for(i=0;i<NUM_DATA_EXCHANGE;i++)
    {
        /* Initialize the WSAOVERLAPPED to 0 */
        memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED));  
        wsaOverlapped.hEvent = hWriteEvent;
        
        /* Set the WSABUF structure */
        wsaSendBuf.len = NUM_BYTE_TO_SEND;        
        wsaSendBuf.buf = dataToSend;

        while (wsaSendBuf.len > 0)
        {
            /* Send some data */
            err = WSASend( server_testSockets[serverId],
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
                    Trace("Server error(%d), Unexpected failure: "
                          "WSASend() "
                          "returned %d\n",serverId,
                          GetLastError());
                     
                    CloseEventHandle(hWriteEvent);
                    threadServerExitCode[serverId]=THREAD_FAIL;
                    ExitThread(0);
                }

                /* Wait 30 seconds for hWriteEvent to be signaled 
                   for pending operation
                */
                waitResult = WaitForSingleObject( hWriteEvent, 30000 );            

                if (waitResult!=WAIT_OBJECT_0)
                {
                    Trace("Server Error(%d), Unexpected failure: "
                          "WaitForSingleObject has timed out \n",serverId);

                    CloseEventHandle(hWriteEvent);
                    threadServerExitCode[serverId]=THREAD_FAIL;
                    ExitThread(0);
                }        

                dwNbrOfByteSent = wsaOverlapped.InternalHigh;
            }
            else
            {
                /* Reset the event */
                ResetEvent(hWriteEvent);
            }

            wsaSendBuf.len -= dwNbrOfByteSent;
            wsaSendBuf.buf += dwNbrOfByteSent;
        }

        /* Set the WSABUF structure */
        memset(dataBuffer, 0, NUM_BYTE_TO_SEND);    

        /* this loop recv expected data */
        for(bufferCount=0;bufferCount<NUM_BYTE_TO_SEND;)
        {
            wsaRecvBuf.len = NUM_BYTE_TO_SEND-bufferCount;
            wsaRecvBuf.buf = dataBuffer+bufferCount;

            /* Initialize the WSAOVERLAPPED to 0 */
            memset(&wsaRecvOverlapped, 0, sizeof(WSAOVERLAPPED));

            /* Specify which event to signal when data is arrived*/
            wsaRecvOverlapped.hEvent = hWriteEvent;

            /* Prepare to receive data */
            err = WSARecv( server_testSockets[serverId],
                        &wsaRecvBuf,
                        dwNbrOfBuf,
                        &dwNbrOfByteSent,
                        &dwRecvFlags,
                        &wsaRecvOverlapped,
                        0 );

            if( err != SOCKET_ERROR )
            {                
                if(dwNbrOfByteSent==0)/* error if on data received */
                {
                    Trace("Server error(%d), Unexpected failure: "
                            "WSARecv() "
                            "returned %d\n",serverId ,
                            GetLastError());
                            
                    CloseEventHandle(hWriteEvent);
                    threadServerExitCode[serverId]=THREAD_FAIL;
                    ExitThread(0);                
                }
                /* Reset the event */
                ResetEvent(hWriteEvent);
            }
            else if(GetLastError()!=WSA_IO_PENDING)
            {
                Trace("Server error(%d), WSARecv() "
                        "returned %d, expected WSA_IO_PENDING\n",serverId ,
                        err );

                CloseEventHandle(hWriteEvent);
                threadServerExitCode[serverId]=THREAD_FAIL;
                ExitThread(0);
            }
            else
            {
                waitResult = WaitForSingleObject( hWriteEvent, 30000 );           
                
                if (waitResult!=WAIT_OBJECT_0)
                {                   
                    Trace("Server error(%d), Unexpected failure: "
                        "WaitForSingleObject has timed out \n",serverId);
                    
                    CloseEventHandle(hWriteEvent);
                    threadServerExitCode[serverId]=THREAD_FAIL;
                    ExitThread(0);
                }        

            }
            /* keeps track of the number of byte received */

            bufferCount+=wsaRecvOverlapped.InternalHigh;
        }

        /* compared received data with expected data */
        if(memcmp(dataBuffer,dataToSend,NUM_BYTE_TO_SEND)!=0)
        {
            Trace("Server error(%d), WSARecv(): "
                "data received was not the expected sent data\n"
                "number of byte received : wsaRecvOverlapped.InternalHigh"
                "=%d\n"
                "number of byte received : wsaRecvOverlapped.Internal"
                "=%u\n");
                
            CloseEventHandle(hWriteEvent);            
            threadServerExitCode[serverId]=THREAD_FAIL;
            ExitThread(0);
        }
            
    }
    /* close the handle to hWriteEvent */
    if(!CloseEventHandle(hWriteEvent))
    {        
        threadServerExitCode[serverId]=THREAD_FAIL;
        ExitThread(0);
    }   
    
    threadServerExitCode[serverId]=THREAD_SUCCESS;
    ExitThread(0);    
} 


/* int CloseThreadHandle(HANDLE thread)
   This function close a thread handle 
   and return an appropriate message in case of error
*/
static int CloseThreadHandle(HANDLE thread)
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
int WaitForThreadToFinish(HANDLE thread,int id, int *pReturnErrorArray)
{
    DWORD   waitResult;   
    
    if(thread==NULL)
    {
        return 0;
    }

    /* Wait for the thread to finish execution */
    waitResult = WaitForSingleObject( thread, 60000 );
            
    if (waitResult!=WAIT_OBJECT_0)
    {   
        Trace("Server error: Unexpected failure: "
              "WaitForSingleObject has timed out waiting for" 
              " client thread %d to finish.\n",id);
        return 0;
    }

    if(pReturnErrorArray[id]==THREAD_SUCCESS)
    {
        return 1;
    }    
    else if(pReturnErrorArray[id]==THREAD_FAIL)
    {
        /* return error, error msg has been handled by the client */        
        return 0;
    }
    else
    {
        Trace("Client error: Unexpected failure: "
              "No known error code defined on the client thread.\n");
        return 0;
    }
}

/* int CloseSocket(SOCKET sockList[], int numSockets)
   This function close sockets contained in sockList parameter.

*/
static int CloseSocket(SOCKET sockList[], int numSockets)
{
    int i;
    int err;

    int bResult=1; /* failure indication */

    if( (sockList != NULL) &&
        (numSockets != 0 ) )
    {
        /* Close the open sockets */
        for( i = 0 ; i < numSockets; i++ )
        {
            if( sockList[i] != INVALID_SOCKET )
            {
                err = closesocket(sockList[i]);
                if( err == SOCKET_ERROR )
                {    
                    Trace("ERROR: Unexpected failure: "
                          "closesocket(%i) returned (%d)\n", 
                          sockList[i],
                          GetLastError());

                    bResult = 0;
                }
            }

        }
    }
    else
    {
        bResult = 0;
    }
    return bResult;
}


