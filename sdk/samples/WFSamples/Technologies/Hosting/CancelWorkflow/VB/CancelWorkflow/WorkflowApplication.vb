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
Imports System.Collections.ObjectModel
Imports System.Threading
Imports System.Workflow.Activities
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Hosting

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)
    Shared expenseService As New ExpenseReportServiceImpl()


    Shared Sub Main()
        Dim connectionString As String = "Initial Catalog=SqlPersistenceService;Data Source=localhost;Integrated Security=SSPI;"
        Using workflowRuntime As New WorkflowRuntime()
            Dim dataService As New ExternalDataExchangeService()
            workflowRuntime.AddService(dataService)
            dataService.AddService(expenseService)

            workflowRuntime.AddService(New SqlWorkflowPersistenceService(connectionString))


            AddHandler workflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler workflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
            AddHandler workflowRuntime.WorkflowIdled, AddressOf OnWorkflowIdled
            AddHandler workflowRuntime.WorkflowAborted, AddressOf OnWorkflowAborted


            Dim workflowInstance As WorkflowInstance
            workflowInstance = workflowRuntime.CreateWorkflow(GetType(SampleWorkflow))
            workflowInstance.Start()

            WaitHandle.WaitOne()

            workflowRuntime.StopRuntime()
        End Using
    End Sub

    Shared Sub OnWorkflowIdled(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Dim workflow As WorkflowInstance = e.WorkflowInstance

        Console.WriteLine(vbCrLf + "...waiting for 3 seconds... " + vbCrLf)
        Thread.Sleep(3000)

        ' what activity is blocking the workflow
        Dim wqi As ReadOnlyCollection(Of WorkflowQueueInfo) = workflow.GetWorkflowQueueData()
        For Each q As WorkflowQueueInfo In wqi

            Dim eq As EventQueueName = TryCast(q.QueueName, EventQueueName)

            If eq IsNot Nothing Then
                ' get activity that is waiting for event
                Dim blockedActivity As ReadOnlyCollection(Of String) = q.SubscribedActivityNames
                Console.WriteLine("Host: Workflow is blocked on " + blockedActivity(0))

                ' this event is never going to arrive eg. employee left the company
                ' lets send an exception to this queue
                ' it will either be handled by exception handler that was modeled in workflow
                ' or the runtime will unwind running compensation handlers and exit the workflow
                Console.WriteLine("Host: This event is not going to arrive")
                Console.WriteLine("Host: Cancel workflow with unhandled exception")
                workflow.EnqueueItem(q.QueueName, New Exception("ExitWorkflowException"), Nothing, Nothing)
            End If
            next
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowAborted(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("Ending workflow...")
        WaitHandle.Set()
    End Sub


    ' class that implements the abstract IExpenseReportService
    Class ExpenseReportServiceImpl
        Implements IExpenseReportService

        Dim expenseId As String

        ' Expense report is created in the system 
        ' This method is invoked by the SubmitExpense activity
        Public Sub SubmitExpense(ByVal Id As String) Implements IExpenseReportService.SubmitExpense
            Console.WriteLine("Host: expense report sent")
            expenseId = Id
        End Sub

        ' This method corresponds to the event being raised to approve the order
        Public Sub ApprovalResult(ByVal workflow As WorkflowInstance, ByVal approval As String)
            RaiseEvent ExpenseApproval(workflow, New ExpenseReportEventArgs(workflow.InstanceId, approval))
        End Sub


        Public Event ExpenseApproval As EventHandler(Of ExpenseReportEventArgs) Implements IExpenseReportService.ExpenseApproval
    End Class
End Class


