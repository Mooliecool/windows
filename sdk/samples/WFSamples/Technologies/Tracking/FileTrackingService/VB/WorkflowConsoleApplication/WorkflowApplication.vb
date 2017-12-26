'----------------------------------------------------------------------
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
'----------------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.Threading
Imports System.Workflow.Runtime

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        Try
            ' Create WorkflowRuntime
            Using workflowRuntime As New WorkflowRuntime()
                ' Add SimpleFileTrackingService
                workflowRuntime.AddService(New SimpleFileTrackingService())

                ' Subscribe to Workflow Completed, Suspended, and Terminated WorkflowRuntime Event
                AddHandler workflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler workflowRuntime.WorkflowSuspended, AddressOf OnWorkflowSuspended
                AddHandler workflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

                ' Start WorkflowRuntime
                workflowRuntime.StartRuntime()

                ' Execute the SimpleWorkflow Workflow
                Console.WriteLine("Executing the SimpleWorkflow...")
                workflowRuntime.StartRuntime()
                Dim simpleWorkflowInstance As WorkflowInstance = workflowRuntime.CreateWorkflow(GetType(SimpleWorkflow))
                simpleWorkflowInstance.Start()
                ' Wait for the Workflow Completion
                WaitHandle.WaitOne()

                ' Execute the SuspendedWorkflow Workflow
                Console.WriteLine("Executing the SuspendedWorkflow...")
                workflowRuntime.StartRuntime()
                Dim suspendedWorkflowInstance As WorkflowInstance = workflowRuntime.CreateWorkflow(GetType(SuspendedWorkflow))
                suspendedWorkflowInstance.Start()
                ' Wait for the Workflow Suspension
                WaitHandle.WaitOne()

                ' Execute the ExceptionWorkflow Workflow
                Console.WriteLine("Executing the ExceptionWorkflow...")
                workflowRuntime.StartRuntime()
                Dim exceptionWorkflowInstance As WorkflowInstance = workflowRuntime.CreateWorkflow(GetType(ExceptionWorkflow))
                exceptionWorkflowInstance.Start()
                ' Wait for the Workflow Termination
                WaitHandle.WaitOne()

                ' Stop Runtime
                workflowRuntime.StopRuntime()
            End Using
        Catch ex As Exception
            Console.WriteLine(vbCrLf + "Exception:" + vbCrLf + vbTab + "Message: {0}" + vbCrLf + vbTab + "Source: {1}", ex.Message, ex.Source)
        End Try
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        Console.WriteLine("Workflow Completed")
        waitHandle.Set()
    End Sub

    Shared Sub OnWorkflowSuspended(ByVal sender As Object, ByVal e As WorkflowSuspendedEventArgs)
        Console.WriteLine("Workflow Suspended")
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine("Workflow Terminated")
        WaitHandle.Set()
    End Sub

End Class


