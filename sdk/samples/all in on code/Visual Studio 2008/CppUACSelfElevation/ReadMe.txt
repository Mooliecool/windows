=============================================================================
          WIN32 APPLICATION : CppUACSelfElevation Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary: 

User Account Control (UAC) is a new security component in Windows Vista and 
newer operating systems. With UAC fully enabled, interactive administrators 
normally run with least user privileges. This example demonstrates how to 
check the privilege level of the current process, and how to self-elevate 
the process by giving explicit consent with the Consent UI.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

You must run this sample on Windows Vista or newer operating systems.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the UAC sample.

Step1. After you successfully build the sample project in Visual Studio 2008, 
you will get an application: CppUACSelfElevation.exe. 

Step2. Run the application as a protected administrator on a Windows Vista or 
Windows 7 system with UAC fully enabled. The application should display the 
following content on the main dialog.

  IsUserInAdminGroup:     True
  IsRunAsAdmin:           False
  IsProcessElevated:      False
  Integrity Level:        Medium

There is a UAC shield icon on the Self-elevate button.

Step3. Click on the Self-elevate button. You will see a Consent UI.

  User Account Control
  ---------------------------------- 
  Do you want to allow the following program from an unknown publisher to 
  make changes to this computer?

Step4. Click Yes to approve the elevation. The original application will then 
be started and display the following content on the main dialog.

  IsUserInAdminGroup:     True
  IsRunAsAdmin:           True
  IsProcessElevated:      True
  Integrity Level:        High

The Self-elevate button on the dialog does not have the UAC shield icon this 
time. That is, the application is running as elevated administrator. The 
elevation succeeds. If you click on the Self-elevate button again, the 
application will tell you that it is running as administrator.

Step5. Click OK to close the application. 


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step1. Create a basic VC++ Win32 dialog based application

Create a new Visual C++ / Win32 / Win32 Project. Name it as 
CppUACSelfElevation and set the Application type to Windows application in 
the Application Settings page. After the project is created, disable 
Precompiled Headers in Project Properties / Configuration Properties / C/C++ 
/ Precompiled Headers. In Resource View, delete all default accelerator, 
dialog, icon, menu, and string table resources because they are not 
necessary in this example. Next, insert a dialog resource with the ID: 
IDD_MAINDIALOG. It serves as the main dialog of the Windows application. In 
Solution Explorer, delete the wizard-generated files stdafx.cpp, stdafx.h, 
targetver.h, CppUACSelfElevation.h, CppUACSelfElevation.ico, and small.ico to 
simplify the project files. Open CppUACSelfElevation.cpp and replace its 
content with the following code, which delineate the skeleton of a VC++ Win32 
dialog based application.

    #include <stdio.h>
    #include <windows.h>
    #include <windowsx.h>
    #include "Resource.h"

    BOOL OnInitDialog(HWND hWnd, HWND hwndFocus, LPARAM lParam)
    {
        return TRUE;
    }
    void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
        case IDOK:
        case IDCANCEL:
            EndDialog(hWnd, 0);
            break;
        }
    }
    void OnClose(HWND hWnd)
    {
        EndDialog(hWnd, 0);
    }
    INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitDialog);
            HANDLE_MSG (hWnd, WM_COMMAND, OnCommand);
            HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

        default:
            return FALSE;
        }
        return 0;
    }
    int APIENTRY wWinMain(HINSTANCE hInstance,
                          HINSTANCE hPrevInstance,
                          LPWSTR    lpCmdLine,
                          int       nCmdShow)
    {
        return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
    }

