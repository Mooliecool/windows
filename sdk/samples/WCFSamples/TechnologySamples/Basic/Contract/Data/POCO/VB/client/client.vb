' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples
    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()

            ' Create a client
            Dim client As New DataContractCalculatorClient()

            ' Call the Add service operation.
            Dim value1 As New ComplexNumber()
            value1.Real = 1
            value1.Imaginary = 2

            Dim value2 As New ComplexNumber()
            value2.Real = 3
            value2.Imaginary = 4

            Dim result As ComplexNumber = client.Add(value1, value2)
            Console.WriteLine("Add({0} + {1}i, {2} + {3}i) = {4} + {5}i", value1.Real, value1.Imaginary, value2.Real, value2.Imaginary, result.Real, _
             result.Imaginary)

            ' Call the Subtract service operation.
            value1 = New ComplexNumber()
            value1.Real = 1
            value1.Imaginary = 2

            value2 = New ComplexNumber()
            value2.Real = 3
            value2.Imaginary = 4

            result = client.Subtract(value1, value2)
            Console.WriteLine("Subtract({0} + {1}i, {2} + {3}i) = {4} + {5}i", value1.Real, value1.Imaginary, value2.Real, value2.Imaginary, result.Real, _
             result.Imaginary)

            ' Call the Multiply service operation.
            value1 = New ComplexNumber()
            value1.Real = 2
            value1.Imaginary = 3

            value2 = New ComplexNumber()
            value2.Real = 4
            value2.Imaginary = 7

            result = client.Multiply(value1, value2)
            Console.WriteLine("Multiply({0} + {1}i, {2} + {3}i) = {4} + {5}i", value1.Real, value1.Imaginary, value2.Real, value2.Imaginary, result.Real, _
             result.Imaginary)

            ' Call the Divide service operation.
            value1 = New ComplexNumber()
            value1.Real = 3
            value1.Imaginary = 7

            value2 = New ComplexNumber()
            value2.Real = 5
            value2.Imaginary = -2

            result = client.Divide(value1, value2)
            Console.WriteLine("Divide({0} + {1}i, {2} + {3}i) = {4} + {5}i", value1.Real, value1.Imaginary, value2.Real, value2.Imaginary, result.Real, _
             result.Imaginary)

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace