'****************************** Module Header ******************************'
' Module Name:  NativeMethods.vb
' Project:      VBWin7TriggerStartService
' Copyright (c) Microsoft Corporation.
' 
' The P/Invoke signatures of native service APIs and structs.
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


<Flags()> _
Public Enum ServiceControlAccessRights

    ' Includes STANDARD_RIGHTS_REQUIRED, in addition to all access rights in 
    ' this table. 
    SC_MANAGER_ALL_ACCESS = &HF003F

    ' Required to connect to the service control manager.
    SC_MANAGER_CONNECT = 1

    ' Required to call the CreateService function to create a service 
    ' object and add it to the database. 
    SC_MANAGER_CREATE_SERVICE = 2

    ' Required to call the EnumServicesStatusEx function to list the 
    ' services that are in the database. 
    SC_MANAGER_ENUMERATE_SERVICE = 4

    ' Required to call the LockServiceDatabase function to acquire a lock on 
    ' the database. 
    SC_MANAGER_LOCK = 8

    ' Required to call the QueryServiceLockStatus function to retrieve the 
    ' lock status information for the database
    SC_MANAGER_QUERY_LOCK_STATUS = &H10

    ' Required to call the NotifyBootConfigStatus function. 
    SC_MANAGER_MODIFY_BOOT_CONFIG = &H20

End Enum


<Flags()> _
Public Enum ServiceAccessRights

    ' Required to call the QueryServiceConfig and QueryServiceConfig2 
    ' functions to query the service configuration. 
    SERVICE_QUERY_CONFIG = 1

    ' Required to call the ChangeServiceConfig or ChangeServiceConfig2 
    ' function to change the service configuration. Because this grants the 
    ' caller the right to change the executable file that the system runs, 
    ' it should be granted only to administrators. 
    SERVICE_CHANGE_CONFIG = 2

    ' Required to call the QueryServiceStatusEx function to ask the service 
    ' control manager about the status of the service. 
    SERVICE_QUERY_STATUS = 4

    ' Required to call the EnumDependentServices function to enumerate all 
    ' the services dependent on the service. 
    SERVICE_ENUMERATE_DEPENDENTS = 8

    ' Required to call the StartService function to start the service. 
    SERVICE_START = &H10

    ' Required to call the ControlService function to stop the service. 
    SERVICE_STOP = &H20

    ' Required to call the ControlService function to pause or continue the 
    ' service.
    SERVICE_PAUSE_CONTINUE = &H40

    ' Required to call the ControlService function to ask the service to 
    ' report its status immediately. 
    SERVICE_INTERROGATE = &H80

    ' Required to call the ControlService function to specify a user-defined 
    ' control code.
    SERVICE_USER_DEFINED_CONTROL = &H100

    ' Includes STANDARD_RIGHTS_REQUIRED in addition to all access rights in 
    ' this table. 
    SERVICE_ALL_ACCESS = &HF01FF

End Enum


Public Enum ServiceConfig2InfoLevel As UInt32
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


Public Enum ServiceTriggerType As UInt32

    ' The event is triggered when a device of the specified device interface 
    ' class arrives or is present when the system starts. This trigger event 
    ' is commonly used to start a service.
    SERVICE_TRIGGER_TYPE_DEVICE_INTERFACE_ARRIVAL = 1

    ' The event is triggered when the first IP address on the TCP/IP 
    ' networking stack becomes available or the last IP address on the stack 
    ' becomes unavailable. This trigger event can be used to start or stop a 
    ' service.
    SERVICE_TRIGGER_TYPE_IP_ADDRESS_AVAILABILITY = 2

    ' The event is triggered when the computer joins or leaves a domain. 
    ' This trigger event can be used to start or stop a service.
    SERVICE_TRIGGER_TYPE_DOMAIN_JOIN = 3

    ' The event is triggered when a firewall port is opened or approximately 
    ' 60 seconds after the firewall port is closed. This trigger event can 
    ' be used to start or stop a service.
    SERVICE_TRIGGER_TYPE_FIREWALL_PORT_EVENT = 4

    ' The event is triggered when a machine policy or user policy change 
    ' occurs. This trigger event is commonly used to start a service.
    SERVICE_TRIGGER_TYPE_GROUP_POLICY = 5

    ' The event is a custom event generated by an Event Tracing for Windows 
    ' (ETW) provider. This trigger event can be used to start or stop a 
    ' service.
    SERVICE_TRIGGER_TYPE_CUSTOM = 20

End Enum


Public Enum ServiceTriggerAction As UInt32

    ' Start the service when the specified trigger event occurs.
    SERVICE_TRIGGER_ACTION_SERVICE_START = 1

    ' Stop the service when the specified trigger event occurs.
    SERVICE_TRIGGER_ACTION_SERVICE_STOP = 2

End Enum


Public Enum ServiceTriggerDataType As UInt32

    ' The trigger-specific data is in binary format.
    SERVICE_TRIGGER_DATA_TYPE_BINARY = 1

    ' The trigger-specific data is in string format.
    SERVICE_TRIGGER_DATA_TYPE_STRING = 2

End Enum


<StructLayout(LayoutKind.Sequential)> _
Public Structure SERVICE_TRIGGER_SPECIFIC_DATA_ITEM

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
Public Structure SERVICE_TRIGGER

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
Public Structure SERVICE_TRIGGER_INFO

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
    ByVal dwAccess As ServiceControlAccessRights) As SafeServiceHandle
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
    ByVal hSCManager As SafeServiceHandle, ByVal lpServiceName As String, _
    ByVal dwDesiredAccess As ServiceAccessRights) As SafeServiceHandle
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
    ByVal hService As SafeServiceHandle, ByVal dwInfoLevel As ServiceConfig2InfoLevel, _
    ByVal lpInfo As IntPtr) As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function


    ''' <summary>
    ''' Retrieves the optional configuration parameters of the specified service.
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
    ByVal hService As SafeServiceHandle, ByVal dwInfoLevel As ServiceConfig2InfoLevel, _
    ByVal lpBuffer As IntPtr, ByVal cbBufSize As Integer, _
    <Out()> ByRef pcbBytesNeeded As Integer) As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function


    ''' <summary>
    ''' Closes a handle to a service control manager or service object.
    ''' </summary>
    ''' <param name="hSCObject">
    ''' A handle to the service control manager object or the service object 
    ''' to close.
    ''' </param>
    <DllImport("advapi32.dll")> _
    Public Shared Function CloseServiceHandle(ByVal hSCObject As IntPtr) _
    As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

End Class