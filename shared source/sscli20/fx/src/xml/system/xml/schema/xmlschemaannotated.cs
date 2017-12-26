//------------------------------------------------------------------------------
// <copyright file="XmlSchemaAnnotated.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaAnnotated.uex' path='docs/doc[@for="XmlSchemaAnnotated"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaAnnotated : XmlSchemaObject {
        string id;
        XmlSchemaAnnotation annotation;
        XmlAttribute[] moreAttributes;

        /// <include file='doc\XmlSchemaAnnotated.uex' path='docs/doc[@for="XmlSchemaAnnotated.Id"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("id", DataType="ID")]
        public string Id {
            get { return id; }
            set { id = value; }
        }

        /// <include file='doc\XmlSchemaAnnotated.uex' path='docs/doc[@for="XmlSchemaAnnotated.Annotation"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("annotation", typeof(XmlSchemaAnnotation))]
        public XmlSchemaAnnotation Annotation {
            get { return annotation; }
            set { annotation = value; }
        }

        /// <include file='doc\XmlSchemaAnnotated.uex' path='docs/doc[@for="XmlSchemaAnnotated.UnhandledAttributes"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAnyAttribute]
        public XmlAttribute[] UnhandledAttributes {
            get { return moreAttributes; }
            set { moreAttributes = value; }
        }

        [XmlIgnore]
        internal override string IdAttribute {
            get { return Id; }
            set { Id = value; }
        }

        internal override void SetUnhandledAttributes(XmlAttribute[] moreAttributes) {
            this.moreAttributes = moreAttributes;
        }
        internal override void AddAnnotation(XmlSchemaAnnotation annotation) {
            this.annotation = annotation;
        }
    }
}
