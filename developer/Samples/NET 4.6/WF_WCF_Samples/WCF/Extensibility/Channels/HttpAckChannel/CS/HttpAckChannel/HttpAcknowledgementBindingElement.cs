//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.ServiceModel.Channels;

namespace Microsoft.Samples.HttpAckChannel
{
    // this channel produces IInputChannel from HTTP's IReplyChannel
    // it will respond with status code 200 depending on the Receive Context outcome
    // note the OneWay Binding in the product always produces a 202
    public class HttpAcknowledgmentBindingElement : BindingElement
    {
        ReceiveContextSettings httpAcknowledgementSettings;

        public HttpAcknowledgmentBindingElement()
        {
            this.httpAcknowledgementSettings = new ReceiveContextSettings();
        }

        protected HttpAcknowledgmentBindingElement(HttpAcknowledgmentBindingElement elementToBeCloned)
        {
            this.httpAcknowledgementSettings = elementToBeCloned.httpAcknowledgementSettings;
        }

        public override bool CanBuildChannelListener<TChannel>(BindingContext context)
        {
            VerifyEncodingMessageVersion(context);
            if (typeof(IInputChannel) == typeof(TChannel) && this.httpAcknowledgementSettings.Enabled)
            {
                return context.CanBuildInnerChannelListener<IReplyChannel>();
            }
            else
            {
                return context.CanBuildInnerChannelListener<TChannel>();
            }
        }

        public override IChannelListener<TChannel> BuildChannelListener<TChannel>(BindingContext context)
        {
            if (!this.CanBuildChannelListener<TChannel>(context))
            {
                throw new ArgumentException(String.Format
                    ("Either the provided <{0}> is not of required IInputChannel shape or the underlying binding element cannot build an IReply channel shape", typeof(TChannel).Name));
            }
            return (IChannelListener<TChannel>)(object)new HttpAcknowledgementChannelListener(context, this.httpAcknowledgementSettings);
        }

        public override bool CanBuildChannelFactory<TChannel>(BindingContext context)
        {
            VerifyEncodingMessageVersion(context);
            if (typeof(IOutputChannel) == typeof(TChannel))
            {
                return context.CanBuildInnerChannelFactory<IRequestChannel>();
            }
            else
            {
                return context.CanBuildInnerChannelFactory<TChannel>();
            }
        }

        public override IChannelFactory<TChannel> BuildChannelFactory<TChannel>(BindingContext context)
        {
            if (!this.CanBuildChannelFactory<TChannel>(context))
            {
                throw new ArgumentException(String.Format
                    ("Either the {0} is not of required IOutputChannel shape or the underlying binding element cannot build an IRequest channel shape", typeof(TChannel).Name));
            }
            return (IChannelFactory<TChannel>)new HttpAcknowledgementChannelFactory(context);
        }
        
        public override BindingElement Clone()
        {
            return new HttpAcknowledgmentBindingElement(this);
        }

        public override T GetProperty<T>(BindingContext context)
        {
            if (typeof(T) == typeof(IReceiveContextSettings))
            {
                return (T)(object)this.httpAcknowledgementSettings;
            }

            return context.GetInnerProperty<T>();
        }

        void VerifyEncodingMessageVersion(BindingContext context)
        {
            if (context != null)
            {
                MessageEncodingBindingElement messageEncoderBindingElement
                    = context.Binding.Elements.Find<MessageEncodingBindingElement>();
                if (messageEncoderBindingElement != null)
                {
                    if (messageEncoderBindingElement.MessageVersion != MessageVersion.None)
                    {
                        throw new ArgumentException("The MessageVersion on the Message Encoding Binding Element must be set to MessageVersion.None for the HttpAcknowledgement binding element to function correctly");
                    }
                }
            }
        }
    }
}
