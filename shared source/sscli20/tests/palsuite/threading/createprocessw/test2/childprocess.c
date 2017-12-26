/*============================================================
**
** Source: createprocessw/test2/childprocess.c
**
** Purpose: This child process reads a string from stdin
**          and writes it out to stdout & stderr
**
** Dependencies: memset
**               fgets
**               gputs
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
**=========================================================*/

#define UNICODE
#include <palsuite.h>
#include "test2.h"


int __cdecl main( int argc, char **argv ) 
{
    int iRetCode = EXIT_OK_CODE; /* preset exit code to OK */
    char szBuf[BUF_LEN];

    WCHAR *swzParam1, *swzParam2, *swzParam3 = NULL;


    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
    if (argc != 4)
    {
        return EXIT_ERR_CODE3;
    }

    swzParam1 = convert(argv[1]);
    swzParam2 = convert(argv[2]);
    swzParam3 = convert(argv[3]);

    if (wcscmp(swzParam1, szArg1) != 0
        || wcscmp(swzParam2, szArg2) != 0
        || wcscmp(swzParam3, szArg3) != 0)
    {
        return EXIT_ERR_CODE4;
    }

    free(swzParam1);
    free(swzParam2);
    free(swzParam3);

    memset(szBuf, 0, BUF_LEN);

    /* Read the string that was written by the parent */
    if (fgets(szBuf, BUF_LEN, stdin) == NULL)
    {
        return EXIT_ERR_CODE1;
    }


    /* Write the string out to the stdout & stderr pipes */
    if (fputs(szBuf, stdout) == EOF
        || fputs(szBuf, stderr) == EOF)
    {
        return EXIT_ERR_CODE2;
    }


    PAL_Terminate();

    /* Return special exit code to indicate success or failure */
    return iRetCode;
}
