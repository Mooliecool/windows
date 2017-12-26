/*=====================================================================
**
** Source:  MoveFileA.c
**
** Purpose: Tests the PAL implementation of the MoveFileA function.
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

LPSTR lpSource[4] = {"src_existing.txt",
                      "src_non-existant.txt",
                      "src_dir_existing",
                      "src_dir_non-existant"};
LPSTR lpDestination[4] = {"dst_existing.txt",
                          "dst_non-existant.txt",
                          "dst_dir_existing",
                          "dst_dir_non-existant"};


/* Create all the required test files */
int createExisting(void)
{
    FILE* tempFile = NULL;
    DWORD dwError;
    BOOL bRc = FALSE;
    char szBuffer[100];

    /* create the src_existing file */
    tempFile = fopen(lpSource[0], "w");
    if (tempFile != NULL)
    {
        fprintf(tempFile, "MoveFileA test file: src_existing.txt\n");
        fclose(tempFile);
    }
    else
    {
        Trace("ERROR: couldn't create %s\n", lpSource[0]);
        return FAIL;
    }

    /* create the src_dir_existing directory and files */
    bRc = CreateDirectoryA(lpSource[2], NULL);
    if (bRc != TRUE)
    {
        Trace("MoveFileA: ERROR: couldn't create \"%s\" because of "
            "error code %ld\n", 
            lpSource[2],
            GetLastError());
        return FAIL;
    }

    memset(szBuffer, 0, 100);
    sprintf(szBuffer, "%s/test01.txt", lpSource[2]);
    tempFile = fopen(szBuffer, "w");
    if (tempFile != NULL)
    {
        fprintf(tempFile, "MoveFileA test file: %s\n", szBuffer);
        fclose(tempFile);
    }
    else
    {
        Trace("ERROR[%ld]:MoveFileA couldn't create %s\n", GetLastError(), szBuffer);
        return FAIL;
    }

    memset(szBuffer, 0, 100);
    sprintf(szBuffer, "%s/test02.txt", lpSource[2]);
    tempFile = fopen(szBuffer, "w");
    if (tempFile != NULL)
    {
        fprintf(tempFile, "MoveFileA test file: %s\n", szBuffer);
        fclose(tempFile);
    }
    else
    {
        Trace("ERROR[%ld]: couldn't create %s\n", GetLastError(), szBuffer);
        return FAIL;
    }


    /* create the dst_existing file */
    tempFile = fopen(lpDestination[0], "w");
    if (tempFile != NULL)
    {
        fprintf(tempFile, "MoveFileA test file: dst_existing.txt\n");
        fclose(tempFile);
    }
    else
    {
        Trace("ERROR[%ld]:MoveFileA couldn't create \"%s\"\n", GetLastError(), lpDestination[0]);
        return FAIL;
    }

    /* create the dst_dir_existing directory and files */
    bRc = CreateDirectoryA(lpDestination[2], NULL);
    if (bRc != TRUE)
    {
        dwError = GetLastError();
		Trace("Error[%ld]:MoveFileA: couldn't create \"%s\"\n", GetLastError(), lpDestination[2]);
        return FAIL;
    }

    tempFile = fopen("dst_dir_existing/test01.txt", "w");
    if (tempFile != NULL)
    {
        fprintf(tempFile, "MoveFileA test file: dst_dir_existing/test01.txt\n");
        fclose(tempFile);
    }
    else
    {
        Trace("ERROR: couldn't create dst_dir_existing/test01.txt\n");
        return FAIL;
    }
    tempFile = fopen("dst_dir_existing/test02.txt", "w");
    if (tempFile != NULL)
    {
        fprintf(tempFile, "MoveFileA test file: dst_dir_existing/test02.txt\n");
        fclose(tempFile);
    }
    else
    {
        Trace("ERROR[%ul]: couldn't create dst_dir_existing/test02.txt\n", GetLastError());
        return FAIL;
    }
	       
    return PASS;
}



