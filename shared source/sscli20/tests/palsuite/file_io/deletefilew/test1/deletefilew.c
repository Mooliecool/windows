/*=====================================================================
**
** Source:  DeleteFileW.c
**
** Purpose: Tests the PAL implementation of the DeleteFileW function.
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

//	delete an existing file
//	delete a non-existant file
//  delete an open file
//	delete files using wild cards
//	delete a hidden file
//  delete a file without proper permissions
//


#include <palsuite.h>


int __cdecl main(int argc, char *argv[])
{
    FILE *tempFile = NULL;
    BOOL bRc = FALSE;
    WCHAR* pTemp = NULL;


    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    //
    // deleting an existing file
    //
    tempFile = fopen("testFile01.tmp", "w");
    if (tempFile == NULL)
    {
        Fail ("DeleteFileW: ERROR: Couldn't create \"DeleteFileW's"
            " testFile.tmp\"\n");
    }

    fprintf(tempFile, "DeleteFileW test file.\n");
    fclose(tempFile);

    pTemp = convert("testFile01.tmp");
    bRc = DeleteFileW(pTemp);
    free(pTemp);
    if (bRc != TRUE)
    {
        Fail ("DeleteFileW: ERROR: Couldn't delete DeleteFileW's"
            " \"testFile01.tmp\"\n");
    }


    //
    // deleting a non-existant file : should fail
    //

    pTemp = convert("testFile02.tmp");
    bRc = DeleteFileW(pTemp);
    free(pTemp);
    if (bRc != FALSE)
    {
        Fail ("DeleteFileW: ERROR: Was able to delete the non-existant"
            " file \"testFile02.tmp\"\n");
    }




    //
    // deleting an open file
    //
    tempFile = fopen("testFile03.tmp", "w");
    if (tempFile == NULL)
    {
        Fail("DeleteFileW: ERROR: Couldn't create \"DeleteFileW's"
            " testFile.tmp\"\n");
    }

    fprintf(tempFile, "DeleteFileW test file.\n");
    fclose(tempFile);

    pTemp = convert("testFile03.tmp");
    bRc = DeleteFileW(pTemp);
    if (bRc != TRUE)
    {
        Fail("DeleteFileW: ERROR: Couldn't delete DeleteFileW's"
            " \"testFile01.tmp\"\n");
        free(pTemp);
    }
    bRc = DeleteFileW(pTemp);
    free(pTemp);




    //
    // delete using wild cards
    //

    // create the test file
    tempFile = fopen("testFile04.tmp", "w");
    if (tempFile == NULL)
    {
        Fail("DeleteFileW: ERROR: Couldn't create DeleteFileW's"
            " \"testFile04.tmp\"\n");
    }
    fprintf(tempFile, "DeleteFileW test file.\n");
    fclose(tempFile);

    // delete using '?'
    pTemp = convert("testFile0?.tmp");
    bRc = DeleteFileW(pTemp);
    free(pTemp);
    if (bRc == TRUE)
    {
        Fail("DeleteFileW: ERROR: Was able to delete using the"
            " \'?\' wildcard\n");
    }

    // delete using '*'
    pTemp = convert("testFile*.tmp");
    bRc = DeleteFileW(pTemp);
    free(pTemp);
    if (bRc == TRUE)
    {
        Fail("DeleteFileW: ERROR: Was able to delete using the"
            " \'*\' wildcard\n");
    }

    pTemp = convert("testFile04.tmp");
    bRc = DeleteFileW(pTemp);
    free(pTemp);
    if (bRc != TRUE)
    {
        Fail ("DeleteFileW: ERROR: Couldn't delete DeleteFileW's"
            " \"testFile04.tmp\"\n");
    }

    PAL_Terminate();  
    return PASS;
}
