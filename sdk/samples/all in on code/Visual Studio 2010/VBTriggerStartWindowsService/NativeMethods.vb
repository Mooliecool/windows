'****************************** Module Header ******************************'
' Module Name:  NativeMethods.vb
' Project:      VBTriggerStartWindowsService
' Copyright (c) Microsoft Corporation.
' 
' The P/Invoke signatures of native service APIs and types.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"

Imports System.Runtime.InteropServices
Imports System.Security
Imports Microsoft.Win32.SafeHandles

#End Region


Friend Class NativeMethods

    ''' <summary>
    ''' Establishes a connection to the service control manager on the 
    ''' specified computer and opens the specified service control manager 
    ''' database.
    ''' </summary>
    ''' <param name="machineName">Name of the target computer.</param>
    ''' <param name="databaseName">
    ''' Name of the service control manager database.
    ''' </param>
    ''' <param name="dwAccess">
    ''' The access to the service control manager.
    ''' </param>
    ''' <returns>
    ''' If the function succeeds, the return value is a handle to the 
    ''' specified service control manager database. If the function fails, 
    ''' the return value is an invalid handle. To get extended error 
    ''' information, call GetLastError.
    ''' </returns>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function OpenSCManager( _
        ByVal machineName As String, ByVal databaseName As String, _
        ByVal dwAccess As ServiceControlAccessRights) _
    As SafeServiceHandle
    End Function


    ''' <summary>
    ''' Opens an existing service.
    ''' </summary>
    ''' <param name="hSCManager">
    ''' A handle to the service control manager database. The OpenSCManager 
    ''' function returns this handle.
    ''' </param>
    ''' <param name="lpServiceName">
    ''' The name of the service to be opened.
    ''' </param>
    ''' <param name="dwDesiredAccess">The access to the service.</param>
    ''' <returns>
    ''' If the function succeeds, the return value is a handle to the 
    ''' specified service. If the function fails, the return value is an 
    ''' invalid handle. To get extended error information, call GetLastError.
    ''' </returns>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function OpenService( _
        ByVal hSCManager As SafeServiceHandle, _
        ByVal lpServiceName As String, _
        ByVal dwDesiredAccess As ServiceAccessRights) _
    As SafeServiceHandle
    End Function


    ''' <summary>
    ''' Changes the optional configuration parameters of a service.
    ''' </summary>
    ''' <param name="hService">A handle to the service.</param>
    ''' <param name="dwInfoLevel">
    ''' The configuration information to be changed. 
    ''' </param>
    ''' <param name="lpInfo"></param>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function ChangeServiceConfig2( _
        ByVal hService As SafeServiceHandle, _
        ByVal dwInfoLevel As ServiceConfig2InfoLevel, _
        ByVal lpInfo As IntPtr) _
    As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function


    ''' <summary>
    ''' Retrieves the optional configuration parameters of the specified 
    ''' service.
    ''' </summary>
    ''' <param name="hService">A handle to the service.</param>
    ''' <param name="dwInfoLevel">
    ''' The configuration information to be queried.
    ''' </param>
    ''' <param name="lpBuffer">
    ''' A pointer to the buffer that receives the service configuration 
    ''' information. 
    ''' </param>
    ''' <param name="cbBufSize">
    ''' The size of the structure pointed to by the lpBuffer parameter, in 
    ''' bytes.
    ''' </param>
    ''' <param name="pcbBytesNeeded">
    ''' A pointer to a variable that receives the number of bytes required to 
    ''' store the configuration information, if the function fails with 
    ''' ERROR_INSUFFICIENT_BUFFER.
    ''' </param>
    ''' <returns>
    ''' If the function succeeds, the return value is nonzero. If the 
    ''' function fails, the return value is zero. To get extended error 
    ''' information, call GetLastError.
    ''' </returns>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function QueryServiceConfig2( _
        ByVal hService As SafeServiceHandle, _
        ByVal dwInfoLevel As ServiceConfig2InfoLevel, _
        ByVal lpBuffer As IntPtr, ByVal cbBufSize As Integer, _
        <Out()> ByRef pcbBytesNeeded As Integer) _
    As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function


    ''' <summary>
    ''' Closes a handle to a service control manager or service object.
    ''' </summary>
    ''' <param name="hSCObject">
    ''' A handle to the service control manager object or the service object 
    ''' to close.
    ''' </param>
    ''' <returns>
    ''' If the function succeeds, the return value is true.
    ''' If the function fails, the return value is false. To get extended 
    ''' error information, call GetLastError.
    ''' </returns>
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function CloseServiceHandle(ByVal hSCObject As IntPtr) _
    As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

End Class


