'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.ServiceModel
Imports System.ServiceModel.Discovery

Namespace Microsoft.Samples.Discovery

    Friend Class Client
        Public Shared Sub Main()
            FindCalculatorServiceAddress()

            Console.WriteLine("Press <ENTER> to exit.\n")
            Console.ReadLine()
        End Sub

        Private Shared Sub FindCalculatorServiceAddress()
            Dim discoveryClient As New DiscoveryClient(New UdpDiscoveryEndpoint())

            AddHandler discoveryClient.FindCompleted, AddressOf ClientFindCompleted
            AddHandler discoveryClient.FindProgressChanged, AddressOf ClientFindProgressChanged

            Console.WriteLine("Finding ICalculatorServices endpoints asynchronously...\n")

            Dim findCriteria As New FindCriteria(GetType(ICalculatorService))
            findCriteria.Duration = TimeSpan.FromSeconds(5)

            ' Find ICalculatorService endpoint asynchronously. The results are returned asynchronously via events            
            discoveryClient.FindAsync(findCriteria)
        End Sub

        Private Shared Sub ClientFindProgressChanged(ByVal sender As Object, ByVal e As FindProgressChangedEventArgs)
            Console.WriteLine("Found ICalculatorService endpoint at {0}\n", e.EndpointDiscoveryMetadata.Address.Uri)
            InvokeCalculatorService(e.EndpointDiscoveryMetadata.Address)
        End Sub

        Private Shared Sub ClientFindCompleted(ByVal sender As Object, ByVal e As FindCompletedEventArgs)
            ' Implement this method to access the FindResponse object through e.Result, which includes all the endpoints found
        End Sub

        Private Shared Sub InvokeCalculatorService(ByVal endpointAddress As EndpointAddress)
            ' Create a client
            Dim client As New CalculatorServiceClient()

            ' Connect to the discovered service endpoint
            client.Endpoint.Address = endpointAddress

            Console.WriteLine("Invoking CalculatorService at {0}", endpointAddress)

            Dim value1 As Double = 100.0R
            Dim value2 As Double = 15.99R

            ' Call the Add service operation.
            Dim result As Double = client.Add(value1, value2)
            Console.WriteLine("Add({0},{1}) = {2}", value1, value2, result)

            ' Call the Subtract service operation.
            result = client.Subtract(value1, value2)
            Console.WriteLine("Subtract({0},{1}) = {2}", value1, value2, result)

            ' Call the Multiply service operation.
            result = client.Multiply(value1, value2)
            Console.WriteLine("Multiply({0},{1}) = {2}", value1, value2, result)

            ' Call the Divide service operation.
            result = client.Divide(value1, value2)
            Console.WriteLine("Divide({0},{1}) = {2}\n", value1, value2, result)

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()
        End Sub
    End Class
End Namespace

