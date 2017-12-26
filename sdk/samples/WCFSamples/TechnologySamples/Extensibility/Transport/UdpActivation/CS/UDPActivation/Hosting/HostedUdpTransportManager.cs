using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel.Channels;
using System.ServiceModel;
using Microsoft.ServiceModel.Samples;

namespace Microsoft.ServiceModel.Samples.Hosting
{
    class HostedUdpTransportManager : UdpTransportManager
    {
        HostedUdpTransportListener transportListener;
        public HostedUdpTransportManager(Uri listenUri)
            : base(listenUri)
        {
        }

        public void Open(int instanceId)
        {
            transportListener = new HostedUdpTransportListener(instanceId, ListenUri,
                this.OnDataReceived);
            transportListener.Open();
        }

        public void OnDataReceived(FramingData data)
        {
            ServiceHostingEnvironment.EnsureServiceAvailable(data.To.LocalPath);

            base.Dispatch(data);
        }
    }
}
