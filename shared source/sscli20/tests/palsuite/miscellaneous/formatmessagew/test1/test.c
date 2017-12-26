/*============================================================
**
** Source : test.c
**
** Purpose: Test for FormatMessageW() function
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


int __cdecl main(int argc, char *argv[]) {

    WCHAR TheString[] = {'P','a','l',' ','T','e','s','t','\0'};
    WCHAR OutBuffer[128];
    int ReturnResult;

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }
 

    ReturnResult = FormatMessage(
        FORMAT_MESSAGE_FROM_STRING, /* source and processing options */
        TheString,                  /* message source */
        0,                          /* message identifier */
        0,                          /* language identifier */
        OutBuffer,                  /* message buffer */
        1024,                       /* maximum size of message buffer */
        NULL                        /* array of message inserts */
        );
  
  
    if(ReturnResult == 0) 
    {
        Fail("ERROR: The return value was 0, which indicates failure.  "
             "The function failed when trying to Format a simple string"
             ", with no formatters in it.");      
    }
  
    if(memcmp(OutBuffer,TheString,wcslen(OutBuffer)*2+2) != 0) 
    {
        Fail("ERROR: The formatted string should be %s but is really %s.",
             convertC(TheString),
             convertC(OutBuffer));
    }

    PAL_Terminate();
    return PASS;
}


