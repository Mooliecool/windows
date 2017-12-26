/*=============================================================
**
** Source: getpeername.c
**
** Purpose: Positive test getpeername function to retrieve the 
**          name of the peer to which a datagram socket is connected.
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
    WORD VersionRequested = MAKEWORD(2, 2);
    WSADATA WsaData;

    int SocketID;
    int err;
    struct sockaddr_in server;
    struct hostent *ServerHostent;
    struct sockaddr_in OutSockaddr_in;
    const char *pHostName = NULL;
    int ServerState;
    int size;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll */
    err = WSAStartup(VersionRequested, &WsaData);
    if(0 != err)
    {
        Fail("\nFailed to find a usable WinSock DLL, "
                "error code=%u\n", GetLastError());
    }

    /*Confirm that the WinSock DLL supports 2.2.*/
    if(LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2)
    {
        Trace("\nFailed to find a usable WinSock DLL!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*create a datagram socket for connection*/
    SocketID = socket(AF_INET, SOCK_DGRAM, 0);
    if(INVALID_SOCKET == SocketID)
    {
        Trace("\nFailed to call socket API to create a stream socket!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*retrive the host name from external file*/
    pHostName = getRotorTestHost();
    if(NULL == pHostName )
    {
        Trace("\nFailed to read server name!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }
    if(strlen(pHostName) == 0)
    {
        Trace("\nFailed to read server name, this is an empty host name!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*
     *retrieve host info based on the host name
    */
    ServerHostent = gethostbyname(pHostName);

    if(NULL == ServerHostent)
    {
        Trace("\nFailed to call gethostbyname API!\n");
        closesocket(SocketID);
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }
    memset( &server, 0, sizeof(server) );
    memcpy( &server.sin_addr, ServerHostent->h_addr, ServerHostent->h_length);
    server.sin_family = ServerHostent->h_addrtype;

    /* Set the port number we want to connect to */
    server.sin_port = th_htons(80);

    ServerState = connect( SocketID, (const struct sockaddr FAR *)&server,
            sizeof(server) );
    if (SOCKET_ERROR == ServerState)
    {
        Trace("\nFailed to call connect API!\n");
        err = closesocket(SocketID);
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call closesocket API!\n");
        }

        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    
    size = sizeof(struct sockaddr);

    /*retrive the peer host info*/
    err = getpeername(SocketID, (struct sockaddr *)&OutSockaddr_in, &size);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call getpeername API!\n");
        err = closesocket(SocketID);
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call closesocket API!\n");
        }

        err= WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }


    /*further check if the returned sockaddr_in struct contains valid value*/
    if(OutSockaddr_in.sin_addr.S_un.S_addr != server.sin_addr.S_un.S_addr ||
       OutSockaddr_in.sin_port != th_htons(80) || OutSockaddr_in.sin_family != AF_INET )
    {
        Trace("\nFailed to call getsockname to retrieve the peer host info, "
                    "the retrieved sockaddr_in struct dose not contain valid "
                    "peer info\n");

        err = closesocket(SocketID);
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call closesocket API!, error code=%d\n",
                    GetLastError());
        }
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!, error code=%d\n",
                GetLastError());
        }
        Fail("");
    }
      
    err = closesocket(SocketID);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call closesocket API!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
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
