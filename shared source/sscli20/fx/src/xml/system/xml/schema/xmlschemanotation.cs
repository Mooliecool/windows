//------------------------------------------------------------------------------
// <copyright file="XmlSchemaNotation.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaNotation.uex' path='docs/doc[@for="XmlSchemaNotation"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaNotation : XmlSchemaAnnotated {
        string name;        
        string publicId;
        string systemId;
        XmlQualifiedName qname = XmlQualifiedName.Empty; 
        
        /// <include file='doc\XmlSchemaNotation.uex' path='docs/doc[@for="XmlSchemaNotation.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name")]
        public string Name { 
            get { return name; }
            set { name = value; }
        }

        /// <include file='doc\XmlSchemaNotation.uex' path='docs/doc[@for="XmlSchemaNotation.Public"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("public")]
        public string Public {
            get { return publicId; }
            set { publicId = value; }
        }

        /// <include file='doc\XmlSchemaNotation.uex' path='docs/doc[@for="XmlSchemaNotation.System"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("system")]
        public string System {
            get { return systemId; }
            set { systemId = value; }
        }

        [XmlIgnore]
        internal XmlQualifiedName QualifiedName {
            get { return qname; }
            set { qname = value; }
        }

        [XmlIgnore]
        internal override string NameAttribute {
            get { return Name; }
            set { Name = value; }
        }
    }
}
