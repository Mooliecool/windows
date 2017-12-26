/*============================================================================
**
** Source:  test3.c
**
** Purpose: Test that passes CP_ACP to GetCPInfo, verifying the results.
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

/* Currently only one CodePage "CP_ACP" is supported by the PAL */

int __cdecl main(int argc, char *argv[])
{
    CPINFO cpinfo;
    
    /* Initialize the PAL.
     */
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }

    /* Test GetCPInfo with CP_ACP.
     */
    if (!GetCPInfo(CP_ACP, &cpinfo))
    {
        Fail("GetCPInfo() unable to get info for code page %d!\n", CP_ACP);
    }

    /* Terminate the PAL.
     */
    PAL_Terminate();
    return PASS;
}

