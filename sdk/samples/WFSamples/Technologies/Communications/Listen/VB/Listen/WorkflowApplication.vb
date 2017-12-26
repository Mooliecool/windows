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
Imports System.Workflow.Activities
Imports System.Workflow.Runtime

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)
    Shared orderService As New OrderServiceImpl()

    Shared Sub Main()
        ' Start the workflow runtime engine.
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            Dim dataService As ExternalDataExchangeService = New ExternalDataExchangeService()
            currentWorkflowRuntime.AddService(dataService)
            dataService.AddService(orderService)

            currentWorkflowRuntime.StartRuntime()

            ' Listen for the workflow events
            AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
            AddHandler currentWorkflowRuntime.WorkflowIdled, AddressOf OnWorkflowIdled

            ' Start the workflow and wait for it to complete
            Dim type As System.Type = GetType(PurchaseOrderWorkflow)
            currentWorkflowRuntime.CreateWorkflow(type).Start()

            WaitHandle.WaitOne()

            ' Stop the workflow runtime engine.
            currentWorkflowRuntime.StopRuntime()
        End Using
    End Sub

    ' This method will be called when a workflow instance is completed
    ' waitHandle is set so the main thread can continue
    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        WaitHandle.Set()
    End Sub

    ' This method is called when the workflow terminates and does not complete
    ' This should not occur in this sample; however, it is good practice to include a
    ' handler for this event so the host application can manage workflows that are
    ' unexpectedly terminated (e.g. unhandled workflow exception).
    ' waitHandle is set so the main thread can continue
    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    ' This method will be called when a workflow instance is idled
    Shared Sub OnWorkflowIdled(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        orderService.instanceId = e.WorkflowInstance

        'Randomly approve, reject or timeout purchase orders
        Dim randGen As Random = New Random()

        Dim pick As Integer = randGen.Next(1, 100) Mod 3
        Select Case pick
            Case 0
                orderService.ApproveOrder()
            Case 1
                orderService.RejectOrder()
            Case 2
                'timeout
        End Select
    End Sub
End Class


