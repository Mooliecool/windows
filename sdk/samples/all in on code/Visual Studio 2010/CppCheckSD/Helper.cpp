/****************************** Module Header ******************************\
Module Name:  Helper.cpp
Project:      CppCheckSD
Copyright (c) Microsoft Corporation.

This module contains helper functions used by CppCheckSD.exe.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "CheckSD.h"
#include <stdio.h>
#include <strsafe.h>

//
//   FUNCTION: DisplayError(DWORD, LPWSTR)
//
//   PURPOSE: Display API and error code in text
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * dwError - Error Code to Display
//   * pszAPI  - API that failed
//
//   RETURN VALUE: none
//
void DisplayError(DWORD dwError, LPWSTR pszAPI)
{
     LPVOID lpvMessageBuffer;

     if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL, dwError,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
                   (LPTSTR)&lpvMessageBuffer, 0, NULL))
	 {
	      wprintf (L"FormatMessage failed: 0x%x\n", GetLastError());
		  return;
	 }

     // Now display this string.
     wprintf(L"ERROR: API        = %s.\n", pszAPI);
     wprintf(L"       error code = %d.\n", dwError);
     wprintf(L"       message    = %s\n", (LPTSTR)lpvMessageBuffer);

     
     // Free the buffer allocated by the system.
     if (LocalFree(lpvMessageBuffer))
		 printf ("LocalFree failed: 0x%x\n", GetLastError());

     ExitProcess(0);
}

//
//   FUNCTION: Privilege(LPWSTR, BOOL)
//
//   PURPOSE: enables or disables a privilege.  the function will first check 
//   to see if the thread is impersonating, if it is the privilege is enabled 
//   in the impersonation token if not, the privilege is enabled in the 
//   process token
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * pszPrivilege - Privilege being targeted
//   * bEnable      - TRUE enables the privilege while FALSE disables the privilege
//
//   RETURN VALUE: The function returns a boolean where TRUE indicates 
//   success while false indicates failure.
//

BOOL Privilege(LPTSTR pszPrivilege, BOOL bEnable)
{
	HANDLE           hToken;
    TOKEN_PRIVILEGES tp;

    
    // Obtain the token, first check the thread and then the process.
    if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, TRUE, &hToken))
	{
		if (GetLastError() == ERROR_NO_TOKEN)
		{
			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
				return FALSE;
		}
		else
			return FALSE;
	}

    // Get the luid for the privilege.
    if (!LookupPrivilegeValue(NULL, pszPrivilege, &tp.Privileges[0].Luid))
		return FALSE;

    tp.PrivilegeCount = 1;

    if (bEnable)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

    // Enable or disable the privilege.
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0))
		return FALSE;

	if (!CloseHandle(hToken))
		return FALSE;

    return TRUE;
}

//
//   FUNCTION: LookupAccountOtherSid(PSID, LPWSTR, LPDWORD, LPWSTR, LPDWORD, PSID_NAME_USE)
//
//   PURPOSE: there are certain builtin accounts on Windows NT which do not have a mapped 
//   account name. LookupAccountSid will return the error ERROR_NONE_MAPPED.  This function 
//   generates SIDs for the following accounts that are not mapped:
//    * ACCOUNT OPERATORS
//    * SYSTEM OPERATORS
//    * PRINTER OPERATORS
//    * BACKUP OPERATORS
//   the other SID it creates is a LOGON SID, it has a prefix of S-1-5-5.  a LOGON SID is a 
//   unique identifier for a user's logon session.
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * psidCheck - pointer to SID to display in textual form
//   * pszName   - name of the SID
//   * pcbName   - size of the name buffer in characters
//   * pszDomain - name of the DOMAIN for the SID
//   * pcbDomain - sizeof the DOMAIN buffer in characters
//   * psnu      - SID_NAME_USE for the SID
//
//   RETURN VALUE: none
//
void LookupAccountOtherSid(PSID psidCheck, LPWSTR pszName, LPDWORD pcbName, LPWSTR pszDomain, LPDWORD pcbDomain, PSID_NAME_USE psnu)
{
	HRESULT                  hr;
	int                      i;
    PSID                     psid[UNKNOWNSIDS];
    PSID                     psidLogonSid;
    SID_IDENTIFIER_AUTHORITY sia                     = SECURITY_NT_AUTHORITY;
    WCHAR                    szName[UNKNOWNSIDS][18] = {L"ACCOUNT OPERATORS", L"SERVER OPERATORS", L"PRINTER OPERATORS", L"BACKUP OPERATORS"};

    //
    // Name should be bigger than 18, builtin should be greater than 8.
    //


    // Create account operators.
    if (!AllocateAndInitializeSid(&sia, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ACCOUNT_OPS, 0, 0, 0, 0, 0, 0, &psid[0]))
		DisplayError(GetLastError(), L"AllocateAndInitializeSid");

    // Create system operators.
    if (!AllocateAndInitializeSid(&sia, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_SYSTEM_OPS, 0, 0, 0, 0, 0, 0, &psid[1]))
		DisplayError(GetLastError(), L"AllocateAndInitializeSid");

    // Create printer operators.
    if (!AllocateAndInitializeSid(&sia, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_PRINT_OPS, 0, 0, 0, 0, 0, 0, &psid[2]))
		DisplayError(GetLastError(), L"AllocateAndInitializeSid");

    // Create backup operators.
    if (!AllocateAndInitializeSid(&sia, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_BACKUP_OPS, 0, 0, 0, 0, 0, 0, &psid[3]))
		DisplayError(GetLastError(), L"AllocateAndInitializeSid");

	// Create a logon SID.
    if (!AllocateAndInitializeSid(&sia, 2, 0x00000005, 0, 0, 0, 0, 0, 0, 0, &psidLogonSid))
		DisplayError(GetLastError(), L"AllocateAndInitializeSid");

    *psnu =  SidTypeAlias;

    for (i = 0; i < 4; i++)
	{
		if (EqualSid(psidCheck, psid[i]))
		{
			hr = StringCbCopy(pszName, 18 * sizeof(TCHAR), szName[i]);
			if (FAILED(hr))
				DisplayError(hr, L"StringCbCopy");

			hr = StringCbCopy(pszDomain, *pcbDomain, L"BUILTIN");
			if (FAILED(hr))
				DisplayError(hr, L"StringCbCopy");

			break;
		}
	}

	if (EqualPrefixSid(psidCheck, psidLogonSid))
	{
		hr = StringCbCopy(pszName, *pcbName, L"LOGON SID");
		if (FAILED(hr))
			DisplayError(hr, L"StringCbCopy");
	}

    // Free the sids
    for (i=0;i<4;i++)
	{
		FreeSid(psid[i]);
	}

	FreeSid(psidLogonSid);
}

//
//   FUNCTION: Is64(void)
//
//   PURPOSE: Determines whether you are running on an x64 system.
//   CppCheckSD.exe
//
//   PARAMETERS: none
//
//   RETURN VALUE: The function returns a BOOL where TRUE indicates 
//   you are on an x64 system and FALSE means you are not.
//
BOOL Is64(void)
{
	SYSTEM_INFO si;

	GetNativeSystemInfo(&si);

	if (si.wProcessorArchitecture && PROCESSOR_ARCHITECTURE_INTEL)
		return FALSE;
	else
		return TRUE;
}