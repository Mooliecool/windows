/****************************** Module Header ******************************\
Module Name:  ServiceController.cpp
Project:      CppControlWindowsService
Copyright (c) Microsoft Corporation.

The file defines the helper functions for installing, uninstalling, finding, 
starting, stopping, and updating the DACL of a Windows service. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "ServiceController.h"
#include <Aclapi.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


HRESULT StopService(SC_HANDLE schSCManager, SC_HANDLE schService);
HRESULT StopDependentServices(SC_HANDLE schSCManager, SC_HANDLE schService);


//
//   FUNCTION: InstallService
//
//   PURPOSE: Install a Windows service.
// 
//   PARAMETERS:
//   * pszMachineName - The name of the target computer. If the pointer is 
//     NULL or points to an empty string, the function connects to the SCM on 
//     the local computer.
//   * pszServiceName - The name of the service to install. The maximum 
//     string length is 256 characters.
//   * pszDisplayName - The display name to be used by user interface programs 
//     to identify the service. This string has a maximum length of 256 chars. 
//   * dwStartType - The service start options. The parameter can be one of 
//     the following values: SERVICE_AUTO_START, SERVICE_BOOT_START, 
//     SERVICE_DEMAND_START, SERVICE_DISABLED, SERVICE_SYSTEM_START.
//   * pszBinaryPathName - The full path to the service binary file. 
//   * pszServiceStartName - The name of the account under which the service 
//     should run. If this parameter is NULL, CreateService uses the 
//     LocalSystem account. The parameter can also be 
//     "NT AUTHORITY\LocalService", "NT AUTHORITY\NetworkService", or in the 
//     form of "DomainName\UserName", or ".\UserName".
//   * pszPassword - The password to the account name specified by the 
//     pszServiceStartName parameter. Specify an empty string if the account 
//     has no password or if the service runs in the LocalService, 
//     NetworkService, or LocalSystem account.
//
//   RETURN VALUE: HRESULT
//
HRESULT InstallService(PCWSTR pszMachineName, 
                       PCWSTR pszServiceName, 
                       PCWSTR pszDisplayName, 
                       DWORD dwStartType, 
                       PCWSTR pszBinaryPathName, 
                       PCWSTR pszServiceStartName, 
                       PCWSTR pszPassword)
{
    HRESULT hr = S_OK;
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;

    // Get full path name, and verify the path.
    wchar_t szBinaryFullPath[MAX_PATH];
    if (0 == GetFullPathName(pszBinaryPathName, ARRAYSIZE(szBinaryFullPath), 
        szBinaryFullPath, NULL))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    if (!PathFileExists(szBinaryFullPath))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(
        pszMachineName,             // Target computer name
        NULL,                       // SCM DB name
        SC_MANAGER_CREATE_SERVICE   // The access to SCM
        );
    if (NULL == schSCManager)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    // Create the service.
    schService = CreateService(
        schSCManager,               // SCM database 
        pszServiceName,             // Name of service 
        pszDisplayName,             // Service name to display 
        SERVICE_ALL_ACCESS,         // Desired access 
        SERVICE_WIN32_OWN_PROCESS,  // Service type 
        dwStartType,                // Start type 
        SERVICE_ERROR_NORMAL,       // Error control type 
        szBinaryFullPath,           // Full path to the binary
        NULL,                       // No load ordering group 
        NULL,                       // No tag identifier
        NULL,                       // No dependencies 
        pszServiceStartName,        // Service start name
        pszPassword                 // Account password
        );
    if (NULL == schService) 
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

Cleanup:
    if (schSCManager != NULL)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService != NULL)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }

    return hr;
}


//
//   FUNCTION: UninstallService
//
//   PURPOSE: Uninstall a Windows service. The specified service and the 
//   services that services that depend on the specified service will be 
//   stopped first, then the service is uninstalled. 
// 
//   PARAMETERS:
//   * pszMachineName - The name of the target computer. If the pointer is 
//     NULL or points to an empty string, the function connects to the SCM on 
//     the local computer.
//   * pszServiceName - The name of the service to start. The maximum string 
//     length is 256 characters.
//
//   RETURN VALUE: HRESULT
//
HRESULT UninstallService(PCWSTR pszMachineName, PCWSTR pszServiceName)
{
    HRESULT hr = S_OK;
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS sStatus = {};

    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(pszMachineName, NULL, 
        SC_MANAGER_ENUMERATE_SERVICE);
    if (schSCManager == NULL)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    // Open the service with delete, stop, and query status permissions.
    schService = OpenService(schSCManager, pszServiceName, DELETE | 
        SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS);
    if (schService == NULL)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    // Try to stop the service, and the services that depend on the service. 
    hr = StopService(schSCManager, schService);
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    // Now remove the service by calling DeleteService.
    if (!DeleteService(schService))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

Cleanup:
    if (schSCManager != NULL)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService != NULL)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }

    return hr;
}


//
//   FUNCTION: IsServiceInstalled
//
//   PURPOSE: Determine if the service is installed in the specified machine.
// 
//   PARAMETERS:
//   * pszMachineName - The name of the target computer. If the pointer is 
//     NULL or points to an empty string, the function connects to the SCM on 
//     the local computer.
//   * pszServiceName - The name of the service to uninstall. The maximum 
//     string length is 256 characters.
//   * pfInstalled - Pointer to a BOOL variable. If the function succeeds, 
//     the BOOL variable indicates whether the service is installed in the 
//     specified machine or not. 
//
//   RETURN VALUE: HRESULT
//
HRESULT IsServiceInstalled(PCWSTR pszMachineName, PCWSTR pszServiceName, 
                           PBOOL pfInstalled)
{
    *pfInstalled = FALSE;

    HRESULT hr = S_OK;
    SC_HANDLE schSCManager = NULL;
    ENUM_SERVICE_STATUS_PROCESS *pServices = NULL;
    DWORD dwSize = 0;
    DWORD dwBytesNeeded = 0;
    DWORD dwServicesReturned = 0;
    DWORD dwResumeHandle = 0;
    
    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(pszMachineName, NULL, 
        SC_MANAGER_ENUMERATE_SERVICE);
    if (schSCManager == NULL)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    // Pass a zero-length buffer to get the required buffer size.
    if (!EnumServicesStatusEx(schSCManager, SC_ENUM_PROCESS_INFO, 
        SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &dwBytesNeeded, 
        &dwServicesReturned, &dwResumeHandle, NULL))
    {
        DWORD dwError = GetLastError();

        if (dwError == ERROR_MORE_DATA)
        {
            // Allocate the buffer.
            dwSize = dwBytesNeeded;
            pServices = static_cast<ENUM_SERVICE_STATUS_PROCESS *>(
                LocalAlloc(LPTR, dwSize));
            if (pServices == NULL)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                goto Cleanup;
            }

            // Get the array of ENUM_SERVICE_STATUS_PROCESS structs.
            if (!EnumServicesStatusEx(schSCManager, SC_ENUM_PROCESS_INFO, 
                SERVICE_WIN32, SERVICE_STATE_ALL, 
                reinterpret_cast<LPBYTE>(pServices), dwSize, &dwBytesNeeded, 
                &dwServicesReturned, &dwResumeHandle, NULL))
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                goto Cleanup;
            }
        }
        else
        {
            hr = HRESULT_FROM_WIN32(dwError);
            goto Cleanup;
        }
    }

    // Enumerate the services, and try to find the service name.
    for (DWORD i = 0; i < dwServicesReturned; i++)
    {
        if (wcscmp(pServices[i].lpServiceName, pszServiceName) == 0)
        {
            *pfInstalled = TRUE;
            break;
        }
    }

Cleanup:
    if (schSCManager != NULL)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (pServices != NULL)
    {
        LocalFree(pServices);
        pServices = NULL;
    }
    
    return hr;
}


//
//   FUNCTION: StartService
//
//   PURPOSE: Start a Windows service.
// 
//   PARAMETERS:
//   * pszMachineName - The name of the target computer. If the pointer is 
//     NULL or points to an empty string, the function connects to the SCM on 
//     the local computer.
//   * pszServiceName - The name of the service to start. The maximum string 
//     length is 256 characters.
//
//   RETURN VALUE: HRESULT
//
HRESULT StartService(PCWSTR pszMachineName, PCWSTR pszServiceName)
{
    HRESULT hr = S_OK;
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS_PROCESS sStatus;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;

    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(pszMachineName, NULL, 
        SC_MANAGER_ENUMERATE_SERVICE);
    if (NULL == schSCManager) 
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    // Open the service.
    schService = OpenService(schSCManager, pszServiceName,
        SERVICE_START | SERVICE_QUERY_STATUS);
    if (NULL == schService)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    // Check the status in case the service is not stopped.
    if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, 
        reinterpret_cast<LPBYTE>(&sStatus), sizeof(sStatus), 
        &dwBytesNeeded))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    // Check if the service is already running. 
    if (sStatus.dwCurrentState != SERVICE_STOPPED && 
        sStatus.dwCurrentState != SERVICE_STOP_PENDING)
    {
        // The service might be already running.
        hr = S_OK;
        goto Cleanup;
    }

    // Save the tick count and initial checkpoint.
    DWORD dwStartTickCount = GetTickCount();
    DWORD dwOldCheckPoint = sStatus.dwCheckPoint;

    // Wait for the service to stop before attempting to start it.
    while (sStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        // Do not wait longer than the wait hint. A good interval is one-
        // tenth of the wait hint but not less than 1 second and not more 
        // than 10 seconds. 
        dwWaitTime = sStatus.dwWaitHint / 10;
        if (dwWaitTime < 1000)
        {
            dwWaitTime = 1000;
        }
        else if (dwWaitTime > 10000)
        {
            dwWaitTime = 10000;
        }
        Sleep(dwWaitTime);

        // Check the status until the service is no longer stop pending. 
        if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, 
            reinterpret_cast<LPBYTE>(&sStatus), sizeof(sStatus), 
            &dwBytesNeeded))
        {
            hr = HRESULT_FROM_WIN32(GetLastError()); 
            goto Cleanup;
        }

        if (sStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // Continue to wait and check.
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = sStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > sStatus.dwWaitHint)
            {
                // Timeout waiting for service to stop.
                hr = RPC_E_TIMEOUT;
                goto Cleanup;
            }
        }
    }

    // Attempt to start the service.
    if (!StartService(schService, 0, NULL))
    {
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    // Service start pending...

    // Check the status until the service is no longer start pending. 
    if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, 
        reinterpret_cast<LPBYTE>(&sStatus), sizeof(sStatus), 
        &dwBytesNeeded))
    {
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    // Save the tick count and initial checkpoint.
    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = sStatus.dwCheckPoint;

    while (sStatus.dwCurrentState == SERVICE_START_PENDING) 
    { 
        // Do not wait longer than the wait hint. A good interval is one-
        // tenth the wait hint, but no less than 1 second and no more than 10 
        // seconds.
        dwWaitTime = sStatus.dwWaitHint / 10;
        if (dwWaitTime < 1000)
        {
            dwWaitTime = 1000;
        }
        else if (dwWaitTime > 10000)
        {
            dwWaitTime = 10000;
        }
        Sleep(dwWaitTime);

        // Check the status again. 
        if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, 
            reinterpret_cast<LPBYTE>(&sStatus), sizeof(sStatus), 
            &dwBytesNeeded))
        {
            hr = HRESULT_FROM_WIN32(GetLastError()); 
            goto Cleanup;
        }

        if (sStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // Continue to wait and check.
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = sStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > sStatus.dwWaitHint)
            {
                // No progress made within the wait hint.
                break;
            }
        }
    }

    // Determine whether the service is running.
    hr = (sStatus.dwCurrentState == SERVICE_RUNNING) ? S_OK : E_FAIL;

Cleanup:
    if (schSCManager != NULL)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService != NULL)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }

    return hr;
}


//
//   FUNCTION: StopService
//
//   PURPOSE: Stop a Windows service.
// 
//   PARAMETERS:
//   * pszMachineName - The name of the target computer. If the pointer is 
//     NULL or points to an empty string, the function connects to the SCM on 
//     the local computer.
//   * pszServiceName - The name of the service to stop. The maximum string 
//     length is 256 characters.
//
//   RETURN VALUE: HRESULT
//
HRESULT StopService(PCWSTR pszMachineName, PCWSTR pszServiceName)
{
    HRESULT hr = S_OK;
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;

    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(pszMachineName, NULL, 
        SC_MANAGER_ENUMERATE_SERVICE);
    if (NULL == schSCManager) 
    {
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    // Open the service.
    schService = OpenService(schSCManager, pszServiceName,
        SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS);
    if (NULL == schService)
    {
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    hr = StopService(schSCManager, schService);

Cleanup:
    if (schSCManager != NULL)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService != NULL)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }

    return hr;
}


//
//   FUNCTION: StopService
//
//   PURPOSE: Stop a Windows service.
// 
//   PARAMETERS:
//   * schSCManager - The handle to the SCM database.
//   * schService - The handle of the service to be stopped. The handle must 
//     be opened with the SERVICE_STOP, SERVICE_QUERY_STATUS, and 
//     SERVICE_ENUMERATE_DEPENDENTS access permissions. 
//
//   RETURN VALUE: HRESULT
//
HRESULT StopService(SC_HANDLE schSCManager, SC_HANDLE schService)
{
    HRESULT hr = S_OK;
    SERVICE_STATUS_PROCESS sStatus;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;
    DWORD dwTimeout = 30000; // 30-second time-out

    // Make sure that the service is not already stopped.
    if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, 
        reinterpret_cast<LPBYTE>(&sStatus), sizeof(sStatus), 
        &dwBytesNeeded))
    {
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    if (sStatus.dwCurrentState == SERVICE_STOPPED)
    {
        // Service is already stopped.
        hr = S_OK;
        goto Cleanup;
    }

    // Save the tick count.
    DWORD dwStartTickCount = GetTickCount();

    // If a stop is pending, wait for it. If the service is successfully 
    // stopped, clean up and quit.
    while (sStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        // Service stop pending...

        // Do not wait longer than the wait hint. A good interval is one-
        // tenth of the wait hint but not less than 1 second and not more 
        // than 10 seconds. 
        dwWaitTime = sStatus.dwWaitHint / 10;
        if (dwWaitTime < 1000)
        {
            dwWaitTime = 1000;
        }
        else
        {
            dwWaitTime = 10000;
        }
        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, 
            reinterpret_cast<LPBYTE>(&sStatus), sizeof(sStatus), 
            &dwBytesNeeded))
        {
            hr = HRESULT_FROM_WIN32(GetLastError()); 
            goto Cleanup;
        }

        if (sStatus.dwCurrentState == SERVICE_STOPPED)
        {
            // Service stopped successfully.
            hr = S_OK;
            goto Cleanup;
        }

        if (GetTickCount() - dwStartTickCount > dwTimeout)
        {
            // Service stop timed out.
            hr = RPC_E_TIMEOUT;
            goto Cleanup;
        }
    }

    // If the service is running, dependencies must be stopped first.
    hr = StopDependentServices(schSCManager, schService);
    if (FAILED(hr))
    {
        // Certain dependent service failed to stop.
        goto Cleanup;
    }

    // Send a stop code to the service.
    if (!ControlService(schService, SERVICE_CONTROL_STOP, 
        reinterpret_cast<LPSERVICE_STATUS>(&sStatus)))
    {
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    // Wait for the service to stop.
    while (sStatus.dwCurrentState != SERVICE_STOPPED) 
    {
        Sleep(sStatus.dwWaitHint);

        if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, 
            reinterpret_cast<LPBYTE>(&sStatus), sizeof(sStatus), 
            &dwBytesNeeded))
        {
            hr = HRESULT_FROM_WIN32(GetLastError()); 
            goto Cleanup;
        }

        if (sStatus.dwCurrentState == SERVICE_STOPPED)
        {
            break;
        }

        if (GetTickCount() - dwStartTickCount > dwTimeout)
        {
            // Service stop timed out.
            hr = RPC_E_TIMEOUT;
            goto Cleanup;
        }
    }

Cleanup:
    return hr;
}


//
//   FUNCTION: StopDependentServices
//
//   PURPOSE: Stop the services that depends on the service specified by 
//   schService.
// 
//   PARAMETERS:
//   * schSCManager - The handle to the SCM database.
//   * schService - The handle of the service. The handle must be opened with 
//     the SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS 
//     access permissions. 
//
//   RETURN VALUE: HRESULT
//
HRESULT StopDependentServices(SC_HANDLE schSCManager, SC_HANDLE schService)
{
    HRESULT hr = S_OK;
    LPENUM_SERVICE_STATUS pDependencies = NULL;
    SERVICE_STATUS_PROCESS sStatus;
    DWORD dwBytesNeeded;
    DWORD dwCount;
    DWORD dwTimeout = 30000; // 30-second time-out

    // Pass a zero-length buffer to get the required buffer size.
    if (EnumDependentServices(schService, SERVICE_ACTIVE, 
        pDependencies, 0, &dwBytesNeeded, &dwCount))
    {
        // If the EnumDependentServices call succeeds, then there are no 
        // dependent services, so do nothing.
        hr = S_OK;
        goto Cleanup;
    }

    if (GetLastError() != ERROR_MORE_DATA)
    {
        // Unexpected error.
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    // Allocate a buffer for the dependencies.
    pDependencies = static_cast<LPENUM_SERVICE_STATUS>(
        LocalAlloc(LPTR, dwBytesNeeded));
    if (pDependencies == NULL)
    {
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    // Enumerate the services that depend on the specified service.
    if (!EnumDependentServices(schService, SERVICE_ACTIVE, 
        pDependencies, dwBytesNeeded, &dwBytesNeeded, &dwCount))
    {
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    // Save the tick count.
    DWORD dwStartTickCount = GetTickCount();

    for (DWORD i = 0; i < dwCount && SUCCEEDED(hr); i++)
    {
        ENUM_SERVICE_STATUS ess = *(pDependencies + i);

        // Open the service.
        SC_HANDLE schDepService = OpenService(schSCManager, ess.lpServiceName, 
            SERVICE_STOP | SERVICE_QUERY_STATUS);
        if (schDepService != NULL)
        {
            // Send a stop code.
            if (ControlService(schDepService, SERVICE_CONTROL_STOP, 
                reinterpret_cast<LPSERVICE_STATUS>(&sStatus)))
            {
                // Wait for the service to stop.
                while (sStatus.dwCurrentState != SERVICE_STOPPED)
                {
                    Sleep(sStatus.dwWaitHint);

                    if (QueryServiceStatusEx(schDepService, 
                        SC_STATUS_PROCESS_INFO, 
                        reinterpret_cast<LPBYTE>(&sStatus), sizeof(sStatus), 
                        &dwBytesNeeded))
                    {
                        if (sStatus.dwCurrentState == SERVICE_STOPPED)
                        {
                            hr = S_OK;
                            break;
                        }

                        if (GetTickCount() - dwStartTickCount > dwTimeout)
                        {
                            // Service stop timed out.
                            hr = RPC_E_TIMEOUT;
                            break;
                        }
                    }
                }
            }

            CloseServiceHandle(schDepService);
            schDepService = NULL;
        }
    }

Cleanup:
    if (pDependencies != NULL)
    {
        LocalFree(pDependencies);
        pDependencies = NULL;
    }

    return hr;
}


//
//   FUNCTION: UpdateServiceDACL
//
//   PURPOSE: Update the service DACL to grant start, stop, delete and read 
//   control access to all authenticated users. 
// 
//   PARAMETERS:
//   * pszMachineName - The name of the target computer. If the pointer is 
//     NULL or points to an empty string, the function connects to the SCM on 
//     the local computer.
//   * pszServiceName - The name of the service to update the DACL. The 
//     maximum string length is 256 characters.
//
//   RETURN VALUE: HRESULT
//
HRESULT UpdateServiceDACL(PCWSTR pszMachineName, PCWSTR pszServiceName)
{
    HRESULT hr = S_OK;
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SECURITY_DESCRIPTOR sd;
    PSECURITY_DESCRIPTOR psd = NULL;
    PACL pAcl = NULL;
    PACL pNewAcl = NULL;
    DWORD dwError = 0;
    DWORD dwSize = 0;
    DWORD dwBytesNeeded = 0;
    BOOL fDaclPresent = FALSE;
    BOOL fDaclDefaulted = FALSE;
    EXPLICIT_ACCESS ea;

    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(pszMachineName, NULL, 
        SC_MANAGER_ENUMERATE_SERVICE);
    if (NULL == schSCManager) 
    {
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    // Open the service.
    schService = OpenService(schSCManager, pszServiceName, 
        READ_CONTROL | WRITE_DAC);
    if (NULL == schService)
    {
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    // Get the size of the current security descriptor.
    if (!QueryServiceObjectSecurity(schService, DACL_SECURITY_INFORMATION, 
        psd, 0, &dwBytesNeeded))
    {
        dwError = GetLastError();
        if (dwError == ERROR_INSUFFICIENT_BUFFER)
        {
            // Allocate the memory.
            dwSize = dwBytesNeeded;
            psd = static_cast<PSECURITY_DESCRIPTOR>(LocalAlloc(LPTR, dwSize));
            if (psd == NULL)
            {
                hr = HRESULT_FROM_WIN32(GetLastError()); 
                goto Cleanup;
            }

            // Get the current security descriptor.
            if (!QueryServiceObjectSecurity(schService, 
                DACL_SECURITY_INFORMATION, psd, dwSize, &dwBytesNeeded))
            {
                hr = HRESULT_FROM_WIN32(GetLastError()); 
                goto Cleanup;
            }
        }
        else
        {
            hr = HRESULT_FROM_WIN32(dwError);
            goto Cleanup;
        }
    }

    // Get the DACL.
    if (!GetSecurityDescriptorDacl(psd, &fDaclPresent, &pAcl, 
        &fDaclDefaulted))
    {
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        goto Cleanup;
    }

    // Build the ACE.
    BuildExplicitAccessWithName(&ea, L"USERS", 
        SERVICE_START | SERVICE_STOP | READ_CONTROL | DELETE, 
        SET_ACCESS, NO_INHERITANCE);

    hr = HRESULT_FROM_WIN32(SetEntriesInAcl(1, &ea, pAcl, &pNewAcl));
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    // Initialize a new security descriptor.
    if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    // Set the new DACL in the security descriptor.
    if (!SetSecurityDescriptorDacl(&sd, TRUE, pNewAcl, FALSE))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    // Set the new DACL for the service object.
    if (!SetServiceObjectSecurity(schService, DACL_SECURITY_INFORMATION, &sd))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

Cleanup:
    if (schSCManager != NULL)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService != NULL)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
    if (psd != NULL)
    {
        LocalFree(psd);
        psd = NULL;
    }
    if (pNewAcl != NULL)
    {
        LocalFree(pNewAcl);
        pNewAcl = NULL;
    }

    return hr;
}