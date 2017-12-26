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

            Try

                ' Call the Add service operation.
                Dim value1 As Integer = 15
                Dim value2 As Integer = 3
                Dim result As Integer = client.Add(value1, value2)
                Console.WriteLine("Add({0},{1}) = {2}", value1, value2, result)

                ' Call the Subtract service operation.
                value1 = 145
                value2 = 76
                result = client.Subtract(value1, value2)
                Console.WriteLine("Subtract({0},{1}) = {2}", value1, value2, result)

                ' Call the Multiply service operation.
                value1 = 9
                value2 = 81
                result = client.Multiply(value1, value2)
                Console.WriteLine("Multiply({0},{1}) = {2}", value1, value2, result)

                ' Call the Divide service operation - trigger a divide by zero error.
                value1 = 22
                value2 = 0
                result = client.Divide(value1, value2)
                Console.WriteLine("Divide({0},{1}) = {2}", value1, value2, result)

                'Closing the client gracefully closes the connection and cleans up resources
                client.Close()

            Catch e As FaultException(Of MathFault)

                Console.WriteLine("FaultException<MathFault>: Math fault while doing " + e.Detail.Operation + ". Problem: " + e.Detail.ProblemType)
                client.Abort()

            Catch e As FaultException

                Console.WriteLine("Unknown FaultException: " + e.[GetType]().Name + " - " + e.Message)
                client.Abort()

            Catch e As Exception

                Console.WriteLine("EXCEPTION: " + e.[GetType]().Name + " - " + e.Message)
                client.Abort()

            End Try

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace