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
Imports System.IO
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime

'NOTE: When changing the namespace; please update XmlnsDefinitionAttribute in AssemblyInfo.vb
<ToolboxItemAttribute(GetType(ActivityToolboxItem))> _
<DefaultProperty("Path")> _
Public Class FileSystemEvent
    Inherits Activity
    Implements IEventActivity, IActivityEventListener(Of QueueEventArgs)

#Region "Default values"
    Public Const FilterDefaultValue As String = "*.*"

    Public Const NotifyFilterDefaultValue As NotifyFilters = _
        System.IO.NotifyFilters.LastWrite Or _
        System.IO.NotifyFilters.FileName Or _
        System.IO.NotifyFilters.DirectoryName

    Public Const IncludeSubdirectoriesDefaultValue As Boolean = True

#End Region

#Region "Private data"

    Private subscriptionId As Guid = Guid.Empty
    Private queueNameValue As IComparable

#End Region

#Region "Dependency Properties"

    Public Shared PathProperty As DependencyProperty = DependencyProperty.Register("Path", GetType(String), GetType(FileSystemEvent))
    Public Shared FilterProperty As DependencyProperty = DependencyProperty.Register("Filter", GetType(String), GetType(FileSystemEvent), New PropertyMetadata(FileSystemEvent.FilterDefaultValue))
    Public Shared NotifyFilterProperty As DependencyProperty = DependencyProperty.Register("NotifyFilter", GetType(System.IO.NotifyFilters), GetType(FileSystemEvent), New PropertyMetadata(FileSystemEvent.NotifyFilterDefaultValue))
    Public Shared IncludeSubdirectoriesProperty As DependencyProperty = DependencyProperty.Register("IncludeSubdirectories", GetType(Boolean), GetType(FileSystemEvent), New PropertyMetadata(FileSystemEvent.IncludeSubdirectoriesDefaultValue))

    Public Shared FileWatcherBeforeEvent As DependencyProperty = DependencyProperty.Register("FileWatcherBefore", GetType(EventHandler(Of EventArgs)), GetType(FileSystemEvent))
    Public Shared FileWatcherEventHandlerEvent As DependencyProperty = DependencyProperty.Register("FileWatcherEventHandler", GetType(EventHandler(Of FileWatcherEventArgs)), GetType(FileSystemEvent))

#End Region

#Region "Activity Properties"

    ' Required by IEventActivity
    <Browsable(False)> _
    Public ReadOnly Property QueueName() As IComparable Implements System.Workflow.Activities.IEventActivity.QueueName
        Get
            Return Me.queueNameValue
        End Get
    End Property

    <Category("File System Watcher")> _
    <DefaultValue(FileSystemEvent.FilterDefaultValue)> _
    Public Property Filter() As String
        Get
            Return CType(MyBase.GetValue(FileSystemEvent.FilterProperty), String)
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(FileSystemEvent.FilterProperty, value)
        End Set
    End Property


    <Category("File System Watcher")> _
    <TypeConverterAttribute("System.Diagnostics.Design.StringValueConverter, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a")> _
    <EditorAttribute("System.Diagnostics.Design.FSWPathEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", "System.Drawing.Design.UITypeEditor, System.Drawing, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a")> _
    Public Property Path() As String
        Get
            Return CType(MyBase.GetValue(FileSystemEvent.PathProperty), String)
        End Get
        Set(ByVal value As String)
            MyBase.SetValue(FileSystemEvent.PathProperty, value)
        End Set
    End Property


    <Category("File System Watcher")> _
    <DefaultValue(FileSystemEvent.IncludeSubdirectoriesDefaultValue)> _
    Public Property IncludeSubdirectories() As Boolean
        Get
            Return CType(MyBase.GetValue(FileSystemEvent.IncludeSubdirectoriesProperty), Boolean)
        End Get
        Set(ByVal value As Boolean)
            MyBase.SetValue(FileSystemEvent.IncludeSubdirectoriesProperty, value)
        End Set
    End Property


    <Category("File System Watcher")> _
    <DefaultValue(FileSystemEvent.NotifyFilterDefaultValue)> _
    Public Property NotifyFilter() As System.IO.NotifyFilters
        Get
            Return CType(MyBase.GetValue(FileSystemEvent.NotifyFilterProperty), System.IO.NotifyFilters)
        End Get
        Set(ByVal value As System.IO.NotifyFilters)
            MyBase.SetValue(FileSystemEvent.NotifyFilterProperty, value)
        End Set
    End Property


    <Description("This event is raised when the FileSystemEvent activity begins executing.")> _
    <Browsable(True)> _
    <Category("Handlers")> _
    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)> _
    Public Custom Event FileWatcherBefore As EventHandler(Of EventArgs)
        AddHandler(ByVal value As EventHandler(Of EventArgs))
            MyBase.AddHandler(FileWatcherBeforeEvent, value)
        End AddHandler

        RemoveHandler(ByVal value As EventHandler(Of EventArgs))
            MyBase.RemoveHandler(FileWatcherBeforeEvent, value)
        End RemoveHandler

        RaiseEvent(ByVal sender As Object, ByVal e As System.EventArgs)

        End RaiseEvent
    End Event


    <Description("This event is raised when a file system event has occurred.")> _
    <Browsable(True)> _
    <Category("Handlers")> _
    <DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)> _
    Public Custom Event FileWatcherEventHandler As EventHandler(Of FileWatcherEventArgs)
        AddHandler(ByVal value As EventHandler(Of FileWatcherEventArgs))
            MyBase.AddHandler(FileWatcherEventHandlerEvent, value)
        End AddHandler

        RemoveHandler(ByVal value As EventHandler(Of FileWatcherEventArgs))
            MyBase.RemoveHandler(FileWatcherEventHandlerEvent, value)
        End RemoveHandler

        RaiseEvent(ByVal sender As Object, ByVal e As FileWatcherEventArgs)

        End RaiseEvent
    End Event
