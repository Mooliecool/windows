/*=============================================================================
**
** Source: wsasend8_client.c (WSASend)
**
** Purpose: This negative test tries to send a buffer with size larger than
**          SO_MAX_MSG_SIZE. Operations are done with a UDP socket 
**          in Non-blocking mode.
**          Normally on Windows, the error that tells that the buffer is too large
**          is retrieved through a call to WSAGetOverLappedResult. Since it is not 
**          supported on FreeBSD, this test only checks the number of bytes sent,
**          which should be 0 - otherwise the test fails.
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

/* utility cleanup function */
static int CloseEventHandle(HANDLE Event);
 
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

    /* variable for iocltsocket */
    u_long argp;    
    
    /* Variables for WSASend */

    WSABUF wsaSendBuf;
    DWORD  dwNbrOfByteSent;
    DWORD  dwNbrOfBuf  = 1;
    DWORD  dwSendFlags = 0;
    
    unsigned char   *sendBuffer;
    

    WSAOVERLAPPED wsaOverlapped;
    
    /* getsockopt variable */
    unsigned int so_max_msg_size;
    unsigned int buffer_too_large;
    unsigned int optlen;    

    /* Socket DLL version */
    const WORD wVersionRequested = MAKEWORD(2,2);

    HANDLE  hWriteEvent;
    DWORD   waitResult;        

    /* PAL initialization */
    if( PAL_Initialize(argc, argv) != 0 )
    {        
        return FAIL;
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
        
        Fail("");
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
        Trace("Client error: Unexpected failure: "
              "WSASocketA"
              "(AF_INET,SOCK_DGRAM,IPPROTO_UDP,NULL,0,WSA_FLAG_OVERLAPPED) "
              " returned %d\n",
              GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( 0, 0);

        
        Fail("");
    }

    /* enable non blocking socket */
    argp=1;
    err = ioctlsocket(testSockets[0], FIONBIO, (u_long FAR *)&argp);

    if (err==SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "ioctlsocket(.., FIONBIO, ..) "
              "returned %d\n",
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

        
        Fail("");
    }       

    /* create events */
    hWriteEvent = CreateEvent( NULL, /* no security   */
                             FALSE,    /* reset type    */
                             FALSE,    /* initial state */
                             NULL );   /* object name   */
            
    if( hWriteEvent == NULL )
    {   
        CloseEventHandle(hWriteEvent);

        Trace("Client error: Unexpected failure: "
              "CreateEvent() "
              "returned %d\n",
              GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );

        
        Fail("");
    }

        
    /* get the SO_MAX_MSG_SIZE for the current socket    */
    optlen=sizeof(unsigned int);
    err = getsockopt(
        testSockets[0],         
        SOL_SOCKET,        
        SO_MAX_MSG_SIZE,      
        (char FAR *)(&so_max_msg_size),
        &optlen
        );

    if(err==SOCKET_ERROR)
    {        
        CloseEventHandle(hWriteEvent);

        Trace("Client error: Unexpected failure: "
                "getsockopt() "
                "returned %d\n",
                GetLastError());

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                            numSockets );
        
        Fail("");
    }
    
    /* set the size bigger than supported */
    buffer_too_large = so_max_msg_size + 100;

     /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED));

    wsaOverlapped.hEvent = hWriteEvent;    
    
    /* Set the WSABUF structure */
    sendBuffer = malloc(buffer_too_large * sizeof(char));

    wsaSendBuf.len = buffer_too_large;
    wsaSendBuf.buf = sendBuffer;
   
    /* Send some data */
    err = WSASend( testSockets[0],
                   &wsaSendBuf,
                   dwNbrOfBuf,
                   &dwNbrOfByteSent,
                   dwSendFlags,
                   &wsaOverlapped,
                   0 );

    if(err != SOCKET_ERROR )
    {
        
        Trace("Client WSASend has "
              "return %d, expected SOCKET_ERROR.",err);

        free(sendBuffer);

        CloseEventHandle(hWriteEvent);

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );
        
        Fail("");
    }
        
    if(GetLastError()!=WSA_IO_PENDING)
    {
        Trace("Client error: Unexpected failure: "
                "WSASend() "
                "returned %d\n",
                GetLastError());

        free(sendBuffer);   

        CloseEventHandle(hWriteEvent);            

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                            numSockets );
        
        Fail("");
    }

    /* Wait 10 seconds for hWriteEvent to be signaled 
        for pending operation
    */
    waitResult = WaitForSingleObject( hWriteEvent,
                                        10000 );
    free(sendBuffer);
    
    if (waitResult!=WAIT_OBJECT_0)
    {   
        Trace("Client Error: Unexpected failure: "
                "WaitForSingleObject has timed out \n");
        
        CloseEventHandle(hWriteEvent);
        
        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                            numSockets );
        
        Fail("");
    }   


    /* Verify that the number of bytes sent is 0       
    */
    if(wsaOverlapped.InternalHigh!=0)
    {   
        Trace("Client WSASend was called with a buffer larger than"
              " SO_MAX_MSG_SIZE "
              "wsaSendBuf.len = %d, "
              "wsaOverlapped.InternalHigh = % d, "
              "so_max_msg_size = %d, "
              "buffer_too_large = %d, ",
              wsaSendBuf.len, wsaOverlapped.InternalHigh,
              so_max_msg_size, buffer_too_large);

        CloseEventHandle(hWriteEvent);

        /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );
        
        Fail("");
    }

    /* 
    
    According to MSDN, 
    For message-oriented sockets, care must be taken not to exceed the 
    maximum message size of the underlying transport, which can be 
    obtained by getting the value of socket option SO_MAX_MSG_SIZE. 
    If the data is too long to pass atomically through the underlying 
    protocol the error WSAEMSGSIZE is returned, and no data is transmitted.

    
    wsaOverlapped.Internal does not return WSAEMSGSIZE.
    The only other way to test is to call WSAGetOverlappedResult and to
    verify if wFlag is WSAEMSGSIZE. But WSAGetOverlappedResult is not
    supported.
    
    Test removed: Verify that wsaOverlapped.Internal is WSAEMSGSIZE.             

    */

    if(!CloseEventHandle(hWriteEvent))
    {
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

