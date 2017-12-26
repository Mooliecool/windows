//------------------------------------------------------------------------------
// <copyright file="XmlSchemaAnnotation.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaAnnotation.uex' path='docs/doc[@for="XmlSchemaAnnotation"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaAnnotation : XmlSchemaObject {
        string id;
        XmlSchemaObjectCollection items = new XmlSchemaObjectCollection();
        XmlAttribute[] moreAttributes;

        /// <include file='doc\XmlSchemaAnnotation.uex' path='docs/doc[@for="XmlSchemaAnnotation.Id"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("id", DataType = "ID")]
        public string Id {
            get { return id; }
            set { id = value; }
        }

        /// <include file='doc\XmlSchemaAnnotation.uex' path='docs/doc[@for="XmlSchemaAnnotation.Items"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("documentation", typeof(XmlSchemaDocumentation)),
         XmlElement("appinfo", typeof(XmlSchemaAppInfo))]
        public XmlSchemaObjectCollection Items {
            get { return items; }
        }

        /// <include file='doc\XmlSchemaAnnotation.uex' path='docs/doc[@for="XmlSchemaAnnotation.UnhandledAttributes"]/*' />
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
    }
}
