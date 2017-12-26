/*=============================================================
**
** Source: bind_neg.c
**
** Purpose: Negative test the bind API to bind address with 
**          a created stream socket. Call bind API before 
**          calling WSAStartup or after calling WSACleanup
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
    int socketID = 0;
    struct sockaddr_in mySockaddr;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*bind address with a stream socket before calling WSAStartup*/
    err = bind(socketID,(struct sockaddr *)&mySockaddr,sizeof(struct sockaddr));

    if(WSANOTINITIALISED != GetLastError() || SOCKET_ERROR != err)
    {
        Fail("\nFailed to call bind API for a negative test, "
            "call bind API before calling WSAStartup, "
            "error code=%u\n", GetLastError());
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested,&WsaData);
    if ( err != 0 )
    {
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    /* Confirm that the WinSock DLL supports 2.2.*/
    if ( LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2 )
    {
        Trace("\nFailed to find a usable WinSock DLL!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }
 
    /*create a stream socket*/
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

    /*prepare the sockaddr_in structure*/
    mySockaddr.sin_family = AF_INET;
    mySockaddr.sin_port = 0;
    mySockaddr.sin_addr.S_un.S_addr = INADDR_ANY;
    memset( &(mySockaddr.sin_zero), 0, 8);

    /*terminate to user WinSock DLL and let bind generate error*/
    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!\n");
    }

    /*bind address with a stream socket after calling WSACleanup*/
    err = bind(socketID,(struct sockaddr *)&mySockaddr,sizeof(struct sockaddr));

    if(WSANOTINITIALISED != GetLastError() || SOCKET_ERROR != err)
    {
        Fail("\nFailed to call bind API for a negative test, "
            "call bind API after calling WSACleanup, "
            "error code=%u\n", GetLastError());
    }

    PAL_Terminate();
    return PASS;
}
