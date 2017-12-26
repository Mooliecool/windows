//------------------------------------------------------------------------------
// <copyright file="XmlSchemaAppInfo.cs" company="Microsoft">
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

    using System.Collections;
    using System.Xml.Serialization;

    /// <include file='doc\XmlSchemaAppInfo.uex' path='docs/doc[@for="XmlSchemaAppInfo"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaAppInfo : XmlSchemaObject {    
        string source;
        XmlNode[] markup;
        
        /// <include file='doc\XmlSchemaAppInfo.uex' path='docs/doc[@for="XmlSchemaAppInfo.Source"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("source", DataType="anyURI")]
        public string Source {
            get { return source; }
            set { source = value; }
        }

        /// <include file='doc\XmlSchemaAppInfo.uex' path='docs/doc[@for="XmlSchemaAppInfo.Markup"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlText(), XmlAnyElement]
        public XmlNode[] Markup {
            get { return markup; }
            set { markup = value; }
        }
    }
    
}
