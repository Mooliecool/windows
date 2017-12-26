' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Create a client with given client endpoint configuration
            Dim client As New CalculatorClient()

            ' Perform addition using a typed message.

            Dim request As New MyMessage()
            request.N1 = 100
            request.N2 = 15.99
            request.Operation = "+"
            Dim response As MyMessage = CType(client, ICalculator).Calculate(request)
            Console.WriteLine("Add({0},{1}) = {2}", request.N1, request.N2, response.Result)

            ' Perform subtraction using a typed message.

            request = New MyMessage()
            request.N1 = 145
            request.N2 = 76.54
            request.Operation = "-"
            response = CType(client, ICalculator).Calculate(request)
            Console.WriteLine("Subtract({0},{1}) = {2}", request.N1, request.N2, response.Result)

            ' Perform multiplication using a typed message.

            request = New MyMessage()
            request.N1 = 9
            request.N2 = 81.25
            request.Operation = "*"
            response = CType(client, ICalculator).Calculate(request)
            Console.WriteLine("Multiply({0},{1}) = {2}", request.N1, request.N2, response.Result)

            ' Perform multiplication using a typed message.

            request = New MyMessage()
            request.N1 = 22
            request.N2 = 7
            request.Operation = "/"
            response = CType(client, ICalculator).Calculate(request)
            Console.WriteLine("Divide({0},{1}) = {2}", request.N1, request.N2, response.Result)

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()
        End Sub
    End Class
End Namespace
