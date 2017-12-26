/*=============================================================
**
** Source: pal_getmachineconfigurationw
**
** Purpose: Positive test the PAL_GetMachineConfigurationW API.
**          Call PAL_GetMachineConfigurationW API to retrieve the
**          system configuration directory.
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

#define DIRECTORYLENGTH 1024

int __cdecl main(int argc, char *argv[])
{
    int err;
    BOOL fBool;
    DWORD dwFileAttribute;
    WCHAR *wpDirectoryName = NULL;
    char *pDirectoryName = NULL; 
  
    //Initialize the PAL environment
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        ExitProcess(FAIL);
    }

    wpDirectoryName = malloc(DIRECTORYLENGTH*sizeof(WCHAR)); 

    if(NULL == wpDirectoryName)
    {
        Fail("\nFailed to allocate memory for storing directory name!\n");
    }
    //retrive the machine configuration directory
    fBool = PAL_GetMachineConfigurationDirectoryW(wpDirectoryName,DIRECTORYLENGTH);
    
    if(FALSE == fBool)
    {
        free(wpDirectoryName);
        Fail("Failed to call PAL_GetMachineConfiguretionDirectoryW API"
               " to retrive the machine configuration directory!\n");
    }

    //convert the wide one to a standard string
    pDirectoryName = convertC(wpDirectoryName);

    if(0 == strlen(pDirectoryName))
    {
        free(wpDirectoryName);
        free(pDirectoryName);
        Fail("Failed to call PAL_GetMachineConfiguretionDirectoryW API!\n");
    }


    //free the memory
    free(pDirectoryName);

    //retrive the attribute of a file or directory
    dwFileAttribute = GetFileAttributesW(wpDirectoryName);

    //check if the attribute indicates a directory
    if(FILE_ATTRIBUTE_DIRECTORY != (dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY))
    {
        free(wpDirectoryName);
        Fail("Failed to call PAL_GetMachineConfiguretionDirectoryW API,"
            "the retrived directory is not a valid directory!\n");
    }

    free(wpDirectoryName);
    PAL_Terminate();
    return PASS;
}
