/*=====================================================================
**
** Source:  MoveFileExA.c
**
** Purpose: Tests the PAL implementation of the MoveFileExA function.
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


LPSTR lpSource[4] = { 
						"src_existing.tmp",
						"src_non-existant.tmp",
						"src_dir_existing",
						"src_dir_non-existant"
					};
LPSTR lpDestination[4]={
						"dst_existing.tmp",
						"dst_non-existant.tmp",
						"dst_dir_existing",
						"dst_dir_non-existant"
						};

LPSTR lpFiles[14] ={
						"src_dir_existing\\test01.tmp",
						"src_dir_existing\\test02.tmp",
						"dst_dir_existing\\test01.tmp",
						"dst_dir_existing\\test02.tmp",
						"src_dir_non-existant\\test01.tmp",
						"src_dir_non-existant\\test02.tmp",
						"dst_existing.tmp\\test01.tmp",
						"dst_existing.tmp\\test02.tmp",
						"dst_non-existant.tmp\\test01.tmp",
						"dst_non-existant.tmp\\test02.tmp",
						"dst_dir_existing\\test01.tmp",
						"dst_dir_existing\\test02.tmp",
						"dst_dir_non-existant\\test01.tmp",
						"dst_dir_non-existant\\test02.tmp"
						};
  
DWORD dwFlag[2] = {MOVEFILE_COPY_ALLOWED, MOVEFILE_REPLACE_EXISTING};



int createExisting(void)
{
    HANDLE tempFile  = NULL;
    HANDLE tempFile2 = NULL;

    /* create the src_existing file and dst_existing file */
    tempFile = CreateFileA(lpSource[0], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,                        
                            FILE_ATTRIBUTE_NORMAL, 0);
    tempFile2 = CreateFileA(lpDestination[0], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, 0);
    CloseHandle(tempFile2);
    CloseHandle(tempFile);

    if ((tempFile == NULL) || (tempFile2 == NULL))
    {
        Trace("ERROR[%ul]: couldn't create %S or %S\n", GetLastError(), lpSource[0], 
                lpDestination[0]);
        return FAIL;    
    }

    /* create the src_dir_existing and dst_dir_existing directory and files */
    CreateDirectoryA(lpSource[2], NULL);

    tempFile = CreateFileA(lpFiles[0], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, 0);
    tempFile2 = CreateFileA(lpFiles[1], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, 0);
    CloseHandle(tempFile2);
    CloseHandle(tempFile);

    if ((tempFile == NULL) || (tempFile2 == NULL))
    {
        Trace("ERROR[%ul]: couldn't create src_dir_existing\\test01.tmp\n", GetLastError());
        return FAIL;
    }

    CreateDirectoryA(lpDestination[2], NULL);
    tempFile = CreateFileA(lpFiles[2], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, 0);
    tempFile2 = CreateFileA(lpFiles[3], GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, 0);
    CloseHandle(tempFile2);
    CloseHandle(tempFile);

    if ((tempFile == NULL) || (tempFile2 == NULL))
    {
        Trace("ERROR[%ul]: couldn't create dst_dir_existing\\test01.tmp\n" , GetLastError());
        return FAIL;
    }
    return PASS;
}


void removeAll(void)
{
    /* get rid of destination dirs and files */
    DeleteFileA(lpSource[0]);
    DeleteFileA(lpSource[1]);

    DeleteFileA(lpFiles[0]);
    DeleteFileA(lpFiles[1]);
    RemoveDirectoryA(lpSource[2]);

    DeleteFileA(lpFiles[4]);
    DeleteFileA(lpFiles[5]);
    RemoveDirectoryA(lpSource[3]);

    /* get rid of destination dirs and files */
    DeleteFileA(lpDestination[0]);
    DeleteFileA(lpFiles[6]);
    DeleteFileA(lpFiles[7]);
    RemoveDirectoryA(lpDestination[0]);

    DeleteFileA(lpDestination[1]);
    DeleteFileA(lpFiles[8]);
    DeleteFileA(lpFiles[9]);
    RemoveDirectoryA(lpDestination[1]);

    DeleteFileA(lpDestination[2]);  
    DeleteFileA(lpFiles[10]);
    DeleteFileA(lpFiles[11]);
    RemoveDirectoryA(lpDestination[2]);

    RemoveDirectoryA(lpDestination[3]);
    DeleteFileA(lpFiles[12]);
    DeleteFileA(lpFiles[13]);
    RemoveDirectoryA(lpDestination[3]);

}

