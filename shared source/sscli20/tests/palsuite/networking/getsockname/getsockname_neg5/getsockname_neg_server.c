/*=============================================================
**
** Source: getsockname_neg_server.c
**
** Purpose: Negatively test the getsockname APIs.
**          This is a server routine. This server routine must be 
**          be started before the related client is started.
**          This test case is for stream socket.
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
    SOCKET aSocket;

    int err;
    int socketID;
    struct sockaddr_in mySockaddr;
    struct sockaddr mySocketaddrConnect;
    int nSocketaddrLength;
    int nBacklogNumber = 1;
    struct timeval waitTime;
    fd_set readFds;
    int socketFds;


    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested,&WsaData);

    if(err != 0)
    {
        Fail("\nFailed to find a usable WinSock DLL!, error code=%d\n",
                GetLastError());
    }

    /*Confirm that the WinSock DLL supports 2.2.*/
    if(LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2)
    {
        Trace("\nFailed to find a usable WinSock DLL!, error code=%d\n",
                GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!, error code=%d\n",
                GetLastError());
        }
        Fail("");
    }

    /*
    here the AF_INET is the same as PF_INET
    create a stream socket in AF_INET domain
    */
    socketID = socket(AF_INET, SOCK_STREAM, 0);

    if(INVALID_SOCKET == socketID)
    {
        Trace("\nFailed to call socket API to create a stream socket!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!, error code=%d\n",
                GetLastError());
        }
        Fail("");
    }

    // Wait for 5 seconds for the client to connect.
    waitTime.tv_sec = 5L;
    waitTime.tv_usec = 0L;

    //initialize the except socket set
    FD_ZERO(&readFds);


    /*prepare the sockaddr_in structure*/
    mySockaddr.sin_family = AF_INET;
    mySockaddr.sin_port = getRotorTestPort();
    mySockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    memset( &(mySockaddr.sin_zero), 0, 8);

    /*bind the local address to the created socket*/
    err = bind(socketID, (struct sockaddr *)&mySockaddr,
                sizeof(struct sockaddr));
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call bind API to bind the stream socket "
                "local address!, error code=%d\n", GetLastError());
        err = closesocket(socketID);
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

    /*to setup the backlog number for a created socket*/
    err = listen(socketID,nBacklogNumber);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call listen API!, error code=%d\n",
                GetLastError());
        err = closesocket(socketID);
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

    /*add socket to readable socket set*/
    FD_SET(socketID,&readFds);

    /*mornitor the readable socket set*/
    socketFds = select(0, &readFds, NULL, NULL, &waitTime);

    if(SOCKET_ERROR == socketFds)
    {
        Trace("\nFailed to call select API!, error code=%d\n",
                GetLastError());
        err = closesocket(socketID);
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
    if(0 == socketFds)
    {
        Trace("\nSelect waiting time is out!, error code=%d\n",
                GetLastError());
        err = closesocket(socketID);
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


    nSocketaddrLength = sizeof(mySocketaddrConnect);

    /*accept a request from client*/
    aSocket = accept(socketID, (struct sockaddr*)&mySocketaddrConnect,
                &nSocketaddrLength);
    if(INVALID_SOCKET == aSocket)
    {
        Trace("\nFailed to call accept API!\n");
        err = closesocket(socketID);
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
        
    err = closesocket(aSocket);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call closesocket API!, error code=%d\n",
                GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!, error code=%d\n",
                GetLastError());
        }
        Fail("");
    }


    err = closesocket(socketID);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call closesocket API!, error code=%d\n",
                GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!, error code=%d\n",
                GetLastError());
        }
        Fail("");
    }

    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!, error code=%d\n",
                GetLastError());
    }

    PAL_Terminate();
    return PASS;
}
