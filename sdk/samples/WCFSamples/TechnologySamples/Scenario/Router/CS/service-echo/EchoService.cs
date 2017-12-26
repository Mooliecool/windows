// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------
namespace Microsoft.ServiceModel.Samples
{
    using System;
    using System.ServiceModel;
	using System.ServiceModel.Dispatcher;
    using System.Diagnostics;
    using System.Configuration;

    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples/")]
    public interface IEchoService
    {
        [OperationContract(IsOneWay = false)]
        string Echo(string message);
    }

    [ServiceBehavior(InstanceContextMode = InstanceContextMode.Single)]
    public class EchoService : IEchoService
    {
        string IEchoService.Echo(string message)
        {
            return message;
        }
    }

    public class ServiceDriver
    {
        public static void Main(string[] args)
        {
            ServiceHost serviceHost = new ServiceHost(typeof(EchoService));
            serviceHost.Open();

            for (int i = 0; i < serviceHost.ChannelDispatchers.Count; i++)
            {
                ChannelDispatcher channelDispatcher = serviceHost.ChannelDispatchers[i] as ChannelDispatcher;
                if (channelDispatcher != null)
                {
                    for (int j = 0; j < channelDispatcher.Endpoints.Count; j++)
                    {
                        EndpointDispatcher endpointDispatcher = channelDispatcher.Endpoints[j];
                        Console.WriteLine("Listening on " + endpointDispatcher.EndpointAddress + "...");
                    }
                }
            }

            Console.WriteLine();
            Console.WriteLine("Press Enter to exit...");
            Console.ReadLine();
        }
    }
}
