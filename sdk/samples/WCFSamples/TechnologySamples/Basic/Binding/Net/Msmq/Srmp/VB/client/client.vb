' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Configuration
Imports System.Messaging
Imports System.ServiceModel
Imports System.Transactions

Namespace Microsoft.ServiceModel.Samples

    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Create a client with given client endpoint configuration
            Dim client As New OrderProcessorClient("OrderProcessorEndpoint")

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
                client.SubmitPurchaseOrder(po)
                ' Complete the transaction.
                scope.Complete()

            End Using

            client.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace