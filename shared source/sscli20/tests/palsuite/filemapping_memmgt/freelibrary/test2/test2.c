/*=====================================================================
** 
** Source:  test2.c (FreeLibrary)
**
** Purpose: Tests the PAL implementation of the FreeLibrary function.
**          This is a negative test that will pass an invalid and a
**          null handle to FreeLibrary.
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

int __cdecl main(int argc, char* argv[])
{
    HANDLE hLib;

    /* Initialize the PAL. 
     */
    if ((PAL_Initialize(argc, argv)) != 0)
    {
        return (FAIL);
    }

    /* Attempt to pass FreeLibrary an invalid handle. 
     */
    hLib = INVALID_HANDLE_VALUE;
    if (FreeLibrary(hLib))
    {
        Fail("ERROR: Able to free library handle = \"0x%lx\".\n",
              hLib);
    }
    
    /* Attempt to pass FreeLibrary a NULL handle. 
     */
    hLib = NULL;
    if (FreeLibrary(hLib))
    {
        Fail("ERROR: Able to free library handle = \"NULL\".\n");
    }


    /* Terminate the PAL.
     */
    PAL_Terminate();
    return PASS;

}
