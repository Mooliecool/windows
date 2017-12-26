'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'  This source code is intended only as a supplement to Microsoft
'  Development Tools and/or on-line documentation.  See these other
'  materials for detailed information regarding Microsoft code samples.
' 
'  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'  PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Threading
Imports System.Workflow.Activities
Imports System.Workflow.Runtime

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)
    Private Shared service As EventService
    Private Shared instanceId As Guid

    Shared Sub Main()
        Using workflowRuntime As New WorkflowRuntime()
            Dim dataExchange As New ExternalDataExchangeService()
            workflowRuntime.AddService(dataExchange)
            service = New EventService()
            dataExchange.AddService(service)

            AddHandler workflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler workflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
            AddHandler workflowRuntime.WorkflowIdled, AddressOf OnWorkflowIdled

            Dim workflowInstance As WorkflowInstance
            workflowInstance = workflowRuntime.CreateWorkflow(GetType(SimpleStateMachine))
            instanceId = workflowInstance.InstanceId
            Console.WriteLine("Starting workflow.")
            workflowInstance.Start()

            WaitHandle.WaitOne()

            workflowRuntime.StopRuntime()
        End Using
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        Console.WriteLine("Workflow completed.")
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowIdled(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("Workflow idled.")
        Console.WriteLine("Sending event to workflow.")
        service.RaiseSetStateEvent(instanceId)
    End Sub

End Class


