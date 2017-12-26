// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

using System;
using System.Web.Hosting;
using Microsoft.ServiceModel.Samples.Activation;
using System.Collections.Generic;

namespace Microsoft.ServiceModel.Samples.Hosting
{
    class UdpAppDomainProtocolHandler : AppDomainProtocolHandler
    {
        IListenerChannelCallback listenerChannelCallback;

        public override void StartListenerChannel(IListenerChannelCallback listenerChannelCallback)
        {
            this.listenerChannelCallback = listenerChannelCallback;

            // Start the real work here
            HostedUdpTransportConfigurationImpl.Value.TransportManager.Open(listenerChannelCallback.GetId());

            // Report started
            listenerChannelCallback.ReportStarted();
        }

        public override void StopListenerChannel(int listenerChannelId, bool immediate)
        {
            listenerChannelCallback.ReportStopped(0);
        }

        public override void StopProtocol(bool immediate)
        {
            // FUTURE: we need to clean up the transport manager so that we won't receive new messages since after.
            listenerChannelCallback.ReportStopped(0);
            HostingEnvironment.UnregisterObject(this);
        }
    }
}
