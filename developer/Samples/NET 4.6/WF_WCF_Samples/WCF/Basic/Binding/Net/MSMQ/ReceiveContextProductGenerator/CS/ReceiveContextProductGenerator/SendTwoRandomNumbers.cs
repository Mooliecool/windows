//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;

namespace Microsoft.Samples.Queues
{
    class SendTwoRandomNumbers
    {
        public static void SendNumbers(Uri queueAddress, int numberOfMessages)
        {
            NetMsmqBinding binding = new NetMsmqBinding(NetMsmqSecurityMode.None);
            ChannelFactory<IProductCalculator> factory = 
                    new ChannelFactory<IProductCalculator>(binding);
            factory.Open();
            IProductCalculator ipc = factory.CreateChannel(new EndpointAddress(queueAddress.ToString()));

            Random rand = new Random();
            for (int i = 0; i < numberOfMessages; i++)
            {
                int num1 = rand.Next(1, 10);
                int num2 = rand.Next(1, 10);
                ipc.CalculateProduct(num1, num2);
            }
            factory.Close();
        }
    }
}
