'****************************** Module Header ******************************\
' Module Name:	Program.vb
' Project:		Client
' Copyright (c) Microsoft Corporation.
' 
' This is the client application that verifies the workflow service works fine.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.ServiceModel
Imports Microsoft.ServiceBus

Module Program

	Sub Main()
		' Replace {service namespace} with your service name space.
		Dim address As New EndpointAddress(ServiceBusEnvironment.CreateServiceUri("https", "{service namespace}", "ProcessDataWorkflowService"))
		Dim binding As New BasicHttpRelayBinding

		' Provide the Service Bus credential.
		Dim sharedSecretServiceBusCredential As New TransportClientEndpointBehavior
		sharedSecretServiceBusCredential.CredentialType = TransportClientCredentialType.SharedSecret
		' Replace {issuer name} and {issuer secret} with your credential.
		sharedSecretServiceBusCredential.Credentials.SharedSecret.IssuerName = "{issuer name}"
		sharedSecretServiceBusCredential.Credentials.SharedSecret.IssuerSecret = "{issuer secret}"
		Dim factory As New ChannelFactory(Of IProcessDataWorkflowServiceChannel)(binding, address)
		factory.Endpoint.Behaviors.Add(sharedSecretServiceBusCredential)
		factory.Open()
		Dim channel As IProcessDataWorkflowServiceChannel = factory.CreateChannel
		channel.Open()
		Console.WriteLine("Processing 10...")
		Console.WriteLine(("Service returned: " & channel.ProcessData(New ProcessDataRequest(0)).string))
		Console.WriteLine("Processing 30...")
		Console.WriteLine(("Service returned: " & channel.ProcessData(New ProcessDataRequest(30)).string))
		channel.Close()
        factory.Close()
        Console.Read()
	End Sub

End Module
