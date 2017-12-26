/*=============================================================================
**
** Source: wsarecv2_client.c (WSASend, WSARecv)
**
** Purpose: Test to ensure that Recv returns WSAENOTCONN when we call 
**          it from an unconnected socket.
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
    WSADATA wsaData;

    /* Sockets descriptor */
    const int numSockets = 1;    /* number of sockets used in this test */

    SOCKET testSockets[1];


    /* Variables for WSASend */
    WSABUF wsaBuf[1];
    DWORD  dwNbrOfByteSent;
    DWORD  dwNbrOfBuf  = 1;
    DWORD  dwRecvFlags = 0;    
    char   myBuffer[255] = "violets are blue, roses are red";

    WSAOVERLAPPED wsaOverlapped;

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

    /* Do not connect to a server */    

    /* Initialize the WSAOVERLAPPED to 0 */
    memset(&wsaOverlapped, 0, sizeof(WSAOVERLAPPED));
    /* Initialize the WSABUF structure */
    wsaBuf[0].buf = myBuffer;
    wsaBuf[0].len = 255;

    err = WSARecv( testSockets[0],
                   wsaBuf,
                   dwNbrOfBuf,
                   &dwNbrOfByteSent,
                   &dwRecvFlags,
                   &wsaOverlapped,
                   0 );

    /* connect hasn't been called, we expect a WSAENOTCONN error */
    if( err != SOCKET_ERROR)      
    {
        
        Trace("ERROR: "
              "WSARecv(...) "
              "returned %d, expected SOCKET_ERROR\n",
              err ); 

         /* Do some cleanup */
        DoWSATestCleanup( testSockets,
                          numSockets );
        
        Fail("");
    }      
    else if( GetLastError()!= WSAENOTCONN )
    {
        Trace("ERROR, WSARecv(...) returned %d "
              "when it should have returned WSAENOTCONN\n", 
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












