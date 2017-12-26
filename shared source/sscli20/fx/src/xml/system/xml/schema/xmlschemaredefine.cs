//------------------------------------------------------------------------------
// <copyright file="XmlSchemaRedefine.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaRedefine.uex' path='docs/doc[@for="XmlSchemaRedefine"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaRedefine : XmlSchemaExternal {
        XmlSchemaObjectCollection items = new XmlSchemaObjectCollection();
        XmlSchemaObjectTable attributeGroups = new XmlSchemaObjectTable();
        XmlSchemaObjectTable types = new XmlSchemaObjectTable();
        XmlSchemaObjectTable groups = new XmlSchemaObjectTable();

        
		/// <include file='doc\XmlSchemaRedefine.uex' path='docs/doc[@for="XmlSchemaRedefine.XmlSchemaRedefine"]/*' />
		/// <devdoc>
		///    <para>[To be supplied.]</para>
		/// </devdoc>
        public XmlSchemaRedefine() {
            Compositor = Compositor.Redefine;
        }

        /// <include file='doc\XmlSchemaRedefine.uex' path='docs/doc[@for="XmlSchemaRedefine.Items"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("annotation", typeof(XmlSchemaAnnotation)),
         XmlElement("attributeGroup", typeof(XmlSchemaAttributeGroup)),
         XmlElement("complexType", typeof(XmlSchemaComplexType)),
         XmlElement("group", typeof(XmlSchemaGroup)),
         XmlElement("simpleType", typeof(XmlSchemaSimpleType))]
        public XmlSchemaObjectCollection Items {
            get { return items; }
        }

        /// <include file='doc\XmlSchemaRedefine.uex' path='docs/doc[@for="XmlSchemaRedefine.AttributeGroups"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public XmlSchemaObjectTable AttributeGroups {
            get { return attributeGroups; }
        }

        /// <include file='doc\XmlSchemaRedefine.uex' path='docs/doc[@for="XmlSchemaRedefine.SchemaTypes"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public XmlSchemaObjectTable SchemaTypes {
            get { return types; }
        }

        /// <include file='doc\XmlSchemaRedefine.uex' path='docs/doc[@for="XmlSchemaRedefine.Groups"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public XmlSchemaObjectTable Groups {
            get { return groups; }
        }

        internal override void AddAnnotation(XmlSchemaAnnotation annotation) {
            items.Add(annotation);
        }
    }
}
