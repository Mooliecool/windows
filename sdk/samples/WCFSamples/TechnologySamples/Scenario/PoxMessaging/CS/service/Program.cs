using System;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples
{
	class Program
	{
		static void Main(string[] args)
		{
			ServiceHost host = new ServiceHost(typeof(CustomerService));
			host.Open();

			Console.WriteLine("Ready and waiting at:");
            foreach (ServiceEndpoint endpoint in host.Description.Endpoints)
            {
                Console.WriteLine("\t" + endpoint.Address.Uri);
            }
			Console.ReadLine();

			host.Close();
		}
	}
}
