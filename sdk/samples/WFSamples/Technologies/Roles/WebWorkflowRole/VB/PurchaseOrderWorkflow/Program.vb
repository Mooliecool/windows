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
    Shared poImpl As StartPurchaseOrder
    Shared workflowInstanceId As Guid
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        CreateRoles()

        ' Start the engine.
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            currentWorkflowRuntime.StartRuntime()

            AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

            ' Load the workflow type.
            Dim type As System.Type = GetType(PurchaseOrderWorkflow)
            workflowInstanceId = Guid.NewGuid()

            Dim dataService As ExternalDataExchangeService = New ExternalDataExchangeService()
            currentWorkflowRuntime.AddService(dataService)

            poImpl = New StartPurchaseOrder()
            dataService.AddService(poImpl)

            Dim instance As WorkflowInstance
            instance = currentWorkflowRuntime.CreateWorkflow(type)
            workflowInstanceId = instance.InstanceId
            instance.Start()

            SendPORequestMessage()

            WaitHandle.WaitOne()

            currentWorkflowRuntime.StopRuntime()
        End Using
    End Sub

    Shared Sub SendPORequestMessage()
        Try
            ' Initialize the Purchase Order details
            Dim itemID As Integer = 191
            Dim itemCost As Single = 200.0F
            Dim itemName As String = "Graphics Card"

            ' Send the event to initiate the PO from the workflow participant
            Dim genIdentity As GenericIdentity = New GenericIdentity("Amanda")
            poImpl.InvokePORequest(workflowInstanceId, itemID, itemCost, itemName, genIdentity)
        Catch e As Exception
            Console.WriteLine("Exception message: {0}", e.ToString())
        End Try
    End Sub

    Shared Sub CreateRoles()
        If Not System.Web.Security.Roles.RoleExists("Clerk") Then
            System.Web.Security.Roles.CreateRole("Clerk")
            Dim users() As String = {"Amanda", "Jones", "Simpson", "Peter"}
            Dim ClerkRole() As String = {"Clerk"}
            System.Web.Security.Roles.AddUsersToRoles(users, ClerkRole)
        End If

        If Not System.Web.Security.Roles.RoleExists("ProductManager") Then
            System.Web.Security.Roles.CreateRole("ProductManager")
            Dim users1() As String = {"Betty", "Chris", "Anil", "Derek"}
            Dim ProductManagerRole() As String = {"ProductManager"}
            System.Web.Security.Roles.AddUsersToRoles(users1, ProductManagerRole)
        End If
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

End Class

