' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Description
Imports System.Configuration
Imports System.Messaging
Imports System.ServiceModel
Imports System.ServiceModel.Channels
Imports System.Transactions
Imports System.Runtime.Serialization
Imports System.Collections.Generic
Imports System.Text
Imports Microsoft.VisualBasic

Namespace Microsoft.ServiceModel.Samples

    ' Define the Purchase Order Line Item
    <DataContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Class PurchaseOrderLineItem

        <DataMember()> _
        Public ProductId As String

        <DataMember()> _
        Public UnitCost As Single

        <DataMember()> _
        Public Quantity As Integer

        Public Overloads Overrides Function ToString() As String

            Dim displayString As String = "Order LineItem: " & Quantity & " of " & ProductId & " @unit price: $" & UnitCost & vbNewLine
            Return displayString

        End Function

        Public ReadOnly Property TotalCost() As Single

            Get

                Return UnitCost * Quantity

            End Get

        End Property

    End Class

    ' Define Purchase Order
    <DataContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Class PurchaseOrder

        Shared OrderStates As String() = {"Pending", "Processed", "Shipped"}
        Shared statusIndexer As New Random(137)

        <DataMember()> _
        Public PONumber As String

        <DataMember()> _
        Public CustomerId As String

        <DataMember()> _
        Public orderLineItems As PurchaseOrderLineItem()

        Public ReadOnly Property TotalCost() As Single

            Get

                Dim tc As Single = 0
                For Each lineItem As PurchaseOrderLineItem In orderLineItems
                    tc += lineItem.TotalCost
                Next
                Return tc

            End Get

        End Property

        Public ReadOnly Property Status() As String

            Get

                Return OrderStates(statusIndexer.[Next](3))

            End Get

        End Property

        Public Overloads Overrides Function ToString() As String

            Dim strbuf As New StringBuilder("Purchase Order: " & PONumber & vbNewLine)
            strbuf.Append(vbTab & "Customer: " & CustomerId & vbNewLine)
            strbuf.Append(vbTab & "OrderDetails" & vbNewLine)

            For Each lineItem As PurchaseOrderLineItem In orderLineItems
                strbuf.Append(vbTab & vbTab & lineItem.ToString())
            Next

            strbuf.Append(vbTab & "Total cost of this order: $" & TotalCost & vbNewLine)
            strbuf.Append(vbTab & "Order status: " & Status.ToString() & vbNewLine)
            Return strbuf.ToString()

        End Function

    End Class

    ' Order Processing Logic
    ' Can replace with transaction-aware resource such as SQL, transacted hashtable to hold orders
    ' This example uses a non-transactional resource
    Public Class Orders

        Shared purchaseOrders As New Dictionary(Of String, PurchaseOrder)()

        Public Shared Sub Add(ByVal po As PurchaseOrder)

            purchaseOrders.Add(po.PONumber, po)

        End Sub

        Public Shared Function GetOrderStatus(ByVal poNumber As String) As String

            Dim po As PurchaseOrder = Nothing
            If purchaseOrders.TryGetValue(poNumber, po) Then
                Return po.Status
            Else
                Return Nothing
            End If

        End Function

        Public Shared Sub DeleteOrder(ByVal poNumber As String)

            If purchaseOrders(poNumber) IsNot Nothing Then
                purchaseOrders.Remove(poNumber)
            End If

        End Sub

    End Class

    ' Define a service contract. 
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IOrderProcessor

        <OperationContract(IsOneWay:=True)> _
        Sub SubmitPurchaseOrder(ByVal po As PurchaseOrder)

    End Interface

    ' Service class which implements the service contract.
    ' Added code to write output to the console window
    <ServiceBehavior(AddressFilterMode:=AddressFilterMode.Any)> _
    Public Class OrderProcessorService
        Implements IOrderProcessor

        <OperationBehavior(TransactionScopeRequired:=True, TransactionAutoComplete:=True)> _
        Public Sub SubmitPurchaseOrder(ByVal po As PurchaseOrder) Implements IOrderProcessor.SubmitPurchaseOrder

            Orders.Add(po)
            Console.WriteLine("Processing {0} ", po)

        End Sub

        Public Shared Sub OnServiceFaulted(ByVal sender As Object, ByVal e As EventArgs)

            Console.WriteLine("Service Faulted...exiting app")
            Environment.[Exit](1)

        End Sub

        ' Host the service within this EXE console application.
        Public Shared Sub Main()

            ' Create a ServiceHost for the OrderProcessorService type.
            Dim serviceHost As New ServiceHost(GetType(OrderProcessorService))

            ' Hook on to the service host faulted events
            AddHandler serviceHost.Faulted, AddressOf OnServiceFaulted

            serviceHost.Open()

            ' The service can now be accessed.
            Console.WriteLine("The poison message service is ready.")
            Console.WriteLine("Press <ENTER> to terminate service.")
            Console.WriteLine()
            Console.ReadLine()

            ' Close the ServiceHostBase to shutdown the service.
            If serviceHost.State <> CommunicationState.Faulted Then

                serviceHost.Close()

            End If

        End Sub

    End Class

End Namespace
