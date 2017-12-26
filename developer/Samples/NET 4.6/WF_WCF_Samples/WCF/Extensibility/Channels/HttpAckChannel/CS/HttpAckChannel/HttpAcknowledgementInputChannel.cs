//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Channels;

namespace Microsoft.Samples.HttpAckChannel
{
    class HttpAcknowledgementInputChannel : ChannelBase, IInputChannel
    {
        IReplyChannel innerChannel;
        ReceiveContextSettings receiveContextSettings;

        public HttpAcknowledgementInputChannel(IReplyChannel innerChannel,
            HttpAcknowledgementChannelListener listener, ReceiveContextSettings receiveContextSettings)
            : base(listener)
        {
            this.innerChannel = innerChannel;
            this.receiveContextSettings = receiveContextSettings;
        }

        public EndpointAddress LocalAddress
        {
            get { return this.innerChannel.LocalAddress; }
        }

        protected override void OnOpen(TimeSpan timeout)
        {
            this.innerChannel.Open(timeout);
        }

        protected override IAsyncResult OnBeginOpen(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return this.innerChannel.BeginOpen(timeout, callback, state);
        }

        protected override void OnEndOpen(IAsyncResult result)
        {
            this.innerChannel.EndOpen(result);
        }

        protected override void OnClose(TimeSpan timeout)
        {
            this.innerChannel.Close(timeout);
        }

        protected override IAsyncResult OnBeginClose(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return this.innerChannel.BeginClose(timeout, callback, state);
        }

        protected override void OnEndClose(IAsyncResult result)
        {
            this.innerChannel.EndClose(result);
        }

        protected override void OnAbort()
        {
            this.innerChannel.Abort();
        }

        public bool WaitForMessage(TimeSpan timeout)
        {
            return this.innerChannel.WaitForRequest(timeout);
        }

        public IAsyncResult BeginWaitForMessage(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return this.innerChannel.BeginWaitForRequest(timeout, callback, state);
        }

        public bool EndWaitForMessage(IAsyncResult result)
        {
            return this.innerChannel.EndWaitForRequest(result);
        }

        public Message Receive()
        {
            return this.Receive(this.DefaultReceiveTimeout);
        }

        public Message Receive(TimeSpan timeout)
        {
            RequestContext requestContext = this.innerChannel.ReceiveRequest(timeout);
            return AddReceiveContextProperty(requestContext); 
        }

        public IAsyncResult BeginReceive(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return this.innerChannel.BeginReceiveRequest(timeout, callback, state);
        }

        public IAsyncResult BeginReceive(AsyncCallback callback, object state)
        {
            return this.innerChannel.BeginReceiveRequest(callback, state);
        }

        public Message EndReceive(IAsyncResult result)
        {
            RequestContext requestContext = this.innerChannel.EndReceiveRequest(result);
            return AddReceiveContextProperty(requestContext); 
        }

        public bool TryReceive(TimeSpan timeout, out Message message)
        {
            RequestContext requestContext;
            message = null;
            if (this.innerChannel.TryReceiveRequest(timeout, out requestContext))
            {
                message = AddReceiveContextProperty(requestContext);
            }
            else
            {
                return false;
            }
            return true;
        }

        public IAsyncResult BeginTryReceive(TimeSpan timeout, AsyncCallback callback, object state)
        {
            return this.innerChannel.BeginTryReceiveRequest(timeout, callback, state);
        }

        public bool EndTryReceive(IAsyncResult result, out Message message)
        {
            RequestContext requestContext;
            message = null;
            if (this.innerChannel.EndTryReceiveRequest(result, out requestContext))
            {
                message = AddReceiveContextProperty(requestContext);
            }
            else
            {
                return false;
            }
            return true;
        }

        Message AddReceiveContextProperty(RequestContext requestContext)
        {
            Message message = null;
            if (requestContext != null)
            {
                message = requestContext.RequestMessage;
                if (message != null)
                {
                    message.Properties.Add(ReceiveContext.Name, new HttpAcknowledgementReceiveContext(requestContext));
                }
            }
            return message;
        }
    }
}
