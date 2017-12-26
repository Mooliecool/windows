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
Imports System.Threading
Imports System.Workflow.Activities
Imports System.Workflow.Runtime
Imports System.Collections.Specialized
Imports Microsoft.VisualBasic

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)
    Shared documentApprover As DocumentApprovalService = New DocumentApprovalService()

    Shared Sub Main()
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            Try
                ' Set up the runtime to unload the workflow instance from memory to file using FilePersistenceService
                currentWorkflowRuntime.AddService(New FilePersistenceService(True))

                ' Add the document approval service
                Dim dataService As ExternalDataExchangeService = New ExternalDataExchangeService()
                currentWorkflowRuntime.AddService(dataService)
                dataService.AddService(documentApprover)

                AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
                AddHandler currentWorkflowRuntime.ServicesExceptionNotHandled, AddressOf OnExceptionNotHandled
                AddHandler currentWorkflowRuntime.WorkflowIdled, AddressOf OnWorkflowIdled

                ' Start the engine
                currentWorkflowRuntime.StartRuntime()

                ' Load the workflow type
                Dim type As System.Type = GetType(DocumentApprovalWorkflow)
                currentWorkflowRuntime.CreateWorkflow(type).Start()

                ' WaitHandle blocks so that the program does not exit until the workflow completes
                WaitHandle.WaitOne()
            Catch ex As Exception
                Console.WriteLine("Exception" + vbCrLf + vbTab + "Source: {0}" + vbCrLf + vbTab + "Message: {1}", ex.Source, ex.Message)
            Finally
                currentWorkflowRuntime.StopRuntime()
                Console.WriteLine("Workflow runtime stopped, program exiting... " + vbLf)
            End Try
        End Using
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        ' Signal the waitHandle because the workflow completed
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowIdled(ByVal sender As Object, ByVal e As System.Workflow.Runtime.WorkflowEventArgs)
        ' Raise the DocumentApproved event
        Console.WriteLine(vbLf + "Host:  RaiseDocumentApproval event to load workflow")
        documentApprover.RaiseDocumentApproved(e.WorkflowInstance.InstanceId, "Approver")
    End Sub

    Shared Sub OnExceptionNotHandled(ByVal sender As Object, ByVal e As System.Workflow.Runtime.ServicesExceptionNotHandledEventArgs)
        Console.WriteLine("Unhandled Workflow Exception ")
        Console.WriteLine("  Type: " + e.GetType().ToString())
        Console.WriteLine("  Message: " + e.Exception.Message)
    End Sub
End Class

' local service
Public Class DocumentApprovalService
    Implements IDocumentApproval

    Private documentId As Guid
    Private approver As String

    ' Received the document approval
    Public Event DocumentApproved(ByVal sender As Object, ByVal e As DocumentEventArgs) Implements IDocumentApproval.DocumentApproved

    ' Send the document for approval
    Public Sub RequestDocumentApproval(ByVal documentId As System.Guid, ByVal approver As String) Implements IDocumentApproval.RequestDocumentApproval
        Me.documentId = documentId
        Me.approver = approver

        Console.WriteLine("Host:  Sending document for approval")
    End Sub

    ' Method to raise a DocumentApproved event to workflow
    Public Sub RaiseDocumentApproved(ByVal documentId As Guid, ByVal approver As String)
        Me.documentId = documentId
        Me.approver = approver

        ThreadPool.QueueUserWorkItem(New WaitCallback(AddressOf RaiseDocumentApprovalEvent), CType(Me, DocumentApprovalService))
    End Sub

    Private Sub RaiseDocumentApprovalEvent(ByVal stateInfo As Object)
        Console.WriteLine(vbLf + "Host:  Loading workflow due to event firing...")
        Dim da As DocumentApprovalService = CType(stateInfo, DocumentApprovalService)
        RaiseEvent DocumentApproved(da.approver, New DocumentEventArgs(da.documentId))
    End Sub
End Class