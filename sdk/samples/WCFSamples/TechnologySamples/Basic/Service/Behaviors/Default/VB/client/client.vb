' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    ' The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    ' Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Create a proxy with default client endpoint configuration.
            Dim client As New CalculatorClient()

            ' Add
            Dim value1 As Double = 100
            Dim value2 As Double = 15.99
            Dim result As Double = client.Add(value1, value2)
            Console.WriteLine("Add({0},{1}) = {2}", value1, value2, result)

            ' Subtract
            value1 = 145
            value2 = 76.54
            result = client.Subtract(value1, value2)
            Console.WriteLine("Subtract({0},{1}) = {2}", value1, value2, result)

            ' Multiply
            value1 = 9
            value2 = 81.25
            result = client.Multiply(value1, value2)
            Console.WriteLine("Multiply({0},{1}) = {2}", value1, value2, result)

            ' Divide
            value1 = 22
            value2 = 7
            result = client.Divide(value1, value2)
            Console.WriteLine("Divide({0},{1}) = {2}", value1, value2, result)

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
