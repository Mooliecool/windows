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

Public Class WorkflowSelectionForm
    Dim workflowView As New WorkflowDesignerControl()
    Public Sub New()
        InitializeComponent()
        workflowView.AutoSize = True
        workflowView.Dock = DockStyle.Fill
    End Sub

    Dim workflowTypesValue As List(Of Type)

    ' The list of workflow types that need to be displayed
    Public ReadOnly Property WorkflowTypes() As List(Of Type)
        Get
            Return workflowTypesValue
        End Get
    End Property

    Public Sub SetWorkflowTypes(ByVal types As List(Of Type))
        workflowTypesValue = types
    End Sub

    Private selectedWorkflowValue As Type = Nothing

    ' Holds selected workflow after the dialog is closed

    Public ReadOnly Property SelectedWorkflow() As Type
        Get
            Return selectedWorkflowValue
        End Get

    End Property

    Dim listViewWorkflowMap As New Dictionary(Of ListViewItem, Type)()

    Private Sub WorkflowSelectionFormLoad(ByVal sender As Object, ByVal e As EventArgs) Handles Me.Load
        If WorkflowTypes.Count = 1 Then
            Me.workflowListView.Visible = False
            Me.descriptionHeader.Location = Me.workflowListView.Location
            Me.workflowDescription.Location = New Point(Me.workflowDescription.Location.X, Me.workflowDescription.Location.Y + Me.workflowDescription.Height + 10)
            selectedWorkflowValue = WorkflowTypes(0)
            Me.workflowDescription.Text = SelectedWorkflow.FullName
        Else
            For Each t As Type In WorkflowTypes
                Dim item As New ListViewItem(t.Name)
                listViewWorkflowMap.Add(item, t)
                workflowListView.Items.Add(item)
            Next
            workflowListView.Items(0).Selected = True
        End If
        Me.workflowViewPanel.Controls.Add(workflowView)
        Me.workflowView.WorkflowType = SelectedWorkflow
    End Sub

    Private Sub OkButtonClick(ByVal sender As Object, ByVal e As EventArgs) Handles ok.Click
        Me.Close()
    End Sub

    Private Sub CancelButtonClick(ByVal sender As Object, ByVal e As EventArgs) Handles cancel.Click
        Me.selectedWorkflowValue = Nothing
        Me.Close()
    End Sub

    Private Sub WorkflowListViewSelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles workflowListView.SelectedIndexChanged
        If workflowListView.SelectedItems.Count = 0 Then Return
        selectedWorkflowValue = listViewWorkflowMap(workflowListView.SelectedItems(0))
        Me.workflowDescription.Text = SelectedWorkflow.FullName
        Me.workflowDescription.Visible = True
        workflowView.WorkflowType = SelectedWorkflow
    End Sub
End Class