<SuppressUnmanagedCodeSecurity()> _
Friend Class SafeServiceHandle
    Inherits SafeHandleZeroOrMinusOneIsInvalid

    Friend Sub New()
        MyBase.New(True)
    End Sub

    Protected Overrides Function ReleaseHandle() As Boolean
        Return NativeMethods.CloseServiceHandle(MyBase.handle)
    End Function

End Class


<StructLayout(LayoutKind.Sequential)> _
Friend Structure SERVICE_TRIGGER_SPECIFIC_DATA_ITEM

    ''' <summary>
    ''' The data type of the trigger-specific data pointed to by pData.
    ''' </summary>
    Public dwDataType As ServiceTriggerDataType

    ''' <summary>
    ''' The size of the trigger-specific data pointed to pData, in bytes. 
    ''' The maximum value is 1024.
    ''' </summary>
    Public cbData As UInt32

    ''' <summary>
    ''' A pointer to the trigger-specific data for the service trigger event.
    ''' Strings must be Unicode; ANSI strings are not supported.
    ''' </summary>
    Public pData As IntPtr

End Structure


<StructLayout(LayoutKind.Sequential)> _
Friend Structure SERVICE_TRIGGER

    ''' <summary>
    ''' The trigger event type.
    ''' </summary>
    Public dwTriggerType As ServiceTriggerType

    ''' <summary>
    ''' The action to take when the specified trigger event occurs.
    ''' </summary>
    Public dwAction As ServiceTriggerAction

    ''' <summary>
    ''' Points to a GUID that identifies the trigger event subtype. The value
    ''' of this member depends on the value of the dwTriggerType member.
    ''' </summary>
    Public pTriggerSubtype As IntPtr

    ''' <summary>
    ''' The number of SERVICE_TRIGGER_SPECIFIC_DATA_ITEM structures in the 
    ''' array pointed to by pDataItems. 
    ''' </summary>
    Public cDataItems As UInt32

    ''' <summary>
    ''' A pointer to an array of SERVICE_TRIGGER_SPECIFIC_DATA_ITEM 
    ''' structures that contain trigger-specific data. 
    ''' </summary>
    Public pDataItems As IntPtr

End Structure


<StructLayout(LayoutKind.Sequential)> _
Friend Structure SERVICE_TRIGGER_INFO

    ''' <summary>
    ''' The number of triggers in the array of SERVICE_TRIGGER structures 
    ''' pointed to by the pTriggers member. 
    ''' </summary>
    Public cTriggers As UInt32

    ''' <summary>
    ''' A pointer to an array of SERVICE_TRIGGER structures that specify the 
    ''' trigger events for the service. 
    ''' </summary>
    Public pTriggers As IntPtr

    ''' <summary>
    ''' This member is reserved and must be NULL.
    ''' </summary>
    Public pReserved As IntPtr

End Structure


Friend Enum ServiceConfig2InfoLevel As UInt32
    SERVICE_CONFIG_DELAYED_AUTO_START_INFO = 3
    SERVICE_CONFIG_DESCRIPTION = 1
    SERVICE_CONFIG_FAILURE_ACTIONS = 2
    SERVICE_CONFIG_FAILURE_ACTIONS_FLAG = 4
    SERVICE_CONFIG_PREFERRED_NODE = 9
    SERVICE_CONFIG_PRESHUTDOWN_INFO = 7
    SERVICE_CONFIG_REQUIRED_PRIVILEGES_INFO = 6
    SERVICE_CONFIG_SERVICE_SID_INFO = 5
    SERVICE_CONFIG_TRIGGER_INFO = 8
End Enum


