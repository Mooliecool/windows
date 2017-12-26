/*=====================================================================
**
** Source:  test1.c (SetErrorMode)
**
** Purpose: Tests the PAL implementation of the SetErrorMode function.
**          This test will set the error mode and then read the error
**          set with GetLastError().
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

int __cdecl main(int argc, char **argv)
{   
    DWORD dErrorReturn;
    UINT  dErrorModes[] = {SEM_NOOPENFILEERRORBOX, SEM_FAILCRITICALERRORS, 0};
    int   i;
    
    /*
     *  Initialize the Pal
     */
    if ((PAL_Initialize(argc,argv)) != 0)
    {
        return (FAIL);
    }

    /*
     *  Loop through the supported Error Modes and verify
     *  that GetLastError() returns the correct Error Mode
     */
    for (i=0; i < (sizeof(dErrorModes) / sizeof(UINT)); i++)
    {
        SetLastError(dErrorModes[i]);
        if ((dErrorReturn = GetLastError()) != dErrorModes[i])
        {   
            Fail("ERROR: SetLastError was set to 0x%4.4x but,"
                    " GetLastError returned 0x%4.4x\n", 
                    dErrorModes[i],
                    dErrorReturn);
        }
    }
        
    PAL_Terminate();
    return (PASS);
}
