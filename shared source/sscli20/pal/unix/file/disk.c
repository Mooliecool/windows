/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    disk.c

Abstract:

    Implementation of the disk information functions.

Revision History:

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/file.h"

#include <sys/param.h>
// do we actually need this on other platforms. We don't seem to be using anything from there
#include <sys/mount.h>
#include <errno.h>
#if HAVE_STATVFS
#include <sys/types.h>
#include <sys/statvfs.h>
#define statfs  statvfs
#if STATVFS64_PROTOTYPE_BROKEN
typedef statvfs_t pal_statfs;
#else
typedef struct statvfs pal_statfs;
#endif
#else // HAVE_STATVFS
typedef struct statfs pal_statfs;
#endif  // HAVE_STATVFS

SET_DEFAULT_DEBUG_CHANNEL(FILE);

/*++

Function:

    GetDiskFreeSpaceW
    
See MSDN doc.
--*/
PALIMPORT
BOOL
PALAPI
GetDiskFreeSpaceW(
          LPCWSTR lpDirectoryName,
          LPDWORD lpSectorsPerCluster,
          LPDWORD lpBytesPerSector,
          LPDWORD lpNumberOfFreeClusters,  /* Caller will ignore output value */
          LPDWORD lpTotalNumberOfClusters) /* Caller will ignore output value */
{
    BOOL bRetVal = FALSE;
    pal_statfs fsInfoBuffer;
    INT  statfsRetVal = 0;
    DWORD dwLastError = NO_ERROR;
    CHAR DirNameBuffer[ MAX_PATH ];

    PERF_ENTRY(GetDiskFreeSpaceW);
    ENTRY( "GetDiskFreeSpaceW( lpDirectoryName=%p (%S), lpSectorsPerCluster=%p,"
           "lpBytesPerSector=%p, lpNumberOfFreeClusters=%p, "
           "lpTotalNumberOfClusters=%p )\n", lpDirectoryName ? lpDirectoryName :
            W16_NULLSTRING, lpDirectoryName ? lpDirectoryName :
            W16_NULLSTRING, lpSectorsPerCluster, lpBytesPerSector, 
            lpNumberOfFreeClusters, lpTotalNumberOfClusters );

    /* Sanity checks. */
    if ( !lpSectorsPerCluster )
    {
        ERROR( "lpSectorsPerCluster cannot be NULL!\n" );
        dwLastError = ERROR_INVALID_PARAMETER;
        goto exit;
    }
    if ( !lpBytesPerSector )
    {
        ERROR( "lpBytesPerSector cannot be NULL!\n" );
        dwLastError = ERROR_INVALID_PARAMETER;
        goto exit;
    }
    if ( lpNumberOfFreeClusters || lpTotalNumberOfClusters )
    {
        TRACE("GetDiskFreeSpaceW is ignoring lpNumberOfFreeClusters"
              " and lpTotalNumberOfClusters\n" );
    }
    if ( lpDirectoryName && PAL_wcslen( lpDirectoryName ) == 0 )
    {
        ERROR( "lpDirectoryName is empty.\n" );
        dwLastError = ERROR_INVALID_PARAMETER;
        goto exit;
    }

    /* Fusion uses this API to round file sizes up to their actual size
       on-disk based on the BytesPerSector * SectorsPerCluster.
       The intent is to avoid computing the sum of all file sizes in the
       cache just in bytes and not account for the cluster-sized slop, when
       determining if the cache is too large or not. */

    if ( lpDirectoryName )
    {
        if ( WideCharToMultiByte( CP_ACP, 0, lpDirectoryName, -1,
                                  DirNameBuffer,MAX_PATH, 0, 0 ) != 0 )
        {
            FILEDosToUnixPathA( DirNameBuffer );
            statfsRetVal = statfs( DirNameBuffer, &fsInfoBuffer );
        }
        else
        {
            ASSERT( "Unable to convert the lpDirectoryName to multibyte.\n" );
            dwLastError = ERROR_INTERNAL_ERROR;
            goto exit;
        }
    }
    else
    {
        statfsRetVal = statfs( "/", &fsInfoBuffer );
    }

    if ( statfsRetVal == 0 )
    {
        *lpBytesPerSector = fsInfoBuffer.f_bsize;
        *lpSectorsPerCluster = 1;
        bRetVal = TRUE;
    }
    else
    {
        if ( errno == ENOTDIR || errno == ENOENT )
        {
            FILEGetProperNotFoundError( DirNameBuffer, &dwLastError );
            goto exit;
        }
        dwLastError = FILEGetLastErrorFromErrno();
        if ( ERROR_INTERNAL_ERROR == dwLastError )
        {
            ASSERT("statfs() not expected to fail with errno:%d (%s)\n", 
                   errno, strerror(errno));
        }
        else
        {
            TRACE("statfs() failed, errno:%d (%s)\n", errno, strerror(errno));
        }
    }

exit:
    if ( NO_ERROR != dwLastError )
    {
        SetLastError( dwLastError );
    }

    LOGEXIT( "GetDiskFreeSpace returning %s.\n", bRetVal == TRUE ? "TRUE" : "FALSE" );
    PERF_EXIT(GetDiskFreeSpaceW);
    return bRetVal;
}

