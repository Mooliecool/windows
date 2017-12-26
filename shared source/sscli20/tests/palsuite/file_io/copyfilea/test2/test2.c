/*=====================================================================
**
** Source:  test2.c
**
** Purpose: Tests the PAL implementation of the CopyFileA function
**          to see if a file can be copied to itself
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

    BOOL bRc = TRUE;
    char* szSrcExisting = "src_existing.tmp";
    FILE* tempFile = NULL;
    DWORD temp;
    int retCode;
    
    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

   /* create the src_existing file */
    tempFile = fopen(szSrcExisting, "w");
    if (tempFile != NULL)
    {
        retCode = fputs("CopyFileA test file: src_existing.tmp ", tempFile);
        if(retCode < 0)
        {
            Fail("CopyFileA: ERROR-> Couldn't write to %s with error "
                "%u.\n", szSrcExisting, GetLastError());
        }
        retCode = fclose(tempFile);
        if(retCode != 0)
        {
            Fail("CopyFileA: ERROR-> Couldn't close file: %s with error "
                "%u.\n", szSrcExisting, GetLastError());
        }

    }
    else
    {
        Fail("CopyFileA: ERROR-> Couldn't create %s with "
            "error %ld\n",szSrcExisting,GetLastError());
    }

    /* Get file attributes of source */
    temp = GetFileAttributes(szSrcExisting);
    if (temp == -1)
    {
        Fail("CopyFileA: GetFileAttributes of source file "
            "failed with error code %ld. \n",
            GetLastError());
    }
    
    /* make sure a file can't copy to itself
    first testing with IfFileExists flag set to true */
    bRc = CopyFileA(szSrcExisting,szSrcExisting,TRUE);
    if(bRc)
    {
        Fail("ERROR: Cannot copy a file to itself, %u",GetLastError());
    }
    
    /* try to get file attributes of desitnation */
    if (GetFileAttributesA(szSrcExisting) == -1)
    {
        Fail("CopyFileA: GetFileAttributes of destination file "
            "failed with error code %ld. \n",
            GetLastError());  
    }
    else
    {
        /* verify attributes of destination file to source file*/               
     
        if(temp != GetFileAttributes(szSrcExisting))
        {
            Fail("CopyFileA : The file attributes of the "
                "destination file do not match the file "
                "attributes of the source file.\n");
        }
    }

    /* testing with IfFileExists flags set to false
    should fail in Windows and pass in UNIX */
    bRc = CopyFileA(szSrcExisting,szSrcExisting,FALSE);
    if(bRc && (GetLastError() != ERROR_ALREADY_EXISTS))
    {
        Fail("ERROR: Cannot copy a file to itself, %u",GetLastError());
    }
    
    if (GetFileAttributesA(szSrcExisting) == -1)
    {
        Fail("CopyFileA: GetFileAttributes of destination file "
            "failed with error code %ld. \n",
            GetLastError());
    }
    else
    {
        /* verify attributes of destination file to source file*/               
     
        if(temp != GetFileAttributes(szSrcExisting))
        {
            Fail("CopyFileA : The file attributes of the "
                "destination file do not match the file "
                "attributes of the source file.\n");
        }
    }

    PAL_Terminate();
    return PASS;
}
