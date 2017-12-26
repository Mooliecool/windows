// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
using System;
using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using System.Text;

namespace Microsoft.Samples.RecipeCatalog
{
	class Program
	{

		static ServiceHost serviceHost;
		
		static void Main(string[] args)
		{
			// Get base address from app settings in configuration.
			Uri baseAddress = new Uri(System.Configuration.ConfigurationManager.AppSettings["baseAddress"]);

			// Create a ServiceHost<T> to host the DocumentService site and endpoints
            serviceHost = new ServiceHost(typeof (DocumentService), baseAddress);

			// Open the ServiceHost to create listeners and start listening for messages.
			serviceHost.Open();

			// Display our base address(es) and endpoint(s), then wait...
			int n = 0;
			foreach (Uri uri in serviceHost.BaseAddresses)
			{
				Console.WriteLine("Base Address {0}: {1}", n++, uri.AbsoluteUri);
			}
			n = 0;
			foreach (ServiceEndpoint ep in serviceHost.Description.Endpoints)
			{
				Console.WriteLine("Endpoint {0}: {1}", n++, ep.Address);
			}
			Console.WriteLine("Press Enter to stop the service...");
			Console.ReadLine();

			// Shutdown the DocumentService service
			serviceHost.Close();
		}
	}
}
