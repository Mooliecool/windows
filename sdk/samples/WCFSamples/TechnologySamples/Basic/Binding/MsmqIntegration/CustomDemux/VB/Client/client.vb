' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Messaging
Imports System.Configuration
Imports System.Transactions

Namespace Microsoft.ServiceModel.Samples

    Class Program

        Public Shared Sub Main(ByVal args As String())

            'Connect to the queue
            Dim orderQueue As New MessageQueue("FormatName:Direct=OS:" & ConfigurationManager.AppSettings("orderQueueName"))

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

            ' submit the purchase order
            Dim msg As New Message()
            msg.Body = po
            msg.Label = "SubmitPurchaseOrder"

            'Submit an Order.
            Using scope As New TransactionScope(TransactionScopeOption.Required)

                orderQueue.Send(msg, MessageQueueTransactionType.Automatic)
                ' Complete the transaction.
                scope.Complete()

            End Using

            Console.WriteLine("Placed the order:{0}", po)

            ' submit the purchase order
            Dim msg2 As New Message()
            msg2.Body = po.poNumber
            msg2.Label = "CancelPurchaseOrder"

            'Cancel the Order.
            Using scope2 As New TransactionScope(TransactionScopeOption.Required)

                orderQueue.Send(msg2, MessageQueueTransactionType.Automatic)
                ' Complete the transaction.
                scope2.Complete()

            End Using

            Console.WriteLine("Cancelled the Order: {0}", po.poNumber)
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
