using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;
using System.ServiceModel.Channels;
using Microsoft.Samples.Channels.ChunkingChannel;
namespace TestService
{
    class Host
    {
        public static void Main()
        {
            CustomBinding binding = new CustomBinding();
            ServiceHost host = new ServiceHost(typeof(service), 
                new Uri("net.tcp://localhost:9000/TestService"), 
                new Uri("http://localhost:8000/TestService"));
            host.AddServiceEndpoint(
                typeof(ITestService),
                new TcpChunkingBinding(),
                "ep1");
            host.Open();
            Console.WriteLine("Service started, press enter to exit");
            Console.ReadLine();
            host.Close();
        }
    }
}
