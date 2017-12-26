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
Imports System.IO
Imports System.Workflow.Runtime

Public Class FileWatcherService
    Private subscriptions As Dictionary(Of String, FileWatcherSubscription)
    Private runtime As WorkflowRuntime


    Public Sub New(ByVal runtime As WorkflowRuntime)
        Me.runtime = runtime
        Me.subscriptions = New Dictionary(Of String, FileWatcherSubscription)()
    End Sub

    Public Function RegisterListener(ByVal queueName As IComparable, ByVal path As String, ByVal filter As String, ByVal notifyFilter As NotifyFilters, ByVal includeSubdirectories As Boolean) As Guid
        Dim fileSystemWatcher As New FileSystemWatcher()

        Try
            fileSystemWatcher.Path = path
        Catch ex As ArgumentException
            Console.WriteLine("Path '{0}' not found.  Please see documentation for setup steps.", path)
            Return Guid.Empty
        End Try

        fileSystemWatcher.Filter = filter
        fileSystemWatcher.NotifyFilter = notifyFilter
        fileSystemWatcher.IncludeSubdirectories = includeSubdirectories

        AddHandler fileSystemWatcher.Changed, AddressOf FileSystemWatcherHandler
        AddHandler fileSystemWatcher.Created, AddressOf FileSystemWatcherHandler
        AddHandler fileSystemWatcher.Deleted, AddressOf FileSystemWatcherHandler
        AddHandler fileSystemWatcher.Error, AddressOf FileSystemWatcherError

        Dim subscription As New FileWatcherSubscription(fileSystemWatcher, WorkflowEnvironment.WorkflowInstanceId, queueName)
        Dim subscriptionId As Guid = Guid.NewGuid()

        SyncLock Me.subscriptions
            Me.subscriptions.Add(subscriptionId.ToString(), subscription)
        End SyncLock

        ' Turn the file system watcher on
        fileSystemWatcher.EnableRaisingEvents = True

        Console.WriteLine("FileWatcherService subscription '" + subscriptionId.ToString() + "' created")

        Return subscriptionId
    End Function

    Public Sub UnregisterListener(ByVal subscriptionId As Guid)
        Dim key As String = subscriptionId.ToString()

        SyncLock Me.subscriptions
            If Me.subscriptions.ContainsKey(key) Then
                Dim watcher As FileSystemWatcher = Me.subscriptions(key).FileSystemWatcher
                ' Turn the file system watcher off
                watcher.EnableRaisingEvents = False
                watcher.Dispose()

                Me.subscriptions.Remove(key)
                Console.WriteLine("FileWatcherService subscription '" + key + "' removed")
            Else
                Console.WriteLine("FileWatcherService subscription '" + key + "' not found")
            End If
        End SyncLock

    End Sub

    Private Sub FileSystemWatcherError(ByVal sender As Object, ByVal e As ErrorEventArgs)

    End Sub

    Private Sub FileSystemWatcherHandler(ByVal sender As Object, ByVal e As FileSystemEventArgs)
        ' Sender is the FileSystemWatcher that raised this event
        Dim fileSystemWatcher As FileSystemWatcher = CType(sender, FileSystemWatcher)
        SyncLock Me.subscriptions
            For Each subscription As FileWatcherSubscription In Me.subscriptions.Values
                If subscription.FileSystemWatcher.Equals(fileSystemWatcher) Then
                    Me.DeliverToWorkflow(subscription, e)
                    Return

                End If
            Next
        End SyncLock
    End Sub

    Private Sub DeliverToWorkflow(ByVal subscription As FileWatcherSubscription, ByVal fileSystemEventArgs As FileSystemEventArgs)
        Try
            ' We can't just use the FileSystemEventArgs because it's not serializable
            Dim eventArgs As New FileWatcherEventArgs(fileSystemEventArgs)

            Dim workflowInstance As WorkflowInstance = Me.runtime.GetWorkflow(subscription.WorkflowInstanceId)
            workflowInstance.EnqueueItem(subscription.QueueName, eventArgs, Nothing, Nothing)
        Catch e As Exception
            ' Write the exception out to the Debug console and throw the exception
            System.Diagnostics.Debug.WriteLine(e)
            Throw e
        End Try
    End Sub

End Class
