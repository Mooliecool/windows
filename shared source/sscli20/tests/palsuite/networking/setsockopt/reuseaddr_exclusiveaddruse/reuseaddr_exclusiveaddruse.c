/*=============================================================
**
** Source: reuseaddr_exclusiveaddruse.c
**
** Purpose: Test to make sure that setting and resetting 
**          SO_REUSEADDR and SO_EXCLUSIVEADDRUSE socket 
**          options work properly, and that their behavior 
**          is consistent with Windows specs, i.e. for the 
**          pair {SO_REUSEADDR, SO_EXCLUSIVEADDRUSE} legal
**          values are {0,0}, {0,1} and {1,0}.
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
    int s;
    int optsize, optval = 0;
    int i;


    err = PAL_Initialize(argc, argv);    
    if(0 != err)
    {
        return FAIL;
    }

    err = WSAStartup(VersionRequested, &WsaData);
    if(err != 0)
    {
        Trace("\nFailed to find a usable WinSock DLL, error code=%d!\n",
              GetLastError());
        goto done;
    }

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == s)
    {
        Trace("\nFailed to create the stream socket, error code =%d!\n",
              GetLastError());
        goto done;
    }    
    optsize = sizeof(int);
    err = getsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, &optsize);
    if(0 != err)
    {
        Trace("\nFailed to get SO_REUSEADDR option error code =%d!\n",
              GetLastError());
        goto done;
    }
    if (optval != 0)
    {
        Trace("\nSO_REUSEADDR should not be set on a new created socket [optval=%d]\n", optval);
        err = -1;
        goto done;
    }

    optsize = sizeof(int);
    err = getsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&optval, &optsize);
    if(0 != err)
    {
        Trace("\nFailed to get SO_EXCLUSIVEADDRUSE option error code =%d!\n",
              GetLastError());
        goto done;
    }
    if (optval != 0)
    {
        Trace("\nSO_EXCLUSIVEADDRUSE should not be set on a new created socket\n");
        err = -1;
        goto done;
    }

    for (i=0;i<10;i++)
    {    
        optval = i%2;
        err = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(int));
        if(0 != err)
        {
            Trace("\nFailed to set SO_REUSEADDR=%d option error code =%d!\n",
                  optval, GetLastError());
            goto done;
        }
        optsize = sizeof(int);
        err = getsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, &optsize);
        if(0 != err)
        {
            Trace("\nFailed to get SO_REUSEADDR option error code =%d!\n",
                  GetLastError());
            goto done;
        }
        if (optval != i%2)
        {
            Trace("\nSO_REUSEADDR should be now %s [optval=%d]\n", i%2, optval);
            err = -1;
            goto done;
        }
    }

    /* reset SO_REUSEADDR */
    optval = 0;
    err = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(int));
    if(0 != err)
    {
        Trace("\nFailed to set SO_REUSEADDR=%d option error code =%d!\n",
              optval, GetLastError());
        goto done;
    }

    for (i=0;i<10;i++)
    {    
        optval = i%2;
        err = setsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&optval, sizeof(int));
        if(0 != err)
        {
            Trace("\nFailed to set SO_EXCLUSIVEADDRUSE=%d option error code =%d!\n",
                  optval, GetLastError());
            goto done;
        }
        optsize = sizeof(int);
        err = getsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&optval, &optsize);
        if(0 != err)
        {
            Trace("\nFailed to get SO_EXCLUSIVEADDRUSE option error code =%d!\n",
                  GetLastError());
            goto done;
        }
        if (optval != i%2)
        {
            Trace("\nSO_EXCLUSIVEADDRUSE should be now %s [optval=%d]\n", i%2, optval);
            err = -1;
            goto done;
        }
    }

    /* reset SO_REUSEADDR */
    optval = 0;
    err = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(int));
    if(0 != err)
    {
        Trace("\nFailed to set SO_REUSEADDR=%d option error code =%d!\n",
              optval, GetLastError());
        goto done;
    }
    /* reset SO_EXCLUSIVEADDRUSE */
    optval = 0;
    err = setsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&optval, sizeof(int));
    if(0 != err)
    {
        Trace("\nFailed to set SO_EXCLUSIVEADDRUSE=%d option error code =%d!\n",
              optval, GetLastError());
        goto done;
    }
    /* set SO_REUSEADDR */
    optval = 1;
    err = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(int));
    if(0 != err)
    {
        Trace("\nFailed to set SO_REUSEADDR=%d option error code =%d!\n",
              optval, GetLastError());
        goto done;
    }
    /* set SO_EXCLUSIVEADDRUSE, expect WSAEINVAL */
    optval = 1;
    err = setsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&optval, sizeof(int));
    if((SOCKET_ERROR != err) || (GetLastError() != WSAEINVAL))
    {
        Trace("\nSet SO_EXCLUSIVEADDRUSE=%d should have failed, intead it did not [GetLastError=%d]!\n",
              optval, GetLastError());
        goto done;
    }
    /* make sure it is indeed 0 */
    optsize = sizeof(int);
    err = getsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&optval, &optsize);
    if(0 != err)
    {
        Trace("\nFailed to get SO_EXCLUSIVEADDRUSE option error code =%d!\n",
              GetLastError());
        goto done;
    }
    if (optval != 0)
    {
        Trace("\nSO_EXCLUSIVEADDRUSE should not be set at this time\n");
        err = -1;
        goto done;
    }

    /* reset SO_REUSEADDR */
    optval = 0;
    err = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(int));
    if(0 != err)
    {
        Trace("\nFailed to set SO_REUSEADDR=%d option error code =%d!\n",
              optval, GetLastError());
        goto done;
    }
    /* set SO_EXCLUSIVEADDRUSE */
    optval = 1;
    err = setsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&optval, sizeof(int));
    if(0 != err)
    {
        Trace("\nFailed to set SO_EXCLUSIVEADDRUSE=%d option error code =%d!\n",
              optval, GetLastError());
        goto done;
    }

    /* set SO_REUSEADDR, expect WSAEINVAL */
    optval = 1;
    err = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(int));
    if((SOCKET_ERROR != err) || (GetLastError() != WSAEINVAL))
    {
        Trace("\nSet SO_REUSEADDR=%d should have failed, intead it did not [GetLastError=%d]!\n",
              optval, GetLastError());
        goto done;
    }
    /* make sure it is indeed 0 */
    optsize = sizeof(int);
    err = getsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, &optsize);
    if(0 != err)
    {
        Trace("\nFailed to get SO_REUSEADDR option error code =%d!\n",
              GetLastError());
        goto done;
    }
    if (optval != 0)
    {
        Trace("\nSO_REUSEADDR should not be set at this time\n");
        err = -1;
        goto done;
    }

    /* close the socket */
    err = closesocket(s);
    if(0 != err)
    {
        Trace("\nFailed to close socket error code =%d!\n",
              GetLastError());
        goto done;
    }

 done:
    if(0 != err)
    {
        Fail("");
    }

    PAL_Terminate();
    return PASS;

};
