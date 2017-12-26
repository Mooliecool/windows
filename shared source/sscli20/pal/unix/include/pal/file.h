/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    include/pal/file.h

Abstract:
    Header file for file utility functions.

Revision History:

--*/

#ifndef _PAL_FILE_H_
#define _PAL_FILE_H_

#include "pal/handle.h"
#include "pal/shmemory.h"
#include <sys/types.h>
#include <dirent.h>
#include <glob.h>

#define SHARE_MODE_NOT_INITALIZED 0xFFFFFFFF

typedef struct
{
    SHMPTR unix_filename;
    SHMPTR fileLockedRgns;    
    UINT refCount;
    SHMPTR next;
    SHMPTR prev;
    DWORD  share_mode; /* FILE_SHARE_READ, FILE_SHARE_WRITE, 
                          FILE_SHARE_DELETE,  0 ( not shared ) or
                          SHARE_MODE_NOT_INITALIZED */
    int nbReadAccess;  /* used to keep track of the minimal
                          access permissions */
    int nbWriteAccess;
} SHMFILELOCKS;

typedef struct _file
{
    HOBJSTRUCT  handle_data;

    struct _file *self_addr;

    int  unix_fd;
    DWORD dwDesiredAccess; /* Unix assumes files are always opened for reading.
                              In Windows we can open a file for writing only */                             
    int  open_flags;       /* stores Unix file creation flags */
    BOOL open_flags_deviceaccessonly;
    char *unix_filename;
    SHMPTR shmFileLocks;
    BOOL inheritable;
} file;

typedef struct _find_handle
{
    struct _find_handle *self_addr; /* for pointer verification */

    char   dir[_MAX_DIR];
    char   fname[_MAX_PATH]; /* includes extension */
    glob_t glob;
    char   **next;
} find_obj;

typedef struct
{
    DWORD  processId;
    file*  fileStructPtr;
    UINT64 lockRgnStart;
    UINT64 nbBytesLocked;
    int    lockType;

    SHMPTR next;
} SHMFILELOCKRGNS;

typedef enum {
    USER_LOCK_RGN, /* Used only for user locks (LockFile or UnlockFile call) */
    RDWR_LOCK_RGN  /* Used to distinguish between the user locks and the internal 
                      locks made when  reading, writing or truncating file */
} LOCK_TYPE ;

/* return TRUE if LockToTest region is behind lockRgn, FALSE otherwise */
#define IS_LOCK_BEFORE(LockToTest, lockRgn) \
    (((LockToTest)->lockRgnStart + (LockToTest)->nbBytesLocked) <= \
                                                        (lockRgn)->lockRgnStart)

/* return TRUE if LockToTest region intersect with lockRgn, FALSE otherwise */
#define IS_LOCK_INTERSECT(LockToTest, lockRgn) \
    (!IS_LOCK_BEFORE(LockToTest, lockRgn) && !IS_LOCK_BEFORE(lockRgn, LockToTest))

/* return TRUE if LockToTest region and lockRgn have the same file pointer and 
   the same process Id, FALSE otherwise */
#define IS_LOCK_HAVE_SAME_OWNER(LockToTest, lockRgn) \
    (((LockToTest)->fileStructPtr == (lockRgn)->fileStructPtr) && \
     ((LockToTest)->processId == (lockRgn)->processId))

/* return TRUE if LockToTest region and lockRgn represent the same lock, 
   FALSE otherwise*/
#define IS_LOCK_EQUAL(LockToTest, lockRgn) \
        (((LockToTest)->processId == (lockRgn)->processId)           && \
         ((LockToTest)->fileStructPtr == (lockRgn)->fileStructPtr)   && \
         ((LockToTest)->lockRgnStart == (lockRgn)->lockRgnStart)     && \
         ((LockToTest)->nbBytesLocked == (lockRgn)->nbBytesLocked)   && \
         ((LockToTest)->lockType == (lockRgn)->lockType))


/*++
Function:
  FILEAcquireFileStruct

Return and lock the file structure associated with a given handle if it is 
valid. If it is not valid, return NULL.
--*/
file *FILEAcquireFileStruct( HANDLE handle );

