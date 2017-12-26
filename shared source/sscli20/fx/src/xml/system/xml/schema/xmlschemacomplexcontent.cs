//------------------------------------------------------------------------------
// <copyright file="XmlSchemaComplexContent.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaComplexContent.uex' path='docs/doc[@for="XmlSchemaComplexContent"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaComplexContent : XmlSchemaContentModel {
        XmlSchemaContent content;
        bool isMixed;
        bool hasMixedAttribute;

        /// <include file='doc\XmlSchemaComplexContent.uex' path='docs/doc[@for="XmlSchemaComplexContent.IsMixed"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("mixed")]
        public bool IsMixed {
            get { return isMixed; }
            set { isMixed = value; hasMixedAttribute = true; }
        }

        /// <include file='doc\XmlSchemaComplexContent.uex' path='docs/doc[@for="XmlSchemaComplexContent.Content"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("restriction", typeof(XmlSchemaComplexContentRestriction)),
         XmlElement("extension", typeof(XmlSchemaComplexContentExtension))]
        public override XmlSchemaContent Content { 
            get { return content; }
            set { content = value; }
        }

        [XmlIgnore]
        internal bool HasMixedAttribute {
            get { return hasMixedAttribute; }
        }

    }

}
