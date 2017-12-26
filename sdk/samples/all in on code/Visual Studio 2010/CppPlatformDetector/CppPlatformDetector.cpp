/****************************** Module Header ******************************\
Module Name:    CppPlatformDetector.cpp
Project:        CppPlatformDetector
Copyright (c) Microsoft Corporation.

The CppPlatformDetector code sample demonstrates the following tasks related 
to platform detection:

1. Detect the name of the current operating system. 
   (e.g. "Microsoft Windows 7 Enterprise")
2. Detect the version of the current operating system.
   (e.g. "Microsoft Windows NT 6.1.7600.0")
3. Determine whether the current operating system is a 64-bit operating 
   system. 
4. Determine whether the current process is a 64-bit process. 
5. Determine whether an arbitrary process running on the system is 64-bit. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>
#include <Windows.h>
#include "PlatformDetector.h"


int wmain(int argc, wchar_t *argv[])
{
    //
    // Print the name of the current operating system. 
    //

    wchar_t szName[512];
    if (GetOSName(szName, ARRAYSIZE(szName)))
    {
        wprintf(L"Current OS: %s\n", szName);
    }
    else
    {
        wprintf(L"Cannot get the operating system name\n");
    }

    //
    // Print the version string of the current operating system.
    //

    wchar_t szVersionString[512];
    if (GetOSVersionString(szVersionString, ARRAYSIZE(szVersionString)))
    {
        wprintf(L"Version: %s\n", szVersionString);
    }
    else
    {
        wprintf(L"Cannot get the operating system version\n");
    }

    //
    // Determine the whether the current OS is a 64-bit operating system. 
    //

    BOOL f64bitOS = Is64BitOS();
    wprintf(L"Current OS is %s64-bit\n", f64bitOS ? L"" : L"not ");

    //
    // Determine whether the current process is a 64-bit process. 
    //

    BOOL f64bitProc = Is64BitProcess();
    wprintf(L"Current process is %s64-bit\n", f64bitProc ? L"" : L"not ");

    //
    // Determine whether an arbitrary process running on the system is 64-bit.
    //

    if (argc > 1)
    {
        // If a process ID is specified in the command line, get the process 
		// id, and open the process handle.
        DWORD dwProcessId = _wtoi(argv[1]);
        if (dwProcessId != 0 /*conversion succeeds*/)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE,
                dwProcessId);
            if (hProcess != NULL)
            {
                // Detect whether the specified process is a 64-bit.
                BOOL f64bitProc = Is64BitProcess(hProcess);
                wprintf(L"Process %d is %s64-bit\n", dwProcessId, 
                    f64bitProc ? L"" : L"not ");

                CloseHandle(hProcess);
            }
            else
            {
                wprintf(L"OpenProcess(%d) failed w/err 0x%08lx\n", 
                    dwProcessId, GetLastError());
            }
        }
        else
        {
            wprintf(L"Invalid process ID: %s\n", argv[1]);
        }
    }

    return 0;
}