' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Transactions
Imports System.ServiceModel.MsmqIntegration

Namespace Microsoft.ServiceModel.Samples

    Class Program

        Public Shared Sub Main(ByVal args As String())

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

            Dim client As New OrderProcessorClient("OrderResponseEndpoint")
            Dim ordermsg As New MsmqMessage(Of PurchaseOrder)(po)
            Using scope As New TransactionScope(TransactionScopeOption.Required)

                client.SubmitPurchaseOrder(ordermsg)
                scope.Complete()

            End Using
            Console.WriteLine("Order has been submitted:{0}", po)

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
