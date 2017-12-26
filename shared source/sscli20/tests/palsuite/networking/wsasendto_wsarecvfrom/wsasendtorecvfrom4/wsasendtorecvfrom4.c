/*=============================================================================
**
** Source: wsasendtorecvfrom4.c (WSASendTo, WSARecvFrom)
**
** Purpose: This test sends and receives a large amount of buffer. A check is 
**          done to verify that information has been sent correctly. Operations 
**          are done in UDP, Blocking and Overlapped. A separate thread is 
**          created to simulate a remote client.
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
static int CloseSocket(SOCKET sockList[], int numSockets);
 
/* We use a global variable to keep track of the 
   client thread return value because GetExitCodeThread
   is not supported on PAL
*/
int threadExitCode;

/**
 * main
 * 
 * executable entry point
 * 
 * The main act as a the server. It will create a thread of the client
 * and signal to the thread when it is ready to recv data.
 * Once the client receive the signal, it start sending data.
 * 
 * The server will not stop until the thread it created has exited.
 * For evey return path, error or not, the server will wait for
 * the client to finish its execution. This is to make sure that all
 * resource are being freed and proper error are logged.
 *
 */
INT __cdecl main(INT argc, CHAR **argv)
{
    int     i;
    int     err;        
    struct  sockaddr_in mySockaddr;
    WSADATA wsaData;
    HANDLE  hReadEvent;
    DWORD   waitResult;

    /* Thread variable */
    HANDLE hThreadClient;
    DWORD dwThreadClient;     
    DWORD dwClientParam[2]; 

    HANDLE hThreadEvent; 
    int bClientStarted=0;

    /* Sockets descriptor */
    const int numSockets = 1;    /* number of sockets used in this test */

    SOCKET testSockets[1];

     /* Variables needed for setsockopt */
    BOOL bReuseAddr = TRUE;
   
    /* Variables needed for WSARecv */
    WSABUF        wsaBuf;
    DWORD         dwNbrOfBuf  = 1;
    DWORD         dwNbrOfByteSent;
    DWORD         dwRecvFlags = 0;
    WSAOVERLAPPED wsaRecvOverlapped;
    
    /* Variable used to store transmitted data */
    unsigned char myBuffer[255];    
    
    int bufferCounter;

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
              "WSAStartup(%d) "
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
                                 SOCK_DGRAM,
                                 IPPROTO_UDP,
                                 NULL, 
                                 0, 
                                 WSA_FLAG_OVERLAPPED );


    if( testSockets[0] == INVALID_SOCKET )

    {
        Trace("Server error: Unexpected failure: "
              "WSASocketA"
              "(AF_INET,SOCK_DGRAM,IPPROTO_UDP,NULL,0,WSA_FLAG_OVERLAPPED)) "
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
        Trace("Server error: Unexpected failure: "
              "bind() socket with local address "
              "returned %d\n",
              GetLastError() );

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail("");
    }
    
    /* Create an Event with initial owner. */
    hThreadEvent = (HANDLE)CreateEvent( NULL,
        TRUE,                /* reset type */
        FALSE,                /* initially not signaled */
        (LPCSTR)"EventClientServer");  /* name of Event */

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

        CloseEventHandle(hThreadEvent);

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

        CloseEventHandle(hReadEvent);        

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        Fail( "");
    }
    
    bufferCounter = 0;    

    /* This loop call WSARecv 500 times to stress tests UDP data exchange
       over connectionless socket.
       Data received are copied in an array and verified after the all receive
       operation are done.
    */
    for(i=0;i<500;i++)
    {   
        /* reset the buffer used by WSARecv */
        memset(myBuffer, 0, 255); 

        /* Initialize the WSAOVERLAPPED to 0 */
        memset(&wsaRecvOverlapped, 0, sizeof(WSAOVERLAPPED));

        /* Specify which event to signal when data is arrived*/
        wsaRecvOverlapped.hEvent = hReadEvent; 

        /* Initialize the WSABUF structure */
        wsaBuf.buf = myBuffer;
        wsaBuf.len = 255;

        /* Prepare to receive data */
        err = WSARecvFrom( testSockets[0],
                       &wsaBuf,
                       dwNbrOfBuf,
                       &dwNbrOfByteSent,
                       &dwRecvFlags,
                       (struct sockaddr FAR *) NULL,
                       (int) NULL,
                       &wsaRecvOverlapped,
                       0 ); 

        if( err != SOCKET_ERROR )
        {            
            if(dwNbrOfByteSent==0)
            {
                /* Packet dropped, UDP is not reliable 
                   we just stop receiving
                */
                break;
            }
            /* Reset the event */
            ResetEvent(hReadEvent);
        }
        else
        {
            err = GetLastError();
            /* Only WSA_IO_PENDING is expected */            
            if(err!=WSA_IO_PENDING)
            {                
                Trace("Server error: WSARecv()"
                      "returned %d, expected WSA_IO_PENDING\n",
                      err );

                WaitForClientThreadToFinish(hThreadClient);

                CloseThreadHandle(hThreadClient);
                
                CloseEventHandle(hThreadEvent);
                
                CloseEventHandle(hReadEvent);
                
                /* Do some cleanup */
                DoWSATestCleanup( testSockets,
                                  numSockets );
        
                Fail("");
            }
        }

        /* verify if it needs to start the client thread */
        if(!bClientStarted)
        {
            if(SetEvent(hThreadEvent)==0)
            {
                Trace("Server error: Unexpected failure: "
                      "SetEvent has not set hThreadEvent as expected"
                      "GetLastError returned = %d.\n",GetLastError());

                WaitForClientThreadToFinish(hThreadClient);

                CloseThreadHandle(hThreadClient);

                CloseEventHandle(hThreadEvent);
                
                CloseEventHandle(hReadEvent);                
    
                /* Do some cleanup */
                DoWSATestCleanup( testSockets,
                                  numSockets );

                Fail("");
            }
            bClientStarted=1;
        }
        
        if(err==WSA_IO_PENDING)
        {
            /* wait for data to be read on the receive buffer */
            waitResult = WaitForSingleObject( hReadEvent,
                                            1000 );
            
            if (waitResult!=WAIT_OBJECT_0)
            {            
                /* Packet dropped, UDP is not reliable 
                   we just stop receiving
                */
                break;
            }
        }
        
        
        /* Verify that the buffer received is not bigger than the 
           the maximum specified in wsaBuf structure
        */
        if(wsaBuf.len<wsaRecvOverlapped.InternalHigh)
        {            
            Trace("Server error: "
                  "WSARecv(...) "
                  "returned wsaRecvOverlapped with InternalHigh of %d" 
                  ", expected value equal ot lower to %d\n",
                  wsaRecvOverlapped.InternalHigh, wsaBuf.len);

            WaitForClientThreadToFinish(hThreadClient);

            CloseThreadHandle(hThreadClient);

            CloseEventHandle(hThreadEvent);
            
            CloseEventHandle(hReadEvent);            
            
            /* Do some cleanup */
            DoWSATestCleanup( testSockets,
                              numSockets );
    
            Fail("");
        }
        
        /* Increment bufferCounter to keep track of the number 
           of byte received */
        bufferCounter += wsaRecvOverlapped.InternalHigh;        
    }

    if(!WaitForClientThreadToFinish(hThreadClient))
    {
        /* Error waiting for the client thread */

        /* Error message generated in function */

        CloseThreadHandle(hThreadClient);
        
        CloseEventHandle(hThreadEvent);
        
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
 
    /* Do some cleanup */
    DoWSATestCleanup( testSockets,
                      numSockets );

    
    if(bufferCounter==0)
    {
        Trace("Server error: 0 byte has been received on UDP socket");
        Fail("");
    }

    PAL_Terminate();
    return PASS;
}

