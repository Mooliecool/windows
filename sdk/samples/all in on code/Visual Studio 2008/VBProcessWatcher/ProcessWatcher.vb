'************************************ Module Header **************************************
'* Module Name:	ProcessWatcher.vb
'* Project:		VBProcessWatcher
'* Copyright (c) Microsoft Corporation.
'* 
'* using WMI Query Language (WQL) to query process events.
'*
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************************

Imports System
Imports System.Management
Imports VBProcessWatcher.WMI

Namespace VBProcessWatcher

    Public Class ProcessWatcher
        Inherits ManagementEventWatcher
        ' Process Events
        Public Event ProcessCreated(ByVal proc As Win32.Process)
        Public Event ProcessDeleted(ByVal proc As Win32.Process)
        Public Event ProcessModified(ByVal proc As Win32.Process)

        ' WMI WQL process query strings
        Shared ReadOnly WMI_OPER_EVENT_QUERY As String = "SELECT * FROM " & _
        "__InstanceOperationEvent WITHIN 1 WHERE TargetInstance ISA 'Win32_Process'" & _
        " and TargetInstance.Name = '{0}'"


        Public Sub New(ByVal processName As String)
            Init(processName)
        End Sub

        Private Sub Init(ByVal processName As String)
            Me.Query.QueryLanguage = "WQL"
            Me.Query.QueryString = String.Format(WMI_OPER_EVENT_QUERY, processName)
            AddHandler Me.EventArrived, AddressOf Me.watcher_EventArrived
        End Sub

        Private Sub watcher_EventArrived(ByVal sender As Object, ByVal e As EventArrivedEventArgs)

            Dim eventType As String = e.NewEvent.ClassPath.ClassName
            Dim proc As New Win32.Process(TryCast(e.NewEvent("TargetInstance"), ManagementBaseObject))

            Select Case eventType
                Case "__InstanceCreationEvent"
                    RaiseEvent ProcessCreated(proc)
                    Exit Select

                Case "__InstanceDeletionEvent"
                    RaiseEvent ProcessDeleted(proc)
                    Exit Select

                Case "__InstanceModificationEvent"
                    RaiseEvent ProcessModified(proc)
                    Exit Select
            End Select
        End Sub
    End Class
End Namespace