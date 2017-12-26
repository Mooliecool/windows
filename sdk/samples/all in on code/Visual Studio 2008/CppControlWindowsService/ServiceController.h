/****************************** Module Header ******************************\
Module Name:  ServiceController.h
Project:      CppControlWindowsService
Copyright (c) Microsoft Corporation.

The file declares the helper functions for installing, uninstalling, finding, 
starting and stopping a Windows service. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once

#include <stdio.h>
#include <windows.h>


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
//   * pszBinaryPathName - The path to the service binary file. 
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
                       PCWSTR pszPassword);


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
//   * pszServiceName - The name of the service to uninstall. The maximum 
//     string length is 256 characters.
//
//   RETURN VALUE: HRESULT
//
HRESULT UninstallService(PCWSTR pszMachineName, PCWSTR pszServiceName);


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
                           PBOOL pfInstalled);


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
HRESULT StartService(PCWSTR pszMachineName, PCWSTR pszServiceName);


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
HRESULT StopService(PCWSTR pszMachineName, PCWSTR pszServiceName);


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
HRESULT UpdateServiceDACL(PCWSTR pszMachineName, PCWSTR pszServiceName);