/*============================================================
**
** Source : test.c
**
** Purpose: Test for GetSystemInfo() function
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

#include <palsuite.h>

int __cdecl main(int argc, char *argv[]) {
  
    SYSTEM_INFO TheSystemInfo;
    SYSTEM_INFO* pSystemInfo = &TheSystemInfo;

    /*
     * Initialize the PAL and return FAILURE if this fails
     */

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    GetSystemInfo(pSystemInfo);

    /* Ensure both valules are > than 0 */
    if(pSystemInfo->dwNumberOfProcessors < 1) 
    {
        Fail("ERROR: The dwNumberofProcessors values should be > 0.");
    } 

    if(pSystemInfo->dwPageSize < 1) 
    {
        Fail("ERROR: The dwPageSize should be greater than 0.");    
    }

    /* If this isn't WIN32, ensure all the other variables are 0 */
  
  
    
    PAL_Terminate();
    return PASS;
}


