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



Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        ' Start the workflow runtime engine.
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            currentWorkflowRuntime.StartRuntime()

            Console.WriteLine("Starting the workflow")

            ' Listen for the workflow events
            AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

            ' Start the workflow and wait for it to complete
            Dim type As System.Type = GetType(DelayWorkflow)
            currentWorkflowRuntime.CreateWorkflow(type).Start()

            WaitHandle.WaitOne()

            ' Stop the workflow runtime engine.
            currentWorkflowRuntime.StopRuntime()
        End Using
    End Sub

    ' This method will be called when a workflow instance is completed
    ' WaitHandle is set so the main thread can continue
    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        WaitHandle.Set()
    End Sub

    ' This method is called when the workflow terminates and does not complete
    ' This should not occur in this sample; however, it is good practice to include a
    ' handler for this event so the host application can manage workflows that are
    ' unexpectedly terminated (e.g. unhandled workflow exception).
    ' WaitHandle is set so the main thread can continue
    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub
End Class


