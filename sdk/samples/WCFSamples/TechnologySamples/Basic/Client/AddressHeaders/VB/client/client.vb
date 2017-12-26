' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel
Imports System.ServiceModel.Channels

Namespace Microsoft.ServiceModel.Samples

    ' The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    ' Client implementation code.
    Class Client

        Public Shared ReadOnly IDName As String = "ID"
        Public Shared ReadOnly IDNamespace As String = "http://Microsoft.ServiceModel.Samples"

        Public Shared Sub Main()

            ' Create a client.
            Dim client As New HelloClient()

            ' Add a reference-parameter header to the address.
            ' Since the EndpointAddress class is immutable, we must use
            ' EndpointAddressBuilder to change the value.
            Dim builder As New EndpointAddressBuilder(client.Endpoint.Address)
            Dim header As AddressHeader = AddressHeader.CreateAddressHeader(IDName, IDNamespace, "John")
            builder.Headers.Add(header)
            client.Endpoint.Address = builder.ToEndpointAddress()

            ' Call the Hello service operation.
            Console.WriteLine(client.Hello())

            'Closing the client gracefully closes the connection and cleans up resources
            client.Close()

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace