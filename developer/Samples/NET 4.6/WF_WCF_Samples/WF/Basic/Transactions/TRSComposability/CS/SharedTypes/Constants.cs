//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.Xml.Linq;

namespace Microsoft.Samples.TRSComposabilitySample
{

    public static class Constants
    {
        public static readonly string ServerAddress = "net.tcp://localhost/TRSSample/Server";
        public static readonly XName ServiceContractName = XName.Get("IServiceContract", "http://tempuri.org/");

        public static readonly Endpoint ServerEndpoint = new Endpoint
        {
            Binding = new NetTcpBinding
            {
                TransactionFlow = true
            },
            AddressUri = new Uri(ServerAddress),
        };
    }
}
