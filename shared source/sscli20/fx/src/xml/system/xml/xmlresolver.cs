//------------------------------------------------------------------------------
// <copyright file="XmlResolver.cs" company="Microsoft">
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
    using System.Text;
    using System.Security;
    using System.Security.Policy;
    using System.Security.Permissions;

    /// <include file='doc\XmlResolver.uex' path='docs/doc[@for="XmlResolver"]/*' />
    /// <devdoc>
    ///    <para>Resolves external XML resources named by a Uniform
    ///       Resource Identifier (URI). This class is <see langword='abstract'/>
    ///       .</para>
    /// </devdoc>
    public abstract class XmlResolver {
        /// <include file='doc\XmlResolver.uex' path='docs/doc[@for="XmlResolver.GetEntity1"]/*' />
        /// <devdoc>
        ///    <para>Maps a
        ///       URI to an Object containing the actual resource.</para>
        /// </devdoc>

        public abstract Object GetEntity(Uri absoluteUri,
                                         string role,
                                         Type ofObjectToReturn);

        /// <include file='doc\XmlResolver.uex' path='docs/doc[@for="XmlResolver.ResolveUri"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public virtual Uri ResolveUri(Uri baseUri, string relativeUri) {
            if ( baseUri == null || ( !baseUri.IsAbsoluteUri && baseUri.OriginalString.Length == 0 ) ) {
                Uri uri = new Uri( relativeUri, UriKind.RelativeOrAbsolute );
                if ( !uri.IsAbsoluteUri && uri.OriginalString.Length > 0 ) {
                    uri = new Uri( Path.GetFullPath( relativeUri ) );
                }
                return uri;
            }
            else {
                if ( relativeUri == null || relativeUri.Length == 0 ) {
                    return baseUri;
                }
                return new Uri( baseUri, relativeUri );
            }
        }

        //UE attension
        /// <include file='doc\XmlResolver.uex' path='docs/doc[@for="XmlResolver.Credentials"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public abstract ICredentials Credentials {
            set;
        }
    }
}
