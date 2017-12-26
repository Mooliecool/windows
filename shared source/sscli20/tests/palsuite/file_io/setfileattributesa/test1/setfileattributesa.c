/*=====================================================================
**
** Source:  SetFileAttributesA.c
**
**
** Purpose: Tests the PAL implementation of the SetFileAttributesA function
** Test that we can set a file READONLY, and then that we're unable to 
** open that file with WRITE access.  Then change it to NORMAL attributes, and
** try to open it again -- it should work now.
**
** Depends:
**        CreateFile
**        CloseHandle
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

/* According to the spec, only READONLY attribute can be set
   in FreeBSD.
*/

#include <palsuite.h>



int __cdecl main(int argc, char **argv)
{
    DWORD TheResult;
    HANDLE TheFile;
    char* FileName = {"test_file"};
    
    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }
    
    
    /* Try to set the file to Read-only */

    TheResult = SetFileAttributesA(FileName, FILE_ATTRIBUTE_READONLY);
    
    if(TheResult == 0)
    {
        Fail("ERROR: SetFileAttributesA returned 0, failure, when trying "
               "to set the FILE_ATTRIBUTE_READONLY attribute.");
    }

    /* Attempt to open this READONLY file with WRITE access,
       The open should fail and the HANDLE should be invalid.
    */

    TheFile = CreateFileA(
        FileName,                         // file name
        GENERIC_READ|GENERIC_WRITE,       // access mode
        0,                                // share mode
        NULL,                             // SD
        OPEN_ALWAYS,                      // how to create
        FILE_ATTRIBUTE_NORMAL,            // file attributes
        NULL                              // handle to template file
        );

    if(TheFile != INVALID_HANDLE_VALUE) 
    {
        TheResult = CloseHandle(TheFile);
        if(TheResult == 0) 
        {
            Fail("ERROR: CloseHandle failed.  This tests relies upon it "
                "working properly.");
        }
        Fail("ERROR: Tried to open a file that was created as "
               "READONLY with the GENERIC_WRITE access mode.  This should"
               " cause CreateFile to return an INVALID_HANDLE_VALUE.");
    }

    /* Try to open the file with READ access, this should be ok.
       The HANDLE will be valid.
    */

    TheFile = CreateFileA(
        FileName,                         // file name
        GENERIC_READ,                     // access mode
        0,                                // share mode
        NULL,                             // SD
        OPEN_ALWAYS,                      // how to create
        FILE_ATTRIBUTE_NORMAL,            // file attributes
        NULL                              // handle to template file
        );

    if(TheFile == INVALID_HANDLE_VALUE) 
    {
        Fail("ERROR: Tried to open a file that was created as "
               "READONLY with the GENERIC_READ access mode.  This should"
               " cause CreateFile to return an valid handle, but "
               "INVALID_HANDLE_VALUE was returned!.");
    }
    
    /* Close that HANDLE */

    TheResult = CloseHandle(TheFile);

    if(TheResult == 0) 
    {
        Fail("ERROR: CloseHandle failed.  This tests relies upon it "
               "working properly.");
    }

    /* Set the file to NORMAL */

    TheResult = SetFileAttributesA(FileName, FILE_ATTRIBUTE_NORMAL);
     
    if(TheResult == 0)
    {
        Fail("ERROR: SetFileAttributesA returned 0, failure, when trying "
               "to set the FILE_ATTRIBUTE_NORMAL attribute.");
    }

    /* To ensure that the set worked correctly, try to open the file
       with WRITE access again -- this time it should succeed.
    */

    TheFile = CreateFileA(
        FileName,                         // file name
        GENERIC_READ|GENERIC_WRITE,       // access mode
        0,                                // share mode
        NULL,                             // SD
        OPEN_ALWAYS,                      // how to create
        FILE_ATTRIBUTE_NORMAL,            // file attributes
        NULL                              // handle to template file
        );
     
    if(TheFile == INVALID_HANDLE_VALUE) 
    {
        Fail("ERROR: Tried to open a file that was created as "
               "NORMAL with the GENERIC_WRITE access mode.  This should"
               " cause CreateFile to return an valid handle, but "
               "INVALID_HANDLE_VALUE was returned!.");
    }

    TheResult = CloseHandle(TheFile);

    if(TheResult == 0) 
    {
        Fail("ERROR: CloseHandle failed.  This tests relies upon it "
               "working properly.");
    }
    
    PAL_Terminate();
    return PASS;
}
