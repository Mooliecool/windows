//------------------------------------------------------------------------------
// <copyright file="XmlSchemaGroupRef.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaGroupRef.uex' path='docs/doc[@for="XmlSchemaGroupRef"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaGroupRef : XmlSchemaParticle {
        XmlQualifiedName refName = XmlQualifiedName.Empty; 
        XmlSchemaGroupBase particle;
        XmlSchemaGroup refined;
        
        /// <include file='doc\XmlSchemaGroupRef.uex' path='docs/doc[@for="XmlSchemaGroupRef.RefName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("ref")]
        public XmlQualifiedName RefName { 
            get { return refName; }
            set { refName = (value == null ? XmlQualifiedName.Empty : value); }
        }

        /// <include file='doc\XmlSchemaGroupRef.uex' path='docs/doc[@for="XmlSchemaGroupRef.Particle"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public XmlSchemaGroupBase Particle {
            get { return particle; }
        }

        internal void SetParticle(XmlSchemaGroupBase value) {
             particle = value; 
        }

        [XmlIgnore]
        internal XmlSchemaGroup Redefined {
            get { return refined; }
            set { refined = value; }
        }
    }
}
