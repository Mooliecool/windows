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
Imports Microsoft.VisualBasic

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)
    Shared workflowSuspended As Boolean = False

    Shared Sub Main()
        ' Start the engine.
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            currentWorkflowRuntime.StartRuntime()

            AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler currentWorkflowRuntime.WorkflowSuspended, AddressOf OnWorkflowSuspend
            AddHandler currentWorkflowRuntime.WorkflowResumed, AddressOf OnWorkflowResume
            AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminate

            ' Load the workflow type.
            Dim type As System.Type = GetType(SuspendAndTerminateWorkflow)
            Dim workflowInstance As WorkflowInstance = currentWorkflowRuntime.CreateWorkflow(type)
            workflowInstance.Start()

            WaitHandle.WaitOne()

            If (workflowSuspended) Then
                Console.WriteLine(vbCrLf + "Resuming Workflow Instance")
                workflowInstance.Resume()
                WaitHandle.WaitOne()
            End If

            currentWorkflowRuntime.StopRuntime()
        End Using
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        Console.WriteLine("Workflow Instance Completed")
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowSuspend(ByVal sender As Object, ByVal instance As WorkflowSuspendedEventArgs)
        workflowSuspended = True
        Console.WriteLine(vbLf + "Workflow Suspend event raised")
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowResume(ByVal sender As Object, ByVal instance As WorkflowEventArgs)
        Console.WriteLine(vbLf + "Workflow Resumed event raised")
    End Sub

    Shared Sub OnWorkflowTerminate(ByVal sender As Object, ByVal instance As WorkflowTerminatedEventArgs)
        Console.WriteLine(vbLf + "Workflow Terminated event raised")
        WaitHandle.Set()
    End Sub

End Class