Step2. Add controls to the main dialog

  Type: Button
  ID: IDC_ELEVATE_BN
  Caption: "Self-elevate"
  
  Type: Static Text
  ID: IDC_INADMINGROUP_STATIC
  Use: Display whether the primary access token of the process belongs to 
  user account that is a member of the local Administrators group, even if it 
  currently is not elevated.
  
  Type: Static Text
  ID: IDC_ISRUNASADMIN_STATIC
  Use: Display whether the application is run as administrator. 
  
  Type: Static Text
  ID: IDC_ISELEVATED_STATIC
  Use: Display whether the process is elevated or not. Token elevation is 
  only available on Windows Vista and newer operating systems. The label 
  shows N/A on systems prior to Windows Vista.

  Type: Static Text
  ID: IDC_IL_STATIC
  Use: Display the integrity level of the current process. Integrity level is 
  only available on Windows Vista and newer operating systems. The label 
  shows N/A on systems prior to Windows Vista.

Step3. Check and display the current process's "run as administrator" status, 
elevation information and integrity level when the application initializes 
the main dialog.

Create the following four helper functions:

//
//   FUNCTION: IsUserInAdminGroup()
//
//   PURPOSE: The function checks whether the primary access token of the 
//   process belongs to user account that is a member of the local 
//   Administrators group, even if it currently is not elevated.
//
//   RETURN VALUE: Returns TRUE if the primary access token of the process 
//   belongs to user account that is a member of the local Administrators 
//   group. Returns FALSE if the token does not.
//
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception which 
//   contains the Win32 error code of the failure.
//
//   EXAMPLE CALL:
//     try 
//     {
//         if (IsUserInAdminGroup())
//             wprintf (L"User is a member of the Administrators group\n");
//         else
//             wprintf (L"User is not a member of the Administrators group\n");
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"IsUserInAdminGroup failed w/err %lu\n", dwError);
//     }
//
BOOL IsUserInAdminGroup();

// 
//   FUNCTION: IsRunAsAdmin()
//
//   PURPOSE: The function checks whether the current process is run as 
//   administrator. In other words, it dictates whether the primary access 
//   token of the process belongs to user account that is a member of the 
//   local Administrators group and it is elevated.
//
//   RETURN VALUE: Returns TRUE if the primary access token of the process 
//   belongs to user account that is a member of the local Administrators 
//   group and it is elevated. Returns FALSE if the token does not.
//
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception which 
//   contains the Win32 error code of the failure.
//
//   EXAMPLE CALL:
//     try 
//     {
//         if (IsRunAsAdmin())
//             wprintf (L"Process is run as administrator\n");
//         else
//             wprintf (L"Process is not run as administrator\n");
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"IsRunAsAdmin failed w/err %lu\n", dwError);
//     }
//
BOOL IsRunAsAdmin();

//
//   FUNCTION: IsProcessElevated()
//
//   PURPOSE: The function gets the elevation information of the current 
//   process. It dictates whether the process is elevated or not. Token 
//   elevation is only available on Windows Vista and newer operating 
//   systems, thus IsProcessElevated throws a C++ exception if it is called 
//   on systems prior to Windows Vista. It is not appropriate to use this 
//   function to determine whether a process is run as administartor.
//
//   RETURN VALUE: Returns TRUE if the process is elevated. Returns FALSE if 
//   it is not.
//
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception 
//   which contains the Win32 error code of the failure. For example, if 
//   IsProcessElevated is called on systems prior to Windows Vista, the error 
//   code will be ERROR_INVALID_PARAMETER.
//
//   NOTE: TOKEN_INFORMATION_CLASS provides TokenElevationType to check the 
//   elevation type (TokenElevationTypeDefault / TokenElevationTypeLimited /
//   TokenElevationTypeFull) of the process. It is different from 
//   TokenElevation in that, when UAC is turned off, elevation type always 
//   returns TokenElevationTypeDefault even though the process is elevated 
//   (Integrity Level == High). In other words, it is not safe to say if the 
//   process is elevated based on elevation type. Instead, we should use 
//   TokenElevation.
//
//   EXAMPLE CALL:
//     try 
//     {
//         if (IsProcessElevated())
//             wprintf (L"Process is elevated\n");
//         else
//             wprintf (L"Process is not elevated\n");
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"IsProcessElevated failed w/err %lu\n", dwError);
//     }
//
BOOL IsProcessElevated();

