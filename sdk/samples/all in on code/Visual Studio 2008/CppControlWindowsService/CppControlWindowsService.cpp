/****************************** Module Header ******************************\
Module Name:  CppControlWindowsService.cpp
Project:      CppControlWindowsService
Copyright (c) Microsoft Corporation.

The code sample demonstrates how to install, uninstall, find, start, stop a 
Windows service and set the service DACL to grant start, stop, delete and 
read control access to all authenticated users programmatically.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>
#include <windows.h>
#include <strsafe.h>
#include "ServiceController.h"


void PrintInstructions()
{
    wprintf(L"CppControlWindowsService Instructions:\n" \
        L"-install <ServiceName> <BinaryPath>  Install a service\n" \
        L"-uninstall <ServiceName>             Uninstall a service\n" \
        L"-find <ServiceName>                  Find a service\n" \
        L"-start <ServiceName>                 Start a service\n" \
        L"-stop <ServiceName>                  Stop a service\n" \
        L"-update dacl <ServiceName>           Update the DACL of a service\n"
        );
}


int wmain(int argc, wchar_t *argv[])
{
    BOOL fGoodCmd = FALSE;

    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
    {
        if (_wcsicmp(L"install", argv[1] + 1) == 0 && argc > 3)
        {
            // Install a service - requires 2 additional parameters:
            // -install <ServiceName> <BinaryPath>
            wchar_t szServiceName[256];
            wchar_t szBinaryPath[MAX_PATH];

            fGoodCmd = 
                SUCCEEDED(StringCchCopy(szServiceName, ARRAYSIZE(szServiceName), argv[2])) && 
                SUCCEEDED(StringCchCopy(szBinaryPath, ARRAYSIZE(szBinaryPath), argv[3]));

            if (fGoodCmd)
            {
                HRESULT hr = InstallService(
                    NULL,                   // Machine name (local machine)
                    szServiceName,          // Service name
                    szServiceName,          // Service display name
                    SERVICE_DEMAND_START,   // Start on demand
                    szBinaryPath,           // Binary path
                    NULL,                   // Local system
                    L"");                   // No password

                if (SUCCEEDED(hr))
                {
                    wprintf(L"Service installed successfully\n");
                }
                else
                {
                    wprintf(L"InstallService failed w/err 0x%08lx\n", hr);
                }
            }
        }
        else if (_wcsicmp(L"uninstall", argv[1] + 1) == 0 && argc > 2)
        {
            // Uninstall the service - requires 1 additional parameter:
            // -uninstall <ServiceName>
            wchar_t szServiceName[256];

            fGoodCmd = 
                SUCCEEDED(StringCchCopy(szServiceName, ARRAYSIZE(szServiceName), argv[2]));

            if (fGoodCmd)
            {
                HRESULT hr = UninstallService(NULL, szServiceName);

                if (SUCCEEDED(hr))
                {
                    wprintf(L"Service uninstalled successfully\n");
                }
                else
                {
                    wprintf(L"UninstallService failed w/err 0x%08lx\n", hr);
                }
            }
        }
        else if (_wcsicmp(L"find", argv[1] + 1) == 0 && argc > 2)
        {
            // Find a service - requires 1 additional parameter:
            // -find <ServiceName>
            wchar_t szServiceName[256];

            fGoodCmd = 
                SUCCEEDED(StringCchCopy(szServiceName, ARRAYSIZE(szServiceName), argv[2]));

            if (fGoodCmd)
            {
                BOOL fInstalled;
                HRESULT hr = IsServiceInstalled(NULL, szServiceName, &fInstalled);

                if (SUCCEEDED(hr))
                {
                    wprintf(L"Service %s was %sinstalled\n", szServiceName, 
                        fInstalled ? L"" : L"not ");
                }
                else
                {
                    wprintf(L"IsServiceInstalled failed w/err 0x%08lx\n", hr);
                }
            }
        }
        else if (_wcsicmp(L"start", argv[1] + 1) == 0 && argc > 2)
        {
            // Start the service - requires 1 additional parameter:
            // -start <ServiceName>
            wchar_t szServiceName[256];

            fGoodCmd = 
                SUCCEEDED(StringCchCopy(szServiceName, ARRAYSIZE(szServiceName), argv[2]));

            if (fGoodCmd)
            {
                HRESULT hr = StartService(NULL, szServiceName);

                if (SUCCEEDED(hr))
                {
                    wprintf(L"Service started successfully\n");
                }
                else
                {
                    if (hr == RPC_E_TIMEOUT)
                    {
                        wprintf(L"Service start timed out\n");
                    }
                    else
                    {
                        wprintf(L"StartService failed w/err 0x%08lx\n", hr);
                    }
                }
            }
        }
        else if (_wcsicmp(L"stop", argv[1] + 1) == 0 && argc > 2)
        {
            // Stop the service - requires 1 additional parameter:
            // -stop <ServiceName>
            wchar_t szServiceName[256];

            fGoodCmd = 
                SUCCEEDED(StringCchCopy(szServiceName, ARRAYSIZE(szServiceName), argv[2]));

            if (fGoodCmd)
            {
                HRESULT hr = StopService(NULL, szServiceName);

                if (SUCCEEDED(hr))
                {
                    wprintf(L"Service stopped successfully\n");
                }
                else
                {
                    if (hr == RPC_E_TIMEOUT)
                    {
                        wprintf(L"Service stop timed out\n");
                    }
                    else
                    {
                        wprintf(L"StopService failed w/err 0x%08lx\n", hr);
                    }
                }
            }
        }
        else if (_wcsicmp(L"update", argv[1] + 1) == 0 && argc > 3)
        {
            // Update the service DACL - requires 2 additional parameters:
            // -update dacl <ServiceName>
            wchar_t szServiceName[256];

            fGoodCmd = 
                (_wcsicmp(L"dacl", argv[2]) == 0) && 
                SUCCEEDED(StringCchCopy(szServiceName, ARRAYSIZE(szServiceName), argv[3]));

            if (fGoodCmd)
            {
                HRESULT hr = UpdateServiceDACL(NULL, szServiceName);

                if (SUCCEEDED(hr))
                {
                    wprintf(L"Service DACL updated successfully\n");
                }
                else
                {
                    wprintf(L"UpdateServiceDACL failed w/err 0x%08lx\n", hr);
                }
            }
        }
    }

    if (!fGoodCmd)
    {
        PrintInstructions();
    }

    return 0;
}