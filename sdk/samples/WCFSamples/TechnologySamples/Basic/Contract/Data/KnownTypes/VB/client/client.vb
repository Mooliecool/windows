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
            value1.realField = 1
            value1.imaginaryField = 2

            Dim value2 As New ComplexNumber()
            value2.realField = 3
            value2.imaginaryField = 4

            Dim result As ComplexNumber = client.Add(value1, value2)
            Console.WriteLine("Add({0} + {1}i, {2} + {3}i) = {4} + {5}i", value1.realField, value1.imaginaryField, value2.realField, value2.imaginaryField, result.realField, _
             result.imaginaryField)

            If TypeOf result Is ComplexNumberWithMagnitude Then

                Console.WriteLine("Magnitude: {0}", (DirectCast(result, ComplexNumberWithMagnitude)).Magnitude)

            Else

                Console.WriteLine("No magnitude was sent from the service")

            End If

            ' Call the Subtract service operation.
            value1 = New ComplexNumber()
            value1.realField = 1
            value1.imaginaryField = 2

            value2 = New ComplexNumber()
            value2.realField = 3
            value2.imaginaryField = 4

            result = client.Subtract(value1, value2)
            Console.WriteLine("Subtract({0} + {1}i, {2} + {3}i) = {4} + {5}i", value1.realField, value1.imaginaryField, value2.realField, value2.imaginaryField, result.realField, _
             result.imaginaryField)

            If TypeOf result Is ComplexNumberWithMagnitude Then

                Console.WriteLine("Magnitude: {0}", (DirectCast(result, ComplexNumberWithMagnitude)).Magnitude)

            Else

                Console.WriteLine("No magnitude was sent from the service")

            End If

            ' Call the Multiply service operation.
            value1 = New ComplexNumber()
            value1.realField = 2
            value1.imaginaryField = 3

            value2 = New ComplexNumber()
            value2.realField = 4
            value2.imaginaryField = 7

            result = client.Multiply(value1, value2)
            Console.WriteLine("Multiply({0} + {1}i, {2} + {3}i) = {4} + {5}i", value1.realField, value1.imaginaryField, value2.realField, value2.imaginaryField, result.realField, _
             result.imaginaryField)

            If TypeOf result Is ComplexNumberWithMagnitude Then

                Console.WriteLine("Magnitude: {0}", (DirectCast(result, ComplexNumberWithMagnitude)).Magnitude)

            Else

                Console.WriteLine("No magnitude was sent from the service")

            End If

            ' Call the Divide service operation.
            value1 = New ComplexNumber()
            value1.realField = 3
            value1.imaginaryField = 7

            value2 = New ComplexNumber()
            value2.realField = 5
            value2.imaginaryField = -2

            result = client.Divide(value1, value2)
            Console.WriteLine("Divide({0} + {1}i, {2} + {3}i) = {4} + {5}i", value1.realField, value1.imaginaryField, value2.realField, value2.imaginaryField, result.realField, _
             result.imaginaryField)

            If TypeOf result Is ComplexNumberWithMagnitude Then

                Console.WriteLine("Magnitude: {0}", (DirectCast(result, ComplexNumberWithMagnitude)).Magnitude)

            Else

                Console.WriteLine("No magnitude was sent from the service")

            End If

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace