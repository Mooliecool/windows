/****************************** Module Header ******************************\
* Module Name:  CppRunAsUser.cpp
* Project:      CppRunAsUser
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

#pragma region Includes and Manifest Dependencies
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include "Resource.h"

#include <userenv.h>
#pragma comment(lib, "userenv.lib")

// Enable Visual Style
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#pragma endregion


#pragma region Helper Functions

//
//   FUNCTION: ReportError(LPWSTR, DWORD)
//
//   PURPOSE: Display an error dialog for the failure of a certain function.
//
//   PARAMETERS:
//   * pszFunction - the name of the function that failed.
//   * dwError - the Win32 error code. Its default value is the calling 
//   thread's last-error code value.
//
//   NOTE: The failing function must be immediately followed by the call of 
//   ReportError if you do not explicitly specify the dwError parameter of 
//   ReportError. This is to ensure that the calling thread's last-error code 
//   value is not overwritten by any calls of API between the failing 
//   function and ReportError.
//
void ReportError(LPWSTR pszFunction, DWORD dwError = GetLastError())
{
    wchar_t szMessage[200];
    int len = swprintf_s(szMessage, ARRAYSIZE(szMessage), 
        L"%s failed w/err 0x%08lx", pszFunction, dwError);
    MessageBox(NULL, (len != -1) ? szMessage : pszFunction, 
        L"An error occurred!", MB_ICONERROR);
}


//
//   FUNCTION: GetLogonSID(HANDLE, PSID *)
//
//   PURPOSE: Obtain the logon SID of the user.
//
//   PARAMETERS:
//   * hToken - 
//   * ppSid - 
//
BOOL GetLogonSID(HANDLE hToken, PSID *ppSid)
{
    BOOL fSucceeded = FALSE;
    DWORD cbTokenGroups = 0;
    PTOKEN_GROUPS pTokenGroups = NULL;
    DWORD dwIndex = 0;
    DWORD dwLength = 0;

    // Get required buffer size and allocate the TOKEN_GROUPS buffer.
    if (!GetTokenInformation(hToken, TokenGroups, NULL, 0, &cbTokenGroups))
    {
        if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
        {
            ReportError(L"GetTokenInformation");
            goto Exit;
        }

        pTokenGroups = (PTOKEN_GROUPS)LocalAlloc(LPTR, cbTokenGroups);
        if (pTokenGroups == NULL)
        {
            ReportError(L"LocalAlloc");
            goto Exit;
        }
    }

    // Get the token group information from the access token.
    if (!GetTokenInformation(hToken, TokenGroups, pTokenGroups, 
        cbTokenGroups, &cbTokenGroups))
    {
        ReportError(L"GetTokenInformation");
        goto Exit;
    }

    // Loop through the groups to find the logon SID.
    for (dwIndex = 0; dwIndex < pTokenGroups->GroupCount; dwIndex++)
    {
        if ((pTokenGroups->Groups[dwIndex].Attributes & SE_GROUP_LOGON_ID) 
            ==  SE_GROUP_LOGON_ID)
        {
            // Found the logon SID; make a copy of it.

            // Determine the length of the SID.
            dwLength = GetLengthSid(pTokenGroups->Groups[dwIndex].Sid);

            // Allocate a buffer for the logon SID.
            *ppSid = (PSID)LocalAlloc(LPTR, dwLength);
            if (*ppSid == NULL)
            {
                ReportError(L"LocalAlloc");
                goto Exit;
            }

            // Obtain a copy of the logon SID.
            if (!CopySid(dwLength, *ppSid, pTokenGroups->Groups[dwIndex].Sid))
            {
                ReportError(L"CopySid");
                goto Exit;
            }

            break;
        }
    }

    fSucceeded = TRUE;

Exit:

    // Centralized cleanup for all allocated resources. Clean up only those 
    // which were allocated, and clean them up in the right order.

    if (pTokenGroups)
    {
        if (LocalFree(pTokenGroups))
        {
            fSucceeded = FALSE;
            ReportError(L"LocalFree");
        }
        // Set variable to same state as resources
        pTokenGroups = NULL;
        cbTokenGroups = 0;
    }

    return fSucceeded;
}


//
//   FUNCTION: FreeLogonSID(PSID *)
//
//   PURPOSE: Free the logon SID of the user.
//
//   PARAMETERS:
//   * pSid - 
//
void FreeLogonSID(PSID *ppSid)
{
    if (LocalFree(*ppSid))
    {
        ReportError(L"LocalFree");
    }
}


#define GENERIC_ACCESS (GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | \
    GENERIC_ALL)

#define WINSTA_ALL (WINSTA_ACCESSCLIPBOARD  | WINSTA_ACCESSGLOBALATOMS | \
   WINSTA_CREATEDESKTOP    | WINSTA_ENUMDESKTOPS      | \
   WINSTA_ENUMERATE        | WINSTA_EXITWINDOWS       | \
   WINSTA_READATTRIBUTES   | WINSTA_READSCREEN        | \
   WINSTA_WRITEATTRIBUTES  | DELETE                   | \
   READ_CONTROL            | WRITE_DAC                | \
   WRITE_OWNER)

#define DESKTOP_ALL (DESKTOP_CREATEMENU      | DESKTOP_CREATEWINDOW  | \
   DESKTOP_ENUMERATE       | DESKTOP_HOOKCONTROL   | \
   DESKTOP_JOURNALPLAYBACK | DESKTOP_JOURNALRECORD | \
   DESKTOP_READOBJECTS     | DESKTOP_SWITCHDESKTOP | \
   DESKTOP_WRITEOBJECTS    | DELETE                | \
   READ_CONTROL            | WRITE_DAC             | \
   WRITE_OWNER)


BOOL AddAceToWindowStation(HWINSTA hWinsta, PSID pSid)
{
    BOOL fSucceeded = FALSE;
    SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;

    DWORD cbSd = 0;
    PSECURITY_DESCRIPTOR pSd = NULL;
    BOOL fDaclPresent = FALSE;
    BOOL fDaclExist = FALSE;
    ACL_SIZE_INFORMATION aclSizeInfo = { 0 };
    DWORD cbDacl = 0;
    PACL pDacl = NULL;

    PSECURITY_DESCRIPTOR pNewSd = NULL;
    DWORD cbNewDacl = 0;
    PACL pNewDacl = NULL;
    DWORD cbNewAce = 0;
    PACCESS_ALLOWED_ACE pNewAce = NULL;

    DWORD dwIndex = 0;
    PVOID pTempAce = NULL;

    // Get required buffer size and allocate the SECURITY_DESCRIPTOR buffer.
    if (!GetUserObjectSecurity(hWinsta, &si, NULL, 0, &cbSd))
    {
        if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
        {
            ReportError(L"GetUserObjectSecurity");
            goto Exit;
        }

        pSd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, cbSd);
        if (pSd == NULL)
        {
            ReportError(L"LocalAlloc");
            goto Exit;
        }
    }

    // Obtain the security descriptor for the window station.
    if (!GetUserObjectSecurity(hWinsta, &si, pSd, cbSd, &cbSd))
    {
        ReportError(L"GetUserObjectSecurity");
        goto Exit;
    }

    // Get the DACL from the security descriptor.
    if (!GetSecurityDescriptorDacl(pSd, &fDaclPresent, &pDacl, &fDaclExist))
    {
        ReportError(L"GetSecurityDescriptorDacl");
        goto Exit;
    }

    // Get the size information of the DACL.
    if (pDacl == NULL)
    {
        // If GetSecurityDescriptorDacl stores a NULL value in the pointer 
        // pointed to by pDacl, the security descriptor has a NULL DACL. A 
        // NULL DACL implicitly allows all access to an object.
        cbDacl = 0;
    }
    else
    {
        if (!GetAclInformation(pDacl, (LPVOID)&aclSizeInfo, 
            sizeof(aclSizeInfo), AclSizeInformation))
        {
            ReportError(L"GetAclInformation");
            goto Exit;
        }
        cbDacl = aclSizeInfo.AclBytesInUse;
    }

    // Allocate memory for the new security descriptor.
    pNewSd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, cbSd);
    if (pNewSd == NULL)
    {
        ReportError(L"LocalAlloc");
        goto Exit;
    }

    // Initialize the new security descriptor.
    if (!InitializeSecurityDescriptor(pNewSd, SECURITY_DESCRIPTOR_REVISION))
    {
        ReportError(L"InitializeSecurityDescriptor");
        goto Exit;
    }

    // Compute the size of a DACL to be added to the new security descriptor.
    cbNewAce = sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(pSid) - sizeof(DWORD);
    cbNewDacl = (cbDacl == 0 ? sizeof(ACL) : cbDacl) + 2 * cbNewAce;

    // Allocate memory for the new DACL.
    pNewDacl = (PACL)LocalAlloc(LPTR, cbNewDacl);
    if (pNewDacl == NULL)
    {
        ReportError(L"LocalAlloc");
        goto Exit;
    }

    // Initialize the new DACL.
    if (!InitializeAcl(pNewDacl, cbNewDacl, ACL_REVISION))
    {
        ReportError(L"InitializeAcl");
        goto Exit;
    }

    // If the original DACL is present, copy it to the new DACL.
    if (fDaclPresent)
    {
        // Copy the ACEs to the new DACL.
        if (aclSizeInfo.AceCount)
        {
            for (dwIndex = 0; dwIndex < aclSizeInfo.AceCount; dwIndex++)
            {
                // Get an ACE.
                if (!GetAce(pDacl, dwIndex, &pTempAce))
                {
                    ReportError(L"GetAce");
                    goto Exit;
                }

                // Add the ACE to the new ACL.
                if (!AddAce(pNewDacl, ACL_REVISION, MAXDWORD, pTempAce, 
                    ((PACE_HEADER)pTempAce)->AceSize))
                {
                    ReportError(L"AddAce");
                    goto Exit;
                }
            }
        }
    }

    // Add two new ACEs to the new DACL to allow the SID full access to the 
    // interactive window station.

    pNewAce = (PACCESS_ALLOWED_ACE)LocalAlloc(LPTR, cbNewAce);
    if (pNewAce == NULL)
    {
        ReportError(L"LocalAlloc");
        goto Exit;
    }

    pNewAce->Header.AceType = ACCESS_ALLOWED_ACE_TYPE;
    pNewAce->Header.AceFlags = CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE | 
        OBJECT_INHERIT_ACE;
    pNewAce->Header.AceSize = (WORD)cbNewAce;
    pNewAce->Mask = GENERIC_ACCESS;
    if (!CopySid(GetLengthSid(pSid), &pNewAce->SidStart, pSid))
    {
        ReportError(L"CopySid");
        goto Exit;
    }

    if (!AddAce(pNewDacl, ACL_REVISION, MAXDWORD, (LPVOID)pNewAce, 
        pNewAce->Header.AceSize))
    {
        ReportError(L"AddAce");
        goto Exit;
    }

    pNewAce->Header.AceFlags = NO_PROPAGATE_INHERIT_ACE;
    pNewAce->Mask = WINSTA_ALL;

    if (!AddAce(pNewDacl, ACL_REVISION, MAXDWORD, (LPVOID)pNewAce, 
        pNewAce->Header.AceSize))
    {
        ReportError(L"AddAce");
        goto Exit;
    }

    // Assign the new DACL to the new security descriptor.
    if (!SetSecurityDescriptorDacl(pNewSd, TRUE, pNewDacl, FALSE))
    {
        ReportError(L"SetSecurityDescriptorDacl");
        goto Exit;
    }

    // Set the new security descriptor for the window station.
    if (!SetUserObjectSecurity(hWinsta, &si, pNewSd))
    {
        ReportError(L"SetUserObjectSecurity");
        goto Exit;
    }

    fSucceeded = TRUE;

Exit:

    // Centralized cleanup for all allocated resources. Clean up only those 
    // which were allocated, and clean them up in the right order.

    if (pSd)
    {
        if (LocalFree(pSd))
        {
            fSucceeded = FALSE;
            ReportError(L"LocalFree");
        }
        pSd = NULL;
        cbSd = 0;
    }

    if (pNewSd)
    {
        if (LocalFree(pNewSd))
        {
            fSucceeded = FALSE;
            ReportError(L"LocalFree");
        }
        pNewSd = NULL;
    }

    if (pNewDacl)
    {
        if (LocalFree(pNewDacl))
        {
            fSucceeded = FALSE;
            ReportError(L"LocalFree");
        }
        pNewDacl = NULL;
        cbNewDacl = 0;
    }

    if (pNewAce)
    {
        if (LocalFree(pNewAce))
        {
            fSucceeded = FALSE;
            ReportError(L"LocalFree");
        }
        pNewAce = NULL;
        cbNewAce = 0;
    }

    return fSucceeded;
}


BOOL AddAceToDesktop(HDESK hDesktop, PSID pSid)
{
    BOOL fSucceeded = FALSE;
    SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;

    DWORD cbSd = 0;
    PSECURITY_DESCRIPTOR pSd = NULL;
    BOOL fDaclPresent = FALSE;
    BOOL fDaclExist = FALSE;
    ACL_SIZE_INFORMATION aclSizeInfo = { 0 };
    DWORD cbDacl = 0;
    PACL pDacl = NULL;

    PSECURITY_DESCRIPTOR pNewSd = NULL;
    DWORD cbNewDacl = 0;
    PACL pNewDacl = NULL;
    DWORD cbNewAce = 0;
    PACCESS_ALLOWED_ACE pNewAce = NULL;

    DWORD dwIndex = 0;
    PVOID pTempAce = NULL;

    // Get required buffer size and allocate the SECURITY_DESCRIPTOR buffer.
    if (!GetUserObjectSecurity(hDesktop, &si, NULL, 0, &cbSd))
    {
        if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
        {
            ReportError(L"GetUserObjectSecurity");
            goto Exit;
        }

        pSd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, cbSd);
        if (pSd == NULL)
        {
            ReportError(L"LocalAlloc");
            goto Exit;
        }
    }

    // Obtain the security descriptor for the desktop object.
    if (!GetUserObjectSecurity(hDesktop, &si, pSd, cbSd, &cbSd))
    {
        ReportError(L"GetUserObjectSecurity");
        goto Exit;
    }

    // Get the DACL from the security descriptor.
    if (!GetSecurityDescriptorDacl(pSd, &fDaclPresent, &pDacl, &fDaclExist))
    {
        ReportError(L"GetSecurityDescriptorDacl");
        goto Exit;
    }

    // Get the size information of the DACL.
    if (pDacl == NULL)
    {
        cbDacl = 0;
    }
    else
    {
        if (!GetAclInformation(pDacl, (LPVOID)&aclSizeInfo, 
            sizeof(aclSizeInfo), AclSizeInformation))
        {
            ReportError(L"GetAclInformation");
            goto Exit;
        }
        cbDacl = aclSizeInfo.AclBytesInUse;
    }

    // Allocate memory for the new security descriptor.
    pNewSd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, cbSd);
    if (pNewSd == NULL)
    {
        ReportError(L"LocalAlloc");
        goto Exit;
    }

    // Initialize the new security descriptor.
    if (!InitializeSecurityDescriptor(pNewSd, SECURITY_DESCRIPTOR_REVISION))
    {
        ReportError(L"InitializeSecurityDescriptor");
        goto Exit;
    }

    // Compute the size of a DACL to be added to the new security descriptor.
    cbNewAce = sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(pSid) - sizeof(DWORD);
    cbNewDacl = (cbDacl == 0 ? sizeof(ACL) : cbDacl) + cbNewAce;

    // Allocate memory for the new DACL.
    pNewDacl = (PACL)LocalAlloc(LPTR, cbNewDacl);
    if (pNewDacl == NULL)
    {
        ReportError(L"LocalAlloc");
        goto Exit;
    }

    // Initialize the new DACL.
    if (!InitializeAcl(pNewDacl, cbNewDacl, ACL_REVISION))
    {
        ReportError(L"InitializeAcl");
        goto Exit;
    }

    // If the original DACL is present, copy it to the new DACL.
    if (fDaclPresent)
    {
        // Copy the ACEs to the new DACL.
        if (aclSizeInfo.AceCount)
        {
            for (dwIndex = 0; dwIndex < aclSizeInfo.AceCount; dwIndex++)
            {
                // Get an ACE.
                if (!GetAce(pDacl, dwIndex, &pTempAce))
                {
                    ReportError(L"GetAce");
                    goto Exit;
                }

                // Add the ACE to the new ACL.
                if (!AddAce(pNewDacl, ACL_REVISION, MAXDWORD, pTempAce, 
                    ((PACE_HEADER)pTempAce)->AceSize))
                {
                    ReportError(L"AddAce");
                    goto Exit;
                }
            }
        }
    }

    // Add a new ACE to the new DACL.
    if (!AddAccessAllowedAce(pNewDacl, ACL_REVISION, DESKTOP_ALL, pSid))
    {
        ReportError(L"AddAccessAllowedAce");
        goto Exit;
    }

    // Assign the new DACL to the new security descriptor.
    if (!SetSecurityDescriptorDacl(pNewSd, TRUE, pNewDacl, FALSE))
    {
        ReportError(L"SetSecurityDescriptorDacl");
        goto Exit;
    }

    //  Set the new security descriptor for the desktop object.
    if (!SetUserObjectSecurity(hDesktop, &si, pNewSd))
    {
        ReportError(L"SetUserObjectSecurity");
        goto Exit;
    }

    fSucceeded = TRUE;

Exit:

    // Centralized cleanup for all allocated resources. Clean up only those 
    // which were allocated, and clean them up in the right order.

    if (pSd)
    {
        if (LocalFree(pSd))
        {
            fSucceeded = FALSE;
            ReportError(L"LocalFree");
        }
        pSd = NULL;
        cbSd = 0;
    }

    if (pNewSd)
    {
        if (LocalFree(pNewSd))
        {
            fSucceeded = FALSE;
            ReportError(L"LocalFree");
        }
        pNewSd = NULL;
    }

    if (pNewDacl)
    {
        if (LocalFree(pNewDacl))
        {
            fSucceeded = FALSE;
            ReportError(L"LocalFree");
        }
        pNewDacl = NULL;
        cbNewDacl = 0;
    }

    if (pNewAce)
    {
        if (LocalFree(pNewAce))
        {
            fSucceeded = FALSE;
            ReportError(L"LocalFree");
        }
        pNewAce = NULL;
        cbNewAce = 0;
    }

    return fSucceeded;
}

#pragma endregion


#pragma region CreateInteractiveProcessWithLogon

//
//   FUNCTION: CreateInteractiveProcessWithLogon(LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR)
//
//   PURPOSE: 
//
//   PARAMETERS:
//   * pszUserName - A pointer to a null-terminated string that specifies the 
//   name of the user. This is the name of the user account to log on to. If 
//   you use the user principal name (UPN) format, User@DNSDomainName, the 
//   pszDomain parameter must be NULL. 
//   * pszDomain - A pointer to a null-terminated string that specifies the 
//   name of the domain or server whose account database contains the 
//   pszUsername account. If this parameter is NULL, the user name must be 
//   specified in UPN format. If this parameter is ".", the function validates 
//   the account by using only the local account database.
//   * pszPassword - A pointer to a null-terminated string that specifies the 
//   plaintext password for the user account specified by lpszUsername. When 
//   you have finished using the password, clear the password from memory by 
//   calling the SecureZeroMemory function. 
//   * pszCommandLine - The command line to be executed. The maximum length 
//   of this string is 1024 characters. The module name portion of 
//   pszCommandLine is limited to MAX_PATH characters.
//
//   NOTE: 
//
BOOL CreateInteractiveProcessWithLogon(LPCWSTR pszUserName, 
                                       LPCWSTR pszDomain, 
                                       LPCWSTR pszPassword, 
                                       LPWSTR pszCommandLine)
{
    BOOL fSucceeded = FALSE;
    HANDLE hToken = NULL;
    LPVOID lpvEnv = NULL;
    wchar_t szUserProfile[MAX_PATH];
    DWORD dwSize = 0;
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    // Attempt to log the user on to the local computer. The logon type is 
    // intended for users who will be interactively using the computer.
    if (!LogonUser(pszUserName, pszDomain, pszPassword, 
        LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hToken))
    {
        ReportError(L"LogonUser");
        goto Exit;
    }

    // Retrieve the environment variables for the specified user. The 
    // environment block inherits from the current process's environment.
    if (!CreateEnvironmentBlock(&lpvEnv, hToken, TRUE))
    {
        ReportError(L"CreateEnvironmentBlock");
        goto Exit;
    }

    // Retrieve the path to the root directory of the user's profile.
    dwSize = ARRAYSIZE(szUserProfile);
    if (!GetUserProfileDirectory(hToken, szUserProfile, &dwSize))
    {
        ReportError(L"GetUserProfileDirectory");
        goto Exit;
    }

    // If the lpDesktop member of the STARTUPINFO structure is initialized 
    // to either NULL or "", CreateProcessWithLogonW implementation adds 
    // permissions for the specified user account to the window station and 
    // desktop inherited from the parent process, which is "winsta0\default" 
    // for the "Default" application desktop.
    si.lpDesktop = NULL;

    // Launch the interative child process.
    if (!CreateProcessWithLogonW(pszUserName, pszDomain, pszPassword, 
        LOGON_WITH_PROFILE, NULL, pszCommandLine, 
        CREATE_UNICODE_ENVIRONMENT, lpvEnv, szUserProfile, &si, &pi))
    {
        ReportError(L"CreateProcessWithLogonW");
        goto Exit;
    }

    // The child process is running now ...

    // (Optional) Wait for the exit of the child process.
    //WaitForSingleObject(pi.hProcess);

    fSucceeded = TRUE;

Exit:

    // Centralized cleanup for all allocated resources. Clean up only those 
    // which were allocated, and clean them up in the right order.

    if (hToken)
    {
        if (!CloseHandle(hToken))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseHandle");
        }
        hToken = NULL;
    }

    if (lpvEnv)
    {
        if (!DestroyEnvironmentBlock(lpvEnv))
        {
            fSucceeded = FALSE;
            ReportError(L"DestroyEnvironmentBlock");
        }
        lpvEnv = NULL;
    }

    if (pi.hProcess)
    {
        if (!CloseHandle(pi.hProcess))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseHandle");
        }
        pi.hProcess = NULL;
    }

    if (pi.hThread)
    {
        if (!CloseHandle(pi.hThread))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseHandle");
        }
        pi.hThread = NULL;
    }

    return fSucceeded;
}

#pragma endregion


#pragma region CreateInteractiveProcessWithToken

//
//   FUNCTION: CreateInteractiveProcessWithToken(LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR)
//
//   PURPOSE: 
//
//   PARAMETERS:
//   * pszUserName - A pointer to a null-terminated string that specifies the 
//   name of the user. This is the name of the user account to log on to. If 
//   you use the user principal name (UPN) format, User@DNSDomainName, the 
//   pszDomain parameter must be NULL. 
//   * pszDomain - A pointer to a null-terminated string that specifies the 
//   name of the domain or server whose account database contains the 
//   pszUsername account. If this parameter is NULL, the user name must be 
//   specified in UPN format. If this parameter is ".", the function validates 
//   the account by using only the local account database.
//   * pszPassword - A pointer to a null-terminated string that specifies the 
//   plaintext password for the user account specified by lpszUsername. When 
//   you have finished using the password, clear the password from memory by 
//   calling the SecureZeroMemory function. 
//   * pszCommandLine - The command line to be executed. The maximum length 
//   of this string is 1024 characters. The module name portion of 
//   pszCommandLine is limited to MAX_PATH characters.
//
//   NOTE: 
//
BOOL CreateInteractiveProcessWithToken(LPCWSTR pszUserName, 
                                       LPCWSTR pszDomain, 
                                       LPCWSTR pszPassword, 
                                       LPWSTR pszCommandLine)
{
    BOOL fSucceeded = FALSE;
    HANDLE hToken = NULL;
    HWINSTA hWinstaSave = NULL;
    HWINSTA hWinsta = NULL;
    HDESK hDesktop = NULL;
    PSID pSid = NULL;
    wchar_t szUserProfile[MAX_PATH];
    DWORD dwSize = ARRAYSIZE(szUserProfile);
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    // Attempt to log the user on to the local computer. The logon type is 
    // intended for users who will be interactively using the computer.
    if (!LogonUser(pszUserName, pszDomain, pszPassword, 
        LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hToken))
    {
        ReportError(L"LogonUser");
        goto Exit;
    }

    // Save a handle to the caller's current window station.
    hWinstaSave = GetProcessWindowStation();
    if (hWinstaSave == NULL)
    {
        ReportError(L"GetProcessWindowStation");
        goto Exit;
    }

    // Get a handle to the interactive window station.
    hWinsta = OpenWindowStation(
        L"winsta0",                  // The interactive window station 
        FALSE,                       // Handle is not inheritable
        READ_CONTROL | WRITE_DAC);   // Rights to read/write the DACL
    if (hWinsta == NULL)
    {
        ReportError(L"OpenWindowStation");
        goto Exit;
    }

    // To get the correct default desktop, set the caller's window station 
    // to the interactive window station.
    if (!SetProcessWindowStation(hWinsta))
    {
        ReportError(L"SetProcessWindowStation");
        goto Exit;
    }

    // Get a handle to the interactive desktop.
    hDesktop = OpenDesktop(
        L"default",            // The interactive window station 
        0,                     // No interaction with other desktop processes
        FALSE,                 // Handle is not inheritable
        READ_CONTROL | WRITE_DAC | DESKTOP_WRITEOBJECTS | 
        DESKTOP_READOBJECTS);  // Rights to read/write the DACL

    // Restore the caller's window station.
    if (!SetProcessWindowStation(hWinstaSave))
    {
        ReportError(L"SetProcessWindowStation");
        goto Exit;
    }

    if (hDesktop == NULL)
    {
        ReportError(L"OpenDesktop");
        goto Exit;
    }

    // Get the SID for the client's logon session.
    if (!GetLogonSID(hToken, &pSid)) 
    {
        goto Exit;
    }

    // Allow logon SID full access to interactive window station.
    if (!AddAceToWindowStation(hWinsta, pSid))
    {
        goto Exit;
    }

    // Allow logon SID full access to interactive desktop.
    if (!AddAceToDesktop(hDesktop, pSid))
    {
        goto Exit;
    }

    // Retrieve the path to the root directory of the user's profile.
    if (!GetUserProfileDirectory(hToken, szUserProfile, &dwSize))
    {
        ReportError(L"GetUserProfileDirectory");
        goto Exit;
    }

    // Specify that the child process runs in the interactive desktop.
    si.lpDesktop = L"winsta0\\default";

    // Launch the process in the client's logon session. 
    if (!CreateProcessWithTokenW(hToken, LOGON_WITH_PROFILE, NULL, 
        pszCommandLine, CREATE_UNICODE_ENVIRONMENT, NULL, szUserProfile, 
        &si, &pi))
    {
        ReportError(L"CreateProcessWithTokenW");
        goto Exit;
    }

    // The child process is running now ...

    // (Optional) Wait for the exit of the child process.
    //WaitForSingleObject(pi.hProcess);

    fSucceeded = TRUE;

Exit:

    // Centralized cleanup for all allocated resources. Clean up only those 
    // which were allocated, and clean them up in the right order.

    if (hToken)
    {
        if (!CloseHandle(hToken))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseHandle");
        }
        hToken = NULL;
    }

    if (hWinsta)
    {
        if (!CloseWindowStation(hWinsta))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseWindowStation");
        }
        hWinsta = NULL;
    }

    if (hDesktop)
    {
        if (!CloseDesktop(hDesktop))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseDesktop");
        }
        hDesktop = NULL;
    }

    if (pSid)
    {
        FreeLogonSID(&pSid);
        pSid = NULL;
    }

    if (pi.hProcess)
    {
        if (!CloseHandle(pi.hProcess))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseHandle");
        }
        pi.hProcess = NULL;
    }

    if (pi.hThread)
    {
        if (!CloseHandle(pi.hThread))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseHandle");
        }
        pi.hThread = NULL;
    }

    return fSucceeded;
}

#pragma endregion


#pragma region CreateInteractiveProcessAsUser


//
//   FUNCTION: CreateInteractiveProcessAsUser(LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR)
//
//   PURPOSE: 
//
//   PARAMETERS:
//   * pszUserName - A pointer to a null-terminated string that specifies the 
//   name of the user. This is the name of the user account to log on to. If 
//   you use the user principal name (UPN) format, User@DNSDomainName, the 
//   pszDomain parameter must be NULL. 
//   * pszDomain - A pointer to a null-terminated string that specifies the 
//   name of the domain or server whose account database contains the 
//   pszUsername account. If this parameter is NULL, the user name must be 
//   specified in UPN format. If this parameter is ".", the function validates 
//   the account by using only the local account database.
//   * pszPassword - A pointer to a null-terminated string that specifies the 
//   plaintext password for the user account specified by lpszUsername. When 
//   you have finished using the password, clear the password from memory by 
//   calling the SecureZeroMemory function. 
//   * pszCommandLine - The command line to be executed. The maximum length 
//   of this string is 1024 characters. The module name portion of 
//   pszCommandLine is limited to MAX_PATH characters.
//
//   NOTE: 
//
BOOL CreateInteractiveProcessAsUser(LPCWSTR pszUserName, 
                                    LPCWSTR pszDomain, 
                                    LPCWSTR pszPassword, 
                                    LPWSTR pszCommandLine)
{
    BOOL fSucceeded = FALSE;
    HANDLE hToken = NULL;
    HWINSTA hWinstaSave = NULL;
    HWINSTA hWinsta = NULL;
    HDESK hDesktop = NULL;
    PSID pSid = NULL;
    wchar_t szUserProfile[MAX_PATH];
    DWORD dwSize = ARRAYSIZE(szUserProfile);
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    // Attempt to log the user on to the local computer. The logon type is 
    // intended for users who will be interactively using the computer.
    if (!LogonUser(pszUserName, pszDomain, pszPassword, 
        LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hToken))
    {
        ReportError(L"LogonUser");
        goto Exit;
    }

    // Save a handle to the caller's current window station.
    hWinstaSave = GetProcessWindowStation();
    if (hWinstaSave == NULL)
    {
        ReportError(L"GetProcessWindowStation");
        goto Exit;
    }

    // Get a handle to the interactive window station.
    hWinsta = OpenWindowStation(
        L"winsta0",                  // The interactive window station 
        FALSE,                       // Handle is not inheritable
        READ_CONTROL | WRITE_DAC);   // Rights to read/write the DACL
    if (hWinsta == NULL)
    {
        ReportError(L"OpenWindowStation");
        goto Exit;
    }

    // To get the correct default desktop, set the caller's window station 
    // to the interactive window station.
    if (!SetProcessWindowStation(hWinsta))
    {
        ReportError(L"SetProcessWindowStation");
        goto Exit;
    }

    // Get a handle to the interactive desktop.
    hDesktop = OpenDesktop(
        L"default",            // The interactive window station 
        0,                     // No interaction with other desktop processes
        FALSE,                 // Handle is not inheritable
        READ_CONTROL | WRITE_DAC | DESKTOP_WRITEOBJECTS | 
        DESKTOP_READOBJECTS);  // Rights to read/write the DACL

    // Restore the caller's window station.
    if (!SetProcessWindowStation(hWinstaSave))
    {
        ReportError(L"SetProcessWindowStation");
        goto Exit;
    }

    if (hDesktop == NULL)
    {
        ReportError(L"OpenDesktop");
        goto Exit;
    }

    // Get the SID for the client's logon session.
    if (!GetLogonSID(hToken, &pSid)) 
    {
        goto Exit;
    }

    // Allow logon SID full access to interactive window station.
    if (!AddAceToWindowStation(hWinsta, pSid))
    {
        goto Exit;
    }

    // Allow logon SID full access to interactive desktop.
    if (!AddAceToDesktop(hDesktop, pSid))
    {
        goto Exit;
    }

    // Retrieve the path to the root directory of the user's profile.
    if (!GetUserProfileDirectory(hToken, szUserProfile, &dwSize))
    {
        ReportError(L"GetUserProfileDirectory");
        goto Exit;
    }

    // Specify that the child process runs in the interactive desktop.
    si.lpDesktop = L"winsta0\\default";

    // Launch the process in the client's logon session. The process that 
    // calls the CreateProcessAsUser function must have the 
    // SE_ASSIGNPRIMARYTOKEN_NAME and SE_INCREASE_QUOTA_NAME privileges. 
    // Generally only processes run as the SYSTEM account has both 
    // privileges. The function fails with the ERROR_PRIVILEGE_NOT_HELD 
    // (0x522) error when the required privileges are not available.
    if (!CreateProcessAsUser(hToken, NULL, pszCommandLine, NULL, NULL, FALSE, 
        CREATE_UNICODE_ENVIRONMENT, NULL, szUserProfile, &si, &pi))
    {
        ReportError(L"CreateProcessAsUser");
        goto Exit;
    }

    // The child process is running now ...

    // (Optional) Wait for the exit of the child process.
    //WaitForSingleObject(pi.hProcess);

    fSucceeded = TRUE;

Exit:

    // Centralized cleanup for all allocated resources. Clean up only those 
    // which were allocated, and clean them up in the right order.

    if (hToken)
    {
        if (!CloseHandle(hToken))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseHandle");
        }
        hToken = NULL;
    }

    if (hWinsta)
    {
        if (!CloseWindowStation(hWinsta))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseWindowStation");
        }
        hWinsta = NULL;
    }

    if (hDesktop)
    {
        if (!CloseDesktop(hDesktop))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseDesktop");
        }
        hDesktop = NULL;
    }

    if (pSid)
    {
        FreeLogonSID(&pSid);
        pSid = NULL;
    }

    if (pi.hProcess)
    {
        if (!CloseHandle(pi.hProcess))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseHandle");
        }
        pi.hProcess = NULL;
    }

    if (pi.hThread)
    {
        if (!CloseHandle(pi.hThread))
        {
            fSucceeded = FALSE;
            ReportError(L"CloseHandle");
        }
        pi.hThread = NULL;
    }

    return fSucceeded;
}

#pragma endregion


void OnCreateProcess(HWND hWnd, int id)
{
    int cchText;
    DWORD cbUserName = 0;
    PWSTR pszUserName = NULL;
    DWORD cbDomain = 0;
    PWSTR pszDomain = NULL;
    DWORD cbPassword = 0;
    PWSTR pszPassword = NULL;
    DWORD cbCommand = 0;
    PWSTR pszCommand = NULL;

    // Get the user name from the dialog.
    HWND hUserNameEdit = GetDlgItem(hWnd, IDC_USERNAME_EDIT);
    cchText = GetWindowTextLength(hUserNameEdit);
    if (cchText == 0)
    {
        MessageBox(hWnd, L"User Name cannot be empty", L"Error", MB_ICONERROR);
        goto Exit;
    }
    cbUserName = (cchText + 1) * sizeof(*pszUserName);
    pszUserName = (PWSTR)LocalAlloc(LPTR, cbUserName);
    GetWindowText(hUserNameEdit, pszUserName, cchText + 1);

    // Get the domain from the dialog.
    HWND hDomainEdit = GetDlgItem(hWnd, IDC_DOMAIN_EDIT);
    cchText = GetWindowTextLength(hDomainEdit);
    if (cchText != 0)
    {
        cbDomain = (cchText + 1) * sizeof(*pszDomain);
        pszDomain = (PWSTR)LocalAlloc(LPTR, cbDomain);
        GetWindowText(hDomainEdit, pszDomain, cchText + 1);
    }

    // Get the password from the dialog.
    HWND hPasswordEdit = GetDlgItem(hWnd, IDC_PASSWORD_EDIT);
    cchText = GetWindowTextLength(hPasswordEdit);
    if (cchText != 0)
    {
        cbPassword = (cchText + 1) * sizeof(*pszPassword);
        pszPassword = (PWSTR)LocalAlloc(LPTR, cbPassword);
        GetWindowText(hPasswordEdit, pszPassword, cchText + 1);
    }

    // Get the command from the dialog.
    HWND hCommandEdit = GetDlgItem(hWnd, IDC_COMMAND_EDIT);
    cchText = GetWindowTextLength(hCommandEdit);
    if (cchText == 0)
    {
        MessageBox(hWnd, L"Command cannot be empty", L"Error", MB_ICONERROR);
        goto Exit;
    }
    cbCommand = (cchText + 1) * sizeof(*pszCommand);
    pszCommand = (PWSTR)LocalAlloc(LPTR, cbCommand);
    GetWindowText(hCommandEdit, pszCommand, cchText + 1);

    // Create the child process.

    switch (id)
    {
    case IDC_CREATEPROCESSWITHLOGON_BN:
        if (!CreateInteractiveProcessWithLogon(pszUserName, pszDomain, 
            pszPassword, pszCommand))
        {
            ReportError(L"CreateInteractiveProcessWithLogon");
        }
        break;

    case IDC_CREATEPROCESSWITHTOKEN_BN:
        if (!CreateInteractiveProcessWithToken(pszUserName, pszDomain, 
            pszPassword, pszCommand))
        {
            ReportError(L"CreateInteractiveProcessWithToken");
        }
        break;

    case IDC_CREATEPROCESSASUSER_BN:
        if (!CreateInteractiveProcessAsUser(pszUserName, pszDomain, 
            pszPassword, pszCommand))
        {
            ReportError(L"CreateInteractiveProcessAsUser");
        }
        break;
    }

Exit:

    if (pszUserName)
    {
        if (LocalFree(pszUserName))
        {
            ReportError(L"LocalFree");
        }
        pszUserName = NULL;
        cbUserName = 0;
    }

    if (pszDomain)
    {
        if (LocalFree(pszDomain))
        {
            ReportError(L"LocalFree");
        }
        pszDomain = NULL;
        cbDomain = 0;
    }

    if (pszPassword)
    {
        SecureZeroMemory(pszPassword, cbPassword);
        if (LocalFree(pszPassword))
        {
            ReportError(L"LocalFree");
        }
        pszPassword = NULL;
        cbPassword = 0;
    }

    if (pszCommand)
    {
        if (LocalFree(pszCommand))
        {
            ReportError(L"LocalFree");
        }
        pszCommand = NULL;
        cbCommand = 0;
    }
}


// 
//   FUNCTION: OnInitDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message. 
//
BOOL OnInitDialog(HWND hWnd, HWND hwndFocus, LPARAM lParam)
{
    return TRUE;
}


//
//   FUNCTION: OnCommand(HWND, int, HWND, UINT)
//
//   PURPOSE: Process the WM_COMMAND message.
//
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDC_CREATEPROCESSWITHLOGON_BN:
    case IDC_CREATEPROCESSWITHTOKEN_BN:
    case IDC_CREATEPROCESSASUSER_BN:
        OnCreateProcess(hWnd, id);
        break;
    case IDOK:
    case IDCANCEL:
        EndDialog(hWnd, 0);
        break;
    }
}


//
//   FUNCTION: OnClose(HWND)
//
//   PURPOSE: Process the WM_CLOSE message.
//
void OnClose(HWND hWnd)
{
    EndDialog(hWnd, 0);
}


//
//  FUNCTION: DialogProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main dialog.
//
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitDialog);

        // Handle the WM_COMMAND message in OnCommand
        HANDLE_MSG (hWnd, WM_COMMAND, OnCommand);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;
    }
    return 0;
}


//
//  FUNCTION: wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
//
//  PURPOSE:  The entry point of the application.
//
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR    lpCmdLine,
                      int       nCmdShow)
{
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
}