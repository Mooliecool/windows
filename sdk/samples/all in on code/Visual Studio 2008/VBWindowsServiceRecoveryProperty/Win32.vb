'*********************************** Module Header ***********************************'
' Module Name:  Win32.vb
' Project:      VBWindowsServiceRecoveryProperty
' Copyright (c) Microsoft Corporation.
' 
' The file declares the P/Invoke signatures of the Win32 APIs and structs.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************************'

#Region "Imports directives"
Imports System
Imports System.Runtime.ConstrainedExecution
Imports System.Runtime.InteropServices
Imports System.Security
Imports System.Security.Permissions
Imports Microsoft.Win32.SafeHandles
#End Region


' Enumeration for SC_ACTION
' The SC_ACTION_TYPE enumeration specifies the actions that the SCM can perform.
Friend Enum SC_ACTION_TYPE
    None = 0
    RestartService = 1
    RebootComputer = 2
    Run_Command = 3
End Enum

' Struct for SERVICE_FAILURE_ACTIONS
' Represents an action that the service control manager can perform.
<StructLayout(LayoutKind.Sequential)> _
Friend Structure SC_ACTION
    Public Type As Integer
    Public Delay As Integer
End Structure

' Struct for ChangeServiceFailureActions
' Represents the action the service controller should take on each failure of a 
' service.
<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Auto)> _
Friend Structure SERVICE_FAILURE_ACTIONS
    Public dwResetPeriod As Integer
    Public lpRebootMsg As String
    Public lpCommand As String
    Public cActions As Integer
    ' A pointer to an array of SC_ACTION structures
    Public lpsaActions As IntPtr
End Structure

' Struct for FailureActionsOnNonCrashFailures
' Contains the failure actions flag setting of a service.
<StructLayout(LayoutKind.Sequential)> _
Friend Structure SERVICE_FAILURE_ACTIONS_FLAG
    Public fFailureActionsOnNonCrashFailures As Boolean
End Structure

' Struct required to set shutdown privileges
' The LUID_AND_ATTRIBUTES structure represents a locally unique identifier 
' (LUID) and its attributes.
<StructLayout(LayoutKind.Sequential)> _
Friend Structure LUID_AND_ATTRIBUTES
    Public Luid As Long
    Public Attributes As Integer
End Structure

' Struct for AdjustTokenPrivileges
' The TOKEN_PRIVILEGES structure contains information about a set of privileges 
' for an access token. Struct required to set shutdown privileges. The Pack 
' attribute specified here is important. We are in essence cheating here because 
' the Privileges field is actually a variable size array of structs.  We use the 
' Pack=1 to align the Privileges field exactly after the PrivilegeCount field 
' when marshalling this struct to Win32. You do not want to know how many hours 
' I had to spend on this alone!!!
<StructLayout(LayoutKind.Sequential, Pack:=1)> _
Friend Structure TOKEN_PRIVILEGES
    Public PrivilegeCount As Integer
    Public Privileges As LUID_AND_ATTRIBUTES
End Structure

''' <summary>
''' Represents a wrapper class for a service handle.
''' </summary>
<SecurityPermission(SecurityAction.InheritanceDemand, UnmanagedCode:=True)> _
<SecurityPermission(SecurityAction.Demand, UnmanagedCode:=True)> _
Friend Class SafeServiceHandle
    Inherits SafeHandleZeroOrMinusOneIsInvalid
    Friend Sub New()
        MyBase.New(True)
    End Sub

    <ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)> _
    Protected Overrides Function ReleaseHandle() As Boolean
        Return Win32.CloseServiceHandle(MyBase.handle)
    End Function
End Class

''' <summary>
''' Represents a wrapper class for a token handle.
''' </summary>
<SecurityPermission(SecurityAction.InheritanceDemand, UnmanagedCode:=True)> _
<SecurityPermission(SecurityAction.Demand, UnmanagedCode:=True)> _
Friend Class SafeTokenHandle
    Inherits SafeHandleZeroOrMinusOneIsInvalid
    Friend Sub New()
        MyBase.New(True)
    End Sub

    Friend Sub New(ByVal handle As IntPtr)
        MyBase.New(True)
        MyBase.SetHandle(handle)
    End Sub

    Protected Overrides Function ReleaseHandle() As Boolean
        Return Win32.CloseHandle(MyBase.handle)
    End Function
