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
Imports System.Configuration
Imports System.Threading
Imports System.Workflow.Activities
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Hosting

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)
    Const amountParameter As String = "Amount"

    Shared Sub Main()
        ' Instanciate and configure workflow runtime
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            currentWorkflowRuntime.AddService( _
                New SqlWorkflowPersistenceService( _
                    ConfigurationManager.AppSettings("ConnectionString"), _
                    True, _
                    New TimeSpan(0, 10, 0), _
                    New TimeSpan(0, 0, 5)))

            ' Subscribe to workflow events
            AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler currentWorkflowRuntime.WorkflowIdled, AddressOf OnWorkflowIdled
            AddHandler currentWorkflowRuntime.ServicesExceptionNotHandled, AddressOf onExceptionNotHandled
            AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
            AddHandler currentWorkflowRuntime.WorkflowAborted, AddressOf OnWorkflowAborted

            ' Start Workflow Runtime
            currentWorkflowRuntime.StartRuntime()

            '
            ' start PO approval workflow with purchase order amount less than $1000
            '
            Console.WriteLine("Workflow 1:")
            Dim poAmount As Int32 = 750
            Dim type As System.Type = GetType(DynamicUpdateWorkflow)
            Dim workflow1Parameters As New Dictionary(Of String, Object)
            workflow1Parameters.Add(amountParameter, poAmount)
            currentWorkflowRuntime.CreateWorkflow(type, workflow1Parameters).Start()
            WaitHandle.WaitOne()

            '
            ' start PO approval workflow with purchase order amount greater than $1000
            '
            Console.WriteLine("Workflow 2:")
            poAmount = 1200
            Dim workflow2Parameters As New Dictionary(Of String, Object)
            workflow2Parameters.Add(amountParameter, poAmount)
            currentWorkflowRuntime.CreateWorkflow(type, workflow2Parameters).Start()
            WaitHandle.WaitOne()

            ' Wait for dynamically created workflow to finish
            WaitHandle.WaitOne()

            ' After workflows have completed, stop runtime and report to command line
            currentWorkflowRuntime.StopRuntime()
            Console.WriteLine("Workflow runtime stopped, program exiting...")
        End Using
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        Console.WriteLine("  Workflow instance " + e.WorkflowInstance.InstanceId.ToString() + " completed")
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowIdled(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("  Workflow is in idle state")
    End Sub

    Shared Sub onExceptionNotHandled(ByVal sender As Object, ByVal e As ServicesExceptionNotHandledEventArgs)
        Console.WriteLine("Unhandled Workflow Exception ")
        Console.WriteLine("  Type: " + e.GetType().ToString())
        Console.WriteLine("  Message: " + e.Exception.Message)
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowAborted(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("Ending workflow...")
        WaitHandle.Set()
    End Sub
End Class


