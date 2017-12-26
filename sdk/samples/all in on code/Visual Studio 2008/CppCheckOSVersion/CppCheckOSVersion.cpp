/****************************** Module Header ******************************\
Module Name:  Program.cs
Project:      CppCheckOSVersion
Copyright (c) Microsoft Corporation.

The CppCheckOSVersion demonstrates how to detect the current OS version, 
and how to make application that checks for the minimum operating system 
version work with later operating system versions.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>
#include <windows.h>


int wmain(int argc, wchar_t *argv[])
{
    //
    // Detect the current OS version.
    // 

    OSVERSIONINFOEX osVersionInfo = { sizeof(osVersionInfo) };
    if (!GetVersionEx((LPOSVERSIONINFO) &osVersionInfo))
    {
        wprintf(L"GetVersionEx failed w/err 0x%08lx\n", GetLastError());
        return 1;
    }

    PCWSTR pszPlatform = NULL;
    switch (osVersionInfo.dwPlatformId)
    {
    case 0:
        pszPlatform = L"Microsoft Win32S";
        break;
    case 1:
        if (osVersionInfo.dwMajorVersion > 4 || 
            (osVersionInfo.dwMajorVersion == 4 && osVersionInfo.dwMinorVersion > 0))
        {
            pszPlatform = L"Microsoft Windows 98";
        }
        else
        {
            pszPlatform = L"Microsoft Windows 95";
        }
        break;
    case 2:
        pszPlatform = L"Microsoft Windows NT";
        break;
    case 3:
        pszPlatform = L"Microsoft Windows CE";
        break;
    default:
        pszPlatform = L"Unknown platform";
        return 1;
    }
    wchar_t szVersionString[256];
    swprintf_s(szVersionString, ARRAYSIZE(szVersionString), 
        L"%s %d.%d.%d.%d", 
        pszPlatform, osVersionInfo.dwMajorVersion, 
        osVersionInfo.dwMinorVersion, 
        osVersionInfo.dwBuildNumber, 
        osVersionInfo.wServicePackMajor << 0x10 | osVersionInfo.wServicePackMinor);
    wprintf(L"Current OS: %s\n", szVersionString);


    //
    // Make application that checks for the minimum operating system version
    // work with later operating system versions. To compare the current 
    // system version to a required version, use the VerifyVersionInfo 
    // function instead of using GetVersionEx to perform the comparison 
    // yourself.
    // 

    // osVersionInfoToCompare contains the OS version requirements to compare
    OSVERSIONINFOEX osVersionInfoToCompare = { sizeof(OSVERSIONINFOEX) };
    osVersionInfoToCompare.dwMajorVersion = 5;
    osVersionInfoToCompare.dwMinorVersion = 1;		// Windows XP
    osVersionInfoToCompare.wServicePackMajor = 2;	// Service Pack 2
    osVersionInfoToCompare.wServicePackMinor = 0;

    // Initialize the condition mask with ULONGLONG VER_SET_CONDITION(
    // ULONGLONG dwlConditionMask, DWORD dwTypeBitMask, BYTE dwConditionMask)
    ULONGLONG comparisonInfo = 0;
    BYTE conditionMask = VER_GREATER_EQUAL;
    VER_SET_CONDITION(comparisonInfo, VER_MAJORVERSION, conditionMask);
    VER_SET_CONDITION(comparisonInfo, VER_MINORVERSION, conditionMask);
    VER_SET_CONDITION(comparisonInfo, VER_SERVICEPACKMAJOR, conditionMask);
    VER_SET_CONDITION(comparisonInfo, VER_SERVICEPACKMINOR, conditionMask);

    // Compares a set of operating system version requirements to the 
    // corresponding values for the currently running version of the system.
    if (!VerifyVersionInfo(&osVersionInfoToCompare, VER_MAJORVERSION | 
        VER_MINORVERSION | VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
        comparisonInfo))
    {
        wprintf(L"Windows XP or later required.");
        // Quit the application due to incompatible OS
        return 1;
    }

    wprintf(L"Application Running...\n");
    getchar();

    return 0;
}