/*=============================================================
**
** Source: getsockopt_neg.c
**
** Purpose: Negative test the getsockopt function in SOL_SOCKET 
**          level to retrieve stream socket option(s).
**          Call getsockopt by passing a NULL buffer
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
    int SocketID;
    int size;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested,&WsaData);
    if ( err != 0 )
    {
        Fail("\nFailed to find a usable WinSock DLL, error code=%d!\n",
                GetLastError());
    }

    /*Confirm that the WinSock DLL supports 2.2.*/
    if(LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2)
    {
        Trace("\nFailed to find a usable WinSock DLL, error code=%d!\n",
                GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API, error code=%d!\n",
                GetLastError());
        }
        Fail("");
    }

    /*create a stream socket in AF_INET domain*/
    SocketID = socket(AF_INET, SOCK_STREAM, 0);
    if(INVALID_SOCKET == SocketID)
    {
        Trace("\nFailed to create the stream socket, error code=%d!\n",
                GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API, error code=%d!\n",
                GetLastError());
        }
        Fail("");
    }

    size = sizeof(BOOL);

    /*retrieve socket option by passing a NULL buffer*/
    err = getsockopt(SocketID, SOL_SOCKET, SO_LINGER, 
                (char *)NULL, &size);

    if(WSAEFAULT != GetLastError() || SOCKET_ERROR != err)
    {    
        Trace("\nFailed to call getsockopt API for a negative test "
            "call getsockopt by passing a NULL buffer, an error "
            "WSAEFAULT is expected, but no error or no expected error "
            "is detected, error code =%d!\n", GetLastError());

        err = closesocket(SocketID);
        if(SOCKET_ERROR == err)
        {    
            Trace("\nFailed to call closesocket API, error code=%d!\n",
                GetLastError());
        }

        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API, error code=%d!\n",
                GetLastError());
        }
        Fail("");
    }

    err = closesocket(SocketID);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call closesocket API, error code=%d!\n",
                GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API, error code=%d!\n",
                GetLastError());
        }
        Fail("");
    }

    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API, error code=%d!\n",
                GetLastError());
    }

    PAL_Terminate();
    return PASS;
}


