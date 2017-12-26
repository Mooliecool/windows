/********************************* Module Header *********************************\
* Module Name:  NativeMethods.cs
* Project:      CSWin7TriggerStartService
* Copyright (c) Microsoft Corporation.
* 
* The P/Invoke signatures of native service APIs and structs.
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


namespace CSWin7TriggerStartService
{
    [Flags]
    internal enum ServiceControlAccessRights : int
    {
        // Required to connect to the service control manager.
        SC_MANAGER_CONNECT = 0x0001,

        // Required to call the CreateService function to create a service 
        // object and add it to the database. 
        SC_MANAGER_CREATE_SERVICE = 0x0002,

        // Required to call the EnumServicesStatusEx function to list the 
        // services that are in the database. 
        SC_MANAGER_ENUMERATE_SERVICE = 0x0004,

        // Required to call the LockServiceDatabase function to acquire a lock on 
        // the database. 
        SC_MANAGER_LOCK = 0x0008,

        // Required to call the QueryServiceLockStatus function to retrieve the 
        // lock status information for the database
        SC_MANAGER_QUERY_LOCK_STATUS = 0x0010,

        // Required to call the NotifyBootConfigStatus function. 
        SC_MANAGER_MODIFY_BOOT_CONFIG = 0x0020,

        // Includes STANDARD_RIGHTS_REQUIRED, in addition to all access rights in 
        // this table. 
        SC_MANAGER_ALL_ACCESS = 0xF003F
    }


    [Flags]
    internal enum ServiceAccessRights : int
    {
        // Required to call the QueryServiceConfig and QueryServiceConfig2 
        // functions to query the service configuration. 
        SERVICE_QUERY_CONFIG = 0x0001,

        // Required to call the ChangeServiceConfig or ChangeServiceConfig2 
        // function to change the service configuration. Because this grants the 
        // caller the right to change the executable file that the system runs, 
        // it should be granted only to administrators. 
        SERVICE_CHANGE_CONFIG = 0x0002,

        // Required to call the QueryServiceStatusEx function to ask the service 
        // control manager about the status of the service. 
        SERVICE_QUERY_STATUS = 0x0004,

        // Required to call the EnumDependentServices function to enumerate all 
        // the services dependent on the service. 
        SERVICE_ENUMERATE_DEPENDENTS = 0x0008,

        // Required to call the StartService function to start the service. 
        SERVICE_START = 0x0010,

        // Required to call the ControlService function to stop the service. 
        SERVICE_STOP = 0x0020,

        // Required to call the ControlService function to pause or continue the 
        // service. 
        SERVICE_PAUSE_CONTINUE = 0x0040,

        // Required to call the ControlService function to ask the service to 
        // report its status immediately. 
        SERVICE_INTERROGATE = 0x0080,

        // Required to call the ControlService function to specify a user-defined 
        // control code.
        SERVICE_USER_DEFINED_CONTROL = 0x0100,

        // Includes STANDARD_RIGHTS_REQUIRED in addition to all access rights in 
        // this table. 
        SERVICE_ALL_ACCESS = 0xF01FF
    }


    internal enum ServiceConfig2InfoLevel : uint
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


    internal enum ServiceTriggerType : uint
    {
        // The event is triggered when a device of the specified device interface 
        // class arrives or is present when the system starts. This trigger event 
        // is commonly used to start a service.
        SERVICE_TRIGGER_TYPE_DEVICE_INTERFACE_ARRIVAL = 1,

        // The event is triggered when the first IP address on the TCP/IP 
        // networking stack becomes available or the last IP address on the stack 
        // becomes unavailable. This trigger event can be used to start or stop a 
        // service.
        SERVICE_TRIGGER_TYPE_IP_ADDRESS_AVAILABILITY = 2,

        // The event is triggered when the computer joins or leaves a domain. 
        // This trigger event can be used to start or stop a service.
        SERVICE_TRIGGER_TYPE_DOMAIN_JOIN = 3,

        // The event is triggered when a firewall port is opened or approximately 
        // 60 seconds after the firewall port is closed. This trigger event can 
        // be used to start or stop a service.
        SERVICE_TRIGGER_TYPE_FIREWALL_PORT_EVENT = 4,

        // The event is triggered when a machine policy or user policy change 
        // occurs. This trigger event is commonly used to start a service.
        SERVICE_TRIGGER_TYPE_GROUP_POLICY = 5,

        // The event is a custom event generated by an Event Tracing for Windows 
        // (ETW) provider. This trigger event can be used to start or stop a 
        // service.
        SERVICE_TRIGGER_TYPE_CUSTOM = 20
    }


    internal enum ServiceTriggerAction : uint
    {
        // Start the service when the specified trigger event occurs.
        SERVICE_TRIGGER_ACTION_SERVICE_START = 1,

        // Stop the service when the specified trigger event occurs.
        SERVICE_TRIGGER_ACTION_SERVICE_STOP = 2
    }


    internal enum ServiceTriggerDataType : uint
    {
        // The trigger-specific data is in binary format.
        SERVICE_TRIGGER_DATA_TYPE_BINARY = 1,

        // The trigger-specific data is in string format.
        SERVICE_TRIGGER_DATA_TYPE_STRING = 2
    }


    [StructLayout(LayoutKind.Sequential)]
    internal struct SERVICE_TRIGGER_SPECIFIC_DATA_ITEM
    {
        /// The data type of the trigger-specific data pointed to by pData.
        public ServiceTriggerDataType dwDataType;

        /// The size of the trigger-specific data pointed to pData, in bytes. 
        /// The maximum value is 1024.
        public uint cbData;

        /// A pointer to the trigger-specific data for the service trigger event.
        /// Strings must be Unicode; ANSI strings are not supported.
        public System.IntPtr pData;
    }


    [StructLayout(LayoutKind.Sequential)]
    internal struct SERVICE_TRIGGER
    {
        /// The trigger event type.
        public ServiceTriggerType dwTriggerType;

        /// The action to take when the specified trigger event occurs.
        public ServiceTriggerAction dwAction;

        /// Points to a GUID that identifies the trigger event subtype. The value
        /// of this member depends on the value of the dwTriggerType member.
        public IntPtr pTriggerSubtype;

        /// The number of SERVICE_TRIGGER_SPECIFIC_DATA_ITEM structures in the 
        /// array pointed to by pDataItems. 
        public uint cDataItems;

        /// A pointer to an array of SERVICE_TRIGGER_SPECIFIC_DATA_ITEM 
        /// structures that contain trigger-specific data. 
        public IntPtr pDataItems;
    }


    [StructLayout(LayoutKind.Sequential)]
    internal struct SERVICE_TRIGGER_INFO
    {
        /// The number of triggers in the array of SERVICE_TRIGGER structures 
        /// pointed to by the pTriggers member. 
        public uint cTriggers;

        /// A pointer to an array of SERVICE_TRIGGER structures that specify the 
        /// trigger events for the service. 
        public IntPtr pTriggers;

        /// This member is reserved and must be NULL.
        public IntPtr pReserved;
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


    internal static class NativeMethods
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
        public static extern SafeServiceHandle OpenSCManager(string machineName,
            string databaseName, ServiceControlAccessRights dwAccess);


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
        public static extern SafeServiceHandle OpenService(SafeServiceHandle hSCManager,
            string lpServiceName, ServiceAccessRights dwDesiredAccess);


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
}