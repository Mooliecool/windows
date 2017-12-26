/********************************* Module Header *********************************\
Module Name:  NativeMethods.cs
Project:      CSControlWindowsService
Copyright (c) Microsoft Corporation.

The P/Invoke signatures of native service APIs and types.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*********************************************************************************/

using System;
using System.Security;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;


namespace CSControlWindowsService
{
    internal class NativeMethods
    {
        /// <summary>
        /// Establishes a connection to the service control manager on the 
        /// specified computer and opens the specified service control manager 
        /// database.
        /// </summary>
        /// <param name="machineName">Name of the target computer.</param>
        /// <param name="databaseName">
        /// Name of the service control manager database.
        /// </param>
        /// <param name="dwAccess">
        /// The access to the service control manager.
        /// </param>
        /// <returns>
        /// If the function succeeds, the return value is a handle to the specified
        /// service control manager database. If the function fails, the return 
        /// value is an invalid handle. To get extended error information, call 
        /// GetLastError.
        /// </returns>
        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern SafeServiceHandle OpenSCManager(
            string lpMachineName,
            string lpDatabaseName,
            ServiceControlAccessRights dwAccess);


        /// <summary>
        /// Creates a service object and adds it to the specified service control 
        /// manager database.
        /// </summary>
        /// <param name="hSCManager">
        /// A handle to the service control manager database. This handle is 
        /// returned by the OpenSCManager function and must have the 
        /// SC_MANAGER_CREATE_SERVICE access right.
        /// </param>
        /// <param name="lpServiceName">
        /// The name of the service to install. The maximum string length is 256 
        /// characters.
        /// </param>
        /// <param name="lpDisplayName">
        /// The display name to be used by user interface programs to identify the 
        /// service. This string has a maximum length of 256 characters.
        /// </param>
        /// <param name="dwDesiredAccess">
        /// The access to the service. Before granting the requested access, the 
        /// system checks the access token of the calling process.
        /// </param>
        /// <param name="dwServiceType">
        /// The service type. 
        /// </param>
        /// <param name="dwStartType">
        /// The service start options. 
        /// </param>
        /// <param name="dwErrorControl">
        /// The severity of the error, and action taken, if this service fails to 
        /// start.
        /// </param>
        /// <param name="lpBinaryPathName">
        /// The fully-qualified path to the service binary file. 
        /// </param>
        /// <param name="lpLoadOrderGroup">
        /// The names of the load ordering group of which this service is a member. 
        /// Specify null or an empty string if the service does not belong to a 
        /// group. 
        /// </param>
        /// <param name="lpdwTagId">
        /// A pointer to a variable that receives a tag value that is unique in the 
        /// group specified in the lpLoadOrderGroup parameter. Specify null if you 
        /// are not changing the existing tag.
        /// </param>
        /// <param name="lpDependencies">
        /// A pointer to a double null-terminated array of null-separated names of 
        /// services or load ordering groups that the system must start before 
        /// this service. Specify null or an empty string if the service has no 
        /// dependencies.
        /// </param>
        /// <param name="lpServiceStartName">
        /// The name of the account under which the service should run. 
        /// If this parameter is null, CreateService uses the LocalSystem account.
        /// If this parameter is NT AUTHORITY\LocalService, CreateService uses the 
        /// LocalService account. If the parameter is NT AUTHORITY\NetworkService, 
        /// CreateService uses the NetworkService account.
        /// </param>
        /// <param name="lpPassword">
        /// The password to the account name specified by the lpServiceStartName 
        /// parameter. Specify an empty string if the account has no password or 
        /// if the service runs in the LocalService, NetworkService, or 
        /// LocalSystem account.
        /// </param>
        /// <returns>
        /// If the function succeeds, the return value is a handle to the service.
        /// If the function fails, the return value is an invalid handle. To get 
        /// extended error information, call GetLastError.
        /// </returns>
        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern SafeServiceHandle CreateService(
            SafeServiceHandle hSCManager,
            string lpServiceName,
            string lpDisplayName,
            ServiceAccessRights dwDesiredAccess,
            ServiceType dwServiceType,
            ServiceStart dwStartType,
            ServiceError dwErrorControl,
            string lpBinaryPathName,
            string lpLoadOrderGroup,
            string lpdwTagId,
            string lpDependencies,
            string lpServiceStartName,
            string lpPassword);


