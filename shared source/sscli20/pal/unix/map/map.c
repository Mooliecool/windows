/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    map.c

Abstract:

    Implementation of file mapping API.

--*/

#include "pal/palinternal.h"
#include "pal/critsect.h"
#include "pal/dbgmsg.h"
#include "pal/virtual.h"
#include "pal/map.h"
#include "pal/file.h"
#include "pal/critsect.h"
#include "pal/handle.h"
#include "pal/thread.h"
#include "pal/init.h"
#include "pal/shmemory.h"
#include "common.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

SET_DEFAULT_DEBUG_CHANNEL(VIRTUAL);

CRITICAL_SECTION mapping_critsec;

static PMAPPED_MEMORY_INFO pMap;
static CONST PCHAR MAP_TEMPLATE = "/.file_mapping_PALbacked.XXXXXXXXXX";

static void MAPDeleteSharedObject( SHMPTR );
static INT MAPProtectionToFileOpenFlags( DWORD );
static BOOL MAPRemoveViewFromObject( PMAPPED_MEMORY_INFO, LPVOID );
static BOOL MAPAddNewViewToObject( HANDLE, DWORD, SIZE_T, LPCVOID );
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
static BOOL MAPAddThisViewToObject( HANDLE, PMAPPED_VIEW_LIST );
#endif
static BOOL MAPIsRequestPermissible( DWORD, file * );
static LPSTR MAPCreateTempFile( PINT );
static BOOL MAPContainsInvalidFlags( DWORD );
static BOOL MAPGrowLocalFile( INT, UINT );
static DWORD MAPConvertProtectToAccess( DWORD );
static BOOL MAPDesiredAccessAllowed( DWORD, DWORD, DWORD );
static INT MAPFileMapToMmapFlags( DWORD );
static PMAPPED_MEMORY_INFO MAPGetViewsObject( LPVOID );
static PGLOBAL_FILE_MAPPING_OBJECT MAPCreateGMMO( LPSTR, LPCWSTR, DWORD, DWORD, BOOL );
static HANDLE MAPAddHandle( PGLOBAL_FILE_MAPPING_OBJECT, DWORD, INT );
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
static NativeMapHolder * NewNativeMapHolder(LPVOID address, SIZE_T size, 
                                     SIZE_T offset, long init_ref_count);
static LONG NativeMapHolderAddRef(NativeMapHolder * thisPMH);
static LONG NativeMapHolderRelease(NativeMapHolder * thisPMH);
static PMAPPED_VIEW_LIST FindSharedMappingReplacement(struct stat * pstat, 
                                                      SIZE_T size, SIZE_T offset);
#endif

#if !HAVE_MMAP_DEV_ZERO
/* We need MAP_ANON. However on some platforms like HP-UX, it is defined as MAP_ANONYMOUS */
#if !defined(MAP_ANON) && defined(MAP_ANONYMOUS)
#define MAP_ANON MAP_ANONYMOUS
#endif
#endif

#if defined _DEBUG
static void MAPDisplayData( void );
#endif

/*++
Function:
  CreateFileMappingA

Note:
  File mapping are used to do inter-process communication.

See MSDN doc.
--*/
HANDLE
PALAPI
CreateFileMappingA(
                   IN HANDLE hFile,
                   IN LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
                   IN DWORD flProtect,
                   IN DWORD dwMaximumSizeHigh,
                   IN DWORD dwMaximumSizeLow,
                   IN LPCSTR lpName)
{
    HANDLE hRetVal = NULL;
    WCHAR WideString[ MAX_PATH ];

    PERF_ENTRY(CreateFileMappingA);
    ENTRY("CreateFileMappingA(hFile=%p, lpAttributes=%p, flProtect=%#x, "
          "dwMaxSizeH=%d, dwMaxSizeL=%d, lpName=%p (%s))\n",
          hFile, lpFileMappingAttributes, flProtect, 
          dwMaximumSizeHigh, dwMaximumSizeLow,
          lpName?lpName:"NULL",
          lpName?lpName:"NULL");

    if ( lpName != NULL )
    {
        if ( 0 == MultiByteToWideChar(CP_ACP, 0, lpName, -1, 
                                      WideString, MAX_PATH ) )
        {
            DWORD dwLastError = GetLastError();
            if ( ERROR_INSUFFICIENT_BUFFER == dwLastError )
            {
                ERROR("lpName is larger than MAX_PATH (%d)!\n", MAX_PATH);
            }
            else
            {
                ERROR("MultiByteToWideChar failure! (error=%d)\n", 
                   dwLastError);
            }
            SetLastError(ERROR_INVALID_PARAMETER);
            goto ExitCreateFileMappingA;
        }

        hRetVal = CreateFileMappingW( hFile, lpFileMappingAttributes, 
                                      flProtect, dwMaximumSizeHigh, 
                                      dwMaximumSizeLow, WideString );
    }
    else
    {
        hRetVal = CreateFileMappingW( hFile, lpFileMappingAttributes, 
                                      flProtect, dwMaximumSizeHigh, 
                                      dwMaximumSizeLow, NULL );
    }

ExitCreateFileMappingA:
    LOGEXIT( "CreateFileMappingA returns HANDLE %p. \n", hRetVal );
    PERF_EXIT(CreateFileMappingA);
    return hRetVal;
}

