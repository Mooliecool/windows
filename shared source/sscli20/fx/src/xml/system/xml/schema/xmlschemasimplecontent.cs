//------------------------------------------------------------------------------
// <copyright file="XmlSchemaSimpleContent.cs" company="Microsoft">
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
// <owner current="true" primary="true">priyal</owner>                                                                 
//------------------------------------------------------------------------------

namespace System.Xml.Schema {

    using System.Xml.Serialization;

    /// <include file='doc\XmlSchemaSimpleContent.uex' path='docs/doc[@for="XmlSchemaSimpleContent"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaSimpleContent : XmlSchemaContentModel {
        XmlSchemaContent content;

        /// <include file='doc\XmlSchemaSimpleContent.uex' path='docs/doc[@for="XmlSchemaSimpleContent.Content"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("restriction", typeof(XmlSchemaSimpleContentRestriction)),
         XmlElement("extension", typeof(XmlSchemaSimpleContentExtension))]
        public override XmlSchemaContent Content { 
            get { return content; }
            set { content = value; }
        }
    }
}
