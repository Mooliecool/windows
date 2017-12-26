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
    ' A workflow is always run asychronously; the main thread waits on this event so the program
    ' doesn't exit before the workflow completes
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        ' Create the WorkflowRuntime
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            currentWorkflowRuntime.StartRuntime()

            ' Set up the WorkflowRuntime events so that the host gets notified when the workflow
            ' completes and terminates
            AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

            ' Load the workflow type
            Dim type As System.Type = GetType(ThrowWorkflow)
            ' Start the workflow and wait for it to complete
            currentWorkflowRuntime.CreateWorkflow(type).Start()
            Console.WriteLine("Workflow Started.")

            WaitHandle.WaitOne()

            currentWorkflowRuntime.StopRuntime()

            Console.WriteLine("Workflow Completed.")
        End Using
    End Sub

    ' This method is called when the workflow succesfully completes - in this sample this will never be called
    ' since the workflow throws an unhandled exception
    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        WaitHandle.Set()
    End Sub

    ' This method is called when the workflow is terminated
    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine("Workflow Terminated.")
        Console.WriteLine("   Exception - Reason: " + e.Exception.Message)
        WaitHandle.Set()
    End Sub

End Class


