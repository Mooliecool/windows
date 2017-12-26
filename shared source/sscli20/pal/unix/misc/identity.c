/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

misc/identity.cpp

Abstract:

Implementation of GetComputerNameW and GetUserNameW functions. 

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/misc.h"
#include "pal/thread.h"
#include "pal/identity.h"

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#if HAVE_NETDB_H && NETDB_DEFINES_MAXHOSTNAMELEN
#include <netdb.h>
#elif HAVE_SYS_PARAM_H && SYS_PARAM_DEFINES_MAXHOSTNAMELEN
#include <sys/param.h>
#endif

#if HAVE_SYSCONF && defined(_SC_GETPW_R_SIZE_MAX)
#include <limits.h> // for INT_MAX
#endif

SET_DEFAULT_DEBUG_CHANNEL(MISC);

#if HAVE_GETPWUID_R

#define DEFAULT_PASSWORD_BUFFER_SIZE 1024
static DWORD dwInitialPasswdBufferSize = DEFAULT_PASSWORD_BUFFER_SIZE;

#else // HAVE_GETPWUID_R

static CRITICAL_SECTION identity_critsec;

#endif // HAVE_GETPWUID_R


/*++
Function:
    IdentityInitialize

Intitialization function called from PAL_Initialize.
Initializes the critical section for the case when thread-safe
getpwuid_r is not available.

--*/
BOOL
IdentityInitialize(void)
{
#if HAVE_GETPWUID_R
 
#if HAVE_SYSCONF && defined(_SC_GETPW_R_SIZE_MAX)
    long lBufferSize = 0;
    lBufferSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    
    if ((long)INT_MAX < lBufferSize)
    {
        ERROR("sysconf(_SC_GETPW_R_SIZE_MAX) returns %ld which is > INT_MAX (%u)\n",
                lBufferSize, INT_MAX);
        return FALSE;
    }

    if (0 >= (int)(lBufferSize))
    {
        WARN("sysconf(_SC_GETPW_R_SIZE_MAX) returns %ld, using %u as the buffer size instead\n", 
                 lBufferSize, dwInitialPasswdBufferSize);
    }
    else
    {
        TRACE("sysconf(_SC_GETPW_R_SIZE_MAX) returns %ld\n", lBufferSize);
        dwInitialPasswdBufferSize = (DWORD)lBufferSize;
    }

#endif // HAVE_SYSCONF && _SC_GETPW_R_SIZE_MAX

#else // HAVE_GETPWUID_R

    if (0 != SYNCInitializeCriticalSection(&identity_critsec))
    {
        return FALSE;
    }

#endif // HAVE_GETPWUID_R

    return TRUE;
}


/*++
Function:
    IdentityCleanup

Termination function called from PAL_Terminate.
Deletes the critical section for the case when thread-safe
getpwuid_r is not available.

--*/
void
IdentityCleanup(void)
{
#if !HAVE_GETPWUID_R
    DeleteCriticalSection(&identity_critsec);
#endif
}

