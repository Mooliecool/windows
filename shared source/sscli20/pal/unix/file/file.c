/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    file.c

Abstract:

    Implementation of the file WIN API for the PAL

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/critsect.h"
#include "pal/handle.h"
#include "pal/file.h"
#include "pal/filetime.h"
#include "pal/utils.h"

#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <errno.h>

typedef enum
{
  PIID_STDIN_HANDLE, 
  PIID_STDOUT_HANDLE,
  PIID_STDERR_HANDLE
} PROCINFO_ID; 

#define PAL_LEGAL_FLAGS_ATTRIBS (FILE_ATTRIBUTE_NORMAL| \
                                 FILE_FLAG_SEQUENTIAL_SCAN| \
                                 FILE_FLAG_WRITE_THROUGH| \
                                 FILE_FLAG_NO_BUFFERING| \
                                 FILE_FLAG_RANDOM_ACCESS| \
                                 FILE_FLAG_BACKUP_SEMANTICS)


SET_DEFAULT_DEBUG_CHANNEL(FILE);

static int FILECloseHandle( HOBJSTRUCT *handle_data);
static int FILECloseStdHandle( HOBJSTRUCT *handle_data);
static int FILEDuplicateHandle( HANDLE handle, HOBJSTRUCT *handle_data);

static file *FILENewFileData( void );

static BOOL FILEAddNewLockedRgn(SHMFILELOCKS* fileLocks,
                                file *fileStructPtr, 
                                SHMFILELOCKRGNS *insertAfter,
                                UINT64 lockRgnStart, 
                                UINT64 nbBytesToLock, 
                                LOCK_TYPE lockType);

static BOOL FILELockFileRegion(file *fileStructPtr,
                               UINT64 lockRgnStart,
                               UINT64  nbBytesToLock, 
                               LOCK_TYPE lockAction);

static BOOL FILEUnlockFileRegion(file *pFileStruct, 
                                 UINT64 unlockRgnStart, 
                                 UINT64 nbBytesToUnlock, 
                                 LOCK_TYPE unlockAction);

static void FILECleanUpLockedRgn(file *fileStructPtr);

static SHMPTR FILEGetSHMFileLocks(char *filename);
/* Static global. The init function must be called
before any other functions and if it is not successful, 
no other functions should be done. */
static HANDLE pStdIn;
static HANDLE pStdOut;
static HANDLE pStdErr;

/*++

Function : 
    FILEGetProperNotFoundError
    
Returns the proper error code, based on the
Windows behavoir.

    IN LPSTR lpPath - The path to check.
    LPDWORD lpErrorCode - The error to set.
*/
void FILEGetProperNotFoundError( LPSTR lpPath, LPDWORD lpErrorCode )
{
    struct stat stat_data;
    LPSTR lpDupedPath = NULL;
    LPSTR lpLastPathSeperator = NULL;

    TRACE( "FILEGetProperNotFoundError( %s )\n", lpPath?lpPath:"(null)" );

    if ( !lpErrorCode )
    {
        ASSERT( "lpErrorCode has to be valid\n" );
        return;
    }

    if ( NULL == ( lpDupedPath = strdup( lpPath ) ) )
    {
        ERROR( "strdup() failed!\n" );
        *lpErrorCode = ERROR_NOT_ENOUGH_MEMORY;
        return;
    }

    /* Determine whether it's a file not found or path not found. */
    lpLastPathSeperator = strrchr( lpDupedPath, '/');
    if ( lpLastPathSeperator != NULL )
    {
        INT nStatRet = 0;
        *lpLastPathSeperator = '\0';
        
        /* If the last path component is a directory,
           we return file not found. If it's a file or
           doesn't exist, we return path not found. */
        if ( '\0' == *lpDupedPath || 
             ( ( nStatRet = stat( lpDupedPath, &stat_data ) ) == 0 && 
             ( stat_data.st_mode & S_IFMT ) == S_IFDIR ) )
        {
            TRACE( "ERROR_FILE_NOT_FOUND\n" );
            *lpErrorCode = ERROR_FILE_NOT_FOUND;
        }
        else
        {
            TRACE( "ERROR_PATH_NOT_FOUND\n" );
            *lpErrorCode = ERROR_PATH_NOT_FOUND;
        }
    }
    else
    {
        TRACE( "ERROR_FILE_NOT_FOUND\n" );
        *lpErrorCode = ERROR_FILE_NOT_FOUND;
    }
    
    free( lpDupedPath );
    lpDupedPath = NULL;
    TRACE( "FILEGetProperNotFoundError returning TRUE\n" );
    return;
}

/*++
Function:
  CreateFileA

Note:
  Only bInherit flag is used from the LPSECURITY_ATTRIBUTES struct.
  Desired access is READ, WRITE or 0
  Share mode is READ, WRITE or DELETE

See MSDN doc.
--*/
HANDLE
PALAPI
CreateFileA(
        IN LPCSTR lpFileName,
        IN DWORD dwDesiredAccess,
        IN DWORD dwShareMode,
        IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        IN DWORD dwCreationDisposition,
        IN DWORD dwFlagsAndAttributes,
        IN HANDLE hTemplateFile)
{
    LPSTR lpUnixPath = NULL;
    DWORD dwLastError = 0;
    BOOL inheritable = FALSE;

    file   *file_data = NULL;
    HANDLE hRet = INVALID_HANDLE_VALUE;
    
    SHMFILELOCKS *fileLocks = NULL;
    int   filed = -1;
    int   create_flags = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    int   open_flags = 0;

    /* use for CREATE_ALWAYS flag */
    const char* template_suffix = ".XXXXXX";
    char* lpFilenameTemplate = NULL;
    BOOL bTmpFileCreated = FALSE;

    BOOL bIsSHMLockSet = FALSE;

    char full_unix_path[MAXPATHLEN];
    BOOL bFirstTimeAccess = FALSE;
    
    PERF_ENTRY(CreateFileA);
    ENTRY("CreateFileA(lpFileName=%p (%s), dwAccess=%#x, dwShareMode=%#x, "
          "lpSecurityAttr=%p, dwDisposition=%#x, dwFlags=%#x, " 
          "hTemplateFile=%p )\n",lpFileName?lpFileName:"NULL",lpFileName?lpFileName:"NULL", dwDesiredAccess, 
          dwShareMode, lpSecurityAttributes, dwCreationDisposition, 
          dwFlagsAndAttributes, hTemplateFile);

    /* for dwShareMode only three flags are accepted */
    if ( dwShareMode & ~(FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE) )
    {
        ASSERT( "dwShareMode is invalid\n" );
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }

    if ( lpFileName == NULL )
    {
        ERROR("CreateFileA called with NULL filename\n");
        dwLastError = ERROR_PATH_NOT_FOUND;
        goto done;
    }
    
    lpUnixPath = strdup( lpFileName );
    if ( lpUnixPath == NULL )
    {
        ERROR("malloc() failed\n");
        dwLastError = ERROR_NOT_ENOUGH_MEMORY;
        goto done;
    }
    FILEDosToUnixPathA( lpUnixPath );
    // Compute the absolute pathname to the file.  This pathname is used
    // to determine if two file names represent the same file.
    if (FILECanonicalizeRealPath(lpUnixPath, full_unix_path) == NULL)
    {
        // This fails if any portion of the lpUnixPath (except the
        // last portion) doesn't exist.
        dwLastError = ERROR_PATH_NOT_FOUND;
        goto done;
    }
    free(lpUnixPath);
    lpUnixPath = strdup( full_unix_path );
    if ( lpUnixPath == NULL )
    {
        ERROR("malloc() failed\n");
        dwLastError = ERROR_NOT_ENOUGH_MEMORY;
        goto done;
    }

    switch( dwDesiredAccess )
    {
    case 0:
        /* Device Query Access was requested. let's use open() with 
           no flags, it's basically the equivalent of O_RDONLY, since 
           O_RDONLY is defined as 0x0000 */
        break;
    case( GENERIC_READ ):
        open_flags |= O_RDONLY;
        break;
    case( GENERIC_WRITE ):
        open_flags |= O_WRONLY;
        break;
    case( GENERIC_READ | GENERIC_WRITE ):
        open_flags |= O_RDWR;
        break;
    default:
        ERROR("dwDesiredAccess value of %d is invalid\n", dwDesiredAccess);
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }

    TRACE("open flags are 0x%lx\n", open_flags);
    
    if ( lpSecurityAttributes )
    {
        if ( lpSecurityAttributes->nLength != sizeof( SECURITY_ATTRIBUTES ) ||
             lpSecurityAttributes->lpSecurityDescriptor != NULL ||
             !lpSecurityAttributes->bInheritHandle )
        {
            ASSERT("lpSecurityAttributes points to invalid values.\n");
            dwLastError = ERROR_INVALID_PARAMETER;
            goto done;
        }
        inheritable = TRUE;
    }

    if ( (dwFlagsAndAttributes & PAL_LEGAL_FLAGS_ATTRIBS) !=
          dwFlagsAndAttributes)
    {
        ASSERT("Bad dwFlagsAndAttributes\n");
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    } 
    else if (dwFlagsAndAttributes & FILE_FLAG_BACKUP_SEMANTICS)
    {
        /* Override the open flags, and always open as readonly.  This
           flag is used when opening a directory, to change its
           creation/modification/access times.  On Windows, the directory
           must be open for write, but on Unix, it needs to be readonly. */
        open_flags = O_RDONLY;
    } else {
        struct stat st;

        if (stat(lpUnixPath, &st) == 0 && (st.st_mode & S_IFDIR))
        {
            /* The file exists and it is a directory.  Without
                   FILE_FLAG_BACKUP_SEMANTICS, Win32 CreateFile always fails
                   to open directories. */
                dwLastError = ERROR_ACCESS_DENIED;
                goto done;
        }
    }

    if ( hTemplateFile )
    {
        ASSERT("hTemplateFile is not NULL, as it should be.\n");
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }

    /* allocate the file_data structure */
    file_data = FILENewFileData();
    if (file_data == NULL)
    {
        dwLastError = ERROR_NOT_ENOUGH_MEMORY;
        goto done;
    }
    
    file_data->unix_filename = strdup( lpUnixPath );
    if ( file_data->unix_filename == NULL )
    {
        ERROR("strdup() failed\n");
        dwLastError = ERROR_NOT_ENOUGH_MEMORY;
        goto done;
    }

    hRet = HMGRGetHandle( (HOBJSTRUCT*)file_data );
    if ( hRet == INVALID_HANDLE_VALUE )
    {
        ERROR("Failed to get handle\n");
        dwLastError = ERROR_NOT_ENOUGH_MEMORY;
        goto done;
    }

    SHMLock();
    bIsSHMLockSet = TRUE; 

    /* check if this file has shared access */
    file_data->shmFileLocks = FILEGetSHMFileLocks(file_data->unix_filename);
    if (file_data->shmFileLocks == 0) 
    {
        /* the FILEGetSHMFileLocks could fail due to out-of-memory 
           in this case the last error will be set by FILEGetSHMFileLocks 
           but dwLastError needs to be set for proper error handling 
           to occur */
        dwLastError = GetLastError();
        goto done;    
    }

    if (SHMPTR_TO_PTR_BOOL(fileLocks, file_data->shmFileLocks) == FALSE)
    {
        ASSERT("Unable to get pointer from shm pointer.\n");
        dwLastError = ERROR_INTERNAL_ERROR;
        goto done;
    }
        
    if(SHARE_MODE_NOT_INITALIZED == fileLocks->share_mode)
    {
        /* this is the first time this file is open */
        fileLocks->share_mode = (int) dwShareMode;
        bFirstTimeAccess = TRUE;
    }
    /* start checking for dwDesired access and dwShareMode conditions */
    else if(0 == fileLocks->share_mode)
    {
        /* file is exclusively locked */
        dwLastError = ERROR_SHARING_VIOLATION;
        goto done;
    }
    /* check for if the desired access is allowed by the share mode */
    else if( (dwDesiredAccess & GENERIC_READ) && 
             !(fileLocks->share_mode & FILE_SHARE_READ) )
    {
        dwLastError = ERROR_SHARING_VIOLATION;
        goto done;     
    }
    else if( (dwDesiredAccess & GENERIC_WRITE) && 
             !(fileLocks->share_mode & FILE_SHARE_WRITE) )
    {
        dwLastError = ERROR_SHARING_VIOLATION;
        goto done;     
    }
    /* The case when changing to a conflicting share mode is particular.
       The general rule is: changing from conflicting share mode is invalid
       (i.e changing from FILE_SHARE_WRITE to FILE_SHARE_READ is invalid).
       However, if one of the share flags is the same
       (i.e changing from FILE_SHARE_WRITE to FILE_SHARE_READ | FILE_SHARE_WRITE)
       the result is valid. (Please note that FILE_SHARE_READ is ignored
       in this case).
    */
    else if( (dwShareMode & FILE_SHARE_READ) && 
             !(dwShareMode & FILE_SHARE_WRITE) &&
             !(fileLocks->share_mode & FILE_SHARE_READ))
                         
    {
        dwLastError = ERROR_SHARING_VIOLATION;
        goto done;     
    }
    else if( (dwShareMode & FILE_SHARE_WRITE) && 
             !(dwShareMode & FILE_SHARE_READ) &&
             !(fileLocks->share_mode & FILE_SHARE_WRITE))
    {
        dwLastError = ERROR_SHARING_VIOLATION;
        goto done;     
    }
    /* Changing to a less permissive sharing permissions is valid
       if the file handle doesn't have an access right that conflicts with
       the sharing permissions we are trying to set
       (ex: changing from FILE_SHARE_READ|FILE_SHARE_WRITE to FILE_SHARE_WRITE
       isn't valid if the file descriptor still has a GENERIC_READ permission).
    */     
    else if( (fileLocks->nbReadAccess) && 
             !(dwShareMode & FILE_SHARE_READ) )
    {
        dwLastError = ERROR_SHARING_VIOLATION;
        goto done;     
    }
    else if( (fileLocks->nbWriteAccess) && 
             !(dwShareMode & FILE_SHARE_WRITE) )
    {
        dwLastError = ERROR_SHARING_VIOLATION;
        goto done;     
    }

    /* we are trying to change to a less restrictive sharing permission set 
       keep the current permissions */  
    if( (dwShareMode & FILE_SHARE_READ) && 
              !(fileLocks->share_mode & FILE_SHARE_READ) )
    {
        dwShareMode = fileLocks->share_mode;
    }

    if( (dwShareMode & FILE_SHARE_WRITE) && 
              !(fileLocks->share_mode & FILE_SHARE_WRITE) )
    {
        dwShareMode = fileLocks->share_mode;
    }

    /* NB: According to MSDN docs, When CREATE_ALWAYS or OPEN_ALWAYS is
       set, CreateFile should SetLastError to ERROR_ALREADY_EXISTS,
       even though/if CreateFile will be successful.
    */
    switch( dwCreationDisposition )
    {
    case( CREATE_ALWAYS ):        
        /* if a file already has the same name as the file we want to create, 
           we should only destroy it if the creation is (reasonably) certain to 
           succeed. To achieve this, we create the file using a temporary
           filename and then rename it. This will overwrite the original file.
        */
        lpFilenameTemplate = malloc((strlen(lpUnixPath) + 
                                     strlen(template_suffix) + 1)*sizeof(char));
        if(NULL == lpFilenameTemplate)
        {
            ERROR("malloc() failed\n");
            dwLastError = ERROR_NOT_ENOUGH_MEMORY;
            goto done;
        } 

        /* create the file using a temporary name */
        strcpy(lpFilenameTemplate, lpUnixPath);
        strcat(lpFilenameTemplate, template_suffix);
        filed = mkstemp(lpFilenameTemplate);
        if ( -1 == filed ) 
        {
            ERROR("mkstemp( %s ) failed\n", lpFilenameTemplate);

            if (ENOENT == errno)
            {
                FILEGetProperNotFoundError(lpFilenameTemplate, &dwLastError);
            }
            else
            {
                dwLastError = FILEGetLastErrorFromErrno();
            }

            goto done;
        }
        /* we created the file, but we might need to erase it if an operation on
           the file descriptor fails */
        bTmpFileCreated = TRUE;
    
        if (0 == access( lpUnixPath, F_OK ) )
        {
            SetLastError(ERROR_ALREADY_EXISTS);
        }        
        
        break;
    case( CREATE_NEW ):
        open_flags |= O_CREAT | O_EXCL;
        break;
    case( OPEN_EXISTING ):
        /* don't need to do anything here */
        break;
    case( OPEN_ALWAYS ):
        if ( access( lpUnixPath, F_OK ) == 0 )
        {
            SetLastError(ERROR_ALREADY_EXISTS);
        }
        open_flags |= O_CREAT;
        break;
    case( TRUNCATE_EXISTING ):
        open_flags |= O_TRUNC;
        break;
    default:
        ASSERT("dwCreationDisposition value of %d is not valid\n",
              dwCreationDisposition);
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }

    if ( dwFlagsAndAttributes & FILE_FLAG_NO_BUFFERING )
    {
        TRACE("I/O will be unbuffered\n");
#ifdef O_DIRECT
        open_flags |= O_DIRECT;
#endif
    }
    else
    {
        TRACE("I/O will be buffered\n");
    }

    /* when  CREATE_ALWAYS is used, we already have a file descriptor
       we don't need to open the file */
    if(CREATE_ALWAYS != dwCreationDisposition)
    {
        filed = open( lpUnixPath, open_flags, create_flags );
        TRACE("Allocated file descriptor [%d]\n", filed);

        if ( filed < 0 )
        {
            WARN("open() failed; error is %s (%d)\n", strerror(errno), errno);
            if ( ENOENT == errno )
            {
                FILEGetProperNotFoundError( lpUnixPath, &dwLastError );
                if ( ERROR_INTERNAL_ERROR == dwLastError )
                {
                    goto done;
                }
            }
            else
            {
               dwLastError = FILEGetLastErrorFromErrno();
            }

/* Commented out the code below in case we need it again afterwards  */
/* for ROTOR debugging purposes : try to open the file with a lowercased file 
   name (but fail even if that succeeds, we just want to know about it)*/
/* #if _DEBUG */
            /* only do this if failure is due to absent file or directory */
/*            if(ENOENT == errno || ENOTDIR == errno)
            {
                char *p = lpUnixPath;

                while(*p)
                {
                    *p = tolower(*p);
                    p++;
                }
                filed = open( lpUnixPath, open_flags, create_flags );
                if(-1 != filed)
                {
                    close(filed);
                    filed = -1;

                    ASSERT("failed to open file <%s>, but found lowercase "
                           "version <%s>\n", lpFileName, lpUnixPath);
                }
            }   */
/* #endif */
            goto done;
        }
    }

#ifndef O_DIRECT
    if ( dwFlagsAndAttributes & FILE_FLAG_NO_BUFFERING )
    {
#ifdef F_NOCACHE
        if (-1 == fcntl(filed, F_NOCACHE, 1))
        {
            ASSERT("Can't set F_NOCACHE; fcntl() failed. errno is %d (%s)\n",
               errno, strerror(errno));
            dwLastError = ERROR_INTERNAL_ERROR;
            goto done;
        }
#elif HAVE_DIRECTIO
#if !DIRECTIO_DISABLED
        /* Use of directio is currently disabled on Solaris because
           this feature doesn't seem to be stable enough on this platform:
           - directio works on ufs and nfs file systems, but it fails on tmpfs;
           - on nfs it is not possible to mmap a file if direct I/O is enabled on it;
           - directio is a per-file persistent suggestion and there is no a real way 
             to probe it, other than turning it on or off;
           - the performance impact of blindly turning off directio for each mmap 
             is roughly 15%;
           - directio is documented on recent official docs (April 2003) to cause data 
             corruption, system hangs, or panics when used concurrently with mmap 
             on clusters.
           As result on Solaris we currently ignore FILE_FLAG_NO_BUFFERING
        */
        if (directio(filed, DIRECTIO_ON) == -1)
        {
            ASSERT("Can't set DIRECTIO_ON; directio() failed. errno is %d (%s)\n",
               errno, strerror(errno));
            dwLastError = ERROR_INTERNAL_ERROR;
            goto done;
        }
#endif // !DIRECTIO_DISABLED
#else
#error Insufficient support for uncached I/O on this platform
#endif
    }
#endif
    
    /* make file descriptor close-on-exec; inheritable handles will get
      "uncloseonexeced" in CreateProcess if they are actually being inherited*/
    if(-1 == fcntl(filed,F_SETFD,1))
    {
        ASSERT("can't set close-on-exec flag; fcntl() failed. errno is %d "
             "(%s)\n", errno, strerror(errno));
        dwLastError = ERROR_INTERNAL_ERROR;
        goto done;
    }
    file_data->inheritable = inheritable;

    /* fill the structure to store with the handle */
    file_data->unix_fd = filed;
    file_data->dwDesiredAccess = dwDesiredAccess;
    file_data->open_flags = open_flags;
    file_data->open_flags_deviceaccessonly = (dwDesiredAccess == 0);

#ifdef O_DIRECT
    if(CREATE_ALWAYS == dwCreationDisposition)
    {
        if (open_flags & O_DIRECT)
        {
            // Mark the file descriptor returned from mkstemp as
            // unbuffered.
            if (-1 == fcntl(filed,F_SETFL, O_DIRECT))
            {
                dwLastError = ERROR_INTERNAL_ERROR;
                goto done;
            }
        }
    }
#endif //O_DIRECT

    if(CREATE_ALWAYS == dwCreationDisposition)
    {
#ifdef O_DIRECT
        if (open_flags & O_DIRECT)
        {
            // Mark the file descriptor returned from mkstemp as
            // unbuffered.
            if (-1 == fcntl(filed,F_SETFL, O_DIRECT))
            {
                dwLastError = ERROR_INTERNAL_ERROR;
                goto done;
            }
        }
#endif //O_DIRECT

        /* mkstemp changes the GROUPS and OTHERS file permissions.
           this will change them back to what they should be */
        if (-1 == fchmod(filed, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))
        {
            ASSERT("failed to change file permissions; fchmod() failed. errno is %d "
             "(%s)\n", errno, strerror(errno));
            dwLastError = ERROR_INTERNAL_ERROR;
            goto done;
        }

        if (-1 == rename(lpFilenameTemplate, lpUnixPath))
        {             
            dwLastError = FILEGetLastErrorFromErrno();
                        
            ERROR("rename lpFilenameTemplate=%s to name=%s failed. errno is %d "
                 "(%s)\n", lpFilenameTemplate, lpUnixPath, errno, strerror(errno));
                           
            goto done;
        }    
    }   

    /* set the share mode again, it's possible that the share mode is now more
       restrictive than the previous mode set. */
    fileLocks->share_mode = dwShareMode;
    if( dwDesiredAccess & GENERIC_READ )
    {
        fileLocks->nbReadAccess++;  
    }
    if( dwDesiredAccess & GENERIC_WRITE )
    {
        fileLocks->nbWriteAccess++;
    }
done:
    if (dwLastError && fileLocks && bFirstTimeAccess) 
    {
        fileLocks->share_mode = SHARE_MODE_NOT_INITALIZED;
    }
    if(bIsSHMLockSet)
    {
        SHMRelease();
    }

    if (dwLastError)
    {    
        /* free the handle if necessary */
        if ( hRet != INVALID_HANDLE_VALUE )
        {
            if ( HMGRFreeHandle(hRet) == FALSE )
            {
                ERROR("Unable to free handle\n");
            }

            hRet = INVALID_HANDLE_VALUE;
        }
    
        /* clean up the file structure */
        if ( file_data )
        {
            FILECloseHandle( (HOBJSTRUCT*)file_data );
        }

        if(bTmpFileCreated)
        {
            /* a problem occured with the tempfile, delete it */
            if( -1 == unlink(lpFilenameTemplate))
            {
                ASSERT("can't delete temporary file; unlink() failed. errno is %d "
                     "(%s)\n", errno, strerror(errno));
            }            
        }            
        SetLastError(dwLastError);
    }
    
    if( lpUnixPath ) 
        free( lpUnixPath );
    
    if( lpFilenameTemplate )
        free( lpFilenameTemplate );

    LOGEXIT("CreateFileA returns HANDLE %p\n", hRet);
    PERF_EXIT(CreateFileA);
    return hRet;
}


