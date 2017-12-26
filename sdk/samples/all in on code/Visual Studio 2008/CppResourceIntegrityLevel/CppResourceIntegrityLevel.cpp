/****************************** Module Header ******************************\
* Module Name:  CppResourceIntegrityLevel.cpp
* Project:      CppResourceIntegrityLevel
* Copyright (c) Microsoft Corporation.
* 
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#define _WIN32_WINNT    0x0600

#include <stdio.h>
#include <windows.h>
#include <accctrl.h>
#include <aclapi.h>
#pragma endregion


#pragma region SetResourceIntegrityLevel

//
//   FUNCTION: SetResourceIntegrityLevel(LPWSTR, SE_OBJECT_TYPE, DWORD)
//
//   PURPOSE: 
//
//   PARAMETERS: 
//   * pszObjectName - A pointer to a null-terminated string that specifies 
//     the name of the object for which to set security information. This can 
//     be the name of a local or remote file or directory on an NTFS file 
//     system, network share, registry key, semaphore, event, mutex, file 
//     mapping, or waitable timer.
//   * ObjectType - A value of the SE_OBJECT_TYPE enumeration that indicates 
//     the type of object named by the pObjectName parameter.
//   * dwIntegrityLevel - 
//
//   RETURN VALUE: 
//
BOOL SetResourceIntegrityLevel(LPWSTR pszObjectName, 
                               SE_OBJECT_TYPE ObjectType, 
                               DWORD dwIntegrityLevel)
{
    DWORD dwError = ERROR_SUCCESS;
    PSID pSid = NULL;
    PACL pSacl = NULL;
    DWORD cbSacl = 0;
    
    // Allocate and initialize a SID of the integrity level.
    SID_IDENTIFIER_AUTHORITY MLAuthority = SECURITY_MANDATORY_LABEL_AUTHORITY;
    if (!AllocateAndInitializeSid(&MLAuthority, 1, dwIntegrityLevel, 
        0, 0, 0, 0, 0, 0, 0, &pSid))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Calculate the initial size of an ACL structure by adding the size of 
    // the ACL structure, the size of each ACE structure and the length of 
    // the SID that the ACL is to contain minus the SidStart member (DWORD) 
    // of the ACE, and aligning the size to the nearest DWORD. 
    cbSacl = sizeof(*pSacl) + sizeof(ACCESS_ALLOWED_ACE) + 
        GetLengthSid(pSid) - sizeof(DWORD);
    cbSacl = (cbSacl + (sizeof(DWORD) - 1)) & 0xFFFFFFFC;

    // Allocate the buffer of the ACL structure.
    pSacl = (PACL)LocalAlloc(LPTR, cbSacl);
    if (pSacl == NULL)
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Initialize the ACL structure.
    if (!InitializeAcl(pSacl, cbSacl, ACL_REVISION))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Add a mandatory ACE to an ACL.
    if (!AddMandatoryAce(pSacl, ACL_REVISION, 0, 
        SYSTEM_MANDATORY_LABEL_NO_WRITE_UP, pSid))
    {
        dwError = GetLastError();
        goto Cleanup;
    }
    
    // Set the SACL of the object identified by its name. The object can be 
    // a local or remote file or directory on an NTFS file system, network 
    // share, registry key, semaphore, event, mutex, file mapping, or 
    // waitable timer. The security descriptor of a named pipe cannot be set 
    // by using this function. You need to call the SetSecurityInfo function 
    // by openning the pipe handle.
    dwError = SetNamedSecurityInfo(pszObjectName, ObjectType, 
        LABEL_SECURITY_INFORMATION, NULL, NULL, NULL, pSacl);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pSid)
    {
        FreeSid(pSid);
        pSid = NULL;
    }
    if (pSacl)
    {
        LocalFree(pSacl);
        pSacl = NULL;
        cbSacl = 0;
    }

    if (ERROR_SUCCESS != dwError)
    {
        SetLastError(dwError);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


BOOL SetResourceIntegrityLevel(HANDLE hObject, 
                               SE_OBJECT_TYPE ObjectType, 
                               DWORD dwIntegrityLevel)
{
    DWORD dwError = ERROR_SUCCESS;
    PSID pSid = NULL;
    PACL pSacl = NULL;
    DWORD cbSacl = 0;
    
    // Allocate and initialize a SID of the integrity level.
    SID_IDENTIFIER_AUTHORITY MLAuthority = SECURITY_MANDATORY_LABEL_AUTHORITY;
    if (!AllocateAndInitializeSid(&MLAuthority, 1, dwIntegrityLevel, 
        0, 0, 0, 0, 0, 0, 0, &pSid))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Calculate the initial size of an ACL structure by adding the size of 
    // the ACL structure, the size of each ACE structure and the length of 
    // the SID that the ACL is to contain minus the SidStart member (DWORD) 
    // of the ACE, and aligning the size to the nearest DWORD. 
    cbSacl = sizeof(*pSacl) + sizeof(ACCESS_ALLOWED_ACE) + 
        GetLengthSid(pSid) - sizeof(DWORD);
    cbSacl = (cbSacl + (sizeof(DWORD) - 1)) & 0xFFFFFFFC;

    // Allocate the buffer of the ACL structure.
    pSacl = (PACL)LocalAlloc(LPTR, cbSacl);
    if (pSacl == NULL)
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Initialize the ACL structure.
    if (!InitializeAcl(pSacl, cbSacl, ACL_REVISION))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Add a mandatory ACE to an ACL.
    if (!AddMandatoryAce(pSacl, ACL_REVISION, 0, 
        SYSTEM_MANDATORY_LABEL_NO_WRITE_UP, pSid))
    {
        dwError = GetLastError();
        goto Cleanup;
    }
    
    // Set the SACL of the object identified by its handle. 
    dwError = SetSecurityInfo(hObject, ObjectType, 
        LABEL_SECURITY_INFORMATION, NULL, NULL, NULL, pSacl);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pSid)
    {
        FreeSid(pSid);
        pSid = NULL;
    }
    if (pSacl)
    {
        LocalFree(pSacl);
        pSacl = NULL;
        cbSacl = 0;
    }

    if (ERROR_SUCCESS != dwError)
    {
        SetLastError(dwError);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


BOOL SetNamedPipeIntegrityLevel(LPWSTR pszPipeName, DWORD dwIntegrityLevel)
{
    // Open the handle of the named pipe.
    return TRUE;
}

#pragma endregion


#pragma region GetResourceIntegrityLevel

//
//   FUNCTION: GetResourceIntegrityLevel(LPWSTR, SE_OBJECT_TYPE, PDWORD)
//
//   PURPOSE: 
//
//   PARAMETERS: 
//   * pszObjectName - A pointer to a null-terminated string that specifies 
//     the name of the object for which to set security information. This can 
//     be the name of a local or remote file or directory on an NTFS file 
//     system, network share, registry key, semaphore, event, mutex, file 
//     mapping, or waitable timer.
//   * ObjectType - A value of the SE_OBJECT_TYPE enumeration that indicates 
//     the type of object named by the pObjectName parameter.
//   * dwIntegrityLevel - 
//
//   RETURN VALUE: 
//
BOOL GetResourceIntegrityLevel(LPWSTR pszObjectName, 
                               SE_OBJECT_TYPE ObjectType, 
                               PDWORD pdwIntegrityLevel)
{
    BOOL fSucceeded = FALSE;

    return fSucceeded;
}

#pragma endregion


int wmain(int argc, wchar_t* argv[])
{
	return 0;
}