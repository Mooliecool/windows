/****************************** Module Header ******************************\
Module Name:  SecObjects.cpp
Project:      CppCheckSD
Copyright (c) Microsoft Corporation

This module contains the variety of functions to obtain the security
descriptor for a variety of securable objects.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "CheckSD.h"
#include <lm.h>
#include <stdio.h>
#include <AclAPI.h>
#include <strsafe.h>

//
//   FUNCTION: DumpObjectWithHandle(LPWSTR, WCHAR, BOOL, BOOL)
//
//   PURPOSE: Obtain a handle to the kernel object.
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * pszObject - name of the kernel object
//   * c         - type of kernel object
//   * bVerbose  - TRUE displays all information on SD and FALSE returns in SDDL format
//   * bAudit    - TRUE indicates SD includes Audit Aces
//
//   RETURN VALUE: none.
//
void DumpObjectWithHandle(LPWSTR pszObject, WCHAR c, BOOL bVerbose, BOOL bAudit)
{
	DWORD                dwError;
	DWORD                dwPid;
	DWORD                dwTid;
	DWORD                dwDesiredAccess = READ_CONTROL;
    HANDLE               hObject;
	HANDLE               hProcess;
	HRESULT              hr;
	HWINSTA              hwinsta;
	HWINSTA              hwinstaCurrent;
	LPWSTR               pszDesktop     = NULL;
	PACL                 pDacl          = NULL;
	PACL                 pSacl          = NULL;
    PSECURITY_DESCRIPTOR psd            = NULL;
	PSID                 pOwner         = NULL;
	PSID                 pGroup         = NULL;
    SECURITY_INFORMATION si             = OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | LABEL_SECURITY_INFORMATION;
	SE_OBJECT_TYPE       sot            = SE_KERNEL_OBJECT;
	WCHAR                szWinsta[4096] = L""; // How long can a windowstation name be?

    // Enable the privilege if you need audit ACE.
	if (bAudit)
	{
		dwDesiredAccess = dwDesiredAccess | ACCESS_SYSTEM_SECURITY;
        si              = si              | SACL_SECURITY_INFORMATION;
		Privilege(SE_SECURITY_NAME, TRUE);
	}

	switch (c)
	{
		case 'a':  // mail slot
		case 'n':  // named pipe
			// No audit aces for named pipe.
			hObject = CreateFile(pszObject, READ_CONTROL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if (hObject == INVALID_HANDLE_VALUE)
				DisplayError(GetLastError(), L"CreateFile");
			break;
		case 'c': // scm
			sot = SE_SERVICE;
			hObject = (HANDLE)OpenSCManager(pszObject, NULL, dwDesiredAccess);
			if (hObject == NULL)
				DisplayError(GetLastError(), L"OpenSCManager");
			break;
		case 'p': // process
            // Convert name to a pid.
			dwPid = _wtol(pszObject);

			hObject = OpenProcess(dwDesiredAccess, FALSE, dwPid);
			if (hObject == NULL)
			{
				if (GetLastError() == 1314)
				{
                    Privilege(SE_DEBUG_NAME, TRUE);

					hObject = OpenProcess(dwDesiredAccess, FALSE, dwPid);
					if (hObject == NULL)
						DisplayError(GetLastError(), L"OpenProcess");

                    Privilege(SE_DEBUG_NAME, FALSE);
				}
				else
					DisplayError(GetLastError(), L"OpenProcess");
			}
			break;
		case 'h': // thread
			// Convert name to a pid.
			dwTid = _wtol(pszObject);

			hObject = OpenThread(dwDesiredAccess, FALSE, dwTid);
			if (hObject == NULL)
				DisplayError(GetLastError(), L"OpenThread");
			break;
		case 'o': // process token
			dwPid = _wtol(pszObject);

            // SD in access token does not support SACLS.
			hProcess= OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
			if (hProcess == NULL)
				DisplayError(GetLastError(), L"OpenProcess");

			if (!OpenProcessToken(hProcess, READ_CONTROL, &hObject))
				DisplayError(GetLastError(), L"OpenProcessToken");

			if (!CloseHandle(hProcess))
				DisplayError(GetLastError(), L"CloseHandle");
			break;
		case 'w': // window station
			hObject = OpenWindowStation(pszObject, FALSE, dwDesiredAccess);
			if (hObject == NULL)
				DisplayError(GetLastError(), L"OpenWindowStation");
			break;
		case 'k': // desktop
			// Find the desktop.
			pszDesktop = wcschr(pszObject, '\\');
			if (pszDesktop == NULL)
				return;
			else
				pszDesktop++;

			// Find the main key.
			hr = StringCbCopyN(szWinsta, sizeof(szWinsta), pszObject, (UINT)(pszDesktop - pszObject - 1));
			if (FAILED(hr))
				DisplayError(hr, L"StringCbCopyN");

			// Obtain a handle to the window station.
			hwinsta = OpenWindowStation(szWinsta, FALSE, MAXIMUM_ALLOWED);
			if (hwinsta == NULL)
				DisplayError(GetLastError(), L"OpenWindowStation");

			hwinstaCurrent = GetProcessWindowStation();
			if (hwinstaCurrent == NULL)
				DisplayError(GetLastError(), L"GetProcessWindowStation");

			if (!SetProcessWindowStation(hwinsta))
				DisplayError(GetLastError(), L"SetProcessWindowStation");

			hObject = OpenDesktop(pszDesktop, 0, FALSE, dwDesiredAccess);
			if (hObject == NULL)
				DisplayError(GetLastError(), L"OpenDesktop");

			if (!SetProcessWindowStation(hwinstaCurrent))
				DisplayError(GetLastError(), L"SetProcessWindowStation");

			if (!CloseWindowStation(hwinsta))
				DisplayError(GetLastError(), L"CloseWindowStation");
			break;
		case 'j': // job object
			hObject = OpenJobObject(dwDesiredAccess, FALSE, pszObject);
			if (hObject == NULL)
				DisplayError(GetLastError(), L"OpenJobObject");
			break;
		case 'z': // waitable timer
			hObject = OpenWaitableTimer(dwDesiredAccess, FALSE, pszObject);
			if (hObject == NULL)
				DisplayError(GetLastError(), L"OpenWaitableTimer");
			break;
		default:
			break;
	}

    // Disable the privilege.
	if (bAudit)
		Privilege(SE_SECURITY_NAME, FALSE);

	if (bVerbose)
		dwError = GetSecurityInfo(hObject, sot, si, &pOwner, &pGroup, &pDacl, &pSacl, &psd);
	else
		dwError = GetSecurityInfo(hObject, sot, si, NULL, NULL, NULL, NULL, &psd);
	
	if (dwError != ERROR_SUCCESS)
		DisplayError(dwError, L"GetSecurityInfo");

    // Dump security descriptor.
    DumpSD(psd, c, bVerbose, bAudit, pOwner, pGroup, pDacl, pSacl);

    // Free the buffer.
	if (NULL != psd)
		if (LocalFree((HLOCAL)psd))
			DisplayError(GetLastError(), L"LocalFree");

	switch (c)
	{
		case 'c':
			CloseServiceHandle((SC_HANDLE)hObject);
			break;
		case 'k':
			CloseDesktop((HDESK)hObject);
			break;
		case 'w':
			CloseWindowStation((HWINSTA)hObject);
			break;
		default:
			if (!CloseHandle(hObject))
				DisplayError(GetLastError(), L"CloseHandle");
	}
}

//
//   FUNCTION: DumpObjectWithName(LPWSTR, WCHAR, BOOl, BOOL)
//
//   PURPOSE: Obtain SD via the kernel name 
//   CppCheckSD.exe
//
//   PARAMETERS:
//   * pszObject - name of the kernel object
//   * c         - type of kernel object
//   * bVerbose  - TRUE displays all information on SD and FALSE returns in SDDL format
//   * bAudit    - TRUE indicates SD includes Audit Aces
//
//   RETURN VALUE: none
//
void DumpObjectWithName(LPWSTR pszObject, WCHAR c, BOOL bVerbose, BOOL bAudit)
{
	DWORD                dwError        = 0;
    DWORD                dwSize         = 0;
    PSECURITY_DESCRIPTOR psd            = NULL;
	PSID                 pOwner         = NULL;
	PSID                 pGroup         = NULL;
	PACL                 pDacl          = NULL;
	PACL                 pSacl          = NULL;
    SECURITY_INFORMATION si             = OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | LABEL_SECURITY_INFORMATION;
	SE_OBJECT_TYPE       sot;


    // Enable the privilege if auditing is enabled
	if (bAudit)
	{
		si = si | SACL_SECURITY_INFORMATION;
		Privilege(SE_SECURITY_NAME, TRUE);
	}

	// obtain security descriptor
    switch (c)
	{
		case 'f': // file
		case 'd': // directory
			sot = SE_FILE_OBJECT;
			break;

		case 'e': // event
        case 'm': // mutex
		case 'z': // waitable timer
		case 'i': // memory mapped file
        case 's': // semaphore
			sot = SE_KERNEL_OBJECT;
			break;
		case 't': // network share
			sot = SE_LMSHARE;
			break;
		case 'v': // service
			sot = SE_SERVICE;
			break;
		case 'l': // printer
			sot = SE_PRINTER;
			break;
		case 'r': // registry
			sot = SE_REGISTRY_KEY;
			break;
		case 'x': // 32 bit registry for a 64 bit app
			sot = SE_REGISTRY_WOW64_32KEY;
			break;
		default:

			break;
	}

	if (bVerbose)
		dwError = GetNamedSecurityInfo(pszObject, sot, si, &pOwner, &pGroup, &pDacl, &pSacl, &psd);
	else
		dwError = GetNamedSecurityInfo(pszObject, sot, si, NULL, NULL, NULL, NULL, &psd);

	if (dwError != ERROR_SUCCESS)
		DisplayError(dwError, L"GetNamedSecurityInfo");
    
    // Disable the privilege if viewing audit aces.
	if (bAudit)
		Privilege(SE_SECURITY_NAME, FALSE);

    // Dump security descriptor.
    DumpSD(psd, c, bVerbose, bAudit, pOwner, pGroup, pDacl, pSacl);

    // Free the buffer.
	if (NULL != psd)
		if (LocalFree((HLOCAL)psd))
			DisplayError(GetLastError(), L"LocalFree");
}




