/*=============================================================================
**
** Source: test1.c
**
** Purpose: Test to ensure WSAStartup works properly.
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Fail
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
#include <palsuite.h>

/**
 * Helper version structure
 */
struct version
{
    BYTE lo;  /* low-order version byte */
    BYTE hi;  /* high-order version byte */
};



/**
 * main
 * 
 * executable entry point
 */
INT __cdecl main( INT argc, CHAR **argv ) 
{
    /* local variables */
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    int i;
    
    /* list of the distinct winsock versions to test for */
    struct version tests[] =
    {
        {2, 2}
    };


    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }

    /* iterate over all the winsock versions */
    for( i = 0; i < sizeof(tests) / sizeof(struct version); i++)
    {
        /* request specified winsock version */
        wVersionRequested = MAKEWORD( tests[i].lo, tests[i].hi );

        err = WSAStartup( wVersionRequested, &wsaData );
        if( err != 0 ) {
            Fail( "Unexpected WSAStartup call failed with error code %d\n",
                  err ); 
        }

        /* Confirm that the WinSock DLL supports the specified version. */
        if( LOBYTE( wsaData.wVersion ) != tests[i].lo ||
            HIBYTE( wsaData.wVersion ) != tests[i].hi )
        {
            /* Tell the user that we could not find a usable winsock DLL. */
            WSACleanup();
            Fail(   "Requested winsock version unsupported, "
                    "returned version %d.%d\n", 
                    LOBYTE( wsaData.wVersion ),
                    HIBYTE( wsaData.wVersion ) ); 
        }

        /* cleanup the winsock library for the next pass */
        if( WSACleanup() != 0 )
        {
            DWORD dwSocketError = GetLastError();
            Fail(   "WSACleanup failed with error code %lu\n",
                    dwSocketError );
        }
    }
    

    /* PAL termination */
    PAL_Terminate();
    return PASS; 
}
