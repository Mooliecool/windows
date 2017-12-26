/*=============================================================
**
** Source: gethostbyaddr_neg.c
**
** Purpose: Negative test gethostbyaddr API to retrieve the 
**          host info to which a stream socket is connected.
**          Call gethostbyaddr before calling WSAStartup() or
**          after calling WSACleanup().
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
    struct hostent *host;
    u_long addr;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*retrieve host info by its IP address before calling WSAStartup*/
    host = gethostbyaddr ((const char *)&addr, sizeof(addr), AF_INET);
 
    if(WSANOTINITIALISED != GetLastError() || NULL != host)
    {
        Fail("\nFailed to call gethostbynaddr API for negative test, "
                "Call gethostbyaddr before calling WSAStartup, "
                "an error is expected, but no error or no expected error "
                "returns, error code=%u\n", GetLastError());
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested, &WsaData);

    if(err != 0)
    {
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    /*Confirm that the WinSock DLL supports 2.2.*/
    if(LOBYTE( WsaData.wVersion ) != 2 ||
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

    addr = inet_addr("127.0.0.1");

    /*terminat the use of WinSock DLL*/
    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!\n");
    }

    /*retrieve host info by its IP address after calling WSACleanup*/
    host = gethostbyaddr ((const char *)&addr, sizeof(addr), AF_INET);
    if(WSANOTINITIALISED != GetLastError() || NULL != host)
    {
        Fail("\nFailed to call gethostbynaddr API for negative test, "
                "Call gethostbyaddr after calling WSACleanup, "
                "an error is expected, but no error or no expected error "
                "returns, error code=%u\n", GetLastError());
    }

    PAL_Terminate();
    return PASS;
}
