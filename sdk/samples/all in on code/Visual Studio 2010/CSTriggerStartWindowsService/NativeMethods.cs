/********************************* Module Header *********************************\
* Module Name:  NativeMethods.cs
* Project:      CSTriggerStartWindowsService
* Copyright (c) Microsoft Corporation.
* 
* The P/Invoke signatures of native service APIs and types.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*********************************************************************************/

#region Using directives
using System;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;
using System.Security;
#endregion


namespace CSTriggerStartWindowsService
{
    static class NativeMethods
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
            string machineName,
            string databaseName, 
            ServiceControlAccessRights dwAccess);


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
        /// Changes the optional configuration parameters of a service.
        /// </summary>
        /// <param name="hService">A handle to the service.</param>
        /// <param name="dwInfoLevel">
        /// The configuration information to be changed. 
        /// </param>
        /// <param name="lpInfo"></param>
        /// <returns></returns>
        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool ChangeServiceConfig2(SafeServiceHandle hService,
            ServiceConfig2InfoLevel dwInfoLevel, IntPtr lpInfo);


        /// <summary>
        /// Retrieves the optional configuration parameters of the specified 
        /// service.
        /// </summary>
        /// <param name="hService">A handle to the service.</param>
        /// <param name="dwInfoLevel">
        /// The configuration information to be queried.
        /// </param>
        /// <param name="lpBuffer">
        /// A pointer to the buffer that receives the service configuration 
        /// information. 
        /// </param>
        /// <param name="cbBufSize">
        /// The size of the structure pointed to by the lpBuffer parameter, in 
        /// bytes.
        /// </param>
        /// <param name="pcbBytesNeeded">
        /// A pointer to a variable that receives the number of bytes required to 
        /// store the configuration information, if the function fails with 
        /// ERROR_INSUFFICIENT_BUFFER.
        /// </param>
        /// <returns>
        /// If the function succeeds, the return value is nonzero. If the 
        /// function fails, the return value is zero. To get extended error 
        /// information, call GetLastError.
        /// </returns>
        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool QueryServiceConfig2(SafeServiceHandle hService,
            ServiceConfig2InfoLevel dwInfoLevel, IntPtr lpBuffer, int cbBufSize,
            out int pcbBytesNeeded);


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
    class SafeServiceHandle : SafeHandleZeroOrMinusOneIsInvalid
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


    [StructLayout(LayoutKind.Sequential)]
    struct SERVICE_TRIGGER_SPECIFIC_DATA_ITEM
    {
        /// <summary>
        /// The data type of the trigger-specific data pointed to by pData.
        /// </summary>
        public ServiceTriggerDataType dwDataType;

        /// <summary>
        /// The size of the trigger-specific data pointed to pData, in bytes. 
        /// The maximum value is 1024.
        /// </summary>
        public uint cbData;

        /// <summary>
        /// A pointer to the trigger-specific data for the service trigger event.
        /// Strings must be Unicode; ANSI strings are not supported.
        /// </summary>
        public System.IntPtr pData;
    }


    [StructLayout(LayoutKind.Sequential)]
    struct SERVICE_TRIGGER
    {
        /// <summary>
        /// The trigger event type.
        /// </summary>
        public ServiceTriggerType dwTriggerType;

        /// <summary>
        /// The action to take when the specified trigger event occurs.
        /// </summary>
        public ServiceTriggerAction dwAction;

        /// <summary>
        /// Points to a GUID that identifies the trigger event subtype. The value
        /// of this member depends on the value of the dwTriggerType member.
        /// </summary>
        public IntPtr pTriggerSubtype;

        /// <summary>
        /// The number of SERVICE_TRIGGER_SPECIFIC_DATA_ITEM structures in the 
        /// array pointed to by pDataItems. 
        /// </summary>
        public uint cDataItems;

        /// <summary>
        /// A pointer to an array of SERVICE_TRIGGER_SPECIFIC_DATA_ITEM 
        /// structures that contain trigger-specific data. 
        /// </summary>
        public IntPtr pDataItems;
    }


    [StructLayout(LayoutKind.Sequential)]
    struct SERVICE_TRIGGER_INFO
    {
        /// <summary>
        /// The number of triggers in the array of SERVICE_TRIGGER structures 
        /// pointed to by the pTriggers member. 
        /// </summary>
        public uint cTriggers;

        /// <summary>
        /// A pointer to an array of SERVICE_TRIGGER structures that specify the 
        /// trigger events for the service. 
        /// </summary>
        public IntPtr pTriggers;

        /// <summary>
        /// This member is reserved and must be NULL.
        /// </summary>
        public IntPtr pReserved;
    }


    enum ServiceConfig2InfoLevel : uint
    {
        SERVICE_CONFIG_DESCRIPTION = 0x00000001,
        SERVICE_CONFIG_FAILURE_ACTIONS = 0x00000002,
        SERVICE_CONFIG_DELAYED_AUTO_START_INFO = 0x00000003,
        SERVICE_CONFIG_FAILURE_ACTIONS_FLAG = 0x00000004,
        SERVICE_CONFIG_SERVICE_SID_INFO = 0x00000005,
        SERVICE_CONFIG_REQUIRED_PRIVILEGES_INFO = 0x00000006,
        SERVICE_CONFIG_PRESHUTDOWN_INFO = 0x00000007,
        SERVICE_CONFIG_TRIGGER_INFO = 0x00000008,
        SERVICE_CONFIG_PREFERRED_NODE = 0x00000009
    }


    enum ServiceTriggerType : uint
    {
        /// <summary>
        /// The event is triggered when a device of the specified device interface 
        /// class arrives or is present when the system starts. This trigger event 
        /// is commonly used to start a service.
        /// </summary>
        SERVICE_TRIGGER_TYPE_DEVICE_INTERFACE_ARRIVAL = 1,

        /// <summary>
        /// The event is triggered when the first IP address on the TCP/IP 
        /// networking stack becomes available or the last IP address on the stack 
        /// becomes unavailable. This trigger event can be used to start or stop a 
        /// service.
        /// </summary>
        SERVICE_TRIGGER_TYPE_IP_ADDRESS_AVAILABILITY = 2,

        /// <summary>
        /// The event is triggered when the computer joins or leaves a domain. 
        /// This trigger event can be used to start or stop a service.
        /// </summary>
        SERVICE_TRIGGER_TYPE_DOMAIN_JOIN = 3,

        /// <summary>
        /// The event is triggered when a firewall port is opened or approximately 
        /// 60 seconds after the firewall port is closed. This trigger event can 
        /// be used to start or stop a service.
        /// </summary>
        SERVICE_TRIGGER_TYPE_FIREWALL_PORT_EVENT = 4,

        /// <summary>
        /// The event is triggered when a machine policy or user policy change 
        /// occurs. This trigger event is commonly used to start a service.
        /// </summary>
        SERVICE_TRIGGER_TYPE_GROUP_POLICY = 5,

        /// <summary>
        /// The event is a custom event generated by an Event Tracing for Windows 
        /// (ETW) provider. This trigger event can be used to start or stop a 
        /// service.
        /// </summary>
        SERVICE_TRIGGER_TYPE_CUSTOM = 20
    }


    internal enum ServiceTriggerAction : uint
    {
        /// <summary>
        /// Start the service when the specified trigger event occurs.
        /// </summary>
        SERVICE_TRIGGER_ACTION_SERVICE_START = 1,

        /// <summary>
        /// Stop the service when the specified trigger event occurs.
        /// </summary>
        SERVICE_TRIGGER_ACTION_SERVICE_STOP = 2
    }


    internal enum ServiceTriggerDataType : uint
    {
        /// <summary>
        /// The trigger-specific data is in binary format.
        /// </summary>
        SERVICE_TRIGGER_DATA_TYPE_BINARY = 1,

        /// <summary>
        /// The trigger-specific data is in string format.
        /// </summary>
        SERVICE_TRIGGER_DATA_TYPE_STRING = 2
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

        GENERIC_READ = (AccessMask.STANDARD_RIGHTS_READ |
            SERVICE_QUERY_CONFIG |
            SERVICE_QUERY_STATUS |
            SERVICE_INTERROGATE |
            SERVICE_ENUMERATE_DEPENDENTS),

        GENERIC_WRITE = (AccessMask.STANDARD_RIGHTS_WRITE |
            SERVICE_CHANGE_CONFIG),

        GENERIC_EXECUTE = (AccessMask.STANDARD_RIGHTS_EXECUTE |
            SERVICE_START |
            SERVICE_STOP |
            SERVICE_PAUSE_CONTINUE |
            SERVICE_USER_DEFINED_CONTROL),

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
        WRITE_OWNER = AccessMask.WRITE_OWNER
    }
}
