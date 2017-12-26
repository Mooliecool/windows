/*=============================================================
**
** Source: _ui64tow.c
**
** Purpose: Positive test the _ui64tow API.
**          convert an integer to a wide character string
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
#define UNICODE
#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{
    int err;
    WCHAR *wpBuffer = NULL;
    char *pChar = NULL;
    unsigned long ul = 1234567890UL;
    char *pChar10 = "1234567890";
    char *pChar2 = "1001001100101100000001011010010";
    char *pChar16 = "499602d2";

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    wpBuffer = malloc(64 * sizeof(WCHAR));
    if(NULL == wpBuffer)
    {
        Fail("\nFail to allocate the buffer to save a converted "
                "wide character string, error code=%d!\n",
                GetLastError());
    }

    /*convert to a 10 base string*/
    _ui64tow(ul, wpBuffer, 10);
    pChar = convertC(wpBuffer);
    if(strcmp(pChar10, pChar))
    {
        free(wpBuffer);
        free(pChar);
        Fail("\nFailed to call _ui64tow API to convert an interger "
                "to a 10 base wide character string, error code=%d\n",
                GetLastError()); 
    }
    free(pChar);
    free(wpBuffer);    

    wpBuffer = malloc(64 * sizeof(WCHAR));
    if(NULL == wpBuffer)
    {
        Fail("\nFail to allocate the buffer to save a converted "
                "wide character string, error code=%d!\n",
                GetLastError());
    }
    
    /*convert to a 16 base string*/
    _ui64tow(ul, wpBuffer, 16);
    pChar = convertC(wpBuffer);
    if(strcmp(pChar16, pChar))
    {
        free(wpBuffer);
        free(pChar);
        Fail("\nFailed to call _ui64tow API to convert an interger "
                "to a 16 base wide character string, error code = %d\n",
                GetLastError()); 
    }
    free(pChar);
    free(wpBuffer);    

    wpBuffer = malloc(64 * sizeof(WCHAR));
    if(NULL == wpBuffer)
    {
        Fail("\nFail to allocate the buffer to save a converted "
                "wide character string, error code=%d!\n",
                GetLastError());
    }
    /*convert to a 2 base string*/
    _ui64tow(ul, wpBuffer, 2);
    pChar = convertC(wpBuffer);
    if(strcmp(pChar2, pChar))
    {
        free(wpBuffer);
        free(pChar);
        Fail("\nFailed to call _ui64tow API to convert an interger "
                "to a 2 base wide character string, error code=%d\n",
                GetLastError()); 
    }
    free(pChar);
    free(wpBuffer);
   
    PAL_Terminate();
    return PASS;
}
