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
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Tracking
Imports Microsoft.VisualBasic

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)
    Const connectionString As String = "Initial Catalog=Tracking;Data Source=localhost;Integrated Security=SSPI;"

    Shared Sub Main()
        Try
            Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()

                currentWorkflowRuntime.AddService(New SqlTrackingService(connectionString))

                AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

                Dim workflowInstance As WorkflowInstance = currentWorkflowRuntime.CreateWorkflow(GetType(SimpleTrackingWorkflow))
                Dim instanceId As Guid = workflowInstance.InstanceId

                currentWorkflowRuntime.StartRuntime()

                workflowInstance.Start()

                WaitHandle.WaitOne()

                currentWorkflowRuntime.StopRuntime()

                GetInstanceTrackingEvents(instanceId)
                GetActivityTrackingEvents(instanceId)

                Console.WriteLine(vbCrLf + "Done running the workflow.")
            End Using
        Catch ex As Exception
            If Not ex.InnerException Is Nothing Then
                Console.WriteLine(ex.InnerException.Message)
            Else
                Console.WriteLine(ex.Message)
            End If
        End Try
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    Shared Sub GetInstanceTrackingEvents(ByVal instanceId As Guid)
        Dim sqlTrackingQuery As SqlTrackingQuery = New SqlTrackingQuery(connectionString)

        Dim sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance = Nothing
        sqlTrackingQuery.TryGetWorkflow(instanceId, sqlTrackingWorkflowInstance)
        If sqlTrackingWorkflowInstance IsNot Nothing Then
            Console.WriteLine(vbCrLf + "Instance Level Events:" + vbCrLf)

            Dim workflowTrackingRecord As WorkflowTrackingRecord
            For Each workflowTrackingRecord In sqlTrackingWorkflowInstance.WorkflowEvents
                Console.WriteLine("EventDescription : {0}  DateTime : {1}", workflowTrackingRecord.TrackingWorkflowEvent, workflowTrackingRecord.EventDateTime)
            Next
        End If
    End Sub

    Shared Sub GetActivityTrackingEvents(ByVal instanceId As Guid)
        Dim sqlTrackingQuery As SqlTrackingQuery = New SqlTrackingQuery(connectionString)

        Dim sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance = Nothing
        sqlTrackingQuery.TryGetWorkflow(instanceId, sqlTrackingWorkflowInstance)
        If sqlTrackingWorkflowInstance IsNot Nothing Then
            Console.WriteLine(vbCrLf + "Activity Tracking Events:" + vbCrLf)

            Dim activityTrackingRecord As ActivityTrackingRecord
            For Each activityTrackingRecord In sqlTrackingWorkflowInstance.ActivityEvents
                Console.WriteLine("StatusDescription : {0}  DateTime : {1} Activity Qualified ID : {2}", activityTrackingRecord.ExecutionStatus, activityTrackingRecord.EventDateTime, activityTrackingRecord.QualifiedName)
            Next
        End If
    End Sub
End Class


