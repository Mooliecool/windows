/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    path.c

Abstract:

    Implementation of all functions related to path support

Revision History:

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/file.h"
#include <errno.h>

#include <unistd.h>
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif  // HAVE_ALLOCA_H

SET_DEFAULT_DEBUG_CHANNEL(FILE);



/*++
Function:
  GetFullPathNameA

See MSDN doc.
--*/
DWORD
PALAPI
GetFullPathNameA(
		 IN LPCSTR lpFileName,
		 IN DWORD nBufferLength,
		 OUT LPSTR lpBuffer,
		 OUT LPSTR *lpFilePart)
{
    DWORD  nReqPathLen, nRet = 0;
    LPSTR lpUnixPath = NULL;
    BOOL fullPath = FALSE;

    PERF_ENTRY(GetFullPathNameA);
    ENTRY("GetFullPathNameA(lpFileName=%p (%s), nBufferLength=%u, lpBuffer=%p, "
          "lpFilePart=%p)\n",
          lpFileName?lpFileName:"NULL",
          lpFileName?lpFileName:"NULL", nBufferLength, lpBuffer, lpFilePart);

    if(NULL == lpFileName)
    {
        WARN("lpFileName is NULL\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    /* find out if lpFileName is a partial or full path */
    if ('\\' == *lpFileName || '/' == *lpFileName)
    {
        fullPath = TRUE;
    }

    if(fullPath)
    {
        lpUnixPath = strdup( lpFileName );
        if(NULL == lpUnixPath)
        {
            ERROR("strdup() failed; error is %d (%s)\n",
                  errno, strerror(errno));
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto done;
        }
    }   
    else
    {
        size_t max_len;

        /* allocate memory for full non-canonical path */
        max_len = strlen(lpFileName)+1; /* 1 for the slash to append */
        max_len += MAX_PATH + 1; 
        lpUnixPath = malloc(max_len);
        if(NULL == lpUnixPath)
        {
            ERROR("malloc() failed; error is %d (%s)\n",
                  errno, strerror(errno));
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto done;
        }
        
        /* build full path */
        if(!GetCurrentDirectoryA(MAX_PATH + 1, lpUnixPath))
        {
            /* no reason for this to fail now... */
            ASSERT("GetCurrentDirectoryA() failed! lasterror is %#xd\n",
                   GetLastError());
            SetLastError(ERROR_INTERNAL_ERROR);
            goto done;
        }
        strcat(lpUnixPath,"/");
        strcat(lpUnixPath, lpFileName);
    }

    /* do conversion to Unix path */
    FILEDosToUnixPathA( lpUnixPath );
 
    /* now we can canonicalize this */
    FILECanonicalizePath(lpUnixPath);

    /* at last, we can figure out how long this path is */
    nReqPathLen = strlen(lpUnixPath)+1;

    if(nBufferLength < nReqPathLen)
    {
        TRACE("reporting insufficient buffer : minimum is %d, caller "
              "provided %d\n", nReqPathLen, nBufferLength);
        nRet = nReqPathLen;
        goto done;
    }

    nRet = nReqPathLen-1;
    strcpy(lpBuffer, lpUnixPath);

    /* locate the filename component if caller cares */
    if(lpFilePart)
    {
        *lpFilePart = strrchr(lpBuffer, '/');

        if (*lpFilePart == NULL)
        {
            ASSERT("Not able to find '/' in the full path.\n");
            SetLastError( ERROR_INTERNAL_ERROR );
            nRet = 0;
            goto done;
        }
        else 
        {
            (*lpFilePart)++; 
        }
    }

done:
    free (lpUnixPath);
    LOGEXIT("GetFullPathNameA returns DWORD %u\n", nRet);
    PERF_EXIT(GetFullPathNameA);
    return nRet;
}


/*++
Function:
  GetFullPathNameW

See MSDN doc.
--*/
DWORD
PALAPI
GetFullPathNameW(
		 IN LPCWSTR lpFileName,
		 IN DWORD nBufferLength,
		 OUT LPWSTR lpBuffer,
		 OUT LPWSTR *lpFilePart)
{
    LPSTR fileNameA;
    /* bufferA needs to be able to hold a path that's potentially as
       large as MAX_PATH WCHARs. */
    CHAR  bufferA[MAX_PATH * sizeof(WCHAR)];
    LPSTR lpFilePartA;
    int   fileNameLength;
    int   srcSize;
    int   length;
    DWORD nRet = 0;

    PERF_ENTRY(GetFullPathNameW);
    ENTRY("GetFullPathNameW(lpFileName=%p (%S), nBufferLength=%u, lpBuffer=%p"
          ", lpFilePart=%p)\n",
          lpFileName?lpFileName:W16_NULLSTRING,
          lpFileName?lpFileName:W16_NULLSTRING, nBufferLength, 
          lpBuffer, lpFilePart);

    /* Find the number of bytes required to convert lpFileName
       to ANSI. This may be more than MAX_PATH. We try to
       handle that case, since it may be less than MAX_PATH
       WCHARs. */
    fileNameLength = WideCharToMultiByte(CP_ACP, 0, lpFileName,
                                         -1, NULL, 0, NULL, NULL);
    if (fileNameLength == 0)
    {
        /* Couldn't convert to ANSI. That's odd. */
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }
    else
    {
        fileNameA = alloca(fileNameLength);
    }
    
    /* Now convert lpFileName to ANSI. */
    srcSize = WideCharToMultiByte (CP_ACP, 0, lpFileName, 
                                   -1, fileNameA, fileNameLength,
                                   NULL, NULL );
    if( srcSize == 0 )
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
        goto done;
    }
    
    length = GetFullPathNameA(fileNameA, sizeof(bufferA), bufferA, &lpFilePartA);
    
    if (length == 0 || length > sizeof(bufferA))
    {
        /* Last error is set by GetFullPathNameA */
        nRet = length;
        goto done;
    }
    
    /* Convert back to Unicode the result */
    nRet = MultiByteToWideChar( CP_ACP, 0, bufferA, -1,
                                lpBuffer, nBufferLength );

    if (nRet == 0)
    {
        if ( GetLastError() == ERROR_INSUFFICIENT_BUFFER )
        {
            /* get the required length */
            nRet = MultiByteToWideChar( CP_ACP, 0, bufferA, -1,
                                        NULL, 0 );
            SetLastError(ERROR_BUFFER_OVERFLOW);
        }

        goto done;
    }
    
    /* MultiByteToWideChar counts the trailing NULL, but
       GetFullPathName does not. */
    nRet--;

    /* now set lpFilePart */
    if (lpFilePart != NULL)
    {
        *lpFilePart = lpBuffer;
        *lpFilePart += MultiByteToWideChar( CP_ACP, 0, bufferA,
                                            lpFilePartA - bufferA, NULL, 0);
    }

done:
    LOGEXIT("GetFullPathNameW returns DWORD %u\n", nRet);
    PERF_EXIT(GetFullPathNameW);
    return nRet;
}


/*++
Function:
  GetLongPathNameW

See MSDN doc.

Note:
  Since short path names are not implemented (nor supported) in the PAL, 
  this function simply copies the given path into the new buffer.

--*/
DWORD
PALAPI
GetLongPathNameW(
		 IN LPCWSTR lpszShortPath,
         OUT LPWSTR lpszLongPath,
  		 IN DWORD cchBuffer)
{
    DWORD dwPathLen = 0;

    PERF_ENTRY(GetLongPathNameW);
    ENTRY("GetLongPathNameW(lpszShortPath=%p (%S), lpszLongPath=%p (%S), "
          "cchBuffer=%d\n", lpszShortPath, lpszShortPath, lpszLongPath, lpszLongPath, cchBuffer);

    if ( !lpszShortPath )
    {
        ERROR( "lpszShortPath was not a valid pointer.\n" )
        SetLastError( ERROR_INVALID_PARAMETER );
        LOGEXIT("GetLongPathNameW returns DWORD 0\n");
        PERF_EXIT(GetLongPathNameW);
        return 0;
    }
    else if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW( lpszShortPath ))
    {
        // last error has been set by GetFileAttributes
        ERROR( "lpszShortPath does not exist.\n" )
        LOGEXIT("GetLongPathNameW returns DWORD 0\n");
        PERF_EXIT(GetLongPathNameW);
        return 0;
    }

    /* all lengths are # of TCHAR characters */
    /* "required size" includes space for the terminating null character */
    dwPathLen = PAL_wcslen(lpszShortPath)+1;

    /* lpszLongPath == 0 means caller is asking only for size */
    if ( lpszLongPath )
    {
        if ( dwPathLen > cchBuffer )
        {
            ERROR("Buffer is too small, need %d characters\n", dwPathLen);
            SetLastError( ERROR_INSUFFICIENT_BUFFER );
        } else 
        {
            if ( lpszShortPath != lpszLongPath )
            {
                // Note: MSDN doesn’t specify the behavior of GetLongPathName API
                //       if the buffers are overlap.
                PAL_wcsncpy( lpszLongPath, lpszShortPath, cchBuffer );
            }

            /* actual size not including terminating null is returned */
            dwPathLen--;
        }
    }

    LOGEXIT("GetLongPathNameW returns DWORD %u\n", dwPathLen);
    PERF_EXIT(GetLongPathNameW);
    return dwPathLen;
}


