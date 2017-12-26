/*=============================================================================
**
** Source: wsacommon.h
**
** Purpose: Common functions for the WSA functions tests
** 
** Dependencies: closesocket
**               Trace
**               Fail
**               WSACleanup
**               GetLastError
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
**===========================================================================*/

#ifndef __WSACOMMON_H__
#define __WSACOMMON_H__

#include <palsuite.h>

/**
 * DoTestCleanup
 *
 * Cleans up the test by
 * closing the active sockets (if any), 
 * cleaning up WSA 
 */
void DoWSATestCleanup( SOCKET sockList[],
                       int numSockets )
{
    int i = 0;    /* counter */
    int err;      /* generic variable to store the error into */
    BOOL bFailed; /* failure indication */

    bFailed = FALSE; /* no failure yet */

    /* sanity check */
    if( (sockList != NULL) &&
        (numSockets != 0 ) )
    {
        /* Close the open sockets */
        for( ; i < numSockets; i++ )
        {            
            if( sockList[i] != INVALID_SOCKET )
            {
                err = closesocket(sockList[i]);
                if( err == SOCKET_ERROR )
                {    
                    Trace("ERROR: Unexpected failure: "
                          "closesocket(%d) returned (%d)\n", 
                          sockList[i],
                          GetLastError());

                    bFailed = TRUE;
                }
            }

        }
    }
 
    /* terminate use of WinSock DLL */
    err = WSACleanup( );
    if( err == SOCKET_ERROR )
    {
        Trace("ERROR: Unexpected failure: "
              "WSACleanup returned %d",
              GetLastError());

        bFailed = TRUE;
    }

    /* Call Fail("") if any failures occured */
    if( bFailed == TRUE )
    {
        Fail("");
    }
}

#endif /* __WSACOMMON_H__ */
