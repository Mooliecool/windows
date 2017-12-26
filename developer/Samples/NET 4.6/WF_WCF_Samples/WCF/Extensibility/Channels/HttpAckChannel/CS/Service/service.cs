//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.ServiceModel;

namespace Microsoft.Samples.HttpAckChannel
{
    class Program
    {
        static void Main(string[] args)
        {
            ServiceHost host = new ServiceHost(typeof(ProcessEmployeeDataService));
            host.Open();

            Console.WriteLine("The host has started...");
            Console.WriteLine("Press <ENTER> to shut down the service...");
            Console.ReadLine();
            host.Close();
        }
    }
}
