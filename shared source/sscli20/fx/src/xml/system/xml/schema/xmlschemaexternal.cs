//------------------------------------------------------------------------------
// <copyright file="XmlSchemaExternal.cs" company="Microsoft">
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
    using System.ComponentModel;
    using System.Xml.Serialization;

    /// <include file='doc\XmlSchemaExternal.uex' path='docs/doc[@for="XmlSchemaExternal"]/*' />
    public abstract class XmlSchemaExternal : XmlSchemaObject {
        string location;
        Uri    baseUri;
        XmlSchema schema; 
        string id;
        XmlAttribute[] moreAttributes;
        Compositor compositor;

        /// <include file='doc\XmlSchemaExternal.uex' path='docs/doc[@for="XmlSchemaExternal.SchemaLocation"]/*' />
        [XmlAttribute("schemaLocation", DataType="anyURI")]
        public string SchemaLocation {
            get { return location; }
            set { location = value; }
        }

        /// <include file='doc\XmlSchemaExternal.uex' path='docs/doc[@for="XmlSchemaExternal.Schema"]/*' />
        [XmlIgnore]
        public XmlSchema Schema {
            get { return schema; }
            set { schema = value; }
        }

        /// <include file='doc\XmlSchemaExternal.uex' path='docs/doc[@for="XmlSchemaExternal.Id"]/*' />
        [XmlAttribute("id", DataType="ID")]
        public string Id {
            get { return id; }
            set { id = value; }
        }

        /// <include file='doc\XmlSchemaExternal.uex' path='docs/doc[@for="XmlSchemaExternal.UnhandledAttributes"]/*' />
        [XmlAnyAttribute]
        public XmlAttribute[] UnhandledAttributes {
            get { return moreAttributes; }
            set { moreAttributes = value; }
        }

        [XmlIgnore]
        internal Uri BaseUri {
            get { return baseUri; }
            set { baseUri = value; }
        }

        [XmlIgnore]
        internal override string IdAttribute {
            get { return Id; }
            set { Id = value; }
        }

        internal override void SetUnhandledAttributes(XmlAttribute[] moreAttributes) {
            this.moreAttributes = moreAttributes;
        }

        internal Compositor Compositor {
            get {
                return compositor;
            }
            set {
                compositor = value;
            }
        }
    }
}
