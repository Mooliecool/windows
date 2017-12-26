' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel

Namespace Microsoft.ServiceModel.Samples

    ' The service contract is defined in generatedClient.vb, generated from the service
    ' by the svcutil tool.
    '
    ' When using such a client, applications must catch 

    ' Client implementation code.
    Class Client

        Public Shared Sub Main()

            DemonstrateCommunicationException()
            DemonstrateTimeoutException()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

        Private Shared Sub DemonstrateCommunicationException()

            ' Create a client
            Dim client As New CalculatorClient()

            Try

                ' Call the Add service operation.
                Dim value1 As Double = 100
                Dim value2 As Double = 15.99
                Dim result As Double = client.Add(value1, value2)
                Console.WriteLine("Add({0},{1}) = {2}", value1, value2, result)

                ' Simulate a network problem by aborting the connection.
                Console.WriteLine("Simulated network problem occurs...")
                client.Abort()

                ' Call the Divide service operation.  Now that the channel has been
                ' abruptly terminated, the next call will fail.
                value1 = 22
                value2 = 7
                result = client.Divide(value1, value2)
                Console.WriteLine("Divide({0},{1}) = {2}", value1, value2, result)

                ' SHOULD NOT GET HERE -- Divide should throw

                ' If we had gotten here, we would want to close the client gracefully so
                ' that the channel closes gracefully and cleans up resources.
                client.Close()

                Console.WriteLine("Service successfully returned all results.")

            Catch exception As TimeoutException

                Console.WriteLine("Got {0}", exception.[GetType]())
                client.Abort()

            Catch exception As CommunicationException

                ' Control comes here when client.Divide throws.  The actual Exception
                ' type is CommunicationObjectAbortedException, which is a subclass of
                ' CommunicationException.
                Console.WriteLine("Got {0}", exception.[GetType]())
                client.Abort()

            End Try

        End Sub

        Private Shared Sub DemonstrateTimeoutException()

            ' Create a client
            Dim client As New CalculatorClient()

            Try

                ' Call the Add service operation.
                Dim value1 As Double = 100
                Dim value2 As Double = 15.99
                Dim result As Double = client.Add(value1, value2)
                Console.WriteLine("Add({0},{1}) = {2}", value1, value2, result)

                ' Set a ridiculously small timeout.  This will cause the next call to
                ' fail with a TimeoutException because it cannot process in time.
                Console.WriteLine("Set timeout too short for method to complete...")
                client.InnerChannel.OperationTimeout = TimeSpan.FromMilliseconds(0.001)

                ' Call the Divide service operation.
                value1 = 22
                value2 = 7
                result = client.Divide(value1, value2)
                Console.WriteLine("Divide({0},{1}) = {2}", value1, value2, result)

                ' SHOULD NOT GET HERE -- Divide should throw

                ' If we had gotten here, we would want to close the client gracefully so
                ' that the channel closes gracefully and cleans up resources.
                client.Close()

                Console.WriteLine("Service successfully returned all results.")

            Catch exception As TimeoutException

                ' Control comes here when client.Divide throws a TimeoutException.
                Console.WriteLine("Got {0}", exception.[GetType]())
                client.Abort()

            Catch exception As CommunicationException

                Console.WriteLine("Got {0}", exception.[GetType]())
                client.Abort()

            End Try

        End Sub

    End Class

End Namespace