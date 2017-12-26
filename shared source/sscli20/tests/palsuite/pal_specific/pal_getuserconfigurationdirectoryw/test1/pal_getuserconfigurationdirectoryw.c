/*=============================================================
**
** Source: pal_getuserconfigurationdirectoryw.c
**
** Purpose: Positive test the PAL_GetUserConfigurationDirectoryW API.
**          Call PAL_GetUserConfigurationDirectoryW to retrieve the user
**          configuration directory.
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
    DWORD dwFileAttribute;

    WCHAR *wDirectoryName = malloc(DIRECTORYLENGTH*sizeof(WCHAR));

    //Initialize the PAL environment
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        ExitProcess(FAIL);
    }

    //retrive the user configuration directory
    err = PAL_GetUserConfigurationDirectoryW(wDirectoryName,DIRECTORYLENGTH);

    if(0 == err || 0 == strlen(convertC(wDirectoryName)))
    {
        free(wDirectoryName);
        Fail("Failed to call PAL_GetUserConfiguretionDirectoryW API!\n");
    }


    //retrive the attributes of a file or directory
    dwFileAttribute = GetFileAttributesW(wDirectoryName);


    //check if the retrived attribute indicates a directory
    if( FILE_ATTRIBUTE_DIRECTORY != (FILE_ATTRIBUTE_DIRECTORY & dwFileAttribute))
    {
        free(wDirectoryName);
        Fail("Failed to call PAL_GetUserConfiguretionDirectoryW API!\n");
    }


    //free the memory
    free(wDirectoryName);

    PAL_Terminate();
    return PASS;

}