/* remove all created files in preparation for the next test */
void removeAll(void)
{
    char szTemp[40];

    /* get rid of source dirs and files */
    DeleteFileA(lpSource[0]);
    DeleteFileA(lpSource[1]);

    sprintf(szTemp, "%s/test01.txt", lpSource[2]);
    DeleteFileA(szTemp);
    sprintf(szTemp, "%s/test02.txt", lpSource[2]);
    DeleteFileA(szTemp);
    RemoveDirectoryA(lpSource[2]);

    sprintf(szTemp, "%s/test01.txt", lpSource[3]);
    DeleteFileA(szTemp);
    sprintf(szTemp, "%s/test02.txt", lpSource[3]);
    DeleteFileA(szTemp);
    RemoveDirectoryA(lpSource[3]);
    DeleteFileA(lpSource[2]);
    DeleteFileA(lpSource[3]);


    /* get rid of destination dirs and files */
    DeleteFileA(lpDestination[0]);

    sprintf(szTemp, "%s/test01.txt", lpDestination[0]);
    DeleteFileA(szTemp);
    sprintf(szTemp, "%s/test02.txt", lpDestination[0]);
    DeleteFileA(szTemp);
    RemoveDirectoryA(lpDestination[0]);

    DeleteFileA(lpDestination[1]);

    sprintf(szTemp, "%s/test01.txt", lpDestination[1]);
    DeleteFileA(szTemp);
    sprintf(szTemp, "%s/test02.txt", lpDestination[1]);
    DeleteFileA(szTemp);
    RemoveDirectoryA(lpDestination[1]);

    sprintf(szTemp, "%s/test01.txt", lpDestination[2]);
    DeleteFileA(szTemp);
    sprintf(szTemp, "%s/test02.txt", lpDestination[2]);
    DeleteFileA(szTemp);
    RemoveDirectoryA(lpDestination[2]);

    sprintf(szTemp, "%s/test01.txt", lpDestination[3]);
    DeleteFileA(szTemp);
    sprintf(szTemp, "%s/test02.txt", lpDestination[3]);
    DeleteFileA(szTemp);
    RemoveDirectoryA(lpDestination[3]);
    /* clean up any destinations that were files */
    DeleteFileA(lpDestination[0]);
    DeleteFileA(lpDestination[1]);
    DeleteFileA(lpDestination[2]);
    DeleteFileA(lpDestination[3]);
}