/*++
Function:
  GetTempPathA

See MSDN.

Notes:
    On Windows NT/2000, the temp path is determined by the following steps:
    1. The value of the "TMP" environment variable, or if it doesn't exist,
    2. The value of the "TEMP" environment variable, or if it doesn't exist,
    3. The Windows directory.
    
    In this implementation, '/tmp' is used instead of the Windows directory
    in the final step.
    
    Also note that dwPathLen will always be the proper return value, since
    GetEnvironmentVariable and MultiByteToWideChar will both return the
    required size of the buffer if they fail, which is what this function
    should do also.
--*/
DWORD
PALAPI
GetTempPathA(
	     IN DWORD nBufferLength,
	     OUT LPSTR lpBuffer)
{
    DWORD dwPathLen = 0;
    const char *env_var1 = "TMP";
    const char *env_var2 = "TEMP";
    // GetTempPath is supposed to include the trailing slash.
    const char *defaultDir = "/tmp/";

    PERF_ENTRY(GetTempPathA);
    ENTRY("GetTempPathA(nBufferLength=%u, lpBuffer=%p)\n",
          nBufferLength, lpBuffer);

    if ( !lpBuffer )
    {
        ERROR( "lpBuffer was not a valid pointer.\n" )
        SetLastError( ERROR_INVALID_PARAMETER );
        LOGEXIT("GetTempPathA returns DWORD %u\n", dwPathLen);
        PERF_EXIT(GetTempPathA);
        return 0;
    }
    
    /* try "TMP" */
    dwPathLen = GetEnvironmentVariableA(env_var1, lpBuffer, nBufferLength);
    if ( dwPathLen > 0 )
    {
        goto done;
    }

    /* no luck, try "TEMP" */
    dwPathLen = GetEnvironmentVariableA(env_var2, lpBuffer, nBufferLength);
    if ( dwPathLen > 0 )
    {
        goto done;
    }
    
    /* still no luck, use /tmp */
    if ( strlen(defaultDir) < nBufferLength )
    {
        strcpy(lpBuffer, defaultDir);
        dwPathLen = strlen(defaultDir);
    }
    else
    {
        /* get the required length */
        dwPathLen = strlen(defaultDir) + 1;
    }

done:
    if ( dwPathLen > nBufferLength )
    {
        ERROR("Buffer is too small, need %d characters\n", dwPathLen);
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
    }

    LOGEXIT("GetTempPathA returns DWORD %u\n", dwPathLen);
    PERF_EXIT(GetTempPathA);
    return dwPathLen;
}

