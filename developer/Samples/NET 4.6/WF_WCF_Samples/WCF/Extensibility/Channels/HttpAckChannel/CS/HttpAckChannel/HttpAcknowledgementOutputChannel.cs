//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Net;
using System.ServiceModel;
using System.ServiceModel.Channels;

namespace Microsoft.Samples.HttpAckChannel
{
    class HttpAcknowledgementOutputChannel : ChannelBase, IOutputChannel
    {
        IRequestChannel innerChannel;

        public HttpAcknowledgementOutputChannel(IRequestChannel inner, HttpAcknowledgementChannelFactory creator)
            : base(creator)
        {
            this.innerChannel = inner;
        }

        public Uri Via
        {
            get { return innerChannel.Via; }
        }

        public EndpointAddress RemoteAddress
        {
            get { return innerChannel.RemoteAddress; }
        }

        protected override void OnAbort()
        {
            this.innerChannel.Abort();
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

        public void Send(Message message)
        {
            this.Send(message, this.DefaultSendTimeout);
        }

        public void Send(Message message, TimeSpan timeout)
        {
            Message reply = this.innerChannel.Request(message, timeout);
            ValidateReply(reply);
        }

        public IAsyncResult BeginSend(Message message, AsyncCallback callback, object state)
        {
            return innerChannel.BeginRequest(message, callback, state);
        }

        public IAsyncResult BeginSend(Message message, TimeSpan timeout, AsyncCallback callback, object state)
        {
            return innerChannel.BeginRequest(message, timeout, callback, state);
        }

        public void EndSend(IAsyncResult result)
        {
            Message reply = this.innerChannel.EndRequest(result);
            ValidateReply(reply);
        }

        void ValidateReply(Message reply)
        {
            if (reply != null)
            {
                object property;
                if (reply.Properties.TryGetValue(HttpResponseMessageProperty.Name, out property))
                {
                    HttpResponseMessageProperty responseProperty = property as HttpResponseMessageProperty;
                    if (responseProperty != null)
                    {
                        if (responseProperty.StatusCode != HttpStatusCode.OK)
                        {
                            // The exception here is expected, when the reply Http status code is 500 - internal server error
                            // we throw an "Acknowledgement failed" communication exception
                            throw new CommunicationException("Acknowledgement failed");
                        }
                    }
                }
            }
            else
            {
                throw new Exception("Received a null reply back!");
            }
        }
    }
}
