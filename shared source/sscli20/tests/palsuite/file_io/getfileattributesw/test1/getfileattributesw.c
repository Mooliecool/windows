/*=============================================================================
**
** Source:  GetFileAttributesW.c
**
** Purpose: Tests the PAL implementation of the GetFileAttributesW function by
**          checking the attributes of:
**          - a normal directory and file
**          - a read only directory and file
**          - a read write directory and file
**          - a hidden directory and file 
**          - a read only hidden directory and file
**          - a directory and a file with no attributes
**          - an invalid file name
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
**===========================================================================*/
#include <palsuite.h>

/* Structure defining a test case */
typedef struct 
{
    char *name;     /* name of the file/directory */
    DWORD attribs;  /* expected attributes */
}TestCase;

int __cdecl main(int argc, char **argv)
{
    int i;
    BOOL  bFailed = FALSE;
    DWORD result;
    WCHAR unicodeName[MAX_PATH];

    char * NormalDirectoryName          = "normal_test_directory";
    char * ReadOnlyDirectoryName        = "ro_test_directory";
    char * ReadWriteDirectoryName       = "rw_test_directory";
    char * HiddenDirectoryName          = ".hidden_directory";
    char * HiddenReadOnlyDirectoryName  = ".hidden_ro_directory";
    char * NoDirectoryName              = "no_directory";

    char * NormalFileName               = "normal_test_file";
    char * ReadOnlyFileName             = "ro_test_file";  
    char * ReadWriteFileName            = "rw_file";
    char * HiddenFileName               = ".hidden_file";
    char * HiddenReadOnlyFileName       = ".hidden_ro_file";
    char * NoFileName                   = "no_file";
    char * NotReallyAFileName           = "not_really_a_file";
    
    int numTests = 13;
    TestCase gfaTests[13]; /* GetFileAttributes tests list */

    /* Tests on directory */
    gfaTests[0].name    = NormalDirectoryName;
    gfaTests[0].attribs = FILE_ATTRIBUTE_DIRECTORY;

    gfaTests[1].name    = ReadOnlyDirectoryName;
    gfaTests[1].attribs = FILE_ATTRIBUTE_DIRECTORY | 
                          FILE_ATTRIBUTE_READONLY;

    gfaTests[2].name    = ReadWriteDirectoryName;
    gfaTests[2].attribs = FILE_ATTRIBUTE_DIRECTORY;

    gfaTests[3].name    = HiddenDirectoryName;
    gfaTests[3].attribs = FILE_ATTRIBUTE_DIRECTORY | 
                          FILE_ATTRIBUTE_HIDDEN;

    gfaTests[4].name    = HiddenReadOnlyDirectoryName;
    gfaTests[4].attribs = FILE_ATTRIBUTE_DIRECTORY | 
                          FILE_ATTRIBUTE_READONLY |
                          FILE_ATTRIBUTE_HIDDEN;

    gfaTests[5].name    = NoDirectoryName;
    gfaTests[5].attribs = FILE_ATTRIBUTE_DIRECTORY;


    /* Tests on file */
    gfaTests[6].name    = NormalFileName;
    gfaTests[6].attribs = FILE_ATTRIBUTE_NORMAL;

    gfaTests[7].name    = ReadOnlyFileName;
    gfaTests[7].attribs = FILE_ATTRIBUTE_READONLY;

    gfaTests[8].name    = ReadWriteFileName;
    gfaTests[8].attribs = FILE_ATTRIBUTE_NORMAL;

    gfaTests[9].name    = HiddenFileName;
    gfaTests[9].attribs = FILE_ATTRIBUTE_HIDDEN;

    gfaTests[10].name    = HiddenReadOnlyFileName;
    gfaTests[10].attribs = FILE_ATTRIBUTE_READONLY |
                           FILE_ATTRIBUTE_HIDDEN;

    gfaTests[11].name    = NoFileName;
    gfaTests[11].attribs = FILE_ATTRIBUTE_NORMAL;

    gfaTests[12].name    = NotReallyAFileName;
    gfaTests[12].attribs = -1;
    

    /* Initialize PAL environment */
    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    /* 
     * Go through all the test cases above,
     * call GetFileAttributesW on the  unicode version of name and
     * make sure the return value is the one expected
     */
    for( i = 0; i < numTests; i++ )
    {
        /* Convert the name to Unicode */
        if( 0 == MultiByteToWideChar(CP_ACP,
                                     0,
                                     gfaTests[i].name,
                                     -1,
                                     unicodeName,
                                     MAX_PATH) )
        {
            Fail("Unexpected Failure: Unable to convert name "
                 "to unicode using MultiByteToWideChar that "
                 "returned 0 with error %d\n",
                 GetLastError());
        }

        result = GetFileAttributesW(unicodeName);

        if( result != gfaTests[i].attribs )
        {
            bFailed = TRUE;

            Trace("ERROR: GetFileAttributesW Test#%u on %s "
                  "returned %u instead of %u. \n",
                  i,
                  gfaTests[i].name,
                  result,
                  gfaTests[i].attribs);

        }
    }

    /* If any errors, just call Fail() */
    if( bFailed )
    {
        Fail("");
    }

    PAL_Terminate();
    return PASS;
}