/*++
Function:
    GetUserNameW

Uses getpwuid_r to get the user name and if it's not available uses
getpwuid (with the safety of a critical section). See MSDN for functional spec.

--*/
PALIMPORT
BOOL
PALAPI
GetUserNameW(
    OUT LPWSTR lpBuffer, // address of name buffer
    IN OUT LPDWORD nSize )    // address of size of name buffer
{
    BOOL bRet = FALSE;
    struct passwd *pPasswd = NULL;
    char *szUserName = NULL;
    DWORD cwchLen = 0;
    int euid = -1;
    int iRet = -1;

#if HAVE_GETPWUID_R
    
    char *pchBuffer = NULL;
    DWORD dwBufLen = 0;
    struct passwd sPasswd;
    
#endif // HAVE_GETPWUID_R

    PERF_ENTRY(GetUserNameW);
    ENTRY("GetUserNameW(lpBuffer = %p, nSize = %p (%d)\n",
          lpBuffer, nSize, nSize?*nSize:0);

    euid = geteuid();

    if (NULL == lpBuffer || NULL == nSize)
    {
        ERROR("lpBuffer == NULL or nSize == NULL");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

#if HAVE_GETPWUID_R

    dwBufLen = dwInitialPasswdBufferSize;

    while (NULL == pPasswd)
    {
        pchBuffer = (char*) malloc(sizeof(pchBuffer[0]) * dwBufLen);
        if (NULL == pchBuffer)
        {
            SetLastError(ERROR_OUTOFMEMORY);
            goto done;
        }

        iRet = getpwuid_r(euid, &sPasswd, pchBuffer, dwBufLen, &pPasswd);
#if GETPWUID_R_SETS_ERRNO
        if (0 != iRet)
        {
            iRet = errno; // some systems sets errno instead of returning error
        }
#endif // GETPWUID_R_SETS_ERRNO

        if (0 != iRet)
        {
            WARN("getpwuid_r(%d) returns %d for a buffer size of %d, error string is %s\n", 
                        euid, iRet, dwBufLen, strerror(iRet));

            if (ERANGE == iRet) // need a bigger buffer
            {
                free(pchBuffer);
                pchBuffer = NULL;
                pPasswd = NULL;
                dwBufLen *= 2; // double the buffer
                continue; // try again
            }

            SetLastError(ERROR_INTERNAL_ERROR);
            goto done;
        }


        if (NULL == pPasswd || NULL == pPasswd->pw_name)
        {
            // No matching entry found! something failed somewhere.
            ERROR("getpwuid_r(%d) returned %p with name NULL!\n", euid, pPasswd);
            SetLastError(ERROR_INTERNAL_ERROR);
            goto done;
        }
    }

    szUserName = pPasswd->pw_name;

#else // HAVE_GETPWUID_R

    SYNCEnterCriticalSection(&identity_critsec);
    pPasswd = getpwuid(euid);

    if ((NULL == pPasswd) || (NULL == pPasswd->pw_name))
    {
        SYNCLeaveCriticalSection(&identity_critsec);
        ERROR("getpwuid(%d) returned %p with name NULL! error (%d) is %s\n",
                euid, pPasswd, errno, strerror(errno));
        SetLastError(ERROR_INTERNAL_ERROR);
        goto done;
    }

    // make a copy so that we can modify it
    szUserName = strdup(pPasswd->pw_name);
    if (NULL == szUserName)
    {
        SYNCLeaveCriticalSection(&identity_critsec);
        SetLastError(ERROR_OUTOFMEMORY);
        goto done;
    }

    SYNCLeaveCriticalSection(&identity_critsec);
#endif // HAVE_GETPWUID_R

    // truncate the user name if it exceeds the maximum allowed limit
    if (strlen(szUserName) > UNLEN)
    {
        szUserName[UNLEN] = '\0';
    }

    // Copy from pPasswd->pw_name
    cwchLen = MultiByteToWideChar(CP_ACP, 0, szUserName, -1, lpBuffer, *nSize);
    if (0 == cwchLen)
    {
        ERROR ("MultiByteToWideChar failed with error %d when trying to convert the username "
               "%s to wide char\n", GetLastError(), szUserName);
        if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
        {
            // Find the required size (including NULL)
            cwchLen = MultiByteToWideChar(CP_ACP, 0, szUserName, -1, NULL, 0);
            if (0 == cwchLen)
            {
                ERROR ("MultiByteToWideChar failed with error %d when trying to find the size of "
                       "%s in wide chars\n", GetLastError(), szUserName);
                SetLastError(ERROR_INTERNAL_ERROR);
            } else {
                // Update the required size
                *nSize = cwchLen;
                SetLastError(ERROR_MORE_DATA);
            }
        }
        goto done;
    }

    *nSize = cwchLen;
    bRet = TRUE;

done:
#if HAVE_GETPWUID_R
    if (NULL != pchBuffer)
    {
        free(pchBuffer);
    }
#else // HAVE_GETPWUID_R
    if (NULL != szUserName)
    {
        free(szUserName);
    }
#endif // HAVE_GETPWUID_R

    LOGEXIT("GetUserNameW returning BOOL %d\n", bRet);
    PERF_EXIT(GetUserNameW);
    return bRet;
}

#ifndef MAXHOSTNAMELEN
// AIX doesn't have MAXHOSTNAMELEN, it recommends using 256
#define MAXHOSTNAMELEN 256
#endif // MAXHOSTNAMELEN

/*++
Function:
    GetComputerNameW

Uses gethostname to get the computer name. See MSDN for functional spec.

--*/
PALIMPORT
BOOL
PALAPI
GetComputerNameW(
    OUT LPWSTR lpBuffer, // address of name buffer
    IN OUT LPDWORD nSize)    // address of size of name buffer
{
    BOOL bRet = FALSE;
    char szHostName[MAXHOSTNAMELEN+1];
    char *pchDot = NULL;
    DWORD cwchLen = 0;

    PERF_ENTRY(GetComputerNameW);
    ENTRY("GetComputerNameW(lpBuffer = %p, nSize = %p (%d)\n",
          lpBuffer, nSize, nSize?*nSize:0);

    if (NULL == lpBuffer || NULL == nSize)
    {
        ERROR("lpBuffer == NULL or nSize == NULL");
        SetLastError(ERROR_INVALID_PARAMETER);
        goto done;
    }

    if (0 != gethostname(szHostName, sizeof(szHostName)/sizeof(szHostName[0])))
    {
        ERROR("gethostname failed with error (%d) %s\n", errno, strerror(errno));
        SetLastError(ERROR_INTERNAL_ERROR);
        goto done;
    }

    // Null terminate the string
    szHostName[sizeof(szHostName)/sizeof(szHostName[0])-1] = '\0';

    // some OSes return the hostname with the domain name included.
    // We want to return only the host part of the name (see the spec for
    // more details
    pchDot = strchr(szHostName, '.');
    if (NULL != pchDot)
    {
        *pchDot = '\0'; // remove the domain name info
    }

    // clip the hostname to MAX_COMPUTERNAME_LENGTH
    if (sizeof(szHostName) > MAX_COMPUTERNAME_LENGTH)
    {
        szHostName[MAX_COMPUTERNAME_LENGTH] = '\0';
    }

    // copy the hostname (including NULL character)
    cwchLen = MultiByteToWideChar(CP_ACP, 0, szHostName, -1, lpBuffer, *nSize);
    if (0 == cwchLen) 
    {
        ERROR ("MultiByteToWideChar failed with error %d when trying to convert the hostname "
                "%s to wide char\n", GetLastError(), szHostName);
        if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
        {
            // Find the required size (including NULL)
            cwchLen = MultiByteToWideChar(CP_ACP, 0, szHostName, -1, NULL, 0);
            if (0 == cwchLen)
            {
                ERROR ("MultiByteToWideChar failed with error %d when trying to find the size of "
                       "%s in wide chars\n", GetLastError(), szHostName);
                SetLastError(ERROR_INTERNAL_ERROR);
            } else {
                // Update the required size
                *nSize = cwchLen - 1; // don't include the NULL
                SetLastError(ERROR_BUFFER_OVERFLOW);
            }
        }
        goto done;
    }

    *nSize = cwchLen - 1; // don't include the NULL
    bRet = TRUE;

done:
    LOGEXIT("GetComputerNameW returning BOOL %d\n", bRet);
    PERF_EXIT(GetComputerNameW);
    return bRet;
}
