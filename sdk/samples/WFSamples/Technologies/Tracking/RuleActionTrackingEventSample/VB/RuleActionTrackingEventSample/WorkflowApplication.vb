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

    Shared waitHandle As New AutoResetEvent(False)

    Shared Sub Main()

        ' Create WorkflowRuntime
        Using workflowRuntime As New WorkflowRuntime()

            ' Add ConsoleTrackingService
            workflowRuntime.AddService(New ConsoleTrackingService())

            ' Subscribe to Workflow Completed WorkflowRuntime Event
            AddHandler workflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted

            ' Subscribe to Workflow Terminated WorkflowRuntime Event
            AddHandler workflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

            ' Start WorkflowRuntime
            workflowRuntime.StartRuntime()

            ' Execute the SampleWorkflow Workflow
            Console.WriteLine("Executing the workflow...")
            workflowRuntime.CreateWorkflow(GetType(SimplePolicyWorkflow)).Start()

            ' Wait for the Workflow Completion
            waitHandle.WaitOne()

            ' Stop Runtime
            workflowRuntime.StopRuntime()
        End Using
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        waitHandle.Set()
    End Sub


    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        waitHandle.Set()
    End Sub
End Class
