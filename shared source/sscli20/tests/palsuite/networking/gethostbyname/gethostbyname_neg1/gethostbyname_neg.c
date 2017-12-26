/*=============================================================
**
** Source: gethostbyname_neg.c
**
** Purpose: Negatively test gethostbyname API against a verified 
**            host and dns server. Call gethostbyname before 
**            calling WSAStartup() or after calling WSACleanup().
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
    struct hostent *MyHostent;
    const char *pHostName = NULL;
    int err;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*retrieve the host name from external file*/
    pHostName = getRotorTestHost();
    if(NULL == pHostName )
    {
        Fail("\nFailed to read server name!\n");
    }
    if(strlen(pHostName) == 0)
    {
        Fail("\nFailed to read server name, an empty host name!\n");
    }

    /*retrieve the host info before calling WSAStartup()*/
    MyHostent = gethostbyname(pHostName);

    if(WSANOTINITIALISED != GetLastError() || NULL != MyHostent)
    {
        Fail("\nFailed to call gethostbyname API for a negative test, "
            " call this API before calling WSAStartup,  an error "
            "WSANOTINITIALISED is expected, but not error or expected "
            "error is detected, error code =%u\n", GetLastError());
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested,&WsaData);
    if ( err != 0 )
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

    /*terminat the use of WinSock DLL*/
    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!\n");
    }

    /*retrieve the host info after calling WSACleanup()*/
    MyHostent = gethostbyname(pHostName);
    

    if(WSANOTINITIALISED != GetLastError() || NULL != MyHostent)
    {
        Fail("\nFailed to call gethostbyname API for a negative test, "
            "call this API after calling WSACleanup, an error "
            "WSANOTINITIALISED is expected, but not error or expected "
            "error is detected, error code =%u\n", GetLastError());
    }

    PAL_Terminate();
    return PASS;
}

