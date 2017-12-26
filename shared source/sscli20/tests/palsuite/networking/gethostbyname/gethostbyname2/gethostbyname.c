/*=============================================================
**
** Source: gethostbyname.c
**
** Purpose: Positive test gethostbyname API against a verified 
**          host and dns server. 
**          Test to insure a call to gethostbyname with NULL as 
**          the argument, returns the same string as a successful 
**          gethostname function call
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
    char HostName[_MAX_FNAME];
    char HostName_2[_MAX_FNAME];
    struct hostent *pMyHostent;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested, &WsaData);
    if ( err != 0 )
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

    memset(HostName_2, 0, _MAX_FNAME);    

    /*retrieve the local host name*/
    err = gethostname(HostName_2, _MAX_FNAME-1);
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

    memset(HostName, 0, _MAX_FNAME);    

    /*retrieve the host info by host name*/
    pMyHostent = gethostbyname(NULL);
    if(NULL == pMyHostent)
    {
        Trace("\nFailed to call gethostbyname API! LastError set to %d\n", 
              GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    strcpy(HostName, pMyHostent->h_name);
    
    Trace("\nBefore trimming domain name: HostName:%s HostName_2:%s\n", 
          HostName, HostName_2);

    /*trim the domain part*/
    strtok(HostName, ".");
    strtok(HostName_2, ".");
    
    /*further compare the retrieved host name*/
    if(strcmp(HostName, HostName_2))
    {
        Trace("\nFailed to call gethostbyname API, "
            "the retrieved host name is not valid\n");

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
    return 0;
}