/*++
Function:
  CreateFileMappingW

Note:
  File mapping are used to do inter-process communication.

See MSDN doc.
--*/
HANDLE
PALAPI
CreateFileMappingW(
               IN HANDLE hFile,
               IN LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
               IN DWORD flProtect,
               IN DWORD dwMaximumSizeHigh,
               IN DWORD dwMaximumSizeLow,
               IN LPCWSTR lpName)
{
    HANDLE hRetVal = NULL;
    struct stat UnixFileInformation;
    LPSTR lpFileName = NULL;
    INT UnixFd = -1;
    BOOL bPALCreatedTempFile = FALSE;
    PGLOBAL_FILE_MAPPING_OBJECT lpKernelObject = NULL;
    UINT nFileSize = 0;
    LPCWSTR lpObjectName = NULL;
    WCHAR MangledObjectName[MAX_PATH];

    PERF_ENTRY(CreateFileMappingW);
    ENTRY("CreateFileMappingW(hFile=%p, lpAttributes=%p, flProtect=%#x, "
          "dwMaxSizeH=%u, dwMaxSizeL=%u, lpName=%p (%S))\n",
          hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, 
          dwMaximumSizeLow, lpName?lpName:W16_NULLSTRING, lpName?lpName:W16_NULLSTRING);

    if ( 0 != dwMaximumSizeHigh )
    {
        ASSERT( "dwMaximumSizeHigh is always 0.\n" );
        SetLastError( ERROR_INVALID_PARAMETER );
        goto ExitCreateFileMappingW;
    }

    if ( lpFileMappingAttributes != NULL )
    {
        ASSERT( "lpFileMappingAttributes are always NULL.\n" );
        SetLastError( ERROR_INVALID_PARAMETER );
        goto ExitCreateFileMappingW;
    }

    if ( PAGE_READWRITE != flProtect && PAGE_READONLY != flProtect &&
         PAGE_WRITECOPY != flProtect )
    {
        ASSERT( "invalid flProtect %#x, acceptable values are PAGE_READONLY "
                "(%#x), PAGE_READWRITE (%#x) and PAGE_WRITECOPY (%#x).\n", 
                flProtect, PAGE_READONLY, PAGE_READWRITE, PAGE_WRITECOPY );
        SetLastError( ERROR_INVALID_PARAMETER );
        goto ExitCreateFileMappingW;
    }

    if ( hFile == INVALID_HANDLE_VALUE && 0 == dwMaximumSizeLow )
    {
        ERROR( "If hFile is INVALID_HANDLE_VALUE, then you must specify a size.\n" );
        SetLastError( ERROR_INVALID_PARAMETER );
        goto ExitCreateFileMappingW;
    }

    if ( hFile != INVALID_HANDLE_VALUE && NULL != lpName )
    {
        ASSERT( "If hFile is not -1, then lpName must be NULL.\n" );
        SetLastError( ERROR_INVALID_PARAMETER );
        goto ExitCreateFileMappingW;
    }

    lpObjectName=lpName;
    if (!MangleObjectNameW(&lpObjectName, MangledObjectName))
    {
        goto ExitCreateFileMappingW;
    }


    /* See if the name already belongs to another mapping object. If it 
       already exists and is a FileMapping object then request access to 
       the mapping object with the protection specified in flProtect, else 
       if it exists and is not a FileMapping object then return NULL, and 
       sets ERROR_INVALID_HANDLE.  All of this is done while holding the
       mapping_critsec and shmlock in order to guarantee that no other
       process creates the same named mapping object concurrently. */

    SYNCEnterCriticalSection( &mapping_critsec , TRUE);
    SHMLock();

    if ( lpName )
    {
        DWORD dwError = GetLastError();
        INT access = MAPConvertProtectToAccess( flProtect );

        if ( -1 != access )
        {
            hRetVal = OpenFileMappingW( access, FALSE, lpName );
        }
        else
        {
            ASSERT( "MapConvertProtectToAccess\n" );
            SetLastError( ERROR_INTERNAL_ERROR );
            goto SYNCLeaveCriticalSection;
        }
        
        if ( NULL != hRetVal )
        {
            /* Windows returns handle to the specified mapping object, yet 
               still sets this error. */
            SHMRelease();
            SYNCLeaveCriticalSection( &mapping_critsec, TRUE);
            SetLastError( ERROR_ALREADY_EXISTS );
            TRACE( "Returning the duplicate object with the desired access rights.\n" );
            goto QuickExit;
        }
        else
        {
            /* If the error code is ERROR_INVALID_HANDLE then
            there is already a non-mapping object in the system with that name. 
            Else no object exists with that name. Does not exist. */
            if ( ERROR_INVALID_HANDLE == GetLastError() )
            {
                SHMRelease();
                SYNCLeaveCriticalSection( &mapping_critsec, TRUE);
                hRetVal = NULL;
                SetLastError( ERROR_INVALID_HANDLE );
                goto QuickExit;
            }
            SetLastError( dwError );
        }
    }

    if (hFile == INVALID_HANDLE_VALUE && !lpName)
    {
        /* Anonymous mapped files. */
        lpFileName = strdup( "/dev/zero" );

        if ( !lpFileName )
        {
            ERROR( "strdup() failed!\n" );
            SetLastError( ERROR_NOT_ENOUGH_MEMORY );
            goto SYNCLeaveCriticalSection;
        }

#if HAVE_MMAP_DEV_ZERO

        UnixFd = open( lpFileName, O_RDWR );
        if ( -1 == UnixFd )
        {
            ERROR( "Unable to open the file.\n");
            SetLastError( ERROR_INTERNAL_ERROR );
            goto SYNCLeaveCriticalSection;
        }

#else //!HAVE_MMAP_DEV_ZERO

        UnixFd = -1;  /* will pass MAP_ANON to mmap() instead */

#endif //!HAVE_MMAP_DEV_ZERO

    } else {
        if ( hFile != INVALID_HANDLE_VALUE )
        {
            file * pFileStruct = FILEAcquireFileStruct( hFile );
            if ( !pFileStruct )
            {
                ERROR( "Unable to lock the file.\n" );
                SetLastError( ERROR_INVALID_PARAMETER );
                goto SYNCLeaveCriticalSection;
            }
        
            /* We need to check to ensure flProtect jives with 
               the permission on the file handle */
            if ( !MAPIsRequestPermissible( flProtect, pFileStruct ) )
            {
                ERROR("File handle does not have the correct "
                      "permissions to create mapping\n" );
                SetLastError( ERROR_ACCESS_DENIED );
                FILEReleaseFileStruct( hFile, pFileStruct );
                goto SYNCLeaveCriticalSection;
            }

            /* duplicate the handle, file mapping should still be
               accessible even if the file is closed */
            UnixFd = dup(pFileStruct->unix_fd);
            FILEReleaseFileStruct( hFile, pFileStruct );
            if ( -1 == UnixFd )
            {
                ERROR( "Unable to duplicate the Unix file descriptor!\n" );
                SetLastError( ERROR_INTERNAL_ERROR );
                goto SYNCLeaveCriticalSection;
            }
  
            lpFileName = strdup(pFileStruct->unix_filename);
            if ( NULL == lpFileName )
            {
                ERROR( "strdup() failed!\n" );
                SetLastError( ERROR_NOT_ENOUGH_MEMORY );
                close( UnixFd );
                goto SYNCLeaveCriticalSection;
            }
        } 
        else 
        {
            TRACE( "INVALID_HANDLE_VALUE was the hFile, time to try to create a "
                   "temporary file" );

            /* Create a temporary file on the filesystem in order to be 
               shared across processes. */
            if ( NULL == ( lpFileName = MAPCreateTempFile( &UnixFd ) ) )
            {
                /* I'll let the helper function set the error code. */
                ERROR( "Unable to create the temporary file.\n" );
                goto SYNCLeaveCriticalSection;
            }
            bPALCreatedTempFile = TRUE;
        }
    
        if ( -1 == fstat( UnixFd, &UnixFileInformation ) )
        {
            ASSERT( "fstat() failed for this reason %s.\n", strerror( errno ) );
            SetLastError( ERROR_INTERNAL_ERROR );
            close( UnixFd );
            goto SYNCLeaveCriticalSection;
        }

        if ( 0 == UnixFileInformation.st_size && 
             0 == dwMaximumSizeHigh && 0 == dwMaximumSizeLow )
        {
            ERROR( "The file cannot be a zero length file.\n" );
            SetLastError( ERROR_FILE_INVALID );
            close( UnixFd );
            goto SYNCLeaveCriticalSection;
        }

        if ( INVALID_HANDLE_VALUE != hFile && 
             dwMaximumSizeLow > UnixFileInformation.st_size && 
             ( PAGE_READONLY == flProtect || PAGE_WRITECOPY == flProtect ) )
        {
            /* In this situation, Windows returns an error, because the
               permissions requested do not allow growing the file */
            ERROR( "The file cannot be grown do to the map's permissions.\n" );
            SetLastError( ERROR_NOT_ENOUGH_MEMORY );
            close( UnixFd );
            goto SYNCLeaveCriticalSection;
        }
      
        if ( UnixFileInformation.st_size < dwMaximumSizeLow )
        {
            TRACE( "Growing the size of file on disk to match requested size.\n" );

            /* Need to grow the file on disk to match size. */
            if ( !MAPGrowLocalFile( UnixFd, dwMaximumSizeLow ) )
            {
                ERROR( "Unable to grow the file on disk.\n" );
                goto SYNCLeaveCriticalSection;
            }
        }
    }

    /* Create the kernel object information, 
    depending on whether or not the object is named,
    the kernel object will be in either shared memory
    or allocated off the heap. */
    nFileSize = ( 0 == dwMaximumSizeLow && 0 == dwMaximumSizeHigh ) ? 
        UnixFileInformation.st_size : dwMaximumSizeLow;
    
    lpKernelObject = MAPCreateGMMO( lpFileName, lpObjectName, flProtect, 
                                    nFileSize, bPALCreatedTempFile );
    if ( lpKernelObject )
    {
        INT access = MAPConvertProtectToAccess( flProtect );
        hRetVal = MAPAddHandle( lpKernelObject, access, UnixFd );
        if ( INVALID_HANDLE_VALUE == hRetVal )
        {
            ASSERT( "The handle manager failed to return a real handle.\n" );
            SetLastError( ERROR_INTERNAL_ERROR );
            if (UnixFd != -1)
            {
                close( UnixFd );
            }
            hRetVal = NULL;
            goto SYNCLeaveCriticalSection;
        }
    }
    else
    {
        ASSERT( "The handle manager failed to return a real handle.\n" );
        SetLastError( ERROR_INTERNAL_ERROR );
        if (UnixFd != 1)
        {
            close( UnixFd );
        }
    }
    
SYNCLeaveCriticalSection:

    SHMRelease();
    SYNCLeaveCriticalSection( &mapping_critsec, TRUE);

ExitCreateFileMappingW:

    if( NULL == hRetVal && bPALCreatedTempFile && lpFileName )
    {
        unlink(lpFileName);
    }

    if ( lpFileName )
    {
        free( lpFileName );
    }

#if defined _DEBUG
    MAPDisplayData();
#endif

QuickExit:
    LOGEXIT( "CreateFileMappingW returning %p .\n", hRetVal );
    PERF_EXIT(CreateFileMappingW);
    return hRetVal;
}

/*++
Function:
  OpenFileMappingA

See MSDN doc.
--*/
HANDLE
PALAPI
OpenFileMappingA(
         IN DWORD dwDesiredAccess,
         IN BOOL bInheritHandle,
         IN LPCSTR lpName)
{
    HANDLE hRetVal = NULL;
    WCHAR WideString[ MAX_PATH ];

    PERF_ENTRY(OpenFileMappingA);
    ENTRY("OpenFileMappingA(dwDesiredAccess=%u, bInheritHandle=%d, lpName=%p (%s)\n",
          dwDesiredAccess, bInheritHandle, lpName?lpName:"NULL", lpName?lpName:"NULL");

    if ( lpName != NULL )
    {
        if ( 0 == MultiByteToWideChar(CP_ACP, 0, lpName, -1, 
                                      WideString, MAX_PATH ) )
        {
            DWORD dwLastError = GetLastError();
            if ( ERROR_INSUFFICIENT_BUFFER == dwLastError )
            {
                ERROR("lpName is larger than MAX_PATH (%d)!\n", MAX_PATH);
            }
            else
            {
                ERROR("MultiByteToWideChar failure! (error=%d)\n", 
                   dwLastError);
            }
            SetLastError(ERROR_INVALID_PARAMETER);
            goto ExitOpenFileMappingA;
        }

        hRetVal = OpenFileMappingW( dwDesiredAccess, bInheritHandle, 
                                    WideString );
    }
    else
    {
        hRetVal = OpenFileMappingW( dwDesiredAccess, bInheritHandle, NULL );
    }
    
ExitOpenFileMappingA:
    LOGEXIT( "OpenFileMappingA returning %p\n", hRetVal );
    PERF_EXIT(OpenFileMappingA);
    return hRetVal;
}


/*++
Function:
  OpenFileMappingW

See MSDN doc.
--*/
HANDLE
PALAPI
OpenFileMappingW(
         IN DWORD dwDesiredAccess,
         IN BOOL bInheritHandle,
         IN LPCWSTR lpName)
{
    HANDLE hRetVal = NULL;
    PGLOBAL_FILE_MAPPING_OBJECT lpMapping = NULL;
    SHMPTR shmMapping = 0;
    LPCWSTR lpObjectName = NULL;
    WCHAR MangledObjectName[MAX_PATH];
    BOOL nameExists;

    PERF_ENTRY(OpenFileMappingW);
    ENTRY("OpenFileMappingW(dwDesiredAccess=%#x, bInheritHandle=%d, lpName=%p (%S)\n",
          dwDesiredAccess, bInheritHandle, lpName?lpName:W16_NULLSTRING, lpName?lpName:W16_NULLSTRING);

    if ( MAPContainsInvalidFlags( dwDesiredAccess ) ) 
    {
        ASSERT( "dwDesiredAccess can be one or more of FILE_MAP_READ, " 
               "FILE_MAP_WRITE, FILE_MAP_COPY or FILE_MAP_ALL_ACCESS.\n" );
        SetLastError( ERROR_INVALID_PARAMETER );
        goto ExitOpenFileMappingW;
    }

    lpObjectName=lpName;
    if (!MangleObjectNameW(&lpObjectName, MangledObjectName))
    {
        goto ExitOpenFileMappingW;
    }

    SYNCEnterCriticalSection( &mapping_critsec , TRUE);
    SHMLock();

    /* Check the namespace for the named mapping object. */
    shmMapping = SHMFindNamedObjectByName( lpObjectName, SHM_NAMED_MAPPINGS,
                                           &nameExists );
    if ( shmMapping )
    {
        lpMapping = (PGLOBAL_FILE_MAPPING_OBJECT)SHMPTR_TO_PTR( shmMapping ); 
    }
    
    if ( lpMapping )
    {
        /* Open the file and clone the object into this process. */
        INT UnixFd = 0;
        
        UnixFd = open( (LPSTR)SHMPTR_TO_PTR( lpMapping->fileName.shm ),
                       MAPProtectionToFileOpenFlags( lpMapping->flProtect ) );
        
        if ( -1 == UnixFd )
        {
            char * errmsg = strerror(errno);
            ASSERT( "Unable to open the file to obtain the file descriptor "
                    "[errno=%d, %s]\n", errno, errmsg ? errmsg : "");
            SetLastError( ERROR_INTERNAL_ERROR );
            goto Cleanup;
        }
        /* Clone the handle. */
        hRetVal = 
            MAPAddHandle( lpMapping,dwDesiredAccess, UnixFd );
        
        if ( NULL == hRetVal )
        {
            ASSERT( "The handle manager failed to return a real handle.\n" );
            SetLastError( ERROR_INTERNAL_ERROR );
            close( UnixFd );
            goto Cleanup;
        }

        /* Increment the shared memory object reference count. */
        lpMapping->_RefCount++;
    }
    else
    {
        if (nameExists)
        {
            SetLastError(ERROR_INVALID_HANDLE);
        }
        else
        {
            SetLastError(ERROR_FILE_NOT_FOUND);
        }
    }

Cleanup:
    SHMRelease();    
    SYNCLeaveCriticalSection( &mapping_critsec , TRUE);

ExitOpenFileMappingW:
    
#if defined _DEBUG
    MAPDisplayData();
#endif
    
    LOGEXIT( "OpenFileMappingW returning %p.\n", hRetVal );
    PERF_EXIT(OpenFileMappingW);
    return hRetVal;
}

