'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'  This source code is intended only as a supplement to Microsoft
'  Development Tools and/or on-line documentation.  See these other
'  materials for detailed information regarding Microsoft code samples.
' 
'  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'  PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Threading
Imports System.Windows.Forms
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel
Imports System.Workflow.Runtime


<Serializable()> _
Public Class TaskEventArgs
    Inherits ExternalDataEventArgs
    Private idValue As String
    Private assigneeValue As String
    Private textValue As String

    Public Sub New(ByVal instanceId As Guid, ByVal taskId As String, ByVal assignee As String, ByVal text As String)
        MyBase.New(instanceId)
        Me.idValue = taskId
        Me.assigneeValue = assignee
        Me.textValue = text
    End Sub

    Public Property Id() As String
        Get
            Return Me.idValue
        End Get
        Set(ByVal Value As String)
            Me.idValue = Value
        End Set
    End Property

    Public Property Assignee() As String
        Get
            Return Me.assigneeValue
        End Get
        Set(ByVal Value As String)
            Me.assigneeValue = Value
        End Set
    End Property

    Public Property Text() As String
        Get
            Return Me.textValue
        End Get
        Set(ByVal Value As String)
            Me.textValue = Value
        End Set
    End Property
End Class

' To identify the corrected TaskCompleted activity after the CreateTask, 
' the CorrelationParameter is set on taskID parameter

' The ExternalDataExchange attribute is a required attribute 
' indicating that the local service participates in data exchange with a workflow
<ExternalDataExchange()> _
<CorrelationParameter("taskId")> _
Public Interface ITaskService
    ' The CorrelationInitializer attribute indicates that the 
    ' attributed method is the one that initializes the correlation value 
    ' and hence must appear first in the workflow (before other operations defined on the 
    ' local service interface) in order for validation to succeed.
    <CorrelationInitializer()> _
    Sub CreateTask(ByVal taskId As String, ByVal assignee As String, ByVal text As String)

    ' The CorrelationAlias attribute overrides the 
    ' CorrelationParameter setting for a specific method or event
    ' when the correlation value must be obtained from a location other 
    ' than that indicated by the CorrelationParameter attribute.
    <CorrelationAlias("taskId", "e.Id")> _
    Event TaskCompleted As EventHandler(Of TaskEventArgs)
End Interface

Public Class TaskService
    Implements ITaskService

    Public Sub Task(ByVal args As TaskEventArgs)
        RaiseEvent TaskCompleted(Nothing, args)
    End Sub

    Public Sub CreateTask(ByVal taskId As String, ByVal assignee As String, ByVal text As String) Implements ITaskService.CreateTask
        Console.WriteLine("task " + taskId + " created for " + assignee)

        ThreadPool.QueueUserWorkItem(AddressOf ShowDialog, New TaskEventArgs(WorkflowEnvironment.WorkflowInstanceId, taskId, assignee, text))
    End Sub

    Public Sub ShowDialog(ByVal state As Object)
        Dim taskEventArgs As TaskEventArgs = CType(state, TaskEventArgs)

        MessageBox.Show(String.Format("{0}, click OK when '{1}' completed.", taskEventArgs.Assignee, taskEventArgs.Text), String.Format("Task {0}", taskEventArgs.Id), MessageBoxButtons.OK)

        Task(taskEventArgs)
    End Sub

    Public Event TaskCompleted As EventHandler(Of TaskEventArgs) Implements ITaskService.TaskCompleted
End Class