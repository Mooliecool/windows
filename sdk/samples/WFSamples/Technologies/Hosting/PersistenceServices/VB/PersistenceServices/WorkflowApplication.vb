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

'This sample demonstrates how to use SqllWorkflowPersistenceService and
' how to manage workflow lifetime using the WorkflowRuntime events.  Once the workflow instance is
' idle due to the delay1 activity, the Host unloads the instance from memory.  When the timer expires
' the Workflow is automatically loaded and it continues to execute.

Class WorkflowApplication
    ' A workflow is always run asychronously; the main thread waits on this event so the program
    ' does not exit before the workflow completes.
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        Try
            ' Create the WorkflowRuntime
            Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()

                ' Add the SqlWorkflowPersistenceService service
                Dim persistenceService As New SqlWorkflowPersistenceService( _
                "Initial Catalog=SqlPersistenceService;Data Source=localhost;Integrated Security=SSPI;", _
                False, _
                New TimeSpan(1, 0, 0), _
                New TimeSpan(0, 0, 10))

                currentWorkflowRuntime.AddService(persistenceService)

                currentWorkflowRuntime.StartRuntime()

                ' Set up the WorkflowRuntime event handlers
                AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler currentWorkflowRuntime.WorkflowIdled, AddressOf OnWorkflowIdled
                AddHandler currentWorkflowRuntime.WorkflowPersisted, AddressOf OnWorkflowPersisted
                AddHandler currentWorkflowRuntime.WorkflowUnloaded, AddressOf OnWorkflowUnloaded
                AddHandler currentWorkflowRuntime.WorkflowLoaded, AddressOf OnWorkflowLoaded
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
                AddHandler currentWorkflowRuntime.WorkflowAborted, AddressOf OnWorkflowAborted

                ' Load the workflow type
                Dim type As System.Type = GetType(PersistenceServicesWorkflow)
                ' Create an instance of the workflow
                Console.WriteLine("Workflow Started.")
                currentWorkflowRuntime.CreateWorkflow(type).Start()


                ' Wait for the event to be signaled
                WaitHandle.WaitOne()

                ' Stop the runtime
                currentWorkflowRuntime.StopRuntime()

                Console.WriteLine("Program Complete.")
            End Using
        Catch ex As Exception
            Console.WriteLine("Application exception occurred: " + ex.Message)
        End Try
    End Sub

    ' This method is called when a workflow instance is completed; because only a single instance is 
    ' started, the event arguments are ignored and the waitHandle is signaled so the main thread can continue.
    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        WaitHandle.Set()
    End Sub

    ' It is good practice to provide a handler for the WorkflowTerminated event
    ' so the host application can manage unexpected problems during workflow execution
    ' such as database connectivity issues, networking issues, and so on.
    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    ' Called when the workflow is loaded back into memory - in this sample this occurs when the timer expires
    Shared Sub OnWorkflowLoaded(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("Workflow was loaded.")
    End Sub

    ' Called when the workflow is unloaded from memory - in this sample the workflow instance is unloaded by the application
    ' in the UnloadInstance method below.
    Shared Sub OnWorkflowUnloaded(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("Workflow was unloaded.")
    End Sub

    ' Called when the workflow is persisted - in this sample when it is unloaded and completed
    Shared Sub OnWorkflowPersisted(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("Workflow was persisted.")
    End Sub

    ' Called when the workflow aborts, such as when the database connection fails
    Shared Sub OnWorkflowAborted(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("Ending workflow...")
        WaitHandle.Set()
    End Sub

    ' Called when the workflow is idle - in this sample this occurs when the workflow is waiting on the
    ' delay1 activity to expire
    Shared Sub OnWorkflowIdled(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("Workflow is idle.")
        e.WorkflowInstance.TryUnload()
    End Sub
End Class

