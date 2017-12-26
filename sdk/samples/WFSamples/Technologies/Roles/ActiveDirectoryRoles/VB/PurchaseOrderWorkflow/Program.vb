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
Imports System.Security.Principal

Class Program
    Private Shared poImpl As StartPurchaseOrder
    Private Shared workflowInstanceId As Guid
    Private Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        ' Start up the engine.
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()

            poImpl = New StartPurchaseOrder()
            Dim dataService As ExternalDataExchangeService = New ExternalDataExchangeService()
            currentWorkflowRuntime.AddService(dataService)
            dataService.AddService(poImpl)

            AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

            ' Load the workflow type.
            Dim type As System.Type = GetType(PurchaseOrderWorkflow)
            Dim instance As WorkflowInstance = currentWorkflowRuntime.CreateWorkflow(type)
            workflowInstanceId = instance.InstanceId

            ' Start the workflow runtime engine
            currentWorkflowRuntime.StartRuntime()

            instance.Start()

            SendPORequestMessage()

            WaitHandle.WaitOne()

            currentWorkflowRuntime.StopRuntime()
        End Using
    End Sub

    Private Shared Sub SendPORequestMessage()
        Try
            ' Initialize the Purchase Order details
            Dim itemID As Integer = 191
            Dim itemCost As Single = 200.0F
            Dim itemName As String = "Graphics Card"

            ' Send the event to initiate the PO from the workflow participant
            ' The current windows identity is used for simulating the Employee
            ' who can initiate a PO
            Dim authUserIdentity As WindowsIdentity = WindowsIdentity.GetCurrent()
            poImpl.InvokePORequest(workflowInstanceId, itemID, itemCost, itemName, authUserIdentity)
        Catch ex As Exception
            Console.WriteLine("Exception message: {0}", ex.ToString())
        End Try
    End Sub

    Private Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        Console.WriteLine("Workflow has completed.")
        WaitHandle.Set()
    End Sub

    Private Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine("Workflow has been terminated. Message: {0}", e.Exception.Message)
        WaitHandle.Set()
    End Sub
End Class


