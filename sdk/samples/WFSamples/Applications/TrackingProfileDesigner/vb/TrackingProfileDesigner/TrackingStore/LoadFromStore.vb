'---------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'   Copyright (C) Microsoft Corporation.  All rights reserved.
' 
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
' 
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System.Reflection
Imports System.IO

Public Class LoadFromStore
    Const NoProfilesAvailable As String = "No profiles available."
    Const NoWorkflowsAvailable As String = "No workflows available."
    Dim workflowProfilesValue As DataTable
    Dim selectedWorkflowValue As Type
    Dim selectedProfileVersionValue As Version


    ' Gets or sets the table used to display the workflow and tracking profile information

    Public Property WorkflowProfiles() As DataTable
        Get
            Return workflowProfilesValue
        End Get
        Set(ByVal value As DataTable)
            workflowProfilesValue = value
        End Set
    End Property

    ' The selected workflow
    Public Property SelectedWorkflow() As Type
        Get
            Return selectedWorkflowValue
        End Get

        Set(ByVal value As Type)
            selectedWorkflowValue = value
        End Set
    End Property

    ' The selected tracking profile

    Public Property SelectedProfileVersion() As Version
        Get
            Return selectedProfileVersionValue
        End Get

        Set(ByVal value As Version)
            selectedProfileVersionValue = value
        End Set
    End Property


    ' Contains information the workflow type info stored in the tracking database
    Private Class WorkflowInfo

        Public typeFullNameValue As String

        Public Property TypeFullName() As String
            Get
                Return typeFullNameValue
            End Get
            Set(ByVal value As String)
                typeFullNameValue = value
            End Set
        End Property

        Public assemblyFullNameValue As String

        Public Property AssemblyFullName() As String
            Get
                Return assemblyFullNameValue
            End Get
            Set(ByVal value As String)
                AssemblyFullNameValue = value
            End Set
        End Property

        Public Overrides Function Equals(ByVal obj As Object) As Boolean
            Dim other As WorkflowInfo = TryCast(obj, WorkflowInfo)
            If other IsNot Nothing Then
                Return other.AssemblyFullName.Equals(Me.AssemblyFullName) AndAlso other.TypeFullName.Equals(Me.TypeFullName)
            Else
                Return False
            End If
        End Function

        Public Overrides Function ToString() As String
            ' We could possibly be listing the same type twice
            ' if an identical name is located in multiple assemblies.
            ' However, for simplicity and brevity, we only use the type name
            Return TypeFullName
        End Function

        Public Overrides Function GetHashCode() As Integer
            ' Always a good idea to override GetHashCode when implementing Equals()
            Return TypeFullName.GetHashCode() Xor AssemblyFullName.GetHashCode()
        End Function

    End Class

    Private Sub LoadFromStoreLoad(ByVal sender As Object, ByVal e As EventArgs) Handles Me.Load
        If WorkflowProfiles.Rows.Count > 0 Then
            For Each row As DataRow In WorkflowProfiles.Rows
                ' Add a row for each workflow type found in the store
                Dim wi As New WorkflowInfo()
                wi.AssemblyFullName = CType(row("AssemblyFullName"), String)
                wi.TypeFullName = CType(row("TypeFullName"), String)
                If Not workflowList.Items.Contains(wi) Then
                    workflowList.Items.Add(wi)
                End If
            Next
        Else
            workflowList.Items.Add(NoWorkflowsAvailable)
        End If
    End Sub

    Private Sub WorkflowListSelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles workflowList.SelectedIndexChanged
        Dim wi As WorkflowInfo = TryCast(workflowList.SelectedItem, WorkflowInfo)
        If wi IsNot Nothing Then
            ' Update the profile list with the available profile versions for the selected workflow type
            profileVersionList.Items.Clear()

            Dim versionRows As DataRow() = WorkflowProfiles.Select(String.Format("TypeFullName = '{0}' AND AssemblyFullName = '{1}'", wi.TypeFullName, wi.AssemblyFullName))
            If versionRows Is Nothing Or versionRows.Length = 0 Then
                ' If no profiles exist
                profileVersionList.Items.Add(NoProfilesAvailable)
            Else
                For Each row As DataRow In versionRows
                    profileVersionList.Items.Add(New Version(CType(row("Version"), String)))
                Next
            End If
        End If
    End Sub

    Private Sub SaveClick(ByVal sender As Object, ByVal e As EventArgs) Handles ok.Click
        If workflowList.SelectedItem IsNot Nothing AndAlso profileVersionList.SelectedItem IsNot Nothing Then
            Dim wi As WorkflowInfo = TryCast(workflowList.SelectedItem, WorkflowInfo)
            If wi IsNot Nothing Then
                Dim typeAsm As Assembly

                Try
                    typeAsm = Assembly.Load(wi.AssemblyFullName)
                Catch ex As FileNotFoundException
                    ' Assembly not found; return
                    Return
                End Try

                SelectedWorkflow = typeAsm.GetType(wi.TypeFullName)
                SelectedProfileVersion = TryCast(profileVersionList.SelectedItem, Version)
                Me.DialogResult = Windows.Forms.DialogResult.OK
                Me.Close()
                Return
            End If
        End If
        MessageBox.Show("Please select a workflow and version to continue.", "Error.")
    End Sub

    Private Sub CancelClick(ByVal sender As Object, ByVal e As EventArgs) Handles cancel.Click
        Me.DialogResult = Windows.Forms.DialogResult.Cancel
        Me.Close()
    End Sub
End Class