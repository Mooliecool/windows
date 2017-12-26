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

int __cdecl main(int argc, char *argv[]) {
  
    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    /* Check to make sure there are no problems accepting all the ICON 
       styles and FLAG styles.  These don't change anything, unless 
       they don't work at all. 
    */
  
    if(MessageBox(NULL, 
                  convert("Pal Testing"), 
                  convert("Pal Title"), 
                  MB_OK |MB_ICONEXCLAMATION|MB_TASKMODAL) != IDOK) 
    {
        Fail("ERROR: The MB_OK style should always return IDOK.");    
    }
  
    if(MessageBox(NULL, 
                  convert("Pal Testing"), 
                  convert("Pal Title"), 
                  MB_OK |MB_ICONINFORMATION|MB_SYSTEMMODAL) != IDOK) 
    {
        Fail("ERROR: The MB_OK style should always return IDOK.");
    }

    /* MB_SERVICE_NOTIFICATION doesn't seem to be available under windows?  
       It claims it exists and it should be supported under FreeBSD.
    */
  
  
    if(MessageBox(NULL, 
                  convert("Pal Testing"), 
                  convert("Pal Title"), 
                  MB_OK |MB_ICONQUESTION) != IDOK) 
    {
        Fail("ERROR: The MB_OK style should always return IDOK.");
    }
    
    PAL_Terminate();
    return PASS;
}


