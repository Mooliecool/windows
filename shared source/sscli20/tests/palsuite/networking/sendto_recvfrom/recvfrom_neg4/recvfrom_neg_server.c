/*=============================================================
**
** Source: recvfrom_neg_server.c
**
** Purpose: Negatively test the recvfrom API.
**          This is a server routine. This server routine must 
**          be started before  the related client is started.
**          This test case is to test recvfrom with MSG_OOB 
**          specified plus socket option SO_OOBINLINE enabled.
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

#define BUFFERSIZE 1024


int __cdecl main(int argc, char *argv[])
{
    WORD VersionRequested = MAKEWORD(2,2);
    WSADATA WsaData;
    SOCKET aSocket;
    int optval = 1;/*set the socket option enable flag*/
    int nLength;
    int err;
    int socketID;
    struct sockaddr_in mySockaddr;
    struct sockaddr mySocketaddrConnect;
    int nSocketaddrLength;
    int nBacklogNumber = 1;
    char data[BUFFERSIZE];
    struct timeval waitTime;
    fd_set readFds;
    int socketFds;


    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }
    /*initialize to use winsock2 .dll*/
    err = WSAStartup(VersionRequested,&WsaData);
    if(err != 0)
    {
        Fail("\nFailed to find a usable WinSock DLL!\n");
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

    /*create a stream socket in AF_INET domain*/
    socketID = socket(AF_INET,SOCK_STREAM,0);

    if(INVALID_SOCKET == socketID)
    {
        Trace("\nFailed to call socket API to create a stream socket!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }


    // Wait for 5 seconds for the client to connect.
    waitTime.tv_sec = 5L;
    waitTime.tv_usec = 0L;

    /*initialize the readable socket set*/
    FD_ZERO(&readFds);

    /*prepare the sockaddr_in structure*/
    mySockaddr.sin_family = AF_INET;
    mySockaddr.sin_port = getRotorTestPort();
    mySockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    memset( &(mySockaddr.sin_zero), 0, 8);
   
    err = bind(socketID,(struct sockaddr *)&mySockaddr,
            sizeof(struct sockaddr));
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call bind API to bind a socket with "
                "local address!\n");
        err = closesocket(socketID);
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
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }


    /*to setup the backlog number for a created socket*/
    err = listen(socketID,nBacklogNumber);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call listen API to set backlog number!\n");
        err = closesocket(socketID);
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
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }


    /*add socket to readable socket set*/
    FD_SET(socketID,&readFds);
    /*mornitor the readable socket set*/
    socketFds = select(0,&readFds,NULL,NULL,&waitTime);

    if(SOCKET_ERROR == socketFds)
    {
        Trace("\nFailed to call select API to monitor readable socket set!\n");
        err = closesocket(socketID);
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
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }
    if(0 == socketFds)
    {
        Trace("\nSelect waiting time is out!\n");
        err = closesocket(socketID);
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
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    nSocketaddrLength = sizeof(mySocketaddrConnect);

    /*accept a request from client*/
    aSocket = accept(socketID,(struct sockaddr*)&mySocketaddrConnect,
            &nSocketaddrLength);

    if(INVALID_SOCKET == aSocket)
    {
        Trace("\nFailed to call accept API to accept a client request!\n");
        err = closesocket(socketID);
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
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*enalbe the socket option SO_OOBINLINE*/
    err = setsockopt(aSocket,SOL_SOCKET,SO_OOBINLINE,
        (const char *)&optval,sizeof(int));
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call setsockopt API to set socket option!\n");
        err = closesocket(aSocket);
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
        err = closesocket(socketID);
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
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    nLength = sizeof(struct sockaddr);
    /*retrive data with specified MSG_OOB*/
    err = recvfrom(aSocket,data,BUFFERSIZE,MSG_OOB,
            (struct sockaddr*)&mySockaddr,&nLength);
    if(WSAEINVAL != GetLastError() || SOCKET_ERROR != err)
    {
        Trace("\nFailed to call recvfrom API for a negative test "
                "with MSG_OOB flag!\n");
        err = closesocket(aSocket);
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
        err = closesocket(socketID);
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
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    err = closesocket(aSocket);
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

    err = closesocket(socketID);
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
