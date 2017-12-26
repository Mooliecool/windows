' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Messaging
Imports System.Configuration
Imports System.Transactions

Namespace Microsoft.ServiceModel.Samples

    ' Define the Purchase Order Line Item
    Class Program

        Shared orderMessageID As String

        Private Shared Sub DisplayOrderStatus()

            Dim orderResponseQueue As New MessageQueue(ConfigurationManager.AppSettings("orderResponseQueueName"))

            'Create a transaction scope.
            Dim responseReceived As Boolean = False
            orderResponseQueue.MessageReadPropertyFilter.CorrelationId = True
            While Not responseReceived

                Dim responseMsg As Message
                Using scope2 As New TransactionScope(TransactionScopeOption.Required)

                    'Receive the Order Response message
                    responseMsg = orderResponseQueue.Receive(MessageQueueTransactionType.Automatic)
                    scope2.Complete()

                End Using
                responseMsg.Formatter = New System.Messaging.XmlMessageFormatter(New Type() {GetType(PurchaseOrder)})
                Dim responsepo As PurchaseOrder = DirectCast(responseMsg.Body, PurchaseOrder)
                'Check if the response is for the order placed
                If orderMessageID = responseMsg.CorrelationId Then

                    responseReceived = True
                    Console.WriteLine("Status of current Order: OrderID-{0},Order Status-{1}", responsepo.poNumber, responsepo.Status)

                Else

                    Console.WriteLine("Status of previous Order: OrderID-{0},Order Status-{1}", responsepo.poNumber, responsepo.Status)

                End If

            End While

        End Sub

        Private Shared Sub PlaceOrder()

            'Connect to the queue
            Dim orderQueue As New MessageQueue("FormatName:Direct=OS:" + ConfigurationManager.AppSettings("orderQueueName"))

            ' Create the purchase order
            Dim po As New PurchaseOrder()
            po.customerId = "somecustomer.com"
            po.poNumber = Guid.NewGuid().ToString()

            Dim lineItem1 As New PurchaseOrderLineItem()
            lineItem1.productId = "Blue Widget"
            lineItem1.quantity = 54
            lineItem1.unitCost = 29.99F

            Dim lineItem2 As New PurchaseOrderLineItem()
            lineItem2.productId = "Red Widget"
            lineItem2.quantity = 890
            lineItem2.unitCost = 45.89F

            po.orderLineItems = New PurchaseOrderLineItem(1) {lineItem1, lineItem2}

            Dim msg As New Message()
            msg.UseDeadLetterQueue = True
            msg.Body = po

            'Create a transaction scope.
            Using scope As New TransactionScope(TransactionScopeOption.Required)

                ' submit the purchase order
                orderQueue.Send(msg, MessageQueueTransactionType.Automatic)
                ' Complete the transaction.
                scope.Complete()

            End Using
            'Save the string for orderResponse correlation
            orderMessageID = msg.Id
            Console.WriteLine("Placed the order, waiting for response...")

        End Sub

        Public Shared Sub Main(ByVal args As String())

            ' Create the transacted response queue if necessary.
            If Not MessageQueue.Exists(ConfigurationManager.AppSettings("orderResponseQueueName")) Then
                MessageQueue.Create(ConfigurationManager.AppSettings("orderResponseQueueName"), True)
            End If

            'Place an order
            PlaceOrder()
            DisplayOrderStatus()
            Console.WriteLine("Press enter to terminate the client")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
