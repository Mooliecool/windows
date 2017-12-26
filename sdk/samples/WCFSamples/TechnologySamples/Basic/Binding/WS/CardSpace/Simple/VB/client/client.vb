' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.IdentityModel.Selectors
Imports System.ServiceModel.Security

Namespace Microsoft.ServiceModel.Samples.CardSpace

    Class Client

        Public Shared Sub Main()

            Try

                ' Create a client with given client endpoint configuration
                Dim client As New SecureCalculatorClient()
                Console.WriteLine("Press <ENTER> to invoke the service...")
                Console.ReadLine()

                ' Display Identity.
                Console.WriteLine("Identity - (Private Personal ID) = {0}", client.GetIdentity())

                ' Call the Add service operation.
                Dim value1 As Double = 100
                Dim value2 As Double = 15.99
                Dim result As Double = client.Add(value1, value2)
                Console.WriteLine("Add({0},{1}) = {2}", value1, value2, result)

                ' Call the Subtract service operation.
                value1 = 145
                value2 = 76.54
                result = client.Subtract(value1, value2)
                Console.WriteLine("Subtract({0},{1}) = {2}", value1, value2, result)

                ' Call the Multiply service operation.
                value1 = 9
                value2 = 81.25
                result = client.Multiply(value1, value2)
                Console.WriteLine("Multiply({0},{1}) = {2}", value1, value2, result)

                ' Call the Divide service operation.
                value1 = 22
                value2 = 7
                result = client.Divide(value1, value2)
                Console.WriteLine("Divide({0},{1}) = {2}", value1, value2, result)

                'Closing the client gracefully closes the connection and cleans up resources
                client.Close()

            Catch ex As UserCancellationException

                Console.WriteLine("1:" + ex.Message)

            Catch ex As UntrustedRecipientException

                Console.WriteLine("2:" + ex.Message)

            Catch ex As ServiceNotStartedException

                Console.WriteLine("3:" + ex.Message)

            Catch ex As TimeoutException

                Console.WriteLine("4:" + ex.Message)

            Catch ex As SecurityNegotiationException

                Console.WriteLine("5:" + ex.Message)

            Catch ex As IdentityValidationException

                Console.WriteLine("7:" + ex.Message)
                Console.WriteLine(ex.StackTrace)

            Catch ex As Exception

                Console.WriteLine("6:" + ex.[GetType]().Name + ex.Message)

            End Try

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
