/*=============================================================
**
** Source: setsockopt_stream.c
**
** Purpose: Positive test the setsockopt function in SOL_SOCKET
**          and IPPROTO_TCP level to set stream socket option(s) 
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
    int nSize;
    int nValue;
    int nValue_get;
    struct linger myLinger;
    struct linger myLinger_get;
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
        Trace("\nFailed to find a usable WinSock DLL, error code=%d!\n",
                GetLastError());
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
            Trace("\nFailed to call WSACleanup API!, error code=%d\n",
                GetLastError());
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


    /*test the SO_RCVBUF option*/
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
        /*retrieve this stream socket option*/
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
            if(nValue > nValue_get)
            {
                strcat(errBuffer, "SO_RCVBUF, ");
                FailFlag = TRUE;
            }
        }
    }


    /*test the SO_SNDBUF option*/
    nValue = 1024; /*set the new send buffer*/
    err = setsockopt(SocketID, SOL_SOCKET, SO_SNDBUF,
                (char *)&nValue, nSize);
    if(0 != err)
    {
        strcat(errBuffer, "SO_SNDBUF, ");
        FailFlag = TRUE;
    }
    else
    {
        /*retrieve this stream socket option*/
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
            if(nValue < nValue_get)
            {
                strcat(errBuffer, "SO_SNDBUF, ");
                FailFlag = TRUE;
            }
        }
    }

    /*test the socket level IPPROTO_TCP*/
    /*test TCP_NODELAY option*/
    bValue = TRUE;
    bValue_get = FALSE;
    err = setsockopt(SocketID, IPPROTO_TCP, TCP_NODELAY,
                (char *)&bValue, size);
    if(0 != err)
    {
        strcat(errBuffer, "TCP_NODELAY in TCP_NODELAY level , ");
        FailFlag = TRUE;
    }
    else
    {
        /*retrieve this stream socket option*/
        err = getsockopt(SocketID, IPPROTO_TCP, TCP_NODELAY,
                    (char *)&bValue_get, &size);
        if(0 != err)
        {
            Trace("\nFail to get the socket option: TCP_NODELAY\n");
            strcat(errBuffer, "TCP_NODELAY, ");
            FailFlag = TRUE;
        }
        else
        {
            /*check if the retrieved socket option is the same as
            *the one set by setsockopt.
            */
            if(bValue != bValue_get)
            {
                strcat(errBuffer, "TCP_NODELAY, ");
                FailFlag = TRUE;
            }
        }
    }


    /*Test the linger option*/
    size = sizeof(struct linger);
    myLinger.l_onoff = 1;/*enable the linger*/
    myLinger.l_linger = 2;/*time out is 2 sec*/

    /*initialize the linger struct for getsockopt*/
    myLinger_get.l_onoff = 0;
    myLinger_get.l_linger = 0;
    /*enable the linger option*/
    err = setsockopt(SocketID, SOL_SOCKET, SO_LINGER, 
                (char *)&myLinger, size);
    if(0 != err)
    {
        strcat(errBuffer, "SO_LINGER, ");
        FailFlag = TRUE;
    }
    else
    {
        /*retrieve this stream socket option*/
        err = getsockopt(SocketID, SOL_SOCKET, SO_LINGER, 
                    (char *)&myLinger_get, &size);
        if(0 != err)
        {
            Trace("\nFail to get the socket option: SO_LINGER\n");
            strcat(errBuffer, "SO_LINGER, ");
            FailFlag = TRUE;
        }
        else
        {
            /*check if the retrieved socket option is the same as 
             *the one set by setsockopt. NOTE: l_onoff has only to
             *be non-zero to be set, it is NOT guaranteed to be the same as what
             *we set it, and in fact, it ISN'T in BSD.
             */
            if(myLinger.l_onoff ? !myLinger_get.l_onoff : myLinger_get.l_onoff
 || 
                myLinger.l_linger != myLinger_get.l_linger)
            {
                strcat(errBuffer, "SO_LINGER, ");
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
        Trace("\nFailed to call closesocket API, error code =%d!\n",
                GetLastError());

        if(FailFlag)
        {
            Trace("\nFailed to call setsockopt to set the socket option(s): "
                "%s \n", errBuffer);
        }

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
        Trace("\nFailed to call WSACleanup API, error code=%d!\n",
                GetLastError());
        if(FailFlag)
        {
            Trace("\nFailed to call setsockopt to set the socket option(s): "
                "%s \n", errBuffer);
        }
        Fail("");
    }

    if(FailFlag)
    {
        Fail("\nFailed to call setsockopt to set the socket option(s): %s \n",
                errBuffer);
    }

    PAL_Terminate();
    return PASS;
}
