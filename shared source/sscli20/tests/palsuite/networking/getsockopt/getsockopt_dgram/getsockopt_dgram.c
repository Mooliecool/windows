/*=============================================================
**
** Source: getsockopt_dgram.c
**
** Purpose: Positive test the getsockopt function in SOL_SOCKET
**          and IPPROTO_TCP level to retrive datagram socket option(s) 
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
    int SocketID;
    int size;
    BOOL FailFlag = FALSE;
    int nValue;
    int nValue_set;
    int nSize;
    char errBuffer[1024];

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
            Trace("\nFailed to call WSACleanup API, error code=%d!\n",
                    GetLastError());
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
            Trace("\nFailed to call WSACleanup API, error code=%d!\n",
                    GetLastError());
        }
        Fail("");
    }

    /*create a datagram socket in AF_INET domain*/
    SocketID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(INVALID_SOCKET == SocketID)
    {
        Trace("\nFailed to create a datagram socket, error code=%d!\n",
                GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*initialize the error info buffer*/
    memset(errBuffer, 0, 1024);

    size = sizeof(BOOL);
    nSize = sizeof(int);

    /*retrieve datagram socket options*/


    /*test the option SO_SNDBUF*/
    /*the default value of SO_SNDBUF is 8192*/  
    nValue = 0;   /*hold the retrieved value*/
    nValue_set = 1024;/*set the new value*/
    err = setsockopt(SocketID, SOL_SOCKET, SO_SNDBUF, 
                (char *)&nValue_set, nSize);
    if(0 != err)
    {
        strcat(errBuffer, "SO_SNDBUF, ");
        FailFlag = TRUE;
    }
    else
    {
        err = getsockopt(SocketID, SOL_SOCKET, SO_SNDBUF,
                (char *)&nValue, &nSize);
        if(0 != err || nValue != nValue_set)
    {
            strcat(errBuffer, "SO_SNDBUF, ");
            FailFlag = TRUE;
    }
    }

    
    /*test the option SO_RCVBUF*/
    /*the default value of SO_RCVBUF is 8192*/  
    nValue = 0;         /*hold the retrieved value*/
    nValue_set = 1024;  /*set the new value*/
    err = setsockopt(SocketID, SOL_SOCKET, SO_RCVBUF, 
                (char *)&nValue_set, nSize);
    if(0 != err)
    {
        strcat(errBuffer, "SO_RCVBUF, ");
        FailFlag = TRUE;
    }
    else
    {
        err = getsockopt(SocketID, SOL_SOCKET, SO_RCVBUF,
                (char *)&nValue, &nSize);
        if(0 != err || nValue != nValue_set)
    {
            strcat(errBuffer, "SO_RCVBUF, ");
            FailFlag = TRUE;
        }
    }

    /*test the option SO_RCVTIMEO*/
    /*the default value of SO_RCVTIMEO is 0*/  
    nValue = 0;   /*hold the retrieved value*/
    nValue_set = 10;/*set the new value*/

    err = setsockopt(SocketID, SOL_SOCKET, SO_RCVTIMEO, 
                (char *)&nValue_set, nSize);
    if(0 != err)
    {
        strcat(errBuffer, "SO_RCVTIMEO, ");
        FailFlag = TRUE;
    }
    else
    {
        err = getsockopt(SocketID, SOL_SOCKET, SO_RCVTIMEO,
                (char *)&nValue, &nSize);        
        if(0 != err || nValue != nValue_set)
        {
            strcat(errBuffer, "SO_RCVTIMEO, ");
            FailFlag = TRUE;
    }

    }


    /*test the option SO_SNDTIMEO*/
    /*the default value of SO_SNDTIMEO is 0*/  
    nValue = 0;   /*hold the retrieved value*/
    nValue_set = 10;/*set the new value*/  
    err = setsockopt(SocketID, SOL_SOCKET, SO_SNDTIMEO, 
                (char *)&nValue_set, nSize);
    if(0 != err)
    {
        strcat(errBuffer, "SO_SNDTIMEO, ");
        FailFlag = TRUE;
    }
    else
    {

        err = getsockopt(SocketID, SOL_SOCKET, SO_SNDTIMEO,
                (char *)&nValue, &nSize);
        if(0 != err || nValue != nValue_set)
    {
            strcat(errBuffer, "SO_SNDTIMEO, ");
            FailFlag = TRUE;
    }

    }


    err = closesocket(SocketID);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call closesocket API!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*terminate use of WinSock DLL*/
    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API, error code=%d!\n",
            GetLastError());
    }

    if(FailFlag)
    {
        Fail("\nFailed to call getsockopt to get datagram socket "
                "option(s): %s \n", errBuffer);
    }

    PAL_Terminate();
    return PASS;
}