/*++
Function:
  GetTempPathW

See MSDN.
See also the comment for GetTempPathA.
--*/
DWORD
PALAPI
GetTempPathW(
	     IN DWORD nBufferLength,
	     OUT LPWSTR lpBuffer)
{
    char TempBuffer[ MAX_PATH ];
    DWORD dwRetVal = 0;

    PERF_ENTRY(GetTempPathW);
    ENTRY( "GetTempPathW(nBufferLength=%u, lpBuffer=%p)\n",
           nBufferLength, lpBuffer);

    dwRetVal = GetTempPathA( MAX_PATH, TempBuffer );

    if ( dwRetVal > nBufferLength )
    {
        ERROR( "The buffer was not large enough.\n " )
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
        *lpBuffer = '\0';
    }
    else if ( dwRetVal != 0 )
    {
        /* Convert to wide. */
        if ( 0 == MultiByteToWideChar( CP_ACP, 0, TempBuffer, -1, 
                                       lpBuffer, dwRetVal + 1 ) )
        {
            ASSERT( "An error occured while converting the string to wide.\n" );
            SetLastError( ERROR_INTERNAL_ERROR );
            dwRetVal = 0;
        }
    }
    else
    {
        ERROR( "The function failed.\n" );
        *lpBuffer = '\0';
    }

    LOGEXIT("GetTempPathW returns DWORD %u\n", dwRetVal );
    PERF_EXIT(GetTempPathW);
    return dwRetVal;
}



