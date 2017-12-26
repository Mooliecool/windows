/*=====================================================================
**
** Source:  MoveFileExW.c
**
** Purpose: Tests the PAL implementation of the MoveFileExW function.
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


LPWSTR lpSource[4];
LPWSTR lpDestination[4];
LPWSTR lpFiles[14];

DWORD dwFlag[2] = {MOVEFILE_COPY_ALLOWED, MOVEFILE_REPLACE_EXISTING};



int createExisting(void)
{
    HANDLE tempFile  = NULL;
    HANDLE tempFile2 = NULL;

    /* create the src_existing file and dst_existing file */
    tempFile = CreateFileW(lpSource[0], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,                        
                            FILE_ATTRIBUTE_NORMAL, 0);
    tempFile2 = CreateFileW(lpDestination[0], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, 0);
    CloseHandle(tempFile2);
    CloseHandle(tempFile);

    if ((tempFile == NULL) || (tempFile2 == NULL))
    {
        Trace("ERROR: couldn't create %S or %S\n", lpSource[0], 
                lpDestination[0]);
        return FAIL;    
    }

    /* create the src_dir_existing and dst_dir_existing directory and files */
    CreateDirectoryW(lpSource[2], NULL);

    tempFile = CreateFileW(lpFiles[0], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, 0);
    tempFile2 = CreateFileW(lpFiles[1], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, 0);
    CloseHandle(tempFile2);
    CloseHandle(tempFile);

    if ((tempFile == NULL) || (tempFile2 == NULL))
    {
        Trace("ERROR: couldn't create src_dir_existing\\test01.tmp\n");
        return FAIL;
    }

    CreateDirectoryW(lpDestination[2], NULL);
    tempFile = CreateFileW(lpFiles[2], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, 0);
    tempFile2 = CreateFileW(lpFiles[3], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, 0);
    CloseHandle(tempFile2);
    CloseHandle(tempFile);

    if ((tempFile == NULL) || (tempFile2 == NULL))
    {
        Trace("ERROR: couldn't create dst_dir_existing\\test01.tmp\n");
        return FAIL;
    }
    return PASS;
}


void removeAll(void)
{
    /* get rid of destination dirs and files */
    DeleteFileW(lpSource[0]);
    DeleteFileW(lpSource[1]);

    DeleteFileW(lpFiles[0]);
    DeleteFileW(lpFiles[1]);
    RemoveDirectoryW(lpSource[2]);

    DeleteFileW(lpFiles[4]);
    DeleteFileW(lpFiles[5]);
    RemoveDirectoryW(lpSource[3]);

    /* get rid of destination dirs and files */
    DeleteFileW(lpDestination[0]);
    DeleteFileW(lpFiles[6]);
    DeleteFileW(lpFiles[7]);
    RemoveDirectoryW(lpDestination[0]);

    DeleteFileW(lpDestination[1]);
    DeleteFileW(lpFiles[8]);
    DeleteFileW(lpFiles[9]);
    RemoveDirectoryW(lpDestination[1]);

    DeleteFileW(lpDestination[2]);  
    DeleteFileW(lpFiles[10]);
    DeleteFileW(lpFiles[11]);
    RemoveDirectoryW(lpDestination[2]);

    RemoveDirectoryW(lpDestination[3]);
    DeleteFileW(lpFiles[12]);
    DeleteFileW(lpFiles[13]);
    RemoveDirectoryW(lpDestination[3]);

}

