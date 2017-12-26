/*=============================================================
**
** Source: socket_neg.c
**
** Purpose: Negatively test socket API to create a stream socket
**          in Win32 environment. Call socket API before calling
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

    /*create a stream socket in AF_INET domain before calling WSAStartup*/
    SocketID = socket(AF_INET, SOCK_STREAM, 0);
    if(WSANOTINITIALISED != GetLastError() || INVALID_SOCKET != SocketID)
    {
        Fail("\nFailed to call socket API for a negative test "
                "Call this API before calling WSAStartup,"
                "an error is expected, but no error or no expected error "
                "is detected, error code=%u\n", GetLastError());
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested, &WsaData);
    if(err != 0)
    {
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    /* Confirm that the WinSock DLL supports 2.2.*/
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

    /*terminate the use of WinSock DLL*/
    /*and let shutdown generate error*/
    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!\n");
    }

    /*create a stream socket in AF_INET domain after calling WSACleanup*/
    SocketID = socket(AF_INET, SOCK_STREAM, 0);
    if(WSANOTINITIALISED != GetLastError() || INVALID_SOCKET != SocketID)
    {
        Fail("\nFailed to call socket API for a negative test "
                "Call this API after calling WSACleanup,"
                "an error is expected, but no error or no expected error "
                "is detected, error code=%u\n", GetLastError());
    }


    PAL_Terminate();
    return PASS;
}
