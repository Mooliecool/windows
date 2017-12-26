/*=====================================================================
**
** Source:  SetFileAttributesW.c
**
** Purpose: Tests the PAL implementation of the SetFileAttributesW function
** Test that the function fails if the file doesn't exist..
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

#define UNICODE

#include <palsuite.h>


int __cdecl main(int argc, char **argv)
{
    DWORD TheResult;
    WCHAR FileName[MAX_PATH];
    
    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }
    
    /* Make a wide character string for the file name */
    
    MultiByteToWideChar(CP_ACP,
                        0,
                        "no_file",
                        -1,
                        FileName,
                        MAX_PATH);

    
    /* Try to set the file to NORMAL on a file that doesn't
       exist.
    */

    TheResult = SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);
    
    if(TheResult != 0)
    {
        Fail("ERROR: SetFileAttributes returned non-zero0, success, when"
               " trying to set the FILE_ATTRIBUTE_NORMAL attribute on a non "
               "existant file.  This should fail.");
    }

   
    
    PAL_Terminate();
    return PASS;
}
