//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.PeerResolvers;
using System.Configuration;

namespace Microsoft.ServiceModel.Samples
{
    class DefaultCustomResolver
    {
        public static void Main()
        {
            // Create a new resolver service
            CustomPeerResolverService crs = new CustomPeerResolverService();
            crs.ControlShape = false;

            // Create a new service host
            ServiceHost customResolver = new ServiceHost(crs);

            // Open the resolver service 
            crs.Open();
            customResolver.Open();
            Console.WriteLine("Custom resolver service is started");
            Console.WriteLine("Press <ENTER> to terminate service");
            Console.ReadLine();
        }
    }
}
