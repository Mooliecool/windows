/********************************* Module Header *********************************\
Module Name:  ServiceController.cs
Project:      CSControlWindowsService
Copyright (c) Microsoft Corporation.

The file defines the helper methods for installing, uninstalling, starting, 
stopping, and updating the DACL of a Windows service. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*********************************************************************************/

using System;
using System.ServiceProcess;
using System.ComponentModel;


namespace CSControlWindowsService
{
    static class ServiceControllerEx
    {
        /// <summary>
        /// Install a Windows service.
        /// </summary>
        /// <param name="machineName">
        /// The name of the target computer. If the value is null or an empty 
        /// string, the function connects to the SCM on the local machine.
        /// </param>
        /// <param name="serviceName">
        /// The name of the service to install. The maximum string length is 256 
        /// characters.
        /// </param>
        /// <param name="displayName">
        /// The display name to be used by user interface programs to identify the 
        /// service. This string has a maximum length of 256 chars. 
        /// </param>
        /// <param name="dwStartType">
        /// The service start options. The parameter can be one of the following 
        /// values: SERVICE_AUTO_START, SERVICE_BOOT_START, SERVICE_DEMAND_START, 
        /// SERVICE_DISABLED, SERVICE_SYSTEM_START.
        /// </param>
        /// <param name="binaryPath">
        /// The full path to the service binary file. 
        /// </param>
        /// <param name="serviceStartName">
        /// The name of the account under which the service should run. If this 
        /// parameter is NULL, CreateService uses the LocalSystem account. The 
        /// parameter can also be "NT AUTHORITY\LocalService", 
        /// "NT AUTHORITY\NetworkService", or in the form of "DomainName\UserName", 
        /// or ".\UserName".
        /// </param>
        /// <param name="password">
        /// The password to the account name specified by the serviceStartName 
        /// parameter. Specify an empty string if the account has no password or if 
        /// the service runs in the LocalService, NetworkService, or LocalSystem 
        /// account.
        /// </param>
        /// <returns></returns>
        public static void InstallService(string machineName, string serviceName,
            string displayName, ServiceStart dwStartType, string binaryPath, 
            string serviceStartName, string password)
        {
            SafeServiceHandle schSCManager = null;
            SafeServiceHandle schService = null;

            try
            {
                // Get a handle to the SCM database.
                schSCManager = NativeMethods.OpenSCManager(
                    machineName,
                    null,
                    ServiceControlAccessRights.SC_MANAGER_CREATE_SERVICE);
                if (schSCManager.IsInvalid)
                {
                    throw new Win32Exception();
                }

                // Create the service.
                schService = NativeMethods.CreateService(
                    schSCManager,
                    serviceName,
                    displayName,
                    ServiceAccessRights.SERVICE_ALL_ACCESS,
                    ServiceType.SERVICE_WIN32_OWN_PROCESS,
                    dwStartType, 
                    ServiceError.SERVICE_ERROR_NORMAL,
                    binaryPath, 
                    null, 
                    null, 
                    null, 
                    serviceStartName, 
                    password);
                if (schService.IsInvalid)
                {
                    throw new Win32Exception();
                }
            }
            finally
            {
                if (schSCManager != null)
                {
                    schSCManager.Close();
                }
                if (schService != null)
                {
                    schService.Close();
                }
            }
        }


        /// <summary>
        /// Uninstall a Windows service. The specified service and the services 
        /// that services that depend on the specified service will be stopped 
        /// first, then the service is uninstalled. 
        /// </summary>
        /// <param name="machineName">
        /// The name of the target computer. If the value is null or an empty 
        /// string, the function connects to the SCM on the local machine.
        /// </param>
        /// <param name="serviceName">
        /// The name of the service to uninstall. The maximum string length is 256 
        /// characters.
        /// </param>
        public static void UninstallService(string machineName, string serviceName)
        {
            // Try to stop the service and the services that depend on the service. 
            ServiceController service = new ServiceController(serviceName, machineName);
            if (service.Status != ServiceControllerStatus.Stopped)
            {
                service.Stop();
                service.WaitForStatus(ServiceControllerStatus.Stopped, 
                    new TimeSpan(0, 0, 30));
            }

            SafeServiceHandle schSCManager = null;
            SafeServiceHandle schService = null;

            try
            {
                // Get a handle to the SCM database.
                schSCManager = NativeMethods.OpenSCManager(
                    machineName,
                    null,
                    ServiceControlAccessRights.SC_MANAGER_CONNECT);
                if (schSCManager.IsInvalid)
                {
                    throw new Win32Exception();
                }

                // Open the service with the delete permission.
                schService = NativeMethods.OpenService(
                    schService, 
                    serviceName, 
                    ServiceAccessRights.SERVICE_STOP);
                if (schService.IsInvalid)
                {
                    throw new Win32Exception();
                }

                // Now remove the service by calling DeleteService.
                if (!NativeMethods.DeleteService(schService))
                {
                    throw new Win32Exception();
                }
            }
            finally
            {
                if (schSCManager != null)
                {
                    schSCManager.Close();
                }
                if (schService != null)
                {
                    schService.Close();
                }
            }
        }


        /// <summary>
        /// Update the service DACL to grant start, stop, delete and read control 
        /// access to all authenticated users. 
        /// </summary>
        /// <param name="machineName">
        /// The name of the target computer. If the value is null or an empty 
        /// string, the function connects to the SCM on the local machine.
        /// </param>
        /// <param name="serviceName">
        /// The name of the service to update the DACL. The maximum string length 
        /// is 256 characters.
        /// </param>
        public static void UpdateServiceDACL(string machineName, string serviceName)
        {
            
        }


        /// <summary>
        /// 
        /// </summary>
        /// <param name="machineName"></param>
        /// <param name="serviceName"></param>
        /// <returns></returns>
        public static bool IsServiceInstalled(string machineName, string serviceName)
        {
            // Get the list of Windows services.
            ServiceController[] services = ServiceController.GetServices(machineName);

            // Try to find the service name.
            foreach (ServiceController service in services)
            {
                if (service.ServiceName == serviceName)
                {
                    return true;
                }
            }
            return false;
        }
    }
}