int __cdecl main(int argc, char *argv[])
{
    BOOL bRc = TRUE;
    BOOL bSuccess = TRUE;
    char results[40];
    FILE* resultsFile = NULL;
    int nCounter = 0;
    int i, j;
    char tempSource[] = {'t','e','m','p','k','.','t','m','p','\0'};
    char tempDest[] = {'t','e','m','p','2','.','t','m','p','\0'};
    HANDLE hFile;
    DWORD result;

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    /* read in the expected results to compare with actual results */
    memset (results, 0, 20);
    resultsFile = fopen("expectedresults.txt", "r");
    if (resultsFile == NULL)
    {
        Fail("MoveFileA ERROR[%ul]: Unable to open \"expectedresults.txt\"\n", GetLastError());
    }

    fgets(results, 20, resultsFile);
    fclose(resultsFile);

    /* clean the slate */
    removeAll();

    if (createExisting() != 0)
    {
        removeAll();
    }


    /* lpSource loop */
    for (i = 0; i < 4; i++)
    {
        /* lpDestination loop */
        for (j = 0; j < 4; j++)
        {
            bRc = MoveFileA(lpSource[i], lpDestination[j]);
            if (bRc == FALSE)
            {
                if (results[nCounter] == '1')
                {
                    Trace("MoveFileA: FAILED: test[%d][%d]: \"%s\" -> \"%s\"\n", 
                        i, j, lpSource[i], lpDestination[j]);
                    bSuccess = FALSE;
                }
            }
            else
            {
                if (results[nCounter] == '0')
                {
                    Trace("MoveFileA: FAILED: test[%d][%d]: \"%s\" -> \"%s\"\n", 
                        i, j, lpSource[i], lpDestination[j]);
                    bSuccess = FALSE;
                }
            }

            /* undo the last move */
            removeAll();
            createExisting();

            nCounter++;
        }
    }

    removeAll();
    if (bSuccess == FALSE)
    {
        Fail("MoveFileA: Test Failed");
    }

    /* create the temp source file */
    hFile = CreateFileA(tempSource, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,                        
                            FILE_ATTRIBUTE_NORMAL, 0);

    if( hFile == INVALID_HANDLE_VALUE )
    {
		Fail("Error[%ul]:MoveFileA: CreateFile failed to "
            "create the file correctly.\n", GetLastError());
    }

    bRc = CloseHandle(hFile);
    if(!bRc)
    {
        Trace("MoveFileA: CloseHandle failed to close the "
            "handle correctly. ERROR:%u\n",GetLastError());
        
        /* delete the created file */
        bRc = DeleteFileA(tempSource);
        if(!bRc)
        {
			Fail("Error[%ul]:MoveFileA: DeleteFileA failed to delete the"
                "file correctly.\n", GetLastError());
        }
        Fail("");
    }

    /* set the file attributes to be readonly */
    bRc = SetFileAttributesA(tempSource, FILE_ATTRIBUTE_READONLY);
    if(!bRc)
    {
        Trace("MoveFileA: SetFileAttributes failed to set file "
            "attributes correctly. GetLastError returned %u\n",GetLastError());
        /* delete the created file */
        bRc = DeleteFileA(tempSource);
        if(!bRc)
        {
			Fail("Error[%ul]:MoveFileA: DeleteFileA failed to delete the"
                "file correctly.\n", GetLastError());
        }
        Fail("");
    }

    /* move the file to the new location */
    bRc = MoveFileA(tempSource, tempDest);
    if(!bRc)
    {
        /* delete the created file */
        bRc = DeleteFileA(tempSource);
        if(!bRc)
        {
			Fail("Error[%ul]:MoveFileA: DeleteFileA failed to delete the"
                "file correctly.\n", GetLastError());
    }

		Fail("Error[%ul]:MoveFileA(%S, %S): GetFileAttributes "
            "failed to get the file's attributes.\n",
            GetLastError(), tempSource, tempDest);
    }

    /* check that the newly moved file has the same file attributes
    as the original */
    result = GetFileAttributesA(tempDest);
    if(result == 0)
    {
        /* delete the created file */
        bRc = DeleteFileA(tempDest);
        if(!bRc)
        {
			Fail("Error[%ul]:MoveFileA: DeleteFileA failed to delete the"
                "file correctly.\n", GetLastError());
        }

		Fail("Error[%ul]:MoveFileA: GetFileAttributes failed to get "
            "the file's attributes.\n", GetLastError());
    }   

    if((result & FILE_ATTRIBUTE_READONLY) != FILE_ATTRIBUTE_READONLY)
    {
        /* delete the newly moved file */
        bRc = DeleteFileA(tempDest);
        if(!bRc)
        {
			Fail("Error[%ul]:MoveFileA: DeleteFileA failed to delete the"
                "file correctly.\n", GetLastError());
        }

        Fail("Error[%ul]MoveFileA: GetFileAttributes failed to get "
            "the correct file attributes.\n", GetLastError());
    }

    /* set the file attributes back to normal, to be deleted */
    bRc = SetFileAttributesA(tempDest, FILE_ATTRIBUTE_NORMAL);
    if(!bRc)
    {
        /* delete the newly moved file */
        bRc = DeleteFileA(tempDest);
        if(!bRc)
        {
			Fail("Error[%ul]:MoveFileA: DeleteFileA failed to delete the"
                "file correctly.\n", GetLastError());
        }

		Fail("Error[%ul]:MoveFileA: SetFileAttributes failed to set "
            "file attributes correctly.\n", GetLastError());
    }

    /* delete the newly moved file */
    bRc = DeleteFileA(tempDest);
    if(!bRc)
    {
		Fail("Error[%ul]:MoveFileA: DeleteFileA failed to delete the"
            "file correctly.\n", GetLastError());
    }

    PAL_Terminate(); 

    return PASS;
}
