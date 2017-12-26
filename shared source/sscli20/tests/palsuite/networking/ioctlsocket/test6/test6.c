/*============================================================================
**
** Source:  test6.c
**
** Purpose: Negative test iostlsocket by passing an invalid socket descriptor
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

int __cdecl main( int argc, char **argv ) 
{
    u_long argp;
    WORD VersionRequested = MAKEWORD(2, 2);
    WSADATA WsaData;
    int err;
    int SocketID = INVALID_SOCKET;

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
        Trace("\nFailed to find a usable WinSock DLL!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
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
  
    argp = 1L;
    /*call ioctlcocket by passing an invalid socket descriptor*/
    err = ioctlsocket(SocketID, FIONBIO, &argp );
    if( SOCKET_ERROR != err || WSAENOTSOCK != GetLastError())
    {
        Trace("\nFailed to call ioctlsocket for a negative test. "
              "call ioctlsocket by passing an invalid socket; an error "
              "WSAENOTSOCK is expected; but no error or no expected error "
              "is detected; error code = %u\n", GetLastError());

        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API, error code =%u\n",
                GetLastError());
        }
        Fail("");
    }

    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API, error code=%u\n",
                GetLastError());
    }

    PAL_Terminate();
    return PASS; 
}
