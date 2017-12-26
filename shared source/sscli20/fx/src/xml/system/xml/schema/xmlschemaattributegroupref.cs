//------------------------------------------------------------------------------
// <copyright file="XmlSchemaAttributeGroupRef.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaAttributeGroupRef.uex' path='docs/doc[@for="XmlSchemaAttributeGroupRef"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaAttributeGroupRef : XmlSchemaAnnotated {
        XmlQualifiedName refName = XmlQualifiedName.Empty; 

        /// <include file='doc\XmlSchemaAttributeGroupRef.uex' path='docs/doc[@for="XmlSchemaAttributeGroupRef.RefName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("ref")]
        public XmlQualifiedName RefName { 
            get { return refName; }
            set { refName = (value == null ? XmlQualifiedName.Empty : value); }
        }
    }

}
