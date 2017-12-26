' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.ServiceModel
Imports System.Security.Cryptography.X509Certificates
Imports System.Net

Namespace Microsoft.ServiceModel.Samples

    'The service contract is defined in generatedClient.vb, generated from the service by the svcutil tool.

    'Client implementation code.
    Class Client

        Public Shared Sub Main()
            ' WARNING: This code is only needed for test certificates such as those created by makecert. It is 
            ' not recommended for production code.
            PermissiveCertificatePolicy.Enact("CN=ServiceModelSamples-HTTPS-Server")

            ' Create a client with given client endpoint configuration
            Dim client As New CalculatorClient()

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

            Console.WriteLine()
            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

    ' WARNING: This code is only needed for test certificates such as those created by makecert. It is 
    ' not recommended for production code.
    Class PermissiveCertificatePolicy

        Private subjectName As String
        Shared currentPolicy As PermissiveCertificatePolicy
        Private Sub New(ByVal subjectName As String)

            Me.subjectName = subjectName
            ServicePointManager.ServerCertificateValidationCallback = AddressOf RemoteCertValidate

        End Sub

        Public Shared Sub Enact(ByVal subjectName As String)

            currentPolicy = New PermissiveCertificatePolicy(subjectName)

        End Sub

        Private Function RemoteCertValidate(ByVal sender As Object, ByVal cert As X509Certificate, ByVal chain As X509Chain, ByVal [error] As System.Net.Security.SslPolicyErrors) As Boolean

            If cert.Subject = subjectName Then

                Return True

            End If

            Return False

        End Function

    End Class

End Namespace
