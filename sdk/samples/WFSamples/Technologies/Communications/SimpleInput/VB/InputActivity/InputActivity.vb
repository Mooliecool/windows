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
Imports System.ComponentModel
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities

<ToolboxItemAttribute(GetType(ActivityToolboxItem))> _
Public Class InputActivity
    Inherits System.Workflow.ComponentModel.Activity

    Protected Overrides Sub Initialize(ByVal provider As System.IServiceProvider)
        Dim qService As WorkflowQueuingService = CType(provider.GetService(GetType(WorkflowQueuingService)), WorkflowQueuingService)
        Dim qName As IComparable = Me.Queue

        If Not qService.Exists(qName) Then
            Dim transactional As Boolean = True
            qService.CreateWorkflowQueue(qName, transactional)
        End If
    End Sub

    Protected Overrides Sub Uninitialize(ByVal provider As System.IServiceProvider)
        Dim qService As WorkflowQueuingService = CType(provider.GetService(GetType(WorkflowQueuingService)), WorkflowQueuingService)
        Dim qName As IComparable = Me.Queue

        If qService.Exists(qName) Then
            If 0 = qService.GetWorkflowQueue(qName).Count Then
                qService.DeleteWorkflowQueue(qName)
            End If
        End If

        MyBase.Uninitialize(provider)
    End Sub

    Protected Overrides Function Execute(ByVal context As System.Workflow.ComponentModel.ActivityExecutionContext) As System.Workflow.ComponentModel.ActivityExecutionStatus
        ' WorkflowQueuingService is a component that allows access to queue items
        ' enqueued to a particular workflow instance
        ' Items are submitted to a workflow instance from the host using the 
        ' EnqueueItem method call on the workflow instance
        Dim qService As WorkflowQueuingService = CType(context.GetService(Of WorkflowQueuingService)(), WorkflowQueuingService)

        ' The Queue name is in the activity context and specified in the Enqueue item call
        Dim qName As IComparable = Me.Queue

        ' Try to dequeue if there are any items available
        ' Register the handler OnQueueItemAvailable when a item is queued
        If Not TryDequeueAndComplete(context, qName) Then
            AddHandler qService.GetWorkflowQueue(qName).QueueItemAvailable, AddressOf Me.OnQueueItemAvailable
            Return System.Workflow.ComponentModel.ActivityExecutionStatus.Executing
        End If

        Return System.Workflow.ComponentModel.ActivityExecutionStatus.Closed
    End Function

    ' Handler when a queue item is available
    Private Sub OnQueueItemAvailable(ByVal sender As Object, ByVal args As QueueEventArgs)
        Dim context As ActivityExecutionContext = CType(sender, ActivityExecutionContext)

        Dim qService As WorkflowQueuingService = CType(context.GetService(Of WorkflowQueuingService)(), WorkflowQueuingService)
        Dim qName As IComparable = Me.Queue

        ' Dequeue the item and remove the handler 
        If TryDequeueAndComplete(context, qName) Then
            RemoveHandler qService.GetWorkflowQueue(qName).QueueItemAvailable, AddressOf Me.OnQueueItemAvailable
        End If
    End Sub

    ' Dequeue the item and then process the item
    Private Function TryDequeueAndComplete(ByVal context As ActivityExecutionContext, ByVal queueName As IComparable) As Boolean
        Dim qService As WorkflowQueuingService = CType(context.GetService(Of WorkflowQueuingService)(), WorkflowQueuingService)
        Dim qName As IComparable = Me.Queue

        If qService.GetWorkflowQueue(qName).Count > 0 Then
            Me.DataValue = qService.GetWorkflowQueue(qName).Dequeue()
            Return True
        End If

        Return False
    End Function

    Public Shared ReadOnly QueueProperty As DependencyProperty = DependencyProperty.Register("Queue", GetType(System.String), GetType(InputActivity), New PropertyMetadata(DependencyPropertyOptions.Metadata))

    <DescriptionAttribute("This is the name of the queue")> _
        <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
   <ValidationOption(ValidationOption.Optional)> _
                <BrowsableAttribute(True)> _
                    Public Property Queue() As System.String
        Get
            Return CType(MyBase.GetValue(QueueProperty), [String])

        End Get
        Set(ByVal value As System.String)
            MyBase.SetValue(QueueProperty, value)

        End Set
    End Property
    Public Shared ReadOnly DataValueProperty As DependencyProperty = DependencyProperty.Register("DataValue", GetType(System.Object), GetType(InputActivity))

    <DescriptionAttribute("This is the item that is dequeued")> _
        <DesignerSerializationVisibilityAttribute(DesignerSerializationVisibility.Visible)> _
            <ValidationOptionAttribute(ValidationOption.Optional)> _
                <BrowsableAttribute(True)> _
                    Public Property DataValue() As System.Object
        Get
            Return CType(MyBase.GetValue(DataValueProperty), [Object])

        End Get
        Set(ByVal value As System.Object)
            MyBase.SetValue(DataValueProperty, value)

        End Set
    End Property
End Class
