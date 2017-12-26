/****************************** Module Header ******************************\
Module Name:  CppCreateLowIntegrityProcess.cpp
Project:      CppCreateLowIntegrityProcess
Copyright (c) Microsoft Corporation.

The code sample demonstrates how to start a low-integrity process. The 
application launches itself at the low integrity level when you click the 
"Launch myself at low integrity level" button on the application. Low 
integrity processes can only write to low integrity locations, such as the 
%USER PROFILE%\AppData\LocalLow folder or the HKEY_CURRENT_USER\Software\
AppDataLow key. If you attempt to gain write access to objects at a higher 
integrity levels, you will get an access denied error even though the 
user's SID is granted write access in the discretionary access control list 
(DACL). 

By default, child processes inherit the integrity level of their parent 
process. To start a low-integrity process, you must start a new child 
process with a low-integrity access token by using CreateProcessAsUser. 
Please refer to the CreateLowIntegrityProcess sample function for details.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes and Manifest Dependencies
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <sddl.h>
#include <shlobj.h>
#include "Resource.h"

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


#pragma region Helper Functions related to Process Integrity Level

// 
//   FUNCTION: CreateLowIntegrityProcess(PCWSTR)
//
//   PURPOSE: The function launches an application at low integrity level. 
//
//   PARAMETERS:
//   * pszCommandLine - The command line to be executed. The maximum length 
//     of this string is 32K characters. This parameter cannot be a pointer 
//     to read-only memory (such as a const variable or a literal string). 
//     If this parameter is a constant string, the function may cause an 
//     access violation.
//
//   RETURN VALUE: If the function succeeds, the return value is TRUE. If the 
//   function fails, the return value is zero. To get extended error 
//   information, call GetLastError.
//
//   COMMENT:
//   To start a low-integrity process, 
//   1) Duplicate the handle of the current process, which is at medium 
//      integrity level.
//   2) Use SetTokenInformation to set the integrity level in the access 
//      token to Low.
//   3) Use CreateProcessAsUser to create a new process using the handle to 
//      the low integrity access token.
//
BOOL CreateLowIntegrityProcess(PWSTR pszCommandLine)
{
    DWORD dwError = ERROR_SUCCESS;
    HANDLE hToken = NULL;
    HANDLE hNewToken = NULL;
    SID_IDENTIFIER_AUTHORITY MLAuthority = SECURITY_MANDATORY_LABEL_AUTHORITY;
    PSID pIntegritySid = NULL;
    TOKEN_MANDATORY_LABEL tml = { 0 };
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    // Open the primary access token of the process.
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE | TOKEN_QUERY |
        TOKEN_ADJUST_DEFAULT | TOKEN_ASSIGN_PRIMARY, &hToken))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Duplicate the primary token of the current process.
    if (!DuplicateTokenEx(hToken, 0, NULL, SecurityImpersonation, 
        TokenPrimary, &hNewToken))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Create the low integrity SID.
    if (!AllocateAndInitializeSid(&MLAuthority, 1, SECURITY_MANDATORY_LOW_RID, 
        0, 0, 0, 0, 0, 0, 0, &pIntegritySid))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    tml.Label.Attributes = SE_GROUP_INTEGRITY;
    tml.Label.Sid = pIntegritySid;

    // Set the integrity level in the access token to low.
    if (!SetTokenInformation(hNewToken, TokenIntegrityLevel, &tml, 
        (sizeof(tml) + GetLengthSid(pIntegritySid))))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Create the new process at the Low integrity level.
    if (!CreateProcessAsUser(hNewToken, NULL, pszCommandLine, NULL, NULL, 
        FALSE, 0, NULL, NULL, &si, &pi))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (hToken)
    {
        CloseHandle(hToken);
        hToken = NULL;
    }
    if (hNewToken)
    {
        CloseHandle(hNewToken);
        hNewToken = NULL;
    }
    if (pIntegritySid)
    {
        FreeSid(pIntegritySid);
        pIntegritySid = NULL;
    }
    if (pi.hProcess)
    {
        CloseHandle(pi.hProcess);
        pi.hProcess = NULL;
    }
    if (pi.hThread)
    {
        CloseHandle(pi.hThread);
        pi.hThread = NULL;
    }

    if (ERROR_SUCCESS != dwError)
    {
        // Make sure that the error code is set for failure.
        SetLastError(dwError);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


//
//   FUNCTION: GetProcessIntegrityLevel(PDWORD)
//
//   PURPOSE: The function gets the integrity level of the current process. 
//   Integrity level is only available on Windows Vista and newer operating 
//   systems, thus GetProcessIntegrityLevel returns FALSE if it is called on 
//   on systems prior to Windows Vista.
//
//   PARAMETERS: 
//   * pdwIntegrityLevel - Outputs the integrity level of the current 
//     process. It is usually one of these values:
//
//     SECURITY_MANDATORY_UNTRUSTED_RID (SID: S-1-16-0x0)
//     Means untrusted level. It is used by processes started by the 
//     Anonymous group. Blocks most write access. 
//
//     SECURITY_MANDATORY_LOW_RID (SID: S-1-16-0x1000)
//     Means low integrity level. It is used by Protected Mode Internet 
//     Explorer. Blocks write acess to most objects (such as files and 
//     registry keys) on the system. 
//
//     SECURITY_MANDATORY_MEDIUM_RID (SID: S-1-16-0x2000)
//     Means medium integrity level. It is used by normal applications 
//     being launched while UAC is enabled. 
//
//     SECURITY_MANDATORY_HIGH_RID (SID: S-1-16-0x3000)
//     Means high integrity level. It is used by administrative applications 
//     launched through elevation when UAC is enabled, or normal 
//     applications if UAC is disabled and the user is an administrator. 
//
//     SECURITY_MANDATORY_SYSTEM_RID (SID: S-1-16-0x4000)
//     Means system integrity level. It is used by services and other 
//     system-level applications (such as Wininit, Winlogon, Smss, etc.)  
//
//   RETURN VALUE: If the function succeeds, the return value is TRUE. If the 
//   function fails, the return value is FALSE. To get extended error 
//   information, call GetLastError. For example, ERROR_INVALID_PARAMETER is 
//   the last error if GetProcessIntegrityLevel is called on systems prior to 
//   Windows Vista or pdwIntegrityLevel is NULL.
//
//   EXAMPLE CALL:
//     DWORD dwIntegrityLevel;
//     if (!GetProcessIntegrityLevel(&dwIntegrityLevel))
//     {
//         wprintf(L"GetProcessIntegrityLevel failed w/err %lu\n", 
//             GetLastError());
//     }
//
BOOL GetProcessIntegrityLevel(PDWORD pdwIntegrityLevel)
{
    DWORD dwError = ERROR_SUCCESS;
    HANDLE hToken = NULL;
    DWORD cbTokenIL = 0;
    PTOKEN_MANDATORY_LABEL pTokenIL = NULL;

    if (pdwIntegrityLevel == NULL)
    {
        dwError = ERROR_INVALID_PARAMETER;
        goto Cleanup;
    }

    // Open the primary access token of the process with TOKEN_QUERY.
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Query the size of the token integrity level information. Note that 
    // we expect a FALSE result and the last error ERROR_INSUFFICIENT_BUFFER
    // from GetTokenInformation because we have given it a NULL buffer. On 
    // exit cbTokenIL will tell the size of the integrity level information.
    if (!GetTokenInformation(hToken, TokenIntegrityLevel, NULL, 0, &cbTokenIL))
    {
        if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
        {
            // When the process is run on operating systems prior to Windows 
            // Vista, GetTokenInformation returns FALSE with the 
            // ERROR_INVALID_PARAMETER error code because TokenElevation 
            // is not supported on those operating systems.
            dwError = GetLastError();
            goto Cleanup;
        }
    }

    // Now we allocate a buffer for the integrity level information.
    pTokenIL = (TOKEN_MANDATORY_LABEL *)LocalAlloc(LPTR, cbTokenIL);
    if (pTokenIL == NULL)
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Retrieve token integrity level information.
    if (!GetTokenInformation(hToken, TokenIntegrityLevel, pTokenIL, 
        cbTokenIL, &cbTokenIL))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Integrity Level SIDs are in the form of S-1-16-0xXXXX. (e.g. 
    // S-1-16-0x1000 stands for low integrity level SID). There is one and 
    // only one subauthority.
    *pdwIntegrityLevel = *GetSidSubAuthority(pTokenIL->Label.Sid, 0);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (hToken)
    {
        CloseHandle(hToken);
        hToken = NULL;
    }
    if (pTokenIL)
    {
        LocalFree(pTokenIL);
        pTokenIL = NULL;
        cbTokenIL = 0;
    }

    if (ERROR_SUCCESS != dwError)
    {
        // Make sure that the error code is set for failure.
        SetLastError(dwError);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

#pragma endregion


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
void ReportError(LPCWSTR pszFunction, DWORD dwError = GetLastError())
{
    wchar_t szMessage[200];
    if (-1 != swprintf_s(szMessage, ARRAYSIZE(szMessage), 
        L"%s failed w/err 0x%08lx", pszFunction, dwError))
    {
        MessageBox(NULL, szMessage, L"Error", MB_ICONERROR);
    }
}


// 
//   FUNCTION: OnInitDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message.
//
BOOL OnInitDialog(HWND hWnd, HWND hwndFocus, LPARAM lParam)
{
    // Get and display the process integrity level.
    HWND hILLabel = GetDlgItem(hWnd, IDC_IL_STATIC);
    DWORD dwIntegrityLevel;
    if (GetProcessIntegrityLevel(&dwIntegrityLevel))
    {
        switch (dwIntegrityLevel)
        {
        case SECURITY_MANDATORY_UNTRUSTED_RID: SetWindowText(hILLabel, L"Untrusted"); break;
        case SECURITY_MANDATORY_LOW_RID: SetWindowText(hILLabel, L"Low"); break;
        case SECURITY_MANDATORY_MEDIUM_RID: SetWindowText(hILLabel, L"Medium"); break;
        case SECURITY_MANDATORY_HIGH_RID: SetWindowText(hILLabel, L"High"); break;
        case SECURITY_MANDATORY_SYSTEM_RID: SetWindowText(hILLabel, L"System"); break;
        default: SetWindowText(hILLabel, L"Unknown"); break;
        }
    }
    else
    {
        ReportError(L"GetProcessIntegrityLevel");
        SetWindowText(hILLabel, L"N/A");
    }

    return TRUE;
}


// 
//   FUNCTION: CreateTestFileInKnownFolder(REFKNOWNFOLDERID)
//
//   PURPOSE: The function attempts to create a test file (testfile.txt) in 
//   the specified Windows known folder, and shows the test result in a 
//   message box.
//
//   PARAMETERS:
//   * rfid - A reference to the KNOWNFOLDERID that identifies the folder.
//
void CreateTestFileInKnownFolder(REFKNOWNFOLDERID rfid) 
{
    HRESULT hr = S_OK;
    PWSTR pszFolder = NULL;
    wchar_t szPath[MAX_PATH];
    HANDLE hFile = INVALID_HANDLE_VALUE;
    wchar_t szMessage[1024];

    // Get the known folder.
    hr = SHGetKnownFolderPath(rfid, 0, NULL, &pszFolder);
    if (FAILED(hr))
    {
        swprintf_s(szMessage, ARRAYSIZE(szMessage), 
            L"SHGetKnownFolderPath failed w/err 0x%08lx.", hr);
        MessageBox(NULL, szMessage, L"Error", MB_ICONERROR);

        goto Cleanup;
    }

    // Append the file name to the folder to get the complete file path.
    wcscpy_s(szPath, ARRAYSIZE(szPath), pszFolder);
    wcscat_s(szPath, ARRAYSIZE(szPath), L"\\testfile.txt");

    // Create the test file.
    hFile = CreateFile(szPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }
    
    // Show the test result.
    if (SUCCEEDED(hr))
    {
        swprintf_s(szMessage, ARRAYSIZE(szMessage), 
            L"Successfully write to the test file: %s", szPath);
        MessageBox(NULL, szMessage, L"Test Result", MB_ICONINFORMATION);
    }
    else if (hr == E_ACCESSDENIED)
    {
        swprintf_s(szMessage, ARRAYSIZE(szMessage), 
            L"Access to the path '%s' is denied.", szPath);
        MessageBox(NULL, szMessage, L"Test Result", MB_ICONERROR);
    }
    else
    {
        swprintf_s(szMessage, ARRAYSIZE(szMessage), 
            L"Failed to write to the test file '%s' w/err 0x%08lx", 
            szPath, hr);
        MessageBox(NULL, szMessage, L"Test Result", MB_ICONERROR);
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pszFolder)
    {
        CoTaskMemFree(pszFolder);
        pszFolder = NULL;
    }
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
}


//
//   FUNCTION: OnCommand(HWND, int, HWND, UINT)
//
//   PURPOSE: Process the WM_COMMAND message
//
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDC_CREATELOWPROCESS_BN:
        {
            wchar_t szPath[MAX_PATH];
            if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
            {
                // Launch the application at low integrity level.
                if (!CreateLowIntegrityProcess(szPath))
                {
                    ReportError(L"CreateLowIntegrityProcess");
                }
            }
        }
        break;

    case IDC_WRITELOCALAPPDATA_BN:
        // Attempt to create a test file in the LocalAppData known folder.
        CreateTestFileInKnownFolder(FOLDERID_LocalAppData);
        break;

    case IDC_WRITELOCALAPPDATALOW_BN:
        // Attempt to create a test file in the LocalAppDataLow known folder.
        CreateTestFileInKnownFolder(FOLDERID_LocalAppDataLow);
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
//   PURPOSE: Process the WM_CLOSE message
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