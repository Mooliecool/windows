/****************************** Module Header ******************************\
* Module Name:	Program.cs
* Project:		CSAzureServiceBusSLRest
* Copyright (c) Microsoft Corporation.
* 
* This is the Main entry point. It hosts the service,
* and exposes it to the internet using Service Bus.
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
using System.Configuration;
using System.ServiceModel.Web;
using Microsoft.ServiceBus;

namespace AzureServiceBusSLRest
{
	class Program
	{
		static void Main(string[] args)
		{
			string serviceNamespace = ConfigurationManager.AppSettings["serviceNamespace"];
			// By default, tranport level security is required for all *RelayBindings; hence, using https is required.
			Uri address = ServiceBusEnvironment.CreateServiceUri("https", serviceNamespace, "");
			WebServiceHost host = new WebServiceHost(typeof(RestService), address);
			host.Open();

			Console.WriteLine("Copy the following address into a browser to see the cross domain policy file:");
			Console.WriteLine(address + "clientaccesspolicy.xml");
			Console.WriteLine();
			Console.WriteLine("The WCF REST service is being listened on:");
			Console.WriteLine(address + "file/");
			Console.WriteLine();
			Console.WriteLine("Press [Enter] to exit");
			Console.ReadLine();
			host.Close();
		}
	}
}
