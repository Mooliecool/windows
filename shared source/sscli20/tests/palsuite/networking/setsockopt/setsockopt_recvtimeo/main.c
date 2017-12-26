/*=============================================================
**
** Source: main.c
**
** Purpose: Positive test the setsockopt function in SOL_SOCKET
**          and IPPROTO_TCP level to set SO_RCVTIMEO option
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
**============================================================*/

#include <palsuite.h>

/* Timeout 10 seconds */
#define TIMEOUT         10000

/* Request Timeout 2 seconds */
int REQUEST_TIMEOUT=2000;

int testHasFailed=0;

/* Global variables  */
const int       listenBacklog   = 1;    /* backlog for listen */

SOCKET clientSocket;
SOCKET serverSocket;

/* Thread that is used as a Client */
void PALAPI Thread_Client(LPVOID lpParam);

/* Thread that is used as a Server */
void PALAPI Thread_Server(LPVOID lpParam);

/* utility cleanup function */
static int CloseHandleHelper(HANDLE handle);

/*
 * main
 * 
 * executable entry point
 */
INT __cdecl main(INT argc, CHAR **argv)
{
    INT ret = 0;
    int     err; 
    int     i;
    SOCKET Socket;

    /* Socket DLL version */
    const WORD wVersionRequested = MAKEWORD(2,2);
    WSADATA wsaData; 
    
     /* Variables needed for setsockopt */
    BOOL bReuseAddr = TRUE;  

    int     addrlen = sizeof(struct sockaddr);
    struct  sockaddr_in mySockaddr;
    
    
    HANDLE clientThreadHandle;
    DWORD clientThreadId;
 
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
        Fail( "Main error: Unexpected failure: "
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
        Fail("Main error: Unexpected failure "
        "to find a usable version of WinSock DLL\n");
    }

    /* create an WSA_FLAG_OVERLAPPED stream socket in AF_INET domain */
    Socket = WSASocketA(AF_INET, 
                        SOCK_STREAM, 
                        IPPROTO_TCP,
                        NULL, 
                        0, 
                        WSA_FLAG_OVERLAPPED ); 

    if( Socket == INVALID_SOCKET )
    {   
        Trace("Server error: Unexpected failure: "
            "WSASocketA"
            "(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED)) "
            " returned %d\n",
            GetLastError());
        Fail ("Setup: Error: Failed @ WSASocketA in main thread\n");
    }

    /* Allows the socket to be bound to an address that is already in use. */
    err = setsockopt( Socket,
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
        Fail ("Setup: Error: Failed @ setsock opt in main thread\n");
    }

    /* prepare the sockaddr structure */
    mySockaddr.sin_family           = AF_INET;
    mySockaddr.sin_port             = getRotorTestPort();
    mySockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    memset( &(mySockaddr.sin_zero), 0, 8);

    /* bind local address to a socket */
    err = bind( Socket,
                (struct sockaddr *)&mySockaddr,
                sizeof(struct sockaddr) );

    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
            "bind() socket with local address "
            "returned %d\n",
            GetLastError() );
        Fail ("Setup: Error: Failed @ bind in main thread\n");
    }
      
    clientThreadHandle =   CreateThread( 
                        NULL,                   /* no security attributes */
                        0,                      /* use default stack size */
                        (LPTHREAD_START_ROUTINE)Thread_Client,/* thread function */
                         NULL,  /* argument to thread function */
                        0,                      /* use default creation flags  */
                         &clientThreadId) ;    /* returns the thread identifier*/

    if(clientThreadHandle == NULL )
    {        
        Trace( "Main error(%d), Unexpected failure: "
                "CreateThread() for Client "
                "returned NULL\n");
        
        testHasFailed=1;
        goto TEST_OVER;

    }

    /* listen to the socket */
    err = listen( Socket, listenBacklog );
    if( err == SOCKET_ERROR )
    {
        Trace("Server error(%d), Unexpected failure: "
            "listen() to sockets "
            "returned %d\n", i,
            GetLastError() );            

        testHasFailed=1;
        goto TEST_OVER;
    }

    /* accept connection */
    serverSocket = accept( Socket,
                            (struct sockaddr *)&mySockaddr,
                            &addrlen );


    if( serverSocket == INVALID_SOCKET )
    {
        Trace("Server error(%d), Unexpected failure: "
            "accept() connection on socket\n ",
            GetLastError());
        testHasFailed=1;
        goto TEST_OVER;
    }

  //  Sleep(6000); //We don't send any data, we wait for client to timeout

    err = WaitForSingleObject(clientThreadHandle, TIMEOUT);
    if(err != WAIT_OBJECT_0)
    {
      Trace("Wait for Object(s) for client returned %d, and GetLastError value is %d\n", err, GetLastError());
      testHasFailed=1;
      goto TEST_OVER;
    }

    Trace("Main:Test Run over, Cleaning up and publishing results \n");

