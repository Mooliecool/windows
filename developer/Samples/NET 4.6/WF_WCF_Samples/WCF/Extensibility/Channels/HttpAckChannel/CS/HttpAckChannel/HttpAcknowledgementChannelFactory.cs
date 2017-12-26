//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Channels;

namespace Microsoft.Samples.HttpAckChannel
{
    class HttpAcknowledgementChannelFactory : ChannelFactoryBase<IOutputChannel>
    {
        IChannelFactory<IRequestChannel> innerFactory;

        public HttpAcknowledgementChannelFactory(BindingContext context)
        {
            this.innerFactory = context.BuildInnerChannelFactory<IRequestChannel>();
        }
        
        protected override void OnOpen(TimeSpan timeout)
        {
            innerFactory.Open(timeout);
        }

        protected override IAsyncResult OnBeginOpen(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return innerFactory.BeginOpen(timeout, callback, state);
        }

        protected override void OnEndOpen(IAsyncResult result)
        {
            innerFactory.EndOpen(result);
        }

        protected override void OnClose(TimeSpan timeout)
        {
            innerFactory.Close(timeout);
        }

        protected override IAsyncResult OnBeginClose(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return innerFactory.BeginClose(timeout, callback, state);
        }

        protected override void OnEndClose(IAsyncResult result)
        {
            innerFactory.EndClose(result);
        }

        protected override void OnAbort()
        {
            innerFactory.Abort();
        }

        protected override IOutputChannel OnCreateChannel(EndpointAddress to, Uri via)
        {
            IRequestChannel innerChannel = innerFactory.CreateChannel(to, via);
            return new HttpAcknowledgementOutputChannel(innerChannel, this);
        }
    }
}
