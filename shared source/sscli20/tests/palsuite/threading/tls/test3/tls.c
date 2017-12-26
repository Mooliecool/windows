/*=============================================================================
**
** Source: tls.c
**
** Purpose: Test to ensure TlsGetValue, TlsSetValue and TlsFree 
**          are not working with an invalid index
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               LocalAlloc
**               LocalFree
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

DWORD dwTlsIndex; /* TLS index */

/**
 * main
 *
 * executable entry point
 */
INT __cdecl main( INT argc, CHAR **argv )
{
    CHAR   lpstrData[256] = "";
    LPVOID lpvData = NULL;
    BOOL   bRet;

    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
         return FAIL;
    }

    /* Invalid TLS index */
    dwTlsIndex = -1;

    /*
     * Set some data in the invalid TLS index
     *Should return 0 and an error
     */
    bRet = TlsSetValue(dwTlsIndex, (LPVOID)lpstrData);

    if ( bRet != 0)
    {/*ERROR */
        Fail("TlsSetValue(%d, %x) returned %d "
             "when it should have returned 0 and an error\n",
			  dwTlsIndex,
			  lpvData,
			  bRet);
    }

    /*
     * Get the data at the invalid index
     * Should return 0 and an error
     */
    lpvData = TlsGetValue(dwTlsIndex);

    if ( lpvData != 0 )
    {/* ERROR */
	    Fail("TlsGetValue(%d) returned %d "
             "when it should have returned 0 and an error\n",
			  dwTlsIndex,
			  lpvData);
    }

    /*
     * Release the invalid TLS index
     * Should return 0 and an error
     */
    bRet = TlsFree( dwTlsIndex );

    if(  bRet != 0 )
    {/* ERROR */
        Fail("TlsFree() returned %d "
             "when it should have returned 0 and an error\n",
			  bRet);
    }

    PAL_Terminate();
    return PASS;
}


