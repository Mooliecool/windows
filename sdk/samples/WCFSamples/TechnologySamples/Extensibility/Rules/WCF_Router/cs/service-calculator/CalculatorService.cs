//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ServiceModel;
using System.ServiceModel.Dispatcher;

namespace Microsoft.ServiceModel.Samples
{
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples/", SessionMode=SessionMode.Required)]
    public interface ICalculatorService
    {
        [OperationContract(IsOneWay = false)]
        int Add(int number);

        [OperationContract(IsOneWay = false)]
        int Subtract(int number);
    }

    [ServiceBehavior(InstanceContextMode = InstanceContextMode.PerSession)]
    public class CalculatorService : ICalculatorService
    {
        int total = 0;

        int ICalculatorService.Add(int number)
        {
            this.total += number;
            return this.total;
        }

        int ICalculatorService.Subtract(int number)
        {
            this.total -= number;
            return this.total;
        }
    }

    public class ServiceDriver
    {
        public static void Main(string[] args)
        {
            ServiceHost serviceHost = new ServiceHost(typeof(CalculatorService));
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