/*++
Function:
  MapViewOfFile

  Limitations: 1) Currently file mappings are supported only at file
                  offset 0.
               2) Some platforms (specifically HP-UX) do not support
                  multiple simultaneous shared mapping of the same file
                  region in the same process. On these platforms, in case
                  we are asked for a new view completely contained in an
                  existing one, we return an address within the existing
                  mapping. In case the new requested view is overlapping
                  with the existing one, but not contained in it, the
                  mapping is impossible, and MapViewOfFile will fail.
                  Since currently the mappings are supported only at file
                  offset 0, MapViewOfFile will succeed if the new view
                  is equal or smaller of the existing one, and the address
                  returned will be the same address of the existing 
                  mapping.
                  Since the underlying mapping is always the same, all 
                  the shared views of the same file region will share the
                  same protection, i.e. they will have the largest 
                  protection requested. If any mapping asked for a
                  read-write access, all the read-only mappings of the 
                  same region will silently get a read-write access to 
                  it.

See MSDN doc.
--*/
LPVOID
PALAPI
MapViewOfFile(
          IN HANDLE hFileMappingObject,
          IN DWORD dwDesiredAccess,
          IN DWORD dwFileOffsetHigh,
          IN DWORD dwFileOffsetLow,
          IN SIZE_T dwNumberOfBytesToMap)
{
    LPVOID lpRetVal = NULL;
    PMAPPED_MEMORY_INFO pHandleObject = NULL;
    PGLOBAL_FILE_MAPPING_OBJECT kernelObject = 0;
    BOOL bret;
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
    PMAPPED_VIEW_LIST pReusedMapping = NULL;
#endif

    PERF_ENTRY(MapViewOfFile);
    ENTRY("MapViewOfFile(hFileMapping=%p, dwDesiredAccess=%u, "
          "dwFileOffsetH=%u dwFileOffsetL=%u, dwNumberOfBytes=%u\n",
          hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh,
          dwFileOffsetLow, dwNumberOfBytesToMap);

    /* Sanity checks */
    if ( MAPContainsInvalidFlags( dwDesiredAccess ) )
    {
        ASSERT( "dwDesiredAccess can be one of FILE_MAP_WRITE, FILE_MAP_READ,"
               " FILE_MAP_COPY or FILE_MAP_ALL_ACCESS.\n" );
        SetLastError( ERROR_INVALID_PARAMETER );
        goto ExitMapViewOfFile;
    }

    if ( 0 != dwFileOffsetHigh || 0 != dwFileOffsetLow )
    {
        ASSERT( "dwFileOffsetHigh and dwFileOffsetLow are always 0.\n" );
        SetLastError( ERROR_INVALID_PARAMETER );
        goto ExitMapViewOfFile;
    }

    if ( dwNumberOfBytesToMap < 0 )
    {
        ERROR( "dwNumberOfBytesToMap must be 0 or greater.\n" );
        SetLastError( ERROR_INVALID_PARAMETER );
        goto ExitMapViewOfFile;
    }

    pHandleObject = (PMAPPED_MEMORY_INFO)HMGRLockHandle2( hFileMappingObject,  HOBJ_MAP);
    if( NULL == pHandleObject)
    {
        ERROR( "Unable to lock handle %p.\n",hFileMappingObject );
        SetLastError( ERROR_INVALID_HANDLE );
        goto ExitMapViewOfFile;
    }                                          

    SYNCEnterCriticalSection( &mapping_critsec , TRUE);
    
    /* Map the file.... */
    if ( pHandleObject->bIsNamed )
    {
        kernelObject = 
            (PGLOBAL_FILE_MAPPING_OBJECT)
                SHMPTR_TO_PTR( pHandleObject->kernelObject.shm );
    }
    else
    {
        kernelObject = pHandleObject->kernelObject.lp;
    }

    if ( !kernelObject )
    {
        ERROR( "Unable to find the map object.\n" );
        SetLastError( ERROR_INVALID_HANDLE );
        goto SYNCLeaveCriticalSection;
    }

    /* If dwNumberOfBytesToMap is 0, we need to map the entire file.
     * mmap doesn't do the same thing as Windows in that case, though,
     * so we use the file size instead. */
    if (0 == dwNumberOfBytesToMap)
    {
        dwNumberOfBytesToMap = kernelObject->MaxSize;
    }

    if ( !MAPDesiredAccessAllowed( kernelObject->flProtect, dwDesiredAccess, 
                                   pHandleObject->dwDesiredAccessWhenOpened ) )
    {
        /* Traces and last error done in function. */
        TRACE( "The parameters are conflicting.\n" );
        goto SYNCLeaveCriticalSection;
    }


    if( FILE_MAP_COPY == dwDesiredAccess )
    {
        int flags = MAP_PRIVATE;

#if !HAVE_MMAP_DEV_ZERO
        if (pHandleObject->UnixFd == -1)
        {
            flags |= MAP_ANON;
        }
#endif
        lpRetVal = mmap( NULL, dwNumberOfBytesToMap, PROT_READ|PROT_WRITE,
                         flags, pHandleObject->UnixFd , 0 );
    }
    else
    {
        INT prot = MAPFileMapToMmapFlags( dwDesiredAccess );
        if ( prot != -1 )
        {
            int flags = MAP_SHARED;

#if !HAVE_MMAP_DEV_ZERO
            if (pHandleObject->UnixFd == -1)
            {
                flags |= MAP_ANON;
            }
#endif

            lpRetVal = mmap( NULL, dwNumberOfBytesToMap, 
                             prot, flags, pHandleObject->UnixFd , 0 );

#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
            if ((MAP_FAILED == lpRetVal) && (ENOMEM == errno))
            {
                /* Search in list of MAPPED_MEMORY_INFO for a shared mapping 
                   with the same inode number
                */
                struct stat st;

                TRACE("Mmap() failed with errno=ENOMEM probably for multiple mapping "
                      "limitation. Searching for a replacement among existing mappings\n");

                if (0 != fstat(pHandleObject->UnixFd, &st))
                {
                    ERROR("fstat failed for fd=%d\n", pHandleObject->UnixFd);
                }
                else
                {
                    pReusedMapping = FindSharedMappingReplacement(&st, dwNumberOfBytesToMap, 0);
                    if (pReusedMapping)
                    {
                        int ret;

                        TRACE("Mapping @ %p {sz=%d offs=%d} fully "
                              "contains the requested one {sz=%d offs=%d}: reusing it\n",
                              pReusedMapping->pNMHolder->address,
                              (int)pReusedMapping->pNMHolder->size,
                              (int)pReusedMapping->pNMHolder->offset,
                              dwNumberOfBytesToMap, 0);

                        /* Let's check the mapping's current protection */
                        ret = mprotect(pReusedMapping->pNMHolder->address,
                                       pReusedMapping->pNMHolder->size,
                                       prot | PROT_CHECK);
                        if (0 != ret)
                        {               
                            /* We need to raise the protection to the desired 
                               one. That will give write access to any read-only
                               mapping sharing this native mapping, but there is 
                               no way around this problem on systems that do not
                               allow more than one mapping per file region, per
                               process */
                            TRACE("Raising protections on mapping @ %p to 0x%x\n",
                                  pReusedMapping->pNMHolder->address, prot);
                            ret = mprotect(pReusedMapping->pNMHolder->address,
                                       pReusedMapping->pNMHolder->size,
                                       prot);
                        }

                        if (ret != 0) 
                        {
                            ERROR( "Failed setting protections on reused mapping\n");

                            NativeMapHolderRelease(pReusedMapping->pNMHolder);
                            free (pReusedMapping);
                            pReusedMapping = NULL;
                        }
                    }
                }
            }
#endif // ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
        }
        else
        {
            ASSERT( "MapFileMapToMmapFlags failed!\n" );
            SetLastError( ERROR_INTERNAL_ERROR );
            lpRetVal = NULL;
            goto SYNCLeaveCriticalSection;
        }
    }

    if ( (MAP_FAILED == lpRetVal)
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
         &&  (pReusedMapping == NULL)
#endif // ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
        )
    {
        ERROR( "mmap failed with code %s.\n", strerror( errno ) );
        SetLastError( ERROR_NOT_ENOUGH_MEMORY );
        lpRetVal = NULL;
        goto SYNCLeaveCriticalSection;

    }

#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
    if (pReusedMapping != NULL)
    {
        bret = MAPAddThisViewToObject(hFileMappingObject, pReusedMapping);
        lpRetVal = pReusedMapping->lpAddress;
    }
    else 
#endif // ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
        bret = MAPAddNewViewToObject( hFileMappingObject, dwDesiredAccess,
                                      dwNumberOfBytesToMap, lpRetVal );
    if (!bret)
    {
        ASSERT( "Unable to add the view to the list.\n" );
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
        if (pReusedMapping != NULL)
        {
            NativeMapHolderRelease(pReusedMapping->pNMHolder);
            free (pReusedMapping);
            pReusedMapping = NULL;
        }
        else
#endif // ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
        if ( -1 == munmap( lpRetVal, dwNumberOfBytesToMap ) )
        {
            ERROR( "Unable to unmap the file. Expect trouble.\n" );
        }
        lpRetVal = NULL;
        SetLastError( ERROR_INTERNAL_ERROR );
        goto SYNCLeaveCriticalSection;
    }
    
    TRACE( "Added %p to the list.\n", lpRetVal );

#if defined _DEBUG
    MAPDisplayData();
#endif

SYNCLeaveCriticalSection:

    SYNCLeaveCriticalSection( &mapping_critsec , TRUE);

ExitMapViewOfFile:
    if(NULL!=pHandleObject)
    {
        HMGRUnlockHandle(hFileMappingObject, &pHandleObject->HandleData);
    }

    LOGEXIT( "MapViewOfFile returning %p.\n", lpRetVal );
    PERF_EXIT(MapViewOfFile);
    return lpRetVal;
}