/*++
Function:
  FileDosToUnixPathA

Abstract:
  Change a DOS path to a Unix path.

  Replaces '\' by '/', removes any trailing dots on directory/filenames,
  and changes '*.*' to be equal to '*'

Parameter:
  IN/OUT lpPath: path to be modified
--*/
void
FILEDosToUnixPathA(
       LPSTR lpPath)
{
    LPSTR p;
    LPSTR pPointAtDot=NULL;
    char charBeforeFirstDot='\0';


    TRACE("Original DOS path = [%s]\n", lpPath);

    if (!lpPath)
    {
        return;
    }

    for (p = lpPath; *p; p++)
    {
        /* Make the \\ to / switch first */
        if (*p == '\\')
        {
            /* Replace \ with / */
            *p = '/';
        }

        if (pPointAtDot)
        {
            /* If pPointAtDot is not NULL, it is pointing at the first encountered
               dot.  If we encountered a \, that means it could be a trailing dot */
            if (*p == '/')
            {
                /* If char before the first dot is a '\' or '.' (special case if the
                   dot is the first char in the path) , then we leave it alone,
                   because it is either . or .., otherwise it is a trailing dot
                   pattern and will be truncated */
                if (charBeforeFirstDot != '.' && charBeforeFirstDot != '/')
                {
                    memmove(pPointAtDot,p,(strlen(p)*sizeof(char))+1);
                    p = pPointAtDot;
                }
                pPointAtDot = NULL; /* Need to reset this */
            }
            else if (*p == '*')
            {
                /* Check our size before doing anything with our pointers */
                if ((p - lpPath) >= 3)
                {
                    /* At this point, we know that there is 1 or more dots and
                       then a star.  AND we know the size of our string at this
                       point is at least 3 (so we can go backwards from our pointer
                       safely AND there could possilby be two characters back)
                       So lets check if there is a '*' and a '.' before, if there
                       is, replace just a '*'.  Otherwise, reset pPointAtDot to NULL
                       and do nothing */
                    if (p[-2] == '*' &&
                        p[-1] == '.' &&
                        p[0] == '*')
                    {
                        memmove(&(p[-2]),p,(strlen(p)*sizeof(char))+1);
                    }

                    pPointAtDot  = NULL;
                }
            }
            else if (*p != '.')
            {
                /* If we are here, that means that this is NOT a trailing dot,
                   some other character is here, so forget our pointer */
                pPointAtDot = NULL;
            }
        }
        else
        {
            if (*p == '.')
            {
                /* If pPointAtDot is NULL, and we encounter a dot, save the pointer */
                pPointAtDot = p;
                if (pPointAtDot != lpPath)
                {
                    charBeforeFirstDot = p[-1];
                }
                else
                {
                    charBeforeFirstDot = lpPath[0];
                }
            }
        }
    }

    /* If pPointAtDot still points at anything, then we still have trailing dots.
       Truncate at pPointAtDot, unless the dots are path specifiers (. or ..) */
    if (pPointAtDot)
    {
        /* make sure the trailing dots don't follow a '/', and that they aren't 
           the only thing in the name */
        if(pPointAtDot != lpPath && *(pPointAtDot-1) != '/')
        {
            *pPointAtDot = '\0';
        }
    }

    TRACE("Resulting Unix path = [%s]\n", lpPath);
}


/*++
Function:
  FileDosToUnixPathW

Abstract:
  Change a DOS path to a Unix path.

  Replaces '\' by '/', removes any trailing dots on directory/filenames,
  and changes '*.*' to be equal to '*'

Parameter:
  IN/OUT lpPath: path to be modified
--*/
void
FILEDosToUnixPathW(
       LPWSTR lpPath)
{
    LPWSTR p;
    LPWSTR pPointAtDot=NULL;
    WCHAR charBeforeFirstDot='\0';


    TRACE("Original DOS path = [%S]\n", lpPath);

    if (!lpPath)
    {
        return;
    }

    for (p = lpPath; *p; p++)
    {
        /* Make the \\ to / switch first */
        if (*p == '\\')
        {
            /* Replace \ with / */
            *p = '/';
        }

        if (pPointAtDot)
        {
            /* If pPointAtDot is not NULL, it is pointing at the first encountered
               dot.  If we encountered a \, that means it could be a trailing dot */
            if (*p == '/')
            {
                /* If char before the first dot is a '\' or '.' (special case if the
                   dot is the first char in the path) , then we leave it alone,
                   because it is either . or .., otherwise it is a trailing dot
                   pattern and will be truncated */
                if (charBeforeFirstDot != '.' && charBeforeFirstDot != '/')
                {
                    memmove(pPointAtDot,p,((PAL_wcslen(p)+1)*sizeof(WCHAR)));
                    p = pPointAtDot;
                }
                pPointAtDot = NULL; /* Need to reset this */
            }
            else if (*p == '*')
            {
                /* Check our size before doing anything with our pointers */
                if ((p - lpPath) >= 3)
                {
                    /* At this point, we know that there is 1 or more dots and
                       then a star.  AND we know the size of our string at this
                       point is at least 3 (so we can go backwards from our pointer
                       safely AND there could possilby be two characters back)
                       So lets check if there is a '*' and a '.' before, if there
                       is, replace just a '*'.  Otherwise, reset pPointAtDot to NULL
                       and do nothing */
                    if (p[-2] == '*' &&
                        p[-1] == '.' &&
                        p[0] == '*')
                    {
                        memmove(&(p[-2]),p,(PAL_wcslen(p)*sizeof(WCHAR)));
                    }

                    pPointAtDot  = NULL;
                }
            }
            else if (*p != '.')
            {
                /* If we are here, that means that this is NOT a trailing dot,
                   some other character is here, so forget our pointer */
                pPointAtDot = NULL;
            }
        }
        else
        {
            if (*p == '.')
            {
                /* If pPointAtDot is NULL, and we encounter a dot, save the pointer */
                pPointAtDot = p;
                if (pPointAtDot != lpPath)
                {
                    charBeforeFirstDot = p[-1];
                }
                else
                {
                    charBeforeFirstDot = lpPath[0];
                }
            }
        }
    }

    /* If pPointAtDot still points at anything, then we still have trailing dots.
       Truncate at pPointAtDot, unless the dots are path specifiers (. or ..) */
    if (pPointAtDot)
    {
        /* make sure the trailing dots don't follow a '/', and that they aren't 
           the only thing in the name */
        if(pPointAtDot != lpPath && *(pPointAtDot-1) != '/')
        {
            *pPointAtDot = '\0';
        }
    }

    TRACE("Resulting Unix path = [%S]\n", lpPath);
}


