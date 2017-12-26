'********************************* Module Header *********************************'
' Module Name:  ServiceRecoveryProperty.vb
' Project:      VBWindowsServiceRecoveryProperty
' Copyright (c) Microsoft Corporation.
' 
' This file demonstrates how to configure service recovery property include grant 
' shutdown privilege to the process, so that we can configure a special option in 
' "Recovery" tab - "Restart Computer Options...".
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*********************************************************************************'


#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Diagnostics
Imports System.Runtime.InteropServices
#End Region


Friend Class ServiceRecoveryProperty

    ''' <summary>
    ''' Change the recovery property of a Windows service.
    ''' </summary>
    ''' <param name="scName">The name of the Windows service</param>
    ''' <param name="scActions">
    ''' A list of SC_ACTION representing the actions that the service control 
    ''' manager can perform.
    ''' </param>
    ''' <param name="resetPeriod">
    ''' The time after which to reset the failure count to zero if there are no 
    ''' failures, in seconds. 
    ''' </param>
    ''' <param name="command">
    ''' The command line of the process for the CreateProcess function to execute 
    ''' in response to the SC_ACTION_RUN_COMMAND service controller action. This 
    ''' process runs under the same account as the service. 
    ''' </param>
    ''' <param name="fFailureActionsOnNonCrashFailures">
    ''' If this member is true and the service has configured failure actions, 
    ''' the failure actions are queued if the service process terminates without 
    ''' reporting a status of SERVICE_STOPPED or if it enters the SERVICE_STOPPED 
    ''' state but the dwWin32ExitCode member of the SERVICE_STATUS structure is 
    ''' not ERROR_SUCCESS (0). If this member is false and the service has 
    ''' configured failure actions, the failure actions are queued only if the 
    ''' service terminates without reporting a status of SERVICE_STOPPED.
    ''' </param>
    ''' <param name="rebootMsg">
    ''' The message to be broadcast to server users before rebooting in response 
    ''' to the SC_ACTION_REBOOT service controller action.
    ''' </param>
    Public Shared Sub ChangeRecoveryProperty(ByVal scName As String, _
                                             ByVal scActions As List(Of SC_ACTION), _
                                             ByVal resetPeriod As Integer, _
                                             ByVal command As String, _
                                             ByVal fFailureActionsOnNonCrashFailures As Boolean, _
                                             ByVal rebootMsg As String)
        Dim hSCManager As SafeServiceHandle = Nothing
        Dim hService As SafeServiceHandle = Nothing
        Dim hGlobal As IntPtr = IntPtr.Zero

        Try
            ' Open the service control manager.
            hSCManager = Win32.OpenSCManager(Nothing, Nothing, Win32.SERVICE_QUERY_CONFIG)
            If hSCManager.IsInvalid Then
                Throw New Win32Exception()
            End If

            ' Open the service.
            hService = Win32.OpenService(hSCManager, scName, Win32.SERVICE_ALL_ACCESS)
            If hService.IsInvalid Then
                Throw New Win32Exception()
            End If

            Dim numActions As Integer = scActions.Count
            Dim falureActions As Integer() = New Integer(numActions * 2) {}
            Dim needShutdownPrivilege As Boolean = False
            Dim i As Integer = 0

            ' We need to copy the actions in scFailureActionArray to an 
            ' unmanaged memory through Marshal.Copy.

            For Each scAction As SC_ACTION In scActions
                falureActions(i) = scAction.Type
                falureActions(System.Threading.Interlocked.Increment(i)) = scAction.Delay
                System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)

                If scAction.Type = DirectCast(SC_ACTION_TYPE.RebootComputer, Integer) Then
                    needShutdownPrivilege = True
                End If
            Next

            ' If we need shutdown privilege, then grant it to this process.
            If needShutdownPrivilege Then
                GrantShutdownPrivilege()
            End If

            ' Allocate memory.
            hGlobal = Marshal.AllocHGlobal(falureActions.Length * Marshal.SizeOf(GetType(Integer)))

            ' Copies data from a one-dimensional, managed 32-bit signed integer 
            ' array to an unmanaged memory pointer.
            Marshal.Copy(falureActions, 0, hGlobal, falureActions.Length)

            ' Set the SERVICE_FAILURE_ACTIONS struct.
            Dim scFailureActions As New SERVICE_FAILURE_ACTIONS()
            scFailureActions.cActions = numActions
            scFailureActions.dwResetPeriod = resetPeriod
            scFailureActions.lpCommand = command
            scFailureActions.lpRebootMsg = rebootMsg
            scFailureActions.lpsaActions = hGlobal

            ' Call the ChangeServiceFailureActions function abstraction of the 
            ' ChangeServiceConfig2 function. 
            If Not Win32.ChangeServiceFailureActions(hService, _
                Win32.SERVICE_CONFIG_FAILURE_ACTIONS, _
                scFailureActions) Then
                Throw New Win32Exception()
            End If

            ' Restart Computer Options....
            Dim flag As New SERVICE_FAILURE_ACTIONS_FLAG()
            flag.fFailureActionsOnNonCrashFailures = fFailureActionsOnNonCrashFailures

            ' Call the FailureActionsOnNonCrashFailures function, the 
            ' abstraction of the ChangeServiceConfig2 function.
            If Not Win32.FailureActionsOnNonCrashFailures(hService, _
                Win32.SERVICE_CONFIG_FAILURE_ACTIONS_FLAG, flag) Then
                Throw New Win32Exception()
            End If
        Finally
            ' Close the service control manager handle.
            If hSCManager IsNot Nothing AndAlso Not hSCManager.IsInvalid Then
                hSCManager.Dispose()
                hSCManager = Nothing
            End If

            ' Close the service handle.
            If hService IsNot Nothing AndAlso Not hService.IsInvalid Then
                hService.Dispose()
                hService = Nothing
            End If

            ' Free the unmanaged memory.
            If hGlobal <> IntPtr.Zero Then
                Marshal.FreeHGlobal(hGlobal)
                hGlobal = IntPtr.Zero
            End If
        End Try
    End Sub

    ''' <summary>
    ''' Grant shutdown privilege to the process.
    ''' </summary>
    Private Shared Sub GrantShutdownPrivilege()
        Dim hToken As SafeTokenHandle = New SafeTokenHandle(IntPtr.Zero)

        Try
            ' Open the access token associated with the current process.
            If Not Win32.OpenProcessToken(Process.GetCurrentProcess().Handle, _
                Win32.TOKEN_ADJUST_PRIVILEGES Or Win32.TOKEN_QUERY, hToken) Then
                Throw New Win32Exception()
            End If

            ' Retrieve the locally unique identifier (LUID) used on a specified 
            ' system to locally represent the specified privilege name.
            Dim Luid As Long = 0
            If Not Win32.LookupPrivilegeValue(Nothing, Win32.SE_SHUTDOWN_NAME, Luid) Then
                Throw New Win32Exception()
            End If

            Dim tokenPrivileges As New TOKEN_PRIVILEGES()
            tokenPrivileges.PrivilegeCount = 1
            tokenPrivileges.Privileges.Luid = Luid
            tokenPrivileges.Privileges.Attributes = Win32.SE_PRIVILEGE_ENABLED

            ' Enable privileges in the specified access token.
            Dim retLen As Integer = 0
            If Not Win32.AdjustTokenPrivileges(hToken, _
                False, tokenPrivileges, _
                0, IntPtr.Zero, retLen) Then
                Throw New Win32Exception()
            End If
        Finally
            If hToken IsNot Nothing AndAlso Not hToken.IsInvalid Then
                hToken.Dispose()
                hToken = Nothing
            End If
        End Try
    End Sub
End Class