        /// <summary>
        /// Opens an existing service.
        /// </summary>
        /// <param name="hSCManager">
        /// A handle to the service control manager database. The OpenSCManager 
        /// function returns this handle.
        /// </param>
        /// <param name="lpServiceName">
        /// The name of the service to be opened.
        /// </param>
        /// <param name="dwDesiredAccess">The access to the service.</param>
        /// <returns>
        /// If the function succeeds, the return value is a handle to the specified 
        /// service. If the function fails, the return value is an invalid handle. 
        /// To get extended error information, call GetLastError.
        /// </returns>
        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern SafeServiceHandle OpenService(
            SafeServiceHandle hSCManager,
            string lpServiceName,
            ServiceAccessRights dwDesiredAccess);


        /// <summary>
        /// Marks the specified service for deletion from the service control 
        /// manager database.
        /// </summary>
        /// <param name="hService">
        /// A handle to the service. This handle is returned by the OpenService or 
        /// CreateService function, and it must have the DELETE access right. 
        /// </param>
        /// <returns>
        /// If the function succeeds, the return value is true.
        /// If the function fails, the return value is false. To get extended error 
        /// information, call GetLastError.
        /// </returns>
        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool DeleteService(SafeServiceHandle hService);


        /// <summary>
        /// Closes a handle to a service control manager or service object.
        /// </summary>
        /// <param name="hSCObject">
        /// A handle to the service control manager object or the service object 
        /// to close.
        /// </param>
        /// <returns>
        /// If the function succeeds, the return value is true.
        /// If the function fails, the return value is false. To get extended error 
        /// information, call GetLastError.
        /// </returns>
        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CloseServiceHandle(IntPtr hSCObject);
    }


    [SuppressUnmanagedCodeSecurity]
    internal class SafeServiceHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        internal SafeServiceHandle()
            : base(true)
        {
        }

