// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel.Activation;

namespace Microsoft.ServiceModel.Samples.Hosting
{
    class HostedUdpTransportConfiguration : HostedTransportConfiguration
    {
        public HostedUdpTransportConfiguration()
        {
        }

        public override Uri[] GetBaseAddresses(string virtualPath)
        {
            return HostedUdpTransportConfigurationImpl.Value.GetBaseAddresses(virtualPath);
        }
    }
}