/*++
Function:
  FileUnixToDosPathA

Abstract:
  Change a Unix path to a DOS path. Replace '/' by '\'.

Parameter:
  IN/OUT lpPath: path to be modified
--*/
void
FILEUnixToDosPathA(
       LPSTR lpPath)
{
    LPSTR p;

    TRACE("Original Unix path = [%s]\n", lpPath);

    if (!lpPath)
        return;

    for (p = lpPath; *p; p++)
    {
        if (*p == '/')
            *p = '\\';
    }

    TRACE("Resulting DOS path = [%s]\n", lpPath);
}


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
				     LPSTR  lpBuffer )
{
    int    full_len, dir_len, i;
    LPCSTR lpDirEnd;
    DWORD  dwRetLength;

    full_len = lstrlenA( lpFullPath );

    /* look for the first path separator backwards */
    lpDirEnd = lpFullPath + full_len - 1;
    while( lpDirEnd >= lpFullPath && *lpDirEnd != '/' && *lpDirEnd != '\\')
	--lpDirEnd;

    dir_len = lpDirEnd - lpFullPath + 1; /* +1 for fencepost */

    if ( dir_len <= 0 )
    {
	dwRetLength = 0;
    }
    else if ( dir_len >= nBufferLength )
    {
	dwRetLength = dir_len + 1; /* +1 for NULL char */
    }
    else
    {
	/* put the directory into the buffer, including 1 or more
	   trailing path separators */
	for( i = 0; i < dir_len; ++i )
	    *(lpBuffer + i) = *(lpFullPath + i);

	*(lpBuffer + i) = '\0';

	dwRetLength = dir_len;
    }

    return( dwRetLength );
}

/*++
Function:
  FILEGetFileNameFromFullPath

Given a full path, return a pointer to the first char of the filename part.
--*/
LPCSTR FILEGetFileNameFromFullPathA( LPCSTR lpFullPath )
{
    int DirLen = FILEGetDirectoryFromFullPathA( lpFullPath, 0, NULL );

    if ( DirLen > 0 )
    {
        return lpFullPath + DirLen - 1;
    }
    else
    {
        return lpFullPath;
    }
}