int __cdecl main(int argc, char *argv[])
{
    BOOL bRc = TRUE;
    char results[40];
    FILE* resultsFile = NULL;
    int i, j, k, nCounter = 0;
    int res = FAIL;
    char tempSource[] = {'t','e','m','p','k','.','t','m','p','\0'};
    char tempDest[] = {'t','e','m','p','2','.','t','m','p','\0'};
    HANDLE hFile;
    DWORD result;

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    Trace ("Started 1");

    /* read in the expected results to compare with actual results */
    memset (results, 0, 20);
    resultsFile = fopen("expectedresults.txt", "r");
    if (resultsFile == NULL)
    {
        Trace("MoveFileExA ERROR: Unable to open \"expectedresults.txt\"\n");
        goto EXIT;
    }

    fgets(results, 20, resultsFile);
    fclose(resultsFile);

    nCounter = 0;

    Trace ("Started 2");
    /* clean the slate */
    removeAll();
    if (createExisting() != PASS)
    {
        goto EXIT;
    }  
     for (i = 0; i < 4; i++)
    {
        Trace("MoveFileExa lpSource[%d] is %s", i, lpSource[i]);
    }
    Trace ("Started 3");
    /* lpSource loop */
    for (i = 0; i < 4; i++)
    {
        /* lpDestination loop */
        for (j = 0; j < 4; j++)
        {
            /* dwFlag loop */
            for (k = 0; k < 2; k++)
            {
                Trace("Started i[%d], j[%d], k[%d],MoveFileExA(%s, %s, %s): Started\n", i, j, k,
                        lpSource[i], lpDestination[j], 
                        k == 1 ? 
                        "MOVEFILE_REPLACE_EXISTING":"MOVEFILE_COPY_ALLOWED");
                /* move the file to the new location */
                //Trace("MoveFileExa lpSource[%d] is %s \n", i, lpSource[i]);
                //Trace("MoveFileExa lpDestination[%d] is %s \n", j, lpDestination[j]);
                //Trace("MoveFileExa dwFlag[%d]\n", k);


                bRc = MoveFileExA(lpSource[i], lpDestination[j], dwFlag[k]);

                if ( ((bRc == FALSE) && (results[nCounter] == '1')) || 
                    ((bRc == TRUE ) && (results[nCounter] == '0')))
                {
                    Trace("MoveFileExA(%s, %s, %s): FAILED\n", 
                        lpSource[i], lpDestination[j], 
                        k == 1 ? 
                        "MOVEFILE_REPLACE_EXISTING":"MOVEFILE_COPY_ALLOWED");
                    goto EXIT;
                }

                Trace("Over %d, %d, %d, MoveFileExA(%s, %s, %s)\n", i, j, k,
                        lpSource[i], lpDestination[j], 
                        k == 1 ? 
                        "MOVEFILE_REPLACE_EXISTING":"MOVEFILE_COPY_ALLOWED");
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
    hFile = CreateFileA(tempSource, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,                        
                            FILE_ATTRIBUTE_NORMAL, 0);

    if( hFile == INVALID_HANDLE_VALUE )
    {
        Trace("MoveFileExA: CreateFile failed to "
            "create the file correctly.\n");
        goto EXIT;
    }
    
    bRc = CloseHandle(hFile);
    if(!bRc)
    {
        Trace("MoveFileExA: CloseHandle failed to close the "
            "handle correctly. yo %u\n",GetLastError());
        goto EXIT;
    }

    /* set the file attributes to be readonly */
    bRc = SetFileAttributesA(tempSource, FILE_ATTRIBUTE_READONLY);
    if(!bRc)
    {
        Trace("MoveFileExA: SetFileAttributes failed to set file "
            "attributes correctly. ERROR:%u\n",GetLastError());
        goto EXIT;
    }

    /* move the file to the new location */
    bRc = MoveFileExA(tempSource, tempDest, MOVEFILE_COPY_ALLOWED );
    if(!bRc)
    {
        Trace("MoveFileExA(%S, %S, %s): GetFileAttributes "
            "failed to get the file's attributes.\n",
            tempSource, tempDest, "MOVEFILE_COPY_ALLOWED");
        goto EXIT;
    }

    /* check that the newly moved file has the same file attributes
    as the original */
    result = GetFileAttributesA(tempDest);
    if(result == 0)
    {
        Trace("MoveFileExA: GetFileAttributes failed to get "
            "the file's attributes.\n");
        goto EXIT;
    }   

    if((result & FILE_ATTRIBUTE_READONLY) != FILE_ATTRIBUTE_READONLY)
    {
        Trace("MoveFileExA: GetFileAttributes failed to get "
            "the correct file attributes.\n");
        goto EXIT;
    }

    /* set the file attributes back to normal, to be deleted */
    bRc = SetFileAttributesA(tempDest, FILE_ATTRIBUTE_NORMAL);
    if(!bRc)
    {
        Trace("MoveFileExA: SetFileAttributes "
            "failed to set file attributes correctly.\n");
        goto EXIT;
    }

    /* delete the newly moved file */
    bRc = DeleteFileA(tempDest);
    if(!bRc)
    {
        Trace("MoveFileExA: DeleteFileA failed to delete the"
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