/*++
Function:
  UnmapViewOfFile

See MSDN doc.
--*/
BOOL
PALAPI
UnmapViewOfFile(
        IN LPCVOID lpBaseAddress)
{
    PMAPPED_MEMORY_INFO pObject = NULL;
    BOOL bRetVal = FALSE;
    
    PERF_ENTRY(UnmapViewOfFile);
    ENTRY("UnmapViewOfFile(lpBaseAddress=%p)\n", lpBaseAddress);
    SYNCEnterCriticalSection( &mapping_critsec , TRUE);

    pObject = MAPGetViewsObject( (LPVOID)lpBaseAddress );
    if ( !pObject )
    {
        ERROR( "lpBaseAddress has to be the address returned by CreateFileMapping"
               " Or OpenFileMapping.\n" );
        SetLastError( ERROR_INVALID_HANDLE );
        bRetVal = FALSE;
        goto Exit;
    }
    
    if ( !MAPRemoveViewFromObject( pObject, (LPVOID)lpBaseAddress ) )
    {
        ASSERT( "Unable to remove view from the mapping object.\n" );
        SetLastError( ERROR_INTERNAL_ERROR );                       
        bRetVal = FALSE;
        goto Exit;
    }
    
    bRetVal = TRUE;
#if defined _DEBUG
    MAPDisplayData();
#endif

Exit:
    SYNCLeaveCriticalSection( &mapping_critsec , TRUE);
    LOGEXIT( "UnmapViewOfFile returning %s.\n", bRetVal ? "TRUE" : "FALSE" );
    PERF_EXIT(UnmapViewOfFile);
    return bRetVal;
}

/*++
Function:
  MAPDeleteSharedObject

    Release the shared memory pointer associated with a mapping object, along 
    with all pointers it contains
                  
Parameters:
    SHMPTR shmPtr : shared memory object to release
                                                                   
(no return value)
--*/
static void MAPDeleteSharedObject(SHMPTR shmPtr)
{
    GLOBAL_FILE_MAPPING_OBJECT *pShmMapping;

    if ( 0 == shmPtr || NULL == ( pShmMapping = SHMPTR_TO_PTR( shmPtr ) ) )
    {
        ERROR( "Mapping object's shared memory pointer (%#x)" 
                " was invalid!\n", shmPtr);
        return;
    }
    
    SHMRemoveNamedObject( shmPtr );
    TRACE( "****NAME***********Freeing %d\n", pShmMapping->_ShmHeader.ShmObjectName );
    SHMfree( pShmMapping->_ShmHeader.ShmObjectName );
    
    if( 0 != pShmMapping->fileName.shm )
    {
        
        SHMfree( pShmMapping->fileName.shm );
        TRACE( "***************FILENAME********Freeing %d\n", pShmMapping->fileName.shm );
    }
    
    TRACE( "*********** ITSELF ******* Freeing %d\n", shmPtr );
    SHMfree( shmPtr );
}


/*++
Function :
    MAPInitialize

    Initialize the critical sections.

Return value:
    TRUE if initialization succeeded
    FALSE otherwise
--*/
BOOL
MAPInitialize( void )
{
    TRACE( "Initialising the critical section.\n" );

    if (0 != SYNCInitializeCriticalSection( &mapping_critsec ))
    {
        ERROR( "Failed initialising the critical section.\n" );
        return FALSE;
    }

    pMap = NULL;

    return TRUE;
}

/*++
Function :
    MAPCleanup

    Deletes the critical sections. And all other necessary cleanup.

Note:
    This function is called after the handle manager is stopped. So
    there shouldn't be any call that will cause an access to the handle
    manager.

--*/
void MAPCleanup( void )
{
    TRACE( "Deleting the critical section.\n" );
    DeleteCriticalSection( &mapping_critsec );
}


