/*=============================================================
**
** Source: socket_stream.c
**
** Purpose: Positively test socket API to create a stream socket.
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
    int SocketErr;
    int SocketID;
    int err;

	/*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll*/
    SocketErr = WSAStartup(VersionRequested, &WsaData);
    if(0 != SocketErr)
    {
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    /*Confirm that the WinSock DLL supports 2.2.*/
    if(LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2)
    {
        Trace("\nFailed to find a usable WinSock DLL!\n");
        SocketErr = WSACleanup();
        if(SOCKET_ERROR == SocketErr)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*create a stream socket in Internet domain*/
    SocketID = socket(AF_INET, SOCK_STREAM, 0);
    if(INVALID_SOCKET == SocketID)
    {
        Trace("\nFailed to call socket API to create a stream socket!\n");
        SocketErr = WSACleanup();
        if(SOCKET_ERROR == SocketErr)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

	SocketErr = closesocket(SocketID);
    if(SOCKET_ERROR == SocketErr)
    {
        Trace("\nFailed to call closesocket API!\n");
        SocketErr = WSACleanup();
        if(SOCKET_ERROR == SocketErr)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*Terminate use of WinSock DLL*/
    SocketErr = WSACleanup();
    if(SOCKET_ERROR == SocketErr)
    {
        Fail("\nFailed to call WSACleanup API!\n");
    }

    PAL_Terminate();
    return PASS;
}
