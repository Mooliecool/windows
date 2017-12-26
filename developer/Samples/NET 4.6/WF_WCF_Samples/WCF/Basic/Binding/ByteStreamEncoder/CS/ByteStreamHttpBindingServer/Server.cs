//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;

namespace Microsoft.Samples.ByteStreamEncoder
{

    class Server
    {
        public static void Main()
        {
            Uri baseAddress = new Uri("http://localhost:8000/byteStream");
            Console.WriteLine("Testing image upload and download using ByteStreamHttpBinding");
            using (ServiceHost host = new ServiceHost(new FileService(), baseAddress))
            {
                host.Open();
                Console.WriteLine("Service started at: " + host.ChannelDispatchers[0].Listener.Uri.AbsoluteUri);
                Console.WriteLine("Press <ENTER> to terminate service");
                Console.ReadLine();
            }
        }
    }
}
