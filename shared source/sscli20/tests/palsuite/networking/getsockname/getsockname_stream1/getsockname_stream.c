/*=============================================================
**
** Source: getsockname_stream.c
**
** Purpose: Positive test the getsockname function to retrive 
**            the local name for a connected stream socket.
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

int __cdecl main(int argc, char *argv[])
{

    WORD VersionRequested = MAKEWORD(2,2);
    WSADATA WsaData;

    int err;
    int socketID;
    struct sockaddr_in server;
    struct hostent *serverHostent;
    struct servent *myServent;
    struct sockaddr mySockaddr;
    const char *HostName = getRotorTestHost();
    int serverState;
    int size;

    /* Initialize the PAL environment
    */
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        ExitProcess(FAIL);
    }


    /* initialize to use winsock2.dll
    */
    err = WSAStartup(VersionRequested,&WsaData);

    if ( err != 0 )
    {
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    /* Confirm that the WinSock DLL supports 2.2.
    */
    if ( LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2 )
    {
        Trace("\nFailed to find a usable WinSock DLL!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Fail("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }


    /* here the AF_INET is the same as PF_INET
     * create a stream socket in AF_INET domain
     */
    socketID = socket(AF_INET,SOCK_STREAM,0);

    if(INVALID_SOCKET == socketID)
    {
        Trace("\nFailed to call socket API to create a stream socket!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Fail("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }


    serverHostent = gethostbyname(HostName);
    if(NULL == serverHostent)
    {
        Trace("\nFailed to call gethostbynam API!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Fail("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }
    memset( &server, 0, sizeof(server) );
    memcpy( &server.sin_addr, serverHostent->h_addr, serverHostent->h_length);
    server.sin_family = serverHostent->h_addrtype;


    myServent = getservbyname("ftp","tcp");

    if(NULL == myServent)
    {
        Trace("\nFailed to call getservbyname API!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Fail("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }
    server.sin_port = myServent->s_port;

    serverState = connect( socketID, (const struct sockaddr FAR *)&server,
            sizeof(server) );

    if (serverState != 0)
    {
        Trace("\nFailed to call connect API!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Fail("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*  test the getsockname;
    */
    size = sizeof(struct sockaddr);

    /* retrive the local name for the socket 
    */
    err = getsockname(socketID,&mySockaddr,&size);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call getsockname API!\n");
        err = closesocket(socketID);
        if(SOCKET_ERROR == err)
        {    
            Trace("\nFailed to call closesocket API!\n");
            err = WSACleanup();
            if(SOCKET_ERROR == err)
            {
                Fail("\nFailed to call WSACleanup API!\n");
            }
            Fail("");
        }
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Fail("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    err = closesocket(socketID);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call closesocket API!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Fail("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!\n");
    }

    PAL_Terminate();
    return PASS;
}
