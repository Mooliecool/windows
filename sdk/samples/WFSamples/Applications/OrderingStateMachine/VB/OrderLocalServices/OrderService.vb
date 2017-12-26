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

<Serializable()> _
Public Class OrderService
    Implements IOrderService
    Public Sub RaiseOrderCreatedEvent(ByVal orderId As String, ByVal instanceId As Guid)
        Dim e As New OrderEventArgs(instanceId, orderId)
        RaiseEvent OrderCreated(Me, e)
    End Sub

    Public Sub RaiseOrderShippedEvent(ByVal orderId As String, ByVal instanceId As Guid)
        Dim e As New OrderEventArgs(instanceId, orderId)
        RaiseEvent OrderShipped(Me, e)
    End Sub

    Public Sub RaiseOrderUpdatedEvent(ByVal orderId As String, ByVal instanceId As Guid)
        Dim e As New OrderEventArgs(instanceId, orderId)
        RaiseEvent OrderUpdated(Me, e)
    End Sub


    Public Sub RaiseOrderProcessedEvent(ByVal orderId As String, ByVal instanceId As Guid)
        Dim e As New OrderEventArgs(instanceId, orderId)
        RaiseEvent OrderProcessed(Me, e)
    End Sub

    Public Sub RaiseOrderCanceledEvent(ByVal orderId As String, ByVal instanceId As Guid)
        Dim e As New OrderEventArgs(instanceId, orderId)
        RaiseEvent OrderCanceled(Me, e)
    End Sub

    Public Event OrderCanceled(ByVal sender As Object, ByVal e As OrderEventArgs) Implements IOrderService.OrderCanceled

    Public Event OrderCreated(ByVal sender As Object, ByVal e As OrderEventArgs) Implements IOrderService.OrderCreated

    Public Event OrderProcessed(ByVal sender As Object, ByVal e As OrderEventArgs) Implements IOrderService.OrderProcessed

    Public Event OrderShipped(ByVal sender As Object, ByVal e As OrderEventArgs) Implements IOrderService.OrderShipped

    Public Event OrderUpdated(ByVal sender As Object, ByVal e As OrderEventArgs) Implements IOrderService.OrderUpdated

End Class
