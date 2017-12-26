//------------------------------------------------------------------------------
// <copyright file="XmlAttributeTokenInfo.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------
namespace System.Xml {

    using System.Net;
    using System.Security;
    using System.Security.Policy;
    using System.Security.Permissions;

    [PermissionSetAttribute( SecurityAction.InheritanceDemand, Name = "FullTrust" )]
    public class XmlSecureResolver : XmlResolver {
        XmlResolver resolver;
        PermissionSet permissionSet;

        public XmlSecureResolver(XmlResolver resolver, string securityUrl) : this(resolver, CreateEvidenceForUrl(securityUrl)) {}

        public XmlSecureResolver(XmlResolver resolver, Evidence evidence) : this(resolver, SecurityManager.ResolvePolicy(evidence)) {}

        public XmlSecureResolver(XmlResolver resolver, PermissionSet permissionSet) {
            this.resolver = resolver;
            this.permissionSet = permissionSet;
        }

        public override ICredentials Credentials {
            set { resolver.Credentials = value; }
        }

        public override object GetEntity(Uri absoluteUri, string role, Type ofObjectToReturn) {
            permissionSet.PermitOnly();
            return resolver.GetEntity(absoluteUri, role, ofObjectToReturn);
        }

        public override Uri ResolveUri(Uri baseUri, string relativeUri) {
            return resolver.ResolveUri(baseUri, relativeUri);
        }

        public static Evidence CreateEvidenceForUrl(string securityUrl) {
            Evidence evidence = new Evidence();
            if (securityUrl != null && securityUrl.Length > 0) {
                evidence.AddHost(new Url(securityUrl));
                evidence.AddHost(Zone.CreateFromUrl(securityUrl));
                Uri uri = new Uri(securityUrl, UriKind.RelativeOrAbsolute);
                if (uri.IsAbsoluteUri && !uri.IsFile) {
                    evidence.AddHost(Site.CreateFromUrl(securityUrl));
                }
            }
            return evidence;
        }
    }
}