TEST_OVER:
    if( !CloseHandleHelper( clientThreadHandle ) )
    {
        Trace("Error:%d: Closing Handle for client thread handle\n", GetLastError());
        testHasFailed=1;
    }

    err = closesocket(serverSocket);
    if( err == SOCKET_ERROR )
    {    
        Trace("ERROR: Unexpected failure: "
                "closesocket(serverSocket) returned (%d)\n", 
                GetLastError());
        testHasFailed=1;
    }

    err = closesocket(clientSocket);
    if( err == SOCKET_ERROR )
    {    
        Trace("ERROR: Unexpected failure: "
                "closesocket(clientSocket) returned (%d)\n", 
                GetLastError());
        testHasFailed=1;
    }

    err = closesocket(Socket);
    if( err == SOCKET_ERROR )
    {    
        Trace("ERROR: Unexpected failure: "
                "closesocket(Socket) returned (%d)\n", 
                GetLastError());
        testHasFailed=1;
    }

    err = WSACleanup( );
    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "WSACleanup returned %d",
              GetLastError());
        testHasFailed=1;
    }


    if(testHasFailed)
    {
        Fail("Test Failed.\n");
    }else
    {
         Trace("Test Passed.\n");
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
    int     err; //used as return code & also number of bytes using recv

    /* Socket variables */
    struct  sockaddr_in mySockaddr; //data to connect to Server     
    int sockOptionRecvTimeOut = 0; // Socket TimeOut option
    int sockBuferLen = 0;
 
    unsigned int counterRecvBufferSize = 10;
    char tmpRecvBuffer[10]     = "recv buf";

    /* Create AF_INET, TCP socket */
    clientSocket          = socket( AF_INET, 
                                    SOCK_STREAM, 
                                    IPPROTO_TCP
                                    ); 

    if( clientSocket == INVALID_SOCKET )
    {
        Trace("Client error: Unexpected failure: "
              "socket(AF_INET,SOCK_STREAM,IPPROTO_TCP) "
              " returned %d\n",  GetLastError());        
        testHasFailed = 1;
        ExitThread(0);
    }

    
    /* prepare the sockaddr_in structure */
    mySockaddr.sin_family           = AF_INET;
    mySockaddr.sin_port             = getRotorTestPort();
    mySockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    memset( &(mySockaddr.sin_zero), 0, 8);
    
    /* Set and check for REQUEST_TIMEOUT for SO_RCVTIMEO */

    if( REQUEST_TIMEOUT > 0 )
    {
        sockBuferLen = sizeof(int);
        sockOptionRecvTimeOut = 0; /*hold the retrieved value*/

        err = setsockopt( clientSocket,
                        SOL_SOCKET,
                        SO_RCVTIMEO,
                        (char *)&REQUEST_TIMEOUT,
                        sockBuferLen );

        if( err == SOCKET_ERROR )
        {
            Trace("Client error: Unexpected failure: "
                "setsockopt(.., SOL_SOCKET,SO_RCVTIMEO, ..) "
                "returned %d, for client %d\n",
                GetLastError() );
            testHasFailed = 1;
            ExitThread(0);        
        }

        err = getsockopt(
                        clientSocket,
                        SOL_SOCKET,
                        SO_RCVTIMEO,
                        (char *)&sockOptionRecvTimeOut,
                        &sockBuferLen 
                        );
    
        if( sockOptionRecvTimeOut != REQUEST_TIMEOUT )
        {
            Trace("Client error: Unexpected failure: "
                "getsockopt(.., SOL_SOCKET,SO_RCVTIMEO, ..) "
                "returned %d, for client with recvbufferlen[%d], sockOptionRecvTimeOut[%d]\n",
                GetLastError(), sockBuferLen, sockOptionRecvTimeOut );
            testHasFailed = 1;
            ExitThread(0);        
        }        
    
    }


    /* connect to a server */
    err = connect( clientSocket, 
                   (struct sockaddr *)&mySockaddr,
                   sizeof(struct sockaddr));

    if( err == SOCKET_ERROR )
    {
        err = GetLastError();
        Trace("Client error Unexpected failure: "
              "connect() socket with local server "
              "returned %d\n",GetLastError());
        testHasFailed = 1;
        ExitThread(0);
    }
    
  

    /* Recv some data */
    err = recv( clientSocket,
                tmpRecvBuffer,
                counterRecvBufferSize,
                0
                );

    if( err == SOCKET_ERROR )
    {
        if(GetLastError() ==  WSAETIMEDOUT)
        {
            Trace("Client Success: Socket timedout recv with error WSAETIMEDOUT[%d]\n", GetLastError());
            ExitThread(0);
         }
        
        Trace("Client Error: GetLastError returned value [%d] other than WSAETIMEDOUT\n", GetLastError());
        testHasFailed = 1;
        ExitThread(0);
    }
    else
    {
        Trace("Client Error: GetLastError returned success value [%d] other than WSAETIMEDOUT, err value is %d\n", GetLastError(), err);
        testHasFailed = 1;
        ExitThread(0);

    }

    testHasFailed = 1;
    ExitThread(0);


} 

/* int CloseHandleHelper(HANDLE handle)
   This function close a Thread/Event/Socket handle 
   and return an appropriate message in case of error
*/
static int CloseHandleHelper(HANDLE handle)
{   
    if(handle!=NULL)
    {
        if( CloseHandle(handle) == 0 )
        {
            Trace("Server error: Unexpected failure: "
                  "CloseHandle(handle) "
                  "returned %d\n",
                GetLastError());
            return 0;
        }
    }
    return 1;
}
