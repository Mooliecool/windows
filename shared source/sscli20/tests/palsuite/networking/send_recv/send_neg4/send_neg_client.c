/*=============================================================
**
** Source: send_neg_client.c
**
** Purpose: Negatively test the send and recv API.
**            This is a client routine. This client routine must 
**            be started after the related server is started.
**          This test case is to test send with an invalid
**            socket descriptor.
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
    /*set an invalid socket descriptor*/
    int InvalidSocketID = -1;
    /*defin a non-zero length data*/
    const char *data = "None-zero length data test";
    int nBuffer;


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
        Trace("\nFailed to find a usable WinSock DLL!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    nBuffer = strlen(data);

    /*call send by passing an invalid socket descriptor*/
    err = send(InvalidSocketID,data,nBuffer,0);
    
    if(WSAENOTSOCK != GetLastError() || SOCKET_ERROR != err)
    {
        Trace("\nFailed to call send API for a negative test "
            "by passing invalid socket!\n");
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
