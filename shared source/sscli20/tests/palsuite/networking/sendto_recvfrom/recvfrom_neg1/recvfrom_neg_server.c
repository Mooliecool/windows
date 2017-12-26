/*=============================================================
**
** Source: recvfrom_neg_server.c
**
** Purpose: Negatively test the recvfrom API.
**          This is a server routine, this server routine must be
**          started before the related client is started.
**          This test case is to test recvfrom before calling
**          WSAStartup or after calling WSACleanup.
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
    WORD VersionRequested = MAKEWORD(2, 2);
    WSADATA WsaData;
    int err;
    int socketID = 0;
    struct sockaddr_in mySockaddr;
    int nLength;
    struct timeval waitTime;
    fd_set readFds;
    int socketFds;
    char data[BUFFERSIZE];

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    memset(data, 0, BUFFERSIZE);

    /*receieve data from this socket before calling WSAStartup*/
    err = recvfrom(socketID, data, BUFFERSIZE, 0,
        (struct sockaddr*)&mySockaddr, &nLength);

    if(WSANOTINITIALISED != GetLastError() || SOCKET_ERROR != err)
    {
        Fail("\nFailed to call recvfrom API for a negative test "
            "call recvfrom before calling WSAStrtup, an error is expected, "
            "but no error or no expected error is detected, "
            "error code = %u\n", GetLastError());
    }

    /*initialize to use winsock2 .dll*/
    err = WSAStartup(VersionRequested, &WsaData);
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

    /*create a datagram socket in AF_INET domain*/
    socketID = socket(AF_INET, SOCK_DGRAM, 0);

    if(INVALID_SOCKET == socketID)
    {
        Trace("\nFailed to call socket API to create a datagram socket!\n");
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
   
    err = bind(socketID, (struct sockaddr *)&mySockaddr,
            sizeof(struct sockaddr));
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call bind API to bind a socket with "
                "local address!\n");
        err = closesocket(socketID);
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

    /*add socket to readable socket set*/
    FD_SET(socketID,&readFds);
    /*mornitor the readable socket set*/
    socketFds = select(0, &readFds, NULL, NULL, &waitTime);

    if(SOCKET_ERROR == socketFds)
    {
        Trace("\nFailed to call select API to monitor readable socket set!\n");
        err = closesocket(socketID);
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
    if(0 == socketFds)
    {
        Trace("\nSelect waiting time is out!\n");
        err = closesocket(socketID);
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


    /*terminate use of WinSock DLL*/
    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!\n");
    }

    /*start server and wait for request from client*/
    nLength = sizeof(struct sockaddr);
    
    memset(data, 0, BUFFERSIZE);

    /*receieve data from this socket after calling WSACleanup*/
    err = recvfrom(socketID, data, BUFFERSIZE, 0,
        (struct sockaddr*)&mySockaddr, &nLength);

    if(WSANOTINITIALISED != GetLastError() || SOCKET_ERROR != err)
    {
        Fail("\nFailed to call recvfrom API for a negative test "
            "call recvfrom after calling WSACleanup, an error is expected, "
            "but no error or no expected error is detected, "
            "error code = %u\n", GetLastError());
    }

    PAL_Terminate();
    return PASS;
}
