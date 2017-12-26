' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Channels
Imports System.Configuration
Imports System.Messaging
Imports System.ServiceModel
Imports System.Transactions
Imports Microsoft.ServiceModel.Samples

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
        Public Sub OrderStatus(ByVal poNumber As String, ByVal status As String) Implements IOrderStatus.OrderStatus

            Console.WriteLine("Status of order {0}:{1} ", poNumber, status)

        End Sub

    End Class

    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Get MSMQ queue name from app settings in configuration
            Dim targetQueueName As String = ConfigurationManager.AppSettings("targetQueueName")

            ' Create the transacted MSMQ queue if necessary.
            ' This is the queue the order status would be reported to
            If Not MessageQueue.Exists(targetQueueName) Then
                MessageQueue.Create(targetQueueName, True)
            End If

            ' Get MSMQ queue name from app settings in configuration
            Dim responseQueueName As String = ConfigurationManager.AppSettings("responseQueueName")

            ' Create the transacted MSMQ queue if necessary.
            ' This is the queue the order status would be reported to
            If Not MessageQueue.Exists(responseQueueName) Then
                MessageQueue.Create(responseQueueName, True)
            End If

            ' Create a ServiceHost for the OrderStatus service type.
            Dim serviceHost As New ServiceHost(GetType(OrderStatusService))

            ' Open the ServiceHostBase to create listeners and start listening for order status messages.
            serviceHost.Open()

            ' Create a proxy with given client endpoint configuration
            Dim client As New OrderProcessorClient()

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

            'Create a transaction scope.
            Using scope As New TransactionScope(TransactionScopeOption.Required)

                ' Make a queued call to submit the purchase order
                client.SubmitPurchaseOrder(po, "net.msmq://localhost/private/ServiceModelSamples/OrderStatus")
                ' Complete the transaction.
                scope.Complete()

            End Using

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

            Try
                serviceHost.Close()
            Catch ex as CommunicationObjectFaultedException
                Console.WriteLine("Warning: status service is in faulted state")
            End Try            

        End Sub

    End Class

End Namespace