#End Region

#Region "Activity Execution Logic"

    Protected Overrides Sub Initialize(ByVal provider As IServiceProvider)
        Console.WriteLine("Initialize")
        Me.queueNameValue = Me.Name + Guid.NewGuid().ToString()
    End Sub

    Protected Overrides Function Execute(ByVal context As ActivityExecutionContext) As ActivityExecutionStatus
        Console.WriteLine("Execute")
        MyBase.RaiseGenericEvent(Of EventArgs)(FileSystemEvent.FileWatcherBeforeEvent, Me, EventArgs.Empty)

        If (Me.ProcessQueueItem(context)) Then
            Return ActivityExecutionStatus.Closed
        End If

        Me.DoSubscribe(context, Me)
        Return ActivityExecutionStatus.Executing
    End Function


    Sub Subscribe(ByVal parentContext As ActivityExecutionContext, ByVal parentEventHandler As IActivityEventListener(Of QueueEventArgs)) Implements IEventActivity.Subscribe
        Console.WriteLine("Subscribe")
        DoSubscribe(parentContext, parentEventHandler)
    End Sub

    Protected Overrides Function Cancel(ByVal context As ActivityExecutionContext) As ActivityExecutionStatus
        Console.WriteLine("Cancel")
        DoUnsubscribe(context, Me)
        DeleteQueue(context)
        Return ActivityExecutionStatus.Closed
    End Function

    Sub Unsubscribe(ByVal parentContext As ActivityExecutionContext, ByVal parentEventHandler As IActivityEventListener(Of QueueEventArgs)) Implements IEventActivity.Unsubscribe
        Console.WriteLine("Unsubscribe")
        DoUnsubscribe(parentContext, parentEventHandler)
    End Sub

    Private Function CreateQueue(ByVal context As ActivityExecutionContext) As WorkflowQueue
        Console.WriteLine("CreateQueue")
        Dim qService As WorkflowQueuingService = context.GetService(Of WorkflowQueuingService)()

        If Not qService.Exists(Me.QueueName) Then
            qService.CreateWorkflowQueue(Me.QueueName, True)
        End If

        Return qService.GetWorkflowQueue(Me.QueueName)
    End Function

    Private Sub DeleteQueue(ByVal context As ActivityExecutionContext)
        Console.WriteLine("DeleteQueue")
        Dim qService As WorkflowQueuingService = context.GetService(Of WorkflowQueuingService)()
        qService.DeleteWorkflowQueue(Me.QueueName)
    End Sub


    Private Function DoSubscribe(ByVal context As ActivityExecutionContext, ByVal listener As IActivityEventListener(Of QueueEventArgs)) As Boolean
        Dim queue As WorkflowQueue = CreateQueue(context)
        queue.RegisterForQueueItemAvailable(listener)

        Dim fileService As FileWatcherService = context.GetService(Of FileWatcherService)()
        Me.subscriptionId = fileService.RegisterListener(Me.QueueName, Me.Path, Me.Filter, Me.NotifyFilter, Me.IncludeSubdirectories)
        Return Not subscriptionId = Guid.Empty

    End Function

    Private Sub DoUnsubscribe(ByVal context As ActivityExecutionContext, ByVal listener As IActivityEventListener(Of QueueEventArgs))
        If Not Me.subscriptionId.Equals(Guid.Empty) Then
            Dim fileService As FileWatcherService = context.GetService(Of FileWatcherService)()
            fileService.UnregisterListener(Me.subscriptionId)
            Me.subscriptionId = Guid.Empty
        End If

        Dim qService As WorkflowQueuingService = context.GetService(Of WorkflowQueuingService)()
        Dim queue As WorkflowQueue = qService.GetWorkflowQueue(Me.QueueName)
        queue.UnregisterForQueueItemAvailable(listener)
    End Sub


    Sub OnEvent(ByVal sender As Object, ByVal e As QueueEventArgs) Implements IActivityEventListener(Of QueueEventArgs).OnEvent
        Console.WriteLine("OnQueueItemAvailable #1")
        ' If activity is not scheduled for execution, do nothing
        If Me.ExecutionStatus = ActivityExecutionStatus.Executing Then
            Console.WriteLine("OnQueueItemAvailable #2")
            Dim context As ActivityExecutionContext = TryCast(sender, ActivityExecutionContext)
            If Me.ProcessQueueItem(context) Then
                context.CloseActivity()
            End If
        End If
    End Sub

    Private Function ProcessQueueItem(ByVal context As ActivityExecutionContext) As Boolean
        Dim qService As WorkflowQueuingService = context.GetService(Of WorkflowQueuingService)()


        If Not qService.Exists(Me.QueueName) Then
            Return False
        End If

        Dim queue As WorkflowQueue = qService.GetWorkflowQueue(Me.QueueName)

        ' If the queue has messages, then process the first one
        If queue.Count = 0 Then
            Return False
        End If

        Dim e As FileWatcherEventArgs = CType(queue.Dequeue(), FileWatcherEventArgs)

        ' Raise the FileSystemEvent
        MyBase.RaiseGenericEvent(Of FileWatcherEventArgs)(FileSystemEvent.FileWatcherEventHandlerEvent, Me, e)
        DoUnsubscribe(context, Me)
        DeleteQueue(context)
        Return True
    End Function


#End Region

End Class
