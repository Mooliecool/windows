//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

namespace Microsoft.ServiceModel.Samples
{
    using System;
    using System.Xml;
    using System.ServiceModel.Description;
    using System.Xml.Schema;
    using System.Collections.ObjectModel;
    using System.Collections.Generic;
    using System.Text;

    public class GZipMessageEncodingBindingElementImporter : IPolicyImportExtension
    {
        public GZipMessageEncodingBindingElementImporter()
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

            ICollection<XmlElement> assertions = context.GetBindingAssertions();
            foreach (XmlElement assertion in assertions)
            {
                if ((assertion.NamespaceURI == GZipMessageEncodingPolicyConstants.GZipEncodingNamespace) &&
                    (assertion.LocalName == GZipMessageEncodingPolicyConstants.GZipEncodingName)
                    )
                {
                    assertions.Remove(assertion);
                    context.BindingElements.Add(new GZipMessageEncodingBindingElement());
                    break;
                }
            }
        }
    }
}

