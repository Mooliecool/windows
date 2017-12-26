/*=============================================================================
**
** Source: test2.c
**
** Purpose: Tests ioctlsocket by setting a socket to non-blocking.
**          It then calls recvfrom and expects a WSAEWOULDBLOCK which only
**          happens in non-blocking mode.
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


/**
 * main
 * 
 * executable entry point
 */
int __cdecl main( int argc, char **argv ) 
{
    /* local variables */
    int err;
    WORD wVersionRequested;
    WSADATA wsaData;
    DWORD dwSocketError;
    u_long one;
    struct  sockaddr_in mySockaddr;

    SOCKET sock = INVALID_SOCKET;

    /* recvfrom operation varaibles*/
    HANDLE hReadEvent;    
    char myBuffer[255];    
    DWORD         dwRecvFlags = 0;
    WSAOVERLAPPED wsaRecvOverlapped;
    int sizeOverlapped;    


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
        
        if (WSACleanup()==0 )
        {                
            Trace("WSACleanup call failed with error code %u\n",
                GetLastError() ); 
        }

        Fail(   "Requested winsock version unsupported, "
                "returned version %d.%d\n", 
                LOBYTE( wsaData.wVersion ),
                HIBYTE( wsaData.wVersion ) ); 
    }

    /* prepare the sockaddr_in structure */
    mySockaddr.sin_family           = AF_INET;
    mySockaddr.sin_port             = getRotorTestPort();
    mySockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    memset( &(mySockaddr.sin_zero), 0, 8);


    /* create a stream socket in Internet domain */
    sock = WSASocketA(  AF_INET,
                        SOCK_DGRAM,
                        IPPROTO_UDP,
                        NULL,
                        0,
                        WSA_FLAG_OVERLAPPED
                    );


    if( sock == INVALID_SOCKET )
    {
        /* check the socket-specific error code */    
        dwSocketError = GetLastError();
        
        if (WSACleanup()==0 )
        {                
            Trace("WSACleanup call failed with error code %u\n",
                GetLastError() ); 
        }

        Fail(   "WSASocket call failed with error code %d\n",
                dwSocketError ); 
    }
    
    /* set the socket to non-blocking */
    one = 1;
    err = ioctlsocket( sock, FIONBIO, &one );
    if( err != 0 )
    {
        /* check the socket-specific error code */    
        dwSocketError = GetLastError();
        Fail(   "ioctlsocket call failed with error code %d\n",
                dwSocketError ); 
    }
    
    
    /* bind local address to a socket */
    err = bind( sock,
                (struct sockaddr *)&mySockaddr,
                sizeof(struct sockaddr) );

    
    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "bind() socket with local address "
              "returned %d\n",
              GetLastError() );       

        /* Do some cleanup */
        if (closesocket( sock )==SOCKET_ERROR )
        {
            Trace("Error closing socket\n");                
        }
       
        if (WSACleanup()==0 )
        {                
            Trace("WSACleanup call failed with error code %u\n",
                GetLastError() ); 
        }

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
        if (closesocket( sock )==SOCKET_ERROR )
        {
            Trace("Error closing socket\n");                
        }
        
        if (WSACleanup()==0 )
        {                
            Trace("WSACleanup call failed with error code %u\n",
                GetLastError() ); 
        }

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
    err = recvfrom( sock,
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
        if (closesocket( sock )==SOCKET_ERROR )
        {
            Trace("Error closing socket\n");                
        }
        
        if (WSACleanup()==0 )
        {                
            Trace("WSACleanup call failed with error code %u\n",
                GetLastError() ); 
        }

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

                if (closesocket( sock )==SOCKET_ERROR )
                {
                    Trace("Error closing socket\n");                
                }

                if (WSACleanup()==0 )
                {                
                    Trace("WSACleanup call failed with error code %u\n",
                        GetLastError() ); 
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
            
            if (closesocket( sock )==SOCKET_ERROR)
            {
                Trace("Error closing socket\n");                
            }

            if (WSACleanup()==0 )
            {                
                Trace("WSACleanup call failed with error code %u\n",
                    GetLastError() ); 
            }
    
            Fail("");
        }     
    }       

    /* close the socket */
    err = closesocket( sock );
    if( err != 0 )
    {
        /* check the socket-specific error code */    
        dwSocketError = GetLastError();

         if (CloseHandle(hReadEvent)==0)
        {
            Trace("Server error: CloseHandle Failed ");      
        }

        if (WSACleanup()==0 )
        {                
            Trace("WSACleanup call failed with error code %u\n",
                GetLastError() ); 
        }

        Fail(   "closesocket call failed with error code %d\n",
                dwSocketError ); 
    }

    if (CloseHandle(hReadEvent)==0)
    {
        Trace("Server error: CloseHandle Failed "); 

        if (WSACleanup()==0 )
        {                
            Trace("WSACleanup call failed with error code %u\n",
                GetLastError() ); 
        }
        Fail("");
    }

    /* cleanup the winsock library */
    err = WSACleanup();
    if( err != 0 )
    {
        /* check the socket-specific error code */    
        dwSocketError = GetLastError();
        Fail(   "WSACleanup call failed with error code %d\n",
                dwSocketError ); 
    }
        
    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}
