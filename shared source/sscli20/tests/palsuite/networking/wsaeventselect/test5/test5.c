/*=============================================================================
**
** Source: test5.c (WSASend, WSARecv)
**
** Purpose: This test calls WSAEventSelect with FD_ACCEPT in order to handle  
**          an incoming connection. After WSAEventSelect is called, 
**          It waits for the specified event to be signaled before calling the
**          accept function. Once done, it sends and receives a buffer using 
**          wsasend and wsarecv. The operations are TCP, Non-Blocking and 
**          Overlapped. The data transfered is compared at the end of the 
**          test.
**
**          Note: Calling WSAEventSelect enable the Non-Blocking mode 
**          automatically no matter what the FD paramater is.
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

/**
 * Global variables
 */

const int       listenBacklog   = 1;    /* backlog for listen */
const int       THREAD_FAIL     = 0; /*  Thread fail error code */
const int       THREAD_SUCCESS  = 1;    /* Thread success error code */
const int       THREAD_UNDEFINED = 2;    /* Thread undefined error code */
const char      srcString[] = "test for wsasend and wsarecv";

/* Thread that is used as a client */
void PALAPI Thread_Client(LPVOID lpParam);

/* utility cleanup function */
static int CloseEventHandle(HANDLE Event);
static int CloseThreadHandle(HANDLE thread);
static int WaitForClientThreadToFinish(HANDLE thread);
static int CloseSocket(SOCKET sockList[], int numSockets);
static void DoWSATestCleanup( SOCKET sockList[], int numSockets );
 
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

    /* Sockets descriptor */
    const int numSockets = 2;    /* number of sockets used in this test */

    SOCKET testSockets[2];

     /* Variables needed for setsockopt */
    BOOL bReuseAddr = TRUE;    

    /* Variables needed for WSAEventSelect */    
    int    socketFds;

    /* Variables needed for WSARecv */
    WSABUF        wsaBuf;
    DWORD         dwNbrOfBuf  = 1;
    DWORD         dwNbrOfByteSent;
    DWORD         dwRecvFlags = 0;
    WSAOVERLAPPED wsaRecvOverlapped;
    
    /* Variable used to store transmitted data */
    unsigned char myBuffer[255];
    
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
              "returned %u\n",
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
              "(AF_INET SOCK_STREAM IPPROTO_TCP NULL 0 WSA_FLAG_OVERLAPPED)) "
              " returned %u\n",
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
              "setsockopt(.. SOL_SOCKET SO_REUSEADDR  ..) "
              "returned %u\n",
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
              "returned %u\n",
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
              "returned %u\n",
              GetLastError() );

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
              "returned %u\n",
              GetLastError());

        WaitForClientThreadToFinish(hThreadClient);        

        CloseThreadHandle(hThreadClient); 

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
        
    }

    /* This will signal hReadEvent when a connection
       is pending.       
    */
    socketFds = WSAEventSelect( testSockets[0],
                                hReadEvent,
                                FD_ACCEPT);
    

    if( socketFds == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure "
              "with WSAEventSelect\n");

        WaitForClientThreadToFinish(hThreadClient);

        CloseThreadHandle(hThreadClient);

        CloseEventHandle(hReadEvent); 

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }
   

    /* Wait 10 seconds for ReadEvent to be signaled 
       from the pending operation. Once it is signaled
       accept can be done.
    */
    waitResult = WaitForSingleObject( hReadEvent, 
                                        10000 );    
    
    if (waitResult!=WAIT_OBJECT_0)
    {           
        Trace("Server error: Unexpected failure: "
            "WaitForSingleObject has timed out \n");

        WaitForClientThreadToFinish(hThreadClient);                

        CloseThreadHandle(hThreadClient);

        CloseEventHandle(hReadEvent);

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
              "returned %u\n",
              GetLastError());

        WaitForClientThreadToFinish(hThreadClient);

        CloseThreadHandle(hThreadClient); 

        CloseEventHandle(hReadEvent); 

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    /* Initialize the WSABUF structure */
    memset(myBuffer, 0, 255);

    wsaBuf.buf = myBuffer;
    wsaBuf.len = 255;    
    
    /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaRecvOverlapped, 0, sizeof(WSAOVERLAPPED));

    /* Specify which event to signal when data is arrived*/
    wsaRecvOverlapped.hEvent = hReadEvent;

    /* reset the buffer used by WSARecv */
    memset(myBuffer, 0, 255);
    
    /* Prepare to receive data */
    err = WSARecv( testSockets[1],
                &wsaBuf,
                dwNbrOfBuf,
                &dwNbrOfByteSent,
                &dwRecvFlags,                
                &wsaRecvOverlapped,
                0 );   

    if( err != SOCKET_ERROR )
    {
        if(dwNbrOfByteSent==0)
        {
            Trace("Server error: WSARecv() "
                        "returned  %d, SOCKET_ERROR\n",
                        err );
                
            WaitForClientThreadToFinish(hThreadClient);            
            
            CloseThreadHandle(hThreadClient);

            CloseEventHandle(hReadEvent);
            
            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                            numSockets );

            Fail("");
        }
    }
    else
    {
        err = GetLastError();
        /* Only WSA_IO_PENDING is expected */
        if(err!=WSA_IO_PENDING)
        {
            Trace("Server error: WSARecv() "
                    "returned %u, expected WSA_IO_PENDING\n",
                    err );
                
            WaitForClientThreadToFinish(hThreadClient);            
            
            CloseThreadHandle(hThreadClient);

            CloseEventHandle(hReadEvent);
            
            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                            numSockets );
    
            Fail("");
        }
        /* Wait 10 seconds for ReadEvent to be signaled 
           from the pending operation
        */
        waitResult = WaitForSingleObject( hReadEvent, 
                                            10000 );    
        
        if (waitResult!=WAIT_OBJECT_0)
        {           
            Trace("Server error: Unexpected failure: "
                "WaitForSingleObject has timed out \n");

            WaitForClientThreadToFinish(hThreadClient);                

            CloseThreadHandle(hThreadClient);

            CloseEventHandle(hReadEvent);                

            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                            numSockets );

            Fail("");
        }
    }    

    /* Verify that the buffer received is not bigger than the 
        the maximum specified in wsaBuf structure
    */
    if(wsaBuf.len<wsaRecvOverlapped.InternalHigh)
    {
        Trace("ERROR: "
                "WSARecv(...) "
                "returned wsaRecvOverlapped with InternalHigh of %d " 
                "expected value equal ot lower to %d\n",
                wsaRecvOverlapped.InternalHigh, wsaBuf.len);

        WaitForClientThreadToFinish(hThreadClient);                

        CloseThreadHandle(hThreadClient);

        CloseEventHandle(hReadEvent);                

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                        numSockets );

        Fail("");
    } 
    
    /* Disconnect the socket */
    err = shutdown( testSockets[1], 
                    SD_BOTH);
    if (err == SOCKET_ERROR)
    {                
        Trace("ERROR: Unexpected failure: "
                "shutdown() socket with local server "
                "returned %u\n",
                GetLastError());  

       WaitForClientThreadToFinish(hThreadClient);                

       CloseThreadHandle(hThreadClient);

       CloseEventHandle(hReadEvent);                

       /* Do some cleanup */
       DoWSATestCleanup( testSockets,
                            numSockets );

       Fail("");            
    }

    if(!WaitForClientThreadToFinish(hThreadClient))
    {
        CloseThreadHandle(hThreadClient);

        CloseEventHandle(hReadEvent);

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }

    if(!CloseThreadHandle(hThreadClient)||
       !CloseEventHandle(hReadEvent))
    {
        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );
        Fail("");
    }     

    /* verify that all data in the data array are as expected */
    if(memcmp(srcString,myBuffer, strlen(srcString))!=0)
    {
        Trace("Server Error comparing received with send data");

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
    
    unsigned char   sendBuffer[255];    

    WSAOVERLAPPED wsaOverlapped;

    /* variable for iocltsocket */
    u_long argp;

    /* Variables needed for select */
    struct timeval waitTime;
    fd_set writeFds;    
    int    socketFds;    

    HANDLE  hWriteEvent;
    DWORD   waitResult;   

    threadExitCode=THREAD_UNDEFINED;
    
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
        Trace("Client error: Unexpected failure: "
              "WSASocketA"
              "(AF_INET SOCK_STREAM IPPROTO_TCP NULL 0 WSA_FLAG_OVERLAPPED) "
              " returned %u\n",
              GetLastError());

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }

    /* enable non blocking socket */
    argp=1;
    err = ioctlsocket(testSockets[0], FIONBIO, (u_long FAR *)&argp);

    if (err==SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "ioctlsocket(.., FIONBIO, ..) "
              "returned %u\n",
              GetLastError() );

        /* Do some cleanup */
        CloseSocket( testSockets, numSockets );
       
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
        err = GetLastError();
        if ( err != WSAEWOULDBLOCK )
        {
            Trace("ERROR: Unexpected failure: "
              "connect() socket with local server "
              "returned %u expected WSAEWOULDBLOCK\n",
              GetLastError());

            /* Do some cleanup */
            CloseSocket( testSockets, numSockets );

            threadExitCode=THREAD_FAIL;

            ExitThread(0);
        }     
        
    }

    /* set the server waiting time as 10 seconds */
    waitTime.tv_sec = 10L;
    waitTime.tv_usec = 0L;

    /* initialize the except socket set  */
    FD_ZERO( &writeFds );

    /* add socket to readable socket set */
    FD_SET( testSockets[0], 
            &writeFds );

    /* monitor the readable socket set to determine the
       completion of the connection request.
    */
    socketFds = select( 0,                        
                        NULL,
                        &writeFds,
                        NULL,
                        &waitTime);

    if( socketFds == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure "
              "with select\n");

        /* Do some cleanup */
        CloseSocket( testSockets, numSockets );
        
        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }

    if( socketFds == 0 )
    {
        Trace("ERROR: Unexpected select "
              "timed out\n");

        /* Do some cleanup */
        CloseSocket( testSockets, numSockets );
        
        threadExitCode=THREAD_FAIL;

        ExitThread(0);
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
              "returned %u\n",
              GetLastError());

        /* Do some cleanup */
        CloseSocket( testSockets, numSockets );

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }   
    
    memcpy(sendBuffer, srcString, strlen(srcString));
    /* Set the WSABUF structure */
    wsaSendBuf.len = 255;        
    wsaSendBuf.buf = sendBuffer;
    
    /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED));  
    wsaOverlapped.hEvent = hWriteEvent;

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
        /* Handle the overlapped operation */
        if(GetLastError()!=WSA_IO_PENDING)
        {
            Trace("Client error: Unexpected failure: "
                "WSASend() "
                "returned %u\n",
                GetLastError());
                    
            CloseEventHandle(hWriteEvent);

            /* Do some cleanup */
            CloseSocket( testSockets, numSockets );

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
            CloseSocket( testSockets, numSockets );

            threadExitCode=THREAD_FAIL;
            ExitThread(0);
        }        
    }
    
    /* Verify that the number of bytes sent are the number of byte
        specified in the wsaBuf structure 
    */
    if(wsaSendBuf.len!=wsaOverlapped.InternalHigh)
    {            
        Trace("Server error: Array out of bound "
            "while writing buffer received in myData.\n");

        CloseEventHandle(hWriteEvent);

        /* Do some cleanup */
        CloseSocket( testSockets, numSockets );

        threadExitCode=THREAD_FAIL;
        ExitThread(0);
    }

    /* close the handle to hWriteEvent */
    if(!CloseEventHandle(hWriteEvent)||
       !CloseSocket( testSockets, numSockets ))
    {
        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }
        
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
                  "returned %u\n",
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
              "returned %u\n",
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
                          "closesocket(%i) returned (%u)\n", 
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

/**
 * DoTestCleanup
 *
 * Cleans up the test by
 * closing the active sockets (if any), 
 * cleaning up WSA 
 */
static void DoWSATestCleanup( SOCKET sockList[],int numSockets )
{
    int err;      /* generic variable to store the error into */
    BOOL bFailed; /* failure indication */

    bFailed = !(CloseSocket(sockList, numSockets));
 
    /* terminate use of WinSock DLL */
    err = WSACleanup( );
    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "WSACleanup returned %u",
              GetLastError());

        bFailed = TRUE;
    }

    /* Call Fail("") if any failures occured */
    if( bFailed == TRUE )
    {
        Fail("");
    }
}
