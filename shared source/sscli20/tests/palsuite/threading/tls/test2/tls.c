/*=============================================================================
**
** Source: tls.c
**
** Purpose: Test to ensure TlsAlloc and TlsFree are working when we try 
**          to allocate the guaranted minimum number of indicies.
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

#define NUM_OF_INDEX    64
/* Minimum guaranteed is at least 64 for all systems.*/

/**
 * main
 *
 * executable entry point
 */
INT __cdecl main( INT argc, CHAR **argv )
{
    DWORD dwIndexes[NUM_OF_INDEX];
    int i,j;

    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
	    return FAIL;
    }

    /* Allocate a bunch of TLS indexes. */
    for( i = 0; i < NUM_OF_INDEX; i++ )
    {
        if( (dwIndexes[i] = TlsAlloc()) == TLS_OUT_OF_INDEXES )
        {/*ERROR */
            DWORD dwError = GetLastError();
            Fail("TlsAlloc() returned -1 with error %d"
                 "when trying to allocate %d index\n",
                  dwError,
                  i);
        }
    }

    /* Free the TLS indexes.*/
    for( j = 0; j < NUM_OF_INDEX; j++ )
    {
        if( TlsFree(dwIndexes[j]) == 0 )
        {/* ERROR */
            DWORD dwError = GetLastError();
            Fail("TlsFree() returned 0 with error %d"
                 "when trying to free %d index\n",
                  dwError,
                  i);
        }
    }

    PAL_Terminate();

    return PASS;
}

