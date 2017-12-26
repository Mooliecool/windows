/*=============================================================
**
** Source: gethostbyname_neg.c
**
** Purpose: Negative test gethostbyname API against a verified 
**          host and dns server by passing an inavlid host name
**          
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

    struct hostent *MyHostent;
    const char *pHostName = "Wrong host name";

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
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    /*Confirm that the WinSock DLL supports 2.2.*/ 
    if ( LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2 )
    {
        /*we could not find a usable WinSock DLL.*/
        Trace("\nFailed to find a usable WinSock DLL!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }


    /*retrieve the host info by passing an invalid host name*/
    MyHostent = gethostbyname(pHostName);
    if(!(WSAHOST_NOT_FOUND == GetLastError() ||
        WSATRY_AGAIN == GetLastError()) ||
        NULL != MyHostent)
    {
        Trace("\nFailed to call gethostbyname API for negative test "
            "call gethostbyname by passing an invalid host name, an "
            "error WSAHOST_NOT_FOUND or WSATRY_AGAIN(BSD) are expected, "
            "but no error or no expected error is detected, "
            "error code=%u\n", GetLastError());

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