/*++
Function:
  FILEReleaseFileStruct

Release control of a file structure, to be called sometime after a call
to FILEAcquireFileStruct.
--*/
void FILEReleaseFileStruct( HANDLE handle, file *file_data );

/*++
FILECanonicalizeRealPath
    Wraps realpath() to hide platform differences. See the man page for
    realpath(3) for details of how realpath() works.
    
    On systems on which realpath() allows the last path component to not
    exist, this is a straight thunk through to realpath(). On other
    systems, we remove the last path component, then call realpath().
--*/
LPSTR FILECanonicalizeRealPath(LPCSTR lpUnixPath, LPSTR lpBuffer);

/*++
FILECanonicalizePath
    Removes all instances of '/./', '/../' and '//' from an absolute path. 
    
Parameters:
    LPSTR lpUnixPath : absolute path to modify, in Unix format

(no return value)                                             
 
Notes :
-behavior is undefined if path is not absolute
-the order of steps *is* important: /one/./../two would give /one/two 
 instead of /two if step 3 was done before step 2
-reason for this function is that GetFullPathName can't use realpath(), since 
 realpath() requires the given path to be valid and GetFullPathName does not.
--*/
void FILECanonicalizePath(LPSTR lpUnixPath);

/*++
Function:
  FileDosToUnixPathA

Abstract:
  Change a DOS path to a Unix path. Replace '\' by '/'.

Parameter:
  IN/OUT lpPath: path to be modified
--*/
void 
FILEDosToUnixPathA(LPSTR lpPath);

/*++
Function:
  FileDosToUnixPathW

Abstract:
  Change a DOS path to a Unix path. Replace '\' by '/'.

Parameter:
  IN/OUT lpPath: path to be modified
  --*/
void
FILEDosToUnixPathW(LPWSTR lpPath);

/*++
Function:
  FileUnixToDosPathA

Abstract:
  Change a Unix path to a DOS path. Replace '/' by '\'.

Parameter:
  IN/OUT lpPath: path to be modified
--*/
void 
FILEUnixToDosPathA(LPSTR lpPath);


/*++
Function:
  FILEGetDirectoryFromFullPathA

Parse the given path. If it contains a directory part and a file part,
put the directory part into the supplied buffer, and return the number of
characters written to the buffer. If the buffer is not large enough,
return the required size of the buffer including the NULL character. If
there is no directory part in the path, return 0.
--*/
DWORD FILEGetDirectoryFromFullPathA( LPCSTR lpFullPath,
                     DWORD  nBufferLength,
                     LPSTR  lpBuffer );

/*++
Function:
  FILEGetFileNameFromFullPath

Given a full path, return a pointer to the first char of the filename part.
--*/
LPCSTR FILEGetFileNameFromFullPathA( LPCSTR lpFullPath );

/*++
Function:
  FILEGetLastErrorFromErrno
  
Convert errno into the appropriate win32 error and return it.
--*/
DWORD FILEGetLastErrorFromErrno( void );

/*++
FILEInitStdHandles

Create handle objects for stdin, stdout and stderr

(no parameters)

Return value:
    TRUE on success, FALSE on failure
--*/
BOOL FILEInitStdHandles(void);

/*++
FILECleanupStdHandles

Close promary handles for stdin, stdout and stderr

(no parameters, no return value)
--*/
void FILECleanupStdHandles(void);

/*++
FILEGetFileNameFromSymLink

Input paramters:

source  = path to the file on input, path to the file with all 
          symbolic links traversed on return

Note: Assumes the maximum size of the source is MAX_PATH

Return value:
    TRUE on success, FALSE on failure
--*/
BOOL FILEGetFileNameFromSymLink(char *source);

/*++

Function : 
    FILEGetProperNotFoundError
    
Returns the proper error code, based on the 
Windows behavoir.

    IN LPSTR lpPath - The path to check.
    LPDWORD lpErrorCode - The error to set.
*/
void FILEGetProperNotFoundError( LPSTR lpPath, LPDWORD lpErrorCode );

#endif /* _PAL_FILE_H_ */




