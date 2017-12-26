' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Text
Imports Microsoft.VisualBasic

Namespace Microsoft.ServiceModel.Samples

    ' Define the Purchase Order Line Item
    <Serializable()> _
    Public Class PurchaseOrderLineItem

        Public productId As String
        Public unitCost As Single
        Public quantity As Integer

        Public Overloads Overrides Function ToString() As String

            Dim displayString As String = "Order LineItem: " & quantity & " of " & productId & " @unit price: $" & unitCost & vbNewLine
            Return displayString

        End Function

        Public ReadOnly Property TotalCost() As Single

            Get

                Return unitCost * quantity

            End Get

        End Property

    End Class

    Public Enum OrderStates

        Pending
        Processed
        Shipped

    End Enum

    ' Define Purchase Order
    <Serializable()> _
    Public Class PurchaseOrder

        Public Shared OrderStates As String() = {"Pending", "Processed", "Shipped"}
        Public poNumber As String
        Public customerId As String
        Public orderLineItems() As PurchaseOrderLineItem
        Public orderStatus As OrderStates

        Public ReadOnly Property TotalCost() As Single

            Get

                Dim tCost As Single = 0
                For Each lineItem As PurchaseOrderLineItem In orderLineItems
                    tCost += lineItem.TotalCost
                Next
                Return tCost

            End Get

        End Property

        Public Property Status() As OrderStates

            Get

                Return orderStatus

            End Get
            Set(ByVal value As OrderStates)

                orderStatus = value

            End Set

        End Property

        Public Overloads Overrides Function ToString() As String
            Dim strbuf As New StringBuilder("Purchase Order: " & poNumber & vbNewLine)
            strbuf.Append(vbTab & "Customer: " & customerId & vbNewLine)
            strbuf.Append(vbTab & "OrderDetails" & vbNewLine)

            For Each lineItem As PurchaseOrderLineItem In orderLineItems

                strbuf.Append(vbTab & vbTab & lineItem.ToString())

            Next

            strbuf.Append(vbTab & "Total cost of this order: $" & TotalCost & vbNewLine)
            strbuf.Append(vbTab & "Order status: " & Status.ToString() & vbNewLine)
            Return strbuf.ToString()

        End Function

    End Class

End Namespace
