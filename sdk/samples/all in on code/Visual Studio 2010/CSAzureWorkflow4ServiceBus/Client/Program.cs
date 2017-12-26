/****************************** Module Header ******************************\
* Module Name:	Program.cs
* Project:		Client
* Copyright (c) Microsoft Corporation.
* 
* This is the client application that verifies the workflow service works fine.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;
using Microsoft.ServiceBus;

namespace Client
{
	class Program
	{
		static void Main(string[] args)
		{
			// Replace {service namespace} with your service name space.
			EndpointAddress address = new EndpointAddress(ServiceBusEnvironment.CreateServiceUri("https", "{service namespace}", "ProcessDataWorkflowService"));
			BasicHttpRelayBinding binding = new BasicHttpRelayBinding();

			// Provide the Service Bus credential.
			TransportClientEndpointBehavior sharedSecretServiceBusCredential = new TransportClientEndpointBehavior();
			sharedSecretServiceBusCredential.CredentialType = TransportClientCredentialType.SharedSecret;
			// Replace {issuer name} and {issuer secret} with your credential.
			sharedSecretServiceBusCredential.Credentials.SharedSecret.IssuerName = "{issuer name}";
			sharedSecretServiceBusCredential.Credentials.SharedSecret.IssuerSecret = "{issuer secret}";

			ChannelFactory<IProcessDataWorkflowServiceChannel> factory = new ChannelFactory<IProcessDataWorkflowServiceChannel>(binding, address);
			factory.Endpoint.Behaviors.Add(sharedSecretServiceBusCredential);
			factory.Open();
			IProcessDataWorkflowServiceChannel channel = factory.CreateChannel();
			channel.Open();
			Console.WriteLine("Processing 10...");
			Console.WriteLine("Service returned: " + channel.ProcessData(new ProcessDataRequest(0)).@string);
			Console.WriteLine("Processing 30...");
			Console.WriteLine("Service returned: " + channel.ProcessData(new ProcessDataRequest(30)).@string);
			channel.Close();
			factory.Close();
            Console.Read();
		}
	}
}
