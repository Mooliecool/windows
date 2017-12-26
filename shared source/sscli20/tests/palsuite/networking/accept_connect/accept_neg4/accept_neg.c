/*=============================================================
**
** Source: accept_neg.c
**
** Purpose: Negatively test the accept API by passing an 
**          invalid socket.
**          This is a server routine. This server routine must
**          be started before the related client if started.
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
    int InvalidSocketID = -1;/*set an invalid socket descriptor*/
    struct sockaddr mySocketaddrConnect;
    int nSocketaddrLength = sizeof(mySocketaddrConnect);

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll*/
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

    /*accept an request from client by passing an invalid socket descriptor*/
    aSocket = accept(InvalidSocketID, (struct sockaddr*)&mySocketaddrConnect, 
                &nSocketaddrLength);

    if(WSAENOTSOCK != GetLastError() || INVALID_SOCKET != aSocket)
    {
        Trace("\nFailed to call accept API for a negative test, "
            "call accept by passing an invalid socket descriptor, "
            "an error WSAENOTSOCK is expected, but no error or no "
            "expected error is detected, error code=%u\n", GetLastError());

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
