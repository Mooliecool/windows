/*=============================================================
**
** Source: pal_getpaldirectoryw.c
**
** Purpose: Positive test the PAL_GetPALDirectoryW API.
**          Call this API to retrieve a fully-qualified 
**          directory name where the PAL DLL is loaded from.
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
**============================================================*/
#define UNICODE
#include <palsuite.h>

int __cdecl main(int argc, char *argv[])
{
    int err;
    BOOL bValue;
    DWORD dwFileAttribute;
    WCHAR *wpDirectoryName = NULL;
    char *pDirectoryName = NULL;
  
    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*allocate momory to store the directory name*/
    wpDirectoryName = malloc(MAX_PATH*sizeof(WCHAR));
    if(NULL == wpDirectoryName)
    {
        Fail("\nFailed to allocate memory for storing directory name!\n");
    } 

    /*retrieve the machine configuration directory*/
    bValue = PAL_GetPALDirectoryW(wpDirectoryName, MAX_PATH);
    if(FALSE == bValue) 
    {
        free(wpDirectoryName);
        Fail("Failed to call PAL_GetPALDirectoryW API, "
                "error code =%u\n", GetLastError());
    }
    

    /*convert wide char string to a standard one*/
    pDirectoryName = convertC(wpDirectoryName);
    if(0 == strlen(pDirectoryName))
    {
        free(wpDirectoryName);
        free(pDirectoryName);
        Fail("The retrieved directory name string is empty!\n");
    }

    /*free the memory*/
    free(pDirectoryName);

    /*retrieve the attribute of a file or directory*/
    dwFileAttribute = GetFileAttributesW(wpDirectoryName);

    /*free the memory*/
    free(wpDirectoryName);

    /*check if the attribute indicates a directory*/
    if(FILE_ATTRIBUTE_DIRECTORY != 
            (dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY))
    {
        Fail("The retrived directory name is not a valid directory!\n");
    }

    PAL_Terminate();
    return PASS;
}
