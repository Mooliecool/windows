' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Create a client
            Dim client As New OneWayCalculatorClient()

            ' Call the Add service operation.
            Dim value1 As Double = 100
            Dim value2 As Double = 15.99
            client.Add(value1, value2)
            Console.WriteLine("Add({0},{1})", value1, value2)

            ' Call the Subtract service operation.
            value1 = 145
            value2 = 76.54
            client.Subtract(value1, value2)
            Console.WriteLine("Subtract({0},{1})", value1, value2)

            ' Call the Multiply service operation.
            value1 = 9
            value2 = 81.25
            client.Multiply(value1, value2)
            Console.WriteLine("Multiply({0},{1})", value1, value2)

            ' Call the Divide service operation.
            value1 = 22
            value2 = 7
            client.Divide(value1, value2)
            Console.WriteLine("Divide({0},{1})", value1, value2)
            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

        End Sub

    End Class

End Namespace
