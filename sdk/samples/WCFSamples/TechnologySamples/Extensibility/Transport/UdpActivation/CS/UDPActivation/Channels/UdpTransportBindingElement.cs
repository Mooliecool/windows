// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

#region using
using System;
using System.ServiceModel.Description;
using System.ServiceModel.Channels;
using System.Globalization;
using System.ServiceModel;

using System.Xml;
using System.Collections.Generic;
using System.ServiceModel.Activation;
#endregion

namespace Microsoft.ServiceModel.Samples
{
    /// <summary>
    /// Udp Binding Element.  
    /// Used to configure and construct Udp ChannelFactories and ChannelListeners.
    /// </summary>
    public class UdpTransportBindingElement 
        : TransportBindingElement // to signal that we're a transport
        , IPolicyExportExtension // for policy export
    {
        bool multicast;

        public UdpTransportBindingElement()
        {
            this.multicast = UdpDefaults.Multicast;
        }

        protected UdpTransportBindingElement(UdpTransportBindingElement other)
            : base(other)
        {
            this.multicast = other.multicast;
        }

        public bool Multicast
        {
            get
            {
                return this.multicast;
            }

            set
            {
                this.multicast = value;
            }
        }

        public override IChannelFactory<TChannel> BuildChannelFactory<TChannel>(BindingContext context)
        {
            if (context == null)
            {
                throw new ArgumentNullException("context");
            }

            return (IChannelFactory<TChannel>)(object)new UdpChannelFactory(this, context);
        }

        public override IChannelListener<TChannel> BuildChannelListener<TChannel>(BindingContext context)
        {
            if (context == null)
            {
                throw new ArgumentNullException("context");
            }

            if (!this.CanBuildChannelListener<TChannel>(context))
            {
                throw new ArgumentException(string.Format(CultureInfo.CurrentCulture, "Unsupported channel type: {0}.", typeof(TChannel).Name));
            }

            UdpChannelListener channelListener = new UdpChannelListener(this, context);

            VirtualPathExtension virtualPathExtension = context.BindingParameters.Find<VirtualPathExtension>();
            if (virtualPathExtension != null)
            {
                channelListener.SetVirtualPath(virtualPathExtension.VirtualPath);
            }

            return (IChannelListener<TChannel>)(object)channelListener;
        }

        /// <summary>
        /// Used by higher layers to determine what types of channel factories this
        /// binding element supports. Which in this case is just IOutputChannel.
        /// </summary>
        public override bool CanBuildChannelFactory<TChannel>(BindingContext context)
        {
            return (typeof(TChannel) == typeof(IOutputChannel));
        }

        /// <summary>
        /// Used by higher layers to determine what types of channel listeners this
        /// binding element supports. Which in this case is just IInputChannel.
        /// </summary>
        public override bool CanBuildChannelListener<TChannel>(BindingContext context)
        {
            return (typeof(TChannel) == typeof(IInputChannel));
        }

        public override string Scheme
        {
            get
            {
                return UdpConstants.Scheme;
            }
        }

        // We expose in policy The fact that we're UDP, and whether we're multicast or not.
        // Import is done through UdpBindingElementImporter.
        void IPolicyExportExtension.ExportPolicy(MetadataExporter exporter, PolicyConversionContext context)
        {
            if (exporter == null)
            {
                throw new ArgumentNullException("exporter");
            }

            if (context == null)
            {
                throw new ArgumentNullException("context");
            }

            ICollection<XmlElement> bindingAssertions = context.GetBindingAssertions();
            XmlDocument xmlDocument = new XmlDocument();
            bindingAssertions.Add(xmlDocument.CreateElement(
                UdpPolicyStrings.Prefix, UdpPolicyStrings.TransportAssertion, UdpPolicyStrings.UdpNamespace));

            if (Multicast)
            {
                bindingAssertions.Add(xmlDocument.CreateElement(
                    UdpPolicyStrings.Prefix, UdpPolicyStrings.MulticastAssertion, UdpPolicyStrings.UdpNamespace));
            }

        }

        public override BindingElement Clone()
        {
            return new UdpTransportBindingElement(this);
        }

        public override T GetProperty<T>(BindingContext context)
        {
            if (context == null)
            {
                throw new ArgumentNullException("context");
            }

            return context.GetInnerProperty<T>();
        }
    }
}
