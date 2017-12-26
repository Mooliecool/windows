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
Imports System.Workflow.Runtime.Hosting
Imports Microsoft.VisualBasic

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            Try
                ' A workflow is always run asychronously; the main thread waits on this event so the program
                ' doesn't exit before the workflow completes
                currentWorkflowRuntime.AddService(New SqlWorkflowPersistenceService("Initial Catalog=SqlPersistenceService;Data Source=localhost;Integrated Security=SSPI;"))

                currentWorkflowRuntime.StartRuntime()

                ' Listen for the workflow events
                AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
                AddHandler currentWorkflowRuntime.WorkflowAborted, AddressOf OnWorkflowAborted

                ' Create an instance of the workflow
                Dim type As System.Type = GetType(NestedExceptionsWorkflow)
                currentWorkflowRuntime.CreateWorkflow(type).Start()
                Console.WriteLine("Workflow Started." + vbLf)

                ' Wait for the event to be signaled
                WaitHandle.WaitOne()
            Catch ex As Exception
                Console.WriteLine("Source: {0}" + vbLf + "Message: {1}", ex.Source, ex.Message)
            Finally
                currentWorkflowRuntime.StopRuntime()
                Console.WriteLine(vbLf + "Workflow Complete.")
            End Try
        End Using
    End Sub

    ' This method is called when the workflow terminates and does not complete
    ' This should not occur in this sample; however, it is good practice to include a
    ' handler for this event so the host application can manage workflows that are
    ' unexpectedly terminated (e.g. unhandled workflow exception).
    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        WaitHandle.Set()
    End Sub

    ' This method will be called when a workflow instance is completed
    ' waitHandle is set so the main thread can continue
    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    ' This method will be called when a workflow is aborted, such as by a failed database connection.
    Shared Sub OnWorkflowAborted(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("Ending workflow...")
        WaitHandle.Set()
    End Sub
End Class


