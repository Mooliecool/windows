' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel.Description
Imports System.ServiceModel.Channels
Imports System.Configuration
Imports System.ServiceModel
Imports System.Xml

Namespace Microsoft.ServiceModel.Samples

    ' Client implementation code.
    ' Dynamically retrieve metadata from the service and use the metadata to 
    ' communicate with each endpoint that implements the ICalculator contract. 
    Class Client
        Public Shared Sub Main()
            ' Create a MetadataExchangeClient for retrieving metadata.
            Dim mexAddress As New EndpointAddress(ConfigurationManager.AppSettings("mexAddress"))
            Dim mexClient As New MetadataExchangeClient(mexAddress)

            ' Retrieve the metadata for all endpoints using metadata exchange protocol (mex).
            Dim metadataSet As MetadataSet = mexClient.GetMetadata()

            'Convert the metadata into endpoints
            Dim importer As New WsdlImporter(metadataSet)
            Dim endpoints As ServiceEndpointCollection = importer.ImportAllEndpoints()

            Dim client As CalculatorClient = Nothing
            Dim contract As ContractDescription = ContractDescription.GetContract(GetType(ICalculator))
            ' Communicate with each endpoint that supports the ICalculator contract.
            For Each ep As ServiceEndpoint In endpoints

                If ep.Contract.[Namespace].Equals(contract.[Namespace]) AndAlso ep.Contract.Name.Equals(contract.Name) Then

                    ' Create a client using the endpoint address and binding.
                    client = New CalculatorClient(ep.Binding, New EndpointAddress(ep.Address.Uri))
                    Console.WriteLine("Communicate with endpoint: ")
                    Console.WriteLine("   AddressPath={0}", ep.Address.Uri.PathAndQuery)
                    Console.WriteLine("   Binding={0}", ep.Binding.Name)

                    ' call operations
                    DoCalculations(client)

                    'Closing the client gracefully closes the connection and cleans up resources
                    client.Close()

                End If
            Next

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

        Private Shared Sub DoCalculations(ByVal client As CalculatorClient)
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

        End Sub

    End Class

End Namespace
