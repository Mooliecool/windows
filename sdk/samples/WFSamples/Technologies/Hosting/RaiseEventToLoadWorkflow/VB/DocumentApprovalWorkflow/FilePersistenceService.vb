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
Imports System.IO
Imports System.Workflow.ComponentModel
Imports System.Workflow.Runtime.Hosting
Imports System.Collections.Specialized
Imports System.Workflow.Runtime
Imports System.Threading
Imports Microsoft.VisualBasic

Public Class FilePersistenceService
    Inherits WorkflowPersistenceService

    Private unloadOnIdleValue As Boolean = False

    Public Sub New(ByVal unloadOnIdle As Boolean)
        Me.unloadOnIdleValue = unloadOnIdle
    End Sub


    ' Load the completed activity state.
    Protected Overrides Function LoadCompletedContextActivity(ByVal scopeId As System.Guid, ByVal outerActivity As System.Workflow.ComponentModel.Activity) As System.Workflow.ComponentModel.Activity
        Console.WriteLine("Loading completed activity context: {0}", scopeId)
        Dim workflowBytes As Byte() = DeserializeFromFile(scopeId)
        Dim deserializedActivities As Activity = WorkflowPersistenceService.RestoreFromDefaultSerializedForm(workflowBytes, outerActivity)
        Return deserializedActivities
    End Function

    ' Load the workflow instance state
    Protected Overrides Function LoadWorkflowInstanceState(ByVal instanceId As System.Guid) As System.Workflow.ComponentModel.Activity
        Console.WriteLine("Loading instance: {0}" + vbLf, instanceId)
        Dim obj As Object = DeserializeFromFile(instanceId)
        Dim workflowBytes As Byte() = DeserializeFromFile(instanceId)
        Return WorkflowPersistenceService.RestoreFromDefaultSerializedForm(workflowBytes, Nothing)
    End Function

    ' Save the completed activity state
    Protected Overrides Sub SaveCompletedContextActivity(ByVal activity As System.Workflow.ComponentModel.Activity)
        Dim contextGuid As Guid = CType(activity.GetValue(activity.ActivityContextGuidProperty), Guid)
        Console.WriteLine("Saving completed activity context: {0}", contextGuid)
        SerializeToFile( _
            WorkflowPersistenceService.GetDefaultSerializedForm(activity), contextGuid)
    End Sub

    ' Save the workflow instance state at the point of persistence with option of locking instance state if it is shared
    ' across multiple runtimes or multiple phase instance updates.
    Protected Overrides Sub SaveWorkflowInstanceState(ByVal rootActivity As System.Workflow.ComponentModel.Activity, ByVal unlock As Boolean)
        Dim contextGuid As Guid = CType(rootActivity.GetValue(Activity.ActivityContextGuidProperty), Guid)
        Console.WriteLine("Saving instance: {0}" + vbLf, contextGuid)
        SerializeToFile( _
            WorkflowPersistenceService.GetDefaultSerializedForm(rootActivity), contextGuid)

        ' See when the next timer (Delay activity) for this workflow will expire
        Dim timers As TimerEventSubscriptionCollection = CType(rootActivity.GetValue(TimerEventSubscriptionCollection.TimerCollectionProperty), TimerEventSubscriptionCollection)
        Dim subscription As TimerEventSubscription = timers.Peek()
        If subscription IsNot Nothing Then
            ' Set a system timer to automatically reload this workflow when its next timer expires
            Dim timeDifference As TimeSpan = subscription.ExpiresAt - DateTime.UtcNow
            Dim callback As TimerCallback = New TimerCallback(AddressOf ReloadWorkflow)
            Dim timer As Timer = New Timer( _
                callback, _
                subscription.WorkflowInstanceId, _
                CType(IIf(timeDifference < TimeSpan.Zero, TimeSpan.Zero, timeDifference), TimeSpan), _
                New TimeSpan(-1))
        End If
    End Sub

    Private Sub ReloadWorkflow(ByVal id As Object)
        ' Reload the workflow so that it will continue processing
        Me.Runtime.GetWorkflow(CType(id, Guid))
    End Sub

    ' Unlock workflow instance state.  
    ' Instance state locking is necessary when multiple runtimes share an instance persistence store
    Protected Overrides Sub UnlockWorkflowInstanceState(ByVal rootActivity As System.Workflow.ComponentModel.Activity)
        ' File locking is not supported in this sample
    End Sub

    ' Returns the status of the unloadOnIdle flag
    Protected Overrides Function UnloadOnIdle(ByVal rootActivity As System.Workflow.ComponentModel.Activity) As Boolean
        Return unloadOnIdleValue
    End Function

    ' Serialize the activity instance state to file 
    Public Shared Sub SerializeToFile(ByVal workflowBytes As Byte(), ByVal id As Guid)
        Dim filename As String = id.ToString()
        Dim fileStream As FileStream = Nothing

        Try
            If File.Exists(filename) Then
                File.Delete(filename)
            End If

            fileStream = New FileStream(filename, FileMode.CreateNew, FileAccess.Write, FileShare.None)

            ' get the compressed serialized form
            fileStream.Write(workflowBytes, 0, workflowBytes.Length)
        Finally
            If Not fileStream Is Nothing Then
                fileStream.Close()
            End If
        End Try
    End Sub

    ' Deserialize instance state from file given the instance id 
    Public Shared Function DeserializeFromFile(ByVal id As Guid) As Byte()
        Dim filename As String = id.ToString()
        Dim fileStream As FileStream = Nothing

        Dim obj As Object = Nothing
        Try
            ' File opened for shared reads but no writes by anyone
            fileStream = New FileStream(filename, FileMode.Open, FileAccess.Read, FileShare.Read)
            fileStream.Seek(0, SeekOrigin.Begin)
            Dim workflowBytes((CType(fileStream.Length, Integer))) As Byte

            ' Get the serialized form
            fileStream.Read(workflowBytes, 0, workflowBytes.Length)

            Return workflowBytes
        Finally
            fileStream.Close()
        End Try
    End Function
End Class

