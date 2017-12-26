/*=============================================================
**
** Source: socket_neg.c
**
** Purpose: Negatively test socket API to create a stream socket
**          which is not supported by UDP protocol.
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

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested, &WsaData);
    if (0 != err)
    {
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    /*Confirm that the WinSock DLL supports 2.2.*/
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

    /*create a stream socket in Internet domain with UDP protocol*/
    SocketID = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);

    /* Some platforms (e.g. AIX) may return WSAEPROTOTYPE instead of WSAEPROTONOSUPPORT.
       According to MSDN description of socket errors, WSAEPROTOTYPE is also an appropriate
       error under these circumstances, if not even a better one.
    */
    if ((WSAEPROTONOSUPPORT != GetLastError() && WSAEPROTOTYPE != GetLastError()) || INVALID_SOCKET != SocketID)
    {
        Trace("\nFailed to call socket API for a negative test, "
            "create stream socket with UDP protocol!\n");
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
    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!\n");
    }

    PAL_Terminate();
    return PASS;
}
