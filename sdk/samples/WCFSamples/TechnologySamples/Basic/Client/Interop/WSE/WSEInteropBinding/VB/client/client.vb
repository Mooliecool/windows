' Copyright (c) Microsoft Corporation.  All Rights Reserved.

Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Security.Cryptography.X509Certificates
Imports System.ServiceModel
Imports System.ServiceModel.Security
Imports System.ServiceModel.Channels
Imports Microsoft.VisualBasic

Namespace Microsoft.ServiceModel.Samples

    ' The service contract is defined in generatedClient.vb, generated from the service by

    ' the svcutil tool.
    Class Program

        Public Shared Sub Main(ByVal args As String())

            CallWcfService()
            ' If the WSE 3.0 SDK is installed, uncomment this code, compile the WSSecurityAnonymous 
            ' WSE 3.0 Quickstart sample and use the configured WseHttpBinding to call the service.
            ' Passing "true" indicates that a WSE 3.0 Policy file will be read
            ' to configure the WseHttpBinding. e.g. CallWseService(true);
            '
            ' CallWseService(True)

        End Sub

        Private Shared Sub CallWcfService()

            Dim address As New EndpointAddress(New Uri("http://localhost:8000/servicemodelsamples/service"), EndpointIdentity.CreateDnsIdentity("localhost"))

            Dim binding As New WseHttpBinding()
            binding.SecurityAssertion = WseSecurityAssertion.AnonymousForCertificate
            binding.EstablishSecurityContext = True
            binding.RequireDerivedKeys = True
            binding.MessageProtectionOrder = MessageProtectionOrder.SignBeforeEncrypt

            ' Use the calculator client in generatedClient.vb
            Dim client As New CalculatorClient(binding, address)
            client.ClientCredentials.ServiceCertificate.SetDefaultCertificate(StoreLocation.CurrentUser, StoreName.TrustedPeople, X509FindType.FindBySubjectName, "localhost")
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

            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

        Private Shared Sub CallWseService(ByVal usePolicyFile As Boolean)

            Dim address As New EndpointAddress(New Uri("http://localhost/WSSecurityAnonymousPolicy/WSSecurityAnonymousService.asmx"), EndpointIdentity.CreateDnsIdentity("WSE2QuickStartServer"))

            Dim binding As New WseHttpBinding()
            If Not usePolicyFile Then

                binding.SecurityAssertion = WseSecurityAssertion.AnonymousForCertificate
                binding.EstablishSecurityContext = False
                binding.RequireDerivedKeys = True
                binding.MessageProtectionOrder = MessageProtectionOrder.SignBeforeEncrypt

            Else
                binding.LoadPolicy("wse3policyCache.config", "ServerPolicy")
            End If

            ' Use the WSSecurityAnonymousServiceSoapClient client in wseclient.vb
            Dim client As New WSSecurityAnonymousServiceSoapClient(binding, address)

            ' Need to supply the credentials depending on the type of WseSecurityAssertion used.
            ' Anonymous only requires server certificate. UsernameForCertificate would also require
            ' a username and password to be supplied.
            client.ClientCredentials.ServiceCertificate.SetDefaultCertificate(StoreLocation.LocalMachine, StoreName.My, X509FindType.FindBySubjectDistinguishedName, "CN=WSE2QuickStartServer")
            Dim symbols As String() = New String() {"FABRIKAM", "CONTOSO"}
            Dim quotes As StockQuote() = client.StockQuoteRequest(symbols)

            client.Close()

            ' Success!
            For Each quote As StockQuote In quotes

                Console.WriteLine("")
                Console.WriteLine("Symbol: " + quote.Symbol)
                Console.WriteLine(vbTab & "Name:" & vbTab & vbTab & vbTab & quote.Name)
                Console.WriteLine(vbTab & "Last Price:" & vbTab & vbTab & quote.Last)
                Console.WriteLine(vbTab & "Previous Change:" & vbTab & quote.PreviousChange & "%")

            Next

            Console.WriteLine("Press <ENTER> to terminate client.")
            Console.ReadLine()

        End Sub

    End Class

End Namespace
