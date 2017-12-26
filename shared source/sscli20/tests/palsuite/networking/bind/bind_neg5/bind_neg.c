/*=============================================================
**
** Source: bind_neg.c
**
** Purpose: Negatively test the bind function to bind an address 
**          with a created stream socket. Bind the same socket
**          twice. 
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
    int socketID2;
    struct sockaddr_in mySockaddr;

    //Initialize the PAL environment
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    //initialize to use winsock2.dll
    err = WSAStartup(VersionRequested,&WsaData);
    if ( err != 0 )
    {
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    // Confirm that the WinSock DLL supports 2.2.
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

 
    //here the AF_INET is the same as PF_INET
    //create a stream socket in AF_INET domain
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

    //here the AF_INET is the same as PF_INET
    //create another stream socket
    socketID2 = socket(AF_INET,SOCK_STREAM,0);

    if(INVALID_SOCKET == socketID2)
    {
        Trace("\nFailed to call socket API to create a stream socket!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Fail("\nFailed to call WSACleanup API!\n");
       }
        Fail("");
    }
    
    //prepare the sockaddr structure
    mySockaddr.sin_family = AF_INET;
    mySockaddr.sin_port = 0;
    mySockaddr.sin_addr.S_un.S_addr = INADDR_ANY;
    memset( &(mySockaddr.sin_zero), 0, 8);

    //bind address with a stream socket
    err = bind(socketID,(struct sockaddr *)&mySockaddr,sizeof(struct sockaddr));
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call bind API to bind socket with an address!\n");
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


    //bind the same socket to a address
    err = bind(socketID,(struct sockaddr *)&mySockaddr,sizeof(struct sockaddr));
    if(WSAEINVAL != GetLastError() || SOCKET_ERROR != err)
    {
        Trace("\nFailed to call bind API for a negative test "
                "try to bind an already bound socket!\n");
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