/*++
FILECanonicalizeRealPath
    Wraps realpath() to hide platform differences. See the man page for
    realpath(3) for details of how realpath() works.
    
    On systems on which realpath() allows the last path component to not
    exist, this is a straight thunk through to realpath(). On other
    systems, we remove the last path component, then call realpath().
--*/
LPSTR FILECanonicalizeRealPath(LPCSTR lpUnixPath, LPSTR lpBuffer)
{
    char *file = NULL;
#if REALPATH_SUPPORTS_NONEXISTENT_FILES
    file = realpath(lpUnixPath, lpBuffer);
#else   // REALPATH_SUPPORTS_NONEXISTENT_FILES
    char existingPath[MAX_PATH];
    char *separator;
    char *filename;
    int filenameLength;

    strncpy(existingPath, lpUnixPath, sizeof(existingPath));
    if (existingPath[MAX_PATH - 1] != '\0')
    {
        file = NULL;
    }
    else
    {
        separator = strrchr(existingPath, '/');
        if (separator == NULL)
        {
            char realpathBuffer[MAX_PATH];

            filenameLength = strlen(lpUnixPath);
            // Only a file name. Use getcwd() for the path, with the
            // understanding that we'll append the file name later.
            if (getcwd(realpathBuffer, MAX_PATH - filenameLength - 1) != NULL)
            {
                file = realpath(realpathBuffer, lpBuffer);
            }
            filename = existingPath;
        }
        else
        {
            *separator = '\0';
            filenameLength = strlen(separator + 1);
            file = realpath(existingPath, lpBuffer);
            filename = separator + 1;
        }
        // Append the file name.
        if (file != NULL)
        {
            if (strlen(lpBuffer) + filenameLength + 2 > MAX_PATH)
            {
                file = NULL;
            }
            else
            {
                strcat(lpBuffer, "/");
                strcat(lpBuffer, filename);
            }
        }
    }
#endif  // REALPATH_SUPPORTS_NONEXISTENT_FILES
    return file;
}

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
void FILECanonicalizePath(LPSTR lpUnixPath)
{
    LPSTR slashslashptr;
    LPSTR dotdotptr;
    LPSTR slashdotptr;
    LPSTR slashptr;

    /* step 1 : replace '//' sequences by a single '/' */

    slashslashptr = lpUnixPath;
    while(1)
    {
        slashslashptr = strstr(slashslashptr,"//");
        if(NULL == slashslashptr)
        {
            break;
        }
        /* remove extra '/' */
        TRACE("stripping '//' from %s\n", lpUnixPath);
        memmove(slashslashptr,slashslashptr+1,strlen(slashslashptr+1)+1);
    }

    /* step 2 : replace '/./' sequences by a single '/' */

    slashdotptr = lpUnixPath;
    while(1)
    {
        slashdotptr = strstr(slashdotptr,"/./");
        if(NULL == slashdotptr)
        {
            break;
        }
        /* strip the extra '/.' */
        TRACE("removing '/./' sequence from %s\n", lpUnixPath);
        memmove(slashdotptr,slashdotptr+2,strlen(slashdotptr+2)+1);
    }

    /* step 3 : replace '/<name>/../' sequences by a single '/' */

    while(1)
    {
        dotdotptr = strstr(lpUnixPath,"/../");
        if(NULL == dotdotptr)
        {
            break;
        }
        if(dotdotptr == lpUnixPath)
        {
            /* special case : '/../' at the beginning of the path are replaced
               by a single '/' */
            TRACE("stripping leading '/../' from %s\n", lpUnixPath);
            memmove(lpUnixPath, lpUnixPath+3,strlen(lpUnixPath+3)+1);
            continue;
        }
        
        /* null-terminate the string before the '/../', so that strrchr will 
           start looking right before it */
        *dotdotptr = '\0';
        slashptr = strrchr(lpUnixPath,'/');
        if(NULL == slashptr)
        {
            /* this happens if this function was called with a relative path. 
               don't do that.  */
            ASSERT("can't find leading '/' before '/../ sequence\n");
            break;
        }
        TRACE("removing '/<dir>/../' sequence from %s\n", lpUnixPath);
        memmove(slashptr,dotdotptr+3,strlen(dotdotptr+3)+1);
    }

    /* step 4 : remove a trailing '/..' */

    dotdotptr = strstr(lpUnixPath,"/..");
    if(dotdotptr == lpUnixPath)
    {
        /* if the full path is simply '/..', replace it by '/' */
        lpUnixPath[1] = '\0';
    }
    else if(NULL != dotdotptr && '\0' == dotdotptr[3])
    {
        *dotdotptr = '\0';
        slashptr = strrchr(lpUnixPath,'/');
        if(NULL != slashptr)
        {
            *slashptr = '\0';
        }
    }

    /* step 5 : remove a traling '/.' */

    slashdotptr = strstr(lpUnixPath,"/.");
    if (slashdotptr != NULL && slashdotptr[2] == '\0')
    {
        if(slashdotptr == lpUnixPath)
        {
            // if the full path is simply '/.', replace it by '/' */
            lpUnixPath[1] = '\0';
        }
        else
        {
            *slashdotptr = '\0';
        }
    }
}


