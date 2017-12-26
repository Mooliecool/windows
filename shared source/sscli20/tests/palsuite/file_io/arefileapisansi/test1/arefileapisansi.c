/*=====================================================================
**
** Source:  AreFileApisANSI.c
**
** Purpose: Tests the PAL implementation of the AreFileApisANSI function.
**          The only possible return is TRUE.
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
**===================================================================*/



#include <palsuite.h>


int __cdecl main(int argc, char *argv[])
{
    BOOL bRc = FALSE;

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    bRc = AreFileApisANSI();


    if (bRc == FALSE)
    {
        Fail("AreFileApisANSI: ERROR: Function returned FALSE whereas only TRUE "
            "is acceptable.\n");
    }

    PAL_Terminate();  
    return PASS;
}