Friend Enum ServiceTriggerType As UInt32

    ''' <summary>
    ''' The event is triggered when a device of the specified device interface 
    ''' class arrives or is present when the system starts. This trigger event 
    ''' is commonly used to start a service.
    ''' </summary>
    SERVICE_TRIGGER_TYPE_DEVICE_INTERFACE_ARRIVAL = 1

    ''' <summary>
    ''' The event is triggered when the first IP address on the TCP/IP 
    ''' networking stack becomes available or the last IP address on the stack 
    ''' becomes unavailable. This trigger event can be used to start or stop a 
    ''' service.
    ''' </summary>
    SERVICE_TRIGGER_TYPE_IP_ADDRESS_AVAILABILITY = 2

    ''' <summary>
    ''' The event is triggered when the computer joins or leaves a domain. 
    ''' This trigger event can be used to start or stop a service.
    ''' </summary>
    SERVICE_TRIGGER_TYPE_DOMAIN_JOIN = 3

    ''' <summary>
    ''' The event is triggered when a firewall port is opened or approximately 
    ''' 60 seconds after the firewall port is closed. This trigger event can 
    ''' be used to start or stop a service.
    ''' </summary>
    SERVICE_TRIGGER_TYPE_FIREWALL_PORT_EVENT = 4

    ''' <summary>
    ''' The event is triggered when a machine policy or user policy change 
    ''' occurs. This trigger event is commonly used to start a service.
    ''' </summary>
    SERVICE_TRIGGER_TYPE_GROUP_POLICY = 5

    ''' <summary>
    ''' The event is a custom event generated by an Event Tracing for Windows 
    ''' (ETW) provider. This trigger event can be used to start or stop a 
    ''' service.
    ''' </summary>
    SERVICE_TRIGGER_TYPE_CUSTOM = 20

End Enum


Friend Enum ServiceTriggerAction As UInt32

    ''' <summary>
    ''' Start the service when the specified trigger event occurs.
    ''' </summary>
    SERVICE_TRIGGER_ACTION_SERVICE_START = 1

    ''' <summary>
    ''' Stop the service when the specified trigger event occurs.
    ''' </summary>
    SERVICE_TRIGGER_ACTION_SERVICE_STOP = 2

End Enum


Friend Enum ServiceTriggerDataType As UInt32

    ''' <summary>
    ''' The trigger-specific data is in binary format.
    ''' </summary>
    SERVICE_TRIGGER_DATA_TYPE_BINARY = 1

    ''' <summary>
    ''' The trigger-specific data is in string format.
    ''' </summary>
    SERVICE_TRIGGER_DATA_TYPE_STRING = 2

End Enum


''' <summary>
''' The AccessMask data type defines standard, specific, and generic rights.
''' http://msdn.microsoft.com/en-us/library/aa374892.aspx
''' </summary>
Friend Enum AccessMask As UInt32
    DELETE = &H10000
    READ_CONTROL = &H20000
    WRITE_DAC = &H40000
    WRITE_OWNER = &H80000
    SYNCHRONIZE = &H100000

    STANDARD_RIGHTS_REQUIRED = &HF0000

    STANDARD_RIGHTS_READ = &H20000
    STANDARD_RIGHTS_WRITE = &H20000
    STANDARD_RIGHTS_EXECUTE = &H20000

    STANDARD_RIGHTS_ALL = &H1F0000

    SPECIFIC_RIGHTS_ALL = &HFFFF

    ACCESS_SYSTEM_SECURITY = &H1000000
End Enum


<Flags()> _
Friend Enum ServiceControlAccessRights

    ''' <summary>
    ''' Required to connect to the service control manager.
    ''' </summary>
    SC_MANAGER_CONNECT = 1

    ''' <summary>
    ''' Required to call the CreateService function to create a service 
    ''' object and add it to the database. 
    ''' </summary>    
    SC_MANAGER_CREATE_SERVICE = 2

    ''' <summary>
    ''' Required to call the EnumServicesStatusEx function to list the 
    ''' services that are in the database. 
    ''' </summary>
    SC_MANAGER_ENUMERATE_SERVICE = 4

    ''' <summary>
    ''' Required to call the LockServiceDatabase function to acquire a lock on 
    ''' the database. 
    ''' </summary>
    SC_MANAGER_LOCK = 8

    ''' <summary>
    ''' Required to call the QueryServiceLockStatus function to retrieve the 
    ''' lock status information for the database
    ''' </summary>
    SC_MANAGER_QUERY_LOCK_STATUS = &H10

    ''' <summary>
    ''' Required to call the NotifyBootConfigStatus function. 
    ''' </summary>
    SC_MANAGER_MODIFY_BOOT_CONFIG = &H20

    ''' <summary>
    ''' Includes STANDARD_RIGHTS_REQUIRED, in addition to all access rights in 
    ''' this table. 
    ''' </summary>
    SC_MANAGER_ALL_ACCESS = (AccessMask.STANDARD_RIGHTS_REQUIRED Or _
        SC_MANAGER_CONNECT Or _
        SC_MANAGER_CREATE_SERVICE Or _
        SC_MANAGER_ENUMERATE_SERVICE Or _
        SC_MANAGER_LOCK Or _
        SC_MANAGER_QUERY_LOCK_STATUS Or _
        SC_MANAGER_MODIFY_BOOT_CONFIG)

    GENERIC_READ = (AccessMask.STANDARD_RIGHTS_READ Or _
        SC_MANAGER_ENUMERATE_SERVICE Or _
        SC_MANAGER_QUERY_LOCK_STATUS)

    GENERIC_WRITE = (AccessMask.STANDARD_RIGHTS_WRITE Or _
        SC_MANAGER_CREATE_SERVICE Or _
        SC_MANAGER_MODIFY_BOOT_CONFIG)

    GENERIC_EXECUTE = (AccessMask.STANDARD_RIGHTS_EXECUTE Or _
        SC_MANAGER_CONNECT Or _
        SC_MANAGER_LOCK)

    GENERIC_ALL = SC_MANAGER_ALL_ACCESS

End Enum


''' <summary>
''' Access to the service. Before granting the requested access, the system 
''' checks the access token of the calling process.
''' </summary>
<Flags()> _
Friend Enum ServiceAccessRights

    ''' <summary>
    ''' Required to call the QueryServiceConfig and QueryServiceConfig2 
    ''' functions to query the service configuration. 
    ''' </summary>
    SERVICE_QUERY_CONFIG = 1

    ''' <summary>
    ''' Required to call the ChangeServiceConfig or ChangeServiceConfig2 
    ''' function to change the service configuration. Because this grants the 
    ''' caller the right to change the executable file that the system runs, 
    ''' it should be granted only to administrators. 
    ''' </summary>
    SERVICE_CHANGE_CONFIG = 2

    ''' <summary>
    ''' Required to call the QueryServiceStatusEx function to ask the service 
    ''' control manager about the status of the service. 
    ''' </summary>
    SERVICE_QUERY_STATUS = 4

    ''' <summary>
    ''' Required to call the EnumDependentServices function to enumerate all 
    ''' the services dependent on the service. 
    ''' </summary>
    SERVICE_ENUMERATE_DEPENDENTS = 8

    ''' <summary>
    ''' Required to call the StartService function to start the service. 
    ''' </summary>
    ''' <remarks></remarks>
    SERVICE_START = &H10

    ''' <summary>
    ''' Required to call the ControlService function to stop the service. 
    ''' </summary>
    ''' <remarks></remarks>
    SERVICE_STOP = &H20

    ''' <summary>
    ''' Required to call the ControlService function to pause or continue the 
    ''' service.
    ''' </summary>
    SERVICE_PAUSE_CONTINUE = &H40

    ''' <summary>
    ''' Required to call the ControlService function to ask the service to 
    ''' report its status immediately. 
    ''' </summary>
    SERVICE_INTERROGATE = &H80

    ''' <summary>
    ''' Required to call the ControlService function to specify a user-defined 
    ''' control code.
    ''' </summary>
    SERVICE_USER_DEFINED_CONTROL = &H100

    ''' <summary>
    ''' Includes STANDARD_RIGHTS_REQUIRED in addition to all access rights in 
    ''' this table. 
    ''' </summary>
    SERVICE_ALL_ACCESS = (AccessMask.STANDARD_RIGHTS_REQUIRED Or _
        SERVICE_QUERY_CONFIG Or _
        SERVICE_CHANGE_CONFIG Or _
        SERVICE_QUERY_STATUS Or _
        SERVICE_ENUMERATE_DEPENDENTS Or _
        SERVICE_START Or _
        SERVICE_STOP Or _
        SERVICE_PAUSE_CONTINUE Or _
        SERVICE_INTERROGATE Or _
        SERVICE_USER_DEFINED_CONTROL)

    GENERIC_READ = (AccessMask.STANDARD_RIGHTS_READ Or _
        SERVICE_QUERY_CONFIG Or _
        SERVICE_QUERY_STATUS Or _
        SERVICE_INTERROGATE Or _
        SERVICE_ENUMERATE_DEPENDENTS)

    GENERIC_WRITE = (AccessMask.STANDARD_RIGHTS_WRITE Or _
        SERVICE_CHANGE_CONFIG)

    GENERIC_EXECUTE = (AccessMask.STANDARD_RIGHTS_EXECUTE Or _
        SERVICE_START Or _
        SERVICE_STOP Or _
        SERVICE_PAUSE_CONTINUE Or _
        SERVICE_USER_DEFINED_CONTROL)

    ''' <summary>
    ''' Required to call the QueryServiceObjectSecurity or 
    ''' SetServiceObjectSecurity function to access the SACL. The proper
    ''' way to obtain this access is to enable the SE_SECURITY_NAME 
    ''' privilege in the caller's current access token, open the handle 
    ''' for ACCESS_SYSTEM_SECURITY access, and then disable the privilege.
    ''' </summary>
    ACCESS_SYSTEM_SECURITY = AccessMask.ACCESS_SYSTEM_SECURITY

    ''' <summary>
    ''' Required to call the DeleteService function to delete the service.
    ''' </summary>
    DELETE = AccessMask.DELETE

    ''' <summary>
    ''' Required to call the QueryServiceObjectSecurity function to query
    ''' the security descriptor of the service object.
    ''' </summary>
    READ_CONTROL = AccessMask.READ_CONTROL

    ''' <summary>
    ''' Required to call the SetServiceObjectSecurity function to modify
    ''' the Dacl member of the service object's security descriptor.
    ''' </summary>
    WRITE_DAC = AccessMask.WRITE_DAC

    ''' <summary>
    ''' Required to call the SetServiceObjectSecurity function to modify 
    ''' the Owner and Group members of the service object's security.
    ''' </summary>
    WRITE_OWNER = AccessMask.WRITE_OWNER

End Enum