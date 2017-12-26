// ----------------------------------------------------------------------------
// Copyright (C) 2003-2005 Microsoft Corporation, All rights reserved.
// ----------------------------------------------------------------------------

#region using
using System;
using System.ServiceModel.Description;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Xml;
#endregion

namespace Microsoft.ServiceModel.Samples
{
    /// <summary>
    /// Policy import/export for Udp
    /// </summary>
    public class UdpBindingElementConverter : IPolicyImportExtension
    {
        public UdpBindingElementConverter()
        {
        }

        void IPolicyImportExtension.ImportPolicy(MetadataImporter importer, PolicyConversionContext context)
        {
            if (importer == null)
            {
                throw new ArgumentNullException("importer");
            }

            if (context == null)
            {
                throw new ArgumentNullException("context");
            }

            ICollection<XmlElement> bindingAssertions = context.GetBindingAssertions();
            List<XmlElement> processedAssertions = new List<XmlElement>();
            UdpTransportBindingElement udpBindingElement = null;
            bool multicast = false;
            foreach (XmlElement assertion in bindingAssertions)
            {
                if (assertion.NamespaceURI != UdpPolicyStrings.UdpNamespace)
                {
                    continue;
                }

                switch (assertion.LocalName)
                {
                    case UdpPolicyStrings.TransportAssertion:
                        udpBindingElement = new UdpTransportBindingElement();
                        break;
                    case UdpPolicyStrings.MulticastAssertion:
                        multicast = true;
                        break;
                    default:
                        continue;
                }

                processedAssertions.Add(assertion);
            }

            if (udpBindingElement != null)
            {
                udpBindingElement.Multicast = multicast;
                context.BindingElements.Add(udpBindingElement);
            }

            for (int i = 0; i < processedAssertions.Count; i++)
            {
                bindingAssertions.Remove(processedAssertions[i]);
            }
        }
    }
}