int __cdecl main(int argc, char *argv[])
{
    BOOL bRc = TRUE;
    char results[40];
    FILE* resultsFile = NULL;
    int i, j, k, nCounter = 0;
    int res = FAIL;
    WCHAR tempSource[] = {'t','e','m','p','k','.','t','m','p','\0'};
    WCHAR tempDest[] = {'t','e','m','p','2','.','t','m','p','\0'};
    HANDLE hFile;
    DWORD result;

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    lpSource[0] = convert("src_existing.tmp");
    lpSource[1] = convert("src_non-existant.tmp");
    lpSource[2] = convert("src_dir_existing");
    lpSource[3] = convert("src_dir_non-existant");

    lpDestination[0] = convert("dst_existing.tmp");
    lpDestination[1] = convert("dst_non-existant.tmp");
    lpDestination[2] = convert("dst_dir_existing");
    lpDestination[3] = convert("dst_dir_non-existant");

    lpFiles[0] = convert("src_dir_existing\\test01.tmp");
    lpFiles[1] = convert("src_dir_existing\\test02.tmp");
    lpFiles[2] = convert("dst_dir_existing\\test01.tmp");
    lpFiles[3] = convert("dst_dir_existing\\test02.tmp");
    lpFiles[4] = convert("src_dir_non-existant\\test01.tmp");
    lpFiles[5] = convert("src_dir_non-existant\\test02.tmp");

    lpFiles[6] = convert("dst_existing.tmp\\test01.tmp");
    lpFiles[7] = convert("dst_existing.tmp\\test02.tmp");

    lpFiles[8] = convert("dst_non-existant.tmp\\test01.tmp");
    lpFiles[9] = convert("dst_non-existant.tmp\\test02.tmp");

    lpFiles[10] = convert("dst_dir_existing\\test01.tmp");  
    lpFiles[11] = convert("dst_dir_existing\\test02.tmp");

    lpFiles[12] = convert("dst_dir_non-existant\\test01.tmp");
    lpFiles[13] = convert("dst_dir_non-existant\\test02.tmp");

    /* read in the expected results to compare with actual results */
    memset (results, 0, 20);
    resultsFile = fopen("expectedresults.txt", "r");
    if (resultsFile == NULL)
    {
        Trace("MoveFileExW ERROR: Unable to open \"expectedresults.txt\"\n");
        goto EXIT;
    }

    fgets(results, 20, resultsFile);
    fclose(resultsFile);

    nCounter = 0;


    /* clean the slate */
    removeAll();
    if (createExisting() != PASS)
    {
        goto EXIT;
    }  

    /* lpSource loop */
    for (i = 0; i < 4; i++)
    {
        /* lpDestination loop */
        for (j = 0; j < 4; j++)
        {
            /* dwFlag loop */
            for (k = 0; k < 2; k++)
            {
                
                /* move the file to the new location */
                bRc = MoveFileExW(lpSource[i], lpDestination[j], dwFlag[k]);

                if ( ((bRc == FALSE) && (results[nCounter] == '1')) || 
                    ((bRc == TRUE ) && (results[nCounter] == '0')))
                {
                    Trace("MoveFileExW(%S, %S, %s): FAILED\n", 
                        lpSource[i], lpDestination[j], 
                        k == 1 ? 
                        "MOVEFILE_REPLACE_EXISTING":"MOVEFILE_COPY_ALLOWED");
                    goto EXIT;
                }

                /* undo the last move */
                removeAll();
                if (createExisting() != PASS)
                {
                    goto EXIT;
                }
                nCounter++;
            }
        }
    }

    /* create the temp source file */
    hFile = CreateFileW(tempSource, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,                        
                            FILE_ATTRIBUTE_NORMAL, 0);

    if( hFile == INVALID_HANDLE_VALUE )
    {
        Trace("MoveFileExW: CreateFile failed to "
            "create the file correctly.\n");
        goto EXIT;
    }
    
    bRc = CloseHandle(hFile);
    if(!bRc)
    {
        Trace("MoveFileExW: CloseHandle failed to close the "
            "handle correctly. yo %u\n",GetLastError());
        goto EXIT;
    }

    /* set the file attributes to be readonly */
    bRc = SetFileAttributesW(tempSource, FILE_ATTRIBUTE_READONLY);
    if(!bRc)
    {
        Trace("MoveFileExW: SetFileAttributes failed to set file "
            "attributes correctly. ERROR:%u\n",GetLastError());
        goto EXIT;
    }

    /* move the file to the new location */
    bRc = MoveFileExW(tempSource, tempDest, MOVEFILE_COPY_ALLOWED );
    if(!bRc)
    {
        Trace("MoveFileExW(%S, %S, %s): GetFileAttributes "
            "failed to get the file's attributes.\n",
            tempSource, tempDest, "MOVEFILE_COPY_ALLOWED");
        goto EXIT;
    }

    /* check that the newly moved file has the same file attributes
    as the original */
    result = GetFileAttributesW(tempDest);
    if(result == 0)
    {
        Trace("MoveFileExW: GetFileAttributes failed to get "
            "the file's attributes.\n");
        goto EXIT;
    }   

    if((result & FILE_ATTRIBUTE_READONLY) != FILE_ATTRIBUTE_READONLY)
    {
        Trace("MoveFileExW: GetFileAttributes failed to get "
            "the correct file attributes.\n");
        goto EXIT;
    }

    /* set the file attributes back to normal, to be deleted */
    bRc = SetFileAttributesW(tempDest, FILE_ATTRIBUTE_NORMAL);
    if(!bRc)
    {
        Trace("MoveFileExW: SetFileAttributes "
            "failed to set file attributes correctly.\n");
        goto EXIT;
    }

    /* delete the newly moved file */
    bRc = DeleteFileW(tempDest);
    if(!bRc)
    {
        Trace("MoveFileExW: DeleteFileW failed to delete the"
            "file correctly.\n");
        goto EXIT;
    }

    res = PASS;

EXIT:
    removeAll();
    for (i=0; i<4; i++)
    {
        free(lpSource[i]);
        free(lpDestination[i]);
    }
    for (i=0; i<14; i++)
    {
        free(lpFiles[i]);
    }

    PAL_Terminate();    
    return res;
}

