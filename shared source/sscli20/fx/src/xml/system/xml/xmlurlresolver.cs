//------------------------------------------------------------------------------
// <copyright file="XmlUrlResolver.cs" company="Microsoft">
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

namespace System.Xml
{
    using System;
    using System.IO;
    using System.Net;
    using System.Threading;
    using System.Security.Permissions;
    using System.Security.Policy;
    using System.Security;

    /// <include file='doc\XmlUrlResolver.uex' path='docs/doc[@for="XmlUrlResolver"]/*' />
    /// <devdoc>
    ///    <para>Resolves external XML resources named by a
    ///       Uniform Resource Identifier (URI).</para>
    /// </devdoc>
    public class XmlUrlResolver : XmlResolver {
        static object s_DownloadManager;
        ICredentials _credentials;

        static XmlDownloadManager DownloadManager {
            get {
                if ( s_DownloadManager == null ) {
                    object dm = new XmlDownloadManager();
                    Interlocked.CompareExchange( ref s_DownloadManager, dm, null );
                }
                return (XmlDownloadManager)s_DownloadManager;
            }
        }

        // Construction

        /// <include file='doc\XmlUrlResolver.uex' path='docs/doc[@for="XmlUrlResolver.XmlUrlResolver"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Creates a new instance of the XmlUrlResolver class.
        ///    </para>
        /// </devdoc>
        public XmlUrlResolver() {
        }

        //UE attension
        /// <include file='doc\XmlUrlResolver.uex' path='docs/doc[@for="XmlUrlResolver.Credentials"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public override ICredentials Credentials {
            set { _credentials = value; }
        }

        // Resource resolution

        /// <include file='doc\XmlUrlResolver.uex' path='docs/doc[@for="XmlUrlResolver.GetEntity"]/*' />
        /// <devdoc>
        ///    <para>Maps a
        ///       URI to an Object containing the actual resource.</para>
        /// </devdoc>
		public override Object GetEntity(Uri absoluteUri,
                                         string role,
                                         Type ofObjectToReturn) {
            if (ofObjectToReturn == null || ofObjectToReturn == typeof(System.IO.Stream)) {
                return DownloadManager.GetStream(absoluteUri, _credentials);
            }
            else {
                throw new XmlException(Res.Xml_UnsupportedClass, string.Empty);
            }
        }

        /// <include file='doc\XmlUrlResolver.uex' path='docs/doc[@for="XmlUrlResolver.ResolveUri"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [PermissionSetAttribute(SecurityAction.InheritanceDemand, Name = "FullTrust")]
        public override Uri ResolveUri(Uri baseUri, string relativeUri){
            return base.ResolveUri(baseUri, relativeUri);
        }
    }
}
