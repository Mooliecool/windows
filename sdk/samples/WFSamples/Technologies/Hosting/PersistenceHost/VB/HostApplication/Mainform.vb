'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Collections.Generic
Imports System.Collections.Specialized
Imports System.IO
Imports System.Text
Imports System.Windows.Forms
Imports System.Workflow.Activities
Imports System.Workflow.Runtime
Imports System.Xml

Public Class Mainform
    Private runtime As WorkflowRuntime
    Private documentService As DocumentApprovalService
    Const instanceFilename As String = "workflowInstances.xml"

    Sub New()

        ' This call is required by the Windows Form Designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.


        Me.runtime = New WorkflowRuntime()

        AddHandler runtime.WorkflowCompleted, AddressOf RuntimeWorkflowCompleted

        ' Set up runtime to unload workflow instance from memory to file using FilePersistenceService
        Dim filePersistence As New FilePersistenceService(True)
        runtime.AddService(filePersistence)

        ' Add document approval service
        Dim dataService As New ExternalDataExchangeService()
        runtime.AddService(dataService)
        documentService = New DocumentApprovalService(Me)
        dataService.AddService(documentService)

        ' Search for workflows that have previously been persisted to file, and load into the listview control.
        ' These workflows will be reloaded by the runtime when events are raised against them.
        LoadWorkflowData()

        ' Start the runtime
        runtime.StartRuntime()

    End Sub

    Friend Sub DocumentRequested(ByVal instanceId As Guid, ByVal approver As String)
        Dim addListItem As New AddListViewItem(AddressOf AddListViewItemAsync)
        Invoke(addListItem, instanceId, approver)
    End Sub



    Private Sub RuntimeWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        ' Remove completed workflow from list view
        Dim remove As New RemoveListViewItem(AddressOf RemoveListViewItemAsync)
        Invoke(remove, e.WorkflowInstance.InstanceId)

        ' Remove completed workflow persistence file
        Dim file As New FileInfo(e.WorkflowInstance.InstanceId.ToString())
        file.Delete()
    End Sub


#Region "Workflow Data Persistence"

    Sub LoadWorkflowData()
        Dim reader As New XmlTextReader(instanceFilename)
        Try

            While reader.Read()

                If reader.Name.Equals("WorkflowInstance") Then

                    ListViewExistingRequests.Items.Add( _
                        New ListViewItem( _
                            New String() {reader.GetAttribute("Approver"), _
                                        reader.GetAttribute("InstanceId")}))
                End If

            End While
        Catch e As FileNotFoundException

        End Try

    End Sub
    Sub SaveWorkflowData()

        Dim writer As New XmlTextWriter(instanceFilename, Encoding.Unicode)
        writer.WriteStartElement("WorkflowInstances")
        For Each item As ListViewItem In ListViewExistingRequests.Items
            writer.WriteStartElement("WorkflowInstance")
            writer.WriteAttributeString("Approver", item.SubItems(0).Text)
            writer.WriteAttributeString("InstanceId", item.SubItems(1).Text)
            writer.WriteEndElement()
        Next
        writer.WriteEndElement()
        writer.Flush()
        writer.Close()
    End Sub
#End Region

#Region "Asynchronous control accessors"

    ' Accessing the list view control from other threads (such as are created when the workflow raises an event)
    ' requires use of Invoke so that the calls are threadsafe 
    Private Delegate Sub RemoveListViewItem(ByVal instanceId As Guid)
    Private Sub RemoveListViewItemAsync(ByVal instanceId As Guid)
        For Each item As ListViewItem In ListViewExistingRequests.Items
            If item.SubItems(1).Text.Equals(instanceId.ToString()) Then
                ListViewExistingRequests.Items.Remove(item)
            End If
        Next
    End Sub

    Private Delegate Sub AddListViewItem(ByVal instanceId As Guid, ByVal approver As String)
    Private Sub AddListViewItemAsync(ByVal instanceId As Guid, ByVal approver As String)
        ListViewExistingRequests.Items.Add(New ListViewItem(New String() {approver, instanceId.ToString()}))
    End Sub

#End Region

#Region "Form Events"
    Private Sub Mainform_FormClosing(ByVal sender As Object, ByVal e As FormClosingEventArgs) Handles MyBase.FormClosing
        SaveWorkflowData()
        runtime.StopRuntime()
        runtime.Dispose()
    End Sub


    Private Sub ButtonCreateNewRequest_Click(ByVal sender As Object, ByVal e As EventArgs) Handles ButtonCreateNewRequest.Click
        Dim parameters As New Dictionary(Of String, Object)()
        parameters.Add("Approver", TextBoxApprover.Text)
        Dim instance As WorkflowInstance = runtime.CreateWorkflow(GetType(DocumentApprovalWorkflow), parameters)

        instance.Start()

        TextBoxApprover.Text = String.Empty
    End Sub

    Private Sub ApproveDocumentToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ApproveDocumentToolStripMenuItem.Click
        If ListViewExistingRequests.SelectedItems.Count = 1 Then
            documentService.ApproveDocument( _
                New Guid(ListViewExistingRequests.SelectedItems(0).SubItems(1).Text), _
                ListViewExistingRequests.SelectedItems(0).SubItems(0).Text)
        End If
    End Sub

#End Region

End Class
