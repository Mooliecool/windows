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

Imports Microsoft.VisualBasic
Imports System
Imports System.Workflow.Runtime
Imports System.Threading

' Local service that implements the contract on the host side
' i.e. it implements the methods and calls the events, which are
' implemented by the workflow
Class OrderServiceImpl
    Implements IOrderService

    Dim orderId As String
    Public instanceId As WorkflowInstance

    ' Called by the workflow to pass an order id
    Public Sub CreateOrder(ByVal id As String) Implements IOrderService.CreateOrder
        Console.WriteLine(vbLf + "Purchase Order Created in System")
        orderId = id
    End Sub

    ' Called by the host to approve an order
    Public Sub ApproveOrder()
        RaiseEvent OrderApproved(Nothing, New OrderEventArgs(instanceId.InstanceId, orderId))
    End Sub

    ' Called by the host to reject an order
    Public Sub RejectOrder()
        RaiseEvent OrderRejected(Nothing, New OrderEventArgs(instanceId.InstanceId, orderId))
    End Sub

    ' Events that handled within a workflow by HandleExternalEventActivity activities
    Public Event OrderApproved(ByVal sender As Object, ByVal e As OrderEventArgs) Implements IOrderService.OrderApproved
    Public Event OrderRejected(ByVal sender As Object, ByVal e As OrderEventArgs) Implements IOrderService.OrderRejected
End Class