/* DWORD  PALAPI Thread_Client(LPVOID lpParam)  
*  This is a client thread started by the main process.
*  It simulate a client connecting to a remote server.   
*
*  As soon as the client thread is started it calls WaitForSingleObject
*  and wait for the server to be ready to receive data. 
*
*  If the client fail somewhere it will set a global variable accordingly
*  and will call ExitThread. The main thread (server) will then continue 
*  its execution normally and handle the client error.
*
*    
*
*/
void  PALAPI Thread_Client(LPVOID lpParam)
{     
    int     i;
    int     err;
    struct  sockaddr_in mySockaddr;    

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

    HANDLE  hWriteEvent;
    DWORD   waitResult;
    
    /* Thread Event handle */
    HANDLE hThreadEvent;

    threadExitCode=THREAD_UNDEFINED;

    /* Create the EventClientServer controled by the server  */     
    hThreadEvent = (HANDLE)CreateEvent( NULL,
        TRUE,                /* ignored */
        FALSE,                /* ignored */
        (LPCSTR)"EventClientServer");  /* name of Event */

    if (hThreadEvent == NULL) 
    {        
        /* Check for error. */
        Trace( "Server Error: Unexpected failure: "
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

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }    
    
    /* Destroy the Event Handle, we don't need it anymore */
    if(!CloseEventHandle(hThreadEvent))
    {       
        /* Trace is done in CloseEventHandle */

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }


    /* Sockets initialization to INVALID_SOCKET */
    for( i = 0; i < numSockets; i++ )
    {
        testSockets[i] = INVALID_SOCKET;
    }


    /* create an overlapped stream socket in AF_INET domain */

    testSockets[0] = WSASocketA( AF_INET, 
                                 SOCK_DGRAM, 
                                 IPPROTO_UDP,
                                 NULL, 
                                 0, 
                                 WSA_FLAG_OVERLAPPED ); 


    if( testSockets[0] == INVALID_SOCKET )

    {
        Trace("Client error: Unexpected failure: "
              "WSASocketA"
              "(AF_INET,SOCK_DGRAM,IPPROTO_UDP,NULL,0,WSA_FLAG_OVERLAPPED)) "
              " returned %d\n",
              GetLastError());        

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }
    
    /* prepare the sockaddr_in structure */
    mySockaddr.sin_family           = AF_INET;
    mySockaddr.sin_port             = getRotorTestPort();
    mySockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    memset( &(mySockaddr.sin_zero), 0, 8);

    /* fill the sent buffer with value */
    for (i=0;i<255;i++)
    {
        sendBuffer[i]= i;
    }

    /* prepare data for the send operation */    

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

        /* Do some cleanup */
        CloseSocket( testSockets, numSockets );

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }    
    
    /* Set the WSABUF structure */
    wsaSendBuf.len = 255;        
    wsaSendBuf.buf = sendBuffer;
    
    byteCounter = 0;

    /*
        That loop will wsasend a 500 buffer of data with UDP datagram to
        the server thread.
    */
    for(i=0;i<500;i++)
    {        
        /* Initialize the WSAOVERLAPPED for every send operation */
        memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED));
        wsaOverlapped.hEvent = hWriteEvent;

        /* Send some data */
        err = WSASendTo( testSockets[0],
                       &(wsaSendBuf),
                       dwNbrOfBuf,
                       &dwNbrOfByteSent,
                       dwSendFlags,
                       (struct sockaddr FAR *)&mySockaddr,
                       sizeof(mySockaddr),
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

                /* Do some cleanup */
                CloseEventHandle(hWriteEvent);

                CloseSocket( testSockets, numSockets );

                threadExitCode=THREAD_FAIL;

                ExitThread(0);
            }

            /* Wait 10 seconds for WriteEvent to be signaled 
               for pending operation
            */
            waitResult = WaitForSingleObject( hWriteEvent, 
                                              10000 );            
 
            if (waitResult!=WAIT_OBJECT_0)
            {                   
                Trace("Client Error: Unexpected failure: "
                      "WaitForSingleObject has timed out \n");

                /* Do some cleanup */

                CloseEventHandle(hWriteEvent);                        
                
                CloseSocket( testSockets, numSockets );

                threadExitCode=THREAD_FAIL;

                ExitThread(0);
            }            
        }
        else
        {
            /* WSASend returned immediately 
               dwNbrOfByteSent should be 255
            */
            if(dwNbrOfByteSent!=255)
            {
                Trace("Client error, Unexpected failure: "
                      "number of byte sent by WSASend to is %d"
                      ", expected 255.\n",dwNbrOfByteSent);

                /* Do some cleanup */

                CloseEventHandle(hWriteEvent);
                
                CloseSocket( testSockets, numSockets );

                threadExitCode=THREAD_FAIL;

                ExitThread(0);
            }
            /* Reset the event */
            ResetEvent(hWriteEvent);
        }
        
        /* test if number of byte sent is different from 0
           in that case it means that the connection was closed
        */
        if(wsaOverlapped.InternalHigh==0)
        {
            Trace("Client Error: Unexpected failure: "
                  "WSASend overlapped operation sent 0 byte. "
                  "wsaOverlapped.Internal "
                  "= %d\n",wsaOverlapped.InternalHigh);
            
            /* Do some cleanup */

            CloseEventHandle(hWriteEvent);
            
            CloseSocket( testSockets, numSockets );                

            threadExitCode=THREAD_FAIL;

            ExitThread(0);            
        }

        /* keep track of the number of bytes sent */
        byteCounter += wsaOverlapped.InternalHigh;
 
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
            CloseSocket( testSockets, numSockets );

            CloseEventHandle(hWriteEvent);

            threadExitCode=THREAD_FAIL;

            ExitThread(0);
        }        
    }   
    
    
    if(!CloseEventHandle(hWriteEvent))
    {
        /* Do some cleanup */
        CloseSocket( testSockets, numSockets );

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }


    /* Expected number of bytes sent is 127500 */
    if(byteCounter!=127500)
    {
        Trace("Client error: Invalid number of byte sent to the server: %d"
               "\n",byteCounter);

        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }    

    /* Do some cleanup */
    if(!CloseSocket( testSockets, numSockets ))
    {
        threadExitCode=THREAD_FAIL;

        ExitThread(0);
    }

    threadExitCode=THREAD_SUCCESS;

    ExitThread(0);    
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

/* int WaitForThreadToFinish(HANDLE thread)
   This function return 0 if the client thread exit code 
   is clientHasFailed and 1 if the execution was successfull
*/
static int WaitForClientThreadToFinish(HANDLE thread)
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
