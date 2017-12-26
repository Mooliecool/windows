/*============================================================================
**
** Source: test1.c
**
** Purpose: Tests WideCharToMultiByte with all the ASCII characters (0-127).
**          Also tests that WideCharToMultiByte handles different buffer
**          lengths correctly (0, -1, and a valid length)
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
**==========================================================================*/

#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{    
    char mbStr[128];
    WCHAR wideStr[128];
    int ret;
    int i;
    int k;
    BOOL bRet=TRUE;

    /* These codepages are currently supported by the PAL */
    int codePages[] ={
        CP_ACP,
        932,
        949,
        950,
        1252,
        1258,
        CP_UTF8
    };


    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }


    /* Go through all of the code pages */
    for(i=0; i<(sizeof(codePages)/sizeof(int)); i++)
    {

        for (k=0; k<128; k++)
        {
            wideStr[k] = 127 - k;
            mbStr[k] = 0;
        }

        /* Convert with buffer size of 0 */
        ret = WideCharToMultiByte(codePages[i], 0, wideStr, -1, 
                                  mbStr, 0, NULL, NULL);
        if (ret != 128)
        {
            Trace("WideCharToMultiByte did not return correct string length!\n"
                  "Got %d, expected %d for code page %d with error %u.\n", 
                  ret, 128,codePages[i],GetLastError());
            bRet=FALSE;
        }

        /* Make sure the ASCII set (0-127) gets translated correctly */
        ret = WideCharToMultiByte(codePages[i], 0, wideStr, -1, 
                                  mbStr, 128, NULL, NULL);
        if (ret != 128)
        {
            Trace("WideCharToMultiByte did not return correct string length!\n"
                  "Got %d, expected %d for code page %d with error %u.\n", 
                  ret, 128,codePages[i],GetLastError());
            bRet=FALSE;
        }

        for (k=0; k<128; k++)
        {
            if (mbStr[k] != 127 - k)
            {
                Trace("WideCharToMultiByte failed to translate correctly!\n"
                      "Expected character %d to be %c (%x), got %c (%x) for "
                      "code page %d\n",k, 127 - k, 127 - k,mbStr[k], mbStr[k],
                      codePages[i]);
                bRet=FALSE;
            }
        }


        /* try a 0 length string ("") */
        wideStr[0] = '\0';
        ret = WideCharToMultiByte(codePages[i], 0, wideStr, -1, 
                                  mbStr, 0, NULL, NULL);
        if (ret != 1)
        {
            Trace("WideCharToMultiByte did not return correct string length!\n"
                  "Got %d, expected %d for code page %d with error %u.\n", 
                  ret, 1,codePages[i],GetLastError());
            bRet=FALSE;
        }
    }

    PAL_Terminate();

    if(bRet == FALSE)
    {
        return FAIL;
    }
    return PASS;
}