End Class

<SuppressUnmanagedCodeSecurity()> _
Friend Class Win32
    Public Const SERVICE_ALL_ACCESS As Integer = &HF01FF
    Public Const SERVICE_QUERY_CONFIG As Integer = &H1
    Public Const SERVICE_CONFIG_FAILURE_ACTIONS As Integer = &H2
    Public Const ERROR_ACCESS_DENIED As Integer = 5
    Public Const SERVICE_CONFIG_FAILURE_ACTIONS_FLAG As Integer = &H4

    Public Const SE_SHUTDOWN_NAME As String = "SeShutdownPrivilege"
    Public Const SE_PRIVILEGE_ENABLED As Integer = 2
    Public Const TOKEN_ADJUST_PRIVILEGES As Integer = 32
    Public Const TOKEN_QUERY As Integer = 8

    ' Win32 function to open the service control manager.
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function OpenSCManager(ByVal lpMachineName As String, _
        ByVal lpDatabaseName As String, _
        ByVal dwDesiredAccess As Integer) As SafeServiceHandle
    End Function

    ' Win32 function to open a service instance.
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function OpenService(ByVal hSCManager As SafeServiceHandle, _
        ByVal lpServiceName As String, _
        ByVal dwDesiredAccess As Integer) As SafeServiceHandle
    End Function

    ' Win32 function to close a service related handle.
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function CloseServiceHandle(ByVal hSCObject As IntPtr) _
    As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

    ' Win32 function to change the service config for the failure actions.
    ' If the service controller handles the SC_ACTION_REBOOT action, 
    ' the caller must have the SE_SHUTDOWN_NAME privilege.
    <DllImport("advapi32.dll", EntryPoint:="ChangeServiceConfig2", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function ChangeServiceFailureActions(ByVal hService As SafeServiceHandle, _
        ByVal dwInfoLevel As Integer, _
        <MarshalAs(UnmanagedType.Struct)> _
        ByRef lpInfo As SERVICE_FAILURE_ACTIONS) As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

    ' This setting is ignored unless the service has configured failure actions.
    <DllImport("advapi32.dll", EntryPoint:="ChangeServiceConfig2", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function FailureActionsOnNonCrashFailures(ByVal hService As SafeServiceHandle, _
        ByVal dwInfoLevel As Integer, _
        <MarshalAs(UnmanagedType.Struct)> _
        ByRef lpInfo As SERVICE_FAILURE_ACTIONS_FLAG) As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

    ' This method adjusts privileges for this process which is needed when
    ' specifying the reboot option for a service failure recover action.
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function AdjustTokenPrivileges(ByVal TokenHandle As SafeTokenHandle, _
        ByVal DisableAllPrivileges As Boolean, _
        <MarshalAs(UnmanagedType.Struct)> _
        ByRef NewState As TOKEN_PRIVILEGES, _
        ByVal BufferLength As Integer, _
        ByVal PreviousState As IntPtr, _
        ByRef ReturnLength As Integer) As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

    ' Looks up the privilege code for the privilege name
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function LookupPrivilegeValue(ByVal lpSystemName As String, _
        ByVal lpName As String, _
        ByRef lpLuid As Long) As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

    ' Opens the security/privilege token for a process handle
    <DllImport("advapi32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function OpenProcessToken(ByVal ProcessHandle As IntPtr, _
        ByVal DesiredAccess As Integer, _
        ByRef TokenHandle As SafeTokenHandle) As <MarshalAs(UnmanagedType.Bool)> Boolean
    End Function

    ' Close the handle.
    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function CloseHandle(ByVal handle As IntPtr) As Boolean
    End Function
End Class