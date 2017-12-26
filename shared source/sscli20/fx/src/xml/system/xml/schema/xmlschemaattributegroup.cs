//------------------------------------------------------------------------------
// <copyright file="XmlSchemaAttributeGroup.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaAttributeGroup.uex' path='docs/doc[@for="XmlSchemaAttributeGroup"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaAttributeGroup : XmlSchemaAnnotated {
        string name;        
        XmlSchemaObjectCollection attributes = new XmlSchemaObjectCollection();
        XmlSchemaAnyAttribute anyAttribute;
        XmlQualifiedName qname = XmlQualifiedName.Empty; 
        XmlSchemaAttributeGroup redefined;
        XmlSchemaObjectTable attributeUses;
        XmlSchemaAnyAttribute attributeWildcard;
        int selfReferenceCount;

        /// <include file='doc\XmlSchemaAttributeGroup.uex' path='docs/doc[@for="XmlSchemaAttributeGroup.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name")]
        public string Name { 
            get { return name; }
            set { name = value; }
        }

        /// <include file='doc\XmlSchemaAttributeGroup.uex' path='docs/doc[@for="XmlSchemaAttributeGroup.Attributes"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("attribute", typeof(XmlSchemaAttribute)),
         XmlElement("attributeGroup", typeof(XmlSchemaAttributeGroupRef))]
        public XmlSchemaObjectCollection Attributes {
            get { return attributes; }
        }

        /// <include file='doc\XmlSchemaAttributeGroup.uex' path='docs/doc[@for="XmlSchemaAttributeGroup.AnyAttribute"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("anyAttribute")]
        public XmlSchemaAnyAttribute AnyAttribute {
            get { return anyAttribute; }
            set { anyAttribute = value; }
        }

        [XmlIgnore]
        public XmlQualifiedName QualifiedName {
            get { return qname; }
        }

        [XmlIgnore]
        internal XmlSchemaObjectTable AttributeUses {
            get { 
                if (attributeUses == null) {
                    attributeUses = new XmlSchemaObjectTable();
                }
                return attributeUses; 
            }
        }

        [XmlIgnore]
        internal XmlSchemaAnyAttribute AttributeWildcard {
            get { return attributeWildcard; }
            set { attributeWildcard = value; }
        }

        /// <include file='doc\XmlSchemaAttributeGroup.uex' path='docs/doc[@for="XmlSchemaAttributeGroup.RedefinedAttributeGroup"]/*' />
        [XmlIgnore]
        public XmlSchemaAttributeGroup RedefinedAttributeGroup {
            get { return redefined; }
        }

        [XmlIgnore]
        internal XmlSchemaAttributeGroup Redefined {
            get { return redefined; }
            set { redefined = value; }
        }
        
        [XmlIgnore]
        internal int SelfReferenceCount {
            get { return selfReferenceCount; }
            set { selfReferenceCount = value; }
        }

        [XmlIgnore]
        internal override string NameAttribute {
            get { return Name; }
            set { Name = value; }
        }

        internal void SetQualifiedName(XmlQualifiedName value) { 
            qname = value;
        }

        internal override XmlSchemaObject Clone() {
            XmlSchemaAttributeGroup newGroup = (XmlSchemaAttributeGroup)MemberwiseClone();
            if (XmlSchemaComplexType.HasAttributeQNameRef(this.attributes)) { //If a ref/type name is present
                newGroup.attributes = XmlSchemaComplexType.CloneAttributes(this.attributes);
                
                //Clear compiled tables
                newGroup.attributeUses = null;
            }
            return newGroup;
        }
    }

}
