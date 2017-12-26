/*============================================================================
**
** Source:  wsaenumnetworkevents_neg.c
**
** Purpose: Negative test the wsaenumnetworkevents API. 
**          Call wsaenumnetworkevents API before 
**          calling WSAStartup or after calling WSACleanup
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
**==========================================================================*/
#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{
    WORD VersionRequested = MAKEWORD(2,2);
    WSADATA WsaData;
    int err;
    int socketID = INVALID_SOCKET;
    HANDLE myEvent = NULL;
    WSANETWORKEVENTS myEventStruct;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*call WSAEnumNetworkEvents before calling WSAStartup*/
    err = WSAEnumNetworkEvents(socketID, (WSAEVENT)myEvent, &myEventStruct);
    if(WSANOTINITIALISED != GetLastError() || SOCKET_ERROR != err)
    {
        Fail("\nFailed to call WSAEnumNetworkEvents API for a negative test, "
            "call WSAEnumNetworkEvents API before calling WSAStartup, "
            "an error WSANOTINITIALISED is expected, but no error or no  "
            "expected error is detected, error code = %u\n", GetLastError());
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

    /*terminate the use of Winsock2*/
    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!\n");
    }

     /*call WSAEnumNetworkEvents after calling WSACleanup*/
    err = WSAEnumNetworkEvents(socketID, (WSAEVENT)myEvent, &myEventStruct);
    if(WSANOTINITIALISED != GetLastError() || SOCKET_ERROR != err)
    {
        Fail("\nFailed to call WSAEnumNetworkEvents API for a negative test, "
            "call WSAEnumNetworkEvents API after calling WSACleanup, "
            "an error WSANOTINITIALISED is expected, but no error or no  "
            "expected error is detected, error code = %u\n", GetLastError());
    }

    PAL_Terminate();
    return PASS;
}

