/*=============================================================
**
** Source: gethostname.c
**
** Purpose: Positive test gethostname API to retrieve local host name 
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
    char HostName[_MAX_FNAME];
    BOOL errorFlag = FALSE;
    int i = 0;
    int strlength = 0;
    char ch;

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
  
    memset(HostName, 0, _MAX_FNAME);    

    /*retrieve the local host name*/
    err = gethostname(HostName, _MAX_FNAME);
    if(SOCKET_ERROR == err)
    {    
        Trace("\nFailed to call gethostname API, "
                "error code=%u\n", GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API, "
                "error code=%u\n", GetLastError());
        }
        Fail("");
    }

    /*
    *further check the host name contains legal character
    *with alphanumeric, dot, underscore and hyphan characters
    */ 
    strlength = (int)strlen(HostName); 

    for(i = 0; i < strlength; i++)
    {
        ch = HostName[i];
        if(!(ch == '.' || ch == '-' || (ch <= 'z' && ch >= 'a') ||
            (ch <= 'Z' && ch >= 'A') || (ch <='9' && ch >= '0') ||
            (ch == '_')))
        {
            errorFlag = TRUE;
        }
    }

    if(errorFlag)
    {
        Trace("\nFailed to call gethostname API to retrieve local host "
            "info, the host name contains invalid character(s), "
                "the host name is = %s\n", HostName);
               
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
        Fail("\nFailed to call WSACleanup API, "
                "error code=%u\n", GetLastError());
    }

    PAL_Terminate();
    return PASS;
}