/*++
Function :
    MAPGetViewObject

    Returns the owning object if the address matches one of the mapped views.
--*/
static PMAPPED_MEMORY_INFO MAPGetViewsObject( LPVOID lpAddress )
{
    PMAPPED_MEMORY_INFO pObject = NULL;

    if ( NULL == lpAddress )
    {
        ERROR( "lpAddress cannot be NULL\n" );
        return NULL;
    }

    pObject = pMap;

    while ( pObject )
    {
        PMAPPED_VIEW_LIST pView = pObject->pViewList;
        while ( pView )
        {
            if ( pView->lpAddress == lpAddress )
            {
                return pObject;
            }
            pView = pView->pNext;
        }
        pObject = pObject->pNext;
    }
    WARN( "No match found.\n" );
    
    return NULL;
}
/*++
Function :

    MAPDesiredAccessAllowed

    Determines if desired access is allowed based on the protection state.

    if dwDesiredAccess conflicts with flProtect then the error is
    ERROR_INVALID_PARAMETER, if the dwDesiredAccess conflicts with
    dwDesiredAccessWhenOpened, then the error code is ERROR_ACCESS_DENIED
--*/
static BOOL MAPDesiredAccessAllowed( DWORD flProtect,
                                     DWORD dwUserDesiredAccess,
                                     DWORD dwDesiredAccessWhenOpened )
{
    TRACE( "flProtect=%d, dwUserDesiredAccess=%d, dwDesiredAccessWhenOpened=%d\n",
           flProtect, dwUserDesiredAccess, dwDesiredAccessWhenOpened );

    /* check flProtect parameters*/
    if ( FILE_MAP_READ!= dwUserDesiredAccess && PAGE_READONLY == flProtect )
    {
        ERROR( "map object is read-only, can't map a view with write access\n");
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if ( FILE_MAP_WRITE == dwUserDesiredAccess && PAGE_READWRITE != flProtect )
    {
        ERROR( "map object not open read-write, can't map a view with write "
               "access.\n" );
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if ( FILE_MAP_COPY == dwUserDesiredAccess  && PAGE_WRITECOPY != flProtect )
    {
        ERROR( "map object not open for copy-on-write, can't map copy-on-write "
               "view.\n" );
        SetLastError( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    
    /* Check to see we don't confict with the desired access we
    opened the mapping object with. */
    if ( ( dwUserDesiredAccess == FILE_MAP_READ ) &&
        !( ( dwDesiredAccessWhenOpened == FILE_MAP_READ ) || 
           ( dwDesiredAccessWhenOpened == FILE_MAP_ALL_ACCESS ) ) ) 
    {
        ERROR( "dwDesiredAccess conflict : read access requested, object not "
               "opened with read access.\n" );
        SetLastError( ERROR_ACCESS_DENIED );
        return FALSE;
    }
    if ( ( dwUserDesiredAccess & FILE_MAP_WRITE ) &&
        !( ( dwDesiredAccessWhenOpened == FILE_MAP_WRITE ) || 
           ( dwDesiredAccessWhenOpened == FILE_MAP_ALL_ACCESS ) ) ) 
    {
        ERROR( "dwDesiredAccess conflict : write access requested, object not "
               "opened with write access.\n" );
        SetLastError( ERROR_ACCESS_DENIED );
        return FALSE;
    }
    if ( ( dwUserDesiredAccess == FILE_MAP_COPY ) &&
        !( dwDesiredAccessWhenOpened == FILE_MAP_COPY ) )
    {
        ERROR( "dwDesiredAccess conflict : copy-on-write access requested, "
               "object not opened with copy-on-write access.\n" );
        SetLastError( ERROR_ACCESS_DENIED );
        return FALSE;
    }

    return TRUE;
}

/*++
Function :
    MAPConvertProtectToAccess

    Converts the PAGE_READONLY type flags to FILE_MAP_READ flags.

--*/
static DWORD MAPConvertProtectToAccess( DWORD flProtect )
{
    if ( PAGE_READONLY == flProtect )
    {
        return FILE_MAP_READ;
    }
    if ( PAGE_READWRITE == flProtect )
    {
        return FILE_MAP_ALL_ACCESS;
    }
    if ( PAGE_WRITECOPY == flProtect )
    {
        return FILE_MAP_COPY;
    }

    ASSERT( "Unknown flag for flProtect. This line "
            "should not have been executed.\n " );
    return -1;
}
/*++
Function :
    MAPFileMapToMmapFlags

    Converts the mapping flags to unix protection flags.
--*/
static INT MAPFileMapToMmapFlags( DWORD flags )
{
    if ( FILE_MAP_READ == flags )
    {
        TRACE( "FILE_MAP_READ\n" );
        return PROT_READ;
    }
    else if ( FILE_MAP_WRITE == flags )
    {
        TRACE( "FILE_MAP_WRITE\n" );
        /* The limitation of x86 archetecture
        means you cant have writable but not readable
        page. In Windows maps of FILE_MAP_WRITE can still be
        read from. */
        return PROT_WRITE | PROT_READ;
    }
    else if ( (FILE_MAP_READ|FILE_MAP_WRITE) == flags )
    {
        TRACE( "FILE_MAP_READ|FILE_MAP_WRITE\n" );
        return PROT_READ | PROT_WRITE;
    }
    else if( FILE_MAP_COPY == flags)
    {
        TRACE( "FILE_MAP_COPY\n");
        return PROT_READ | PROT_WRITE;
    } 

    ASSERT( "Unknown flag. This line should not have been executed.\n " );
    return -1;
}

#if defined _DEBUG
/*++
Function :

    MAPDisplayData

    Displays the per process map handle objects.
--*/
static void MAPDisplayData( void )
{
    PMAPPED_MEMORY_INFO lpMappedInfo = NULL;
    PGLOBAL_FILE_MAPPING_OBJECT lpKernelObject = 0;
    PMAPPED_VIEW_LIST lpViewList = NULL;
    
    DBGOUT( "\n================== Filemapping data ==================\n" );

    SYNCEnterCriticalSection( &mapping_critsec , TRUE);
    SHMLock();

    /* Get the start of the list and iterate through. */
    lpMappedInfo = pMap;
    while( lpMappedInfo )
    {
        UINT count = 0;
        
        if ( lpMappedInfo->bIsNamed )
        {
            lpKernelObject = (PGLOBAL_FILE_MAPPING_OBJECT)SHMPTR_TO_PTR( 
                lpMappedInfo->kernelObject.shm );
        }
        else
        {
            lpKernelObject = lpMappedInfo->kernelObject.lp;
        }
        
        if ( lpKernelObject ) 
        {
            DBGOUT( "KernelObject Information : \n" );
            if ( lpMappedInfo->bIsNamed )
            {
                DBGOUT( "\t\tThis handle object repersents the mapping named \t%S.\n", 
                        (LPWSTR)SHMPTR_TO_PTR( lpKernelObject->_ShmHeader.ShmObjectName ) );
                
                DBGOUT( "\t\tShmSelf %d \n", lpKernelObject->_ShmHeader.ShmSelf );
                DBGOUT( "\t\t_RefCount %d \n", lpKernelObject->_RefCount );
                
                DBGOUT( "\t\tFileName %s \n", 
                        (LPSTR)SHMPTR_TO_PTR( lpKernelObject->fileName.shm ) );
            }
            else
            {
                DBGOUT( "\t\tUNNAMED MAPPING.\n" );
                DBGOUT( "\t\tFile name %s \n",  lpKernelObject->fileName.lp );
            }

            DBGOUT( "\t\tbPALCreatedFile = %s \n", 
                    lpKernelObject->bPALCreatedTempFile ? "TRUE" : "FALSE" );
            
            DBGOUT( "\t\tflProtect %d \n", lpKernelObject->flProtect );
            DBGOUT( "\t\tMax Size %d \n", lpKernelObject->MaxSize );
        }

        DBGOUT( "This handle's access is \t\t\t\t%d.\n", 
                lpMappedInfo->dwDesiredAccessWhenOpened );
        
        DBGOUT( "Number of instances of this handle \t\t\t%d.\n", 
                lpMappedInfo->HandleRefCount );
        
        DBGOUT( "Views mapped on to this handle.\t\t\t\t\n" );
        lpViewList = lpMappedInfo->pViewList;
        while ( lpViewList )
        {
            DBGOUT( "\t\t\t\t\t\tEntry %d\n", count );
            DBGOUT( "\t\t\t\t\t\tdwDesiredAccess %d\n", lpViewList->dwDesiredAccess );
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
            DBGOUT( "\t\t\t\t\t\tlpAddress %p (%p)\n", 
                    lpViewList->lpAddress,
                    lpViewList->pNMHolder->address );
            DBGOUT( "\t\t\t\t\t\tNumberOfBytesToMap %d (%d)\n", 
                    (int)lpViewList->NumberOfBytesToMap, (int)lpViewList->pNMHolder->size );
#else
            DBGOUT( "\t\t\t\t\t\tlpAddress %p \n", lpViewList->lpAddress );
            DBGOUT( "\t\t\t\t\t\tNumberOfBytesToMap %d\n", (int)lpViewList->NumberOfBytesToMap );
#endif
            DBGOUT( "\t\t\t\t\t\tNext Element %p \n", lpViewList->pNext )
            DBGOUT( "\t\t\t\t\t\tPrevious Element %p\n\n", lpViewList->pPrev );

            lpViewList = lpViewList->pNext;
            count++;
        }
        lpMappedInfo = lpMappedInfo->pNext;
    }

    SHMRelease();
    SYNCLeaveCriticalSection( &mapping_critsec , TRUE);
    return;
    
}
#endif

/*++
Function :

    MAPGrowLocalFile

    Grows the file on disk to match the specified size.
    
--*/
static BOOL MAPGrowLocalFile( INT UnixFD, UINT NewSize )
{
    BOOL bRetVal = FALSE;
    INT  TruncateRetVal = -1;
    struct stat FileInfo;
    TRACE( "Entered MapGrowLocalFile (UnixFD=%d,NewSize%d)\n", UnixFD, NewSize );

    /* ftruncate is a standard function, but the behavoir of enlarging files is
    non-standard.  So I will try to enlarge a file, and if that fails try the
    less efficent way.*/
    TruncateRetVal = ftruncate( UnixFD, NewSize );
    fstat( UnixFD, &FileInfo );

    if ( TruncateRetVal != 0 || FileInfo.st_size != NewSize )
    {
        INT OrigSize;
        CONST UINT  BUFFER_SIZE = 128;
        BYTE buf[BUFFER_SIZE];
        UINT x = 0;
        UINT CurrentPosition = 0;

        TRACE( "Trying the less efficent way.\n" );

        CurrentPosition = lseek( UnixFD, 0, SEEK_CUR );
        OrigSize = lseek( UnixFD, 0, SEEK_END );
        if ( OrigSize == -1 )
        {
            ERROR( "Unable to locate the EOF marker. Reason=%s\n",
                   strerror( errno ) );
            SetLastError(ERROR_INTERNAL_ERROR);
            goto done;
        }

        if (NewSize <= OrigSize)
        {
            return TRUE;
        }

        memset( buf, 0, BUFFER_SIZE );

        for ( x = 0; x < NewSize - OrigSize - BUFFER_SIZE; x += BUFFER_SIZE )
        {
            if ( write( UnixFD, (LPVOID)buf, BUFFER_SIZE ) == -1 )
            {
                ERROR( "Unable to grow the file. Reason=%s\n", strerror( errno ) );
                if((errno == ENOSPC) || (errno == EDQUOT))
                {
                    SetLastError(ERROR_DISK_FULL);
                }
                else
                {
                    SetLastError(ERROR_INTERNAL_ERROR);
                }
                goto done;
            }
        }
        /* Catch any left overs. */
        if ( x != NewSize )
        {
            if ( write( UnixFD, (LPVOID)buf, NewSize - OrigSize - x) == -1 )
            {
                ERROR( "Unable to grow the file. Reason=%s\n", strerror( errno ) );
                if((errno == ENOSPC) || (errno == EDQUOT))
                {
                    SetLastError(ERROR_DISK_FULL);
                }
                else
                {
                    SetLastError(ERROR_INTERNAL_ERROR);
                }
                goto done;
            }
        }

        /* restore the file pointer position */
        lseek( UnixFD, CurrentPosition, SEEK_SET );
    }

    bRetVal = TRUE;
done:
    return bRetVal;
}

/*++
Function :
    MAPContainsInvalidFlags

    Checks that only valid flags are in the parameter.
    
--*/
static BOOL MAPContainsInvalidFlags( DWORD flags )
{

    if ( (flags == FILE_MAP_READ) ||
         (flags == FILE_MAP_WRITE) ||
         (flags == FILE_MAP_ALL_ACCESS) ||
         (flags == FILE_MAP_COPY) )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*++
Function :
    MAPDuplicateMappingHandle

    Duplicates the mapping handle.

parameters, return value : see definition of DUPHANDLEFUNC in handle.h
--*/
static INT MAPDuplicateMappingHandle( HANDLE Handle, HOBJSTRUCT * pHandleData )
{
    PMAPPED_MEMORY_INFO pMapHandleInfo = NULL;
    PGLOBAL_FILE_MAPPING_OBJECT map_shmobject;
    INT retval = 0;

    TRACE( "MAPDuplicateMappingHandle( %p, %p )\n", Handle, pHandleData );
    SYNCEnterCriticalSection( &mapping_critsec , TRUE);

    pMapHandleInfo = (PMAPPED_MEMORY_INFO)pHandleData;
    /* Make sure it is a mapping handle. */
    if ( pMapHandleInfo && HOBJ_MAP == pMapHandleInfo->HandleData.type )
    {
        (pMapHandleInfo->HandleRefCount)++;
        
        /* Increment the kernel object reference counter. */
        if ( pMapHandleInfo->bIsNamed )
        {
            SHMLock();
            map_shmobject = (PGLOBAL_FILE_MAPPING_OBJECT)SHMPTR_TO_PTR( 
                pMapHandleInfo->kernelObject.shm );
        
            if( 0 != map_shmobject )
            {
                map_shmobject->_RefCount++;
            }
            else
            {
                ERROR( "SHMPTR %08x is invalid!\n",
                      pMapHandleInfo->kernelObject.shm );
                retval = -2;    
            }
            SHMRelease();
        }
        
        SYNCLeaveCriticalSection( &mapping_critsec , TRUE);
        return retval;
    }
    else
    {
        ERROR( "pHandleData %p is not a mapping object. \n", pHandleData );
        SYNCLeaveCriticalSection( &mapping_critsec , TRUE);
        return -1;
    }
}

/*++
Function :
    MAPCloseMappingHandle

    Closes the mapping handle.

parameters, return value : see definition of CLOSEHANDLEFUNC in handle.h
--*/
static INT MAPCloseMappingHandle( HOBJSTRUCT * pHandleData )
{
    PMAPPED_MEMORY_INFO pMapHandleInfo = (PMAPPED_MEMORY_INFO)pHandleData;
    
    TRACE( "Closing handle to mapping object %p.\n", pHandleData );
    SYNCEnterCriticalSection( &mapping_critsec , TRUE);
    
    /* Make sure it is a mapping handle. */
    if ( pMapHandleInfo && pMapHandleInfo->HandleData.type == HOBJ_MAP )
    {
        PGLOBAL_FILE_MAPPING_OBJECT lpMappingObject = NULL;
        PMAPPED_VIEW_LIST pView = NULL;

        SHMLock();
        if ( pMapHandleInfo->bIsNamed )
        {
            TRACE( "Converting the SHMptr to a real ptr\n" );
            lpMappingObject = 
                (PGLOBAL_FILE_MAPPING_OBJECT)
                    SHMPTR_TO_PTR( pMapHandleInfo->kernelObject.shm );
        }
        else
        {
            lpMappingObject = pMapHandleInfo->kernelObject.lp;

        }
        if( NULL == lpMappingObject )
        {
            ERROR("SHMPTR %08x is invalid!\n", 
                  pMapHandleInfo->kernelObject.shm );
            SHMRelease();
            SYNCLeaveCriticalSection( &mapping_critsec , TRUE);
            return -2;
        }

        /* Decrement the reference counters */
        (pMapHandleInfo->HandleRefCount)--;
        if ( pMapHandleInfo->bIsNamed )
        {
            lpMappingObject->_RefCount--;
        }
        
        if ( 0 == pMapHandleInfo->HandleRefCount )
        {
            /* Delete the views. */ 
            pView = pMapHandleInfo->pViewList;
            while ( pView )
            {
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
                if( 0 > NativeMapHolderRelease(pView->pNMHolder))
                {
                    ERROR( "Negative reference count releasing view holder @ %p\n",
                            pView->pNMHolder );
                    goto error;
                }
#else
                if ( -1 == munmap( pView->lpAddress, pView->NumberOfBytesToMap ) )
                {
                    ASSERT( "Unable to unmap memory. Error=%s.\n",
                            strerror( errno ) );
                    goto error;
                }
                else
                {
                    TRACE( "Successfully unmapped %p \n", pView->lpAddress );
                }
#endif
                if ( pView->pNext )
                {
                    pView = pView->pNext;
                    free( pView->pPrev );
                    pView->pPrev = NULL;
                }
                else
                {
                    /* Last entry. */
                    free( pView );
                    pView = NULL;
                }
            }

            if (pMapHandleInfo->UnixFd != -1)
            {
                int ret = close(pMapHandleInfo->UnixFd);
                if (ret)
                {
                    ERROR ("Failed to close mapping underlying file [ret=%d errno=%d]\n", 
                           ret, errno);
                }
            }

            if ( 0 == lpMappingObject->_RefCount )
            {
                /* Delete the file. */
                if ( lpMappingObject->bPALCreatedTempFile )
                {
                    LPSTR fileName = NULL;

                    if ( pMapHandleInfo->bIsNamed )
                    {
                        fileName = (LPSTR)SHMPTR_TO_PTR( 
                            lpMappingObject->fileName.shm );
                    }
                    else
                    {
                        fileName = lpMappingObject->fileName.lp;
                    }

                    if ( -1 == unlink( fileName ) )
                    {
                        ERROR( "Unable to delete the file.\n" );
                    }
                }

                /* Delete the memory. */
                if ( pMapHandleInfo->bIsNamed )
                {
                    TRACE( "++++++++++ FREEING THE SHARED GMMO MEMORY\n\n" );
                    MAPDeleteSharedObject( lpMappingObject->_ShmHeader.ShmSelf );
                    lpMappingObject->_ShmHeader.ShmSelf = 0;
                }
                else
                {
                    /* Cleanup the heap allocated structures, since this 
                    is unnamed, the memory mapping file would have been mapped
                    against /dev/zero, and we don't want to unlink that. */
                    TRACE( "++++++++++ FREEING THE HEAP GMMO MEMORY\n\n" );
                    free( lpMappingObject->fileName.lp );
                    lpMappingObject->fileName.lp = NULL;
                    
                    free( pMapHandleInfo->kernelObject.lp );
                    pMapHandleInfo->kernelObject.lp = NULL;
                }
            }

            /* Patch the file handle list. */
            if ( pMapHandleInfo->pPrev )
            {
                pMapHandleInfo->pPrev->pNext = pMapHandleInfo->pNext;
            }
            if ( pMapHandleInfo->pNext )
            {
                pMapHandleInfo->pNext->pPrev = pMapHandleInfo->pPrev;
            }
            
            if ( pMapHandleInfo == pMap)
            {
                pMap=pMapHandleInfo->pNext;
            }

            TRACE( "++++++++++ FREEING THE HANDLE MEMORY\n\n" );
            free( pMapHandleInfo );
            pMapHandleInfo = NULL;
        }
error:
        SHMRelease();
        SYNCLeaveCriticalSection( &mapping_critsec , TRUE);
        
#if defined _DEBUG
        MAPDisplayData();
#endif
        return 0;
    }
    else
    {
        ASSERT( "pHandleData %p is not a mapping object. \n", pHandleData );
        SYNCLeaveCriticalSection( &mapping_critsec , TRUE);
        return -1;
    }
}

/*++
Function :
    MAPCreateGMMO

    Adds the file mapping object to the shared memory linked list.

    Returns a PGLOBAL_FILE_MAPPING_OBJECT, NULL otherwise.
    
    NOTE: The caller must own the critical section.
--*/
static PGLOBAL_FILE_MAPPING_OBJECT MAPCreateGMMO( 
    LPSTR lpFileName,
    LPCWSTR lpObjectName,
    DWORD flProtect,
    DWORD dwMaximumSize,
    BOOL  bPALCreatedTempFile )
{
    SHMPTR shmKernelObject = 0;
    PGLOBAL_FILE_MAPPING_OBJECT lpKernelObject = NULL;

    if ( lpObjectName )
    {
        /* Named objects go into shared memory. */
        shmKernelObject = SHMalloc( sizeof( GLOBAL_FILE_MAPPING_OBJECT ) );
        if ( 0 == shmKernelObject )
        {
            ERROR( "Unable to allocate shared memory\n" );
            goto exit;
        }
        /* Make the SHMPTR useful */
        lpKernelObject = (PGLOBAL_FILE_MAPPING_OBJECT)
                    SHMPTR_TO_PTR( shmKernelObject );

    }
    else
    {
        /* Shared memory is not needed. */
        lpKernelObject = (PGLOBAL_FILE_MAPPING_OBJECT)
                    malloc( sizeof( GLOBAL_FILE_MAPPING_OBJECT  ) );
    }
        
    if ( !lpKernelObject )
    {
        ERROR( "Unable to create the GLOBAL_FILE_MAPPING_OBJECT structure.\n" );
        goto exit;
    }
    
    /* Set the shared memory specific fields. */
    if ( lpObjectName )
    {
        /* Header information. */
        lpKernelObject->_ShmHeader.ObjectType = SHM_NAMED_MAPPINGS;
        lpKernelObject->_ShmHeader.ShmSelf = shmKernelObject;
        
        /* Filename and Object name */
        lpKernelObject->fileName.shm = SHMStrDup( lpFileName );
        lpKernelObject->_ShmHeader.ShmObjectName = SHMWStrDup( lpObjectName );

        if ( (0 == lpKernelObject->fileName.shm) || 
             (0 == lpKernelObject->_ShmHeader.ShmObjectName) )
        {
            ERROR( "Unable to allocate shared memory!\n" );
            if (0 != lpKernelObject->fileName.shm) 
                SHMfree(lpKernelObject->fileName.shm);
            if (0 != lpKernelObject->_ShmHeader.ShmObjectName)
                SHMfree(lpKernelObject->_ShmHeader.ShmObjectName);
            SHMfree( shmKernelObject );
            lpKernelObject = NULL;
            goto exit;
        }

        /* Everytime a handle is created, or duplicated, 
        this reference count should be incremented. But only
        if is is a shared memory ptr.*/
        lpKernelObject->_RefCount = 1;

        SHMAddNamedObject( shmKernelObject );
    }
    else
    {
        /* Non-shared mapping. */
        LPSTR lpTemp = strdup( lpFileName );
        if ( lpTemp )
        {
            lpKernelObject->fileName.lp = lpTemp;
        }
        else
        {
            ERROR( "Unable to allocate memory for the file name!\n" );
            free( lpKernelObject );
            lpKernelObject = NULL;
            goto exit;
        }

        lpKernelObject->_ShmHeader.ObjectType = -1;
        lpKernelObject->_ShmHeader.ShmNext = 0;
        lpKernelObject->_ShmHeader.ShmObjectName = 0;
        lpKernelObject->_ShmHeader.ShmSelf = 0;
        lpKernelObject->_RefCount = 0;
    }

    /* Set the rest of the fields. */
    lpKernelObject->MaxSize = dwMaximumSize;
    lpKernelObject->flProtect = flProtect;
    lpKernelObject->bPALCreatedTempFile = bPALCreatedTempFile;

exit:
    TRACE( "MAPCreateGMMO returning %p.\n", lpKernelObject );
    return lpKernelObject;
}

/*++
Function :
    MAPAddNewViewToObject

    Adds a new mapping to the view of the file.
    NOTE: This function assumes the Mapping critical section is owned
          by the current thread

--*/
static BOOL MAPAddNewViewToObject( HANDLE hFileMappingObject,
                                   DWORD dwDesiredAccess,
                                   SIZE_T dwNumberOfBytesToMap,
                                   LPCVOID lpAddress )
{
    PMAPPED_VIEW_LIST pMapViewEntry = NULL; 
    PMAPPED_MEMORY_INFO pMapObject = NULL;
        
    TRACE( "MAPAddNewViewToObject( hFileMappingObject=%p, dwDesiredAccess=%d,"
           " dwNumberOfBytesToMap=%d, lpAddress=%p )\n", hFileMappingObject, 
           dwDesiredAccess, dwNumberOfBytesToMap, lpAddress );
    
    /* This function assumes the Mapping critical section is owned
       by the current thread */
    pMapObject = (PMAPPED_MEMORY_INFO)HMGRLockHandle2( hFileMappingObject, HOBJ_MAP);
    if ( pMapObject )
    {
        pMapViewEntry = pMapObject->pViewList;
    }
    else
    {
        ERROR( "Unable to retrieve the map object from the handle manager.\n" );
        return FALSE;
    }

    if ( !pMapViewEntry )
    {
        TRACE( "First entry in the list.\n" );
        pMapViewEntry = (PMAPPED_VIEW_LIST)malloc( sizeof( MAPPED_VIEW_LIST ) );

        if ( pMapViewEntry )
        {
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
            pMapViewEntry->pNMHolder = NewNativeMapHolder((LPVOID)lpAddress, 
                                                          dwNumberOfBytesToMap, 0, 1);
            if (!pMapViewEntry->pNMHolder)
            {
                ERROR("No memory for native mapping holder\n");
                free(pMapViewEntry);
                HMGRUnlockHandle(hFileMappingObject,&pMapObject->HandleData);
                return FALSE;
            }
#endif
            pMapViewEntry->lpAddress = (LPVOID)lpAddress;
            pMapViewEntry->NumberOfBytesToMap = dwNumberOfBytesToMap;
            pMapViewEntry->dwDesiredAccess = dwDesiredAccess;
            
            pMapViewEntry->pNext = NULL;
            pMapViewEntry->pPrev = NULL;


            /* for the first entry we duplicate the file mapping handle
               to ensure the file mapping object stays valid even if the
               handle is closed. This is necessary to maintain the views
               that haven't been unmapped */
            DuplicateHandle(GetCurrentProcess(),
                            hFileMappingObject,
                            GetCurrentProcess(),
                            &(pMapViewEntry->hFileMapping),
                            0,
                            FALSE,
                            DUPLICATE_SAME_ACCESS);

            if ( INVALID_HANDLE_VALUE == pMapViewEntry->hFileMapping )
            {
                ERROR("Unable to duplicate the file mapping handle\n");
                free(pMapViewEntry);
                HMGRUnlockHandle(hFileMappingObject,&pMapObject->HandleData);
                return FALSE;
            }

            pMapObject->pViewList = pMapViewEntry;
            HMGRUnlockHandle(hFileMappingObject,&pMapObject->HandleData);
            return TRUE;
        }
    }
    else
    {
        PMAPPED_VIEW_LIST pNewView =
            (PMAPPED_VIEW_LIST)malloc( sizeof( MAPPED_VIEW_LIST ) );

        TRACE( "Adding to the start of the list.\n" );

        if ( pNewView )
        {
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
            pNewView->pNMHolder = NewNativeMapHolder((LPVOID)lpAddress, 
                                                     dwNumberOfBytesToMap, 0, 1);
            if (!pNewView->pNMHolder)
            {
                ERROR("No memory for native mapping holder\n");
                free(pMapViewEntry);
                HMGRUnlockHandle(hFileMappingObject,&pMapObject->HandleData);
                return FALSE;
            }
#endif
            pNewView->lpAddress = (LPVOID)lpAddress;
            pNewView->NumberOfBytesToMap = dwNumberOfBytesToMap;
            pNewView->dwDesiredAccess = dwDesiredAccess;
            pNewView->hFileMapping = pMapViewEntry->hFileMapping;
            pNewView->pNext = pMapViewEntry;
            pNewView->pPrev = NULL;
            pMapViewEntry->pPrev = pNewView;
            pMapObject->pViewList = pNewView;
            HMGRUnlockHandle(hFileMappingObject,&pMapObject->HandleData);
            return TRUE;
        }
    }
    HMGRUnlockHandle(hFileMappingObject,&pMapObject->HandleData);
    ERROR( "Unable to allocate memory for the view structure.\n" );
    return FALSE;
}

#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
/*++
Function :
    MAPAddThisViewToObject

    Adds a new mapping to the view of the file.
    NOTE: This function assumes the Mapping critical section is owned
          by the current thread

--*/
static BOOL MAPAddThisViewToObject( HANDLE hFileMappingObject,
                                    PMAPPED_VIEW_LIST pNewView )
{
    PMAPPED_MEMORY_INFO pMapObject = NULL;
        
    TRACE( "MAPAddThisViewToObject( hFileMappingObject=%p, pThisView=%p )\n",
           hFileMappingObject, pNewView );
    
    pMapObject = (PMAPPED_MEMORY_INFO)HMGRLockHandle2( hFileMappingObject, HOBJ_MAP);
    if ( !pMapObject )
    {
        ERROR( "Unable to retrieve the map object from the handle manager.\n" );
        return FALSE;
    }

    if ( pMapObject->pViewList )
    {
        pNewView->hFileMapping = pMapObject->pViewList->hFileMapping;
    }
    else
    {
        /* for the first entry we duplicate the file mapping handle
           to ensure the file mapping object stays valid even if the
           handle is closed. This is necessary to maintain the views
           that haven't been unmapped */
        DuplicateHandle(GetCurrentProcess(),
                        hFileMappingObject,
                        GetCurrentProcess(),
                        &(pNewView->hFileMapping),
                        0,
                        FALSE,
                        DUPLICATE_SAME_ACCESS);

        if ( INVALID_HANDLE_VALUE == pNewView->hFileMapping )
        {
            ERROR("Unable to duplicate the file mapping handle\n");
            HMGRUnlockHandle(hFileMappingObject,&pMapObject->HandleData);
            return FALSE;
        }
    }

    /* Insert view in front of the list */
    pNewView->pNext = pMapObject->pViewList;
    pNewView->pPrev = NULL;
    if (pMapObject->pViewList)
        pMapObject->pViewList->pPrev = pNewView;
    pMapObject->pViewList = pNewView;

    HMGRUnlockHandle(hFileMappingObject,&pMapObject->HandleData);
    return TRUE;
}
#endif // ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS

/*++
Function :
    MAPRemoveViewFromObject

    Removes a view of a file from the handle.
--*/
static BOOL MAPRemoveViewFromObject( PMAPPED_MEMORY_INFO pObject, 
                                     LPVOID lpAddress )
{
    PMAPPED_MEMORY_INFO pMappedMemoryInfo = pObject;
    PMAPPED_VIEW_LIST pMapViewEntry = NULL;

    /* This function assumes the Mapping critical section is owned
       by the current thread */
    if ( pMappedMemoryInfo->HandleData.type != HOBJ_MAP )
    {
        ERROR( "The handle returned by the handle manager was not a file "
               "mapping handle.\n" );
        return FALSE;
    }

    /* Find the entry to remove. */
    pMapViewEntry = pMappedMemoryInfo->pViewList;
    while ( pMapViewEntry )
    {
        if (pMapViewEntry->lpAddress == lpAddress)
        {
            TRACE( "Removing the mapping at %p.\n", lpAddress);

#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
            NativeMapHolderRelease(pMapViewEntry->pNMHolder);
            pMapViewEntry->pNMHolder = NULL;
#else
            if ( -1 == munmap( lpAddress, pMapViewEntry->NumberOfBytesToMap ) )
            {
                ASSERT( "Unable to unmap the memory. Error=%s.\n",
                        strerror( errno ) );
                return FALSE;
            }
#endif
            /* Patch the list. */
            if ( pMapViewEntry->pPrev )
            {
                pMapViewEntry->pPrev->pNext = pMapViewEntry->pNext;
            }
            if ( pMapViewEntry->pNext )
            {
                pMapViewEntry->pNext->pPrev = pMapViewEntry->pPrev;
            }
            if ( pMapViewEntry == pMappedMemoryInfo->pViewList)
            {
                TRACE( "No more entries in the list\n" );
                pMappedMemoryInfo->pViewList = pMapViewEntry->pNext;
            }
            
            /* if it's the last view, close the 
               duplicated file mapping handle */
            if ( NULL == pMappedMemoryInfo->pViewList )
            {
                TRACE( "It is the last view!\n" );
                if ( !CloseHandle( pMapViewEntry->hFileMapping ) )
                {
                    WARN("Unable to close the duplicated"
                          " file mapping handle\n");
                }
            }
    
            free( pMapViewEntry );
            return TRUE;
        }
        pMapViewEntry = pMapViewEntry->pNext;
    }

    ERROR( "Unable to locate the view to remove.\n" );
    return FALSE;
}

/*++
Function : 
    MAPProtectionToFileOpenFlags
    
    Converts the PAGE_* flags to the O_* flags.
 
    Returns the file open flags.
--*/
static INT MAPProtectionToFileOpenFlags( DWORD flProtect )
{
    INT retVal = 0;
    switch(flProtect)
    {
    case PAGE_READONLY:
        retVal = O_RDONLY;
        break;
    case PAGE_READWRITE:
        retVal = O_RDWR;
        break;
    case PAGE_WRITECOPY:
        retVal = O_RDONLY;
        break;
    default:
        ASSERT("unexpected flProtect value %#x\n", flProtect);
        retVal = 0;
        break;
    }         
    return retVal;
}

/*++
Function :
    
    MAPIsRequestPermissible
    
        DWORD flProtect     - The requested file mapping protection .
        file * pFileStruct  - The file structure containing all the information.

--*/
static BOOL MAPIsRequestPermissible( DWORD flProtect, file * pFileStruct )
{
    if ( ( (flProtect == PAGE_READONLY || flProtect == PAGE_WRITECOPY) && 
           (pFileStruct->open_flags_deviceaccessonly == TRUE || 
            pFileStruct->open_flags & O_WRONLY) )
       )
    {
        /*
         * PAGE_READONLY or PAGE_WRITECOPY access to a file must at least be
         * readable. Contrary to what MSDN says, PAGE_WRITECOPY
         * only needs to be readable.
         */
        return FALSE;
    }
    else if ( flProtect == PAGE_READWRITE && !(pFileStruct->open_flags & O_RDWR) )
    {
        /*
         * PAGE_READWRITE access to a file needs to be readable and writable 
         */
        return FALSE;
    }
    else
    {
        /* Action is permissible */
        return TRUE;
    }
}

/*++
Function: 
    
    MAPCreateTempFile - 
        
        Creates the temp file for a NAMED mapping.
    
        OUT lpUnixFd - A pointer to the unix file descriptor.
        
        Returns a pointer to the filename on sucess, NULL otherwise.

    NOTE:   This function's caller owns the critical section.
            This function sets the last error code.
--*/
static LPSTR MAPCreateTempFile( PINT lpUnixFd )
{
    LPSTR lpFileName = NULL;

    TRACE( "MAPCreateTempFile( PINT lpUnixFd = %p )\n", lpUnixFd );

    lpFileName = (LPSTR)malloc( MAX_PATH + 1 );
    if ( !lpFileName )
    {
        ERROR( "Unable to allocate memory.\n" );
        SetLastError( ERROR_NOT_ENOUGH_MEMORY );
        goto exit;
    }

    *lpFileName = '\0';
    if ( !PALGetPalConfigDir( lpFileName, MAX_PATH ) )
    {
        ASSERT( "Unable to determine the PAL config directory.\n" );
        SetLastError( ERROR_INTERNAL_ERROR );
        goto exit;
    }

    strcat( lpFileName, MAP_TEMPLATE );
    
    /* Create a file to be used as the paging file. */
    *lpUnixFd = mkstemp( lpFileName );  
    if ( -1 == *lpUnixFd )
    {
        char * errmsg = strerror(errno);
        ERROR( "mkstemp() failed; errno is %d (%s)\n", errno, 
               errmsg ? errmsg : "");
        SetLastError( ERROR_INTERNAL_ERROR );
        free( lpFileName );
        lpFileName = NULL;
    }

exit:
    TRACE( "returning %s\n", lpFileName );
    return lpFileName;
}

/*++

caller must own the critical section.
--*/
static HANDLE MAPAddHandle( 
    PGLOBAL_FILE_MAPPING_OBJECT lpKernelObject, 
    DWORD dwDesiredAccess, 
    INT UnixFd )
{
    HANDLE hRetVal = INVALID_HANDLE_VALUE;
    PMAPPED_MEMORY_INFO lpHandleObject = NULL;
    
    /* Create the handle */
    lpHandleObject = (PMAPPED_MEMORY_INFO)malloc( sizeof( MAPPED_MEMORY_INFO ) );
    if ( lpHandleObject )
    {
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
        struct stat st;
#endif
        /* Set the per-process information. */
        lpHandleObject->dwDesiredAccessWhenOpened = dwDesiredAccess;
        lpHandleObject->pViewList = NULL;
        lpHandleObject->HandleRefCount = 1;
        
        /* Set the pointer to the GMMO object, */
        if ( 0 != lpKernelObject->_ShmHeader.ShmObjectName )
        {
            lpHandleObject->kernelObject.shm = 
                lpKernelObject->_ShmHeader.ShmSelf;
            lpHandleObject->bIsNamed = TRUE;
        }
        else
        {
            lpHandleObject->kernelObject.lp = lpKernelObject;
            lpHandleObject->bIsNamed = FALSE;
        }

        /* Set the generic fields. */
        lpHandleObject->HandleData.type = HOBJ_MAP;
        lpHandleObject->HandleData.close_handle = &MAPCloseMappingHandle;
        lpHandleObject->HandleData.dup_handle = &MAPDuplicateMappingHandle;
        lpHandleObject->UnixFd = UnixFd;
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
        if (-1 == UnixFd)
        {
            lpHandleObject->MappedFileDevNum = (dev_t)-1; /* there is no standard NO_DEV */
            lpHandleObject->MappedFileInodeNum = NO_INO;
        }
        else if (0 == fstat(UnixFd, &st))
        {
            lpHandleObject->MappedFileDevNum = st.st_dev;
            lpHandleObject->MappedFileInodeNum = st.st_ino;
        }
        else
        {
            ERROR("Couldn't get inode info for fd=%d to be stored in mapping object\n", UnixFd);
            free( lpHandleObject );
            lpHandleObject = NULL;
            goto exit;
        }
#endif // ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
        lpHandleObject->pNext = NULL;
        lpHandleObject->pPrev = NULL;

        hRetVal = HMGRGetHandle( (HOBJSTRUCT*)lpHandleObject ); 
        if( INVALID_HANDLE_VALUE == hRetVal )
        {
            ERROR("Couldn't allocate a handle for mapping object\n");
            free( lpHandleObject );
            lpHandleObject = NULL;
            goto exit;
        }
        
        /* Add the handle to the list. */
        lpHandleObject->pNext = pMap;
        if ( lpHandleObject->pNext )
        {
            lpHandleObject->pNext->pPrev = lpHandleObject;
        }

        pMap = lpHandleObject;
    }
    else
    {
        ERROR( "Unable to create the global mapping object.\n" );
    }

exit:
    return hRetVal;
}

/*++
Function :
    MAPGetRegionSize

    Parameters: 
    lpAddress: pointer to the starting memory location, not necessary
               to be rounded to the page location

    Note: This function is to be used in virtual.c
        
    Returns the region size in byte
--*/
SIZE_T MAPGetRegionSize( LPVOID lpAddress)
{
    PMAPPED_MEMORY_INFO pObject = NULL;
    UINT MappedSize;

    if ( NULL == lpAddress )
    {
        ERROR( "lpAddress cannot be NULL\n" );
        return -1;
    }

    SYNCEnterCriticalSection( &mapping_critsec , TRUE);
    pObject = pMap;

    while ( pObject )
    {
        PMAPPED_VIEW_LIST pView = pObject->pViewList;
        while ( pView )
        {   
            VOID * real_map_addr;
            SIZE_T real_map_sz;
#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
            real_map_addr = pView->pNMHolder->address;
            real_map_sz = pView->pNMHolder->size;
#else
            real_map_addr = pView->lpAddress;
            real_map_sz = pView->NumberOfBytesToMap;
#endif
                
            MappedSize = ((real_map_sz-1) & ~VIRTUAL_PAGE_MASK) + VIRTUAL_PAGE_SIZE; 
            if ( real_map_addr <= lpAddress && 
                 (real_map_addr+MappedSize) > lpAddress )
            {
                SIZE_T ret=MappedSize + (UINT_PTR) real_map_addr - 
                       ((UINT_PTR) lpAddress & ~VIRTUAL_PAGE_MASK);
                SYNCLeaveCriticalSection( &mapping_critsec , TRUE);
                return ret;
            }
            pView = pView->pNext;
        }
        pObject = pObject->pNext;
    }
    /* Nothing found */
    SYNCLeaveCriticalSection( &mapping_critsec , TRUE);
    return 0;
}

#if ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS

static PMAPPED_VIEW_LIST FindSharedMappingReplacement(
    struct stat * pstat, 
    SIZE_T size, 
    SIZE_T offset)
{
    PMAPPED_MEMORY_INFO pObject = NULL;
    PMAPPED_VIEW_LIST pNewView = NULL;
    BOOL done = 0;

    if (pstat == NULL)
    {
        ERROR("NULL stat pointer\n");
        return NULL;
    }

    if (pstat->st_ino == NO_INO)
    {
        ERROR("Searching for invalid inode number\n");
        return NULL;
    }

    if (size == 0)
    {
        ERROR("Mapping size cannot be NULL\n");
        return NULL;
    }

    SYNCEnterCriticalSection(&mapping_critsec, TRUE);
    pObject = pMap;

    while (pObject && !done)
    {
        if ((pObject->MappedFileInodeNum == pstat->st_ino) && 
            (pObject->MappedFileDevNum == pstat->st_dev) && 
            (pObject->dwDesiredAccessWhenOpened != FILE_MAP_COPY))
        {
            PMAPPED_VIEW_LIST pView = pObject->pViewList;
            while (pView && !done)
            {   
                SIZE_T real_map_offs = pView->pNMHolder->offset;
                SIZE_T real_map_sz = pView->pNMHolder->size;

                if ((pView->dwDesiredAccess != FILE_MAP_COPY) && 
                    (real_map_offs <= offset) && (real_map_offs+real_map_sz >= offset))
                {
                    /* Found a shared mapping of the same inode overlapping
                       with the one desired. Whether or not this mapping is
                       reusable for the new one, the search is over. In fact
                       on ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS systems 
                       there cannot be shared mappings of two overlapping 
                       regions of the same file, in the same process. 
                       Therefore, whether this mapping is reusable or we 
                       cannot map the requested region of the specified file 
                    */
                    done = 1;
                    if (real_map_offs+real_map_sz >= offset+size)
                    {
                        /* The new desired mapping is fully contained in the 
                           one just found: we can reuse this one */

                        pNewView = (PMAPPED_VIEW_LIST)malloc(sizeof(MAPPED_VIEW_LIST));
                        if (pNewView)
                        {
                            pNewView->pNMHolder = pView->pNMHolder;
                            NativeMapHolderAddRef(pNewView->pNMHolder);
                            pNewView->lpAddress = pNewView->pNMHolder->address + 
                                offset - pNewView->pNMHolder->offset;
                            pNewView->NumberOfBytesToMap = size;
                        }
                        else
                        {
                            ERROR("No memory for new MAPPED_VIEW_LIST node\n");
                        }
                    }
                }
                pView = pView->pNext;
            }
        }
        pObject = pObject->pNext;
    }
    SYNCLeaveCriticalSection(&mapping_critsec , TRUE);

    TRACE ("FindSharedMappingReplacement returning %p\n", pNewView);
    return pNewView;
}

static NativeMapHolder * NewNativeMapHolder(LPVOID address, SIZE_T size, 
                                     SIZE_T offset, long init_ref_count)
{
    NativeMapHolder * pThisMapHolder;
    
    if (init_ref_count < 0)
    {
        ASSERT("Negative initial reference count for new map holder\n");
        return NULL;
    }

    pThisMapHolder = 
        (NativeMapHolder *)malloc(sizeof(NativeMapHolder));
        
    if (pThisMapHolder)
    {
        pThisMapHolder->ref_count = init_ref_count;
        pThisMapHolder->address = address;
        pThisMapHolder->size = size;
        pThisMapHolder->offset = offset;
    }
    
    return pThisMapHolder;
}

static LONG NativeMapHolderAddRef(NativeMapHolder * thisNMH)
{
    LONG ret = InterlockedIncrement(&thisNMH->ref_count);
    return ret;
}

static LONG NativeMapHolderRelease(NativeMapHolder * thisNMH)
{
    LONG ret = InterlockedDecrement(&thisNMH->ref_count);
    if (ret == 0)
    {
        if (-1 == munmap(thisNMH->address, thisNMH->size))
        {
            ASSERT( "Unable to unmap memory. Error=%s.\n",
                    strerror( errno ) );
        }
        else
        {
            TRACE( "Successfully unmapped %p (size=%lu)\n", 
                   thisNMH->address, (unsigned long)thisNMH->size);
        }
        free (thisNMH);
    }
    else if (ret < 0)
    {
        ASSERT( "Negative reference count for map holder %p"
                " {address=%p, size=%lu}\n", thisNMH->address, 
                (unsigned long)thisNMH->size);
    }

    return ret;
}

#endif // ONE_SHARED_MAPPING_PER_FILEREGION_PER_PROCESS
