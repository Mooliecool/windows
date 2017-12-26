/*=============================================================
**
** Source: getsockname_neg.c
**
** Purpose: Negative test the getsockname function to retrieve 
**          the local name for an unbound stream socket  
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

    int err;
    int socketID;
    struct sockaddr OutSockaddr;
    int size;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested, &WsaData);
    if(err != 0 )
    {
        Trace("\nFailed to find a usable WinSock DLL!, error code=%d\n",
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

    /*create a stream socket in AF_INET domain*/
    socketID = socket(AF_INET, SOCK_STREAM, 0);
    if(INVALID_SOCKET == socketID)
    {
        Trace("\nFailed to call socket API to create a "
                "stream socket!, error code=%d\n", GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }


    /*no bind operation for this socket*/
    size = sizeof(struct sockaddr);

    /*retrieve the local name for an unbound socket*/
    err = getsockname(socketID, &OutSockaddr, &size);

    if(WSAEINVAL != GetLastError() || SOCKET_ERROR != err)
    {
        Trace("\nFailed to call getsockname API for a negative test "
            "by passing an unbound socket!, error code=%d\n",
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
        Fail("\nFailed to call WSACleanup API!\n");
    }

    PAL_Terminate();
    return PASS;
}
