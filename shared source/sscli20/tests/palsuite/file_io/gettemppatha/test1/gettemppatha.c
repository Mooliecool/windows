/*=====================================================================
**
** Source:  gettemppatha.c (test 1)
**
** Purpose: Tests the PAL implementation of the GetTempPathA function.
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


static void SetEnvVar(char tmpValue[], char tempPath[])
{
    BOOL checkValue=FALSE;
    
    /* see if the environment variable is created correctly */
    checkValue = SetEnvironmentVariable(tmpValue, tempPath);
    if(!checkValue)
    {
        if (tempPath == NULL)
        {
            if (SetEnvironmentVariable(tmpValue, "") &&
                SetEnvironmentVariable(tmpValue, NULL))
            {
                return;
            }
        }
        
        Fail("GetTempPathA: ERROR -> Failed to set the environment "
            "variable correctly.\n");
    }  
}


int __cdecl main(int argc, char *argv[])
{
    DWORD dwBuffLength = _MAX_DIR;
    CHAR  path[_MAX_DIR];
    char* tmpValue = "TMP";
    char* tempValue = "TEMP";
#if WIN32
    char tempPath[] = "C:\\temp\\";
    char tmpPath[] = "C:\\tmp\\";
#else
    char tempPath[] = ".\\temp\\";
    char tmpPath[] = ".\\tmp\\";
#endif


    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    if (GetTempPathA(dwBuffLength, path) == 0)
    {
        Fail("GetTempPathA: ERROR -> Failed to return a temporary path. "
            "error code: %ld\n", GetLastError());
    }
    else
    {
        // CreateDirectory should fail on an existing directory
        if (CreateDirectoryA(path, NULL) != 0)
        {
            Fail("GetTempPathA: ERROR -> The path returned is apparently "
                "invalid since CreateDirectory succeeded whereas it should "
                "have failed.\n");
        }
    }


    /* set both tmp and temp to null and check that gettemppath returns
    a non zero value*/
    SetEnvVar(tmpValue, NULL);
    
    /* create the variable tempValue */
    SetEnvVar(tempValue, "");  
    
    /* set tempValue to null initially */
    SetEnvironmentVariable(tempValue, NULL);  

    /* get the temp path */
    if(GetTempPathA(dwBuffLength, path) == 0)
    {
        Fail("GetTempPathA: ERROR -> Failed to return a temporary path. "
            "error code: %ld\n", GetLastError());
    }

    /* set temp, and gettemppatha should return value of temp */
    SetEnvVar(tempValue, tempPath);

    if(GetTempPathA(dwBuffLength, path) == 0)
    {
        Fail("GetTempPathA: ERROR -> Failed to return a temporary path. "
            "error code: %ld\n", GetLastError());
    }
    
    if(strcmp(path,tempPath) != 0)
    {
        Fail("GetTempPathA: ERROR -> Failed to return correct temporary path. "
            "Expected path %s but got %s.\n", tempPath, path);
    }

    /* set temp to null, and set temp to a proper value,
    gettemppatha should return value stored in tmp */
    SetEnvVar(tempValue, NULL);
    SetEnvVar(tmpValue, tmpPath);
  
    if(GetTempPathA(dwBuffLength, path) == 0)
    {
        Fail("GetTempPathA: ERROR -> Failed to return a temporary path. "
            "error code: %ld\n", GetLastError());
    }

    if(strcmp(path,tmpPath) != 0)
    {
        Fail("GetTempPathA: ERROR -> Failed to return correct temporary path. "
            "Expected path %s but got %s.\n", tmpPath, path);
    }

    /* set temp and gettemppatha should return value stored in tmp */
    SetEnvVar(tempValue, tempPath);
  
    if(GetTempPathA(dwBuffLength, path) == 0)
    {
        Fail("GetTempPathA: ERROR -> Failed to return a temporary path. "
            "error code: %ld\n", GetLastError());
    }

    if(strcmp(path,tmpPath) != 0)
    {
        Fail("GetTempPathA: ERROR -> Failed to return correct temporary path. "
            "Expected path %s but got %s.\n", tmpPath, path);
    }
         
    PAL_Terminate();
    return PASS;
}