/*++
Function:
  SearchPathA

See MSDN doc.

PAL-specific notes :
-lpPath must be non-NULL; path delimiters are platform-dependent (':' for Unix)
-lpFileName must be non-NULL, may be an absolute path
-lpExtension must be NULL
-lpFilePart (if non-NULL) doesn't need to be used (but we do)
--*/
DWORD
PALAPI
SearchPathA(
    IN LPCSTR lpPath,
    IN LPCSTR lpFileName,
    IN LPCSTR lpExtension,
    IN DWORD nBufferLength,
    OUT LPSTR lpBuffer,
    OUT LPSTR *lpFilePart
    )
{
    DWORD nRet = 0;
    CHAR FullPath[MAX_PATH];
    CHAR CanonicalFullPath[MAX_PATH];
    LPCSTR pPathStart;
    LPCSTR pPathEnd;
    size_t PathLength;
    size_t FileNameLength;
    DWORD dw;

    PERF_ENTRY(SearchPathA);
    ENTRY("SearchPathA(lpPath=%p (%s), lpFileName=%p (%s), lpExtension=%p, "
          "nBufferLength=%u, lpBuffer=%p, lpFilePart=%p)\n",
      lpPath,
      lpPath, lpFileName, lpFileName, lpExtension, nBufferLength, lpBuffer, 
          lpFilePart);

    /* validate parameters */
    
    if(NULL == lpPath)
    {
        ASSERT("lpPath may not be NULL\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }
    if(NULL == lpFileName)
    {
        ASSERT("lpFileName may not be NULL\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }
    if(NULL != lpExtension)
    {
        ASSERT("lpExtension must be NULL, is %p instead\n", lpExtension);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    FileNameLength = strlen(lpFileName);

    /* special case : if file name contains absolute path, don't search the 
       provided path */
    if('\\' == lpFileName[0] || '/' == lpFileName[0])
    {
        if(FileNameLength >= MAX_PATH)
        {
            WARN("absolute file name <%s> is too long\n", lpFileName);
            SetLastError(ERROR_INVALID_PARAMETER);
            goto done;
        }
        /* Canonicalize the path to deal with back-to-back '/', etc. */
        dw = GetFullPathNameA(lpFileName, MAX_PATH, CanonicalFullPath, NULL);
        if (dw == 0 || dw >= MAX_PATH) 
        {
            WARN("couldn't canonicalize path <%s>, error is %#x. failing.\n",
                 FullPath, GetLastError());
            SetLastError(ERROR_INVALID_PARAMETER);
            goto done;
        }

        /* see if the file exists */
        if(0 == access(CanonicalFullPath, F_OK))
        {
            /* found it */
            nRet = dw;
        }
    }
    else
    {
        LPCSTR pNextPath;
        
        pNextPath = lpPath;
    
        while (*pNextPath) 
        {
            pPathStart = pNextPath;
            
            /* get a pointer to the end of the first path in pPathStart */
            pPathEnd = strchr(pPathStart, ':');
            if (!pPathEnd)
            {
                pPathEnd = pPathStart + strlen(pPathStart);
                /* we want to break out of the loop after this pass, so let
                   *pNextPath be '\0' */
                pNextPath = pPathEnd;  
            }
            else
            {
                /* point to the next component in the path string */
                pNextPath = pPathEnd+1;
            }
    
            PathLength = pPathEnd-pPathStart;
    
            if (PathLength+FileNameLength+1 >= MAX_PATH) 
            {
                /* The path+'/'+file length is too long.  Skip it. */
                WARN("path component %.*s is too long, skipping it\n", 
                     (int)PathLength, pPathStart);
                continue;
            }
            else if(0 == PathLength)
            {
                /* empty component : there were 2 consecutive ':' */
                continue;
            }
    
            /* Construct a pathname by concatenating one path from lpPath, '/' 
               and lpFileName */
            memcpy(FullPath, pPathStart, PathLength);
            FullPath[PathLength] = '/';
            strcpy(&FullPath[PathLength+1], lpFileName);
    
            /* Canonicalize the path to deal with back-to-back '/', etc. */
            dw = GetFullPathNameA(FullPath, MAX_PATH,
                                  CanonicalFullPath, NULL);
            if (dw == 0 || dw >= MAX_PATH) 
            {
                /* Call failed - possibly low memory.  Skip the path */
                WARN("couldn't canonicalize path <%s>, error is %#x. "
                     "skipping it\n", FullPath, GetLastError());
                continue;
            }
    
            /* see if the file exists */
            if(0 == access(CanonicalFullPath, F_OK))
            {
                /* found it */
                nRet = dw;
                break;
            }
        }
    }

    if (nRet == 0) 
    {
       /* file not found anywhere; say so. in Windows, this always seems to say
          FILE_NOT_FOUND, even if path doesn't exist */
       SetLastError(ERROR_FILE_NOT_FOUND);
    }
    else
    {
        if (nRet < nBufferLength) 
        {
            if(NULL == lpBuffer)
            {
                /* Windows merily crashes here, but let's not */
                ERROR("caller told us buffer size was %d, but buffer is NULL\n",
                      nBufferLength);
                SetLastError(ERROR_INVALID_PARAMETER);
                nRet = 0;
                goto done;
            }
            strcpy(lpBuffer, CanonicalFullPath);
            if(NULL != lpFilePart)
            {
                *lpFilePart = strrchr(lpBuffer,'/');
                if(NULL == *lpFilePart)
                {
                    ASSERT("no '/' in full path!\n");
                }
                else
                {
                    /* point to character after last '/' */
                    (*lpFilePart)++;
                }
            }
        }
        else
        {
            /* if buffer is too small, report required length, including 
               terminating null */
            nRet++;
        }
    }
done:
    LOGEXIT("SearchPathA returns DWORD %u\n", nRet);
    PERF_EXIT(SearchPathA);
    return nRet;
}


/*++
Function:
  SearchPathW

See MSDN doc.

PAL-specific notes :
-lpPath must be non-NULL; path delimiters are platform-dependent (':' for Unix)
-lpFileName must be non-NULL, may be an absolute path
-lpExtension must be NULL
-lpFilePart (if non-NULL) doesn't need to be used (but we do)
--*/
DWORD
PALAPI
SearchPathW(
    IN LPCWSTR lpPath,
    IN LPCWSTR lpFileName,
    IN LPCWSTR lpExtension,
    IN DWORD nBufferLength,
    OUT LPWSTR lpBuffer,
    OUT LPWSTR *lpFilePart
    )
{
    DWORD nRet = 0;
    WCHAR FullPath[MAX_PATH];
    LPCWSTR pPathStart;
    LPCWSTR pPathEnd;
    size_t PathLength;
    size_t FileNameLength;
    DWORD dw;
    char AnsiPath[MAX_PATH];
    WCHAR CanonicalPath[MAX_PATH];

    PERF_ENTRY(SearchPathW);
    ENTRY("SearchPathW(lpPath=%p (%S), lpFileName=%p (%S), lpExtension=%p, "
          "nBufferLength=%u, lpBuffer=%p, lpFilePart=%p)\n",
	  lpPath,
	  lpPath, lpFileName, lpFileName, lpExtension, nBufferLength, lpBuffer, 
          lpFilePart);

    /* validate parameters */
    
    if(NULL == lpPath)
    {
        ASSERT("lpPath may not be NULL\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }
    if(NULL == lpFileName)
    {
        ASSERT("lpFileName may not be NULL\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }
    if(NULL != lpExtension)
    {
        ASSERT("lpExtension must be NULL, is %p instead\n", lpExtension);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }          

    /* special case : if file name contains absolute path, don't search the 
       provided path */
    if('\\' == lpFileName[0] || '/' == lpFileName[0])
    {
        /* Canonicalize the path to deal with back-to-back '/', etc. */
        dw = GetFullPathNameW(lpFileName, MAX_PATH, CanonicalPath, NULL);
        if (dw == 0 || dw >= MAX_PATH) 
        {
            WARN("couldn't canonicalize path <%S>, error is %#x. failing.\n",
                 lpPath, GetLastError());
            SetLastError(ERROR_INVALID_PARAMETER);
            goto done;
        }

        /* see if the file exists */
	WideCharToMultiByte(CP_ACP, 0, CanonicalPath, -1,
			    AnsiPath, MAX_PATH, NULL, NULL);
        if(0 == access(AnsiPath, F_OK))
        {
            /* found it */
            nRet = dw;
        }
    }
    else
    {
        LPCWSTR pNextPath;

        pNextPath = lpPath;
    
        FileNameLength = PAL_wcslen(lpFileName);

        while (*pNextPath) 
        {
            pPathStart = pNextPath;
    
            /* get a pointer to the end of the first path in pPathStart */
            pPathEnd = PAL_wcschr(pPathStart, ':');
            if (!pPathEnd)
            {
                pPathEnd = pPathStart + PAL_wcslen(pPathStart);
                /* we want to break out of the loop after this pass, so let
                   *pNextPath be '\0' */
                pNextPath = pPathEnd;
            }
            else
            {
                /* point to the next component in the path string */
                pNextPath = pPathEnd+1;
            }
    
            PathLength = pPathEnd-pPathStart;
    
            if (PathLength+FileNameLength+1 >= MAX_PATH) 
            {
                /* The path+'/'+file length is too long.  Skip it. */
                WARN("path component %.*S is too long, skipping it\n", 
                     (int)PathLength, pPathStart);
                continue;
            }
            else if(0 == PathLength)
            {
                /* empty component : there were 2 consecutive ':' */
                continue;
            }
    
            /* Construct a pathname by concatenating one path from lpPath, '/' 
               and lpFileName */
            memcpy(FullPath, pPathStart, PathLength*sizeof(WCHAR));
            FullPath[PathLength] = '/';
            PAL_wcscpy(&FullPath[PathLength+1], lpFileName);
    
            /* Canonicalize the path to deal with back-to-back '/', etc. */
            dw = GetFullPathNameW(FullPath, MAX_PATH,
                                  CanonicalPath, NULL);
            if (dw == 0 || dw >= MAX_PATH) 
            {
                /* Call failed - possibly low memory.  Skip the path */
                WARN("couldn't canonicalize path <%S>, error is %#x. "
                     "skipping it\n", FullPath, GetLastError());
                continue;
            }
    
            /* see if the file exists */
            WideCharToMultiByte(CP_ACP, 0, CanonicalPath, -1,
                                AnsiPath, MAX_PATH, NULL, NULL);
            if(0 == access(AnsiPath, F_OK))
            {
                /* found it */
                nRet = dw;
                break;
            }
        }
    }

    if (nRet == 0) 
    {
       /* file not found anywhere; say so. in Windows, this always seems to say
          FILE_NOT_FOUND, even if path doesn't exist */
       SetLastError(ERROR_FILE_NOT_FOUND);
    }
    else
    {
        /* find out the required buffer size, copy path to buffer if it's 
           large enough */
        nRet = PAL_wcslen(CanonicalPath)+1;
        if(nRet <= nBufferLength)
        {
            if(NULL == lpBuffer)
            {
                /* Windows merily crashes here, but let's not */
                ERROR("caller told us buffer size was %d, but buffer is NULL\n",
                      nBufferLength);
                SetLastError(ERROR_INVALID_PARAMETER);
                nRet = 0;
                goto done;
            }
            PAL_wcscpy(lpBuffer, CanonicalPath);

            /* don't include the null-terminator in the count if buffer was 
               large enough */
            nRet--;
            
            if(NULL != lpFilePart)
            {
                *lpFilePart = PAL_wcsrchr(lpBuffer, '/');
                if(NULL == *lpFilePart)
                {
                    ASSERT("no '/' in full path!\n");
                }
                else
                {
                    /* point to character after last '/' */
                    (*lpFilePart)++;
                }
            }
        }
    }
done:
    LOGEXIT("SearchPathW returns DWORD %u\n", nRet);
    PERF_EXIT(SearchPathW);
    return nRet;
}

