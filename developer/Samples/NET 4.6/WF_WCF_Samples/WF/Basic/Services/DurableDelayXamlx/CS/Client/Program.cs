//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using Client.DurableDelayService;

namespace Microsoft.Samples.DurableDelayXamlX
{
    class Program
    {   
        static void Main()
        {
            ServiceClient client = new ServiceClient();
            client.Start();

            Console.WriteLine("Activating Durable Delay Service");
            Console.WriteLine("Done - see C:\\WFDurableDelaySampleResult.txt");
            Console.WriteLine("Press <ENTER> to exit.");
            Console.ReadLine();

            client.Close();
        }
    }
}