/*++
Function:
  CreateFileW

Note:
  Only bInherit flag is used from the LPSECURITY_ATTRIBUTES struct.
  Desired access is READ, WRITE or 0
  Share mode is READ, WRITE or DELETE

See MSDN doc.
--*/
HANDLE
PALAPI
CreateFileW(
        IN LPCWSTR lpFileName,
        IN DWORD dwDesiredAccess,
        IN DWORD dwShareMode,
        IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        IN DWORD dwCreationDisposition,
        IN DWORD dwFlagsAndAttributes,
        IN HANDLE hTemplateFile)
{
    char    name[MAX_PATH];
    int     size;
    HANDLE  hRet = INVALID_HANDLE_VALUE;

    PERF_ENTRY(CreateFileW);
    ENTRY("CreateFileW(lpFileName=%p (%S), dwAccess=%#x, dwShareMode=%#x, "
          "lpSecurityAttr=%p, dwDisposition=%#x, dwFlags=%#x, hTemplateFile=%p )\n",
          lpFileName?lpFileName:W16_NULLSTRING,
          lpFileName?lpFileName:W16_NULLSTRING, dwDesiredAccess, dwShareMode,
          lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes,
          hTemplateFile);

    size = WideCharToMultiByte( CP_ACP, 0, lpFileName, -1, name, MAX_PATH,
                                NULL, NULL );
    if( size == 0 )
    {
        DWORD dwLastError = GetLastError();
        if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
        {
            ERROR("lpFileName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ERROR("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    hRet = CreateFileA( name,
            dwDesiredAccess,
            dwShareMode,
            lpSecurityAttributes,
            dwCreationDisposition,
            dwFlagsAndAttributes,
            hTemplateFile );

done:
    LOGEXIT( "CreateFileW returns HANDLE %p\n", hRet );
    PERF_EXIT(CreateFileW);
    return hRet;
}


/*++
Function:
  CopyFileW

See MSDN doc.

Notes:
  There are several (most) error paths here that do not call SetLastError().
This is because we know that CreateFile, ReadFile, and WriteFile will do so,
and will have a much better idea of the specific error.
--*/
BOOL
PALAPI
CopyFileW(
      IN LPCWSTR lpExistingFileName,
      IN LPCWSTR lpNewFileName,
      IN BOOL bFailIfExists)
{

    char    source[MAX_PATH];
    char    dest[MAX_PATH];
    int     src_size,dest_size;
    BOOL        bRet = FALSE;

    PERF_ENTRY(CopyFileW);
    ENTRY("CopyFileW(lpExistingFileName=%p (%S), lpNewFileName=%p (%S), bFailIfExists=%d)\n",
          lpExistingFileName?lpExistingFileName:W16_NULLSTRING,
          lpExistingFileName?lpExistingFileName:W16_NULLSTRING,
          lpNewFileName?lpNewFileName:W16_NULLSTRING,
          lpNewFileName?lpNewFileName:W16_NULLSTRING, bFailIfExists);

    src_size = WideCharToMultiByte( CP_ACP, 0, lpExistingFileName, -1, source, MAX_PATH,
                                NULL, NULL );
    if( src_size == 0 )
    {
        DWORD dwLastError = GetLastError();
        if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
        {
            ERROR("lpExistingFileName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    dest_size = WideCharToMultiByte( CP_ACP, 0, lpNewFileName, -1, dest, MAX_PATH,
                                NULL, NULL );
    if( dest_size == 0 )
    {
        DWORD dwLastError = GetLastError();
        if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
        {
            ERROR("lpNewFileName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    bRet = CopyFileA(source,dest,bFailIfExists);

done:
    LOGEXIT("CopyFileW returns BOOL %d\n", bRet);
    PERF_EXIT(CopyFileW);
    return bRet;
}


/*++
Function:
  DeleteFileA

See MSDN doc.
--*/
BOOL
PALAPI
DeleteFileA(
        IN LPCSTR lpFileName)
{
    int     result;
    BOOL    bRet = FALSE;
    DWORD   dwLastError = 0;
    char    lpUnixFileName[MAX_PATH];

    PERF_ENTRY(DeleteFileA);
    ENTRY("DeleteFileA(lpFileName=%p (%s))\n", lpFileName?lpFileName:"NULL", lpFileName?lpFileName:"NULL");

    if (strlen(lpFileName) >= MAX_PATH)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    strcpy( lpUnixFileName, lpFileName);
    
    FILEDosToUnixPathA( lpUnixFileName );
    
    if ( !FILEGetFileNameFromSymLink(lpUnixFileName))
    {
        if(ENOENT==errno)
        {
             FILEGetProperNotFoundError( lpUnixFileName, &dwLastError );
        }
        else
        {
            dwLastError = FILEGetLastErrorFromErrno();
        }
        goto done;
    }

    result = unlink( lpUnixFileName );

    if ( result < 0 )
    {
        TRACE("unlink returns %d\n", result);
        switch( errno )
        {
        case ENOTDIR:
            dwLastError = ERROR_PATH_NOT_FOUND;
            break;
        case ENOENT:
            dwLastError = ERROR_FILE_NOT_FOUND;
            break;
        default:
            dwLastError = ERROR_ACCESS_DENIED;
        }
    }
    else
    {
        bRet = TRUE;
    }

done:
    if(dwLastError)
    {
        SetLastError( dwLastError );
    }
    LOGEXIT("DeleteFileA returns BOOL %d\n", bRet);
    PERF_EXIT(DeleteFileA);
    return bRet;
}


/*++
Function:
  DeleteFileW

See MSDN doc.
--*/
BOOL
PALAPI
DeleteFileW(
        IN LPCWSTR lpFileName)
{
    int  size;
    char name[MAX_PATH];
    BOOL bRet = FALSE;

    PERF_ENTRY(DeleteFileW);
    ENTRY("DeleteFileW(lpFileName=%p (%S))\n",
      lpFileName?lpFileName:W16_NULLSTRING,
      lpFileName?lpFileName:W16_NULLSTRING);

    size = WideCharToMultiByte( CP_ACP, 0, lpFileName, -1, name, MAX_PATH,
                                NULL, NULL );
    if( size == 0 )
    {
        DWORD dwLastError = GetLastError();
        if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
        {
            ERROR("lpFilePathName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        bRet = FALSE;
        goto done;
    }

    bRet = DeleteFileA( name );

done:
    LOGEXIT("DeleteFileW returns BOOL %d\n", bRet);
    PERF_EXIT(DeleteFileW);
    return bRet;
}


/*++
Function:
  MoveFileA

See MSDN doc.
--*/
BOOL
PALAPI
MoveFileA(
     IN LPCSTR lpExistingFileName,
     IN LPCSTR lpNewFileName)
{
    BOOL bRet;

    PERF_ENTRY(MoveFileA);
    ENTRY("MoveFileA(lpExistingFileName=%p (%s), lpNewFileName=%p (%s))\n",
          lpExistingFileName?lpExistingFileName:"NULL",
          lpExistingFileName?lpExistingFileName:"NULL",
          lpNewFileName?lpNewFileName:"NULL",
          lpNewFileName?lpNewFileName:"NULL");

    bRet = MoveFileExA( lpExistingFileName,
            lpNewFileName,
            MOVEFILE_COPY_ALLOWED );

    LOGEXIT("MoveFileA returns BOOL %d\n", bRet);
    PERF_EXIT(MoveFileA);
    return bRet;
}


/*++
Function:
  MoveFileW

See MSDN doc.
--*/
BOOL
PALAPI
MoveFileW(
     IN LPCWSTR lpExistingFileName,
     IN LPCWSTR lpNewFileName)
{
    BOOL bRet;

    PERF_ENTRY(MoveFileW);
    ENTRY("MoveFileW(lpExistingFileName=%p (%S), lpNewFileName=%p (%S))\n",
          lpExistingFileName?lpExistingFileName:W16_NULLSTRING,
          lpExistingFileName?lpExistingFileName:W16_NULLSTRING,
          lpNewFileName?lpNewFileName:W16_NULLSTRING,
          lpNewFileName?lpNewFileName:W16_NULLSTRING);

    bRet = MoveFileExW( lpExistingFileName,
            lpNewFileName,
            MOVEFILE_COPY_ALLOWED );

    LOGEXIT("MoveFileW returns BOOL %d\n", bRet);
    PERF_EXIT(MoveFileW);
    return bRet;
}

/*++
Function:
  MoveFileExA

See MSDN doc.
--*/
BOOL
PALAPI
MoveFileExA(
        IN LPCSTR lpExistingFileName,
        IN LPCSTR lpNewFileName,
        IN DWORD dwFlags)
{
    int   result;
    char  source[MAX_PATH];
    char  dest[MAX_PATH];
    BOOL  bRet = TRUE;
    DWORD dwLastError = 0;

    PERF_ENTRY(MoveFileExA);
    ENTRY("MoveFileExA(lpExistingFileName=%p (%S), lpNewFileName=%p (%S), "
          "dwFlags=%#x)\n",
          lpExistingFileName?lpExistingFileName:"NULL",
          lpExistingFileName?lpExistingFileName:"NULL",
          lpNewFileName?lpNewFileName:"NULL",
          lpNewFileName?lpNewFileName:"NULL", dwFlags);

    /* only two flags are accepted */
    if ( dwFlags & ~(MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING) )
    {
        ASSERT( "dwFlags is invalid\n" );
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }

    if (strlen(lpExistingFileName) >= MAX_PATH)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }
    strcpy( source, lpExistingFileName);
    FILEDosToUnixPathA( source );

    if (strlen(lpNewFileName) >= MAX_PATH)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }
    strcpy( dest, lpNewFileName);
    FILEDosToUnixPathA( dest );

    if ( !FILEGetFileNameFromSymLink(source))
    {
        TRACE( "FILEGetFileNameFromSymLink failed\n" );
        if(ENOENT==errno)
        {
            FILEGetProperNotFoundError( source, &dwLastError );
        }
        else
        {
            dwLastError = FILEGetLastErrorFromErrno();
        }
        goto done;
    }

    if ( !(dwFlags & MOVEFILE_REPLACE_EXISTING) )
    {
#if HAVE_CASE_SENSITIVE_FILESYSTEM
        if ( strcmp(source, dest) != 0 )
#else   // HAVE_CASE_SENSITIVE_FILESYSTEM
        if ( strcasecmp(source, dest) != 0 )
#endif  // HAVE_CASE_SENSITIVE_FILESYSTEM
        {
            // Let things proceed normally if source and
            // dest are the same.
            if ( access(dest, F_OK) == 0 )
            {
                dwLastError = ERROR_ALREADY_EXISTS;
                goto done;
            }
        }
    }

    result = rename( source, dest );
    if ((result < 0) && (dwFlags & MOVEFILE_REPLACE_EXISTING) &&
        ((errno == ENOTDIR) || (errno == EEXIST)))
    {
        bRet = DeleteFileA( lpNewFileName );
        
        if ( bRet ) 
        {
            result = rename( source, dest );
        }
        else
        { 
            dwLastError = GetLastError();
        }
    } 

    if ( result < 0 )
    {
        switch( errno )
        {
        case EXDEV: /* we tried to link across devices */
        
            if ( dwFlags & MOVEFILE_COPY_ALLOWED )
            {
                BOOL bFailIfExists = !(dwFlags & MOVEFILE_REPLACE_EXISTING);
            
                /* if CopyFile fails here, so should MoveFailEx */
                bRet = CopyFileA( lpExistingFileName,
                          lpNewFileName,
                          bFailIfExists );
                /* CopyFile should set the appropriate error */
                if ( !bRet ) 
                {
                    dwLastError = GetLastError();
                }
                else
                {
                    if (!DeleteFileA(lpExistingFileName))
                    {
                        ERROR("Failed to delete the source file\n");
                        dwLastError = GetLastError();
                    
                        /* Delete the destination file if we're unable to delete 
                           the source file */
                        if (!DeleteFileA(lpNewFileName))
                        {
                            ERROR("Failed to delete the destination file\n");
                        }
                    }
                }
            }
            else
            {
                dwLastError = ERROR_ACCESS_DENIED;
            }
        case EEXIST: /* file already exists */
            dwLastError = ERROR_ALREADY_EXISTS;
            break;
        case ENOTDIR:
            dwLastError = ERROR_PATH_NOT_FOUND;
            break;
        case ENOENT:
            dwLastError = ERROR_FILE_NOT_FOUND;
            break;
        default:
            dwLastError = ERROR_ACCESS_DENIED;
        }
    }

done:
    if ( dwLastError )
    {
        SetLastError( dwLastError );
        bRet = FALSE;
    }

    LOGEXIT( "MoveFileExA returns BOOL %d\n", bRet );
    PERF_EXIT(MoveFileExA);
    return bRet;
}

/*++
Function:
  MoveFileExW

See MSDN doc.
--*/
BOOL
PALAPI
MoveFileExW(
        IN LPCWSTR lpExistingFileName,
        IN LPCWSTR lpNewFileName,
        IN DWORD dwFlags)
{
    char    source[MAX_PATH];
    char    dest[MAX_PATH];
    int     src_size,dest_size;
    BOOL        bRet = FALSE;

    PERF_ENTRY(MoveFileExW);
    ENTRY("MoveFileExW(lpExistingFileName=%p (%S), lpNewFileName=%p (%S), dwFlags=%#x)\n",
          lpExistingFileName?lpExistingFileName:W16_NULLSTRING,
          lpExistingFileName?lpExistingFileName:W16_NULLSTRING,
          lpNewFileName?lpNewFileName:W16_NULLSTRING,
          lpNewFileName?lpNewFileName:W16_NULLSTRING, dwFlags);

    src_size = WideCharToMultiByte( CP_ACP, 0, lpExistingFileName, -1, source, MAX_PATH,
                                NULL, NULL );
    if( src_size == 0 )
    {
        DWORD dwLastError = GetLastError();
        if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
        {
            ERROR("lpExistingFileName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    dest_size = WideCharToMultiByte( CP_ACP, 0, lpNewFileName, -1, dest, MAX_PATH,
                                NULL, NULL );
    if( dest_size == 0 )
    {
        DWORD dwLastError = GetLastError();
        if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
        {
            ERROR("lpNewFileName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    bRet = MoveFileExA(source,dest,dwFlags);

done:
    LOGEXIT("MoveFileExW returns BOOL %d\n", bRet);
    PERF_EXIT(MoveFileExW);
    return bRet;
}

/*++
Function:
  GetFileAttributesA

Note:
  Checking for directory and read-only file, according to Rotor spec.

Caveats:
  There are some important things to note about this implementation, which
are due to the differences between the FAT filesystem and Unix filesystems:

- fifo's, sockets, and symlinks will return -1, and GetLastError() will
  return ERROR_ACCESS_DENIED

- if a file is write-only, or has no permissions at all, it is treated
  the same as if it had mode 'rw'. This is consistent with behaviour on
  NTFS files with the same permissions.

- filenames beginning with a '.' are considered to be hidden files.

- the following flags will never be returned:

FILE_ATTRIBUTE_SYSTEM
FILE_ATTRIBUTE_ARCHIVE

--*/
DWORD
PALAPI
GetFileAttributesA(
           IN LPCSTR lpFileName)
{
    struct stat stat_data;
    DWORD dwAttr = 0;
    DWORD dwLastError = 0;
    CHAR UnixFileName[MAX_PATH + 1];

    PERF_ENTRY(GetFileAttributesA);
    ENTRY("GetFileAttributesA(lpFileName=%p (%s))\n", lpFileName?lpFileName:"NULL", lpFileName?lpFileName:"NULL");
                           
    if (lpFileName == NULL) 
    {
        dwLastError = ERROR_PATH_NOT_FOUND;
        goto done;
    }
    
    strcpy( UnixFileName, lpFileName );
    FILEDosToUnixPathA( UnixFileName );

    if ( stat(UnixFileName, &stat_data) != 0 )
    {
        if (ENOENT == errno)
        {
            /* The file doesn't exist. */
            TRACE("Couldn't stat non-existent file %s\n", UnixFileName);

            FILEGetProperNotFoundError( UnixFileName, &dwLastError );
            if ( ERROR_INTERNAL_ERROR == dwLastError )
            {
                goto done;
            }
        }
        else if ((dwLastError = FILEGetLastErrorFromErrno()) == ERROR_INTERNAL_ERROR)
        {
            ASSERT("stat() not expected to fail with errno:%d (%s)\n",
                   errno, strerror(errno));
        }
        goto done;
    }

    if ( (stat_data.st_mode & S_IFMT) == S_IFDIR )
    {
        dwAttr |= FILE_ATTRIBUTE_DIRECTORY;
    }
    else if ( (stat_data.st_mode & S_IFMT) != S_IFREG )
    {
        ERROR("Not a regular file or directory, S_IFMT is %#x\n", 
              stat_data.st_mode & S_IFMT);
        dwLastError = ERROR_ACCESS_DENIED;
        goto done;
    }

    if ( UTIL_IsReadOnlyBitsSet( &stat_data ) )
    {
        dwAttr |= FILE_ATTRIBUTE_READONLY;
    }
    
    if ( *(FILEGetFileNameFromFullPathA(UnixFileName)) == '.' )
    {
        dwAttr |= FILE_ATTRIBUTE_HIDDEN;
    }

    /* finally, if nothing is set... */
    if ( dwAttr == 0 )
    {
        dwAttr = FILE_ATTRIBUTE_NORMAL;
    }

done:
    if (dwLastError)
    {
        SetLastError(dwLastError);
        dwAttr = -1;
    }

    LOGEXIT("GetFileAttributesA returns DWORD %#x\n", dwAttr);
    PERF_EXIT(GetFileAttributesA);
    return dwAttr;
}




/*++
Function:
  GetFileAttributesW

Note:
  Checking for directory and read-only file

See MSDN doc.
--*/
DWORD
PALAPI
GetFileAttributesW(
           IN LPCWSTR lpFileName)
{
    int   size;
    char  filename[MAX_PATH];
    DWORD dwRet = -1;

    PERF_ENTRY(GetFileAttributesW);
    ENTRY("GetFileAttributesW(lpFileName=%p (%S))\n",
          lpFileName?lpFileName:W16_NULLSTRING,
          lpFileName?lpFileName:W16_NULLSTRING);

    if (lpFileName == NULL) 
    {
        SetLastError(ERROR_PATH_NOT_FOUND);
        goto done;
    }
    
    size = WideCharToMultiByte( CP_ACP, 0, lpFileName, -1, filename, MAX_PATH,
                                NULL, NULL );
    if( size == 0 )
    {
        DWORD dwLastError = GetLastError();
        if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
        {
            ERROR("lpFileName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        SetLastError(ERROR_INVALID_PARAMETER);
    }
    else
        dwRet = GetFileAttributesA( filename );
done:
    LOGEXIT("GetFileAttributesW returns DWORD %#x\n", dwRet);
    PERF_EXIT(GetFileAttributesW);
    return dwRet;
}


/*++
Function:
  GetFileAttributesExW

See MSDN doc, and notes for GetFileAttributesW.
--*/
BOOL
PALAPI
GetFileAttributesExW(
             IN LPCWSTR lpFileName,
             IN GET_FILEEX_INFO_LEVELS fInfoLevelId,
             OUT LPVOID lpFileInformation)
{
    BOOL bRet = FALSE;
    DWORD dwLastError = 0;
    LPWIN32_FILE_ATTRIBUTE_DATA attr_data;

    struct stat stat_data;

    char name[MAX_PATH];
    int  size;

    PERF_ENTRY(GetFileAttributesExW);
    ENTRY("GetFileAttributesExW(lpFileName=%p (%S), fInfoLevelId=%d, "
          "lpFileInformation=%p)\n", lpFileName?lpFileName:W16_NULLSTRING, lpFileName?lpFileName:W16_NULLSTRING,
          fInfoLevelId, lpFileInformation);

    if ( fInfoLevelId != GetFileExInfoStandard )
    {
        ASSERT("Unrecognized value for fInfoLevelId=%d\n", fInfoLevelId);
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }

    if ( !lpFileInformation )
    {
        ASSERT("lpFileInformation is NULL\n");
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }

    if (lpFileName == NULL) 
    {
        dwLastError = ERROR_PATH_NOT_FOUND;
        goto done;
    }
    
    size = WideCharToMultiByte( CP_ACP, 0, lpFileName, -1, name, MAX_PATH,
                                NULL, NULL );
    if( size == 0 )
    {
        dwLastError = GetLastError();
        if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
        {
            ERROR("lpFileName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }

    attr_data = (LPWIN32_FILE_ATTRIBUTE_DATA)lpFileInformation;

    attr_data->dwFileAttributes = GetFileAttributesW(lpFileName);
    /* assume that GetFileAttributes will call SetLastError appropriately */
    if ( attr_data->dwFileAttributes == -1 )
    {
        goto done;
    }

    FILEDosToUnixPathA(name);
    /* do the stat */
    if ( stat(name, &stat_data) != 0 )
    {
        ERROR("stat failed on %S\n", lpFileName);
        dwLastError = FILEGetLastErrorFromErrno();
        goto done;
    }

    /* get the file times */
    attr_data->ftCreationTime =
        FILEUnixTimeToFileTime( stat_data.st_ctime,
                                ST_CTIME_NSEC(&stat_data) );
    attr_data->ftLastAccessTime =
        FILEUnixTimeToFileTime( stat_data.st_atime,
                                ST_ATIME_NSEC(&stat_data) );
    attr_data->ftLastWriteTime =
        FILEUnixTimeToFileTime( stat_data.st_mtime,
                                ST_MTIME_NSEC(&stat_data) );

    /* Get the file size. GetFileSize is not used because it gets the
       size of an already-open file */
    attr_data->nFileSizeLow = (DWORD) stat_data.st_size;
#if SIZEOF_OFF_T > 4
    attr_data->nFileSizeHigh = (DWORD)(stat_data.st_size >> 32);
#else
    attr_data->nFileSizeHigh = 0;
#endif

    bRet = TRUE;

done:
    if (dwLastError) SetLastError(dwLastError);

    LOGEXIT("GetFileAttributesExW returns BOOL %d\n", bRet);
    PERF_EXIT(GetFileAttributesExW);
    return bRet;
}

/*++
Function:
  SetFileAttributesW

Notes:
  Used for setting read-only attribute on file only.

--*/
BOOL
PALAPI
SetFileAttributesW(
           IN LPCWSTR lpFileName,
           IN DWORD dwFileAttributes)
{
    char name[MAX_PATH];
    int  size;

    DWORD dwLastError = 0;
    BOOL  bRet = FALSE;

    PERF_ENTRY(SetFileAttributesW);
    ENTRY("SetFileAttributesW(lpFileName=%p (%S), dwFileAttributes=%#x)\n",
        lpFileName?lpFileName:W16_NULLSTRING,
        lpFileName?lpFileName:W16_NULLSTRING, dwFileAttributes);

    if (lpFileName == NULL) 
    {
        dwLastError = ERROR_PATH_NOT_FOUND;
        goto done;
    }
    
    size = WideCharToMultiByte( CP_ACP, 0, lpFileName, -1, name, MAX_PATH,
                                NULL, NULL );
    if( size == 0 )
    {
        dwLastError = GetLastError();
        if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
        {
            ERROR("lpFileName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }
    bRet = SetFileAttributesA(name,dwFileAttributes);

done:
    if (dwLastError) SetLastError(dwLastError);

    LOGEXIT("SetFileAttributes returns BOOL %d\n", bRet);
    PERF_EXIT(SetFileAttributesW);
    return bRet;
}


/*++
Function:
  WriteFileW

Note:
  lpOverlapped always NULL.

See MSDN doc.
--*/
BOOL
PALAPI
WriteFile(
      IN HANDLE hFile,
      IN LPCVOID lpBuffer,
      IN DWORD nNumberOfBytesToWrite,
      OUT LPDWORD lpNumberOfBytesWritten,
      IN LPOVERLAPPED lpOverlapped)
{
    int res;
    file *file_data = NULL;
    BOOL ret = FALSE, bLocked = FALSE;

    DWORD dwLastError = 0;
    DWORD writeOffsetStartLow = 0, writeOffsetStartHigh = 0;
    UINT64 writeOffsetStart = 0;

    PERF_ENTRY(WriteFile);
    ENTRY("WriteFile(hFile=%p, lpBuffer=%p, nToWrite=%u, lpWritten=%p, "
          "lpOverlapped=%p)\n", hFile, lpBuffer, nNumberOfBytesToWrite, 
          lpNumberOfBytesWritten, lpOverlapped);

    if ( hFile == INVALID_HANDLE_VALUE )
    {
        dwLastError = ERROR_INVALID_HANDLE;
        goto done;
    }
    else if ( lpOverlapped )
    {
        ASSERT( "lpOverlapped is not NULL, as it should be.\n" );
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }
    else if ( !lpNumberOfBytesWritten )
    {
        ASSERT( "lpNumberOfBytesWritten is NULL\n" );
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }

    /* as per MSDN doc, this is the first thing done */
    *lpNumberOfBytesWritten = 0;

    file_data = FILEAcquireFileStruct(hFile);
    if ( !file_data )
    {
        ERROR("Could not extract structure from handle %p\n", hFile);
        dwLastError = ERROR_INVALID_HANDLE;
        goto done;
    }

    /* we need to lock the region to be written to avoid locking a region using
       LockFile while writing on it */
    
    if (file_data->unix_filename != NULL)
    {
        /* Get the current file position to calculate the region to lock */
        writeOffsetStartLow = SetFilePointer(hFile, 0, &writeOffsetStartHigh,
                                             FILE_CURRENT);

        if (writeOffsetStartLow == INVALID_SET_FILE_POINTER)
        {
            ASSERT("Failed to get the current file position\n");
            dwLastError = ERROR_INTERNAL_ERROR;
            goto done;
        }

        writeOffsetStart = ((UINT64) writeOffsetStartHigh) << 32 | writeOffsetStartLow;

        bLocked = FILELockFileRegion(file_data, writeOffsetStart,
                                     nNumberOfBytesToWrite, RDWR_LOCK_RGN);
        if (bLocked == FALSE)
        {
            ERROR("Failed to lock file region !\n");
            dwLastError = ERROR_LOCK_VIOLATION;
            goto done;
        }
    }

#ifdef WRITE_0_BYTES_HANGS_TTY
    if( nNumberOfBytesToWrite == 0 && isatty(file_data->unix_fd) )
    {
        res = 0;
        ret = TRUE;
        *lpNumberOfBytesWritten = 0;
        goto unlock;
    }
#endif

    res = write( file_data->unix_fd, lpBuffer, nNumberOfBytesToWrite );  
    TRACE("write() returns %d\n", res);

    if ( res >= 0 )
    {
        ret = TRUE;
    }

    if ( ret )
    {
        *lpNumberOfBytesWritten = res;
    }
    else
    {
        dwLastError = FILEGetLastErrorFromErrno();
    }

unlock:
    if ((bLocked == TRUE) &&
        FILEUnlockFileRegion(file_data, writeOffsetStart, nNumberOfBytesToWrite,
                             RDWR_LOCK_RGN) == FALSE)
    {
        WARN("Failed to unlock the locked region !\n");
    }

done:
    if (file_data)
    {
        FILEReleaseFileStruct(hFile,file_data);
    }
    if (dwLastError)
    {
        SetLastError(dwLastError);
    }

    LOGEXIT("WriteFile returns BOOL %d\n", ret);
    PERF_EXIT(WriteFile);
    return ret;
}


/*++
Function:
  ReadFileW

Note:
  lpOverlapped always NULL.

See MSDN doc.
--*/
BOOL
PALAPI
ReadFile(
     IN HANDLE hFile,
     OUT LPVOID lpBuffer,
     IN DWORD nNumberOfBytesToRead,
     OUT LPDWORD lpNumberOfBytesRead,
     IN LPOVERLAPPED lpOverlapped)
{
    int  res;
    file *file_data = NULL;
    BOOL ret = FALSE, bLocked = FALSE;
    DWORD readOffsetStartLow = 0, readOffsetStartHigh = 0;
    UINT64 readOffsetStart = 0;

    DWORD dwLastError = 0;

    PERF_ENTRY(ReadFile);
    ENTRY("ReadFile(hFile=%p, lpBuffer=%p, nToRead=%u, "
          "lpRead=%p, lpOverlapped=%p)\n",
          hFile, lpBuffer, nNumberOfBytesToRead, 
          lpNumberOfBytesRead, lpOverlapped);

    if ( hFile == INVALID_HANDLE_VALUE )
    {
        ERROR( "Invalid file handle\n" );
        dwLastError = ERROR_INVALID_HANDLE;
        goto done;
    }
    else if ( lpOverlapped )
    {
        ASSERT( "lpOverlapped is not NULL, as it should be.\n" );
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }
    else if ( !lpNumberOfBytesRead )
    {
        ERROR( "lpNumberOfBytesRead is NULL\n" );
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }
    else if (!lpBuffer || IsBadWritePtr(lpBuffer, nNumberOfBytesToRead))
    {
        ERROR( "Invalid parameter. (lpBuffer:%p)\n", lpBuffer);
        SetLastError( ERROR_NOACCESS );
        goto done;
    }

    /* as per MSDN doc, this is the first thing done */
    *lpNumberOfBytesRead = 0;

    file_data = FILEAcquireFileStruct(hFile);
    if ( !file_data )
    {
        ERROR("Could not extract structure from handle %p\n", hFile);
        dwLastError = ERROR_INVALID_HANDLE;
        goto done;
    }

    /* we need to lock the region to be written to avoid locking a region using 
       LockFile while writing on it */
    if (file_data->unix_filename != NULL)                           
    {
        /* Get the current file position to calculate the region to lock */
        readOffsetStartLow = SetFilePointer(hFile, 0, &readOffsetStartHigh, 
                                             FILE_CURRENT);
    
        if (readOffsetStartLow == INVALID_SET_FILE_POINTER) 
        {
            ASSERT("Failed to get the current file position\n");
            dwLastError = ERROR_INTERNAL_ERROR;
            goto done;
        }
    
        readOffsetStart = ((UINT64) readOffsetStartHigh) << 32 | readOffsetStartLow;

        bLocked = FILELockFileRegion(file_data, readOffsetStart, 
                                     nNumberOfBytesToRead, RDWR_LOCK_RGN);

        if (bLocked == FALSE)
        {
            ERROR("Failed to lock file region !\n");
            dwLastError = ERROR_LOCK_VIOLATION;
            goto done;
        }
    }
    
    TRACE( "Reading from file descriptor %d\n", file_data->unix_fd );

Read:
    res = read( file_data->unix_fd, lpBuffer, nNumberOfBytesToRead );
    TRACE("read() returns %d\n", res);

    if ( res > 0 )
    {
        *lpNumberOfBytesRead = res;
        ret = TRUE;
    }
    else 
    if ( res == 0 )
    {
        ret = TRUE;
    }
    else
    {
        if ( errno == EBADF )
        {
            dwLastError = ERROR_ACCESS_DENIED;
        }
        else
        {
            if (errno == EINTR)
            {
                // Try to read again.
                goto Read;
            }
            dwLastError = ERROR_READ_FAULT;
        }
    }

    if ((bLocked == TRUE) && 
        FILEUnlockFileRegion(file_data, readOffsetStart, nNumberOfBytesToRead, 
                             RDWR_LOCK_RGN) == FALSE)
    {
        WARN("Failed to unlock the locked region !\n");
    }

done:
    if (file_data)
    {
        FILEReleaseFileStruct(hFile,file_data);
    }
    if (dwLastError)
    {
        SetLastError(dwLastError);
    }

    LOGEXIT("ReadFile returns BOOL %d\n", ret);
    PERF_EXIT(ReadFile);
    return ret;
}


/*++
Function:
  GetStdHandle

See MSDN doc.
--*/
HANDLE
PALAPI
GetStdHandle(
         IN DWORD nStdHandle)
{
    HANDLE hRet = INVALID_HANDLE_VALUE;

    PERF_ENTRY(GetStdHandle);
    ENTRY("GetStdHandle(nStdHandle=%#x)\n", nStdHandle);

    switch( nStdHandle )
    {
    case STD_INPUT_HANDLE:
        hRet = pStdIn;
        break;
    case STD_OUTPUT_HANDLE:
        hRet = pStdOut;
        break;
    case STD_ERROR_HANDLE:
        hRet = pStdErr; 
        break;
    default:
        ERROR("nStdHandle is invalid\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        break;
    }

    LOGEXIT("GetStdHandle returns HANDLE %p\n", hRet);
    PERF_EXIT(GetStdHandle);
    return hRet;
}



/*++
Function:
  SetEndOfFile

See MSDN doc.
--*/
BOOL
PALAPI
SetEndOfFile(
         IN HANDLE hFile)
{
    file *file_data;
    DWORD dwLastError = 0;
    off_t curr;

    BOOL bRet = FALSE;

    PERF_ENTRY(SetEndOfFile);
    ENTRY("SetEndOfFile(hFile=%p)\n", hFile);

    file_data = FILEAcquireFileStruct(hFile);
    if ( !file_data )
    {
        ERROR("Could not extract structure from handle %p\n", hFile);
        dwLastError = ERROR_INVALID_HANDLE;
        goto done;
    }

    if (file_data->open_flags_deviceaccessonly == TRUE)
    {
        ERROR("No write access on file\n");
        dwLastError = ERROR_NOACCESS;
        goto done;
    }
    
    curr = lseek( file_data->unix_fd, 0, SEEK_CUR );

    TRACE("current file pointer offset is %u\n", curr);
    if ( curr < 0 )
    {
        ERROR("lseek returned %ld\n", curr);
        dwLastError = FILEGetLastErrorFromErrno();
        goto done;
    }

#if SIZEOF_OFF_T > 4
#if !HAVE_FTRUNCATE_LARGE_LENGTH_SUPPORT
    if (curr >= 0xFFFFFFFF000)
    {
        ERROR("Skipping ftruncate because the offset is too large\n");
        dwLastError = ERROR_INVALID_PARAMETER;
        goto done;
    }
#endif  // !HAVE_FTRUNCATE_LARGE_LENGTH_SUPPORT
#endif  // SIZEOF_OFF_T

    if ( ftruncate(file_data->unix_fd, curr) != 0 )
    {
        ERROR("ftruncate failed\n");
        if ( errno == EACCES )
        {
            ERROR("file may not be writable\n");
        }
        dwLastError = FILEGetLastErrorFromErrno();
        goto done;
    }

    bRet = TRUE;

done:
    if (file_data)
    {
        FILEReleaseFileStruct(hFile, file_data);
    }
    if (dwLastError)
    {
        SetLastError(dwLastError);
    }

    LOGEXIT("SetEndOfFile returns BOOL %d\n", bRet);
    PERF_EXIT(SetEndOfFile);
    return bRet;
}


/*++
Function:
  SetFilePointer

See MSDN doc.
--*/
DWORD
PALAPI
SetFilePointer(
           IN HANDLE hFile,
           IN LONG lDistanceToMove,
           IN PLONG lpDistanceToMoveHigh,
           IN DWORD dwMoveMethod)
{
    DWORD       dwLastError = 0;
    DWORD   dwRet = INVALID_SET_FILE_POINTER;
    int     seek_whence = 0;
    __int64 seek_offset = 0LL;
    __int64 seek_res = 0LL;
    off_t old_offset;

    file        *file_data = NULL;

    PERF_ENTRY(SetFilePointer);
    ENTRY("SetFilePointer(hFile=%p, lDistance=%d, lpDistanceHigh=%p, "
          "dwMoveMethod=%#x)\n", hFile, lDistanceToMove,
          lpDistanceToMoveHigh, dwMoveMethod);

    switch( dwMoveMethod )
    {
    case FILE_BEGIN:
        seek_whence = SEEK_SET; 
        break;
    case FILE_CURRENT:
        seek_whence = SEEK_CUR; 
        break;
    case FILE_END:
        seek_whence = SEEK_END; 
        break;
    default:
        ERROR("dwMoveMethod = %d is invalid\n", dwMoveMethod);
        SetLastError( ERROR_INVALID_PARAMETER );
        goto done;
    }

    file_data = FILEAcquireFileStruct(hFile);
    if ( !file_data )
    {
        ERROR("Could not extract structure from handle %p\n", hFile);
        dwLastError = ERROR_INVALID_HANDLE;
        goto done;
    }

    /* according to MSDN, if lpDistanceToMoveHigh is not null, 
       lDistanceToMove is treated as unsigned; 
       it is treated as signed otherwise                 */
    if ( lpDistanceToMoveHigh )
    {
        /* set the high 32 bits of the offset */
        seek_offset = ((__int64)*lpDistanceToMoveHigh << 32);
        /* set the low 32 bits */
        /* cast to unsigned long to avoid sign extension */
        seek_offset |= (unsigned long) lDistanceToMove;
    }
    else
    {
        seek_offset |= lDistanceToMove;
    }

    /* store the current position, in case the lseek moves the pointer
       before the beginning of the file */
    old_offset = lseek(file_data->unix_fd, 0, SEEK_CUR);
    if (old_offset == -1)
    {
        ERROR("lseek(fd,0,SEEK_CUR) failed errno:%d (%s)\n", 
              errno, strerror(errno));
        dwLastError = ERROR_ACCESS_DENIED;
        goto done;
    }
    
    // Check to see if we're going to seek to a negative offset.
    // If we're seeking from the beginning or the current mark,
    // this is simple.
    if ((seek_whence == SEEK_SET && seek_offset < 0) ||
        (seek_whence == SEEK_CUR && seek_offset + old_offset < 0))
    {
        dwLastError = ERROR_NEGATIVE_SEEK;
        goto done;
    }
    else if (seek_whence == SEEK_END && seek_offset < 0)
    {
        // We need to determine if we're seeking past the
        // beginning of the file, but we don't want to adjust
        // the mark in the process. stat is the only way to
        // do that.
        struct stat fileData;
        int result;
        
        result = fstat(file_data->unix_fd, &fileData);
        if (result == -1)
        {
            // It's a bad fd. This shouldn't happen because
            // we've already called lseek on it, but you
            // never know. This is the best we can do.
            dwLastError = ERROR_ACCESS_DENIED;
            goto done;
        }
        if (fileData.st_size < -seek_offset)
        {
            // Seeking past the beginning.
            dwLastError = ERROR_NEGATIVE_SEEK;
            goto done;
        }
    }

    seek_res = (__int64)lseek( file_data->unix_fd,
                               seek_offset,
                               seek_whence );
    if ( seek_res < 0 )
    {
        /* lseek() returns -1 on error, but also can seek to negative
           file offsets, so -1 can also indicate a successful seek to offset
           -1.  Win32 doesn't allow negative file offsets, so either case
           is an error. */
        ERROR("lseek failed errno:%d (%s)\n", errno, strerror(errno));
        lseek(file_data->unix_fd, old_offset, SEEK_SET);
        dwLastError = ERROR_ACCESS_DENIED;
    }
    else
    {
        /* store high-order DWORD */
        if ( lpDistanceToMoveHigh )
            *lpDistanceToMoveHigh = (DWORD)(seek_res >> 32);
    
        /* return low-order DWORD of seek result */
        dwRet = (DWORD)seek_res;
    }
done:
    if (file_data)
    {
        FILEReleaseFileStruct(hFile, file_data);
    }
    /* This function must always call SetLastError - even if successful. 
       If we seek to a value greater than 2^32 - 1, we will effectively be
       returning a negative value from this function. Now, let's say that
       returned value is -1. Furthermore, assume that win32error has been 
       set before even entering this function. Then, when this function 
       returns to SetFilePointer in win32native.cs, it will have returned 
       -1 and win32error will have been set, which will cause an error to be
       returned. Since -1 may not be an error in this case and since we 
       can't assume that the win32error is related to SetFilePointer, 
       we need to always call SetLastError here. That way, if this function 
       succeeds, SetFilePointer in win32native won't mistakenly determine 
       that it failed. */
    SetLastError(dwLastError);
    LOGEXIT("SetFilePointer returns DWORD %#x\n", dwRet);
    PERF_EXIT(SetFilePointer);
    return dwRet;
}


/*++
Function:
  GetFileSize

See MSDN doc.
--*/
DWORD
PALAPI
GetFileSize(
        IN HANDLE hFile,
        OUT LPDWORD lpFileSizeHigh)
{
    file *file_data;
    struct stat stat_data;

    DWORD dwLastError = 0;
    DWORD dwRet = -1;

    PERF_ENTRY(GetFileSize);
    ENTRY("GetFileSize(hFile=%p, lpFileSizeHigh=%p)\n", hFile, lpFileSizeHigh);

    file_data = FILEAcquireFileStruct(hFile);
    if ( !file_data )
    {
        ERROR("Could not extract structure from handle %p\n", hFile);
        dwLastError = ERROR_INVALID_HANDLE;
        goto done;
    }
    
    if ( fstat(file_data->unix_fd, &stat_data) != 0 )
    {
        ERROR("fstat failed of file descriptor %d\n", file_data->unix_fd);
        dwLastError = FILEGetLastErrorFromErrno();
        goto done;
    }

    dwRet = (DWORD)stat_data.st_size;
    
    if ( lpFileSizeHigh )
    {
#if SIZEOF_OFF_T > 4
        *lpFileSizeHigh = (DWORD)(stat_data.st_size >> 32);
#else
        *lpFileSizeHigh = 0;
#endif
    }

done:
    if (file_data)
    {
        FILEReleaseFileStruct(hFile,file_data);
    }
    if (dwLastError)
    {
        SetLastError(dwLastError);
    }

    LOGEXIT("GetFileSize returns DWORD %u\n", dwRet);
    PERF_EXIT(GetFileSize);
    return dwRet;
}


/*++
Function:
  FlushFileBuffers

See MSDN doc.
--*/
BOOL
PALAPI
FlushFileBuffers(
         IN HANDLE hFile)
{
    file  *file_data;
    DWORD dwLastError = 0;
    BOOL  bRet = FALSE;

    PERF_ENTRY(FlushFileBuffers);
    ENTRY("FlushFileBuffers(hFile=%p)\n", hFile);

    file_data = FILEAcquireFileStruct(hFile);
    if ( !file_data )
    {
        ERROR("Could not extract structure from handle %p\n", hFile);
        dwLastError = ERROR_INVALID_HANDLE;
        goto done;
    }

    if (file_data->open_flags_deviceaccessonly == TRUE)
    {
        ERROR("No write access on file\n");
        dwLastError = ERROR_NOACCESS;
        goto done;
    }
    
    /* flush all buffers out to disk - there is no way to flush
       an individual file descriptor's buffers out. */
    sync();

    bRet = TRUE;

done:
    if (file_data)
    {
        FILEReleaseFileStruct(hFile,file_data);
    }
    if (dwLastError)
    {
        SetLastError(dwLastError);
    }

    LOGEXIT("FlushFileBuffers returns BOOL %d\n", bRet);
    PERF_EXIT(FlushFileBuffers);
    return bRet;
}


/*++
Function:
  GetFileType

See MSDN doc.

--*/
DWORD
PALAPI
GetFileType(
        IN HANDLE hFile)
{
    file *file_data;
    struct stat stat_data;

    DWORD dwLastError = 0;
    DWORD dwRet = FILE_TYPE_UNKNOWN;

    PERF_ENTRY(GetFileType);
    ENTRY("GetFileType(hFile=%p)\n", hFile);

    file_data = FILEAcquireFileStruct(hFile);
    if ( !file_data )
    {
        ERROR("Could not extract structure from handle %p\n", hFile);
        dwLastError = ERROR_INVALID_HANDLE;
        goto done;
    }
    
    if ( fstat(file_data->unix_fd, &stat_data) != 0 )
    {
        ERROR("fstat failed of file descriptor %d\n", file_data->unix_fd);
        dwLastError = FILEGetLastErrorFromErrno();
        goto done;
    }

    TRACE("st_mode & S_IFMT = %#x\n", stat_data.st_mode & S_IFMT);
    if (S_ISREG(stat_data.st_mode) || S_ISDIR(stat_data.st_mode))
    {
        dwRet = FILE_TYPE_DISK;
    }
    else if (S_ISCHR(stat_data.st_mode))
    {
        dwRet = FILE_TYPE_CHAR;
    }
    else if (S_ISFIFO(stat_data.st_mode))
    {
        dwRet = FILE_TYPE_PIPE;
    }
    else
    {
        dwRet = FILE_TYPE_UNKNOWN;
    }

done:
    if (file_data)
    {
        FILEReleaseFileStruct(hFile,file_data);
    }
    if (dwLastError)
    {
        SetLastError(dwLastError);
    }

    LOGEXIT("GetFileType returns DWORD %#x\n", dwRet);
    PERF_EXIT(GetFileType);
    return dwRet;
}

#define ENSURE_UNIQUE_NOT_ZERO \
    if ( uUniqueSeed == 0 ) \
    {\
        uUniqueSeed++;\
    }

/*++
 Function:
   GetTempFileNameA

uUnique is always 0.
 --*/
const int MAX_PREFIX        = 3;
const int MAX_SEEDSIZE      = 8; /* length of "unique portion of 
                                   the string, plus extension(FFFF.TMP). */
static USHORT uUniqueSeed   = 0;
static BOOL IsInitialized   = FALSE;

UINT
PALAPI
GetTempFileNameA(
                 IN LPCSTR lpPathName,
                 IN LPCSTR lpPrefixString,
                 IN UINT   uUnique,
                 OUT LPSTR lpTempFileName)
{
    CHAR    full_name[ MAX_PATH + 1 ];
    CHAR    file_template[ MAX_PATH + 1 ];
 
    HANDLE  hTempFile;
    UINT    uRet = 0;
    DWORD   dwError;
    USHORT  uLoopCounter = 0;

    PERF_ENTRY(GetTempFileNameA);
    ENTRY("GetTempFileNameA(lpPathName=%p (%s), lpPrefixString=%p (%s), uUnique=%u, " 
          "lpTempFileName=%p)\n",  lpPathName?lpPathName:"NULL",  lpPathName?lpPathName:"NULL", 
        lpPrefixString?lpPrefixString:"NULL", 
        lpPrefixString?lpPrefixString:"NULL", uUnique, 
        lpTempFileName?lpTempFileName:"NULL");


    if ( !IsInitialized )
    {
        uUniqueSeed = (USHORT)( time( NULL ) );
    
        /* On the off chance 0 is returned.
        0 being the error return code.  */
        ENSURE_UNIQUE_NOT_ZERO
        IsInitialized = TRUE;
    }

    if ( !lpPathName || *lpPathName == '\0' )
    {
       SetLastError( ERROR_DIRECTORY );
       goto done;
    }

    if ( NULL == lpTempFileName )  
    {
        ERROR( "lpTempFileName cannot be NULL\n" );
        SetLastError( ERROR_INVALID_PARAMETER );
        goto done;
    }

    if ( strlen( lpPathName ) + MAX_SEEDSIZE + MAX_PREFIX >= MAX_PATH ) 
    {
        ERROR( "File names larger than MAX_PATH (%d)!\n", MAX_PATH );
        SetLastError( ERROR_INVALID_PARAMETER );
        goto done;
    }

    *file_template = '\0';
    strcat( file_template, lpPathName );
    strcat( file_template, "\\" );
    
    if ( lpPrefixString )
    {
        strncat( file_template, lpPrefixString, MAX_PREFIX );
    }
    FILEDosToUnixPathA( file_template );
    strncat( file_template, "%.4x.TMP", MAX_SEEDSIZE );

    /* Create the file. */
    dwError = GetLastError();
    SetLastError( NOERROR );

    sprintf( full_name, file_template, uUniqueSeed );
    
    hTempFile = CreateFileA( full_name, GENERIC_WRITE, 
                             FILE_SHARE_READ, NULL, CREATE_NEW, 0, NULL );
    
    /* The USHORT will overflow back to 0 if we go past
    65536 files, so break the loop after 65536 iterations.
    If the CreateFile call was not successful within that 
    number of iterations, then there are no temp file names
    left for that directory. */
    while ( ERROR_PATH_NOT_FOUND != GetLastError() && 
            INVALID_HANDLE_VALUE == hTempFile && uLoopCounter < 0xFFFF )
    {
        uUniqueSeed++;
        ENSURE_UNIQUE_NOT_ZERO;

        SetLastError( NOERROR );
        sprintf( full_name, file_template, uUniqueSeed );
        hTempFile = CreateFileA( full_name, GENERIC_WRITE, 
                                 FILE_SHARE_READ, NULL, CREATE_NEW, 0, NULL );
        uLoopCounter++;
    
    }

    /* Reset the error code.*/
    if ( NOERROR == GetLastError() )
    {
        SetLastError( dwError );
    }

    /* Windows sets ERROR_FILE_EXISTS,if there
    are no available temp files. */
    if ( INVALID_HANDLE_VALUE != hTempFile )
    {
        uRet = uUniqueSeed;
        uUniqueSeed++;
        ENSURE_UNIQUE_NOT_ZERO;
        
        if ( CloseHandle( hTempFile ) )
        {
            strcpy( lpTempFileName, full_name );
        }
        else  
        {
            ASSERT( "Unable to close the handle %p\n", hTempFile );
            SetLastError( ERROR_INTERNAL_ERROR );
            *lpTempFileName = '\0';
            uRet = 0;
        }
    }
    else if ( INVALID_HANDLE_VALUE == hTempFile && uLoopCounter < 0xFFFF )
    {
        ERROR( "Unable to create temp file. \n" );
        uRet = 0;
        
        if ( ERROR_PATH_NOT_FOUND == GetLastError() )
        {
            /* CreateFile failed because it could not 
            find the path. */
            SetLastError( ERROR_DIRECTORY );
        } /* else use the lasterror value from CreateFileA */
    }
    else
    {
        TRACE( "65535 files already exist in the directory. "
               "No temp files available for creation.\n" );
        SetLastError( ERROR_FILE_EXISTS );
    }

done:
    LOGEXIT("GetTempFileNameA returns UINT %u\n", uRet);
    PERF_EXIT(GetTempFileNameA);
    return uRet;
       
}
        
/*++
Function:
  GetTempFileNameW

uUnique is always 0.
--*/
UINT
PALAPI
GetTempFileNameW(
         IN LPCWSTR lpPathName,
         IN LPCWSTR lpPrefixString,
         IN UINT uUnique,
         OUT LPWSTR lpTempFileName)
{
    INT path_size = 0;
    INT prefix_size = 0;
    CHAR full_name[ MAX_PATH + 1 ];
    CHAR prefix_string[ MAX_PATH + 1 ];
    CHAR tempfile_name[ MAX_PATH + 1 ];
    UINT   uRet;

    PERF_ENTRY(GetTempFileNameW);
    ENTRY("GetTempFileNameW(lpPathName=%p (%S), lpPrefixString=%p (%S), uUnique=%u, "
          "lpTempFileName=%p)\n", lpPathName?lpPathName:W16_NULLSTRING, lpPathName?lpPathName:W16_NULLSTRING,
          lpPrefixString?lpPrefixString:W16_NULLSTRING,
          lpPrefixString?lpPrefixString:W16_NULLSTRING,uUnique, lpTempFileName);

    /* Sanity checks. */
    if ( !lpPathName || *lpPathName == '\0' )
    {
        SetLastError( ERROR_DIRECTORY );
        uRet = 0;
        goto done;
    }

    path_size = WideCharToMultiByte( CP_ACP, 0, lpPathName, -1, full_name,
                                     MAX_PATH, NULL, NULL );
    if( path_size == 0 )
    {
        DWORD dwLastError = GetLastError();
        if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
        {
            ERROR("lpPathName is larger than MAX_PATH (%d)!\n", MAX_PATH);
        }
        else
        {
            ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        uRet = 0;
        goto done;
    }
    
    if (lpPrefixString != NULL) 
    {
        prefix_size = WideCharToMultiByte( CP_ACP, 0, lpPrefixString, -1, 
                                           prefix_string,
                                           MAX_PATH - path_size - MAX_SEEDSIZE, 
                                           NULL, NULL );
        if( prefix_size == 0 )
        {
            DWORD dwLastError = GetLastError();
            if( dwLastError == ERROR_INSUFFICIENT_BUFFER )
            {
                ERROR("Full name would be larger than MAX_PATH (%d)!\n", MAX_PATH);
            }
            else
            {
                ASSERT("WideCharToMultiByte failure! error is %d\n", dwLastError);
            }
            SetLastError( ERROR_INVALID_PARAMETER);
            uRet = 0;
            goto done;
        }
    }
       
    uRet = GetTempFileNameA(full_name, 
                            (lpPrefixString == NULL) ? NULL : prefix_string,
                            0, tempfile_name);
        
    if ( uRet && !MultiByteToWideChar( CP_ACP, 0, tempfile_name, -1, 
                                       lpTempFileName, MAX_PATH ))
    {
        DWORD dwLastError = GetLastError();
        if (dwLastError == ERROR_INSUFFICIENT_BUFFER)
        {
            ERROR("File names larger than MAX_PATH (%d)! \n", MAX_PATH);
        }
        else
        {
            ASSERT("MultiByteToWideChar failure! error is %d", dwLastError);     
        }
        SetLastError(dwLastError);
        uRet = 0;
        goto done;
    }

done:
    LOGEXIT("GetTempFileNameW returns UINT %u\n", uRet);
    PERF_EXIT(GetTempFileNameW);
    return uRet;
}


/*++
Function:
  FILEAcquireFileStruct

Lock and return the file structure associated with a given handle if it is 
valid. If it is not valid, return NULL.
--*/
file *FILEAcquireFileStruct( HANDLE handle )
{
    file * file_data;

    if ( handle == INVALID_HANDLE_VALUE )
    {
        ERROR("Received handle == INVALID_HANDLE_VALUE\n");
        return NULL;
     }

   file_data = (file*)HMGRLockHandle2(handle, HOBJ_FILE);
    
    if ( file_data == NULL )
    {
        ERROR("Not a file handle\n");
        return NULL;
    }

    
    return file_data;
}


/*++
Function:
  FILEReleaseFileStruct

Release control of a file structure, to be called sometime after a call
to FILEAcquireFileStruct.
--*/
void FILEReleaseFileStruct( HANDLE handle, file *file_data )
{
    if ( file_data == NULL || file_data->self_addr != file_data )
    {
        ERROR("Invalid file data parameter\n");
        return;
    }

    HMGRUnlockHandle(handle,&file_data->handle_data);
}


/*++
Function:
  FILENewFileData

Initialize a new file structure for use with a new file handle.
--*/
static file *FILENewFileData( void )
{
    file *data;

    data = (file*)malloc( sizeof(file) );

    if (data == NULL)
    {
        return NULL;
    }

    data->handle_data.type = HOBJ_FILE;
    data->handle_data.dup_handle = FILEDuplicateHandle;
    data->handle_data.close_handle = FILECloseHandle;

    data->self_addr = data;

    data->shmFileLocks = 0;

    data->unix_filename = NULL;
    data->open_flags_deviceaccessonly = FALSE;
    data->unix_fd = -1;
    data->inheritable = FALSE;

    return data;
}


/*++
Function:
  FILECloseHandle
Misc cleanup for when the handle is closed.

parameters, return value : see definition of CLOSEHANDLEFUNC in handle.h
--*/
static int FILECloseHandle( HOBJSTRUCT *handle_data)
{

    int ret = 0, saved_errno = -1;
    file *file_data = (file*)handle_data;

    if ( file_data == NULL || file_data->self_addr != file_data )
    {
        ASSERT("Not a file handle\n");
        return -1;
    }


    file_data->self_addr = 0;

    if ( file_data->unix_fd >= 0 )
    {
        ret = close( file_data->unix_fd );
            
        if ( ret != 0 )
        {
             saved_errno = errno; // save the errno
             ERROR("close(%d) failed, errno is %d (%s)\n",
             file_data->unix_fd, errno, strerror(errno));
        }
    }


    free(file_data->unix_filename);
    FILECleanUpLockedRgn(file_data);

    free( file_data );

    if (saved_errno != -1)
    {
        errno = saved_errno;
    }

    return ret;      
}

/*++
Function:
  FILECloseStdHandle

Misc cleanup for when the handle is closed. This should only be called when closing
standard file handles (stdin, stdout, stderr) due to a pthread library bug in
FreeBSD (see the comment inside the function)


parameters, return value : see definition of CLOSEHANDLEFUNC in handle.h
--*/
static int FILECloseStdHandle( HOBJSTRUCT *handle_data)
{
    FILECloseHandle (handle_data);

    /* WORKAROUND: 
             We are returning 0 here (success) all the time here.  This is
             due to a bug in FreeBSD's Pthread implementation.  Here is the 
             scenario: if you attempt to close STDIN, or STDERR in a program
             that is linked against pthread library running in the background 
             with no shell, it will fail with errno 9, invalid file descriptor.
             We dup() the standard files and place them into our own standard 
             file HANDLEs, which works without error, but during the cleanup
             process close()ing these dup()ed files fails too (in the case
             of running a PAL app in the background and then closing the shell)
             This SHOULD work and reveals a bug in the pthread implementation -
             this behaviour does not show up in a test app that is NOT linked
             against the pthread library.  This bug was present up to and 
             including FreeBSD 4.5.  

             We always return success because there is nothing we can really do 
             about it besides display an ERROR diagnostic (which we do
             in FILECloseHandle) */
    return 0;
}


/*++
Function:
  FILEDuplicateHandle

Increment reference count, etc., when a handle is duplicated.

parameters, return value : see definition of DUPHANDLEFUNC in handle.h
--*/
static int FILEDuplicateHandle( HANDLE handle, HOBJSTRUCT *handle_data)
{
    file *file_data = (file*)handle_data, *data = NULL;
    SHMFILELOCKS *fileLocks;
    BOOL SHM_Locked = FALSE;

    if ( file_data == NULL || file_data->self_addr != file_data )
    {
        ERROR("Not a file handle\n");
        goto EXIT;
    }

    data = (file*)malloc( sizeof(file) );
    if (data == NULL)
    {
        ERROR("Not enough memory\n");
        goto EXIT;
    }
    memcpy(data, file_data, sizeof(file));
    data->unix_filename = NULL;
    data->self_addr = data;
    
    data->unix_fd = dup(file_data->unix_fd);
    if( data->unix_fd == -1)
    {
        ERROR("Cannot duplicate file handle\n");
        goto EXIT;
    }
            
    if(file_data->unix_filename != NULL)
    {
        data->unix_filename = strdup(file_data->unix_filename);
        if( data->unix_filename == NULL)
        {
            ERROR("Not enough memory\n");
            goto EXIT;
        }
    }

    SHMLock();
    SHM_Locked = TRUE;

    if (SHMPTR_TO_PTR_BOOL(fileLocks, file_data->shmFileLocks) == FALSE)
    {
        ERROR("Unable to get pointer from shm pointer.\n");
        goto EXIT;
    }

    if (fileLocks != NULL)
    {
        if ( fileLocks->refCount <= 0 )
        {
            ERROR("Reference count has to be non-zero on a valid lock.\n");
            goto EXIT;
        }
        
        fileLocks->refCount++;
    }

    SHMRelease();
    SHM_Locked = FALSE;

    if(!HMGRReplaceHandleData(handle, (HOBJSTRUCT *) data))
    {
       ERROR("Handle data replace faild\n");
       goto EXIT;
    }

    // Everything went well    
    return 0;

EXIT:
    if ( SHM_Locked == TRUE );
       SHMRelease();

    if ( data != NULL )
    {
       if ( data->unix_filename != NULL );
          free(data->unix_filename);
       if ( data->unix_fd != -1 && close(data->unix_fd) == -1 )
          ERROR("Cannot free file descriptor\n");
       free(data);
    }

    return -1;
}

/*++
Function:
  FILEGetLastErrorFromErrno

Convert errno into the appropriate win32 error and return it.
--*/
DWORD FILEGetLastErrorFromErrno( void )
{
    DWORD dwRet;

    switch(errno)
    {
    case 0:
        dwRet = ERROR_SUCCESS; 
        break;
    case ENOTDIR:
        dwRet = ERROR_PATH_NOT_FOUND; 
        break;
    case ENOENT:
        dwRet = ERROR_FILE_NOT_FOUND; 
        break;
    case EACCES:
        /* FALL THROUGH */
    case EROFS:
        dwRet = ERROR_ACCESS_DENIED; 
        break;
    case EEXIST:
        dwRet = ERROR_ALREADY_EXISTS; 
        break;
    // ENOTEMPTY is the same as EEXIST on AIX. Meaningful when involving directory operations
    case ENOTEMPTY:
        dwRet = ERROR_DIR_NOT_EMPTY; 
        break;
    case EBADF:
        dwRet = ERROR_READ_FAULT; 
        break;
    case ENOMEM:
        dwRet = ERROR_NOT_ENOUGH_MEMORY; 
        break;
    case EISDIR:
        dwRet = ERROR_ACCESS_DENIED;
        break;
    case EPERM:
        dwRet = ERROR_ACCESS_DENIED;
        break;
    default:
        dwRet = ERROR_INTERNAL_ERROR;
    }

    TRACE("errno = %d (%s), LastError = %d\n", errno, strerror(errno), dwRet);

    return dwRet;
}



/*++
Function:
  CopyFileA

See MSDN doc.

Notes:
  There are several (most) error paths here that do not call SetLastError().
This is because we know that CreateFile, ReadFile, and WriteFile will do so,
and will have a much better idea of the specific error.
--*/
BOOL
PALAPI
CopyFileA(
      IN LPCSTR lpExistingFileName,
      IN LPCSTR lpNewFileName,
      IN BOOL bFailIfExists)
{
    HANDLE       hSource = INVALID_HANDLE_VALUE;
    HANDLE       hDest = INVALID_HANDLE_VALUE;
    DWORD        dwDestCreationMode;
    BOOL         bGood = FALSE;
    DWORD        dwSrcFileAttributes;
    struct stat  SrcFileStats;
    
    LPSTR lpUnixPath = NULL;
    const int    buffer_size = 16*1024;
    char         buffer[buffer_size];
    int          bytes_read;
    int          bytes_written;
    int          permissions;


    PERF_ENTRY(CopyFileA);
    ENTRY("CopyFileA(lpExistingFileName=%p (%s), lpNewFileName=%p (%s), bFailIfExists=%d)\n",
          lpExistingFileName?lpExistingFileName:"NULL",
          lpExistingFileName?lpExistingFileName:"NULL",
          lpNewFileName?lpNewFileName:"NULL",
          lpNewFileName?lpNewFileName:"NULL", bFailIfExists);

    if ( bFailIfExists )
    {
        dwDestCreationMode = CREATE_NEW;
    }
    else
    {
        dwDestCreationMode = CREATE_ALWAYS;
    }
    
    hSource = CreateFileA( lpExistingFileName,
               GENERIC_READ,
               FILE_SHARE_READ,
               NULL,
               OPEN_EXISTING,
               0,
               NULL );

    if ( hSource == INVALID_HANDLE_VALUE )
    {
        ERROR("CreateFileA failed for %s\n", lpExistingFileName);
        goto done;
    }

    /* Need to preserve the file attributes */
    dwSrcFileAttributes = GetFileAttributes(lpExistingFileName);
    if (dwSrcFileAttributes == 0xffffffff)
    {
        ERROR("GetFileAttributes failed for %s\n", lpExistingFileName);
        goto done;
    }

    /* Need to preserve the owner/group and chmod() flags */
    lpUnixPath = strdup(lpExistingFileName);
    if ( lpUnixPath == NULL )
    {
        ERROR("malloc() failed\n");
        SetLastError(FILEGetLastErrorFromErrno());
        goto done;
    }
    FILEDosToUnixPathA(lpUnixPath);
    if (stat (lpUnixPath, &SrcFileStats) == -1)
    {
        ERROR("stat() failed for %s\n", lpExistingFileName);
        SetLastError(FILEGetLastErrorFromErrno());
        goto done;
    }

    hDest = CreateFileA( lpNewFileName,
             GENERIC_WRITE,
             FILE_SHARE_READ,
             NULL,
             dwDestCreationMode,
             0,
             NULL );

    if ( hDest == INVALID_HANDLE_VALUE )
    {
        ERROR("CreateFileA failed for %s\n", lpNewFileName);    
        goto done;
    }

    free(lpUnixPath);
    lpUnixPath = strdup(lpNewFileName);
    if ( lpUnixPath == NULL )
    {
        ERROR("malloc() failed\n");
        SetLastError(FILEGetLastErrorFromErrno());
        goto done;
    }
    FILEDosToUnixPathA( lpUnixPath );
    
 
    // We don't set file attributes in CreateFile. The only attribute
    // that is reflected on disk in Unix is read-only, and we set that
    // here.
    permissions = (S_IRWXU | S_IRWXG | S_IRWXO);
    if ((dwSrcFileAttributes & FILE_ATTRIBUTE_READONLY) != 0)
    {
        permissions &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
    }
    
    /* Make sure the new file has the same owner/group and chmod() flags */
    if (chmod(lpUnixPath, SrcFileStats.st_mode & permissions) == -1)
    {
        WARN ("chmod() failed to set mode 0x%x on new file\n",
              SrcFileStats.st_mode & permissions);
        SetLastError(FILEGetLastErrorFromErrno());
        goto done;
    }

    while( (bGood = ReadFile( hSource, &buffer, buffer_size, &bytes_read, NULL ))
           && bytes_read > 0 )
    {
        bGood = ( WriteFile( hDest, &buffer, bytes_read, &bytes_written, NULL )
          && bytes_written == bytes_read);
    }

    if (!bGood)
    {
        ERROR("Copy failed\n");
        if ( bytes_written != bytes_read )
        {
            ASSERT("the bytes written and the bytes read do not match \n");
            SetLastError( ERROR_INTERNAL_ERROR );
        }

        if ( !CloseHandle(hDest) ||
             !DeleteFileA(lpNewFileName) )
        {
            ERROR("Unable to clean up partial copy\n");
        }
        hDest = INVALID_HANDLE_VALUE;

        goto done;
    }
    
done:

    if ( hSource != INVALID_HANDLE_VALUE ) 
    {
        CloseHandle( hSource );
    }
    if ( hDest != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hDest );
    }
    if (lpUnixPath) 
    {
        free(lpUnixPath);
    }

    LOGEXIT("CopyFileA returns BOOL %d\n", bGood);
    PERF_EXIT(CopyFileA);
    return bGood;
}


/*++
Function:
  SetFileAttributesA

Notes:
  Used for setting read-only attribute on file only.

--*/
BOOL
PALAPI
SetFileAttributesA(
           IN LPCSTR lpFileName,
           IN DWORD dwFileAttributes)
{
    struct stat stat_data;
    mode_t new_mode;

    DWORD dwLastError = 0;
    BOOL  bRet = FALSE;
    LPSTR UnixFileName = NULL;

    PERF_ENTRY(SetFileAttributesA);
    ENTRY("SetFileAttributesA(lpFileName=%p (%s), dwFileAttributes=%#x)\n",
        lpFileName?lpFileName:"NULL",
        lpFileName?lpFileName:"NULL", dwFileAttributes);

    if ( (dwFileAttributes & FILE_ATTRIBUTE_NORMAL) &&
         (dwFileAttributes != FILE_ATTRIBUTE_NORMAL) )
    {
        WARN("Ignoring FILE_ATTRIBUTE_NORMAL -- it must be used alone\n");
    }

    if (lpFileName == NULL)
    {
        dwLastError = ERROR_FILE_NOT_FOUND;
        goto done;
    }

    if ((UnixFileName = strdup(lpFileName )) == NULL)
    {
        ERROR("strdup() failed\n");
        dwLastError = ERROR_NOT_ENOUGH_MEMORY;
        goto done;
    }

    FILEDosToUnixPathA( UnixFileName );
    if ( stat(UnixFileName, &stat_data) != 0 )
    {
        TRACE("stat failed on %s; errno is %d (%s)\n", 
             UnixFileName, errno, strerror(errno));
        
        if ( ENOENT == errno )
        {
            FILEGetProperNotFoundError( UnixFileName, &dwLastError );
            if ( ERROR_INTERNAL_ERROR == dwLastError )
            {
                goto done;
            }
        }
        else
        {
            dwLastError = FILEGetLastErrorFromErrno();
        }
        goto done;
    }

    new_mode = stat_data.st_mode;
    TRACE("st_mode is %#x\n", new_mode);

    /* if we can't do GetFileAttributes on it, don't do SetFileAttributes */
    if ( !(new_mode & S_IFREG) && !(new_mode & S_IFDIR) )
    {
        ERROR("Not a regular file or directory, S_IFMT is %#x\n",
              new_mode & S_IFMT);
        dwLastError = ERROR_ACCESS_DENIED;
        goto done;
    }

    /* set or unset the "read-only" attribute */
    if ( dwFileAttributes & FILE_ATTRIBUTE_READONLY )
    {
        /* remove the write bit from everybody */
        new_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
    }
    else
    {
        /* give write permission to the owner if the owner
         * already has read permission */
        if ( new_mode & S_IRUSR )
        {
            new_mode |= S_IWUSR;
        }
    }
    TRACE("new mode is %#x\n", new_mode);

    bRet = TRUE;
    if ( new_mode != stat_data.st_mode )
    {
        if ( chmod(UnixFileName, new_mode) != 0 )
        {
            ERROR("chmod(%s, %#x) failed\n", UnixFileName, new_mode);
            dwLastError = FILEGetLastErrorFromErrno();
            bRet = FALSE;
        }
    }

done:
    if (dwLastError)
        SetLastError(dwLastError);
    free( UnixFileName );

    LOGEXIT("SetFileAttributesA returns BOOL %d\n", bRet);
    PERF_EXIT(SetFileAttributesA);
    return bRet;
}

/*++
Function:
  CreatePipe

See MSDN doc.
--*/
PALIMPORT
BOOL
PALAPI
CreatePipe(
        OUT PHANDLE hReadPipe,
        OUT PHANDLE hWritePipe,
        IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
        IN DWORD nSize)
{
    BOOL ret = FALSE;
    int readWritePipeDes[2] = {-1, -1};
    file *readpipe_data = NULL;
    file *writepipe_data = NULL;

    PERF_ENTRY(CreatePipe);
    ENTRY("CreatePipe(hReadPipe:%p, hWritePipe:%p, lpPipeAttributes:%p, nSize:%d\n",
          hReadPipe, hWritePipe, lpPipeAttributes, nSize);

    if ((hReadPipe == NULL) || (hWritePipe == NULL))
    {
        ERROR("One of the two parameters hReadPipe(%p) and hWritePipe(%p) is Null\n",hReadPipe,hWritePipe);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto EXIT;
    }

    if ((lpPipeAttributes == NULL) || 
        (lpPipeAttributes->bInheritHandle == FALSE) ||
        (lpPipeAttributes->lpSecurityDescriptor != NULL))
    {
        ASSERT("invalid security attributes!\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto EXIT;
    }

    *hReadPipe = *hWritePipe = INVALID_HANDLE_VALUE;

    if (pipe(readWritePipeDes) == -1)
    {
        ASSERT("pipe() call failed errno:%d (%s) \n", errno, strerror(errno));
        SetLastError(ERROR_INTERNAL_ERROR);
        goto EXIT;
    }

    /* enable close-on-exec for both pipes; if one gets passed to CreateProcess
       it will be "uncloseonexeced" in order to be inherited */
    if(-1 == fcntl(readWritePipeDes[0],F_SETFD,1))
    {
        ASSERT("can't set close-on-exec flag; fcntl() failed. errno is %d "
             "(%s)\n", errno, strerror(errno));
        close(readWritePipeDes[0]);
        close(readWritePipeDes[1]);
        SetLastError(ERROR_INTERNAL_ERROR);
        goto EXIT;
    }
    if(-1 == fcntl(readWritePipeDes[1],F_SETFD,1))
    {
        ASSERT("can't set close-on-exec flag; fcntl() failed. errno is %d "
             "(%s)\n", errno, strerror(errno));
        close(readWritePipeDes[0]);
        close(readWritePipeDes[1]);
        SetLastError(ERROR_INTERNAL_ERROR);
        goto EXIT;
    }            

    /* allocate the file_data structure */
    if ( ((readpipe_data = FILENewFileData()) == NULL) ||
         ((writepipe_data = FILENewFileData()) == NULL))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto CLEANUP;
    }

    if (((*hReadPipe  = HMGRGetHandle((HOBJSTRUCT*) readpipe_data)) ==
         INVALID_HANDLE_VALUE) ||
        ((*hWritePipe = HMGRGetHandle((HOBJSTRUCT*) writepipe_data)) ==
         INVALID_HANDLE_VALUE))
    {
        ERROR("Failed to get handle\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto CLEANUP;
    }

    /* fill the structures to store with the handles */
    readpipe_data->unix_fd = readWritePipeDes[0];
    readpipe_data->open_flags = O_RDONLY;
    readpipe_data->inheritable = TRUE;

    writepipe_data->unix_fd = readWritePipeDes[1];
    writepipe_data->open_flags = O_WRONLY;
    writepipe_data->inheritable = TRUE;

    ret = TRUE;
    goto EXIT;

CLEANUP:
    /* clean up the pipe structures */
    if (close(readWritePipeDes[0]) == -1)
    {
        ERROR("close(fd:%d) failed\n", close(readWritePipeDes[0]));
    }
    if (close(readWritePipeDes[1]) == -1)
    {
        ERROR("close(fd:%d) failed\n", close(readWritePipeDes[1]));
    }
    if ((*hReadPipe != INVALID_HANDLE_VALUE) && (!HMGRFreeHandle(*hReadPipe)))
    {
        ERROR("Unable to free handle\n");
    }
    if ((*hWritePipe != INVALID_HANDLE_VALUE) && (!HMGRFreeHandle(*hWritePipe)))
    {
        ERROR("Unable to free handle\n");
    }
    if ( (readpipe_data != NULL) &&
         (FILECloseHandle((HOBJSTRUCT*)readpipe_data) == -1))
    {
        ERROR("Unable to close handle\n");
    }
    if ( (writepipe_data != NULL) &&
         (FILECloseHandle((HOBJSTRUCT*)writepipe_data) == -1))
    {
        ERROR("Unable to close handle\n");
    }
EXIT:
    LOGEXIT("CreatePipe return %s\n", ret ? "TRUE":"FALSE");
    PERF_EXIT(CreatePipe);
    return ret;
}

/*++
Function:
  LockFile

See MSDN doc.
--*/
PALIMPORT
BOOL
PALAPI
LockFile(HANDLE hFile,
         DWORD dwFileOffsetLow,
         DWORD dwFileOffsetHigh,
         DWORD nNumberOfBytesToLockLow,
         DWORD nNumberOfBytesToLockHigh)
{
    UINT64 lockRgnStart;
    UINT64 nbBytesToLock;
    BOOL bRet = FALSE;
    file * pFileStruct = NULL;

    PERF_ENTRY(LockFile);
    ENTRY("LockFile(hFile:%p, offsetLow:%u, offsetHigh:%u, nbBytesLow:%u,"
           " nbBytesHigh:%u\n", hFile, dwFileOffsetLow, dwFileOffsetHigh, 
          nNumberOfBytesToLockLow, nNumberOfBytesToLockHigh);

    /* make sure we have a valid handle */
    if ((pFileStruct = FILEAcquireFileStruct( hFile )) == NULL)
    {
        ERROR("Could not extract structure from handle %p\n", hFile);
        SetLastError(ERROR_INVALID_HANDLE);
        goto EXIT;
    }

    lockRgnStart  = ((UINT64)dwFileOffsetHigh) << 32  | dwFileOffsetLow;
    nbBytesToLock = ((UINT64)nNumberOfBytesToLockHigh) << 32  | 
                             nNumberOfBytesToLockLow;
    
    bRet = FILELockFileRegion(pFileStruct, lockRgnStart, nbBytesToLock,
                              USER_LOCK_RGN);
    FILEReleaseFileStruct(hFile, pFileStruct);    

EXIT:
    LOGEXIT("LockFile returns %s\n", bRet ? "TRUE":"FALSE");
    PERF_EXIT(LockFile);
    return bRet;
}

/*++
Function:
  UnlockFile

See MSDN doc.
--*/
PALIMPORT
BOOL
PALAPI
UnlockFile(HANDLE hFile,
           DWORD dwFileOffsetLow,
           DWORD dwFileOffsetHigh,
           DWORD nNumberOfBytesToUnlockLow,
           DWORD nNumberOfBytesToUnlockHigh)
{
    UINT64 unlockRgnStart;
    UINT64 nbBytesToUnlock;
    BOOL bRet = FALSE;
    file * pFileStruct = NULL;

    PERF_ENTRY(UnlockFile);
    ENTRY("UnlockFile(hFile:%p, offsetLow:%u, offsetHigh:%u, nbBytesLow:%u,"
          "nbBytesHigh:%u\n", hFile, dwFileOffsetLow, dwFileOffsetHigh, 
          nNumberOfBytesToUnlockLow, nNumberOfBytesToUnlockHigh);

    /* make sure we have a valid handle */
    if ((pFileStruct = FILEAcquireFileStruct( hFile )) == NULL)
    {
        ERROR("Could not extract structure from handle %p\n", hFile);
        SetLastError(ERROR_INVALID_HANDLE);
        goto EXIT;
    }
    
    nbBytesToUnlock = ((UINT64)nNumberOfBytesToUnlockHigh) << 32  | 
                               nNumberOfBytesToUnlockLow;

    unlockRgnStart  = ((UINT64)dwFileOffsetHigh) << 32  | dwFileOffsetLow;

    bRet = FILEUnlockFileRegion(pFileStruct, unlockRgnStart, nbBytesToUnlock,
                                USER_LOCK_RGN);
    FILEReleaseFileStruct(hFile, pFileStruct);

EXIT:    
    LOGEXIT("UnlockFile returns %s\n", bRet ? "TRUE" : "FALSE");
    PERF_EXIT(UnlockFile);
    return bRet;
}

/*++
FILELockFileRegion

locks a file region

Parameters :
    file *fileStructPtr : file structure.
    DWORD offsetStart : starting byte offset of the region
    DWORD nbBytes : length of the region
    LOCK_TYPE lockAction : type of lock action
Return Value :
    FALSE if the indicated region is totally or partially locked explicitly 
    (by LockFile call) by other filepointer, TRUE if not.
--*/
BOOL 
FILELockFileRegion(file *pFileStruct, 
                   UINT64 lockRgnStart, 
                   UINT64 nbBytesToLock, 
                   LOCK_TYPE lockAction)
{
    BOOL bRet = FALSE;
    SHMFILELOCKRGNS *curLock, *prevLock, *insertAfter, 
                     lockRgn, fakeLock = {0,0,0,0};
    SHMFILELOCKS *fileLocks;

    SHMLock();

    /* make sure we don't have a pipe handle or std file handle */
    if (pFileStruct->unix_filename == NULL)        
    {
        ERROR("Couldn't lock a pipe or std handle\n");
        SetLastError(ERROR_ACCESS_DENIED);
        goto EXIT;
    }
    
    /* make sure we don't have an access-only handle */
    if (pFileStruct->open_flags_deviceaccessonly == TRUE)
    {
        ERROR("The file must been opened with read and/or write access\n");
        SetLastError(ERROR_ACCESS_DENIED);
        goto EXIT;
    }
    
    /* nothing to do if the region to lock is empty */
    if (nbBytesToLock == 0) 
    {
        TRACE("Locking an empty region (%I64d, %I64d)\n", lockRgnStart, nbBytesToLock);
        bRet = TRUE;
        goto EXIT;
    }
    
    
    if (pFileStruct->shmFileLocks == 0)
    {        
        /* pFileStruct->shmFileLocks is assigned in CreateFileA */
        ASSERT("pFileStruct->shmFileLocks should be valid\n");
        goto EXIT;
    }
    
    if (SHMPTR_TO_PTR_BOOL(fileLocks, pFileStruct->shmFileLocks) == FALSE)
    {
        ASSERT("Unable to get pointer from shm pointer.\n");
        goto EXIT;
    }

    if (fileLocks->fileLockedRgns != 0)
    {        
        prevLock = &fakeLock;
        
        if (SHMPTR_TO_PTR_BOOL(curLock, fileLocks->fileLockedRgns) 
            == FALSE)
        {
            ASSERT("Unable to get pointer from shm pointer.\n");
            goto EXIT;
        }
        
        lockRgn.lockRgnStart  = lockRgnStart;
        lockRgn.nbBytesLocked = nbBytesToLock;
        lockRgn.fileStructPtr = pFileStruct;
        lockRgn.processId     = GetCurrentProcessId();
        lockRgn.lockType      = lockAction;

        while((curLock != NULL) && IS_LOCK_BEFORE(curLock, &lockRgn))
        {            
            prevLock = curLock; 
            if (SHMPTR_TO_PTR_BOOL(curLock, curLock->next) == FALSE)
            {
                ASSERT("Unable to get pointer from shm pointer.\n");
                goto EXIT;
            }
        }
                
        while((curLock != NULL) && IS_LOCK_INTERSECT(curLock, &lockRgn))
        {
            /* we couldn't lock the requested region if it overlap with other 
               region locked explicitly (by LockFile call) by other file pointer */
            if ((lockAction == USER_LOCK_RGN) || 
                ((curLock->lockType  == USER_LOCK_RGN) && 
                 !IS_LOCK_HAVE_SAME_OWNER(curLock, &lockRgn)))
            {
                WARN("The requested lock region overlaps an existing locked region\n");
                SetLastError(ERROR_LOCK_VIOLATION);
                goto EXIT;
            }
            
            prevLock = curLock; 
            if (SHMPTR_TO_PTR_BOOL(curLock, curLock->next) == FALSE)
            {
                ASSERT("Unable to get pointer from shm pointer.\n");
                goto EXIT;
            }
        }
        
        /* save the previous lock in case we need to insert the requested lock */
        insertAfter = prevLock;         
                
        while(((curLock != NULL) && IS_LOCK_INTERSECT(&lockRgn, curLock)))
        {
            /* we couldn't lock the requested region if it overlap with other region 
               locked explicitly (by LockFile call) by other file pointer */
            if ((lockAction == USER_LOCK_RGN) || 
                ((curLock->lockType  == USER_LOCK_RGN) &&  
                 !IS_LOCK_HAVE_SAME_OWNER(curLock, &lockRgn)))
            {
                WARN("The requested lock region overlaps an existing locked region\n");
                SetLastError(ERROR_LOCK_VIOLATION);
                goto EXIT;
            }
            
            prevLock = curLock; 
            if (SHMPTR_TO_PTR_BOOL(curLock, curLock->next) == FALSE)
            {
                ASSERT("Unable to get pointer from shm pointer.\n");
                goto EXIT;
            }
        }
       
        if (insertAfter == &fakeLock) 
        {
            insertAfter = NULL;
        }

        if(FILEAddNewLockedRgn(fileLocks, pFileStruct, insertAfter, 
                               lockRgnStart, nbBytesToLock, lockAction) == FALSE)
        {
            WARN("Couldn't add the new locked region into SHM\n");
            goto EXIT;
        }
    }
    else /* lock region list is empty. */
    {
        if(FILEAddNewLockedRgn(fileLocks, pFileStruct, NULL, 
                               lockRgnStart, nbBytesToLock, lockAction) == FALSE)
        {
            ERROR("Couldn't add the first file locked region \n");
            goto EXIT;
        }
    }
    
    bRet = TRUE;
EXIT:
    SHMRelease();
    return bRet;
}

/*++
FILEUnlockFileRegion

Unlock a file region locked by a previous call to FILELockFileRegion

Parameters :
    file *fileStructPtr: file structure.
    DWORD offsetStart  : starting byte offset of the region
    DWORD nbBytes      : length of the region
    LOCK_TYPE unlockAction   : type of the lock to unlock.
Return Value :
    TRUE, if the indicated region is unlocked with success, FALSE otherwise
--*/
BOOL 
FILEUnlockFileRegion(file *pFileStruct, 
                     UINT64 unlockRgnStart, 
                     UINT64 nbBytesToUnlock, 
                     LOCK_TYPE unlockType)
{
    SHMFILELOCKRGNS *prevLock = NULL, *curLockRgn = NULL, unlockRgn;
    SHMPTR shmcurLockRgn;
    SHMFILELOCKS *fileLocks;

    BOOL bRet = FALSE;

    SHMLock();

    /* make sure we don't have a pipe handle or std file handle */
    if (pFileStruct->unix_filename == NULL)        
    {
        ERROR("Couldn't lock a pipe or std handle\n");
        SetLastError(ERROR_ACCESS_DENIED);
        goto EXIT;
    }
    
    /* make sure we don't have an access-only handle */
    if (pFileStruct->open_flags_deviceaccessonly == TRUE)
    {
        ERROR("The file must been opened with read and/or write access\n");
        SetLastError(ERROR_ACCESS_DENIED);
        goto EXIT;
    }
    
    /* check if the region to unlock is empty or not */
    if (nbBytesToUnlock == 0) 
    {
        bRet = FALSE;
        SetLastError(ERROR_NOT_LOCKED);
        WARN("Attempt to unlock an empty region\n");
        goto EXIT;
    }
    
    if (pFileStruct->shmFileLocks == 0)
    {
        /* pFileStruct->shmFileLocks should have been set in CreateFileA */     
        ASSERT("pFileStruct->shmFileLocks should be non NULL\n");
        SetLastError(ERROR_NOT_LOCKED);
        goto EXIT;
    }

    if ((SHMPTR_TO_PTR_BOOL(fileLocks, pFileStruct->shmFileLocks) == FALSE) ||
        (SHMPTR_TO_PTR_BOOL(curLockRgn, fileLocks->fileLockedRgns) == FALSE))
    {
        ASSERT("Unable to get pointer from shm pointer.\n");
        goto EXIT;
    }
    
    unlockRgn.processId = GetCurrentProcessId();
    unlockRgn.fileStructPtr = pFileStruct;
    unlockRgn.lockRgnStart = unlockRgnStart;
    unlockRgn.nbBytesLocked = nbBytesToUnlock;
    unlockRgn.lockType = unlockType;

    shmcurLockRgn = fileLocks->fileLockedRgns;
    
    while((curLockRgn != NULL) && !IS_LOCK_EQUAL(curLockRgn, &unlockRgn))
    {
        prevLock = curLockRgn; 
        shmcurLockRgn = curLockRgn->next;
        if (SHMPTR_TO_PTR_BOOL(curLockRgn, shmcurLockRgn) == FALSE)
        {
            ASSERT("Unable to get pointer from shm pointer.\n");
            goto EXIT;
        }
    }
    
    if (curLockRgn != NULL) 
    {
        TRACE("removing the lock region (%I64u, %I64u)\n", 
               curLockRgn->lockRgnStart, curLockRgn->nbBytesLocked);

        if (prevLock == NULL) 
        {
            /* removing the first lock */
            fileLocks->fileLockedRgns = curLockRgn->next;
        }
        else
        {
            prevLock->next = curLockRgn->next;
        }
        SHMfree(shmcurLockRgn);
        bRet = TRUE;
    }
    else
    {
        /* the lock doesn't exist */
        WARN("Attempt to unlock a non locked region\n");
        SetLastError(ERROR_NOT_LOCKED);
        goto EXIT;
    }
    
EXIT:    
    SHMRelease();
    return bRet;
}


/*++
FILEGetSHMFileLocks

Find a SHMFILELOCKS structure in shared memory

Parameters :
    char *filename : file name of the SHMFILELOCKS structure to look for.

Return Value :
    Pointer to SHMFILELOCKS structure of file in shared memory
    (NULL if not found)
--*/
SHMPTR
FILEGetSHMFileLocks(char *filename)
{
    SHMPTR shmPtrRet = 0;
    SHMFILELOCKS *filelocksPtr, *nextFilelocksPtr;
    char *unix_filename;

    SHMLock();

    shmPtrRet = SHMGetInfo(SIID_FILE_LOCKS);

    while(shmPtrRet != 0)
    {        
        if ( (SHMPTR_TO_PTR_BOOL(filelocksPtr, shmPtrRet) == FALSE) ||
             (SHMPTR_TO_PTR_BOOL(unix_filename, filelocksPtr->unix_filename) == FALSE))
        {
            ASSERT("Unable to get pointer from shm pointer.\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            goto EXIT;
        }

        if (unix_filename == NULL)
        {
            ERROR("Unexpected lock file name value.\n");
            SetLastError(ERROR_INTERNAL_ERROR);
            goto EXIT;
        }
        
#if HAVE_CASE_SENSITIVE_FILESYSTEM
        if (strcmp(unix_filename, filename) == 0)
#else   // HAVE_CASE_SENSITIVE_FILESYSTEM
        if (strcasecmp(unix_filename, filename) == 0)
#endif  // HAVE_CASE_SENSITIVE_FILESYSTEM
        {
            filelocksPtr->refCount++;
            goto EXIT;
        }
        
        shmPtrRet = filelocksPtr->next;
    }

    /* the file has never been locked before.*/
    TRACE("Create a new entry in the file lock list in SMH\n");

    /* Create a new entry in the file lock list in SMH */
    if ((shmPtrRet = SHMalloc(sizeof(SHMFILELOCKS))) == 0)
    {
        ERROR("Can't allocate SHMFILELOCKS structure\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto EXIT;
    }

    if (SHMPTR_TO_PTR_BOOL(filelocksPtr, shmPtrRet) == FALSE)
    {
        ASSERT("Unable to get pointer from shm pointer.\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        goto CLEANUP1;
    }

    filelocksPtr->unix_filename = SHMStrDup(filename);
    if (filelocksPtr->unix_filename == 0)
    {
        ERROR("Can't allocate shared memory for filename\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto CLEANUP1;
    }

    filelocksPtr->fileLockedRgns = 0;
    filelocksPtr->prev = 0;
    filelocksPtr->next = SHMGetInfo(SIID_FILE_LOCKS);
    filelocksPtr->refCount = 1;
    filelocksPtr->share_mode = SHARE_MODE_NOT_INITALIZED;
    filelocksPtr->nbReadAccess = 0;
    filelocksPtr->nbWriteAccess = 0;

    if (SHMPTR_TO_PTR_BOOL(nextFilelocksPtr, filelocksPtr->next) == FALSE)
    {
        ASSERT("Unable to get pointer from shm pointer.\n");
        SetLastError(ERROR_INTERNAL_ERROR);
        goto CLEANUP2;
    }
    
    if (nextFilelocksPtr != NULL)
    {
        nextFilelocksPtr->prev = shmPtrRet;
    }

    SHMSetInfo(SIID_FILE_LOCKS, shmPtrRet);
    goto EXIT;

CLEANUP2:
    SHMfree(filelocksPtr->unix_filename);
CLEANUP1:
    SHMfree(shmPtrRet);
    shmPtrRet = 0;
EXIT:    
    SHMRelease();
    return shmPtrRet;
}

/*++
FILEAddNewLockedRgn

Add a new lock region to shared memory

Parameters :
    file *fileStructPtr : file data structure with a region to be locked.
    SHMFILELOCKRGNS *insertAfter: the lock region ptr to precede the new lock region
    UINT64 lockRgnStart: starting byte offset in the file where the lock should begin.
    UINT64 nbBytesToLock: length of the byte range to be locked.
    int lockType : type of file lock to add 
Return Value :
    TRUE when it succeeds, FALSE if not.
--*/
BOOL 
FILEAddNewLockedRgn(SHMFILELOCKS* fileLocks,
                    file *fileStructPtr,
                    SHMFILELOCKRGNS *insertAfter,
                    UINT64 lockRgnStart, 
                    UINT64 nbBytesToLock,
                    LOCK_TYPE lockType)
{
    SHMFILELOCKRGNS *newLockRgn, *lockRgnPtr;
    SHMPTR shmNewLockRgn;
    BOOL bRet = FALSE;

    if ((fileLocks == NULL) || (fileStructPtr == NULL))
    {
        ASSERT("Invalid Null parameter.\n");
        return FALSE;
    }

    SHMLock();
    
    /* Create a new entry for the new locked region */
    TRACE("Create a new entry for the new lock region (%I64u %I64u)\n", 
          lockRgnStart, nbBytesToLock);
    
    if ((shmNewLockRgn = SHMalloc(sizeof(SHMFILELOCKRGNS))) == 0)
    {
        ERROR("Can't allocate SHMFILELOCKRGNS structure\n");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto EXIT;
    }

    if (SHMPTR_TO_PTR_BOOL(newLockRgn, shmNewLockRgn) == FALSE)
    {
        ASSERT("Unable to get pointer from shm pointer.\n");
        goto CLEANUP;
    }
    
    newLockRgn->processId = GetCurrentProcessId();
    newLockRgn->fileStructPtr = fileStructPtr;
    newLockRgn->lockRgnStart = lockRgnStart;
    newLockRgn->nbBytesLocked = nbBytesToLock;
    newLockRgn->lockType = lockType;
    
    /* All locked regions with the same offset should be sorted ascending */
    /* the sort is based on the length of the locked byte range */
    if (insertAfter != NULL)
    {
        if (SHMPTR_TO_PTR_BOOL(lockRgnPtr, insertAfter->next) == FALSE)
        {
            ASSERT("Unable to get pointer from shm pointer.\n");
            goto CLEANUP;
        }
    }
    else
    {
        if (SHMPTR_TO_PTR_BOOL(lockRgnPtr, fileLocks->fileLockedRgns) == FALSE)
        {
            ASSERT("Unable to get pointer from shm pointer.\n");
            goto CLEANUP;
        }
    }
    
    while(lockRgnPtr != NULL)
    {
        if ( (lockRgnPtr->lockRgnStart == newLockRgn->lockRgnStart) &&
             (newLockRgn->nbBytesLocked > lockRgnPtr->nbBytesLocked))
        {
            insertAfter = lockRgnPtr;
            if (SHMPTR_TO_PTR_BOOL(lockRgnPtr, lockRgnPtr->next) == FALSE)
            {
                ASSERT("Unable to get pointer from shm pointer.\n");
                goto CLEANUP;
            }
            continue;
        }

        break;
    }
    
    if (insertAfter != NULL)
    {       
        TRACE("Adding lock after the lock rgn (%I64d %I64d)\n", 
              insertAfter->lockRgnStart,insertAfter->nbBytesLocked);
        newLockRgn->next = insertAfter->next;
        insertAfter->next = shmNewLockRgn;
    }
    else
    {
        TRACE("adding lock into the head of the list\n");
        newLockRgn->next = fileLocks->fileLockedRgns;
        fileLocks->fileLockedRgns = shmNewLockRgn;
    }
    bRet = TRUE;
    goto EXIT;

CLEANUP:
    SHMfree(shmNewLockRgn);
EXIT:    
    SHMRelease();
    return bRet;
}

/*++
FILECleanUpLockedRgn

Remove all regions, locked by a file pointer, from shared memory

Parameters :
    file *fileStructPtr : file data structure with a regions to be removed.

--*/
void 
FILECleanUpLockedRgn(file *fileStructPtr)
{
    SHMFILELOCKRGNS *curLockRgn = NULL, *prevLock = NULL;
    SHMFILELOCKS *fileLocks, *prevFileLocks, *nextFileLocks;
    SHMPTR shmcurLockRgn;    

    if (fileStructPtr == NULL) 
    {
        ASSERT("Invalid Null parameter.\n");
        return;
    }

    SHMLock();

    if (SHMPTR_TO_PTR_BOOL(fileLocks, fileStructPtr->shmFileLocks) == FALSE)
    {
        ASSERT("Unable to get pointer from shm pointer.\n");
        goto EXIT;
    }

    if (fileLocks != NULL)
    {
        if(fileLocks->fileLockedRgns !=0)
        {        
            shmcurLockRgn = fileLocks->fileLockedRgns;
            if (SHMPTR_TO_PTR_BOOL(curLockRgn, shmcurLockRgn) == FALSE)
            {
                ASSERT("Unable to get pointer from shm pointer.\n");
                goto EXIT;
            }
            
            while(curLockRgn != NULL)
            {
                if ((curLockRgn->fileStructPtr == fileStructPtr) && 
                    (curLockRgn->processId == GetCurrentProcessId()))
                {
                    /* found the locked rgn to remove from SHM */
                    TRACE("Removing the locked region (%I64u, %I64u) from SMH\n", 
                          curLockRgn->lockRgnStart, curLockRgn->nbBytesLocked);
                    
                    if (prevLock == NULL) 
                    {
                        /* removing the first lock */
                        fileLocks->fileLockedRgns = curLockRgn->next;
                        SHMfree(shmcurLockRgn);
                        shmcurLockRgn = fileLocks->fileLockedRgns;
                        if (SHMPTR_TO_PTR_BOOL(curLockRgn, shmcurLockRgn) == FALSE)
                        {
                            ASSERT("Unable to get pointer from shm pointer.\n");
                            goto EXIT;
                        }
                    }
                    else
                    {
                        prevLock->next = curLockRgn->next;
                        SHMfree(shmcurLockRgn);
                        shmcurLockRgn = prevLock->next;
                        if (SHMPTR_TO_PTR_BOOL(curLockRgn, shmcurLockRgn) == FALSE)
                        {
                            ASSERT("Unable to get pointer from shm pointer.\n");
                            goto EXIT;
                        }
                    }
                    continue;
                }
                
                prevLock = curLockRgn;
                shmcurLockRgn = curLockRgn->next;
                if (SHMPTR_TO_PTR_BOOL(curLockRgn, shmcurLockRgn) == FALSE)
                {
                    ASSERT("Unable to get pointer from shm pointer.\n");
                    goto EXIT;
                }
            }
        }
    
        if( fileStructPtr->dwDesiredAccess & GENERIC_READ )
        {   
            fileLocks->nbReadAccess--;      
        }
        if( fileStructPtr->dwDesiredAccess & GENERIC_WRITE )
        {
            fileLocks->nbWriteAccess--; 
        }  

        /* remove the SHMFILELOCKS structure from SHM if there's no more locked 
           region left and no more reference to it */        
        if ((--(fileLocks->refCount) == 0) && (fileLocks->fileLockedRgns == 0))
        {            
            TRACE("Removing the SHMFILELOCKS structure from SHM\n");

            if ( (SHMPTR_TO_PTR_BOOL(prevFileLocks, fileLocks->prev) == FALSE) ||
                 (SHMPTR_TO_PTR_BOOL(nextFileLocks, fileLocks->next) == FALSE))
            {
                ASSERT("Unable to get pointer from shm pointer.\n");
                goto EXIT;
            }

            if (prevFileLocks == NULL) 
            {
                /* removing the first lock file*/
                SHMSetInfo(SIID_FILE_LOCKS, fileLocks->next);
            }
            else
            {
                prevFileLocks->next = fileLocks->next;
            }

            if (nextFileLocks != NULL)
            {
                nextFileLocks->prev = fileLocks->prev;
            }

            if (fileLocks->unix_filename)
                SHMfree(fileLocks->unix_filename);

            SHMfree(fileStructPtr->shmFileLocks);
        }
    }    
EXIT:
    SHMRelease();
    return;
}


/*++
init_std_handle [static]

utility function for FILEInitStdHandles. do the work that is common to all 
three standard handles

Parameters:
    HANDLE pStd : Defines which standard handle to assign
    FILE *stream        : file stream to associate to handle

Return value:
    handle for specified stream, or INVALID_HANDLE_VALUE on failure
--*/
static HANDLE init_std_handle(HANDLE * pStd, FILE *stream)
{
    file *file_data;
    HANDLE hFile;
    int new_fd;

    /* duplicate the FILE *, so that we can fclose() in FILECloseHandle without
       closing the original */
    new_fd = dup(fileno(stream));
    if(-1 == new_fd)
    {
        ERROR("dup() failed; errno is %d (%s)\n", errno, strerror(errno));
        return INVALID_HANDLE_VALUE;
    }

    file_data = FILENewFileData();
    if (file_data == NULL)
    {
        ERROR("couldn't create file object structure!\n");
        return INVALID_HANDLE_VALUE;
    }

    file_data->unix_fd = new_fd;
    file_data->open_flags = 0;
    file_data->unix_filename = NULL;
    file_data->inheritable = TRUE;
    
    /* Override the standard CloseHandle implementation, to
       avoid a pthread library bug in FreeBSD. Please see
       comment inside of FILECloseStdHandle() */
    file_data->handle_data.close_handle = FILECloseStdHandle; 

    hFile = HMGRGetHandle( (HOBJSTRUCT*)file_data );
    if (hFile == INVALID_HANDLE_VALUE)
    {
        ERROR("Unable to allocate a handle\n");
        FILECloseHandle( (HOBJSTRUCT*)file_data);
        return INVALID_HANDLE_VALUE;
    }
    
    *pStd = hFile;

    return hFile;
}


/*++
FILEInitStdHandles

Create handle objects for stdin, stdout and stderr

(no parameters)

Return value:
    TRUE on success, FALSE on failure
--*/
BOOL FILEInitStdHandles(void)
{
    HANDLE stdin_handle;
    HANDLE stdout_handle;
    HANDLE stderr_handle;

    TRACE("creating handle objects for stdin, stdout, stderr\n");

    stdin_handle = init_std_handle(&pStdIn, stdin);
    if(INVALID_HANDLE_VALUE == stdin_handle)
    {
        ERROR("failed to create stdin handle\n");
        goto fail;
    }

    stdout_handle = init_std_handle(&pStdOut, stdout);
    if(INVALID_HANDLE_VALUE == stdout_handle)
    {
        ERROR("failed to create stdout handle\n");
        CloseHandle(stdin_handle);
        goto fail;
    }

    stderr_handle = init_std_handle(&pStdErr, stderr);
    if(INVALID_HANDLE_VALUE == stderr_handle)
    {
        ERROR("failed to create stderr handle\n");
        CloseHandle(stdin_handle);
        CloseHandle(stdout_handle);
        goto fail;
    }
    return TRUE;

fail:
    pStdIn = INVALID_HANDLE_VALUE;
    pStdOut = INVALID_HANDLE_VALUE;
    pStdErr = INVALID_HANDLE_VALUE;
    return FALSE;
}


/*++
FILECleanupStdHandles

Remove all regions, locked by a file pointer, from shared memory

Parameters :
    file *fileStructPtr : file data structure with a regions to be removed.

--*/
void FILECleanupStdHandles(void)
{
    HANDLE stdin_handle;
    HANDLE stdout_handle;
    HANDLE stderr_handle;

    TRACE("closing standard handles\n");
    stdin_handle = pStdIn;
    stdout_handle = pStdOut;
    stderr_handle = pStdErr;
    pStdIn = INVALID_HANDLE_VALUE;
    pStdOut = INVALID_HANDLE_VALUE;
    pStdErr = INVALID_HANDLE_VALUE;
    CloseHandle(stdin_handle);
    CloseHandle(stdout_handle);
    CloseHandle(stderr_handle);
}


/*++
FILEGetFileNameFromSymLink

Input paramters:

source  = path to the file on input, path to the file with all 
          symbolic links traversed on return

Note: Assumes the maximum size of the source is MAX_PATH

Return value:
    TRUE on success, FALSE on failure
--*/
BOOL FILEGetFileNameFromSymLink(char *source)
{
    int ret;
    char sLinkData[MAX_PATH];

    do
    {
        ret = readlink(source, sLinkData, MAX_PATH);
        if (ret>0)
        {
            sLinkData[ret] = '\0';
            strcpy(source, sLinkData);
        }
    } while (ret > 0);

    return (errno == EINVAL);
}
