' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Channels
Imports System.Configuration
Imports System.Messaging
Imports System.ServiceModel
Imports System.Transactions
Imports System.Net


Namespace Microsoft.ServiceModel.Samples

    ' Define the service contract for order status replies
    <ServiceContract([Namespace]:="http://Microsoft.ServiceModel.Samples")> _
    Public Interface IOrderStatus

        <OperationContract(IsOneWay:=True)> _
        Sub OrderStatus(ByVal poNumber As String, ByVal status As String)

    End Interface

    ' Service that handles order status
    <ServiceBehavior()> _
    Public Class OrderStatusService
        Implements IOrderStatus

        <OperationBehavior(TransactionAutoComplete:=True, TransactionScopeRequired:=True)> _
        Public Sub OrderStatus(ByVal poNumber As String, ByVal status As String) _
            Implements IOrderStatus.OrderStatus

            Console.WriteLine("Status of order {0}:{1} ", poNumber, status)

        End Sub

    End Class

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Get MSMQ queue name from app settings in configuration
            Dim queueName As String = ConfigurationManager.AppSettings("queueName")

            ' Create the transacted MSMQ queue if necessary.
            ' This is the queue the order status would be reported to
            If Not MessageQueue.Exists(queueName) Then
                MessageQueue.Create(queueName, True)
            End If

            ' Create a ServiceHost for the OrderStatus service type.
            Using serviceHost As New ServiceHost(GetType(OrderStatusService))

                ' Open the ServiceHostBase to create listeners and start listening for order status messages.
                serviceHost.Open()
                ' Create the purchase order
                Dim po As New PurchaseOrder()
                po.CustomerId = "somecustomer.com"
                po.PONumber = Guid.NewGuid().ToString()

                Dim lineItem1 As New PurchaseOrderLineItem()
                lineItem1.ProductId = "Blue Widget"
                lineItem1.Quantity = 54
                lineItem1.UnitCost = 29.99F

                Dim lineItem2 As New PurchaseOrderLineItem()
                lineItem2.ProductId = "Red Widget"
                lineItem2.Quantity = 890
                lineItem2.UnitCost = 45.89F

                po.orderLineItems = New PurchaseOrderLineItem(1) {}
                po.orderLineItems(0) = lineItem1
                po.orderLineItems(1) = lineItem2

                ' Create a client with given client endpoint configuration
                Dim client As New OrderProcessorClient("OrderProcessorEndpoint")

                'Create a transaction scope.
                Using scope As New TransactionScope(TransactionScopeOption.Required)

                    Dim hostName As String = Dns.GetHostName()

                    ' Make a queued call to submit the purchase order
                    client.SubmitPurchaseOrder(po, "net.msmq://" & hostName & "/private/ServiceModelSamplesTwo-way/OrderStatus")

                    ' Complete the transaction.
                    scope.Complete()

                End Using

                'Close down the client
                client.Close()

                Console.WriteLine()
                Console.WriteLine("Press <ENTER> to terminate client.")
                Console.ReadLine()

            End Using

        End Sub

    End Class

End Namespace