//
//   FUNCTION: GetProcessIntegrityLevel()
//
//   PURPOSE: The function gets the integrity level of the current process. 
//   Integrity level is only available on Windows Vista and newer operating 
//   systems, thus GetProcessIntegrityLevel throws a C++ exception if it is 
//   called on systems prior to Windows Vista.
//
//   RETURN VALUE: Returns the integrity level of the current process. It is 
//   usually one of these values:
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
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception 
//   which contains the Win32 error code of the failure. For example, if 
//   GetProcessIntegrityLevel is called on systems prior to Windows Vista, 
//   the error code will be ERROR_INVALID_PARAMETER.
//
//   EXAMPLE CALL:
//     try 
//     {
//         DWORD dwIntegrityLevel = GetProcessIntegrityLevel();
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"GetProcessIntegrityLevel failed w/err %lu\n", dwError);
//     }
//
DWORD GetProcessIntegrityLevel();

In OnInitDialog of the main dialog, check and display the "run as 
administrator" status, the elevation information, and the integrity level of 
the current process.

    // Get and display whether the primary access token of the process 
    // belongs to user account that is a member of the local Administrators 
    // group even if it currently is not elevated (IsUserInAdminGroup).
    HWND hInAdminGroupLabel = GetDlgItem(hWnd, IDC_INADMINGROUP_STATIC);
    try
    {
        BOOL const fInAdminGroup = IsUserInAdminGroup();
        SetWindowText(hInAdminGroupLabel, fInAdminGroup ? L"True" : L"False");
    }
    catch (DWORD dwError)
    {
        SetWindowText(hInAdminGroupLabel, L"N/A");
        ReportError(L"IsUserInAdminGroup", dwError);
    }

    // Get and display whether the process is run as administrator or not 
    // (IsRunAsAdmin).
    HWND hIsRunAsAdminLabel = GetDlgItem(hWnd, IDC_ISRUNASADMIN_STATIC);
    try
    {
        BOOL const fIsRunAsAdmin = IsRunAsAdmin();
        SetWindowText(hIsRunAsAdminLabel, fIsRunAsAdmin ? L"True" : L"False");
    }
    catch (DWORD dwError)
    {
        SetWindowText(hIsRunAsAdminLabel, L"N/A");
        ReportError(L"IsRunAsAdmin", dwError);
    }
    
    // Get and display the process elevation information (IsProcessElevated) 
    // and integrity level (GetProcessIntegrityLevel). The information is not 
    // available on operating systems prior to Windows Vista.

    HWND hIsElevatedLabel = GetDlgItem(hWnd, IDC_ISELEVATED_STATIC);
    HWND hILLabel = GetDlgItem(hWnd, IDC_IL_STATIC);

    OSVERSIONINFO osver = { sizeof(osver) };
    if (GetVersionEx(&osver) && osver.dwMajorVersion >= 6)
    {
        // Running Windows Vista or later (major version >= 6).

        try
        {
            // Get and display the process elevation information.
            BOOL const fIsElevated = IsProcessElevated();
            SetWindowText(hIsElevatedLabel, fIsElevated ? L"True" : L"False");

            // Update the Self-elevate button to show the UAC shield icon on 
            // the UI if the process is not elevated. The 
            // Button_SetElevationRequiredState macro (declared in Commctrl.h) 
            // is used to show or hide the shield icon in a button. You can 
            // also get the shield directly as an icon by calling 
            // SHGetStockIconInfo with SIID_SHIELD as the parameter.
            HWND hElevateBtn = GetDlgItem(hWnd, IDC_ELEVATE_BN);
            Button_SetElevationRequiredState(hElevateBtn, !fIsElevated);
        }
        catch (DWORD dwError)
        {
            SetWindowText(hIsElevatedLabel, L"N/A");
            ReportError(L"IsProcessElevated", dwError);
        }

        try
        {
            // Get and display the process integrity level.
            DWORD const dwIntegrityLevel = GetProcessIntegrityLevel();
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
        catch (DWORD dwError)
        {
            SetWindowText(hILLabel, L"N/A");
            ReportError(L"GetProcessIntegrityLevel", dwError);
        }
    }
    else
    {
        SetWindowText(hIsElevatedLabel, L"N/A");
        SetWindowText(hILLabel, L"N/A");
    }

Step4. Handle the click event of the Self-elevate button in OnCommand. When 
user clicks the button, elevate the process by calling ShellExecuteEx with 
SHELLEXECUTEINFO.lpVerb = L"runas" to restart itself if the process is not 
run as administrator.

    void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
        case IDC_ELEVATE_BN:
            {
                // Check the current process's "run as administrator" status
                BOOL fIsRunAsAdmin;
                try
                {
                    fIsRunAsAdmin = IsRunAsAdmin();
                }
                catch (DWORD dwError)
                {
                    ReportError(L"IsRunAsAdmin", dwError);
                    break;
                }

                // Elevate the process if it is not run as administrator.
                if (!fIsRunAsAdmin)
                {
                    wchar_t szPath[MAX_PATH];
                    if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
                    {
                        // Launch itself as administrator.
                        SHELLEXECUTEINFO sei = { sizeof(sei) };
                        sei.lpVerb = L"runas";
                        sei.lpFile = szPath;
                        sei.hwnd = hWnd;
                        sei.nShow = SW_NORMAL;

                        if (!ShellExecuteEx(&sei))
                        {
                            DWORD dwError = GetLastError();
                            if (dwError == ERROR_CANCELLED)
                            {
                                // The user refused the elevation.
                                // Do nothing ...
                            }
                            else
                            {
                                ReportError(L"ShellExecuteEx", dwError);
                            }
                        }
                        else
                        {
                            EndDialog(hWnd, TRUE);  // Quit itself
                        }
                    }
                }
                else
                {
                    MessageBox(hWnd, L"The process is running as administrator", L"UAC", MB_OK);
                }
            }
            break;
        }
    }

