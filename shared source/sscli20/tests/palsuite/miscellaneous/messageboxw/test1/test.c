/*============================================================
**
** Source: test.c
**
** Purpose: Test for MessageBoxW() function
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

int __cdecl main(int argc, char *argv[]) 
{
    /* Declare Variables to use with convert()*/
    WCHAR * PalTitle = NULL;
    WCHAR * OkTesting = NULL;
    WCHAR * AbortTesting = NULL;
    WCHAR * YesTesting = NULL;
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
    
    PalTitle = convert("Pal Testing");
    OkTesting = convert("Click OK Please!");

    /* Handle, text, title, style */
    if(MessageBox(NULL, OkTesting, 
                  PalTitle, 
                  MB_OK) != IDOK) 
    {
        free(OkTesting);
        free(PalTitle);
        Fail("ERROR: The MB_OK style should return IDOK.");
    }

    free(OkTesting);
    AbortTesting = convert("Click Abort Please!");
    if(MessageBox(NULL, 
                  AbortTesting, 
                  PalTitle, 
                  MB_ABORTRETRYIGNORE) != IDABORT) 
    {
        free(AbortTesting);
        free(PalTitle);
        Fail("ERROR: The MB_ABORTRETRYIGNORE style should "
             "return IDABORT.");   
    }

    free(AbortTesting);
    YesTesting = convert("Click No Please!");

    if(MessageBox(NULL, 
                  YesTesting, 
                  PalTitle, 
                  MB_YESNO) != IDNO) 
    {
        free(PalTitle);
        free(YesTesting);
        Fail("ERROR: The MB_YESNO style should return IDNO.");
    }    
    
    free(YesTesting);
    free(PalTitle);
    
    PAL_Terminate();
    return PASS;
}



