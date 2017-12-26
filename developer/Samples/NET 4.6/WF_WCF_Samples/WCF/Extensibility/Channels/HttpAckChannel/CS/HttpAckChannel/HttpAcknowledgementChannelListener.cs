//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.ServiceModel.Channels;

namespace Microsoft.Samples.HttpAckChannel
{
    class HttpAcknowledgementChannelListener : ChannelListenerBase<IInputChannel>
    {
        IChannelListener<IReplyChannel> innerListener;

        public HttpAcknowledgementChannelListener(BindingContext context, ReceiveContextSettings receiveContextSettings)
        {
            this.innerListener = context.BuildInnerChannelListener<IReplyChannel>();
            this.ReceiveContextSettings = receiveContextSettings;
        }

        public override Uri Uri
        {
            get { return this.innerListener.Uri; }
        }

        internal ReceiveContextSettings ReceiveContextSettings {get; set;}

        protected override bool OnWaitForChannel(TimeSpan timeout)
        {
            return this.innerListener.WaitForChannel(timeout);
        }

        protected override IAsyncResult OnBeginWaitForChannel(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return this.innerListener.BeginWaitForChannel(timeout, callback, state);
        }

        protected override bool OnEndWaitForChannel(IAsyncResult result)
        {
            return this.innerListener.EndWaitForChannel(result);
        }

        protected override void OnClose(TimeSpan timeout)
        {
            this.innerListener.Close(timeout);
        }

        protected override IAsyncResult OnBeginClose(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return this.innerListener.BeginClose(timeout, callback, state);
        }

        protected override void OnEndClose(IAsyncResult result)
        {
            this.innerListener.EndClose(result);
        }

        protected override void OnOpen(TimeSpan timeout)
        {
            this.innerListener.Open(timeout);
        }

        protected override IAsyncResult OnBeginOpen(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return this.innerListener.BeginOpen(timeout, callback, state);
        }

        protected override void OnEndOpen(IAsyncResult result)
        {
            this.innerListener.EndOpen(result);
        }

        protected override void OnAbort()
        {
            this.innerListener.Abort();
        }

        protected override IInputChannel OnAcceptChannel(TimeSpan timeout)
        {
            IReplyChannel innerChannel = this.innerListener.AcceptChannel(timeout);
            return WrapChannel(innerChannel);
        }

        protected override IAsyncResult OnBeginAcceptChannel(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return this.innerListener.BeginAcceptChannel(timeout, callback, state);
        }

        protected override IInputChannel OnEndAcceptChannel(IAsyncResult result)
        {
            IReplyChannel innerChannel = this.innerListener.EndAcceptChannel(result);
            return WrapChannel(innerChannel);
        }

        private IInputChannel WrapChannel(IReplyChannel innerChannel)
        {
            if (innerChannel == null)
            {
                return null;
            }
            return new HttpAcknowledgementInputChannel(innerChannel, this, this.ReceiveContextSettings);
        }

        public override T GetProperty<T>()
        {
            if (typeof(T) == typeof(IReceiveContextSettings))
            {
                return (T)(object)this.ReceiveContextSettings;
            }

            return base.GetProperty<T>();
        }
    }
}
