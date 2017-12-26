/*=============================================================
**
** Source: getsockopt_stream.c
**
** Purpose: Positive test the getsockopt function in SOL_SOCKET 
**          and IPPROTO_TCP level to retrieve stream socket option(s) 
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
    BOOL bValue_set;
    int nValue;
    int nValue_set;
    int nSize;
    struct linger myLinger;
    struct linger myLinger_set;
    char errBuffer[1024];


    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);    
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested,&WsaData);
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

    /*create a stream socket in AF_INET domain*/
    SocketID = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == SocketID)
    {
        Trace("\nFailed to create the stream socket, error code =%d!\n",
                GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API, error code=%d!\n",
                GetLastError());
        }
        Fail("");
    }

    /*initialize the error buffer*/
    memset(errBuffer, 0, 1024);

    size = sizeof(BOOL);
    nSize = sizeof(int);

    
    /*test the option SO_RCVBUF*/
    nValue = 0;   /*hold the retrieved value*/
    nValue_set = 1024;/*set the new value*/
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
        if(0 != err || nValue < nValue_set)
        {
            strcat(errBuffer, "SO_RCVBUF, ");
            FailFlag = TRUE;
        }
    }

    /*test the option SO_SNDBUF*/
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
        if(0 != err || nValue < nValue_set)
        {
            strcat(errBuffer, "SO_SNDBUF, ");
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

    /*Test the linger option*/
    size = sizeof(struct linger);
    myLinger_set.l_onoff = 1;/*enable the linger*/
    myLinger_set.l_linger = 2;/*time out is 2 sec*/

    /*initialize the linger struct for getsockopt*/
    myLinger.l_onoff = 0;
    myLinger.l_linger = 0;
    /*enable the linger option*/
    err = setsockopt(SocketID, SOL_SOCKET, SO_LINGER, 
                (char *)&myLinger_set, size);
    if(0 != err)
    {
        strcat(errBuffer, "SO_LINGER, ");
        FailFlag = TRUE;
    }
    else
    {
        err = getsockopt(SocketID, SOL_SOCKET, SO_LINGER, 
                (char *)&myLinger, &size);
        if(0 != err || 0 == myLinger.l_onoff ||
                    myLinger_set.l_linger != myLinger.l_linger)
        {
            strcat(errBuffer, "SO_LINGER, ");
            FailFlag = TRUE;
        }
    }

    /*test the socket level IPPROTO_TCP*/
    /*test TCP_NODELAY option, the default is FALSE*/
    size = sizeof(BOOL);
    bValue = FALSE;     /*hold the retrieved value*/
    bValue_set = TRUE;  /*set the new value*/
    err = setsockopt(SocketID, IPPROTO_TCP, TCP_NODELAY, 
                (char *)&bValue_set, size);
    if(0 != err)
    {
        strcat(errBuffer, "TCP_NODELAY, ");
        FailFlag = TRUE;
    }
    else
    {
        err = getsockopt(SocketID, IPPROTO_TCP, TCP_NODELAY, 
                    (char *)&bValue, &size);
        if(0 != err || bValue != bValue_set)
        {
            strcat(errBuffer, "TCP_NODELAY, ");
            FailFlag = TRUE;
        }
    }

    err = closesocket(SocketID);
    if(SOCKET_ERROR == err)
    {    
        Trace("\nFailed to call closesocket API, error code =%d!\n",
                GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API, error code =%d!\n",
                GetLastError());
        }
        Fail("");
    }

    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API, error code=%d!\n",
                GetLastError());
    }


    if(FailFlag)
    {
        Fail("\nFailed to call getsockopt to set the socket option(s): %s \n",
                errBuffer);
    }

    PAL_Terminate();
    return PASS;
}
