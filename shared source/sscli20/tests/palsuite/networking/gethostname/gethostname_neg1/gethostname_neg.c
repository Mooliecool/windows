/*=============================================================
**
** Source: gethostname_neg.c
**
** Purpose: Negative test gethostname API to retrieve local host name 
**          by passing a zero length buffer
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

#define HNAME_SIZE 255

int __cdecl main(int argc, char *argv[])
{
    WORD VersionRequested = MAKEWORD(2, 2);
    WSADATA WsaData;
    int err;
    char HostName[HNAME_SIZE];

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }
    /*initialize to use winsock2 .dll*/
    err = WSAStartup(VersionRequested, &WsaData);
    if(err != 0)
    {
        Fail("\nFailed to find a usable WinSock DLL, "
                "error code=%u\n", GetLastError());
    }

    /*Confirm that the WinSock DLL supports 2.2.*/
    if(LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2)
    {
        Trace("\nFailed to find a usable WinSock DLL, "
                "error code=%u\n", GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API, "
                "error code=%u\n", GetLastError());
        }
        Fail("");
    }

    memset(HostName, 0, HNAME_SIZE);
    /*
    retrive the local host name
    pass zero length of buffer to create an error
    */
    err = gethostname(HostName, 0);

    if(WSAEFAULT != GetLastError() || SOCKET_ERROR != err)
    {    
        Trace("\nFailed to call gethostname API for a negative test "
            "by passing zero length buffer!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API, "
                "error code=%u\n", GetLastError());
        }
        Fail("");
    }
    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API, "
                "error code=%u\n", GetLastError());
    }

    PAL_Terminate();
    return PASS;
}
