/*=============================================================
**
** Source: gethostname_neg.c
**
** Purpose: Negatively test gethostname API to retrieve local hostname.
**          Call gethostname without calling WSAStartup first.
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

#define HNAME_SIZE 1024

int __cdecl main(int argc, char *argv[])
{
    int err;
    char HostName[HNAME_SIZE];

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    memset(HostName, 0, HNAME_SIZE);
    /*
    retrieve the local host name without calling
    WSAStartup API
    */
    err = gethostname(HostName, HNAME_SIZE);
    if(WSANOTINITIALISED != GetLastError() || SOCKET_ERROR != err)
    {    
        Fail("\nFailed to call gethostname API for a negative test, "
            "call this API without calling WSAStartup!\n");
    }

    PAL_Terminate();
    return PASS;
}
