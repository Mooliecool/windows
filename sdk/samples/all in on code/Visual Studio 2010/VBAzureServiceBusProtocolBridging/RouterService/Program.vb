'****************************** Module Header ******************************\
' Project Name:   CSAzureServiceBusProtocolBridging
' Module Name:    RouterService
' File Name:      Program.vb
' Copyright (c) Microsoft Corporation
'
' This console application exposes a Routing Service through Service Bus 
' https://{namespace}.servicebus.windows.net/MyService.
' The Routing Service routs WCF requests to net.tcp://localhost:9090/MyService
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports Microsoft.ServiceBus
Imports System.ServiceModel
Imports System.ServiceModel.Routing
Imports System.ServiceModel.Description
Imports Common

Module Program

    Sub Main()
        ' Create the service URI based on the service namespace.
        Dim address As Uri = ServiceBusEnvironment.CreateServiceUri("https", Settings.ServiceNamespace, "")

        ' Create the credential object for the endpoint.
        Dim sharedSecretServiceBusCredential As New TransportClientEndpointBehavior()
        sharedSecretServiceBusCredential.CredentialType = TransportClientCredentialType.SharedSecret
        sharedSecretServiceBusCredential.Credentials.SharedSecret.IssuerName = Settings.IssuerName
        sharedSecretServiceBusCredential.Credentials.SharedSecret.IssuerSecret = Settings.IssuerSecret

        Console.WriteLine("Starting service.")
        Try
            Using host As New ServiceHost(GetType(RoutingService), address)
                ' Add the Service Bus credentials to all endpoints specified in configuration.
                For Each endpoint As ServiceEndpoint In host.Description.Endpoints
                    endpoint.Behaviors.Add(sharedSecretServiceBusCredential)
                Next

                host.Open()

                Console.WriteLine("Service is ready at {0}", host.Description.Endpoints(0).Address.ToString())

                Console.WriteLine("Please press [Enter] to exit.")
                Console.ReadLine()
            End Using
        Catch ex As Exception
            Console.WriteLine(ex.Message)
            Console.WriteLine("Please press [Enter] to exit.")
            Console.ReadLine()
        End Try
    End Sub

End Module
