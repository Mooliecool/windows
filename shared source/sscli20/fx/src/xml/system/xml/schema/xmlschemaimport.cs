//------------------------------------------------------------------------------
// <copyright file="XmlSchemaImport.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaImport.uex' path='docs/doc[@for="XmlSchemaImport"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaImport : XmlSchemaExternal {
        string ns;
        XmlSchemaAnnotation annotation;
        
		/// <include file='doc\XmlSchemaImport.uex' path='docs/doc[@for="XmlSchemaImport.XmlSchemaImport"]/*' />
        public XmlSchemaImport() {
            Compositor = Compositor.Import;
        }

        /// <include file='doc\XmlSchemaImport.uex' path='docs/doc[@for="XmlSchemaImport.Namespace"]/*' />
        [XmlAttribute("namespace", DataType="anyURI")]
        public string Namespace {
            get { return ns; }
            set { ns = value; }
        }

        /// <include file='doc\XmlSchemaImport.uex' path='docs/doc[@for="XmlSchemaImport.Annotation"]/*' />
        [XmlElement("annotation", typeof(XmlSchemaAnnotation))]
        public XmlSchemaAnnotation Annotation {
            get { return annotation; }
            set { annotation = value; }
        }

        internal override void AddAnnotation(XmlSchemaAnnotation annotation) {
            this.annotation = annotation;
        }
    }
}