        protected override bool ReleaseHandle()
        {
            return NativeMethods.CloseServiceHandle(base.handle);
        }
    }


    /// <summary>
    /// The AccessMask data type defines standard, specific, and generic rights.
    /// </summary>
    /// <see cref="http://msdn.microsoft.com/en-us/library/aa374892.aspx"/>
    [Flags]
    enum AccessMask : uint
    {
        DELETE = 0x00010000,
        READ_CONTROL = 0x00020000,
        WRITE_DAC = 0x00040000,
        WRITE_OWNER = 0x00080000,
        SYNCHRONIZE = 0x00100000,

        STANDARD_RIGHTS_REQUIRED = 0x000f0000,

        STANDARD_RIGHTS_READ = 0x00020000,
        STANDARD_RIGHTS_WRITE = 0x00020000,
        STANDARD_RIGHTS_EXECUTE = 0x00020000,

        STANDARD_RIGHTS_ALL = 0x001f0000,

        SPECIFIC_RIGHTS_ALL = 0x0000ffff,

        ACCESS_SYSTEM_SECURITY = 0x01000000
    }


    [Flags]
    internal enum ServiceControlAccessRights : uint
    {
        /// <summary>
        /// Required to connect to the service control manager.
        /// </summary>
        SC_MANAGER_CONNECT = 0x0001,

        /// <summary>
        /// Required to call the CreateService function to create a service object 
        /// and add it to the database.
        /// </summary>
        SC_MANAGER_CREATE_SERVICE = 0x0002,

        /// <summary>
        /// Required to call the EnumServicesStatusEx function to list the services
        /// that are in the database. 
        /// </summary>
        SC_MANAGER_ENUMERATE_SERVICE = 0x0004,

        /// <summary>
        /// Required to call the LockServiceDatabase function to acquire a lock on 
        /// the database. 
        /// </summary>
        SC_MANAGER_LOCK = 0x0008,

        /// <summary>
        /// Required to call the QueryServiceLockStatus function to retrieve the 
        /// lock status information for the database
        /// </summary>
        SC_MANAGER_QUERY_LOCK_STATUS = 0x0010,

        /// <summary>
        /// Required to call the NotifyBootConfigStatus function. 
        /// </summary>
        SC_MANAGER_MODIFY_BOOT_CONFIG = 0x0020,

        /// <summary>
        /// Includes STANDARD_RIGHTS_REQUIRED, in addition to all access rights in 
        /// this table. 
        /// </summary>
        SC_MANAGER_ALL_ACCESS = (AccessMask.STANDARD_RIGHTS_REQUIRED |
            SC_MANAGER_CONNECT |
            SC_MANAGER_CREATE_SERVICE |
            SC_MANAGER_ENUMERATE_SERVICE |
            SC_MANAGER_LOCK |
            SC_MANAGER_QUERY_LOCK_STATUS |
            SC_MANAGER_MODIFY_BOOT_CONFIG),

        GENERIC_READ = (AccessMask.STANDARD_RIGHTS_READ |
            SC_MANAGER_ENUMERATE_SERVICE |
            SC_MANAGER_QUERY_LOCK_STATUS),

        GENERIC_WRITE = (AccessMask.STANDARD_RIGHTS_WRITE |
            SC_MANAGER_CREATE_SERVICE |
            SC_MANAGER_MODIFY_BOOT_CONFIG),

        GENERIC_EXECUTE = (AccessMask.STANDARD_RIGHTS_EXECUTE |
            SC_MANAGER_CONNECT | SC_MANAGER_LOCK),

        GENERIC_ALL = SC_MANAGER_ALL_ACCESS
    }


    /// <summary>
    /// Access to the service. Before granting the requested access, the system 
    /// checks the access token of the calling process.
    /// </summary>
    [Flags]
    internal enum ServiceAccessRights : uint
    {
        /// <summary>
        /// Required to call the QueryServiceConfig and 
        /// QueryServiceConfig2 functions to query the service configuration.
        /// </summary>
        SERVICE_QUERY_CONFIG = 0x00001,

        /// <summary>
        /// Required to call the ChangeServiceConfig or ChangeServiceConfig2 function 
        /// to change the service configuration. Because this grants the caller 
        /// the right to change the executable file that the system runs, 
        /// it should be granted only to administrators.
        /// </summary>
        SERVICE_CHANGE_CONFIG = 0x00002,

        /// <summary>
        /// Required to call the QueryServiceStatusEx function to ask the service 
        /// control manager about the status of the service.
        /// </summary>
        SERVICE_QUERY_STATUS = 0x00004,

        /// <summary>
        /// Required to call the EnumDependentServices function to enumerate all 
        /// the services dependent on the service.
        /// </summary>
        SERVICE_ENUMERATE_DEPENDENTS = 0x00008,

        /// <summary>
        /// Required to call the StartService function to start the service.
        /// </summary>
        SERVICE_START = 0x00010,

        /// <summary>
        ///     Required to call the ControlService function to stop the service.
        /// </summary>
        SERVICE_STOP = 0x00020,

        /// <summary>
        /// Required to call the ControlService function to pause or continue 
        /// the service.
        /// </summary>
        SERVICE_PAUSE_CONTINUE = 0x00040,

        /// <summary>
        /// Required to call the EnumDependentServices function to enumerate all
        /// the services dependent on the service.
        /// </summary>
        SERVICE_INTERROGATE = 0x00080,

        /// <summary>
        /// Required to call the ControlService function to specify a user-defined
        /// control code.
        /// </summary>
        SERVICE_USER_DEFINED_CONTROL = 0x00100,

        /// <summary>
        /// Includes STANDARD_RIGHTS_REQUIRED in addition to all access rights in 
        /// this table.
        /// </summary>
        SERVICE_ALL_ACCESS = (AccessMask.STANDARD_RIGHTS_REQUIRED |
            SERVICE_QUERY_CONFIG |
            SERVICE_CHANGE_CONFIG |
            SERVICE_QUERY_STATUS |
            SERVICE_ENUMERATE_DEPENDENTS |
            SERVICE_START |
            SERVICE_STOP |
            SERVICE_PAUSE_CONTINUE |
            SERVICE_INTERROGATE |
            SERVICE_USER_DEFINED_CONTROL),

        GENERIC_READ = AccessMask.STANDARD_RIGHTS_READ |
            SERVICE_QUERY_CONFIG |
            SERVICE_QUERY_STATUS |
            SERVICE_INTERROGATE |
            SERVICE_ENUMERATE_DEPENDENTS,

        GENERIC_WRITE = AccessMask.STANDARD_RIGHTS_WRITE |
            SERVICE_CHANGE_CONFIG,

        GENERIC_EXECUTE = AccessMask.STANDARD_RIGHTS_EXECUTE |
            SERVICE_START |
            SERVICE_STOP |
            SERVICE_PAUSE_CONTINUE |
            SERVICE_USER_DEFINED_CONTROL,

        /// <summary>
        /// Required to call the QueryServiceObjectSecurity or 
        /// SetServiceObjectSecurity function to access the SACL. The proper
        /// way to obtain this access is to enable the SE_SECURITY_NAME 
        /// privilege in the caller's current access token, open the handle 
        /// for ACCESS_SYSTEM_SECURITY access, and then disable the privilege.
        /// </summary>
        ACCESS_SYSTEM_SECURITY = AccessMask.ACCESS_SYSTEM_SECURITY,

        /// <summary>
        /// Required to call the DeleteService function to delete the service.
        /// </summary>
        DELETE = AccessMask.DELETE,

        /// <summary>
        /// Required to call the QueryServiceObjectSecurity function to query
        /// the security descriptor of the service object.
        /// </summary>
        READ_CONTROL = AccessMask.READ_CONTROL,

        /// <summary>
        /// Required to call the SetServiceObjectSecurity function to modify
        /// the Dacl member of the service object's security descriptor.
        /// </summary>
        WRITE_DAC = AccessMask.WRITE_DAC,

        /// <summary>
        /// Required to call the SetServiceObjectSecurity function to modify 
        /// the Owner and Group members of the service object's security 
        /// descriptor.
        /// </summary>
        WRITE_OWNER = AccessMask.WRITE_OWNER,
    }


    /// <summary>
    /// Service types.
    /// </summary>
    [Flags]
    public enum ServiceType : uint
    {
        /// <summary>
        /// Driver service.
        /// </summary>
        SERVICE_KERNEL_DRIVER = 0x00000001,

        /// <summary>
        /// File system driver service.
        /// </summary>
        SERVICE_FILE_SYSTEM_DRIVER = 0x00000002,

        /// <summary>
        /// Service that runs in its own process.
        /// </summary>
        SERVICE_WIN32_OWN_PROCESS = 0x00000010,

        /// <summary>
        /// Service that shares a process with one or more other services.
        /// </summary>
        SERVICE_WIN32_SHARE_PROCESS = 0x00000020,

        /// <summary>
        /// The service can interact with the desktop.
        /// </summary>
        SERVICE_INTERACTIVE_PROCESS = 0x00000100,
    }


    /// <summary>
    /// Service start options
    /// </summary>
    public enum ServiceStart : uint
    {
        /// <summary>
        /// A device driver started by the system loader. This value is valid only 
        /// for driver services.
        /// </summary>
        SERVICE_BOOT_START = 0x00000000,

        /// <summary>
        /// A device driver started by the IoInitSystem function. This value is 
        /// valid only for driver services.
        /// </summary>
        SERVICE_SYSTEM_START = 0x00000001,

        /// <summary>
        /// A service started automatically by the service control manager during 
        /// system startup. For more information, see Automatically Starting 
        /// Services.
        /// </summary>         
        SERVICE_AUTO_START = 0x00000002,

        /// <summary>
        /// A service started by the service control manager when a process calls
        /// the StartService function. For more information, see Starting Services
        /// on Demand.
        /// </summary>
        SERVICE_DEMAND_START = 0x00000003,

        /// <summary>
        /// A service that cannot be started. Attempts to start the service result 
        /// in the error code ERROR_SERVICE_DISABLED.
        /// </summary>
        SERVICE_DISABLED = 0x00000004,
    }


    /// <summary>
    /// Severity of the error, and action taken, if this service fails to start.
    /// </summary>
    public enum ServiceError
    {
        /// <summary>
        /// The startup program ignores the error and continues the startup 
        /// operation.
        /// </summary>
        SERVICE_ERROR_IGNORE = 0x00000000,

        /// <summary>
        /// The startup program logs the error in the event log but continues the 
        /// startup operation.
        /// </summary>
        SERVICE_ERROR_NORMAL = 0x00000001,

        /// <summary>
        /// The startup program logs the error in the event log. If the last-known-
        /// good configuration is being started, the startup operation continues.
        /// Otherwise, the system is restarted with the last-known-good 
        /// configuration.
        /// </summary>
        SERVICE_ERROR_SEVERE = 0x00000002,

        /// <summary>
        /// The startup program logs the error in the event log, if possible. If 
        /// the last-known-good configuration is being started, the startup 
        /// operation fails. Otherwise, the system is restarted with the last-
        /// known good configuration.
        /// </summary>
        SERVICE_ERROR_CRITICAL = 0x00000003
    }
}