Step5. Automatically elevate the process when it's started up.

If your application always requires administrative privileges, such as during 
an installation step, the operating system can automatically prompt the user 
for privileges elevation each time your application is invoked. 

If a specific kind of resource (RT_MANIFEST) is found embedded within the 
application executable, the system looks for the <trustInfo> section and 
parses its contents. Here is an example of this section in the manifest file:

    <trustInfo xmlns="urn:schemas-microsoft-com:asm.v2">
       <security>
          <requestedPrivileges>
             <requestedExecutionLevel
                level="requireAdministrator"
             />
          </requestedPrivileges>
       </security>
    </trustInfo>

Three different values are possible for the level attribute

  a) requireAdministrator 
  The application must be started with Administrator privileges; it won't run 
  otherwise.

  b) highestAvailable 
  The application is started with the highest possible privileges.
  If the user is logged on with an Administrator account, an elevation prompt 
  appears. If the user is a Standard User, the application is started 
  (without any elevation prompt) with these standard privileges.

  c) asInvoker 
  The application is started with the same privileges as the calling 
  application.

To configure the elevation level in a VC++ project, open the project's 
properties dialog, turn to Linker /Manifest File, and select UAC Execution 
Level.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: User Account Control
http://msdn.microsoft.com/en-us/library/aa511445.aspx

MSDN: Windows Vista Application Development Requirements for User Account 
Control Compatibility
http://msdn.microsoft.com/en-us/library/bb530410.aspx

MSDN: Windows NT Security
http://msdn.microsoft.com/en-us/library/ms995339.aspx

How to tell if the current user is in administrators group programmatically
http://blogs.msdn.com/junfeng/archive/2007/01/26/how-to-tell-if-the-current-user-is-in-administrators-group-programmatically.aspx


/////////////////////////////////////////////////////////////////////////////