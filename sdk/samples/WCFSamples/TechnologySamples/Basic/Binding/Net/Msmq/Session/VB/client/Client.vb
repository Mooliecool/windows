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

            'Create a transaction scope.
            Using scope As New TransactionScope(TransactionScopeOption.Required)

                ' Create a client with given client endpoint configuration
                Dim client As New OrderTakerClient("OrderTakerEndpoint")

                ' Open a purchase order
                client.OpenPurchaseOrder("somecustomer.com")
                Console.WriteLine("Purchase Order created")

                ' Add product line items
                Console.WriteLine("Adding 10 quantities of blue widget")
                client.AddProductLineItem("Blue Widget", 10)
                Console.WriteLine("Adding 23 quantities of red widget")
                client.AddProductLineItem("Red Widget", 23)

                ' Close the purchase order
                Console.WriteLine("Closing the purchase order")
                client.EndPurchaseOrder()

                'Closing the client gracefully closes the connection and cleans up resources
                client.Close()

                ' Complete the transaction.
                scope.Complete()

            End Using

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace