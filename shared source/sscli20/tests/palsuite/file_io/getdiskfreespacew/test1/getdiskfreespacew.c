/*=====================================================================
**
** Source:  GetDiskFreeSpaceW.c (test 1)
**
** Purpose: Tests the PAL implementation of the GetDiskFreeSpaceW function.
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
    DWORD dwSectorsPerCluster_02;  /* sectors per cluster */
    DWORD dwBytesPerSector_02;     /* bytes per sector */
    DWORD dwNumberOfFreeClusters;  /* free clusters */
    DWORD dwTotalNumberOfClusters; /* total clusters */
    BOOL bRc = FALSE;
    WCHAR szwRootPath[10] = {'/','\0'};


    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }

    /*  test the NULL option which translates to the current drive */
    bRc = GetDiskFreeSpaceW(NULL,
        &dwSectorsPerCluster,   
        &dwBytesPerSector,      
        &dwNumberOfFreeClusters,
        &dwTotalNumberOfClusters);
    if (bRc != TRUE)
    {
        Fail("GetDiskFreeSpaceW: ERROR -> Failed with error code: %ld\n", 
            GetLastError());
    }
    else if (dwSectorsPerCluster == 0)
    {
        Fail("GetDiskFreeSpaceW: ERROR -> dwSectorsPerCluster returned 0\n");
    }
    else if (dwBytesPerSector == 0)
    {
        Fail("GetDiskFreeSpaceW: ERROR -> dwBytesPerSector returned 0\n");
    }

    /*  test the root directory to the current drive */
    bRc = GetDiskFreeSpaceW(szwRootPath,
        &dwSectorsPerCluster_02,   
        &dwBytesPerSector_02,      
        &dwNumberOfFreeClusters,
        &dwTotalNumberOfClusters);
    if (bRc != TRUE)
    {
        Fail("GetDiskFreeSpaceW: ERROR -> Failed with error code: %ld\n", 
            GetLastError());
    }
    else if (dwSectorsPerCluster == 0)
    {
        Fail("GetDiskFreeSpaceW: ERROR -> dwSectorsPerCluster returned 0\n");
    }
    else if (dwBytesPerSector == 0)
    {
        Fail("GetDiskFreeSpaceW: ERROR -> dwBytesPerSector returned 0\n");
    }
    /* 
    ** make sure the values returned for NULL path and root path 
    ** are the same
    */
    else if (dwSectorsPerCluster_02 != dwSectorsPerCluster)
    {
        Fail("GetDiskFreeSpaceW: ERROR -> dwSectorsPerCluster for NULL path "
            "(%u) should have been the same as the root path (%u).\n",
            dwSectorsPerCluster,
            dwSectorsPerCluster_02);
    }
    else if (dwBytesPerSector_02 != dwBytesPerSector)
    {
        Fail("GetDiskFreeSpaceW: ERROR -> dwBytesPerSector for NULL path "
            "(%u) should have been the same as the root path (%u).\n",
            dwBytesPerSector,
            dwBytesPerSector_02);
    }

    PAL_Terminate();
    return PASS;
}
