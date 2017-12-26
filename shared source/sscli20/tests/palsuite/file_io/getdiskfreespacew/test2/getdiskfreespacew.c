/*=====================================================================
**
** Source:  GetDiskFreeSpaceW.c (test 2)
**
** Purpose: Tests the PAL implementation of the GetDiskFreeSpaceW 
**          function on valid non-root paths.
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
    DWORD dwSectorsPerCluster;     /* sectors per cluster */
    DWORD dwBytesPerSector;        /* bytes per sector */
    DWORD dwNumberOfFreeClusters;  /* free clusters */
    DWORD dwTotalNumberOfClusters; /* total clusters */
    BOOL bRc = FALSE;
    WCHAR szwCurrentPath[MAX_PATH];


    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    /* get the current directory so we are sure to have a valid path */
    if (!GetCurrentDirectoryW(MAX_PATH, szwCurrentPath))
    {
        Fail("GetDiskFreeSpaceW: ERROR -> GetCurrentDirectoryW failed with "
            "error code: %u.\n", 
            GetLastError());
    }

    /*  test the current path*/
    bRc = GetDiskFreeSpaceW(szwCurrentPath,
        &dwSectorsPerCluster,
        &dwBytesPerSector,
        &dwNumberOfFreeClusters,
        &dwTotalNumberOfClusters);
    if (bRc != TRUE)
    {
        Fail("GetDiskFreeSpaceW: ERROR -> Failed with error code: %u for "
            "the path \"%S\".\n", 
            GetLastError(),
            szwCurrentPath);
    }
    else if (dwSectorsPerCluster == 0)
    {
        Fail("GetDiskFreeSpaceW: ERROR -> dwSectorsPerCluster returned 0\n");
    }
    else if (dwBytesPerSector == 0)
    {
        Fail("GetDiskFreeSpaceW: ERROR -> dwBytesPerSector returned 0\n");
    }

    PAL_Terminate();
    return PASS;
}
