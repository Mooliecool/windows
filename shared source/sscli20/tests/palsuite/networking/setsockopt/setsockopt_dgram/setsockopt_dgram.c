/*=============================================================
**
** Source: setsockopt_dgram.c
**
** Purpose: Positive test the setsockopt function in SOL_SOCKET
**          level to set datagram socket option(s) 
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
    WORD VersionRequested = MAKEWORD(2, 2);
    WSADATA WsaData;
    int err;
    int SocketID;
    int size;
    BOOL FailFlag = FALSE;
    BOOL bValue;
    BOOL bValue_get;
    int nValue = 0;
    int nValue_get;
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
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    /*Confirm that the WinSock DLL supports 2.2.*/
    if(LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2)
    {
        Trace("\nFailed to find a usable WinSock DLL!, error code=%d\n",
                    GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API, error code=%d!\n",
                    GetLastError());
        }
        Fail("");
    }

    /*create a datagram socket in AF_INET domain*/
    SocketID = socket(AF_INET, SOCK_DGRAM, 0);
    if(INVALID_SOCKET == SocketID)
    {
        Trace("\nFailed to create the stream socket, error code=%d!\n",
                GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    memset(errBuffer, 0, 1024);

    size = sizeof(BOOL);
    nSize = sizeof(int);

    /*test setsockopt to set datagram socket options*/
    bValue = TRUE;
    bValue_get = FALSE;
    /*test the option SO_BROADCASE */
    /*the default of SO_BROADCAST is FALSE*/
    err = setsockopt(SocketID, SOL_SOCKET, SO_BROADCAST, 
                (char *)&bValue, size);
    if(0 != err)
    {
        strcat(errBuffer, "SO_BROADCAST, ");
        FailFlag = TRUE;
    }

    /*test SO_RCVBUF option*/
    /*default of SO_RCVBUF is 8192 in Win32*/
    nValue = 1024; /*set the new receive buffer*/
    nValue_get = 0; 
    err = setsockopt(SocketID, SOL_SOCKET, SO_RCVBUF, 
                (char *)&nValue, nSize);
    if(0 != err)
    {
        strcat(errBuffer, "SO_RCVBUF, ");
        FailFlag = TRUE;
    }
    else
    {
        /*retrieve this datagram socket option*/
        err = getsockopt(SocketID, SOL_SOCKET, SO_RCVBUF, 
                    (char *)&nValue_get, &nSize);
        if(0 != err)
        {
            Trace("\nFail to get the socket option: SO_RCVBUF\n");
            strcat(errBuffer, "SO_RCVBUF, ");
            FailFlag = TRUE;
        }
        else
        {
            /*check if the retrieved socket option is the same as 
            *the one set by setsockopt.
            */
            if(nValue != nValue_get)
            {
                strcat(errBuffer, "SO_RCVBUF, ");
                FailFlag = TRUE;
            }
        }
    }

    /*test SO_SNDBUF option*/
    /*default of SO_SNDBUF is 8192 in Win32*/
    nValue = 1024; /*set the new send buffer*/
    nValue_get = 0;
    err = setsockopt(SocketID, SOL_SOCKET, SO_SNDBUF, 
                (char *)&nValue, nSize);
    if(0 != err)
    {
        strcat(errBuffer, "SO_SNDBUF, ");
        FailFlag = TRUE;
    }
    else
    {
        /*retrieve this datagram socket option*/
        err = getsockopt(SocketID, SOL_SOCKET, SO_SNDBUF, 
                    (char *)&nValue_get, &nSize);
        if(0 != err)
        {
            Trace("\nFail to get the socket option: SO_SNDBUF\n");
            strcat(errBuffer, "SO_SNDBUF, ");
            FailFlag = TRUE;
        }
        else
        {
            /*check if the retrieved socket option is the same as 
            *the one set by setsockopt.
            */
            if(nValue != nValue_get)
            {
                strcat(errBuffer, "SO_SNDBUF, ");
                FailFlag = TRUE;
            }
        }
    }

    /*test the option SO_RCVTIMEO*/
    /*the default value of SO_RCVTIMEO is 0*/  
    nValue = 10;   /*set the new value*/
    nValue_get = 0;/*hold the retrieved value*/
    
    err = setsockopt(SocketID, SOL_SOCKET, SO_RCVTIMEO, 
                (char *)&nValue, nSize);
    if(0 != err)
    {
        strcat(errBuffer, "SO_RCVTIMEO, ");
        FailFlag = TRUE;
    }
    else
    {
    
        err = getsockopt(SocketID, SOL_SOCKET, SO_RCVTIMEO,
                (char *)&nValue_get, &nSize);
        if(0 != err || nValue != nValue_get)
        {
            strcat(errBuffer, "SO_RCVTIMEO, ");
            FailFlag = TRUE;
        }
        
    }

    /*test the option SO_SNDTIMEO*/
    /*the default value of SO_SNDTIMEO is 0*/  
    nValue = 10;   /*set the new value*/
    nValue_get = 0; /*hold the retrieved value*/ 
    err = setsockopt(SocketID, SOL_SOCKET, SO_SNDTIMEO, 
                (char *)&nValue, nSize);
    if(0 != err)
    {
        strcat(errBuffer, "SO_SNDTIMEO, ");
        FailFlag = TRUE;
    }
    else
    {
    
        err = getsockopt(SocketID, SOL_SOCKET, SO_SNDTIMEO,
                (char *)&nValue_get, &nSize);
        if(0 != err || nValue != nValue_get)
        {
            strcat(errBuffer, "SO_SNDTIMEO, ");
            FailFlag = TRUE;
        }
        
    }

    err = closesocket(SocketID);
    if(SOCKET_ERROR == err)
    {
        if(FailFlag)
        {
            Trace("\nFailed to call setsockopt to set datagram socket "
                "option(s): %s \n", errBuffer);
        }
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
        if(FailFlag)
        {
            Trace("\nFailed to call setsockopt to set datagram socket "
                "option(s): %s \n", errBuffer);
        }

        Fail("\nFailed to call WSACleanup API, error code=%d!\n",
            GetLastError());
    }

    if(FailFlag)
    {
        Fail("\nFailed to call setsockopt to set datagram socket "
                "option(s): %s \n", errBuffer);
    }

    PAL_Terminate();
    return PASS;
}
